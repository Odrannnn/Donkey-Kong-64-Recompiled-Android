#ifndef DKCOOP_COMBAT_ACTOR_H
#define DKCOOP_COMBAT_ACTOR_H
// Included after coop.c's actor registry helper and globals.
extern GlobalASMStruct35 D_global_asm_807FBB70;
extern u32 global_properties_bitfield;
extern f32 loading_zone_transition_speed;
extern CharacterSpawner* D_global_asm_807FDC9C;
extern s32 func_global_asm_80678014(s32);
extern LedgeInfo* func_global_asm_80665F24(Actor*);
extern void func_global_asm_8066E854(Actor*, f32, f32, f32, s32);
extern void func_global_asm_80614644(Actor*, AnimationStateUnk0*, f32);
extern void func_global_asm_806AD54C(void), func_global_asm_806AD7AC(void);
extern void func_global_asm_806AE588(void);
extern void func_global_asm_806AEE84(void), func_global_asm_806B0354(void);
extern void func_global_asm_806B91E0(void), func_global_asm_806B0848(void);
extern void func_global_asm_806B486C(void), func_global_asm_806B48B8(void);
extern void func_global_asm_806B4904(void), func_global_asm_806B513C(void);
extern void func_global_asm_806B1EA8(void), func_global_asm_806B1EC8(void);
extern void func_global_asm_806B1EE8(void), func_global_asm_806B1F08(void);
extern void func_global_asm_806B1F28(void);
extern void func_global_asm_806B2790(void), func_global_asm_806B2D64(void);
extern void func_global_asm_806B75F4(void), func_global_asm_806B761C(void);
extern void func_global_asm_806B7660(void), func_global_asm_806AFB58(void);
extern void func_global_asm_806B640C(void), func_global_asm_806B63E0(void);
extern void func_global_asm_806B3680(void);
extern void func_global_asm_806B02EC(void);
extern void func_global_asm_806BC080(void), func_global_asm_806BC0E4(void);
extern void func_global_asm_806BC148(void);
extern void func_global_asm_806AD9F4(void);
extern void func_global_asm_806B24B8(void);
extern void func_global_asm_806BC1AC(void);
extern void func_boss_800254D0(void);
extern void func_boss_8002A92C(void);
extern void func_boss_80033AF0(void);
extern void func_boss_80029468(void);
extern void func_boss_80031AA0(void);
extern void func_boss_8002D230(void);
extern void func_boss_8002DE04(void);
extern void func_boss_8002EA7C(void);
extern void func_boss_8002FF74(void);
extern void func_boss_80030EC4(void);
extern struct { s16 count, padding; EnemySpawner* first; } D_807FDC88;
extern struct { s16 type, model; s32 data[11]; } D_global_asm_8074E8B0[];
extern u8 D_global_asm_807206A8[], D_global_asm_807200A0[], D_global_asm_8072029C[];
extern u8 D_global_asm_807201A0[], D_global_asm_80720268[];
extern void func_global_asm_807149FC(s32), func_global_asm_807149B8(u8), func_global_asm_80714A28(u16);
extern Struct80717D84* func_global_asm_80714C08(void*, f32, Actor*, s32, u8);

typedef struct {
    Actor* actor;
    u32 generation, life, defeated, kind;
    u32 pose_peer_life, pose_stamp;
} CoopEnemySlot;
typedef struct {
    Actor *actor, *retiring;
    u32 generation, retiring_generation, id, kind;
} CoopShotSlot;
typedef struct {
    CoopShot shot;
    u32 missed;
} CoopLocalShotSlot;
typedef struct {
    u8 pad[0x14];
    u8 phase, script;
} CoopBossData;
typedef struct {
    u8 pad[0x12];
    signed char phase, reaction;
} CoopKutOutData;
typedef struct {
    u8 pad[0x12];
    u16 toe;
    u8 attack, script;
} CoopKRoolFootData;
typedef struct {
    Actor* actor;
    u32 generation, life, phase, kind;
} CoopBossSlot;
static CoopEnemySlot combat_enemies[256];
static CoopShotSlot combat_shots[COOP_SHOTS];
static CoopLocalShotSlot combat_local_shots[COOP_SHOTS];
static CoopBossSlot combat_boss;
static u32 combat_layout, combat_file, combat_file_changed, combat_hooks, combat_capture_page;
static u32 combat_life_counter; // Does not reset with the game's per-map actor counter.
static EnemySpawner* combat_spawner_table;
static s32 combat_map = -1;
static void (*enemy_behaviors[COOP_ENEMY_KIND_COUNT])(void);
static void (*boss_behaviors[COOP_BOSS_KIND_COUNT])(void);
static u32 boss_hooks;
// Pinned US actor-table entries, in combat_types.h wire-ID order. Never accept
// an actor ID or function pointer from the network. Every entry is checked
// before installing any wrapper, including the original three enemy kinds.
static const struct { unsigned type; void (*original)(void); }
combat_enemy_types[COOP_ENEMY_KIND_COUNT] = {
    {ACTOR_BEAVER_BLUE, func_global_asm_806AD54C},
    {ACTOR_BEAVER_GOLD, func_global_asm_806AD7AC},
    {ACTOR_KREMLING, func_global_asm_806AE588},
    {ACTOR_KLUMP, func_global_asm_806AEE84},
    {ACTOR_MUSHROOM_MAN, func_global_asm_806B0354},
    {ACTOR_ROBO_KREMLING, func_global_asm_806B91E0},
    {ACTOR_KOSHA, func_global_asm_806B0848},
    {ACTOR_ZINGER_0, func_global_asm_806B486C},
    {ACTOR_ZINGER_1, func_global_asm_806B48B8},
    {ACTOR_ROBO_ZINGER, func_global_asm_806B4904},
    {ACTOR_BAT, func_global_asm_806B513C},
    {ACTOR_KASPLAT_DK, func_global_asm_806B1EA8},
    {ACTOR_KASPLAT_DIDDY, func_global_asm_806B1EC8},
    {ACTOR_KASPLAT_LANKY, func_global_asm_806B1EE8},
    {ACTOR_KASPLAT_TINY, func_global_asm_806B1F08},
    {ACTOR_KASPLAT_CHUNKY, func_global_asm_806B1F28},
    {ACTOR_SHURI, func_global_asm_806B2790},
    {ACTOR_GIMPFISH, func_global_asm_806B2D64},
    {ACTOR_KLAPTRAP_GREEN, func_global_asm_806B75F4},
    {ACTOR_KLAPTRAP_PURPLE, func_global_asm_806B761C},
    {ACTOR_KLAPTRAP_RED, func_global_asm_806B7660},
    {ACTOR_KROSSBONES, func_global_asm_806AFB58},
    {ACTOR_KABOOM, func_global_asm_806B640C},
    {ACTOR_KLOBBER, func_global_asm_806B63E0},
    {ACTOR_PUFFTUP_0, func_global_asm_806B3680},
    {ACTOR_KRITTER_IN_A_SHEET, func_global_asm_806B02EC},
    {ACTOR_MR_DICE_0, func_global_asm_806BC080},
    {ACTOR_SIR_DOMINO, func_global_asm_806BC0E4},
    {ACTOR_MR_DICE_1, func_global_asm_806BC148},
    {ACTOR_SPIDERLING, func_global_asm_806AD9F4},
    {ACTOR_FIREBALL_WITH_GLASSES, func_global_asm_806B24B8},
    {ACTOR_RULER, func_global_asm_806BC1AC},
};
static const struct { unsigned type; void (*original)(void); }
combat_boss_types[COOP_BOSS_KIND_COUNT] = {
    {ACTOR_BOSS_ARMY_DILLO, func_boss_800254D0},
    {ACTOR_BOSS_DOGADON, func_boss_8002A92C},
    {ACTOR_BOSS_MAD_JACK, func_boss_80033AF0},
    {ACTOR_BOSS_PUFFTOSS, func_boss_80029468},
    {ACTOR_BOSS_KUTOUT_TAG, func_boss_80031AA0},
    {ACTOR_BOSS_KROOL_DK, func_boss_8002D230},
    {ACTOR_BOSS_KROOL_DIDDY, func_boss_8002DE04},
    {ACTOR_BOSS_KROOL_LANKY, func_boss_8002EA7C},
    {ACTOR_BOSS_KROOL_FOOT, func_boss_8002FF74},
    {ACTOR_BOSS_KROOL_CHUNKY, func_boss_80030EC4},
};

static unsigned combat_explosive_kind(unsigned kind) {
    return kind == COOP_KABOOM || kind == COOP_KLOBBER;
}
static unsigned combat_defeat_state(unsigned kind) {
    if (kind == COOP_PUFFTUP) return 0x27;
    if (kind == COOP_FIREBALL_WITH_GLASSES) return 0x40;
    return 0x37;
}
static unsigned combat_nonhealth_defeat_kind(unsigned kind) {
    return combat_explosive_kind(kind) || kind == COOP_PUFFTUP
        || kind == COOP_FIREBALL_WITH_GLASSES;
}

static unsigned combat_actor_live(Actor* actor, unsigned generation) {
    if (!actor || D_global_asm_807FBB34 > 64) return 0;
    for (unsigned i = 0; i < D_global_asm_807FBB34; ++i)
        if (D_global_asm_807FB930[i].actor == actor) return actor->unk54 == generation;
    return 0;
}
static unsigned combat_projectile_kind(unsigned type) {
    switch (type) {
        case ACTOR_PROJECTILE_COCONUTS: return COOP_COCONUT;
        case ACTOR_PROJECTILE_PEANUT: return COOP_PEANUT;
        case ACTOR_PROJECTILE_GRAPE: return COOP_GRAPE;
        case ACTOR_PROJECTILE_FEATHER: return COOP_FEATHER;
        case ACTOR_PROJECTILE_PINEAPPLE: return COOP_PINEAPPLE;
        case ACTOR_PROJECTILE_ORANGE: return COOP_ORANGE;
    }
    return 0;
}
static unsigned combat_own_attack(Actor* actor) {
    if (!actor || D_global_asm_807FBB34 > 64) return 0;
    for (unsigned i = 0; i < D_global_asm_807FBB34; ++i) {
        if (D_global_asm_807FB930[i].actor != actor) continue;
        return actor == gPlayerPointer || (combat_projectile_kind(actor->unk58) && actor->unk11C == gPlayerPointer);
    }
    return 0;
}
static unsigned combat_enemy_kind(unsigned type) {
    for (unsigned i = 0; i < COOP_ENEMY_KIND_COUNT; ++i)
        if (combat_enemy_types[i].type == type) return i + 1;
    return 0;
}
static unsigned combat_damage_ready(Actor* actor) {
    // These enemies' knockback state 0x32 skips the collision-death branch.
    // Leave the host command pending until the original behavior can accept it.
    const unsigned kind = combat_enemy_kind(actor->unk58);
    const unsigned dice = (kind >= COOP_MR_DICE_0 && kind <= COOP_MR_DICE_1) || kind == COOP_RULER;
    return actor->health > 0 && actor->control_state < 0x36
        && (!dice || D_global_asm_807FDC9C->unkA_u8[0] == 0)
        && !((actor->unk58 == ACTOR_KREMLING || actor->unk58 == ACTOR_KLUMP
                || (kind >= COOP_KASPLAT_DK && kind <= COOP_KASPLAT_CHUNKY))
            && actor->control_state == 0x32);
}
static unsigned combat_game_ready(void) {
    return combat_enabled && !combat_file_changed && game_mode == GAME_MODE_ADVENTURE
        && !(global_properties_bitfield & 3)
        && loading_zone_transition_speed == 0.0f
        && !is_cutscene_active && current_file < 3 && gPlayerPointer && current_character_index[0] < 5
        && (unsigned)gPlayerPointer->unk58 == current_character_index[0] + 2;
}
static void combat_enemy_behavior(void) {
    Actor* actor = gCurrentActorPointer;
    unsigned kind = combat_enemy_kind(actor->unk58);
    if (!kind) return;
    unsigned generation = actor->unk54, key = 0;
    unsigned hit = 0, applied = 0;
    if (combat_game_ready() && coop_combat_map(current_map) && combat_hooks && combat_result.status == COOP_COMBAT_READY
            && D_807FDC88.count > 0 && D_807FDC88.count <= 256 && D_807FDC88.first) {
        for (unsigned i = 0; i < (unsigned)D_807FDC88.count; ++i) {
            CoopEnemySlot* slot = &combat_enemies[i];
            if (D_807FDC88.first[i].tied_actor == actor && slot->actor == actor && slot->generation == generation) {
                key = i + 1;
                hit = D_global_asm_807FBB70.unk200 == 9 && combat_own_attack(D_global_asm_807FBB70.unk1FC);
                for (unsigned c = 0; c < COOP_ENEMIES; ++c) {
                    CoopEnemy command = combat_result.apply[c];
                    if (command.kind != kind) continue;
                    if (coop_combat_health_matches(command, key, slot->life,
                            actor->object_properties_bitfield & 0x10,
                            combat_damage_ready(actor), slot->kind == kind, 1,
                            (unsigned)actor->health)) {
                        // Partial health converges only downward. Do not forge a
                        // collision source, reaction animation or damage amount.
                        actor->health = coop_enemy_health(command);
                        break;
                    }
                    if (!coop_combat_apply_matches(command, key, slot->life,
                            actor->object_properties_bitfield & 0x10,
                            combat_damage_ready(actor), slot->kind == kind, 1)) continue;
                    // Enter the existing enemy death path for this actor's update.
                    // No remote script ID, attacker pointer, damage amount or switch is accepted.
                    actor->health = 0;
                    D_global_asm_807FBB70.unk200 = 9;
                    D_global_asm_807FBB70.unk1FC = NULL;
                    if (combat_explosive_kind(kind) || kind == COOP_PUFFTUP) {
                        // These two handlers own an explicit explosion state
                        // rather than the ordinary health-based death helper.
                        actor->control_state = combat_defeat_state(kind);
                        actor->control_state_progress = 0;
                    }
                    applied = 1;
                    break;
                }
                break;
            }
        }
    }
    enemy_behaviors[kind - 1]();
    // Keep a confirmed defeat after the actor is removed, until its next spawn.
    // A disappearing/culled actor alone is never a hit or a defeat.
    if (key && (hit || applied) && combat_actor_live(actor, generation)
            && actor->control_state == combat_defeat_state(kind)
            && (actor->health <= 0 || combat_nonhealth_defeat_kind(kind)))
        combat_enemies[key - 1].defeated = 1;
}
static unsigned combat_boss_actor_kind(unsigned type) {
    for (unsigned i = 0; i < COOP_BOSS_KIND_COUNT; ++i)
        if (combat_boss_types[i].type == type) return i + 1;
    return COOP_BOSS_NONE;
}
static unsigned combat_boss_phase(unsigned kind, Actor* actor, CoopBossData* data) {
    if (!actor || !data) return 5;
    if (kind == COOP_BOSS_ARMY_DILLO) return data->phase <= 4 ? data->phase : 5;
    if (kind == COOP_BOSS_DOGADON && data->phase <= 3) {
        // Dogadon's fourth TNT impact enters terminal state 0x56 without
        // incrementing its private three-step hit counter. Raw value 4 belongs
        // to a separate vanilla timer/fallback script and is deliberately not
        // interpreted as damage progress.
        return data->phase == 3 && actor->control_state == 0x56 ? 4 : data->phase;
    }
    if ((kind == COOP_BOSS_MAD_JACK || kind == COOP_BOSS_PUFFTOSS)
            && data->phase <= 4) return data->phase;
    if (kind == COOP_BOSS_KING_KUT_OUT) {
        CoopKutOutData* kut = (CoopKutOutData*)data;
        return kut->phase >= 0 && kut->phase <= 3 ? (unsigned)kut->phase : 5;
    }
    if (kind >= COOP_BOSS_K_ROOL_DK && kind <= COOP_BOSS_K_ROOL_LANKY) {
        if (data->phase > 3) return 5;
        return data->phase == 3 && actor->control_state == 0x37 ? 4 : data->phase;
    }
    if (kind == COOP_BOSS_K_ROOL_TINY) {
        CoopKRoolFootData* foot = (CoopKRoolFootData*)data;
        if (foot->toe > 3) return 5;
        return actor->control_state == 0x37 ? foot->toe + 1 : foot->toe;
    }
    if (kind == COOP_BOSS_K_ROOL_CHUNKY) {
        if (data->phase > 3) return 5;
        return data->phase == 3 && actor->control_state == 0x37 ? 4 : data->phase;
    }
    return 5;
}
static unsigned combat_boss_step_ready(unsigned kind, Actor* actor, CoopBossData* data) {
    if (kind == COOP_BOSS_ARMY_DILLO || kind == COOP_BOSS_DOGADON)
        return actor->control_state >= 0x11 && actor->control_state <= (kind == COOP_BOSS_DOGADON ? 0x6A : 0x57)
            && actor->control_state != 0x37 && actor->control_state != 0x4D
            && (kind != COOP_BOSS_DOGADON || actor->control_state != 0x56)
            && D_global_asm_807FBB70.unk200 == 0;
    if (kind == COOP_BOSS_MAD_JACK)
        return actor->control_state == 0x25 || actor->control_state == 0x28;
    if (kind == COOP_BOSS_PUFFTOSS)
        return actor->control_state == 0x28 || actor->control_state == 0x2A
            || actor->control_state == 0x46;
    if (kind == COOP_BOSS_KING_KUT_OUT) {
        CoopKutOutData* kut = (CoopKutOutData*)data;
        return (kut->reaction == 0 || kut->reaction == 1) && kut->phase < 3;
    }
    if (kind == COOP_BOSS_K_ROOL_DK)
        return actor->control_state == 0x29 || actor->control_state == 0x2A;
    if (kind == COOP_BOSS_K_ROOL_DIDDY)
        return actor->control_state == 0x28;
    if (kind == COOP_BOSS_K_ROOL_LANKY)
        return actor->control_state == 0x2B && actor->control_state_progress == 3;
    if (kind == COOP_BOSS_K_ROOL_TINY) {
        CoopKRoolFootData* foot = (CoopKRoolFootData*)data;
        return actor->control_state == 0x28 && actor->control_state_progress <= 3
            && foot->toe < 4 && foot->attack == foot->toe && actor->unk138 == 0;
    }
    if (kind == COOP_BOSS_K_ROOL_CHUNKY)
        return actor->control_state == 0x29 && actor->control_state_progress == 6
            && data->phase < 4;
    return 0;
}
static void combat_boss_apply_step(unsigned kind, Actor* actor, CoopBossData* data) {
    if (kind == COOP_BOSS_ARMY_DILLO || kind == COOP_BOSS_DOGADON) {
        // Both pinned handlers treat collision result 4 as one TNT impact.
        D_global_asm_807FBB70.unk200 = 4;
        D_global_asm_807FBB70.unk1FC = NULL;
    } else if (kind == COOP_BOSS_MAD_JACK) {
        // Vanilla's hit reaction owns the phase increment at reaction step 8.
        actor->control_state = 0x27; actor->control_state_progress = 0;
    } else if (kind == COOP_BOSS_PUFFTOSS) {
        // Vanilla's ring-completion reaction owns the phase increment at step 4.
        actor->control_state = 0x4D; actor->control_state_progress = 0;
    } else if (kind == COOP_BOSS_KING_KUT_OUT) {
        // Enter the controller's normal three-hit reaction; it advances the
        // round only after its own animation and cutscene sequence completes.
        ((CoopKutOutData*)data)->reaction = 4;
    } else if (kind >= COOP_BOSS_K_ROOL_DK && kind <= COOP_BOSS_K_ROOL_LANKY) {
        // These three rounds converge through their own normal hurt state. The
        // overlay performs every counter increment and round transition.
        actor->control_state = 0x31; actor->control_state_progress = 0;
    } else if (kind == COOP_BOSS_K_ROOL_TINY) {
        // The shoe actor's terminal state sets exactly the current toe's
        // temporary flag and returns to Tiny's arena through vanilla loading.
        actor->control_state = 0x37; actor->control_state_progress = 0;
    } else if (kind == COOP_BOSS_K_ROOL_CHUNKY) {
        if (data->phase < 3) {
            // Continue at the exact post-punch wait used by the non-final hit.
            actor->control_state_progress = 8;
        } else {
            // The final hit's branch owns both terminal state and this flag.
            // Reproduce those two writes together; the rest remains vanilla.
            actor->control_state = 0x37; actor->control_state_progress = 0;
            setFlag(0x1B0, 1, 0); // PERMFLAG_PROGRESS_K_ROOL_DEFEATED
        }
    }
}
static void coop_boss_behavior(void) {
    Actor* actor = gCurrentActorPointer;
    const unsigned kind = actor ? combat_boss_actor_kind(actor->unk58) : COOP_BOSS_NONE;
    const unsigned index = kind ? kind - 1 : 0;
    CoopBossData* data = actor ? (CoopBossData*)actor->unk178 : NULL;
    const unsigned phase = combat_boss_phase(kind, actor, data);
    const unsigned bit = kind ? 1u << index : 0;
    const CoopBoss command = combat_result.boss;
    if (kind && (boss_hooks & bit) && boss_behaviors[index] == combat_boss_types[index].original
            && D_global_asm_8074C0A0[combat_boss_types[index].type] == coop_boss_behavior
            && data && command.kind == kind && command.life == combat_boss.life && command.peer_life
            && command.phase > phase && command.phase <= 4
            && combat_result.status == COOP_COMBAT_READY && combat_game_ready()
            && coop_boss_kind(current_map) == kind && combat_boss.actor == actor
            && combat_boss.kind == kind && combat_boss.generation == actor->unk54
            && combat_actor_live(actor, actor->unk54) && (actor->object_properties_bitfield & 0x10)
            && combat_boss_step_ready(kind, actor, data))
        combat_boss_apply_step(kind, actor, data);
    if (kind && boss_behaviors[index]) boss_behaviors[index]();
}
static void coop_boss_hook(void) {
    const unsigned kind = combat_enabled ? coop_boss_kind(current_map) : COOP_BOSS_NONE;
    if (!kind) { boss_hooks = 0; return; }
    const unsigned index = kind - 1, bit = 1u << index;
    const unsigned type = combat_boss_types[index].type;
    void (*current)(void) = D_global_asm_8074C0A0[type];
    if (current == coop_boss_behavior) {
        boss_hooks = boss_behaviors[index] == combat_boss_types[index].original ? bit : 0;
        return;
    }
    if (current == combat_boss_types[index].original) {
        boss_behaviors[index] = current;
        D_global_asm_8074C0A0[type] = coop_boss_behavior;
        boss_hooks = bit;
        return;
    }
    boss_hooks = 0;
}
static void combat_clear_pose_samples(void) {
    for (unsigned i = 0; i < 256; ++i) {
        combat_enemies[i].pose_peer_life = 0;
        combat_enemies[i].pose_stamp = 0;
    }
}
static void coop_combat_move_enemies(void) {
    if (role != ROLE_JOIN || combat_enabled < 2 || !(combat_result.movement & COOP_COMBAT_MOVEMENT) || !combat_game_ready()
            || !coop_combat_map(current_map) || combat_result.status != COOP_COMBAT_READY
            || D_807FDC88.count <= 0 || D_807FDC88.count > 256 || D_807FDC88.first != combat_spawner_table) {
        combat_clear_pose_samples();
        return;
    }
    const unsigned pose_context = combat_enabled == 3 && (combat_result.movement & COOP_COMBAT_POSE);
    if (!pose_context) combat_clear_pose_samples();
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
        const CoopEnemy* command = &combat_result.motion[i];
        if (!command->key || command->key > (unsigned)D_807FDC88.count || command->state != COOP_ENEMY_ALIVE) continue;
        CoopEnemySlot* slot = &combat_enemies[command->key - 1];
        Actor* actor = slot->actor;
        if (slot->life != command->life || slot->kind != command->kind || slot->defeated
                || D_807FDC88.first[command->key - 1].tied_actor != actor
                || !combat_actor_live(actor, slot->generation)) continue;
        if (combat_enemy_kind(actor->unk58) != command->kind || !(actor->object_properties_bitfield & 0x10)
                || !combat_damage_ready(actor)) continue;
        float x = bits_float(command->x), y = bits_float(command->y), z = bits_float(command->z);
        if (!(x >= -100000.0f && x <= 100000.0f && y >= -100000.0f && y <= 100000.0f
                && z >= -100000.0f && z <= 100000.0f)
                || (command->yaw & ~COOP_ENEMY_PACK_MASK) || !coop_enemy_health(*command)) continue;
        // Correct towards the host only; never advance an enemy during a gap in
        // host snapshots. Local AI/animation/collision still runs normally.
        float dx = x - actor->x_position, dy = y - actor->y_position, dz = z - actor->z_position;
        float blend = dx * dx + dy * dy + dz * dz > 250000.0f ? 1.0f : 0.4f;
        actor->x_position += dx * blend; actor->y_position += dy * blend; actor->z_position += dz * blend;
        actor->y_rotation = actor->unkEE = coop_enemy_yaw(*command);
        if (pose_context
                && coop_enemy_pose(*command) && actor->animation_state && actor->animation_state->unk0) {
            ActorAnimationState* animation = actor->animation_state;
            AnimationStateUnk0* track = animation->unk0;
            const unsigned peer_life = coop_enemy_peer_life(*command);
            const unsigned stamp = (coop_enemy_pose_hash(*command) << 5)
                | coop_enemy_pose(*command);
            if (track->unk0 && track->unk10 >= 0 && track->unk10 < 2048 && track->unk0->unk12
                    && peer_life && (slot->pose_peer_life != peer_life || slot->pose_stamp != stamp)
                    && coop_enemy_clip_hash((unsigned)track->unk10) == coop_enemy_pose_hash(*command)) {
                // Evaluate only the already-local clip at the host's normalized
                // frame. Suppress and restore callbacks so a visual correction
                // cannot synthesize an attack, sound, effect or state change.
                // Consume a reciprocally bound sample once so the local animation
                // advances normally between the companion's 20 Hz snapshots.
                s32 script = animation->unk68, sound = animation->unk6C, extra = animation->unk74;
                s32 (*callback)(Actor*) = animation->unk70;
                animation->unk68 = animation->unk6C = animation->unk74 = 0; animation->unk70 = NULL;
                func_global_asm_80614644(actor, track,
                    coop_enemy_pose_frame(coop_enemy_pose(*command), track->unk0->unk12));
                animation->unk68 = script; animation->unk6C = sound;
                animation->unk70 = callback; animation->unk74 = extra;
                slot->pose_peer_life = peer_life; slot->pose_stamp = stamp;
            }
        }
    }
}
static void coop_combat_init(void) {
    if (!combat_enabled) return;
    for (unsigned i = 0; i < COOP_ENEMY_KIND_COUNT; ++i) {
        enemy_behaviors[i] = D_global_asm_8074C0A0[combat_enemy_types[i].type];
        if (enemy_behaviors[i] != combat_enemy_types[i].original) {
            recomp_printf("[dk64-coop] Enemy behavior modified by another mod; enemy synchronization disabled.\n");
            return;
        }
    }
    for (unsigned i = 0; i < COOP_ENEMY_KIND_COUNT; ++i)
        D_global_asm_8074C0A0[combat_enemy_types[i].type] = combat_enemy_behavior;
    combat_hooks = 1;
}
static void combat_retire_shot(CoopShotSlot* slot) {
    if (slot->actor && actor_is_alive(slot->actor, slot->generation)) {
        slot->retiring = slot->actor; slot->retiring_generation = slot->generation;
        deleteActor(slot->actor);
    }
    slot->actor = NULL; slot->id = 0;
}
static unsigned coop_shot_behavior(Actor* actor) {
    for (unsigned i = 0; i < COOP_SHOTS; ++i) {
        CoopShotSlot* slot = &combat_shots[i];
        if (actor == slot->retiring && actor->unk54 == slot->retiring_generation) return 1;
        if (actor == slot->actor && actor->unk54 == slot->generation) {
            actor->interactable = 0; actor->noclip_byte = 1; actor->unk138 = 0;
            if (actor->animation_state) renderActor(actor, 0);
            return 1;
        }
    }
    return 0;
}
static void coop_combat_capture(void) {
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
        const CoopEnemy* done = &combat_result.apply[i];
        if (done->state != COOP_ENEMY_ABSENT || !done->key || done->key > 256) continue;
        CoopEnemySlot* slot = &combat_enemies[done->key - 1];
        if (slot->defeated && slot->life == done->life && slot->kind == done->kind)
            slot->defeated = 0;
    }
    combat_input = (CoopCombatFrame){0};
    if (combat_map != (s32)current_map) {
        combat_map = current_map; combat_layout = 0;
        combat_capture_page = 0;
        combat_spawner_table = NULL;
        combat_boss = (CoopBossSlot){0};
        for (unsigned i = 0; i < 256; ++i) combat_enemies[i] = (CoopEnemySlot){0};
        for (unsigned i = 0; i < COOP_SHOTS; ++i) combat_retire_shot(&combat_shots[i]);
        for (unsigned i = 0; i < COOP_SHOTS; ++i) combat_local_shots[i] = (CoopLocalShotSlot){0};
    }
    if (combat_enabled && game_mode == GAME_MODE_ADVENTURE && current_file < 3) {
        if (!combat_file) combat_file = current_file + 1;
        else if (combat_file != current_file + 1) combat_file_changed = 1;
    }
    if (loading_zone_transition_speed != 0.0f) {
        // Invalidate before unloading, including a reload into the same map.
        combat_layout = 0; combat_spawner_table = NULL;
        combat_boss = (CoopBossSlot){0};
        for (unsigned i = 0; i < 256; ++i) combat_enemies[i] = (CoopEnemySlot){0};
        for (unsigned i = 0; i < COOP_SHOTS; ++i) combat_local_shots[i] = (CoopLocalShotSlot){0};
    }
    coop_boss_hook();
    if (!combat_game_ready() || D_global_asm_807FBB34 > 64) return;
    combat_input.enabled = combat_enabled; combat_input.file = combat_file; combat_input.hands = gPlayerPointer->unk146;
    combat_input.pages = 1;
    const unsigned boss_kind = coop_boss_kind(current_map);
    const unsigned boss_bit = boss_kind ? 1u << (boss_kind - 1) : 0;
    if (boss_kind && (boss_hooks & boss_bit)) {
        Actor* actor = NULL;
        const unsigned boss_type = combat_boss_types[boss_kind - 1].type;
        for (unsigned i = 0; i < D_global_asm_807FBB34; ++i) {
            Actor* candidate = D_global_asm_807FB930[i].actor;
            if (candidate && candidate->unk58 == boss_type
                    && (candidate->object_properties_bitfield & 0x10)) { actor = candidate; break; }
        }
        CoopBossData* data = actor ? (CoopBossData*)actor->unk178 : NULL;
        const unsigned phase = combat_boss_phase(boss_kind, actor, data);
        if (actor && data && phase <= 4) {
            if (combat_boss.actor != actor || combat_boss.generation != actor->unk54
                    || combat_boss.kind != boss_kind) {
                combat_boss = (CoopBossSlot){actor, actor->unk54, 0, phase, boss_kind};
                combat_life_counter = (combat_life_counter + 1) & COOP_ENEMY_LIFE_MASK;
                if (!combat_life_counter) combat_life_counter = 1;
                combat_boss.life = combat_life_counter;
            }
            if (phase >= combat_boss.phase) combat_boss.phase = phase;
        }
        if (combat_boss.life && combat_boss.kind == boss_kind
                && (phase <= 4 || combat_boss.phase == 4))
            combat_input.boss = (CoopBoss){boss_kind, combat_boss.life, 0, combat_boss.phase};
    }
    if (coop_combat_map(current_map) && combat_hooks && D_807FDC88.count > 0 && D_807FDC88.count <= 256 && D_807FDC88.first) {
        {
            u32 hash = 2166136261u;
            // Immutable spawn identity fields only; speed/AI/respawn fields mutate.
            for (unsigned i = 0; i < (unsigned)D_807FDC88.count; ++i) {
                SpawnerFileData* init = &D_807FDC88.first[i].init;
                unsigned fields[] = {i, init->enemy_value, (u16)init->x_pos, (u16)init->y_pos, (u16)init->z_pos};
                for (unsigned j = 0; j < 5; ++j) { hash ^= fields[j]; hash *= 16777619u; }
            }
            hash = hash ? hash : 1;
            if (combat_layout != hash || combat_spawner_table != D_807FDC88.first) {
                for (unsigned i = 0; i < 256; ++i) combat_enemies[i] = (CoopEnemySlot){0};
                combat_capture_page = 0;
                combat_layout = hash; combat_spawner_table = D_807FDC88.first;
            }
        }
        combat_input.layout = combat_layout;
        unsigned eligible = 0;
        for (unsigned i = 0; i < (unsigned)D_807FDC88.count; ++i) {
            CoopEnemySlot* slot = &combat_enemies[i];
            Actor* actor = D_807FDC88.first[i].tied_actor;
            unsigned live = 0;
            for (unsigned a = 0; a < D_global_asm_807FBB34; ++a)
                if (actor && D_global_asm_807FB930[a].actor == actor) { live = 1; break; }
            if (live && combat_enemy_kind(actor->unk58) && (actor->object_properties_bitfield & 0x10)) {
                unsigned kind = combat_enemy_kind(actor->unk58);
                if (slot->actor != actor || slot->generation != actor->unk54 || slot->kind != kind) {
                    slot->actor = actor; slot->generation = actor->unk54;
                    slot->kind = kind;
                    combat_life_counter = (combat_life_counter + 1) & COOP_ENEMY_LIFE_MASK;
                    if (!combat_life_counter) combat_life_counter = 1;
                    slot->life = combat_life_counter;
                    slot->defeated = 0;
                    slot->pose_peer_life = slot->pose_stamp = 0;
                }
                if (actor->health <= 0 && !slot->defeated) live = 0; // Unobserved/scripted death is not replicated.
            } else live = 0;
            if (slot->life && (live || slot->defeated)) ++eligible;
        }
        combat_input.pages = (eligible + COOP_ENEMIES - 1) / COOP_ENEMIES;
        if (!combat_input.pages) combat_input.pages = 1;
        combat_input.page = combat_capture_page++ % combat_input.pages;
        const unsigned first = combat_input.page * COOP_ENEMIES;
        unsigned seen = 0, n = 0;
        for (unsigned i = 0; i < (unsigned)D_807FDC88.count && n < COOP_ENEMIES; ++i) {
            CoopEnemySlot* slot = &combat_enemies[i];
            Actor* actor = slot->actor;
            const unsigned live = actor && D_807FDC88.first[i].tied_actor == actor
                && combat_actor_live(actor, slot->generation)
                && combat_enemy_kind(actor->unk58) == slot->kind
                && (actor->object_properties_bitfield & 0x10) && actor->health > 0;
            if (!slot->life || (!live && !slot->defeated)) continue;
            if (seen++ < first) continue;
            {
                CoopEnemy* state = &combat_input.enemies[n++];
                *state = (CoopEnemy){i + 1, slot->life, slot->defeated ? COOP_ENEMY_DEFEATED : COOP_ENEMY_ALIVE, 0, slot->kind, 0, 0, 0, 0};
                if (live && !slot->defeated) {
                    state->x = float_bits(actor->x_position); state->y = float_bits(actor->y_position);
                    state->z = float_bits(actor->z_position);
                    unsigned pose = 0, clip_hash = 0;
                    if (combat_enabled == 3 && actor->animation_state && actor->animation_state->unk0) {
                        AnimationStateUnk0* track = actor->animation_state->unk0;
                        if (track->unk0 && track->unk10 >= 0 && track->unk10 < 2048 && track->unk0->unk12) {
                            pose = coop_enemy_pose_encode(track->unk4, track->unk0->unk12);
                            if (pose) clip_hash = coop_enemy_clip_hash((unsigned)track->unk10);
                        }
                    }
                    state->peer_life = clip_hash << COOP_ENEMY_POSE_HASH_SHIFT;
                    state->yaw = coop_enemy_pack_pose((unsigned)actor->y_rotation, (unsigned)actor->health, pose);
                }
            }
        }
    }
    enum { COOP_SHOT_HOLD_FRAMES = 6 };
    for (unsigned i = 0; i < COOP_SHOTS; ++i) if (combat_local_shots[i].shot.id) {
        if (++combat_local_shots[i].missed > COOP_SHOT_HOLD_FRAMES)
            combat_local_shots[i] = (CoopLocalShotSlot){0};
    }
    for (unsigned i = 0; i < D_global_asm_807FBB34; ++i) {
        Actor* actor = D_global_asm_807FB930[i].actor;
        if (!actor || actor->unk11C != gPlayerPointer || !(actor->object_properties_bitfield & 0x10)) continue;
        unsigned kind = combat_projectile_kind(actor->unk58);
        if (!kind) continue;
        float scale = actor->unk124 ? actor->unk124->unkC : 1.0f;
        if (!(scale >= 0.01f && scale <= 4.0f)) scale = 1.0f;
        unsigned id = actor->unk54 + 1; if (!id) id = 1;
        CoopLocalShotSlot* slot = NULL;
        for (unsigned s = 0; s < COOP_SHOTS; ++s)
            if (combat_local_shots[s].shot.id == id && combat_local_shots[s].shot.kind == kind) {
                slot = &combat_local_shots[s]; break;
            }
        if (!slot) for (unsigned s = 0; s < COOP_SHOTS; ++s)
            if (!combat_local_shots[s].shot.id) { slot = &combat_local_shots[s]; break; }
        if (!slot) continue;
        slot->shot = (CoopShot){id, kind, float_bits(actor->x_position), float_bits(actor->y_position),
            float_bits(actor->z_position), (unsigned)actor->unkEE & 0xFFF, float_bits(scale)};
        slot->missed = 0;
    }
    unsigned n = 0;
    for (unsigned i = 0; i < COOP_SHOTS && n < COOP_SHOTS; ++i)
        if (combat_local_shots[i].shot.id) combat_input.shots[n++] = combat_local_shots[i].shot;
}
static void coop_combat_render(unsigned visible) {
    static const unsigned types[6] = {ACTOR_PROJECTILE_COCONUTS, ACTOR_PROJECTILE_PEANUT, ACTOR_PROJECTILE_GRAPE,
        ACTOR_PROJECTILE_FEATHER, ACTOR_PROJECTILE_PINEAPPLE, ACTOR_PROJECTILE_ORANGE};
    static void* const sprites[6] = {D_global_asm_807206A8, D_global_asm_807200A0, D_global_asm_8072029C,
        NULL, D_global_asm_807201A0, D_global_asm_80720268};
    for (unsigned i = 0; i < COOP_SHOTS; ++i) {
        CoopShotSlot* slot = &combat_shots[i];
        if (!actor_is_alive(slot->actor, slot->generation)) slot->actor = NULL;
        if (!actor_is_alive(slot->retiring, slot->retiring_generation)) slot->retiring = NULL;
        const CoopShot* shot = NULL;
        if (visible) for (unsigned s = 0; s < COOP_SHOTS; ++s)
            if (combat_result.shots[s].id == slot->id && slot->id) { shot = &combat_result.shots[s]; break; }
        if (!shot || shot->kind != slot->kind) combat_retire_shot(slot);
    }
    if (!visible) return;
    for (unsigned s = 0; s < COOP_SHOTS; ++s) {
        const CoopShot* shot = &combat_result.shots[s];
        if (!shot->id || shot->kind < 1 || shot->kind > 6) continue;
        CoopShotSlot* slot = NULL;
        for (unsigned i = 0; i < COOP_SHOTS; ++i)
            if (combat_shots[i].id == shot->id && combat_shots[i].kind == shot->kind) { slot = &combat_shots[i]; break; }
        if (!slot) for (unsigned i = 0; i < COOP_SHOTS; ++i)
            if (!combat_shots[i].actor && !combat_shots[i].retiring && !combat_shots[i].id) { slot = &combat_shots[i]; break; }
        if (!slot) continue;
        float x = bits_float(shot->x), y = bits_float(shot->y), z = bits_float(shot->z);
        if (!slot->actor && D_global_asm_807FBB34 < 56) {
            int model = -1;
            for (unsigned m = 0; m < 128; ++m)
                if ((unsigned)D_global_asm_8074E8B0[m].type == types[shot->kind - 1]) { model = D_global_asm_8074E8B0[m].model; break; }
            if (model < 0 || model > 512) continue;
            Actor* saved_last = gLastSpawnedActor;
            // Sprite projectiles have model zero. Use the engine's model-less
            // initializer; the 3D initializer would index model asset -1.
            if (model ? spawnActor(ACTOR_PUSHABLE_BOX, model) : func_global_asm_80678014(ACTOR_PUSHABLE_BOX)) {
                slot->actor = gLastSpawnedActor; slot->generation = slot->actor->unk54;
                slot->id = shot->id; slot->kind = shot->kind;
                Actor* actor = slot->actor;
                actor->object_properties_bitfield |= 0x10; actor->interactable = 0; actor->unk138 = 0;
                actor->noclip_byte = 1; actor->shadow_opacity = 0; actor->draw_distance = 2000;
                actor->x_position = x; actor->y_position = y; actor->z_position = z;
                if (!model) {
                    // Follow the engine's sprite initialization: bone 1 is an
                    // origin attachment, not a collision sphere. The box type
                    // does not receive the sprite type's automatic transform.
                    guTranslateF(actor->unkC, x, y, z);
                    actor->ledge_info_pointer = func_global_asm_80665F24(actor);
                    if (!actor->ledge_info_pointer) {
                        combat_retire_shot(slot); gLastSpawnedActor = saved_last; continue;
                    }
                    func_global_asm_8066E854(actor, 0.0f, 0.0f, 0.0f, -1);
                }
                if (actor->animation_state) {
                    actor->animation_state->scale_x = actor->animation_state->scale_y = actor->animation_state->scale_z = bits_float(shot->scale) * 0.15f;
                    if (shot->kind == COOP_FEATHER) func_global_asm_80613CA8(actor, 0x404, 0.0f, 0.0f);
                }
                if (sprites[shot->kind - 1]) {
                    func_global_asm_807149FC(-1); func_global_asm_807149B8(1); func_global_asm_80714A28(4);
                    func_global_asm_80714C08(sprites[shot->kind - 1], bits_float(shot->scale), actor, 1, 2);
                }
            }
            gLastSpawnedActor = saved_last;
        }
        if (slot->actor) {
            // Fast projectiles use the latest observed position; no invented collision or extrapolation.
            slot->actor->x_position = x; slot->actor->y_position = y; slot->actor->z_position = z;
            slot->actor->y_rotation = shot->yaw;
            if (!slot->actor->animation_state) guTranslateF(slot->actor->unkC, x, y, z);
        }
    }
}
#endif
