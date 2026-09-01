// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "modding.h"
#include "ap_shops.h"

extern volatile u32 ap_gameplay_mode;
extern Actor *gCurrentActorPointer, *gPlayerPointer;
extern u32 global_properties_bitfield;
extern void loadText(Actor* actor, u16 file, u8 index);
extern s32 func_global_asm_80629148(void);
extern void func_global_asm_80629174(void), func_global_asm_8061CB08(void);
extern void func_global_asm_806F833C(u8 player);
extern void func_global_asm_806F8BC4(s32 resource, u8 show, u8 player);
extern void setFlag(s16 flag, u8 value, u8 type);
extern void func_menu_80026804(ap_shop_paad* paad, void* moves);
extern void ap_stock_func_menu_800262A8(ap_shop_paad*, void*, int);
extern void ap_stock_func_menu_80026B28(ap_shop_paad*, void*, int);
extern void* ap_stock_func_global_asm_8070DDDC(u16 file, s32 mode);

// Parsed text-file layout used by the stock loader/renderer (8070D6D8/E548).
// This private u16 file ID is outside ordinary u8 text-file IDs. The bank is
// immutable and lives with the mod, never in the heap-owned text cache. The
// stock release routine sees no cache entry and leaves this storage alone.
#define AP_SHOP_TEXT_FILE 0xfffe
typedef struct { u32 offset; u16 length, unused; } text_string;
typedef struct { u8 count, pad[3]; const text_string* strings; } text_language;
typedef struct { u8 type, pad[3]; const void* effect; const text_language* language; float delay; } text_block;
typedef struct { u8 count, pad[3]; const text_block* blocks; } text_box;
typedef struct { u8 count, pad[3]; const text_box* boxes; u16 size, unused; const char* text; } text_file;
_Static_assert(sizeof(text_string) == 8 && sizeof(text_language) == 8 && sizeof(text_block) == 16
    && sizeof(text_box) == 8 && sizeof(text_file) == 16, "Stock text ABI");

#define OFFER "YOU HAVE ENOUGH COINS FOR THIS ARCHIPELAGO CHECK. YOUR COINS WILL NOT BE SPENT."
#define SHORT "YOU NEED MORE COINS FOR THIS ARCHIPELAGO CHECK. YOUR COINS WILL NOT BE SPENT."
// Original randomizer's generic-item explanation (patch_text.py, index 0x2C).
#define POST "PAY ATTENTION, ~. THERE'S PLENTY MORE ITEMS TO GATHER IN THIS GAME. GET MOVING SO WE CAN DISPENSE OF K. ROOL"
static const char text[] = OFFER SHORT POST;
static const text_string strings[] = {
    {0, sizeof(OFFER)-1, 0},
    {sizeof(OFFER)-1, sizeof(SHORT)-1, 0},
    {sizeof(OFFER)+sizeof(SHORT)-2, sizeof(POST)-1, 0},
};
static const text_language languages[] = {{1,{0},&strings[0]}, {1,{0},&strings[1]}, {1,{0},&strings[2]}};
static const text_block blocks[] = {{1,{0},0,&languages[0],0}, {1,{0},0,&languages[1],0}, {1,{0},0,&languages[2],0}};
static const text_box boxes[] = {{1,{0},&blocks[0]}, {1,{0},&blocks[1]}, {1,{0},&blocks[2]}};
static const text_file bank = {3,{0},boxes,sizeof(text)-1,0,text};

RECOMP_PATCH void* func_global_asm_8070DDDC(u16 file, s32 mode) {
    // Queued text may outlive a gate transition. Always serve our private ID;
    // every ordinary file keeps the exact stock cache/refcount behavior.
    if (file == AP_SHOP_TEXT_FILE) return (void*)&bank;
    return ap_stock_func_global_asm_8070DDDC(file, mode);
}

RECOMP_PATCH void func_menu_800262A8(ap_shop_paad* paad, void* moves, int first_visit) {
    if (!(ap_gameplay_mode & AP_GAMEPLAY_SHOPS) || (paad && paad->item_type != AP_SHOP_ITEM_TYPE)) {
        ap_stock_func_menu_800262A8(paad, moves, first_visit);
        return;
    }
    // Stock dialogue indexes move arrays with item_type. AP's type 21 must
    // never enter that path. Empty shops/Jetpac retain their stock dialogue.
    if (!paad || !ap_apshop_context() || paad->substate != 0 || !func_global_asm_80629148()) return;
    int response = ap_apshop_afford(paad, 0) & 3;
    if (response != 2 && response != 3) {
        func_menu_80026804(paad, moves); // invalid/stale quote: request a fresh offer
        return;
    }
    func_global_asm_80629174();
    ((u8*)paad)[0x12] = 0; // original NPC's instrument-upgrade dialogue marker
    paad->substate = 1;
    loadText(gCurrentActorPointer, AP_SHOP_TEXT_FILE, response == 2 ? 0 : 1);
    func_global_asm_806F833C(0);
    func_global_asm_806F8BC4(1, 1, 0);
    func_global_asm_806F8BC4(0xe, 1, 0);
}

RECOMP_PATCH void func_menu_80026B28(ap_shop_paad* paad, void* moves, int intro_flag) {
    if (!(ap_gameplay_mode & AP_GAMEPLAY_SHOPS) || (paad && paad->item_type != AP_SHOP_ITEM_TYPE)) {
        ap_stock_func_menu_80026B28(paad, moves, intro_flag);
        return;
    }
    if (!paad || !ap_apshop_context()) return;
    if (!ap_apshop_committed(paad)) {
        // A rejected purchase must not display a success explanation or mark
        // an intro flag. Return to selection after its animation has ended.
        if (func_global_asm_80629148()) func_menu_80026804(paad, moves);
        return;
    }
    switch (paad->substate) {
        case 0:
            if (!func_global_asm_80629148()) return;
            global_properties_bitfield |= 0x10030;
            gPlayerPointer->object_properties_bitfield &= ~0x40000000u;
            func_global_asm_80629174();
            loadText(gCurrentActorPointer, AP_SHOP_TEXT_FILE, 2);
            paad->substate = 1;
            break;
        case 1:
            if (gCurrentActorPointer->object_properties_bitfield & 0x02000000u) return;
            // Caller-supplied stock intro flags only, never AP shop flags.
            if ((gCurrentActorPointer->unk58 == 0xbd && (intro_flag == 0x177 || intro_flag == 0x17f))
                || (gCurrentActorPointer->unk58 == 0xbe && intro_flag == 0x175)
                || (gCurrentActorPointer->unk58 == 0xbf && intro_flag == 0x176)) setFlag(intro_flag, 1, FLAG_TYPE_PERMANENT);
            func_global_asm_8061CB08();
            paad->substate = 2;
            break;
        case 2:
            if (func_global_asm_80629148()) func_menu_80026804(paad, moves);
            break;
    }
}
