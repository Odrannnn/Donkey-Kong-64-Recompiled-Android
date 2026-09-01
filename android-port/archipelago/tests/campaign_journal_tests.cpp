// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_channel.hpp"
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (0)
using namespace dkap;

template<class F> void rejects(F action, Error expected) {
    try { action(); } catch (const Failure& failure) { CHECK(failure.code == expected); return; }
    throw std::runtime_error("Expected rejection");
}

CampaignSeed test_seed() {
    Json seed{{"format", 1}, {"world_version", "1.5.8"},
        {"revision", "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4"}, {"game", "Donkey Kong 64"},
        {"seed_name", "journal-seed"}, {"player", "DK64Test"}, {"team", 0}, {"slot", 1},
        {"locations", Json::array({14041180, 14041181})}, {"starting_inventory", Json::array()},
        {"boss_bananas", Json::array({60,120,200,250,300,350,400})}, {"edits", {{"entrances", Json::array()}}}};
    seed["blockers"] = Json::array();
    for (int i = 0; i < 8; ++i) seed["blockers"].push_back({{"item", "keys"}, {"count", i + 1}});
    return CampaignSeed::parse(seed);
}

int main() {
    auto root = std::filesystem::temp_directory_path()
        / ("dkap-campaign-journal-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(root);
    auto path = root / "campaign.json";
    try {
        auto seed = test_seed();
        std::vector<Item> delivered{{14041104, 14041180, 1, 1}, {14041108, 14041181, 2, 1}};
        {
            CampaignJournalState state(seed, path);
            CHECK(state.receipts().empty() && !ap_check_has(&state.checks(), 14041180));
            rejects([&] { CampaignJournalState locked(seed, path); }, Error::storage);

            ap_check_set first{}; ap_check_add(&first, 14041180);
            CHECK(state.record(first));
            CHECK(!state.record(first));
            ap_check_set stale{};
            CHECK(!state.record(stale) && ap_check_has(&state.checks(), 14041180));
            ap_check_set invalid{}; invalid.words[AP_LOCATION_WORDS - 1] = 0x80000000u;
            rejects([&] { state.record(invalid); }, Error::protocol);
            ap_check_set unselected{}; ap_check_add(&unselected, 14041182);
            rejects([&] { state.record(unselected); }, Error::protocol);
            CHECK(ap_check_has(&state.checks(), 14041180) && !ap_check_has(&state.checks(), 14041181));

            CHECK(state.receive(0, delivered));
            CHECK(!state.receive(0, delivered));
            CHECK(!state.receive(1, {delivered[1]}));
            rejects([&] { state.receive(3, {}); }, Error::protocol);
            rejects([&] { state.receive(1, {Item{14041112, 0, 1, 0}}); }, Error::protocol);
            rejects([&] { state.receive(0, {delivered[0]}); }, Error::protocol);
            rejects([&] { state.receive(2, {Item{999, 0, 1, 0}}); }, Error::unsupported);
            rejects([&] { state.receive(2, {Item{14041112, 0, 1, 8}}); }, Error::protocol);
            CHECK(state.receipts() == delivered);

            ap_campaign_save blank{};
            CHECK(!state.stage(blank));
            CampaignReceipts transaction(seed, delivered);
            auto identity = transaction.initialize();
            auto staged = state.stage(identity);
            CHECK(staged && staged->item_ids.empty() && ap_check_has(&staged->selected, 14041180)
                && ap_check_has(&staged->observed, 14041180) && staged->next_item == 14041104);
            ++identity.counter; ap_campaign_save_seal(&identity);
            staged = state.stage(identity);
            CHECK(staged && staged->item_ids == std::vector<uint32_t>{14041104} && staged->next_item == 14041108);
            ++identity.counter; ap_campaign_save_seal(&identity);
            staged = state.stage(identity);
            CHECK(staged && staged->item_ids == std::vector<uint32_t>({14041104, 14041108}) && !staged->next_item);
            auto wrong = identity; wrong.binding[0] ^= 1; ap_campaign_save_seal(&wrong);
            rejects([&] { (void)state.stage(wrong); }, Error::seed);
            auto ahead = identity; ++ahead.counter; ap_campaign_save_seal(&ahead);
            rejects([&] { (void)state.stage(ahead); }, Error::storage);
            auto corrupt = identity; corrupt.checksum ^= 1;
            rejects([&] { (void)state.stage(corrupt); }, Error::storage);

            // Upstream advances known legacy/event receipts, but the game-side
            // reducer cannot stage them until an explicit effect is implemented.
            CHECK(state.receive(2, {Item{14041090, 0, 1, 0}}));
            auto unsupported = identity; ++unsupported.counter; ap_campaign_save_seal(&unsupported);
            rejects([&] { (void)state.stage(unsupported); }, Error::unsupported);
        }
        {
            CampaignJournalState restored(seed, path);
            CHECK(restored.receipts().size() == 3 && ap_check_has(&restored.checks(), 14041180));
            auto changed = seed; changed.binding[0] ^= 1;
            rejects([&] { CampaignJournalState wrong(changed, path); }, Error::storage);
        }
        {
            auto failure_path = root / "commit-failure.json";
            CampaignJournalState failure(seed, failure_path);
            auto temporary = failure_path; temporary += ".tmp";
            std::filesystem::create_directory(temporary); // Atomic writer cannot replace a directory.
            ap_check_set first{}; ap_check_add(&first, 14041180);
            rejects([&] { failure.record(first); }, Error::storage);
            CHECK(!ap_check_has(&failure.checks(), 14041180));
            rejects([&] { failure.receive(0, {delivered[0]}); }, Error::storage);
            CHECK(failure.receipts().empty());
            std::filesystem::remove_all(temporary);
        }
        {
            auto channel_path = root / "channel.json";
            CampaignChannel channel;
            channel.start(seed, channel_path);
            auto identity = CampaignReceipts(seed, {}).initialize();
            CampaignStageSnapshot snapshot;
            auto poll = [&](auto action, CampaignChannelResult expected) {
                for (unsigned i = 0; i < 200; ++i) {
                    auto result = action();
                    if (result == expected) return true;
                    if (result != CampaignChannelResult::retry) return false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
                return false;
            };
            bool ready = poll([&] { return channel.try_stage(identity, snapshot); }, CampaignChannelResult::ready);
            CHECK(ready && snapshot.item_ids.empty() && !snapshot.next_item);
            CampaignDurableSnapshot durable;
            CHECK(poll([&] { return channel.try_snapshot(durable); }, CampaignChannelResult::ready)
                && durable.receipt_count == 0 && !ap_check_has(&durable.observed, 14041180));
            CHECK(channel.try_receive(item_limit, {delivered[0], delivered[1]}) == CampaignChannelResult::rejected);
            CHECK(poll([&] { return channel.try_receive(0, {delivered[0]}); }, CampaignChannelResult::ready));
            // The journal may be ahead, but staging remains bound to the counter committed by the game save.
            CHECK(poll([&] { return channel.try_stage(identity, snapshot); }, CampaignChannelResult::ready)
                && snapshot.item_ids.empty() && snapshot.next_item == 14041104);
            ++identity.counter; ap_campaign_save_seal(&identity);
            CHECK(poll([&] { return channel.try_stage(identity, snapshot); }, CampaignChannelResult::ready)
                && snapshot.item_ids == std::vector<uint32_t>{14041104} && !snapshot.next_item);
            ap_check_set first{}; ap_check_add(&first, 14041180);
            CHECK(poll([&] { return channel.try_record(first); }, CampaignChannelResult::ready));
            CHECK(channel.try_snapshot(durable) == CampaignChannelResult::ready
                && durable.receipt_count == 1 && ap_check_has(&durable.observed, 14041180));
            // A durable update invalidates and recomputes an otherwise identical staged identity.
            CHECK(poll([&] { return channel.try_stage(identity, snapshot); }, CampaignChannelResult::ready)
                && ap_check_has(&snapshot.observed, 14041180));
            ap_check_set unselected{}; ap_check_add(&unselected, 14041182);
            CHECK(poll([&] { return channel.try_record(unselected); }, CampaignChannelResult::rejected));
            CHECK(poll([&] { return channel.try_receive(0, {delivered[1]}); }, CampaignChannelResult::rejected));
            CHECK(channel.try_snapshot(durable) == CampaignChannelResult::ready
                && durable.receipt_count == 1 && ap_check_has(&durable.observed, 14041180));
            std::atomic<bool> keep_polling{true};
            std::thread caller([&] {
                CampaignStageSnapshot current;
                while (keep_polling.load()) (void)channel.try_stage(identity, current);
            });
            channel.stop(); // An in-flight poll retains the implementation until it returns.
            keep_polling = false; caller.join();
            CHECK(channel.try_stage(identity, snapshot) == CampaignChannelResult::unavailable);
            CampaignJournalState restored(seed, channel_path);
            CHECK(restored.receipts() == std::vector<Item>{delivered[0]}
                && ap_check_has(&restored.checks(), 14041180));
        }
        {
            CampaignChannel channel;
            channel.start(seed, root / "destructor-channel.json");
        } // Destruction stops and joins even while the journal may still be loading.
        { std::ofstream file(path); file << "{\"format\":1,\"binding\":\"wrong\",\"items\":[],\"checks\":[]}"; }
        rejects([&] { CampaignJournalState corrupt(test_seed(), path); }, Error::storage);
        std::filesystem::remove_all(root);
        std::cout << "PASS: durable campaign receipts/checks, atomic replay, seed binding and asynchronous staging\n";
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
