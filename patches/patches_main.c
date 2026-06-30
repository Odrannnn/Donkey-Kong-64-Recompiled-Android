#include "patches_main.h"

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

RECOMP_PATCH void func_global_asm_805FBFF4(s32 arg0) {
    s32 phi_s4;
    OSMesg* sp38;
    recomp_printf("func_global_asm_805FBFF4 Started:\n");

    phi_s4 = 1;
    if (osTvType == OS_TV_PAL) {
        D_global_asm_807444BC = 1.25f;
    }

    func_global_asm_805FBC5C();
    osViSetSpecialFeatures(VI_CTRL_TYPE_16 | VI_CTRL_SERRATE_ON);
    func_global_asm_805FBE04();
    D_global_asm_8076A070 = D_global_asm_80767CC0 - 2;
    osRecvMesg(&D_global_asm_8076A110, (void*)&sp38, 1);

    OSTimer timer;
    OSMesgQueue mq;
    OSMesg mq_buf;

    osCreateMesgQueue(&mq, &mq_buf, 1);
    osSetTimer(&timer, OS_USEC_TO_CYCLES(500000), 0, &mq, NULL);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);

    playSound(0x23C, 0x7FFF, 63.0f, 1.0f, 0, 0);

    osCreateMesgQueue(&mq, &mq_buf, 1);
    osSetTimer(&timer, OS_USEC_TO_CYCLES(1300000), 0, &mq, NULL);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);

    while (TRUE) {
        D_global_asm_8074682C = 0xC8;

        while (D_global_asm_80744460) {}

        if (D_global_asm_8076A0B1 & 1 && !D_global_asm_8076A0B2) {
            func_global_asm_805FE7FC();
            if (D_global_asm_807444F8 == 2) {
                global_properties_bitfield |= 0x200;
                D_global_asm_80744504 = 8;
            }
        }
        //gEXEnable(); // @recomp

        switch (is_cutscene_active) {
        case 6:
            //gEXSetRefreshRate(, 60 / 2);
            func_global_asm_8070A934(next_map, next_exit);
            break;
        case 3:
            //gEXSetRefreshRate(, 60);
            func_arcade_80024000();
            break;
        case 4:
            //gEXSetRefreshRate(, 60);
            func_jetpac_80024000();
            break;
        case 5:
            break;
        default:
            //gEXSetRefreshRate(, 60 / 2);
#ifdef DEBUG_WARP
            menuMain();
#endif
            func_global_asm_805FC2B0();
            break;
        }

        func_global_asm_80600B10();
        func_global_asm_8066AF40();
        func_global_asm_80610268(0x4D2);
        if (D_global_asm_807F059C[0]) {
            func_global_asm_80610268(0x929);
        }
        func_global_asm_80600674(); // calculateLagBoost()
        if ((is_cutscene_active == 0) || (is_cutscene_active == 1) || (is_cutscene_active == 7)) {
            func_global_asm_80658CCC();
            func_global_asm_80700BF4();
        }
        func_global_asm_80611730();
        if (gStackCanary != 0x12345678) {
            raiseException(2, 0, 0, 0);
        }
        if (phi_s4) {
            osSendMesg((void*)D_global_asm_807655E0, (void*)0x309, OS_MESG_BLOCK);
            phi_s4 = 0;
        }
        if (D_global_asm_8076A0B1 & 1 && D_global_asm_807FD888 == 31.0f) {
            D_global_asm_8076A0B2--;
        }
        D_global_asm_807444F0 = is_cutscene_active;
    }
}

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
        D_global_asm_80744498 = 0;
        D_global_asm_8074449C = 0;
        D_global_asm_807444A0 = (D_global_asm_8074450C * 320) - 1;
        D_global_asm_807444A4 = (D_global_asm_8074450C * 240) - 1;
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
        D_global_asm_80744498 = D_global_asm_8074450C * 10;
        D_global_asm_8074449C = D_global_asm_8074450C * 10;
        D_global_asm_807444A0 = (D_global_asm_8074450C * 310) - 1;
        D_global_asm_807444A4 = (D_global_asm_8074450C * 230) - 1;
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
    D_global_asm_80744490 = D_global_asm_8074450C * 320; //width
    D_global_asm_80744494 = D_global_asm_8074450C * 240; //height
    D_global_asm_807444AC = D_global_asm_8074449C + (D_global_asm_8074450C * 30); //overscan
    D_global_asm_807444B0 = D_global_asm_807444A4 - (D_global_asm_8074450C * 30); //overscan
    D_global_asm_807444A8 = D_global_asm_8074449C;
    D_global_asm_807444B4 = D_global_asm_807444A4;
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
    osSetTimer(&D_global_asm_8076A130, 0xD693A4, 0, (void*)mq, mq->msgs[0]);
    //@recomp move playsound to a bit later
    //playSound(0x23C, 0x7FFF, 63.0f, 1.0f, 0, 0);
}

RECOMP_PATCH void func_global_asm_805FB7E4(void) {
    Gfx* dl = D_global_asm_8076A050[D_global_asm_807444FC];
    s32 empty;
    s32 sp58;
    s32* temp_s0;
    u16* var_v1; // 50
    u16* sp4C;
    s32 sp48;
    s32 sp44;
    s32 sp40;
    s32 sp3C;
    u32 sp38;
    u16* end;
    s32 y, x;

    var_v1 = D_global_asm_80744470[0];
    end = &var_v1[0x12C00];
    while (var_v1 < end) {
        *var_v1++ = 1;
    }
    //osWritebackDCacheAll();
    temp_s0 = (s32*) & D_global_asm_80744470[1][0x6400];
    func_global_asm_805FB750(0x38, 0x10, temp_s0);
    func_global_asm_805FB750(temp_s0[0] + 0x178, 0x10, temp_s0);
    func_global_asm_805FB750(temp_s0[0], temp_s0[1] - temp_s0[0], temp_s0);
    sp3C = (s32)temp_s0;
    sp38 = (u32)D_global_asm_80744470[1]; //framebuffer pointer
    sp4C = (u16*)D_global_asm_80744470[1];
    func_dk64_boot_800024E0((void*) & sp3C, &sp38, &D_global_asm_80744470[1][0xAF00]); //dump red nintendo logo to framebuffer
    //TODO: add f3dex2 commands here to draw the red nintendo logo (how do we make sure the task is submitted (aka, how to get DL pointer?))

    var_v1 = &D_global_asm_80744470[0][0x7840];
    for (y = 0; y < 0x30; y++) {
        for (x = 0; x < 0xC0; x++) {
            *var_v1++ = *sp4C++;
        }
        var_v1 += 0x80;
    }
    var_v1 = D_global_asm_80744470[1];
    end = &var_v1[0x12C00];
    while (var_v1 < end) {
        *var_v1++ = 1;
    }
    //osWritebackDCacheAll();
}