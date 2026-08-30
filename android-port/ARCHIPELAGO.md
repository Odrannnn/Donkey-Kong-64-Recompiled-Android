# Archipelago integration assessment

Status: researched on 2026-08-30; **Archipelago is not implemented or playable in this Android port**. No compatible released DK64 recomp Archipelago mod was found in the repositories and releases checked. Android mod management was subsequently added in dev3; the Archipelago-specific work below remains outstanding.

## Feasibility

A dedicated DK64 recomp mod, paired with an Android-compatible Archipelago client, is a plausible route. The existing mod runtime is a foundation, not a complete integration. This would be a substantial game-logic port as well as a networking feature.

The current DK64 Randomizer integration runs injected ROM-hack code and uses a separate client to exchange data through game memory. Its `dk64.apworld` is an Archipelago world package, not a recomp `.nrm` mod. Importing that package or a randomized ROM into this app would not implement its hooks. DK64 Recompiled executes code generated from the supported original game; bypassing ROM validation cannot make arbitrary injected ROM code work.

## Findings in the current Android source

- N64ModernRuntime retains the `.nrm` code-mod loader and live recompilation support. ARM64 compilation alone does not verify any particular mod on the tablet.
- The Android launcher now has DocumentsUI mod import and installed-mod management. RecompFrontend's install/folder buttons route to it after closing the game session. Online mod discovery is disabled.
- Mods can declare native libraries. Those libraries would need Android ARM64/Bionic builds and the runtime's expected exports. Windows DLLs and ordinary Linux ARM64 libraries are not interchangeable with Android libraries.
- The manifest currently has no Internet permission, and the app has no Archipelago connection, item-delivery or location-check client.
- Mod game modes exist, but save isolation for an Archipelago seed and slot must be designed and verified explicitly.

## Work required

1. **Randomizer mod:** port the relevant DK64 Randomizer item, location, progression, world-state and completion hooks to the recomp mod API. Define a versioned seed-data format and a generation/export path compatible with that mod. Keep supported original-ROM validation intact.
2. **Client bridge:** implement Archipelago connection/authentication, location reporting, item delivery, reconnect and resynchronization on Android. Use a small validated, bounded event queue between networking and game execution; apply game mutations only at safe game-thread points. An Android native library or an app-hosted client are options to investigate, not existing implementations.
3. **Android interface:** build on the DocumentsUI import/management added in dev3; provide server, slot and optional password entry; show connection errors and unsupported versions. Native companion-library import still needs implementation if that client architecture is chosen. Add Internet permission when the client is implemented. Keep credentials out of diagnostics.
4. **Persistence:** isolate saves by seed and slot and persist receipt progress. Test interrupted delivery, reconnect, replayed item lists and restart to prevent duplicate rewards or lost progression. Preserve existing non-randomizer saves.
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
