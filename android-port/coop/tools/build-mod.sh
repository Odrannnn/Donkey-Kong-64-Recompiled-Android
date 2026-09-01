#!/usr/bin/env bash
set -euo pipefail
coop=$(cd "$(dirname "$0")/.." && pwd)
port=$(cd "$coop/.." && pwd)
export LD_LIBRARY_PATH="$port/.local/llvm-wsl/usr/lib/x86_64-linux-gnu:$port/.local/llvm-wsl/usr/lib/llvm-21/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export PATH="$port/.local/llvm-wsl/usr/lib/llvm-21/bin:$PATH"
cd "$coop"
mkdir -p build/mod dist
clang -target mips -mips2 -mabi=32 -O2 -G0 -mno-abicalls -mno-odd-spreg -mno-check-zero-division \
    -fomit-frame-pointer -fno-builtin -Wall -Wextra -Wno-incompatible-library-redeclaration \
    -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-variable -Wno-missing-braces \
    -Wno-unsupported-floating-point-opt -Werror=section -Wno-macro-redefined \
    -nostdinc -D_LANGUAGE_C -DMIPS -DF3DEX_GBI_2 -DF3DEX_GBI_2x \
    -I vendor/recomp -I "$port/upstream/patches" -I "$port/upstream/patches/dummy_headers" \
    -I "$port/upstream/lib/dk64_decomp/include" -I "$port/upstream/lib/dk64_decomp/include/2.0L" \
    -I "$port/upstream/lib/dk64_decomp/include/2.0L/PR" -I "$port/upstream/lib/rt64/include" \
    -c mod/coop.c -o build/mod/coop.o
ld.lld build/mod/coop.o -nostdlib -T vendor/recomp/mod.ld -Map build/mod/mod.map \
    --unresolved-symbols=ignore-all --emit-relocs -e 0 --no-nmagic -o build/mod/mod.elf
"$port/.local/host-tools-wsl/RecompModTool" mod.toml dist
