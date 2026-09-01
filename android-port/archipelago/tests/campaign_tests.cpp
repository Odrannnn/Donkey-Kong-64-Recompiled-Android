// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign.hpp"
#include "seed.hpp"
#include "receipts.hpp"
#include "presentation.hpp"
#include "../mod/campaign_inventory.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (0)
using namespace dkap;
template<class F> void rejected(F f) { try { f(); } catch (const Failure&) { return; } throw std::runtime_error("Expected rejection"); }
int main() {
    try {
        std::ifstream file(AP_CATALOG_PATH); Json source; file >> source;
        CampaignCatalog catalog(source);
        CHECK(source["checks"].size() == AP_LOCATION_COUNT && AP_LOCATION_COUNT == 868 && AP_ITEM_COUNT == 184);
        for (unsigned i = 0; i < AP_LOCATION_COUNT; ++i) {
            const auto& def = ap_locations[i];
            CampaignChecks checks({int64_t(def.id)});
            CHECK(checks.observe_event(def.id)); CHECK(!checks.observe_event(def.id));
            CHECK(checks.pending() == std::vector<int64_t>{def.id});
            checks.confirm({def.id}); CHECK(checks.pending().empty());
            CampaignChecks stock({int64_t(def.id)});
            CHECK(stock.observe_stock_flag(def.flag) == (def.detector == AP_DETECT_STOCK_FLAG));
            CHECK(ap_location_index(def.id) == int(i));
        }
        for (const auto& alias : source["flag_aliases"]) {
            auto ids = alias.get<std::vector<int64_t>>();
            rejected([&] { CampaignChecks invalid(ids); });
        }
        auto compare_inventory = [&](const CampaignInventory& expected, const ap_campaign_inventory& actual) {
            CHECK(std::equal(expected.moves.begin(), expected.moves.end(), actual.moves));
            CHECK(std::equal(expected.guns.begin(), expected.guns.end(), actual.guns));
            CHECK(std::equal(expected.instruments.begin(), expected.instruments.end(), actual.instruments));
            CHECK(std::equal(expected.hints.begin(), expected.hints.end(), actual.hints));
            CHECK(std::equal(expected.blueprints.begin(), expected.blueprints.end(), actual.blueprints));
            CHECK(expected.kongs == actual.kongs && expected.keys == actual.keys && expected.abilities == actual.abilities);
            CHECK(expected.special == actual.special && expected.slam == actual.slam && expected.belt == actual.belt);
            CHECK(expected.instrument_upgrades == actual.instrument_upgrades && expected.shopkeepers == actual.shopkeepers);
            CHECK(expected.climbing == !!actual.climbing && expected.golden_bananas == actual.golden_bananas);
            CHECK(expected.fairies == actual.fairies && expected.crowns == actual.crowns && expected.medals == actual.medals);
            CHECK(expected.pearls == actual.pearls && expected.rainbow_coins == actual.rainbow_coins && expected.junk == actual.junk);
            for (unsigned i = 0; i < AP_TRAP_TYPE_COUNT; ++i) {
                auto found = expected.traps.find(ap_trap_type_names[i]);
                CHECK(actual.traps[i] == (found == expected.traps.end() ? 0u : found->second));
            }
            for (unsigned flag = 0; flag < AP_ITEM_FLAG_WORDS * 32u; ++flag)
                CHECK(ap_campaign_inventory_has_flag(&actual, flag) == expected.flags.contains(uint16_t(flag)));
        };
        ap_campaign_inventory compact{};
        std::vector<Item> supported_receipts;
        std::vector<ap_uint32_t> supported_ids;
        for (unsigned i = 0; i < AP_ITEM_COUNT; ++i) {
            CHECK(ap_item_effects[i].id == ap_item_ids[i]);
            auto one = catalog.inventory({Item{ap_item_ids[i], int64_t(i), 1, 1}});
            ap_campaign_inventory before = compact;
            int result = ap_campaign_inventory_add(&compact, ap_item_ids[i]);
            if (!one.unsupported.empty()) {
                CHECK(result == 0 && std::memcmp(&before, &compact, sizeof(compact)) == 0);
            } else {
                CHECK(result == 1);
                supported_receipts.push_back(Item{ap_item_ids[i], int64_t(i), 1, 1});
                supported_ids.push_back(ap_item_ids[i]);
                compare_inventory(catalog.inventory(supported_receipts), compact);
            }
        }
        ap_campaign_inventory unknown_before = compact;
        CHECK(ap_campaign_inventory_add(&compact, 0xFFFFFFFFu) == -1);
        CHECK(std::memcmp(&unknown_before, &compact, sizeof(compact)) == 0);
        ap_campaign_inventory rebuilt;
        std::memset(&rebuilt, 0xA5, sizeof(rebuilt));
        CHECK(ap_campaign_inventory_rebuild(&rebuilt, supported_ids.data(), unsigned(supported_ids.size())) == 1);
        compare_inventory(catalog.inventory(supported_receipts), rebuilt);
        auto rejected_rebuild = rebuilt;
        auto invalid_prefix = supported_ids; invalid_prefix.push_back(14041090u);
        CHECK(ap_campaign_inventory_rebuild(&rebuilt, invalid_prefix.data(), unsigned(invalid_prefix.size())) == 0);
        CHECK(std::memcmp(&rejected_rebuild, &rebuilt, sizeof(rebuilt)) == 0);
        CHECK(ap_campaign_inventory_rebuild(&rebuilt, nullptr, 1) == -1);
        CHECK(ap_campaign_inventory_rebuild(nullptr, nullptr, 0) == -1);
        std::vector<Item> repeated;
        ap_campaign_inventory repeated_compact{};
        for (unsigned i = 0; i < 10; ++i) {
            for (auto id : {14041101u, 14041130u, 14041137u, 14041358u}) {
                CHECK(ap_campaign_inventory_add(&repeated_compact, id) == 1);
                repeated.push_back(Item{int64_t(id), int64_t(i), 1, 1});
            }
        }
        compare_inventory(catalog.inventory(repeated), repeated_compact);
        CHECK(AP_SHOP_COUNT == 120);
        unsigned shared_count = 0, shop_conflicts = 0;
        for (const auto& shared : ap_shop_locations) {
            CHECK(ap_locations[ap_location_index(shared.id)].detector == AP_DETECT_SHOP);
            if (shared.kong != AP_SHOP_SHARED) continue;
            ++shared_count;
            for (const auto& other : ap_shop_locations) {
                if (other.kong < 5 && other.vendor == shared.vendor && other.level == shared.level) {
                    rejected([&] { CampaignChecks invalid({shared.id, other.id}); }); ++shop_conflicts;
                }
            }
        }
        CHECK(shared_count == 20 && shop_conflicts == 100);
        for (const auto& shop : ap_shop_locations) {
            auto label = make_shop_label(shop.id, "Baboon Blast! ~%", "Player One", 130);
            CHECK(ap_label_valid(&label) && label.location == shop.id && label.frames == 130);
            CHECK(std::string(label.item) == "BABOON BLAST" && std::string(label.subtitle) == "TO PLAYER ONE");
        }
        auto shop_id = ap_shop_locations[0].id;
        for (unsigned c = 0; c <= 255; ++c) {
            auto label = make_shop_label(shop_id, std::string(1, char(c)), std::string(1, char(c)));
            CHECK(ap_label_valid(&label));
        }
        auto long_label = make_shop_label(shop_id, std::string(512, 'a'), std::string(512, 'b'), 50);
        CHECK(std::string(long_label.item).size() == 32 && std::string(long_label.subtitle).size() == 32 && ap_label_valid(&long_label));
        auto unknown_label = make_shop_label(shop_id, "\xE2\x98\x83", "\n%~", 255);
        CHECK(std::string(unknown_label.item) == "UNKNOWN ITEM" && std::string(unknown_label.subtitle) == "TO UNKNOWN PLAYER");
        rejected([&] { make_shop_label(14041180, "A", "B"); });
        rejected([&] { make_shop_label(0xffffffffu, "A", "B"); });
        rejected([&] { make_shop_label(shop_id, "A", "B", 49); });
        rejected([&] { make_shop_label(shop_id, "A", "B", 256); });
        rejected([&] { make_shop_label(shop_id, std::string(513, 'a'), "B"); });
        rejected([&] { make_shop_label(shop_id, "A", std::string(513, 'b')); });
        rejected([&] { CampaignChecks invalid({999}); });
        CampaignChecks tracked({14041180, 14041181});
        CHECK(tracked.observe_stock_flag(4));
        rejected([&] { tracked.confirm({14041180, 999}); });
        CHECK(tracked.pending() == std::vector<int64_t>{14041180});
        rejected([&] { tracked.observe_event(999); });
        ap_check_set invalid{}; invalid.words[AP_LOCATION_WORDS-1] = 0x80000000u; CHECK(!ap_check_valid(&invalid));
        // Receiving only a third move must not grant the first or second one.
        auto inventory = catalog.inventory({Item{14041106, 4, 1, 1}});
        CHECK(!owns_move(inventory, 0, 1) && !owns_move(inventory, 0, 2) && owns_move(inventory, 0, 3));
        std::vector<Item> receipts;
        for (unsigned i = 0; i < AP_ITEM_COUNT; ++i) receipts.push_back(Item{ap_item_ids[i], int64_t(i), 1, 1});
        auto all = catalog.inventory(receipts);
        CHECK(all.kongs == 31 && all.keys == 255 && all.abilities == 252);
        CHECK(all.moves == (std::array<uint8_t,5>{7,7,7,7,7}));
        CHECK(all.slam == 3 && all.belt == 2 && all.instrument_upgrades == 3);
        CHECK(all.shopkeepers == 15 && all.climbing && all.special == 7);
        CHECK(all.traps.size() == 16 && all.golden_bananas == 1);
        CHECK(meets(all, Requirement::keys, 8) && !meets(all, Requirement::keys, 9));
        CHECK(inventory_count(all, Requirement::moves) == 42); // 15 special + 10 gun/instrument + 8 upgrades + 2 gun upgrades + 7 abilities.
        CHECK(CampaignCatalog::builtin().inventory(receipts).moves == all.moves);
        auto altered = source; altered["items"][3]["count_id"] = {{"field", "kong_bitfield"}, {"bit", 99}};
        rejected([&] { CampaignCatalog invalid(altered); });
        CHECK(catalog.inventory({Item{14041089, 0, 1, 0}}).flags.empty());
        CHECK(!catalog.inventory({Item{14041090, 0, 1, 0}}).unsupported.empty());
        rejected([&] { catalog.inventory({Item{999, 0, 1, 0}}); });
        Json seed{{"format", 1}, {"world_version", "1.5.8"}, {"revision", "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4"},
            {"game", "Donkey Kong 64"}, {"seed_name", "seed"}, {"player", "DK64Test"}, {"team", 0}, {"slot", 1},
            {"locations", Json::array({14041180,14041181})}, {"starting_inventory", Json::array({14041091})},
            {"boss_bananas", Json::array({60,120,200,250,300,350,400})}, {"edits", {{"entrances", Json::array()}}}};
        seed["blockers"] = Json::array();
        for (int i = 0; i < 8; ++i) seed["blockers"].push_back({{"item", "keys"}, {"count", i + 1}});
        auto manifest = CampaignSeed::parse(seed);
        CHECK(manifest.binding_hex() == "8aff15421fa2367292aef0ed61d71d5f"
            && CampaignSeed::parse(seed).binding == manifest.binding);
        auto changed_seed = seed; changed_seed["edits"]["entrances"] = Json::array({1});
        CHECK(CampaignSeed::parse(changed_seed).binding != manifest.binding);
        auto extra_seed_field = seed; extra_seed_field["ignored"] = true;
        rejected([&] { CampaignSeed::parse(extra_seed_field); });

        std::vector<Item> durable_receipts{
            Item{14041104, 14041180, 1, 1}, Item{14041108, 14041181, 2, 1}};
        CampaignReceipts transaction(manifest, durable_receipts);
        ap_campaign_save blank{};
        auto step = transaction.next(blank);
        CHECK(step.status == CampaignReceiptStatus::unbound && !step.item);
        auto save_header = transaction.initialize();
        CHECK(save_header.magic == AP_CAMPAIGN_SAVE_MAGIC && save_header.format == AP_CAMPAIGN_SAVE_FORMAT
            && save_header.counter == 0 && ap_campaign_save_valid(&save_header)
            && std::equal(manifest.binding.begin(), manifest.binding.end(), save_header.binding));
        step = transaction.next(save_header);
        CHECK(step.status == CampaignReceiptStatus::receipt && step.index == 0 && step.item == durable_receipts[0]);
        // A crash before the game-save commit exposes the same receipt again.
        CHECK(transaction.next(save_header).item == durable_receipts[0]);
        ++save_header.counter; ap_campaign_save_seal(&save_header); // Effect and counter are one committed game save.
        step = transaction.next(save_header);
        CHECK(step.status == CampaignReceiptStatus::receipt && step.index == 1 && step.item == durable_receipts[1]);
        ++save_header.counter; ap_campaign_save_seal(&save_header);
        step = transaction.next(save_header);
        CHECK(step.status == CampaignReceiptStatus::complete && step.index == 2 && !step.item);
        // Restoring an older game save restores its effects and counter together, so replay is required.
        save_header.counter = 0; ap_campaign_save_seal(&save_header);
        CHECK(transaction.next(save_header).item == durable_receipts[0]);
        save_header.counter = 3; ap_campaign_save_seal(&save_header);
        rejected([&] { transaction.next(save_header); });
        save_header = transaction.initialize(); save_header.magic ^= 1;
        rejected([&] { transaction.next(save_header); });
        save_header = transaction.initialize(); ++save_header.format; ap_campaign_save_seal(&save_header);
        rejected([&] { transaction.next(save_header); });
        ap_campaign_save partial{}; partial.magic = AP_CAMPAIGN_SAVE_MAGIC;
        rejected([&] { transaction.next(partial); });
        save_header = transaction.initialize(); save_header.binding[0] ^= 1; ap_campaign_save_seal(&save_header);
        try { transaction.next(save_header); throw std::runtime_error("Expected seed rejection"); }
        catch (const Failure& failure) { CHECK(failure.code == Error::seed); }
        rejected([&] { CampaignReceipts invalid(manifest, {Item{999, 0, 1, 0}}); });
        rejected([&] { CampaignReceipts invalid(manifest, {Item{14041104, 0, 1, 8}}); });
        // The original client advances its counter for known legacy/event items
        // even when their catalog entry has no game effect.
        CampaignReceipts no_effect(manifest, {Item{14041090, 0, 1, 0}});
        CHECK(no_effect.next(no_effect.initialize()).item == (Item{14041090, 0, 1, 0}));
        CampaignReceipts empty_transaction(manifest, {});
        CHECK(empty_transaction.next(empty_transaction.initialize()).status == CampaignReceiptStatus::complete);
        std::vector<Item> receipt_limit(item_limit, Item{14041104, 0, 1, 0});
        CampaignReceipts boundary(manifest, receipt_limit);
        auto boundary_save = boundary.initialize(); boundary_save.counter = uint16_t(item_limit - 1); ap_campaign_save_seal(&boundary_save);
        CHECK(boundary.next(boundary_save).status == CampaignReceiptStatus::receipt);
        ++boundary_save.counter; ap_campaign_save_seal(&boundary_save);
        CHECK(boundary.next(boundary_save).status == CampaignReceiptStatus::complete);
        for (const auto& shop : ap_shop_locations) {
            auto with_shop = seed;
            with_shop["locations"] = Json::array({shop.id});
            rejected([&] { CampaignSeed::parse(with_shop); });
            with_shop["shops"] = Json::array({Json{{"location", shop.id}, {"price", 17}}});
            auto parsed = CampaignSeed::parse(with_shop);
            CHECK(parsed.shops.size() == 1 && parsed.shops[0].location == shop.id && parsed.shops[0].price == 17);
            CHECK(parsed.required_hooks.contains("shops"));
            auto bad = with_shop; bad["shops"][0]["price"] = 256;
            rejected([&] { CampaignSeed::parse(bad); });
            bad = with_shop; bad["shops"][0]["price"] = true;
            rejected([&] { CampaignSeed::parse(bad); });
            bad = with_shop; bad["shops"][0]["price"] = 17.0;
            rejected([&] { CampaignSeed::parse(bad); });
            bad = with_shop; bad["shops"].push_back(bad["shops"][0]);
            rejected([&] { CampaignSeed::parse(bad); });
            bad = with_shop; bad["shops"][0]["location"] = 14041180;
            rejected([&] { CampaignSeed::parse(bad); });
            bad = with_shop; bad["shops"][0]["item"] = 14041104;
            rejected([&] { CampaignSeed::parse(bad); });
        }
        CHECK(manifest.edits == seed["edits"]);
        CHECK(manifest.blocker_open(7, all) && !manifest.blocker_open(8, all));
        CHECK(manifest.boss_door_open(6, 400) && !manifest.boss_door_open(6, 399));
        rejected([&] { manifest.require_hooks({}); });
        auto incomplete_hooks = manifest.required_hooks; incomplete_hooks.erase("edit:entrances");
        rejected([&] { manifest.require_hooks(incomplete_hooks); });
        manifest.require_hooks(manifest.required_hooks);
        auto malformed_seed = seed; malformed_seed["blockers"][0]["count"] = true;
        rejected([&] { CampaignSeed::parse(malformed_seed); });
        malformed_seed = seed; malformed_seed["starting_inventory"] = Json::array({14041090});
        rejected([&] { CampaignSeed::parse(malformed_seed); });
        malformed_seed = seed; malformed_seed["format"] = 1.0;
        rejected([&] { CampaignSeed::parse(malformed_seed); });
        malformed_seed = seed; malformed_seed["edits"]["oversized"] = std::string(message_limit, 'x');
        rejected([&] { CampaignSeed::parse(malformed_seed); });
        std::cout << "PASS: all 868 location detectors, shop aliases, atomic confirmation, 184 item definitions, independent moves and requirements\n";
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
