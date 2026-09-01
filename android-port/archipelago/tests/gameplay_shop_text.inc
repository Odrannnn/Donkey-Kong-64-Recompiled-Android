// Original shop callers and text-bank traversal, linked to the actual NRM.
static void text_expand(uint8_t*, recomp_context*);
#define func_global_asm_8070DED4 text_expand
#define func_global_asm_8070E34C unused_baseline_path
#define func_menu_80026DAC unused_baseline_path
#include "gameplay-flow.c"
#define TEXT_ACTOR 0x80907800u
#define TEXT_DATA 0x80907A00u
#define TEXT_NODE 0x80907B00u
#define TEXT_CACHE 0x80907C00u
static unsigned grounded, ground_sets, text_loads, text_index, text_file_id, cancellations, text_flags, text_flag_id, shown_resources;
static char expanded_text[256];
static void text_expand(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK(ctx->r4 == (int32_t)TEXT_DATA); CHECK(ctx->r6 > 0 && ctx->r6 < sizeof(expanded_text));
    for (unsigned i = 0; i < ctx->r6; ++i) expanded_text[i] = get8((uint32_t)ctx->r5 + i);
    expanded_text[ctx->r6] = 0;
    // Only glyph/layout allocation is stubbed; original E548/E2AC traverse
    // the mod's real relocated text structure and its length/offset fields.
    put32(TEXT_DATA + 0xc, TEXT_NODE); put32(TEXT_NODE + 0x1c, 0);
}
static void text_load(uint8_t* rdram, recomp_context* ctx) {
    CHECK(ctx->r4 == (int32_t)ACTOR); ++text_loads;
    text_file_id = ctx->r5; text_index = ctx->r6;
    put32(ACTOR + 0x60, get32(ACTOR + 0x60) | 0x02000000);
    if (text_file_id == 0xfffe) {
        put32(TEXT_ACTOR + 0x174, TEXT_DATA); put32(TEXT_DATA + 0xc, 0);
        put8(0x807550C8, 0); // US game's only supported language
        ctx->r4 = (int32_t)TEXT_ACTOR;
        flow_func_global_asm_8070E548(rdram, ctx);
        put32(TEXT_ACTOR + 0x60, 0x10); // next message releases its old file
    }
}
static void text_ground(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = grounded; }
static void text_ground_set(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; ++ground_sets; }
static void text_cancel(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; ++cancellations; }
static void text_flag(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK(ctx->r5 == 1 && ctx->r6 == 0); ++text_flags; text_flag_id = ctx->r4;
}
static void text_resource(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK((ctx->r4 == 1 || ctx->r4 == 14) && ctx->r5 == 1 && ctx->r6 == 0); shown_resources |= 1u << ctx->r4;
}
static void text_cutscene(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK((ctx->r5 == 2 || ctx->r5 == 15) && ctx->r6 == 1); ++shop_cutscenes;
}
static void dialogue_setup(const ap_shop_location_def* def, unsigned buyer, unsigned price) {
    configure_shop(def, buyer, price); select_shop();
    grounded = 1; ground_sets = text_loads = cancellations = text_flags = shown_resources = 0;
    put32(0x807550C0, 0); put32(0x807FBB60, 0x80000000); expanded_text[0] = 0;
    func_global_asm_80629148 = text_ground; func_global_asm_80629174 = text_ground_set;
    loadText = text_load; func_global_asm_8061CB08 = text_cancel; setFlag = text_flag;
    func_global_asm_806F833C = noop; func_global_asm_806F8BC4 = text_resource;
    func_menu_80026804 = flow_func_menu_80026804;
    func_global_asm_8070D928 = flow_func_global_asm_8070D928;
    playCutscene = text_cutscene;
}
static unsigned intro_for(const ap_shop_location_def* def) { return def->vendor == 0 ? (def->level == 7 ? 0x17f : 0x177) : def->vendor == 1 ? 0x175 : 0x176; }
static void dialogue_step(unsigned intro) { invoke(flow_func_menu_80027028, SCRIPT, 0x807FC950, intro); }
static void close_text(void) { put32(ACTOR + 0x60, get32(ACTOR + 0x60) & ~0x02000000u); }
static void test_shop_text(void) {
    unsigned char inventory[0x306];
    for (unsigned s = 0; s < AP_SHOP_COUNT; ++s) {
        const ap_shop_location_def* def = &ap_shop_locations[s];
        for (unsigned buyer = 0; buyer < 5; ++buyer) {
            if (def->kong != AP_SHOP_SHARED && def->kong != buyer) continue;
            dialogue_setup(def, buyer, 17); unsigned intro = intro_for(def);
            memcpy(inventory, memory + 0x7FC950, sizeof(inventory));
            grounded = 0; dialogue_step(intro); CHECK(!text_loads && !ground_sets && !get8(SCRIPT + 0xe));
            grounded = 1; dialogue_step(intro);
            CHECK(text_file_id == 0xfffe && text_index == 0 && text_loads == 1 && ground_sets == 1);
            CHECK(strstr(expanded_text, "ENOUGH COINS") && strstr(expanded_text, "WILL NOT BE SPENT"));
            CHECK(shown_resources == ((1u << 1) | (1u << 14)) && get8(SCRIPT + 0xf) == 1);
            dialogue_step(intro); CHECK(text_loads == 1 && get8(SCRIPT + 0xd) == 0);
            close_text(); dialogue_step(intro); CHECK(get8(SCRIPT + 0xd) == 1 && get8(SCRIPT + 0xe) == 0 && cancellations == 1);
            // Accepted input reaches the original NPC animation branch.
            put8(SCRIPT + 0xe, 10); dialogue_step(intro); expect_shop_snapshot(def->id);
            CHECK(get8(SCRIPT + 0xd) == 2 && get8(SCRIPT + 0xe) == 0);
            unsigned player_props = get32(PLAYER + 0x60);
            grounded = 0; dialogue_step(intro); CHECK(text_loads == 1 && get32(PLAYER + 0x60) == player_props);
            grounded = 1; dialogue_step(intro);
            CHECK(text_index == 2 && text_loads == 2 && get8(SCRIPT + 0xe) == 1);
            CHECK(strstr(expanded_text, "PAY ATTENTION, ~.") && strstr(expanded_text, "K. ROOL"));
            CHECK(get32(0x807FBB60) == 0x80010030 && !(get32(PLAYER + 0x60) & 0x40000000));
            dialogue_step(intro); CHECK(text_flags == 0 && cancellations == 1);
            close_text(); dialogue_step(intro); CHECK(text_flags == 1 && text_flag_id == intro && cancellations == 2 && get8(SCRIPT + 0xe) == 2);
            grounded = 0; dialogue_step(intro); CHECK(get8(SCRIPT + 0xe) == 2);
            grounded = 1; dialogue_step(intro);
            CHECK(get8(SCRIPT + 0xd) == 0 && get8(SCRIPT + 0xe) == 0 && (signed char)get8(SCRIPT + 0xb) < 0);
            CHECK(get8(SCRIPT + 0xa) == 1 && text_flags == 1 && text_loads == 2);
            CHECK(!memcmp(inventory, memory + 0x7FC950, sizeof(inventory)) && shop_coins == 17);
            expect_shop_snapshot(def->id);
            CHECK(get32(0x807550C0) == 0); // no heap cache owns mod-static text
        }
    }
    const ap_shop_location_def* def = &ap_shop_locations[0];
    unsigned buyer = def->kong < 5 ? def->kong : 0, intro = intro_for(def);
    // Unaffordable offers do not open the purchase menu or report a check.
    for (unsigned price = 1; price <= 255; price += 127) {
        dialogue_setup(def, buyer, price); shop_coins = price - 1; dialogue_step(intro);
        CHECK(text_index == 1 && strstr(expanded_text, "NEED MORE COINS") && get8(SCRIPT + 0xf) == 3);
        close_text(); dialogue_step(intro); CHECK(get8(SCRIPT + 0xd) == 3); expect_shop_snapshot(0);
    }
    // A stale offer at the confirmation boundary cannot claim success.
    dialogue_setup(def, buyer, 17); put8(SCRIPT + 5, 18);
    invoke(test_func_menu_800262A8, SCRIPT, 0x807FC950, 0);
    CHECK(!text_loads && !text_flags && get8(SCRIPT + 5) == 17); expect_shop_snapshot(0);
    // Rejected purchase animations recover selection without a success message.
    dialogue_setup(def, buyer, 17); shop_coins = 0; put8(SCRIPT + 0xd, 1); put8(SCRIPT + 0xe, 10);
    dialogue_step(intro); dialogue_step(intro);
    CHECK(!text_loads && !text_flags && get8(SCRIPT + 0xd) == 0); expect_shop_snapshot(0);
    // Intro flag validation cannot write AP flags or another vendor's flag.
    const unsigned bad_flags[] = {0, 0x320, 0x26b, 0xffff, 0x175};
    for (unsigned i = 0; i < sizeof(bad_flags)/sizeof(*bad_flags); ++i) {
        dialogue_setup(def, buyer, 0); buy_shop(); put8(SCRIPT + 0xe, 1);
        invoke(test_func_menu_80026B28, SCRIPT, 0, bad_flags[i]); CHECK(text_flags == 0 && get8(SCRIPT + 0xe) == 2);
    }
    // Context loss pauses, without writing state/flags or attempting stock AP text.
    for (unsigned invalid = 0; invalid < 5; ++invalid) {
        dialogue_setup(def, buyer, 0); buy_shop(); put8(SCRIPT + 0xe, 0);
        if (invalid == 0) put32(0x807FBB4C, 0);
        if (invalid == 1) put32(0x807FBB40, 0);
        if (invalid == 2) put8(0x807467C8, 1);
        if (invalid == 3) put8(0x80755318, 0);
        if (invalid == 4) shop_world = 8;
        invoke(test_func_menu_800262A8, SCRIPT, 0, 0); invoke(test_func_menu_80026B28, SCRIPT, 0, intro);
        CHECK(!text_loads && !text_flags && !ground_sets && get8(SCRIPT + 0xe) == 0);
    }
    // Original text-cache hits retain stock refcounts in every gate mode.
    for (unsigned mode = 0; mode <= 7; ++mode) for (unsigned policy = 0; policy <= 2; ++policy) {
        setup(mode, 0, 0); func_global_asm_8070D928 = flow_func_global_asm_8070D928;
        put32(0x807550C0, TEXT_CACHE); put16(TEXT_CACHE, 8); put16(TEXT_CACHE + 2, 5);
        put32(TEXT_CACHE + 4, TEXT_DATA); put8(TEXT_CACHE + 8, policy); put32(TEXT_CACHE + 0xc, 0);
        CHECK(invoke(test_func_global_asm_8070DDDC, 8, 0, 0) == TEXT_DATA);
        unsigned count = get16(TEXT_CACHE + 2);
        put16(TEXT_CACHE + 2, 5);
        CHECK(invoke(baseline_func_global_asm_8070DDDC, 8, 0, 0) == TEXT_DATA && get16(TEXT_CACHE + 2) == count);
        unsigned bank = invoke(test_func_global_asm_8070DDDC, 0xfffe, 0, 0);
        put32(TEST_MODE_ADDRESS, 0);
        CHECK(invoke(test_func_global_asm_8070DDDC, 0xfffe, 0, 0) == bank);
        CHECK(get32(0x807550C0) == TEXT_CACHE && get16(TEXT_CACHE + 2) == count);
        invoke(flow_func_global_asm_8070D970, 0xfffe, 0, 0); CHECK(get16(TEXT_CACHE + 2) == count);
    }
    // Off-mode and non-AP post dialogue use the independent original behavior.
    for (unsigned mode = 0; mode <= 4; mode += 4) for (unsigned oracle = 0; oracle < 2; ++oracle) {
        dialogue_setup(def, buyer, 0); put32(TEST_MODE_ADDRESS, mode); put8(SCRIPT + 0xb, 0); put8(SCRIPT + 0xe, 1);
        put32(ACTOR + 0x60, 0x02000000);
        invoke(oracle ? baseline_func_menu_80026B28 : test_func_menu_80026B28, SCRIPT, 0, intro);
        CHECK(!text_flags && !cancellations && get8(SCRIPT + 0xe) == 1);
        close_text(); invoke(oracle ? baseline_func_menu_80026B28 : test_func_menu_80026B28, SCRIPT, 0, intro);
        CHECK(text_flags == 1 && cancellations == 1 && get8(SCRIPT + 0xe) == 2);
        grounded = 0; put8(SCRIPT + 0xe, 0);
        invoke(oracle ? baseline_func_menu_800262A8 : test_func_menu_800262A8, SCRIPT, 0, 0);
        CHECK(!text_loads && !get8(SCRIPT + 0xe));
    }
    // Restore fixture services used by the next placement's existing tests.
    setFlag = flag_write; func_global_asm_806F8BC4 = noop;
}
