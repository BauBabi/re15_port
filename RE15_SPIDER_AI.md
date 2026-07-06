# RE1.5 Spider-Baby (type 0x26, EM026) — byte-true RE

Der Spider = **Enemy-Typ 0x26** (Modell **EM026**, EMS-Blob-Index 11 @0x1b5800, ~16 Clips, 33 KB — ein
kleines Baby-Spider-Modell). **Kein Läufer** — ein **stationärer Web-Spitter / Ambush-Kreatur**: er
emergt an seinem Spawn-Punkt, bleibt stehen, macht −2 Contact-Damage und armt Kollisions-Hit-Codes +
Telegraph-fx („Web"). RE via Workflow wf_b77c2591 (5 CLUSTER-Agenten + 5 Refuter, adversarial verifiziert).

## Dispatch (byte-verifiziert, STAGE1.BIN)

- **EXE-Dispatch @0x80072bac[0x26] = 0x80116288** (STAGE1-Reg @0x8011e8f4: `sw v0, 0x80072c44`).
- Root **0x80116288** dispatcht STATE (+0x4) via Tabelle **@0x80121268** (16 Einträge, KEIN sltiu-Bound),
  DANN geteilter Tail (0x8002b498 contact-clear, +0x1d0-Emerge-Gate, 0x8002aec4 body-push, Contact-Damage).

| +0x4 | Handler | Rolle |
|---|---|---|
| 0 | 0x801164b0 | INIT |
| 1 | 0x801166fc | ACTIVE (Attack-Arming-Brain, Trampolin +0x5) |
| 2-4 | 0x8011697c | Decision-Trampolin → **immer** 0x80116a04 (via @0x80121290, idx 10+(+0x5)) |
| 5-7 | 0x80116758 | Behavior A / (Root-State: DEATH-Twin) — Strike-Loop +0x6, Budget 0x28=40 |
| 8-9 | 0x80116870 | Behavior B / (Root-State: DEATH) — Strike-Loop +0x6, Budget 0x2c=44 → Corpse-Latch |
| 10-15 | 0x80116a04 | Behavior C / (Root-State: HURT) — 3-Frame-One-Shot +0x7 → reset state 1 |

**Dual-Use-Falle (aliasing):** dieselben Handler sind (a) über den STATE[1]-`+0x5`-Trampolin
(state_table[5+(+0x5)]) **Attack-Behaviors** UND (b) als direkte Root-States (via take_damage-Set von +0x4)
**HURT (0x80116a04)/DEATH (0x80116870)**. Statisch nicht 100% disambiguierbar — die genaue +0x1d0-Emerge-vs-
Attack-Progression braucht einen Savestate als Schiedsrichter (kein Spider in ROOM1140; STAGE1-unreachable).

## STATE [0] — INIT 0x801164b0 (byte-true, adversarial CONFIRMED)

Ein-Frame-Init, immer → state 1: **HP=100** (+0x9a, @0x801164f8), clear +0x93/+0x94(clip)/+0x95/+0x8f
(@0x801164ec-544), anim_set(bank +0x16c, frame 0, flags 0x200), Collision-Box-Ptr +0x78 = @0x80121258
(Halfwords {0,0,0,600,720,600}), Shadow-Init (600×600 Bodenschatten, 0x8001af5c), **+0x1d0..+0x1ec = 0**
(8 Words genullt @0x80116594 → +0x1d0 startet 0), Home-Pos-Cache (+0x1d8=X, +0x1da=Z, +0x1d6=Y), Flags
(+0x0 &= 0x1FFFFFFF, |= 0x40000000, |= 0x8), **+0x4=1 ACTIVE** (@0x80116690), **+0x5 = +0x9 & 0x7f**
(Variant aus Spawn-Byte, data-driven, KEIN RNG, @0x801166ac), if !(+0x9&0x80) → Spawn-fx 0x09031800
(rot_y, *(+0x188)+0x40, zero-pose @0x80121248). Keine Se/RNG/Damage/Movement.

## Root Tail — EMERGE-Gate + Contact-Damage (byte-true CONFIRMED)

- **Global-Pause-Gate** (shared): +0x9 & 0x20 = „tick trotz Freeze", sonst skip (@0x801162c8).
- **+0x1d0-EMERGE-Gate** (sltiu<13 @0x80116328): 
  - **+0x1d0 < 13 → EMERGE**: +0x0 |= 0x2|0x40 (intangibel), KEIN body-push/contact. Vertikal-Motion im
    Tail: +0x34=+0x1d8 (X), +0x3c=+0x1da (Z), **+0x38 = +0x1d6 − step·(+0x1d0−1)** (step 40 wenn +0x9&0x80,
    sonst 20 @0x80116474/90) — der Baby-Spider klettert aus dem Spawn hoch.
  - **+0x1d0 ≥ 13 → ACTIVE/solid**: +0x0 &= ~(0x2|0x40), body-push (0x8002aec4).
- **Contact-Damage** (nur wenn body-push überlappt, @0x80116378): wenn player.hit_react==0 →
  **DAT_800aca58=2** (player STATE=2 = Hit/Stagger, NICHT Grab), **DAT_800aca59=facing_aligned+2** (2 back/3
  front, directional Stagger), **DAT_800aca5a=0**; dann wenn **player.hp ≥ 4** → **player.hp −= 2**
  (@0x801163c8, Floor: nie unter 2). KEIN Se/fx/Timer. **Das ist ein Hit-STAGGER-Marker, KEIN Web/Grab.**

## STATE [1] — ACTIVE Attack-Arming-Brain 0x801166fc (byte-true, exhaustive-scan CONFIRMED)

**KEINE Lokomotion, KEIN clip/anim-Write, KEIN player.hp-Write, KEIN DAT_800aca5x** im ganzen state-1-Tree
(exhaustiv gescannt). Trampolin: `state_table[5+(+0x5)]` (Basis 0x8012127c). Epilog: 0x8001b064
(Collision-Box-Rebuild, +0xb0, −1800) — NICHT Movement. +0x5 → Behavior:
- **A** 0x80116758 (+0x5∈{0,1,2}): Strike-Loop auf +0x6, Budget +0x1d0=0x28=40. +0x6=0 ROLL-fx→1; =1
  Windup (+0x1d4−−; auf 0: ≥48 hold / <8 → +0x6=3 tot / sonst STEP-Arm 0x22, +0x1d0++, →2); =2 COMMIT-Arm
  0x12 + Timer-Reroll →1.
- **B** 0x80116870 (+0x5∈{3,4}): = A, ABER Budget 0x2c=44 UND KEIN +0x6=3-Dead-Abort (der <8-Fall skippt
  nur den STEP-Arm, macht +0x1d0++/+0x6=2 weiter → COMMIT).
- **C** 0x80116a04 (+0x5∈{5..10}): 3-Phasen auf +0x7. =0: +0x93=3, ≥8→WINDUP-Arm 0x23 / <8→SE-Request-Bit
  (0x8004ef90(0x800b1028, variant+29), nur variant<3), +0x1d0−− (Floor 1) →1; =1 COMMIT-Arm 0x12 →2; =2
  RESET +0x4=0x00010001 (state 1), +0x5=variant, +0x93=0.

**Attack-Primitive 0x80019d50** (shared): schreibt a2 (Hit-Code 0x22/0x12/0x23) in die matchenden Model-
Instanzen (Pool @0x800a73b8, stride 132, ≤96; match inst+0x70==a0(slot 8/0x10) && +0x71==3 && +0x74==a3).
Das Shared-Contact-System liest inst+0 → Player-Schaden. **Telegraph-fx (ROLL 0x80116d00 → 0x80019700):**
fx-id `(+0x1d0<<8)|0x08030000` (variant 0,3) / `|0x10030000` (variant 1,2,4) — die sichtbare „Web"-Spucke.
Timer +0x1d4 = rng()&0x3f + 16 = [16,79].

## HURT / DEATH (Root-States, dual-use mit den Behaviors)

- **HURT** 0x80116a04 als state 10-15 (+0x7): +0x93=3 (Hit-Flash), clip 0x23/SE, clip 0x12, → **RESET state
  1** (+0x4=0x10001, +0x5=variant, +0x93=0). 3-Frame, non-lethal.
- **DEATH** 0x80116870 als state 8/9 (+0x6): Gib-Burst (ROLL 0x80116d00 fx type 0x03, count 8/16) → per-
  Cycle 0x22+0x12-Spurts während +0x1d0→0x30 → **permanent inerter Corpse** (+0x1d4 free-runt 0xffff). Wenn
  INIT-rng +0x1d0≥0x30 seedet → Corpse beim ERSTEN Expiry ohne Spurt.
- **Twin** 0x80116758 (state 5-7): byte-identisch zu 0x80116870 außer +0x6=3-Terminal (@0x8011683c).

## Shared-Callees (schon portiert / zu portieren)

| Addr | Rolle |
|---|---|
| 0x8002b498 / 0x8002aec4 | contact-clear / body-push (shared, portiert) |
| 0x8001a780 | facing_aligned (half-plane 0/1) |
| 0x80019700 | fx-spawn (Telegraph-„Web" + Spawn + Gib-Burst) |
| 0x80019d50 | Attack-Hit-Code auf Model-Instanzen (Pool 0x800a73b8) — **NEU, spider-spezifisch nötig** |
| 0x8004ef90 | Bitset (SE-Request @0x800b1028, variant+29) |
| 0x8001b064 | Collision-Box-Rebuild (Epilog) |
| 0x8001af5c | Ground-Shadow-Init (cosmetic) |
| 0x8001af20 | RNG |

## ✅ LIVE-VERIFIZIERT (2026-07-06, ROOM1090)

Der Spider spawnt in **STAGE1 ROOM1090** (6× Schwarm, port-ladbar + DuckStation-capturebar). DuckStation-
Savestate `stage_saves/mzd_stage1_spider.sav` (JUMP 0x109 = `--left 27`) bestätigt **byte-true**:
- **Wave 1:** Typ 0x26 (6×), +0x4=1 (active), +0x5/+0x9 = Variant 0-4 (== mein `grid&0x7f`, Behavior A für
  0-2 / B für 3-4), **Hitbox 600/720** (== mein re15_damage.c case 0x26).
- **Wave 2 (Hit-Codes) LIVE-GEPINNT:** der Kollisions-Instanz-Pool @0x800a73b8 zeigt **14 Instanzen mit
  inst+0 = 0x12 (COMMIT-Hit-Code)** auf Body-Part-Slots **8/16** (inst+0x71=3, inst+0x74=Model-Inst-Ptr
  0x8013cxxx) — **exakt** die RE (COMMIT 0x12 via 0x80019d50, a0=slot 8 für Variant 0,3 / 0x10 für 1,2,4,
  a1=3). Der Damage-Mechanismus (0x80019d50 schreibt den Hit-Code in inst+0; das shared contact system liest
  inst+0 → Player-Schaden) ist damit live bestätigt.

**PORT-Anforderung Wave 2:** das **Kollisions-Instanz-System** (0x80019d50 Hit-Code-Arming + der shared
contact reader inst+0 → Damage) — der Spider-Angriff läuft NICHT über direkten player.hp-Write, sondern über
diese geteilte Infra (die der Port noch nicht hat). Substantieller Infra-Zusatz.

## Wellen-Status

1. ✅ **ENUMERATE + CLUSTER RE** (wf_b77c2591, adversarial verifiziert): alle 6 Handler + Root + Sub-Dispatch
   byte-true, jede Konstante disasm-zitiert.
2. **Wave 1 (Port-Ziel):** re15_spider_ai_tick — INIT + Root-EMERGE-Gate (vertikal-Climb, intangibel) +
   Contact-Damage −2 + HURT/DEATH (killbar, Corpse). Neue Struct-Felder (+0x1d0 Phase, +0x1d4 Timer,
   +0x1d6/+0x1d8/+0x1da Home-Pos). Test test_spider_ai (seeded, kein Spider in STAGE1).
3. **Wave 2 (deferred):** die exakte +0x1d0-Emerge-vs-Attack-Progression (Savestate-Schiedsrichter nötig) +
   das Kollisions-Instanz-Hit-Code-Arming (0x80019d50 → inst+0 → Player-Schaden via shared contact) + die
   Telegraph-„Web"-fx-Präsentation. EM026-Clip-Frame-Counts via parse_container beim Port.
4. **Dynamik-Verify:** Savestate aus einem Spider-Raum (spätere Stage) + re15_enemy_state.py mit Spider-Label-
   Map @0x80121268; die +0x1d0-Progression + der Hit-Code→Damage-Pfad live beobachten.
