param([string]$JavaHome = $env:JAVA_HOME)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$classes = Join-Path $root '.local/driver-tests/classes'
$scratch = Join-Path $root '.local/driver-tests/scratch'
New-Item -ItemType Directory -Force $classes, $scratch | Out-Null
& "$JavaHome/bin/javac.exe" -d $classes "$root/android/app/src/main/java/io/github/dk64port/DriverArchive.java" "$root/tools/tests/DriverArchiveTest.java"
if ($LASTEXITCODE -ne 0) { throw 'Driver archive test compilation failed.' }
& "$JavaHome/bin/java.exe" -cp $classes io.github.dk64port.DriverArchiveTest $scratch
if ($LASTEXITCODE -ne 0) { throw 'Driver archive tests failed.' }
