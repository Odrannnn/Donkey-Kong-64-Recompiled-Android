#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
cmake -S "$root/tools/tests/audio" -B "$root/.local/audio-tests" -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build "$root/.local/audio-tests" --parallel 8
ctest --test-dir "$root/.local/audio-tests" --output-on-failure
