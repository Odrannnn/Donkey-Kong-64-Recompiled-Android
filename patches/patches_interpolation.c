#include "common_structs.h"
#include "enums.h"
#include "debug_config.h"
#include "misc_funcs.h"
#include "patches_main.h"
#include "patches_interpolation.h"

#define INTERPOLATION_DEBUG 0

s32 interpolation_disable_timer = 0;
u8 skip_interpolation = FALSE;
#if INTERPOLATION_DEBUG
    s32 debug_counter = 0;
#endif

void set_interpolation_lockdown(s32 value) {
    // Sets a lockdown timer for when interpolation can occur
    if (interpolation_disable_timer > value) {
        // If something else has put a lockdown on interpolation that lasts longer, then don't overwrite it
        return;
    }
    #if INTERPOLATION_DEBUG
        recomp_printf("\n[%d] Disabling interpolation for %d frames\n", debug_counter, value);
        debug_counter++;
    #endif
    interpolation_disable_timer = value;
}

Gfx *handle_interpolation(Gfx * dl) {
    skip_interpolation = FALSE;
    if (interpolation_disable_timer > 0) {
        skip_interpolation = TRUE;
        #if INTERPOLATION_DEBUG
            recomp_printf("[%d] Interpolation disabled. %d frames left\n", debug_counter, interpolation_disable_timer);
            debug_counter++;
        #endif
        interpolation_disable_timer--;
    }
    if ((is_cutscene_active == 3) || (is_cutscene_active == 4)) {
        // Skip interpolation for Arcade/Jetpac
        skip_interpolation = TRUE;
    }
    if (MTXTAG_GLOBAL != 0) {
        if (skip_interpolation) {
            gEXMatrixGroupSkipAllAspect(dl++, MTXTAG_GLOBAL, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO);
        }
        else {
            gEXMatrixGroup(dl++, MTXTAG_GLOBAL, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
        }
    }
    else if (skip_interpolation) {
        gEXMatrixGroupNoInterpolate(dl++, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
    else {
        gEXMatrixGroupSimpleNormal(dl++, G_EX_ID_AUTO, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
    return dl;
}

RECOMP_PATCH void func_global_asm_8061DBD4(Actor* arg0, f32* arg1, f32* arg2, f32* arg3, f32* arg4, f32* arg5, f32* arg6, f32* arg7, f32* arg8, f32* arg9) {
    CameraPaad* AAD;
    OSTime temp_v0_9;
    Actor* temp_v0; // 94
    f32 temp_f0;
    f64 var_f0;
    f32 temp_f14_5;
    FuncBank_value *params;
    f32 sp84, sp80;
    s32 var_s1;
    u16 temp_t0;
    f32 temp_f20;

    AAD = arg0->AAD_as_array[0];
    temp_v0 = AAD->unk0;
    D_global_asm_807F5CD0 = (s16) (s32) (temp_v0->unkAC - temp_v0->floor);
    AAD->unk9E = func_global_asm_806CC190(AAD->unk9E, temp_v0->y_rotation, 4.0f);
    if ((temp_v0->object_properties_bitfield & 0x200) && !(AAD->unkAC & 0x100)) {
        getBonePosition(temp_v0, 8, &AAD->unkD8.x, &AAD->unkD8.y, &AAD->unkD8.z);
    } else {
        AAD->unkD8.x = temp_v0->position.f[0];
        AAD->unkD8.y = temp_v0->position.f[1];
        AAD->unkD8.z = temp_v0->position.f[2];
    }
    temp_f0 = _sqrtf(
        SQ(temp_v0->position.f[0] - AAD->unkCC.x) +
        SQ(temp_v0->position.f[1] - AAD->unkCC.y) +
        SQ(temp_v0->position.f[2] - AAD->unkCC.z) 
    );
    AAD->unk10 = (temp_f0 * 40.0f) * (D_global_asm_80744478 * 0.5);
    if (AAD->unkF1 != 0) {
        AAD->unkF1--;
    }
    if (AAD->unkF5 != 0) {
        AAD->unkF5--;
    }
    if (AAD->unkF6 != 0) {
        AAD->unkF6--;
    }
    if (AAD->unkF7 != 0) {
        AAD->unkF7--;
    }
    func_global_asm_8061D060(AAD);
    func_global_asm_8061D1FC(arg0);
    arg0->z_rotation = func_global_asm_806CC190(arg0->z_rotation, temp_v0->z_rotation, 20.0f);
    func_global_asm_8061C0FC(AAD);
    switch (is_cutscene_active) {
        case 0:
            if ((AAD->unkAC & 0x100) == 0) {
                if (loading_zone_transition_speed > 0.0f) {
                    if (!gameIsInDKTVMode()) {
                        return;
                    }
                }
                if (AAD->unkF3 == 9) {
                    return;
                } else if ((temp_v0->control_state == 0x54) && (temp_v0->control_state_progress < 3)) {
                    return;
                } else if ((temp_v0->control_state == 0x52) && (temp_v0->control_state_progress < 8))  {
                    return;
                } else if ((temp_v0->control_state == 0x53) && (temp_v0->control_state_progress < 3))  {
                    return;
                } else {
                    if (temp_v0->control_state == 0x42) {
                        if ((extra_player_info_pointer->unkBC != 0x11) && (extra_player_info_pointer->unkBC != 0x60) && (extra_player_info_pointer->unkBC != 0x49)) {
                            return;
                        }
                    }
                    if (temp_v0->control_state == 0x43) {
                        return;
                    }
                }
            }
            func_global_asm_8061D6A8(AAD);
            if (AAD->unkAC & 0x80000000) {
                func_global_asm_8061C39C(arg0);
            } else {
                func_global_asm_80622B24(arg0, arg0->position.f, &arg0->position.f[1], &arg0->position.f[2], &AAD->unk78, &AAD->unk7C, &AAD->unk80, temp_v0);
                if ((global_properties_bitfield & 0x2000) || (AAD->unkAC & 0x100000)) {
                    if (AAD->unkAC & 0x100000) {
                        temp_f0 = AAD->unk38;
                        AAD->unkAC &= ~0x00100000;
                        arg0->position.f[0] = temp_f0;
                        *arg1 = temp_f0;
                        AAD->unk84 = temp_f0;
                        temp_f0 = AAD->unk3C;
                        arg0->position.f[1] = temp_f0;
                        *arg2 = temp_f0;
                        AAD->unk88 = temp_f0;
                        temp_f0 = AAD->unk40;
                        arg0->position.f[2] = temp_f0;
                        *arg3 = temp_f0;
                        AAD->unk8C = temp_f0;
                        func_global_asm_80602498();
                    } else {
                        *arg1 = arg0->position.f[0];
                        AAD->unk84 = arg0->position.f[0];
                        temp_f14_5 = arg0->position.f[1];
                        *arg2 = temp_f14_5;
                        AAD->unk88 = temp_f14_5;
                        temp_f14_5 = arg0->position.f[2];
                        *arg3 = temp_f14_5;
                        AAD->unk8C = temp_f14_5;
                    }
                    *arg4 = AAD->unk78;
                    *arg5 = AAD->unk7C;
                    *arg6 = AAD->unk80;
                    global_properties_bitfield ^= 0x2000;
                } else {
                    func_global_asm_80625320(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                }
            }
            break;
        case 1:
            temp_v0_9 = osGetTime();
            D_global_asm_807476C8 = temp_v0_9 - D_global_asm_807476D0;
            D_global_asm_807476D0 = temp_v0_9;
            if (func_global_asm_8061B4B0()) {
                func_global_asm_8061B4E4();
            }
            func_global_asm_8061B7E0(arg0, AAD, *arg1, *arg3);
            if (D_global_asm_807F5CF4 & 0x40) {
                func_global_asm_8061B660(AAD, arg1, arg2, arg3, 0.2f, 0.1f, 50.0f, 40.0f);
            } else {
                if (D_global_asm_807476E4 != 0) {
                    D_global_asm_807476E4--;
                    arg0->x_rotation = D_global_asm_807476E0 + ((f32) (D_global_asm_807476DC - D_global_asm_807476E0) * ((f32) D_global_asm_807476E4 / (f32) D_global_asm_807476E8));
                }
                D_global_asm_807476F0++;
                do {
                    var_s1 = 0;
                    if (D_global_asm_807F5CEC == 0) {
                        func_global_asm_8061D898();
                        D_global_asm_807F5CF0++;
                        if (D_global_asm_807476FC->camera_bank[D_global_asm_807476F4].point_count < D_global_asm_807F5CF0) {
                            if (D_global_asm_807F5CF4 & 0x10) {
                                if ((gPlayerPointer->unk6A & 0x100) == 0) {
                                    gPlayerPointer->unk6A |= 0x100;
                                }
                            } else {
                                func_global_asm_8061D4E4(arg0);
                            }
                            //@recomp: On cutscene end, disable interpolation for 2f. 1f seems to not be enough
                            set_interpolation_lockdown(2);
                        } else {
                            temp_t0 = D_global_asm_807476FC->camera_bank[D_global_asm_807476F4].length_array[D_global_asm_807F5CF0 - 1];
                            params = &D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].params[0];
                            switch (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command) {
                                case 10:
                                case 15:
                                case 16:
                                case 17:
                                    global_properties_bitfield |= 0x2000;
                                    break;
                                case 12:
                                    playSong(params[0].valu16_0, 1.0f);
                                    if (D_global_asm_807F5CEC == 0) {
                                        func_global_asm_8061D898();
                                        D_global_asm_807F5CEC = 0;
                                        var_s1 = TRUE;
                                    }
                                    break;
                                case 11:
                                    if (D_global_asm_807F5CF4 & 4) {
                                        D_global_asm_807476FC = &D_global_asm_807F5B10[0];
                                    }
                                    func_global_asm_8061D4E4(arg0);
                                    AAD->unkAC &= ~0x40000;
                                    if ((AAD->unkF3 != 0xB) && (AAD->unkF3 != 3)) {
                                        temp_f14_5 = gPlayerPointer->position.f[1];
                                        AAD->unk70 = temp_f14_5;
                                        AAD->unk6C = temp_f14_5;
                                        AAD->unkA0 = _sqrtf(SQ(*arg3 - gPlayerPointer->position.f[2]) + SQ(*arg1 - gPlayerPointer->position.f[0]));
                                        AAD->unkA4 = AAD->unkA0;
                                        AAD->unkB2 = func_global_asm_80665DE0(gPlayerPointer->position.f[0], gPlayerPointer->position.f[2], *arg1, *arg3);
                                        temp_f14_5 = *arg2 - gPlayerPointer->position.f[1];
                                        AAD->unkB8 = temp_f14_5;
                                        arg0->distance_from_floor = temp_f14_5;
                                        AAD->unk84 = *arg1;
                                        AAD->unk88 = *arg2;
                                        AAD->unk8C = *arg3;
                                        *arg4 = ((*arg4 - *arg1) * 0.1) + *arg1;
                                        *arg5 = ((*arg5 - *arg2) * 0.1) + *arg2;
                                        *arg6 = ((*arg6 - *arg3) * 0.1) + *arg3;
                                        AAD->unk78 = *arg4;
                                        AAD->unk7C = *arg5;
                                        AAD->unk80 = *arg6;
                                        if (D_global_asm_807FBB64 & 1) {
                                            func_global_asm_8062217C(arg0, 2);
                                        } else {
                                            func_global_asm_80622334(arg0, (s16) (s32) AAD->unkA0, arg2);
                                        }
                                        AAD->unk94 = AAD->unkA0 / 3.0;
                                        func_global_asm_8061F164(AAD, 0x1E);
                                        AAD->unkF3 = 1U;
                                        global_properties_bitfield &= ~0x2000;
                                        func_global_asm_8061D6A8(AAD);
                                    }
                                    break;
                                case 14:
                                    func_global_asm_8060098C(func_global_asm_8061DA14,
                                        temp_t0 + 0x80000000, 0, 0, 0);
                                    func_global_asm_8061D898();
                                    D_global_asm_807F5CEC = 0;
                                    var_s1 = TRUE;
                                    break;
                                case 13:
                                    if (temp_t0) {
                                        func_global_asm_8060098C(func_global_asm_80627F04,
                                            temp_t0 + 0x80000000,
                                            params[0].vals32,
                                            params[1].vals32,
                                            params[2].vals32);
                                    } else {
                                        func_global_asm_80627F04(
                                            params[0].vals32,
                                            params[1].vals32,
                                            params[2].vals32,
                                            D_global_asm_807F5CF2);
                                    }
                                    func_global_asm_8061D898();
                                    D_global_asm_807F5CEC = 0;
                                    var_s1 = TRUE;
                                    break;
                                case 6:
                                    D_global_asm_807F5CEE = temp_t0;
                                    func_global_asm_8061D898();
                                    D_global_asm_807F5CEC = 0;
                                    var_s1 = TRUE;
                                    break;
                                case 4:
                                case 5:
                                    //@recomp: On cutscene segment init, disable interpolation for 2f. 1f seems to not be enough
                                    set_interpolation_lockdown(2);
                                    break;                            
                            }
                        }
                    }
                } while (var_s1);
                if (is_cutscene_active) {
                    D_global_asm_807F5CFC = D_global_asm_807476FC->camera_bank[D_global_asm_807476F4].length_array[D_global_asm_807F5CF0 - 1];
                    if (D_global_asm_807F5CFC != 1.0f) {
                        D_global_asm_807F5D00 = (D_global_asm_807F5CEC - 1) / (D_global_asm_807F5CFC - 1.0f);
                    } else {
                        D_global_asm_807F5D00 = 0.0f;
                    }
                    if (D_global_asm_807F5CEC > 0) {
                        D_global_asm_807F5CEC--;
                    }
                    switch (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command) {
                        case 8:
                        case 12:
                            break;
                        case 4:
                        case 5:
                            D_global_asm_807F5D0C->position.f[0] = D_global_asm_807F5CE8->position.f[0];
                            D_global_asm_807F5D0C->position.f[1] = D_global_asm_807F5CE8->position.f[1];
                            D_global_asm_807F5D0C->position.f[2] = D_global_asm_807F5CE8->position.f[2];
                            D_global_asm_807F5D0C->unk15E = MIN(0x14, D_global_asm_807F5CE8->unk15E);
                            D_global_asm_807F5D0C->y_rotation = D_global_asm_807F5CE8->y_rotation;
                            func_global_asm_80622B24(arg0, arg1, arg2, arg3, &AAD->unk78, &AAD->unk7C, &AAD->unk80, D_global_asm_807F5D0C);
                            *arg4 = AAD->unk78;
                            *arg5 = AAD->unk7C;
                            *arg6 = AAD->unk80;
                            break;
                        default:
                            D_global_asm_807476A4 = 0.3f;
                            func_global_asm_80622B24(arg0, arg1, arg2, arg3, &AAD->unk78, &AAD->unk7C, &AAD->unk80, D_global_asm_807F5CE8);
    
                            switch (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command) {
                                case 7:
                                case 0xF:
                                case 0x11:
                                    break;
                                default:
                                    var_f0 = D_global_asm_807F5CE8->distance_from_floor != 0.0f ? 0.3 : 0.1;
                                    func_global_asm_80625994(arg0, var_f0, arg4, arg5, arg6);
                                    break;
                            }
                            if ((global_properties_bitfield & 0x2000) == 0) {
                                if ((D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command != 7) && (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command != 0xF) && (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command != 0x11)) {
                                    break;
                                }
                            }
                            global_properties_bitfield &= ~0x2000;
                            *arg4 = AAD->unk78;
                            *arg5 = AAD->unk7C;
                            *arg6 = AAD->unk80;
                    }
                    if (AAD->unkF2) {
                        func_global_asm_8061EDA0(AAD, arg1, arg2, arg3, AAD->unkC0, 0);
                    }
                }
            }
            break;
    }
    AAD->unkAC &= 0x7FFDAF0F;
    if (D_global_asm_807FBB64 & 0x8000) {
        AAD->unkAC |= 4;
    } else if ((AAD->unkAC & 0x10000) == 0) {
        AAD->unkAC &= ~4;
    }
    if (AAD->unkF2 != 0) {
        func_global_asm_8061EDA0(AAD, arg4, arg5, arg6, AAD->unkC0 * 0.333, 1);
    }
    if (!(AAD->unkAC & 0x100)) {
        func_global_asm_80627490(&sp84, &sp80, *arg4, *arg5, *arg6, *arg1, *arg2, *arg3);
        temp_f20 = func_global_asm_80612D10(sp80);
        *arg8 = func_global_asm_80612790(arg0->x_rotation);
        *arg7 = func_global_asm_80612794(arg0->x_rotation) * func_global_asm_80612D10(sp84) * temp_f20;
        *arg9 = func_global_asm_80612794(arg0->x_rotation) * func_global_asm_80612D1C(sp84) * temp_f20;
        arg0->unkEC = arg0->x_rotation;
    }
    AAD->unkCC.x = temp_v0->position.f[0];
    AAD->unkCC.y = temp_v0->position.f[1];
    AAD->unkCC.z = temp_v0->position.f[2];
}

void func_global_asm_8061B840(CameraPaad*, u8);
void func_global_asm_80620628(Actor *, f32, s16, u8);
extern f32 D_global_asm_807476A8;

// @recomp: Camera Flip handler
RECOMP_PATCH s32 func_global_asm_80620F00(Actor* arg0, u8 arg1, u8 arg2) {
    f32 temp_f2;
    CameraPaad* temp_s0;
    PlayerAdditionalActorData* temp_v0;
    u8 var_a1;
    u8 var_a2;
    u8 var_v1;
    u8 var_t7;

    temp_s0 = arg0->AAD_as_array[0];
    temp_f2 = D_global_asm_807476A8 - character_change_array[temp_s0->unkFB].near;
    temp_v0 = temp_s0->unk0->AAD_as_array[0];
    var_v1 = D_global_asm_807476A8 > 40.0f && temp_v0->unk114 < temp_f2;
    var_a1 = temp_v0->unk118 < temp_f2;
    var_a2 = temp_v0->unk11A < temp_f2;
    var_t7 = temp_v0->unk116 < temp_f2;
    if (arg2 != 0 && !var_a1 && !var_a2) {
        var_v1 = FALSE;
    }
    if (var_v1) {
        if (arg1 && (temp_s0->unk0->unkB8 != 0.0f)) {
            if (var_a1 && !var_a2) {
                temp_s0->unkB2 += 0x32;
                func_global_asm_8061B840(temp_s0, 0xA);
            } else if (var_a2 && !var_a1) {
                temp_s0->unkB2 -= 0x32;
                func_global_asm_8061B840(temp_s0, 0xA);
            }
        }
        // @recomp: Disable camera flips
        // temp_s0->unkF4++;
        if (var_t7 && (temp_s0->unkF4 >= 0x15)) {
            temp_s0->unkF4 = 0U;
            if ((temp_s0->unk0->unkB8 != 0.0f) && (temp_s0->unk0->control_state != 0x59)) {
                func_global_asm_80620628(arg0, 0.0f, temp_s0->unkB2, 1);
                return TRUE;
            }
            func_global_asm_80620628(arg0, 0.0f, temp_s0->unk0->y_rotation, 1);
        }
        return TRUE;
    }
    temp_s0->unkFF = 0;
    temp_s0->unkF4 = 0U;
    return FALSE;
}