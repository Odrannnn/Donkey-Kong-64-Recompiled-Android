// SPDX-License-Identifier: GPL-3.0-or-later
// Headless verification peer using the production campaign session and journal channel.
#include "campaign_session.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: ap_campaign_peer config.json manifest.json journal.json expected-items\n"; return 2;
    }
    try {
        std::ifstream config_input(argv[1]), manifest_input(argv[2]);
        dkap::Json config_json, manifest_json; config_input >> config_json; manifest_input >> manifest_json;
        auto seed = dkap::CampaignSeed::parse(manifest_json);
        dkap::Config config{config_json.at("server"), seed.player, config_json.value("password", ""),
            seed.seed_name, seed.team, seed.slot};
        dkap::CampaignChannel channel; channel.start(seed, std::filesystem::absolute(argv[3]));
        dkap::CampaignSession session; session.start(config, seed, channel);
        ap_check_set observed{};
        for (auto id : seed.locations) ap_check_add(&observed, uint32_t(id));
        auto expected = size_t(std::stoul(argv[4]));
        uint32_t previous = ~0u;
        for (unsigned i = 0; i < 400; ++i) {
            auto word = session.tick(observed);
            if (word != previous) { std::cout << word << std::endl; previous = word; }
            if (((word >> 16) & 3) == uint32_t(dkap::Status::rejected)) return 3;
            if (((word >> 16) & 3) == uint32_t(dkap::Status::ready)
                && channel.snapshot().receipt_count == expected) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                session.stop(); channel.stop(); return 0;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        session.stop(); channel.stop(); return 4;
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 5; }
}
