"""Verify local US ROM metadata and generate ID/counter mappings (no assets).

Input is the user's locally decompressed ROM. Table layouts and pickup ordering
follow the pinned decomp code_135D30.c, code_36880.c and code_F9450.c.
"""
from pathlib import Path
import collections
import struct
import sys
import zlib

root = Path(__file__).resolve().parents[1]
rom = Path(sys.argv[1]).read_bytes()
def at(address, fmt):
    return struct.unpack_from(fmt, rom, 0x2000000 + address - 0x805FB300)

levels = at(0x807445E0, '>221B')
lobbies = at(0x80744734, '>9H')
def level(m):
    return 7 if m in lobbies else levels[m]

gbs = {}
gb_props = {}
for i in range(113):
    m, obj, flag, actor = at(0x80755A20 + i * 8, '>BxhhBx')
    if 0x225 <= flag <= 0x24C:  # Five Helm medals also use this prop table.
        continue
    assert 2 <= actor <= 6 and level(m) < 8 and flag not in gbs
    gbs[flag] = (m, level(m), actor - 2)
    gb_props[flag] = obj
# Parent maps for the 52 used reward-table entries. These are the vanilla
# BonusBarrelData/MinigameLocation map identities audited against the pinned
# setup files. Nine non-barrel reward rows are included explicitly: two
# minecarts, Japes' boulder, Aztec's vulture and the five Isles instrument pads.
bonus_maps = {
    0x001: 7, 0x002: 7, 0x00B: 7, 0x018: 6, 0x019: 7, 0x01C: 7,
    0x034: 38, 0x03B: 24, 0x03C: 23, 0x03E: 38, 0x044: 38, 0x049: 20,
    0x074: 26, 0x086: 26, 0x087: 26, 0x088: 26, 0x089: 26,
    0x0A3: 30, 0x0A4: 30, 0x0B8: 47, 0x0C5: 43, 0x0C6: 43,
    0x0C8: 46, 0x0CA: 179, 0x0D3: 48, 0x0E0: 63, 0x0E3: 64,
    0x0EB: 59, 0x0FA: 48, 0x0FE: 188, 0x107: 90, 0x126: 72,
    0x127: 72, 0x12A: 186, 0x131: 88, 0x132: 105, 0x137: 112,
    0x13B: 151, 0x13C: 163, 0x13E: 106, 0x13F: 164, 0x15E: 87,
    0x18E: 34, 0x192: 173, 0x194: 34, 0x196: 170, 0x19A: 34,
    0x19F: 193, 0x1A0: 195, 0x1A8: 34, 0x1A9: 34, 0x1AC: 34,
}
# Actor-spawner IDs for the 43 physical bonus barrels. The remaining nine
# reward-table rows are scripted races, boulders, vultures or instrument pads.
# These IDs are pinned vanilla setup identities (runtime actor type 0x1C).
bonus_barrels = {
    0x001: 32, 0x002: 31, 0x00B: 34, 0x01C: 33,
    0x034: 35, 0x03B: 0, 0x03C: 0, 0x03E: 33, 0x049: 2,
    0x074: 16, 0x086: 13, 0x087: 0, 0x088: 14, 0x089: 15,
    0x0A3: 6, 0x0A4: 7, 0x0B8: 0, 0x0C5: 1, 0x0C6: 0,
    0x0C8: 0, 0x0CA: 3, 0x0D3: 18, 0x0E0: 0, 0x0E3: 8,
    0x0EB: 3, 0x0FA: 21, 0x0FE: 22, 0x107: 0, 0x126: 6,
    0x127: 7, 0x12A: 19, 0x131: 1, 0x132: 0, 0x137: 0,
    0x13B: 0, 0x13C: 0, 0x13F: 0, 0x15E: 9, 0x192: 1,
    0x196: 10, 0x19F: 2, 0x1A0: 2, 0x1AC: 11,
}
assert set(bonus_barrels) < set(bonus_maps)
bonus_flags = set()
for i in range(53):
    flag, actor = at(0x80755F50 + i * 4, '>hBx')
    if not actor:
        assert flag == 0x1C4  # Unused bonus barrel.
        continue
    bonus_flags.add(flag)
    lvl = next((j for j, end in enumerate((0x30, 0x70, 0x9A, 0xCD, 0x103, 0x131, 0x17D)) if flag < end), 7)
    reward = (bonus_maps[flag], lvl, actor - 2)
    if flag in gbs:
        # Baboon Blast returns to a persistent reward prop in the main map.
        # That loaded prop, rather than the excluded minigame overlay, is the
        # source-map guard relevant to live delivery.
        assert gbs[flag][1:] == reward[1:]
    else:
        gbs[flag] = reward
assert bonus_flags == set(bonus_maps)
# Direct scripted rewards outside the prop/barrel tables: seal race, Chunky
# minecart and Tiny castle car race (pinned decomp reward call sites).
for flag, reward in {0xA5: (39, 3, 0), 0xD7: (55, 4, 4), 0x145: (185, 6, 3)}.items():
    assert flag not in gbs
    gbs[flag] = reward
assert len(gbs) == 161
distribution = collections.Counter((l, k) for _, l, k in gbs.values())
assert all(distribution[l, k] == (5 if (l, k) == (7, 3) else 4) for l in range(8) for k in range(5))

types = {10, 13, 22, 28, 29, 30, 31, 35, 36, 39, 43, 517, 518, 519, 520}
mapping = {}
for i in range(40):
    typ, hud, _, _, _, actor, _, _, _ = at(0x80753EF0 + i * 20, '>HbBIIhhhh')
    if typ in types:
        assert typ not in mapping and hud in (0, 1) and 2 <= actor <= 6
        mapping[typ] = (actor - 2, 0 if hud == 1 else 5 if typ in (43, 517, 518, 519, 520) else 1)
assert set(mapping) == types
base = 0x101C50
table = base + struct.unpack_from('>I', rom, base + 9 * 4)[0]
pickups = [[] for _ in range(8)]
actors = []
model2_types = {}
actor_types = {}
actor_ordinal = 0
for m in range(221):
    start, end = struct.unpack_from('>II', rom, table + m * 4)
    data = zlib.decompress(rom[base + (start & 0x7FFFFFFF):base + (end & 0x7FFFFFFF)], 47)
    count = struct.unpack_from('>I', data)[0]
    assert 4 + count * 48 <= len(data)
    for i in range(count):
        typ, obj = struct.unpack_from('>HH', data, 4 + i * 48 + 40)
        assert (m, obj) not in model2_types
        model2_types[m, obj] = typ
        if typ in types:
            assert level(m) < 8
            pickups[level(m)].append((*mapping[typ], m, obj, typ))
    offset = 4 + count * 48
    offset += 4 + struct.unpack_from('>I', data, offset)[0] * 36
    actor_count = struct.unpack_from('>I', data, offset)[0]
    assert offset + 4 + actor_count * 56 == len(data)
    for i in range(actor_count):
        typ, obj = struct.unpack_from('>HH', data, offset + 4 + i * 56 + 50)
        typ += 16
        assert (m, obj) not in actor_types
        actor_types[m, obj] = typ
        if typ in (0x5B, 0x6F, 0x70, 0x71, 0x72, 0x8B):
            flag = 0x26B + actor_ordinal
            actor_ordinal += 1
            if level(m) >= 8:
                assert m == 0 and typ == 0x72, (m, typ)  # Test-map balloon, not adventure.
                continue
            kong = {0x5B: 1, 0x6F: 4, 0x70: 3, 0x71: 2, 0x72: 0, 0x8B: 0}[typ]
            actors.append((flag, obj, typ, m, level(m), kong, 0 if typ == 0x8B else 10))
assert actor_ordinal == 121 and len(actors) == 120
assert all(actor_types[bonus_maps[flag], obj] == 0x1C for flag, obj in bonus_barrels.items())
assert actor_types[7, 6] == 0x3D  # Rear-Japes boulder that owns bunch flag 0x01D.
assert sum(a[6] == 0 for a in actors) == 16
assert [len(p) for p in pickups] == [226, 263, 245, 245, 271, 194, 253, 3]
amounts = collections.Counter()
for l, rows in enumerate(pickups):
    for k, amount, _, _, _ in rows:
        amounts[l, k] += amount
assert all(n <= 100 for n in amounts.values())
for _, _, _, _, l, k, amount in actors:
    amounts[l, k] += amount
# Check setup totals, including three surplus single-banana entries. Chunky's
# Japes boulder actor drop is outside both persistent pickup tables.
expected_cb = {(0, 4): 95, (0, 0): 101, (0, 1): 101, (3, 3): 101}
assert all(amounts[l, k] == expected_cb.get((l, k), 100) for l in range(7) for k in range(5))

lines = ['// Generated by tools/generate-collectibles.py from verified local US metadata.',
         '// Only collectible IDs, counter destinations and amounts; no ROM assets.',
         '#ifndef COOP_COLLECTIBLE_IDS_H', '#define COOP_COLLECTIBLE_IDS_H',
         'typedef struct { unsigned short flag, prop, prop_type; unsigned char barrel, map, level, kong; } CoopGoldenBanana;',
         'static const CoopGoldenBanana coop_golden_bananas[161] = {']
for flag, (m, l, k) in sorted(gbs.items()):
    prop = gb_props.get(flag, 0xFFFF)
    prop_type = model2_types[m, prop] if prop != 0xFFFF else 0xFFFF
    barrel = bonus_barrels.get(flag, 0xFF)
    lines.append(f'    {{0x{flag:03X}, 0x{prop:04X}, 0x{prop_type:04X}, 0x{barrel:02X}, {m}, {l}, {k}}},')
lines += ['};', 'typedef struct { unsigned short object, type; unsigned char map, kong, amount; } CoopPickup;',
          '// amount 0 = one coin; amount 1/5 = colored bananas. Ordinal is per level.',
          'static const unsigned short coop_pickup_start[9] = {0, 226, 489, 734, 979, 1250, 1444, 1697, 1700};',
          'static const CoopPickup coop_pickups[1700] = {']
for l, rows in enumerate(pickups):
    lines.append(f'    // Level {l}, setup order within ascending map IDs.')
    lines += [f'    {{0x{obj:04X}, 0x{typ:04X}, {m}, {k}, {amount}}},' for k, amount, m, obj, typ in rows]
lines += ['};', 'typedef struct { unsigned short flag, object; unsigned char type, map, level, kong, amount; } CoopActorPickup;',
          '// amount 10 = balloon; amount 0 = rainbow coin (+5 to every Kong).',
          'static const CoopActorPickup coop_actor_pickups[120] = {']
lines += [f'    {{0x{f:03X}, 0x{o:04X}, 0x{t:02X}, {m}, {l}, {k}, {a}}},' for f, o, t, m, l, k, a in actors]
lines += ['};', '#endif', '']
output = '\n'.join(lines)
path = root / 'mod/collectible_ids.h'
if '--check' in sys.argv:
    assert path.read_text(encoding='utf-8') == output
else:
    path.write_text(output, encoding='utf-8')
print('PASS: 161 GBs, 1700 ordinary pickups, 104 balloons, 16 rainbow coins; all 40 GB and 35 CB buckets checked')
