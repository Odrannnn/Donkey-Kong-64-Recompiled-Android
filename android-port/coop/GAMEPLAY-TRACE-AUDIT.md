# Gameplay blocker audit

Audit date: 2026-09-04
Co-op version audited: 0.79.0
Protocol version: 79

## Runtime evidence

The retained gameplay captures do not contain a usable transient blocker tuple.
The latest two-Windows capture contains forty unanswered trace probes and no
JSON response. The older live-gameplay capture predates transient reporting and
only proves that permanent item synchronization was active. It cannot identify
a temporary object, its map, or the activation edge that failed.

Consequently, none of the candidates below is labelled as reproduced from a
runtime trace. They come from a static audit of the pinned US ROM setup scripts
and must be confirmed during gameplay before an adapter is added.

The old trace schema only reported the aggregate transient status and game
status. Schema 2 adds the local, remote, and merged transient contexts and their
record arrays, plus both players' transition routes. A future trace can now show
the exact map, room epoch, record kind, object key, state, and value that either
client published or applied.

Capture both clients while reproducing a blocker:

```text
python tools/query_trace.py --ip <host-ip> --watch 0.5 > host-trace.txt
python tools/query_trace.py --ip <peer-ip> --watch 0.5 > peer-trace.txt
```

The query tool scans the supported private-LAN trace ports. Start each capture
before operating the mechanism and keep it running until both clients have
either passed the object or observed the permanent result.

## Resolution in 0.79.0

All five highest-priority groups and all eleven conventional rows below now use
typed trigger adapters. Castle Lower Cave and the Japes-lobby Wrinkly doors keep
one accepted leaf activation for the current room epoch so a late peer can run
its own timed opening exactly once. Controllers, linked doors, timers, cutscenes
and rewards are never copied.

The Japes breakable X panel now has a dedicated three-step sequence adapter. It
maps only `1 -> 2`, `3 -> 4`, and `5 -> 6` to logical progress and rejects the
unrelated `20`, `30`, and `50` availability states. Each receiver advances only
its next locally ready step.

The game-adapter regression exercises every new map/object/state tuple, rejects
wrong readiness, checks replay idempotence and verifies the two room-epoch
latches. These checks validate the boundary but do not replace two-device game
testing.

## Implemented highest-priority mechanisms

These use narrow, locally valid activation edges. They do not require driving a
room controller or copying an arbitrary object state.

| Area | Map | Object | Candidate activation | Safety boundary |
| --- | ---: | --- | --- | --- |
| Castle Tree | 164 | `0x05`, `0x06`, `0x07` | exact state `11 -> 12` | Never drive controller `0x04`; each target owns its timer while the controller owns sequencing and the reward. |
| Castle Lower Cave | 183 | gun switches `0x05` through `0x09` | exact state `1 -> 2` | Never sync doors `0x03` and `0x04` directly; the local switch scripts open them for 400 frames. |
| Castle Shed | 166 | question box `0x05` | exact state `10 -> 11` | Vanilla owns the 100-frame break sequence and permanent flag 321. |
| Caves Diddy Upper Cabin | 200 | thin flames `0x06` through `0x08` | exact state `1 -> 2` | Never drive controller `0x05`; it owns completion, reward, and flag 293. |
| Japes Lobby | 169 | Wrinkly doors `0x00`, `0x02` through `0x05` | exact state `1 -> 2` | Keep availability and character checks local; reject raw states `0` and `20`. |

Castle Lower Cave uses a room-epoch latch so a late-arriving peer receives the
current 400-frame opening without repeatedly restarting the local scripts.

## Implemented conventional mechanisms

These appear to expose a self-contained switch or break activation but have not
yet been observed in a gameplay capture:

| Map | Object | Candidate activation |
| ---: | --- | --- |
| 7 | `0x1A`, DK green slam | `1 -> 2` |
| 12 | `0x02`, `0x03`, Tiny green slams | `1 -> 2` |
| 13 | `0x02`, `0x03`, wooden legs | `1 -> 2` |
| 16 | `0x1A` slam; `0x7C` triangle | `1 -> 2` |
| 20 | `0x0E` through `0x10` slams; `0x15` trombone | `1 -> 2` |
| 114 | `0x04` red slam; `0x06` coconut | `12 -> 13`; `1 -> 2` |
| 163 | `0x0C`, trombone | `1 -> 2` |
| 174 | `0x0A`, Chunky slam | `1 -> 2` |
| 175 | `0x0E`, box | `10 -> 11` |
| 194 | `0x00`, `0x01`, ice walls | `1 -> 2` |
| 193 | `0x00`, iron bars | `1 -> 2`; lower confidence because persistence is unclear |

## Implemented bespoke mechanism

The Japes map 7 breakable X panel, object `0x117`, exposes several logical
progress edges: `1 -> 2`, `3 -> 4`, and `5 -> 6`. States `20`, `30`, and `50`
are availability waits rather than network progress. Its purpose-built
monotonic adapter never treats the largest numeric state as the most advanced
state.

## Compound controllers kept local

The following objects remain local after auditing their vanilla instance-script
blocks. They are not missing leaf adapters:

- Aztec Lobby map 173 object `0x04` combines Wrinkly presentation, collision,
  availability, timed resets and two distinct `50/51` and `60/61` routes. It has
  no safe network-owned progress value.
- Fungi Lobby map 178 object `0x09` owns the five-gun order, texture updates,
  cutscene, 300-frame timer and permanent flag `0x195`. The permanent result is
  already applied live; copying a numeric state would skip the block that owns
  those side effects.
- Caves map 72 object `0x32` owns a rotating-room transition, camera, timer,
  player action and minigame dispatch. It remains a local simulation.
- Factory map 26 object `0x0C` continuously alternates platform motion states
  `12/13`; network correction would fight the vanilla oscillator every frame.
- Aztec Lobby map 173 objects `0x13` and `0x14` duplicate the encounter sequence
  that writes flag `0x192`. The permanent platform/result already converges via
  the audited live-world consumer.
- Castle Tree map 164 controller `0x04`, Caves Upper Cabin map 200 controller
  `0x05`, and Castle Lower Cave map 183 doors `0x03` and `0x04`; only their
  audited leaf triggers are synchronized. Their local controllers now observe
  those triggers and retain ownership of sequencing, timers and rewards.

No remaining statically identified object both blocks the peer and lacks either
a typed leaf adapter or a live permanent-result consumer. A new concrete trace
may still reveal a setup variant that this static audit could not observe.

## Acceptance rule

For future reports, add an adapter only when a two-client capture shows that the
peer is physically blocked or misses the permanent result. Replay the smallest exact vanilla
activation edge on the receiving client, preserve room ownership of timers and
rewards, and reject unrelated readiness, availability, and controller states.
Each mechanism should land as a separate feature commit with a game-adapter
regression test.
