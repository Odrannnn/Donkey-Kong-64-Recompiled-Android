# Release build

Run the release entry point from PowerShell on the Windows host:

```powershell
& .\coop\tools\Build-Release.ps1
```

It resolves the workspace from the script location, so spaces in the checkout
path are supported. It derives the semantic version from `mod.toml`, the
protocol version from `native/protocol.hpp`, and the required `dk64_coop_tick_vN`
export from those values. The manifest, source import and built NRM must agree.
The script requires Android SDK CMake 3.31.5, Ninja, NDK 27.3.13750724, and its
Windows ARM64 API 29 compiler. It also requires `JAVA_HOME` to point to JDK 17
or newer and the pinned test-only `.local/mod-tests/json.jar`. It configures a
fresh isolated Android cache, then verifies its ARM64/API 29/NDK settings.

Native sanitizer tests run with WSL Ubuntu's explicitly selected `/usr/bin/c++`;
the release preflight currently requires and records GNU C++ 15.2.0. MIPS NRM
and Windows x64 work use the repository-pinned LLVM, RecompModTool, and
llvm-mingw toolchains. Android work runs in the Windows host
context, which is required by the Android SDK tools. The script builds and logs
every CTest registered by CMake, MIPS NRM, Android ARM64, and Windows x64;
checks the protocol/export and binary formats; then packages only after every
check succeeds. Finally it compiles the Android app's real `ModStore` importer
and installs the completed Android ZIP into a temporary build directory,
verifying that the NRM and native companion arrive byte-for-byte unchanged.

Results are written beneath `coop/build/build-v<manifest-version>/`, including
individual command logs and `build-status.json`. It also builds and verifies the
compatible Android application, then writes its APK, the release archives, and
`SHA256SUMS.txt` to `coop/dist/`. The APK and Android co-op ZIP remain separate
because Android installs the application while the in-app mod importer installs
the ZIP, but both are produced and checksummed by the same release run. The entry
point does not install, launch, commit, push, or publish anything, and it does not
perform gameplay testing.

The build passes the resolved workspace explicitly to every CMake target and to
`build-mod.sh`. RecompModTool receives a generated build-local manifest whose
paths are absolute; the checked-in manifest stays relocatable. This prevents an
old adjacent checkout or caller working directory from silently supplying the
wrong symbols, ELF, or toolchain.

If an earlier run failed, its directory is retained for inspection and the
default command refuses to overwrite it. After inspection, rerun with the
following explicit option to remove only the derived
`coop/build/build-v<manifest-version>/` directory and start again:

```powershell
& .\coop\tools\Build-Release.ps1 -Clean
```
