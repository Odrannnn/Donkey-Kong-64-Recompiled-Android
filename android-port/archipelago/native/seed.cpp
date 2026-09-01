// SPDX-License-Identifier: GPL-3.0-or-later
#include "seed.hpp"
#include <mbedtls/sha256.h>
#include <algorithm>
namespace dkap {
namespace {
uint32_t number(const Json& value, uint32_t maximum) {
    if (!value.is_number_integer() || (value.is_number_integer() && value.get<int64_t>() < 0)
        || value.get<uint64_t>() > maximum) throw Failure(Error::config);
    return value.get<uint32_t>();
}
std::string text(const Json& value) {
    if (!value.is_string()) throw Failure(Error::config);
    auto result = value.get<std::string>();
    if (result.empty() || result.size() > 256) throw Failure(Error::config);
    for (unsigned char c : result) if (c < 32 || c == 127) throw Failure(Error::config);
    return result;
}
Requirement requirement(const std::string& name) {
    static const std::map<std::string, Requirement> names{
        {"golden_bananas", Requirement::golden_bananas}, {"blueprints", Requirement::blueprints}, {"keys", Requirement::keys},
        {"fairies", Requirement::fairies}, {"crowns", Requirement::crowns}, {"medals", Requirement::medals},
        {"pearls", Requirement::pearls}, {"rainbow_coins", Requirement::rainbow_coins}, {"bean", Requirement::bean},
        {"nintendo_coin", Requirement::nintendo_coin}, {"rareware_coin", Requirement::rareware_coin},
        {"kongs", Requirement::kongs}, {"moves", Requirement::moves}};
    auto found = names.find(name);
    if (found == names.end()) throw Failure(Error::unsupported);
    return found->second;
}
}
CampaignSeed CampaignSeed::parse(const Json& source) {
    try {
        if (source.dump().size() > message_limit) throw Failure(Error::config);
        static const std::set<std::string> fields{"format", "world_version", "revision", "game", "seed_name", "player",
            "team", "slot", "locations", "starting_inventory", "blockers", "boss_bananas", "shops", "edits"};
        if (!source.is_object() || source.size() < fields.size() - 1 || source.size() > fields.size()) throw Failure(Error::config);
        for (const auto& field : source.items()) if (!fields.contains(field.key())) throw Failure(Error::config);
        if (number(source.at("format"), 1) != 1 || source.at("world_version") != world_version
            || source.at("revision") != "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4"
            || source.at("game") != "Donkey Kong 64") throw Failure(Error::unsupported);
        CampaignSeed result;
        result.seed_name = text(source.at("seed_name")); result.player = text(source.at("player"));
        result.team = number(source.at("team"), 65535); result.slot = number(source.at("slot"), 65535);
        if (!result.slot) throw Failure(Error::config);
        const auto& locations = source.at("locations");
        if (!locations.is_array() || locations.empty() || locations.size() > AP_LOCATION_COUNT) throw Failure(Error::config);
        for (const auto& location : locations) result.locations.push_back(number(location, UINT32_MAX));
        CampaignChecks validate_locations(result.locations);
        const auto& items = source.at("starting_inventory");
        if (!items.is_array() || items.size() > item_limit) throw Failure(Error::config);
        for (const auto& item : items) result.starting_inventory.push_back(Item{number(item, UINT32_MAX), -2, result.slot, 0});
        if (!CampaignCatalog::builtin().inventory(result.starting_inventory).unsupported.empty()) throw Failure(Error::unsupported);
        const auto& blockers = source.at("blockers");
        const auto& bosses = source.at("boss_bananas");
        if (!blockers.is_array() || blockers.size() != 8 || !bosses.is_array() || bosses.size() != 7) throw Failure(Error::config);
        for (size_t i = 0; i < 8; ++i) result.blockers[i] = {requirement(text(blockers[i].at("item"))), number(blockers[i].at("count"), 65535)};
        for (size_t i = 0; i < 7; ++i) result.boss_bananas[i] = uint16_t(number(bosses[i], 500));
        // These are required regardless of what an input manifest claims. A catalog is not a gameplay port.
        result.required_hooks = {"reward_replacement", "separate_location_inventory", "seed_save_transaction", "starting_state",
            "move_checks", "blockers", "bosses", "goal"};
        for (auto id : result.locations) {
            const auto& check = ap_locations[ap_location_index(uint32_t(id))];
            if (check.detector == AP_DETECT_SHOP) result.required_hooks.insert("shops");
            else if (check.detector == AP_DETECT_ENEMY) result.required_hooks.insert("enemy_drops");
            else if (check.detector == AP_DETECT_EVENT) result.required_hooks.insert("event_locations");
        }
        std::set<uint32_t> expected_shops;
        for (auto id : result.locations) if (ap_locations[ap_location_index(uint32_t(id))].detector == AP_DETECT_SHOP)
            expected_shops.insert(uint32_t(id));
        if (source.contains("shops")) {
            const auto& shops = source.at("shops");
            if (!shops.is_array() || shops.size() > AP_SHOP_COUNT) throw Failure(Error::config);
            for (const auto& shop : shops) {
                auto location = number(shop.at("location"), UINT32_MAX);
                auto price = number(shop.at("price"), 255);
                if (!shop.is_object() || shop.size() != 2 || !expected_shops.erase(location)) throw Failure(Error::config);
                result.shops.push_back({location, uint16_t(price), 0});
            }
        }
        // A missing threshold must not silently turn a paid AP check into a free one.
        if (!expected_shops.empty()) throw Failure(Error::config);
        const auto& edits = source.at("edits");
        if (!edits.is_object() || edits.size() > 128) throw Failure(Error::config);
        result.edits = edits;
        for (const auto& edit : edits.items()) {
            // Preserve every option as a capability requirement, even false/empty entries.
            // The generator must omit options that cause no edits, not let the client guess.
            result.required_hooks.insert("edit:" + text(Json(edit.key())));
        }
        auto canonical = source.dump(-1, ' ', true);
        unsigned char digest[32];
        if (mbedtls_sha256(reinterpret_cast<const unsigned char*>(canonical.data()), canonical.size(), digest, 0))
            throw Failure(Error::config);
        std::copy(digest, digest + result.binding.size(), result.binding.begin());
        return result;
    } catch (const Json::exception&) { throw Failure(Error::config); }
}
std::string CampaignSeed::binding_hex() const {
    static constexpr char hex[] = "0123456789abcdef";
    std::string result;
    result.reserve(binding.size() * 2);
    for (auto byte : binding) { result += hex[byte >> 4]; result += hex[byte & 15]; }
    return result;
}
void CampaignSeed::require_hooks(const std::set<std::string>& implemented_hooks) const {
    for (const auto& hook : required_hooks) if (!implemented_hooks.contains(hook)) throw Failure(Error::unsupported);
}
bool CampaignSeed::blocker_open(unsigned level, const CampaignInventory& inventory) const {
    return level < blockers.size() && meets(inventory, blockers[level].kind, blockers[level].count);
}
bool CampaignSeed::boss_door_open(unsigned level, uint32_t bananas) const {
    return level < boss_bananas.size() && bananas >= boss_bananas[level];
}
}
