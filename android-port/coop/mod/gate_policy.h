#ifndef COOP_GATE_POLICY_H
#define COOP_GATE_POLICY_H
// Portable game-side policy, shared by the MIPS mod and host regression tests.
// Each member is one 32-bit ABI word; no pointers cross the network.
typedef struct {
    unsigned enabled, file, ready, gate, request, session_hi, session_lo, scope;
} CoopGateInput;
typedef struct {
    unsigned status, apply, acknowledged, session_hi, session_lo, scope;
} CoopGateResult;
typedef struct {
    CoopGateInput input;
    CoopGateResult result;
    unsigned locked_file, file_changed, applying;
} CoopGate;

static inline void coop_gate_prepare(CoopGate* g, unsigned enabled, unsigned adventure,
        unsigned file, unsigned in_japes, unsigned gate_open) {
    if (enabled && adventure && file < 3) {
        if (!g->locked_file) g->locked_file = file + 1;
        else if (g->locked_file != file + 1) g->file_changed = 1;
    }
    g->input.enabled = enabled && !g->file_changed;
    g->input.file = g->locked_file;
    g->input.ready = g->input.enabled && adventure && in_japes && file < 3 && g->locked_file == file + 1;
    g->input.gate = g->input.ready && gate_open;
    if (!g->input.ready) {
        g->input.request = 0;
        g->result = (CoopGateResult){0};
        g->input.session_hi = g->input.session_lo = g->input.scope = 0;
    }
}
static inline void coop_gate_receive(CoopGate* g, CoopGateResult result) {
    if (result.session_hi != g->input.session_hi || result.session_lo != g->input.session_lo
            || result.scope != g->input.scope || result.status != 2 || result.apply)
        g->input.request = 0;
    g->input.session_hi = result.session_hi; g->input.session_lo = result.session_lo;
    g->input.scope = result.scope; g->result = result;
}
// Return 1 to defer the guest's write. Host-confirmed application bypasses this.
static inline unsigned coop_gate_defer(CoopGate* g, unsigned join, int flag, unsigned value,
        unsigned type, unsigned current_file, unsigned adventure, unsigned in_japes) {
    if (!join || !g->input.enabled || g->applying || !adventure || !in_japes || flag != 0 || type != 0)
        return 0;
    if (value && g->input.ready && current_file + 1 == g->locked_file
            && g->result.status == 2 && g->input.scope && !g->input.gate)
        g->input.request = 1;
    return 1; // Never independently grant or clear the supported gate on a guest.
}
#endif
