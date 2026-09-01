// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "receipts.hpp"
#include "../mod/checks.h"

namespace dkap {
struct CampaignStageSnapshot {
    ap_campaign_save identity{};
    std::vector<uint32_t> item_ids;
    std::optional<uint32_t> next_item;
    ap_check_set selected{};
    ap_check_set observed{};
};

// Durable full-campaign state, kept separate from the live integration-test
// journal until every required gameplay capability is available.
class CampaignJournalState {
    CampaignSeed seed_;
    Journal journal_;
    std::vector<Item> items_;
    ap_check_set selected_{}, observed_{};
    void persist(const std::vector<Item>& items, const ap_check_set& observed);
public:
    CampaignJournalState(const CampaignSeed&, const std::filesystem::path&);
    bool receive(size_t index, const std::vector<Item>& incoming);
    bool record(const ap_check_set& observed);
    std::optional<CampaignStageSnapshot> stage(const ap_campaign_save&) const;
    const std::vector<Item>& receipts() const { return items_; }
    const ap_check_set& checks() const { return observed_; }
};
}
