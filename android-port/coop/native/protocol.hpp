#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "progress.hpp"
#include "combat.hpp"
#include "items.hpp"
#include "world.hpp"
#include "transient.hpp"

namespace dkcoop {
constexpr uint16_t protocol_version = 55;
constexpr uint32_t compatibility = 0x00010237; // DK64 US ROM / boss animation pose contract 55.
constexpr size_t item_offset = 104 + COOP_COMBAT_WIRE_WORDS * 4;
constexpr size_t world_offset = item_offset + COOP_ITEM_WIRE_WORDS * 4;
// Four new world words reuse the retired Japes-gate wire prefix at bytes
// 80..95. The remaining fifteen stay at the established world offset, so all
// combat/item offsets and the established 1200-byte core remain unchanged.
constexpr size_t world_prefix_offset = 80, world_prefix_words = 4;
constexpr uint32_t world_prefix_marker = 0x574F524Cu; // "WORL" in retired gate value.
constexpr size_t world_suffix_words = COOP_WORLD_WIRE_WORDS - world_prefix_words;
constexpr size_t transition_offset = world_offset + world_suffix_words * 4;
constexpr size_t transient_offset = transition_offset + 8;
constexpr size_t authority_offset = transient_offset + COOP_TRANSIENT_WIRE_WORDS * 4;
constexpr size_t packet_size = authority_offset + 16;
static_assert(COOP_WORLD_WIRE_WORDS == 19 && world_suffix_words == 15
    && transition_offset == 1192 && transient_offset == 1200
    && authority_offset == 1352 && packet_size == 1368);
constexpr size_t state_words = 12;
enum class Kind : uint16_t { hello = 1, welcome = 2, state = 3, bye = 4, busy = 5, authority = 6 };
enum : uint32_t { active = 1, cutscene = 2 };
struct State {
    uint32_t map = 0, epoch = 0, character = 0, flags = 0;
    float x = 0, y = 0, z = 0;
    uint32_t yaw = 0, animation = 0; // Local Kong raw-pose table row + 1, zero = no pose.
    float animation_frame = 0;
    // Appended on the wire to preserve every established progress/combat/item/world offset.
    // A monotonic ticket plus packed reviewed source/destination/signed-exit route.
    uint32_t transition_ticket = 0, transition_route = 0;
};
struct Packet {
    Kind kind = Kind::hello;
    uint32_t sequence = 0;
    uint64_t session = 0, nonce = 0;
    uint32_t room = 0;
    State player{};
    ProgressWire progress{};
    CoopCombatFrame combat{};
    ItemWire items{};
    WorldWire world{};
    TransientWire transient{};
    uint64_t authority_term = 0, authority_node = 0;
};
using Bytes = std::array<uint8_t, packet_size>;
bool valid_state(const State& state);
bool decode(const uint8_t* bytes, size_t size, Packet& output);
Bytes encode(const Packet& packet);
bool newer(uint32_t candidate, uint32_t previous);
bool authority_newer(uint64_t term, uint64_t node, uint64_t known_term, uint64_t known_node);
std::array<uint32_t, state_words> state_to_words(const State& state);
State state_from_words(const std::array<uint32_t, state_words>& words);
}
