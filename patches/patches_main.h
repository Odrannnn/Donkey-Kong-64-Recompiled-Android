#include "patches.h"
#include "PR/os_message.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "misc_funcs.h"
#include "PR/sched.h"
#include "enums.h"

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define	OS_TV_PAL		0
#define PI_STATUS_DMA_BUSY (1 << 0)
#define OS_READ 0
#define UNK_ADDR 0x800002E8
#define	OS_CLOCK_RATE		62500000LL
#define	OS_CPU_COUNTER		(OS_CLOCK_RATE*3/4)
#define OS_NSEC_TO_CYCLES(n)	(((u64)(n)*(OS_CPU_COUNTER/15625000LL))/(1000000000LL/15625000LL))
#define BUFFER_TIME OS_NSEC_TO_CYCLES(48484843)
#define SQ(x) ((x) * (x))

extern void osWritebackDCache(void*, s32);
extern u32  osPiGetStatus(void);
extern void osInvalDCache(void*, s32);

typedef u32 OSIntMask;

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

typedef struct OverlayInfoStruct {
    s32 rom_code_start;
    s32 rom_data_end;
    void* rdram_start;
    void* overlay_end;
    void* rdram_code_end;
    void* rdram_data_end;
} OverlayInfoStruct;
extern OverlayInfoStruct gOverlayTable[];

typedef struct {
    /* 0x0 */ s32 unk0;   // ID or handle; freed via func_global_asm_80611408
    /* 0x4 */ u8  unk4;   // countdown timer; evict when decremented to 0
    /* 0x5 */ u8  pad5;
    /* 0x6 */ u16 pad6;
    /* 0x8 */             // sizeof == 0x8
} Unk807F0A58Entry;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Struct80744464;

typedef struct UnkMQStruct {
    OSMesgQueue mq;
    OSMesg msgs[1];
} UnkMQStruct;

extern Struct80744464 D_global_asm_80744464;
typedef u64	OSTime;
extern OSThread D_global_asm_80761430;
extern OSTime osGetTime(void);
extern Unk807F0A58Entry D_global_asm_807F0A58[];
extern s32 D_global_asm_807F5A58;  // active entry count
extern s32 D_global_asm_8076A070;
extern s32 osTvType;
extern s8 D_global_asm_80744460;
extern u16 D_global_asm_8074682C;
extern u8 D_global_asm_80744504;
extern u8 D_global_asm_807F059C[];
extern u64 gStackCanary;
extern s32 D_global_asm_807655E0;
extern u8 D_global_asm_807444F0;
extern s32 D_global_asm_80767CC0;
extern Maps current_map;
extern s32 current_exit;
extern Maps next_map;
extern s32 next_exit;
extern u8 is_cutscene_active;
extern f32 D_global_asm_807444BC;
extern f32 D_global_asm_807FD888;
extern u32 global_properties_bitfield;
extern u8 D_global_asm_807444F8;
extern OSMesgQueue D_global_asm_8076A110;
extern u8  D_global_asm_8076A0B1;
extern u8  D_global_asm_8076A0B2;
extern u16* D_global_asm_80744470[2];
extern Gfx* D_global_asm_8076A050[];
extern u8 D_global_asm_807444FC;

typedef struct {
    OSMesgQueue queue;
    u8 unk18[0x260 - 0x18];
    s32 unk260;
    u8 unk264[0x280 - 0x264];
    s32 unk280;
    s32 frame_count;
} Struct80767A40;

extern Struct80767A40 D_global_asm_80767A40; // TODO: Might be an array, see func_global_asm_805FBC5C
extern s32 D_global_asm_8076AF10;
extern u8 D_global_asm_8076AF14;
extern s16 D_global_asm_8076AF00[];
extern u8 D_global_asm_80745290;
extern u32 D_global_asm_80744478;
extern u32 object_timer;
extern s32 D_global_asm_807FBB64; // shadow_display_bitfield, shockwave_bitfield, long_distance_actor_spawn


extern OSViMode osViModeTable[];
extern s16 D_global_asm_80744494;
extern s16 D_global_asm_80744498;
extern s16 D_global_asm_8074449C;
extern s16 D_global_asm_807444A0;
extern s16 D_global_asm_807444A4;
extern s16 D_global_asm_807444A8;
extern s16 D_global_asm_807444AC;
extern s16 D_global_asm_807444B0;
extern s16 D_global_asm_807444B4;
extern u8 D_global_asm_8074450C;
extern u8 D_global_asm_80744510;
extern s32 D_global_asm_80744588[];
extern s8 D_global_asm_807445A0;
extern s8 D_global_asm_807445A4;
extern s16 D_global_asm_80744490;
extern s16 D_global_asm_8076A0AA;
extern u8 D_global_asm_807FD890;

typedef struct Unk {
    char unk_00[4];
    s32 unk_04;
    s32 unk_08;
} Unk;

typedef struct Struct131B0_1 Struct131B0_1;

struct Struct131B0_1 {
    Struct131B0_1* next;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    char unk_14[0x3C];
    Unk* unk_50;
};

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
} Struct131B0_2_unk268;

// P Sure this is OSScTask_s
// typedef struct {
//     s32 unk0;
//     s32 unk4;
//     s32 unk8;
//     s32 unkC;
//     OSTask unk10;
// /* 0x50 */ char unk_50[4];
// /* 0x54 */ OSMesgQueue* queue;
// /* 0x58 */ OSMesg mesg;
// } Struct131B0_2_unk274;

typedef struct {
    /* 0x000 */ u8 pad0[0x18];
    /* 0x018 */ void* func;
    /* 0x01C */ u8 pad1C[0x3C];
    /* 0x058 */ OSMesgQueue* mesgQueue;
    /* 0x05C */ char pad5C[4];
    /* 0x060 */ s32 unk_60;
    /* 0x064 */ char pad64[0xC];                    /* maybe part of unk_60[4]? */
    /* 0x070 */ void* unk70;                        /* inferred */
    /* 0x074 */ char pad74[0x3C];                   /* maybe part of unk70[0x10]? */
    /* 0x0B0 */ OSThread unkB0;                     /* inferred */
    /* 0x260 */ Struct131B0_1* unk260;
    /* 0x264 */ Struct131B0_1* unk264;
    /* 0x268 */ Struct131B0_2_unk268* unk268;
    /* 0x26C */ Struct131B0_1* unk26C;
    /* 0x270 */ Struct131B0_1* unk270;
    /* 0x274 */ OSScTask* unk274;
    /* 0x278 */ void* unk_278; //unknown what this points to
    /* 0x27C */ char pad27C[4];
    /* 0x280 */ s32 unk_280;
    /* 0x284 */ s32 unk_284;
    /* 0x288 */ OSTime unk_288;
    /* 0x290 */ OSTime unk290;
} Struct131B0_2;                                    /* size = 0x298 */

extern s32 D_global_asm_8076A084;
extern UnkMQStruct D_global_asm_807655F0;
extern UnkMQStruct D_global_asm_807656D0;

extern UnkMQStruct D_global_asm_807659E8;

extern OSMesg D_global_asm_8076A108;
extern OSTimer D_global_asm_8076A130;

void func_global_asm_805FB750(s32 arg0, s32 arg1, void* arg2);
void func_dk64_boot_800024E0(u8** arg0, s32* arg1, void* arg2);
void func_global_asm_806003EC(s16);
s32 func_global_asm_8060042C(s16 arg0);
void func_global_asm_8060EC80(OSMesgQueue* arg0, void* arg1, s32 arg2, u8 arg3, u8 arg4);
extern s32 D_global_asm_807655E0;
extern s32 D_global_asm_80767CD8;
void func_global_asm_8060FFF0(void);
void func_global_asm_8060A900(void);
void func_global_asm_80600D50(void);
void func_global_asm_8060ED6C(Struct131B0_2* arg0, Struct131B0_1* arg1, s32 arg2, s32 arg3, s32 arg4);
extern s32 D_global_asm_8076A070;
extern s32 D_global_asm_8076A07C;
void setIntroStoryPlaying(u8 arg0);
void func_global_asm_8073239C(void);
void func_global_asm_805FE544(u8 arg0);
void func_global_asm_805FB7E4(void);
void func_global_asm_80610350(u8, u8, s32);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
void func_global_asm_80601CF0(s32 arg0);
void func_global_asm_8060E930(void);
void func_global_asm_80611724(u32, u32);
void func_global_asm_80611408(s32);
void boot_osPiRawStartDma(s32, s32, void*, s32);
void func_global_asm_805FBC5C(void);
void osViSetSpecialFeatures(u32);
void func_global_asm_805FBE04(void);
void func_global_asm_805FE7FC(void);
void func_global_asm_8070A934(s32 nextMap, s32 nextExit);
void yield_self(void);
void func_dk64_boot_80000980(void);
void load_dk64_overlay(s32, void*, s32);
void osInitialize(void);
void func_arcade_80024000(void);
void func_jetpac_80024000(void);
void func_global_asm_805FC2B0(void);
void func_global_asm_80600B10(void);
void func_global_asm_8066AF40(void);
void func_global_asm_80610268(u32 arg0);
void func_global_asm_80600674(void); //calculate lag boost?
void func_global_asm_80658CCC(void);
void func_global_asm_80700BF4(void);
void raiseException(u8 arg0, s32 arg1, s32 arg2, s32 arg3);
void cFuncLoop(void);
void func_global_asm_80610044(void* arg0, s32 arg1, u8 arg2, u8 arg3, s32 arg4, u8 arg5);

typedef struct AnimationStateUnk0_0 {
    f32 unk0;
    u16 unk4;
    u8 pad6[0x12 - 0x6];
    u8 unk12;
} AnimationStateUnk0_0;

// TODO: What is this actually?
typedef struct animation_state_unk0 {
    AnimationStateUnk0_0* unk0;
    f32 unk4; // Used
    f32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10; // Used
    s16 unk12;
    f32 unk14;
    f32 unk18;
    s16 unk1C;
    s16 unk1E;
    s16 unk20;
    s16 unk22;
    f32 unk24; // Used
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
} AnimationStateUnk0;

typedef struct {
    s8 unk0;
    s8 unk1;
    union {
        s16 unk2;
        u16 unk2_u16; // used in K Rool Diddy code
    };
    s16 unk4;
    union {
        s16 unk6;
        u16 unk6_u16;
    };
    // Note: This is correctly sized, if you get references to unk8, use unk1C[1].unk0 instead
} AnimationStateUnk1C;

// TODO: Fill this in properly
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    f32 unk4;
    f32 unk8;
    f32 unkC;
} AnimationStateUnk24;

// See boss func_dk64_boot_8002FB7C
typedef struct animation_state_unk20 {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    f32 unk4;
    f32 unk8;
    f32 unkC;
} AnimationStateUnk20;

typedef struct actor_collision ActorCollision;
typedef struct actor Actor;

typedef struct ActorAnimationState_unk170 {
    u8 pad0[0x80];
} ActorAnimationState_unk170;

typedef struct actor_animation_state {
    AnimationStateUnk0* unk0;
    AnimationStateUnk0* unk4;
    AnimationStateUnk0* unk8;
    AnimationStateUnk0* unkC;
    f32 unk10;
    Mtx* bone_arrays[2]; // at 0x14, camera, update bone positions // TODO: Proper type
    AnimationStateUnk1C* unk1C;
    AnimationStateUnk20* unk20; // See boss func_dk64_boot_8002FB7C
    AnimationStateUnk24* unk24;
    s16 unk28;
    s16 unk2A;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    s16 unk32;
    union {
        struct {
            f32 scale_x; // at 0x34
            f32 scale_y; // at 0x38
            f32 scale_z; // at 0x3C
        };
        f32 scale[3];
    };
    f32 unk40;
    f32 unk44;
    f32 unk48;
    f32 unk4C;
    f32 unk50;
    f32 unk54;
    f32 unk58;
    u16 unk5C;
    s16 unk5E;
    s32 unk60;
    u16 unk64; // Used
    u16 unk66;
    s32 unk68;
    s32 unk6C;
    s32(*unk70)(Actor*);
    s32 unk74;
    s32 unk78;
    s32 unk7C;
    f32 unk80;
    s32 unk84;
    s32 unk88;
    f32 unk8C;
    AnimationStateUnk0 unk90[4];
    ActorAnimationState_unk170 unk170[1];
} ActorAnimationState;

typedef struct SnideAaD180 {
    u8 control_byte;
    u8 screen;
    u8 turned_in_count;
    u8 pad[0x3];
    u8 unk6;
    u8 unk7;
    s32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    s8 previous_y;
    s8 minigame_menu_unlocked;
    u8 unturned_count;
    u8 pad2[0x5];
} SnideAaD180;

typedef struct otherSpriteControl otherSpriteControl;

typedef struct player_additional_actor_data {
    s16 unk0; // Used
    s16 unk2;
    f32 unk4;
    s16 unk8; // Used
    s16 unkA; // Used, Actor->y_rotation copied here
    s16 unkC; // Used
    s16 unkE; // Used
    s16 unk10; // Used
    s16 unk12; // Used
    s16 unk14; // Used
    union {
        struct {
            s8  unk16_s8;
            s8  unk17; // Used, set to current file selection?
        };
        s16 unk16;
    };
    s16 unk18; // Used
    u16 unk1A;
    f32 unk1C; // Used
    f32 unk20; // Used
    s16 unk24; // Used
    s16 unk26;
    s16 unk28; // Used
    s16 unk2A;
    f32 unk2C; // Used
    f32 unk30; // Used
    f32 unk34; // Used
    f32 unk38; // Used
    s16 unk3C;
    s16 unk3E; // Used
    s16 unk40;
    u8 unk42; // Used, related to distance from floor
    u8 unk43;
    u8 unk44;
    u8 unk45; // Used, VehicleAdditionalActorData
    u8 unk46;
    u8 unk47;
    s16 unk48; // Used
    s16 unk4A;
    s16 unk4C; // Used
    s16 unk4E;
    s16 unk50; // Used
    s16 unk52;
    f32 unk54; // Used, y velocity?
    s32 unk58; // Used, related to D_global_asm_80767CC0
    s32 unk5C;
    s32 unk60;
    s16 unk64; // Used
    s16 unk66;
    f32 unk68;
    f32 unk6C; // Used
    s16 unk70; // Used, y rotation?
    s16 unk72;
    f32 unk74;
    f32 unk78; // used
    f32 unk7C; // used
    s16 unk80; // used
    s16 unk82;
    s32 unk84;
    Actor* vehicle_actor_pointer; // Used
    u8  unk8C; // Used
    u8  unk8D; // Used
    u8 unk8E; // Used
    u8 unk8F;
    u8 unk90;
    u8 unk91; // Used
    u8 unk92;
    u8 unk93;
    f32 unk94; // Used
    f32 unk98;
    f32 unk9C; // Used
    f32 unkA0; // Used
    f32 unkA4; // Used
    f32 unkA8; // Used
    union {
        s32 unkAC; // Used
        f32 unkAC_f32; // Used
    }; // TODO: Deconflict
    union {
        struct {
            s16 unkB0;
            s16 unkB2; // Used
        };
        f32 unkB0_f32;
    }; // TODO: Deconflict
    f32 unkB4; // Used
    f32 unkB8; // Used
    s16 unkBC; // Used
    s16 unkBE;
    s16 unkC0;
    s16 unkC2; // Used
    s32 unkC4;
    s16 unkC8;
    u8  unkCA; // Used
    s8  unkCB;
    s16 unkCC; // Used
    s16 unkCE;
    s32 unkD0;
    s16 unkD4; // Used
    s16 unkD6;
    u8 unkD8; // Used
    u8 unkD9;
    u8 unkDA;
    u8 unkDB;
    Actor* unkDC; // Used
    f32 unkE0; // Used
    f32 unkE4; // Used
    u8 unkE8; // Used
    u8 unkE9;
    s16 unkEA; // Used
    s16 unkEC;
    u8 unkEE;
    u8 unkEF; // Used
    union { // TODO: No idea what's going on here, both are used
        float unkF0_f32;
        u8 unkF0;
        u8 unkF0_u8[4];
    };
    union { // TODO: No idea what's going on here, both are used
        float unkF4_f32;
        u8 unkF4;
        u8 unkF4_u8[4];
    };
    u8 unkF8; // Used
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    union { // TODO: Deconflict
        u8 unkFC; // Used
        s32 unkFC_s32; // Used
    };
    s32 unk100;
    Actor* unk104; // Used, Actor*?
    f32 unk108; // Used
    s8 unk10C; // Used
    u8 unk10D;
    s16 unk10E;
    s16 unk110;
    s16 unk112;
    s32 unk114;
    s32 unk118;
    u8 unk11C; // Used, VehicleAdditionalActorData?
    u8 unk11D;
    s16 unk11E; // Used
    s32 unk120;
    f32 unk124; // Used
    s32 unk128;
    s32 unk12C;
    Actor* unk130;
    s8 unk134;
    s8 unk135;
    s8 unk136;
    s8 unk137;
    f32 unk138;
    f32 unk13C;
    f32 unk140;
    f32 unk144;
    f32 unk148;
    f32 unk14C; // Used
    f32 unk150; // Used
    s16 unk154; // Used
    s16 unk156;
    Actor* unk158;
    s32 unk15C;
    s32 unk160;
    s32 unk164;
    s32 unk168;
    s32 unk16C;
    s32 unk170;
    s32 unk174;
    s32 unk178;
    s32 unk17C;
    s32 unk180;
    s32 unk184;
    s32 unk188;
    s32 unk18C;
    s32 unk190;
    s32 unk194;
    s32 unk198;
    s16 unk19C; // Used
    s16 unk19E; // Used
    u16 unk1A0; // Used
    u8  unk1A2;
    u8  unk1A3; // Used
    u8  unk1A4; // Used, playerIndex?
    u8  unk1A5;
    u8  unk1A6;
    u8  unk1A7;
    Actor* unk1A8;
    Actor* unk1AC; // TODO: Is this correct?
    Actor* unk1B0; // Used
    s32 unk1B4;
    u16 unk1B8; // Used
    s16 unk1BA;
    f32 unk1BC; // Used
    union {
        struct {
            f32 unk1C0; // Used, x scale
            f32 unk1C4; // Used, y scale
            f32 unk1C8; // Used, z scale
        };
        f32 scale1C0[3];
    };
    union {
        struct {
            f32 unk1CC; // Used, x scale
            f32 unk1D0; // Used, y scale
            f32 unk1D4; // Used, z scale
        };
        f32 scale1CC[3];
    };
    f32 unk1D8; // Used
    f32 unk1DC; // Used
    f32 unk1E0; // Used
    s32 unk1E4;
    f32 unk1E8;
    u8 unk1EC;
    u8 unk1ED;
    s16 unk1EE; // Used
    s32 unk1F0; // Used, bitfield // TODO: Proper bitfield syntax
    s32 unk1F4;
    s32 unk1F8;
    u8 unk1FC; // Used
    u8 unk1FD;
    u16 unk1FE; // Used
    s16 unk200; // Used
    s16 unk202;
    f32 unk204;
    f32 unk208;
    f32 unk20C;
    f32 unk210; // Used, x
    f32 unk214; // Used, y
    f32 unk218; // Used, z
    s16 unk21C;
    u16 unk21E; // Used
    f32 unk220; // Used, x
    f32 unk224; // Used, y
    f32 unk228; // Used, z
    f32 unk22C; // Used, x
    f32 unk230; // Used, y
    f32 unk234; // Used, z
    f32 unk238; // Used
    u16 unk23C; // Used
    u8 unk23E;
    u8 unk23F; // Used
    u8 unk240; // Used
    u8 unk241;
    u16 unk242; // Used, map?
    u8 unk244;
    u8 unk245; // Used
    s8 unk246; // Used func_global_asm_806CA26C
    s8 unk247; // Used func_global_asm_806CA26C
    s8 unk248;
    s8 unk249;
    u8 unk24A;
    s8 unk24B; // Used
    u16 unk24C; // Used // TODO: s16? func_global_asm_806CA1B4 uses this as u16
    u16 unk24E; // Used // TODO: s16? func_global_asm_806CA1B4 uses this as u16
    s32 unk250;
    otherSpriteControl* unk254;
    f32 unk258; // Used
    f32 unk25C; // Used
    void* unk260; // Used (multiplayer)
    s32 unk264; // Used
    u8 unk268; // Used
} PlayerAdditionalActorData;

//f32 at unk8 doesn't match with what's in structs.h (s16) 
//for PlayerAdditionalActorData so making this struct
typedef struct {
    u8 pad0[0x8];
    f32 unk8;
} PaaD0;

// Bonus func_bonus_800253E4
typedef struct {
    Actor* unk0;
} BaaD2;

typedef struct TempAAD {
    s8 unk0;
    u8 pad0[0x14 - 0x1];
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    u8 pad1[0x24 - 0x1C];
    s8 unk24;
} TempAAD;

typedef struct struct806A57C0_2 Struct806A57C0_2;
typedef struct struct806A57C0_3 Struct806A57C0_3;

typedef struct struct806A57C0_3_sub10 {
    s16 unk0;
    s16 unk2;
    void* unk4;
    Mtx unk8[2];
    f32 unk88;
} struct806A57C0_3_sub10;

struct struct806A57C0_3 {
    s16 unk0;
    u8 unk2; // Used
    u8 unk3;
    f32 unk4; // Used
    f32 unk8; // Used
    s32 unkC;
    struct806A57C0_3_sub10 unk10;
    Struct806A57C0_3* unkA0; // Used
};

struct struct806A57C0_2 {
    u8 unk0; // Used
    u8 unk1;
    u8 unk2;
    u8 unk3;
    s32 unk4;
    s32 unk8;
    Struct806A57C0_3* unkC;
    u16 unk10; // Used
    u16 unk12;
    s32 unk14;
    f32 unk18;
    Struct806A57C0_2* next; // 0x1C
};

typedef struct {
    u16 unk0; // Used
    s16 unk2;
    s32 unk4;
    s32 unk8;
    Struct806A57C0_2* unkC; // Used
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    f32 unk14; // Used
    f32 unk18; // Used
    u8 unk1C;
    u8 unk1D; // Used
    u8 unk1E;
    u8 unk1F;
    f32 unk20; // Used
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    void* unk34; // Used
    s16 unk38;
    s16 unk3A;
    s32 unk3C;
    f32 unk40; // Used
    f32 unk44; // Used
    f32 unk48; // Used
    f32 unk4C; // Used
    f32 unk50;
    u8 unk54;
    u8 unk55; // Used
} AAD_global_asm_806A4DDC;

// TODO: This appears to be a "texture renderer" according to ScriptHawk, needs a better name though. Linked list at Actor->unk158
typedef struct global_asm_struct_60 GlobalASMStruct60;

struct global_asm_struct_60 {
    s32* unk0; // Used
    GlobalASMStruct60* unk4; // Used? Might be wrong datatype, see func_global_asm_80688638
    u8 unk8; // Used
    u8 unk9;
    s16 unkA; // Used
    s16 unkC; // Used - Texture index?
    s16 unkE;
    f32 unk10; // Used
    f32 unk14; // Used
    s16 unk18; // Used
    s16 unk1A; // Used
    s16 unk1C; // Used
    s16 unk1E; // Used
    u8 unk20; // Used
    u8 unk21; // Used
    u8 unk22; // Used
    u8 unk23;
    GlobalASMStruct60* next; // at 0x24
};

typedef struct {
    s32 unk0; // TODO: Proper members
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
} FloorTriangle;

typedef struct {
    Actor* unk0;
    Actor* unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    s16 unk18;
} CameraPaad_unk44;

typedef struct {
    Actor* unk0;
    u8 pad4[0x20 - 0x4];
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    u8 pad38[0x44 - 0X38];
    CameraPaad_unk44* unk44;
    s32 unk48;
    u8 pad4C[0x90 - 0x4C];
    f32 unk90;
    u8 pad94[0xAC - 0x94];
    s32 unkAC;
    u8 padB0[0xF3 - 0xB0];
    u8 unkF3;
    u8 padF4[0xFA - 0xF4];
    u8 unkFA;
    u8 unkFB;
} CameraPaad;

typedef struct {
    // Small header on top of DisplayList Pointer
    u8 unk0[0x1C - 0x0];
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 unk21[0x38 - 0x21];
    f32 unk38;
} Actor_unk0;

typedef struct KRoolChunkyAAD178 {
    Actor* unk0;
    u8 pad4[0x14 - 0x4];
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
} KRoolChunkyAAD178;

typedef struct {
    Actor* unk0[4]; // Used
    u8 unk10; // Used, index into unk0
    s8 unk11;
    s8 unk12;
    s8 unk13;
} Actor114;

typedef struct {
    s32 unk0; // TODO: Proper members
    s32 unk4;
    f32 unk8;
    u8 unkC;
    s8 unkD;
    s16 unkE;
    s16 unk10;
    s16 unk12;
} Actor118;

typedef struct {
    union {
        f32 unk0; // Used
        s32 unk0_s32; // Used // TODO: We might have another aaD situation here...
    };
    union {
        f32 unk4; // Used
        s32 unk4_s32; // Used // TODO: We might have another aaD situation here...
    };
    union {
        f32 unk8; // Used
        s32 unk8_s32; // Used // TODO: We might have another aaD situation here...
    };
    union {
        f32 unkC; // Used
        s32 unkC_s32; // Used // TODO: We might have another aaD situation here...
    };
    f32 unk10; // Used
    union {
        f32 unk14; // Used
        s32 unk14_s32; // Used // TODO: We might have another aaD situation here...
    };
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
    s32 unk40;
    s32 unk44;
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
} Actor124;

typedef struct ledge_info_8c LedgeInfo8C;

struct ledge_info_8c {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC; // Used
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s16 unk24;
    u8 unk26; // Used
    u8 unk27; // Used
    LedgeInfo8C* next;
};

typedef struct ledge_info_90 LedgeInfo90;

struct ledge_info_90 {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    LedgeInfo90* next;
};

typedef struct {
    f32 unk0; // Used
    s32 unk4;
    s32 unk8;
    f32 unkC; // Used
    s32 unk10;
    s32 unk14;
    f32 unk18; // Used
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s16 unk54;
    s8 unk56;
    s8 unk57;
} LedgeInfo98;

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
    s16 unk28;
    s8 unk2A; // Used
    s8 unk2B;
} LedgeInfoA0;

typedef struct LedgeInfo74 {
    Mtx unk0[];
} LedgeInfo74;

typedef struct ledge_info {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    f32 unk20;
    u8 unk24;
    u8 unk25;
    u8 unk26;
    u8 unk27;
    u32 unk28;
    u32 unk2C;
    u32 unk30;
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u32 unk40;
    u32 unk44;
    u16 unk48;
    u16 unk4A;
    u32 unk4C;
    u32 unk50;
    u32 unk54;
    u32 unk58;
    u32 unk5C;
    u32 unk60;
    u16 unk64;
    u16 unk66; // Used
    u32 unk68;
    u8 unk6C;
    u8 unk6D;
    u8 unk6E;
    u8 unk6F;
    Mtx* unk70;
    Mtx* unk74[2];
    s32 unk7C;
    s32 unk80; // object_timer gets written here
    s32 unk84;
    s32 unk88; // object_timer gets written here
    LedgeInfo8C* unk8C; // Linked List
    LedgeInfo90* unk90;
    s32 unk94;
    LedgeInfo98* unk98; // Array
    s32 unk9C;
    LedgeInfoA0* unkA0; // Array
    s32 unkA4;
    s32 unkA8;
    u8 unkAC;
} LedgeInfo;

typedef struct other_additional_actor_data {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    s16 unkA;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    s16 unk18;
    s16 unk1A;
    s32 unk1C;
} OtherAdditionalActorData;

typedef struct another_additional_actor_data {
    s32 unk0;
    s32 unk4;
    f32 unk8;
    s32 unkC;
    s16 unk10;
    u8 unk12;
    u8 unk13; // Used in func_global_asm_806925AC
} AnotherAdditionalActorData;

typedef struct {
    u8 unk0[0x27 - 0x0];
    u8 unk27;
    u8 unk28[0x36 - 0x28];
    u8 unk36;
} RaaD_unk20_unk4;

typedef struct {
    Actor* unk0;
    RaaD_unk20_unk4* unk4;
} RaaD_unk20;

typedef struct race_additional_actor_data {
    s16 unk0;
    s16 unk2; // used
    f32 unk4;
    s16 unk8;
    s16 unkA;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    s16 unk18;
    s16 unk1A;
    u16 unk1C;
    u8 unk1E;
    s8 unk1F;
    RaaD_unk20* unk20; // Array
    u8  unk24;
    u8  unk25;
    u8  unk26; // Used
    s8  unk27;
    u8 unk28; // Used
    s8 unk29; // Used
    s16 unk2A;
    s32 unk2C;
    Actor* unk30;
    u8 unk34;
    u8 unk35;
    u8 unk36;
    u8 unk37;
    s32 unk38;
    u8 unk3C;
    u8 unk3D;
    u8 unk3E;
    u8 unk3F;
    s16 unk40;
    u8 unk42; // Used
    u8 unk43;
    u8 unk44;
    u8 unk45; // Used
    u8 pad46[2];
    s16* unk48;
    u8 pad4C[4];
    Mtx unk50[2];
} RaceAdditionalActorData;

typedef struct race_additional_actor_data2 {
    u8  unk0; // Used
    u8  unk1;
    u8  unk2;
    u8  unk3;
    u8 pad2[0x24 - 0x4];
    u8 unk24;
    u8 pad25[0x36 - 0x25];
    u8 unk36; // Used
} RaceAdditionalActorData2;

typedef struct yaad5 {
    f32 unk0; // Used
    f32 unk4; // Used
    f32 unk8; // Used
    Actor* unkC; // Used
    Actor* unk10; // Used
    s16 unk14; // Used
    s16 unk16; // Used
    u16 unk18;
    s16 unk1A; // Used
    s16 unk1C; // Used
    s8 unk1E; // Used
} YetAnotherAdditionalActorData5;

typedef struct yet_another_additional_actor_data {
    Actor* unk0;
    u16 unk4;
    u16 unk6;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s16 unk20;
    s16 unk22;
    f32 unk24;
    s16 unk28;
    s16 unk2A;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s16 unk38;
    s16 unk3A;
    s32 unk3C;
} YetAnotherAdditionalActorData;

typedef struct yaad4 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s16 unk10; // Used
    s16 unk12; // Used
    s16 unk14; // Used
} YetAnotherAdditionalActorData4;

typedef struct {
    u8 unk0;
    u8 unk1; // Used
} A178_806A18A8;

typedef struct actor_17C {
    union {
        s32 unk0; // TODO: Actor*? Boss overlay Might require a union?
        s16 unk0_s16[2]; // TODO: Actor*? Boss overlay Might require a union?
        u16 unk0_u16[2]; // TODO: Actor*? Boss overlay Might require a union?
    };
    u8 unk4;
    u8 unk5;
    s16 unk6;
    s16 unk8;
} Actor17C;

typedef struct {
    u8 unk0;
    u8 unk1;
} Struct80027840;

typedef struct race_actor_178 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    f32 unk14;
} RaceActor178;

typedef struct boss_actor_178 {
    s32 unk0; // Used
} BossActor178;

typedef struct actor_178 {
    s16 unk0; // Used
    s16 unk2;
    u8 unk4;
    u8 unk5;
    s16 unk6;
    s16 unk8;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u16 unk10; // Used
    u16 unk12; // Used
    u8 unk14;
} Actor178;

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

struct actor {
    Actor_unk0* unk0;
    ActorAnimationState* animation_state;
    u32* unk8; // Current bone array Pointer // TODO: Proper type
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
    Actors unk58;
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
    Actor114* unk114;
    Actor118* unk118;
    Actor* unk11C;
    f32 unk120;
    Actor124* unk124;
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
    ActorCollision* collision_queue_pointer; // at 0x13C
    LedgeInfo* ledge_info_pointer; // at 0x140
    u8 noclip_byte; // at 0x144
    u8 unk145;
    union {
        u16 unk146; // used (0x147 hand state? 0x146 seems to be u16)
        s16 unk146_s16; // used func_global_asm_8068A764
    };
    void* unk148; // Used
    void* unk14C; // Used
    FloorTriangle* unk150;
    u8 control_state; // at 0x154
    u8 control_state_progress; // at 0x155
    u8 unk156;
    u8 unk157;
    GlobalASMStruct60* unk158; // Texture renderer linked list
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
    union {
        void* AAD_as_array[4];
        struct {
            union {
                void* additional_actor_data; // For when you don't know which one it's using
                OtherAdditionalActorData* OaaD;
                AnotherAdditionalActorData* AaaD;
                RaceAdditionalActorData* RaaD;
                RaceAdditionalActorData2* RaaD2;
                YetAnotherAdditionalActorData* YaaD;
                YetAnotherAdditionalActorData4* YaaD4;
                YetAnotherAdditionalActorData5* YaaD5;
                MenuAdditionalActorData* MaaD;
                PlayerAdditionalActorData* PaaD;
                CameraPaad* CaaD;
                PaaD0* PaaD0; // TODO: Figure this out
                BaaD2* BaaD2; // TODO: Figure this out
                TempAAD* TaaD; // TODO: Figure this out (race overlay)
                void* additional_data_pointer; //legacy
            };
            union {
                void* unk178;
                Actor178* a178;
                RaceActor178* race178;
                BossActor178* boss178;
                A178_806A18A8* a178_806A18A8;
            };
            union {
                Actor17C* unk17C;
            };
            union {
                void* unk180;
                SnideAaD180* snide_aad_180;
            };
        };
    };
    void* unk184;
};

typedef struct enemy_info {
    u8 enemy_type; // at 0x00
    u8 unk1;
    u16 y_rotation; // at 0x02
    s16 x_position; // at 0x04
    s16 y_position; // at 0x06
    s16 z_position; // at 0x08
    u8 cutscene_model_index; // at 0x0A
    u8 unkB;
    u32 unkC;
    u16 unk10;
    u8 unk12;
    u8 unk13;
    u32 unk14;
    Actor* unk18; // TODO: Is this accurate?
    u32 unk1C;
    Struct80027840* unk20;
    u16 unk24;
    u16 unk26;
    u16 unk28; // Used
    s16 unk2A; // Used
    u32 unk2C;
    f32 unk30; // Used
    f32 unk34; // Used
    f32 unk38; // Used
    f32 unk3C; // at 0x3C
    s16 unk40; // Used
    s16 unk42;
    u8 unk44; // Used
    u8 unk45;
    u16 unk46; // Used
} EnemyInfo;

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
    Actor* playerPointer;    // 0x04
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
    Actor* unk2A0;
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

typedef struct {
    s16     button;
    s8      stick_x;        /* -80 <= stick_x <= 80 */
    s8      stick_y;        /* -80 <= stick_y <= 80 */
} DKTV_OSContPad;

extern s32 D_global_asm_80755338; // cutscene_will_play_next_map
extern s32 D_global_asm_8075533C; // cutscene_to_play_next_map
extern Vtx D_global_asm_80754C48[2][4];
extern u16 *D_global_asm_807ECE94;
extern DKTV_OSContPad *D_global_asm_807ECE98;
extern u8 is_autowalking;
extern u16 D_global_asm_8075531C; // Demo Fadeout Timer
extern Gfx *func_global_asm_805FD030(Gfx *dl);

extern Gfx **D_1000118;
extern Mtx D_2000000;
extern Mtx D_2000080;
extern Mtx D_20000C0;
extern Mtx D_2000180;
extern Mtx D_2000200;

extern u8 D_global_asm_807FDB0F; // alpha
extern s32 D_global_asm_807FDB10;
extern void *D_global_asm_807FDB14; // TODO: Function pointer
extern u8 D_global_asm_807FDB18;
extern s16 D_global_asm_807FDB1A;
extern u8 D_global_asm_807FDB1C;
extern u8 D_global_asm_807FDB1D;
extern u8 D_global_asm_807FDB28;
extern s32 D_global_asm_807FDB2C;
extern s32 D_global_asm_807FDB30;
extern u16 D_global_asm_807FDB36;
extern s16 D_global_asm_807FDB38;
extern s16 D_global_asm_807FDB3A;
extern s16 D_global_asm_807FDB3C;
extern s16 D_global_asm_807FDB3E;
extern s16 D_global_asm_807FDB40;
extern s16 D_global_asm_807FDB42;
extern s16 D_global_asm_807F7358;
extern s16 D_global_asm_807F735A;
extern s16 D_global_asm_807F735C;
extern s16 D_global_asm_807F735E;
extern s16 D_global_asm_807FDB3C;
extern s16 D_global_asm_807FDB3E;
extern s16 D_global_asm_807FDB40;
extern s16 D_global_asm_807FDB42;

extern u8 cc_number_of_players;
extern Gfx *func_global_asm_805FCFD8(Gfx *dl);
extern void func_global_asm_8061134C(void *);
extern u8 isFlagSet(s16 flagIndex, u8 flagType);

extern void *getPointerTableFile(enum pointertable_e pointerTableIndex, s32 fileIndex, u8 arg2, u8 arg3);
extern void func_global_asm_8066B434(void *arg0, s32 arg1, s32 arg2);
extern f32 D_global_asm_80750228;
extern f32 D_global_asm_8075022C;

extern void func_global_asm_80658E58(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
extern s32 func_global_asm_80626F8C(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, s32 arg5, f32 arg6, s32 arg7);