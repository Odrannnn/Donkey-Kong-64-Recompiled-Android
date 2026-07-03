#include "patches.h"
#include "PR/os_message.h"
#include "enums.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "misc_funcs.h"
#include "debug_config.h"

extern OSMesgQueue D_dk64_boot_80011548;
extern s8 D_dk64_boot_8000DCB0;

void osViBlack(u8);

//RECOMP_PATCH void func_dk64_boot_800004F4(s32 arg0) {
//    OSMesg sp24;
//    void (*gaFunc)(OSMesgQueue*, s32);
//    recomp_printf("Started func_dk64_boot_800004F4\n");
//    osRecvMesg(&D_dk64_boot_80011548, &sp24, OS_MESG_BLOCK);
//    recomp_printf("Block ended, sp24 is %04X\n", sp24);
//    switch ((u32)sp24) {
//    case 0x29d:
//        if (D_dk64_boot_8000DCB0) {
//            gaFunc = (void*)0x805fb300; //TODO-shift: this number is hardcoded to the overlay starting address
//            gaFunc(&D_dk64_boot_80011548, 2);
//            break;
//        }
//        osViBlack(1);
//        while (1);
//    case 0x309:
//        osSetThreadPri(NULL, 0x12);
//        gaFunc = (void*)0x805fb300; //TODO-shift: this number is hardcoded to the overlay starting address
//        gaFunc(&D_dk64_boot_80011548, 1);
//        while (1);
//    }
//}