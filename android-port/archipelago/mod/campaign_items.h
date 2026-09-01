// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_inventory.h"
#include "gameplay_mode.h"

// Receipt-derived equivalent of the original randomizer's CountStruct query
// surface. Queries return -1 while the publication is unavailable/stale or
// when their selectors are invalid; otherwise they return a nonnegative count.
enum {
    AP_ITEM_QUERY_KONG,
    AP_ITEM_QUERY_KEY,
    AP_ITEM_QUERY_HINT,
    AP_ITEM_QUERY_BLUEPRINT,
    AP_ITEM_QUERY_FAIRY,
    AP_ITEM_QUERY_CROWN,
    AP_ITEM_QUERY_COMPANY_COIN,
    AP_ITEM_QUERY_MEDAL,
    AP_ITEM_QUERY_BEAN,
    AP_ITEM_QUERY_PEARL,
    AP_ITEM_QUERY_RAINBOW_COIN,
    AP_ITEM_QUERY_TRAP,
    AP_ITEM_QUERY_JUNK,
    AP_ITEM_QUERY_MOVE,
    AP_ITEM_QUERY_GOLDEN_BANANA,
    AP_ITEM_QUERY_SHOPKEEPER,
};

#if defined(__cplusplus)
extern "C" {
#endif
// Stage only while every gameplay capability is disabled. The source must be
// the exact inventory publication bound to the current durable trailer.
int ap_campaign_items_stage(const ap_campaign_save* identity);
int ap_campaign_item_count(unsigned item, int level, int kong);
int ap_campaign_item_has_flag(unsigned flag);
// Adapters for the original randomizer's requirement_item ABI and Kong flag
// helper. Unlike the diagnostic query above, unavailable state returns zero so
// patched gameplay checks fail closed.
int ap_campaign_original_item_count(int item, int level, int kong);
int ap_campaign_get_kong_ownership_from_flag(int flag);
// Exact counterpart to the original randomizer's hasFlagMove. When the item
// capability is off it preserves the stock permanent-flag result; when it is
// on, stale or unavailable receipt state fails closed.
int ap_campaign_has_flag_move(int flag);
void ap_campaign_items_reset(void);
#if defined(__cplusplus)
}
#endif
