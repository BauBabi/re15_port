# ROOM1060 — Treppenhaus, Kamera schwenkt nach der letzten Treppe „teilweise" nicht um

Stand: 2026-08-06. Sechs parallele RE-Lanes + adversariale Gegenprüfung, danach von mir
selbst nachgerechnet (Disasm aus `info/Re1.5/PSX.EXE`, RDT-Bytes aus
`re15_port/shared_assets/PSX/STAGE1/ROOM1060.RDT`, Port-Quelltext, 19 neue Sonden).
**Kein Engine-/Plattform-Code wurde geändert** (`git diff --stat -- re15_port/engine
re15_port/include re15_port/platform` = leer).

---

## 0. Urteil in vier Sätzen

1. Die Kette bricht, weil der **Treppen-Gait des Ports 3761 Einheiten trägt**, die
   Ankunfts-Kamerazone aber erst 3820 (Ost) bzw. 3920 (West) Einheiten hinter dem
   frühestmöglichen Auslösepunkt beginnt. Der Gait endet dann **vor** der Zone, und der
   anschließende Ein-Tick-Versatz (Port-Eject bzw. byte-true Kollisions-Push) fliegt über
   den 1000 Einheiten tiefen Streifen hinweg. **BELEGT.**
2. Das ist ein **PORT-FEHLER**, kein Original-Quirk — mit drei disassembly-belegten
   Mechanismus-Divergenzen genau in dem Code, der die Strecke erzeugt. **WAHRSCHEINLICH**
   (die tatsächliche Original-Streckenlänge ist nicht gemessen, siehe §6).
3. Der minimale byte-true Fix sind **zwei Mechanismus-Korrekturen in
   `stair_common.c` (≈20 Zeilen), keine einzige neue Konstante**. Der Finalize-Eject wird
   dabei **nicht** angefasst — sein Entfernen behebt das Symptom **nachweislich nicht**
   (zwei unabhängige Trial-Patches).
4. Betroffen sind messbar **ROOM1060 und ROOM10A0**; der Mechanismus ist in 8 von 14
   Treppenräumen scharf.

---

## 1. Warum bricht die Cut-Kette — exakter Mechanismus

### 1.1 Die Soll-Kette und die zwei kritischen Rechtecke — BELEGT

`ROOM1060.RDT`, RVD-Basis = `u32LE @0x28` = **0x0160**, 22 Records à 20 Byte, Terminator
@0x0318, **alle floor-Bytes 0xFF** (Etagen-Gate inert). Selbst geparst:

| Record | Datei-Offset | from→to | Rechteck |
|---|---|---|---|
| RVD[1]  | 0x0174 | 0→1 | x[23000..27500] z[21500..22500] |
| RVD[4]  | 0x01b0 | 1→2 | x[19500..23000] z[22000..23000] |
| RVD[7]  | 0x01ec | 2→3 | x[23000..27500] z[23000..24000] |
| **RVD[10]** | **0x0228** | **3→4** | **x[23000..27500] z[20000..21000]** |
| **RVD[13]** | **0x0264** | **4→5** | **x[19500..23000] z[24000..25000]** |
| RVD[16] | 0x02a0 | 5→7 | x[23500..24500] z[24500..27500] (Tür-Cut) |

Soll-Kette: `0 →[1]→ 1 →[4]→ 2 →[7]→ 3 →[10]→ 4 →[13]→ 5 →[16]→ 7`.
**Fünf der sechs Umschaltungen passieren WÄHREND eines Treppenlaufs; 2→3 und 3→4 sogar im
selben Lauf (Treppe 3).**

Der Scan `FUN_80014230` ist eine **Zustandsmaschine mit reinem Punkt-in-Rechteck-Test**,
einmal pro Frame:
- `lw a0,-14444(a0)` @0x80014234 → `DAT_800ac794` (Gruppen-Anker)
- `lbu v0,-1099(v0)` @0x8001423c → `DAT_800afbb5` (angezeigter Cut)
- `lbu v1,22(a0)` @0x80014254 + `bne v1,v0` @0x8001425c → nur die Gruppe mit
  `cam_from == aktueller Cut`
- `addiu s0,a0,22` @0x80014268 → **der erste Record jeder Gruppe (der raumweite Anker
  x[17500..29000] z[16000..29000]) wird NIE getestet** → kein Rettungsnetz
- `lbu v1,-1(s0)` @0x8001426c / `beq v1,s2` (s2=0xff) @0x80014274 / `lbu v0,-13610(v0)`
  @0x80014280 → Floor-Gate gegen `player+0x82`; in ROOM1060 inert
- `addiu a0,a0,-13688` @0x80014294 (= `&playerX` 0x800aca88) + `jal 0x80014368`
  @0x80014298 → **Punkt**, keine Überstreichung
- `jal 0x800142f4` @0x800142ac + `j 0x800142d8` @0x800142b4 → höchstens EIN Wechsel pro
  Frame, erster Treffer gewinnt.

**Konsequenz (BELEGT):** wer einen Übergang verpasst, bekommt ihn nie zurück. RVD[13]
verlangt `cam_from = 4`; steht der Cut auf 3, ist die Zone dauerhaft dormant, und danach
ist auch RVD[16] (`cam_from = 5`) tot → die Kamera bleibt bis zum Raumwechsel falsch.

### 1.2 Die byte-true Auslösefenster — BELEGT

Treppen-`Aot_set` (opcode 0x2c, sce 13, flags 0x31), selbst geparst @0x0d72..0x0dfe:

| Slot | Offset | Band | Rechteck | corner | extent | side | count |
|---|---|---|---|---|---|---|---|
| 3,4,7,8 (OST) | 0x0d72/0d86/0dc2/0dd6 | 8/6/4/2 | x[23800..26500] z[19800..24200] | 19800 | 4400 | 0 | 2 |
| 5,6,9,10 (WEST) | 0x0d9a/0dae/0dea/0dfe | 6/4/2/0 | x[20200..22600] z[20700..25000] | 20700 | 4300 | 1 | 2 |

(Feldlage im Record: corner = z @+0x08, extent = d @+0x0c, side @+0x0e, count @+0x10 —
d.h. für Slot 3 liegt `count` auf Datei-Offset **0x0d82**, nicht 0x0d80.)

Richtungsregel `LAB_800435cc`: `lh a0,6(a2)` corner @0x800435dc, `lhu v0,10(a2)` extent
@0x800435e4, `srl v0,v0,1` @0x800435ec, `slt` @0x800435f0 → `low_half = (playerZ-corner) <
extent>>1`; mit `side` (@0x800435fc/@0x80043624) folgt:
**OST (side=0) steigt aus der HOHEN Hälfte ab → −Z, Kopf bei z≈24200.
WEST (side=1) steigt aus der NIEDRIGEN Hälfte ab → +Z, Kopf bei z≈20700.**
(Der Handler schreibt **keine** Blickrichtung — nur entity+5=0xb @0x8004360c/34/4c,
entity+6=1|2 @0x80043660, `DAT_800aca3c |= 0x40` @0x80043674, entity+0x9e=count|0x80
@0x80043688.)

Fenstergrenzen, beide Seiten byte-true herleitbar:
- **Untere Grenze = Vorwärtssonde 620** (`ori v0,zero,0x26c` @0x80042bd0, gedreht per
  `jal 0x8004f008` @0x80042bfc), getestet über `andi v0,v1,0x20` @0x80042ef8 (Bit 0x20 in
  flags 0x31 gesetzt; der Spieler-Positions-Test an Bit 0x40 @0x80042ea8 ist bei 0x31
  CLEAR).
- **Obere Grenze = Kollisionskante** der massiven SCA-Schachtzelle. Selbst geparst:
  Zelle @0x04dc `w=3800 d=4100 x=19600 z=20800` (Band 0) → West-Schacht z[20800..24900];
  Zelle @0x0578 `w=4200 d=4100 x=22900 z=20000` (floor 0x23 = Band 2) → Ost-Schacht
  z[20000..24100]. Push-Out = Radius (Caller-Parameter a2) + **18er-Skin**
  (`addiu t0,v0,18` @0x8003dab0, `addiu a0,a0,-18` @0x8003dab8).

| Treppe | Richtung | legales Fenster | Breite |
|---|---|---|---|
| Treppe 3 (OST, Band 4→2) | −Z | **z0 ∈ [24568 .. 24820]** | 253 |
| Treppe 4 (WEST, Band 2→0, die LETZTE) | +Z | **z0 ∈ [20080 .. 20332]** | 253 |

(24568 = 24100+450+18; 24820 = 24200+620; 20080 = 20700−620; 20332 = 20800−450−18.)

### 1.3 Der Defekt: Gait 3761 — BELEGT (fünf unabhängige Sonden, identischer Wert)

Der Port-Gait trägt **konstant 3761 Einheiten** pro 2-Band-Treppe, unabhängig vom
Einstiegspunkt — reproduziert in `probe_lanea_1060_margin`, `probe_lb_stairfsm`,
`probe_lane_d_traj1060`, `probe_lanee_margin_1060`, `probe_adv_stairband_1060`.
Davon stammen nur 580–590 aus dem Vorwärts-10 (`ori v0,zero,0xa` @0x80038cc0,
`sh v0,-13600(at)` @0x80038cc8, `jal 0x800245d8` @0x80038dc4, gegated `sltiu v0,v0,0x1f`
@0x80038db8); der Rest (~3180) kommt aus dem FK-Fußzug `FUN_800390e0` (`jal 0x800390e0`
@0x80038d50).

Daraus folgt arithmetisch (und deckt sich mit jeder Messung):

| | Zone | Bedingung | erforderlicher Gait | Fehlbetrag | Ausfallfenster |
|---|---|---|---|---|---|
| Treppe 3 (OST) | RVD[10] z≤21000 | `z0 − Gait ≤ 21000` | ≥ 3820 (bei z0=24820) | **59** | z0 ∈ [24762..24820] = **23,3 %** |
| Treppe 4 (WEST) | RVD[13] z≥24000 | `z0 + Gait ≥ 24000` | ≥ 3920 (bei z0=20080) | **159** | z0 ∈ [20080..20238] = **62,8 %** |

### 1.4 Warum der verpasste Übergang endgültig ist — BELEGT

Endet der Gait vor der Zone, steht der Spieler **in** der massiven Schachtzelle. Der
nächste Positionsschritt ist ein **Ein-Tick-Versatz von 1257…1527 Einheiten** — gemessen
sowohl mit dem Port-Eject (`stair_common.c:162`, 48 × 100u) **als auch** mit dem reinen
byte-true Resolver `re15_collision_constrain` (Portierung von `FUN_8003b0a4` →
`FUN_8003da78`). Beide landen exakt auf **z=25368** (= 24900+450+18) bzw. **z=19532**
(= 20000−450−18). Der Streifen ist 1000 tief → er wird komplett übersprungen.

**Kausalitäts-Messung (zwei unabhängige Trial-Patches, das ist der Beweis):**

`probe_advF_ejectcam_1060` — letzte Treppe, Start-Cut 4, 143 legale Einstiege
20080..21500, danach Lauf zur Tür, **mit der echten Engine-Funktion `re15_aot_scan`**:

| Modus | Beschreibung | Ausfälle |
|---|---|---|
| A | Port wie ausgeliefert | **16** (Fenster z=20080..20230), Tür-CUT = 4 |
| B | Kamera-Scan auf dem Eject-Pfad (100u-Schritte) | **0** |
| C | **Eject ersatzlos entfernt** (= genau `LAB_80038e50`) | **16**, dasselbe Fenster |

`probe_advz_cause_1060` (Lane E, unabhängig) T9 bestätigt Modus C: mit entferntem Eject
und normaler Frame-Reihenfolge (Move → `re15_collision_constrain` → `re15_aot_scan`) bleibt
END-CUT bei z0 = 20080/20120/20160/20200 **unverändert 4**.

Ende-zu-Ende-Repro der Symptomkette (`probe_lanee_chain_1060` E/F):
```
!! F536 UEBERSPRUNGEN: RVD[13] 4->5  Strecke (21388,23901)->(21388,25368) len=1467   END-CUT=4
!! F378 UEBERSPRUNGEN: RVD[10] 3->4  Strecke (25162,21029)->(25162,19532) len=1497   END-CUT=3
```

### 1.5 Warum es sich „teilweise" anfühlt — BELEGT

Das Auslösefenster ist 253 Einheiten breit, weil die 620er-Sonde 620 Einheiten vor dem
Spieler tastet. Praktisch heißt das:

> **Wer bis an die Wand läuft und dann □ drückt, bekommt den Schwenk.
> Wer drückt, sobald das Spiel es zulässt, verliert ihn** — West ab 94 Einheiten
> Wandabstand, Ost ab 194 Einheiten.

Zwei unabhängige Pfade zum selben Symptom:
- **P2 (direkt, 62,8 %)** — die letzte Treppe verfehlt RVD[13] selbst → Cut bleibt 4 →
  Tür-Zone RVD[16] (`cam_from=5`) dormant.
- **P1 (indirekt, 23,3 %)** — schon Treppe 3 verfehlt RVD[10] → man kommt mit Cut **3**
  am Kopf der letzten Treppe an → RVD[13] (`cam_from=4`) ist dormant → die letzte Treppe
  kann **gar nicht** schwenken, egal wie perfekt man sie fährt.

Das erklärt exakt die Vormessung „isoliert auf Cut 4 gestartet schaltet die letzte Treppe
bei JEDEM Start-Z korrekt" — die isolierte Messung setzt P1 außer Kraft, und ihr
Start-Z-Sweep (20800..22800) lag zudem vollständig **innerhalb der Wand** und damit außerhalb
des legalen Fensters (siehe §7.2).

---

## 2. Port-Fehler oder Original-Quirk?

### 2.1 Klare Antwort: **PORT-FEHLER.** Belegte Mechanismus-Divergenzen im Gait

**BELEGT — Divergenz 1: Pose-Abtastung um EINEN Keyframe verschoben.**
`FUN_8001f314` liest den Cursor **unverändert**:
```
8001f358: srl  v1,v0,16
8001f35c: lbu  v0,149(t0)        ; t0 = player base, +0x95 = DAT_800acae9  → Cursor VOR ++
8001f364: sll  v0,v0,2
8001f368: addu a2,v1,v0
8001f36c: sw   a2,360(t0)        ; Keyframe-Zeiger
```
Das Inkrement steht erst am **Ende** von `FUN_8001f3bc`
(`RE_15_Quellcode_V2/FUN_8001f3bc.c` Z.89-94). Der Port inkrementiert **vorher**:
`stair_common.c:203-205` `s_cursor++; int frame = s_cursor % STAIR_CLIPLEN;
p->anim_frame = (uint16_t)frame;` — und benutzt genau dieses `frame` für die
FK-Abfrage (:230). Gemessene Kosten: **445 Einheiten** pro Treppe
(`probe_lb_stairfsm` Modus 1: 3944 statt 4389). Frame-Trace-Deckung exakt: Port f1..f8 ==
Original f2..f9.

**BELEGT — Divergenz 2: gemeinsame statt per-Knochen-Fußreferenz + Skip beim Fußwechsel.**
`FUN_800390e0` adressiert die Vorframe-Fußposition **mit dem Fuß-Index**:
```
80039148: sll   v0,s1,7          ; s1 = footsel
8003914c: addu  v0,v0,s1         ; *129
80039150: sll   v0,v0,2          ; *516
80039154: addiu v0,v0,688
80039158: addu  s3,s3,v0         ; s3 = *(entity+0x188) + 688 + 516*footsel
...
800391a0: lw    a0,84(s3)        ; footPrev.X  = s3+0x54   (pro Knochen!)
800391b0: sw    v0,52(a1)        ; player+0x34 -= Delta
```
und der Aufruf ist **unbedingt** — beide Zweige der sel-Auswahl laufen auf denselben
`jal` zusammen:
```
80038d34: addiu v0,v0,-11
80038d38: sltiu v0,v0,0xf
80038d3c: beq   v0,zero,0x80038d4c
80038d40: addu  a0,zero,zero      ; a0 = 0 (Drag-Zweig) IMMER
80038d44: j     0x80038d50
80038d48: ori   a1,zero,0x1       ; a1 = sel
80038d4c: addu  a1,zero,zero
80038d50: jal   0x800390e0        ; unbedingt
```
Der Port hält **eine** gemeinsame Referenz (`stair_common.c:75` `static int32_t
s_footref[3]`, `:77 s_prev_sel`) und **unterdrückt den Zug beim Fußwechsel**
(`:240 if (s_footref_ok && sel == s_prev_sel)`) sowie **nach jedem Band-Loop**
(`:264 s_footref_ok = 0;`). Kosten: **256 Einheiten** (`probe_lb_stairfsm` Modus 2: 4133
statt 4389) plus die nicht-monotonen Rückwärtsausschläge (§7.3).
Der Schreiber der Referenz ist inzwischen lokalisiert: `FUN_8001e9ec` schreibt
bone+0x40 (`addiu s1,s2,64` @0x8001ea18 + `jal 0x80022da0` @0x8001ea24), Per-Knochen-Loop
mit Stride 172 @0x8001e998 — also **jeden Frame für jeden Knochen frisch**.

**BELEGT — Divergenz 3: fehlender 7-Frame-Crossfade.**
Descend-Init setzt `ori v1,zero,0x7` @0x80038cd4 + `sb v1,-13597(at)` @0x80038cdc →
`DAT_800acae3` = player+0x8f = 7; der Finalize setzt ihn auf 0 (@0x80038e78).
`FUN_8001f3bc` mischt bei ≠0 per GTE zwischen Vorpose und neuem Keyframe (Z.23, 40-61, 78)
— und zwar auf **genau den Bone-Matrizen, die der FK kettet**. Der Port setzt
`anim_frac` nie (`grep anim_frac stair_common.c` = 0 Treffer) und schaltet den Blend für
die FK-Abfrage sogar explizit ab (`:233 g_anim_pose_actor = NULL;`).

**Summe der belegten Divergenzen: Port 3761 → Original-Modell 4389** (628 Einheiten,
`probe_lb_stairfsm`, gegen den Port bit-genau validiert).

### 2.2 Warum kein Quirk — WAHRSCHEINLICH (zwei port-unabhängige Argumente)

1. **Der Gait überspannt nicht einmal sein eigenes Treppen-Rechteck.** Das West-Record
   trägt `extent = 4300` (Datei @0x0da6), das Ost-Record `4400` (@0x0d7e) — reine
   RDT-Bytes, keine Port-Annahme. Ein Abstieg, der 3761 trägt, verlässt das Rechteck
   nicht.
2. **Drei disassembly-belegte Fehler sitzen in genau den 40 Zeilen, die die Strecke
   erzeugen.** Es ist keine Hypothese nötig, um eine Unterdeckung zu erklären — sie ist
   direkt aus dem Disasm ableitbar.

### 2.3 Was ich AKTIV widerlegt habe (drei Sackgassen, damit niemand sie erneut geht)

**(a) „Das Original kann keinen 1000er-Streifen überspringen" — FALSCH.**
Drei Lanes stützten sich darauf; eine vierte las in `FUN_8003da78` einen
Magnituden-Clamp. Ich habe die Stelle selbst disassembliert:
```
8003db74: bgez  t0,0x8003db80
8003db7c: subu  v0,zero,v0
8003db80: addiu v0,v0,400
8003db88: sltiu v0,v0,0x321       ; |t0| < 401 ?
8003db8c: beq   v0,zero,0x8003dbc8 ; NEIN -> Fallback (NICHT: kein Push)
8003db94: sw    v0,52(a1)          ; JA  -> X-Push auf a1
...
8003dbc8..8003dc28:  |t0| vs |a3| vergleichen, die KLEINERE Durchdringung
                     UNBEGRENZT auf *(0x800b8294) anwenden
```
`DAT_800b8294` ist **dieselbe Entity**: `sw a1,-32108(at)` @0x8003da94 (Ghidra-XREF: genau
ein Writer). Der 400er-Test ist ein **Achsen-Selektor**, kein Clamp — beide Zweige
schieben, der Fallback unbegrenzt. **Das Original kann sehr wohl in einem Frame um
Zelltiefe+2·Skin versetzen.** Damit ist der große Ein-Tick-Sprung *als solcher* nicht
un-original, und die Argumentationslinie „Eject = Port-Erfindung = Ursache" bricht.

**(b) „Der Finalize-Eject ist die Ursache" — WIDERLEGT.**
Zwei unabhängige Trial-Patches (§1.4, Modus C und Lane-E T9) zeigen: mit entferntem Eject
bleibt das Ausfallfenster **identisch**. Der Gait endet dann bei z=23841..23991, also immer
noch **unterhalb** der Zonen-Unterkante z=24000. Der Eject macht den Fehler nur permanent.
`LAB_80038e50` @0x80038e50..0x80038eec hat tatsächlich **keinen** Store auf player+0x34/
+0x3c (vollständige Store-Liste: 0x800acae8=2 @0x80038e68, 0x800acae9=0 @0x80038e70,
0x800acae3=0 @0x80038e78, `jal 0x8001f314` @0x80038e7c, 0x800aca59=0 @0x80038ea8,
0x800aca5a=0 @0x80038eb0, `sb zero,0(s0)`→0x800aca5b @0x80038eb4, +0x93&=0xfe @0x80038ec0,
player+0x82 = −`DAT_800acc0e`/1800 @0x80038edc) — das ist eine **echte Divergenz**, aber
**nicht die Ursache dieses Bugs**.

**(c) „Die Zonen-Auswahl / der Scan / das Etagen-Gate sind schuld" — AUSGESCHLOSSEN.**
- `probe_rvd_diff_10a0`: 532610 Gitterpunkte × 10 Cuts, **0 Abweichungen**.
- `FUN_80014230` hat **game-weit genau einen Aufrufer** (@0x8001ccec) — eigener
  binärweiter jal-Scan über PSX.EXE + alle 8 BINs, zusätzlich Zeiger-Scan (0 Treffer).
  Gates: Debug-Pad-Toggle @0x8001ccb4-c0 (init 0 @0x8001c960) und `DAT_800aca3c & 0x100`
  @0x8001ccd4-e4. `DAT_800aca58` und `DAT_800aca40` gaten ihn **nicht**.
- ROOM1060: alle 22 Floor-Bytes 0xFF → Etagen-Gate inert.
- Der Port scannt in jedem Treppen-Frame (`game_step_common.c:456`); ein Vergleichslauf
  mit umgedrehter Reihenfolge (SCAN-DANN-TICK = Original) liefert dieselbe Kette.

---

## 3. Der minimale, byte-true Fix

**Ziel-Kriterium (geometrisch, nicht geraten):** der Gait muss ≥ **3920** Einheiten tragen
(West-Fenster-Untergrenze), dann ist das **gesamte** legale Auslösefenster beider Treppen
abgedeckt. Jede einzelne der beiden folgenden Korrekturen liefert bereits ≥ 256 Einheiten;
beide zusammen ergeben 4389 (gemessen) — Marge 469 (West) / 569 (Ost).

### Fix A — `re15_port/engine/src/stair_common.c:203-205`

```c
/* IST */                              /* SOLL */
s_cursor++;                            int frame = s_cursor % STAIR_CLIPLEN;
int frame = s_cursor % STAIR_CLIPLEN;  p->anim_frame = (uint16_t)frame;
p->anim_frame = (uint16_t)frame;       s_cursor++;      /* nach der Abtastung */
```
**Beleg:** `lbu v0,149(t0)` @0x8001f35c liest `player+0x95` unverändert; das Inkrement
steht am Ende von `FUN_8001f3bc` (Z.89-94, Wrap bei `frame_count`).
**Wichtig:** die nachfolgenden Tests (sel-Fenster @0x80038d34/38, Bob @0x80038d64/8c,
Vorwärts-Gate @0x80038db8, Band-Schritt @0x80038de4) sehen im Original den **erhöhten**
Cursor — das tut der Port bereits, hier ändert sich nichts.

### Fix B — `re15_port/engine/src/stair_common.c:75-77, 240, 264`

```c
/* :75-77 */  static int32_t s_footref[8][3];   /* pro Knochen (praktisch 4 und 7) */
              static uint8_t s_footref_have[8];
/* :240   */  if (s_footref_have[foot]) { p->x -= ...; p->z -= ...; p->y -= ...; }
/* :264   */  /* s_footref_ok = 0;  ENTFÄLLT — kein Reseed über den Band-Loop */
```
und in **jedem** Tick beide Fußknochen fortschreiben (nicht nur den aktiven).
**Beleg:** `s3 = *(entity+0x188) + 688 + 516*footsel` @0x80039148-58, Referenz-Load
`lw a0,84(s3)` @0x800391a0 (X) / `lw a0,92(s3)` @0x800391c0 (Z) / `lw a0,88(s3)`
@0x800391e0 (Y) — **fuß-indiziert**; Aufruf `jal 0x800390e0` @0x80038d50 **unbedingt**
(a0 = 0 im Delay-Slot @0x80038d40, a1 = sel); der Band-Loop @0x80038e3c schreibt
ausschließlich `cursor = 2`. Der Referenz-Schreiber ist `FUN_8001e9ec` (bone+0x40,
`addiu s1,s2,64` @0x8001ea18 + `jal 0x80022da0` @0x8001ea24, Per-Knochen-Loop Stride 172
@0x8001e998) — jeden Frame frisch, also nie „stale".

**Beide Fixes sind reine Mechanismus-Korrekturen. Keine neue Konstante, kein neuer
Zahlenwert, keine Skalierung.**

### Was der Fix NICHT anfasst

| bewusst unangetastet | Grund |
|---|---|
| `stair_common.c:141-186` — der Finalize-Eject (48 × 100u) | Entfernen behebt das Symptom **nachweislich nicht** (§1.4 Modus C / T9) und bringt „Leon hängt in der Wand" zurück, solange der Gait die Zelle nicht räumt. Bleibt als **klar kommentierte Port-Krücke** stehen, bis §6.1 gemessen ist. |
| `stair_common.c:233` `g_anim_pose_actor = NULL` + fehlendes `anim_frac = 7` (@0x80038cdc) | Fasst die gemeinsame Crossfade-Maschinerie an → Risiko für **alle** Actor-Renderpfade. Eigener Commit nach eigener Messung. |
| Band-Pin auf 7 während der Traversierung (@0x80038ce4 / @0x80038a28) | Echte Divergenz, aber in ROOM1060 **inert** (alle Floor-Bytes 0xFF). Deaktiviert schlagartig sämtliche Band-Gates (AOT, Kollision, Treppen-Wiederauslösung). **Eigener Commit.** Und: der Pin liegt in Phase 0 des Gait, **nicht** im sce-Handler — `re15_stair_try_start` wäre einen Frame zu früh. |
| `aot_common.c` RVD-Auswahl / `rdt_common.c` Anker-Regel | Byte-identisch verifiziert (532610 Punkte, 0 Abweichungen). |
| `game_step_common.c:454-456` Scan-Reihenfolge | Original scannt @0x8001ccec vor der Spieler-FSM @0x8001ce0c; gemessen ohne Wirkung auf die Kette (reine 1-Frame-Phase). |
| `STAIR_REACH 450` (`:41-45`), `STAIR_MAXFRAMES 600` (`:63`) | Guess-Tells (§7.4), aber nicht ursächlich. |
| `aot_common.c:700` `(cam_pb < 0)` | Unbelegter Ausweg im Floor-Gate; in ROOM1060 wirkungslos. |
| SCD / RDT-Daten | ROOM1060 enthält **null** Kamera-Opcodes (0x29/0x2A/0x37/0x3C/0x4B — opcode-exakter Walk über main+sub+extra). Es darf **keine** Zone und **kein** `Cut_chg` „nachgerüstet" werden. |

### Reihenfolge

1. **Zuerst die kaputten Sonden reparieren** (§7.2) — sonst lügt das Gate.
2. **Gate landen** (§5), rot.
3. Fix A + Fix B als **ein** Commit; Akzeptanz = Gate grün + Gait ≥ 3920.
4. Danach separat: Crossfade, Band-Pin, Eject (erst nach §6.1).

---

## 4. Zensus — welche Räume sind betroffen

**14 Räume mit Treppen-AOTs (sce 12/13), game-weiter RDT-Scan:**

| Stage | Räume (Anzahl Treppen-AOTs) |
|---|---|
| 1 | ROOM1060 (8), ROOM10A0 (8), ROOM1170 (4) |
| 2 | ROOM2030 (1), ROOM2040 (2), ROOM2080 (1) |
| 3 | ROOM3010 (1), ROOM3040 (4), ROOM30C0 (2), ROOM30D0 (2) |
| 4 | ROOM4070 (12 = 6 Läufe) |
| 5 | ROOM5020 (2) |
| 6 | ROOM6010 (8), ROOM6030 (1) |

**Gemessene Zonen-Überspringer** (`probe_lanee_eject_census`, Abstiege über das ganze
Auslöserechteck):

| Raum | Abstiege | größter Ein-Tick-Versatz | Überspringer |
|---|---|---|---|
| **ROOM1060** | 758 | 2907 | **10** (RVD[10] 3→4, RVD[13] 4→5) |
| **ROOM10A0** | 674 | 2907 | **4** (RVD[10] 3→4) |
| ROOM6010 | 435 | **7098** | 0 |
| ROOM4070 | 1010 | 2807 | 0 |
| ROOM1170 | 133 | 1737 | 0 |
| ROOM30C0 / ROOM30D0 | 38 / 38 | 1369 | 0 |
| ROOM2040 | 212 | 1307 | 0 |
| ROOM3040 | 123 | 1151 | 0 |
| ROOM5020 | 34 | 294 | 0 |
| ROOM2030 / 2080 / 3010 / 6030 | — | 0 | 0 |

**Lesart:** die 0 bei ROOM6010/4070/1170 heißt nur, dass an den abgetasteten Punkten
zufällig keine Zone im Sprungkorridor lag — **der Mechanismus ist in 8 von 14 Räumen
scharf** (Versatz > übliche Streifentiefe). Kleinste getestete RVD-Zone game-weit:
**400 Einheiten** (ROOM30D0 @0x3f0, 2500×400, 7→2); STAGE1-Minimum 409 (ROOM1180 @0x238).
Das ist die Obergrenze für jede Schrittweite, die je in einer Scan-Schleife verwendet wird.

**Nicht vermessen:** der **Aufstieg** (`LAB_80038850`, Cursor-Fenster +1, Bob 12/28,
Band-Schritt 33, Loop → 3). Er benutzt denselben Finalize und dieselbe FK-Routine — die
Fixes A/B wirken dort identisch, die Fehlerfenster sind aber unbekannt.

**Etagen-Gate (Nebenzensus, für den Fix irrelevant, aber dokumentiert):** von 5576
RVD-Records in 240 RDTs tragen **144 getestete** ein Floor ≠ 0xFF: ROOM11B0/11C0 (7),
ROOM11E0 (0), ROOM2000/20B0 (0 **und** 1 als echte Etagen-Paare). Der Port-Kommentar
`aot_common.c:694-698` („0xFF across all STAGE1 rooms") ist damit **sachlich falsch**.

---

## 5. Regressions-Gate

Neuer ctest `unit_stair_cam_1060_chain` (Sonde `probe_stair_cam_1060_chain.c`,
zusammengeführt aus `probe_advc_chain1060` + `probe_laneF_gap_1060` +
`probe_lanee_margin_1060`). Vier Assertions, **keine** hartkodierten Framenummern:

**G1 — Auslösefenster aus den Daten ableiten, nie setzen.**
untere Grenze = Rechteckkante ∓ 620 (`ori v0,zero,0x26c` @0x80042bd0), obere Grenze =
gemessene Kollisionskante über `re15_collision_constrain`. Blickrichtung aus
`side/corner/extent` des Records (Regel @0x800435ec-f0), **nie** hartkodiert.
→ ROOM1060 muss [24568..24820] und [20080..20332] liefern.

**G2 — Volle Kette über das GANZE Fenster.**
Für jeden Einstieg in beiden Fenstern (Schrittweite ≤ 10), kollisions-geführter Lauf
zwischen den Treppen, **in allen drei Routenvarianten (diagonal / X-zuerst / Z-zuerst)**:
```
KETTE == 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 7      (END-CUT 7 an der Tür nach ROOM1040)
```
Die Routenvarianten sind Pflicht: `probe_advc_chain1060` hat gezeigt, dass der
End-Cut vom Laufweg abhängt (Z-zuerst kann RVD[12] 4→3 mitnehmen).

**G3 — „diskret == dicht" (Anti-Skip-Invariante).**
Jedes Frame-Segment (inkl. des Finalize-Segments) wird zusätzlich in ≤ 100u-Schritten
abgetastet und gegen alle Zonen mit passendem `cam_from` geprüft.
```
uebersprungene Zonen == 0
```
Das ist die eigentliche Schutz-Invariante — sie hält auch, wenn sich Cut-Nummern durch
eine legitime Bahnänderung verschieben.

**G4 — Gait-Untergrenze, geometrisch formuliert.**
```
gait_travel >= (window_max - zone_far_edge)   je Treppe
```
ROOM1060: ≥ 3920 (West), ≥ 3820 (Ost). **Keine feste Zahl im Test** — aus RDT + Zonen
berechnet.

**Bestehende Wächter, die im selben Lauf grün bleiben müssen:**
`probe_rvd_diff_10a0` (0 Abweichungen), `unit_rvd_scan_order`, `unit_cam_1030_reentry`,
`probe_cut_1030`, `probe_cut_replace_1030`, `probe_camload_1030`, `probe_room1040_switch`,
`probe_door_1170`, `unit_stair_1170_regression`, `probe_stair_10a0`,
`probe_stair_eject_10a0`, `probe_stair_cam_10a0`, `test_collision_layers`,
`test_slope_collision`, `test_anim_pose_order`, `test_getup_advance` — plus der volle
ctest-Lauf (Baseline 114/114 grün).

**Zusätzlich als Dauer-Wächter:** `probe_lanee_eject_census` über alle 14 Treppenräume —
Abnahmekriterium nach dem Fix: **0 Überspringer**, und der größte Ein-Tick-Versatz muss
unter der kleinsten RVD-Streifentiefe des jeweiligen Raums liegen.

---

## 6. Was offen ist und Hardware braucht (DuckStation-Trace)

### 6.1 KRITISCH — die tatsächliche Gait-Strecke des Originals
`player+0x34` (0x800ACA88) und `player+0x3c` (0x800ACA90) **pro Frame** während eines
echten ROOM1060-Abstiegs, beide Schächte, count=2.
Das entscheidet drei Dinge auf einmal:
- ob die belegten Fixes A/B **ausreichen** (4389) oder ob ~640 Einheiten fehlen
  (geometrisch nötig für eine wandfreie Landung: 5018 Ost / 5028 West — Schachtzelle
  4100 + 2 × (Radius 450 + Skin 18));
- ob das Original überhaupt **wandfrei landet** oder sich selbst per `FUN_8003b0a4`
  herausdrücken lässt (dann darf der Eject als Krücke bleiben, sonst muss er weg);
- ob das Original bei früher Auslösung **denselben** Teilausfall zeigt → dann wäre es
  doch ein Quirk. **Bis dahin ist §2 „PORT-FEHLER" ein WAHRSCHEINLICH, kein BELEGT.**
Skills: `re15-room-capture` + `re15-savestate-ghidra`.

### 6.2 Ist die Original-Bahn monoton?
Der Port geht auf 8 von 63 Ticks **rückwärts** (max 42–65 Einheiten) und erzeugt an der
Naht RVD[7] z[23000..24000] / RVD[9] z[24000..25000] ein sichtbares Ping-Pong
(`F405 2→3 @23983`, `F408 3→2 @24027`, `F410 2→3 @23934`). Ob das Original das auch tut,
hängt allein am Inhalt von bone+0x54 — es wählt an Cursor 9/10 **denselben** Knochen
(a1 = sel, a0 = 0 @0x80038d40) und zieht unbedingt, kann also prinzipiell ebenfalls
rückwärts gehen. **Kein Rückwärts-Clamp einbauen, bevor das gemessen ist.** Ein Clamp im
Scan ist ohnehin ausgeschlossen — `FUN_80014230` hat nachweislich keinen Kantenzustand.

### 6.3 Der allererste Gait-Tick
Im Original enthält bone+0x54 dort die Weltposition aus dem letzten
Nicht-Treppen-Render-Frame (Standby-Pose, alter Yaw). Es wird also ein einmaliges Delta
angewandt, das weder Port noch Modell kennen. Größe unbekannt.

### 6.4 Der Ausricht-Vorlauf (Mode 0xb, Clip 5)
`LAB_800435cc` setzt **immer** Mode 0xb (@0x80043540/68/84); Phase 0 (@0x8003888c-c0) setzt
Clip 5 und Speed 0x4b, Phase 1 (@0x800388c4-0x800389ac) lässt den Yaw
`DAT_800acabe` (= player+0x6a) geometrisch auf die Achsen-Kardinale konvergieren
(Exit-Gate `andi v0,v1,0x3e0` @0x800389a0, Snap `andi 0xff00` @0x800389ac). Der Port
snappt in **0** Frames (`stair_common.c:396-406`). Bei kardinalem Einstiegs-Yaw ist das
genau **ein unsichtbarer Frame** — die Hardware-Messung „kein sichtbarer Turn" und der
Disasm widersprechen sich also **nicht**. Ein echter Widerspruch entstünde erst bei
nicht-kardinalem Yaw; die Nachmessung muss genau den fahren
(zwei frühere Versuche wurden zurückgenommen: `536c3c48`, `84ea3678`).

### 6.5 Live-Verifikation der Gesamtkette
Alles hier ist statischer Disasm + port-seitige Sondenmessung. Ein Durchlauf ab
Title → NEW GAME → Raumkette mit `RE15_INPUT_SCRIPT` (Memory `reai-v2-playthrough-not-jumpin`)
plus gdigrab-Sichtprüfung (Skill `re15-port-visual-verify`) steht aus und ist Pflicht,
bevor „gefixt" gemeldet wird.

---

## 7. Anhang

### 7.1 Zwei Sonden-Artefakte, die die Vormessung verfälscht haben — BELEGT

**`probe_stair_cam_chain.c:201-206`** fährt Treppe 1 mit `rot = 3072` (+Z) und Treppe 3 mit
`rot = 1024` (−Z) — **dasselbe Ost-Record, derselbe Triggerpunkt (25150,23500)**. Mit 3072
läuft der Spieler die Treppe rückwärts hinaus bis z=29418, **außerhalb** des raumweiten
RVD-Quads z[16000..29000]. Zusätzlich läuft `march_to()` (:73-91) **ohne**
`re15_collision_constrain` und löst dabei RVD[12] 4→3 aus.
→ Die gemeldete Beobachtung „Treppe 1 und 2: KEIN Cut-Wechsel" ist **kein Port-Verhalten**.
Fix: Blickrichtung aus `side/corner/extent` ableiten, Lauf über den Resolver führen.

**`probe_stair_cam_1060.c:203-206`** sweept für die letzte Treppe z = 20800..22800 —
dieser Bereich liegt **vollständig in der massiven Schachtzelle** @0x04dc z[20800..24900]
(an allen 11 Stichpunkten blockiert, auf Start- **und** Zielband). Der erreichbare Bereich
ist [20080..20332] und wurde **nie berührt**. Daher die falsche Aussage „die letzte Treppe
schaltet bei JEDEM Start-Z korrekt".

### 7.2 Was byte-true verifiziert und unangetastet ist
- Clip 21/20 (`ori v0,zero,0x15` @0x80038cb4), Speed 10 (@0x80038cc0/c8), sel-Fenster
  [11,26) (@0x80038d34/38), Bob +150 bei 11/27 (@0x80038d64/8c), Vorwärts-Gate < 31
  (@0x80038db8), Band-Schritt bei 32 mit +1800 (@0x80038de4/@0x80038e00), Loop → 2
  (@0x80038e3c) — alles im Port korrekt.
- Ascend-Spiegel (Fenster [12,27), Bob 12/28, Band 33, Loop 3) — korrekt.
- Richtungsregel `LAB_800435cc` (side × Rect-Hälfte) — korrekt.
- RVD-Gruppen-Semantik (Anker-Skip, zusammenhängende `cam_from`-Gruppe, ein Schwenk pro
  Frame, sofortige Neu-Verankerung `FUN_800142f4` @0x800142f4-18 / `FUN_80014324`) —
  korrekt; game-weit 0 von 206 RDTs mit nicht-zusammenhängender Gruppierung.
- Anker-Initialisierung beim Raumladen: `lh a0,4068(a0)` @0x8003993c + `jal 0x800142f4`
  @0x80039944 → nie stale. Raum-Init löscht Bit 0x100 explizit (`and v0,v0,0x80000000`
  @0x8001d53c-44) → Scan beim Eintritt AN.

### 7.3 Guess-Tells im Treppen-Code (Verdachtsstellen, gemeldet)
| Ort | Text | Bewertung |
|---|---|---|
| `stair_common.c:42` | `STAIR_REACH 450` — „port fallback, no PSX referent" | vorbildlich gekennzeichnet; durch das flags-Gate (`andi v0,v1,0x40` @0x80042ea8) für 0x31-Records inert |
| `:57` | `STAIR_CLIPLEN 42` — „PL00 clip 21/20 frame count", ohne Offset | datenseitig belegt (beide Clips frame_count 42), aber ohne Zitat im Code |
| `:63` | `STAIR_MAXFRAMES 600 /* safety */` | kein Original-Äquivalent; greift zusätzlich in den Band-Commit-Zweig (:288/:293) = stiller Sonderpfad |
| `:142` | „FORWARD eject (**byte-true OUTCOME** of LAB_80038e50)" | irreführend. Das *Ergebnis* deckt sich auf den relevanten Pfaden mit `FUN_8003b0a4`, der *Mechanismus* nicht (905/2709 Abweichungen in ROOM1060). Muss als **Port-Krücke** umformuliert werden. |
| `:162/174` | `48` Schritte à `100` | ohne @0x-Beleg |
| `:177` | „Bulletproof fallback (geometrically unreachable …)" | aus **einem** Raum verallgemeinert; Zensus widerlegt (ROOM6010 7098) |
| `re15_stair.h` Kopf | „Faithful-available port", „Interim animation = the normal WALK clip" | komplett veraltet, beschreibt eine verworfene Mechanik |
| `aot_common.c:694-698` | „0xFF across all STAGE1 rooms" | sachlich falsch (ROOM11B0/11C0/11E0) |
| `aot_common.c:682-683` | „Zero-init → scan DISABLED at room entry" | Polarität falsch; widerlegt durch @0x8001d53c-44 |
| `game_step_common.c:455` | `g_scd.cut_auto_enabled = 1;` im Treppen-Zweig | ohne @0x-Beleg; gemessen ist `cut_auto` in ROOM1060 nach Raum-Init ohnehin 1, die Zeile kann ersatzlos weg (NICHT die Zeile :781 nach `scd_room_reenter` — die ist durch @0x8001d53c-44 gedeckt) |

### 7.4 Sonden-Inventar (19 neu, kein ctest-Eintrag, kein Engine-Code)
`probe_lanea_1060_zonepath`, `probe_lanea_1060_margin`, `probe_lb_stairfsm`,
`probe_adv_ejectcam_1060`, `probe_lanec_camscan_1060`, `probe_advc_chain1060`,
`probe_lane_d_traj1060`, `probe_adv_stairband_1060`, `probe_lanee_chain_1060`,
`probe_lanee_reach_1060`, `probe_lanee_margin_1060`, `probe_lanee_eject_census`,
`probe_advz_cause_1060`, `probe_laneF_floor_1060`, `probe_laneF_gap_1060`,
`probe_laneF_floorgate_rooms`, `probe_advF_ejectcam_1060`, `probe_refA_gaitcause_1060`.
Vor dem Fix zu einem ctest zusammenführen (§5), Rest als Diagnosewerkzeug behalten.
