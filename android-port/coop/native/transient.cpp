#include "transient.hpp"
#include "protocol.hpp"

namespace dkcoop {
static_assert(sizeof(CoopTransientRecord) == 16);
static_assert(sizeof(CoopTransientInput) == COOP_TRANSIENT_WIRE_WORDS * 4);
static_assert(sizeof(CoopTransientResult) == COOP_TRANSIENT_RESULT_WORDS * 4);

namespace {
bool valid_record(const CoopTransientRecord& record) {
    if (!record.kind) return !record.key && !record.state && !record.value;
    if (record.kind > COOP_TRANSIENT_KIND_COUNT || !record.key || record.key > 0xFFFFu)
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
        case COOP_TRANSIENT_AI:
            return record.state <= 0xFFFFu;
    }
    return false;
}
bool same_record(const CoopTransientRecord& a, const CoopTransientRecord& b) {
    return a.kind == b.kind && a.key == b.key && a.state == b.state && a.value == b.value;
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
    if (context != next) context = next;
    output.map = local.map; output.epoch = local.epoch;
    if (host) { output.status = COOP_TRANSIENT_SYNCED; return; }
    output.status = COOP_TRANSIENT_SYNCED;
    for (unsigned i = 0; i < remote.count && output.count < COOP_TRANSIENT_RECORDS; ++i) {
        const auto& wanted = remote.records[i];
        const CoopTransientRecord* current = nullptr;
        for (unsigned j = 0; j < input.count; ++j)
            if (input.records[j].kind == wanted.kind && input.records[j].key == wanted.key) {
                current = &input.records[j]; break;
            }
        if (!current || !same_record(*current, wanted)) output.records[output.count++] = wanted;
    }
    if (output.count) output.status = COOP_TRANSIENT_APPLYING;
}
}
