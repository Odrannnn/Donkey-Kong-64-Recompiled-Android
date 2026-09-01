"""Prepare executable tests against the built NRM, not a parallel move model."""
from pathlib import Path
import json
import re
import struct
import tomllib
import zipfile

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "build/mod"


def main():
    with zipfile.ZipFile(ROOT / "dist/dk64_archipelago.nrm") as archive:
        symbols = archive.read("mod_syms.bin")
        binary = archive.read("mod_binary.bin")
    if symbols[:8] != b"N64RSYMS" or struct.unpack_from("<I", symbols, 8)[0] != 1:
        raise ValueError("Unsupported mod symbol format")
    counts = struct.unpack_from("<10I", symbols, 12)
    offset = 52 + counts[9]
    functions = {}; sections = []; data_relocs = []
    for index in range(counts[0]):
        flags, file_offset, vram, size, bss, funcs, relocs = struct.unpack_from("<7I", symbols, offset)
        offset += 28
        sections.append((file_offset, vram, size, bss))
        for i in range(funcs):
            address, length = struct.unpack_from("<2I", symbols, offset); offset += 8
            functions[vram + address] = f"mod_func_{len(functions)}"
        for i in range(relocs):
            at, kind, target_offset, target_section = struct.unpack_from("<4I", symbols, offset); offset += 16
            if kind == 2 and target_section & 0x80000000 and target_section < 0xfffffffd:
                data_relocs.append((index, at, target_section & 0x7fffffff, target_offset))
    map_symbols = {}
    for line in (OUT / "mod.map").read_text().splitlines():
        match = re.fullmatch(r"\s*([0-9a-f]+)\s+[0-9a-f]+\s+[0-9a-f]+\s+\d+\s+(\w+)", line)
        if match: map_symbols[match[2]] = int(match[1], 16)
    header = ["/* Generated test bindings from the actual linked mod. */"]
    manifest = json.loads((ROOT / "mod/gameplay-patches.json").read_text())
    for entry in manifest["functions"]:
        for name in (entry["function"], "ap_stock_" + entry["function"]):
            header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    header.append(f"#define test_ap_prepare_move_shops {functions[map_symbols['ap_prepare_move_shops']]}")
    for name in ("ap_prepare_ap_shops", "ap_apshop_snapshot", "ap_apshop_restore", "ap_apshop_set_labels", "ap_apshop_display", "ap_apshop_generation", "ap_shop_overlay_draw"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    for name in ("ap_campaign_trailer_read", "ap_campaign_trailer_stage"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    for name in ("ap_campaign_inventory_clear", "ap_campaign_inventory_add", "ap_campaign_inventory_rebuild", "ap_campaign_inventory_has_flag"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    for name in ("ap_campaign_inventory_stage", "ap_campaign_inventory_snapshot", "ap_campaign_inventory_reset"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    for name in ("ap_campaign_items_stage", "ap_campaign_item_count", "ap_campaign_item_has_flag",
                 "ap_campaign_original_item_count", "ap_campaign_get_kong_ownership_from_flag",
                 "ap_campaign_has_flag_move", "ap_campaign_items_reset"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    for name in ("ap_campaign_transaction_build", "ap_campaign_transaction_stage",
                 "ap_campaign_transaction_snapshot", "ap_campaign_transaction_commit",
                 "ap_campaign_transaction_reset"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    for name in ("ap_campaign_trap_executable", "ap_campaign_trap_can_start", "ap_campaign_trap_start",
                 "ap_campaign_trap_tick", "ap_campaign_trap_button_mask", "ap_campaign_trap_reset"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    for name in ("ap_campaign_checks_prepare", "ap_campaign_checks_validate", "ap_campaign_checks_restore", "ap_campaign_checks_event",
                 "ap_campaign_checks_stock_flag", "ap_campaign_checks_snapshot", "ap_campaign_checks_reset"):
        header.append(f"#define test_{name} {functions[map_symbols[name]]}")
    header.append(f"#define test_ap_campaign_stage_apply {functions[map_symbols['ap_campaign_stage_apply']]}")
    for name in ("ap_cranky_moves", "ap_funky_moves", "ap_candy_moves"):
        header.append(f"#define TEST_{name.upper()} (0x{map_symbols[name]:08X}u + test_mod_delta)")
    header.append(f"#define TEST_OVERLAY_DRAW (0x{map_symbols['ap_shop_overlay_draw']:08X}u + test_mod_delta)")
    header += ["static uint32_t test_mod_delta;", f"#define TEST_MODE_ADDRESS (0x{map_symbols['ap_gameplay_mode']:08X}u + test_mod_delta)"]
    header.append("#define TEST_CAMPAIGN_TRAILER 0x807ED688u")
    # Independent off-mode oracle: the port's existing native recompilation of
    # the original ROM, not another invocation of our relocation algorithm.
    helper_names = ["func_menu_80025D14", "func_menu_80025D44", "func_menu_80025D8C", "func_menu_80026250",
        "func_global_asm_8060C648", "func_global_asm_8060C6B8", "func_global_asm_8060C724",
        "func_global_asm_8060C340", "func_global_asm_8060C430", "func_global_asm_8060C2C4",
        "func_global_asm_8060BCA0", "func_global_asm_8060BEA8", "func_global_asm_8060BF84",
        "func_global_asm_8060BB18", "func_global_asm_8060BBE0", "getEEPROMSaveSlot"]
    baseline_names = {entry["function"] for entry in manifest["functions"]} | set(helper_names)
    flow_names = {"func_menu_80026804", "func_menu_80026684", "func_menu_80027028",
        "func_global_asm_8070E548", "func_global_asm_8070E2AC", "func_global_asm_8070D970", "func_global_asm_8070D928",
        "guScaleF", "guTranslateF", "guMtxCatF", "guMtxF2L", "guMtxIdentF"}
    missing = set(baseline_names) | flow_names
    baseline = []; flow = []
    for path in sorted((ROOT.parent / "upstream/RecompiledFuncs").glob("*.c")):
        source = path.read_text()
        for name in list(missing):
            found = re.search(r"RECOMP_FUNC void " + re.escape(name) + r"\([^\n]+\) \{.*?\n;?\}", source, re.S)
            if found:
                if name in baseline_names:
                    baseline.append(found[0].replace("void " + name + "(", "void baseline_" + name + "(", 1))
                if name in flow_names: flow.append(found[0])
                missing.remove(name)
        if not missing: break
    if missing: raise ValueError(f"Missing original recompilation: {missing}")
    baseline_text = "\n\n".join(baseline)
    prototypes = []
    for name in sorted(baseline_names):
        baseline_text = re.sub(r"\b" + re.escape(name) + r"\(", "baseline_" + name + "(", baseline_text)
        prototypes.append(f"RECOMP_FUNC void baseline_{name}(uint8_t*, recomp_context*);")
    (OUT / "gameplay-baseline.c").write_text("\n".join(prototypes) + "\n" + baseline_text + "\n")
    # Original caller/rendering code linked to actual mod replacements, for
    # integration tests. Keep this separate from the untouched baseline oracle.
    flow_text = "\n\n".join(flow)
    for name in flow_names:
        flow_text = re.sub(r"\b" + re.escape(name) + r"\(", "flow_" + name + "(", flow_text)
    for entry in manifest["functions"]:
        name = entry["function"]
        flow_text = re.sub(r"\b" + re.escape(name) + r"\(", "test_" + name + "(", flow_text)
    (OUT / "gameplay-flow.c").write_text("\n".join(f"RECOMP_FUNC void flow_{name}(uint8_t*, recomp_context*);" for name in sorted(flow_names)) + "\n" + flow_text + "\n")
    header.append("static void load_mod(uint8_t* rdram) {")
    for index, (file_offset, vram, size, bss) in enumerate(sections):
        header.append(f"section_addresses[{index}] = (int32_t)(0x{vram:08X}u + test_mod_delta);")
        header.append(f"load_file(rdram, \"mod_binary.bin\", {file_offset}u, 0x{vram:08X}u + test_mod_delta, {size}u);")
    for index, at, target_index, target_offset in data_relocs:
        # R_MIPS_32 has an implicit addend in the linked word. Runtime mods.cpp
        # adjusts that word by the target section's load delta, not by replacing
        # it with the relocation's symbol base (which loses table entry offsets).
        linked = struct.unpack_from(">I", binary, sections[index][0] + at)[0]
        header.append(f"put32(0x{sections[index][1] + at:08X}u + test_mod_delta, 0x{linked:08X}u + test_mod_delta);")
    header.append("}")
    # Load the original static section so constants, pointers and arrays keep their actual values.
    sections = tomllib.loads((ROOT.parent / "upstream/DK64Syms/dump.toml").read_text())["section"]
    static = next(s for s in sections if s["vram"] <= 0x8063EA14 < s["vram"] + s["size"])
    menu = next(s for s in sections if s["name"] == ".menu")
    header += [f'#define TEST_ROM "{(ROOT.parent / "upstream/donkeykong64.decompressed.us.z64").as_posix()}"',
        f'#define TEST_STATIC_ROM {static["rom"]}u', f'#define TEST_STATIC_VRAM 0x{static["vram"]:08X}u', f'#define TEST_STATIC_SIZE {static["size"]}u',
        f'#define TEST_MENU_ROM {menu["rom"]}u', f'#define TEST_MENU_VRAM 0x{menu["vram"]:08X}u', f'#define TEST_MENU_SIZE {menu["size"]}u']
    (OUT / "gameplay-symbols.h").write_text("\n".join(header) + "\n")
    (OUT / "mod_binary.bin").write_bytes(binary)
    (OUT / "mod_syms.bin").write_bytes(symbols)
    print(f"Prepared bindings for {len(manifest['functions'])} real game replacements")


if __name__ == "__main__": main()
