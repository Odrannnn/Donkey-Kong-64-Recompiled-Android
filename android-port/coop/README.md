# DK64 LAN co-op prototype

Independent, AI-assisted mod for DK64 Recompiled 1.0.2 and the vanilla US ROM.
Version **0.53.0** exchanges durable authority generations over LAN. A returning
former host automatically yields to a newer promoted host, reopens as Join with
its existing host save copy, and remembers that follower role across restarts.
Same-generation hosts use stable node identities as a deterministic tie-break.
Version 0.52.0 persists a fully acknowledged guest checkpoint and promoted
authority in a checksummed sidecar beside the isolated campaign save. A normal
Join/Automatic configuration restores that promoted guest as Host after a full
process restart; an explicit save-copy choice overrides recovery. Version 0.51.0's
room-code discovery still follows DHCP address changes while retaining the numeric
address as a direct fallback. Protocol 53 requires a matching peer and native
companion. It retains 0.50.0's upstream map-load and
EEPROM-load lifecycle boundaries and the
0.49.0 typed same-area activation for the four Frantic Factory
production switches, Diddy's three 3-1-2-4 room switches, Lanky's piano notes,
Tiny's six dartboard targets, Galleon's three cannon targets plus six gun/five instrument/two slam switches, and
Llama Temple's coconut/bongo switches. Tiny Temple's opening switch, Diddy
guitar pad and charge switch use the same pinned activation path, and its KONG
letters advance through a bounded four-step sequence.
All five-door temples mirror their reviewed weapon-panel hits while each copy
owns its wall textures, animation and completion sequence.
Galleon's cannon game mirrors a target hit only when that same target is already
exposed in both copies; target selection, timing and game startup remain local.
Its Enguarde door also mirrors the exact charge result while each copy owns the
door cutscene, animation and permanent completion.
The main-map breakable gate mirrors its exact unbroken-to-breaking entry while
the local script owns its sound, animation and saved removal.
Llama Temple's sixteen matching-game heads now mirror individual grape hits
only after both local copies have armed that head; sounds, pair validation,
resets and the reward remain in each copy's original scripts.
Its quicksand-tunnel slam switch also mirrors the reviewed activation edge and
lets each local script perform the delayed door update.
The temple's grape switch likewise mirrors its single hit and lets each copy
run the timed gate, texture and reset sequence locally.
Chunky's Factory cage switch also starts the stock rescue sequence on both peers;
the cage, cutscene and ownership award remain locally validated.
Factory's triangle, guitar and trombone pads mirror their reviewed activation
edge while their timers and linked production-room objects run locally.
The Snatch Room grate mirrors its exact state-zero punch entry while its camera,
animation, sound and reset sequence run locally.
Factory's two remaining punch switches, two breakable grates and coconut switch
also mirror only their exact local activation entries.
Japes Mountain's paired slam switches mirror each hit together with the stock
state-10 update of the opposite switch.
Its separate GB slam switch mirrors only the state-2 reward-sequence entry.
Japes' seventeen reviewed Diddy-cage, hive, painting-room, Diddy-cave, Kong-hut,
timed-door and Rambi switches now run their local counters and gate sequences
from their exact activation states.
Aztec's three exterior llama switches share only their bounded projectile path;
their separate linked states remain local. Diddy's exterior guitar pad runs its
own rotating-tower timer and linked-object sequence on both peers.
Aztec's four exterior gate switches and all four blueprint-door switches mirror
only their exact projectile-hit entry; their doors and flags remain locally scripted.
Its five totem weapon switches mirror hits only after both copies have reached
the vanilla revealed state; totem activation and availability remain local.
Fungi's paired feather and pineapple tunnel switches and its yellow-tunnel grape
switch now use the same bounded projectile-entry path.
The Aztec and Fungi lobby feather switches also mirror only their stock
projectile-hit entry while their linked panels and doors remain local.
The Isles trombone pad can mirror its rocket-barrel reveal only after the
receiving pad has independently reached its barrel-ready state.
The Fairy Island sax pad uses the same local ready-state requirement before its
instrument activation is mirrored.
Caves' small and large boulder pads mirror only their reviewed deposit entries;
each local script still destroys its own dome and records permanent completion.
The three main-map breakable ice walls mirror their exact punch entry while
each local script owns debris, collision, paired visuals and saved removal.
Chunky's Caves cabin mirrors its three gun-target hits and advances the local
Gorilla Gone controller once per newly received target.
Tiny's Caves igloo target mirrors four bounded hits, one locally ready gate at
a time; its miss/failure branches, presentation and reward remain local.
Fungi's dark-attic guitar pad and Lanky attic slam switch mirror their reviewed
activation entries while their room sequences remain local.
The Winch Room up button mirrors its exact state-10-to-11 activation while the
400-frame winch and reward path stay local.
Fungi's Diddy night slam and Lanky mushroom slam switches also mirror their
state-2 activations without copying day/night or permanent reward state.
The main-map breakable door mirrors its exact punch entry while its breaking
animation and reset cycle run locally.
Thornvine Barn's two slam boxes also mirror only their reviewed state-2 entry.
The Fungi Mill-front slam box mirrors its distinct state-10-to-11 activation.
The rear-mill triangle pad mirrors its state-2 instrument activation while the
crusher start and reward sequence execute locally.
The two rear-mill question boxes mirror only their exact state-10 break entry.
Winch Room and Mill-front state-11 triggers now also require exact state 10.
Hideout Helm's five instrument pads mirror only their reviewed activation
entries; their local doors, timers, flags and final lever remain independent.
Castle's Lanky/Tiny crypt grape and Simian Slam switches now mirror their exact
activation entries while local scripts retain the door and platform sequences.
The three DK/Diddy/Chunky crypt gun switches also mirror their reviewed hit
entries while their shared door, sounds and reset timing remain local.
The three Castle basement DK/Diddy/Lanky slam switches mirror only their exact
activation entries; each peer retains its own door and platform sequence.
Castle's tree grape and Chunky-punch switches mirror their distinct activation
entries while each peer runs its own door animation and reset sequence.
The peer enters each allowlisted switch's exact vanilla activation state; piano
progress advances one locally valid note or target gate at a time. Local timers, presentation,
enemies and reward scripts run normally. Raw controller states are never copied
and permanent GB ownership still uses the item channel. Galleon water and Fungi
day/night timer records now align the actual countdown once per fresh network
sample without freezing local script time between packets. The native transport
now accepts model-two object ID zero, used by the first Galleon water switch and
Tiny Temple opening switch. It retains
the read-only LAN trace endpoint and query tool for
diagnosing live host/peer sessions without exposing ROM or save bytes. It also
retains the 0.46.0 additions: the last vanilla colored-banana collectible that was
outside the generated pickup tables: Chunky's five-banana bunch behind the rear
Japes boulder. It uses the pickup's own permanent flag and credits the receiver
only outside Japes, preventing the local actor from duplicating it. It also
shares the saved final K. Rool defeat after the complete K. Lumsy/key route has
been proven. The receiver gets the persistent completion state without a forced
ending cutscene or map transition. The saved two-coin Arcade replay access also
transfers once the lever and Arcade GB are owned, without charging the receiver.
Item pages remain synchronized while players occupy different
areas by publishing each device's last verified safe inventory snapshot through
shops, bosses, minigames, and other unsafe overlays. Incoming writes remain queued
until that device reaches a safe gameplay frame. It retains v0.41.1's Android/Wi-Fi
jitter tolerance, campaign-save isolation and v0.51 automatic host-address changes. It
keeps eight explicit co-op campaign saves and LAN-address reporting. Campaign 1
preserves the existing co-op saves; campaigns
2–8 are separate. IP, port, room-code and Wi-Fi changes never select a save.
While Host or Join is selected, the adventure menu shows only that campaign's
single physical game file and Delete; Role Off keeps the original three files.
Independent exploration remains the default. Players may use
different maps while collectible and progression synchronization continues;
the remote model and combat pause until they share a map again. A join-side
**Follow host** option restores coordinated travel through 169 reviewed ordinary routes.
Live application now covers 17 permanent gate, hut, switch and door flags:
41 affected loaded objects enter their flag-positive vanilla script state without
a room reload, while complex changes keep the reload fallback. Galleon water and
Fungi day/night now use their loaded vanilla switch scripts too. This version
shares bounded damage phases for Army Dillo, Dogadon, Mad Jack, Pufftoss, King
Kut Out, all five K. Rool rounds, and Tiny's shoe/toe sequence. It also retains
the 0.34 enemy-pose correction that
prevents one 20 Hz network sample from being reapplied on every render frame.
After exact enemy identity and life binding, the guest accepts each new host
normalized-frame sample once, and only when both peers already have matching local clips.
No animation clip, AI state or attack is transferred. Compact pages cover all
supported linked enemies across 58 reviewed vanilla gameplay maps, with downward partial
enemy-health synchronization, loss-resilient collectible-page delivery, and
optional **enemy defeat and movement synchronization** for thirty-four enemy kinds.
It adds Pufftup and the sheeted Kritter alongside Kaboom and Klobber to all three Klaptrap colors, Krossbones and
the earlier beavers, Kremlings, Klumps, Kasplats, flying and aquatic enemies. The
glasses fireball retains its original immediate disappearance and local effect.
Each page still carries 20 records. With the movement option, the guest also follows
the host boss position and facing after reciprocal boss-life binding. Boss AI,
attacks, timers, projectiles and player damage remain local.

The earlier 2394 collectible/progression IDs and Fungi day/night/Galleon water
state are retained. Feeding adds 3500 cumulative milestones; the boulder bunch,
final K. Rool completion and saved Arcade payment append three stable IDs, for
5897 shared IDs. The feeding IDs
are accounting thresholds, not additional collectible locations.
Troff & Scoff feeding from 0.16 is retained: incoming cumulative payments consume
matching available bananas outside their level and update the next boss-door visit.
See [the permanent-unlock coverage table](WORLD-UNLOCKS.md).

Android ARM64 and Windows x64 use the same `.nrm` and UDP protocol, including
Android-to-Android sessions. The existing runtime mod loader is unchanged. Each
platform ZIP contains `dk64_lan_coop.nrm` and one native companion (`.so` on
Android, `.dll` on Windows). No ROM or game assets are included.

**This is experimental, not complete campaign co-op. Version 0.53.0 expands the
persistent recovery and protocol suites with authority reconciliation. All 15 Linux
ASan/UBSan suites and the complete maintained MIPS, Android ARM64, Windows x64,
ABI/export, APK, package and Android-importer pipeline pass. Gameplay and device
validation remain pending.**
Earlier 0.10 results below do not validate
the expanded combat or later progression changes.
Back up experimental saves before using the build.

Training Grounds (map 176) may establish the first verified item snapshot, so
two new files starting there can reach LAN ITEMS/WORLD readiness. Its live apply
path is restricted to the reviewed training flags and exit switch described
below; every unrelated incoming item or world event remains deferred.

## Read-only LAN trace

While the native companion is loaded, `tools/query_trace.py` discovers v0.53
clients on the local `/24` network and queries trace UDP ports 6465 through
6472. Use `python tools/query_trace.py`; if broadcast discovery is unavailable,
pass the current address explicitly, for example `--ip 192.168.68.61`. The
endpoint reports protocol/session identity, role, current map and epoch,
connection age, item/world/event/combat status, and the local reason a pending
item cannot apply. It never returns ROM data, save bytes, arbitrary memory, or
configuration secrets. Requests are accepted only from loopback or private IPv4
addresses and replies are bounded JSON. The trace worker answers from a cached
snapshot, so pausing emulation does not make diagnostics disappear and the
worker never accesses game memory itself.

## Install and connect

1. Close both games. Install the complete matching **0.53.0** ZIP on each device;
   do not mix an older NRM, native companion or peer with this build.
   Both games must provide the upstream 1.0.2 map-load and EEPROM-load events.
2. Android: use the Android port's native-mods-capable dev5 APK or later.
   Import the complete Android ZIP through **Manage Mods -> Import**.
   Windows: extract the Windows ZIP into the mods folder, keeping the NRM and
   DLL together. Enable the mod in the normal mod menu.
3. Set **LAN session** to Host on one device and Join on the other. Start the
   host and read its numeric LAN IPv4 address from the host HUD, then enter it on
   the joining device. Android dev12 opens the software keyboard when a native
   text field is tapped. The address is a fast path: room-code discovery can
   reconnect after either device receives a different DHCP address. Match the port
   (default 6464) and six-digit room code (default 123456). Leave **Transition
   behavior** at its default **Independent exploration**, or choose **Follow
   host** on the joining device for coordinated travel.
4. Select the same **Co-op campaign save** (1–8) on both devices. Enable **Shared
   collectibles and upgrades -> Unique collectibles** on both. Optional combat
   can run alongside it. Item sharing owns the first Japes gate; its older
   standalone experiment has been retired. To avoid
   manually leaving a level for permanent world updates, enable **Auto-refresh
   shared world -> Live update with reload fallback** on each device where you want it.
   This setting may differ between peers. All experiments default to Off.
5. Restart after changing settings. With Host or Join selected, the adventure
   menu shows only the campaign's first physical file plus Delete; campaign 1–8
   is the co-op save selector. Connect in adventure gameplay and wait for **LAN ITEMS: SYNCED**
   before collecting on the guest. Players may then use different maps. The
   remote model and combat resume automatically whenever both share a map.

Role Off leaves normal saves alone. Campaign 1 retains the existing isolated
`items_host_v6` / `items_guest_v6` namespaces when item sharing is enabled and
`prototype_host_v1` / `prototype_guest_v1` otherwise. Campaigns 2–8 append
`_campaignN` to those names. Changing IP address, port, room code or network does
not change this selection. The retired `gate_host_v2` / `gate_guest_v2` files may
remain on disk but are never selected. Fully exit before changing role or campaign.

Each campaign deliberately has a host and guest copy. The host is authoritative
for shared progress; the guest copy supports offline/reconnect safety. Both peers
must choose the same campaign number. If a selected guest copy contains progress
beyond its selected host, **GUEST SAVE AHEAD** stops synchronization instead of
overwriting either file. To preserve both sides, set the host's **Guest-save
conflict** option to **Merge guest progress into host** and restart. The host
imports only validated permanent shared IDs through normal game grant/save paths;
when both show **LAN ITEMS: SYNCED**, return the option to **Stop safely**. This
produces the union and cannot import excluded temporary state. Back up both role
files together before resolving a conflict.

## Host recovery and address changes

Protocol 53 retains the v0.51 behavior that sends each disconnected Join hello
to both the last known host and
the active interfaces' LAN broadcast addresses. A matching host replies directly,
and the guest adopts that source as its new destination. A guest address change
is handled by the same fresh hello after the host's three-second timeout. Networks
with Wi-Fi client isolation or blocked UDP broadcast still require entering the
new host address manually.

The guest continuously evaluates a recovery checkpoint even when the recovery
setting is Off. It requests a normal DK64 save when convergence begins and becomes
eligible only after 120 consecutive regular frames
with both persistent channels synchronized, no requests or applies pending, no
reward/save/reload work queued, and the guest in a reviewed save-safe map. Any
new guest pickup or reversible-world action before disconnection invalidates the
checkpoint until both saves converge again. The native companion then records the
selected save's size and checksum, campaign/copy binding, synchronized-state
fingerprint, random local node identity and authority generation in an atomically
replaced `.coop-recovery` sidecar. Corrupt, foreign, symlinked or mismatched records
grant no checkpoint or authority.

To recover after the host is confirmed offline:

1. On the guest, open the mod configuration and set **Manual host recovery** to
   **Promote now**. Promotion waits for safe gameplay and fails closed without a
   checkpoint. It restarts the UDP socket as Host, creates a new session generation,
   clears old acknowledgements/transient events and keeps the loaded guest save.
2. The promotion is journaled before it becomes active. If the game process later
   restarts while **LAN session** is still **Join** and **Recovery save copy** is
   **Automatic**, that device restores its guest campaign as the effective Host.
   Selecting an explicit save copy disables this automatic role override.
3. The promoted Host broadcasts its durable authority generation and stable node
   identity while waiting. When the former Host returns, it recognizes the newer
   generation, closes its Host socket, reopens as Join toward the promoted device,
   and keeps its existing host campaign copy. This follower decision is journaled,
   so the former Host rejoins after another restart even if its configured role
   still says Host. LAN discovery handles a changed address on either device.
4. If the former host copy contains additional progress, **GUEST SAVE AHEAD**
   still stops writes. Enable **Merge guest progress into host** on the promoted
   Host for that one reconciliation and return it to **Stop safely** after both
   clients reach SYNCED.

Do not promote during a temporary Wi-Fi interruption. A higher authority
generation wins; two accidental hosts at the same generation use their stable
node IDs as a deterministic tie-break. This stops two live authorities from
remaining active once their LAN can exchange advertisements, but it cannot merge
progress made independently during the partition. The ordinary save-ahead guard
continues to block that divergence. A failed local host-port bind returns the device to
Join and requires changing the recovery option away from **Promote now** before
retrying. The sidecar stores only recovery metadata and a save digest; no raw save
bytes are copied into it.

Expanded sharing may reveal differences between previously independent
collectibles or upgrades in old test saves. Use the matching campaign, a fresh
campaign, or the explicit host merge above; restart after changing settings.

Use trusted LAN peers only. Room codes are plaintext collision avoidance, not
authentication or encryption. Do not expose the port to the Internet. There is
no account, matchmaking, relay, router setup or firewall modification.

## How rewards synchronize

- Golden Bananas use a verified local flag-to-Kong/level mapping, including all
  Snide rewards, bonus barrels, races and the Rareware GB. Each missing reward
  credits its intended bucket once, even when a different Kong is active.
- Ordinary pickups include 793 single bananas, 333 bunch locations and 574 coin
  locations. Setup/save-bit identities distinguish each pickup. A bunch grants
  five bananas; an ordinary coin grants one coin to its correct Kong.
- Balloons grant ten bananas to the correct Kong/level. Rainbow coins add five
  coins to **each of the five Kongs**, once per collected rainbow coin.
- Incoming banana credits preserve bananas already fed to Troff & Scoff. Reaching
  75 through remote credits awards the appropriate banana medal.
- **Shop spending remains local.** The purchased upgrade is shared, but only
  its buyer pays. Simultaneous independent purchases can still charge both players.
- **Troff & Scoff feeding is shared from 0.16.** The missing cumulative amount
  is deducted from the matching available banana balance, not copied from the
  other player's balance. Collected totals and banana-medal credit are preserved.
- **Final K. Rool completion is shared.** The terminal save flag applies only in
  safe ordinary gameplay after the snapshot proves all eight keys, turn-ins,
  the lobby and final K. Lumsy state. The winner sees the normal local ending;
  the receiver gets no cutscene, transition, inventory change or HUD event.

Both players may establish and refresh item/progression snapshots in regular
gameplay in DK's treehouse, Isles, the seven main levels, and the reviewed
ordinary gameplay maps, including their ordinary interiors, all seven lobbies
and Hideout Helm. Interior/lobby/Helm snapshots are publish-only: received writes
and saves still wait until that device reaches DK's treehouse, Isles or a main
level. Shops, unreviewed interiors, training grounds/barrels, boss arenas,
minigames, cutscenes and pending reward queues retain the last verified snapshot;
local gains are picked up when the player returns to a reviewed map. Numeric
GB/banana/coin rewards still require main-world/Isles play; the treehouse is for
unlocks and progression. Presence/combat use their own rules.

**Leave the reward's level to receive incoming numeric credits.** For example,
a Japes reward applies when the recipient is playing in Isles or another main
level. Interiors and lobbies count as their owning level (lobbies are Isles).
Snide rewards are the exception: leave HQ and resume main-world play. Shops,
minigames, cutscenes and pending collectible animations do not receive numeric
grants. This prevents a reward being credited while its old collectible or
reward script is still loaded locally. The next level load reads the updated
collection bit. If both players collect the same location first, neither gets a
second credit when network confirmation arrives.

## How upgrades and unlocks synchronize

- Diddy, Tiny, Lanky and Chunky rescue flags transfer once the recipient leaves
  the rescue's level (Japes, Aztec or Factory). No forced Kong switch or replay
  of the rescue cutscene is performed; the separate GB uses its normal rule.
- Each Kong's three special-move tiers, gun and instrument ownership are shared.
  Slam tiers 2/3, ammo belts and global gun/instrument upgrades apply to all five
  Kongs as the stock shops do. Grants never downgrade a tier or replace other bits.
- **Training now synchronizes live:** barrel availability, diving, vines, orange
  throwing, barrel throwing, completion and Cranky's first slam gift. A stable
  Training Grounds frame may receive only these reviewed training IDs; its stock
  barrel actors notice the flags on their next update. The permanent exit-switch
  flag advances both the loaded switch and door through their vanilla state-20
  paths. No leave/re-enter cycle is required. A pending local reward remains
  local and delays remote application until its queue drains.
- When both players are in Training Grounds, a newly received or derived
  all-training-complete flag requests cutscene 3 once. It cannot start from a
  different map, during loading, during another cutscene, or while a reward is
  pending. Cranky's own first-slam conversation remains local; receiving the
  resulting saved gift raises each Kong to at least slam 1 without replaying it.
- Completing all four shared courses derives their completion flag. Someone
  must still obtain Cranky's first slam gift; the mod does not invent that gift.
  Receiving it raises all five Kongs to at least slam 1. Higher upgrades apply
  afterwards without a downgrade, purchase charge or repeated gift scene.
- **Camera and shockwave share their normal combined unlock.** Receive it outside
  Fairy Island, in a safe map. Camera film, crystals and orange refills stay
  local: collect resources or use normal game refills before using the ability.
- On a fresh save, also speak to K. Lumsy and open Japes locally on both devices.
  The shared initial opening waits outside Isles, so it cannot get a player out
  of an otherwise locked Isles. Do not wait there for SYNCED before this visit.
- Candy's maximum melon capacity is shared separately. Current health, ammo,
  instrument energy, crystals, film and purchase refills stay local. A remote
  unlock may need a normal refill or a Kong/map change before use; test this.
- The initial Japes-lobby opening, eight key turn-in flags and final K. Lumsy
  release are shared **outside Isles**. A turned key requires that key's owned
  flag and the initial opening; final release requires all eight turn-ins.
  No cutscene or transition is forced. Return to Isles to load the changed world.
  Permanent lobby puzzles/doors now share as listed below. Ongoing boss encounters
  remain local except for Army Dillo's bounded damage phases when combat sharing is enabled.

The host confirms guest pickups using game-state readback. Requests retry across
packet loss and short pauses. Guest pickups made offline/before binding, or left
unconfirmed across a new session, remain local and can trigger GUEST SAVE AHEAD;
they are not rolled back or silently transferred to another host.

**COUNTER CONFLICT** means an inconsistent GB flag/count, unsupported pickup
layout, out-of-range balance/upgrade, inconsistent global upgrades or key
prerequisites, or another mod changing a counter during a grant.
Sharing pauses until restart without guessing a repair. Only vanilla setups and
inventory rules are supported: do not combine this with Archipelago, randomizers,
progression cheats or other inventory-changing mods.

Incoming changes request the game's standard save. **SYNCED means supported collectible/progression
ownership agrees in memory; it does not confirm a durable disk write or equal
spendable balances.** Save normally and verify both files after closing/reopening.

## Shared travel unlocks (0.11, untested)

The existing **Shared collectibles and upgrades -> Unique collectibles** option
also enables these unlocks on both peers; no additional setting is required.

- **Bananaport tags:** each of the 87 vanilla tag flags is shared independently.
  Tagging one end does not automatically tag its partner. This covers pads in
  Isles, all seven main levels, Llama Temple and the Castle crypt.
- Incoming tags wait until the recipient is **outside that pad's map**, in
  regular play in a main level, Isles or DK's treehouse. Return to the pad's
  map afterwards so its script reads the new flag. No teleport, pad actor
  modification, tutorial replay or route change is performed remotely.
- Three hidden pads are already enabled by their existing GB flags: Aztec's
  tunnel pad, Galleon's gold-tower pad and Caves' hidden pad. They receive no
  duplicate tag ID and retain the normal GB credit/defer rules. Japes' hidden
  mountain pad has a separate tag; its GB reveal must also be owned before
  the incoming tag can apply. This preserves all 90 vanilla pad behaviors.
- **B. Locker:** clearing any of the eight lobby checks shares its clearance.
  Leave the lobby and resume safe gameplay to send/receive it, then re-enter
  the lobby. The stock actor reads the flag on initialization. The mod does
  not deduct GBs, change the requirement, open the outer lobby entrance or
  make the first K. Lumsy visit unnecessary. Troff & Scoff feeding uses the separate 0.16 accounting path below.

Only actual host ownership or a request from the bound guest can grant these
flags. Existing guest-only tags/clearances can trigger **GUEST SAVE AHEAD**
when upgrading an older test save; there is no automatic merge or reset.

## Shared permanent world unlocks (0.12, untested)

The existing item-sharing option now also covers ten permanent completion flags:

- **Aztec:** freeing the llama and cooling Llama Temple's water. Cooling requires
  the llama rescue flag; receiving cooling alone cannot invent that rescue.
- **Galleon:** freeing the seal, including its saved availability at the race.
- **Fungi Forest:** the five Giant Mushroom gun switches independently, the
  rabbit's first-race completion, and the grown beanstalk.

The recipient must **leave the affected level**, then resume regular gameplay
in Isles, another main level or DK's treehouse. Return to the affected level to
load its changed actors and scripts. Awards earned in interiors/races wait for
the sender to return to an allowed snapshot map, as other progression does.
No loaded NPC or puzzle is patched and no cutscene or map transition is forced.

Only completion flags transfer. Race timers, the temporarily carried bean,
refill drops and other temporary puzzle state remain local. Any Golden Banana
reward keeps its existing ID and numeric-credit rules; these flags do not grant
an extra GB. Day/night and Galleon's reversible water height were outside this
0.12 pass; they use the separate 0.15 path below. Other puzzle/door states and
boss fights remain outside the allowlist. Both players may still need to
open unrelated routes locally. Existing guest-only completions can trigger
**GUEST SAVE AHEAD** on older experimental saves; no automatic merge occurs.

## Shared Factory unlocks (0.13, untested)

The same item-sharing option adds eight permanent Factory flags:

- Entry hatch and the gate into the testing area.
- Production core activation from the Power Hut sequence.
- The storage and R&D breakable metal grates, plus the completed storage
  question-mark box.
- Arcade-lever reveal and the opened crown-pad grate.
- The saved two-coin Arcade replay access after the lever and Arcade GB are
  already owned. Receiving this outcome never deducts the other player's coins.

**Leave Factory to receive these flags**, then return to reload its actors and
scripts. Merely entering the Power Hut or another Factory interior is not enough;
resume safe gameplay in Isles, another main level or DK's treehouse. Progress
earned in an interior is sent when its player returns to a safe snapshot map.

This shares permanent completion only. Machine/platform timing, temporary Kong
production switches and puzzle actions remain local. The Arcade payment result
waits outside Factory and applies only after the shared lever and Arcade GB; it
does not deduct coins or force a game. GBs, Nintendo Coin and the crown retain
their existing IDs and award rules; these new flags do not credit additional
rewards. No cutscene, live grate destruction
or map transition is triggered by a network grant. Other routes/puzzles may
still need local actions. Older guest-only Factory progress can cause
**GUEST SAVE AHEAD**; existing experimental saves are not merged or reset.

## Complete permanent-unlock pass (0.14, untested)

The same **Shared collectibles and upgrades** option covers all remaining
reviewed script-written persistent unlock/completion flags. No new setting
is needed. [WORLD-UNLOCKS.md](WORLD-UNLOCKS.md) lists every new ID, its vanilla
script source and all exclusions. Existing GB-controlled reveals already use
their collectible flags and are not assigned duplicate IDs.

- **Japes:** first gate, shell/hive gate, four huts, painting-cave gate and
  Rambi's wall. The retired standalone gate experiment cannot be enabled.
- **Aztec:** permanent doors/tunnel, beetle tower, five-door-temple activation
  and Tiny Temple's melted ice.
- **Galleon:** permanent gates, ship activation, treasure/fairy chests and all
  five pearls. Pearl ownership is shared individually; the mermaid reward stays
  a separate GB, awarded by the normal game.
- **Fungi:** tunnel gates, pulley stages, mill boxes/conveyor and mushroom-board
  completion. **Caves:** ice walls, igloo pad reveals and saved encounter clear.
- **Castle:** ballroom/tower clears and pad/barrel reveals, crypt/tree routes,
  toolshed box and all six basement grates.
- **Isles/interiors:** training exit, permanent lobby puzzles, Rareware room,
  boulder/barrel reveals and the two Helm-lobby warp tags.
- **Helm:** saved shutdown completion, coin/crown doors and both warp tags.
  This does not share an ongoing Helm timer or partial minigame phases.
- The seven levels' boss portals can share their saved closure once their key
  is owned. These closure flags do not pay Troff & Scoff or synchronize an ongoing boss
  fight. Feeding itself is shared by the separate 0.16 path below.

**Receive ordinary world changes outside their affected level, then return.**
The same safe snapshot maps still apply. Interior-only lobby, training-exit and
Fairy Island changes may arrive in any safe map, including DK's treehouse,
because their scripts are unloaded there. Item pages continue exchanging through
unsafe interiors after each device has one safe snapshot, but received writes wait
for a later safe frame. Isles overworld changes wait outside Isles. Nothing
teleports players or patches loaded actors/scripts.

Requirements are checked both in complete network ownership and against local
flags before applying: closed boss portals need their key; the Rareware room
needs camera/shockwave and all 20 fairies; Helm's coin door needs both special
coins; its crown door needs **any four crowns**. The second pulley completion
needs the raised pulley. Requirements are never consumed or automatically
awarded, and meeting them does not itself invent a completion flag.

There are now **91 shared Bananaport tag flags** (87 original plus four in Helm
and its lobby), with the three existing automatic GB-controlled pads unchanged.
Tags remain independent; no partner pad is activated automatically.

The permanent-unlock pass covers saved outcomes, not every door's live state.
Version 0.15 adds the separate day/night and water-height path described below.
Temporary/timed puzzle steps, shop spending, physical animation timing, tutorial
bookkeeping and the ending cutscene remain local; only its saved final-completion
outcome is shared. **Full campaign co-op is not complete.** New flags may
expose pre-existing guest-only progress as **GUEST SAVE AHEAD**; no automatic
merge, reset or normal-save migration is performed.

## Troff & Scoff feeding (0.16, untested)

Use the existing **Shared collectibles and upgrades -> Unique collectibles**
option on both devices. All five Kongs in Japes, Aztec, Factory, Galleon, Fungi,
Caves and Castle are covered; no extra setting or save migration is required.

1. Connect and wait for **LAN ITEMS: SYNCED** before feeding on the guest.
2. Feed Troff & Scoff normally and let the feeding animation finish. Leave their
   room afterward so your game can capture settled counters in a supported main map.
3. The recipient must leave that entire level for Isles or another main world.
   Return both players to Isles for the simplest exchange. DK's treehouse does
   not apply numeric feeding deductions.
4. Wait for **LAN ITEMS: SYNCED**, then revisit Troff & Scoff. The received fed
   total counts toward their normal requirement; the game opens the door through
   its own initialization. No boss, cutscene or player transition is forced.

Each Kong/level pair shares the **higher cumulative amount fed**, not the sum of
two independent payments from copied inventories. For example, DK feeding 20 on
one peer and 15 on the other converges to 20 on both; the second peer spends only
five additional available bananas. Different Kongs' amounts add toward the level
total. Simultaneous feeding with different Kongs can exceed the door requirement;
those already-made payments are retained, not refunded. Wait for synchronization
between feeding visits if you want to avoid unnecessary payments.

**LAN ITEMS: FEEDING PENDING** means the receiver is still in the affected level,
lacks corresponding available bananas, or is waiting for earlier milestones.
Existing pickup synchronization supplies missing available bananas first; feeding
never creates bananas or makes a balance negative. The rear Japes boulder bunch
now supplies its exact five-banana credit through permanent flag `0x01D`; a
receiver applies it only after leaving Japes.

Incoming deductions preserve `available + fed`, so they do not create extra
banana medals or collected totals. Readback advances acknowledgement and the
existing isolated-save path requests persistence. SYNCED does not certify that
the game's asynchronous save worker has finished writing to disk.

Old guest saves with more feeding than their host can show **GUEST SAVE AHEAD**;
there is no automatic refund or merge. Disconnects preserve already-applied
feeding in each isolated save, but guest-only feeding that never reached the
host can produce that same conflict on reconnect. Back up saves first. Shop
coins, player ammo/health, other boss combat and feeding animations remain local.

## Reversible world state (0.15, untested)

Enable the existing **Shared collectibles and upgrades -> Unique collectibles**
option on both peers; this also enables the three reversible states. No new loader,
extra companion, save migration or device installation is required by the code.
Install both files from the matching platform ZIP with the game closed.

- **Galleon:** either player can raise or lower the water with the normal switches.
- **Fungi:** either player can select day or night with the normal clock switches.
- **Caves lobby:** either player can operate the reversible pressure switch. With
  automatic world refresh enabled on a receiver already in that lobby, it runs
  the pinned vanilla press/release sequence immediately. Otherwise leave the
  lobby to exchange the saved state and initialize it on the next entry.
- Connect and wait for **LAN ITEMS: SYNCED** and **LAN WORLD: SYNCED** before
  operating switches on the guest. Its pre-existing/offline toggle values are
  replaced by host state, not merged into the host save.
- Both peers must have a current reviewed inventory snapshot and be outside
  cutscenes and collectible-credit queues to exchange changes. Ordinary item
  writes still require Isles, a main level or DK's treehouse. With
  **Auto-refresh shared world** enabled, an incoming Galleon water or Fungi
  day/night change may apply from that level's main map; the mod saves and reloads
  the map once through its current entrance. With the option Off, leave for Isles,
  wait for **LAN WORLD: SYNCED**, then re-enter as before. Local switches still
  perform their normal local animations.
- **LAN WORLD: PENDING - LEAVE LEVEL** means a request, revision or save readback
  has not converged. **WAITING FOR BOTH PLAYERS** includes stale connections,
  transitions and unsafe maps. Item-save conflicts suspend this channel too.
- The host rejects a request based on an older revision, including simultaneous
  changes; its newer state wins. A losing request is acknowledged, so it cannot
  keep undoing the host. This is a latest-state channel, not replay of every switch
  action; rapid changes during a readiness gap may collapse to the final value.
- A transport reconnect starts from the host's actual saved-in-memory flags.
  Requests and accepted changes not yet applied to the host are discarded.
  Wait for WORLD: SYNCED before disconnecting. SYNCED confirms state readback,
  not completion of the game's asynchronous disk write.

Only flags `0xA0`, `0xCE` and `0x19D` use this channel. First-use switch cutscenes,
timers, carried objects and other reversible puzzles remain local. Fungi
lighting/actor state and Galleon water physics are not patched
live; their optional reload asks vanilla initialization to rebuild them. The
Caves-lobby switch uses its exact loaded state-2/state-6 sequence and falls back
to a same-lobby reload if object 6 is absent. Existing
permanent-item IDs and isolated `items_host_v6` / `items_guest_v6`
saves are unchanged. Back up these experimental saves before upgrading.

## Expanded enemy and bounded boss combat (0.36; gameplay untested)

Choose the same **Combat experiment** option on both peers, then restart.
**Shots and enemy defeats** shares downward health changes and confirmed kills;
**Shots, defeats and enemy movement** also corrects the guest's enemy positions and
facing toward the host. **Shots, defeats, movement and enemy pose** additionally
corrects a normalized frame within the guest's already-local enemy clip. Both
peers must choose that fourth option; the exact enemy identity, reciprocal life
tokens and a compact hash of the clips already loaded on both peers must agree.
The network never selects or loads an animation clip.
Each reciprocally bound clip/frame sample is evaluated at most once. Local enemy
animation continues between network updates instead of holding the last sample.
Both enemy copies must be loaded and linked while alive before health or kills can transfer.

The supported kinds are blue/gold beavers, regular Kremlings, Klumps, Mushroom
Men, Robo-Kremlings, Koshas, both Zinger variants, Robo-Zingers, bats, all five
Kasplats, Shuris, Gimpfish, all three Klaptraps, Krossbones, Kaboom, Klobber,
Pufftup, the sheeted Kritter, Ruler, Book and Toy Monster. They use
their own local death animations and behavior. Klump and Kremling commands wait
until knockback ends. Flying enemies keep their local flight AI and attacks;
position correction does not synchronize their full flight path or animation.
Kasplat blueprint drops remain local engine events, while the existing item
channel shares the resulting blueprint ownership. Kasplat remote deaths wait
through knockback state 0x32 rather than bypassing their normal handler. Shuris
and Gimpfish retain local swimming, attacks, animation and death effects. Klaptrap
skeleton remnants and detached Krossbones parts remain local secondary actors.

Combat is eligible in 58 reviewed vanilla maps: the seven main levels; ordinary
enemy interiors in Japes, Aztec, Galleon, Fungi, Caves and Castle; Hideout Helm;
DK Isles; and four lobbies that contain supported enemies. This includes the
Fungi Spider room and Chunky's Caves igloo. The ordinary-enemy channel excludes
boss arenas, races, bonus games, crown battles and reward controllers; Army
Dillo and Dogadon use the separate bounded channel below. Giant Clam and tomato
encounter controllers have no ordinary defeat path and remain local with all other
unlisted kinds. Book and Toy Monster use their pinned vanilla disappearance states
rather than a fabricated health-based death.
If another mod has replaced any supported behavior handler before initialization,
enemy synchronization is disabled without replacing any of those handlers.

Every supported live or acknowledgement-pending enemy is now covered by compact
20-record pages. The game captures pages in stable spawner order while the native
bridge caches them by enemy key; actual UDP sends rotate duplicated pages independently
of render-frame timing. Confirmed records retire only after both peers report the
same bound defeat. There is no historical kill replay or remote spawning. Partial health converges to the
lower value accepted by the host; values can never heal an enemy. Damage amounts,
hit reactions, drops and player health/ammo remain local. Guest hits are
speculative until the host reads back the lower health, with no rollback.

Army Dillo uses a separate four-word boss record in maps 8 and 196. Both peers
must enable combat and enter the same Dillo arena. A real local TNT impact
advances the overlay's private phase counter; the host merges the higher observed
phase, then each receiver enters the pinned vanilla collision-result-4 path one
phase at a time. Commands wait through the vanilla `0x4D` hit reaction and cannot
replace another local collision. The mod never writes the phase byte or script
index directly. When both peers select enemy movement, the guest corrects Dillo's
position and facing toward the host. AI decisions, attacks, missiles, barrel actors,
animation, sounds, cutscenes and player damage remain local.
If another mod owns the pinned Dillo handler, this boss channel disables itself.

Dogadon uses the same four-word record in maps 83 and 197, but its vanilla
private counter only records the first three TNT impacts. The fourth impact
enters terminal control state `0x56` without incrementing that counter, so the
mod exposes a bounded synthetic fourth wire step only after observing that exact
vanilla state. A raw private-byte value 4 also belongs to an unrelated vanilla
timer/fallback script; the boss channel rejects it rather than treating it as
damage progress. Receivers still advance exclusively through Dogadon's pinned
collision-result-4 handler, one impact at a time, and defer during state `0x4D`.
The mod never writes Dogadon's hit counter or script index. Movement, flight,
fire attacks, Hunky Chunky/TNT barrel actors, arena sinking, animation, sounds,
cutscenes and player damage remain local. A conflicting Dogadon handler disables
this channel without replacing it.

## Reviewed host-follow transitions (0.39; gameplay untested)

As of 0.40.1, this is disabled by default. **Transition behavior -> Independent
exploration** ignores host tickets and permits different maps. Selecting **Follow
host** on the joining device enables the behavior below; the host setting is
ignored because hosts always publish the bounded route record.

When both peers are connected in the same eligible source map, an opted-in Join
player follows the Host through 169 reviewed ordinary vanilla routes across DK
Isles, the level lobbies, portals, tunnels, and ordinary interiors in all seven
levels. The scripted Isles cannon into Castle lobby remains excluded.

While connected, the host publishes a monotonic ticket and a packed source-map,
destination-map and signed-exit tuple for 300 game frames. This lets a guest left
in the source receive the route after the host has loaded. Disconnecting clears
that retained route. A guest accepts a new ticket only
while already in that source map, connected, active, in normal adventure play,
outside cutscenes and with no local transition running. It then calls DK64's
pinned two-argument transition function. A successful call consumes the ticket;
disconnects reset ticket history, and leaving the source cancels a pending one.

The decoder rejects every tuple outside the reviewed list. Hideout Helm, the
Castle cannon, K. Rool, boss portals, Troff & Scoff, races, bonus barrels,
cutscene maps, menus and transition variants with extra parameters remain local.
If either player takes a different route at nearly the same time, the map
mismatch is left alone rather than being overwritten later.

## Other implemented features

- Two participants, nonblocking 20 Hz UDP, bounded receives, checked packets and
  emulated-memory spans. Stale presence hides after 750 ms; sessions time out
  after three seconds and can reconnect. Protocol/native ABI v53 rejects old peers.
- Remote Kong position/facing, main skeletal pose and frame interpolation for
  all five Kongs. Proxies are inert: no second engine-controlled local player.
- Optional gun/orange projectile visuals and hand/weapon visibility. Locally owned
  projectiles remain advertised for six capture frames after teardown so a fast
  shot cannot disappear entirely between 20 Hz sends.
- Optional linked partial health and defeats for the thirty-four enemy kinds above in the 58
  reviewed combat maps, plus optional host position/facing and guarded normalized
  pose corrections. Enable the same combat option on both peers and wait for the
  enemy-link HUD message.
- Optional bounded damage-phase synchronization for the five standard bosses and
  all five K. Rool rounds through pinned vanilla reaction paths. Tiny's hits are
  committed by the foot actor in the shoe map. Other boss state remains local.
- The first Japes gate is part of ordinary item/world sharing. The former
  standalone gate experiment is retired and its old save files are ignored.
- Proxy cleanup across disconnects, map/Kong changes and actor destruction;
  normal save isolation is selected before the game's EEPROM worker starts.

Enemy AI, attack states, hit reactions and attack timing still run locally. Pose
correction suppresses animation callbacks while evaluating the already-local clip;
it does not synchronize attacks, sounds, effects or damage. Enemy health
from the guest is speculative until host acceptance; there is no rollback. Other enemies, boss behavior outside the bounded phase adapters, enemy drops, shared
player damage/ammo, secondary animation layers, sounds/effects, other puzzles/doors,
transitions outside the 169-route reviewed ordinary-route allowlist
and world progression outside the allowlist are **not synchronized**. The remote
shot visuals cannot themselves hit enemies or switches.
Full Tag Anywhere compatibility is not established.

## Validation history and gameplay checklist

**No test suites, LAN probes or game sessions were run for 0.21.** Only the
mod and the Android/Windows native libraries were compiled and packaged.

Earlier version 0.10 verification covered the actual production inventory adapter with a
recording engine: every GB mapping, all 1700 ordinary pickups, all balloons and
rainbow coins, exact credits, canceled writes, queued local pickups, loaded-level
deferral, local-first races, medal derivation, bounds, unrelated-inventory
sentinels, stale/missing pages, loss/replay, reconnection and native ABI bounds.
The added progression adapter checks cover all 50 new IDs, rescue/Isles
deferral, canceled flags, local training prerequisites, tier/global-bit semantics,
key dependencies across wire pages and full inventory preservation. Eight more
IDs cover training/camera: safe-map restrictions, first-slam ordering, all
partial training combinations, completion derivation and unchanged resources.
For 0.10, the seven suites ran on Windows and the tablet; Linux used ASan/UBSan.
That version's tablet-host/Windows-guest **synthetic** LAN probe exchanged all 2199 IDs. It does
not load the ROM or exercise real pickups. Package verification checks the actual
Android importer, same NRM on both platforms, 16 KiB ELF load alignment and native
dependencies. Historical results: `build/verification-v0.10.0`. The v0.46 full
release record and artifact hashes are in `build/build-v0.46.0/build-status.json`;
the distributable hash list is `dist/SHA256SUMS.txt`. Hashes are kept outside
the source documentation so the source ZIP never contains its own checksum.

Version 0.10 actual gameplay testing connected the tablet host and a fresh
portable Windows 1.0.1 guest. Both displayed the other Kong and ITEMS: SYNCED
in DK's treehouse. The tablet visited Cranky to unlock the training barrels,
then returned to the treehouse. Windows received flag 0x17F without leaving
the treehouse or visiting Cranky, and its isolated save changed from absent
to present with valid slot checksums. The tablet then completed the diving
course and returned; Windows received and saved flag 0x182 without entering
a training barrel. Both games were then restarted and returned to SYNCED;
the Windows save still contained both flags with valid checksums. Android's
activity log confirms its game process restarted. This is a limited training
save/reconnect smoke test; offline reloads, use of the received abilities and
other rewards remain unverified.
Local evidence is recorded in `build/verification-v0.10.0/live-gameplay.json`.
Earlier prototype testing also exercised reconnects and remote Kong movement.
Windows-host inbound connectivity was previously blocked/suspected filtered;
use the tablet as host in this environment. No firewall setting was changed.

Before relying on a save:

1. Test a GB, single banana, bunch, ordinary coin, balloon and rainbow coin in
   both directions. Leave the reward's level on the recipient and check the
   correct Kong/level counters, including all five balances for a rainbow coin.
2. Collect the same location on both devices before leaving. It should count
   once per save. Re-enter: the already-collected object must not reappear.
3. Complete training courses on each client and return to DK's treehouse. Check
   course removal on re-entry, ability use, first-slam sharing and no downgrade.
   Unlock camera/shockwave, leave Fairy Island, and check both abilities after
   obtaining local film/crystals. No remote refill should occur.
4. Buy moves, guns, instruments, global upgrades and Candy melon upgrades from
   both devices. Check the correct Kongs, use of each move, retained coins,
   unchanged current health/ammo and no charge when merely receiving an upgrade.
   Feed Troff & Scoff on either peer, leave the level and receive its cumulative
   feeding on the other: preserve available-plus-fed and avoid duplicate deductions.
   Check a remote 75-banana medal.
5. Rescue each Kong, turn each key and finish K. Lumsy's sequence. Leave the
   recipient's affected level/Isles, return and check cages/lobbies/padlocks.
   Verify no duplicate GB, repeated scene or missed progression.
6. Lock/unlock, leave/rejoin, transition maps and reconnect. Confirm no repeated
   reward and no writes during cutscenes/collection queues.
7. Save, fully close, reopen and check persistence on both clients. Check the
   ordinary non-mod saves remain unchanged. Also test Android-to-Android.
8. Tag pads separately on each peer, leave their maps to receive, then return.
   Check paired-end requirements, the hidden GB-controlled pads, Llama Temple
   and the crypt. Clear each B. Locker, leave/re-enter its lobby and verify the
   other client receives clearance without changing GB balances or outer doors.

These real-game checks remain outstanding; native/synthetic tests cannot certify
the renderer, collision engine, scripts, performance or durable saves.

## Build

The separate `coop/` project uses the prepared parent `../upstream` pinned DK64
symbols/structs and recomp ABI. It does not change or rebuild the game loader.

- MIPS NRM: `bash coop/tools/build-mod.sh` (WSL).
- Windows companion: `bash coop/tools/build-windows.sh` (WSL).
- Android companion: `coop/tools/Build-Android.ps1` (PowerShell).
- Linux: `cmake -S coop -B coop/build/linux -DCOOP_SANITIZE=ON`, then
  `cmake --build coop/build/linux` and `ctest --test-dir coop/build/linux --output-on-failure`.
- Run the seven `coop_*tests` programs from the appropriate platform build folder.
- Package: `python coop/tools/package.py`.
- Verify metadata: `python coop/tools/generate-collectibles.py LOCAL_DECOMPRESSED_US_ROM --check`.
  Generation records IDs/counter destinations only; it does not copy game assets.
- Regenerate vanilla travel metadata: `python coop/tools/generate-travel.py LOCAL_DECOMPRESSED_US_ROM`.
  This emits tag IDs, map/object identities and the existing GB dependency only.
- Optional finite synthetic LAN probe:
  `coop_peer host|join HOST_IPV4 PORT ROOM SECONDS items` (1-120 seconds).

Toolchain versions/notices are in `THIRD_PARTY.md` and `licenses/`.
