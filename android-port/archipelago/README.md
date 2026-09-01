# DK64 Archipelago direct mod — 0.2.0

**The mod connects directly to the Archipelago server. No desktop relay, Python client, UDP bridge, or APK rebuild is required to play the test profile.** WebSocket networking and TLS run inside the Android native library shipped with the `.nrm`.

This remains an experimental integration test, not the full DK64 Randomizer. It uses five original randomizer item IDs and two location IDs, but connects as the separate **DK64 Recomp Integration Test** world. Ordinary `dk64.apworld` seeds, `.chunky` files, randomized ROMs and full campaign randomization are unsupported. Vanilla rewards, shops, Kong unlocks and progression remain unchanged. Unofficial and AI-assisted; no ROM or original game assets are distributed.

The source now includes the full 868-location/184-item catalog, a generated fixed game-side effect table, atomic receipt-prefix inventory rebuilding, seed/counter-checked restart staging, a seed-bound game-side check ledger, and a fixed endian-safe RDRAM staging wire. The dormant ledger validates complete selected/restored bitsets, observes all 291 safe stock-flag candidates or exact event IDs, suppresses duplicates and invalidates itself when the current trailer identity changes. A dormant campaign bridge parses a bounded manifest, owns its isolated journal on a worker and asynchronously prepares the sealed identity, both check sets and up to 4096 committed item IDs; the game-thread ABI only polls and copies a completed snapshot. A separate direct campaign WebSocket/TLS session validates the original `Donkey Kong 64` room, exact seed/team/slot/world version and complete location partition before it can commit indexed receipts or send journaled checks. The combined MIPS consumer rejects the entire snapshot before publication when any field is invalid. Its transaction executor admits 148 of 184 catalog definitions, covering every receivable replay-safe ownership and counted-resource effect plus 13 aliases for Disable A/B/Z/C-Up and Dry traps. The other 17 trap definitions and 19 non-placeable metadata definitions remain rejected. AP shop handlers record all 120 shop IDs separately from inventory and enforce cumulative prices and shared-shop exclusions. Generic dialogue and guarded item/recipient purchase overlays run inside the mod, with bounded text snapshots and original fade timing. The active test-profile session scouts selected locations, resolves recipient-game item names and team aliases, and exports canonical replacement label snapshots directly to the game through a bounded, nonblocking ABI. Campaign activation remains disabled: no production MIPS code configures, connects, ticks or stages the campaign session, and the other 11 trap behaviors plus lifecycle orchestration are not implemented. See [FULL-PORT.md](FULL-PORT.md) for exact coverage and executable gameplay tests.

## Install and connect

1. Generate and host a disposable test seed using `dk64_recomp_test.apworld` and `test-world/player.yaml`. Install that world only in your Archipelago generator, not in Android. Generation and hosting are separate from the mod; the Android device connects straight to wherever the AP server runs.
2. Close the game. On Android dev7 or later, use **Manage Mods → Import mod file** to import **`DK64-Archipelago-0.2.0-android-direct-test.zip`**. Keep its `.nrm` and matching ARM64 `.so` together. Disable co-op and other save/gameplay mods. No APK changes are needed.
3. In the native Mods menu, enter:

   | Setting | Value |
   | --- | --- |
   | Server | Full `wss://host:port` or `ws://host:port` URL of the AP server |
   | Player / slot name | Exact AP player name, normally `DK64Test` in the supplied YAML |
   | Room password | Optional AP room password |
   | Expected AP seed name | Exact `RoomInfo.seed_name` from the room; not the numeric input used to generate a seed |
   | Team / slot number | AP team and player numbers, normally `0` / `1` for a one-player test |

   The expected seed/team/slot bind a separate save **before** DK64 starts its save worker. A mismatch fails closed. The server address and password do not change the save binding. For a LAN-hosted server, use the server computer's LAN address, not `127.0.0.1` (which means the Android device itself).
4. Set **Direct integration test** On and fully restart the game. Restart after changing any connection setting. Use **DK64 save slot 1**; the other game save slots cannot send checks or receive items.
5. Follow the normal tutorial/Japes route and collect the two supported bananas. The overlay shows `AP DIRECT TEST: CONNECTED` after synchronization. A previously owned move has no additional effect.

Prefer `wss://`, which verifies the certificate and hostname against bundled Mozilla CA roots. `ws://` is unencrypted, including its room password. Passwords are unmasked and stored in the app's private mod settings; do not reuse an account password. They are not written to the item journal or native diagnostics. Self-signed certificates and authentication inside URLs are not supported. There is no certificate-verification bypass.

## Supported gameplay

| Direction | Content |
| --- | --- |
| Receive | Baboon Blast, Chimpy Charge, Orangstand, Mini Monkey, Hunky Chunky |
| Send | Japes in Front of Diddy Cage: flag 4 / AP 14041180 |
| Send | Japes Free Diddy Item: flag 5 / AP 14041181 |
| Test goal | Both checks collected and Baboon Blast + Chimpy Charge received |

The test world places the first two moves at the two checks. Other first moves can be exercised with starting inventory options. You still receive the vanilla golden bananas. This verifies the AP integration path; it does not implement shuffled DK64 gameplay or the original randomizer's logic guarantees.

Stock DK64 stores special moves as levels 0–3. The original randomizer changes them to independent bits. Until those gameplay changes are ported, this mod safely grants only the first move for each Kong, changing level zero to one and never incrementing or downgrading another value. Counted items, later independent moves, progressive upgrades, Kong/key randomization, traps, hints, DeathLink and campaign completion are not implemented. Unsupported items or locations stop the session.

## Saves, reconnects and limits

- Off is the default. Save selection runs during `recomp_on_init`, before the EEPROM worker; late activation is refused. Active saves are `saves/odrannnn_dk64_archipelago/ap_<32-hex-binding>.bin`. The mod verifies the active save's ownership before game updates. Normal-game saves are not used.
- The native client stores `ap_<binding>.ap.json` beside that save. It durably records received items and outgoing checks before exposing/sending them, using a temporary file, synchronization and atomic replacement. A lock prevents two clients using the same journal. Keep the journal with the save. Corrupt or mismatched journals fail closed; do not delete one to work around an error without understanding the lost receipt history.
- The client reconnects automatically with a 1–30 second backoff. A previous nonempty inventory requires full server replay. Gaps request `Sync`; overlapping deliveries are checked and deduplicated. Conflicting history, rollback or wrong seed/team/slot/profile/version produces an error requiring correction and restart.
- AP omits an empty `ReceivedItems` list. A fresh journal can therefore report checks after slot validation, but grants nothing until actual items arrive. Previously granted and saved moves remain usable offline. Restart and reconnect to restore received moves after an unsaved crash; the mod does not force an EEPROM save.
- Check flags are sampled at safe gameplay points and queued even during a disconnect. Once journaled, they survive a game crash and are resent until confirmed. A crash before either journaling or a normal save may require recollecting a banana. Never reset or copy a save into another seed's binding.
- Game memory is changed only by the MIPS callback: active adventure, first save slot, player present, no cutscene, fade/transition or pause. Native frame calls exchange atomic bitmasks and bounded label snapshots; the label snapshot uses a nonblocking lock attempt and is dormant in the test profile. Networking, JSON parsing, TLS, reconnect waits and disk I/O stay on background threads.
- Input limits: 1 MiB WebSocket message, 256 fragments, 32 queued events / 2 MiB, JSON depth 32, 256 AP commands per message and 4096 receipt entries. HTTP handshake lines and headers are bounded. Compression is disabled. No network-provided memory addresses or executable code are accepted.

The full-port source also contains a dormant campaign receipt contract. It hashes the complete validated seed manifest into a 128-bit binding and reconciles a durable item list with a 16-bit counter in a versioned game-save header. A separate native campaign journal persists the complete indexed receipt list and canonical observed-check set, committing disk state before publishing changes. It rejects gaps, rollback, conflicting replay, unselected checks, wrong bindings and malformed state, then derives only the exact receipt prefix committed by the game trailer and the optional next durable item. A native campaign channel loads that journal and owns a single-flight update lane, while `CampaignProtocol` and `CampaignSession` provide direct original-world transport with bounded authentication, exact active-location validation, Sync recovery and durable replay. The N64-order stage feeds a MIPS transaction planner which proves one exact supported transition with sealed current/next identities and complete before/after inventories. Its dormant executor commits 148 catalog definitions into stock progress or the trailer-bound receipt projection and the same guarded save image: all replay-safe moves, abilities, Kongs, keys, flags, specials, hints, blueprints and counted resources, No Item, and the state effects for Disable A/B/Z/C-Up and Dry. The generated trap policy reproduces the original map and movement exclusions; controller masks expire after 240 frames. Golden Banana and Rainbow Coin receipts also apply their original live stock effects. The other 17 trap definitions remain fail-closed until their actor, screen and crowd-control dependencies are ported; the other 19 rejected IDs are upstream client metadata that AP generation does not place. A separate trailer-bound query layer mirrors the original randomizer's `CountStruct` ownership/count semantics without writing its added flags into stock EEPROM. Exact ability, item-count and Kong-ownership call sites consume it behind the disabled item capability, and stale gameplay adapters return zero rather than treating an unavailable diagnostic value as ownership. This does not activate campaign delivery: no production MIPS code calls these campaign exports or enables the internal capability gates, and the remaining traps plus live lifecycle hooks remain unfinished.

Version 0.1 relay configuration is obsolete. This release retains the same deterministic save-name binding for matching seed/name/team/slot/profile/version, but does not import the old relay database. The server must replay its inventory, and existing saved flags are reported again. Back up the old save/database before upgrading, use the same binding, and keep old relay recovery data until device validation is complete. Never run the old relay client and direct client simultaneously for one slot.

## Build and verification

Prepare the parent workspace's pinned game sources, user-owned US ROM and host mod tools as described in its README. The gameplay transformation verifies the decompressed ROM and selected function hashes; it never modifies that ROM. Native dependencies are pinned in `native-dependencies.json`; bootstrap clones only into the parent workspace's `.local/` and applies receive-size limits. It refuses to replace a checkout at a different revision.

```powershell
python archipelago/tools/bootstrap-native.py
wsl bash '/mnt/c/Users/leona/Documents/ChatGPT/DK64 android port/archipelago/tools/build-mod.sh'
./archipelago/tools/Build-Android.ps1
python archipelago/tools/verify_upstream.py .local/dk64-randomizer-source
python archipelago/tools/package.py
./archipelago/tools/Test-Import.ps1 -JavaHome PATH_TO_JDK17 -JsonJar PATH_TO_ORG_JSON_JAR
```

The source ZIP includes the pinned IXWebSocket and mbedTLS trees (including the framework submodule and applied patches) under `.local/`; no native dependency bootstrap is needed when using those exported sources. Game headers, symbols and mod tools still come from the prepared parent port. The Android package contains the mod, library and notices; the AP-world file is a separate generator artifact. No desktop relay package is produced.

Host checks, from `archipelago/` in WSL:

```sh
cmake -S . -B build/direct-host -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAP_SANITIZE=ON
cmake --build build/direct-host -j4
ctest --test-dir build/direct-host --output-on-failure
```

For the network checks, use an isolated Archipelago 0.6.7 source checkout with its required server dependencies installed in Python 3.12; the scripts use that Python executable to launch the server. Do not point tests at your installed AP or a real game save.

```sh
AP_PYTHON tests/real_server_smoke.py AP_SOURCE GENERATED_TEST_SEED_ZIP build/direct-host/ap_peer
AP_PYTHON tests/network_smoke.py build/direct-host/ap_peer build/direct-host/ap_tls_probe
```

Verification on 2026-09-01 includes native state/journal/protocol/ABI tests under AddressSanitizer and UndefinedBehaviorSanitizer, all 120 shop labels in the bounded scout processor, the direct RDRAM label-snapshot ABI, and a real AP 0.6.7 server accepting direct test-profile authentication, scouting/name resolution, checks, item delivery and goal completion, reconnect replay and invalid-password/seed rejection. A separate local original-world campaign server test covers direct campaign authentication, durable checks/items, duplicate replay and reconnect recovery. Local fault tests cover interrupted connections, duplicate items, malformed JSON, oversized frames/fragments, a trusted test TLS certificate, hostname mismatch and rejection of self-signed certificates by the production client. Host tests simulate checks; they do not collect bananas inside the game. Android/MIPS builds and the actual Java mod-import path are checked separately, including 16 KiB ELF segment alignment.

**Device validation is still required:** Android library loading, keyboard/settings entry, physical item effects and banana collections, pause/resume, save/restart restoration and long sessions. Successful native builds and host server tests do not establish those behaviors.

The remaining full-randomizer work is the game layer: a versioned recomp seed export, independent move checks, item placement/reward replacement, shops, starting state, remaining Kong/key gameplay hooks, requirements, entrances, boss/Helm logic and campaign completion. The original `Donkey Kong 64` AP world must not be advertised as supported until those hooks and seed capabilities are implemented and tested. See `../ARCHIPELAGO.md`, `THIRD_PARTY.md` and `upstream.json`.
