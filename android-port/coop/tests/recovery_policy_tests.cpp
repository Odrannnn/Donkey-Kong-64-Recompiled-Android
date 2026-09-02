#include <cstdio>

#include "../mod/recovery_policy.h"

static unsigned checks;
#define CHECK(value) do { checks++; if (!(value)) { \
    std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)

int main() {
    CHECK(coop_recovery_use_host_copy(1, 0));
    CHECK(!coop_recovery_use_host_copy(2, 0));
    CHECK(coop_recovery_use_host_copy(2, 1));
    CHECK(!coop_recovery_use_host_copy(1, 2));

    CoopHostRecovery recovery{};
    CHECK(coop_host_recovery_update(&recovery, 1, 1, 1, 1,
        COOP_RECOVERY_STANDBY) == COOP_RECOVERY_BUILDING);
    for (unsigned i = 1; i < COOP_RECOVERY_STABLE_FRAMES; ++i)
        coop_host_recovery_update(&recovery, 1, 1, 1, 1, COOP_RECOVERY_STANDBY);
    CHECK(recovery.checkpoint);
    CHECK(coop_host_recovery_update(&recovery, 1, 1, 1, 1,
        COOP_RECOVERY_STANDBY) == COOP_RECOVERY_READY);

    // A pending persistent change before loss invalidates the old checkpoint.
    CHECK(coop_host_recovery_update(&recovery, 1, 1, 0, 1,
        COOP_RECOVERY_STANDBY) == COOP_RECOVERY_BUILDING);
    CHECK(!recovery.checkpoint);
    CHECK(coop_host_recovery_update(&recovery, 1, 0, 0, 1,
        COOP_RECOVERY_PROMOTE) == COOP_RECOVERY_NO_CHECKPOINT);

    for (unsigned i = 0; i < COOP_RECOVERY_STABLE_FRAMES; ++i)
        coop_host_recovery_update(&recovery, 1, 1, 1, 1, COOP_RECOVERY_STANDBY);
    CHECK(recovery.checkpoint);
    CHECK(coop_host_recovery_update(&recovery, 1, 0, 0, 0,
        COOP_RECOVERY_PROMOTE) == COOP_RECOVERY_WAIT_SAFE);
    CHECK(coop_host_recovery_update(&recovery, 1, 0, 0, 1,
        COOP_RECOVERY_PROMOTE) == COOP_RECOVERY_START);
    coop_host_recovery_complete(&recovery, 0);
    CHECK(coop_host_recovery_update(&recovery, 1, 0, 0, 1,
        COOP_RECOVERY_PROMOTE) == COOP_RECOVERY_FAILED);

    // Returning to standby explicitly permits a retry.
    CHECK(coop_host_recovery_update(&recovery, 1, 0, 0, 1,
        COOP_RECOVERY_STANDBY) == COOP_RECOVERY_READY);
    CHECK(coop_host_recovery_update(&recovery, 1, 0, 0, 1,
        COOP_RECOVERY_PROMOTE) == COOP_RECOVERY_START);
    coop_host_recovery_complete(&recovery, 1);
    CHECK(coop_host_recovery_update(&recovery, 0, 0, 0, 0,
        COOP_RECOVERY_OFF) == COOP_RECOVERY_ACTIVE);

    std::printf("PASS: %u conservative checkpoint and manual promotion checks\n", checks);
}
