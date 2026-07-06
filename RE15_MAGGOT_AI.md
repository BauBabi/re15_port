# RE1.5 Maggots (type 0x27, EM027 = MAGGOTS_BABY) — byte-true RE

Der Maggot = **Enemy-Typ 0x27** (Modell **EM027**, BioModels `EM027.EMD = MAGGOTS_BABY`, EMS-idx 12
@0x1be000, **208 KB**, 29 Clips). Ein **großes, bewegendes Boden-Vieh** (1600×1440-Body, **HP 180**,
Crawl-Speed ~1.7) mit dem **größten State-Automaten bisher (16 States)**. RE via Workflow **wf_f597f55d**
(5 CLUSTER + 5 Refuter, adversarial verifiziert).

## Dispatch (byte-verifiziert, STAGE1.BIN)

- **EXE-Dispatch @0x80072bac[0x27] = 0x80116db8** (STAGE1-Reg @0x8011e904: `sw v0, 0x80072c48`).
- Root **0x80116db8**: Global-Pause-Gate (+0x9&0x20) → State-Dispatch (+0x4) via **@0x801213c8** (16 States)
  → geteilter Body-Tail: 0x8002b498 contact-clear, 0x8002aec4 + 0x8002b544 body-push, **0x8003b0a4 SCA-
  Wall-Clamp** (Box-Halbmaß 1600), 0x8001b064 Box-Rebuild. **Root macht KEINEN Schaden** — alles in den States.
- Zwei kontiguöse Tabellen: PRIMARY @0x801213c8 (per +0x4, 16 States) + SECONDARY @0x80121428 (=&tab[8],
  per +0x5 aus dem STATE[1]-Brain). A[i]=state_table[8+i].

| +0x4 | Handler | Rolle |
|---|---|---|
| 0 | 0x80116f50 | INIT |
| 1 | 0x80117254 | ACTIVE-BRAIN (2-Tab-Dispatch A decision + B movement auf +0x5) |
| 2 | 0x8011af5c | ballistic special (leap-abort-Ziel) |
| 3 | 0x8011b6fc | **DEATH-Lane** (ballistic settle → death-dispatch @0x80121500) |
| 4/5 | 0x8011bdec | (special) |
| 6 | 0x8011c598 | (special) |
| 7 | 0x8011be54 | (special) |
| 8-15 | 0x80117484… | ACTIVE-Sub-Handler (idle/chase/bite/leap, via +0x5-Trampolin) |
| 13/14/15 | 0x80118268/544/900 | **`jr ra;nop` NO-OP** (unreachable, kein +0x4=0xd/e/f Write) |

## STATE [0] INIT 0x80116f50 (byte-true CONFIRMED)

→ state 1 (oder 0x101 = state 1 + sub 1 wenn +0x9&1). **HP=180** (Tabelle @0x8011f034 + type·32 + (rng&0xf)·2;
row 0x27 @0x8011f514 = alle-180). Clear +0x94/+0x95/+0x8f/+0x93. Steer-Target = Spieler-Pos (+0x1bc/+0x1be).
Timer-Seeds (+0x1b9=2, +0x9c=0, +0x1e2=4=Collision-Shape-Index). anim_set(clip 0). Speed +0x166=0x1b33 (Q12).
Collision-Shape +0x78 = @0x80121350 = {0,−1440,0,**1600**,1440,1600}. Flags +0x0 |= 0x800.

## STATE [1] ACTIVE-BRAIN 0x80117254 (byte-true CONFIRMED)

LOS (0x8001bc08 → +0x1d8, Bit0→+0x1d0) + Dist (SquareRoot0 → +0x1d4). Zwei-Tab-Dispatch auf +0x5:
A (decision @0x801213e8=tab[8+i]) + B (movement @0x80121428). Proximity-Flags (bit29 <4000, bit31 <5000).

- **sub 0**: A[0] 0x80117484 **DECISION** (Prio: +0x1dc-Lockout; dist<5000 & LOS → **+0x5=3 CHASE**;
  dist<6000 & `DAT_800aca58==0x0701` grab-cmd → **+0x5=4 GRAB**; blind dist<3000 → +0x5=3/+0x7=1). B[0]
  0x80117574 **idle-wander** (clip **0x16**, Wander-Timer +0x9c=rng+59; Expiry → CHASE).
- **sub 3**: CHASE (A[3] 0x80117a3c decision → attack-Wahl; B[3] 0x80117c90 **crawl** clip 4/5/7 rng, yaw-slew
  0x8001a8f8 + move-helper 0x8011bf50 + Se).
- **sub 4**: GRAB/LUNGE-Selector (→ attacks).

## ATTACKS (SECONDARY, +0x5) — DIREKTER player.hp-Schaden (byte-true CONFIRMED)

- **sub 5** 0x80118270 **BITE** clip **0x12**: Damage-Window {0x0c-0x0f}, Hitbox 0x8001bff8 (1000) →
  **player.hp −= 6** + Se(6) + **Grab-Handshake** (aca58=2, aca59=facing+2, aca5a=0; wenn hp<0 → aca58=3).
- **sub 6** 0x8011854c **HEAVY BITE** clip **0x13**: Dual-Hitbox (800/800), Window {0x15-0x18,0x21,0x22} →
  **player.hp −= 12** + Se(5) + Grab-Handshake (aca59=facing+4). case2 → chained zurück zu bite/chase.
- **sub 7** 0x80118908 **LEAP/ballistic pounce** clip **0x14**: windup (0x8001aa68 yaw-kick) → launch @f0xa
  (+0x8c=rng&0x1f+200, +0x1e0=1) → airborne ballistic (0x8001c1a4 a2=−60, dust-fx wenn +0x93&2) → land
  (Se(2), +0x93=0) / mid-air-finisher (hp<50 & in 5000/192 → +0x5=8, Se(7)) / abort-on-hit → state 2. KEIN
  direkter Schaden (routet zu act 8).

**Damage-Pfad (alle Bites):** direkt `player.hp -= N` + Grab-Handshake (aca58/59/5a). **KEINE 0x80019d50-
Kollisions-Hit-Codes** (anders als der Spider) — der Maggot schlägt direkt.

## DEATH-Lane (primary[3] 0x8011b6fc, byte-true CONFIRMED)

take_damage (shared) setzt hp<0 → +0x4=3. 0x8011b6fc: ballistic settle (wenn +0x1e0, Se(2)) → dispatch +0x5
@0x80121500 → **death-anim 0x8011b7b8** (clip **0xe**, +0x93|=2, gore-fx @0x80121388, **Se(0)**, bitset
@0x800b1038) → **corpse 0x8011b998** (clip **0xa**, oder **0xb** wenn +0x93&0x80; inert).

## EM027-Clip-Frame-Counts (byte-true, dir[1], 29 Clips)

`{78,20,15,70,78,39,24,12,25,25,40,40,100,40,70,90,35,20,25,40,40,21,58,30,50,40,70,30,52}` — idle 0x16=58,
bite 0x12=25, heavy 0x13=40, leap 0x14=40, death 0xe=70, corpse 0xa=40, crawl 4=78.

## Wellen-Status

1. ✅ **ENUMERATE + CLUSTER RE** (wf_f597f55d, adversarial): alle 16 States + Root/INIT + die 2 Tabellen +
   die Attacks + Death byte-true, jede Konstante disasm-zitiert.
### ✅ LIVE-VERIFIZIERT (2026-07-06, ROOM11C0)

Der Maggot spawnt in **STAGE1 ROOM11C0** (2×, port-ladbar + DuckStation-capturebar). DuckStation-Savestate
`stage_saves/mzd_stage1_maggot.sav` (JUMP 0x11C = `--left 8`) bestätigt mein Wave-1-Port **byte-true**: Typ
0x27, +0x4=1 (active), +0x5=0 (idle-wander), +0x9=0x30 (mode 0), **Hitbox 1600/1440** (== mein
re15_damage.c case 0x27). Live im Port (`RE15_START_ROOM=11C0`): 2 Maggots st=1, idle→chase. **Live-Bug
gefixt (Commit 2213dcd0):** re15_dog_arc dog_dist-s16-Overflow (Maggot biss Luft bei ~45000 Distanz).

2. ✅ **Wave 1 PORTIERT** (test_maggot_ai, 40/40): re15_maggot_ai_tick (run_all `else if t==0x27`, Boden→wall-
   clamp) — INIT (HP=180) + STATE[1] idle-wander (clip 0x16) + CHASE (yaw-slew + crawl clip 4) + killbar
   (DEATH clip 0xe → CORPSE clip 0xa). EM027-Clips embedded, Hitbox 0x27=1600/1440.
3. ✅ **Wave 2a PORTIERT (test_maggot_ai (2b), 41/41): der BITE-Angriff.** CHASE (sub 3) → wenn Spieler in
   range 3000 & Lockout(+0x1dc)==0 (A[3] @0x80117a5c) → **BITE sub 5** (clip 0x12): im Damage-Window
   anim_frame {0x0c-0x0f} & Cone 2000/384 → **player.hp −= 6** (@0x80118468) + Se(6) + Lockout 45 → zurück
   zu CHASE. `dog_atk_cd` als +0x1dc-Lockout wiederverwendet.
4. ✅ **Wave 2b HEAVY-BITE PORTIERT + LIVE-VERIFIZIERT (test_maggot_ai (2c), 42/42).** Der **Savestate-
   Repacker** (`re15_ss_patch.py`, neu — patcht RAM in einem DuckStation-Save + repackt; DuckStation
   akzeptiert es, kein Checksum) erlaubte den **Provoke ohne Navigation**: Maggot neben den Player
   teleportieren + Skip-Tick (+0x9&0x20) clearen → `stage_saves/mzd_stage1_maggot_heavy.sav` zeigt die
   **+0x5 3→4→6-Eskalation** live, Player HP 94→82 = **−12**. Die volle byte-true Kette:
   - **A[3] CHASE tail @0x80117c54:** `player.hit_react==0 & dist>=6001 (0x1771) & +0x1d0&1 (LOS)` → **+0x5=4
     SELECTOR** (der Player ist fern-aber-sichtbar → committe zum Heavy-Approach).
   - **SELECTOR A[4]=state[12] 0x80117e40:** krabbelt näher (B[4] clip 6, advance) bis **in range 4000
     (a0=0xfa0) cone 192 (a1=0xc0) @0x80117e90 & hit_react==0 @0x80117e88 & +0x1dc-Lockout==0 @0x80117eb0**
     → **+0x5=6 HEAVY** (@0x80117ec4). [Der else-Zweig +0x5=7 LEAP = wave 2c, s.u.]
   - **HEAVY sub 6 0x8011854c:** clip 0x13, **LUNGE nach vorne** (move-helper 0x8011bf50 @0x80118664), dual-
     hitbox 800/800 (0x8001bff8 a2=0x320 ×2), Damage-Window frame≥0x15 (@0x80118650) → **player.hp −= 12**
     (@0x801187bc) + **Se(5)** (@0x80118798) + Grab-Handshake aca58/59/5a (@0x801187d8) → zurück zu CHASE.

5. ✅ **Wave 2c LEAP PORTIERT + LIVE-VERIFIZIERT (test_maggot_ai (2d), 42/42).** RE via Workflow
   **wf_c1de93d6-bae** (5 Agenten + adversariale Verifikation, 3/4 CONFIRMED). Der SELECTOR-else-Zweig
   (+0x5=7, clip 0x14) ist ein **ballistischer 0-DAMAGE-Reposition-Pounce** (KEIN player.hp-Write in den
   260 Instruktionen von 0x80118908 — CONFIRMED), der die Distanz schließt, landet, und zum SELECTOR
   zurückkehrt (→ dann Heavy). Byte-true:
   - **Trigger (far-ballistic Path B @0x80118028):** Player außerhalb des Heavy-Fensters (dist>4000 ODER
     off-cone) & LOS(+0x1d0&1) & **dist(+0x1d4)>=6001** (@0x8011806c) & maggot **aimed ±32** (arc_test
     0x8001a9cc @0x80118084) & **Player blickt ZUM Maggot** (facing_aligned 0x8001a780==0 @0x801180bc:
     playerYaw vs maggotYaw >±90°) & **rng&1** (~50% @0x801180a8, bypass wenn *(0x800aca58)==0x701).
     [Path A Zone-Leaps (+0x1e2!=0 → 0x8003b93c Raum-Zonen-Lookup, +0x7=1/3) brauchen BLK-Kollisions-Zonen.]
   - **Launch (sub 7 = 4-Phasen-FSM auf +0x6):** Phase 0/1 WINDUP frames 0..9 (slew facing) → **frame 10 =
     LAUNCH** (@0x80118a3c: +0x6=2, **airborne +0x1e0=1**, Impuls **+0x8c=rng&0x1f+200** @0x80118a50, bzw.
     0x32a=810 für Variante +0x7=3) → Phase 2 IN-FLIGHT: ballistic-Integrator **0x8001c1a4(a0=+0x8c horiz,
     a1=+0x1d8=(+0x1e2)*30+600=720 vert, a2=-60 gravity, a3=+0x1ba groundY)** — Parabel apex ~12 frames,
     total ~25 airborne → Landung → Recovery (+0x5=9 @0x80118b60) → zurück zum SELECTOR.
   - **LIVE-VERIFIZIERT** (forced +0x5=7 via re15_ss_patch): **Impuls +0x8c=201 geseedet** (=rng&0x1f+200,
     der Launch lief), Maggot schloss **dist 10000→596**, Player-HP unberührt vom Leap selbst.
   - Port (sub 7): windup 10 frames → launch (crow_speed-Impuls) → re15_crow_advance + Y-Parabel (720/-60) →
     Land bei ~25 frames → SELECTOR. Der Se/exakte Se-Bank (landing-thud 0x800453d0 a0=2) ist faithful-line.

   **Historischer Blocker (jetzt GELÖST):** A[3] CHASE emittet +0x5=5 (Bite @0x80117a94) / 15/0xf
   (@0x80117b34) / **4 (@0x80117c54, den ich zuerst übersah)**. Der Attack-Selector
   (state[12]/A[4] 0x80117e40) + Heavy/Leap werden also NICHT direkt von CHASE erreicht, sondern über die
   **zwei-stufige Decide(A)/Animate(B)-FSM** (A base @0x801213e8=&tab[8], B base @0x80121428=&tab[24]), wo
   mehrere Decide-Handler +0x5 durch 5→6→7 eskalieren. Diese Eskalations-DYNAMIK ist statisch nicht sauber
   pinbar → braucht einen **provozierten** Maggot-Savestate (Spieler mid-attack am Maggot in ROOM11C0). Der
   Maggot ist funktional byte-true KOMPLETT (Chase + Bite −6, live-verifiziert); Heavy/Leap sind Sekundär-
   Varianten.

   **LEAP-Skelett byte-zitiert (statisch soweit erschöpft):** CHASE A[3] emittet +0x5=15/0xf (@0x80117b34).
   +0x5=15 → **A[15] = tab[8+15] @0x80121424 = 0x8011a878 = der ballistische AIRBORNE/Landing-Handler**:
   liest +0x93 → `&0x2`==0 (nicht airborne) → Ende (No-Op); `&0x2` gesetzt & `&0x40`==0 (mid-air) → Bit 0x2
   clearen + **Landing-Dust-fx** 0x80019700 (Offset-Vektor @0x801213a8, rotiert um +0x6a); `&0x2 && &0x40`
   (gelandet) → +0x38=+0x1ba (ground-y restore), +0x9c=0, **+0x4=2 (Recovery-State 0x8011af5c), +0x5=7**.
   Der Leap-LAUNCH (sub 7 0x80118908) setzt +0x1e0=1 (airborne) + +0x8c=rng-Impuls + +0x93&0x2. Also die
   ballistische Flow selbst ist byte-true bekannt — nur die **Launch-ENTSCHEIDUNG** (welcher Decide-Handler
   +0x5=15 unter welcher +0x93/Distanz-Bedingung setzt) ist der dynamische Rest → Savestate.
   **Statische Wege erschöpft:** A[3]-Emits (5/15), A[15]-Airborne, 0x8011a900-Landing-fx, alle +0x93-Bit-
   Writer (@0x8011a8c8/a900/aaec/aba8/ad1c/ae00/ae8c) getract; die Entscheidung bleibt DYNAMIK.
4. **Dynamik-Verify:** Savestate aus einem Maggot-Raum + re15_enemy_state.py mit Maggot-Label-Map
   @0x801213c8; die CHASE→Attack-Transition + der Bite-Damage live.
