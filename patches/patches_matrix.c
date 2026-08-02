#include "common_structs.h"

extern u8 skip_interpolation;

Mtx identity_fixed_mtx = {{
    {
        0x00010000, 0x00000000,
        0x00000001, 0x00000000, }, {
        0x00000000, 0x00010000,
        0x00000000, 0x00000001,
    },
    {
        0x00000000, 0x00000000,
        0x00000000, 0x00000000, }, {
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
    }
}};
s32 cur_drawn_model_transform_id = 0;
s32 cur_model_transform_id_offset = 0;
u8 cur_drawn_model_is_map = FALSE;
u8 cur_model_uses_ex_vertex = FALSE;

typedef struct Struct80614C38_0 Struct80614C38;
struct Struct80614C38_0 {
    void *unk0;
    void *unk4;
    u8 unk8;
    u8 pad9[0xC - 0x9];
    Struct80614C38 *unkC;
    u8 pad10[0x14 - 0x10];
    Struct80614C38 *next;
};

typedef struct ActorModelHeader {
    s32 unk0;
    union {
        s32 *unk4;
        s32 unk4_raw;
    };
    s32 unk8;
    s32 unkC;
    s32 unk10;
    Struct80614C38 * unk14;
    u8 pad18[0x20 - 0x18];
    u8 bone_count;
    u8 unk21;
    u8 pad22[0x28 - 0x22];
} ActorModelHeader;

Gfx *func_global_asm_80614C38(Gfx *, Actor *, ActorModelHeader *);
Gfx *func_global_asm_80687EE0(Gfx *dl, Actor *arg1);
extern Actor *actor_list[];
extern s16 actor_count;

Gfx *set_model_matrix_group(Gfx * dl, void *geo_list, u8 skip_rotation, u8 *pushed) {
    if (cur_drawn_model_transform_id != 0) {
        u32 group_id;
        // Pick a group ID based on whether this is a map or not.
        // if (cur_drawn_model_is_map) {
        //     // Map models use a group ID determined by the offset of the geo command to guarantee they're unique and consistent between frames.
        //     // group_id = cur_drawn_model_transform_id + (u32)geo_list - (u32)modelRenderModelBin - modelRenderModelBin->geo_list_offset_4;
        // }
        // else {
            // Other models use a group ID determined by the transform ID offset.
            group_id = cur_drawn_model_transform_id + cur_model_transform_id_offset;
        // }
        if (skip_interpolation) {
            //  || cur_drawn_model_skip_interpolation
            // Skip interpolation if all interpolation is currently skipped or the transform was specified to be skipped.
            gEXMatrixGroupSkipAll(dl++, group_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        }
        else {
            // Tag the matrix with simple matrix interpolation if the model uses bones.
            // Use decomposed matrix interpolation on any other model.
            // u8 interpolation_mode = cur_model_uses_bones ? G_EX_INTERPOLATE_SIMPLE : G_EX_INTERPOLATE_DECOMPOSE;
            // u8 rotation_mode = skip_rotation ? G_EX_COMPONENT_SKIP : G_EX_COMPONENT_INTERPOLATE;
            u8 vertex_interpolation_mode = cur_drawn_model_is_map && !cur_model_uses_ex_vertex ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            u8 texcoord_interpolation_mode = cur_drawn_model_is_map ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            u8 interpolation_mode = G_EX_INTERPOLATE_SIMPLE;
            u8 rotation_mode = G_EX_COMPONENT_INTERPOLATE;
            // u8 vertex_interpolation_mode = G_EX_COMPONENT_SKIP;
            // u8 texcoord_interpolation_mode = G_EX_COMPONENT_SKIP;
            gEXMatrixGroup(dl++, group_id, interpolation_mode, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_COMPONENT_INTERPOLATE, rotation_mode,
                G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, vertex_interpolation_mode, G_EX_COMPONENT_INTERPOLATE,
                G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, texcoord_interpolation_mode, G_EX_COMPONENT_AUTO);
        }
        *pushed = TRUE;
        return dl;
    }
    else if (skip_interpolation) {
        gEXMatrixGroupNoInterpolate(dl++, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        *pushed = TRUE;
        return dl;
    }
    else {
        *pushed = FALSE;
        return dl;
    }
}

Gfx *pop_model_matrix_group(Gfx *dl) {
    gEXPopMatrixGroup(dl++, G_MTX_MODELVIEW);
    return dl;
}

// @recomp: Actor matrix stuff
RECOMP_PATCH Gfx *func_global_asm_80614B34(Gfx *dl, Actor *arg1) {
    ActorModelHeader *var_s0;
    s32 var_v1;
    u8 pushed_matrix_group = FALSE;
    s32 i;

    cur_drawn_model_transform_id = 0x1000 + arg1->unk54;
    var_s0 = (ActorModelHeader *)arg1->unk0;
    if (arg1->unk4C != NULL) {
        var_s0 = (ActorModelHeader *)arg1->unk4C;
    }
    if (arg1->unk50 != NULL) {
        var_s0 = (ActorModelHeader *)arg1->unk50;
    }
    dl = func_global_asm_80687EE0(dl, arg1);
    dl = func_global_asm_80614C38(dl, arg1, var_s0);
    gSPSegment(dl++, 0x04, osVirtualToPhysical(arg1->unk8));
    gSPSegment(dl++, 0x03, osVirtualToPhysical((ActorModelHeader *)var_s0->unk0));
    for (var_v1 = 0; var_v1 < var_s0->unk21; var_v1++) {
        cur_model_transform_id_offset = var_v1;
        gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
        gSPDisplayList(dl++, var_s0->unk4[var_v1]);
        gSPPopMatrix(dl++, G_MTX_MODELVIEW);
        if (pushed_matrix_group) {
            dl = pop_model_matrix_group(dl);
        }
    }
    return dl;
}


void func_global_asm_80636D38(Prop*, s32, s32);
void func_global_asm_80639968(void*);
void func_global_asm_8065EB10(u8, s16, f32, f32, f32, s32, f32, s32);
f32 func_global_asm_8065D0FC(f32 arg0);
s32 func_global_asm_80659470(s32 arg0);
s32 func_global_asm_806D0964(s32 arg0, u8 playerIndex);
void func_global_asm_8065C334(f32 arg0, f32 arg1, f32 arg2, s16 arg3, u8 *arg4, u8 *arg5, u8 *arg6, s16 arg7);
void func_global_asm_8063E6B4(Prop_ScriptData *arg0);
void func_global_asm_8065CE4C(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 *arg5);
Gfx *func_global_asm_8065D008(Gfx *dl, s16 arg1, u8 arg2);
extern u8 D_global_asm_80750AB4;
extern u8 cc_player_index;
extern u8  D_global_asm_807444FC;
extern Prop *D_global_asm_807F6000;

typedef struct Struct80636FFC {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    u8 pad1C[0x20 - 0x1C];
    Mtx unk20[2];
    Gfx *unkA0[2];
    Gfx *unkA8[2];
    void *unkB0;
    s16 unkB4;
    s16 unkB6;
    s32 unkB8;
    u8 padBC[0xC0 - 0xBC];
    s16 unkC0;
    u8 unkC2;
    u8 unkC3;
    u8 unkC4;
    u8 unkC5;
} Struct80636FFC;

// @recomp: Model 2 Matrix stuff
RECOMP_PATCH Gfx* func_global_asm_80636FFC(Struct80636FFC* arg0, Gfx* dl, s32 arg2, s32 arg3, s32 arg4, u8 arg5, s16 arg6) {
    f32 spF8[4][4];
    f32 spB8[4][4];
    s32* temp_a2;
    Prop_ScriptData* temp_a0_4;
    s32 pad;
    u8 var_v0;
    void ** var_a2;
    f32 var_f6;
    f32 var_f8;
    f32 temp_f0_6;
    u8 sp97;
    u8 sp96;
    u8 sp95;
    s16 sp92;
    s16 temp_v0_3;
    f32 var_f0;
    f32 sp88;
    f32 var_f2;
    f32 dX;
    f32 dY;
    f32 dZ;
    f32 var_f4;
    u8 pushed_matrix_group = FALSE;

    if (arg5 != 0) {
        dX = SQ(character_change_array[cc_player_index].unk21C - arg0->unk0);
        dY = character_change_array[cc_player_index].unk220 - arg0->unk4;
        dZ = character_change_array[cc_player_index].unk224 - arg0->unk8;
        sp88 = _sqrtf(dX + SQ(dY) + SQ(dZ));
        var_f2 = func_global_asm_8065D0FC(arg0->unkC0);
        if (arg0->unkC5 & 1) {
            var_f2 = (f32) arg0->unkC0;
        }
        if (var_f2 < sp88) {
            return dl;
        }
    }
    if (arg0->unkC3 != 0) {
        func_global_asm_8065EB10(arg0->unkC3, arg0->unkB6, arg0->unk0, arg0->unk4, arg0->unk8, arg0->unkC0, sp88 / var_f2, (s32) arg6);
    }
    if ((arg0->unkB4 == 0) || (arg0->unkB4 == 0x241)) {
        return dl;
    }
    if ((arg0->unkB4 == 0x74) || (arg0->unkB4 == 0x288) || (arg0->unkB4 == 0x290)) {
        arg0->unk14 = (f32) ((f64) arg0->unk14 + 10.0);
    }
    guScaleF(spF8, arg0->unkC, arg0->unkC, arg0->unkC);
    if (arg0->unk10 != 0.0) {
        guRotateF(spB8, arg0->unk10, 1.0f, 0.0f, 0.0f);
        guMtxCatF(spF8, spB8, spF8);
    }
    if (arg0->unk14 != 0.0) {
        guRotateF(spB8, arg0->unk14, 0.0f, 1.0f, 0.0f);
        guMtxCatF(spF8, spB8, spF8);
    }
    if (arg0->unk18 != 0.0) {
        guRotateF(spB8, arg0->unk18, 0.0f, 0.0f, 1.0f);
        guMtxCatF(spF8, spB8, spF8);
    }
    guTranslateF(spB8, arg0->unk0, arg0->unk4, arg0->unk8);
    guMtxCatF(spF8, spB8, spF8);
    guMtxF2L(spF8, &arg0->unk20[D_global_asm_807444FC]);
    gDPPipeSync(dl++);
    gSPSegment(dl++, 0x08, osVirtualToPhysical(arg0->unkB0));
    var_a2 = NULL;
    if (arg0->unkB6 != -1) {
        temp_v0_3 = func_global_asm_80659470(arg0->unkB6);
        func_global_asm_80636D38(&D_global_asm_807F6000[temp_v0_3], 0, 1);
        var_a2 = (void **)D_global_asm_807F6000[temp_v0_3].unk78;
        if (func_global_asm_806D0964(D_global_asm_807F6000[temp_v0_3].unk24->unk0, cc_player_index) != 0) {
            return dl;
        }
    }
    if (var_a2 != NULL) {
        gSPSegment(dl++, 0x09, osVirtualToPhysical(var_a2[D_global_asm_807444FC]));
    }
    gSPMatrix(dl++, osVirtualToPhysical(&arg0->unk20[D_global_asm_807444FC]), G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    if (arg0->unkB8 != 0) {
        var_v0 = D_global_asm_807444FC;
        func_global_asm_80639968(arg0);
    } else {
        var_v0 = 0;
    }
    gSPSegment(dl++, 0x0A, osVirtualToPhysical(arg0->unkA0[var_v0]));
    gSPClearGeometryMode(dl++, G_CULL_BOTH | G_FOG);
    if ((arg5) && (arg0->unkC2 == 0)) {
        if (D_global_asm_807F6000[temp_v0_3].unk7C) {
            var_f0 = D_global_asm_807F6000[temp_v0_3].unk7C->unk98;
        } else {
            var_f0 = 0;
        }
        func_global_asm_8065C334(arg0->unk0, arg0->unk4 + var_f0, arg0->unk8, 0, &sp97, &sp96, &sp95, (s16) (s32) arg6);
        temp_f0_6 = arg0->unkC4 / 15.0f;
        sp97 *= temp_f0_6;
        sp96 *= temp_f0_6;
        sp95 *= temp_f0_6;
    } else {
        sp95 = 0xFF;
        sp96 = 0xFF;
        sp97 = 0xFF;
    }
    gDPSetPrimColor(dl++, 0, 0xFF, sp97, sp96, sp95, 0xFF);
    if (arg5) {
        if ((arg0->unkB6 != -1) && (temp_a0_4 = D_global_asm_807F6000[temp_v0_3].unk7C, (temp_a0_4 != NULL)) && (temp_a0_4->unk60 == 1)) {
            func_global_asm_8063E6B4(temp_a0_4);
            sp92 = D_global_asm_807F6000[temp_v0_3].unk7C->unk64;
        } else {
            func_global_asm_8065CE4C(arg0->unk0, arg0->unk4, arg0->unk8, (f32) arg0->unkC0, (s16) (s32) arg0->unkB4, &sp92);
            if (arg0->unkB6 != -1) {
                if ((D_global_asm_807F6000[temp_v0_3].unk7C != NULL) && (D_global_asm_80750AB4 == 1)) {
                    D_global_asm_807F6000[temp_v0_3].unk7C->unk62 = sp92;
                }
            }
            if (arg0->unkB6 != -1) {
                if (D_global_asm_807F6000[temp_v0_3].unk7C != NULL) {
                    func_global_asm_8063E6B4(D_global_asm_807F6000[temp_v0_3].unk7C);
                }
            }
            if (arg0->unkB6 != -1) {
                if ((D_global_asm_807F6000[temp_v0_3].unk7C != NULL) && (D_global_asm_80750AB4 == 1)) {
                    sp92 = D_global_asm_807F6000[temp_v0_3].unk7C->unk64;
                }
            }
        }
    } else {
        sp92 = 0xFF;
    }
    if (sp92 != 0) {
        if (arg0->unkB6 != -1) {
            if (D_global_asm_807F6000[temp_v0_3].unk8C != 0) {
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 8) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_DK)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 2) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_DIDDY)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 4) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_TINY)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 1) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_CHUNKY)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 0x10) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_LANKY)) {
                    sp92 = 0x64;
                }
            }
        }
        dl = func_global_asm_8065D008(dl, sp92, 0U);
        gDPPipeSync(dl++);
        // @recomp: mtx tag
        cur_drawn_model_transform_id = 0x8000 + D_global_asm_807F6000[temp_v0_3].unk8A;
        gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
        // 
        gSPDisplayList(dl++, osVirtualToPhysical(arg0->unkA0[var_v0]));
        gDPPipeSync(dl++);
        gSPDisplayList(dl++, osVirtualToPhysical(arg0->unkA8[var_v0]));
        // @recomp: Mtx untag
        gSPPopMatrix(dl++, G_MTX_MODELVIEW);
        if (pushed_matrix_group) {
            dl = pop_model_matrix_group(dl);
        }
        //
    }
    gDPPipeSync(dl++);
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    return dl;
}
