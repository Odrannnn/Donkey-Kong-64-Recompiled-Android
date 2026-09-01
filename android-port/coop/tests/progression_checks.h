// Included by items_tests.cpp so this executes the production MIPS game adapter
// with the same flag callbacks, pending queues, save writer and inventory layout.
static void progression_checks() {
    static_assert(offsetof(CoopPlayerProgress, melons) == 0x2FC);
    const unsigned rescue_flags[] = {0x6, 0x42, 0x46, 0x75};
    const unsigned rescue_levels[] = {0, 1, 1, 2};
    const unsigned keys[] = {0x1A, 0x4A, 0x8A, 0xA8, 0xEC, 0x124, 0x13D, 0x17C};
    for (unsigned id = 2141; id < 2155; ++id) {
        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        unsigned f = id < 2145 ? rescue_flags[id - 2141] : id == 2145 ? 0x1BB : id == 2154 ? 0x315 : 0x1BC + id - 2146;
        CHECK(coop_item_flag(id) == int(f));
        if (id >= 2146) flags[0x1BB] = 1;
        if (id >= 2146 && id < 2154) flags[keys[id - 2146]] = 1;
        if (id == 2154) for (unsigned i = 0; i < 8; ++i) flags[keys[i]] = flags[0x1BC + i] = 1;
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        mock_level = id < 2145 ? rescue_levels[id - 2141] : 7;
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
        CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 444; r.apply[id / 32] = bit(id);
        coop_items_receive(&g, r);
        coop_items_apply(&g, 1); CHECK(!writes && !saves); // Live rescue/Isles objects stay untouched.
        mock_level = (mock_level + 1) % 8;
        coop_items_apply(&g, 0); CHECK(!writes && !saves);
        block_write = 1; coop_items_apply(&g, 1); CHECK(writes == 1 && !saves && !flags[f]);
        block_write = 0; coop_items_apply(&g, 1); CHECK(writes == 2 && saves == 1 && flags[f]);
        coop_items_capture(&g, 1, 1, 0); coop_items_apply(&g, 1);
        CHECK(g.input.ready && !any(g.input.request) && writes == 2 && saves == 1);
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        // This same new permanent write is a request when locally obtained in a bound session.
        flags[f] = 0; g.previous[id / 32] &= ~bit(id);
        setFlag(f, 1, 0); coop_items_capture(&g, 1, 1, 0);
        CHECK(coop_item_has(g.input.request, id));
    }
    // Independent expected stock shop grants: Kong 5 means all five, field 5 is
    // player melon capacity. Weapon/instrument bits retain each Kong's base bit.
    struct Grant { unsigned id, kong, field, value; };
    const Grant grants[] = {
        {2155,0,0,1},{2156,0,0,2},{2157,0,0,3}, {2158,1,0,1},{2159,1,0,2},{2160,1,0,3},
        {2161,2,0,1},{2162,2,0,2},{2163,2,0,3}, {2164,3,0,1},{2165,3,0,2},{2166,3,0,3},
        {2167,4,0,1},{2168,4,0,2},{2169,4,0,3}, {2170,5,1,2},{2171,5,1,3},
        {2172,5,3,1},{2173,5,3,2}, {2174,0,2,1},{2175,1,2,1},{2176,2,2,1},{2177,3,2,1},{2178,4,2,1},
        {2179,5,2,2},{2180,5,2,4}, {2181,0,4,1},{2182,1,4,1},{2183,2,4,1},{2184,3,4,1},{2185,4,4,1},
        {2186,5,4,2},{2187,5,4,4},{2188,5,4,8}, {2189,5,5,2},{2190,5,5,3}
    };
    for (auto grant : grants) {
        reset_engine();
        flags[0x17F] = flags[0x182] = flags[0x183] = flags[0x184] = flags[0x185] = flags[0x187] = 1; CoopItems g{}; current_game = &g; g.join = 1;
        for (unsigned k = 0; k < 5; ++k) {
            auto& p = D_global_asm_807FC950[0].character_progress[k];
            p.coins = 34 + k; p.progression[1] = 1;
            if ((grant.field == 2 || grant.field == 4) && grant.value > 1) p.progression[grant.field] = k % 2;
        }
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
        CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 444; r.apply[grant.id / 32] = bit(grant.id);
        coop_items_receive(&g, r); coop_items_apply(&g, 1);
        CHECK(!saves && !writes && !coop_item_owned(grant.id)); // First-slam gift must precede higher upgrades.
        flags[0x180] = 1;
        g.previous[COOP_FIRST_SLAM / 32] |= bit(COOP_FIRST_SLAM); // Seed gift readback; new tests cover actual local events.
        coop_items_apply(&g, 0); CHECK(!saves);
        D_global_asm_80754280 = nullptr; coop_items_apply(&g, 1); CHECK(!saves);
        D_global_asm_80754280 = &hud_object;
        current_map = 5; coop_items_capture(&g, 1, 1, 0); coop_items_apply(&g, 1);
        CHECK(g.input.ready && g.deferred && !saves && !g.counter_error); // Cached snapshot travels; overlay writes wait.
        current_map = 7; D_global_asm_807FD730 = &hud_object;
        coop_items_capture(&g, 1, 1, 0); coop_items_apply(&g, 1); CHECK(!saves);
        D_global_asm_807FD730 = nullptr; coop_items_capture(&g, 1, 1, 0);
        coop_items_apply(&g, 1); CHECK(saves == 1 && !writes && coop_item_owned(grant.id));
        if (grant.field == 5) expected[0].melons = grant.value;
        else for (unsigned k = 0; k < 5; ++k) if (grant.kong == 5 || grant.kong == k) {
            auto& value = expected[0].character_progress[k].progression[grant.field];
            if (grant.field == 2 || grant.field == 4) value |= grant.value; else value = grant.value;
        }
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        coop_items_capture(&g, 1, 1, 0); coop_items_apply(&g, 1);
        CHECK(g.input.ready && !g.counter_error && saves == 1 && !any(g.input.request)); // Includes implied lower tiers.
        // No remote grant can downgrade or charge/refill the buyer a second time.
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
    }
    // Local numeric purchases have no flag callback; rising ownership must create
    // a request, including all lower tiers, across a suspended shop frame.
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
    coop_items_capture(&g, 1, 1, 0);
    CoopItemResult r{}; r.status = 3; r.scope = 1; r.session_lo = 444; coop_items_receive(&g, r);
    current_map = 5; coop_items_capture(&g, 1, 1, 0); r.status = 1; coop_items_receive(&g, r);
    D_global_asm_807FC950[0].character_progress[2].progression[0] = 3;
    current_map = 7; coop_items_capture(&g, 1, 1, 0);
    for (unsigned id = 2161; id <= 2163; ++id) CHECK(coop_item_has(g.input.request, id));
    // Reject modified/corrupt inventory without touching it or silently merging.
    for (unsigned field = 0; field < 5; ++field) {
        reset_engine(); g = {}; const unsigned invalid[] = {4,4,8,3,16};
        D_global_asm_807FC950[0].character_progress[4].progression[field] = invalid[field];
        coop_items_capture(&g, 1, 1, 0); CHECK(g.counter_error && !g.input.ready);
    }
    for (unsigned field : {1u,2u,3u,4u}) {
        reset_engine(); g = {};
        D_global_asm_807FC950[0].character_progress[4].progression[field] = field == 2 || field == 4 ? 2 : 1;
        coop_items_capture(&g, 1, 1, 0); CHECK(g.counter_error && !g.input.ready);
    }
    reset_engine(); g = {}; D_global_asm_807FC950[0].melons = 4;
    coop_items_capture(&g, 1, 1, 0); CHECK(g.counter_error && !g.input.ready);
    // Key prerequisite is cross-page: a packet is valid, a full keyless save is not.
    ItemWire w{1,1,1,1}; w.page = 2;
    w.owned[2145/32] |= bit(2145); w.owned[2146/32] |= bit(2146);
    CHECK(valid_items(w) && !coop_items_full_dependencies(w.owned));
    w.owned[70/32] |= bit(70); CHECK(coop_items_full_dependencies(w.owned));
    for (unsigned id : {2156u,2157u,2171u,2173u,2190u,2154u}) {
        ItemWire bad{1,1,1,1}; bad.page = 2; bad.owned[id/32] = bit(id); CHECK(!valid_items(bad));
    }
    // Failed key readback cannot unlock its padlock, even with an apply bit.
    reset_engine(); g = {}; mock_level = 0; flags[0x1BB] = 1;
    coop_items_capture(&g, 1, 1, 0); g.result.status = 2; g.result.apply[2146/32] = bit(2146);
    coop_items_apply(&g, 1); CHECK(!writes && !saves && !flags[0x1BC]);
    flags[keys[0]] = 1; coop_items_apply(&g, 1); CHECK(writes == 1 && saves == 1 && flags[0x1BC]);
    CoopItemInput local{1,1,1}; local.session_lo = 444; local.scope = 1;
    ItemWire remote{1,1,1,1}; remote.owned[2155/32] = bit(2155);
    auto result = items_result(true, local, remote, true, true, 444);
    CHECK(result.status == 4 && !any(result.apply)); // An offline guest move cannot advance the host.
    remote.request[2155/32] = bit(2155);
    result = items_result(true, local, remote, true, true, 444);
    CHECK(result.status == 2 && coop_item_has(result.apply, 2155));
    CHECK(!any(items_result(true, local, remote, true, false, 444).apply));
    std::puts("PASS: 50 progression IDs, rescue/key deferral, shop tiers/global bits, training guard, cross-page prerequisites and full-inventory preservation");
}
