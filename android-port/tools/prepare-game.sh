#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
cd "$root"

# WSL host toolchain; no sudo or system package changes required.
mkdir -p .local/llvm-packages .local/llvm-wsl
if [[ ! -x .local/llvm-wsl/usr/lib/llvm-21/bin/clang ]]; then
    (
        cd .local/llvm-packages
        apt-get download clang-21 lld-21 libllvm21 libclang-cpp21 libclang1-21 libclang-common-21-dev llvm-21-linker-tools
        for package in *.deb; do dpkg-deb -x "$package" ../llvm-wsl; done
    )
fi
export LD_LIBRARY_PATH="$root/.local/llvm-wsl/usr/lib/x86_64-linux-gnu:$root/.local/llvm-wsl/usr/lib/llvm-21/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export PATH="$root/.local/llvm-wsl/usr/lib/llvm-21/bin:$PATH"

cmake -S upstream/lib/N64ModernRuntime/N64Recomp -B .local/host-tools-wsl -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build .local/host-tools-wsl --target N64RecompCLI RSPRecomp --parallel 8
g++ -std=c++17 -O2 upstream/lib/rt64/src/tools/file_to_c/file_to_c.cpp -o .local/host-tools-wsl/file_to_c

cd upstream
../.local/host-tools-wsl/N64Recomp us.toml
../.local/host-tools-wsl/RSPRecomp n_aspMain.toml
# PATH lookup avoids whitespace in make's CC/LD variable expansion.
make -C patches CC=clang LD=ld.lld -j8
../.local/host-tools-wsl/N64Recomp patches.toml
../.local/host-tools-wsl/file_to_c patches/patches.bin bk_patches_bin \
    RecompiledPatches/patches_bin.c RecompiledPatches/patches_bin.h
printf 'Generated game, RSP, and patch sources are ready.\n'
