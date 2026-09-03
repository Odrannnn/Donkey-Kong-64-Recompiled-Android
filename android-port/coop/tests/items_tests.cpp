#include "session.hpp"
#include "../mod/inventory_types.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <cstring>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
using namespace dkcoop;
static unsigned checks = 0;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "ITEM FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)
#include "lossy_relay.hpp"
static unsigned flags[0x400]{}, writes = 0, saves = 0, block_write = 0;
static CoopItems* current_game;
using MockCharacterProgress = CoopCharacterProgress;
using MockPlayerProgress = CoopPlayerProgress;
static_assert(sizeof(MockCharacterProgress) == 0x5E && sizeof(MockPlayerProgress) == 0x306);
static MockPlayerProgress D_global_asm_807FC950[4]{};
static unsigned short D_global_asm_807FC930[8]{};
static int hud_object;
static void* D_global_asm_80754280 = &hud_object;
static unsigned current_map = 7, mock_level = 7, mock_layout_error = 0;
static short D_global_asm_807F6240[600]{};
static unsigned live_calls = 0, live_slot = 0, live_state = 0;
static unsigned live_reveals = 0, live_reveal_object = 0;
static unsigned live_mermaid_available = 0, live_mermaid_state = 30;
static unsigned live_mermaid_progress = 0, live_mermaid_refreshes = 0;
static unsigned live_trombone_notifications = 0, live_trombone_mode = 0;
static unsigned char live_raw[0x200]{};
static void func_global_asm_8063DA40(short slot, short state) {
    ++live_calls; live_slot = (unsigned short)slot; live_state = (unsigned short)state;
    unsigned object = (unsigned short)D_global_asm_807F6240[live_slot];
    if (object < sizeof(live_raw)) live_raw[object] = (unsigned char)state;
}
static unsigned coop_live_world_object_raw_state(unsigned object, unsigned* raw) {
    for (unsigned slot = 0; slot < 600; ++slot)
        if ((unsigned short)D_global_asm_807F6240[slot] == object) {
            if (object >= sizeof(live_raw)) return 0;
            *raw = live_raw[object]; return 1;
        }
    return 0;
}
static unsigned coop_live_world_reveal_object(unsigned object) {
    unsigned raw = 0;
    if (!coop_live_world_object_raw_state(object, &raw)) return 0;
    ++live_reveals; live_reveal_object = object; return 1;
}
static unsigned coop_live_world_mermaid_ready(void) {
    return live_mermaid_available && (live_mermaid_state == 30 || live_mermaid_state == 31
        || live_mermaid_state == 32 || live_mermaid_state == 39);
}
static unsigned coop_live_world_mermaid_refresh(void) {
    if (!coop_live_world_mermaid_ready()) return 0;
    if (live_mermaid_state == 39 || live_mermaid_state == 32) return 1;
    unsigned pearls = 0;
    for (unsigned flag = 0xBA; flag <= 0xBE; ++flag) pearls += flags[flag] != 0;
    live_mermaid_state = flags[0xBF] ? 32 : (pearls == 5 ? 39 : (pearls ? 31 : 30));
    live_mermaid_progress = 0;
    ++live_mermaid_refreshes;
    return 1;
}
static unsigned coop_live_world_isles_trombone_ready(void) {
    unsigned raw = 0;
    return coop_live_world_object_raw_state(0x31, &raw) && raw <= 6;
}
static unsigned coop_live_world_isles_trombone_refresh(void) {
    unsigned raw = 0;
    if (!coop_live_world_object_raw_state(0x31, &raw) || raw > 6) return 0;
    if (raw >= 3) return 1;
    ++live_trombone_notifications;
    live_trombone_mode = 2;
    live_raw[0x31] = 6;
    return 1;
}
static void* D_global_asm_807FD730 = nullptr;
static unsigned pickups[1700]{};
static unsigned getLevelIndex(unsigned, unsigned) { return mock_level; }
static unsigned func_global_asm_80631C20(unsigned l) { return coop_pickup_start[l+1] - coop_pickup_start[l] + mock_layout_error; }
static unsigned func_global_asm_8060E3B0(unsigned ordinal, unsigned level) {
    CHECK(level < 8 && ordinal < func_global_asm_80631C20(level)); return pickups[coop_pickup_start[level] + ordinal];
}
static void func_global_asm_8060E430(unsigned ordinal, unsigned value, unsigned level) {
    CHECK(value == 1 && level < 8 && ordinal < func_global_asm_80631C20(level));
    ++writes; if (!block_write) pickups[coop_pickup_start[level] + ordinal] = 1;
}
static void seed_gbs(unsigned level, unsigned kong, unsigned count) {
    for (unsigned id = COOP_GB_FIRST; id < COOP_PICKUP_FIRST && count; ++id) {
        auto row = coop_golden_bananas[id-COOP_GB_FIRST];
        if (row.level == level && row.kong == kong) { flags[row.flag] = 1; --count; }
    }
    CHECK(!count);
}
static unsigned hud_updates = 0, mutate_counter = 0;
static void func_global_asm_806F8278(unsigned player) { CHECK(player == 0 && D_global_asm_80754280); ++hud_updates; }
static unsigned isFlagSet(int flag, unsigned type) {
    CHECK(type == 0 && (coop_item_id(flag) >= 0 || flag == 0x180
        || flag == 0xA0 || flag == 0xCE || flag == 0x19D));
    return flags[flag];
}
static void setFlag(int flag, unsigned value, unsigned type) {
    unsigned reversible_world = flag == 0xA0 || flag == 0xCE || flag == 0x19D;
    CHECK(type == 0 && value <= 1 && (coop_item_id(flag) >= 0 || reversible_world));
    // Execute the production flag observation policy on incoming applications too.
    coop_items_observe(current_game, 1, flag, value, type, flags[flag], 1, 0);
    ++writes; if (!block_write) flags[flag] = value;
    if (mutate_counter && flag >= 0x1FD && flag <= 0x224)
        ++D_global_asm_807FC950[0].character_progress[(flag - 0x1FD) % 5].golden_bananas[(flag - 0x1FD) / 5];
}
static void func_global_asm_8060DEC8() { ++saves; }
#include "../mod/world_live_game.h"
#include "../mod/items_game.h"
#include "../mod/world_game.h"
static unsigned bit(unsigned id) { return 1u << (id % 32); }
static bool any(const unsigned* words) { for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) if (words[i]) return true; return false; }
static void protocol_checks() {
    Packet p{Kind::state, 10, 123, 456, 123456}; p.authority_node = 789;
    p.items = {1, 2, 1, 2, {0x80000001, 0x12345678, 0x8000, 0x80000000, 0x12345678}, {1, 0, 0x8000, 0, 0x10000000}};
    p.world = {1, 2, 2, 1, 5, 5, {1, 2, 3}};
    Packet out{}; auto b = encode(p);
    CHECK(b.size() == packet_size && decode(b.data(), b.size(), out));
    CHECK(item_words(out.items) == item_words(p.items));
    CHECK(world_words(out.world) == world_words(p.world));
    CHECK(b[96] == 0x57 && b[97] == 0x4F && b[98] == 0x52 && b[99] == 0x4C);
    WorldWire malformed_world = p.world; malformed_world.revision[2] = 0;
    CHECK(!valid_world(malformed_world));
    malformed_world = {}; malformed_world.feature = malformed_world.file = 1;
    malformed_world.request[2] = 1; malformed_world.base[2] = 1;
    CHECK(!valid_world(malformed_world));
    CHECK(b[943] == 1 && b[940] == 0x80 && b[947] == 0x78);
    for (unsigned offset : {923u, 927u, 931u, 935u, 939u, 1036u}) {
        auto bad = b; bad[offset] = 255; CHECK(!decode(bad.data(), bad.size(), out));
    }
    auto bad = b; bad[5] = 6; CHECK(!decode(bad.data(), bad.size(), out));
    bad = b; bad[5] = 9; CHECK(!decode(bad.data(), bad.size(), out)); // v9 has the same size but different ID rules.
    CHECK(!decode(b.data(), 920, out));
    Packet overlap = p; overlap.progress = {1, 2, 1, 2, 1, 0};
    auto overlap_bytes = encode(overlap); CHECK(!decode(overlap_bytes.data(), overlap_bytes.size(), out));
    for (Kind kind : {Kind::hello, Kind::welcome, Kind::bye, Kind::busy}) {
        p.kind = kind; if (kind == Kind::hello || kind == Kind::busy) p.session = 0;
        b = encode(p); CHECK(!decode(b.data(), b.size(), out));
    }
    for (unsigned page = 0; page < COOP_ITEM_PAGES; ++page) {
        Packet q{Kind::state, 11, 123, 456, 123456}; q.authority_node = 789; q.items = {1, 1, 1, 1}; q.items.page = page;
        unsigned id = page * COOP_ITEM_PAGE_WORDS * 32 + 8;
        q.items.owned[id / 32] = q.items.request[id / 32] = bit(id);
        auto raw = encode(q); CHECK(decode(raw.data(), raw.size(), out));
        CHECK(out.items.page == page && coop_item_has(out.items.owned, id) && coop_item_has(out.items.request, id));
    }
    Packet reserved{Kind::state, 12, 123, 456, 123456}; reserved.authority_node = 789; reserved.items = {1, 1, 1, 1}; reserved.items.page = COOP_ITEMS / (COOP_ITEM_PAGE_WORDS * 32);
    reserved.items.owned[COOP_ITEMS / 32] = bit(COOP_ITEMS);
    auto invalid_bits = encode(reserved); CHECK(!decode(invalid_bits.data(), invalid_bits.size(), out));
    reserved.items.owned[COOP_ITEMS / 32] = 0; reserved.items.owned[COOP_ITEM_WORDS - 1] = 1;
    invalid_bits = encode(reserved); CHECK(!decode(invalid_bits.data(), invalid_bits.size(), out));
    CHECK(!valid_items({0, 1})); CHECK(!valid_items({1, 0, 1}));
    CHECK(!valid_items({1, 1, 0, 1, {1}}));
    CHECK(!valid_items({1, 1, 1, 1, {}, {1}}));
    CoopItemInput invalid{1, 1, 1}; invalid.owned[2] = 0x10000;
    CHECK(!valid_items_input(invalid));
    for (unsigned i = 0; i < COOP_SNIDE_COUNT; ++i) {
        ItemWire w{1, 1, 1, 1}; unsigned id = COOP_SNIDE_FIRST + i;
        w.owned[id / 32] = bit(id); CHECK(!valid_items(w));
        w.owned[i / 32] |= bit(i); CHECK(valid_items(w));
        w.request[id / 32] = bit(id); CHECK(valid_items(w));
    }
}
static void policy_checks() {
    unsigned seen[0x400]{};
    for (unsigned id = 0; id < COOP_ITEMS; ++id) {
        int f = coop_item_flag(id);
        if ((id >= COOP_PICKUP_FIRST && id < COOP_ACTOR_PICKUP_FIRST)
                || (id >= COOP_MOVE_FIRST && id < COOP_TRAINING_FIRST)
                || (id >= COOP_TROFF_FIRST && id < COOP_TROFF_END)) { CHECK(f == -1); continue; }
        CHECK(f >= 0 && f < 0x400 && !seen[f]++); CHECK(coop_item_id(f) == int(id));
    }
    CHECK(coop_item_flag(COOP_ITEMS) == -1 && coop_item_flag(~0u) == -1);
    for (int flag : {-1, 0x17A, 0x26B, 0x400})
        CHECK(coop_item_id(flag) == -1);
    CoopItems g{}; coop_items_prepare(&g, 1, 1, 0);
    coop_items_observe(&g, 1, 0x1D5, 1, 0, 0, 1, 0); CHECK(!any(g.input.request));
    CoopItemResult ready{}; ready.status = 3; ready.session_lo = 123; ready.scope = 2;
    coop_items_receive(&g, ready);
    for (unsigned id = 0; id < COOP_PICKUP_FIRST; ++id) {
        coop_items_observe(&g, 1, coop_item_flag(id), 1, 0, 0, 1, 0);
        CHECK(g.input.request[id / 32] & bit(id));
    }
    coop_items_observe(&g, 1, 0x01D, 1, 0, 0, 1, 0);
    CHECK(coop_item_has(g.input.request, COOP_JAPES_BOULDER_BUNCH));
    coop_items_prepare(&g, 1, 0, 0); CHECK(any(g.input.request)); // Map loading retains pending events.
    ready.status = 1; coop_items_receive(&g, ready); CHECK(any(g.input.request)); // Short network gap.
    ready.session_lo++; coop_items_receive(&g, ready); CHECK(!any(g.input.request));
    ready.status = 2; coop_items_receive(&g, ready); coop_items_prepare(&g, 1, 1, 0);
    for (unsigned type : {1u, 2u}) coop_items_observe(&g, 1, 0x1D5, 1, type, 0, 1, 0);
    coop_items_observe(&g, 0, 0x1D5, 1, 0, 0, 1, 0);
    coop_items_observe(&g, 1, 0x1D5, 0, 0, 0, 1, 0);
    coop_items_observe(&g, 1, 0x1D5, 1, 0, 1, 1, 0);
    coop_items_observe(&g, 1, 0x1D5, 1, 0, 0, 0, 0);
    coop_items_observe(&g, 1, 0x1D5, 1, 0, 0, 1, 1);
    g.applying = 1; coop_items_observe(&g, 1, 0x1D5, 1, 0, 0, 1, 0); g.applying = 0;
    CHECK(!any(g.input.request));
    coop_items_observe(&g, 1, 0x1D5, 1, 0, 0, 1, 0); CHECK(g.input.request[0] == 1);
    g.input.owned[0] = 1;
    ItemWire r{1, 2, 1, 2};
    CHECK(items_wire(false, g.input, r, true, 124).request[0] == 1);
    CHECK(!any(items_wire(false, g.input, r, true, 125).request));
    CHECK(!any(items_wire(false, g.input, r, false, 124).request));
    CHECK(!any(items_wire(true, g.input, r, true, 124).request));
    g.input.owned[0] = 0; CHECK(!any(items_wire(false, g.input, r, true, 124).request));
    g.save_pending = g.refresh_pending = 1; coop_items_prepare(&g, 1, 1, 1);
    CHECK(g.file_changed && !g.input.enabled && !g.save_pending && !g.refresh_pending && !any(g.input.request));
    coop_items_prepare(&g, 1, 1, 0); CHECK(!g.input.enabled);
}
static void engine_checks() {
    CoopItems g{}; current_game = &g;
    coop_items_capture(&g, 1, 1, 0); CHECK(!any(g.input.owned));
    g.result.status = 2; g.result.apply[0] = g.result.apply[1] = ~0u; g.result.apply[2] = 0xFFFF;
    block_write = 1; coop_items_apply(&g, 1); CHECK(writes == 80 && saves == 0);
    block_write = 0; coop_items_apply(&g, 0); CHECK(writes == 160 && saves == 0 && g.save_pending);
    CHECK(!any(g.input.request)); // No echo of a remote application.
    coop_items_apply(&g, 1); CHECK(writes == 160 && saves == 1 && !g.save_pending);
    coop_items_apply(&g, 1); CHECK(writes == 160 && saves == 1);
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.owned[0] == ~0u && g.input.owned[1] == ~0u && g.input.owned[2] == 0xFFFF);
    for (unsigned flag = 0; flag < 0x400; ++flag) CHECK(flags[flag] == (coop_item_id(flag) >= 0 && coop_item_id(flag) < 80 ? 1u : 0u));
    flags[0x1D5] = 0; g.result.status = 4; coop_items_apply(&g, 1); CHECK(!flags[0x1D5]);
    g.result.status = 2; coop_items_capture(&g, 1, 0, 0); coop_items_apply(&g, 1); CHECK(!flags[0x1D5]);
    coop_items_capture(&g, 1, 1, 1); coop_items_apply(&g, 1); CHECK(!flags[0x1D5]);
}
static void reset_engine() {
    std::memset(flags, 0, sizeof(flags));
    std::memset(pickups, 0, sizeof(pickups));
    D_global_asm_807FD730 = nullptr; mock_level = 7; mock_layout_error = 0;
    // Sentinel all unrelated inventory, unused levels, extra Kong and other players.
    std::memset(D_global_asm_807FC950, 0xA5, sizeof(D_global_asm_807FC950));
    for (unsigned k = 0; k < 5; ++k) for (unsigned l = 0; l < 8; ++l) {
        auto& p = D_global_asm_807FC950[0].character_progress[k];
        p.golden_bananas[l] = p.coloured_bananas[l] = p.coloured_bananas_fed_to_tns[l] = p.coins = 0;
    }
    for (unsigned k = 0; k < 5; ++k) {
        auto& p = D_global_asm_807FC950[0].character_progress[k];
        for (unsigned f = 0; f < 5; ++f) p.progression[f] = 0;
    }
    D_global_asm_807FC950[0].melons = 1;
    current_map = 7;
    for (auto& object : D_global_asm_807F6240) object = -1;
    std::memset(live_raw, 0, sizeof(live_raw));
    writes = saves = hud_updates = block_write = mutate_counter = live_calls = live_slot = live_state = 0;
    live_reveals = live_reveal_object = 0;
    live_mermaid_available = live_mermaid_refreshes = 0;
    live_mermaid_state = 30; live_mermaid_progress = 0;
    live_trombone_notifications = live_trombone_mode = 0;
    D_global_asm_80754280 = &hud_object;
}
static unsigned main_map_for_level(unsigned level) {
    static const unsigned maps[8] = {7, 38, 26, 30, 48, 72, 87, 34};
    CHECK(level < 8); return maps[level];
}
static void snide_and_medal_checks() {
    for (unsigned i = 0; i < 40; ++i) {
        reset_engine(); CoopItems g{}; current_game = &g;
        unsigned id = COOP_SNIDE_FIRST + i, kong = i % 5, level = i / 5;
        flags[0x1D5 + i] = 1;
        auto& count = D_global_asm_807FC950[0].character_progress[kong].golden_bananas[level];
        count = i % 4; seed_gbs(level, kong, count);
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        ++expected[0].character_progress[kong].golden_bananas[level];
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && !g.counter_error);
        g.result.status = 2; g.result.apply[id / 32] = bit(id);
        coop_items_apply(&g, 0); CHECK(!writes && !saves && !flags[0x1FD + i]);
        D_global_asm_80754280 = nullptr;
        coop_items_apply(&g, 1); CHECK(!writes && !saves); // HUD not initialized yet.
        D_global_asm_80754280 = &hud_object;
        block_write = 1; coop_items_apply(&g, 1);
        CHECK(writes == 1 && count == i % 4 && !saves && !hud_updates); // Canceled flag cannot award GB.
        block_write = 0; coop_items_apply(&g, 1);
        CHECK(flags[0x1FD + i] && writes == 2 && saves == 1 && hud_updates == 1);
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        CHECK(!any(g.input.request));
        coop_items_apply(&g, 1); coop_items_capture(&g, 1, 1, 0);
        CHECK(coop_item_has(g.input.owned, id) && writes == 2 && saves == 1 && hud_updates == 1);
        // The actual Snide menu guards each local increment with the same flag.
        if (!flags[0x1FD + i]) { setFlag(0x1FD + i, 1, 0); ++count; }
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);

        // Local collection wins the race: subsequent remote delivery cannot credit again.
        reset_engine(); g = {}; flags[0x1D5 + i] = 1;
        coop_items_capture(&g, 1, 1, 0);
        CoopItemResult r{}; r.status = 3; r.session_lo = 123; r.scope = 1;
        coop_items_receive(&g, r);
        setFlag(0x1FD + i, 1, 0); ++count; // Verified original menu flag-then-increment sequence.
        CHECK(coop_item_has(g.input.request, id));
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && coop_item_has(g.input.owned, id));
        g.result.apply[id / 32] = bit(id); coop_items_apply(&g, 1);
        CHECK(count == 1 && writes == 1 && !hud_updates);
    }
    reset_engine(); CoopItems g{}; current_game = &g;
    MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
    coop_items_capture(&g, 1, 1, 0); g.result.status = 2;
    for (unsigned id = COOP_MEDAL_FIRST; id < COOP_GB_FIRST; ++id) g.result.apply[id / 32] |= bit(id);
    coop_items_apply(&g, 1); CHECK(writes == 40 && saves == 1 && !hud_updates);
    for (unsigned i = 0; i < 40; ++i) CHECK(flags[0x225 + i]);
    CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0); // No colored banana or GB award.
    coop_items_apply(&g, 1); CHECK(writes == 40 && saves == 1);

    // Missing dependency, counter overflow, inconsistent loaded saves and external writers.
    reset_engine(); g = {}; coop_items_capture(&g, 1, 1, 0);
    g.result.status = 2; g.result.apply[2] = 1u << 16;
    coop_items_apply(&g, 1); CHECK(!writes && !saves); // Blueprint must be actually applied first.
    flags[0x1D5] = 1;
    D_global_asm_807FC950[0].character_progress[0].golden_bananas[0] = 5;
    coop_items_apply(&g, 1); CHECK(g.counter_error && !g.input.ready && !writes && !flags[0x1FD]);
    D_global_asm_807FC950[0].character_progress[0].golden_bananas[0] = 0;
    coop_items_capture(&g, 1, 1, 0); CHECK(!g.input.ready); // Error is latched until restart.
    for (unsigned scenario = 0; scenario < 3; ++scenario) {
        reset_engine(); g = {}; flags[0x1FD] = 1; flags[0x1D5] = scenario != 2;
        D_global_asm_807FC950[0].character_progress[0].golden_bananas[0] = scenario == 0 ? 0 : scenario == 1 ? 6 : 1;
        coop_items_capture(&g, 1, 1, 0); CHECK(g.counter_error && !g.input.ready && !writes && !saves);
    }
    reset_engine(); g = {}; flags[0x1D5] = 1; coop_items_capture(&g, 1, 1, 0);
    g.result.status = 2; g.result.apply[2] = 1u << 16; mutate_counter = 1;
    coop_items_apply(&g, 1);
    CHECK(g.counter_error && D_global_asm_807FC950[0].character_progress[0].golden_bananas[0] == 1 && !saves);

    // Tiny's sixth Isles GB is legal; every other bucket must stay within five.
    reset_engine(); g = {}; flags[0x1D5 + 38] = 1;
    D_global_asm_807FC950[0].character_progress[3].golden_bananas[7] = 5; seed_gbs(7, 3, 5);
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
    g.result.status = 2; g.result.apply[118 / 32] = bit(118); coop_items_apply(&g, 1);
    CHECK(!g.counter_error && D_global_asm_807FC950[0].character_progress[3].golden_bananas[7] == 6);
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
}
static void remaining_collectible_checks() {
    unsigned bucket[8][5]{};
    for (unsigned id = COOP_GB_FIRST; id < COOP_PICKUP_FIRST; ++id) {
        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        unsigned l, k; CHECK(coop_item_gb(id, &l, &k)); ++bucket[l][k];
        current_map = main_map_for_level(l); mock_level = l;
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
        g.result.status = 2; g.result.apply[id / 32] = bit(id);
        if (!coop_gb_same_level_safe(id, current_map)) {
            coop_items_apply(&g, 1); CHECK(!writes && !saves); // Loaded old GB remains local.
        }
        mock_level = (l + 1) % 8;
        D_global_asm_807FD730 = &hud_object; coop_items_apply(&g, 1); CHECK(!writes);
        D_global_asm_807FD730 = nullptr;
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        expected[0].character_progress[k].golden_bananas[l] = 1;
        block_write = 1; coop_items_apply(&g, 1); CHECK(writes == 1 && !saves);
        block_write = 0; coop_items_apply(&g, 1); coop_items_apply(&g, 1);
        CHECK(writes == 2 && saves == 1 && hud_updates == 1 && flags[coop_item_flag(id)]);
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && !any(g.input.request));
    }
    for (unsigned l = 0; l < 8; ++l) for (unsigned k = 0; k < 5; ++k)
        CHECK(bucket[l][k] + 1 == coop_snide_capacity(l, k));
    unsigned coins = 0, singles = 0, bunches = 0;
    for (unsigned i = 0; i < 1700; ++i) {
        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        unsigned id = COOP_PICKUP_FIRST + i, l = coop_pickup_level(i), k = coop_pickups[i].kong;
        unsigned amount = coop_pickups[i].amount;
        CHECK(k < 5 && l < 8 && (amount == 0 || amount == 1 || amount == 5));
        if (!amount) ++coins; else if (amount == 1) ++singles; else ++bunches;
        auto& p = D_global_asm_807FC950[0].character_progress[k];
        // Different local shopping/Troff balances must not be overwritten.
        p.coins = 9; p.coloured_bananas[l] = 20; p.coloured_bananas_fed_to_tns[l] = 15;
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        if (amount) expected[0].character_progress[k].coloured_bananas[l] += amount;
        else ++expected[0].character_progress[k].coins;
        mock_level = l; coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && g.baseline);
        CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 123;
        r.apply[id / 32] = bit(id); coop_items_receive(&g, r);
        coop_items_apply(&g, 1); CHECK(!writes && !pickups[i]);
        mock_level = (l + 1) % 8;
        block_write = 1; coop_items_apply(&g, 1); CHECK(writes == 1 && !saves && !pickups[i]);
        block_write = 0; coop_items_apply(&g, 1); coop_items_apply(&g, 1);
        CHECK(writes == 2 && saves == 1 && pickups[i] && !any(g.input.request));
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && !any(g.input.request));
        // Queue-bit-before-credit ordering: no request/readback until drained.
        reset_engine(); g = {}; g.join = 1; coop_items_capture(&g, 1, 1, 0);
        coop_items_receive(&g, r); pickups[i] = 1; D_global_asm_807FD730 = &hud_object;
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && g.deferred && !any(g.input.request));
        if (amount) p.coloured_bananas[l] += amount; else ++p.coins;
        D_global_asm_807FD730 = nullptr; coop_items_capture(&g, 1, 1, 0);
        CHECK(g.input.ready && coop_item_has(g.input.request, id));
        coop_items_apply(&g, 1); CHECK(!writes && !saves); // Local pickup won race.
    }
    CHECK(coins == 574 && singles == 793 && bunches == 333);
    // Remote colored bananas trigger the correct medal, preserve fed CBs, and
    // publish a newly derived medal even when local-only balloons differ.
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
    unsigned i = 0; while (!(coop_pickups[i].amount == 5 && coop_pickups[i].kong == 0)) ++i;
    unsigned l = coop_pickup_level(i), id = COOP_PICKUP_FIRST + i;
    auto& p = D_global_asm_807FC950[0].character_progress[0];
    p.coloured_bananas[l] = 20; p.coloured_bananas_fed_to_tns[l] = 51;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
    CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 123; r.apply[id/32] = bit(id);
    coop_items_receive(&g, r); coop_items_apply(&g, 1);
    CHECK(p.coloured_bananas[l] == 25 && p.coloured_bananas_fed_to_tns[l] == 51);
    CHECK(flags[0x225 + l*5] && coop_item_has(g.input.request, COOP_MEDAL_FIRST + l*5));
    CHECK(!coop_item_has(g.input.request, id));
    reset_engine(); g = {}; p.coloured_bananas[l] = 97;
    coop_items_capture(&g, 1, 1, 0); coop_items_receive(&g, r); coop_items_apply(&g, 1);
    CHECK(g.counter_error && !writes && !pickups[i] && p.coloured_bananas[l] == 97);
    // Changed setup lengths fail closed before reading a different save layout.
    reset_engine(); g = {}; mock_layout_error = 1; coop_items_capture(&g, 1, 1, 0);
    CHECK(g.counter_error && !g.input.ready && !writes);
    reset_engine(); g = {}; p.coins = 1000; coop_items_capture(&g, 1, 1, 0);
    CHECK(g.counter_error && !g.input.ready && !writes);
}
static void actor_collectible_checks() {
    unsigned balloons = 0, rainbows = 0;
    for (unsigned i = 0; i < 120; ++i) {
        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        auto a = coop_actor_pickups[i]; unsigned id = COOP_ACTOR_PICKUP_FIRST + i;
        if (a.amount) ++balloons; else ++rainbows;
        for (unsigned k = 0; k < 5; ++k) D_global_asm_807FC950[0].character_progress[k].coins = k * 7;
        auto& p = D_global_asm_807FC950[0].character_progress[a.kong];
        p.coloured_bananas[a.level] = 21; p.coloured_bananas_fed_to_tns[a.level] = 12;
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        if (a.amount) expected[0].character_progress[a.kong].coloured_bananas[a.level] += 10;
        else for (unsigned k = 0; k < 5; ++k) expected[0].character_progress[k].coins += 5;
        mock_level = a.level; coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
        CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 123; r.apply[id / 32] = bit(id);
        coop_items_receive(&g, r); coop_items_apply(&g, 1); CHECK(!writes && !saves);
        mock_level = (a.level + 1) % 8;
        block_write = 1; coop_items_apply(&g, 1); CHECK(writes == 1 && !saves);
        block_write = 0; coop_items_apply(&g, 1); coop_items_apply(&g, 1);
        CHECK(writes == 2 && saves == 1 && flags[a.flag]);
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && !any(g.input.request));
        // Local flag written during queued pickups must still become a request.
        reset_engine(); g = {}; g.join = 1; coop_items_capture(&g, 1, 1, 0); coop_items_receive(&g, r);
        D_global_asm_807FD730 = &hud_object; coop_items_capture(&g, 1, 1, 0);
        r.status = 1; coop_items_receive(&g, r); setFlag(a.flag, 1, 0);
        if (a.amount) p.coloured_bananas[a.level] += 10;
        else for (unsigned k = 0; k < 5; ++k) D_global_asm_807FC950[0].character_progress[k].coins += 5;
        D_global_asm_807FD730 = nullptr; coop_items_capture(&g, 1, 1, 0);
        CHECK(coop_item_has(g.input.request, id));
    }
    CHECK(balloons == 104 && rainbows == 16);
    // Check the fifth balance before any of the first four are credited.
    reset_engine(); CoopItems g{}; current_game = &g;
    unsigned i = 0; while (coop_actor_pickups[i].amount) ++i;
    auto a = coop_actor_pickups[i]; unsigned id = COOP_ACTOR_PICKUP_FIRST + i;
    mock_level = (a.level + 1) % 8;
    D_global_asm_807FC950[0].character_progress[4].coins = 995;
    MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
    g.result.status = 2; g.result.apply[id / 32] = bit(id); coop_items_apply(&g, 1);
    CHECK(g.counter_error && !writes && !saves && !flags[a.flag]);
    CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
}
static void same_level_gb_delivery_checks() {
    // These exact rewards originate in an unloaded lobby/interior while the
    // receiver is on the owning main map. No source reward controller remains
    // loaded, so their flag and counter may be committed atomically in place.
    for (unsigned row = 0; row < COOP_SAME_LEVEL_GB_COUNT; ++row) {
        int found = coop_item_id(coop_same_level_gbs[row].flag);
        CHECK(found >= (int)COOP_GB_FIRST && found < (int)COOP_PICKUP_FIRST);
        unsigned id = (unsigned)found, level = 0, kong = 0;
        CHECK(coop_item_gb(id, &level, &kong));
        CHECK(main_map_for_level(level) == coop_same_level_gbs[row].map);
        CHECK(coop_gb_same_level_safe(id, current_map = coop_same_level_gbs[row].map));

        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        current_map = coop_same_level_gbs[row].map; mock_level = level;
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
        g.result.status = 2; g.result.apply[id / 32] = bit(id);
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        expected[0].character_progress[kong].golden_bananas[level] = 1;
        coop_items_apply(&g, 1);
        CHECK(writes == 1 && saves == 1 && hud_updates == 1 && flags[coop_item_flag(id)]);
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        coop_items_apply(&g, 1); CHECK(writes == 1 && saves == 1 && hud_updates == 1);
    }

    // Every non-reviewed same-level GB remains deferred. This includes
    // same-map spawners and reward props such as Seal Race and Baboon Blast.
    for (unsigned id = COOP_GB_FIRST; id < COOP_PICKUP_FIRST; ++id) {
        unsigned level = 0, kong = 0; CHECK(coop_item_gb(id, &level, &kong));
        unsigned map = main_map_for_level(level);
        if (coop_gb_same_level_safe(id, map)) continue;
        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        current_map = map; mock_level = level; coop_items_capture(&g, 1, 1, 0);
        g.result.status = 2; g.result.apply[id / 32] = bit(id);
        coop_items_apply(&g, 1);
        CHECK(!writes && !saves && !hud_updates && !flags[coop_item_flag(id)]);
        CHECK(g.wait_reason == COOP_TRACE_WAIT_SAME_LEVEL_ITEM && g.wait_id == id);
    }

    // The new allowlist never overrides existing save, HUD, reward-queue, or
    // exact-map guards, and a failed flag write retries without counter drift.
    unsigned id = (unsigned)coop_item_id(0x018), level = 0, kong = 0;
    CHECK(coop_item_gb(id, &level, &kong));
    for (unsigned scenario = 0; scenario < 4; ++scenario) {
        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        current_map = scenario == 3 ? 38 : 7; mock_level = level;
        coop_items_capture(&g, 1, 1, 0); g.result.status = 2;
        g.result.apply[id / 32] = bit(id);
        if (scenario == 1) D_global_asm_80754280 = nullptr;
        if (scenario == 2) D_global_asm_807FD730 = &hud_object;
        coop_items_apply(&g, scenario != 0);
        CHECK(!writes && !saves && !hud_updates && !flags[0x018]);
    }
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
    current_map = 7; mock_level = 0; coop_items_capture(&g, 1, 1, 0);
    g.result.status = 2; g.result.apply[id / 32] = bit(id); block_write = 1;
    coop_items_apply(&g, 1); CHECK(writes == 1 && !saves && !flags[0x018]);
    CHECK(D_global_asm_807FC950[0].character_progress[kong].golden_bananas[level] == 0);
    block_write = 0; coop_items_apply(&g, 1);
    CHECK(writes == 2 && saves == 1 && flags[0x018]);
    CHECK(D_global_asm_807FC950[0].character_progress[kong].golden_bananas[level] == 1);
}
static void japes_boulder_bunch_checks() {
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
    current_map = 38; mock_level = 1; // Outside Japes: no local boulder/drop actor is loaded.
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && !coop_item_owned(COOP_JAPES_BOULDER_BUNCH));
    CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 321;
    r.apply[COOP_JAPES_BOULDER_BUNCH / 32] = bit(COOP_JAPES_BOULDER_BUNCH);
    coop_items_receive(&g, r);
    block_write = 1; coop_items_apply(&g, 1);
    CHECK(writes == 1 && !flags[0x01D] && !saves
        && D_global_asm_807FC950[0].character_progress[4].coloured_bananas[0] == 0);
    block_write = 0; coop_items_apply(&g, 1);
    CHECK(writes == 2 && flags[0x01D] && saves == 1
        && D_global_asm_807FC950[0].character_progress[4].coloured_bananas[0] == 5
        && !any(g.input.request));
    coop_items_capture(&g, 1, 1, 0);
    CHECK(coop_item_owned(COOP_JAPES_BOULDER_BUNCH)
        && coop_item_has(g.input.owned, COOP_JAPES_BOULDER_BUNCH));

    // The vanilla actor writes flag 0x01D before its queued five-banana credit.
    // Observe it once, then let the queue drain; subsequent remote delivery is a no-op.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 7; mock_level = 0; coop_items_capture(&g, 1, 1, 0);
    CoopItemResult ready{}; ready.status = 3; ready.scope = 1; ready.session_lo = 322;
    coop_items_receive(&g, ready);
    setFlag(0x01D, 1, 0);
    D_global_asm_807FC950[0].character_progress[4].coloured_bananas[0] = 5;
    CHECK(coop_item_has(g.input.request, COOP_JAPES_BOULDER_BUNCH));
    coop_items_capture(&g, 1, 1, 0);
    g.result.status = 2;
    g.result.apply[COOP_JAPES_BOULDER_BUNCH / 32] = bit(COOP_JAPES_BOULDER_BUNCH);
    coop_items_apply(&g, 1);
    CHECK(writes == 1 && saves == 0
        && D_global_asm_807FC950[0].character_progress[4].coloured_bananas[0] == 5);
}
static void seed_krool_route() {
    flags[coop_item_flag(COOP_LOBBY)] = 1;
    for (unsigned i = 0; i < 8; ++i) {
        flags[coop_item_flag(70 + i)] = 1;
        flags[coop_item_flag(COOP_KEY_TURN_FIRST + i)] = 1;
    }
    flags[coop_item_flag(COOP_KLUMSY_FREE)] = 1;
}
static void krool_completion_checks() {
    // The terminal completion bit is accepted only after the complete vanilla
    // route: all keys, their K. Lumsy turn-ins, the lobby and K. Lumsy freed.
    unsigned owned[COOP_ITEM_WORDS]{};
    owned[COOP_KROOL_DEFEATED / 32] |= bit(COOP_KROOL_DEFEATED);
    CHECK(!coop_items_full_dependencies(owned));
    owned[COOP_LOBBY / 32] |= bit(COOP_LOBBY);
    for (unsigned i = 0; i < 8; ++i) {
        owned[(70 + i) / 32] |= bit(70 + i);
        owned[(COOP_KEY_TURN_FIRST + i) / 32] |= bit(COOP_KEY_TURN_FIRST + i);
    }
    owned[COOP_KLUMSY_FREE / 32] |= bit(COOP_KLUMSY_FREE);
    CHECK(coop_items_full_dependencies(owned));

    // A receiver writes and saves only the persistent completion flag. No
    // ending cutscene, transition, inventory counter or HUD path is invoked.
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1; seed_krool_route();
    current_map = 7; mock_level = 0;
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && !coop_item_owned(COOP_KROOL_DEFEATED));
    CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 323;
    r.apply[COOP_KROOL_DEFEATED / 32] = bit(COOP_KROOL_DEFEATED);
    coop_items_receive(&g, r);
    block_write = 1; coop_items_apply(&g, 1);
    CHECK(writes == 1 && !flags[0x1B0] && !saves && !hud_updates);
    block_write = 0; coop_items_apply(&g, 1);
    CHECK(writes == 2 && flags[0x1B0] && saves == 1 && !hud_updates && !any(g.input.request));
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && coop_item_owned(COOP_KROOL_DEFEATED)
        && coop_item_has(g.input.owned, COOP_KROOL_DEFEATED));

    // A real local victory observed first wins the race. A later network copy
    // neither rewrites nor resaves the already-owned completion flag.
    reset_engine(); g = {}; current_game = &g; g.join = 1; seed_krool_route();
    current_map = 7; mock_level = 0; coop_items_capture(&g, 1, 1, 0);
    CoopItemResult ready{}; ready.status = 3; ready.scope = 1; ready.session_lo = 324;
    coop_items_receive(&g, ready);
    setFlag(0x1B0, 1, 0);
    CHECK(coop_item_has(g.input.request, COOP_KROOL_DEFEATED));
    coop_items_capture(&g, 1, 1, 0);
    g.result.status = 2;
    g.result.apply[COOP_KROOL_DEFEATED / 32] = bit(COOP_KROOL_DEFEATED);
    coop_items_apply(&g, 1);
    CHECK(writes == 1 && saves == 0 && flags[0x1B0]);

    // A corrupt save that claims victory before the route is rejected and
    // remains latched fail-closed for the process lifetime.
    reset_engine(); g = {}; current_game = &g; flags[0x1B0] = 1;
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.counter_error && !g.input.ready && !writes && !saves);
}
static void seed_arcade_payment_route() {
    flags[0x081] = flags[0x082] = 1;
    D_global_asm_807FC950[0].character_progress[0].golden_bananas[2] = 1;
}
static void arcade_payment_checks() {
    unsigned owned[COOP_ITEM_WORDS]{};
    owned[COOP_ARCADE_COINS_PAID / 32] |= bit(COOP_ARCADE_COINS_PAID);
    CHECK(!coop_items_full_dependencies(owned));
    owned[COOP_FACTORY_ARCADE_LEVER / 32] |= bit(COOP_FACTORY_ARCADE_LEVER);
    CHECK(!coop_items_full_dependencies(owned));
    owned[(COOP_GB_FIRST + 54) / 32] |= bit(COOP_GB_FIRST + 54);
    CHECK(coop_items_full_dependencies(owned));

    // Factory and its Arcade overlay must unload before the persistent access
    // result changes. The receiver's coins are deliberately untouched.
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1; seed_arcade_payment_route();
    current_map = 26; mock_level = 2;
    D_global_asm_807FC950[0].character_progress[0].coins = 17;
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && !coop_item_owned(COOP_ARCADE_COINS_PAID));
    CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 325;
    r.apply[COOP_ARCADE_COINS_PAID / 32] = bit(COOP_ARCADE_COINS_PAID);
    coop_items_receive(&g, r); coop_items_apply(&g, 1);
    CHECK(!writes && !saves && !flags[0x083]
        && D_global_asm_807FC950[0].character_progress[0].coins == 17);
    current_map = 7; mock_level = 0;
    block_write = 1; coop_items_apply(&g, 1);
    CHECK(writes == 1 && !flags[0x083] && !saves);
    block_write = 0; coop_items_apply(&g, 1);
    CHECK(writes == 2 && flags[0x083] && saves == 1 && !hud_updates
        && D_global_asm_807FC950[0].character_progress[0].coins == 17
        && !any(g.input.request));
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && coop_item_has(g.input.owned, COOP_ARCADE_COINS_PAID));

    // A local payment observed first remains authoritative and is never charged
    // again when the matching remote bit arrives.
    reset_engine(); g = {}; current_game = &g; g.join = 1; seed_arcade_payment_route();
    current_map = 7; mock_level = 0;
    D_global_asm_807FC950[0].character_progress[0].coins = 11;
    coop_items_capture(&g, 1, 1, 0);
    CoopItemResult ready{}; ready.status = 3; ready.scope = 1; ready.session_lo = 326;
    coop_items_receive(&g, ready); setFlag(0x083, 1, 0);
    CHECK(coop_item_has(g.input.request, COOP_ARCADE_COINS_PAID));
    coop_items_capture(&g, 1, 1, 0);
    g.result.status = 2;
    g.result.apply[COOP_ARCADE_COINS_PAID / 32] = bit(COOP_ARCADE_COINS_PAID);
    coop_items_apply(&g, 1);
    CHECK(writes == 1 && saves == 0
        && D_global_asm_807FC950[0].character_progress[0].coins == 11);

    reset_engine(); g = {}; current_game = &g; flags[0x083] = 1;
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.counter_error && !g.input.ready && !writes && !saves);
}
static void world_refresh_checks() {
    // Default behavior preserves the old outside-level restriction.
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
    current_map = 38; mock_level = 1;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
    CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 901;
    r.apply[COOP_WORLD_FIRST / 32] = bit(COOP_WORLD_FIRST);
    coop_items_receive(&g, r); coop_items_apply(&g, 1);
    CHECK(!flags[0x32] && !writes && !saves && !g.refresh_pending);

    // Opting in writes and saves all verified flags first, then queues exactly
    // one vanilla same-map rebuild for the frame adapter to initiate.
    g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0x32] && writes == 1 && saves == 1 && g.refresh_pending && g.refresh_map == 38);
    coop_items_apply(&g, 1); CHECK(writes == 1 && saves == 1 && g.refresh_pending);

    // Reviewed simple gates enter their pinned vanilla completed state live,
    // avoiding a map reload only when every affected object is loaded.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 7; mock_level = 0; D_global_asm_807F6240[4] = 0x1A; D_global_asm_807F6240[9] = 0x1B;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
    r = {}; r.status = 2; r.scope = 1; r.session_lo = 902;
    unsigned first_gate = (unsigned)COOP_WORLD_FIRST + (unsigned)COOP_WORLD_FIRST_GATE;
    r.apply[first_gate / 32] = bit(first_gate); coop_items_receive(&g, r);
    g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0] && writes == 1 && saves == 1 && live_calls == 2 && live_slot == 9
        && live_state == 20 && !g.refresh_pending);

    // A missing reviewed object fails closed into the existing reload fallback.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 7; mock_level = 0; D_global_asm_807F6240[4] = 0x1A;
    coop_items_capture(&g, 1, 1, 0); coop_items_receive(&g, r);
    g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0] && live_calls == 0 && g.refresh_pending && g.refresh_map == 7);

    // The full Japes shell gate and each hut's switch plus door use the exact
    // flag-positive initializer states audited from the pinned US scripts.
    reset_engine(); current_map = 7;
    D_global_asm_807F6240[1] = 0x34; D_global_asm_807F6240[2] = 0x35;
    D_global_asm_807F6240[3] = 0x33;
    CHECK(coop_live_world_refresh(0x007) && live_calls == 3 && live_state == 20);
    live_calls = 0; D_global_asm_807F6240[1] = 0x38; D_global_asm_807F6240[2] = 0x41;
    D_global_asm_807F6240[3] = -1;
    CHECK(coop_live_world_refresh(0x00D) && live_calls == 2 && live_state == 7);

    // Factory's revealed arcade lever uses its exact flag-positive state and
    // no longer requires a map rebuild.
    reset_engine(); current_map = 26; D_global_asm_807F6240[6] = 0x2D;
    CHECK(coop_live_world_refresh(0x081) && live_calls == 1 && live_state == 20);

    // Every main-level boss portal component enters the same terminal vanilla
    // state. The saved flag initializes any separate-map consumers on entry.
    struct PortalCase { unsigned map, flag, count; unsigned short object[5]; };
    const PortalCase portals[] = {
        { 7, 0x02E, 3, {0x02C, 0x037, 0x11A}},
        {38, 0x06C, 5, {0x008, 0x009, 0x00A, 0x00B, 0x0EC}},
        {26, 0x098, 5, {0x046, 0x047, 0x048, 0x049, 0x04A}},
        {30, 0x0CB, 5, {0x022, 0x023, 0x024, 0x025, 0x026}},
        {48, 0x102, 5, {0x015, 0x016, 0x03B, 0x047, 0x052}},
        {72, 0x12E, 4, {0x023, 0x024, 0x025, 0x026}},
        {87, 0x160, 3, {0x00B, 0x00C, 0x00D}},
    };
    CHECK(COOP_LIVE_WORLD_STATE_COUNT == 179);
    CHECK(coop_live_world_transient_eligible(&coop_live_world_states[0]));
    for (const auto& portal : portals) {
        reset_engine(); current_map = portal.map;
        for (unsigned i = 0; i < portal.count; ++i)
            D_global_asm_807F6240[20 + i] = (short)portal.object[i];
        CHECK(coop_live_world_refresh(portal.flag) && live_calls == portal.count
            && live_slot == 19 + portal.count && live_state == 20);
    }
    CHECK(!coop_live_world_transient_eligible(&coop_live_world_states[43]));

    // Permanent breakables replay their exact flag-positive initializer and
    // never become transient host-owned scripts.
    reset_engine(); current_map = 72; D_global_asm_807F6240[2] = 0x1F;
    CHECK(coop_live_world_refresh(0x109) && live_calls == 1 && live_state == 0);
    CHECK(!coop_live_world_transient_eligible(&coop_live_world_states[85]));

    // Factory storage's three box scripts replay as one atomic completion.
    reset_engine(); current_map = 26;
    D_global_asm_807F6240[1] = 0x61; D_global_asm_807F6240[2] = 0x62;
    D_global_asm_807F6240[3] = 0x77;
    CHECK(coop_live_world_refresh(0x078) && live_calls == 3 && live_state == 0);
    CHECK(!coop_live_world_transient_eligible(&coop_live_world_states[104]));

    // The front-mill box requires its linked controller but does not reset it;
    // the box initializer itself wakes that controller in vanilla.
    reset_engine(); current_map = 61; D_global_asm_807F6240[1] = 0x06;
    CHECK(!coop_live_world_refresh(0x0DA) && live_calls == 0);
    D_global_asm_807F6240[2] = 0x02;
    CHECK(coop_live_world_refresh(0x0DA) && live_calls == 1 && live_state == 0);

    // Receiving that interior-only flag in Fungi's main map is an audited
    // no-op, avoiding a reload that cannot affect any loaded script.
    reset_engine(); current_map = 48;
    CHECK(coop_live_world_refresh(0x0DA) && live_calls == 0);

    // Factory production validates all thirteen child scripts before waking
    // only the two audited controllers.
    reset_engine(); current_map = 26;
    D_global_asm_807F6240[0] = 0x000; D_global_asm_807F6240[1] = 0x107;
    for (unsigned object = 1; object <= 0x0D; ++object)
        D_global_asm_807F6240[object + 1] = (short)object;
    CHECK(coop_live_world_refresh(0x06F) && live_calls == 2 && live_state == 0);
    CHECK(!coop_live_world_transient_eligible(&coop_live_world_states[122]));

    // Aztec lobby applies the accepted permanent panel flag through both
    // vanilla saved-complete initializers as one loaded-map transaction.
    reset_engine(); current_map = 173;
    D_global_asm_807F6240[1] = 0x10; D_global_asm_807F6240[2] = 0x0F;
    CHECK(coop_live_world_refresh(0x18F) && live_calls == 2
        && live_slot == 2 && live_state == 0);
    reset_engine(); current_map = 173; D_global_asm_807F6240[1] = 0x10;
    CHECK(!coop_live_world_refresh(0x18F) && live_calls == 0);

    // Galleon lobby completes its slam switch and door through their saved
    // flag branches, with the pair preflighted before either script changes.
    reset_engine(); current_map = 174;
    D_global_asm_807F6240[1] = 0x0A; D_global_asm_807F6240[2] = 0x0C;
    CHECK(coop_live_world_refresh(0x191) && live_calls == 2
        && live_slot == 2 && live_state == 0);
    reset_engine(); current_map = 174; D_global_asm_807F6240[1] = 0x0C;
    CHECK(!coop_live_world_refresh(0x191) && live_calls == 0);

    // Helm lobby must actively undo its incomplete bridge visibility before
    // replaying the coconut switch's saved-complete initializer.
    reset_engine(); current_map = 170;
    D_global_asm_807F6240[1] = 0x00; D_global_asm_807F6240[2] = 0x02;
    CHECK(coop_live_world_refresh(0x197) && live_calls == 2
        && live_slot == 2 && live_state == 0);
    reset_engine(); current_map = 170; D_global_asm_807F6240[1] = 0x02;
    CHECK(!coop_live_world_refresh(0x197) && live_calls == 0);

    // The Isles boulder applies both exact terminal controller states and the
    // inverse of object 0x56's visibility gate as one reviewed loaded unit.
    reset_engine(); current_map = 34;
    D_global_asm_807F6240[1] = 0x3C; D_global_asm_807F6240[2] = 0x00;
    D_global_asm_807F6240[3] = 0x56;
    CHECK(coop_live_world_refresh(0x1AE) && live_calls == 2 && live_state == 100);
    CHECK(live_reveals == 1 && live_reveal_object == 0x56);
    reset_engine(); current_map = 34;
    D_global_asm_807F6240[1] = 0x3C; D_global_asm_807F6240[2] = 0x00;
    CHECK(!coop_live_world_refresh(0x1AE) && live_calls == 0 && live_reveals == 0);

    // Waiting Galleon controllers run their saved-complete initializer. Active
    // vanilla activation and steady interaction states continue untouched.
    reset_engine(); current_map = 30;
    D_global_asm_807F6240[2] = 0x27; D_global_asm_807F6240[3] = 0x51;
    CHECK(coop_live_world_refresh(0x09C) && live_calls == 2 && live_state == 0);
    reset_engine(); current_map = 30;
    D_global_asm_807F6240[2] = 0x27; D_global_asm_807F6240[3] = 0x51;
    live_raw[0x27] = 10; live_raw[0x51] = 10;
    CHECK(coop_live_world_refresh(0x09C) && live_calls == 0);
    live_raw[0x27] = 15; live_raw[0x51] = 14;
    CHECK(coop_live_world_refresh(0x09C) && live_calls == 0);
    live_raw[0x27] = 26; live_raw[0x51] = 0;
    CHECK(coop_live_world_refresh(0x09C) && live_calls == 1 && live_slot == 3);

    // Unknown script states still fail closed before either controller changes.
    reset_engine(); current_map = 30;
    D_global_asm_807F6240[2] = 0x27; D_global_asm_807F6240[3] = 0x51;
    live_raw[0x27] = 9;
    CHECK(!coop_live_world_refresh(0x09C) && live_calls == 0);
    live_raw[0x27] = 10; live_raw[0x51] = 15;
    CHECK(!coop_live_world_refresh(0x09C) && live_calls == 0);

    // Each remote pearl removes only its matching treasure-chest prop through
    // that prop's saved-complete initializer.
    for (unsigned pearl = 0; pearl < 5; ++pearl) {
        reset_engine(); current_map = 44;
        unsigned flag = 0xBA + pearl;
        flags[flag] = 1;
        D_global_asm_807F6240[10 + pearl] = (short)pearl;
        CHECK(coop_live_world_refresh(flag) && live_calls == 1
            && live_slot == 10 + pearl && live_state == 0);
    }
    reset_engine(); current_map = 44; flags[0xBC] = 1;
    CHECK(!coop_live_world_refresh(0xBC) && live_calls == 0);

    // The Mermaid actor follows its vanilla partial and all-five entry states.
    // Active final-reward and completed states are accepted without rewinding.
    reset_engine(); current_map = 45; live_mermaid_available = 1;
    live_mermaid_progress = 7; flags[0xBA] = 1;
    CHECK(coop_live_world_refresh(0xBA) && live_mermaid_state == 31
        && live_mermaid_progress == 0 && live_mermaid_refreshes == 1);
    flags[0xBB] = flags[0xBC] = flags[0xBD] = flags[0xBE] = 1;
    CHECK(coop_live_world_refresh(0xBE) && live_mermaid_state == 39
        && live_mermaid_refreshes == 2);
    live_mermaid_progress = 9;
    CHECK(coop_live_world_refresh(0xBD) && live_mermaid_state == 39
        && live_mermaid_progress == 9 && live_mermaid_refreshes == 2);
    live_mermaid_state = 32; live_mermaid_progress = 4;
    CHECK(coop_live_world_refresh(0xBC) && live_mermaid_state == 32
        && live_mermaid_progress == 4 && live_mermaid_refreshes == 2);
    live_mermaid_state = 33;
    CHECK(!coop_live_world_refresh(0xBB));
    live_mermaid_state = 30; live_mermaid_available = 0;
    CHECK(!coop_live_world_refresh(0xBB));

    // Applying the network item writes the pearl first, then refreshes the
    // loaded Mermaid on the same safe frame without scheduling a reload.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 45; mock_level = 3; live_mermaid_available = 1;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && g.deferred);
    r = {}; r.status = 2; r.scope = 1; r.session_lo = 905;
    unsigned pearl_one = (unsigned)coop_item_id(0xBA);
    CHECK(pearl_one < COOP_ITEMS);
    r.apply[pearl_one / 32] = bit(pearl_one);
    coop_items_receive(&g, r); g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0xBA] && writes == 1 && saves == 1 && live_mermaid_state == 31
        && live_mermaid_refreshes == 1 && !g.refresh_pending);

    // Helm and Helm-lobby tags replay only their exact tagged-visibility
    // initializer. Each flag names one loaded Bananaport and no paired script.
    struct HelmTagCase { unsigned map, flag, object; };
    const HelmTagCase helm_tags[] = {
        {170, 0x1A1, 0x008}, {170, 0x1A2, 0x009},
        { 17, 0x305, 0x059}, { 17, 0x306, 0x058},
    };
    for (const auto& tag : helm_tags) {
        reset_engine(); current_map = tag.map; flags[tag.flag] = 1;
        D_global_asm_807F6240[6] = (short)tag.object;
        CHECK(coop_live_world_refresh(tag.flag) && live_calls == 1
            && live_slot == 6 && live_state == 0);
    }
    reset_engine(); current_map = 170; flags[0x1A1] = 1;
    CHECK(!coop_live_world_refresh(0x1A1) && live_calls == 0);

    // Helm lobby admits a verified snapshot only so its exact incoming tag can
    // update and save in place; unrelated grants remain deferred.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 170; mock_level = 7; D_global_asm_807F6240[3] = 0x08;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && g.deferred);
    r = {}; r.status = 2; r.scope = 1; r.session_lo = 906;
    unsigned helm_lobby_tag = (unsigned)coop_item_id(0x1A1);
    CHECK(helm_lobby_tag < COOP_ITEMS);
    r.apply[helm_lobby_tag / 32] = bit(helm_lobby_tag);
    coop_items_receive(&g, r); g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0x1A1] && writes == 1 && saves == 1 && live_calls == 1
        && live_slot == 3 && live_state == 0 && !g.refresh_pending);

    // A remotely completed Isles trombone pad sends only the stock rocket-
    // barrel spawner notification, enters terminal state 6 and disables the
    // pad. All three idle states are accepted; active reveal states continue.
    for (unsigned raw = 0; raw <= 2; ++raw) {
        reset_engine(); current_map = 34; flags[0x1AA] = 1;
        D_global_asm_807F6240[5] = 0x31; live_raw[0x31] = (unsigned char)raw;
        CHECK(coop_live_world_refresh(0x1AA));
        CHECK(live_trombone_notifications == 1 && live_trombone_mode == 2
            && live_raw[0x31] == 6 && live_calls == 0);
    }
    for (unsigned raw = 3; raw <= 6; ++raw) {
        reset_engine(); current_map = 34; flags[0x1AA] = 1;
        D_global_asm_807F6240[5] = 0x31; live_raw[0x31] = (unsigned char)raw;
        CHECK(coop_live_world_refresh(0x1AA));
        CHECK(!live_trombone_notifications && !live_trombone_mode
            && live_raw[0x31] == raw && live_calls == 0);
    }
    reset_engine(); current_map = 34; flags[0x1AA] = 1;
    D_global_asm_807F6240[5] = 0x31; live_raw[0x31] = 7;
    CHECK(!coop_live_world_refresh(0x1AA) && !live_trombone_notifications);
    reset_engine(); current_map = 34; flags[0x1AA] = 1;
    CHECK(!coop_live_world_refresh(0x1AA) && !live_trombone_notifications);

    // The network path writes and saves the permanent flag, then applies the
    // loaded reveal without scheduling a main-map rebuild.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 34; mock_level = 7; D_global_asm_807F6240[4] = 0x31;
    live_raw[0x31] = 2;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && !g.deferred);
    r = {}; r.status = 2; r.scope = 1; r.session_lo = 907;
    unsigned isles_trombone = (unsigned)coop_item_id(0x1AA);
    CHECK(isles_trombone < COOP_ITEMS);
    r.apply[isles_trombone / 32] = bit(isles_trombone);
    coop_items_receive(&g, r); g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0x1AA] && writes == 1 && saves == 1
        && live_trombone_notifications == 1 && live_trombone_mode == 2
        && live_raw[0x31] == 6 && !g.refresh_pending);

    // Interior-only structures do not reload an unrelated main map.
    reset_engine(); current_map = 38;
    CHECK(coop_live_world_refresh(0x045) && live_calls == 0);
    reset_engine(); current_map = 87;
    CHECK(coop_live_world_refresh(0x15C) && live_calls == 0);

    // A reviewed ordinary interior may accept only its exact loaded world
    // unit. The flag, replay and isolated save complete without a map reload.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 16; mock_level = 1;
    D_global_asm_807F6240[2] = 0x04; D_global_asm_807F6240[3] = 0x0A;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && g.deferred);
    r = {}; r.status = 2; r.scope = 1; r.session_lo = 904;
    unsigned temple_ice = (unsigned)coop_item_id(0x045);
    r.apply[temple_ice / 32] = bit(temple_ice);
    r.apply[78 / 32] |= bit(78); // Unrelated Nintendo coin stays deferred.
    coop_items_receive(&g, r); g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0x045] && !flags[0x084] && writes == 1 && saves == 1
        && live_calls == 2 && live_state == 0 && !g.refresh_pending);

    // Preflight is atomic: a missing component leaves both the flag and loaded
    // scripts untouched so the same unit can be retried on a later frame.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 16; mock_level = 1; D_global_asm_807F6240[2] = 0x04;
    coop_items_capture(&g, 1, 1, 0);
    r = {}; r.status = 2; r.scope = 1; r.session_lo = 906;
    r.apply[temple_ice / 32] = bit(temple_ice); coop_items_receive(&g, r);
    g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(!flags[0x045] && !writes && !saves && !live_calls
        && g.wait_reason == COOP_TRACE_WAIT_PROGRESSION_CONTEXT);

    // Level-less lobbies are admitted only through the same exact table match.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 194; mock_level = 255; D_global_asm_807F6240[4] = 0;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready && g.deferred);
    r = {}; r.status = 2; r.scope = 1; r.session_lo = 905;
    unsigned caves_wall = (unsigned)coop_item_id(0x198);
    r.apply[caves_wall / 32] = bit(caves_wall);
    coop_items_receive(&g, r); g.refresh_enabled = 1; coop_items_apply(&g, 1);
    CHECK(flags[0x198] && writes == 1 && saves == 1 && live_calls == 1
        && live_state == 0 && !g.refresh_pending);

    // A partially loaded portal must still request the full reload fallback.
    reset_engine(); current_map = 38;
    for (unsigned i = 0; i < portals[1].count - 1; ++i)
        D_global_asm_807F6240[20 + i] = (short)portals[1].object[i];
    CHECK(!coop_live_world_refresh(portals[1].flag)
        && live_calls == 0);

    // Training's switch and exit door must advance as one reviewed live unit.
    reset_engine(); current_map = 176;
    D_global_asm_807F6240[5] = 0x39; D_global_asm_807F6240[8] = 0x49;
    CHECK(coop_live_world_refresh(0x181) && live_calls == 2 && live_slot == 8
        && live_state == 20);

    // Reversible world state remains blocked by default, then starts the exact
    // loaded vanilla up/down switch sequence instead of rebuilding the map.
    reset_engine(); g = {}; current_game = &g; g.input.ready = g.bound = 1;
    CoopWorld w{}; w.input.ready = 1; w.result.scope = 1; w.result.status = 2;
    w.result.apply = w.result.desired = 1; current_map = 30; mock_level = 3;
    coop_world_apply(&w, &g, 1);
    CHECK(!flags[0xA0] && !g.save_pending && !g.refresh_pending);
    D_global_asm_807F6240[12] = 0; // Galleon up switch object.
    g.refresh_enabled = 1; coop_world_apply(&w, &g, 1);
    CHECK(flags[0xA0] && writes == 1 && g.save_pending && live_calls == 1
        && live_slot == 12 && live_state == 10 && !g.refresh_pending);

    // If the chosen paired script is absent, preserve the reviewed reload path.
    reset_engine(); g = {}; current_game = &g; g.input.ready = g.bound = 1;
    w = {}; w.input.ready = 1; w.result.scope = 1; w.result.status = 2;
    w.result.apply = 2; current_map = 48; mock_level = 4;
    flags[0xCE] = 1; // Remote asks this night-time peer to return to day.
    g.refresh_enabled = 1; coop_world_apply(&w, &g, 1);
    CHECK(writes == 1 && g.save_pending && !live_calls && g.refresh_pending && g.refresh_map == 48);

    // Fungi day/night selects the night or day switch from the desired bit.
    reset_engine(); g = {}; current_game = &g; g.input.ready = g.bound = 1;
    w = {}; w.input.ready = 1; w.result.scope = 1; w.result.status = 2;
    w.result.apply = w.result.desired = 2; current_map = 48; mock_level = 4;
    D_global_asm_807F6240[7] = 4;
    g.refresh_enabled = 1; coop_world_apply(&w, &g, 1);
    CHECK(flags[0xCE] && writes == 1 && live_calls == 1 && live_slot == 7
        && live_state == 10 && !g.refresh_pending);

    // The reversible Caves-lobby pressure switch uses the same host-authority
    // channel. It applies and saves only after leaving the loaded lobby; the
    // next lobby entry initializes its linked doors from the desired flag.
    reset_engine(); g = {}; current_game = &g; g.input.ready = g.bound = 1;
    w = {}; w.input.ready = 1; w.result.scope = 1; w.result.status = 2;
    w.result.apply = w.result.desired = 4; current_map = 7; mock_level = 0;
    coop_world_apply(&w, &g, 1);
    CHECK(flags[0x19D] && writes == 1 && g.save_pending && !g.refresh_pending);

    // Capture exposes all three bits and gives the lobby state its own
    // monotonic change counter rather than aliasing water or day/night.
    w = {}; g.deferred = 0; g.input.enabled = g.input.ready = g.bound = 1;
    g.input.file = 1; coop_world_capture(&w, &g);
    CHECK(w.input.ready && w.input.values == 4 && !w.input.change[2]);
    flags[0x19D] = 0; coop_world_capture(&w, &g);
    CHECK(w.input.ready && w.input.values == 0 && w.input.change[2] == 1
        && !w.input.change[0] && !w.input.change[1]);

    // With automatic refresh, a peer already in the Caves lobby runs the exact
    // vanilla press/release state on object 6 and saves without leaving.
    reset_engine(); g = {}; current_game = &g; g.input.ready = g.bound = g.live_snapshot = 1;
    g.refresh_enabled = 1; w = {}; w.input.ready = 1; w.result.scope = 1; w.result.status = 2;
    w.result.apply = w.result.desired = 4; current_map = 194; mock_level = 7;
    D_global_asm_807F6240[12] = 6;
    coop_world_apply(&w, &g, 1);
    CHECK(flags[0x19D] && writes == 1 && live_calls == 1 && live_slot == 12
        && live_state == 2 && g.save_pending && g.world_save_pending && !g.refresh_pending);
    coop_items_save_world_lobby(&g, 0); CHECK(!saves && g.world_save_pending);
    coop_items_save_world_lobby(&g, 1);
    CHECK(saves == 1 && !g.save_pending && !g.world_save_pending);

    reset_engine(); g = {}; current_game = &g; g.input.ready = g.bound = g.live_snapshot = 1;
    g.refresh_enabled = 1; w = {}; w.input.ready = 1; w.result.scope = 1; w.result.status = 2;
    w.result.apply = 4; current_map = 194; mock_level = 7; flags[0x19D] = 1;
    D_global_asm_807F6240[5] = 6;
    coop_world_apply(&w, &g, 1);
    CHECK(!flags[0x19D] && live_calls == 1 && live_state == 6 && !g.refresh_pending);

    // Missing switch object keeps the verified flag/save and falls back to the
    // existing same-map vanilla rebuild instead of claiming partial live success.
    reset_engine(); g = {}; current_game = &g; g.input.ready = g.bound = g.live_snapshot = 1;
    g.refresh_enabled = 1; w = {}; w.input.ready = 1; w.result.scope = 1; w.result.status = 2;
    w.result.apply = w.result.desired = 4; current_map = 194; mock_level = 7;
    coop_world_apply(&w, &g, 1);
    CHECK(flags[0x19D] && !live_calls && g.refresh_pending && g.refresh_map == 194
        && g.world_save_pending);

    // The reviewed lobby frame is live for reversible capture even though item
    // grants remain deferred there.
    reset_engine(); g = {}; current_game = &g; current_map = 194; mock_level = 7;
    coop_items_capture(&g, 1, 1, 0); g.bound = 1;
    w = {}; coop_world_capture(&w, &g);
    CHECK(g.deferred && g.live_snapshot && w.input.ready && !w.input.values);
    flags[0x19D] = 1; coop_world_capture(&w, &g);
    CHECK(w.input.values == 4 && w.input.change[2] == 1);
}
static void world_authority_checks() {
    WorldSync host, guest;
    CoopWorldInput h{1, 1, 1}, g{1, 2, 1};
    constexpr uint64_t session = 0x123456789ULL;
    auto exchange = [&] {
        WorldWire hw = host.wire(), gw = guest.wire();
        host.update(true, h, gw, true, true, true, session);
        guest.update(false, g, hw, true, true, true, session);
    };
    exchange(); exchange();
    h.session_hi = g.session_hi = unsigned(session >> 32);
    h.session_lo = g.session_lo = unsigned(session);
    h.scope = g.scope = 1;
    for (unsigned i = 0; i < 6; ++i) exchange();
    CHECK(host.result().scope == 1 && guest.result().scope == 1
        && host.wire().revision[0] == 1 && host.wire().revision[1] == 1
        && host.wire().revision[2] == 1);

    // A guest lobby-switch change uses only lane 2. Host authority accepts it,
    // requests local readback, and acknowledges without touching the two older lanes.
    g.values = 4; g.change[2] = 1;
    for (unsigned i = 0; i < 4; ++i) exchange();
    CHECK((host.result().apply & 4) && !(host.result().apply & 3)
        && host.wire().revision[0] == 1 && host.wire().revision[1] == 1
        && host.wire().revision[2] == 2 && host.wire().ack[2] == 1);
    h.values = 4;
    for (unsigned i = 0; i < 6; ++i) exchange();
    CHECK(host.result().status == 3 && guest.result().status == 3
        && host.wire().desired == 4 && guest.wire().desired == 4
        && !host.result().pending && !guest.result().pending);
}
static void cross_area_cache_checks() {
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && g.baseline && !g.deferred);
    CoopItemResult r{}; r.status = 3; r.scope = 1; r.session_lo = 903;
    coop_items_receive(&g, r);
    unsigned before[COOP_ITEM_WORDS];
    std::memcpy(before, g.input.owned, sizeof(before));

    // A shop/boss/minigame frame keeps advertising the last verified snapshot,
    // but it cannot sample reversible world state or apply a remote write.
    current_map = 5;
    flags[0x84] = 1; // Local Nintendo coin flag settles after its reward queue.
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && g.deferred && coop_item_has(g.input.owned, 78)
        && coop_item_has(g.input.request, 78));
    before[78 / 32] |= bit(78);
    CHECK(std::memcmp(before, g.input.owned, sizeof(before)) == 0);
    CoopWorld w{}; coop_world_capture(&w, &g); CHECK(!w.input.ready);
    g.result.status = 2; g.result.apply[79 / 32] = bit(79);
    coop_items_apply(&g, 1);
    CHECK(!flags[0x17B] && !writes && !saves);

    // The queued grant applies once this device can verify and save its live
    // inventory again; no peer map comparison is involved.
    current_map = 7;
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && !g.deferred);
    coop_items_apply(&g, 1);
    CHECK(flags[0x17B] && writes == 1 && saves == 1);

    // A fresh session may establish its complete snapshot in a reviewed
    // ordinary interior. It publishes immediately but cannot apply/save there.
    reset_engine(); g = {}; current_game = &g; g.join = 1;
    current_map = 4; mock_level = 0; // Japes mountain: reviewed ordinary map.
    coop_items_capture(&g, 1, 1, 0);
    CHECK(g.input.ready && g.baseline && g.deferred);
    w = {}; w.input.ready = 1; coop_world_capture(&w, &g); CHECK(!w.input.ready);
    g.result.status = 2; g.result.apply[78 / 32] = bit(78);
    coop_items_apply(&g, 1);
    CHECK(!flags[0x84] && !writes && !saves);

    // Peaceful Galleon/Fungi/Caves lobbies are explicit snapshot maps even
    // though they contain no enemy and therefore are not combat-map entries.
    for (unsigned lobby : {174u, 178u, 194u}) {
        reset_engine(); g = {}; current_game = &g; current_map = lobby; mock_level = 7;
        coop_items_capture(&g, 1, 1, 0);
        CHECK(g.input.ready && g.baseline && g.deferred);
    }

    // An unreviewed overlay still cannot become the first snapshot.
    reset_engine(); g = {}; current_game = &g;
    current_map = 5; mock_level = 0;
    coop_items_capture(&g, 1, 1, 0);
    CHECK(!g.input.ready && !g.baseline && !g.deferred);
}
static void live_checks() {
    uint64_t now = 10000; Session host, guest;
    CHECK(host.start({Role::host, "", 0, 123456}, now));
    LossyRelay relay(host.bound_port()); Config join{Role::join, "127.0.0.1", relay.port, 123456};
    CHECK(guest.start(join, now));
    CoopItems h{}, g{}; g.join = 1;
    State hs{7, 1, 0, active}, gs{38, 2, 1, active}; // Inventory is not restricted to same-map peers.
    unsigned ho[COOP_ITEM_WORDS]{}, go[COOP_ITEM_WORDS]{}, hw = 0, gw = 0;
    bool apply_h = true, apply_g = true, hready = true, gready = true;
    unsigned host_item_mode = 1;
    auto pump = [&](unsigned frames) {
        for (unsigned i = 0; i < frames; ++i) {
            now += 10; coop_items_prepare(&h, host_item_mode, hready, 0); coop_items_prepare(&g, 1, gready, 1);
            for (unsigned j = 0; j < COOP_ITEM_WORDS; ++j) { h.input.owned[j] = ho[j]; g.input.owned[j] = go[j]; }
            relay.forward(); host.tick(hs, now, {}, {}, h.input); relay.forward();
            guest.tick(gs, now, {}, {}, g.input); relay.forward();
            coop_items_receive(&h, host.items(now)); coop_items_receive(&g, guest.items(now));
            for (unsigned id = 0; id < COOP_ITEMS; ++id) {
                unsigned w = id / 32, b = bit(id);
                if (apply_h && (h.result.apply[w] & b) && !(ho[w] & b)) { ho[w] |= b; ++hw; }
                if (apply_g && (g.result.apply[w] & b) && !(go[w] & b)) { go[w] |= b; ++gw; }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    pump(120); CHECK(h.result.status == 3 && g.result.status == 3);
    // One missing page must block all grants/acks even with fresh presence.
    relay.blocked_item_page = 1; pump(300);
    CHECK(h.result.status == 1 && g.result.status == 1 && !any(h.result.apply) && !any(g.result.acknowledged));
    relay.blocked_item_page = -1; pump(120); CHECK(h.result.status == 3 && g.result.status == 3);
    // Collect all 2141 allowlisted items, alternate collectors, with duplicate writes.
    apply_h = apply_g = false;
    for (unsigned id = 0; id < COOP_PROGRESSION_FIRST; ++id) {
        if (id & 1) {
            if (id < COOP_PICKUP_FIRST) coop_items_observe(&g, 1, coop_item_flag(id), 1, 0, 0, 1, 1);
            else coop_items_note(&g, id);
            go[id / 32] |= bit(id);
            coop_items_observe(&g, 1, coop_item_flag(id), 1, 0, 1, 1, 1);
        } else ho[id / 32] |= bit(id);
    }
    pump(100); CHECK(any(h.result.apply) && any(g.result.apply) && any(g.input.request));
    // Pausing/loading and short stale gaps cannot award from cached payloads or lose requests.
    gready = false; pump(100); CHECK(h.result.status == 1 && g.result.status == 1 && any(g.input.request));
    CHECK(!any(h.result.apply) && !any(g.result.apply));
    gready = true; pump(100);
    auto session_before = g.input.session_lo;
    now += 800; auto stale = guest.items(now);
    CHECK(stale.status == 1 && !any(stale.apply) && !any(stale.acknowledged));
    coop_items_receive(&g, stale); CHECK(any(g.input.request) && g.input.session_lo == session_before);
    pump(100); apply_h = true; pump(100);
    // Independently retried pages require a host readback and a return
    // trip for acknowledgements. Loss can exceed the old fixed 1-second window.
    // Bound convergence explicitly while still forbidding extra grants.
    for (unsigned retry = 0; retry < 300 && (hw != 1070 || any(g.input.request)); ++retry) {
        CHECK(hw <= 1070 && gw == 0); pump(1);
    }
    CHECK(hw == 1070 && gw == 0 && !any(g.input.request));
    apply_g = true; pump(250); CHECK(hw == 1070 && gw == 1071);
    // With paged snapshots, a lossy run may end inside a permitted freshness
    // gap. Require both peers to observe convergence within a bounded window,
    // rather than requiring the last arbitrarily chosen tick to be fresh.
    for (unsigned retry = 0; retry < 300 && (h.result.status != 3 || g.result.status != 3); ++retry) pump(1);
    CHECK(h.result.status == 3 && g.result.status == 3);
    pump(100); CHECK(hw == 1070 && gw == 1071);
    guest.stop(); pump(10); CHECK(!any(h.result.apply));
    // The relay may drop BYE. Allow the old host reservation to time out (3 s)
    // plus HELLO retries; a new nonce must not evict that reservation early.
    CHECK(guest.start(join, now)); pump(700);
    CHECK(h.result.status == 3 && g.result.status == 3 && hw == 1070 && gw == 1071);
    // A lagging guest catches up after reconnect; its old inventory never advances host.
    guest.stop(); pump(10); for (auto& word : go) word = 0;
    CHECK(guest.start(join, now)); pump(700);
    CHECK(h.result.status == 3 && g.result.status == 3 && gw == 3212 && hw == 1070);
    for (unsigned id = COOP_PROGRESSION_FIRST; id < COOP_ITEMS; ++id) {
        coop_items_note(&g, id); go[id / 32] |= bit(id);
    }
    pump(400);
    for (unsigned retry = 0; retry < 300 && (h.result.status != 3 || g.result.status != 3); ++retry) pump(1);
    CHECK(h.result.status == 3 && g.result.status == 3 && hw == 1070 + ((unsigned)COOP_ITEMS - (unsigned)COOP_PROGRESSION_FIRST) && !any(g.input.request));
    for (unsigned id = 0; id < COOP_ITEMS; ++id) CHECK(coop_item_has(ho, id) && coop_item_has(go, id));
    ho[4] &= ~0x80000000u; pump(100); CHECK(h.result.status == 4 && g.result.status == 4);
    CHECK(!(ho[4] & 0x80000000u) && !any(h.result.apply) && !any(g.result.apply));
    // Explicit host recovery imports validated guest ownership through the
    // ordinary apply/readback path, then both peers converge on the union.
    host_item_mode = 2; pump(100);
    for (unsigned retry = 0; retry < 300 && (h.result.status != 3 || g.result.status != 3); ++retry) pump(1);
    CHECK((ho[4] & 0x80000000u) && h.result.status == 3 && g.result.status == 3);
    now += 3100; host.tick(hs, now, {}, {}, h.input); CHECK(!any(host.items(now).apply));
    CHECK(relay.dropped > 100 && relay.replayed > 100);
    CHECK(host.statistics().rejected + guest.statistics().rejected > 100);
}
#include "progression_checks.h"
#include "training_checks.h"
int main() {
    protocol_checks(); policy_checks(); engine_checks(); snide_and_medal_checks(); remaining_collectible_checks(); same_level_gb_delivery_checks(); actor_collectible_checks(); japes_boulder_bunch_checks(); krool_completion_checks(); arcade_payment_checks(); progression_checks(); training_checks(); world_refresh_checks(); world_authority_checks(); cross_area_cache_checks(); live_checks();
    std::printf("PASS: %u item, all GBs/1700 pickups adapter, inventory preservation, authority, deduplication, lossy UDP and reconnect checks\n", checks);
}
