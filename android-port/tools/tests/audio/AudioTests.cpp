#include "android_audio.h"
#include <SDL.h>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <thread>
#include <vector>

static unsigned checks = 0, queues = 0, clears = 0;
static uint32_t forced_backlog = 0;
static bool simulate_paused_backend = false;
static std::vector<float> last_output;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)

extern "C" int __real_SDL_QueueAudio(SDL_AudioDeviceID, const void*, Uint32);
extern "C" void __real_SDL_ClearQueuedAudio(SDL_AudioDeviceID);
extern "C" Uint32 __real_SDL_GetQueuedAudioSize(SDL_AudioDeviceID);
extern "C" int __wrap_SDL_QueueAudio(SDL_AudioDeviceID device, const void* data, Uint32 bytes) {
    CHECK(!simulate_paused_backend);
    CHECK(bytes % (2 * sizeof(float)) == 0); // No partial samples OR split stereo frames.
    CHECK(bytes <= 48000 * 2 * sizeof(float) / 10); // Never more than 100 ms.
    auto samples = static_cast<const float*>(data);
    for (size_t i = 0; i < bytes / sizeof(float); i++) CHECK(std::isfinite(samples[i]) && std::abs(samples[i]) <= .51f);
    last_output.assign(samples, samples + bytes / sizeof(float));
    queues++;
    return __real_SDL_QueueAudio(device, data, bytes);
}
extern "C" void __wrap_SDL_ClearQueuedAudio(SDL_AudioDeviceID device) {
    CHECK(!simulate_paused_backend); clears++;
    __real_SDL_ClearQueuedAudio(device);
}
extern "C" Uint32 __wrap_SDL_GetQueuedAudioSize(SDL_AudioDeviceID device) {
    CHECK(!simulate_paused_backend);
    return forced_backlog ? forced_backlog : __real_SDL_GetQueuedAudioSize(device);
}

int main() {
    namespace audio = dk64::android_audio;
    CHECK(SDL_setenv("SDL_AUDIODRIVER", "dummy", 1) == 0);
    CHECK(SDL_Init(SDL_INIT_AUDIO) == 0);
    audio::set_paused(false);
    CHECK(audio::open(48000));
    std::vector<int16_t> input(2048);
    for (size_t i = 0; i < input.size(); i += 2) { input[i] = 12000; input[i + 1] = -8000; }
    audio::queue_samples(input.data(), input.size(), 1);
    CHECK(queues == 1);
    CHECK(last_output[last_output.size() - 2] < 0 && last_output.back() > 0); // Channel swap preserved.
    const unsigned initial = queues;
    audio::queue_samples(input.data(), 3, 1);
    audio::queue_samples(nullptr, input.size(), 1);
    audio::queue_samples(input.data(), 0, 1);
    CHECK(queues == initial);
    audio::queue_samples(input.data(), 2, 1); // Shorter than resampling history is safe.
    CHECK(queues == initial + 1);
    for (uint32_t rate : {8000u, 22050u, 32000u, 44100u, 48000u, 96000u}) {
        audio::set_frequency(rate);
        for (size_t frames : {1u, 5u, 137u, 511u, 1001u}) {
            // Includes >3.2 seconds, where the old unbounded shift was undefined,
            // and deliberately misaligned old queues. Fresh audio must remain aligned.
            for (uint32_t backlog : {0u, 38401u, 48000u * 8u * 4u, 0xffffffffu}) {
                forced_backlog = backlog;
                audio::queue_samples(input.data(), frames * 2, 1);
            }
        }
    }
    forced_backlog = 0;
    audio::set_frequency(48000);
    audio::queue_samples(input.data(), input.size(), std::numeric_limits<float>::quiet_NaN());
    for (float sample : last_output) CHECK(std::isfinite(sample));
    for (unsigned cycle = 0; cycle < 50; cycle++) {
        audio::set_paused(true);
        simulate_paused_backend = true;
        auto before = queues, before_clear = clears;
        audio::queue_samples(input.data(), input.size(), 1);
        CHECK(audio::remaining_frames() == 0);
        audio::service();
        audio::set_paused(false);
        audio::queue_samples(input.data(), input.size(), 1); // Resume is not acknowledged yet.
        CHECK(audio::remaining_frames() == 0);
        CHECK(queues == before && clears == before_clear);
        simulate_paused_backend = false; // SDL event pump has resumed the backend.
        audio::service();
        CHECK(clears == before_clear + 1);
        audio::service(); CHECK(clears == before_clear + 1); // Redundant callbacks don't clear live audio.
        audio::queue_samples(input.data(), input.size(), 1);
        CHECK(queues == before + 1);
        CHECK(last_output[0] == 0 && last_output[1] == 0); // Fade from silence, not stale history.
    }
    std::atomic_bool done{false};
    std::thread producer([&] { while (!done.load()) audio::queue_samples(input.data(), input.size(), 1); });
    for (unsigned cycle = 0; cycle < 1000; cycle++) {
        audio::set_paused(true); audio::set_paused(false); audio::service();
    }
    done = true; producer.join();
    audio::set_paused(true);
    SDL_Quit();
    std::printf("PASS: %u checks; real SDL conversion, stereo alignment, backlog bounds, 50 paused-backend cycles and 1000 concurrent resume cycles\n", checks);
}
