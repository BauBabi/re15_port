# UNTERSUCHUNG B — Welches Karten-Rechteck ist die untere Etage von ROOM1170?

Datum 2026-08-31. Alle Zahlen sind gemessen (EXE-Byte-Offsets, PIX-Pixel, RDT-Bytes) oder
disassembliert. Keine geratenen Werte. Bilder: `analysis/karte_2026-08-31/img_B/`.
Mess-Skripte im Scratchpad: `mapB/render.py, blobs.py, match.py, match2.py, cmp.py,
composite.py, screen.py, proj1170.py, shape.py, bands.py, titles.py, final.py`.

Adresse -> Datei-Offset der PSX.EXE: `off(a) = a - 0x80010000 + 0x800`
(Header @0x18 = 0x80010000, geprueft mit `xxd -l 128 info/Re1.5/PSX.EXE`).

---

## 0. KERNERGEBNIS IN EINEM SATZ

Es gibt **kein** eigenes Rechteck fuer "die untere Etage" von ROOM1170 — weil das Original
den **gesamten zweiten Bereich** (oberer Korridor Band 4 -> Absatz Band 2 -> unterste Ebene
Band 0) als **EIN** Rechteck zeichnet: Seite 5 Rect **r0** (48x24, uv(192,16)) — genau das
Rechteck, das der Port bereits benutzt. Beim Treppe-Runterlaufen darf die Karte deshalb
**nicht** umschalten; der Marker wandert innerhalb von r0 vom unteren Arm in den linken Block
— und das tut der Port bereits korrekt.

---

## 1. WELCHE PIX GEHOERT ZU WELCHER SEITE (Tabelle gelesen, nicht geraten)

CD-Datei-ID-Tabelle @0x80074c4c (u16 je Seite), Datei-Offset **0x6544c**:

| Seite | id | Datei | Titelkachel (uv(0,0) 88x32, gerendert) |
|---|---|---|---|
| 0 | 12 | MAP01.PIX | POLICE STATION **B1** |
| 1 | 13 | MAP02.PIX | POLICE STATION **B2** |
| 2 | 14 | MAP03.PIX | POLICE STATION **1F** |
| 3 | 15 | MAP04.PIX | POLICE STATION **2F** |
| 4 | 16 | **MAP05.PIX** | POLICE STATION **3F** |
| 5 | 17 | **MAP06.PIX** | POLICE STATION *(KEINE Etagenangabe)* |
| 6 | 18 | MAP07.PIX | DRAINS B2 |
| 7 | 19 | MAP08.PIX | FACTORY |
| 8..11 | 20..23 | MAP09..MAP0C | LABORATORY B1 / B2 / B3 / B4 |
| 12 | 24 | MAP0D.PIX | SUBWAY |
| 13 | 0 | — | Tabellen-Ueberlauf (bekannt) |

Bild: `img_B/all_titles.png`.
=> Die Etage steckt im **Seiten-Titelbild**, nicht in einer Laufzeit-Abfrage. Seite 5
(= nur ROOM1170) traegt **gar keine** Etagenangabe.

## 2. RECHTECK-TABELLEN (gelesen)

Paar-Tabelle @0x80076840 (`{u16 count, u16 pad, u32 ptr}`, Datei-Offset **0x67040**),
Eintrag 12 Byte `{s16 x, s16 y, s16 w, s16 h, u8 u, pad, u8 v, pad}`.

* Seite 4 -> count 7, ptr 0x80076468 (Datei-Offset 0x66c68)
  * r0 (127,137) 16x16 uv(168,40) | r1 (136,137) 24x24 uv(168,16) | r2 (120,80) 32x40 uv(96,48)
  * r3 (152,89) 48x24 uv(192,16) | r4 (144,80) 32x80 uv(0,32) | r5 (120,119) 40x40 uv(128,16)
  * r6 (160,110) 56x56 uv(32,32)
* Seite 5 -> count 2, ptr 0x800764bc (Datei-Offset 0x66cbc)
  * r0 (140,80) 48x24 uv(192,16) | r1 (148,101) 48x56 uv(0,32)

CLUT: TEX.TIM CLUT-Block @VRAM(256,480), 32x24 Eintraege; clut-ID 0x7d50 -> VRAM(256,501)
-> **Zeile 21, Spalte 0**, 16 Eintraege:
`0=0x0000 (Index 0 = transparent) | 1=0x81a4 rgb(32,104,0) gruene Fuellung |
2=0x00e1 | 3=0x0081 | 4=0x5ad6 rgb(176,176,176) Wand-/Tuerlinie | 5..9 Graustufen |
10=0x0db8 | 11=0x02b8 | 12=0x55a8 | 13=0x3505 | 14=0x1476 | 15=0x146d`.

Gerenderte Bilder:
* `img_B/page4_MAP05_uv_overlay.png`, `img_B/page5_MAP06_uv_overlay.png` — die komplette
  256x256-4bpp-Seite mit Rect-Umrissen und Index (Schachbrett = Palettenindex 0 = transparent).
* `img_B/page4_screen_alpha.png`, `img_B/page5_screen_alpha.png` — die Seiten so, wie der
  Zeichner sie zusammensetzt (fixe Sprites (30,30) 88x32 uv(0,0) und (270,40) 32x48 uv(96,0)
  plus die Rects, Index 0 transparent).
* `img_B/tiles_compare.png` — alle relevanten Kacheln nebeneinander, 8x vergroessert.

### Was jede Kachel zeigt (Beschreibung am Rendering)

**Seite 4 (3F):**

| Rect | Kachel | Beschreibung |
|---|---|---|
| r0 | uv(168,40) 16x16 | winziger, fast quadratischer Raum, nur Umriss, keine Tuermarke |
| r1 | uv(168,16) 24x24 | kleiner Raum, eine Tuermarke unten links |
| r2 | uv(96,48) 32x40 | schlichtes Hochrechteck ohne Innenmarken |
| r3 | uv(192,16) 48x24 | **L-Form**: linker Block x0..17/y0..21 + unterer Arm bis x=41, y14..21; **zwei** graue Tuermarken (Index 4) bei Kachel-(10..15,1..5) und (10..14,16..20) |
| r4 | uv(0,32) 32x80 | sehr schmaler, langer Flur mit Knick, Tuermarke oben links |
| r5 | uv(128,16) 40x40 | L-foermiger Raum, Tuermarke rechts oben |
| r6 | uv(32,32) 56x56 | groesster Raum der Seite, Stufe rechts unten, zwei kleine Marken links |

**Seite 5:**

| Rect | Kachel | Beschreibung |
|---|---|---|
| r0 | uv(192,16) 48x24 | **dieselbe L-Form wie Seite-4-r3**, aber **ohne** die zwei Tuermarken |
| r1 | uv(0,32) 48x56 | grosser, fast rechteckiger Raum mit kleiner Stufe rechts unten |

Kein einziges Rechteck traegt ein **Treppensymbol**; die einzigen Innenmarken sind
Tuerboegen in Palettenindex 4.

---

## 3. DIE PIX ENTHALTEN NICHTS AUSSER DEN RECTS (Blob-Zaehlung)

`blobs.py` zaehlt alle zusammenhaengenden Nicht-Null-Bereiche:

* **MAP06.PIX (Seite 5)** hat genau **drei** Bildgruppen: (a) Titel/Kompass/Massstab y<16,
  (b) die beiden Rect-Kacheln uv(192,16) und uv(0,32), (c) einen **unbenutzten** Klecks bei
  **uv(193,177) 54x76**.
  `composite.py`: setzt man r0+r1 an ihren Bildschirmpositionen zusammen, ergibt das eine
  54x76-Flaeche, die mit dem Klecks bei uv(193,177) **byte-identisch** ist:
  **0 von 4104 Pixeln unterschiedlich (100,0 %)**.
  => Der Klecks ist die vom Kuenstler fertig zusammengesetzte Seite-5-Karte.
  **Auf Seite 5 existiert also KEIN drittes Raumbild.** (`img_B/p5_unused_big.png`)
* **MAP05.PIX (Seite 4)** analog: unbenutzter Klecks bei **uv(156,164) 96x85**; die 7 Rects
  zusammengesetzt ergeben 95x85 und stimmen bei Ausrichtung uv(157,164) zu **97,8 %**
  (181/8075 abweichende Pixel — die Differenz sind Tuermarken). (`img_B/p4_unused_big.png`)

---

## 4. HAT DAS ORIGINAL EINEN ETAGEN-BEGRIFF IN DER KARTENLOGIK? -> NEIN

### 4a. Der Seiten-Setzer @0x8004b568 liest NUR die Raumnummer

```
8004b568  lui   v1,0x800b
8004b56c  lh    v1,DAT_800b0fe2(v1)     ; Raum-Index (0..0x25)
8004b574  sltiu v0,v1,0x26
8004b578  beq   v0,zero,default
8004b58c  lw    v0,0x0(at)              ; Sprungtabelle @0x8001103c
8004b594  jr    v0
```

Fall-Through-Switch; die Seite (DAT_800b260e) wird nur an sechs Stellen geschrieben:

| Adresse | Fall | Wert |
|---|---|---|
| 0x8004b678 -> 0x8004b88c | caseD_b  (Raum 11) | Seite 2 |
| 0x8004b6ec -> 0x8004b88c | caseD_11 (Raum 17) | Seite 3 |
| 0x8004b74c -> 0x8004b88c | caseD_16 (Raum 22) | Seite 4 |
| **0x8004b75c/68 -> 0x8004b88c** | **caseD_17 (Raum 23 = ROOM1170)** | **Seite 5** |
| 0x8004b7dc | caseD_1d (Raum 29) | Seite 0 (`sb zero`) |
| 0x8004b87c/84 | caseD_25 (Raum 37) | Seite 1 |

In **jedem** Fall wird zusaetzlich `DAT_800b260d = DAT_800b0fe2` gesetzt (Marker-Skalen-
Zeilenindex = Raum-Index). Deckungsgleich mit `re15_inv_screen.c:254-259`.

### 4b. Im gesamten Karten-/Inventarcode wird KEIN Band und KEIN Y gelesen

Bereich 0x80046a1c ... 0x8004c3fc (Zeilen 159340-166399 in `ghidra1_V2.txt`) enthaelt
folgende `DAT_800ac*`-Referenzen — und sonst keine:

```
25x DAT_800aca34 (Doppelpuffer-Index)   10x DAT_800acaee    9x DAT_800ac76c (Pad)
 6x DAT_800ac762   6x DAT_800ac760       4x DAT_800acaec    3x DAT_800ac766
 2x DAT_800aca88  <- Spieler-X           2x DAT_800aca90 <- Spieler-Z
 2x DAT_800aca38
```

**`DAT_800acad6` (Spieler-Band = entity+0x82) kommt NICHT vor.** Alle 27 Xrefs von
DAT_800acad6 liegen bei 0x80014280, 0x8001d7a4/7ac/8f8/900, 0x8002bf38,
0x8002d364/4fc/5b0/5d0/694/6c4/760, 0x80031d04, 0x80035b08, 0x800366d4, 0x80036858,
0x800368e4, 0x800381c8..0x80038edc — Kollision/AOT/Treppe, **nie** Karte.
Ebenso fehlt DAT_800aca8c (Spieler-Y).

### 4c. Die Markerformel FUN_800473f8 bestaetigt es Instruktion fuer Instruktion

```
8004741c lw    v0,0x0(s4)=>DAT_800aca88        ; X
80047424 lbu   a2,DAT_800b260d                 ; Zeilenindex = RAUM-Index
80047428 addiu v0,v0,0x7d00                    ; +32000
8004742c sll   v1,v0,2 / addu v1,v1,v0 / sll t0,v1,1   ; *10
80047444 lhu   v0,DAT_800768b4(+idx*8)         ; sx
8004744c mult  t0,v0 / mflo t0 / sra t0,t0,0x14        ; >>20
80047460 addiu t0,t0,5
80047464 mult  t0,a3 (0x66666667)              ; /10
800474e8 lh    v0,DAT_800768b0(+idx*8)         ; ox
800474f4 addu  t0,t0,v0                        ; -> mx
8004746c lw    v1,DAT_800aca90                 ; Z
8004747c lhu   a0,DAT_800768b6(+idx*8)         ; sy
800474ac addiu v1,v1,5
800474b0 subu  v1,zero,v1                      ; <- die y-Spiegelung
800474b4 mult  v1,a3                           ; /10
80047514 ...   DAT_800768b2(+idx*8)            ; oy  -> my
```

Eingaenge: **X, Z, Raum-Index. Sonst nichts.** Kein Band, kein Y, kein Etagenterm.

> **ERGEBNIS (vollwertig):** Das Original hat einen Etagenbegriff nur auf **SEITEN**-Ebene
> (Titelbild B2/B1/1F/2F/3F), fest an die Raumnummer gekoppelt. **Innerhalb eines Raums gibt
> es keinerlei Etagen-Umschaltung.** Jede rauminterne Etagen-Umschaltung im Port ist eine
> PORT-ERGAENZUNG und muss als solche gekennzeichnet werden.

---

## 5. DIE MARKER-SKALENTABELLE @0x800768b0 ALS ORIGINAL-GROUND-TRUTH

8 Byte je Raum-Index: `{s16 ox, s16 oy, u16 sx, u16 sy}`, Datei-Offset **0x670b0**.
Nur **13 von 38** Zeilen sind kalibriert; die uebrigen stehen auf dem Stub `0,0,1,1`
(-> Marker landet bei ~(0,0); die Original-Karte ist an diesen Stellen unfertig):

| idx | Raum | Adresse | ox | oy | sx | sy |
|---|---|---|---|---|---|---|
| 2 | ROOM1020 | 0x800768c0 | 100 | 136 | 2287 | 2287 |
| 3 | ROOM1030 | 0x800768c8 | 108 | 194 | 2428 | 2229 |
| 4 | ROOM1040 | 0x800768d0 | 94 | 193 | 2080 | 2320 |
| 7 | ROOM1070 | 0x800768e8 | 79 | 205 | 2229 | 2088 |
| **21** | **ROOM1150** | **0x80076958** | 111 | 130 | 2296 | 2312 |
| **23** | **ROOM1170** | **0x80076968** | **100** | **206** | **2280** | **2268** |
| 25 | ROOM1190 | 0x80076978 | 75 | 127 | 1920 | 2301 |
| 26 | ROOM11A0 | 0x80076980 | 138 | 113 | 1776 | 2048 |
| 27 | ROOM11B0 | 0x80076988 | 159 | 110 | 2272 | 1954 |
| 30 | ROOM11E0 | 0x800769a0 | 91 | 157 | 2304 | 2240 |
| 32 | ROOM1200 | 0x800769b0 | 129 | 150 | 3168 | 2305 |
| 33 | ROOM1210 | 0x800769b8 | 177 | 140 | 2496 | 2250 |
| 38 | (jenseits des Switch) | 0x800769e0 | 80 | 196 | 2340 | 2308 |

`sx/sy ~ 2280..2300` ist der **feste Weltmassstab der Karte**: 1 Kartenpixel = 2^20/2290
~ **458 Welteinheiten**. Die GROESSE eines Raums auf der Karte ist damit **nicht frei
waehlbar** und taugt als Pruefmass.

### 5a. Damit laesst sich jede kalibrierte Zeile gegen ihre Seite pruefen

Projektion der RDT-Kollisionszellen durch die Originalformel:

| Raum | Seite | Projektion (Bildschirm) | bestes Rect | IoU |
|---|---|---|---|---|
| ROOM1020 | 2 | x[107,168] y[75,128] | r1 | 0,81 |
| ROOM1030 | 2 | x[117,193] y[126,192] | r4 | 0,89 |
| ROOM1040 | 2 | x[93,127] y[67,165] | r2 | 0,58 |
| ROOM1070 | 2 | x[143,196] y[107,144] | r3 | 0,74 |
| **ROOM1150** | **4** | x[118,150] y[81,121] | **r2** (120,80) 32x40 | **0,84** |
| ROOM11B0 | 0 | x[165,237] y[75,105] | r3 | 0,81 |
| ROOM1200 | 1 | x[137,176] y[99,143] | r2 | 0,75 |

=> **ROOM1150 -> Seite 4 Rect 2 ist damit ORIGINAL-BELEGT** (nicht nur heuristisch).
Der Port hat genau das: `re15_map_zones.h:56 { 0x1150, ..., 4, 2, 0, 24 }`. OK.

---

## 6. ROOM1170 IM DETAIL

### 6a. Kollision: VIER Baender, zwei Orte

`ROOM1170.RDT` Kollisionsblock @0x0a80, ceil=(55888,51982), 5 Gruppen a 30 Zellen
(die 5 Gruppen sind die Quadranten-Partition; Gruppe 0 genuegt):

| Band (`floor>>4`) | Zellen | Welt-Bbox | Rolle |
|---|---|---|---|
| 3 (floor=0x33) | 2 | x[-9632, 11410] z[-9128, 15640] | **Hof-/Helipad-BODEN** |
| 4 (floor=0x43) | 18 | x[-21946, 15658] z[-28734, 17746] | Hof-**Waende** + oberer Korridor des 2. Bereichs |
| 2 (floor=0x23) | 4 | x[-28900, -22300] z[-28160, -22885] | **Treppen-Absatz** |
| 0 (floor=0x03) | 6 | x[-28622, -17876] z[-28841, -17070] | **unterste Ebene** |

Zwei raeumlich getrennte Komponenten (GAP 1500):

* **comp0 = Hof**, 80 Zellen, x[-13430, 15658] z[-10910, 17746]
* **comp1 = 2. Bereich**, 70 Zellen, x[-28900, -8784] z[-28841, -17070]
  -> am Kartenmassstab **43,9 x 25,7 Kartenpixel**

Bild: `img_B/room1170_bands_scale.png` (alle Zellen am Original-Massstab, nach Band gefaerbt).

### 6b. Tuergraph (RDT-Tuersaetze) — welcher Bereich haengt woran

```
ROOM1170, 7 Tueren:
  lokal(  2550, 15250) -> ROOM1170 bei (-11710,-26500)   SELBST-TUER: Hof -> 2. Bereich
  lokal(  5660, -7940) -> ROOM10B0                        (Hof)
  lokal(     0,     0) -> ROOM1240                        (Eintritt)
  lokal(-21230,-17925) -> ROOM1130 bei ( 2650, 15550)     2. BEREICH -> 1130
  lokal(-21600,-27005) -> ROOM1140 bei (-7250, -1200)     2. BEREICH -> 1140
  lokal(-11065,-27850) -> ROOM1170 bei ( 2300, 14365)     2. Bereich -> Hof zurueck
Gegenprobe:
  ROOM1130 lokal( 2700,14650) -> ROOM1170 bei (-21225,-18870)   (= 2. Bereich)
  ROOM1140 lokal(-7250, -250) -> ROOM1170 bei (-21937,-25713)   (= 2. Bereich)
```

=> Der 2. Bereich von ROOM1170 grenzt an **ROOM1130 und ROOM1140** — beide liegen auf
**Seite 4 = 3F**. Der Hof grenzt an ROOM10B0/ROOM1240.
(Nebenbefund: ROOM1170 hat **keine** Tuer nach ROOM1150; 1150 haengt nur an 1130.)

### 6c. Der Hof ist BEWIESEN Seite-5-r1

Originalformel mit Zeile 23 (ox=100 oy=206 sx=2280 sy=2268):

| Gruppe | Welt | -> Bildschirm |
|---|---|---|
| **Band 3 (Hof-BODEN)** | x[-9632,11410] z[-9128,15640] | **x[149,194] y[103,157]** |
| Band 4 (Hof-Waende) | x[-13430,15658] z[-10910,17746] | x[140,204] y[98,160] |
| Band 0 (unterste Ebene) | x[-28622,-17876] z[-28841,-17070] | x[107,131] y[174,199] |
| Band 2 (Absatz) | x[-28900,-22300] z[-28160,-22885] | x[107,121] y[186,198] |
| comp1 gesamt | x[-28900,-8784] z[-28841,-17070] | x[107,150] y[174,199] |

**Rect r1 = (148,101) 48x56 -> x[148,196] y[101,157].**
Der Hof-BODEN landet auf **x[149,194] y[103,157]** — Abweichung <= 2 px auf allen vier Kanten.
=> **Seite 5 r1 = ROOM1170-Hof, byte-belegt.** Der Port hat das (`{0x1170,...,5,1,0,26}`). OK.
Bild: `img_B/page5_1170_projection.png` (rot = Hof-Zellen, blau = 2. Bereich, gelb = Rects).

### 6d. Der 2. Bereich faellt durch die ORIGINALFORMEL aus der Karte

comp1 -> x[107,150] y[174,199]. Um auf r0 (x[140,188] y[80,104]) zu landen, fehlten
**(+33, -94) Pixel**. Das ist **kein** Beweis, dass kein Rechteck existiert — es beweist nur,
dass das Original **nur EINE lineare Abbildung je Raum** kennt und die auf den Hof kalibriert
ist. Der 2. Bereich hat im Original **keine** gueltige Markerabbildung (derselbe Unfertig-
Zustand wie die 25 Stub-Zeilen in §5).

### 6e. ABER: r0 IST der 2. Bereich — geometrisch bewiesen

Gemessene Belegung der Seite-5-r0-Kachel (pro Zeile/Spalte):

* **linker Block** x 0..17, y 0..21
* **unterer Arm** x 0..41, y 14..21 (Zeilen 22/23 leer, Spalten 42..47 leer)

Der 2. Bereich, am **Kartenmassstab** (px/Einheit = 2280/2^20 bzw. 2268/2^20), in
Kachelkoordinaten (Ursprung x=-28900, z=-17070, z gespiegelt):

| Band | Welt | -> Kachel |
|---|---|---|
| 0 (unterste Ebene) | x[-28622,-17876] z[-28841,-17070] | x 0,6..24,0 / y 0..25,5 -> **linker Block** |
| 2 (Absatz) | x[-28900,-22300] z[-28160,-22885] | x 0..14,4 / y 12,6..24,0 -> **linker Block, unten** |
| 4 (oberer Korridor) | x[-21946,-8924] z[-28734,-23154] | x 15,1..43,4 / y 13,2..25,2 -> **unterer Arm** |

Groesse: comp1 = **43,9 x 25,7 px**, r0 = **48x24 px** -> Abweichung **5,9 px** gesamt; das ist
der **beste Groessentreffer aller 119 Rechtecke des Spiels** fuer diese Flaeche
(naechstbeste: Seite 12 r4 48x24, ebenfalls 5,9 — aber andere Stage).

Die vier Treppen-AOTs (Aot_set Typ 12/13) fallen genau in die richtigen Teile der L-Form
(Port-Abbildung Zonen-Bbox -> Rect, `img_B/r0_bands_overlay.png`):

| AOT | Welt | Kachelpixel | Lage in der L-Form |
|---|---|---|---|
| Selbsttuer-Ankunft (Band 4) | (-11710,-26500) | (41,19) | rechtes Ende des unteren Arms |
| Treppe Band 4 | (-20690,-25595) | (19,17) | unterer Arm |
| Treppe Band 2 | (-23295,-26155) | (13,18) | Knick |
| Treppe Band 2 | (-25640,-23425) | (7,12) | linker Block, unten |
| Treppe Band 0 | (-25670,-20645) | (7,7) | **linker Block, oben = unterste Ebene** |

=> **Der linke Block der L-Form IST die untere Etage; der untere Arm ist der obere Korridor.
Beides steckt in EINEM Rechteck.** Die Wendeltreppe Band 4 -> 2 -> 0 laeuft in der Kachel
diagonal von rechts unten nach links oben.

### 6f. Warum dieselbe L-Form auf Seite 4 nochmal auftaucht

`match2.py`: Seite-4-r3 uv(192,16) 48x24 und Seite-5-r0 uv(192,16) 48x24 unterscheiden sich in
genau **22 von 1152 Pixeln**, und **alle 22** sind Index **4 -> 1** (graue Tuerlinie -> gruene
Fuellung) an zwei Stellen: uv(202..207,17..21) und uv(202..206,32..36).
Die **Silhouetten sind exakt identisch** (Maskenvergleich: gleich in allen 1152 Pixeln,
IoU = 1,000).
=> **Dieselbe Zeichnung**, auf Seite 4 mit zwei Tuermarken, auf Seite 5 ohne.

Die zwei Tuermarken (Kachel-x ~ 10..15; eine oben y 1..5, eine unten y 16..20) passen nach
Anzahl und senkrechter Reihenfolge zu den **zwei** Ausgaengen des 2. Bereichs
(-> ROOM1130 bei Kachel-y ~ 2, -> ROOM1140 bei Kachel-y ~ 20). Die waagerechte Lage weicht um
~5-6 px ab (haengt davon ab, welche Bbox man ansetzt) — daher **stuetzend**, nicht beweisend.
Konsistente Lesart: der 2. Bereich liegt geografisch auf 3F (Tueren zu 1130/1140), deshalb hat
der Kuenstler ihn auf dem 3F-Blatt (r3, mit Tueren) **und** auf dem 1170-Blatt (r0) gezeichnet.

### 6g. Die Kachelfragen aus dem Auftrag, direkt beantwortet

* **Seite-4-r4 uv(0,32) 32x80 vs. Seite-5-r1 uv(0,32) 48x56 — derselbe Grundriss?**
  **NEIN.** Gleiche uv-Ecke, verschiedene Bilder: im gemeinsamen 32x56-Fenster **1142
  abweichende Pixel**; Silhouetten-IoU 0,41. Seite-4-r4 ist ein schmaler Flur
  (nonzero 961/2560), Seite-5-r1 ein fast voller Raum (2464/2688).
* **Seite-4-r3 uv(192,16) vs. Seite-5-r0 uv(192,16) — welcher Raum wirklich?**
  Beides **ROOM1170, 2. Bereich** (§6e/6f): Groesse 43,9x25,7 px vs. 48x24 (Delta 5,9 px),
  die L-Form deckt Band 0/2 (linker Block) und Band 4 (unterer Arm), der Tuergraph zu 1130/1140
  erklaert die Doppelzeichnung auf dem 3F-Blatt.
  **Nicht** ROOM1140-Zone 1 und **nicht** ROOM1160.

---

## 7. PRUEFUNG DER PORT-ZONEN-ZUORDNUNG FUER SEITE 4

Pruefmass: die Kartengroesse ist ueber sx ~ 2290/2^20 **fest**; die Zellen-Bbox eines Raums
ergibt seine Pixelgroesse.
(Die Silhouette taugt **nicht** als Mass — SCA-Zellen sind Wandquader, nicht der Boden;
ROOM1150 erreicht gegen sein BEWIESENES r2 nur IoU 0,17.)

| Raum / Zone | Welt-Bbox (Gruppe 0) | Kartenpixel | Port -> Rect | Rectgroesse | Delta |
|---|---|---|---|---|---|
| ROOM1120 | 19200 x 21000 | 41,9 x 45,9 | r5 | 40x40 | **8** OK |
| **ROOM1130** | 14650 x 38150 | **32,0 x 83,3** | **FEHLT in der Tabelle** | (r4 32x80 waere Delta **3**) | LUECKE |
| ROOM1140 Zone 0 | 27300 x 27450 | 59,6 x 59,9 | r6 | 56x56 | **8** OK |
| ROOM1140 Zone 1 | 12700 x 6100 | 27,7 x 13,3 | **r3** | 48x24 | **31** FALSCH |
| ROOM1150 | 14292 x 17694 | 31,2 x 38,6 | r2 | 32x40 | **2** OK (zusaetzlich original-belegt, §5a) |
| ROOM1160 | 16750 x 13100 | 36,6 x 28,6 | r1 | 24x24 | 17 (r3 waere 16 -> Patt) |
| ROOM1170 Bereich 2 | 20116 x 11771 | 43,9 x 25,7 | (Seite 5 r0) | 48x24 | **6** |

Befunde:

1. **r4 (32x80) gehoert ROOM1130** (32,0 x 83,3 px — Delta 3 px, der schaerfste Treffer der
   ganzen Seite). ROOM1130 fehlt komplett in `re15_map_zones.h` (kein `0x1130`-Eintrag),
   r4 ist unbelegt. Echte Luecke.
2. **r3 gehoert NICHT ROOM1140 Zone 1.** ROOM1140 Zone 1 ist 27,7 x 13,3 px, r3 ist 48x24
   (Delta 31 px). Zusaetzlich liegt die Zone-1-Bbox x[-5250,7450] z[-17450,-11350]
   **vollstaendig innerhalb** der Zone-0-Bbox x[-10600,16700] z[-25600,1850]; beide Zellgruppen
   sind ausschliesslich Band 0 — es sind also keine zwei getrennten ORTE, sondern ein
   Zerlegungs-Artefakt des Generators.
3. Der Eintrag aus v0.3.69 (ROOM1170-Zone 1 -> Seite 4 r3) war damit **inhaltlich richtig, was
   das RECHTECK angeht** — nur die SEITE stimmt nicht, denn das Original zeigt fuer Raum 23
   immer Seite 5 (@0x8004b75c-68).
4. ROOM1120 -> r5, ROOM1140 Zone 0 -> r6, ROOM1150 -> r2 sind bestaetigt.
5. ROOM1160 -> r1 vs. r3 ist mit der Groesse **nicht** entscheidbar (17 vs. 16 px) — nicht
   anfassen. (Indiz gegen r3: ROOM1160s beide Tuersaetze zeigen auf dieselbe Position
   (-300,-16500), also nur **ein** Tuersymbol zu erwarten; r3 traegt **zwei**.)
6. r0 (16x16) bleibt ohne Zuordnung — kein Raum der Seite ist so klein.

---

## 8. ANTWORT AUF DEN NUTZER-REPORT

"Beim Runterlaufen der Treppe wechselt die Map erneut nicht."
**Das ist korrekt so — es gibt nichts, worauf gewechselt werden koennte:**

* MAP06.PIX enthaelt ausser den zwei Rects nur deren 100 % byte-identische Zusammensetzung (§3).
* Der 2. Bereich ist als **eine** L-Form gezeichnet, in der der linke Block die unterste Ebene
  und der untere Arm den oberen Korridor darstellt (§6e).
* Das Original liest im gesamten Kartencode **kein** Band und **kein** Y (§4b/4c).

Was beim Abstieg tatsaechlich passiert (Port-Rechnung nachgerechnet,
`re15_map_zones.c:123-146`, Zone-1-Bbox -> r0):

```
(-11710,-26500) -> Kachel(41,19)   Ankunft aus der Selbsttuer, Band 4
(-20690,-25595) -> Kachel(19,17)   Treppe Band 4
(-23295,-26155) -> Kachel(13,18)   Treppe Band 2
(-25640,-23425) -> Kachel( 7,12)   Treppe Band 2
(-25670,-20645) -> Kachel( 7, 7)   Treppe Band 0 (unterste Ebene)
```

Der Marker **wandert** ueber die Treppe vom rechten Ende des unteren Arms in den linken Block
hinauf — genau entlang der gezeichneten Wendeltreppe. Der Port macht das bereits richtig.

---

## 9. WAS OFFEN BLIEB

* Die genaue Identitaet von Seite-4-r0 (16x16) und der ROOM1160-Zuordnung (r1 vs. r3) ist mit
  Groesse+Silhouette nicht entscheidbar. Naechster Weg: die Tuermarken (Index-4-Pixel) jeder
  Kachel als Punkte extrahieren und gegen die RDT-Tuerpositionen aller Seiten-4-Raeume
  optimieren (ein Zuordnungsproblem mit harter Groessenschranke) — oder ein DuckStation-
  Savestate im Kartenschirm von ROOM1160 und den Marker direkt ablesen (dessen Skalenzeile 22
  ist allerdings ein Stub, der Marker landet also bei ~(0,0) — dieser Weg trifft ins Leere).
* Warum Seite 2 und Seite 3 identische Rect-Listen haben (Seite 3 = Seite 2 ohne r10) ist nicht
  untersucht.
* Der Klecks bei MAP05 uv(157,164) weicht in 181 Pixeln von der Rect-Zusammensetzung ab; welche
  Tuermarken dort zusaetzlich stehen, ist nicht ausgewertet.
