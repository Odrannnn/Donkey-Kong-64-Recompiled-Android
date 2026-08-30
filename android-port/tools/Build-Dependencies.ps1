param(
    [string]$SdkPath = "$env:LOCALAPPDATA/Android/Sdk",
    [string]$NdkVersion = '27.3.13750724'
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$cmake = Join-Path $SdkPath 'cmake/3.31.5/bin/cmake.exe'
$ninja = Join-Path $SdkPath 'cmake/3.31.5/bin/ninja.exe'
$toolchain = Join-Path $SdkPath "ndk/$NdkVersion/build/cmake/android.toolchain.cmake"
$prefix = Join-Path $root '.local/prefix-arm64'
function Run-CMake([string[]]$CmakeArgs) {
    & $cmake @CmakeArgs
    if ($LASTEXITCODE -ne 0) { throw "CMake failed with exit code $LASTEXITCODE" }
}
foreach ($dependency in @('SDL', 'freetype')) {
    $source = Join-Path $root ".local/deps/$dependency"
    $build = Join-Path $root ".local/build-$dependency-arm64"
    $extra = if ($dependency -eq 'SDL') {
        @('-DSDL_SHARED=ON', '-DSDL_STATIC=OFF', '-DSDL_TEST=OFF')
    } else {
        @('-DBUILD_SHARED_LIBS=OFF', '-DFT_DISABLE_ZLIB=ON', '-DFT_DISABLE_BZIP2=ON',
          '-DFT_DISABLE_PNG=ON', '-DFT_DISABLE_HARFBUZZ=ON', '-DFT_DISABLE_BROTLI=ON')
    }
    Run-CMake (@('-Wno-deprecated', '-S', $source, '-B', $build, '-G', 'Ninja',
        "-DCMAKE_MAKE_PROGRAM=$ninja", "-DCMAKE_TOOLCHAIN_FILE=$toolchain",
        '-DANDROID_ABI=arm64-v8a', '-DANDROID_PLATFORM=android-26',
        '-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON', '-DCMAKE_BUILD_TYPE=Release',
        '-DCMAKE_POSITION_INDEPENDENT_CODE=ON', "-DCMAKE_INSTALL_PREFIX=$prefix") + $extra)
    Run-CMake @('--build', $build, '--parallel', '8')
    Run-CMake @('--install', $build)
}
