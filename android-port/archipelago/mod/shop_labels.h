// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
// Fixed-width, pointer-free game-thread input. Never pass server text directly
// to the game's formatter: '~', '%' and other control characters are excluded.
#define AP_LABEL_LENGTH 33
typedef struct {
    unsigned int location;
    unsigned short frames, reserved;
    char item[AP_LABEL_LENGTH], subtitle[AP_LABEL_LENGTH];
    unsigned char padding[2];
} ap_shop_label;
#if defined(__cplusplus)
static_assert(sizeof(ap_shop_label) == 76);
#else
_Static_assert(sizeof(ap_shop_label) == 76, "Shop label ABI");
#endif
static inline int ap_label_text_valid(const char* text, int allow_empty) {
    int ended = 0, visible = 0;
    for (unsigned int i = 0; i < AP_LABEL_LENGTH; ++i) {
        unsigned char c = text[i];
        if (!c) { ended = 1; continue; }
        if (ended || !((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ')) return 0;
        visible |= c != ' ';
    }
    return ended && (visible || allow_empty);
}
static inline int ap_label_valid(const ap_shop_label* label) {
    return label && label->frames >= 50 && label->frames <= 255 && !label->reserved
        && !label->padding[0] && !label->padding[1]
        && ap_label_text_valid(label->item, 0) && ap_label_text_valid(label->subtitle, 1);
}
