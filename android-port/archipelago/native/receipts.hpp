// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "seed.hpp"
#include "../mod/campaign_receipts.h"
#include <optional>

namespace dkap {
enum class CampaignReceiptStatus { unbound, receipt, complete };
struct CampaignReceiptStep {
    CampaignReceiptStatus status = CampaignReceiptStatus::unbound;
    uint16_t index = 0;
    std::optional<Item> item;
};

// Reconciles a durable AP ReceivedItems list with the counter stored in the
// randomized game save. It never owns an acknowledgement: the game save is the
// authority for which prefix was applied.
class CampaignReceipts {
    std::array<uint8_t, AP_CAMPAIGN_BINDING_SIZE> binding_{};
    std::vector<Item> receipts_;
public:
    CampaignReceipts(const CampaignSeed&, std::vector<Item> durable_receipts);
    ap_campaign_save initialize() const;
    CampaignReceiptStep next(const ap_campaign_save&) const;
    size_t size() const { return receipts_.size(); }
};
}
