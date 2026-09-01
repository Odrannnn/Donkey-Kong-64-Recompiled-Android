#ifndef COOP_PROGRESSION_IDS_H
#define COOP_PROGRESSION_IDS_H
#include "travel_ids.h"
#include "world_ids.h"
// Append-only IDs after v8's 2141 collectibles. Every value is locally defined.
enum {
    COOP_PROGRESSION_FIRST = 2141, COOP_KONG_FIRST = 2141,
    COOP_LOBBY = 2145, COOP_KEY_TURN_FIRST = 2146, COOP_KLUMSY_FREE = 2154,
    COOP_MOVE_FIRST = 2155, COOP_SLAM_FIRST = 2170, COOP_BELT_FIRST = 2172,
    COOP_GUN_FIRST = 2174, COOP_GUN_UPGRADE_FIRST = 2179,
    COOP_INSTRUMENT_FIRST = 2181, COOP_INSTRUMENT_UPGRADE_FIRST = 2186,
    COOP_MELON_FIRST = 2189, COOP_TRAINING_FIRST = 2191,
    COOP_TRAINING_SPAWNED = 2191, COOP_TRAINING_SKILL_FIRST = 2192,
    COOP_TRAINING_COMPLETE = 2196, COOP_FIRST_SLAM = 2197,
    COOP_CAMERA_SHOCKWAVE = 2198, COOP_WARP_FIRST = 2199,
    COOP_BLOCKER_FIRST = COOP_WARP_FIRST + COOP_WARP_TAG_COUNT,
    COOP_WORLD_FIRST = COOP_BLOCKER_FIRST + 8,
    COOP_PROGRESSION_END = COOP_WORLD_FIRST + COOP_WORLD_UNLOCK_COUNT
};
static inline int coop_progression_flag(unsigned id) {
    static const unsigned flags[14] = {0x6, 0x42, 0x46, 0x75, 0x1BB,
        0x1BC, 0x1BD, 0x1BE, 0x1BF, 0x1C0, 0x1C1, 0x1C2, 0x1C3, 0x315};
    static const unsigned training[8] = {0x17F, 0x182, 0x183, 0x184, 0x185, 0x187, 0x180, 0x179};
    if (id >= COOP_WORLD_FIRST && id < COOP_PROGRESSION_END) return coop_world_unlocks[id - COOP_WORLD_FIRST].flag;
    if (id >= COOP_BLOCKER_FIRST && id < COOP_WORLD_FIRST) return 0x1CD + id - COOP_BLOCKER_FIRST;
    if (id >= COOP_WARP_FIRST && id < COOP_BLOCKER_FIRST) return coop_warp_tags[id - COOP_WARP_FIRST].flag;
    if (id >= COOP_TRAINING_FIRST && id < COOP_WARP_FIRST) return training[id - COOP_TRAINING_FIRST];
    return id >= COOP_PROGRESSION_FIRST && id < COOP_MOVE_FIRST ? (int)flags[id - COOP_PROGRESSION_FIRST] : -1;
}
// Returns a Kong (5 means all five), inventory byte, value and scalar/bit mode.
static inline unsigned coop_progression_move(unsigned id, unsigned* kong, unsigned* field,
        unsigned* value, unsigned* bits) {
    *kong = 5; *bits = 0;
    if (id >= COOP_MOVE_FIRST && id < COOP_SLAM_FIRST) {
        *kong = (id - COOP_MOVE_FIRST) / 3; *field = 0; *value = 1 + (id - COOP_MOVE_FIRST) % 3;
    } else if (id >= COOP_SLAM_FIRST && id < COOP_BELT_FIRST) {
        *field = 1; *value = 2 + id - COOP_SLAM_FIRST;
    } else if (id >= COOP_BELT_FIRST && id < COOP_GUN_FIRST) {
        *field = 3; *value = 1 + id - COOP_BELT_FIRST;
    } else if (id >= COOP_GUN_FIRST && id < COOP_GUN_UPGRADE_FIRST) {
        *kong = id - COOP_GUN_FIRST; *field = 2; *value = 1; *bits = 1;
    } else if (id >= COOP_GUN_UPGRADE_FIRST && id < COOP_INSTRUMENT_FIRST) {
        *field = 2; *value = 2u << (id - COOP_GUN_UPGRADE_FIRST); *bits = 1;
    } else if (id >= COOP_INSTRUMENT_FIRST && id < COOP_INSTRUMENT_UPGRADE_FIRST) {
        *kong = id - COOP_INSTRUMENT_FIRST; *field = 4; *value = 1; *bits = 1;
    } else if (id >= COOP_INSTRUMENT_UPGRADE_FIRST && id < COOP_MELON_FIRST) {
        *field = 4; *value = 2u << (id - COOP_INSTRUMENT_UPGRADE_FIRST); *bits = 1;
    } else return 0;
    return 1;
}
#endif
