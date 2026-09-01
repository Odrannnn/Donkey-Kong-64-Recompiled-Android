// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DKAP_POLICY_H
#define DKAP_POLICY_H
/* Vanilla uses a LEVEL (0..3), not the randomizer's three independent move bits.
 * Only each Kong's first move is supported until the gameplay checks are ported. */
static unsigned char ap_first_move_level(unsigned char current, unsigned granted) {
    return current == 0 && granted ? 1 : current;
}
static int ap_safe_frame(unsigned adventure, unsigned file, unsigned player,
                         unsigned cutscene, unsigned transition, unsigned paused) {
    return adventure && file == 0 && player && !cutscene && !transition && !paused;
}
static int ap_owns_save_path(const unsigned char* path, const char* save_name) {
    const char* owner = "odrannnn_dk64_archipelago/";
    const char* extension = ".bin";
    unsigned length = 0, owner_len = 0;
    if (!path) return 0;
    while (path[length]) ++length;
    while (owner[owner_len]) ++owner_len;
    unsigned suffix_len = owner_len + 35 + 4;
    if (length <= suffix_len) return 0;
    unsigned start = length - suffix_len;
    if (path[start-1] != '/' && path[start-1] != '\\') return 0;
    for (unsigned i = 0; i < owner_len; ++i) {
        char c = path[start+i]; if (c == '\\') c = '/';
        if (c != owner[i]) return 0;
    }
    for (unsigned i = 0; i < 35; ++i)
        if (path[start+owner_len+i] != save_name[i]) return 0;
    for (unsigned i = 0; i < 4; ++i)
        if (path[length-4+i] != extension[i]) return 0;
    return 1;
}
#endif
