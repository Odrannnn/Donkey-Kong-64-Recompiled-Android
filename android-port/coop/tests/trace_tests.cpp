#include "session.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
using TestSocket = SOCKET;
constexpr TestSocket invalid_test_socket = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
using TestSocket = int;
constexpr TestSocket invalid_test_socket = -1;
#endif

#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "TRACE FAIL %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)

static void close_test_socket(TestSocket socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

static TestSocket occupy_udp_port(uint16_t port) {
    TestSocket result = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(result != invalid_test_socket);
#ifdef _WIN32
    BOOL exclusive = TRUE;
    CHECK(setsockopt(result, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
        reinterpret_cast<const char*>(&exclusive), sizeof(exclusive)) == 0);
#endif
    sockaddr_in local{}; local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY); local.sin_port = htons(port);
    CHECK(bind(result, reinterpret_cast<const sockaddr*>(&local), sizeof(local)) == 0);
    return result;
}

static bool no_packet(TestSocket socket) {
    char buffer[16];
    int count = int(recvfrom(socket, buffer, sizeof(buffer), 0, nullptr, nullptr));
    if (count >= 0) return false;
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}

int main() {
    dkcoop::Session session;
    uint16_t base = 0;
    for (uint16_t candidate = 36464; candidate < 36564; candidate += 2) {
        if (session.start({dkcoop::Role::host, "", candidate, 123456}, dkcoop::clock_ms())
                && session.trace_port() == candidate + 1) {
            base = candidate; break;
        }
        session.stop();
    }
    CHECK(base && session.status() == dkcoop::Status::listening);
    // A second Windows client on the same machine must still expose traces.
    // Force three collisions to verify that every candidate uses a fresh
    // socket; Winsock cannot reliably re-bind one socket after a failed bind.
    TestSocket blocker2 = occupy_udp_port(base + 2);
    TestSocket blocker3 = occupy_udp_port(base + 3);
    dkcoop::Session second;
    CHECK(second.start({dkcoop::Role::join, "127.0.0.1", base, 123456}, dkcoop::clock_ms()));
    CHECK(second.trace_port() == base + 4);

    TestSocket query = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(query != invalid_test_socket);
#ifdef _WIN32
    u_long nonblocking = 1;
    CHECK(ioctlsocket(query, FIONBIO, &nonblocking) == 0);
#else
    int flags = fcntl(query, F_GETFL, 0);
    CHECK(flags >= 0 && fcntl(query, F_SETFL, flags | O_NONBLOCK) == 0);
#endif
    sockaddr_in target{}; target.sin_family = AF_INET; target.sin_port = htons(session.trace_port());
    CHECK(inet_pton(AF_INET, "127.0.0.1", &target.sin_addr) == 1);

    dkcoop::State player{34, 7, 2, dkcoop::active, 1, 2, 3, 4, 5, 6,
        7, 0x0000A922u}; // Japes -> Japes lobby, exit 0.
    dkcoop::State peer{34, 8, 3, dkcoop::active, 7, 8, 9, 10, 11, 12,
        9, 0x000222A9u}; // Japes lobby -> Japes, exit 2.
    CoopTransientInput local_transient{};
    local_transient.enabled = 1; local_transient.file = 0; local_transient.map = 34;
    local_transient.epoch = 7; local_transient.revision = 11; local_transient.count = 1;
    local_transient.records[0] = {COOP_TRANSIENT_TRIGGER, 0x5D, 1, 10};
    CoopTransientInput peer_transient{};
    peer_transient.enabled = 1; peer_transient.file = 0; peer_transient.map = 34;
    peer_transient.epoch = 8; peer_transient.revision = 12; peer_transient.count = 1;
    peer_transient.records[0] = {COOP_TRANSIENT_TRIGGER, 0x5D, 2, 10};
    CoopTraceInput trace{}; trace.version = COOP_TRACE_VERSION;
    trace.flags = COOP_TRACE_PLAYING | COOP_TRACE_ITEM_SAFE_MAP | COOP_TRACE_HUD_READY
        | COOP_TRACE_RECOVERY_CHECKPOINT | COOP_TRACE_PROMOTED_HOST;
    trace.level = 7; trace.item_baseline = trace.item_bound = trace.item_live_snapshot = 1;
    trace.item_wait_reason = COOP_TRACE_WAIT_SAME_LEVEL_ITEM; trace.item_wait_id = 160;
    trace.recovery_state = 7;

    uint64_t now = dkcoop::clock_ms();
    for (unsigned attempt = 0; attempt < 200
            && (session.status() != dkcoop::Status::connected
                || second.status() != dkcoop::Status::connected); ++attempt) {
        now += 10;
        session.tick(player, now, {}, {}, {}, {}, local_transient, trace);
        second.tick(peer, now, {}, {}, {}, {}, peer_transient, {});
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    CHECK(session.status() == dkcoop::Status::connected
        && second.status() == dkcoop::Status::connected);
    for (unsigned attempt = 0; attempt < 20; ++attempt) {
        now += 10;
        session.tick(player, now, {}, {}, {}, {}, local_transient, trace);
        second.tick(peer, now, {}, {}, {}, {}, peer_transient, {});
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    const char malformed[] = "DK64COOP_TRACE_V0";
    CHECK(sendto(query, malformed, int(sizeof(malformed) - 1), 0,
        reinterpret_cast<const sockaddr*>(&target), sizeof(target)) == int(sizeof(malformed) - 1));
    session.tick(player, ++now, {}, {}, {}, {}, local_transient, trace);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    CHECK(no_packet(query));

    constexpr char request[] = "DK64COOP_TRACE_V1";
    CHECK(sendto(query, request, int(sizeof(request) - 1), 0,
        reinterpret_cast<const sockaddr*>(&target), sizeof(target)) == int(sizeof(request) - 1));
    std::string response;
    // The trace worker must answer from the last immutable snapshot even when
    // the emulator/game thread is paused and Session::tick is not running.
    // Give the UDP worker a scheduling point before polling the nonblocking
    // client. Some host schedulers otherwise delay it behind the receive loop.
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    for (unsigned attempt = 0; attempt < 1000 && response.empty(); ++attempt) {
        char buffer[8192];
        int count = int(recvfrom(query, buffer, sizeof(buffer), 0, nullptr, nullptr));
        if (count > 0) response.assign(buffer, size_t(count));
        else std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    if (!response.starts_with("{\"schema\":2,"))
        std::fprintf(stderr, "TRACE RESPONSE %zu queries=%llu rejected=%llu: %.120s\n",
            response.size(), (unsigned long long)session.statistics().trace_queries,
            (unsigned long long)session.statistics().trace_rejected, response.c_str());
    CHECK(response.starts_with("{\"schema\":2,"));
    CHECK(response.find("\"mod\":\"0.78.0\"") != std::string::npos);
    CHECK(response.find("\"role\":\"host\"") != std::string::npos);
    CHECK(response.find("\"lan_discovery\":true") != std::string::npos);
    CHECK(response.find("\"room_fingerprint\":"
        + std::to_string(uint32_t(123456u * 2654435761u)) + ",\"local_ip\":\"") != std::string::npos);
    CHECK(response.find("\"authority\":{\"term\":\"0\",\"node\":\"") != std::string::npos);
    CHECK(response.find("\"map\":34") != std::string::npos);
    CHECK(response.find("\"transition\":{\"local\":{\"ticket\":7,\"route\":43298,\"source\":34,\"destination\":169,\"exit\":0}")
        != std::string::npos);
    CHECK(response.find("\"remote\":{\"ticket\":9,\"route\":139945,\"source\":169,\"destination\":34,\"exit\":2}")
        != std::string::npos);
    CHECK(response.find("\"local\":{\"enabled\":1,\"file\":0,\"map\":34,\"epoch\":7,\"revision\":11,\"count\":1,\"records\":[{\"kind\":5,\"key\":93,\"state\":1,\"value\":10}]")
        != std::string::npos);
    CHECK(response.find("\"remote\":{\"enabled\":1,\"file\":0,\"map\":34,\"epoch\":8,\"revision\":12,\"count\":1,\"records\":[{\"kind\":5,\"key\":93,\"state\":2,\"value\":10}]")
        != std::string::npos);
    CHECK(response.find("\"result\":{\"status\":2,\"map\":34,\"epoch\":7,\"count\":1,\"records\":[{\"kind\":5,\"key\":93,\"state\":2,\"value\":10}]")
        != std::string::npos);
    CHECK(response.find("\"wait_reason\":\"same_level_item\"") != std::string::npos);
    CHECK(response.find("\"wait_item_id\":160") != std::string::npos);
    CHECK(response.find("\"recovery\":{\"checkpoint\":true,\"promoted_host\":true,\"state\":7}")
        != std::string::npos);
    CHECK(session.statistics().trace_queries == 1 && session.statistics().trace_rejected == 1);
    close_test_socket(query);
    close_test_socket(blocker3);
    close_test_socket(blocker2);
    second.stop();
    session.stop();
    std::puts("PASS: private-LAN trace endpoint rejects malformed requests and returns bounded v78 JSON");
}
