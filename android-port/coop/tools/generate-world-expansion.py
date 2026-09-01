"""Generate reviewed vanilla world-unlock metadata, never script/ROM assets.

Usage: python tools/generate-world-expansion.py <decompressed-US-ROM>
The explicit selection is intentional: positive permanent writes alone do not
distinguish an unlock from a tutorial, payment, or reversible puzzle latch.
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
table = base + struct.unpack_from('>I', rom, base + 10 * 4)[0]
flags = collections.defaultdict(lambda: {'writers': set(), 'readers': set(), 'initializers': set()})
for m in range(216):
    a, b = struct.unpack_from('>II', rom, table + 4 * m)
    data = rom[base + (a & 0x7fffffff):base + (b & 0x7fffffff)]
    if not data:
        continue
    if data[:2] == b'\x1f\x8b':
        data = zlib.decompress(data, 47)
    offset = 2
    for _ in range(struct.unpack_from('>H', data)[0]):
        obj, blocks, _ = struct.unpack_from('>3H', data, offset)
        offset += 6
        for _ in range(blocks):
            sides = []
            for _ in range(2):
                count = struct.unpack_from('>H', data, offset)[0]
                offset += 2
                sides.append([struct.unpack_from('>4H', data, offset + i * 8) for i in range(count)])
                offset += count * 8
            conditions, executions = sides
            for op, flag, _, _ in conditions:
                if (op & 0x7fff) == 45:
                    flags[flag]['readers'].add((m, obj))
                    if (1, 0, 0, 0) in conditions:
                        flags[flag]['initializers'].add((m, obj))
            for op, flag, value, arg in executions:
                if op == 107:
                    flags[flag]['writers'].add((m, obj, value, arg))

# Append-only after the 18 world rows released in v0.13. Level 255 is for
# scripts/actors confined to interiors excluded by coop_items_safe_map().
# The first seven Japes rows were started before the broader request.
rows = [
    (0x007, 0, 'Japes shell/hive gate'),
    (0x00D, 0, 'Japes Diddy-switch hut'),
    (0x00E, 0, 'Japes Lanky-switch hut'),
    (0x00F, 0, 'Japes DK-switch hut'),
    (0x010, 0, 'Japes Tiny-switch hut'),
    (0x01E, 0, 'Japes painting-cave gate'),
    (0x02B, 0, 'Japes Rambi rock wall'),
    (0x000, 0, 'Japes first gate'),
    (0x02E, 0, 'Japes boss portals closed after key'),
    (0x02F, 1, 'Aztec permanent door, object 0x40'),
    (0x030, 1, 'Aztec permanent door, object 0x8D'),
    (0x035, 1, 'Aztec beetle-race tower reveal'),
    (0x037, 1, 'Aztec five-door-temple totem activated'),
    (0x045, 1, 'Tiny Temple ice melted'),
    (0x04E, 1, 'Aztec tunnel door'),
    (0x06C, 1, 'Aztec boss portals closed after key'),
    (0x098, 2, 'Factory boss portals closed after key'),
    (0x099, 3, 'Galleon gate, object 2'),
    (0x09B, 3, 'Galleon coconut gate'),
    (0x09C, 3, 'Galleon ship/lighthouse activation'),
    (0x09F, 3, 'Galleon Enguarde door'),
    (0x0A1, 3, 'Galleon peanut gate'),
    (0x0A2, 3, 'Galleon gate, object 5'),
    (0x0B3, 3, 'Galleon treasure chest, object 0xD'),
    (0x0B4, 3, 'Galleon fairy chest'),
    (0x0B5, 3, 'Galleon GB chest'),
    (0x0B9, 3, 'Galleon breakable gate'),
    *[(0xBA + i, 3, f'Galleon pearl {i + 1}') for i in range(5)],
    (0x0CB, 3, 'Galleon boss portals closed after key'),
    (0x0CF, 4, 'Fungi green tunnel feather gate'),
    (0x0D0, 4, 'Fungi green tunnel pineapple gate'),
    (0x0D2, 4, 'Fungi gold tunnel gate'),
    (0x0D4, 4, 'Fungi mill pulley raised'),
    (0x0D5, 4, 'Fungi mill pulley guitar sequence complete'),
    (0x0DA, 4, 'Fungi front-mill question-mark box'),
    (0x0DC, 4, 'Fungi mill conveyor puzzle complete'),
    (0x0DE, 4, 'Fungi rear-mill question-mark box'),
    (0x0DF, 4, 'Fungi rear-mill Mini Monkey box'),
    (0x0E5, 4, 'Fungi mushroom board puzzle complete'),
    (0x102, 4, 'Fungi boss portals closed after key'),
    *[(0x109 + i, 5, f'Caves breakable ice wall {i + 1}') for i in range(3)],
    (0x118, 5, 'Caves Lanky igloo balloon pad reveal'),
    (0x128, 5, 'Caves igloo instrument pads revealed'),
    (0x12C, 5, 'Caves persistent encounter clear'),
    (0x12E, 5, 'Caves boss portals closed after key'),
    (0x130, 6, 'Castle ballroom encounter clear/barrel reveal'),
    (0x133, 6, 'Castle tower encounter clear/balloon pad'),
    (0x138, 6, 'Castle crypt metal panel opened'),
    (0x141, 6, 'Castle toolshed question-mark box'),
    (0x144, 6, 'Castle tree gate'),
    *[(0x157 + i, 6, f'Castle basement breakable grate {i + 1}') for i in range(6)],
    (0x160, 6, 'Castle boss portals closed after key'),
    (0x181, 255, 'Training grounds exit switch'),
    (0x189, 255, 'Fairy Island Rareware room door'),
    (0x18D, 255, 'Factory lobby lever/panel'),
    (0x18F, 255, 'Aztec lobby stone panel'),
    (0x190, 255, 'Aztec lobby encounter platform'),
    (0x191, 255, 'Galleon lobby door'),
    (0x195, 255, 'Fungi lobby gun-order puzzle complete'),
    (0x197, 255, 'Helm lobby coconut-switch route'),
    (0x198, 255, 'Caves lobby ice wall 1'),
    (0x199, 255, 'Caves lobby ice wall 2'),
    (0x1A1, 255, 'Helm lobby Bananaport tag 1'),
    (0x1A2, 255, 'Helm lobby Bananaport tag 2'),
    (0x1AA, 7, 'Isles trombone-pad barrel reveal'),
    (0x1AE, 7, 'Isles boulder opened'),
    (0x302, 8, 'Helm shutdown complete'),
    (0x303, 8, 'Helm coin door'),
    (0x304, 8, 'Helm crown door'),
    (0x305, 8, 'Helm Bananaport tag 1'),
    (0x306, 8, 'Helm Bananaport tag 2'),
]

# All script-written flags not already shared or selected are explicitly
# classified. No arbitrary newly encountered flag is automatically granted.
excluded = {0xA0: 'reversible Galleon water height', 0xCE: 'reversible day/night',
            0x19D: 'reversible Caves-lobby pressure switch'}
for flag in (0x2C, 0x2D, *range(0x60, 0x6C), 0xC3, 0xC4, 0xFF, 0x100,
             0x163, 0x164, 0x166, *range(0x16F, 0x173), 0x188, 0x1AB):
    excluded[flag] = 'tutorial/first-use/repeat-cutscene bookkeeping, not a persistent unlock'
known = set()
for name in ('collectible_ids.h', 'travel_ids.h'):
    known.update(int(n, 16) for n in re.findall(r'\{0x([0-9a-fA-F]+),', (root / 'mod' / name).read_text(encoding='utf-8')))
known.update(range(0x1D5, 0x26B))
known.update(range(0x1BB, 0x1C4))
known.update(range(0x1CD, 0x1D5))
known.update((0x6,0x42,0x46,0x75,0x315,0x17F,0x182,0x183,0x184,0x185,0x187,0x180,0x179,
              0x1A,0x4A,0x8A,0xA8,0xEC,0x124,0x13D,0x17C,0x84,0x17B))
# Exact v0.13 prefix, deliberately not parsed from the expanded header.
known.update((0x32,0x4C,0x9E,0xE6,0xE7,0xE8,0xE9,0xEA,0xF8,0xFB,
              0x6D,0x6E,0x6F,0x77,0x78,0x81,0x85,0x97))
selected = {f for f, _, _ in rows}
assert len(rows) == 82 and len(selected) == len(rows)
assert not selected & known and not selected & excluded.keys()
assert not {f for f, v in flags.items() if v['writers']} - known - selected - excluded.keys()
safe_maps = {7,26,30,34,38,48,72,87,171}
for flag, level, _ in rows:
    record = flags[flag]
    assert record['writers'] and record['initializers']
    assert all(value == 1 and arg == 0 for _, _, value, arg in record['writers'])
    if level == 255:
        assert not {m for m, _ in record['readers']} & safe_maps
pulley = 18 + next(i for i, row in enumerate(rows) if row[0] == 0xD4)
lines = ['// Generated by tools/generate-world-expansion.py. Metadata only.',
         '// Append-only after v0.13; flag, excluded receiving level, world prerequisite.']
for i, (flag, level, label) in enumerate(rows):
    prerequisite = pulley if flag == 0xD5 else -1
    lines.append(f'    {{0x{flag:03X}, {level}, {prerequisite}}}, // ID {2312+i}: {label}.')
(root / 'mod/world_expansion.inc').write_text('\n'.join(lines) + '\n', encoding='utf-8')
doc = ['# Vanilla permanent-world flag coverage (0.14, untested)', '',
       'All 216 vanilla US instance-script maps were inspected. This file contains',
       'only flag/object metadata, not ROM assets or replacement scripts.',
       'Compilation and metadata generation are not runtime or gameplay validation.', '',
       'This pass adds 82 IDs to the 2312 in v0.13: **2394 IDs total**, including',
       '100 world rows. The selection covers every remaining script-written',
       'persistent unlock/completion flag; excluded bookkeeping is listed below.',
       'Existing GB-controlled doors/reveals retain their original collectible IDs.', '',
       'Level 255 means related scripts are confined to excluded interiors: receive',
       'in any normal safe snapshot map. Other rows require leaving their level.',
       'Level 8 is Helm; all safe snapshot maps are outside it.', '',
       '| ID | Flag | Excluded level | Completion | Vanilla writer map/object |',
       '| --- | --- | --- | --- | --- |']
for i, (flag, level, label) in enumerate(rows):
    writers = ', '.join(f'{m}/0x{o:X}' for m, o, _, _ in sorted(flags[flag]['writers']))
    doc.append(f'| {2312+i} | 0x{flag:03X} | {level} | {label} | {writers} |')
doc += ['', '## Excluded script-written state', '', '| Flag | Reason |', '| --- | --- |']
doc += [f'| 0x{f:03X} | {reason} |' for f, reason in sorted(excluded.items())]
doc += ['', 'Paid arcade entry (0x83), level visits, captions, first-time actor dialogue,',
        'quit/ending/global flags and ongoing boss phases are not world unlocks.',
        'Temporary switches, timers, live actors, damage, water height and day/night',
        'need separate synchronization; they must not be ORed into this ownership set.', '',
        'Prerequisites: closed boss portals require their boss key; Rareware room',
        'requires camera/shockwave and 20 fairies; Helm coin door requires both',
        'special coins; Helm crown door requires any four crowns; mill pulley',
        'completion requires its raised state. No collectible is spent by receiving',
        'a flag. The local game must award the completion; no auto-clear is inferred.', '']
(root / 'WORLD-UNLOCKS.md').write_text('\n'.join(doc), encoding='utf-8')
print('Generated 82 reviewed permanent flags; all remaining script-written flags classified; no tests run.')
