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

## DAMAGE-MODELL (verifiziert — die Krähe macht KEINEN direkten Schaden)

**Byte-true über ALLE Krähen-States geprüft: kein Spieler-HP-Write, kein Player-Command-Write
(0x800aca58 nur als READ im Root = das Standard-Player-State-Gate), kein Damage-Entry-Call
(0x80012d60/0x80011f50), keine Hitbox (0x8002b5xx), keine HP-Arithmetik.** Die Proximity-Helfer
0x80012974/**0x80012a0c** setzen ein Flag auf der **EIGENEN Krähen-Entity** (`sw v1,0(cur_entity)`,
bit 0x20000000/0x80000000 @0x80012a88, dist<6000 zum Spieler) — ein Render/LOD-Flag auf der Krähe
selbst, KEIN Spieler-Schaden. → **Die Krähe (0x21) ist ein positioneller/Harassment-Gegner; ihr
Sturzflug ist rein positionell.** Ob Krähen den Spieler im Prototyp überhaupt verletzen (via einem
geteilten Spieler-seitigen Kontakt-System) ist offen — die Krähen-KI selbst tut es nicht.

> ⚠️ **KORREKTUR (2026-07-06):** Ein früher notierter „Touch-Damage `player.hp −= 2` im State-7-
> Pre-Pass 0x80116288" war eine **Fehlzuordnung**. **0x80116288 ist der Root-Handler von Nachbar-
> Typ 0x26**, NICHT der Krähe — die Registrierungs-Tabelle @0x8011e8e4-904 lädt drei getrennte
> Roots: `0x80112020` (Krähe 0x21), `0x80116288` (Typ 0x26), `0x80116db8` (Typ 0x27). Der Krähen-
> Root ruft 0x80116288 NIE. Der Death/Special-CLUSTER-Agent war in 0x26s Code gewandert; der
> adversariale Verify prüfte die Adressen (die den −2-Code DO enthalten), aber nicht die Typ-
> Zugehörigkeit — die Disziplin-Falle „ein Nachbar-Handler ≠ dieser Typ". Die 0x80116288/
> 0x80116758-Details unten beschreiben also **Typ 0x26**, nicht die Krähe.

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
2. **Wave 2 — Schwarm-Flug + Sturzflug (GETRACT, dedizierter Port):** die „Event-Quelle" ist das
   Krähen-**Flocking** (0x800aca50, oben vollständig gemappt), kein externes Event. Byte-true portierbar,
   aber es ist das **volle Flug-Brain**: (a) den Dispatcher 0x80116068 (Flag-Leser → forced Sub-States
   14/15/17); (b) die 18 Flug-Substates (Move @0x80121184 / Steer @0x8012113c) mit ihren Flag-Set-
   Bedingungen (das Soaring, das die Krähe auf Dive-Höhe bringt); (c) DANN die Dive (0x80112628 →
   sub4/5, Dive-END `y<perch−3600`). **KEINE Touch-Damage** (0x80116288 war Typ 0x26). Ein Teil-Dive
   ist nicht byte-true (die Dive-Physik braucht die ersoarte Korridor-Höhe). = ein Mehr-Sitzungs-Port
   des Schwarm-Systems; die Struktur ist jetzt komplett bekannt.
3. **Wave 3 — Death**: Downed-Promotion 4→3, Fall-from-sky + Land + Gib, State 7 (RE komplett).
4. **Dynamik-Verify**: Savestate ROOM10C0/1120 (JUMP hex) + `re15_enemy_state.py` mit NEUER Krähen-
   Label-Map (die Zombie-Map @0x8011f7b4 passt NICHT — eigene Root-Tabelle @0x8012111c).

## Offene Leaves (ehrlich markiert)

- Body von `0x8001af5c` (Schatten- vs. Kollisions-Register) + `0x800453d0` (Se-Variante) + `0x8004ef90`
  — geteilte EXE-Leaves, nicht am Krähen-Verhalten hängend (Namen via Adresse zitiert).
- Exakte EXE-Instruktion, die +0x9|0x40 (Downed-Bit) setzt — im geteilten Hit-Pfad um FUN_80011f50 /
  0x8001b9b4 (nächster Weg, falls je gebraucht — für den Port genügt: HP≤0 → Downed → Promotion).
