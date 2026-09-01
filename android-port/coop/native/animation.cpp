#include "animation.hpp"
#include <algorithm>
#include <cmath>

namespace dkcoop {
void AnimationTimeline::reset() { *this = {}; }
float AnimationTimeline::frame(uint64_t now) const {
    if (!initialized) return 0;
    if (!duration || now >= received_at + duration) return latest.animation_frame;
    if (now <= received_at) return from;
    const float t = float(now - received_at) / float(duration);
    return from + (latest.animation_frame - from) * t;
}
void AnimationTimeline::receive(const State& state, uint64_t now) {
    if (!valid_state(state)) { reset(); return; }
    const bool same = initialized && now >= received_at && now - received_at <= 150
        && state.map == latest.map && state.epoch == latest.epoch && state.character == latest.character
        && state.flags == latest.flags && (state.flags & active) && !(state.flags & cutscene)
        && state.animation && state.animation == latest.animation;
    const float current = frame(now);
    // Clip changes, pauses, loop wraps, restarts and large frame jumps snap.
    // Backwards playback steps snap too: never interpolate backwards across a wrap.
    const float delta = state.animation_frame - latest.animation_frame;
    const bool continuous = same && delta > 0 && delta <= 4.0f;
    from = continuous ? current : state.animation_frame;
    duration = continuous ? std::clamp(now - received_at, uint64_t(1), uint64_t(50)) : 0;
    latest = state;
    received_at = now;
    initialized = true;
}
}
