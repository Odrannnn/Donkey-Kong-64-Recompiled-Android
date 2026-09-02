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
static std::array<Prop, 256> props;
static std::array<Prop_ScriptData, 256> scripts;
static Prop* D_global_asm_807F6000 = props.data();

static s16 func_global_asm_80659470(s32 object) {
    return object >= 0 && object < 256 ? static_cast<s16>(object) : -1;
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
    CHECK(contains(COOP_TRANSIENT_SCRIPT, 0x34, 4));
    load(3, 0x30, 1); load(4, 0x31, 2); load(5, 0x32, 3);
    bool saw_japes_30 = false, saw_japes_31 = false, saw_japes_32 = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_japes_30 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 1, 2);
        saw_japes_31 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 2, 2);
        saw_japes_32 |= contains_value(COOP_TRANSIENT_TRIGGER, 0x32, 2, 2);
    }
    CHECK(saw_japes_30 && saw_japes_31 && saw_japes_32);

    reset(); current_map = 30; load(0, 0, 10, 123); load(1, 1, 6, 45);
    coop_transient_capture(1);
    coop_transient_capture(1); // Galleon's reviewed script rows occupy page zero.
    CHECK(contains_value(COOP_TRANSIENT_TIMER, 0, 10, 123));
    CHECK(contains_value(COOP_TRANSIENT_TIMER, 1, 6, 45));

    reset(); current_map = 30; load(0, 0x11, 1); load(1, 0x1C, 3);
    bool saw_bongo = false, saw_tiny_slam = false;
    for (unsigned page = 0; page < 8; ++page) {
        coop_transient_capture(1);
        saw_bongo |= contains_value(COOP_TRANSIENT_TRIGGER, 0x11, 1, 2);
        saw_tiny_slam |= contains_value(COOP_TRANSIENT_TRIGGER, 0x1C, 2, 2);
    }
    CHECK(saw_bongo && saw_tiny_slam);

    reset(); current_map = 20; load(0, 0x12, 1); load(1, 0x16, 4);
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x12, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x16, 2, 2));

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
    coop_transient_capture(1);
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x2E, 1, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x2F, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x30, 2, 2));
    CHECK(contains_value(COOP_TRANSIENT_TRIGGER, 0x31, 1, 2));

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

    reset(); role = ROLE_JOIN; current_map = 30; load(0, 0x13, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 30, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x13, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x13 && last_state == 2);
    coop_transient_apply(); CHECK(script_calls == 1); // Local pad sequence owns later states.

    reset(); role = ROLE_JOIN; current_map = 20; load(0, 0x16, 1);
    transient_result = {COOP_TRANSIENT_APPLYING, 20, 9, 1,
        {{COOP_TRANSIENT_TRIGGER, 0x16, 2, 2}}};
    coop_transient_apply();
    CHECK(script_calls == 1 && last_object == 0x16 && last_state == 2);

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
