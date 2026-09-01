// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign_journal.hpp"
#include <mutex>

namespace dkap {
enum class CampaignChannelResult { ready, retry, unbound, rejected, unavailable };
struct CampaignDurableSnapshot {
    size_t receipt_count = 0;
    ap_check_set observed{};
};

// Loads and owns the campaign journal off the game thread. Snapshot requests
// use try_lock and are reduced by the worker before a later call can copy them.
class CampaignChannel {
    struct Impl;
    mutable std::mutex lifetime_mutex;
    std::shared_ptr<Impl> impl;
public:
    CampaignChannel();
    ~CampaignChannel();
    void start(const CampaignSeed&, const std::filesystem::path&);
    void stop();
    CampaignChannelResult try_receive(size_t index, const std::vector<Item>&);
    CampaignChannelResult try_record(const ap_check_set&);
    CampaignChannelResult try_snapshot(CampaignDurableSnapshot&) const;
    CampaignChannelResult try_stage(const ap_campaign_save&, CampaignStageSnapshot&) const;
    // Blocking adapters are for background protocol workers only.
    void commit_receive(size_t index, const std::vector<Item>&);
    void commit_record(const ap_check_set&);
    CampaignDurableSnapshot snapshot() const;
};
}
