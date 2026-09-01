// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_items.h"
#include "campaign_save.h"

extern ap_uint8_t current_file;
extern volatile ap_uint32_t ap_gameplay_mode;
extern ap_uint8_t isFlagSet(short flag, ap_uint8_t type);

static ap_campaign_inventory published;
static ap_campaign_save published_identity;
static ap_uint8_t published_ready;

static int same_identity(const ap_campaign_save* a, const ap_campaign_save* b) {
    if (a->magic != b->magic || a->format != b->format || a->counter != b->counter
        || a->reserved != b->reserved || a->checksum != b->checksum) return 0;
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i)
        if (a->binding[i] != b->binding[i]) return 0;
    return 1;
}

static int active(void) {
    ap_campaign_save trailer;
    return published_ready && current_file == 0 && (ap_gameplay_mode & AP_GAMEPLAY_ITEMS)
        && ap_campaign_trailer_read(&trailer) == 1 && same_identity(&published_identity, &trailer);
}

static int capped(ap_uint32_t value) { return value > 0x7FFFFFFFu ? 0x7FFFFFFF : (int)value; }
static int popcount(unsigned value) {
    int count = 0;
    while (value) { count += value & 1u; value >>= 1; }
    return count;
}

int ap_campaign_items_stage(const ap_campaign_save* identity) {
    ap_campaign_inventory candidate;
    if (!identity || current_file != 0 || ap_gameplay_mode != 0
        || ap_campaign_inventory_snapshot(&candidate, identity) != 1) return 0;
    published = candidate;
    published_identity = *identity;
    published_ready = 1;
    return 1;
}

int ap_campaign_item_count(unsigned item, int level, int kong) {
    if (!active()) return -1;
    switch (item) {
        case AP_ITEM_QUERY_KONG:
            if (level != -1 && level != 0) return -1;
            if (kong == -1) return popcount(published.kongs & 31u);
            return kong >= 0 && kong < 5 ? !!(published.kongs & (1u << kong)) : -1;
        case AP_ITEM_QUERY_KEY:
            if (kong != -1 && kong != 0) return -1;
            if (level == -1) return popcount(published.keys);
            return level >= 0 && level < 8 ? !!(published.keys & (1u << level)) : -1;
        case AP_ITEM_QUERY_HINT: {
            if (level < -1 || level >= 8 || kong < -1 || kong >= 5) return -1;
            int count = 0;
            for (int k = 0; k < 5; ++k) if (kong == -1 || kong == k)
                for (int l = 0; l < 8; ++l) if ((level == -1 || level == l) && (published.hints[k] & (1u << l))) ++count;
            return count;
        }
        case AP_ITEM_QUERY_BLUEPRINT:
            if (level != -1 && level != 0) return -1;
            if (kong < -1 || kong >= 5) return -1;
            if (kong >= 0) return published.blueprints[kong];
            return published.blueprints[0] + published.blueprints[1] + published.blueprints[2]
                + published.blueprints[3] + published.blueprints[4];
        case AP_ITEM_QUERY_FAIRY: return level == 0 && kong == 0 ? capped(published.fairies) : -1;
        case AP_ITEM_QUERY_CROWN: return level == 0 && kong == 0 ? capped(published.crowns) : -1;
        case AP_ITEM_QUERY_COMPANY_COIN:
            if (level != 0 || kong < -1 || kong > 1) return -1;
            if (kong == 0) return !!(published.special & 1u);
            if (kong == 1) return !!(published.special & 2u);
            return !!(published.special & 1u) + !!(published.special & 2u);
        case AP_ITEM_QUERY_MEDAL: return level == 0 && kong == 0 ? capped(published.medals) : -1;
        case AP_ITEM_QUERY_BEAN: return level == 0 && kong == 0 ? !!(published.special & 4u) : -1;
        case AP_ITEM_QUERY_PEARL: return level == 0 && kong == 0 ? capped(published.pearls) : -1;
        case AP_ITEM_QUERY_RAINBOW_COIN: return level == 0 && kong == 0 ? capped(published.rainbow_coins) : -1;
        case AP_ITEM_QUERY_TRAP: {
            if (level != -1 && (level < 0 || level >= AP_TRAP_TYPE_COUNT)) return -1;
            if (kong != -1 && kong != 0) return -1;
            if (level >= 0) return capped(published.traps[level]);
            ap_uint32_t total = 0;
            for (unsigned i = 0; i < AP_TRAP_TYPE_COUNT; ++i) {
                ap_uint32_t next = total + published.traps[i];
                total = next < total ? 0xFFFFFFFFu : next;
            }
            return capped(total);
        }
        case AP_ITEM_QUERY_JUNK: return level == 0 && kong == 0 ? capped(published.junk) : -1;
        case AP_ITEM_QUERY_MOVE:
            if (level >= 0 && level < 3)
                return kong >= 0 && kong < 5 ? !!(published.moves[kong] & (1u << level)) : -1;
            if (level == 3) return kong == 0 ? published.slam : -1;
            if (level == 4) return kong >= 0 && kong < 5 ? !!(published.guns[kong] & 1u) : -1;
            if (level == 5 || level == 6) return kong == 0 ? !!(published.guns[0] & (1u << (level - 4))) : -1;
            if (level == 7) return kong == 0 ? published.belt : -1;
            if (level == 8) return kong >= 0 && kong < 5 ? !!(published.instruments[kong] & 1u) : -1;
            if (level == 9) return kong == 0 ? published.instrument_upgrades : -1;
            if (level == 10) return kong >= 0 && kong < 6 ? !!(published.abilities & (0x80u >> kong)) : -1;
            if (level == 11) return kong == 0 ? !!published.climbing : -1;
            if (level == 12) return kong == 0 ? (published.abilities & 12u) == 12u : -1;
            return -1;
        case AP_ITEM_QUERY_GOLDEN_BANANA:
            return level == 0 && kong == 0 ? capped(published.golden_bananas) : -1;
        case AP_ITEM_QUERY_SHOPKEEPER:
            if (level != 0) return -1;
            if (kong == -1) return popcount(published.shopkeepers & 15u);
            return kong >= 0 && kong < 4 ? !!(published.shopkeepers & (1u << kong)) : -1;
        default: return -1;
    }
}

int ap_campaign_item_has_flag(unsigned flag) {
    return active() ? ap_campaign_inventory_has_flag(&published, flag) : -1;
}

static int gameplay_count(unsigned item, int level, int kong) {
    int value = ap_campaign_item_count(item, level, kong);
    return value < 0 ? 0 : value;
}

int ap_campaign_original_item_count(int item, int level, int kong) {
    // requirement_item values pinned by the original randomizer's
    // common_enums.h. Normalize selectors whose original implementation
    // ignored unused arguments.
    switch (item) {
        case 1: return gameplay_count(AP_ITEM_QUERY_KONG, 0, kong);
        case 2:
            switch (level) {
                case 3:  // Slam level
                case 5:  // Homing ammo
                case 6:  // Sniper scope
                case 7:  // Ammo belt level
                case 9:  // Instrument upgrade level
                case 11: // Climbing
                case 12: // Camera and Shockwave
                    kong = 0;
                    break;
                default:
                    break;
            }
            return gameplay_count(AP_ITEM_QUERY_MOVE, level, kong);
        case 3: return gameplay_count(AP_ITEM_QUERY_GOLDEN_BANANA, 0, 0);
        case 4: return gameplay_count(AP_ITEM_QUERY_BLUEPRINT, 0, kong);
        case 5: return gameplay_count(AP_ITEM_QUERY_FAIRY, 0, 0);
        case 6: return gameplay_count(AP_ITEM_QUERY_KEY, level, 0);
        case 7: return gameplay_count(AP_ITEM_QUERY_CROWN, 0, 0);
        case 8: return gameplay_count(AP_ITEM_QUERY_COMPANY_COIN, 0, kong);
        case 9: return gameplay_count(AP_ITEM_QUERY_MEDAL, 0, 0);
        case 10: return gameplay_count(AP_ITEM_QUERY_BEAN, 0, 0);
        case 11: return gameplay_count(AP_ITEM_QUERY_PEARL, 0, 0);
        case 12: return gameplay_count(AP_ITEM_QUERY_RAINBOW_COIN, 0, 0);
        case 13: return gameplay_count(AP_ITEM_QUERY_TRAP, -1, 0);
        case 18: return gameplay_count(AP_ITEM_QUERY_JUNK, 0, 0);
        case 19: return gameplay_count(AP_ITEM_QUERY_HINT, level, kong);
        case 20: return gameplay_count(AP_ITEM_QUERY_SHOPKEEPER, 0, kong);
        default: return 0;
    }
}

int ap_campaign_get_kong_ownership_from_flag(int flag) {
    static const short flags[5] = {0x181, 0x006, 0x046, 0x042, 0x075};
    for (int kong = 0; kong < 5; ++kong)
        if (flag == flags[kong]) return gameplay_count(AP_ITEM_QUERY_KONG, 0, kong);
    return 0;
}

int ap_campaign_has_flag_move(int flag) {
    int selector = -1;
    switch (flag) {
        case 0x182: selector = 0; break; // Diving
        case 0x184: selector = 1; break; // Oranges
        case 0x185: selector = 2; break; // Barrels
        case 0x183: selector = 3; break; // Vines
        case 0x2FD: selector = 4; break; // Decoupled camera
        case 0x179: selector = 5; break; // Shockwave
        default: break;
    }
    if (!(ap_gameplay_mode & AP_GAMEPLAY_ITEMS))
        return flag == 0x2FD ? 0 : isFlagSet((short)flag, 0);
    if (selector < 0) return isFlagSet((short)flag, 0);
    return ap_campaign_item_count(AP_ITEM_QUERY_MOVE, 10, selector) > 0;
}

void ap_campaign_items_reset(void) {
    ap_campaign_inventory_clear(&published);
    ap_uint8_t* bytes = (ap_uint8_t*)&published_identity;
    for (unsigned i = 0; i < sizeof(published_identity); ++i) bytes[i] = 0;
    published_ready = 0;
}
