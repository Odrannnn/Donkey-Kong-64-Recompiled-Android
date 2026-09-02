> Current source: 0.49.0, protocol/native ABI v49. All eleven pinned native suites
> pass; the complete release build and gameplay validation remain pending. Version
> 0.48.0 passed the full pinned sanitizer and release pipeline.

# Prototype integration and limits

## Loader boundary

The `.nrm` declares `dk64_coop_bridge` with the ordinary `native_libraries` manifest
field. Its four functions are imported with `RECOMP_IMPORT(".", ...)`. Both
platforms receive the same mod bytes. The platform library sits beside the mod;
no embedded-library convention, manifest extension or loader patch is used.

`recomp_on_init`, `dk64recomp_every_frame` and (since 0.2) `recomp_on_flag_change`,
already exported by DK64's patches, are registered. There are **no original-function hooks**. Testing an
original global-function hook exposed the pinned runtime's unfinished DK64 ROM
decompressor; redirecting an already patched function also failed on Android.
Those attempts were removed. The working build uses existing event callbacks.
Since 0.4 the combat option also wraps verified entries in the game's actor
behavior table, retaining their original handlers. This edits game data through
the standard mod; it does not use runtime function hooking or modify the loader.

## Save selection

The initialization event precedes the first map initialization, which starts the
EEPROM worker. Before switching saves, the mod checks the game's worker-started
flag `D_global_asm_807467E0`. If it is already set, co-op is disabled and the save
path is left alone. This is a conservative compatibility guard, not support for
arbitrary initialization mods.

`recomp_change_save_file` selects a role-specific name inside the runtime's
mod-specific save namespace. Campaign 1 preserves the established names:
`items_host_v6` / `items_guest_v6` with item sharing enabled and
`prototype_host_v1` / `prototype_guest_v1` otherwise. Campaigns 2–8 append
`_campaignN`. Normal save data is never copied or merged. Role Off skips all save
selection, actor and networking setup. Role and campaign are read once per
process; restart after changes. IP address, port, room code and network adapter
are deliberately absent from the save key, so DHCP/network changes do not hide
progress. Both peers must select the same campaign. A guest copy ahead of the
authoritative host stops item synchronization without clearing or merging either
save. The retired `gate_host_v2` / `gate_guest_v2` files are never selected.

The default guest-save policy remains fail-closed. An explicit host-only merge
mode encodes local `CoopItemInput.enabled == 2`; it is transmitted as ordinary
feature value 1, so the peer cannot enable the policy. In that mode the host may
apply guest-owned IDs absent from its save even when they lack a live guest
request. The complete received ownership still must pass all bit bounds, prefix,
tier, key, warp and world prerequisites. The game adapter applies each accepted
ID through the existing safe-map, counter, flag and save-readback paths. Once the
host advertises the resulting union, the guest converges normally. Raw flags,
save bytes and excluded temporary state never enter this path. Users should turn
the option back off after `LAN ITEMS: SYNCED` so another unintended guest cannot
be imported into the campaign.

## Remote actor ownership

The prototype uses actor 21 (pushable box) with one of the five models read from
the local Kong model table. It replaces that actor's behavior table entry only
if it still equals the known original handler. Other box instances still call
their original handler. This is game data, not runtime loader behavior.

The proxy has no player controller, interactions or shadow; its handler only
renders. Movement is interpolated, with large displacements snapped. The main bone pose
is driven by a validated local Kong-table row and frame, without animation scripts. No game pointers, model
IDs, function addresses or animation bytecode are supplied over the network.

Before dereferencing a saved actor pointer, each frame checks the live registry,
actor type and the generation counter assigned during actor creation. Queued
deletion is tracked separately so a retiring proxy cannot fall back to the box's
gameplay handler or be replaced twice. Spawning stops near registry capacity.
Map changes, changed remote epochs/Kongs, stale state and disconnect remove the
proxy. This design still needs testing across many maps, cutscenes and deaths;
disabling interaction fields alone does not prove every script ignores it.

## Pose playback (0.3)

The game main track supplies the raw clip and fractional frame. Capture rejects
non-Kong actor types and searches the Kong column of `D_global_asm_807FBB58`.
The local offsets in `D_global_asm_807FBB50[2..3]` must describe exactly 110 rows
of seven 16-bit entries and match the game table pointer. Wire animation values
are row + 1 (1..110); zero clears the pose. No ROM assets ship in the mod.

The receiver looks up its own Kong column, rejects invalid/special clip IDs and
calls `func_global_asm_80613CA8` only to change raw pose. This follows the pose-copy
pattern in the pinned `code_2C910.c`. `func_global_asm_80614644` seeks within the
loaded clip's local frame count; all incoming frames must be finite and 0..254.
`playAnimation`, `playActorAnimation` and the animation-bytecode interpreter are
never called for a proxy. Script pointers/callbacks are cleared, animation speed
is zero, and the normal renderer evaluates the externally selected pose.

The native timeline interpolates forward frame steps over at most 50 ms and
holds the last frame during packet loss. It snaps clip/identity changes, backwards
steps (including loop wraps), large jumps, pauses and gaps over 150 ms. The session
resets this state on disconnect/rejoin. The MIPS adapter limits pose loads/retries
to once per three game-frame callbacks and uses no blend/secondary clips, bounding
cache use to one clip per proxy. Tests cover a full cache without per-frame retries.

This reproduces the main skeletal pose only: secondary tracks, blend weights,
separate held props, animation effects and special poses outside the local table
are not synchronized. Version 0.4 additionally copies the hand/weapon visibility
mask when both peers enable combat. Attack poses alone never apply damage.

## Combat (0.5, experimental)

`mod/combat_actor.h` captures initialized local projectiles only when their parent
is the current local Kong. Six wire kinds map locally to coconut, peanut, grape,
feather, pineapple and thrown orange actors. At most eight snapshots carry a
generation-derived ID, position, yaw and scale. All coordinates/scales are finite
and bounded; the packet cannot supply actor/model IDs, pointers or scripts.

The receiver creates inert actor-21 proxies using the local projectile model table
(`D_global_asm_8074E8B0`, 48-byte rows). Feather uses its local model and fixed raw
pose 0x404; the other five use the engine's model-less initializer. Model zero must
never enter the 3D asset loader. Sprite proxies receive an origin attachment via
`func_global_asm_80665F24` / `func_global_asm_8066E854` and an explicitly updated
translation matrix, then use the game's attached-sprite path. No collision sphere,
projectile behavior, homing logic, player controller, sound or explosion callback
is installed. Actor deletion removes the attached sprite. Active plus retiring
shot slots stay bounded at eight; ownership checks include the live registry,
actor type and generation. A bounded local queue retains the last position of a
locally owned projectile for six capture frames after it leaves the live actor
registry. This makes short-lived fire visible to the 20 Hz sender without inventing
motion, collision or an impact. Snapshots older than 150 ms still hide remote shots.

The original blue/gold beavers and regular Kremlings (actors 178/212/238), plus
the thirty-one kinds documented in the versioned sections below, are eligible
for shared defeat in 58 reviewed vanilla gameplay maps. Every numeric map value
is statically checked against the game's enum. Bosses, bonus games, races, crown
battles and reward controllers remain excluded.
On initialization all thirty-four behavior entries must match the pinned original handlers;
otherwise the mod leaves them untouched and disables defeat sharing. The wrapper
still calls the original handler for every actor update. It records collision
result 9 only from the actual local player or one of that player's projectiles,
and requires the original handler's resulting health <= 0 / death state 0x37.
Scripted disappearance, unloading and unrelated attackers are not defeat events.

Enemy identity is the local spawner index, allowlisted kind and immutable-layout hash, with distinct
local and peer life tokens. The mod assigns nonzero life tokens from a counter
that survives map loads; the game's own actor counter can reset on re-entry.
Transitions clear captured spawn records, including reloads into the same map.
Up to twenty records are sent per page. Every supported live or acknowledgement-pending
record is captured across pages. Both copies must be observed alive, and each must
echo the other's life before a defeat can apply. Bindings are keyed by identity,
not snapshot order. Re-entry, respawn, a changed layout, pause/cutscene, another
save slot, stale state or reconnect cannot bind an old death to a fresh live actor.

A guest reports its actual local defeat as a request. The host returns an apply
command for that exact local spawn, then advertises a commit only after game
readback reports the defeat. A guest accepts commits only from its session host.
Repeated snapshots retry requests/commits; a live/generation guard makes application
idempotent. The game wrapper sets health zero and enters the existing enemy death
path with no attacker pointer; the original local handler owns animation and drops.
No damage amount or arbitrary death-script ID is accepted from the network.
Kremling, Klump and Kasplat state 0x32 skip that death branch in the original code, so commands wait
without touching health until knockback ends. The original handler chooses its
local death animation (0x201 for the Kremling's injected zero-health case).

Combat mode 2 adds host movement; mode 1 shares only shots/defeats. The native
bridge emits guest-only motion commands when both peers choose mode 2 and a live
same-kind pair has exchanged reciprocal life acknowledgments. Guest coordinates
never become host commands. The command carries the receiving actor's life token;
the game checks its current spawner binding, kind, live registry, generation,
health, initialization, save/map context and readiness before applying it.

On the game-frame callback the guest corrects position by 40% toward the newest
host snapshot (snap beyond 500 game units) and adopts yaw. No extrapolation,
remote spawn, control-state assignment, animation script or AI target is applied.
Motion commands expire after 150 ms without host state, independently of the
750 ms presence/defeat timeout. Local physics/AI still runs, so this is position
correction rather than deterministic or fully authoritative enemy simulation.
The same setting must be enabled on both peers; otherwise defeats continue without
movement. The mod never moves the local player or the host's enemies from guest data.

This is eventual agreement on supported defeats, not host-authoritative combat
simulation. Guest hits happen locally before host acceptance and are not rolled
back if the host rejects them. Enemy AI/attack states, partial damage, local pickups,
ammo, player damage, orange effects, switches and all other enemy types stay local.
A late joiner does not receive historical defeats. The gate option is independent.
Both experiments must be enabled before launch and need real two-client gameplay
validation. The recording adapter tests verify engine calls/guards, not the
renderer, collision engine, scene teardown or disk saves.

Pinned references: `code_936B0.c` and the recompiled `func_global_asm_8069346C`
identify gun/orange actors and sprites; `done/code_B1F60.c` contains the beaver death
path. The generated `func_global_asm_806AE588` verifies the Kremling branch (the
commented draft C in `code_B2CE0.c` has inverted conditions and is not authoritative).
`code_7CA80.c` contains actor initialization/deletion; `code_70FD0.c` provides
origin attachments; `code_119370.c` owns sprite creation/deletion; `patches_matrix.c`
tags those sprites for the recompiler renderer. No ROM bytes or assets are packaged.

## Network and native ABI (0.41)

Protocol 41 uses 1200-byte fixed-size packets, explicit big-endian words and
compatibility ID 0x00010129. The established 1192-byte body and all of its
section offsets are unchanged. The first 104 bytes contain presence/gate state;
204 combat words follow. At byte 920, 53 item words carry feature, file,
readiness, host scope, page number, 24 ownership words and 24 request words.
Eight 768-bit pages cover 5894 IDs in 192 in-memory words; unused bits must
be zero. IDs 0..2393 are unchanged; 2394..5893 encode cumulative Troff feeding.
Every round sends all eight pages once, with its starting page rotating backward
by one each round. At 20 Hz this gives a maximum gap of nine sends (450 ms)
per page and moves pages between send phases under periodic packet loss.

Sessions check endpoint, room, nonce, session and increasing packet sequences.
Item caches clear on disconnect/new session, disabled/not-ready state or
file/scope changes. Once assembled, every page must have arrived within the last
2.5 seconds before item application or acknowledgement. Current player state must
still have arrived within 750 ms. Same-page dependencies remain in one page;
all cross-page dependencies require the fully assembled ownership snapshot.
A missing page pauses grants without discarding bound requests.

At byte 1132, a separate 15-word reversible-state section carries feature,
file, host scope, readiness, two observed bits, two desired bits, two revisions,
two request serials, two base revisions, two request-value bits and two
acknowledgements. These fields are never interpreted as item ownership.
Non-state packets require this section disabled/zero. Decoder bounds reject
unknown bits, invalid files/scopes, unpaired revisions and requests without
base revisions. Enabled world state must match an enabled item file; a ready
world snapshot also requires a ready item snapshot.

At byte 1192, two appended words carry the reviewed-transition ticket and packed
source/destination/signed-exit tuple. Both words must be zero together or form
one of the 35 static vanilla routes. Keeping this extension at the end avoids
moving the combat, item or world decoders while staying at the 1200-byte LAN
datagram ceiling.

The game-thread socket is nonblocking, capped at 32 receives/tick and 20 Hz sends.
Presence expires after 750 ms; transport expires after three seconds. The native
bridge `dk64_coop_tick_v44` validates twelve-word presence spans, a **2636-byte**
gate/combat/item/world input and a **3324-byte** combined result before memory
access. The new export rejects old NRM/companion pairs. Native structs are never
serialized directly. No loader changes or background game-memory access are used.
Room/session identifiers are not authentication or encryption; trusted LAN only.

`dk64_coop_local_ipv4` reports the host adapter chosen after a successful bind.
Windows uses `GetAdaptersAddresses`; Android and other Unix targets use
`getifaddrs`. Private Ethernet/WLAN addresses rank above other usable interfaces,
while loopback, link-local, multicast, broadcast, tunnel and cellular candidates
are rejected or deprioritized. Failure returns zero and the HUD shows
`IP UNKNOWN`; it never changes the save campaign or protocol identity.

## Collectible identities

`items_policy.h` keeps the original IDs: 0..39 blueprints, 40..59 fairies,
60..69 crowns, 70..77 keys, 78/79 Nintendo/Rareware coins, 80..119 Snide GBs,
120..159 medals. IDs 160..320 cover the other 161 GBs; 321..2020 are 1700
ordinary persistent pickups; 2021..2140 are 104 balloons and 16 rainbow coins.
No peer chooses a flag number, amount, Kong, level, pointer or save offset.

`tools/generate-collectibles.py` verifies local US ROM metadata and writes only
IDs/counter mappings into `mod/collectible_ids.h`. Sources are the pinned decomp:

- `code_135D30.c`: 113 prop-flag rows (five are Helm medals), 53 barrel rows
  (one unused, two GBs overlap props), and three direct race/minecart rewards.
  Combined with Snide, all 40 Kong/level GB buckets contain five rewards, except
  Tiny Isles' six. Lobby maps count as Isles, not their nominal level.
- `code_36880.c`: 15 persistent model-2 types, counted in setup order within
  ascending maps, with independent per-level ordinals. Counts are
  226/263/245/245/271/194/253/3. The original table at D80753EF0 determines the
  required Kong and HUD item; single/bunch types award one/five bananas.
- `code_8D3E0.c`: actor setup types 0x5B/0x6F/0x70/0x71/0x72/0x8B consume a
  global flag ordinal starting at 0x26B. The test-map balloon consumes an ordinal
  but is excluded from the allowlist. The 120 adventure rewards retain the
  original flag ordering. `done/code_AAAF0.c` verifies balloons' flag/+10 path;
  rainbow coins use the flagged dropped-actor/prop path and `code_F9450.c`'s
  five-Kong +5 award. Setup CB totals also cross-check color assignments.

No ROM assets or setup geometry are shipped. Chunky's Japes boulder actor bunch
is outside both generated identity tables, so v0.46 appends its existing
permanent flag `0x01D` as one explicit stable ID. Three surplus single-banana
setup entries are included as identities but are not automatically granted:
only observed pickups or host-owned bits can be shared. Vanilla counter caps
still apply. There is no blanket flag OR or save-buffer transfer.

## Capture, authority and counter application

The initialization callback selects existing `items_host_v6`/`items_guest_v6`
files before the EEPROM worker starts. Normal saves and older experiment files
are untouched. When Host or Join is configured, six menu-function replacements
render and navigate only physical file 1 plus Delete; the delete submenu can
target only file 1. Role Off retains the original three-file menus. The existing
runtime file locks remain as a defensive check if another mod or engine path
changes the adventure file unexpectedly. Expanding the supported IDs can expose
old guest-ahead saves; no automatic merge/reset occurs.

`menu_slot_policy.h` keeps the normal four-entry file-menu mapping (files 1/2,
Delete, file 3) and three-entry delete mapping separate from the co-op two-entry
and one-entry layouts. Co-op always maps its one visible file to physical index
0; campaign, role and save namespace provide the actual isolation.

The existing pre-write flag callback observes local unique items. Frame capture
also observes rising bits after establishing a baseline, covering ordinary
pickups (which do not emit permanent-flag events) and flags written while queued
pickup animations suspended readiness. The previous bitmap is updated for remote
grants so these cannot echo as local events. Bound requests survive temporary
waiting; new sessions/scopes and conflicts clear binding. Unbound/pre-existing
guest items absent from the host cause conflict rather than import.

Capture waits through cutscenes/loading and requires a main-world/Isles map or DK's house;
boss/minigame overlays can temporarily replace upgrades or melon capacity. Capture/application also wait while
D807FD730 has pending collectible credits: ordinary
pickup save bits can precede inventory increments. GB counts must exactly match
all their allowlisted flags; Snide also requires its blueprint. Ordinary setup
counts must match the pinned table. Coins must be at most 999; unspent plus fed
bananas at most 100. These are compatibility guards, not a layout hash: mods that
reorder same-size setups, change rewards or rewrite inventory are unsupported.

The host applies new guest requests; the guest applies host-owned bits. A grant
requires actual game write/readback, then adds exactly the locally mapped amount
to the local counter. Already-owned items never grant twice. Canceled writes do
not credit or acknowledge. A counter changed unexpectedly by another mod latches
COUNTER CONFLICT instead of adding another reward. GB layout is pinned to
CharacterProgress stride 0x5E/GB offset 0x42, PlayerProgress stride 0x306.

Incoming numeric rewards apply only in ordinary main-world/Isles play, with a
loaded HUD, **outside the reward's level**. Snide remains safe in main-world play
outside HQ because its menu checks each reward flag before incrementing. Other
rewards wait to avoid a loaded old prop, balloon, dirt patch or race script
awarding again. This avoids live prop deletion or engine hooks. On re-entry the
normal spawner/setup code checks the updated collection bit. Race/script and
world refresh behavior still require real-game testing.

Ordinary pickup bits use `saveFile.c::func_8060E3B0`/`func_8060E430` with a validated
per-level ordinal; they are not permanent flag indices. A new coin credits its
Kong's current local balance, preserving purchases. Bananas credit the unspent
bucket without changing Troff & Scoff's fed bucket. Balloons add ten. Rainbow
coins preflight all five balances before setting the flag and then add five to
each Kong. Shop transactions are not synchronized. Version 0.16 adds cumulative Troff
feeding with matching available-banana deductions; see its section below.

A remote CB/bunch/balloon crossing 75 sets the correct normal level medal flag.
A newly derived guest medal is requested separately, since local spending or
unsupported drops may differ. Medals alone never grant bananas. GB HUD totals
refresh once per batch; coin/CB HUD pointers read their own inventory. Regular
play batches incoming changes through `func_8060DEC8`. SYNCED confirms ownership
readback, not equal balances or durable EEPROM/disk completion.

Tests compile the actual C adapter against a recording engine and sentinel-filled
production inventory layout. They exercise every supported reward, both local-first
and remote-first paths, deferred loaded levels, pending queues, canceled writes,
medal derivation, spending/fed preservation, capacity errors, source/binding guards,
loss/replay, missing pages and reconnection. The native peer probe is synthetic:
no game or ROM is loaded. Real pickup scripts, graphics, performance, map re-entry
and save/reload remain unverified for these new features.

## Kong, shop and K. Lumsy progression (0.9)

Append-only IDs 2141..2154 represent Diddy/Tiny/Lanky/Chunky (flags
0x6/0x42/0x46/0x75), initial Japes lobby opening (0x1BB), key turn-ins
(0x1BC..0x1C3), and final K. Lumsy release (0x315). IDs 2155..2190 encode
15 Kong-specific move tiers, slam 2/3, belt 1/2, five base guns, two global
gun bits, five base instruments, three global instrument bits, and melons 2/3.
There is no arbitrary flag, byte offset or upgrade value supplied by a peer.

`menu/code_1AF0.c::func_menu_80025E04` is the source for stock purchase rules:
CharacterProgress bytes 0/1/3 are scalar move/slam/belt tiers; bytes 2/4 are
weapon/instrument bitfields. Base guns/instruments target one Kong. Higher bits
and slam/belt upgrades target all five, including locked Kongs. Validation
requires legal bounds (3/3/7/2/15), equal global fields/bits, and melons <=3.
Tier ownership includes all lower tiers; remote grants update their previous
bits together to avoid guest echo. Local purchases are discovered on main-world
return, including after a shop temporarily suspended network readiness.

The adapter raises only these allowlisted fields. PlayerProgress melon offset
0x2FC, Kong stride 0x5E and player stride 0x306 are compile-time checked. Shop
functions are not called remotely: no coin deduction or ammo/health refill.
`func_80709464` fills health rather than merely refreshing a HUD, so it is not
called. Current health, all resource counts, unknown bytes, unused Kong/level
slots and other players are preserved. Save/reload uses the existing inventory
serialization in `saveFile.c`; actual game disk persistence remains unverified.

Upgrades wait for the recipient's flag 0x180 (Cranky's first slam, local or shared). The training
gift in `menu/code_1AF0.c` writes slam=1 to all Kongs; applying an upgrade before
that gift would risk a downgrade. Version 0.10 shares the first slam, training and camera as described below. Rescue grants wait outside Japes/Aztec/Factory as appropriate.
K. Lumsy grants wait outside Isles, require key ownership plus the initial
lobby opening, and require all turn-ins before final release. These rules follow
`code_C1E70.c`, `code_135D30.c` and the pinned enums/prop scripts. They do not
force cutscenes, reopen loaded cages or modify arbitrary puzzle/door flags.
The initial Japes opening cannot bootstrap a fresh recipient out of locked
Isles under this deferral rule: both players must make that first K. Lumsy
visit locally. This is an explicit prototype limitation, not automatic catch-up.

Only host ownership or bound guest requests can advance these IDs. Existing
guest progression absent from the host still causes SAVE AHEAD. Isolated v6
save names are retained. Protocol/compatibility/export v14 prevents pairing
with v13 or earlier despite unchanged 1196-byte packets and native buffer sizes.

`tests/progression_checks.h` runs the production adapter: all 50 new IDs,
canceled writes, level/Isles deferral, training/HUD/queue guards, global and
individual upgrades, implied tiers, full-inventory sentinels, invalid state,
missing key prerequisites and offline guest progression. The actual Windows
and tablet game scenes and move activation still require gameplay testing.

## Training and camera/shockwave (0.10)

IDs 2191..2198 append eight flags: training spawned (0x17F), four course/ability
flags (0x182..0x185), all-training-complete (0x187), first slam (0x180), and the
combined camera/shockwave flag (0x179). All prerequisites fit on page 2. Course
flags require spawned; completion and first slam require all four courses.
First-slam ownership also requires the five Kong inventories to have slam >=1.

Source: `critter/code_5110.c` awards the course flag on a completed barrel and
restores local oranges; `global_asm/code_80150.c::func_80681BD8` checks flags on
training-barrel updates and derives completion on a local exit. Co-op can combine
courses without a local final barrel exit, so it derives completion from the
four actual skill flags and spawned flag, only in a safe bound session. The guest
requests this newly derived bit; remote echoes remain suppressed. Cranky's first
gift is shared only after actual host ownership or a bound guest request.

First slam follows `menu/code_1AF0.c`: after flag write/readback, raise each of the
five Kong slam bytes to at least 1. A canceled flag cannot grant inventory; later
duplicates cannot downgrade existing tiers. No purchase routine, cutscene, map
transition, ammo refill or health heal runs remotely. Higher shop upgrades wait
until this gift has been applied. Invalid flag-only inventory fails closed.

`critter/code_3340.c::func_critter_80027DC0` initializes the fairy queen from the
combined flag, then grants it in her proximity scene and later refills film.
Only the ownership flag is shared. `code_450.c` calls `func_806F6F28` each regular
frame to refresh player capability bits; no cached-player struct patch is needed.
Film/crystals/oranges remain local, with no remote replenishment.

Map 171 (DK's house) joins the snapshot/progression allowlist. It lets a fresh
guest receive training before opening the starting area's exit. Maps 176
(training grounds), 177/180/181/182 (courses), 189 (Fairy Island), shops, other
interiors and minigames remain excluded. This avoids granting while their reward
actors or scripts are loaded. Leave those areas, receive in the house/Isles/a
main world, then re-enter to rebuild actors from flags. Numeric collectible
credits retain the original main-world-only condition, including their level
deferral; the treehouse does not bypass it. Initial K. Lumsy visit stays local.

`tests/training_checks.h` uses the production adapter for all eight flags, canceled
writes, all 216 map IDs, first-slam and higher-slam ordering, split course sets,
queued/scene pickups, guest requests, prerequisite validation and full-inventory
sentinels. Live gameplay and disk persistence must be reported separately from
these recording-engine tests.

Live 0.10 testing connected an actual Android host and Windows 1.0.1 guest,
both showing the remote Kong and ITEMS: SYNCED in map 171. Only the tablet
visited Cranky; after it returned, Windows received flag 0x17F and the regular
save path wrote it to the isolated guest EEPROM. A read-only parser based on
`saveFile.c` and `code_135D30.c` verified the flag changed from 0 to 1 and all
four 428-byte slot CRC32 values remained valid. The same session also transferred
and saved diving (0x182) after the tablet completed its course; the Windows
guest remained in the treehouse. No flags were injected or save files patched.
Both games were restarted and returned to SYNCED, with both training flags
still present in a CRC-valid Windows save. Android's activity log confirms a
new game process. This limited training save/reconnect smoke test does not
establish offline reload behavior, course/move activation or broader inventory
persistence. Evidence: `build/verification-v0.10.0/live-gameplay.json`.

The gate relay test now allows five simulated seconds for reconnect. Its
one-in-three loss can discard BYE, so the old peer can legitimately remain
bound for the three-second timeout before HELLO retry/readback. The previous
1.2-second assertion occasionally failed. No production timeout was changed.

## Bananaport tags and B. Locker clearance (0.11, untested)

IDs 2199..2285 are 87 vanilla Bananaport tag flags, ordered by map then setup
object ID. IDs 2286..2293 map to B. Locker clearances 0x1CD..0x1D4. Total 2294
shared IDs; three 1024-bit pages and 1196-byte packets remain unchanged.
Native input/result sizes remain 1640/2200. The new v11 export, protocol and
compatibility token prevent an older companion or peer from interpreting the
expanded allowlist. Existing item save namespaces and the loader are unchanged.

`tools/generate-travel.py` reads setup table 9 and instance-script table 10
from the user's decompressed vanilla US ROM. It emits only `mod/travel_ids.h`
metadata. Script condition 0x2D reads permanent flags; execution 0x6B writes
them (`global_asm/code_42630.c`). Tutorial flag 0x163 is deliberately excluded.
The ten adventure warp maps contain 90 pads, 87 distinct tag writes and three
pads whose initialization/use is controlled by existing GB flags. Their flags
are 0x3E (Aztec), 0xA3 (Galleon), 0x127 (Caves); no randomizer-only substitute
flags are used and their numeric GB credit stays on the original path.

Japes map 7/object 0x12B writes tag 0x27 but requires GB 0x17 (item 175) to be
revealed. The complete-page validator checks that dependency across pages,
and game-side application checks the GB flag again. Other pads' counterpart
GB checks do not become prerequisites for tagging the local pad. Each tag
remains independent: no paired-tag grant is derived.

Only safe snapshot maps capture/apply, as before. Pad delivery additionally
waits outside the exact pad map; entering it later initializes from the saved
flag without remotely changing loaded scripts or triggering warp/cutscene code.
Llama Temple and Castle crypt tags are captured on returning to a safe map.
No inventory byte is written by travel grants. Existing flag readback, save
requests, guest request retries and save-ahead rejection are reused.

`critter/code_3340.c::func_critter_80027448` sets a B. Locker clearance after
the vanilla GB check. Its initialization enters the cleared state when the
flag is already present. All lobbies are outside the safe-map allowlist, so
remote clearances are applied only while the actor is unloaded. No threshold,
GB counter, Troff feeding, local tutorial, outer lobby entrance or transition
is changed. Day/night and Galleon water height are intentionally excluded:
they are reversible state and cannot use monotonic ownership sharing.

No tests were run or new gameplay claims made for these paths. Metadata
generation includes structural guards and the build/package commands retain
their normal compile-time/packaging checks; these are not gameplay validation.

## Retired first-gate experiment

Version 0.41 removes the standalone configuration and save selection for the old
Japes-only gate prototype. Item/world sharing owns permanent flag 0 along with the
other reviewed progression. The fixed gate words remain reserved in the packet
and native bridge spans to avoid shifting established combat/item/world offsets,
but the NRM sends them as zero and the bridge ignores nonzero input and returns
zero. Therefore an old config or `gate_host_v2` / `gate_guest_v2` file cannot
reactivate the experiment. The underlying policy regression test remains only to
guard the reserved wire layout.

## Remaining gameplay work

Two-game pickup, level reload and disk-save validation is required. Enemy
AI/attack states, other enemies/bosses, local damage/ammo, doors/puzzles outside the allowlist,
shop/Troff transactions, coordinated
transitions and the Japes actor bunch remain independent. This is not full
campaign co-op or randomizer/Archipelago integration.

## Permanent world unlocks (0.12, untested)

Append-only IDs 2294..2303 in `mod/world_ids.h` add ten flags. Total 2304 IDs
(2141 collectibles and 163 progression IDs). Existing IDs, the 1196-byte
packet, three 1024-bit pages, and 1640/2200-byte native input/result sizes
are unchanged. Export `dk64_coop_tick_v12`, protocol 12 and compatibility
0x0001010C reject earlier companions/peers. Loader and isolated v6 saves are
unchanged. No engine functions for puzzles, cutscenes or item awards are called.

| IDs | Permanent flags | Completion | Excluded receiving level |
| --- | --- | --- | --- |
| 2294 | 0x32 | Llama freed | Aztec (1) |
| 2295 | 0x4C | Llama Temple water cooled | Aztec (1) |
| 2296 | 0x9E | Seal freed | Galleon (3) |
| 2297..2301 | 0xE6..0xEA | Giant Mushroom coconut/grape/feather/peanut/pineapple switches | Fungi (4) |
| 2302 | 0xF8 | Rabbit first race complete | Fungi (4) |
| 2303 | 0xFB | Beanstalk grown | Fungi (4) |

The enum values are compile-time guarded against the pinned decomp. Source
inspection used that decomp and the user's vanilla US instance-script table,
not randomizer-modified scripts. Only flag/map metadata is distributed:

- `global_asm/code_135D30.c` ties Aztec/Llama Temple spawns to flag 0x32.
  Vanilla map 38/object 0 writes 0x32; objects 0xD..0xF and 0x26 read it.
  Map 20/object 0x16 reads both 0x32 and 0x4C on initialization and writes
  0x4C after its sequence. Object 0x18 also initializes from 0x4C.
  `code_C2A90.c::func_global_asm_806C1E44` initializes the llama from 0x4C;
  `code_6710.c` uses it for the temple water effect. The complete-ownership
  validator and grant adapter require 0x32 before accepting/applying 0x4C.
- `code_C2A90.c::func_global_asm_806C226C` sets 0x9E when the seal is freed
  and initializes seal actors from it. Vanilla map 30/object 0x38 and map
  54/object 0 also read it; leaving all of Galleon unloads the related state.
- Vanilla map 64/objects 0xD, 0xE, 0xF, 0x10, 0xC each set/read one of the
  five mushroom switches. `propScripts.c::func_global_asm_8064EAB4` derives
  cannon progression from their sum. No sixth completion flag is invented.
- `code_C2A90.c::func_global_asm_806BE8BC` reads 0xF8 to select the second
  rabbit race and sets it on victory. The local first-race refill drops
  are not replayed or credited by this mod.
- `done/code_A6280.c::func_global_asm_806A1AC4` sets 0xFB during the local
  bean sequence, and initializes directly to the grown state when already
  set. Vanilla maps 48/object 0x4D and 52/object 5 read it on initialization.
  Only the grown state is shared, not the temporary carried bean.

The existing safe-map/HUD/cutscene/pending-award gates still apply. World
flags additionally defer anywhere in their owning level; receiving outside
it lets the next load initialize naturally. Existing host ownership/bound
requests, flag readback, rising-bit recovery, save requests, request retry
and SAVE AHEAD handling apply to these IDs. Remote writes update previous
ownership to prevent echo requests. No balances, live actors, timers, temporary
flags, route requirements, GB IDs or inventory bytes are changed by this path.
Day/night (0xCE), Galleon water height (0xA0), local spending, and other untraced
puzzle/door/boss state remain excluded.

The mod and both companions were built and packaged only. Automated tests,
network probes, gameplay and save/reload validation were skipped as requested;
source inspection and compilation do not establish runtime correctness.
Build record: `build/build-v0.12.0/build-status.json`. Nothing was installed
over the running test games or published by this step.

## Factory permanent unlocks (0.13, untested)

Append-only world rows 10..17 add IDs 2304..2311. Total 2312 IDs: 2141
collectibles and 171 progression IDs. `world_ids.h` supplies only local,
fixed flag identities; no arbitrary flag, object pointer, script state or
counter address comes from the peer. All eight entries exclude level 2 via
the existing world adapter, including Factory's related interiors. Earlier
IDs, inventory logic, save namespaces and the mod loader remain unchanged.
The wire remains three 1024-bit pages in 1196-byte packets, with 1640/2200-byte
native input/result spans. Matching protocol/export v13 and compatibility
0x0001010D are required; the earlier v12 contract does not include these IDs.

The following is metadata from the user's vanilla US setup/script tables
(map 26), not replacement randomizer scripts. Flags are unnamed in the
pinned decomp enums, so their source is the explicit script identities below.
Condition 0x2D reads permanent flags; execution 0x6B writes them, as decoded
in `global_asm/code_42630.c`.

| Item ID | Flag | Writer object | Permanent change |
| --- | --- | --- | --- |
| 2304 | 0x6D | 0x17 | Entry hatch switch completed |
| 2305 | 0x6E | 0x16 | Testing-area gate opened |
| 2306 | 0x6F | 0x00 | Production core activated |
| 2307 | 0x77 | 0x13C | Storage breakable metal grate removed |
| 2308 | 0x78 | 0x62 | Storage question-mark box completed |
| 2309 | 0x81 | 0x2D | Arcade lever revealed |
| 2310 | 0x85 | 0x3C | R&D breakable metal grate removed |
| 2311 | 0x97 | 0x4C | Crown-pad grate opened |

Each writer also reads its completion flag during initialization. Inspection
of all 216 map script entries found only set-to-one writes for these eight
flags, and their readers are confined to Factory, Power Hut (29) and Factory
Baboon Blast (110). No existing collectible, travel or world ID is reused.
The following reload behavior is why grants wait outside the whole level:

- Hatch objects 0x10/0x1D and switch 0x17 read 0x6D; switch/gate initialization
  selects the already-completed path. Gate 0x16 and neutral switch 0xB0 do
  the same for 0x6E.
- Core object 0 reads 0x6F, starts its local machinery, and skips its startup
  cutscene when the flag is already set. Factory object 0x107 and Power Hut
  objects 0/2 read it too. The mod does not synchronize rotation, movement
  phase or clocks and does not run the local activation sequence remotely.
- Breakable grate objects 0x13C/0x3C initialize hidden/noncolliding from flags
  0x77/0x85. Object 0x62 does likewise for 0x78; objects 0x61/0x77 read that
  flag as well. These are persistent completions, not every live puzzle action.
- Lever 0x2D initializes to its available state from 0x81 without repeating
  its reveal scene. Baboon Blast object 0 also reads that flag. Arcade GB
  0x82, two-coin payment 0x83 and Nintendo Coin 0x84 are distinct. The later
  append-only payment ID is documented under 0.46; this original world row
  neither charges coins nor changes temporary Arcade state.
- Grate 0x4C initializes hidden/noncolliding from 0x97; objects 0x4E/0x106
  also read it. Crown ownership remains the separate collectible ID.

All existing snapshot safety gates, host ownership/bound guest requests,
flag readback, request retries, save requests and guest-save-ahead checks
are reused. A peer already inside Factory waits before receiving new flags;
its live puzzles are not patched. The four temporary Kong production-switch
states, other transient puzzles, prices/payments and machine timing are not
added to monotonic ownership. No new prerequisite is inferred from a route
or a Kong ability: only the existing world-table prerequisite (cooled Llama
Temple requires rescued llama) remains.

Builds/package generation only; automated tests, LAN probes and gameplay were
skipped as requested. No runtime or save/reload validation is claimed for
these paths. Record: `build/build-v0.13.0/build-status.json`. This step did not
install over running games or publish a release.

## Complete permanent-unlock pass (0.14, untested)

`tools/generate-world-expansion.py` scans all 216 vanilla US instance-script
maps and emits `world_expansion.inc` plus [WORLD-UNLOCKS.md](WORLD-UNLOCKS.md).
The generator uses an explicit reviewed selection, not a blanket OR of flags.
It checks every selected flag has a persistent initializer, only set-to-one
script writes, and no overlap with prior IDs. Every other script-written flag
must already be shared or appear in the explicit exclusions. New unknown flags
fail generation rather than becoming network grants. This is source-metadata
generation, not automated or gameplay testing. No bytecode/assets are packaged.

The 18 world rows released in 0.13 remain fixed. Rows 18..99 add IDs 2312..2393
(82 flags), bringing the total to 2394. The existing 87 pad-tag IDs are unchanged;
four additional Helm/lobby tags use new world IDs, giving 91 saved tags and the
same three automatic GB-controlled pads. Packets remain 1196 bytes with three
1024-bit pages. Native input/result sizes remain 1640/2200. Protocol 14,
compatibility 0x0001010E and `dk64_coop_tick_v14` reject older peers/companions.
The loader and isolated v6 item saves are unchanged.

Game-side world grants retain safe-map/HUD/queue/cutscene gates and level
deferral. Level 255 is reserved for interior-only rows whose readers are outside
the caller's safe-map allowlist: training exit, fairy room, permanent lobby
puzzles and Helm-lobby pads can receive in safe maps including the treehouse.
Level 8 represents Helm, also excluded from snapshot/application maps. Other
world rows retain full owning-level deferral, including their related interiors.
The generator checks interior-only rows against the same safe-map identities.

`CoopWorldRequirement` is a local table of flag, first item ID, range length and
minimum owned count. Both full assembled network ownership and game-side flag
readback check it. Cross-page dependencies are never checked on isolated pages:
seven closed-boss-portal flags require their keys; Rareware room requires 20
fairies plus camera/shockwave; coin door requires both special coins; crown door
requires any 4 of the 10 crowns (`propScripts.c::func_global_asm_8064F404`).
`critter/code_3340.c` supplies the fairy-queen requirements. Mill stage 0xD5
requires 0xD4 via the existing world-row prerequisite; temple cooling retains
its llama prerequisite. These gates consume nothing and do not derive new
completion flags from requirements. Local game completion or a bound guest
request remains necessary. The five pearls are individual flags; the mermaid
in `code_C2A90.c` counts them, without a copied numeric pearl balance.

Flag 0 now has one authority path. The item callback observes ordinary local gate
writes, and the retired gate protocol cannot cancel, apply or acknowledge the
same flag. With item sharing Off, the gate remains local. No configuration schema
or mod-loader changes are required.

Persistent unlocks are distinct from live encounter/door state: Helm shutdown
is shared after leaving Helm, not its timer/minigame phases. Saved encounter
clears initialize naturally on return; no AI, HP, actor spawns, reward routines
or cutscenes are directly invoked by the grant code. Existing numeric rewards
retain original IDs, deduplication and deferral rules. Day/night (0xCE), Galleon
water height (0xA0), Caves-lobby pressure switch (0x19D), temporary/timed puzzles,
tutorial/first-use bits, level visits and ending global flags are excluded.
Some doors only have temporary state or an already-shared GB condition; those
cannot be turned into new monotonic unlock flags. This is not full campaign sync.

MIPS mod and Android/Windows companions were compiled and packaged only;
no tests, network probes, gameplay sessions or save/reload validation were run.
Build record: `build/build-v0.14.0/build-status.json`. No running installation,
normal save, repository remote or GitHub release was changed by this pass.

## Reversible Fungi/Galleon state (0.15, untested)

Source: `mod/world_types.h`, `mod/world_game.h`, `native/world.hpp/.cpp`.
This extends the item-sharing option without allocating new monotonic IDs.
Both normal save namespaces and isolated item v6 namespaces are unchanged.

Vanilla US setup scripts were inspected directly from the local user ROM:

| Bit | Flag | State | Vanilla writers | Receiver exclusion |
| --- | --- | --- | --- | --- |
| 0 | 0xA0 | Galleon water raised | Map 30 object 0 writes 1, object 1 writes 0 | Level 3 |
| 1 | 0xCE | Fungi nighttime | Map 48 object 4 writes 1, object 5 writes 0 | Level 4 |

Each of those four scripts reads its flag during initialization. No randomizer
replacement script is used as evidence of vanilla behavior. The decomp's
`global_asm/code_CC800.c` also reads nighttime into player state on creation,
and `func_global_asm_806C9658` adjusts Galleon exit heights when exits load.
Changing flags remotely while those levels are loaded would leave parts of
live state stale; the adapter deliberately waits outside the entire level.
No engine water/lighting routines, cutscenes or first-use bookkeeping are invoked.

The mod captures actual flag differences after the existing safe-map,
cutscene, inventory-consistency and pending-credit guards permit a snapshot.
Local per-bit change serials distinguish intentional changes from an unapplied
host target. Remote writes are read back and update the local baseline, preventing
echo. Cancelled writes remain pending; successful writes use `items.save_pending`
and the existing normal engine save-request routine in the isolated namespace.
The two boolean allowlisted flags are the only writable addresses in this path.

The bridge binds state to the transport session, local file, peer file and host
scope, with a game-side echo of the session/scope before use. Initialization uses
the host's actual flags; old/offline guest values do not create requests. The
host maintains a revision per bit and processes verified local intent before
incoming requests. A guest request contains a monotonically advancing serial,
its desired bit and the host revision the guest had observed. The host accepts
only an exact base-revision match, increments the revision, and acknowledges
both accepted and stale/rejected requests. Retransmission cannot replay an
already acknowledged change. Guest requests repeat until their exact serial is
acknowledged; while pending, that bit is not overwritten speculatively.
Rapid local changes replace a pending desired state, not an event-log history.

Host packets alone supply authority and acknowledgements; guest packets alone
supply requests. Revision comparisons are wrap-aware and skip zero. Local intent
is processed before the newest received packet, so a concurrent host change
cannot accidentally legitimize a request created against older authority.
An older host revision cannot roll back the guest's cached target. A lost request
or acknowledgement is repeated by later snapshots. Readiness gaps preserve
requests and revisions; they never reset authority to revision one in an active
binding. Both fresh, non-conflicting item inventories are required to process
world changes; stale or missing item pages also pause this channel. Session
expiry/reconnect discards its in-memory requests and unapplied authority and
starts from the host's actual flags. There is no durable intent journal.

A separate WORLD HUD says synced only after both ready observed snapshots and
revisions agree, with no outstanding local request. Save readback is not proof
that the asynchronous save worker has finished writing to storage. Remote
application waits outside Galleon/Fungi; returning to Isles is the simple route.
The request origin keeps its normal local switch behavior and cutscene.

Protocol 15 keeps packet size below 1200 bytes by reducing item page width from
32 to 24 words and increasing page count from three to four. All item IDs,
in-memory ownership sizes and same-page dependency groupings remain unchanged.
The combined native input adds 36 bytes and result adds 28 bytes. Existing ABI
and packet fixtures were adjusted for these layouts, but no test suites, network
probes or gameplay sessions were run. This version was compiled and packaged
only. Build record: `build/build-v0.15.0/build-status.json`.

## Cumulative Troff & Scoff feeding (0.16, untested)

Source: `mod/troff_ids.h`, `mod/troff_game.h`, `mod/items_policy.h` and
`mod/items_game.h`. This shares settled feeding totals, not actor animation,
room script state or network-supplied damage/payment amounts.

For each of seven levels and five Kongs, amount `n` owns milestones 1 through
`n` (up to 100). ID = `2394 + (level * 5 + kong) * 100 + amount - 1`.
The 3500 new IDs end at 5893. A bitwise union is the maximum of the two cumulative
fed counters, so duplicate feeding of copied inventory cannot count twice.
Amounts fed by different Kongs represent different balances and add normally;
concurrent payments can exceed the door requirement and are not refunded.
No Isles, Helm, unused inventory level or sixth Kong slot is addressed.

Milestones reuse the existing host authority, session/scope binding, guest
rising-bit requests, save-ahead policy, packet retransmission and readback
acknowledgement. Capture only runs in the existing safe maps after cutscenes and
pending reward credits. A contiguous prefix is required per bucket in fully
assembled snapshots; it is not checked on isolated pages because prefixes cross
page boundaries. Holes defer the entire snapshot. No network flag number,
address or arbitrary amount is accepted. `coop_item_flag` returns -1 for these
IDs and flag observation searches only the earlier flag-backed range.

Incoming feeding runs after pickup credits and permanent grants. For a milestone
`fed + 1`, the adapter requires a bound ready session, safe main-world gameplay,
a HUD, no reward queue, and a location outside the affected level. It verifies
all five Kong balances for that level satisfy `available + fed <= 100` and waits
if the selected Kong has no available banana or an earlier milestone is missing.
It deducts one available banana, increments the fed bucket and updates the level
aggregate. Duplicate/already-owned milestones are skipped. Readback updates the
previous-ownership baseline to prevent guest echo and requests the existing
isolated-save writer. There is no refill, medal grant, forced boss door or
transition. Missing pickup credits keep feeding pending instead of inventing
currency; the explicitly mapped Japes boulder bunch closes the former five-
banana gap for Chunky.

Pinned decomp evidence:

- `global_asm/code_C2A90.c:func_global_asm_806BE09C` calls
  `changeCollectableCount(0, 0, -1)`, increments the active Kong's fed counter
  and sums the five Kong counters to stop local feeding at the requirement.
- `global_asm/hud.c:changeCollectableCount` updates the available counter;
  the medal check applies only to positive collected-banana credits.
- `global_asm/code_11BE00.c:func_global_asm_8071D0F0` increments
  `D_global_asm_807FC930[level]` after the flying-banana effect completes, later
  than the fed-counter write. Remote application runs outside the whole level
  and reconstructs this aggregate from actual fed counters. A lagging aggregate
  is allowed; an aggregate greater than the counters fails closed as a conflict.
- `global_asm/code_3C10.c:func_global_asm_805FF0C8` compares that aggregate with
  the boss-door requirement. `code_9DD70.c:func_global_asm_8069DD40` reads it at
  room-actor initialization and uses the normal door scripts when funded.
- `global_asm/saveFile.c:func_global_asm_8060DC3C` loads available/fed counters
  separately and reconstructs the aggregate; `func_global_asm_8060DEC8` saves
  both fields. `DK64Syms/data_dump.toml` exposes the pinned aggregate symbol.

Packet size stays 1192 bytes, with the world section unchanged. Item arrays grow
from 96 to 192 words; input/result structures grow to 1560/1552 bytes. Combined
native input/result sizes are 2444/2996 bytes. Protocol 16, compatibility
0x00010110 and `dk64_coop_tick_v16` reject old peers/companions. The backward page
rotation preserves 20 Hz traffic and the existing 750 ms freshness threshold;
eight pages are required before items or reversible world state can proceed.
Existing fixture spans, reserved-bit locations and non-flag ID handling were
updated, but no suites or probes were executed. Build/package only; no gameplay,
installation, commit, push or publication. Record: `build/build-v0.16.0/build-status.json`.


## Expanded enemy allowlist (0.17, untested)

The normal combat modes now accept eleven stable wire kinds. No packet fields,
record capacities, collectible IDs, save namespaces or loader behavior change.
Protocol 17, compatibility `0x00010111` and `dk64_coop_tick_v17` reject older
companions/peers despite the unchanged 1192-byte packet and 2444/2996-byte native
spans. Native validation rejects kind zero and values above eleven.

| Wire kind | Enemy | Local actor | Pinned original handler |
| --- | --- | --- | --- |
| 1 | Blue beaver | 178 | 806AD54C |
| 2 | Gold beaver | 212 | 806AD7AC |
| 3 | Regular Kremling | 238 | 806AE588 |
| 4 | Klump | 187 | 806AEE84 |
| 5 | Mushroom Man | 224 | 806B0354 |
| 6 | Robo-Kremling | 235 | 806B91E0 |
| 7 | Kosha | 291 | 806B0848 |
| 8 | Zinger 0 | 183 | 806B486C |
| 9 | Zinger 1 | 206 | 806B48B8 |
| 10 | Robo-Zinger | 261 | 806B4904 |
| 11 | Bat | 285 | 806B513C |

The table was inspected in the locally decompressed US ROM at the symbol
`D_global_asm_8074C0A0`, using the ROM/VRAM mapping in `DK64Syms/data_dump.toml`.
Actor enum values are also pinned by compile-time assertions. The mod stores
these pairs in one local descriptor table ordered by wire kind, derives kind
lookup from that table and checks every original handler before replacing any.
Another mod's modified entry disables all enemy hooks. This check is performed
at initialization; later mods replacing handlers remain an unverified conflict.

The eight additions all reach the ordinary death state through
`func_global_asm_8072D714`. Its generated code in `RecompiledFuncs/funcs_13.c`
explicitly supports a null attacker and calls `func_global_asm_8072B59C` with
state 0x37. Local animation IDs remain chosen by each original handler:
Klump 0x2AE, Mushroom Man 0x37A, Robo-Kremling 0x327, Kosha 0x367, Zingers 0x251,
Robo-Zinger 0x254 and bat 0x30E. These IDs never enter the network contract.

Klump's authoritative generated handler in `RecompiledFuncs/funcs_61.c` skips
normal collision-death handling during state 0x32; its remote command is deferred
without clearing health, as for Kremlings. Kosha's generated handler in
`RecompiledFuncs/funcs_45.c` does not have that same exclusion. The commented draft
bodies in `code_B2CE0.c` are not used as authoritative branch conditions.
Mushroom Man's compiled C in `code_B2CE0.c` and Robo-Kremling's in `code_BDEE0.c`
use the standard collision result 9 death path. `code_B7490.c` defines the shared
`func_global_asm_806B42A8` path used by both Zingers, Robo-Zinger and bat; the bat
skips the Zinger bounce during its own local death progression. Their local
initialization, animation, flight, death physics and drop behavior still run.

Capture still requires an initialized spawner-bound actor and a real local-player
or owned-projectile hit, followed by health <= 0 and state 0x37. Remote application
requires the exact reciprocal life binding and safe context; the wrapper then
injects zero health/collision result 9 with a null attacker and observes the
original handler's result. Disappearance, culling and script kills do not become
network defeat events. Movement uses the existing guest-only bounded correction;
flight AI, animation, attack timing, damage before death and drops stay local.

The limit is still sixteen records in spawner order, including confirmed deaths
retained until respawn/map reset. Later eligible enemies can be starved of a record
even after early enemies die. There is no unacknowledged tombstone eviction or
rotating partial frame: either would need additional binding/acknowledgment work.
All seven main-map restrictions remain; interior and boss encounters are excluded.
Klaptraps are excluded because the null-attacker path can change their secondary
skeleton phase; Kaboom/Klobber explosion/removal paths and the Toy Monster-linked
dice/domino branches also need separate adapters. Other kinds remain unsupported.

Only the MIPS mod and Android/Windows native companions were built and packaged.
Existing test fixtures were updated for the enum/export/table changes but not
compiled or run. New mock behavior stubs are link fixtures, not simulations or
validation of these enemies. No suites, LAN probes, gameplay, device installation,
commit, push or release publication. Record: `build/build-v0.17.0/build-status.json`.
No ROM bytes, disassembled game functions or game assets are distributed.


## Kasplat combat allowlist (0.18, untested)

Wire kinds 12 through 16 are DK, Diddy, Lanky, Tiny and Chunky Kasplats,
local actors 241 through 245. Their pinned behavior handlers are respectively
`806B1EA8`, `806B1EC8`, `806B1EE8`, `806B1F08` and `806B1F28`. The common
`func_global_asm_806B1DD4` selects the local blueprint association before calling
`func_global_asm_806B13B4`; no blueprint identity enters combat packets.

The authoritative generated `func_global_asm_806B13B4` in
`RecompiledFuncs/funcs_28.c` accepts collision result 9 with a null attacker,
selects its 0x20B zero-health death animation, enters state 0x37 through the
existing common helper and preserves the handler's local death/drop progression.
State 0x32 skips normal collision death unless the engine's alternate collision
flag is present, so the adapter defers remote commands in that state without
changing health. Locally earned blueprint flags continue through the existing
item channel; combat does not invent or transmit a drop or award.

The original-handler descriptor is now sixteen entries and all entries must match
before any wrapper is installed. Compile-time assertions pin Kasplat actor IDs and
the sixteen-kind wire maximum. Native validation rejects kinds above sixteen.
Protocol 18, compatibility `0x00010112` and `dk64_coop_tick_v18` reject 0.17
peers/companions. Packet/native span sizes and the sixteen-record snapshot remain
unchanged. The equality of supported-kind count and record capacity does not mean
every kind or every enemy gets a slot: capacity is per spawned record in spawner
order, and retained defeats can still starve later records until respawn/map reset.

Only mod and companion builds and structural packaging were performed. Existing
fixtures received the new enum, handler and export names but were not compiled or
run. No test suites, LAN probes, gameplay, installation, commit, push or publication.
Record: `build/build-v0.18.0/build-status.json`.


## Aquatic common enemies (0.19, untested)

Wire kinds 17 and 18 are Shuri and Gimpfish, local actors 267 and 268 with
pinned handlers `806B2790` and `806B2D64`. Their compiled C in `code_B7490.c`
uses collision result 9, the common null-attacker-safe death helper and state
0x37, followed by local death progression to state 0x40. Shuri selects animation
0x2E1 and Gimpfish 0x2E7. Both retain local swimming, attacks, animation and
effects; optional host movement only corrects the guest position and facing.

They are eligible only in the existing seven main-map allowlist, which includes
Galleon. No interior aquatic maps are enabled. Clams are not ordinary killable
enemies; Pufftups and tomatoes use explosion paths. Krossbones detach head actors,
and books and toy enemies use nonstandard or phase-dependent state machines, so
those remain excluded.

Protocol 19, compatibility `0x00010113` and `dk64_coop_tick_v19` reject older
peers/companions. Packet sizes, the sixteen-record snapshot and isolated saves
are unchanged. The supported kind count is now eighteen but only sixteen spawned
records can be linked at once; spawner-order starvation remains explicit.
Existing fixtures were updated but not compiled or run. Only builds and structural
packaging were performed; no tests, probes, gameplay, installation or publication.
Record: `build/build-v0.19.0/build-status.json`.


## Klaptraps and Krossbones (0.20, untested)

Wire kinds 19–22 are green, purple and red Klaptraps (actors 205/208/209,
handlers `806B75F4`/`806B761C`/`806B7660`) and Krossbones (actor 262, handler
`806AFB58`). The compiled Klaptrap common routine in `done/code_BB300.c` and the
generated Krossbones handler both accept collision result 9 with a null attacker,
enter state 0x37 through the common helper and retain their original local death
progression. Klaptrap skeleton remnants and any detached Krossbones head actors
remain locally created secondary effects and are not independently bound.

Protocol 20, compatibility `0x00010114` and `dk64_coop_tick_v20` reject older
peers. Packet and native spans, the sixteen-record limit and saves are unchanged.
Existing fixtures were updated but not run. Build-only record:
`build/build-v0.20.0/build-status.json`.


## Kaboom and Klobber adapter (0.21, untested)

Wire kinds 23/24 bind Kaboom actor 175/handler `806B640C` and Klobber actor 182/handler `806B63E0`. Their common `806B5A90` progression uses explicit state 0x37 for the existing explosion. For an exact life-token command, the adapter sets only health zero, collision result 9 and that existing state/progress zero, then calls the pinned handler. Readback requires state 0x37. No timer, radius, script or effect comes from the packet. Protocol 27 uses compatibility `0x0001011B` and `dk64_coop_tick_v27`. Build-only; tests skipped. Record: `build/build-v0.27.0/build-status.json`.


## Sheeted Kritter (0.23, untested)

Wire kind 26 binds actor 289 and pinned handler `806B02EC`. That handler calls the compiled common `806AD260` with animation 0x320; collision result 9 enters the null-attacker-safe state 0x37 path. Protocol 27 uses compatibility `0x0001011B` and `dk64_coop_tick_v27`. Pufftup remains kind 25 with its state-0x27 adapter from 0.22. Build-only; tests skipped. Record: `build/build-v0.27.0/build-status.json`.


## Acknowledged combat-record retirement (0.24, untested)

A defeated record no longer remains for the whole map visit. After the guest has applied and read back a host commit, it advertises `DEFEATED` rather than `REQUEST` for one acknowledged snapshot. Native code emits an internal `ABSENT` retirement command only when the existing reciprocal binding matches key, kind and both life tokens and both game inputs read back defeated. The host retires only after receiving that guest acknowledgement. The MIPS adapter clears only the tombstone bit for that exact slot/life/kind; it keeps generation/life metadata, so a later respawn still receives a new token. Retired dead actors are omitted from the sixteen wire records, allowing later spawner entries to link. Protocol 27 uses compatibility `0x0001011B` and `dk64_coop_tick_v27`. Tests remain skipped. Record: `build/build-v0.27.0/build-status.json`.


## Phase-gated dice enemies (0.25, untested)

Wire kinds 27–29 bind Mr. Dice 0, Sir Domino and Mr. Dice 1 (actors 269–271; handlers `806BC080`, `806BC0E4`, `806BC148`). Their wrappers call the ordinary `806AD260` death path only when the spawner phase byte is zero or an internal Toy Monster check allows it. The network adapter conservatively accepts remote commands only when that byte is zero; commands wait during encounter-controlled phases instead of bypassing `806BA240`. Protocol 27 uses compatibility `0x0001011B` and `dk64_coop_tick_v27`. Build-only; tests skipped. Record: `build/build-v0.27.0/build-status.json`.


## Spiderling-only interior combat (0.26, untested)

Wire kind 30 binds Spiderling actor 276 and handler `806AD9F4`, whose compiled collision path enters the common null-attacker-safe state 0x37 death progression. Combat map eligibility now includes map 60 only for this encounter in addition to the seven main maps. The Spider boss actor, boss health, encounter controller and cutscenes remain unhooked and local. Spiderling silk actors remain local secondary effects. Binding still requires identical layout/key/kind and reciprocal life tokens, so phase-diverged spawns do not inherit historical defeats. Protocol 27 uses compatibility `0x0001011B` and `dk64_coop_tick_v27`.

## Loss-resilient item paging (0.27)

The previous per-round backward rotation accidentally gave each collectible page
a fixed nine-send recurrence. A relay or network that dropped every third
datagram could therefore discard the same page forever, leaving both peers at
`LAN ITEMS: WAITING` even while all other pages arrived.

The v27 scheduler sends each page in an adjacent pair. A periodic pattern that
drops isolated datagrams cannot discard both consecutive copies, and the first
copy of all eight pages spans fourteen send intervals (700 ms at the 20 Hz
heartbeat). Item pages now have a separate 900 ms freshness budget: this covers
the worst 850 ms successful-copy gap under the deterministic one-in-three loss
test while player presence, animation and combat retain their 750 ms limit. This
restores the delivery pattern used by the last fully verified item build while
keeping page selection independent of handshake sequence numbers. Protocol 27 uses compatibility
`0x0001011B` and `dk64_coop_tick_v27`; packet size and native span sizes remain
unchanged.

The complete native CTest set passes in a fresh Linux Debug build with ASan and
UBSan: collectible recovery (including blocked-page expiry and reconvergence),
combat game adapter, combat authority, animation timeline, protocol/session,
native ABI and gate recovery. No sanitizer diagnostics were emitted. This does
not replace Android/Windows gameplay validation.

## Downward enemy-health convergence (0.28)

Alive enemy records now pack a positive 15-bit health value above the existing
12-bit facing field. The record remains nine words, so the packet stays 1192
bytes and the bridge input/result spans stay 2444/2996 bytes. Reserved upper
bits, zero health on an alive record and nonzero health on a defeated/request
record fail validation.

For an exact reciprocal key, kind and pair of life tokens, the host selects the
lower positive health read back by either game. The game adapter accepts only a
strictly lower target for the same initialized live actor and never creates a
collision source, reaction, script ID or arbitrary damage amount. The original
local behavior continues to own hit reactions and AI. The guest follows the
host's accepted value; duplicate and replayed targets are idempotent, higher
values cannot heal, and zero health continues through the separate acknowledged
defeat protocol. Health commands carry no position or facing payload, and the
adapter rejects reserved packed bits and nonzero command payload fields. Bosses,
unsupported actors, player health, hit reactions, drops and AI remain local.

Protocol 28 uses compatibility `0x0001011C` and `dk64_coop_tick_v28` so older
companions and peers cannot interpret the packed field as plain facing.

## Compact twenty-enemy wire format (0.29)

The game and native bridge retain explicit nine-word enemy records, but the
network representation compacts each record to seven words. A validated 16-bit
identity word contains the 9-bit spawner key, 2-bit state and 5-bit enemy kind;
the remaining words carry the reciprocal life tokens, position and packed
facing/health field. Twenty records use 140 words. Together with the four-word
combat header and eight seven-word shot records, four canonical zero tail words
fill the established 204-word combat section.

Decoding rejects any identity bits above bit 15 and any nonzero tail word. The
fixed packet remains 1192 bytes and item/world offsets remain unchanged. The raw
game/native arrays now hold twenty explicit records, so the bridge input/result
spans grow to 2588/3284 bytes and retain full pre-access bounds checks.

The capacity is twenty active or acknowledgement-pending records in spawner
order. There is still no record rotation, historical defeat replay or remote
spawning. Protocol 29 uses compatibility `0x0001011D` and
`dk64_coop_tick_v29`, preventing older peers or companions from interpreting the
compact record layout.

## Glasses fireball defeat synchronization (0.30)

Wire kind 31 binds the glasses fireball actor 273 to pinned original handler
`806B24B8`. The actor table in the decompressed US ROM and the compiled handler
both confirm the binding. An exact reciprocal remote defeat injects only the
handler's existing collision result 9. The original handler then owns terminal
state `0x40`, disappearance and its local visual effect. A locally owned gun or
orange collision follows the same observation and acknowledgement path; an
unowned projectile, scripted collision or culled actor cannot create a request.

The rabbit target and surrounding encounter state are not transmitted. Toy
Monster, Ruler and tomato encounter controllers also remain excluded because
their deaths are coupled to multi-phase scripts. This addition does not create
remote actors, replay historical kills or synchronize AI and attacks.

Kind 31 is the highest value in the compact five-bit kind field. The maximum
valid key/state/kind identity is `0xff00`; malformed kind 32 encoding must fail
instead of spilling into reserved bits. The packet remains 1192 bytes, the game
and native spans remain 2588/3284 bytes, and all four compact tail words remain
canonical zero. Protocol 30 uses compatibility `0x0001011E` and
`dk64_coop_tick_v30` so older peers and native companions cannot misinterpret
the new combat contract.

## Reviewed interior combat maps (0.31)

Combat eligibility expands from the seven main maps plus the Fungi Spider room
to 58 explicit vanilla maps. The additions cover ordinary-enemy interiors in
Japes, Aztec, Galleon, Fungi, Caves and Castle, plus Hideout Helm, DK Isles and
the Japes, Aztec, Factory and Castle lobbies. Chunky's Caves igloo (map 95) is
included for its five glasses fireballs. Each included map has at least one of
the 31 already-pinned enemy kinds in the reviewed vanilla enemy-location table.

This is only a map gate expansion. An exchange still requires both players in
the same map, identical immutable spawner-layout hashes, matching enemy kind and
key, and reciprocal nonzero life tokens. A map transition clears captured lives
even when returning to the same interior. The adapter never spawns a missing
enemy or replays a kill seen before binding. The existing original-handler,
owned-collision, positive-health and terminal-state checks are unchanged.

Boss maps, battle crowns, races, bonus barrels, minigames and scripted reward
controllers remain outside the allowlist. Unsupported actors inside an allowed
map also remain local. The original Guard/Kop is deliberately not added: it has
no reviewed vanilla enemy slot, its contact path can warp the player out, and
randomizer-specific Kop variants alter that behavior.

All 58 map numbers are compile-time asserted against the pinned DK64 enum and
are exercised as accepted cases in both game-adapter and native authority tests;
representative boss/minigame IDs are exercised as rejected cases. Packet size,
combat record layout and native spans remain 1192 bytes and 2588/3284 bytes.
Protocol 31 uses compatibility `0x0001011F` and `dk64_coop_tick_v31` so a v30
peer cannot silently apply the narrower map contract.

## Ruler defeat synchronization and six-bit kinds (0.32)

Wire kind 32 binds Ruler actor 230 to the pinned vanilla handler
`806BC1AC`. The reviewed vanilla enemy table identifies spawner keys 40 and 41
in Frantic Factory as Rulers. The handler retains its own `806BB32C` setup and
`806BA93C` behavior path; the game's crown-enemy accounting recognizes its
existing state `0x37` as defeated. An exact reciprocal command supplies only
health zero and collision result 9, then calls that pinned handler. Locally
owned gun/orange collisions use the same confirmed readback path.

The adapter accepts Ruler health/defeat commands only while the current
spawner phase byte is zero. Commands wait through encounter-controlled phases
instead of bypassing them. Toy Monster and both tomato encounter controllers
remain local, as do Ruler AI, attacks, hit reactions, drops and sounds. No
remote actor, attacker pointer, behavior argument or script ID is accepted.

The compact identity word now reserves nine bits for the spawner key, two for
state and six for enemy kind. Bits above bit 16 are rejected, so kind 32 is
representable without accepting arbitrary high fields. The maximum valid
key/state/kind identity is `0x10700`; malformed kind 64 and noncanonical tail
words fail validation. The four zero tail words, 1192-byte packet, twenty-record
capacity and 2588/3284-byte native spans remain unchanged. Protocol 32 uses
compatibility `0x00010120` and `dk64_coop_tick_v32` so older peers and native
companions cannot decode the expanded identity contract.

## Guarded enemy animation-frame correction (0.33)

The fourth combat option retains host-authoritative movement and adds a visual
frame correction for linked enemies. It activates only when both peers select
the option. The receiver still requires the same reviewed map, layout, spawner
key, enemy kind, exact reciprocal life-token binding, live actor and ready
session used by health and movement synchronization.

The existing `peer_life` word now carries the reciprocal life token in its low
26 bits and a six-bit hash of the sender's current local animation clip in its
high bits. The packed yaw word retains its twelve-bit facing and fifteen-bit
health fields and uses the five high bits for a normalized frame in 31 steps.
Both are bounded visual metadata. A peer cannot supply a pointer, clip ID,
animation script, control state, attack state, timer, effect or damage event.
Life tokens wrap inside the nonzero 26-bit domain; exact reciprocal comparisons
still prevent a stale record from binding to a different actor generation.

On the guest, correction is skipped unless its enemy already has a valid local
animation track and the six-bit hash of that local clip matches the host. The
normalized sample is mapped into that local clip's own frame count. The adapter
temporarily clears the track's animation callback fields while invoking the
game's existing frame evaluator, then restores every callback field. This avoids
running animation-script callbacks while applying the visual correction. A
six-bit collision can only seek within the clip the guest already selected; it
cannot load the host's clip or change AI. Attacks, hit reactions, sounds, effects,
secondary actors and damage continue to run locally, so this remains visual
timing correction rather than synchronized enemy control.

The twenty combat records, four canonical-zero tail words, 1192-byte packet and
2588/3284-byte native spans remain unchanged. Nonzero tail words now use invalid
combat mode 4 as their decoder sentinel because mode 3 is a valid negotiated
feature. Protocol 33 uses compatibility `0x00010121` and
`dk64_coop_tick_v33`, rejecting older peers and companions. All seven native
suites pass in a fresh isolated Linux Clang 21 Debug ASan/UBSan build; gameplay
validation remains pending.

## Non-freezing enemy pose sampling (0.34)

The guest now records the last successfully evaluated enemy pose sample per
exact local spawner binding. The local-only key combines the host's nonzero
26-bit life token with the six-bit local-clip hash and five-bit normalized
frame. Repeated render callbacks for the same 20 Hz result do not seek the
animation again, allowing the game's local animation to advance between new
network samples. A different normalized frame, clip hash or host life token is
a new sample.

The sample is consumed only after the guest has a valid local animation track,
the local clip hash agrees and the callback-suppressed evaluator has run. A clip
mismatch is therefore retryable if local AI later selects the matching clip.
Changing actor generation or spawner binding clears the sample. Disconnect,
stale state, unsafe map/session context, or leaving the fourth combat mode also
clears all remembered samples so a restored session may apply its current pose.

This changes no wire fields or native struct sizes. The twenty combat records,
four canonical-zero tail words, 1192-byte packet and 2588/3284-byte native spans
remain unchanged. Protocol 34 uses compatibility `0x00010122` and
`dk64_coop_tick_v34`, rejecting the earlier companion so a v33 NRM cannot retain
the repeated-seek behavior. The adapter test covers one-time application, a new
sample, context restoration, clip mismatch and animation-callback restoration.
All seven native suites pass under Linux Clang 21 Debug ASan/UBSan; gameplay
validation remains pending.

## Army Dillo damage phases (0.35)

Army Dillo actor 185 and overlay handler `func_boss_800254D0` are supported only
in the Japes and Caves boss arenas, maps 8 and 196. Disassembly of the pinned US
overlay shows that `Actor.health` is not the fight's damage counter. A successful
TNT collision result 4 enters control state `0x4D`, clears the script index at
additional-data offset `0x15`, increments the phase byte at offset `0x14`, and
selects the next map-specific phase script. Four increments reach the terminal
phase. The adapter reads that phase but never assigns it or the script index.

The former four canonical-zero combat tail words now encode a bounded boss kind,
local life token, reciprocal peer life token and phase 0..4. Both peers must be
active in the same arena with combat enabled. A new actor generation receives a
new nonzero 26-bit life token. Native binding requires the exact reciprocal life
pair and kind. The host merges phase by maximum, so simultaneous reports of the
same phase do not count twice. Guest progress remains a request until the host's
game adapter reads back the phase; only host readback commits progress to the
guest. The terminal phase is retained through arena teardown so it can be sent.

When a bound target is ahead, the receiver injects collision result 4 before the
pinned vanilla handler. It advances at most one phase per handler call and waits
while state `0x4D` is active. It also requires initialized actor identity,
generation, ready session, nonzero reciprocal life and an otherwise empty local
collision result. Local collisions are never replaced. The handler itself owns
the phase increment, reaction, animation and subsequent script selection. If its
behavior-table entry differs from the pinned overlay function, the channel fails
closed without replacing that entry.

No boss position, rotation, pointer, phase address, script index, animation,
timer, attack, missile, barrel actor, cutscene, sound, effect or player damage is
transmitted. All other bosses remain local. The packet remains 1192 bytes because
the boss record uses the existing tail; the raw game/native combat structs gain
four words, making bridge spans 2604/3300 bytes. Protocol 35 uses compatibility
`0x00010123` and `dk64_coop_tick_v35`. Native tests cover both arenas, reciprocal
rebinding, host readback, monotonic maximum, malformed phases, real local impacts,
reaction deferral, collision preservation and handler conflicts. All seven suites
pass under ASan/UBSan; gameplay validation remains pending.

## Dogadon damage phases (0.36)

Dogadon actor 236 and overlay handler `func_boss_8002A92C` are supported only in
the Fungi and Aztec boss arenas, maps 83 and 197. Pinned US overlay disassembly
shows the same private counter/script layout used by Dillo: offsets `0x14` and
`0x15` from actor additional data. Collision result 4 increments the counter and
enters reaction state `0x4D` for the first three impacts. When the counter is
already three, the fourth result enters terminal state `0x56` without incrementing
the counter. A separate non-collision timer/fallback path writes raw value 4 and
dispatches a different script table entry; capture deliberately rejects that raw
value rather than confusing it with terminal damage.

The common four-word boss record therefore assigns Dogadon logical phases 0..3
to the observed private counter and exposes logical phase 4 only after the local
vanilla handler has entered exact state `0x56`. The terminal logical value is
then retained through actor teardown. The adapter never writes Dogadon's private
counter or script index. A receiver behind the bound target injects only collision
result 4 with a null source and invokes the pinned handler; state `0x4D` defers
the next step, state `0x56` cannot accept another, and a nonzero local collision
always wins.

The native layer maps kind strictly to arena: Dillo only on 8/196 and Dogadon
only on 83/197. Both use exact actor-generation life tokens, reciprocal peer
acknowledgement, host maximum and host game-readback authority. Each actor-table
entry is hooked only when it still equals its pinned overlay handler; a conflict
fails closed independently. Dogadon movement, flight, fire, Hunky Chunky/TNT
barrels, arena sinking, attacks, animation, effects, sounds, cutscenes and player
damage remain local.

No packet or native span grows: the packet remains 1192 bytes and the bridge
spans remain 2604/3300. Protocol 36 uses compatibility `0x00010124` and export
`dk64_coop_tick_v36` so v35 companions cannot interpret the new boss kind.
Native and production-adapter tests cover both Dogadon arenas, all four logical
steps, terminal readback/retention, map-kind rejection, transition deferral and
handler conflict. Gameplay validation remains pending.

## Reviewed coordinated transitions (0.39)

Version 0.40.1 makes consumption a join-side opt-in. The host continues to emit
the same validated transition record, preserving the wire format and allowing a
guest that selected **Follow host** to consume it. The default **Independent
exploration** passes a disabled join state into the existing transition policy,
which clears pending history and never calls the map-transition function. Item,
world and presence traffic continues across different maps; remote rendering and
combat already require matching map IDs. No protocol or native ABI field changed.

The v39 player state appends a monotonic host ticket and one packed route word
after the former 1192-byte packet body. The route contains an eight-bit source
map, eight-bit destination map and signed eight-bit exit; its high byte must be
zero. The decoder accepts exactly 35 pinned US tuples: Isles to the six ordinary
physical level lobbies and their returns, Castle lobby back to Isles, and both
directions between all seven lobbies and their main levels. Eight additional
tuples cover both directions between Jungle Japes and Mountain, Beehive,
Painting Room, and Underground. Interior entry uses exit 0; the four returns
use the pinned Japes exit-table rows 2, 1, 5, and 8. Lobby entry uses
exit 0, level return uses exit 1, and lobby-to-Isles exits are 2, 3, 4, 5, 6,
10 and 11. Every other combination fails packet validation.

Capture occurs only on the host while a peer is already connected, in active
adventure play, on a rising loading transition edge whose complete tuple is
allowlisted. Each accepted edge receives a new nonzero ticket. The host retains
the record for 300 game callbacks, even after its current map changes, so ordinary
UDP loss during the closing transition does not strand a guest in the source map.
A peer/session loss immediately clears that retained record, preventing it from
being published to a later connection.

The guest first binds a new ticket only if it is already in the recorded source
map. It waits there until connected, active, outside cutscenes and local loading,
then invokes pinned vanilla `func_global_asm_805FF378(destination, exit)`. The
ticket is consumed only if loading starts. Leaving the source cancels the pending
ticket, duplicates cannot invoke twice, and a disconnect clears ticket history
for the next negotiated session. A record first observed outside its source is
stale and cannot fire if the player later returns.

Isles-to-Castle-lobby is excluded because the pinned US trigger table does not
contain that tuple; the vanilla entrance is scripted. Hideout Helm, K. Rool,
boss portals, Troff & Scoff, Japes Minecart, races, bonus barrels, cutscene maps,
menus and the four-argument transition variants are also excluded. The channel transfers no
cutscene state, actor pointer, script argument, save award or player resources.

Protocol 39 uses compatibility `0x00010127`, export `dk64_coop_tick_v39`, a
1200-byte packet and 48-byte local/remote state spans. The extra input/result
spans remain 2604/3300 bytes. The eighth native suite covers all 35 accepted
tuples, representative exclusions, persistent tickets, same-process replay,
disconnect reset and stale-source cancellation. Gameplay validation remains
pending.

## Automatic world rebuild (0.39)

`automatic_world_refresh` is a local opt-in layered on the existing shared-item
and reversible-world authority paths. It does not add network fields or accept
arbitrary maps, exits or flags from a peer. When a verified permanent unlock or
the Galleon/Fungi reversible flag targets the currently loaded level, the adapter
admits the write only in the existing safe-main-map allowlist, on a stable frame
with no transition in progress. It first verifies the flag readback, queues the
isolated save, and records the current map as needing a rebuild.

After the save call and outside cutscenes or collectible-credit queues, the frame
adapter invokes pinned vanilla `func_global_asm_805FF378(current_map,
current_exit)`. This reconstructs actors and instance scripts through their own
map initialization and returns the player to the entrance used for the current
visit. No actor fields, script states, cutscenes, rewards or player coordinates
are patched. A player-started transition cancels the queued reload because it
already supplies the rebuild; map changes and disabled/error states also clear
stale work. The coordinated host-follow transition path is skipped on a frame
where the automatic reload starts, preventing two transition calls.

Interiors, lobbies, rescues, Bananaport pads, numeric rewards, shops, Troff and
Scoff, bosses and minigames retain their previous deferral rules. Thus the option
removes the whole-level departure for permanent changes observed from a safe main
map, but a player in an excluded interior must first return to a supported map.
Protocol 39 keeps the v38 packet and native-span sizes but uses compatibility
`0x00010127` and export `dk64_coop_tick_v39` so the release remains an exact
NRM/native/peer set. Unit coverage exercises default deferral, opt-in current-level
application, single pending rebuild state, reversible-state handling and stale
pending cleanup. Gameplay validation remains pending.

## Reviewed live world scripts (0.40)

The opt-in automatic world refresh tries a reviewed live path before queuing the
v0.39 same-map rebuild. The pinned US instance-script metadata now identifies 17
flags whose affected main-map readers can safely enter their flag-positive state.
These cover 41 objects: the Japes first, shell/hive and painting-cave gates plus
all four switch huts; Factory's testing-area gate and revealed arcade lever;
Galleon's three weapon gates; two Aztec permanent doors; and three Fungi tunnel
gates.

For each row, the pinned flag-positive state-0 initializer selects the reviewed
state. Gate/hut model and collision changes use their vanilla state 20 path;
the four already-open hut switches resume their normal local state 7 interaction
checks. After the permanent flag write succeeds, the
adapter resolves each pinned object ID through the loaded 600-slot instance-script
table and calls vanilla `func_global_asm_8063DA40(slot, completed_state)`. It does
not rerun state 0, manufacture the interaction that earned the flag, grant a
reward, start a cutscene, or call an arbitrary function supplied by the peer.

Live success requires every reviewed object for that map and flag to resolve. A
missing object, unreviewed flag, mixed side-effect script, actor spawn, global
water/day-night change, or complex puzzle queues the existing save-and-reload
fallback. Thus a partial setup cannot suppress the full rebuild. The feature adds
no network fields; protocol 40 and compatibility `0x00010128` distinguish the new
NRM/native/peer set. Focused host coverage checks complete live resolution and the
missing-object fallback. MIPS compilation verifies both pinned engine symbols;
gameplay validation remains pending.

## Android item-page jitter tolerance (0.41.1)

The adjacent-pair scheduler needs roughly 800 ms for one eight-page rotation at
20 Hz, while v0.41.0 retained only a 900 ms assembled-page lifetime. Android
render scheduling and ordinary Wi-Fi jitter could consume that 100 ms margin,
making item status alternate between `WAITING` and `SYNCED`; reversible world
status followed because it requires an assembled item snapshot.

Version 0.41.1 gives monotonic item ownership/request pages a 2.5-second cache.
The 750 ms current-player/presence check is unchanged and gates every result, and
the transport still disconnects after three seconds. A cached item page therefore
cannot keep a missing player active or cause writes by itself. Item identities,
dependency checks, authority, save-ahead handling, packet fields, bridge spans,
protocol 41 and compatibility `0x00010129` are unchanged. The blocked-page test
now verifies expiry beyond the longer cache followed by bounded reconvergence.

## Cross-area item snapshot continuity (0.41.2)

Item synchronization has no map-identity requirement. After the adapter captures
one complete, dependency-valid inventory in a safe map, it continues advertising
that snapshot while the local player is in a shop, boss, minigame, or other map
whose overlay may temporarily replace inventory fields. This prevents a safe peer
and an overlay peer from forcing `LAN ITEMS` to `WAITING` merely because they are
in different areas.

The cached frame is transport state only. The overlay peer neither samples
temporary inventory nor applies or saves an incoming grant until a later safe
capture verifies the live save layout. Reversible world toggles remain live-sample
only and explicitly reject the cached item frame, so no host revision can be
invented from stale day/night or water state. A fresh device still must capture
one safe snapshot before advertising item readiness. Protocol 41, native bridge
ABI, session binding, item authority, dependencies, and save-ahead policy remain
unchanged.

## Expanded routes, live toggles, and boss rounds (0.42.0)

The reviewed transition table now contains 169 exact source-map,
destination-map, and signed-exit tuples. The additions cover ordinary tunnels,
doors, and interiors in Aztec, Factory, Galleon, Fungi, Caves, and Castle.
Races, minecarts, bonus barrels, boss maps, scripted cannons, Helm, Mech Fish,
Spider, and other special transitions remain absent. The same monotonic ticket,
source-map, active-game, cutscene, and replay checks apply to every tuple.

Automatic world refresh now applies Galleon water height and Fungi day/night
through the exact loaded vanilla switch scripts. Galleon resolves object 0 for
water up or object 1 for water down; Fungi resolves object 4 for night or object
5 for day, then requests script state 10. If the required loaded script is not
present, the adapter retains the same-map reload fallback. Reversible frames
still require current live samples and never use the cross-area cached inventory
frame.

The four-word boss record now assigns ten fixed kinds: Army Dillo, Dogadon, Mad
Jack, Pufftoss, King Kut Out, and the DK, Diddy, Lanky, Tiny, and Chunky K. Rool
rounds. Each map selects one pinned actor type and one pinned US overlay handler.
Remote progress advances at most one local vanilla reaction per actor update and
requires the receiver's reciprocal life token, live actor generation, expected
map, exact handler, initialized actor, normal game state, and a boss-specific
vulnerable state. The private phase byte remains game-owned.

Mad Jack and Pufftoss enter their normal hit states `0x27` and `0x4D`; their
handlers increment phase later in the reaction. Kut Out asks the controller for
reaction 4 only while it is idle. DK, Diddy, and Lanky enter their normal `0x31`
hurt state under round-specific gates. Tiny is bound to the foot actor in map 214:
state `0x37` sets the current toe's temporary flag and returns through vanilla
loading. Chunky's first three steps resume at post-punch progress 8. Its final
step performs the same terminal-state and permanent-victory-flag writes as the
vanilla final-punch branch. Boss AI, movement, attacks, timers, projectiles,
cutscenes, player health, and resource counts remain local.

The packet remains 1200 bytes, while protocol/native ABI v42, compatibility
`0x0001012A`, and export `dk64_coop_tick_v42` reject older behavior sets and
local companions. The manifest requires the matching 0.42.0 package.

## Paged ordinary-enemy snapshots (0.43.0)

The fixed twenty-record game snapshot is now one page rather than a global
ceiling. The adapter walks every supported live or defeat-acknowledgement record
in stable spawner order, reports the total page count, and rotates its captured
page each game frame. The native bridge caches each local and remote page by the
validated spawner key. Bindings remain keyed by layout, kind, local life and peer
life, so a record changing page position cannot inherit another actor's damage.

Network sends choose a page independently of the render-frame capture phase and
send each selected page twice. This avoids frame/send cadence aliasing and gives
periodic packet loss another copy. Missing pages delay only their own enemies;
current shots, hands and the bounded boss record remain in every state packet.
Page-count or session-context changes clear the corresponding caches. Respawn,
map, layout, epoch and reconnect guards remain unchanged.

The page number and count are appended only to the game/native frame and packed
into the previously bounded first combat wire word. The combat section stays 204
words, item/world/transition offsets stay fixed, and the LAN datagram remains
1200 bytes. The combined bridge input grows from 2604 to 2612 bytes; the result
remains 3300 bytes. Protocol/native ABI v43, compatibility `0x0001012B`, export
`dk64_coop_tick_v43`, and the 0.43.0 manifest reject older peers and companions.

## Book and Toy Monster defeat adapters (0.43.0)

Actor 181 (`ACTOR_BOOK`, pinned handler `func_global_asm_806B52DC`) and actor 228
(`ACTOR_TOY_MONSTER`, pinned handler `func_global_asm_806BB400`) join the ordinary
combat allowlist. Neither uses the normal health-driven enemy death helper. A Book
fades through state `0x37`; Toy Monster enters `0x37` and its own handler advances
to terminal state `0x40`. The wrapper records those states only when the collision
source is the real local player or that player's owned projectile.

A validated remote command enters the same pinned local state and then calls the
original handler. No pointer, script, animation, damage amount or effect crosses
the network. Giant Clam remains excluded because its handler only opens and closes
the environmental hazard and exposes no defeat path. Tomatoes remain encounter
controllers rather than ordinary enemies. All thirty-four supported handlers must
still match the pinned US actor table before any ordinary combat hook is installed.

## Reciprocally bound boss movement (0.44.0)

Combat modes 2 and 3 now carry the host boss position and facing to the guest for
all ten bounded boss rounds. The command requires the expected map-selected kind,
the receiver's current boss life token, the reciprocal tail-record binding, the
pinned live actor generation, normal gameplay state, and finite bounded coordinates.
The host never accepts guest motion. The guest uses the ordinary 40% correction
and 500-unit snap threshold; motion expires after 150 ms without a fresh packet.

Boss motion occupies the otherwise empty first ordinary-enemy wire slot on boss
maps. A reserved kind range distinguishes it during explicit serialization, and
the decoder removes it before ordinary-enemy validation. No actor type, pointer,
overlay address or arbitrary state is accepted. Boss AI, attacks, timers, animation,
projectiles, arena scripts and player damage remain local; the existing bounded
phase channel still owns damage agreement.

The game/native frame and result each gain a six-word typed motion record, growing
the combined bridge spans to 2636 and 3324 bytes. The compact combat section and
all subsequent offsets remain unchanged, so packets stay 1200 bytes. Protocol 44,
compatibility `0x0001012C`, export `dk64_coop_tick_v44`, and manifest 0.44.0 reject
older peers and native companions.

## Third reversible world state (0.45.0)

The host-authoritative reversible channel now includes the Caves-lobby pressure
switch flag `0x19D` alongside Galleon water and Fungi day/night. It has its own
monotonic revision, guest request, host acknowledgement and readback bit. A local
change is observed only from a complete reviewed inventory frame. With automatic
world refresh enabled, an incoming change in map 194 resolves pinned object 6 and
enters its exact vanilla state 2 (press) or state 6 (release); that script owns the
linked-door animation and switch presentation. The reviewed lobby frame uses the
ordinary isolated-save call without admitting any deferred item grant. A missing
object saves the verified flag and falls back to a same-lobby vanilla rebuild.
With refresh disabled, state still applies safely outside the lobby and its twelve
readers initialize on the next entry. No player position, collision source or
arbitrary script state is accepted from the peer.

Three independent revision/request/base/ack lanes require nineteen world words.
Four new words reuse bytes 80..95 of the retired standalone Japes-gate wire area,
with a fixed marker in its old value word; the remaining fifteen world words keep
their established offset. Progress and world encodings are mutually exclusive,
the final retired word remains canonical zero, combat/item offsets do not move,
and the UDP packet stays 1200 bytes. The game/native input span grows to 2640
bytes while the result remains 3324 bytes. Protocol 45, compatibility
`0x0001012D`, export `dk64_coop_tick_v45`, and manifest 0.45.0 reject older sets.

## Rear Japes boulder bunch (0.46.0)

The final vanilla colored-banana location outside the two generated pickup
tables now has stable item ID 5894. ROM/decomp tracing identifies actor 110
(`ACTOR_CB_BUNCH`) and the rear Japes boulder actor handler. Boulder spawner 6
checks permanent flag `0x01D` and passes that same flag to the spawned bunch.
The item adapter therefore uses vanilla persistence instead of inventing a save
bit or sidecar file.

On remote receipt outside Japes, the adapter first writes and reads back flag
`0x01D`, then adds exactly five available Japes bananas to Chunky under the
existing 100-banana cap and medal derivation rules. Inside Japes it waits, so no
loaded boulder or drop actor can race the write. A locally collected bunch is
observed through the ordinary permanent-flag callback and its queued vanilla
credit drains before snapshot capture. Existing IDs 0 through 5893 do not move.
Protocol 46, compatibility `0x0001012E`, export `dk64_coop_tick_v46`, and
manifest 0.46.0 reject older packages.

The same release appends stable item ID 5895 for
`PERMFLAG_PROGRESS_K_ROOL_DEFEATED` (`0x1B0`). The pinned final Chunky K. Rool
handler writes this flag after the local winning sequence; the frontend later
reads it for the ending and main-event menu state. A remote grant uses only the
ordinary permanent-flag write, readback and isolated-save path in a safe gameplay
map. It never invokes a cutscene, transition, boss handler, HUD or inventory path.
Complete-snapshot validation requires the final K. Lumsy state, whose existing
dependencies require the lobby, all eight key items and all eight turn-ins.
Canceled writes retry, an already-local victory wins the race without a second
save, and a save claiming victory without that route latches fail-closed. Existing
IDs 0 through 5894 do not move; packet, bridge and protocol sizes are unchanged.

Stable item ID 5896 covers `PERMFLAG_PROGRESS_ARCADE_2_COINS_PAID` (`0x083`).
Pinned `func_global_asm_8064EA48` deducts two local coins and then writes this
flag; `func_global_asm_8064EB3C` reads it only after the Factory Arcade GB
(`0x082`) has been won. Complete-snapshot validation therefore requires both
the existing Arcade-lever world row (`0x081`, item 2309) and Arcade GB item 214.
The remote adapter waits outside Factory, writes and reads back only `0x083`,
then requests the normal isolated save. It never changes a coin counter, starts
Arcade, sets a temporary Arcade flag or grants either reward. A blocked write
retries, a local payment wins the race, and invalid prerequisite snapshots fail
closed. IDs 0 through 5895 remain unchanged.

## Read-only LAN trace endpoint (0.48.0)

Each native session binds the first available UDP port from `base + 1` through
`base + 8` using a fresh socket for every candidate. A dedicated worker answers
the fixed `DK64COOP_TRACE_V1` request from loopback and private IPv4 sources. It
serializes a mutex-protected cache populated by `Session::tick`; it never reads
RDRAM or calls the game adapter. The bounded JSON contains protocol and
compatibility values, local/session fingerprints, role/status/address, packet
age, rejected-packet count, current map/epoch/player flags, and item, world,
same-area-event, and combat readiness. Local item wait reasons distinguish first
snapshot, unsafe area, save-file change, invalid counters, reward queue, HUD,
same-level pickup, progression context, and Troff feeding.

`tools/query_trace.py` uses one socket per target/port. This avoids Windows UDP
`WSAECONNRESET` from an unused port invalidating a reply received for another
candidate. With no `--ip`, it recomputes local `/24` broadcast addresses on each
watch iteration, so DHCP address changes do not require a stored diagnostic
address. The trace endpoint is diagnostic only and does not alter gameplay,
campaign saves, peer discovery, or the mod loader. Protocol 48, compatibility
`0x00010130`, export `dk64_coop_tick_v48`, and manifest 0.48.0 reject mixed
native companions and peers.

Training Grounds map 176 can publish the first complete snapshot in 0.48.0. Its
ordinary save inventory layout passes the same dependency and counter checks as
the main worlds, allowing a fresh session to initialize before either player
exits the starting area. The general `deferred` gate remains set, but a second
per-ID gate admits only training IDs 2191..2197 and exit-switch ID 2375 while the
frame is active, the HUD exists, and no collectible-credit queue is pending.

The four barrel actors run `func_global_asm_80681BD8`, which checks spawned flag
`0x17F` and ability flags `0x182..0x185` every update and selects its own vanilla
visibility/control state. Course rewards therefore cross through the persistent
item transaction after the originating barrel's local queue drains; no remote
barrel actor, resource refill, timer, or reward-queue pointer is copied.

The Training Grounds exit is permanent flag `0x181`, distinct from all-courses
flag `0x187`. Pinned US setup map 176 writes it from object `0x39`; flag-positive
initializers select state 20 for switch `0x39` and door `0x49`. Both rows are in
the live-world table and must resolve together. A verified grant runs those exact
loaded script states and saves once, without a room reload. Missing scripts keep
the permanent result for the next vanilla initialization and never invoke an
arbitrary object.

When the all-courses bit is newly received or derived in map 176 and the peer's
validated state is also map 176, the adapter raises a one-frame request for the
stock `playCutscene(player, 3, 1)` call. The request is consumed once after the
item transaction and only during regular play with no loading or reward queue.
Every other remotely-startable cutscene remains prohibited; the generic
same-area cutscene channel still only aligns two already-running matching scenes.

## Factory Snatch Room grate (0.49.0)

Factory map 26 object `0x15` is the Snatch Room punchable grate. It uniquely
waits in state 0 and enters state 1 after the stock Chunky punch checks. The
typed-trigger wire now accepts activation state 1, while the game adapter
permits a state-zero receiver only for this exact map and object.

The local state-1 script retains its camera, sound, grate animation and reset
through states 2 to 4. No packet can name those later states, a player move or
a linked object, and an active local sequence is never rewound.

## Factory production switch triggers (0.49.0)

The four production-room slam switches in Factory map 26 are pinned to objects
`0x2E`, `0x2F`, `0x30` and `0x31` (Chunky, Tiny, Lanky and Diddy). A typed
same-area trigger record normalizes each loaded script to ready state 1 or fired
state 2. When the host fires one and the guest still reports ready state 1, the
guest invokes `func_global_asm_8063DA40` with that exact allowlisted object and
vanilla activation state 2. The local script then owns its timer, cutscene,
linked object changes and reward spawn.

No later raw state, timer, object pointer or reward is accepted from the network.
A ready host never rewinds a guest action, repeat packets are idempotent, and a
missing or unlisted script is ignored. Persistent production GB ownership remains
in the item channel. Both players must be in the same Factory room epoch with
**Same-area events** enabled. Protocol 49, compatibility `0x00010131`, export
`dk64_coop_tick_v49` and manifest 0.49.0 reject mixed semantic versions.

## Factory Chunky cage switch (0.49.0)

Factory object `0x24` is the slam switch that frees Chunky. It waits in state 1
and enters state 2 after the stock character, slam and contact checks. A typed
trigger can mirror only that entry while both players share the same Factory
epoch. The receiving script then owns its 600-frame timer, cage notification and
rescue presentation.

The packet cannot name cage object `0x21`, reward object `0x78`, a cutscene or an
ownership flag. State 0 is rejected, later states cannot be selected, and
permanent Chunky ownership still converges through the item transaction.

## Factory instrument pads (0.49.0)

Factory objects `0x37`, `0x38` and `0x3B` are the triangle, guitar and trombone
pads. Their stock scripts wait in state 1 and enter state 2 after validating the
instrument, Kong and contact. A typed trigger mirrors only that entry. Each
receiving script retains its own 80-frame timer, presentation and update of
linked object `0x36`, `0x39` or `0x3A`.

The packet carries no instrument input, energy, timer or linked-object state.
States 3 and later cannot be restarted or rewound, and an absent pad fails
closed. Both players must be in the same Factory epoch.

## Factory 3-1-2-4 room switches (0.49.0)

Diddy's timed R&D room uses Factory objects `0x3F`, `0x40` and `0x41`. Their
scripts use states 1–4 for the currently accepted number and enter state 5 only
after the correct local switch hit. The same-area adapter publishes a typed
fired observation while the host script is in states 5–19. A receiving script
may enter state 5 only while its own state is one of 1–4. The allowlist also
pins activation value 5, so a packet cannot select another script entry.

Each receiving script then runs the original 120-frame sequence, cutscene,
switch hiding, enemy spawn and final reward path. States 20 and above are treated
as finished or reset observations and never rewind the peer. Missing scripts,
wrong activation values and repeated packets fail closed. Both players must be
in the same Factory room epoch.

## Factory piano sequence (0.49.0)

Factory object `0x14` is the 112-block piano controller. Twenty-five pinned
wait-state/correct-hit-state pairs describe its vanilla sequence. The wire sends
only a logical completed-note count from 0 through 25. If the host is ahead and
the guest controller is sitting at the exact wait state for its next note, the
adapter invokes only that next correct-hit state. The original script then owns
the note sound, timer, presentation, cutscene and eventual reward before another
network step is eligible.

The controller is never started remotely: both copies must already have reached
its first note wait. Failure states 250–252 publish zero progress, an ahead guest
is never rewound, and a guest cannot skip more than one locally gated note per
script step. The packet cannot carry a raw script state or reward identity.

## Factory dartboard sequence (0.49.0)

Factory object `0x7F` controls Tiny's six-target dartboard. Its non-monotonic raw
states are mapped to a logical hit count from 0 through 6. Six pinned wait/hit
pairs (`15→50`, `16→52`, `17→54`, `18→56`, `19→58`, `20→23`) let the guest run
only its next correct-hit block when the host is ahead. Miss handling, three-frame
delays, target images, sounds and the final reward path remain local.

The network cannot start the controller, choose a target actor or supply a raw
state. Both copies must reach the first local wait state. An ahead guest is not
rewound, completion is idempotent, and every catch-up step waits for the preceding
local delay to reach the next pinned wait state.

## Same-area timer countdowns (0.49.0)

The existing typed timer records for Galleon objects `0`/`1` and Fungi objects
`4`/`5` now carry the script's unsigned 16-bit timer at offset `0x44` together
with its reviewed state. The guest enters the host state through the vanilla
script-state function and applies that timer sample. A per-room cache applies
each distinct network sample once; repeated render frames cannot keep resetting
the countdown to a stale value and freeze it between UDP updates.

No other script field or timer is exposed. The cache resets on room epoch change,
records remain constrained to the four allowlisted objects, and malformed timer
values are rejected by the native protocol validator.

Model-two object ID zero is valid and is distinguished from an unused record by
the nonzero record kind. The native validator therefore accepts key zero for a
typed record while still requiring every kind-zero tail record to contain only
zero words. This covers Galleon water switch `0` and Tiny Temple opening switch
`0` without weakening the record allowlist enforced by the game adapter.

## Galleon instrument and slam switches (0.49.0)

Galleon map 30 objects `0x11`, `0x12`, `0x13`, `0x14` and `0x1B` are the five
ship-opening instrument pads. Objects `0x1C` and `0x1D` are Tiny's and Lanky's
slam switches. Every reviewed script waits in state 1 and enters state 2 after
its exact local input check. Typed trigger records let the guest enter only that
state-2 block while the host sequence is active.

The receiving copy then runs the original instrument or slam presentation,
linked-door state change, timer and completion path. No instrument energy,
player input, door state, cutscene ID or permanent flag is supplied by the
packet. Finished states do not rewind, arbitrary Galleon objects are rejected,
and persistent completion still converges through the item/world channel.

## Llama Temple coconut and bongo switches (0.49.0)

Llama Temple map 20 objects `0x12` and `0x16` are the coconut gun switch and DK
bongo pad. Both wait in state 1 and enter their exact state-2 block after vanilla
input validation. A typed trigger mirrors that activation only while both peers
share the same map epoch. The receiving script performs its own switch/pad
presentation, temple cooling or llama-spit cutscene, linked-object update and
permanent flag write.

The lava gate itself is not driven directly by the packet; it observes the local
flag through its original script. This prevents the network from choosing gate or
cutscene states and keeps persistent ownership in the item/world transaction.

## Llama Temple matching heads (0.49.0)

Llama Temple objects `0x19` through `0x28` are the sixteen matching-game heads.
Each head finishes its local initialization in state 11 and a grape hit enters
state 12. A typed trigger mirrors only that `11→12` edge. The receiving head then
runs its own five-frame debounce, matching sound and controller notification.

State 10 is deliberately rejected because it has not yet enabled contact or its
sound actor. Later states cannot be selected by a packet, repeated samples are
idempotent, and the controller, wrong-pair reset, temporary active flag and
Golden Banana reward remain local vanilla behavior. Permanent GB ownership
continues through the item channel.

## Llama Temple quicksand switch (0.49.0)

Llama Temple object `0x69` is the slam switch for the quicksand tunnel. Its
stock script waits in state 1 and enters state 2 only after the local slam,
character and contact checks. A typed trigger mirrors that entry. The receiving
script retains the local 25-frame and 50-frame delays before updating Aztec door
object `0xA1` and permanent flag `0x3E` through the original operations.

The packet contains no player input, timer, linked object, map transition or
flag. Completed state 20 is never rewound and a missing switch fails closed.

## Tiny Temple opening, guitar and charge switches (0.49.0)

Tiny Temple map 16 objects `0`, `4` and `0x14` are the opening switch, Diddy's
guitar pad and the charge switch. Each waits in state 1 and enters its original
state-2 sequence only after its local input condition. A typed trigger can make
the guest enter exactly state 2 while both peers share the same map epoch.

The local scripts retain every linked-door update, cooling step, cutscene and
permanent-flag write. Packets cannot select those later states or name their
linked objects. Objects `0xC` through `0xF`, the four KONG-letter switches, use
the ordered logical sequence below instead of a generic fired trigger.

## Tiny Temple KONG-letter sequence (0.49.0)

Tiny Temple's four letter scripts activate in object order `0xF`, `0xE`, `0xD`,
`0xC`. The adapter derives a logical progress value from 0 through 4 by finding
the one letter that the vanilla chain has placed in state 10. Completion is
observed only while object `0xC` runs its reviewed states 11 or 20–22. Raw
letter states never cross the native ABI.

When the host is ahead, the guest may move only its next active letter from
state 10 to state 11. The original script then advances the following letter,
runs wrong-hit resets, presentation and timers, and ultimately starts the local
Tiny rescue controller. A guest whose first letter has not been enabled by the
charge-switch sequence is left untouched, so a packet cannot start or complete
the puzzle on its own. Permanent Tiny ownership still converges through the
item channel.

## Japes Diddy-cage coconut switches (0.49.0)

Jungle Japes map 7 objects `0x30`, `0x31` and `0x32` are the three vanilla
coconut switches around Diddy's cage. Each waits in state 1 and enters state 2
only after the local projectile check. A typed trigger mirrors that exact entry
while both players share the same Japes map epoch.

The receiving switch executes its original counter, sound, model and temporary
state operations. The cage objects `0x2D` through `0x2F` observe that local
counter and run their own timed opening and reward path; the packet never names
a cage state, counter value, projectile or Diddy ownership flag. Permanent Diddy
ownership remains in the item channel.

## Japes hive-area feather switches (0.49.0)

Jungle Japes map 7 objects `0x34` and `0x35` are the two feather switches for
the hive-area gate. Their state-1 projectile checks enter state 2 and update the
vanilla local counter observed by gate object `0x33`. Typed triggers now carry
only that activation instead of exposing the switches through generic raw
same-area script records.

The existing flag-7 live-world table remains responsible for completed-state
refresh of both switches and the gate. During the live sequence, the receiving
switch runs its own sound, model, counter and timer operations. Packets cannot
select gate state, counter value, projectile data or the permanent completion
flag.

## Remaining Japes gun switches (0.49.0)

Four further Jungle Japes scripts have the same pinned activation boundary:
painting-room switch `0x28`, paired Diddy-cave switches `0x29`/`0x2A`, and Rambi
switch `0x123`. Each enters state 2 only after its vanilla projectile check.
The typed trigger then lets the receiving script perform its own linked-object,
counter, sound, model and timer operations.

The painting switch's completed-state path still uses the existing flag-`0x1E`
live-world table. The Rambi switch remains temporary and starts its own 500-frame
local window; no timer or linked object state crosses the network. Supporting
object `0x123` also verifies that the adapter and test boundary retain the full
16-bit script-object key rather than truncating it to one byte.

## Aztec exterior llama switches (0.49.0)

Angry Aztec map 38 objects `0xD`, `0xE` and `0xF` are the coconut, grape and
feather switches for the exterior llama puzzle. A local projectile hit enters
state 2, and states 2–6 perform the reviewed target, timer and linked-door work.
A map-specific predicate publishes fired progress only in that range.

These scripts also use states 10–12 for a separate linked vanilla event. Those
states are explicitly normalized to ready and cannot be translated into a gun
hit on the receiver. A valid fired record can move a receiving switch only from
state 1 to state 2; later linked-object states, gun data, cutscenes and permanent
flags remain local.

## Aztec exterior guitar pad (0.49.0)

Angry Aztec map 38 object `0x44` is Diddy's exterior guitar pad. Its exact
instrument checks move state 1 to state 2, after which the original script owns
the 200-frame timer and activates linked rotating-tower object `0x9C`. A typed
trigger mirrors only that state-2 entry while the peers share the same map epoch.

Instrument input, energy, timer values, linked-object state and permanent flags
are not carried by the record. The guest's local script performs those actions,
and a pad already in its state-2/state-3 sequence is never restarted or rewound.

## Aztec blueprint-door switches (0.49.0)

Aztec objects `0x9D` and `0x9E` are the two coconut switches for the blueprint
door route. Both wait in state 1 and enter state 2 after their exact projectile
check. Typed trigger rows supersede the broader completed-world script rows for
these two objects, so an in-progress packet can request only the state-2 entry.

The original scripts retain their 100-frame presentation and update linked door
object `0x8D` plus permanent flag `0x30`. The linked door is not a trigger key,
and its completed state still refreshes through the permanent world table.

## Fungi tunnel gun switches (0.49.0)

Fungi objects `0x18`/`0x19` are the green-tunnel feather pair, `0x1A`/`0x1B`
are its pineapple pair, and `0x1E` is the yellow-tunnel grape switch. Each waits
in state 1 and enters state 2 after its exact local projectile check. Typed
trigger rows supersede broad raw-state mirroring for these five objects.

The receiving scripts retain their 100-frame switch presentation and linked
gate updates. Permanent flags `0xCF`, `0xD0` and `0xD2` still converge through
the world transaction and their completed gate rows. Packets cannot name the
linked gate objects, counters, projectiles or permanent flags.

## Isles lobby feather switches (0.49.0)

Aztec lobby object `0x10` and Fungi lobby object `0x05` are the two vanilla
feather switches. Each waits in state 1 and enters state 2 after its exact local
projectile check. A typed trigger mirrors only that entry while both players
share the same lobby epoch.

The Aztec script retains its linked stone-panel update; the Fungi script retains
its 200-frame timer and linked door update. Permanent lobby flags continue
through the world transaction. Records are map-bound and cannot cross between
the two lobbies or select their linked objects.

## Isles rocket-barrel trombone pad (0.49.0)

Isles object `0x31` is Lanky's trombone pad for the high rocket barrel. The
script waits in state 1 until its local barrel actor is available, then waits in
state 2 for the instrument. The same-area adapter may request state 3 only from
local state 2, preserving that dependency before the script starts its reveal.

The original script owns the presentation, linked actor state and permanent
flag `0x1AA`. A guest still in state 1 is left untouched, and packets cannot
name the barrel actor or force the later completion state.

## Caves boulder pads (0.49.0)

Caves object `0x2E` is the small-boulder pad and enters state 2 from ready state
1 after the stock carried-object check. Object `0x2F` is the large-boulder pad;
it becomes eligible only in state 12 after its local reveal and enters state 13
after the carried-object check. Typed triggers mirror those two exact edges.

No carried actor, player state or object pointer crosses the network. Each
receiving pad runs its original dome notification, presentation and permanent
flag `0x10E` update. Dome objects `0x27` and `0x2B` remain locally controlled,
and the large pad cannot be activated before local state 12.

## Fungi attic activators (0.49.0)

Fungi Rafters map 56 object `0` is the dark-attic guitar pad, and Mill Attic
map 58 object `0` is Lanky's Simian Slam switch. Both initialize to ready state
1 and enter state 2 only after their pinned vanilla move checks.

Typed triggers mirror that activation edge. The local scripts retain their
timers, sounds, room objects, enemies and permanent completion flags. Completed
state 20 is never treated as a remotely startable action.

## Chunky Caves cabin targets (0.49.0)

Chunky's Caves cabin map 90 objects `3`, `4` and `5` are the three gun targets.
Each enters state 2 from ready state 1 after a stock projectile hit. That same
vanilla block increments Gorilla Gone controller object `6`, state index 0, by
one before changing the target state.

The receive adapter reproduces both parts in that order. It accepts a target
only once and increments controller state only from 1 through 3; a missing,
uninitialized or completed controller blocks the packet. Target state 20,
Gorilla Gone activation, warp collision, reward and saved completion stay local.

## Castle Lanky/Tiny crypt switches (0.49.0)

Castle crypt map 108 object `0` is Lanky's grape switch and object `4` is Tiny's
Simian Slam switch. Both wait in state 1 and enter state 2 after their respective
local input checks. Typed triggers mirror only those activation entries.

The grape script retains its sound, 25/110-frame delays, linked door `1` and
platform state. The slam script retains its 300-frame sequence and linked
objects `6`, `7`, `9`, `0xA`, `0xB` and `0xC`. None of those linked objects,
timers or player inputs can be selected by a packet.

## Castle DK/Diddy/Chunky crypt switches (0.49.0)

Castle crypt map 112 objects `0xD`, `0xE` and `0xF` are the pineapple,
coconut and peanut switches. Each waits in state 1 and enters state 2 after
its exact projectile check. Typed triggers mirror only those entries.

Each local script retains its own switch animation, sound, shared door update
and reset timing. Packets cannot name the door, a projectile or later states,
and an already-running switch is never rewound.

## Castle basement slam switches (0.49.0)

Castle Basement map 163 objects `4`, `5` and `6` are the DK, Diddy and Lanky
Simian Slam switches. Each initializes into ready state 1 and enters state 2
only after the stock move, Kong and contact checks. Typed triggers mirror that
single activation edge.

Each receiving script retains its local sound, door/platform target and states
4 through 7. Packets cannot select a Kong, target object or later state, and a
local sequence that already left state 1 is never rewound.

## Castle tree door switches (0.49.0)

Castle tree map 164 object `1` is the grape switch and enters state 2 from
ready state 1. Object `9` is the Chunky-punch switch and enters state 5 only
from ready state 1. Typed triggers mirror those exact activation edges.

The receiving scripts retain their local door changes, animation, sound and
reset timing. Object `9` cannot be remotely advanced from intermediate states,
and neither packet can identify a linked door, player move or projectile.

## Galleon paired gun switches (0.49.0)

Gloomy Galleon map 30 objects `6`/`7`, `8`/`9` and `0xA`/`0xB` are the paired
cannon-game, shipwreck and lighthouse gun switches. Every script moves from
state 1 to state 2 after its pinned projectile check. Typed triggers replace
their former generic same-area records and invoke only that activation entry.

Each receiving switch updates its own vanilla pair counter, model, sound and
timer. Permanent flags `0x99`, `0xA1` and `0x9B` retain their existing completed
live-world refresh rows for the switches and linked controllers. The packets do
not carry a counter, projectile, linked controller state or permanent flag.
