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
    // Galleon's treasure chest and Mermaid room, plus the Helm lobby's two
    // Bananaports, are admitted specifically for exact reviewed live refresh;
    // other grants stay deferred.
    return coop_items_safe_map() || current_map == 176 || coop_combat_map(current_map)
        || current_map == 174 || current_map == 178 || current_map == 187
        || current_map == 194 || current_map == 44 || current_map == 45
        || current_map == 170;
}
static inline unsigned coop_training_ground_apply_id(unsigned id) {
    return (id >= COOP_TRAINING_SPAWNED && id <= COOP_FIRST_SLAM)
        || id == COOP_WORLD_TRAINING_EXIT;
}
#include "troff_game.h"
static inline unsigned coop_item_owned(unsigned id) {
    if (id == COOP_JAPES_BOULDER_BUNCH) return isFlagSet(0x01D, 0) != 0;
    if (id == COOP_KROOL_DEFEATED) return isFlagSet(0x1B0, 0) != 0;
    if (id == COOP_ARCADE_COINS_PAID) return isFlagSet(0x083, 0) != 0;
    if (id >= COOP_TROFF_FIRST && id < COOP_TROFF_END) return coop_troff_owned(id);
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
    g->training_scene_pending = 0;
    g->wait_reason = COOP_TRACE_WAIT_NONE;
    g->wait_id = 0xFFFFFFFFu;
    if (!g->input.ready) {
        coop_items_wait(g, COOP_TRACE_WAIT_SNAPSHOT, 0xFFFFFFFFu); return;
    }
    unsigned stable = safe_to_save;
    unsigned training_ground = stable && current_map == 176;
    unsigned live_world_applied = 0;
    unsigned live_only = g->deferred && !training_ground;
    unsigned pending_live_world = 0;
    if (live_only && stable && g->refresh_enabled
            && (g->result.status == 2 || g->result.status == 3)) {
        for (unsigned id = COOP_WORLD_FIRST; id < COOP_PROGRESSION_END; ++id)
            if (coop_item_has(g->result.apply, id) && !coop_item_owned(id)
                    && coop_live_world_has_flag(
                        coop_world_unlocks[id - COOP_WORLD_FIRST].flag)) {
                pending_live_world = 1; break;
            }
    }
    if (live_only && !pending_live_world) {
        coop_items_wait(g, COOP_TRACE_WAIT_LOCAL_AREA, 0xFFFFFFFFu); return;
    }
    if (g->file_changed) {
        coop_items_wait(g, COOP_TRACE_WAIT_FILE, 0xFFFFFFFFu); return;
    }
    if (g->counter_error) {
        coop_items_wait(g, COOP_TRACE_WAIT_COUNTER, 0xFFFFFFFFu); return;
    }
    if (D_global_asm_807FD730) {
        coop_items_wait(g, COOP_TRACE_WAIT_REWARD_QUEUE, 0xFFFFFFFFu); return;
    }
    unsigned here = getLevelIndex(current_map, 1);
    safe_to_save = stable && coop_items_safe_map();
    if (g->result.status == 2 || g->result.status == 3) {
        g->applying = 1;
        for (unsigned id = 0; id < COOP_ITEMS; ++id) {
            if (!coop_item_has(g->result.apply, id) || coop_item_owned(id)) continue;
            unsigned exact_live_world = id >= COOP_WORLD_FIRST
                && id < COOP_PROGRESSION_END && stable && g->refresh_enabled
                && coop_live_world_has_flag(
                    coop_world_unlocks[id - COOP_WORLD_FIRST].flag);
            // A reviewed interior admits only its exact loaded permanent-world
            // unit. Inventory, rewards, purchases and unrelated flags remain
            // deferred until the ordinary save-safe map policy resumes.
            if (live_only && !exact_live_world) {
                coop_items_wait(g, COOP_TRACE_WAIT_LOCAL_AREA, id); continue;
            }
            if (id >= COOP_TROFF_FIRST && id < COOP_TROFF_END) {
                if (!coop_troff_apply(g, id, safe_to_save, here)) {
                    g->troff_pending = 1;
                    coop_items_wait(g, COOP_TRACE_WAIT_TROFF, id);
                }
                if (g->counter_error) break;
                continue;
            }
            if (id >= COOP_PROGRESSION_FIRST && id < COOP_PROGRESSION_END) {
                if (!coop_progression_valid()) { g->counter_error = 1; break; }
                unsigned progression_safe = safe_to_save || exact_live_world
                    || (training_ground && coop_training_ground_apply_id(id));
                if (!progression_safe) {
                    coop_items_wait(g, COOP_TRACE_WAIT_LOCAL_AREA, id); continue;
                }
                unsigned refresh = exact_live_world || (id >= COOP_WORLD_FIRST
                    && here == coop_world_unlocks[id - COOP_WORLD_FIRST].level);
                if (training_ground && id == COOP_WORLD_TRAINING_EXIT) refresh = 1;
                if (live_only && exact_live_world && !coop_live_world_ready(
                        coop_world_unlocks[id - COOP_WORLD_FIRST].flag)) {
                    coop_items_wait(g, COOP_TRACE_WAIT_PROGRESSION_CONTEXT, id); continue;
                }
                if (coop_progression_apply(id, progression_safe, here,
                        g->refresh_enabled)) {
                    // Simple loaded doors and gates enter the same completed
                    // vanilla state selected by their flag-positive initializer.
                    // Everything else retains the full map-rebuild fallback.
                    unsigned live = refresh && coop_live_world_refresh(
                        coop_world_unlocks[id - COOP_WORLD_FIRST].flag);
                    if (exact_live_world && live) live_world_applied = 1;
                    if (refresh && !live && !training_ground) {
                        g->refresh_pending = 1;
                        g->refresh_map = current_map;
                    }
                    if (training_ground && id == COOP_TRAINING_COMPLETE
                            && g->peer_same_map) g->training_scene_pending = 1;
                    // A tier also owns every lower tier; record all newly owned
                    // progression so the guest never echoes an implied grant.
                    for (unsigned p = COOP_PROGRESSION_FIRST; p < COOP_PROGRESSION_END; ++p)
                        if (coop_progression_owned(p)) g->previous[p / 32] |= 1u << (p % 32);
                    g->save_pending = 1;
                } else coop_items_wait(g, COOP_TRACE_WAIT_PROGRESSION_CONTEXT, id);
                continue;
            }
            unsigned level = 0, kong = 0, before = 0, amount = 1;
            unsigned gb = coop_item_gb(id, &level, &kong);
            unsigned pickup = id >= COOP_PICKUP_FIRST && id < COOP_ACTOR_PICKUP_FIRST;
            unsigned actor = id >= COOP_ACTOR_PICKUP_FIRST && id < COOP_PROGRESSION_FIRST, rainbow = 0;
            unsigned boulder_bunch = id == COOP_JAPES_BOULDER_BUNCH;
            unsigned arcade_paid = id == COOP_ARCADE_COINS_PAID;
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
            if (boulder_bunch) { level = 0; kong = 4; amount = 5; }
            // This flag controls access to a later Arcade run. Let Factory and
            // the Arcade overlay unload before changing it; never copy its fee.
            if (arcade_paid && (!safe_to_save || here == 2)) {
                coop_items_wait(g, COOP_TRACE_WAIT_SAVE_UNSAFE, id); continue;
            }
            if (gb || pickup || actor || boulder_bunch) {
                // Outside the reward's level no stale prop/reward script remains
                // loaded. Next entry sees the save bit and omits the old item.
                // Snide's flag-guarded menu is safe outside HQ, as in v0.7.
                if (!safe_to_save || !coop_items_main_world() || here >= 8) {
                    coop_items_wait(g, COOP_TRACE_WAIT_SAVE_UNSAFE, id); continue;
                }
                if (!D_global_asm_80754280) {
                    coop_items_wait(g, COOP_TRACE_WAIT_HUD, id); continue;
                }
                if ((pickup || actor || boulder_bunch) && level == here) {
                    coop_items_wait(g, COOP_TRACE_WAIT_SAME_LEVEL_ITEM, id); continue;
                }
                if (id >= COOP_GB_FIRST && id < COOP_PICKUP_FIRST && level == here
                        && !coop_gb_same_level_safe(id, current_map)) {
                    coop_items_wait(g, COOP_TRACE_WAIT_SAME_LEVEL_ITEM, id); continue;
                }
                if (gb && id < COOP_MEDAL_FIRST && !isFlagSet(0x1D5 + id - COOP_SNIDE_FIRST, 0)) {
                    coop_items_wait(g, COOP_TRACE_WAIT_PROGRESSION_CONTEXT, id); continue;
                }
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
                if (((pickup && coop_pickups[id - COOP_PICKUP_FIRST].amount) || actor || boulder_bunch) && level < 7) {
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
        if (!g->counter_error && (safe_to_save || training_ground) && D_global_asm_80754280
                && coop_training_prerequisites()
                && !coop_item_has(g->result.apply, COOP_TRAINING_COMPLETE)
                && !coop_item_owned(COOP_TRAINING_COMPLETE)) {
            setFlag(coop_item_flag(COOP_TRAINING_COMPLETE), 1, 0);
            if (coop_item_owned(COOP_TRAINING_COMPLETE)) {
                coop_items_note(g, COOP_TRAINING_COMPLETE);
                g->previous[COOP_TRAINING_COMPLETE / 32] |= 1u << (COOP_TRAINING_COMPLETE % 32);
                g->save_pending = 1;
                if (training_ground && g->peer_same_map) g->training_scene_pending = 1;
            }
        }
        g->applying = 0;
    }
    if (g->counter_error) { g->input.ready = 0; return; }
    unsigned save_safe = safe_to_save || training_ground || live_world_applied;
    if (save_safe && g->hud_pending && D_global_asm_80754280) {
        func_global_asm_806F8278(0); g->hud_pending = 0;
    }
    if (save_safe && g->save_pending) {
        func_global_asm_8060DEC8(); g->save_pending = 0;
    }
}
#endif
