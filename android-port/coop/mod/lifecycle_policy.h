#ifndef DKCOOP_LIFECYCLE_POLICY_H
#define DKCOOP_LIFECYCLE_POLICY_H

typedef struct {
    int map;
    unsigned event_serial;
} CoopMapLifecycle;

// A 1.0.2 map-load event catches same-map reloads that a map-number poll
// cannot observe. The map comparison remains a fail-safe for the initial frame
// and for any transition whose event is delayed or unexpectedly absent.
static unsigned coop_map_lifecycle_changed(CoopMapLifecycle* lifecycle,
        unsigned map, unsigned event_serial) {
    unsigned changed = lifecycle->event_serial != event_serial
        || lifecycle->map != (int)map;
    lifecycle->event_serial = event_serial;
    lifecycle->map = (int)map;
    return changed;
}

#endif
