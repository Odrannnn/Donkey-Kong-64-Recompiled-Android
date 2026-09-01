// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "state.hpp"
#include "../mod/shop_labels.h"
#include <map>
#include <set>

namespace dkap {
struct ScoutRecord {
    int64_t location = 0, item = 0;
    uint32_t recipient = 0, flags = 0;
    std::string game, item_name, recipient_name;
    bool operator==(const ScoutRecord&) const = default;
};
struct ScoutSnapshot {
    bool enabled = false;
    std::vector<ScoutRecord> locations;
    std::vector<ap_shop_label> shop_labels;
};

// Validates and resolves AP LocationScouts metadata. It never changes checks,
// receipts, inventory, or the durable journal.
class Scouts {
    struct Slot { std::string game, name; uint32_t type = 0; };
    struct Placement { int64_t item = 0; uint32_t recipient = 0, flags = 0; bool operator==(const Placement&) const = default; };
    std::vector<int64_t> selected_;
    uint32_t team_ = 0;
    bool room_metadata_ = false, enabled_ = false;
    std::map<std::string, std::string> checksums_;
    std::map<uint32_t, Slot> slots_;
    std::map<uint32_t, std::string> aliases_;
    std::map<int64_t, Placement> placements_;
    std::map<std::string, std::map<int64_t, std::string>> item_names_;
    std::set<std::string> requested_games_;
    ScoutSnapshot build_snapshot() const;
public:
    explicit Scouts(std::vector<int64_t> selected);
    void disconnect();
    void room(const Json& message);
    std::vector<Json> connected(const Json& message, uint32_t team, uint32_t slot);
    std::vector<Json> location_info(const Json& message);
    void data_package(const Json& message);
    void room_update(const Json& message);
    ScoutSnapshot snapshot() const { return build_snapshot(); }
};
}
