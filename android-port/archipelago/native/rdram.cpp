// SPDX-License-Identifier: GPL-3.0-or-later
#include "rdram.hpp"
#include "../mod/campaign_stage.h"

namespace dkap {
void validate_rdram_span(uint8_t* rdram, uint32_t address, size_t size) {
    if (!rdram || address < 0x80000000u || uint64_t(address - 0x80000000u) + size > 0x20000000u)
        throw Failure(Error::config);
}
namespace {
void write_byte(uint8_t* rdram, uint32_t address, uint8_t value) {
    validate_rdram_span(rdram, address, 1);
    rdram[(address - 0x80000000u) ^ 3] = value;
}
void write_half(uint8_t* rdram, uint32_t address, uint16_t value) {
    write_byte(rdram, address, uint8_t(value >> 8));
    write_byte(rdram, address + 1, uint8_t(value));
}
void write_word(uint8_t* rdram, uint32_t address, uint32_t value) {
    write_byte(rdram, address, uint8_t(value >> 24));
    write_byte(rdram, address + 1, uint8_t(value >> 16));
    write_byte(rdram, address + 2, uint8_t(value >> 8));
    write_byte(rdram, address + 3, uint8_t(value));
}
}
void write_rdram_shop_label(uint8_t* rdram, uint32_t address, const ap_shop_label& label) {
    validate_rdram_span(rdram, address, sizeof(ap_shop_label));
    write_word(rdram, address, label.location);
    write_half(rdram, address + 4, label.frames);
    write_half(rdram, address + 6, label.reserved);
    for (uint32_t i = 0; i < 33; ++i) write_byte(rdram, address + 8 + i, uint8_t(label.item[i]));
    for (uint32_t i = 0; i < 33; ++i) write_byte(rdram, address + 41 + i, uint8_t(label.subtitle[i]));
    write_byte(rdram, address + 74, label.padding[0]);
    write_byte(rdram, address + 75, label.padding[1]);
}
ap_campaign_save read_rdram_campaign_save(uint8_t* rdram, uint32_t address) {
    if (address & 3u) throw Failure(Error::config);
    validate_rdram_span(rdram, address, sizeof(ap_campaign_save));
    auto read_byte = [&](uint32_t at) { return rdram[(at - 0x80000000u) ^ 3]; };
    auto read_half = [&](uint32_t at) { return uint16_t(read_byte(at) << 8 | read_byte(at + 1)); };
    auto read_word = [&](uint32_t at) {
        return uint32_t(read_byte(at)) << 24 | uint32_t(read_byte(at + 1)) << 16
            | uint32_t(read_byte(at + 2)) << 8 | read_byte(at + 3);
    };
    ap_campaign_save result{};
    result.magic = read_word(address); result.format = read_half(address + 4); result.counter = read_half(address + 6);
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) result.binding[i] = read_byte(address + 8 + i);
    result.reserved = read_word(address + 24); result.checksum = read_word(address + 28);
    return result;
}
ap_check_set read_rdram_check_set(uint8_t* rdram, uint32_t address) {
    if (address & 3u) throw Failure(Error::config);
    validate_rdram_span(rdram, address, sizeof(ap_check_set));
    auto read_byte = [&](uint32_t at) { return rdram[(at - 0x80000000u) ^ 3]; };
    auto read_word = [&](uint32_t at) {
        return uint32_t(read_byte(at)) << 24 | uint32_t(read_byte(at + 1)) << 16
            | uint32_t(read_byte(at + 2)) << 8 | read_byte(at + 3);
    };
    ap_check_set result{};
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i) result.words[i] = read_word(address + 4 * i);
    if (!ap_check_valid(&result)) throw Failure(Error::config);
    return result;
}
void write_rdram_campaign_save(uint8_t* rdram, uint32_t address, const ap_campaign_save& save) {
    if (address & 3u) throw Failure(Error::config);
    validate_rdram_span(rdram, address, sizeof(save));
    write_word(rdram, address, save.magic);
    write_half(rdram, address + 4, save.format);
    write_half(rdram, address + 6, save.counter);
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) write_byte(rdram, address + 8 + i, save.binding[i]);
    write_word(rdram, address + 24, save.reserved);
    write_word(rdram, address + 28, save.checksum);
}
void write_rdram_campaign_stage(uint8_t* rdram, uint32_t address, const CampaignStageSnapshot& snapshot) {
    if ((address & 3u) || snapshot.item_ids.size() > AP_CAMPAIGN_RECEIPT_LIMIT
        || snapshot.identity.counter != snapshot.item_ids.size()
        || !ap_campaign_save_valid(&snapshot.identity)
        || snapshot.identity.magic != AP_CAMPAIGN_SAVE_MAGIC || snapshot.identity.format != AP_CAMPAIGN_SAVE_FORMAT
        || !ap_check_valid(&snapshot.selected) || !ap_check_distinct_flags(&snapshot.selected)
        || !ap_check_valid(&snapshot.observed)) throw Failure(Error::config);
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i)
        if (snapshot.observed.words[i] & ~snapshot.selected.words[i]) throw Failure(Error::config);
    const size_t bytes = AP_OFFSETOF(ap_campaign_stage_wire, item_ids) + snapshot.item_ids.size() * sizeof(uint32_t);
    validate_rdram_span(rdram, address, bytes); // No output is written before the complete span validates.
    write_word(rdram, address, AP_CAMPAIGN_STAGE_MAGIC);
    write_half(rdram, address + 4, AP_CAMPAIGN_STAGE_FORMAT);
    write_half(rdram, address + 6, 0);
    const uint32_t identity = address + AP_OFFSETOF(ap_campaign_stage_wire, identity);
    write_word(rdram, identity, snapshot.identity.magic);
    write_half(rdram, identity + 4, snapshot.identity.format);
    write_half(rdram, identity + 6, snapshot.identity.counter);
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) write_byte(rdram, identity + 8 + i, snapshot.identity.binding[i]);
    write_word(rdram, identity + 24, snapshot.identity.reserved);
    write_word(rdram, identity + 28, snapshot.identity.checksum);
    const uint32_t selected = address + AP_OFFSETOF(ap_campaign_stage_wire, selected);
    const uint32_t observed = address + AP_OFFSETOF(ap_campaign_stage_wire, observed);
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i) {
        write_word(rdram, selected + 4 * i, snapshot.selected.words[i]);
        write_word(rdram, observed + 4 * i, snapshot.observed.words[i]);
    }
    write_word(rdram, address + AP_OFFSETOF(ap_campaign_stage_wire, item_count), uint32_t(snapshot.item_ids.size()));
    write_word(rdram, address + AP_OFFSETOF(ap_campaign_stage_wire, next_item_id), snapshot.next_item.value_or(0));
    write_half(rdram, address + AP_OFFSETOF(ap_campaign_stage_wire, next_item_valid), snapshot.next_item ? 1 : 0);
    write_half(rdram, address + AP_OFFSETOF(ap_campaign_stage_wire, transaction_reserved), 0);
    const uint32_t items = address + AP_OFFSETOF(ap_campaign_stage_wire, item_ids);
    for (size_t i = 0; i < snapshot.item_ids.size(); ++i) write_word(rdram, items + uint32_t(4 * i), snapshot.item_ids[i]);
}
}
