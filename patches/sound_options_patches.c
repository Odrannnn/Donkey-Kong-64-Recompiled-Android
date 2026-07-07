#include "patches.h"
#include "PR/os_message.h"
#include "enums.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "misc_funcs.h"
#include "debug_config.h"
#include "sound.h"
#include "options.h"

extern s8 D_global_asm_80745840;
extern s8 D_global_asm_8074583C;
extern void func_global_asm_8060A398(s32 arg0);
extern void func_global_asm_80737B58(u8 arg0, u16 arg1);

void AlterVolumes(void) {
    s32 i;
    s8 new_bgm_vol;
    s8 new_sfx_vol;

    //@recomp: Music Volume
    new_bgm_vol = recomp_get_bgm_volume();
    if (D_global_asm_80745840 != new_bgm_vol) {
        D_global_asm_80745840 = new_bgm_vol;
        func_global_asm_8060A398(0);
        func_global_asm_8060A398(2);
    }
    new_sfx_vol = recomp_get_sfx_volume();
    if (D_global_asm_8074583C != new_sfx_vol) {
        D_global_asm_8074583C = new_sfx_vol;
        for (i = 0; i < 4; i++) {
            func_global_asm_80737B58(i, new_sfx_vol * 625);
        }
    }
}

extern u16 D_global_asm_80744734[];
extern u8 isFlagSet(s16 flagIndex, u8 flagType);
extern void setFlag(s16 flagIndex, u8 newValue, u8 flagType);
extern void func_global_asm_805FF378(Maps nextMap, s32 nextExit);
extern void func_global_asm_80712524(Maps newMap, s32 cutsceneIndex);
extern Maps current_map;
extern u8 game_mode;

// @recomp: Level Intro Story Skip reads
RECOMP_PATCH s32 func_global_asm_8068ABE0(s16 arg0) {
    s16 cutsceneIndex;
    s32 sp20;
    s32 found;
    s32 i;
    Maps map;
    s32 exit; // 20

    cutsceneIndex = -1;
    i = 0, found = 0;
    while (!found && i < 8) {
        if (current_map == D_global_asm_80744734[i]) {
            found = 1;
        } else {
            i++;
        }
    }
    if (found) {
        if (!isFlagSet(PERMFLAG_LEVEL_ENTERED_JAPES + i, FLAG_TYPE_PERMANENT)) {
            exit = 0;
            switch (arg0) {
                case 0x7:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0xF;
                    break;
                case 0x26:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x10;
                    break;
                case 0x1A:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x11;
                    break;
                case 0x1E:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x12;
                    break;
                case 0x30:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x13;
                    break;
                case 0x48:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x14;
                    break;
                case 0x57:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x15;
                    exit = 0x15;
                    break;
                case 0x11:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x16;
                    break;
            }
            if (cutsceneIndex != -1) {
                setFlag(PERMFLAG_LEVEL_ENTERED_JAPES + i, TRUE, FLAG_TYPE_PERMANENT);
                //@recomp: change story_skip to the function read
                if (recomp_get_story_skip() == 2) {
                    func_global_asm_805FF378(arg0, exit);
                    return TRUE;
                } else {
                    func_global_asm_80712524(map, cutsceneIndex);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

extern OSTime D_global_asm_807445B8;
extern u8 D_global_asm_80755350;
extern void func_global_asm_806C9AE0(void);
extern void func_global_asm_80731030(void);
extern void func_global_asm_8060DC3C(u8 fileIndex, s32 arg1);
extern u8 current_character_index[];
extern s32 func_global_asm_8060C6B8(s32 arg0, u8 arg1, u8 arg2, u8 fileIndex);
extern u8 current_file;
extern s32 D_global_asm_80755338;
extern s32 D_global_asm_8075533C;
extern void func_global_asm_805FF4D8(Maps map, s32 exit);

// @recomp: Intro Story Story Skip Read
RECOMP_PATCH void func_global_asm_807144B8(s8 arg0) {
    Maps map;
    s32 exit;

    D_global_asm_807445B8 = osGetTime();
    D_global_asm_80755350 = 0;
    func_global_asm_806C9AE0();
    func_global_asm_80731030(); // clearTemporaryFlags()
    func_global_asm_8060DC3C(arg0, 1);
    current_character_index[0] = 0; // DK
    //@recomp: change story_skip to the function read
    if (func_global_asm_8060C6B8(0xD, 0, 0, current_file) || (recomp_get_story_skip() > 0)) {
        // Flag: Isles: Escape Cutscene
        if (isFlagSet(PERMFLAG_CUTSCENE_ISLES_FTCS, FLAG_TYPE_PERMANENT)) {
            map = MAP_DK_ISLES_OVERWORLD;
            exit = 0;
        } else {
            map = MAP_TRAINING_GROUNDS;
            exit = 1;
        }
    } else {
        D_global_asm_80755338 = 1;
        map = MAP_DK_ISLES_DK_THEATRE;
        D_global_asm_8075533C = 0;
        exit = 0;
    }
    func_global_asm_805FF4D8(map, exit); // initMapChange()
    game_mode = GAME_MODE_ADVENTURE;
}

typedef struct tuple_f {
    union {
        struct {
            f32 x;
            f32 y;
            f32 z;
        };
        f32 f[3];
    };
} tuple_f;

typedef struct actor Actor;
typedef struct player_additional_actor_data {
    s16 unk0; // Used
    s16 unk2;
    f32 unk4;
    s16 unk8; // Used
    s16 unkA; // Used, Actor->y_rotation copied here
    s16 unkC; // Used
    s16 unkE; // Used
    s16 unk10; // Used
    s16 unk12; // Used
    s16 unk14; // Used
    union {
        struct {
            s8  unk16_s8;
            s8  unk17; // Used, set to current file selection?
        };
        s16 unk16;
    };
    s16 unk18; // Used
    u16 unk1A;
    f32 unk1C; // Used
    f32 unk20; // Used
    s16 unk24; // Used
    s16 unk26;
    s16 unk28; // Used
    s16 unk2A;
    f32 unk2C; // Used
    f32 unk30; // Used
    f32 unk34; // Used
    f32 unk38; // Used
    s16 unk3C;
    s16 unk3E; // Used
    s16 unk40;
    u8 unk42; // Used, related to distance from floor
    u8 unk43;
    u8 unk44;
    u8 unk45; // Used, VehicleAdditionalActorData
    u8 unk46;
    u8 unk47;
    s16 unk48; // Used
    s16 unk4A;
    s16 unk4C; // Used
    s16 unk4E;
    s16 unk50; // Used
    s16 unk52;
    f32 unk54; // Used, y velocity?
    s32 unk58; // Used, related to D_global_asm_80767CC0
    s32 unk5C;
    s32 unk60;
    s16 unk64; // Used
    s16 unk66;
    f32 unk68;
    f32 unk6C; // Used
    s16 unk70; // Used, y rotation?
    s16 unk72;
    f32 unk74;
    f32 unk78; // used
    f32 unk7C; // used
    s16 unk80; // used
    s16 unk82;
    s32 unk84;
    Actor* vehicle_actor_pointer; // Used
    u8  unk8C; // Used
    u8  unk8D; // Used
    u8 unk8E; // Used
    u8 unk8F;
    u8 unk90;
    u8 unk91; // Used
    u8 unk92;
    u8 unk93;
    f32 unk94; // Used
    f32 unk98;
    f32 unk9C; // Used
    f32 unkA0; // Used
    f32 unkA4; // Used
    f32 unkA8; // Used
    union {
        s32 unkAC; // Used
        f32 unkAC_f32; // Used
    }; // TODO: Deconflict
    union {
        struct {
            s16 unkB0;
            s16 unkB2; // Used
        };
        f32 unkB0_f32;
    }; // TODO: Deconflict
    f32 unkB4; // Used
    f32 unkB8; // Used
    s16 unkBC; // Used
    s16 unkBE;
    s16 unkC0;
    s16 unkC2; // Used
    s32 unkC4;
    s16 unkC8;
    u8  unkCA; // Used
    s8  unkCB;
    s16 unkCC; // Used
    s16 unkCE;
    s32 unkD0;
    s16 unkD4; // Used
    s16 unkD6;
    u8 unkD8; // Used
    u8 unkD9;
    u8 unkDA;
    u8 unkDB;
    Actor *unkDC; // Used
    f32 unkE0; // Used
    f32 unkE4; // Used
    u8 unkE8; // Used
    u8 unkE9;
    s16 unkEA; // Used
    s16 unkEC;
    u8 unkEE;
    u8 unkEF; // Used
    union { // TODO: No idea what's going on here, both are used
        float unkF0_f32;
        u8 unkF0;
        u8 unkF0_u8[4];
    };
    union { // TODO: No idea what's going on here, both are used
        float unkF4_f32;
        u8 unkF4;
        u8 unkF4_u8[4];
    };
    u8 unkF8; // Used
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    union { // TODO: Deconflict
        u8 unkFC; // Used
        s32 unkFC_s32; // Used
    };
    s32 unk100;
    Actor *unk104; // Used, Actor*?
    f32 unk108; // Used
    s8 unk10C; // Used
    u8 unk10D;
    s16 unk10E;
    s16 unk110;
    s16 unk112;
    s32 unk114;
    s32 unk118;
    u8 unk11C; // Used, VehicleAdditionalActorData?
    u8 unk11D;
    s16 unk11E; // Used
    s32 unk120;
    f32 unk124; // Used
    s32 unk128;
    s32 unk12C;
    Actor *unk130;
    s8 unk134;
    s8 unk135;
    s8 unk136;
    s8 unk137;
    f32 unk138;
    f32 unk13C;
    f32 unk140;
    f32 unk144;
    f32 unk148;
    f32 unk14C; // Used
    f32 unk150; // Used
    s16 unk154; // Used
    s16 unk156;
    Actor *unk158;
    s32 unk15C;
    s32 unk160;
    s32 unk164;
    s32 unk168;
    s32 unk16C;
    s32 unk170;
    s32 unk174;
    s32 unk178;
    s32 unk17C;
    s32 unk180;
    s32 unk184;
    s32 unk188;
    s32 unk18C;
    s32 unk190;
    s32 unk194;
    s32 unk198;
    s16 unk19C; // Used
    s16 unk19E; // Used
    u16 unk1A0; // Used
    u8  unk1A2;
    u8  unk1A3; // Used
    u8  unk1A4; // Used, playerIndex?
    u8  unk1A5;
    u8  unk1A6;
    u8  unk1A7;
    Actor *unk1A8;
    Actor *unk1AC; // TODO: Is this correct?
    Actor *unk1B0; // Used
    s32 unk1B4;
    u16 unk1B8; // Used
    s16 unk1BA;
    f32 unk1BC; // Used
    union {
        struct {
            f32 unk1C0; // Used, x scale
            f32 unk1C4; // Used, y scale
            f32 unk1C8; // Used, z scale
        };
        f32 scale1C0[3];
    };
    union {
        struct {
            f32 unk1CC; // Used, x scale
            f32 unk1D0; // Used, y scale
            f32 unk1D4; // Used, z scale
        };
        f32 scale1CC[3];
    };
    f32 unk1D8; // Used
    f32 unk1DC; // Used
    f32 unk1E0; // Used
    s32 unk1E4;
    s32 unk1E8;
    u8 unk1EC;
    u8 unk1ED;
    s16 unk1EE; // Used
    s32 unk1F0; // Used, bitfield // TODO: Proper bitfield syntax
    s32 unk1F4;
    s32 unk1F8;
    u8 unk1FC; // Used
    u8 unk1FD;
    u16 unk1FE; // Used
    s16 unk200; // Used
    s16 unk202;
    f32 unk204;
    f32 unk208;
    f32 unk20C;
    f32 unk210; // Used, x
    f32 unk214; // Used, y
    f32 unk218; // Used, z
    s16 unk21C;
    u16 unk21E; // Used
    f32 unk220; // Used, x
    f32 unk224; // Used, y
    f32 unk228; // Used, z
    f32 unk22C; // Used, x
    f32 unk230; // Used, y
    f32 unk234; // Used, z
    f32 unk238; // Used
    u16 unk23C; // Used
    u8 unk23E;
    u8 unk23F; // Used
    u8 unk240; // Used
    u8 unk241;
    u16 unk242; // Used, map?
    u8 unk244;
    u8 unk245; // Used
    s8 unk246; // Used func_global_asm_806CA26C
    s8 unk247; // Used func_global_asm_806CA26C
    s8 unk248;
    s8 unk249;
    u8 unk24A;
    s8 unk24B; // Used
    u16 unk24C; // Used // TODO: s16? func_global_asm_806CA1B4 uses this as u16
    u16 unk24E; // Used // TODO: s16? func_global_asm_806CA1B4 uses this as u16
    s32 unk250;
    void *unk254;
    f32 unk258; // Used
    f32 unk25C; // Used
    void *unk260; // Used (multiplayer)
    s32 unk264; // Used
    u8 unk268; // Used
} PlayerAdditionalActorData;

struct actor {
    void *unk0;
    void *animation_state;
    u32 *unk8; // Current bone array Pointer // TODO: Proper type
    s16 unkC;
    s16 unkE;
    u32 unk10;
    u32 unk14;
    u32 unk18;
    u32 unk1C;
    u32 unk20;
    f32 unk24;
    u32 unk28;
    f32 unk2C;
    u32 unk30;
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u32 unk40;
    u32 unk44;
    u32 unk48;
    u32 unk4C;
    u32 unk50;
    u32 unk54;
    s32 unk58;
    u16 interactable; // Bitfield at 0x5C
    u16 unk5E;
    union {
        struct {
            u32 OPBBit0 : 1;
            u32 OPBBit1 : 1;
            u32 OPBBit2 : 1;
            u32 OPBBit3 : 1;
            u32 OPBBit4 : 1;
            u32 OPBBit5 : 1;
            u32 OPBBit6 : 1;
            u32 OPBBit7 : 1;
            u32 OPBBit8 : 1;
            u32 OPBBit9 : 1;
            u32 OPBBit10 : 1;
            u32 OPBBit11 : 1;
            u32 OPBBit12 : 1;
            u32 OPBBit13 : 1;
            u32 OPBBit14 : 1;
            u32 OPBBit15 : 1;
            u32 OPBBit16 : 1;
            u32 OPBBit17 : 1;
            u32 OPBBit18 : 1;
            u32 OPBBit19 : 1;
            u32 OPBBit20 : 1;
            u32 OPBBit21 : 1;
            u32 OPBBit22 : 1;
            u32 OPBBit23 : 1;
            u32 OPBBit24 : 1;
            u32 OPBBit25 : 1;
            u32 OPBBit26 : 1;
            u32 OPBBit27 : 1;
            u32 OPBBit28 : 1;
            u32 OPBBit29 : 1;
            u32 OPBBit30 : 1;
            u32 OPBBit31 : 1;
        };
        u32 object_properties_bitfield; // at 0x60
    };
    s32 unk64; // Another bitfield
    u16 unk68;
    u16 unk6A; // is_grounded?
    u16 unk6C;
    s16 unk6E[1]; // TODO: How many elements?
    s16 unk70;
    u16 unk72;
    f32 unk74;
    u8 unk78;
    u8 unk79;
    u8 unk7A;
    u8 unk7B;
    union {
        struct {
            f32 x_position; // at 0x7C
            f32 y_position; // at 0x80
            f32 z_position; // at 0x84
        };
        tuple_f position;
    };
    f32 unk88;
    f32 unk8C;
    f32 unk90;
    f32 unk94;
    f32 unk98;
    f32 unk9C;
    f32 unkA0;
    f32 floor; // at 0xA4
    f32 unkA8; // Used
    f32 unkAC;
    s8 unkB0;
    s8 padB1[3];
    f32 distance_from_floor; // at 0xB4
    f32 unkB8; // at 0xB8 // velocity?
    f32 unkBC; // at 0xBC
    f32 y_velocity; // at 0xC0
    f32 y_acceleration; // at 0xC4
    f32 terminal_velocity; // at 0xC8
    u8 unkCC; // Used, bone index?
    u8 unkCD;
    s16 unkCE;
    u8 unkD0;
    u8 unkD1; // Used
    u8 unkD2;
    u8 unkD3;
    u32 unkD4;
    s16 unkD8; // Used
    s16 unkDA; // Used
    s16 unkDC; // Used
    s16 unkDE;
    f32 unkE0;
    s16 x_rotation; // at 0xE4
    s16 y_rotation; // at 0xE6
    s16 z_rotation; // at 0xE8
    s16 unkEA;
    s16 unkEC; // Something to do with shadow opacity
    s16 unkEE; // TODO: Interacts with rotation, maybe a copy of it?
    s16 unkF0;
    s16 unkF2;
    s16 unkF4;
    s16 unkF6;
    s16 unkF8;
    s16 unkFA; // Used
    u8 unkFC;
    u8 unkFD;
    u8 unkFE;
    u8 unkFF;
    f32 unk100; // Used
    f32 unk104; // Used
    f32 unk108; // Used
    s16 unk10C;
    u8 unk10E; // Used
    u8 unk10F;
    u8 locked_to_pad; // at 0x110
    u8 unk111;
    u8 unk112;
    u8 unk113;
    void *unk114;
    void *unk118;
    Actor *unk11C;
    f32 unk120;
    void *unk124;
    s16 shadow_opacity; // at 0x128, max value 0xFF
    s16 draw_distance; // at 0x12A
    s16 unk12C; // Used
    u8 unk12E;
    u8 unk12F; // Used
    u8 unk130; // Used
    u8 unk131; // Used
    union {
        s16 unk132;
        u16 unk132_u16;
    };
    s16 health;
    s16 unk136;
    u32 unk138;
    void *collision_queue_pointer; // at 0x13C
    void *ledge_info_pointer; // at 0x140
    u8 noclip_byte; // at 0x144
    u8 unk145;
    union {
        u16 unk146; // used (0x147 hand state? 0x146 seems to be u16)
        s16 unk146_s16; // used func_global_asm_8068A764
    };
    void *unk148; // Used
    void *unk14C; // Used
    void *unk150;
    u8 control_state; // at 0x154
    u8 control_state_progress; // at 0x155
    u8 unk156;
    u8 unk157;
    void *unk158; // Texture renderer linked list
    s16 unk15C;
    u8 unk15E;
    u8 unk15F; // Used, set to 0x01, 0x06, 0x0B in some cases
    f64 unk160; // TODO: Probably not correct
    s16 unk168; // Used
    u8 unk16A;
    u8 unk16B;
    u8 unk16C;
    u8 unk16D;
    u8 unk16E;
    u8 unk16F;
    u8 unk170;
    u8 unk171;
    u8 unk172;
    u8 unk173;
    void *AAD_as_array[4];
    void* unk184;
};
extern s32 D_global_asm_807FBB64;
extern int gameIsInDKTVMode(void);

// @recomp: Camera Type
RECOMP_PATCH u8 func_global_asm_80621174(s32 arg0, PlayerAdditionalActorData *arg1, Actor *arg2) {
    // Formatting is off here for better manual formatting. Whitespace doesn't affect the match here.
    // clang-format off
    return (arg2->control_state == 0x5B) ||
           ((D_global_asm_807FBB64 & 0x04000004) && (arg2->unkB8 != 0.0f)) ||
           (D_global_asm_807FBB64 & 0x08000000) ||
           (arg1->unkAC & 4) ||
           ((arg1->unkAC & 8) && (arg2->unkB8 != 0.0f)) ||
           ((recomp_get_camera_type() == 1) && (arg2->unkB8 != 0.0f) && (gameIsInDKTVMode() == 0)) ||
           (arg1->unkF0_u8[3] == 0xA) ||
           (arg1->unkF0_u8[3] == 0xD) ||
           (arg1->unkF0_u8[3] == 5) ||
           (arg1->unkF0_u8[3] == 0xC) ||
           (arg1->unkFC && (arg2->unkB8 != 0.0f)) ||
           (arg2->control_state == 0x6E) ||
           (arg2->unk58 == 8);
    // clang-format on
}

typedef struct {
    Actor *unk0;
    u8 pad4[0x8 - 0x4];
    u16 unk8;
    u8 padA[0x20 - 0xA];
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    u8 pad38[0x44 - 0X38];
    void * unk44;
    s32 unk48;
    u8 pad4C[0x90 - 0x4C];
    f32 unk90;
    u8 pad94[0xA4 - 0x94];
    f32 unkA4;
    u8 padA8[0xAC - 0xA8];
    s32 unkAC;
    s16 unkB0;
    u8 padB2[0xB8 - 0xB2];
    f32 unkB8;
    u8 padBC[0xEE - 0xBC];
    u8 unkEE;
    u8 unkEF;
    u8 unkF0;
    u8 unkF1;
    u8 unkF2;
    u8 unkF3;
    u8 unkF4;
    u8 unkF5;
    u8 unkF6;
    u8 padF7[0xFA - 0xF7];
    u8 unkFA;
    u8 unkFB;
    u8 unkFC;
} CameraPaad;

typedef struct Struct807FD610 {
    s32 unk0; // Timer that ticks up once per frame
    f32 unk4; // Probably float
    f32 unk8; // Probably float
    f32 unkC; // Probably float
    f32 unk10[4];
    s16 unk20[4];
    s16 unk28; // Used
    u16 unk2A; // Used, controller button bitfield
    u16 unk2C; // Used, controller button bitfield
    s8 unk2E; // Used
    s8 unk2F; // Used
    u8 unk30; // Used
    u8 unk31;
    s16 unk32;
} Struct807FD610;

extern Struct807FD610 D_global_asm_807FD610[];
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern u8 cc_player_index;

// @recomp: Orbit Camera
RECOMP_PATCH void func_global_asm_806EA200(void) {
    s8 change = 0x2D;
    s8 require_new_input = 1;
    s8 cooldown = 0xB;
    if (recomp_get_camera_type() == 2) {
        cooldown = 5;
        change = 5 * (cooldown - 2);
        require_new_input = 0;
    }
    CameraPaad *CaaD = extra_player_info_pointer->unk104->AAD_as_array[0];
    if (((D_global_asm_807FD610[cc_player_index].unk2C & L_CBUTTONS) || (!require_new_input)) && (CaaD->unkF1 < 3)) {
        CaaD->unkB0 -= change;
        CaaD->unkF1 = cooldown;
    }
}

RECOMP_PATCH void func_global_asm_806EA26C(void) {
    s8 change = 0x2D;
    s8 require_new_input = 1;
    s8 cooldown = 0xB;
    if (recomp_get_camera_type() == 2) {
        cooldown = 5;
        change = 5 * (cooldown - 2);
        require_new_input = 0;
    }
    CameraPaad *CaaD = extra_player_info_pointer->unk104->AAD_as_array[0];
    if (((D_global_asm_807FD610[cc_player_index].unk2C & R_CBUTTONS) || (!require_new_input)) && (CaaD->unkF1 < 3)) {
        CaaD->unkB0 += change;
        CaaD->unkF1 = cooldown;
    }
}