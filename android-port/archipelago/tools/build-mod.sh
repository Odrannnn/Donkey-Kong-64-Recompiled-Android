#!/usr/bin/env bash
set -euo pipefail
archipelago=$(cd "$(dirname "$0")/.." && pwd)
port=$(cd "$archipelago/.." && pwd)
export LD_LIBRARY_PATH="$port/.local/llvm-wsl/usr/lib/x86_64-linux-gnu:$port/.local/llvm-wsl/usr/lib/llvm-21/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export PATH="$port/.local/llvm-wsl/usr/lib/llvm-21/bin:$PATH"
cd "$archipelago"
mkdir -p build/mod dist
python3 tools/lift-gameplay.py
for unit in ap gameplay campaign_save campaign_inventory campaign_inventory_stage campaign_items campaign_traps campaign_transaction campaign_checks campaign_stage shops ap_shops shop_text shop_overlay; do
clang -target mips -mips2 -mabi=32 -O2 -G0 -mno-abicalls -mno-odd-spreg -mno-check-zero-division \
    -fomit-frame-pointer -fno-builtin -Wall -Wextra -Wno-incompatible-library-redeclaration \
    -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-variable -Wno-missing-braces \
    -Wno-unsupported-floating-point-opt -Werror=section -Wno-macro-redefined \
    -nostdinc -D_LANGUAGE_C -DMIPS -DF3DEX_GBI_2 -DF3DEX_GBI_2x \
    -I vendor/recomp -I "$port/upstream/patches" -I "$port/upstream/patches/dummy_headers" \
    -I "$port/upstream/lib/dk64_decomp/include" -I "$port/upstream/lib/dk64_decomp/include/2.0L" \
    -I "$port/upstream/lib/dk64_decomp/include/2.0L/PR" -I "$port/upstream/lib/rt64/include" \
    -c "mod/$unit.c" -o "build/mod/$unit.o"
done
clang -target mips -mips2 -mabi=32 -G0 -mno-abicalls -c build/mod/gameplay.S -o build/mod/gameplay-lift.o
ld.lld build/mod/ap.o build/mod/gameplay.o build/mod/campaign_save.o build/mod/campaign_inventory.o build/mod/campaign_inventory_stage.o build/mod/campaign_items.o build/mod/campaign_traps.o build/mod/campaign_transaction.o build/mod/campaign_checks.o build/mod/campaign_stage.o build/mod/shops.o build/mod/ap_shops.o build/mod/shop_text.o build/mod/shop_overlay.o build/mod/gameplay-lift.o -nostdlib -T vendor/recomp/mod.ld -Map build/mod/mod.map \
    --unresolved-symbols=ignore-all --emit-relocs -e 0 --no-nmagic -o build/mod/mod.elf
"$port/.local/host-tools-wsl/RecompModTool" mod.toml dist
