# Adreno 840 water investigation

## Current default — included in dev11, 2026-08-31

At the user's request, the app now defaults to `TU_DEBUG=gmem` for all imported Turnip drivers, regardless of GPU. The system driver still receives no Turnip flags. This is a default policy based on the Poco test below, not a claim of compatibility or improved performance on every device.

New installations, missing/invalid saved selections and the old empty **Off — driver defaults** selection resolve to **GMEM rendering — default**. Existing explicit `sysmem`, `gmem`, depth/compression or strict-sync selections are preserved. The stored strings, rather than the old menu indices, determine the choice.

**Turnip compatibility mode (sysmem)** remains a fallback: On selects only `sysmem`; switching Off returns to GMEM. Under Advanced Turnip diagnostics, **Driver defaults (automatic)** explicitly sends no debug flags. This opt-out is saved as `driver-defaults` so it is distinguishable from the old empty default after restarting. Other diagnostic modes replace the default; they do not silently inherit GMEM or sysmem. All changes apply to a new game-process launch.

Dev11 includes the default change first built locally in dev10, alongside audio recovery and touch D-pad changes. No device installation was performed while preparing dev11. The older sections below describe their respective builds.

## Observations

- User device: Poco F8 Ultra / Adreno 840. User reports flashing and speckled water throughout the game with StevenMXZ Turnip Gen8 V30 and V35, while their Snapdragon 8 Gen 3 device renders correctly.
- Setting Antialiasing to None and Framerate to Original did not help, according to the user.
- The phone's Android log confirms `Custom Vulkan loader initialized`, `Turnip Gen8 V30`, and `Adreno (TM) 840 ()`. This confirms that launch used the imported driver rather than the fallback system driver.
- A user screenshot shows heavy speckling across the water. A separate ADB screenshot caught a water scene without the same heavy speckling; a single frame cannot establish temporal correctness.
- With **System-memory rendering (`TU_DEBUG=sysmem`)** enabled in dev6 on the Poco using Turnip Gen8 V30, the user revisited water and reported that the corruption seemed fully gone. This is a user-confirmed workaround for that tested combination.
- The user also reports that the sysmem workaround works correctly with Turnip Gen8 V35 on the same phone. That V35 result is user-reported; the recorded ADB startup verification used V30.
- The initial sysmem result suggested an issue involving the tiled rendering path or its interaction with RT64, but the subsequent forced-GMEM result below means it is not evidence that GMEM always fails. The exact root cause is **not confirmed**. No water shader or native renderer changes have been applied. Other devices, performance impact and long-session stability have not been verified.

## Local dev9 GMEM experiment — 2026-08-31

At the user's request, the local `1.0.1-android-dev9-gmem-test` build adds two entries under **Advanced Turnip diagnostics**:

| Mode | TU_DEBUG | Purpose |
| --- | --- | --- |
| GMEM rendering — experimental | `gmem` | Requests tiled rendering as a comparison baseline |
| GMEM + strict synchronization — very slow | `gmem,flushall,syncdraw` | Requests tiled rendering with cache flushes before GPU operations and waits after them |

Turnip's [documented flags](https://docs.mesa3d.org/envvars.html#turnip-driver-environment-variables) are debugging controls, not a general high-accuracy preset. This experiment may narrow down a synchronization/cache problem, but does not establish the cause of the water corruption and may be slower than sysmem. An imported driver's support and any render-pass restrictions still apply; the app's log confirms requested flags, not the memory path actually used for every pass.

Existing saved selections, including `sysmem`, remain unchanged after updating. Select **GMEM + strict synchronization — very slow**, save, then start a fresh game process. Compare the same water scene using the same imported driver and game graphics settings. Compare GMEM alone in a separate launch if useful. Record water stability and performance separately. Save game progress before ending a session.

To restore the user-confirmed workaround, enable **Turnip compatibility mode** and start a fresh game process. That selects only `sysmem`; the GMEM/synchronization flags are not combined with it. **Off — driver defaults** restores the driver's normal mode selection, which is not the same as forcing GMEM. No settings are applied to the system Vulkan driver or other apps. The native renderer and shaders are unchanged.

This is a local diagnostic build, not a published replacement for dev8.

### Poco result — 2026-08-31

The dev9 Debug APK was installed on the Poco F8 Ultra with an in-place update. Turnip Gen8 V35 and the previous `sysmem` selection were retained immediately after installation. The user then selected **GMEM rendering — experimental** and reported that it "works perfectly" in the ongoing water test.

Read-only ADB verification confirms saved flags `gmem` and, at `08-31 00:27:29`, `Launch diagnostics: TU_DEBUG=gmem; customDriver=true`. The same process initialized Turnip Gen8 V35 on Adreno 840. This verifies the requested **GMEM-only** mode, not `gmem,flushall,syncdraw`. No successful combined-mode test is claimed. No setting was changed or game session interrupted during this verification.

The visual improvement is user-reported; sustained performance, full gameplay and long-session stability remain unmeasured. Forcing GMEM differs from letting the driver choose its rendering path automatically. Both forcing sysmem and forcing GMEM have now been reported to avoid the water symptom in this setup, so the earlier sysmem result does not isolate the cause to GMEM itself. No transition/autotuning bug is proven. Leave GMEM selected for this test, with the existing sysmem option available if corruption returns.

## Local dev7 compatibility toggle

The launcher now exposes **Turnip compatibility mode** as an on/off switch with an information box explaining that it may fix flashing or speckled water and other graphical glitches on Adreno 840. The box names the user-tested Poco F8 Ultra / Turnip Gen8 V30 and V35 combinations, explains that it uses system-memory rendering, warns that performance may vary, and says that changes apply on the next game launch. It recommends leaving the switch off when graphics already work correctly.

The switch uses the same private saved selection as dev6: an existing `sysmem` selection appears enabled without migration or rewriting data. New installations default to Off. Turning the switch on selects `sysmem`; turning it off restores driver defaults. Other modes remain under **Advanced Turnip diagnostics** and replace compatibility mode rather than being combined. Selecting an advanced mode updates the switch to reflect the saved state. Returning to the system driver does not discard the preference, but no Turnip debug flags are passed to system-driver launches. No renderer or shader changes are required.

## Local dev6 diagnostic build

The launcher adds **Turnip diagnostics**. Off is the default. Each selection is saved atomically in app-private `gpu-drivers/diagnostics.txt` and read again in the game process on launch. SDL's native environment setter applies `TU_DEBUG` before the game's Vulkan loader opens the imported driver. System-driver launches receive no diagnostic flags. No Android system properties or device-wide driver settings are changed.

Select one mode, save, and start a new game session. Compare the same water scene. Changing the selection does not affect an already running session. Save game progress before closing or updating the app.

| Mode | TU_DEBUG | What the comparison tests |
| --- | --- | --- |
| Off | empty | Unmodified driver behavior |
| System-memory rendering | `sysmem` | Bypasses tiled GMEM rendering |
| Disable depth optimization | `nolrz` | Disables the driver's low-resolution depth optimization, not the game's depth testing |
| Disable framebuffer compression | `noubwc` | Bypasses UBWC compression |
| Strict GPU synchronization | `flushall,syncdraw` | Flushes caches and waits after GPU operations; can be extremely slow |

For the tested Poco F8 Ultra / Turnip Gen8 V30 and V35 combinations, keep system-memory rendering enabled. Its saved selection persists across launches. There is no need to try the other diagnostic modes while this resolves the symptom. New installations and other devices still default to Off; the working Snapdragon 8 Gen 3 setup is unchanged. A change in symptoms narrows the investigation; it does not by itself prove whether the fault is in RT64 or Turnip. An imported driver may handle debug flags differently. Off restores the baseline on the next launch.

The Android log tag `DK64Graphics` records the flags passed at launch. `DK64Driver` records the initialized driver and GPU. These logs confirm requested settings and loader selection, not that a custom driver honored every flag.

This build also contains the existing local dev5 native-mod importer changes. It does not install any mods or drivers. Existing app data is retained when updating with the same signing identity. The diagnostic build is debuggable so private native logs can be read through authorized ADB; it is intended for local testing, not a published release.

## Verification and limits

Dev7 was subsequently installed on the Poco at the user's request with `adb install -r`. The launcher UI hierarchy confirms the compatibility switch, Adreno 840 information box, and advanced diagnostic control are present, with the retained selection shown as Compatibility mode (system-memory rendering). Turnip Gen8 V35 remains selected, and app data remains present. The switch was not toggled during this verification, and no changes were made to the tablet.

Java compilation and debug APK packaging pass. Package checks verify eight ARM64 native libraries, 16 KiB load alignment, and absence of bundled ROMs, imported drivers, or validation layers. All eight native libraries match dev4 byte-for-byte, and the APK signing certificate matches dev4.

After the user confirmed their progress was saved, dev6 was installed on the Poco with `adb install -r`. The app's saves, imported ROM, settings and driver selection remain present. The launcher diagnostic dialog was exercised on the phone: System-memory rendering was selected and saved, and the private diagnostics file contains `sysmem`. A new game process logged `Launch diagnostics: TU_DEBUG=sysmem; customDriver=true` and selected Turnip Gen8 V30 / Adreno 840. The native main menu renders. The user then controlled the game and reported that the water corruption seemed fully gone. This confirms the reported visual improvement, not an independently measured or comprehensive rendering test. The tablet was not changed.

References: [Mesa Turnip debug flags](https://docs.mesa3d.org/envvars.html#turnip-driver-environment-variables), [StevenMXZ Gen8 V35](https://github.com/StevenMXZ/Adreno-Tools-Drivers/releases/tag/v35), [StevenMXZ Gen8 V30](https://github.com/StevenMXZ/Adreno-Tools-Drivers/releases/tag/v30). These Gen8 version numbers are not the same as whitebelyash's separately numbered mainline releases.
