// Run the same-area game adapter against a recording engine boundary. This
// proves that network records can only touch reviewed loaded script objects and
// can only align an already-running matching cutscene by one camera phase.
#include "../mod/transient_types.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>

using u8 = unsigned char; using s8 = signed char; using u16 = unsigned short;
using s16 = short; using u32 = unsigned; using s32 = int; using f32 = float;
enum { ROLE_HOST = 1, ROLE_JOIN = 2 };

struct Prop_ScriptData { s16 unk44[2]{}; u8 unk48[1]{}; };
struct Prop { Prop_ScriptData* unk7C{}; };

static unsigned checks, script_calls, last_object, last_state;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "TRANSIENT ADAPTER FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)

static u32 role = ROLE_HOST, current_file, current_map = 7, epoch = 9;
static u32 transient_enabled = 1, transient_revision = 1, transient_page;
static u32 transient_file, transient_file_changed;
static CoopTransientInput transient_input{};
static CoopTransientResult transient_result{};
static f32 loading_zone_transition_speed;
static s8 is_cutscene_active;
static s16 D_global_asm_807476F8 = -1;
static u16 D_global_asm_807F5CF0, D_global_asm_807F5CF4;
static s16 D_global_asm_807F6240[600];
static std::array<Prop, 512> props;
static std::array<Prop_ScriptData, 512> scripts;
static Prop* D_global_asm_807F6000 = props.data();

static s16 func_global_asm_80659470(s32 object) {
    return object >= 0 && object < static_cast<s32>(scripts.size()) ? static_cast<s16>(object) : -1;
}
static void func_global_asm_8063DA40(s16 slot, s16 state) {
    CHECK(slot >= 0 && slot < 600);
    unsigned object = static_cast<unsigned short>(D_global_asm_807F6240[slot]);
    CHECK(object < scripts.size() && props[object].unk7C == &scripts[object]);
    scripts[object].unk48[0] = static_cast<u8>(state);
    ++script_calls; last_object = object; last_state = static_cast<unsigned short>(state);
}

#include "../mod/world_live_game.h"
#include "../mod/transient_game.h"

static void reset() {
    std::fill(std::begin(D_global_asm_807F6240), std::end(D_global_asm_807F6240), static_cast<s16>(-1));
    props = {}; scripts = {};
    role = ROLE_HOST; current_file = 0; current_map = 7; epoch = 9;
    transient_enabled = 1; transient_revision = 1; transient_page = 0;
    transient_file = transient_file_changed = 0;
    coop_transient_applied_timer_epoch = 0;
    std::fill(std::begin(coop_transient_applied_timers), std::end(coop_transient_applied_timers), CoopTransientRecord{});
    transient_input = {}; transient_result = {};
    loading_zone_transition_speed = 0; is_cutscene_active = 0;
    D_global_asm_807476F8 = -1; D_global_asm_807F5CF0 = D_global_asm_807F5CF4 = 0;
    script_calls = last_object = last_state = 0;
}
static void load(unsigned slot, unsigned object, unsigned state, unsigned timer = 0) {
    CHECK(slot < 600 && object < scripts.size());
    D_global_asm_807F6240[slot] = static_cast<s16>(object);
    scripts[object].unk48[0] = static_cast<u8>(state);
    scripts[object].unk44[0] = static_cast<s16>(timer);
    props[object].unk7C = &scripts[object];
}
static bool contains(unsigned kind, unsigned key, unsigned state) {
    for (unsigned i = 0; i < transient_input.count; ++i) {
        const auto& record = transient_input.records[i];
        if (record.kind == kind && record.key == key && record.state == state) return true;
    }
    return false;
}
static bool contains_value(unsigned kind, unsigned key, unsigned state, unsigned value) {
    for (unsigned i = 0; i < transient_input.count; ++i) {
        const auto& record = transient_input.records[i];
        if (record.kind == kind && record.key == key && record.state == state
                && record.value == value) return true;
    }
    return false;
}

static void capture_checks() {
    reset();
    load(0, 0x1A, 2); load(1, 0x1B, 3); load(2, 0x34, 4);
    coop_transient_capture(1);
    CHECK(transient_input.enabled && transient_input.map == 7 && transient_input.epoch == 9);
    CHECK(transient_file == 1 && !transient_file_changed);
    CHECK(contains(COOP_TRANSIENT_SCRIPT, 0x1A, 2));
    CHECK(contains(COOP_TRANSIENT_SCRIPT, 0x1B, 3));
    CHECK(!contains(COOP_TRANSIENT_SCRIPT, 0x34, 4));
    load(3, 0x30, 1); load(4, 0x31, 2); load(5, 0x32, 3);
    load(6, 0x34, 1); load(7, 0x35, 2);
    load(8, 0x28, 1); load(9, 0x29, 2); load(10, 0x2A, 3); load(11, 0x123, 2);
    bool saw_japes_30 = false, saw_japes_31 = false, saw_japes_32 = false;
    bool saw_japes_34 = false, saw_japes_35 = false;
    bool saw_japes_28 = false, saw_japes_29 = false, saw_japes_2a = false, saw_japes_123 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_japes_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 1, 2);
        saw_japes_31 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 2, 2);
        saw_japes_32 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x32, 2, 2);
        saw_japes_34 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x34, 1, 2);
        saw_japes_35 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x35, 2, 2);
        saw_japes_28 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x28, 1, 2);
        saw_japes_29 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x29, 2, 2);
        saw_japes_2a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2A, 2, 2);
        saw_japes_123 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x123, 2, 2);
    }
    CHECK(saw_japes_30 && saw_japes_31 && saw_japes_32);
    CHECK(saw_japes_34 && saw_japes_35);
    CHECK(saw_japes_28 && saw_japes_29 && saw_japes_2a && saw_japes_123);

    reset(); current_map = 7;
    load(0, 0x38, 1); load(1, 0x39, 2); load(2, 0x3A, 1);
    load(3, 0x3B, 20); load(4, 0x115, 2);
    bool saw_hut_38 = false, saw_hut_39 = false, saw_hut_3a = false;
    bool saw_hut_3b = false, saw_rambi_wall = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_hut_38 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x38, 1, 2);
        saw_hut_39 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x39, 2, 2);
        saw_hut_3a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3A, 1, 2);
        saw_hut_3b |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3B, 1, 2);
        saw_rambi_wall |= contains_value(COOP_TRANSIENT_TRIGGER, 0x115, 2, 2);
    }
    CHECK(saw_hut_38 && saw_hut_39 && saw_hut_3a && saw_hut_3b && saw_rambi_wall);

    reset(); current_map = 7;
    load(0, 0x1F, 1); load(1, 0x20, 11); load(2, 0x58, 2);
    bool saw_feather = false, saw_grape = false, saw_peanut = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_feather |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1F, 1, 11);
        saw_grape |= contains_value(COOP_TRANSIENT_TRIGGER, 0x20, 2, 11);
        saw_peanut |= contains_value(COOP_TRANSIENT_TRIGGER, 0x58, 2, 2);
    }
    CHECK(saw_feather && saw_grape && saw_peanut);

    reset(); current_map = 38; load(0, 0x0D, 2); load(1, 0x0E, 6); load(2, 0x0F, 10);
    load(3, 0x44, 3); load(4, 0x9D, 1); load(5, 0x9E, 2);
    bool saw_aztec_d = false, saw_aztec_e = false, saw_aztec_f = false;
    bool saw_aztec_guitar = false, saw_aztec_blueprint_d = false, saw_aztec_blueprint_e = false;
    for (unsigned page = 0; page < 4; ++page) {
        coop_transient_capture(1);
        saw_aztec_d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0D, 2, 2);
        saw_aztec_e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2);
        saw_aztec_f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0F, 1, 2);
        saw_aztec_guitar |= contains_value(COOP_TRANSIENT_TRIGGER, 0x44, 2, 2);
        saw_aztec_blueprint_d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x9D, 1, 2);
        saw_aztec_blueprint_e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x9E, 2, 2);
    }
    CHECK(saw_aztec_d && saw_aztec_e && saw_aztec_f);
    CHECK(saw_aztec_guitar);
    CHECK(saw_aztec_blueprint_d && saw_aztec_blueprint_e);
    CHECK(!contains(COOP_TRANSIENT_SCRIPT, 0x9D, 1));

    reset(); current_map = 30; load(0, 0, 10, 123); load(1, 1, 6, 45);
    bool saw_timer_0 = false, saw_timer_1 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_timer_0 |= contains_value(COOP_TRANSIENT_TIMER, 0, 10, 123);
        saw_timer_1 |= contains_value(COOP_TRANSIENT_TIMER, 1, 6, 45);
    }
    CHECK(saw_timer_0 && saw_timer_1);

    reset(); current_map = 48;
    load(0, 0x18, 1); load(1, 0x19, 2); load(2, 0x1A, 1);
    load(3, 0x1B, 2); load(4, 0x1E, 3);
    bool saw_fungi_18 = false, saw_fungi_19 = false, saw_fungi_1a = false;
    bool saw_fungi_1b = false, saw_fungi_1e = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_fungi_18 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x18, 1, 2);
        saw_fungi_19 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x19, 2, 2);
        saw_fungi_1a |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1A, 1, 2);
        saw_fungi_1b |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1B, 2, 2);
        saw_fungi_1e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1E, 2, 2);
    }
    CHECK(saw_fungi_18 && saw_fungi_19 && saw_fungi_1a && saw_fungi_1b && saw_fungi_1e);
    reset(); current_map = 48; load(0, 0x0F, 2); load(1, 0xEB, 1);
    bool saw_fungi_night = false, saw_fungi_mushroom = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_fungi_night |= contains_value(COOP_TRANSIENT_TRIGGER, 0x0F, 2, 2);
        saw_fungi_mushroom |= contains_value(COOP_TRANSIENT_TRIGGER, 0xEB, 1, 2);
    }
    CHECK(saw_fungi_night && saw_fungi_mushroom);

    reset(); current_map = 56; load(0, 0x00, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 2, 2));
    reset(); current_map = 58; load(0, 0x00, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 1, 2));
    reset(); current_map = 57; load(0, 0x03, 11);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x03, 2, 11));
    reset(); current_map = 59; load(0, 0x01, 2); load(1, 0x24, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x01, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x24, 1, 2));
    reset(); current_map = 61; load(0, 0x06, 11);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x06, 2, 11));
    reset(); current_map = 62; load(0, 0x00, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 2, 2));
    reset(); current_map = 17;
    load(0, 0x2C, 2); load(1, 0x2D, 11); load(2, 0x2E, 12);
    load(3, 0x2F, 13); load(4, 0x30, 11);
    bool saw_helm_2c = false, saw_helm_2d = false, saw_helm_2e = false;
    bool saw_helm_2f = false, saw_helm_30 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_helm_2c |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2C, 2, 2);
        saw_helm_2d |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2D, 1, 12);
        saw_helm_2e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2E, 2, 12);
        saw_helm_2f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 2, 12);
        saw_helm_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 1, 12);
    }
    CHECK(saw_helm_2c && saw_helm_2d && saw_helm_2e && saw_helm_2f && saw_helm_30);

    reset(); current_map = 173; load(0, 0x10, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x10, 2, 2));
    reset(); current_map = 178; load(0, 0x05, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 1, 2));

    reset(); current_map = 34; load(0, 0x31, 3);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 2, 3));
    reset(); current_map = 34; load(0, 0x33, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x33, 1, 3));

    reset(); current_map = 72; load(0, 0x2E, 2); load(1, 0x2F, 13);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x2E, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 2, 13));

    reset(); current_map = 90;
    load(0, 0x03, 1); load(1, 0x04, 2); load(2, 0x05, 20); load(3, 0x06, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x03, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 0, 0) == false);

    reset(); current_map = 108; load(0, 0x00, 2); load(1, 0x04, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x00, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));

    reset(); current_map = 112;
    load(0, 0x0D, 1); load(1, 0x0E, 2); load(2, 0x0F, 3);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0D, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0F, 2, 2));

    reset(); current_map = 163;
    load(0, 0x04, 1); load(1, 0x05, 2); load(2, 0x06, 7);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x06, 2, 2));

    for (unsigned map = 21; map <= 24; ++map) {
        reset(); current_map = map; load(0, 0x04, 1); load(1, 0x05, 2);
        if (map == 24) load(2, 0x07, 1);
        coop_transient_capture(1);
        CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));
        CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 2, 2));
        if (map == 24)
            CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x07, 1, 2));
    }

    reset(); current_map = 164; load(0, 0x01, 2); load(1, 0x09, 5);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x01, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x09, 2, 5));

    reset(); current_map = 26; load(0, 0x15, 0);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x15, 1, 1));
    scripts[0x15].unk48[0] = 1; transient_page = 0; coop_transient_capture(2);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x15, 2, 1));

    reset(); current_map = 4; load(0, 0x0A, 2); load(1, 0x0B, 10);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0A, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x0B, 1, 2));
    reset(); current_map = 4; load(0, 0x06, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x06, 2, 2));

    reset(); current_map = 30; load(0, 0x11, 1); load(1, 0x1C, 3);
    load(2, 6, 1); load(3, 7, 2); load(4, 8, 3); load(5, 9, 1);
    load(6, 0xA, 2); load(7, 0xB, 1);
    bool saw_bongo = false, saw_tiny_slam = false;
    bool saw_galleon_6 = false, saw_galleon_7 = false, saw_galleon_8 = false;
    bool saw_galleon_9 = false, saw_galleon_a = false, saw_galleon_b = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_bongo |= contains_value(COOP_TRANSIENT_TRIGGER, 0x11, 1, 2);
        saw_tiny_slam |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1C, 2, 2);
        saw_galleon_6 |= contains_value(COOP_TRANSIENT_TRIGGER, 6, 1, 2);
        saw_galleon_7 |= contains_value(COOP_TRANSIENT_TRIGGER, 7, 2, 2);
        saw_galleon_8 |= contains_value(COOP_TRANSIENT_TRIGGER, 8, 2, 2);
        saw_galleon_9 |= contains_value(COOP_TRANSIENT_TRIGGER, 9, 1, 2);
        saw_galleon_a |= contains_value(COOP_TRANSIENT_TRIGGER, 0xA, 2, 2);
        saw_galleon_b |= contains_value(COOP_TRANSIENT_TRIGGER, 0xB, 1, 2);
    }
    CHECK(saw_bongo && saw_tiny_slam);
    CHECK(saw_galleon_6 && saw_galleon_7 && saw_galleon_8);
    CHECK(saw_galleon_9 && saw_galleon_a && saw_galleon_b);

    reset(); current_map = 19; load(0, 0x04, 1); load(1, 0x05, 2);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x04, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x05, 2, 2));

    reset(); current_map = 20; load(0, 0x12, 1); load(1, 0x16, 4);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x12, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x16, 2, 2));

    reset(); current_map = 20;
    for (unsigned object = 0x19; object <= 0x28; ++object)
        load(object - 0x19, object, object == 0x23 ? 12 : 11);
    bool saw_matching_ready = false, saw_matching_hit = false, saw_matching_last = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_matching_ready |= contains_value(COOP_TRANSIENT_TRIGGER, 0x19, 1, 12);
        saw_matching_hit |= contains_value(COOP_TRANSIENT_TRIGGER, 0x23, 2, 12);
        saw_matching_last |= contains_value(COOP_TRANSIENT_TRIGGER, 0x28, 1, 12);
    }
    CHECK(saw_matching_ready && saw_matching_hit && saw_matching_last);

    reset(); current_map = 20; load(0, 0x69, 2);
    bool saw_quicksand_switch = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_quicksand_switch |= contains_value(COOP_TRANSIENT_TRIGGER, 0x69, 2, 2);
    }
    CHECK(saw_quicksand_switch);

    reset(); current_map = 20; load(0, 0x6B, 1);
    bool saw_grape_ready = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_grape_ready |= contains_value(COOP_TRANSIENT_TRIGGER, 0x6B, 1, 2);
    }
    CHECK(saw_grape_ready);
    scripts[0x6B].unk48[0] = 2; transient_page = 0;
    bool saw_grape_hit = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_grape_hit |= contains_value(COOP_TRANSIENT_TRIGGER, 0x6B, 2, 2);
    }
    CHECK(saw_grape_hit);

    reset(); current_map = 16; load(0, 0, 1); load(1, 4, 2); load(2, 0x14, 3);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 4, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x14, 2, 2));

    reset(); current_map = 16;
    load(0, 0x0C, 1); load(1, 0x0D, 1); load(2, 0x0E, 10); load(3, 0x0F, 22);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x0C, 1, 0));
    scripts[0x0C].unk48[0] = 20; coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x0C, 4, 0));

    reset(); current_map = 194; load(0, 6, 2);
    coop_transient_capture(1);
    CHECK(contains(COOP_TRANSIENT_PLATFORM, 6, 2));

    reset(); current_map = 26;
    load(0, 0x2E, 1); load(1, 0x2F, 2); load(2, 0x30, 3); load(3, 0x31, 20);
    load(4, 0x24, 2);
    bool saw_2e = false, saw_2f = false, saw_30 = false, saw_31 = false, saw_24 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_2e |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2E, 1, 2);
        saw_2f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 2, 2);
        saw_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 2, 2);
        saw_31 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 1, 2);
        saw_24 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x24, 2, 2);
    }
    CHECK(saw_2e && saw_2f && saw_30 && saw_31 && saw_24);

    reset(); current_map = 26;
    load(0, 0x37, 1); load(1, 0x38, 2); load(2, 0x3B, 3);
    bool saw_factory_triangle = false, saw_factory_guitar = false, saw_factory_trombone = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_factory_triangle |= contains_value(COOP_TRANSIENT_TRIGGER, 0x37, 1, 2);
        saw_factory_guitar |= contains_value(COOP_TRANSIENT_TRIGGER, 0x38, 2, 2);
        saw_factory_trombone |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3B, 2, 2);
    }
    CHECK(saw_factory_triangle && saw_factory_guitar && saw_factory_trombone);

    reset(); current_map = 26;
    load(0, 0x3F, 4); load(1, 0x40, 5); load(2, 0x41, 10);
    bool saw_3f = false, saw_40 = false, saw_41 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_3f |= contains_value(COOP_TRANSIENT_TRIGGER, 0x3F, 1, 5);
        saw_40 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x40, 2, 5);
        saw_41 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x41, 2, 5);
    }
    CHECK(saw_3f && saw_40 && saw_41);

    reset(); current_map = 26; load(0, 0x14, 31);
    bool saw_piano = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_piano |= contains_value(COOP_TRANSIENT_SEQUENCE, 0x14, 9, 0);
    }
    CHECK(saw_piano);
    scripts[0x14].unk48[0] = 250; transient_page = 0; saw_piano = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_piano |= contains_value(COOP_TRANSIENT_SEQUENCE, 0x14, 0, 0);
    }
    CHECK(saw_piano); // Failure/restart states never masquerade as completion.

    reset(); current_map = 26; load(0, 0x7F, 55);
    bool saw_dartboard = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_dartboard |= contains_value(COOP_TRANSIENT_SEQUENCE, 0x7F, 3, 0);
    }
    CHECK(saw_dartboard);

    reset(); current_map = 84; load(0, 0x00, 1);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 0, 0));
    scripts[0x00].unk48[0] = 6; transient_page = 0;
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 2, 0));
    scripts[0x00].unk48[0] = 30; transient_page = 0;
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 4, 0));
    scripts[0x00].unk48[0] = 50; transient_page = 0;
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_SEQUENCE, 0x00, 0, 0));

    reset(); load(0, 0x1A, 2); loading_zone_transition_speed = 1;
    coop_transient_capture(1); CHECK(!transient_input.enabled);

    reset(); load(0, 0x1A, 2); coop_transient_capture(1);
    current_file = 1; coop_transient_capture(1);
    CHECK(transient_file_changed && !transient_input.enabled);
}

static void object_apply_checks() {
    reset(); role = ROLE_JOIN; load(0, 0x1A, 2);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_SCRIPT, 0x1A, 20, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x1A && last_state == 20 && scripts[0x1A].unk48[0] == 20);

    transient_result.records[0] = {COOP_TRANSIENT_SCRIPT, 0x99, 7, 0};
    coop_transient_apply(); CHECK(script_calls == 1); // Arbitrary object.
    transient_result.records[0] = {COOP_TRANSIENT_TIMER, 0x1A, 7, 0};
    coop_transient_apply(); CHECK(script_calls == 1); // Wrong reviewed kind.
    transient_result.records[0] = {COOP_TRANSIENT_SCRIPT, 0x1A, 7, 0};
    transient_result.epoch = 8; coop_transient_apply(); CHECK(script_calls == 1);
    transient_result.epoch = 9; role = ROLE_HOST; coop_transient_apply(); CHECK(script_calls == 1);

    role = ROLE_JOIN; transient_file_changed = 1;
    transient_result.records[0] = {COOP_TRANSIENT_SCRIPT, 0x1A, 7, 0};
    coop_transient_apply(); CHECK(script_calls == 1); // Save-slot lock blocks apply too.

    reset(); role = ROLE_JOIN; load(0, 0x31, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x31, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x31 && last_state == 2);

    reset(); role = ROLE_JOIN; load(0, 0x35, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x35, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x35 && last_state == 2);

    reset(); role = ROLE_JOIN; load(0, 0x123, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x123, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x123 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x0E, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x0E && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x3A, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x3A, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x3A && last_state == 2);
    coop_transient_apply(); CHECK(script_calls == 1);
    scripts[0x3A].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // A completed hut switch never restarts.

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x115, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x115, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x115 && last_state == 2);
    scripts[0x115].unk48[0] = 0; coop_transient_apply();
    CHECK(script_calls == 1); // The packet cannot initialize the wall switch.

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x1F, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x1F, 2, 11}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x1F && last_state == 11);
    scripts[0x1F].unk48[0] = 10; coop_transient_apply();
    CHECK(script_calls == 1); // Intermediate states cannot satisfy exact readiness.

    reset(); role = ROLE_JOIN; current_map = 7; load(0, 0x58, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 7, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x58, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x58 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x44, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x44, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x44 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 38; load(0, 0x9D, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 38, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x9D, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x9D && last_state == 2);
    transient_result.records[0].key = 0x9F; coop_transient_apply();
    CHECK(script_calls == 1); // The linked door cannot be named as a trigger.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x31, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x31, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x31 && last_state == 2);
    coop_transient_apply(); CHECK(script_calls == 1); // Idempotent after activation.
    scripts[0x31].unk48[0] = 1; transient_result.records[0].state = 1;
    coop_transient_apply(); CHECK(script_calls == 1); // Ready never rewinds/triggers.
    transient_result.records[0] = {COOP_TRANSIENT_TRIGGER, 0x32, 2, 2};
    coop_transient_apply(); CHECK(script_calls == 1); // Arbitrary trigger rejected.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x24, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x24, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x24 && last_state == 2);
    scripts[0x24].unk48[0] = 0; coop_transient_apply();
    CHECK(script_calls == 1); // The peer cannot start an uninitialized cage switch.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x38, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x38, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x38 && last_state == 2);
    scripts[0x38].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // The local pad owns completion and reset timing.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x3F, 3);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x3F, 2, 5}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x3F && last_state == 5);
    scripts[0x3F].unk48[0] = 3; transient_result.records[0].value = 4;
    coop_transient_apply(); CHECK(script_calls == 1); // Wrong activation state rejected.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x14, 30);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x14, 10, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x14 && last_state == 31);
    coop_transient_apply(); CHECK(script_calls == 1); // Wait for local timer/state gate.
    scripts[0x14].unk48[0] = 32; coop_transient_apply();
    CHECK(script_calls == 2 && last_state == 33); // Catch up one more local note.
    scripts[0x14].unk48[0] = 34; transient_result.records[0].state = 9;
    coop_transient_apply(); CHECK(script_calls == 2); // Host never rewinds an ahead peer.
    transient_result.records[0].state = 26;
    coop_transient_apply(); CHECK(script_calls == 2); // Out-of-range progress rejected.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0, 3, 100);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TIMER, 0, 3, 75}}};
    coop_transient_apply();
    CHECK(!script_calls && scripts[0].unk44[0] == 75); // Same state still aligns timer.
    scripts[0].unk44[0] = 74; coop_transient_apply();
    CHECK(scripts[0].unk44[0] == 74); // Repeated render frame does not freeze countdown.
    transient_result.records[0] = {COOP_TRANSIENT_TIMER, 0, 4, 60};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_state == 4 && scripts[0].unk44[0] == 60);

    reset(); role = ROLE_JOIN; current_map = 48; load(0, 0x1A, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 48, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x1A, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x1A && last_state == 2);
    scripts[0x1A].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Permanent completion cannot be rewound.
    reset(); role = ROLE_JOIN; current_map = 48; load(0, 0xEB, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 48, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0xEB, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0xEB && last_state == 2);
    scripts[0xEB].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Saved completion cannot restart the sequence.

    reset(); role = ROLE_JOIN; current_map = 173; load(0, 0x10, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 173, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x10, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x10 && last_state == 2);
    transient_result.map = 178; coop_transient_apply();
    CHECK(script_calls == 1); // A lobby record cannot cross into another map.

    reset(); role = ROLE_JOIN; current_map = 34; load(0, 0x31, 2);
    transient_result = {COOP_TRANSIENT_APPLYING, 34, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x31, 2, 3}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x31 && last_state == 3);
    scripts[0x31].unk48[0] = 1; coop_transient_apply();
    CHECK(script_calls == 1); // Barrel dependency must have armed state 2 locally.

    reset(); role = ROLE_JOIN; current_map = 34; load(0, 0x33, 2);
    transient_result = {COOP_TRANSIENT_APPLYING, 34, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x33, 2, 3}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x33 && last_state == 3);
    scripts[0x33].unk48[0] = 1; coop_transient_apply();
    CHECK(script_calls == 1); // Local barrel initialization is mandatory.

    reset(); role = ROLE_JOIN; current_map = 72; load(0, 0x2F, 12);
    transient_result = {COOP_TRANSIENT_APPLYING, 72, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2F, 2, 13}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2F && last_state == 13);
    scripts[0x2F].unk48[0] = 11; coop_transient_apply();
    CHECK(script_calls == 1); // The large pad must finish its local reveal.
    reset(); role = ROLE_JOIN; current_map = 72; load(0, 0x2E, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 72, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2E, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2E && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 90;
    load(0, 0x03, 1); load(1, 0x04, 1); load(2, 0x05, 1); load(3, 0x06, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 90, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x04, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x04 && last_state == 2);
    CHECK(scripts[0x06].unk48[0] == 2);
    transient_result.records[0].key = 0x05; coop_transient_apply();
    CHECK(script_calls == 2 && scripts[0x06].unk48[0] == 3);
    scripts[0x06].unk48[0] = 4; transient_result.records[0].key = 0x03;
    coop_transient_apply();
    CHECK(script_calls == 2); // Completed controller cannot receive another hit.

    reset(); role = ROLE_JOIN; current_map = 56; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 56, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    reset(); role = ROLE_JOIN; current_map = 59; load(0, 0x24, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 59, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x24, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x24 && last_state == 2);
    scripts[0x24].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // Completed local box cannot be restarted.
    reset(); role = ROLE_JOIN; current_map = 61; load(0, 0x06, 10);
    transient_result = {COOP_TRANSIENT_APPLYING, 61, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x06, 2, 11}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x06 && last_state == 11);
    scripts[0x06].unk48[0] = 0; coop_transient_apply();
    CHECK(script_calls == 1); // The permanent pre-activation path stays local.
    reset(); role = ROLE_JOIN; current_map = 62; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 62, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    scripts[0x00].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // Completed pad path cannot be replayed.
    reset(); role = ROLE_JOIN; current_map = 17; load(0, 0x2C, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 17, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2C, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2C && last_state == 2);
    reset(); role = ROLE_JOIN; current_map = 17; load(0, 0x2F, 11);
    transient_result = {COOP_TRANSIENT_APPLYING, 17, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x2F, 2, 12}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x2F && last_state == 12);
    scripts[0x2F].unk48[0] = 10; coop_transient_apply();
    CHECK(script_calls == 1); // Local Helm pad setup must reach state 11.
    reset(); role = ROLE_JOIN; current_map = 58; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 58, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    reset(); role = ROLE_JOIN; current_map = 57; load(0, 0x03, 10);
    transient_result = {COOP_TRANSIENT_APPLYING, 57, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x03, 2, 11}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x03 && last_state == 11);
    scripts[0x03].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Saved completion cannot restart the winch.

    reset(); role = ROLE_JOIN; current_map = 108; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 108, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x00, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    transient_result.records[0].key = 0x01; coop_transient_apply();
    CHECK(script_calls == 1); // The linked door is not a trigger.

    reset(); role = ROLE_JOIN; current_map = 112; load(0, 0x0E, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 112, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x0E, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x0E && last_state == 2);
    scripts[0x0E].unk48[0] = 5; coop_transient_apply();
    CHECK(script_calls == 1); // Local reset/door states are never overwritten.

    reset(); role = ROLE_JOIN; current_map = 163; load(0, 0x05, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 163, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x05, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x05 && last_state == 2);
    scripts[0x05].unk48[0] = 4; coop_transient_apply();
    CHECK(script_calls == 1); // A running door sequence is never rewound.

    reset(); role = ROLE_JOIN; current_map = 164; load(0, 0x09, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 164, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x09, 2, 5}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x09 && last_state == 5);
    scripts[0x09].unk48[0] = 2; coop_transient_apply();
    CHECK(script_calls == 1); // Only the reviewed ready state may enter state 5.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x15, 0);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x15, 2, 1}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x15 && last_state == 1);
    scripts[0x15].unk48[0] = 2; coop_transient_apply();
    CHECK(script_calls == 1); // Active grate animation is never rewound.

    reset(); role = ROLE_JOIN; current_map = 4; load(0, 0x0A, 1); load(1, 0x0B, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 4, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x0A, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 2 && last_object == 0x0A && last_state == 2);
    CHECK(scripts[0x0B].unk48[0] == 10);
    coop_transient_apply();
    CHECK(script_calls == 2); // The same hit cannot replay either switch.
    reset(); role = ROLE_JOIN; current_map = 4; load(0, 0x06, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 4, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x06, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x06 && last_state == 2);
    scripts[0x06].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Collected reward path cannot be restarted.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0x13, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x13, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x13 && last_state == 2);
    coop_transient_apply(); CHECK(script_calls == 1); // Local pad sequence owns later states.

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 8, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 8, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 8 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x16, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x16, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x16 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x23, 11);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x23, 2, 12}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x23 && last_state == 12);
    coop_transient_apply(); CHECK(script_calls == 1); // A held record cannot replay the hit.
    scripts[0x23].unk48[0] = 10; coop_transient_apply();
    CHECK(script_calls == 1); // The network cannot arm an uninitialized head.
    scripts[0x23].unk48[0] = 11; transient_result.records[0].value = 11;
    coop_transient_apply(); CHECK(script_calls == 1); // Pinned activation is mandatory.

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x69, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x69, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x69 && last_state == 2);
    scripts[0x69].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 1); // Completed tunnel switches never rewind.

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x6B, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x6B, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x6B && last_state == 2);
    scripts[0x6B].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // The local timed cycle cannot be restarted.

    reset(); role = ROLE_JOIN; current_map = 19; load(0, 0x04, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 19, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x04, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x04 && last_state == 2);
    scripts[0x04].unk48[0] = 3; coop_transient_apply();
    CHECK(script_calls == 1); // Panel animation states remain locally owned.

    reset(); role = ROLE_JOIN; current_map = 24; load(0, 0x07, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 24, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x07, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x07 && last_state == 2);
    transient_result.records[0].key = 0x06; coop_transient_apply();
    CHECK(script_calls == 1); // A linked wall panel is not an allowed trigger.

    reset(); role = ROLE_JOIN; current_map = 16; load(0, 4, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 16, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 4, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 4 && last_state == 2);

    reset(); role = ROLE_JOIN; current_map = 16;
    load(0, 0x0C, 1); load(1, 0x0D, 1); load(2, 0x0E, 1); load(3, 0x0F, 10);
    transient_result = {COOP_TRANSIENT_APPLYING, 16, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x0C, 3, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x0F && last_state == 11);
    coop_transient_apply(); CHECK(script_calls == 1); // Local chain must advance first.
    scripts[0x0F].unk48[0] = 22; scripts[0x0E].unk48[0] = 10;
    coop_transient_apply();
    CHECK(script_calls == 2 && last_object == 0x0E && last_state == 11);
    scripts[0x0E].unk48[0] = 1; scripts[0x0D].unk48[0] = 10;
    transient_result.records[0].state = 4; coop_transient_apply();
    CHECK(script_calls == 3 && last_object == 0x0D && last_state == 11);
    scripts[0x0D].unk48[0] = 1; scripts[0x0C].unk48[0] = 10;
    coop_transient_apply();
    CHECK(script_calls == 4 && last_object == 0x0C && last_state == 11);

    reset(); role = ROLE_JOIN; current_map = 16;
    load(0, 0x0C, 1); load(1, 0x0D, 1); load(2, 0x0E, 1); load(3, 0x0F, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 16, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x0C, 1, 0}}};
    coop_transient_apply(); CHECK(!script_calls); // Cannot start the puzzle remotely.

    reset(); role = ROLE_JOIN; current_map = 26; load(0, 0x7F, 17);
    transient_result = {COOP_TRANSIENT_APPLYING, 26, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x7F, 4, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x7F && last_state == 54);
    scripts[0x7F].unk48[0] = 19; transient_result.records[0].state = 6;
    coop_transient_apply();
    CHECK(script_calls == 2 && last_state == 58); // Exactly the next hit, no skip.
    scripts[0x7F].unk48[0] = 20; coop_transient_apply();
    CHECK(script_calls == 3 && last_state == 23); // Vanilla final-hit entry.
    scripts[0x7F].unk48[0] = 23; coop_transient_apply();
    CHECK(script_calls == 3); // Completion is idempotent.

    reset(); role = ROLE_JOIN; current_map = 84; load(0, 0x00, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 84, 9, 1,
        {{COOP_TRANSIENT_SEQUENCE, 0x00, 4, 0}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x00 && last_state == 2);
    coop_transient_apply();
    CHECK(script_calls == 1); // Wait for the local intermediate states.
    scripts[0x00].unk48[0] = 4; coop_transient_apply();
    CHECK(script_calls == 2 && last_state == 5);
    scripts[0x00].unk48[0] = 6; coop_transient_apply();
    CHECK(script_calls == 3 && last_state == 7);
    scripts[0x00].unk48[0] = 8; coop_transient_apply();
    CHECK(script_calls == 4 && last_state == 9);
    scripts[0x00].unk48[0] = 9; coop_transient_apply();
    CHECK(script_calls == 4); // Reward states are entered only by the local script.
    scripts[0x00].unk48[0] = 50; coop_transient_apply();
    CHECK(script_calls == 4); // A failure branch cannot be advanced remotely.
}

static void cutscene_checks() {
    reset(); current_map = 48; is_cutscene_active = 1;
    D_global_asm_807476F8 = 12; D_global_asm_807F5CF0 = 3; D_global_asm_807F5CF4 = 0x41;
    coop_transient_capture(1);
    CHECK(contains(COOP_TRANSIENT_CUTSCENE, 13, 3));

    role = ROLE_JOIN;
    transient_result = {COOP_TRANSIENT_APPLYING, 48, 9, 1,
        {{COOP_TRANSIENT_CUTSCENE, 13, 4, 0x41}}};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 4);
    transient_result.records[0].state = 6;
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 4); // Never skip camera phases.
    transient_result.records[0] = {COOP_TRANSIENT_CUTSCENE, 14, 5, 0x41};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 4); // Wrong cutscene ID.
    transient_result.records[0] = {COOP_TRANSIENT_CUTSCENE, 13, 5, 0x40};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 4); // Wrong control flags.
    is_cutscene_active = 0; transient_result.records[0] = {COOP_TRANSIENT_CUTSCENE, 13, 5, 0x41};
    coop_transient_apply(); CHECK(D_global_asm_807F5CF0 == 4); // A packet cannot start one.
}

int main() {
    capture_checks(); object_apply_checks(); cutscene_checks();
    std::printf("PASS: %u reviewed script/cutscene adapter checks\n", checks);
}
