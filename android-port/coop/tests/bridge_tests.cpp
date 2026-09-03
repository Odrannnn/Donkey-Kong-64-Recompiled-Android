#include "session.hpp"
#include "../mod/recovery_storage.h"
#include "recomp.h"
#include <array>
#include <bit>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <thread>

extern "C" void dk64_coop_start(uint8_t*, recomp_context*);
extern "C" void dk64_coop_local_ipv4(uint8_t*, recomp_context*);
extern "C" void dk64_coop_recovery_configure_v76(uint8_t*, recomp_context*);
extern "C" void dk64_coop_recovery_status_v76(uint8_t*, recomp_context*);
extern "C" void dk64_coop_tick_v76(uint8_t*, recomp_context*);
extern "C" void dk64_coop_stop(uint8_t*, recomp_context*);
#define CHECK(expr) do { if (!(expr)) { std::fprintf(stderr, "ABI FAIL %d: %s\n", __LINE__, #expr); return 1; } } while (0)
int main() {
    // Only exercise small valid spans; invalid addresses must fail before dereference.
    std::array<uint8_t, 16384> rdram{}; recomp_context ctx{};
    dkcoop::Session host; CHECK(host.start({dkcoop::Role::host, "", 0, 123456}, dkcoop::clock_ms()));
    const char ip[] = "127.0.0.1";
    for (size_t i = 0; i < sizeof(ip); i++) rdram[(256 + i) ^ 3] = uint8_t(ip[i]);
    ctx.r4 = 2; ctx.r5 = int64_t(int32_t(0x80000100u)); ctx.r6 = host.bound_port(); ctx.r7 = 123456;
    dk64_coop_start(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::connecting));
    std::random_device random;
    auto recovery_root = std::filesystem::temp_directory_path()
        / ("dk64-coop-bridge-" + std::to_string(random()));
    std::filesystem::create_directories(recovery_root);
    auto recovery_save = recovery_root / "items_guest_v6.bin";
    std::ofstream(recovery_save, std::ios::binary) << "complete save";
    auto recovery_path = recovery_save.string();
    CHECK(recovery_path.size() < 4095);
    for (size_t i = 0; i <= recovery_path.size(); ++i)
        rdram[(8192 + i) ^ 3] = i < recovery_path.size() ? uint8_t(recovery_path[i]) : 0;
    ctx.r4 = int64_t(int32_t(0x80002000u)); ctx.r5 = 0;
    ctx.r6 = COOP_RECOVERY_SAVE_ITEMS; ctx.r7 = 123456;
    dk64_coop_recovery_configure_v76(rdram.data(), &ctx);
    CHECK(ctx.r2 == COOP_RECOVERY_STORAGE_CONFIGURED);
    dk64_coop_recovery_status_v76(rdram.data(), &ctx);
    CHECK(ctx.r2 == COOP_RECOVERY_STORAGE_CONFIGURED);
    dkcoop::State h{34, 1, 0, dkcoop::active, -123.25f, 456.5f, 987.75f, 4000, 2, 3.5f,
        77, 0x000007A9u};
    dkcoop::State g{34, 2, 4, dkcoop::active, 99.5f, -25.25f, 10, 100, 110, 254};
    auto local = dkcoop::state_to_words(g); std::memcpy(rdram.data() + 512, local.data(), dkcoop::state_words * 4);
    bool exchanged = false;
    for (unsigned i = 0; i < 1000; i++) {
        auto now = dkcoop::clock_ms(); host.tick(h, now);
        ctx.r4 = int64_t(int32_t(0x80000200u)); ctx.r5 = int64_t(int32_t(0x80000300u));
        ctx.r6 = int64_t(int32_t(0x80000400u)); ctx.r7 = int64_t(int32_t(0x80001000u));
        dk64_coop_tick_v76(rdram.data(), &ctx);
        std::array<uint32_t, dkcoop::state_words> remote{};
        std::memcpy(remote.data(), rdram.data() + 768, dkcoop::state_words * 4);
        if (remote == dkcoop::state_to_words(h) && dkcoop::state_to_words(host.remote(now)) == local) { exchanged = true; break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    CHECK(exchanged);
    dk64_coop_recovery_status_v76(rdram.data(), &ctx);
    CHECK((ctx.r2 & COOP_RECOVERY_STORAGE_FOLLOWER) != 0);
    CHECK(dkcoop::state_from_words(local).transition_ticket == 0
        && dkcoop::state_from_words(dkcoop::state_to_words(h)).transition_route == 0x000007A9u);
    // The extended third/fourth spans carry actual projectile snapshots without
    // overwriting adjacent memory or confusing float words with host pointers.
    CoopCombatFrame hc{1, 1, 0, 7, {}, {}}, gc{1, 2, 0, 3, {}, {}};
    hc.pages = gc.pages = 1;
    hc.shots[0] = {123, COOP_GRAPE, std::bit_cast<unsigned>(12.5f), std::bit_cast<unsigned>(-8.0f),
        std::bit_cast<unsigned>(4.0f), 4095, std::bit_cast<unsigned>(0.75f)};
    std::memcpy(rdram.data() + 1024 + sizeof(dkcoop::ProgressInput), &gc, sizeof(gc));
    rdram[4096 + sizeof(dkcoop::ProgressResult) + sizeof(CoopCombatResult) + sizeof(CoopItemResult)
        + sizeof(CoopWorldResult) + sizeof(CoopTransientResult)] = 0xA5;
    bool shot_exchanged = false;
    for (unsigned i = 0; i < 1000; ++i) {
        auto now = dkcoop::clock_ms(); host.tick(h, now, {}, hc);
        ctx.r4 = int64_t(int32_t(0x80000200u)); ctx.r5 = int64_t(int32_t(0x80000300u));
        ctx.r6 = int64_t(int32_t(0x80000400u)); ctx.r7 = int64_t(int32_t(0x80001000u));
        dk64_coop_tick_v76(rdram.data(), &ctx);
        CoopCombatResult combat{};
        std::memcpy(&combat, rdram.data() + 4096 + sizeof(dkcoop::ProgressResult), sizeof(combat));
        if (combat.status == COOP_COMBAT_SHOTS && combat.hands == 7 && combat.shots[0].x == hc.shots[0].x
                && combat.shots[0].id == 123 && combat.shots[0].kind == COOP_GRAPE) { shot_exchanged = true; break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    CHECK(shot_exchanged);
    CHECK(rdram[4096 + sizeof(dkcoop::ProgressResult) + sizeof(CoopCombatResult) + sizeof(CoopItemResult)
        + sizeof(CoopWorldResult) + sizeof(CoopTransientResult)] == 0xA5);
    // Exercise all four ABI arguments with the actual progression structures.
    h.map = g.map = 7;
    local = dkcoop::state_to_words(g); std::memcpy(rdram.data() + 512, local.data(), dkcoop::state_words * 4);
    hc.enabled = gc.enabled = 2; hc.layout = gc.layout = 0x1234;
    hc.enemies[0] = {1, 10, COOP_ENEMY_ALIVE, 0, COOP_KREMLING,
        std::bit_cast<unsigned>(321.0f), 0, 0, coop_enemy_pack(512, 3)};
    gc.enemies[0] = {1, 20, COOP_ENEMY_ALIVE, 0, COOP_KREMLING,
        0, 0, 0, coop_enemy_pack(0, 3)};
    std::memcpy(rdram.data() + 1024 + sizeof(dkcoop::ProgressInput), &gc, sizeof(gc));
    bool moved = false;
    for (unsigned i = 0; i < 1000; ++i) {
        host.tick(h, dkcoop::clock_ms(), {}, hc);
        dk64_coop_tick_v76(rdram.data(), &ctx);
        CoopCombatResult combat{};
        std::memcpy(&combat, rdram.data() + 4096 + sizeof(dkcoop::ProgressResult), sizeof(combat));
        if (combat.motion[0].life == 20 && combat.motion[0].kind == COOP_KREMLING
                && combat.motion[0].x == hc.enemies[0].x
                && coop_enemy_yaw(combat.motion[0]) == 512) { moved = true; break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    CHECK(moved);
    CHECK(rdram[4096 + sizeof(dkcoop::ProgressResult) + sizeof(CoopCombatResult) + sizeof(CoopItemResult)
        + sizeof(CoopWorldResult) + sizeof(CoopTransientResult)] == 0xA5);
    // The former four-word combat tail now carries a reciprocally bound boss
    // phase without changing packet size or accepting overlay pointers.
    h.map = g.map = 8;
    local = dkcoop::state_to_words(g); std::memcpy(rdram.data() + 512, local.data(), dkcoop::state_words * 4);
    hc = {}; hc.enabled = 1; hc.file = 1; hc.pages = 1; hc.boss = {COOP_BOSS_ARMY_DILLO, 700, 0, 1};
    gc = {}; gc.enabled = 1; gc.file = 2; gc.pages = 1; gc.boss = {COOP_BOSS_ARMY_DILLO, 800, 0, 0};
    std::memcpy(rdram.data() + 1024 + sizeof(dkcoop::ProgressInput), &gc, sizeof(gc));
    bool boss_exchanged = false;
    for (unsigned i = 0; i < 1000; ++i) {
        host.tick(h, dkcoop::clock_ms(), {}, hc);
        dk64_coop_tick_v76(rdram.data(), &ctx);
        CoopCombatResult combat{};
        std::memcpy(&combat, rdram.data() + 4096 + sizeof(dkcoop::ProgressResult), sizeof(combat));
        if (combat.status == COOP_COMBAT_READY && combat.boss.kind == COOP_BOSS_ARMY_DILLO
                && combat.boss.life == 800 && combat.boss.peer_life == 700 && combat.boss.phase == 1) {
            boss_exchanged = true; break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    CHECK(boss_exchanged && rdram[4096 + sizeof(dkcoop::ProgressResult) + sizeof(CoopCombatResult)
        + sizeof(CoopItemResult) + sizeof(CoopWorldResult) + sizeof(CoopTransientResult)] == 0xA5);
    h.map = g.map = 83;
    local = dkcoop::state_to_words(g); std::memcpy(rdram.data() + 512, local.data(), dkcoop::state_words * 4);
    hc = {}; hc.enabled = 1; hc.file = 1; hc.pages = 1; hc.boss = {COOP_BOSS_DOGADON, 900, 0, 4};
    gc = {}; gc.enabled = 1; gc.file = 2; gc.pages = 1; gc.boss = {COOP_BOSS_DOGADON, 901, 0, 3};
    std::memcpy(rdram.data() + 1024 + sizeof(dkcoop::ProgressInput), &gc, sizeof(gc));
    boss_exchanged = false;
    for (unsigned i = 0; i < 1000; ++i) {
        host.tick(h, dkcoop::clock_ms(), {}, hc);
        dk64_coop_tick_v76(rdram.data(), &ctx);
        CoopCombatResult combat{};
        std::memcpy(&combat, rdram.data() + 4096 + sizeof(dkcoop::ProgressResult), sizeof(combat));
        if (combat.status == COOP_COMBAT_READY && combat.boss.kind == COOP_BOSS_DOGADON
                && combat.boss.life == 901 && combat.boss.peer_life == 900 && combat.boss.phase == 4) {
            boss_exchanged = true; break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    CHECK(boss_exchanged && rdram[4096 + sizeof(dkcoop::ProgressResult) + sizeof(CoopCombatResult)
        + sizeof(CoopItemResult) + sizeof(CoopWorldResult) + sizeof(CoopTransientResult)] == 0xA5);
    h.map = g.map = 7;
    local = dkcoop::state_to_words(g); std::memcpy(rdram.data() + 512, local.data(), dkcoop::state_words * 4);
    // v45 retires the standalone Japes gate. Nonzero data in its reserved
    // incoming bridge words cannot reactivate the old authority path.
    dkcoop::ProgressInput progress{1, 1, 1, 1, 1, 1, 1, 1};
    std::memcpy(rdram.data() + 1024, &progress, sizeof(progress));
    ctx.r4 = int64_t(int32_t(0x80000200u)); ctx.r5 = int64_t(int32_t(0x80000300u));
    ctx.r6 = int64_t(int32_t(0x80000400u)); ctx.r7 = int64_t(int32_t(0x80001000u));
    dk64_coop_tick_v76(rdram.data(), &ctx);
    dkcoop::ProgressResult retired{}; std::memcpy(&retired, rdram.data() + 4096, sizeof(retired));
    const dkcoop::ProgressResult zero_progress{};
    CHECK(std::memcmp(&retired, &zero_progress, sizeof(retired)) == 0);
    // Last ABI extension: flags travel in both directions and require actual
    // game readback, including medals in the last word and Snide requests in the middle.
    constexpr size_t item_in = 1024 + sizeof(dkcoop::ProgressInput) + sizeof(CoopCombatFrame);
    constexpr size_t item_out = 4096 + sizeof(dkcoop::ProgressResult) + sizeof(CoopCombatResult);
    CoopItemInput host_items{1, 2, 1, {1, 0x80000000u, 0x8000u, 0, 0x80000000u}};
    CoopItems guest_items{}; coop_items_prepare(&guest_items, 1, 1, 0);
    bool items_exchanged = false;
    for (unsigned i = 0; i < 1000; ++i) {
        auto now = dkcoop::clock_ms(); host.tick(h, now, {1, 2, 1, 1}, hc, host_items);
        std::memcpy(rdram.data() + item_in, &guest_items.input, sizeof(CoopItemInput));
        dk64_coop_tick_v76(rdram.data(), &ctx);
        CoopItemResult result{}; std::memcpy(&result, rdram.data() + item_out, sizeof(result));
        coop_items_receive(&guest_items, result);
        for (unsigned j = 0; j < COOP_ITEM_WORDS; ++j) guest_items.input.owned[j] |= result.apply[j];
        if (host.items(now).status == 3 && guest_items.result.status == 3
                && guest_items.input.owned[2] == 0x8000u) { items_exchanged = true; break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    CHECK(items_exchanged && guest_items.input.owned[0] == 1 && guest_items.input.owned[1] == 0x80000000u
        && guest_items.input.owned[4] == 0x80000000u);
    coop_items_observe(&guest_items, 1, 0x84, 1, 0, 0, 1, 0); // Nintendo coin collected by guest.
    guest_items.input.owned[2] |= 0x4000u; CHECK(guest_items.input.request[2] == 0x4000u);
    coop_items_observe(&guest_items, 1, 0x1FD, 1, 0, 0, 1, 0);
    guest_items.input.owned[2] |= 0x10000u; CHECK(guest_items.input.request[2] == 0x14000u);
    items_exchanged = false;
    for (unsigned i = 0; i < 1000; ++i) {
        auto now = dkcoop::clock_ms(); host.tick(h, now, {1, 2, 1, 1}, hc, host_items);
        auto hr = host.items(now); for (unsigned j = 0; j < COOP_ITEM_WORDS; ++j) host_items.owned[j] |= hr.apply[j];
        std::memcpy(rdram.data() + item_in, &guest_items.input, sizeof(CoopItemInput));
        dk64_coop_tick_v76(rdram.data(), &ctx);
        CoopItemResult result{}; std::memcpy(&result, rdram.data() + item_out, sizeof(result));
        coop_items_receive(&guest_items, result);
        if (host_items.owned[2] == 0x1C000u && !guest_items.input.request[2]) { items_exchanged = true; break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    CHECK(items_exchanged && rdram[item_out + sizeof(CoopItemResult) + sizeof(CoopWorldResult)
        + sizeof(CoopTransientResult)] == 0xA5);
    for (uint32_t bad : {0u, 0x7fffffffu, 0x80000201u, 0x9ffffffcu, 0xffffffffu}) {
        ctx.r4 = bad; ctx.r5 = 0x80000300u; dk64_coop_tick_v76(rdram.data(), &ctx);
        CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
    }
    for (uint32_t bad : {0u, 0x80000401u, 0x9ffffc78u, 0x9ffffd00u, 0x9ffffe00u, 0x9ffffffcu, 0xffffffffu}) {
        ctx.r4 = 0x80000200u; ctx.r5 = 0x80000300u; ctx.r6 = bad; ctx.r7 = 0x80001000u;
        dk64_coop_tick_v76(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
        ctx.r6 = 0x80000400u; ctx.r7 = bad;
        dk64_coop_tick_v76(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
    }
    ctx.r6 = 0x80000400u; ctx.r7 = 0x9ffffb00u; // 1280 bytes remain: full v76 result cannot fit.
    dk64_coop_tick_v76(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
    ctx.r7 = 0x9ffffa48u; // 1464 bytes remain: older results fit, full v76 must fail.
    dk64_coop_tick_v76(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
    ctx.r6 = 0xA0000000u - 2864u; ctx.r7 = 0x80001000u; // Four bytes short of v76 input.
    dk64_coop_tick_v76(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
    ctx.r6 = 0x80000400u; ctx.r7 = 0xA0000000u - 3472u; // Four bytes short of v76 result.
    dk64_coop_tick_v76(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
    ctx.r4 = 2; ctx.r5 = 0; ctx.r6 = 6464; ctx.r7 = 123456;
    dk64_coop_start(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::error));
    dk64_coop_stop(rdram.data(), &ctx); CHECK(ctx.r2 == 0);
    uint16_t available_port = 0;
    for (uint16_t candidate = 36640; candidate < 36740; candidate += 2) {
        dkcoop::Session probe;
        if (probe.start({dkcoop::Role::host, "", candidate, 123456}, dkcoop::clock_ms())) {
            available_port = candidate;
            probe.stop();
            break;
        }
    }
    CHECK(available_port != 0);
    ctx.r4 = 1; ctx.r5 = 0; ctx.r6 = available_port; ctx.r7 = 123456;
    dk64_coop_start(rdram.data(), &ctx); CHECK(ctx.r2 == uint32_t(dkcoop::Status::listening));
    dk64_coop_local_ipv4(rdram.data(), &ctx);
    const uint32_t local_ipv4 = uint32_t(ctx.r2);
    CHECK(!local_ipv4 || (local_ipv4 >> 24) != 127);
    dk64_coop_stop(rdram.data(), &ctx); CHECK(ctx.r2 == 0);
    std::filesystem::remove_all(recovery_root);
    std::puts("PASS: v76 native ABI, host IPv4 reporting, transient span isolation, coordinated transitions, Army Dillo/Dogadon phases, sign-extended pointers, pose/shot/enemy movement, item grants/requests and invalid address guards");
}
