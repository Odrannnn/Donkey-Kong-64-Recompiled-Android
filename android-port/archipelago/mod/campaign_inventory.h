// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_receipts.h"
#include "../generated/catalog.h"

// Derived campaign ownership rebuilt from the durable ReceivedItems prefix.
// Spendable resources and one-shot trap state deliberately do not live here.
typedef struct {
    ap_uint8_t moves[5], guns[5], instruments[5], hints[5], blueprints[5];
    ap_uint8_t kongs, keys, abilities, special, slam, belt, instrument_upgrades, shopkeepers, climbing;
    ap_uint8_t reserved[2];
    ap_uint32_t golden_bananas, fairies, crowns, medals, pearls, rainbow_coins, junk;
    ap_uint32_t traps[AP_TRAP_TYPE_COUNT];
    ap_uint32_t flags[AP_ITEM_FLAG_WORDS];
} ap_campaign_inventory;

// add: 1 supported, 0 known but unsupported, -1 unknown/corrupt table entry.
// rebuild publishes only a fully supported prefix and leaves output unchanged on failure.
#if defined(__cplusplus)
extern "C" {
#endif
void ap_campaign_inventory_clear(ap_campaign_inventory* inventory);
int ap_campaign_inventory_add(ap_campaign_inventory* inventory, ap_uint32_t item_id);
int ap_campaign_inventory_rebuild(ap_campaign_inventory* output, const ap_uint32_t* item_ids, unsigned count);
int ap_campaign_inventory_has_flag(const ap_campaign_inventory* inventory, unsigned flag);
// Restart reconciliation. Stage is permitted only before gameplay capabilities
// are enabled and only for the exact receipt prefix committed in the trailer.
int ap_campaign_inventory_stage(const ap_campaign_save* identity, const ap_uint32_t* item_ids, unsigned count);
int ap_campaign_inventory_snapshot(ap_campaign_inventory* output, const ap_campaign_save* identity);
void ap_campaign_inventory_reset(void);
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
static_assert(sizeof(ap_item_effect) == 4 && sizeof(ap_item_effect_def) == 12);
static_assert(sizeof(ap_campaign_inventory) == 252);
static_assert(AP_OFFSETOF(ap_campaign_inventory, moves) == 0);
static_assert(AP_OFFSETOF(ap_campaign_inventory, guns) == 5);
static_assert(AP_OFFSETOF(ap_campaign_inventory, instruments) == 10);
static_assert(AP_OFFSETOF(ap_campaign_inventory, hints) == 15);
static_assert(AP_OFFSETOF(ap_campaign_inventory, blueprints) == 20);
static_assert(AP_OFFSETOF(ap_campaign_inventory, kongs) == 25);
static_assert(AP_OFFSETOF(ap_campaign_inventory, golden_bananas) == 36);
static_assert(AP_OFFSETOF(ap_campaign_inventory, traps) == 64);
static_assert(AP_OFFSETOF(ap_campaign_inventory, flags) == 128);
#else
_Static_assert(sizeof(ap_item_effect) == 4 && sizeof(ap_item_effect_def) == 12, "Item effect ABI layout");
_Static_assert(sizeof(ap_campaign_inventory) == 252, "Campaign inventory ABI layout");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, moves) == 0, "Campaign moves offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, guns) == 5, "Campaign guns offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, instruments) == 10, "Campaign instruments offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, hints) == 15, "Campaign hints offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, blueprints) == 20, "Campaign blueprints offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, kongs) == 25, "Campaign kongs offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, golden_bananas) == 36, "Campaign count offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, traps) == 64, "Campaign traps offset");
_Static_assert(AP_OFFSETOF(ap_campaign_inventory, flags) == 128, "Campaign flags offset");
#endif
