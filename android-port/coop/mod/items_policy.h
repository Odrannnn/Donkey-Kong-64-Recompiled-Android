#ifndef COOP_ITEMS_POLICY_H
#define COOP_ITEMS_POLICY_H
#include "collectible_ids.h"
#include "progression_ids.h"
#include "troff_ids.h"
// Stable item IDs, never arbitrary flags supplied by a peer. All ABI fields are words.
enum { COOP_ITEMS = COOP_TROFF_END, COOP_ITEM_WORDS = 192, COOP_ITEM_PAGE_WORDS = 24, COOP_ITEM_PAGES = 8,
    COOP_ITEM_WIRE_WORDS = 5 + 2 * COOP_ITEM_PAGE_WORDS,
    COOP_SNIDE_FIRST = 80, COOP_SNIDE_COUNT = 40, COOP_MEDAL_FIRST = 120,
    COOP_GB_FIRST = 160, COOP_PICKUP_FIRST = 321, COOP_ACTOR_PICKUP_FIRST = 2021 };
typedef struct {
    unsigned enabled, file, ready, owned[COOP_ITEM_WORDS], request[COOP_ITEM_WORDS], session_hi, session_lo, scope;
} CoopItemInput;
typedef struct {
    unsigned status, apply[COOP_ITEM_WORDS], acknowledged[COOP_ITEM_WORDS], session_hi, session_lo, scope;
} CoopItemResult;
typedef struct {
    CoopItemInput input;
    CoopItemResult result;
    unsigned locked_file, file_changed, applying, save_pending, counter_error, hud_pending, troff_pending;
    // Optional local quality-of-life behavior. A verified world flag may update
    // a reviewed loaded script directly or queue one same-map vanilla rebuild.
    unsigned refresh_enabled, refresh_pending, refresh_map;
    // A verified snapshot remains publishable while the player is in a shop,
    // boss, minigame or other unsafe overlay. Writes stay deferred until a
    // later safe capture proves the live save layout again.
    unsigned baseline, join, bound, deferred, previous[COOP_ITEM_WORDS];
} CoopItems;
static inline int coop_item_flag(unsigned id) {
    static const unsigned keys[8] = {0x1A, 0x4A, 0x8A, 0xA8, 0xEC, 0x124, 0x13D, 0x17C};
    if (id < 40) return 0x1D5 + id; // Blueprints, not Snide's separate GB rewards.
    if (id < 60) return 0x24D + id - 40;
    if (id < 70) return 0x261 + id - 60;
    if (id < 78) return keys[id - 70];
    if (id == 78) return 0x84;
    if (id == 79) return 0x17B;
    if (id < COOP_MEDAL_FIRST) return 0x1FD + id - COOP_SNIDE_FIRST;
    if (id < COOP_GB_FIRST) return 0x225 + id - COOP_MEDAL_FIRST;
    if (id < COOP_PICKUP_FIRST) return coop_golden_bananas[id - COOP_GB_FIRST].flag;
    if (id >= COOP_ACTOR_PICKUP_FIRST && id < COOP_PROGRESSION_FIRST) return coop_actor_pickups[id - COOP_ACTOR_PICKUP_FIRST].flag;
    return coop_progression_flag(id);
}
static inline unsigned coop_item_has(const unsigned* words, unsigned id) {
    return id < COOP_ITEMS && (words[id / 32] & (1u << (id % 32))) != 0;
}
static inline unsigned coop_items_dependencies(const unsigned* words) {
    for (unsigned id = COOP_ITEMS; id < COOP_ITEM_WORDS * 32; ++id)
        if (words[id / 32] & (1u << (id % 32))) return 0;
    for (unsigned i = 0; i < COOP_SNIDE_COUNT; ++i)
        if (coop_item_has(words, COOP_SNIDE_FIRST + i) && !coop_item_has(words, i)) return 0;
    // All of these dependencies are within one wire page. Key ownership spans
    // pages and is checked only after assembly by coop_items_full_dependencies.
    for (unsigned i = 0; i < 15; ++i)
        if (i % 3 && coop_item_has(words, COOP_MOVE_FIRST + i) && !coop_item_has(words, COOP_MOVE_FIRST + i - 1)) return 0;
    if (coop_item_has(words, COOP_SLAM_FIRST + 1) && !coop_item_has(words, COOP_SLAM_FIRST)) return 0;
    if (coop_item_has(words, COOP_BELT_FIRST + 1) && !coop_item_has(words, COOP_BELT_FIRST)) return 0;
    if (coop_item_has(words, COOP_MELON_FIRST + 1) && !coop_item_has(words, COOP_MELON_FIRST)) return 0;
    for (unsigned i = 0; i < 4; ++i)
        if (coop_item_has(words, COOP_TRAINING_SKILL_FIRST + i) && !coop_item_has(words, COOP_TRAINING_SPAWNED)) return 0;
    if (coop_item_has(words, COOP_TRAINING_COMPLETE) || coop_item_has(words, COOP_FIRST_SLAM)) {
        if (!coop_item_has(words, COOP_TRAINING_SPAWNED)) return 0;
        for (unsigned i = 0; i < 4; ++i) if (!coop_item_has(words, COOP_TRAINING_SKILL_FIRST + i)) return 0;
    }
    if (coop_item_has(words, COOP_KLUMSY_FREE)) {
        if (!coop_item_has(words, COOP_LOBBY)) return 0;
        for (unsigned i = 0; i < 8; ++i) if (!coop_item_has(words, COOP_KEY_TURN_FIRST + i)) return 0;
    }
    return 1;
}
static inline unsigned coop_items_full_dependencies(const unsigned* words) {
    if (!coop_items_dependencies(words)) return 0;
    // Feeding prefixes can cross packet pages. Only the complete snapshot can
    // prove that a claimed cumulative payment has every preceding milestone.
    for (unsigned bucket = 0; bucket < COOP_TROFF_BUCKETS; ++bucket) {
        unsigned first = COOP_TROFF_FIRST + bucket * COOP_TROFF_CAPACITY;
        unsigned gap = 0;
        for (unsigned amount = 0; amount < COOP_TROFF_CAPACITY; ++amount) {
            if (!coop_item_has(words, first + amount)) gap = 1;
            else if (gap) return 0;
        }
    }
    for (unsigned i = 0; i < 8; ++i)
        if (coop_item_has(words, COOP_KEY_TURN_FIRST + i)
                && (!coop_item_has(words, 70 + i) || !coop_item_has(words, COOP_LOBBY))) return 0;
    // Japes' hidden, taggable pad requires its GB reveal. That GB is on a
    // different wire page: check only complete ownership, never one page.
    for (unsigned i = 0; i < COOP_WARP_TAG_COUNT; ++i) {
        int required = coop_warp_tags[i].required_item;
        if (required >= 0 && coop_item_has(words, COOP_WARP_FIRST + i)
                && !coop_item_has(words, (unsigned)required)) return 0;
    }
    // Validate complete ownership, so future appended dependencies may cross
    // page boundaries. A cooled temple never invents the llama rescue.
    for (unsigned i = 0; i < COOP_WORLD_UNLOCK_COUNT; ++i) {
        int required = coop_world_unlocks[i].prerequisite;
        if (required >= 0 && coop_item_has(words, COOP_WORLD_FIRST + i)
                && !coop_item_has(words, COOP_WORLD_FIRST + (unsigned)required)) return 0;
        if (!coop_item_has(words, COOP_WORLD_FIRST + i)) continue;
        for (unsigned r = 0; r < COOP_WORLD_REQUIREMENT_COUNT; ++r) {
            const CoopWorldRequirement* requirement = &coop_world_requirements[r];
            if (requirement->flag != coop_world_unlocks[i].flag) continue;
            unsigned owned = 0;
            for (unsigned p = 0; p < requirement->count; ++p)
                owned += coop_item_has(words, requirement->first + p);
            if (owned < requirement->minimum) return 0;
        }
    }
    return 1;
}
static inline unsigned coop_snide_capacity(unsigned level, unsigned kong) {
    // Isles Tiny has the extra Rareware GB. All other vanilla buckets have five.
    return level == 7 && kong == 3 ? 6 : 5;
}
static inline int coop_item_id(int flag) {
    if (flag < 0) return -1;
    for (unsigned id = 0; id < COOP_PICKUP_FIRST; ++id) if (coop_item_flag(id) == flag) return id;
    for (unsigned id = COOP_ACTOR_PICKUP_FIRST; id < COOP_PROGRESSION_END; ++id) if (coop_item_flag(id) == flag) return id;
    return -1;
}
static inline unsigned coop_item_gb(unsigned id, unsigned* level, unsigned* kong) {
    if (id >= COOP_SNIDE_FIRST && id < COOP_MEDAL_FIRST) {
        *level = (id - COOP_SNIDE_FIRST) / 5; *kong = (id - COOP_SNIDE_FIRST) % 5; return 1;
    }
    if (id >= COOP_GB_FIRST && id < COOP_PICKUP_FIRST) {
        *level = coop_golden_bananas[id - COOP_GB_FIRST].level;
        *kong = coop_golden_bananas[id - COOP_GB_FIRST].kong; return 1;
    }
    return 0;
}
static inline unsigned coop_pickup_level(unsigned index) {
    unsigned level = 0;
    while (level < 7 && index >= coop_pickup_start[level + 1]) ++level;
    return level;
}
static inline void coop_items_note(CoopItems* g, unsigned id) {
    if (g->join && g->input.ready && g->input.scope && g->bound)
        g->input.request[id / 32] |= 1u << (id % 32);
}
static inline void coop_items_prepare(CoopItems* g, unsigned enabled, unsigned adventure, unsigned file) {
    if (enabled && adventure && file < 3) {
        if (!g->locked_file) g->locked_file = file + 1;
        else if (g->locked_file != file + 1) g->file_changed = 1;
    }
    g->input.enabled = g->file_changed ? 0 : enabled;
    g->input.file = g->locked_file;
    g->input.ready = g->input.enabled && !g->counter_error && adventure && file < 3 && g->locked_file == file + 1;
    if (!g->input.enabled) {
        for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) g->input.request[i] = 0;
        g->save_pending = 0;
        g->hud_pending = 0;
        g->refresh_pending = 0;
    }
}
static inline void coop_items_receive(CoopItems* g, CoopItemResult result) {
    unsigned changed = result.session_hi != g->input.session_hi || result.session_lo != g->input.session_lo
        || result.scope != g->input.scope;
    if (changed || result.status == 4) g->bound = 0;
    if (result.status == 2 || result.status == 3) g->bound = 1;
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i)
        g->input.request[i] = changed ? 0 : g->input.request[i] & ~result.acknowledged[i];
    g->input.session_hi = result.session_hi; g->input.session_lo = result.session_lo;
    g->input.scope = result.scope; g->result = result;
}
// Observe only new, local, permanent writes in a bound session. Do not suppress
// the pickup's own flag, save, cutscene, refill or other local side effects.
static inline void coop_items_observe(CoopItems* g, unsigned join, int flag, unsigned value,
        unsigned type, unsigned already_owned, unsigned adventure, unsigned file) {
    if (!join || !g->input.enabled || !g->input.ready || g->applying || !value || type || already_owned
            || !adventure || file >= 3 || g->locked_file != file + 1 || !g->input.scope
            || (g->result.status != 2 && g->result.status != 3)) return;
    int id = coop_item_id(flag);
    if (id >= 0) g->input.request[(unsigned)id / 32] |= 1u << ((unsigned)id % 32);
}
#endif
