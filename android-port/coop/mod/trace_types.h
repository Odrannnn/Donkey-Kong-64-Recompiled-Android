#ifndef COOP_TRACE_TYPES_H
#define COOP_TRACE_TYPES_H

enum {
    COOP_TRACE_VERSION = 53,
    COOP_TRACE_PLAYING = 1u << 0,
    COOP_TRACE_CUTSCENE = 1u << 1,
    COOP_TRACE_LOADING = 1u << 2,
    COOP_TRACE_ITEM_SAFE_MAP = 1u << 3,
    COOP_TRACE_ITEM_SNAPSHOT_MAP = 1u << 4,
    COOP_TRACE_REWARD_QUEUE = 1u << 5,
    COOP_TRACE_HUD_READY = 1u << 6,
    COOP_TRACE_REFRESH_PENDING = 1u << 7,
    COOP_TRACE_SAVE_PENDING = 1u << 8,
    COOP_TRACE_WORLD_SAVE_PENDING = 1u << 9,
    COOP_TRACE_RECOVERY_CHECKPOINT = 1u << 10,
    COOP_TRACE_PROMOTED_HOST = 1u << 11,
    COOP_TRACE_RECOVERY_PERSIST_SAFE = 1u << 12,
};

enum {
    COOP_TRACE_WAIT_NONE = 0,
    COOP_TRACE_WAIT_LOCAL_AREA = 1,
    COOP_TRACE_WAIT_REWARD_QUEUE = 2,
    COOP_TRACE_WAIT_HUD = 3,
    COOP_TRACE_WAIT_SAME_LEVEL_ITEM = 4,
    COOP_TRACE_WAIT_SAVE_UNSAFE = 5,
    COOP_TRACE_WAIT_PROGRESSION_CONTEXT = 6,
    COOP_TRACE_WAIT_TROFF = 7,
    COOP_TRACE_WAIT_COUNTER = 8,
    COOP_TRACE_WAIT_FILE = 9,
    COOP_TRACE_WAIT_SNAPSHOT = 10,
    COOP_TRACE_WAIT_WORLD_REFRESH = 11,
};

typedef struct {
    unsigned version, flags, level;
    unsigned item_deferred, item_baseline, item_bound, item_live_snapshot;
    unsigned item_wait_reason, item_wait_id, item_refresh_map, item_result_status;
    unsigned world_result_status, world_pending, transient_status, combat_status;
    unsigned recovery_state, recovery_fingerprint;
} CoopTraceInput;

#endif
