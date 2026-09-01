#pragma once
#include "../mod/transient_types.h"
#include <array>
#include <cstdint>

namespace dkcoop {
struct State;

struct TransientWire {
    uint32_t feature = 0, file = 0, map = 0, epoch = 0, revision = 0, count = 0;
    CoopTransientRecord records[COOP_TRANSIENT_RECORDS]{};
};

bool valid_transient(const TransientWire& wire);
bool valid_transient_input(const CoopTransientInput& input);
std::array<uint32_t, COOP_TRANSIENT_WIRE_WORDS> transient_words(const TransientWire& wire);
TransientWire transient_from_words(const std::array<uint32_t, COOP_TRANSIENT_WIRE_WORDS>& words);

class TransientSync {
public:
    void reset();
    void update(bool host, const State& local, const CoopTransientInput& input,
        const State& peer, const TransientWire& remote, bool connected, bool fresh,
        uint64_t session);
    TransientWire wire() const { return outgoing; }
    CoopTransientResult result() const { return output; }
private:
    TransientWire outgoing{};
    CoopTransientResult output{};
    std::array<uint32_t, 6> context{};
};
}
