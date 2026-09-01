#include "../mod/ap_shops.h"
#define RESTORED 0x80907400u
#define SNAPSHOT 0x80907600u
static unsigned shop_spawns, shop_cutscenes;
static void fill_memory(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    for (uint32_t i = 0; i < ctx->r6; ++i) put8((uint32_t)ctx->r4 + i, ctx->r5);
    ctx->r2 = ctx->r4;
}
static void shop_spawn(uint8_t* rdram, recomp_context* ctx) { (void)rdram; CHECK(ctx->r4 == 0x144); ++shop_spawns; }
static void shop_cutscene(uint8_t* rdram, recomp_context* ctx) { (void)rdram; CHECK(ctx->r5 == 2 && ctx->r6 == 1); ++shop_cutscenes; }
static void offer_at(unsigned at, unsigned location, unsigned price, unsigned reserved) {
    put32(PAYLOAD + 8*at, location); put16(PAYLOAD + 8*at + 4, price); put16(PAYLOAD + 8*at + 6, reserved);
}
static void restore_bit(unsigned id) {
    unsigned i = ap_location_index(id);
    put32(RESTORED + 4*(i/32), get32(RESTORED + 4*(i/32)) | (1u << (i%32)));
}
static void expect_shop_snapshot(unsigned id) {
    invoke(test_ap_apshop_snapshot, SNAPSHOT, 0, 0);
    int index = id ? ap_location_index(id) : -1;
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i)
        CHECK(get32(SNAPSHOT + i*4) == (index >= 0 && i == (unsigned)index/32 ? 1u << ((unsigned)index%32) : 0));
}
static void configure_shop(const ap_shop_location_def* def, unsigned buyer, unsigned price) {
    setup(0, buyer, 7); put8(0x807FC928, 0);
    offer_at(0, def->id, price, 0);
    CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 1, RESTORED) == 1);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_SHOPS);
    put32(ACTOR + 0x58, 0xBD + def->vendor); shop_world = def->level; shop_coins = price;
    put32(0x807FC924, COUNTER + 0x100); put32(COUNTER + 0x104, PLAYER);
}
static void select_shop(void) { invoke(test_func_menu_80025FB4, SCRIPT, 0x807FC950, 0); }
static void buy_shop(void) { invoke(test_func_menu_80025E04, SCRIPT, 0x807FC950, 0); }
static void test_ap_shops(void) {
    *rom_memset = fill_memory;
    unsigned char inventory[0x306];
    // Every AP shop, including all 20 shared entries, tested from all five Kongs.
    for (unsigned s = 0; s < AP_SHOP_COUNT; ++s) for (unsigned buyer = 0; buyer < 5; ++buyer) {
        const ap_shop_location_def* def = &ap_shop_locations[s];
        configure_shop(def, buyer, 17); select_shop();
        expect_shop_snapshot(0);
        if (def->kong != AP_SHOP_SHARED && def->kong != buyer) {
            CHECK((signed char)get8(SCRIPT + 0xb) < 0); buy_shop(); expect_shop_snapshot(0); continue;
        }
        CHECK(get8(SCRIPT + 0xb) == AP_SHOP_ITEM_TYPE && get8(SCRIPT + 4) == buyer && get8(SCRIPT + 5) == 17);
        CHECK(get16(0x80750AC8) == 17 && get8(SCRIPT + 0xc) == def->level);
        CHECK(get16(SCRIPT + 2) == ap_locations[ap_location_index(def->id)].flag);
        shop_coins = 16;
        CHECK(invoke(test_func_menu_800261C0, SCRIPT, 0, 0) == 3);
        buy_shop(); expect_shop_snapshot(0);
        shop_coins = 17;
        CHECK(invoke(test_func_menu_800261C0, SCRIPT, 0, 0) == 2);
        put8(SCRIPT + 0xa, 1); CHECK(invoke(test_func_menu_800261C0, SCRIPT, 0, 0) == 6); put8(SCRIPT + 0xa, 0);
        // Stale or malformed offers cannot report a different check.
        const unsigned offsets[] = {2,4,5,0xb,0xc};
        for (unsigned j = 0; j < 5; ++j) {
            unsigned old = get8(SCRIPT + offsets[j]); put8(SCRIPT + offsets[j], old ^ 1);
            buy_shop(); expect_shop_snapshot(0); put8(SCRIPT + offsets[j], old);
        }
        memcpy(inventory, memory + 0x7FC950, sizeof(inventory));
        buy_shop(); expect_shop_snapshot(def->id);
        CHECK(!memcmp(inventory, memory + 0x7FC950, sizeof(inventory)));
        CHECK(flag_writes == 0 && hud_updates == 0 && get16(COUNTER) == 10 && shop_coins == 17);
        buy_shop(); expect_shop_snapshot(def->id); // duplicate frame is idempotent
        select_shop(); CHECK((signed char)get8(SCRIPT + 0xb) < 0);
        // All Kongs see a shared purchase disappear; individual offers remain scoped.
        if (def->kong == AP_SHOP_SHARED) for (unsigned k = 0; k < 5; ++k) {
            put8(0x8074E77C, k); select_shop(); CHECK((signed char)get8(SCRIPT + 0xb) < 0);
        }
        CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 0, RESTORED) == 0);
        CHECK(invoke(test_ap_apshop_restore, RESTORED, 0, 0) == 1); expect_shop_snapshot(def->id); // no rollback
    }
    // Invalid seed batches and restore snapshots leave the previous plan intact.
    const ap_shop_location_def* first = &ap_shop_locations[0];
    configure_shop(first, first->kong == AP_SHOP_SHARED ? 0 : first->kong, 17);
    put32(TEST_MODE_ADDRESS, 0);
    offer_at(0, first->id, 256, 0); CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 1, RESTORED) == 0);
    offer_at(0, first->id, 17, 1); CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 1, RESTORED) == 0);
    offer_at(0, first->id, 17, 0); offer_at(1, first->id, 17, 0);
    CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 2, RESTORED) == 0);
    offer_at(1, 14041180, 0, 0); CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 2, RESTORED) == 0);
    CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 121, RESTORED) == 0);
    CHECK(invoke(test_ap_prepare_ap_shops, 0, 1, RESTORED) == 0);
    restore_bit(14041180); CHECK(invoke(test_ap_apshop_restore, RESTORED, 0, 0) == 0); expect_shop_snapshot(0);
    CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 1, RESTORED) == 0);
    memset(memory + RESTORED - 0x80000000u, 0, sizeof(ap_check_set));
    put32(TEST_MODE_ADDRESS, 4); select_shop(); CHECK(get8(SCRIPT + 5) == 17);
    // Shared conflicts include every Kong, not only the matching DK flag.
    for (unsigned s = 0; s < AP_SHOP_COUNT; ++s) if (ap_shop_locations[s].kong == AP_SHOP_SHARED) {
        const ap_shop_location_def* shared = &ap_shop_locations[s];
        for (unsigned k = 0; k < AP_SHOP_COUNT; ++k) {
            const ap_shop_location_def* other = &ap_shop_locations[k];
            if (other->kong >= 5 || shared->vendor != other->vendor || shared->level != other->level) continue;
            put32(TEST_MODE_ADDRESS, 0); offer_at(0, shared->id, 0, 0); offer_at(1, other->id, 0, 0);
            CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 2, RESTORED) == 0);
        }
    }
    // Restore a previously journaled check before the first offer. No re-grant.
    configure_shop(first, first->kong == AP_SHOP_SHARED ? 0 : first->kong, 0);
    restore_bit(first->id); CHECK(invoke(test_ap_apshop_restore, RESTORED, 0, 0) == 1);
    select_shop(); buy_shop(); expect_shop_snapshot(first->id);
    put32(TEST_MODE_ADDRESS, 0); CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 1, RESTORED) == 1);
    put32(TEST_MODE_ADDRESS, 4); select_shop(); CHECK((signed char)get8(SCRIPT + 0xb) < 0);
    for (unsigned price = 0; price <= 255; price += 255) {
        configure_shop(first, first->kong == AP_SHOP_SHARED ? 0 : first->kong, price); select_shop();
        CHECK(invoke(test_func_menu_800261C0, SCRIPT, 0, 0) == 2); buy_shop(); expect_shop_snapshot(first->id);
    }
    // A full individual-shop selection can coexist; buying one Kong's check
    // must leave the other Kongs' offers available at the same vendor/level.
    setup(0, 0, 7); unsigned offer_count = 0;
    for (unsigned i = 0; i < AP_SHOP_COUNT; ++i) if (ap_shop_locations[i].kong < 5)
        offer_at(offer_count++, ap_shop_locations[i].id, 0, 0);
    CHECK(offer_count == 100);
    CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, offer_count, RESTORED) == 1);
    put32(TEST_MODE_ADDRESS, 4); put32(ACTOR + 0x58, 0xBD); shop_world = 0; shop_coins = 0;
    select_shop(); buy_shop();
    for (unsigned k = 1; k < 5; ++k) {
        put8(0x8074E77C, k); select_shop(); CHECK(get8(SCRIPT + 0xb) == AP_SHOP_ITEM_TYPE);
    }
    // Combined move/reward/shop capabilities still use the three-bit save path.
    configure_shop(first, first->kong == AP_SHOP_SHARED ? 0 : first->kong, 17);
    save_setup(7, 1, 0, 608);
    put8(0x8074E77C, first->kong == AP_SHOP_SHARED ? 0 : first->kong);
    put32(ACTOR + 0x58, 0xBD + first->vendor); shop_world = first->level; shop_coins = 17;
    select_shop(); buy_shop(); CHECK(save_commits == 1); expect_shop_snapshot(first->id);
    memset(memory + 0x7FC950, 0, 0x306); invoke(test_func_global_asm_8060DC3C, 0, 0, 0);
    for (unsigned k = 0; k < 5; ++k) CHECK(get8(0x807FC950 + k*0x5e) == (1 + k));
    // Invalid game context never falls back to granting a vanilla shop reward.
    for (unsigned invalid = 0; invalid < 7; ++invalid) {
        configure_shop(first, first->kong == AP_SHOP_SHARED ? 0 : first->kong, 0); select_shop();
        if (invalid == 0) put8(0x807467C8, 1);
        if (invalid == 1) put8(0x80755318, 0);
        if (invalid == 2) put8(0x80755314, 0);
        if (invalid == 3) put32(0x807FBB4C, 0);
        if (invalid == 4) put32(ACTOR + 0x58, 0xBA);
        if (invalid == 5) shop_world = 8;
        if (invalid == 6) put8(0x8074E77C, 5);
        buy_shop(); expect_shop_snapshot(0);
    }
    // Execute the NPC's actual accepted-purchase state, including the vendor
    // animation hook, rather than only calling the inner purchase function.
    func_menu_80026250 = baseline_func_menu_80026250;
    func_global_asm_806F8D58 = noop; spawnActor = shop_spawn; playCutscene = shop_cutscene;
    for (unsigned vendor = 0; vendor < 3; ++vendor) {
        const ap_shop_location_def* def = NULL;
        for (unsigned i = 0; i < AP_SHOP_COUNT && !def; ++i) if (ap_shop_locations[i].vendor == vendor && ap_shop_locations[i].kong == 0) def = &ap_shop_locations[i];
        CHECK(def != NULL); configure_shop(def, 0, 0); select_shop();
        put8(SCRIPT + 0xe, 10); shop_spawns = shop_cutscenes = 0;
        memcpy(inventory, memory + 0x7FC950, sizeof(inventory));
        invoke(test_func_menu_80026874, SCRIPT, 0x807FC950, 0);
        expect_shop_snapshot(def->id); CHECK(!memcmp(inventory, memory + 0x7FC950, sizeof(inventory)));
        CHECK(shop_cutscenes == 1 && shop_spawns == (vendor ? 1 : 0));
        CHECK(get8(SCRIPT + 0xd) == 2 && get8(SCRIPT + 0xe) == 0);
        CHECK(actions == (vendor == 1 ? 0 : 1));
        CHECK(animations == (vendor == 1 ? 1 : 0));
    }
    // Off-mode outer state remains equivalent to the independent original ROM.
    for (unsigned baseline = 0; baseline < 2; ++baseline) {
        setup(0, 0, 0); put8(0x807FC928, 0); put32(ACTOR + 0x58, 0xBD);
        put32(0x807FC924, COUNTER + 0x100); put32(COUNTER + 0x104, PLAYER);
        put8(SCRIPT + 0xe, 10); put8(SCRIPT + 0x11, 1); shop_spawns = shop_cutscenes = 0;
        invoke(baseline ? baseline_func_menu_80026874 : test_func_menu_80026874, SCRIPT, 0x807FC950, 0);
        CHECK(get8(0x807FC950) == 1 && get8(SCRIPT + 0xd) == 2 && get8(SCRIPT + 0xe) == 0);
        CHECK(shop_cutscenes == 1 && actions == 1 && last_action == 0x52 && shop_spawns == 0);
    }
}
