#include "progress.hpp"

namespace dkcoop {
bool valid_progress(const ProgressWire& p) {
    if (!p.feature) return !(p.file || p.ready || p.scope || p.value || p.ack);
    return p.feature == japes_gate_feature && p.file <= 3 && p.ready <= 1 && p.scope <= 3
        && p.value <= 1 && p.ack <= 1 && (!p.ready || p.file != 0)
        && (p.ready || !(p.scope || p.value || p.ack));
}
bool valid_progress_input(const ProgressInput& p) {
    return p.enabled <= 1 && p.file <= 3 && p.ready <= 1 && p.gate <= 1 && p.request <= 1
        && p.seen_scope <= 3 && (!p.ready || (p.enabled && p.file));
}
namespace {
bool available(const ProgressInput& l, const ProgressWire& r, bool connected) {
    return valid_progress_input(l) && l.enabled && l.ready && connected
        && valid_progress(r) && r.feature == japes_gate_feature && r.ready;
}
bool bound(const ProgressInput& l, const ProgressWire& r, uint64_t session) {
    return l.seen_session_hi == uint32_t(session >> 32) && l.seen_session_lo == uint32_t(session)
        && l.seen_scope == r.file && r.scope == r.file;
}
}
ProgressWire progress_wire(bool host, const ProgressInput& l, const ProgressWire& r,
        bool connected, uint64_t session) {
    if (!valid_progress_input(l) || !l.enabled) return {};
    ProgressWire w{japes_gate_feature, l.file, l.ready, 0, 0, 0};
    if (!l.ready) return w;
    if (host) { w.scope = l.file; w.value = l.gate; return w; }
    if (!available(l, r, connected) || !bound(l, r, session)) return w;
    w.scope = r.file;
    // The guest's old save is never offered as host progression. Only a new
    // observed switch event can request a change in the current host session.
    w.value = l.request && !l.gate && !r.value;
    w.ack = l.gate;
    return w;
}
ProgressResult progress_result(bool host, const ProgressInput& l, const ProgressWire& r,
        bool connected, uint64_t session) {
    if (!valid_progress_input(l) || !l.enabled) return {};
    ProgressResult out{uint32_t(ProgressStatus::waiting)};
    if (!available(l, r, connected)) return out;
    const uint32_t scope = host ? l.file : r.file;
    if ((host && r.scope != l.file) || (!host && r.scope != r.file)) return out;
    out.session_hi = uint32_t(session >> 32); out.session_lo = uint32_t(session); out.scope = scope;
    if ((host && !l.gate && r.ack) || (!host && l.gate && !r.value)) {
        out.status = uint32_t(ProgressStatus::conflict); return out;
    }
    if (host) {
        out.apply = !l.gate && r.value;
        out.acknowledged = l.gate && r.ack;
        out.status = uint32_t(l.gate ? ProgressStatus::committed : ProgressStatus::ready);
    } else {
        out.apply = r.value && !l.gate;
        out.acknowledged = r.value && l.gate;
        out.status = uint32_t(r.value && l.gate ? ProgressStatus::committed : ProgressStatus::ready);
    }
    return out;
}
}
