[CmdletBinding()]
param(
    [string] $WorkspaceRoot,
    [switch] $Clean
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Require-File([string] $Path, [string] $Name) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "$Name was not found: $Path" }
    return (Resolve-Path -LiteralPath $Path).Path
}

function Require-Directory([string] $Path, [string] $Name) {
    if (-not (Test-Path -LiteralPath $Path -PathType Container)) { throw "$Name was not found: $Path" }
    return (Resolve-Path -LiteralPath $Path).Path
}

function Quote-Bash([string] $Value) {
    return "'" + $Value.Replace("'", '''"''"''') + "'"
}

function To-WslPath([string] $Path) {
    $full = [IO.Path]::GetFullPath($Path)
    if ($full -notmatch '^[Cc]:\\') { throw "WSL path conversion supports the configured C: workspace only: $full" }
    return '/mnt/c/' + $full.Substring(3).Replace('\', '/')
}

$scriptRoot = Split-Path -Parent $PSCommandPath
$coopRoot = [IO.Path]::GetFullPath((Join-Path $scriptRoot '..'))
$workspace = if ($WorkspaceRoot) { [IO.Path]::GetFullPath($WorkspaceRoot) } else { [IO.Path]::GetFullPath((Join-Path $coopRoot '..')) }
if ((Split-Path -Leaf $coopRoot) -ne 'coop') { throw "Unexpected script location: $scriptRoot" }
Require-Directory $workspace 'Workspace root' | Out-Null
Require-Directory (Join-Path $workspace 'upstream') 'Prepared upstream source' | Out-Null
Require-File (Join-Path $coopRoot 'mod.toml') 'Co-op manifest' | Out-Null

$manifestText = Get-Content -LiteralPath (Join-Path $coopRoot 'mod.toml') -Raw
$protocolText = Get-Content -LiteralPath (Join-Path $coopRoot 'native/protocol.hpp') -Raw
if ($manifestText -notmatch '(?m)^version\s*=\s*"([0-9]+\.[0-9]+\.[0-9]+)"') { throw 'mod.toml has no semantic manifest version' }
$version = $Matches[1]
if ($version -notmatch '^0\.([0-9]+)\.[0-9]+$') { throw "Manifest version must encode the protocol in its minor component: $version" }
$manifestProtocol = [uint32]$Matches[1]
if ($protocolText -notmatch 'protocol_version\s*=\s*([0-9]+)') { throw 'protocol.hpp has no protocol_version' }
$protocolVersion = [uint32]$Matches[1]
if ($manifestProtocol -ne $protocolVersion) { throw "Manifest version $version and protocol $protocolVersion disagree" }
$expectedTick = "dk64_coop_tick_v$protocolVersion"
if ($manifestText -notmatch [regex]::Escape($expectedTick)) { throw "mod.toml does not declare $expectedTick" }
$modSourceText = Get-Content -LiteralPath (Join-Path $coopRoot 'mod/coop.c') -Raw
if ($modSourceText -notmatch [regex]::Escape($expectedTick)) { throw "mod/coop.c does not import $expectedTick" }
$releaseRoot = Join-Path $coopRoot "build/build-v$version"
$logsRoot = Join-Path $releaseRoot 'logs'
if (Test-Path -LiteralPath $releaseRoot) {
    $entries = @(Get-ChildItem -LiteralPath $releaseRoot -Force)
    if ($entries.Count -ne 0) {
        if (-not $Clean) { throw "Release directory already contains output: $releaseRoot. Inspect it, or rerun with -Clean to remove only this release directory." }
        $expectedReleaseRoot = [IO.Path]::GetFullPath((Join-Path $coopRoot "build/build-v$version")).TrimEnd([IO.Path]::DirectorySeparatorChar, [IO.Path]::AltDirectorySeparatorChar)
        $actualReleaseRoot = [IO.Path]::GetFullPath($releaseRoot).TrimEnd([IO.Path]::DirectorySeparatorChar, [IO.Path]::AltDirectorySeparatorChar)
        if ($actualReleaseRoot -ne $expectedReleaseRoot -or $actualReleaseRoot -notlike (Join-Path $coopRoot 'build/*')) { throw "Refusing to clean an unexpected path: $actualReleaseRoot" }
        Remove-Item -LiteralPath $actualReleaseRoot -Recurse -Force
    }
} else {
    New-Item -ItemType Directory -Path $releaseRoot | Out-Null
}
New-Item -ItemType Directory -Path $logsRoot | Out-Null

$commands = [System.Collections.Generic.List[object]]::new()
function Invoke-Logged([string] $Name, [string] $File, [string[]] $Arguments, [string] $Log, [string] $WorkingDirectory = $workspace) {
    $commandText = ($File + ' ' + (($Arguments | ForEach-Object { '"' + $_.Replace('"', '\"') + '"' }) -join ' ')).Trim()
    "# $commandText" | Set-Content -LiteralPath $Log -Encoding utf8
    Push-Location -LiteralPath $WorkingDirectory
    try {
        & $File @Arguments *>&1 | Tee-Object -FilePath $Log -Append
        $code = $LASTEXITCODE
    } finally {
        Pop-Location
    }
    $commands.Add([pscustomobject]@{ name = $Name; command = $commandText; log = [IO.Path]::GetFileName($Log); exit_code = $code })
    if ($code -ne 0) { throw "$Name failed with exit code $code; see $Log" }
}

$sdkRoot = if ($env:ANDROID_HOME) { $env:ANDROID_HOME } else { Join-Path $env:LOCALAPPDATA 'Android/Sdk' }
$sdkRoot = Require-Directory $sdkRoot 'Android SDK'
$androidCmake = Require-File (Join-Path $sdkRoot 'cmake/3.31.5/bin/cmake.exe') 'Android SDK CMake 3.31.5'
$androidNinja = Require-File (Join-Path $sdkRoot 'cmake/3.31.5/bin/ninja.exe') 'Android SDK Ninja 3.31.5'
$ndkRoot = Require-Directory (Join-Path $sdkRoot 'ndk/27.3.13750724') 'Android NDK 27.3.13750724'
$androidToolchain = Require-File (Join-Path $ndkRoot 'build/cmake/android.toolchain.cmake') 'Android NDK CMake toolchain'
$androidCompiler = Require-File (Join-Path $ndkRoot 'toolchains/llvm/prebuilt/windows-x86_64/bin/aarch64-linux-android29-clang++.cmd') 'Android ARM64 API 29 compiler'
$androidReadElf = Require-File (Join-Path $ndkRoot 'toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-readelf.exe') 'Android NDK llvm-readelf'

$wsl = Require-File (Join-Path $env:WINDIR 'System32/wsl.exe') 'WSL launcher'
$wslCoop = To-WslPath $coopRoot
$wslWorkspace = To-WslPath $workspace
$wslMipsLlvm = To-WslPath (Require-Directory (Join-Path $workspace '.local/llvm-wsl') 'Pinned WSL LLVM')
$wslRecomp = To-WslPath (Require-File (Join-Path $workspace '.local/host-tools-wsl/RecompModTool') 'Pinned RecompModTool')
$wslMingw = To-WslPath (Require-Directory (Join-Path $workspace '.local/coop-toolchains/llvm-mingw-20260826-ucrt-ubuntu-22.04-x86_64') 'Pinned llvm-mingw')

$toolVersions = [ordered]@{}
$toolVersions.android_cmake = (& $androidCmake --version | Select-Object -First 1)
$toolVersions.android_ninja = (& $androidNinja --version | Select-Object -First 1)
$toolVersions.android_ndk = '27.3.13750724'
$toolVersions.android_compiler = (& $androidCompiler --version | Select-Object -First 1)
$toolVersions.wsl_distribution = 'Ubuntu (explicit -d Ubuntu)'

$preflight = Join-Path $logsRoot '00-preflight.log'
Invoke-Logged 'WSL pinned toolchain preflight' $wsl @('-d', 'Ubuntu', '--cd', $wslCoop, '--', 'bash', '-lc', "set -euo pipefail; test -x /usr/bin/c++; test -x $(Quote-Bash $wslRecomp); test -x $(Quote-Bash "$wslMipsLlvm/usr/lib/llvm-21/bin/clang"); test -x $(Quote-Bash "$wslMingw/bin/x86_64-w64-mingw32-clang++"); test -x $(Quote-Bash "$wslMingw/bin/x86_64-w64-mingw32-nm"); cmake --version; ninja --version; printf 'Host C++: '; /usr/bin/c++ --version | head -n1; printf 'MIPS clang: '; $(Quote-Bash "$wslMipsLlvm/usr/lib/llvm-21/bin/clang") --version | head -n1; printf 'Windows clang: '; $(Quote-Bash "$wslMingw/bin/x86_64-w64-mingw32-clang++") --version | head -n1; printf 'RecompModTool SHA256: '; sha256sum $(Quote-Bash $wslRecomp) | cut -d' ' -f1; $(Quote-Bash $wslRecomp) --help >/dev/null") $preflight
$toolVersions.wsl_cmake = (Select-String -LiteralPath $preflight -Pattern '^cmake version ' | Select-Object -First 1).Line
$toolVersions.wsl_ninja = (Select-String -LiteralPath $preflight -Pattern '^\d+(\.\d+)+' | Select-Object -First 1).Line
$toolVersions.host_cxx = (Select-String -LiteralPath $preflight -Pattern '^Host C\+\+: ' | Select-Object -First 1).Line
if ($toolVersions.host_cxx -notmatch '15\.2\.0') { throw "Unexpected Linux host-test compiler: $($toolVersions.host_cxx)" }
$toolVersions.mips_clang = (Select-String -LiteralPath $preflight -Pattern '^MIPS clang: ' | Select-Object -First 1).Line
$toolVersions.windows_clang = (Select-String -LiteralPath $preflight -Pattern '^Windows clang: ' | Select-Object -First 1).Line
$toolVersions.recomp_mod_tool_sha256 = (Select-String -LiteralPath $preflight -Pattern '^RecompModTool SHA256: ' | Select-Object -First 1).Line

$hostBuild = Join-Path $releaseRoot 'host-tests'
$wslHostBuild = To-WslPath $hostBuild
$hostLog = Join-Path $logsRoot '01-host-tests.log'
$hostCommand = "set -euo pipefail; cmake -S $(Quote-Bash $wslCoop) -B $(Quote-Bash $wslHostBuild) -G Ninja -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DBUILD_TESTING=ON -DCOOP_SANITIZE=ON -DCMAKE_BUILD_TYPE=Debug; cmake --build $(Quote-Bash $wslHostBuild) --parallel 4; ctest --test-dir $(Quote-Bash $wslHostBuild) --output-on-failure"
Invoke-Logged 'Linux ASan/UBSan native tests' $wsl @('-d', 'Ubuntu', '--cd', $wslCoop, '--', 'bash', '-lc', $hostCommand) $hostLog
$expectedHostTests = (Select-String -LiteralPath (Join-Path $coopRoot 'CMakeLists.txt') -Pattern '^\s*add_test\(').Count
$hostSummary = [regex]::Match((Get-Content -LiteralPath $hostLog -Raw), '100% tests passed, 0 tests failed out of (\d+)')
if ($expectedHostTests -lt 1 -or !$hostSummary.Success -or [int]$hostSummary.Groups[1].Value -ne $expectedHostTests) {
    throw "Host validation count mismatch: CMake registers $expectedHostTests tests"
}

$mipsLog = Join-Path $logsRoot '02-mips-nrm.log'
Invoke-Logged 'MIPS NRM build' $wsl @('-d', 'Ubuntu', '--cd', $wslCoop, '--', 'bash', '-lc', 'set -euo pipefail; ./tools/build-mod.sh') $mipsLog
$mipsRoot = Join-Path $releaseRoot 'mips'
New-Item -ItemType Directory -Path $mipsRoot | Out-Null
Copy-Item -LiteralPath (Join-Path $coopRoot 'build/mod/coop.o') -Destination $mipsRoot
Copy-Item -LiteralPath (Join-Path $coopRoot 'build/mod/mod.elf') -Destination $mipsRoot
Copy-Item -LiteralPath (Join-Path $coopRoot 'build/mod/mod.map') -Destination $mipsRoot
$nrm = Require-File (Join-Path $coopRoot 'dist/dk64_lan_coop.nrm') 'MIPS NRM artifact'
Copy-Item -LiteralPath $nrm -Destination $mipsRoot
$releaseNrm = Require-File (Join-Path $mipsRoot 'dk64_lan_coop.nrm') 'Isolated MIPS NRM artifact'

$androidBuild = Join-Path $releaseRoot 'android'
$androidConfigureLog = Join-Path $logsRoot '03-android-arm64-configure.log'
$androidBuildLog = Join-Path $logsRoot '04-android-arm64-build.log'
Invoke-Logged 'Android ARM64 configure' $androidCmake @('-S', $coopRoot, '-B', $androidBuild, '-G', 'Ninja', "-DCMAKE_MAKE_PROGRAM=$androidNinja", "-DCMAKE_TOOLCHAIN_FILE=$androidToolchain", '-DANDROID_ABI=arm64-v8a', '-DANDROID_PLATFORM=android-29', '-DANDROID_STL=c++_static', '-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON', '-DCMAKE_BUILD_TYPE=Release') $androidConfigureLog
$androidCache = Require-File (Join-Path $androidBuild 'CMakeCache.txt') 'Fresh Android ARM64 API 29 cache'
$cacheText = Get-Content -LiteralPath $androidCache -Raw
foreach ($expected in @('ANDROID_ABI.*=arm64-v8a', 'ANDROID_PLATFORM.*=android-29', '27\.3\.13750724', 'CMAKE_MAKE_PROGRAM.*ninja')) {
    if ($cacheText -notmatch $expected) { throw "Fresh Android cache does not contain expected value: $expected" }
}
Invoke-Logged 'Android ARM64 build' $androidCmake @('--build', $androidBuild, '--parallel', '4') $androidBuildLog
$androidLibrary = Require-File (Join-Path $androidBuild 'dk64_coop_bridge.so') 'Android ARM64 bridge'

$windowsBuild = Join-Path $releaseRoot 'windows'
$wslWindowsBuild = To-WslPath $windowsBuild
$windowsLog = Join-Path $logsRoot '05-windows-x64.log'
$windowsCommand = "set -euo pipefail; cmake -S $(Quote-Bash $wslCoop) -B $(Quote-Bash $wslWindowsBuild) -G Ninja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_CXX_COMPILER=$(Quote-Bash "$wslMingw/bin/x86_64-w64-mingw32-clang++") -DCMAKE_RC_COMPILER=$(Quote-Bash "$wslMingw/bin/x86_64-w64-mingw32-windres") -DCMAKE_BUILD_TYPE=Release; cmake --build $(Quote-Bash $wslWindowsBuild) --parallel 4"
Invoke-Logged 'Windows x64 cross build' $wsl @('-d', 'Ubuntu', '--cd', $wslCoop, '--', 'bash', '-lc', $windowsCommand) $windowsLog
$windowsLibrary = Require-File (Join-Path $windowsBuild 'dk64_coop_bridge.dll') 'Windows x64 bridge'

function Read-U16([byte[]] $Bytes, [int] $Offset) { return [BitConverter]::ToUInt16($Bytes, $Offset) }
function Read-U32([byte[]] $Bytes, [int] $Offset) { return [BitConverter]::ToUInt32($Bytes, $Offset) }
function Read-U64([byte[]] $Bytes, [int] $Offset) { return [BitConverter]::ToUInt64($Bytes, $Offset) }
function Test-AndroidElf([string] $Path) {
    $bytes = [IO.File]::ReadAllBytes($Path)
    if ($bytes.Length -lt 64 -or $bytes[0] -ne 0x7F -or [char]$bytes[1] -ne 'E' -or [char]$bytes[2] -ne 'L' -or [char]$bytes[3] -ne 'F' -or $bytes[4] -ne 2 -or $bytes[5] -ne 1) { throw 'Android bridge is not a 64-bit little-endian ELF' }
    if ((Read-U16 $bytes 16) -ne 3 -or (Read-U16 $bytes 18) -ne 183) { throw 'Android bridge is not aarch64 ET_DYN' }
    $programOffset = [int](Read-U64 $bytes 32); $stride = Read-U16 $bytes 54; $count = Read-U16 $bytes 56
    for ($i = 0; $i -lt $count; ++$i) {
        $offset = $programOffset + $i * $stride
        if ((Read-U32 $bytes $offset) -ne 1) { continue }
        $fileOffset = Read-U64 $bytes ($offset + 8); $virtualAddress = Read-U64 $bytes ($offset + 16); $alignment = Read-U64 $bytes ($offset + 48)
        if ($alignment -lt 16384 -or ($fileOffset % 16384) -ne ($virtualAddress % 16384)) { throw 'Android PT_LOAD alignment is not 16 KiB compatible' }
    }
}
function Test-WindowsPe([string] $Path) {
    $bytes = [IO.File]::ReadAllBytes($Path)
    if ($bytes.Length -lt 64 -or $bytes[0] -ne [byte][char]'M' -or $bytes[1] -ne [byte][char]'Z') { throw 'Windows bridge is not an MZ executable' }
    $pe = [int](Read-U32 $bytes 60)
    if ($pe + 6 -gt $bytes.Length -or [Text.Encoding]::ASCII.GetString($bytes, $pe, 4) -ne "PE`0`0" -or (Read-U16 $bytes ($pe + 4)) -ne 0x8664) { throw 'Windows bridge is not PE x64' }
}

$checkLog = Join-Path $logsRoot '06-artifact-checks.log'
"# release artifact checks" | Set-Content -LiteralPath $checkLog -Encoding utf8
Test-AndroidElf $androidLibrary
Test-WindowsPe $windowsLibrary
$androidExportLog = Join-Path $logsRoot '06a-android-export.log'
Invoke-Logged 'Android ARM64 export check' $androidReadElf @('--dyn-syms', '--wide', $androidLibrary) $androidExportLog
if ((Get-Content -LiteralPath $androidExportLog -Raw) -notmatch ("\b" + [regex]::Escape($expectedTick) + "\b")) { throw "Android bridge does not export $expectedTick" }
$wslWindowsLibrary = To-WslPath $windowsLibrary
$windowsExportLog = Join-Path $logsRoot '06b-windows-export.log'
$windowsExportCommand = "set -euo pipefail; $(Quote-Bash "$wslMingw/bin/x86_64-w64-mingw32-nm") -g --defined-only $(Quote-Bash $wslWindowsLibrary) | grep -F $(Quote-Bash $expectedTick)"
Invoke-Logged 'Windows x64 export check' $wsl @('-d', 'Ubuntu', '--cd', $wslCoop, '--', 'bash', '-lc', $windowsExportCommand) $windowsExportLog
if ($protocolText -notmatch ("protocol_version\s*=\s*" + $protocolVersion) -or $manifestText -notmatch [regex]::Escape($expectedTick) -or $modSourceText -notmatch [regex]::Escape($expectedTick)) { throw "Protocol $protocolVersion/export contract check failed" }
Add-Type -AssemblyName System.IO.Compression.FileSystem
$nrmArchive = [IO.Compression.ZipFile]::OpenRead($releaseNrm)
try {
    $nrmEntry = $nrmArchive.GetEntry('mod.json')
    if ($null -eq $nrmEntry) { throw 'Built NRM does not contain mod.json' }
    $nrmReader = [IO.StreamReader]::new($nrmEntry.Open())
    try { $nrmManifest = $nrmReader.ReadToEnd() | ConvertFrom-Json } finally { $nrmReader.Dispose() }
} finally { $nrmArchive.Dispose() }
if ($nrmManifest.version -ne $version -or (@($nrmManifest.native_libraries.dk64_coop_bridge) -join ',') -ne "dk64_coop_start,dk64_coop_local_ipv4,$expectedTick,dk64_coop_stop") { throw 'Built NRM manifest version/native function contract check failed' }
Add-Content -LiteralPath $checkLog -Value "PASS protocol $protocolVersion/export, Android ELF aarch64/16KiB/export, Windows PE x64/export, built NRM mod.json version/native functions"
$commands.Add([pscustomobject]@{ name = 'Artifact contract checks'; command = 'PowerShell binary and manifest checks'; log = [IO.Path]::GetFileName($checkLog); exit_code = 0 })

function Find-WorkingPython {
    $candidates = [System.Collections.Generic.List[string]]::new()
    $pythonCommand = Get-Command python.exe -ErrorAction SilentlyContinue
    if ($null -ne $pythonCommand) { $candidates.Add($pythonCommand.Source) }
    $candidates.Add((Join-Path $workspace '.local/ap-integration-source/.venv/Scripts/python.exe'))
    $candidates.Add((Join-Path $env:USERPROFILE '.cache/codex-runtimes/codex-primary-runtime/dependencies/python/python.exe'))
    foreach ($candidate in $candidates) {
        if (-not (Test-Path -LiteralPath $candidate -PathType Leaf)) { continue }
        try {
            & $candidate --version *> $null
            if ($LASTEXITCODE -eq 0) { return (Resolve-Path -LiteralPath $candidate).Path }
        } catch { }
    }
    throw 'No runnable Python interpreter was found for coop/tools/package.py'
}
$python = Find-WorkingPython
$toolVersions.python = (& $python --version | Select-Object -First 1)
$packageLog = Join-Path $logsRoot '07-package.log'
Invoke-Logged 'Release packaging' $python @((Join-Path $coopRoot 'tools/package.py'), '--nrm', $releaseNrm, '--android-library', $androidLibrary, '--windows-library', $windowsLibrary, '--dist', (Join-Path $coopRoot 'dist')) $packageLog $coopRoot

$artifacts = @($releaseNrm, (Join-Path $mipsRoot 'mod.elf'), $androidLibrary, $windowsLibrary, (Join-Path $coopRoot "dist/DK64-LAN-Coop-$version-android-prototype.zip"), (Join-Path $coopRoot "dist/DK64-LAN-Coop-$version-windows-prototype.zip"), (Join-Path $coopRoot "dist/DK64-LAN-Coop-$version-source.zip"), (Join-Path $coopRoot 'dist/SHA256SUMS.txt'))
$hashes = [ordered]@{}
foreach ($artifact in $artifacts) { $hashes[[IO.Path]::GetFileName($artifact)] = (Get-FileHash -Algorithm SHA256 -LiteralPath (Require-File $artifact 'Release artifact')).Hash.ToLowerInvariant() }
$status = [ordered]@{
    version = $version
    protocol = $protocolVersion
    expected_export = $expectedTick
    generated_utc = [DateTime]::UtcNow.ToString('o')
    workspace = $workspace
    toolchains = $toolVersions
    validation = [ordered]@{ suite = 'Linux Debug ASan/UBSan native CTest'; result = "$expectedHostTests/$expectedHostTests passed"; log = '01-host-tests.log' }
    commands = $commands
    artifacts_sha256 = $hashes
    not_performed = @('gameplay testing', 'installation', 'game launch', 'commit', 'push', 'publish')
}
$statusPath = Join-Path $releaseRoot 'build-status.json'
$status | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $statusPath -Encoding utf8
Write-Host "PASS: v$version artifacts and status written to $releaseRoot"
