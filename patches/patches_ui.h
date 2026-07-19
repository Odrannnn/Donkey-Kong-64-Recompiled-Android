#define MATH_HALF_PI_D 1.5707963705062866
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

extern void *func_global_asm_8068C12C(u16 tex);
extern f32 func_global_asm_80612D10(f32 arg0);
extern f32 func_global_asm_80612D1C(f32 arg0);

extern u8 *getTextString(u8 fileIndex, s32 stringIndex, s32 arg2);
extern Mtx D_global_asm_807FDAC0;
extern u32 object_timer;

typedef struct {
    s32 id;
    u8 images_per_frame_horizontal;
    u8 images_per_frame_vertical;
    u8 unk6;
    u8 codec;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 table;
    s16 width;
    s16 height;
    s16 image_count;
    s16 images[1]; // TODO: How many elements? m2c doesn't support VLAs
} SpriteData;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
} CharacterChange8Array;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
} CharacterChange250;

typedef struct {
    f32 unk0;
    u16 unk4;
    u16 unk6;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
} CharacterChange2DC;

typedef struct {
    u8      does_player_exist; // bitfield? 0x00
    u8      unk1;
    u8      unk2;
    u8      unk3;
    void* playerPointer;    // 0x04
    Mtx     unk8[2];
    Mtx     unk88[2];
    u8      pad108[0x188 - 0x108];
    u16     unk188;
    u8      pad18A[0x190 - 0x18A];
    LookAt  unk190[2];
    Hilite  unk1D0[2];
    Hilite  unk1F0[2]; // Unsure on struct. Is a 0x10-sized struct
    union {
        struct {
            f32     look_at_eye_x; // 0x210 maybe an array?
            f32     look_at_eye_y; // 0x214
            f32     look_at_eye_z; // 0x218
        };
        f32 look_at_eye[3];
    };
    f32     unk21C; // Used
    f32     unk220; // Used
    f32     unk224; // Used
    f32     look_at_at_x; // 0x228 maybe an array?
    f32     look_at_at_y; // 0x22C
    f32     look_at_at_z; // 0x230
    f32     unk234; // Used
    f32     unk238; // Used
    f32     unk23C; // Used
    f32     look_at_up_x; // 0x240 maybe an array?
    f32     look_at_up_y; // 0x244
    f32     look_at_up_z; // 0x248;
    f32     unk24C;
    CharacterChange250 unk250[2];
    s16     unk270[4];
    union {
        struct {
            s16     unk278;
            s16     unk27A;
        };
        s16 unk278_arr[2];
    };
    f32     fov_y; // 0x27C
    f32     unk280;
    f32     near; // 0x284
    f32     far; // 0x288
    f32     unk28C;
    s16     chunk; // 0x290
    s16     unk292;
    OSContPad* unk294; // Used
    OSContPad* new_controller_inputs; // bitfield 0x298
    s16     action_initiated; // 0x29C
    s16     unk29E;
    void* unk2A0;
    s32     unk2A4;
    s32     unk2A8; // Used
    s32     unk2AC;
    s32     unk2B0; // Used
    tuple_f unk2B4;
    u8      unk2C0; // Used
    u8      unk2C1; // Used
    u8      unk2C2; // Used
    u8      unk2C3; // Used
    f32     unk2C4; // Used
    s16     unk2C8; // Used
    s16     unk2CA;
    s16     unk2CC;
    s16     unk2CE;
    f32     unk2D0;
    f32     unk2D4;
    f32     unk2D8;
    CharacterChange2DC      unk2DC;
    u8      unk2E8;
    u8      unk2E9;
    u8      unk2EA;
    u8      unk2EB;
    u8      unk2EC;
    u8      unk2ED;
    u8      unk2EE;
    u8      unk2EF;
} CharacterChange;


extern CharacterChange* character_change_array;

typedef struct global_asm_struct_71 GlobalASMStruct71;

struct global_asm_struct_71 {
    s32 unk0;
    s32 unk4;
    s32 unk8; // Used
    s32 unkC;
    s16 unk10;
    s16 unk12;
    GlobalASMStruct71 *unk14; // Used, prev?
    GlobalASMStruct71 *unk18; // Next?
};

typedef struct {
    u8 unk0[0x340 - 0x0];
    f32 unk340;
    f32 unk344;
    u8 unk348[0x35E - 0x348];
    s16 unk35E;
    f32 unk360;
    f32 unk364;
} Struct806F9AF0_arg0;

extern void *D_global_asm_80750518;
extern f32 D_global_asm_807FD7A0[];
extern void func_global_asm_806F9AF0(GlobalASMStruct71 *arg0, s8 *arg1);
extern void func_global_asm_806F966C(GlobalASMStruct71 **arg0);
extern void func_global_asm_806F96CC(GlobalASMStruct71 *arg0, u32 arg1);

typedef struct {
    s32 unk0;
    void *unk4[1];
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s8 unk18[1];
} Struct8002733C;
void func_global_asm_80715908(Struct80717D84 *arg0);
void func_global_asm_8071A038(Struct80717D84 *arg0, s32 arg1);
extern SpriteData D_global_asm_8071FFD4; 
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);

typedef struct {
    OSTime unk0;
    u32 unk8;
    s32 unkC;
    u8 unk10;
} AAD_global_asm_806A2A10;
typedef struct Struct80754AD0 Struct80754AD0;

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
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
} Struct806FD9FC;

struct Struct80754AD0 {
    Struct80754AD0 *next;
    u8 *unk4;
    Struct806FD9FC *unk8;
    u8 unkC;
    u8 unkD;
    u8 padE[0x10 - 0x0E];
    f32 unk10;
    f32 unk14;
    f32 unk18;
    s16 unk1C;
};

extern SpriteData D_global_asm_8071FC58;
extern u8 func_global_asm_805FCA64(void);
extern Gfx *func_global_asm_8070068C(Gfx *dl);
extern Struct80754AD0 *func_global_asm_806FD9B4(s16 arg0);
extern Gfx *func_global_asm_806FE078(Gfx *dl, s16 arg1, s32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6);
extern Mtx D_2000100;

typedef struct {
    u8 unk0;
    u8 unk1;
    s16 unk2;
    s16 unk4;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
} A178_80024000;

typedef struct {
    u8 unk0[0x14 - 0x0];
    s16 unk14;
    s16 unk16;
    u8 unk18;
    u8 unk19;
} AAD_bonus_800252A0;

typedef struct KremlingKoshAAD {
    void* sprite[5];
    u8 unk14[0x1E - 0x14];
    s16 x;
    s16 y;
    u8 unk22;
    u8 timer;
    u8 unk24;
    u8 unk25;
    u8 unk26;
} KremlingKoshAAD;

typedef struct KremlingKoshInit {
    Actor* slots[8];
    s16 hit_requirement;
    s16 hit_requirement_hud;
    u8 unk24;
    u8 unk25;
    u8 unk26;
    u8 no_spawn_percent;
    u8 green_chance;
    u8 time_limit;
    u8 unk2A[2];
    f32 unk2C;
} KremlingKoshInit;

extern Gfx *func_bonus_80026690(Gfx *dl, Actor *arg1);
extern u8 func_global_asm_806FD894(s16 arg0);

Gfx* func_global_asm_8068DC54(Gfx* dl, s16 arg1, s16 arg2, s16* arg3, s16 arg4, u8* arg5);

typedef struct KrazyKKAAD {
    u8 pad0[0x25];
    u8 unk25;
    u8 unk26;
    u8 unk27;
    s16 unk28;
    s16 unk2A;
} KrazyKKAAD;

typedef struct KrazyKKAAD178 {
    u8 pad0[0x3];
    u8 unk3;
    u8 pad4[0x6 - 0x4];
    u8 unk6;
    u8 unk7;
    s16 unk8;
    s16 unkA;
    u8 padC[0x11 - 0xC];
    u8 unk11;
    u8 unk12;
    u8 unk13;
    s16 unk14;
    s16 unk16;
} KrazyKKAAD178;

typedef struct {
    u8 unk0[0x23];
    u8 unk23;
    u8 unk24;
    u8 unk25;
    s16 unk26;
    s16 unk28;
} AAD_8002CC08;

extern Gfx *displayImage(Gfx *dl, u16 textureIndex, s32 arg3, u32 codec, s32 width, s32 height, s16 x, s16 y, f32 xScale, f32 yScale, s32 arg11, f32 arg12);
extern s32 func_global_asm_80626F8C(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, s32 arg5, f32 arg6, s32 arg7);
extern f32 D_bonus_8002DEB4;

typedef struct {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    u8 unk3;
    s8 unk4;
    s8 unk5;
    u8 unk6;
    s8 unk7;
    s16 unk8;
    s16 unkA;
    u8 unkC;
    u8 unkD;
} AAD_8002D010;

extern Maps current_map;
extern s8 D_bonus_8002DEF0[];
#define D_bonus_8002D92C (*(volatile s8 *)0x8002D92C)

extern u8 D_global_asm_80750AD4;
extern Gfx **D_1000118;
extern Gfx* printText(Gfx* dl, s16 x, s16 y, f32 scale, u8* string);