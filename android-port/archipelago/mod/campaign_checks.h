// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "checks.h"
#include "campaign_save.h"

// Seed-bound campaign check ledger. Preparation replaces the complete selected
// set only while all gameplay capabilities are disabled. Runtime observations
// are accepted only after the dedicated capability is enabled.
int ap_campaign_checks_prepare(const ap_campaign_save* identity, const ap_check_set* selected, const ap_check_set* restored);
int ap_campaign_checks_validate(const ap_campaign_save* identity, const ap_check_set* selected, const ap_check_set* restored);
int ap_campaign_checks_restore(const ap_check_set* restored);
int ap_campaign_checks_event(unsigned int location);
int ap_campaign_checks_stock_flag(unsigned short flag);
int ap_campaign_checks_snapshot(ap_check_set* output);
int ap_campaign_checks_reset(void);
