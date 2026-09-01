#include "session.hpp"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>

// Standalone transport probe: never loads the game, a ROM or the native mod ABI.
int main(int argc, char** argv) {
    if ((argc != 6 && argc != 7) || (std::string(argv[1]) != "host" && std::string(argv[1]) != "join" && std::string(argv[1]) != "mirror")) {
        std::fprintf(stderr, "Usage: coop_peer host|join|mirror HOST_IPV4 PORT ROOM SECONDS [items]\nSynthetic transport/inventory probe, not a second game client.\n"); return 2;
    }
    bool test_items = argc == 7;
    if (test_items && (std::string(argv[6]) != "items" || std::string(argv[1]) == "mirror")) return 2;
    int port = std::atoi(argv[3]), room = std::atoi(argv[4]), seconds = std::atoi(argv[5]);
    if (port < 1024 || port > 65535 || room < 100000 || room > 999999 || seconds < 1 || seconds > 120) return 2;
    dkcoop::Config config{std::string(argv[1]) == "host" ? dkcoop::Role::host : dkcoop::Role::join,
        argv[2], uint16_t(port), uint32_t(room)};
    dkcoop::Session session;
    if (!session.start(config, dkcoop::clock_ms())) { std::fprintf(stderr, "%s\n", session.error().c_str()); return 1; }
    std::printf("READY %s port=%u\n", argv[1], session.bound_port()); std::fflush(stdout);
    dkcoop::State local{34, 1, config.role == dkcoop::Role::host ? 0u : 1u, dkcoop::active, 10, 20, 30, 123, 0, 0};
    auto start = dkcoop::clock_ms(); unsigned snapshots = 0, invalid = 0;
    bool mirror = std::string(argv[1]) == "mirror";
    CoopItems items{}; items.join = config.role == dkcoop::Role::join;
    bool collected = false, items_synced = false;
    if (test_items) {
        coop_items_prepare(&items, 1, 1, config.role == dkcoop::Role::host ? 0 : 1);
        if (config.role == dkcoop::Role::host) { items.input.owned[0] = ~0u; items.input.owned[1] = 0xFF; }
    }
    if (mirror) local.flags = 0;
    while (dkcoop::clock_ms() - start < uint64_t(seconds) * 1000) {
        auto now = dkcoop::clock_ms(); session.tick(local, now, {}, {}, items.input); auto remote = session.remote(now);
        if (test_items) {
            coop_items_receive(&items, session.items(now));
            for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) items.input.owned[i] |= items.result.apply[i];
            if (config.role == dkcoop::Role::join && items.result.status == 3 && !collected) {
                for (unsigned id = 40; id < COOP_ITEMS; ++id) {
                    if (id < COOP_PICKUP_FIRST) coop_items_observe(&items, 1, coop_item_flag(id), 1, 0, 0, 1, 1);
                    else coop_items_note(&items, id);
                    items.input.owned[id / 32] |= 1u << (id % 32);
                }
                collected = true;
            }
            bool complete = items.result.status == 3;
            for (unsigned id = 0; id < COOP_ITEMS; ++id) complete &= coop_item_has(items.input.owned, id);
            for (auto word : items.input.request) complete &= !word;
            items_synced |= complete;
        }
        if (remote.flags & dkcoop::active) {
            snapshots++;
            if (!mirror && (remote.map != 34 || remote.character == local.character || remote.x != 10 || remote.y != 20
                    || remote.z != 30 || remote.yaw != 123)) invalid++;
        }
        if (mirror) {
            // Keep the source Kong: row/frame data refers to that Kong's clip.
            local = remote;
            float phase = float(now - start) / 1800.0f;
            local.x += 70.0f * std::cos(phase); local.z += 70.0f * std::sin(phase);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    auto stats = session.statistics();
    std::printf("{\"role\":\"%s\",\"received_snapshots\":%u,\"invalid_snapshots\":%u,\"sent\":%llu,\"received\":%llu,\"rejected\":%llu}\n",
        argv[1], snapshots, invalid, (unsigned long long)stats.sent, (unsigned long long)stats.received, (unsigned long long)stats.rejected);
    if (test_items) std::printf("%s: synthetic inventory probe, all 5894 collectible/progression/feeding IDs and host acknowledgments exchanged\n", items_synced ? "PASS" : "FAIL");
    return snapshots > 0 && invalid == 0 && (!test_items || items_synced) ? 0 : 1;
}
