#include "session.hpp"
#include "animation.hpp"
#include <atomic>
#include <chrono>
#include <cstring>
#include <mutex>
#include <random>
#include <sstream>
#include <string_view>
#include <thread>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
using Socket = SOCKET;
using SockLen = int;
constexpr Socket invalid_socket = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>
using Socket = int;
using SockLen = socklen_t;
constexpr Socket invalid_socket = -1;
#endif

namespace dkcoop {
namespace {
constexpr uint64_t heartbeat_ms = 50, retry_ms = 500, stale_ms = 750,
    item_stale_ms = 2500, timeout_ms = 3000;
uint64_t random_id() {
    std::random_device random;
    uint64_t result = (uint64_t(random()) << 32) ^ uint64_t(random());
    return result ? result : 1;
}
bool same_peer(const sockaddr_in& a, const sockaddr_in& b) {
    return a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port == b.sin_port;
}
void close_socket(Socket socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}
bool would_block() {
#ifdef _WIN32
    int error = WSAGetLastError(); return error == WSAEWOULDBLOCK;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}
bool private_ipv4(uint32_t address) {
    const uint32_t first = address >> 24;
    const uint32_t second = (address >> 16) & 0xFF;
    return first == 10 || (first == 172 && second >= 16 && second <= 31)
        || (first == 192 && second == 168);
}
bool trace_source_ipv4(uint32_t address) {
    return private_ipv4(address) || (address >> 24) == 127;
}
const char* role_name(Role role) {
    if (role == Role::host) return "host";
    if (role == Role::join) return "join";
    return "off";
}
const char* status_name(Status status) {
    switch (status) {
        case Status::off: return "off";
        case Status::listening: return "listening";
        case Status::connecting: return "connecting";
        case Status::connected: return "connected";
        case Status::error: return "error";
        case Status::busy: return "busy";
    }
    return "unknown";
}
const char* wait_name(uint32_t reason) {
    switch (reason) {
        case COOP_TRACE_WAIT_NONE: return "none";
        case COOP_TRACE_WAIT_LOCAL_AREA: return "local_area";
        case COOP_TRACE_WAIT_REWARD_QUEUE: return "reward_queue";
        case COOP_TRACE_WAIT_HUD: return "hud";
        case COOP_TRACE_WAIT_SAME_LEVEL_ITEM: return "same_level_item";
        case COOP_TRACE_WAIT_SAVE_UNSAFE: return "save_unsafe";
        case COOP_TRACE_WAIT_PROGRESSION_CONTEXT: return "progression_context";
        case COOP_TRACE_WAIT_TROFF: return "troff";
        case COOP_TRACE_WAIT_COUNTER: return "counter";
        case COOP_TRACE_WAIT_FILE: return "file";
        case COOP_TRACE_WAIT_SNAPSHOT: return "snapshot";
        case COOP_TRACE_WAIT_WORLD_REFRESH: return "world_refresh";
        default: return "unknown";
    }
}
bool usable_ipv4(uint32_t address) {
    const uint32_t first = address >> 24;
    return address && address != 0xFFFFFFFFu && first != 127 && first < 224
        && (address & 0xFFFF0000u) != 0xA9FE0000u;
}
uint32_t preferred_local_ipv4() {
    uint32_t selected = 0;
    int selected_rank = -1;
#ifdef _WIN32
    ULONG bytes = 0;
    constexpr ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST
        | GAA_FLAG_SKIP_DNS_SERVER;
    if (GetAdaptersAddresses(AF_INET, flags, nullptr, nullptr, &bytes) != ERROR_BUFFER_OVERFLOW || !bytes)
        return 0;
    std::vector<unsigned char> storage(bytes);
    auto* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(storage.data());
    if (GetAdaptersAddresses(AF_INET, flags, nullptr, adapters, &bytes) != NO_ERROR) return 0;
    for (auto* adapter = adapters; adapter; adapter = adapter->Next) {
        if (adapter->OperStatus != IfOperStatusUp || adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK
                || adapter->IfType == IF_TYPE_TUNNEL) continue;
        const bool lan_adapter = adapter->IfType == IF_TYPE_IEEE80211
            || adapter->IfType == IF_TYPE_ETHERNET_CSMACD;
        for (auto* entry = adapter->FirstUnicastAddress; entry; entry = entry->Next) {
            if (!entry->Address.lpSockaddr || entry->Address.lpSockaddr->sa_family != AF_INET) continue;
            const auto* address = reinterpret_cast<const sockaddr_in*>(entry->Address.lpSockaddr);
            const uint32_t host = ntohl(address->sin_addr.s_addr);
            if (!usable_ipv4(host)) continue;
            const int rank = (private_ipv4(host) ? 4 : 0) + (lan_adapter ? 2 : 0);
            if (rank > selected_rank) { selected = host; selected_rank = rank; }
        }
    }
#else
    ifaddrs* interfaces = nullptr;
    if (getifaddrs(&interfaces) != 0) return 0;
    for (ifaddrs* entry = interfaces; entry; entry = entry->ifa_next) {
        if (!entry->ifa_addr || entry->ifa_addr->sa_family != AF_INET
                || !(entry->ifa_flags & IFF_UP) || (entry->ifa_flags & IFF_LOOPBACK)) continue;
        const auto* address = reinterpret_cast<const sockaddr_in*>(entry->ifa_addr);
        const uint32_t host = ntohl(address->sin_addr.s_addr);
        if (!usable_ipv4(host)) continue;
        const std::string name = entry->ifa_name ? entry->ifa_name : "";
        const bool tunnel_or_cellular = name.starts_with("tun") || name.starts_with("tap")
            || name.starts_with("rmnet") || name.starts_with("ccmni") || name.starts_with("pdp");
        const bool lan_adapter = name.starts_with("wlan") || name.starts_with("wifi")
            || name.starts_with("eth") || name.starts_with("en") || name.starts_with("ap");
        const int rank = (private_ipv4(host) ? 4 : 0) + (lan_adapter ? 2 : 0)
            - (tunnel_or_cellular ? 3 : 0);
        if (rank > selected_rank) { selected = host; selected_rank = rank; }
    }
    freeifaddrs(interfaces);
#endif
    return selected;
}

std::vector<sockaddr_in> discovery_broadcasts(uint16_t port) {
    std::vector<sockaddr_in> result;
    auto add = [&](uint32_t host_address) {
        if (!host_address) return;
        sockaddr_in candidate{}; candidate.sin_family = AF_INET;
        candidate.sin_port = htons(port); candidate.sin_addr.s_addr = htonl(host_address);
        for (const auto& existing : result)
            if (same_peer(existing, candidate)) return;
        result.push_back(candidate);
    };
    add(0xFFFFFFFFu);
#ifdef _WIN32
    ULONG bytes = 0;
    constexpr ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST
        | GAA_FLAG_SKIP_DNS_SERVER;
    if (GetAdaptersAddresses(AF_INET, flags, nullptr, nullptr, &bytes) == ERROR_BUFFER_OVERFLOW && bytes) {
        std::vector<unsigned char> storage(bytes);
        auto* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(storage.data());
        if (GetAdaptersAddresses(AF_INET, flags, nullptr, adapters, &bytes) == NO_ERROR) {
            for (auto* adapter = adapters; adapter; adapter = adapter->Next) {
                if (adapter->OperStatus != IfOperStatusUp
                        || (adapter->IfType != IF_TYPE_IEEE80211
                            && adapter->IfType != IF_TYPE_ETHERNET_CSMACD)) continue;
                for (auto* entry = adapter->FirstUnicastAddress; entry; entry = entry->Next) {
                    if (!entry->Address.lpSockaddr || entry->Address.lpSockaddr->sa_family != AF_INET
                            || entry->OnLinkPrefixLength > 32) continue;
                    const auto* address = reinterpret_cast<const sockaddr_in*>(entry->Address.lpSockaddr);
                    const uint32_t host = ntohl(address->sin_addr.s_addr);
                    const unsigned prefix = entry->OnLinkPrefixLength;
                    const uint32_t mask = prefix ? 0xFFFFFFFFu << (32 - prefix) : 0;
                    add(host | ~mask);
                }
            }
        }
    }
#else
    ifaddrs* interfaces = nullptr;
    if (getifaddrs(&interfaces) == 0) {
        for (ifaddrs* entry = interfaces; entry; entry = entry->ifa_next) {
            if (!entry->ifa_addr || entry->ifa_addr->sa_family != AF_INET
                    || !(entry->ifa_flags & IFF_UP)) continue;
            const std::string name = entry->ifa_name ? entry->ifa_name : "";
            const bool lan_adapter = name.starts_with("wlan") || name.starts_with("wifi")
                || name.starts_with("eth") || name.starts_with("en") || name.starts_with("ap");
            if (!lan_adapter) continue;
            if ((entry->ifa_flags & IFF_BROADCAST) && entry->ifa_broadaddr
                    && entry->ifa_broadaddr->sa_family == AF_INET) {
                const auto* broadcast = reinterpret_cast<const sockaddr_in*>(entry->ifa_broadaddr);
                add(ntohl(broadcast->sin_addr.s_addr));
            }
        }
        freeifaddrs(interfaces);
    }
#endif
    return result;
}

struct TraceWorker {
    Socket socket = invalid_socket;
    uint16_t port = 0;
    std::atomic<bool> stopping{false};
    std::atomic<uint64_t> queries{0}, rejected{0};
    std::mutex response_mutex;
    std::string response = "{\"schema\":1,\"mod\":\"0.60.0\",\"status\":\"starting\"}";
    std::thread thread;

    ~TraceWorker() { stop(); }
    bool start(uint16_t base_port) {
        if (base_port == 65535) return false;
        sockaddr_in local{}; local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        for (uint32_t candidate = uint32_t(base_port) + 1;
                candidate <= uint32_t(base_port) + 8 && candidate <= 65535; ++candidate) {
            Socket candidate_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (candidate_socket == invalid_socket) continue;
            bool configured = true;
#ifdef _WIN32
            u_long nonblocking = 1;
            BOOL exclusive = TRUE;
            if (ioctlsocket(candidate_socket, FIONBIO, &nonblocking) != 0
                    || setsockopt(candidate_socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                        reinterpret_cast<const char*>(&exclusive), sizeof(exclusive)) != 0) {
                configured = false;
            }
#else
            int flags = fcntl(candidate_socket, F_GETFL, 0);
            if (flags < 0 || fcntl(candidate_socket, F_SETFL, flags | O_NONBLOCK) < 0) configured = false;
#endif
            local.sin_port = htons(uint16_t(candidate));
            if (configured && bind(candidate_socket, reinterpret_cast<const sockaddr*>(&local),
                    sizeof(local)) == 0) {
                socket = candidate_socket;
                port = uint16_t(candidate);
                break;
            }
            close_socket(candidate_socket);
        }
        if (socket == invalid_socket) return false;
        thread = std::thread([this] { run(); });
        return true;
    }
    void publish(std::string value) {
        std::lock_guard lock(response_mutex);
        response = std::move(value);
    }
    void run() {
        constexpr std::string_view request = "DK64COOP_TRACE_V1";
        while (!stopping.load(std::memory_order_relaxed)) {
            bool received = false;
            for (unsigned budget = 0; budget < 8; ++budget) {
                char buffer[64]{}; sockaddr_in from{}; SockLen length = sizeof(from);
                int count = int(recvfrom(socket, buffer, sizeof(buffer), 0,
                    reinterpret_cast<sockaddr*>(&from), &length));
                if (count < 0) {
                    if (!would_block()) rejected.fetch_add(1, std::memory_order_relaxed);
                    break;
                }
                received = true;
                const uint32_t source = ntohl(from.sin_addr.s_addr);
                if (!trace_source_ipv4(source) || size_t(count) != request.size()
                        || std::memcmp(buffer, request.data(), request.size()) != 0) {
                    rejected.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }
                std::string snapshot;
                {
                    std::lock_guard lock(response_mutex);
                    snapshot = response;
                }
                if (snapshot.size() <= 4096) {
                    int sent = int(sendto(socket, snapshot.data(), int(snapshot.size()), 0,
                        reinterpret_cast<const sockaddr*>(&from), sizeof(from)));
                    if (sent == int(snapshot.size())) queries.fetch_add(1, std::memory_order_relaxed);
                }
            }
            if (!received) std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    void stop() {
        stopping.store(true, std::memory_order_relaxed);
        if (thread.joinable()) thread.join();
        if (socket != invalid_socket) { close_socket(socket); socket = invalid_socket; }
        port = 0;
    }
};
}
uint64_t clock_ms() {
    return uint64_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}
struct Session::Impl {
    Socket socket = invalid_socket;
    std::unique_ptr<TraceWorker> trace_worker;
    Config config;
    Status status = Status::off;
    std::string error;
    sockaddr_in peer{}, destination{};
    std::vector<sockaddr_in> discovery_targets;
    uint16_t bound_port = 0;
    uint32_t local_ipv4 = 0;
    bool has_peer = false, received_sequence = false, yielded = false;
    uint64_t session = 0, nonce = 0, last_receive = 0, last_state = 0, last_send = 0,
        last_hello = 0, last_authority = 0, last_address_refresh = 0;
    uint32_t send_sequence = 0, receive_sequence = 0, item_page_turn = 0, combat_page_turn = 0;
    State local_player{}, remote{};
    AnimationTimeline animation;
    ProgressWire remote_progress{};
    ProgressInput local_progress{};
    ItemWire remote_items{};
    uint64_t item_page_time[COOP_ITEM_PAGES]{};
    CoopItemInput local_items{};
    CoopWorldInput local_world{};
    WorldWire remote_world{};
    WorldSync world_sync;
    CoopTransientInput local_transient{};
    CoopTraceInput local_trace{};
    TransientWire remote_transient{};
    TransientSync transient_sync;
    CoopCombatFrame remote_combat{};
    CombatSync combat_sync;
    Statistics stats{};
#ifdef _WIN32
    bool winsock = false;
#endif
    bool send(Kind kind, const sockaddr_in& target, const State& local = {}, uint64_t reply_nonce = 0) {
        Packet packet{kind, ++send_sequence,
            kind == Kind::hello || kind == Kind::busy || kind == Kind::authority ? 0 : session,
            reply_nonce ? reply_nonce : nonce, config.room, local};
        packet.authority_term = config.authority_term;
        packet.authority_node = config.authority_node ? config.authority_node : nonce;
        if (kind == Kind::state) packet.progress = progress_wire(config.role == Role::host,
            local_progress, remote_progress, status == Status::connected, session);
        if (kind == Kind::state) {
            const auto& combat = combat_sync.wire();
            packet.combat = combat.enabled && combat.pages
                ? combat_sync.wire((combat_page_turn++ / 2) % combat.pages) : CoopCombatFrame{};
        }
        if (kind == Kind::state) packet.items = items_wire(config.role == Role::host,
            local_items, remote_items, status == Status::connected, session);
        if (kind == Kind::state) {
            packet.world = world_sync.wire();
            if (!packet.items.feature || packet.world.file != packet.items.file) packet.world = {};
            else if (!packet.items.ready) packet.world.ready = 0;
        }
        // Send each page in an adjacent pair. A periodic loss pattern cannot
        // discard both consecutive copies. A complete eight-page snapshot takes
        // about 800 ms; its separate 2.5 s monotonic cache tolerates Android
        // frame scheduling and Wi-Fi jitter while live presence remains 750 ms.
        if (packet.items.ready) {
            packet.items.page = (item_page_turn++ / 2) % COOP_ITEM_PAGES;
        }
        if (kind == Kind::state) packet.transient = transient_sync.wire();
        auto bytes = encode(packet);
        int result = int(sendto(socket, reinterpret_cast<const char*>(bytes.data()), int(bytes.size()), 0,
            reinterpret_cast<const sockaddr*>(&target), sizeof(target)));
        if (result == int(bytes.size())) { stats.sent++; return true; }
        return false; // UDP backpressure drops a snapshot; a later tick retries naturally.
    }
    std::string trace_json(uint64_t now) const {
        char local_address[INET_ADDRSTRLEN] = "0.0.0.0";
        char peer_address[INET_ADDRSTRLEN] = "";
        in_addr local{}; local.s_addr = htonl(local_ipv4);
        inet_ntop(AF_INET, &local, local_address, sizeof(local_address));
        if (has_peer) inet_ntop(AF_INET, &peer.sin_addr, peer_address, sizeof(peer_address));
        uint32_t page_mask = 0;
        std::ostringstream ages;
        ages << '[';
        for (unsigned i = 0; i < COOP_ITEM_PAGES; ++i) {
            if (i) ages << ',';
            if (item_page_time[i] && now >= item_page_time[i]) {
                const uint64_t age = now - item_page_time[i];
                ages << age;
                if (age <= item_stale_ms) page_mask |= 1u << i;
            } else ages << -1;
        }
        ages << ']';
        const auto item_result = items_result(config.role == Role::host, local_items, remote_items,
            status == Status::connected, page_mask == (1u << COOP_ITEM_PAGES) - 1
                && now - last_state <= stale_ms, session);
        const auto world_result = world_sync.result();
        const auto transient_result = transient_sync.result();
        const auto combat_result = combat_sync.result();
        const uint32_t room_fingerprint = config.room * 2654435761u;
        std::ostringstream result;
        char destination_address[INET_ADDRSTRLEN] = "";
        if (config.role == Role::join)
            inet_ntop(AF_INET, &destination.sin_addr, destination_address, sizeof(destination_address));
        result << "{\"schema\":1,\"mod\":\"0.60.0\",\"protocol\":" << protocol_version
            << ",\"role\":\"" << role_name(config.role) << "\",\"status\":\"" << status_name(status)
            << "\",\"room_fingerprint\":" << room_fingerprint
            << ",\"local_ip\":\"" << local_address << "\",\"coop_port\":" << bound_port
            << ",\"trace_port\":" << (trace_worker ? trace_worker->port : 0)
            << ",\"lan_discovery\":" << (!discovery_targets.empty() ? "true" : "false")
            << ",\"destination_ip\":\"" << destination_address << "\",\"peer_ip\":\"" << peer_address
            << "\",\"peer_port\":" << (has_peer ? ntohs(peer.sin_port) : 0)
            << ",\"session\":\"" << std::hex << session << std::dec
            << "\",\"last_receive_ms\":" << (last_receive && now >= last_receive ? now - last_receive : 0)
            << ",\"last_state_ms\":" << (last_state && now >= last_state ? now - last_state : 0)
            << ",\"packets\":{\"sent\":" << stats.sent << ",\"received\":" << stats.received
            << ",\"rejected\":" << stats.rejected << "}"
            << ",\"authority\":{\"term\":\"" << config.authority_term
            << "\",\"node\":\"" << std::hex << config.authority_node << std::dec
            << "\",\"yielded\":" << (yielded ? "true" : "false") << "}"
            << ",\"player\":{\"map\":" << local_player.map << ",\"level\":" << local_trace.level
            << ",\"epoch\":" << local_player.epoch << ",\"kong\":" << local_player.character
            << ",\"state_flags\":" << local_player.flags << ",\"game_flags\":" << local_trace.flags << "}"
            << ",\"remote\":{\"map\":" << remote.map << ",\"epoch\":" << remote.epoch
            << ",\"kong\":" << remote.character << ",\"state_flags\":" << remote.flags << "}"
            << ",\"items\":{\"enabled\":" << local_items.enabled << ",\"file\":" << local_items.file
            << ",\"ready\":" << local_items.ready << ",\"scope\":" << local_items.scope
            << ",\"status\":" << item_result.status << ",\"previous_status\":" << local_trace.item_result_status
            << ",\"deferred\":" << local_trace.item_deferred << ",\"baseline\":" << local_trace.item_baseline
            << ",\"bound\":" << local_trace.item_bound << ",\"live_snapshot\":" << local_trace.item_live_snapshot
            << ",\"fresh_page_mask\":" << page_mask << ",\"page_age_ms\":" << ages.str()
            << ",\"wait_reason\":\"" << wait_name(local_trace.item_wait_reason)
            << "\",\"wait_reason_id\":" << local_trace.item_wait_reason
            << ",\"wait_item_id\":" << local_trace.item_wait_id << "}"
            << ",\"world\":{\"status\":" << world_result.status << ",\"pending\":" << world_result.pending
            << ",\"game_status\":" << local_trace.world_result_status << "}"
            << ",\"transient\":{\"status\":" << transient_result.status
            << ",\"game_status\":" << local_trace.transient_status << "}"
            << ",\"combat\":{\"status\":" << combat_result.status
            << ",\"game_status\":" << local_trace.combat_status << "}"
            << ",\"recovery\":{\"checkpoint\":"
            << ((local_trace.flags & COOP_TRACE_RECOVERY_CHECKPOINT) ? "true" : "false")
            << ",\"promoted_host\":"
            << ((local_trace.flags & COOP_TRACE_PROMOTED_HOST) ? "true" : "false")
            << ",\"state\":" << local_trace.recovery_state << "}}";
        return result.str();
    }
    void clear_peer(uint64_t now) {
        has_peer = false; session = 0; received_sequence = false; remote = {}; remote_progress = {};
        remote_items = {}; remote_world = {}; world_sync.reset();
        local_transient = {}; remote_transient = {}; transient_sync.reset();
        for (auto& time : item_page_time) time = 0;
        animation.reset();
        remote_combat = {}; combat_sync.reset();
        last_receive = last_state = now;
        status = config.role == Role::host ? Status::listening : Status::connecting;
        if (config.role == Role::join) nonce = random_id();
    }
};
Session::Session() : impl(std::make_unique<Impl>()) { }
Session::~Session() { stop(); }
bool Session::start(const Config& config, uint64_t now) {
    stop();
    auto& s = *impl;
    s = Impl{};
    s.config = config;
    if (config.role == Role::off) return true;
    auto fail = [&](const char* reason) { stop(); s.status = Status::error; s.error = reason; return false; };
    if ((config.role != Role::host && config.role != Role::join) || config.room < 100000 || config.room > 999999)
        return fail("Invalid role or six-digit room code");
    if (s.config.role == Role::host && !s.config.authority_node)
        s.config.authority_node = random_id();
#ifdef _WIN32
    WSADATA data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return fail("Winsock initialization failed");
    s.winsock = true;
#endif
    s.socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s.socket == invalid_socket) return fail("Cannot create LAN socket");
#ifdef _WIN32
    u_long nonblocking = 1;
    if (ioctlsocket(s.socket, FIONBIO, &nonblocking) != 0) return fail("Cannot make socket nonblocking");
    BOOL exclusive = TRUE;
    if (config.role == Role::host && setsockopt(s.socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
            reinterpret_cast<const char*>(&exclusive), sizeof(exclusive)) != 0) return fail("Cannot reserve host port");
#else
    int flags = fcntl(s.socket, F_GETFL, 0);
    if (flags < 0 || fcntl(s.socket, F_SETFL, flags | O_NONBLOCK) < 0) return fail("Cannot make socket nonblocking");
#endif
    sockaddr_in local{};
    local.sin_family = AF_INET; local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(config.role == Role::host ? config.port : 0);
    if (bind(s.socket, reinterpret_cast<const sockaddr*>(&local), sizeof(local)) != 0) return fail("Cannot bind LAN port (already in use or denied)");
    SockLen length = sizeof(local);
    if (getsockname(s.socket, reinterpret_cast<sockaddr*>(&local), &length) != 0) return fail("Cannot read bound port");
    s.bound_port = ntohs(local.sin_port);
    s.local_ipv4 = preferred_local_ipv4();
    s.destination.sin_family = AF_INET; s.destination.sin_port = htons(config.port);
    if (config.role == Role::join && (config.port == 0 || inet_pton(AF_INET, config.host_ip.c_str(), &s.destination.sin_addr) != 1
            || s.destination.sin_addr.s_addr == INADDR_ANY || s.destination.sin_addr.s_addr == INADDR_BROADCAST))
        return fail("Join requires the host's numeric IPv4 address and port");
    {
        bool discovery_enabled = false;
#ifdef _WIN32
        BOOL enabled = TRUE;
        discovery_enabled = setsockopt(s.socket, SOL_SOCKET, SO_BROADCAST,
            reinterpret_cast<const char*>(&enabled), sizeof(enabled)) == 0;
#else
        int enabled = 1;
        discovery_enabled = setsockopt(s.socket, SOL_SOCKET, SO_BROADCAST,
            &enabled, sizeof(enabled)) == 0;
#endif
        if (discovery_enabled) s.discovery_targets = discovery_broadcasts(config.port);
    }
    s.nonce = random_id(); s.last_receive = s.last_state = now;
    s.last_address_refresh = now;
    s.last_hello = now >= retry_ms ? now - retry_ms : 0;
    s.status = config.role == Role::host ? Status::listening : Status::connecting;
    if (config.port < 65535) {
        auto worker = std::make_unique<TraceWorker>();
        if (worker->start(config.port)) s.trace_worker = std::move(worker);
    }
    if (s.trace_worker) s.trace_worker->publish(s.trace_json(now));
    return true;
}
void Session::stop() {
    auto& s = *impl;
    if (s.socket != invalid_socket) {
        if (s.has_peer && s.session != 0) s.send(Kind::bye, s.peer);
        close_socket(s.socket); s.socket = invalid_socket;
    }
    s.trace_worker.reset();
#ifdef _WIN32
    if (s.winsock) { WSACleanup(); s.winsock = false; }
#endif
    s.has_peer = false; s.remote = {}; s.remote_progress = {}; s.local_progress = {};
    s.remote_items = {}; s.remote_world = {}; s.world_sync.reset(); for (auto& time : s.item_page_time) time = 0; s.local_items = {};
    s.animation.reset();
    s.remote_combat = {}; s.combat_sync.reset();
    s.local_world = {}; s.local_transient = {}; s.remote_transient = {}; s.transient_sync.reset();
    s.status = Status::off; s.bound_port = 0; s.local_ipv4 = 0;
}
void Session::tick(const State& local, uint64_t now, const ProgressInput& progress, const CoopCombatFrame& combat, const CoopItemInput& items, const CoopWorldInput& world, const CoopTransientInput& transient, const CoopTraceInput& trace) {
    auto& s = *impl;
    if (s.socket == invalid_socket) return;
    State outgoing = valid_state(local) ? local : State{};
    s.local_player = outgoing;
    s.local_progress = valid_progress_input(progress) ? progress : ProgressInput{};
    s.local_items = valid_items_input(items) ? items : CoopItemInput{};
    s.local_world = valid_world_input(world) && s.local_items.enabled
        && world.file == s.local_items.file && (!world.ready || s.local_items.ready) ? world : CoopWorldInput{};
    s.local_transient = valid_transient_input(transient) ? transient : CoopTransientInput{};
    s.local_trace = trace.version == COOP_TRACE_VERSION ? trace : CoopTraceInput{};
    if (now - s.last_address_refresh >= 1000) {
        s.local_ipv4 = preferred_local_ipv4();
        s.last_address_refresh = now;
    }
    if (outgoing.map != japes_map) s.local_progress.ready = 0;
    if (s.has_peer && now - s.last_receive > timeout_ms) s.clear_peer(now);
    if (now - s.last_state > stale_ms) s.remote_progress = {};
    // WELCOME may trigger an immediate reply below; do not pair new player
    // context with last frame's combat payload in that reply.
    s.combat_sync.update(s.config.role == Role::host, outgoing, combat, s.remote, s.remote_combat,
        s.status == Status::connected && now - s.last_state <= stale_ms);
    bool yield_to_new_authority = false;
    sockaddr_in authority_target{};
    uint64_t authority_term = 0, authority_node = 0;
    // The game thread performs bounded, nonblocking work. No networking thread touches game memory.
    for (unsigned budget = 0; budget < 32; budget++) {
        uint8_t buffer[packet_size + 1]; sockaddr_in from{}; SockLen length = sizeof(from);
        int count = int(recvfrom(s.socket, reinterpret_cast<char*>(buffer), sizeof(buffer), 0,
            reinterpret_cast<sockaddr*>(&from), &length));
        if (count < 0) { if (would_block()) break; s.stats.rejected++; continue; }
        Packet packet;
        if (!decode(buffer, size_t(count), packet) || packet.room != s.config.room) { s.stats.rejected++; continue; }
        s.stats.received++;
        if (packet.kind == Kind::authority) {
            if (ntohs(from.sin_port) != s.config.port) { s.stats.rejected++; continue; }
            if (s.config.role == Role::host) {
                if (authority_newer(packet.authority_term, packet.authority_node,
                        s.config.authority_term, s.config.authority_node)) {
                    yield_to_new_authority = true; authority_target = from;
                    authority_term = packet.authority_term; authority_node = packet.authority_node;
                } else if (authority_newer(s.config.authority_term, s.config.authority_node,
                        packet.authority_term, packet.authority_node)) {
                    s.send(Kind::authority, from);
                }
            } else if (!s.has_peer && (!s.config.authority_node
                    || packet.authority_term > s.config.authority_term
                    || (packet.authority_term == s.config.authority_term
                        && packet.authority_node == s.config.authority_node)
                    || authority_newer(packet.authority_term, packet.authority_node,
                        s.config.authority_term, s.config.authority_node))) {
                s.config.authority_term = packet.authority_term;
                s.config.authority_node = packet.authority_node;
                s.destination = from; s.status = Status::connecting;
                s.nonce = random_id(); s.last_hello = now >= retry_ms ? now - retry_ms : 0;
            }
            continue;
        }
        if (s.config.role == Role::host && packet.kind == Kind::hello) {
            if (!s.has_peer) {
                s.peer = from; s.has_peer = true; s.session = random_id(); s.nonce = packet.nonce;
                s.last_receive = now; s.received_sequence = false; s.remote = {};
                s.remote_items = {}; s.remote_world = {}; s.world_sync.reset(); for (auto& time : s.item_page_time) time = 0;
                s.remote_transient = {}; s.transient_sync.reset();
                s.animation.reset();
                s.remote_combat = {}; s.combat_sync.reset();
            }
            if (same_peer(from, s.peer) && packet.nonce == s.nonce) s.send(Kind::welcome, from);
            else { s.stats.rejected++; s.send(Kind::busy, from, {}, packet.nonce); }
            // A new join cannot evict an established/pending peer.
            continue;
        }
        const bool host_source = ntohs(from.sin_port) == s.config.port;
        if (s.config.role == Role::join && !s.has_peer && packet.kind == Kind::busy
                && host_source && packet.nonce == s.nonce) {
            s.status = Status::busy; continue; // Continue retrying; the host may become available.
        }
        if (s.config.role == Role::join && packet.kind == Kind::welcome && host_source
                && packet.nonce == s.nonce && (!s.has_peer || packet.session == s.session)
                && (!s.config.authority_node
                    || packet.authority_term > s.config.authority_term
                    || (packet.authority_term == s.config.authority_term
                        && packet.authority_node == s.config.authority_node)
                    || authority_newer(packet.authority_term, packet.authority_node,
                        s.config.authority_term, s.config.authority_node))) {
            if (!s.has_peer) {
                s.session = packet.session; s.peer = from; s.destination = from; s.has_peer = true;
                s.config.authority_term = packet.authority_term;
                s.config.authority_node = packet.authority_node;
                s.status = Status::connected; s.last_receive = now; s.received_sequence = false;
            }
            s.send(Kind::state, from, outgoing); s.last_send = now;
            continue;
        }
        if (!s.has_peer || !same_peer(from, s.peer) || packet.session != s.session || packet.nonce != s.nonce
                || packet.authority_term != s.config.authority_term
                || packet.authority_node != s.config.authority_node
                || (packet.kind != Kind::state && packet.kind != Kind::bye)
                || (s.received_sequence && !newer(packet.sequence, s.receive_sequence))) { s.stats.rejected++; continue; }
        if (packet.kind == Kind::bye) { s.clear_peer(now); continue; }
        s.received_sequence = true; s.receive_sequence = packet.sequence; s.last_receive = s.last_state = now;
        s.remote = packet.player; s.remote_progress = packet.progress;
        const auto& received = packet.items;
        if (!received.ready || received.file != s.remote_items.file || received.scope != s.remote_items.scope) {
            s.remote_items = {}; for (auto& time : s.item_page_time) time = 0;
        }
        s.remote_items.feature = received.feature; s.remote_items.file = received.file;
        s.remote_items.scope = received.scope; s.remote_items.ready = received.ready;
        if (received.ready) {
            s.item_page_time[received.page] = now;
            for (unsigned i = received.page * COOP_ITEM_PAGE_WORDS; i < (received.page + 1) * COOP_ITEM_PAGE_WORDS; ++i) {
                s.remote_items.owned[i] = received.owned[i]; s.remote_items.request[i] = received.request[i];
            }
        }
        s.remote_combat = packet.combat;
        s.remote_world = packet.world;
        s.remote_transient = packet.transient;
        s.animation.receive(s.remote, now);
        if (s.remote.map != japes_map) s.remote_progress = {};
        s.status = Status::connected;
    }
    s.combat_sync.update(s.config.role == Role::host, outgoing, combat, s.remote, s.remote_combat,
        s.status == Status::connected && now - s.last_state <= stale_ms);
    if (yield_to_new_authority) {
        char address[INET_ADDRSTRLEN]{};
        if (!inet_ntop(AF_INET, &authority_target.sin_addr, address, sizeof(address))) return;
        Config follower = s.config;
        follower.role = Role::join; follower.host_ip = address;
        follower.authority_term = authority_term; follower.authority_node = authority_node;
        if (start(follower, now)) impl->yielded = true;
        return;
    }
    // Keep authority/requests through readiness gaps, but do not process them
    // until both inventories are fresh and free of save-ahead conflicts.
    const auto item_status = this->items(now).status;
    s.world_sync.update(s.config.role == Role::host, s.local_world, s.remote_world,
        s.status == Status::connected, now - s.last_state <= stale_ms,
        item_status == 2 || item_status == 3, s.session);
    s.transient_sync.update(s.config.role == Role::host, outgoing, s.local_transient,
        s.remote, s.remote_transient, s.status == Status::connected,
        now - s.last_state <= stale_ms, s.session);
    if (s.config.role == Role::join && !s.has_peer && now - s.last_hello >= retry_ms) {
        s.send(Kind::hello, s.destination); s.last_hello = now;
        for (const auto& target : s.discovery_targets)
            if (!same_peer(s.destination, target)) s.send(Kind::hello, target);
    }
    if (s.config.role == Role::host && !s.has_peer && now - s.last_authority >= retry_ms) {
        for (const auto& target : s.discovery_targets) s.send(Kind::authority, target);
        s.last_authority = now;
    }
    if (s.status == Status::connected && now - s.last_send >= heartbeat_ms) {
        s.send(Kind::state, s.peer, outgoing); s.last_send = now;
    }
    if (s.trace_worker) s.trace_worker->publish(s.trace_json(now));
}
Status Session::status() const { return impl->status; }
State Session::remote(uint64_t now) const {
    if (impl->status != Status::connected || now - impl->last_state > stale_ms) return {};
    State state = impl->remote;
    state.animation_frame = impl->animation.frame(now);
    return state;
}
ProgressResult Session::progress(uint64_t now) const {
    return progress_result(impl->config.role == Role::host, impl->local_progress, impl->remote_progress,
        impl->status == Status::connected && now - impl->last_state <= stale_ms, impl->session);
}
CoopItemResult Session::items(uint64_t now) const {
    bool pages_fresh = true;
    for (auto time : impl->item_page_time) pages_fresh &= time && now - time <= item_stale_ms;
    return items_result(impl->config.role == Role::host, impl->local_items, impl->remote_items,
        impl->status == Status::connected, pages_fresh && now - impl->last_state <= stale_ms, impl->session);
}
CoopWorldResult Session::world(uint64_t now) const {
    if (impl->status != Status::connected) return {};
    auto result = impl->world_sync.result();
    if (now - impl->last_state > stale_ms) { result.apply = 0; if (result.status) result.status = 1; }
    return result;
}
CoopTransientResult Session::transient(uint64_t now) const {
    if (impl->status != Status::connected || now - impl->last_state > stale_ms) return {};
    return impl->transient_sync.result();
}
CoopCombatResult Session::combat(uint64_t now) const {
    if (impl->status != Status::connected || now - impl->last_state > stale_ms) return {};
    auto result = impl->combat_sync.result();
    if (now - impl->last_state > 150) {
        for (auto& shot : result.shots) shot = {};
        for (auto& motion : result.motion) motion = {};
        result.boss_motion = {};
        result.movement = 0;
    }
    return result;
}
uint16_t Session::bound_port() const { return impl->bound_port; }
uint16_t Session::trace_port() const { return impl->trace_worker ? impl->trace_worker->port : 0; }
uint32_t Session::local_ipv4() const { return impl->local_ipv4; }
Role Session::role() const { return impl->config.role; }
uint64_t Session::authority_term() const { return impl->config.authority_term; }
uint64_t Session::authority_node() const { return impl->config.authority_node; }
bool Session::yielded() const { return impl->yielded; }
bool Session::set_authority(uint64_t term, uint64_t node) {
    if (impl->config.role != Role::host || !node) return false;
    impl->config.authority_term = term; impl->config.authority_node = node;
    return true;
}
const std::string& Session::error() const { return impl->error; }
Statistics Session::statistics() const {
    auto result = impl->stats;
    if (impl->trace_worker) {
        result.trace_queries += impl->trace_worker->queries.load(std::memory_order_relaxed);
        result.trace_rejected += impl->trace_worker->rejected.load(std::memory_order_relaxed);
    }
    return result;
}
}
