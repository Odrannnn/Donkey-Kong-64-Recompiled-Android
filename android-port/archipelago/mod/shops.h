// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
// Original move-shop record: the former 16-bit price is split into Kong/price.
// These are legacy move offers, not AP location rewards. Full campaign shops
// additionally need the original getNextMovePurchase/purchaseMove replacements.
typedef struct { short type, rank; unsigned char kong, price; } ap_shop_move;
_Static_assert(sizeof(ap_shop_move) == 6, "Original move-shop table stride");
extern ap_shop_move ap_cranky_moves[5][7], ap_funky_moves[5][7], ap_candy_moves[5][7];
// Internal seed preparation only. No mod export or user setting enables this.
int ap_prepare_move_shops(const ap_shop_move offers[3][5][7]);
