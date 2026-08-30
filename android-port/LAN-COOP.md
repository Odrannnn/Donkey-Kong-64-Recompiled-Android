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

The second device's platform remains unspecified. Android-to-Android would need the bridge on both apps; Windows-to-Android also requires a compatible Windows bridge/build. The same `.nrm` game logic may be reusable, but that alone does not supply the platform networking layer.

## Work required

1. Identify safe game-thread hooks for reading the local player's state and creating/updating/removing a remote avatar, including model, animation, map identity and scene lifetime. Test that a remote avatar cannot accidentally trigger local-only game scripts.
2. Add the LAN bridge and Android network permission. The current app has neither. An app-provided, versioned networking API would avoid depending on the currently unsupported native companion-library import path. Keep network callbacks separate from game-memory mutation; use bounded validated messages and game-thread event handling.
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
- `android/app/src/main/AndroidManifest.xml`: no current Internet permission.
- `android/app/src/main/java/io/github/dk64port/ModStore.java`: current Android native-library mod limitation.

This is independent of the proposed Archipelago integration. Archipelago item/location exchange does not implement real-time shared-world co-op.
