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
    COOP_LIVE_WORLD_LLAMA_FREE = 9,
    COOP_LIVE_WORLD_LLAMA_WATER = 10,
    COOP_LIVE_WORLD_SEAL = 11,
    COOP_LIVE_WORLD_MUSHROOM_SWITCH = 12,
    COOP_LIVE_WORLD_RABBIT = 13,
    COOP_LIVE_WORLD_BEANSTALK = 14,
    COOP_LIVE_WORLD_SCRIPT_SLOTS = 600,
    COOP_LIVE_WORLD_STATE_COUNT = 263
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

    // Aztec lobby's feather switch and stone panel both expose a dedicated
    // flag-positive state-0 initializer. Replaying those initializers applies
    // the stock completed switch presentation and hides/disables the panel;
    // it cannot enter the projectile, cutscene or permanent-flag write path.
    {173, 0x18F, 0x010,  0, COOP_LIVE_WORLD_REPLAY},
    {173, 0x18F, 0x00F,  0, COOP_LIVE_WORLD_REPLAY},

    // Galleon lobby's Chunky slam switch and door likewise have isolated
    // flag-positive initializers. They apply the completed switch animation
    // and hide/disable the open door without replaying the slam or cutscene.
    {174, 0x191, 0x00A,  0, COOP_LIVE_WORLD_REPLAY},
    {174, 0x191, 0x00C,  0, COOP_LIVE_WORLD_REPLAY},

    // Helm lobby's saved flag leaves its bridge visible by default on map
    // load, but the already-loaded bridge was hidden by the incomplete branch.
    // State 10 is the stock inverse visibility operation; the switch replays
    // only its completed initializer and never the coconut-hit cutscene path.
    {170, 0x197, 0x000, 10, COOP_LIVE_WORLD_PERMANENT},
    {170, 0x197, 0x002,  0, COOP_LIVE_WORLD_REPLAY},

    // Factory lobby's panel and linked platform expose their exact persistent
    // entry states. Selecting 20/10 preserves the platform actor/effect setup
    // while bypassing the local lever interaction and its reward cutscene.
    {175, 0x18D, 0x00C, 20, COOP_LIVE_WORLD_PERMANENT},
    {175, 0x18D, 0x00D, 10, COOP_LIVE_WORLD_PERMANENT},

    // Fairy Island's door has a saved-open state-0 branch. The Rareware GB
    // prop may be partway through its locked-room fade loop when the flag
    // arrives, so apply the exact fully-visible target; the new flag then
    // prevents that loop from scheduling another fade.
    {189, 0x189, 0x01D,  0, COOP_LIVE_WORLD_REPLAY},
    {189, 0x189, 0x01E,  0, COOP_LIVE_WORLD_REVEAL},

    // Aztec lobby's rotating panel and encounter platform each have an
    // isolated flag-positive initializer. Replaying state 0 performs their
    // normal linked-model/platform completion without entering the enemy,
    // reward, camera or permanent-flag write sequence.
    {173, 0x190, 0x004,  0, COOP_LIVE_WORLD_REPLAY},
    {173, 0x190, 0x011,  0, COOP_LIVE_WORLD_REPLAY},

    // Fungi lobby's gun-order controller and wooden panel each provide a
    // flag-positive state-0 initializer. These paths hide the controller and
    // settle the panel without replaying the five-hit order, timer, camera,
    // linked switch call or permanent-flag write.
    {178, 0x195, 0x009,  0, COOP_LIVE_WORLD_REPLAY},
    {178, 0x195, 0x004,  0, COOP_LIVE_WORLD_REPLAY},

    // Lanky's Caves igloo balloon pad has a dedicated saved-revealed state-0
    // branch. It selects the normal active interaction state without replaying
    // the local encounter completion, flag write, sound or camera sequence.
    { 85, 0x118, 0x000,  0, COOP_LIVE_WORLD_REPLAY},

    // Caves' five igloo instrument pads and DK-star reveal controller form one
    // permanent unit. Every state-0 flag branch selects the normal revealed
    // pad setup or hides the spent controller; none enters the five-pad input,
    // timer, camera, sound, linked-object or flag-write path.
    { 72, 0x128, 0x000,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x128, 0x001,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x128, 0x002,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x128, 0x003,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x128, 0x004,  0, COOP_LIVE_WORLD_REPLAY},
    { 72, 0x128, 0x038,  0, COOP_LIVE_WORLD_REPLAY},

    // Caves' persistent encounter controller has a saved-clear state-0 branch
    // that pauses it before the delayed spawn/camera path can run.
    { 72, 0x12C, 0x034,  0, COOP_LIVE_WORLD_REPLAY},

    // Castle ballroom's saved-clear initializer selects state 10, which waits
    // for the stock room-ready condition, reveals actor spawner 0 and pauses.
    // Replaying state 0 bypasses the enemy/camera/flag-write sequence itself.
    { 88, 0x130, 0x003,  0, COOP_LIVE_WORLD_REPLAY},

    // Castle tower's saved-clear branch selects the balloon pad's active state,
    // while a fresh completed map relies on default visibility. The loaded
    // incomplete pad was explicitly hidden, so restore the stock fully-visible
    // target after replaying its state-0 completed branch.
    {105, 0x133, 0x001,  0, COOP_LIVE_WORLD_REPLAY},
    {105, 0x133, 0x001,  0, COOP_LIVE_WORLD_REVEAL},

    // Helm's coin door has a dedicated flag-positive state-0 branch that
    // enters its completed hide/presentation path. It never rechecks, deducts
    // or awards either special coin and skips the local opening cutscene.
    { 17, 0x303, 0x003,  0, COOP_LIVE_WORLD_REPLAY},

    // Helm's crown door uses the same isolated saved-open pattern. The replay
    // enters only its completed hide/presentation path; crown ownership remains
    // a prerequisite and no crown count or local opening sequence is copied.
    { 17, 0x304, 0x004,  0, COOP_LIVE_WORLD_REPLAY},

    // Helm shutdown is one 21-script permanent unit: the K. Rool door, four
    // instrument pads, fifteen power beams and the final door controller all
    // expose flag-positive state-0 initializers. Replaying those branches
    // applies the vanilla shutdown layout without copying the Helm timer,
    // barrel rooms, instrument challenges, cameras or completion sequence.
    { 17, 0x302, 0x005,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x02D,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x02E,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x02F,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x030,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x020,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x00E,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x008,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x026,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x011,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x009,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x024,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x00A,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x00F,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x01E,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x00B,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x010,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x022,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x00D,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x00C,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x302, 0x1F6,  0, COOP_LIVE_WORLD_REPLAY},
    // Each power-beam group calls its matching medal barrier's state 10. Make
    // those five dependencies part of preflight without scheduling them twice.
    { 17, 0x302, 0x01F,  0, COOP_LIVE_WORLD_REQUIRE},
    { 17, 0x302, 0x021,  0, COOP_LIVE_WORLD_REQUIRE},
    { 17, 0x302, 0x023,  0, COOP_LIVE_WORLD_REQUIRE},
    { 17, 0x302, 0x025,  0, COOP_LIVE_WORLD_REQUIRE},
    { 17, 0x302, 0x027,  0, COOP_LIVE_WORLD_REQUIRE},

    // Aztec's beetle-tower reveal hides four spent gongs and moves the metal
    // tower controller into its saved state-20 setup. The GB prop was hidden
    // by the already-loaded incomplete initializer, so restore its exact
    // fully-visible target without running the race/reward/camera sequence.
    { 38, 0x035, 0x01A,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x035, 0x01B,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x035, 0x01C,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x035, 0x01D,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x035, 0x01E,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x035, 0x0A3,  0, COOP_LIVE_WORLD_REVEAL},

    // Aztec's five-door-temple unit contains the Diddy slam switch, Feed Me
    // Totem and five gun switches. Their state-0 flag branches select the
    // completed switch/totem presentation and each gun switch's normal reveal
    // state, without replaying feeding, slam, cameras, projectiles or flag writes.
    { 38, 0x037, 0x028,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x037, 0x027,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x037, 0x010,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x037, 0x011,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x037, 0x012,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x037, 0x013,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x037, 0x014,  0, COOP_LIVE_WORLD_REPLAY},

    // Helm and its lobby use the same two Bananaport scripts. Their sole
    // flag-positive state-0 operation selects the vanilla tagged visibility
    // target; no interaction, cutscene, destination or second flag is replayed.
    {170, 0x1A1, 0x008,  0, COOP_LIVE_WORLD_REPLAY},
    {170, 0x1A2, 0x009,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x305, 0x059,  0, COOP_LIVE_WORLD_REPLAY},
    { 17, 0x306, 0x058,  0, COOP_LIVE_WORLD_REPLAY},

    // Freeing the exterior llama activates all three gun switches through
    // their isolated saved-complete initializers, then removes the caged actor
    // exactly as the character-spawner flag filter does on map load. Inside
    // the temple, waking object 0x16 applies the newly eligible llama route.
    { 38, 0x032, 0x00D,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x032, 0x00E,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x032, 0x00F,  0, COOP_LIVE_WORLD_REPLAY},
    { 38, 0x032, 0xFFFF, 0, COOP_LIVE_WORLD_LLAMA_FREE},
    { 20, 0x032, 0x016,  0, COOP_LIVE_WORLD_REPLAY},

    // Temple water cooling has two flag-positive state-0 consumers. The llama
    // actor uses state 39/progress 4 for a save that entered already cooled;
    // the exterior has no loaded consumer and may accept the flag immediately.
    { 20, 0x04C, 0x016,  0, COOP_LIVE_WORLD_REPLAY},
    { 20, 0x04C, 0x018,  0, COOP_LIVE_WORLD_REPLAY},
    { 20, 0x04C, 0xFFFF, 0, COOP_LIVE_WORLD_LLAMA_WATER},
    { 38, 0x04C, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},

    // Galleon's object 0x38 has an isolated flag-positive entrance branch.
    // The actor adapter moves the loaded cage/race copies to the same states
    // chosen by their completed initializers; the race map already implies
    // that the seal was freed and therefore needs no loaded mutation.
    { 30, 0x09E, 0x038,  0, COOP_LIVE_WORLD_REPLAY},
    { 30, 0x09E, 0xFFFF, 0, COOP_LIVE_WORLD_SEAL},
    { 39, 0x09E, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},

    // The exterior has no consumer for the five Giant Mushroom gun flags, so
    // it can persist them immediately. Inside the mushroom, each switch is
    // paired with the shared board controller by the specialized atomic mode.
    { 48, 0x0E6, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 48, 0x0E7, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 48, 0x0E8, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 48, 0x0E9, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 48, 0x0EA, 0xFFFF, 0, COOP_LIVE_WORLD_NOOP},
    { 64, 0x0E6, 0x00D,  0, COOP_LIVE_WORLD_MUSHROOM_SWITCH},
    { 64, 0x0E7, 0x00E,  0, COOP_LIVE_WORLD_MUSHROOM_SWITCH},
    { 64, 0x0E8, 0x00F,  0, COOP_LIVE_WORLD_MUSHROOM_SWITCH},
    { 64, 0x0E9, 0x010,  0, COOP_LIVE_WORLD_MUSHROOM_SWITCH},
    { 64, 0x0EA, 0x00C,  0, COOP_LIVE_WORLD_MUSHROOM_SWITCH},

    // The rabbit reads round-one completion every actor tick. The adapter only
    // gates the permanent write to idle/completed states, preventing a remote
    // finish from changing the rules of a first race already in progress.
    { 48, 0x0F8, 0xFFFF, 0, COOP_LIVE_WORLD_RABBIT},

    // The beanstalk is an actor rather than an instance-script prop. Its
    // adapter restores the completed model, scale and collision when idle,
    // while preserving a local growth cutscene that is already underway.
    { 48, 0x0FB, 0xFFFF, 0, COOP_LIVE_WORLD_BEANSTALK},

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
                && state->mode != COOP_LIVE_WORLD_LLAMA_FREE
                && state->mode != COOP_LIVE_WORLD_LLAMA_WATER
                && state->mode != COOP_LIVE_WORLD_SEAL
                && state->mode != COOP_LIVE_WORLD_MUSHROOM_SWITCH
                && state->mode != COOP_LIVE_WORLD_RABBIT
                && state->mode != COOP_LIVE_WORLD_BEANSTALK
                && !coop_live_world_find_object(state->object, 0)) return 0;
        if (state->mode == COOP_LIVE_WORLD_MERMAID
                && !coop_live_world_mermaid_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_ISLES_TROMBONE
                && !coop_live_world_isles_trombone_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_LLAMA_FREE
                && !coop_live_world_llama_free_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_LLAMA_WATER
                && !coop_live_world_llama_water_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_SEAL
                && !coop_live_world_seal_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_MUSHROOM_SWITCH
                && !coop_live_world_mushroom_switch_ready(state->object)) return 0;
        if (state->mode == COOP_LIVE_WORLD_RABBIT
                && !coop_live_world_rabbit_ready()) return 0;
        if (state->mode == COOP_LIVE_WORLD_BEANSTALK
                && !coop_live_world_beanstalk_ready()) return 0;
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

static inline unsigned coop_live_reversible_ready(unsigned toggle, unsigned desired) {
    if (toggle == 0 && (unsigned)current_map == 30) // Galleon up/down switches 0/1.
        return coop_live_world_find_object(desired ? 0 : 1, 0);
    if (toggle == 1 && (unsigned)current_map == 48) // Fungi night/day switches 4/5.
        return coop_live_world_find_object(desired ? 4 : 5, 0);
    if (toggle == 2 && (unsigned)current_map == 194) // Caves lobby pressure switch.
        return coop_live_world_find_object(6, 0);
    return 0;
}

// Start the exact loaded vanilla switch sequence for the reversible world
// modifiers. State 10 is the interaction's post-press sequence: it updates the
// water/lighting, collision, exits and paired switch presentation without
// synthesizing a player interaction. The caller preflights the selected script
// and defers the flag while it is absent.
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
        if (state->mode == COOP_LIVE_WORLD_LLAMA_FREE) {
            if (!coop_live_world_llama_free_refresh()) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_LLAMA_WATER) {
            if (!coop_live_world_llama_water_refresh()) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_SEAL) {
            if (!coop_live_world_seal_refresh()) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_MUSHROOM_SWITCH) {
            if (!coop_live_world_mushroom_switch_refresh(state->object)) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_RABBIT) {
            if (!coop_live_world_rabbit_refresh()) return 0;
            continue;
        }
        if (state->mode == COOP_LIVE_WORLD_BEANSTALK) {
            if (!coop_live_world_beanstalk_refresh()) return 0;
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
