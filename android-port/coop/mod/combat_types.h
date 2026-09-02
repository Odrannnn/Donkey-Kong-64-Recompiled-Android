#ifndef DKCOOP_COMBAT_TYPES_H
#define DKCOOP_COMBAT_TYPES_H
// Fixed 32-bit ABI words, also used by the native bridge. Network serialization
// remains explicit; no host pointers, actor types or arbitrary damage amounts cross it.
#define COOP_ENEMIES 20
#define COOP_ENEMY_KEYS 256
#define COOP_COMBAT_PAGES ((COOP_ENEMY_KEYS + COOP_ENEMIES - 1) / COOP_ENEMIES)
#define COOP_SHOTS 8
#define COOP_COMBAT_FRAME_WORDS (14 + COOP_ENEMIES * 9 + COOP_SHOTS * 7 + 4)
// Compact network form: identity + life pair + position + packed yaw/health.
// Page metadata is packed into the first wire word, so the four-word boss record,
// all following established core offsets remain unchanged.
#define COOP_COMBAT_WIRE_WORDS 204
#define COOP_COMBAT_RESULT_WORDS (12 + COOP_ENEMIES * 9 + COOP_SHOTS * 7 + COOP_ENEMIES * 9 + 4)
enum { COOP_ENEMY_ABSENT, COOP_ENEMY_ALIVE, COOP_ENEMY_DEFEATED, COOP_ENEMY_REQUEST };
enum { COOP_COMBAT_OFF, COOP_COMBAT_WAITING, COOP_COMBAT_SHOTS, COOP_COMBAT_READY, COOP_COMBAT_LAYOUT_MISMATCH };
enum { COOP_COMBAT_MOVEMENT = 1, COOP_COMBAT_POSE = 2 };
enum { COOP_COCONUT = 1, COOP_PEANUT, COOP_GRAPE, COOP_FEATHER, COOP_PINEAPPLE, COOP_ORANGE };
enum {
    COOP_BOSS_NONE,
    COOP_BOSS_ARMY_DILLO,
    COOP_BOSS_DOGADON,
    COOP_BOSS_MAD_JACK,
    COOP_BOSS_PUFFTOSS,
    COOP_BOSS_KING_KUT_OUT,
    COOP_BOSS_K_ROOL_DK,
    COOP_BOSS_K_ROOL_DIDDY,
    COOP_BOSS_K_ROOL_LANKY,
    COOP_BOSS_K_ROOL_TINY,
    COOP_BOSS_K_ROOL_CHUNKY,
    COOP_BOSS_KIND_COUNT = COOP_BOSS_K_ROOL_CHUNKY
};
enum {
    COOP_MAP_JAPES_DILLO = 8,
    COOP_MAP_FUNGI_DOGADON = 83,
    COOP_MAP_CAVES_DILLO = 196,
    COOP_MAP_AZTEC_DOGADON = 197,
    COOP_MAP_FACTORY_MAD_JACK = 154,
    COOP_MAP_GALLEON_PUFFTOSS = 111,
    COOP_MAP_CASTLE_KUT_OUT = 199,
    COOP_MAP_K_ROOL_DK = 203,
    COOP_MAP_K_ROOL_DIDDY = 204,
    COOP_MAP_K_ROOL_LANKY = 205,
    COOP_MAP_K_ROOL_TINY = 206,
    COOP_MAP_K_ROOL_CHUNKY = 207,
    COOP_MAP_K_ROOL_SHOE = 214
};
// Stable wire IDs; the local actor/handler table follows this order.
enum {
    COOP_BLUE_BEAVER = 1, COOP_GOLD_BEAVER, COOP_KREMLING,
    COOP_KLUMP, COOP_MUSHROOM_MAN, COOP_ROBO_KREMLING, COOP_KOSHA,
    COOP_ZINGER_0, COOP_ZINGER_1, COOP_ROBO_ZINGER, COOP_BAT,
    COOP_KASPLAT_DK, COOP_KASPLAT_DIDDY, COOP_KASPLAT_LANKY,
    COOP_KASPLAT_TINY, COOP_KASPLAT_CHUNKY,
    COOP_SHURI, COOP_GIMPFISH,
    COOP_KLAPTRAP_GREEN, COOP_KLAPTRAP_PURPLE, COOP_KLAPTRAP_RED,
    COOP_KROSSBONES,
    COOP_KABOOM, COOP_KLOBBER, COOP_PUFFTUP, COOP_KRITTER_IN_A_SHEET,
    COOP_MR_DICE_0, COOP_SIR_DOMINO, COOP_MR_DICE_1,
    COOP_SPIDERLING,
    COOP_FIREBALL_WITH_GLASSES,
    COOP_RULER,
    COOP_BOOK,
    COOP_TOY_MONSTER,
    COOP_ENEMY_KIND_COUNT = COOP_TOY_MONSTER
};
typedef struct { unsigned key, life, state, peer_life, kind, x, y, z, yaw; } CoopEnemy;
typedef struct { unsigned id, kind, x, y, z, yaw, scale; } CoopShot;
typedef struct { unsigned kind, life, peer_life, phase; } CoopBoss;
typedef struct { unsigned kind, life, x, y, z, yaw, pose, clip_hash; } CoopBossMotion;
typedef struct {
    unsigned enabled, file, layout, hands;
    CoopEnemy enemies[COOP_ENEMIES];
    CoopShot shots[COOP_SHOTS];
    CoopBoss boss;
    unsigned page, pages; // Game/native ABI only; packed into wire feature word.
    CoopBossMotion boss_motion; // ABI only; encoded in enemy wire slot zero on boss maps.
} CoopCombatFrame;
typedef struct {
    unsigned status, paired, hands, movement;
    CoopEnemy apply[COOP_ENEMIES];
    CoopShot shots[COOP_SHOTS];
    CoopEnemy motion[COOP_ENEMIES]; // Guest-only commands, with the guest's local life token.
    CoopBoss boss; // One-step game-thread boss phase target with the receiver's life token.
    CoopBossMotion boss_motion; // Guest-only host position/facing and local-clip pose correction.
} CoopCombatResult;

// Enemy facing needs 12 bits. Alive records use the next 15 bits for positive
// s16 health and the high five bits for a normalized visual-pose sample.
#define COOP_ENEMY_YAW_MASK 0xFFFu
#define COOP_ENEMY_HEALTH_SHIFT 12u
#define COOP_ENEMY_HEALTH_MASK 0x7FFFu
#define COOP_ENEMY_POSE_SHIFT 27u
#define COOP_ENEMY_POSE_MASK 0x1Fu
#define COOP_ENEMY_PACK_MASK (COOP_ENEMY_YAW_MASK | (COOP_ENEMY_HEALTH_MASK << COOP_ENEMY_HEALTH_SHIFT) | (COOP_ENEMY_POSE_MASK << COOP_ENEMY_POSE_SHIFT))
#define COOP_ENEMY_LIFE_MASK 0x03FFFFFFu
#define COOP_ENEMY_POSE_HASH_SHIFT 26u
#define COOP_ENEMY_POSE_HASH_MASK 0x3Fu
#define COOP_ENEMY_POSE_HASH_PACK_MASK (COOP_ENEMY_POSE_HASH_MASK << COOP_ENEMY_POSE_HASH_SHIFT)
#define COOP_ENEMY_KEY_MASK 0x1FFu
#define COOP_ENEMY_STATE_SHIFT 9u
#define COOP_ENEMY_KIND_SHIFT 11u
#define COOP_ENEMY_KIND_MASK 0x3Fu
#define COOP_ENEMY_IDENTITY_MASK 0x1FFFFu
static inline unsigned coop_enemy_pack(unsigned yaw, unsigned health) {
    return (yaw & COOP_ENEMY_YAW_MASK) | ((health & COOP_ENEMY_HEALTH_MASK) << COOP_ENEMY_HEALTH_SHIFT);
}
static inline unsigned coop_enemy_pack_pose(unsigned yaw, unsigned health, unsigned pose) {
    return coop_enemy_pack(yaw, health) | ((pose & COOP_ENEMY_POSE_MASK) << COOP_ENEMY_POSE_SHIFT);
}
static inline unsigned coop_enemy_yaw(CoopEnemy enemy) { return enemy.yaw & COOP_ENEMY_YAW_MASK; }
static inline unsigned coop_enemy_health(CoopEnemy enemy) {
    return (enemy.yaw >> COOP_ENEMY_HEALTH_SHIFT) & COOP_ENEMY_HEALTH_MASK;
}
static inline unsigned coop_enemy_pose(CoopEnemy enemy) {
    return (enemy.yaw >> COOP_ENEMY_POSE_SHIFT) & COOP_ENEMY_POSE_MASK;
}
static inline unsigned coop_enemy_peer_life(CoopEnemy enemy) { return enemy.peer_life & COOP_ENEMY_LIFE_MASK; }
static inline unsigned coop_enemy_pose_hash(CoopEnemy enemy) {
    return (enemy.peer_life >> COOP_ENEMY_POSE_HASH_SHIFT) & COOP_ENEMY_POSE_HASH_MASK;
}
static inline unsigned coop_enemy_clip_hash(unsigned clip) {
    return (clip ^ (clip >> 6) ^ (clip >> 12)) & COOP_ENEMY_POSE_HASH_MASK;
}

static inline unsigned coop_boss_kind(unsigned map) {
    if (map == COOP_MAP_JAPES_DILLO || map == COOP_MAP_CAVES_DILLO)
        return COOP_BOSS_ARMY_DILLO;
    if (map == COOP_MAP_FUNGI_DOGADON || map == COOP_MAP_AZTEC_DOGADON)
        return COOP_BOSS_DOGADON;
    if (map == COOP_MAP_FACTORY_MAD_JACK) return COOP_BOSS_MAD_JACK;
    if (map == COOP_MAP_GALLEON_PUFFTOSS) return COOP_BOSS_PUFFTOSS;
    if (map == COOP_MAP_CASTLE_KUT_OUT) return COOP_BOSS_KING_KUT_OUT;
    if (map == COOP_MAP_K_ROOL_DK) return COOP_BOSS_K_ROOL_DK;
    if (map == COOP_MAP_K_ROOL_DIDDY) return COOP_BOSS_K_ROOL_DIDDY;
    if (map == COOP_MAP_K_ROOL_LANKY) return COOP_BOSS_K_ROOL_LANKY;
    // Tiny's actual damage and temporary toe flag are committed by the foot
    // actor in the shoe map. Her arena reads those flags on return.
    if (map == COOP_MAP_K_ROOL_SHOE) return COOP_BOSS_K_ROOL_TINY;
    if (map == COOP_MAP_K_ROOL_CHUNKY) return COOP_BOSS_K_ROOL_CHUNKY;
    return COOP_BOSS_NONE;
}
static inline unsigned coop_boss_map(unsigned map) { return coop_boss_kind(map) != COOP_BOSS_NONE; }
static inline unsigned coop_enemy_pose_encode(float frame, unsigned count) {
    if (!(frame >= 0.0f && frame <= 254.0f) || !count || count > 255) return 0;
    if (count == 1) return 1;
    float last = (float)(count - 1);
    if (frame > last) frame = last;
    unsigned step = (unsigned)((frame * 30.0f / last) + 0.5f);
    return 1 + (step > 30 ? 30 : step);
}
static inline float coop_enemy_pose_frame(unsigned pose, unsigned count) {
    if (!pose || pose > COOP_ENEMY_POSE_MASK || !count || count > 255) return 0.0f;
    if (count == 1) return 0.0f;
    return (float)(pose - 1) * (float)(count - 1) / 30.0f;
}

static inline unsigned coop_combat_map(unsigned map) {
    // Reviewed vanilla maps containing at least one supported ordinary enemy.
    // Bosses, bonus games, races, battle arenas and reward controllers remain excluded.
    switch (map) {
        case 4: case 7: case 12: case 13:                         // Japes
        case 16: case 19: case 20: case 21: case 22: case 23: case 24: case 38: // Aztec
        case 26:                                                  // Factory
        case 30: case 43: case 46: case 47: case 49: case 179:   // Galleon
        case 48: case 52: case 57: case 58: case 59: case 60:
        case 61: case 62: case 64: case 70: case 71:             // Fungi
        case 72: case 84: case 85: case 86: case 91: case 92:
        case 93: case 94: case 95: case 200:                     // Caves
        case 87: case 88: case 105: case 108: case 112: case 113:
        case 114: case 151: case 163: case 164: case 166: case 183: // Castle
        case 17: case 34:                                        // Helm and Isles
        case 169: case 173: case 175: case 193:                  // reviewed lobbies
            return 1;
        default:
            return 0;
    }
}

static inline unsigned coop_combat_apply_matches(CoopEnemy command, unsigned key, unsigned life,
        unsigned initialized, unsigned alive, unsigned supported, unsigned context_ready) {
    return context_ready && initialized && alive && supported && key && life
        && command.key == key && command.life == life && command.state == COOP_ENEMY_DEFEATED
        && !command.x && !command.y && !command.z && !command.yaw;
}
static inline unsigned coop_combat_health_matches(CoopEnemy command, unsigned key, unsigned life,
        unsigned initialized, unsigned alive, unsigned supported, unsigned context_ready,
        unsigned current_health) {
    unsigned target = coop_enemy_health(command);
    return context_ready && initialized && alive && supported && key && life
        && command.key == key && command.life == life && command.state == COOP_ENEMY_ALIVE
        && !command.x && !command.y && !command.z
        && !(command.yaw & ~COOP_ENEMY_PACK_MASK) && !coop_enemy_yaw(command) && !coop_enemy_pose(command)
        && target && target < current_health;
}
#endif
