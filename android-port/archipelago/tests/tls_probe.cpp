// SPDX-License-Identifier: GPL-3.0-or-later
// Test the same TLS backend with a disposable test CA (not exposed by the mod).
#include <ixwebsocket/IXWebSocket.h>
#include <atomic>
#include <chrono>
#include <thread>
int main(int argc, char** argv) {
    if (argc != 4) return 2;
    std::atomic<int> result{0};
    ix::WebSocket socket;
    socket.setUrl(argv[1]); socket.disableAutomaticReconnection(); socket.disablePerMessageDeflate();
    ix::SocketTLSOptions tls; tls.caFile = argv[2]; socket.setTLSOptions(tls);
    socket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& message) {
        if (message->type == ix::WebSocketMessageType::Open) result = 1;
        if (message->type == ix::WebSocketMessageType::Error) result = 2;
    });
    socket.start();
    for (int i = 0; i < 200 && !result; ++i) std::this_thread::sleep_for(std::chrono::milliseconds(50));
    socket.stop();
    return result == std::stoi(argv[3]) ? 0 : 1;
}
