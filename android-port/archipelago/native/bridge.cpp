// SPDX-License-Identifier: GPL-3.0-or-later
#include "session.hpp"
#include "rdram.hpp"
#include "campaign_channel.hpp"
#include "campaign_session.hpp"
#include "seed.hpp"
#include "recomp.h"
#include "../mod/policy.h"
#include "../mod/ap_shops.h"
#include <cstring>
#include <optional>
#ifdef _WIN32
#define AP_EXPORT extern "C" __declspec(dllexport)
#else
#define AP_EXPORT extern "C" __attribute__((visibility("default")))
#endif
AP_EXPORT uint32_t recomp_api_version = 1;
namespace {
dkap::Session session;
dkap::Config config;
std::string save_name;
bool configured = false, started = false;
dkap::CampaignChannel campaign_channel;
dkap::CampaignSession campaign_session;
std::optional<dkap::CampaignSeed> campaign_seed;
std::string campaign_save_name;
bool campaign_configured = false, campaign_started = false, campaign_connected = false;
void span(uint8_t* rdram, uint32_t address, size_t size) {
    dkap::validate_rdram_span(rdram, address, size);
}
std::string read_string(uint8_t* rdram, uint32_t address, uint32_t max_length) {
    span(rdram, address, size_t(max_length) + 1);
    std::string text;
    for (uint32_t i = 0; i <= max_length; ++i) {
        char c = char(rdram[((address - 0x80000000u) + i) ^ 3]);
        if (!c) return text;
        text.push_back(c);
    }
    throw dkap::Failure(dkap::Error::config);
}
std::string read_bytes(uint8_t* rdram, uint32_t address, uint32_t length) {
    if (!length || length > dkap::message_limit) throw dkap::Failure(dkap::Error::config);
    span(rdram, address, length);
    std::string bytes(length, '\0');
    for (uint32_t i = 0; i < length; ++i) bytes[i] = char(rdram[((address - 0x80000000u) + i) ^ 3]);
    return bytes;
}
void write_string(uint8_t* rdram, uint32_t address, const std::string& value, uint32_t capacity) {
    if (value.size() + 1 > capacity) throw dkap::Failure(dkap::Error::config);
    span(rdram, address, capacity);
    for (uint32_t i = 0; i < capacity; ++i)
        rdram[((address - 0x80000000u) + i) ^ 3] = i < value.size() ? uint8_t(value[i]) : 0;
}
uint32_t word(uint8_t* rdram, uint32_t address) {
    span(rdram, address, 4);
    if (address & 3) throw dkap::Failure(dkap::Error::config);
    uint32_t result;
    std::memcpy(&result, rdram + (address - 0x80000000u), 4);
    return result;
}
}
// Configure before the EEPROM worker starts; this function never connects or writes a save.
AP_EXPORT void dk64_ap_configure_v2(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = 0;
    if (started || campaign_configured || campaign_started) return;
    configured = false;
    try {
        auto input = uint32_t(ctx->r4), output = uint32_t(ctx->r5);
        span(rdram, input, 24); span(rdram, output, 36);
        dkap::Config candidate{read_string(rdram, word(rdram, input), 1024),
            read_string(rdram, word(rdram, input + 4), 256), read_string(rdram, word(rdram, input + 8), 256),
            read_string(rdram, word(rdram, input + 12), 256), word(rdram, input + 16), word(rdram, input + 20)};
        save_name = "ap_" + dkap::identity_for(candidate);
        write_string(rdram, output, save_name, 36);
        config = std::move(candidate); configured = true; ctx->r2 = 1;
    } catch (...) { configured = false; }
}
AP_EXPORT void dk64_ap_start_v2(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = uint32_t(dkap::Status::rejected);
    if (!configured || started || campaign_started) return;
    try {
        auto save = read_string(rdram, uint32_t(ctx->r4), 4095);
        if (!ap_owns_save_path(reinterpret_cast<const unsigned char*>(save.c_str()), save_name.c_str())) return;
        auto path = std::filesystem::path(std::u8string(save.begin(), save.end()));
        // Reject traversal even if the string has a plausible suffix.
        if (!path.is_absolute() || path.lexically_normal() != path) return;
        path.replace_extension(".ap.json");
        session.start(config, path); started = true; ctx->r2 = uint32_t(dkap::Status::waiting);
    } catch (...) { session.request_stop(); }
}
// Atomics only: low five bits = moves, bits 16..17 = status, bits 24..31 = error.
AP_EXPORT void dk64_ap_tick_v2(uint8_t*, recomp_context* ctx) {
    ctx->r2 = started ? session.tick(uint32_t(ctx->r4)) : uint32_t(dkap::Status::rejected) << 16;
}
// Game-thread metadata snapshot. UINT32_MAX means invalid output bounds; zero is
// a valid empty snapshot and must clear names retained from a previous room.
AP_EXPORT void dk64_ap_labels_v2(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = UINT32_MAX;
    try {
        auto output = uint32_t(ctx->r4), capacity = uint32_t(ctx->r5);
        if (capacity > AP_SHOP_COUNT) return;
        span(rdram, output, size_t(capacity) * sizeof(ap_shop_label));
        dkap::ScoutSnapshot snapshot;
        if (started && !session.try_scout_snapshot(snapshot)) { ctx->r2 = UINT32_MAX - 1; return; }
        if (snapshot.shop_labels.size() > capacity) return;
        for (size_t i = 0; i < snapshot.shop_labels.size(); ++i)
            dkap::write_rdram_shop_label(rdram, output + uint32_t(i * sizeof(ap_shop_label)), snapshot.shop_labels[i]);
        ctx->r2 = uint32_t(snapshot.shop_labels.size());
    } catch (...) { ctx->r2 = UINT32_MAX; }
}
AP_EXPORT void dk64_ap_stop_v2(uint8_t*, recomp_context* ctx) {
    session.request_stop(); ctx->r2 = 0;
}

// Dormant full-campaign ABI. Manifest parsing is bounded and side-effect free;
// no campaign network or gameplay caller is connected to these exports yet.
AP_EXPORT void dk64_ap_campaign_configure_v3(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = 0;
    if (configured || started || campaign_started) return;
    campaign_configured = false;
    campaign_seed.reset(); campaign_save_name.clear();
    try {
        auto manifest = read_bytes(rdram, uint32_t(ctx->r4), uint32_t(ctx->r5));
        auto candidate = dkap::CampaignSeed::parse(dkap::parse_bounded(manifest));
        auto name = "ap_" + candidate.binding_hex();
        write_string(rdram, uint32_t(ctx->r6), name, 36);
        campaign_seed = std::move(candidate); campaign_save_name = std::move(name);
        campaign_configured = true; ctx->r2 = 1;
    } catch (...) {
        campaign_seed.reset(); campaign_save_name.clear(); campaign_configured = false;
    }
}

AP_EXPORT void dk64_ap_campaign_start_v3(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = uint32_t(dkap::Status::rejected);
    if (!campaign_configured || !campaign_seed || campaign_started || started) return;
    try {
        auto save = read_string(rdram, uint32_t(ctx->r4), 4095);
        if (!ap_owns_save_path(reinterpret_cast<const unsigned char*>(save.c_str()), campaign_save_name.c_str())) return;
        auto path = std::filesystem::path(std::u8string(save.begin(), save.end()));
        if (!path.is_absolute() || path.lexically_normal() != path) return;
        path.replace_extension(".campaign.json");
        campaign_channel.start(*campaign_seed, path);
        campaign_started = true; ctx->r2 = uint32_t(dkap::Status::waiting);
    } catch (...) { campaign_channel.stop(); campaign_started = false; }
}

AP_EXPORT void dk64_ap_campaign_connect_v3(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = uint32_t(dkap::Status::rejected);
    if (!campaign_started || !campaign_seed || campaign_connected) return;
    try {
        dkap::Config network{read_string(rdram, uint32_t(ctx->r4), 1024), campaign_seed->player,
            read_string(rdram, uint32_t(ctx->r5), 256), campaign_seed->seed_name,
            campaign_seed->team, campaign_seed->slot};
        dkap::validate_config(network);
        campaign_session.start(network, *campaign_seed, campaign_channel);
        campaign_connected = true; ctx->r2 = uint32_t(dkap::Status::waiting);
    } catch (...) { campaign_session.stop(); campaign_connected = false; }
}

AP_EXPORT void dk64_ap_campaign_tick_v3(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = uint32_t(dkap::Status::rejected) << 16;
    if (!campaign_connected) return;
    try { ctx->r2 = campaign_session.tick(dkap::read_rdram_check_set(rdram, uint32_t(ctx->r4))); }
    catch (...) { ctx->r2 = uint32_t(dkap::Error::protocol) << 24 | uint32_t(dkap::Status::rejected) << 16; }
}

// UINT32_MAX is rejected, -1 is retry, -2 is an unbound game save and -3 is
// an unavailable journal worker. Any other value is the staged item count.
AP_EXPORT void dk64_ap_campaign_stage_v3(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = UINT32_MAX;
    if (!campaign_started) return;
    try {
        auto capacity = uint32_t(ctx->r6);
        if (capacity > dkap::item_limit) return;
        auto identity = dkap::read_rdram_campaign_save(rdram, uint32_t(ctx->r4));
        dkap::CampaignStageSnapshot snapshot;
        switch (campaign_channel.try_stage(identity, snapshot)) {
        case dkap::CampaignChannelResult::retry: ctx->r2 = UINT32_MAX - 1; return;
        case dkap::CampaignChannelResult::unbound: ctx->r2 = UINT32_MAX - 2; return;
        case dkap::CampaignChannelResult::unavailable: ctx->r2 = UINT32_MAX - 3; return;
        case dkap::CampaignChannelResult::rejected: return;
        case dkap::CampaignChannelResult::ready: break;
        }
        if (snapshot.item_ids.size() > capacity) return;
        dkap::write_rdram_campaign_stage(rdram, uint32_t(ctx->r5), snapshot);
        ctx->r2 = uint32_t(snapshot.item_ids.size());
    } catch (...) { ctx->r2 = UINT32_MAX; }
}

AP_EXPORT void dk64_ap_campaign_stop_v3(uint8_t*, recomp_context* ctx) {
    campaign_session.stop(); campaign_channel.stop(); campaign_seed.reset(); campaign_save_name.clear();
    campaign_connected = false; campaign_started = false; campaign_configured = false; ctx->r2 = 0;
}
