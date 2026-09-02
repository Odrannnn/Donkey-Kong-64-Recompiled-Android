// Run the production game adapter against a recording engine boundary. These
// tests verify ownership/lifecycle and which engine operations are invoked;
// they do not emulate DK64 rendering, enemy AI, collision or save persistence.
#include "../mod/combat_types.h"
#include <array>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <type_traits>
using u8 = unsigned char; using u16 = unsigned short; using s16 = short;
using u32 = unsigned; using s32 = int; using f32 = float;
enum { ACTOR_PUSHABLE_BOX = 21, ACTOR_BEAVER_BLUE = 178, ACTOR_BEAVER_GOLD = 212, ACTOR_KREMLING = 238,
    ACTOR_KLUMP = 187, ACTOR_MUSHROOM_MAN = 224, ACTOR_ROBO_KREMLING = 235, ACTOR_KOSHA = 291,
    ACTOR_ZINGER_0 = 183, ACTOR_ZINGER_1 = 206, ACTOR_ROBO_ZINGER = 261, ACTOR_BAT = 285,
    ACTOR_KASPLAT_DK = 241, ACTOR_KASPLAT_DIDDY = 242, ACTOR_KASPLAT_LANKY = 243,
    ACTOR_KASPLAT_TINY = 244, ACTOR_KASPLAT_CHUNKY = 245, ACTOR_SHURI = 267, ACTOR_GIMPFISH = 268,
    ACTOR_KLAPTRAP_GREEN = 205, ACTOR_KLAPTRAP_PURPLE = 208, ACTOR_KLAPTRAP_RED = 209, ACTOR_KROSSBONES = 262,
    ACTOR_KABOOM = 175, ACTOR_BOOK = 181, ACTOR_KLOBBER = 182, ACTOR_TOY_MONSTER = 228, ACTOR_PUFFTUP_0 = 290, ACTOR_KRITTER_IN_A_SHEET = 289, ACTOR_MR_DICE_0 = 269, ACTOR_SIR_DOMINO = 270, ACTOR_MR_DICE_1 = 271, ACTOR_SPIDERLING = 276, ACTOR_FIREBALL_WITH_GLASSES = 273, ACTOR_RULER = 230,
    ACTOR_BOSS_KUTOUT_TAG = 165, ACTOR_BOSS_ARMY_DILLO = 185, ACTOR_BOSS_SPIDER = 251,
    ACTOR_BOSS_MAD_JACK = 204, ACTOR_BOSS_PUFFTOSS = 216, ACTOR_BOSS_DOGADON = 236,
    ACTOR_BOSS_KROOL_FOOT = 227, ACTOR_BOSS_KROOL_DK = 281,
    ACTOR_BOSS_KROOL_DIDDY = 292, ACTOR_BOSS_KROOL_LANKY = 293,
    ACTOR_BOSS_KROOL_TINY = 294, ACTOR_BOSS_KROOL_CHUNKY = 295,
    ACTOR_PROJECTILE_COCONUTS = 48, ACTOR_PROJECTILE_PEANUT = 36, ACTOR_PROJECTILE_GRAPE = 42,
    ACTOR_PROJECTILE_FEATHER = 43, ACTOR_PROJECTILE_PINEAPPLE = 38, ACTOR_PROJECTILE_ORANGE = 41,
    MAP_JAPES = 7, MAP_JAPES_ARMY_DILLO = 8, MAP_FUNGI_DOGADON = 83,
    MAP_GALLEON_PUFFTOSS = 111, MAP_FACTORY_MAD_JACK = 154,
    MAP_CAVES_ARMY_DILLO = 196, MAP_AZTEC_DOGADON = 197, MAP_CASTLE_KUT_OUT = 199,
    MAP_KROOL_DK = 203, MAP_KROOL_DIDDY = 204, MAP_KROOL_LANKY = 205,
    MAP_KROOL_TINY = 206, MAP_KROOL_CHUNKY = 207, MAP_KROOL_SHOE = 214,
    MAP_FUNGI_SPIDER = 60, GAME_MODE_ADVENTURE = 1 };
enum { ROLE_HOST = 1, ROLE_JOIN = 2 };
static unsigned role = ROLE_HOST;
struct Actor;
struct AnimationStateUnk0_0 { float unk0{}; unsigned short unk4{}; unsigned char pad6[12]{}; unsigned char unk12{}; };
struct AnimationStateUnk0 { AnimationStateUnk0_0* unk0{}; float unk4{}; short unk10{}; };
struct ActorAnimationState {
    AnimationStateUnk0* unk0{};
    float scale_x{}, scale_y{}, scale_z{};
    int unk68{}, unk6C{}; int (*unk70)(Actor*){}; int unk74{};
};
struct Actor124 { float unkC = 1; };
struct LedgeInfo { bool origin = false; };
struct Actor {
    unsigned unk54{}, unk58{}, object_properties_bitfield{}, interactable{}, unk138{}, unk146{};
    unsigned noclip_byte{}, control_state{}, control_state_progress{};
    int health{}, shadow_opacity{}, draw_distance{}, unkEE{}, y_rotation{};
    float x_position{}, y_position{}, z_position{};
    float unkC[4][4]{}; LedgeInfo* ledge_info_pointer{};
    Actor* unk11C{}; Actor124* unk124{}; ActorAnimationState* animation_state{};
    void* unk178{};
};
struct SpawnerFileData { unsigned enemy_value{}; short x_pos{}, y_pos{}, z_pos{}; };
struct EnemySpawner { SpawnerFileData init; Actor* tied_actor{}; };
struct CharacterSpawner { unsigned char pad[10]{}; unsigned char unkA_u8[2]{}; };
struct GlobalASMStruct35 { unsigned unk200{}; Actor* unk1FC{}; };
struct Struct80717D84 {};
static unsigned checks, deaths, deleted, model_spawns, sprite_spawns, sprites, pose_loads, enemy_pose_evals, boss_impacts;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "ADAPTER FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)
static unsigned combat_enabled = 1, current_file, current_map = MAP_JAPES, game_mode = GAME_MODE_ADVENTURE, is_cutscene_active;
static unsigned current_character_index[1];
static CoopCombatFrame combat_input{}; static CoopCombatResult combat_result{};
static Actor *gPlayerPointer, *gCurrentActorPointer, *gLastSpawnedActor;
static struct { Actor* actor; unsigned metadata; } D_global_asm_807FB930[64];
static u16 D_global_asm_807FBB34;
static void (*D_global_asm_8074C0A0[300])();
static unsigned float_bits(float f) { return std::bit_cast<unsigned>(f); }
static float bits_float(unsigned u) { return std::bit_cast<float>(u); }
static void recomp_printf(const char*, ...) {}
static void renderActor(Actor*, u8) {}
static s32 deleteActor(Actor*);
static s32 spawnActor(s32, s32);
static void func_global_asm_80613CA8(Actor*, s16, float, float);
void func_global_asm_80614644(Actor*, AnimationStateUnk0*, float);
static void setFlag(s16, u8, u8);
static void guTranslateF(float m[4][4], float x, float y, float z) {
    m[3][0] = x; m[3][1] = y; m[3][2] = z;
}
static unsigned actor_is_alive(Actor* a, unsigned generation) {
    for (unsigned i = 0; i < D_global_asm_807FBB34; ++i)
        if (a && D_global_asm_807FB930[i].actor == a) return a->unk54 == generation && a->unk58 == ACTOR_PUSHABLE_BOX;
    return 0;
}
#include "../mod/combat_actor.h"
GlobalASMStruct35 D_global_asm_807FBB70{};
u32 global_properties_bitfield;
f32 loading_zone_transition_speed;
decltype(D_807FDC88) D_807FDC88{};
static CharacterSpawner mock_character_spawner{};
CharacterSpawner* D_global_asm_807FDC9C = &mock_character_spawner;
using ProjectileModel = std::remove_reference_t<decltype(D_global_asm_8074E8B0[0])>;
ProjectileModel D_global_asm_8074E8B0[128]{};
u8 D_global_asm_807206A8[1], D_global_asm_807200A0[1], D_global_asm_8072029C[1], D_global_asm_807201A0[1], D_global_asm_80720268[1];
static std::array<Actor, 64> allocated;
static std::array<ActorAnimationState, 64> animations;
static std::array<LedgeInfo, 64> ledges;
static std::array<Actor*, 64> queued;
static unsigned allocations, queued_count;
static unsigned boss_flags;
u8 D_global_asm_807FBD70;
static void register_actor(Actor* a) { CHECK(D_global_asm_807FBB34 < 64); D_global_asm_807FB930[D_global_asm_807FBB34++].actor = a; }
static void fake_beaver() {
    if (D_global_asm_807FBB70.unk200 == 9 && gCurrentActorPointer->control_state != 0x37) {
        gCurrentActorPointer->control_state = 0x37; ++deaths;
    }
}
void func_global_asm_806AD54C() { fake_beaver(); }
void func_global_asm_806AD7AC() { fake_beaver(); }
void func_global_asm_806AE588() {
    if (gCurrentActorPointer->control_state != 0x32) fake_beaver();
}
// Link fixtures only; these do not model the additional enemies' real AI.
void func_global_asm_806AEE84() { func_global_asm_806AE588(); }
void func_global_asm_806B0354() { fake_beaver(); }
void func_global_asm_806B91E0() { fake_beaver(); }
void func_global_asm_806B0848() { fake_beaver(); }
void func_global_asm_806B486C() { fake_beaver(); }
void func_global_asm_806B48B8() { fake_beaver(); }
void func_global_asm_806B4904() { fake_beaver(); }
void func_global_asm_806B513C() { fake_beaver(); }
void func_global_asm_806B1EA8() { func_global_asm_806AE588(); }
void func_global_asm_806B1EC8() { func_global_asm_806AE588(); }
void func_global_asm_806B1EE8() { func_global_asm_806AE588(); }
void func_global_asm_806B1F08() { func_global_asm_806AE588(); }
void func_global_asm_806B1F28() { func_global_asm_806AE588(); }
void func_global_asm_806B2790() { fake_beaver(); }
void func_global_asm_806B2D64() { fake_beaver(); }
void func_global_asm_806B75F4() { fake_beaver(); }
void func_global_asm_806B761C() { fake_beaver(); }
void func_global_asm_806B7660() { fake_beaver(); }
void func_global_asm_806AFB58() { fake_beaver(); }
void func_global_asm_806B640C() { fake_beaver(); }
void func_global_asm_806B63E0() { fake_beaver(); }
void func_global_asm_806B3680() { fake_beaver(); }
void func_global_asm_806B02EC() { fake_beaver(); }
void func_global_asm_806BC080() { fake_beaver(); }
void func_global_asm_806BC0E4() { fake_beaver(); }
void func_global_asm_806BC148() { fake_beaver(); }
void func_global_asm_806AD9F4() { fake_beaver(); }
void func_global_asm_806B24B8() {
    if (D_global_asm_807FBB70.unk200 == 9) gCurrentActorPointer->control_state = 0x40;
}
void func_global_asm_806BC1AC() { fake_beaver(); }
void func_global_asm_806B52DC() {
    if (D_global_asm_807FBB70.unk200 == 9) gCurrentActorPointer->control_state = 0x37;
}
void func_global_asm_806BB400() {
    if (D_global_asm_807FBB70.unk200 == 9) gCurrentActorPointer->control_state = 0x37;
    if (gCurrentActorPointer->control_state == 0x37) gCurrentActorPointer->control_state = 0x40;
}
void func_boss_800254D0() {
    auto* data = (CoopBossData*)gCurrentActorPointer->unk178;
    if (data && D_global_asm_807FBB70.unk200 == 4 && data->phase < 4) {
        gCurrentActorPointer->control_state = 0x4D;
        gCurrentActorPointer->control_state_progress = 0;
        data->script = 0;
        ++data->phase;
        ++boss_impacts;
    }
}
void func_boss_8002A92C() {
    auto* data = (CoopBossData*)gCurrentActorPointer->unk178;
    if (data && D_global_asm_807FBB70.unk200 == 4 && data->phase <= 3) {
        gCurrentActorPointer->control_state = data->phase == 3 ? 0x56 : 0x4D;
        gCurrentActorPointer->control_state_progress = 0;
        data->script = 0;
        if (data->phase < 3) ++data->phase;
        ++boss_impacts;
    }
}
void func_boss_80033AF0() {
    auto* data = (CoopBossData*)gCurrentActorPointer->unk178;
    if (data && gCurrentActorPointer->control_state == 0x27
            && !gCurrentActorPointer->control_state_progress && data->phase < 4) {
        ++data->phase; gCurrentActorPointer->control_state = 0x25; ++boss_impacts;
    }
}
void func_boss_80029468() {
    auto* data = (CoopBossData*)gCurrentActorPointer->unk178;
    if (data && gCurrentActorPointer->control_state == 0x4D
            && !gCurrentActorPointer->control_state_progress && data->phase < 4) {
        ++data->phase; gCurrentActorPointer->control_state = 0x28; ++boss_impacts;
    }
}
void func_boss_80031AA0() {
    auto* data = (CoopKutOutData*)gCurrentActorPointer->unk178;
    if (data && data->reaction == 4 && data->phase < 3) {
        ++data->phase; data->reaction = 0; ++boss_impacts;
    }
}
static void fake_krool_reaction() {
    auto* data = (CoopBossData*)gCurrentActorPointer->unk178;
    if (data && gCurrentActorPointer->control_state == 0x31
            && !gCurrentActorPointer->control_state_progress && data->phase <= 3) {
        if (data->phase == 3) gCurrentActorPointer->control_state = 0x37;
        else { ++data->phase; gCurrentActorPointer->control_state = 0x28; }
        ++boss_impacts;
    }
}
void func_boss_8002D230() { fake_krool_reaction(); }
void func_boss_8002DE04() { fake_krool_reaction(); }
void func_boss_8002EA7C() { fake_krool_reaction(); }
void func_boss_8002FF74() {
    auto* data = (CoopKRoolFootData*)gCurrentActorPointer->unk178;
    if (data && gCurrentActorPointer->control_state == 0x37) ++boss_impacts;
}
void func_boss_80030EC4() {
    auto* data = (CoopBossData*)gCurrentActorPointer->unk178;
    if (data && gCurrentActorPointer->control_state == 0x29
            && gCurrentActorPointer->control_state_progress == 8 && data->phase < 3) {
        ++data->phase; gCurrentActorPointer->control_state_progress = 6; ++boss_impacts;
    } else if (data && gCurrentActorPointer->control_state == 0x37) {
        ++boss_impacts;
    }
}
void func_boss_8002C964() {
    if (D_global_asm_807FBD70 == 4 && gCurrentActorPointer->health == 1
            && gCurrentActorPointer->control_state == 0x27
            && gCurrentActorPointer->control_state_progress == 2) {
        gCurrentActorPointer->control_state = 0x28;
        gCurrentActorPointer->control_state_progress = 0;
        ++boss_impacts;
    }
}
static void setFlag(s16 flag, u8 value, u8 type) {
    CHECK(flag == 0x1B0 && value == 1 && type == 0); ++boss_flags;
}
static s32 deleteActor(Actor* a) { ++deleted; queued[queued_count++] = a; return 1; }
static void drain_deletes() {
    for (unsigned q = 0; q < queued_count; ++q) for (unsigned i = 0; i < D_global_asm_807FBB34; ++i)
        if (D_global_asm_807FB930[i].actor == queued[q]) {
            D_global_asm_807FB930[i] = D_global_asm_807FB930[--D_global_asm_807FBB34]; break;
        }
    queued_count = 0;
}
s32 func_global_asm_80678014(s32 type) {
    CHECK(type == ACTOR_PUSHABLE_BOX); CHECK(allocations < allocated.size());
    auto& actor = allocated[allocations++]; actor.unk58 = type; actor.unk54 = 1000 + allocations;
    gLastSpawnedActor = &actor; register_actor(&actor); ++sprite_spawns; return 1;
}
static s32 spawnActor(s32 type, s32 model) {
    CHECK(model == 134); // No 3D asset zero or network-supplied model.
    func_global_asm_80678014(type); --sprite_spawns; ++model_spawns;
    gLastSpawnedActor->animation_state = &animations[allocations - 1]; return 1;
}
static void func_global_asm_80613CA8(Actor* a, s16 clip, float start, float blend) {
    CHECK(a->animation_state && clip == 0x404 && start == 0 && blend == 0); ++pose_loads;
}
void func_global_asm_80614644(Actor* a, AnimationStateUnk0* track, float frame) {
    CHECK(a && a->animation_state && a->animation_state->unk0 == track && frame >= 0 && frame <= 254);
    CHECK(!a->animation_state->unk68 && !a->animation_state->unk6C
        && !a->animation_state->unk70 && !a->animation_state->unk74);
    track->unk4 = frame; ++enemy_pose_evals;
}
LedgeInfo* func_global_asm_80665F24(Actor* a) {
    CHECK(a == gLastSpawnedActor && !a->animation_state);
    return &ledges[allocations - 1];
}
void func_global_asm_8066E854(Actor* a, float x, float y, float z, s32 bone) {
    CHECK(a->ledge_info_pointer && x == 0 && y == 0 && z == 0 && bone == -1);
    a->ledge_info_pointer->origin = true;
}
void func_global_asm_807149FC(s32 v) { CHECK(v == -1); }
void func_global_asm_807149B8(u8 v) { CHECK(v == 1); }
void func_global_asm_80714A28(u16 v) { CHECK(v == 4); }
Struct80717D84* func_global_asm_80714C08(void*, float scale, Actor* a, s32 bone, u8 mode) {
    CHECK(scale > 0 && scale <= 4 && a && !a->animation_state && bone == 1 && mode == 2);
    CHECK(a->ledge_info_pointer && a->ledge_info_pointer->origin);
    CHECK(a->unkC[3][0] == a->x_position && a->unkC[3][1] == a->y_position && a->unkC[3][2] == a->z_position);
    CHECK(a->interactable == 0 && a->unk138 == 0 && a->unk11C == nullptr); ++sprites;
    return nullptr;
}
int main() {
    Actor player{}, blue{}, gold{}, projectile{}; Actor124 shot_params;
    AnimationStateUnk0_0 blue_clip{}; blue_clip.unk12 = 61;
    AnimationStateUnk0 blue_track{&blue_clip, 10.0f, 0x201};
    ActorAnimationState blue_animation{}; blue_animation.unk0 = &blue_track;
    blue_animation.unk68 = 11; blue_animation.unk6C = 12;
    blue_animation.unk70 = +[](Actor*) { return 13; }; blue_animation.unk74 = 14;
    player.unk58 = 2; player.unk54 = 1; player.health = 12;
    blue.unk58 = ACTOR_BEAVER_BLUE; blue.unk54 = 100; blue.health = 5; blue.object_properties_bitfield = 0x10; blue.control_state = 1;
    blue.animation_state = &blue_animation;
    gold = blue; gold.unk58 = ACTOR_BEAVER_GOLD; gold.unk54 = 200;
    projectile.unk58 = ACTOR_PROJECTILE_PEANUT; projectile.unk54 = 300; projectile.object_properties_bitfield = 0x10;
    projectile.unk11C = &player; projectile.unk124 = &shot_params;
    gPlayerPointer = &player;
    for (auto* a : {&player, &blue, &gold, &projectile}) register_actor(a);
    EnemySpawner spawners[5]{}; spawners[0].tied_actor = &blue; spawners[1].tied_actor = &gold;
    spawners[0].init.enemy_value = 1; spawners[1].init.enemy_value = 2;
    D_807FDC88.count = 2; D_807FDC88.first = spawners;
    for (const auto& entry : combat_enemy_types)
        D_global_asm_8074C0A0[entry.type] = entry.original;
    coop_combat_init(); CHECK(combat_hooks);
    coop_combat_capture(); CHECK(combat_input.enemies[0].life == 1 && combat_input.enemies[1].life == 2);
    CHECK(combat_input.shots[0].id == 301 && combat_input.shots[0].kind == COOP_PEANUT);
    combat_result.status = COOP_COMBAT_READY;
    CHECK(coop_enemy_health(combat_input.enemies[0]) == 5);
    combat_result.apply[0] = {1, combat_input.enemies[0].life, COOP_ENEMY_ALIVE, 201,
        COOP_BLUE_BEAVER, 0, 0, 0, coop_enemy_pack(0, 3)};
    gCurrentActorPointer = &blue; combat_enemy_behavior(); CHECK(!deaths && blue.health == 3);
    combat_result.apply[0].yaw = coop_enemy_pack(0, 4);
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Network health never heals.
    combat_result.apply[0].yaw = 0x80000000u | coop_enemy_pack(0, 2);
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Reserved packed bits fail closed.
    combat_result.apply[0].yaw = coop_enemy_pack(0, 0);
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Zero is not a live health target.
    combat_result.apply[0].yaw = coop_enemy_pack(1, 2);
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Health commands have no facing payload.
    combat_result.apply[0].yaw = coop_enemy_pack(0, 2); combat_result.apply[0].x = float_bits(1);
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Health commands have no position payload.
    combat_result.apply[0].x = 0;
    combat_result.apply[0].yaw = coop_enemy_pack(0, 2); combat_result.apply[0].life++;
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // A new lifetime cannot reuse health.
    combat_result.apply[0].life = combat_input.enemies[0].life;
    combat_result.apply[0].kind = COOP_GOLD_BEAVER;
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Exact enemy kind is required.
    combat_result.apply[0].kind = COOP_BLUE_BEAVER;
    combat_result.apply[0] = {1, combat_input.enemies[0].life, COOP_ENEMY_DEFEATED, 201, COOP_BLUE_BEAVER, 0, 0, 0, 0};
    combat_result.apply[0].z = float_bits(1);
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Defeat commands have no position payload.
    combat_result.apply[0].z = 0; combat_result.apply[0].yaw = coop_enemy_pack(1, 0);
    combat_enemy_behavior(); CHECK(!deaths && blue.health == 3); // Defeat commands have no packed payload.
    combat_result.apply[0].yaw = 0;
    combat_enemy_behavior();
    CHECK(deaths == 1 && blue.health == 0 && combat_enemies[0].defeated);
    combat_enemy_behavior(); CHECK(deaths == 1 && player.health == 12);
    blue.health = 1; blue.control_state = 1; blue.unk54++;
    coop_combat_capture(); D_global_asm_807FBB70 = {}; combat_enemy_behavior();
    CHECK(deaths == 1 && blue.health == 1); // Stale command must not kill the respawn.
    combat_result.apply[0].life = combat_input.enemies[0].life;
    global_properties_bitfield = 2; combat_enemy_behavior(); CHECK(blue.health == 1); global_properties_bitfield = 0;
    current_map = 8; combat_enemy_behavior(); CHECK(blue.health == 1); current_map = MAP_JAPES;
    gCurrentActorPointer = &gold; gold.health = 0;
    D_global_asm_807FBB70 = {9, &projectile}; combat_enemy_behavior();
    CHECK(combat_enemies[1].defeated && deaths == 2); // Record an actual owned-weapon hit.
    coop_combat_capture(); CHECK(combat_input.enemies[1].state == COOP_ENEMY_DEFEATED);
    gold.health = 1; gold.control_state = 1; gold.unk54++;
    coop_combat_capture(); projectile.unk11C = nullptr; gold.health = 0;
    D_global_asm_807FBB70 = {9, &projectile}; combat_enemy_behavior();
    CHECK(!combat_enemies[1].defeated); // An unrelated actor's hit is never forwarded.
    projectile.unk11C = &player;
    D_global_asm_807FBB70 = {};

    const unsigned types[] = {48, 36, 42, 43, 38, 41};
    for (unsigned i = 0; i < 6; ++i) {
        D_global_asm_8074E8B0[i].type = (short)types[i]; D_global_asm_8074E8B0[i].model = i == 3 ? 134 : 0;
        combat_result.shots[i] = {10 + i, i + 1, float_bits(10), float_bits(20), float_bits(30), 100, float_bits(1)};
    }
    auto* saved_last = gLastSpawnedActor; coop_combat_render(1);
    CHECK(model_spawns == 1 && sprite_spawns == 5 && sprites == 5 && pose_loads == 1);
    CHECK(gLastSpawnedActor == saved_last && player.health == 12);
    combat_result.shots[0].x = float_bits(123);
    coop_combat_render(1); CHECK(combat_shots[0].actor->unkC[3][0] == 123 && sprites == 5);
    for (unsigned i = 0; i < 6; ++i) CHECK(coop_shot_behavior(combat_shots[i].actor));
    coop_combat_capture(); CHECK(combat_input.shots[0].id == 301 && !combat_input.shots[1].id); // No echo loop.
    for (unsigned i = 0; i < 6; ++i) combat_result.shots[i].id += 100;
    coop_combat_render(1); CHECK(deleted == 6 && allocations == COOP_SHOTS); // Six retiring + two free slots.
    CHECK(coop_shot_behavior(combat_shots[0].retiring)); // Queued deletion never runs box behavior.
    drain_deletes(); coop_combat_render(1); CHECK(allocations == 12);
    const unsigned before = deleted;
    Actor* reused = combat_shots[0].actor; reused->unk54++;
    coop_combat_render(1); CHECK(deleted == before && !coop_shot_behavior(reused)); // Lost ownership is not deletion permission.
    coop_combat_render(0); drain_deletes(); CHECK(player.health == 12);

    // Even identical pointers/generation after a same-map reload need new tokens.
    const auto previous_life = combat_enemies[0].life;
    loading_zone_transition_speed = 1; coop_combat_capture(); CHECK(!combat_input.enabled);
    gCurrentActorPointer = &blue; combat_enemy_behavior(); CHECK(blue.health == 1);
    loading_zone_transition_speed = 0; coop_combat_capture();
    CHECK(combat_enemies[0].life != previous_life);
    combat_enemy_behavior(); CHECK(blue.health == 1);

    // Host transforms apply only to a linked live guest copy. No health, control
    // state, animation or player object is copied from the host.
    combat_enabled = 2; role = ROLE_JOIN; combat_result.movement = 1;
    combat_result.motion[0] = {1, combat_enemies[0].life, COOP_ENEMY_ALIVE, 900, COOP_BLUE_BEAVER,
        float_bits(100), float_bits(20), float_bits(-50), coop_enemy_pack(3072, 1)};
    coop_combat_move_enemies(); CHECK(blue.x_position == 40 && blue.y_position == 8 && blue.z_position == -20);
    CHECK(blue.y_rotation == 3072 && blue.unkEE == 3072 && blue.health == 1 && player.x_position == 0);
    const auto x_before = blue.x_position;
    role = ROLE_HOST; coop_combat_move_enemies(); CHECK(blue.x_position == x_before); role = ROLE_JOIN;
    global_properties_bitfield = 2; coop_combat_move_enemies(); CHECK(blue.x_position == x_before); global_properties_bitfield = 0;
    combat_result.motion[0].kind = COOP_GOLD_BEAVER; coop_combat_move_enemies(); CHECK(blue.x_position == x_before);
    combat_result.motion[0].kind = COOP_BLUE_BEAVER;
    combat_result.motion[0].x = float_bits(std::numeric_limits<float>::quiet_NaN()); coop_combat_move_enemies(); CHECK(blue.x_position == x_before);
    combat_result.motion[0].x = float_bits(100);
    blue.unk54++; coop_combat_move_enemies(); CHECK(blue.x_position == x_before);
    coop_combat_capture(); coop_combat_move_enemies(); CHECK(blue.x_position == x_before); // Stale life after respawn.
    combat_result.motion[0].life = combat_enemies[0].life;
    combat_result.motion[0].x = float_bits(1000); coop_combat_move_enemies(); CHECK(blue.x_position == 1000); // Large correction snaps.
    blue.health = 0; coop_combat_move_enemies(); CHECK(blue.x_position == 1000); blue.health = 1;
    combat_enabled = 3; combat_result.movement = COOP_COMBAT_MOVEMENT | COOP_COMBAT_POSE;
    const unsigned pose_peer_life = 900;
    combat_result.motion[0].peer_life = (coop_enemy_clip_hash(0x201) << COOP_ENEMY_POSE_HASH_SHIFT) | pose_peer_life;
    combat_result.motion[0].yaw = coop_enemy_pack_pose(3072, 1, coop_enemy_pose_encode(30.0f, 61));
    coop_combat_move_enemies();
    CHECK(enemy_pose_evals == 1 && blue_track.unk4 == 30.0f
        && blue_animation.unk68 == 11 && blue_animation.unk6C == 12
        && blue_animation.unk70 && blue_animation.unk74 == 14);
    coop_combat_move_enemies(); CHECK(enemy_pose_evals == 1); // Do not hold one 20 Hz sample every render frame.
    combat_result.motion[0].yaw = coop_enemy_pack_pose(3072, 1, coop_enemy_pose_encode(44.0f, 61));
    coop_combat_move_enemies(); CHECK(enemy_pose_evals == 2 && blue_track.unk4 == 44.0f);
    combat_result.status = COOP_COMBAT_OFF; coop_combat_move_enemies();
    combat_result.status = COOP_COMBAT_READY; coop_combat_move_enemies();
    CHECK(enemy_pose_evals == 3); // Restored context may apply the current sample again.
    combat_result.motion[0].peer_life = (((coop_enemy_clip_hash(0x201) + 1) & COOP_ENEMY_POSE_HASH_MASK)
        << COOP_ENEMY_POSE_HASH_SHIFT) | pose_peer_life;
    coop_combat_move_enemies(); CHECK(enemy_pose_evals == 3); // A different local clip hash cannot be driven.
    blue_track.unk10 = 10; coop_combat_move_enemies(); CHECK(enemy_pose_evals == 4); // Mismatch was not consumed.
    blue_track.unk10 = 0x201;
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;

    // Army Dillo damage uses its overlay-private phase byte rather than Actor.health.
    // Remote progress enters the pinned vanilla TNT-impact branch one phase at a time.
    Actor dillo{}; CoopBossData dillo_data{};
    AnimationStateUnk0_0 dillo_clip{}; dillo_clip.unk12 = 91;
    AnimationStateUnk0 dillo_track{&dillo_clip, 45.0f, 0x333};
    ActorAnimationState dillo_animation{}; dillo_animation.unk0 = &dillo_track;
    dillo_animation.unk68 = 21; dillo_animation.unk6C = 22;
    dillo_animation.unk70 = +[](Actor*) { return 23; }; dillo_animation.unk74 = 24;
    dillo.unk58 = ACTOR_BOSS_ARMY_DILLO; dillo.unk54 = 700;
    dillo.object_properties_bitfield = 0x10; dillo.control_state = 0x27;
    dillo.animation_state = &dillo_animation; dillo.unk178 = &dillo_data;
    register_actor(&dillo);
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO] = func_boss_800254D0;
    combat_enabled = 3; dillo.x_position = 10; dillo.y_position = 20; dillo.z_position = 30; dillo.y_rotation = 256;
    current_map = MAP_JAPES_ARMY_DILLO; coop_combat_capture();
    CHECK(boss_hooks && D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO] == coop_boss_behavior);
    CHECK(combat_input.boss.kind == COOP_BOSS_ARMY_DILLO && combat_input.boss.life && !combat_input.boss.phase);
    CHECK(combat_input.boss_motion.kind == COOP_BOSS_ARMY_DILLO
        && combat_input.boss_motion.life == combat_input.boss.life
        && combat_input.boss_motion.x == float_bits(10) && combat_input.boss_motion.yaw == 256
        && combat_input.boss_motion.pose == coop_enemy_pose_encode(45.0f, 91)
        && combat_input.boss_motion.clip_hash == coop_enemy_clip_hash(0x333));
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.movement = COOP_COMBAT_MOVEMENT | COOP_COMBAT_POSE;
    combat_result.boss_motion = {COOP_BOSS_ARMY_DILLO, combat_input.boss.life,
        float_bits(110), float_bits(70), float_bits(-20), 1024,
        coop_enemy_pose_encode(60.0f, 91), coop_enemy_clip_hash(0x333)};
    const unsigned boss_pose_before = enemy_pose_evals;
    role = ROLE_JOIN; gCurrentActorPointer = &dillo; D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo.x_position == 50 && dillo.y_position == 40 && dillo.z_position == 10
        && dillo.y_rotation == 1024);
    CHECK(enemy_pose_evals == boss_pose_before + 1 && dillo_track.unk4 == 60.0f
        && dillo_animation.unk68 == 21 && dillo_animation.unk6C == 22
        && dillo_animation.unk70 && dillo_animation.unk74 == 24);
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(enemy_pose_evals == boss_pose_before + 1); // One network sample is consumed once.
    combat_result.status = COOP_COMBAT_OFF;
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    combat_result.status = COOP_COMBAT_READY;
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(enemy_pose_evals == boss_pose_before + 2); // A restored context may apply the current sample.
    combat_result.boss_motion.clip_hash = (coop_enemy_clip_hash(0x333) + 1) & COOP_ENEMY_POSE_HASH_MASK;
    combat_result.boss_motion.pose = coop_enemy_pose_encode(30.0f, 91);
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(enemy_pose_evals == boss_pose_before + 2); // A different local clip cannot be driven.
    role = ROLE_HOST; combat_enabled = 1;
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_ARMY_DILLO, combat_input.boss.life, 900, 2};
    gCurrentActorPointer = &dillo; D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo_data.phase == 1 && dillo.control_state == 0x4D && boss_impacts == 1);
    D_global_asm_807FBB70 = {}; D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo_data.phase == 1 && boss_impacts == 1); // Do not skip the vanilla reaction state.
    dillo.control_state = 0x27; D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo_data.phase == 2 && boss_impacts == 2);
    dillo.control_state = 0x27; combat_result.boss.life++; D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo_data.phase == 2 && boss_impacts == 2); // Stale local lifetime cannot advance a boss.
    combat_result.boss.life = combat_boss.life; D_global_asm_807FBB70 = {9, &projectile};
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo_data.phase == 2 && boss_impacts == 2 && D_global_asm_807FBB70.unk200 == 9); // Never replace a local collision.
    combat_result.boss = {}; D_global_asm_807FBB70 = {4, &projectile};
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo_data.phase == 3 && boss_impacts == 3); // A real local TNT result remains vanilla-owned.
    D_global_asm_807FBB70 = {}; coop_combat_capture();
    CHECK(combat_input.boss.phase == 3 && combat_input.boss.life == combat_boss.life);
    dillo.control_state = 0x27; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_ARMY_DILLO, combat_boss.life, 900, 4};
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO]();
    CHECK(dillo_data.phase == 4 && boss_impacts == 4);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 4); // Retain final readback through the arena transition.
    current_map = MAP_CAVES_ARMY_DILLO;
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO] = func_boss_800254D0;
    coop_combat_capture(); CHECK(boss_hooks && combat_input.boss.kind == COOP_BOSS_ARMY_DILLO);
    D_global_asm_8074C0A0[ACTOR_BOSS_ARMY_DILLO] = fake_beaver;
    coop_combat_capture(); CHECK(!boss_hooks && !combat_input.boss.kind); // Modified overlay handler fails closed.

    // Dogadon shares the first three private-counter increments and a synthetic
    // fourth wire step for vanilla terminal state 0x56 in both boss arenas.
    Actor dogadon{}; CoopBossData dogadon_data{};
    dogadon.unk58 = ACTOR_BOSS_DOGADON; dogadon.unk54 = 800;
    dogadon.object_properties_bitfield = 0x10; dogadon.control_state = 0x27; dogadon.unk178 = &dogadon_data;
    register_actor(&dogadon);
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON] = func_boss_8002A92C;
    current_map = MAP_AZTEC_DOGADON; coop_combat_capture();
    CHECK(boss_hooks == 2 && D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON] == coop_boss_behavior);
    CHECK(combat_input.boss.kind == COOP_BOSS_DOGADON && combat_input.boss.life && !combat_input.boss.phase);
    const unsigned dogadon_life = combat_input.boss.life, dogadon_start = boss_impacts;
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_DOGADON, dogadon_life, 901, 3};
    gCurrentActorPointer = &dogadon; D_global_asm_807FBB70 = {}; D_global_asm_807FBB70.unk200 = 9;
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON]();
    CHECK(!dogadon_data.phase && boss_impacts == dogadon_start && D_global_asm_807FBB70.unk200 == 9);
    D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON]();
    CHECK(dogadon_data.phase == 1 && dogadon.control_state == 0x4D && boss_impacts == dogadon_start + 1);
    D_global_asm_807FBB70 = {}; D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON]();
    CHECK(dogadon_data.phase == 1 && boss_impacts == dogadon_start + 1); // Reaction state defers the next step.
    dogadon.control_state = 0x27; D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON]();
    dogadon.control_state = 0x27; D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON]();
    CHECK(dogadon_data.phase == 3 && boss_impacts == dogadon_start + 3);
    dogadon.control_state = 0x27; combat_result.boss.phase = 4; D_global_asm_807FBB70 = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON]();
    CHECK(dogadon_data.phase == 3 && dogadon.control_state == 0x56 && boss_impacts == dogadon_start + 4);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 4 && combat_input.boss.life == dogadon_life);
    dogadon.object_properties_bitfield = 0; coop_combat_capture();
    CHECK(combat_input.boss.phase == 4); // Retain terminal readback after actor teardown begins.
    current_map = MAP_FUNGI_DOGADON; dogadon.object_properties_bitfield = 0x10;
    dogadon.control_state = 0x27; dogadon.unk54++; dogadon_data = {};
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON] = func_boss_8002A92C;
    coop_combat_capture();
    CHECK(boss_hooks == 2 && combat_input.boss.kind == COOP_BOSS_DOGADON
        && combat_input.boss.life && combat_input.boss.life != dogadon_life);
    dogadon_data.phase = 4; coop_combat_capture();
    CHECK(!combat_input.boss.kind); // Raw 4 is a separate fallback script, never synthetic terminal damage.
    dogadon_data.phase = 0; coop_combat_capture(); CHECK(combat_input.boss.kind == COOP_BOSS_DOGADON);
    D_global_asm_8074C0A0[ACTOR_BOSS_DOGADON] = fake_beaver;
    coop_combat_capture(); CHECK(!boss_hooks && !combat_input.boss.kind);

    // Mad Jack and Pufftoss enter their pinned vanilla hit reactions. Their
    // original handlers retain ownership of phase increments and terminal flow.
    Actor mad_jack{}; CoopBossData mad_data{};
    mad_jack.unk58 = ACTOR_BOSS_MAD_JACK; mad_jack.unk54 = 810;
    mad_jack.object_properties_bitfield = 0x10; mad_jack.control_state = 0x20; mad_jack.unk178 = &mad_data;
    register_actor(&mad_jack);
    D_global_asm_8074C0A0[ACTOR_BOSS_MAD_JACK] = func_boss_80033AF0;
    current_map = MAP_FACTORY_MAD_JACK; coop_combat_capture();
    CHECK(boss_hooks == 4 && D_global_asm_8074C0A0[ACTOR_BOSS_MAD_JACK] == coop_boss_behavior);
    CHECK(combat_input.boss.kind == COOP_BOSS_MAD_JACK && combat_input.boss.life && !combat_input.boss.phase);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_MAD_JACK, combat_input.boss.life, 902, 2};
    gCurrentActorPointer = &mad_jack; const unsigned mad_start = boss_impacts;
    D_global_asm_8074C0A0[ACTOR_BOSS_MAD_JACK]();
    CHECK(!mad_data.phase && boss_impacts == mad_start); // Non-vulnerable states defer the command.
    mad_jack.control_state = 0x25; D_global_asm_8074C0A0[ACTOR_BOSS_MAD_JACK]();
    CHECK(mad_data.phase == 1 && mad_jack.control_state == 0x25 && boss_impacts == mad_start + 1);
    D_global_asm_8074C0A0[ACTOR_BOSS_MAD_JACK]();
    CHECK(mad_data.phase == 2 && boss_impacts == mad_start + 2);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 2);
    D_global_asm_8074C0A0[ACTOR_BOSS_MAD_JACK] = fake_beaver;
    coop_combat_capture(); CHECK(!boss_hooks && !combat_input.boss.kind);

    Actor pufftoss{}; CoopBossData puff_data{};
    pufftoss.unk58 = ACTOR_BOSS_PUFFTOSS; pufftoss.unk54 = 820;
    pufftoss.object_properties_bitfield = 0x10; pufftoss.control_state = 0x20; pufftoss.unk178 = &puff_data;
    register_actor(&pufftoss);
    D_global_asm_8074C0A0[ACTOR_BOSS_PUFFTOSS] = func_boss_80029468;
    current_map = MAP_GALLEON_PUFFTOSS; coop_combat_capture();
    CHECK(boss_hooks == 8 && D_global_asm_8074C0A0[ACTOR_BOSS_PUFFTOSS] == coop_boss_behavior);
    CHECK(combat_input.boss.kind == COOP_BOSS_PUFFTOSS && combat_input.boss.life && !combat_input.boss.phase);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_PUFFTOSS, combat_input.boss.life, 903, 2};
    gCurrentActorPointer = &pufftoss; const unsigned puff_start = boss_impacts;
    D_global_asm_8074C0A0[ACTOR_BOSS_PUFFTOSS]();
    CHECK(!puff_data.phase && boss_impacts == puff_start);
    pufftoss.control_state = 0x28; D_global_asm_8074C0A0[ACTOR_BOSS_PUFFTOSS]();
    CHECK(puff_data.phase == 1 && pufftoss.control_state == 0x28 && boss_impacts == puff_start + 1);
    D_global_asm_8074C0A0[ACTOR_BOSS_PUFFTOSS]();
    CHECK(puff_data.phase == 2 && boss_impacts == puff_start + 2);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 2);

    // Kut Out's controller owns each caught-Kut-Out sequence. Remote progress
    // asks for reaction 4 only when the controller is idle between rounds.
    Actor kut_out{}; CoopKutOutData kut_data{};
    kut_out.unk58 = ACTOR_BOSS_KUTOUT_TAG; kut_out.unk54 = 830;
    kut_out.object_properties_bitfield = 0x10; kut_out.unk178 = &kut_data;
    register_actor(&kut_out);
    D_global_asm_8074C0A0[ACTOR_BOSS_KUTOUT_TAG] = func_boss_80031AA0;
    current_map = MAP_CASTLE_KUT_OUT; coop_combat_capture();
    CHECK(boss_hooks == 16 && D_global_asm_8074C0A0[ACTOR_BOSS_KUTOUT_TAG] == coop_boss_behavior);
    CHECK(combat_input.boss.kind == COOP_BOSS_KING_KUT_OUT && combat_input.boss.life && !combat_input.boss.phase);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_KING_KUT_OUT, combat_input.boss.life, 904, 3};
    gCurrentActorPointer = &kut_out; const unsigned kut_start = boss_impacts;
    kut_data.reaction = 2; D_global_asm_8074C0A0[ACTOR_BOSS_KUTOUT_TAG]();
    CHECK(!kut_data.phase && boss_impacts == kut_start);
    kut_data.reaction = 0; D_global_asm_8074C0A0[ACTOR_BOSS_KUTOUT_TAG]();
    CHECK(kut_data.phase == 1 && !kut_data.reaction && boss_impacts == kut_start + 1);
    D_global_asm_8074C0A0[ACTOR_BOSS_KUTOUT_TAG]();
    D_global_asm_8074C0A0[ACTOR_BOSS_KUTOUT_TAG]();
    CHECK(kut_data.phase == 3 && boss_impacts == kut_start + 3);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 3);

    // K. Rool's DK, Diddy and Lanky rounds share a four-hit wire model, but
    // each wrapper is pinned to its round's own actor and vulnerable state.
    Actor krool_dk{}; CoopBossData krool_dk_data{};
    krool_dk.unk58 = ACTOR_BOSS_KROOL_DK; krool_dk.unk54 = 840;
    krool_dk.object_properties_bitfield = 0x10; krool_dk.control_state = 0x20; krool_dk.unk178 = &krool_dk_data;
    register_actor(&krool_dk); D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_DK] = func_boss_8002D230;
    current_map = MAP_KROOL_DK; coop_combat_capture();
    CHECK(boss_hooks == 32 && combat_input.boss.kind == COOP_BOSS_K_ROOL_DK);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_K_ROOL_DK, combat_input.boss.life, 905, 2};
    gCurrentActorPointer = &krool_dk; const unsigned dk_start = boss_impacts;
    D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_DK](); CHECK(!krool_dk_data.phase && boss_impacts == dk_start);
    krool_dk.control_state = 0x29; D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_DK]();
    CHECK(krool_dk_data.phase == 1 && boss_impacts == dk_start + 1);
    krool_dk.control_state = 0x2A; D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_DK]();
    CHECK(krool_dk_data.phase == 2 && boss_impacts == dk_start + 2);

    Actor krool_diddy{}; CoopBossData krool_diddy_data{};
    krool_diddy.unk58 = ACTOR_BOSS_KROOL_DIDDY; krool_diddy.unk54 = 850;
    krool_diddy.object_properties_bitfield = 0x10; krool_diddy.control_state = 0x28; krool_diddy.unk178 = &krool_diddy_data;
    register_actor(&krool_diddy); D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_DIDDY] = func_boss_8002DE04;
    current_map = MAP_KROOL_DIDDY; coop_combat_capture();
    CHECK(boss_hooks == 64 && combat_input.boss.kind == COOP_BOSS_K_ROOL_DIDDY);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_K_ROOL_DIDDY, combat_input.boss.life, 906, 4};
    gCurrentActorPointer = &krool_diddy; const unsigned diddy_start = boss_impacts;
    for (unsigned i = 0; i < 4; ++i) D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_DIDDY]();
    CHECK(krool_diddy_data.phase == 3 && krool_diddy.control_state == 0x37
        && boss_impacts == diddy_start + 4);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 4);

    Actor krool_lanky{}; CoopBossData krool_lanky_data{};
    krool_lanky.unk58 = ACTOR_BOSS_KROOL_LANKY; krool_lanky.unk54 = 860;
    krool_lanky.object_properties_bitfield = 0x10; krool_lanky.control_state = 0x2B;
    krool_lanky.control_state_progress = 2; krool_lanky.unk178 = &krool_lanky_data;
    register_actor(&krool_lanky); D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_LANKY] = func_boss_8002EA7C;
    current_map = MAP_KROOL_LANKY; coop_combat_capture();
    CHECK(boss_hooks == 128 && combat_input.boss.kind == COOP_BOSS_K_ROOL_LANKY);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_K_ROOL_LANKY, combat_input.boss.life, 907, 2};
    gCurrentActorPointer = &krool_lanky; const unsigned lanky_start = boss_impacts;
    D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_LANKY](); CHECK(!krool_lanky_data.phase);
    krool_lanky.control_state_progress = 3; D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_LANKY]();
    CHECK(krool_lanky_data.phase == 1 && boss_impacts == lanky_start + 1);
    krool_lanky.control_state = 0x2B; krool_lanky.control_state_progress = 3;
    D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_LANKY]();
    CHECK(krool_lanky_data.phase == 2 && boss_impacts == lanky_start + 2);

    // Tiny's foot commits the toe flag and map return in its existing 0x37
    // state. An inactive toe or raised foot cannot accept a remote hit.
    Actor krool_foot{}; CoopKRoolFootData foot_data{};
    krool_foot.unk58 = ACTOR_BOSS_KROOL_FOOT; krool_foot.unk54 = 870;
    krool_foot.object_properties_bitfield = 0x10; krool_foot.control_state = 0x28;
    krool_foot.control_state_progress = 2; krool_foot.unk178 = &foot_data; foot_data.attack = 1;
    register_actor(&krool_foot); D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_FOOT] = func_boss_8002FF74;
    current_map = MAP_KROOL_SHOE; coop_combat_capture();
    CHECK(boss_hooks == 256 && combat_input.boss.kind == COOP_BOSS_K_ROOL_TINY);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_K_ROOL_TINY, combat_input.boss.life, 908, 1};
    gCurrentActorPointer = &krool_foot; const unsigned tiny_start = boss_impacts;
    D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_FOOT]();
    CHECK(krool_foot.control_state == 0x28 && boss_impacts == tiny_start);
    foot_data.attack = 0; D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_FOOT]();
    CHECK(krool_foot.control_state == 0x37 && boss_impacts == tiny_start + 1);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 1);

    // Chunky's first three hits resume at vanilla post-punch progress 8. The
    // fourth performs the two writes in the original terminal hit branch.
    Actor krool_chunky{}; CoopBossData krool_chunky_data{};
    krool_chunky.unk58 = ACTOR_BOSS_KROOL_CHUNKY; krool_chunky.unk54 = 880;
    krool_chunky.object_properties_bitfield = 0x10; krool_chunky.control_state = 0x29;
    krool_chunky.control_state_progress = 5; krool_chunky.unk178 = &krool_chunky_data;
    register_actor(&krool_chunky); D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_CHUNKY] = func_boss_80030EC4;
    current_map = MAP_KROOL_CHUNKY; coop_combat_capture();
    CHECK(boss_hooks == 512 && combat_input.boss.kind == COOP_BOSS_K_ROOL_CHUNKY);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_K_ROOL_CHUNKY, combat_input.boss.life, 909, 4};
    gCurrentActorPointer = &krool_chunky; const unsigned chunky_start = boss_impacts;
    D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_CHUNKY](); CHECK(!krool_chunky_data.phase);
    krool_chunky.control_state_progress = 6;
    for (unsigned i = 0; i < 4; ++i) D_global_asm_8074C0A0[ACTOR_BOSS_KROOL_CHUNKY]();
    CHECK(krool_chunky_data.phase == 3 && krool_chunky.control_state == 0x37
        && boss_impacts == chunky_start + 4 && boss_flags == 1);
    coop_combat_capture(); CHECK(combat_input.boss.phase == 4);

    // The Fungi Spider has one final vulnerable collision after its locally
    // simulated Spiderling waves. The pinned handler owns the full death path.
    Actor spider{}; CoopBossData spider_data{};
    spider.unk58 = ACTOR_BOSS_SPIDER; spider.unk54 = 890; spider.health = 6;
    spider.object_properties_bitfield = 0x10; spider.control_state = 0x27;
    spider.control_state_progress = 1; spider.unk178 = &spider_data;
    register_actor(&spider); D_global_asm_8074C0A0[ACTOR_BOSS_SPIDER] = func_boss_8002C964;
    current_map = MAP_FUNGI_SPIDER; D_global_asm_807FBD70 = 0; coop_combat_capture();
    CHECK(boss_hooks == 1024 && combat_input.boss.kind == COOP_BOSS_FUNGI_SPIDER
        && !combat_input.boss.phase && !combat_input.boss_motion.kind);
    combat_result = {}; combat_result.status = COOP_COMBAT_READY;
    combat_result.boss = {COOP_BOSS_FUNGI_SPIDER, combat_input.boss.life, 910, 1};
    gCurrentActorPointer = &spider; const unsigned spider_start = boss_impacts;
    D_global_asm_8074C0A0[ACTOR_BOSS_SPIDER]();
    CHECK(spider.control_state == 0x27 && boss_impacts == spider_start); // Wait until vulnerable.
    spider.control_state_progress = 2; D_global_asm_807FBD70 = 7;
    D_global_asm_8074C0A0[ACTOR_BOSS_SPIDER]();
    CHECK(spider.control_state == 0x27 && boss_impacts == spider_start); // Preserve a local collision.
    D_global_asm_807FBD70 = 0; D_global_asm_8074C0A0[ACTOR_BOSS_SPIDER]();
    CHECK(spider.health == 1 && spider.control_state == 0x28 && boss_impacts == spider_start + 1);
    D_global_asm_807FBD70 = 0; coop_combat_capture();
    CHECK(combat_input.boss.phase == 1 && !combat_input.boss_motion.kind);
    current_map = MAP_JAPES; combat_result = {}; combat_result.status = COOP_COMBAT_READY;

    // The added enemy retains its own handler, including the knockback guard.
    Actor kremling{}; kremling.unk58 = ACTOR_KREMLING; kremling.unk54 = 500;
    kremling.health = 1; kremling.object_properties_bitfield = 0x10; kremling.control_state = 0x32;
    register_actor(&kremling); spawners[2].tied_actor = &kremling; spawners[2].init.enemy_value = 3;
    D_807FDC88.count = 3; coop_combat_capture(); CHECK(combat_enemies[2].kind == COOP_KREMLING);
    combat_result.apply[2] = {3, combat_enemies[2].life, COOP_ENEMY_DEFEATED, 800, COOP_KREMLING, 0, 0, 0, 0};
    gCurrentActorPointer = &kremling; D_global_asm_807FBB70 = {};
    combat_enemy_behavior(); CHECK(kremling.health == 1); // Wait until local knockback ends.
    kremling.control_state = 1; combat_enemy_behavior(); CHECK(kremling.health == 0 && combat_enemies[2].defeated);
    const unsigned supported_maps[] = {
        4, 7, 12, 13, 16, 17, 19, 20, 21, 22, 23, 24, 26, 30, 34, 38,
        43, 46, 47, 48, 49, 52, 57, 58, 59, 60, 61, 62, 64, 70, 71, 72,
        84, 85, 86, 87, 88, 91, 92, 93, 94, 95, 105, 108, 112, 113, 114,
        151, 163, 164, 166, 169, 173, 175, 179, 183, 193, 200
    };
    for (unsigned map : supported_maps) {
        current_map = map; kremling.health = 1; kremling.control_state = 1; kremling.unk54++;
        D_global_asm_807FBB70 = {}; coop_combat_capture();
        CHECK(combat_input.layout && combat_enemies[2].life);
        combat_result.apply[2].life = combat_enemies[2].life;
        combat_enemy_behavior(); CHECK(kremling.health == 0 && combat_enemies[2].defeated);
    }
    for (unsigned map : {8u, 10u, 14u, 31u, 53u, 73u, 81u, 82u, 83u, 96u, 109u, 111u, 157u, 196u, 197u, 198u, 201u}) {
        current_map = map; kremling.health = 1; kremling.control_state = 1; kremling.unk54++;
        D_global_asm_807FBB70 = {}; coop_combat_capture(); combat_enemy_behavior();
        CHECK(!combat_input.layout && kremling.health == 1);
    }
    current_map = MAP_JAPES;

    // The Caves glasses fireball uses its original immediate terminal state.
    current_map = 95; // Chunky's Caves igloo has five vanilla glasses fireballs.
    Actor fireball{}; fireball.unk58 = ACTOR_FIREBALL_WITH_GLASSES; fireball.unk54 = 700;
    fireball.health = 1; fireball.object_properties_bitfield = 0x10; fireball.control_state = 0x23;
    register_actor(&fireball); spawners[2].tied_actor = &fireball; spawners[2].init.enemy_value = 4;
    D_807FDC88.count = 3; coop_combat_capture();
    CHECK(combat_enemies[2].kind == COOP_FIREBALL_WITH_GLASSES);
    combat_result.apply[2] = {3, combat_enemies[2].life, COOP_ENEMY_DEFEATED, 801,
        COOP_FIREBALL_WITH_GLASSES, 0, 0, 0, 0};
    gCurrentActorPointer = &fireball; D_global_asm_807FBB70 = {};
    combat_enemy_behavior();
    CHECK(fireball.health == 0 && fireball.control_state == 0x40 && combat_enemies[2].defeated);
    fireball.health = 1; fireball.control_state = 0x23; fireball.unk54++;
    coop_combat_capture(); D_global_asm_807FBB70 = {9, &projectile};
    combat_enemy_behavior();
    CHECK(fireball.control_state == 0x40 && combat_enemies[2].defeated); // Owned local hit.
    D_global_asm_807FBB70 = {};
    current_map = MAP_JAPES;

    // Factory's two vanilla Rulers use their pinned ordinary state-0x37 death
    // path. Encounter-controlled spawner phases remain local and block commands.
    current_map = 26;
    Actor ruler{}; ruler.unk58 = ACTOR_RULER; ruler.unk54 = 800;
    ruler.health = 1; ruler.object_properties_bitfield = 0x10; ruler.control_state = 1;
    register_actor(&ruler); spawners[2].tied_actor = &ruler; spawners[2].init.enemy_value = 5;
    D_807FDC88.count = 3; coop_combat_capture();
    CHECK(combat_enemies[2].kind == COOP_RULER);
    combat_result.apply[2] = {3, combat_enemies[2].life, COOP_ENEMY_DEFEATED, 802,
        COOP_RULER, 0, 0, 0, 0};
    gCurrentActorPointer = &ruler; D_global_asm_807FBB70 = {};
    mock_character_spawner.unkA_u8[0] = 1; combat_enemy_behavior();
    CHECK(ruler.health == 1 && !combat_enemies[2].defeated);
    mock_character_spawner.unkA_u8[0] = 0; combat_enemy_behavior();
    CHECK(ruler.health == 0 && ruler.control_state == 0x37 && combat_enemies[2].defeated);
    ruler.health = 1; ruler.control_state = 1; ruler.unk54++;
    coop_combat_capture(); ruler.health = 0; D_global_asm_807FBB70 = {9, &projectile};
    combat_enemy_behavior(); CHECK(ruler.control_state == 0x37 && combat_enemies[2].defeated);
    D_global_asm_807FBB70 = {};

    // Book and Toy Monster use non-health disappearance paths. Remote commands
    // enter only their pinned vanilla terminal states; Clam has no defeat path.
    Actor book{}, toy{};
    book.unk58 = ACTOR_BOOK; book.unk54 = 810; book.health = 1;
    book.object_properties_bitfield = 0x10; book.control_state = 1;
    toy.unk58 = ACTOR_TOY_MONSTER; toy.unk54 = 811; toy.health = 1;
    toy.object_properties_bitfield = 0x10; toy.control_state = 0x23;
    register_actor(&book); register_actor(&toy);
    spawners[3].tied_actor = &book; spawners[3].init.enemy_value = 6;
    spawners[4].tied_actor = &toy; spawners[4].init.enemy_value = 7;
    D_807FDC88.count = 5; current_map = 87; coop_combat_capture();
    CHECK(combat_enemies[3].kind == COOP_BOOK && combat_enemies[4].kind == COOP_TOY_MONSTER);
    combat_result.apply[0] = {4, combat_enemies[3].life, COOP_ENEMY_DEFEATED, 900,
        COOP_BOOK, 0, 0, 0, 0};
    gCurrentActorPointer = &book; combat_enemy_behavior();
    CHECK(book.health == 0 && book.control_state == 0x37 && combat_enemies[3].defeated);
    combat_result.apply[0] = {5, combat_enemies[4].life, COOP_ENEMY_DEFEATED, 901,
        COOP_TOY_MONSTER, 0, 0, 0, 0};
    gCurrentActorPointer = &toy; combat_enemy_behavior();
    CHECK(toy.health == 0 && toy.control_state == 0x40 && combat_enemies[4].defeated);
    D_global_asm_807FBB70 = {}; current_map = MAP_JAPES;

    // A short-lived owned projectile remains advertised for six capture frames,
    // long enough for the 20 Hz network sampler to observe it, then retires.
    coop_combat_capture(); CHECK(combat_input.shots[0].id == 301);
    for (unsigned i = 0; i < D_global_asm_807FBB34; ++i)
        if (D_global_asm_807FB930[i].actor == &projectile) D_global_asm_807FB930[i].actor = NULL;
    for (unsigned i = 0; i < 6; ++i) {
        coop_combat_capture(); CHECK(combat_input.shots[0].id == 301);
    }
    coop_combat_capture(); CHECK(!combat_input.shots[0].id);

    // Game snapshots page every supported live record instead of starving keys
    // after the first twenty.
    enum { CROWD_COUNT = 24 };
    Actor crowd[CROWD_COUNT]{}; EnemySpawner crowd_spawners[CROWD_COUNT]{};
    for (unsigned i = 0; i < CROWD_COUNT; ++i) {
        crowd[i].unk58 = ACTOR_BEAVER_BLUE; crowd[i].unk54 = 1000 + i;
        crowd[i].health = 1; crowd[i].control_state = 1; crowd[i].object_properties_bitfield = 0x10;
        crowd_spawners[i].tied_actor = &crowd[i]; crowd_spawners[i].init.enemy_value = i + 1;
        crowd_spawners[i].init.x_pos = i * 10; register_actor(&crowd[i]);
    }
    D_807FDC88.count = CROWD_COUNT; D_807FDC88.first = crowd_spawners;
    unsigned saw_first = 0, saw_last = 0;
    for (unsigned page = 0; page < 4; ++page) {
        coop_combat_capture(); CHECK(combat_input.pages == 2 && combat_input.page < 2);
        for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
            saw_first |= combat_input.enemies[i].key == 1;
            saw_last |= combat_input.enemies[i].key == CROWD_COUNT;
        }
    }
    CHECK(saw_first && saw_last);

    // Save-slot switch is sticky for this process; returning to the old slot cannot re-enable damage.
    current_file = 1; coop_combat_capture(); CHECK(combat_file_changed && !combat_input.enabled);
    current_file = 0; coop_combat_capture(); CHECK(!combat_input.enabled);
    combat_hooks = 0; auto replacement = +[] {};
    D_global_asm_8074C0A0[ACTOR_BEAVER_BLUE] = replacement;
    D_global_asm_8074C0A0[ACTOR_BEAVER_GOLD] = func_global_asm_806AD7AC;
    coop_combat_init(); CHECK(!combat_hooks && D_global_asm_8074C0A0[ACTOR_BEAVER_BLUE] == replacement);
    std::printf("PASS: %u production combat-adapter checks (ten boss rounds, host movement, Rulers, map allowlist, owned hits, spawn/lifetime guards, inert shots, file and hook conflicts)\n", checks);
}
