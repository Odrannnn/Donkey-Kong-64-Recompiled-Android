#include "world.hpp"
#include "protocol.hpp"

namespace dkcoop {
static_assert(sizeof(CoopWorldInput) == 36 && sizeof(CoopWorldResult) == 28);
std::array<uint32_t, COOP_WORLD_WIRE_WORDS> world_words(const WorldWire& w) {
    return {w.feature, w.file, w.scope, w.ready, w.values, w.desired,
        w.revision[0], w.revision[1], w.request[0], w.request[1],
        w.base[0], w.base[1], w.request_values, w.ack[0], w.ack[1]};
}
WorldWire world_from_words(const std::array<uint32_t, COOP_WORLD_WIRE_WORDS>& w) {
    return {w[0], w[1], w[2], w[3], w[4], w[5], {w[6], w[7]}, {w[8], w[9]},
        {w[10], w[11]}, w[12], {w[13], w[14]}};
}
bool valid_world(const WorldWire& w) {
    if (!w.feature) {
        for (auto word : world_words(w)) if (word) return false;
        return true;
    }
    if (w.feature != 1 || w.file > 3 || w.scope > 3 || w.ready > 1
            || ((w.values | w.desired | w.request_values) & ~COOP_WORLD_MASK)) return false;
    if ((!w.file && (w.scope || w.values)) || (!!w.revision[0] != !!w.revision[1])) return false;
    if (w.ready && (!w.file || !w.scope || !w.revision[0])) return false;
    if (w.revision[0] && (!w.file || !w.scope)) return false;
    if (!w.revision[0] && (w.desired || w.request_values || w.request[0] || w.request[1]
            || w.base[0] || w.base[1] || w.ack[0] || w.ack[1])) return false;
    for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i) {
        if (!!w.request[i] != !!w.base[i]) return false;
        if (!w.request[i] && (w.request_values & (1u << i))) return false;
    }
    return true;
}
bool valid_world_input(const CoopWorldInput& l) {
    return l.enabled <= 1 && l.file <= 3 && l.scope <= 3 && l.ready <= 1
        && !(l.values & ~COOP_WORLD_MASK) && (!l.ready || (l.enabled && l.file));
}
void WorldSync::reset() { *this = WorldSync{}; }
namespace {
uint32_t advance(uint32_t value) { ++value; return value ? value : 1; }
void set_bit(uint32_t& values, unsigned bit, uint32_t source) {
    values = (values & ~bit) | (source & bit);
}
}
void WorldSync::update(bool host, const CoopWorldInput& l, const WorldWire& r,
        bool connected, bool fresh, bool permitted, uint64_t session) {
    output = {};
    if (!valid_world_input(l) || !l.enabled) { reset(); return; }
    if (!connected || !session || !l.file) {
        reset(); outgoing.feature = 1; outgoing.file = l.file; output.status = 1; return;
    }
    if (!valid_world(r) || !r.feature || !r.file) {
        // Temporary feature/readiness gaps do not reset host revisions within
        // a transport session. That would make delayed acknowledgements unsafe.
        if (binding != session || file != l.file) reset();
        outgoing.feature = 1; outgoing.file = l.file; outgoing.ready = 0; output.status = 1;
        if (binding) {
            output.session_hi = uint32_t(binding >> 32); output.session_lo = uint32_t(binding); output.scope = scope;
        }
        return;
    }
    const uint32_t wanted_scope = host ? l.file : r.file;
    if (binding != session || file != l.file || scope != wanted_scope || peer_file != r.file) {
        reset(); binding = session; file = l.file; scope = wanted_scope; peer_file = r.file;
    }
    outgoing.feature = 1; outgoing.file = file; outgoing.scope = scope;
    output.status = 1; output.session_hi = uint32_t(session >> 32);
    output.session_lo = uint32_t(session); output.scope = scope;
    const bool bound = l.session_hi == output.session_hi && l.session_lo == output.session_lo && l.scope == scope;
    // Guest revisions are only echoes; only the host can publish authority.
    const bool peer_bound = r.scope == scope && r.revision[0] && r.revision[1];
    const bool peer_role_valid = host ? (!r.ack[0] && !r.ack[1])
        : (!r.request[0] && !r.request[1] && !r.base[0] && !r.base[1] && !r.request_values);
    outgoing.ready = 0;
    if (!bound || !fresh || !permitted || !peer_role_valid) return;
    if (!initialized) {
        if (!l.ready || (!host && !peer_bound)) return;
        initialized = true;
        outgoing.desired = host ? l.values : r.desired;
        for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i) {
            outgoing.revision[i] = host ? 1 : r.revision[i];
            last_change[i] = l.change[i]; // Never turn an offline guest save into a request.
        }
    }
    if (l.ready) {
        outgoing.values = l.values; outgoing.ready = 1;
        // Process local intent before incoming changes. Requests are based on
        // the revision the guest actually saw, not a newer packet in this tick.
        for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i) {
            const unsigned bit = 1u << i;
            if (l.change[i] == last_change[i]) continue;
            last_change[i] = l.change[i];
            if (host) {
                set_bit(outgoing.desired, bit, l.values);
                outgoing.revision[i] = advance(outgoing.revision[i]);
            } else {
                outgoing.request[i] = advance(outgoing.request[i]);
                outgoing.base[i] = outgoing.revision[i];
                set_bit(outgoing.request_values, bit, l.values);
                pending |= bit;
            }
        }
    }
    if (!peer_bound) return;
    for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i) {
        const unsigned bit = 1u << i;
        if (host) {
            if (r.request[i] && (!outgoing.ack[i] || newer(r.request[i], outgoing.ack[i]))) {
                // Reject requests based on old authority, but acknowledge them
                // so a losing simultaneous request does not retry forever.
                if (r.base[i] == outgoing.revision[i]) {
                    set_bit(outgoing.desired, bit, r.request_values);
                    outgoing.revision[i] = advance(outgoing.revision[i]);
                }
                outgoing.ack[i] = r.request[i];
            }
        } else if (r.revision[i] == outgoing.revision[i] || newer(r.revision[i], outgoing.revision[i])) {
            outgoing.revision[i] = r.revision[i];
            set_bit(outgoing.desired, bit, r.desired);
            if (outgoing.request[i] && r.ack[i] == outgoing.request[i]) pending &= ~bit;
        }
    }
    output.desired = outgoing.desired;
    output.pending = pending | (l.values ^ outgoing.desired) | (r.values ^ outgoing.desired);
    // A guest must see the latest authority and both saves must read back the
    // desired bits before the HUD can say synced. Readiness pauses keep queues.
    for (unsigned i = 0; i < COOP_WORLD_TOGGLES; ++i)
        if (r.revision[i] != outgoing.revision[i]) output.pending |= 1u << i;
    output.apply = l.ready ? (l.values ^ outgoing.desired) & ~pending : 0;
    output.status = !l.ready || !r.ready ? 1 : output.pending ? 2 : 3;
}
}
