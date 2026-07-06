# RE1.5 Krähe (CROW, Typ 0x21) — byte-true RE (vollständig, adversarial-verifiziert)

Die Krähe = **Enemy-Typ 0x21** (Modell **EM021.EMD**, BioModels-bestätigt). Ein 3D-FLIEGER mit
Höhen-Korridor-Kreisflug + Distanz-getriggertem Sturzflug. Vorkommen (RDT-Sce_em_set): **STAGE1
ROOM10C0/10C1/1120/1121**. Diese Datei ist das **byte-true Port-Blueprint** — jede Konstante zitiert
eine Disasm-Adresse in `STAGE1.BIN` (file_off = addr − 0x80100000). Erstellt via `re15-enemy-ai-re`
(ENUMERATE + 5-Cluster-CLUSTER-Workflow wf_cede3fd1, **jeder Befund adversarial gegen die rohen Bytes
verifiziert** — 1 echter Fehler gefunden+korrigiert: +0x1db invertiert).

## Dispatch

- **EXE @0x80072bac[0x21]=0x80072c30** → registriert @0x8011e8e0 → **Root-Handler 0x80112020**
  (eigener Handler; teilt die Zombie-Entity-Struct + geteilte EXE-Infra — bestätigt die 70%-These).
- **Root-STATE-Tabelle @0x8012111c** (idx +0x4, verifiziert entry-für-entry):

| +0x4 | Handler | Rolle |
|---|---|---|
| 0 | 0x8011224c | **INIT** — Perch-Höhe kapseln + Lift-off |
| 1 | 0x80112420 | **ACTIVE-Flug** (Kreis/Cruise-Treiber, Höhen-Korridor) |
| 2,5,6 | 0x80114e4c | **HURT = LEERER STUB** (`jr ra` @0x80114e4c) — keine Stagger-Reaktion |
| 3 | 0x801146d0 | **DEATH** (Sturz vom Himmel + Gib) → State 7 |
| 4 | 0x80114e54 | **FLIGHT-2-Dispatcher** (Ascend/Hover/Orient/Spin/Dive-Arm) + Quelle der Death-Promotion |
| 7 | 0x801157e8 | **SPECIAL** (Color-Fade / geskriptetes Event) |
| 8,9,10,11 | 0x80112628 | **DISTANZ-DIVE-Commit** (der Angriffs-Trigger) |

- **Root-Promotion Hit→DEATH** (0x80112020, byte-verifiziert @0x80112044-8c): `take_damage FUN_80011f50`
  schreibt **NICHT** +0x4 (nur sltiu 0x14/0x40-Thresholds), sondern setzt über den geteilten Hit-Pfad das
  **Downed-Bit +0x9|0x40**. Root: `if (state==4 && (+0x9 & 0x40)) → +0x4=3 DEATH` + `0x80115d74(a0=0)`.

## Krähen-spezifische Felder (JENSEITS der Zombie-Struct — das Neue)

| Offset | Typ | Bedeutung |
|---|---|---|
| +0x34 / +0x38 / +0x3c | s32 | **Flug-Position X / Y(HÖHE) / Z** — kleineres Y = höher |
| +0x1ea (490) | s16 | **Ziel-/Perch-Höhe** — EINMAL am INIT gesetzt (= Spawn-+0x38) |
| +0x1ec (492) | s16 | **Vertikal-Fehler** = `0x800aca8c(playerY) − +0x38`, jeden ACTIVE-Tick |
| +0x1e4 (484) | s16 | **Vertikal-Geschwindigkeit** (in +0x38 integriert) |
| +0x1e8 (488) | s16 | Gravity-Accel für den Todessturz (=0x26=38) |
| +0x1ba (442) | s16 | **Boden-Y-Referenz** (room_coll a3=0x400); Sturz-Clamp + Schatten-Skala |
| +0x1dc (476) | s16 | Horizontal-Distanz zum Spieler (SquareRoot0) |
| +0x1d4 (468) | u8 | **Mode/Flags**: low 6 bit = vvel-Betrag; bit0x80 = Climb/Descend-Hysterese; bit0 = Richtung |
| +0x1d2/1d3/1d5/1d6/1d7/1d8/1da/1db | — | Phase-Toggle / Pending-Turn / Timer / Latches |

## INIT — 0x8011224c (verifiziert, +0x1db KORRIGIERT)

`0x800acc0c=1` → clear +0x1b8/+0x1b9/+0x1ba/+0x93/+0x9a → **count Typ-0x21** (`0x8001b768`) →
`0x800aca50 = count<<4` → +0xa0=+0x34, +0xa2=+0x3c → **set clip** (`0x80115d94`, +0x94=0) → anim
(`0x8001f314`) + Schatten/Box-Register (`0x8001af5c`, +0x78=0x801210fc) → **clear 8-Wort-Block
+0x1d0..+0x1ec** (mode=0, vvel=0) → **+0x1ea = +0x38** (Perch-Höhe, @0x801123a0) → **+0x38 −= 400**
(Lift-off 400 über Perch, @0x801123b8) → **+0x1db = (grid & 0x10) ? 0 : 1** (@0x801123cc-ec, KORRIGIERT
vom Verify) → **+0x4 = 1**, überschrieben zu **+0x4 = 4 iff (grid & 0x40)** (@0x801123f8). Kein
Sub-Mode-Dispatch; +0x5/+0x6/+0x7 unberührt.

## ACTIVE-Flug — 0x80112420 (der Cruise-Treiber, verifiziert ~95% byte-true)

1. `0x8001bc08` → +0x1d2 &1 (Per-Frame-Phase-Toggle, @0x8011243c/54).
2. **Horizontal-Dist**: dx=playerX−+0x34, dz=playerZ−+0x3c, `SquareRoot0` (`0x80065f60`) → **+0x1dc**
   (@0x8011246c-ac).
3. **Vertikal-Fehler**: `+0x1ec = 0x800aca8c − +0x38` (@0x801124bc-cc).
4. AI-Cmd-Flags: `if (0x800aca50 & 0xff00) → 0x80116068` (Flug-Command-Dispatcher, @0x801124dc-e8).
5. **Doppel-Dispatch by +0x5**: Steer-Tabelle **@0x8012113c** (@0x801124fc-514) + Move-Tabelle
   **@0x80121184** (@0x80112530-548).
6. `entity[0] &= 0x1fffffff` (Render-Flags clear, @0x8011256c).
7. **+0x1ec-Band-SFX** (@0x80112588): `≥4001 → 0x80012a0c(6000)` [Proximity-Flag |=0x80000000],
   `<800 → 0x80012974(6000)` [|=0x20000000], `800..4000 → entity[0] |= 0x40000000`. **KEIN HP-Write.**
8. `0x80115f70` (Schatten/Level-Flug-Kosmetik); `0x800aca50 & 0x800` One-Shot +0x1db-Bookkeeping.

### Steer-Tabelle @0x8012113c (by +0x5): `[0-3]=0x80112628 [4]/[6]=0x80112a28(ret) [5]=0x80112bac [7]=0x80112e4c [8]=0x801130fc [9]=0x8011325c [10]=0x801134f8 [11]=0x8011376c`
### Move-Tabelle @0x80121184 (by +0x5): `[0]0x8011279c [1]0x80112854 [2]0x801128e4 [3]0x80112974 [4]0x80112a30 [5]0x80112bec [6]0x80112d34 [7]0x80112e88 [8]0x80113138 [9]0x80113384 [10]0x80113590 [11]0x801137fc`

## Höhen-Oracle — 0x80115dc8 (reine Predicate, gibt dir=+1/−1)

```
a0=+0x1ea (target), a1=+0x38 (currentY)
if (currentY < target−5400) return +1        // @0x80115ddc — über der Decke → descend (Y wächst)
if (currentY > target−1800) return −1        // @0x80115dec — unter dem Band → climb
// im Band [target−5400 .. target−1800]:
return (+0x1d4 & 0x80) ? −1 : +1             // @0x80115e04 — Hysterese
```
→ **Höhen-Korridor 1800–5400 Einheiten über der Perch-Höhe +0x1ea.**

## Bewegungs-Primitive (jede Flug-Sub-State baut aus genau diesen drei)

- **A. Vertikal-Integration**: `+0x38 += +0x1e4` (@0x80112cec-cfc etc.).
- **B. Horizontal-Advance**: `0x800245d8` (pos_advance a0=0) entlang Yaw +0x6a. KEIN Root-Motion
  (0x8001ad68), KEIN atan2-Bearing (0x8001a6d4) im Flug-Pfad.
- **C. Yaw-Slew zum Spieler**: `0x8001a8f8(a0=&playerX @0x800b−13688, a1=rate)` — **rate 50 Cruise /
  100 Dive**; feste Bank ±40 (Dive) / ±80 (Evasive 0x80115e24).

### vvel-(+0x1e4)-Profile je Phase (zitiert)
- **LAUNCH** (move[4] 0x80112a30): vvel=−80 (@0x80112ac4), dann +6/Tick (Gravity, @0x80112b3c); +0x8c=60.
- **Dive-Gravity** +8 (@0x80112f8c); **Slow-Glide** +3 (@0x80112fe4).
- **CRUISE** (move[6] 0x80112d34): Yaw-Slew(50) → Oracle 0x80115dc8 → `+0x1e4 = (+0x1d4 & 0x3f) × dir`
  (@0x80112df0-dfc), dann +0x38 += +0x1e4.
- **DIVE-Descent** (0x80114600): Yaw-Slew(100); `+0x1e4 = (+0x1d4 & 0x3f) + 48` (@0x8011461c), nur solange
  `+0x38 < +0x1ba − 750` (@0x8011463c — stoppt 750 über Boden).

## DISTANZ-DIVE-Commit — 0x80112628 (der Angriff, 4 Commit-Pfade, 0 Refutations)

- **Pfad 0** (Pending-Turn, @0x80112638): `if (+0x1d3 != 0)` → clear +0x1d3, Yaw ±40 (by +0x1d4&1),
  **sub_state = +0x1d4 & 3** (@0x80112694).
- **Pfad 1** (Distanz-Ring): `grid&0x80 ? (dist +0x1dc < 5000) : (dist < 10000)` (@0x801126ac-d4) →
  `+0x1ec < 5400` (@0x801126e8) → **0x80115d74(a0=4)** = sub_state 4 DIVE.
- **Pfad 2** (g_flag52 & 1, @0x80112708) → +0x1ec<5400 → sub_state 4.
- **Pfad 3** (0x800aca50 & 0x1000, @0x80112748) → **sub_state = (+0x1d4 % 3) + 1** (mod-3-Magic verifiziert).

### Dive-Ausführung (verifiziert)
- **sub4** 0x80112a30: Se(a0=1), clip6, vvel−80, Wing-Flap-Loop (+0x95==8), Gravity+6 → **sub5** (@0x80112b94).
- **sub5** 0x80112bec: clip4, vvel−120, +0x8c=160, Gravity+6.
- **Dive-END** 0x80112bac: `if (+0x38 < +0x1ea − 3600) → sub_state 9` (@0x80112bc8-d4).
- **Climb-back sub6** 0x80112d34: Yaw-Slew(50), +0x1d5-Countdown → sub9.
- **resume sub9** 0x80113384: clip, Steer (0x80115dc8).

## DAMAGE-MODELL (byte-true — die Krähe macht Schaden in ihren ANGRIFFS-Substates)

Die Krähe verletzt den Spieler in den **Flock-koordinierten Angriffs-Substates** (nicht im
Cruise/Dive-Decide, die ich zuerst allein scannte — daher die frühere Fehl-Schlussfolgerung):
- **DIVE-ATTACK move[11] 0x801137fc**: bei Treffer (dist<600 & vert-err∈[1,3599] & player+0x93==0)
  → **`player.hp −= 4`** (@0x80113b04), `0x800aca58=2` (dive-hit-cmd).
- **GRAPPLE move[12] 0x80113c7c**: bei Kontakt (+0x1d0) → **`player.hp −= 8`** (@0x80113e34),
  `0x800aca58=5` (grab/hold), Grab-Link-Block (0x800acbcc/bd0/bfc), → sub 13.
- **GRAB-HOLD/FEED move[13] 0x80113e94**: Peck-Loop + Struggle-Drain (+0x9c, RNG), Release → sub 14
  (flock `|0x4000`); bei Spieler-Tod flock `|0x2000`.
- **STRIKE/PECK move[16] 0x80114484**: → **`player.hp −= 4`** (@0x801144f0); bei Kill flock `|0x2000`.
- Bei jedem lethalen Treffer (`player.hp<0`) → `0x800aca58=3` (death-cmd) + flock `|0x2000` = KILL-
  Broadcast (der Dispatcher zwingt alle Krähen in den Plunge sub 17); `+0x1d8=1` exempt den Killer.

> ⚠️ **Zwei Korrekturen dokumentiert:** (1) Die früher notierte „Touch-Damage −2 im Pre-Pass
> 0x80116288" war eine **Fehlzuordnung** — 0x80116288 ist der Root von Nachbar-**Typ 0x26** (Reg-
> Tabelle @0x8011e8e4-904: 0x80112020=Krähe / 0x80116288=0x26 / 0x80116db8=0x27); der Krähen-Root
> ruft es nie. (2) Meine daraus folgende „Krähe macht KEINEN Schaden" war ebenfalls FALSCH — ich
> hatte nur die Cruise/Dive-Decide-States gescannt, nicht die Angriffs-Substates 11/12/13/16. Der
> volle 18-Substate-RE (Workflow wf_a7e1ebcf) zeigt: die Krähe macht −4 (Dive/Strike) und −8 (Grab).
> Die Proximity-Helfer 0x80012974/0x80012a0c flaggen nur die eigene Entity (Render/LOD, kein Schaden).

## DEATH — 0x801146d0 (Tabelle @0x801211cc: [0-6]→0x80114738, [7]→0x801149c4 Gib)

- **step0** 0x80114784: Se(a0=3), +0x9a=−1, +0x1e4=0, **+0x1e8=0x26 (Gravity)**, +0x8c=0, +0x6c=0.
- **step1 FALL** 0x80114828: +0x6c += 140 (clamp 1024), +0x8c=60, **+0x1e4 += +0x1e8** (Gravity),
  **+0x38 += +0x1e4** (via 0x800245d8), Boden-Clamp `+0x1ba − 400` → **Land**: clip 0x0a, Se(a0=5),
  +0x1d5=11, step→2. (Nutzt +0x1e4/+0x1e8, NICHT die Flug-Paare +0x1ea/+0x1ec.)
- **step2 FINISH** 0x80114934: anim, +0x1d5-Countdown → **+0x4=7**, flags |=0x2 |=0x40.
- **GIB** 0x801149c4 (+0x5==7): 13 Kinder Stride 0xac ab +0x188, je +0x68=0x8f/+0x96=−50/+0x9a=3/
  flags|=0x4a; +0x1d5=0x32; Despawn-Loop → ebenfalls **+0x4=7** (beide Death-Lanes konvergieren).

## FLIGHT-2-Dispatcher [4] — 0x80114e54 (Substate-Tabelle @0x80121220)

Preamble: +0x82=signed-rng; +0x1ba=room_coll(a3=0x400); +0x1dc=dist; +0x1ec=0x800aca8c−+0x38;
tick(a1=0x1d) & +0x5≠3 → 0x80115d74(a0=3). Substates (Nested-Step-Machine, keyed auf +0x6):
`[0]Ascend 0x80114fb8` (vvel−120, Gravity+6, → sub1 wenn +0x1ec≥2001), `[1]Hover 0x80115130`
(Oszillation: vvel negieren wenn +0x1ec<2000), `[2]Orient 0x801152e0` (clip8), `[3]Spin 0x801153ac`
(clip4, +0x6a+=60), `[4]Dive-Arm 0x8011553c` (clip4, +0x1e4=0xc8, +0x1d5=9-const, flags|=0x2/0x40/0x8),
`[5-9]` Fade/Scripted (= die SPECIAL-Handler).

## SPECIAL [7] — 0x801157e8 (Tabelle @0x80121234)

`[0]0x80115830` Color-Fade (+0xbc/+0xbe += 10, +0xc4/+0xec Farbe = `(old&0xff000000)|0x00ffff38`),
`[1]0x80115910`, `[2]0x801159bc`/`[3]0x80115b80` geskriptetes Event (gate `0x800aca5a sltiu 5`, Overlay-
Tabellen @0x801002ec/@0x80100304; SE 0x80045024 a0=0x03020001). *(0x80116288/0x80116758 = Typ 0x26, siehe
Korrektur oben — NICHT die Krähe.)*

## Die „mode/Dive-Event-Quelle" — GETRACT: es ist KRÄHEN-FLOCKING (2026-07-06)

Die Frage „wer setzt 0x800aca50 & 0x8000/0x4000/0x2000, die die mode/Dive triggern" ist beantwortet:
**die Krähen selbst** — es ist ein Schwarm-Koordinations-Global, kein externes Event.

- **Schreiber (alle im Krähen-Code, decompile-belegt `_DAT_800aca50`):** INIT `FUN_80111a4c` (`count<<4`),
  dann setzen die Flug-Substates die High-Bits: `| 0x4000` (FUN_80112534@L69, FUN_80113694@L63,
  FUN_80112d90@L89), `| 0x8000` (FUN_8011347c@L52), `| 0x2000` (viele: FUN_80112230/80112938/80112a5c/
  80112b84/80112d90/80112ffc/801132d0/8011347c/80113694/80113c84/801132d0…), `| 0x800` (FUN_80113f38@L43,
  via `& 0xf0ff | 0x800`). Muster überall: `_DAT_800aca50 = _DAT_800aca50 & 0xfff | <bit>` (low 12 bit =
  pending-yaw, High-Bits = Flock-Kommandos). Clear: `& 0xf0ff` (FUN_80111c20/80111c9c/80113f38).
- **Leser = Dispatcher 0x80116068** (jeden ACTIVE-Tick, @0x801124e8): bit 0x8000 → zwingt Flock-Mate in
  **Sub-State 15**, 0x4000 → **14**, 0x2000 → **17** (via 0x80115d74), gated auf grid/+0x5/+0x1d8.
- **Volle 18-Einträge-Sub-State-Tabellen** (NICHT 12 — die Flymove-RE hatte nur [0-11]):
  - Steer @0x8012113c: `[12/13/15/16]=0x80113c0c [14]=0x80114100 [17]=0x80112a28(ret)`.
  - Move @0x80121184: `[12]=0x80113c7c [13]=0x80113e94 [14]=0x8011413c [15]=0x8011431c [16]=0x80114484
    [17]=0x80114594`. → Sub-States 12-17 = die **geflockten Koordinations-Manöver** (der Schwarm-Dive).

→ **Konsequenz für den Port:** Der Sturzflug ist kein isoliertes Feature, sondern das **volle
Flug-Brain**: der Dispatcher 0x80116068 + die 18 Flug-Substates (0-17) + ihre Flag-Bedingungen + die
Korridor-Soaring (die die Krähe erst auf Dive-Höhe bringt). Ein Teil-Dive kann nicht byte-true sein (die
Dive-END `y<perch−3600` setzt die ersoarte Höhe voraus). = ein dedizierter Mehr-Sitzungs-Port des
kompletten Schwarm-Flug-Systems, jetzt vollständig gemappt.

## Port-Plan (Wave-basiert)

**Port-Infrastruktur ist Wave-1-bereit** (verifiziert): RDT ROOM10C0.RDT vorhanden; `op_sce_em_set` spawnt
Typ 0x21; Modell EM021 lädt generisch aus **CDEMD0.EMS @Index 8** (Offset 0x17b800, 69944 B, valides
8-Sektionen-EMD) via `pc_enemy_load(0x21)`; NPC-Render-Loop zeichnet 0x21 generisch. **Einzige Lücke =
der KI-Tick** (`run_all` gated auf 0x10/0x11/0x16, [enemy_ai_common.c:2457]).

1. **✅ Wave 1 — Spawn + Flug (PORTIERT, Commit 9d3ce206):** `re15_crow_ai_tick` in enemy_ai_common.c;
   Branch `else if (t==0x21)` in run_all; INIT + ACTIVE-Cruise (Yaw-Slew rate 50 + Höhen-Oracle +
   vvel-Integration + Horizontal-Advance). Live: 3 Krähen kreisen/fliegen zum Spieler in ROOM10C0,
   Modell rendert. Test test_crow_ai.
2. **✅ Wave 2 — Volles Flug-Brain PORTIERT + LIVE (Commit folgt):** das komplette Schwarm-Flug-System
   in `re15_crow_ai_tick` (enemy_ai_common.c): Root-Pre-Pass + ACTIVE-Orchestrator (sense → flock-
   dispatch 0x80116068 → steer[+0x5] → move[+0x5]) + **alle 18 Substates** (Patrol 0-3, Dive-Launch 4,
   Second-Arc 5, Cruise 6, Crash-Dive 7, Land 8, Resume 9, Cruise-to-Player 10, **Dive-Attack 11 (−4)**,
   **Grapple 12 (−8)**, **Grab-Feed 13**, Bank 14, Homing 15, **Strike 16 (−4)**, Plunge 17) + der Flock-
   Dispatcher + die 4-Pfad-Dive-Decide + der KILL-Broadcast (0x2000). Shared `s_crow_flock` (=0x800aca50),
   Reset in re15_enemy_reset. **Live verifiziert** (ROOM10C0): Krähen perchen bis Spieler<5000, dann
   Patrol→Dive→Grapple→GRAB (Spieler HP 100→92)→Bank→Resume→Re-Grapple→Strike, kontinuierlich, kein
   Freeze. Test test_crow_ai (37/37).

   **Byte-true-Härtung (Commits 773b20af + Folge — „100% byte-true"):**
   - **Anim-Takt**: die echten EM021-Clip-Frame-Counts (23/35/21/16/10/24/33/40/8/30/40/36/15/16, aus
     dem Modell-EDD) verbatim eingebettet; re15_crow_anim = byte-true 0x8001f314 (POST-inc +0x95,
     +0x8f-Blend-Decay, Wrap an echter Clip-Länge). Krähen aus re15_actors_anim_advance ausgenommen.
   - **Hitbox** r=200/h=180 byte-true aus dem +0x78-Dim-Block @0x801210fc.
   - **Kontakt +0x1d0**: der Root-Post-Pass-Body-Push (aec4) mit echter Krähen-Hitbox statt dist<500.
   - **ARRIVAL-Oracle** (move[10]): dist≤9000 UND facing±100 (0x8001a804). **AABB-Box** (steer[11]):
     die echte 0xf00×0x300-Box (0x8001b9b4).
   - **+0x1d2 Parität** = der frame-parity/blink-Toggle (0x8001bc08 & 1; kein Ground-Floor-LOS für
     einen Flieger). **mode +0x1d4=0 ist BYTE-TRUE für STAGE1** (Bit 0x1f wird nur in STAGE3/STAGE5
     gesetzt, nie STAGE1 — root @0x80112048). **+0x1db=1 ist BYTE-TRUE** (FUN_80111a4c: grid=0 → 1).
   - **Grab-Player-Pin**: s_player_grabbed für Krähe sub 13 (0x800aca58=5) → game_step skippt
     re15_player_tick, wie der Zombie-Grab.

   **✅ PRÄSENTATION PORTIERT (Audio):** alle Se-Calls (0x800453d0 → re15_audio_room_se) an den
   zitierten Punkten verdrahtet — Patrol Se(6) @0x801129f4, Dive-Launch Se(1) @0x80112a7c + frame-8-
   Flap-Se(1), Second-Arc/Cruise Se(0), Cruise-to-Player Se(1) @0x801135e8, Dive-Hit Se(4) @0x80113ae8,
   Grab-Peck Se(2) @0x80113f74 + Re-Peck/Chirp, Bank Se(0) @0x80114188; **Screech 0x801161e8** (distanz-
   getiert nach +0x1ec: 1500/3000/3600) an Dive-Hit @0x80113b7c + Strike @0x801144bc. Die Krähe cawt/
   screecht jetzt durch den ganzen Flug + Angriff (war stumm). Die Angriffs-**Animation** (clip 8) rendert
   bereits byte-true über den generischen NPC-Render + die echten Clip-Längen.

   **Verbleibend (ein Render-Overlay):** 0x80019700 = der ESP-Model-Instance-Spawner (`model_inst_pool`
   @0x800a73b8) spawnt beim Strike/Grab einen sekundären Render-Effekt aus der krähen-spezifischen
   Clip-/Pose-Tabelle 0x8012110c (a0=Pose-Selektor +0x1d7<<11 / 0x2000, a3=0x8012110c). Das exakte
   Pose-Format dieser Tabelle ist noch nicht RE'd → nicht geraten (das würde einen falschen Effekt
   spawnen). Die sichtbare Angriffs-Pose = clip 8 (die byte-true Animation) ist DA; der 0x80019700-
   Overlay ist ein zusätzlicher Effekt-Layer (nächster RE-Schritt: das 0x8012110c-Pose-Format).
3. **Wave 3 — Death**: Downed-Promotion 4→3, Fall-from-sky + Land + Gib, State 7 (RE komplett).
4. **Dynamik-Verify**: Savestate ROOM10C0/1120 (JUMP hex) + `re15_enemy_state.py` mit NEUER Krähen-
   Label-Map (die Zombie-Map @0x8011f7b4 passt NICHT — eigene Root-Tabelle @0x8012111c).

## Offene Leaves (ehrlich markiert)

- Body von `0x8001af5c` (Schatten- vs. Kollisions-Register) + `0x800453d0` (Se-Variante) + `0x8004ef90`
  — geteilte EXE-Leaves, nicht am Krähen-Verhalten hängend (Namen via Adresse zitiert).
- Exakte EXE-Instruktion, die +0x9|0x40 (Downed-Bit) setzt — im geteilten Hit-Pfad um FUN_80011f50 /
  0x8001b9b4 (nächster Weg, falls je gebraucht — für den Port genügt: HP≤0 → Downed → Promotion).
