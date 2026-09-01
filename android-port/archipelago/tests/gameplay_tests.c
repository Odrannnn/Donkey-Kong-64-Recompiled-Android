// Execute the actual MIPS replacements after upstream offline recompilation.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gameplay-recompiled.c"
void strlen_recomp(uint8_t*, recomp_context*);
void bzero_recomp(uint8_t*, recomp_context*);
void rand_recomp(uint8_t*, recomp_context*);
void osWritebackDCacheAll_recomp(uint8_t*, recomp_context*);
void osEepromWrite_recomp(uint8_t*, recomp_context*);
static recomp_func_t** const rom_memcpy = &memcpy;
static recomp_func_t** const rom_memset = &memset;
#undef memcpy
#undef memset
#define memcpy_recomp (*rom_memcpy)
static void unused_baseline_path(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; fprintf(stderr, "Unexpected baseline helper path\n"); exit(1); }
#define _strchr unused_baseline_path
#define func_global_asm_80631C20 unused_baseline_path
#include "gameplay-baseline.c"

static unsigned checks, actions, animations, flag_writes, hud_updates, calls, last_action;
static unsigned last_flag, last_flag_value, last_flag_type;
static unsigned flag_result, collection_flag;
static uint8_t* memory;
#define CHECK(x) do { ++checks; if (!(x)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define ACTOR 0x80900000u
#define EXTRA 0x80901000u
#define SCRIPT 0x80902000u
#define PAYLOAD 0x80903000u
#define PLAYER 0x80904000u
#define MODEL 0x80905000u
#define HUD 0x80906000u
#define COUNTER 0x80907000u
static void put32(uint32_t a, uint32_t v) { memcpy(memory + a - 0x80000000u, &v, 4); }
static uint32_t get32(uint32_t a) { uint32_t v; memcpy(&v, memory + a - 0x80000000u, 4); return v; }
static void put16(uint32_t a, uint16_t v) { memcpy(memory + ((a - 0x80000000u) ^ 2), &v, 2); }
static uint16_t get16(uint32_t a) { uint16_t v; memcpy(&v, memory + ((a - 0x80000000u) ^ 2), 2); return v; }
static void put8(uint32_t a, uint8_t v) { memory[(a - 0x80000000u) ^ 3] = v; }
static uint8_t get8(uint32_t a) { return memory[(a - 0x80000000u) ^ 3]; }
void strlen_recomp(uint8_t* rdram, recomp_context* ctx) {
    uint32_t length = 0; while (MEM_B(length, ctx->r4)) ++length; ctx->r2 = length;
}
void bzero_recomp(uint8_t* rdram, recomp_context* ctx) {
    for (uint32_t i = 0; i < ctx->r5; ++i) MEM_B(i, ctx->r4) = 0;
}
void rand_recomp(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = 0; }
void osWritebackDCacheAll_recomp(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; }
void osEepromWrite_recomp(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; fprintf(stderr, "Unexpected EEPROM write\n"); exit(1); }
static void load_file(uint8_t* rdram, const char* path, uint32_t offset, uint32_t address, uint32_t size) {
    FILE* file = fopen(path, "rb"); CHECK(file != NULL); CHECK(!fseek(file, offset, SEEK_SET));
    uint8_t* input = malloc(size); CHECK(input != NULL); CHECK(fread(input, 1, size, file) == size);
    for (uint32_t i = 0; i < size; ++i) rdram[((address - 0x80000000u) + i) ^ 3] = input[i];
    free(input);
    fclose(file);
}
#include "gameplay-symbols.h"

static void unexpected(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; fprintf(stderr, "Unexpected game call\n"); exit(1); }
static void noop(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ++calls; ctx->r2 = 0; }
static void action(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ++actions; last_action = ctx->r4; }
static void animation(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; ++animations; }
static void flag(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = flag_result; }
static void kong(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = ctx->r4; }
static void zero(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = 0; }
static void one(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = 1; }
static void hud(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = (int32_t)HUD; }
static void capacity(uint8_t* rdram, recomp_context* ctx) { (void)rdram; ctx->r2 = 999; }
static void collection(uint8_t* rdram, recomp_context* ctx) { (void)rdram; put32((uint32_t)ctx->r4, 5); ctx->r2 = collection_flag; }
static void flag_write(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK(ctx->r5 == 1 && ctx->r6 == 0); ++flag_writes;
    last_flag = (unsigned)ctx->r4; last_flag_value = (unsigned)ctx->r5; last_flag_type = (unsigned)ctx->r6;
}
static void update_hud(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; ++hud_updates; }
static void bad_switch(const char* function, uint32_t address, uint32_t table) { fprintf(stderr, "Unresolved jump table %s %x/%x\n", function, address, table); exit(1); }

static void setup(unsigned mode, unsigned current_kong, unsigned moves) {
    memset(memory + 0x900000, 0, 0x8000);
    memset(memory + 0x7FC950, 0, 0x306 * 4);
    memset(memory + 0x7FBB70, 0, 0x300);
    put32(TEST_MODE_ADDRESS, mode);
    put32(0x807FBB40, ACTOR); put32(0x807FBB4C, PLAYER); put32(0x807FD580, EXTRA);
    put32(ACTOR + 4, MODEL); put32(ACTOR + 0x174, PAYLOAD); put32(PLAYER + 0x174, EXTRA);
    put32(ACTOR + 0x60, 0x10); put32(MODEL + 0x34, 0x3f800000); put32(MODEL + 0x3c, 0x3f800000);
    put8(0x807FC929, 0); put8(0x8074E77C, current_kong); put32(PLAYER + 0x58, current_kong + 2);
    put8(0x807FC950 + 0x5e * current_kong, moves);
    put8(0x807FD584, 0); put32(0x8076A0A8, 7); put32(0x80767CC0, 100);
    put8(0x807467C8, 0); put8(0x80755314, 6); put8(0x80755318, 6);
    put16(0x807FD63C, 0); put32(0x807FBB68, 0);
    for (unsigned i = 0; i < 15; ++i) put32(HUD + i * 0x30, COUNTER);
    put16(COUNTER, 10);
    actions = animations = flag_writes = hud_updates = calls = last_action = flag_result = collection_flag = 0;
    last_flag = last_flag_value = last_flag_type = 0;
}
static uint32_t invoke(recomp_func_t* fn, uint32_t a0, uint32_t a1, uint32_t a2) {
    recomp_context ctx = {0};
    ctx.r4 = (int32_t)a0; ctx.r5 = (int32_t)a1; ctx.r6 = (int32_t)a2;
    ctx.r29 = (int32_t)0x81ff0000; ctx.f_odd = &ctx.f0.u32h;
    fn(memory, &ctx);
    return ctx.r2;
}
#include "gameplay_shop_save.inc"
#include "gameplay_campaign_save.inc"
#include "gameplay_campaign_inventory.inc"
#include "gameplay_campaign_items.inc"
#include "gameplay_campaign_traps.inc"
#include "gameplay_campaign_transaction.inc"
#include "gameplay_campaign_checks.inc"
#include "gameplay_campaign_stage.inc"
#include "gameplay_ap_shops.inc"
#include "gameplay_shop_text.inc"
#include "gameplay_shop_overlay.inc"

int main(void) {
    memory = calloc(1, 0x2000000); CHECK(memory != NULL);
    load_file(memory, TEST_ROM, TEST_STATIC_ROM, TEST_STATIC_VRAM, TEST_STATIC_SIZE);
    load_file(memory, TEST_ROM, TEST_MENU_ROM, TEST_MENU_VRAM, TEST_MENU_SIZE); load_mod(memory);
    for (unsigned i = 0; i < sizeof(reference_symbol_funcs)/sizeof(*reference_symbol_funcs); ++i) reference_symbol_funcs[i] = unexpected;
    isFlagSet = flag; setAction = action; playAnimation = animation;
    func_global_asm_806C8D2C = kong; func_global_asm_806C8DE0 = zero;
    func_global_asm_806F6EDC = one; countSetFlags = zero;
    func_global_asm_806F7FD0 = hud;
    func_global_asm_805FEF10 = collection; setFlag = flag_write; func_global_asm_806F8278 = update_hud;
    func_global_asm_8067ACB4 = noop; func_global_asm_8065D254 = noop; renderActor = noop;
    switch_error = bad_switch;
    const char* test_slice = getenv("AP_GAMEPLAY_TEST");
    if (test_slice && !strcmp(test_slice, "campaign-items")) {
        // Struct publication assignments in the linked MIPS payload lower to
        // memcpy. Full-suite setup installs this stub in an earlier fixture;
        // focused execution must install it explicitly.
        *rom_memcpy = copy_memory;
        for (unsigned placement = 0; placement < 2; ++placement) {
            test_mod_delta = placement * 0x400000;
            load_mod(memory);
            test_campaign_items();
        }
        free(memory);
        printf("PASS: executable MIPS campaign-item query slice (%u assertions)\n", checks);
        return 0;
    }
    if (test_slice && !strcmp(test_slice, "campaign-transaction")) {
        setup_save_references();
        for (unsigned placement = 0; placement < 2; ++placement) {
            test_mod_delta = placement * 0x400000;
            load_mod(memory);
            test_campaign_transaction();
        }
        free(memory);
        printf("PASS: executable MIPS campaign-transaction slice (%u assertions)\n", checks);
        return 0;
    }
    if (test_slice && !strcmp(test_slice, "campaign-traps")) {
        func_global_asm_8060AC34 = noop;
        for (unsigned placement = 0; placement < 2; ++placement) {
            test_mod_delta = placement * 0x400000;
            load_mod(memory);
            test_campaign_traps();
        }
        free(memory);
        printf("PASS: executable MIPS campaign-trap slice (%u assertions)\n", checks);
        return 0;
    }
    for (unsigned placement = 0; placement < 2; ++placement) {
    test_mod_delta = placement * 0x400000;
    load_mod(memory);
    // Every bit pattern, every Kong and script level, including inverted conditions.
    for (unsigned mode = 0; mode < 2; ++mode) for (unsigned k = 0; k < 5; ++k) for (unsigned bits = 0; bits < 8; ++bits) {
        for (unsigned rank = 1; rank <= 3; ++rank) for (unsigned invert = 0; invert < 2; ++invert) {
            setup(mode, k, bits); put16(SCRIPT, 0x34 | (invert ? 0x8000 : 0)); put16(SCRIPT + 2, k); put16(SCRIPT + 4, rank);
            unsigned expected = mode ? !!(bits & (1u << (rank - 1))) : bits >= rank;
            CHECK(invoke(test_func_global_asm_8063EA14, SCRIPT, 0, 0) == (expected ^ invert));
            if (!mode) CHECK(invoke(baseline_func_global_asm_8063EA14, SCRIPT, 0, 0) == (expected ^ invert));
        }
        setup(mode, k, bits);
        unsigned rank = get8(0x80754238 + k);
        CHECK(rank >= 1 && rank <= 3);
        unsigned expected = mode ? !!(bits & (1u << (rank - 1))) : bits >= rank;
        CHECK(invoke(test_func_global_asm_806F6E58, k, 0, 0) == expected);
        if (!mode) CHECK(invoke(baseline_func_global_asm_806F6E58, k, 0, 0) == expected);
        flag_result = 1; CHECK(invoke(test_func_global_asm_806F6E58, k, 0, 0) == 1);
    }
    for (unsigned variant = 0; variant < 3; ++variant) for (unsigned bits = 0; bits < 8; ++bits) {
        unsigned mode = variant == 1;
        recomp_func_t* twirl = variant == 2 ? baseline_func_global_asm_806E208C : test_func_global_asm_806E208C;
        recomp_func_t* attack = variant == 2 ? baseline_func_global_asm_806E4740 : test_func_global_asm_806E4740;
        recomp_func_t* rocket = variant == 2 ? baseline_func_global_asm_80681E18 : test_func_global_asm_80681E18;
        recomp_func_t* barrel = variant == 2 ? baseline_func_global_asm_8067E784 : test_func_global_asm_8067E784;
        setup(mode, 3, bits); put16(0x807FD63C, 0x8000);
        unsigned expected = mode ? !!(bits & 2) : bits >= 2;
        invoke(twirl, 0, 0, 0); CHECK(actions == expected);
        if (expected) { CHECK(last_action == 9); CHECK(get32(EXTRA + 0x58) == 70); }
        actions = 0; put32(0x8076A0A8, 14); invoke(twirl, 0, 0, 0); CHECK(actions == 0);
        setup(mode, 1, bits); put16(0x807FD63C, 0x4000); invoke(attack, 0, 0, 0);
        CHECK((get8(ACTOR + 0x154) == 0x2e) == (mode ? !!(bits & 1) : bits > 0));
        setup(mode, 2, bits); put16(0x807FD63C, 0x4000); invoke(attack, 0, 0, 0);
        CHECK(actions == (mode ? !!(bits & 1) : bits > 0)); if (actions) CHECK(last_action == 0x25);
        setup(mode, 4, bits); put16(0x807FD63C, 0x4000); invoke(attack, 0, 0, 0);
        CHECK(actions == expected); if (actions) CHECK(last_action == 0x26);
        setup(mode, 1, bits); invoke(rocket, 0, 0, 0);
        CHECK(get16(ACTOR + 0x132) == (expected ? 1 : 2));
        const unsigned kongs[] = {0,2,3,4,4}, types[] = {0x39,0x38,0x28,0x19,0x60}, ranks[] = {2,3,1,1,1};
        for (unsigned i = 0; i < 5; ++i) {
            setup(mode, kongs[i], bits); put32(ACTOR + 0x58, types[i]); put8(ACTOR + 0x154, 6);
            invoke(barrel, 0, 0, 0);
            expected = mode ? !!(bits & (1u << (ranks[i]-1))) : bits >= ranks[i];
            CHECK(get16(ACTOR + 0x132) == (expected ? 1 : 2));
        }
    }
    // Suppression preserves location flags and the HUD; unrelated resources and spending still work.
    for (unsigned variant = 0; variant < 3; ++variant) {
        unsigned mode = variant == 1 ? 2 : 0;
        recomp_func_t* reward = variant == 2 ? baseline_changeCollectableCount : test_changeCollectableCount;
        setup(mode, 0, 0); collection_flag = 1;
        invoke(reward, 8, 0, 1);
        CHECK(get16(COUNTER) == (mode ? 10 : 11)); CHECK(flag_writes == 1 && hud_updates == 1);
        setup(mode, 0, 0); invoke(reward, 8, 0, (uint32_t)-1); CHECK(get16(COUNTER) == 9);
        setup(mode, 0, 0); invoke(reward, 3, 0, 5); CHECK(get16(COUNTER) == 15);
        setup(mode, 0, 0); put8(0x807467C8, 1); invoke(reward, 8, 0, 1); CHECK(get16(COUNTER) == 11);
        setup(mode, 0, 0); put8(HUD + 8 * 0x30 + 0x2c, 1); collection_flag = 1; invoke(reward, 8, 0, 1);
        CHECK(get16(COUNTER) == 10 && flag_writes == 0 && hud_updates == 0);
    }
    setup(2, 0, 0); put16(COUNTER, 1200); collection_flag = 1;
    invoke(test_changeCollectableCount, 8, 0, 1);
    CHECK(get16(COUNTER) == 1200 && flag_writes == 1 && hud_updates == 1);
    test_shops_and_saves();
    test_campaign_save();
    test_campaign_inventory();
    test_campaign_inventory_stage();
    test_campaign_items();
    func_global_asm_8060AC34 = noop;
    test_campaign_traps();
    test_campaign_transaction();
    test_campaign_checks();
    test_campaign_stage_wire();
    test_ap_shops();
    test_shop_text();
    test_shop_overlay();
    }
    free(memory);
    printf("PASS: executable MIPS gameplay/shop/save replacements and original fallbacks (%u assertions)\n", checks);
    return 0;
}
