// One-receipt transaction proofs executed from the linked MIPS payload.
#include "../mod/campaign_transaction.h"

static unsigned expected_transaction_classes(const ap_item_effect_def* item) {
    unsigned result = 0;
    for (unsigned i = 0; i < 2; ++i) switch (item->effects[i].kind) {
        case AP_EFFECT_UNSUPPORTED: return 0;
        case AP_EFFECT_NONE: break;
        case AP_EFFECT_FLAG: case AP_EFFECT_KONG: case AP_EFFECT_KEY: case AP_EFFECT_ABILITY:
        case AP_EFFECT_SPECIAL: case AP_EFFECT_HINT: result |= AP_TRANSACTION_OWNERSHIP; break;
        case AP_EFFECT_BLUEPRINT: case AP_EFFECT_FAIRY: case AP_EFFECT_CROWN: case AP_EFFECT_MEDAL:
        case AP_EFFECT_PEARL: case AP_EFFECT_RAINBOW: case AP_EFFECT_JUNK:
            result |= AP_TRANSACTION_COUNTED; break;
        case AP_EFFECT_TRAP: result |= AP_TRANSACTION_TRAP; break;
        case AP_EFFECT_TRANSFER:
            result |= item->effects[i].arg0 == 1 ? AP_TRANSACTION_COUNTED : AP_TRANSACTION_OWNERSHIP; break;
        default: return 0;
    }
    return result ? result : AP_TRANSACTION_NOOP;
}

static int expected_executable_commit(const ap_item_effect_def* item) {
    int has_effect = 0;
    for (unsigned i = 0; i < 2; ++i) {
        if (item->effects[i].kind == AP_EFFECT_UNSUPPORTED) return 0;
        if (item->effects[i].kind == AP_EFFECT_NONE) continue;
        has_effect = 1;
        if (item->effects[i].kind == AP_EFFECT_ABILITY || item->effects[i].kind == AP_EFFECT_KONG
            || item->effects[i].kind == AP_EFFECT_KEY || item->effects[i].kind == AP_EFFECT_SPECIAL
            || item->effects[i].kind == AP_EFFECT_HINT || item->effects[i].kind == AP_EFFECT_BLUEPRINT
            || item->effects[i].kind == AP_EFFECT_FAIRY || item->effects[i].kind == AP_EFFECT_CROWN
            || item->effects[i].kind == AP_EFFECT_MEDAL || item->effects[i].kind == AP_EFFECT_PEARL
            || item->effects[i].kind == AP_EFFECT_RAINBOW || item->effects[i].kind == AP_EFFECT_JUNK) continue;
        if (item->effects[i].kind == AP_EFFECT_FLAG) continue;
        if (item->effects[i].kind == AP_EFFECT_TRAP) {
            if (!expected_trap_executable(item->effects[i].arg0)) return 0;
            continue;
        }
        if (item->effects[i].kind != AP_EFFECT_TRANSFER) return 0;
    }
    return 1; // Includes the canonical No Item receipt.
}

static unsigned transaction_stage4(uint32_t identity, uint32_t before, uint32_t item, unsigned valid) {
    recomp_context ctx = {0};
    ctx.r4 = (int32_t)identity; ctx.r5 = (int32_t)before; ctx.r6 = item; ctx.r7 = valid;
    ctx.r29 = (int32_t)0x81ff0000; ctx.f_odd = &ctx.f0.u32h;
    test_ap_campaign_transaction_stage(memory, &ctx);
    return ctx.r2;
}

static void test_campaign_transaction(void) {
    const uint32_t identity = PAYLOAD + 0x1800, before = PAYLOAD + 0x1880;
    const uint32_t plan = PAYLOAD + 0x1A00, expected = PAYLOAD + 0x1D00;
    setup(0, 0, 0);
    write_campaign_header(identity, 37, 61);
    invoke(test_ap_campaign_inventory_clear, before, 0, 0);
    unsigned accepted = 0, rejected = 0, noops = 0, ownership = 0, counted = 0, traps = 0;
    unsigned char unchanged[sizeof(ap_campaign_transaction)];
    for (unsigned i = 0; i < AP_ITEM_COUNT; ++i) {
        memset(memory + plan - 0x80000000u, 0xA5, sizeof(ap_campaign_transaction));
        memcpy(unchanged, memory + plan - 0x80000000u, sizeof(unchanged));
        unsigned classes = expected_transaction_classes(&ap_item_effects[i]);
        recomp_context ctx = {0};
        ctx.r4 = (int32_t)plan; ctx.r5 = (int32_t)identity; ctx.r6 = (int32_t)before; ctx.r7 = ap_item_ids[i];
        ctx.r29 = (int32_t)0x81ff0000; ctx.f_odd = &ctx.f0.u32h;
        test_ap_campaign_transaction_build(memory, &ctx);
        unsigned result = ctx.r2;
        CHECK(result == !!classes);
        if (!classes) {
            ++rejected; CHECK(!memcmp(unchanged, memory + plan - 0x80000000u, sizeof(unchanged))); continue;
        }
        ++accepted;
        if (classes & AP_TRANSACTION_NOOP) ++noops;
        if (classes & AP_TRANSACTION_OWNERSHIP) ++ownership;
        if (classes & AP_TRANSACTION_COUNTED) ++counted;
        if (classes & AP_TRANSACTION_TRAP) ++traps;
        CHECK(get32(plan) == AP_CAMPAIGN_TRANSACTION_MAGIC);
        CHECK(get16(plan + 4) == AP_CAMPAIGN_TRANSACTION_FORMAT);
        CHECK(get16(plan + 6) == classes);
        CHECK(get32(plan + 8) == ap_item_ids[i] && get32(plan + 12) == 0);
        check_campaign_header(plan + AP_OFFSETOF(ap_campaign_transaction, current), 37, 61);
        check_campaign_header(plan + AP_OFFSETOF(ap_campaign_transaction, next), 38, 61);
        CHECK(!memcmp(memory + plan + AP_OFFSETOF(ap_campaign_transaction, before) - 0x80000000u,
            memory + before - 0x80000000u, sizeof(ap_campaign_inventory)));
        memcpy(memory + expected - 0x80000000u, memory + before - 0x80000000u, sizeof(ap_campaign_inventory));
        CHECK(invoke(test_ap_campaign_inventory_add, expected, ap_item_ids[i], 0) == 1);
        CHECK(!memcmp(memory + plan + AP_OFFSETOF(ap_campaign_transaction, after) - 0x80000000u,
            memory + expected - 0x80000000u, sizeof(ap_campaign_inventory)));
    }
    CHECK(accepted == 165 && rejected == 19 && noops == 1);
    CHECK(ownership == 117 && counted == 17 && traps == 30);

    memset(memory + plan - 0x80000000u, 0x5A, sizeof(ap_campaign_transaction));
    memcpy(unchanged, memory + plan - 0x80000000u, sizeof(unchanged));
    CHECK(invoke(test_ap_campaign_transaction_build, plan + 1, identity, before) == 0);
    CHECK(!memcmp(unchanged, memory + plan - 0x80000000u, sizeof(unchanged)));
    write_campaign_header(identity, 0xFFFF, 61);
    recomp_context ctx = {0}; ctx.r4 = (int32_t)plan; ctx.r5 = (int32_t)identity; ctx.r6 = (int32_t)before;
    ctx.r7 = 14041104u; ctx.r29 = (int32_t)0x81ff0000; ctx.f_odd = &ctx.f0.u32h;
    test_ap_campaign_transaction_build(memory, &ctx); CHECK(ctx.r2 == 0);

    // Publication is identity-bound and distinguishes pending from complete.
    write_campaign_header(identity, 0, 62); write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 62);
    invoke(test_ap_campaign_transaction_reset, 0, 0, 0);
    ctx = (recomp_context){0}; ctx.r4 = (int32_t)identity; ctx.r5 = (int32_t)before; ctx.r6 = 14041104u;
    ctx.r7 = 1; ctx.r29 = (int32_t)0x81ff0000; ctx.f_odd = &ctx.f0.u32h;
    test_ap_campaign_transaction_stage(memory, &ctx); CHECK(ctx.r2 == 1);
    CHECK(invoke(test_ap_campaign_transaction_snapshot, plan, identity, 0) == 1);
    CHECK(get32(plan + 8) == 14041104u);
    put32(TEST_MODE_ADDRESS, 8);
    test_ap_campaign_transaction_stage(memory, &ctx); CHECK(ctx.r2 == 0);
    CHECK(invoke(test_ap_campaign_transaction_snapshot, plan, identity, 0) == 1);
    put32(TEST_MODE_ADDRESS, 0);
    ctx = (recomp_context){0}; ctx.r4 = (int32_t)identity; ctx.r5 = (int32_t)before; ctx.r6 = 0;
    ctx.r7 = 0; ctx.r29 = (int32_t)0x81ff0000; ctx.f_odd = &ctx.f0.u32h;
    test_ap_campaign_transaction_stage(memory, &ctx); CHECK(ctx.r2 == 1);
    memset(memory + plan - 0x80000000u, 0x3C, sizeof(ap_campaign_transaction));
    memcpy(unchanged, memory + plan - 0x80000000u, sizeof(unchanged));
    CHECK(invoke(test_ap_campaign_transaction_snapshot, plan, identity, 0) == 2);
    CHECK(!memcmp(unchanged, memory + plan - 0x80000000u, sizeof(unchanged)));
    write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 63);
    CHECK(invoke(test_ap_campaign_transaction_snapshot, plan, identity, 0) == 0);

    // Every catalog item is admitted or rejected by the executable commit.
    // Move state is projected into CharacterProgress; ability, Kong and key
    // state remains in the durable prefix consumed by the exact query hooks.
    unsigned committed = 0;
    for (unsigned i = 0; i < AP_ITEM_COUNT; ++i) {
        save_setup(0, 0, 0, 0);
        trap_make_safe();
        put16(PLAYER + 0x1A6, 100);
        write_campaign_header(identity, 0, 70); write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 70);
        invoke(test_ap_campaign_inventory_clear, before, 0, 0);
        invoke(test_ap_campaign_transaction_reset, 0, 0, 0);
        unsigned supported = expected_transaction_classes(&ap_item_effects[i]) != 0;
        CHECK(transaction_stage4(identity, before, ap_item_ids[i], 1) == supported);
        if (supported) CHECK(invoke(test_ap_campaign_transaction_snapshot, plan, identity, 0) == 1);
        for (unsigned kong = 0; kong < 5; ++kong) {
            uint32_t progress = 0x807FC950 + 0x5e * kong;
            put8(progress, 0xA7); put8(progress + 1, 0x55); put8(progress + 2, 0xA7);
            put8(progress + 3, 0x55); put8(progress + 4, 0xAF);
        }
        put8(0x807FC950 + 0x2fc, 1);
        for (unsigned kong = 0; kong < 5; ++kong) put16(0x807FC950 + 0x5e * kong + 8, 70 + kong);
        put16(0x807FCC40, 101); put16(0x807FCC42, 102); put16(0x807FCC44, 103);
        put16(0x807FCC46, 104); put16(0x807FCC48, 105); put16(0x807FCC4E, 106);
        unsigned char live_before[0x306];
        memcpy(live_before, memory + 0x7FC950, sizeof(live_before));
        unsigned gb_before = 0;
        for (unsigned kong = 0; kong < 5; ++kong)
            for (unsigned level = 0; level < 9; ++level)
                gb_before += get16(0x807FC950 + 0x5e * kong + 0x42 + 2 * level);
        if (supported) {
            put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_MOVES | AP_GAMEPLAY_SAVE);
            CHECK(invoke(test_ap_campaign_transaction_commit, plan, 0, 0) == 0);
            CHECK(save_commits == 0);
            CHECK(!memcmp(live_before, memory + 0x7FC950, sizeof(live_before)));
            check_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 70);
        }
        unsigned has_supported_trap = 0, is_dry = 0, is_ice_floor = 0;
        for (unsigned effect = 0; effect < 2; ++effect) if (ap_item_effects[i].effects[effect].kind == AP_EFFECT_TRAP) {
            has_supported_trap = expected_trap_executable(ap_item_effects[i].effects[effect].arg0);
            is_dry = ap_item_effects[i].effects[effect].arg0 == AP_TRAP_TYPE_DRY;
            is_ice_floor = ap_item_effects[i].effects[effect].arg0 == AP_TRAP_TYPE_ICEFLOOR;
        }
        put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_MOVES | AP_GAMEPLAY_SAVE | AP_GAMEPLAY_ITEMS
            | (has_supported_trap ? AP_GAMEPLAY_TRAPS : 0));
        unsigned result = supported ? invoke(test_ap_campaign_transaction_commit, plan, 0, 0) : 0;
        unsigned expected_commit = expected_executable_commit(&ap_item_effects[i]);
        int expected_flag = -1;
        for (unsigned effect = 0; effect < 2; ++effect)
            if (ap_item_effects[i].effects[effect].kind == AP_EFFECT_FLAG)
                expected_flag = (int)ap_item_effects[i].effects[effect].arg1;
        CHECK(result == expected_commit);
        if (!expected_commit) {
            CHECK(save_commits == 0);
            CHECK(flag_writes == 0);
            CHECK(!memcmp(live_before, memory + 0x7FC950, sizeof(live_before)));
            check_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 70);
            continue;
        }
        ++committed;
        CHECK(save_commits == 1);
        CHECK(flag_writes == (unsigned)(expected_flag >= 0));
        if (expected_flag >= 0)
            CHECK(last_flag == (unsigned)expected_flag && last_flag_value == 1 && last_flag_type == 0);
        check_campaign_header(TEST_CAMPAIGN_TRAILER, 1, 70);
        check_campaign_header(0x807EDE88, 1, 70);
        unsigned melon_target = 1;
        for (unsigned kong = 0; kong < 5; ++kong) {
            uint32_t progress = 0x807FC950 + 0x5e * kong;
            uint32_t after = plan + AP_OFFSETOF(ap_campaign_transaction, after);
            unsigned moves = get8(after + AP_OFFSETOF(ap_campaign_inventory, moves) + kong);
            unsigned guns = get8(after + AP_OFFSETOF(ap_campaign_inventory, guns) + kong);
            unsigned instruments = get8(after + AP_OFFSETOF(ap_campaign_inventory, instruments) + kong);
            CHECK(get8(progress) == (0xA0 | moves));
            CHECK(get8(progress + 1) == get8(after + AP_OFFSETOF(ap_campaign_inventory, slam)));
            CHECK(get8(progress + 2) == (0xA0 | guns));
            CHECK(get8(progress + 3) == get8(after + AP_OFFSETOF(ap_campaign_inventory, belt)));
            CHECK(get8(progress + 4) == (0xA0 | instruments));
            if (instruments & 3u) melon_target = melon_target < 2 ? 2 : melon_target;
            if (instruments & 12u) melon_target = 3;
        }
        CHECK(get8(0x807FC950 + 0x2fc) == melon_target);
        unsigned gb_after = 0;
        for (unsigned kong = 0; kong < 5; ++kong) {
            for (unsigned level = 0; level < 9; ++level)
                gb_after += get16(0x807FC950 + 0x5e * kong + 0x42 + 2 * level);
            unsigned expected_coins = 50 + kong;
            for (unsigned effect = 0; effect < 2; ++effect)
                if (ap_item_effects[i].effects[effect].kind == AP_EFFECT_RAINBOW) expected_coins += 5;
            CHECK(get16(0x807FC950 + 0x5e * kong + 6) == expected_coins);
            CHECK(get16(0x807FC950 + 0x5e * kong + 8) == (is_dry ? 0 : 70 + kong));
        }
        CHECK(get16(0x807FCC40) == (is_dry ? 0 : 101));
        CHECK(get16(0x807FCC42) == (is_dry ? 0 : 102));
        CHECK(get16(0x807FCC44) == (is_dry ? 0 : 103));
        CHECK(get16(0x807FCC46) == (is_dry ? 0 : 104));
        CHECK(get16(0x807FCC48) == (is_dry ? 0 : 105));
        CHECK(get16(0x807FCC4E) == (is_dry ? 0 : 106));
        CHECK(get16(PLAYER + 0x1A6) == (is_ice_floor ? 1 : 100));
        unsigned expected_gb_delta = 0;
        for (unsigned effect = 0; effect < 2; ++effect)
            if (ap_item_effects[i].effects[effect].kind == AP_EFFECT_TRANSFER
                && ap_item_effects[i].effects[effect].arg0 == 1) expected_gb_delta = 1;
        CHECK(gb_after == gb_before + expected_gb_delta);
        CHECK(invoke(test_ap_campaign_transaction_snapshot, plan, identity, 0) == 0);
    }
    CHECK(committed == 149);

    // Replaying an already-owned permanent receipt advances the exact receipt
    // prefix without repeating the stock flag write.
    save_setup(0, 0, 0, 0);
    write_campaign_header(identity, 0, 71); write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 71);
    invoke(test_ap_campaign_inventory_clear, before, 0, 0);
    CHECK(invoke(test_ap_campaign_inventory_add, before, 14041100u, 0) == 1); // Climbing
    invoke(test_ap_campaign_transaction_reset, 0, 0, 0);
    CHECK(transaction_stage4(identity, before, 14041100u, 1) == 1);
    CHECK(invoke(test_ap_campaign_transaction_snapshot, plan, identity, 0) == 1);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_MOVES | AP_GAMEPLAY_SAVE | AP_GAMEPLAY_ITEMS);
    CHECK(invoke(test_ap_campaign_transaction_commit, plan, 0, 0) == 1);
    CHECK(flag_writes == 0 && save_commits == 1);
    check_campaign_header(TEST_CAMPAIGN_TRAILER, 1, 71);
}
