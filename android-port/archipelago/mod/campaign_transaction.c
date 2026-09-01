// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "campaign_transaction.h"
#include "campaign_save.h"
#include "campaign_traps.h"

extern ap_uint8_t current_file;
extern ap_uint8_t game_mode, game_mode_copy, cc_number_of_players;
extern volatile ap_uint32_t ap_gameplay_mode;
typedef struct {
    ap_uint8_t moves, simian_slam, weapon, ammo_belt, instrument;
    ap_uint8_t unknown_05;
    ap_uint16_t coins, instrument_energy;
    ap_uint16_t coloured_bananas[14], coloured_bananas_fed_to_tns[14];
    short golden_bananas[14];
} ap_character_progress;
typedef struct {
    ap_character_progress character_progress[6];
    ap_uint8_t rest[0x2FC - 6 * 0x5E];
    ap_uint8_t melons;
    ap_uint8_t tail[0x306 - 0x2FD];
} ap_player_progress;
_Static_assert(sizeof(ap_character_progress) == 0x5E, "CharacterProgress layout");
_Static_assert(AP_OFFSETOF(ap_character_progress, coins) == 0x06, "CharacterProgress coins offset");
_Static_assert(AP_OFFSETOF(ap_character_progress, golden_bananas) == 0x42, "CharacterProgress GB offset");
_Static_assert(sizeof(ap_player_progress) == 0x306, "PlayerProgress layout");
_Static_assert(AP_OFFSETOF(ap_player_progress, melons) == 0x2FC, "PlayerProgress melons offset");
extern ap_player_progress D_global_asm_807FC950[];
extern void func_global_asm_8060DEC8(void);
extern void setFlag(short flag, ap_uint8_t value, ap_uint8_t type);

static ap_campaign_transaction published;
static ap_campaign_save published_identity;
static ap_uint8_t published_ready, published_has_item;

static int aligned(const void* pointer) { return !((ap_uint32_t)pointer & 3u); }
static int supported(const ap_campaign_save* save) {
    return save && aligned(save) && save->magic == AP_CAMPAIGN_SAVE_MAGIC
        && save->format == AP_CAMPAIGN_SAVE_FORMAT && ap_campaign_save_valid(save);
}
static int same_identity(const ap_campaign_save* a, const ap_campaign_save* b) {
    if (a->magic != b->magic || a->format != b->format || a->counter != b->counter
        || a->reserved != b->reserved || a->checksum != b->checksum) return 0;
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i)
        if (a->binding[i] != b->binding[i]) return 0;
    return 1;
}
static int same_bytes(const void* a, const void* b, unsigned size) {
    const ap_uint8_t* left = (const ap_uint8_t*)a;
    const ap_uint8_t* right = (const ap_uint8_t*)b;
    for (unsigned i = 0; i < size; ++i) if (left[i] != right[i]) return 0;
    return 1;
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
static unsigned effect_class(const ap_item_effect* effect) {
    switch (effect->kind) {
        case AP_EFFECT_NONE: return 0;
        case AP_EFFECT_FLAG: case AP_EFFECT_KONG: case AP_EFFECT_KEY: case AP_EFFECT_ABILITY:
        case AP_EFFECT_SPECIAL: case AP_EFFECT_HINT: return AP_TRANSACTION_OWNERSHIP;
        case AP_EFFECT_BLUEPRINT: case AP_EFFECT_FAIRY: case AP_EFFECT_CROWN: case AP_EFFECT_MEDAL:
        case AP_EFFECT_PEARL: case AP_EFFECT_RAINBOW: case AP_EFFECT_JUNK: return AP_TRANSACTION_COUNTED;
        case AP_EFFECT_TRAP: return AP_TRANSACTION_TRAP;
        case AP_EFFECT_TRANSFER:
            return effect->arg0 == 1 ? AP_TRANSACTION_COUNTED : AP_TRANSACTION_OWNERSHIP;
        default: return 0x100u;
    }
}

static int executable_item(const ap_item_effect_def* item) {
    if (!item) return 0;
    for (unsigned i = 0; i < 2; ++i) {
        const ap_item_effect* effect = &item->effects[i];
        switch (effect->kind) {
            case AP_EFFECT_NONE:
            case AP_EFFECT_KONG:
            case AP_EFFECT_KEY:
            case AP_EFFECT_ABILITY:
            case AP_EFFECT_SPECIAL:
            case AP_EFFECT_HINT:
            case AP_EFFECT_BLUEPRINT:
            case AP_EFFECT_FAIRY:
            case AP_EFFECT_CROWN:
            case AP_EFFECT_MEDAL:
            case AP_EFFECT_PEARL:
            case AP_EFFECT_RAINBOW:
            case AP_EFFECT_JUNK:
            case AP_EFFECT_FLAG:
                break;
            case AP_EFFECT_TRAP:
                if (!ap_campaign_trap_executable(effect->arg0)) return 0;
                break;
            case AP_EFFECT_TRANSFER:
                break;
            default:
                return 0;
        }
    }
    return 1;
}

int ap_campaign_transaction_build(ap_campaign_transaction* output, const ap_campaign_save* identity,
    const ap_campaign_inventory* before, ap_uint32_t item_id) {
    if (!output || !aligned(output) || !before || !aligned(before) || !supported(identity)
        || identity->counter == 0xFFFFu) return 0;
    const ap_item_effect_def* item = find_item(item_id);
    if (!item) return 0;
    unsigned classes = 0;
    for (unsigned i = 0; i < 2; ++i) {
        unsigned value = effect_class(&item->effects[i]);
        if (value & 0x100u) return 0;
        classes |= value;
    }
    if (!classes) classes = AP_TRANSACTION_NOOP;
    ap_campaign_transaction candidate;
    candidate.magic = AP_CAMPAIGN_TRANSACTION_MAGIC;
    candidate.format = AP_CAMPAIGN_TRANSACTION_FORMAT;
    candidate.classes = (ap_uint16_t)classes;
    candidate.item_id = item_id;
    candidate.reserved = 0;
    candidate.current = *identity;
    candidate.next = *identity;
    candidate.next.counter++;
    ap_campaign_save_seal(&candidate.next);
    candidate.before = *before;
    candidate.after = *before;
    if (ap_campaign_inventory_add(&candidate.after, item_id) != 1) return 0;
    *output = candidate;
    return 1;
}

int ap_campaign_transaction_stage(const ap_campaign_save* identity, const ap_campaign_inventory* before,
    ap_uint32_t item_id, unsigned has_item) {
    ap_campaign_save trailer;
    ap_campaign_transaction candidate;
    if (!supported(identity) || !before || !aligned(before) || has_item > 1 || current_file != 0
        || ap_gameplay_mode != 0 || ap_campaign_trailer_read(&trailer) != 1
        || !same_identity(identity, &trailer)) return 0;
    if (has_item && !ap_campaign_transaction_build(&candidate, identity, before, item_id)) return 0;
    if (has_item) published = candidate;
    published_identity = *identity;
    published_has_item = (ap_uint8_t)has_item;
    published_ready = 1;
    return 1;
}

int ap_campaign_transaction_snapshot(ap_campaign_transaction* output, const ap_campaign_save* identity) {
    ap_campaign_save trailer;
    if (!output || !aligned(output) || !supported(identity) || current_file != 0 || !published_ready
        || !same_identity(identity, &published_identity) || ap_campaign_trailer_read(&trailer) != 1
        || !same_identity(identity, &trailer)) return 0;
    if (!published_has_item) return 2;
    *output = published;
    return 1;
}

static int executable_only(const ap_campaign_inventory* before, const ap_campaign_inventory* after) {
    ap_campaign_inventory left = *before, right = *after;
    for (unsigned kong = 0; kong < 5; ++kong) {
        left.moves[kong] = right.moves[kong] = 0;
        left.guns[kong] = right.guns[kong] = 0;
        left.instruments[kong] = right.instruments[kong] = 0;
    }
    left.slam = right.slam = 0;
    left.belt = right.belt = 0;
    left.instrument_upgrades = right.instrument_upgrades = 0;
    left.abilities = right.abilities = 0;
    left.kongs = right.kongs = 0;
    left.keys = right.keys = 0;
    for (unsigned i = 0; i < AP_ITEM_FLAG_WORDS; ++i) left.flags[i] = right.flags[i] = 0;
    left.climbing = right.climbing = 0;
    left.shopkeepers = right.shopkeepers = 0;
    left.special = right.special = 0;
    for (unsigned kong = 0; kong < 5; ++kong) left.hints[kong] = right.hints[kong] = 0;
    for (unsigned kong = 0; kong < 5; ++kong) left.blueprints[kong] = right.blueprints[kong] = 0;
    left.golden_bananas = right.golden_bananas = 0;
    left.fairies = right.fairies = 0;
    left.crowns = right.crowns = 0;
    left.medals = right.medals = 0;
    left.pearls = right.pearls = 0;
    left.rainbow_coins = right.rainbow_coins = 0;
    left.junk = right.junk = 0;
    for (unsigned i = 0; i < AP_TRAP_TYPE_COUNT; ++i) left.traps[i] = right.traps[i] = 0;
    return same_bytes(&left, &right, sizeof(left));
}

static void give_balanced_golden_banana(void) {
    short* target = &D_global_asm_807FC950[0].character_progress[0].golden_bananas[0];
    int minimum = 99999;
    for (unsigned kong = 0; kong < 5; ++kong) {
        for (unsigned level = 0; level < 9; ++level) {
            short* candidate = &D_global_asm_807FC950[0].character_progress[kong].golden_bananas[level];
            if (*candidate < minimum) { target = candidate; minimum = *candidate; }
        }
    }
    *target = (short)(*target + 1);
}

int ap_campaign_transaction_commit(const ap_campaign_transaction* source) {
    ap_campaign_transaction transaction, staged;
    if (!source || !aligned(source) || current_file != 0 || cc_number_of_players != 1
        || game_mode != GAME_MODE_ADVENTURE || game_mode_copy != GAME_MODE_ADVENTURE
        || (ap_gameplay_mode & (AP_GAMEPLAY_MOVES | AP_GAMEPLAY_SAVE | AP_GAMEPLAY_ITEMS))
            != (AP_GAMEPLAY_MOVES | AP_GAMEPLAY_SAVE | AP_GAMEPLAY_ITEMS)) return 0;
    transaction = *source;
    const ap_item_effect_def* item = find_item(transaction.item_id);
    if (transaction.magic != AP_CAMPAIGN_TRANSACTION_MAGIC
        || transaction.format != AP_CAMPAIGN_TRANSACTION_FORMAT || transaction.reserved != 0
        || (transaction.classes & ~(AP_TRANSACTION_NOOP | AP_TRANSACTION_OWNERSHIP | AP_TRANSACTION_COUNTED | AP_TRANSACTION_TRAP))
        || !transaction.classes || !supported(&transaction.current) || !supported(&transaction.next)
        || transaction.current.counter == 0xFFFFu
        || transaction.next.counter != (unsigned)transaction.current.counter + 1u
        || !same_identity(&transaction.current, &published_identity)
        || !executable_item(item)
        || !executable_only(&transaction.before, &transaction.after)
        || ap_campaign_transaction_snapshot(&staged, &transaction.current) != 1
        || !same_bytes(&transaction, &staged, sizeof(transaction))) return 0;
    if ((transaction.classes & AP_TRANSACTION_TRAP) && !(ap_gameplay_mode & AP_GAMEPLAY_TRAPS)) return 0;
    for (unsigned i = 0; i < 2; ++i)
        if (item->effects[i].kind == AP_EFFECT_TRAP
            && !ap_campaign_trap_can_start(item->effects[i].arg0)) return 0;
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i)
        if (transaction.current.binding[i] != transaction.next.binding[i]) return 0;

    // The trailer is staged first in the volatile save image. Everything after
    // this point is infallible game-thread assignment followed by the stock save
    // entry point, which persists CharacterProgress and the same image together.
    if (!ap_campaign_trailer_stage(&transaction.next)) return 0;
    for (unsigned i = 0; i < 2; ++i)
        if (item->effects[i].kind == AP_EFFECT_TRAP)
            (void)ap_campaign_trap_start(item->effects[i].arg0);
    for (unsigned i = 0; i < 2; ++i) if (item->effects[i].kind == AP_EFFECT_FLAG) {
        unsigned flag = item->effects[i].arg1;
        unsigned word = flag >> 5, bit = 1u << (flag & 31u);
        if (!(transaction.before.flags[word] & bit) && (transaction.after.flags[word] & bit))
            setFlag((short)flag, 1, FLAG_TYPE_PERMANENT);
    }
    if (transaction.after.golden_bananas > transaction.before.golden_bananas)
        give_balanced_golden_banana();
    if (transaction.after.rainbow_coins > transaction.before.rainbow_coins)
        for (unsigned kong = 0; kong < 5; ++kong)
            D_global_asm_807FC950[0].character_progress[kong].coins += 5;
    unsigned melons = 0;
    for (unsigned kong = 0; kong < 5; ++kong) {
        ap_character_progress* progress = &D_global_asm_807FC950[0].character_progress[kong];
        progress->moves = (progress->moves & ~7u) | transaction.after.moves[kong];
        progress->simian_slam = transaction.after.slam;
        progress->weapon = (progress->weapon & ~7u) | transaction.after.guns[kong];
        progress->ammo_belt = transaction.after.belt;
        progress->instrument = (progress->instrument & ~15u) | transaction.after.instruments[kong];
        if (transaction.after.instruments[kong] & 3u) melons = melons < 2 ? 2 : melons;
        if (transaction.after.instruments[kong] & 12u) melons = 3;
    }
    if (D_global_asm_807FC950[0].melons < melons) D_global_asm_807FC950[0].melons = melons;
    func_global_asm_8060DEC8();
    published_ready = 0; // A new native stage must bind the advanced counter.
    return 1;
}

void ap_campaign_transaction_reset(void) {
    ap_uint8_t* bytes = (ap_uint8_t*)&published;
    for (unsigned i = 0; i < sizeof(published); ++i) bytes[i] = 0;
    bytes = (ap_uint8_t*)&published_identity;
    for (unsigned i = 0; i < sizeof(published_identity); ++i) bytes[i] = 0;
    published_ready = published_has_item = 0;
}
