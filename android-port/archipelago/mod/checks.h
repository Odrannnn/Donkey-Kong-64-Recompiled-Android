// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "../generated/catalog.h"

typedef struct { unsigned int words[AP_LOCATION_WORDS]; } ap_check_set;
static inline int ap_location_index(unsigned int id) {
    unsigned int low = 0, high = AP_LOCATION_COUNT;
    while (low < high) {
        unsigned int middle = low + (high - low) / 2;
        if (ap_locations[middle].id < id) low = middle + 1; else high = middle;
    }
    return low < AP_LOCATION_COUNT && ap_locations[low].id == id ? (int)low : -1;
}
static inline int ap_check_has_index(const ap_check_set* set, unsigned int index) {
    return index < AP_LOCATION_COUNT && (set->words[index / 32] & (1u << (index % 32))) != 0;
}
static inline int ap_check_add(ap_check_set* set, unsigned int id) {
    int index = ap_location_index(id);
    if (index < 0) return 0;
    set->words[(unsigned int)index / 32] |= 1u << ((unsigned int)index % 32);
    return 1;
}
static inline int ap_check_has(const ap_check_set* set, unsigned int id) {
    int index = ap_location_index(id);
    return index >= 0 && ap_check_has_index(set, (unsigned int)index);
}
// Observe stock flags only. Randomizer-added flags never touch stock save storage.
static inline void ap_check_stock_flag(ap_check_set* observed, const ap_check_set* active, unsigned short flag) {
    for (unsigned int i = 0; i < AP_LOCATION_COUNT; ++i) {
        if (ap_check_has_index(active, i) && ap_locations[i].detector == AP_DETECT_STOCK_FLAG && ap_locations[i].flag == flag)
            observed->words[i / 32] |= 1u << (i % 32);
    }
}
// New game hooks report the AP ID, not a guessed stock-memory offset.
static inline int ap_check_event(ap_check_set* observed, const ap_check_set* active, unsigned int id) {
    if (!ap_check_has(active, id)) return 0;
    return ap_check_add(observed, id);
}
static inline int ap_check_valid(const ap_check_set* set) {
    unsigned int remainder = AP_LOCATION_COUNT % 32;
    return !remainder || !(set->words[AP_LOCATION_WORDS - 1] >> remainder);
}
// Shared shop and Kong-specific shop checks can use the same upstream flag.
// A seed must choose only one; silently reporting both would send an unearned item.
static inline int ap_check_distinct_flags(const ap_check_set* active) {
    if (!ap_check_valid(active)) return 0;
    for (unsigned int i = 0; i < AP_LOCATION_COUNT; ++i) {
        if (!ap_check_has_index(active, i) || ap_locations[i].flag == 65535) continue;
        for (unsigned int j = i + 1; j < AP_LOCATION_COUNT; ++j)
            if (ap_check_has_index(active, j) && ap_locations[i].flag == ap_locations[j].flag) return 0;
    }
    // A shared shop occupies the entire vendor/level, including the four Kong
    // checks whose individual flags do not numerically alias the shared DK flag.
    for (unsigned int i = 0; i < AP_SHOP_COUNT; ++i) {
        const ap_shop_location_def* shared = &ap_shop_locations[i];
        if (shared->kong != AP_SHOP_SHARED || !ap_check_has(active, shared->id)) continue;
        for (unsigned int j = 0; j < AP_SHOP_COUNT; ++j) {
            const ap_shop_location_def* other = &ap_shop_locations[j];
            if (i != j && shared->vendor == other->vendor && shared->level == other->level && ap_check_has(active, other->id)) return 0;
        }
    }
    return 1;
}
