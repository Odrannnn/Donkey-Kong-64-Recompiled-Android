#include "common_structs.h"
#include "debug_config.h"
#include "sound.h"
#include "options.h"

extern s8 D_global_asm_80745840;
extern s8 D_global_asm_8074583C;
extern void func_global_asm_8060A398(s32 arg0);
extern void func_global_asm_80737B58(u8 arg0, u16 arg1);
RECOMP_DECLARE_EVENT(recomp_on_file_start());
RECOMP_DECLARE_EVENT(recomp_on_new_file_start());
RECOMP_DECLARE_EVENT(recomp_on_dirty_file_start());

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

void fixHelmMedalsBug(void) {
    s32 i;
    
    if (isFlagSet(0x302, FLAG_TYPE_PERMANENT)) { // BoM Shut Down
        for (i = 0; i < 5; i++) {
            setFlag(0x4B + i, TRUE, FLAG_TYPE_TEMPORARY);
        }
    }
}

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
    recomp_on_file_start();
    if (func_global_asm_8060C6B8(0xD, 0, 0, current_file)) {
        recomp_on_dirty_file_start();
        fixHelmMedalsBug();
    } else {
        recomp_on_file_start();
    }
    func_global_asm_805FF4D8(map, exit); // initMapChange()
    game_mode = GAME_MODE_ADVENTURE;
}

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