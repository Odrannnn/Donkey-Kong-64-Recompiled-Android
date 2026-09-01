// SPDX-License-Identifier: GPL-3.0-or-later
#include "scouts.hpp"
#include "../mod/checks.h"
#include <iostream>
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (0)
using namespace dkap;

template<class F> void rejects(F f, Error expected) {
    try { f(); } catch (const Failure& e) { CHECK(e.code == expected); return; }
    throw std::runtime_error("Expected rejection");
}
Json room() {
    return {{"cmd", "RoomInfo"}, {"datapackage_checksums", {
        {game, "1111111111111111111111111111111111111111"},
        {"Other Game", "2222222222222222222222222222222222222222"}}}};
}
Json connect() {
    return {{"cmd", "Connected"}, {"slot_info", {
        {"1", {{"name", "DK64Test"}, {"game", game}, {"type", 1}, {"class", "NetworkSlot"}}},
        {"2", {{"name", "OtherPlayer"}, {"game", "Other Game"}, {"type", 1}, {"class", "NetworkSlot"}}}}},
        {"players", Json::array({
            {{"team", 0}, {"slot", 1}, {"alias", "DK Alias"}, {"name", "DK64Test"}, {"class", "NetworkPlayer"}},
            {{"team", 0}, {"slot", 2}, {"alias", "Friend"}, {"name", "OtherPlayer"}, {"class", "NetworkPlayer"}},
            {{"team", 1}, {"slot", 2}, {"alias", "Wrong Team"}, {"name", "OtherPlayer"}, {"class", "NetworkPlayer"}}})}};
}
Json packages() {
    return {{"cmd", "DataPackage"}, {"data", {{"games", {
        {game, {{"checksum", "1111111111111111111111111111111111111111"},
                {"item_name_to_id", {{"Local Forty Two", 42}, {"Unused", 43}}}}},
        {"Other Game", {{"checksum", "2222222222222222222222222222222222222222"},
                {"item_name_to_id", {{"Remote Forty Two", 42}}}}}}}}}};
}
int main() {
    try {
        Scouts scouts({location_ids.begin(), location_ids.end()});
        scouts.room(room());
        auto commands = scouts.connected(connect(), 0, 1);
        CHECK(commands.size() == 1 && commands[0]["cmd"] == "LocationScouts"
            && commands[0]["create_as_hint"] == 0 && commands[0]["locations"].size() == 2);
        Json info{{"cmd", "LocationInfo"}, {"locations", Json::array({
            {{"item", 42}, {"location", location_ids[0]}, {"player", 1}, {"flags", 1}, {"class", "NetworkItem"}},
            {{"item", 42}, {"location", location_ids[1]}, {"player", 2}, {"flags", 2}, {"class", "NetworkItem"}}})}};
        commands = scouts.location_info(info);
        CHECK(commands.size() == 1 && commands[0]["cmd"] == "GetDataPackage" && commands[0]["games"].size() == 2);
        scouts.data_package(packages());
        auto snapshot = scouts.snapshot();
        CHECK(snapshot.enabled && snapshot.locations.size() == 2 && snapshot.shop_labels.empty());
        CHECK(snapshot.locations[0].item_name == "Local Forty Two" && snapshot.locations[0].recipient_name == "DK Alias");
        CHECK(snapshot.locations[1].item_name == "Remote Forty Two" && snapshot.locations[1].recipient_name == "Friend");
        scouts.room_update({{"cmd", "RoomUpdate"}, {"players", Json::array({
            {{"team", 0}, {"slot", 1}, {"alias", "New DK"}, {"name", "DK64Test"}},
            {{"team", 0}, {"slot", 2}, {"alias", "New Friend"}, {"name", "OtherPlayer"}},
            {{"team", 1}, {"slot", 2}, {"alias", "Still Wrong"}, {"name", "OtherPlayer"}}})}});
        CHECK(scouts.snapshot().locations[1].recipient_name == "New Friend");
        auto conflict = info; conflict["locations"][0]["item"] = 43;
        rejects([&] { scouts.location_info(conflict); }, Error::protocol);
        CHECK(scouts.snapshot().locations[0].item_name == "Local Forty Two");
        scouts.disconnect(); CHECK(!scouts.snapshot().enabled && scouts.snapshot().locations.empty());

        std::vector<int64_t> shops;
        Json shop_info = Json::array();
        for (const auto& shop : ap_shop_locations) {
            shops.push_back(shop.id);
            shop_info.push_back(Json{{"item", 42}, {"location", shop.id}, {"player", 1}, {"flags", 1}});
        }
        Scouts campaign(shops); campaign.room(room()); campaign.connected(connect(), 0, 1);
        CHECK(campaign.location_info({{"cmd", "LocationInfo"}, {"locations", shop_info}}).size() == 1);
        auto local_package = packages(); local_package["data"]["games"].erase("Other Game");
        campaign.data_package(local_package);
        snapshot = campaign.snapshot();
        CHECK(snapshot.locations.size() == AP_SHOP_COUNT && snapshot.shop_labels.size() == AP_SHOP_COUNT);
        for (size_t i = 0; i < snapshot.shop_labels.size(); ++i) {
            CHECK(snapshot.shop_labels[i].location == uint32_t(shops[i]));
            CHECK(std::string(snapshot.shop_labels[i].item) == "LOCAL FORTY TWO");
            CHECK(std::string(snapshot.shop_labels[i].subtitle) == "TO DK ALIAS");
        }
        auto wrong_checksum = local_package;
        wrong_checksum["data"]["games"][game]["checksum"] = "3333333333333333333333333333333333333333";
        Scouts bad(shops); bad.room(room()); bad.connected(connect(), 0, 1);
        bad.location_info({{"cmd", "LocationInfo"}, {"locations", shop_info}});
        rejects([&] { bad.data_package(wrong_checksum); }, Error::protocol);
        CHECK(bad.snapshot().locations.empty());
        std::cout << "PASS: team aliases, cross-game ID names, checksums, atomic replays, reconnect clearing and 120 shop labels\n";
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
