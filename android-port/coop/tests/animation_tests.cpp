#include "animation.hpp"
#include "session.hpp"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <chrono>
#include <thread>

// Execute the production pose adapter with a recording engine boundary. This
// checks its load/seek calls and script isolation, not the real game's renderer.
using s16 = short;
using f32 = float;
struct Clip { unsigned char unk12 = 20; };
struct AnimationStateUnk0 { Clip* unk0 = nullptr; short unk10 = -1; };
struct ActorAnimationState {
    AnimationStateUnk0* unk0;
    unsigned unk68 = 42, unk6C = 42, unk74 = 42;
    void (*unk70)() = nullptr;
    float unk48 = 1, unk58 = 1;
};
struct Actor { ActorAnimationState* animation_state; };
#include "../mod/animation_actor.h"
static unsigned checks, loads, seeks;
static int loaded_clip;
static float sought_frame;
static bool full_cache;
static Clip asset;
#define CHECK(expr) do { ++checks; if (!(expr)) { std::fprintf(stderr, "ANIMATION FAIL %d: %s\n", __LINE__, #expr); std::exit(1); } } while (0)
void func_global_asm_80613CA8(Actor* actor, s16 clip, f32 start, f32 blend) {
    ++loads; loaded_clip = clip;
    CHECK(start == 0 && blend == 0);
    auto* track = actor->animation_state->unk0;
    track->unk10 = full_cache ? -1 : clip;
    track->unk0 = clip < 0 || full_cache ? nullptr : &asset;
}
void func_global_asm_80614644(Actor*, AnimationStateUnk0* track, f32 frame) {
    ++seeks; sought_frame = frame;
    CHECK(track->unk0 && frame >= 0 && frame <= track->unk0->unk12 - 1);
}

static void policy_and_actor() {
    std::array<unsigned short, COOP_ANIMATION_ROWS * 7> table;
    table.fill(65535);
    for (unsigned kong = 0; kong < 5; ++kong) {
        table[7 + kong] = (unsigned short)(100 + kong);
        CHECK(coop_animation_clip(table.data(), 110, kong, 2) == int(100 + kong));
        CHECK(coop_animation_find(table.data(), 110, kong, 100 + kong) == 2);
        CHECK(coop_animation_find(table.data(), 110, kong, 100 + (kong + 1) % 5) == 0);
    }
    CHECK(coop_animation_rows(5104, 6644) == 110);
    CHECK(!coop_animation_rows(6644, 5104)); CHECK(!coop_animation_rows(5104, 6646));
    CHECK(!coop_animation_rows(0xfffffff0, 100)); CHECK(!coop_animation_rows(131074, 132614));
    CHECK(coop_animation_clip(nullptr, 110, 0, 2) == -1);
    CHECK(coop_animation_clip(table.data(), 109, 0, 2) == -1);
    CHECK(coop_animation_clip(table.data(), 110, 5, 2) == -1);
    CHECK(coop_animation_clip(table.data(), 110, 0, 0) == -1);
    CHECK(coop_animation_clip(table.data(), 110, 0, 111) == -1);
    CHECK(coop_animation_clip(table.data(), 110, 0, 1) == -1);
    table[0] = 0x4000; CHECK(coop_animation_clip(table.data(), 110, 0, 1) == -1);
    table[0] = 0; CHECK(coop_animation_find(table.data(), 110, 0, 0) == 1);
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    CHECK(!coop_animation_valid_frame(nan));
    CHECK(!coop_animation_valid_frame(std::numeric_limits<float>::infinity()));
    CHECK(!coop_animation_valid_frame(-1)); CHECK(!coop_animation_valid_frame(255));
    CHECK(coop_animation_frame(254, 20) == 19); CHECK(coop_animation_frame(3.5f, 20) == 3.5f);
    CHECK(coop_animation_frame(3, 0) == 0); CHECK(coop_animation_frame(3, 256) == 0);

    CoopPose pose{}; AnimationStateUnk0 track;
    ActorAnimationState animation{&track}; Actor actor{&animation};
    animation.unk70 = [] {};
    coop_pose_apply(&pose, &actor, 100, 3.5f);
    CHECK(loads == 1 && seeks == 1 && loaded_clip == 100 && sought_frame == 3.5f);
    CHECK(!animation.unk68 && !animation.unk6C && !animation.unk70 && !animation.unk74);
    CHECK(animation.unk48 == 0 && animation.unk58 == 0);
    coop_pose_apply(&pose, &actor, 100, 254);
    CHECK(loads == 1 && sought_frame == 19);
    coop_pose_apply(&pose, &actor, 101, 5);
    CHECK(loads == 1); // Clip churn bounded while the current pose stays valid.
    coop_pose_apply(&pose, &actor, 101, 5);
    CHECK(loads == 2 && loaded_clip == 101 && sought_frame == 5);
    coop_pose_apply(&pose, &actor, 101, nan);
    CHECK(loads == 3 && track.unk10 == -1);
    coop_pose_apply(&pose, &actor, -1, 0);
    CHECK(loads == 3);
    full_cache = true; pose.cooldown = 0;
    unsigned before = loads, before_seeks = seeks;
    for (unsigned i = 0; i < 30; ++i) coop_pose_apply(&pose, &actor, 100, 2);
    CHECK(loads - before == 10 && seeks == before_seeks);
    full_cache = false;
    pose = {}; // Same reset used after respawning for Kong/map/session changes.
    coop_pose_apply(&pose, &actor, 103, 9);
    CHECK(track.unk10 == 103 && sought_frame == 9);
    before = loads;
    coop_pose_apply(&pose, nullptr, 103, 9);
    actor.animation_state = nullptr; coop_pose_apply(&pose, &actor, 103, 9);
    CHECK(loads == before);
}

static void timeline() {
    using namespace dkcoop;
    AnimationTimeline t;
    State s{7, 1, 0, active, 0, 0, 0, 0, 2, 0};
    t.receive(s, 1000);
    s.animation_frame = 1.5f; t.receive(s, 1050);
    CHECK(t.frame(1050) == 0); CHECK(t.frame(1075) == 0.75f); CHECK(t.frame(1100) == 1.5f);
    for (unsigned i = 2; i <= 10; ++i) {
        s.animation_frame = float(i) * 1.5f; t.receive(s, 1000 + i * 50);
        CHECK(std::abs(t.frame(1025 + i * 50) - (s.animation_frame - 0.75f)) < 0.001f);
    }
    CHECK(t.frame(5000) == 15); // Lost packets cannot invent ongoing movement/attacks.
    s.animation_frame = 0.5f; t.receive(s, 1550);
    CHECK(t.frame(1550) == 0.5f); // Wrap/restart, no backwards sweep.
    s.animation_frame = 0.25f; t.receive(s, 1600); CHECK(t.frame(1600) == 0.25f);
    s.animation_frame = 1; t.receive(s, 1650);
    t.receive(s, 1700); CHECK(t.frame(1700) == 1); // Pause stays exactly at transmitted frame.
    s.animation_frame = 30; t.receive(s, 1750); CHECK(t.frame(1750) == 30);
    s.animation_frame = 31; t.receive(s, 1950); CHECK(t.frame(1950) == 31); // Long gap snaps.
    uint64_t now = 2000;
    auto reset_check = [&] { s.animation_frame += 1; t.receive(s, now); CHECK(t.frame(now) == s.animation_frame); now += 50; };
    s.animation++; reset_check(); s.character++; reset_check(); s.map++; reset_check(); s.epoch++; reset_check();
    s.flags = 0; reset_check(); s.flags = active; reset_check(); s.flags = active | cutscene; reset_check();
    t.reset(); CHECK(t.frame(now) == 0); s.flags = active; reset_check();
    s.animation = 0; s.animation_frame = 0; t.receive(s, now); CHECK(t.frame(now) == 0);
    s.animation = 111; t.receive(s, now); CHECK(t.frame(now) == 0);
}

static void wire_bounds() {
    using namespace dkcoop;
    State s{7, 1, 4, active, 1, 2, 3, 4095, 110, 254};
    CHECK(valid_state(s));
    Packet p{Kind::state, 1, 2, 3, 123456, s}, decoded;
    auto bytes = encode(p); CHECK(decode(bytes.data(), bytes.size(), decoded));
    CHECK(state_to_words(decoded.player) == state_to_words(s));
    bytes[5] = 2; CHECK(!decode(bytes.data(), bytes.size(), decoded));
    for (auto id : {111u, 0x4000u, 0xffffffffu}) { s.animation = id; CHECK(!valid_state(s)); }
    s.animation = 0; CHECK(!valid_state(s)); s.animation_frame = 0; CHECK(valid_state(s));
    s.animation = 1; s.animation_frame = std::numeric_limits<float>::quiet_NaN(); CHECK(!valid_state(s));
}

static void udp_animation() {
    using namespace dkcoop;
    Session host, guest;
    uint64_t now = 10000;
    CHECK(host.start({Role::host, "", 0, 123456}, now));
    const Config join{Role::join, "127.0.0.1", host.bound_port(), 123456};
    CHECK(guest.start(join, now));
    State h{7, 1, 0, active, 1, 2, 3, 0, 1, 0}, g = h;
    auto pump = [&](unsigned steps) {
        for (unsigned i = 0; i < steps; ++i) {
            now += 10; guest.tick(g, now); host.tick(h, now);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    pump(100); CHECK(host.status() == Status::connected);
    g.animation_frame = 3;
    bool interpolated = false;
    for (unsigned i = 0; i < 20; ++i) {
        pump(1); float f = host.remote(now).animation_frame;
        if (f > 0 && f < 3) interpolated = true;
    }
    CHECK(interpolated && host.remote(now).animation_frame == 3);
    g.animation = 2; g.animation_frame = 20; pump(20);
    CHECK(host.remote(now).animation == 2 && host.remote(now).animation_frame == 20);
    now += 800; CHECK(host.remote(now).flags == 0);
    g.animation_frame = 21; pump(20); CHECK(host.remote(now).animation_frame == 21);
    guest.stop(); pump(20); CHECK(host.remote(now).flags == 0); // Allow the actual UDP BYE to arrive.
    CHECK(guest.start(join, now)); g.character = 4; g.animation_frame = 0.5f;
    pump(100); CHECK(host.remote(now).character == 4 && host.remote(now).animation_frame == 0.5f);
}
int main() {
    policy_and_actor(); timeline(); wire_bounds(); udp_animation();
    std::printf("PASS: %u animation checks (Kong table, real adapter boundary, cache pressure, timeline, resets, wire bounds, live UDP poses)\n", checks);
}
