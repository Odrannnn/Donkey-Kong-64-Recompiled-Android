#include <cstdio>

static unsigned checks;
#define CHECK(value) do { checks++; if (!(value)) { \
    std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)

#include "../mod/lifecycle_policy.h"

int main() {
    CoopMapLifecycle lifecycle{-1, 0};
    CHECK(coop_map_lifecycle_changed(&lifecycle, 7, 0));
    CHECK(!coop_map_lifecycle_changed(&lifecycle, 7, 0));

    // The new event distinguishes a same-map reload from an ordinary frame.
    CHECK(coop_map_lifecycle_changed(&lifecycle, 7, 1));
    CHECK(!coop_map_lifecycle_changed(&lifecycle, 7, 1));

    // One event plus a new map is one lifecycle edge, not two.
    CHECK(coop_map_lifecycle_changed(&lifecycle, 38, 2));
    CHECK(!coop_map_lifecycle_changed(&lifecycle, 38, 2));

    // The map-number fallback still catches a transition without an event.
    CHECK(coop_map_lifecycle_changed(&lifecycle, 48, 2));
    CHECK(!coop_map_lifecycle_changed(&lifecycle, 48, 2));

    std::printf("PASS: %u exact map-event and polling-fallback lifecycle checks\n", checks);
}
