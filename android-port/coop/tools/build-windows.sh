#!/usr/bin/env bash
set -euo pipefail
coop=$(cd "$(dirname "$0")/.." && pwd)
port=$(cd "$coop/.." && pwd)
mingw="$port/.local/coop-toolchains/llvm-mingw-20260826-ucrt-ubuntu-22.04-x86_64"
cmake -S "$coop" -B "$coop/build/windows" -G Ninja -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_CXX_COMPILER="$mingw/bin/x86_64-w64-mingw32-clang++" \
    -DCMAKE_RC_COMPILER="$mingw/bin/x86_64-w64-mingw32-windres" -DCMAKE_BUILD_TYPE=Release
cmake --build "$coop/build/windows" -j4
