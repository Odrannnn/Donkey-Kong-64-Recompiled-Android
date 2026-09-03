# Tag Anywhere compatibility

Audit date: 2026-09-03  
Co-op version: 0.78.1
Tag Anywhere source: 1.0.1, commit
`b7e009133ba3d716a0d214a5fe0131b828f35e18`

## Source-level result

DK64 Tag Anywhere and LAN co-op are compatible at the source and mod-loader
boundaries reviewed here. Tag Anywhere registers one
`dk64recomp_every_frame` callback. It does not replace a game function, install
an original-function hook, replace an actor behavior, select a save path, or
subscribe to the flag, map-load, or EEPROM-load events used by co-op.

The runtime supports multiple callbacks for one event and executes them in the
configured mod order. Game-memory writes survive between callbacks. Therefore
co-op observes either the complete state before a tag or the complete state
after it; changing mod order can change the observation by one rendered frame,
but does not suppress either callback.

Tag Anywhere changes `current_character_index[0]` and the player actor model in
the same vanilla `func_global_asm_806C8E58` call. Co-op accepts all five normal
Kong indices, sends the local index in every player snapshot, and recreates the
remote visual proxy when that index changes. The two peers may use the same
Kong or different Kongs, and Tag Anywhere may be enabled on either peer or both.
It is not a wire capability and does not need matching settings.

Co-op progression is stored and synchronized per Kong. A remotely received
Kong rescue or move is visible to Tag Anywhere through the normal loaded save
data on the following frame. Tag Anywhere does not choose or copy a save, so
co-op's isolated campaign selection before EEPROM load remains authoritative.

The audited Tag Anywhere source also compiles and links with the project's
pinned LLVM 21 MIPS toolchain against the same prepared DK64 1.0.2 headers used
by co-op. The result is a 32-bit big-endian MIPS-II ELF. Its three compiler
warnings are existing signed/unsigned loop comparisons in HUD bookkeeping; no
missing symbol, layout, section, or linker incompatibility was found.

## Co-op hardening

Co-op now requires the local player actor model to agree with
`current_character_index[0]` before advertising an active player. This rejects
a partial swap if a future tag implementation spreads the model and index
updates across frames.

The combat adapter retains a vanished local projectile for six rendered frames
so the 20 Hz network sampler can observe fast shots. A Kong change now clears
only those retained local samples. This prevents an old Kong's gun or orange
visual from briefly appearing beside the new remote Kong model. Live game
projectiles already make Tag Anywhere reject a tag; enemy state, room state,
items and remote projectile actors are not reset.

## Compatibility matrix

| Scenario | Source-level status | Result |
| --- | --- | --- |
| Tag Anywhere on Host only | Supported | Join receives Host's changing Kong in ordinary presence snapshots. |
| Tag Anywhere on Join only | Supported | Host receives Join's changing Kong; no host authority is needed for the choice. |
| Tag Anywhere on both peers | Supported | Each player changes independently. Simultaneous and duplicate Kong choices are valid. |
| Independent areas | Supported | Kong choice does not affect cross-map item/world exchange. Remote avatar and combat remain same-map features. |
| Follow Host transitions | Supported by separation | Transition tickets contain map and exit, not Kong choice. Tag Anywhere blocks tags during loading-zone fade and locked portal states. |
| Shared rescues and moves | Supported | Tag Anywhere reads the ordinary rescue flags and per-Kong progression that co-op updates. |
| Isolated co-op campaigns | Supported | Tag Anywhere does not alter the selected EEPROM/save path. |
| Gun/orange visuals across a tag | Hardened | Co-op discards retained old-Kong samples when it observes the new coherent Kong. |
| Animal transformations | Intentionally hidden | Co-op advertises only the five normal Kongs; Tag Anywhere rejects tags during transformations. |

## Remaining gameplay risks

This is a source audit plus adapter tests, not a completed two-device gameplay
certification. Tag Anywhere's own guards block shops, bosses, Baboon Blast,
most locked minigames, cutscenes, reward dances, transformations, live
projectiles, instruments, loading-zone fades, and many constrained movement
states.

It deliberately permits tagging during some ordinary attacks, while carrying
objects, and in several puzzle interiors. Co-op combat is safe because enemy
identity and health are not keyed to the attacking Kong. Temporary puzzle
controllers remain the main uncertainty: tagging in the middle of a locally
owned carry, placement, switch, or scripted sequence may expose a vanilla or
Tag Anywhere issue even when no network state is involved. Each reviewed co-op
adapter still validates its own exact local object and state before applying a
peer command.

## Gameplay certification checklist

Run the following with matching LAN co-op packages and Tag Anywhere 1.0.1:

1. Enable Tag Anywhere on Host only, tag through all five unlocked Kongs, and
   verify the Join proxy changes once per tag without disappearing permanently.
2. Repeat with Tag Anywhere on Join only, then on both peers simultaneously,
   including both players choosing the same Kong.
3. Tag with the gun holstered and drawn, fire each Kong's weapon, throw an
   orange, and confirm no old projectile follows the new proxy.
4. Receive a Kong rescue and shop move over LAN, then select that Kong without
   changing area and verify the move and HUD values.
5. Collect a Kong-specific banana, coin, blueprint and Golden Banana after a
   tag; wait for item synchronization and verify both save copies.
6. Exercise an ordinary same-area gun switch, slam switch and instrument pad,
   tagging immediately before activation and after the local script finishes.
7. Verify independent-area play, a normal Bananaport, and optional Follow Host
   transitions after each player selects a different Kong.
8. Disconnect and reconnect while the players use different Kongs; verify the
   remote proxy returns with the current choice.
9. Save, quit, and reload the isolated campaign with both mods enabled; verify
   vanilla saves remain separate and the selected co-op campaign is unchanged.

Any failure should be captured from both clients with `tools/query_trace.py` and
reported with map, Kong before/after, control state, action, and which mod ran
first in the mod list.
