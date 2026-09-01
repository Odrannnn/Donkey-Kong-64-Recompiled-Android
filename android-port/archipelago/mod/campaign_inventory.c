// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_inventory.h"

static ap_uint32_t increment(ap_uint32_t value) { return value == 0xFFFFFFFFu ? value : value + 1u; }
static ap_uint8_t capped_increment(ap_uint8_t value, ap_uint8_t cap) { return value < cap ? value + 1u : value; }

void ap_campaign_inventory_clear(ap_campaign_inventory* inventory) {
    if (!inventory) return;
    ap_uint8_t* bytes = (ap_uint8_t*)inventory;
    for (unsigned i = 0; i < sizeof(*inventory); ++i) bytes[i] = 0;
}

static const ap_item_effect_def* find_item(ap_uint32_t item_id) {
    unsigned low = 0, high = AP_ITEM_COUNT;
    while (low < high) {
        unsigned middle = low + (high - low) / 2;
        if (ap_item_effects[middle].id < item_id) low = middle + 1;
        else high = middle;
    }
    return low < AP_ITEM_COUNT && ap_item_effects[low].id == item_id ? &ap_item_effects[low] : 0;
}

static int effect_valid(const ap_item_effect* effect) {
    switch (effect->kind) {
        case AP_EFFECT_UNSUPPORTED: case AP_EFFECT_NONE: case AP_EFFECT_FAIRY: case AP_EFFECT_CROWN:
        case AP_EFFECT_MEDAL: case AP_EFFECT_PEARL: case AP_EFFECT_RAINBOW: case AP_EFFECT_JUNK: return 1;
        case AP_EFFECT_FLAG: return effect->arg1 < AP_ITEM_FLAG_WORDS * 32u;
        case AP_EFFECT_KONG: return effect->arg0 < 5;
        case AP_EFFECT_KEY: return effect->arg0 < 8;
        case AP_EFFECT_ABILITY: return effect->arg0 && !(effect->arg0 & 3u);
        case AP_EFFECT_SPECIAL: return effect->arg0 && !(effect->arg0 & ~7u);
        case AP_EFFECT_HINT: return effect->arg0 < 5 && effect->arg1 < 8;
        case AP_EFFECT_BLUEPRINT: return effect->arg0 < 5;
        case AP_EFFECT_TRAP: return effect->arg0 < AP_TRAP_TYPE_COUNT;
        case AP_EFFECT_TRANSFER:
            return effect->arg0 == 1 || effect->arg0 == 2 || (effect->arg0 >= 26 && effect->arg0 <= 55);
        default: return 0;
    }
}

static void apply_transfer(ap_campaign_inventory* inventory, unsigned transfer) {
    if (transfer == 1) inventory->golden_bananas = increment(inventory->golden_bananas);
    else if (transfer == 2 || transfer == 51) inventory->slam = capped_increment(inventory->slam, 3);
    else if (transfer >= 26 && transfer <= 40)
        inventory->moves[(transfer - 26) / 3] |= (ap_uint8_t)(1u << ((transfer - 26) % 3));
    else if (transfer >= 41 && transfer <= 45) inventory->instruments[transfer - 41] |= 1;
    else if (transfer >= 46 && transfer <= 50) inventory->guns[transfer - 46] |= 1;
    else if (transfer == 52 || transfer == 53)
        for (unsigned kong = 0; kong < 5; ++kong) inventory->guns[kong] |= (ap_uint8_t)(1u << (transfer - 51));
    else if (transfer == 54) inventory->belt = capped_increment(inventory->belt, 2);
    else if (transfer == 55) {
        inventory->instrument_upgrades = capped_increment(inventory->instrument_upgrades, 3);
        ap_uint8_t mask = (ap_uint8_t)(((1u << (inventory->instrument_upgrades + 1u)) - 1u) & ~1u);
        for (unsigned kong = 0; kong < 5; ++kong) inventory->instruments[kong] |= mask;
    }
}

static void apply_effect(ap_campaign_inventory* inventory, const ap_item_effect* effect) {
    switch (effect->kind) {
        case AP_EFFECT_NONE: break;
        case AP_EFFECT_FLAG:
            inventory->flags[effect->arg1 >> 5] |= 1u << (effect->arg1 & 31);
            if (effect->arg1 == 671) inventory->climbing = 1;
            if (effect->arg1 >= 962 && effect->arg1 <= 965) inventory->shopkeepers |= (ap_uint8_t)(1u << (effect->arg1 - 962));
            break;
        case AP_EFFECT_KONG: inventory->kongs |= (ap_uint8_t)(1u << effect->arg0); break;
        case AP_EFFECT_KEY: inventory->keys |= (ap_uint8_t)(1u << effect->arg0); break;
        case AP_EFFECT_ABILITY: inventory->abilities |= effect->arg0; break;
        case AP_EFFECT_SPECIAL: inventory->special |= effect->arg0; break;
        case AP_EFFECT_HINT: inventory->hints[effect->arg0] |= (ap_uint8_t)(1u << effect->arg1); break;
        case AP_EFFECT_BLUEPRINT:
            inventory->blueprints[effect->arg0] = capped_increment(inventory->blueprints[effect->arg0], 8); break;
        case AP_EFFECT_FAIRY: inventory->fairies = increment(inventory->fairies); break;
        case AP_EFFECT_CROWN: inventory->crowns = increment(inventory->crowns); break;
        case AP_EFFECT_MEDAL: inventory->medals = increment(inventory->medals); break;
        case AP_EFFECT_PEARL: inventory->pearls = increment(inventory->pearls); break;
        case AP_EFFECT_RAINBOW: inventory->rainbow_coins = increment(inventory->rainbow_coins); break;
        case AP_EFFECT_JUNK: inventory->junk = increment(inventory->junk); break;
        case AP_EFFECT_TRAP: inventory->traps[effect->arg0] = increment(inventory->traps[effect->arg0]); break;
        case AP_EFFECT_TRANSFER: apply_transfer(inventory, effect->arg0); break;
    }
}

int ap_campaign_inventory_add(ap_campaign_inventory* inventory, ap_uint32_t item_id) {
    if (!inventory) return -1;
    const ap_item_effect_def* item = find_item(item_id);
    if (!item) return -1;
    for (unsigned i = 0; i < 2; ++i) {
        if (!effect_valid(&item->effects[i])) return -1;
        if (item->effects[i].kind == AP_EFFECT_UNSUPPORTED) return 0;
    }
    for (unsigned i = 0; i < 2; ++i) apply_effect(inventory, &item->effects[i]);
    return 1;
}

int ap_campaign_inventory_rebuild(ap_campaign_inventory* output, const ap_uint32_t* item_ids, unsigned count) {
    if (!output || (count && !item_ids) || count > 65535u) return -1;
    ap_campaign_inventory candidate;
    ap_campaign_inventory_clear(&candidate);
    for (unsigned i = 0; i < count; ++i) {
        int result = ap_campaign_inventory_add(&candidate, item_ids[i]);
        if (result != 1) return result;
    }
    *output = candidate;
    return 1;
}

int ap_campaign_inventory_has_flag(const ap_campaign_inventory* inventory, unsigned flag) {
    return inventory && flag < AP_ITEM_FLAG_WORDS * 32u && !!(inventory->flags[flag >> 5] & (1u << (flag & 31)));
}
