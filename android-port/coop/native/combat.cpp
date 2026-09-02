#include "combat.hpp"
#include "protocol.hpp"
#include <bit>
#include <cmath>
#include <type_traits>

namespace dkcoop {
static_assert(sizeof(CoopCombatFrame) == COOP_COMBAT_FRAME_WORDS * 4);
static_assert(sizeof(CoopCombatResult) == COOP_COMBAT_RESULT_WORDS * 4);
static_assert(std::is_trivially_copyable_v<CoopCombatFrame>);
namespace {
constexpr unsigned boss_wire_kind_base = 48;
bool bounded_float(unsigned bits, float min, float max) {
    const float value = std::bit_cast<float>(bits);
    return std::isfinite(value) && value >= min && value <= max;
}
const CoopEnemy* find_enemy(const std::array<std::array<CoopEnemy, COOP_ENEMIES>, COOP_COMBAT_PAGES>& pages,
        unsigned page_count, unsigned key) {
    for (unsigned page = 0; page < page_count; ++page)
        for (const auto& enemy : pages[page]) if (enemy.key == key && key) return &enemy;
    return nullptr;
}
void store_page(std::array<std::array<CoopEnemy, COOP_ENEMIES>, COOP_COMBAT_PAGES>& cache,
        unsigned& page_count, const CoopCombatFrame& frame) {
    if (page_count != frame.pages) { cache = {}; page_count = frame.pages; }
    cache[frame.page] = {};
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
        const auto& incoming = frame.enemies[i];
        if (incoming.key) for (unsigned page = 0; page < page_count; ++page) {
            if (page == frame.page) continue;
            for (auto& cached : cache[page]) if (cached.key == incoming.key) cached = {};
        }
        cache[frame.page][i] = incoming;
    }
}
}
bool valid_combat(const CoopCombatFrame& f) {
    if (f.enabled > 3 || f.file > 3 || f.hands > 65535 || (f.enabled && !f.file)) return false;
    if (!f.enabled && (f.file || f.layout || f.hands || f.page || f.pages)) return false;
    if (f.enabled && (!f.pages || f.pages > COOP_COMBAT_PAGES || f.page >= f.pages)) return false;
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
        const auto& e = f.enemies[i];
        if (!e.key) { if (e.life || e.state || e.peer_life || e.kind || e.x || e.y || e.z || e.yaw) return false; continue; }
        if (!f.enabled || !f.layout || e.key > 256 || !e.life || e.life > COOP_ENEMY_LIFE_MASK
                || e.state < COOP_ENEMY_ALIVE || e.state > COOP_ENEMY_REQUEST) return false;
        const unsigned health = coop_enemy_health(e);
        if (e.kind < COOP_BLUE_BEAVER || e.kind > COOP_ENEMY_KIND_COUNT || (e.yaw & ~COOP_ENEMY_PACK_MASK)
                || (e.state == COOP_ENEMY_ALIVE ? !health : health)
                || !bounded_float(e.x, -100000, 100000) || !bounded_float(e.y, -100000, 100000)
                || !bounded_float(e.z, -100000, 100000)) return false;
        for (unsigned j = 0; j < i; ++j) if (f.enemies[j].key == e.key) return false;
    }
    for (unsigned i = 0; i < COOP_SHOTS; ++i) {
        const auto& s = f.shots[i];
        if (!s.id) { if (s.kind || s.x || s.y || s.z || s.yaw || s.scale) return false; continue; }
        if (!f.enabled || s.kind < COOP_COCONUT || s.kind > COOP_ORANGE || s.yaw >= 4096
            || !bounded_float(s.x, -100000, 100000) || !bounded_float(s.y, -100000, 100000)
            || !bounded_float(s.z, -100000, 100000) || !bounded_float(s.scale, 0.01f, 4.0f)) return false;
        for (unsigned j = 0; j < i; ++j) if (f.shots[j].id == s.id) return false;
    }
    const auto& b = f.boss;
    if (!b.kind) {
        if (b.life || b.peer_life || b.phase) return false;
    } else if (!f.enabled || b.kind > COOP_BOSS_KIND_COUNT || !b.life
            || b.life > COOP_ENEMY_LIFE_MASK || b.peer_life > COOP_ENEMY_LIFE_MASK || b.phase > 4
            || (b.kind == COOP_BOSS_FUNGI_SPIDER && b.phase > 1)) return false;
    const auto& m = f.boss_motion;
    if (!m.kind) {
        if (m.life || m.x || m.y || m.z || m.yaw || m.pose || m.clip_hash) return false;
    } else if (f.enabled < 2 || m.kind != b.kind
            || m.life != b.life || m.yaw >= 4096
            || m.pose > COOP_ENEMY_POSE_MASK || m.clip_hash > COOP_ENEMY_POSE_HASH_MASK
            || (!m.pose && m.clip_hash) || (f.enabled < 3 && m.pose)
            || !bounded_float(m.x, -100000, 100000) || !bounded_float(m.y, -100000, 100000)
            || !bounded_float(m.z, -100000, 100000)) return false;
    // Wire slot zero carries boss motion. Refuse a frame that would silently
    // replace an ordinary record; the game adapter reserves this slot on the
    // only mixed boss/enemy map.
    if (m.kind && f.enemies[0].key) return false;
    return true;
}
std::array<uint32_t, COOP_COMBAT_WIRE_WORDS> combat_words(const CoopCombatFrame& f) {
    std::array<uint32_t, COOP_COMBAT_WIRE_WORDS> w{};
    size_t n = 0;
    w[n++] = f.enabled | (f.page << 8) | (f.pages << 16);
    w[n++] = f.file; w[n++] = f.layout; w[n++] = f.hands;
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
        CoopEnemy e = f.enemies[i];
        const bool boss_slot_collision = !i && f.boss_motion.kind && e.key;
        if (!i && f.boss_motion.kind) e = {1, f.boss_motion.life, COOP_ENEMY_ALIVE,
            f.boss_motion.pose | (f.boss_motion.clip_hash << 5),
            boss_wire_kind_base + f.boss_motion.kind, f.boss_motion.x, f.boss_motion.y,
            f.boss_motion.z, f.boss_motion.yaw};
        uint32_t identity = e.key | (e.state << COOP_ENEMY_STATE_SHIFT) | (e.kind << COOP_ENEMY_KIND_SHIFT);
        if (boss_slot_collision || e.key > 256 || e.state > 3 || e.kind > COOP_ENEMY_KIND_MASK)
            identity |= 0x20000u;
        w[n++] = identity;
        w[n++] = e.life; w[n++] = e.peer_life;
        w[n++] = e.x; w[n++] = e.y; w[n++] = e.z; w[n++] = e.yaw;
    }
    for (const auto& s : f.shots) { w[n++] = s.id; w[n++] = s.kind; w[n++] = s.x; w[n++] = s.y; w[n++] = s.z; w[n++] = s.yaw; w[n++] = s.scale; }
    w[n++] = f.boss.kind; w[n++] = f.boss.life; w[n++] = f.boss.peer_life; w[n++] = f.boss.phase;
    static_assert(4 + COOP_ENEMIES * 7 + COOP_SHOTS * 7 + 4 == COOP_COMBAT_WIRE_WORDS);
    return w;
}
CoopCombatFrame combat_from_words(const std::array<uint32_t, COOP_COMBAT_WIRE_WORDS>& w) {
    CoopCombatFrame f{}; size_t n = 0;
    const uint32_t feature = w[n++];
    f.enabled = feature & 0xFFu; f.page = (feature >> 8) & 0xFFu; f.pages = (feature >> 16) & 0xFFu;
    if (feature & 0xFF000000u) f.enabled = 4; // Fail valid_combat.
    f.file = w[n++]; f.layout = w[n++]; f.hands = w[n++];
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
        auto& e = f.enemies[i];
        const uint32_t identity = w[n++];
        e.key = identity & COOP_ENEMY_KEY_MASK;
        e.state = (identity >> COOP_ENEMY_STATE_SHIFT) & 3u;
        e.kind = (identity >> COOP_ENEMY_KIND_SHIFT) & COOP_ENEMY_KIND_MASK;
        if (identity & ~COOP_ENEMY_IDENTITY_MASK) e.key = 257; // Fail valid_combat.
        e.life = w[n++]; e.peer_life = w[n++];
        e.x = w[n++]; e.y = w[n++]; e.z = w[n++]; e.yaw = w[n++];
        if (!i && e.key == 1 && e.state == COOP_ENEMY_ALIVE
                && e.kind > boss_wire_kind_base && e.kind <= boss_wire_kind_base + COOP_BOSS_KIND_COUNT) {
            f.boss_motion = {e.kind - boss_wire_kind_base, e.life, e.x, e.y, e.z, e.yaw,
                e.peer_life & COOP_ENEMY_POSE_MASK,
                (e.peer_life >> 5) & COOP_ENEMY_POSE_HASH_MASK};
            if (e.peer_life & ~0x7FFu) f.boss_motion.pose = COOP_ENEMY_POSE_MASK + 1;
            e = {};
        }
    }
    for (auto& s : f.shots) { s.id = w[n++]; s.kind = w[n++]; s.x = w[n++]; s.y = w[n++]; s.z = w[n++]; s.yaw = w[n++]; s.scale = w[n++]; }
    f.boss.kind = w[n++]; f.boss.life = w[n++]; f.boss.peer_life = w[n++]; f.boss.phase = w[n++];
    return f;
}
void CombatSync::reset() { *this = {}; }
CoopCombatFrame CombatSync::wire(unsigned page) const {
    if (!outgoing.enabled || !outgoing.pages || page >= outgoing.pages) return {};
    CoopCombatFrame frame = outgoing;
    frame.page = page;
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) frame.enemies[i] = local_pages[page][i];
    for (auto& enemy : frame.enemies) {
        if (!enemy.key) continue;
        enemy.peer_life &= COOP_ENEMY_POSE_HASH_PACK_MASK;
        const auto& binding = bindings[enemy.key - 1];
        if (binding.key == enemy.key && binding.local_life == enemy.life && binding.kind == enemy.kind)
            enemy.peer_life |= binding.peer_life;
        if (!local_host && enemy.state == COOP_ENEMY_DEFEATED) {
            const auto* peer = find_enemy(remote_pages, remote_page_count, enemy.key);
            if (!peer || peer->state != COOP_ENEMY_DEFEATED
                    || coop_enemy_peer_life(*peer) != enemy.life)
                enemy.state = COOP_ENEMY_REQUEST;
        }
    }
    return frame;
}
void CombatSync::update(bool host, const State& local, const CoopCombatFrame& input,
        const State& peer, const CoopCombatFrame& remote, bool fresh) {
    local_host = host;
    outgoing = valid_combat(input) ? input : CoopCombatFrame{};
    output = {};
    for (auto& e : outgoing.enemies) {
        // Preserve only locally captured visual metadata. The bridge owns the
        // low reciprocal-life bits and all request/commit roles.
        e.peer_life &= COOP_ENEMY_POSE_HASH_PACK_MASK;
        if (e.state == COOP_ENEMY_REQUEST) { outgoing = {}; break; } // Not a game readback state.
    }
    outgoing.boss.peer_life = 0; // The bridge owns reciprocal binding tokens.
    if (!outgoing.enabled) {
        bindings = {}; boss_binding = {}; context = {}; local_pages = {}; remote_pages = {};
        local_page_count = remote_page_count = 0; return;
    }
    store_page(local_pages, local_page_count, outgoing);
    output.status = COOP_COMBAT_WAITING;
    if (!fresh || !valid_combat(remote) || !remote.enabled || !valid_state(local) || !valid_state(peer)
        || local.flags != active || peer.flags != active || local.map != peer.map) {
        bindings = {}; boss_binding = {}; context = {}; remote_pages = {}; remote_page_count = 0; return;
    }
    output.status = COOP_COMBAT_SHOTS;
    output.hands = remote.hands;
    for (unsigned i = 0; i < COOP_SHOTS; ++i) output.shots[i] = remote.shots[i];
    const std::array<uint32_t, 7> next{local.epoch, peer.epoch, outgoing.file, remote.file, outgoing.layout, remote.layout, local.map};
    if (context != next) {
        bindings = {}; boss_binding = {}; local_pages = {}; remote_pages = {};
        local_page_count = remote_page_count = 0; context = next;
        store_page(local_pages, local_page_count, outgoing);
    }
    store_page(remote_pages, remote_page_count, remote);
    const unsigned expected_boss = coop_boss_kind(local.map);
    if (expected_boss && outgoing.boss.kind == expected_boss
            && remote.boss.kind == expected_boss) {
        if (boss_binding.local_life != outgoing.boss.life || boss_binding.peer_life != remote.boss.life
                || boss_binding.kind != outgoing.boss.kind)
            boss_binding = {outgoing.boss.life, remote.boss.life, outgoing.boss.kind};
        outgoing.boss.peer_life = boss_binding.peer_life;
        if (remote.boss.peer_life == outgoing.boss.life) {
            output.status = COOP_COMBAT_READY;
            ++output.paired;
            if (outgoing.enabled >= 2 && remote.enabled >= 2) {
                output.movement |= COOP_COMBAT_MOVEMENT;
                if (outgoing.enabled == 3 && remote.enabled == 3)
                    output.movement |= COOP_COMBAT_POSE;
                if (!host && remote.boss_motion.kind == expected_boss
                        && remote.boss_motion.life == remote.boss.life) {
                    output.boss_motion = remote.boss_motion;
                    output.boss_motion.life = outgoing.boss.life;
                }
            }
            const unsigned target = host
                ? (outgoing.boss.phase > remote.boss.phase ? outgoing.boss.phase : remote.boss.phase)
                : remote.boss.phase;
            if (target > outgoing.boss.phase)
                output.boss = {outgoing.boss.kind, outgoing.boss.life, boss_binding.peer_life, target};
        }
    } else boss_binding = {};
    if (!coop_combat_map(local.map) || !outgoing.layout || !remote.layout) { bindings = {}; return; }
    if (outgoing.layout != remote.layout) { bindings = {}; output.status = COOP_COMBAT_LAYOUT_MISMATCH; return; }
    output.status = COOP_COMBAT_READY;
    if (outgoing.enabled >= 2 && remote.enabled >= 2) output.movement |= COOP_COMBAT_MOVEMENT;
    if (outgoing.enabled == 3 && remote.enabled == 3) output.movement |= COOP_COMBAT_POSE;
    for (unsigned i = 0; i < COOP_ENEMIES; ++i) {
        auto& e = outgoing.enemies[i];
        if (!e.key) continue;
        auto& b = bindings[e.key - 1];
        const auto* p = find_enemy(remote_pages, remote_page_count, e.key);
        if (!p || e.kind != p->kind) { b = {}; continue; }
        if (b.key != e.key || b.local_life != e.life || b.peer_life != p->life) {
            b = {};
            // Both copies must have been seen alive in this session. Never bind
            // a historical defeat to a new/respawned or late-joining actor.
            if (e.state == COOP_ENEMY_ALIVE && p->state == COOP_ENEMY_ALIVE)
                b = {e.key, e.life, p->life, e.kind};
        }
        if (!b.key) continue;
        e.peer_life = (e.peer_life & COOP_ENEMY_POSE_HASH_PACK_MASK) | b.peer_life;
        if (coop_enemy_peer_life(*p) != e.life) continue; // Reciprocal spawn acknowledgement.
        ++output.paired;
        const bool local_defeated = input.enemies[i].key == e.key
            && input.enemies[i].life == e.life
            && input.enemies[i].state == COOP_ENEMY_DEFEATED;
        if (local_defeated && p->state == COOP_ENEMY_DEFEATED) {
            if (!host) e.state = COOP_ENEMY_DEFEATED;
            output.apply[i] = {e.key, e.life, COOP_ENEMY_ABSENT, p->life, e.kind, 0, 0, 0, 0};
            continue;
        }
        if (e.state == COOP_ENEMY_ALIVE && p->state == COOP_ENEMY_ALIVE) {
            const unsigned local_health = coop_enemy_health(e);
            const unsigned peer_health = coop_enemy_health(*p);
            const unsigned target_health = host
                ? (local_health < peer_health ? local_health : peer_health)
                : peer_health;
            // Health only converges downward. The host accepts a guest's lower
            // readback, then both peers wait for the game adapter to publish
            // that exact target before it becomes authoritative wire state.
            if (target_health && target_health < local_health)
                output.apply[i] = {e.key, e.life, COOP_ENEMY_ALIVE, p->life, e.kind,
                    0, 0, 0, coop_enemy_pack(0, target_health)};
        }
        if (!host && output.movement && e.state == COOP_ENEMY_ALIVE && p->state == COOP_ENEMY_ALIVE) {
            output.motion[i] = *p;
            output.motion[i].life = e.life;
            output.motion[i].peer_life = (p->peer_life & COOP_ENEMY_POSE_HASH_PACK_MASK) | p->life;
        }
        if (e.state == COOP_ENEMY_ALIVE && p->state == (host ? COOP_ENEMY_REQUEST : COOP_ENEMY_DEFEATED)) {
            // On host: a guest's confirmed local hit requests a defeat. On guest:
            // only the host's actual readback can commit one. Repeating this
            // command is safe; the game checks the same local actor life.
            output.apply[i] = {e.key, e.life, COOP_ENEMY_DEFEATED, p->life, e.kind, 0, 0, 0, 0};
        }
    }
    outgoing = wire(input.page);
}
}
