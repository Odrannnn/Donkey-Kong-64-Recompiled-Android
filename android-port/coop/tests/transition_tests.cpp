#include "../mod/transition_policy.h"
#include <cstdio>

#define CHECK(expr) do { if (!(expr)) { std::fprintf(stderr, "TRANSITION FAIL %d: %s\n", __LINE__, #expr); return 1; } } while (0)

int main() {
    struct Route { unsigned source, destination; int exit; };
    const Route reviewed[] = {
#define COOP_TRANSITION_ROUTE(source, destination, exit) {source, destination, exit},
#include "../mod/transition_routes.inc"
#undef COOP_TRANSITION_ROUTE
    };
    static_assert(sizeof(reviewed) / sizeof(reviewed[0]) == 169);
    static_assert(COOP_TRANSITION_ROUTE_COUNT == 169);
    for (const auto& item : reviewed) {
        CHECK(coop_transition_route_allowed(item.source, item.destination, item.exit));
        CHECK(coop_transition_valid(123, coop_transition_pack(item.source, item.destination, item.exit)));
    }
    CHECK(coop_transition_route_allowed(34, 169, 0));
    CHECK(coop_transition_route_allowed(169, 34, 2));
    CHECK(coop_transition_route_allowed(169, 7, 0));
    CHECK(coop_transition_route_allowed(7, 169, 1));
    CHECK(coop_transition_route_allowed(193, 87, 0));
    CHECK(coop_transition_route_allowed(38, 20, 0)); // Aztec Llama Temple.
    CHECK(coop_transition_route_allowed(30, 43, 2)); // Galleon Lanky 5D ship.
    CHECK(coop_transition_route_allowed(48, 64, 4)); // Upper mushroom door.
    CHECK(coop_transition_route_allowed(72, 200, 0)); // Diddy upper cabin.
    CHECK(coop_transition_route_allowed(87, 114, 1)); // Castle library exit door.
    CHECK(coop_transition_route_allowed(151, 163, 0)); // Castle upper cave to dungeon.
    CHECK(!coop_transition_route_allowed(34, 193, 0)); // Scripted Castle cannon.
    CHECK(!coop_transition_route_allowed(34, 203, 0)); // K. Rool DK phase.
    CHECK(!coop_transition_route_allowed(170, 17, 0)); // Helm lobby/level.
    CHECK(!coop_transition_route_allowed(7, 42, 0)); // Troff & Scoff.
    CHECK(!coop_transition_route_allowed(38, 14, 0)); // Race.
    CHECK(!coop_transition_route_allowed(7, 37, 0)); // Bonus barrel.
    CHECK(!coop_transition_route_allowed(4, 6, 0)); // Minecart chain.
    CHECK(!coop_transition_route_allowed(88, 113, 2)); // Castle Monkeyport.
    CHECK(!coop_transition_route_allowed(30, 51, 0)); // Mech Fish scripted challenge.
    CHECK(!coop_transition_route_allowed(62, 60, 0)); // Spider boss.
    CHECK(!coop_transition_route_allowed(169, 7, -1)); // Not the pinned US exit.
    unsigned allowed_count = 0;
    for (unsigned source = 0; source < COOP_MAP_COUNT; ++source)
        for (unsigned destination = 0; destination < COOP_MAP_COUNT; ++destination)
            for (int exit = -128; exit < 128; ++exit)
                allowed_count += coop_transition_route_allowed(source, destination, exit) != 0;
    CHECK(allowed_count == COOP_TRANSITION_ROUTE_COUNT);
    unsigned japes = coop_transition_pack(169, 7, 0);
    CHECK(coop_transition_source(japes) == 169 && coop_transition_destination(japes) == 7
        && coop_transition_exit(japes) == 0 && coop_transition_valid(1, japes));
    CHECK(coop_transition_valid(0, 0));
    CHECK(!coop_transition_valid(0, japes) && !coop_transition_valid(1, 0));
    CHECK(!coop_transition_valid(1, japes | 0x01000000u));

    CoopTransitionCapture capture{};
    unsigned ticket = 99, route = 99;
    coop_transition_capture(&capture, 1, 1, 0, 169, 7, 0, &ticket, &route);
    CHECK(!ticket && !route);
    coop_transition_capture(&capture, 1, 1, 1, 169, 7, 0, &ticket, &route);
    CHECK(ticket == 1 && route == japes);
    // The ticket survives host arrival and packet loss for a bounded window.
    coop_transition_capture(&capture, 1, 1, 1, 7, 7, 0, &ticket, &route);
    CHECK(ticket == 1 && route == japes);
    coop_transition_capture(&capture, 1, 1, 0, 7, 7, 0, &ticket, &route);
    CHECK(ticket == 1 && route == japes);
    // A later same-map eligible rising edge receives a distinct ticket.
    capture.was_loading = 0;
    coop_transition_capture(&capture, 1, 1, 1, 7, 169, 1, &ticket, &route);
    CHECK(ticket == 2 && coop_transition_source(route) == 7 && coop_transition_destination(route) == 169);
    // Losing the peer/session clears retained publication before a later guest can join.
    coop_transition_capture(&capture, 0, 1, 1, 7, 169, 1, &ticket, &route);
    CHECK(!ticket && !route && !capture.hold_frames);
    // Ineligible, unsupported and join-side observations never arm.
    capture.was_loading = 0; capture.hold_frames = 0;
    coop_transition_capture(&capture, 1, 0, 1, 7, 169, 1, &ticket, &route);
    CHECK(!ticket && !route);
    capture.was_loading = 0;
    coop_transition_capture(&capture, 1, 1, 1, 7, 42, 0, &ticket, &route);
    CHECK(!ticket && !route);
    coop_transition_capture(&capture, 0, 1, 1, 169, 7, -1, &ticket, &route);
    CHECK(!ticket && !route);

    CoopTransitionFollow follow{};
    follow.connected = 1;
    follow.seen_ticket = 9;
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 0, 0));
    CHECK(follow.seen_ticket == 9 && !follow.pending);
    CHECK(coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 10, japes));
    coop_transition_consumed(&follow);
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 10, japes));
    // Unsafe receipt is retryable while the guest remains in the source map.
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 0, 169, 1, 11, japes));
    CHECK(coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 11, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 0, 1, 0, 169, 1, 12, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 1, 169, 1, 13, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 0, 14, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 3, 15, japes));
    // First observation outside the recorded source is permanently stale for this session.
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 7, 1, 16, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 16, japes));
    // Leaving after an eligible receipt also cancels the old ticket.
    CHECK(coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 17, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 7, 1, 17, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 17, japes));
    // Disconnect resets ticket history for a newly negotiated session.
    CHECK(!coop_transition_should_follow(&follow, 0, 1, 1, 0, 169, 1, 17, japes));
    CHECK(coop_transition_should_follow(&follow, 1, 1, 1, 0, 169, 1, 17, japes));
    std::puts("PASS: reviewed route allowlist, connected capture, guest safety gates and stale replay rejection");
}
