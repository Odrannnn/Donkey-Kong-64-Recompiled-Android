// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "ap_shops.h"

extern volatile u32 ap_gameplay_mode;
extern u8 game_mode, game_mode_copy, current_file, current_character_index[];
extern u32 current_map;
extern Actor *gCurrentActorPointer, *gPlayerPointer;
extern u16 D_global_asm_80750AC8;
extern u8 D_global_asm_807FC950[];
extern u8 isFlagSet(s16 flag, u8 type);
extern u8 getLevelIndex(u8 map, u8 mode);
extern s32 func_global_asm_806F8AD4(s32 resource, u8 player);
extern void func_global_asm_8060DEC8(void);

static ap_check_set shop_active, shop_observed;
static u16 shop_prices[AP_SHOP_COUNT];
static u8 shop_prepared;
static ap_shop_label shop_labels[AP_SHOP_COUNT];
static unsigned int shop_generation;

static int shop_index(u32 id) {
    for (u32 i = 0; i < AP_SHOP_COUNT; ++i) if (ap_shop_locations[i].id == id) return i;
    return -1;
}
static int subset(const ap_check_set* part, const ap_check_set* whole) {
    if (!part || !ap_check_valid(part)) return 0;
    for (u32 i = 0; i < AP_LOCATION_WORDS; ++i) if (part->words[i] & ~whole->words[i]) return 0;
    return 1;
}
int ap_prepare_ap_shops(const ap_shop_offer* offers, unsigned int count, const ap_check_set* restored) {
    if (ap_gameplay_mode || count > AP_SHOP_COUNT || (count && !offers)) return 0;
    ap_check_set active = {0};
    for (u32 i = 0; i < count; ++i) {
        const ap_shop_offer* offer = &offers[i];
        if (shop_index(offer->location) < 0 || offer->price > 255 || offer->reserved || ap_check_has(&active, offer->location)) return 0;
        ap_check_add(&active, offer->location);
    }
    if (!ap_check_distinct_flags(&active) || !subset(restored, &active)) return 0;
    // Publish only after all input has been validated. No partial new seed.
    for (u32 i = 0; i < AP_SHOP_COUNT; ++i) shop_prices[i] = 0;
    for (u32 i = 0; i < count; ++i) shop_prices[shop_index(offers[i].location)] = offers[i].price;
    shop_active = active; shop_observed = *restored; shop_prepared = 1;
    for (u32 i = 0; i < AP_SHOP_COUNT; ++i) shop_labels[i].location = 0;
    if (!++shop_generation) ++shop_generation;
    return 1;
}
int ap_apshop_restore(const ap_check_set* restored) {
    if (!shop_prepared || !subset(restored, &shop_active)) return 0;
    // Synchronization can add durable checks, never reopen purchases by rollback.
    for (u32 i = 0; i < AP_LOCATION_WORDS; ++i) shop_observed.words[i] |= restored->words[i];
    return 1;
}
void ap_apshop_snapshot(ap_check_set* result) { if (result) *result = shop_observed; }

static int owner_context(Actor* owner, u32* vendor, u32* level, u32* kong) {
    if (!(ap_gameplay_mode & AP_GAMEPLAY_SHOPS) || !shop_prepared || current_file != 0
        || game_mode != GAME_MODE_ADVENTURE || game_mode_copy != GAME_MODE_ADVENTURE
        || !owner || !gPlayerPointer) return 0;
    *vendor = owner->unk58 - 0xBD;
    *kong = current_character_index[0];
    if (*vendor >= 3 || *kong >= 5) return 0;
    *level = getLevelIndex((u8)current_map, 0);
    return *level < 8;
}
static int context(u32* vendor, u32* level, u32* kong) {
    return owner_context(gCurrentActorPointer, vendor, level, kong);
}
unsigned int ap_apshop_generation(void) { return shop_generation; }
int ap_apshop_set_labels(const ap_shop_label* labels, unsigned int count) {
    if (!shop_prepared || count > AP_SHOP_COUNT || (count && !labels)) return 0;
    ap_check_set seen = {0};
    for (u32 i = 0; i < count; ++i) {
        if (!ap_label_valid(&labels[i]) || !ap_check_has(&shop_active, labels[i].location)
            || ap_check_has(&seen, labels[i].location)) return 0;
        ap_check_add(&seen, labels[i].location);
    }
    // A session snapshot replaces the previous one. In particular, an empty
    // reconnect snapshot must remove names from the old room.
    for (u32 i = 0; i < AP_SHOP_COUNT; ++i) shop_labels[i].location = 0;
    for (u32 i = 0; i < count; ++i) shop_labels[shop_index(labels[i].location)] = labels[i];
    return 1;
}
int ap_apshop_display(void* owner, const ap_shop_paad* paad, ap_shop_label* result) {
    u32 vendor, level, kong;
    if (!paad || !result || !owner_context(owner, &vendor, &level, &kong)
        || paad->item_type != AP_SHOP_ITEM_TYPE || paad->level != level || paad->kong != kong) return 0;
    for (u32 i = 0; i < AP_SHOP_COUNT; ++i) {
        const ap_shop_location_def* def = &ap_shop_locations[i];
        if (def->vendor == vendor && def->level == level && (def->kong == kong || def->kong == AP_SHOP_SHARED)
            && ap_check_has(&shop_active, def->id) && ap_check_has(&shop_observed, def->id)
            && paad->price == shop_prices[i] && paad->flag == ap_locations[ap_location_index(def->id)].flag) {
            if (shop_labels[i].location) *result = shop_labels[i];
            else {
                const ap_shop_label pending = {0, 130, 0, "ARCHIPELAGO CHECK", "ITEM DETAILS PENDING", {0}};
                *result = pending; result->location = def->id;
            }
            return 1;
        }
    }
    return 0;
}
static int selected(u32 vendor, u32 level, u32 kong) {
    for (u32 i = 0; i < AP_SHOP_COUNT; ++i) {
        const ap_shop_location_def* def = &ap_shop_locations[i];
        if (def->vendor == vendor && def->level == level && (def->kong == kong || def->kong == AP_SHOP_SHARED)
            && ap_check_has(&shop_active, def->id) && !ap_check_has(&shop_observed, def->id)) return i;
    }
    return -1;
}
int ap_apshop_context(void) {
    u32 vendor, level, kong;
    return context(&vendor, &level, &kong);
}
int ap_apshop_committed(const ap_shop_paad* paad) {
    u32 vendor, level, kong;
    if (!paad || !context(&vendor, &level, &kong) || paad->item_type != AP_SHOP_ITEM_TYPE
        || paad->level != level || paad->kong != kong) return 0;
    for (u32 i = 0; i < AP_SHOP_COUNT; ++i) {
        const ap_shop_location_def* def = &ap_shop_locations[i];
        if (def->vendor == vendor && def->level == level && (def->kong == kong || def->kong == AP_SHOP_SHARED)
            && ap_check_has(&shop_active, def->id) && ap_check_has(&shop_observed, def->id)
            && paad->price == shop_prices[i] && paad->flag == ap_locations[ap_location_index(def->id)].flag) return 1;
    }
    return 0;
}
static int quote(const ap_shop_paad* paad) {
    u32 vendor, level, kong;
    if (!paad || !context(&vendor, &level, &kong)) return -1;
    int index = selected(vendor, level, kong);
    if (index < 0) return -1;
    const ap_location_def* def = &ap_locations[ap_location_index(ap_shop_locations[index].id)];
    // Recheck context and the quote at payment time: a stale PAAD from another
    // Kong/world must not report the current context's different AP location.
    if (paad->item_type != AP_SHOP_ITEM_TYPE || paad->level != level || paad->kong != kong
        || paad->price != shop_prices[index] || paad->flag != def->flag) return -1;
    return index;
}
void ap_apshop_select(ap_shop_paad* paad, void* moves, int first_visit) {
    (void)moves; (void)first_visit;
    if (!paad) return;
    paad->item_type = -1; paad->price = 0; paad->flag = 0xffff; paad->item_level = 0;
    paad->kong = current_character_index[0]; paad->melons = D_global_asm_807FC950[0x2fc];
    D_global_asm_80750AC8 = 0;
    u32 vendor, level, kong;
    if (!context(&vendor, &level, &kong)) return;
    paad->level = level;
    int index = selected(vendor, level, kong);
    if (index < 0) {
        // Original AP lookup does not require having entered a level to buy.
        // The final level-entered flag only changes the exhausted-shop dialogue.
        if (isFlagSet(0x1cb, FLAG_TYPE_PERMANENT)) paad->item_type = -2;
        return;
    }
    paad->item_type = AP_SHOP_ITEM_TYPE;
    paad->price = (u8)shop_prices[index];
    paad->flag = ap_locations[ap_location_index(ap_shop_locations[index].id)].flag;
    D_global_asm_80750AC8 = paad->price;
}
int ap_apshop_afford(ap_shop_paad* paad, int unused) {
    (void)unused;
    if (!paad) return 0;
    int result = paad->item_type == -2 ? 1 : 0;
    paad->response = 3;
    if (quote(paad) >= 0) {
        result = 3;
        if (func_global_asm_806F8AD4(1, 0) >= paad->price) { result = 2; paad->response = 1; }
    }
    // Stock dialogue distinguishes a first visit from a subsequent offer.
    if (((u8*)paad)[0xa]) result += 4;
    return result;
}
void ap_apshop_purchase(ap_shop_paad* paad, void* moves) {
    (void)moves;
    int index = quote(paad);
    if (index < 0 || func_global_asm_806F8AD4(1, 0) < paad->price) return;
    // AP uses cumulative price thresholds. Neither deduct coins, grant a move,
    // refill resources, nor write shop flags into the stock EEPROM flag block.
    // The receipt path is the sole owner of received items, including local ones.
    ap_check_add(&shop_observed, ap_shop_locations[index].id);
    func_global_asm_8060DEC8();
}
