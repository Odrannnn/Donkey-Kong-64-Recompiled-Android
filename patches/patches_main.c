#include "patches.h"
#include "PR/os_message.h"
#include "maps.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "misc_funcs.h"

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

extern void osWritebackDCache(void*, s32);
extern u32  osPiGetStatus(void);
extern void osInvalDCache(void*, s32);

typedef u32 OSIntMask;

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

void __osSpSetStatus(u32);
void osDpSetStatus(u32);
extern void osViBlack(u8);
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

RECOMP_PATCH void func_dk64_boot_8000102C(s32 offset, s32 size, void* dramAddr) {
    while (size & 0xf)
    {
        size++;
    }

    //@recomp: load overlays
    recomp_load_overlays(offset, dramAddr, size);

    osWritebackDCache(dramAddr, size);

    //@recomp: patch to call osPiRawStartDma variant
    //osPiRawStartDma(OS_READ, gOverlayTable[11].rom_code_start + offset, dramAddr, size);
    boot_osPiRawStartDma(OS_READ, gOverlayTable[11].rom_code_start + offset, dramAddr, size);

    do {} while (osPiGetStatus() & PI_STATUS_DMA_BUSY);
    osInvalDCache(dramAddr, size);
}

RECOMP_PATCH void func_dk64_boot_80000450(s32 devAddr, s32 arg1, void* dramAddr) {
    u32 size = arg1 - devAddr;

    //@recomp: load and map compressed address to uncompressed address
    load_dk64_overlay(devAddr, dramAddr, arg1 - devAddr);

    osInvalDCache(dramAddr, size);

    //@recomp: dma uncompressed data
    boot_osPiRawStartDma(OS_READ, devAddr, dramAddr, arg1 - devAddr);
    //osPiRawStartDma(OS_READ, devAddr, dramAddr, size);

    do {} while (osPiGetStatus() & PI_STATUS_DMA_BUSY);
}


RECOMP_PATCH void func_global_asm_80611730(void) {
    s32 corrupted = 0;
    s32 count;
    Unk807F0A58Entry* entry;

    func_global_asm_80611724(0x3791DFFF, 0x4BFFD668);
    //@recomp: patch to just remove this probable anti piracy check (it loads from uncached memory originally)
    //if (~0x3791DFFF != *(s32*)UNK_ADDR) {
    //    corrupted = 1;
    //}

    count = D_global_asm_807F5A58;
    if (count <= 0) {
        return;
    }

    entry = &D_global_asm_807F0A58[0];
    do {
        entry->unk4--;
        if (entry->unk4 == 0 && !corrupted) {
            func_global_asm_80611408(entry->unk0);
            count = --D_global_asm_807F5A58;
            *entry = D_global_asm_807F0A58[count];
        }
        else {
            entry++;
        }
    } while (entry < &D_global_asm_807F0A58[count]);
}


RECOMP_PATCH void func_global_asm_80600674(void) {
    s32 max_boost = 1;
    s32 min_boost = 20;
    s32 newBoost;
    s32 pad;
    s32 cap;
    s32 idx;
    s32 updateLagBoost;
    u32 oldBoost;
    s32 i;
    Struct80767A40* osdata;

    //@recomp: patch to always greater than 1 (on console, default is 2. if zero, it will divide by zero and crash)
    if (D_global_asm_80744478 <= 1) {
        D_global_asm_80744478 = 2;
    }

    if (D_global_asm_8076AF14) {
        osdata = &D_global_asm_80767A40;
        newBoost = osdata->frame_count - D_global_asm_8076AF10;
        newBoost = MAX(1, newBoost);
        D_global_asm_8076AF00[D_global_asm_80745290++] = newBoost;
        updateLagBoost = FALSE;
        if (D_global_asm_80745290 == 8) {
            D_global_asm_80745290 = 0;
        }
        oldBoost = D_global_asm_80744478;
        if (oldBoost >= 4) {
            cap = 1;
        }
        else if (oldBoost < newBoost) {
            cap = 2;
        }
        else {
            cap = 4;
        }
        idx = D_global_asm_80745290;
        for (i = 0; i < cap; i++) {
            idx--;
            if (idx < 0) {
                idx = 7;
            }
            max_boost = MAX(max_boost, D_global_asm_8076AF00[idx]);
            min_boost = MIN(min_boost, D_global_asm_8076AF00[idx]);
        }
        if ((oldBoost < newBoost) && (oldBoost < min_boost)) {
            updateLagBoost = TRUE;
        }
        else if ((newBoost < oldBoost) && (max_boost < oldBoost)) {
            updateLagBoost = TRUE;
        }
        if (updateLagBoost) {
            //@recomp: dont update; stays at 2
            //D_global_asm_80744478 = newBoost;
        }
        if (object_timer > 10) {
            while (D_global_asm_8076AF10 + D_global_asm_80744478 > osdata->frame_count) {
                //@recomp: yield so this progresses correctly
                yield_self();
            }
        }
        D_global_asm_8076AF10 = osdata->frame_count;
        return;
    }
    osdata = &D_global_asm_80767A40;

    //@recomp: dont update; stays at 2
    //D_global_asm_80744478 = osdata->frame_count - D_global_asm_8076AF10;

    D_global_asm_8076AF10 = osdata->frame_count;
    //recomp_printf("D_global_asm_80744478 is %d:\n", D_global_asm_80744478);
}


RECOMP_PATCH void func_dk64_boot_800009D0(void) {
    u32* tmp_a0;
    osInitialize();
    tmp_a0 = (void*)0x802FE1C0; //@recomp: patch to cached read
    while (0xAD170014 != *tmp_a0);
    *tmp_a0 = 0xF0F0F0F0;
    func_dk64_boot_80000980();
}

RECOMP_PATCH void func_global_asm_805FB5C4(OSMesgQueue* arg0, s32 arg1) {
    OSTime target_time;
    Struct80744464 sp34;
    OSTime buffer_time;
    u8 buffer[1];
    void* sp20;
    u8 buffer2[5];
    static OSTime D_global_asm_807655E8;
    

    sp34 = D_global_asm_80744464;
    if (arg1 == 2) {
        osViBlack(1);
        func_global_asm_80601CF0(1);
        D_global_asm_80744460 = 1;
        while (TRUE) {}
    }
    osRecvMesg(arg0, &sp20, 1);
    D_global_asm_80744460 = 1;
    func_global_asm_80601CF0(1);
    osStopThread(&D_global_asm_80761430); //this was previously patched to a osDestoryThread; is that neccessary?
    osSetThreadPri(NULL, 0xB);
    D_global_asm_807655E8 = osGetTime();
    while (osGetTime() < D_global_asm_807655E8 + BUFFER_TIME);
    osViBlack(1);

    //@recomp:patch don't call these because?

    func_global_asm_8060E930();
    while (TRUE) {}
}
