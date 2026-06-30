#include "patches.h"
#include "PR/os_message.h"
#include "maps.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "misc_funcs.h"

u8* getTextString(u8 fileIndex, s32 stringIndex, s32 arg2);
u8 func_global_asm_806FD894(s16 arg0);
extern Gfx** D_1000118;
extern Mtx D_20000C0;
extern s16 D_global_asm_80744494;
extern s16 D_global_asm_80744490;
void* func_global_asm_8068C12C(u16 textureIndex);
Gfx* printStyledText(Gfx* dl, s16 style, s16 x, s16 y, u8* string, u32 extraBitfield);
s32 getCenterOfString(s16 renderStyle, u8* string);
Gfx* printText(Gfx* dl, s16 x, s16 y, f32 scale, u8* string);
extern u8 D_global_asm_8074450C;

extern u8 D_menu_800339D0_02175720;

#define TEXT_SCALE 0.25f
#define ORIGINAL_TEXT_SCALE 0.5f

RECOMP_PATCH Gfx* func_menu_8003292C(Gfx* dl) {
    u8* spCC;
    u8* spC8;
    u8* spC4;
    s32 spC0;
    s32 sp48;
    s32 sp50;
    f32 temp_f6;
    s16 temp_s0_19;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_t0;
    s32 temp_t9;
    s32 temp_t6;
    s32 var_a2;
    s32 x_offset, y_offset;
    s32 i, j;
    void* temp_v0;

    spCC = getTextString(0xD, 0, 1);
    spC8 = getTextString(0xD, 1, 1);
    spC4 = getTextString(0xD, 2, 1);
    spC0 = func_global_asm_806FD894(1);

    gEXEnable(dl++);

    gDPPipeSync(dl++);
    gSPDisplayList(dl++, &D_1000118);
    gDPSetRenderMode(dl++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    var_a2 = D_menu_800339D0_02175720 * 0xF;
    if (var_a2 >= 0x100) {
        var_a2 = 0xFF;
    }

    gDPSetPrimColor(dl++, 0, 0, var_a2, var_a2, var_a2, 0xFF);
    gDPSetTextureFilter(dl++, G_TF_POINT);
    gDPSetTexturePersp(dl++, G_TP_NONE);

    temp_v0 = func_global_asm_8068C12C(0xB0U);

    s32 half_width = 204 / 2;   // 102
    s32 half_height = 82 / 2;   // 41
    s32 center_x = (s32)((f32)(D_global_asm_80744490 - half_width) * 0.5f);
    s32 center_y = (s32)((f32)(D_global_asm_80744494 - half_height) * 0.5f);

    gEXSetRectAspect(dl++, G_EX_ASPECT_ADJUST);

    y_offset = center_y;
    for (i = 0; i < 0x52; i += 0x29) {
        temp_a1 = i + 0x29;
        temp_a2 = y_offset + (0x29 / 2);
        x_offset = center_x;
        for (j = 0; j < 0xCC; j += 0x66) {
            gDPLoadTextureTile_4b(dl++, temp_v0, G_IM_FMT_I, 204, 82,
                j, i, j + 0x66, temp_a1, 0,
                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(dl++,
                x_offset << 2, y_offset << 2,
                (x_offset + (0x66 / 2)) << 2, temp_a2 << 2,
                G_TX_RENDERTILE,
                j << 5, i << 5,
                0x0800, 0x0800);
            x_offset += (0x66 / 2);
        }
        y_offset += (0x29 / 2);
    }


    gDPSetTextureFilter(dl++, G_TF_BILERP);
    gDPSetTexturePersp(dl++, G_TP_PERSP);

    temp_f6 = (f32)(D_global_asm_80744490 - getCenterOfString(1, spCC)) * 0.5f;

    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_a2);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0,
        0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);

    gEXSetRectAspect(dl++, G_EX_ASPECT_AUTO);

    dl = printText(dl,
        (s16)((s32)temp_f6 * 4) + 330,
        (s16)((s32)(((f32)(D_global_asm_80744494 - 82.0f) * 0.5f) - (f32)spC0) * 4) + 105,
        TEXT_SCALE * 2, spCC);

    temp_a2_2 = D_global_asm_8074450C * 0xC8;
    temp_s0_19 = (s16)((s32)((f32)D_global_asm_80744490 * TEXT_SCALE) * 4);

    dl = printText(dl, temp_s0_19 + 330, temp_a2_2 * 4, 0.25f, spC8);
    dl = printText(dl, temp_s0_19 + 330, ((s32)(temp_a2_2 + (spC0 * 0.5)) * 4) - 15, 0.25f, spC4);

    return dl;
}