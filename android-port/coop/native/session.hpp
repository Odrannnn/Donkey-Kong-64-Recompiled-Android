#pragma once
#include "protocol.hpp"
#include "../mod/trace_types.h"
#include <memory>
#include <string>

namespace dkcoop {
enum class Role : uint32_t { off = 0, host = 1, join = 2 };
enum class Status : uint32_t { off = 0, listening = 1, connecting = 2, connected = 3, error = 4, busy = 5 };
struct Config {
    Role role = Role::off;
    std::string host_ip;
    uint16_t port = 6464;
    uint32_t room = 123456;
    uint64_t authority_term = 0, authority_node = 0;
};
struct Statistics { uint64_t sent = 0, received = 0, rejected = 0, trace_queries = 0, trace_rejected = 0; };
class Session {
public:
    Session();
    ~Session();
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    bool start(const Config& config, uint64_t now_ms);
    void tick(const State& local, uint64_t now_ms, const ProgressInput& progress = {}, const CoopCombatFrame& combat = {}, const CoopItemInput& items = {}, const CoopWorldInput& world = {}, const CoopTransientInput& transient = {}, const CoopTraceInput& trace = {});
    void stop();
    Status status() const;
    State remote(uint64_t now_ms) const;
    ProgressResult progress(uint64_t now_ms) const;
    CoopCombatResult combat(uint64_t now_ms) const;
    CoopItemResult items(uint64_t now_ms) const;
    CoopWorldResult world(uint64_t now_ms) const;
    CoopTransientResult transient(uint64_t now_ms) const;
    uint16_t bound_port() const;
    uint16_t trace_port() const;
    uint32_t local_ipv4() const;
    Role role() const;
    uint64_t authority_term() const;
    uint64_t authority_node() const;
    bool yielded() const;
    bool set_authority(uint64_t term, uint64_t node);
    const std::string& error() const;
    Statistics statistics() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
uint64_t clock_ms();
}
