# DK64 Recompiled — experimental Android port

> [!CAUTION]
> **AI-GENERATED AND AI-ASSISTED ANDROID PORT — UNOFFICIAL AND UNAFFILIATED**
>
> This Android port contains AI-generated code and was developed with AI assistance. It is an independent, unofficial project, **not affiliated with, endorsed by, or supported by Rainchus or the upstream DK64 Recompiled / N64: Recompiled maintainers**.
>
> This disclosure applies to the Android-port changes, not the original upstream code or artwork, whose authorship and notices are retained. Please report Android-port issues [in this fork](https://github.com/Odrannnn/Donkey-Kong-64-Recompiled-Android/issues), not to upstream. Full gameplay and all-device compatibility are not established.

Local Android port of [Rainchus/Donkey-Kong-64-Recompiled 1.0.1](https://github.com/Rainchus/Donkey-Kong-64-Recompiled/releases/tag/1.0.1), targeting ARM64 tablets. **Development build: the intro and early gameplay render with Turnip; full gameplay is still being tested.** No ROM or Turnip driver is bundled.

Published source fork: [Odrannnn/Donkey-Kong-64-Recompiled-Android](https://github.com/Odrannnn/Donkey-Kong-64-Recompiled-Android), default branch `codex/android-port`. The public fork keeps upstream history and contains this Android project under `android-port/`. Initial source publication: `8e952a56021f83ec56a0f642f3f3655ff04a413b`. The separate local publication checkout is `.local/github-publish`; the current workspace and tablet installation were not moved or replaced.

## Current status

The native game and Android APK compile. ROM import, private asset storage and the touch overlay have run on a Lenovo TB520FU (Snapdragon 8 Gen 3 / Adreno 750, Android 16).

The first system-driver test crashed during renderer startup. The stock driver rejected `fbReadAnyChanges`, `fbReadAnyFull`, `fbWriteColor`, `fbWriteDepth` and `fbWriteDepthMS` compute pipelines with `VK_ERROR_UNKNOWN`; binding a failed pipeline then crashed inside `vulkan.adreno.so`. Those shader binaries pass offline SPIR-V validation. The underlying rejection is not resolved.

The updated APK adds Turnip-compatible driver import and a shared Vulkan loader for SDL and RT64. **Import and actual Turnip loading are verified on the tablet, and the DK Rap intro renders.** RT64 reports `Turnip Adreno (TM) 750`; the five compute-pipeline failures disappear. Device validation also exposed an incorrectly reduced bindless descriptor layout; this port restores the upstream range capacity instead of allocating one slot for an 8,192-entry shader array. Saves, controller mappings, performance, suspend/resume and long sessions are not yet validated. The source patches retain the desktop game's timing and DK64-specific fixes.

Tested driver: KIMCHI's regular [Turnip v26.0.0 R8](https://github.com/K11MCH1/AdrenoToolsDrivers/releases/tag/v26.0.0-rc08), `Turnip_v26.0.0_R8.zip`, SHA-256 `e634db0f929e2205e95511c769071817d0390180ec72c8e690bc76375e813715`. It is a separate user import, not an APK asset. The test ZIP on the tablet is named `DK64-test-Turnip_v26.0.0_R8.zip`.

## Use the development APK

**Graphics driver warning:** the system Vulkan driver may cause a black screen or crash. A compatible Turnip driver may be required on supported Qualcomm Adreno GPUs. Turnip is not compatible with every GPU and is imported separately; custom-driver import requires Android 9 or newer. The same warning is displayed above **Launch game** in the app.

1. Download **[Android dev12](https://github.com/Odrannnn/Donkey-Kong-64-Recompiled-Android/releases/tag/v1.0.1-android-dev12)**, or build it below. This is a regular GitHub release. The signed APK uses the same certificate as earlier builds so those installations can update without uninstalling. Save your progress before updating; installation closes the running game.
2. Choose **Import US DK64 ROM** and select your own original US ROM. `.z64`, `.n64` and `.v64` byte orders are supported; validation uses the normalized SHA-1, not the extension. Existing user files are never modified.
3. To try Turnip, choose **Import graphics driver ZIP** and select a trusted ARM64 driver package compatible with the tablet's GPU and Android version. Standard `meta.json` / `libraryName` packages are supported. ZIPs without metadata must have an unambiguous `libvulkan_freedreno.so`, `vulkan.freedreno.so` or `libvulkan.so`.
4. Press **Launch game**. The selected driver applies to the new game process. The launcher shows the selection and the last initialized GPU separately.
5. If a driver prevents launch, return to the launcher and choose **Use system driver**, or import another compatible ZIP. Resetting preserves imported packages and game data. It does not alter the tablet's system driver.

Driver ZIPs execute native code inside this app: import only from sources you trust. Imports are private to this app, checked for unsafe/duplicate paths, size limits and ARM64 ELF libraries, and made read-only. Custom drivers require Android 9 or newer. A library-load failure falls back to the system driver; a later driver crash is reported on the launcher rather than automatically retrying indefinitely. Selecting a driver does not establish that it supports this renderer.

Touch controls include an analog stick, a separate **D-pad above the left shoulder buttons**, A/B/Z/L/R, C buttons and Start. The D-pad supports diagonals and sliding between directions; the center is neutral. **Touch** hides/shows the overlay; **Menu** sends Escape to the native menu. Native menus also accept touch events outside the controls. Bluetooth/USB controller support comes from SDL; mappings need device testing. Online mod discovery and native desktop file pickers are disabled on Android.

**Dev12 text entry:** tapping a native menu text field now starts SDL text input,
which asks Android to show the software keyboard. This is used by mod settings
such as the DK64 LAN co-op guest IPv4 field. Leaving the field stops text input.

**Dev11 audio recovery:** Android now waits for activity resume, window focus and the render surface before restarting game work. After SDL resumes its audio backend, the app discards stale queued audio and resampling history, then fades in fresh samples. Audio backlog recovery keeps complete stereo frames instead of the earlier byte-skipping path, which could split float samples and cause static. Automated regression checks pass, but the intermittent lock-screen problem and new touch layout still require device confirmation; this release was not installed during preparation because ADB was unavailable.

Dev11 also includes the previously local dev9/dev10 Turnip choices: **GMEM is the default for imported Turnip drivers**. Explicit sysmem or diagnostic choices are retained. Choose **Driver defaults (automatic)** under Advanced Turnip diagnostics to opt out; system-driver launches receive no Turnip debug flags. See [GRAPHICS-DIAGNOSTICS.md](GRAPHICS-DIAGNOSTICS.md) for the limited Poco test evidence and tradeoffs.

## Mods on Android

**Dev7 and later include native companion ZIP installation**, introduced in local dev5. The previous dev4 release rejects native packages. Native companion execution is verified with the separately released DK64 LAN co-op mod; the mod remains a separate release asset and is not embedded in the APK. No runtime mod-loader changes are made for this support.

Choose **Manage Mods** on the Android launcher, then **Import mod file**. The Android document picker can read Downloads and other document providers without granting access to the app's private directory. Supported imports are DK64 `.nrm` mods, `.rtz` texture packs, or a ZIP containing exactly one such file (including a nested folder). Review the mod name/version and confirm **Install** or **Update**. For a ZIP containing multiple mods, import the individual `.nrm`/`.rtz` files separately.

Tap an installed mod to **Enable**, **Disable** or **Remove** it. Removal keeps saves, mod settings and your original download. Updates match the manifest ID. Dev5 installs/replaces/removes the archive and declared native bridge as a recoverable transaction; startup restores the previous complete installation if a process interruption left it unfinished. The native game's Mods menu still handles mod options, ordering, compatibility errors and dependency details. Dependencies required by another enabled mod may be enabled automatically by the runtime.

The in-game **Install Mods** button routes to Android import, and **Open Mods Folder** is replaced by **Manage Mods**. These routes prompt you to close the game session first; save before confirming. Changes apply on the next launch. A cross-process file lock prevents replacing mod files or editing `mods.json` while the game is running, including while it is in the background.

Import only trusted mods: they execute code with this app's permissions, including networking in dev5. The importer rejects unsafe/duplicate paths, corrupt entries, wrong-game manifests, `.offline.nrm` files and non-mod formats such as `.apworld`. Windows DLLs, macOS libraries and native libraries embedded inside `.nrm` are rejected. Limits are 512 MiB for the selected file, 2 GiB total expanded contents across the outer/inner archives, 32,768 entries and 256 KiB for `mod.json`. Full mod API/version/content compatibility is checked by the runtime at launch; passing import checks does not guarantee gameplay compatibility or make an untrusted mod safe.

For a native companion mod, import an **Android ZIP containing one `.nrm` and one declared ARM64 `.so` beside it**, optionally together in a nested folder. The existing `native_libraries` manifest maps the library stem to exported functions; e.g. `"native_libraries": {"dk64_coop_bridge": ["bridge_ping"]}` requires `dk64_coop_bridge.so`. The v0.42.0 co-op Android ZIP follows this layout and is available beside the dev12 APK in the GitHub release. Keep native libraries self-contained apart from public Android NDK dependencies/the app's C++ runtime; match the runtime's native API and build 16 KiB-aligned load segments. The `.so` limit is 64 MiB. Libraries are never executed during import. Missing, undeclared, conflicting or shared companion filenames are refused. Import a complete ZIP for updates, and close the game before replacing native libraries on either platform. If a mod manifest becomes corrupt, removal deletes only its archive because native-file ownership cannot be established safely.

The tablet's native log confirms import and loading of `dk64_tag_anywhere.nrm`, followed by game startup with Turnip. Its gameplay behavior has not been independently verified. No third-party mod or test fixture is bundled in the APK.

The initial dev3 mod-manager build is installed on the tablet. The final downloadable dev3 APK additionally keeps the native **Install Mods** button enabled during gameplay (it uses the close-game prompt) and retains the mod-storage lock until the game process exits. Those last adjustments were built and checked but not reinstalled while the user was playing. The complete native-button handoff and on-device toggle/removal sequence still need verification. Tap **Touch** to hide the controller overlay if it covers native menu controls.

## Build on this Windows workstation

Prerequisites: Git, Python 3, JDK 17, Android SDK platform/build-tools 35, NDK `27.3.13750724`, CMake `3.31.5` and Ninja. WSL Ubuntu needs GCC/G++, CMake, Ninja, Make, Python, `apt-get` and `dpkg-deb`. The host recompilation script extracts LLVM 21 packages locally; it does not install system packages or use sudo. Its package step requires a distro repository offering LLVM 21 (tested on Ubuntu resolute).

```powershell
# Restore pinned source repositories and apply Android patches.
python tools/bootstrap_sources.py

# Replace the path below with your own original US ROM.
python tools/prepare_rom.py 'C:\path\to\your\Donkey Kong 64 (USA).n64'

# Run from this workspace through its /mnt/c/... path in WSL.
wsl bash '/mnt/c/src/DK64-Android/android-port/tools/prepare-game.sh'

$env:JAVA_HOME = 'C:\path\to\jdk-17'
./tools/Build-Dependencies.ps1
./tools/Build-Android.ps1 -PythonPath 'python'

./tools/Test-DriverImport.ps1
./tools/Test-GraphicsDiagnostics.ps1 # Default migration, explicit overrides and system-driver isolation.
./tools/Test-TouchLifecycle.ps1 # D-pad directions and activity/focus/surface gating.
./tools/Test-ModImport.ps1 # Uses cached org.json 20240303 on the host; or supply -JsonJar.
# In WSL, run tools/Test-AndroidAudio.sh for SDL dummy-output ASan/UBSan regression tests.
python tools/verify_apk.py android/app/build/outputs/apk/debug/app-debug.apk
```

The normalized US ROM SHA-1 is `cf806ff2603640a748fca5026ded28802f1f4a50`. Recompiled game/RSP code and MIPS patches are generated locally from that input. Never commit or distribute `.local/rom-source`, ROM files, the decompressed ROM or generated game-source directories.

The Gradle wrapper packages prebuilt native libraries from `.local/game-arm64`; running Gradle alone does **not** rebuild C++. Use `Build-Android.ps1` after native source edits. The output is `android/app/build/outputs/apk/debug/app-debug.apk`. Python and SDK paths can be supplied explicitly to the build scripts. Keep the local debug signing key if you want future builds to update this installation without uninstalling it.

For the non-debuggable release variant, run `./tools/Build-Android.ps1 -Variant Release` (plus `-PythonPath` if needed). Its output is `android/app/build/outputs/apk/release/app-release.apk`. This release configuration deliberately reuses the local development signing identity so existing testers can update. Keep that private keystore backed up and outside Git. A build made on another machine normally has a different signing certificate.

## Diagnostics and maintenance

Dev12 retains the original DK64 Recompiled banana app icon and the AI/unaffiliated-project disclosures introduced in dev8. The icon is reused unchanged, with attribution in [android/ICON-NOTICE.md](android/ICON-NOTICE.md); it is not AI-generated. Dev12 adds native-menu software-keyboard activation and retains the dev11 audio changes; the renderer is unchanged.

**Dev11** includes the dev10 change to default to `TU_DEBUG=gmem` for all imported Turnip drivers. The user reported correct water rendering on Poco F8 Ultra / Adreno 840 with V35 using GMEM alone, without strict synchronization. New installations and the old empty Off/default selection adopt GMEM; explicit sysmem and other diagnostic selections stay intact. **Turnip compatibility mode (sysmem)** enables the fallback that also resolved the water issue with V30 and V35; switching it off returns to GMEM. **Driver defaults (automatic)** in Advanced Turnip diagnostics explicitly opts out. The system driver is unchanged, and settings apply on the next game launch. Broader device compatibility, performance and the exact cause remain unconfirmed. See [GRAPHICS-DIAGNOSTICS.md](GRAPHICS-DIAGNOSTICS.md) for the evidence and limits.

The `run-as` commands below require the **Debug** variant; they are unavailable in the non-debuggable release APK. Launcher status and system crash reports remain available.

```powershell
$adb = "$env:LOCALAPPDATA/Android/Sdk/platform-tools/adb.exe"
& $adb install -r android/app/build/outputs/apk/debug/app-debug.apk
& $adb shell am start -n io.github.dk64port/.MainActivity
& $adb shell run-as io.github.dk64port cat files/data/native.log
& $adb shell run-as io.github.dk64port cat files/data/graphics-driver.json
& $adb shell dumpsys activity exit-info io.github.dk64port
```

`native.log` is replaced each game launch. The game runs in a separate `:game` process so a native crash does not destroy the import/recovery launcher. No storage-wide permissions or root is required. Local dev5 adds `INTERNET` permission for optional LAN companion mods; it adds no online service or discovery/location permission. The original ROM, app-private ROM, configuration and saves are kept separate from APK assets.

Local checkouts and build outputs are ignored by the outer repository. **The maintained port is the Android project, platform files, tools, `upstream.lock.json` and the six source patches in `patches/`.** Run `python tools/snapshot_sources.py` after editing upstream or nested dependencies, then keep the updated patches and lock file with your changes. The bootstrap refuses a dependency on an unexpected commit and does not overwrite existing edits.

See [PORT-NOTES.md](PORT-NOTES.md) for dependencies, attribution and verification limits. Do not treat a successful APK build as evidence of working gameplay.

An optional [Archipelago integration-test mod](archipelago/README.md) is implemented locally, with direct Android WebSocket/TLS networking inside the mod, isolated saves, five first special moves and two Japes checks using original DK64 Randomizer IDs. **No desktop relay is needed. It is not the full randomizer and cannot play ordinary DK64 AP seeds.** No APK changes or automatic installation are made. See [ARCHIPELAGO.md](ARCHIPELAGO.md) for the remaining game-logic port and device-validation requirements.
