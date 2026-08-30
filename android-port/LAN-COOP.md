# LAN co-op feasibility

Assessed 2026-08-30. **No LAN networking or co-op gameplay has been implemented.** The APK and tablet installation are unchanged by this assessment. The user confirms that the imported Tag Anywhere mod works in-game; that establishes a working recomp-mod path on this Android port, not multiplayer support.

## Assessment

A dedicated DK64 recomp mod plus an Android-compatible networking bridge is technically plausible. Full campaign co-op would be a substantial reverse-engineering and synchronization project. No compatible released DK64 recomp LAN co-op mod was found in the sources searched.

Local source includes player/actor structures, per-player character/camera data and mod hooks. These are useful starting points, but they do not demonstrate that story maps, scripts or bosses safely support multiple independent players. The existing multiplayer setting is not evidence of campaign co-op or LAN functionality.

[RetroFoundry's experimental Zelda Recompiled networking project](https://github.com/retrofoundry/mm-network-sync) demonstrates a recomp mod API, a native networking library, a server and remote-player rendering. Its documented synchronization is limited to player position/rotation, with limited recovery handling. Its game-specific hooks are not compatible DK64 code, and it does not establish full-world synchronization.

## Proposed first target

- Two devices on the same local network; each renders its own view and controls its own Kong.
- Explicit Host/Join controls, initially joining by host IP rather than requiring discovery or an external service.
- Players stay in the same map; the host coordinates transitions. Independent exploration across maps is a later feature.
- A separate co-op save owned by the host. Joining must not overwrite the guest's normal saves.
- Shared progression governed by the host. Define each collectible/flag's behavior explicitly; do not merge entire save files or blindly OR all flags.
- A fixed protocol and compatible game/mod versions. Tag Anywhere interaction, simultaneous Kong changes and duplicate-Kong rules need dedicated tests.

The user wants both Android and Windows support, including mixed-platform sessions. Target Android ARM64 and Windows x64 initially. Android-to-Android, Windows-to-Windows and Android-to-Windows sessions are all intended configurations; either platform should be able to host. These are design requirements, not tested capabilities.

## Project boundary and cross-platform design

Keep co-op development in a separate project/repository from the Android port. The co-op project should own the game hooks, protocol, shared networking/session code, platform adapters and compatibility tests. The user explicitly requires leaving the runtime mod loader unchanged. Distribute the native networking bridge beside the mod using its existing `native_libraries` support. Android importer/manager support and network permission belong in the Android app; they do not change the runtime loader. Windows compatibility must still be tested.

- Ship separate Android and Windows ZIP downloads containing the shared `.nrm` and its matching companion library in the same directory: Android ARM64 `.so` or Windows x64 `.dll`. Do not embed the native library inside the `.nrm` or introduce a new mod manifest format. Android import should install both files from one ZIP; Windows users extract both into the mods directory.
- Use a portable networking/session core with narrow platform adapters. Define explicit byte order, fixed-width fields, bounded packet sizes, protocol versions and validation. Never transmit raw C structs, native pointers or process memory images.
- Keep networking callbacks away from game memory. Transfer validated events through bounded queues and apply gameplay changes at verified game-thread hooks on both platforms.
- Support explicit Host/Join by LAN IP first. The host controls shared state and progression; no external account, relay or dedicated server should be required for the initial target. Internet matchmaking and host migration are outside the first milestone.
- Reject incompatible protocol, bridge, game and co-op mod versions before creating remote actors or modifying saves. Define which additional gameplay mods are allowed rather than assuming every installed mod combination works.
- Test Android-host/Windows-guest and Windows-host/Android-guest first, then both same-platform configurations. Mixed-platform testing should begin at the first connection/avatar milestone, not after synchronization is finished.

The earlier dev4 APK has no network permission and rejects native companion imports. Dev7 includes the Android ZIP companion import/management and `INTERNET` permission introduced in local dev5, without changing the runtime loader. Host tests cover package handling and interrupted-update recovery; actual native bridge execution on a device remains unverified. A mod cannot grant its host app Android permissions. Importing a co-op `.nrm` alone will not enable networking in dev4, and this release includes no co-op bridge or gameplay.

The pinned runtime already reads `native_libraries` and loads platform-specific companion libraries with `LoadLibraryExW` or `dlopen`. Keep its existing lookup and manifest interpretation unchanged. Its lookup expects the native library beside the `.nrm`, not embedded inside it. Initial Android bundle support accepts one declared native bridge per mod; additional platform dependencies should use Android public NDK libraries or be linked into that bridge.

The Android importer must validate native payloads, reject library filename conflicts, and update/remove companion files with their owning mod. Use private storage, preserve archive safety checks, and recover interrupted multi-file installation before starting the game. Never replace native libraries while the game process is running. Windows installations also require closing the game before replacing an in-use DLL. Validate the unchanged loading path on the tablet; if a platform restriction blocks it, report that rather than modifying the loader or bypassing restrictions.

Android references: [network permissions](https://developer.android.com/develop/connectivity/network-ops/connecting) and [dynamic code loading risks and mitigations](https://developer.android.com/privacy-and-security/risks/dynamic-code-loading). Mod bundles contain executable code with the app's permissions; format and architecture checks alone do not make an untrusted mod safe.

## Work required

1. Identify safe game-thread hooks for reading the local player's state and creating/updating/removing a remote avatar, including model, animation, map identity and scene lifetime. Test that a remote avatar cannot accidentally trigger local-only game scripts.
2. Prototype a harmless Android-native companion mod to verify installation, runtime exports and library lifecycle, then add the LAN bridge and Android network permission. Prefer a versioned companion-library API shared with Windows. Keep network callbacks separate from game-memory mutation; use bounded validated messages and game-thread event handling.
3. Establish stable object/event identities and host ownership for shared interactions. Position snapshots can be interpolated, while item grants, switches, damage and progression need reliable, deduplicated events. Raw pointers and process memory addresses must not be used as cross-device identities.
4. Handle map changes, loading, death, pause, app backgrounding, disconnect and reconnect. Initially end or suspend a session cleanly when the host leaves; host migration is separate work.
5. Extend synchronization to enemies, moving objects, combat, puzzles, minigames, bosses and cutscenes, with per-system testing. Displaying another player is only an early networking milestone.

## Acceptance milestones

1. Two real devices connect and show independently moving avatars in one safe area; leaving/rejoining and map unload do not leave stale actors.
2. Either player collects one supported item or activates one supported switch; both views agree, the host saves it once, and replayed messages cannot award it twice.
3. Coordinated transitions, death and reconnect preserve the session's progression without touching normal saves.
4. Expand supported gameplay and compatibility with Tag Anywhere. Only describe campaign co-op as playable after the relevant progression and interaction systems have been tested together.

No effort estimate or full-campaign compatibility claim is justified before the first multiplayer actor and synchronization experiments.

## Local source references

- `upstream/patches/common_structs.h`: Actor, PlayerAdditionalActorData and CharacterChange structures.
- `upstream/patches/patches_main.c`: uses of `character_change_array` and `cc_player_index`, including player/camera/culling behavior.
- `upstream/src/game/recomp_extension_api.cpp`: object extension data facilities; applicability to remote-player state still needs investigation.
- `android/app/src/main/AndroidManifest.xml`: dev5 adds Internet permission for optional native LAN mods.
- `android/app/src/main/java/io/github/dk64port/ModStore.java`: current Android native-library mod limitation.

This is independent of the proposed Archipelago integration. Archipelago item/location exchange does not implement real-time shared-world co-op.
