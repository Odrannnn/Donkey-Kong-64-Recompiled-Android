// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "checks.h"
#include "shop_labels.h"
#include "gameplay_mode.h"
#define AP_SHOP_ITEM_TYPE 21
// Host/game contract. Prices are the original AP cumulative thresholds; never
// subtract them or derive them from the item received at a location.
typedef struct { unsigned int location; unsigned short price, reserved; } ap_shop_offer;
typedef struct {
    unsigned char unused0[2]; unsigned short flag;
    unsigned char kong, price, unused6[5]; signed char item_type;
    unsigned char level, state, substate, response, melons, item_level;
} ap_shop_paad;
#if defined(__cplusplus)
static_assert(sizeof(ap_shop_offer) == 8 && sizeof(ap_shop_paad) == 0x12);
#else
_Static_assert(sizeof(ap_shop_offer) == 8 && sizeof(ap_shop_paad) == 0x12, "Shop ABI layout");
#endif
// Internal game-thread interfaces, not native/mod exports. Preparation cannot
// change the session while any gameplay gate is enabled. Restored checks must
// come from the future seed-bound durable campaign journal.
int ap_prepare_ap_shops(const ap_shop_offer* offers, unsigned int count, const ap_check_set* restored);
int ap_apshop_restore(const ap_check_set* restored);
void ap_apshop_snapshot(ap_check_set* result);
void ap_apshop_select(ap_shop_paad* paad, void* moves, int first_visit);
void ap_apshop_purchase(ap_shop_paad* paad, void* moves);
int ap_apshop_afford(ap_shop_paad* paad, int unused);
int ap_apshop_context(void);
int ap_apshop_committed(const ap_shop_paad* paad);
// Metadata snapshots replace atomically, are restricted to the prepared seed's shops, and
// independent of received inventory/check state. Future trusted game-thread
// transport must reject responses from a different seed/session before calling.
int ap_apshop_set_labels(const ap_shop_label* labels, unsigned int count);
unsigned int ap_apshop_generation(void);
int ap_apshop_display(void* owner, const ap_shop_paad* paad, ap_shop_label* result);
