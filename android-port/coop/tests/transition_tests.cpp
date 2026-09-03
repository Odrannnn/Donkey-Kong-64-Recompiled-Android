#include "../mod/transition_policy.h"
#include <cstdio>

#define CHECK(expr) do { if (!(expr)) { std::fprintf(stderr, "TRANSITION FAIL %d: %s\n", __LINE__, #expr); return 1; } } while (0)

int main() {
    struct Route { unsigned source, destination; int exit; };
    const Route ordinary[] = {
#define COOP_TRANSITION_ROUTE(source, destination, exit) {source, destination, exit},
#include "../mod/transition_routes.inc"
#undef COOP_TRANSITION_ROUTE
    };
    static_assert(sizeof(ordinary) / sizeof(ordinary[0]) == 169);
    static_assert(COOP_TRANSITION_ROUTE_COUNT == 169);
    static_assert(COOP_BBLAST_COMPLETION_ROUTE_COUNT == 5);
    for (const auto& item : ordinary) {
        CHECK(coop_transition_ordinary_route_allowed(item.source, item.destination, item.exit));
        CHECK(coop_transition_route_allowed(item.source, item.destination, item.exit));
        CHECK(coop_transition_valid(123, coop_transition_pack(item.source, item.destination, item.exit)));
    }

    const Route blast[] = {{37, 7, 7}, {41, 38, 11}, {54, 30, 16}, {110, 26, 15}, {187, 87, 17}};
    const int trigger_x[] = {0x056B, 0x04B0, 0x03B6, 0x04B0, 0x06C7};
    const int trigger_z[] = {0x056C, 0x0506, 0x040C, 0x0506, 0x02AA};
    for (unsigned i = 0; i < COOP_BBLAST_COMPLETION_ROUTE_COUNT; ++i) {
        const auto& item = blast[i];
        unsigned packed = coop_transition_pack(item.source, item.destination, item.exit);
        int x = 0, z = 0;
        CHECK(!coop_transition_ordinary_route_allowed(item.source, item.destination, item.exit));
        CHECK(coop_transition_bblast_completion(item.source, item.destination, item.exit));
        CHECK(coop_transition_route_is_bblast(packed));
        CHECK(coop_transition_valid(123, packed));
        CHECK(coop_transition_bblast_trigger(packed, &x, &z));
        CHECK(x == trigger_x[i] && z == trigger_z[i]);
        CHECK(!coop_transition_bblast_completion(item.source, item.destination, item.exit + 1));
    }

    CHECK(coop_transition_route_allowed(34, 169, 0));
    CHECK(coop_transition_route_allowed(169, 34, 2));
    CHECK(coop_transition_route_allowed(38, 20, 0));
    CHECK(coop_transition_route_allowed(30, 43, 2));
    CHECK(coop_transition_route_allowed(48, 64, 4));
    CHECK(coop_transition_route_allowed(72, 200, 0));
    CHECK(coop_transition_route_allowed(87, 114, 1));
    CHECK(!coop_transition_route_allowed(34, 193, 0)); // Castle cannon.
    CHECK(!coop_transition_route_allowed(34, 203, 0)); // K. Rool phase.
    CHECK(!coop_transition_route_allowed(170, 17, 0)); // Helm.
    CHECK(!coop_transition_route_allowed(7, 42, 0)); // Troff & Scoff.
    CHECK(!coop_transition_route_allowed(38, 14, 0)); // Beetle race.
    CHECK(!coop_transition_route_allowed(7, 37, 0)); // Blast entry.
    CHECK(!coop_transition_route_allowed(4, 6, 0)); // Minecart chain.
    CHECK(!coop_transition_route_allowed(62, 60, 0)); // Spider boss.
    CHECK(!coop_transition_route_allowed(188, 48, 26)); // Fungi nested bonus course.
    CHECK(!coop_transition_route_allowed(186, 72, 28)); // Caves has no direct trigger.
    CHECK(!coop_transition_route_allowed(188, 145, 0)); // Fungi nested bonus entry.
    CHECK(!coop_transition_route_allowed(186, 131, 0)); // Caves nested bonus entry.
    unsigned allowed_count = 0;
    for (unsigned source = 0; source < COOP_MAP_COUNT; ++source)
        for (unsigned destination = 0; destination < COOP_MAP_COUNT; ++destination)
            for (int exit = -128; exit < 128; ++exit)
                allowed_count += coop_transition_route_allowed(source, destination, exit) != 0;
    CHECK(allowed_count == COOP_TRANSITION_ROUTE_COUNT + COOP_BBLAST_COMPLETION_ROUTE_COUNT);

    unsigned japes = coop_transition_pack(169, 7, 0);
    unsigned japes_blast = coop_transition_pack(37, 7, 7);
    CHECK(coop_transition_valid(0, 0));
    CHECK(!coop_transition_valid(0, japes) && !coop_transition_valid(1, 0));
    CHECK(!coop_transition_valid(1, japes | 0x01000000u));

    CoopTransitionCapture capture{};
    unsigned ticket = 99, route = 99;
    coop_transition_capture(&capture, 1, 1, 0, 1, 0, 0, 169, 7, 0, &ticket, &route);
    CHECK(!ticket && !route);
    coop_transition_capture(&capture, 1, 1, 0, 1, 0, 1, 169, 7, 0, &ticket, &route);
    CHECK(ticket == 1 && route == japes);
    // Retain a ticket while the sender loads, then clear it on disconnect.
    coop_transition_capture(&capture, 1, 1, 0, 1, 0, 1, 7, 7, 0, &ticket, &route);
    CHECK(ticket == 1 && route == japes);
    coop_transition_capture(&capture, 0, 1, 0, 1, 0, 1, 7, 7, 0, &ticket, &route);
    CHECK(!ticket && !route && !capture.hold_frames);
    // Guest authority never publishes an ordinary route.
    capture.was_loading = 0;
    coop_transition_capture(&capture, 1, 0, 1, 1, 1, 1, 169, 7, 0, &ticket, &route);
    CHECK(!ticket && !route);
    // Either role publishes an exact direct finish when the caller's same-area
    // Donkey and reward-queue gates report shared_eligible.
    capture.was_loading = 0;
    coop_transition_capture(&capture, 1, 0, 1, 1, 1, 1, 37, 7, 7, &ticket, &route);
    CHECK(ticket == 2 && route == japes_blast);
    capture.hold_frames = 0; capture.was_loading = 0;
    coop_transition_capture(&capture, 1, 1, 0, 1, 1, 1, 37, 7, 7, &ticket, &route);
    CHECK(!ticket && !route);
    capture.was_loading = 0;
    coop_transition_capture(&capture, 1, 1, 1, 1, 0, 1, 37, 7, 7, &ticket, &route);
    CHECK(!ticket && !route);
    capture.was_loading = 0;
    coop_transition_capture(&capture, 1, 1, 1, 1, 1, 1, 188, 48, 26, &ticket, &route);
    CHECK(!ticket && !route);

    CoopTransitionFollow follow{};
    CHECK(coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 169, 1, 10, japes));
    coop_transition_consumed(&follow);
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 169, 1, 10, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 0, 0, 169, 1, 11, japes));
    CHECK(coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 169, 1, 11, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 0, 0, 1, 0, 169, 1, 12, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 1, 1, 169, 1, 13, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 169, 0, 14, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 169, 3, 15, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 7, 1, 16, japes));
    CHECK(!coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 169, 1, 16, japes));
    CHECK(!coop_transition_should_follow(&follow, 0, 1, 0, 1, 0, 169, 1, 17, japes));
    CHECK(coop_transition_should_follow(&follow, 1, 1, 0, 1, 0, 169, 1, 17, japes));

    CoopTransitionFollow blast_follow{};
    CHECK(!coop_transition_should_follow(&blast_follow, 1, 0, 0, 1, 0, 37, 1, 20, japes_blast));
    CHECK(coop_transition_should_follow(&blast_follow, 1, 0, 1, 1, 0, 37, 1, 20, japes_blast));
    coop_transition_consumed(&blast_follow);
    CHECK(!coop_transition_should_follow(&blast_follow, 1, 0, 1, 1, 0, 37, 1, 20, japes_blast));
    CHECK(!coop_transition_should_follow(&blast_follow, 1, 1, 0, 1, 0, 37, 1, 21, japes_blast));
    CHECK(coop_transition_should_follow(&blast_follow, 1, 0, 1, 1, 0, 37, 1, 21, japes_blast));
    std::puts("PASS: ordinary host-follow routes and five bidirectional direct Baboon Blast finishes");
}
