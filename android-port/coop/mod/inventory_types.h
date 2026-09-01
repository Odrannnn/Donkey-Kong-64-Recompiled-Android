#ifndef COOP_INVENTORY_TYPES_H
#define COOP_INVENTORY_TYPES_H
// Minimal inventory view of the pinned US game. The patches' common_structs.h
// omits these decomp types. No pointer or entire save buffer is sent to a peer.
// Snide's recompiled routine uses Kong stride 0x5E and GB offset 0x42.
typedef struct {
    unsigned char progression[6];
    unsigned short coins, instrument_ammo;
    unsigned short coloured_bananas[14], coloured_bananas_fed_to_tns[14], golden_bananas[14];
} CoopCharacterProgress;
typedef struct {
    CoopCharacterProgress character_progress[6];
    unsigned char untouched[0x2FC - 6 * 0x5E]; // Includes ammo and current health: never overwritten.
    unsigned char melons;
    unsigned char tail[0x306 - 0x2FD];
} CoopPlayerProgress;
#endif
