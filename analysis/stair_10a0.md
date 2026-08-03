# ROOM10A0 Treppenhaus — Richtungswahl UP vs DOWN (RE-Dossier, 2026-08-03)

**Nutzer-Report:** Ein Kamera-Hintergrund zeigt zwei Treppen (vorne + hinten). Die vordere
Treppe hinuntersteigen funktioniert; löst man danach die HINTERE Treppe aus (führt nach
UNTEN), steigt Leon stattdessen HOCH.

**Ergebnis in einem Satz:** Das Original entscheidet UP/DOWN **pro Zone** aus
(a) der Rect-Hälfte, in der der Spieler entlang der Treppen-Achse steht, und
(b) dem u16-Seiten-Flag `rec+0xC` des AOT-Records — der Port dagegen paart Zonen über
`event_id`, in dem er genau dieses Seiten-Flag (nur 0|1) gespeichert hat; in ROOM10A0
(4 Treppen, side-Werte 0,0,1,1) paart Treppe C mit Treppe A und Treppe D mit Treppe B,
wodurch Ziel-Band und Richtung falsch werden. Dynamisch reproduziert
(`re15_port/tests/unit/probe_stair_10a0.c`): C-DESCEND 4→3 wird UP 4→8, D-DESCEND 3→1
wird UP 3→6 — exakt der Report.

---

## (A) Original-Mechanik der Treppen-Aktivierung

### A.1 Registrierung: der Runtime-AOT-Record IST der SCD-Byte-Stream

Aot_set-Opcode-Handler (0x2C) @`LAB_80040534`:

```
80040578 lw   v0,0x1c(a0)          ; SCD-pc
80040580 addiu v0,v0,0x2
80040584 sw   v0,0x0(v1)=>DAT_800ac9b0   ; Record-Tabelle[slot] = pc+2  (KEINE Kopie!)
80040590 lbu  v0,0x3(v1) / andi 0x80     ; Langform-Erkennung
800405ac addiu v0,v1,0x14                ; pc += 20 (kurz) / 0x1c (Polygon)
```

Record-Layout (Offsets relativ pc+2, kurze Form):

| rec | SCD | Bedeutung (Treppe sce 12/13) |
|---|---|---|
| +0 | pc[2] | sce (12 = X-Achse, 13 = Z-Achse) |
| +1 | pc[3] | flags (Treppen shippen 0x31 = enabled 0x01 + action 0x10 + fwd-Probe-Test 0x20) |
| +2 | pc[4] | **chain = Band-Gate** (Plattform, auf der dieses Zonen-Ende feuert) |
| +4 | pc[6..7] | s16 Rect-Ecke X |
| +6 | pc[8..9] | s16 Rect-Ecke Z |
| +8 | pc[10..11] | u16 Breite W |
| +0xA | pc[12..13] | u16 Tiefe D |
| +0xC | pc[14..15] | **u16 `side` ∈ {0,1} — Seiten-Flag der Richtungs-Entscheidung** |
| +0xE | pc[16] | **u8 `count` — Stufen-/Band-Zähler (nur &7 benutzt)** |

### A.2 Action-Scan `FUN_80042bac` — welche Zone feuert

```
80042bd0 ori  v0,zero,0x26c        ; Vorwärts-Probe = 620 Einheiten vor dem Spieler
80042bf8 lh   a0,0x6a(s1)          ; Spieler-Yaw → FUN_8004f008 rotiert (620,0)
80042c50 lw   s0,0x0(s4)=>DAT_800ac9b0  ; Record-Slot-Loop
80042c5c sw   s0,DAT_800bbd9c      ; aktueller Record-Ptr (liest der sce-Handler)
80042c84 lbu v0,0x1(s0); and a3    ; Gate 1: flags & scan-mask(1) — enabled
80042ca0 andi v0,0x10; bne v0,s6   ; Gate 2: action-Bit muss zum Scan-Typ passen
80042cac lbu v0,0x2(s0); andi 0x80 ; Gate 3 (BAND):
80042cc0 lbu v1,0x82(s1)           ;   wenn rec[2]&0x80 == 0:
80042ccc bne v1,v0 → skip          ;   rec[2] (chain) MUSS == player+0x82 (Band, DAT_800acad6)
80042ef8 andi v0,v1,0x20           ; Geometrie: flags-Bit 0x20 → Rect-Test mit der
80042f10/80042f20 …                ;   PROBE (620 voraus), Bit 0x40 → mit der Spieler-Pos
80042f84 lw v0,PTR_LAB_8007469c[sce]; Dispatch: Handler-Tabelle @0x8007469c
```

sce-Dispatch-Tabelle @`0x8007469c`: `[12] = LAB_80043500`, `[13] = LAB_800435cc`
(Bytes @0x800746cc `00 35 04 80`, @0x800746d0 `cc 35 04 80`).

### A.3 Die RICHTUNGS-EntscHEIDUNG — sce-12-Handler `LAB_80043500` (X-Achse)

```
80043500 lui a2,0x800c / lw a2,DAT_800bbd9c   ; a2 = AOT-Record
80043508 lui a1,0x800b / lw a1,DAT_800ac784   ; a1 = Spieler-Block (=0x800aca54)
80043510 lh  a0,0x4(a2)          ; rect_x (Ecke)
80043514 lw  v1,0x34(a1)         ; Spieler-X
80043518 lhu v0,0x8(a2)          ; W
8004351c subu v1,v1,a0           ; delta = player_x - rect_x
80043520 srl v0,v0,0x1           ; half = W/2
80043524 slt v1,v1,v0            ; low_half = (delta < half)   [signed]
80043528 beq v1,zero,LAB_80043558
8004352c _ori v0,zero,0x1
  ; --- Spieler in der NIEDRIG-Koordinaten-Hälfte ---
80043530 lhu v0,0xc(a2)          ; side (rec+0xC)
80043538 bne v0,zero,LAB_80043580   ; side != 0 → v0=2 (DESCEND)
80043540 ori v0,zero,0xb / 80043544 sb v0,0x5(a1)   ; Mode DAT_800aca59 = 0xb
80043550 j LAB_80043594 / 80043554 _ori v0,zero,0x1 ; aca5a = 1 (ASCEND)
LAB_80043558: ; --- HOCH-Koordinaten-Hälfte ---
80043558 lhu v1,0xc(a2)
80043560 bne v1,v0,LAB_80043580     ; side != 1 → v0=2 (DESCEND)
80043568 ori v0,zero,0xb / sb 0x5(a1) ; Mode 0xb
80043578 j LAB_80043594 / _ori v0,zero,0x1 ; aca5a = 1 (ASCEND)
LAB_80043580: ; --- Hälfte passt NICHT zum side-Flag ---
80043580 ori v0,zero,0xb / sb 0x5(a1) ; Mode 0xb
80043590 ori v0,zero,0x2              ; aca5a = 2 (DESCEND)
LAB_80043594:
80043594 sb v0,0x6(v1)           ; DAT_800aca5a = 1|2  ← DIE RICHTUNG
800435a0-ac DAT_800aca3c |= 0x40 ; Control-Latch
800435b8 lbu v0,0xe(a2)          ; count-Byte
800435c0 sb v0,0x9e(v1)          ; DAT_800acaf2 = count
```

### A.4 sce-13-Handler `LAB_800435cc` (Z-Achse) — identisch, nur:

```
800435dc lh  a0,0x6(a2)          ; rect_z statt rect_x
800435e0 lw  v1,0x3c(a1)         ; Spieler-Z statt X
800435e4 lhu v0,0xa(a2)          ; D statt W
...
8004367c lbu v0,0xe(a2)
80043688 ori v0,v0,0x80          ; count | 0x80  → Achsen-Flag im Zähler
8004368c sb v0,0x9e(v1)          ; DAT_800acaf2 = count|0x80
```

**Formel (belegt):**
```
delta    = player[axis] − rect_corner[axis]      (X: sce12, Z: sce13)
low_half = delta < extent/2
ASCEND  (aca5a=1)  ⇔  (low_half ∧ side==0) ∨ (¬low_half ∧ side==1)
DESCEND (aca5a=2)  sonst
```
`side` benennt also die Rect-Hälfte, die das UNTERE Treppen-Ende ist (side=0 → niedrige
Koordinaten-Hälfte, side=1 → hohe). Wer dort steht, steigt HOCH; wer in der anderen
Hälfte steht, steigt RUNTER. **Kein Paar-Lookup, kein Zonen-Ziel, keine Y-Vergleiche.**

### A.5 Konsum: Mode 0xb → 0xb (ascend) oder 0xc (descend)

Spieler-Mode-Dispatch (Doppel-Tabelle, Index = `DAT_800aca59`):
`80031ed4 lbu v0,DAT_800aca59` → `PTR_LAB_80073fb0[mode]` (Bank A) und
`80031efc … PTR_LAB_80073ff0[mode]` (Bank B). Einträge (PSX.EXE-Bytes @0x80073fb0+):
`[0xb] = 0x80038848/0x80038850` (ASCEND-Stub/Tick), `[0xc] = 0x80038c58/0x80038c60`
(DESCEND). Der sce-Handler setzt IMMER Mode 0xb; erst der Mode-0xb-Handler
`LAB_80038850` (Phase `DAT_800aca5b` 0→1) schaltet nach dem Ausricht-Settle um:

```
800389b4 lbu v0,DAT_800acaf2
800389c0 sb 2,DAT_800aca5b        ; Phase 2 (Gait)
800389c8 lbu v1,DAT_800aca5a
800389cc andi v0,v0,0x7           ; count &= 7
800389d0 addiu v0,v0,-0x1
800389d4 andi v1,v1,0x2           ; Richtungstest
800389dc sb v0,DAT_800acaf2       ; Zähler = (count&7) − 1
800389e0 beq v1,zero,default      ; aca5a==1 → bleibt Mode 0xb = ASCEND
800389e4 _ori v0,zero,0xc
800389ec sb v0,DAT_800aca59       ; aca5a==2 → Mode 0xc = DESCEND
800389f4 sb zero,DAT_800aca5b     ; Phase 0 des Descend
```

Phase 0 des Mode 0xb (@0x8003888c) setzt vorher `DAT_800acae0=0x4b`,
`DAT_800acae8=5` (Clip 5), Phase 1 (@0x800388c4-800389ac) lässt das Heading-Residual
`DAT_800acabe` geometrisch auf die nächste Achsen-Kardinale konvergieren
(X-Treppe → Vielfache von 0x800, Z-Treppe → 0x400+k·0x800; Auswahl über das
0x80-Achsen-Flag in acaf2 @0x80038920-28, ±(res>>2)&0xff @0x80038934-0x80038988,
Exit-Gate `(acabe&0x3e0)==0` @0x800389a0).

### A.6 Zähler-Semantik + Finalize (Descend `LAB_80038c60`)

```
80038cb0 Phase0: Clip DAT_800acae8=0x15, Speed acae0=0xa, Band acad6=7 (Sentinel),
         Cursor acae9=0, savedY acaf0 = Spieler-Y (lhu DAT_800aca8c)
80038df0 @Cursor 0x20: lh v0,acaf0; addiu v0,0x708 → Y += 0x708 (ein Band)
80038dfc lbu a0,acaf2  ; PRE-Decrement-Wert
80038e14 addiu v0,a0,-1 / sb → acaf2--
80038e30 beq a0,zero → LAB_80038e40 (aca5b=2 = Finalize)   ; sonst Cursor=2 (Loop)
```
⇒ **Gesamt-Band-Schritte = count-Byte (&7)** (count=2 → acaf2 startet bei 1: zwei
Y+=0x708-Zyklen; count=1 → ein Zyklus). Finalize @`LAB_80038e50`: Clip 2, Band
`DAT_800acad6 = −DAT_800acc0e · 0x91a2b3c5-Magie = −Y/0x708` (@0x80038e84-edc),
aca59/5a/5b = 0 — KEINE Repositionierung (deckt sich mit Katalog LAB_80038e50).

---

## (B) ROOM10A0-Daten (byteweise)

`re15_port/shared_assets/PSX/STAGE1/ROOM10A0.RDT`: main_scd @0xdb0 (Header u32 @0x40),
sub_scd @0xee4. Acht Aot_set-Records sce 13 (alle Z-Achse, flags 0x31, kurze Form):

| Datei-Offset | Slot | chain (Band) | Rect-Ecke (x,z) | W×D | side | count | Treppe |
|---|---|---|---|---|---|---|---|
| 0x0e40 | 3 | **8** | (20100,19500) | 2700×4300 | 0 | 2 | A (West) oben |
| 0x0e54 | 4 | **6** | (20100,19500) | 2700×4300 | 0 | 2 | A unten |
| 0x0e68 | 5 | **6** | (24000,20100) | 2700×4500 | 1 | 2 | B (Ost) oben |
| 0x0e7c | 6 | **4** | (24000,20100) | 2700×4500 | 1 | 2 | B unten |
| 0x0e90 | 7 | **4** | (20100,19500) | 2700×2300 | 0 | 1 | C (West) oben |
| 0x0ea4 | 8 | **3** | (20100,19500) | 2700×2300 | 0 | 1 | C unten |
| 0x0eb8 | 9 | **3** | (24000,20100) | 2700×4500 | 1 | 2 | D (Ost) oben |
| 0x0ecc | 10 | **1** | (24000,20100) | 2700×4500 | 1 | 2 | D unten |

Topologie (Wendel-/Switchback-Treppenhaus, Bänder = Etagen, Y = −Band·0x708):

```
Band 8 ──A(count2, side0: unten=niedriges z)── Band 6
Band 6 ──B(count2, side1: unten=hohes z)────── Band 4
Band 4 ──C(count1, side0)───────────────────── Band 3
Band 3 ──D(count2, side1)───────────────────── Band 1
```
- West-Treppen A/C teilen die x-Spanne [20100..22800]; Ost-Treppen B/D haben das
  IDENTISCHE Rect [24000..26700]×[20100..24600] — nur das Band-Gate `rec[2]` trennt,
  welcher Record feuert (Band 6→Slot 5, 4→Slot 6, 3→Slot 9, 1→Slot 10).
- Jede Treppe = 2 Records mit identischem Rect (einer pro Anschluss-Band), damit sie
  von beiden Etagen triggerbar ist. `count` stimmt jeweils mit |ΔBand| überein
  (A/B/D: 2, C: 1).
- Die Unterscheidung vordere/hintere Treppe leistet allein das Band-Gate + die
  XZ-Rect-Zugehörigkeit — NICHT die AOT-Slots untereinander.

SCA-Kollision (Sektion @0x5b8, 5 Quadranten-Partitionen à 33 Zellen): ausschließlich
Wall-Style-Rects `(w5&0xf00)==0x300` mit Band = `w5>>12` ∈ {1,3,4,6,8} — deckungsgleich
mit den AOT-chains; KEINE Slope-/Treppen-Zelltypen (2/4/5/6/7) in diesem Raum.

**Halbe-Probe (Original-Formel) für die Report-Situation:** Spieler auf Band 4 oben an
Treppe C (z≈21400): delta=1900 ≥ half=1150 → hohe Hälfte, side=0 → **DESCEND** ✓;
auf Band 3 oben an D (z≈21000): delta=900 < half=2250 → niedrige Hälfte, side=1 →
**DESCEND** ✓.

---

## (C) PORT-IST (`re15_port/engine/src/stair_common.c`) und Repro

### C.1 Divergenz-Kette (re15_stair_try_start, Zeilen 274-414)

| Port | Original | Bewertung |
|---|---|---|
| Zonen-Match `a->band == cur` (Z. 325-327) | Band-Gate `rec[2]==player+0x82` @0x80042cac-ccc | ✅ äquivalent |
| Vorwärts-Probe 800u + Spieler-Reach 450 (Z. 41-42, 308-310) | Probe **620** (=0x26c @0x80042bd0), Rect-Test NUR mit der Probe (flags 0x20 @0x80042ef8) | ⚠️ klein: 800/450 ohne PSX-Beleg |
| **Paar-Suche über `event_id` (Z. 341-348)** | **existiert NICHT** — Richtung = Rect-Hälfte × side (A.3) | ❌ Kernfehler: event_id trägt data0/side (nur 0/1; scd_vm.c Z. 2163/2176 → aot_common.c Z. 97-107) |
| `target = paired->band` (Z. 349) | Ziel implizit: `cur ± (count&7)` (A.5/A.6) | ❌ folgt aus dem falschen Paar |
| `s_ydir = (target<cur)?+1:-1` (Z. 368) | `aca5a` aus Hälfte×side @0x80043594 | ❌ falsch, sobald das Paar falsch ist |
| `s_bands_left = |target−cur| − 1` (Z. 369-370) | `DAT_800acaf2=(count&7)−1` @0x800389cc-dc | ⚠️ nur äquivalent, wenn Paar korrekt |
| Facing = atan2 auf die Paar-Zone (Z. 381-383, 398) | acabe-Decay auf die nächste Achsen-Kardinale (A.5) | ❌ zielt mit falschem Paar auf die falsche Treppe |

ROOM1170 funktionierte nur, weil dort GENAU eine side-0- und eine side-1-Treppe existiert
(Slots 7/8 side1, 9/10 side0) — die event_id-Paarung fand dort zufällig den richtigen
Partner. ROOM10A0 hat je ZWEI side-0- und side-1-Treppen → die Paar-Suche (first match,
Z. 341-348) bindet C(7/8)→A-Slot-3 und D(9/10)→B-Slot-5.

### C.2 Dynamische Reproduktion (engine-only)

`re15_port/tests/unit/probe_stair_10a0.c` (CMake-Target `probe_stair_10a0`; lädt das
echte ROOM10A0.RDT, fährt main/sub-SCD hoch → 8 STAIR-AOTs installiert, setzt Band+
Position+Blick und ruft `re15_stair_try_start` + `re15_stair_tick` bis Finalize):

```
A DESCEND 8->6 : ORIGINAL DESCEND Ziel 6 | PORT DOWN → Band 6  ✓ (vordere Treppe „funktioniert")
B DESCEND 6->4 : ORIGINAL DESCEND Ziel 4 | PORT DOWN → Band 4  ✓
C DESCEND 4->3 : ORIGINAL DESCEND Ziel 3 | PORT **UP → Band 8** ✗  ← der Report
D DESCEND 3->1 : ORIGINAL DESCEND Ziel 1 | PORT **UP → Band 6** ✗  ← der Report
C ASCEND  3->4 : ORIGINAL ASCEND  Ziel 4 | PORT UP → Band 8 ✗ (Overshoot, 5 Bänder)
D ASCEND  1->3 : ORIGINAL ASCEND  Ziel 3 | PORT UP → Band 6 ✗ (Overshoot, 5 Bänder)
A ASCEND  6->8 / B ASCEND 4->6 : ✓ (Paar zufällig korrekt)
```

Antwort auf „warum kehrt die hintere Treppe die Richtung um": NICHT stale Band (das
Band-Gate matcht korrekt Slot 7 bzw. 9), NICHT der Endpunkt-Vergleich, NICHT die
chain-Verkettung — sondern die **event_id-Paar-Suche findet die falsche Partner-Zone**
(gleiches side-Flag einer ANDEREN Treppe mit höherem Band), und `target<cur` wird
dadurch zu `target>cur` → UP.

---

## (D) FIX-PLAN (byte-true, mit Belegen)

1. **Registrierung erweitern** (`scd_vm.c` op_aot_set, Typ 12/13; `re15_aot.h`/
   `aot_common.c re15_aot_set_stair`): pro Treppen-AOT zusätzlich speichern:
   - `side` = u16 LE `pc[14..15]` (= rec+0xC; lhu @0x80043530/0x80043558 bzw.
     @0x800435fc/0x80043624),
   - `count` = `pc[16]` (= rec+0xE; lbu @0x800435b8 sce12 / @0x8004367c sce13),
   - `axis` = sce (12 = X @0x80043510/`lw 0x34`, 13 = Z @0x800435dc/`lw 0x3c`),
   - Rect-ECKE + Extents roh (Ecke = centre − half, Extents sind positiv).
   `chain`→`band` bleibt wie gehabt (Band-Gate @0x80042cac-ccc).
2. **`re15_stair_try_start` umbauen**: Paar-Suche (Z. 341-351) ERSATZLOS streichen.
   Nach dem Zonen-Match (Band-Gate + Rect):
   ```c
   int32_t delta = (axis==13) ? p->z - rect_z : p->x - rect_x;   /* @0x8004351c/800435e8 */
   int low  = delta < (extent >> 1);                              /* slt @0x80043524/800435f0 */
   int up   = (low && side==0) || (!low && side==1);              /* @0x80043530-90 */
   int n    = count & 7;                                          /* @0x800389cc */
   int target = up ? cur + n : cur - n;                           /* Gait: n × ±0x708, A.6 */
   s_bands_left = n - 1;                                          /* DAT_800acaf2 @0x800389d0-dc */
   s_ydir  = up ? -1 : +1;  s_motion = up ? STAIR_UP : STAIR_DOWN;
   ```
3. **Facing**: statt atan2 auf die Paar-Zone → Snap auf die nächste Achsen-Kardinale
   (Endpunkt des byte-true acabe-Decays A.5): Typ 13 → nächstes aus {1024,3072}
   (±Z in Port-Konvention), Typ 12 → {0,2048} (±X). (Der sichtbare Clip-5-Settle bleibt
   wie bisher ausgelassen — Nutzer-Hardware-Messung 2026-07-24, s. Offen O2.)
4. **Optional (Kleinst-Divergenz)**: Probe-Distanz 800→620 (=0x26c @0x80042bd0) und den
   Spieler-Reach-Fallback (450, ohne PSX-Referent) hinter die Probe zurückstellen —
   getrennt verifizieren (Trigger-Reichweite an Türen/1170 nicht verschlechtern).
5. **Verifikation**: `probe_stair_10a0` → alle 8 Szenarien müssen auf dem
   Original-Zielband landen (C: 4→3/3→4, D: 3→1/1→3); danach ROOM1170-Regression
   (4 Zonen, beide Richtungen) + Durchlauf-Harness (`RE15_DEBUG_JUMP=10a0`), ideal
   PSX-Gegenprobe per re15-room-capture (s. Offen O1).

---

## Offene Punkte

- **O1:** PSX-Hardware-/DuckStation-Gegenprobe für ROOM10A0 selbst (Savestate: Band-4-
  Spieler an Treppe C, DESCEND beobachten) steht aus — die Original-Seite ist hier
  statisch (Disasm) + datenseitig (RDT-Bytes) belegt, nicht dynamisch am Gerät.
- **O2:** Mode-0xb Phase 0/1 setzt Clip 5 + Speed 0x4b (@0x8003888c-800388c0) = ein
  Ausricht-Settle VOR dem Gait — der Port hat „kein Turn" per Hardware-Messung
  (2026-07-24) verbaut. Nicht neu vermessen; für die Richtungsfrage irrelevant, für
  100%-Byte-Treue der Treppen-Präsentation noch offen (evtl. 0 sichtbare Frames, wenn
  acabe bereits kardinal ist).
- **O3:** Welche der beiden Treppen im gemeldeten Kamera-Cut „vorne"/„hinten" ist
  (C vs D), wurde nicht gegen die RVD-Zonen geprüft — beide sind identisch defekt,
  der Fix deckt beide.
