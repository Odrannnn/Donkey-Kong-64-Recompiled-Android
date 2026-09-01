// SPDX-License-Identifier: GPL-3.0-or-later
#include "receipts.hpp"
#include <algorithm>

namespace dkap {
static_assert(item_limit <= UINT16_MAX);
namespace {
bool unbound(const ap_campaign_save& save) {
    const auto* first = reinterpret_cast<const unsigned char*>(&save);
    return std::all_of(first, first + sizeof(save), [](unsigned char byte) { return byte == 0; });
}
}

CampaignReceipts::CampaignReceipts(const CampaignSeed& seed, std::vector<Item> durable_receipts)
    : binding_(seed.binding), receipts_(std::move(durable_receipts)) {
    if (receipts_.size() > item_limit) throw Failure(Error::protocol);
    for (const auto& receipt : receipts_) {
        if (receipt[2] < 0 || receipt[2] > 65535 || receipt[3] < 0 || receipt[3] > 7)
            throw Failure(Error::protocol);
        (void)CampaignCatalog::builtin().item(receipt[0]);
    }
}

ap_campaign_save CampaignReceipts::initialize() const {
    ap_campaign_save save{};
    save.magic = AP_CAMPAIGN_SAVE_MAGIC;
    save.format = AP_CAMPAIGN_SAVE_FORMAT;
    std::copy(binding_.begin(), binding_.end(), save.binding);
    ap_campaign_save_seal(&save);
    return save;
}

CampaignReceiptStep CampaignReceipts::next(const ap_campaign_save& save) const {
    if (unbound(save)) return {};
    if (!ap_campaign_save_valid(&save)) throw Failure(Error::storage);
    if (save.magic != AP_CAMPAIGN_SAVE_MAGIC || save.format != AP_CAMPAIGN_SAVE_FORMAT)
        throw Failure(Error::storage);
    if (!std::equal(binding_.begin(), binding_.end(), save.binding)) throw Failure(Error::seed);
    if (save.counter > receipts_.size()) throw Failure(Error::storage);
    if (save.counter == receipts_.size()) return {CampaignReceiptStatus::complete, save.counter, std::nullopt};
    return {CampaignReceiptStatus::receipt, save.counter, receipts_[save.counter]};
}
}
