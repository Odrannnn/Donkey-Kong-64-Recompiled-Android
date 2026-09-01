// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_channel.hpp"

namespace dkap {
// Validates the original DK64 AP room protocol and publishes only journaled
// receipts/checks. Transport and reconnect timing remain separate.
class CampaignProtocol {
    Config config_;
    CampaignSeed seed_;
    CampaignChannel& channel_;
    ap_check_set selected_{}, confirmed_{};
    bool room_ = false, connected_ = false, synced_ = false;
public:
    CampaignProtocol(const Config&, const CampaignSeed&, CampaignChannel&);
    void disconnect();
    void observe(const ap_check_set&);
    std::vector<Json> receive(const std::string&);
    std::vector<Json> flush() const;
    bool ready() const { return room_ && connected_ && synced_; }
};
}
