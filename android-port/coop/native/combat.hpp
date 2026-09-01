#pragma once
#include "../mod/combat_types.h"
#include <array>
#include <cstdint>

namespace dkcoop {
struct State;
bool valid_combat(const CoopCombatFrame& frame);
std::array<uint32_t, COOP_COMBAT_WIRE_WORDS> combat_words(const CoopCombatFrame& frame);
CoopCombatFrame combat_from_words(const std::array<uint32_t, COOP_COMBAT_WIRE_WORDS>& words);

class CombatSync {
public:
    void reset();
    void update(bool host, const State& local, const CoopCombatFrame& input,
        const State& peer, const CoopCombatFrame& remote, bool fresh);
    const CoopCombatFrame& wire() const { return outgoing; }
    const CoopCombatResult& result() const { return output; }
private:
    struct Binding { uint32_t key = 0, local_life = 0, peer_life = 0, kind = 0; };
    struct BossBinding { uint32_t local_life = 0, peer_life = 0, kind = 0; };
    std::array<Binding, COOP_ENEMIES> bindings{};
    BossBinding boss_binding{};
    std::array<uint32_t, 7> context{};
    CoopCombatFrame outgoing{};
    CoopCombatResult output{};
};
}
