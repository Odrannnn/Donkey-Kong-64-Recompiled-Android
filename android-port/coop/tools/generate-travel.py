"""Extract vanilla Bananaport flag metadata from the user's decompressed US ROM.

Only map/object identities, tag flags and an existing GB dependency are emitted.
No setup/script bytecode, assets or randomizer-only flags are distributed.
Script opcodes follow the pinned decomp's code_42630.c (45: read permanent
flag; 107: write permanent flag). This is a build-time metadata generator.
"""
from pathlib import Path
import collections
import re
import struct
import sys
import zlib

root = Path(__file__).resolve().parents[1]
rom = Path(sys.argv[1]).read_bytes()
base = 0x101C50

def table_entry(table_index, map_id):
    table = base + struct.unpack_from('>I', rom, base + table_index * 4)[0]
    start, end = struct.unpack_from('>II', rom, table + map_id * 4)
    raw = rom[base + (start & 0x7FFFFFFF):base + (end & 0x7FFFFFFF)]
    return zlib.decompress(raw, 47) if raw[:2] == b'\x1f\x8b' else raw

def scripts(data):
    result = {}
    offset = 2
    for _ in range(struct.unpack_from('>H', data)[0]):
        obj, count, _ = struct.unpack_from('>3H', data, offset)
        offset += 6
        blocks = []
        for _ in range(count):
            sides = []
            for _ in range(2):
                n = struct.unpack_from('>H', data, offset)[0]
                offset += 2
                sides.append([struct.unpack_from('>4H', data, offset + i * 8) for i in range(n)])
                offset += n * 8
            blocks.append(sides)
        assert obj not in result
        result[obj] = blocks
    return result

header = (root / 'mod/collectible_ids.h').read_text(encoding='utf-8')
gb_section = header.split('coop_golden_bananas[161] = {', 1)[1].split('};', 1)[0]
gb_ids = {int(flag, 16): 160 + i for i, flag in enumerate(re.findall(r'\{0x([0-9A-F]+),', gb_section))}
assert len(gb_ids) == 161
tags, automatic, counts = [], [], collections.Counter()
# Adventure Bananaport maps only; multiplayer/test maps reuse prop types.
for map_id in (7, 20, 26, 30, 34, 38, 48, 72, 87, 112):
    setup = table_entry(9, map_id)
    n = struct.unpack_from('>I', setup)[0]
    pads = []
    for i in range(n):
        typ, obj = struct.unpack_from('>2H', setup, 4 + i * 48 + 40)
        if 0x210 <= typ <= 0x214:
            pads.append(obj)
    if not pads:
        continue
    local_scripts = scripts(table_entry(10, map_id))
    for obj in sorted(pads):
        blocks = local_scripts[obj]
        counts[map_id] += 1
        # FTT flag 0x163 plays a tutorial; it is not a pad unlock.
        writes = {a for _, executions in blocks for op, a, value, _ in executions
                  if op == 107 and value == 1 and a != 0x163}
        assert all(value == 1 for _, executions in blocks for op, _, value, _ in executions if op == 107)
        reads = {a for conditions, _ in blocks for op, a, _, _ in conditions if (op & 0x7FFF) == 45}
        # Initialization gates describe THIS pad's reveal. The warp-use block
        # can also read the counterpart's GB; tagging must not require that.
        init_reads = {a for conditions, _ in blocks if (1, 0, 0, 0) in conditions
                      for op, a, _, _ in conditions if (op & 0x7FFF) == 45}
        prerequisites = sorted(init_reads & gb_ids.keys())
        assert len(prerequisites) <= 1
        required = gb_ids[prerequisites[0]] if prerequisites else -1
        if writes:
            assert len(writes) == 1
            flag = next(iter(writes))
            assert flag in reads and flag not in gb_ids
            tags.append((flag, map_id, obj, required))
        else:
            # Three hidden pads have no vanilla tag bit: their existing GB
            # flag already reveals/enables them. Do not invent a second ID.
            assert len(prerequisites) == 1
            automatic.append((map_id, obj, prerequisites[0], required))

assert dict(counts) == {7:10, 20:4, 26:10, 30:10, 34:10, 38:10, 48:10, 72:10, 87:10, 112:6}
assert len(tags) == 87 and len({row[0] for row in tags}) == 87
assert automatic == [(30, 0x55, 0xA3, gb_ids[0xA3]), (38, 0x87, 0x3E, gb_ids[0x3E]), (72, 0x56, 0x127, gb_ids[0x127])]
assert [(m, obj, req) for _, m, obj, req in tags if req >= 0] == [(7, 0x12B, gb_ids[0x17])]
lines = [
    '// Generated from vanilla US setups/scripts by tools/generate-travel.py.',
    '// Metadata only. No script bytecode or invented randomizer flags.',
    '#ifndef COOP_TRAVEL_IDS_H', '#define COOP_TRAVEL_IDS_H',
    'enum { COOP_WARP_TAG_COUNT = 87 };',
    'typedef struct { unsigned short flag; unsigned char map; short required_item; } CoopWarpTag;',
    'static const CoopWarpTag coop_warp_tags[COOP_WARP_TAG_COUNT] = {'
]
lines += [f'    {{0x{flag:03X}, {m}, {required}}}, // object 0x{obj:03X}' for flag, m, obj, required in tags]
lines += ['};', '// Three automatic pads use already-shared GB ownership:']
lines += [f'// map {m}, object 0x{obj:03X}: GB flag 0x{flag:03X}, item {item}.' for m, obj, flag, item in automatic]
lines += ['#endif', '']
(root / 'mod/travel_ids.h').write_text('\n'.join(lines), encoding='utf-8')
print('Generated 87 vanilla pad tags; 3 automatic pads retain their existing GB IDs; 1 tag requires its GB reveal.')
