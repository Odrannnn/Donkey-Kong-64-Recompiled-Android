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
    // A reviewed room action. State 2 means enter the object's exact vanilla
    // activation state; state 1 is an inert ready/finished observation.
    COOP_TRANSIENT_TRIGGER,
    COOP_TRANSIENT_KIND_COUNT = COOP_TRANSIENT_TRIGGER
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

typedef struct {
    unsigned enabled, file, map, epoch, revision, count;
    CoopTransientRecord records[COOP_TRANSIENT_RECORDS];
} CoopTransientInput;

typedef struct {
    unsigned status, map, epoch, count;
    CoopTransientRecord records[COOP_TRANSIENT_RECORDS];
} CoopTransientResult;

#endif
