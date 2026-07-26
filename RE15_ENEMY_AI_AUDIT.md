# Enemy-AI Divergence Audit + Fix Campaign (2026-07-26)

**Trigger:** user report "deviations from the original everywhere in the enemy AI (crows, zombies, …)".
**Method (lesson.txt as law):** workflow audit wf_827f186d — one finder per enemy type comparing the
port state-by-state/constant-by-constant against the raw STAGE1.BIN/PSX.EXE disasm (decompiles
cross-checked, ≥10 existing @0x citations re-verified per type), every claim adversarially verified
(refuters, default-unproven). **Result: 88 claims → 81 CONFIRMED, 2 refuted, 5 resolved inline.**
Fix campaign wf_ace63747: sequential waves per type, each ctest-green + committed, final review PASS
(spot-checks re-disassembled from the binaries, zero guess-tell language).

## Wave commits

| Wave | Commit | Fixed | Notes |
|------|--------|-------|-------|
| W1 zombie-live | 919b5b61 | 6 + 2 citations | HIGH: TURN brace bug (face-snap every frame); downed-HURT stub (brick-forever → byte-true flinch handlers FUN_80106a38/0x801068a0 raw-disasm'd end-to-end) |
| W1b addendum | b4411125 | 1 | HURT torso-bend cadence: audit said 7 ticks, disasm arbitration → **6** (@0x80105dbc-dd0 falls through) |
| W2 dog | 139fbb10 | 15 (7 HIGH) | 8-phase BITE leap machine, airborne HURT/DEATH tables @0x80121018/70, pack-alert aca50, CHASE constants, reroute hop + floor change, pounce-land; death-detour claim CONFIRMED+implemented |
| W3 maggot | 099768aa | 17 (3 HIGH), 1 OPEN | Sub-15 REAR-UP GRAB/PIN (B[15] 0x8011a960), full HURT 3-lane FSM, sighted clip 5, real root-motion locators (FUN_8011bf50/c024), mid-air finisher + crit flag in re15_damage.c |
| W4 crow | dd1be65e | 16 + 2 claims | crow_mode = fresh RNG/tick (@0x80112028), staged LOS probe, perch-return FUN_80115f00, per-tick floor ref, pad-mash grab drain, screech tiers; wall-pass + player-down dive CONFIRMED+implemented |
| W5 zombie-girl | 0a3244c2 | 8 (2 HIGH) | mode-0 FSM via girl tables @0x80120230/264/2a8 (word-diff-verified vs standard), grab kill-window → devour handoff (player dies only via hp<0), death/hurt FSMs, invented advance-30 removed |
| W6 spider | 20203ff7 | 7 (2 HIGH) | invented DEATH removed (original: hp<0 = flinch, **unkillable**), unconditional root x/z-pin + y-home-step, per-overlap -2 contact, HURT phase--/windup/SE; body-push claim partially refuted (call pushes the SPIDER @0x80116368) |
| W7 npc | 6e345ff7 | 9 (2 HIGH) | Plc_dest → shared executor (not the player walker FSM), walk/turn arrival transitions + arrival bit, 24-entry sub-VM table (sub 19 SCD-invoked), watcher 0x8004f100 wired, per-type stride-2 [speed,cone] tables @0x80076c00/c80 |

All fixes carry `@0x` citations in code + commit; tests extended/rewritten where they pinned the old
wrong behavior (e.g. test_room1140_combat 14b, spider death). 96/96 ctest.

## HONEST-OPEN list (documented in code with exact bytes — NOT faked)

- **zombie-live:** 25% INIT spawn-variant roll @0x801008b0-8f0 (8 variant fns via @0x8011f7ec write
  model-pool render/tint fields — pool+0x158|=0x480, +0x1e0/+0x1e4, +0x1f0=0x8080e0 — the port
  renderer has no per-instance tint/variant channel). Lying decide/animate deep rows (@0x8011f9d8/d4
  wake rows [5]/[6]=0x80104b38/40) beyond the now-working shot-awake path.
- **dog:** entity word0 flag bits (0x80012974 proximity 0x20000000; corpse |2|0x40); +0x1c0=0x8000
  hit-tween latch; FUN_80111870 skeletal root-motion assist; +0xb0 shadow aux; 0x80037edc SE-tier
  accumulator; +0x90 contact-class bits from FUN_8003b0a4 (reroute-14 drop-class).
- **maggot:** Path-A ZONE leaps (#11) — FUN_8003b93c (SCA attr 0x10/0x20) + FUN_8003b0a4 escape-heading
  writer; B[7] launch variants implemented but dormant.
- **crow:** entity[0] top-byte vert-band flags (@0x80112560-c8 / EXE 0x80012a0c/0x80012974);
  0x80115f70 shadow/targetability helper; FUN_8001c6e8 entity-overlap arm; the player command FSM
  producers (aca58 cmds 2/5/3, aca52-bit0 knockdown, FLIGHT-2 SPIN aca5a @0x801120a0-e8; dispatch
  @0x80073f90) — the crow side consumes them byte-true once produced.
- **zombie-girl:** +0x1c0 anim-status writer (mode-0 anim-interrupt @0x8010b6e4-b708 cannot fire —
  port-wide gap shared with the standard zombie); +0x0 lifecycle word; FUN_80012aa4(0xbb8)/
  FUN_8001bd60 aux; state [4]=0x8010919c idle.
- **spider:** web-spit model-instance hit-code choreography (#6: 0x80019d50 arms 0x22/0x12/0x23,
  part tables @0x8010031c/334/34c/364, mouth spawn @0x80116d44-88) — generic ESP fx placeholder
  documented in code.
- **npc:** obstruction probe FUN_8002d7d8 (recovery FSM itself ported); footstep FX 0x80045630 +
  sub-19 root-motion (needs the EDD 0x4000/0x2000 flag channel); executor rows [10..17] + [23]/[24]
  step-up; state-1 deep decide/act rows ≥3; +0x1c8/+0x1ca turn-rate stash (no consumer).

**Regenerate the finding set:** the full verified JSON lives in the session scratchpad
(`ai_divergences.json`, 81 entries with byte evidence per side); the audit/fix workflow scripts are
under the session `workflows/scripts/` dir (`enemy-ai-divergence-audit-*.js`, `enemy-ai-fix-waves-*.js`).
