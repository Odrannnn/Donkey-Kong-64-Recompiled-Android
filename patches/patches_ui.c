#include "patches.h"
#include "PR/os_message.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "misc_funcs.h"
#include "PR/sched.h"
#include "enums.h"
#include "ui.h"

typedef struct Struct80717D84 Struct80717D84;
typedef struct actor Actor;

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

typedef struct {
    Gfx *unk0[2];
} Struct80717D84_unk0;

typedef struct {
    s32 unk0;
    u8 unk4[0xA - 0x4];
    u8 unkA;
    u8 unkB;
    s16 unkC;
    s16 unkE;
    u8 pad10[0x14 - 0x10];
    u8 unk14;
    u8 unk15;
    s16 unk16;
} Struct80717D84_unk330;

struct Struct80717D84 {
    Struct80717D84_unk0 unk0[4];
    u8 unk20;
    u8 unk21;
    u8 unk22[0x28 - 0x22];
    Vtx unk28[4 * 4];
    float unk128[4][2][4][4];
    void *unk328;
    u8 unk32C;
    u8 unk32D[0x330 - 0x32D];
    Struct80717D84_unk330 *unk330;
    s8 unk334;
    s8 unk335;
    s8 unk336;
    s8 unk337;
    void *unk338;
    s16 unk33C;
    s8 unk33E;
    s8 unk33F;
    f32 unk340;
    f32 unk344;
    f32 unk348;
    s16 unk34C;
    s16 unk34E;
    u8 unk350;
    u8 unk351;
    u8 unk352;
    u8 unk353;
    s32 unk354;
    s32 unk358;
    s32 unk35C;
    f32 unk360;
    f32 unk364;
    s16 unk368;
    u8 unk36A;
    u8 unk36B;
    u8 unk36C;
    u8 unk36D;
    u8 unk36E;
    u8 unk36F;
    u32 unk370[4];
    s32 unk380;
    void *unk384;
    s16 unk388;
    s8 unk38A;
    s8 unk38B;
    u16 unk38C;
    s16 unk38E;
    s16 unk390;
    s16 unk392;
    s16 unk394;
    Struct80717D84 *unk398;
    Struct80717D84 *unk39C;
};

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

extern u8 D_global_asm_807444FC;
extern s16 D_global_asm_80744490;
extern s16 D_global_asm_80744494;
extern u8 D_global_asm_807F6009;
extern u32 D_global_asm_807F600C;
extern u32 global_properties_bitfield;
extern Actor *gPlayerPointer;
extern Mtx D_2000000;
extern Mtx D_20000C0;
extern Mtx D_2000180;
extern Mtx D_2000200;
extern void func_global_asm_8065C334(f32 arg0, f32 arg1, f32 arg2, s16 arg3, u8 *arg4, u8 *arg5, u8 *arg6, s16 arg7);
extern s32 func_global_asm_806522CC(s16 arg0, s16 arg1, s16 arg2);
extern u8 func_global_asm_80651B64(s16 arg0);
extern Gfx *func_global_asm_805FD030(Gfx *dl);
extern u8 cc_player_index;

extern u8 D_global_asm_807FDB1D; // This is being used to determine the sprite alignment
typedef enum enumSpriteAlignment {
    ALIGN_NOT_2D,
    ALIGN_UNALIGNED,
    ALIGN_LEFT,
    ALIGN_RIGHT,
} enumSpriteAlignment;
void setSpriteAlignment(enumSpriteAlignment alignment) {
    D_global_asm_807FDB1D = alignment;
}

Gfx *alignHUD(Gfx * dl, enumSpriteAlignment alignment) {
    s32 margin_reduction = 8;
    gEXPushScissor(dl++);
    gEXPushViewport(dl++);
    gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, D_global_asm_80744494);
    if (alignment == ALIGN_RIGHT) {
        // Right align
        gEXSetRectAlign(dl++, G_EX_ORIGIN_RIGHT, G_EX_ORIGIN_RIGHT,
            -(D_global_asm_80744490 - margin_reduction) * 4, 0,
            -(D_global_asm_80744490 - margin_reduction) * 4, 0);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_RIGHT, -(D_global_asm_80744490 - margin_reduction) * 4, 0);
    } else if (alignment == ALIGN_LEFT) {
        gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, 0, -margin_reduction * 4, 0, -margin_reduction * 4);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, 0, 0);
    }
    return dl;
}

Gfx *popHUD(Gfx *dl) {
    gEXPopScissor(dl++);
    gEXPopViewport(dl++);
    // Clear gEX
    gEXSetRectAlign(dl++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    gEXSetViewportAlign(dl++, G_EX_ORIGIN_NONE, 0, 0);
    return dl;
}

#define BORDER_SPRITE_TOLERANCE 60
// Draw sprites
RECOMP_PATCH Gfx * func_global_asm_80715E94(Struct80717D84* sprite, Gfx *dl, s16 arg2) {
    s32 i;
    u8 sp15B;
    u8 sp15A;
    u8 sp159;
    f32 sp154;
    f32 sp150;
    f32 sp14C;
    Gfx* temp_s0;
    
    sp154 = 1.0f;
    sp150 = 1.0f;
    sp14C = 1.0f;
    if (sprite->unk38C & 0x50) {
        return dl;
    }
    if ((((gPlayerPointer->control_state == 0x83) || (gPlayerPointer->control_state == 0x67)) && !(sprite->unk38C & 0x20)) || (((global_properties_bitfield & 0x100002) == 0x100002) && !(sprite->unk38C & 0x100))) {
        return dl;
    }
    if (arg2 == -1) {
        arg2 = sprite->unk38A;
    }
    if (sprite->unk388 != -1) {
        if (sprite->unk36F != 0) {
            // if (!func_global_asm_806522CC(sprite->unk340 * 0.25, sprite->unk344 * 0.25, sprite->unk388)) {
            //     return dl;
            // }
        } else {
            if (arg2 != sprite->unk388) {
                return dl;
            }
        }
    }
    temp_s0 = sprite->unk0[sprite->unk21++].unk0[D_global_asm_807444FC];
    gSPDisplayList(dl++, osVirtualToPhysical(temp_s0));
    if (sprite->unk36F) {
        temp_s0 = alignHUD(temp_s0, sprite->unk36F);
    }
    gDPPipeSync(temp_s0++);
    gDPSetCycleType(temp_s0++, G_CYC_1CYCLE);
    gSPLoadGeometryMode(temp_s0++, 0);
    gDPSetColorDither(temp_s0++, G_CD_DISABLE);
    if (sprite->unk36E != 0) {
        gSPSetGeometryMode(temp_s0++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
        gDPSetRenderMode(temp_s0++, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2);
    } else {
        gSPSetGeometryMode(temp_s0++, G_SHADE | G_SHADING_SMOOTH);
        gDPSetRenderMode(temp_s0++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }
    gSPTexture(temp_s0++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombineMode(temp_s0++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    if ((sprite->unk330->unk14 == 0) && (func_global_asm_80651B64(arg2) != 0)) {
        func_global_asm_8065C334(sprite->unk340, sprite->unk344, sprite->unk348, 0, &sp15B, &sp15A, &sp159, arg2);
        sp154 = (sp15B / 255.0);
        sp150 = (sp15A / 255.0);
        sp14C = (sp159 / 255.0);
    }
    gDPSetPrimColor(temp_s0++, 0, 0, sprite->unk36A * sp154, sprite->unk36B * sp150, sprite->unk36C * sp14C, sprite->unk36D);
    if (sprite->unk36F != 0) {
        gSPMatrix(temp_s0++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPMatrix(temp_s0++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        temp_s0 = func_global_asm_805FD030(temp_s0);
        // gDPSetScissor(temp_s0++, G_SC_NON_INTERLACE,
        //     0,
        //     0,
        //     D_global_asm_80744490,
        //     D_global_asm_80744494
        // );
        // if (sprite->unk388 == -1) {
        //     gDPSetScissor(temp_s0++, G_SC_NON_INTERLACE,
        //         sprite->unk38E,
        //         sprite->unk390,
        //         sprite->unk392,
        //         sprite->unk394
        //     );
        // }
    }
    gSPMatrix(temp_s0++, osVirtualToPhysical(sprite->unk128[cc_player_index][D_global_asm_807444FC]), G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    for (i = 0; i < sprite->unk380; i++) {
        if ((D_global_asm_807F6009 != 5) || (D_global_asm_807F600C != sprite->unk370[i])) {
            gDPPipeSync(temp_s0++);
            switch (sprite->unk330->unkB) {
                case 0:
                    gDPLoadTextureBlock_4b(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD    
                    );
                    break;
                case 1:
                    gDPLoadTextureBlock(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA, G_IM_SIZ_8b,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD
                    );
                    break;
                case 2:
                    gDPLoadTextureBlock(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA, G_IM_SIZ_16b,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD
                    );
                    break;
                case 3:
                    gDPLoadTextureBlock(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA, G_IM_SIZ_32b,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD
                    );
                    break;
            }
            D_global_asm_807F600C = sprite->unk370[i];
        }
        gSPVertex(temp_s0++, osVirtualToPhysical(sprite->unk28 + (i << 2)), 4, 0);
        gSP2Triangles(temp_s0++, 0, 1, 3, 0, 0, 2, 3, 0);
    }
    gSPPopMatrix(temp_s0++, G_MTX_MODELVIEW);
    gDPPipeSync(temp_s0++);
    gDPSetColorDither(temp_s0++, G_CD_MAGICSQ);
    if (sprite->unk36F != 0) {
        gSPMatrix(temp_s0++, &D_2000000, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPMatrix(temp_s0++, &D_2000200, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
        if (sprite->unk388 == -1) {
            gDPSetScissor(temp_s0++, G_SC_NON_INTERLACE,
                0,
                0,
                D_global_asm_80744490,
                D_global_asm_80744494
            );
        }
        temp_s0 = popHUD(temp_s0);
    }
    gDPPipeSync(temp_s0++);
    gSPEndDisplayList(temp_s0++);
    D_global_asm_807F6009 = 5;
    return dl;
}

typedef struct {
    s32 unk0; // screen x
    s32 unk4; // screen y
    Mtx unk8[2];
} Struct806F9D8C_arg14;

typedef struct {
    s32 unk0;
    f32 unk4;
    f32 unk8;
    s32 unkC;
    s16 unk10;
    s16 unk12;
    void *unk14;
} Struct806FA504_arg1;

typedef struct HUDDisplay {
	/* 0x000 */ u16* actual_count_pointer;
	/* 0x004 */	u16 hud_count;
	/* 0x006 */	u8 freeze_timer;
	/* 0x007 */	u8 counter_timer;
	/* 0x008 */	s32 screen_x;
	/* 0x00C */	s32 screen_y;
	/* 0x010 */ f32 unk_10;
    /* 0x014 */ f32 unk_14;
    /* 0x018 */ f32 unk_18;
    /* 0x01C */ u8 unk_1c;
    /* 0x01D */ u8 unk_1d;
    /* 0x01E */ u8 unk_1e;
    /* 0x01F */ u8 unk_1f;
	/* 0x020 */ u32 hud_state; // 0 = invisible, 1 = appearing, 2 = visible, 3 = disappearing
	/* 0x024 */ s32 unk_24;
	/* 0x028 */	void* counter_pointer;
	/* 0x02C */ u8 unk_2c; // Infinites?
    /* 0x02D */ u8 unk_2d; // Infinites?
    /* 0x02E */ u8 unk_2e;
    /* 0x02F */ u8 unk_2f;
} HUDDisplay;

typedef struct {
    // TODO: Union with friendly field names?
    // TODO: Enum with indexes?
    // 0 = Coloured Banana
    // 1 = Banana Coin
    // 2 = ???
    // 3 = ???
    // 4 = ???
    // 5 = Crystal Coconut
    // 6 = ???
    // 7 = ???
    // 8 = GB Count (Character)
    // 9 = ???
    // 10 = Banana Medal
    // 11 = ???
    // 12 = Blueprint
    // 13 = Coloured Banana?
    // 14 = Banana Coin?
    HUDDisplay hud_item[15];
} PlayerHUD;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    union {
        struct {
            s16 unk4;
            s16 unk6;
            s16 unk8;
            s16 unkA;
        };
        s16 unk4_arr[4];
    };
} Struct80750948;
extern u8 D_global_asm_807FD7E4;
extern f32 func_global_asm_80612794(s16 arg0);
extern int _sprintf(char *s, const char *fmt, ...);
extern f32 func_global_asm_80612E40(f32 arg0);
extern s32 getCenterOfString(s16 renderStyle, u8 *string);
extern Struct80750948 *func_global_asm_806C7C94(u8 arg0);
extern Mtx D_2000080;
extern Gfx **D_1000118;
extern u8 D_global_asm_807444FC;
extern Gfx *printStyledText(Gfx *dl, s16 style, s16 x, s16 y, u8 *string, u32 extraBitfield);
extern PlayerHUD *D_global_asm_80754280;
extern s16 D_global_asm_80744490;

RECOMP_PATCH Gfx *func_global_asm_806F9D8C(s32 arg0, Struct806FA504_arg1 *arg1, Gfx *dl) {
    Struct806F9D8C_arg14 *sp74;
    Struct80750948 *temp_v0_3;
    u8 sp6C[4];
    u8 sp6B;
    f32 temp_f0;
    s32 var_a2;
    s32 sp5C;
    enumSpriteAlignment alignment;

    sp74 = arg1->unk14;
    temp_f0 = func_global_asm_80612794(arg1->unk10);
    sp6B = temp_f0 * 255.0;
    sp5C = 0;
    alignment = ALIGN_UNALIGNED;
    if (arg1->unk4 < 0) {
        alignment = ALIGN_LEFT;
    } else if (arg1->unk4 > 0) {
        alignment = ALIGN_RIGHT;
    }
    dl = alignHUD(dl, alignment);
    gSPMatrix(dl++, &D_2000080, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guTranslate(&sp74->unk8[D_global_asm_807444FC], arg1->unk4 * (1.0f - temp_f0), arg1->unk8 * (1.0f - temp_f0), 0.0f);

    switch (D_global_asm_80754280->hud_item[arg0].unk_2c) {
        case 1:
            _sprintf(sp6C, "o");
            break;
        case 2:
            _sprintf(sp6C, "NA");
            break;
        default:
            if (arg0 == 5) {
                var_a2 = func_global_asm_80612E40(D_global_asm_80754280->hud_item[arg0].hud_count * 0.006666667f);
            } else {
                var_a2 = D_global_asm_80754280->hud_item[arg0].hud_count;
            }
            _sprintf(sp6C, "%d", var_a2);
            break;
    }
    gSPMatrix(dl++, &sp74->unk8[D_global_asm_807444FC], G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0, 0, 0, sp6B);
    D_global_asm_807FD7E4 = sp6B;
    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
    temp_v0_3 = func_global_asm_806C7C94(0U);
    // gDPSetScissor(dl++, G_SC_NON_INTERLACE, temp_v0_3->unk4, temp_v0_3->unk6, temp_v0_3->unk8, temp_v0_3->unkA);
    if ((D_global_asm_80744490 * 0.5) < D_global_asm_80754280->hud_item[arg0].screen_x) {
        sp5C = getCenterOfString(0x81, sp6C);
    }
    dl = printStyledText(dl, 0x81, (sp74->unk0 - sp5C) * 4, sp74->unk4 * 4, sp6C, 0U);
    dl = popHUD(dl);
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    return dl;
}


typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    f32 unk20;
    f32 unk24;
} Struct80717D84_80030894;

typedef struct menu_additional_actor_data {
    //u8 pad0[0x17];
    f32 unk0;
    f32 unk4;
    f32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s8  unk12;
    s8  unk13;
    s8 unk14;
    s8 unk15;
    s8  unk16;
    s8 unk17;
} MenuAdditionalActorData;

extern void func_menu_80030C14(s32, void*, void*);

extern void func_global_asm_80714950(s32 arg0);
extern void func_global_asm_8071495C(void);
extern void func_global_asm_8071498C(void *arg0);
extern void func_global_asm_80714998(u8 arg0);
extern void func_global_asm_807149FC(s32 arg0);
extern void func_global_asm_80714A28(u16 arg0);
extern Struct80717D84 *drawSpriteAtPosition(void *sprite, f32 scale, f32 x, f32 y, f32 z);
extern void *_malloc(s32);
extern s8 D_menu_80033F38;
#define SQ(x) ((x) * (x)) 

//@recomp: Menu Sprite Init
RECOMP_PATCH void func_menu_80030894(MenuAdditionalActorData *arg0, void *sprite, s32 x, s32 y, f32 scale, u8 arg5, s32 arg6) {
    Struct80717D84 *sp3C;
    Struct80717D84_80030894 *temp_v0;
    f32 dX;
    f32 dY;
    f32 d;
    f32 temp_f2;
    enumSpriteAlignment alignment;

    func_global_asm_80714998(arg5);
    alignment = ALIGN_UNALIGNED;
    switch (arg6) {
        case 0:
            // A/B Buttons (Barrel Screen, File Select, File Info, Delete Select,
            //              Delete Confirm, Multi Type, multi join, multi scores,
            //              sound, options, mystery, minigame scores)
            // LR Joystick (Barrel Screen, Delete Select, Delete Confirm)
            // C Up Button (Multi Join)
            // Start Button (Multi Join)
        case 0xB: // A/B Buttons (Multi Join)
        case 0xD: // A/B/CDown Buttons (Multi Join)
        case 0x10:  // A Button (Mystery Menu)
            if (x > 240) {
                alignment = ALIGN_RIGHT;
            } else if (x < 80) {
                alignment = ALIGN_LEFT;
            }
            break;
        case 2: // GB, Orange (file select screen)
        case 3: // GB (Delete Select)
        case 4: // LR Joystick (file select screen, multi type, multi join, sound, options), Orange (Delete Select)
        case 6: // Menu Icons (inside the barrel)
        case 7: // Fairy menu icon (inside the barrel)
        case 8: // Orange (Delete confirm)
        case 9: // GB (File info screen)
        case 12: // Z Button (sound, options)
        case 14: // Fairy (Mystery Menu)
        case 15: // Kong Heads (file info screen), kong placeholders (multi join)
        case 0x11: // Z Button (Mystery Menu)
        case 0x12: // Barrel Bottom
        default:
            break;
    }
    setSpriteAlignment(alignment);
    func_global_asm_807149FC(-1);
    func_global_asm_80714950((s32)arg0);
    func_global_asm_8071498C((void*)func_menu_80030C14);
    func_global_asm_80714A28(1);
    sp3C = drawSpriteAtPosition(sprite, scale, x, y, -10.0f);
    temp_v0 = _malloc(sizeof(Struct80717D84_80030894));
    sp3C->unk384 = (void*)temp_v0;
    temp_v0->unk0 = arg6;
    temp_v0->unk4 = arg0->unk15;
    temp_v0->unk8 = sp3C->unk340;
    temp_v0->unkC = sp3C->unk344;
    switch (arg6) {
        case 14:
            sp3C->unk360 = -sp3C->unk360;
            // fallthrough
        case 0:
        case 1:
        case 11:
        case 12:
        case 13:
        case 16:
        case 17:
        case 19:
            dX = 640.0f - temp_v0->unk8;
            dY = 480.0f - temp_v0->unkC;
            d = _sqrtf(SQ(dX) + SQ(dY));
            temp_f2 = 1040.0f - d;
            temp_v0->unk10 = -(dX / d) * temp_f2;
            temp_v0->unk14 = -(dY / d) * temp_f2;
            temp_v0->unk18 = sp3C->unk360;
            break;
        case 2:
        case 3:
        case 10:
            temp_v0->unk8 = x;
            temp_v0->unk10 = scale;
            temp_v0->unk14 = 0.0f;
            break;
        case 15:
            temp_v0->unk8 = x;
            break;
        default:
            temp_v0->unk10 = scale;
            temp_v0->unk14 = 0.0f;
            break;
    }
    if (D_menu_80033F38 == 0) {
        sp3C->unk36A = 0x80;
        sp3C->unk36B = 0x80;
        sp3C->unk36C = 0x80;
    }
    D_menu_80033F38 = 1;
}

typedef struct {
    s16 unk0;
    s16 unk2;
    s8 unk4;
    s8 unk5;
    s8 unk6;
    s8 unk7;
    Struct80717D84 *unk8;
} Struct806F9744_arg0_unk14;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    Struct806F9744_arg0_unk14 *unk14;
} Struct806F9744_arg0;
extern void func_global_asm_80714944(s32 arg0);
extern void changeActorColor(u8 red, u8 green, u8 blue, u8 alpha);
extern void *func_global_asm_806FACE8(u32 arg0);
extern void func_global_asm_806F94AC(Struct80717D84 *arg0, s32 arg1);
extern void func_global_asm_8071BE04(Struct80717D84 *arg0, s32 arg1);

RECOMP_PATCH void func_global_asm_806F9744(Struct806F9744_arg0 *arg0, s32 arg1, f32 x, f32 y, s32 arg4) {
    s32 temp[2]; // TODO: Hmm
    s32 sp2C;
    Struct806F9744_arg0_unk14 *temp_s0;

    temp_s0 = arg0->unk14;
    sp2C = 2;
    func_global_asm_8071495C();
    func_global_asm_807149FC(-1);
    if (arg1 == 0xE) {
        sp2C = 1;
    }
    func_global_asm_80714998(sp2C);
    func_global_asm_80714944(arg4);
    func_global_asm_80714950((s32)arg0);
    func_global_asm_8071498C(func_global_asm_8071BE04);
    func_global_asm_80714A28(0x21);
    if (arg1 == 3) {
        changeActorColor(0xFF, 0, 0, 0xFF);
    }
    s32 new_x, new_y;
    recomp_get_ui_position(x, y, &new_x, &new_y);
    temp_s0->unk8 = drawSpriteAtPosition(func_global_asm_806FACE8(arg1), 1.0f, new_x, new_y, -10.0f);
    temp_s0->unk2 = 0;
    if (arg1 == 7) {
        temp_s0->unk4 = 1;
    } else {
        temp_s0->unk4 = 0;
    }
    // TODO: unk8 is otherSpriteControl*?
    func_global_asm_806F94AC(temp_s0->unk8, arg1);
}

extern void *func_global_asm_8068C12C(u16 tex);

Gfx* displayImage_simple(Gfx* dl, s32 x, s32 y, s32 width, s32 height, s16 texture_index, s32 fmt, s32 size, f32 xScale, f32 yScale, u8 xflip, u8 yflip) {
    void *texture = func_global_asm_8068C12C(texture_index);
    gDPPipeSync(dl++);
    switch (size) {
        case 0:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_4b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case 1:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_8b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case 2:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_16b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case 3:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_32b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
    }

    s32 dsdx = (s32)((1 << 10) / xScale);
    s32 dtdy = (s32)((1 << 10) / yScale);
    s32 s_start = 0;
    s32 t_start = 0;
    s32 x0, x1, y0, y1;

    if (xflip) {
        s_start = (width - 1) << 5;
        dsdx = -dsdx;
        x1 = x;
        x0 = x1 - (s32)(width * xScale);
    } else {
        x0 = x;
        x1 = x + (s32)(width * xScale);
    }

    if (yflip) {
        t_start = (height - 1) << 5;
        dtdy = -dtdy;
        y1 = y;
        y0 = y1 - (s32)(height * yScale);
    } else {
        y0 = y;
        y1 = y + (s32)(height * yScale);
    }

    gSPTextureRectangle(dl++,
        x0 << 2, y0 << 2,
        x1 << 2, y1 << 2,
        G_TX_RENDERTILE,
        s_start, t_start, dsdx, dtdy);

    gDPPipeSync(dl++);
    return dl;
}

#define DKTV_OFFSET 0

//@recomp: Demo tv corner borders
RECOMP_PATCH Gfx *func_global_asm_806FF144(Gfx *dl) {
    gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gEXPushScissor(dl++);
    gEXPushViewport(dl++);
    gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, D_global_asm_80744494);
    gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, 0, 0, 0, 0);
    gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, 0, 0);
    dl = displayImage_simple(dl,
        0, 0,
        0x40, 0x40,
        0x3A, G_IM_FMT_IA, 1,
        2.0f, 2.0f,
        0, 0);
    dl = displayImage_simple(dl,
        0, D_global_asm_80744494,
        0x40, 0x40,
        0x3A, G_IM_FMT_IA, 1,
        2.0f, 2.0f,
        0, 1);
    gEXPopViewport(dl++);
    gEXPopViewport(dl++);
    gEXSetRectAlign(dl++, G_EX_ORIGIN_RIGHT, G_EX_ORIGIN_RIGHT, 0, 0, 0, 0);
    gEXSetViewportAlign(dl++, G_EX_ORIGIN_RIGHT, 0, 0);
    dl = alignHUD(dl, ALIGN_RIGHT);
    dl = displayImage_simple(dl,
        D_global_asm_80744490, 0,
        0x40, 0x40,
        0x3A, G_IM_FMT_IA, 1,
        2.0f, 2.0f,
        1, 0);
    dl = displayImage_simple(dl,
        D_global_asm_80744490, D_global_asm_80744494,
        0x40, 0x40,
        0x3A, G_IM_FMT_IA, 1,
        2.0f, 2.0f,
        1, 1);
    gEXPopScissor(dl++);
    gEXPopViewport(dl++);
    gEXSetRectAlign(dl++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    gEXSetViewportAlign(dl++, G_EX_ORIGIN_NONE, 0, 0);
    return dl;
}

extern u8 *getTextString(u8 fileIndex, s32 stringIndex, s32 arg2);
extern Mtx D_global_asm_807FDAC0;
//@recomp: DKTV "DK TV" text
RECOMP_PATCH Gfx *func_global_asm_8071338C(Gfx *dl) {
    u8 *string;
    f32 temp;

    string = getTextString(0xC, 0, 1);
    gDPSetCombineMode(dl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gSPMatrix(dl++, &D_global_asm_807FDAC0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    temp = 20.0f;
    temp *= 2.0f;
    temp *= 4.0f;
    dl = alignHUD(dl, ALIGN_LEFT);
    dl = printStyledText(dl, 1, 0x118, temp, string, 4);
    dl = popHUD(dl);
    return dl;
}

extern u32 object_timer;
//@recomp: Locked camera icon
RECOMP_PATCH Gfx *func_global_asm_8068D8C8(Gfx *dl, s32 arg1) {
    gSPDisplayList(dl++, &D_1000118);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xA0);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    dl = alignHUD(dl, ALIGN_RIGHT);
    dl = displayImage_simple(dl,
        D_global_asm_80744490 - 50, D_global_asm_80744494 - 40,
        32, 32,
        ((((u32)object_timer >> 1) & 0xF) + 0x8F),
        G_IM_FMT_RGBA, 2,
        1.0f, 1.0f,
        0, 0);
    dl = popHUD(dl);
    return dl;
}

// This requires mallocs to be resolved
/*
    Gfx* func_global_asm_806ABA6C(Gfx*, void*, s32);
    extern s8 D_global_asm_80750530[];
    extern s8 D_global_asm_80750538;
    extern s8 D_global_asm_80750560[];
    extern s16 D_global_asm_8075056C[];
    extern s8 D_global_asm_807505CC;
    extern u8 **D_global_asm_807FC7E0;
    typedef struct Struct807FC7F0 {
        f32 unk0;
        f32 unk4;
        s16 unk8;
        s16 unkA;
        s16 unkC;
        s16 unkE;
        s8 unk10;
        u8 unk11[7];
    } Struct807FC7F0;
    extern Struct807FC7F0 *D_global_asm_807FC7F0[];
    extern s8 D_global_asm_807FC7F8[];
    extern s8 D_global_asm_807FC80C;
    extern s8 D_global_asm_807FC818[];
    extern f32 D_global_asm_807FC840[];
    extern f32 D_global_asm_807FC868[];
    extern f32 D_global_asm_807FC890[];
    extern s8 D_global_asm_807FCC4B;
    extern u8 D_global_asm_807FCC4C;
    typedef struct character_progress {
        u8 moves;
        u8 simian_slam;
        u8 weapon;
        u8 ammo_belt;
        u8 instrument;
        u8 unk5;
        u16 coins;
        u16 instrument_ammo;
        u16 coloured_bananas[14];
        u16 coloured_bananas_fed_to_tns[14];
        u16 golden_bananas[14];
    } CharacterProgress;
    typedef struct PlayerProgress {
        union {
            CharacterProgress character_progress[6];
            u8 character_progress_as_bytes[6][0x5E];
            u16 character_progress_as_shorts[6][0x2F];
        };
        u8 unk234[0x2F0 - 0x234];
        u16 standardAmmo;
        u16 homingAmmo;
        u16 oranges;
        u16 crystals;
        u16 film;
        s8 unk2FA;
        s8 health;
        u8 melons;
        s8 unk2FD;
        u16 unk2FE[(0x306 - 0x2FE) / 2];
    } PlayerProgress;

    typedef struct PauseAAD {
        f32 unk0;
        f32 unk4;
        s16 unk8[2];
        s16 unkC[2];
        s16 unk10;
        s8 unk12;
        u8 unk13;
        s8 unk14;
        u8 unk15;
        s8 unk16;
        s8 unk17;
        u8 unk18;
    } PauseAAD;

    extern void *_malloc(s32);
    extern PlayerProgress D_global_asm_807FC950[];
    extern u8 getLevelIndex(u8 map, u8 arg1);
    extern u8 D_global_asm_8076A0AB;
    extern void func_global_asm_8061134C(void*);
    extern Gfx *func_global_asm_806A921C(Gfx *dl);
    extern Gfx *displayImage(Gfx *dl, u16 textureIndex, s32 arg3, u32 codec, s32 width, s32 height, s16 x, s16 y, f32 xScale, f32 yScale, s32 arg11, f32 arg12);
    extern Gfx *printText(Gfx *dl, s16 x, s16 y, f32 scale, u8 *string);
    extern f32 func_global_asm_80612790(s16 arg0);
    extern f32 func_global_asm_80612794(s16 arg0);
    extern Gfx *func_global_asm_806AA09C(s16 x, s16 y, s16 arg2, s16 arg3, Gfx *dl, s8 arg5, f32 scale);
    extern s16 D_global_asm_807FC828[];
    extern u8 func_global_asm_80712628(void);

    #define BORDER_TEXT_TOLERANCE 100
    RECOMP_PATCH void* func_global_asm_806A92B4(Gfx *dl, Actor *arg1) {
        u8 sp160[0x40];
        PauseAAD* sp15C;
        s16 i;
        s16 j;
        PlayerProgress *player;
        s32 world;
        Gfx* local_dl;
        f32 temp_f0;
        s16 temp_f18;
        f32 temp_f20;
        f32 temp_f22;
        f32 temp_f24;
        f32 temp_f26;
        f32 temp_f30;
        f32 temp_f8;
        f32 var_f0;
        f32 var_f14;
        f32 var_f2;
        s16 temp_s0;
        s16 temp_s4_2;
        s16 temp_t1;
        s16 var_s1;
        s32 var_s2_2;
        s16 var_s4;
        s32 gb_count;
        s32 temp_f8_2;
        Struct807FC7F0 *temp_s2;
        s32 temp_t7;
        u8 temp_t8;
        s32 temp_t8_2;
        s32 temp_t9_2;
        s32 var_v1;
        s8 health;
        Struct807FC7F0 *temp_v0_2;
        s32 gb_count_0;
        s32 x;
        s32 y;
        s32 offset;

        sp15C = arg1->AAD_as_array[0];
        player = &D_global_asm_807FC950[0];
        world = getLevelIndex(D_global_asm_8076A0AB, 1U);
        local_dl = _malloc(0x1F40);
        func_global_asm_8061134C(local_dl);
        gSPDisplayList(dl++, local_dl);
        local_dl = func_global_asm_806A921C(local_dl);
        for (j = 0; j < 2; j++) {
            temp_s2 = D_global_asm_807FC7F0[j];
            if (temp_s2) {
                for (i = 0; i < D_global_asm_807FC7F8[j]; i++) {
                    var_f0 = sp15C->unk0;
                    temp_v0_2 = &temp_s2[i];
                    if (temp_v0_2->unk10 == 2) {
                        var_f0 += sp15C->unk4;
                    }
                    x = (temp_v0_2->unk8 * 4) + (s32)(var_f0 * temp_v0_2->unk0) + (s16)(temp_v0_2->unkC * 4);
                    y = (temp_v0_2->unkA * 4) + (s32)(var_f0 * temp_v0_2->unk4) + (temp_v0_2->unkE * 4);
                    if (x < (BORDER_TEXT_TOLERANCE * 4)) {
                        local_dl = alignHUD(dl, 1, 0);
                    } else if (x > ((320 - BORDER_TEXT_TOLERANCE) * 4)) {
                        local_dl = alignHUD(dl, 0, 1);
                    }
                    y += (sp15C->unkC[j] * 4);
                    local_dl = printStyledText(local_dl, 1, x, y, temp_v0_2->unk11, 1U);
                }
            }
        }
        switch (sp15C->unk12) {
        case 0:
            temp_t8 = (player->melons * 0x34) - 0x34;
            health = player->health;\
            temp_s4_2 = 0x140;
            temp_s4_2 = 0xA0 - (s32) (temp_s4_2 * sp15C->unk0);
            for (i = 0; i < player->melons; i++) {
                temp_s0 = (i << 0xA) + (object_timer * 0xC8);
                temp_f24 = (func_global_asm_80612794(temp_s0) * 0.3f) + 3.0f;
                temp_f20 = (func_global_asm_80612794(temp_s0 + 0x800) * 0.3f) + 3.0f;
                temp_f22 = func_global_asm_80612794((temp_s0 >> 1) + 0x200) * 10.0f;
                temp_f8 = func_global_asm_80612794(temp_s0);
                temp_f24 *= 0.75f;
                gDPPipeSync(local_dl++);
                gDPSetPrimColor(local_dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x40);
                temp_f8_2 = (((i * 0x69) + 0x280) + (temp_f22 * 0.75f)) - temp_t8;
                temp_f26 = -(temp_f20 * 0.75f);
                temp_f8 *= 0.5f;
                temp_f8 += 0.5f;
                temp_f8 *= 30.0f;
                temp_f8 *= 0.75f;
                local_dl = displayImage(local_dl, 0x5DU, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0x30, 0x2A,
                    temp_f8_2, temp_s4_2 + (s16)temp_f8,
                    temp_f24, temp_f26, 0, 0.0f);
                gDPPipeSync(local_dl++);
                gDPSetPrimColor(local_dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                if (health > 0) {
                    var_v1 = MIN(health - 1, 3);
                    local_dl = displayImage(local_dl, var_v1 + 0x5A, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0x30, 0x2A,
                        temp_f8_2, temp_s4_2 + temp_f8,
                        temp_f24, temp_f26, 0, 0.0f);
                }
                health = MAX(0, health - 4);
            }
            gDPPipeSync(local_dl++);
            var_s1 = 0x198;
            var_s4 = (1.0f - sp15C->unk0) * 255.0f;
            if (var_s4 < 0) {
                var_s4 = 0;
            }
            if (var_s4 >= 0x100) {
                var_s4 = 0xFF;
            }
            if (sp15C->unk17 != 3) {
                for (i = 0; i < 3; i++) {
                    if (i == sp15C->unk17) {
                        gDPSetPrimColor(local_dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_s4);
                    } else {
                        gDPSetPrimColor(local_dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_s4 >> 1);
                    }
                    if ((i == 2) && ((world == 7) || ((world == 8) && (func_global_asm_80712628()) && (D_global_asm_807505CC == 0)))) {
                        i++;
                    }
                    j = i;
                    if ((i == 1) && (D_global_asm_807505CC != 0)) {
                        j = 4;
                    }
                    if ((j == 2) && (D_global_asm_807505CC != 0)) {
                        j = 5;
                    }
                    local_dl = printText(local_dl, 0x280, var_s1, 0.7f, D_global_asm_807FC7E0[j]);
                    var_s1 += 0x44;
                }
            } else {
                gDPSetPrimColor(local_dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_s4);
                local_dl = printText(local_dl, 0x280, 0x198, 0.7f, D_global_asm_807FC7E0[7]);
                _sprintf(sp160, "q %s", D_global_asm_807FC7E0[0x10]);
                local_dl = printText(local_dl, 0x280, 0x1E8, 1, sp160);
                _sprintf(sp160, "b %s", D_global_asm_807FC7E0[0x11]);
                local_dl = printText(local_dl, 0x280, 0x238, 1, sp160);
            }
            break;
        case 1:
            for (j = 0; j < 2; j++) {
                temp_t8_2 = sp15C->unkC[j] << 2;
                var_s2_2 = -sp15C->unk10;
                temp_f0 = 1.0f - sp15C->unk0;
                if ((temp_t8_2 < 0x3B6) && (temp_t8_2 >= -0x3B5)) {
                    if (temp_f0 > 0.05f) {
                        temp_f20 = 180.0f * temp_f0;
                        temp_f0 *= 3.0f;
                        i = 0;
                        while (i < 5) {
                            temp_s0 = (s32) (func_global_asm_80612794(var_s2_2) * temp_f20) + 0x280;
                            temp_f18 = func_global_asm_80612790(var_s2_2) * temp_f20;
                            offset = i + (j * 5);
                            D_global_asm_807FC840[offset] = (s16) (temp_s0 + 0x20) - (6.0f * temp_f0);
                            D_global_asm_807FC868[offset] = (s16) ((s32) (((32.0f * temp_f0) + 480.0f) - temp_f18) + temp_t8_2) - (30.0f * temp_f0);
                            D_global_asm_807FC890[offset] = temp_f0 * 1.33;
                            if (D_global_asm_80750530[i++]) {
                                D_global_asm_807FC890[offset] *= 0.75;
                            }
                            var_s2_2 += 0x333;                       
                        }
                        gDPPipeSync(local_dl++);
                    }
                    local_dl = func_global_asm_806ABA6C(local_dl, sp15C, j);
                }
                temp_t1 = sp15C->unk8[j];
                if (temp_t1 != 8) {
                    var_f14 = 780.0f;
                    var_f0 = MAX(sp15C->unk0, sp15C->unk4);
                    if (temp_t8_2 == 0) {
                        var_f14 += (var_f0 * 400.0);
                    }
                    gb_count_0 = *(s16*)(&player->character_progress[sp15C->unk14].golden_bananas[D_global_asm_80750560[temp_t1]]);
                    local_dl = func_global_asm_806AA09C(0x2A8,
                        temp_t8_2 + var_f14,
                        gb_count_0,
                        5, local_dl, 0, 0.8f);
                }
            }
            D_global_asm_80750538++;
            if (D_global_asm_80750538 >= 0xE) {
                D_global_asm_80750538 = 0;
            }
            break;
        case 2:
            for (j = 0; j < 2; j++) {
                temp_t9_2 = sp15C->unkC[j] * 4;
                if ((temp_t9_2 < 0x3B6) && (temp_t9_2 >= -0x3B5)) {
                    local_dl = func_global_asm_806ABA6C(local_dl, sp15C, j);
                }
                gb_count = 0;
                for (i = 0; i < 5; i++) {
                    gb_count += player->character_progress[i].golden_bananas[D_global_asm_80750560[sp15C->unk8[j]]];
                }
                if (sp15C->unk8[j] != 8) {
                    var_f14 = 780.0f;
                    var_f0 = MAX(sp15C->unk0, sp15C->unk4);
                    if (temp_t9_2 == 0) {
                        var_f14 += (f64)(var_f0 * 400.0);
                    }
                    local_dl = func_global_asm_806AA09C(0x2B0, temp_t9_2 + var_f14, gb_count, 0x19, local_dl, 0, 0.8f);
                }
            }
            break;
        case 3:
            local_dl = printText(local_dl, 0x280, 0x74, 0.5f, D_global_asm_807FC7E0[0x12 + D_global_asm_807FC818[D_global_asm_807FC80C]]);
            local_dl = printText(local_dl, 0x280, 0x3C, 0.65f, D_global_asm_807FC7E0[6]);
            var_f14 = MAX(sp15C->unk4, sp15C->unk0);
            var_f2 = (1.0f - var_f14) * 0.65f;
            if (var_f2 < 0.01f) {
                var_f2 = 0.01f;
            }
            local_dl = func_global_asm_806AA09C(0x280, 0x33C, D_global_asm_807FC828[D_global_asm_807FC818[D_global_asm_807FC80C]], D_global_asm_8075056C[D_global_asm_807FC818[D_global_asm_807FC80C]], local_dl, 1, var_f2);
            break;
        }
        gSPEndDisplayList(local_dl++);
        return dl;
    }
*/
