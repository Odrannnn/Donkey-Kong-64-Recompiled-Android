// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "../generated/catalog.h"

#if defined(__cplusplus)
extern "C" {
#endif
// The directly reproducible receipt traps: Disable A/B/Z/C-Up, Dry and Ice Floor.
int ap_campaign_trap_executable(unsigned type);
int ap_campaign_trap_can_start(unsigned type);
int ap_campaign_trap_start(unsigned type);
void ap_campaign_trap_tick(void);
unsigned ap_campaign_trap_button_mask(void);
void ap_campaign_trap_reset(void);
#if defined(__cplusplus)
}
#endif
