// Generated from vanilla US setups/scripts by tools/generate-travel.py.
// Metadata only. No script bytecode or invented randomizer flags.
#ifndef COOP_TRAVEL_IDS_H
#define COOP_TRAVEL_IDS_H
enum { COOP_WARP_TAG_COUNT = 87 };
typedef struct { unsigned short flag; unsigned char map; short required_item; } CoopWarpTag;
static const CoopWarpTag coop_warp_tags[COOP_WARP_TAG_COUNT] = {
    {0x020, 7, -1}, // object 0x059
    {0x021, 7, -1}, // object 0x05A
    {0x028, 7, -1}, // object 0x05E
    {0x029, 7, -1}, // object 0x06F
    {0x025, 7, -1}, // object 0x097
    {0x023, 7, -1}, // object 0x098
    {0x024, 7, -1}, // object 0x09E
    {0x022, 7, -1}, // object 0x09F
    {0x026, 7, -1}, // object 0x12A
    {0x027, 7, 175}, // object 0x12B
    {0x058, 20, -1}, // object 0x04E
    {0x059, 20, -1}, // object 0x058
    {0x05A, 20, -1}, // object 0x099
    {0x05B, 20, -1}, // object 0x09A
    {0x08D, 26, -1}, // object 0x07D
    {0x091, 26, -1}, // object 0x0D9
    {0x095, 26, -1}, // object 0x0EE
    {0x094, 26, -1}, // object 0x105
    {0x096, 26, -1}, // object 0x10B
    {0x093, 26, -1}, // object 0x10C
    {0x08F, 26, -1}, // object 0x141
    {0x08E, 26, -1}, // object 0x142
    {0x092, 26, -1}, // object 0x143
    {0x090, 26, -1}, // object 0x144
    {0x0A9, 30, -1}, // object 0x015
    {0x0AA, 30, -1}, // object 0x016
    {0x0AF, 30, -1}, // object 0x056
    {0x0AC, 30, -1}, // object 0x05F
    {0x0AE, 30, -1}, // object 0x060
    {0x0AD, 30, -1}, // object 0x066
    {0x0AB, 30, -1}, // object 0x06C
    {0x0B1, 30, -1}, // object 0x1F6
    {0x0B2, 30, -1}, // object 0x1F7
    {0x1B1, 34, -1}, // object 0x010
    {0x1B2, 34, -1}, // object 0x011
    {0x1B3, 34, -1}, // object 0x012
    {0x1B4, 34, -1}, // object 0x013
    {0x1B6, 34, -1}, // object 0x014
    {0x1BA, 34, -1}, // object 0x015
    {0x1B5, 34, -1}, // object 0x016
    {0x1B7, 34, -1}, // object 0x017
    {0x1B8, 34, -1}, // object 0x018
    {0x1B9, 34, -1}, // object 0x019
    {0x04F, 38, -1}, // object 0x006
    {0x050, 38, -1}, // object 0x007
    {0x056, 38, -1}, // object 0x073
    {0x052, 38, -1}, // object 0x07F
    {0x051, 38, -1}, // object 0x080
    {0x057, 38, -1}, // object 0x082
    {0x053, 38, -1}, // object 0x095
    {0x054, 38, -1}, // object 0x098
    {0x055, 38, -1}, // object 0x0B1
    {0x0ED, 48, -1}, // object 0x035
    {0x0EE, 48, -1}, // object 0x036
    {0x0EF, 48, -1}, // object 0x049
    {0x0F0, 48, -1}, // object 0x04A
    {0x0F1, 48, -1}, // object 0x04B
    {0x0F2, 48, -1}, // object 0x04E
    {0x0F3, 48, -1}, // object 0x04F
    {0x0F4, 48, -1}, // object 0x051
    {0x0F5, 48, -1}, // object 0x055
    {0x0F6, 48, -1}, // object 0x056
    {0x11B, 72, -1}, // object 0x021
    {0x11C, 72, -1}, // object 0x022
    {0x11E, 72, -1}, // object 0x036
    {0x11D, 72, -1}, // object 0x037
    {0x123, 72, -1}, // object 0x057
    {0x121, 72, -1}, // object 0x060
    {0x11F, 72, -1}, // object 0x06A
    {0x120, 72, -1}, // object 0x06B
    {0x122, 72, -1}, // object 0x0B5
    {0x14D, 87, -1}, // object 0x021
    {0x148, 87, -1}, // object 0x022
    {0x14C, 87, -1}, // object 0x023
    {0x147, 87, -1}, // object 0x024
    {0x14A, 87, -1}, // object 0x028
    {0x14E, 87, -1}, // object 0x029
    {0x150, 87, -1}, // object 0x02A
    {0x149, 87, -1}, // object 0x02B
    {0x14B, 87, -1}, // object 0x02C
    {0x14F, 87, -1}, // object 0x02D
    {0x151, 112, -1}, // object 0x018
    {0x153, 112, -1}, // object 0x019
    {0x155, 112, -1}, // object 0x01A
    {0x156, 112, -1}, // object 0x01B
    {0x154, 112, -1}, // object 0x01C
    {0x152, 112, -1}, // object 0x01D
};
// Three automatic pads use already-shared GB ownership:
// map 30, object 0x055: GB flag 0x0A3, item 222.
// map 38, object 0x087: GB flag 0x03E, item 189.
// map 72, object 0x056: GB flag 0x127, item 277.
#endif
