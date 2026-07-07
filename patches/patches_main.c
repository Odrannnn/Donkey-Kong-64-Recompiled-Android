#include "patches_main.h"

#define gScissorUpLX D_global_asm_80744498
#define gScissorUpLY D_global_asm_8074449C
#define gScissorLowerRightX D_global_asm_807444A0
#define gScissorLowerRightY D_global_asm_807444A4

#define gScissor2LowerRightX D_global_asm_80744490
#define gScissor2LowerRightY D_global_asm_80744494

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
    AlterVolumes();

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

    //@recomp: patch to cached read
    tmp_a0 = (void*)0x802FE1C0; 

    while (0xAD170014 != *tmp_a0);
    *tmp_a0 = 0xF0F0F0F0;
    func_dk64_boot_80000980();
}

//RECOMP_PATCH void func_global_asm_805FB5C4(OSMesgQueue* arg0, s32 arg1) {
//    OSTime target_time;
//    Struct80744464 sp34;
//    OSTime buffer_time;
//    u8 buffer[1];
//    void* sp20;
//    u8 buffer2[5];
//    static OSTime D_global_asm_807655E8;
//
//
//    sp34 = D_global_asm_80744464;
//    if (arg1 == 2) {
//        osViBlack(1);
//        func_global_asm_80601CF0(1);
//        D_global_asm_80744460 = 1;
//        while (TRUE) {}
//    }
//    osRecvMesg(arg0, &sp20, 1);
//    D_global_asm_80744460 = 1;
//    func_global_asm_80601CF0(1);
//    osStopThread(&D_global_asm_80761430); //this was previously patched to a osDestoryThread; is that neccessary?
//    osSetThreadPri(NULL, 0xB);
//    D_global_asm_807655E8 = osGetTime();
//    while (osGetTime() < D_global_asm_807655E8 + BUFFER_TIME);
//    osViBlack(1);
//
//    //@recomp:patch don't call these because?
//    __osSpSetStatus(0xAAAA82);
//    osDpSetStatus(0x1D6);
//
//    func_global_asm_8060E930();
//    while (TRUE) {}
//}

#define SCREEN_HEIGHT 240 // Normally 240
#define SCREEN_WIDTH 427 // Normally 320
#define OVERSCAN_SIZE 0  // Normally 10
#define CUTSCENE_BORDERING 0 // Normally 30

RECOMP_PATCH void func_global_asm_805FB944(u8 arg0) {
    u8 var_a1 = 1;
    s32 var_a2;

    var_a2 = 0;
    func_global_asm_806003EC(D_global_asm_8076A0AA);
    //@recomp Patch to always be 240p by setting D_global_asm_8074450C to 1
    if (current_map == MAP_NINTENDO_LOGO) {
        D_global_asm_8074450C = 1;
    }
    else {
        D_global_asm_8074450C = 1;
    }
    switch (is_cutscene_active) {
    case 3:
        var_a1 = 9;
    case 4:
        if (var_a1 == 1) {
            var_a1 = 0xA;
        }
        gScissorUpLX = 0;
        gScissorUpLY = 0;
        gScissorLowerRightX = (D_global_asm_8074450C * 320);
        gScissorLowerRightY = (D_global_asm_8074450C * 240);
        break;
    default:
        var_a2 = func_global_asm_8060042C(current_map);
        var_a1 = 1;
        if (D_global_asm_807FBB64 & 1) {
            var_a1 = 7;
        }
        else if (D_global_asm_807FBB64 & 0x1000) {
            var_a1 = 6;
        }
        else if (D_global_asm_807FBB64 & 0x104000) {
            var_a1 = 8;
        }
        else if (D_global_asm_807FBB64 & 0x80000) {
            var_a1 = 4;
        }
        else if (D_global_asm_807FBB64 & 0x2000) {
            var_a1 = 5;
        }
        else if (D_global_asm_807FBB64 & 0x04000000) {
            var_a1 = 3;
        }
        else if (D_global_asm_807FBB64 & 0x40000000) {
            var_a1 = 2;
        }
        gScissorUpLX = D_global_asm_8074450C * OVERSCAN_SIZE;
        gScissorUpLY = D_global_asm_8074450C * OVERSCAN_SIZE;
        gScissorLowerRightX = (D_global_asm_8074450C * (320 - OVERSCAN_SIZE));
        gScissorLowerRightY = (D_global_asm_8074450C * (240 - OVERSCAN_SIZE));
        break;
    }
    func_global_asm_80610350(arg0, var_a1, var_a2);
    if (D_global_asm_807445A4 == 0) {
        osViSetMode(&osViModeTable[D_global_asm_80744588[osTvType + osTvType + D_global_asm_8074450C - 1]]);
        if (D_global_asm_807445A0 == 0) {
            osViBlack(1U);
        }
        D_global_asm_80744510 = 0;
        D_global_asm_807445A0 = 0;
    }
    else {
        D_global_asm_80744510 = 1;
        D_global_asm_807445A0 = 1;
        D_global_asm_807445A4 = 0;
        func_global_asm_805FB7E4();
    }
    osViSetSpecialFeatures(VI_CTRL_TYPE_16 | VI_CTRL_SERRATE_ON);
    gScissor2LowerRightX = D_global_asm_8074450C * 320; //width
    gScissor2LowerRightY = D_global_asm_8074450C * 240; //height
    // Force cutscene borders to be on for the level intros where you have K. Rool's speech as text
    // TODO: Tie to the options
    s32 cutscene_border_size = CUTSCENE_BORDERING;
    if (current_map == MAP_HELM_LEVEL_INTROS_GAME_OVER) {
        if (D_global_asm_80755338 && ((D_global_asm_8075533C >= 15) && (D_global_asm_8075533C <= 22))) {
            // Is level intro
            cutscene_border_size = 40;
        }
    }
    D_global_asm_807444AC = gScissorUpLY + (D_global_asm_8074450C * cutscene_border_size);
    D_global_asm_807444B0 = gScissorLowerRightY - (D_global_asm_8074450C * cutscene_border_size);
    D_global_asm_807444A8 = gScissorUpLY;
    D_global_asm_807444B4 = gScissorLowerRightY;
}

RECOMP_PATCH void func_global_asm_805FBC5C(void) {
    UnkMQStruct* mq;
    D_global_asm_8076A084 = gOverlayTable[12].rom_data_end - gOverlayTable[12].rom_code_start;
    osCreateMesgQueue(&D_global_asm_807655F0.mq, &D_global_asm_807655F0.msgs[0], 0x32);
    osCreateMesgQueue(&D_global_asm_807656D0.mq, &D_global_asm_807656D0.msgs[0], 0xC0);
    func_global_asm_8060EC80(
        &D_global_asm_80767A40.queue,
        &D_global_asm_80767A40,
        0x19,
        D_global_asm_80744588[osTvType + osTvType], 1);
    osCreateMesgQueue(&D_global_asm_807659E8.mq, &D_global_asm_807659E8.msgs[0], 0x10);
    func_global_asm_8060ED6C(
        (void*) &D_global_asm_80767A40,
        (void*)&D_global_asm_80767CD8,
        (s32) &D_global_asm_807659E8, 1, 1);
    current_map = next_map;
    func_global_asm_805FB944(0);
    D_global_asm_8076A07C = 5;
    func_global_asm_8060FFF0();
    func_global_asm_8060A900();
    func_global_asm_80600D50();
    setIntroStoryPlaying(0);
    func_global_asm_8073239C();
    mq = (void*)&D_global_asm_8076A110;
    osCreateMesgQueue((void*)mq, &D_global_asm_8076A108, 2);
    //@recomp: patch this timer to be significantly faster; we dont need to wait long
    osSetTimer(&D_global_asm_8076A130, OS_USEC_TO_CYCLES(100000), 0, (void*)mq, mq->msgs[0]); //wait 0.1 seconds
    //@recomp move playsound to a bit later
    //playSound(0x23C, 0x7FFF, 63.0f, 1.0f, 0, 0);
}

extern s32 D_global_asm_807F5E64;
extern f32 D_global_asm_807F5FA8;
extern f32 D_global_asm_807F5FAC;
extern f32 D_global_asm_807F5FB0;
extern f32 D_global_asm_807F5FB4;
extern CharacterChange* character_change_array;
extern u8 cc_player_index; // index into character_change_array, current_character_index[]
extern f32 D_global_asm_807F5FE0;
extern f32 D_global_asm_807F5FDC;
extern f32 D_global_asm_807F5E68;
extern f32 D_global_asm_807F5E20[][3];
extern s32 D_global_asm_807F5DE4; // TODO: Actually a pointer to a struct (map model?)
extern s32 D_global_asm_807F5E60;
extern u8 D_global_asm_807F5FEC;
extern s32 D_global_asm_807F5FF0;
extern u8 D_global_asm_80750AB4;
extern void* D_global_asm_807F5DE8;
extern void* D_global_asm_807F5DEC;

f32 func_global_asm_80612D10(f32 arg0);
void func_global_asm_8062DB70(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5);
void func_global_asm_8062A944(f32 arg0, f32 arg1, f32 arg2);
void func_global_asm_8062AC68(void* arg0);
void func_global_asm_8062AD28(f32 arg0, f32 arg1, f32 arg2, void* arg3, f32* arg4);
void func_global_asm_8062D620(s32, s32, s32, f32, f32, f32, s32, s32, s32); //first and second arg here was s32, now void*
Gfx* func_global_asm_80722294(Gfx*, Actor*, s16);
void func_global_asm_8062C99C(CharacterChange250*, s32, s32, s32, s32);
Gfx* func_global_asm_8065919C(Gfx* dl);
Gfx* func_global_asm_8070835C(Gfx*, u8);
extern Mtx D_2000180;
Gfx* func_global_asm_8062CA70(Gfx* dl, s32 arg1, s32 arg2, f32 arg3, f32 arg4, f32 arg5, s32 arg6);

//RECOMP_PATCH Gfx* func_global_asm_8062C29C(Gfx* dl, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9) {
//    s32 pad[25];
//    Gfx* temp_s3;
//
//    temp_s3 = (void*) & character_change_array[cc_player_index].unk1F0[D_global_asm_807444FC];
//    D_global_asm_807F5E64 = 0;
//    D_global_asm_807F5FB4 = arg7;
//    D_global_asm_807F5FB0 = arg8;
//    D_global_asm_807F5FAC = arg9;
//    D_global_asm_807F5FA8 = D_global_asm_807F5FAC;
//    D_global_asm_807F5FE0 = func_global_asm_80612D10(arg7 * 0.017453292f);
//    D_global_asm_807F5FE0 = (((D_global_asm_807F5FE0 - D_global_asm_807F5FDC) * 3.0) + D_global_asm_807F5FDC);
//    func_global_asm_8062DB70(arg1, arg2, arg3, arg4, arg5, arg6);
//    func_global_asm_8062A944(D_global_asm_807F5FB4, D_global_asm_807F5FB0, D_global_asm_807F5FA8);
//    func_global_asm_8062AC68(&character_change_array[cc_player_index].unk8[D_global_asm_807444FC]);
//    func_global_asm_8062AD28(arg1, arg2, arg3, &D_global_asm_807F5E68, (void*) & D_global_asm_807F5E20);
//    func_global_asm_8062D620(D_global_asm_807F5DE4, D_global_asm_807F5E60, D_global_asm_807F5FF0, arg1, arg2, arg3, 0, !D_global_asm_807F5FEC, (D_global_asm_80750AB4 >= 2));
//    gSPSegment(dl++, 0x06, osVirtualToPhysical(D_global_asm_807F5DE8));
//    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
//    dl = func_global_asm_80722294(dl, character_change_array[cc_player_index].playerPointer, cc_player_index);
//    gSPSegment(dl++, 0x05, osVirtualToPhysical(temp_s3));
//    gDPSetHilite1Tile(temp_s3++, G_TX_RENDERTILE, &character_change_array[cc_player_index].unk1D0[D_global_asm_807444FC], 32, 32);
//    gSPEndDisplayList(temp_s3++);
//    func_global_asm_8062C99C(&character_change_array[cc_player_index].unk250[D_global_asm_807444FC], character_change_array[cc_player_index].unk270[0], character_change_array[cc_player_index].unk270[1], character_change_array[cc_player_index].unk270[2], character_change_array[cc_player_index].unk270[3]);
//    gSPPerspNormalize(dl++, character_change_array[cc_player_index].unk188);
//    gSPViewport(dl++, osVirtualToPhysical(&character_change_array[cc_player_index].unk250[D_global_asm_807444FC]));
//    //@recomp: change scissor to cover whole screen
//    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, 0, 320, 240);
//    gSPLookAt(dl++, osVirtualToPhysical(&character_change_array[cc_player_index].unk190[D_global_asm_807444FC]));
//    func_global_asm_80658E58(character_change_array[cc_player_index].unk270[0], character_change_array[cc_player_index].unk270[1], character_change_array[cc_player_index].unk270[2], character_change_array[cc_player_index].unk270[3]);
//    dl = func_global_asm_8065919C(dl);
//    dl = func_global_asm_8070835C(dl, cc_player_index);
//    gSPMatrix(dl++, osVirtualToPhysical(&character_change_array[cc_player_index].unk88[D_global_asm_807444FC]), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
//    gSPMatrix(dl++, osVirtualToPhysical(&character_change_array[cc_player_index].unk8[D_global_asm_807444FC]), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
//    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
//    dl = func_global_asm_8062CA70(dl, D_global_asm_807F5DE4, D_global_asm_807F5E60, arg1, arg2, arg3, global_properties_bitfield);
//    gDPPipeSync(dl++);
//    return dl;
//}

//@recomp: Seems to be used for the culling of many objects, including actors and props
RECOMP_PATCH void func_global_asm_80658E58(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    D_global_asm_807F7358 = -0x7FFF;
    D_global_asm_807F735A = -0x7FFF;
    D_global_asm_807F735C = 0x7FFF;
    D_global_asm_807F735E = 0x7FFF;
}

//@recomp: Sprite culling. Also used for the scissor of the sprite itself
RECOMP_PATCH void func_global_asm_80714A68(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    D_global_asm_807FDB3C = OVERSCAN_SIZE;
    D_global_asm_807FDB3E = OVERSCAN_SIZE;
    D_global_asm_807FDB40 = (320 - OVERSCAN_SIZE);
    D_global_asm_807FDB42 = (240 - OVERSCAN_SIZE);
}

//@recomp: Used for a bunch of display list initialization
RECOMP_PATCH Gfx *func_global_asm_807132DC(Gfx *dl) {
    dl = func_global_asm_805FD030(dl);
    gSPDisplayList(dl++, &D_1000118);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, OVERSCAN_SIZE, OVERSCAN_SIZE, 320 - OVERSCAN_SIZE, 240 - OVERSCAN_SIZE);
    return dl;
}

//@recomp: Sprite reset function. Adjust scissor variables to not be tied to the original screen dimensions
RECOMP_PATCH void func_global_asm_80714A9C(void) {
    D_global_asm_807FDB0F = 0;
    D_global_asm_807FDB10 = 1;
    D_global_asm_807FDB14 = 0;
    D_global_asm_807FDB18 = 0;
    D_global_asm_807FDB1C = 1;
    D_global_asm_807FDB1A = 0;
    D_global_asm_807FDB1D = 0;
    D_global_asm_807FDB28 = 0;
    D_global_asm_807FDB2C = 0;
    D_global_asm_807FDB30 = 0;
    D_global_asm_807FDB36 = 0;
    D_global_asm_807FDB38 = -1;
    D_global_asm_807FDB3C = D_global_asm_8074450C * OVERSCAN_SIZE;
    D_global_asm_807FDB3E = D_global_asm_8074450C * OVERSCAN_SIZE;
    D_global_asm_807FDB40 = D_global_asm_8074450C * ((320 - OVERSCAN_SIZE));
    D_global_asm_807FDB42 = D_global_asm_8074450C * ((240 - OVERSCAN_SIZE));
    D_global_asm_807FDB3A = 0x258;
}

#define D_global_asm_807463AC (*(volatile s16*)0x807463AC)
#define D_global_asm_807463B0 (*(volatile s16*)0x807463B0)
#define D_global_asm_807463B4 (*(volatile u16*)0x807463B4)
#define DK_DKTV_LAG_START 215
#define DK_DKTV_LAG_END 325
#define DK_DKTV_LAG_START2 420


void getDKTVAttrs(s32 *numerator, s32 *denominator, s32 *scaled_frame) {
    // Correction code for DK's DK TV Demo where lag desyncs the inputs from the intended sequence of events
    // Rather than inducing lag, we change how frequently it'll change to the next input set.
    *numerator = 2;
    *denominator = 2;
    *scaled_frame = D_global_asm_807463AC;
    if (current_map == MAP_JAPES) {
        if (D_global_asm_807463AC > DK_DKTV_LAG_START) {
            if (D_global_asm_807463AC < DK_DKTV_LAG_END) {
                *numerator = 3;
                *scaled_frame = DK_DKTV_LAG_START + (((D_global_asm_807463AC - DK_DKTV_LAG_START) * 2) / 3);
            } else if (D_global_asm_807463AC > DK_DKTV_LAG_START2) {
                *numerator = 3;
                *scaled_frame = DK_DKTV_LAG_START2 + (((D_global_asm_807463AC - DK_DKTV_LAG_START2) * 2) / 3);
            }
        }
    }
}

//@recomp: Slow down the rate of the DK64 Buttons being parsed for DK's demo
RECOMP_PATCH void func_global_asm_8060B55C(u16 *arg0) {
    s32 numerator;
    s32 denominator;
    s32 scaled_frame;
    getDKTVAttrs(&numerator, &denominator, &scaled_frame);
    if (scaled_frame < (D_global_asm_807463B0 - 1)) {
        *arg0 = D_global_asm_807ECE98->button & 0x7FFF;
        if ((D_global_asm_807463AC % numerator) != denominator) {
            D_global_asm_807ECE98++;
        } else {
            // Correct the demo fadeout timer counter so it doesn't fade out early
            if (D_global_asm_8075531C > 0) {
                D_global_asm_8075531C++;
            }
        }
        D_global_asm_807463AC++;
        return;
    }
    is_autowalking = 0;
}

//@recomp: Slow down the rate of the stick inputs being parsed for DK's demo
RECOMP_PATCH void func_global_asm_8060B4D4(OSContPad *arg0) {
    s32 numerator;
    s32 denominator;
    s32 scaled_frame;
    getDKTVAttrs(&numerator, &denominator, &scaled_frame);
    if (scaled_frame < (D_global_asm_807463B0 - 1)) {
        arg0->stick_x = D_global_asm_807ECE98->stick_x;
        arg0->stick_y = D_global_asm_807ECE98->stick_y;
        if (D_global_asm_807ECE98->button & 0x8000) {
            if ((D_global_asm_807463AC % numerator) != denominator) {
                D_global_asm_807463B4 = D_global_asm_807ECE94[0];
                D_global_asm_807ECE94++;
            }
        }
        arg0->button = D_global_asm_807463B4 & 0xEFFF;
    }
}

//@recomp: Adjust the vertices for the fade transition square. Currently have this tied to SCREEN_WIDTH and SCREEN_HEIGHT
RECOMP_PATCH Gfx *func_global_asm_80703AB0(Gfx *dl, u8 arg1) {
    gSPDisplayList(dl++, &D_1000118);
    gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gDPPipeSync(dl++);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetTexturePersp(dl++, G_TP_NONE);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(dl++, G_CC_SHADE, G_CC_SHADE);

    D_global_asm_80754C48[D_global_asm_807444FC][0].v.cn[3] = MIN(0xFF, arg1 * 2);
    D_global_asm_80754C48[D_global_asm_807444FC][1].v.cn[3] = arg1;
    D_global_asm_80754C48[D_global_asm_807444FC][2].v.cn[3] = MIN(0xFF, arg1 * 2);
    D_global_asm_80754C48[D_global_asm_807444FC][3].v.cn[3] = arg1;
    D_global_asm_80754C48[D_global_asm_807444FC][1].v.ob[0] = SCREEN_WIDTH;
    D_global_asm_80754C48[D_global_asm_807444FC][2].v.ob[0] = SCREEN_WIDTH;
    D_global_asm_80754C48[D_global_asm_807444FC][2].v.ob[1] = SCREEN_HEIGHT;
    D_global_asm_80754C48[D_global_asm_807444FC][3].v.ob[1] = SCREEN_HEIGHT;
    gSPVertex(dl++, osVirtualToPhysical(&D_global_asm_80754C48[D_global_asm_807444FC][0]), 4, 0);
    gSP2Triangles(dl++, 0, 1, 2, 0, 0, 2, 3, 0);
    gDPPipeSync(dl++);
    return dl;
}