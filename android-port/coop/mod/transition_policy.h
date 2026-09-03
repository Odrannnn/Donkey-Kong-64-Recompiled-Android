#ifndef DKCOOP_TRANSITION_POLICY_H
#define DKCOOP_TRANSITION_POLICY_H

enum { COOP_MAP_COUNT = 216, COOP_TRANSITION_HOLD_FRAMES = 300 };

typedef struct {
    unsigned char source;
    unsigned char destination;
    signed char exit;
} CoopTransitionRoute;

static const CoopTransitionRoute coop_transition_routes[] = {
#define COOP_TRANSITION_ROUTE(source, destination, exit) {source, destination, exit},
#include "transition_routes.inc"
#undef COOP_TRANSITION_ROUTE
};

// Five Baboon Blast courses have direct scripted completion exits. Fungi and
// Caves instead end at nested bonus barrels, so neither course-to-parent route
// belongs to this channel.
static const CoopTransitionRoute coop_bblast_completion_routes[] = {
    {37, 7, 7},    // Japes
    {41, 38, 11},  // Aztec
    {54, 30, 16},  // Galleon
    {110, 26, 15}, // Factory
    {187, 87, 17}, // Castle
};

enum {
    COOP_TRANSITION_ROUTE_COUNT =
        sizeof(coop_transition_routes) / sizeof(coop_transition_routes[0]),
    COOP_BBLAST_COMPLETION_ROUTE_COUNT =
        sizeof(coop_bblast_completion_routes) / sizeof(coop_bblast_completion_routes[0])
};

// Ordinary vanilla portals, doors and tunnels with pinned US exits.
static inline unsigned coop_transition_ordinary_route_allowed(unsigned source, unsigned destination, int exit) {
    for (unsigned index = 0; index < COOP_TRANSITION_ROUTE_COUNT; ++index) {
        const CoopTransitionRoute* route = &coop_transition_routes[index];
        if (route->source == source && route->destination == destination && route->exit == exit)
            return 1;
    }
    return 0;
}

static inline unsigned coop_transition_bblast_completion(unsigned source, unsigned destination, int exit) {
    for (unsigned index = 0; index < COOP_BBLAST_COMPLETION_ROUTE_COUNT; ++index) {
        const CoopTransitionRoute* route = &coop_bblast_completion_routes[index];
        if (route->source == source && route->destination == destination && route->exit == exit)
            return 1;
    }
    return 0;
}

static inline unsigned coop_transition_route_allowed(unsigned source, unsigned destination, int exit) {
    return coop_transition_ordinary_route_allowed(source, destination, exit)
        || coop_transition_bblast_completion(source, destination, exit);
}

static inline unsigned coop_transition_pack(unsigned source, unsigned destination, int exit) {
    return (source & 0xFFu) | ((destination & 0xFFu) << 8) | (((unsigned)exit & 0xFFu) << 16);
}
static inline unsigned coop_transition_source(unsigned route) { return route & 0xFFu; }
static inline unsigned coop_transition_destination(unsigned route) { return (route >> 8) & 0xFFu; }
static inline int coop_transition_exit(unsigned route) {
    int exit = (int)((route >> 16) & 0xFFu);
    return exit >= 128 ? exit - 256 : exit;
}
static inline unsigned coop_transition_route_is_bblast(unsigned route) {
    return !(route & 0xFF000000u)
        && coop_transition_bblast_completion(coop_transition_source(route),
            coop_transition_destination(route), coop_transition_exit(route));
}

// Pinned centers of the five vanilla type-0x0C trigger records. The full
// loading-zone dispatcher is followed by the same player action vanilla uses.
static inline unsigned coop_transition_bblast_trigger(unsigned route, int* x, int* z) {
    if (!coop_transition_route_is_bblast(route)) return 0;
    switch (coop_transition_source(route)) {
        case 37: *x = 0x056B; *z = 0x056C; return 1;
        case 41: *x = 0x04B0; *z = 0x0506; return 1;
        case 54: *x = 0x03B6; *z = 0x040C; return 1;
        case 110: *x = 0x04B0; *z = 0x0506; return 1;
        case 187: *x = 0x06C7; *z = 0x02AA; return 1;
        default: return 0;
    }
}
static inline unsigned coop_transition_valid(unsigned ticket, unsigned route) {
    if (!ticket || !route || (route & 0xFF000000u)) return !ticket && !route;
    unsigned source = coop_transition_source(route), destination = coop_transition_destination(route);
    return source < COOP_MAP_COUNT && destination < COOP_MAP_COUNT
        && coop_transition_route_allowed(source, destination, coop_transition_exit(route));
}

typedef struct {
    unsigned was_loading;
    unsigned ticket_counter;
    unsigned ticket;
    unsigned route;
    unsigned hold_frames;
} CoopTransitionCapture;

typedef struct {
    unsigned connected;
    unsigned seen_ticket;
    unsigned pending;
    unsigned source_map;
} CoopTransitionFollow;

static inline void coop_transition_capture(CoopTransitionCapture* capture, unsigned connected,
        unsigned host, unsigned shared_events, unsigned ordinary_eligible,
        unsigned shared_eligible, unsigned loading, unsigned source_map, int next_map,
        int next_exit, unsigned* ticket, unsigned* route) {
    *ticket = 0;
    *route = 0;
    if (!connected) {
        capture->was_loading = loading != 0;
        capture->hold_frames = 0;
        capture->ticket = 0;
        capture->route = 0;
        return;
    }
    unsigned ordinary = next_map >= 0 && next_map < COOP_MAP_COUNT
        && coop_transition_ordinary_route_allowed(source_map, (unsigned)next_map, next_exit);
    unsigned bblast = next_map >= 0 && next_map < COOP_MAP_COUNT
        && coop_transition_bblast_completion(source_map, (unsigned)next_map, next_exit);
    if (loading && !capture->was_loading && source_map < COOP_MAP_COUNT
            && ((host && ordinary && ordinary_eligible)
                || (shared_events && bblast && shared_eligible))) {
        if (!++capture->ticket_counter) ++capture->ticket_counter;
        capture->ticket = capture->ticket_counter;
        capture->route = coop_transition_pack(source_map, (unsigned)next_map, next_exit);
        capture->hold_frames = COOP_TRANSITION_HOLD_FRAMES;
    }
    capture->was_loading = loading != 0;
    if (capture->hold_frames && coop_transition_valid(capture->ticket, capture->route)) {
        --capture->hold_frames;
        *ticket = capture->ticket;
        *route = capture->route;
    }
}

static inline unsigned coop_transition_should_follow(CoopTransitionFollow* follow,
        unsigned connected, unsigned ordinary_follow, unsigned shared_events,
        unsigned playing, unsigned loading, unsigned local_map, unsigned remote_flags,
        unsigned ticket, unsigned route) {
    if (!connected) {
        follow->connected = 0;
        follow->seen_ticket = 0;
        follow->pending = 0;
        return 0;
    }
    if (!follow->connected) {
        follow->connected = 1;
        follow->seen_ticket = 0;
        follow->pending = 0;
    }
    if (!ticket && !route) return 0;
    if (!coop_transition_valid(ticket, route)) return 0;
    unsigned source = coop_transition_source(route);
    if (ticket != follow->seen_ticket) {
        follow->seen_ticket = ticket;
        follow->source_map = source;
        follow->pending = local_map == source;
    } else if (follow->pending && local_map != follow->source_map) {
        follow->pending = 0;
    }
    unsigned authorized = (ordinary_follow
            && coop_transition_ordinary_route_allowed(source,
                coop_transition_destination(route), coop_transition_exit(route)))
        || (shared_events && coop_transition_route_is_bblast(route));
    return follow->pending && authorized && playing && !loading && local_map == source
        && (remote_flags & 1u) && !(remote_flags & 2u);
}

static inline void coop_transition_consumed(CoopTransitionFollow* follow) { follow->pending = 0; }

#endif
