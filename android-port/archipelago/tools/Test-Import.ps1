param([string]$JavaHome = $env:JAVA_HOME, [string]$JsonJar)
$ErrorActionPreference = 'Stop'
$projectPath = Split-Path $PSScriptRoot -Parent
$portPath = Split-Path $projectPath -Parent
if (-not $JavaHome) { throw 'Supply -JavaHome pointing to JDK 17 or newer.' }
if (-not $JsonJar) { $JsonJar = Join-Path $portPath '.local/mod-tests/json.jar' }
if (-not (Test-Path -LiteralPath $JsonJar)) { throw 'Supply -JsonJar pointing to an org.json JVM jar (test-only dependency).' }
$classes = Join-Path $projectPath 'build/importer-classes'
$scratch = Join-Path $projectPath 'build/importer-scratch'
New-Item -ItemType Directory -Force $classes, $scratch | Out-Null
$sources = @('DriverArchive', 'ModSession', 'NativeMod', 'ModTransaction', 'ModArchive', 'ModStore') | ForEach-Object { "$portPath/android/app/src/main/java/io/github/dk64port/$_.java" }
& "$JavaHome/bin/javac.exe" -cp $JsonJar -d $classes @sources "$projectPath/tests/BundleImportSmoke.java"
if ($LASTEXITCODE) { throw 'Importer test compilation failed.' }
& "$JavaHome/bin/java.exe" -cp "$classes;$JsonJar" io.github.dk64port.BundleImportSmoke "$projectPath/dist/DK64-Archipelago-0.2.0-android-direct-test.zip" $scratch
if ($LASTEXITCODE) { throw 'Actual bundle import test failed.' }
