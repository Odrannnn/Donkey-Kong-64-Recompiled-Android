// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <nlohmann/json.hpp>
#include <array>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace dkap {
using Json = nlohmann::json;
inline constexpr const char* game = "DK64 Recomp Integration Test";
inline constexpr const char* profile = "dk64-recomp-first-moves-japes-v1";
inline constexpr const char* world_version = "1.5.8";
inline constexpr uint32_t check_mask = 3, move_mask = 31;
inline constexpr size_t message_limit = 1024 * 1024, item_limit = 4096;
inline constexpr std::array<int64_t, 5> item_ids{14041104, 14041108, 14041112, 14041116, 14041120};
inline constexpr std::array<int64_t, 2> location_ids{14041180, 14041181};
enum class Status : uint32_t { waiting = 0, ready = 1, rejected = 2, off = 3 };
enum class Error : uint32_t { none, config, seed, slot, unsupported, storage, protocol, transport };
struct Failure : std::runtime_error {
    Error code;
    explicit Failure(Error code) : std::runtime_error("Archipelago session rejected"), code(code) {}
};
struct Config {
    std::string server, name, password, seed;
    uint32_t team = 0, slot = 1;
};
void validate_config(const Config& config);
Json binding(const Config& config);
std::string identity_for(const Config& config);
Json parse_bounded(const std::string& text);
using Item = std::array<int64_t, 4>;
class Journal {
    struct Impl;
    std::unique_ptr<Impl> impl;
public:
    explicit Journal(const std::filesystem::path& path);
    ~Journal();
    Json load();
    void commit(const Json& data);
};
struct ScoutSnapshot;
class Scouts;
class State {
    Config config_;
    std::string identity_;
    Journal journal_;
    std::vector<Item> items_;
    uint32_t checks_ = 0, confirmed_ = 0;
    bool room_ = false, connected_ = false, synced_ = false, goal_sent_ = false;
    std::unique_ptr<Scouts> scouts_;
    void persist(const std::vector<Item>& items, uint32_t checks);
public:
    State(const Config& config, const std::filesystem::path& journal);
    ~State();
    void disconnect();
    void record_checks(uint32_t checks);
    std::vector<Json> receive(const std::string& text);
    std::vector<Json> flush();
    bool ready() const { return room_ && connected_ && synced_; }
    uint32_t moves() const;
    uint32_t checks() const { return checks_; }
    size_t item_count() const { return items_.size(); }
    ScoutSnapshot scout_snapshot() const;
    const std::string& identity() const { return identity_; }
};
}
