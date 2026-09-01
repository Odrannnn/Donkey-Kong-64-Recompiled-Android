// Included by gameplay_tests.c: all calls below execute the real built payload
// or independently recompiled original ROM functions, including save bit packing.
static unsigned shop_world, shop_coins, save_commits;
static void world(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = shop_world; }
static void coins(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = shop_coins; }
static void yes(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = 1; }
static void copy_memory(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    for (uint32_t i = 0; i < ctx->r6; ++i) put8((uint32_t)ctx->r4 + i, get8((uint32_t)ctx->r5 + i));
    ctx->r2 = ctx->r4;
}
static void commit_save(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK(ctx->r4 == 1); ++save_commits;
    memcpy(memory + 0x7ED6A8, memory + 0x7ECEA8, 0x800);
}
static void save_setup(unsigned mode, unsigned bits, unsigned file, unsigned dynamic_bits) {
    setup(mode, 0, bits);
    memset(memory + 0x7ECEA8, 0, 0x1000);
    memset(memory + 0x7FC930, 0, 16);
    put16(0x807ECEA0, dynamic_bits);
    for (unsigned i = 0; i < 4; ++i) put8(0x807EDEA8 + i, i);
    put8(0x807FC928, 1); put8(0x807467C8, file);
    put8(0x807FC950 + 0x2fc, 3);
    for (unsigned k = 0; k < 5; ++k) {
        uint32_t p = 0x807FC950 + 0x5e * k;
        put8(p, (bits + k) & 7); put8(p + 1, 3); put8(p + 2, 7);
        put8(p + 3, 2); put8(p + 4, 15); put8(p + 5, 0x85);
        put16(p + 6, 50 + k);
        for (unsigned l = 0; l < 8; ++l) {
            put16(p + 0xa + 2*l, 100 - k - l);
            put16(p + 0x26 + 2*l, k + l);
            put16(p + 0x42 + 2*l, (k + l) % 6);
        }
    }
    save_commits = 0;
}
static void put_offer(uint32_t p, int type, unsigned rank, unsigned target, unsigned price) {
    put16(p, type); put16(p + 2, rank); put8(p + 4, target); put8(p + 5, price);
}
static void empty_offers(uint32_t base) {
    for (unsigned i = 0; i < 105; ++i) put_offer(base + i * 6, -1, 0, 0, 0);
}
static void setup_save_references(void) {
    func_global_asm_8060C648 = baseline_func_global_asm_8060C648;
    func_global_asm_8060C6B8 = baseline_func_global_asm_8060C6B8;
    func_global_asm_8060C724 = baseline_func_global_asm_8060C724;
    gameIsInQuitGameMode = zero; gameIsInAdventureMode = yes;
    func_global_asm_805FC98C = zero; func_global_asm_8060B8F8 = commit_save;
    func_global_asm_806C9AE0 = noop; func_global_asm_80709464 = noop; func_global_asm_806C7B00 = noop;
    *rom_memcpy = copy_memory;
    func_menu_80025D14 = baseline_func_menu_80025D14;
    func_menu_80025D44 = baseline_func_menu_80025D44;
    func_menu_80025D8C = baseline_func_menu_80025D8C;
    getLevelIndex = world; func_global_asm_806F8AD4 = coins;
}
static void test_shops_and_saves(void) {
    setup_save_references();

    // Save/load all move masks, including the high bit, through the original
    // field-address calculator and bit writer/reader. Verify neighbouring fields.
    unsigned char stock_save[0x800];
    for (unsigned variant = 0; variant < 3; ++variant) for (unsigned bits = 0; bits < 8; ++bits)
    for (unsigned dyn = 0; dyn < 2; ++dyn) for (unsigned file = 0; file < (variant == 1 ? 1 : 3); ++file) {
        unsigned mode = variant == 1;
        recomp_func_t* save = variant == 2 ? baseline_func_global_asm_8060DEC8 : test_func_global_asm_8060DEC8;
        recomp_func_t* read = variant == 2 ? baseline_func_global_asm_8060DC3C : test_func_global_asm_8060DC3C;
        if (!mode) {
            save_setup(0, bits, file, dyn * 608);
            invoke(baseline_func_global_asm_8060DEC8, 0, 0, 0);
            memcpy(stock_save, memory + 0x7ED6A8, sizeof(stock_save));
        }
        save_setup(mode, bits, file, dyn * 608);
        invoke(save, 0, 0, 0); CHECK(save_commits == 1);
        if (!mode) CHECK(!memcmp(stock_save, memory + 0x7ED6A8, sizeof(stock_save)));
        memset(memory + 0x7FC950, 0, 0x306);
        invoke(read, file, 0, 0);
        CHECK(get8(0x807467C8) == file && get8(0x807FC950 + 0x2fc) == 3);
        for (unsigned k = 0; k < 5; ++k) {
            uint32_t p = 0x807FC950 + 0x5e * k;
            CHECK(get8(p) == (((bits + k) & 7) & (mode ? 7 : 3)));
            CHECK(get8(p + 1) == 3 && get8(p + 2) == 7 && get8(p + 3) == 2 && get8(p + 4) == 15);
            CHECK(get8(p + 5) == (mode ? 0x80 : 0x85));
            CHECK(get16(p + 6) == 50 + k);
            for (unsigned l = 0; l < 8; ++l) {
                CHECK(get16(p + 0xa + 2*l) == 100 - k - l);
                CHECK(get16(p + 0x26 + 2*l) == k + l);
                CHECK(get16(p + 0x42 + 2*l) == (k + l) % 6);
            }
        }
    }

    // The internal table loader must reject malformed seeds atomically, and
    // cannot be used to replace offers after gameplay has been activated.
    setup(0, 0, 0); empty_offers(PAYLOAD);
    put_offer(PAYLOAD, 0, 3, 4, 255);
    CHECK(invoke(test_ap_prepare_move_shops, PAYLOAD, 0, 0) == 1);
    CHECK(get8(TEST_AP_CRANKY_MOVES + 5) == 255);
    put_offer(PAYLOAD, 0, 1, 0, 5); put_offer(PAYLOAD + 104*6, 0, 4, 0, 1);
    CHECK(invoke(test_ap_prepare_move_shops, PAYLOAD, 0, 0) == 0);
    CHECK(get8(TEST_AP_CRANKY_MOVES + 5) == 255);
    put_offer(PAYLOAD + 104*6, 4, 4, 5, 1);
    CHECK(invoke(test_ap_prepare_move_shops, PAYLOAD, 0, 0) == 0);
    put_offer(PAYLOAD + 104*6, -1, 0, 0, 0); put32(TEST_MODE_ADDRESS, 1);
    CHECK(invoke(test_ap_prepare_move_shops, PAYLOAD, 0, 0) == 0);

    const unsigned maxima[] = {3,3,3,2,4};
    for (unsigned vendor = 0; vendor < 3; ++vendor) for (unsigned buyer = 0; buyer < 5; ++buyer)
    for (unsigned target = 0; target < 5; ++target) for (unsigned type = 0; type < 5; ++type)
    for (unsigned rank = 1; rank <= maxima[type]; ++rank) for (unsigned owned = 0; owned < (type == 4 ? 16 : 8); ++owned) {
        setup(0, buyer, 0); empty_offers(PAYLOAD);
        shop_world = (buyer + target + type) % 7; flag_result = 1;
        put_offer(PAYLOAD + ((vendor*5 + buyer)*7 + shop_world)*6, type, rank, target, 239);
        CHECK(invoke(test_ap_prepare_move_shops, PAYLOAD, 0, 0) == 1);
        put32(TEST_MODE_ADDRESS, 1); put32(ACTOR + 0x58, 0xBD + vendor);
        put8(0x807FC950 + buyer*0x5e + type, 7);
        put8(0x807FC950 + target*0x5e + type, owned);
        put8(0x807FCC4C, 3);
        invoke(test_func_menu_80025FB4, SCRIPT, 0x807FC950 + buyer*0x5e, 0);
        unsigned has = (type == 0 || type == 2 || type == 4) ? !(owned & (1u << (rank-1))) : owned < rank;
        CHECK(get8(SCRIPT + 0xb) == (has ? type : 254));
        CHECK(get16(0x80750AC8) == (has ? 239 : 0));
        CHECK(get16(SCRIPT + 4) == (has ? (target << 8) | 239 : 0));
        CHECK(get8(SCRIPT + 0xc) == shop_world && get8(SCRIPT + 0x10) == 3);
        if (has) CHECK(get8(SCRIPT + 0x11) == rank);
    }
    // Unsupported legacy shop worlds stay empty, without modifying any code.
    for (unsigned l = 7; l < 10; ++l) {
        setup(1, 0, 0); shop_world = l; flag_result = 1; put32(ACTOR + 0x58, 0xBD);
        uint32_t original_store = get32(0x80026194);
        invoke(test_func_menu_80025FB4, SCRIPT, 0x807FC950, 0);
        CHECK(get8(SCRIPT + 0xb) == 255 && get16(SCRIPT + 4) == 0);
        CHECK(get32(0x80026194) == original_store);
    }

    // All original vendor/Kong paths stay byte-equivalent with the gate off.
    unsigned char expected_menu[32];
    for (unsigned vendor = 0; vendor < 3; ++vendor) for (unsigned k = 0; k < 5; ++k) {
        setup(0, k, 0); shop_world = 3; flag_result = 1; put32(ACTOR + 0x58, 0xBD + vendor);
        invoke(baseline_func_menu_80025FB4, SCRIPT, 0x807FC950 + k*0x5e, 0);
        memcpy(expected_menu, memory + SCRIPT - 0x80000000u, 32);
        unsigned price = get16(0x80750AC8);
        setup(0, k, 0); flag_result = 1; put32(ACTOR + 0x58, 0xBD + vendor);
        invoke(test_func_menu_80025FB4, SCRIPT, 0x807FC950 + k*0x5e, 0);
        CHECK(!memcmp(expected_menu, memory + SCRIPT - 0x80000000u, 32));
        CHECK(get16(0x80750AC8) == price);
    }
    for (unsigned variant = 0; variant < 3; ++variant) for (unsigned buyer = 0; buyer < 5; ++buyer)
    for (unsigned target = 0; target < 5; ++target) for (unsigned rank = 1; rank <= 3; ++rank)
    for (unsigned bits = 0; bits < 8; ++bits) {
        unsigned mode = variant == 1;
        recomp_func_t* buy = variant == 2 ? baseline_func_menu_80025E04 : test_func_menu_80025E04;
        setup(mode, buyer, 0); put8(0x807FC928, 0);
        put8(0x807FC950 + target*0x5e, bits);
        put16(SCRIPT + 4, mode ? (target << 8) | 7 : 7); put8(SCRIPT + 0xb, 0); put8(SCRIPT + 0x11, rank);
        for (unsigned i = 0; i < 15; ++i) { put32(HUD + i*0x30, COUNTER + i*2); put16(COUNTER + i*2, 30); }
        invoke(buy, SCRIPT, 0x807FC950 + buyer*0x5e, 0);
        for (unsigned k = 0; k < 5; ++k) {
            unsigned expected = k == target ? bits : 0;
            if (mode && k == target) expected |= 1u << (rank-1);
            if (!mode && k == buyer) expected = rank;
            CHECK(get8(0x807FC950 + k*0x5e) == expected);
        }
        CHECK(get16(COUNTER + 2) == 23);
    }
    // Gun/instrument bits and shared slam/belt/upgrade purchases must preserve
    // the original all-Kong rules while targeting a different Kong's first item.
    for (unsigned variant = 0; variant < 3; ++variant) for (unsigned target = 0; target < 5; ++target)
    for (unsigned type = 1; type <= 4; ++type) for (unsigned rank = 1; rank <= maxima[type]; ++rank) {
        unsigned mode = variant == 1, buyer = (target + 1) % 5;
        setup(mode, buyer, 0); put8(0x807FC928, 0); put8(0x807FC950 + 0x2fc, 1);
        put32(0x807FC924, COUNTER + 0x100);
        put16(SCRIPT + 4, mode ? (target << 8) | 7 : 7); put8(SCRIPT + 0xb, type); put8(SCRIPT + 0x11, rank);
        for (unsigned k = 0; k < 5; ++k) put8(0x807FC950 + k*0x5e + type, 1);
        for (unsigned i = 0; i < 15; ++i) { put32(HUD + i*0x30, COUNTER + i*2); put16(COUNTER + i*2, 30); }
        invoke(variant == 2 ? baseline_func_menu_80025E04 : test_func_menu_80025E04, SCRIPT, 0x807FC950 + buyer*0x5e, 0);
        for (unsigned k = 0; k < 5; ++k) {
            unsigned expected = 1;
            if (type == 1 || type == 3) expected = rank;
            else if (rank > 1 || k == (mode ? target : buyer)) expected |= 1u << (rank-1);
            CHECK(get8(0x807FC950 + k*0x5e + type) == expected);
        }
        CHECK(get16(COUNTER + 2) == 23);
        unsigned melons = type == 4 ? get8(0x800334DC + rank) : 1;
        CHECK(get8(0x807FC950 + 0x2fc) == (melons > 1 ? melons : 1));
    }
    // Purchase followed by an actual game save and reload keeps independent bits.
    save_setup(1, 0, 0, 608); put8(0x807FC950 + 4*0x5e, 2);
    put16(SCRIPT + 4, (4 << 8) | 7); put8(SCRIPT + 0xb, 0); put8(SCRIPT + 0x11, 3);
    invoke(test_func_menu_80025E04, SCRIPT, 0x807FC950, 0); CHECK(save_commits == 1);
    memset(memory + 0x7FC950, 0, 0x306);
    invoke(test_func_global_asm_8060DC3C, 0, 0, 0);
    CHECK(get8(0x807FC950 + 4*0x5e) == 6);

    for (unsigned variant = 0; variant < 3; ++variant) for (unsigned target = 0; target < 5; ++target)
    for (unsigned enough = 0; enough < 2; ++enough) {
        unsigned mode = variant == 1;
        setup(mode, 0, 0); shop_coins = enough ? 17 : 16;
        put16(SCRIPT + 4, mode ? (target << 8) | 17 : 17); put8(SCRIPT + 0xb, 0);
        recomp_func_t* afford = variant == 2 ? baseline_func_menu_800261C0 : test_func_menu_800261C0;
        CHECK(invoke(afford, SCRIPT, 0, 0) == (enough ? 2 : 3));
        CHECK(get8(SCRIPT + 0xf) == (enough ? 1 : 3));
    }
}
