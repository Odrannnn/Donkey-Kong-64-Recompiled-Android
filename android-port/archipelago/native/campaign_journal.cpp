// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_journal.hpp"
#include <algorithm>
#include <cstring>

namespace dkap {
namespace {
int64_t integer(const Json& value, int64_t low, int64_t high) {
    if (!value.is_number_integer() || (value.is_number_unsigned() && value.get<uint64_t>() > uint64_t(high)))
        throw Failure(Error::storage);
    auto number = value.get<int64_t>();
    if (number < low || number > high) throw Failure(Error::storage);
    return number;
}

Item stored_item(const Json& value) {
    if (!value.is_array() || value.size() != 4) throw Failure(Error::storage);
    return {integer(value[0], 0, UINT32_MAX), integer(value[1], INT64_MIN, INT64_MAX),
        integer(value[2], 0, 65535), integer(value[3], 0, 7)};
}

bool subset(const ap_check_set& part, const ap_check_set& whole) {
    if (!ap_check_valid(&part)) return false;
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i) if (part.words[i] & ~whole.words[i]) return false;
    return true;
}
}

CampaignJournalState::CampaignJournalState(const CampaignSeed& seed, const std::filesystem::path& path)
    : seed_(seed), journal_(path) {
    for (auto id : seed_.locations) {
        if (id < 0 || id > UINT32_MAX || !ap_check_add(&selected_, uint32_t(id))) throw Failure(Error::storage);
    }
    try {
        auto saved = journal_.load();
        if (saved.is_null()) { persist({}, {}); return; }
        static const std::array<const char*, 4> fields{"format", "binding", "items", "checks"};
        if (!saved.is_object() || saved.size() != fields.size()) throw Failure(Error::storage);
        for (const auto& field : fields) if (!saved.contains(field)) throw Failure(Error::storage);
        if (!saved.at("format").is_number_integer() || saved.at("format") != 1
            || !saved.at("binding").is_string() || saved.at("binding") != seed_.binding_hex()
            || !saved.at("items").is_array() || saved.at("items").size() > item_limit
            || !saved.at("checks").is_array() || saved.at("checks").size() > seed_.locations.size())
            throw Failure(Error::storage);
        std::vector<Item> items;
        for (const auto& value : saved.at("items")) items.push_back(stored_item(value));
        (void)CampaignReceipts(seed_, items); // Validate every receipt before publication.
        ap_check_set observed{};
        for (const auto& value : saved.at("checks")) {
            auto id = uint32_t(integer(value, 0, UINT32_MAX));
            if (!ap_check_has(&selected_, id) || ap_check_has(&observed, id) || !ap_check_add(&observed, id))
                throw Failure(Error::storage);
        }
        items_ = std::move(items); observed_ = observed;
    } catch (...) { throw Failure(Error::storage); }
}

void CampaignJournalState::persist(const std::vector<Item>& items, const ap_check_set& observed) {
    Json checks = Json::array();
    for (unsigned i = 0; i < AP_LOCATION_COUNT; ++i)
        if (ap_check_has_index(&observed, i)) checks.push_back(ap_locations[i].id);
    journal_.commit(Json{{"format", 1}, {"binding", seed_.binding_hex()}, {"items", items}, {"checks", checks}});
}

bool CampaignJournalState::receive(size_t index, const std::vector<Item>& incoming) {
    if (index > item_limit || incoming.size() > item_limit - index) throw Failure(Error::protocol);
    (void)CampaignReceipts(seed_, incoming); // Validate the whole update first.
    if (index > items_.size() || (index == 0 && incoming.size() < items_.size())) throw Failure(Error::protocol);
    auto overlap = std::min(incoming.size(), items_.size() - index);
    if (!std::equal(incoming.begin(), incoming.begin() + overlap, items_.begin() + index))
        throw Failure(Error::protocol);
    std::vector<Item> updated = items_;
    updated.insert(updated.end(), incoming.begin() + overlap, incoming.end());
    if (updated == items_) return false;
    persist(updated, observed_); // Disk is authoritative before publication.
    items_ = std::move(updated);
    return true;
}

bool CampaignJournalState::record(const ap_check_set& observed) {
    if (!subset(observed, selected_)) throw Failure(Error::protocol);
    ap_check_set updated = observed_;
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i) updated.words[i] |= observed.words[i];
    if (!std::memcmp(&updated, &observed_, sizeof(updated))) return false;
    persist(items_, updated); // Never acknowledge volatile observations first.
    observed_ = updated;
    return true;
}

std::optional<CampaignStageSnapshot> CampaignJournalState::stage(const ap_campaign_save& identity) const {
    CampaignReceipts transaction(seed_, items_);
    auto step = transaction.next(identity);
    if (step.status == CampaignReceiptStatus::unbound) return std::nullopt;
    std::vector<Item> prefix(items_.begin(), items_.begin() + identity.counter);
    if (!CampaignCatalog::builtin().inventory(prefix).unsupported.empty()) throw Failure(Error::unsupported);
    CampaignStageSnapshot result;
    result.identity = identity; result.selected = selected_; result.observed = observed_;
    if (step.item) result.next_item = uint32_t((*step.item)[0]);
    result.item_ids.reserve(prefix.size());
    for (const auto& item : prefix) result.item_ids.push_back(uint32_t(item[0]));
    return result;
}
}
