#!/usr/bin/env bash
set -euo pipefail
archipelago=$(cd "$(dirname "$0")/.." && pwd)
port=$(cd "$archipelago/.." && pwd)
cd "$archipelago"
python3 tools/prepare-gameplay-tests.py
"$port/.local/host-tools-wsl/OfflineModRecomp" build/mod/mod_syms.bin build/mod/mod_binary.bin "$port/upstream/DK64Syms/dump.toml" build/mod/gameplay-recompiled.c
# The offline generator expresses MIPS wrapping shifts in signed C arithmetic.
# GCC's wrapping semantics and aliasing mode match that memory/register model.
cc -std=c11 -O1 -g -fwrapv -fno-strict-aliasing -fsanitize=address,undefined -fno-omit-frame-pointer \
    -I tests -I build/mod -I "$port/upstream/lib/N64ModernRuntime/N64Recomp/include" \
    tests/gameplay_tests.c -lm -o build/mod/gameplay_tests
cd build/mod
UBSAN_OPTIONS=halt_on_error=1 ASAN_OPTIONS=halt_on_error=1 ./gameplay_tests
