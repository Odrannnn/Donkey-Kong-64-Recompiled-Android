#include "session.hpp"
#include "animation.hpp"
#include <chrono>
#include <cstring>
#include <random>
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
}
uint64_t clock_ms() {
    return uint64_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}
struct Session::Impl {
    Socket socket = invalid_socket;
    Config config;
    Status status = Status::off;
    std::string error;
    sockaddr_in peer{}, destination{};
    uint16_t bound_port = 0;
    uint32_t local_ipv4 = 0;
    bool has_peer = false, received_sequence = false;
    uint64_t session = 0, nonce = 0, last_receive = 0, last_state = 0, last_send = 0, last_hello = 0;
    uint32_t send_sequence = 0, receive_sequence = 0, item_page_turn = 0, combat_page_turn = 0;
    State remote{};
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
    TransientWire remote_transient{};
    TransientSync transient_sync;
    CoopCombatFrame remote_combat{};
    CombatSync combat_sync;
    Statistics stats{};
#ifdef _WIN32
    bool winsock = false;
#endif
    bool send(Kind kind, const sockaddr_in& target, const State& local = {}, uint64_t reply_nonce = 0) {
        Packet packet{kind, ++send_sequence, kind == Kind::hello || kind == Kind::busy ? 0 : session,
            reply_nonce ? reply_nonce : nonce, config.room, local};
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
    if (config.role == Role::host) s.local_ipv4 = preferred_local_ipv4();
    s.destination.sin_family = AF_INET; s.destination.sin_port = htons(config.port);
    if (config.role == Role::join && (config.port == 0 || inet_pton(AF_INET, config.host_ip.c_str(), &s.destination.sin_addr) != 1
            || s.destination.sin_addr.s_addr == INADDR_ANY || s.destination.sin_addr.s_addr == INADDR_BROADCAST))
        return fail("Join requires the host's numeric IPv4 address and port");
    s.nonce = random_id(); s.last_receive = s.last_state = now;
    s.last_hello = now >= retry_ms ? now - retry_ms : 0;
    s.status = config.role == Role::host ? Status::listening : Status::connecting;
    return true;
}
void Session::stop() {
    auto& s = *impl;
    if (s.socket != invalid_socket) {
        if (s.has_peer && s.session != 0) s.send(Kind::bye, s.peer);
        close_socket(s.socket); s.socket = invalid_socket;
    }
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
void Session::tick(const State& local, uint64_t now, const ProgressInput& progress, const CoopCombatFrame& combat, const CoopItemInput& items, const CoopWorldInput& world, const CoopTransientInput& transient) {
    auto& s = *impl;
    if (s.socket == invalid_socket) return;
    State outgoing = valid_state(local) ? local : State{};
    s.local_progress = valid_progress_input(progress) ? progress : ProgressInput{};
    s.local_items = valid_items_input(items) ? items : CoopItemInput{};
    s.local_world = valid_world_input(world) && s.local_items.enabled
        && world.file == s.local_items.file && (!world.ready || s.local_items.ready) ? world : CoopWorldInput{};
    s.local_transient = valid_transient_input(transient) ? transient : CoopTransientInput{};
    if (outgoing.map != japes_map) s.local_progress.ready = 0;
    if (s.has_peer && now - s.last_receive > timeout_ms) s.clear_peer(now);
    if (now - s.last_state > stale_ms) s.remote_progress = {};
    // WELCOME may trigger an immediate reply below; do not pair new player
    // context with last frame's combat payload in that reply.
    s.combat_sync.update(s.config.role == Role::host, outgoing, combat, s.remote, s.remote_combat,
        s.status == Status::connected && now - s.last_state <= stale_ms);
    // The game thread performs bounded, nonblocking work. No networking thread touches game memory.
    for (unsigned budget = 0; budget < 32; budget++) {
        uint8_t buffer[packet_size + 1]; sockaddr_in from{}; SockLen length = sizeof(from);
        int count = int(recvfrom(s.socket, reinterpret_cast<char*>(buffer), sizeof(buffer), 0,
            reinterpret_cast<sockaddr*>(&from), &length));
        if (count < 0) { if (would_block()) break; s.stats.rejected++; continue; }
        Packet packet;
        if (!decode(buffer, size_t(count), packet) || packet.room != s.config.room) { s.stats.rejected++; continue; }
        s.stats.received++;
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
        if (s.config.role == Role::join && !s.has_peer && packet.kind == Kind::busy
                && same_peer(from, s.destination) && packet.nonce == s.nonce) {
            s.status = Status::busy; continue; // Continue retrying; the host may become available.
        }
        if (s.config.role == Role::join && packet.kind == Kind::welcome && same_peer(from, s.destination)
                && packet.nonce == s.nonce && (!s.has_peer || packet.session == s.session)) {
            if (!s.has_peer) {
                s.session = packet.session; s.peer = from; s.has_peer = true;
                s.status = Status::connected; s.last_receive = now; s.received_sequence = false;
            }
            s.send(Kind::state, from, outgoing); s.last_send = now;
            continue;
        }
        if (!s.has_peer || !same_peer(from, s.peer) || packet.session != s.session || packet.nonce != s.nonce
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
    }
    if (s.status == Status::connected && now - s.last_send >= heartbeat_ms) {
        s.send(Kind::state, s.peer, outgoing); s.last_send = now;
    }
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
uint32_t Session::local_ipv4() const { return impl->local_ipv4; }
const std::string& Session::error() const { return impl->error; }
Statistics Session::statistics() const { return impl->stats; }
}
