// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_protocol.hpp"
#include <algorithm>
#include <cstring>

namespace dkap {
namespace {
inline constexpr const char* campaign_game = "Donkey Kong 64";

int64_t integer(const Json& value, int64_t low, int64_t high) {
    if (!value.is_number_integer() || (value.is_number_unsigned() && value.get<uint64_t>() > uint64_t(high)))
        throw Failure(Error::protocol);
    auto number = value.get<int64_t>();
    if (number < low || number > high) throw Failure(Error::protocol);
    return number;
}

Item received_item(const Json& value) {
    Json array = value.is_object()
        ? Json::array({value.at("item"), value.at("location"), value.at("player"), value.at("flags")}) : value;
    if (!array.is_array() || array.size() != 4) throw Failure(Error::protocol);
    return {integer(array[0], 0, UINT32_MAX), integer(array[1], INT64_MIN, INT64_MAX),
        integer(array[2], 0, 65535), integer(array[3], 0, 7)};
}

ap_check_set location_set(const Json& values, const ap_check_set& selected) {
    if (!values.is_array() || values.size() > AP_LOCATION_COUNT) throw Failure(Error::protocol);
    ap_check_set result{};
    for (const auto& value : values) {
        auto id = uint32_t(integer(value, 0, UINT32_MAX));
        if (!ap_check_has(&selected, id) || ap_check_has(&result, id) || !ap_check_add(&result, id))
            throw Failure(Error::protocol);
    }
    return result;
}

Json check_command(const ap_check_set& observed, const ap_check_set& confirmed) {
    Json locations = Json::array();
    for (unsigned i = 0; i < AP_LOCATION_COUNT; ++i)
        if (ap_check_has_index(&observed, i) && !ap_check_has_index(&confirmed, i))
            locations.push_back(ap_locations[i].id);
    return Json{{"cmd", "LocationChecks"}, {"locations", std::move(locations)}};
}

bool empty_checks(const Json& command) { return command.at("locations").empty(); }
}

CampaignProtocol::CampaignProtocol(const Config& config, const CampaignSeed& seed, CampaignChannel& channel)
    : config_(config), seed_(seed), channel_(channel) {
    validate_config(config_);
    if (config_.name != seed_.player || config_.seed != seed_.seed_name
        || config_.team != seed_.team || config_.slot != seed_.slot) throw Failure(Error::config);
    for (auto id : seed_.locations)
        if (id < 0 || id > UINT32_MAX || !ap_check_add(&selected_, uint32_t(id))) throw Failure(Error::config);
}

void CampaignProtocol::disconnect() {
    room_ = connected_ = synced_ = false;
    confirmed_ = {};
}

void CampaignProtocol::observe(const ap_check_set& observed) {
    if (!ap_check_valid(&observed)) throw Failure(Error::protocol);
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i)
        if (observed.words[i] & ~selected_.words[i]) throw Failure(Error::protocol);
    channel_.commit_record(observed);
}

std::vector<Json> CampaignProtocol::receive(const std::string& text) {
    auto batch = parse_bounded(text);
    if (!batch.is_array() || batch.size() > 256) throw Failure(Error::protocol);
    std::vector<Json> commands;
    try {
        for (const auto& message : batch) {
            if (!message.is_object() || !message.at("cmd").is_string()) throw Failure(Error::protocol);
            auto cmd = message.at("cmd").get<std::string>();
            if (cmd == "RoomInfo") {
                if (room_) throw Failure(Error::protocol);
                const auto& games = message.at("games");
                if (message.at("seed_name") != seed_.seed_name || !games.is_array()
                    || std::find(games.begin(), games.end(), Json(campaign_game)) == games.end())
                    throw Failure(Error::seed);
                room_ = true;
                commands.push_back({{"cmd", "Connect"}, {"game", campaign_game}, {"name", seed_.player},
                    {"password", config_.password.empty() ? Json(nullptr) : Json(config_.password)},
                    {"uuid", "dk64recomp-campaign-" + seed_.binding_hex()}, {"items_handling", 1},
                    {"tags", Json::array({"AP"})},
                    {"version", {{"major", 0}, {"minor", 6}, {"build", 5}, {"class", "Version"}}}});
            } else if (cmd == "Connected") {
                if (!room_ || connected_) throw Failure(Error::protocol);
                if (integer(message.at("team"), 0, 65535) != seed_.team
                    || integer(message.at("slot"), 1, 65535) != seed_.slot) throw Failure(Error::slot);
                const auto& slot_data = message.at("slot_data");
                if (!slot_data.is_object() || !slot_data.contains("Version")
                    || slot_data.at("Version") != world_version) throw Failure(Error::unsupported);
                auto checked = location_set(message.at("checked_locations"), selected_);
                auto missing = location_set(message.at("missing_locations"), selected_);
                for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i)
                    if ((checked.words[i] & missing.words[i])
                        || (checked.words[i] | missing.words[i]) != selected_.words[i])
                        throw Failure(Error::unsupported);
                confirmed_ = checked;
                synced_ = channel_.snapshot().receipt_count == 0;
                connected_ = true;
                commands.push_back({{"cmd", "Sync"}});
            } else if (cmd == "ReceivedItems") {
                if (!connected_) throw Failure(Error::protocol);
                auto index = size_t(integer(message.at("index"), 0, item_limit));
                const auto& values = message.at("items");
                if (!values.is_array() || values.size() > item_limit - index) throw Failure(Error::protocol);
                std::vector<Item> incoming;
                incoming.reserve(values.size());
                for (const auto& value : values) incoming.push_back(received_item(value));
                auto durable = channel_.snapshot();
                if (index > durable.receipt_count || (!synced_ && index != 0)) {
                    synced_ = false;
                    commands.push_back({{"cmd", "Sync"}});
                    auto checks = check_command(durable.observed, confirmed_);
                    if (!empty_checks(checks)) commands.push_back(std::move(checks));
                    continue;
                }
                if (index == 0 && incoming.size() < durable.receipt_count) throw Failure(Error::protocol);
                channel_.commit_receive(index, incoming);
                synced_ = true;
            } else if (cmd == "RoomUpdate") {
                if (!connected_) throw Failure(Error::protocol);
                if (message.contains("checked_locations")) {
                    auto checked = location_set(message.at("checked_locations"), selected_);
                    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i) confirmed_.words[i] |= checked.words[i];
                }
            } else if (cmd == "ConnectionRefused") throw Failure(Error::slot);
            else if (cmd == "InvalidPacket") throw Failure(Error::protocol);
        }
    } catch (const Json::exception&) { throw Failure(Error::protocol); }
    return commands;
}

std::vector<Json> CampaignProtocol::flush() const {
    if (!ready()) return {};
    auto durable = channel_.snapshot();
    auto command = check_command(durable.observed, confirmed_);
    if (empty_checks(command)) return {};
    return {std::move(command)};
}
}
