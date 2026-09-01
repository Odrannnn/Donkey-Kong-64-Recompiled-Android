// Original-randomizer CountStruct semantics over the receipt-derived prefix.
#include "../mod/campaign_items.h"

static unsigned item_count4(unsigned item, int level, int kong) {
    recomp_context ctx = {0};
    ctx.r4 = item; ctx.r5 = level; ctx.r6 = kong;
    ctx.r29 = (int32_t)0x81ff0000; ctx.f_odd = &ctx.f0.u32h;
    test_ap_campaign_item_count(memory, &ctx);
    return ctx.r2;
}

static void test_campaign_items(void) {
    const uint32_t identity = PAYLOAD + 0x2100, ids = PAYLOAD + 0x2200;
    setup(0, 0, 0);
    // Gate-off behavior retains stock permanent flags. The randomizer-only
    // camera flag has no stock storage and therefore remains false.
    flag_result = 1;
    CHECK(invoke(test_ap_campaign_has_flag_move, 0x182, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_has_flag_move, 0x123, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_has_flag_move, 0x2FD, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_original_item_count, 6, (uint32_t)-1, 0) == 0);
    CHECK(invoke(test_ap_campaign_get_kong_ownership_from_flag, 0x181, 0, 0) == 0);
    flag_result = 0;
    unsigned count = 0;
    for (unsigned i = 0; i < AP_ITEM_COUNT; ++i)
        if (ap_item_effects[i].effects[0].kind != AP_EFFECT_UNSUPPORTED
            && ap_item_effects[i].effects[1].kind != AP_EFFECT_UNSUPPORTED)
            put32(ids + 4 * count++, ap_item_ids[i]);
    CHECK(count == 165);
    write_campaign_header(identity, (uint16_t)count, 81);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 81);
    invoke(test_ap_campaign_items_reset, 0, 0, 0);
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, ids, count) == 1);
    CHECK(invoke(test_ap_campaign_items_stage, identity, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_KONG, -1, -1) == (unsigned)-1); // Gate is still off.
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_ITEMS);

    CHECK(item_count4(AP_ITEM_QUERY_KONG, -1, -1) == 5);
    CHECK(item_count4(AP_ITEM_QUERY_KEY, -1, 0) == 8);
    for (int i = 0; i < 5; ++i) CHECK(item_count4(AP_ITEM_QUERY_KONG, 0, i) == 1);
    for (int i = 0; i < 8; ++i) CHECK(item_count4(AP_ITEM_QUERY_KEY, i, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_HINT, -1, -1) == 35);
    for (int kong = 0; kong < 5; ++kong) CHECK(item_count4(AP_ITEM_QUERY_HINT, -1, kong) == 7);
    for (int level = 0; level < 7; ++level) CHECK(item_count4(AP_ITEM_QUERY_HINT, level, -1) == 5);
    CHECK(item_count4(AP_ITEM_QUERY_HINT, 7, -1) == 0);
    CHECK(item_count4(AP_ITEM_QUERY_BLUEPRINT, 0, -1) == 5);
    CHECK(item_count4(AP_ITEM_QUERY_FAIRY, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_CROWN, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_COMPANY_COIN, 0, -1) == 2);
    CHECK(item_count4(AP_ITEM_QUERY_COMPANY_COIN, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_COMPANY_COIN, 0, 1) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_MEDAL, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_BEAN, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_PEARL, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_RAINBOW_COIN, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_TRAP, -1, 0) == 30);
    CHECK(item_count4(AP_ITEM_QUERY_JUNK, 0, 0) == 6);
    CHECK(item_count4(AP_ITEM_QUERY_GOLDEN_BANANA, 0, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_SHOPKEEPER, 0, -1) == 4);
    CHECK(item_count4(AP_ITEM_QUERY_MOVE, 3, 0) == 3);
    CHECK(item_count4(AP_ITEM_QUERY_MOVE, 7, 0) == 2);
    CHECK(item_count4(AP_ITEM_QUERY_MOVE, 9, 0) == 3);
    for (int kong = 0; kong < 5; ++kong) {
        for (int level = 0; level < 3; ++level) CHECK(item_count4(AP_ITEM_QUERY_MOVE, level, kong) == 1);
        CHECK(item_count4(AP_ITEM_QUERY_MOVE, 4, kong) == 1);
        CHECK(item_count4(AP_ITEM_QUERY_MOVE, 8, kong) == 1);
    }
    for (int ability = 0; ability < 6; ++ability) CHECK(item_count4(AP_ITEM_QUERY_MOVE, 10, ability) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_MOVE, 11, 0) == 1);
    CHECK(item_count4(AP_ITEM_QUERY_MOVE, 12, 0) == 1);
    // Original requirement_item ABI used by the pinned pause/K. Lumsy edits.
    CHECK(invoke(test_ap_campaign_original_item_count, 1, 99, (uint32_t)-1) == 5);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 10, 4) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 3, 99) == 3);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 5, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 6, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 7, 99) == 2);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 9, 99) == 3);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 11, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 2, 12, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 3, 99, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 4, 99, (uint32_t)-1) == 5);
    CHECK(invoke(test_ap_campaign_original_item_count, 5, 99, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 6, (uint32_t)-1, 99) == 8);
    CHECK(invoke(test_ap_campaign_original_item_count, 7, 99, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 8, 99, (uint32_t)-1) == 2);
    CHECK(invoke(test_ap_campaign_original_item_count, 9, 99, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 10, 99, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 11, 99, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 12, 99, 99) == 1);
    CHECK(invoke(test_ap_campaign_original_item_count, 13, 99, 99) == 30);
    CHECK(invoke(test_ap_campaign_original_item_count, 18, 99, 99) == 6);
    CHECK(invoke(test_ap_campaign_original_item_count, 19, (uint32_t)-1, (uint32_t)-1) == 35);
    CHECK(invoke(test_ap_campaign_original_item_count, 20, 99, (uint32_t)-1) == 4);
    CHECK(invoke(test_ap_campaign_original_item_count, 21, 0, 0) == 0);
    const unsigned kong_flags[] = {0x181, 0x006, 0x046, 0x042, 0x075};
    for (unsigned kong = 0; kong < 5; ++kong)
        CHECK(invoke(test_ap_campaign_get_kong_ownership_from_flag, kong_flags[kong], 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_get_kong_ownership_from_flag, 0x123, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_item_has_flag, 671, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_item_has_flag, AP_ITEM_FLAG_WORDS * 32u, 0, 0) == 0);
    const unsigned ability_flags[] = {0x182, 0x184, 0x185, 0x183, 0x2FD, 0x179};
    for (unsigned i = 0; i < 6; ++i)
        CHECK(invoke(test_ap_campaign_has_flag_move, ability_flags[i], 0, 0) == 1);
    flag_result = 1;
    CHECK(invoke(test_ap_campaign_has_flag_move, 0x123, 0, 0) == 1);
    flag_result = 0;

    // The crystal-coconut stock call site is one of the exact upstream
    // hasFlagMove redirects. It observes receipt-derived Shockwave while the
    // independent-move and item capabilities are both active.
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_MOVES | AP_GAMEPLAY_ITEMS);
    CHECK(invoke(test_func_global_asm_806F6E58, 0, 0, 0) == 1);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_ITEMS);

    // Invalid selectors and unknown query kinds are rejected distinctly.
    CHECK(item_count4(AP_ITEM_QUERY_KONG, 1, 0) == (unsigned)-1);
    CHECK(item_count4(AP_ITEM_QUERY_KEY, 8, 0) == (unsigned)-1);
    CHECK(item_count4(AP_ITEM_QUERY_HINT, 8, 0) == (unsigned)-1);
    CHECK(item_count4(AP_ITEM_QUERY_BLUEPRINT, 0, 5) == (unsigned)-1);
    CHECK(item_count4(AP_ITEM_QUERY_MOVE, 13, 0) == (unsigned)-1);
    CHECK(item_count4(0xFFFFFFFFu, 0, 0) == (unsigned)-1);

    // A corrupted, advanced, or foreign trailer invalidates all queries.
    put8(TEST_CAMPAIGN_TRAILER + 9, get8(TEST_CAMPAIGN_TRAILER + 9) ^ 1);
    CHECK(item_count4(AP_ITEM_QUERY_KONG, -1, -1) == (unsigned)-1);
    CHECK(invoke(test_ap_campaign_has_flag_move, 0x182, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_original_item_count, 6, (uint32_t)-1, 0) == 0);
    CHECK(invoke(test_ap_campaign_get_kong_ownership_from_flag, 0x181, 0, 0) == 0);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)(count + 1), 81);
    CHECK(item_count4(AP_ITEM_QUERY_KEY, -1, 0) == (unsigned)-1);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 82);
    CHECK(invoke(test_ap_campaign_item_has_flag, 671, 0, 0) == (unsigned)-1);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 81);
    CHECK(item_count4(AP_ITEM_QUERY_KONG, -1, -1) == 5);
    put8(0x807467C8, 1);
    CHECK(item_count4(AP_ITEM_QUERY_KONG, -1, -1) == (unsigned)-1);
    put8(0x807467C8, 0);

    // Staging cannot replace a live publication while any capability is on.
    CHECK(invoke(test_ap_campaign_items_stage, identity, 0, 0) == 0);
    put32(TEST_MODE_ADDRESS, 0);
    invoke(test_ap_campaign_items_reset, 0, 0, 0);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_ITEMS);
    CHECK(item_count4(AP_ITEM_QUERY_KONG, -1, -1) == (unsigned)-1);
    put32(TEST_MODE_ADDRESS, 0);
}
