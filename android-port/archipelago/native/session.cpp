// SPDX-License-Identifier: GPL-3.0-or-later
#include "session.hpp"
#include "ca_bundle.hpp"
#include <ixwebsocket/IXWebSocket.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

namespace dkap {
using Clock = std::chrono::steady_clock;
struct Session::Impl {
    struct Event { ix::WebSocketMessageType type; std::string text; };
    std::thread worker;
    std::atomic<bool> quit{false}, live{false}, overflow{false};
    std::atomic<uint32_t> observed{0}, published{uint32_t(Status::off) << 16};
    std::mutex mutex;
    mutable std::mutex scout_mutex;
    ScoutSnapshot scouts;
    std::condition_variable wake;
    std::deque<Event> events;
    size_t queued_bytes = 0;
    ~Impl() { stop(); if (worker.joinable()) worker.join(); }
    void stop() { quit = true; live = false; published = uint32_t(Status::off) << 16; wake.notify_all(); }
    void publish(Status status, Error error = Error::none, uint32_t moves = 0) {
        published = uint32_t(error) << 24 | uint32_t(status) << 16 | moves;
    }
    void publish_scouts(const ScoutSnapshot& value) { std::lock_guard lock(scout_mutex); scouts = value; }
    void run(Config config, std::filesystem::path path) {
        try {
            State state(config, path);
            unsigned backoff = 1;
            while (!quit) {
                state.disconnect(); publish_scouts(state.scout_snapshot()); live = false; overflow = false;
                { std::lock_guard lock(mutex); events.clear(); queued_bytes = 0; }
                publish(Status::waiting);
                ix::WebSocket socket;
                socket.setUrl(config.server);
                socket.disableAutomaticReconnection();
                socket.disablePerMessageDeflate();
                socket.setAutoThreadName(false);
                socket.setHandshakeTimeout(10);
                socket.setPingInterval(10);
                ix::SocketTLSOptions tls;
                tls.caFile = ca_bundle; // Embedded Mozilla roots; certificate and hostname checks stay enabled.
                socket.setTLSOptions(tls);
                socket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& message) {
                    auto type = message->type;
                    if (type == ix::WebSocketMessageType::Open) live = true;
                    if (type == ix::WebSocketMessageType::Close || type == ix::WebSocketMessageType::Error) live = false;
                    if (type != ix::WebSocketMessageType::Open && type != ix::WebSocketMessageType::Close
                        && type != ix::WebSocketMessageType::Error && type != ix::WebSocketMessageType::Message) return;
                    try {
                        std::lock_guard lock(mutex);
                        if (message->binary || message->str.size() > message_limit || events.size() >= 32
                            || queued_bytes + message->str.size() > 2 * message_limit) overflow = true;
                        else {
                            // Do not retain transport error strings: they may contain a user URL.
                            std::string text = type == ix::WebSocketMessageType::Message ? message->str : "";
                            queued_bytes += text.size(); events.push_back({type, std::move(text)});
                        }
                    } catch (...) { overflow = true; }
                    wake.notify_all();
                });
                socket.start();
                bool retry = false;
                auto deadline = Clock::now() + std::chrono::seconds(30), next_flush = Clock::now();
                auto send = [&](const std::vector<Json>& commands) {
                    if (commands.empty()) return;
                    if (!live || socket.bufferedAmount() > message_limit || !socket.sendText(Json(commands).dump()).success) retry = true;
                };
                while (!quit && !retry) {
                    std::deque<Event> pending;
                    {
                        std::unique_lock lock(mutex);
                        wake.wait_for(lock, std::chrono::milliseconds(100), [&] { return quit || overflow || !events.empty(); });
                        pending.swap(events); queued_bytes = 0;
                    }
                    if (quit) break;
                    if (overflow) throw Failure(Error::protocol);
                    state.record_checks(observed.load());
                    for (auto& event : pending) {
                        if (event.type == ix::WebSocketMessageType::Close || event.type == ix::WebSocketMessageType::Error) { retry = true; break; }
                        if (event.type == ix::WebSocketMessageType::Message) {
                            send(state.receive(event.text)); publish_scouts(state.scout_snapshot());
                        }
                        if (retry) break;
                    }
                    if (state.ready() && live && !retry) {
                        backoff = 1;
                        deadline = Clock::now() + std::chrono::seconds(30);
                        publish(Status::ready, Error::none, state.moves());
                        if (Clock::now() >= next_flush) { send(state.flush()); next_flush = Clock::now() + std::chrono::seconds(1); }
                    } else {
                        publish(Status::waiting);
                        if (Clock::now() >= deadline) retry = true;
                    }
                }
                live = false; socket.stop(); // Joining and disk I/O happen only on this worker.
                if (quit) break;
                publish(Status::waiting, Error::transport);
                auto until = Clock::now() + std::chrono::seconds(backoff);
                backoff = std::min(backoff * 2, 30u);
                while (!quit && Clock::now() < until) {
                    state.record_checks(observed.load());
                    std::unique_lock lock(mutex);
                    wake.wait_for(lock, std::chrono::milliseconds(100), [&] { return quit.load(); });
                }
            }
            state.record_checks(observed.load());
            publish(Status::off);
        } catch (const Failure& failure) { live = false; publish(Status::rejected, failure.code); }
        catch (...) { live = false; publish(Status::rejected, Error::protocol); }
    }
};
Session::Session() = default;
Session::~Session() = default;
void Session::start(const Config& config, const std::filesystem::path& path) {
    validate_config(config);
    impl.reset(); // Called only at startup, never from a frame callback.
    impl = std::make_unique<Impl>();
    impl->publish(Status::waiting);
    impl->worker = std::thread([ptr = impl.get(), config, path] { ptr->run(config, path); });
}
void Session::request_stop() { if (impl) impl->stop(); }
uint32_t Session::tick(uint32_t checks) {
    if (!impl) return uint32_t(Status::off) << 16;
    if (checks & ~check_mask) return uint32_t(Error::protocol) << 24 | uint32_t(Status::rejected) << 16;
    impl->observed.fetch_or(checks);
    auto word = impl->published.load();
    if (((word >> 16) & 3) == uint32_t(Status::ready) && !impl->live) return uint32_t(Status::waiting) << 16;
    return word;
}
ScoutSnapshot Session::scout_snapshot() const {
    if (!impl) return {};
    std::lock_guard lock(impl->scout_mutex);
    return impl->scouts;
}
bool Session::try_scout_snapshot(ScoutSnapshot& output) const {
    if (!impl) { output = {}; return true; }
    std::unique_lock lock(impl->scout_mutex, std::try_to_lock);
    if (!lock.owns_lock()) return false;
    output = impl->scouts;
    return true;
}
}
