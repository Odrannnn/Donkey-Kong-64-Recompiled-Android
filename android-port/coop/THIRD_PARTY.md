# Sources and build tools

- DK64 Recompiled: https://github.com/Rainchus/Donkey-Kong-64-Recompiled,
  prepared parent workspace based on `c6730d2f244d7b2d9d8c47c94c2eecfa1bfb1a43`.
  GPL-3.0. Game structs and symbol files are build inputs; no ROM is distributed.
- N64Recomp native ABI / RecompModTool: parent pinned commit
  `81213c1831fab2521a6a5459c67b63437d67e253`,
  https://github.com/N64Recomp/N64Recomp. See its license in the prepared source.
- Vendored `modding.h`, `recompconfig.h`, `recomputils.h`, and `mod.ld`:
  https://github.com/Zelda64Recomp/MMRecompModTemplate at
  `0c3e82c919dc16993ccc32f1dacb8b74ad19f8ac`, CC0-1.0.
  Unmodified copies and original license are in `vendor/recomp/`.
- DK64 Tag Anywhere 1.0.1 at
  `b7e009133ba3d716a0d214a5fe0131b828f35e18` was inspected for frame-event,
  player-swap, save and mod-conflict compatibility:
  https://github.com/Killklli/DK64TagAnywhereRecomp. Its gameplay code is not
  bundled here.
- Windows cross-compiler: official llvm-mingw release 20260826,
  https://github.com/mstorsjo/llvm-mingw/releases/tag/20260826,
  `llvm-mingw-20260826-ucrt-ubuntu-22.04-x86_64.tar.xz`.
  SHA-256: `cee8d2ce3da5145ce4dc882e70d0b0719a783d53a99752c60948fc0659975a65`.
  Toolchain stays outside the project; linked runtime notices are in `licenses/`.
- Android NDK 27.3.13750724; statically linked libc++ runtime.
  See distributed runtime notices in `licenses/`.

This is an unofficial fan mod, not affiliated with Nintendo, Rare, Microsoft,
the original recomp authors, or the referenced projects. Implementation is AI-assisted.
