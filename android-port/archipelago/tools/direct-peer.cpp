// SPDX-License-Identifier: GPL-3.0-or-later
// Headless verification peer using the exact production Session, with no relay.
#include "session.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
int main(int argc, char** argv) {
    if (argc != 4) { std::cerr << "Usage: ap_peer config.json journal.json checks-mask\n"; return 2; }
    try {
        std::ifstream input(argv[1]); dkap::Json j; input >> j;
        dkap::Config config{j.at("server"), j.at("name"), j.value("password", ""), j.at("seed"), j.value("team", 0u), j.value("slot", 1u)};
        auto expected_scouts = j.value("expect_scouts", 0u);
        dkap::Session session; session.start(config, std::filesystem::absolute(argv[2]));
        auto checks = uint32_t(std::stoul(argv[3]));
        uint32_t previous = ~0u;
        for (unsigned i = 0; i < 400; ++i) {
            auto word = session.tick(checks);
            if (word != previous) { std::cout << word << std::endl; previous = word; }
            if (((word >> 16) & 3) == 2) return 3;
            auto scouts = session.scout_snapshot();
            if ((word & 0x3001fu) == 0x10003u && (!expected_scouts || scouts.locations.size() == expected_scouts)) {
                if (expected_scouts) std::cout << "scouts=" << scouts.locations.size() << " labels=" << scouts.shop_labels.size() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                session.request_stop(); return 0;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return 4;
    } catch (...) { std::cerr << "Peer failed\n"; return 5; }
}
