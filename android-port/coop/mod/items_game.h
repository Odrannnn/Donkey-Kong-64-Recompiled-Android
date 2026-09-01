#ifndef COOP_ITEMS_GAME_H
#define COOP_ITEMS_GAME_H
#include "items_policy.h"
#include "progression_game.h"
static inline unsigned coop_items_main_world(void) {
    return current_map == 34 || current_map == 7 || current_map == 38 || current_map == 26
        || current_map == 30 || current_map == 48 || current_map == 72 || current_map == 87;
}
static inline unsigned coop_items_safe_map(void) {
    // A fresh player can revisit this room before the training exit opens.
    // No training barrel, Cranky or fairy-queen reward actor is loaded here.
    return coop_items_main_world() || current_map == 171; // MAP_DK_HOUSE
}
static inline unsigned coop_items_snapshot_map(void) {
    // Reviewed ordinary gameplay maps keep the same persistent inventory
    // layout as the main worlds. They may establish and refresh a complete
    // ownership snapshot, but remain deferred so no remote write or save runs
    // while an interior/lobby/Helm script is loaded. Bosses, shops, races,
    // bonuses and other overlays are absent from coop_combat_map().
    // Galleon, Fungi and Caves lobbies have no supported enemy, so they are
    // absent from the combat allowlist despite using the same stable inventory.
    return coop_items_safe_map() || coop_combat_map(current_map)
        || current_map == 174 || current_map == 178 || current_map == 194;
}
#include "troff_game.h"
static inline unsigned coop_item_owned(unsigned id) {
    if (id >= COOP_TROFF_FIRST) return coop_troff_owned(id);
    if (id >= COOP_PROGRESSION_FIRST) return coop_progression_owned(id);
    if (id < COOP_PICKUP_FIRST || id >= COOP_ACTOR_PICKUP_FIRST) return isFlagSet(coop_item_flag(id), 0) != 0;
    unsigned index = id - COOP_PICKUP_FIRST, level = coop_pickup_level(index);
    return func_global_asm_8060E3B0(index - coop_pickup_start[level], level) != 0;
}
static inline unsigned coop_items_counters_valid(void) {
    if (!coop_progression_valid()) return 0;
    unsigned counts[8][5] = {{0}};
    for (unsigned id = COOP_SNIDE_FIRST; id < COOP_PICKUP_FIRST; ++id) {
        unsigned level, kong;
        if (coop_item_gb(id, &level, &kong) && coop_item_owned(id)) {
            if (id < COOP_MEDAL_FIRST && !isFlagSet(0x1D5 + id - COOP_SNIDE_FIRST, 0)) return 0;
            ++counts[level][kong];
        }
    }
    for (unsigned l = 0; l < 8; ++l) {
        // Different setups/randomizers change the ordinal-to-save-bit mapping.
        if (func_global_asm_80631C20(l) != coop_pickup_start[l + 1] - coop_pickup_start[l]) return 0;
        for (unsigned k = 0; k < 5; ++k) {
            CoopCharacterProgress* p = &D_global_asm_807FC950[0].character_progress[k];
            if (p->golden_bananas[l] != counts[l][k] || p->coins > 999
                    || p->coloured_bananas[l] + p->coloured_bananas_fed_to_tns[l] > 100) return 0;
        }
    }
    return 1;
}
static inline void coop_items_capture(CoopItems* g, unsigned enabled, unsigned adventure, unsigned file) {
    coop_items_prepare(g, enabled, adventure, file);
    g->deferred = 0;
    g->live_snapshot = 0;
    if (!g->input.ready) return;
    // Boss/minigame overlays can temporarily replace upgrades or melon counts.
    // Keep publishing the last complete safe snapshot instead of exporting the
    // overlay. This lets peers exchange item pages from different areas. A
    // fresh session still waits for its first verified safe snapshot.
    if (!coop_items_snapshot_map() || D_global_asm_807FD730) {
        if (g->baseline) {
            g->deferred = 1;
            // Once the local reward queue has drained, permanent flag-backed
            // rewards are stable even when numeric inventory fields belong to
            // an overlay. Extend only a dependency-valid cached snapshot; raw
            // pickup counters, shop tiers and Troff feeding still wait.
            if (!D_global_asm_807FD730) {
                unsigned observed[COOP_ITEM_WORDS];
                for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) observed[i] = g->input.owned[i];
                for (unsigned id = 0; id < COOP_ITEMS; ++id) {
                    int flag = coop_item_flag(id);
                    if (flag >= 0 && isFlagSet(flag, 0)) observed[id / 32] |= 1u << (id % 32);
                }
                if (coop_items_full_dependencies(observed)) {
                    for (unsigned id = 0; id < COOP_ITEMS; ++id)
                        if (coop_item_has(observed, id) && !coop_item_has(g->previous, id)) coop_items_note(g, id);
                    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i)
                        g->input.owned[i] = g->previous[i] = observed[i];
                }
            }
        } else g->input.ready = 0;
        return;
    }
    // Persistent bits precede the HUD credit. Wait for queued collectible awards.
    if (!coop_items_counters_valid()) { g->counter_error = 1; g->input.ready = 0; return; }
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) g->input.owned[i] = 0;
    for (unsigned id = 0; id < COOP_ITEMS; ++id) {
        if (coop_item_owned(id)) {
            g->input.owned[id / 32] |= 1u << (id % 32);
            // Ordinary pickups have no flag event. Rising-bit capture also
            // recovers permanent writes made while the credit queue suspended
            // readiness. Remote writes already update previous[] below.
            if (g->baseline && !coop_item_has(g->previous, id)) coop_items_note(g, id);
        }
    }
    if (!coop_items_full_dependencies(g->input.owned)) {
        g->counter_error = 1; g->input.ready = 0; return;
    }
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) g->previous[i] = g->input.owned[i];
    g->baseline = 1;
    g->live_snapshot = 1;
    // A reviewed ordinary interior can publish this freshly verified state,
    // including as the first snapshot of a session. Applications still wait
    // for the narrower save-safe map set above.
    if (!coop_items_safe_map()) g->deferred = 1;
}
static inline void coop_items_save_world_lobby(CoopItems* g, unsigned stable) {
    if (!g->world_save_pending || !stable || current_map != 194 || D_global_asm_807FD730) return;
    // No item grant is admitted in this publish-only lobby. This saves only
    // already-verified pending writes, including the reversible switch flag.
    func_global_asm_8060DEC8();
    g->world_save_pending = 0;
    g->save_pending = 0;
}
static inline void coop_items_apply(CoopItems* g, unsigned safe_to_save) {
    g->troff_pending = 0;
    if (!g->input.ready || g->deferred || g->file_changed || g->counter_error || D_global_asm_807FD730) return;
    unsigned here = getLevelIndex(current_map, 1);
    safe_to_save = safe_to_save && coop_items_safe_map();
    if (g->result.status == 2 || g->result.status == 3) {
        g->applying = 1;
        for (unsigned id = 0; id < COOP_ITEMS; ++id) {
            if (!coop_item_has(g->result.apply, id) || coop_item_owned(id)) continue;
            if (id >= COOP_TROFF_FIRST) {
                if (!coop_troff_apply(g, id, safe_to_save, here)) g->troff_pending = 1;
                if (g->counter_error) break;
                continue;
            }
            if (id >= COOP_PROGRESSION_FIRST) {
                if (!coop_progression_valid()) { g->counter_error = 1; break; }
                unsigned refresh = id >= COOP_WORLD_FIRST
                    && here == coop_world_unlocks[id - COOP_WORLD_FIRST].level;
                if (coop_progression_apply(id, safe_to_save, here, g->refresh_enabled)) {
                    // Simple loaded doors and gates enter the same completed
                    // vanilla state selected by their flag-positive initializer.
                    // Everything else retains the full map-rebuild fallback.
                    unsigned live = refresh && coop_live_world_refresh(
                        coop_world_unlocks[id - COOP_WORLD_FIRST].flag);
                    if (refresh && !live) {
                        g->refresh_pending = 1;
                        g->refresh_map = current_map;
                    }
                    // A tier also owns every lower tier; record all newly owned
                    // progression so the guest never echoes an implied grant.
                    for (unsigned p = COOP_PROGRESSION_FIRST; p < COOP_PROGRESSION_END; ++p)
                        if (coop_progression_owned(p)) g->previous[p / 32] |= 1u << (p % 32);
                    g->save_pending = 1;
                }
                continue;
            }
            unsigned level = 0, kong = 0, before = 0, amount = 1;
            unsigned gb = coop_item_gb(id, &level, &kong);
            unsigned pickup = id >= COOP_PICKUP_FIRST && id < COOP_ACTOR_PICKUP_FIRST;
            unsigned actor = id >= COOP_ACTOR_PICKUP_FIRST && id < COOP_PROGRESSION_FIRST, rainbow = 0;
            unsigned rainbow_before[5] = {0};
            unsigned short* counter = 0;
            if (pickup) {
                unsigned index = id - COOP_PICKUP_FIRST;
                level = coop_pickup_level(index); kong = coop_pickups[index].kong;
                amount = coop_pickups[index].amount;
            }
            if (actor) {
                const CoopActorPickup* a = &coop_actor_pickups[id - COOP_ACTOR_PICKUP_FIRST];
                level = a->level; kong = a->kong; amount = a->amount; rainbow = !amount;
            }
            if (gb || pickup || actor) {
                // Outside the reward's level no stale prop/reward script remains
                // loaded. Next entry sees the save bit and omits the old item.
                // Snide's flag-guarded menu is safe outside HQ, as in v0.7.
                if (!safe_to_save || !coop_items_main_world() || !D_global_asm_80754280 || here >= 8
                        || ((pickup || id >= COOP_GB_FIRST) && level == here)) continue;
                if (gb && id < COOP_MEDAL_FIRST && !isFlagSet(0x1D5 + id - COOP_SNIDE_FIRST, 0)) continue;
                CoopCharacterProgress* p = &D_global_asm_807FC950[0].character_progress[kong];
                if (rainbow) {
                    for (unsigned k = 0; k < 5; ++k) {
                        rainbow_before[k] = D_global_asm_807FC950[0].character_progress[k].coins;
                        if (rainbow_before[k] > 994) g->counter_error = 1;
                    }
                    if (g->counter_error) break;
                } else {
                    counter = gb ? &p->golden_bananas[level] : amount ? &p->coloured_bananas[level] : &p->coins;
                    before = *counter;
                    unsigned capacity = gb ? coop_snide_capacity(level, kong) : amount ? 100 - p->coloured_bananas_fed_to_tns[level] : 999;
                    if (!amount) amount = 1;
                    if (before + amount > capacity) { g->counter_error = 1; break; }
                }
            }
            if (pickup) func_global_asm_8060E430(id - COOP_PICKUP_FIRST - coop_pickup_start[level], 1, level);
            else setFlag(coop_item_flag(id), 1, 0);
            if (!coop_item_owned(id)) continue;
            if (rainbow) {
                for (unsigned k = 0; k < 5; ++k)
                    if (D_global_asm_807FC950[0].character_progress[k].coins != rainbow_before[k]) g->counter_error = 1;
                if (g->counter_error) break;
                for (unsigned k = 0; k < 5; ++k)
                    D_global_asm_807FC950[0].character_progress[k].coins = rainbow_before[k] + 5;
            }
            if (counter) {
                if (*counter != before) { g->counter_error = 1; break; } // Other mod writer.
                *counter = before + amount;
                if (gb) g->hud_pending = 1;
                // Remote CBs crossing 75 must still award that Kong's medal.
                if (((pickup && coop_pickups[id - COOP_PICKUP_FIRST].amount) || actor) && level < 7) {
                    unsigned total = *counter + D_global_asm_807FC950[0].character_progress[kong].coloured_bananas_fed_to_tns[level];
                    unsigned medal = COOP_MEDAL_FIRST + level * 5 + kong;
                    if (total >= 75 && !coop_item_owned(medal)) {
                        setFlag(coop_item_flag(medal), 1, 0);
                        if (coop_item_owned(medal)) coop_items_note(g, medal); // Newly derived reward, not echo.
                    }
                }
            }
            g->previous[id / 32] |= 1u << (id % 32);
            g->save_pending = 1;
        }
        // Split-course co-op can finish all four skills outside a training
        // barrel, so its local exit script may never derive the completion bit.
        // Derive only this flag, from readback, in a safe bound session. Never
        // derive Cranky's gift or run the training-completion cutscene remotely.
        if (!g->counter_error && safe_to_save && D_global_asm_80754280 && coop_training_prerequisites()
                && !coop_item_has(g->result.apply, COOP_TRAINING_COMPLETE)
                && !coop_item_owned(COOP_TRAINING_COMPLETE)) {
            setFlag(coop_item_flag(COOP_TRAINING_COMPLETE), 1, 0);
            if (coop_item_owned(COOP_TRAINING_COMPLETE)) {
                coop_items_note(g, COOP_TRAINING_COMPLETE);
                g->previous[COOP_TRAINING_COMPLETE / 32] |= 1u << (COOP_TRAINING_COMPLETE % 32);
                g->save_pending = 1;
            }
        }
        g->applying = 0;
    }
    if (g->counter_error) { g->input.ready = 0; return; }
    if (safe_to_save && g->hud_pending && D_global_asm_80754280) {
        func_global_asm_806F8278(0); g->hud_pending = 0;
    }
    if (safe_to_save && g->save_pending) {
        func_global_asm_8060DEC8(); g->save_pending = 0;
    }
}
#endif
