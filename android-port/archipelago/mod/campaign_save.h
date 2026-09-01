// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_receipts.h"
#include "gameplay_mode.h"

#define AP_CAMPAIGN_TRAILER_OFFSET (0x800u - (unsigned)sizeof(ap_campaign_save))

// Internal game-thread functions. A zero trailer is unbound, 1 is valid and
// -1 is corrupt. Staging does not queue a save or enable campaign gameplay.
int ap_campaign_trailer_read(ap_campaign_save* output);
int ap_campaign_trailer_stage(const ap_campaign_save* candidate);
