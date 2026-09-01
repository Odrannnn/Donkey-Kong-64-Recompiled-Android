#include "protocol.hpp"
#include "../mod/animation_policy.h"
#include "../mod/transition_policy.h"
#include <bit>
#include <cmath>

namespace dkcoop {
namespace {
void put16(uint8_t* b, uint16_t v) { b[0] = uint8_t(v >> 8); b[1] = uint8_t(v); }
void put32(uint8_t* b, uint32_t v) { put16(b, uint16_t(v >> 16)); put16(b + 2, uint16_t(v)); }
void put64(uint8_t* b, uint64_t v) { put32(b, uint32_t(v >> 32)); put32(b + 4, uint32_t(v)); }
uint16_t get16(const uint8_t* b) { return uint16_t((uint16_t(b[0]) << 8) | b[1]); }
uint32_t get32(const uint8_t* b) { return (uint32_t(get16(b)) << 16) | get16(b + 2); }
uint64_t get64(const uint8_t* b) { return (uint64_t(get32(b)) << 32) | get32(b + 4); }
}
bool valid_state(const State& s) {
    return s.map <= 255 && s.character < 5 && (s.flags & ~(active | cutscene)) == 0
        && s.yaw < 4096 && s.animation <= COOP_ANIMATION_ROWS && coop_animation_valid_frame(s.animation_frame)
        && (s.animation != 0 || s.animation_frame == 0)
        && std::isfinite(s.x) && std::isfinite(s.y) && std::isfinite(s.z)
        && std::abs(s.x) <= 100000 && std::abs(s.y) <= 100000 && std::abs(s.z) <= 100000
        && coop_transition_valid(s.transition_ticket, s.transition_route);
}
std::array<uint32_t, state_words> state_to_words(const State& s) {
    return {s.map, s.epoch, s.character, s.flags, std::bit_cast<uint32_t>(s.x),
        std::bit_cast<uint32_t>(s.y), std::bit_cast<uint32_t>(s.z), s.yaw, s.animation,
        std::bit_cast<uint32_t>(s.animation_frame), s.transition_ticket, s.transition_route};
}
State state_from_words(const std::array<uint32_t, state_words>& w) {
    return {w[0], w[1], w[2], w[3], std::bit_cast<float>(w[4]), std::bit_cast<float>(w[5]),
        std::bit_cast<float>(w[6]), w[7], w[8], std::bit_cast<float>(w[9]), w[10], w[11]};
}
Bytes encode(const Packet& p) {
    Bytes b{};
    put32(b.data(), 0x444B4350); // DKCP
    put16(b.data() + 4, protocol_version); put16(b.data() + 6, uint16_t(p.kind));
    put32(b.data() + 8, compatibility); put32(b.data() + 12, p.sequence);
    put64(b.data() + 16, p.session); put64(b.data() + 24, p.nonce);
    put32(b.data() + 32, p.room);
    auto words = state_to_words(p.player);
    for (size_t i = 0; i < 10; i++) put32(b.data() + 40 + 4 * i, words[i]);
    const auto& g = p.progress;
    std::array<uint32_t, 6> progress{g.feature, g.file, g.ready, g.scope, g.value, g.ack};
    for (size_t i = 0; i < progress.size(); i++) put32(b.data() + 80 + 4 * i, progress[i]);
    auto combat = combat_words(p.combat);
    for (size_t i = 0; i < combat.size(); ++i) put32(b.data() + 104 + 4 * i, combat[i]);
    auto items = item_words(p.items);
    for (size_t i = 0; i < items.size(); ++i) put32(b.data() + item_offset + 4 * i, items[i]);
    auto world = world_words(p.world);
    for (size_t i = 0; i < world.size(); ++i) put32(b.data() + world_offset + 4 * i, world[i]);
    put32(b.data() + transition_offset, words[10]);
    put32(b.data() + transition_offset + 4, words[11]);
    return b;
}
bool decode(const uint8_t* b, size_t size, Packet& output) {
    if (size != packet_size || get32(b) != 0x444B4350 || get16(b + 4) != protocol_version
            || get32(b + 8) != compatibility || get32(b + 36) != 0) return false;
    uint16_t kind = get16(b + 6);
    if (kind < uint16_t(Kind::hello) || kind > uint16_t(Kind::busy)) return false;
    Packet p;
    p.kind = Kind(kind); p.sequence = get32(b + 12); p.session = get64(b + 16);
    p.nonce = get64(b + 24); p.room = get32(b + 32);
    std::array<uint32_t, state_words> words{};
    for (size_t i = 0; i < 10; i++) words[i] = get32(b + 40 + 4 * i);
    words[10] = get32(b + transition_offset);
    words[11] = get32(b + transition_offset + 4);
    p.player = state_from_words(words);
    p.progress = {get32(b + 80), get32(b + 84), get32(b + 88), get32(b + 92), get32(b + 96), get32(b + 100)};
    std::array<uint32_t, COOP_COMBAT_WIRE_WORDS> combat{};
    for (size_t i = 0; i < combat.size(); ++i) combat[i] = get32(b + 104 + 4 * i);
    p.combat = combat_from_words(combat);
    std::array<uint32_t, COOP_ITEM_WIRE_WORDS> items{};
    for (size_t i = 0; i < items.size(); ++i) items[i] = get32(b + item_offset + 4 * i);
    p.items = items_from_words(items);
    std::array<uint32_t, COOP_WORLD_WIRE_WORDS> world{};
    for (size_t i = 0; i < world.size(); ++i) world[i] = get32(b + world_offset + 4 * i);
    p.world = world_from_words(world);
    if (!valid_world(p.world) || (p.kind != Kind::state && p.world.feature)) return false;
    if (p.world.feature && (!p.items.feature || p.world.file != p.items.file
            || (p.world.ready && !p.items.ready))) return false;
    if (!valid_items(p.items) || (p.kind != Kind::state && p.items.feature)) return false;
    if (!valid_combat(p.combat) || (p.kind != Kind::state && p.combat.enabled)) return false;
    if (!valid_progress(p.progress) || (p.kind != Kind::state && p.progress.feature)) return false;
    if (p.kind != Kind::state && (p.player.transition_ticket || p.player.transition_route)) return false;
    if (p.room < 100000 || p.room > 999999 || p.nonce == 0 || !valid_state(p.player)) return false;
    if (p.kind == Kind::hello || p.kind == Kind::busy) {
        if (p.session != 0) return false;
    } else if (p.session == 0) return false;
    output = p;
    return true;
}
bool newer(uint32_t candidate, uint32_t previous) {
    uint32_t difference = candidate - previous;
    return difference != 0 && difference < 0x80000000u;
}
}
