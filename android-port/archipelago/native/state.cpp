// SPDX-License-Identifier: GPL-3.0-or-later
#include "state.hpp"
#include "campaign.hpp"
#include "scouts.hpp"
#include <mbedtls/sha256.h>
#include <ixwebsocket/IXUrlParser.h>
#include <algorithm>

namespace dkap {
namespace {
int64_t integer(const Json& value, int64_t low = 0, int64_t high = INT64_MAX) {
    if (!value.is_number_integer() || (value.is_number_unsigned() && value.get<uint64_t>() > uint64_t(high)))
        throw Failure(Error::protocol);
    auto number = value.get<int64_t>();
    if (number < low || number > high) throw Failure(Error::protocol);
    return number;
}
Item item_from(const Json& value) {
    Json array = value.is_object() ? Json::array({value.at("item"), value.at("location"), value.at("player"), value.at("flags")}) : value;
    if (!array.is_array() || array.size() != 4) throw Failure(Error::protocol);
    Item result{integer(array[0]), integer(array[1], INT64_MIN), integer(array[2], 0, 65535), integer(array[3], 0, 7)};
    if (std::find(item_ids.begin(), item_ids.end(), result[0]) == item_ids.end()) throw Failure(Error::unsupported);
    return result;
}
uint32_t locations(const Json& values) {
    if (!values.is_array() || values.size() > location_ids.size()) throw Failure(Error::unsupported);
    uint32_t mask = 0;
    for (const auto& value : values) {
        auto found = std::find(location_ids.begin(), location_ids.end(), integer(value));
        if (found == location_ids.end()) throw Failure(Error::unsupported);
        mask |= 1u << (found - location_ids.begin());
    }
    return mask;
}
Json check_command(uint32_t mask) {
    Json ids = Json::array();
    for (size_t i = 0; i < location_ids.size(); ++i) if (mask & (1u << i)) ids.push_back(location_ids[i]);
    return Json{{"cmd", "LocationChecks"}, {"locations", ids}};
}
bool valid_text(const std::string& value, size_t limit, bool empty = false) {
    return (empty || !value.empty()) && value.size() <= limit
        && std::none_of(value.begin(), value.end(), [](unsigned char c) { return c < 32 || c == 127; });
}
}
void validate_config(const Config& c) {
    if (!valid_text(c.name, 256) || !valid_text(c.seed, 256) || !valid_text(c.password, 256, true)
        || c.team > 65535 || c.slot == 0 || c.slot > 65535 || !valid_text(c.server, 1024)
        || c.server.find_first_of(" \t\r\n@?#") != std::string::npos) throw Failure(Error::config);
    std::string scheme, host, path, query;
    int port = 0;
    if (!ix::UrlParser::parse(c.server, scheme, host, path, query, port)
        || (scheme != "ws" && scheme != "wss") || host.empty() || port < 1 || port > 65535 || !query.empty())
        throw Failure(Error::config);
    // IX falls back to the scheme's default for an out-of-range explicit port.
    auto authority_start = c.server.find("://") + 3;
    auto authority = c.server.substr(authority_start, c.server.find('/', authority_start) - authority_start);
    auto colon = authority.front() == '[' ? authority.find(':', authority.find(']')) : authority.find(':');
    if (colon != std::string::npos) {
        auto digits = authority.substr(colon + 1);
        if (digits.empty() || digits.size() > 5 || digits.find_first_not_of("0123456789") != std::string::npos
            || std::stoul(digits) != uint32_t(port)) throw Failure(Error::config);
    }
    try { (void)binding(c).dump(-1, ' ', true); (void)Json(c.password).dump(); }
    catch (const Json::exception&) { throw Failure(Error::config); }
}
Json binding(const Config& c) {
    return {{"seed", c.seed}, {"name", c.name}, {"team", c.team}, {"slot", c.slot},
            {"profile", profile}, {"world_version", world_version}};
}
std::string identity_for(const Config& c) {
    validate_config(c);
    auto input = binding(c).dump(-1, ' ', true); // Same canonical binding as version 0.1.
    unsigned char hash[32];
    if (mbedtls_sha256(reinterpret_cast<const unsigned char*>(input.data()), input.size(), hash, 0)) throw Failure(Error::config);
    const char* hex = "0123456789abcdef";
    std::string result;
    for (size_t i = 0; i < 16; ++i) { result += hex[hash[i] >> 4]; result += hex[hash[i] & 15]; }
    return result;
}
Json parse_bounded(const std::string& text) {
    if (text.size() > message_limit) throw Failure(Error::protocol);
    try {
        return Json::parse(text, [](int depth, Json::parse_event_t, Json&) {
            if (depth > 32) throw Failure(Error::protocol);
            return true;
        });
    } catch (const Json::exception&) { throw Failure(Error::protocol); }
}
State::State(const Config& config, const std::filesystem::path& path)
    : config_(config), identity_(identity_for(config)), journal_(path),
      scouts_(std::make_unique<Scouts>(std::vector<int64_t>(location_ids.begin(), location_ids.end()))) {
    try {
        auto saved = journal_.load();
        if (saved.is_null()) { persist({}, 0); return; }
        if (saved.at("format") != 2 || saved.at("binding") != binding(config_)
            || !saved.at("items").is_array() || saved.at("items").size() > item_limit) throw Failure(Error::storage);
        checks_ = uint32_t(integer(saved.at("checks"), 0, check_mask));
        for (const auto& value : saved.at("items")) items_.push_back(item_from(value));
    } catch (...) { throw Failure(Error::storage); }
}
State::~State() = default;
void State::persist(const std::vector<Item>& items, uint32_t checks) {
    journal_.commit(Json{{"format", 2}, {"binding", binding(config_)}, {"items", items}, {"checks", checks}});
}
void State::disconnect() { room_ = connected_ = synced_ = goal_sent_ = false; confirmed_ = 0; scouts_->disconnect(); }
ScoutSnapshot State::scout_snapshot() const { return scouts_->snapshot(); }
void State::record_checks(uint32_t checks) {
    if (checks & ~check_mask) throw Failure(Error::protocol);
    if ((checks | checks_) != checks_) { persist(items_, checks | checks_); checks_ |= checks; }
}
uint32_t State::moves() const {
    uint32_t result = 0;
    if (ready()) {
        const auto inventory = CampaignCatalog::builtin().inventory(items_);
        for (unsigned kong = 0; kong < 5; ++kong) if (owns_move(inventory, kong, 1)) result |= 1u << kong;
    }
    return result;
}
std::vector<Json> State::receive(const std::string& text) {
    std::vector<Json> commands;
    auto batch = parse_bounded(text);
    if (!batch.is_array() || batch.size() > 256) throw Failure(Error::protocol);
    try {
        for (const auto& message : batch) {
            if (!message.is_object() || !message.at("cmd").is_string()) throw Failure(Error::protocol);
            auto cmd = message.at("cmd").get<std::string>();
            if (cmd == "RoomInfo") {
                if (room_) throw Failure(Error::protocol);
                const auto& games = message.at("games");
                if (message.at("seed_name") != config_.seed || !games.is_array()
                    || std::find(games.begin(), games.end(), Json(game)) == games.end()) throw Failure(Error::seed);
                scouts_->room(message);
                room_ = true;
                commands.push_back({{"cmd", "Connect"}, {"game", game}, {"name", config_.name},
                    {"password", config_.password.empty() ? Json(nullptr) : Json(config_.password)},
                    {"uuid", "dk64recomp-" + identity_}, {"items_handling", 7}, {"tags", Json::array({"AP"})}, {"slot_data", true},
                    {"version", {{"major", 0}, {"minor", 6}, {"build", 5}, {"class", "Version"}}}});
            } else if (cmd == "Connected") {
                if (!room_ || connected_) throw Failure(Error::protocol);
                const auto& data = message.at("slot_data");
                if (integer(message.at("team"), 0, 65535) != config_.team
                    || integer(message.at("slot"), 1, 65535) != config_.slot
                    || data.at("recomp_profile") != profile || data.at("Version") != world_version) throw Failure(Error::slot);
                confirmed_ = locations(message.at("checked_locations"));
                auto missing = locations(message.at("missing_locations"));
                if ((confirmed_ | missing) != check_mask || (confirmed_ & missing)) throw Failure(Error::unsupported);
                auto scout_commands = scouts_->connected(message, config_.team, config_.slot);
                connected_ = true;
                synced_ = items_.empty(); // AP omits empty ReceivedItems, including after Sync.
                commands.push_back({{"cmd", "Sync"}});
                commands.insert(commands.end(), scout_commands.begin(), scout_commands.end());
            } else if (cmd == "ReceivedItems") {
                if (!connected_) throw Failure(Error::protocol);
                auto index = size_t(integer(message.at("index"), 0, item_limit));
                const auto& values = message.at("items");
                if (!values.is_array() || values.size() > item_limit - index) throw Failure(Error::protocol);
                std::vector<Item> incoming;
                for (const auto& value : values) incoming.push_back(item_from(value));
                if (index > items_.size() || (!synced_ && index != 0)) {
                    synced_ = false;
                    commands.push_back({{"cmd", "Sync"}}); commands.push_back(check_command(checks_ & ~confirmed_));
                    continue;
                }
                if (index == 0 && incoming.size() < items_.size()) throw Failure(Error::protocol);
                auto overlap = std::min(incoming.size(), items_.size() - index);
                if (!std::equal(incoming.begin(), incoming.begin() + overlap, items_.begin() + index)) throw Failure(Error::protocol);
                auto updated = items_;
                updated.insert(updated.end(), incoming.begin() + overlap, incoming.end());
                if (updated != items_) persist(updated, checks_);
                items_ = std::move(updated); synced_ = true;
            } else if (cmd == "LocationInfo") {
                if (!connected_) throw Failure(Error::protocol);
                auto scout_commands = scouts_->location_info(message);
                commands.insert(commands.end(), scout_commands.begin(), scout_commands.end());
            } else if (cmd == "DataPackage") {
                if (!connected_) throw Failure(Error::protocol);
                scouts_->data_package(message);
            } else if (cmd == "RoomUpdate") {
                if (!connected_) throw Failure(Error::protocol);
                if (message.contains("checked_locations")) confirmed_ |= locations(message.at("checked_locations"));
                scouts_->room_update(message);
            } else if (cmd == "ConnectionRefused") throw Failure(Error::slot);
            else if (cmd == "InvalidPacket") throw Failure(Error::protocol);
        }
    } catch (const Json::exception&) { throw Failure(Error::protocol); }
    return commands;
}
std::vector<Json> State::flush() {
    std::vector<Json> commands;
    if (!ready()) return commands;
    if (checks_ & ~confirmed_) commands.push_back(check_command(checks_ & ~confirmed_));
    if (!goal_sent_ && checks_ == check_mask && (moves() & 3) == 3) {
        commands.push_back({{"cmd", "StatusUpdate"}, {"status", 30}}); goal_sent_ = true;
    }
    return commands;
}
}
