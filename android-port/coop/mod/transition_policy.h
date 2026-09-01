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

enum {
    COOP_TRANSITION_ROUTE_COUNT =
        sizeof(coop_transition_routes) / sizeof(coop_transition_routes[0])
};

// Ordinary vanilla portals, doors and tunnels with pinned US exits. Scripted
// Castle-cannon, Helm, boss, T&S, cart, race, bonus and cutscene routes stay local.
static inline unsigned coop_transition_route_allowed(unsigned source, unsigned destination, int exit) {
    for (unsigned index = 0; index < COOP_TRANSITION_ROUTE_COUNT; ++index) {
        const CoopTransitionRoute* route = &coop_transition_routes[index];
        if (route->source == source && route->destination == destination && route->exit == exit)
            return 1;
    }
    return 0;
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

static inline void coop_transition_capture(CoopTransitionCapture* capture, unsigned host,
        unsigned eligible, unsigned loading, unsigned source_map, int next_map, int next_exit,
        unsigned* ticket, unsigned* route) {
    *ticket = 0;
    *route = 0;
    if (!host) {
        capture->was_loading = loading != 0;
        capture->hold_frames = 0;
        return;
    }
    if (loading && !capture->was_loading && eligible && source_map < COOP_MAP_COUNT
            && next_map >= 0 && next_map < COOP_MAP_COUNT
            && coop_transition_route_allowed(source_map, (unsigned)next_map, next_exit)) {
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
        unsigned connected, unsigned join, unsigned playing, unsigned loading,
        unsigned local_map, unsigned remote_flags, unsigned ticket, unsigned route) {
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
    return follow->pending && join && playing && !loading && local_map == source
        && (remote_flags & 1u) && !(remote_flags & 2u);
}

static inline void coop_transition_consumed(CoopTransitionFollow* follow) { follow->pending = 0; }

#endif
