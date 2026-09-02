#include "recovery_journal.hpp"
#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace dkcoop {
namespace {
constexpr uint32_t format = 1;
constexpr uint32_t flag_checkpoint = 1u << 0;
constexpr uint32_t flag_promoted = 1u << 1;
constexpr size_t record_size = 68;
constexpr std::array<uint8_t, 8> magic{'D','K','6','4','C','R','J','1'};

void put32(std::vector<uint8_t>& bytes, uint32_t value) {
    for (unsigned shift : {24u, 16u, 8u, 0u}) bytes.push_back(uint8_t(value >> shift));
}
void put64(std::vector<uint8_t>& bytes, uint64_t value) {
    put32(bytes, uint32_t(value >> 32)); put32(bytes, uint32_t(value));
}
uint32_t get32(const uint8_t*& p) {
    uint32_t value = 0;
    for (unsigned i = 0; i < 4; ++i) value = (value << 8) | *p++;
    return value;
}
uint64_t get64(const uint8_t*& p) { return (uint64_t(get32(p)) << 32) | get32(p); }
uint32_t crc32(const uint8_t* data, size_t size) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}
uint64_t random_node() {
    std::random_device random;
    uint64_t value = (uint64_t(random()) << 32) ^ random();
    return value ? value : 1;
}
std::string expected_name(uint32_t profile, uint32_t kind) {
    const bool host = kind & COOP_RECOVERY_SAVE_HOST;
    const bool items = kind & COOP_RECOVERY_SAVE_ITEMS;
    std::string name = items ? "items_" : "prototype_";
    name += host ? "host_" : "guest_";
    name += items ? "v6" : "v1";
    if (profile) name += "_campaign" + std::to_string(profile + 1);
    return name + ".bin";
}
bool sync_file(FILE* file) {
    if (std::fflush(file) != 0) return false;
#ifdef _WIN32
    return _commit(_fileno(file)) == 0;
#else
    return fsync(fileno(file)) == 0;
#endif
}
bool replace_file(const std::filesystem::path& temporary, const std::filesystem::path& destination) {
#ifdef _WIN32
    return MoveFileExW(temporary.c_str(), destination.c_str(),
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    if (::rename(temporary.c_str(), destination.c_str()) != 0) return false;
    int directory = open(destination.parent_path().c_str(), O_RDONLY | O_DIRECTORY);
    if (directory >= 0) { (void)fsync(directory); close(directory); }
    return true;
#endif
}
}

void RecoveryJournal::reset() {
    config_ = {}; journal_path_.clear(); record_ = {};
    configured_ = error_ = save_advanced_ = false;
}

uint32_t RecoveryJournal::configure(const RecoveryJournalConfig& config) {
    reset();
    try {
        if (config.profile >= 8 || config.save_kind > 3 || config.room < 100000 || config.room > 999999
                || !config.save_path.is_absolute() || config.save_path.lexically_normal() != config.save_path
                || config.save_path.filename().string() != expected_name(config.profile, config.save_kind)
                || !std::filesystem::is_directory(config.save_path.parent_path())) {
            error_ = true; return status();
        }
        if (std::filesystem::exists(config.save_path)
                && (std::filesystem::is_symlink(config.save_path)
                    || !std::filesystem::is_regular_file(config.save_path))) {
            error_ = true; return status();
        }
        config_ = config;
        journal_path_ = config.save_path; journal_path_ += ".coop-recovery";
        if (std::filesystem::exists(journal_path_)
                && (std::filesystem::is_symlink(journal_path_)
                    || !std::filesystem::is_regular_file(journal_path_))) {
            error_ = true; return status();
        }
        configured_ = true;
        if (!load()) return status();
        if (!record_.node) record_.node = random_node();
        if (record_.flags & flag_checkpoint) {
            uint64_t size = 0, hash = 0;
            if (!digest(size, hash)) {
                record_.flags &= ~(flag_checkpoint | flag_promoted);
                error_ = true;
            } else if (size != record_.save_size || hash != record_.save_hash) {
                Record candidate = record_;
                candidate.flags &= ~flag_checkpoint;
                if (!commit(candidate)) error_ = true;
                else save_advanced_ = true;
            }
        }
    } catch (...) { error_ = true; }
    return status();
}

bool RecoveryJournal::load() {
    if (!std::filesystem::exists(journal_path_)) {
        record_.profile = config_.profile; record_.save_kind = config_.save_kind;
        record_.room = config_.room; record_.node = random_node();
        return true;
    }
    if (std::filesystem::file_size(journal_path_) != record_size) { error_ = true; return false; }
    std::array<uint8_t, record_size> bytes{};
    std::ifstream input(journal_path_, std::ios::binary);
    input.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
    if (!input || !std::equal(magic.begin(), magic.end(), bytes.begin())
            || crc32(bytes.data(), bytes.size() - 4) !=
                (uint32_t(bytes[64]) << 24 | uint32_t(bytes[65]) << 16
                    | uint32_t(bytes[66]) << 8 | bytes[67])) {
        error_ = true; return false;
    }
    const uint8_t* p = bytes.data() + magic.size();
    if (get32(p) != format) { error_ = true; return false; }
    Record candidate;
    candidate.profile = get32(p); candidate.save_kind = get32(p); candidate.flags = get32(p);
    candidate.room = get32(p); candidate.snapshot = get32(p); candidate.term = get64(p);
    candidate.node = get64(p); candidate.save_size = get64(p); candidate.save_hash = get64(p);
    if (candidate.profile != config_.profile || candidate.save_kind != config_.save_kind
            || (candidate.flags & ~(flag_checkpoint | flag_promoted)) || !candidate.node) {
        error_ = true; return false;
    }
    record_ = candidate;
    return true;
}

bool RecoveryJournal::digest(uint64_t& size, uint64_t& hash) const {
    if (!std::filesystem::exists(config_.save_path) || std::filesystem::is_symlink(config_.save_path)
            || !std::filesystem::is_regular_file(config_.save_path)) return false;
    std::ifstream input(config_.save_path, std::ios::binary);
    if (!input) return false;
    hash = 1469598103934665603ull; size = 0;
    std::array<char, 4096> buffer{};
    while (input) {
        input.read(buffer.data(), buffer.size());
        const auto count = input.gcount();
        for (std::streamsize i = 0; i < count; ++i) {
            hash ^= uint8_t(buffer[size_t(i)]); hash *= 1099511628211ull;
        }
        size += uint64_t(count);
    }
    return input.eof() && size > 0;
}

bool RecoveryJournal::commit(const Record& record) {
    std::vector<uint8_t> bytes(magic.begin(), magic.end());
    put32(bytes, format); put32(bytes, record.profile); put32(bytes, record.save_kind);
    put32(bytes, record.flags); put32(bytes, record.room); put32(bytes, record.snapshot);
    put64(bytes, record.term); put64(bytes, record.node); put64(bytes, record.save_size);
    put64(bytes, record.save_hash); put32(bytes, crc32(bytes.data(), bytes.size()));
    if (bytes.size() != record_size) return false;
    auto temporary = journal_path_; temporary += ".tmp";
#ifdef _WIN32
    FILE* output = _wfopen(temporary.c_str(), L"wb");
#else
    FILE* output = std::fopen(temporary.c_str(), "wb");
#endif
    if (!output) return false;
    const bool written = std::fwrite(bytes.data(), 1, bytes.size(), output) == bytes.size()
        && sync_file(output);
    const bool closed = std::fclose(output) == 0;
    if (!written || !closed || !replace_file(temporary, journal_path_)) {
        std::error_code ignored; std::filesystem::remove(temporary, ignored); return false;
    }
    record_ = record; error_ = false; save_advanced_ = false;
    return true;
}

void RecoveryJournal::observe(bool checkpoint, bool persist_safe, uint32_t snapshot, uint32_t room) {
    if (!configured_ || error_) return;
    if (!checkpoint) {
        if (record_.flags & flag_checkpoint) {
            Record candidate = record_; candidate.flags &= ~flag_checkpoint;
            if (!commit(candidate)) error_ = true;
        }
        return;
    }
    if (!persist_safe) return;
    if ((record_.flags & flag_checkpoint) && record_.snapshot == snapshot
            && record_.room == room && !save_advanced_) return;
    uint64_t size = 0, hash = 0;
    if (!digest(size, hash)) { error_ = true; return; }
    Record candidate = record_;
    candidate.profile = config_.profile; candidate.save_kind = config_.save_kind;
    candidate.flags |= flag_checkpoint; candidate.room = room; candidate.snapshot = snapshot;
    candidate.save_size = size; candidate.save_hash = hash;
    if (!candidate.node) candidate.node = random_node();
    if (!commit(candidate)) error_ = true;
}

bool RecoveryJournal::promote(uint32_t room) {
    if (!configured_ || error_ || !(record_.flags & flag_checkpoint)) return false;
    Record candidate = record_; candidate.flags |= flag_promoted; candidate.room = room;
    candidate.term = candidate.term == UINT64_MAX ? UINT64_MAX : candidate.term + 1;
    if (!candidate.term) candidate.term = 1;
    if (!commit(candidate)) { error_ = true; return false; }
    return true;
}

uint32_t RecoveryJournal::status() const {
    uint32_t result = configured_ ? COOP_RECOVERY_STORAGE_CONFIGURED : 0;
    if (record_.flags & flag_checkpoint) result |= COOP_RECOVERY_STORAGE_CHECKPOINT;
    if (record_.flags & flag_promoted) result |= COOP_RECOVERY_STORAGE_PROMOTED;
    if (save_advanced_) result |= COOP_RECOVERY_STORAGE_SAVE_ADVANCED;
    if (error_) result |= COOP_RECOVERY_STORAGE_ERROR;
    return result;
}
uint64_t RecoveryJournal::authority_term() const { return record_.term; }
uint64_t RecoveryJournal::node_id() const { return record_.node; }

}
