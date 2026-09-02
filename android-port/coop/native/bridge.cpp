#include "session.hpp"
#include "recovery_journal.hpp"
#include "recomp.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#define COOP_EXPORT extern "C" __declspec(dllexport)
#else
#define COOP_EXPORT extern "C" __attribute__((visibility("default")))
#endif
COOP_EXPORT uint32_t recomp_api_version = 1;

namespace {
dkcoop::Session session;
dkcoop::RecoveryJournal recovery_journal;
dkcoop::Status previous = dkcoop::Status::off;
bool failed = false;
uint32_t recovery_room = 0;
struct ExtraInput { dkcoop::ProgressInput gate; CoopCombatFrame combat; CoopItemInput items; CoopWorldInput world; CoopTransientInput transient; CoopTraceInput trace; };
struct ExtraResult { dkcoop::ProgressResult gate; CoopCombatResult combat; CoopItemResult items; CoopWorldResult world; CoopTransientResult transient; };
constexpr uint32_t rdram_base = 0x80000000u, rdram_size = 0x20000000u;
bool valid_span(uint32_t address, size_t size, bool aligned = true) {
    return address >= rdram_base && (!aligned || (address & 3) == 0)
        && size <= rdram_size && uint64_t(address - rdram_base) + size <= rdram_size;
}
std::string read_ip(uint8_t* rdram, uint32_t address) {
    if (!valid_span(address, 16, false)) throw std::runtime_error("Invalid IP string address");
    std::string result;
    for (uint32_t i = 0; i < 16; i++) {
        char value = char(rdram[((address - rdram_base) + i) ^ 3]);
        if (!value) return result;
        result.push_back(value);
    }
    throw std::runtime_error("IPv4 string exceeds 15 characters");
}
std::string read_string(uint8_t* rdram, uint32_t address, uint32_t maximum) {
    if (!valid_span(address, size_t(maximum) + 1, false))
        throw std::runtime_error("Invalid string address");
    std::string result;
    for (uint32_t i = 0; i <= maximum; ++i) {
        char value = char(rdram[((address - rdram_base) + i) ^ 3]);
        if (!value) return result;
        result.push_back(value);
    }
    throw std::runtime_error("String exceeds recovery storage limit");
}
void log_status(dkcoop::Status status) {
    if (status != previous) {
        std::fprintf(stdout, "[dk64-coop] Connection status %u\n", unsigned(status));
        std::fflush(stdout); previous = status;
    }
}
}
COOP_EXPORT void dk64_coop_start(uint8_t* rdram, recomp_context* ctx) {
    try {
        failed = false;
        uint32_t role = uint32_t(ctx->r4), port = uint32_t(ctx->r6), room = uint32_t(ctx->r7);
        if (role > 2 || port < 1024 || port > 65535) throw std::runtime_error("Invalid role or port");
        const auto network_role = dkcoop::Role(role);
        const uint64_t authority_node = network_role == dkcoop::Role::host
            ? recovery_journal.node_id() : recovery_journal.authority_node();
        dkcoop::Config config{network_role, role == 2 ? read_ip(rdram, uint32_t(ctx->r5)) : "",
            uint16_t(port), room, recovery_journal.authority_term(), authority_node};
        if (!session.start(config, dkcoop::clock_ms())) std::fprintf(stdout, "[dk64-coop] %s\n", session.error().c_str());
        ctx->r2 = uint32_t(session.status()); log_status(session.status());
    } catch (const std::exception& error) {
        session.stop(); failed = true; ctx->r2 = uint32_t(dkcoop::Status::error);
        std::fprintf(stdout, "[dk64-coop] Start failed: %s\n", error.what());
    }
}
COOP_EXPORT void dk64_coop_local_ipv4(uint8_t*, recomp_context* ctx) {
    ctx->r2 = session.local_ipv4();
}
COOP_EXPORT void dk64_coop_recovery_configure_v64(uint8_t* rdram, recomp_context* ctx) {
    try {
        auto bytes = read_string(rdram, uint32_t(ctx->r4), 4095);
        auto path = std::filesystem::path(std::u8string(bytes.begin(), bytes.end()));
        recovery_room = uint32_t(ctx->r7);
        ctx->r2 = recovery_journal.configure({path, uint32_t(ctx->r5),
            uint32_t(ctx->r6), recovery_room});
    } catch (...) {
        recovery_journal.reset(); recovery_room = 0;
        ctx->r2 = COOP_RECOVERY_STORAGE_ERROR;
    }
}
COOP_EXPORT void dk64_coop_recovery_status_v64(uint8_t*, recomp_context* ctx) {
    ctx->r2 = recovery_journal.status();
}
COOP_EXPORT void dk64_coop_recovery_promote_v64(uint8_t*, recomp_context* ctx) {
    const bool promoted = recovery_journal.promote(recovery_room);
    ctx->r2 = promoted && session.set_authority(recovery_journal.authority_term(),
        recovery_journal.node_id()) ? 1 : 0;
}
// A new export rejects older NRM/library pairs before reading the larger spans.
COOP_EXPORT void dk64_coop_tick_v64(uint8_t* rdram, recomp_context* ctx) {
    uint32_t local_address = uint32_t(ctx->r4), remote_address = uint32_t(ctx->r5);
    uint32_t progress_address = uint32_t(ctx->r6), result_address = uint32_t(ctx->r7);
    constexpr size_t bytes = dkcoop::state_words * sizeof(uint32_t);
    static_assert(sizeof(dkcoop::ProgressInput) == dkcoop::progress_input_words * 4);
    static_assert(sizeof(dkcoop::ProgressResult) == dkcoop::progress_result_words * 4);
    static_assert(sizeof(ExtraInput) == 2868 && sizeof(ExtraResult) == 3476);
    if (!valid_span(local_address, bytes) || !valid_span(remote_address, bytes)
            || !valid_span(progress_address, sizeof(ExtraInput))
            || !valid_span(result_address, sizeof(ExtraResult))) {
        session.stop(); failed = true; ctx->r2 = uint32_t(dkcoop::Status::error); return;
    }
    try {
        std::array<uint32_t, dkcoop::state_words> local{};
        std::memcpy(local.data(), rdram + (local_address - rdram_base), bytes);
        ExtraInput progress{};
        std::memcpy(&progress, rdram + (progress_address - rdram_base), sizeof(progress));
        auto now = dkcoop::clock_ms();
        // The former Japes-only gate experiment is retired. Its fixed-size
        // bridge result slots remain reserved and canonical zero in v46. Four
        // retired wire words now carry the expanded world-state prefix.
        if (!failed) session.tick(dkcoop::state_from_words(local), now, {}, progress.combat,
            progress.items, progress.world, progress.transient, progress.trace);
        bool followed_new_authority = false;
        if (!failed && session.role() == dkcoop::Role::join && session.authority_node()
                && (recovery_journal.authority_term() != session.authority_term()
                    || recovery_journal.authority_node() != session.authority_node())) {
            if (!recovery_journal.follow(session.authority_term(), session.authority_node())) {
                session.stop(); failed = true;
            } else followed_new_authority = true;
        }
        recovery_journal.observe(
            !followed_new_authority
                && (progress.trace.flags & COOP_TRACE_RECOVERY_CHECKPOINT) != 0,
            (progress.trace.flags & COOP_TRACE_RECOVERY_PERSIST_SAFE) != 0,
            progress.trace.recovery_fingerprint, recovery_room);
        auto remote = dkcoop::state_to_words(session.remote(now));
        std::memcpy(rdram + (remote_address - rdram_base), remote.data(), bytes);
        ExtraResult result{{}, session.combat(now), session.items(now), session.world(now), session.transient(now)};
        std::memcpy(rdram + (result_address - rdram_base), &result, sizeof(result));
        auto status = failed ? dkcoop::Status::error : session.status();
        ctx->r2 = uint32_t(status); log_status(status);
    } catch (const std::exception& error) {
        session.stop(); failed = true; ctx->r2 = uint32_t(dkcoop::Status::error);
        std::memset(rdram + (remote_address - rdram_base), 0, bytes);
        std::memset(rdram + (result_address - rdram_base), 0, sizeof(ExtraResult));
        std::fprintf(stdout, "[dk64-coop] Tick failed: %s\n", error.what());
    }
}
COOP_EXPORT void dk64_coop_stop(uint8_t*, recomp_context* ctx) {
    session.stop(); failed = false; ctx->r2 = 0; log_status(dkcoop::Status::off);
}
