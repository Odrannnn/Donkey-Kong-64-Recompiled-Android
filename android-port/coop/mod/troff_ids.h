#ifndef COOP_TROFF_IDS_H
#define COOP_TROFF_IDS_H
#include "progression_ids.h"
// Each bucket owns the contiguous prefix 1..amount_fed. OR therefore means
// max(amount_fed), never the sum of two players spending copied inventory.
enum {
    COOP_TROFF_FIRST = COOP_PROGRESSION_END, COOP_TROFF_LEVELS = 7,
    COOP_TROFF_KONGS = 5, COOP_TROFF_CAPACITY = 100,
    COOP_TROFF_BUCKETS = COOP_TROFF_LEVELS * COOP_TROFF_KONGS,
    COOP_TROFF_END = COOP_TROFF_FIRST + COOP_TROFF_BUCKETS * COOP_TROFF_CAPACITY
};
static inline unsigned coop_troff_id(unsigned level, unsigned kong, unsigned amount) {
    if (level >= COOP_TROFF_LEVELS || kong >= COOP_TROFF_KONGS || !amount || amount > COOP_TROFF_CAPACITY)
        return COOP_TROFF_END;
    return COOP_TROFF_FIRST + (level * COOP_TROFF_KONGS + kong) * COOP_TROFF_CAPACITY + amount - 1;
}
static inline unsigned coop_troff_bucket(unsigned id, unsigned* level, unsigned* kong, unsigned* amount) {
    if (id < COOP_TROFF_FIRST || id >= COOP_TROFF_END) return 0;
    unsigned index = id - COOP_TROFF_FIRST, bucket = index / COOP_TROFF_CAPACITY;
    *level = bucket / COOP_TROFF_KONGS; *kong = bucket % COOP_TROFF_KONGS;
    *amount = index % COOP_TROFF_CAPACITY + 1;
    return 1;
}
#endif
