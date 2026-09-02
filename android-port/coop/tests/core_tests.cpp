#include "session.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>
#include <thread>
#include <chrono>
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
#define CHECK(expression) do { ++checks; if (!(expression)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expression); std::exit(1); } } while (0)

static void protocol_tests() {
    Packet p{Kind::state, 0x12345678, 0x0102030405060708ULL, 0x1122334455667788ULL, 123456,
        {34, 3, 4, active, 1.0f, -2.5f, 0.0f, 4095, 5, 3.5f, 11, 0x000007A9u}};
    auto bytes = encode(p); Packet decoded;
    CHECK(bytes.size() == packet_size);
    const std::array<uint8_t, 40> header{0x44,0x4b,0x43,0x50,0,50,0,3,0,1,2,0x32,0x12,0x34,0x56,0x78,
        1,2,3,4,5,6,7,8,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0,1,0xe2,0x40,0,0,0,0};
    for (size_t i = 0; i < header.size(); ++i) CHECK(bytes[i] == header[i]);
    CHECK(bytes[56] == 0x3f && bytes[57] == 0x80 && bytes[60] == 0xc0 && bytes[61] == 0x20);
    CHECK(bytes[1195] == 11 && bytes[1196] == 0 && bytes[1197] == 0
        && bytes[1198] == 7 && bytes[1199] == 0xa9);
    CHECK(decode(bytes.data(), bytes.size(), decoded));
    CHECK(decoded.session == p.session && decoded.nonce == p.nonce && decoded.sequence == p.sequence);
    CHECK(state_to_words(decoded.player) == state_to_words(p.player));
    for (size_t length = 0; length < packet_size; length++) CHECK(!decode(bytes.data(), length, decoded));
    CHECK(!decode(bytes.data(), packet_size + 1, decoded));
    for (size_t index : {size_t(0), size_t(4), size_t(8), size_t(36)}) {
        auto bad = bytes; bad[index] ^= 0x80; CHECK(!decode(bad.data(), bad.size(), decoded));
    }
    auto rejected = [&](Packet invalid) { auto raw = encode(invalid); CHECK(!decode(raw.data(), raw.size(), decoded)); };
    auto bad = p; bad.player.x = std::numeric_limits<float>::infinity(); rejected(bad);
    bad = p; bad.player.z = std::numeric_limits<float>::quiet_NaN(); rejected(bad);
    bad = p; bad.player.y = 100001; rejected(bad);
    bad = p; bad.player.character = 5; rejected(bad);
    bad = p; bad.player.map = 256; rejected(bad);
    bad = p; bad.player.yaw = 4096; rejected(bad);
    bad = p; bad.player.flags = 128; rejected(bad);
    bad = p; bad.player.animation_frame = -1; rejected(bad);
    bad = p; bad.player.transition_ticket = 0; rejected(bad);
    bad = p; bad.player.transition_route = 0; rejected(bad);
    bad = p; bad.player.transition_route |= 0x01000000u; rejected(bad);
    bad = p; bad.player.transition_route = 0x00002AA9u; rejected(bad); // T&S is not a reviewed route.
    bad = p; bad.room = 99999; rejected(bad);
    bad = p; bad.nonce = 0; rejected(bad);
    bad = p; bad.session = 0; rejected(bad);
    bad = p; bad.kind = Kind::hello; rejected(bad);
    auto welcome = p; welcome.kind = Kind::welcome;
    welcome.player.transition_ticket = 0; welcome.player.transition_route = 0;
    auto welcome_bytes = encode(welcome);
    CHECK(decode(welcome_bytes.data(), welcome_bytes.size(), decoded));
    welcome.player.transition_ticket = p.player.transition_ticket;
    welcome.player.transition_route = p.player.transition_route;
    rejected(welcome);
    CHECK(newer(0, 0xffffffffu)); CHECK(!newer(0xffffffffu, 0));
    CHECK(!newer(1, 1)); CHECK(!newer(0x80000000u, 0));
    std::mt19937 random(42);
    for (unsigned i = 0; i < 50000; i++) {
        std::array<uint8_t, packet_size + 16> fuzz{}; for (auto& byte : fuzz) byte = uint8_t(random());
        if (decode(fuzz.data(), random() % (fuzz.size() + 1), decoded)) CHECK(valid_state(decoded.player));
    }
}
static void session_tests() {
    uint64_t now = 10000;
    Session host, guest;
    CHECK(host.start({Role::host, "", 0, 123456}, now));
    Config join{Role::join, "127.0.0.1", host.bound_port(), 123456};
    CHECK(guest.start(join, now));
    State h{34, 7, 0, active, 1, 2, 3, 42, 0, 0}, g{34, 9, 4, active, 4, 5, 6, 123, 0, 0};
    auto pump = [&](unsigned frames) {
        for (unsigned i = 0; i < frames; ++i) {
            now += 10; host.tick(h, now); guest.tick(g, now);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    pump(100);
    CHECK(host.status() == Status::connected && guest.status() == Status::connected);
    CHECK(state_to_words(host.remote(now)) == state_to_words(g));
    CHECK(state_to_words(guest.remote(now)) == state_to_words(h));
    Session third; CHECK(third.start(join, now));
    for (unsigned i = 0; i < 70; i++) { pump(1); third.tick(g, now); }
    CHECK(third.status() == Status::busy);
    CHECK(host.remote(now).character == g.character);
    Session wrong; auto wrong_config = join; wrong_config.room++;
    CHECK(wrong.start(wrong_config, now));
    for (unsigned i = 0; i < 70; i++) { pump(1); wrong.tick(g, now); }
    CHECK(wrong.status() == Status::connecting);
    CHECK(host.statistics().rejected > 0);
    // No guest ticks: stale avatars disappear before the connection times out.
    now += 800; host.tick(h, now); CHECK(host.remote(now).flags == 0);
    now += 3100; host.tick(h, now);
    CHECK(host.status() == Status::listening);
    third.stop(); wrong.stop(); guest.stop();
    CHECK(guest.start(join, now)); pump(120);
    CHECK(host.status() == Status::connected && guest.status() == Status::connected);
    guest.stop();
    for (unsigned i = 0; i < 20 && host.status() != Status::listening; i++) {
        now += 10; host.tick(h, now); std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    CHECK(host.status() == Status::listening);
    CHECK(host.remote(now).flags == 0);
    Session conflict; CHECK(!conflict.start({Role::host, "", host.bound_port(), 123456}, now));
    CHECK(conflict.status() == Status::error);
    CHECK(!conflict.start({Role::join, "not-an-ip", host.bound_port(), 123456}, now));
    CHECK(!conflict.start({Role::join, "255.255.255.255", host.bound_port(), 123456}, now));
    CHECK(!conflict.start({Role::host, "", 0, 0}, now));
    CHECK(conflict.start({Role::off, "", 0, 0}, now)); CHECK(conflict.status() == Status::off);
}

// Exercise the real socket boundary with a peer that can send adversarial packets.
// This checks ownership and replay defenses independently of Session's sender.
struct RawPeer {
#ifdef _WIN32
    SOCKET socket = INVALID_SOCKET;
#else
    int socket = -1;
#endif
    sockaddr_in target{};
    explicit RawPeer(uint16_t port) {
#ifdef _WIN32
        WSADATA data{}; CHECK(WSAStartup(MAKEWORD(2, 2), &data) == 0);
#endif
        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
        CHECK(socket != INVALID_SOCKET);
        u_long nonblocking = 1; CHECK(ioctlsocket(socket, FIONBIO, &nonblocking) == 0);
#else
        CHECK(socket >= 0);
        CHECK(fcntl(socket, F_SETFL, O_NONBLOCK) == 0);
#endif
        sockaddr_in address{}; address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        CHECK(bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0);
        target = address; target.sin_port = htons(port);
    }
    ~RawPeer() {
#ifdef _WIN32
        closesocket(socket); WSACleanup();
#else
        close(socket);
#endif
    }
    void send(const Packet& packet) {
        auto bytes = encode(packet);
        CHECK(sendto(socket, reinterpret_cast<const char*>(bytes.data()), int(bytes.size()), 0,
            reinterpret_cast<sockaddr*>(&target), sizeof(target)) == int(bytes.size()));
    }
    bool receive(Packet& packet) {
        std::array<uint8_t, packet_size + 1> bytes{};
        int count = int(recvfrom(socket, reinterpret_cast<char*>(bytes.data()), int(bytes.size()), 0, nullptr, nullptr));
        return count > 0 && decode(bytes.data(), size_t(count), packet);
    }
};

static void adversarial_session_tests() {
    uint64_t now = 10000;
    Session host;
    CHECK(host.start({Role::host, "", 0, 123456}, now));
    RawPeer peer(host.bound_port()), stranger(host.bound_port());
    State h{34, 1, 0, active, 1, 2, 3, 0, 0, 0};
    auto pump = [&] {
        for (unsigned i = 0; i < 20; i++) {
            host.tick(h, ++now); std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    peer.send({Kind::hello, 1, 0, 0x123456789, 123456, {}});
    Packet welcome{}; bool welcomed = false;
    for (unsigned i = 0; i < 10 && !welcomed; i++) { pump(); welcomed = peer.receive(welcome); }
    CHECK(welcomed && welcome.kind == Kind::welcome && welcome.nonce == 0x123456789);
    Packet state{Kind::state, 100, welcome.session, welcome.nonce, 123456,
        {34, 2, 1, active, 40, 50, 60, 100, 0, 0}};
    peer.send(state); pump();
    CHECK(host.status() == Status::connected && host.remote(now).x == 40);
    auto rejected_before = host.statistics().rejected;
    auto bad = state; bad.player.x = 999;
    peer.send(bad); // Duplicate sequence, different state.
    bad.sequence = 99; peer.send(bad); // Old state.
    bad.sequence = 101; bad.session ^= 1; peer.send(bad);
    bad = state; bad.sequence = 102; bad.nonce ^= 1; peer.send(bad);
    bad = state; bad.sequence = 103; stranger.send(bad); // Valid fields, wrong endpoint.
    bad.kind = Kind::bye; stranger.send(bad);
    bad.nonce ^= 1; peer.send(bad); // Correct endpoint cannot terminate another nonce.
    bad = state; bad.sequence = 104; bad.room++; peer.send(bad);
    pump();
    CHECK(host.statistics().rejected >= rejected_before + 8);
    CHECK(host.status() == Status::connected && host.remote(now).x == 40);
    state.sequence = 105; state.player.x = 41; peer.send(state); pump();
    CHECK(host.remote(now).x == 41); // Rejected packets did not advance the receive sequence.
    state.kind = Kind::bye; state.sequence = 106; peer.send(state); pump();
    CHECK(host.status() == Status::listening && host.remote(now).flags == 0);
    peer.send(state); pump(); // Replayed goodbye does not resurrect the old session.
    CHECK(host.status() == Status::listening);
}
int main() {
    protocol_tests(); session_tests(); adversarial_session_tests();
    std::printf("PASS: %u assertions, 50000 malformed-packet probes, live UDP lifecycle tests\n", checks);
}
