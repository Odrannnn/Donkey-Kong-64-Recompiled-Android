#ifndef COOP_TROFF_GAME_H
#define COOP_TROFF_GAME_H
#include "troff_ids.h"
static inline unsigned coop_troff_owned(unsigned id) {
    unsigned level, kong, amount;
    return coop_troff_bucket(id, &level, &kong, &amount)
        && D_global_asm_807FC950[0].character_progress[kong].coloured_bananas_fed_to_tns[level] >= amount;
}
static inline unsigned coop_troff_apply(CoopItems* g, unsigned id, unsigned safe, unsigned here) {
    unsigned level, kong, amount;
    if (!coop_troff_bucket(id, &level, &kong, &amount) || !safe || !g->bound
            || !coop_items_main_world() || !D_global_asm_80754280 || here >= 8 || here == level) return 0;
    CoopCharacterProgress* p = &D_global_asm_807FC950[0].character_progress[kong];
    unsigned fed = p->coloured_bananas_fed_to_tns[level], available = p->coloured_bananas[level];
    if (fed + available > COOP_TROFF_CAPACITY) { g->counter_error = 1; return 0; }
    // Earlier milestones and incoming pickup credits must apply first. Do not
    // invent bananas or acknowledge feeding that the local inventory cannot fund.
    if (amount != fed + 1 || !available) return 0;
    unsigned total = 0;
    for (unsigned k = 0; k < COOP_TROFF_KONGS; ++k) {
        CoopCharacterProgress* other = &D_global_asm_807FC950[0].character_progress[k];
        unsigned paid = other->coloured_bananas_fed_to_tns[level];
        if (paid + other->coloured_bananas[level] > COOP_TROFF_CAPACITY) {
            g->counter_error = 1; return 0;
        }
        total += paid;
    }
    // Vanilla's flying-banana effect updates this cache after the fed counter.
    // Outside the entire level there is no live feeding animation. Rebuild the
    // cache from actual counters, allowing a lagging cache but never an excess.
    if (D_global_asm_807FC930[level] > total) { g->counter_error = 1; return 0; }
    p->coloured_bananas[level] = available - 1;
    p->coloured_bananas_fed_to_tns[level] = fed + 1;
    D_global_asm_807FC930[level] = total + 1;
    if (p->coloured_bananas[level] != available - 1 || p->coloured_bananas_fed_to_tns[level] != fed + 1
            || D_global_asm_807FC930[level] != total + 1) { g->counter_error = 1; return 0; }
    g->previous[id / 32] |= 1u << (id % 32); // Readback, not a new local payment to echo.
    g->save_pending = 1;
    return 1;
}
#endif
