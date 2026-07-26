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
| W3 maggot **= the GORILLA BOSS** | 099768aa | 17 (3 HIGH), 1 OPEN | Type 0x27 IS the ROOM11C0 parking-garage boss (TCRF: MAIN16.BGM "Gorilla Boss fight"; "maggot" = BioModels fan label for EM027). Sub-15 REAR-UP GRAB/PIN (B[15] 0x8011a960), full HURT 3-lane FSM, sighted clip 5, real root-motion locators (FUN_8011bf50/c024), mid-air finisher + crit flag in re15_damage.c |
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

## Boss / late-stage wave (2026-07-26) — audit wf_efd92a2c, fix waves wf_23c59f5e/wf_64bfa717/wf_46cb1c14

A second campaign covering the 8 late-stage/boss types the STAGE1 sweep did NOT touch (the user asked
about the Birkin boss). Same method: adversarial audit (8 finders vs raw STAGE2-5.BIN disasm, refuters)
→ **107 claims, 66 first-round CONFIRMED, the rest verify-then-fixed** (adult-spider 25/25 + ivy 9/9
confirmed in the fix wave after the first-round refuters died on a credit limit). **0 refuted overall.**
Each type = one byte-true wave, ctest-green + committed. The two most surprising reversals were
independently re-disassembled by the main loop before accepting.

| Type | Commit | Headline divergence fixed |
|------|--------|---------------------------|
| **G-BIRKIN 0x30/0x36** | 78d2b2dc | **The boss had NO phase-2 mutation.** Root (0x80116230) forces state 0x601 at hp<100; sub-6 (@0x80117f80) is the mutation set-piece (HP reset 150, grid\|=0x10 = form-2 attacks/speed, ESP 0x1d) — the port's sub-6 was a no-op mislabeled "throw follow-up". Rebuilt the whole family: root low-HP driver, HURT super-armor (+0x1dc) + mutation-revive (+0x1dd&8→HP=50 @0x8011a3f0), DEATH morph-tail (wait grid&0xf==2 → clip 0xc → sub-11 run-off to (-22000,-12000)). Port boss no longer dies single-phase at 0/300. |
| **TYRANT 0x2b** | bb32ae3c | 16 findings: missing rear-grab (facing-select sub 5/9), real GRAB1/2 budget+mash timers, ATTACK1 window (frame≥10 not inverted 3..9), stagger→charge pipeline, two-stage death. |
| **COCKROACH 0x29** | 95d99f78 | Port had **NO flee/flight**: HURT→fly-away (subs 7/8/9), leap-launch, HEAVY window {21..24,33,34}, SCURRY speed 180-211 re-rolled/frame (was rolled once >>3 = 8× slow). The STAGE1-flagged "advance-30" **was invented** — removed. |
| **ALLIGATOR 0x23** | 66e39ff3 | Port **INVENTED a "100-frame jaws-hold → unconditional pl->hp=-1 swallow"** — no such loop/write exists in 0x8010c448. Replaced with byte-true bite = KNOCKDOWN aca58=2 (eaten aca58=3 is bgez-gated on hp<0); restored the 11 missing sub-brains. (Model is a known data gap — AI only.) |
| **WRITHER 0x1a** | 38b0a4b9 | Roster doc said "unkillable/harmless" — **WRONG**: INIT (0x8010c33c) installs a 300-radius damage box (+0x78) and writes NO HP → spawn HP=0 → any shot → real DEATH (clip 3 + gore) handler. Self-verified vs disasm. |
| **DORMANT 0x22/24/2a/31-35/1c-1f** | 2d6e49a2 | 4 doc/routing corrections, 3 OPEN (kept stub/unrouted where byte-true). |
| **ADULT-SPIDER 0x25** | 5afc5a2a | Port had **invented almost the entire ACTIVE brain** (dual-table A/B FSM @0x80118e44/e64). 24 findings fixed: ROAM/APPROACH cascades, GRAB-A pin (grid:=0), SPIT-7, SCAN latched ±0x18, stuck-ctr +0x1dc, INIT pose cluster, kill-flag; + a latent LOS-latch bug found in-flight. |
| **IVY 0x2d** | f879c7c0 | Port had **invented an entire chase/grab/instant-kill/hurt engine by mis-reading the neighbouring 0x40 NPC root**. The shipped ivy is a **dormant, weapon-immune scripted PROP** — state table @0x8011a2c0 = only 2 code ptrs (INIT/ACTIVE), ACTIVE (0x801169b8) is flag-gated bone-sway with zero player-read, dmg row @0x8006f048 = all-zero. Self-verified vs STAGE4.BIN. Removed the invented engine + made it non-targetable. |

**Honest-OPEN (documented in code with exact bytes, none faked):** the room-collision LOS raycasts
(+0x1d0&1 / FUN_8001bc08) unavailable in the collision-less AI harness (range+arc proxies at every
site); per-bone attack spheres (0x8001bff8) proxied by frame-window + arc; the aca58/aca59 player-
command victim FSM is unported port-wide (grabs drive the player via the shared s_player_grabbed +
devour); SCD-triggered set-pieces (Birkin death grid==2 morph, alligator submerge water-level); model-
data gaps (alligator EM023 absent from CDEMD0.EMS) → clip lengths/root-motion proxied; render-side
gore/tint/sink. Birkin/Tyrant/Ivy/Alligator have NO PSX savestate here (STAGE2-5) → not live-parity'd;
their mechanics are pinned by the raw disasm + the type ctests. Fresh DuckStation captures via
`re15-room-capture` would extend the live table.

## Live parity check vs PSX savestates (2026-07-26, post-campaign)

Method per `re15-parity-verify`: same room, same walk, compare state trajectories (port
`RE15_STATE_LOG` vs `re15_enemy_state.py` on the DuckStation saves). Results:

| Check | PSX ground truth | Port live | Verdict |
|-------|------------------|-----------|---------|
| ROOM1140 entry roster | `room1140_entry.sav`: lyer 0x16 g=0x88 sub0 + 4 feeders sub 0xc, dists 7419/6122/7038/8748/8046 | 5/5 slots field-identical, **distances to the unit** | ✅ EXACT |
| ROOM1140 combat chain | `mzd_stage1_walked.sav`: woken feeder → +0x5=6 devour, second at ENGAGE, player state 7 hp=−1 | wake at d=2974 (<3000 gate) → 13 → ENGAGE w/ aware-clip {2..5} → GRAB(4) → DEVOUR(6) → player pst=7 hp=−1; killer feeds corpse after | ✅ same chain, same end state |
| **TURN slew (W1 headline fix)** | disasm: ±0x80/frame off-cone, one-time snap on cone entry | slot4 F212-218: rot 2507→3275 = **exactly +128/frame ×7**, snap+exit F219, approach F220 | ✅ rate byte-perfect |
| Collision wall | skill-documented x=−5118 (both sides) | player stops at x=−5118 | ✅ |
| ROOM1090 spiders | 7 spiders subs {0,1,2,4,3,3,4} = behavior nibble | 7/7 slot-for-slot sub+grid match | ✅ |
| ROOM11C0 roster | NPC 0x42 **st=4 ss1=6 g=0x40** + 2 maggots g=0x30 | identical (the W7 scripted-start live) | ✅ (see LOW note) |
| ROOM1190 dog | (no usable dog savestate — `mzd_stage1_dog.sav` is an NPC scene) | spawns **st=4 sub0 g=0x40** = the W2 scripted-start fix, stable | ✅ port-side |
| ROOM10C0 crow | (no savestate) | perch brain cycles subs 0↔1↔2↔3, clips 0/1/2, rot micro-slew, no stuck state | ✅ port-side |

**New LOW finding from the parity run:** frozen spawns (maggot g=0x30, freeze bit +0x9&0x20): PSX
shows `+0x4=1` at spawn (INIT ran before the freeze gates the brain), the port holds `st=0` until
unfreeze — an INIT-ordering difference under the freeze bit, no observable behavior delta after the
SCD unfreeze. Settling it byte-true needs the gate placement in the root prologue disasm (does the
freeze check sit before or after the state-0 INIT dispatch). Tracked open.

The dive/grapple (crow) and bite/heavy (maggot) live paths need their SCD provocation events — their
mechanics are pinned by test_crow_ai/test_maggot_ai + the wave disasm; fresh PSX captures via
`re15-room-capture` would extend the table.

**Regenerate the finding set:** the full verified JSON lives in the session scratchpad
(`ai_divergences.json`, 81 entries with byte evidence per side); the audit/fix workflow scripts are
under the session `workflows/scripts/` dir (`enemy-ai-divergence-audit-*.js`, `enemy-ai-fix-waves-*.js`).
