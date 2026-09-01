// SPDX-License-Identifier: GPL-3.0-or-later
#include "campaign_session.hpp"
#include "ca_bundle.hpp"
#include <ixwebsocket/IXWebSocket.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

namespace dkap {
using CampaignClock = std::chrono::steady_clock;
struct CampaignSession::Impl {
    struct Event { ix::WebSocketMessageType type; std::string text; };
    std::thread worker;
    std::atomic<bool> quit{false}, live{false}, overflow{false};
    std::atomic<uint32_t> published{uint32_t(Status::off) << 16};
    std::array<std::atomic<uint32_t>, AP_LOCATION_WORDS> observed{};
    ap_check_set selected{};
    std::mutex mutex;
    std::condition_variable wake;
    std::deque<Event> events;
    size_t queued_bytes = 0;
    ~Impl() { signal(); if (worker.joinable()) worker.join(); }
    void signal() { quit = true; live = false; published = uint32_t(Status::off) << 16; wake.notify_all(); }
    void publish(Status status, Error error = Error::none) {
        published = uint32_t(error) << 24 | uint32_t(status) << 16;
    }
    ap_check_set observations() const {
        ap_check_set result{};
        for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i) result.words[i] = observed[i].load();
        return result;
    }
    void run(Config config, CampaignSeed seed, CampaignChannel* channel) {
        try {
            CampaignProtocol state(config, seed, *channel);
            unsigned backoff = 1;
            while (!quit) {
                state.disconnect(); live = false; overflow = false;
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
                tls.caFile = ca_bundle;
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
                            std::string text = type == ix::WebSocketMessageType::Message ? message->str : "";
                            queued_bytes += text.size(); events.push_back({type, std::move(text)});
                        }
                    } catch (...) { overflow = true; }
                    wake.notify_all();
                });
                socket.start();
                bool retry = false;
                auto deadline = CampaignClock::now() + std::chrono::seconds(30);
                auto next_flush = CampaignClock::now();
                auto send = [&](const std::vector<Json>& commands) {
                    if (commands.empty()) return;
                    if (!live || socket.bufferedAmount() > message_limit
                        || !socket.sendText(Json(commands).dump()).success) retry = true;
                };
                while (!quit && !retry) {
                    std::deque<Event> pending;
                    {
                        std::unique_lock lock(mutex);
                        wake.wait_for(lock, std::chrono::milliseconds(100),
                            [&] { return quit || overflow || !events.empty(); });
                        pending.swap(events); queued_bytes = 0;
                    }
                    if (quit) break;
                    if (overflow) throw Failure(Error::protocol);
                    state.observe(observations());
                    for (auto& event : pending) {
                        if (event.type == ix::WebSocketMessageType::Close
                            || event.type == ix::WebSocketMessageType::Error) { retry = true; break; }
                        if (event.type == ix::WebSocketMessageType::Message) send(state.receive(event.text));
                        if (retry) break;
                    }
                    if (state.ready() && live && !retry) {
                        backoff = 1;
                        deadline = CampaignClock::now() + std::chrono::seconds(30);
                        publish(Status::ready);
                        if (CampaignClock::now() >= next_flush) {
                            send(state.flush()); next_flush = CampaignClock::now() + std::chrono::seconds(1);
                        }
                    } else {
                        publish(Status::waiting);
                        if (CampaignClock::now() >= deadline) retry = true;
                    }
                }
                live = false; socket.stop();
                if (quit) break;
                publish(Status::waiting, Error::transport);
                auto until = CampaignClock::now() + std::chrono::seconds(backoff);
                backoff = std::min(backoff * 2, 30u);
                while (!quit && CampaignClock::now() < until) {
                    state.observe(observations());
                    std::unique_lock lock(mutex);
                    wake.wait_for(lock, std::chrono::milliseconds(100), [&] { return quit.load(); });
                }
            }
            state.observe(observations());
            publish(Status::off);
        } catch (const Failure& failure) { live = false; publish(Status::rejected, failure.code); }
        catch (...) { live = false; publish(Status::rejected, Error::protocol); }
    }
};

CampaignSession::CampaignSession() = default;
CampaignSession::~CampaignSession() { stop(); }
void CampaignSession::start(const Config& config, const CampaignSeed& seed, CampaignChannel& channel) {
    std::lock_guard lifetime_lock(lifetime_mutex);
    if (impl) { impl->signal(); if (impl->worker.joinable()) impl->worker.join(); impl.reset(); }
    auto next = std::make_shared<Impl>();
    for (auto id : seed.locations)
        if (id < 0 || id > UINT32_MAX || !ap_check_add(&next->selected, uint32_t(id))) throw Failure(Error::config);
    next->publish(Status::waiting);
    next->worker = std::thread([ptr = next, config, seed, &channel] { ptr->run(config, seed, &channel); });
    impl = std::move(next);
}
void CampaignSession::stop() {
    std::lock_guard lifetime_lock(lifetime_mutex);
    if (impl) { impl->signal(); if (impl->worker.joinable()) impl->worker.join(); impl.reset(); }
}
uint32_t CampaignSession::tick(const ap_check_set& checks) {
    std::shared_ptr<Impl> current;
    {
        std::unique_lock lifetime_lock(lifetime_mutex, std::try_to_lock);
        if (!lifetime_lock.owns_lock()) return uint32_t(Status::waiting) << 16;
        current = impl;
    }
    if (!current) return uint32_t(Status::off) << 16;
    if (!ap_check_valid(&checks)) return uint32_t(Error::protocol) << 24 | uint32_t(Status::rejected) << 16;
    for (unsigned i = 0; i < AP_LOCATION_WORDS; ++i) {
        if (checks.words[i] & ~current->selected.words[i])
            return uint32_t(Error::protocol) << 24 | uint32_t(Status::rejected) << 16;
        current->observed[i].fetch_or(checks.words[i]);
    }
    auto word = current->published.load();
    if (((word >> 16) & 3) == uint32_t(Status::ready) && !current->live)
        return uint32_t(Status::waiting) << 16;
    return word;
}
}
