#ifndef DKCOOP_SINGLE_SLOT_MENU_H
#define DKCOOP_SINGLE_SLOT_MENU_H

#include "menu_slot_policy.h"

extern s8 current_file_selection;
extern s8 D_menu_80033F38;
extern s8 D_menu_80033F48;
extern u32 global_properties_bitfield;
extern char** label_string_pointer_array;
extern u8 D_global_asm_807204BC[];
extern u8 D_global_asm_8072052C[];
extern u8 D_global_asm_80720C34[];
extern u8 D_global_asm_80720CF0[];
extern u8 D_global_asm_80720D14[];

extern void func_menu_8002FC1C(Actor*, MenuAdditionalActorData*, s32);
extern void func_menu_8002FD38(MenuAdditionalActorData*, s32, s32);
extern void func_menu_8002FE08(MenuAdditionalActorData*, s32);
extern void func_menu_80030894(MenuAdditionalActorData*, void*, s32, s32, f32, u8, s32);
extern s32 func_menu_800317E8(void*, f32, f32, f32*, f32*, s32, s8, f32);
extern s32 func_menu_800322D0(s32);
extern Gfx* printText(Gfx*, s16, s16, f32, char*);
extern s16 playSound(s16, s32, f32, f32, u8, u8);

static unsigned coop_single_file_menu(void) {
    return role == ROLE_HOST || role == ROLE_JOIN;
}

// Main adventure file selector. Co-op exposes one physical file and the delete
// action; campaign selection lives in the mod configuration.
RECOMP_PATCH void func_menu_80028C20(Actor* actor, s32 input) {
    MenuAdditionalActorData* menu = actor->MaaD;
    unsigned single = coop_single_file_menu();
    if (menu->unk0 == 0.0f) {
        if (menu->unk4 == 0.0f) {
            if (input & 1) {
                menu->unk16 = 0;
                playSound(0x2C9, 0x7FFF, 63.0f, 1.0f, 0, 0);
                if ((unsigned char)menu->unk17 == coop_file_menu_delete_index(single)) {
                    menu->unk13 = 4;
                } else {
                    D_menu_80033F48 = coop_file_menu_file(single, (unsigned char)menu->unk17);
                    menu->unk13 = 3;
                }
            } else if (input & 2) {
                playSound(0x2C9, 0x7FFF, 63.0f, 1.0f, 0, 0);
                menu->unk16 = 0;
                menu->unk13 = 1;
            } else {
                func_menu_8002FD38(menu, coop_file_menu_count(single), input);
            }
        }
        func_menu_8002FE08(menu, coop_file_menu_count(single));
    }
    func_menu_8002FC1C(actor, menu, 1);
}

RECOMP_PATCH Gfx* func_menu_80028D3C(Actor* actor, Gfx* dl) {
    void* data = actor->additional_actor_data;
    f32 x, y;
    unsigned single = coop_single_file_menu();
    global_properties_bitfield &= ~0x10;
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    s32 selection = func_menu_800317E8(data, 160.0f, 15.0f, &x, &y,
        coop_file_menu_count(single), 1, 0.3f);
    char text[16];
    if ((unsigned)selection == coop_file_menu_delete_index(single)) {
        _sprintf(text, "%s", label_string_pointer_array[5]);
    } else {
        unsigned file = coop_file_menu_file(single, selection);
        if (func_menu_800322D0(file)) _sprintf(text, "%s", label_string_pointer_array[6]);
        else _sprintf(text, "%s %d", label_string_pointer_array[7], file + 1);
    }
    return printText(dl, x * 4.0f, y * 4.0f, 0.6f, text);
}

RECOMP_PATCH void func_menu_80028EA8(Actor* actor, s32 arg1) {
    MenuAdditionalActorData* menu = actor->MaaD;
    unsigned single = coop_single_file_menu();
    menu->unk17 = single ? 0 : current_file_selection;
    func_menu_80030894(menu, D_global_asm_80720C34, 0xA0, 0x78, 0.75f, 2, 4);
    if (func_menu_800322D0(0)) D_menu_80033F38 = 0;
    func_menu_80030894(menu, D_global_asm_8072052C, 0, 0, 1.0f, 2, 2);
    if (!single) {
        if (func_menu_800322D0(1)) D_menu_80033F38 = 0;
        func_menu_80030894(menu, D_global_asm_8072052C, 1, 0, 1.0f, 2, 2);
        if (func_menu_800322D0(2)) D_menu_80033F38 = 0;
        func_menu_80030894(menu, D_global_asm_8072052C, 3, 0, 1.0f, 2, 2);
    }
    func_menu_80030894(menu, D_global_asm_807204BC,
        coop_file_menu_delete_index(single), 0, 1.0f, 2, 2);
    func_menu_80030894(menu, D_global_asm_80720CF0, 0x122, 0xD2, 0.75f, 2, 0);
    func_menu_80030894(menu, D_global_asm_80720D14, 0x23, 0xD2, 0.75f, 2, 0);
}

// Delete-file selector. Co-op can delete only its single campaign file.
RECOMP_PATCH void func_menu_80029AAC(Actor* actor, s32 input) {
    MenuAdditionalActorData* menu = actor->MaaD;
    unsigned count = coop_delete_menu_count(coop_single_file_menu());
    if (menu->unk0 == 0.0f) {
        if (menu->unk4 == 0.0f) {
            if (input & 1) {
                if (func_menu_800322D0(menu->unk17) == 0) {
                    menu->unk16 = 0;
                    menu->unk13 = 5;
                    D_menu_80033F48 = menu->unk17;
                } else {
                    playSound(0x98, 0x7FFF, 63.0f, 1.0f, 0, 0);
                }
            } else if (input & 2) {
                playSound(0x2C9, 0x7FFF, 63.0f, 1.0f, 0, 0);
                menu->unk16 = 0;
                menu->unk13 = 2;
            } else {
                func_menu_8002FD38(menu, count, input);
            }
        }
        func_menu_8002FE08(menu, count);
    }
    func_menu_8002FC1C(actor, menu, 1);
}

RECOMP_PATCH Gfx* func_menu_80029BB4(Actor* actor, Gfx* dl) {
    void* data = actor->additional_actor_data;
    f32 x, y;
    unsigned count = coop_delete_menu_count(coop_single_file_menu());
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    s32 file = func_menu_800317E8(data, 160.0f, 25.0f, &x, &y, count, 1, 0.45f);
    char text[16];
    if (func_menu_800322D0(file)) _sprintf(text, "%s", label_string_pointer_array[6]);
    else _sprintf(text, "%s %d", label_string_pointer_array[7], file + 1);
    dl = printText(dl, x * 4.0f, y * 4.0f, 0.6f, text);
    return printText(dl, x * 4.0f, (y * 4.0f) - 0x3C, 0.6f, label_string_pointer_array[8]);
}

RECOMP_PATCH void func_menu_80029D30(Actor* actor, s32 arg1) {
    MenuAdditionalActorData* menu = actor->MaaD;
    unsigned single = coop_single_file_menu();
    menu->unk17 = 0;
    func_menu_80030894(menu, D_global_asm_80720CF0, 0x122, 0xD2, 0.75f, 2, 0);
    func_menu_80030894(menu, D_global_asm_80720D14, 0x23, 0xD2, 0.75f, 2, 0);
    if (func_menu_800322D0(0)) D_menu_80033F38 = 0;
    func_menu_80030894(menu, D_global_asm_8072052C, 0, 0, 1.0f, 2, 3);
    if (!single) {
        if (func_menu_800322D0(1)) D_menu_80033F38 = 0;
        func_menu_80030894(menu, D_global_asm_8072052C, 1, 0, 1.0f, 2, 3);
        if (func_menu_800322D0(2)) D_menu_80033F38 = 0;
        func_menu_80030894(menu, D_global_asm_8072052C, 2, 0, 1.0f, 2, 3);
    }
    func_menu_80030894(menu, D_global_asm_807204BC, 0xA0, 0x78, 1.0f, 2, 4);
    func_menu_80030894(menu, D_global_asm_80720C34, 0xA0, 0xD2, 0.75f, 2, 0);
}

#endif
