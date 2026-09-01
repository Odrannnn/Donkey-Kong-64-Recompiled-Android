#pragma once
#include "protocol.hpp"

namespace dkcoop {
// Interpolate received poses over one snapshot interval. Never extrapolate an
// attack or loop from an absent packet; a stopped sender produces a held pose.
class AnimationTimeline {
public:
    void reset();
    void receive(const State& state, uint64_t now);
    float frame(uint64_t now) const;
private:
    State latest{};
    float from = 0;
    uint64_t received_at = 0, duration = 0;
    bool initialized = false;
};
}
