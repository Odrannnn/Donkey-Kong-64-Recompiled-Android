// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_checks.h"

extern unsigned char current_file;
extern volatile unsigned int ap_gameplay_mode;

static ap_check_set campaign_selected;
static ap_check_set campaign_observed;
static ap_campaign_save campaign_identity;
static unsigned char campaign_ready;

static int aligned(const void* pointer) { return !((unsigned int)pointer & 3u); }

static void copy_set(ap_check_set* destination, const ap_check_set* source) {
    unsigned char* out = (unsigned char*)destination;
    const unsigned char* in = (const unsigned char*)source;
    for (unsigned int i = 0; i < sizeof(*destination); ++i) out[i] = in[i];
}

static void clear_set(ap_check_set* set) {
    for (unsigned int i = 0; i < AP_LOCATION_WORDS; ++i) set->words[i] = 0;
}

static int subset(const ap_check_set* part, const ap_check_set* whole) {
    if (!ap_check_valid(part)) return 0;
    for (unsigned int i = 0; i < AP_LOCATION_WORDS; ++i)
        if (part->words[i] & ~whole->words[i]) return 0;
    return 1;
}

static int same_identity(const ap_campaign_save* a, const ap_campaign_save* b) {
    if (a->magic != b->magic || a->format != b->format || a->counter != b->counter
        || a->reserved != b->reserved || a->checksum != b->checksum) return 0;
    for (unsigned int i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i)
        if (a->binding[i] != b->binding[i]) return 0;
    return 1;
}

static int current_identity(const ap_campaign_save* identity) {
    ap_campaign_save trailer;
    return identity->magic == AP_CAMPAIGN_SAVE_MAGIC && identity->format == AP_CAMPAIGN_SAVE_FORMAT
        && ap_campaign_save_valid(identity) && ap_campaign_trailer_read(&trailer) == 1
        && same_identity(identity, &trailer);
}

static int current_campaign(void) {
    return campaign_ready && current_file == 0 && current_identity(&campaign_identity);
}

static int validate(const ap_campaign_save* identity, const ap_check_set* selected, const ap_check_set* restored,
                    ap_campaign_save* identity_copy, ap_check_set* selected_copy, ap_check_set* restored_copy) {
    if (!identity || !selected || !restored || !aligned(identity) || !aligned(selected) || !aligned(restored)
        || current_file != 0 || ap_gameplay_mode != 0) return 0;
    {
        unsigned char* out = (unsigned char*)identity_copy;
        const unsigned char* in = (const unsigned char*)identity;
        for (unsigned int i = 0; i < sizeof(*identity_copy); ++i) out[i] = in[i];
    }
    copy_set(selected_copy, selected);
    copy_set(restored_copy, restored);
    return current_identity(identity_copy) && ap_check_valid(selected_copy) && ap_check_distinct_flags(selected_copy)
        && subset(restored_copy, selected_copy);
}

int ap_campaign_checks_validate(const ap_campaign_save* identity, const ap_check_set* selected, const ap_check_set* restored) {
    ap_campaign_save identity_copy; ap_check_set selected_copy, restored_copy;
    return validate(identity, selected, restored, &identity_copy, &selected_copy, &restored_copy);
}

int ap_campaign_checks_prepare(const ap_campaign_save* identity, const ap_check_set* selected, const ap_check_set* restored) {
    ap_campaign_save identity_copy; ap_check_set selected_copy, restored_copy;
    if (!validate(identity, selected, restored, &identity_copy, &selected_copy, &restored_copy)) return 0;
    campaign_identity = identity_copy;
    campaign_selected = selected_copy;
    campaign_observed = restored_copy;
    campaign_ready = 1;
    return 1;
}

int ap_campaign_checks_restore(const ap_check_set* restored) {
    ap_check_set restored_copy;
    if (!current_campaign() || !restored || !aligned(restored)) return 0;
    copy_set(&restored_copy, restored);
    if (!subset(&restored_copy, &campaign_selected)) return 0;
    for (unsigned int i = 0; i < AP_LOCATION_WORDS; ++i)
        campaign_observed.words[i] |= restored_copy.words[i];
    return 1;
}

int ap_campaign_checks_event(unsigned int location) {
    if (!current_campaign() || !(ap_gameplay_mode & AP_GAMEPLAY_CHECKS)
        || !ap_check_has(&campaign_selected, location)) return -1;
    if (ap_check_has(&campaign_observed, location)) return 0;
    ap_check_add(&campaign_observed, location);
    return 1;
}

int ap_campaign_checks_stock_flag(unsigned short flag) {
    ap_check_set previous;
    if (!current_campaign() || !(ap_gameplay_mode & AP_GAMEPLAY_CHECKS)) return -1;
    previous = campaign_observed;
    ap_check_stock_flag(&campaign_observed, &campaign_selected, flag);
    for (unsigned int i = 0; i < AP_LOCATION_WORDS; ++i)
        if (previous.words[i] != campaign_observed.words[i]) return 1;
    return 0;
}

int ap_campaign_checks_snapshot(ap_check_set* output) {
    if (!output || !aligned(output) || !current_campaign()) return 0;
    *output = campaign_observed;
    return 1;
}

int ap_campaign_checks_reset(void) {
    if (ap_gameplay_mode != 0) return 0;
    clear_set(&campaign_selected);
    clear_set(&campaign_observed);
    {
        unsigned char* bytes = (unsigned char*)&campaign_identity;
        for (unsigned int i = 0; i < sizeof(campaign_identity); ++i) bytes[i] = 0;
    }
    campaign_ready = 0;
    return 1;
}
