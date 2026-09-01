#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "progress.hpp"
#include "combat.hpp"
#include "items.hpp"
#include "world.hpp"

namespace dkcoop {
constexpr uint16_t protocol_version = 44;
constexpr uint32_t compatibility = 0x0001012C; // DK64 US 1.0.1 / boss motion contract 44.
constexpr size_t item_offset = 104 + COOP_COMBAT_WIRE_WORDS * 4;
constexpr size_t world_offset = item_offset + COOP_ITEM_WIRE_WORDS * 4;
constexpr size_t transition_offset = world_offset + COOP_WORLD_WIRE_WORDS * 4;
constexpr size_t packet_size = transition_offset + 8;
static_assert(transition_offset == 1192 && packet_size == 1200);
constexpr size_t state_words = 12;
enum class Kind : uint16_t { hello = 1, welcome = 2, state = 3, bye = 4, busy = 5 };
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
};
using Bytes = std::array<uint8_t, packet_size>;
bool valid_state(const State& state);
bool decode(const uint8_t* bytes, size_t size, Packet& output);
Bytes encode(const Packet& packet);
bool newer(uint32_t candidate, uint32_t previous);
std::array<uint32_t, state_words> state_to_words(const State& state);
State state_from_words(const std::array<uint32_t, state_words>& words);
}
