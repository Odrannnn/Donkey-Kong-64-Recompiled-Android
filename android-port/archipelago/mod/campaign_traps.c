// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "modding.h"
#include "gameplay_mode.h"
#include "campaign_traps.h"

#define AP_TRAP_FRAMES 240u
#define AP_ICE_FLOOR_FRAMES 450u
#define AP_MAP_CAVES_BEETLE_RACE 82
#define AP_CONT_A 0x8000u
#define AP_CONT_B 0x4000u
#define AP_CONT_Z 0x2000u
#define AP_CONT_C_UP 0x0008u

typedef struct {
    u8 pad000[0x13C];
    void* collision_queue_pointer;
    u8 pad140[0x154 - 0x140];
    u8 control_state;
    u8 pad155[0x1A6 - 0x155];
    s16 traction;
    u8 pad1A8[0x248 - 0x1A8];
    s16 shockwave_timer;
    u8 pad24A[0x370 - 0x24A];
    u32 strong_kong_ostand_bitfield;
} ap_trap_player;
_Static_assert(__builtin_offsetof(ap_trap_player, collision_queue_pointer) == 0x13C, "Player collision offset");
_Static_assert(__builtin_offsetof(ap_trap_player, control_state) == 0x154, "Player control-state offset");
_Static_assert(__builtin_offsetof(ap_trap_player, traction) == 0x1A6, "Player traction offset");
_Static_assert(__builtin_offsetof(ap_trap_player, shockwave_timer) == 0x248, "Player shockwave offset");
_Static_assert(__builtin_offsetof(ap_trap_player, strong_kong_ostand_bitfield) == 0x370, "Player state-bit offset");

typedef struct {
    s16 standard_ammo, homing_ammo, oranges, crystals, film;
    u8 unknown_0A, health, melons, stored_damage;
    s16 instrument_energy;
} ap_collectable_base;
_Static_assert(sizeof(ap_collectable_base) == 0x10, "CollectableBase layout");
_Static_assert(__builtin_offsetof(ap_collectable_base, instrument_energy) == 0x0E, "Collectable energy offset");

typedef struct {
    u8 pad00[8];
    u16 instrument_energy;
    u8 pad0A[0x5E - 0x0A];
} ap_trap_character_progress;
_Static_assert(sizeof(ap_trap_character_progress) == 0x5E, "CharacterProgress trap layout");

extern volatile u32 ap_gameplay_mode;
extern ap_trap_player* gPlayerPointer;
extern s32 object_timer, current_map;
extern s8 is_autowalking;
extern f32 D_global_asm_807FD888;
extern ap_collectable_base D_global_asm_807FCC40;
extern ap_trap_character_progress D_global_asm_807FC950[];
extern void ap_stock_func_global_asm_80712944(OSContPad* pads);

static u16 button_timers[4];
static u16 ice_floor_timer;

static int button_slot(unsigned type) {
    if (type == AP_TRAP_TYPE_DISABLE_A) return 0;
    if (type == AP_TRAP_TYPE_DISABLE_B) return 1;
    if (type == AP_TRAP_TYPE_DISABLE_Z) return 2;
    if (type == AP_TRAP_TYPE_DISABLE_C_UP) return 3;
    return -1;
}

static unsigned button_bit(unsigned slot) {
    static const u16 bits[4] = {AP_CONT_A, AP_CONT_B, AP_CONT_Z, AP_CONT_C_UP};
    return slot < 4 ? bits[slot] : 0;
}

int ap_campaign_trap_executable(unsigned type) {
    return button_slot(type) >= 0 || type == AP_TRAP_TYPE_DRY || type == AP_TRAP_TYPE_ICEFLOOR;
}

static void reset_ice_floor(void) {
    if (gPlayerPointer) gPlayerPointer->traction = current_map == AP_MAP_CAVES_BEETLE_RACE ? 20 : 100;
    ice_floor_timer = 0;
}

int ap_campaign_trap_can_start(unsigned type) {
    if (!(ap_gameplay_mode & AP_GAMEPLAY_TRAPS) || !ap_campaign_trap_executable(type)
        || !gPlayerPointer || gPlayerPointer->collision_queue_pointer || object_timer < 5
        || D_global_asm_807FD888 > 15.0f
        || (gPlayerPointer->strong_kong_ostand_bitfield & 0x100u) || is_autowalking
        || gPlayerPointer->shockwave_timer != -1 || current_map < 0
        || (unsigned)current_map >= AP_TRAP_MAP_COUNT
        || ap_trap_map_state[current_map] != AP_TRAP_MAP_ALLOW
        || gPlayerPointer->control_state >= AP_TRAP_MOVEMENT_COUNT
        || ap_trap_movement_banned[gPlayerPointer->control_state]) return 0;
    return 1;
}

int ap_campaign_trap_start(unsigned type) {
    if (!ap_campaign_trap_can_start(type)) return 0;
    int slot = button_slot(type);
    if (slot >= 0) {
        button_timers[slot] = AP_TRAP_FRAMES;
    } else if (type == AP_TRAP_TYPE_DRY) {
        D_global_asm_807FCC40.standard_ammo = 0;
        D_global_asm_807FCC40.homing_ammo = 0;
        D_global_asm_807FCC40.oranges = 0;
        D_global_asm_807FCC40.crystals = 0;
        D_global_asm_807FCC40.film = 0;
        D_global_asm_807FCC40.instrument_energy = 0;
        for (unsigned kong = 0; kong < 5; ++kong)
            D_global_asm_807FC950[kong].instrument_energy = 0;
    } else {
        gPlayerPointer->traction = 1;
        ice_floor_timer = AP_ICE_FLOOR_FRAMES;
    }
    return 1;
}

unsigned ap_campaign_trap_button_mask(void) {
    unsigned mask = 0xFFFFu;
    for (unsigned slot = 0; slot < 4; ++slot)
        if (button_timers[slot]) mask &= ~button_bit(slot);
    return mask;
}

void ap_campaign_trap_tick(void) {
    if (!(ap_gameplay_mode & AP_GAMEPLAY_TRAPS)) {
        ap_campaign_trap_reset();
        return;
    }
    for (unsigned slot = 0; slot < 4; ++slot)
        if (button_timers[slot]) --button_timers[slot];
    if (ice_floor_timer && !--ice_floor_timer) reset_ice_floor();
}

void ap_campaign_trap_reset(void) {
    for (unsigned slot = 0; slot < 4; ++slot) button_timers[slot] = 0;
    if (ice_floor_timer) reset_ice_floor();
}

RECOMP_CALLBACK("*", dk64recomp_every_frame) void ap_campaign_trap_frame(void) {
    ap_campaign_trap_tick();
}

RECOMP_PATCH void func_global_asm_80712944(OSContPad* pads) {
    ap_stock_func_global_asm_80712944(pads);
    if (!(ap_gameplay_mode & AP_GAMEPLAY_TRAPS)) return;
    u16 mask = (u16)ap_campaign_trap_button_mask();
    for (unsigned controller = 0; controller < 4; ++controller)
        pads[controller].button &= mask;
}
