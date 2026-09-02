#!/usr/bin/env bash
set -euo pipefail
coop=$(cd "$(dirname "$0")/.." && pwd)
port=${DK64_PORT_ROOT:-$(cd "$coop/.." && pwd)}
test -d "$port/upstream"
test -x "$port/.local/host-tools-wsl/RecompModTool"
test -x "$port/.local/llvm-wsl/usr/lib/llvm-21/bin/clang"
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
# RecompModTool resolves symbol references from its working directory. Keep the
# checked-in manifest relocatable, and materialize only a build-local manifest
# with the pipeline's already validated prepared-workspace path.
resolved_manifest=build/mod/resolved-mod.toml
sed -e "s#elf_path = \"build/mod/mod.elf\"#elf_path = \"$coop/build/mod/mod.elf\"#" \
    -e "s#../upstream/DK64Syms/#$port/upstream/DK64Syms/#g" mod.toml > "$resolved_manifest"
"$port/.local/host-tools-wsl/RecompModTool" "$resolved_manifest" dist
