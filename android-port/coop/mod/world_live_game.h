#ifndef COOP_WORLD_LIVE_GAME_H
#define COOP_WORLD_LIVE_GAME_H

// Pinned US 1.0 instance-script metadata. Every row is a flag-positive state-0
// initializer whose only action selects this completed state. Every object that
// reads the flag in the listed map is represented, and the selected state's
// immediate blocks contain only local model/collision/script-state operations.
// Complex scripts keep the full vanilla map-reload fallback.
typedef struct {
    unsigned short map, flag, object;
    unsigned char state;
} CoopLiveWorldState;

enum { COOP_LIVE_WORLD_STATE_COUNT = 73, COOP_LIVE_WORLD_SCRIPT_SLOTS = 600 };
static const CoopLiveWorldState coop_live_world_states[COOP_LIVE_WORLD_STATE_COUNT] = {
    { 7, 0x000, 0x01A, 20}, { 7, 0x000, 0x01B, 20},
    { 7, 0x007, 0x034, 20}, { 7, 0x007, 0x035, 20}, { 7, 0x007, 0x033, 20},
    { 7, 0x00D, 0x038, 20}, { 7, 0x00D, 0x041,  7},
    { 7, 0x00E, 0x03A, 20}, { 7, 0x00E, 0x03F,  7},
    { 7, 0x00F, 0x03B, 20}, { 7, 0x00F, 0x040,  7},
    { 7, 0x010, 0x039, 20}, { 7, 0x010, 0x03E,  7},
    { 7, 0x01E, 0x027, 20}, { 7, 0x01E, 0x028, 20},
    {26, 0x06E, 0x016, 20}, {26, 0x06E, 0x0B0, 20},
    {26, 0x081, 0x02D, 20},
    {30, 0x099, 0x002, 20}, {30, 0x099, 0x006, 20}, {30, 0x099, 0x007, 20},
    {30, 0x0A1, 0x003, 20}, {30, 0x0A1, 0x008, 20}, {30, 0x0A1, 0x009, 20},
    {30, 0x09B, 0x004, 20}, {30, 0x09B, 0x00A, 20}, {30, 0x09B, 0x00B, 20},
    {38, 0x02F, 0x09F, 20}, {38, 0x02F, 0x0A0, 20}, {38, 0x02F, 0x040, 20},
    {38, 0x030, 0x09D, 20}, {38, 0x030, 0x09E, 20}, {38, 0x030, 0x08D, 20},
    {48, 0x0CF, 0x017, 20}, {48, 0x0CF, 0x018, 20}, {48, 0x0CF, 0x019, 20},
    {48, 0x0D0, 0x01C, 20}, {48, 0x0D0, 0x01A, 20}, {48, 0x0D0, 0x01B, 20},
    {48, 0x0D2, 0x01D, 20}, {48, 0x0D2, 0x01E, 20},
    // Closed boss portals. State 20 is terminal in every loaded main-map
    // portal script, so it removes the model/collision without replaying a
    // boss transition, reward or cutscene. Other-map consumers initialize
    // from the saved flag when those maps are entered.
    { 7, 0x02E, 0x02C, 20}, { 7, 0x02E, 0x037, 20}, { 7, 0x02E, 0x11A, 20},
    {38, 0x06C, 0x008, 20}, {38, 0x06C, 0x009, 20}, {38, 0x06C, 0x00A, 20},
    {38, 0x06C, 0x00B, 20}, {38, 0x06C, 0x0EC, 20},
    {26, 0x098, 0x046, 20}, {26, 0x098, 0x047, 20}, {26, 0x098, 0x048, 20},
    {26, 0x098, 0x049, 20}, {26, 0x098, 0x04A, 20},
    {30, 0x0CB, 0x022, 20}, {30, 0x0CB, 0x023, 20}, {30, 0x0CB, 0x024, 20},
    {30, 0x0CB, 0x025, 20}, {30, 0x0CB, 0x026, 20},
    {48, 0x102, 0x015, 20}, {48, 0x102, 0x016, 20}, {48, 0x102, 0x03B, 20},
    {48, 0x102, 0x047, 20}, {48, 0x102, 0x052, 20},
    {72, 0x12E, 0x023, 20}, {72, 0x12E, 0x024, 20},
    {72, 0x12E, 0x025, 20}, {72, 0x12E, 0x026, 20},
    {87, 0x160, 0x00B, 20}, {87, 0x160, 0x00C, 20}, {87, 0x160, 0x00D, 20},
    // Training exit switch and door. State 20 is each flag-positive vanilla
    // initializer and runs their normal completed presentation/removal path.
    {176, 0x181, 0x039, 20}, {176, 0x181, 0x049, 20},
};

// Portal rows exist only to apply an accepted permanent flag. Their ordinary
// in-progress script states belong to local Troff/boss flow and must never
// consume records or become host-authoritative in the transient channel.
static inline unsigned coop_live_world_transient_eligible(const CoopLiveWorldState* state) {
    return state->flag != 0x02E && state->flag != 0x06C && state->flag != 0x098
        && state->flag != 0x0CB && state->flag != 0x102 && state->flag != 0x12E
        && state->flag != 0x160;
}

static inline unsigned coop_live_world_set_object(unsigned object, unsigned state) {
    for (unsigned slot = 0; slot < COOP_LIVE_WORLD_SCRIPT_SLOTS; ++slot) {
        if ((unsigned short)D_global_asm_807F6240[slot] != object) continue;
        func_global_asm_8063DA40((short)slot, (short)state);
        return 1;
    }
    return 0;
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
    unsigned expected = 0, updated = 0;
    for (unsigned row = 0; row < COOP_LIVE_WORLD_STATE_COUNT; ++row) {
        const CoopLiveWorldState* state = &coop_live_world_states[row];
        if (state->map != (unsigned)current_map || state->flag != flag) continue;
        ++expected;
        updated += coop_live_world_set_object(state->object, state->state);
    }
    // A partial setup must use the full reload path; never report success when
    // a reviewed object is absent or the flag has no complete live-state row.
    return expected && updated == expected;
}

#endif
