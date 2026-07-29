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
extern s16 D_global_asm_80744490;
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

typedef struct {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u32 unk14;
    u32 unk18;
    u32 unk1C;
} Struct8070A848;

u16 stored_framebuffer[320 * 240];

extern Actor *gCurrentActorPointer;
extern void addActorToTextOverlayRenderArray(void *arg0, Actor *arg1, u8 arg2);
s8 storedFBEffect = -1;


Gfx *rdpStoreFB(Gfx *dl, Actor *a) {
    if (D_global_asm_807F5D84 == 2) {
        storedFBEffect = D_global_asm_807F5D85;
        D_global_asm_807F5D84 = 0;
    }
    recomp_printf("Snap!\n");
    gDPSetColorImage(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, OS_K0_TO_PHYSICAL(stored_framebuffer));
    return dl;
}

extern void *D_global_asm_80744470[];
extern u8  D_global_asm_807444FC;
Gfx *rdpClearFB(Gfx *dl, Actor *a) {
    if (storedFBEffect > -1) {
        D_global_asm_807F5D85 = storedFBEffect; // Delays it by 1f
        D_global_asm_807F5D84 = 1;
        storedFBEffect = -1;
    }
    recomp_printf("Reset!\n");
    gDPSetColorImage(dl++, 0, 2, D_global_asm_80744490, osVirtualToPhysical(D_global_asm_80744470[D_global_asm_807444FC]));
    return dl;
}

RECOMP_PATCH void func_global_asm_8070A848(Struct8070A848 *arg0, Struct8070A848 *arg1) {
    s32 i;
    Struct8070A848 *src = arg1;
    Struct8070A848 *dst = arg0;

    for (i = 0; i < ((D_global_asm_80744490 * D_global_asm_80744494) / 16); i++) {
        dst->unk0 = src->unk0 | 0x10001;
        dst->unk4 = src->unk4 | 0x10001;
        dst->unk8 = src->unk8 | 0x10001;
        dst->unkC = src->unkC | 0x10001;
        dst->unk10 = src->unk10 | 0x10001;
        dst->unk14 = src->unk14 | 0x10001;
        dst->unk18 = src->unk18 | 0x10001;
        dst->unk1C = src->unk1C | 0x10001;
        dst++;
        src++;
    }
    addActorToTextOverlayRenderArray(rdpStoreFB, gCurrentActorPointer, 0);
    addActorToTextOverlayRenderArray(rdpClearFB, gCurrentActorPointer, 3);
}

extern s32 D_global_asm_80747B30;
extern s32 D_global_asm_80747B34;
// @recomp: Framebuffer effects renderer
RECOMP_PATCH Gfx *func_global_asm_80629300(Gfx *dl) {
    f32 sp54;
    s32 width, height;
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
            gEXMatrixGroup(dl++, 2, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_STRETCH, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
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
            // gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, 0, 0, 0, 0);
            gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, 0, 0);
            switch (D_global_asm_807F5D85) {
                case 7: // Pausing (Blurred Background)
                    // @recomp: We can't morph the framebuffer image as this breaks RT64 widescreen support
                    // func_global_asm_8062A3F0();
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    // Blurs it a little
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 128);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 319.0f, 239.0f, 0.0f, 0.0f);
                    // Add a feathering call to add a little distortion
                    func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x50, 0, 0.0f, 320, 239.0f, 0, 0.0f, 1, 0x10, 1, NULL);
                    if (global_properties_bitfield & 0x40) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 1: // Fade Transition
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, D_global_asm_807F5D86);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 319.0f, 239.0f, 0.0f, 0.0f);
                    D_global_asm_807F5D86 -= sp54 * 5;
                    if (D_global_asm_807F5D86 < 0) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 2: // L -> R Swipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, D_global_asm_807F5D86, 0.0f, 319.0f, 239.0f, D_global_asm_807F5D86, 0.0f);
                    gDPPipeSync(dl++);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 >= 0x11) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    }
                    D_global_asm_807F5D86 += (sp54 * 0xA);
                    if (D_global_asm_807F5D86 >= 0x137) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 0: // R -> L Swipe (Lanky/Tiny crypt)
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0xA, 0x50, 0.0f, 0.0f, D_global_asm_807F5D86, 239.0f, 0.0f, 0.0f);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 < 0x131) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x20, D_global_asm_807F5D86, 0.0f, D_global_asm_807F5D86 + 0x10, 239.0f, D_global_asm_807F5D86, 0.0f, 1, 0x10, 2, NULL);
                    }
                    D_global_asm_807F5D86 -= (sp54 * 0xA);
                    if (D_global_asm_807F5D86 < 0xB) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 3: // Dual Swipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, D_global_asm_807F5D86, 0.0f, 319.0f, 239.0f, D_global_asm_807F5D86, 0.0f);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0xA, 0x50, 0.0f, 0.0f, D_global_asm_807F5D88, 239.0f, 0.0f, 0.0f);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 >= 0x11) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    }
                    if (D_global_asm_807F5D88 < 0x131) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x20, D_global_asm_807F5D88, 0.0f, D_global_asm_807F5D88 + 0x10, 239.0f, D_global_asm_807F5D88, 0.0f, 1, 0x10, 2, NULL);
                    }
                    D_global_asm_807F5D86 += (sp54 * 0xA);
                    D_global_asm_807F5D88 -= (sp54 * 0xA);
                    if (D_global_asm_807F5D86 >= 0x137) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 4: // Iris Wipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D8C = D_global_asm_807F5D8C + (5.0 * sp54);
                    D_global_asm_807F5D90 = D_global_asm_807F5D8C + 40.0f;
                    func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, width, height, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A24C);
                    if (D_global_asm_807F5D8C > 170.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 5: // TL->BR Wipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D8C = D_global_asm_807F5D8C + (12.0 * sp54);
                    D_global_asm_807F5D90 = D_global_asm_807F5D8C + 40.0f;
                    func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, width, height, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A228);
                    if (D_global_asm_807F5D8C > 350.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 6: // Clock Wipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D94 = D_global_asm_807F5D94 + (15.0 * sp54);
                    func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, width, height, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A130);
                    if (D_global_asm_807F5D94 > 350.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
            }
            gEXMatrixGroup(dl++, 2, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
            dl = popHUD(dl);
            gDPPipeSync(dl++);
            gDPSetColorDither(dl++, G_CD_MAGICSQ);
            gDPSetTextureFilter(dl++, G_TF_BILERP);
        }
    }
    return dl;
}

extern Actor *gLastSpawnedActor;
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern s32 spawnActor(Actors actorIndex, s32 modelIndex);
extern void moveAndScaleActorToAnother(Actor *destination, Actor *source, f32 scale); 
extern void func_global_asm_806291B4(u8 arg0);
extern void func_global_asm_8065EACC(void);
extern void func_global_asm_80672C30(Actor *arg0);
extern void func_global_asm_806C8220(s32, void *, s32);
extern s32 func_global_asm_8061EB04(Actor *playerPointer, u8 playerIndex);
extern void func_global_asm_80602498(void);
extern s32 handleInputsForControlState(s32 arg0);
extern void playAnimation(Actor *arg0, s32 arg2);
extern void setYAccelerationFrom80753578(void);
extern void applyActorYAcceleration(void);
extern void func_global_asm_80617770(Actor *arg0, u32 arg1, u8 arg2);
extern void func_global_asm_806C8D20(Actor *arg0);
extern void func_global_asm_8065EAF4(void);
extern void func_global_asm_806CC970(void);
extern void renderActor(Actor *arg0, u8 arg1);

// @recomp: Function to warp player upon taking warp pad
RECOMP_PATCH void func_global_asm_806DB670(void) {
    gCurrentActorPointer->unkB8 = 0;
    switch (gCurrentActorPointer->control_state_progress) {
        case 0:
            gCurrentActorPointer->noclip_byte = 1;
            if (spawnActor(ACTOR_BANAPORTER, 0x98)) {
                moveAndScaleActorToAnother(gLastSpawnedActor, gCurrentActorPointer, 0.15f);
            }
            gCurrentActorPointer->control_state_progress++;
            break;
        case 1:
            gCurrentActorPointer->control_state_progress++;
            extra_player_info_pointer->unk23C = 4;
            break;
        case 2:
            gCurrentActorPointer->x_position = ((extra_player_info_pointer->unk210 - gCurrentActorPointer->x_position) * 0.5) + gCurrentActorPointer->x_position;
            gCurrentActorPointer->y_position = ((extra_player_info_pointer->unk214 - gCurrentActorPointer->y_position) * 0.5) + gCurrentActorPointer->y_position;
            gCurrentActorPointer->z_position = ((extra_player_info_pointer->unk218 - gCurrentActorPointer->z_position) * 0.5) + gCurrentActorPointer->z_position;
            if (extra_player_info_pointer->unk23C != 0) {
                extra_player_info_pointer->unk23C--;
                if (extra_player_info_pointer->unk23C == 0) {
                    gCurrentActorPointer->object_properties_bitfield &= 0xFFFF7FFF;
                    gCurrentActorPointer->shadow_opacity = 0;
                }
            }
            break;
        case 3:
            gCurrentActorPointer->object_properties_bitfield &= 0xFFFF7FFF;
            gCurrentActorPointer->shadow_opacity = 0;
            gCurrentActorPointer->control_state_progress++;
            break;
        case 5:
            extra_player_info_pointer->unk23C = 0x14;
            break;
        case 6:
            extra_player_info_pointer->unk23C -= 1;
            if (extra_player_info_pointer->unk23C == 0) {
                func_global_asm_806291B4(2); // @recomp: Push transition effect 1f earlier to capture snapshot properly
                D_global_asm_807F5D84 = 2; // @recomp: Delay notice
                func_global_asm_8065EACC();
                gCurrentActorPointer->control_state_progress++;
            }
            break;
        case 7:
            global_properties_bitfield &= 0xFFFCFFDF;
            func_global_asm_80672C30(gCurrentActorPointer);
            gCurrentActorPointer->x_position = extra_player_info_pointer->unk204;
            gCurrentActorPointer->y_position = extra_player_info_pointer->unk208;
            gCurrentActorPointer->unk8C = gCurrentActorPointer->y_position;
            gCurrentActorPointer->z_position = extra_player_info_pointer->unk20C;
            func_global_asm_806C8220(0, gCurrentActorPointer->unk178, gCurrentActorPointer->unk58);
            func_global_asm_8061EB04(gCurrentActorPointer, extra_player_info_pointer->unk1A4);
            extra_player_info_pointer->unk23C = 0x14;
            
            gCurrentActorPointer->control_state_progress++;
            break;
        case 8:
            global_properties_bitfield |= 0x30020;
            extra_player_info_pointer->unk23C -= 1;
            if (extra_player_info_pointer->unk23C == 0) {
                func_global_asm_80602498();
                if (spawnActor(ACTOR_BANAPORTER, 0x98)) {
                    moveAndScaleActorToAnother(gLastSpawnedActor, gCurrentActorPointer, 0.15f);
                }
                gCurrentActorPointer->control_state_progress++;
                extra_player_info_pointer->unk23C = 0xC;
            }
            break;
        case 10:
            if (extra_player_info_pointer->unk23C != 0) {
                extra_player_info_pointer->unk23C--;
                if (extra_player_info_pointer->unk23C == 0) {
                    gCurrentActorPointer->x_position = extra_player_info_pointer->unk210;
                    gCurrentActorPointer->y_position = extra_player_info_pointer->unk214;
                    gCurrentActorPointer->z_position = extra_player_info_pointer->unk218;
                    gCurrentActorPointer->object_properties_bitfield |= 0x8000;
                }
            }
            break;
        case 11:
            handleInputsForControlState(0x20);
            playAnimation(gCurrentActorPointer, 0x1B);
            setYAccelerationFrom80753578();
            gCurrentActorPointer->control_state_progress++;
            break;
        case 12:
        case 13:
            handleInputsForControlState(0x20);
            applyActorYAcceleration();
            break;
        case 14:
            func_global_asm_80617770(gCurrentActorPointer, 0x2F, 0);
            gCurrentActorPointer->control_state_progress++;
            // fallthrough
        case 15:
            func_global_asm_806C8D20(gCurrentActorPointer);
            func_global_asm_8065EAF4();
            handleInputsForControlState(0x30);
            break;
    }
    func_global_asm_806CC970();
    renderActor(gCurrentActorPointer, 0);
}

typedef struct Struct80767CE8 {
    Mtx unk0;
    u8 pad40[0xDB0 - 0x40];
    Gfx unkDB0;
    u8 padDB8[0x11B0 - 0xDB8];
} Struct80767CE8;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
} Struct807FD9A8_unkC;

typedef struct {
    s32 unk0;
    s32 unk4;
    void *unk8;
    Struct807FD9A8_unkC *unkC;
    void *unk10;
    void *unk14;
    s32 *unk18;
    s32 *unk1C;
    s32 *unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
} Struct807FD9A8;

typedef struct {
    s32 offset;
    s32 width;
    s32 height;
} TexEntry;

extern u8 D_global_asm_807444F4;
extern u8 D_global_asm_807444F8;
extern Struct80767CE8 D_global_asm_80767CE8[2];
extern Struct80767CE8 *D_global_asm_8076A048;
extern s32 D_global_asm_8076A088;
extern s32 D_global_asm_8076A08C;
extern u8 D_global_asm_8076A0A4;
extern u8 D_global_asm_8076A0B1;
extern u8 D_global_asm_8076A0B2;
extern f32 D_global_asm_807FD888;
extern Struct807FD9A8 * D_global_asm_807FD9A8;
extern void* D_global_asm_807FD9B0;
extern TexEntry* D_global_asm_807FD9B4;
extern u8 D_global_asm_807FD9BC;
extern u8 D_global_asm_807FD9BD;
extern Gfx *D_global_asm_8076A050[];
extern u32 object_timer;
extern u8 is_cutscene_active;
extern s32 D_global_asm_807FBB64;
extern u8 D_global_asm_807501E0;
extern Vtx *D_global_asm_807FD9B8;
extern s32 D_global_asm_80755068;
extern s32 D_global_asm_8075506C;
extern void *D_global_asm_807FD9A4;
extern s32 func_global_asm_8070B7EC(Gfx**, void*, void*);
extern void func_global_asm_80610044(void *arg0, s32 arg1, u8 arg2, u8 arg3, s32 arg4, u8 arg5);
extern void func_global_asm_8070AF24(void);
extern void func_global_asm_8070AC74(Mtx *arg0, Gfx **dlp);
extern void func_global_asm_805FF378(Maps nextMap, s32 nextExit);
extern void func_global_asm_8066B434(void *arg0, s32 arg1, s32 arg2);
extern void func_global_asm_8061CBCC(void);
extern void func_global_asm_805FE71C(Gfx *dl, u8 arg1, s32 *arg2, u8 arg3);
extern void func_global_asm_805FE7B4(Gfx *dl, Gfx *arg1, s32 *arg2, u8 arg3);
extern void *getPointerTableFile(enum pointertable_e pointerTableIndex, s32 fileIndex, u8 arg2, u8 arg3);
extern void func_global_asm_80709890(Vtx *, Struct807FD9A8 **, void **, s32);
extern s32 func_global_asm_80709ACC(Struct807FD9A8 *);
extern void func_global_asm_807095E4(s32, s32);

extern void *D_global_asm_807FD9AC;
extern void *_malloc(s32);

// @recomp: Zipper Snapshot
RECOMP_PATCH void func_global_asm_8070AF24(void) {
    switch (D_global_asm_807444F8) {
        case 1:
            D_global_asm_80755068 = 0;
            D_global_asm_8075506C = 0;
            break;
        case 2:
            D_global_asm_80755068 = 0x78;
            break;
    }
    D_global_asm_807FD9B8 = getPointerTableFile(TABLE_19_UNKNOWN, 1, 1, 1);
    // D_global_asm_807FD9B0 = _malloc(D_global_asm_80744490 * D_global_asm_80744494 * 2);
    // func_global_asm_8070A848(D_global_asm_807FD9B0, D_global_asm_80744470[D_global_asm_807444FC]);
    func_global_asm_80709890(D_global_asm_807FD9B8, &D_global_asm_807FD9A8, &D_global_asm_807FD9AC, 0);
    func_global_asm_80709ACC(D_global_asm_807FD9A8);
    D_global_asm_807FD9A4 = D_global_asm_807FD9A8->unk8;
    D_global_asm_807FD9A8->unk8 = _malloc(D_global_asm_807FD9A8->unk0 * 0xC);
    func_global_asm_807095E4(D_global_asm_807FD9A8->unk24, D_global_asm_807FD9A8->unk28);
}

// @recomp: Zipper Display
RECOMP_PATCH void func_global_asm_8070A934(enum map_e arg0, s32 arg1) {
    Gfx* sp34;
    Gfx* sp30;
    u8 temp_t1;
    u8 temp_t5;
    s32 i;

    func_global_asm_80610044(D_global_asm_8076A050[D_global_asm_807444FC], D_global_asm_8076A088, 3U, 1U, 0x4D2, 1U);
    D_global_asm_807444FC ^= 1;
    object_timer += 1;
    D_global_asm_8076A048 = &D_global_asm_80767CE8[D_global_asm_807444FC];
    sp30 = &D_global_asm_8076A048->unkDB0;
    switch (D_global_asm_807FD9BC) {
    case 1:
        break;
    case 0:
        D_global_asm_807FD9BC = 1;
        recomp_printf("Taking Zipper Snapshot\n");
        func_global_asm_8070AF24();
        break;
    }
    if ((D_global_asm_8076A0B1 & 1) && (D_global_asm_807FD888 == 31.0f)) {
        sp34 = D_global_asm_8076A050[D_global_asm_807444FC];
        if (D_global_asm_8076A0B2 == 1) {
            is_cutscene_active = D_global_asm_807444F4;
        }
    } else {
        func_global_asm_8070AC74(&D_global_asm_8076A048->unk0, &sp34);
        D_global_asm_807501E0 = 0; // @recomp: Release all overlays to prevent repeated snapshots
        gEXMatrixGroup(sp34++, 2, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_STRETCH, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
        if (func_global_asm_8070B7EC(&sp34, D_global_asm_807FD9B8, stored_framebuffer) != 0) {
            switch (D_global_asm_807444F8) {
            case 1:
                osViBlack(1U);
                func_global_asm_805FF378(arg0, arg1);
                D_global_asm_807FD888 = 31.0f;
                D_global_asm_807444F8 = 2;
                break;
            case 2:
                // func_global_asm_8061134C(D_global_asm_807FD9B0);
                func_global_asm_8061134C(D_global_asm_807FD9A8->unk8);
                func_global_asm_8061134C(D_global_asm_807FD9B4);
                func_global_asm_8061134C(D_global_asm_807FD9A8->unk10);
                func_global_asm_8061134C(D_global_asm_807FD9A8->unk14);
                func_global_asm_8066B434(D_global_asm_807FD9B8, 0x24B, 0x4A);
                is_cutscene_active = D_global_asm_807444F4;
                if ((is_cutscene_active == 1) && (D_global_asm_807FBB64 & 1)) {
                    func_global_asm_8061CBCC();
                }
                D_global_asm_807444F8 = 3;
                D_global_asm_807FD888 = 0.0f;
                break;
            }
        }
        gEXMatrixGroup(sp34++, 2, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
    }
    if (D_global_asm_807FD9BD != 0) {
        D_global_asm_807FD9BD--;
        if (!D_global_asm_807FD9BD) {
            D_global_asm_8076A0B1 |= 2;
        }
    }
    if ((D_global_asm_8076A0A4 != 0) && (is_cutscene_active != 6)) {
        func_global_asm_805FE71C(sp34, D_global_asm_807444FC, &D_global_asm_8076A088, 0U);
        func_global_asm_805FE7B4(sp30, (Gfx* ) D_global_asm_8076A048, &D_global_asm_8076A08C, 1U);
        return;
    }
    func_global_asm_805FE71C(sp34, D_global_asm_807444FC, &D_global_asm_8076A088, 1U);
}

void func_global_asm_80682E38(void *arg0);
extern s32 D_global_asm_8074E848[];

typedef struct TagAAD {
    Actor *unk0;
    u16 unk4;
    s8 unk6;
    u8 unk7;
    Actor *unk8[5];
    Actor *unk1C;
    s8 unk20;
    u8 pad21;
    s16 unk22;
    f32 unk24;
    u8 pad28[4];
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    s16 unk38;
    u8 pad3A[2];
    s32 unk3C;
} TagAAD;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} Struct807FBB70_unk278;
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    s16 unk10;
    s16 unk12;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
    u8 pad1D[0x24 - 0x1D];
    u8 unk24;
    u8 unk25;
    u8 pad26[0x2C - 0x26];
    u8 unk2C;
    u8 pad2D[0x38 - 0x2D];
    u8 unk38;
    u8 unk39;
    u16 unk3A;
    f32 unk3C;
    f32 unk40;
    f32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    Actor *unk7C[4];
    s32 unk8C;
    u8 unk90;
    u8 unk91;
    s16 unk92;
    s16 unk94;
    u8 unk96;
    u8 unk97;
    s32 unk98;
    s32 unk9C;
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    s32 unkC4;
    s32 unkC8;
    s32 unkCC;
    s32 unkD0;
    s32 unkD4;
    f32 unkD8;
    f32 unkDC;
    f32 unkE0;
    u8 unkE4;
    u8 unkE5;
    u8 unkE6;
    u8 unkE7;
    f32 unkE8;
    s32 unkEC;
    f32 unkF0;
    f32 unkF4;
    u8 padF8[0x1F8 - 0xF8];
    s32 unk1F8;
    Actor *unk1FC;
    u8 unk200;
    u8 unk201;
    s16 unk202;
    Actor *unk204[1];
    u8 pad208[0x243 - 0x208];
    u8 unk243[1];
    u8 unk244[1];
    s8 unk245;
    s8 unk246;
    s8 unk247;
    u8 pad248[0x254 - 0x248];
    u8 unk254;
    u8 unk255;
    u8 unk256;
    u8 unk257;
    s32 unk258[1];
    s32 unk25C;
    s32 unk260;
    s32 unk264;
    s32 unk268;
    s32 unk26C;
    s32 unk270;
    s32 unk274;
    Struct807FBB70_unk278 *unk278[1];
} GlobalASMStruct35;
typedef struct {
    s16 unk0;
    s16 unk2;
    u8 unk4;
    u8 unk5;
} GlobalASMStruct45;

extern Maps current_map;
extern GlobalASMStruct45 D_global_asm_8074E814[];
extern GlobalASMStruct35 D_global_asm_807FBB70;
extern u8 func_global_asm_8061CB50(void);
extern void func_global_asm_806C9304(Actor *arg0, PlayerAdditionalActorData *arg1);
extern void func_global_asm_806C93E4(Actor *arg0, PlayerAdditionalActorData *arg1);
extern void func_global_asm_80659620(f32 *arg0, f32 *arg1, f32 *arg2, s16 arg3);
extern void func_global_asm_80659670(f32 arg0, f32 arg1, f32 arg2, s16 arg3);
extern void playSong(MUSIC_E arg0, f32 arg1);
extern void func_global_asm_80602CE0(s32 arg0, s32 arg1, u8 arg2);
extern s32 func_global_asm_805FF018(s32 actorBehaviour, s32 kongIndex);
extern void setFlag(s16 flagIndex, u8 newValue, u8 flagType);
extern void func_global_asm_80627878(Actor *arg0);
extern void func_global_asm_8060DEC8(void);
extern s32 playCutscene(Actor *arg0, s16 arg1, u8 arg2);
extern void func_global_asm_8061B650(Actor *arg0);
extern void func_global_asm_806FB218(void);

// @recomp: Enter Tag Barrel
RECOMP_PATCH void func_global_asm_806833DC(TagAAD *arg0) {
    s32 pad0;
    s16 temp_v0_4;
    Actor *temp_a0;
    PlayerAdditionalActorData *temp_a1; // 30
    s32 flag;
    s32 temp_v0_3;

    switch (gCurrentActorPointer->control_state_progress) {
        case 0xB:
            if (!func_global_asm_8061CB50()) {
                gCurrentActorPointer->noclip_byte = 2;
                gCurrentActorPointer->control_state_progress = 0;
            }
            break;
        case 0xA:
        case 0:
            if (D_global_asm_807FBB70.unk200 == 4) {
                arg0->unk0 = D_global_asm_807FBB70.unk1FC;
                arg0->unk0->noclip_byte = 1;
                temp_a1 = arg0->unk0->PaaD;
                temp_a1->unk1F0 |= 0x20000000;
                gCurrentActorPointer->control_state_progress = 1;
                func_global_asm_806291B4(3); // @recomp: Push transition effect 1f earlier to capture snapshot properly
                D_global_asm_807F5D84 = 2; // @recomp: Delay notice
                arg0->unk4 = 2;
                arg0->unk3C = 0;
            }
            break;
        case 1:
            if (arg0->unk4 != 0) {
                arg0->unk4--;
                return;
            }
            temp_a0 = arg0->unk0;
            temp_v0_3 = temp_a0->unk12C;
            temp_a1 = temp_a0->PaaD;
            if (temp_v0_3 == -1) {
                temp_v0_3 = 0;
            }
            func_global_asm_806C93E4(temp_a0, temp_a1);
            func_global_asm_806C9304(arg0->unk0, temp_a1);
            func_global_asm_80659620(&arg0->unk2C, &arg0->unk30, &arg0->unk34, temp_v0_3);
            func_global_asm_80659670(1.0f, 1.0f, 1.0f, temp_v0_3);
            gCurrentActorPointer->y_rotation = arg0->unk22;
            gCurrentActorPointer->noclip_byte = 1;
            temp_v0_4 = temp_a0->unk58;
            global_properties_bitfield &= ~0x30030;
            arg0->unk6 = 0;
            while (temp_v0_4 != D_global_asm_8074E814[arg0->unk6].unk2) {
                arg0->unk6++;
            }
            arg0->unk7 = arg0->unk6;
            if (current_map != MAP_HELM) {
                playSong(MUSIC_101_TAG_BARREL_ALL_OF_THEM, 1.0f);
                func_global_asm_80602CE0(0x65, 0x7E - D_global_asm_8074E848[arg0->unk6], 1);
            }
            flag = func_global_asm_805FF018(gCurrentActorPointer->unk58, arg0->unk6);
            setFlag(flag, TRUE, FLAG_TYPE_PERMANENT);
            func_global_asm_80682E38(arg0);
            func_global_asm_80627878(temp_a1->unk104);
            arg0->unk38 = temp_a0->y_rotation;
            temp_a0->y_rotation = gCurrentActorPointer->y_rotation;
            func_global_asm_8060DEC8();
            playCutscene(gCurrentActorPointer, 0xE, 0xC);
            func_global_asm_8061B650(temp_a1->unk104);
            func_global_asm_806FB218();
            gCurrentActorPointer->control_state = 1;
            gCurrentActorPointer->control_state_progress = 0;
            break;
    }
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

extern int gameIsInDKTVMode(void);
extern int gameIsInMysteryMenuMinigameMode(void);
extern int gameIsInAdventureMode(void);
extern u8 func_global_asm_8061CB50(void);
extern u8 func_global_asm_8062919C(void);
extern u8 func_global_asm_806291A8(void);
extern s8 D_global_asm_807FC8B9;
extern u8 cc_player_index;

// @recomp: Press start to pause code
RECOMP_PATCH void func_global_asm_806E6234(void) {
    if ((D_global_asm_807FD610[cc_player_index].unk2C & START_BUTTON)
        && !func_global_asm_8061CB50()
        && (D_global_asm_807FD888 == 0.0f)
        && (gameIsInAdventureMode() || gameIsInMysteryMenuMinigameMode() || gameIsInDKTVMode())
        && !func_global_asm_8062919C()
        && !func_global_asm_806291A8()) {
        if (!gameIsInDKTVMode()) {
            func_global_asm_806291B4(7); // @recomp: Push transition effect 1f earlier to capture snapshot properly
            D_global_asm_807F5D84 = 2; // @recomp: Delay notice
        }
        D_global_asm_807FC8B9 = cc_player_index;
        global_properties_bitfield |= 1;
    }
}

extern void func_global_asm_806F5378(void);
extern void func_global_asm_807313BC(void);
extern void func_global_asm_805FF5A0(Maps map);
extern Maps next_map;
extern s32 next_exit;
extern s16 D_global_asm_8076AEF2;

// @recomp: Zipper Transition
RECOMP_PATCH void func_global_asm_805FF4D8(Maps map, s32 exit) {
    func_global_asm_806F5378();
    global_properties_bitfield |= 0x100;
    next_map = map;
    next_exit = exit;
    D_global_asm_8076AEF2 = current_map;
    func_global_asm_807313BC();
    func_global_asm_805FF5A0(map);
    // @recomp: Take the snapshot now
    addActorToTextOverlayRenderArray(rdpStoreFB, gCurrentActorPointer, 0);
    addActorToTextOverlayRenderArray(rdpClearFB, gCurrentActorPointer, 5);
    // D_global_asm_807FD9B0 = _malloc(D_global_asm_80744490 * D_global_asm_80744494 * 2);
    // func_global_asm_8070A848(D_global_asm_807FD9B0, D_global_asm_80744470[D_global_asm_807444FC]);
}

extern Gfx *func_global_asm_805FCFD8(Gfx *);
extern Gfx *func_global_asm_805FE398(Gfx *);
extern Gfx *func_global_asm_805FE4D4(Gfx *);
extern void *D_global_asm_8076A080;
extern u16 D_global_asm_8076A09C;
extern Gfx **D_1000090;

// @recomp: Zipper DL Setup
RECOMP_PATCH void func_global_asm_8070AC74(Mtx *arg0, Gfx **dlp) {
    Gfx *dl;
    dl = D_global_asm_8076A050[D_global_asm_807444FC];
    gSPSegment(dl++, 0x00, 0x00000000);
    gSPSegment(dl++, 0x02, osVirtualToPhysical(arg0));
    gSPSegment(dl++, 0x01, osVirtualToPhysical(D_global_asm_8076A080));
    gSPDisplayList(dl++, &D_1000090);
    dl = func_global_asm_805FCFD8(dl);
    dl = func_global_asm_805FE398(dl);
    gDPPipeSync(dl++);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    guTranslate(&arg0[6], 0.0f, 0.0f, 0.0f);
    guLookAt(&arg0[8], 0.0f, 0.0f, 200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    guPerspective(arg0, &D_global_asm_8076A09C, 61.9f, 1.3333334f, 10.0f, 1000.0f, 1.0f);
    gDPPipeSync(dl++);
    dl = func_global_asm_805FE4D4(dl);
    gDPSetColorDither(dl++, G_CD_MAGICSQ);
    gDPSetAlphaDither(dl++, G_AD_PATTERN);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, 0, D_global_asm_80744490, D_global_asm_80744494);
    gDPSetFillColor(dl++, 0xFFC1FFC1);
    gDPSetRenderMode(dl++, G_RM_NOOP, G_RM_NOOP2);
    gSPClearGeometryMode(dl++, G_ZBUFFER);
    gDPFillRectangle(dl++, 0, 0, D_global_asm_80744490, D_global_asm_80744494);
    gSPPerspNormalize(dl++, D_global_asm_8076A09C);
    gSPClipRatio(dl++, FRUSTRATIO_2);
    *dlp = dl;
}

extern s16 D_global_asm_80754CE0;
extern s16 D_global_asm_80754CEC[];
s16 menuXShift = 0;
s16 menuYShift = 0;
extern void *D_global_asm_807FD978[8];

// @recomp: Render main menu scrolling background
RECOMP_PATCH Gfx* func_global_asm_80706F90(Gfx* dl) {
    s32 i;
    s32 spF8;
    s32 spF4;
    s32 x;
    s32 y;
    s32 var_t5;
    u16 spE6;
    s16 var_s4;
    void *a2;
    s32 j;
    s32 X_REPEAT_COUNT, Y_REPEAT_COUNT;
    s32 width, height;

    recomp_get_ui_bounds(&width, &height);
    X_REPEAT_COUNT = (width >> 7) + 2;
    Y_REPEAT_COUNT = (height >> 7) + 2;

    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetTexturePersp(dl++, G_TP_NONE);
    gDPSetTextureFilter(dl++, G_TF_POINT);
    // 
    gEXPushScissor(dl++);
    gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, D_global_asm_80744494);
    gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, 0, 0, 0, 0);
    //
    gSPTexture(dl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPPipeSync(dl++);
    var_s4 = menuYShift + D_global_asm_80754CE0;
    if (var_s4 < 0) {
        var_s4 += 0x200;
    }
    if (var_s4 >= 0x200) {
        var_s4 -= 0x200;
    }
    if (D_global_asm_807FD978[0] == NULL) {
        for (i = 0; i < 8; i++) {
            D_global_asm_807FD978[i] = getPointerTableFile(TABLE_25_TEXTURES_GEOMETRY, D_global_asm_80754CEC[i], 0U, 0U);
        }
    }
    spE6 = 0;
    for (spF4 = 0x40; spF4 >= 0; spF4 -= 0x40) {
        spF8 = 0;
        while (spF8 < 0x80) {
            gDPPipeSync(dl++);
            a2 = D_global_asm_807FD978[spE6 % 8];
            gDPLoadTextureBlock(dl++,
                OS_PHYSICAL_TO_K0(a2),
                G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0,
                G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOLOD);
            x = (spF8 << 2) - menuXShift;
            for (var_t5 = 0; var_t5 < X_REPEAT_COUNT; var_t5++) {
                y = (spF4 << 2) - var_s4;
                for (j = 0; j < Y_REPEAT_COUNT; j++) {
                    gSPScisTextureRectangle(
                        dl++,
                        x,
                        y,
                        x + 0x80,
                        y + 0x100,
                        0,
                        0,
                        0x7FF,
                        1024,
                        -1024
                    );
                    y += 0x200;
                }
                x += 0x200;
            }
            spF8 += 0x20, spE6++;
        }
    }
    menuXShift += 4;
    menuYShift += 4;
    if (menuXShift >= 0x200) {
        menuXShift = 0;
    }
    if (menuYShift >= 0x200) {
        menuYShift = 0;
    }
    gEXPopScissor(dl++);
    gEXSetRectAlign(dl++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    gDPPipeSync(dl++);
    gDPSetTexturePersp(dl++, G_TP_PERSP);
    gDPSetTextureFilter(dl++, G_TF_BILERP);
    return dl;
}

// @recomp: Tag Barrel Kickout
void func_global_asm_80682AB4(void);
void func_global_asm_80682DF4(void*, PlayerAdditionalActorData *, u8);
void func_global_asm_806C8E58(s16);
u8 func_global_asm_805FCA64(void);
void func_global_asm_80602B60(s32 arg0, u8 arg1);
s32 deleteActor(Actor*);
void func_global_asm_806F09F0(Actor *arg0, u16 arg1);
void func_global_asm_80627888(Actor *arg0);
void func_global_asm_80709464(u8 playerIndex);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
extern s32 D_global_asm_8074E848[];
    
RECOMP_PATCH void func_global_asm_806836D0(TagAAD* arg0) {
    s32 temp_s0;
    u16 sp4A;
    s32 pad[2];
    s16 temp_s0_2;
    s32 var_v0;
    Actor* temp_v1;
    PlayerAdditionalActorData* sp30;
    s32 temp_t3;
    s32 var_v1;

    if (gCurrentActorPointer->control_state_progress == 0xFF) {
        // Kickout procedure
        temp_v1 = arg0->unk0;
        sp30 = temp_v1->AAD_as_array[0];
        var_v0 = temp_v1->unk12C;
        if (var_v0 == -1) {
            var_v0 = 0;
        }
        func_global_asm_80659670(arg0->unk2C, arg0->unk30, arg0->unk34, var_v0);
        global_properties_bitfield |= 0x30030;
        func_global_asm_80602B60(0x48, 0U);
        func_global_asm_80602B60(0x6B, 0U);
        func_global_asm_80602B60(0x49, 0U);
        func_global_asm_80602B60(0x47, 0U);
        func_global_asm_80602B60(0x65, 0U);
        deleteActor(arg0->unk1C);
        arg0->unk1C = NULL;
        var_v1 = 0;
        for (var_v1 = 0; var_v1 < 5; var_v1++) {
            deleteActor(arg0->unk8[var_v1]);
            arg0->unk8[var_v1] = 0;
        }
        // @recomp: Delay kickout stuff by 1f
        gCurrentActorPointer->control_state = 2;
        gCurrentActorPointer->control_state_progress = 0;
        arg0->unk4 = 0x1EU;
        sp30->unk1F0 &= 0xDFFFFFFF;
        temp_s0_2 = D_global_asm_8074E814[arg0->unk6].unk2;
        func_global_asm_806C8E58(temp_s0_2);
        func_global_asm_806F09F0(temp_v1, temp_s0_2);
        sp30->unk8C = 0;
        temp_v1->y_rotation = arg0->unk38;
        func_global_asm_80627888(sp30->unk104);
        func_global_asm_8061EB04(temp_v1, 0U);
        func_global_asm_80709464(0U);
        character_change_array->unk2DC.unk6 |= 0x11;
    } else {
        sp4A = arg0->unk4;
        if (arg0->unk4 != 0) {
            arg0->unk4--;
            func_global_asm_80682AB4();
        }
        arg0->unk3C++;
        if ((arg0->unk4 == 0) && (func_global_asm_805FCA64())) {
            temp_s0 = arg0->unk8[arg0->unk6]->unk58 == 0x13C;
            temp_t3 = arg0->unk3C > 9000;
            if (((D_global_asm_807FD610->unk30 >= 0x29) && (temp_t3 == 0)) || (!temp_s0)) {
                arg0->unk4 = 0x14U;
                if (arg0->unk8[arg0->unk6]->unk58 == 0x13C) {
                    func_global_asm_80682DF4(arg0->unk8[arg0->unk6], arg0->unk8[arg0->unk6]->PaaD, 0x8C);
                }
                arg0->unk7 = arg0->unk6;
                if (temp_s0 != 0) {
                    arg0->unk20 = D_global_asm_807FD610->unk2E >= 0 ? 1 : -1;
                }
                arg0->unk6 += arg0->unk20;
                if (arg0->unk6 < 0) {
                    arg0->unk6 = 4;
                } else if (arg0->unk6 >= 5) {
                    arg0->unk6 = 0;
                }
                if (arg0->unk8[arg0->unk6]->unk58 == 0x13C) {
                    func_global_asm_80682DF4(arg0->unk8[arg0->unk6], arg0->unk8[arg0->unk6]->PaaD, 0x8B);
                }
            } else {
                if ((!temp_t3) && (sp4A)) {
                    func_global_asm_80602CE0(0x65, 0x7E - D_global_asm_8074E848[arg0->unk6], 3U);
                    func_global_asm_80602CE0(0x65, D_global_asm_8074E848[arg0->unk6], 2U);
                    return;
                }
                if ((((temp_t3) && (temp_s0)) || (D_global_asm_807FD610->unk2C & 0xE000)) && (!func_global_asm_8062919C())) {
                    if (temp_s0) {
                        // @recomp: Delay kickout stuff by 1f
                        gCurrentActorPointer->control_state_progress = 0xFF;
                        func_global_asm_806291B4(3U); // @recomp: Push transition effect 1f earlier to capture snapshot properly
                        D_global_asm_807F5D84 = 2; // @recomp: Delay notice
                        return;
                    }
                    playSound(0x98, 0x7FFF, 63.0f, 1.0f, 0U, 0U);
                }
            }
        }
    }
}