#ifndef COOP_PROGRESSION_GAME_H
#define COOP_PROGRESSION_GAME_H
#include "progression_ids.h"
static inline unsigned coop_progression_owned(unsigned id) {
    if (id < COOP_PROGRESSION_FIRST || id >= COOP_PROGRESSION_END) return 0;
    if (id < COOP_MOVE_FIRST || id >= COOP_TRAINING_FIRST) return isFlagSet(coop_progression_flag(id), 0) != 0;
    if (id >= COOP_MELON_FIRST) return D_global_asm_807FC950[0].melons >= 2 + id - COOP_MELON_FIRST;
    unsigned kong, field, value, bits;
    if (!coop_progression_move(id, &kong, &field, &value, &bits)) return 0;
    for (unsigned k = 0; k < 5; ++k) if (kong == 5 || k == kong) {
        unsigned have = D_global_asm_807FC950[0].character_progress[k].progression[field];
        if (bits ? !(have & value) : have < value) return 0;
    }
    return 1;
}
static inline unsigned coop_progression_valid(void) {
    const unsigned char* first = D_global_asm_807FC950[0].character_progress[0].progression;
    if (D_global_asm_807FC950[0].melons > 3) return 0;
    for (unsigned k = 0; k < 5; ++k) {
        const unsigned char* p = D_global_asm_807FC950[0].character_progress[k].progression;
        // Pinned vanilla shop: moves/slam/belt are tiers, gun/instrument bits.
        if (p[0] > 3 || p[1] > 3 || p[2] > 7 || p[3] > 2 || p[4] > 15) return 0;
        if (isFlagSet(0x180, 0) && p[1] == 0) return 0; // A first-slam flag alone is not its inventory grant.
        if (p[1] != first[1] || p[3] != first[3] || (p[2] & 6) != (first[2] & 6)
                || (p[4] & 14) != (first[4] & 14)) return 0;
    }
    return 1;
}
static inline unsigned coop_training_prerequisites(void) {
    if (!isFlagSet(0x17F, 0)) return 0;
    for (unsigned i = 0; i < 4; ++i) if (!isFlagSet(0x182 + i, 0)) return 0;
    return 1;
}
static inline unsigned coop_progression_apply(unsigned id, unsigned safe, unsigned here,
        unsigned refresh_loaded_world) {
    if (!safe || !D_global_asm_80754280 || here >= 8 || id < COOP_PROGRESSION_FIRST || id >= COOP_PROGRESSION_END) return 0;
    if (id >= COOP_WORLD_FIRST) {
        const CoopWorldUnlock* unlock = &coop_world_unlocks[id - COOP_WORLD_FIRST];
        // Normally leave the entire level: related actors/scripts may exist in
        // both the main map and its interiors. The optional refresh path is
        // admitted only by the caller's safe-main-map gate and immediately
        // reloads that map after the verified flag write and isolated save.
        if (here == unlock->level && !refresh_loaded_world) return 0;
        if (unlock->prerequisite >= 0
                && !isFlagSet(coop_world_unlocks[(unsigned)unlock->prerequisite].flag, 0)) return 0;
        for (unsigned r = 0; r < COOP_WORLD_REQUIREMENT_COUNT; ++r) {
            const CoopWorldRequirement* requirement = &coop_world_requirements[r];
            if (requirement->flag != unlock->flag) continue;
            unsigned owned = 0;
            for (unsigned i = 0; i < requirement->count; ++i)
                owned += isFlagSet(coop_item_flag(requirement->first + i), 0) != 0;
            if (owned < requirement->minimum) return 0;
        }
        // Only the permanent completion bit. No live script state, cutscene,
        // carried bean, race timer, pickup drop or extra GB/inventory award.
        setFlag(unlock->flag, 1, 0);
    } else if (id >= COOP_WARP_FIRST) {
        if (id < COOP_BLOCKER_FIRST) {
            const CoopWarpTag* pad = &coop_warp_tags[id - COOP_WARP_FIRST];
            // Never change the state of a loaded pad or force a teleport.
            // Interiors resume capture when returning to an allowed safe map.
            if ((unsigned)current_map == pad->map) return 0;
            if (pad->required_item >= 0 && !isFlagSet(coop_item_flag((unsigned)pad->required_item), 0)) return 0;
        }
        // B. Locker lives only in lobbies, all excluded by the caller's map
        // allowlist. Its next initialization reads the saved clearance flag.
        // No GB deduction, threshold rewrite, conversation or actor patch.
        setFlag(coop_progression_flag(id), 1, 0);
    } else if (id >= COOP_TRAINING_FIRST) {
        // Only the caller's allowlisted maps (including DK's house) are safe;
        // never deliver inside loaded training barrels, the grounds, or shops.
        if (id >= COOP_TRAINING_SKILL_FIRST && id < COOP_TRAINING_COMPLETE && !isFlagSet(0x17F, 0)) return 0;
        if ((id == COOP_TRAINING_COMPLETE || id == COOP_FIRST_SLAM) && !coop_training_prerequisites()) return 0;
        setFlag(coop_progression_flag(id), 1, 0);
        if (!coop_progression_owned(id)) return 0; // Canceled flag: no free slam or acknowledgement.
        if (id == COOP_FIRST_SLAM) {
            for (unsigned k = 0; k < 5; ++k) {
                unsigned char* slam = &D_global_asm_807FC950[0].character_progress[k].progression[1];
                if (*slam < 1) *slam = 1;
            }
        }
        // Camera/shockwave and orange unlocks never copy/refill local resources.
    } else if (id < COOP_MOVE_FIRST) {
        if (id < COOP_LOBBY) {
            // Diddy/Japes, Tiny and Lanky/Aztec, Chunky/Factory. Never rescue a
            // loaded prisoner or replay their GB/cutscene from the network.
            static const unsigned levels[4] = {0, 1, 1, 2};
            if (here == levels[id - COOP_KONG_FIRST]) return 0;
        } else {
            // Isles contains K. Lumsy and the lobby-opening scenes. All peers
            // reload those objects from their local save on the next visit.
            if (here == 7) return 0;
            if (id >= COOP_KEY_TURN_FIRST && id < COOP_KLUMSY_FREE
                    && (!isFlagSet(coop_item_flag(70 + id - COOP_KEY_TURN_FIRST), 0)
                        || !isFlagSet(coop_item_flag(COOP_LOBBY), 0))) return 0;
            if (id == COOP_KLUMSY_FREE) {
                if (!isFlagSet(coop_item_flag(COOP_LOBBY), 0)) return 0;
                for (unsigned i = 0; i < 8; ++i)
                    if (!isFlagSet(coop_item_flag(COOP_KEY_TURN_FIRST + i), 0)) return 0;
            }
        }
        setFlag(coop_progression_flag(id), 1, 0);
    } else {
        // The first-slam gift, local or shared, must precede higher upgrades.
        // This prevents a later Cranky gift from writing a downgrade to slam=1.
        if (!isFlagSet(0x180, 0)) return 0;
        if (id >= COOP_MELON_FIRST) {
            unsigned value = 2 + id - COOP_MELON_FIRST;
            if (D_global_asm_807FC950[0].melons < value) D_global_asm_807FC950[0].melons = value;
        } else {
            unsigned kong, field, value, bits;
            if (!coop_progression_move(id, &kong, &field, &value, &bits)) return 0;
            for (unsigned k = 0; k < 5; ++k) if (kong == 5 || k == kong) {
                unsigned char* have = &D_global_asm_807FC950[0].character_progress[k].progression[field];
                if (bits) *have |= value;
                else if (*have < value) *have = value;
            }
        }
        // No purchase routine: no second charge, health heal or ammo refill.
    }
    return coop_progression_owned(id);
}
#endif
