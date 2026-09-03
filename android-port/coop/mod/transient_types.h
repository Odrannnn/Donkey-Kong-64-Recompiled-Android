#ifndef DKCOOP_TRANSIENT_TYPES_H
#define DKCOOP_TRANSIENT_TYPES_H

// Same-area state is deliberately separate from persistent item/world state.
// Records contain only bounded identifiers selected by the game adapter; no
// pointer, function address, arbitrary flag, or process-memory offset crosses
// the native ABI or network.
#define COOP_TRANSIENT_RECORDS 8
#define COOP_TRANSIENT_WIRE_WORDS (6 + COOP_TRANSIENT_RECORDS * 4)
#define COOP_TRANSIENT_RESULT_WORDS (4 + COOP_TRANSIENT_RECORDS * 4)

enum {
    COOP_TRANSIENT_NONE,
    COOP_TRANSIENT_SCRIPT,
    COOP_TRANSIENT_TIMER,
    COOP_TRANSIENT_PLATFORM,
    COOP_TRANSIENT_CUTSCENE,
    // A reviewed room action. State 2 means enter the exact vanilla activation
    // state carried in value; state 1 is an inert ready/finished observation.
    COOP_TRANSIENT_TRIGGER,
    // A reviewed ordered controller. State is bounded logical progress; the
    // game adapter may execute only its next pinned local step.
    COOP_TRANSIENT_SEQUENCE,
    // A reviewed actor-driven environmental cycle. Key is a one-based entry
    // in the immutable enemy-spawner table; state/timer remain type-specific.
    COOP_TRANSIENT_ACTOR_CYCLE,
    // Caves Ice Tomato's sixteen-cell board while both local encounter
    // controllers are active. Value contains sixteen validated two-bit cells.
    COOP_TRANSIENT_TOMATO_BOARD,
    // The same encounter's host countdown. State 1 carries 0..60 seconds
    // remaining; state 2 is the final expiry edge after the final board sample.
    COOP_TRANSIENT_TOMATO_CLOCK,
    // One of four exact vanilla Kremling Kosh controllers completed. This
    // monotonic event is bidirectional; each copy runs its own success helper.
    COOP_TRANSIENT_MINIGAME_SUCCESS,
    // One of three exact vanilla Minecart Mayhem controllers completed. This
    // is separate from Kosh because Minecart has its own terminal state gate.
    COOP_TRANSIENT_MINECART_SUCCESS,
    // One of Fungi Forest's two vanilla Rabbit Race rounds completed. The
    // stock Rabbit owns both presentations, the first-round refill and GB.
    COOP_TRANSIENT_RABBIT_SUCCESS,
    // One of three exact vanilla Batty Barrel Bandit controllers completed.
    // Reels, lives, timer, presentation, destination and reward remain local.
    COOP_TRANSIENT_BATTY_SUCCESS,
    // Fungi Forest's vanilla Diddy Owl Race completed. Rings, flight,
    // presentation and the actor-owned reward sequence remain local.
    COOP_TRANSIENT_OWL_SUCCESS,
    COOP_TRANSIENT_KIND_COUNT = COOP_TRANSIENT_OWL_SUCCESS
};
enum {
    COOP_TRANSIENT_OFF,
    COOP_TRANSIENT_WAITING,
    COOP_TRANSIENT_APPLYING,
    COOP_TRANSIENT_SYNCED
};

typedef struct {
    unsigned kind, key, state, value;
} CoopTransientRecord;

// Pinned private layout of DK64's stock countdown actor. It lives here so the
// MIPS translation unit can assert offsets before including the game adapter.
typedef struct {
    unsigned long long started;
    unsigned elapsed;
    int duration;
    unsigned char text;
} CoopCountdownData;

typedef struct {
    unsigned enabled, file, map, epoch, revision, count;
    CoopTransientRecord records[COOP_TRANSIENT_RECORDS];
} CoopTransientInput;

typedef struct {
    unsigned status, map, epoch, count;
    CoopTransientRecord records[COOP_TRANSIENT_RECORDS];
} CoopTransientResult;

#endif
