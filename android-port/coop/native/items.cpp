#include "items.hpp"
namespace dkcoop {
static_assert(COOP_ITEM_PAGE_WORDS * COOP_ITEM_PAGES == COOP_ITEM_WORDS);
static_assert(sizeof(unsigned) == 4 && sizeof(CoopItemInput) == 1560 && sizeof(CoopItemResult) == 1552);
std::array<uint32_t, COOP_ITEM_WIRE_WORDS> item_words(const ItemWire& w) {
    std::array<uint32_t, COOP_ITEM_WIRE_WORDS> out{w.feature, w.file, w.ready, w.scope};
    out[4] = w.page;
    if (w.page >= COOP_ITEM_PAGES) return out;
    for (unsigned i = 0; i < COOP_ITEM_PAGE_WORDS; ++i) {
        out[5 + i] = w.owned[w.page * COOP_ITEM_PAGE_WORDS + i];
        out[5 + COOP_ITEM_PAGE_WORDS + i] = w.request[w.page * COOP_ITEM_PAGE_WORDS + i];
    }
    return out;
}
ItemWire items_from_words(const std::array<uint32_t, COOP_ITEM_WIRE_WORDS>& w) {
    ItemWire out{w[0], w[1], w[2], w[3]}; out.page = w[4];
    if (out.page >= COOP_ITEM_PAGES) return out;
    for (unsigned i = 0; i < COOP_ITEM_PAGE_WORDS; ++i) {
        out.owned[out.page * COOP_ITEM_PAGE_WORDS + i] = w[5 + i];
        out.request[out.page * COOP_ITEM_PAGE_WORDS + i] = w[5 + COOP_ITEM_PAGE_WORDS + i];
    }
    return out;
}
bool valid_items(const ItemWire& w) {
    if (w.page >= COOP_ITEM_PAGES) return false;
    if (!w.feature) {
        for (auto word : item_words(w)) if (word) return false;
        for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) if (w.owned[i] || w.request[i]) return false;
        return true;
    }
    if (w.feature != 1 || w.file > 3 || w.ready > 1 || w.scope > 3 || (w.ready && !w.file)
            || !coop_items_dependencies(w.owned)) return false;
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) {
        if (!w.ready && (w.owned[i] || w.request[i])) return false;
        if (w.request[i] & ~w.owned[i]) return false; // A pre-write event alone is not a collection.
    }
    return true;
}
bool valid_items_input(const CoopItemInput& l) {
    // enabled == 2 is a host-only local policy: accept validated guest-owned
    // progress into the host save. It is serialized on the wire as feature 1.
    return l.enabled <= 2 && l.ready <= 1 && l.file <= 3 && l.scope <= 3
        && (!l.ready || (l.enabled && l.file && coop_items_full_dependencies(l.owned)));
}
static bool bound(const CoopItemInput& l, const ItemWire& r, uint64_t session) {
    return session && l.session_hi == uint32_t(session >> 32) && l.session_lo == uint32_t(session)
        && r.scope == r.file && l.scope == r.file && l.scope;
}
ItemWire items_wire(bool host, const CoopItemInput& l, const ItemWire& r, bool connected, uint64_t session) {
    if (!valid_items_input(l) || !l.enabled) return {};
    ItemWire w{1, l.file, l.ready, host ? l.file : 0};
    if (!host && connected && valid_items(r) && r.feature && bound(l, r, session)) w.scope = r.file;
    if (!l.ready) return w;
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) {
        w.owned[i] = l.owned[i];
        if (!host && w.scope) w.request[i] = l.request[i] & l.owned[i];
    }
    return w;
}
CoopItemResult items_result(bool host, const CoopItemInput& l, const ItemWire& r,
        bool connected, bool fresh, uint64_t session) {
    if (!valid_items_input(l) || !l.enabled) return {};
    CoopItemResult out{}; out.status = 1;
    if (!connected || !session || !valid_items(r) || !r.feature || !r.file || !l.file) return out;
    // Preserve pending requests over a short gap or map transition, but never
    // issue writes or acknowledge from stale snapshots. A new session clears them.
    out.session_hi = uint32_t(session >> 32); out.session_lo = uint32_t(session);
    out.scope = host ? l.file : r.file;
    if (!fresh || !l.ready || !r.ready || (host ? r.scope != l.file : r.scope != r.file)) return out;
    // Individual packets contain one page; require cross-page prerequisites only
    // on the assembled, fresh snapshot. A reordered page can briefly defer this.
    if (!coop_items_full_dependencies(r.owned)) return out;
    if (!host && !bound(l, r, session)) return out;
    bool different = false;
    const bool merge_guest = host && l.enabled == 2;
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) {
        const unsigned extra = host ? r.owned[i] & ~l.owned[i] & ~r.request[i]
            : l.owned[i] & ~r.owned[i] & ~l.request[i];
        if ((extra && !merge_guest) || (!host && r.request[i])) { out.status = 4; return out; }
        different |= l.owned[i] != r.owned[i];
    }
    out.status = different ? 2 : 3;
    for (unsigned i = 0; i < COOP_ITEM_WORDS; ++i) {
        out.apply[i] = (host ? (merge_guest ? r.owned[i] : r.request[i] & r.owned[i]) : r.owned[i]) & ~l.owned[i];
        out.acknowledged[i] = host ? r.owned[i] & l.owned[i] : r.owned[i];
    }
    return out;
}
}
