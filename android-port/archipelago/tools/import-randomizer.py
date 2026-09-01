"""Compile the pinned upstream catalogs and inventory patch sites without executing upstream code."""
from pathlib import Path
import argparse
import ast
import hashlib
import json
import re

ROOT = Path(__file__).resolve().parents[1]
REVISION = "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4"
SOURCES = ("archipelago/client/ap_check_ids.py", "archipelago/client/check_flag_locations.py",
           "archipelago/client/items.py", "randomizer/Lists/Location.py")

def value(node, constants):
    if isinstance(node, ast.Constant): return node.value
    if isinstance(node, ast.Name) and node.id in constants: return constants[node.id]
    if isinstance(node, ast.Dict):
        result = {}
        for key, entry in zip(node.keys, node.values):
            k, v = value(key, constants), value(entry, constants)
            if k in result and result[k] != v: raise ValueError(f"Conflicting duplicate key {k}")
            result[k] = v
        return result
    if isinstance(node, (ast.List, ast.Tuple)): return [value(x, constants) for x in node.elts]
    if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Add, ast.Sub)):
        left, right = value(node.left, constants), value(node.right, constants)
        if type(left) is not int or type(right) is not int: raise ValueError("Non-integer expression")
        return left + right if isinstance(node.op, ast.Add) else left - right
    if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.USub): return -value(node.operand, constants)
    raise ValueError(f"Unsupported catalog expression: {ast.dump(node)}")

def assignment(path, name, constants=None):
    for node in ast.parse(path.read_text(encoding="utf-8-sig")).body:
        target = node.target if isinstance(node, ast.AnnAssign) else node.targets[0] if isinstance(node, ast.Assign) else None
        if isinstance(target, ast.Name) and target.id == name: return value(node.value, constants or {})
    raise ValueError(f"Missing {name} in {path}")

def shop_flag(name):
    parts = name.split(" ")
    if len(parts) != 3: return None
    level, shop, kong = parts
    levels = ["Japes", "Aztec", "Factory", "Galleon", "Forest", "Caves", "Castle", "Isles"]
    kongs = ["Donkey", "Diddy", "Lanky", "Tiny", "Chunky"]
    if level not in levels or shop not in ("Cranky", "Funky", "Candy") or kong not in kongs + ["Shared"]: return None
    level = levels.index(level); kong = 0 if kong == "Shared" else kongs.index(kong)
    if shop == "Cranky": index = level
    elif shop == "Funky" and level < 7: index = level + 8
    elif shop == "Candy" and 1 <= level <= 3: index = level - 1 + 15
    elif shop == "Candy" and 5 <= level <= 6: index = level - 5 + 18
    else: return None
    return 800 + index * 5 + kong

ABILITY_BITS = {"diving": 128, "oranges": 64, "barrels": 32, "vines": 16, "camera": 8, "shockwave": 4}
SPECIAL_BITS = {"nintendo_coin": 1, "rareware_coin": 2, "bean": 4}
SCALAR_EFFECTS = {"fairies": "FAIRY", "crowns": "CROWN", "medals": "MEDAL", "pearls": "PEARL",
                  "rainbow_coins": "RAINBOW", "junk_items": "JUNK"}

def ice_trap_safety(source):
    """Extract the original map/control-state safety policy in declaration order."""
    text = (source / "base-hack/src/item rando/ice_trap.c").read_text(encoding="utf-8")
    maps_body = re.search(r"banned_trap_maps\[\]\s*=\s*\{(.*?)\};", text, re.S)
    movement_body = re.search(r"banned_trap_movement\s*=\s*\{(.*?)\};", text, re.S)
    if not maps_body or not movement_body: raise ValueError("Missing ice-trap safety tables")
    names = re.findall(r"ICETRAPREQ_(BANNED|SUPER|ALLOW)", maps_body.group(1))
    map_values = [{"BANNED": 0, "SUPER": 1, "ALLOW": 2}[name] for name in names]
    movement_values = [int(value) for value in re.findall(r"\.\w+\s*=\s*([01])", movement_body.group(1))]
    if len(map_values) != 221 or len(movement_values) != 137:
        raise ValueError(f"Unexpected ice-trap safety table sizes: {len(map_values)}/{len(movement_values)}")
    return map_values, movement_values

def item_effects(ap_id, data, trap_types):
    """Normalize the upstream client metadata into a bounded game-side ABI."""
    if ap_id == 14041089: return [("NONE", 0, 0)]
    if data.get("flag_id") is not None: return [("FLAG", 0, data["flag_id"])]
    source = data.get("count_id")
    if source is None: return [("UNSUPPORTED", 0, 0)]
    entries = source if isinstance(source, list) else [source]
    if not 1 <= len(entries) <= 2: raise ValueError(f"Unsupported effect count for item {ap_id}")
    result = []
    for effect in entries:
        field = effect.get("field")
        if field == "kong_bitfield": op = ("KONG", effect["bit"], 0)
        elif field == "key_bitfield": op = ("KEY", effect["bit"], 0)
        elif field == "flag_moves": op = ("ABILITY", ABILITY_BITS[effect["bit"]], 0)
        elif field == "special_items": op = ("SPECIAL", SPECIAL_BITS[effect["bit"]], 0)
        elif field == "hint_bitfield": op = ("HINT", effect["kong"], effect["level"])
        elif field == "bp_count": op = ("BLUEPRINT", effect["kong"], 0)
        elif field in SCALAR_EFFECTS: op = (SCALAR_EFFECTS[field], 0, 0)
        elif field == "ice_traps": op = ("TRAP", trap_types.index(effect["ice_trap_type"]), 0)
        elif "item" in effect: op = ("TRANSFER", effect["item"], 0)
        else: raise ValueError(f"Unsupported effect for item {ap_id}: {effect}")
        kind, arg0, arg1 = op
        if not 0 <= arg0 <= 255 or not 0 <= arg1 <= 65535: raise ValueError(f"Effect out of range for item {ap_id}")
        result.append(op)
    return result

def generate(source, output=None):
    # Reuse the established source hashes, rather than trusting a branch name.
    lock = json.loads((ROOT / "upstream.json").read_text())
    gameplay = json.loads((ROOT / "mod/gameplay-patches.json").read_text())
    for name, digest in {**lock["files"], **lock.get("campaign_files", {}), **gameplay["sources"]}.items():
        # Git for Windows may check text out with CRLF; compare canonical Git line endings.
        if hashlib.sha256((source / name).read_bytes().replace(b"\r\n", b"\n")).hexdigest() != digest: raise ValueError(f"Source mismatch: {name}")
    drops = assignment(source / SOURCES[3], "DROPSANITY_FLAG_START")
    ids = assignment(source / SOURCES[0], "check_names_to_id")
    flags = assignment(source / SOURCES[1], "location_flag_to_name", {"DROPSANITY_FLAG_START": drops})
    items = assignment(source / SOURCES[2], "item_ids")
    trap_types = sorted({entry["ice_trap_type"] for data in items.values()
        for entry in (data.get("count_id") if isinstance(data.get("count_id"), list) else [data.get("count_id")])
        if isinstance(entry, dict) and entry.get("field") == "ice_traps"})
    trap_maps, trap_movement = ice_trap_safety(source)
    by_name = {name: flag for flag, name in flags.items()}
    checks = []
    for name, ap_id in ids.items():
        flag = by_name.get(name)
        origin = "flag"
        if flag is None: flag, origin = shop_flag(name), "shop"
        if flag is None: flag, origin = items.get(ap_id, {}).get("flag_id"), "item_fallback"
        if flag is None and name == "Banana Hoard": flag, origin = 0x1B0, "goal"
        if flag is None and re.fullmatch(r"Helm (Donkey|Diddy|Lanky|Tiny|Chunky) Barrel [12]", name):
            kong = ["Donkey", "Diddy", "Lanky", "Tiny", "Chunky"].index(name.split()[1])
            flag, origin = 0x3CC + 2 * kong + int(name[-1]) - 1, "helm_barrel"
        if flag is None: origin = "unresolved"
        # Keep added/reassigned flags out of the stock save. Their hooks must emit events explicitly.
        kind = "event"
        # 0x26B onward includes newly assigned picture/ability/patch/grabbable flags.
        if origin == "flag" and flag < 0x26B: kind = "stock_flag"
        if origin == "shop": kind = "shop"
        if flag is not None and flag >= drops: kind = "enemy"
        checks.append({"id": ap_id, "name": name, "flag": flag, "source": origin, "detector": kind})
    checks.sort(key=lambda x: x["id"])
    if len({x["id"] for x in checks}) != len(checks): raise ValueError("Duplicate location ID")
    aliases = {}
    for check in checks:
        if check["flag"] is not None: aliases.setdefault(check["flag"], []).append(check["id"])
    catalog = {"format": 1, "revision": REVISION, "world_version": "1.5.8", "checks": checks,
        "items": [{"id": i, **data} for i, data in sorted(items.items())],
        "flag_aliases": {str(flag): names for flag, names in aliases.items() if len(names) > 1}}
    out = output or ROOT / "generated"; out.mkdir(exist_ok=True)
    (out / "catalog.json").write_text(json.dumps(catalog, indent=2) + "\n", encoding="utf-8")
    digest = hashlib.sha256(json.dumps(catalog, sort_keys=True, separators=(",", ":"), ensure_ascii=True).encode()).hexdigest()
    header = ["/* Generated from pinned DK64 Randomizer; MIT notice: licenses/DK64-Randomizer.txt. */", "#pragma once",
        f'#define AP_CATALOG_SHA256 "{digest}"',
        f"#define AP_LOCATION_COUNT {len(checks)}", f"#define AP_LOCATION_WORDS {(len(checks)+31)//32}",
        "typedef struct { unsigned int id; unsigned short flag; unsigned char detector; } ap_location_def;",
        "enum { AP_DETECT_STOCK_FLAG, AP_DETECT_EVENT, AP_DETECT_SHOP, AP_DETECT_ENEMY };",
        "static const ap_location_def ap_locations[AP_LOCATION_COUNT] = {"]
    for check in checks:
        flag = 65535 if check["flag"] is None else check["flag"]
        header.append(f'    {{{check["id"]}u, {flag}, AP_DETECT_{check["detector"].upper()}}}, /* {check["name"]} */')
    header += ["};", f"#define AP_ITEM_COUNT {len(items)}", "static const unsigned int ap_item_ids[AP_ITEM_COUNT] = {"]
    header += [f'    {i}u, /* {data["name"]} */' for i, data in sorted(items.items())]
    header += ["};", "enum { AP_EFFECT_UNSUPPORTED, AP_EFFECT_NONE, AP_EFFECT_FLAG, AP_EFFECT_KONG, AP_EFFECT_KEY,",
        "    AP_EFFECT_ABILITY, AP_EFFECT_SPECIAL, AP_EFFECT_HINT, AP_EFFECT_BLUEPRINT, AP_EFFECT_FAIRY,",
        "    AP_EFFECT_CROWN, AP_EFFECT_MEDAL, AP_EFFECT_PEARL, AP_EFFECT_RAINBOW, AP_EFFECT_JUNK,",
        "    AP_EFFECT_TRAP, AP_EFFECT_TRANSFER };",
        "typedef struct { unsigned char kind, arg0; unsigned short arg1; } ap_item_effect;",
        "typedef struct { unsigned int id; ap_item_effect effects[2]; } ap_item_effect_def;",
        f"#define AP_TRAP_TYPE_COUNT {len(trap_types)}",
        f"#define AP_ITEM_FLAG_WORDS {(max(data.get('flag_id') or 0 for data in items.values()) + 32) // 32}",
        "enum {"]
    header += [f"    AP_TRAP_TYPE_{re.sub(r'[^A-Za-z0-9]+', '_', name).upper()} = {index},"
        for index, name in enumerate(trap_types)]
    header += ["};", "static const char* const ap_trap_type_names[AP_TRAP_TYPE_COUNT] = {"]
    header += [f'    "{name}",' for name in trap_types]
    header += ["};", "enum { AP_TRAP_MAP_BANNED, AP_TRAP_MAP_SUPER, AP_TRAP_MAP_ALLOW };",
        f"#define AP_TRAP_MAP_COUNT {len(trap_maps)}",
        "static const unsigned char ap_trap_map_state[AP_TRAP_MAP_COUNT] = {"]
    for offset in range(0, len(trap_maps), 24):
        header.append("    " + ", ".join(str(value) for value in trap_maps[offset:offset + 24]) + ",")
    header += ["};", f"#define AP_TRAP_MOVEMENT_COUNT {len(trap_movement)}",
        "static const unsigned char ap_trap_movement_banned[AP_TRAP_MOVEMENT_COUNT] = {"]
    for offset in range(0, len(trap_movement), 24):
        header.append("    " + ", ".join(str(value) for value in trap_movement[offset:offset + 24]) + ",")
    header += ["};", "static const ap_item_effect_def ap_item_effects[AP_ITEM_COUNT] = {"]
    for ap_id, data in sorted(items.items()):
        effects = item_effects(ap_id, data, trap_types)
        effects += [("NONE", 0, 0)] * (2 - len(effects))
        values = ", ".join(f"{{AP_EFFECT_{kind}, {arg0}, {arg1}}}" for kind, arg0, arg1 in effects)
        header.append(f'    {{{ap_id}u, {{{values}}}}}, /* {data["name"]} */')
    header += ["};"]
    shops = [c for c in checks if c["detector"] == "shop"]
    header += [f"#define AP_SHOP_COUNT {len(shops)}", "#define AP_SHOP_SHARED 5",
        "typedef struct { unsigned int id; unsigned char vendor, level, kong; } ap_shop_location_def;",
        "static const ap_shop_location_def ap_shop_locations[AP_SHOP_COUNT] = {"]
    for check in shops:
        level, vendor, kong = check["name"].split()
        level = ["Japes", "Aztec", "Factory", "Galleon", "Forest", "Caves", "Castle", "Isles"].index(level)
        vendor = ["Cranky", "Funky", "Candy"].index(vendor)
        kong = ["Donkey", "Diddy", "Lanky", "Tiny", "Chunky", "Shared"].index(kong)
        header.append(f'    {{{check["id"]}u, {vendor}, {level}, {kong}}}, /* {check["name"]} */')
    header += ["};", ""]
    (out / "catalog.h").write_text("\n".join(header), encoding="utf-8")
    patch_sites = []
    implemented_sites = {int(pc, 16) for function in gameplay["functions"] for kind in ("words", "hooks", "halfwords") for pc in function.get(kind, {})}
    # Relocatable symbol loads replace upstream HI/LO immediate writes.
    implemented_sites.update(int(pc, 16) + 2 for function in gameplay["functions"] for pc in function.get("assembly", {}))
    # Whole-function dispatchers replace the callee of these original JAL sites.
    # The lifter verifies the actual source calls target that function.
    implemented_sites.update(int(pc, 16) for function in gameplay["functions"] for pc in function.get("replaces_calls", []))
    for path in sorted((source / "randomizer/Patching").rglob("*.py")):
        tree = ast.parse(path.read_text(encoding="utf-8-sig"))
        for node in ast.walk(tree):
            if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id in ("writeValue", "writeFunction", "writeHook", "writeFloat", "writeLabelValue"):
                status = "not_ported"
                if (path.relative_to(source).as_posix() in gameplay["sources"] and len(node.args) > 1
                    and isinstance(node.args[1], ast.Constant) and node.args[1].value in implemented_sites):
                    status = "implemented_gated"
                patch_sites.append({"file": path.relative_to(source).as_posix(), "line": node.lineno,
                    "operation": node.func.id, "arguments": [ast.unparse(a) for a in node.args], "status": status})
    c_sources = [{"file": p.relative_to(source).as_posix(), "sha256": hashlib.sha256(p.read_bytes().replace(b"\r\n", b"\n")).hexdigest(),
        "lines": len(p.read_text(encoding="utf-8").splitlines())} for p in sorted((source / "base-hack/src").rglob("*.c"))]
    report = {"revision": REVISION, "location_count": len(checks), "item_count": len(items),
        "catalog_sha256": digest,
        "detectors": {kind: sum(x["detector"] == kind for x in checks) for kind in ("stock_flag", "event", "shop", "enemy")},
        "unresolved": [x for x in checks if x["flag"] is None], "c_sources": c_sources,
        "gameplay": {"functions": len(gameplay["functions"]), "source_sites_implemented_gated": sum(p["status"] == "implemented_gated" for p in patch_sites),
            "runtime_enabled": False, "manifest": "mod/gameplay-patches.json"},
        "patch_sites": patch_sites, "note": "Source-site inventory, not runtime-expanded patches or a claim of gameplay compatibility."}
    (out / "port-coverage.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("location_count", "item_count", "detectors", "unresolved")}))
    print(f"Inventoried {len(c_sources)} C files / {sum(x['lines'] for x in c_sources)} lines and {len(patch_sites)} Python patch call sites.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(); parser.add_argument("source", type=Path)
    generate(parser.parse_args().source)
