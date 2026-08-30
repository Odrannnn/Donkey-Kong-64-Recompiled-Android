param([string]$JavaHome = $env:JAVA_HOME, [string]$JsonJar)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
if (-not $JsonJar) {
    $JsonJar = Get-ChildItem -LiteralPath "$env:USERPROFILE/.gradle/caches/modules-2/files-2.1/org.json/json/20240303" -Filter '*.jar' -Recurse | Select-Object -First 1 -ExpandProperty FullName
}
if (-not (Test-Path -LiteralPath $JsonJar)) { throw 'Supply -JsonJar pointing to an org.json JVM jar (test dependency only).' }
$classes = Join-Path $root '.local/mod-tests/classes'
$scratch = Join-Path $root '.local/mod-tests/scratch'
New-Item -ItemType Directory -Force $classes, $scratch | Out-Null
$localJson = Join-Path $root '.local/mod-tests/json.jar'
Copy-Item -LiteralPath $JsonJar -Destination $localJson -Force
$JsonJar = $localJson
$sources = @('DriverArchive', 'ModSession', 'NativeMod', 'ModTransaction', 'ModArchive', 'ModStore') | ForEach-Object { "$root/android/app/src/main/java/io/github/dk64port/$_.java" }
& "$JavaHome/bin/javac.exe" -cp $JsonJar -d $classes @sources "$root/tools/tests/ModImportTest.java" "$root/tools/tests/NativeModImportTest.java"
if ($LASTEXITCODE -ne 0) { throw 'Mod import test compilation failed.' }
& "$JavaHome/bin/java.exe" -cp "$classes;$JsonJar" io.github.dk64port.ModImportTest $scratch
if ($LASTEXITCODE -ne 0) { throw 'Mod import tests failed.' }
& "$JavaHome/bin/java.exe" -cp "$classes;$JsonJar" io.github.dk64port.NativeModImportTest $scratch
if ($LASTEXITCODE -ne 0) { throw 'Native mod bundle tests failed.' }
