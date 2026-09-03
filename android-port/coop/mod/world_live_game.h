#ifndef COOP_WORLD_LIVE_GAME_H
#define COOP_WORLD_LIVE_GAME_H

// Pinned US 1.0 instance-script metadata. Direct rows select a reviewed
// completed state. Replay rows wake state 0 after the permanent flag is written,
// so the loaded object executes the same flag-positive initializer used on map
// load. Dependency rows are checked but never changed. Audited no-op rows mean
// that the flag has no consumer in this loaded map and will initialize naturally
// when its actual room is entered.
typedef struct {
    unsigned short map, flag, object;
    unsigned char state, mode;
} CoopLiveWorldState;

enum {
    COOP_LIVE_WORLD_DIRECT = 0,
    COOP_LIVE_WORLD_REPLAY = 1,
    COOP_LIVE_WORLD_PERMANENT = 2,
    COOP_LIVE_WORLD_REQUIRE = 3,
    COOP_LIVE_WORLD_NOOP = 4,
    COOP_LIVE_WORLD_CONTINUE_GALLEON = 5,
    COOP_LIVE_WORLD_REVEAL = 6,
    COOP_LIVE_WORLD_MERMAID = 7,
    COOP_LIVE_WORLD_ISLES_TROMBONE = 8,
    COOP_LIVE_WORLD_SCRIPT_SLOTS = 600,
    COOP_LIVE_WORLD_STATE_COUNT = 173
};
static const CoopLiveWorldState coop_live_world_states[COOP_LIVE_WORLD_STATE_COUNT] = {
    { 7, 0x000, 0x01A, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x000, 0x01B, 20, COOP_LIVE_WORLD_DIRECT},
    { 7, 0x007, 0x034, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x007, 0x035, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x007, 0x033, 20, COOP_LIVE_WORLD_DIRECT},
    { 7, 0x00D, 0x038, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x00D, 0x041,  7, COOP_LIVE_WORLD_DIRECT},
    { 7, 0x00E, 0x03A, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x00E, 0x03F,  7, COOP_LIVE_WORLD_DIRECT},
    { 7, 0x00F, 0x03B, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x00F, 0x040,  7, COOP_LIVE_WORLD_DIRECT},
    { 7, 0x010, 0x039, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x010, 0x03E,  7, COOP_LIVE_WORLD_DIRECT},
    { 7, 0x01E, 0x027, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x01E, 0x028, 20, COOP_LIVE_WORLD_DIRECT},
    {26, 0x06E, 0x016, 20, COOP_LIVE_WORLD_DIRECT}, {26, 0x06E, 0x0B0, 20, COOP_LIVE_WORLD_DIRECT},
    {26, 0x081, 0x02D, 20, COOP_LIVE_WORLD_DIRECT},
    {30, 0x099, 0x002, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x099, 0x006, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x099, 0x007, 20, COOP_LIVE_WORLD_DIRECT},
    {30, 0x0A1, 0x003, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x0A1, 0x008, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x0A1, 0x009, 20, COOP_LIVE_WORLD_DIRECT},
    {30, 0x09B, 0x004, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x09B, 0x00A, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x09B, 0x00B, 20, COOP_LIVE_WORLD_DIRECT},
    {38, 0x02F, 0x09F, 20, COOP_LIVE_WORLD_DIRECT}, {38, 0x02F, 0x0A0, 20, COOP_LIVE_WORLD_DIRECT}, {38, 0x02F, 0x040, 20, COOP_LIVE_WORLD_DIRECT},
    {38, 0x030, 0x09D, 20, COOP_LIVE_WORLD_DIRECT}, {38, 0x030, 0x09E, 20, COOP_LIVE_WORLD_DIRECT}, {38, 0x030, 0x08D, 20, COOP_LIVE_WORLD_DIRECT},
    {48, 0x0CF, 0x017, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x0CF, 0x018, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x0CF, 0x019, 20, COOP_LIVE_WORLD_DIRECT},
    {48, 0x0D0, 0x01C, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x0D0, 0x01A, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x0D0, 0x01B, 20, COOP_LIVE_WORLD_DIRECT},
    {48, 0x0D2, 0x01D, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x0D2, 0x01E, 20, COOP_LIVE_WORLD_DIRECT},
    // Closed boss portals. State 20 is terminal in every loaded main-map
    // portal script, so it removes the model/collision without replaying a
    // boss transition, reward or cutscene. Other-map consumers initialize
    // from the saved flag when those maps are entered.
    { 7, 0x02E, 0x02C, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x02E, 0x037, 20, COOP_LIVE_WORLD_DIRECT}, { 7, 0x02E, 0x11A, 20, COOP_LIVE_WORLD_DIRECT},
    {38, 0x06C, 0x008, 20, COOP_LIVE_WORLD_DIRECT}, {38, 0x06C, 0x009, 20, COOP_LIVE_WORLD_DIRECT}, {38, 0x06C, 0x00A, 20, COOP_LIVE_WORLD_DIRECT},
    {38, 0x06C, 0x00B, 20, COOP_LIVE_WORLD_DIRECT}, {38, 0x06C, 0x0EC, 20, COOP_LIVE_WORLD_DIRECT},
    {26, 0x098, 0x046, 20, COOP_LIVE_WORLD_DIRECT}, {26, 0x098, 0x047, 20, COOP_LIVE_WORLD_DIRECT}, {26, 0x098, 0x048, 20, COOP_LIVE_WORLD_DIRECT},
    {26, 0x098, 0x049, 20, COOP_LIVE_WORLD_DIRECT}, {26, 0x098, 0x04A, 20, COOP_LIVE_WORLD_DIRECT},
    {30, 0x0CB, 0x022, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x0CB, 0x023, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x0CB, 0x024, 20, COOP_LIVE_WORLD_DIRECT},
    {30, 0x0CB, 0x025, 20, COOP_LIVE_WORLD_DIRECT}, {30, 0x0CB, 0x026, 20, COOP_LIVE_WORLD_DIRECT},
    {48, 0x102, 0x015, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x102, 0x016, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x102, 0x03B, 20, COOP_LIVE_WORLD_DIRECT},
    {48, 0x102, 0x047, 20, COOP_LIVE_WORLD_DIRECT}, {48, 0x102, 0x052, 20, COOP_LIVE_WORLD_DIRECT},
    {72, 0x12E, 0x023, 20, COOP_LIVE_WORLD_DIRECT}, {72, 0x12E, 0x024, 20, COOP_LIVE_WORLD_DIRECT},
    {72, 0x12E, 0x025, 20, COOP_LIVE_WORLD_DIRECT}, {72, 0x12E, 0x026, 20, COOP_LIVE_WORLD_DIRECT},
    {87, 0x160, 0x00B, 20, COOP_LIVE_WORLD_DIRECT}, {87, 0x160, 0x00C, 20, COOP_LIVE_WORLD_DIRECT}, {87, 0x160, 0x00D, 20, COOP_LIVE_WORLD_DIRECT},
    // Training exit switch and door. State 20 is each flag-positive vanilla
    // initializer and runs their normal completed presentation/removal path.
    {176, 0x181, 0x039, 20, COOP_LIVE_WORLD_DIRECT}, {176, 0x181, 0x049, 20, COOP_LIVE_WORLD_DIRECT},

    // Breakable walls, grates and gates. Replay mode is intentionally state 0:
    // each loaded script runs its own flag-positive completion branch, including
    // linked collision-region updates on the Caves ice walls.
    {  7, 0x02B, 0x115,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x04E, 0x044,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x04E, 0x09C,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x077, 0x13C,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x085, 0x03C,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x097, 0x04E,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x097, 0x04C,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x097, 0x106,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x09F, 0x021,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x0A2, 0x005,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x0A2, 0x00C,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x0B9, 0x03F,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x109, 0x01F,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x10A, 0x01E,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x10B, 0x01D,  0, COOP_LIVE_WORLD_REPLAY},
    { 87, 0x144, 0x020,  0, COOP_LIVE_WORLD_REPLAY},
    { 16, 0x045, 0x004,  0, COOP_LIVE_WORLD_REPLAY},
    { 16, 0x045, 0x00A,  0, COOP_LIVE_WORLD_REPLAY},
    {112, 0x138, 0x011, 30, COOP_LIVE_WORLD_PERMANENT},
    {163, 0x157, 0x028,  0, COOP_LIVE_WORLD_REPLAY},
    {163, 0x158, 0x029,  0, COOP_LIVE_WORLD_REPLAY},
    {163, 0x159, 0x02A,  0, COOP_LIVE_WORLD_REPLAY},
    {163, 0x15A, 0x02B,  0, COOP_LIVE_WORLD_REPLAY},
    {163, 0x15B, 0x02C,  0, COOP_LIVE_WORLD_REPLAY},
    {163, 0x15C, 0x02D,  0, COOP_LIVE_WORLD_REPLAY},
    {187, 0x144, 0x000,  0, COOP_LIVE_WORLD_REPLAY},
    {187, 0x144, 0x001,  0, COOP_LIVE_WORLD_REPLAY},
    {187, 0x144, 0x002,  0, COOP_LIVE_WORLD_REPLAY},
    {187, 0x144, 0x003,  0, COOP_LIVE_WORLD_REPLAY},
    {194, 0x198, 0x000,  0, COOP_LIVE_WORLD_REPLAY},
    {194, 0x199, 0x001,  0, COOP_LIVE_WORLD_REPLAY},

    // Chests, boxes and their completion controllers. The state-0 replays run
    // only the saved-complete initializer, never the interaction/reward path.
    { 26, 0x078, 0x061,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x078, 0x062,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x078, 0x077,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x0B3, 0x00D,  0, COOP_LIVE_WORLD_REPLAY},
    // This exact initializer also creates the vanilla fairy reveal controller.
    { 30, 0x0B4, 0x045,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x0B5, 0x043,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x0B5, 0x00E,  0, COOP_LIVE_WORLD_REPLAY},
    { 61, 0x0DA, 0x006,  0, COOP_LIVE_WORLD_REPLAY},
    { 61, 0x0DA, 0x002,  0, COOP_LIVE_WORLD_REQUIRE},
    { 48, 0x0DA, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 62, 0x0DE, 0x001,  0, COOP_LIVE_WORLD_REPLAY},
    { 48, 0x0DE, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 48, 0x0DF, 0x02D,  0, COOP_LIVE_WORLD_REPLAY},
    { 62, 0x0DF, 0x003,  0, COOP_LIVE_WORLD_REPLAY},
    { 64, 0x0E5, 0x00B,  0, COOP_LIVE_WORLD_REPLAY},
    { 48, 0x0E5, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    {166, 0x141, 0x005,  0, COOP_LIVE_WORLD_REPLAY},
    { 87, 0x141, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},

    // Machine and platform controllers. Permanent rows select bounded completed
    // states; replay rows run the saved-complete initializer. REQUIRE rows make
    // Factory production atomic without scheduling the child scripts directly.
    { 26, 0x06D, 0x017, 20, COOP_LIVE_WORLD_PERMANENT},
    { 26, 0x06D, 0x01D, 20, COOP_LIVE_WORLD_PERMANENT},
    { 26, 0x06D, 0x010, 10, COOP_LIVE_WORLD_PERMANENT},
    { 26, 0x06F, 0x000, 10, COOP_LIVE_WORLD_PERMANENT},
    { 26, 0x06F, 0x107,  0, COOP_LIVE_WORLD_REPLAY},
    { 26, 0x06F, 0x001,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x002,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x003,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x004,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x005,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x006,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x007,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x008,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x009,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x00A,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x00B,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x00C,  0, COOP_LIVE_WORLD_REQUIRE},
    { 26, 0x06F, 0x00D,  0, COOP_LIVE_WORLD_REQUIRE},
    // A waiting controller replays its saved-complete initializer. A controller
    // already inside the audited ship/lighthouse sequence keeps running; the
    // incoming flag must not rewind its cutscene, timer or linked operations.
    { 30, 0x09C, 0x027,  0, COOP_LIVE_WORLD_CONTINUE_GALLEON},
    { 30, 0x09C, 0x051,  0, COOP_LIVE_WORLD_CONTINUE_GALLEON},
    { 48, 0x0D4, 0x025, 50, COOP_LIVE_WORLD_PERMANENT},
    { 48, 0x0D4, 0x027, 12, COOP_LIVE_WORLD_PERMANENT},
    { 48, 0x0D5, 0x025, 50, COOP_LIVE_WORLD_PERMANENT},
    { 48, 0x0D5, 0x027,  0, COOP_LIVE_WORLD_REPLAY},
    { 48, 0x0DC, 0x02B, 20, COOP_LIVE_WORLD_PERMANENT},

    // Isles boulder. The two controllers expose exact saved-complete states.
    // Object 0x56 only has an incomplete visibility initializer, so REVEAL
    // applies its audited inverse target without replaying the boulder event.
    { 34, 0x1AE, 0x03C, 40, COOP_LIVE_WORLD_PERMANENT},
    { 34, 0x1AE, 0x000,100, COOP_LIVE_WORLD_PERMANENT},
    { 34, 0x1AE, 0x056,  0, COOP_LIVE_WORLD_REVEAL},

    // The five pearl props each expose a flag-positive state-0 initializer in
    // the treasure chest. The Mermaid room has no instance-script consumer;
    // its actor recomputes the same vanilla partial/final state instead.
    { 44, 0x0BA, 0x000,  0, COOP_LIVE_WORLD_REPLAY},
    { 44, 0x0BB, 0x001,  0, COOP_LIVE_WORLD_REPLAY},
    { 44, 0x0BC, 0x002,  0, COOP_LIVE_WORLD_REPLAY},
    { 44, 0x0BD, 0x003,  0, COOP_LIVE_WORLD_REPLAY},
    { 44, 0x0BE, 0x004,  0, COOP_LIVE_WORLD_REPLAY},
    { 45, 0x0BA, 0xFFFF, 0, COOP_LIVE_WORLD_MERMAID},
    { 45, 0x0BB, 0xFFFF, 0, COOP_LIVE_WORLD_MERMAID},
    { 45, 0x0BC, 0xFFFF, 0, COOP_LIVE_WORLD_MERMAID},
    { 45, 0x0BD, 0xFFFF, 0, COOP_LIVE_WORLD_MERMAID},
    { 45, 0x0BE, 0xFFFF, 0, COOP_LIVE_WORLD_MERMAID},

    // Isles' trombone pad reveals the high rocket barrel through actor spawner
    // 9. The special adapter skips the cutscene and instrument edge, performs
    // only the stock completed notification/pad mode, and preserves any local
    // reveal sequence already active in states 3-6.
    { 34, 0x1AA, 0x031, 0, COOP_LIVE_WORLD_ISLES_TROMBONE},

    // Helm and its lobby use the same two Bananaport scripts. Their sole
    // flag-positive state-0 operation selects the vanilla tagged visibility
    // target; no interaction, cutscene, destination or second flag is replayed.
    {170, 0x1A1, 0x008,  0, COOP_LIVE_WORLD_REPLAY},
    {170, 0x1A2, 0x009,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x305, 0x059,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x306, 0x058,  0, COOP_LIVE_WORLD_REPLAY},

    // These completions are consumed only inside their submaps. Receiving one
    // in the corresponding main world needs a save, but no loaded script or
    // map rebuild; the target room initializes from the flag on entry.
    { 38, 0x045, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 87, 0x138, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 87, 0x157, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 87, 0x158, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 87, 0x159, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 87, 0x15A, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 87, 0x15B, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 87, 0x15C, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
};

// Portal rows exist only to apply an accepted permanent flag. Their ordinary
// in-progress script states belong to local Troff/boss flow and must never
// consume records or become host-authoritative in the transient channel.
static inline unsigned coop_live_world_transient_eligible(const CoopLiveWorldState* state) {
    return state->mode == COOP_LIVE_WORLD_DIRECT
        && state->flag != 0x02E && state->flag != 0x06C && state->flag != 0x098
        && state->flag != 0x0CB && state->flag != 0x102 && state->flag != 0x12E
        && state->flag != 0x160;
}

static inline unsigned coop_live_world_has_flag(unsigned flag) {
    for (unsigned row = 0; row < COOP_LIVE_WORLD_STATE_COUNT; ++row)
        if (coop_live_world_states[row].map == (unsigned)current_map
                && coop_live_world_states[row].flag == flag) return 1;
    return 0;
}

static inline unsigned coop_live_world_find_object(unsigned object, unsigned* found_slot) {
    for (unsigned slot = 0; slot < COOP_LIVE_WORLD_SCRIPT_SLOTS; ++slot) {
        if ((unsigned short)D_global_asm_807F6240[slot] != object) continue;
        if (found_slot) *found_slot = slot;
        return 1;
    }
    return 0;
}

static inline unsigned coop_live_world_set_object(unsigned object, unsigned state) {
    unsigned slot = 0;
    if (!coop_live_world_find_object(object, &slot)) return 0;
    func_global_asm_8063DA40((short)slot, (short)state);
    return 1;
}

static inline unsigned coop_live_world_galleon_state_valid(unsigned object, unsigned raw) {
    if (raw == 0) return 1;
    // Object 0x27 owns activation states 10-17 and its ordinary post-opening
    // interaction states 26-28. Object 0x51 owns activation states 10-14.
    if (object == 0x027) return (raw >= 10 && raw <= 17) || (raw >= 26 && raw <= 28);
    if (object == 0x051) return raw >= 10 && raw <= 14;
    return 0;
}

static inline unsigned coop_live_world_ready(unsigned flag) {
    unsigned expected = 0;
    for (unsigned row = 0; row < COOP_LIVE_WORLD_STATE_COUNT; ++row) {
        const CoopLiveWorldState* state = &coop_live_world_states[row];
        if (state->map != (unsigned)current_map || state->flag != flag) continue;
        ++expected;
        if (state->mode != COOP_LIVE_WORLD_NOOP
                && state->mode != COOP_LIVE_WORLD_MERMAID
                && !coop_live_world_find_object(state->object, 0)) return 0;
        if (state->mode == COOP_LIVE_WORLD_MERMAID
                && !coop_live_world_mermaid_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_ISLES_TROMBONE
                && !coop_live_world_isles_trombone_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_CONTINUE_GALLEON) {
            unsigned raw = 0;
            if (!coop_live_world_object_raw_state(state->object, &raw)
                    || !coop_live_world_galleon_state_valid(state->object, raw)) return 0;
        }
        if (state->mode == COOP_LIVE_WORLD_REVEAL) {
            unsigned raw = 0;
            if (!coop_live_world_object_raw_state(state->object, &raw)) return 0;
        }
    }
    return expected != 0;
}

// Start the exact loaded vanilla switch sequence for the reversible world
// modifiers. State 10 is the interaction's post-press sequence: it updates the
// water/lighting, collision, exits and paired switch presentation without
// synthesizing a player interaction. Missing scripts fail into the map rebuild.
static inline unsigned coop_live_reversible_refresh(unsigned toggle, unsigned desired) {
    if (toggle == 0 && (unsigned)current_map == 30) // Galleon: up/down switches 0/1.
        return coop_live_world_set_object(desired ? 0 : 1, 10);
    if (toggle == 1 && (unsigned)current_map == 48) // Fungi: night/day switches 4/5.
        return coop_live_world_set_object(desired ? 4 : 5, 10);
    if (toggle == 2 && (unsigned)current_map == 194) // Caves lobby: press/release state.
        return coop_live_world_set_object(6, desired ? 2 : 6);
    return 0;
}

static inline unsigned coop_live_world_refresh(unsigned flag) {
    // Preflight the complete reviewed unit before changing any loaded script.
    // This prevents a partial update if a linked object has not spawned yet.
    if (!coop_live_world_ready(flag)) return 0;
    for (unsigned row = 0; row < COOP_LIVE_WORLD_STATE_COUNT; ++row) {
        const CoopLiveWorldState* state = &coop_live_world_states[row];
        if (state->map != (unsigned)current_map || state->flag != flag
                || state->mode == COOP_LIVE_WORLD_NOOP
                || state->mode == COOP_LIVE_WORLD_REQUIRE) continue;
        if (state->mode == COOP_LIVE_WORLD_MERMAID) {
            if (!coop_live_world_mermaid_refresh()) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_ISLES_TROMBONE) {
            if (!coop_live_world_isles_trombone_refresh()) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_REVEAL) {
            if (!coop_live_world_reveal_object(state->object)) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_CONTINUE_GALLEON) {
            unsigned raw = 0;
            if (!coop_live_world_object_raw_state(state->object, &raw)) return 0;
            if (raw != 0) continue;
        }
        // REPLAY deliberately selects state 0. func_global_asm_8063DA40 also
        // wakes a paused script, so vanilla evaluates the newly written flag.
        if (!coop_live_world_set_object(state->object, state->state)) return 0;
    }
    return 1;
}

#endif
