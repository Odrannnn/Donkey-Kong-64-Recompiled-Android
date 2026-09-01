// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "state.hpp"
#include "../mod/checks.h"
#include <map>
#include <set>

namespace dkap {
// Persistent inventory totals, separate from both location flags and spendable game counters.
// This model does not apply traps or refill consumables; those need a game-save receipt transaction.
struct CampaignInventory {
    std::array<uint8_t, 5> moves{}, guns{}, instruments{}, hints{}, blueprints{};
    uint8_t kongs = 0, keys = 0, abilities = 0, special = 0, slam = 0, belt = 0, instrument_upgrades = 0, shopkeepers = 0;
    bool climbing = false;
    uint32_t golden_bananas = 0, fairies = 0, crowns = 0, medals = 0, pearls = 0, rainbow_coins = 0, junk = 0;
    std::map<std::string, uint32_t> traps;
    std::set<uint16_t> flags;
    std::vector<int64_t> unsupported;
};
class CampaignCatalog {
    std::map<int64_t, Json> items_;
public:
    static const CampaignCatalog& builtin();
    explicit CampaignCatalog(const Json& source);
    const Json& item(int64_t id) const;
    CampaignInventory inventory(const std::vector<Item>& receipts) const;
};
class CampaignChecks {
    ap_check_set active_{}, observed_{}, confirmed_{};
public:
    explicit CampaignChecks(const std::vector<int64_t>& active);
    bool observe_stock_flag(uint16_t flag);
    bool observe_event(int64_t location);
    void confirm(const std::vector<int64_t>& locations);
    std::vector<int64_t> pending() const;
    std::vector<int64_t> observed() const;
};
// Seed-driven progression predicates shared with host tests and future game hooks.
enum class Requirement { golden_bananas, blueprints, keys, fairies, crowns, medals, pearls, rainbow_coins, bean,
    nintendo_coin, rareware_coin, kongs, moves };
uint32_t inventory_count(const CampaignInventory&, Requirement);
bool meets(const CampaignInventory&, Requirement, uint32_t amount);
bool owns_move(const CampaignInventory&, uint32_t kong, uint32_t level);
}
