// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "campaign.hpp"
#include "../mod/ap_shops.h"
namespace dkap {
struct SeedRequirement { Requirement kind; uint32_t count; };
struct CampaignSeed {
    std::string seed_name, player;
    uint32_t team, slot;
    std::array<uint8_t, 16> binding{};
    std::vector<int64_t> locations;
    std::vector<Item> starting_inventory;
    std::array<SeedRequirement, 8> blockers;
    std::array<uint16_t, 7> boss_bananas;
    std::vector<ap_shop_offer> shops;
    Json edits;
    std::set<std::string> required_hooks;
    static CampaignSeed parse(const Json&);
    std::string binding_hex() const;
    void require_hooks(const std::set<std::string>& implemented_hooks) const;
    bool blocker_open(unsigned level, const CampaignInventory&) const;
    bool boss_door_open(unsigned level, uint32_t collected_bananas) const;
};
}
