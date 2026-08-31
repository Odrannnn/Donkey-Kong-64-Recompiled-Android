param([string]$JavaHome = $env:JAVA_HOME)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$classes = Join-Path $root '.local/touch-tests/classes'
New-Item -ItemType Directory -Force $classes | Out-Null
& "$JavaHome/bin/javac.exe" -d $classes "$root/android/app/src/main/java/io/github/dk64port/DpadInput.java" "$root/android/app/src/main/java/io/github/dk64port/GameLifecycle.java" "$root/tools/tests/TouchLifecycleTest.java"
if ($LASTEXITCODE) { throw 'Touch/lifecycle compilation failed.' }
& "$JavaHome/bin/java.exe" -cp $classes io.github.dk64port.TouchLifecycleTest
if ($LASTEXITCODE) { throw 'Touch/lifecycle checks failed.' }
