#include "../native/recovery_journal.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

static unsigned checks;
#define CHECK(value) do { checks++; if (!(value)) { \
    std::fprintf(stderr, "JOURNAL FAIL line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)

static void write_bytes(const std::filesystem::path& path, const std::string& bytes) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(bytes.data(), std::streamsize(bytes.size()));
}
static void put32(std::vector<unsigned char>& bytes, uint32_t value) {
    for (unsigned shift : {24u, 16u, 8u, 0u}) bytes.push_back(uint8_t(value >> shift));
}
static void put64(std::vector<unsigned char>& bytes, uint64_t value) {
    put32(bytes, uint32_t(value >> 32)); put32(bytes, uint32_t(value));
}
static uint32_t crc32(const std::vector<unsigned char>& bytes) {
    uint32_t crc = 0xFFFFFFFFu;
    for (unsigned char byte : bytes) {
        crc ^= byte;
        for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}

int main() {
    std::random_device random;
    auto root = std::filesystem::temp_directory_path()
        / ("dk64-coop-recovery-" + std::to_string(random()));
    std::filesystem::create_directories(root);
    const auto save = root / "items_guest_v6_campaign2.bin";
    write_bytes(save, "first complete isolated EEPROM image");
    dkcoop::RecoveryJournalConfig config{save, 1, COOP_RECOVERY_SAVE_ITEMS, 123456};

    dkcoop::RecoveryJournal journal;
    CHECK(journal.configure(config) == COOP_RECOVERY_STORAGE_CONFIGURED);
    journal.observe(true, true, 0x10203040u, 123456);
    CHECK((journal.status() & (COOP_RECOVERY_STORAGE_CONFIGURED
        | COOP_RECOVERY_STORAGE_CHECKPOINT | COOP_RECOVERY_STORAGE_ERROR))
        == (COOP_RECOVERY_STORAGE_CONFIGURED | COOP_RECOVERY_STORAGE_CHECKPOINT));
    CHECK(journal.node_id() != 0 && journal.authority_term() == 0);

    dkcoop::RecoveryJournal restored;
    CHECK((restored.configure(config) & COOP_RECOVERY_STORAGE_CHECKPOINT) != 0);
    CHECK(restored.node_id() == journal.node_id());
    restored.observe(false, false, 0, 123456);
    CHECK((restored.status() & COOP_RECOVERY_STORAGE_CHECKPOINT) == 0);
    restored.observe(true, true, 0x10203040u, 123456);
    CHECK((restored.status() & COOP_RECOVERY_STORAGE_CHECKPOINT) != 0);
    CHECK(restored.promote(123456));
    CHECK((restored.status() & COOP_RECOVERY_STORAGE_PROMOTED) != 0
        && restored.authority_term() == 1);

    // A promoted authority survives legitimate later save progress, but the
    // old digest is no longer advertised as a synchronized checkpoint.
    write_bytes(save, "newer complete isolated EEPROM image after promotion");
    dkcoop::RecoveryJournal advanced;
    const uint32_t advanced_status = advanced.configure(config);
    CHECK((advanced_status & COOP_RECOVERY_STORAGE_PROMOTED) != 0);
    CHECK((advanced_status & COOP_RECOVERY_STORAGE_SAVE_ADVANCED) != 0);
    CHECK((advanced_status & COOP_RECOVERY_STORAGE_CHECKPOINT) == 0);
    dkcoop::RecoveryJournal advanced_restart;
    const uint32_t restart_status = advanced_restart.configure(config);
    CHECK((restart_status & COOP_RECOVERY_STORAGE_PROMOTED) != 0);
    CHECK((restart_status & (COOP_RECOVERY_STORAGE_CHECKPOINT
        | COOP_RECOVERY_STORAGE_ERROR)) == 0);
    advanced.observe(true, true, 0x55667788u, 654321);
    CHECK((advanced.status() & (COOP_RECOVERY_STORAGE_CHECKPOINT
        | COOP_RECOVERY_STORAGE_SAVE_ADVANCED | COOP_RECOVERY_STORAGE_ERROR))
        == COOP_RECOVERY_STORAGE_CHECKPOINT);

    // Binding and integrity failures never yield authority or a checkpoint.
    auto sidecar = save; sidecar += ".coop-recovery";
    write_bytes(sidecar, "corrupt");
    dkcoop::RecoveryJournal corrupt;
    CHECK((corrupt.configure(config) & COOP_RECOVERY_STORAGE_ERROR) != 0);
    CHECK((corrupt.status() & (COOP_RECOVERY_STORAGE_CHECKPOINT
        | COOP_RECOVERY_STORAGE_PROMOTED)) == 0);

    // Durable authority is never restored without the corresponding save.
    write_bytes(save, "replacement save for missing-file check");
    std::filesystem::remove(sidecar);
    dkcoop::RecoveryJournal missing_source;
    CHECK(missing_source.configure(config) == COOP_RECOVERY_STORAGE_CONFIGURED);
    missing_source.observe(true, true, 0x10203040u, 123456);
    CHECK(missing_source.promote(123456));
    std::filesystem::remove(save);
    dkcoop::RecoveryJournal missing;
    const uint32_t missing_status = missing.configure(config);
    CHECK((missing_status & COOP_RECOVERY_STORAGE_ERROR) != 0);
    CHECK((missing_status & (COOP_RECOVERY_STORAGE_CHECKPOINT
        | COOP_RECOVERY_STORAGE_PROMOTED)) == 0);

    // Following a newer authority is durable, invalidates the old checkpoint,
    // and makes the next safe promotion advance beyond the observed term.
    std::filesystem::remove(sidecar);
    write_bytes(save, "save synchronized with replacement authority");
    dkcoop::RecoveryJournal follower;
    CHECK(follower.configure(config) == COOP_RECOVERY_STORAGE_CONFIGURED);
    const uint64_t follower_node = follower.node_id();
    const uint64_t leader_node = follower_node == 0xABCDEFu ? 0xABCDEEu : 0xABCDEFu;
    follower.observe(true, true, 0x11112222u, 123456);
    CHECK(follower.follow(7, leader_node));
    CHECK((follower.status() & COOP_RECOVERY_STORAGE_FOLLOWER) != 0);
    CHECK((follower.status() & (COOP_RECOVERY_STORAGE_CHECKPOINT
        | COOP_RECOVERY_STORAGE_PROMOTED | COOP_RECOVERY_STORAGE_ERROR)) == 0);
    dkcoop::RecoveryJournal followed_restart;
    CHECK((followed_restart.configure(config) & COOP_RECOVERY_STORAGE_FOLLOWER) != 0);
    CHECK(followed_restart.authority_term() == 7
        && followed_restart.authority_node() == leader_node
        && followed_restart.node_id() == follower_node);
    CHECK(!followed_restart.promote(123456));
    followed_restart.observe(true, true, 0x33334444u, 123456);
    CHECK(followed_restart.promote(123456));
    CHECK(followed_restart.authority_term() == 8
        && followed_restart.authority_node() == follower_node
        && (followed_restart.status() & COOP_RECOVERY_STORAGE_PROMOTED));

    // A valid v0.52 format-1 identity record is upgraded atomically.
    std::filesystem::remove(sidecar);
    std::vector<unsigned char> legacy{'D','K','6','4','C','R','J','1'};
    put32(legacy, 1); put32(legacy, 1); put32(legacy, COOP_RECOVERY_SAVE_ITEMS);
    put32(legacy, 0); put32(legacy, 123456); put32(legacy, 0);
    put64(legacy, 0); put64(legacy, 0x123456789ULL); put64(legacy, 0); put64(legacy, 0);
    put32(legacy, crc32(legacy));
    CHECK(legacy.size() == 68);
    std::ofstream legacy_output(sidecar, std::ios::binary | std::ios::trunc);
    legacy_output.write(reinterpret_cast<const char*>(legacy.data()), std::streamsize(legacy.size()));
    legacy_output.close();
    dkcoop::RecoveryJournal migrated;
    CHECK(migrated.configure(config) == COOP_RECOVERY_STORAGE_CONFIGURED);
    CHECK(migrated.node_id() == 0x123456789ULL
        && std::filesystem::file_size(sidecar) == 76);

    dkcoop::RecoveryJournal foreign;
    CHECK((foreign.configure({root / "wrong.bin", 1, COOP_RECOVERY_SAVE_ITEMS, 123456})
        & COOP_RECOVERY_STORAGE_ERROR) != 0);

    std::filesystem::remove_all(root);
    std::printf("PASS: %u persistent checkpoint, promotion, binding and corruption checks\n", checks);
}
