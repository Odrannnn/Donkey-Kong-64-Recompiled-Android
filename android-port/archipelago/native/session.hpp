// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "state.hpp"
#include "scouts.hpp"
namespace dkap {
class Session {
    struct Impl;
    std::unique_ptr<Impl> impl;
public:
    Session();
    ~Session();
    void start(const Config&, const std::filesystem::path& journal);
    void request_stop();
    uint32_t tick(uint32_t checks);
    ScoutSnapshot scout_snapshot() const;
    bool try_scout_snapshot(ScoutSnapshot& output) const;
};
}
