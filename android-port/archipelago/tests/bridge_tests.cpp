// SPDX-License-Identifier: GPL-3.0-or-later
#include "recomp.h"
#include "rdram.hpp"
#include "seed.hpp"
#include "../mod/campaign_stage.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
extern "C" void dk64_ap_configure_v2(uint8_t*, recomp_context*);
extern "C" void dk64_ap_start_v2(uint8_t*, recomp_context*);
extern "C" void dk64_ap_tick_v2(uint8_t*, recomp_context*);
extern "C" void dk64_ap_labels_v2(uint8_t*, recomp_context*);
extern "C" void dk64_ap_stop_v2(uint8_t*, recomp_context*);
extern "C" void dk64_ap_campaign_configure_v3(uint8_t*, recomp_context*);
extern "C" void dk64_ap_campaign_start_v3(uint8_t*, recomp_context*);
extern "C" void dk64_ap_campaign_connect_v3(uint8_t*, recomp_context*);
extern "C" void dk64_ap_campaign_tick_v3(uint8_t*, recomp_context*);
extern "C" void dk64_ap_campaign_stage_v3(uint8_t*, recomp_context*);
extern "C" void dk64_ap_campaign_stop_v3(uint8_t*, recomp_context*);
int main() {
    recomp_context ctx{}; uint8_t tiny[32]{};
    for (uint32_t pointer : {0u, 0x7fffffffu, 0x9fffffffu, 0xffffffffu}) {
        ctx.r4 = pointer; ctx.r5 = pointer;
        dk64_ap_configure_v2(tiny, &ctx); if (ctx.r2 != 0) return 1;
    }
    ctx.r4 = 0x80000000u; dk64_ap_configure_v2(nullptr, &ctx); if (ctx.r2) return 2;
    dk64_ap_start_v2(nullptr, &ctx); if (ctx.r2 != 2) return 3;
    dk64_ap_tick_v2(nullptr, &ctx); if (ctx.r2 != 2u << 16) return 4;
    ctx.r4 = 0x80000000u; ctx.r5 = 121; dk64_ap_labels_v2(tiny, &ctx); if (ctx.r2 != UINT32_MAX) return 8;
    dk64_ap_stop_v2(nullptr, &ctx);
    std::vector<uint8_t> memory(32768);
    auto put = [&](uint32_t offset, const char* value) {
        for (size_t i = 0; i <= std::strlen(value); ++i) memory[(offset + i) ^ 3] = uint8_t(value[i]);
    };
    auto put_bytes = [&](uint32_t offset, const std::string& value) {
        for (size_t i = 0; i < value.size(); ++i) memory[(offset + i) ^ 3] = uint8_t(value[i]);
    };
    auto get_string = [&](uint32_t offset) {
        std::string value;
        while (memory[(offset + value.size()) ^ 3]) value += char(memory[(offset + value.size()) ^ 3]);
        return value;
    };

    dkap::Json manifest{{"format", 1}, {"world_version", "1.5.8"},
        {"revision", "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4"}, {"game", "Donkey Kong 64"},
        {"seed_name", "bridge-seed"}, {"player", "DK64Test"}, {"team", 0}, {"slot", 1},
        {"locations", dkap::Json::array({14041180, 14041181})}, {"starting_inventory", dkap::Json::array()},
        {"boss_bananas", dkap::Json::array({60,120,200,250,300,350,400})},
        {"edits", {{"entrances", dkap::Json::array()}}}};
    manifest["blockers"] = dkap::Json::array();
    for (int i = 0; i < 8; ++i) manifest["blockers"].push_back({{"item", "keys"}, {"count", i + 1}});
    auto manifest_bytes = manifest.dump();
    put_bytes(0x6000, manifest_bytes);
    ctx.r4 = 0x80006000u; ctx.r5 = manifest_bytes.size(); ctx.r6 = 0x80005f00u;
    dk64_ap_campaign_configure_v3(memory.data(), &ctx); if (ctx.r2 != 1) return 16;
    auto campaign_name = get_string(0x5f00);
    auto seed = dkap::CampaignSeed::parse(manifest);
    if (campaign_name != "ap_" + seed.binding_hex() || campaign_name.size() != 35) return 17;
    // The ordinary test session cannot configure while the campaign owns the bridge.
    ctx.r4 = 0x80000000u; ctx.r5 = 0x80000200u;
    dk64_ap_configure_v2(memory.data(), &ctx); if (ctx.r2) return 18;

    auto campaign_root = std::filesystem::temp_directory_path()
        / ("dkap-bridge-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    auto owner = campaign_root / "odrannnn_dk64_archipelago";
    std::filesystem::create_directories(owner);
    auto campaign_save = owner / (campaign_name + ".bin");
    auto campaign_path = campaign_save.generic_string();
    put(0x7000, campaign_path.c_str()); ctx.r4 = 0x80007000u;
    dk64_ap_campaign_start_v3(memory.data(), &ctx); if (ctx.r2 != uint32_t(dkap::Status::waiting)) return 19;
    ctx.r4 = 0x80005700u; dk64_ap_campaign_tick_v3(memory.data(), &ctx);
    if (ctx.r2 != uint32_t(dkap::Status::rejected) << 16) return 27;
    put(0x7200, "not-a-websocket-url"); put(0x7300, "");
    ctx.r4 = 0x80007200u; ctx.r5 = 0x80007300u; dk64_ap_campaign_connect_v3(memory.data(), &ctx);
    if (ctx.r2 != uint32_t(dkap::Status::rejected)) return 28;
    put(0x7200, "ws://127.0.0.1:1");
    ctx.r4 = 0x80007200u; ctx.r5 = 0x80007300u; dk64_ap_campaign_connect_v3(memory.data(), &ctx);
    if (ctx.r2 != uint32_t(dkap::Status::waiting)) return 29;
    ctx.r4 = 0x80005700u; dk64_ap_campaign_tick_v3(memory.data(), &ctx);
    if (((ctx.r2 >> 16) & 3) != uint32_t(dkap::Status::waiting)) return 30;
    ctx.r4 = 0x80005701u; dk64_ap_campaign_tick_v3(memory.data(), &ctx);
    if (ctx.r2 != (uint32_t(dkap::Error::protocol) << 24 | uint32_t(dkap::Status::rejected) << 16)) return 31;

    ap_campaign_save identity{};
    dkap::write_rdram_campaign_save(memory.data(), 0x80005800u, identity);
    auto poll_stage = [&](uint32_t expected) {
        for (unsigned i = 0; i < 200; ++i) {
            ctx.r4 = 0x80005800u; ctx.r5 = 0x80004000u; ctx.r6 = AP_CAMPAIGN_RECEIPT_LIMIT;
            dk64_ap_campaign_stage_v3(memory.data(), &ctx);
            if (ctx.r2 == expected) return true;
            if (ctx.r2 != UINT32_MAX - 1 && ctx.r2 != UINT32_MAX - 3) return false;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return false;
    };
    if (!poll_stage(UINT32_MAX - 2)) return 20;
    identity = dkap::CampaignReceipts(seed, {}).initialize();
    dkap::write_rdram_campaign_save(memory.data(), 0x80005800u, identity);
    if (!poll_stage(0)) return 21;
    uint32_t staged_magic = 0, staged_count = 1;
    std::memcpy(&staged_magic, memory.data() + 0x4000, 4);
    std::memcpy(&staged_count, memory.data() + 0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, item_count), 4);
    if (staged_magic != AP_CAMPAIGN_STAGE_MAGIC || staged_count != 0
        || dkap::read_rdram_campaign_save(memory.data(), 0x80004008u).checksum != identity.checksum) return 22;
    auto wrong = identity; wrong.binding[0] ^= 1; ap_campaign_save_seal(&wrong);
    dkap::write_rdram_campaign_save(memory.data(), 0x80005800u, wrong);
    if (!poll_stage(UINT32_MAX)) return 23;
    ctx.r4 = 0x80005801u; ctx.r5 = 0x80004000u; ctx.r6 = AP_CAMPAIGN_RECEIPT_LIMIT;
    dk64_ap_campaign_stage_v3(memory.data(), &ctx); if (ctx.r2 != UINT32_MAX) return 24;
    ctx.r4 = 0x80005800u; ctx.r6 = AP_CAMPAIGN_RECEIPT_LIMIT + 1u;
    dk64_ap_campaign_stage_v3(memory.data(), &ctx); if (ctx.r2 != UINT32_MAX) return 25;
    dk64_ap_campaign_stop_v3(nullptr, &ctx); if (ctx.r2) return 26;
    std::filesystem::remove_all(campaign_root);

    put(1024, "ws://127.0.0.1:38281"); put(2048, "DK64Test"); put(3072, ""); put(4096, "seed");
    uint32_t config[6] = {0x80000400, 0x80000800, 0x80000c00, 0x80001000, 0, 1};
    std::memcpy(memory.data(), config, sizeof(config));
    ctx.r4 = 0x80000000; ctx.r5 = 0x80000200;
    dk64_ap_configure_v2(memory.data(), &ctx); if (ctx.r2 != 1) return 5;
    if (memory[512 ^ 3] != 'a' || memory[513 ^ 3] != 'p' || memory[514 ^ 3] != '_' || memory[547 ^ 3]) return 6;
    put(8192, "/tmp/ordinary-save.bin"); ctx.r4 = 0x80002000;
    dk64_ap_start_v2(memory.data(), &ctx); if (ctx.r2 != 2) return 7;
    ctx.r4 = 0x80003000u; ctx.r5 = 120; dk64_ap_labels_v2(memory.data(), &ctx); if (ctx.r2 != 0) return 9;
    ctx.r4 = 0x9fffffffu; ctx.r5 = 120; dk64_ap_labels_v2(memory.data(), &ctx); if (ctx.r2 != UINT32_MAX) return 10;
    ap_shop_label label{}; label.location = 0x12345678u; label.frames = 0x3456u;
    std::memcpy(label.item, "ITEM", 4); std::memcpy(label.subtitle, "TO PLAYER", 9);
    dkap::write_rdram_shop_label(memory.data(), 0x80003000u, label);
    uint32_t stored_word = 0; uint16_t stored_half = 0;
    std::memcpy(&stored_word, memory.data() + 0x3000, 4);
    std::memcpy(&stored_half, memory.data() + (0x3004 ^ 2), 2);
    if (stored_word != label.location || stored_half != label.frames
        || memory[(0x3008u ^ 3)] != 'I' || memory[(0x3029u ^ 3)] != 'T') return 11;
    dkap::CampaignStageSnapshot stage;
    stage.identity.magic = AP_CAMPAIGN_SAVE_MAGIC; stage.identity.format = AP_CAMPAIGN_SAVE_FORMAT;
    stage.identity.counter = 2;
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) stage.identity.binding[i] = uint8_t(0x40 + i);
    ap_campaign_save_seal(&stage.identity);
    ap_check_add(&stage.selected, 14041180); ap_check_add(&stage.selected, 14041181);
    ap_check_add(&stage.observed, 14041181);
    stage.item_ids = {14041104, 14041108};
    stage.next_item = 14041112;
    dkap::write_rdram_campaign_stage(memory.data(), 0x80004000u, stage);
    auto read_word = [&](uint32_t offset) { uint32_t value; std::memcpy(&value, memory.data() + offset, 4); return value; };
    auto read_half = [&](uint32_t offset) { uint16_t value; std::memcpy(&value, memory.data() + (offset ^ 2), 2); return value; };
    if (read_word(0x4000) != AP_CAMPAIGN_STAGE_MAGIC || read_half(0x4004) != AP_CAMPAIGN_STAGE_FORMAT
        || read_word(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, item_count)) != 2
        || read_word(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, next_item_id)) != 14041112
        || read_half(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, next_item_valid)) != 1
        || read_half(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, identity) + 6) != 2
        || memory[(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, identity) + 8) ^ 3] != 0x40
        || read_word(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, selected) + 4) != 0x300
        || read_word(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, observed) + 4) != 0x200
        || read_word(0x4000 + AP_OFFSETOF(ap_campaign_stage_wire, item_ids)) != 14041104) return 12;
    auto rejected_stage = [&](uint32_t address, const dkap::CampaignStageSnapshot& value) {
        try { dkap::write_rdram_campaign_stage(memory.data(), address, value); }
        catch (const dkap::Failure& failure) { return failure.code == dkap::Error::config; }
        return false;
    };
    std::memset(memory.data() + 0x5000, 0xA5, 512);
    auto bad = stage; bad.identity.counter = 1; ap_campaign_save_seal(&bad.identity);
    if (!rejected_stage(0x80005000u, bad)) return 13;
    for (unsigned i = 0; i < 512; ++i) if (memory[0x5000 + i] != 0xA5) return 14;
    bad = stage; ap_check_add(&bad.observed, 14041182);
    if (!rejected_stage(0x80005000u, bad)) return 15;
    bad = stage; bad.selected = {}; bad.observed = {};
    bool alias_found = false;
    for (unsigned i = 0; i < AP_LOCATION_COUNT && !alias_found; ++i) if (ap_locations[i].flag != 65535)
        for (unsigned j = i + 1; j < AP_LOCATION_COUNT; ++j) if (ap_locations[i].flag == ap_locations[j].flag) {
            ap_check_add(&bad.selected, ap_locations[i].id); ap_check_add(&bad.selected, ap_locations[j].id);
            alias_found = true; break;
        }
    if (!alias_found || !rejected_stage(0x80005000u, bad) || !rejected_stage(0x80005001u, stage)
        || !rejected_stage(0x9ffffff0u, stage)) return 15;
    std::cout << "PASS: native ABI bounds, isolated campaign channel, word-swapped staging, labels and save ownership\n";
}
