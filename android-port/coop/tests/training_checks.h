static void training_checks() {
    const unsigned training_flags[] = {0x17F,0x182,0x183,0x184,0x185,0x187,0x180,0x179};
    // Flag identity, cancellation, safe-room delivery and exact first-slam side effect.
    for (unsigned i = 0; i < 8; ++i) {
        reset_engine(); CoopItems g{}; current_game = &g; g.join = 1;
        unsigned id = 2191 + i, f = training_flags[i];
        CHECK(coop_item_flag(id) == int(f) && coop_item_id(f) == int(id));
        if (i >= 1 && i <= 6) flags[0x17F] = 1;
        if (i == 5 || i == 6) for (unsigned k = 0; k < 4; ++k) flags[0x182 + k] = 1;
        if (i == 6) flags[0x187] = 1;
        MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
        current_map = 171; mock_level = 7;
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
        CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 777; r.apply[id/32] = bit(id);
        coop_items_receive(&g, r);
        coop_items_apply(&g, 0); CHECK(!writes && !saves);
        block_write = 1; coop_items_apply(&g, 1); CHECK(writes == 1 && !saves && !flags[f]);
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        block_write = 0; coop_items_apply(&g, 1); CHECK(writes == 2 && saves == 1 && flags[f]);
        if (i == 6) for (unsigned k = 0; k < 5; ++k) expected[0].character_progress[k].progression[1] = 1;
        CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
        coop_items_capture(&g, 1, 1, 0); coop_items_apply(&g, 1);
        CHECK(g.input.ready && !any(g.input.request) && saves == 1 && writes == 2);
    }
    // Reviewed ordinary maps may publish a complete first snapshot. Only the
    // narrower main-world/treehouse set may apply a training grant or save.
    // Grounds (176), barrels, Fairy Island (189), Cranky (5), shops and boss
    // overlays remain outside both sets.
    for (unsigned map = 0; map < 216; ++map) {
        reset_engine(); CoopItems g{}; current_game = &g; current_map = map;
        bool apply_allowed = map == 7 || map == 26 || map == 30 || map == 34 || map == 38
            || map == 48 || map == 72 || map == 87 || map == 171;
        bool snapshot_allowed = apply_allowed || coop_combat_map(map)
            || map == 174 || map == 178 || map == 194;
        coop_items_capture(&g, 1, 1, 0);
        CHECK(bool(g.input.ready) == snapshot_allowed && bool(g.deferred) == (snapshot_allowed && !apply_allowed)
            && !g.counter_error);
        g.result.status = 2; g.result.apply[2198/32] = bit(2198); coop_items_apply(&g, 1);
        CHECK(bool(flags[0x179]) == apply_allowed && writes == unsigned(apply_allowed));
    }
    // Fresh guest receives all training and the real host's first gift, then a
    // pending higher slam. Never replay the gift or downgrade on duplicate packets.
    reset_engine(); CoopItems g{}; current_game = &g; g.join = 1; current_map = 171;
    coop_items_capture(&g, 1, 1, 0);
    CoopItemResult r{}; r.status = 2; r.scope = 1; r.session_lo = 777;
    for (unsigned id = 2191; id < 2199; ++id) r.apply[id/32] |= bit(id);
    r.apply[2170/32] |= bit(2170); r.apply[2171/32] |= bit(2171);
    coop_items_receive(&g, r);
    D_global_asm_807FD730 = &hud_object; coop_items_apply(&g, 1); CHECK(!writes && !saves);
    D_global_asm_807FD730 = nullptr;
    D_global_asm_80754280 = nullptr; coop_items_apply(&g, 1); CHECK(!writes && !saves);
    D_global_asm_80754280 = &hud_object;
    MockPlayerProgress expected[4]; std::memcpy(expected, D_global_asm_807FC950, sizeof(expected));
    coop_items_apply(&g, 1); CHECK(writes == 8 && saves == 1);
    for (unsigned k = 0; k < 5; ++k) CHECK(D_global_asm_807FC950[0].character_progress[k].progression[1] == 1);
    coop_items_capture(&g, 1, 1, 0); coop_items_apply(&g, 1); // Higher slam may now apply.
    for (unsigned k = 0; k < 5; ++k) expected[0].character_progress[k].progression[1] = 3;
    CHECK(std::memcmp(expected, D_global_asm_807FC950, sizeof(expected)) == 0);
    coop_items_capture(&g, 1, 1, 0); coop_items_apply(&g, 1);
    CHECK(writes == 8 && saves == 2 && !any(g.input.request));
    // The stock Cranky branch is skipped once its grant flag has been committed.
    CHECK(flags[0x180] && coop_progression_valid());
    // A flag-only or dependency-free first gift cannot be acknowledged as valid.
    reset_engine(); g = {}; current_map = 171;
    coop_items_capture(&g, 1, 1, 0); g.result.status = 2;
    g.result.apply[2197/32] = bit(2197); coop_items_apply(&g, 1);
    CHECK(!writes && !saves && !flags[0x180]);
    flags[0x180] = 1; coop_items_capture(&g, 1, 1, 0); CHECK(g.counter_error && !g.input.ready);
    // All course combinations: only the four actual readbacks derive completion.
    for (unsigned mask = 0; mask < 16; ++mask) {
        reset_engine(); g = {}; g.join = 1; current_map = 171; flags[0x17F] = 1;
        for (unsigned k = 0; k < 4; ++k) flags[0x182 + k] = (mask >> k) & 1;
        coop_items_capture(&g, 1, 1, 0);
        r = {}; r.status = 3; r.scope = 1; r.session_lo = 777; coop_items_receive(&g, r);
        coop_items_apply(&g, 1);
        CHECK(bool(flags[0x187]) == (mask == 15));
        CHECK(coop_item_has(g.input.request, 2196) == unsigned(mask == 15));
        CHECK(!flags[0x180]); // Completion alone never invents Cranky's gift.
        // Receive the remaining courses: both peers can have contributed to the set.
        for (unsigned k = 0; k < 4; ++k) if (!(mask & (1u << k))) r.apply[(2192+k)/32] |= bit(2192+k);
        coop_items_receive(&g, r); coop_items_apply(&g, 1);
        CHECK(flags[0x187] && !flags[0x180] && coop_item_has(g.input.request, 2196));
        for (unsigned k = 0; k < 4; ++k) CHECK(flags[0x182 + k]);
        coop_items_capture(&g, 1, 1, 0); CHECK(g.input.ready);
    }
    // Local camera gift during an excluded scene is captured on safe-room return.
    reset_engine(); g = {}; g.join = 1; current_map = 171;
    coop_items_capture(&g, 1, 1, 0); r = {}; r.status = 3; r.scope = 1; r.session_lo = 777; coop_items_receive(&g, r);
    current_map = 189; coop_items_capture(&g, 1, 1, 0); r.status = 1; coop_items_receive(&g, r);
    setFlag(0x179, 1, 0); CHECK(!any(g.input.request));
    current_map = 34; coop_items_capture(&g, 1, 1, 0); CHECK(coop_item_has(g.input.request, 2198));
    // Flags are all on page 2: malformed individual pages must fail immediately.
    for (unsigned id = 2192; id <= 2197; ++id) {
        ItemWire w{1,1,1,1}; w.page = 2; w.owned[id/32] = bit(id); CHECK(!valid_items(w));
        w.owned[2191/32] |= bit(2191);
        CHECK(valid_items(w) == (id < 2196));
        for (unsigned k = 0; k < 4; ++k) w.owned[(2192+k)/32] |= bit(2192+k);
        CHECK(valid_items(w));
    }
    // Treehouse is a training/progression delivery room, not a numeric-reward
    // loophole: old GB/CB/coin credits retain the main-world restriction.
    reset_engine(); g = {}; current_map = 171; coop_items_capture(&g, 1, 1, 0);
    g.result.status = 2; g.result.apply[160/32] = bit(160); g.result.apply[321/32] |= bit(321);
    coop_items_apply(&g, 1); CHECK(!writes && !saves);
    CHECK(!coop_progression_owned(0) && !coop_progression_apply(0, 1, 7, 0));
    CHECK(!coop_progression_owned(COOP_ITEMS) && !coop_progression_apply(COOP_ITEMS, 1, 7, 0));
    std::puts("PASS: 8 training/camera IDs, safe treehouse, first-slam ordering, split-course completion, blocked writes, prerequisites and unchanged resources");
}
