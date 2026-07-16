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
RECOMP_PATCH s32 func_global_asm_80658E8C(f32 arg0, f32 arg1, f32 arg2, u8 arg3, u8 arg4) {
    return 0;
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

Gfx* func_global_asm_8062CEA8(Gfx*, void*, u8);      /* extern */
Gfx* func_global_asm_8063A968(Gfx*, s32);           /* extern */
void* func_global_asm_80656B98(Gfx*, s32, s32);       /* extern */
Gfx* func_global_asm_8065D994(Gfx*, s16);           /* extern */
void* func_global_asm_8065FD88(Gfx*, s32, s32);       /* extern */

typedef struct Struct807F6C0C {
    s32 unk0;
    s32 unk4;
    void *unk8;
    void *unkC;
} Struct807F6C0C;

typedef struct {
    u8 loaded;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 visible;
    u8 unk6;
    u8 unk7;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    void *unk14;
    void *unk18;
    void *unk1C;
    void *unk20;
    s32 unk24;
    u8 pad28[0x2C - 0x28];
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    u8 pad3C[0x4C - 0x3C];
    void *unk4C;
    s32 unk50;
    u8 pad54[0x58 - 0x54];
    void *unk58;
    u8 pad5C[0x60 - 0x5C];
    s32 unk60[1];
    s32 unk64;
    s32 deload1;
    s32 deload2;
    s32 deload3;
    s32 deload4;
    void *unk78;
    void *unk7C;
    s16 unk80;
    s16 unk82;
    s16 unk84;
    s16 unk86;
    u8 pad2[0x1C8 - 0x88];
} Chunk;

typedef struct Struct80630B70 Struct80630B70;
struct Struct80630B70 {
    Actor *unk0;
    s32 unk4;
    s32 unk8;
    f32 unkC;
    u8 pad10[0x14-0x10];
    Struct80630B70 *unk14;
    u8 pad18[0x24 - 0x18];
    u8 unk24;
};

extern void* D_global_asm_807F5DE8;
extern void* D_global_asm_807F5DEC;
extern u8 D_global_asm_807F5FEC;
extern Struct80630B70* D_global_asm_807F5FFC;
extern u8 D_global_asm_807F6009;
extern s32 D_global_asm_807F600C;
extern s16 D_global_asm_807F6BF0[];
extern u8 D_global_asm_807F6C08;
extern Struct807F6C0C* D_global_asm_807F6C0C;
extern s16 D_global_asm_807F6C58[];
extern s32 D_global_asm_807F6C80;
extern void* D_global_asm_807F7074;
extern Chunk *chunk_array_pointer;
extern void func_global_asm_8062EE48(u8 arg0);
extern Gfx *func_critter_80027034(Gfx *dl);
extern Gfx *func_global_asm_806634A4(Gfx *dl);
extern Gfx *func_global_asm_80630B70(Gfx*, void *, f32, f32, f32, s32, s16, u8);
extern Gfx *func_global_asm_806592B4(Gfx *dl);
extern Gfx *func_global_asm_8062EDA8(Gfx *dl, u8 arg1);

typedef struct Struct80655DD0_arg1 {
    s32 unk0;
    s32 unk4;
    void *unk8;
} Struct80655DD0_arg1;

// @recomp: Chunk bounds fix
RECOMP_PATCH Gfx *func_global_asm_80655DD0(Gfx * dl, Struct80655DD0_arg1 * arg1, f32 arg2, f32 arg3, f32 arg4, s32 arg5) {
    s32 i;
    s32 var_s6;
    void *var_a1;
    s32 sp60;

    var_s6 = 0;
    sp60 = D_global_asm_807F5FEC && character_change_array[cc_player_index].fov_y == 45.0;
    gDPPipeSync(dl++);
    if (arg5 & 0x10) {
        gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(dl++, 0x06, osVirtualToPhysical(D_global_asm_807F5DE8));
        gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
        dl = func_global_asm_8062CEA8(dl, arg1, 1);
        if (arg1->unk8 != (void*)-1) {
            gDPPipeSync(dl++);
            gSPDisplayList(dl++, osVirtualToPhysical(arg1->unk8));
        }
        dl = func_global_asm_8065FD88(dl, 0, 0);
        for (i = 0; i < D_global_asm_807F6C08; i++) {
            if (D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unk8 != (void*)-1) {
                gDPPipeSync(dl++);
                gSPDisplayList(dl++, osVirtualToPhysical(D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unk8));
            }
            if (D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unkC != (void*)-1) {
                gDPPipeSync(dl++);
                gSPDisplayList(dl++, osVirtualToPhysical(D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unkC));
            }
        }
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }
    for (i = D_global_asm_807F6C80 - 1; i >= 0; i--) {
        var_s6 = D_global_asm_807F6C58[i];
        if (chunk_array_pointer[var_s6].loaded == 1) {
            if (chunk_array_pointer[var_s6].unk2 != 0) {
                func_global_asm_8062EE48(var_s6);
                chunk_array_pointer[var_s6].unk2 = 0;
            }
            gDPPipeSync(dl++);
            gDPSetScissor(dl++, G_SC_NON_INTERLACE,
                0,
                0,
                D_global_asm_80744490,
                D_global_asm_80744494);
            func_global_asm_80658E58(
                0,
                0,
                D_global_asm_80744490,
                D_global_asm_80744494);
            dl = func_global_asm_806592B4(dl);
            if (arg5 & 0x10) {
                dl = func_global_asm_8062EDA8(dl, var_s6);
                if (chunk_array_pointer[var_s6].unk2C != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 0);
                    gDPPipeSync(dl++);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                }
                if (chunk_array_pointer[var_s6].unk34 != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 2);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                }
            }
            gDPPipeSync(dl++);
            if (!(D_global_asm_807FBB64 & 0x01000000)) {
                dl = func_global_asm_8065D994(dl, var_s6);
            }
            dl = func_global_asm_8063A968(dl, chunk_array_pointer[var_s6].unk24);
            D_global_asm_807F6009 = 0xFF;
            var_a1 = ((u8)sp60) ? chunk_array_pointer[var_s6].unk18 : chunk_array_pointer[var_s6].unk14;
            dl = func_global_asm_80630B70(dl, var_a1, arg2, arg3, arg4, arg5, var_s6, sp60);
            if (arg5 & 0x10) {
                gSPLoadGeometryMode(dl++, 0);
                gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
                dl = func_global_asm_806592B4(dl);
                dl = func_global_asm_8062EDA8(dl, var_s6);
                if (chunk_array_pointer[var_s6].unk30 != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 1);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gDPPipeSync(dl++);
                }
                if (chunk_array_pointer[var_s6].unk38 != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 3);
                    gDPPipeSync(dl++);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                }
            }
        }
    }
    if (arg5 & 0x10) {
        gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(dl++, 0x06, osVirtualToPhysical(D_global_asm_807F5DE8));
        gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
        dl = func_global_asm_8062CEA8(dl, arg1, 2);
        if (D_global_asm_807FBB64 & 0x1000) {
            gDPPipeSync(dl++);
            dl = func_critter_80027034(dl);
            gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        }
        dl = func_global_asm_8065FD88(dl, var_s6, 1);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(dl++);
        dl = func_global_asm_806634A4(dl);
    }
    for (i = D_global_asm_807F6C80 - 1; i >= 0; i--) {
        var_s6 = D_global_asm_807F6C58[i];
        if (chunk_array_pointer[var_s6].loaded == 1) {
            gDPPipeSync(dl++);
            gDPSetScissor(dl++, G_SC_NON_INTERLACE,
                0,
                0,
                D_global_asm_80744490,
                D_global_asm_80744494);
            func_global_asm_80658E58(
                0,
                0,
                D_global_asm_80744490,
                D_global_asm_80744494);
            D_global_asm_807F6009 = 0xFF;
            var_a1 = ((u8)sp60) ? chunk_array_pointer[var_s6].unk20 : chunk_array_pointer[var_s6].unk1C;
            dl = func_global_asm_80630B70(dl, var_a1, arg2, arg3, arg4, arg5, var_s6, sp60);
            if (D_global_asm_807FBB64 & 0x01000000) {
                dl = func_global_asm_8065D994(dl, var_s6);
            }
        }
    }
    if (arg5 & 0x10) {
        gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(dl++, 0x06, osVirtualToPhysical(D_global_asm_807F5DE8));
        gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
        dl = func_global_asm_8062CEA8(dl, arg1, 3);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(dl++);
    }
    gDPPipeSync(dl++);
    gDPSetScissorFrac(dl++, G_SC_NON_INTERLACE,
        character_change_array[cc_player_index].unk270[0] * 4.0f,
        character_change_array[cc_player_index].unk270[1] * 4.0f,
        character_change_array[cc_player_index].unk270[2] * 4.0f,
        character_change_array[cc_player_index].unk270[3] * 4.0f);
    func_global_asm_80658E58(
        character_change_array[cc_player_index].unk270[0],
        character_change_array[cc_player_index].unk270[1],
        character_change_array[cc_player_index].unk270[2],
        character_change_array[cc_player_index].unk270[3]);
    D_global_asm_807F600C = 0;
    dl = func_global_asm_80630B70(dl, D_global_asm_807F5FFC, arg2, arg3, arg4, arg5, -1, 0U);
    dl = func_global_asm_8065D994(dl, -1);
    return dl;
}

//@recomp: Water Screen Overlay
RECOMP_PATCH Gfx *func_global_asm_80701CA0(Gfx *dl) {
    CameraPaad* camera_paad;
    PlayerAdditionalActorData* player_aad;
    f32 var_f2;
    u8 spC3;
    u8 var_fp;
    s8 spBA;
    u8 i;
    Mtx* sp6C;
    Mtx* sp68;

    spC3 = FALSE;
    var_fp = 0x64;
    spBA = FALSE;
    switch (current_map) {
        case MAP_GALLEON_MERMAID:
            spBA = TRUE;
            goto block_12;
        case MAP_GALLEON_SUBMARINE:
        case MAP_GALLEON_SHIPWRECK_DIDDY_LANKY_CHUNKY:
        case MAP_GALLEON_SHIPWRECK_DK_TINY:
        case MAP_GALLEON_SHIPWRECK_LANKY_TINY:
            return dl;
        case MAP_GALLEON:
            if (((character_change_array->chunk == 9) || (character_change_array->chunk == 0xA)) && (isFlagSet(0x9C, 0U))) {
                return dl;
            }
        default:
    block_12:
            for (i = 0; i < cc_number_of_players; i++) {
                if (character_change_array[i].does_player_exist) {
                    player_aad = character_change_array[i].playerPointer->PaaD;
                    camera_paad = player_aad->unk104->CaaD;
                    if (spBA || (
                        character_change_array[i].unk2E8 && 
                        (character_change_array[i].unk220 < (character_change_array[i].unk24C + 3.0f))
                    )) {
                        spC3 = TRUE;
                        gDPPipeSync(dl++);
                        dl = func_global_asm_805FCFD8(dl);
                        gDPSetScissor(dl++, G_SC_NON_INTERLACE,
                            gScissorUpLX,
                            gScissorUpLY,
                            gScissorLowerRightX,
                            gScissorLowerRightY);
                        gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
                        gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                        var_f2 = character_change_array[i].unk24C - character_change_array[i].unk220;
                        switch (current_map) {
                            case MAP_GALLEON:
                                var_f2 *= 0.07;
                                break;
                            case MAP_GALLEON_TREASURE_CHEST:
                                var_fp = 50;
                                var_f2 *= 0.07;
                                break;
                            case MAP_GALLEON_PUFFTOSS:
                                var_fp = 50;
                            default:
                                var_f2 *= 0.4;
                                break;
                        }
                        if (var_f2 > 80.0f) {
                            var_f2 = 80.0f;
                        }
                        if (D_global_asm_807FD890) {
                            player_aad->unk1E8 = var_f2;
                        } else {
                            player_aad->unk1E8 += ((var_f2 - player_aad->unk1E8) * 0.05);
                            var_f2 = (u8) player_aad->unk1E8;
                        }
                        gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x3C, (u8)(var_fp + var_f2));
                        gDPSetCycleType(dl++, G_CYC_1CYCLE);\
                        gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        //@recomp: Used to be a verts-based draw, now is just fillrect
                        gDPFillRectangle(dl++,
                            character_change_array[i].unk270[0],
                            character_change_array[i].unk270[1],
                            character_change_array[i].unk270[2],
                            character_change_array[i].unk270[3]
                        );
                        gDPPipeSync(dl++);
                    }
                    character_change_array[i].unk2E8 = camera_paad->unkFA;
                    character_change_array[i].unk24C = camera_paad->unk90;
                }
            }
            if (spC3) {
                gSPMatrix(dl++, &D_2000000, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                gSPMatrix(dl++, &D_2000200, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
                gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                D_global_asm_807FD890 = 0;
            } else {
                D_global_asm_807FD890 = 1;
            }
            break;
    }
    return dl;
}

//@recomp: Sandstorm screen overlay
RECOMP_PATCH Gfx* func_global_asm_8068D264(Gfx* dl, f32* cooldown_timer) {
    void* temp_v0;
    f32 cooldown;
    f32 half_lag;

    cooldown = *cooldown_timer;
    half_lag = D_global_asm_80744478 * 0.5;
    temp_v0 = getPointerTableFile(TABLE_25_TEXTURES_GEOMETRY, 0x173C, 1U, 0U);
    func_global_asm_8066B434(temp_v0, 0x1B2, 0x46);
    gDPPipeSync(dl++);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, \
        character_change_array->unk270[0], \
        character_change_array->unk270[1], \
        character_change_array->unk270[2], \
        character_change_array->unk270[3]);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetTextureLOD(dl++, G_TL_LOD);
    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    if (current_map == MAP_AZTEC) {
        gDPSetPrimColor(dl++, 0, 0, 0x8A, 0x52, 0x16, 200.0f * cooldown);
    } else {
        gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 35.0f * cooldown);
    }
    gDPLoadTextureBlock(dl++, (s32)temp_v0 + 0x80000000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gDPTextureRectangle(
        dl++,
        character_change_array->unk270[0] * 4,
        character_change_array->unk270[1] * 4,
        character_change_array->unk270[2] * 4,
        character_change_array->unk270[3] * 4,
        G_TX_RENDERTILE,
        (s32)D_global_asm_80750228,
        (s32)D_global_asm_8075022C,
        1024,
        -1024
    );
    gDPPipeSync(dl++);

    if (current_map == MAP_AZTEC) {
        D_global_asm_80750228 -= (2.0 * half_lag);
        D_global_asm_8075022C -= (14.0 * half_lag);
    } else {
        D_global_asm_80750228 -= (f64)half_lag;
        D_global_asm_8075022C -= (0.5 * half_lag);
    }
    if (D_global_asm_80750228 < 0.0) {
        D_global_asm_80750228 = 255.0f;
    }
    if (D_global_asm_8075022C < 0.0) {
        D_global_asm_8075022C = 255.0f;
    }
    return dl;
}

//@recomp: Patch "wrong cutscene" fade transition to match func_global_asm_80703374 (roughly)
RECOMP_PATCH Gfx *func_global_asm_80703AB0(Gfx *dl, u8 arg1) {
    if (arg1 != 0) {
        gSPClearGeometryMode(dl++, G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH | G_CLIPPING | 0x0040F9FA);
        gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
        gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
        gDPPipeSync(dl++);
        gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
        gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, arg1);
        gDPSetCycleType(dl++, G_CYC_1CYCLE);
        gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPFillRectangle(dl++, gScissorUpLX, gScissorUpLY, gScissorLowerRightX, gScissorLowerRightY);
    }
    gDPPipeSync(dl++);
    return dl;
}

//@recomp: rand() causes some issues with stdlib and collisions there. For now, just have this, which is what rand() does
#define RANDNUM() (func_global_asm_806119A0() & 0x7FFFFFFF)

// @recomp: Patch the static effect in the Rap to fill the full bounds
RECOMP_PATCH Gfx *func_global_asm_807035C4(Gfx *dl, Actor *arg1) {
    s16 var_s1;
    s16 temp_s2;
    s16 temp_t2;

    gDPPipeSync(dl++);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gDPSetPrimColor(dl++, 0, 0, 0xC8, 0xC8, 0xC8, 0x80);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetCombineLERP(dl++, NOISE, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, NOISE, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, character_change_array->unk270[0], character_change_array->unk270[1], character_change_array->unk270[2], character_change_array->unk270[3]);
    gDPFillRectangle(dl++, character_change_array->unk270[0], character_change_array->unk270[1], character_change_array->unk270[2], character_change_array->unk270[3]);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    for (var_s1 = 0; var_s1 < 10; var_s1++) {
        temp_s2 = (RANDNUM() >> 0xF) % (character_change_array->unk270[3] - character_change_array->unk270[1]) + character_change_array->unk270[1];
        temp_t2 = ((RANDNUM() >> 0xF) % 10) + 2;
        gDPFillRectangle(dl++, character_change_array->unk270[0], temp_s2, character_change_array->unk270[2], temp_s2 + temp_t2);
    }
    gDPPipeSync(dl++);
    return dl;
}

RECOMP_PATCH s32 func_global_asm_806522CC(s16 arg0, s16 arg1, s16 arg2) {
    return 1;
}

/*
extern s16 D_global_asm_80754CEC[];
extern void *D_global_asm_807FD978[8];
extern s16 D_global_asm_80754CE0;
s16 x_shift = 0;
s16 y_shift = 0;
#define X_REPEAT_COUNT 4
#define Y_REPEAT_COUNT 3

RECOMP_PATCH Gfx* func_global_asm_80706F90(Gfx* dl) {
    // have to be function static
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

    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetTexturePersp(dl++, G_TP_NONE);
    gDPSetTextureFilter(dl++, G_TF_POINT);
    gSPTexture(dl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gEXPushScissor(dl++);
    gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, D_global_asm_80744494);
    gDPPipeSync(dl++);
    var_s4 = y_shift + D_global_asm_80754CE0;
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
            x = (spF8 << 2) - x_shift;
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
    x_shift += 4;
    y_shift += 4;
    if (x_shift >= 0x200) {
        x_shift = 0;
    }
    if (y_shift >= 0x200) {
        y_shift = 0;
    }
    gEXPopScissor(dl++);
    gDPPipeSync(dl++);
    gDPSetTexturePersp(dl++, G_TP_PERSP);
    gDPSetTextureFilter(dl++, G_TF_BILERP);
    return dl;
}
*/

#define macro_8062DBDC_IF(a0, a1, a2) ((a0 * D_global_asm_807F5E50[0]) + (a1 * D_global_asm_807F5E50[1]) + (a2 * D_global_asm_807F5E50[2])) + D_global_asm_807F5E50[3]

//@recomp: Culling patch for maps
RECOMP_PATCH s32 func_global_asm_8062DBDC(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9, Struct8062DBDC *arg10) {
    s32 var_a0;
    s32 var_a1;
    s32 var_a2;
    s32 i;
    s32 pad[3];
 
    if (macro_8062DBDC_IF(arg0, arg1, arg2) < 0.0) {
        if (macro_8062DBDC_IF(arg0, arg1, arg5) < 0.0) {
            if (macro_8062DBDC_IF(arg0, arg4, arg2) < 0.0) {
                if (macro_8062DBDC_IF(arg0, arg4, arg5) < 0.0) {
                    if (macro_8062DBDC_IF(arg3, arg1, arg2) < 0.0) {
                        if (macro_8062DBDC_IF(arg3, arg1, arg5) < 0.0) {
                            if (macro_8062DBDC_IF(arg3, arg4, arg2) < 0.0) {
                                if (macro_8062DBDC_IF(arg3, arg4, arg5) < 0.0) {
                                    return TRUE;  //@recomp: Was false
                                 }
                            }
                        }
                    }
                }
            }
        }
    }
    var_a0 = FALSE;
    var_a1 = FALSE;
    var_a2 = FALSE;
    if ((arg0 <= arg6) && (arg6 <= arg3) && (arg1 <= arg7) && (arg7 <= arg4) && (arg2 <= arg8) && (arg8 <= arg5)) {
        return TRUE;
    }
    for (i = 0; (i < 5) && (!var_a2); i++) {
        if ((arg10[i].unk0[7] + (((arg10[i].unk0[4] * arg0) + (arg10[i].unk0[5] * arg1)) + (arg10[i].unk0[6] * arg2))) < 0) {
            var_a0 = TRUE;
        }
        if ((arg10[i].unk0[7] + (((arg10[i].unk0[4] * arg0) + (arg10[i].unk0[5] * arg4)) + (arg10[i].unk0[6] * arg2))) < 0) {
            var_a1 = TRUE;
        }
        if ((var_a0) && (var_a1)) {
            var_a2 = TRUE;
        }
    }
    if ((!var_a2) && (func_global_asm_8062E040(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) < arg9)) {
        return TRUE;
    }
    for (i = 0; i < 5; i++) {
        if (func_global_asm_8062E1F8(i, arg0, arg1, arg2, arg3, arg4, arg5, arg10)) {
            return TRUE;
        }
    }
    return TRUE;  // @recomp: Was false
}