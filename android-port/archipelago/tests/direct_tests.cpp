// SPDX-License-Identifier: GPL-3.0-or-later
#include "state.hpp"
#include "../mod/policy.h"
#include <fstream>
#include <iostream>
#include <chrono>
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (0)
using namespace dkap;
template<class F> void rejects(F f, Error expected) {
    try { f(); } catch (const Failure& e) { CHECK(e.code == expected); return; }
    throw std::runtime_error("Expected rejection");
}
Json connected() { return {{"cmd", "Connected"}, {"team", 0}, {"slot", 1}, {"slot_data", {{"recomp_profile", profile}, {"Version", world_version}}},
    {"checked_locations", Json::array()}, {"missing_locations", location_ids}}; }
std::vector<Json> packet(State& state, const Json& message) { return state.receive(Json::array({message}).dump()); }
void login(State& s) {
    auto connect = packet(s, {{"cmd", "RoomInfo"}, {"seed_name", "seed"}, {"games", Json::array({game})}});
    CHECK(connect.size() == 1 && connect[0]["cmd"] == "Connect");
    CHECK(connect[0]["password"].is_null());
    CHECK(packet(s, connected())[0]["cmd"] == "Sync");
}
int main() {
    auto root = std::filesystem::temp_directory_path() / ("dkap-test-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(root);
    try {
        Config c{"ws://127.0.0.1:38281", "DK64Test", "", "seed"};
        auto id = identity_for(c); CHECK(id == "70d663fa33e134c9516ce85cb1415b3c"); // Version 0.1 Python binding.
        auto moved = c; moved.server = "wss://example.org:443"; moved.password = "different"; CHECK(identity_for(moved) == id);
        moved.seed = "different"; CHECK(identity_for(moved) != id);
        for (auto url : {"http://host", "wss://user:pass@host", "ws://host:70000", "ws://host/#x", "ws://host/?password=x"}) {
            auto bad = c; bad.server = url;
            try { rejects([&] { validate_config(bad); }, Error::config); }
            catch (...) { std::cerr << "URL validation: " << url << '\n'; throw; }
        }
        auto path = root / "state.json";
        {
            State s(c, path); CHECK(!s.ready() && s.moves() == 0);
            rejects([&] { State locked(c, path); }, Error::storage);
            login(s); CHECK(s.ready()); // Real AP sends no empty ReceivedItems packet.
            s.record_checks(3); CHECK(s.flush()[0]["locations"].size() == 2);
            Json items = Json::array({Item{item_ids[0], location_ids[0], 1, 1}, Item{item_ids[1], location_ids[1], 1, 1}});
            packet(s, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", items}});
            CHECK(s.moves() == 3 && s.item_count() == 2);
            packet(s, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", items}}); CHECK(s.item_count() == 2);
            auto flush = s.flush(); CHECK(flush.back()["cmd"] == "StatusUpdate"); CHECK(flush.back()["status"] == 30);
            CHECK(s.flush().size() == 1);
            s.disconnect(); CHECK(!s.ready() && s.moves() == 0);
            login(s); CHECK(!s.ready());
            CHECK(packet(s, {{"cmd", "ReceivedItems"}, {"index", 2}, {"items", Json::array()}})[0]["cmd"] == "Sync");
            packet(s, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", items}}); CHECK(s.ready() && s.moves() == 3);
            auto rollback = items; rollback.erase(1);
            rejects([&] { packet(s, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", rollback}}); }, Error::protocol);
            auto unsupported = items; unsupported[0][0] = 999;
            rejects([&] { packet(s, {{"cmd", "ReceivedItems"}, {"index", 0}, {"items", unsupported}}); }, Error::unsupported);
            CHECK(s.item_count() == 2);
        }
        {
            State s(c, path); CHECK(s.checks() == 3 && s.item_count() == 2 && !s.ready());
            rejects([&] { packet(s, {{"cmd", "RoomInfo"}, {"seed_name", "wrong"}, {"games", Json::array({game})}}); }, Error::seed);
        }
        auto bad_binding = c; bad_binding.slot = 2;
        rejects([&] { State s(bad_binding, path); }, Error::storage);
        { std::ofstream file(path); file << "{broken"; }
        rejects([&] { State s(c, path); }, Error::storage);
        rejects([&] { parse_bounded(std::string(message_limit + 1, ' ')); }, Error::protocol);
        rejects([&] { parse_bounded(std::string(40, '[') + "0" + std::string(40, ']')); }, Error::protocol);
        CHECK(ap_first_move_level(0, 1) == 1 && ap_first_move_level(3, 1) == 3 && ap_first_move_level(0, 0) == 0);
        CHECK(ap_safe_frame(1, 0, 1, 0, 0, 0));
        CHECK(!ap_safe_frame(1, 1, 1, 0, 0, 0) && !ap_safe_frame(1, 0, 1, 1, 0, 0));
        std::filesystem::remove_all(root);
        std::cout << "PASS: binding, protocol, replay, durable checks/items, exclusive journal, corruption, bounds and grant policy\n";
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
