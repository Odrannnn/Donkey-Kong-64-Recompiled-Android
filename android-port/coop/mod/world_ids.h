#ifndef COOP_WORLD_IDS_H
#define COOP_WORLD_IDS_H
// Pinned vanilla permanent unlocks only; see TECHNICAL-NOTES.md for sources.
// Append-only order. Levels use getLevelIndex(map, 1), never network offsets.
// A prerequisite is another row in this table (-1 means none).
enum { COOP_WORLD_FACTORY_FIRST = 10, COOP_WORLD_JAPES_FIRST = 18,
    COOP_WORLD_FIRST_GATE = 25, COOP_WORLD_UNLOCK_COUNT = 100 };
// Level 255 is reserved for scripts/actors confined to excluded interiors.
// The caller's safe-map allowlist must hold before applying any row.
typedef struct { unsigned short flag; unsigned char level; signed char prerequisite; } CoopWorldUnlock;
static const CoopWorldUnlock coop_world_unlocks[COOP_WORLD_UNLOCK_COUNT] = {
    {0x032, 1, -1}, // Llama freed: Aztec actor spawns and exterior scripts.
    {0x04C, 1,  0}, // Llama Temple water cooled; requires the freed llama.
    {0x09E, 3, -1}, // Seal freed: Galleon and seal-race entrance.
    {0x0E6, 4, -1}, // Giant Mushroom cannon: coconut switch.
    {0x0E7, 4, -1}, // Grape switch.
    {0x0E8, 4, -1}, // Feather switch.
    {0x0E9, 4, -1}, // Peanut switch.
    {0x0EA, 4, -1}, // Pineapple switch.
    {0x0F8, 4, -1}, // Rabbit's first race complete; second race becomes available.
    {0x0FB, 4, -1}, // Beanstalk grown; no temporary carried-bean state is copied.
    // Factory: vanilla map 26 script writers/readers; see technical notes.
    // All affected main-map, Power Hut and Baboon Blast scripts belong to
    // level 2. Never deliver while that level is loaded. No machine phase,
    // temporary production switches, arcade payment or extra reward is shared.
    {0x06D, 2, -1}, // Entry hatch switch: object 0x17; hatch reads on reload.
    {0x06E, 2, -1}, // Testing-area gate: object 0x16, neutral switch 0xB0.
    {0x06F, 2, -1}, // Production core activated: object 0; Power Hut reads too.
    {0x077, 2, -1}, // Storage breakable metal grate: object 0x13C.
    {0x078, 2, -1}, // Storage question-mark box completed: object 0x62.
    {0x081, 2, -1}, // Arcade lever revealed: object 0x2D; no coin payment.
    {0x085, 2, -1}, // R&D breakable metal grate: object 0x3C.
    {0x097, 2, -1}, // Crown-pad grate opened: object 0x4C, not the crown award.
#include "world_expansion.inc"
};
// Requirements concern owned collectibles, never available currency balances.
// A threshold (crowns) can be satisfied by any members of its range.
typedef struct { unsigned short flag, first; unsigned char count, minimum; } CoopWorldRequirement;
enum { COOP_WORLD_REQUIREMENT_COUNT = 11 };
static const CoopWorldRequirement coop_world_requirements[COOP_WORLD_REQUIREMENT_COUNT] = {
    {0x02E, 70, 1, 1}, {0x06C, 71, 1, 1}, {0x098, 72, 1, 1},
    {0x0CB, 73, 1, 1}, {0x102, 74, 1, 1}, {0x12E, 75, 1, 1}, {0x160, 76, 1, 1},
    {0x189, 40, 20, 20}, {0x189, 2198, 1, 1}, // Fairies and camera/shockwave.
    {0x303, 78, 2, 2}, // Nintendo + Rareware coins; never spendable coins.
    {0x304, 60, 10, 4}, // Vanilla requires any four crowns, not all ten.
};
#endif
