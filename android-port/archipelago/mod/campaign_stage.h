// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_inventory.h"
#include "campaign_checks.h"
#include "campaign_transaction.h"

#define AP_CAMPAIGN_STAGE_MAGIC 0x41505331u /* "APS1" */
#define AP_CAMPAIGN_STAGE_FORMAT 2u
#define AP_CAMPAIGN_RECEIPT_LIMIT 4096u

typedef struct {
    ap_uint32_t magic;
    ap_uint16_t format, reserved;
    ap_campaign_save identity;
    ap_check_set selected, observed;
    ap_uint32_t item_count;
    ap_uint32_t next_item_id;
    ap_uint16_t next_item_valid, transaction_reserved;
    ap_uint32_t item_ids[AP_CAMPAIGN_RECEIPT_LIMIT];
} ap_campaign_stage_wire;

// Consumes a complete native snapshot while all gameplay capabilities are off.
// The wire is variable-length on transport through item_ids[item_count].
int ap_campaign_stage_apply(const ap_campaign_stage_wire* wire);

#if defined(__cplusplus)
static_assert(AP_OFFSETOF(ap_campaign_stage_wire, identity) == 8);
static_assert(AP_OFFSETOF(ap_campaign_stage_wire, selected) == 40);
static_assert(AP_OFFSETOF(ap_campaign_stage_wire, observed) == 152);
static_assert(AP_OFFSETOF(ap_campaign_stage_wire, item_count) == 264);
static_assert(AP_OFFSETOF(ap_campaign_stage_wire, next_item_id) == 268);
static_assert(AP_OFFSETOF(ap_campaign_stage_wire, item_ids) == 276);
#else
_Static_assert(AP_OFFSETOF(ap_campaign_stage_wire, identity) == 8, "Stage identity offset");
_Static_assert(AP_OFFSETOF(ap_campaign_stage_wire, selected) == 40, "Stage selected offset");
_Static_assert(AP_OFFSETOF(ap_campaign_stage_wire, observed) == 152, "Stage observed offset");
_Static_assert(AP_OFFSETOF(ap_campaign_stage_wire, item_count) == 264, "Stage count offset");
_Static_assert(AP_OFFSETOF(ap_campaign_stage_wire, next_item_id) == 268, "Stage next item offset");
_Static_assert(AP_OFFSETOF(ap_campaign_stage_wire, item_ids) == 276, "Stage items offset");
#endif
