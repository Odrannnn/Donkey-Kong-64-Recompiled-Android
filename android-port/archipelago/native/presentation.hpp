// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <algorithm>
#include "campaign.hpp"
#include "../mod/shop_labels.h"
namespace dkap {
// Preparation for the future seed-bound LocationInfo path, not a new live ABI.
// Unknown glyphs are omitted; labels consisting entirely of unsupported glyphs
// get explicit fallbacks. Names are never interpreted as formatter instructions.
inline ap_shop_label make_shop_label(uint32_t location, const std::string& item, const std::string& recipient, uint16_t frames = 130) {
    int index = ap_location_index(location);
    if (index < 0 || ap_locations[index].detector != AP_DETECT_SHOP || frames < 50 || frames > 255
        || item.size() > 512 || recipient.size() > 512) throw Failure(Error::config);
    auto clean = [](const std::string& input, const char* fallback) {
        std::string output;
        for (unsigned char c : input) {
            if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
            if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) output += char(c);
            else if (c == ' ' && !output.empty() && output.back() != ' ') output += ' ';
        }
        if (!output.empty() && output.back() == ' ') output.pop_back();
        return output.empty() ? std::string(fallback) : output;
    };
    auto title = clean(item, "UNKNOWN ITEM").substr(0, 32);
    auto subtitle = ("TO " + clean(recipient, "UNKNOWN PLAYER")).substr(0, 32);
    ap_shop_label result{}; result.location = location; result.frames = frames;
    std::copy(title.begin(), title.end(), result.item);
    std::copy(subtitle.begin(), subtitle.end(), result.subtitle);
    return result;
}
}
