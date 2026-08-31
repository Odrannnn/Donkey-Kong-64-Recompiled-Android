# Port notes and attribution

## Sources

All source revisions, including nested dependencies, are pinned in `upstream.lock.json`. Original notices and licenses remain in their source checkouts.

- DK64 Recompiled: Rainchus and contributors; upstream 1.0.1, commit `c6730d2f244d7b2d9d8c47c94c2eecfa1bfb1a43`.
- N64ModernRuntime, N64Recomp, RecompFrontend, RT64 and their dependencies: their respective authors; see upstream licenses.
- Android integration reference: [AurelioB/BanjoRecomp-Android](https://github.com/AurelioB/BanjoRecomp-Android). Selected SDL Vulkan, private paths, generated shader dependencies and VI pause changes were adapted without replacing DK64's full runtime/frontend/renderer versions.
- Plume Android/loader hooks adapted from AurelioB/plume `2073b0440749f0f6333f13281fb75e6a44da7206`; Plume copyright renderbag and contributors, MIT license.
- Android null file-dialog backend adapted from AurelioB/nativefiledialog-extended `f64980da56c0ef76b4f3fa0bc16b9d084d890741`; see that dependency's license.
- [AdrenoTools](https://github.com/bylaws/libadrenotools): Billy Laws and contributors, BSD-2-Clause; revision `8fae8ce254dfc1344527e05301e43f37dea2df80`. Its nested liblinkernsbypass is pinned as well. Only the custom-driver loading feature is used; no GPU overclocking or system-driver replacement.
- SDL 2.32.10 and FreeType 2.13.3; see their source licenses.

Upstream DK64's contribution guidance disallows AI-generated contributions. At the user's request, the Android sources were published in the independent [Odrannnn/Donkey-Kong-64-Recompiled-Android fork](https://github.com/Odrannnn/Donkey-Kong-64-Recompiled-Android), with AI assistance and verification limits disclosed. No upstream contribution or PR has been submitted. Retain all applicable notices and review licenses before redistribution. Nintendo game assets are not provided here; the user supplies their own supported ROM.

## Android changes

- ARM64 shared-library build, Android NDK/Gradle packaging and host-side shader embedding.
- Local generation of recompilation inputs using WSL LLVM for MIPS patches.
- SAF ROM import with exact size, byte-order normalization and supported SHA-1 validation.
- Separate SDL game process, app-private assets/configuration/ROM, touch overlay and VI pause/surface lifecycle hooks.
- Android Vulkan window/surface handling, single-sample frontend UI and aligned allocation compatibility.
- Optional trusted driver ZIP import with atomic selection, bounded private extraction and system-driver recovery.
- AdrenoTools loader support. A small `libdk64vulkan.so` dispatch bridge lets SDL obtain exactly the same `vkGetInstanceProcAddr` used by RT64/Volk, including Vulkan surface creation. Driver/library handles live for the game process lifetime.
- Extracted native libraries (`useLegacyPackaging`) as required by AdrenoTools' hook-library API. All built native load segments target 16 KiB alignment.
- Android mod manager with DocumentsUI import, manifest/ZIP checks, recoverable bundle updates, activation controls and removal that preserves saves/settings. Native mod-folder/install buttons route to this manager after a close-game prompt. A file lock excludes simultaneous runtime and manager access. Dev7 includes the native companion-library imports described in the dev5 record below.

## Verification record — 2026-08-30

Passed: native ARM64 compilation and APK packaging; signed debug APK verification; supported US ROM normalization; tablet asset/ROM import and SDL startup; real driver ZIP import; actual Turnip device selection (`Turnip Adreno (TM) 750`); rendering of the DK Rap intro; CPU-side driver archive tests (valid ARM64, unsafe/duplicate paths, wrong ABI, truncation, invalid ZIP, too many entries and decompression size limit); APK native-library/architecture/alignment/content checks; exported source patches checked against the modified working trees.

Failed on tablet with the stock Adreno 750 driver: five framebuffer compute pipelines returned `VK_ERROR_UNKNOWN`, followed by a null pipeline crash in `vkCmdBindPipeline` during RT64's dummy workload. Offline SPIR-V validation passes. Device validation found an additional invalid bindless descriptor capacity introduced in the borrowed Plume changes; it has been removed, restoring upstream range capacity. No shader algorithm has been replaced to conceal the stock-driver failure. Turnip successfully creates the pipelines and starts the game.

Not yet verified: full gameplay; input mappings; save persistence; resume/rotation; thermal/performance behavior. Wireless debugging reconnected on the tablet's new port and device tests resumed. Successful intro rendering is not proof of full game compatibility.

The final APK also forces Android fullscreen mode and keeps touch controls inside reported system-bar insets. It compiled and passed APK checks, but the second ADB interruption prevented installation/retesting of that last UI change. The installed build is the preceding version that rendered the intro with Turnip.

Later in the same session, ADB reconnected and the dev3 mod-manager build was installed with `adb install -r`. This includes the fullscreen/inset changes. The selected Turnip driver was retained. The tablet now contains `dk64_tag_anywhere.nrm`; the native log reports both opening and loading that mod, followed by game startup. This confirms real recomp-mod loading on Android, not the mod's in-game behavior. The earlier installation note above describes the previous interruption.

Mod verification: 19 host-side archive/store checks pass, covering import/update/cancel, nested ZIPs, traversal and duplicate paths, malformed/oversized manifests, wrong-game/native/AP packages, stream/entry/expansion limits, CRC corruption, texture-pack recognition, activation persistence, default-enabled behavior, backup recovery, filename collision, session locking and removal containment/save preservation. The 12 existing driver-import checks still pass. Host tests use an existing org.json JVM jar; the APK uses Android's own JSON implementation and includes no extra JSON dependency. On-device test-package toggle/removal and the native button close/reopen flow still need verification.

Early gameplay was also observed on the tablet with Tag Anywhere loaded (Donkey Kong near the starting area and one Golden Banana shown). This is additional rendering evidence, not a complete gameplay/save/controller test. Further UI test taps were stopped while the user played. The final dev3 APK keeps Android's native Install Mods button available after game start and holds the mod-storage lock until process exit, with a brief acquisition retry for the manager handoff. These final adjustments were built/checked but not installed over the active game session.

The user subsequently confirmed that Tag Anywhere works fine in-game. LAN campaign co-op is a separate, unimplemented feature; see `LAN-COOP.md` for the feasibility assessment.

Test driver: KIMCHI's regular `Turnip_v26.0.0_R8.zip` from [this release](https://github.com/K11MCH1/AdrenoToolsDrivers/releases/tag/v26.0.0-rc08), downloaded separately and checked against the release asset SHA-256 `e634db0f929e2205e95511c769071817d0390180ec72c8e690bc76375e813715`. Imported through Android DocumentsUI using the same path available to users. No Turnip library is packaged in the APK.

An official Khronos Android validation layer was downloaded into ignored `.local/validation-layers` for diagnosis. Normal APKs exclude it. For local diagnosis only, the matching layer can be packaged using Gradle `-PvulkanValidation`; the development Android Plume build discovers it if present. Do not distribute diagnostic layers or user-imported Turnip files accidentally.

## Android dev4 prerelease — 2026-08-30

- Added a persistent, high-contrast warning above Launch game: the system Vulkan driver may fail with a black screen/crash, and a compatible Turnip driver may be required on supported Adreno GPUs. It also explains the Android 9 requirement for custom-driver import.
- Version `1.0.1-android-dev4`, version code 3. The `Release` Gradle variant is non-debuggable and signed with the existing local development certificate to preserve update compatibility. No private signing material is distributed. Debug builds remain available for `run-as` diagnostics.
- Packaged the pinned dependencies' available root license/copyright/notice files, FreeType notices, and source lock metadata alongside the existing attribution. The APK includes 43 dependency notice files.
- Verified native/Gradle build, release lint checks, APK architecture/content and 16 KiB alignment, warning presence in packaged code, and valid APK signing with the same certificate as dev3. All 19 mod archive/storage checks and 12 driver checks pass.
- Previous tablet evidence covers Turnip initialization, intro/early gameplay and user-confirmed Tag Anywhere behavior. The dev4 warning and release packaging have not received a fresh on-device gameplay test; the active game session was left untouched. Full gameplay/save/lifecycle coverage and the final native mod-menu handoff remain incomplete.
- Publication is an experimental GitHub prerelease, with an APK and SHA-256 checksum; ROMs, imported graphics drivers, signing keys and third-party mods are excluded.

## Local dev5 native companion bundle support — 2026-08-30

- Co-op packaging direction: platform-specific ZIP with an unchanged `.nrm` plus its declared Android ARM64 `.so` or Windows x64 `.dll` beside it. The user explicitly requires no runtime mod-loader changes. The existing runtime loader, manifest parser and loader header have no changes for this work.
- Android importer/manager accepts one declared Android bridge per ZIP, checks ARM64 ELF structure and 16 KiB load alignment, rejects undeclared/embedded/desktop native libraries and filename conflicts, and manages the companion during update/removal. Native code is not executed during import, and validation does not establish trust or runtime compatibility.
- Journaled installation restores the previous complete file set after a process interruption. GameActivity performs recovery under the existing cross-process mod lock before native startup. Tests cover interruption at every file replacement for a renamed bridge update, initial install and removal. These are process-interruption checks, not a guarantee against storage hardware failure or sudden power loss.
- Local version `1.0.1-android-dev5`, version code 4; debug APK built and signature verified with the existing development certificate. Added `INTERNET` permission for optional LAN mods, no discovery/location permission. No networking service, co-op bridge or multiplayer gameplay has been implemented or bundled.
- All 19 existing mod/store checks, 19 new native-bundle checks and 12 driver archive checks pass. Native fixtures are structural test data and were never executed. Native compilation/Gradle packaging and APK ARM64/content/16 KiB checks pass.
- Full `lintDebug` reports 27 errors, all in unchanged SDL Java files (Bluetooth/audio/vibration permissions and a broadcast-receiver flag). No new importer errors are reported. These SDL findings remain unresolved; no lint suppression or unrelated permission grants were added.
- This development APK has not been installed on the tablet or published to GitHub. On-device native companion loading and Windows runtime compatibility remain unverified. Existing game sessions and the dev4 GitHub release are unchanged.

## Android dev7 release — 2026-08-30

- Version `1.0.1-android-dev7`, version code 6. Published as a regular GitHub release, not a prerelease, at the user's request. This release status does not establish complete gameplay or device compatibility.
- Adds a persistent **Turnip compatibility mode** toggle and Adreno 840 information box. It selects `TU_DEBUG=sysmem` before loading the custom driver. Existing dev6 selections are retained; new installations default to Off. Other isolated driver experiments remain under Advanced Turnip diagnostics. No system-driver settings, game assets, native shaders or renderer algorithms were changed for this workaround.
- The user reports that this mode removes flashing/speckled water on Poco F8 Ultra / Adreno 840 with Turnip Gen8 V30 and V35. ADB verified the V30 custom-driver launch and sysmem flags. Dev7 was installed on the phone, and its launcher controls, V35 selection and retained compatibility choice were checked. This is a user-confirmed workaround, not a confirmed root-cause diagnosis. Performance, long-session behavior and other GPUs remain unverified.
- Includes the previously local dev5 native companion-bundle importer, recovery journal and optional-mod INTERNET permission. Native companion execution remains unverified; no co-op or Archipelago implementation is included in this release.
- The downloadable Release APK is non-debuggable and retains the existing signing certificate for in-place updates. Phone UI verification used the Debug variant; the separate Release packaging has not received a fresh on-device gameplay test. ROMs, user-imported drivers, mods, private signing material and validation layers are excluded.
- See `GRAPHICS-DIAGNOSTICS.md` for the detailed test record. The earlier dev4/dev5 sections above are historical snapshots.

## Android dev8 release — 2026-08-30

- Version `1.0.1-android-dev8`, version code 7. Regular GitHub release, not a prerelease. The package remains `io.github.dk64port` and retains the existing signing identity for updates.
- Uses an unchanged copy of upstream `icons/app.png` for the launcher and round-icon reference. The original SVG and its embedded attribution metadata are packaged with `android/ICON-NOTICE.md`. The icon is not AI-generated.
- Adds a prominent disclosure to the GitHub and Android-port READMEs and release notes: the Android port contains AI-generated and AI-assisted changes and is not affiliated with, endorsed by, or supported by the upstream maintainers. Original upstream authorship, artwork and notices are retained.
- No Java gameplay/launcher behavior, native renderer, shader, driver-selection or compatibility-mode changes. Verification for this update covers release packaging, icon inclusion, signing and comparison against dev7's native libraries. It does not add new on-device gameplay evidence; the phone's active session is left untouched.

## Local dev9 GMEM diagnostic — 2026-08-31

- Version `1.0.1-android-dev9-gmem-test`, version code 8. Local Debug build for the user-requested `TU_DEBUG=gmem,flushall,syncdraw` experiment; not a new GitHub release.
- Adds separate GMEM-only and GMEM-with-strict-synchronization choices under Advanced Turnip diagnostics. Existing saved flags and the sysmem compatibility toggle are retained. Applies only to an imported driver on the next game-process launch.
- The launcher explains that strict synchronization is an experiment, not a high-accuracy setting or a confirmed fix, and can be very slow. No native renderer or shader changes. See `GRAPHICS-DIAGNOSTICS.md` for the comparison and recovery procedure. No new on-device rendering or performance result is claimed.

### Subsequent Poco test result

- The dev9 diagnostic APK was installed on Poco F8 Ultra, retaining app data and Turnip Gen8 V35. The user selected **GMEM rendering — experimental** and reported that it works perfectly in the water test.
- Read-only ADB confirmed version code 8, saved `gmem`, and a fresh launch with `TU_DEBUG=gmem; customDriver=true`, Turnip Gen8 V35 and Adreno 840. This is GMEM alone; the combined `gmem,flushall,syncdraw` mode has no confirmed result. The active game and selected mode were left untouched.
- This updates the earlier pre-test note above. It is user-reported visual evidence, not a measured performance or full-game result. Native libraries remain identical to dev8. The tablet and GitHub releases were not changed.

## Local dev10 GMEM default — 2026-08-31

- Version `1.0.1-android-dev10`, version code 9. Makes `gmem` the app default for all imported Turnip drivers, at the user's request. System-driver launches still receive no Turnip debug flags. This is a local update; dev8 remains the public release.
- Missing/default preferences and the legacy empty Off selection adopt GMEM. Existing explicit sysmem and diagnostic selections survive updating. A separate `driver-defaults` saved value provides an explicit automatic-driver opt-out without being mistaken for the legacy default.
- Retains the sysmem compatibility switch; turning it off returns to GMEM. Other advanced diagnostics replace the default. The launcher explains the current behavior and the limits of the Poco result.
- No native renderer, shader, mod-loading or game changes. Host regression checks cover default migration, retained explicit modes, automatic opt-out persistence and isolation from the system driver. No further on-device gameplay or general performance improvement is claimed, and neither device is updated while the user plays.

## Android dev11 — audio resume and touch D-pad — 2026-08-31

- Version `1.0.1-android-dev11`, version code 10. Non-debuggable ARM64 Release APK, with the existing signing certificate for in-place updates. This release also carries the dev9/dev10 GMEM choices/default described above; explicit graphics choices are preserved.
- Addresses a concrete audio corruption path: the former backlog catch-up divided byte counts and could queue a partial float or stereo frame, with an unbounded shift for long backlogs. Android now queues whole stereo frames and drops stale backlog at a 100 ms bound. Queue timing uses the output rate. Short buffers and resampling boundaries are checked.
- Audio waits for a resumed, focused activity with a surface. Java publishes atomic lifecycle state only; it never waits on SDL audio locks. The SDL event thread clears queued samples and interpolation history after SDL's backend resumes, and applies a 10 ms fade. Audio production and converter updates are serialized. A pause followed quickly by resume is retained as a generation change. Desktop audio code and the runtime mod loader are unchanged.
- Adds a separate N64 D-pad above Z/L, supporting cardinal directions, diagonals, finger sliding and simultaneous analog/buttons. Opposite directions cancel; release, cancel, focus loss, pause and overlay hiding clear held input.
- Automated validation: real SDL conversion with a dummy output under ASan/UBSan; partial-frame/large-backlog cases, 50 simulated backend-pause cycles and 1,000 concurrent producer/resume cycles; 45 Java D-pad/lifecycle checks; graphics, mod and driver import regressions; APK/signature/16 KiB library checks.
- **Device validation pending:** ADB could not connect to the tablet during this update. The user reports that switching apps restores audio after lock-screen static. The identified corruption path is corrected, but eliminating every device-specific resume glitch and the D-pad's physical layout have not been confirmed on a device. No APK was installed, no saves/driver selections were altered and no co-op mod is included in this release.
- Audio API references: [SDL_QueueAudio](https://wiki.libsdl.org/SDL2/SDL_QueueAudio) and [SDL_ClearQueuedAudio](https://wiki.libsdl.org/SDL2/SDL_ClearQueuedAudio). SDL clearing cannot retract samples already sent to hardware; a brief old tail is still possible.
