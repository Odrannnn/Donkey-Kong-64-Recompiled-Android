// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_channel.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>

namespace dkap {
struct CampaignChannel::Impl {
    enum class UpdateKind { none, receive, record };
    std::thread worker;
    std::atomic<bool> quit{false};
    std::atomic<CampaignChannelResult> status{CampaignChannelResult::retry};
    mutable std::mutex mutex;
    std::condition_variable wake;
    ap_campaign_save requested{};
    uint64_t request_generation = 0;
    uint64_t completed_generation = 0;
    bool request_pending = false;
    CampaignChannelResult completed_result = CampaignChannelResult::retry;
    CampaignStageSnapshot cached;
    UpdateKind update_kind = UpdateKind::none;
    size_t update_index = 0;
    std::vector<Item> update_items;
    ap_check_set update_checks{};
    uint64_t update_generation = 0;
    uint64_t completed_update_generation = 0;
    bool update_pending = false;
    CampaignChannelResult completed_update_result = CampaignChannelResult::retry;
    CampaignDurableSnapshot durable;
    ~Impl() { stop(); }
    void stop() {
        quit = true; status = CampaignChannelResult::unavailable; wake.notify_all();
        if (worker.joinable()) worker.join();
    }
};

CampaignChannel::CampaignChannel() = default;
CampaignChannel::~CampaignChannel() { stop(); }
void CampaignChannel::start(const CampaignSeed& seed, const std::filesystem::path& path) {
    std::lock_guard lifetime_lock(lifetime_mutex);
    if (impl) { impl->stop(); impl.reset(); }
    auto next = std::make_shared<Impl>();
    next->worker = std::thread([ptr = next, seed, path] {
        try {
            CampaignJournalState state(seed, path);
            if (ptr->quit) return;
            {
                std::lock_guard lock(ptr->mutex);
                ptr->durable.receipt_count = state.receipts().size();
                ptr->durable.observed = state.checks();
            }
            ptr->status = CampaignChannelResult::ready;
            std::unique_lock lock(ptr->mutex);
            for (;;) {
                ptr->wake.wait(lock, [&] { return ptr->quit.load() || ptr->update_pending || ptr->request_pending; });
                if (ptr->quit) return;
                if (ptr->update_pending) {
                    auto kind = ptr->update_kind;
                    auto index = ptr->update_index;
                    auto items = ptr->update_items;
                    auto checks = ptr->update_checks;
                    auto generation = ptr->update_generation;
                    ptr->update_pending = false;
                    lock.unlock();
                    CampaignChannelResult result = CampaignChannelResult::rejected;
                    CampaignDurableSnapshot durable;
                    try {
                        if (kind == Impl::UpdateKind::receive) (void)state.receive(index, items);
                        else if (kind == Impl::UpdateKind::record) (void)state.record(checks);
                        else throw Failure(Error::protocol);
                        durable.receipt_count = state.receipts().size();
                        durable.observed = state.checks();
                        result = CampaignChannelResult::ready;
                    } catch (...) { result = CampaignChannelResult::rejected; }
                    lock.lock();
                    if (generation == ptr->update_generation) {
                        ptr->completed_update_result = result;
                        ptr->completed_update_generation = generation;
                        if (result == CampaignChannelResult::ready && ptr->request_generation) {
                            ptr->durable = durable;
                            ptr->completed_generation = 0;
                            ptr->request_pending = true;
                        } else if (result == CampaignChannelResult::ready) ptr->durable = durable;
                    }
                    continue;
                }
                auto identity = ptr->requested;
                auto generation = ptr->request_generation;
                ptr->request_pending = false;
                lock.unlock();
                CampaignStageSnapshot snapshot;
                CampaignChannelResult result = CampaignChannelResult::rejected;
                try {
                    auto staged = state.stage(identity);
                    if (staged) { snapshot = std::move(*staged); result = CampaignChannelResult::ready; }
                    else result = CampaignChannelResult::unbound;
                } catch (...) { result = CampaignChannelResult::rejected; }
                lock.lock();
                if (generation == ptr->request_generation) {
                    ptr->cached = std::move(snapshot);
                    ptr->completed_result = result;
                    ptr->completed_generation = generation;
                }
            }
        } catch (...) { ptr->status = CampaignChannelResult::unavailable; }
    });
    impl = std::move(next);
}
void CampaignChannel::stop() {
    std::lock_guard lifetime_lock(lifetime_mutex);
    if (impl) { impl->stop(); impl.reset(); }
}
CampaignChannelResult CampaignChannel::try_receive(size_t index, const std::vector<Item>& items) {
    if (index > item_limit || items.size() > item_limit - index) return CampaignChannelResult::rejected;
    auto copied_items = items;
    std::shared_ptr<Impl> current;
    {
        std::unique_lock lifetime_lock(lifetime_mutex, std::try_to_lock);
        if (!lifetime_lock.owns_lock()) return CampaignChannelResult::retry;
        current = impl;
    }
    if (!current) return CampaignChannelResult::unavailable;
    auto status = current->status.load();
    if (status != CampaignChannelResult::ready) return status;
    std::unique_lock lock(current->mutex, std::try_to_lock);
    if (!lock.owns_lock()) return CampaignChannelResult::retry;
    bool same = current->update_kind == Impl::UpdateKind::receive && current->update_index == index
        && current->update_items == items;
    if (current->update_generation && current->completed_update_generation != current->update_generation)
        return CampaignChannelResult::retry;
    if (!current->update_generation || !same) {
        current->update_kind = Impl::UpdateKind::receive;
        current->update_index = index;
        current->update_items = std::move(copied_items);
        current->update_checks = {};
        ++current->update_generation;
        current->update_pending = true;
        current->completed_update_result = CampaignChannelResult::retry;
        current->wake.notify_one();
        return CampaignChannelResult::retry;
    }
    return current->completed_update_result;
}
CampaignChannelResult CampaignChannel::try_record(const ap_check_set& checks) {
    std::shared_ptr<Impl> current;
    {
        std::unique_lock lifetime_lock(lifetime_mutex, std::try_to_lock);
        if (!lifetime_lock.owns_lock()) return CampaignChannelResult::retry;
        current = impl;
    }
    if (!current) return CampaignChannelResult::unavailable;
    auto status = current->status.load();
    if (status != CampaignChannelResult::ready) return status;
    std::unique_lock lock(current->mutex, std::try_to_lock);
    if (!lock.owns_lock()) return CampaignChannelResult::retry;
    bool same = current->update_kind == Impl::UpdateKind::record
        && !std::memcmp(&current->update_checks, &checks, sizeof(checks));
    if (current->update_generation && current->completed_update_generation != current->update_generation)
        return CampaignChannelResult::retry;
    if (!current->update_generation || !same) {
        current->update_kind = Impl::UpdateKind::record;
        current->update_index = 0;
        current->update_items.clear();
        current->update_checks = checks;
        ++current->update_generation;
        current->update_pending = true;
        current->completed_update_result = CampaignChannelResult::retry;
        current->wake.notify_one();
        return CampaignChannelResult::retry;
    }
    return current->completed_update_result;
}
CampaignChannelResult CampaignChannel::try_snapshot(CampaignDurableSnapshot& output) const {
    std::shared_ptr<Impl> current;
    {
        std::unique_lock lifetime_lock(lifetime_mutex, std::try_to_lock);
        if (!lifetime_lock.owns_lock()) return CampaignChannelResult::retry;
        current = impl;
    }
    if (!current) return CampaignChannelResult::unavailable;
    auto status = current->status.load();
    if (status != CampaignChannelResult::ready) return status;
    std::unique_lock lock(current->mutex, std::try_to_lock);
    if (!lock.owns_lock()) return CampaignChannelResult::retry;
    output = current->durable;
    return CampaignChannelResult::ready;
}
CampaignChannelResult CampaignChannel::try_stage(const ap_campaign_save& identity, CampaignStageSnapshot& output) const {
    std::shared_ptr<Impl> current;
    {
        std::unique_lock lifetime_lock(lifetime_mutex, std::try_to_lock);
        if (!lifetime_lock.owns_lock()) return CampaignChannelResult::retry;
        current = impl;
    }
    if (!current) return CampaignChannelResult::unavailable;
    auto status = current->status.load();
    if (status != CampaignChannelResult::ready) return status;
    std::unique_lock lock(current->mutex, std::try_to_lock);
    if (!lock.owns_lock()) return CampaignChannelResult::retry;
    if (!current->request_generation || std::memcmp(&identity, &current->requested, sizeof(identity))) {
        current->requested = identity;
        ++current->request_generation;
        current->request_pending = true;
        current->completed_result = CampaignChannelResult::retry;
        current->wake.notify_one();
        return CampaignChannelResult::retry;
    }
    if (current->completed_generation != current->request_generation) return CampaignChannelResult::retry;
    if (current->completed_result == CampaignChannelResult::ready) output = current->cached;
    return current->completed_result;
}
void CampaignChannel::commit_receive(size_t index, const std::vector<Item>& items) {
    for (;;) {
        auto result = try_receive(index, items);
        if (result == CampaignChannelResult::ready) return;
        if (result == CampaignChannelResult::rejected) throw Failure(Error::protocol);
        if (result == CampaignChannelResult::unavailable || result == CampaignChannelResult::unbound)
            throw Failure(Error::storage);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
void CampaignChannel::commit_record(const ap_check_set& checks) {
    for (;;) {
        auto result = try_record(checks);
        if (result == CampaignChannelResult::ready) return;
        if (result == CampaignChannelResult::rejected) throw Failure(Error::protocol);
        if (result == CampaignChannelResult::unavailable || result == CampaignChannelResult::unbound)
            throw Failure(Error::storage);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
CampaignDurableSnapshot CampaignChannel::snapshot() const {
    CampaignDurableSnapshot output;
    for (;;) {
        auto result = try_snapshot(output);
        if (result == CampaignChannelResult::ready) return output;
        if (result == CampaignChannelResult::rejected) throw Failure(Error::protocol);
        if (result == CampaignChannelResult::unavailable || result == CampaignChannelResult::unbound)
            throw Failure(Error::storage);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
}
