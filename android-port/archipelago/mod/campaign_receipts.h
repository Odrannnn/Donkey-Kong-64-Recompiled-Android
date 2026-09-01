// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#if defined(MIPS)
typedef __UINT8_TYPE__ ap_uint8_t;
typedef __UINT16_TYPE__ ap_uint16_t;
typedef __UINT32_TYPE__ ap_uint32_t;
#define AP_OFFSETOF(type, field) __builtin_offsetof(type, field)
#else
#include <stddef.h>
#include <stdint.h>
typedef uint8_t ap_uint8_t;
typedef uint16_t ap_uint16_t;
typedef uint32_t ap_uint32_t;
#define AP_OFFSETOF(type, field) offsetof(type, field)
#endif

// Persisted with the randomized game file. The received-item list remains in
// the native journal; this header identifies which list the counter belongs to.
// Applying one item and incrementing counter must be part of the same game-save
// transaction. This structure is a logical ABI, not a byte-order-neutral disk
// format and must not be memcpy'd directly across the recomp RDRAM boundary.
#define AP_CAMPAIGN_SAVE_MAGIC 0x41504331u /* "APC1" */
#define AP_CAMPAIGN_SAVE_FORMAT 1u
#define AP_CAMPAIGN_BINDING_SIZE 16u

typedef struct {
    ap_uint32_t magic;
    ap_uint16_t format;
    ap_uint16_t counter;
    ap_uint8_t binding[AP_CAMPAIGN_BINDING_SIZE];
    ap_uint32_t reserved;
    ap_uint32_t checksum;
} ap_campaign_save;

static inline ap_uint32_t ap_campaign_crc_byte(ap_uint32_t crc, ap_uint8_t byte) {
    crc ^= byte;
    for (unsigned bit = 0; bit < 8; ++bit) crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    return crc;
}
static inline ap_uint32_t ap_campaign_crc_word(ap_uint32_t crc, ap_uint32_t word, unsigned bytes) {
    for (unsigned i = 0; i < bytes; ++i) crc = ap_campaign_crc_byte(crc, (ap_uint8_t)(word >> (8u * (bytes - 1u - i))));
    return crc;
}
static inline ap_uint32_t ap_campaign_save_checksum(const ap_campaign_save* save) {
    ap_uint32_t crc = ap_campaign_crc_word(0xFFFFFFFFu, save->magic, 4);
    crc = ap_campaign_crc_word(crc, save->format, 2);
    crc = ap_campaign_crc_word(crc, save->counter, 2);
    for (unsigned i = 0; i < AP_CAMPAIGN_BINDING_SIZE; ++i) crc = ap_campaign_crc_byte(crc, save->binding[i]);
    return ~ap_campaign_crc_word(crc, save->reserved, 4);
}
static inline void ap_campaign_save_seal(ap_campaign_save* save) {
    save->reserved = 0;
    save->checksum = ap_campaign_save_checksum(save);
}
static inline int ap_campaign_save_valid(const ap_campaign_save* save) {
    return save->reserved == 0 && save->checksum == ap_campaign_save_checksum(save);
}

#if defined(__cplusplus)
static_assert(sizeof(ap_campaign_save) == 32);
static_assert(AP_OFFSETOF(ap_campaign_save, magic) == 0);
static_assert(AP_OFFSETOF(ap_campaign_save, format) == 4);
static_assert(AP_OFFSETOF(ap_campaign_save, counter) == 6);
static_assert(AP_OFFSETOF(ap_campaign_save, binding) == 8);
static_assert(AP_OFFSETOF(ap_campaign_save, reserved) == 24);
static_assert(AP_OFFSETOF(ap_campaign_save, checksum) == 28);
#else
_Static_assert(sizeof(ap_campaign_save) == 32, "Campaign save ABI layout");
_Static_assert(AP_OFFSETOF(ap_campaign_save, magic) == 0, "Campaign magic offset");
_Static_assert(AP_OFFSETOF(ap_campaign_save, format) == 4, "Campaign format offset");
_Static_assert(AP_OFFSETOF(ap_campaign_save, counter) == 6, "Campaign counter offset");
_Static_assert(AP_OFFSETOF(ap_campaign_save, binding) == 8, "Campaign binding offset");
_Static_assert(AP_OFFSETOF(ap_campaign_save, reserved) == 24, "Campaign reserved offset");
_Static_assert(AP_OFFSETOF(ap_campaign_save, checksum) == 28, "Campaign checksum offset");
#endif
