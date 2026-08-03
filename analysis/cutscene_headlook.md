# Cutscene-Head-Look (Plc_neck): Leons verdrehter Kopf in ROOM10D0 — GLOBALER Port-Defekt

**Datum:** 2026-08-03 · **Trigger:** Nutzer-Report „ROOM10D0 Marvin-Cutscene: Leons Kopf ist komisch verdreht"
**Status:** Ursache **BELEGT** (Messung + Disasm). Kein Engine-Code geändert — Diagnose-Probe
`re15_port/tests/unit/probe_headlook_10d0.c` (Build-Target `probe_headlook_10d0`, kein ctest).

---

## 0. Ergebnis in einem Satz

Der RE1.5-`Plc_neck`-Opcode zielt auf das **per-Thread-Work-Entity** (`lw v1,0x154(a0)` @0x80041e9c) —
der Port hardcodet stattdessen **immer den Spieler** (`scd_vm.c op_plc_neck`,
`g_actors[RE15_ACTOR_SLOT_PLAYER]`). In ROOM10D0 sub21 gehen 5 der 9 `Plc_neck` an
**Work_set(2,0) = MARVIN**; der Port dreht damit LEONS Kopf auf Marvins Blickziele
(u.a. auf **Leons eigene Position** → degenerierter atan2 → Pitch/Yaw weit über den
Original-Klemmen). Zusätzlich fehlen im Port: der komplette **NPC-Head-Look**
(Original: jeder STAGE1-NPC-Root ruft den Neck-FSM `FUN_80037358`, `jal @0x8011c69c`),
die **Winkel-Klemmen** (Spieler ±0x200/±0x138 @0x800319b0/b8), die **Modi 2/3/4**
(relative/sweep) und das **Gameplay-Auto-Look** (state-1-Prolog @0x80031e04 +
`FUN_8003703c(0xfa0)` @0x80031e20). Game-weit betroffen: **147 von 351 `Plc_neck`
in 28 Räumen** aller 6 Stages → globaler Fix nötig (vom Nutzer ausdrücklich erlaubt).

---

## 1. MESSUNG (Schritt 1 des RE-Gates)

Probe: `re15_port/tests/unit/probe_headlook_10d0.c` — lädt `STAGE1/ROOM10D0.RDT` +
`PLD/PL00.EDD/EMR`, feuert `scd_event_fire(21)` (sub21 = Marvin-Szene; AOT slot 0x12
@sub00+0x36: x=4700 z=19400 w/h=800, Event 0x15), tickt 900 Frames und dumpt pro Tick
Leons `neck_flags`/Target/Akkumulator + den tatsächlichen Kopf-Vorwärtsvektor aus
`re15_skel_compute_pose` (Bone 8).

**Lauf A (Port-Ist):**

```
t=  31 fl=0x84 tgt=(5875,0,19825) dXZ=(775,25)    ← MARVINS Plc_neck (Work_set(2,0)!) landet auf LEON
t= 169 fl=0x84 tgt=(5775,0,23625) acc(yaw=-901, pit=139)   ← Yaw -901 = 79° über die Schulter
t= 573 fl=0x84 tgt=(5850,0,25100) acc(yaw=-708, pit=419)   ← Pitch 419 > Klemme 312
SUMMARY: max|neck_yaw|=901 (Original-Klemme 0x200=512)  max|neck_pitch|=426 (Klemme 0x138=312)
         Ticks über Yaw-Klemme=539/900   über Pitch-Klemme=227/900
```

**Lauf B (Port-Neck-Pfad stummgeschaltet, sonst identisch):**
`max|neck_yaw|=0, max|neck_pitch|=0` — Kopf bleibt auf sauberen Keyframes.
→ **Der Twist kommt zu 100 % aus dem Port-Neck-Pfad, nicht aus RBJ/Keyframes/Bone-Mapping.**

**Referenz ROOM1150 (gilt live als korrekt):** alle 10 `Plc_neck` dort sind
`Work_set(1,0)` (= Spieler) + Modus 1/0 mit fernem Ziel (Irons (-22000,-720,-26131)) —
exakt die Teilmenge, die der Port implementiert. Darum fällt der Defekt dort nicht auf.
(Der Proben-Lauf `event 8` startete die Szene im isolierten Harness nicht — Gate;
für die Diagnose irrelevant, das SCD-Listing oben ist der Beleg.)

### 1.1 Warum „komisch verdreht" (Mechanik des Fehlbilds)

sub21 @0x0820: `Work_set(2,0)` → `Plc_neck(1, 5875,0,19825, 100)` = **Marvin soll zu
Leon schauen**. Der Port schreibt das Ziel auf Leon: Ziel ≈ Leons eigene Position →
`dXZ=(775,25)`, `dy = KopfWelt-Y(≈-1450) - 0` → `atan2(1450, 775)` ≈ 62° Pitch — mehr als
das Doppelte der Original-Klemme (±0x138 = 27°); der Yaw-Anteil läuft ohne Klemme bis
-901 (Original: ±0x200). Danach halten `Sleep(100)+Sleep(30)` den Zustand ~130 Ticks —
Leon starrt mit verrenktem Hals auf seine eigenen Füße, während Marvin spricht.
Marvins Kopf bewegt sich nie (Port hat keinen NPC-Neck-Pfad).

---

## 2. ORIGINAL-Mechanismus (vollständig, mit Adressen)

### 2.1 Opcode-Handler `Plc_neck` (0x41) @0x80041e98

```
80041e98 lw   v0,0x1c(a0)        ; thread->pc
80041e9c lw   v1,0x154(a0)       ; ★ ZIEL = per-Thread-WORK-ENTITY (nicht der Spieler!)
80041ea0 lbu  a1,0x1(v0)         ; mode = pc[1]
80041ea4 ori  v0,zero,0x80
80041ea8 sb   v0,0x1b8(v1)       ; +0x1b8 = 0x80 (Speed-Override-Bit), dann OR Modus-Bits:
  caseD_0 @0x80041ed8: |0x12   caseD_1 @0x80041ee4: |0x04   caseD_2 @0x80041ef0: |0x08
  caseD_3 @0x80041efc: |0x2a   caseD_4 @0x80041f08: |0x58
80041f20/f28 lhu pc[2] → sh +0x160   ; Ziel X   (Modus 4: Sweep-ZÄHLER)
80041f34/f3c lhu pc[4] → sh +0x162   ; Ziel Y   (Modus 2: relativer Yaw)
80041f48/f50 lhu pc[6] → sh +0x164   ; Ziel Z   (Modus 2: Pitch-Betrag)
80041f5c lh  a1,0x8(v0)              ; speed s16
80041f68 sb  a1,0x9e(v1)             ; low  byte → Yaw-Step-Override
80041f6c sb  v0,0x9f(v1)             ; high byte → Pitch-Step-Override  (srl 8 @0x80041f64)
```
Der Port-Store (Modus-Bit-Tabelle `{0x12,0x04,0x08,0x2a,0x58}`, LE-s16-Operanden,
Speed-Split low/high) ist byte-true — **nur das Ziel-Entity ist falsch.**

### 2.2 Work-Entity-Bindung (thread+0x154) — Work_set-Handler @0x80040d40-0x80040e14

- kind 1 → Spieler `0x800aca54` (@0x80040da4-db0)
- kind 2 → SPL/NPC-Array `0x800ACC2C + idx*0x1F4` (@0x80040db4-dcc) — **Marvin = (2,0)**
- kind 3 → `0x800B3F98 + idx*0x94` (@0x80040dd4-dec)
- kind 5 → Pointer-Tabelle `DAT_800B23F4[idx]` (@0x80040df4-e04)
(indirekte Variante 0x53 @0x80040e18 analog, via `DAT_800B0FD0`-Tabelle.)
Der Port trackt das bereits korrekt in `t->work_slot` (`scd_work_bind`) — `op_plc_motion`
und `op_plc_dest` benutzen es, **`op_plc_neck` nicht**.

### 2.3 Der Neck-FSM `FUN_80037358` (EXE) — läuft für Spieler UND NPCs

Aufrufer:
- **Spieler:** `FUN_80031c44` (Per-Frame-Player-Update) `jal @0x80031d78`, nach dem
  State-Dispatch `PTR_LAB_80073f90[state]` und der Pose (`FUN_8002dc48`).
- **NPCs (STAGE1):** alle sechs NPC-Roots rufen ihn mit `DAT_800ac784` = NPC:
  `FUN_8011c5a0` (Typ **0x40 = Marvin**, installiert @`_DAT_80072cac` in FUN_8011e864,
  `jal 0x80037358 @0x8011c69c`), ebenso FUN_8011cb70 (0x42), FUN_8011d140 (0x45),
  FUN_8011d6d4 (0x47), FUN_8011dc68 (0x49), FUN_8011e22c (0x4b).
  Gate im Root: `!(DAT_800aca40 & 0x20000000) && !(entity+9 & 0x20)` (globaler AI-Freeze).

Ablauf (Entity `wk`, Kopf-Part `p = modelpool(+0x188) + wk[+0x1b9]*0xac`):

1. **Gate:** `if (wk+0x1b8 & 0x01) return;` — Bit 0 schaltet den FSM ab (kein Shipped-Pfad setzt es).
2. **Default-Ziel (keine Modus-Bits):** Welt-Position des **Kopf-Parts des Ziel-Entities**
   `wk+0x1a8` (dessen `+0x1b9`-Part, Part+0x54/58/5c) → „schaue Entity X an".
3. `FUN_8003790c(p+0x54, ziel, &winkel)` — Welt-Yaw/Pitch via `catan`
   (`0x1000-catan(dz*4096/dx)` mit Quadranten-Korrektur; Pitch über
   `SquareRoot0(dx²+dz²)`, `dy = origin.y - ziel.y`). Port-Äquivalent
   (`re15_atan2_q12 - 1024`, `re15_squareroot0`) ist verifiziert byte-true.
4. **Modus-Bits von +0x1b8** (nach 0x80|x aus 2.1):
   - `0x04` Welt-Look-at: Ziel = s16-Weltpunkt `+0x160/162/164` (Modus 1). ✅ im Port
   - `0x08` relativ: Yaw = bodyYaw + heading + `+0x162`; (Basis von Modus 2/3/4)
   - `0x40` Yaw-Sweep („Kopfschütteln"): `+0x160` = Zähler; erster Pass setzt Ziel =
     volle Yaw-Klemme (`p+0x9c`); bei Snap-Arrival Spiegelung @0x80037850-58
     (`+0x162 = 2*bodyYaw + 2*heading - alt`), Zähler--, bei 1→0: `+0x1b8 = 0x12`
     (@0x80037698). Modus 4 = 0x58 = Sweep + Pitch-Null + relativ.
   - `0x20` Pitch-Analogon des Sweep (Nicken), Arrival-Logik @0x80037858-Region,
     Abschluss ebenfalls `+0x1b8 = 0x12` (@0x80037858).
   - `0x02` Ziel = aktueller Keyframe (→ Akkumulator läuft auf 0 = Release)
   - `0x10` Pitch-Ziel = 0
   - `0x80` Step-Quelle = SCD-Speed-Bytes `wk+0x9e/0x9f` statt Part-Defaults
5. **Klemmen:** Yaw auf ±`p+0x9c`, Pitch auf ±`p+0x9e` (Werte s.u.); Überlauf wird auf
   die Klemme mit Vorzeichen gesetzt (`if (clamp*2 < ((res+clamp)&0xfff)) …`).
6. **Slew:** Akkumulator `p+0x94` (Yaw) / `p+0x96` (Pitch) bewegt sich pro Frame um
   konstanten Step (`p+0x98/0x9a` bzw. Override `wk+0x9e/0x9f`) auf
   `ziel - (bodyYaw+heading+keyframe)` zu, mit Snap innerhalb 2*Step.
7. **Anwendung:** `p+0x62 (vy) += p+0x94; p+0x64 (vz) += p+0x96; RotMatrix(p+0x60 → p+0x18);`
   danach beide Subtraktionen zurück (@0x80037870-Region) — der Offset lebt NUR im
   Part-Matrix-Rebuild, nie in den Keyframe-Winkeln. (Port-Äquivalent: additiv in
   `re15_skel_compute_pose`, Akkumulator in `actor->neck_yaw/pitch` — Struktur korrekt.)

### 2.4 Initialisierung / Reset (die Klemmen- und Step-Quellen)

**Spieler** (State-0-INIT `LAB_800318f8`, Tabelle `PTR_LAB_80073f90[0]` @0x80073f90):
```
80031938 sb 8   → wk+0x1b9        ; Kopf-Bone-Index = 8 (PL00)
80031974 sb 0   → wk+0x1b8        ; Neck-Flags = 0
800319a4/a8 ori 0x60; sh → part8+0x98 ; Yaw-Step  = 96
800319ac        sh → part8+0x9a       ; Pitch-Step = 96
800319b0/b4 ori 0x200; sh → part8+0x9c ; Yaw-KLEMME  = ±512 (±45°)
800319b8/c4 ori 0x138; sh → part8+0x9e ; Pitch-KLEMME = ±312 (±27.4°)
800319bc/c0 sh 0 → part8+0x94/0x96     ; Akkumulatoren = 0
```

**NPC Typ 0x40 (Marvin), INIT `FUN_8011c6dc`** (State-Tabelle @0x80121598, [0]=INIT,
[4]=0x80050be8 State-4-Executor):
```
8011c738 sw &player(0x800aca54) → wk+0x1a8  ; Default-Blickziel = SPIELER
8011c748 sh -1  → wk+0x9a                   ; „lookable"-Marker (FUN_8003703c-Kategorie)
8011c758 sb 0x78 → wk+0x9e                  ; Default-Speed-Override 120
8011c768 sb 0   → wk+0x1b8                  ; Flags = 0 → TRACK-PLAYER-Modus!
8011c778 sb 8   → wk+0x1b9                  ; Kopf-Bone-Index = 8
8011c790 sh 0x40  → part8+0x98              ; Yaw-Step 64
8011c798 sh 0x30  → part8+0x9a              ; Pitch-Step 48
8011c7a0 sh 0x2c8 → part8+0x9c              ; Yaw-Klemme ±712 (±62.6°)
8011c7a8/ac sh 0  → part8+0x94/0x96         ; Akkumulatoren
8011c7b0 sh 0x138 → part8+0x9e              ; Pitch-Klemme ±312
```
**SPL-Spawn-Opcode** (Sce_em_set-SPL-Pfad): nach dem INIT-Dispatch
`@0x8004260c-18: if (entity+9 & 0x40) → sb 0x12 → +0x1b8` (Cutscene-gespawnte NPCs
starten neck-idle statt Player-Tracking).

### 2.5 Gameplay-Gates & Auto-Look (Reset pro Frame)

Spieler-State-1 (`LAB_80031de8`, Idle/Control):
```
80031e04 ori v0,v0,0x12 ; sb → wk+0x1b8    ; JEDEN Tick: Flags |= 0x12 (Idle-Release)
80031e10 lbu DAT_800aca59; bne 7 →         ; außer im Walk-Substate 7
80031e20 jal FUN_8003703c(a0=0xfa0)        ; nächstes „lookable" SPL-Entity in 4000 Units
80031e3c andi v0,0xed; sb → wk+0x1b8       ; gefunden → Flags &= ~0x12 = 0x00 = ENTITY-TRACKING
```
→ Im normalen Gameplay **trackt Leons Kopf automatisch den nächsten NPC** (Flags 0
= Modus „schaue wk+0x1a8 an", Klemmen/Steps aus den Part-Defaults). `FUN_8003703c`
wählt per 2D-Distanz + Kategorie aus `wk+0x9a`-Vorzeichenbits und schreibt
`DAT_800acbfc` (= Spieler+0x1a8). Weitere Call-Sites: @0x80031c2c (INIT, r=100),
@0x80032fa8/0x800340f0/0x80034f78/0x800355cc (weitere Player-States),
`FUN_80037250` (Rotations-Scan) @0x80033f50.
**Ende einer Cutscene:** beide Skripte schließen mit `Plc_neck(0,…)` (=0x92, Release);
zusätzlich stellt der State-1-Prolog `0x12` jeden Frame wieder her — es gibt also
zwei redundante Reset-Wege.

---

## 3. PORT-IST vs. Original (Zeile für Zeile)

| # | Original | Port | Diagnose |
|---|---|---|---|
| P1 | Ziel = `thread+0x154` (@0x80041e9c) | `op_plc_neck` (scd_vm.c:1945-1980): fest `g_actors[RE15_ACTOR_SLOT_PLAYER]` | ❌ **Hauptdefekt** — Marvins Blickziele landen auf Leon; `t->work_slot` existiert bereits und wird von Plc_motion/Plc_dest korrekt benutzt |
| P2 | FSM läuft für JEDES Entity mit `+0x1b8`-Daten; alle 6 NPC-Roots rufen ihn (@0x8011c69c u.a.) | Neck-Zweig nur `bact == &g_actors[PLAYER]` (skeleton_common.c:328) | ❌ NPCs haben im Port keinerlei Head-Look (Marvin schaut nie zu Leon) |
| P3 | Klemmen ±`part+0x9c`/±`part+0x9e` (Spieler 0x200/0x138 @0x800319b0/b8; NPC 0x2c8/0x138 @0x8011c7a0/b0) | fehlen komplett (Kommentar „not yet extracted") | ❌ gemessen: Yaw -901, Pitch 426 — die Klemmen hätten exakt das „Verdrehte" begrenzt |
| P4 | Modi: 0x04 Welt-Ziel, 0x08 relativ, 0x40/0x20 Sweep/Nicken (Kopfschütteln, Spiegelung @0x80037850, Abschluss→0x12 @0x80037698), 0x02 Keyframe, 0x10 Pitch 0, Flags 0 = Entity-Tracking | nur `0x80|0x04` aktiv; alles andere = Release-Ease | ❌ Modus 2 (Kopf senken, 23 Fälle) und 4 (Kopfschütteln, 33 Fälle) fehlen; 10D0 nutzt beide (0x09a8/0x09b6/0x09e2/0x09f0) |
| P5 | Step-Quelle: Part-Default (`+0x98/0x9a`) außer Bit 0x80 → SCD-Bytes `wk+0x9e/0x9f` | nur SCD-Bytes (`neck_speed`), Default-Steps fehlen | ⚠️ wirkt erst, wenn Flags-0-Tracking/State-1-Pfad portiert wird |
| P6 | Kopf-Bone = `wk+0x1b9` (Spieler 8 @0x80031938; NPC 8 @0x8011c778; andere Typen z.B. 0xe) | hart `b == 8` | ⚠️ für Spieler+STAGE1-NPCs zufällig korrekt; für Nicht-8-Typen falsch, sobald P2 gefixt wird |
| P7 | Reset: INIT `+0x1b8=0` (@0x80031974), State-1-Prolog `|=0x12` (@0x80031e04), SPL-Spawn `0x12` (@0x80042618) | kein Reset außer Script-`Plc_neck(0)`; `neck_flags` überlebt Raumwechsel | ⚠️ mitigiert durch Script-Releases; Restrisiko Raum-übergreifender Stale-State |
| P8 | Gameplay-Auto-Look (FUN_8003703c, Radius 4000 @0x80031e24) | fehlt komplett | ❌ Basis-Verhalten „Leon schaut NPCs an" fehlt (fällt vor jeder NPC-Szene auf) |
| P9 | Store-Seite des Opcodes (Modus-Bits, LE-Operanden, Speed-Split) | byte-true | ✅ |
| P10 | atan2/sqrt/Slew/Snap-Mathe, Anwendung yaw→vy pitch→vz | byte-true (re15_atan2_q12-1024, re15_squareroot0, skeleton_common.c:363-386) | ✅ |

---

## 4. Global-Scope (Census über alle 240 RDTs)

Scan `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`, main+sub, mit Work_set-Kontext
(Script im Scratchpad `neck_census.py`):

- **351 `Plc_neck` gesamt** · Modus-Verteilung: mode0=91, mode1=185, mode2=23, mode3=17, mode4=33
- **147 an `Work_set(2,…)` = NPC** in **28 Räumen**:
  1021 10D0 11B0 11B1 11C0 1211 2000 2001 3000 3001 3020 3061 3070 3071 3080 3091
  30C1 30E0 30E1 4000 4001 4010 4031 5011 50B0 6000 6030 6031
- ROOM1150 (Referenz „korrekt"): 10/10 an `Work_set(1,0)` + nur Modus 1/0 — deckt genau
  die funktionierende Port-Teilmenge ab; darum blieb der Defekt dort unsichtbar.
- (2 Census-Zeilen „mode=79" = Offline-Walker-Desync in Daten-Regionen, bekanntes
  Artefakt [reai-v2-scd-pad-predicates] — keine echten Opcodes.)

→ **Der Defekt ist NICHT raumspezifisch.** Raumspezifisch an ROOM10D0 ist nur, dass es
der erste Raum der Kampagnen-Kette ist, der NPC-`Plc_neck` + Modi 2/4 kombiniert.

## 5. FIX-PLAN (alle Punkte global; Nutzer hat globale Fixes erlaubt)

1. **`op_plc_neck` auf das Work-Entity routen** (@0x80041e9c): Ziel-Actor =
   `t->work_slot >= 0 ? t->work_slot : PLAYER` — identisch zum bestehenden Muster in
   `op_plc_motion`/`op_plc_dest`. Die `neck_*`-Felder existieren bereits auf jedem
   `re15_actor_t`. (Behebt: Leons Twist UND „Marvin schaut nie".)
2. **Neck-Zweig in `re15_skel_compute_pose` ent-hardcoden:** Bedingung
   `bact != NULL && bone == bact->neck_bone` statt `b==8 && bact==PLAYER`;
   `neck_bone` beim Actor-Setup setzen (Spieler 8 @0x80031938; NPC-Typen 0x40-0x4b: 8
   @0x8011c778 u. Geschwister; Default 8). NPC-Renderpfad setzt `g_anim_pose_actor`
   bereits pro Actor.
3. **Klemmen + Default-Steps einbauen** (`neck_yaw_clamp/pitch_clamp/step`-Felder):
   Spieler 96/96, ±0x200/±0x138 (@0x800319a4-c4); NPC 64/48, ±0x2c8/±0x138
   (@0x8011c790-b0). Klemm-Semantik: Ziel-Reduktion VOR dem Slew auf ±Klemme setzen
   (`@0x80037458-64`-Äquivalent), nicht den Akkumulator kappen.
4. **Modi 2/3/4 (Bits 0x08/0x20/0x40/0x10/0x02) portieren** (FUN_80037358):
   relativer Yaw (+0x162 als Offset auf bodyYaw+heading), Pitch-Betrag +0x164,
   Sweep-Zähler +0x160 mit Spiegelung @0x80037850-58 und Abschluss `flags=0x12`
   @0x80037698 — deckt „Kopf senken" (23×) und „Kopfschütteln" (33×) ab.
5. **Flags-0 = Entity-Tracking** (Ziel = Kopf-Part-Weltposition von `+0x1a8`):
   Voraussetzung für 6.; `neck_target_actor`-Feld (NPC-INIT-Default = Spieler
   @0x8011c738).
6. **Gameplay-Auto-Look** (separat schätzbar, sichtbares Basis-Feature):
   State-1-Prolog `flags|=0x12` (@0x80031e04) + `FUN_8003703c`-Portierung
   (Kategorien aus `wk+0x9a`-Vorzeichen, 2D-Distanz, Radius 4000 @0x80031e24,
   weitere Radien je Call-Site) + NPC-Seite (NPC-Roots rufen den FSM jeden Frame,
   Flags 0 ab INIT @0x8011c768 → Marvin verfolgt Leon mit dem Kopf schon VOR der Szene).
7. **Reset-Härtung:** beim Raumwechsel/Player-Re-Init `neck_flags=0`,
   Akkumulatoren 0 (Äquivalent @0x80031974/0x800319bc) — schützt gegen Stale-State,
   falls ein Skript ohne `Plc_neck(0)` endet.

Reihenfolge 1→3 beseitigt den gemeldeten ROOM10D0-Twist; 4→5 macht die Szene
byte-true (Kopf senken/schütteln); 6 ist das fehlende Basis-Gameplay-Feature.

## 6. OFFEN (ehrlich)

- **O1:** `FUN_8003703c`-Kategorienlogik (Vorzeichen-Bits von `wk+0x9a`, drei
  Prioritätsklassen) ist decompiliert, aber die Bedeutung der Klassen (freundlich/
  feindlich?) ist nicht verhaltens-verifiziert; ebenso die Radien der übrigen
  Call-Sites (@0x80032fa8/0x800340f0/0x80034f78/0x800355cc, `FUN_80037250`).
- **O2:** Wann genau der Spieler-State-0-INIT (LAB_800318f8) erneut läuft
  (Raumwechsel vs. nur Spielstart) ist nicht end-verfolgt — für Fix 7 als
  konservatives „bei Raumwechsel zurücksetzen" ausreichend, byte-true-Timing offen.
- **O3:** Der ROOM1150-Referenz-Lauf der Probe startete sub08 im isolierten Harness
  nicht (Szenen-Gate ungeklärt) — Diagnose stützt sich dort auf das SCD-Listing.
- **O4:** Modus 3 (0x2a) ist statisch hergeleitet (Bits 0x02|0x08|0x20 = relativer
  Sweep/Nicken); ein Live-PSX-Vergleich (DuckStation, z.B. ROOM11B1 sub02) steht aus.
- **O5:** `+0x1b9`-Werte anderer Entity-Typen (0xe/0/1/2/3 in STAGE1-INITs) sind
  erfasst, aber nur die NPC-Typen rufen den FSM — ob ein Nicht-NPC-Typ ihn in
  STAGE2-6-Overlays ruft, wurde nicht per Overlay-Scan aller Stages geprüft.

## 7. Artefakte

- Probe: `re15_port/tests/unit/probe_headlook_10d0.c` (+ CMake-Eintrag, kein ctest)
  — Build: `PATH=/c/msys64/mingw64/bin:$PATH cmake --build re15_port/build --target probe_headlook_10d0`
  (⚠️ ohne PATH-Prefix schlägt cc1 dieser Umgebung STILL fehl, RC=1 ohne Fehlertext)
- Census-Script: Scratchpad `neck_census.py`; ROOM10D0-SCD-Dump: Scratchpad `rdt10d0_out.txt`
- Original-Quellen: `RE_15_Quellcode_V2/FUN_80037358.c` (+ eigene Disasm-Zitate oben),
  `FUN_8003790c.c`, `FUN_8003703c.c`, `FUN_80037250.c`, `FUN_80031c44.c`;
  Overlay `RE_15_Quellcode_Overlays/STAGE1_full/FUN_8011c5a0.c`/`FUN_8011c6dc.c`
  (disasm-verifiziert via `re15_disasm.py`, STAGE1.BIN ohne Header)
