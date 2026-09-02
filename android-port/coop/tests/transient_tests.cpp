#include "protocol.hpp"
#include "transient.hpp"
#include <cstdio>

using namespace dkcoop;
static unsigned checks;
#define CHECK(expr) do { ++checks; if (!(expr)) { std::fprintf(stderr, "TRANSIENT FAIL %d: %s\n", __LINE__, #expr); return 1; } } while (0)

static CoopTransientInput frame(unsigned map, unsigned epoch, unsigned revision) {
    CoopTransientInput input{};
    input.enabled = 1; input.file = 0; input.map = map; input.epoch = epoch;
    input.revision = revision; input.count = 5;
    input.records[0] = {COOP_TRANSIENT_SCRIPT, 0x34, 20, 0};
    input.records[1] = {COOP_TRANSIENT_TIMER, 0x25, 3, 240};
    input.records[2] = {COOP_TRANSIENT_PLATFORM, 0x27, 4, 0x12345678};
    input.records[3] = {COOP_TRANSIENT_TRIGGER, 0x31, 2, 2};
    input.records[4] = {COOP_TRANSIENT_SEQUENCE, 0x14, 8, 0};
    return input;
}

int main() {
    auto host_input = frame(7, 10, 4);
    CHECK(valid_transient_input(host_input));
    TransientWire wire{host_input.enabled, host_input.file, host_input.map, host_input.epoch,
        host_input.revision, host_input.count, {}};
    for (unsigned i = 0; i < COOP_TRANSIENT_RECORDS; ++i) wire.records[i] = host_input.records[i];
    auto decoded = transient_from_words(transient_words(wire));
    CHECK(valid_transient(decoded) && decoded.records[2].value == 0x12345678);
    auto zero_object = decoded; zero_object.records[3].key = 0;
    CHECK(valid_transient(zero_object)); // Model-two object ID zero is not an empty record.
    auto state_zero_trigger = decoded; state_zero_trigger.records[3].value = 1;
    CHECK(valid_transient(state_zero_trigger)); // Factory grate uses activation state 1.
    state_zero_trigger.records[3].value = 0;
    CHECK(!valid_transient(state_zero_trigger));
    auto bad = decoded; bad.records[5].kind = COOP_TRANSIENT_SCRIPT; CHECK(!valid_transient(bad));
    bad = decoded; bad.records[3].key = 0x10000; CHECK(!valid_transient(bad));
    bad = decoded; bad.records[1].key = bad.records[0].key; bad.records[1].kind = bad.records[0].kind;
    CHECK(!valid_transient(bad));
    bad = decoded; bad.records[0].state = 0x100; CHECK(!valid_transient(bad));
    bad = decoded; bad.records[3].state = 3; CHECK(!valid_transient(bad));
    bad = decoded; bad.records[3].state = 2; bad.records[3].value = 0x100; CHECK(!valid_transient(bad));
    bad = decoded; bad.records[4].state = 26; CHECK(!valid_transient(bad));
    bad = decoded; bad.records[4].state = 8; bad.records[4].value = 1; CHECK(!valid_transient(bad));
    bad = decoded; bad.count = 6;
    bad.records[5] = {COOP_TRANSIENT_ACTOR_CYCLE, 1, 3, 90};
    CHECK(valid_transient(bad));
    bad.records[5].key = 257; CHECK(!valid_transient(bad));
    bad.records[5].key = 1; bad.records[5].state = 4; CHECK(!valid_transient(bad));
    bad.records[5].state = 3; bad.records[5].value = 91; CHECK(!valid_transient(bad));
    bad.records[5] = {COOP_TRANSIENT_TOMATO_BOARD, 0, 1, 0xA5A5A5A5u};
    CHECK(valid_transient(bad));
    bad.records[5].key = 1; CHECK(!valid_transient(bad));
    bad.records[5].key = 0; bad.records[5].state = 0; CHECK(!valid_transient(bad));
    bad.records[5].state = 1; bad.records[5].value = 3; CHECK(!valid_transient(bad));

    State hs{7, 10, 0, active}, gs{7, 20, 1, active};
    auto guest_input = frame(7, 20, 8);
    guest_input.records[0].state = 1; guest_input.records[1].value = 30;
    guest_input.count = 2;
    for (unsigned i = guest_input.count; i < COOP_TRANSIENT_RECORDS; ++i) guest_input.records[i] = {};
    TransientSync host, guest;
    guest.update(false, gs, guest_input, hs, {}, true, false, 55);
    host.update(true, hs, host_input, gs, guest.wire(), true, true, 55);
    CHECK(host.result().status == COOP_TRANSIENT_SYNCED && !host.result().count);
    guest.update(false, gs, guest_input, hs, host.wire(), true, true, 55);
    auto result = guest.result();
    CHECK(result.status == COOP_TRANSIENT_APPLYING && result.count == 5);
    CHECK(result.map == 7 && result.epoch == 20 && result.records[0].key == 0x34);
    // A ready host never asks a guest to rewind an already-fired trigger.
    host_input.records[3].state = 1; guest_input.records[1] = host_input.records[3];
    guest_input.records[1].state = 2;
    host.update(true, hs, host_input, gs, guest.wire(), true, true, 55);
    guest.update(false, gs, guest_input, hs, host.wire(), true, true, 55);
    bool rewound = false;
    for (unsigned i = 0; i < guest.result().count; ++i)
        rewound |= guest.result().records[i].kind == COOP_TRANSIENT_TRIGGER;
    CHECK(!rewound);
    // A guest that has already completed more piano notes is never rewound.
    host_input.records[4].state = 8; guest_input = host_input; guest_input.epoch = 20;
    guest_input.records[4].state = 9;
    host.update(true, hs, host_input, gs, guest.wire(), true, true, 55);
    guest.update(false, gs, guest_input, hs, host.wire(), true, true, 55);
    bool sequence_rewound = false;
    for (unsigned i = 0; i < guest.result().count; ++i)
        sequence_rewound |= guest.result().records[i].kind == COOP_TRANSIENT_SEQUENCE;
    CHECK(!sequence_rewound);
    guest_input = host_input; guest_input.epoch = 20;
    guest.update(false, gs, guest_input, hs, host.wire(), true, true, 55);
    CHECK(guest.result().status == COOP_TRANSIENT_SYNCED && !guest.result().count);

    // Either side loading, leaving, changing file, changing map, or aging out
    // immediately removes all apply commands. A later room epoch starts clean.
    auto stale_host = host.wire(); stale_host.map = 8;
    guest.update(false, gs, guest_input, hs, stale_host, true, true, 55);
    CHECK(guest.result().status == COOP_TRANSIENT_WAITING && !guest.result().count);
    State cut_h = hs, cut_g = gs; cut_h.flags = cut_g.flags = cutscene;
    host_input.epoch = ++cut_h.epoch; guest_input.epoch = ++cut_g.epoch;
    host.update(true, cut_h, host_input, cut_g, guest.wire(), true, true, 55);
    guest.update(false, cut_g, guest_input, cut_h, host.wire(), true, true, 55);
    CHECK(guest.result().status >= COOP_TRANSIENT_APPLYING);
    guest.update(false, cut_g, guest_input, cut_h, host.wire(), true, false, 55);
    CHECK(guest.result().status == COOP_TRANSIENT_WAITING && !guest.result().count);

    Packet packet{}; packet.kind = Kind::state; packet.sequence = 1; packet.session = 2;
    packet.authority_node = 4;
    packet.nonce = 3; packet.room = 123456; packet.player = hs; packet.transient = wire;
    auto bytes = encode(packet); Packet roundtrip{};
    CHECK(bytes.size() == 1368 && decode(bytes.data(), bytes.size(), roundtrip));
    CHECK(roundtrip.transient.count == 5 && roundtrip.transient.records[4].kind == COOP_TRANSIENT_SEQUENCE);
    bytes[transient_offset + 6 * 4 + 1] = 9; // Kind 9 is outside the bounded enum.
    CHECK(!decode(bytes.data(), bytes.size(), roundtrip));

    std::printf("PASS: %u same-area transient protocol checks (typed records, host authority, epochs, stale rejection, cutscene context)\n", checks);
}
