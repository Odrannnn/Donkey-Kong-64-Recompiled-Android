#ifndef COOP_WORLD_GAME_H
#define COOP_WORLD_GAME_H
#include "world_types.h"
typedef struct {
    CoopWorldInput input;
    CoopWorldResult result;
    unsigned baseline, previous;
} CoopWorld;
static const unsigned short coop_world_flags[COOP_WORLD_TOGGLES] = {0xA0, 0xCE, 0x19D};
// The lobby switch is sampled as reversible authority, but applies only after
// leaving its loaded lobby, so it has no excluded main-world level.
static const unsigned char coop_world_levels[COOP_WORLD_TOGGLES] = {3, 4, 255};
static inline void coop_world_capture(CoopWorld* w, const CoopItems* g) {
    w->input.enabled = g->input.enabled && !g->counter_error;
    w->input.file = g->input.file;
    // Item snapshots may use a last-known-safe cache in overlay maps. Reversible
    // world toggles must still be sampled live, so never derive a revision from
    // that deferred frame.
    w->input.ready = w->input.enabled && g->input.ready
        && (!g->deferred || g->live_snapshot) && g->bound;
    if (!w->input.ready) return;
    unsigned values = 0;
    for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i)
        if (isFlagSet(coop_world_flags[i], 0)) values |= 1u << i;
    for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i) {
        // Capture verified changes after scripts/cutscenes finish. A pre-write
        // flag callback could still be cancelled by another mod.
        if (w->baseline && ((w->previous ^ values) & (1u << i))) {
            if (!++w->input.change[i]) ++w->input.change[i];
        }
    }
    w->baseline = 1; w->previous = values; w->input.values = values;
}
static inline void coop_world_receive(CoopWorld* w, CoopWorldResult result) {
    w->result = result;
    w->input.session_hi = result.session_hi; w->input.session_lo = result.session_lo;
    w->input.scope = result.scope;
}
static inline void coop_world_apply(CoopWorld* w, CoopItems* g, unsigned playing) {
    if (!playing || !w->input.ready || !g->input.ready || !g->bound || g->file_changed
            || g->counter_error || D_global_asm_807FD730
            || !w->result.scope || !w->result.status) return;
    unsigned here = getLevelIndex(current_map, 1);
    for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i) {
        unsigned bit = 1u << i;
        unsigned lobby_live = i == 2 && current_map == 194;
        if (!coop_items_safe_map() && !lobby_live) continue;
        unsigned refresh = lobby_live || here == coop_world_levels[i];
        if (!(w->result.apply & bit) || (refresh && !g->refresh_enabled)) continue;
        unsigned desired = (w->result.desired & bit) != 0;
        if ((isFlagSet(coop_world_flags[i], 0) != 0) == desired) continue;
        // Keep the saved flag and loaded switch atomic. A setup that is still
        // loading, temporarily absent, or replaced by another setup remains a
        // pending network request; leaving the area applies it safely without
        // touching a loaded controller.
        if (refresh && !coop_live_reversible_ready(i, desired)) continue;
        setFlag(coop_world_flags[i], desired, 0);
        if ((isFlagSet(coop_world_flags[i], 0) != 0) == desired) {
            w->previous = (w->previous & ~bit) | (w->result.desired & bit);
            g->save_pending = 1; // Use the existing isolated-save request path.
            if (lobby_live) g->world_save_pending = 1;
            if (refresh) coop_live_reversible_refresh(i, desired);
        }
    }
}
#endif
