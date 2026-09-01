// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_protocol.hpp"

namespace dkap {
class CampaignSession {
    struct Impl;
    mutable std::mutex lifetime_mutex;
    std::shared_ptr<Impl> impl;
public:
    CampaignSession();
    ~CampaignSession();
    void start(const Config&, const CampaignSeed&, CampaignChannel&);
    void stop();
    uint32_t tick(const ap_check_set&);
};
}
