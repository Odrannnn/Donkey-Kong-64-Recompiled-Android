// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "campaign_save.h"

extern u8 D_global_asm_807ECEA8[];
extern u8 current_file;
extern volatile u32 ap_gameplay_mode;

static u8* trailer(void) { return &D_global_asm_807ECEA8[AP_CAMPAIGN_TRAILER_OFFSET]; }
static void copy_bytes(void* destination, const void* source) {
    u8* out = destination; const u8* in = source;
    for (unsigned i = 0; i < sizeof(ap_campaign_save); ++i) out[i] = in[i];
}
static int all_zero(const ap_campaign_save* save) {
    const u8* bytes = (const u8*)save;
    for (unsigned i = 0; i < sizeof(*save); ++i) if (bytes[i]) return 0;
    return 1;
}
static int same_binding(const ap_campaign_save* a, const ap_campaign_save* b) {
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) if (a->binding[i] != b->binding[i]) return 0;
    return 1;
}
static int supported(const ap_campaign_save* save) {
    return ap_campaign_save_valid(save) && save->magic == AP_CAMPAIGN_SAVE_MAGIC
        && save->format == AP_CAMPAIGN_SAVE_FORMAT;
}

int ap_campaign_trailer_read(ap_campaign_save* output) {
    if (!output || current_file != 0) return -1;
    copy_bytes(output, trailer());
    if (all_zero(output)) return 0;
    return supported(output) ? 1 : -1;
}

int ap_campaign_trailer_stage(const ap_campaign_save* source) {
    ap_campaign_save candidate, previous;
    if (!source || current_file != 0) return 0;
    copy_bytes(&candidate, source);
    if (!supported(&candidate)) return 0;
    int state = ap_campaign_trailer_read(&previous);
    if (state < 0) return 0;
    if (!state) {
        if (candidate.counter != 0 || (ap_gameplay_mode & AP_GAMEPLAY_SAVE)) return 0;
    } else {
        if (!same_binding(&candidate, &previous)) return 0;
        if (candidate.counter != previous.counter) {
            if (!(ap_gameplay_mode & AP_GAMEPLAY_SAVE) || previous.counter == 0xFFFFu
                || candidate.counter != (unsigned)previous.counter + 1u) return 0;
        }
    }
    copy_bytes(trailer(), &candidate);
    return 1;
}
