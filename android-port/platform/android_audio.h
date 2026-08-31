#pragma once
#include <cstddef>
#include <cstdint>

namespace dk64::android_audio {
bool open(uint32_t output_frequency);
void set_frequency(uint32_t frequency);
void queue_samples(const int16_t* samples, size_t count, float volume);
size_t remaining_frames();
// Java lifecycle callbacks only publish state; they never wait on SDL's audio lock.
void set_paused(bool paused);
// Call on the SDL event thread AFTER pumping events (and SDL's backend resume).
void service();
}
