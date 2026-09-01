$ErrorActionPreference = 'Stop'
$projectPath = Split-Path $PSScriptRoot -Parent
$sdkPath = if ($env:ANDROID_HOME) { $env:ANDROID_HOME } else { Join-Path $env:LOCALAPPDATA 'Android/Sdk' }
$ndkPath = Join-Path $sdkPath 'ndk/27.3.13750724'
$cmakePath = Join-Path $sdkPath 'cmake/3.31.5/bin/cmake.exe'
$ninjaPath = Join-Path $sdkPath 'cmake/3.31.5/bin/ninja.exe'
& $cmakePath -S $projectPath -B "$projectPath/build/android" -G Ninja `
    "-DCMAKE_MAKE_PROGRAM=$ninjaPath" "-DCMAKE_TOOLCHAIN_FILE=$ndkPath/build/cmake/android.toolchain.cmake" `
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-29 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE) { throw 'Android configure failed' }
& $cmakePath --build "$projectPath/build/android" --parallel 4
if ($LASTEXITCODE) { throw 'Android build failed' }
