#include "session.hpp"
#include "../mod/gate_policy.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
using namespace dkcoop;
static unsigned checks = 0;
#define CHECK(x) do { ++checks; if (!(x)) { std::fprintf(stderr, "GATE FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)
#include "lossy_relay.hpp"
static ProgressInput input(const CoopGate& g) {
    const auto& p = g.input;
    return {p.enabled, p.file, p.ready, p.gate, p.request, p.session_hi, p.session_lo, p.scope};
}
static void receive(CoopGate& g, ProgressResult r) {
    coop_gate_receive(&g, {r.status, r.apply, r.acknowledged, r.session_hi, r.session_lo, r.scope});
}
static void protocol_checks() {
    Packet p{Kind::state, 10, 123, 456, 123456, {7, 1, 0, active}, {1, 2, 1, 2, 1, 0}};
    Packet out{}; auto b = encode(p);
    CHECK(decode(b.data(), b.size(), out));
    CHECK(b[83] == 1 && b[87] == 2 && b[91] == 1 && b[95] == 2 && b[99] == 1 && b[103] == 0);
    for (unsigned offset : {83u, 87u, 91u, 95u, 99u, 103u}) {
        auto bad = b; bad[offset] = 255; CHECK(!decode(bad.data(), bad.size(), out));
    }
    auto old = b; old[5] = 1; CHECK(!decode(old.data(), old.size(), out));
    CHECK(!decode(old.data(), 80, out));
    p.kind = Kind::welcome; b = encode(p); CHECK(!decode(b.data(), b.size(), out));
    CHECK(!valid_progress({0, 0, 0, 0, 1, 0}));
    CHECK(!valid_progress({1, 0, 1, 0, 0, 0}));
    CHECK(!valid_progress({1, 1, 0, 1, 1, 0}));
}
static void policy_checks() {
    CoopGate g{};
    coop_gate_prepare(&g, 1, 1, 0, 1, 0);
    CHECK(g.input.ready && g.locked_file == 1);
    // Offline guest writes are deferred without queuing an action for another host.
    CHECK(coop_gate_defer(&g, 1, 0, 1, 0, 0, 1, 1)); CHECK(!g.input.request);
    receive(g, {2, 0, 0, 0x1234, 0x5678, 1});
    CHECK(coop_gate_defer(&g, 1, 0, 1, 0, 0, 1, 1)); CHECK(g.input.request);
    CHECK(!coop_gate_defer(&g, 0, 0, 1, 0, 0, 1, 1)); // Host local action unchanged.
    CHECK(!coop_gate_defer(&g, 1, 4, 1, 0, 0, 1, 1)); // GB and every other flag unchanged.
    CHECK(!coop_gate_defer(&g, 1, 0, 1, 1, 0, 1, 1));
    CHECK(!coop_gate_defer(&g, 1, 0, 1, 2, 0, 1, 1));
    g.applying = 1; CHECK(!coop_gate_defer(&g, 1, 0, 1, 0, 0, 1, 1)); g.applying = 0;
    receive(g, {2, 0, 0, 0x1234, 0x5679, 1}); CHECK(!g.input.request);
    coop_gate_prepare(&g, 1, 1, 1, 1, 0); CHECK(g.file_changed && !g.input.enabled);
    coop_gate_prepare(&g, 1, 1, 0, 1, 0); CHECK(!g.input.enabled); // Restart required.

    ProgressInput l{1, 1, 1, 0, 1, 0, 123, 2};
    ProgressWire r{1, 2, 1, 2, 0, 0};
    CHECK(progress_wire(false, l, r, true, 123).value == 1);
    CHECK(progress_wire(false, l, r, true, 124).value == 0); // Old request/new session.
    r.file = r.scope = 3; CHECK(progress_wire(false, l, r, true, 123).value == 0);
    CHECK(progress_wire(false, l, r, false, 123).value == 0);
    l.gate = 1; CHECK(progress_result(false, l, r, true, 123).status == 4);
    CHECK(progress_wire(false, l, r, true, 123).value == 0); // Old guest save never grants host.
    r.value = 1; CHECK(progress_result(false, l, r, true, 123).apply == 0);
    CHECK(progress_result(true, {1, 1, 1, 0}, {1, 1, 1, 2, 1}, true, 123).apply == 0);
    CHECK(progress_result(true, {1, 1, 1, 0}, {1, 1, 1, 1, 1, 1}, true, 123).status == 4);
}
static void live_checks() {
    uint64_t now = 10000;
    Session host, guest;
    CHECK(host.start({Role::host, "", 0, 123456}, now));
    LossyRelay relay(host.bound_port());
    Config join{Role::join, "127.0.0.1", relay.port, 123456};
    CHECK(guest.start(join, now));
    CoopGate h{}, g{};
    State hs{7, 1, 0, active}, gs{7, 1, 1, active};
    bool host_gate = false, guest_gate = false, apply_host = true, apply_guest = true;
    unsigned host_writes = 0, guest_writes = 0;
    auto pump = [&](unsigned frames) {
        for (unsigned i = 0; i < frames; i++) {
            now += 10;
            coop_gate_prepare(&h, 1, 1, 0, hs.map == 7, host_gate);
            coop_gate_prepare(&g, 1, 1, 0, gs.map == 7, guest_gate);
            relay.forward(); host.tick(hs, now, input(h)); relay.forward();
            guest.tick(gs, now, input(g)); relay.forward();
            auto hr = host.progress(now), gr = guest.progress(now);
            receive(h, hr); receive(g, gr);
            if (apply_host && hr.apply && !host_gate) { host_gate = true; ++host_writes; }
            if (apply_guest && gr.apply && !guest_gate) { guest_gate = true; ++guest_writes; }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    pump(100); CHECK(h.result.status == 2 && g.result.status == 2);
    // Delay host application/readback and guest delivery/ack, emulating dropped work.
    apply_host = apply_guest = false;
    CHECK(coop_gate_defer(&g, 1, 0, 1, 0, 0, 1, 1));
    pump(100); CHECK(h.result.apply && g.input.request && !guest_gate && !host_gate);
    apply_host = true; pump(100);
    CHECK(host_gate && !guest_gate && host_writes == 1 && !h.result.acknowledged);
    CHECK(g.result.apply && !g.input.request); // Commit replaces the request, even before application.
    apply_guest = true; pump(100);
    CHECK(host_gate && guest_gate && host_writes == 1 && guest_writes == 1);
    CHECK(h.result.acknowledged && g.result.status == 3);
    pump(200); CHECK(host_writes == 1 && guest_writes == 1); // Replayed state never applies twice.
    // Reconnect retains host's save-derived state, without reapplying either save flag.
    guest.stop(); pump(10); CHECK(!h.result.apply);
    // The lossy relay can drop BYE. The host then keeps the old peer until
    // its 3-second timeout; allow that timeout plus HELLO retries/readback.
    CHECK(guest.start(join, now)); pump(500);
    CHECK(h.result.acknowledged && host_writes == 1 && guest_writes == 1);
    gs.map = 34; pump(100); CHECK(h.result.status == 1 && g.result.status == 1);
    gs.map = 7; pump(100); CHECK(h.result.acknowledged);
    // A lagging guest catches up to an already-open host gate. No host rewrite.
    guest_gate = false; pump(100); CHECK(guest_gate && guest_writes == 2 && host_writes == 1);
    // Host has a closed gate, guest ahead: refuse to merge/clear either save.
    host_gate = false; pump(100);
    CHECK(h.result.status == 4 && g.result.status == 4);
    CHECK(host_writes == 1 && guest_writes == 2 && !host_gate && guest_gate);
    // Timeout cannot emit new apply commands from cached packets.
    // Query the aged cache without draining a newly arrived UDP packet: tick()
    // legitimately timestamps such a packet as fresh at the supplied time.
    now += 800; CHECK(host.progress(now).status == 1);
    now += 3100; host.tick(hs, now, input(h)); CHECK(!host.progress(now).apply);
    CHECK(relay.dropped > 100 && relay.replayed > 100);
    CHECK(host.statistics().rejected + guest.statistics().rejected > 100);
}
int main() {
    protocol_checks(); policy_checks(); live_checks();
    std::printf("PASS: %u gate authority, game-side policy, retry, acknowledgement, save-conflict and reconnect checks\n", checks);
}
