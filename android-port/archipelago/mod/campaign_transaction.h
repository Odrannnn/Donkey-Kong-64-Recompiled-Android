// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_inventory.h"
#include "gameplay_mode.h"

#define AP_CAMPAIGN_TRANSACTION_MAGIC 0x41505431u /* "APT1" */
#define AP_CAMPAIGN_TRANSACTION_FORMAT 1u

enum {
    AP_TRANSACTION_NOOP = 1u,
    AP_TRANSACTION_OWNERSHIP = 2u,
    AP_TRANSACTION_COUNTED = 4u,
    AP_TRANSACTION_TRAP = 8u,
};

// A proof that one catalog receipt advances one exact staged prefix. This is
// intentionally only a plan: it does not mutate game state, advance the save
// trailer, queue EEPROM I/O, or enable gameplay capabilities.
typedef struct {
    ap_uint32_t magic;
    ap_uint16_t format, classes;
    ap_uint32_t item_id, reserved;
    ap_campaign_save current, next;
    ap_campaign_inventory before, after;
} ap_campaign_transaction;

#if defined(__cplusplus)
extern "C" {
#endif
int ap_campaign_transaction_build(ap_campaign_transaction* output, const ap_campaign_save* identity,
    const ap_campaign_inventory* before, ap_uint32_t item_id);
// Stage/snapshot are game-thread publication helpers. Snapshot returns 1 for a
// pending receipt, 2 when the durable journal has no next receipt, and 0 when
// unavailable or stale. A complete snapshot leaves output untouched.
int ap_campaign_transaction_stage(const ap_campaign_save* identity, const ap_campaign_inventory* before,
    ap_uint32_t item_id, unsigned has_item);
int ap_campaign_transaction_snapshot(ap_campaign_transaction* output, const ap_campaign_save* identity);
// Execute only the subset represented completely by stock CharacterProgress
// or the trailer-bound receipt query: special moves, guns, instruments, slam,
// ammo belt, abilities, Kongs, keys, Climbing, shopkeeper unlocks, company
// coins, Bean, hints, counted collectibles, catalog permanent flags and no-op
// receipts. Disable A/B/Z/C-Up, Dry and Ice Floor traps reproduce their original state
// effects after the original safety checks. Flag-backed items set their original permanent flags. Golden Bananas use the
// original AP client's balanced live distribution; Rainbow Coins add five live
// coins per Kong. Other query-owned effects do not write randomizer-only state
// into stock EEPROM. Success advances the trailer and invokes the stock save
// entry point. The caller must already enable MOVES|ITEMS|SAVE at a validated
// safe frame; trap receipts additionally require the TRAPS capability.
int ap_campaign_transaction_commit(const ap_campaign_transaction* transaction);
void ap_campaign_transaction_reset(void);
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
static_assert(sizeof(ap_campaign_transaction) == 584);
static_assert(AP_OFFSETOF(ap_campaign_transaction, current) == 16);
static_assert(AP_OFFSETOF(ap_campaign_transaction, next) == 48);
static_assert(AP_OFFSETOF(ap_campaign_transaction, before) == 80);
static_assert(AP_OFFSETOF(ap_campaign_transaction, after) == 332);
#else
_Static_assert(sizeof(ap_campaign_transaction) == 584, "Campaign transaction ABI layout");
_Static_assert(AP_OFFSETOF(ap_campaign_transaction, current) == 16, "Transaction current offset");
_Static_assert(AP_OFFSETOF(ap_campaign_transaction, next) == 48, "Transaction next offset");
_Static_assert(AP_OFFSETOF(ap_campaign_transaction, before) == 80, "Transaction before offset");
_Static_assert(AP_OFFSETOF(ap_campaign_transaction, after) == 332, "Transaction after offset");
#endif
