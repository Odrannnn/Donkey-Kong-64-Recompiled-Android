#pragma once
#include <cstdint>
#include <filesystem>
#include "../mod/recovery_storage.h"

namespace dkcoop {

struct RecoveryJournalConfig {
    std::filesystem::path save_path;
    uint32_t profile = 0;
    uint32_t save_kind = 0;
    uint32_t room = 0;
};

class RecoveryJournal {
public:
    uint32_t configure(const RecoveryJournalConfig& config);
    void reset();
    void observe(bool checkpoint, bool persist_safe, uint32_t snapshot, uint32_t room);
    bool promote(uint32_t room);
    bool follow(uint64_t term, uint64_t leader);
    uint32_t status() const;
    uint64_t authority_term() const;
    uint64_t authority_node() const;
    uint64_t node_id() const;

private:
    struct Record {
        uint32_t profile = 0, save_kind = 0, flags = 0, room = 0, snapshot = 0;
        uint64_t term = 0, node = 0, leader = 0, save_size = 0, save_hash = 0;
    };
    RecoveryJournalConfig config_{};
    std::filesystem::path journal_path_{};
    Record record_{};
    bool configured_ = false, error_ = false, save_advanced_ = false;

    bool load();
    bool commit(const Record& record);
    bool digest(uint64_t& size, uint64_t& hash) const;
};

}
