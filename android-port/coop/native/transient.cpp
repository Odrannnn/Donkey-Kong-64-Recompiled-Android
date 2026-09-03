#include "transient.hpp"
#include "protocol.hpp"

namespace dkcoop {
static_assert(sizeof(CoopTransientRecord) == 16);
static_assert(sizeof(CoopTransientInput) == COOP_TRANSIENT_WIRE_WORDS * 4);
static_assert(sizeof(CoopTransientResult) == COOP_TRANSIENT_RESULT_WORDS * 4);

namespace {
bool valid_record(const CoopTransientRecord& record) {
    if (!record.kind) return !record.key && !record.state && !record.value;
    // Model-two object ID zero is valid (for example Tiny Temple's opening
    // switch and Galleon's first water switch). Kind zero, rather than key
    // zero, is the empty-record discriminator.
    if (record.kind > COOP_TRANSIENT_KIND_COUNT || record.key > 0xFFFFu)
        return false;
    switch (record.kind) {
        case COOP_TRANSIENT_SCRIPT:
            return record.state <= 0xFFu && record.value <= 2u;
        case COOP_TRANSIENT_TIMER:
            return record.state <= 0xFFu && record.value <= 0xFFFFu;
        case COOP_TRANSIENT_PLATFORM:
            return record.state <= 0xFFu;
        case COOP_TRANSIENT_CUTSCENE:
            return record.key <= 0xFFu && record.state <= 0xFFu && record.value <= 0xFFu;
        case COOP_TRANSIENT_TRIGGER:
            return (record.state == 1u || record.state == 2u)
                && record.value >= 1u && record.value <= 0xFFu;
        case COOP_TRANSIENT_SEQUENCE:
            return record.state <= 25u && !record.value;
        case COOP_TRANSIENT_ACTOR_CYCLE:
            return record.key >= 1u && record.key <= 256u
                && record.state <= 3u && record.value <= 90u;
        case COOP_TRANSIENT_TOMATO_BOARD:
            if (record.key || (record.state != 1u && record.state != 2u)) return false;
            for (unsigned i = 0; i < 16; ++i)
                if (((record.value >> (i * 2)) & 3u) == 3u) return false;
            return true;
        case COOP_TRANSIENT_TOMATO_CLOCK:
            return !record.key && ((record.state == 1u && record.value <= 60u)
                || (record.state == 2u && !record.value));
        case COOP_TRANSIENT_MINIGAME_SUCCESS:
            return record.key >= 1u && record.key <= 4u
                && record.state == 1u && !record.value;
        case COOP_TRANSIENT_MINECART_SUCCESS:
            return record.key >= 1u && record.key <= 3u
                && record.state == 1u && !record.value;
        case COOP_TRANSIENT_RABBIT_SUCCESS:
            return record.key >= 1u && record.key <= 2u
                && record.state == 1u && !record.value;
        case COOP_TRANSIENT_BATTY_SUCCESS:
            return record.key >= 1u && record.key <= 3u
                && record.state == 1u && !record.value;
        case COOP_TRANSIENT_OWL_SUCCESS:
            return record.key == 1u && record.state == 1u && !record.value;
    }
    return false;
}
bool same_record(const CoopTransientRecord& a, const CoopTransientRecord& b) {
    // A ready host never rewinds a local room action. Once the host has fired
    // a trigger, the receiver only needs to report its own fired observation.
    if (a.kind == COOP_TRANSIENT_TRIGGER && b.kind == COOP_TRANSIENT_TRIGGER
            && a.key == b.key && a.value == b.value)
        return b.state == 1u || a.state == 2u;
    if (a.kind == COOP_TRANSIENT_SEQUENCE && b.kind == COOP_TRANSIENT_SEQUENCE
            && a.key == b.key)
        return a.state >= b.state;
    return a.kind == b.kind && a.key == b.key && a.state == b.state && a.value == b.value;
}
bool bidirectional_event(const CoopTransientRecord& record) {
    // These records are bounded commands rather than copied runtime state.
    // The receiving game adapter still validates the immutable map/object
    // allowlist and the exact local ready state before executing one step.
    if (record.kind == COOP_TRANSIENT_TRIGGER) return record.state == 2u;
    if (record.kind == COOP_TRANSIENT_SEQUENCE) return record.state > 0u;
    return record.kind == COOP_TRANSIENT_MINIGAME_SUCCESS
        || record.kind == COOP_TRANSIENT_MINECART_SUCCESS
        || record.kind == COOP_TRANSIENT_RABBIT_SUCCESS
        || record.kind == COOP_TRANSIENT_BATTY_SUCCESS
        || record.kind == COOP_TRANSIENT_OWL_SUCCESS;
}
unsigned lobby_pad_index(unsigned map, unsigned key) {
    if (map == 173 && key <= 0x03) return key + 1;
    if (map == 174 && key >= 0x04 && key <= 0x08) return 5 + key - 0x04;
    if (map == 175 && key <= 0x04) return 10 + key;
    if (map == 193 && key >= 0x02 && key <= 0x06) return 15 + key - 0x02;
    if (map == 194 && key >= 0x10 && key <= 0x14) return 20 + key - 0x10;
    return 0;
}
}

std::array<uint32_t, COOP_TRANSIENT_WIRE_WORDS> transient_words(const TransientWire& wire) {
    std::array<uint32_t, COOP_TRANSIENT_WIRE_WORDS> words{};
    words[0] = wire.feature; words[1] = wire.file; words[2] = wire.map;
    words[3] = wire.epoch; words[4] = wire.revision; words[5] = wire.count;
    size_t n = 6;
    for (const auto& record : wire.records) {
        words[n++] = record.kind; words[n++] = record.key;
        words[n++] = record.state; words[n++] = record.value;
    }
    return words;
}

TransientWire transient_from_words(const std::array<uint32_t, COOP_TRANSIENT_WIRE_WORDS>& words) {
    TransientWire wire{};
    wire.feature = words[0]; wire.file = words[1]; wire.map = words[2];
    wire.epoch = words[3]; wire.revision = words[4]; wire.count = words[5];
    size_t n = 6;
    for (auto& record : wire.records) {
        record.kind = words[n++]; record.key = words[n++];
        record.state = words[n++]; record.value = words[n++];
    }
    return wire;
}

bool valid_transient(const TransientWire& wire) {
    if (!wire.feature) {
        for (auto word : transient_words(wire)) if (word) return false;
        return true;
    }
    if (wire.feature != 1 || wire.file > 2 || wire.map > 255 || !wire.epoch
            || !wire.revision || wire.count > COOP_TRANSIENT_RECORDS) return false;
    for (unsigned i = 0; i < COOP_TRANSIENT_RECORDS; ++i) {
        if (i < wire.count) {
            if (!valid_record(wire.records[i])) return false;
            for (unsigned j = 0; j < i; ++j)
                if (wire.records[j].kind == wire.records[i].kind
                        && wire.records[j].key == wire.records[i].key) return false;
        } else if (!valid_record(wire.records[i]) || wire.records[i].kind) return false;
    }
    return true;
}

bool valid_transient_input(const CoopTransientInput& input) {
    TransientWire wire{input.enabled, input.file, input.map, input.epoch,
        input.revision, input.count, {}};
    for (unsigned i = 0; i < COOP_TRANSIENT_RECORDS; ++i) wire.records[i] = input.records[i];
    return valid_transient(wire);
}

void TransientSync::reset() { *this = {}; }

void TransientSync::update(bool host, const State& local, const CoopTransientInput& input,
        const State& peer, const TransientWire& remote, bool connected, bool fresh,
        uint64_t session) {
    output = {};
    if (!valid_transient_input(input) || !input.enabled) { reset(); return; }
    outgoing.feature = 1; outgoing.file = input.file; outgoing.map = input.map;
    outgoing.epoch = input.epoch; outgoing.revision = input.revision;
    outgoing.count = input.count;
    for (unsigned i = 0; i < COOP_TRANSIENT_RECORDS; ++i) outgoing.records[i] = input.records[i];
    output.status = COOP_TRANSIENT_WAITING;
    if (!connected || !fresh || !session || !valid_state(local) || !valid_state(peer)
            || !valid_transient(remote) || !remote.feature || local.map != peer.map
            || input.map != local.map || remote.map != peer.map || input.file != remote.file
            || !(local.flags & (active | cutscene)) || !(peer.flags & (active | cutscene))) {
        context = {}; return;
    }
    const std::array<uint32_t, 6> next{uint32_t(session >> 32), uint32_t(session),
        local.map, local.epoch, peer.epoch, input.file};
    if (context != next) {
        context = next;
        remote_lobby_pad_state.fill(0);
    }
    output.map = local.map; output.epoch = local.epoch;
    output.status = COOP_TRANSIENT_SYNCED;
    for (unsigned i = 0; i < remote.count && output.count < COOP_TRANSIENT_RECORDS; ++i) {
        const auto& wanted = remote.records[i];
        // Raw scripts, timers, platforms, actor cycles, cutscenes and boards
        // remain host-authoritative. Reviewed activation and ordered-sequence
        // commands are safe in either direction because the game adapter can
        // execute only their next pinned vanilla entry from an exact ready
        // state. The host therefore validates a Join request locally and its
        // next captured frame rebroadcasts the accepted result.
        if (host && !bidirectional_event(wanted)) continue;
        const CoopTransientRecord* current = nullptr;
        for (unsigned j = 0; j < input.count; ++j)
            if (input.records[j].kind == wanted.kind && input.records[j].key == wanted.key) {
                current = &input.records[j]; break;
            }
        bool deliver = !current || !same_record(*current, wanted);
        unsigned pad = wanted.kind == COOP_TRANSIENT_TRIGGER
            ? lobby_pad_index(remote.map, wanted.key) : 0;
        if (pad && wanted.value == 2u) {
            uint8_t& observed = remote_lobby_pad_state[pad - 1];
            if (wanted.state == 1u) {
                observed = 1;
                deliver = false;
            } else if (wanted.state == 2u) {
                deliver = observed != 2;
                observed = 2;
            }
        }
        if (deliver) output.records[output.count++] = wanted;
    }
    if (output.count) output.status = COOP_TRANSIENT_APPLYING;
}
}
