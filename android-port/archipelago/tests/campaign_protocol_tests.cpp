// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_protocol.hpp"
#include <chrono>
#include <iostream>
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (0)
using namespace dkap;

template<class F> void rejects(F action, Error expected) {
    try { action(); } catch (const Failure& failure) { CHECK(failure.code == expected); return; }
    throw std::runtime_error("Expected rejection");
}

CampaignSeed protocol_seed() {
    Json seed{{"format", 1}, {"world_version", "1.5.8"},
        {"revision", "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4"}, {"game", "Donkey Kong 64"},
        {"seed_name", "campaign-room"}, {"player", "DK64Test"}, {"team", 0}, {"slot", 1},
        {"locations", Json::array({14041180, 14041181})}, {"starting_inventory", Json::array()},
        {"boss_bananas", Json::array({60,120,200,250,300,350,400})},
        {"edits", {{"entrances", Json::array()}}}};
    seed["blockers"] = Json::array();
    for (int i = 0; i < 8; ++i) seed["blockers"].push_back({{"item", "keys"}, {"count", i + 1}});
    return CampaignSeed::parse(seed);
}

std::vector<Json> packet(CampaignProtocol& state, const Json& message) {
    return state.receive(Json::array({message}).dump());
}

int main() {
    auto root = std::filesystem::temp_directory_path()
        / ("dkap-campaign-protocol-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(root);
    try {
        auto seed = protocol_seed();
        Config config{"ws://127.0.0.1:38281", seed.player, "secret", seed.seed_name, seed.team, seed.slot};
        CampaignChannel channel; channel.start(seed, root / "campaign.json");
        CampaignProtocol state(config, seed, channel);
        auto commands = packet(state, {{"cmd", "RoomInfo"}, {"seed_name", seed.seed_name},
            {"games", Json::array({"Donkey Kong 64"})}});
        CHECK(commands.size() == 1 && commands[0]["cmd"] == "Connect" && commands[0]["game"] == "Donkey Kong 64"
            && commands[0]["name"] == seed.player && commands[0]["password"] == "secret"
            && commands[0]["items_handling"] == 1
            && commands[0]["uuid"] == "dk64recomp-campaign-" + seed.binding_hex());
        commands = packet(state, {{"cmd", "Connected"}, {"team", 0}, {"slot", 1},
            {"slot_data", {{"Version", "1.5.8"}}}, {"checked_locations", Json::array()},
            {"missing_locations", Json::array({14041180, 14041181})}});
        CHECK(commands.size() == 1 && commands[0]["cmd"] == "Sync" && state.ready());

        ap_check_set observed{}; ap_check_add(&observed, 14041180); state.observe(observed);
        commands = state.flush();
        CHECK(commands.size() == 1 && commands[0]["cmd"] == "LocationChecks"
            && commands[0]["locations"] == Json::array({14041180}));
        packet(state, {{"cmd", "RoomUpdate"}, {"checked_locations", Json::array({14041180})}});
        CHECK(state.flush().empty());

        Json first = Json::array({14041104, 14041180, 1, 1});
        CHECK(packet(state, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", Json::array({first})}}).empty());
        CHECK(channel.snapshot().receipt_count == 1);
        CHECK(packet(state, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", Json::array({first})}}).empty());
        commands = packet(state, {{"cmd", "ReceivedItems"}, {"index", 2}, {"items", Json::array()}});
        CHECK(commands.size() == 1 && commands[0]["cmd"] == "Sync" && !state.ready());
        CHECK(packet(state, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", Json::array({first})}}).empty()
            && state.ready());
        Json conflict = Json::array({14041108, 14041180, 1, 1});
        rejects([&] { packet(state, {{"cmd", "ReceivedItems"}, {"index", 0},
            {"items", Json::array({conflict})}}); }, Error::protocol);
        CHECK(channel.snapshot().receipt_count == 1);

        ap_check_set invalid{}; ap_check_add(&invalid, 14041182);
        rejects([&] { state.observe(invalid); }, Error::protocol);
        state.disconnect(); CHECK(!state.ready() && state.flush().empty());

        auto rejected_case = [&](Json room, Json connected, Error error) {
            CampaignProtocol candidate(config, seed, channel);
            if (!room.is_null()) packet(candidate, room);
            rejects([&] { packet(candidate, connected); }, error);
        };
        rejects([&] { CampaignProtocol wrong(Config{"ws://127.0.0.1:38281", "Other", "", seed.seed_name, 0, 1}, seed, channel); }, Error::config);
        CampaignProtocol wrong_room(config, seed, channel);
        rejects([&] { packet(wrong_room, {{"cmd", "RoomInfo"}, {"seed_name", "wrong"},
            {"games", Json::array({"Donkey Kong 64"})}}); }, Error::seed);
        Json room{{"cmd", "RoomInfo"}, {"seed_name", seed.seed_name}, {"games", Json::array({"Donkey Kong 64"})}};
        rejected_case(room, {{"cmd", "Connected"}, {"team", 0}, {"slot", 2},
            {"slot_data", {{"Version", "1.5.8"}}}, {"checked_locations", Json::array()},
            {"missing_locations", Json::array({14041180, 14041181})}}, Error::slot);
        rejected_case(room, {{"cmd", "Connected"}, {"team", 0}, {"slot", 1},
            {"slot_data", {{"Version", "1.5.7"}}}, {"checked_locations", Json::array()},
            {"missing_locations", Json::array({14041180, 14041181})}}, Error::unsupported);
        rejected_case(room, {{"cmd", "Connected"}, {"team", 0}, {"slot", 1},
            {"slot_data", {{"Version", "1.5.8"}}}, {"checked_locations", Json::array({14041180})},
            {"missing_locations", Json::array()}}, Error::unsupported);

        channel.stop(); std::filesystem::remove_all(root);
        std::cout << "PASS: campaign AP identity, partition, durable receipts/checks, replay and resync protocol\n";
    } catch (const std::exception& error) {
        std::filesystem::remove_all(root); std::cerr << error.what() << '\n'; return 1;
    }
}
