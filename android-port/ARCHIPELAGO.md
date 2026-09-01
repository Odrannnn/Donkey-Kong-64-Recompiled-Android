# Archipelago integration status and assessment

Updated 2026-09-01: an **experimental direct integration-test implementation (0.2.0)** is in [archipelago/](archipelago/README.md). Its Android ARM64 mod library connects directly to AP with WebSocket/TLS; no desktop relay is required. It includes connection settings in the mod menu, isolated saves, durable item/check synchronization and a separately named developer AP world. It supports five first special moves and two Japes checks using pinned original DK64 Randomizer IDs. **The full DK64 Randomizer is not ported; ordinary `dk64.apworld` seeds and `.chunky` patches are still unsupported.** Vanilla rewards and gameplay remain. This is an integration milestone, not a campaign randomizer.

The original assessment below was researched on 2026-08-30. No compatible released full DK64 recomp Archipelago mod was found in the repositories and releases checked. The new prototype does not change those gameplay limitations.

Full-port source work now includes all 868 original location IDs and 184 client item definitions, a generated bounded game-side effect table, atomic receipt-prefix ownership rebuilding, seed/counter-checked restart staging, a dormant original-`CountStruct` query surface, and a seed-bound game check ledger. A separate native campaign journal durably stores indexed receipts and canonical observed checks, commits disk changes before publication, rejects AP replay gaps/conflicts/rollback, and derives the exact prefix plus optional next item for the game trailer. A background campaign channel exposes that state as a bounded N64-order snapshot. The MIPS side proves every next supported receipt as an exact current/next identity and before/after inventory transaction. It now executes 148 of the 184 catalog definitions: No Item, every receivable replay-safe move, ability, Kong, key, permanent flag, special, hint, blueprint and counted-resource receipt, plus 13 aliases for Disable A/B/Z/C-Up and Dry traps. Those traps use the original 221-map/137-movement safety policy; controller disables last eight seconds and Dry wipes the original live resources. Permanent flags use the stock flag writer, Golden Bananas use the original client's balanced live distribution, Rainbow Coins add five coins to every Kong, and the other randomizer-only counts remain in the trailer-bound projection. The other 17 trap definitions (11 behaviors) and 19 non-placeable metadata definitions fail before mutation. The query surface covers Kongs, keys, abilities, special items, hints, blueprints, shopkeepers and counted resources. Exact original-randomizer ability, item-count and Kong-ownership call sites now consume that surface behind a disabled capability, including pause/file percentage, Snide blueprint, balloon visibility, fairy capacity and K. Lumsy key checks. A dormant original-world `CampaignSession` connects directly with WebSocket/TLS and feeds only durable receipts/checks across reconnects. Forty-one game-function replacements cover the existing move/shop/save paths, the pinned ability and count-query edits, a GB reward interception path, a guarded EEPROM writer and the stock controller filter wrapped by the trap mask. AP shop handlers record all 120 shop IDs with cumulative prices and shared-shop exclusions. Generic dialogue and guarded item/recipient purchase overlays run directly inside the mod. The campaign ABI still has no production MIPS caller or gate assignment. Eleven trap behaviors, trap presentation, remaining event hooks and live campaign selection are unfinished, so activation remains disabled. [Full-port status](archipelago/FULL-PORT.md) documents the remaining work.

Dev5 added generic Android native-companion ZIP import and Internet permission. Native execution was subsequently verified with the separate LAN presence prototype. The Archipelago mod itself still requires device validation; neither that earlier co-op test nor the new host tests establish its in-game behavior.

The direct client builds and passes native state/journal/ABI checks and an isolated Archipelago 0.6.7 server test covering direct authentication, location scouting, game-specific item-name resolution, checks, items, test-goal reporting and reconnect. Local TLS tests check trust and hostname validation. The server test simulates game checks; it does not establish tablet gameplay or full DK64 randomizer compatibility. Setup, artifacts, exact scope and reproduction steps are in [archipelago/README.md](archipelago/README.md).

## Feasibility

A dedicated DK64 recomp mod, paired with an Android-compatible Archipelago client, is a plausible route. The existing mod runtime is a foundation, not a complete integration. This would be a substantial game-logic port as well as a networking feature.

The current DK64 Randomizer integration runs injected ROM-hack code and uses a separate client to exchange data through game memory. Its `dk64.apworld` is an Archipelago world package, not a recomp `.nrm` mod. Importing that package or a randomized ROM into this app would not implement its hooks. DK64 Recompiled executes code generated from the supported original game; bypassing ROM validation cannot make arbitrary injected ROM code work.

## Findings in the current Android source

- N64ModernRuntime retains the `.nrm` code-mod loader and live recompilation support. ARM64 compilation alone does not verify any particular mod on the tablet.
- The Android launcher now has DocumentsUI mod import and installed-mod management. RecompFrontend's install/folder buttons route to it after closing the game session. Online mod discovery is disabled.
- Mods can declare native libraries. Those libraries would need Android ARM64/Bionic builds and the runtime's expected exports. Windows DLLs and ordinary Linux ARM64 libraries are not interchangeable with Android libraries.
- The Android manifest already has Internet permission. The experimental mod connects directly from its native library and exposes AP settings in the native Mods menu.
- The prototype selects a mod-specific save before the EEPROM worker starts, binds its filename to seed/team/slot/profile, and verifies ownership before game updates. Normal-game saves are not used. Runtime/device save recovery still needs validation.

## Work required

1. **Randomizer mod:** port the relevant DK64 Randomizer item, location, progression, world-state and completion hooks to the recomp mod API. Define a versioned seed-data format and a generation/export path compatible with that mod. Keep supported original-ROM validation intact.
2. **Client integration:** direct connection/authentication, location reporting, item delivery, reconnect and resynchronization are implemented for the test profile in the mod's native library. A bounded event queue separates networking from game execution. Extend its protocol contract only as the remaining gameplay hooks become supported.
3. **Android interface:** server, player, password and seed binding are in native mod settings. Validate entry and reconnect behavior on the device. Passwords are stored unmasked in private settings; use TLS, avoid reused passwords, and keep credentials out of diagnostics.
4. **Persistence:** seed/slot save isolation, durable native receipts and a guarded game-save receipt trailer are implemented. Connect them to item effects as one recoverable lifecycle, then test interrupted delivery, reconnect, replayed item lists and restart on Android. Preserve existing non-randomizer saves.
5. **Compatibility:** define the supported DK64 AP world/version/options. Implement optional behavior such as DeathLink only after basic progression is reliable. Server generation can initially remain on a computer; generating multiworlds on the tablet is a separate task.

## Important ABI boundary

The upstream client's `DK64MemoryMap` includes pointers at `0x807FFF1C` and `0x807FFFB8` into structures installed by the randomizer ROM hack. The hack defines `APData`, item-feed handling and other integration state. These are not an API available in the original recompiled game. Copying the client's writes to those addresses would be unsafe and would not substitute for porting the hack.

The wider randomizer dependency is material: `handleArchipelagoFeed` depends on randomizer configuration and game hooks, not only a socket connection. A connection screen without those hooks would not produce a randomized game.

## First acceptance milestone

Use an authorized local test multiworld and one supported seed to verify all of the following on the tablet:

- Receive one real item from the server and grant the correct in-game effect at a safe point.
- Collect one supported location and report it to the server exactly once.
- Disconnect, reconnect and restart without duplicate grants or lost checks.
- Reject mismatched seed/slot/version data and keep normal-game saves unchanged.

This would establish a vertical slice, not full randomizer compatibility. Broader location coverage, progression logic, completion, optional features and long-session testing follow it. The underlying Android port still needs full gameplay, input, save and lifecycle validation.

## Sources checked

- [DK64 Randomizer](https://github.com/2dos/DK64-Randomizer), `dev` revision observed during this assessment: `66d0dc90064a572e9bf2a2eada53ef81a7f47eb4`.
- [DK64 client](https://github.com/2dos/DK64-Randomizer/blob/66d0dc90064a572e9bf2a2eada53ef81a7f47eb4/archipelago/DK64Client.py) and [memory map](https://github.com/2dos/DK64-Randomizer/blob/66d0dc90064a572e9bf2a2eada53ef81a7f47eb4/archipelago/client/common.py).
- [ROM-hack Archipelago interface](https://github.com/2dos/DK64-Randomizer/blob/66d0dc90064a572e9bf2a2eada53ef81a7f47eb4/base-hack/include/archipelago.h) and [implementation](https://github.com/2dos/DK64-Randomizer/blob/66d0dc90064a572e9bf2a2eada53ef81a7f47eb4/base-hack/src/item%20rando/archipelago.c).
- [DK64 AP world v1.5.8 release](https://github.com/2dos/DK64-Randomizer-Release/releases/tag/v1.5.8), distributing `dk64.apworld`.
- [MMRecompRando](https://github.com/RecompRando/MMRecompRando), an example of Archipelago integration through recomp mod support for a different game. It is an architectural reference, not a compatible DK64 mod.
- Local pinned sources: `upstream/lib/N64ModernRuntime/librecomp/src/mods.cpp`, `recomp.cpp`, and RecompFrontend's `ui_mod_menu.cpp` / `ui_mod_installer.cpp`; dependency revisions are recorded in `upstream.lock.json`.
