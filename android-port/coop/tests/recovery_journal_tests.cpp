#include "../native/recovery_journal.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

static unsigned checks;
#define CHECK(value) do { checks++; if (!(value)) { \
    std::fprintf(stderr, "JOURNAL FAIL line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)

static void write_bytes(const std::filesystem::path& path, const std::string& bytes) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(bytes.data(), std::streamsize(bytes.size()));
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

    dkcoop::RecoveryJournal foreign;
    CHECK((foreign.configure({root / "wrong.bin", 1, COOP_RECOVERY_SAVE_ITEMS, 123456})
        & COOP_RECOVERY_STORAGE_ERROR) != 0);

    std::filesystem::remove_all(root);
    std::printf("PASS: %u persistent checkpoint, promotion, binding and corruption checks\n", checks);
}
