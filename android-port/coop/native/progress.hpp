#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace dkcoop {
// Feature 1 is one monotonic event: Japes' first gate. Never a raw game flag ID.
constexpr uint32_t japes_gate_feature = 1, japes_map = 7;
constexpr size_t progress_input_words = 8, progress_result_words = 6;
struct ProgressWire {
    uint32_t feature = 0, file = 0, ready = 0, scope = 0, value = 0, ack = 0;
};
struct ProgressInput {
    uint32_t enabled = 0, file = 0, ready = 0, gate = 0, request = 0;
    uint32_t seen_session_hi = 0, seen_session_lo = 0, seen_scope = 0;
};
enum class ProgressStatus : uint32_t { off, waiting, ready, committed, conflict };
struct ProgressResult {
    uint32_t status = 0, apply = 0, acknowledged = 0, session_hi = 0, session_lo = 0, scope = 0;
};
bool valid_progress(const ProgressWire& wire);
bool valid_progress_input(const ProgressInput& input);
ProgressWire progress_wire(bool host, const ProgressInput& local, const ProgressWire& remote,
    bool connected, uint64_t session);
ProgressResult progress_result(bool host, const ProgressInput& local, const ProgressWire& remote,
    bool connected, uint64_t session);
}
