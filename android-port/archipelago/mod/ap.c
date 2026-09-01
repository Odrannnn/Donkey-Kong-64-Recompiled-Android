// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "modding.h"
#include "recompconfig.h"
#include "recomputils.h"
#include "policy.h"
#include "checks.h"
#include "ap_shops.h"

RECOMP_IMPORT(".", u32 dk64_ap_configure_v2(const u32* config, char* save_name));
RECOMP_IMPORT(".", u32 dk64_ap_start_v2(const char* save_path));
RECOMP_IMPORT(".", u32 dk64_ap_tick_v2(u32 checks));
RECOMP_IMPORT(".", u32 dk64_ap_labels_v2(ap_shop_label* labels, u32 capacity));
RECOMP_IMPORT(".", void dk64_ap_stop_v2(void));
extern u8 game_mode, game_mode_copy, current_file, D_global_asm_807467E0;
extern volatile u32 ap_gameplay_mode;
extern u8 D_global_asm_8076A0B1;
extern u32 global_properties_bitfield;
extern f32 D_global_asm_807FD888;
extern s8 is_cutscene_active;
extern Actor* gPlayerPointer;
extern u8 isFlagSet(s16 flag, u8 type);
// First five CharacterProgress entries, validated against the decomp structs.h.
extern struct { u8 moves; u8 rest[0x5D]; } D_global_asm_807FC950[];
extern void addActorToTextOverlayRenderArray(void* callback, Actor* actor, u8 layer);
extern Gfx* printStyledText(Gfx* dl, s16 font, s16 x, s16 y, u8* text, u32 flags);
static u32 initialized, enabled, status = 3, error, frames;
static char save_name[36];
static ap_check_set active_checks, observed_checks;
static ap_shop_label native_shop_labels[AP_SHOP_COUNT];
static void fail(void) {
    enabled = 0; status = 2; dk64_ap_stop_v2();
    recomp_printf("[dk64-ap] Disabled: invalid configuration, save ownership or companion. Normal save data was not modified by this mod.\n");
}
static int owns_save(void) {
    unsigned char* path = recomp_get_save_file_path();
    int okay = ap_owns_save_path(path, save_name);
    recomp_free(path);
    return okay;
}
RECOMP_CALLBACK("*", recomp_on_init) void ap_init(void) {
    if (initialized) return;
    initialized = 1;
    if (recomp_get_config_u32("enabled") != 1) return;
    if (D_global_asm_807467E0) { fail(); return; }
    char* server = recomp_get_config_string("server");
    char* name = recomp_get_config_string("slot_name");
    char* password = recomp_get_config_string("password");
    char* seed = recomp_get_config_string("seed");
    u32 config[6] = {(u32)server, (u32)name, (u32)password, (u32)seed,
        recomp_get_config_u32("team"), recomp_get_config_u32("slot")};
    if (dk64_ap_configure_v2(config, save_name)) {
        recomp_change_save_file(save_name);
        unsigned char* path = recomp_get_save_file_path();
        if (ap_owns_save_path(path, save_name)) status = dk64_ap_start_v2((const char*)path);
        recomp_free(path);
        enabled = status == 0;
    }
    recomp_free_config_string(server); recomp_free_config_string(name);
    recomp_free_config_string(password); recomp_free_config_string(seed);
    if (!enabled) fail();
    else {
        ap_check_add(&active_checks, 14041180);
        ap_check_add(&active_checks, 14041181);
        recomp_printf("[dk64-ap] Integration TEST profile. Use save slot 1. Vanilla rewards remain; ordinary randomizer seeds are unsupported.\n");
    }
}
static Gfx* draw_status(Gfx* dl, Actor* unused) {
    const char* text = status == 1 ? "AP DIRECT TEST: CONNECTED" : "AP DIRECT TEST: CONNECTING";
    if (status == 0 && error == 7) text = "AP TEST: NETWORK / TLS RETRY";
    if (status == 2) {
        text = error == 2 ? "AP TEST: WRONG SEED" : error == 3 ? "AP TEST: CHECK SLOT / PASSWORD" :
            error == 4 ? "AP TEST: UNSUPPORTED WORLD" : error == 5 ? "AP TEST: SAVE JOURNAL ERROR" : "AP TEST: CONFIG / PROTOCOL ERROR";
    }
    if (current_file != 0) text = "AP TEST: USE SAVE 1";
    return printStyledText(dl, 1, 24, 30, (u8*)text, 0);
}
RECOMP_CALLBACK("*", dk64recomp_every_frame) void ap_frame(void) {
    if (status == 3) return;
    addActorToTextOverlayRenderArray(draw_status, NULL, 5);
    if (!enabled || ++frames % 6) return;
    if (!owns_save()) { fail(); return; }
    int safe = ap_safe_frame(game_mode == GAME_MODE_ADVENTURE && game_mode_copy == GAME_MODE_ADVENTURE,
        current_file, gPlayerPointer != NULL, is_cutscene_active != 0,
        (D_global_asm_8076A0B1 & 1) || D_global_asm_807FD888 != 0.0f,
        global_properties_bitfield & 3);
    u32 checks = 0;
    if (safe) {
        for (unsigned i = 0; i < AP_LOCATION_COUNT; ++i) {
            if (ap_check_has_index(&active_checks, i) && ap_locations[i].detector == AP_DETECT_STOCK_FLAG
                && isFlagSet(ap_locations[i].flag, FLAG_TYPE_PERMANENT))
                ap_check_event(&observed_checks, &active_checks, ap_locations[i].id);
        }
        // ABI v2 remains deliberately limited to the validated test profile.
        checks = ap_check_has(&observed_checks, 14041180) ? 1 : 0;
        checks |= ap_check_has(&observed_checks, 14041181) ? 2 : 0;
    }
    u32 response = dk64_ap_tick_v2(checks);
    status = (response >> 16) & 3;
    error = response >> 24;
    if (ap_gameplay_mode & AP_GAMEPLAY_SHOPS) {
        u32 label_count = dk64_ap_labels_v2(native_shop_labels, AP_SHOP_COUNT);
        if (label_count != 0xFFFFFFFEu
            && (label_count > AP_SHOP_COUNT || !ap_apshop_set_labels(native_shop_labels, label_count))) { fail(); return; }
    }
    if (!safe || status != 1) return;
    for (unsigned kong = 0; kong < 5; ++kong) {
        u8 old = D_global_asm_807FC950[kong].moves;
        u8 next = ap_first_move_level(old, (response >> kong) & 1);
        if (old != next) {
            D_global_asm_807FC950[kong].moves = next;
            recomp_printf("[dk64-ap] Granted first special move for Kong %u.\n", kong);
        }
    }
}
