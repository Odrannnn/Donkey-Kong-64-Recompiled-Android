// Full campaign check ledger executed from the linked MIPS payload.
#include "../mod/campaign_checks.h"

static void clear_check_set(uint32_t address) {
    memset(memory + address - 0x80000000u, 0, sizeof(ap_check_set));
}

static void set_check_index(uint32_t address, unsigned index) {
    put32(address + 4u * (index / 32u), get32(address + 4u * (index / 32u)) | (1u << (index % 32u)));
}

static void test_campaign_checks(void) {
    const uint32_t selected = PAYLOAD + 0x2000, restored = PAYLOAD + 0x2100, snapshot = PAYLOAD + 0x2200;
    const uint32_t identity = PAYLOAD + 0x2300;
    setup(0, 0, 0);
    write_campaign_header(identity, 0, 41);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 41);
    CHECK(invoke(test_ap_campaign_checks_reset, 0, 0, 0) == 1);

    // Every generated location can be selected and delivered through its exact
    // AP ID. Only the 291 stock candidates react to stock-flag observation.
    unsigned stock_count = 0;
    for (unsigned i = 0; i < AP_LOCATION_COUNT; ++i) {
        clear_check_set(selected); clear_check_set(restored); clear_check_set(snapshot);
        set_check_index(selected, i);
        put32(TEST_MODE_ADDRESS, 0);
        CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored) == 1);
        put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_CHECKS);
        uint32_t stock = invoke(test_ap_campaign_checks_stock_flag, ap_locations[i].flag, 0, 0);
        CHECK(stock == (ap_locations[i].detector == AP_DETECT_STOCK_FLAG));
        if (stock) ++stock_count;
        uint32_t event = invoke(test_ap_campaign_checks_event, ap_locations[i].id, 0, 0);
        CHECK(event == (stock ? 0u : 1u));
        CHECK(invoke(test_ap_campaign_checks_event, ap_locations[i].id, 0, 0) == 0);
        CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot, 0, 0) == 1);
        CHECK(get32(snapshot + 4u * (i / 32u)) == (1u << (i % 32u)));
        CHECK(invoke(test_ap_campaign_checks_event, 0xFFFFFFFFu, 0, 0) == 0xFFFFFFFFu);
    }
    CHECK(stock_count == 291);

    put32(TEST_MODE_ADDRESS, 0);
    clear_check_set(selected); clear_check_set(restored);
    set_check_index(selected, 0); set_check_index(selected, 1);
    set_check_index(restored, 0);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored) == 1);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_CHECKS);
    CHECK(invoke(test_ap_campaign_checks_event, ap_locations[1].id, 0, 0) == 1);
    clear_check_set(restored); // A stale reconnect snapshot cannot remove observations.
    CHECK(invoke(test_ap_campaign_checks_restore, restored, 0, 0) == 1);
    clear_check_set(snapshot);
    CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot, 0, 0) == 1);
    CHECK(get32(snapshot) == 3);

    // Restoration is validated completely before the current ledger changes.
    clear_check_set(restored); set_check_index(restored, 2);
    CHECK(invoke(test_ap_campaign_checks_restore, restored, 0, 0) == 0);
    clear_check_set(snapshot); CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot, 0, 0) == 1);
    CHECK(get32(snapshot) == 3);
    CHECK(invoke(test_ap_campaign_checks_restore, restored + 1, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot + 1, 0, 0) == 0);

    // Invalid tail bits, ordinary flag aliases, and shared/individual shop
    // conflicts all reject an entire replacement snapshot.
    put32(TEST_MODE_ADDRESS, 0);
    clear_check_set(selected); clear_check_set(restored);
    put32(selected + 4u * (AP_LOCATION_WORDS - 1), 0x80000000u);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored) == 0);
    unsigned alias_a = AP_LOCATION_COUNT, alias_b = AP_LOCATION_COUNT;
    for (unsigned i = 0; i < AP_LOCATION_COUNT && alias_a == AP_LOCATION_COUNT; ++i)
        if (ap_locations[i].flag != 65535)
            for (unsigned j = i + 1; j < AP_LOCATION_COUNT; ++j)
                if (ap_locations[i].flag == ap_locations[j].flag) { alias_a = i; alias_b = j; break; }
    CHECK(alias_a < AP_LOCATION_COUNT);
    clear_check_set(selected); set_check_index(selected, alias_a); set_check_index(selected, alias_b);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored) == 0);
    unsigned shared = AP_SHOP_COUNT, individual = AP_SHOP_COUNT;
    for (unsigned i = 0; i < AP_SHOP_COUNT && shared == AP_SHOP_COUNT; ++i) if (ap_shop_locations[i].kong == AP_SHOP_SHARED)
        for (unsigned j = 0; j < AP_SHOP_COUNT; ++j)
            if (ap_shop_locations[j].kong != AP_SHOP_SHARED
                && ap_shop_locations[i].vendor == ap_shop_locations[j].vendor
                && ap_shop_locations[i].level == ap_shop_locations[j].level) { shared = i; individual = j; break; }
    CHECK(shared < AP_SHOP_COUNT);
    clear_check_set(selected);
    set_check_index(selected, (unsigned)ap_location_index(ap_shop_locations[shared].id));
    set_check_index(selected, (unsigned)ap_location_index(ap_shop_locations[individual].id));
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored) == 0);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_CHECKS);
    clear_check_set(snapshot);
    CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot, 0, 0) == 1);
    CHECK(get32(snapshot) == 3); // Failed replacements left the old publication intact.
    put32(TEST_MODE_ADDRESS, 0);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity + 1, selected, restored) == 0);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected + 1, restored) == 0);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored + 1) == 0);

    // Context and capability failures cannot expose or erase a prepared ledger.
    clear_check_set(selected); set_check_index(selected, 0);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored) == 1);
    CHECK(invoke(test_ap_campaign_checks_event, ap_locations[0].id, 0, 0) == 0xFFFFFFFFu);
    put32(TEST_MODE_ADDRESS, AP_GAMEPLAY_CHECKS);
    CHECK(invoke(test_ap_campaign_checks_prepare, identity, selected, restored) == 0);
    CHECK(invoke(test_ap_campaign_checks_reset, 0, 0, 0) == 0);
    CHECK(invoke(test_ap_campaign_checks_event, ap_locations[0].id, 0, 0) == 1);
    write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 42);
    CHECK(invoke(test_ap_campaign_checks_event, ap_locations[0].id, 0, 0) == 0xFFFFFFFFu);
    memset(memory + snapshot - 0x80000000u, 0xA5, sizeof(ap_check_set));
    unsigned char foreign_before[sizeof(ap_check_set)];
    memcpy(foreign_before, memory + snapshot - 0x80000000u, sizeof(foreign_before));
    CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot, 0, 0) == 0);
    CHECK(!memcmp(foreign_before, memory + snapshot - 0x80000000u, sizeof(foreign_before)));
    write_campaign_header(TEST_CAMPAIGN_TRAILER, 0, 41);
    put8(0x807467C8, 1);
    memset(memory + snapshot - 0x80000000u, 0xA5, sizeof(ap_check_set));
    unsigned char before[sizeof(ap_check_set)];
    memcpy(before, memory + snapshot - 0x80000000u, sizeof(before));
    CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot, 0, 0) == 0);
    CHECK(!memcmp(before, memory + snapshot - 0x80000000u, sizeof(before)));
    CHECK(invoke(test_ap_campaign_checks_event, ap_locations[0].id, 0, 0) == 0xFFFFFFFFu);
    put8(0x807467C8, 0); put32(TEST_MODE_ADDRESS, 0);
    CHECK(invoke(test_ap_campaign_checks_reset, 0, 0, 0) == 1);
    CHECK(invoke(test_ap_campaign_checks_snapshot, snapshot, 0, 0) == 0);
}
