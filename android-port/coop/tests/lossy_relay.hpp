#pragma once
// Real UDP relay: lose one third of datagrams and replay older states after newer
// states. Both production Session objects still validate their actual endpoints.
struct LossyRelay {
#ifdef _WIN32
    SOCKET socket = INVALID_SOCKET;
    using Length = int;
#else
    int socket = -1;
    using Length = socklen_t;
#endif
    sockaddr_in host{}, guest{}, delayed_target{};
    Bytes delayed{};
    bool have_delayed = false;
    uint16_t port = 0;
    unsigned datagrams = 0, dropped = 0, replayed = 0;
    int blocked_item_page = -1;
    explicit LossyRelay(uint16_t host_port) {
#ifdef _WIN32
        WSADATA data{}; CHECK(WSAStartup(MAKEWORD(2, 2), &data) == 0);
#endif
        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
        CHECK(socket != INVALID_SOCKET); u_long mode = 1;
        CHECK(ioctlsocket(socket, FIONBIO, &mode) == 0);
#else
        CHECK(socket >= 0); CHECK(fcntl(socket, F_SETFL, O_NONBLOCK) == 0);
#endif
        sockaddr_in local{}; local.sin_family = AF_INET; local.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        CHECK(bind(socket, reinterpret_cast<sockaddr*>(&local), sizeof(local)) == 0);
        Length length = sizeof(local); CHECK(getsockname(socket, reinterpret_cast<sockaddr*>(&local), &length) == 0);
        port = ntohs(local.sin_port); host = local; host.sin_port = htons(host_port);
    }
    ~LossyRelay() {
#ifdef _WIN32
        closesocket(socket); WSACleanup();
#else
        close(socket);
#endif
    }
    void forward() {
        for (unsigned budget = 0; budget < 32; budget++) {
            Bytes bytes{}; sockaddr_in from{}; Length length = sizeof(from);
            int count = int(recvfrom(socket, reinterpret_cast<char*>(bytes.data()), int(bytes.size()), 0,
                reinterpret_cast<sockaddr*>(&from), &length));
            if (count < 0) return;
            CHECK(count == int(bytes.size()));
            sockaddr_in target{};
            if (from.sin_port == host.sin_port) target = guest;
            else { guest = from; target = host; }
            if (!target.sin_port) continue;
            Packet decoded{}; CHECK(decode(bytes.data(), bytes.size(), decoded));
            if (decoded.kind == Kind::state && decoded.items.feature && int(decoded.items.page) == blocked_item_page) { ++dropped; continue; }
            if (++datagrams % 3 == 0) { dropped++; continue; }
            auto send = [&](const Bytes& b, const sockaddr_in& to) {
                CHECK(sendto(socket, reinterpret_cast<const char*>(b.data()), int(b.size()), 0,
                    reinterpret_cast<const sockaddr*>(&to), sizeof(to)) == int(b.size()));
            };
            send(bytes, target);
            Packet packet{}; CHECK(decode(bytes.data(), bytes.size(), packet));
            if (packet.kind == Kind::state) {
                if (have_delayed) { send(delayed, delayed_target); replayed++; }
                delayed = bytes; delayed_target = target; have_delayed = true;
            }
        }
    }
};
