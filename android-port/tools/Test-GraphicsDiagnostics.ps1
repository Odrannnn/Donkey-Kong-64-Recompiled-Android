param(
    [string]$JavaHome = $env:JAVA_HOME,
    [string]$SdkPath = "$env:LOCALAPPDATA/Android/Sdk"
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$classes = Join-Path $root '.local/graphics-tests/classes'
$androidJar = Join-Path $SdkPath 'platforms/android-35/android.jar'
if (-not (Test-Path -LiteralPath $androidJar)) { throw 'Android platform 35 android.jar is required.' }
New-Item -ItemType Directory -Force $classes | Out-Null
& "$JavaHome/bin/javac.exe" -cp $androidJar -d $classes "$root/android/app/src/main/java/io/github/dk64port/GraphicsDiagnostics.java" "$root/tools/tests/GraphicsDiagnosticsTest.java"
if ($LASTEXITCODE -ne 0) { throw 'Graphics diagnostics test compilation failed.' }
& "$JavaHome/bin/java.exe" -cp "$classes$([IO.Path]::PathSeparator)$androidJar" io.github.dk64port.GraphicsDiagnosticsTest
if ($LASTEXITCODE -ne 0) { throw 'Graphics diagnostics tests failed.' }
