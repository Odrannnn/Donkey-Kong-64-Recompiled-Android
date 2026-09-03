// Run the same-area game adapter against a recording engine boundary. This
// proves that network records can only touch reviewed loaded script objects and
// can only align an already-running matching cutscene by one camera phase.
#include "../mod/transient_types.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>

using u8 = unsigned char; using s8 = signed char; using u16 = unsigned short;
using s16 = short; using u32 = unsigned; using s32 = int; using f32 = float;
using Maps = u32;
enum { ROLE_OFF, ROLE_HOST, ROLE_JOIN };

struct Prop_ScriptData { s16 unk44[2]{}; u8 unk48[2]{}; };
struct Prop { Prop_ScriptData* unk7C{}; };
struct CoopClamTestData { u8 pad[0x2C]{}; s32 timer{}; };
struct AnimationState { u8 pad[0x64]{}; u16 unk64{}; };
#define DKCOOP_OWL_PRIVATE_TYPES_DEFINED
struct CoopOwlRaceData { u8 pad0[0x2C]{}; s16 rings{}; };
struct CoopOwlEnemyData { u8 pad0[0x46]{}; u16 flags{}; };
struct Actor {
    u32 object_properties_bitfield{};
    u32 unk54{}, unk58{};
    s16 unk132{};
    u8 unk15F{};
    u32 unk168{};
    u8 control_state{}, control_state_progress{};
    void* additional_actor_data{};
    void* unk178{};
    void* unk180{};
    Actor* unk11C{};
    AnimationState* animation_state{};
};
struct EnemySpawner { Actor* tied_actor{}; };
struct CharacterSpawner {
    u8 pad0[2]{}; s16 unk2{}, unk4{}, unk6{}, unk8{};
    union { u8 unkA_u8[2]; u16 unkA_u16; s16 unkA_s16; };
    u8 unkC{}, unkD{}, unkE{}, unkF{}, unk10{}, unk11{}, unk12{}, unk13{};
};
struct ExtraPlayerInfo {
    Actor* vehicle_actor_pointer{};
    Actor* unk1A8{};
    u32 unk1F0{}, unk1F4{};
};
#define MATH_PI_F 3.1415927f
enum { MAP_JAPES_SHELL = 12, MAP_BATTY_BARREL_BANDIT_EASY = 32, MAP_CAVES_ICE_CASTLE = 98,
    MAP_BATTY_BARREL_BANDIT_EASY_2 = 121, MAP_BATTY_BARREL_BANDIT_NORMAL = 122,
    MAP_BATTY_BARREL_BANDIT_HARD = 123,
    MAP_FUNGI = 48,
    ACTOR_TOMATO_ICE = 164, ACTOR_TIMER = 176, ACTOR_TIMER_CONTROLLER = 177, ACTOR_CLAM = 286,
    ACTOR_MINECART_BONUS = 87, ACTOR_OWL = 250, ACTOR_RABBIT_RACE = 252,
    ACTOR_MINIGAME_CONTROLLER = 256,
    ACTOR_BANDIT_HANDLE = 218, ACTOR_BANDIT_SLOT = 219,
    PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE = 0xF8,
    PERMFLAG_ITEM_GB_FUNGI_RABBIT_RACE = 0xF9,
    TEMPFLAG_ICE_TOMATO_BOARD_ACTIVE = 0x30,
    FLAG_TYPE_PERMANENT = 0, FLAG_TYPE_TEMPORARY = 2 };

static unsigned checks, script_calls, last_object, last_state;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "TRANSIENT ADAPTER FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)

static u32 role = ROLE_HOST, current_file, current_map = 7, epoch = 9;
static u32 transient_enabled = 1, transient_revision = 1, transient_page;
static u32 transient_file, transient_file_changed;
static CoopTransientInput transient_input{};
static CoopTransientResult transient_result{};
static f32 loading_zone_transition_speed;
static s8 is_cutscene_active;
static s16 D_global_asm_807476F8 = -1;
static u16 D_global_asm_807F5CF0, D_global_asm_807F5CF4;
static s16 D_global_asm_807F6240[600];
static std::array<Prop, 512> props;
static std::array<Prop_ScriptData, 512> scripts;
static Prop* D_global_asm_807F6000 = props.data();
static std::array<EnemySpawner, 4> enemy_spawners;
static std::array<Actor, 8> actors;
static std::array<CoopClamTestData, 8> actor_data;
static std::array<u8, 0x3A> tomato_data;
struct CoopCountdownTestData {
    unsigned long long started{};
    u32 elapsed{};
    s32 duration{};
    u8 text{};
};
static CoopCountdownTestData tomato_clock_data;
static s8 D_global_asm_807FC8C0[16];
static bool tomato_board_active, caves_pad_available;
struct ActorListEntry { Actor* actor{}; u32 metadata{}; };
static ActorListEntry D_global_asm_807FB930[8];
static u16 D_global_asm_807FBB34;
#define DKCOOP_ENEMY_SPAWNER_TABLE_DEFINED
typedef struct { s16 count, padding; EnemySpawner* first; } CoopEnemySpawnerTable;
static CoopEnemySpawnerTable D_807FDC88;
static unsigned animation_calls, motion_calls, last_animation;
static unsigned tile_activation_calls;
static CharacterSpawner character_spawners[2];
static CharacterSpawner* D_global_asm_807FDC9C = character_spawners;
static CoopOwlRaceData owl_race, owl_race_replacement;
static CoopOwlRaceData* D_global_asm_807FDC90 = &owl_race;
static CoopOwlEnemyData owl_enemy, owl_enemy_replacement;
static CoopOwlEnemyData* D_global_asm_807FDC98 = &owl_enemy;
static Actor* gCurrentActorPointer;
static Actor* gPlayerPointer;
static ExtraPlayerInfo rabbit_player_info;
static ExtraPlayerInfo* extra_player_info_pointer;
static u32 D_global_asm_807FBB64;
static u8 current_character_index[1];
static void (*D_global_asm_8074C0A0[300])(void);
static Maps kosh_parent;
static s32 kosh_exit;
static bool kosh_parent_valid;
static unsigned kosh_original_calls, kosh_success_calls, kosh_success_arg, kosh_success_text;
static AnimationState minecart_animation;
static std::array<u8, 32> minecart_actor_data, minecart_player_data;
static unsigned minecart_original_calls, minecart_success_calls, minecart_cleanup_calls;
static unsigned minecart_success_arg, minecart_success_text, minecart_original_mode;
static char minecart_order[16];
static unsigned minecart_order_count;
static AnimationState rabbit_animation;
static bool rabbit_first_complete, rabbit_gb_owned;
static unsigned rabbit_original_calls, rabbit_original_mode, rabbit_outcome_calls;
struct BattyTestData {
    Actor* reels[4]{};
    s16 unk10{}, unk12{}, unk14{}, unk16{};
    s8 unk18{}; u8 unk19{}, unk1A{}; s8 unk1B{};
    u8 unk1C{}, unk1D{}, unk1E{}; s8 unk1F{};
    void* unk20{};
};
struct BattyReelTestData { Actor* owner{}; s16 speed{}; };
static BattyTestData batty_data;
static std::array<BattyReelTestData, 4> batty_reel_data;
static std::array<u8, 32> batty_timer_data, batty_player_data;
static unsigned batty_original_calls, batty_original_mode, batty_success_calls;
static unsigned batty_success_arg, batty_success_text, batty_cutscene_calls;
static unsigned batty_hide_calls, batty_hide_element;
static f32 batty_hide_rotation;
static unsigned batty_hud_remove_calls;
static void* batty_removed_hud;
static Actor* batty_cutscene_actor;
static s16 batty_cutscene_index;
static u8 batty_cutscene_mode;
static char batty_order[16];
static unsigned batty_order_count;
struct OwlTimerTestData { s16 timer{}; };
static s16 owl_a178;
static OwlTimerTestData owl_timer, owl_timer_replacement;
static AnimationState owl_animation;
static std::array<u8, 32> owl_player_data;
static bool owl_reward_owned;
static unsigned owl_original_calls, owl_original_mode, owl_relocate_calls;
static unsigned owl_text_calls, owl_text_file, owl_text_index;
static unsigned owl_animation_calls, owl_animation_id;
static unsigned owl_cutscene_calls, owl_cutscene_id, owl_cutscene_mode;
static char owl_order[16];
static unsigned owl_order_count;

static void owl_log(char step) {
    CHECK(owl_order_count + 1 < sizeof(owl_order));
    owl_order[owl_order_count++] = step;
    owl_order[owl_order_count] = 0;
}

static void minecart_log(char step) {
    CHECK(minecart_order_count + 1 < sizeof(minecart_order));
    minecart_order[minecart_order_count++] = step;
    minecart_order[minecart_order_count] = 0;
}

static s32 func_global_asm_805FF800(Maps* map, s32* exit) {
    if (!kosh_parent_valid) return 0;
    *map = kosh_parent; *exit = kosh_exit; return 1;
}
static void func_bonus_80024158(void) {
    ++kosh_original_calls;
    if (gCurrentActorPointer && gCurrentActorPointer->control_state == 1)
        ++gCurrentActorPointer->control_state;
}
static void func_bonus_800264E0(u8 success, u8 text) {
    if (gCurrentActorPointer && gCurrentActorPointer->unk58 == ACTOR_BANDIT_HANDLE) {
        ++batty_success_calls; batty_success_arg = success; batty_success_text = text;
        batty_order[batty_order_count++] = 'S'; batty_order[batty_order_count] = 0;
    } else {
        ++kosh_success_calls; kosh_success_arg = success; kosh_success_text = text;
    }
    if (gCurrentActorPointer) {
        if (gCurrentActorPointer->unk11C) gCurrentActorPointer->unk11C->control_state = 0;
        ++gCurrentActorPointer->control_state;
    }
    if (gPlayerPointer) gPlayerPointer->control_state = 0x44;
}
static void func_global_asm_806A2E30(void) {}
static void func_bonus_800261B8(void) {}
static void func_global_asm_806FDAB8(s16 element, f32 rotation) {
    ++batty_hide_calls; batty_hide_element = static_cast<unsigned short>(element);
    batty_hide_rotation = rotation;
    batty_order[batty_order_count++] = 'H'; batty_order[batty_order_count] = 0;
}
static void func_global_asm_80715908(void* sprite) {
    ++batty_hud_remove_calls; batty_removed_hud = sprite;
    batty_order[batty_order_count++] = 'D'; batty_order[batty_order_count] = 0;
}
static void func_global_asm_80726EE0(u8 state) {
    CHECK(state == 0); ++minecart_cleanup_calls; minecart_log('C');
}
static void func_minecart_80024000(u8 success, u8 text) {
    ++minecart_success_calls; minecart_success_arg = success; minecart_success_text = text;
    minecart_log('S');
    if (gCurrentActorPointer) {
        if (gCurrentActorPointer->unk11C) gCurrentActorPointer->unk11C->control_state = 0;
        ++gCurrentActorPointer->control_state;
    }
    if (gPlayerPointer) gPlayerPointer->control_state = 0x44;
}
static void func_minecart_80024FD0(void) {
    ++minecart_original_calls; minecart_log('O');
    if (!gCurrentActorPointer) return;
    if (minecart_original_mode == 1 && gCurrentActorPointer->control_state >= 1
            && gCurrentActorPointer->control_state <= 3) {
        func_global_asm_80726EE0(0);
        func_minecart_80024000(1, 0xE);
        gCurrentActorPointer->control_state = 5;
    } else if (minecart_original_mode == 2) {
        if (gPlayerPointer) gPlayerPointer->control_state = 0x43;
        gCurrentActorPointer->control_state = 5;
    } else if (minecart_original_mode == 3 && gCurrentActorPointer->control_state == 4) {
        gCurrentActorPointer->control_state = 3;
    } else if (minecart_original_mode == 4 && gCurrentActorPointer->control_state == 10) {
        gCurrentActorPointer->control_state = 3;
    } else if (minecart_original_mode == 5) {
        D_global_asm_807FB930[0].actor = nullptr;
    } else if (minecart_original_mode == 6) {
        ++gCurrentActorPointer->unk54;
    } else if (minecart_original_mode == 7) {
        ++gCurrentActorPointer->unk11C->unk54;
    }
}
static void func_global_asm_806BE8BC(void) {
    ++rabbit_original_calls;
    if (!gCurrentActorPointer) return;
    if (rabbit_original_mode == 1 && gCurrentActorPointer->control_state == 2) {
        gCurrentActorPointer->control_state = 0x28;
        gCurrentActorPointer->control_state_progress = 0;
    } else if (rabbit_original_mode == 2 && gCurrentActorPointer->control_state == 2) {
        gCurrentActorPointer->control_state = 0x27;
        gCurrentActorPointer->control_state_progress = 0;
    } else if (rabbit_original_mode == 3) {
        D_global_asm_807FB930[0].actor = nullptr;
    } else if (rabbit_original_mode == 4) {
        ++gCurrentActorPointer->unk54;
    } else if (rabbit_original_mode == 5) {
        current_map = 49;
    } else if (rabbit_original_mode == 6) {
        D_global_asm_807FBB64 &= ~4u;
    } else if (rabbit_original_mode == 7) {
        scripts[0x1F].unk48[0] = 2;
    } else if (rabbit_original_mode == 8) {
        gCurrentActorPointer = &actors[1];
    } else if (rabbit_original_mode == 9) {
        // Explicit no-op used to prove non-active states cannot consume a packet.
    } else if (rabbit_original_mode == 10) {
        gPlayerPointer->additional_actor_data = nullptr;
    } else if (gCurrentActorPointer->control_state == 0x28) {
        ++rabbit_outcome_calls;
        if (rabbit_first_complete) rabbit_gb_owned = true;
        else rabbit_first_complete = true;
        gCurrentActorPointer->control_state = 0x37;
        gCurrentActorPointer->control_state_progress = 0;
    }
}
static s32 playCutscene(Actor* actor, s16 cutscene, u8 mode) {
    if (actor && actor->unk58 == ACTOR_OWL) {
        ++owl_cutscene_calls; owl_cutscene_id = static_cast<unsigned short>(cutscene);
        owl_cutscene_mode = mode; owl_log('C');
        return 0;
    }
    ++batty_cutscene_calls; batty_cutscene_actor = actor;
    batty_cutscene_index = cutscene; batty_cutscene_mode = mode;
    batty_order[batty_order_count++] = 'C'; batty_order[batty_order_count] = 0;
    return 0;
}
static void func_bonus_8002570C(void) {
    ++batty_original_calls;
    batty_order[batty_order_count++] = 'O'; batty_order[batty_order_count] = 0;
    if (!gCurrentActorPointer) return;
    auto* data = static_cast<BattyTestData*>(gCurrentActorPointer->additional_actor_data);
    if (batty_original_mode == 1 && gCurrentActorPointer->control_state == 7) {
        func_global_asm_806FDAB8(data->unk19, MATH_PI_F);
        --data->unk16;
        gPlayerPointer->control_state_progress = 1;
        func_bonus_800264E0(0, 0);
        gCurrentActorPointer->control_state = 8;
        gCurrentActorPointer->control_state_progress = 0;
        gCurrentActorPointer->unk168 = 4;
        data->unk10 = 0; data->unk1A = 0;
        playCutscene(nullptr, 0, 0x11);
        if (data->unk20) {
            func_global_asm_80715908(data->unk20);
            data->unk20 = nullptr;
        }
    } else if (batty_original_mode == 2) {
        gPlayerPointer->control_state = 0x43;
        gCurrentActorPointer->control_state = 9;
    } else if (batty_original_mode == 3 && gCurrentActorPointer->control_state == 3) {
        gCurrentActorPointer->control_state = 2;
    } else if (batty_original_mode == 4 && gCurrentActorPointer->control_state == 7) {
        gCurrentActorPointer->control_state = 3;
    } else if (batty_original_mode == 5) {
        D_global_asm_807FB930[0].actor = nullptr;
    } else if (batty_original_mode == 6) {
        ++gCurrentActorPointer->unk54;
    } else if (batty_original_mode == 7) {
        D_global_asm_807FB930[1].actor = nullptr;
    } else if (batty_original_mode == 8) {
        ++gCurrentActorPointer->unk11C->unk54;
    } else if (batty_original_mode == 9) {
        D_global_asm_807FB930[2].actor = nullptr;
    } else if (batty_original_mode == 10) {
        ++data->reels[0]->unk54;
    } else if (batty_original_mode == 11) {
        data->reels[0] = &actors[7];
    } else if (batty_original_mode == 12) {
        gCurrentActorPointer->unk11C = &actors[7];
    }
}
static void recomp_printf(const char*, ...) {}

static s16 func_global_asm_80659470(s32 object) {
    return object >= 0 && object < static_cast<s32>(scripts.size()) ? static_cast<s16>(object) : -1;
}
static unsigned coop_live_world_object_raw_state(unsigned object, unsigned* raw) {
    s16 prop = func_global_asm_80659470((s32)object);
    if (prop < 0 || !D_global_asm_807F6000[prop].unk7C) return 0;
    *raw = D_global_asm_807F6000[prop].unk7C->unk48[0];
    return 1;
}
static unsigned coop_live_world_reveal_object(unsigned object) {
    s16 prop = func_global_asm_80659470((s32)object);
    if (prop < 0 || !D_global_asm_807F6000[prop].unk7C) return 0;
    return 1;
}
static unsigned coop_live_world_mermaid_ready(void) { return 0; }
static unsigned coop_live_world_mermaid_refresh(void) { return 0; }
static unsigned coop_live_world_isles_trombone_ready(void) { return 0; }
static unsigned coop_live_world_isles_trombone_refresh(void) { return 0; }
static unsigned coop_live_world_llama_free_ready(void) { return 0; }
static unsigned coop_live_world_llama_free_refresh(void) { return 0; }
static unsigned coop_live_world_llama_water_ready(void) { return 0; }
static unsigned coop_live_world_llama_water_refresh(void) { return 0; }
static unsigned coop_live_world_seal_ready(void) { return 0; }
static unsigned coop_live_world_seal_refresh(void) { return 0; }
static unsigned coop_live_world_mushroom_switch_ready(unsigned) { return 0; }
static unsigned coop_live_world_mushroom_switch_refresh(unsigned) { return 0; }
static unsigned coop_live_world_rabbit_ready(void) { return 0; }
static unsigned coop_live_world_rabbit_refresh(void) { return 0; }
static unsigned coop_live_world_beanstalk_ready(void) { return 0; }
static unsigned coop_live_world_beanstalk_refresh(void) { return 0; }
static void func_global_asm_8063DA40(s16 slot, s16 state) {
    CHECK(slot >= 0 && slot < 600);
    unsigned object = static_cast<unsigned short>(D_global_asm_807F6240[slot]);
    CHECK(object < scripts.size() && props[object].unk7C == &scripts[object]);
    scripts[object].unk48[0] = static_cast<u8>(state);
    ++script_calls; last_object = object; last_state = static_cast<unsigned short>(state);
}
static void func_global_asm_8063DA78(s16 slot, s16 state, s16 state_index) {
    CHECK(slot >= 0 && slot < 600 && state_index == 1);
    unsigned object = static_cast<unsigned short>(D_global_asm_807F6240[slot]);
    CHECK(object < scripts.size() && props[object].unk7C == &scripts[object]);
    scripts[object].unk48[1] = static_cast<u8>(state);
    ++tile_activation_calls;
}
static u8 isFlagSet(s16 flag, u8 type) {
    if (flag == 0x19D && type == FLAG_TYPE_PERMANENT) return caves_pad_available;
    if (flag == PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE && type == FLAG_TYPE_PERMANENT)
        return rabbit_first_complete;
    if (flag == PERMFLAG_ITEM_GB_FUNGI_RABBIT_RACE && type == FLAG_TYPE_PERMANENT)
        return rabbit_gb_owned;
    if (flag == 0xFA && type == FLAG_TYPE_PERMANENT) return owl_reward_owned;
    CHECK(flag == TEMPFLAG_ICE_TOMATO_BOARD_ACTIVE && type == FLAG_TYPE_TEMPORARY);
    return tomato_board_active;
}
static void playActorAnimation(Actor* actor, s32 animation) {
    ++animation_calls; last_animation = static_cast<unsigned>(animation);
    if (actor && actor->animation_state) actor->animation_state->unk64 = static_cast<u16>(animation);
    if (actor && actor->unk58 == ACTOR_OWL) {
        ++owl_animation_calls; owl_animation_id = static_cast<unsigned>(animation); owl_log('A');
    }
}
static void func_global_asm_807266E8(Actor* actor, CharacterSpawner* spawner) {
    CHECK(actor && actor->unk58 == ACTOR_OWL && spawner == character_spawners);
    ++owl_relocate_calls; owl_log('R');
}
static void loadText(Actor* actor, u16 file, u8 text) {
    CHECK(actor && actor->unk58 == ACTOR_OWL);
    ++owl_text_calls; owl_text_file = file; owl_text_index = text; owl_log('T');
}
static void func_global_asm_806C55E0(void) {
    ++owl_original_calls; owl_log('O');
    if (!gCurrentActorPointer) return;
    Actor* actor = gCurrentActorPointer;
    extra_player_info_pointer->unk1F4 &= ~0x40u;
    if (actor->control_state == 2 || actor->control_state == 0x26) {
        extra_player_info_pointer->unk1F4 |= 0x40u;
        actor->unk15F = static_cast<u8>((0x10 - D_global_asm_807FDC90->rings) + 1);
    }
    if (owl_original_mode == 1 && (actor->control_state == 2 || actor->control_state == 0x26)) {
        D_global_asm_807FDC90->rings = 0;
        func_global_asm_807266E8(actor, D_global_asm_807FDC9C);
        loadText(actor, 0x15, 3);
        playActorAnimation(actor, 0x35B);
        actor->control_state = 0x2A; actor->control_state_progress = 0;
        playCutscene(actor, 0x14, 1);
    } else if (owl_original_mode == 2 && (actor->control_state == 2 || actor->control_state == 0x26)) {
        func_global_asm_807266E8(actor, D_global_asm_807FDC9C);
        loadText(actor, 0x15, 2);
        playActorAnimation(actor, 0x35A);
        actor->control_state = 0x2A; actor->control_state_progress = 0;
        playCutscene(actor, 0x16, 1);
    } else if (owl_original_mode == 3) {
        D_global_asm_807FB930[0].actor = nullptr;
    } else if (owl_original_mode == 4) {
        ++actor->unk54;
    } else if (owl_original_mode == 5) {
        ++gPlayerPointer->unk54;
    } else if (owl_original_mode == 6) {
        gPlayerPointer = &actors[7];
    } else if (owl_original_mode == 7) {
        actor->unk178 = &actors[7];
    } else if (owl_original_mode == 8) {
        actor->unk180 = &owl_timer_replacement;
    } else if (owl_original_mode == 9) {
        D_global_asm_807FDC90 = &owl_race_replacement;
    } else if (owl_original_mode == 10) {
        gCurrentActorPointer = &actors[1];
    } else if (owl_original_mode == 11) {
        extra_player_info_pointer->unk1F0 = 0;
    } else if (owl_original_mode == 12) {
        current_map = 49;
    } else if (owl_original_mode == 13) {
        D_global_asm_8074C0A0[ACTOR_OWL] = func_global_asm_806C55E0;
    } else if (owl_original_mode == 14) {
        owl_reward_owned = true;
    }
}
static void func_global_asm_80614D00(Actor*, f32, f32) { ++motion_calls; }

#include "../mod/world_live_game.h"
#include "../mod/transient_game.h"

static void reset() {
    std::fill(std::begin(D_global_asm_807F6240), std::end(D_global_asm_807F6240), static_cast<s16>(-1));
    props = {}; scripts = {};
    enemy_spawners = {}; actors = {}; actor_data = {}; D_807FDC88 = {};
    tomato_data = {}; tomato_clock_data = {}; tomato_board_active = false;
    caves_pad_available = true; D_global_asm_807FBB34 = 0;
    std::fill(std::begin(D_global_asm_807FB930), std::end(D_global_asm_807FB930), ActorListEntry{});
    std::fill(std::begin(D_global_asm_807FC8C0), std::end(D_global_asm_807FC8C0), static_cast<s8>(-1));
    role = ROLE_HOST; current_file = 0; current_map = 7; epoch = 9;
    coop_transient_passage_map = coop_transient_passage_epoch = 0;
    coop_transient_passage_bits = 0;
    transient_enabled = 1; transient_revision = 1; transient_page = 0;
    transient_file = transient_file_changed = 0;
    coop_transient_applied_timer_epoch = 0;
    std::fill(std::begin(coop_transient_applied_timers), std::end(coop_transient_applied_timers), CoopTransientRecord{});
    coop_transient_last_cutscene = {};
    coop_transient_cutscene_epoch = coop_transient_cutscene_hold = 0;
    coop_lobby_pad_applied_epoch = coop_lobby_pad_applied_map = 0;
    coop_lobby_pad_applied_object = 0;
    transient_input = {}; transient_result = {};
    loading_zone_transition_speed = 0; is_cutscene_active = 0;
    D_global_asm_807476F8 = -1; D_global_asm_807F5CF0 = D_global_asm_807F5CF4 = 0;
    script_calls = last_object = last_state = 0;
    animation_calls = motion_calls = last_animation = 0;
    tile_activation_calls = 0;
    std::fill(std::begin(character_spawners), std::end(character_spawners), CharacterSpawner{});
    D_global_asm_807FDC9C = character_spawners;
    owl_race = {}; owl_race_replacement = {}; D_global_asm_807FDC90 = &owl_race;
    owl_enemy = {}; owl_enemy_replacement = {}; D_global_asm_807FDC98 = &owl_enemy;
    gCurrentActorPointer = gPlayerPointer = nullptr;
    std::fill(std::begin(D_global_asm_8074C0A0), std::end(D_global_asm_8074C0A0), nullptr);
    D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = func_bonus_80024158;
    D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = func_minecart_80024FD0;
    D_global_asm_8074C0A0[ACTOR_RABBIT_RACE] = func_global_asm_806BE8BC;
    D_global_asm_8074C0A0[ACTOR_OWL] = func_global_asm_806C55E0;
    D_global_asm_8074C0A0[ACTOR_BANDIT_HANDLE] = func_bonus_8002570C;
    D_global_asm_8074C0A0[ACTOR_TIMER] = func_global_asm_806A2E30;
    D_global_asm_8074C0A0[ACTOR_BANDIT_SLOT] = func_bonus_800261B8;
    kosh_parent = 0; kosh_exit = 0; kosh_parent_valid = false;
    kosh_original_calls = kosh_success_calls = kosh_success_arg = kosh_success_text = 0;
    coop_kosh_original = nullptr; coop_kosh_hook = 0;
    coop_kosh_pending_epoch = coop_kosh_pending_key = 0;
    coop_kosh_applied_epoch = coop_kosh_applied_key = 0;
    coop_kosh_success_epoch = coop_kosh_success_key = 0;
    current_character_index[0] = 0;
    minecart_animation = {}; minecart_actor_data = {}; minecart_player_data = {};
    minecart_original_calls = minecart_success_calls = minecart_cleanup_calls = 0;
    minecart_success_arg = minecart_success_text = minecart_original_mode = 0;
    minecart_order_count = 0; minecart_order[0] = 0;
    coop_minecart_original = nullptr; coop_minecart_hook = 0;
    coop_minecart_pending_epoch = coop_minecart_pending_key = 0;
    coop_minecart_applied_epoch = coop_minecart_applied_key = 0;
    coop_minecart_success_epoch = coop_minecart_success_key = 0;
    rabbit_animation = {}; rabbit_player_info = {};
    extra_player_info_pointer = &rabbit_player_info; D_global_asm_807FBB64 = 0;
    rabbit_first_complete = rabbit_gb_owned = false;
    rabbit_original_calls = rabbit_original_mode = rabbit_outcome_calls = 0;
    coop_rabbit_original = nullptr; coop_rabbit_hook = 0;
    coop_rabbit_pending_epoch = coop_rabbit_pending_key = 0;
    coop_rabbit_applied_epoch = coop_rabbit_applied_key = 0;
    coop_rabbit_success_epoch = coop_rabbit_success_key = 0;
    batty_data = {}; batty_reel_data = {}; batty_timer_data = {}; batty_player_data = {};
    batty_original_calls = batty_original_mode = batty_success_calls = 0;
    batty_success_arg = batty_success_text = batty_cutscene_calls = 0;
    batty_hide_calls = batty_hide_element = 0; batty_hide_rotation = 0;
    batty_hud_remove_calls = 0; batty_removed_hud = nullptr;
    batty_cutscene_actor = nullptr; batty_cutscene_index = 0; batty_cutscene_mode = 0;
    batty_order_count = 0; batty_order[0] = 0;
    coop_batty_original = nullptr; coop_batty_hook = 0;
    coop_batty_pending_epoch = coop_batty_pending_key = 0;
    coop_batty_applied_epoch = coop_batty_applied_key = 0;
    coop_batty_success_epoch = coop_batty_success_key = 0;
    owl_a178 = 0; owl_timer = {}; owl_timer_replacement = {};
    owl_animation = {}; owl_player_data = {}; owl_reward_owned = false;
    owl_original_calls = owl_original_mode = owl_relocate_calls = 0;
    owl_text_calls = owl_text_file = owl_text_index = 0;
    owl_animation_calls = owl_animation_id = 0;
    owl_cutscene_calls = owl_cutscene_id = owl_cutscene_mode = 0;
    owl_order_count = 0; owl_order[0] = 0;
    coop_owl_original = nullptr; coop_owl_hook = 0;
    coop_owl_pending_epoch = coop_owl_pending_key = 0;
    coop_owl_applied_epoch = coop_owl_applied_key = 0;
    coop_owl_success_epoch = coop_owl_success_key = 0;
    coop_transient_init();
}
static void load(unsigned slot, unsigned object, unsigned state, unsigned timer = 0) {
    CHECK(slot < 600 && object < scripts.size());
    D_global_asm_807F6240[slot] = static_cast<s16>(object);
    scripts[object].unk48[0] = static_cast<u8>(state);
    scripts[object].unk44[0] = static_cast<s16>(timer);
    props[object].unk7C = &scripts[object];
}
static void load_clam(unsigned slot, unsigned state, int timer) {
    CHECK(slot < actors.size());
    D_807FDC88.count = static_cast<s16>(slot + 1);
    D_807FDC88.first = enemy_spawners.data();
    actors[slot].object_properties_bitfield = 0x10;
    actors[slot].unk58 = ACTOR_CLAM;
    actors[slot].control_state = static_cast<u8>(state);
    actors[slot].additional_actor_data = &actor_data[slot];
    actor_data[slot].timer = timer;
    enemy_spawners[slot].tied_actor = &actors[slot];
}
static void load_tomato(unsigned state) {
    tomato_data[0x38] = static_cast<u8>(state);
    actors[0].object_properties_bitfield = 0x10;
    actors[0].unk58 = ACTOR_TOMATO_ICE;
    actors[0].unk178 = tomato_data.data();
    actors[0].unk11C = &actors[1];
    actors[1].unk58 = ACTOR_TIMER_CONTROLLER;
    actors[1].control_state = 2;
    actors[1].additional_actor_data = &tomato_clock_data;
    tomato_clock_data.elapsed = 12;
    tomato_clock_data.duration = 60;
    D_global_asm_807FB930[0].actor = &actors[0];
    D_global_asm_807FBB34 = 1;
    tomato_board_active = true;
    for (unsigned i = 0; i < 16; ++i) load(i, i, 0);
}
static void load_kosh(unsigned key) {
    CHECK(key >= 1 && key <= 4);
    const auto& row = coop_kosh_identities[key - 1];
    current_map = row.map; kosh_parent = row.parent; kosh_exit = 0; kosh_parent_valid = true;
    character_spawners[1].pad0[0] = 2;
    character_spawners[0].unkA_u8[0] = row.target;
    static CoopKoshData data;
    data = {}; data.intro = 2;
    actors[0].object_properties_bitfield = 0x10;
    actors[0].unk54 = 77; actors[0].unk58 = ACTOR_MINIGAME_CONTROLLER;
    actors[0].unk178 = &data; actors[0].unk11C = &actors[1];
    gCurrentActorPointer = &actors[0]; gPlayerPointer = &actors[2];
    D_global_asm_807FB930[0].actor = &actors[0]; D_global_asm_807FBB34 = 1;
}
static void load_minecart(unsigned key) {
    CHECK(key >= 1 && key <= 3);
    const auto& row = coop_minecart_identities[key - 1];
    current_map = row.map; kosh_parent = row.parent; kosh_exit = 0; kosh_parent_valid = true;
    current_character_index[0] = row.kong;
    actors[0].object_properties_bitfield = 0x10;
    actors[0].unk54 = 91; actors[0].unk58 = ACTOR_MINECART_BONUS;
    actors[0].additional_actor_data = minecart_actor_data.data();
    actors[0].animation_state = &minecart_animation; actors[0].unk11C = &actors[1];
    actors[1].control_state = 5;
    actors[2].additional_actor_data = minecart_player_data.data();
    gCurrentActorPointer = &actors[0]; gPlayerPointer = &actors[2];
    D_global_asm_807FB930[0].actor = &actors[0];
    D_global_asm_807FB930[1].actor = &actors[1]; D_global_asm_807FBB34 = 2;
}
static void load_rabbit(unsigned key) {
    CHECK(key >= 1 && key <= 2);
    current_map = MAP_FUNGI; current_character_index[0] = 2;
    rabbit_first_complete = key == 2; rabbit_gb_owned = false;
    actors[0].object_properties_bitfield = 0x10;
    actors[0].unk54 = 113; actors[0].unk58 = ACTOR_RABBIT_RACE;
    actors[0].animation_state = &rabbit_animation;
    actors[0].control_state = 2; actors[0].control_state_progress = 7;
    gCurrentActorPointer = &actors[0]; gPlayerPointer = &actors[2];
    actors[2].additional_actor_data = &rabbit_player_info;
    extra_player_info_pointer = &rabbit_player_info;
    rabbit_player_info.unk1F0 = 0; rabbit_player_info.unk1F4 = 0x40;
    D_global_asm_807FBB64 = 4;
    D_global_asm_807FB930[0].actor = &actors[0]; D_global_asm_807FBB34 = 1;
    load(0x1F, 0x1F, 3);
    if (key == 2) load(0x57, 0x57, 0);
}
static void load_owl(unsigned state = 2) {
    current_map = MAP_FUNGI; current_character_index[0] = 1;
    actors[0].object_properties_bitfield = 0x10;
    actors[0].unk54 = 127; actors[0].unk58 = ACTOR_OWL;
    actors[0].animation_state = &owl_animation;
    actors[0].control_state = static_cast<u8>(state);
    actors[0].control_state_progress = 2;
    actors[0].unk168 = 0; actors[0].unk15F = 1;
    actors[0].unk178 = &owl_a178; actors[0].unk180 = &owl_timer;
    owl_animation.unk64 = 0x357; owl_timer.timer = 0x78;
    actors[2].unk54 = 128; actors[2].control_state = 0x63;
    actors[2].additional_actor_data = &rabbit_player_info;
    gCurrentActorPointer = &actors[0]; gPlayerPointer = &actors[2];
    extra_player_info_pointer = &rabbit_player_info;
    rabbit_player_info.unk1F0 = 0x100000; rabbit_player_info.unk1F4 = 0x40;
    owl_race.rings = 0x10; owl_enemy.flags = 0x24;
    D_global_asm_807FB930[0].actor = &actors[0]; D_global_asm_807FBB34 = 1;
}
static void load_batty(unsigned key) {
    CHECK(key >= 1 && key <= 3);
    const auto& row = coop_batty_identities[key - 1];
    current_map = row.map; kosh_parent = row.parent; kosh_exit = 0; kosh_parent_valid = true;
    current_character_index[0] = row.kong;
    actors[0].object_properties_bitfield = 0x10;
    actors[0].unk54 = 131; actors[0].unk58 = ACTOR_BANDIT_HANDLE;
    actors[0].control_state = 2; actors[0].control_state_progress = 1;
    actors[0].unk168 = 0; actors[0].additional_actor_data = &batty_data;
    actors[0].unk11C = &actors[1];
    actors[1].object_properties_bitfield = 0x10;
    actors[1].unk54 = 132; actors[1].unk58 = ACTOR_TIMER;
    actors[1].control_state = 2; actors[1].unk15F = 6;
    actors[1].additional_actor_data = batty_timer_data.data();
    batty_data.unk10 = 17; batty_data.unk14 = 3; batty_data.unk16 = 3;
    batty_data.unk1A = 1; batty_data.unk1C = row.unk1C;
    batty_data.unk1D = row.unk1D; batty_data.unk1E = row.unk1E;
    *reinterpret_cast<s32*>(batty_timer_data.data() + 0xC) = row.unk1C;
    for (unsigned i = 0; i < 4; ++i) {
        Actor& reel = actors[i + 2];
        reel.object_properties_bitfield = 0x10; reel.unk54 = 140 + i;
        reel.unk58 = ACTOR_BANDIT_SLOT; reel.control_state = 0; reel.unk15F = i;
        reel.additional_actor_data = &batty_reel_data[i];
        batty_reel_data[i].owner = &actors[0]; batty_reel_data[i].speed = 10 + i;
        batty_data.reels[i] = &reel;
    }
    actors[6].additional_actor_data = batty_player_data.data();
    actors[6].control_state = 0x49;
    gCurrentActorPointer = &actors[0]; gPlayerPointer = &actors[6];
    rabbit_player_info.vehicle_actor_pointer = &actors[0];
    rabbit_player_info.unk1A8 = &actors[1];
    actors[6].additional_actor_data = &rabbit_player_info;
    extra_player_info_pointer = &rabbit_player_info;
    for (unsigned i = 0; i < 6; ++i) D_global_asm_807FB930[i].actor = &actors[i];
    D_global_asm_807FBB34 = 6;
}
static bool contains(unsigned kind, unsigned key, unsigned state) {
    for (unsigned i = 0; i < transient_input.count; ++i) {
        const auto& record = transient_input.records[i];
        if (record.kind == kind && record.key == key && record.state == state) return true;
    }
    return false;
}
static bool contains_value(unsigned kind, unsigned key, unsigned state, unsigned value) {
    for (unsigned i = 0; i < transient_input.count; ++i) {
        const auto& record = transient_input.records[i];
        if (record.kind == kind && record.key == key && record.state == state
                && record.value == value) return true;
    }
    return false;
}

static void capture_checks() {
    reset();
    load(0, 0x1A, 2); load(1, 0x1B, 3); load(2, 0x34, 4);
    coop_transient_capture(1);
    CHECK(transient_input.enabled && transient_input.map == 7 && transient_input.epoch == 9);
    CHECK(transient_file == 1 && !transient_file_changed);
    CHECK(contains(COOP_TRANSIENT_SCRIPT, 0x1A, 2));
    CHECK(contains(COOP_TRANSIENT_SCRIPT, 0x1B, 3));
    CHECK(!contains(COOP_TRANSIENT_SCRIPT, 0x34, 4));
    load(3, 0x30, 1); load(4, 0x31, 2); load(5, 0x32, 3);
    load(6, 0x34, 1); load(7, 0x35, 2);
    load(8, 0x28, 1); load(9, 0x29, 2); load(10, 0x2A, 3); load(11, 0x123, 2);
    bool saw_japes_30 = false, saw_japes_31 = false, saw_japes_32 = false;
    bool saw_japes_34 = false, saw_japes_35 = false;
    bool saw_japes_28 = false, saw_japes_29 = false, saw_japes_2a = false, saw_japes_123 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_japes_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 1, 2);
        saw_japes_31 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 2, 2);
        saw_japes_32 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x32, 2, 2);
        saw_japes_34 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x34, 1, 2);
        saw_japes_35 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x35, 2, 2);
        saw_japes_28 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x28, 1, 2);
        saw_japes_29 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x29, 2, 2);
        saw_japes_2a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2A, 2, 2);
        saw_japes_123 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x123, 2, 2);
    }
    CHECK(saw_japes_30 && saw_japes_31 && saw_japes_32);
    CHECK(saw_japes_34 && saw_japes_35);
    CHECK(saw_japes_28 && saw_japes_29 && saw_japes_2a && saw_japes_123);

    reset(); current_map = 7;
    load(0, 0x38, 1); load(1, 0x39, 2); load(2, 0x3A, 1);
    load(3, 0x3B, 20); load(4, 0x115, 2);
    bool saw_hut_38 = false, saw_hut_39 = false, saw_hut_3a = false;
    bool saw_hut_3b = false, saw_rambi_wall = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_hut_38 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x38, 1, 2);
        saw_hut_39 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x39, 2, 2);
        saw_hut_3a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3A, 1, 2);
        saw_hut_3b |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3B, 1, 2);
        saw_rambi_wall |= contains_value(COOP_TRANSIENT_TRIGGER, 0x115, 2, 2);
    }
    CHECK(saw_hut_38 && saw_hut_39 && saw_hut_3a && saw_hut_3b && saw_rambi_wall);

    reset(); current_map = 7;
    load(0, 0x1F, 1); load(1, 0x20, 11); load(2, 0x58, 2);
    bool saw_feather = false, saw_grape = false, saw_peanut = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_feather |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1F, 1, 11);
        saw_grape |= contains_value(COOP_TRANSIENT_TRIGGER, 0x20, 2, 11);
        saw_peanut |= contains_value(COOP_TRANSIENT_TRIGGER, 0x58, 2, 2);
    }
    CHECK(saw_feather && saw_grape && saw_peanut);
    // Reviewed temporary passages retain one fired observation for this room
    // epoch after their vanilla timer closes. Ordinary reward switches do not.
    scripts[0x20].unk48[0] = 1; scripts[0x58].unk48[0] = 1; transient_page = 0;
    bool saw_latched_grape = false, saw_ready_peanut = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_latched_grape |= contains_value(COOP_TRANSIENT_TRIGGER, 0x20, 2, 11);
        saw_ready_peanut |= contains_value(COOP_TRANSIENT_TRIGGER, 0x58, 1, 2);
    }
    CHECK(saw_latched_grape && saw_ready_peanut);
    ++epoch; transient_page = 0; bool saw_rearmed_grape = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_rearmed_grape |= contains_value(COOP_TRANSIENT_TRIGGER, 0x20, 1, 11);
    }
    CHECK(saw_rearmed_grape);

    reset(); current_map = 38; load(0, 0x0D, 2); load(1, 0x0E, 6); load(2, 0x0F, 10);
    load(3, 0x44, 3); load(4, 0x9D, 1); load(5, 0x9E, 2);
    bool saw_aztec_d = false, saw_aztec_e = false, saw_aztec_f = false;
    bool saw_aztec_guitar = false, saw_aztec_blueprint_d = false, saw_aztec_blueprint_e = false;
    for (unsigned page = 0; page < 4; ++page) {
        coop_transient_capture(1);
        saw_aztec_d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0D, 2, 2);
        saw_aztec_e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2);
        saw_aztec_f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0F, 1, 2);
        saw_aztec_guitar |= contains_value(COOP_TRANSIENT_TRIGGER, 0x44, 2, 2);
        saw_aztec_blueprint_d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x9D, 1, 2);
        saw_aztec_blueprint_e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x9E, 2, 2);
    }
    CHECK(saw_aztec_d && saw_aztec_e && saw_aztec_f);
    CHECK(saw_aztec_guitar);
    CHECK(saw_aztec_blueprint_d && saw_aztec_blueprint_e);
    CHECK(!contains(COOP_TRANSIENT_SCRIPT, 0x9D, 1));

    reset(); current_map = 38;
    load(0, 0x02, 1); load(1, 0x03, 2); load(2, 0x04, 1);
    load(3, 0x05, 2); load(4, 0x9F, 1); load(5, 0xA0, 2);
    bool saw_aztec_2 = false, saw_aztec_3 = false, saw_aztec_4 = false;
    bool saw_aztec_5 = false, saw_aztec_9f = false, saw_aztec_a0 = false;
    for (unsigned page = 0; page < 10; ++page) {
        coop_transient_capture(1);
        saw_aztec_2 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x02, 1, 2);
        saw_aztec_3 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x03, 2, 2);
        saw_aztec_4 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2);
        saw_aztec_5 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 2, 2);
        saw_aztec_9f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x9F, 1, 2);
        saw_aztec_a0 |= contains_value(COOP_TRANSIENT_TRIGGER, 0xA0, 2, 2);
    }
    CHECK(saw_aztec_2 && saw_aztec_3 && saw_aztec_4);
    CHECK(saw_aztec_5 && saw_aztec_9f && saw_aztec_a0);

    reset(); current_map = 38;
    load(0, 0x10, 12); load(1, 0x11, 13); load(2, 0x12, 11);
    load(3, 0x13, 12); load(4, 0x14, 17);
    bool saw_totem_10 = false, saw_totem_11 = false, saw_totem_12 = false;
    bool saw_totem_13 = false, saw_totem_14 = false;
    for (unsigned page = 0; page < 12; ++page) {
        coop_transient_capture(1);
        saw_totem_10 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x10, 1, 13);
        saw_totem_11 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x11, 2, 13);
        saw_totem_12 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x12, 1, 13);
        saw_totem_13 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x13, 1, 13);
        saw_totem_14 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x14, 2, 13);
    }
    CHECK(saw_totem_10 && saw_totem_11 && saw_totem_12 && saw_totem_13 && saw_totem_14);

    reset(); current_map = 30; load(0, 0, 10, 123); load(1, 1, 6, 45);
    bool saw_timer_0 = false, saw_timer_1 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_timer_0 |= contains_value(COOP_TRANSIENT_TIMER, 0, 10, 123);
        saw_timer_1 |= contains_value(COOP_TRANSIENT_TIMER, 1, 6, 45);
    }
    CHECK(saw_timer_0 && saw_timer_1);

    reset(); current_map = 48;
    load(0, 0x18, 1); load(1, 0x19, 2); load(2, 0x1A, 1);
    load(3, 0x1B, 2); load(4, 0x1E, 3);
    bool saw_fungi_18 = false, saw_fungi_19 = false, saw_fungi_1a = false;
    bool saw_fungi_1b = false, saw_fungi_1e = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_fungi_18 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x18, 1, 2);
        saw_fungi_19 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x19, 2, 2);
        saw_fungi_1a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1A, 1, 2);
        saw_fungi_1b |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1B, 2, 2);
        saw_fungi_1e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1E, 2, 2);
    }
    CHECK(saw_fungi_18 && saw_fungi_19 && saw_fungi_1a && saw_fungi_1b && saw_fungi_1e);
    reset(); current_map = 48; load(0, 0x0F, 2); load(1, 0xEB, 1);
    bool saw_fungi_night = false, saw_fungi_mushroom = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_fungi_night |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0F, 2, 2);
        saw_fungi_mushroom |= contains_value(COOP_TRANSIENT_TRIGGER, 0xEB, 1, 2);
    }
    CHECK(saw_fungi_night && saw_fungi_mushroom);

    reset(); current_map = 48; load(0, 0x07, 1);
    bool saw_fungi_door = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_fungi_door |= contains_value(COOP_TRANSIENT_TRIGGER, 0x07, 1, 2);
    }
    CHECK(saw_fungi_door);

    reset(); current_map = 56; load(0, 0x00, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 2, 2));
    reset(); current_map = 58; load(0, 0x00, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 1, 2));
    reset(); current_map = 57; load(0, 0x03, 11);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x03, 2, 11));
    reset(); current_map = 59; load(0, 0x01, 2); load(1, 0x24, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x01, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x24, 1, 2));
    reset(); current_map = 61; load(0, 0x06, 11);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x06, 2, 11));
    reset(); current_map = 62; load(0, 0x00, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 2, 2));

    reset(); current_map = 62; load(0, 0x01, 10); load(1, 0x03, 11);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x01, 1, 11));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x03, 2, 11));
    reset(); current_map = 17;
    load(0, 0x2C, 2); load(1, 0x2D, 11); load(2, 0x2E, 12);
    load(3, 0x2F, 13); load(4, 0x30, 11);
    bool saw_helm_2c = false, saw_helm_2d = false, saw_helm_2e = false;
    bool saw_helm_2f = false, saw_helm_30 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_helm_2c |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2C, 2, 2);
        saw_helm_2d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2D, 1, 12);
        saw_helm_2e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2E, 2, 12);
        saw_helm_2f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 2, 12);
        saw_helm_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 1, 12);
    }
    CHECK(saw_helm_2c && saw_helm_2d && saw_helm_2e && saw_helm_2f && saw_helm_30);

    reset(); current_map = 17;
    load(0, 0x5D, 10); load(1, 0x5A, 10); load(2, 0x58, 10);
    load(3, 0x61, 10); load(4, 0x60, 10);
    bool saw_5d = false, saw_5a = false, saw_58 = false, saw_61 = false, saw_60 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_5d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x5D, 2, 10);
        saw_5a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x5A, 2, 10);
        saw_58 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x58, 2, 10);
        saw_61 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x61, 2, 10);
        saw_60 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x60, 2, 10);
    }
    CHECK(saw_5d && saw_5a && saw_58 && saw_61 && saw_60);

    reset(); current_map = 173; load(0, 0x10, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x10, 2, 2));
    reset(); current_map = 178; load(0, 0x05, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 1, 2));

    reset(); current_map = 34; load(0, 0x31, 3);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 2, 3));
    reset(); current_map = 34; load(0, 0x33, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x33, 1, 3));

    reset(); current_map = 72; load(0, 0x2E, 2); load(1, 0x2F, 13);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x2E, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 2, 13));

    reset(); current_map = 72;
    load(0, 0x1D, 1); load(1, 0x1E, 2); load(2, 0x1F, 3);
    bool saw_ice_1d = false, saw_ice_1e = false, saw_ice_1f = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_ice_1d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1D, 1, 2);
        saw_ice_1e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1E, 2, 2);
        saw_ice_1f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1F, 2, 2);
    }
    CHECK(saw_ice_1d && saw_ice_1e && saw_ice_1f);

    reset(); current_map = 90;
    load(0, 0x03, 1); load(1, 0x04, 2); load(2, 0x05, 20); load(3, 0x06, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x03, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 0, 0) == false);

    reset(); current_map = 108; load(0, 0x00, 2); load(1, 0x04, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));

    reset(); current_map = 112;
    load(0, 0x0D, 1); load(1, 0x0E, 2); load(2, 0x0F, 3);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0D, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0F, 2, 2));

    reset(); current_map = 163;
    load(0, 0x04, 1); load(1, 0x05, 2); load(2, 0x06, 7);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x06, 2, 2));

    for (unsigned map = 21; map <= 24; ++map) {
        reset(); current_map = map; load(0, 0x04, 1); load(1, 0x05, 2);
        if (map == 24) load(2, 0x07, 1);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));
        CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 2, 2));
        if (map == 24)
            CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x07, 1, 2));
    }

    reset(); current_map = 164; load(0, 0x01, 2); load(1, 0x09, 5);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x01, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x09, 2, 5));

    reset(); current_map = 26; load(0, 0x15, 0);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x15, 1, 1));
    scripts[0x15].unk48[0] = 1; transient_page = 0; coop_transient_capture(2);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x15, 2, 1));

    reset(); current_map = 26;
    load(0, 0x20, 0); load(1, 0x3C, 1); load(2, 0x13A, 20);
    load(3, 0x13C, 0); load(4, 0x140, 2);
    bool saw_punch_20 = false, saw_grate_3c = false, saw_punch_13a = false;
    bool saw_grate_13c = false, saw_coconut_140 = false;
    for (unsigned page = 0; page < 10; ++page) {
        coop_transient_capture(1);
        saw_punch_20 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x20, 1, 1);
        saw_grate_3c |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3C, 2, 1);
        saw_punch_13a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x13A, 1, 1);
        saw_grate_13c |= contains_value(COOP_TRANSIENT_TRIGGER, 0x13C, 1, 1);
        saw_coconut_140 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x140, 2, 2);
    }
    CHECK(saw_punch_20 && saw_grate_3c && saw_punch_13a);
    CHECK(saw_grate_13c && saw_coconut_140);

    reset(); current_map = 4; load(0, 0x0A, 2); load(1, 0x0B, 10);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0A, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0B, 1, 2));
    reset(); current_map = 4; load(0, 0x06, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x06, 2, 2));

    reset(); current_map = 30; load(0, 0x11, 1); load(1, 0x1C, 3);
    load(2, 6, 1); load(3, 7, 2); load(4, 8, 3); load(5, 9, 1);
    load(6, 0xA, 2); load(7, 0xB, 1);
    bool saw_bongo = false, saw_tiny_slam = false;
    bool saw_galleon_6 = false, saw_galleon_7 = false, saw_galleon_8 = false;
    bool saw_galleon_9 = false, saw_galleon_a = false, saw_galleon_b = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_bongo |= contains_value(COOP_TRANSIENT_TRIGGER, 0x11, 1, 2);
        saw_tiny_slam |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1C, 2, 2);
        saw_galleon_6 |= contains_value(COOP_TRANSIENT_TRIGGER, 6, 1, 2);
        saw_galleon_7 |= contains_value(COOP_TRANSIENT_TRIGGER, 7, 2, 2);
        saw_galleon_8 |= contains_value(COOP_TRANSIENT_TRIGGER, 8, 2, 2);
        saw_galleon_9 |= contains_value(COOP_TRANSIENT_TRIGGER, 9, 1, 2);
        saw_galleon_a |= contains_value(COOP_TRANSIENT_TRIGGER, 0xA, 2, 2);
        saw_galleon_b |= contains_value(COOP_TRANSIENT_TRIGGER, 0xB, 1, 2);
    }
    CHECK(saw_bongo && saw_tiny_slam);
    CHECK(saw_galleon_6 && saw_galleon_7 && saw_galleon_8);
    CHECK(saw_galleon_9 && saw_galleon_a && saw_galleon_b);

    reset(); current_map = 30;
    load(0, 0x2F, 12); load(1, 0x30, 13); load(2, 0x31, 11);
    bool saw_target_2f = false, saw_target_30 = false, saw_target_31 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_target_2f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 1, 13);
        saw_target_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 2, 13);
        saw_target_31 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 1, 13);
    }
    CHECK(saw_target_2f && saw_target_30 && saw_target_31);

    reset(); current_map = 30; load(0, 0x21, 1);
    bool saw_enguarde_door = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_enguarde_door |= contains_value(COOP_TRANSIENT_TRIGGER, 0x21, 1, 2);
    }
    CHECK(saw_enguarde_door);

    reset(); current_map = 30; load(0, 0x3F, 0);
    bool saw_breakable_gate = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_breakable_gate |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3F, 1, 1);
    }
    CHECK(saw_breakable_gate);

    reset(); current_map = 19; load(0, 0x04, 1); load(1, 0x05, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 2, 2));

    reset(); current_map = 20; load(0, 0x12, 1); load(1, 0x16, 4);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x12, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x16, 2, 2));

    reset(); current_map = 20;
    for (unsigned object = 0x19; object <= 0x28; ++object)
        load(object - 0x19, object, object == 0x23 ? 12 : 11);
    bool saw_matching_ready = false, saw_matching_hit = false, saw_matching_last = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_matching_ready |= contains_value(COOP_TRANSIENT_TRIGGER, 0x19, 1, 12);
        saw_matching_hit |= contains_value(COOP_TRANSIENT_TRIGGER, 0x23, 2, 12);
        saw_matching_last |= contains_value(COOP_TRANSIENT_TRIGGER, 0x28, 1, 12);
    }
    CHECK(saw_matching_ready && saw_matching_hit && saw_matching_last);

    reset(); current_map = 20; load(0, 0x69, 2);
    bool saw_quicksand_switch = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_quicksand_switch |= contains_value(COOP_TRANSIENT_TRIGGER, 0x69, 2, 2);
    }
    CHECK(saw_quicksand_switch);

    reset(); current_map = 20; load(0, 0x6B, 1);
    bool saw_grape_ready = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_grape_ready |= contains_value(COOP_TRANSIENT_TRIGGER, 0x6B, 1, 2);
    }
    CHECK(saw_grape_ready);
    scripts[0x6B].unk48[0] = 2; transient_page = 0;
    bool saw_grape_hit = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_grape_hit |= contains_value(COOP_TRANSIENT_TRIGGER, 0x6B, 2, 2);
    }
    CHECK(saw_grape_hit);

    reset(); current_map = 16; load(0, 0, 1); load(1, 4, 2); load(2, 0x14, 3);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 4, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x14, 2, 2));
    for (unsigned state : {2u, 40u, 20u, 30u, 3u, 4u}) {
        scripts[4].unk48[0] = state; transient_page = 0; coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 4, 2, 2));
    }
    for (unsigned state : {1u, 5u, 19u, 21u, 39u, 41u}) {
        scripts[4].unk48[0] = state; transient_page = 0; coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 4, 1, 2));
    }

    reset(); current_map = 16;
    load(0, 0x0C, 1); load(1, 0x0D, 1); load(2, 0x0E, 10); load(3, 0x0F, 22);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x0C, 1, 0));
    scripts[0x0C].unk48[0] = 20; coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x0C, 4, 0));

    reset(); current_map = 194; load(0, 6, 2);
    coop_transient_capture(1);
    CHECK(contains(COOP_TRANSIENT_PLATFORM, 6, 2));

    reset(); current_map = 26;
    load(0, 0x2E, 1); load(1, 0x2F, 2); load(2, 0x30, 3); load(3, 0x31, 20);
    load(4, 0x24, 2);
    bool saw_2e = false, saw_2f = false, saw_30 = false, saw_31 = false, saw_24 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_2e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2E, 1, 2);
        saw_2f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 2, 2);
        saw_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 2, 2);
        saw_31 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 1, 2);
        saw_24 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x24, 2, 2);
    }
    CHECK(saw_2e && saw_2f && saw_30 && saw_31 && saw_24);

    reset(); current_map = 26;
    load(0, 0x37, 1); load(1, 0x38, 2); load(2, 0x3B, 3);
    bool saw_factory_triangle = false, saw_factory_guitar = false, saw_factory_trombone = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_factory_triangle |= contains_value(COOP_TRANSIENT_TRIGGER, 0x37, 1, 2);
        saw_factory_guitar |= contains_value(COOP_TRANSIENT_TRIGGER, 0x38, 2, 2);
        saw_factory_trombone |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3B, 2, 2);
    }
    CHECK(saw_factory_triangle && saw_factory_guitar && saw_factory_trombone);

    reset(); current_map = 26;
    load(0, 0x3F, 4); load(1, 0x40, 5); load(2, 0x41, 10);
    bool saw_3f = false, saw_40 = false, saw_41 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_3f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3F, 1, 5);
        saw_40 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x40, 2, 5);
        saw_41 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x41, 2, 5);
    }
    CHECK(saw_3f && saw_40 && saw_41);

    reset(); current_map = 26; load(0, 0x14, 31);
    bool saw_piano = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_piano |= contains_value(COOP_TRANSIENT_SEQUENCE, 0x14, 9, 0);
    }
    CHECK(saw_piano);
    scripts[0x14].unk48[0] = 250; transient_page = 0; saw_piano = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_piano |= contains_value(COOP_TRANSIENT_SEQUENCE, 0x14, 0, 0);
    }
    CHECK(saw_piano); // Failure/restart states never masquerade as completion.

    reset(); current_map = 26; load(0, 0x7F, 55);
    bool saw_dartboard = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_dartboard |= contains_value(COOP_TRANSIENT_SEQUENCE, 0x7F, 3, 0);
    }
    CHECK(saw_dartboard);

    reset(); current_map = 84; load(0, 0x00, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 0, 0));
    scripts[0x00].unk48[0] = 6; transient_page = 0;
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 2, 0));
    scripts[0x00].unk48[0] = 30; transient_page = 0;
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 4, 0));
    scripts[0x00].unk48[0] = 50; transient_page = 0;
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 0, 0));

    reset(); current_map = MAP_JAPES_SHELL; load_clam(0, 2, 47);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_ACTOR_CYCLE, 1, 2, 47));
    actor_data[0].timer = 200; transient_page = 0;
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_ACTOR_CYCLE, 1, 2, 90));
    actors[0].object_properties_bitfield = 0; transient_page = 0;
    coop_transient_capture(1);
    CHECK(!contains(COOP_TRANSIENT_ACTOR_CYCLE, 1, 2));

    reset(); current_map = MAP_CAVES_ICE_CASTLE; load_tomato(3);
    unsigned tomato_value = 0;
    for (unsigned i = 0; i < 16; ++i) {
        D_global_asm_807FC8C0[i] = static_cast<s8>((i % 3) - 1);
        tomato_value |= (i % 3) << (i * 2);
    }
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TOMATO_BOARD, 0, 1, tomato_value));
    CHECK(contains_value(COOP_TRANSIENT_TOMATO_CLOCK, 0, 1, 48));
    tomato_data[0x38] = 2; transient_page = 0; coop_transient_capture(1);
    CHECK(!contains(COOP_TRANSIENT_TOMATO_BOARD, 0, 1));
    CHECK(!contains(COOP_TRANSIENT_TOMATO_CLOCK, 0, 1));
    tomato_data[0x38] = 4; tomato_board_active = false; transient_page = 0;
    coop_transient_capture(1); CHECK(!contains(COOP_TRANSIENT_TOMATO_BOARD, 0, 1));
    CHECK(!contains(COOP_TRANSIENT_TOMATO_CLOCK, 0, 1));
    tomato_data[0x38] = 6; transient_page = 0; coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TOMATO_BOARD, 0, 2, tomato_value));
    CHECK(contains_value(COOP_TRANSIENT_TOMATO_CLOCK, 0, 2, 0));

    reset(); load(0, 0x1A, 2); loading_zone_transition_speed = 1;
    coop_transient_capture(1); CHECK(!transient_input.enabled);

    reset(); load(0, 0x1A, 2); coop_transient_capture(1);
    current_file = 1; coop_transient_capture(1);
    CHECK(transient_file_changed && !transient_input.enabled);
}

static void object_apply_checks() {
    reset(); role = ROLE_JOIN; load(0, 0x1A, 2);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_SCRIPT, 0x1A, 20, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x1A && last_state == 20 && scripts[0x1A].unk48[0] == 20);

    transient_result.records[0] = {COOP_TRANSIENT_SCRIPT, 0x99, 7, 0};
    coop_transient_apply(); CHECK(script_calls == 1); // Arbitrary object.
    transient_result.records[0] = {COOP_TRANSIENT_TIMER, 0x1A, 7, 0};
    coop_transient_apply(); CHECK(script_calls == 1); // Wrong reviewed kind.
    transient_result.records[0] = {COOP_TRANSIENT_SCRIPT, 0x1A, 7, 0};
    transient_result.epoch = 8; coop_transient_apply(); CHECK(script_calls == 1);
    transient_result.epoch = 9; role = ROLE_HOST; coop_transient_apply(); CHECK(script_calls == 1);

    // A Host accepts only reviewed activation/sequence commands from a Join.
    // Raw runtime-state records above remain follower-only.
    reset(); role = ROLE_HOST; load(0, 0x31, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x31, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x31 && last_state == 2);

    reset(); role = ROLE_HOST; current_map = 26; load(0, 0x7F, 17);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x7F, 4, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x7F && last_state == 54);

    role = ROLE_JOIN; transient_file_changed = 1;
    transient_result.records[0] = {COOP_TRANSIENT_SCRIPT, 0x1A, 7, 0};
    coop_transient_apply(); CHECK(script_calls == 1); // Save-slot lock blocks apply too.

    reset(); role = ROLE_JOIN; load(0, 0x31, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x31, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x31 && last_state == 2);

    reset(); role = ROLE_JOIN; load(0, 0x35, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x35, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x35 && last_state == 2);

    reset(); role = ROLE_JOIN; load(0, 0x123, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x123, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x123 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x0E, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x0E && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x3A, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x3A, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x3A && last_state == 2);
    coop_transient_apply(); CHECK(script_calls == 1);
    scripts[0x3A].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // A completed hut switch never restarts.

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x115, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x115, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x115 && last_state == 2);
    scripts[0x115].unk48[0] = 0; coop_transient_apply();
    CHECK(script_calls == 1); // The packet cannot initialize the wall switch.

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x1F, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x1F, 2, 11}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x1F && last_state == 11);
    scripts[0x1F].unk48[0] = 1; coop_transient_apply();
    CHECK(script_calls == 1); // The same room epoch consumes the latch once.
    ++epoch; transient_result.epoch = epoch; coop_transient_apply();
    CHECK(script_calls == 2 && last_state == 11); // A later room visit gets one chance.

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x58, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x58, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x58 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x44, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x44, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x44 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x9D, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x9D, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x9D && last_state == 2);
    transient_result.records[0].key = 0x9F; coop_transient_apply();
    CHECK(script_calls == 1); // The other switch is not loaded in this setup.

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x9F, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x9F, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x9F && last_state == 2);
    scripts[0x9F].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Completed presentation cannot restart.

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x12, 12);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x12, 2, 13}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x12 && last_state == 13);
    scripts[0x12].unk48[0] = 11; coop_transient_apply();
    CHECK(script_calls == 1); // An unrevealed switch is not remotely armed.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x31, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x31, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x31 && last_state == 2);
    coop_transient_apply(); CHECK(script_calls == 1); // Idempotent after activation.
    scripts[0x31].unk48[0] = 1; transient_result.records[0].state = 1;
    coop_transient_apply(); CHECK(script_calls == 1); // Ready never rewinds/triggers.
    transient_result.records[0] = {COOP_TRANSIENT_TRIGGER, 0x32, 2, 2};
    coop_transient_apply(); CHECK(script_calls == 1); // Arbitrary trigger rejected.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x24, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x24, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x24 && last_state == 2);
    scripts[0x24].unk48[0] = 0; coop_transient_apply();
    CHECK(script_calls == 1); // The peer cannot start an uninitialized cage switch.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x38, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x38, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x38 && last_state == 2);
    scripts[0x38].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // The local pad owns completion and reset timing.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x3F, 3);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x3F, 2, 5}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x3F && last_state == 5);
    scripts[0x3F].unk48[0] = 3; transient_result.records[0].value = 4;
    coop_transient_apply(); CHECK(script_calls == 1); // Wrong activation state rejected.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x14, 30);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x14, 10, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x14 && last_state == 31);
    coop_transient_apply(); CHECK(script_calls == 1); // Wait for local timer/state gate.
    scripts[0x14].unk48[0] = 32; coop_transient_apply();
    CHECK(script_calls == 2 && last_state == 33); // Catch up one more local note.
    scripts[0x14].unk48[0] = 34; transient_result.records[0].state = 9;
    coop_transient_apply(); CHECK(script_calls == 2); // Host never rewinds an ahead peer.
    transient_result.records[0].state = 26;
    coop_transient_apply(); CHECK(script_calls == 2); // Out-of-range progress rejected.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0, 3, 100);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TIMER, 0, 3, 75}}};
    coop_transient_apply();
    CHECK(!script_calls && scripts[0].unk44[0] == 75); // Same state still aligns timer.
    scripts[0].unk44[0] = 74; coop_transient_apply();
    CHECK(scripts[0].unk44[0] == 74); // Repeated render frame does not freeze countdown.
    transient_result.records[0] = {COOP_TRANSIENT_TIMER, 0, 4, 60};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_state == 4 && scripts[0].unk44[0] == 60);

    reset(); role = ROLE_JOIN; current_map = 48; load(0, 0x1A, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 48, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x1A, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x1A && last_state == 2);
    scripts[0x1A].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Permanent completion cannot be rewound.

    reset(); role = ROLE_JOIN; current_map = 48; load(0, 0x07, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 48, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x07, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x07 && last_state == 2);
    scripts[0x07].unk48[0] = 4; coop_transient_apply();
    CHECK(script_calls == 1); // The local break animation cannot restart.
    reset(); role = ROLE_JOIN; current_map = 48; load(0, 0xEB, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 48, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0xEB, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0xEB && last_state == 2);
    scripts[0xEB].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Saved completion cannot restart the sequence.

    reset(); role = ROLE_JOIN; current_map = 173; load(0, 0x10, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 173, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x10, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x10 && last_state == 2);
    transient_result.map = 178; coop_transient_apply();
    CHECK(script_calls == 1); // A lobby record cannot cross into another map.

    reset(); role = ROLE_JOIN; current_map = 34; load(0, 0x31, 2);
    transient_result = {COOP_TRANSIENT_APPLYING, 34, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x31, 2, 3}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x31 && last_state == 3);
    scripts[0x31].unk48[0] = 1; coop_transient_apply();
    CHECK(script_calls == 1); // Barrel dependency must have armed state 2 locally.

    reset(); role = ROLE_JOIN; current_map = 34; load(0, 0x33, 2);
    transient_result = {COOP_TRANSIENT_APPLYING, 34, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x33, 2, 3}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x33 && last_state == 3);
    scripts[0x33].unk48[0] = 1; coop_transient_apply();
    CHECK(script_calls == 1); // Local barrel initialization is mandatory.

    reset(); role = ROLE_JOIN; current_map = 72; load(0, 0x2F, 12);
    transient_result = {COOP_TRANSIENT_APPLYING, 72, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2F, 2, 13}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2F && last_state == 13);
    scripts[0x2F].unk48[0] = 11; coop_transient_apply();
    CHECK(script_calls == 1); // The large pad must finish its local reveal.
    reset(); role = ROLE_JOIN; current_map = 72; load(0, 0x2E, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 72, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2E, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2E && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 72; load(0, 0x1E, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 72, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x1E, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x1E && last_state == 2);
    scripts[0x1E].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // A destroyed wall cannot replay its break entry.

    reset(); role = ROLE_JOIN; current_map = 90;
    load(0, 0x03, 1); load(1, 0x04, 1); load(2, 0x05, 1); load(3, 0x06, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 90, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x04, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x04 && last_state == 2);
    CHECK(scripts[0x06].unk48[0] == 2);
    transient_result.records[0].key = 0x05; coop_transient_apply();
    CHECK(script_calls == 2 && scripts[0x06].unk48[0] == 3);
    scripts[0x06].unk48[0] = 4; transient_result.records[0].key = 0x03;
    coop_transient_apply();
    CHECK(script_calls == 2); // Completed controller cannot receive another hit.

    reset(); role = ROLE_JOIN; current_map = 56; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 56, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    reset(); role = ROLE_JOIN; current_map = 59; load(0, 0x24, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 59, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x24, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x24 && last_state == 2);
    scripts[0x24].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // Completed local box cannot be restarted.
    reset(); role = ROLE_JOIN; current_map = 61; load(0, 0x06, 10);
    transient_result = {COOP_TRANSIENT_APPLYING, 61, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x06, 2, 11}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x06 && last_state == 11);
    scripts[0x06].unk48[0] = 9; coop_transient_apply();
    CHECK(script_calls == 1); // Only exact armed state 10 may activate.
    scripts[0x06].unk48[0] = 0; coop_transient_apply();
    CHECK(script_calls == 1); // The permanent pre-activation path stays local.
    reset(); role = ROLE_JOIN; current_map = 62; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 62, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    scripts[0x00].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // Completed pad path cannot be replayed.
    reset(); role = ROLE_JOIN; current_map = 62; load(0, 0x03, 10);
    transient_result = {COOP_TRANSIENT_APPLYING, 62, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x03, 2, 11}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x03 && last_state == 11);
    scripts[0x03].unk48[0] = 9; coop_transient_apply();
    CHECK(script_calls == 1); // Question boxes require exact armed state 10.
    reset(); role = ROLE_JOIN; current_map = 17; load(0, 0x2C, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 17, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2C, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2C && last_state == 2);
    reset(); role = ROLE_JOIN; current_map = 17; load(0, 0x2F, 11);
    transient_result = {COOP_TRANSIENT_APPLYING, 17, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2F, 2, 12}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2F && last_state == 12);
    scripts[0x2F].unk48[0] = 10; coop_transient_apply();
    CHECK(script_calls == 1); // Local Helm pad setup must reach state 11.
    reset(); role = ROLE_JOIN; current_map = 17; load(0, 0x5D, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 17, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x5D, 2, 10}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x5D && last_state == 10);
    for (unsigned raw : {0u, 2u, 20u}) {
        reset(); role = ROLE_JOIN; current_map = 17; load(0, 0x5D, raw);
        transient_result = {COOP_TRANSIENT_APPLYING, 17, 9, 1,
            {{COOP_TRANSIENT_TRIGGER, 0x5D, 2, 10}}};
        coop_transient_apply(); CHECK(!script_calls);
    }
    reset(); role = ROLE_JOIN; current_map = 58; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 58, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    reset(); role = ROLE_JOIN; current_map = 57; load(0, 0x03, 10);
    transient_result = {COOP_TRANSIENT_APPLYING, 57, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x03, 2, 11}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x03 && last_state == 11);
    scripts[0x03].unk48[0] = 9; coop_transient_apply();
    CHECK(script_calls == 1); // Only exact winch-ready state 10 may activate.
    scripts[0x03].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Saved completion cannot restart the winch.

    reset(); role = ROLE_JOIN; current_map = 108; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 108, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    transient_result.records[0].key = 0x01; coop_transient_apply();
    CHECK(script_calls == 1); // The linked door is not a trigger.

    reset(); role = ROLE_JOIN; current_map = 112; load(0, 0x0E, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 112, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x0E && last_state == 2);
    scripts[0x0E].unk48[0] = 5; coop_transient_apply();
    CHECK(script_calls == 1); // Local reset/door states are never overwritten.

    reset(); role = ROLE_JOIN; current_map = 163; load(0, 0x05, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 163, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x05, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x05 && last_state == 2);
    scripts[0x05].unk48[0] = 4; coop_transient_apply();
    CHECK(script_calls == 1); // A running door sequence is never rewound.

    reset(); role = ROLE_JOIN; current_map = 164; load(0, 0x09, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 164, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x09, 2, 5}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x09 && last_state == 5);
    scripts[0x09].unk48[0] = 2; coop_transient_apply();
    CHECK(script_calls == 1); // Only the reviewed ready state may enter state 5.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x15, 0);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x15, 2, 1}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x15 && last_state == 1);
    scripts[0x15].unk48[0] = 2; coop_transient_apply();
    CHECK(script_calls == 1); // Active grate animation is never rewound.

    reset(); role = ROLE_JOIN; current_map = 4; load(0, 0x0A, 1); load(1, 0x0B, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 4, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x0A, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 2 && last_object == 0x0A && last_state == 2);
    CHECK(scripts[0x0B].unk48[0] == 10);
    coop_transient_apply();
    CHECK(script_calls == 2); // The same hit cannot replay either switch.
    reset(); role = ROLE_JOIN; current_map = 4; load(0, 0x06, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 4, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x06, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x06 && last_state == 2);
    scripts[0x06].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Collected reward path cannot be restarted.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0x13, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x13, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x13 && last_state == 2);
    coop_transient_apply(); CHECK(script_calls == 1); // Local pad sequence owns later states.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 8, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 8, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 8 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x16, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x16, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x16 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x23, 11);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x23, 2, 12}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x23 && last_state == 12);
    coop_transient_apply(); CHECK(script_calls == 1); // A held record cannot replay the hit.
    scripts[0x23].unk48[0] = 10; coop_transient_apply();
    CHECK(script_calls == 1); // The network cannot arm an uninitialized head.
    scripts[0x23].unk48[0] = 11; transient_result.records[0].value = 11;
    coop_transient_apply(); CHECK(script_calls == 1); // Pinned activation is mandatory.

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x69, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x69, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x69 && last_state == 2);
    scripts[0x69].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Completed tunnel switches never rewind.

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x6B, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x6B, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x6B && last_state == 2);
    scripts[0x6B].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // The local timed cycle cannot be restarted.

    reset(); role = ROLE_JOIN; current_map = 19; load(0, 0x04, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 19, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x04, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x04 && last_state == 2);
    scripts[0x04].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // Panel animation states remain locally owned.

    reset(); role = ROLE_JOIN; current_map = 24; load(0, 0x07, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 24, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x07, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x07 && last_state == 2);
    transient_result.records[0].key = 0x06; coop_transient_apply();
    CHECK(script_calls == 1); // A linked wall panel is not an allowed trigger.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0x2F, 12);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2F, 2, 13}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2F && last_state == 13);
    scripts[0x2F].unk48[0] = 11; coop_transient_apply();
    CHECK(script_calls == 1); // An unexposed target cannot be hit remotely.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0x21, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x21, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x21 && last_state == 2);
    scripts[0x21].unk48[0] = 4; coop_transient_apply();
    CHECK(script_calls == 1); // Completion cannot replay the charge sequence.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0x3F, 0);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x3F, 2, 1}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x3F && last_state == 1);
    scripts[0x3F].unk48[0] = 2; coop_transient_apply();
    CHECK(script_calls == 1); // A broken gate never replays its punch entry.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x13C, 0);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x13C, 2, 1}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x13C && last_state == 1);
    scripts[0x13C].unk48[0] = 2; coop_transient_apply();
    CHECK(script_calls == 1); // A breaking grate cannot restart.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x140, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x140, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x140 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 16; load(0, 4, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 16, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 4, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 4 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 16;
    load(0, 0x0C, 1); load(1, 0x0D, 1); load(2, 0x0E, 1); load(3, 0x0F, 10);
    transient_result = {COOP_TRANSIENT_APPLYING, 16, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x0C, 3, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x0F && last_state == 11);
    coop_transient_apply(); CHECK(script_calls == 1); // Local chain must advance first.
    scripts[0x0F].unk48[0] = 22; scripts[0x0E].unk48[0] = 10;
    coop_transient_apply();
    CHECK(script_calls == 2 && last_object == 0x0E && last_state == 11);
    scripts[0x0E].unk48[0] = 1; scripts[0x0D].unk48[0] = 10;
    transient_result.records[0].state = 4; coop_transient_apply();
    CHECK(script_calls == 3 && last_object == 0x0D && last_state == 11);
    scripts[0x0D].unk48[0] = 1; scripts[0x0C].unk48[0] = 10;
    coop_transient_apply();
    CHECK(script_calls == 4 && last_object == 0x0C && last_state == 11);

    reset(); role = ROLE_JOIN; current_map = 16;
    load(0, 0x0C, 1); load(1, 0x0D, 1); load(2, 0x0E, 1); load(3, 0x0F, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 16, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x0C, 1, 0}}};
    coop_transient_apply(); CHECK(!script_calls); // Cannot start the puzzle remotely.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x7F, 17);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x7F, 4, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x7F && last_state == 54);
    scripts[0x7F].unk48[0] = 19; transient_result.records[0].state = 6;
    coop_transient_apply();
    CHECK(script_calls == 2 && last_state == 58); // Exactly the next hit, no skip.
    scripts[0x7F].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 3 && last_state == 23); // Vanilla final-hit entry.
    scripts[0x7F].unk48[0] = 23; coop_transient_apply();
    CHECK(script_calls == 3); // Completion is idempotent.

    reset(); role = ROLE_JOIN; current_map = 84; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 84, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x00, 4, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    coop_transient_apply();
    CHECK(script_calls == 1); // Wait for the local intermediate states.
    scripts[0x00].unk48[0] = 4; coop_transient_apply();
    CHECK(script_calls == 2 && last_state == 5);
    scripts[0x00].unk48[0] = 6; coop_transient_apply();
    CHECK(script_calls == 3 && last_state == 7);
    scripts[0x00].unk48[0] = 8; coop_transient_apply();
    CHECK(script_calls == 4 && last_state == 9);
    scripts[0x00].unk48[0] = 9; coop_transient_apply();
    CHECK(script_calls == 4); // Reward states are entered only by the local script.
    scripts[0x00].unk48[0] = 50; coop_transient_apply();
    CHECK(script_calls == 4); // A failure branch cannot be advanced remotely.

    reset(); role = ROLE_JOIN; current_map = MAP_JAPES_SHELL; load_clam(0, 0, 70);
    transient_result = {COOP_TRANSIENT_APPLYING, MAP_JAPES_SHELL, 9, 1,
        {{COOP_TRANSIENT_ACTOR_CYCLE, 1, 2, 33}}};
    coop_transient_apply();
    CHECK(actors[0].control_state == 1 && !actors[0].control_state_progress);
    CHECK(animation_calls == 1 && last_animation == 0x35D && motion_calls == 1);
    coop_transient_apply();
    CHECK(animation_calls == 1 && motion_calls == 1); // Opening animation cannot be skipped.
    actors[0].control_state = 2; actor_data[0].timer = 80;
    coop_transient_apply();
    CHECK(actor_data[0].timer == 33); // Stable open countdown follows the host.
    transient_result.records[0] = {COOP_TRANSIENT_ACTOR_CYCLE, 1, 0, 22};
    coop_transient_apply();
    CHECK(actors[0].control_state == 3 && actors[0].unk132 == 2 && motion_calls == 2);
    coop_transient_apply(); CHECK(motion_calls == 2); // Closing animation cannot be skipped.
    actors[0].control_state = 0; actor_data[0].timer = 80;
    coop_transient_apply(); CHECK(actor_data[0].timer == 22);
    actors[0].unk58 = 0; transient_result.records[0].state = 2;
    coop_transient_apply(); CHECK(animation_calls == 1); // Wrong actor type fails closed.

    reset(); role = ROLE_JOIN; current_map = MAP_CAVES_ICE_CASTLE; load_tomato(3);
    D_global_asm_807FC8C0[0] = 0;
    unsigned wanted = 0;
    for (unsigned i = 0; i < 16; ++i) wanted |= ((i & 1) ? 2u : 1u) << (i * 2);
    transient_result = {COOP_TRANSIENT_APPLYING, MAP_CAVES_ICE_CASTLE, 9, 1,
        {{COOP_TRANSIENT_TOMATO_BOARD, 0, 1, wanted}}};
    coop_transient_apply();
    for (unsigned i = 0; i < 16; ++i)
        CHECK(D_global_asm_807FC8C0[i] == static_cast<s8>(i & 1));
    CHECK(tile_activation_calls == 15); // Cell zero already matched the host.
    unsigned activations = tile_activation_calls;
    coop_transient_apply(); CHECK(tile_activation_calls == activations); // Idempotent snapshot.
    transient_result.records[0].value = 3; coop_transient_apply();
    CHECK(tile_activation_calls == activations); // Reserved cell encoding rejects the whole record.
    transient_result.records[0].value = wanted; tomato_data[0x38] = 2;
    D_global_asm_807FC8C0[1] = -1; coop_transient_apply();
    CHECK(D_global_asm_807FC8C0[1] == -1); // A packet cannot start or resume the encounter.
    tomato_data[0x38] = 3; tomato_board_active = false; coop_transient_apply();
    CHECK(D_global_asm_807FC8C0[1] == -1);

    reset(); role = ROLE_JOIN; current_map = MAP_CAVES_ICE_CASTLE; load_tomato(4);
    tomato_clock_data.elapsed = 17; tomato_clock_data.duration = 60;
    transient_result = {COOP_TRANSIENT_APPLYING, MAP_CAVES_ICE_CASTLE, 9, 1,
        {{COOP_TRANSIENT_TOMATO_CLOCK, 0, 1, 25}}};
    coop_transient_apply();
    CHECK(tomato_clock_data.duration == 42 && actors[1].control_state == 2);
    tomato_clock_data.elapsed = 18; coop_transient_apply();
    CHECK(tomato_clock_data.duration == 42); // Persisted result is evaluated once.
    transient_result.records[0].value = 24; coop_transient_apply();
    CHECK(tomato_clock_data.duration == 42); // New sample preserves 24 seconds remaining.
    transient_result.records[0] = {COOP_TRANSIENT_TOMATO_CLOCK, 0, 2, 0};
    coop_transient_apply();
    CHECK(actors[1].control_state == 5 && actors[1].control_state_progress == 0);
    actors[1].control_state = 2; tomato_data[0x38] = 2; coop_transient_apply();
    CHECK(actors[1].control_state == 2); // A packet cannot start or resume the encounter.
}

static void cutscene_checks() {
    reset(); current_map = 48; is_cutscene_active = 1;
    D_global_asm_807476F8 = 12; D_global_asm_807F5CF0 = 3; D_global_asm_807F5CF4 = 0x41;
    coop_transient_capture(1);
    CHECK(contains(COOP_TRANSIENT_CUTSCENE, 13, 3));
    is_cutscene_active = 0;
    coop_transient_capture(1);
    CHECK(contains(COOP_TRANSIENT_CUTSCENE, 13, 3)); // Same-epoch target survives host completion.
    epoch++;
    coop_transient_capture(1);
    CHECK(!contains(COOP_TRANSIENT_CUTSCENE, 13, 3)); // Never retain across a room epoch.
    epoch--;

    reset(); current_map = 7; is_cutscene_active = 1;
    D_global_asm_807476F8 = 12; D_global_asm_807F5CF0 = 3; D_global_asm_807F5CF4 = 0x41;
    unsigned slot = 0;
    for (const auto& entry : coop_transient_extra_objects) if (entry.map == 7)
        load(slot++, entry.object, entry.activation);
    bool saw_full_cutscene_page = false;
    for (unsigned page = 0; page < 16; ++page) {
        coop_transient_capture(1);
        saw_full_cutscene_page |= transient_input.count == COOP_TRANSIENT_RECORDS
            && contains(COOP_TRANSIENT_CUTSCENE, 13, 3);
    }
    CHECK(saw_full_cutscene_page); // Cutscene target has priority on a full page.

    role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_CUTSCENE, 13, 4, 0x41}}};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 4);
    transient_result.records[0].state = 6;
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 5); // Catch up one phase per frame.
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 6);
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 6); // Never advance past the host.
    transient_result.records[0].state = 5;
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 6); // Never rewind camera phases.
    transient_result.records[0] = {COOP_TRANSIENT_CUTSCENE, 14, 7, 0x41};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 6); // Wrong cutscene ID.
    transient_result.records[0] = {COOP_TRANSIENT_CUTSCENE, 13, 7, 0x40};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 6); // Wrong control flags.
    is_cutscene_active = 0; transient_result.records[0] = {COOP_TRANSIENT_CUTSCENE, 13, 7, 0x41};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 6); // A packet cannot start one.
}

static void lobby_instrument_pad_checks() {
    struct Pad { unsigned map, first, count; } groups[] = {
        {173, 0x00, 4}, {174, 0x04, 5}, {175, 0x00, 5},
        {193, 0x02, 5}, {194, 0x10, 5},
    };
    unsigned total = 0;
    for (const auto& group : groups) {
        reset(); current_map = group.map;
        for (unsigned i = 0; i < group.count; ++i) {
            unsigned object = group.first + i;
            CHECK(coop_lobby_instrument_pad(group.map, object));
            load(i, object, 1); ++total;
        }
        coop_transient_capture(1);
        CHECK(transient_input.count == group.count);
        for (unsigned i = 0; i < group.count; ++i)
            CHECK(contains_value(COOP_TRANSIENT_TRIGGER, group.first + i, 1, 2));
        for (unsigned i = 0; i < group.count; ++i)
            scripts[group.first + i].unk48[0] = 2;
        coop_transient_capture(1);
        for (unsigned i = 0; i < group.count; ++i)
            CHECK(contains_value(COOP_TRANSIENT_TRIGGER, group.first + i, 2, 2));
    }
    CHECK(total == 24);
    CHECK(!coop_lobby_instrument_pad(173, 0x04)); // Complex lobby controller.
    CHECK(!coop_lobby_instrument_pad(175, 0x0C)); // Permanent Factory lever.
    CHECK(!coop_lobby_instrument_pad(178, 0x00)); // Fungi gun-order controller.

    // Every reviewed key accepts only the exact raw state-1 activation edge.
    for (const auto& group : groups) for (unsigned i = 0; i < group.count; ++i) {
        reset(); role = ROLE_JOIN; current_map = group.map;
        unsigned object = group.first + i; load(0, object, 1);
        transient_result = {COOP_TRANSIENT_APPLYING, group.map, epoch, 1,
            {{COOP_TRANSIENT_TRIGGER, object, 2, 2}}};
        coop_transient_apply();
        CHECK(script_calls == 1 && last_object == object && last_state == 2);
        for (unsigned raw : {0u, 2u, 3u, 4u, 20u}) {
            scripts[object].unk48[0] = static_cast<u8>(raw);
            coop_transient_apply(); CHECK(script_calls == 1);
        }
        scripts[object].unk48[0] = 1;
        coop_transient_apply(); CHECK(script_calls == 1); // Replayed fired pulse cannot restart.
        transient_result.status = COOP_TRANSIENT_SYNCED;
        transient_result.count = 0; coop_transient_apply();
        transient_result.status = COOP_TRANSIENT_APPLYING;
        transient_result.count = 1; coop_transient_apply();
        CHECK(script_calls == 2); // A later native edge may start a new local loop.
    }

    // Caves pads exist only while the local pressure-switch flag makes them
    // available. A network record cannot create the unavailable state.
    reset(); role = ROLE_JOIN; current_map = 194; load(0, 0x10, 1);
    caves_pad_available = false;
    transient_result = {COOP_TRANSIENT_APPLYING, 194, epoch, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x10, 2, 2}}};
    coop_transient_apply(); CHECK(!script_calls && scripts[0x10].unk48[0] == 1);
    caves_pad_available = true; coop_transient_apply(); CHECK(script_calls == 1);

    // Reviewed triggers are intentionally accepted by either role. Stale room
    // context, transitions and a mismatched activation value still reject it.
    for (unsigned receiver_role : {ROLE_HOST, ROLE_JOIN}) {
        reset(); role = receiver_role; current_map = 173; load(0, 0, 1);
        transient_result = {COOP_TRANSIENT_APPLYING, 173, epoch, 1,
            {{COOP_TRANSIENT_TRIGGER, 0, 2, 2}}};
        coop_transient_apply(); CHECK(script_calls == 1 && scripts[0].unk48[0] == 2);
    }
    for (unsigned scenario = 0; scenario < 4; ++scenario) {
        reset(); role = ROLE_JOIN; current_map = 173; load(0, 0, 1);
        transient_result = {COOP_TRANSIENT_APPLYING, 173, epoch, 1,
            {{COOP_TRANSIENT_TRIGGER, 0, 2, 2}}};
        if (scenario == 0) transient_result.epoch--;
        if (scenario == 1) transient_result.map++;
        if (scenario == 2) loading_zone_transition_speed = 1.0f;
        if (scenario == 3) transient_result.records[0].value = 3;
        coop_transient_apply(); CHECK(!script_calls && scripts[0].unk48[0] == 1);
    }
}

static void kosh_checks() {
    for (unsigned key = 1; key <= 4; ++key) {
        reset(); load_kosh(key);
        actors[0].control_state = 1; actors[2].control_state = 0x44;
        D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER]();
        CHECK(kosh_original_calls == 1 && !kosh_success_calls && actors[0].control_state == 2);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_MINIGAME_SUCCESS, key, 1, 0));

        for (unsigned receiver_role : {ROLE_HOST, ROLE_JOIN}) {
            reset(); load_kosh(key); role = receiver_role;
            transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
                {{COOP_TRANSIENT_MINIGAME_SUCCESS, key, 1, 0}}};
            coop_transient_apply();
            CHECK(!kosh_success_calls && coop_kosh_pending_key == key);
            D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER]();
            CHECK(kosh_success_calls == 1 && kosh_success_arg == 1 && kosh_success_text == 0xE);
            CHECK(kosh_original_calls == 1 && actors[0].control_state == 2
                && actors[2].control_state == 0x44);
            coop_transient_apply();
            D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER]();
            CHECK(kosh_success_calls == 1); // Persistent result is idempotent.
            coop_transient_capture(1);
            CHECK(contains_value(COOP_TRANSIENT_MINIGAME_SUCCESS, key, 1, 0));
        }
    }

    // Exact record, room epoch and loading context are mandatory before a
    // remote success can even become pending.
    reset(); load_kosh(1); role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_MINIGAME_SUCCESS, 1, 1, 0}}};
    for (unsigned scenario = 0; scenario < 6; ++scenario) {
        coop_kosh_pending_key = 0;
        auto saved = transient_result;
        if (scenario == 0) transient_result.records[0].key = 2;
        if (scenario == 1) transient_result.records[0].state = 2;
        if (scenario == 2) transient_result.records[0].value = 1;
        if (scenario == 3) transient_result.epoch = epoch - 1;
        if (scenario == 4) transient_result.map = current_map + 1;
        if (scenario == 5) loading_zone_transition_speed = 1.0f;
        coop_transient_apply(); CHECK(!coop_kosh_pending_key);
        transient_result = saved; loading_zone_transition_speed = 0.0f;
    }

    // Every controller-side identity check fails closed and still delegates to
    // the original behavior. No score, timer, flag, or transition is written.
    for (unsigned scenario = 0; scenario < 8; ++scenario) {
        reset(); load_kosh(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_MINIGAME_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); CHECK(coop_kosh_pending_key == 1);
        if (scenario == 0) kosh_parent = 25;
        if (scenario == 1) character_spawners[0].unkA_u8[0] = 19;
        if (scenario == 2) character_spawners[1].pad0[0] = 3;
        if (scenario == 3) ((CoopKoshData*)actors[0].unk178)->intro = 1;
        if (scenario == 4) D_global_asm_807FBB34 = 0;
        if (scenario == 5) actors[0].object_properties_bitfield = 0;
        if (scenario == 6) actors[0].unk11C = nullptr;
        if (scenario == 7) D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = func_bonus_80024158;
        coop_kosh_behavior();
        CHECK(!kosh_success_calls && kosh_original_calls == 1 && actors[0].control_state == 0);
    }

    reset(); load_kosh(1);
    actors[0].control_state = 1; actors[2].control_state = 0x43;
    D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER]();
    coop_transient_capture(1);
    CHECK(!contains(COOP_TRANSIENT_MINIGAME_SUCCESS, 1, 1)); // Failure never propagates.

    for (unsigned scenario = 0; scenario < 2; ++scenario) {
        reset(); load_kosh(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_MINIGAME_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); CHECK(coop_kosh_pending_key == 1);
        if (!scenario) loading_zone_transition_speed = 1.0f;
        else gPlayerPointer = nullptr;
        D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER]();
        CHECK(!kosh_success_calls && !coop_kosh_pending_key && kosh_original_calls == 1);
    }

    reset(); transient_enabled = 0; coop_kosh_hook = 0;
    D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = func_bonus_80024158;
    coop_transient_init();
    CHECK(!coop_kosh_hook && D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] == func_bonus_80024158);
}

static void minecart_checks() {
    // Every vanilla identity publishes a natural local win, and both peers can
    // consume the event from each of the three genuinely active states.
    for (unsigned key = 1; key <= 3; ++key) {
        reset(); load_minecart(key); actors[0].control_state = 1;
        minecart_original_mode = 1;
        D_global_asm_8074C0A0[ACTOR_MINECART_BONUS]();
        CHECK(minecart_original_calls == 1 && minecart_cleanup_calls == 1
            && minecart_success_calls == 1 && actors[0].control_state == 5);
        CHECK(actors[1].control_state == 0 && actors[2].control_state == 0x44);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_MINECART_SUCCESS, key, 1, 0));

        for (unsigned receiver_role : {ROLE_HOST, ROLE_JOIN}) for (unsigned state = 1; state <= 3; ++state) {
            reset(); load_minecart(key); role = receiver_role; actors[0].control_state = state;
            transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
                {{COOP_TRANSIENT_MINECART_SUCCESS, key, 1, 0}}};
            coop_transient_apply();
            CHECK(!minecart_success_calls && coop_minecart_pending_key == key);
            D_global_asm_8074C0A0[ACTOR_MINECART_BONUS]();
            CHECK(minecart_original_calls == 1 && minecart_cleanup_calls == 1
                && minecart_success_calls == 1 && minecart_success_arg == 1
                && minecart_success_text == 0xE);
            CHECK(minecart_order_count == 3 && minecart_order[0] == 'O'
                && minecart_order[1] == 'C' && minecart_order[2] == 'S');
            CHECK(actors[0].control_state == 5 && actors[1].control_state == 0
                && actors[2].control_state == 0x44 && !coop_minecart_pending_key);
            coop_transient_apply();
            D_global_asm_8074C0A0[ACTOR_MINECART_BONUS]();
            CHECK(minecart_success_calls == 1); // Persistent result is idempotent.
            coop_transient_capture(1);
            CHECK(contains_value(COOP_TRANSIENT_MINECART_SUCCESS, key, 1, 0));
        }
    }

    // The native result cannot queue a different identity or malformed event.
    reset(); load_minecart(1); role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
    for (unsigned scenario = 0; scenario < 6; ++scenario) {
        coop_minecart_pending_key = 0;
        auto saved = transient_result;
        if (scenario == 0) transient_result.records[0].key = 2;
        if (scenario == 1) transient_result.records[0].key = 4;
        if (scenario == 2) transient_result.records[0].state = 2;
        if (scenario == 3) transient_result.records[0].value = 1;
        if (scenario == 4) transient_result.epoch--;
        if (scenario == 5) transient_result.map++;
        coop_transient_apply(); CHECK(!coop_minecart_pending_key);
        transient_result = saved;
    }
    loading_zone_transition_speed = 1.0f;
    coop_transient_apply(); CHECK(!coop_minecart_pending_key);

    // Each controller identity/pointer invariant fails closed, clears the
    // queued success, and still calls the stock controller exactly once.
    for (unsigned scenario = 0; scenario < 18; ++scenario) {
        reset(); load_minecart(1); role = ROLE_JOIN; actors[0].control_state = 1;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); CHECK(coop_minecart_pending_key == 1);
        if (scenario == 0) ++current_map;
        if (scenario == 1) kosh_parent = 8;
        if (scenario == 2) kosh_exit = 1;
        if (scenario == 3) current_character_index[0] = 3;
        if (scenario == 4) actors[0].unk58 = ACTOR_MINIGAME_CONTROLLER;
        if (scenario == 5) actors[0].object_properties_bitfield = 0;
        if (scenario == 6) actors[0].additional_actor_data = nullptr;
        if (scenario == 7) actors[0].animation_state = nullptr;
        if (scenario == 8) actors[0].unk11C = nullptr;
        if (scenario == 9) gPlayerPointer = nullptr;
        if (scenario == 10) actors[2].additional_actor_data = nullptr;
        if (scenario == 11) D_global_asm_807FBB34 = 0;
        if (scenario == 12) D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = func_minecart_80024FD0;
        if (scenario == 13) minecart_original_mode = 5;
        if (scenario == 14) minecart_original_mode = 6;
        if (scenario == 15) kosh_parent_valid = false;
        if (scenario == 16) D_global_asm_807FB930[1].actor = nullptr;
        if (scenario == 17) minecart_original_mode = 7;
        coop_minecart_behavior();
        CHECK(minecart_original_calls == 1 && !minecart_success_calls
            && !minecart_cleanup_calls && !coop_minecart_pending_key);
    }

    // Intro/transition states cannot consume the event in the same callback.
    // Once stock code advances 4 or 10 to active state 3, the next frame may.
    for (unsigned state : {4u, 10u}) {
        reset(); load_minecart(1); role = ROLE_JOIN; actors[0].control_state = state;
        minecart_original_mode = state == 4 ? 3 : 4;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); coop_minecart_behavior();
        CHECK(actors[0].control_state == 3 && coop_minecart_pending_key == 1
            && !minecart_success_calls);
        minecart_original_mode = 0; coop_minecart_behavior();
        CHECK(actors[0].control_state == 5 && minecart_success_calls == 1);
    }
    for (unsigned state : {0u, 5u, 6u}) {
        reset(); load_minecart(1); role = ROLE_JOIN; actors[0].control_state = state;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); coop_minecart_behavior();
        CHECK(!minecart_success_calls && !minecart_cleanup_calls && !coop_minecart_pending_key);
    }

    // A local failure wins the race against a queued remote success. The same
    // applies to the exact failure-animation sentinel before stock transitions.
    reset(); load_minecart(1); role = ROLE_JOIN; actors[0].control_state = 1;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); minecart_original_mode = 2; coop_minecart_behavior();
    CHECK(actors[0].control_state == 5 && actors[2].control_state == 0x43
        && !minecart_success_calls && !minecart_cleanup_calls && !coop_minecart_pending_key);
    coop_transient_capture(1);
    CHECK(!contains(COOP_TRANSIENT_MINECART_SUCCESS, 1, 1));

    reset(); load_minecart(1); role = ROLE_JOIN; actors[0].control_state = 2;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); minecart_animation.unk64 = 0x292; coop_minecart_behavior();
    CHECK(!minecart_success_calls && !minecart_cleanup_calls && !coop_minecart_pending_key);

    // Natural success performs stock cleanup/helper once and is merely
    // observed by the wrapper, even when a remote event was already queued.
    reset(); load_minecart(1); role = ROLE_JOIN; actors[0].control_state = 2;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); minecart_original_mode = 1; coop_minecart_behavior();
    CHECK(minecart_original_calls == 1 && minecart_cleanup_calls == 1
        && minecart_success_calls == 1 && minecart_order_count == 3
        && !coop_minecart_pending_key);

    // Queued events are discarded on every stale lifecycle boundary.
    for (unsigned scenario = 0; scenario < 4; ++scenario) {
        reset(); load_minecart(1); role = ROLE_JOIN; actors[0].control_state = 1;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_MINECART_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); CHECK(coop_minecart_pending_key == 1);
        if (scenario == 0) loading_zone_transition_speed = 1.0f;
        if (scenario == 1) transient_file_changed = 1;
        if (scenario == 2) gPlayerPointer = nullptr;
        if (scenario == 3) ++epoch;
        coop_minecart_behavior();
        CHECK(!minecart_success_calls && !minecart_cleanup_calls && !coop_minecart_pending_key);
    }

    // Either hook conflict disables only that feature; the other minigame
    // adapter remains available. A globally disabled transient feature hooks neither.
    reset(); D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = func_bonus_80024158;
    D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = nullptr;
    coop_kosh_hook = coop_minecart_hook = 0; coop_transient_init();
    CHECK(coop_kosh_hook && !coop_minecart_hook);
    reset(); D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = nullptr;
    D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = func_minecart_80024FD0;
    coop_kosh_hook = coop_minecart_hook = 0; coop_transient_init();
    CHECK(!coop_kosh_hook && coop_minecart_hook);
    reset(); transient_enabled = 0; coop_kosh_hook = coop_minecart_hook = 0;
    D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = func_bonus_80024158;
    D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = func_minecart_80024FD0;
    coop_transient_init();
    CHECK(!coop_kosh_hook && !coop_minecart_hook);
}

static void rabbit_checks() {
    // Each round publishes the same exact natural-success edge under its own
    // key. The wrapper observes stock code and never performs outcome work.
    for (unsigned key = 1; key <= 2; ++key) {
        reset(); load_rabbit(key); rabbit_original_mode = 1;
        D_global_asm_8074C0A0[ACTOR_RABBIT_RACE]();
        CHECK(rabbit_original_calls == 1 && actors[0].control_state == 0x28
            && actors[0].control_state_progress == 0 && !rabbit_outcome_calls
            && !coop_rabbit_pending_key);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_RABBIT_SUCCESS, key, 1, 0));
    }

    // Host and Join can consume either round. Injection only writes the stock
    // terminal state; the next ordinary call owns flags/scripts/presentation.
    for (unsigned receiver_role : {ROLE_HOST, ROLE_JOIN}) for (unsigned key = 1; key <= 2; ++key) {
        reset(); load_rabbit(key); role = receiver_role;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_RABBIT_SUCCESS, key, 1, 0}}};
        coop_transient_apply();
        CHECK(coop_rabbit_pending_key == key && !rabbit_original_calls);
        D_global_asm_8074C0A0[ACTOR_RABBIT_RACE]();
        CHECK(rabbit_original_calls == 1 && actors[0].control_state == 0x28
            && actors[0].control_state_progress == 0 && !rabbit_outcome_calls
            && !script_calls && !coop_rabbit_pending_key);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_RABBIT_SUCCESS, key, 1, 0));
        D_global_asm_8074C0A0[ACTOR_RABBIT_RACE]();
        CHECK(rabbit_original_calls == 2 && rabbit_outcome_calls == 1
            && actors[0].control_state == 0x37);
        CHECK(key == 1 ? (rabbit_first_complete && !rabbit_gb_owned)
                       : (rabbit_first_complete && rabbit_gb_owned));
        coop_transient_apply();
        D_global_asm_8074C0A0[ACTOR_RABBIT_RACE]();
        CHECK(rabbit_outcome_calls == 1 && !coop_rabbit_pending_key);
    }

    // Malformed, stale and wrong-round results never queue an event.
    for (unsigned scenario = 0; scenario < 8; ++scenario) {
        reset(); load_rabbit(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_RABBIT_SUCCESS, 1, 1, 0}}};
        if (scenario == 0) transient_result.records[0].key = 0;
        if (scenario == 1) transient_result.records[0].key = 2;
        if (scenario == 2) transient_result.records[0].key = 3;
        if (scenario == 3) transient_result.records[0].state = 2;
        if (scenario == 4) transient_result.records[0].value = 1;
        if (scenario == 5) transient_result.epoch--;
        if (scenario == 6) transient_result.map++;
        if (scenario == 7) loading_zone_transition_speed = 1.0f;
        coop_transient_apply();
        CHECK(!coop_rabbit_pending_key);
    }
    reset(); load_rabbit(2); role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_RABBIT_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); CHECK(!coop_rabbit_pending_key);
    for (unsigned key = 1; key <= 2; ++key) {
        reset(); load_rabbit(key); role = ROLE_JOIN; rabbit_gb_owned = true;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_RABBIT_SUCCESS, key, 1, 0}}};
        coop_transient_apply(); CHECK(!coop_rabbit_pending_key);
    }

    // Every actor, player, race, script and round identity gate fails closed.
    // Post-original unload/replacement/generation cases also prove the saved
    // actor is revalidated before the wrapper touches its state.
    for (unsigned scenario = 0; scenario < 25; ++scenario) {
        unsigned key = scenario == 16 ? 2 : 1;
        reset(); load_rabbit(key); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_RABBIT_SUCCESS, key, 1, 0}}};
        coop_transient_apply(); CHECK(coop_rabbit_pending_key == key);
        if (scenario == 0) ++current_map;
        if (scenario == 1) current_character_index[0] = 1;
        if (scenario == 2) actors[0].unk58 = ACTOR_MINIGAME_CONTROLLER;
        if (scenario == 3) actors[0].object_properties_bitfield = 0;
        if (scenario == 4) actors[0].animation_state = nullptr;
        if (scenario == 5) gPlayerPointer = nullptr;
        if (scenario == 6) extra_player_info_pointer = nullptr;
        if (scenario == 7) rabbit_player_info.unk1F0 = 0x100000;
        if (scenario == 8) rabbit_player_info.unk1F4 = 0;
        if (scenario == 9) D_global_asm_807FBB64 = 0;
        if (scenario == 10) D_global_asm_8074C0A0[ACTOR_RABBIT_RACE] = func_global_asm_806BE8BC;
        if (scenario == 11) D_global_asm_807FB930[0].actor = nullptr;
        if (scenario == 12) rabbit_original_mode = 4;
        if (scenario == 13) rabbit_original_mode = 3;
        if (scenario == 14) props[0x1F].unk7C = nullptr;
        if (scenario == 15) scripts[0x1F].unk48[0] = 2;
        if (scenario == 16) props[0x57].unk7C = nullptr;
        if (scenario == 17) rabbit_first_complete = true;
        if (scenario == 18) rabbit_gb_owned = true;
        if (scenario == 19) rabbit_original_mode = 5;
        if (scenario == 20) rabbit_original_mode = 8;
        if (scenario == 21) rabbit_original_mode = 6;
        if (scenario == 22) rabbit_original_mode = 7;
        if (scenario == 23) actors[2].additional_actor_data = nullptr;
        if (scenario == 24) rabbit_original_mode = 10;
        coop_rabbit_behavior();
        CHECK(rabbit_original_calls == 1 && !rabbit_outcome_calls
            && !coop_rabbit_pending_key);
        if (scenario != 10) CHECK(actors[0].control_state != 0x28);
    }

    // No inactive, introductory, failure, outcome or cleanup state consumes a
    // queued success, even though the stock handler is still called once.
    for (unsigned state : {0u, 0x13u, 0x1Eu, 0x1Fu, 0x27u, 0x28u, 0x37u, 0x40u}) {
        reset(); load_rabbit(1); role = ROLE_JOIN;
        actors[0].control_state = static_cast<u8>(state); rabbit_original_mode = 9;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_RABBIT_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); coop_rabbit_behavior();
        CHECK(rabbit_original_calls == 1 && actors[0].control_state == state
            && !rabbit_outcome_calls && !coop_rabbit_pending_key);
    }

    // A local failure always wins over an already queued remote success.
    reset(); load_rabbit(1); role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_RABBIT_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); rabbit_original_mode = 2; coop_rabbit_behavior();
    CHECK(rabbit_original_calls == 1 && actors[0].control_state == 0x27
        && !coop_rabbit_pending_key);
    coop_transient_capture(1);
    CHECK(!contains(COOP_TRANSIENT_RABBIT_SUCCESS, 1, 1));

    // Natural success also wins over a queued packet and is latched once.
    reset(); load_rabbit(2); role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_RABBIT_SUCCESS, 2, 1, 0}}};
    coop_transient_apply(); rabbit_original_mode = 1; coop_rabbit_behavior();
    CHECK(rabbit_original_calls == 1 && actors[0].control_state == 0x28
        && !rabbit_outcome_calls && !coop_rabbit_pending_key);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_RABBIT_SUCCESS, 2, 1, 0));

    // All stale lifecycle boundaries clear a queued event before application.
    for (unsigned scenario = 0; scenario < 4; ++scenario) {
        reset(); load_rabbit(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_RABBIT_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); CHECK(coop_rabbit_pending_key == 1);
        if (scenario == 0) loading_zone_transition_speed = 1.0f;
        if (scenario == 1) transient_file_changed = 1;
        if (scenario == 2) gPlayerPointer = nullptr;
        if (scenario == 3) ++epoch;
        coop_rabbit_behavior();
        CHECK(rabbit_original_calls == 1 && actors[0].control_state == 2
            && !coop_rabbit_pending_key);
    }

    // A Rabbit handler conflict disables only Rabbit; global transient Off
    // installs none of the three reviewed minigame/race hooks.
    reset();
    D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = func_bonus_80024158;
    D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = func_minecart_80024FD0;
    D_global_asm_8074C0A0[ACTOR_RABBIT_RACE] = nullptr;
    coop_kosh_hook = coop_minecart_hook = coop_rabbit_hook = 0; coop_transient_init();
    CHECK(coop_kosh_hook && coop_minecart_hook && !coop_rabbit_hook);
    reset(); transient_enabled = 0;
    coop_kosh_hook = coop_minecart_hook = coop_rabbit_hook = 0;
    D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = func_bonus_80024158;
    D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = func_minecart_80024FD0;
    D_global_asm_8074C0A0[ACTOR_RABBIT_RACE] = func_global_asm_806BE8BC;
    coop_transient_init();
    CHECK(!coop_kosh_hook && !coop_minecart_hook && !coop_rabbit_hook);
}

static void batty_checks() {
    // All three reviewed vanilla identities publish only their exact natural
    // terminal-success edge. The stock helper/postlude runs once.
    for (unsigned key = 1; key <= 3; ++key) {
        reset(); load_batty(key); actors[0].control_state = 7;
        actors[0].control_state_progress = 0; actors[0].unk168 = 4;
        batty_data.unk16 = 1; batty_data.unk20 = &batty_data;
        batty_original_mode = 1;
        D_global_asm_8074C0A0[ACTOR_BANDIT_HANDLE]();
        CHECK(batty_original_calls == 1 && batty_success_calls == 1
            && batty_hide_calls == 1 && batty_cutscene_calls == 1
            && batty_hud_remove_calls == 1 && !batty_data.unk20);
        CHECK(actors[0].control_state == 8 && actors[0].control_state_progress == 0
            && actors[0].unk168 == 4 && actors[1].control_state == 0
            && actors[6].control_state == 0x44 && actors[6].control_state_progress == 1
            && batty_data.unk16 == 0 && batty_data.unk10 == 0 && batty_data.unk1A == 0);
        CHECK(batty_order_count == 5 && batty_order[0] == 'O' && batty_order[1] == 'H'
            && batty_order[2] == 'S' && batty_order[3] == 'C' && batty_order[4] == 'D');
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_BATTY_SUCCESS, key, 1, 0));
    }

    // Either peer can consume each identity from the single stable state and
    // each legitimate live countdown state. Original behavior always runs first.
    for (unsigned receiver_role : {ROLE_HOST, ROLE_JOIN}) for (unsigned key = 1; key <= 3; ++key)
            for (unsigned timer_state : {1u, 2u, 4u}) {
        reset(); load_batty(key); role = receiver_role;
        actors[1].control_state = static_cast<u8>(timer_state);
        batty_data.unk19 = 23; batty_data.unk20 = &batty_data;
        std::array<u8, 4> reel_states{}, reel_faces{};
        std::array<s16, 4> reel_speeds{};
        for (unsigned i = 0; i < 4; ++i) {
            reel_states[i] = actors[i + 2].control_state;
            reel_faces[i] = actors[i + 2].unk15F;
            reel_speeds[i] = batty_reel_data[i].speed;
        }
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, key, 1, 0}}};
        coop_transient_apply(); CHECK(coop_batty_pending_key == key && !batty_original_calls);
        D_global_asm_8074C0A0[ACTOR_BANDIT_HANDLE]();
        CHECK(batty_original_calls == 1 && batty_success_calls == 1
            && batty_success_arg == 0 && batty_success_text == 0
            && batty_hide_calls == 1 && batty_hide_element == 23
            && batty_hide_rotation == MATH_PI_F && batty_cutscene_calls == 1
            && !batty_cutscene_actor && batty_cutscene_index == 0
            && batty_cutscene_mode == 0x11 && batty_hud_remove_calls == 1
            && batty_removed_hud == &batty_data);
        CHECK(batty_order_count == 5 && batty_order[0] == 'O' && batty_order[1] == 'H'
            && batty_order[2] == 'S' && batty_order[3] == 'C' && batty_order[4] == 'D');
        CHECK(actors[0].control_state == 8 && actors[0].control_state_progress == 0
            && actors[0].unk168 == 4 && actors[1].control_state == 0
            && actors[6].control_state == 0x44 && actors[6].control_state_progress == 1
            && batty_data.unk16 == 0 && batty_data.unk10 == 0 && batty_data.unk1A == 0
            && !batty_data.unk20 && !coop_batty_pending_key);
        for (unsigned i = 0; i < 4; ++i)
            CHECK(actors[i + 2].control_state == reel_states[i]
                && actors[i + 2].unk15F == reel_faces[i]
                && batty_reel_data[i].speed == reel_speeds[i]
                && batty_reel_data[i].owner == &actors[0]);
        coop_transient_apply(); coop_batty_behavior();
        CHECK(batty_success_calls == 1 && batty_cutscene_calls == 1);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_BATTY_SUCCESS, key, 1, 0));
    }

    // Malformed, stale, wrong-map and cross-identity packets never queue.
    for (unsigned scenario = 0; scenario < 8; ++scenario) {
        reset(); load_batty(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
        if (scenario == 0) transient_result.records[0].key = 0;
        if (scenario == 1) transient_result.records[0].key = 2;
        if (scenario == 2) transient_result.records[0].key = 4;
        if (scenario == 3) transient_result.records[0].state = 2;
        if (scenario == 4) transient_result.records[0].value = 1;
        if (scenario == 5) transient_result.epoch--;
        if (scenario == 6) transient_result.map++;
        if (scenario == 7) loading_zone_transition_speed = 1.0f;
        coop_transient_apply(); CHECK(!coop_batty_pending_key);
    }

    // Intro and active spin/result states retain a queued success. A stock
    // 3->2 or failed 7->3 edge cannot consume until a later stable callback.
    for (unsigned state : {0u, 3u, 4u, 5u, 6u, 7u}) {
        reset(); load_batty(1); role = ROLE_JOIN;
        actors[0].control_state = static_cast<u8>(state);
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); coop_batty_behavior();
        CHECK(batty_original_calls == 1 && !batty_success_calls
            && coop_batty_pending_key == 1);
    }
    reset(); load_batty(1); role = ROLE_JOIN; actors[0].control_state = 3;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); batty_original_mode = 3; coop_batty_behavior();
    CHECK(actors[0].control_state == 2 && coop_batty_pending_key == 1 && !batty_success_calls);
    batty_original_mode = 0; coop_batty_behavior(); CHECK(batty_success_calls == 1);
    reset(); load_batty(1); role = ROLE_JOIN; actors[0].control_state = 7;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); batty_original_mode = 4; coop_batty_behavior();
    CHECK(actors[0].control_state == 3 && coop_batty_pending_key == 1 && !batty_success_calls);

    // Stable-state normalization gates wait while a reel is moving, but
    // expired or terminal attempts clear. Local timeout/natural success wins.
    reset(); load_batty(1); role = ROLE_JOIN; actors[2].control_state = 5;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); coop_batty_behavior();
    CHECK(coop_batty_pending_key == 1 && !batty_success_calls);
    actors[2].control_state = 0; coop_batty_behavior(); CHECK(batty_success_calls == 1);
    reset(); load_batty(1); role = ROLE_JOIN; actors[1].control_state = 5;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); coop_batty_behavior();
    CHECK(!coop_batty_pending_key && !batty_success_calls);
    for (unsigned state : {1u, 8u, 9u}) {
        reset(); load_batty(1); role = ROLE_JOIN; actors[0].control_state = state;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); coop_batty_behavior();
        CHECK(!coop_batty_pending_key && !batty_success_calls);
    }
    reset(); load_batty(1); role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); batty_original_mode = 2; coop_batty_behavior();
    CHECK(actors[0].control_state == 9 && actors[6].control_state == 0x43
        && !coop_batty_pending_key && !batty_success_calls);
    reset(); load_batty(2); role = ROLE_JOIN; actors[0].control_state = 7;
    actors[0].control_state_progress = 0; actors[0].unk168 = 4;
    batty_data.unk16 = 1; batty_data.unk20 = &batty_data;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_BATTY_SUCCESS, 2, 1, 0}}};
    coop_transient_apply(); batty_original_mode = 1; coop_batty_behavior();
    CHECK(batty_success_calls == 1 && batty_cutscene_calls == 1 && !coop_batty_pending_key);
    coop_transient_capture(1); CHECK(contains_value(COOP_TRANSIENT_BATTY_SUCCESS, 2, 1, 0));

    // General identity, stable-only gates, child ownership and post-original
    // generation/removal/replacement all fail closed without stale dereference.
    for (unsigned scenario = 0; scenario < 38; ++scenario) {
        reset(); load_batty(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); CHECK(coop_batty_pending_key == 1);
        if (scenario == 0) current_map = MAP_BATTY_BARREL_BANDIT_HARD;
        if (scenario == 1) ++kosh_parent;
        if (scenario == 2) kosh_exit = 1;
        if (scenario == 3) ++current_character_index[0];
        if (scenario == 4) actors[0].unk58 = ACTOR_MINIGAME_CONTROLLER;
        if (scenario == 5) actors[0].object_properties_bitfield = 0;
        if (scenario == 6) actors[0].additional_actor_data = nullptr;
        if (scenario == 7) actors[0].unk11C = nullptr;
        if (scenario == 8) actors[6].additional_actor_data = nullptr;
        if (scenario == 9) extra_player_info_pointer = nullptr;
        if (scenario == 10) rabbit_player_info.vehicle_actor_pointer = &actors[7];
        if (scenario == 11) rabbit_player_info.unk1A8 = &actors[7];
        if (scenario == 12) D_global_asm_807FB930[0].actor = nullptr;
        if (scenario == 13) D_global_asm_807FBB34 = 65;
        if (scenario == 14) actors[1].unk58 = ACTOR_TIMER_CONTROLLER;
        if (scenario == 15) actors[1].object_properties_bitfield = 0;
        if (scenario == 16) actors[1].additional_actor_data = nullptr;
        if (scenario == 17) actors[1].unk15F = 5;
        if (scenario == 18) ++*reinterpret_cast<s32*>(batty_timer_data.data() + 0xC);
        if (scenario == 19) D_global_asm_8074C0A0[ACTOR_TIMER] = nullptr;
        if (scenario == 20) batty_data.unk14 = 2;
        if (scenario == 21) batty_data.unk16 = 4;
        if (scenario == 22) batty_data.unk1A = 2;
        if (scenario == 23) ++batty_data.unk1C;
        if (scenario == 24) batty_data.reels[0] = reinterpret_cast<Actor*>(1);
        if (scenario == 25) actors[2].unk58 = ACTOR_BANDIT_HANDLE;
        if (scenario == 26) actors[2].object_properties_bitfield = 0;
        if (scenario == 27) actors[2].additional_actor_data = nullptr;
        if (scenario == 28) actors[2].control_state = 6;
        if (scenario == 29) batty_reel_data[0].owner = &actors[7];
        if (scenario == 30) batty_data.reels[1] = batty_data.reels[0];
        if (scenario == 31) D_global_asm_8074C0A0[ACTOR_BANDIT_SLOT] = nullptr;
        if (scenario == 32) batty_original_mode = 5;
        if (scenario == 33) batty_original_mode = 6;
        if (scenario == 34) batty_original_mode = 7;
        if (scenario == 35) batty_original_mode = 8;
        if (scenario == 36) batty_original_mode = 9;
        if (scenario == 37) actors[0].unk11C = reinterpret_cast<Actor*>(1);
        coop_batty_behavior();
        CHECK(batty_original_calls == 1 && !batty_success_calls && !coop_batty_pending_key);
    }
    for (unsigned mode : {10u, 11u, 12u}) {
        reset(); load_batty(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); batty_original_mode = mode; coop_batty_behavior();
        CHECK(batty_original_calls == 1 && !batty_success_calls && !coop_batty_pending_key);
    }

    // The fourth, hard Batty map has a distinct vanilla contract and remains local.
    reset(); load_batty(1); current_map = MAP_BATTY_BARREL_BANDIT_HARD;
    kosh_parent = 43; current_character_index[0] = 4;
    actors[0].control_state = 7; actors[0].control_state_progress = 0;
    actors[0].unk168 = 4; batty_data.unk16 = 1; batty_data.unk20 = &batty_data;
    batty_original_mode = 1; coop_batty_behavior(); coop_transient_capture(1);
    CHECK(batty_success_calls == 1 && !contains(COOP_TRANSIENT_BATTY_SUCCESS, 1, 1));
    for (unsigned scenario = 0; scenario < 4; ++scenario) {
        reset(); load_batty(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
        coop_transient_apply();
        if (scenario == 0) loading_zone_transition_speed = 1.0f;
        if (scenario == 1) transient_file_changed = 1;
        if (scenario == 2) gPlayerPointer = nullptr;
        if (scenario == 3) ++epoch;
        coop_batty_behavior();
        CHECK(batty_original_calls == 1 && !batty_success_calls && !coop_batty_pending_key);
    }

    // Receiver-only stable predicates cannot force terminal success.
    for (unsigned scenario = 0; scenario < 7; ++scenario) {
        reset(); load_batty(1); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_BATTY_SUCCESS, 1, 1, 0}}};
        coop_transient_apply();
        if (scenario == 0) actors[0].control_state_progress = 0;
        if (scenario == 1) actors[0].unk168 = 1;
        if (scenario == 2) actors[6].control_state = 0x44;
        if (scenario == 3) batty_data.unk16 = 0;
        if (scenario == 4) actors[1].control_state = 0;
        if (scenario == 5) actors[1].control_state = 3;
        if (scenario == 6) actors[1].control_state = 5;
        coop_batty_behavior();
        CHECK(!batty_success_calls);
        if (scenario == 6) CHECK(!coop_batty_pending_key);
        else CHECK(coop_batty_pending_key == 1);
    }

    // A controller hook conflict disables Batty alone. Global Off installs no hook.
    reset(); D_global_asm_8074C0A0[ACTOR_BANDIT_HANDLE] = nullptr;
    coop_batty_hook = 0; coop_transient_init(); CHECK(!coop_batty_hook);
    reset(); transient_enabled = 0; coop_batty_hook = 0;
    D_global_asm_8074C0A0[ACTOR_BANDIT_HANDLE] = func_bonus_8002570C;
    coop_transient_init(); CHECK(!coop_batty_hook
        && D_global_asm_8074C0A0[ACTOR_BANDIT_HANDLE] == func_bonus_8002570C);
}

static void owl_checks() {
    // Both stock active states expose the same exact natural-success edge.
    for (unsigned state : {2u, 0x26u}) {
        reset(); load_owl(state); owl_original_mode = 1;
        D_global_asm_8074C0A0[ACTOR_OWL]();
        CHECK(owl_original_calls == 1 && actors[0].control_state == 0x2A
            && actors[0].control_state_progress == 0 && owl_race.rings == 0
            && owl_animation.unk64 == 0x35B && !coop_owl_pending_key);
        CHECK(owl_order_count == 5 && owl_order[0] == 'O' && owl_order[1] == 'R'
            && owl_order[2] == 'T' && owl_order[3] == 'A' && owl_order[4] == 'C');
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0));
    }

    // Host and Join apply only after the stock handler remains in the same
    // active state. The injected block is the exact vanilla terminal sequence.
    for (unsigned receiver_role : {ROLE_HOST, ROLE_JOIN}) for (unsigned state : {2u, 0x26u}) {
        reset(); load_owl(state); role = receiver_role;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); CHECK(coop_owl_pending_key == 1);
        D_global_asm_8074C0A0[ACTOR_OWL]();
        CHECK(owl_original_calls == 1 && owl_relocate_calls == 1 && owl_text_calls == 1
            && owl_text_file == 0x15 && owl_text_index == 3
            && owl_animation_calls == 1 && owl_animation_id == 0x35B
            && owl_cutscene_calls == 1 && owl_cutscene_id == 0x14 && owl_cutscene_mode == 1);
        CHECK(owl_order_count == 5 && owl_order[0] == 'O' && owl_order[1] == 'R'
            && owl_order[2] == 'T' && owl_order[3] == 'A' && owl_order[4] == 'C');
        CHECK(actors[0].control_state == 0x2A && actors[0].control_state_progress == 0
            && !coop_owl_pending_key);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0));
        coop_transient_apply(); coop_owl_behavior();
        CHECK(owl_relocate_calls == 1 && owl_text_calls == 1 && owl_cutscene_calls == 1);
    }

    // The shared terminal state is insufficient: the stock failure animation,
    // text and cutscene clear a queued win and never publish success.
    reset(); load_owl(); role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
        {{COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0}}};
    coop_transient_apply(); owl_original_mode = 2; coop_owl_behavior();
    CHECK(actors[0].control_state == 0x2A && owl_animation.unk64 == 0x35A
        && owl_text_index == 2 && owl_cutscene_id == 0x16 && !coop_owl_pending_key);
    coop_transient_capture(1); CHECK(!contains(COOP_TRANSIENT_OWL_SUCCESS, 1, 1));

    // Malformed, stale, wrong-map and already-owned records never queue.
    for (unsigned scenario = 0; scenario < 8; ++scenario) {
        reset(); load_owl(); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0}}};
        if (scenario == 0) transient_result.records[0].key = 0;
        if (scenario == 1) transient_result.records[0].key = 2;
        if (scenario == 2) transient_result.records[0].state = 2;
        if (scenario == 3) transient_result.records[0].value = 1;
        if (scenario == 4) transient_result.epoch--;
        if (scenario == 5) transient_result.map++;
        if (scenario == 6) loading_zone_transition_speed = 1.0f;
        if (scenario == 7) owl_reward_owned = true;
        coop_transient_apply(); CHECK(!coop_owl_pending_key);
    }

    // Every immutable actor, player and race identity predicate fails closed.
    for (unsigned scenario = 0; scenario < 25; ++scenario) {
        reset(); load_owl();
        if (scenario == 0) current_map = 49;
        if (scenario == 1) current_character_index[0] = 2;
        if (scenario == 2) actors[0].unk58 = ACTOR_RABBIT_RACE;
        if (scenario == 3) actors[0].object_properties_bitfield = 0;
        if (scenario == 4) actors[0].animation_state = nullptr;
        if (scenario == 5) actors[0].unk178 = nullptr;
        if (scenario == 6) actors[0].unk180 = nullptr;
        if (scenario == 7) gPlayerPointer = nullptr;
        if (scenario == 8) actors[2].additional_actor_data = nullptr;
        if (scenario == 9) extra_player_info_pointer = nullptr;
        if (scenario == 10) actors[2].additional_actor_data = &owl_player_data;
        if (scenario == 11) actors[2].control_state = 0x62;
        if (scenario == 12) rabbit_player_info.unk1F0 = 0;
        if (scenario == 13) rabbit_player_info.unk1F4 = 0;
        if (scenario == 14) owl_enemy.flags = 0x20;
        if (scenario == 15) owl_race.rings = -1;
        if (scenario == 16) owl_race.rings = 17;
        if (scenario == 17) actors[0].unk168 = 17;
        if (scenario == 18) actors[0].unk15F = 0;
        if (scenario == 19) actors[0].unk15F = 17;
        if (scenario == 20) owl_timer.timer = -1;
        if (scenario == 21) owl_timer.timer = 0x79;
        if (scenario == 22) owl_reward_owned = true;
        if (scenario == 23) D_global_asm_8074C0A0[ACTOR_OWL] = func_global_asm_806C55E0;
        if (scenario == 24) D_global_asm_807FB930[0].actor = nullptr;
        CHECK(!coop_owl_identity(&actors[0]));
    }

    // Unload, generation/pointer replacement and campaign changes made by
    // the stock call are revalidated before any terminal helper is invoked.
    for (unsigned mode = 3; mode <= 14; ++mode) {
        reset(); load_owl(); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); owl_original_mode = mode; coop_owl_behavior();
        CHECK(owl_original_calls == 1 && !owl_relocate_calls && !owl_text_calls
            && !owl_animation_calls && !owl_cutscene_calls && !coop_owl_pending_key);
    }

    // Intro/outcome/cleanup states and stale lifecycle boundaries cannot consume.
    for (unsigned state : {0u, 0x1Eu, 0x27u, 0x28u, 0x29u, 0x2Au, 0x30u, 0x3Cu, 0x40u}) {
        reset(); load_owl(state); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0}}};
        coop_transient_apply(); coop_owl_behavior();
        CHECK(owl_original_calls == 1 && !owl_relocate_calls && !coop_owl_pending_key);
    }
    for (unsigned scenario = 0; scenario < 4; ++scenario) {
        reset(); load_owl(); role = ROLE_JOIN;
        transient_result = {COOP_TRANSIENT_APPLYING, current_map, epoch, 1,
            {{COOP_TRANSIENT_OWL_SUCCESS, 1, 1, 0}}};
        coop_transient_apply();
        if (scenario == 0) loading_zone_transition_speed = 1.0f;
        if (scenario == 1) transient_file_changed = 1;
        if (scenario == 2) gPlayerPointer = nullptr;
        if (scenario == 3) ++epoch;
        coop_owl_behavior(); CHECK(!owl_relocate_calls && !coop_owl_pending_key);
    }

    // A handler conflict disables Owl alone; global Off installs no Owl hook.
    reset(); D_global_asm_8074C0A0[ACTOR_OWL] = nullptr;
    coop_owl_hook = 0; coop_transient_init(); CHECK(!coop_owl_hook);
    reset(); transient_enabled = 0; coop_owl_hook = 0;
    D_global_asm_8074C0A0[ACTOR_OWL] = func_global_asm_806C55E0;
    coop_transient_init(); CHECK(!coop_owl_hook
        && D_global_asm_8074C0A0[ACTOR_OWL] == func_global_asm_806C55E0);
}

int main() {
    capture_checks(); object_apply_checks(); cutscene_checks(); lobby_instrument_pad_checks();
    kosh_checks(); minecart_checks(); rabbit_checks(); batty_checks(); owl_checks();
    std::printf("PASS: %u reviewed script/cutscene adapter checks\n", checks);
}
