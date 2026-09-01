// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "modding.h"
#include "gameplay_mode.h"

// Internal capability gate, not a configuration option or an exported mod API.
// Only a future validated campaign/save handshake may set these bits. The direct
// integration-test profile deliberately leaves them zero, including on reconnect.
volatile u32 ap_gameplay_mode;
extern u8 game_mode, game_mode_copy, current_file;
extern void ap_stock_changeCollectableCount(s32 item, u8 player, s16 amount);
// HUDDisplay layout from the pinned decomp; only the collection-disable byte is needed.
typedef struct { u32 unused[11]; u8 blocked, rest[3]; } ap_hud_entry;
_Static_assert(sizeof(ap_hud_entry) == 0x30, "HUDDisplay stride");
extern ap_hud_entry* func_global_asm_806F7FD0(s32 player);
extern s32 func_global_asm_805FEF10(s32* flag);
extern void setFlag(s16 flag, u8 value, u8 type);
extern void func_global_asm_806F8278(u8 player);

RECOMP_PATCH void changeCollectableCount(s32 item, u8 player, s16 amount) {
    // A checked banana belongs to the location ledger, not received inventory.
    // Received GB totals will be reconciled from receipts; they must not use this
    // vanilla collection path. Other resources, spending, HUD updates and off-mode
    // behavior continue through the byte-preserved, relocated stock function.
    if ((ap_gameplay_mode & AP_GAMEPLAY_REWARDS) && current_file == 0 && player == 0
        && game_mode == GAME_MODE_ADVENTURE && game_mode_copy == GAME_MODE_ADVENTURE
        && item == 8 && amount > 0) {
        // Calling stock with amount zero still clamps its counter to the vanilla
        // cap (100). Preserve the location/HUD effects explicitly so collecting a
        // location cannot reduce a larger AP inventory either.
        ap_hud_entry* hud = func_global_asm_806F7FD0(player);
        if (!hud[item].blocked) {
            s32 flag;
            if (func_global_asm_805FEF10(&flag)) setFlag(flag, 1, FLAG_TYPE_PERMANENT);
            func_global_asm_806F8278(player);
        }
        return;
    }
    ap_stock_changeCollectableCount(item, player, amount);
}
