#ifndef COOP_WORLD_TYPES_H
#define COOP_WORLD_TYPES_H
// Reversible state is deliberately separate from permanent item ownership.
enum { COOP_WORLD_TOGGLES = 3, COOP_WORLD_MASK = 7, COOP_WORLD_WIRE_WORDS = 19 };
typedef struct {
    unsigned enabled, file, ready, values, change[COOP_WORLD_TOGGLES];
    unsigned session_hi, session_lo, scope;
} CoopWorldInput;
typedef struct {
    unsigned status, desired, apply, pending, session_hi, session_lo, scope;
} CoopWorldResult;
#endif
