#include "common_structs.h"

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
        //     group_id = cur_drawn_model_transform_id + (u32)geo_list - (u32)modelRenderModelBin - modelRenderModelBin->geo_list_offset_4;
        // }
        // else {
            // Other models use a group ID determined by the transform ID offset.
            group_id = cur_drawn_model_transform_id + cur_model_transform_id_offset;
        // }

        // if (skip_all_interpolation || cur_drawn_model_skip_interpolation) {
            // Skip interpolation if all interpolation is currently skipped or the transform was specified to be skipped.
            // gEXMatrixGroupSkipAll((*dl_ptr)++, group_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        // }
        // else {
            // Tag the matrix with simple matrix interpolation if the model uses bones.
            // Use decomposed matrix interpolation on any other model.
            // u8 interpolation_mode = cur_model_uses_bones ? G_EX_INTERPOLATE_SIMPLE : G_EX_INTERPOLATE_DECOMPOSE;
            // u8 rotation_mode = skip_rotation ? G_EX_COMPONENT_SKIP : G_EX_COMPONENT_INTERPOLATE;
            // u8 vertex_interpolation_mode = cur_drawn_model_is_map && !cur_model_uses_ex_vertex ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            // u8 texcoord_interpolation_mode = cur_drawn_model_is_map ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            u8 interpolation_mode = G_EX_INTERPOLATE_SIMPLE;
            u8 rotation_mode = G_EX_COMPONENT_INTERPOLATE;
            u8 vertex_interpolation_mode = G_EX_COMPONENT_SKIP;
            u8 texcoord_interpolation_mode = G_EX_COMPONENT_SKIP;
            gEXMatrixGroup(dl++, group_id, interpolation_mode, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_COMPONENT_INTERPOLATE, rotation_mode,
                G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, vertex_interpolation_mode, G_EX_COMPONENT_INTERPOLATE,
                G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, texcoord_interpolation_mode, G_EX_COMPONENT_AUTO);
        // }
        *pushed = TRUE;
        return dl;
    }
    // else if (skip_all_interpolation) {
    //     gEXMatrixGroupNoInterpolate((*dl_ptr)++, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
    //     return TRUE;
    // }
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