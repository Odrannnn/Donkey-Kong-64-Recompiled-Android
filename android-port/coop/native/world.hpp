#pragma once
#include "../mod/world_types.h"
#include <array>
#include <cstdint>

namespace dkcoop {
struct WorldWire {
    uint32_t feature = 0, file = 0, scope = 0, ready = 0, values = 0, desired = 0;
    uint32_t revision[COOP_WORLD_TOGGLES]{}, request[COOP_WORLD_TOGGLES]{},
        base[COOP_WORLD_TOGGLES]{}, request_values = 0, ack[COOP_WORLD_TOGGLES]{};
};
bool valid_world(const WorldWire& wire);
bool valid_world_input(const CoopWorldInput& input);
std::array<uint32_t, COOP_WORLD_WIRE_WORDS> world_words(const WorldWire& wire);
WorldWire world_from_words(const std::array<uint32_t, COOP_WORLD_WIRE_WORDS>& words);
class WorldSync {
public:
    void reset();
    void update(bool host, const CoopWorldInput& local, const WorldWire& remote,
        bool connected, bool fresh, bool permitted, uint64_t session);
    WorldWire wire() const { return outgoing; }
    CoopWorldResult result() const { return output; }
private:
    WorldWire outgoing{};
    CoopWorldResult output{};
    uint64_t binding = 0;
    uint32_t file = 0, scope = 0, peer_file = 0, last_change[COOP_WORLD_TOGGLES]{}, pending = 0;
    bool initialized = false;
};
}
