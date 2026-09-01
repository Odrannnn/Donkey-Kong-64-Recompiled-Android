#ifndef DKCOOP_ANIMATION_POLICY_H
#define DKCOOP_ANIMATION_POLICY_H

// US 1.0.1's local raw-pose table has 110 rows and seven character columns.
// Wire values are row + 1 (zero means no pose), never asset IDs or scripts.
#define COOP_ANIMATION_ROWS 110u
#define COOP_ANIMATION_COLUMNS 7u
#define COOP_ANIMATION_MAX_FRAME 254.0f

static inline unsigned coop_animation_rows(unsigned start, unsigned end) {
    if (start < 16 || start > 131072 || end < start || end > 131072 || (start & 1)) return 0;
    return end - start == COOP_ANIMATION_ROWS * COOP_ANIMATION_COLUMNS * 2 ? COOP_ANIMATION_ROWS : 0;
}

static inline int coop_animation_clip(const unsigned short* table, unsigned rows, unsigned kong, unsigned pose) {
    if (!table || rows != COOP_ANIMATION_ROWS || kong >= 5 || !pose || pose > rows) return -1;
    unsigned clip = table[(pose - 1) * COOP_ANIMATION_COLUMNS + kong];
    return clip < 2048 ? (int)clip : -1;
}

static inline unsigned coop_animation_find(const unsigned short* table, unsigned rows, unsigned kong, int clip) {
    if (!table || rows != COOP_ANIMATION_ROWS || kong >= 5 || clip < 0 || clip >= 2048) return 0;
    for (unsigned row = 1; row <= rows; ++row)
        if (coop_animation_clip(table, rows, kong, row) == clip) return row;
    return 0;
}

static inline int coop_animation_valid_frame(float frame) {
    // Both comparisons reject NaN; the bounds also reject infinities.
    return frame >= 0.0f && frame <= COOP_ANIMATION_MAX_FRAME;
}

static inline float coop_animation_frame(float frame, unsigned count) {
    if (!coop_animation_valid_frame(frame) || count == 0 || count > 255) return 0.0f;
    float last = (float)(count - 1);
    return frame < last ? frame : last;
}
#endif
