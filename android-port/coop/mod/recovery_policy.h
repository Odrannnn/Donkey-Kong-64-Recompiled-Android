#ifndef DKCOOP_RECOVERY_POLICY_H
#define DKCOOP_RECOVERY_POLICY_H

enum {
    COOP_RECOVERY_OFF = 0,
    COOP_RECOVERY_STANDBY = 1,
    COOP_RECOVERY_PROMOTE = 2,
    COOP_RECOVERY_STABLE_FRAMES = 120,
};

enum {
    COOP_RECOVERY_NONE = 0,
    COOP_RECOVERY_BUILDING = 1,
    COOP_RECOVERY_READY = 2,
    COOP_RECOVERY_NO_CHECKPOINT = 3,
    COOP_RECOVERY_WAIT_SAFE = 4,
    COOP_RECOVERY_START = 5,
    COOP_RECOVERY_FAILED = 6,
    COOP_RECOVERY_ACTIVE = 7,
};

typedef struct {
    unsigned stable_frames;
    unsigned checkpoint;
    unsigned attempted;
    unsigned promoted;
    unsigned failed;
} CoopHostRecovery;

// Save-copy selection is independent from the live UDP role during recovery.
// 0 follows the role, 1 selects the host copy, and 2 selects the guest copy.
static unsigned coop_recovery_use_host_copy(unsigned role, unsigned copy) {
    return copy == 1 || (copy == 0 && role == 1);
}

// A checkpoint is armed only after both persistent channels have remained
// fully converged and locally save-safe for a sustained period. Disconnecting
// preserves that last acknowledged checkpoint, while any observed divergence
// before the disconnect invalidates it.
static unsigned coop_host_recovery_update(CoopHostRecovery* recovery,
        unsigned join, unsigned connected, unsigned checkpoint_safe,
        unsigned promotion_safe, unsigned command) {
    if (!join) return recovery->promoted ? COOP_RECOVERY_ACTIVE : COOP_RECOVERY_NONE;
    if (connected) {
        recovery->attempted = recovery->failed = 0;
        if (!checkpoint_safe) {
            recovery->stable_frames = 0;
            recovery->checkpoint = 0;
            return command ? COOP_RECOVERY_BUILDING : COOP_RECOVERY_NONE;
        }
        if (recovery->stable_frames < COOP_RECOVERY_STABLE_FRAMES)
            recovery->stable_frames++;
        if (recovery->stable_frames >= COOP_RECOVERY_STABLE_FRAMES)
            recovery->checkpoint = 1;
        if (!command) return COOP_RECOVERY_NONE;
        return recovery->checkpoint ? COOP_RECOVERY_READY : COOP_RECOVERY_BUILDING;
    }
    recovery->stable_frames = 0;
    if (command != COOP_RECOVERY_PROMOTE) {
        recovery->attempted = recovery->failed = 0;
        return recovery->checkpoint && command == COOP_RECOVERY_STANDBY
            ? COOP_RECOVERY_READY : COOP_RECOVERY_NONE;
    }
    if (recovery->failed) return COOP_RECOVERY_FAILED;
    if (!recovery->checkpoint) return COOP_RECOVERY_NO_CHECKPOINT;
    if (!promotion_safe) return COOP_RECOVERY_WAIT_SAFE;
    if (recovery->attempted) return COOP_RECOVERY_FAILED;
    recovery->attempted = 1;
    return COOP_RECOVERY_START;
}

static void coop_host_recovery_complete(CoopHostRecovery* recovery, unsigned success) {
    recovery->failed = !success;
    recovery->promoted = success;
}

#endif
