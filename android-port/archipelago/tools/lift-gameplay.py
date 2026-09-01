"""Relocate verified stock functions into recomp replacements, with original fallback.

This is a build-time transformation of the user's prepared ROM, never a runtime
instruction write. Only pinned functions/patches are accepted. No ROM is exported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import tomllib

ROOT = Path(__file__).resolve().parents[1]


def signed16(value):
    value &= 0xffff
    return value - 0x10000 if value & 0x8000 else value


class Lifter:
    def __init__(self, port, manifest):
        self.rom = (port / "upstream/donkeykong64.decompressed.us.z64").read_bytes()
        if hashlib.sha256(self.rom).hexdigest() != manifest["rom_sha256"]:
            raise ValueError("Prepared ROM differs from the supported US build")
        self.sections = tomllib.loads((port / "upstream/DK64Syms/dump.toml").read_text())["section"]
        self.functions = {}
        for section in self.sections:
            for function in section.get("functions", []):
                self.functions.setdefault(function["vram"], []).append((section, function))
        self.report = []

    def word(self, section, address):
        offset = section["rom"] + address - section["vram"]
        if address < section["vram"] or address + 4 > section["vram"] + section["size"] or offset + 4 > len(self.rom):
            raise ValueError(f"Address outside source section: {address:08x}")
        return struct.unpack_from(">I", self.rom, offset)[0]

    def function(self, address, name):
        choices = self.functions.get(address, [])
        choices = [(s, f) for s, f in choices if f["name"] == name]
        if len(choices) != 1:
            raise ValueError(f"Function must identify one source overlay: {name}/{address:08x}")
        return choices[0]

    def emit(self, entry):
        start = int(entry["vram"], 16)
        section, function = self.function(start, entry["function"])
        for address in entry.get("replaces_calls", []):
            pc = int(address, 16)
            word = self.word(section, pc)
            target = ((pc + 4) & 0xf0000000) | ((word & 0x3ffffff) << 2)
            if word >> 26 != 3 or target != start:
                raise ValueError(f"Claimed source call does not target {function['name']}: {address}")
        end = start + function["size"]
        words = {pc: self.word(section, pc) for pc in range(start, end, 4)}
        body = b"".join(struct.pack(">I", word) for word in words.values())
        if hashlib.sha256(body).hexdigest() != entry["sha256"] or function["name"] != entry["function"]:
            raise ValueError(f"Function/symbol mismatch: {entry['function']}")
        changes = {int(pc, 16): int(value, 16) for pc, value in entry.get("words", {}).items()}
        halves = {int(pc, 16): int(value, 16) for pc, value in entry.get("halfwords", {}).items()}
        assembly = {int(pc, 16): line for pc, line in entry.get("assembly", {}).items()}
        hooks = {int(pc, 16): asm for pc, asm in entry.get("hooks", {}).items()}
        for pc, value in halves.items():
            if pc % 2 or pc & ~3 not in words or not 0 <= value <= 65535:
                raise ValueError("Invalid halfword edit")
            base, shift = pc & ~3, (0 if pc & 2 else 16)
            if base in changes: raise ValueError("Overlapping word/halfword edits")
            changes[base] = (words[base] & ~(0xffff << shift)) | (value << shift)
        if any(pc not in words for pc in changes.keys() | assembly.keys()) or any(pc not in words or pc + 4 not in words for pc in hooks):
            raise ValueError("Patch outside function")
        if changes.keys() & assembly.keys() or any(pc in changes or pc + 4 in changes or pc in assembly or pc + 4 in assembly or pc + 4 in hooks for pc in hooks):
            raise ValueError("Overlapping patches")
        # Recognize and relocate the stock compiler's LUI/ADDU/LW/JR tables.
        # Unknown indirect jumps fail the build instead of retaining old code pointers.
        tables, relocations = {}, {}
        for pc, instruction in words.items():
            opcode = instruction >> 26
            if opcode in (1, 4, 5, 6, 7, 20, 21, 22, 23) or (opcode == 17 and (instruction >> 21) & 31 == 8):
                target = pc + 4 + 4 * signed16(instruction)
                if target not in words:
                    raise ValueError(f"Branch outside lifted function at {pc:08x}")
            if instruction & 0xfc1fffff != 8 or instruction >> 21 == 31:
                continue
            upper, add, lower = (words.get(pc - delta, 0) for delta in (12, 8, 4))
            reg = (upper >> 16) & 31
            if (upper >> 26 != 15 or add & 0xfc00003f != 0x21 or (add >> 11) & 31 != reg
                or ((add >> 21) & 31 != reg and (add >> 16) & 31 != reg)
                or lower >> 26 != 35 or (lower >> 21) & 31 != reg or (lower >> 16) & 31 != instruction >> 21):
                raise ValueError(f"Unrecognized indirect jump at {pc:08x}")
            address = ((upper & 0xffff) << 16) + signed16(lower)
            entries = []
            while True:
                target = self.word(section, address + 4 * len(entries))
                if target < start or target >= end or target % 4:
                    break
                entries.append(target)
                if len(entries) > 256:
                    raise ValueError("Oversized jump table")
            if not entries:
                raise ValueError("Empty jump table")
            tables[address] = entries
            relocations[pc - 12] = ("R_MIPS_HI16", address)
            relocations[pc - 4] = ("R_MIPS_LO16", address)
        output = []
        variants = ("stock", "rando") if changes or hooks or assembly else ("stock",)
        for variant in variants:
            symbol = f"ap_{variant}_{function['name']}"
            prefix = f".L{variant}_{start:08X}_"
            label = lambda pc: prefix + f"{pc:08X}"
            table_label = lambda pc: prefix + f"table_{pc:08X}"
            output += ['.section .text.ap_lift,"ax",@progbits', '.balign 4', f'.globl {symbol}', f'.type {symbol},@function', f'{symbol}:']
            for pc, original in words.items():
                word = changes.get(pc, original) if variant == "rando" else original
                output.append(f"{label(pc)}:")
                if variant == "rando" and pc in hooks:
                    output.append(f"j {prefix}hook_{pc:08X}")
                    continue
                if variant == "rando" and pc - 4 in hooks:
                    output.append("nop")
                    continue
                if variant == "rando" and pc in assembly:
                    if "\n" in assembly[pc]: raise ValueError("Assembly edit must be one instruction")
                    output.append(assembly[pc])
                    continue
                if pc in relocations:
                    kind, address = relocations[pc]
                    if kind == "R_MIPS_HI16":
                        output.append(f"lui ${(word >> 16) & 31}, %hi({table_label(address)})")
                    else:
                        output.append(f"lw ${(word >> 16) & 31}, %lo({table_label(address)})(${(word >> 21) & 31})")
                elif word >> 26 in (2, 3):
                    target = ((pc + 4) & 0xf0000000) | ((word & 0x3ffffff) << 2)
                    if start <= target < end:
                        name = label(target)
                    else:
                        candidates = self.functions.get(target, [])
                        # Overlays reuse virtual addresses. A call in an overlay
                        # resolves within that overlay before considering static code.
                        local = [(s, f) for s, f in candidates if s is section]
                        if local: candidates = local
                        if len(candidates) != 1:
                            raise ValueError(f"Ambiguous call target {target:08x}")
                        name = candidates[0][1]["name"]
                    output.append(f"{'jal' if word >> 26 == 3 else 'j'} {name}")
                else:
                    output.append(f".word 0x{word:08X}")
            if variant == "rando":
                for pc, hook_lines in hooks.items():
                    output.append(f"{prefix}hook_{pc:08X}:")
                    for line in hook_lines:
                        # Explicit local labels use @; original continuation addresses use {XXXXXXXX}.
                        line = line.replace("@", prefix)
                        def continuation(match):
                            target = int(match[1], 16)
                            if target not in words: raise ValueError("Hook continuation outside function")
                            return label(target)
                        line = re.sub(r"\{([0-9A-Fa-f]{8})\}", continuation, line)
                        output.append(line)
            output.append(f".size {symbol}, .-{symbol}")
            for address, entries in tables.items():
                output += ['.balign 4', f'{table_label(address)}:']
                output += [f".word {label(target)}" for target in entries]
                output.append('.word 0')  # Terminate the recompiler's table-size analysis.
        if len(variants) == 2:
            name = function["name"]
            gate_mask = entry.get("gate_mask", 1)
            if gate_mask not in (1, 4, 8, 32): raise ValueError("Invalid gameplay capability")
            output += ['.section .recomp_patch,"ax",@progbits', '.balign 4', f'.globl {name}', f'.type {name},@function', f'{name}:']
            if "dispatch" in entry:
                dispatch = entry["dispatch"]
                if dispatch["mask"] != 4 or not re.fullmatch(r"ap_apshop_[a-z]+", dispatch["symbol"]):
                    raise ValueError("Invalid AP shop dispatcher")
                output += ['lui $t0, %hi(ap_gameplay_mode)', 'lw $t0, %lo(ap_gameplay_mode)($t0)', 'andi $t0, $t0, 4',
                    f'beqz $t0, .Llegacy_{start:08X}', 'nop', f'j {dispatch["symbol"]}', 'nop', f'.Llegacy_{start:08X}:']
            output += [
                'lui $t0, %hi(ap_gameplay_mode)', 'lw $t0, %lo(ap_gameplay_mode)($t0)', f'andi $t0, $t0, {gate_mask}',
                f'beqz $t0, .Lfallback_{start:08X}', 'nop', f'j ap_rando_{name}', 'nop', f'.Lfallback_{start:08X}:',
                f'j ap_stock_{name}', 'nop', f'.size {name}, .-{name}']
        self.report.append({"function": function["name"], "section": section["name"], "vram": entry["vram"], "source_bytes": len(body),
            "word_edits": len(changes), "assembly_edits": len(assembly), "hooks": len(hooks), "jump_tables": len(tables), "variants": list(variants)})
        if "dispatch" in entry: self.report[-1]["dispatch"] = entry["dispatch"]
        return output


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=Path, default=ROOT.parent)
    parser.add_argument("--output", type=Path, default=ROOT / "build/mod")
    args = parser.parse_args()
    manifest = json.loads((ROOT / "mod/gameplay-patches.json").read_text())
    lifter = Lifter(args.port, manifest)
    output = ['/* Generated locally from the verified user ROM. Do not edit. */', '.set noreorder', '.set noat']
    for entry in manifest["functions"]:
        output += lifter.emit(entry)
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / "gameplay.S").write_text("\n".join(output) + "\n")
    (args.output / "gameplay-lift.json").write_text(json.dumps(lifter.report, indent=2) + "\n")
    print(f"Lifted {len(lifter.report)} verified functions; campaign gate remains disabled")


if __name__ == "__main__":
    main()
