// Original ice-trap safety and state effects executed from the linked MIPS payload.
#include "../mod/campaign_traps.h"

static int expected_trap_executable(unsigned type) {
    return type == AP_TRAP_TYPE_DISABLE_A || type == AP_TRAP_TYPE_DISABLE_B
        || type == AP_TRAP_TYPE_DISABLE_Z || type == AP_TRAP_TYPE_DISABLE_C_UP
        || type == AP_TRAP_TYPE_DRY || type == AP_TRAP_TYPE_ICEFLOOR;
}

static void trap_make_safe(void) {
    put32(0x8076A064, 5);                 // ObjectModel2Timer
    put32(0x807FD888, 0);                 // LZFadeoutProgress
    put8(0x807463B8, 0);                  // IsAutowalking
    put32(PLAYER + 0x13C, 0);             // collision queue
    put8(PLAYER + 0x154, 0);              // allowed movement state
    put16(PLAYER + 0x248, 0xFFFF);         // shockwave timer == -1
    put32(PLAYER + 0x370, 0);             // seasick bit clear
    put32(0x8076A0A8, 0);                 // allowed map
    put16(0x80755308, 0xFFFF);             // stock enabled buttons
    put8(0x8075530C, 0xFF); put8(0x80755310, 0xFF);
    invoke(test_ap_campaign_trap_reset, 0, 0, 0);
}

static void trap_safe_setup(void) {
    setup(AP_GAMEPLAY_TRAPS, 0, 0);
    trap_make_safe();
}

static void test_campaign_traps(void) {
    unsigned executable = 0;
    for (unsigned type = 0; type < AP_TRAP_TYPE_COUNT; ++type)
        executable += invoke(test_ap_campaign_trap_executable, type, 0, 0) != 0;
    CHECK(executable == 6);

    const unsigned types[] = {AP_TRAP_TYPE_DISABLE_A, AP_TRAP_TYPE_DISABLE_B,
        AP_TRAP_TYPE_DISABLE_Z, AP_TRAP_TYPE_DISABLE_C_UP, AP_TRAP_TYPE_DRY, AP_TRAP_TYPE_ICEFLOOR};
    for (unsigned i = 0; i < sizeof(types) / sizeof(*types); ++i) {
        trap_safe_setup();
        CHECK(invoke(test_ap_campaign_trap_can_start, types[i], 0, 0) == 1);
    }

    // Every common upstream safety condition blocks state mutation.
    trap_safe_setup(); put32(0x807FBB4C, 0); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(PLAYER + 0x13C, EXTRA); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(0x8076A064, 4); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(0x807FD888, 0x41800000); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(PLAYER + 0x370, 0x100); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put8(0x807463B8, 1); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put16(PLAYER + 0x248, 0); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(0x8076A0A8, 1); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(0x8076A0A8, 3); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(0x8076A0A8, AP_TRAP_MAP_COUNT); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put8(PLAYER + 0x154, 4); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put8(PLAYER + 0x154, AP_TRAP_MOVEMENT_COUNT); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);
    trap_safe_setup(); put32(TEST_MODE_ADDRESS, 0); CHECK(invoke(test_ap_campaign_trap_can_start, types[0], 0, 0) == 0);

    // Concurrent button traps filter all four pads after the untouched stock filter.
    trap_safe_setup();
    CHECK(invoke(test_ap_campaign_trap_start, AP_TRAP_TYPE_DISABLE_A, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_trap_start, AP_TRAP_TYPE_DISABLE_B, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_trap_button_mask, 0, 0, 0) == 0x3FFFu);
    for (unsigned pad = 0; pad < 4; ++pad) {
        put16(PAYLOAD + 6 * pad, 0xFFFF); put8(PAYLOAD + 6 * pad + 2, 0x7F); put8(PAYLOAD + 6 * pad + 3, 0x7F);
    }
    invoke(test_func_global_asm_80712944, PAYLOAD, 0, 0);
    for (unsigned pad = 0; pad < 4; ++pad) {
        CHECK(get16(PAYLOAD + 6 * pad) == 0x3FFF);
        CHECK(get8(PAYLOAD + 6 * pad + 2) == 0x7F && get8(PAYLOAD + 6 * pad + 3) == 0x7F);
    }
    for (unsigned frame = 0; frame < 239; ++frame) invoke(test_ap_campaign_trap_tick, 0, 0, 0);
    CHECK(invoke(test_ap_campaign_trap_button_mask, 0, 0, 0) == 0x3FFFu);
    invoke(test_ap_campaign_trap_tick, 0, 0, 0);
    CHECK(invoke(test_ap_campaign_trap_button_mask, 0, 0, 0) == 0xFFFFu);

    // Turning the capability off restores controls immediately and clears stale timers.
    trap_safe_setup(); CHECK(invoke(test_ap_campaign_trap_start, AP_TRAP_TYPE_DISABLE_Z, 0, 0) == 1);
    put32(TEST_MODE_ADDRESS, 0); invoke(test_ap_campaign_trap_tick, 0, 0, 0);
    CHECK(invoke(test_ap_campaign_trap_button_mask, 0, 0, 0) == 0xFFFFu);

    // Dry wipes the same six live resources and each Kong's saved instrument energy.
    trap_safe_setup();
    for (unsigned offset = 0; offset < 0x10; offset += 2) put16(0x807FCC40 + offset, 0x1234 + offset);
    put8(0x807FCC4A, 0xA1); put8(0x807FCC4B, 0xA2); put8(0x807FCC4C, 0xA3); put8(0x807FCC4D, 0xA4);
    for (unsigned kong = 0; kong < 5; ++kong) put16(0x807FC950 + 0x5E * kong + 8, 40 + kong);
    CHECK(invoke(test_ap_campaign_trap_start, AP_TRAP_TYPE_DRY, 0, 0) == 1);
    CHECK(get16(0x807FCC40) == 0 && get16(0x807FCC42) == 0 && get16(0x807FCC44) == 0);
    CHECK(get16(0x807FCC46) == 0 && get16(0x807FCC48) == 0 && get16(0x807FCC4E) == 0);
    CHECK(get8(0x807FCC4A) == 0xA1 && get8(0x807FCC4B) == 0xA2);
    CHECK(get8(0x807FCC4C) == 0xA3 && get8(0x807FCC4D) == 0xA4);
    for (unsigned kong = 0; kong < 5; ++kong) CHECK(get16(0x807FC950 + 0x5E * kong + 8) == 0);

    // Ice Floor uses the original traction values and fifteen-second lifetime.
    trap_safe_setup(); put16(PLAYER + 0x1A6, 100);
    CHECK(invoke(test_ap_campaign_trap_start, AP_TRAP_TYPE_ICEFLOOR, 0, 0) == 1);
    CHECK(get16(PLAYER + 0x1A6) == 1);
    for (unsigned frame = 0; frame < 449; ++frame) invoke(test_ap_campaign_trap_tick, 0, 0, 0);
    CHECK(get16(PLAYER + 0x1A6) == 1);
    invoke(test_ap_campaign_trap_tick, 0, 0, 0); CHECK(get16(PLAYER + 0x1A6) == 100);
    trap_safe_setup(); put32(0x8076A0A8, 82); put16(PLAYER + 0x1A6, 100);
    CHECK(invoke(test_ap_campaign_trap_start, AP_TRAP_TYPE_ICEFLOOR, 0, 0) == 1);
    for (unsigned frame = 0; frame < 450; ++frame) invoke(test_ap_campaign_trap_tick, 0, 0, 0);
    CHECK(get16(PLAYER + 0x1A6) == 20);
    trap_safe_setup(); put16(PLAYER + 0x1A6, 100);
    CHECK(invoke(test_ap_campaign_trap_start, AP_TRAP_TYPE_ICEFLOOR, 0, 0) == 1);
    put32(TEST_MODE_ADDRESS, 0); invoke(test_ap_campaign_trap_tick, 0, 0, 0);
    CHECK(get16(PLAYER + 0x1A6) == 100);
}
