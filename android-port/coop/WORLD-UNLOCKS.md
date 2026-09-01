# Vanilla permanent-world flag coverage (0.14, untested)

All 216 vanilla US instance-script maps were inspected. This file contains
only flag/object metadata, not ROM assets or replacement scripts.
Compilation and metadata generation are not runtime or gameplay validation.

This pass adds 82 IDs to the 2312 in v0.13: **2394 IDs total**, including
100 world rows. The selection covers every remaining script-written
persistent unlock/completion flag; excluded bookkeeping is listed below.
Existing GB-controlled doors/reveals retain their original collectible IDs.

Level 255 means related scripts are confined to excluded interiors: receive
in any normal safe snapshot map. Other rows require leaving their level.
Level 8 is Helm; all safe snapshot maps are outside it.

| ID | Flag | Excluded level | Completion | Vanilla writer map/object |
| --- | --- | --- | --- | --- |
| 2312 | 0x007 | 0 | Japes shell/hive gate | 7/0x33 |
| 2313 | 0x00D | 0 | Japes Diddy-switch hut | 7/0x38 |
| 2314 | 0x00E | 0 | Japes Lanky-switch hut | 7/0x3A |
| 2315 | 0x00F | 0 | Japes DK-switch hut | 7/0x3B |
| 2316 | 0x010 | 0 | Japes Tiny-switch hut | 7/0x39 |
| 2317 | 0x01E | 0 | Japes painting-cave gate | 7/0x27 |
| 2318 | 0x02B | 0 | Japes Rambi rock wall | 7/0x115 |
| 2319 | 0x000 | 0 | Japes first gate | 7/0x1A |
| 2320 | 0x02E | 0 | Japes boss portals closed after key | 7/0x2C, 7/0x37, 7/0x11A |
| 2321 | 0x02F | 1 | Aztec permanent door, object 0x40 | 38/0x40 |
| 2322 | 0x030 | 1 | Aztec permanent door, object 0x8D | 38/0x8D |
| 2323 | 0x035 | 1 | Aztec beetle-race tower reveal | 38/0x1E |
| 2324 | 0x037 | 1 | Aztec five-door-temple totem activated | 38/0x27 |
| 2325 | 0x045 | 1 | Tiny Temple ice melted | 16/0x4 |
| 2326 | 0x04E | 1 | Aztec tunnel door | 38/0x9C |
| 2327 | 0x06C | 1 | Aztec boss portals closed after key | 38/0x8, 38/0x9, 38/0xA, 38/0xB, 38/0xEC |
| 2328 | 0x098 | 2 | Factory boss portals closed after key | 26/0x46, 26/0x47, 26/0x48, 26/0x49, 26/0x4A |
| 2329 | 0x099 | 3 | Galleon gate, object 2 | 30/0x2 |
| 2330 | 0x09B | 3 | Galleon coconut gate | 30/0x4 |
| 2331 | 0x09C | 3 | Galleon ship/lighthouse activation | 30/0x27 |
| 2332 | 0x09F | 3 | Galleon Enguarde door | 30/0x21 |
| 2333 | 0x0A1 | 3 | Galleon peanut gate | 30/0x3 |
| 2334 | 0x0A2 | 3 | Galleon gate, object 5 | 30/0x5 |
| 2335 | 0x0B3 | 3 | Galleon treasure chest, object 0xD | 30/0xD |
| 2336 | 0x0B4 | 3 | Galleon fairy chest | 30/0x45 |
| 2337 | 0x0B5 | 3 | Galleon GB chest | 30/0x43 |
| 2338 | 0x0B9 | 3 | Galleon breakable gate | 30/0x3F |
| 2339 | 0x0BA | 3 | Galleon pearl 1 | 44/0x0 |
| 2340 | 0x0BB | 3 | Galleon pearl 2 | 44/0x1 |
| 2341 | 0x0BC | 3 | Galleon pearl 3 | 44/0x2 |
| 2342 | 0x0BD | 3 | Galleon pearl 4 | 44/0x3 |
| 2343 | 0x0BE | 3 | Galleon pearl 5 | 44/0x4 |
| 2344 | 0x0CB | 3 | Galleon boss portals closed after key | 30/0x22, 30/0x23, 30/0x24, 30/0x25, 30/0x26 |
| 2345 | 0x0CF | 4 | Fungi green tunnel feather gate | 48/0x17 |
| 2346 | 0x0D0 | 4 | Fungi green tunnel pineapple gate | 48/0x1C |
| 2347 | 0x0D2 | 4 | Fungi gold tunnel gate | 48/0x1D |
| 2348 | 0x0D4 | 4 | Fungi mill pulley raised | 48/0x25 |
| 2349 | 0x0D5 | 4 | Fungi mill pulley guitar sequence complete | 48/0x25 |
| 2350 | 0x0DA | 4 | Fungi front-mill question-mark box | 61/0x6 |
| 2351 | 0x0DC | 4 | Fungi mill conveyor puzzle complete | 61/0xE |
| 2352 | 0x0DE | 4 | Fungi rear-mill question-mark box | 62/0x1 |
| 2353 | 0x0DF | 4 | Fungi rear-mill Mini Monkey box | 62/0x3 |
| 2354 | 0x0E5 | 4 | Fungi mushroom board puzzle complete | 64/0xB |
| 2355 | 0x102 | 4 | Fungi boss portals closed after key | 48/0x15, 48/0x16, 48/0x3B, 48/0x47, 48/0x52 |
| 2356 | 0x109 | 5 | Caves breakable ice wall 1 | 72/0x1F |
| 2357 | 0x10A | 5 | Caves breakable ice wall 2 | 72/0x1E |
| 2358 | 0x10B | 5 | Caves breakable ice wall 3 | 72/0x1D |
| 2359 | 0x118 | 5 | Caves Lanky igloo balloon pad reveal | 85/0x0 |
| 2360 | 0x128 | 5 | Caves igloo instrument pads revealed | 72/0x38 |
| 2361 | 0x12C | 5 | Caves persistent encounter clear | 72/0x34 |
| 2362 | 0x12E | 5 | Caves boss portals closed after key | 72/0x23, 72/0x24, 72/0x25, 72/0x26 |
| 2363 | 0x130 | 6 | Castle ballroom encounter clear/barrel reveal | 88/0x3 |
| 2364 | 0x133 | 6 | Castle tower encounter clear/balloon pad | 105/0x1 |
| 2365 | 0x138 | 6 | Castle crypt metal panel opened | 112/0x11 |
| 2366 | 0x141 | 6 | Castle toolshed question-mark box | 166/0x5 |
| 2367 | 0x144 | 6 | Castle tree gate | 87/0x20 |
| 2368 | 0x157 | 6 | Castle basement breakable grate 1 | 163/0x28 |
| 2369 | 0x158 | 6 | Castle basement breakable grate 2 | 163/0x29 |
| 2370 | 0x159 | 6 | Castle basement breakable grate 3 | 163/0x2A |
| 2371 | 0x15A | 6 | Castle basement breakable grate 4 | 163/0x2B |
| 2372 | 0x15B | 6 | Castle basement breakable grate 5 | 163/0x2C |
| 2373 | 0x15C | 6 | Castle basement breakable grate 6 | 163/0x2D |
| 2374 | 0x160 | 6 | Castle boss portals closed after key | 87/0xB, 87/0xC, 87/0xD, 151/0x4, 183/0x2 |
| 2375 | 0x181 | 255 | Training grounds exit switch | 176/0x39 |
| 2376 | 0x189 | 255 | Fairy Island Rareware room door | 189/0x1D |
| 2377 | 0x18D | 255 | Factory lobby lever/panel | 175/0xC |
| 2378 | 0x18F | 255 | Aztec lobby stone panel | 173/0xF |
| 2379 | 0x190 | 255 | Aztec lobby encounter platform | 173/0x11 |
| 2380 | 0x191 | 255 | Galleon lobby door | 174/0xC |
| 2381 | 0x195 | 255 | Fungi lobby gun-order puzzle complete | 178/0x9 |
| 2382 | 0x197 | 255 | Helm lobby coconut-switch route | 170/0x2 |
| 2383 | 0x198 | 255 | Caves lobby ice wall 1 | 194/0x0 |
| 2384 | 0x199 | 255 | Caves lobby ice wall 2 | 194/0x1 |
| 2385 | 0x1A1 | 255 | Helm lobby Bananaport tag 1 | 170/0x8 |
| 2386 | 0x1A2 | 255 | Helm lobby Bananaport tag 2 | 170/0x9 |
| 2387 | 0x1AA | 7 | Isles trombone-pad barrel reveal | 34/0x31 |
| 2388 | 0x1AE | 7 | Isles boulder opened | 34/0x0 |
| 2389 | 0x302 | 8 | Helm shutdown complete | 17/0x30 |
| 2390 | 0x303 | 8 | Helm coin door | 17/0x3 |
| 2391 | 0x304 | 8 | Helm crown door | 17/0x4 |
| 2392 | 0x305 | 8 | Helm Bananaport tag 1 | 17/0x59 |
| 2393 | 0x306 | 8 | Helm Bananaport tag 2 | 17/0x58 |

## Excluded script-written state

| Flag | Reason |
| --- | --- |
| 0x02C | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x02D | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x060 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x061 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x062 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x063 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x064 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x065 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x066 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x067 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x068 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x069 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x06A | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x06B | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x0A0 | reversible Galleon water height |
| 0x0C3 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x0C4 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x0CE | reversible day/night |
| 0x0FF | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x100 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x163 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x164 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x166 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x16F | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x170 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x171 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x172 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x188 | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |
| 0x19D | reversible Caves-lobby pressure switch |
| 0x1AB | tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock |

Paid arcade entry (0x83), level visits, captions, first-time actor dialogue,
quit/global flags and ongoing boss phases are not world unlocks. Final K. Rool
defeat (`0x1B0`) is synchronized separately as explicit item ID 5895 after the
complete key/K. Lumsy dependency chain; its ending cutscene stays local.
Temporary switches, timers, live actors, damage, water height and day/night
need separate synchronization; they must not be ORed into this ownership set.

Prerequisites: closed boss portals require their boss key; Rareware room
requires camera/shockwave and 20 fairies; Helm coin door requires both
special coins; Helm crown door requires any four crowns; mill pulley
completion requires its raised state. No collectible is spent by receiving
a flag. The local game must award the completion; no auto-clear is inferred.
