// Combined native-to-game campaign staging wire executed from the linked MIPS payload.
#include "../mod/campaign_stage.h"

static void test_campaign_stage_wire(void) {
    const uint32_t wire = PAYLOAD + 0x3000;
    const uint32_t identity = wire + AP_OFFSETOF(ap_campaign_stage_wire, identity);
    const uint32_t selected = wire + AP_OFFSETOF(ap_campaign_stage_wire, selected);
    const uint32_t observed = wire + AP_OFFSETOF(ap_campaign_stage_wire, observed);
    const uint32_t ids = wire + AP_OFFSETOF(ap_campaign_stage_wire, item_ids);
    const uint32_t inventory = PAYLOAD + 0x3800, checks_out = PAYLOAD + 0x3A00;
    setup(0, 0, 0);
    invoke(test_ap_campaign_inventory_reset, 0, 0, 0);
    invoke(test_ap_campaign_checks_reset, 0, 0, 0);
    unsigned count = 0;
    for (unsigned i = 0; i < AP_ITEM_COUNT; ++i)
        if (ap_item_effects[i].effects[0].kind != AP_EFFECT_UNSUPPORTED
            && ap_item_effects[i].effects[1].kind != AP_EFFECT_UNSUPPORTED)
            put32(ids + 4 * count++, ap_item_ids[i]);
    CHECK(count == 165);
    put32(wire, AP_CAMPAIGN_STAGE_MAGIC); put16(wire + 4, AP_CAMPAIGN_STAGE_FORMAT); put16(wire + 6, 0);
    write_campaign_header(identity, (uint16_t)count, 51);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 51);
    memset(memory + selected - 0x80000000u, 0, sizeof(ap_check_set));
    memset(memory + observed - 0x80000000u, 0, sizeof(ap_check_set));
    put32(selected, 3); put32(observed, 1);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, item_count), count);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_id), 14041112u);
    put16(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_valid), 1);
    put16(wire + AP_OFFSETOF(ap_campaign_stage_wire, transaction_reserved), 0);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, inventory, identity, 0) == 1);
    CHECK(invoke(test_ap_campaign_checks_snapshot, checks_out, 0, 0) == 1);
    CHECK(get32(checks_out) == 1);
    CHECK(invoke(test_ap_campaign_transaction_snapshot, inventory + 0x400, identity, 0) == 1);
    CHECK(get32(inventory + 0x400 + AP_OFFSETOF(ap_campaign_transaction, item_id)) == 14041112u);
    unsigned char before[sizeof(ap_campaign_inventory)];
    memcpy(before, memory + inventory - 0x80000000u, sizeof(before));

    // Every malformed input fails before either successful publication changes.
    uint32_t first = get32(ids); put32(ids, 14041090u);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, inventory, identity, 0) == 1);
    CHECK(!memcmp(before, memory + inventory - 0x80000000u, sizeof(before)));
    CHECK(invoke(test_ap_campaign_checks_snapshot, checks_out, 0, 0) == 1 && get32(checks_out) == 1);
    put32(ids, first);
    put32(selected + 4u * (AP_LOCATION_WORDS - 1), 0x80000000u);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    put32(selected + 4u * (AP_LOCATION_WORDS - 1), 0);
    put32(observed, 4); CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0); put32(observed, 1);
    put32(wire, 0); CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0); put32(wire, AP_CAMPAIGN_STAGE_MAGIC);
    put16(wire + 6, 1); CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0); put16(wire + 6, 0);
    put16(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_valid), 2);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    put16(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_valid), 1);
    put16(wire + AP_OFFSETOF(ap_campaign_stage_wire, transaction_reserved), 1);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    put16(wire + AP_OFFSETOF(ap_campaign_stage_wire, transaction_reserved), 0);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_id), 14041090u);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_id), 14041112u);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, item_count), count - 1);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, item_count), AP_CAMPAIGN_RECEIPT_LIMIT + 1);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, item_count), count);
    CHECK(invoke(test_ap_campaign_stage_apply, wire + 1, 0, 0) == 0);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_CHECKS);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    put32(TEST_MODE_ADDRESS, 0);
    put8(0x807467C8, 1); CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0); put8(0x807467C8, 0);

    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 52);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, inventory, identity, 0) == 0);
    CHECK(invoke(test_ap_campaign_checks_snapshot, checks_out, 0, 0) == 0);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, (uint16_t)count, 51);
    CHECK(invoke(test_ap_campaign_inventory_snapshot, inventory, identity, 0) == 1);
    CHECK(invoke(test_ap_campaign_checks_snapshot, checks_out, 0, 0) == 1 && get32(checks_out) == 1);

    // The same prefix with no next receipt publishes an explicit complete state.
    put16(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_valid), 0);
    put32(wire + AP_OFFSETOF(ap_campaign_stage_wire, next_item_id), 0);
    CHECK(invoke(test_ap_campaign_stage_apply, wire, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_transaction_snapshot, inventory + 0x400, identity, 0) == 2);
}
