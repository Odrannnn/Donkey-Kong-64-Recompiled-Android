// Campaign trailer helpers execute from the linked MIPS payload.
#include "../mod/campaign_receipts.h"

static void write_campaign_header(uint32_t address, uint16_t counter, unsigned binding_salt) {
    ap_campaign_save save = {0};
    save.magic = AP_CAMPAIGN_SAVE_MAGIC; save.format = AP_CAMPAIGN_SAVE_FORMAT; save.counter = counter;
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) save.binding[i] = (uint8_t)(binding_salt + i * 7);
    ap_campaign_save_seal(&save);
    put32(address, save.magic); put16(address + 4, save.format); put16(address + 6, save.counter);
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) put8(address + 8 + i, save.binding[i]);
    put32(address + 24, save.reserved); put32(address + 28, save.checksum);
}
static void check_campaign_header(uint32_t address, uint16_t counter, unsigned binding_salt) {
    ap_campaign_save save = {0};
    save.magic = get32(address); save.format = get16(address + 4); save.counter = get16(address + 6);
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) save.binding[i] = get8(address + 8 + i);
    save.reserved = get32(address + 24); save.checksum = get32(address + 28);
    CHECK(save.counter == counter && ap_campaign_save_valid(&save));
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) CHECK(save.binding[i] == (uint8_t)(binding_salt + i * 7));
}
static void copy_format_string(uint8_t* rdram, recomp_context* ctx) {
    uint32_t i = 0; do { MEM_B(i, ctx->r4) = MEM_B(i, ctx->r5); } while (MEM_B(i++, ctx->r5));
    ctx->r2 = ctx->r4;
}
static void setup_campaign_writer(unsigned mode, unsigned dynamic_bits) {
    setup(mode, 0, 0);
    memset(memory + 0x7ECEA8, 0xA5, 0x1000);
    put16(0x807ECEA0, dynamic_bits); put32(0x807EDEAC, 2); put8(0x80744460, 1); put8(0x807ECD08, 0);
    put32(0x807467C0, PAYLOAD + 0x200); put8(PAYLOAD + 0x200, '7'); put8(PAYLOAD + 0x201, '.');
    put8(PAYLOAD + 0x202, '2'); put8(PAYLOAD + 0x203, 0);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, 23, 9);
    *rom_memcpy = copy_memory; _sprintf = copy_format_string;
    func_global_asm_8060BA14 = noop; func_global_asm_8060B968 = zero;
    strlen = strlen_recomp; bzero = bzero_recomp; osWritebackDCacheAll = osWritebackDCacheAll_recomp;
    func_global_asm_8060B84C = noop; osEepromWrite = osEepromWrite_recomp;
}
static void test_campaign_writer(void) {
    unsigned char expected[0x800];
    for (unsigned dynamic_bits = 0; dynamic_bits <= 608; dynamic_bits += 608) {
        setup_campaign_writer(0, dynamic_bits); invoke(baseline_func_global_asm_8060D2C8, 0, 0, 0);
        memcpy(expected, memory + 0x7ED6A8, sizeof(expected));
        setup_campaign_writer(0, dynamic_bits); invoke(test_func_global_asm_8060D2C8, 0, 0, 0);
        CHECK(!memcmp(expected, memory + 0x7ED6A8, sizeof(expected)));
        for (unsigned i = 0; i < sizeof(ap_campaign_save); ++i) CHECK(get8(0x807EDE88 + i) == 0);

        setup_campaign_writer(8, dynamic_bits); invoke(test_func_global_asm_8060D2C8, 0, 0, 0);
        CHECK(get8(0x807EDE87) == 0); // Last byte before the reserved trailer is still cleared.
        check_campaign_header(0x807EDE88, 23, 9);
    }
}
static void test_campaign_save(void) {
    const uint32_t candidate = PAYLOAD, output = PAYLOAD + 0x80;
    setup(0, 0, 0); memset(memory + TEST_CAMPAIGN_TRAILER - 0x80000000u, 0, sizeof(ap_campaign_save));
    CHECK(invoke(test_ap_campaign_trailer_read, output, 0, 0) == 0);
    write_campaign_header(candidate, 0, 3);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 1);
    check_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 3);
    CHECK(invoke(test_ap_campaign_trailer_read, output, 0, 0) == 1);
    check_campaign_header(output, 0, 3);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 1); // Idempotent.

    write_campaign_header(candidate, 2, 3); put32(TEST_MODE_ADDRESS, 8);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0); // Cannot skip.
    write_campaign_header(candidate, 1, 4);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0); // Cannot change seed.
    write_campaign_header(candidate, 1, 3); put32(TEST_MODE_ADDRESS, 0);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0); // Advance needs active save capability.
    put32(TEST_MODE_ADDRESS, 8);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 1);
    check_campaign_header(TEST_CAMPAIGN_TRAILER, 1, 3);
    write_campaign_header(candidate, 0, 3);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0); // No rollback.

    write_campaign_header(candidate, 2, 3); put32(candidate + 28, get32(candidate + 28) ^ 1);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0);
    put8(0x807467C8, 1); write_campaign_header(candidate, 2, 3);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0);
    put8(0x807467C8, 0);

    write_campaign_header(TEST_CAMPAIGN_TRAILER, 0xFFFF, 3);
    write_campaign_header(candidate, 0, 3);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0);
    write_campaign_header(candidate, 0xFFFF, 3);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 1);
    put8(TEST_CAMPAIGN_TRAILER + 10, get8(TEST_CAMPAIGN_TRAILER + 10) ^ 0x80);
    CHECK(invoke(test_ap_campaign_trailer_read, output, 0, 0) == 0xFFFFFFFFu);
    CHECK(invoke(test_ap_campaign_trailer_stage, candidate, 0, 0) == 0); // Corrupt state cannot be overwritten.
    test_campaign_writer();
}
