param(
    [string]$SdkPath = "$env:LOCALAPPDATA/Android/Sdk",
    [string]$PythonPath = 'python',
    [switch]$NativeOnly
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$cmake = Join-Path $SdkPath 'cmake/3.31.5/bin/cmake.exe'
$ninja = Join-Path $SdkPath 'cmake/3.31.5/bin/ninja.exe'
$ndk = Join-Path $SdkPath 'ndk/27.3.13750724'
$prefix = Join-Path $root '.local/prefix-arm64'
$build = Join-Path $root '.local/game-arm64'
foreach ($required in @('upstream/RecompiledPatches/patches.c', 'upstream/RecompiledPatches/patches_bin.c',
    'upstream/rsp/n_aspMain.cpp', '.local/prefix-arm64/lib/libSDL2.so')) {
    if (-not (Test-Path (Join-Path $root $required))) { throw "Missing $required. Follow README.md preparation steps." }
}
& $cmake -Wno-deprecated -S (Join-Path $root 'upstream') -B $build -G Ninja `
    "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_TOOLCHAIN_FILE=$ndk/build/cmake/android.toolchain.cmake" `
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-26 -DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON `
    -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$prefix" "-DCMAKE_FIND_ROOT_PATH=$prefix" `
    "-DSDL2_DIR=$prefix/lib/cmake/SDL2" "-DPython3_EXECUTABLE=$PythonPath"
if ($LASTEXITCODE -ne 0) { throw 'Native configuration failed.' }
& $cmake --build $build --target DK64Recompiled --parallel 8
if ($LASTEXITCODE -ne 0) { throw 'Native compilation failed.' }
Copy-Item -LiteralPath "$ndk/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so" -Destination $build
if (-not $NativeOnly) {
    $env:ANDROID_HOME = $SdkPath
    & "$root/android/gradlew.bat" --no-daemon -p "$root/android" :app:assembleDebug
    if ($LASTEXITCODE -ne 0) { throw 'APK packaging failed.' }
}
