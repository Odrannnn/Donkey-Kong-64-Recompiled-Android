#include "common_structs.h"

typedef struct Struct807FDC90 Struct807FDC90;
struct Struct807FDC90 {
    Struct807FDC90 *unk0;
    Actor *unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    u16 unk18;
    u16 unk1A;
    union {
        struct {
            u16 unk1C;
            u8 unk1E;
            u8 unk1F;
        };
        s32 unk1C_s32;
    };
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
    s16 unk26;
    s32 unk28;
    s16 unk2C;
    s16 unk2E;
    f32 unk30;
    u8 unk34;
    u8 unk35;
    u8 unk36;
    u8 unk37;
    u8 unk38;
};

typedef struct enemy_info {
    u8 enemy_type; // at 0x00
    u8 unk1;
    u16 y_rotation; // at 0x02
    s16 x_position; // at 0x04
    s16 y_position; // at 0x06
    s16 z_position; // at 0x08
    u8 cutscene_model_index; // at 0x0A
    u8 unkB;
    u32 unkC;
    u16 unk10;
    u8 unk12;
    u8 unk13;
    u32 unk14;
    Actor* unk18; // TODO: Is this accurate?
    u32 unk1C;
    Struct80027840* unk20;
    u16 unk24;
    u16 unk26;
    u16 unk28; // Used
    s16 unk2A; // Used
    u32 unk2C;
    f32 unk30; // Used
    f32 unk34; // Used
    f32 unk38; // Used
    f32 unk3C; // at 0x3C
    s16 unk40; // Used
    s16 unk42;
    u8 unk44; // Used
    u8 unk45;
    u16 unk46; // Used
} EnemyInfo;

typedef struct {
    u8 pad0[0x2 - 0x0];
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    union {
        u8 unkA_u8[2];
        u16 unkA_u16;
        s16 unkA_s16;
    };
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
} CharacterSpawner;

u8 isFlagSet(s16 flagIndex, u8 flagType);
void initializeCharacterSpawnerActor(void);
void func_global_asm_80724CA4(s16 arg0, s16 arg1);
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern Actor *gCurrentActorPointer;
extern EnemyInfo *D_global_asm_807FDC98;
void playActorAnimation(Actor *arg0, s32 arg1);
void func_global_asm_80728950(u8 arg0);
void func_global_asm_80684850(u8 arg0);
s32 func_global_asm_80629148(void);
void func_global_asm_8063DA40(s16 arg0, s16 arg1);
void func_global_asm_80672C30(Actor *arg0);
void func_global_asm_80726EE0(u8 arg0);
void func_global_asm_806BE674(u8 arg0);
void func_global_asm_807289B0(u8 arg0, u8 arg1);
void func_global_asm_80641874(s16 arg0, s16 arg1);
void loadText(Actor *arg0, u16 fileIndex, u8 textIndex);
void func_global_asm_8061CAD8(void);
void addActorToTextOverlayRenderArray(void *arg0, Actor *arg1, u8 arg2);
Gfx *func_global_asm_806BE6F0(Gfx *dl, Actor *arg1);
void playSong(MUSIC_E arg0, f32 arg1);
extern s32 D_global_asm_807FBB64;
void func_global_asm_8061CB08(void);
void func_global_asm_8062217C(Actor*, s16);
extern Actor *D_global_asm_807F5D10;
void func_global_asm_8072B324(Actor *arg0, s32 arg1);
extern CharacterSpawner *D_global_asm_807FDC9C;
void func_global_asm_8061F510(u8 arg0, u8 arg1);
void func_global_asm_80602B60(s32 arg0, u8 arg1);
s32 playCutscene(Actor *arg0, s16 arg1, u8 arg2);
s16 func_global_asm_80665DE0(f32 arg0, f32 arg1, f32 arg2, f32 arg3);
void func_global_asm_8070E8DC(u8 arg0);
extern Struct807FDC90 *D_global_asm_807FDC90;
void func_global_asm_806A5DF0(s16 arg0, f32 x, f32 y, f32 z, s16 arg4, u8 arg5, s16 arg6, s32 arg7);
void setFlag(s16 flagIndex, u8 newValue, u8 flagType);
void func_global_asm_8072DC7C(u8 arg0);
void func_global_asm_8072EC94(s32 arg0, u8 arg1);
void func_global_asm_806ACC00(u8 arg0);
Gfx *func_global_asm_8068E474(Gfx *dl, Actor *arg1);
extern GlobalASMStruct35 D_global_asm_807FBB70;
void func_global_asm_8072881C(s32, void*);
u8 func_global_asm_8072AB74(u8 arg0, f32 x, f32 z, u16 arg3, f32 arg4);
u8 func_global_asm_8072D13C(u8 arg0, s32 arg1);
void func_global_asm_80724E48(u8 arg0);
void func_global_asm_8072A450(void);
void renderActor(Actor *arg0, u8 arg1);
extern u32 D_global_asm_80744478;
extern Actor *gPlayerPointer;
u32 func_global_asm_806119A0(void);
extern u32 object_timer;
extern u8 D_global_asm_807506C0[];

#define ACTOR_UNINITIALIZED(actor) (!(actor->object_properties_bitfield & 0x10))
#define RANDNUM() (func_global_asm_806119A0() & 0x7FFFFFFF)
#define RandClamp(a) ((RANDNUM() >> 0xF) % a)
#define RandChance(a) (RandClamp(1000) > (1000 - ((s32)((a) * 1000))))

RECOMP_PATCH void func_global_asm_806BE8BC(void) {
    u8 sp37;
    s16 j;
    s16 i;
    s16 var_v0;

    sp37 = isFlagSet(PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE, FLAG_TYPE_PERMANENT);
    initializeCharacterSpawnerActor();
    if (extra_player_info_pointer->unk1F0 & 0x100000) {
        gCurrentActorPointer->control_state = 0x40;
        return;
    }
    if (ACTOR_UNINITIALIZED(gCurrentActorPointer)) {
        func_global_asm_80724CA4(2, 1);
        D_global_asm_807FDC98->unk46 |= 0x20;
        playActorAnimation(gCurrentActorPointer, 0x306);
        func_global_asm_80728950(0);
        gCurrentActorPointer->control_state = 0x1E;
        gCurrentActorPointer->control_state_progress = 0;
        gCurrentActorPointer->unk15F = 0;
    }
    switch (gCurrentActorPointer->control_state) {
        case 0x26:
            playActorAnimation(gCurrentActorPointer, 0x307);
            gCurrentActorPointer->control_state = 0x27;
            gCurrentActorPointer->control_state_progress = 0;
            break;
        case 0x1E:
            func_global_asm_80684850(1);
            if (gCurrentActorPointer->shadow_opacity < 0xFF) {
                gCurrentActorPointer->shadow_opacity += 5;
            }
            if (func_global_asm_80629148()) {
                func_global_asm_8063DA40(0x1F, 3);
                func_global_asm_80672C30(gPlayerPointer);
                func_global_asm_80726EE0(1);
                D_global_asm_807FDC98->unk46 |= 4;
                func_global_asm_806BE674(1);
                func_global_asm_807289B0(0, 0);
                if (sp37) {
                    func_global_asm_80641874(0x17, 0x14);
                    loadText(gCurrentActorPointer, 0x14, 1);
                } else {
                    loadText(gCurrentActorPointer, 0x14, 0);
                }
                playActorAnimation(gCurrentActorPointer, 0x309);
                gCurrentActorPointer->control_state = 0x1F;
                gCurrentActorPointer->control_state_progress = 0;
            }
            break;
        case 0x1F:
            switch (gCurrentActorPointer->control_state_progress) {
                case 0:
                    if (func_global_asm_80629148()) {
                        func_global_asm_8061CAD8();
                        func_global_asm_8061CAD8();
                        gCurrentActorPointer->control_state_progress++;
                        gCurrentActorPointer->unk168 = 0x78;
                    }
                    break;
                case 1:
                    addActorToTextOverlayRenderArray(func_global_asm_806BE6F0, gCurrentActorPointer, 3);
                    break;
                case 2:
                    playSong(MUSIC_169_FUNGI_FOREST_RABBIT_RACE, 1.0f);
                    D_global_asm_807FBB64 |= 4;
                    func_global_asm_8061CB08();
                    func_global_asm_8062217C(D_global_asm_807F5D10, 3);
                    playActorAnimation(gCurrentActorPointer, 0x302);
                    func_global_asm_8072B324(gCurrentActorPointer, (sp37 ? 1.5 : 1.0) * D_global_asm_807FDC9C->unkD);
                    gCurrentActorPointer->control_state = 2;
                    gCurrentActorPointer->control_state_progress = 0;
                    func_global_asm_8061F510(1, 0xA);
                    extra_player_info_pointer->unk1F4 |= 0x40;
                    break;
            }
            break;
        case 0x27:
            if (gCurrentActorPointer->control_state_progress == 0) {
                func_global_asm_80602B60(0xA9, 0);
                gCurrentActorPointer->object_properties_bitfield &= ~0x8000;
                gCurrentActorPointer->shadow_opacity = 0xFF;
                func_global_asm_806BE674(0);
                playCutscene(gCurrentActorPointer, 0xF, 5);
                gCurrentActorPointer->y_rotation = func_global_asm_80665DE0(gPlayerPointer->x_position, gPlayerPointer->z_position, gCurrentActorPointer->x_position, gCurrentActorPointer->z_position);
                loadText(gCurrentActorPointer, 0x14, 4);
                gCurrentActorPointer->control_state = 0x37;
                gCurrentActorPointer->control_state_progress = 0;
                gCurrentActorPointer->y_position = gCurrentActorPointer->floor;
            }
            break;
        case 0x28:
            switch (gCurrentActorPointer->control_state_progress) {
                case 0:
                    func_global_asm_80602B60(0xA9, 0);
                    gCurrentActorPointer->object_properties_bitfield &= ~0x8000;
                    gCurrentActorPointer->shadow_opacity = 0xFF;
                    func_global_asm_806BE674(0);
                    playCutscene(gCurrentActorPointer, 0x1F, 1);
                    gCurrentActorPointer->y_rotation = func_global_asm_80665DE0(gPlayerPointer->x_position, gPlayerPointer->z_position, gCurrentActorPointer->x_position, gCurrentActorPointer->z_position);
                    playActorAnimation(gCurrentActorPointer, 0x308);
                    func_global_asm_8070E8DC(1);
                    if (sp37) {
                        loadText(gCurrentActorPointer, 0x14, 3);
                    } else {
                        loadText(gCurrentActorPointer, 0x14, 2);
                    }
                    gCurrentActorPointer->control_state_progress = 1;
                    gCurrentActorPointer->y_position = gCurrentActorPointer->floor;
                    break;
                case 1:
                    if (func_global_asm_80629148()) {
                        if (sp37) {
                            func_global_asm_8063DA40(0x57, 0xA);
                            D_global_asm_807FDC90->unk1A |= 0x8000;
                        } else {
                            var_v0 = gCurrentActorPointer->y_rotation - 0x12C;
                            playSong(MUSIC_47_MELON_SLICE_DROP, 1.0f);
                            for (i = 0; i < 3; i++, var_v0 += 0x12C) {
                                func_global_asm_806A5DF0(
                                    0x35,
                                    gCurrentActorPointer->x_position,
                                    gCurrentActorPointer->y_position,
                                    gCurrentActorPointer->z_position,
                                    var_v0,
                                    0x63,
                                    -1,
                                    0
                                );
                            }
                        }
                        setFlag(PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE, TRUE, FLAG_TYPE_PERMANENT);
                        gCurrentActorPointer->control_state = 0x37;
                        gCurrentActorPointer->control_state_progress = 0;
                        gCurrentActorPointer->y_position = gCurrentActorPointer->floor;
                    }
                    break;
            }
            break;
        case 0x37:
            if (!(gCurrentActorPointer->object_properties_bitfield & 0x02000000)) {
                func_global_asm_8072DC7C(2);
                if (gCurrentActorPointer->control_state_progress != 0) {
                    D_global_asm_807FBB64 &= ~4;
                    extra_player_info_pointer->unk23F = 2;
                    func_global_asm_8061F510(1, 0);
                    extra_player_info_pointer->unk1F4 &= ~0x40;
                    func_global_asm_8072EC94(0x16, 0);
                    func_global_asm_80726EE0(2);
                    if (D_global_asm_807FDC90->unk1A & 0x8000) {
                        gCurrentActorPointer->control_state = 0x40;
                    } else {
                        if (gCurrentActorPointer->animation_state->unk64 != 0x308) {
                            func_global_asm_806ACC00(2);
                        }
                        func_global_asm_8063DA40(0x1F, 1);
                        gCurrentActorPointer->control_state = 0x3C;
                    }
                }
            }
            break;
        case 0x13:
            playActorAnimation(gCurrentActorPointer, 0x305);
            gCurrentActorPointer->y_velocity = 200.0f;
            gCurrentActorPointer->control_state = 2;
            gCurrentActorPointer->control_state_progress = 0;
            // fallthrough
        default:
            if (gCurrentActorPointer->unk15F < 0x10U) {
                addActorToTextOverlayRenderArray(func_global_asm_8068E474, gCurrentActorPointer, 3);
                //@recomp: Change rabbit speed based on lag boost. Normal N64 Lag here is 2.2
                func_global_asm_8072B324(gCurrentActorPointer, (sp37 ? 1.5 : 1.0) * ((f32)D_global_asm_80744478 / 2.2f) * D_global_asm_807FDC9C->unkD);
            }
            if ((RandChance(0.01)) && (gCurrentActorPointer->animation_state->unk64 == 0x302)) {
                playActorAnimation(gCurrentActorPointer, (object_timer & 1) ? 0x303 : 0x304);
            }
            if (D_global_asm_807FBB70.unk0 != 0) {
                if ((D_global_asm_807FBB70.unk1 >= D_global_asm_807506C0[gCurrentActorPointer->unk15F]) && (gCurrentActorPointer->unk15F < 0x10U)) {
                    if (D_global_asm_807FBB70.unk1 != D_global_asm_807506C0[gCurrentActorPointer->unk15F]) {
                        playActorAnimation(gCurrentActorPointer, 0x301);
                        gCurrentActorPointer->control_state = 0x27;
                        gCurrentActorPointer->control_state_progress = 0;
                    } else {
                        gCurrentActorPointer->unk15F++;
                        if (gCurrentActorPointer->unk15F >= 0x10U) {
                            func_global_asm_8072EC94(0x16, 1);
                        }
                    }
                }
            }
            for (j = 0; j < D_global_asm_807FBB70.unk254; j++) {
                if (D_global_asm_807FBB70.unk258[j] == 3) {
                    gCurrentActorPointer->control_state = 0x28;
                    gCurrentActorPointer->control_state_progress = 0;
                }
            }
            func_global_asm_8072881C(0, &D_global_asm_807FDC90->unk28);
            func_global_asm_8072AB74(gCurrentActorPointer->control_state, D_global_asm_807FDC90->unkA, D_global_asm_807FDC90->unkE, 0x10, 0);
            func_global_asm_8072D13C(gCurrentActorPointer->control_state, 0);
            break;
    }
    if ((gCurrentActorPointer->animation_state->unk64 == 0x301) || (gCurrentActorPointer->animation_state->unk64 == 0x302)) {
        func_global_asm_80724E48(0);
    }
    func_global_asm_8072A450();
    renderActor(gCurrentActorPointer, 0);
}
