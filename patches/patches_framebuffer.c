#include "patches.h"
#include "PR/os_message.h"
#include "enums.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "misc_funcs.h"
#include "debug_config.h"

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

/*
This is the function that renders all framebuffer transition effects
RECOMP_PATCH Gfx *func_global_asm_80629300(Gfx *dl) {
    s32 sp54;

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
            switch (D_global_asm_807F5D85) {
                case 7:
                    func_global_asm_8062A3F0();
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 319.0f, 239.0f, 0.0f, 0.0f);
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
                    func_global_asm_807023E8(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, D_global_asm_807F5D86, 0.0f, 319.0f, 239.0f, D_global_asm_807F5D86, 0.0f);
                    gDPPipeSync(dl++);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 >= 0x11) {
                        func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    }
                    D_global_asm_807F5D86 += (sp54 * 0xA);
                    if (D_global_asm_807F5D86 >= 0x137) {
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
                    func_global_asm_807023E8(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x20, 0x20, D_global_asm_807F5D86, 0.0f, 319.0f, 239.0f, D_global_asm_807F5D86, 0.0f);
                    func_global_asm_807023E8(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0xA, 0x50, 0.0f, 0.0f, D_global_asm_807F5D88, 239.0f, 0.0f, 0.0f);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 >= 0x11) {
                        func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    }
                    if (D_global_asm_807F5D88 < 0x131) {
                        func_global_asm_807024E0(&dl, D_global_asm_807F5D80, 0, 0x140, 0xF0, 0x10, 0x20, D_global_asm_807F5D88, 0.0f, D_global_asm_807F5D88 + 0x10, 239.0f, D_global_asm_807F5D88, 0.0f, 1, 0x10, 2, NULL);
                    }
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
            gDPPipeSync(dl++);
            gDPSetColorDither(dl++, G_CD_MAGICSQ);
            gDPSetTextureFilter(dl++, G_TF_BILERP);
        }
    }
    return dl;
}
*/
