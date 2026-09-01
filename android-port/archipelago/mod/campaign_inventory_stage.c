// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_inventory.h"
#include "campaign_save.h"

extern ap_uint8_t current_file;
extern volatile ap_uint32_t ap_gameplay_mode;

static ap_campaign_inventory published_inventory;
static ap_campaign_save published_identity;
static ap_uint8_t published_ready;

static void copy_bytes(void* destination, const void* source, unsigned size) {
    ap_uint8_t* out = (ap_uint8_t*)destination;
    const ap_uint8_t* in = (const ap_uint8_t*)source;
    for (unsigned i = 0; i < size; ++i) out[i] = in[i];
}

static int supported(const ap_campaign_save* save) {
    return save && save->magic == AP_CAMPAIGN_SAVE_MAGIC && save->format == AP_CAMPAIGN_SAVE_FORMAT
        && ap_campaign_save_valid(save);
}

static int same_identity(const ap_campaign_save* a, const ap_campaign_save* b) {
    if (a->magic != b->magic || a->format != b->format || a->counter != b->counter
        || a->reserved != b->reserved || a->checksum != b->checksum) return 0;
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i)
        if (a->binding[i] != b->binding[i]) return 0;
    return 1;
}

static int current_identity(const ap_campaign_save* identity) {
    ap_campaign_save trailer;
    return supported(identity) && ap_campaign_trailer_read(&trailer) == 1 && same_identity(identity, &trailer);
}

int ap_campaign_inventory_stage(const ap_campaign_save* source, const ap_uint32_t* item_ids, unsigned count) {
    ap_campaign_save identity;
    ap_campaign_inventory inventory;
    if (!source || current_file != 0 || ap_gameplay_mode != 0 || count > 65535u || (count && !item_ids)
        || (count && ((ap_uint32_t)item_ids & 3u))) return 0;
    copy_bytes(&identity, source, sizeof(identity));
    if (identity.counter != count || !current_identity(&identity)) return 0;
    if (ap_campaign_inventory_rebuild(&inventory, item_ids, count) != 1) return 0;
    published_inventory = inventory;
    published_identity = identity;
    published_ready = 1;
    return 1;
}

int ap_campaign_inventory_snapshot(ap_campaign_inventory* output, const ap_campaign_save* source) {
    ap_campaign_save identity;
    if (!output || ((ap_uint32_t)output & 3u) || !source || current_file != 0 || !published_ready) return 0;
    copy_bytes(&identity, source, sizeof(identity));
    if (!supported(&identity) || !same_identity(&identity, &published_identity) || !current_identity(&identity)) return 0;
    *output = published_inventory;
    return 1;
}

void ap_campaign_inventory_reset(void) {
    ap_campaign_inventory_clear(&published_inventory);
    copy_bytes(&published_identity, &published_inventory, sizeof(published_identity));
    published_ready = 0;
}
