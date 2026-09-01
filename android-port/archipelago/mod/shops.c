// SPDX-License-Identifier: GPL-3.0-or-later
#include "shops.h"
#define EMPTY {-1, 0, 0, 0}
#define ROW {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}
ap_shop_move ap_cranky_moves[5][7] = {ROW, ROW, ROW, ROW, ROW};
ap_shop_move ap_funky_moves[5][7] = {ROW, ROW, ROW, ROW, ROW};
ap_shop_move ap_candy_moves[5][7] = {ROW, ROW, ROW, ROW, ROW};
extern volatile unsigned int ap_gameplay_mode;

int ap_prepare_move_shops(const ap_shop_move offers[3][5][7]) {
    if (!offers || ap_gameplay_mode) return 0;
    // Validate the complete table before modifying anything. Invalid ranks must
    // never reach a MIPS variable shift, and Kong bytes must stay in bounds.
    const unsigned char maximum[5] = {3, 3, 3, 2, 4};
    for (unsigned int v = 0; v < 3; ++v) for (unsigned int k = 0; k < 5; ++k) for (unsigned int l = 0; l < 7; ++l) {
        const ap_shop_move* p = &offers[v][k][l];
        if (p->kong >= 5 || p->type < -1 || p->type > 4) return 0;
        if (p->type == -1) { if (p->rank || p->price) return 0; }
        else if (p->rank < 1 || p->rank > maximum[p->type]) return 0;
    }
    ap_shop_move (*tables[3])[7] = {ap_cranky_moves, ap_funky_moves, ap_candy_moves};
    for (unsigned int v = 0; v < 3; ++v) for (unsigned int k = 0; k < 5; ++k) for (unsigned int l = 0; l < 7; ++l)
        tables[v][k][l] = offers[v][k][l];
    return 1;
}
