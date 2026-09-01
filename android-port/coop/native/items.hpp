#pragma once
#include "../mod/items_policy.h"
#include <array>
#include <cstdint>
namespace dkcoop {
struct ItemWire { uint32_t feature = 0, file = 0, ready = 0, scope = 0, owned[COOP_ITEM_WORDS]{}, request[COOP_ITEM_WORDS]{}; uint32_t page = 0; };
bool valid_items(const ItemWire& w);
bool valid_items_input(const CoopItemInput& l);
std::array<uint32_t, COOP_ITEM_WIRE_WORDS> item_words(const ItemWire& w);
ItemWire items_from_words(const std::array<uint32_t, COOP_ITEM_WIRE_WORDS>& w);
ItemWire items_wire(bool host, const CoopItemInput& l, const ItemWire& r, bool connected, uint64_t session);
CoopItemResult items_result(bool host, const CoopItemInput& l, const ItemWire& r,
    bool connected, bool fresh, uint64_t session);
}
