// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "state.hpp"
#include "campaign_journal.hpp"
#include "../mod/shop_labels.h"

namespace dkap {
void validate_rdram_span(uint8_t* rdram, uint32_t address, size_t size);
void write_rdram_shop_label(uint8_t* rdram, uint32_t address, const ap_shop_label& label);
ap_campaign_save read_rdram_campaign_save(uint8_t* rdram, uint32_t address);
ap_check_set read_rdram_check_set(uint8_t* rdram, uint32_t address);
void write_rdram_campaign_save(uint8_t* rdram, uint32_t address, const ap_campaign_save& save);
void write_rdram_campaign_stage(uint8_t* rdram, uint32_t address, const CampaignStageSnapshot& snapshot);
}
