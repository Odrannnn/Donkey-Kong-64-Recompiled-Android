#include "session.hpp"
#include <algorithm>
#include <bit>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <thread>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
using namespace dkcoop;
static unsigned checks = 0;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "COMBAT FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)
#include "lossy_relay.hpp"
static unsigned bits(float x) { return std::bit_cast<unsigned>(x); }
static CoopCombatFrame frame(unsigned life) {
    CoopCombatFrame f{1, 1, 0x12345678, 3, {}, {}};
    f.enemies[0] = {17, life, COOP_ENEMY_ALIVE, 0, COOP_BLUE_BEAVER,
        bits(100), bits(200), bits(300), coop_enemy_pack(2048, 5)};
    f.shots[0] = {11, COOP_COCONUT, bits(12.5f), bits(-4.25f), bits(6), 4095, bits(0.75f)};
    f.pages = 1;
    return f;
}
static CoopCombatFrame boss_frame(unsigned life, unsigned phase, unsigned kind = COOP_BOSS_ARMY_DILLO) {
    CoopCombatFrame f{1, 1, 0, 0, {}, {}, {kind, life, 0, phase}};
    f.pages = 1;
    return f;
}
static void protocol_checks() {
    static_assert(COOP_ENEMIES == 20 && COOP_COMBAT_FRAME_WORDS == 246 && COOP_COMBAT_WIRE_WORDS == 204);
    auto f = frame(100); CHECK(valid_combat(f));
    CHECK(coop_enemy_yaw(f.enemies[0]) == 2048 && coop_enemy_health(f.enemies[0]) == 5);
    CHECK(combat_words(combat_from_words(combat_words(f))) == combat_words(f));
    Packet p{Kind::state, 5, 6, 7, 123456, {7, 1, 0, active}, {}, f}, out;
    auto b = encode(p); CHECK(b.size() == 1200 && decode(b.data(), b.size(), out));
    CHECK(combat_words(out.combat) == combat_words(f));
    CHECK(b[107] == 1 && b[112] == 0x12 && b[113] == 0x34 && b[114] == 0x56 && b[115] == 0x78);
    auto bad = [&](CoopCombatFrame input) { CHECK(!valid_combat(input)); p.combat = input; auto raw = encode(p); CHECK(!decode(raw.data(), raw.size(), out)); };
    auto x = f; x.enabled = 4; bad(x); x = f; x.enabled = 0; bad(x);
    x = f; x.file = 0; bad(x); x = f; x.file = 4; bad(x);
    x = f; x.pages = 0; bad(x); x = f; x.pages = COOP_COMBAT_PAGES + 1; bad(x);
    x = f; x.pages = 2; x.page = 2; bad(x);
    x = f; x.hands = 65536; bad(x); x = f; x.layout = 0; bad(x);
    x = f; x.enemies[0].key = 257; bad(x); x = f; x.enemies[0].life = 0; bad(x);
    x = f; x.enemies[0].state = 4; bad(x); x = f; x.enemies[1] = x.enemies[0]; bad(x);
    x = f; x.enemies[0].kind = 0; bad(x); x = f; x.enemies[0].kind = COOP_ENEMY_KIND_COUNT + 1; bad(x);
    x = f; x.enemies[0].yaw = coop_enemy_pack_pose(COOP_ENEMY_YAW_MASK, COOP_ENEMY_HEALTH_MASK, COOP_ENEMY_POSE_MASK);
    CHECK(valid_combat(x)); CHECK(coop_enemy_yaw(x.enemies[0]) == COOP_ENEMY_YAW_MASK
        && coop_enemy_health(x.enemies[0]) == COOP_ENEMY_HEALTH_MASK
        && coop_enemy_pose(x.enemies[0]) == COOP_ENEMY_POSE_MASK);
    x = f; x.enemies[0].life = COOP_ENEMY_LIFE_MASK + 1; bad(x);
    x = f; x.enemies[0].yaw = coop_enemy_pack(0, 0); bad(x);
    x = f; x.enemies[0].state = COOP_ENEMY_DEFEATED; bad(x);
    x = f; x.enemies[0].state = COOP_ENEMY_DEFEATED; x.enemies[0].yaw = 0; CHECK(valid_combat(x));
    x = f; x.enemies[0].key = 0; bad(x);
    x = f; x.shots[1] = x.shots[0]; bad(x); x = f; x.shots[0].kind = 7; bad(x);
    x = f; x.shots[0].id = 0; bad(x); x = f; x.shots[0].yaw = 4096; bad(x);
    x = f; x.shots[0].scale = bits(4.01f); bad(x);
    for (float v : {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), 100001.0f}) {
        x = f; x.shots[0].x = bits(v); bad(x);
        x = f; x.enemies[0].x = bits(v); bad(x);
    }
    auto full = f;
    for (unsigned i = 0; i < COOP_ENEMIES; ++i)
        full.enemies[i] = {i + 1, 1000 + i, COOP_ENEMY_ALIVE, 0,
            i % COOP_ENEMY_KIND_COUNT + 1, bits(float(i)), 0, 0,
            coop_enemy_pack(i * 100 & COOP_ENEMY_YAW_MASK, i + 1)};
    CHECK(valid_combat(full));
    CHECK(combat_words(combat_from_words(combat_words(full))) == combat_words(full));
    auto edge = f;
    edge.enemies[0] = {256, 0x01223344, COOP_ENEMY_REQUEST, 0x55667788,
        COOP_ENEMY_KIND_COUNT, 0, 0, 0, 0};
    CHECK(valid_combat(edge));
    auto edge_words = combat_words(edge);
    CHECK(edge_words[4] == 0x10700u); // Maximum key/state/kind fields do not overlap.
    auto edge_roundtrip = combat_from_words(edge_words);
    CHECK(edge_roundtrip.enemies[0].key == 256
        && edge_roundtrip.enemies[0].state == COOP_ENEMY_REQUEST
        && edge_roundtrip.enemies[0].kind == COOP_ENEMY_KIND_COUNT
        && edge_roundtrip.enemies[0].life == 0x01223344
        && edge_roundtrip.enemies[0].peer_life == 0x55667788);
    p.combat = full; auto compact = encode(p); CHECK(decode(compact.data(), compact.size(), out));
    CHECK(out.combat.enemies[19].key == 20 && out.combat.enemies[19].life == 1019);
    compact[121] |= 0x02; CHECK(!decode(compact.data(), compact.size(), out)); // Kind bit 6 (kind 64) is reserved.
    compact = encode(p);
    compact[120] |= 0x80; CHECK(!decode(compact.data(), compact.size(), out)); // Reserved high identity bit.
    compact = encode(p); compact[904] = 1; CHECK(!decode(compact.data(), compact.size(), out)); // Boss kind without a life token.
    auto boss = boss_frame(700, 3); CHECK(valid_combat(boss));
    CHECK(combat_words(combat_from_words(combat_words(boss))) == combat_words(boss));
    boss = boss_frame(701, 4, COOP_BOSS_DOGADON); CHECK(valid_combat(boss));
    x = boss; x.boss.kind = COOP_BOSS_KIND_COUNT + 1; bad(x);
    x = boss; x.boss.life = 0; bad(x); x = boss; x.boss.phase = 5; bad(x);
    x = boss; x.boss.peer_life = COOP_ENEMY_LIFE_MASK + 1; bad(x);
    p.combat = f; p.kind = Kind::welcome; b = encode(p); CHECK(!decode(b.data(), b.size(), out));
    p.kind = Kind::state; b = encode(p); b[5] = 4; CHECK(!decode(b.data(), b.size(), out));
}
struct Rig {
    CombatSync h, g;
    State hs{7, 1, 0, active}, gs{7, 2, 1, active};
    CoopCombatFrame hi = frame(100), gi = frame(200);
    void tick(unsigned n = 1) {
        for (unsigned i = 0; i < n; ++i) {
            auto hw = h.wire(), gw = g.wire();
            h.update(true, hs, hi, gs, gw, true);
            g.update(false, gs, gi, hs, hw, true);
        }
    }
    void arm() { tick(8); CHECK(h.result().paired == 1 && g.result().paired == 1); }
};
static void authority_checks() {
    Rig r; r.arm();
    r.gi.enemies[0].yaw = coop_enemy_pack(2048, 3); r.tick(3);
    CHECK(r.h.result().apply[0].state == COOP_ENEMY_ALIVE
        && coop_enemy_health(r.h.result().apply[0]) == 3);
    r.hi.enemies[0].yaw = coop_enemy_pack(2048, 3); r.tick(3);
    CHECK(!r.h.result().apply[0].key && coop_enemy_health(r.h.wire().enemies[0]) == 3);
    r.hi.enemies[0].yaw = coop_enemy_pack(2048, 2); r.tick(3);
    CHECK(r.g.result().apply[0].state == COOP_ENEMY_ALIVE
        && coop_enemy_health(r.g.result().apply[0]) == 2);
    r.gi.enemies[0].yaw = coop_enemy_pack(2048, 2); r.tick(3);
    r.gi.enemies[0].yaw = coop_enemy_pack(2048, 4); r.tick(3);
    CHECK(!r.h.result().apply[0].key); // A higher peer value can never heal the host.
    r.gi.enemies[0].yaw = coop_enemy_pack(2048, 2);
    r.gi.enemies[0].state = COOP_ENEMY_DEFEATED; r.gi.enemies[0].yaw = 0; r.tick(4);
    CHECK(r.g.wire().enemies[0].state == COOP_ENEMY_REQUEST);
    CHECK(r.h.wire().enemies[0].state == COOP_ENEMY_ALIVE); // No commit before game readback.
    CHECK(r.h.result().apply[0].key == 17 && r.h.result().apply[0].life == 100);
    for (unsigned i = 0; i < 100; ++i) { r.tick(); CHECK(r.h.wire().enemies[0].state == COOP_ENEMY_ALIVE); }
    r.hi.enemies[0].state = COOP_ENEMY_DEFEATED; r.hi.enemies[0].yaw = 0; r.tick(4);
    CHECK(r.h.wire().enemies[0].state == COOP_ENEMY_DEFEATED
        && r.h.result().apply[0].key == 17 && r.h.result().apply[0].state == COOP_ENEMY_ABSENT);
    r.hi = frame(101); r.tick(8); CHECK(!r.h.result().apply[0].key && !r.h.result().paired); // Old guest kill/new host spawn.
    r.gi = frame(201); r.arm();
    r.hi.enemies[0].state = COOP_ENEMY_DEFEATED; r.hi.enemies[0].yaw = 0; r.tick(4);
    CHECK(r.g.result().apply[0].life == 201); // Host hit commits on the guest.
    r.gi.enemies[0].state = COOP_ENEMY_DEFEATED; r.gi.enemies[0].yaw = 0; r.tick(4);
    CHECK(r.g.result().apply[0].key == 17 && r.g.result().apply[0].state == COOP_ENEMY_ABSENT);
    r.gi = frame(202); r.tick(8); CHECK(!r.g.result().apply[0].key && !r.g.result().paired);

    Rig reorder; reorder.arm();
    std::swap(reorder.hi.enemies[0], reorder.hi.enemies[5]); reorder.tick(2);
    reorder.gi.enemies[0].state = COOP_ENEMY_DEFEATED; reorder.gi.enemies[0].yaw = 0; reorder.tick(3);
    CHECK(reorder.h.result().apply[5].key == 17); // Position in snapshot is not enemy identity.

    Rig forged; forged.arm();
    auto wire = forged.g.wire(); wire.enemies[0].state = COOP_ENEMY_DEFEATED; wire.enemies[0].yaw = 0;
    forged.h.update(true, forged.hs, forged.hi, forged.gs, wire, true);
    CHECK(!forged.h.result().apply[0].key); // Guest cannot label a request as an authoritative commit.
    wire = forged.h.wire(); wire.enemies[0].state = COOP_ENEMY_REQUEST; wire.enemies[0].yaw = 0;
    forged.g.update(false, forged.gs, forged.gi, forged.hs, wire, true);
    CHECK(!forged.g.result().apply[0].key);
    wire = forged.g.wire(); wire.enemies[0].state = COOP_ENEMY_REQUEST; wire.enemies[0].yaw = 0; wire.enemies[0].peer_life++;
    forged.h.update(true, forged.hs, forged.hi, forged.gs, wire, true); CHECK(!forged.h.result().apply[0].key);
}
static void boss_checks() {
    CHECK(coop_boss_kind(8) == COOP_BOSS_ARMY_DILLO
        && coop_boss_kind(196) == COOP_BOSS_ARMY_DILLO
        && coop_boss_kind(83) == COOP_BOSS_DOGADON
        && coop_boss_kind(197) == COOP_BOSS_DOGADON
        && coop_boss_kind(154) == COOP_BOSS_MAD_JACK
        && coop_boss_kind(111) == COOP_BOSS_PUFFTOSS
        && coop_boss_kind(199) == COOP_BOSS_KING_KUT_OUT
        && coop_boss_kind(203) == COOP_BOSS_K_ROOL_DK
        && coop_boss_kind(204) == COOP_BOSS_K_ROOL_DIDDY
        && coop_boss_kind(205) == COOP_BOSS_K_ROOL_LANKY
        && !coop_boss_kind(206) // Tiny's damage is committed by the shoe actor.
        && coop_boss_kind(207) == COOP_BOSS_K_ROOL_CHUNKY
        && coop_boss_kind(214) == COOP_BOSS_K_ROOL_TINY);
    CombatSync host, guest;
    State hs{8, 1, 0, active}, gs{8, 2, 1, active};
    auto hi = boss_frame(700, 0), gi = boss_frame(800, 0);
    auto tick = [&](unsigned count = 1) {
        for (unsigned i = 0; i < count; ++i) {
            auto hw = host.wire(), gw = guest.wire();
            host.update(true, hs, hi, gs, gw, true);
            guest.update(false, gs, gi, hs, hw, true);
        }
    };
    tick(8);
    CHECK(host.result().status == COOP_COMBAT_READY && guest.result().status == COOP_COMBAT_READY);
    CHECK(host.result().paired == 1 && guest.result().paired == 1
        && host.wire().boss.peer_life == 800 && guest.wire().boss.peer_life == 700);
    gi.boss.phase = 1; tick(3);
    CHECK(host.result().boss.kind == COOP_BOSS_ARMY_DILLO && host.result().boss.life == 700
        && host.result().boss.peer_life == 800 && host.result().boss.phase == 1);
    CHECK(!guest.result().boss.kind); // Guest progress is a request until host game readback.
    hi.boss.phase = 1; tick(3); CHECK(!host.result().boss.kind && !guest.result().boss.kind);
    hi.boss.phase = 2; tick(3);
    CHECK(guest.result().boss.kind == COOP_BOSS_ARMY_DILLO && guest.result().boss.life == 800
        && guest.result().boss.phase == 2); // Only host readback commits to the guest.
    gi.boss.phase = 2; tick(3); CHECK(!guest.result().boss.kind);
    gi.boss.phase = 4; tick(3); CHECK(host.result().boss.phase == 4); // Catch up one vanilla transition at a time.
    hi.boss.phase = 3; tick(3); CHECK(host.result().boss.phase == 4);
    hi.boss.phase = 4; tick(3); CHECK(!host.result().boss.kind && !guest.result().boss.kind);
    auto old_host = host.wire(); gi = boss_frame(801, 0);
    guest.update(false, gs, gi, hs, old_host, true);
    CHECK(!guest.result().paired && !guest.result().boss.kind);
    tick(5); CHECK(host.result().paired == 1 && guest.result().paired == 1);
    auto forged = guest.wire(); forged.boss.phase = 5;
    host.update(true, hs, hi, gs, forged, true);
    CHECK(host.result().status == COOP_COMBAT_WAITING && !host.result().boss.kind);
    hs.map = gs.map = 196; hi = boss_frame(900, 0); gi = boss_frame(901, 0); tick(8);
    CHECK(host.result().status == COOP_COMBAT_READY && guest.result().status == COOP_COMBAT_READY);
    hs.map = gs.map = 83;
    hi = boss_frame(1000, 0, COOP_BOSS_DOGADON);
    gi = boss_frame(1001, 0, COOP_BOSS_DOGADON); tick(8);
    CHECK(host.result().status == COOP_COMBAT_READY && guest.result().status == COOP_COMBAT_READY
        && host.result().paired == 1 && guest.result().paired == 1);
    gi.boss.phase = 3; tick(3);
    CHECK(host.result().boss.kind == COOP_BOSS_DOGADON && host.result().boss.phase == 3
        && host.result().boss.life == 1000 && host.result().boss.peer_life == 1001);
    CHECK(!guest.result().boss.kind); // Dogadon also commits only after host readback.
    hi.boss.phase = 3; tick(3); CHECK(!host.result().boss.kind);
    hi.boss.phase = 4; tick(3);
    CHECK(guest.result().boss.kind == COOP_BOSS_DOGADON && guest.result().boss.phase == 4);
    gi.boss.phase = 4; tick(3); CHECK(!guest.result().boss.kind);
    hs.map = gs.map = 197;
    hi = boss_frame(1100, 0, COOP_BOSS_DOGADON);
    gi = boss_frame(1101, 0, COOP_BOSS_DOGADON); tick(8);
    CHECK(host.result().status == COOP_COMBAT_READY && guest.result().status == COOP_COMBAT_READY);
    hi = boss_frame(1200, 0); gi = boss_frame(1201, 0); tick(4);
    CHECK(host.result().status == COOP_COMBAT_SHOTS && !host.result().paired && !host.result().boss.kind);
}
static void context_checks() {
    for (unsigned scenario = 0; scenario < 9; ++scenario) {
        Rig r; r.arm(); auto old = r.g.wire(); old.enemies[0].state = COOP_ENEMY_REQUEST; old.enemies[0].yaw = 0;
        bool fresh = true;
        switch (scenario) {
            case 0: r.hs.epoch++; break;
            case 1: r.gs.epoch++; break;
            case 2: r.hi.file++; break;
            case 3: old.file++; break;
            case 4: old.layout++; break;
            case 5: r.gs.flags = active | cutscene; break;
            case 6: r.hs.map = 34; break;
            case 7: fresh = false; break;
            case 8: r.h.reset(); break;
        }
        r.h.update(true, r.hs, r.hi, r.gs, old, fresh);
        CHECK(!r.h.result().apply[0].key);
    }
    Rig r; r.arm(); r.gi = {}; r.tick(3); CHECK(r.h.result().status == COOP_COMBAT_WAITING);
    r.gi = frame(200); r.arm();
    auto c = r.h.result(); CHECK(c.shots[0].id == 11 && c.hands == 3);
    r.hi.layout++; r.tick(3); CHECK(r.h.result().status == COOP_COMBAT_LAYOUT_MISMATCH);
    CHECK(r.h.result().shots[0].id == 11); // Layout mismatch blocks damage while leaving harmless visuals.

    CoopEnemy command{17, 100, COOP_ENEMY_DEFEATED, 200, COOP_BLUE_BEAVER, 0, 0, 0, 0};
    CHECK(coop_combat_apply_matches(command, 17, 100, 1, 1, 1, 1));
    CHECK(!coop_combat_apply_matches(command, 18, 100, 1, 1, 1, 1));
    CHECK(!coop_combat_apply_matches(command, 17, 101, 1, 1, 1, 1));
    for (unsigned missing = 0; missing < 4; ++missing)
        CHECK(!coop_combat_apply_matches(command, 17, 100, missing != 0, missing != 1, missing != 2, missing != 3));
    CoopEnemy health{17, 100, COOP_ENEMY_ALIVE, 200, COOP_BLUE_BEAVER, 0, 0, 0,
        coop_enemy_pack_pose(0, 2, 10)};
    CHECK(!coop_combat_health_matches(health, 17, 100, 1, 1, 1, 1, 5));
    CHECK(coop_enemy_pose_encode(30.0f, 61) == 16
        && coop_enemy_pose_frame(16, 61) == 30.0f
        && coop_enemy_clip_hash(0x201) < 64);
}
static void movement_checks() {
    const unsigned supported_maps[] = {
        4, 7, 12, 13, 16, 17, 19, 20, 21, 22, 23, 24, 26, 30, 34, 38,
        43, 46, 47, 48, 49, 52, 57, 58, 59, 60, 61, 62, 64, 70, 71, 72,
        84, 85, 86, 87, 88, 91, 92, 93, 94, 95, 105, 108, 112, 113, 114,
        151, 163, 164, 166, 169, 173, 175, 179, 183, 193, 200
    };
    for (unsigned map : supported_maps) for (unsigned kind = 1; kind <= 3; ++kind) {
        Rig r; r.hs.map = r.gs.map = map;
        r.hi.enabled = r.gi.enabled = 2; r.hi.enemies[0].kind = r.gi.enemies[0].kind = kind;
        r.hi.enemies[0].x = bits(75); r.gi.enemies[0].x = bits(-800);
        r.arm();
        CHECK(!r.h.result().motion[0].key); // Guest positions never steer the host.
        const auto m = r.g.result().motion[0];
        CHECK(m.key == 17 && m.life == 200 && m.peer_life == 100 && m.kind == kind && m.x == bits(75));
        CHECK(m.state == COOP_ENEMY_ALIVE && !r.g.result().apply[0].key);
        r.hi.enemies[0].x = bits(90); r.tick(3); CHECK(r.g.result().motion[0].x == bits(90));
        r.gi.enabled = 1; r.tick(3); CHECK(!r.g.result().movement && !r.g.result().motion[0].key);
        r.gi.enabled = 2; r.tick(3); CHECK(r.g.result().movement && r.g.result().motion[0].key);
        r.hi.enemies[0].state = COOP_ENEMY_DEFEATED; r.hi.enemies[0].yaw = 0; r.tick(3);
        CHECK(!r.g.result().motion[0].key && r.g.result().apply[0].kind == kind);
    }
    for (unsigned map : {8u, 10u, 14u, 31u, 53u, 73u, 81u, 82u, 83u, 96u, 109u, 111u, 157u, 196u, 197u, 198u, 201u}) {
        Rig r; r.hs.map = r.gs.map = map; r.hi.enabled = r.gi.enabled = 2; r.tick(8);
        CHECK(!r.g.result().motion[0].key && !r.g.result().paired && r.g.result().status == COOP_COMBAT_SHOTS);
    }
    Rig mismatch; mismatch.hi.enabled = mismatch.gi.enabled = 2; mismatch.arm();
    mismatch.hi.enemies[0].kind = COOP_KREMLING; mismatch.tick(3);
    CHECK(!mismatch.g.result().paired && !mismatch.g.result().motion[0].key);
    mismatch.hi.enemies[0].state = COOP_ENEMY_DEFEATED; mismatch.hi.enemies[0].yaw = 0; mismatch.tick(3);
    CHECK(!mismatch.g.result().apply[0].key); // A different enemy kind cannot reuse a binding.
    Rig stale; stale.hi.enabled = stale.gi.enabled = 2; stale.arm();
    stale.g.update(false, stale.gs, stale.gi, stale.hs, stale.h.wire(), false);
    CHECK(!stale.g.result().motion[0].key);
    Rig respawn; respawn.hi.enabled = respawn.gi.enabled = 2; respawn.arm();
    auto old_host = respawn.h.wire(); respawn.gi.enemies[0].life++;
    respawn.g.update(false, respawn.gs, respawn.gi, respawn.hs, old_host, true);
    CHECK(!respawn.g.result().motion[0].key); // New spawn must receive a new reciprocal acknowledgement.
    Rig pose; pose.hi.enabled = pose.gi.enabled = 3;
    const unsigned hash = coop_enemy_clip_hash(0x201);
    pose.hi.enemies[0].peer_life = hash << COOP_ENEMY_POSE_HASH_SHIFT;
    pose.hi.enemies[0].yaw = coop_enemy_pack_pose(2048, 5, coop_enemy_pose_encode(30.0f, 61));
    pose.arm();
    CHECK(pose.g.result().movement == (COOP_COMBAT_MOVEMENT | COOP_COMBAT_POSE));
    CHECK(coop_enemy_peer_life(pose.g.result().motion[0]) == 100
        && coop_enemy_pose_hash(pose.g.result().motion[0]) == hash
        && coop_enemy_pose(pose.g.result().motion[0]) == 16);
    pose.gi.enabled = 2; pose.tick(3);
    CHECK(pose.g.result().movement == COOP_COMBAT_MOVEMENT
        && pose.g.result().motion[0].key); // Pose requires explicit opt-in on both peers.
}
static void paging_checks() {
    CombatSync host, guest;
    State hs{7, 10, 0, active}, gs{7, 20, 1, active};
    CoopCombatFrame hi[2]{frame(100), frame(101)}, gi[2]{frame(200), frame(201)};
    for (unsigned page = 0; page < 2; ++page) {
        hi[page].page = gi[page].page = page;
        hi[page].pages = gi[page].pages = 2;
        hi[page].enemies[0].key = gi[page].enemies[0].key = page ? 42 : 17;
    }
    for (unsigned round = 0; round < 16; ++round) {
        const unsigned page = round & 1;
        const auto hw = host.wire(page), gw = guest.wire(page);
        host.update(true, hs, hi[page], gs, gw, true);
        guest.update(false, gs, gi[page], hs, hw, true);
    }
    CHECK(coop_enemy_peer_life(host.wire(0).enemies[0]) == 200
        && coop_enemy_peer_life(host.wire(1).enemies[0]) == 201);
    CHECK(coop_enemy_peer_life(guest.wire(0).enemies[0]) == 100
        && coop_enemy_peer_life(guest.wire(1).enemies[0]) == 101);
    gi[1].enemies[0].state = COOP_ENEMY_DEFEATED; gi[1].enemies[0].yaw = 0;
    bool requested = false;
    for (unsigned round = 0; round < 16; ++round) {
        const unsigned page = round & 1;
        const auto hw = host.wire(page), gw = guest.wire(page);
        host.update(true, hs, hi[page], gs, gw, true);
        guest.update(false, gs, gi[page], hs, hw, true);
        if (page == 1 && host.result().apply[0].key == 42
                && host.result().apply[0].state == COOP_ENEMY_DEFEATED) requested = true;
    }
    CHECK(requested);
    auto wire_page = combat_from_words(combat_words(host.wire(1)));
    CHECK(wire_page.page == 1 && wire_page.pages == 2 && wire_page.enemies[0].key == 42);
}
static void live_checks() {
    uint64_t now = 10000; Session host, guest;
    CHECK(host.start({Role::host, "", 0, 123456}, now));
    LossyRelay relay(host.bound_port());
    Config join{Role::join, "127.0.0.1", relay.port, 123456}; CHECK(guest.start(join, now));
    State hs{7, 1, 0, active}, gs{7, 1, 1, active};
    auto hi = frame(100), gi = frame(200);
    hi.enabled = gi.enabled = 2;
    unsigned host_writes = 0, guest_writes = 0; bool apply_host = false;
    auto apply = [](CoopCombatFrame& f, const CoopCombatResult& result, unsigned& writes) {
        for (const auto& command : result.apply) for (auto& enemy : f.enemies)
            if (coop_combat_apply_matches(command, enemy.key, enemy.life, 1, enemy.state == COOP_ENEMY_ALIVE, 1, result.status == COOP_COMBAT_READY)) {
                enemy.state = COOP_ENEMY_DEFEATED; enemy.yaw = 0; ++writes;
            }
    };
    auto pump = [&](unsigned n) {
        for (unsigned i = 0; i < n; ++i) {
            now += 10; relay.forward(); host.tick(hs, now, {}, hi); relay.forward();
            guest.tick(gs, now, {}, gi); relay.forward();
            if (apply_host) apply(hi, host.combat(now), host_writes);
            apply(gi, guest.combat(now), guest_writes);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    pump(200); CHECK(host.combat(now).paired == 1 && guest.combat(now).paired == 1);
    CHECK(guest.combat(now).motion[0].x == hi.enemies[0].x && !host.combat(now).motion[0].key);
    hi.enemies[0].x = bits(321); pump(30); CHECK(guest.combat(now).motion[0].x == bits(321));
    CHECK(!guest.combat(now + 151).motion[0].key && !guest.combat(now + 151).movement);
    CHECK(host.combat(now).shots[0].x == bits(12.5f));
    gi.enemies[0].state = COOP_ENEMY_DEFEATED; gi.enemies[0].yaw = 0; pump(100);
    CHECK(host_writes == 0 && hi.enemies[0].state == COOP_ENEMY_ALIVE);
    CHECK(host.combat(now).apply[0].key == 17);
    apply_host = true; pump(200); CHECK(host_writes == 1 && guest_writes == 0);
    pump(100); CHECK(host_writes == 1); // Duplicate/replayed packets cannot duplicate a defeat.
    hi = frame(101); pump(100); CHECK(host_writes == 1 && hi.enemies[0].state == COOP_ENEMY_ALIVE);
    gi = frame(201); pump(150); CHECK(host.combat(now).paired == 1);
    hi.enemies[0].state = COOP_ENEMY_DEFEATED; hi.enemies[0].yaw = 0; pump(200); CHECK(guest_writes == 1);
    gi.shots[0] = {}; pump(40); CHECK(!host.combat(now).shots[0].id);
    CHECK(!guest.combat(now + 151).shots[0].id); // Stale projectiles hide independently of avatar timeout.
    CHECK(!host.combat(now + 800).apply[0].key && host.combat(now + 800).status == COOP_COMBAT_OFF);
    guest.stop(); pump(100); CHECK(guest.start(join, now)); pump(200);
    CHECK(host_writes == 1 && guest_writes == 1 && !host.combat(now).paired); // No historical-death binding after reconnect.
    CHECK(relay.dropped > 100 && relay.replayed > 100);
    CHECK(host.statistics().rejected + guest.statistics().rejected > 100);
}
int main() {
    protocol_checks(); authority_checks(); boss_checks(); context_checks(); movement_checks(); paging_checks(); live_checks();
    std::printf("PASS: %u combat checks (Army Dillo/Dogadon phases, host movement, enemy kinds/maps, shot bounds, spawn binding, host readback, role validation, stale/respawn guards, real UDP loss/replay)\n", checks);
}
