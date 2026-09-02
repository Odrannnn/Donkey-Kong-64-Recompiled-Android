#include "common_structs.h"
#include "modding.h"
#include "recompconfig.h"
#include "recomputils.h"
#include "gate_policy.h"
#include "animation_actor.h"
#include "combat_types.h"
#include "items_policy.h"
#include "inventory_types.h"
#include "world_types.h"
#include "transient_types.h"
#include "transition_policy.h"
#include "trace_types.h"
#include "lifecycle_policy.h"
#include "recovery_policy.h"
#include "recovery_storage.h"

typedef struct { CoopGateInput gate; CoopCombatFrame combat; CoopItemInput items; CoopWorldInput world; CoopTransientInput transient; CoopTraceInput trace; } CoopExtraInput;
typedef struct { CoopGateResult gate; CoopCombatResult combat; CoopItemResult items; CoopWorldResult world; CoopTransientResult transient; } CoopExtraResult;
_Static_assert(sizeof(CoopExtraInput) == 2868 && sizeof(CoopExtraResult) == 3476, "v55 bridge ABI");
_Static_assert(sizeof(CoopCharacterProgress) == 0x5E && __builtin_offsetof(CoopCharacterProgress, golden_bananas) == 0x42
    && __builtin_offsetof(CoopCharacterProgress, coins) == 0x6
    && __builtin_offsetof(CoopCharacterProgress, coloured_bananas) == 0xA
    && __builtin_offsetof(CoopCharacterProgress, coloured_bananas_fed_to_tns) == 0x26
    && __builtin_offsetof(CoopPlayerProgress, melons) == 0x2FC
    && sizeof(CoopPlayerProgress) == 0x306, "Pinned inventory layout");
_Static_assert(PERMFLAG_PROGRESS_GIVEN_FIRST_SLAM == 0x180 && PERMFLAG_PROGRESS_JAPES_LOBBY_OPEN == 0x1BB
    && PERMFLAG_PROGRESS_KEY_1_TURNED == 0x1BC && PERMFLAG_PROGRESS_KEY_8_TURNED == 0x1C3
    && PERMFLAG_PROGRESS_K_LUMSY_FREE == 0x315, "Progression flag guards");
_Static_assert(PERMFLAG_KONG_DIDDY == 0x6 && PERMFLAG_KONG_TINY == 0x42
    && PERMFLAG_KONG_LANKY == 0x46 && PERMFLAG_KONG_CHUNKY == 0x75, "Kong rescue flags");
_Static_assert(MAP_DK_HOUSE == 171 && MAP_TRAINING_GROUNDS == 176 && MAP_FAIRY_ISLAND == 189
    && PERMFLAG_PROGRESS_TRAINING_SPAWNED == 0x17F && PERMFLAG_ITEM_MOVE_DIVING == 0x182
    && PERMFLAG_ITEM_MOVE_VINES == 0x183 && PERMFLAG_ITEM_MOVE_ORANGETHROWING == 0x184
    && PERMFLAG_ITEM_MOVE_BARRELTHROWING == 0x185 && PERMFLAG_PROGRESS_ALL_TRAINING_COMPLETE == 0x187
    && PERMFLAG_ITEM_MOVE_SHOCKWAVE_CAMERA == 0x179, "Training/camera flags and safe room");
_Static_assert(PERMFLAG_ITEM_GB_JAPES_BLUEPRINT_DK == 0x1FD && PERMFLAG_ITEM_GB_ISLES_BLUEPRINT_CHUNKY == 0x224
    && PERMFLAG_ITEM_MEDAL_JAPES_DK == 0x225 && PERMFLAG_ITEM_MEDAL_HELM_DIDDY == 0x24C, "Snide/medal allowlist");
_Static_assert(PERMFLAG_ITEM_BLUEPRINT_JAPES_DK == 0x1D5 && PERMFLAG_ITEM_FAIRY_JAPES_POOL == 0x24D
    && PERMFLAG_ITEM_CROWN_JAPES == 0x261 && PERMFLAG_ITEM_NINTENDO_COIN == 0x84
    && PERMFLAG_ITEM_RAREWARE_COIN == 0x17B, "Unique item flag allowlist");
_Static_assert(PERMFLAG_ITEM_BLUEPRINT_ISLES_CHUNKY == 0x1FC && PERMFLAG_ITEM_CROWN_HELM == 0x26A
    && PERMFLAG_ITEM_KEY_1 == 0x1A && PERMFLAG_ITEM_KEY_2 == 0x4A && PERMFLAG_ITEM_KEY_3 == 0x8A
    && PERMFLAG_ITEM_KEY_4 == 0xA8 && PERMFLAG_ITEM_KEY_5 == 0xEC && PERMFLAG_ITEM_KEY_6 == 0x124
    && PERMFLAG_ITEM_KEY_7 == 0x13D && PERMFLAG_ITEM_KEY_8 == 0x17C
    && PERMFLAG_PROGRESS_ARCADE_2_COINS_PAID == 0x83, "Unique item flag endpoints/keys/payment");
_Static_assert(MAP_JAPES == 7 && MAP_FACTORY == 26 && MAP_GALLEON == 30 && MAP_AZTEC == 38
    && MAP_FUNGI == 48 && MAP_CAVES == 72 && MAP_CASTLE == 87, "Main-map allowlist");
_Static_assert(MAP_JAPES_MOUNTAIN == 4 && MAP_JAPES_SHELL == 12 && MAP_JAPES_LANKY_CAVE == 13
    && MAP_AZTEC_TINY_TEMPLE == 16 && MAP_AZTEC_FIVE_DOOR_TEMPLE_DK == 19
    && MAP_AZTEC_LLAMA_TEMPLE == 20 && MAP_AZTEC_FIVE_DOOR_TEMPLE_DIDDY == 21
    && MAP_AZTEC_FIVE_DOOR_TEMPLE_TINY == 22 && MAP_AZTEC_FIVE_DOOR_TEMPLE_LANKY == 23
    && MAP_AZTEC_FIVE_DOOR_TEMPLE_CHUNKY == 24, "Japes/Aztec combat interiors");
_Static_assert(MAP_GALLEON_SHIPWRECK_DIDDY_LANKY_CHUNKY == 43 && MAP_GALLEON_SHIPWRECK_DK_TINY == 46
    && MAP_GALLEON_SHIPWRECK_LANKY_TINY == 47 && MAP_GALLEON_LIGHTHOUSE == 49
    && MAP_GALLEON_SUBMARINE == 179 && MAP_FUNGI_ANT_HILL == 52 && MAP_FUNGI_DIDDY_ATTIC == 57
    && MAP_FUNGI_LANKY_ATTIC == 58 && MAP_FUNGI_DK_BARN == 59 && MAP_FUNGI_SPIDER == 60
    && MAP_FUNGI_FRONT_PART_OF_MILL == 61 && MAP_FUNGI_REAR_PART_OF_MILL == 62
    && MAP_FUNGI_GIANT_MUSHROOM == 64 && MAP_FUNGI_MUSHROOM_LEAP == 70
    && MAP_FUNGI_SHOOTING_GAME == 71, "Galleon/Fungi combat interiors");
_Static_assert(MAP_CAVES_IGLOO_TINY == 84 && MAP_CAVES_IGLOO_LANKY == 85 && MAP_CAVES_IGLOO_DK == 86
    && MAP_CAVES_SHACK_DK == 91 && MAP_CAVES_SHACK_DIDDY_MIDDLE_PART == 92
    && MAP_CAVES_SHACK_TINY == 93 && MAP_CAVES_LANKY_HUT == 94 && MAP_CAVES_IGLOO_CHUNKY == 95
    && MAP_CAVES_SHACK_DIDDY_UPPER_PART == 200 && MAP_CASTLE_BALLROOM == 88
    && MAP_CASTLE_TOWER == 105 && MAP_CASTLE_CRYPT_LANKY_TINY == 108
    && MAP_CASTLE_CRYPT_DK_DIDDY_CHUNKY == 112 && MAP_CASTLE_MUSEUM == 113
    && MAP_CASTLE_LIBRARY == 114 && MAP_CASTLE_DUNGEON == 151 && MAP_CASTLE_BASEMENT == 163
    && MAP_CASTLE_TREE == 164 && MAP_CASTLE_CHUNKY_TOOLSHED == 166 && MAP_CASTLE_CRYPT == 183,
    "Caves/Castle combat interiors");
_Static_assert(MAP_HELM == 17 && MAP_DK_ISLES_OVERWORLD == 34 && MAP_JAPES_LOBBY == 169
    && MAP_AZTEC_LOBBY == 173 && MAP_GALLEON_LOBBY == 174 && MAP_FACTORY_LOBBY == 175
    && MAP_FUNGI_LOBBY == 178 && MAP_CASTLE_LOBBY == 193 && MAP_CAVES_LOBBY == 194,
    "Helm/Isles/lobby combat maps");
_Static_assert(PERMFLAG_B_LOCKER_CLEARED_JAPES == 0x1CD && PERMFLAG_B_LOCKER_CLEARED_HELM == 0x1D4
    && COOP_WARP_TAG_COUNT == 87 && COOP_BLOCKER_FIRST == 2286 && COOP_WORLD_FIRST == 2294,
    "Vanilla travel unlock contract");
_Static_assert(PERMFLAG_PROGRESS_LLAMA_FREE == 0x32 && PERMFLAG_PROGRESS_LLAMA_TEMPLE_WATER_COOLED == 0x4C
    && PERMFLAG_PROGRESS_SEAL_FREED == 0x9E && PERMFLAG_PROGRESS_CANNON_COCONUT_SWITCH == 0xE6
    && PERMFLAG_PROGRESS_CANNON_GRAPE_SWITCH == 0xE7 && PERMFLAG_PROGRESS_CANNON_FEATHER_SWITCH == 0xE8
    && PERMFLAG_PROGRESS_CANNON_PEANUT_SWITCH == 0xE9 && PERMFLAG_PROGRESS_CANNON_PINEAPPLE_SWITCH == 0xEA
    && PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE == 0xF8 && PERMFLAG_PROGRESS_BEANSTALK_GROWN == 0xFB
    && COOP_WORLD_FACTORY_FIRST == 10 && COOP_WORLD_FIRST + COOP_WORLD_FACTORY_FIRST == 2304
    && COOP_WORLD_JAPES_FIRST == 18 && COOP_WORLD_FIRST + COOP_WORLD_JAPES_FIRST == 2312
    && COOP_WORLD_FIRST_GATE == 25 && COOP_WORLD_UNLOCK_COUNT == 100 && COOP_PROGRESSION_END == 2394,
    "Vanilla world unlock contract");
_Static_assert(PERMFLAG_PROGRESS_JAPES_FIRST_GATE_OPENED == 0
    && PERMFLAG_ITEM_PEARL_1 == 0xBA && PERMFLAG_ITEM_PEARL_5 == 0xBE
    && PERMFLAG_PROGRESS_RAREWARE_ROOM_OPEN == 0x189 && COOP_CAMERA_SHOCKWAVE == 2198
    && PERMFLAG_PROGRESS_HELM_SHUTDOWN == 0x302 && PERMFLAG_PROGRESS_CROWN_DOOR_OPENED == 0x304
    && COOP_WORLD_TRAINING_EXIT == 2375
    && COOP_WORLD_UNLOCK_COUNT <= 128 && COOP_WORLD_REQUIREMENT_COUNT == 11,
    "Expanded vanilla flags and bounded prerequisite rows");

_Static_assert(ACTOR_BEAVER_BLUE == 178 && ACTOR_BEAVER_GOLD == 212 && ACTOR_KREMLING == 238
    && ACTOR_KLUMP == 187 && ACTOR_MUSHROOM_MAN == 224 && ACTOR_ROBO_KREMLING == 235
    && ACTOR_KOSHA == 291 && ACTOR_ZINGER_0 == 183 && ACTOR_ZINGER_1 == 206
    && ACTOR_ROBO_ZINGER == 261 && ACTOR_BAT == 285
    && ACTOR_KASPLAT_DK == 241 && ACTOR_KASPLAT_DIDDY == 242 && ACTOR_KASPLAT_LANKY == 243
    && ACTOR_KASPLAT_TINY == 244 && ACTOR_KASPLAT_CHUNKY == 245
    && ACTOR_SHURI == 267 && ACTOR_GIMPFISH == 268
    && ACTOR_KLAPTRAP_GREEN == 205 && ACTOR_KLAPTRAP_PURPLE == 208 && ACTOR_KLAPTRAP_RED == 209
    && ACTOR_KROSSBONES == 262 && ACTOR_KABOOM == 175 && ACTOR_KLOBBER == 182
    && ACTOR_PUFFTUP_0 == 290 && ACTOR_KRITTER_IN_A_SHEET == 289
    && ACTOR_MR_DICE_0 == 269 && ACTOR_SIR_DOMINO == 270 && ACTOR_MR_DICE_1 == 271
    && ACTOR_SPIDERLING == 276 && ACTOR_FIREBALL_WITH_GLASSES == 273 && ACTOR_RULER == 230
    && ACTOR_BOOK == 181 && ACTOR_TOY_MONSTER == 228 && ACTOR_CLAM == 286
    && ACTOR_BOSS_KUTOUT_TAG == 165 && ACTOR_BOSS_ARMY_DILLO == 185
    && ACTOR_BOSS_MAD_JACK == 204 && ACTOR_BOSS_PUFFTOSS == 216
    && ACTOR_BOSS_KROOL_FOOT == 227 && ACTOR_BOSS_DOGADON == 236
    && ACTOR_BOSS_KROOL_DK == 281 && ACTOR_BOSS_KROOL_DIDDY == 292
    && ACTOR_BOSS_KROOL_LANKY == 293 && ACTOR_BOSS_KROOL_TINY == 294
    && ACTOR_BOSS_KROOL_CHUNKY == 295
    && MAP_FUNGI_SPIDER == 60 && COOP_ENEMY_KIND_COUNT == 34,
    "Pinned enemy/boss actor IDs and enemy wire kind count");
_Static_assert(COOP_MAP_JAPES_DILLO == 8 && COOP_MAP_FUNGI_DOGADON == 83
    && COOP_MAP_GALLEON_PUFFTOSS == 111 && COOP_MAP_FACTORY_MAD_JACK == 154
    && COOP_MAP_CAVES_DILLO == 196 && COOP_MAP_AZTEC_DOGADON == 197
    && COOP_MAP_CASTLE_KUT_OUT == 199 && COOP_MAP_K_ROOL_DK == 203
    && COOP_MAP_K_ROOL_DIDDY == 204 && COOP_MAP_K_ROOL_LANKY == 205
    && COOP_MAP_K_ROOL_TINY == 206 && COOP_MAP_K_ROOL_CHUNKY == 207
    && COOP_MAP_K_ROOL_SHOE == 214 && COOP_BOSS_KIND_COUNT == 10,
    "Pinned boss map IDs and wire kind count");

_Static_assert(COOP_TROFF_FIRST == 2394 && COOP_TROFF_END == 5894 && COOP_JAPES_BOULDER_BUNCH == 5894
    && COOP_KROOL_DEFEATED == 5895 && COOP_ARCADE_COINS_PAID == 5896 && COOP_ITEMS == 5897
    && COOP_FACTORY_ARCADE_LEVER == 2309
    && COOP_TROFF_LEVELS == 7 && COOP_TROFF_KONGS == 5 && COOP_TROFF_CAPACITY == 100
    && COOP_ITEM_WORDS * 32 >= COOP_ITEMS, "Cumulative Troff feeding contract");

RECOMP_IMPORT(".", u32 dk64_coop_start(u32 role, const char* ip, u32 port, u32 room));
RECOMP_IMPORT(".", u32 dk64_coop_local_ipv4(void));
RECOMP_IMPORT(".", u32 dk64_coop_recovery_configure_v55(const char* save_path, u32 profile, u32 save_kind, u32 room));
RECOMP_IMPORT(".", u32 dk64_coop_recovery_status_v55(void));
RECOMP_IMPORT(".", u32 dk64_coop_recovery_promote_v55(void));
RECOMP_IMPORT(".", u32 dk64_coop_tick_v55(const u32* local, u32* remote, const CoopExtraInput* input, CoopExtraResult* result));
RECOMP_IMPORT(".", void dk64_coop_stop(void));

extern Actor *gPlayerPointer, *gCurrentActorPointer, *gLastSpawnedActor;
extern Maps current_map;
extern s32 current_exit;
extern Maps next_map;
extern s32 next_exit;
extern f32 loading_zone_transition_speed;
extern void func_global_asm_805FF378(Maps nextMap, s32 nextExit);
extern u8 game_mode;
extern u8 current_file;
extern u8 isFlagSet(s16 flagIndex, u8 flagType);
extern void setFlag(s16 flagIndex, u8 newValue, u8 flagType);
extern void func_global_asm_8060DEC8(void);
extern void func_global_asm_806F8278(u8 player);
extern CoopPlayerProgress D_global_asm_807FC950[4];
extern u16 D_global_asm_807FC930[]; // Troff & Scoff per-level fed-total cache.
extern void* D_global_asm_80754280; // Only checked for null here; the game's HUD routine owns its layout.
extern void* D_global_asm_807FD730; // Pending collectible credits: never grant/capture mid-queue.
extern u8 func_global_asm_8060E3B0(u16 ordinal, u8 level);
extern void func_global_asm_8060E430(u16 ordinal, u8 value, u8 level);
extern s16 func_global_asm_80631C20(u8 level);
extern s16 D_global_asm_807F6240[];
extern Prop* D_global_asm_807F6000;
extern s16 func_global_asm_80659470(s32 object);
extern void func_global_asm_8063DA40(s16 script_slot, s16 state);
extern void playActorAnimation(Actor* actor, s32 animation);
extern void func_global_asm_80614D00(Actor* actor, f32 speed, f32 start);
extern s32 playCutscene(Actor* actor, s16 cutscene, u8 mode);
extern u8 getLevelIndex(Maps map, u8 include_lobbies);
extern s8 is_cutscene_active;
extern s16 D_global_asm_807476F8;
extern u16 D_global_asm_807F5CF0;
extern u16 D_global_asm_807F5CF4;
extern u8 current_character_index[];
extern void (*D_global_asm_8074C0A0[])(void);
extern u16 D_global_asm_8074E814[];
extern s32* D_global_asm_807FBB50;
extern u16* D_global_asm_807FBB58;
extern u16 D_global_asm_807FBB34;
extern u8 D_global_asm_807467E0; // Set when the game's EEPROM worker starts reading save slots.
extern struct { Actor* actor; u32 metadata; } D_global_asm_807FB930[];
extern Gfx D_1000118[];
extern Mtx D_2000180, D_20000C0;
extern s32 spawnActor(Actors actor_index, s32 model_index);
extern s32 deleteActor(Actor* actor);
extern void renderActor(Actor* actor, u8 mode);
extern void func_global_asm_8067C2B8(void);
extern void addActorToTextOverlayRenderArray(void* callback, Actor* actor, u8 layer);
extern Gfx* printStyledText(Gfx* dl, s16 font, s16 x, s16 y, u8* text, u32 flags);

enum { ROLE_OFF, ROLE_HOST, ROLE_JOIN };
enum { NET_OFF, NET_LISTENING, NET_CONNECTING, NET_CONNECTED, NET_ERROR, NET_BUSY };
enum { STATE_MAP, STATE_EPOCH, STATE_KONG, STATE_FLAGS, STATE_X, STATE_Y, STATE_Z, STATE_YAW,
    STATE_ANIM, STATE_FRAME, STATE_TRANSITION_TICKET, STATE_TRANSITION_ROUTE, STATE_WORDS };
enum { STATE_ACTIVE = 1, STATE_CUTSCENE = 2 };
static u32 role, configured_role, status, epoch = 1, boot_initialized;
static u32 map_load_serial, eeprom_load_started;
static CoopMapLifecycle map_lifecycle = {-1, 0};
static u32 save_profile, save_copy, using_host_save;
static u32 shared_items;
static u32 merge_guest_progress;
static u32 recovery_state, promoted_host, recovery_storage_status, recovery_save_issued;
static CoopHostRecovery host_recovery;
static u32 recovery_world_change[COOP_WORLD_TOGGLES];
static u32 automatic_world_refresh;
static u32 follow_host_transitions;
static u32 transient_enabled, transient_revision = 1, transient_page;
static u32 transient_file, transient_file_changed;
static CoopTransientInput transient_input;
static CoopTransientResult transient_result;
static CoopItems items;
static u32 combat_enabled;
static CoopCombatFrame combat_input;
static CoopCombatResult combat_result;
static u32 local_ipv4;
static u32 local_state[STATE_WORDS], remote_state[STATE_WORDS];
static Actor* remote_actor;
static Actor* retiring_actor;
static u32 remote_generation, retiring_generation;
static u32 remote_epoch, remote_kong;
static CoopPose remote_pose;
static CoopTransitionCapture transition_capture;
static CoopTransitionFollow transition_follow;
static u8 avatar_available;
static void (*original_box_behavior)(void);
static Gfx* draw_coop_status(Gfx* dl, Actor* unused);

static u32 float_bits(f32 value) { union { f32 f; u32 u; } bits; bits.f = value; return bits.u; }
static f32 bits_float(u32 value) { union { f32 f; u32 u; } bits; bits.u = value; return bits.f; }

#include "world_live_game.h"
#include "items_game.h"
#include "world_game.h"
#include "transient_game.h"
#include "single_slot_menu.h"
static CoopWorld world;
_Static_assert(PERMFLAG_PROGRESS_IS_GALLEON_WATER_RAISED == 0xA0
    && PERMFLAG_PROGRESS_IS_NIGHTTIME == 0xCE && MAP_CAVES_LOBBY == 194
    && COOP_WORLD_TOGGLES == 3, "Reversible world flag contract");

static unsigned animation_rows(void) {
    if (!D_global_asm_807FBB50 || !D_global_asm_807FBB58) return 0;
    unsigned rows = coop_animation_rows(D_global_asm_807FBB50[2], D_global_asm_807FBB50[3]);
    if (!rows || (u8*)D_global_asm_807FBB58 != (u8*)D_global_asm_807FBB50 + D_global_asm_807FBB50[2]) return 0;
    return rows;
}

static void capture_pose(unsigned kong, unsigned rows) {
    // Animal transformations and replacement actor types do not use Kong poses.
    if ((unsigned)gPlayerPointer->unk58 != kong + 2) return;
    AnimationStateUnk0* track = gPlayerPointer->animation_state->unk0;
    if (!track || !track->unk0 || !track->unk0->unk12 || !coop_animation_valid_frame(track->unk4)) return;
    unsigned pose = coop_animation_find(D_global_asm_807FBB58, rows, kong, track->unk10);
    if (!pose) return;
    local_state[STATE_ANIM] = pose;
    local_state[STATE_FRAME] = float_bits(coop_animation_frame(track->unk4, track->unk0->unk12));
}

static const char* const item_host_saves[] = {
    "items_host_v6", "items_host_v6_campaign2", "items_host_v6_campaign3", "items_host_v6_campaign4",
    "items_host_v6_campaign5", "items_host_v6_campaign6", "items_host_v6_campaign7", "items_host_v6_campaign8"
};
static const char* const item_guest_saves[] = {
    "items_guest_v6", "items_guest_v6_campaign2", "items_guest_v6_campaign3", "items_guest_v6_campaign4",
    "items_guest_v6_campaign5", "items_guest_v6_campaign6", "items_guest_v6_campaign7", "items_guest_v6_campaign8"
};
static const char* const presence_host_saves[] = {
    "prototype_host_v1", "prototype_host_v1_campaign2", "prototype_host_v1_campaign3", "prototype_host_v1_campaign4",
    "prototype_host_v1_campaign5", "prototype_host_v1_campaign6", "prototype_host_v1_campaign7", "prototype_host_v1_campaign8"
};
static const char* const presence_guest_saves[] = {
    "prototype_guest_v1", "prototype_guest_v1_campaign2", "prototype_guest_v1_campaign3", "prototype_guest_v1_campaign4",
    "prototype_guest_v1_campaign5", "prototype_guest_v1_campaign6", "prototype_guest_v1_campaign7", "prototype_guest_v1_campaign8"
};

// recomp_on_init fires before the first map loads and starts the game's EEPROM worker.
// Fail closed if another mod has already started that worker; never switch cached saves.
static void coop_select_save(void) {
    if (boot_initialized) return;
    boot_initialized = 1;
    role = recomp_get_config_u32("role");
    if (role != ROLE_HOST && role != ROLE_JOIN) { role = ROLE_OFF; return; }
    configured_role = role;
    if (eeprom_load_started || D_global_asm_807467E0 != 0) {
        role = ROLE_OFF; status = NET_ERROR;
        recomp_printf("[dk64-coop] Save worker already started; co-op disabled without changing saves.\n");
        return;
    }
    shared_items = recomp_get_config_u32("shared_items") == 1;
    save_profile = recomp_get_config_u32("save_profile");
    if (save_profile >= 8) save_profile = 0;
    save_copy = recomp_get_config_u32("recovery_save_copy");
    if (save_copy > 2) save_copy = 0;
    using_host_save = coop_recovery_use_host_copy(role, save_copy);
    automatic_world_refresh = shared_items && recomp_get_config_u32("automatic_world_refresh") == 1;
    follow_host_transitions = recomp_get_config_u32("follow_host_transitions") == 1;
    combat_enabled = recomp_get_config_u32("combat");
    if (combat_enabled > 3) combat_enabled = 0;
    transient_enabled = recomp_get_config_u32("same_area_events") == 1;
    if (shared_items) recomp_change_save_file(using_host_save ? item_host_saves[save_profile] : item_guest_saves[save_profile]);
    else recomp_change_save_file(using_host_save ? presence_host_saves[save_profile] : presence_guest_saves[save_profile]);
    unsigned char* save_path = recomp_get_save_file_path();
    u32 save_kind = (using_host_save ? COOP_RECOVERY_SAVE_HOST : 0)
        | (shared_items ? COOP_RECOVERY_SAVE_ITEMS : 0);
    recovery_storage_status = dk64_coop_recovery_configure_v55((const char*)save_path,
        save_profile, save_kind, recomp_get_config_u32("room"));
    recomp_free(save_path);
    host_recovery.checkpoint = (recovery_storage_status & COOP_RECOVERY_STORAGE_CHECKPOINT) != 0;
    if (configured_role == ROLE_HOST
            && !(recovery_storage_status & COOP_RECOVERY_STORAGE_ERROR)
            && (recovery_storage_status & COOP_RECOVERY_STORAGE_FOLLOWER)) {
        role = ROLE_JOIN;
        recomp_printf("[dk64-coop] Following the newer recovered host authority with the host campaign copy.\n");
    }
    if (configured_role == ROLE_JOIN && save_copy == 0
            && !(recovery_storage_status & COOP_RECOVERY_STORAGE_ERROR)
            && (recovery_storage_status & COOP_RECOVERY_STORAGE_PROMOTED)) {
        role = ROLE_HOST;
        host_recovery.promoted = 1;
        recomp_printf("[dk64-coop] Restored promoted host authority from the guest campaign journal.\n");
    }
    merge_guest_progress = role == ROLE_HOST && recomp_get_config_u32("save_conflict") == 1;
    promoted_host = role == ROLE_HOST && !using_host_save;
    recomp_printf("[dk64-coop] Using isolated co-op campaign %d %s copy.\n",
        save_profile + 1, using_host_save ? "host" : "guest");
}

// Existing upstream event: no original-function hooks or loader changes.
RECOMP_CALLBACK("*", recomp_on_flag_change) void coop_flag_change(s16* flag, u8* value, u8* type) {
    if (shared_items && *type == 0 && *value && coop_item_id(*flag) >= 0)
        coop_items_observe(&items, role == ROLE_JOIN, *flag, *value, *type, isFlagSet(*flag, 0),
            game_mode == GAME_MODE_ADVENTURE, current_file);
}

static void remove_remote(void) {
    Actor* actor = remote_actor;
    remote_actor = NULL;
    remote_pose.cooldown = 0;
    if (actor != NULL) { retiring_actor = actor; retiring_generation = remote_generation; deleteActor(actor); }
}

// Upstream 1.0.2 exposes exact map and EEPROM lifecycle boundaries. The map
// callback records only a serial here; game-state cleanup runs on the next
// ordinary co-op frame, after the freshly loaded actor registry is stable.
RECOMP_CALLBACK("*", recomp_on_map_load) void coop_map_loaded(void) {
    map_load_serial++;
}

RECOMP_CALLBACK("*", recomp_on_eeprom_load) void coop_eeprom_loaded(void) {
    eeprom_load_started = 1;
    if (role == ROLE_HOST || role == ROLE_JOIN)
        recomp_printf("[dk64-coop] Isolated campaign selected before EEPROM load.\n");
}

static void coop_reset_loaded_map(void) {
    epoch++;
    remove_remote();
    transient_page = 0;
    transient_result = (CoopTransientResult){0};
    // A normal transition already rebuilt the old map, so a queued refresh
    // for it is no longer needed.
    if (items.refresh_pending && items.refresh_map != (u32)current_map) items.refresh_pending = 0;
    if (items.world_save_pending && (u32)current_map != 194) items.world_save_pending = 0;
}

static u32 coop_item_words_any(const unsigned* words) {
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) if (words[i]) return 1;
    return 0;
}

static u32 coop_recovery_fingerprint(void) {
    u32 hash = 2166136261u;
#define COOP_RECOVERY_MIX(value) do { hash ^= (u32)(value); hash *= 16777619u; } while (0)
    COOP_RECOVERY_MIX(save_profile + 1);
    COOP_RECOVERY_MIX(using_host_save);
    COOP_RECOVERY_MIX(items.input.file);
    COOP_RECOVERY_MIX(world.input.values);
    for (u32 i = 0; i < COOP_ITEM_WORDS; ++i) COOP_RECOVERY_MIX(items.input.owned[i]);
#undef COOP_RECOVERY_MIX
    return hash;
}

static u32 coop_recovery_local_safe(u32 playing) {
    if (!playing || loading_zone_transition_speed != 0.0f || D_global_asm_807FD730) return 0;
    if (!shared_items) return 1;
    return coop_items_safe_map() && items.input.ready && items.baseline && items.live_snapshot
        && !items.deferred && !items.file_changed && !items.counter_error
        && !items.save_pending && !items.world_save_pending && !items.refresh_pending
        && !coop_item_words_any(items.input.request);
}

static u32 coop_recovery_checkpoint_safe(u32 playing) {
    if (!coop_recovery_local_safe(playing) || status != NET_CONNECTED) return 0;
    if (!shared_items) return 1;
    return items.result.status == 3 && !coop_item_words_any(items.result.apply)
        && world.result.status == 3 && !world.result.pending && !world.result.apply;
}

static void coop_recovery_observe_local_intent(void) {
    if (role != ROLE_JOIN) return;
    u32 changed = coop_item_words_any(items.input.request);
    for (u32 i = 0; i < COOP_WORLD_TOGGLES; ++i) {
        changed |= recovery_world_change[i] != world.input.change[i];
        recovery_world_change[i] = world.input.change[i];
    }
    if (changed) {
        host_recovery.checkpoint = 0;
        host_recovery.stable_frames = 0;
        recovery_save_issued = 0;
    }
}

static u32 coop_start_network(u32 network_role) {
    char* ip = recomp_get_config_string("host_ip");
    u32 result = dk64_coop_start(network_role, ip,
        recomp_get_config_u32("port"), recomp_get_config_u32("room"));
    recomp_free_config_string(ip);
    return result;
}

static u32 coop_promote_guest(void) {
    dk64_coop_stop();
    u32 promoted_status = coop_start_network(ROLE_HOST);
    if (promoted_status != NET_LISTENING) {
        // A failed bind must not strand the player in a half-host session.
        status = coop_start_network(ROLE_JOIN);
        coop_host_recovery_complete(&host_recovery, 0);
        return 0;
    }
    if (!dk64_coop_recovery_promote_v55()) {
        dk64_coop_stop();
        status = coop_start_network(ROLE_JOIN);
        coop_host_recovery_complete(&host_recovery, 0);
        return 0;
    }
    role = ROLE_HOST;
    status = promoted_status;
    promoted_host = 1;
    merge_guest_progress = recomp_get_config_u32("save_conflict") == 1;
    local_ipv4 = dk64_coop_local_ipv4();
    items.join = 0;
    items.bound = 0;
    items.input.session_hi = items.input.session_lo = items.input.scope = 0;
    for (u32 i = 0; i < COOP_ITEM_WORDS; ++i) items.input.request[i] = 0;
    items.result = (CoopItemResult){0};
    world.input.session_hi = world.input.session_lo = world.input.scope = 0;
    world.result = (CoopWorldResult){0};
    transient_input = (CoopTransientInput){0};
    transient_result = (CoopTransientResult){0};
    transition_capture = (CoopTransitionCapture){0};
    transition_follow = (CoopTransitionFollow){0};
    remove_remote();
    epoch++;
    coop_host_recovery_complete(&host_recovery, 1);
    recomp_printf("[dk64-coop] Guest checkpoint promoted to LAN host; the loaded save copy is unchanged.\n");
    return 1;
}

// Original-function hooks require DK64's unfinished runtime ROM decompressor.
// Instead validate ownership against the live actor registry before dereferencing
// a saved pointer. The generation ID prevents allocator-address reuse confusion.
static u8 actor_is_alive(Actor* actor, u32 generation) {
    if (actor == NULL || D_global_asm_807FBB34 > 64) return 0;
    for (u32 i = 0; i < D_global_asm_807FBB34; i++) {
        if (D_global_asm_807FB930[i].actor == actor)
            return actor->unk54 == generation && actor->unk58 == ACTOR_PUSHABLE_BOX;
    }
    return 0;
}

#include "combat_actor.h"

static void remote_behavior(void) {
    if (coop_shot_behavior(gCurrentActorPointer)) return;
    if (gCurrentActorPointer == retiring_actor && gCurrentActorPointer->unk54 == retiring_generation) return;
    if (gCurrentActorPointer != remote_actor || gCurrentActorPointer->unk54 != remote_generation) {
        // Preserve behavior if the supposedly unused box actor appears in a map or another mod.
        if (original_box_behavior != NULL) original_box_behavior();
        return;
    }
    remote_actor->interactable = 0;
    remote_actor->noclip_byte = 1;
    remote_actor->shadow_opacity = 0;
    remote_actor->object_properties_bitfield |= 0x10; // Initialized: no box/player gameplay initialization.
    renderActor(remote_actor, 0);
}

RECOMP_CALLBACK("*", recomp_on_init) void coop_initialize(void) {
    coop_select_save();
    if (role == ROLE_OFF || avatar_available) return;
    original_box_behavior = D_global_asm_8074C0A0[ACTOR_PUSHABLE_BOX];
    if (original_box_behavior != func_global_asm_8067C2B8) {
        recomp_printf("[dk64-coop] Actor slot is already modified; refusing to replace another mod's behavior.\n");
        status = NET_ERROR; return;
    }
    D_global_asm_8074C0A0[ACTOR_PUSHABLE_BOX] = remote_behavior;
    avatar_available = 1;
    coop_combat_init();
    status = coop_start_network(role);
    local_ipv4 = role == ROLE_HOST ? dk64_coop_local_ipv4() : 0;
    recomp_printf("[dk64-coop] Prototype initialized. Experimental combat %s; same-area events %s; remote visual actors never collide.\n",
        combat_enabled ? "ON" : "OFF", transient_enabled ? "ON" : "OFF");
}

RECOMP_CALLBACK("*", dk64recomp_every_frame) void coop_frame(void) {
    if (role == ROLE_OFF || !avatar_available) return;
    if (!actor_is_alive(remote_actor, remote_generation)) remote_actor = NULL;
    if (!actor_is_alive(retiring_actor, retiring_generation)) retiring_actor = NULL;
    addActorToTextOverlayRenderArray(draw_coop_status, NULL, 5);
    if (coop_map_lifecycle_changed(&map_lifecycle, current_map, map_load_serial))
        coop_reset_loaded_map();
    local_state[STATE_MAP] = current_map;
    local_state[STATE_EPOCH] = epoch;
    local_state[STATE_FLAGS] = 0;
    local_state[STATE_ANIM] = local_state[STATE_FRAME] = 0;
    unsigned pose_rows = animation_rows();
    u8 present = game_mode == GAME_MODE_ADVENTURE && gPlayerPointer != NULL
        && gPlayerPointer->animation_state != NULL && current_character_index[0] < 5;
    u8 playing = present && is_cutscene_active == 0;
    if (playing) {
        local_state[STATE_FLAGS] = STATE_ACTIVE;
        local_state[STATE_KONG] = current_character_index[0];
        local_state[STATE_X] = float_bits(gPlayerPointer->x_position);
        local_state[STATE_Y] = float_bits(gPlayerPointer->y_position);
        local_state[STATE_Z] = float_bits(gPlayerPointer->z_position);
        local_state[STATE_YAW] = gPlayerPointer->y_rotation & 0xFFF;
        capture_pose(local_state[STATE_KONG], pose_rows);
    } else if (present && is_cutscene_active == 1) {
        local_state[STATE_FLAGS] = STATE_CUTSCENE;
        local_state[STATE_KONG] = current_character_index[0];
    }
    unsigned transition_ticket = 0, transition_route = 0;
    coop_transition_capture(&transition_capture,
        role == ROLE_HOST && status == NET_CONNECTED, playing,
        loading_zone_transition_speed != 0.0f, current_map, next_map, next_exit,
        &transition_ticket, &transition_route);
    local_state[STATE_TRANSITION_TICKET] = transition_ticket;
    local_state[STATE_TRANSITION_ROUTE] = transition_route;
    coop_combat_capture();
    items.join = role == ROLE_JOIN;
    // Never admit a loaded-level write during either half of an existing
    // transition. That transition will initialize from the synchronized flag
    // on a later stable frame instead.
    items.refresh_enabled = automatic_world_refresh && loading_zone_transition_speed == 0.0f;
    // Wait through cutscenes/loading before checking flag/counter agreement.
    // Rising-bit capture recovers local awards when regular play resumes.
    coop_items_capture(&items, shared_items ? (merge_guest_progress ? 2 : 1) : 0, playing, current_file);
    coop_world_capture(&world, &items);
    coop_recovery_observe_local_intent();
    coop_transient_capture(present);
    // Keep the retired v1-v40 gate words canonical zero so the established
    // combat/item/world bridge offsets remain unchanged.
    CoopTraceInput trace = {0};
    trace.version = COOP_TRACE_VERSION;
    trace.flags = (playing ? COOP_TRACE_PLAYING : 0)
        | (is_cutscene_active == 1 ? COOP_TRACE_CUTSCENE : 0)
        | (loading_zone_transition_speed != 0.0f ? COOP_TRACE_LOADING : 0)
        | (coop_items_safe_map() ? COOP_TRACE_ITEM_SAFE_MAP : 0)
        | (coop_items_snapshot_map() ? COOP_TRACE_ITEM_SNAPSHOT_MAP : 0)
        | (D_global_asm_807FD730 ? COOP_TRACE_REWARD_QUEUE : 0)
        | (D_global_asm_80754280 ? COOP_TRACE_HUD_READY : 0)
        | (items.refresh_pending ? COOP_TRACE_REFRESH_PENDING : 0)
        | (items.save_pending ? COOP_TRACE_SAVE_PENDING : 0)
        | (items.world_save_pending ? COOP_TRACE_WORLD_SAVE_PENDING : 0)
        | (host_recovery.checkpoint ? COOP_TRACE_RECOVERY_CHECKPOINT : 0)
        | (promoted_host ? COOP_TRACE_PROMOTED_HOST : 0)
        | (host_recovery.checkpoint && coop_recovery_checkpoint_safe(playing)
            ? COOP_TRACE_RECOVERY_PERSIST_SAFE : 0);
    trace.level = getLevelIndex(current_map, 1);
    trace.item_deferred = items.deferred;
    trace.item_baseline = items.baseline;
    trace.item_bound = items.bound;
    trace.item_live_snapshot = items.live_snapshot;
    trace.item_wait_reason = items.wait_reason;
    trace.item_wait_id = items.wait_id;
    trace.item_refresh_map = items.refresh_map;
    trace.item_result_status = items.result.status;
    trace.world_result_status = world.result.status;
    trace.world_pending = world.result.pending;
    trace.transient_status = transient_result.status;
    trace.combat_status = combat_result.status;
    trace.recovery_state = recovery_state;
    trace.recovery_fingerprint = coop_recovery_fingerprint();
    CoopExtraInput extra = {{0}, combat_input, items.input, world.input, transient_input, trace};
    CoopExtraResult extra_result = {0};
    status = dk64_coop_tick_v55(local_state, remote_state, &extra, &extra_result);
    coop_items_receive(&items, extra_result.items);
    coop_world_receive(&world, extra_result.world);
    transient_result = extra_result.transient;
    coop_transient_apply();
    coop_world_apply(&world, &items, playing);
    coop_items_save_world_lobby(&items, playing && loading_zone_transition_speed == 0.0f);
    items.peer_same_map = status == NET_CONNECTED
        && remote_state[STATE_MAP] == (u32)current_map
        && (remote_state[STATE_FLAGS] & (STATE_ACTIVE | STATE_CUTSCENE));
    // Shops, bonus games and transitions may have partially completed awards.
    coop_items_apply(&items, playing);
    if (items.training_scene_pending) {
        // This is the one reviewed remotely-startable presentation. Ownership
        // and same-area context were verified before the request was raised.
        if (playing && current_map == MAP_TRAINING_GROUNDS && items.peer_same_map
                && !D_global_asm_807FD730 && gPlayerPointer)
            playCutscene(gPlayerPointer, 3, 1);
        items.training_scene_pending = 0;
    }
    u8 world_refresh_started = 0;
    if (items.refresh_pending) {
        if (items.refresh_map != (u32)current_map) {
            items.refresh_pending = 0;
        } else if (loading_zone_transition_speed != 0.0f) {
            // A player-initiated transition, including a same-map transition,
            // already provides the required vanilla actor/script rebuild.
            items.refresh_pending = 0;
        } else if (playing && loading_zone_transition_speed == 0.0f && !items.save_pending
                && !D_global_asm_807FD730) {
            // Re-enter through the same entrance. This rebuilds every actor and
            // instance script through vanilla initialization without guessing
            // at their private live state.
            func_global_asm_805FF378(current_map, current_exit);
            if (loading_zone_transition_speed != 0.0f) {
                items.refresh_pending = 0;
                world_refresh_started = 1;
                remove_remote();
                recomp_printf("[dk64-coop] Reloading the current map to apply shared world state.\n");
            }
        }
    }
    u32 recovery_command = recomp_get_config_u32("host_recovery");
    if (recovery_command > COOP_RECOVERY_PROMOTE) recovery_command = COOP_RECOVERY_OFF;
    recovery_storage_status = dk64_coop_recovery_status_v55();
    if (role == ROLE_HOST && (recovery_storage_status & COOP_RECOVERY_STORAGE_FOLLOWER)) {
        role = ROLE_JOIN; promoted_host = 0; merge_guest_progress = 0;
        host_recovery.promoted = 0; host_recovery.checkpoint = 0;
        recomp_printf("[dk64-coop] A newer LAN authority was found; this device is rejoining as the peer.\n");
    }
    if (status != NET_CONNECTED && !(recovery_storage_status & COOP_RECOVERY_STORAGE_CHECKPOINT))
        host_recovery.checkpoint = 0;
    u32 checkpoint_safe = coop_recovery_checkpoint_safe(playing);
    recovery_state = coop_host_recovery_update(&host_recovery, role == ROLE_JOIN,
        status == NET_CONNECTED, checkpoint_safe,
        coop_recovery_local_safe(playing), recovery_command);
    if (role == ROLE_JOIN && status == NET_CONNECTED && checkpoint_safe
            && host_recovery.stable_frames == 1 && !host_recovery.checkpoint
            && !recovery_save_issued) {
        func_global_asm_8060DEC8();
        recovery_save_issued = 1;
    }
    if (recovery_storage_status & COOP_RECOVERY_STORAGE_ERROR)
        recovery_state = COOP_RECOVERY_STORAGE_FAILED;
    if (recovery_state == COOP_RECOVERY_START) {
        if (coop_promote_guest()) recovery_state = COOP_RECOVERY_ACTIVE;
        else recovery_state = COOP_RECOVERY_FAILED;
        return;
    }
    combat_result = extra_result.combat;
    if (!world_refresh_started && coop_transition_should_follow(&transition_follow,
            status == NET_CONNECTED, follow_host_transitions && role == ROLE_JOIN,
            playing, loading_zone_transition_speed != 0.0f, current_map, remote_state[STATE_FLAGS],
            remote_state[STATE_TRANSITION_TICKET], remote_state[STATE_TRANSITION_ROUTE])) {
        unsigned route = remote_state[STATE_TRANSITION_ROUTE];
        func_global_asm_805FF378((Maps)coop_transition_destination(route), coop_transition_exit(route));
        if (loading_zone_transition_speed != 0.0f) {
            coop_transition_consumed(&transition_follow);
            remove_remote();
        }
    }
    coop_combat_move_enemies();
    coop_combat_render(playing && status == NET_CONNECTED && combat_input.enabled
        && combat_result.status >= COOP_COMBAT_SHOTS && remote_state[STATE_MAP] == (u32)current_map);
    if (!playing || loading_zone_transition_speed != 0.0f || status != NET_CONNECTED
            || !(remote_state[STATE_FLAGS] & STATE_ACTIVE)
            || remote_state[STATE_MAP] != (u32)current_map || remote_state[STATE_KONG] >= 5) {
        remove_remote(); return;
    }
    if (remote_actor != NULL && (remote_epoch != remote_state[STATE_EPOCH] || remote_kong != remote_state[STATE_KONG])) remove_remote();
    f32 x = bits_float(remote_state[STATE_X]), y = bits_float(remote_state[STATE_Y]), z = bits_float(remote_state[STATE_Z]);
    if (remote_actor == NULL && retiring_actor == NULL && D_global_asm_807FBB34 < 60) {
        Actor* saved_last = gLastSpawnedActor;
        // The existing six-byte Kong model table provides a model from a local allowlist.
        // Network data never supplies actor IDs, model IDs, function pointers or animation bytecode.
        if (spawnActor(ACTOR_PUSHABLE_BOX, D_global_asm_8074E814[remote_state[STATE_KONG] * 3])) {
            remote_actor = gLastSpawnedActor;
            remote_pose.cooldown = 0;
            remote_generation = remote_actor->unk54;
            remote_actor->interactable = 0;
            remote_actor->noclip_byte = 1;
            remote_actor->shadow_opacity = 0;
            remote_actor->draw_distance = 2000;
            remote_actor->object_properties_bitfield |= 0x10;
            remote_actor->unk64 |= 2;
            if (remote_actor->animation_state != NULL) {
                remote_actor->animation_state->scale_x = 0.15f;
                remote_actor->animation_state->scale_y = 0.15f;
                remote_actor->animation_state->scale_z = 0.15f;
            }
            remote_actor->x_position = x; remote_actor->y_position = y; remote_actor->z_position = z;
            remote_epoch = remote_state[STATE_EPOCH]; remote_kong = remote_state[STATE_KONG];
            recomp_printf("[dk64-coop] Created remote visual proxy.\n");
        }
        gLastSpawnedActor = saved_last;
    }
    if (remote_actor != NULL) {
        // Smooth ordinary snapshots; snap large discontinuities instead of crossing the map.
        f32 dx = x - remote_actor->x_position, dy = y - remote_actor->y_position, dz = z - remote_actor->z_position;
        f32 blend = dx * dx + dy * dy + dz * dz > 250000.0f ? 1.0f : 0.4f;
        remote_actor->x_position += dx * blend; remote_actor->y_position += dy * blend; remote_actor->z_position += dz * blend;
        remote_actor->y_rotation = remote_state[STATE_YAW];
        remote_actor->unk146 = combat_result.status >= COOP_COMBAT_SHOTS ? combat_result.hands : 0;
        coop_pose_apply(&remote_pose, remote_actor,
            coop_animation_clip(D_global_asm_807FBB58, pose_rows, remote_kong, remote_state[STATE_ANIM]),
            bits_float(remote_state[STATE_FRAME]));
        renderActor(remote_actor, 0);
    }
}

static Gfx* draw_coop_status(Gfx* dl, Actor* unused) {
    (void)unused;
    char host_text[40];
    char* text = "LAN: CONNECTION ERROR";
    if (status == NET_LISTENING && local_ipv4) {
        _sprintf(host_text, "LAN HOST IP: %d.%d.%d.%d", local_ipv4 >> 24,
            (local_ipv4 >> 16) & 0xFF, (local_ipv4 >> 8) & 0xFF, local_ipv4 & 0xFF);
        text = host_text;
    }
    else if (status == NET_LISTENING) text = "LAN HOST: WAITING - IP UNKNOWN";
    else if (status == NET_CONNECTING) text = "LAN: CONNECTING";
    else if (status == NET_CONNECTED) text = remote_actor ? "LAN: PLAYER CONNECTED" : "LAN: CONNECTED - SAME MAP NEEDED";
    else if (status == NET_BUSY) text = "LAN: HOST IS FULL";
    if ((recovery_storage_status & COOP_RECOVERY_STORAGE_FOLLOWER)
            && status == NET_CONNECTING)
        text = "LAN RECOVERY: REJOINING NEW HOST";
    if (combat_enabled && status == NET_CONNECTED) {
        if (combat_file_changed) text = "LAN COMBAT: RESTART AFTER FILE CHANGE";
        else if (combat_result.status == COOP_COMBAT_LAYOUT_MISMATCH) text = "LAN COMBAT: SPAWN LAYOUT MISMATCH";
        else if (combat_result.status == COOP_COMBAT_READY) text = combat_result.paired ?
            (combat_result.movement ? "LAN ENEMIES: HOST MOVEMENT" : "LAN ENEMIES: DEFEATS LINKED") : "LAN COMBAT: WAITING FOR LIVE ENEMIES";
        else if (combat_result.status == COOP_COMBAT_SHOTS) text = "LAN COMBAT: SHOTS ONLY IN THIS MAP";
        else text = "LAN COMBAT: WAITING FOR PEER";
    }
    if (recovery_state == COOP_RECOVERY_BUILDING) text = "LAN RECOVERY: BUILDING CHECKPOINT";
    else if (recovery_state == COOP_RECOVERY_READY)
        text = status == NET_CONNECTED ? "LAN RECOVERY: CHECKPOINT READY"
            : "LAN RECOVERY: HOST LOST - SELECT PROMOTE";
    else if (recovery_state == COOP_RECOVERY_NO_CHECKPOINT) text = "LAN RECOVERY: NO SAFE CHECKPOINT";
    else if (recovery_state == COOP_RECOVERY_WAIT_SAFE) text = "LAN RECOVERY: MOVE TO A SAFE MAP";
    else if (recovery_state == COOP_RECOVERY_FAILED) text = "LAN RECOVERY: START FAILED - RESET OPTION";
    else if (recovery_state == COOP_RECOVERY_STORAGE_FAILED) text = "LAN RECOVERY: JOURNAL ERROR";
    else if (recovery_state == COOP_RECOVERY_ACTIVE && status == NET_LISTENING)
        text = "LAN RECOVERY HOST: WAITING FOR PEER";
    gSPDisplayList(dl++, D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 100, 255, 220, 255);
    dl = printStyledText(dl, 6, 14 * 4, 20 * 4, (u8*)text, 1);
    if (shared_items) {
        char* item_text = "LAN ITEMS: WAITING FOR BOTH PLAYERS";
        if (items.file_changed) item_text = "LAN ITEMS: RESTART AFTER FILE CHANGE";
        else if (items.counter_error) item_text = "LAN ITEMS: COUNTER CONFLICT";
        else if (items.result.status == 4) item_text = role == ROLE_HOST
            ? "LAN ITEMS: GUEST SAVE AHEAD - ENABLE MERGE" : "LAN ITEMS: GUEST SAVE AHEAD - ASK HOST TO MERGE";
        else if (items.troff_pending) item_text = "LAN ITEMS: FEEDING PENDING";
        else if (items.result.status == 3) item_text = items.deferred
            ? "LAN ITEMS: SYNCED - SAFE APPLY QUEUED" : "LAN ITEMS: SYNCED";
        else if (items.result.status == 2) item_text = "LAN ITEMS: SYNCHRONIZING";
        dl = printStyledText(dl, 6, 14 * 4, 33 * 4, (u8*)item_text, 1);
        char* world_text = "LAN WORLD: WAITING FOR BOTH PLAYERS";
        if (world.result.status == 3) world_text = "LAN WORLD: SYNCED";
        else if (world.result.status == 2) world_text = "LAN WORLD: PENDING - LEAVE LEVEL";
        else if (status == NET_CONNECTED && (items.result.status == 2 || items.result.status == 3))
            world_text = items.deferred ? "LAN WORLD: PAUSED - LOCAL AREA UNSAFE"
                : "LAN WORLD: PAUSED - PEER AREA UNSAFE";
        if (items.file_changed || items.counter_error || items.result.status == 4)
            world_text = "LAN WORLD: ITEM SAVE CONFLICT";
        dl = printStyledText(dl, 6, 14 * 4, 46 * 4, (u8*)world_text, 1);
    }
    if (transient_enabled) {
        char* event_text = "LAN EVENTS: DIFFERENT AREA";
        if (transient_file_changed) event_text = "LAN EVENTS: RESTART AFTER FILE CHANGE";
        else if (status != NET_CONNECTED) event_text = "LAN EVENTS: WAITING FOR PEER";
        else if (remote_state[STATE_MAP] == (u32)current_map)
            event_text = transient_result.status >= COOP_TRANSIENT_APPLYING
                ? "LAN EVENTS: HOST AUTHORITY ACTIVE"
                : "LAN EVENTS: WAITING FOR ROOM EPOCH";
        dl = printStyledText(dl, 6, 14 * 4, (shared_items ? 59 : 33) * 4, (u8*)event_text, 1);
    }
    return dl;
}
