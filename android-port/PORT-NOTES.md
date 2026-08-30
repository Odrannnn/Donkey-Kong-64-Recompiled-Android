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

Upstream DK64's contribution guidance disallows AI-generated contributions. This independent Android fork contains AI-assisted changes. No upstream contribution or PR has been submitted; it is not an official Android release of the upstream project. Retain all applicable notices and review licenses before redistribution. Nintendo game assets are not provided here; the user supplies their own supported ROM.

## Android changes

- ARM64 shared-library build, Android NDK/Gradle packaging and host-side shader embedding.
- Local generation of recompilation inputs using WSL LLVM for MIPS patches.
- SAF ROM import with exact size, byte-order normalization and supported SHA-1 validation.
- Separate SDL game process, app-private assets/configuration/ROM, touch overlay and VI pause/surface lifecycle hooks.
- Android Vulkan window/surface handling, single-sample frontend UI and aligned allocation compatibility.
- Optional trusted driver ZIP import with atomic selection, bounded private extraction and system-driver recovery.
- AdrenoTools loader support. A small `libdk64vulkan.so` dispatch bridge lets SDL obtain exactly the same `vkGetInstanceProcAddr` used by RT64/Volk, including Vulkan surface creation. Driver/library handles live for the game process lifetime.
- Extracted native libraries (`useLegacyPackaging`) as required by AdrenoTools' hook-library API. All built native load segments target 16 KiB alignment.
- Android mod manager with DocumentsUI import, manifest/ZIP checks, atomic single-mod updates, activation controls and removal that preserves saves/settings. Native mod-folder/install buttons route to this manager after a close-game prompt. A file lock excludes simultaneous runtime and manager access. Native companion-library imports are not supported yet.

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
