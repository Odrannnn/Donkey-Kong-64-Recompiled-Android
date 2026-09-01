// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign.hpp"
#include "campaign_catalog.hpp"
#include <mbedtls/sha256.h>
#include <algorithm>
#include <bit>

namespace dkap {
const CampaignCatalog& CampaignCatalog::builtin() {
    static const CampaignCatalog catalog(parse_bounded(campaign_catalog_json));
    return catalog;
}
CampaignCatalog::CampaignCatalog(const Json& source) {
    // Reject edited metadata before it can drive indexing, shifting or item effects.
    auto canonical = source.dump(-1, ' ', true);
    unsigned char hash[32];
    if (mbedtls_sha256(reinterpret_cast<const unsigned char*>(canonical.data()), canonical.size(), hash, 0)) throw Failure(Error::unsupported);
    std::string digest;
    for (auto byte : hash) { digest += "0123456789abcdef"[byte >> 4]; digest += "0123456789abcdef"[byte & 15]; }
    if (digest != AP_CATALOG_SHA256) throw Failure(Error::unsupported);
    if (source.at("format") != 1 || source.at("revision") != "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4"
        || source.at("items").size() != AP_ITEM_COUNT) throw Failure(Error::unsupported);
    for (size_t i = 0; i < AP_ITEM_COUNT; ++i) {
        const auto& item = source.at("items").at(i);
        if (item.at("id") != ap_item_ids[i]) throw Failure(Error::unsupported);
        items_.emplace(ap_item_ids[i], item);
    }
}
const Json& CampaignCatalog::item(int64_t id) const {
    auto found = items_.find(id);
    if (found == items_.end()) throw Failure(Error::unsupported);
    return found->second;
}
namespace {
void count_effect(CampaignInventory& inv, const Json& effect) {
    if (effect.is_array()) { for (const auto& e : effect) count_effect(inv, e); return; }
    auto field = effect.value("field", std::string());
    if (field == "kong_bitfield") inv.kongs |= uint8_t(1u << effect.at("bit").get<unsigned>());
    else if (field == "key_bitfield") inv.keys |= uint8_t(1u << effect.at("bit").get<unsigned>());
    else if (field == "flag_moves") {
        static const std::map<std::string, uint8_t> masks{{"diving", 128}, {"oranges", 64}, {"barrels", 32}, {"vines", 16}, {"camera", 8}, {"shockwave", 4}};
        inv.abilities |= masks.at(effect.at("bit"));
    } else if (field == "special_items") {
        static const std::map<std::string, uint8_t> masks{{"nintendo_coin", 1}, {"rareware_coin", 2}, {"bean", 4}};
        inv.special |= masks.at(effect.at("bit"));
    } else if (field == "hint_bitfield") inv.hints.at(effect.at("kong")) |= uint8_t(1u << effect.at("level").get<unsigned>());
    else if (field == "bp_count") {
        auto& amount = inv.blueprints.at(effect.at("kong")); amount = uint8_t(std::min(8u, unsigned(amount) + 1));
    } else if (field == "fairies") ++inv.fairies;
    else if (field == "crowns") ++inv.crowns;
    else if (field == "medals") ++inv.medals;
    else if (field == "pearls") ++inv.pearls;
    else if (field == "rainbow_coins") ++inv.rainbow_coins;
    else if (field == "junk_items") ++inv.junk;
    else if (field == "ice_traps") ++inv.traps[effect.at("ice_trap_type").get<std::string>()];
    else if (effect.contains("item")) {
        auto transfer = effect.at("item").get<unsigned>();
        if (transfer == 1) ++inv.golden_bananas;
        else if (transfer == 2 || transfer == 51) inv.slam = uint8_t(std::min(3, inv.slam + 1));
        else if (transfer >= 26 && transfer <= 40) inv.moves[(transfer - 26) / 3] |= uint8_t(1u << ((transfer - 26) % 3));
        else if (transfer >= 41 && transfer <= 45) inv.instruments[transfer - 41] |= 1;
        else if (transfer >= 46 && transfer <= 50) inv.guns[transfer - 46] |= 1;
        else if (transfer == 52 || transfer == 53) for (auto& gun : inv.guns) gun |= uint8_t(1u << (transfer - 51));
        else if (transfer == 54) inv.belt = uint8_t(std::min(2, inv.belt + 1));
        else if (transfer == 55) {
            inv.instrument_upgrades = uint8_t(std::min(3, inv.instrument_upgrades + 1));
            for (auto& instrument : inv.instruments) instrument |= uint8_t(((1u << (inv.instrument_upgrades + 1)) - 1) & ~1u);
        } else throw Failure(Error::unsupported);
    } else throw Failure(Error::unsupported);
}
}
CampaignInventory CampaignCatalog::inventory(const std::vector<Item>& receipts) const {
    if (receipts.size() > item_limit) throw Failure(Error::protocol);
    CampaignInventory inv;
    for (const auto& receipt : receipts) {
        const auto& definition = item(receipt[0]);
        // No Item's legacy flag 0 is not permission to set Japes' first progression gate.
        if (receipt[0] == 14041089) continue;
        if (!definition.at("flag_id").is_null()) {
            auto flag = definition.at("flag_id").get<uint16_t>();
            inv.flags.insert(flag);
            if (flag == 671) inv.climbing = true;
            if (flag >= 962 && flag <= 965) inv.shopkeepers |= uint8_t(1u << (flag - 962));
        } else if (!definition.at("count_id").is_null()) count_effect(inv, definition.at("count_id"));
        else inv.unsupported.push_back(receipt[0]); // Upstream client also has no effect for these legacy/event entries.
    }
    return inv;
}
CampaignChecks::CampaignChecks(const std::vector<int64_t>& active) {
    for (auto id : active) {
        if (id < 0 || id > UINT32_MAX || ap_check_has(&active_, uint32_t(id)) || !ap_check_add(&active_, uint32_t(id))) throw Failure(Error::unsupported);
    }
    if (!ap_check_distinct_flags(&active_)) throw Failure(Error::unsupported);
}
bool CampaignChecks::observe_stock_flag(uint16_t flag) {
    auto previous = observed_; ap_check_stock_flag(&observed_, &active_, flag);
    return !std::equal(std::begin(previous.words), std::end(previous.words), std::begin(observed_.words));
}
bool CampaignChecks::observe_event(int64_t location) {
    if (location < 0 || location > UINT32_MAX || !ap_check_has(&active_, uint32_t(location))) throw Failure(Error::unsupported);
    bool changed = !ap_check_has(&observed_, uint32_t(location));
    ap_check_event(&observed_, &active_, uint32_t(location)); return changed;
}
void CampaignChecks::confirm(const std::vector<int64_t>& locations) {
    // Validate the whole server update before changing any confirmation bits.
    for (auto id : locations) if (id < 0 || id > UINT32_MAX || !ap_check_has(&active_, uint32_t(id))) throw Failure(Error::unsupported);
    for (auto id : locations) ap_check_add(&confirmed_, uint32_t(id));
}
std::vector<int64_t> CampaignChecks::pending() const {
    std::vector<int64_t> result;
    for (unsigned i = 0; i < AP_LOCATION_COUNT; ++i)
        if (ap_check_has_index(&observed_, i) && !ap_check_has_index(&confirmed_, i)) result.push_back(ap_locations[i].id);
    return result;
}
std::vector<int64_t> CampaignChecks::observed() const {
    std::vector<int64_t> result;
    for (unsigned i = 0; i < AP_LOCATION_COUNT; ++i) if (ap_check_has_index(&observed_, i)) result.push_back(ap_locations[i].id);
    return result;
}
bool owns_move(const CampaignInventory& inv, uint32_t kong, uint32_t level) {
    return kong < 5 && level >= 1 && level <= 3 && (inv.moves[kong] & (1u << (level - 1)));
}
uint32_t inventory_count(const CampaignInventory& inv, Requirement requirement) {
    switch (requirement) {
        case Requirement::golden_bananas: return inv.golden_bananas;
        case Requirement::fairies: return inv.fairies;
        case Requirement::crowns: return inv.crowns;
        case Requirement::medals: return inv.medals;
        case Requirement::pearls: return inv.pearls;
        case Requirement::rainbow_coins: return inv.rainbow_coins;
        case Requirement::keys: return std::popcount(inv.keys);
        case Requirement::kongs: return std::popcount(inv.kongs);
        case Requirement::bean: return !!(inv.special & 4);
        case Requirement::nintendo_coin: return !!(inv.special & 1);
        case Requirement::rareware_coin: return !!(inv.special & 2);
        case Requirement::blueprints: { uint32_t count = 0; for (auto bp : inv.blueprints) count += bp; return count; }
        case Requirement::moves: {
            uint32_t count = inv.slam + inv.belt + inv.instrument_upgrades + std::popcount(inv.abilities) + inv.climbing;
            for (unsigned i = 0; i < 5; ++i) count += std::popcount(inv.moves[i]) + (inv.guns[i] & 1) + (inv.instruments[i] & 1);
            return count + std::popcount(uint8_t(inv.guns[0] & 6));
        }
    }
    throw Failure(Error::unsupported);
}
bool meets(const CampaignInventory& inv, Requirement kind, uint32_t amount) { return inventory_count(inv, kind) >= amount; }
}
