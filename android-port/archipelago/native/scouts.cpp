// SPDX-License-Identifier: GPL-3.0-or-later
#include "scouts.hpp"
#include "presentation.hpp"
#include <algorithm>
#include <charconv>
#include <limits>

namespace dkap {
namespace {
constexpr size_t max_slots = 4096, max_games = 128, max_names = 32768, max_name_bytes = 512;
int64_t number(const Json& value, int64_t low = 0, int64_t high = INT64_MAX) {
    if (!value.is_number_integer() || (value.is_number_unsigned() && value.get<uint64_t>() > uint64_t(high)))
        throw Failure(Error::protocol);
    auto result = value.get<int64_t>();
    if (result < low || result > high) throw Failure(Error::protocol);
    return result;
}
std::string text_value(const Json& value, bool empty = false) {
    if (!value.is_string()) throw Failure(Error::protocol);
    auto result = value.get<std::string>();
    if ((!empty && result.empty()) || result.size() > max_name_bytes
        || std::any_of(result.begin(), result.end(), [](unsigned char c) { return c < 32 || c == 127; }))
        throw Failure(Error::protocol);
    return result;
}
uint32_t object_slot(const std::string& key) {
    uint32_t result = 0;
    auto parsed = std::from_chars(key.data(), key.data() + key.size(), result);
    if (key.empty() || parsed.ec != std::errc() || parsed.ptr != key.data() + key.size() || result == 0 || result > 65535)
        throw Failure(Error::protocol);
    return result;
}
bool hex_checksum(const std::string& value) {
    return value.size() == 40 && std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    });
}
Json field(const Json& value, const char* name, size_t index) {
    if (value.is_object()) return value.at(name);
    if (value.is_array() && value.size() > index) return value.at(index);
    throw Failure(Error::protocol);
}
}

Scouts::Scouts(std::vector<int64_t> selected) : selected_(std::move(selected)) {
    if (selected_.empty() || selected_.size() > AP_LOCATION_COUNT) throw Failure(Error::unsupported);
    std::sort(selected_.begin(), selected_.end());
    if (std::adjacent_find(selected_.begin(), selected_.end()) != selected_.end()) throw Failure(Error::unsupported);
    for (auto id : selected_) if (id < 0 || id > UINT32_MAX || ap_location_index(uint32_t(id)) < 0) throw Failure(Error::unsupported);
}
void Scouts::disconnect() {
    team_ = 0; room_metadata_ = enabled_ = false;
    checksums_.clear(); slots_.clear(); aliases_.clear(); placements_.clear(); item_names_.clear(); requested_games_.clear();
}
void Scouts::room(const Json& message) {
    checksums_.clear(); room_metadata_ = false;
    if (!message.contains("datapackage_checksums")) return; // AP pre-0.4/minimal test fixtures.
    const auto& values = message.at("datapackage_checksums");
    if (!values.is_object() || values.empty() || values.size() > max_games) throw Failure(Error::protocol);
    std::map<std::string, std::string> parsed;
    for (auto it = values.begin(); it != values.end(); ++it) {
        auto name = text_value(Json(it.key()));
        auto checksum = text_value(it.value());
        if (!hex_checksum(checksum) || !parsed.emplace(std::move(name), std::move(checksum)).second) throw Failure(Error::protocol);
    }
    checksums_ = std::move(parsed); room_metadata_ = true;
}
std::vector<Json> Scouts::connected(const Json& message, uint32_t team, uint32_t slot) {
    (void)slot;
    bool has_players = message.contains("players"), has_slots = message.contains("slot_info");
    if (!room_metadata_ || (!has_players && !has_slots)) return {};
    if (!has_players || !has_slots) throw Failure(Error::protocol);
    const auto& slot_info = message.at("slot_info");
    const auto& players = message.at("players");
    if (!slot_info.is_object() || slot_info.empty() || slot_info.size() > max_slots
        || !players.is_array() || players.size() > max_slots) throw Failure(Error::protocol);
    std::map<uint32_t, Slot> parsed_slots;
    for (auto it = slot_info.begin(); it != slot_info.end(); ++it) {
        uint32_t id = object_slot(it.key());
        Slot entry{text_value(field(it.value(), "game", 1)), text_value(field(it.value(), "name", 0)),
                   uint32_t(number(field(it.value(), "type", 2), 0, 3))};
        if (!checksums_.contains(entry.game) || !parsed_slots.emplace(id, std::move(entry)).second) throw Failure(Error::protocol);
    }
    std::map<uint32_t, std::string> parsed_aliases;
    for (const auto& value : players) {
        auto player_team = uint32_t(number(field(value, "team", 0), 0, 65535));
        auto player_slot = uint32_t(number(field(value, "slot", 1), 1, 65535));
        auto alias = text_value(field(value, "alias", 2));
        (void)text_value(field(value, "name", 3));
        if (player_team == team && !parsed_aliases.emplace(player_slot, std::move(alias)).second) throw Failure(Error::protocol);
    }
    if (!parsed_slots.contains(slot) || !parsed_aliases.contains(slot)) throw Failure(Error::protocol);
    team_ = team; slots_ = std::move(parsed_slots); aliases_ = std::move(parsed_aliases);
    placements_.clear(); item_names_.clear(); requested_games_.clear(); enabled_ = true;
    return {{{"cmd", "LocationScouts"}, {"locations", selected_}, {"create_as_hint", 0}}};
}
std::vector<Json> Scouts::location_info(const Json& message) {
    if (!enabled_) return {};
    const auto& values = message.at("locations");
    if (!values.is_array() || values.size() != selected_.size()) throw Failure(Error::protocol);
    std::map<int64_t, Placement> parsed;
    for (const auto& value : values) {
        auto location = number(field(value, "location", 1));
        Placement placement{number(field(value, "item", 0), INT64_MIN),
            uint32_t(number(field(value, "player", 2), 1, 65535)), uint32_t(number(field(value, "flags", 3), 0, 7))};
        if (!std::binary_search(selected_.begin(), selected_.end(), location) || !slots_.contains(placement.recipient)
            || !aliases_.contains(placement.recipient) || !parsed.emplace(location, placement).second) throw Failure(Error::protocol);
    }
    if (!placements_.empty() && placements_ != parsed) throw Failure(Error::protocol);
    placements_ = std::move(parsed);
    std::set<std::string> needed;
    for (const auto& [location, placement] : placements_) {
        (void)location;
        const auto& target_game = slots_.at(placement.recipient).game;
        if (!item_names_.contains(target_game) && !requested_games_.contains(target_game)) needed.insert(target_game);
    }
    if (needed.empty()) return {};
    requested_games_.insert(needed.begin(), needed.end());
    return {{{"cmd", "GetDataPackage"}, {"games", needed}}};
}
void Scouts::data_package(const Json& message) {
    if (!enabled_) return;
    const auto& data = message.at("data");
    const auto& games = data.at("games");
    if (!data.is_object() || !games.is_object() || games.empty() || games.size() > max_games) throw Failure(Error::protocol);
    auto updated = item_names_;
    for (auto game_it = games.begin(); game_it != games.end(); ++game_it) {
        const std::string game_name = game_it.key();
        if (!requested_games_.contains(game_name) || !checksums_.contains(game_name) || updated.contains(game_name))
            throw Failure(Error::protocol);
        const auto& package = game_it.value();
        auto checksum = text_value(package.at("checksum"));
        const auto& names = package.at("item_name_to_id");
        if (checksum != checksums_.at(game_name) || !names.is_object() || names.size() > max_names) throw Failure(Error::protocol);
        std::set<int64_t> needed;
        for (const auto& [location, placement] : placements_) {
            (void)location;
            if (slots_.at(placement.recipient).game == game_name) needed.insert(placement.item);
        }
        std::map<int64_t, std::string> resolved;
        for (auto name_it = names.begin(); name_it != names.end(); ++name_it) {
            auto name = text_value(Json(name_it.key()));
            auto id = number(name_it.value(), INT64_MIN);
            if (needed.contains(id) && !resolved.emplace(id, std::move(name)).second) throw Failure(Error::protocol);
        }
        for (auto id : needed) if (!resolved.contains(id)) throw Failure(Error::protocol);
        updated.emplace(game_name, std::move(resolved));
    }
    item_names_ = std::move(updated);
}
void Scouts::room_update(const Json& message) {
    if (!enabled_ || !message.contains("players")) return;
    const auto& players = message.at("players");
    if (!players.is_array() || players.size() > max_slots) throw Failure(Error::protocol);
    auto updated = aliases_;
    std::set<uint32_t> seen;
    for (const auto& value : players) {
        auto player_team = uint32_t(number(field(value, "team", 0), 0, 65535));
        auto player_slot = uint32_t(number(field(value, "slot", 1), 1, 65535));
        auto alias = text_value(field(value, "alias", 2));
        (void)text_value(field(value, "name", 3));
        if (player_team == team_) {
            if (!slots_.contains(player_slot) || !seen.insert(player_slot).second) throw Failure(Error::protocol);
            updated[player_slot] = std::move(alias);
        }
    }
    aliases_ = std::move(updated);
}
ScoutSnapshot Scouts::build_snapshot() const {
    ScoutSnapshot result; result.enabled = enabled_;
    if (!enabled_) return result;
    for (const auto& [location, placement] : placements_) {
        const auto& slot = slots_.at(placement.recipient);
        auto game_names = item_names_.find(slot.game);
        if (game_names == item_names_.end()) continue;
        auto item_name = game_names->second.find(placement.item);
        if (item_name == game_names->second.end()) continue;
        ScoutRecord record{location, placement.item, placement.recipient, placement.flags, slot.game,
                           item_name->second, aliases_.at(placement.recipient)};
        result.locations.push_back(record);
        int index = location >= 0 && location <= UINT32_MAX ? ap_location_index(uint32_t(location)) : -1;
        if (index >= 0 && ap_locations[index].detector == AP_DETECT_SHOP)
            result.shop_labels.push_back(make_shop_label(uint32_t(location), record.item_name, record.recipient_name));
    }
    return result;
}
}
