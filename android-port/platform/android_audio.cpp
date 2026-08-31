#include "android_audio.h"
#include <SDL.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <mutex>
#include <vector>

namespace dk64::android_audio {
namespace {
constexpr size_t channels = 2, history_frames = 4, frame_bytes = channels * sizeof(float);
constexpr uint32_t max_queue_ms = 100, fade_ms = 10;
std::mutex audio_mutex;
// Low bit: paused. Other bits: generation, so a quick pause/resume cannot be lost.
std::atomic<uint64_t> lifecycle{1};
uint64_t applied_lifecycle = 1;
SDL_AudioDeviceID device = 0;
SDL_AudioCVT converter{};
uint32_t input_rate = 48000, output_rate = 48000;
size_t discard_frames = history_frames, fade_remaining = 0;
std::array<float, history_frames * channels> history{};
std::vector<float> buffer;

bool ready() {
    const auto state = lifecycle.load(std::memory_order_acquire);
    return device != 0 && (state & 1) == 0 && state == applied_lifecycle;
}
bool build_converter(uint32_t input, uint32_t output) {
    SDL_AudioCVT next{};
    if (SDL_BuildAudioCVT(&next, AUDIO_F32SYS, channels, input, AUDIO_F32SYS, channels, output) < 0) {
        std::fprintf(stderr, "[DK64Audio] Cannot build converter: %s\n", SDL_GetError());
        return false;
    }
    converter = next;
    input_rate = input; output_rate = output;
    discard_frames = history_frames * output_rate / input_rate;
    history.fill(0);
    return true;
}
void reset_queue() {
    SDL_ClearQueuedAudio(device);
    history.fill(0);
    fade_remaining = output_rate * fade_ms / 1000;
}
}

void set_paused(bool paused) {
    auto previous = lifecycle.load(std::memory_order_relaxed);
    while (bool(previous & 1) != paused) {
        const auto next = ((previous + 2) & ~uint64_t(1)) | uint64_t(paused);
        if (lifecycle.compare_exchange_weak(previous, next, std::memory_order_release, std::memory_order_relaxed)) return;
    }
}

bool open(uint32_t frequency) {
    std::lock_guard lock(audio_mutex);
    if (frequency < 8000 || frequency > 192000 || device != 0) return false;
    SDL_AudioSpec desired{};
    desired.freq = int(frequency); desired.format = AUDIO_F32SYS;
    desired.channels = channels; desired.samples = 0x100;
    device = SDL_OpenAudioDevice(nullptr, 0, &desired, nullptr, 0);
    if (!device) return false;
    if (!build_converter(input_rate, frequency)) {
        SDL_CloseAudioDevice(device); device = 0; return false;
    }
    applied_lifecycle = lifecycle.load(std::memory_order_acquire);
    fade_remaining = output_rate * fade_ms / 1000;
    SDL_PauseAudioDevice(device, int(applied_lifecycle & 1));
    std::printf("[DK64Audio] Opened %s, %u Hz stereo float; frame-aligned queue\n", SDL_GetCurrentAudioDriver(), output_rate);
    std::fflush(stdout);
    return true;
}

void service() {
    // SDL holds its mixer lock for the duration of an Android pause. Never call
    // SDL audio APIs from Java or while the requested lifecycle is inactive.
    if (lifecycle.load(std::memory_order_acquire) & 1) return;
    std::lock_guard lock(audio_mutex);
    const auto state = lifecycle.load(std::memory_order_acquire);
    if (!device || (state & 1) || state == applied_lifecycle) return;
    SDL_PauseAudioDevice(device, 1);
    reset_queue();
    applied_lifecycle = state;
    SDL_PauseAudioDevice(device, 0);
    std::printf("[DK64Audio] Foreground audio reset (generation %llu)\n", static_cast<unsigned long long>(state >> 1));
    std::fflush(stdout);
}

void set_frequency(uint32_t frequency) {
    std::lock_guard lock(audio_mutex);
    if (frequency < 8000 || frequency > 192000 || frequency == input_rate) return;
    build_converter(frequency, output_rate);
}

void queue_samples(const int16_t* samples, size_t count, float volume) {
    if ((lifecycle.load(std::memory_order_acquire) & 1) || !samples || !count || count % channels || count > 192000) return;
    std::lock_guard lock(audio_mutex);
    if (!ready()) return; // No pre-resume samples can refill the queue before it is reset.
    const size_t total = count + history.size();
    buffer.resize(total * size_t(std::max(1, converter.len_mult)));
    std::copy(history.begin(), history.end(), buffer.begin());
    const float gain = (std::isfinite(volume) ? std::clamp(volume, 0.0f, 1.0f) : 0.0f) * (0.5f / 32768.0f);
    for (size_t i = 0; i < count; i += channels) {
        // Emulated RDRAM's address xor reverses the two 16-bit channels.
        buffer[history.size() + i] = samples[i + 1] * gain;
        buffer[history.size() + i + 1] = samples[i] * gain;
    }
    // Also valid for chunks shorter than the interpolation history.
    std::copy_n(buffer.data() + count, history.size(), history.begin());
    converter.buf = reinterpret_cast<Uint8*>(buffer.data());
    converter.len = int(total * sizeof(float));
    if (SDL_ConvertAudio(&converter) < 0) {
        std::fprintf(stderr, "[DK64Audio] Conversion failed: %s\n", SDL_GetError()); return;
    }
    const size_t converted_frames = size_t(converter.len_cvt) / frame_bytes;
    if (converted_frames <= discard_frames) return;
    size_t frames = converted_frames - discard_frames;
    float* output = buffer.data() + (discard_frames / 2) * channels;
    const size_t max_frames = output_rate * max_queue_ms / 1000;
    const uint32_t queued_bytes = SDL_GetQueuedAudioSize(device);
    if (queued_bytes % frame_bytes || queued_bytes / frame_bytes + frames > max_frames) {
        // Drop stale complete frames instead of dividing a byte count or shifting
        // by an unbounded backlog. A partial float/frame would corrupt all later audio.
        reset_queue();
        if (frames > max_frames) { output += (frames - max_frames) * channels; frames = max_frames; }
    }
    const size_t fade_total = output_rate * fade_ms / 1000;
    const size_t fade_frames = std::min(frames, fade_remaining);
    for (size_t i = 0; i < fade_frames; i++) {
        const float fade = float(fade_total - fade_remaining + i) / float(fade_total);
        output[i * channels] *= fade; output[i * channels + 1] *= fade;
    }
    fade_remaining -= fade_frames;
    if (SDL_QueueAudio(device, output, uint32_t(frames * frame_bytes)) < 0)
        std::fprintf(stderr, "[DK64Audio] Queue failed: %s\n", SDL_GetError());
}

size_t remaining_frames() {
    if (lifecycle.load(std::memory_order_acquire) & 1) return 0;
    std::lock_guard lock(audio_mutex);
    if (!ready()) return 0;
    const uint64_t output_frames = SDL_GetQueuedAudioSize(device) / frame_bytes;
    const size_t input_frames = size_t(output_frames * input_rate / output_rate);
    const size_t one_vi = input_rate / 60;
    return input_frames > one_vi ? input_frames - one_vi : 0;
}
}
