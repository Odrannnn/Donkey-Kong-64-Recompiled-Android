#include "common_structs.h"
#include "ui.h"

extern u32 D_global_asm_80744478;
extern s32 D_global_asm_80747B24;
extern u16 *D_global_asm_807F5D80;
extern s8 D_global_asm_807F5D84;
extern s8 D_global_asm_807F5D85;
extern s16 D_global_asm_807F5D86;
extern s16 D_global_asm_807F5D88;
extern f32 D_global_asm_807F5D8C;
extern f32 D_global_asm_807F5D90;
extern f32 D_global_asm_807F5D94;
extern Mtx D_global_asm_807F5D98;
extern u32 global_properties_bitfield;
extern u8 D_global_asm_80747B20; 
extern void func_global_asm_8061134C(void*);
extern Gfx *func_global_asm_805FD030(Gfx *);
extern void func_global_asm_8062A3F0(void);
extern void func_global_asm_807023E8(Gfx **, u16 *, s32, s32, s32, s32, s32, f32, f32, f32, f32, f32, f32);
extern void func_global_asm_807024E0(Gfx **, void*, s32, s32, s32, s32, s32, f32, f32, f32, f32, f32, f32, s32, s32, s32, void *);
void func_global_asm_8062A130(s32 arg0, s32 arg1, void *arg2);
void func_global_asm_8062A228(s32 arg0, s32 arg1, void *arg2);
extern void func_global_asm_8062A24C(s32 arg0, s32 arg1, void *arg2);
extern s16 D_global_asm_80744494;
extern s16 D_global_asm_80744498;
extern s16 D_global_asm_8074449C;
extern s16 D_global_asm_807444A0;
extern s16 D_global_asm_807444A4;
extern s16 D_global_asm_807444A8;
extern s16 D_global_asm_807444AC;
extern s16 D_global_asm_807444B0;
extern s16 D_global_asm_807444B4;
extern Gfx **D_1000118;
extern Mtx D_2000140;

extern Gfx *alignHUD(Gfx *, enumSpriteAlignment);
extern Gfx *popHUD(Gfx *);

Gfx* gfx_draw_textured_rect_stretched(Gfx* dl, s32 destX, s32 destY, s32 destW, s32 destH, u8* texture, s32 srcW, s32 srcH, s32 minX, s32 maxX) {
    gDPPipeSync(dl++);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetTextureFilter(dl++, G_TF_BILERP);
    gDPSetCombineMode(dl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(dl++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE2);

    f32 scaleX = (f32)destW / (f32)srcW;
    f32 scaleY = (f32)destH / (f32)srcH;

    u16 dsdx = (u16)((1.0f / scaleX) * (1 << 10));
    u16 dtdy = (u16)((1.0f / scaleY) * (1 << 10));

    for (s32 tile_y = 0; tile_y < srcH; tile_y += 32) {
        s32 cur_h = tile_y + 32 > srcH ? srcH - tile_y : 32;

        for (s32 tile_x = 0; tile_x < srcW; tile_x += 64) {
            s32 cur_w = tile_x + 64 > srcW ? srcW - tile_x : 64;

            s32 dx0 = destX + (s32)(tile_x * scaleX);
            s32 dx1 = destX + (s32)((tile_x + cur_w) * scaleX);
            s32 dy0 = destY + (s32)(tile_y * scaleY);
            s32 dy1 = destY + (s32)((tile_y + cur_h) * scaleY);
            if (dx1 <= minX || dx0 >= maxX) {
                continue;
            }
            s32 clippedLeft = 0;
            if (dx0 < minX) {
                clippedLeft = minX - dx0;
                dx0 = minX;
            }
            if (dx1 > maxX) {
                dx1 = maxX;
            }
            if (dx0 >= dx1) {
                continue; // fully clipped away after rounding
            }

            u8* tile_texture = texture + ((tile_y * srcW) + tile_x) * 2;

            gDPLoadTextureTile(dl++,
                tile_texture, G_IM_FMT_RGBA, G_IM_SIZ_16b,
                srcW, srcH,
                0, 0, cur_w - 1, cur_h - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            u16 sStart = (u16)(((s64)clippedLeft * dsdx) / 32);

            gSPTextureRectangle(dl++,
                dx0 << 2, dy0 << 2,
                (dx1 << 2) - 1, (dy1 << 2) - 1,
                G_TX_RENDERTILE,
                sStart, 0, dsdx, dtdy);
        }
    }

    gDPPipeSync(dl++);
    return dl;
}

//This is the function that renders all framebuffer transition effects
RECOMP_PATCH Gfx *func_global_asm_80629300(Gfx *dl) {
    s32 sp54;
    s32 width;
    s32 height;
    s32 progress;

    sp54 = D_global_asm_80744478 * 0.5f;
    if (D_global_asm_80747B24 == 0) {
        guScale(&D_global_asm_807F5D98, 2.0f, 2.0f, 1.0f);
        D_global_asm_80747B24 = 1;
    }
    if ((D_global_asm_807F5D84 == 0) && (D_global_asm_80747B20 != 0)) {
        D_global_asm_80747B20--;
    }
    if (D_global_asm_807F5D84 < 0) {
        D_global_asm_807F5D84++;
        if (D_global_asm_807F5D84 == 0) {
            func_global_asm_8061134C(D_global_asm_807F5D80);
        }
    } else {
        if (D_global_asm_807F5D84 > 0) {
            gSPDisplayList(dl++, &D_1000118);
            dl = func_global_asm_805FD030(dl);
            gSPMatrix(dl++, &D_2000140, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            gSPMatrix(dl++, &D_global_asm_807F5D98, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPPipeSync(dl++);
            gDPSetTextureFilter(dl++, G_TF_POINT);
            gDPSetColorDither(dl++, G_CD_DISABLE);
            gDPSetScissor(dl++, G_SC_NON_INTERLACE, D_global_asm_80744498, D_global_asm_8074449C, D_global_asm_807444A0, D_global_asm_807444A4);
            gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
            recomp_get_ui_bounds(&width, &height);
            recomp_printf("Rendering framebuffer %d: Width %d, Height %d\n", D_global_asm_807F5D85, width, height);
            gEXPushScissor(dl++);
            gEXPushViewport(dl++);
            gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, D_global_asm_80744494);
            gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, 0, 0, 0, 0);
            gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, 0, 0);
            
            switch (D_global_asm_807F5D85) {
                case 7:
                    func_global_asm_8062A3F0();
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    dl = gfx_draw_textured_rect_stretched(dl, 0, 0, width, height, (u8*)D_global_asm_807F5D80, 320, 240, 0, width);
                    if (global_properties_bitfield & 0x40) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 1:
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, D_global_asm_807F5D86);
                    func_global_asm_807023E8(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 319.0f, 239.0f, 0.0f, 0.0f);
                    D_global_asm_807F5D86 -= sp54 * 5;
                    if (D_global_asm_807F5D86 < 0) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 2:
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    dl = gfx_draw_textured_rect_stretched(dl, 0, 0, width, height, (u8*)D_global_asm_807F5D80, 320, 240, D_global_asm_807F5D86, width);
                    // func_global_asm_807023E8(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, D_global_asm_807F5D86, 0.0f, 319.0f, 239.0f, D_global_asm_807F5D86, 0.0f);
                    gDPPipeSync(dl++);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    // if (D_global_asm_807F5D86 >= 0x11) {
                    //     func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    // }
                    D_global_asm_807F5D86 += (sp54 * 0xA);
                    if (D_global_asm_807F5D86 >= width) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 0:
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0xA, 0x50, 0.0f, 0.0f, D_global_asm_807F5D86, 239.0f, 0.0f, 0.0f);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 < 0x131) {
                        func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x10, 0x20, D_global_asm_807F5D86, 0.0f, D_global_asm_807F5D86 + 0x10, 239.0f, D_global_asm_807F5D86, 0.0f, 1, 0x10, 2, NULL);
                    }
                    D_global_asm_807F5D86 -= (sp54 * 0xA);
                    if (D_global_asm_807F5D86 < 0xB) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 3:
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    progress = (320 - D_global_asm_807F5D86) * ((f32)width / 320.0f);
                    dl = gfx_draw_textured_rect_stretched(dl, 0, 0, width, height, (u8*)D_global_asm_807F5D80, 320, 240, width - progress, width);
                    dl = gfx_draw_textured_rect_stretched(dl, 0, 0, width, height, (u8*)D_global_asm_807F5D80, 320, 240, 0, progress);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    // if (D_global_asm_807F5D86 >= 0x11) {
                    //     func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    // }
                    // if (D_global_asm_807F5D88 < 0x131) {
                    //     func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x10, 0x20, D_global_asm_807F5D88, 0.0f, D_global_asm_807F5D88 + 0x10, 239.0f, D_global_asm_807F5D88, 0.0f, 1, 0x10, 2, NULL);
                    // }
                    D_global_asm_807F5D86 += (sp54 * 0xA);
                    D_global_asm_807F5D88 -= (sp54 * 0xA);
                    if (D_global_asm_807F5D86 >= 0x137) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 4:
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D8C = D_global_asm_807F5D8C + (5.0 * sp54);
                    D_global_asm_807F5D90 = D_global_asm_807F5D8C + 40.0f;
                    func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 320.0f, 239.0f, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A24C);
                    if (D_global_asm_807F5D8C > 170.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 5:
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D8C = D_global_asm_807F5D8C + (12.0 * sp54);
                    D_global_asm_807F5D90 = D_global_asm_807F5D8C + 40.0f;
                    func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 320.0f, 239.0f, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A228);
                    if (D_global_asm_807F5D8C > 350.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 6:
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D94 = D_global_asm_807F5D94 + (15.0 * sp54);
                    func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 320.0f, 239.0f, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A130);
                    if (D_global_asm_807F5D94 > 350.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
            }
            dl = popHUD(dl);
            gDPPipeSync(dl++);
            gDPSetColorDither(dl++, G_CD_MAGICSQ);
            gDPSetTextureFilter(dl++, G_TF_BILERP);
        }
    }
    return dl;
}
