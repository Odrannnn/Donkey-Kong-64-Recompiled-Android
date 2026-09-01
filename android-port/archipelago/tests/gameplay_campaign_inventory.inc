// Compact campaign inventory reducer executed from the linked MIPS payload.
#include "../mod/campaign_inventory.h"

#define INV_BYTE(address, field) get8((address) + AP_OFFSETOF(ap_campaign_inventory, field))
#define INV_WORD(address, field) get32((address) + AP_OFFSETOF(ap_campaign_inventory, field))

static unsigned trap_total(uint32_t address) {
    unsigned total = 0;
    for (unsigned i = 0; i < AP_TRAP_TYPE_COUNT; ++i)
        total += get32(address + AP_OFFSETOF(ap_campaign_inventory, traps) + 4 * i);
    return total;
}

static void test_campaign_inventory(void) {
    const uint32_t inventory = PAYLOAD, rebuilt = PAYLOAD + 0x200, ids = PAYLOAD + 0x400;
    setup(0, 0, 0);
    memset(memory + inventory - 0x80000000u, 0xA5, sizeof(ap_campaign_inventory));
    invoke(test_ap_campaign_inventory_clear, inventory, 0, 0);
    for (unsigned i = 0; i < sizeof(ap_campaign_inventory); ++i) CHECK(get8(inventory + i) == 0);

    unsigned supported = 0, unsupported = 0;
    unsigned char before[sizeof(ap_campaign_inventory)];
    for (unsigned i = 0; i < AP_ITEM_COUNT; ++i) {
        memcpy(before, memory + inventory - 0x80000000u, sizeof(before));
        int expected = ap_item_effects[i].effects[0].kind != AP_EFFECT_UNSUPPORTED
            && ap_item_effects[i].effects[1].kind != AP_EFFECT_UNSUPPORTED;
        uint32_t result = invoke(test_ap_campaign_inventory_add, inventory, ap_item_ids[i], 0);
        CHECK(result == (unsigned)expected);
        if (expected) {
            put32(ids + 4 * supported++, ap_item_ids[i]);
        } else {
            ++unsupported;
            CHECK(!memcmp(before, memory + inventory - 0x80000000u, sizeof(before)));
        }
    }
    CHECK(supported == 165 && unsupported == 19);
    CHECK(INV_BYTE(inventory, kongs) == 31 && INV_BYTE(inventory, keys) == 255);
    CHECK(INV_BYTE(inventory, abilities) == 252 && INV_BYTE(inventory, special) == 7);
    CHECK(INV_BYTE(inventory, slam) == 3 && INV_BYTE(inventory, belt) == 2);
    CHECK(INV_BYTE(inventory, instrument_upgrades) == 3 && INV_BYTE(inventory, shopkeepers) == 15);
    CHECK(INV_BYTE(inventory, climbing) == 1);
    for (unsigned kong = 0; kong < 5; ++kong) {
        CHECK(get8(inventory + AP_OFFSETOF(ap_campaign_inventory, moves) + kong) == 7);
        CHECK(get8(inventory + AP_OFFSETOF(ap_campaign_inventory, guns) + kong) == 7);
        CHECK(get8(inventory + AP_OFFSETOF(ap_campaign_inventory, instruments) + kong) == 15);
        CHECK(get8(inventory + AP_OFFSETOF(ap_campaign_inventory, hints) + kong) == 0x7F);
        CHECK(get8(inventory + AP_OFFSETOF(ap_campaign_inventory, blueprints) + kong) == 1);
    }
    CHECK(INV_WORD(inventory, golden_bananas) == 1 && INV_WORD(inventory, fairies) == 1);
    CHECK(INV_WORD(inventory, crowns) == 1 && INV_WORD(inventory, medals) == 1);
    CHECK(INV_WORD(inventory, pearls) == 1 && INV_WORD(inventory, rainbow_coins) == 1);
    CHECK(INV_WORD(inventory, junk) == 6 && trap_total(inventory) == 30);
    CHECK(invoke(test_ap_campaign_inventory_has_flag, inventory, 671, 0) == 1);
    CHECK(invoke(test_ap_campaign_inventory_has_flag, inventory, 962, 0) == 1);
    CHECK(invoke(test_ap_campaign_inventory_has_flag, inventory, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_inventory_has_flag, inventory, AP_ITEM_FLAG_WORDS * 32u, 0) == 0);
    memcpy(before, memory + inventory - 0x80000000u, sizeof(before));
    CHECK(invoke(test_ap_campaign_inventory_add, inventory, 0xFFFFFFFFu, 0) == 0xFFFFFFFFu);
    CHECK(!memcmp(before, memory + inventory - 0x80000000u, sizeof(before)));

    memset(memory + rebuilt - 0x80000000u, 0x5A, sizeof(ap_campaign_inventory));
    CHECK(invoke(test_ap_campaign_inventory_rebuild, rebuilt, ids, supported) == 1);
    CHECK(!memcmp(memory + rebuilt - 0x80000000u, memory + inventory - 0x80000000u, sizeof(ap_campaign_inventory)));
    memcpy(before, memory + rebuilt - 0x80000000u, sizeof(before));
    put32(ids + 4 * supported, 14041090u);
    CHECK(invoke(test_ap_campaign_inventory_rebuild, rebuilt, ids, supported + 1) == 0);
    CHECK(!memcmp(before, memory + rebuilt - 0x80000000u, sizeof(before)));
    CHECK(invoke(test_ap_campaign_inventory_rebuild, rebuilt, 0, 1) == 0xFFFFFFFFu);
    CHECK(invoke(test_ap_campaign_inventory_rebuild, 0, 0, 0) == 0xFFFFFFFFu);
    CHECK(invoke(test_ap_campaign_inventory_rebuild, rebuilt, ids, 65536) == 0xFFFFFFFFu);

    invoke(test_ap_campaign_inventory_clear, rebuilt, 0, 0);
    for (unsigned i = 0; i < 10; ++i) {
        CHECK(invoke(test_ap_campaign_inventory_add, rebuilt, 14041101u, 0) == 1);
        CHECK(invoke(test_ap_campaign_inventory_add, rebuilt, 14041130u, 0) == 1);
        CHECK(invoke(test_ap_campaign_inventory_add, rebuilt, 14041137u, 0) == 1);
        CHECK(invoke(test_ap_campaign_inventory_add, rebuilt, 14041358u, 0) == 1);
    }
    CHECK(INV_BYTE(rebuilt, slam) == 3 && INV_BYTE(rebuilt, belt) == 2 && INV_BYTE(rebuilt, instrument_upgrades) == 3);
    CHECK(get8(rebuilt + AP_OFFSETOF(ap_campaign_inventory, blueprints)) == 8);
}

static void test_campaign_inventory_stage(void) {
    const uint32_t identity = PAYLOAD + 0x1000, other = PAYLOAD + 0x1080;
    const uint32_t snapshot = PAYLOAD + 0x1100, ids = PAYLOAD + 0x1200, expected = PAYLOAD + 0x1600;
    setup(0, 0, 0);
    memset(memory + TEST_CAMPAIGN_TRAILER - 0x80000000u, 0, sizeof(ap_campaign_save));
    unsigned count = 0;
    for (unsigned i = 0; i < AP_ITEM_COUNT; ++i)
        if (ap_item_effects[i].effects[0].kind != AP_EFFECT_UNSUPPORTED
            && ap_item_effects[i].effects[1].kind != AP_EFFECT_UNSUPPORTED)
            put32(ids + 4 * count++, ap_item_ids[i]);
    CHECK(count == 165);
    write_campaign_header(identity, (uint16_t)count, 21);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 21);
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, ids, count) == 1);
    memset(memory + snapshot - 0x80000000u, 0xA5, sizeof(ap_campaign_inventory));
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 1);
    CHECK(invoke(test_ap_campaign_inventory_rebuild, expected, ids, count) == 1);
    CHECK(!memcmp(memory + snapshot - 0x80000000u, memory + expected - 0x80000000u, sizeof(ap_campaign_inventory)));

    put32(TEST_MODE_ADDRESS, 8);
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, ids, count) == 0);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 1); // Existing publication remains valid.
    put32(TEST_MODE_ADDRESS, 0);
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, ids + 1, count) == 0); // Misaligned prefix.
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot + 1, identity, 0) == 0);

    write_campaign_header(other, (uint16_t)(count - 1), 21);
    CHECK(invoke(test_ap_campaign_inventory_stage, other, ids, count) == 0);
    write_campaign_header(other, (uint16_t)count, 22);
    CHECK(invoke(test_ap_campaign_inventory_stage, other, ids, count) == 0);
    write_campaign_header(other, (uint16_t)count, 21); put32(other + 28, get32(other + 28) ^ 1);
    CHECK(invoke(test_ap_campaign_inventory_stage, other, ids, count) == 0);

    put32(ids + 4 * count, 14041090u);
    write_campaign_header(other, (uint16_t)(count + 1), 21);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)(count + 1), 21);
    CHECK(invoke(test_ap_campaign_inventory_stage, other, ids, count + 1) == 0);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 21);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 1);

    unsigned char before[sizeof(ap_campaign_inventory)];
    memset(memory + snapshot - 0x80000000u, 0x5A, sizeof(ap_campaign_inventory));
    memcpy(before, memory + snapshot - 0x80000000u, sizeof(before));
    put8(TEST_CAMPAIGN_TRAILER + 9, get8(TEST_CAMPAIGN_TRAILER + 9) ^ 1);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 0);
    CHECK(!memcmp(before, memory + snapshot - 0x80000000u, sizeof(before)));
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 21);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 22); // Valid, but belongs to another seed.
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 0);
    CHECK(!memcmp(before, memory + snapshot - 0x80000000u, sizeof(before)));
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 21);
    put8(0x807467C8, 1);
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, ids, count) == 0);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 0);
    put8(0x807467C8, 0);

    invoke(test_ap_campaign_inventory_reset, 0, 0, 0);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 0);
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, ids, count) == 1);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 1);

    write_campaign_header(identity, 0, 30); write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 30);
    invoke(test_ap_campaign_inventory_reset, 0, 0, 0);
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, snapshot, identity, 0) == 1);
    for (unsigned i = 0; i < sizeof(ap_campaign_inventory); ++i) CHECK(get8(snapshot + i) == 0);
    memset(memory + TEST_CAMPAIGN_TRAILER - 0x80000000u, 0, sizeof(ap_campaign_save));
    CHECK(invoke(test_ap_campaign_inventory_stage, identity, 0, 0) == 0);
}

#undef INV_BYTE
#undef INV_WORD
