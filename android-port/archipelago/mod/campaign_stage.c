// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_stage.h"
#include "campaign_items.h"

extern unsigned char current_file;
extern volatile unsigned int ap_gameplay_mode;

int ap_campaign_stage_apply(const ap_campaign_stage_wire* wire) {
    ap_campaign_inventory candidate;
    if (!wire || ((ap_uint32_t)wire & 3u) || current_file != 0 || ap_gameplay_mode != 0
        || wire->magic != AP_CAMPAIGN_STAGE_MAGIC || wire->format != AP_CAMPAIGN_STAGE_FORMAT
        || wire->reserved != 0 || wire->transaction_reserved != 0 || wire->next_item_valid > 1
        || wire->item_count > AP_CAMPAIGN_RECEIPT_LIMIT
        || wire->identity.counter != wire->item_count) return 0;
    // Both publications repeat these checks. On the single game thread there
    // is no intervening writer, so neither commit can fail after this preflight.
    if (!ap_campaign_checks_validate(&wire->identity, &wire->selected, &wire->observed)
        || ap_campaign_inventory_rebuild(&candidate, wire->item_ids, wire->item_count) != 1) return 0;
    ap_campaign_transaction transaction;
    if (wire->next_item_valid
        && !ap_campaign_transaction_build(&transaction, &wire->identity, &candidate, wire->next_item_id)) return 0;
    if (!ap_campaign_checks_prepare(&wire->identity, &wire->selected, &wire->observed)) return 0;
    if (!ap_campaign_inventory_stage(&wire->identity, wire->item_ids, wire->item_count)) return 0;
    if (!ap_campaign_items_stage(&wire->identity)) return 0;
    return ap_campaign_transaction_stage(&wire->identity, &candidate, wire->next_item_id, wire->next_item_valid);
}
