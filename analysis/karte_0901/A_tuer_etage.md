# A — ROOM1170: die Tür der unteren Etage stand auf dem ROOF-Blatt

**Nutzer-Report (2026-09-01):** „Bei ROOM 1170 zeigt er die Tuer, die eigentlich fuer die
Etage unten gedacht ist, bereits bei der Roof-Etage an."

**Kurzfassung.** Der Report ist wörtlich richtig, und ich habe ihn pixelgenau reproduziert:
das ausgelieferte v0.3.80 malt auf dem ROOF-Blatt (Seite 5 Rect 0) **35 Pixel**, die dort
nicht hingehören — zwei Türsymbole und ein Treppensymbol, die alle **Band 0 bzw. 0/2**
tragen und damit auf das 3F-Blatt gehören. Ursache ist ein Kopierschritt im Generator, der
ALLE Marken einer Zone auf JEDES Etagenblatt der Zone legte, ohne das Band der einzelnen
Marke anzusehen. Das Original entscheidet dieselbe Frage über die **Zeichnung**:
`DATA/MAP05.PIX` (3F) malt die beiden Türnischen in die Kachel, `DATA/MAP06.PIX` (ROOF)
malt an derselben Kachel-uv **gar nichts** — Unterschied exakt 22 von 1152 Pixeln, alles
Symbolpixel.

Während dieser Untersuchung (Arbeitsbaum, noch nicht committet: `re15_map_zones.h`
geschrieben 12:16:20, Build `build_fix` 12:16:46) wurde in `gen_map_zones.py` bereits ein
Gegenmittel eingebaut (`blatt_fuer_band`). Ich habe es unabhängig nachgemessen: es räumt die
35 Pixel restlos ab, lässt alles andere in Ruhe und ist auch an ROOM1060/ROOM4070 richtig.
Es hat aber **eine messbare Nebenwirkung** (§7): ROOM10C0s Tür zum Treppenhaus wandert auf
dem 2F-Blatt aus dem eigenen Grundriss in das 16×16-Kästchen des Treppenhauses.

---

## 1. Alle Durchgänge von ROOM1170 mit Band und Weltposition

Quelle: `shared_assets/PSX/STAGE1/ROOM1170.RDT`, `mainScd` @0x11F4, Sektionstabelle
RDT+0x20. Türen = `Door_aot_set` Opcode 0x3b (32 B), Band = `pc[4]` = `obj[0x82]`
(das Original gattet die Tür-Interaktion darauf, FUN_8002bd44 @0x8002bf38).
Treppen = `Aot_set` Opcode 0x2c mit `sce` 12 (X-Achse) / 13 (Z-Achse), `chain = pc[4]` =
Band dieses Endes, `count = pc[16]` = Zahl der überquerten Bänder
(Handler LAB_80043500 liest x @0x80043510-14, LAB_800435cc liest z @0x800435dc-e0;
`lbu count` @0x800435b8 bzw. @0x8004367c).

| # | Datei-Offset | Slot | Art | Band | Weltmitte (x,z) | Rechteck (x,z,w,d) | Ziel | Spawn-Y | Zone |
|---|---|---|---|---|---|---|---|---|---|
| T0 | 0x001206 | 0 | Tür | **4** | (2550, 15250) | (1500,14400,2100,1700) | ROOM1170 (selbst) | **-7200** | 0 |
| — | 0x001226 | 1 | Tür | 4 | (5660, -7940) | (5040,-9400,1240,2920) | ROOM10B0 | -9000 | — |
| — | 0x00129C | 2 | Tür | 0 | (0,0) | **0×0** | ROOM1240 | 0 | — |
| — | 0x0012CC | 3 | Tür | 0 | (0,0) | **0×0** | ROOM1170 | -7200 | — |
| T1 | 0x0012F8 | 4 | Tür | **0** | (-21230, -17925) | (-22370,-18380,2280,910) | **ROOM1130** | **0** | 1 |
| T2 | 0x001338 | 5 | Tür | **0** | (-21600, -27005) | (-22740,-27610,2280,1210) | **ROOM1140** | **0** | 1 |
| T3 | 0x00135A | 6 | Tür | **4** | (-11065, -27850) | (-11940,-28450,1750,1200) | ROOM1170 (selbst) | **-7200** | 1 |
| S0 | 0x00137A | 7 | Treppe | **0** | (-25670, -20645) | Achse Z, count 2 | → Band 2 | — | 1 |
| S1 | 0x00138E | 8 | Treppe | **2** | (-25640, -23425) | Achse Z, count 2 | → Band 0 | — | 1 |
| S2 | 0x0013A2 | 9 | Treppe | **2** | (-23295, -26155) | Achse X, count 2 | → Band 4 | — | 1 |
| S3 | 0x0013B6 | 10 | Treppe | **4** | (-20690, -25595) | Achse X, count 2 | → Band 2 | — | 1 |

Nicht auf der Karte (und richtig so): Slot 1 hat `sce = 0` und wird nirgends per
`Aot_reset` neu bewaffnet — im Leon-Szenario tot; Slot 2 und 3 haben ein
**0×0-Rechteck**, sind also Skript-Warps auf genau einen Weltpunkt (Trefftest
FUN_80042b64 @0x80042b68-98 ist Ecke+Ausdehnung, `w = d = 0` heißt Gleichheit auf beiden
Achsen), keine begehbaren Türen.

### 1.1 Gegengröße für das Band-Byte: die Spawn-Y desselben Datensatzes

Das Band steht in `pc[4]`. Völlig unabhängig davon steht in demselben Datensatz die
**Spawn-Y** (Feld `ny`, Bytes +14..+15 des 0x3b-Records). Die Engine leitet ein Band aus
einer Y ab als `band = -(y / 0x708)` (0x708 = 1800; `re15_collision.c:186`, dieselbe Formel
benutzt der Spielstand-Loader). Probe:

* T0 / T3: Spawn-Y **-7200** = -4 · 1800 → Band **4** ✓ (in `pc[4]` steht 4)
* T1 / T2: Spawn-Y **0** → Band **0** ✓ (in `pc[4]` steht 0)

Zwei verschiedene Bytes desselben Records sagen dasselbe. Das Band-Byte ist damit bestätigt,
ohne die Kartenlogik zu benutzen.

### 1.2 Zweite Gegengröße: die Kollisionsgeometrie

`ROOM1170.RDT` Kollision @0xA80, `counts = (30,30,30,30,30)` (fünf byte-gleiche
Quadranten-Gruppen), 150 Zellen à 12 B ab 0xA98, Layout
`{u16 w, u16 d, s16 x, s16 z, u8 type, u8 u0, u8 u1, u8 floor}`, Band = `floor >> 4`:

| Bereich | Band | Zellen | Bbox der Band-Zellen |
|---|---|---|---|
| Zone 1 (Treppenblock) | 0 | 30 | x[-28622..-17876] z[-28841..-17070] |
| | 2 | 20 | x[-28900..-22300] z[-28160..-22885] |
| | 4 | 20 | x[-21946..-8784] z[-28734..-22758] |
| Zone 0 (Helipad) | 3 / 4 | 10 / 70 | — |

T3 (Band 4) liegt bei x = -11065 — **nur** die Band-4-Zellen reichen so weit nach Osten
(Band 0 endet bei x = -17876). T1 (Band 0) liegt bei z = -17925 — **nur** die Band-0-Zellen
reichen so weit nach Norden (Band 2 endet bei z = -22885, Band 4 bei z = -22758). Der
Helipad (Zone 0) ist selbst Band 3/4, und T0 ↔ T3 verbinden ihn mit dem Block: der Block ist
oben Band 4 = **auf Dachhöhe**, unten Band 0 = **7200 Einheiten tiefer** = 4 Bänder = 3F.

---

## 2. Welcher Durchgang gehört auf welches Blatt

Die Etagentabelle `s_map_floors` (generiert, `re15_map_zones.h`) sagt für ROOM1170 Zone 1:

```c
{ 0x1170, 1,  0,  4,  3 },   /* Band 0 -> Seite 4 Rect 3 */
{ 0x1170, 1,  4,  5,  0 },   /* Band 4 -> Seite 5 Rect 0 */
```

Die Seiten-ids stehen @0x80074c4c (u16 je Seite, id 12..24 → MAP01..MAP0D):
**Seite 4 → id 16 → MAP05.PIX**, **Seite 5 → id 17 → MAP06.PIX**.
Die Etagen-Beschriftung steckt im Titelbild jeder Kachel (uv(0,0), 88×32, gezeichnet als
festes Sprite bei (30,30) — Bauer FUN_80046fd8, xy `ori 0x1e/0x1e`, wh `0x58/0x20`).
Aus den PIX-Dateien abgelesen:

| Seite | PIX | Etagen-Label im Bild |
|---|---|---|
| 0 | MAP01 | B1 |
| 1 | MAP02 | B2 |
| 2 | MAP03 | **1F** |
| 3 | MAP04 | **2F** |
| **4** | **MAP05** | **3F** |
| **5** | **MAP06** | **(leer)** — das Helipad-Dach, im Original unbeschriftet |
| 6 | MAP07 | B2 |
| 7 | MAP08 | (leer) |
| 8 / 9 / 10 / 11 | MAP09..0C | B1 / B2 / B3 / B4 |
| 12 | MAP0D | (leer) |

Damit ist die Zuordnung eindeutig und **gemessen, nicht gesetzt**:

| Durchgang | Band | gehört auf | Begründung |
|---|---|---|---|
| T1 → ROOM1130 | 0 | **Seite 4 (3F)** | Zielraum ROOM1130 liegt auf Seite 4; Spawn-Y 0 |
| T2 → ROOM1140 | 0 | **Seite 4 (3F)** | Zielraum ROOM1140 liegt auf Seite 4; Spawn-Y 0 |
| T3 → ROOM1170 Zone 0 | 4 | **Seite 5 (ROOF)** | Ziel ist der Helipad, Spawn-Y -7200 |
| T0 → ROOM1170 Zone 1 | 4 | **Seite 5 (ROOF)** | Gegenstück von T3, dieselbe Tür |
| S0 / S1 (Bänder 0↔2) | 0 / 2 | **Seite 4 (3F)** | beide Enden liegen unterhalb Band 4 |
| S2 / S3 (Bänder 2↔4) | 2 / 4 | **Seite 4 UND Seite 5** | verbindet die beiden Blätter |

Band 2 ist das Zwischenpodest und hat **kein eigenes Blatt**. Sowohl der Generator
(`blatt_fuer_band`) als auch die Laufzeit (`re15_map_floor_lookup`, `re15_map_zones.c:357`,
und `floor_row` :332) nehmen das Blatt mit dem kleinsten `|zeile.band − band|` und lösen den
Gleichstand mit `<` (echt kleiner) über die Tabellenreihenfolge auf; die Tabelle ist nach
Band aufsteigend sortiert → **Gleichstand fällt immer auf die untere Etage**. Beide Seiten
benutzen dieselbe Regel — das ist wichtig: liefe der Generator anders als die Laufzeit, säße
eine Marke auf einem Blatt, das die Laufzeit für dieses Band nie zeigt, und sie fehlte
einfach.

---

## 3. Beleg, dass der Port ALLE Marken auf JEDES Blatt legte

### 3.1 Die Markentabelle vor der Änderung (Git-HEAD = ausgeliefertes v0.3.80)

```c
{  4,  3,  159,   98, 4,  24, 255 },   /* Treppe A, waagerechte Sprossen */
{  4,  3,  168,  106, 5,  24, 255 },   /* Treppe B, senkrechte Sprossen  */
{  5,  0,  147,   89, 4,  24, 255 },   /* Treppe A   <- Baender 0/2 = 3F */
{  5,  0,  156,   97, 5,  24, 255 },   /* Treppe B                       */
{  5,  0,  157,   80, 0,  24, 255 },   /* TUER Nord  <- Band 0 = 3F      */
{  5,  0,  157,  101, 2,  24, 255 },   /* TUER Sued  <- Band 0 = 3F      */
{  5,  1,  174,  101, 0,  23,  24 },   /* Selbst-Tuer T0/T3, Band 4      */
```

Rechtecke (Tabelle @0x80076840[seite] = {count, ptr}, Einträge 12 B
`{x,y,w,h,u@+8,v@+10}`, gelesen im Bauer @0x8004731c-60):
`rects(5)[0] = (140,80,48,24) uv(192,16)` und `rects(4)[3] = (152,89,48,24) uv(192,16)` —
**dieselbe Kachel-uv**, Versatz genau (+12,+9). Alle vier Marken von Rect (5,0) tauchen in
(4,3) mit exakt diesem Versatz wieder auf bzw. wurden dort unterdrückt. Das ist der
Kopierschritt „MARKEN AUF DIE ZWEIT-ZEICHNUNG MITNEHMEN" in `gen_map_zones.py`: er kopierte
die **Zone**, nicht die einzelne Marke.

Zuordnung der Marken zu den Datensätzen (Bbox-Projektion der Zone auf ihr Rechteck, z-Achse
gespiegelt wie im Original):

| Datensatz | rohe Projektion auf (5,0) | Marke in der Tabelle |
|---|---|---|
| T1 (Band 0 → 1130) | (158, 81) | (157, 80) kind 0 = Nordwand |
| T2 (Band 0 → 1140) | (157, 100) | (157, 101) kind 2 = Südwand |
| T3 (Band 4 → Zone 0) | (182, 101) | — (mit T0 zu **einer** Marke auf (5,1) verschmolzen) |
| S0 (147,87) + S1 (147,92) | Mittelpunkt (147, 89) | (147, 89) kind 4 |
| S2 (153,98) + S3 (159,97) | Mittelpunkt (156, 97) | (156, 97) kind 5 |

Die beiden auf dem ROOF-Blatt gezeichneten Türen sind also **genau die beiden
Band-0-Türen** nach ROOM1130 und ROOM1140. Die Band-4-Tür ist auf Rect (5,0) gar nicht
vertreten — sie wurde als gemeinsamer Durchgang mit T0 auf Rect (5,1) gelegt. Der Nutzer
sieht auf dem Dach also *ausschließlich* die falschen Türen.

Zum Zeichnen: `re15_inv_screen.c` Abschnitt 4c filtert die Markentabelle mit genau einer
Bedingung — `if (mpage != (int)st->map_page) continue;`. Kein Band, kein Rect. Ein Band
könnte dort auch gar nicht geprüft werden: `re15_map_mark_t` = `{page, rect, mx, my, kind,
zid, zid2}` **führt kein Band-Feld**. Der Filter muss deshalb im Generator sitzen.

### 3.2 Pixel-Gegenprobe am ausgelieferten Stand

Lauf ab Titel mit dem **ausgelieferten** `release/pkg-win/re15_port_v0.3.80/re15_pc.exe`
(11:51 Uhr — der Stand, den der Nutzer getestet hat), und derselbe Lauf mit dem aktuellen
Arbeitsbaum-Build `build_fix` (12:16:46):

```
RE15_NO_INTRO=1  RE15_TITLE_SHOT=…  RE15_DEBUG_JUMP=1170@gp
RE15_INV_OPEN_AT=60#1170  RE15_MAP_SHOT_PAGE=5
RE15_INV_FB_SHOT=…bmp  RE15_INV_FB_SHOT_AT=40
```

Differenz innerhalb Rect (5,0) = x140..187 / y80..103, 1152 Pixel:

```
Unterschied alt vs. neu: 35 Pixel
   y= 80  x=[159]
   y= 81  x=[155, 159]        <- NORD-Tuersymbol (Marke 157,80 kind 0)   11 Pixel
   y= 82  x=[155, 159]           (SYM hat 13; 155,80 und 157,85 liegen auf
   y= 83  x=[155, 159]            einer ohnehin gemalten Wandzeile)
   y= 84  x=[155, 158]
   y= 85  x=[155, 156]
   y= 87  x=[145,146,147,148,149]   <- Treppe A, drei 5x1-Sprossen        15 Pixel
   y= 89  x=[145,146,147,148,149]      (Marke 147,89 kind 4)
   y= 91  x=[145,146,147,148,149]
   y= 96  x=[155, 157]        <- SUED-Tuersymbol (Marke 157,101 kind 2)    9 Pixel
   y= 97  x=[155]                (155,101/159,101 liegen auf der Bodenwand,
   y= 98  x=[155, 159]            156,96/158,97 auf Sprossen der Treppe B)
   y= 99  x=[155, 159]
   y=100  x=[155, 159]
```

11 + 15 + 9 = 35. Alle drei entfernten Marken tragen Band 0 bzw. 0/2. Außerhalb dieser
35 Pixel sind die beiden Abzüge in Rect (5,0) **identisch** — insbesondere stehen die drei
senkrechten Sprossen der Treppe B (Marke 156,97) in beiden Läufen unverändert bei
x = 154/156/158, y = 95..99. Der Fix hat also genau das Falsche entfernt und nichts sonst.

Abzüge: `…/scratchpad/roof_v0380.bmp` (alt) und `…/scratchpad/roof.bmp` (neu).

### 3.3 Gegenprobe auf dem 3F-Blatt (Seite 4, aktueller Build)

Derselbe Lauf mit `RE15_MAP_SHOT_PAGE=4`, Abzug `…/scratchpad/s4_neu.bmp`,
Rect (4,3) = (152,89,48,24):

```
 89 GGGGGGGGGGGGGGGGGGG      Nordwand
 90 G##########G###G#G       <- gemalte NORD-Nische der Kachel (x163..167,
 91 G##########G###G#G          y90..94) - kommt aus MAP05, nicht vom Port
 92 G##########G###G#G
 93 G###########G##G#G
 94 G############GGG#G
 96 G####WWWWW#######G       <- Treppe A, Port-Marke (159,98) kind 4
 98 G####WWWWW#######G
100 G####WWWWW#######G
103 G################GGGGGGGGGGGGGGGGGGGGGGGGG
104 G#############W#W#W######################G   <- Treppe B, Port-Marke
105 G#########GGG#W#W#W######################G      (168,106) kind 5
106 G#########G##GW#W#W######################G
107 G#########G###W#W#W######################G   <- gemalte SUED-Nische
108 G#########G###W#W#W######################G      der Kachel (x162..166,
109 G#########G###G##########################G      y105..109)
110 GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
```

Auf 3F sind **beide Türen sichtbar** — gezeichnet vom Rechteck-Sprite aus der Kachel, nicht
vom Port — plus beide Treppen als Port-Marken. Keine Doppelung. Das ist das gewünschte Bild.

*(Restposten:* zwei der elf gemalten Pixel der Süd-Nische, (166,107) und (166,108), werden
von der rechten Sprosse der Treppe B überschrieben — 2 von 11. Kosmetisch, aber es ist
dieselbe Fehlerklasse, die schon einmal als „es fehlt die eingezeichnete Tuer unter der
Treppe" gemeldet wurde. Behebbar, indem die Treppen-Sprossen keine Pixel setzen, an denen
die Kachel bereits Palettenindex 4 zeigt.)*

---

## 4. Was das ORIGINAL an Türen zeichnet — Daten oder Farbe?

### 4.1 Die Zeichenroutine zeichnet **keine** Türen

`FUN_800473f8` @0x800473f8–0x80047644 ist die komplette Zeichenroutine des Kartenschirms
(einziger Aufrufer FUN_80049a5c @0x80049bcc). Sie tut genau zwei Dinge:

**(1) Positionsmarker.** Aus der Spieler-Welt-XZ (`DAT_800aca88` @0x8004741c,
`DAT_800aca90` @0x8004746c) und der Maßstabszeile:

```
80047424  lbu   a2, DAT_800b260d        ; Zeilenindex (Raum-Slot)
80047434  sll   a2, a2, 0x3             ; * 8 Byte je Zeile
80047444  lhu   v0, DAT_800768b4(at)    ; sx   (lhu = VORZEICHENLOS)
8004747c  lhu   a0, DAT_800768b6(at)    ; sy
800474e8  lh    v0, DAT_800768b0(at)    ; ox
80047518  lhu   v1, DAT_800768b2(at)    ; oy
80047528  sh    v0, DAT_800b2606        ; my
```

Daraus die vier Ecken eines 8×8-Quads (`addiu a0,t0,-0x4` @0x80047554,
`addiu a1,t0,0x4` @0x80047564, y ±4 @0x80047580/@0x800475ac), **zuerst** eingehängt
(`AddPrim` @0x800475d8).

**(2) `count + 2` fertige Prims.** `count` ist die Rechteckzahl der Seite:

```
800474cc  lhu   s3, DAT_80076840(at)    ; at = 0x80076840 + seite*8
800475e0  addiu v0, s3, 0x2             ; count + 2
800474b8  ori   s1, s1, 0xa900          ; Prim-Feld &DAT_8019a900
800475f8  addiu s1, s1, 0x28            ; Schrittweite 0x28
80047608  jal   AddPrim
80047614  slt   v0, v0, s2              ; bis count+2 erreicht
80047640  jr    ra
```

**Nichts sonst.** Keine Schleife über Türen, keine über Treppen, kein Zugriff auf eine Tür-
oder Aot-Tabelle. Auch das Spieler-Band `DAT_800acad6` kommt in der ganzen Routine nicht
vor (und im Seiten-Setzer @0x8004b568 ebenfalls nicht — der liest nur die Raumnummer).

Was in `&DAT_8019a900` steht, baut `FUN_80046fd8` @0x80046fd8–0x800473e4 einmalig auf:

* 2 × Marker-`POLY_F4` (Doppelpuffer `&DAT_8019a800` / `&DAT_8019a828`)
* 2 × festes `SPRT`, xy(0x1e,0x1e) wh(0x58,0x20) uv(0,0) — das **Titelbild mit der Etage**
* 2 × festes `SPRT`, xy(0x10e,0x28) wh(0x20,0x30) uv(0x60,0) — der **Nordpfeil**
  (auf allen Seiten pixelgleich; nachgemessen an MAP03/04/05/06)
* je Rechteck ein `SPRT`, Code `0x64 | 2` (semitransparent), dessen x/y/w/h/u/v **direkt**
  aus dem 12-Byte-Eintrag der Rechteck-Tabelle @0x80076844[seite] kommen

Die beiden festen Sprites sind exakt die „+2" aus `addiu v0,s3,0x2`.
**Ein Türsymbol ist im Original also kein Prim und keine Datenstruktur — es ist Teil der
Kachel-Textur.**

### 4.2 …sondern die Kachel malt sie

Die Kacheln sind headerlose 256×256-4bpp-Bilder in `DATA/MAP0x.PIX` (128 Byte je Zeile,
unteres Nibble = linkes Pixel). Direkter Vergleich der Kachel uv(192,16), 48×24 — dieselbe
uv, die Rect (4,3) und Rect (5,0) beide tragen:

```
MAP05 (Seite 4 = 3F)                  MAP06 (Seite 5 = ROOF)
 0 |##################                 0 |##################
 1 |#1111111111#111#1#                 1 |#1111111111111111#
 2 |#1111111111#111#1#                 2 |#1111111111111111#
 3 |#1111111111#111#1#                 3 |#1111111111111111#
 4 |#11111111111#11#1#                 4 |#1111111111111111#
 5 |#111111111112###1#                 5 |#1111111111111111#
 …                                     …
16 |#111111111###1111…                16 |#11111111111111111…
17 |#111111111#11#111…                17 |#11111111111111111…
18 |#111111111#111#11…                18 |#11111111111111111…
19 |#111111111#111#11…                19 |#11111111111111111…
20 |#111111111#111#11…                20 |#11111111111111111…
21 |#################…                21 |#################…
```

Unterschied: **22 von 1152 Pixeln**, alle von Palettenindex 4 (Wandlinie, rgb(176,176,176))
auf Index 1 (Innenraum). Sie bilden zwei 5×5-Stempel bei Kachel-(13,3) und (12,18) — zwei
Türnischen, eine in der Nord-, eine in der Südwand.

Der Symbolkatalog `analysis/kartensymbole/symbolkatalog.csv` führt genau diese beiden als
`TUER, 5x5 Standard, Palettenindex 4`, Wand **N** und **S**, Bildschirm (163,90) und
(162,105) — Mittelpunkte (165,92) und (164,107).

**Zuordnung Symbol ↔ Datensatz, mit Nullmodell.** ROOM1170s Türen der Zone 1, projiziert
auf Rect (4,3):

| Datensatz | Band | Kartenpixel | Abstand zu Symbol N (165,92) | Abstand zu Symbol S (164,107) |
|---|---|---|---|---|
| T1 → ROOM1130 | 0 | (170, 90) | **5,4 px** | 18,0 px |
| T2 → ROOM1140 | 0 | (169, 109) | 17,5 px | **5,4 px** |
| T3 → Zone 0 (ROOF) | 4 | (194, 110) | 34,1 px | 30,1 px |

Die beiden Band-0-Türen liegen 5,4 px von „ihrem" Symbol, die Band-4-Tür 30–34 px von
beiden — Faktor 5,6 bis 6,3.

**Unabhängige Gegengröße, die in dieser Zuordnung nicht vorkommt: die Wandseite.**
Der Katalog liest sie aus den Kachelpixeln (auf welcher Seite die Wand weiterläuft), der
Port leitet sie aus der RDT-z-Koordinate und der Kachelkante ab (`snap_wall`). Beide sagen
für T1 **Nord** und für T2 **Süd**. Zwei getrennte Herleitungen, gleiches Ergebnis, bei
beiden Türen. Und es sind genau zwei gemalte Symbole für genau zwei Band-0-Türen.

**Auf dem ROOF-Blatt malt das Original nichts.** Unabhängig vom Katalog nachgezählt —
Wandpixel (Index 4), die in mindestens einer Achse beidseitig von Innenraum umgeben sind,
also keine Außenwand sein können:

```
Seite 4 (MAP05): Rect 1: 10   Rect 3: 20   Rect 4: 10   Rect 5: 10   Rect 6: 17   (= 67)
Seite 5 (MAP06): keine, in KEINEM ihrer Rechtecke                                 (=  0)
```

Der Katalog kommt zum selben Ergebnis: 92 Symbole über alle Seiten
(60 TUER, 15 TUER2, 6 TREPPE, 3 TREPPENHAUS, 4 ARTEFAKT, 4 SONST), davon **null auf
Seite 5**.

### 4.3 Fazit zu §4

Die Frage „welche Tür auf welchem Blatt" ist im Original **nicht über Code** entschieden —
der Kartencode kennt weder Türen noch das Spieler-Band. Sie ist über die **Zeichnung**
entschieden, und die Zeichnung sagt: die beiden Band-0-Türen gehören auf 3F, auf ROOF
gehört keine Tür. Der Port muss diese Aussage nachbilden; ein Band-Filter tut genau das.
Die Etagen-Umschaltung selbst bleibt eine Port-Ergänzung — aber eine, deren *Ergebnis* an
der Original-Grafik prüfbar ist.

---

## 5. Die Filterregel

**Regel (das ist auch das, was seit 12:16 im Arbeitsbaum steht — `blatt_fuer_band`):**

1. **Jede Marke trägt ihr eigenes Band**, nicht das ihrer Zone.
   Tür → `Door_aot_set pc[4]`. Treppe → `pc[4]` **ihres Endes** (jedes Ende ist ein eigener
   `Aot_set`-Datensatz vom Typ 12/13).
2. **Das Blatt einer Marke** = die `s_map_floors`-Zeile ihrer (Raum, Zone) mit dem kleinsten
   `|zeile.band − marke.band|`. Bei Gleichstand gewinnt die **erste** Zeile; die Tabelle ist
   nach Band aufsteigend sortiert, der Gleichstand fällt also auf die **untere** Etage.
   Die Marke wird um die Differenz der Rechteck-Ecken versetzt (die Kachel-uv ist dieselbe).
   ⛔ **Generator und Laufzeit müssen hier dieselbe Regel fahren.** Die Laufzeit
   (`re15_map_floor_lookup` :357 und `floor_row` :332 in `re15_map_zones.c`) tut es bereits:
   gleiche Nächster-Nachbar-Suche, gleiches `<`, gleiche Tabellenreihenfolge.
3. **Eine Treppe braucht keine Sonderregel.** Sie liegt als zwei Datensätze vor, einer je
   Ende; jedes Ende geht auf sein Blatt. Verbindet sie zwei gezeichnete Etagen, erscheint
   sie auf beiden (ROOM1170 Treppe B: (5,0)/(156,97) und (4,3)/(168,106) — dieselbe Stelle,
   zwei Blätter). Liegen beide Enden auf derselben Etage (Treppe A: Bänder 0 und 2, beide
   → 3F), erscheint sie einmal; die zweite Marke fällt in der Dedup-Prüfung
   `(seite, rect, mx, my, wandseite)` weg.
4. **Marke, deren Band zu KEINEM Etagen-Eintrag passt:**
   * Hat die Zone **weniger als zwei** Etagenzeilen, gibt es in ihr keine Umschaltung —
     die Marke bleibt auf dem Blatt ihrer Zone (`blatt_fuer_band` liefert `None`). Das ist
     der Normalfall: nur 3 von 118 Zonen haben überhaupt Etagenzeilen.
   * Hat die Zone Etagenzeilen und liegt das Band **zwischen** ihnen (ROOM1170 Band 2,
     ROOM1060 Bänder 2/6, ROOM4070 Bänder 2/4/6/10), fängt die Nächster-Nachbar-Regel sie ab.
   * **Die Marke wird nie verworfen.** Eine weggelassene Tür ist die Fehlerklasse, die der
     Nutzer schon zweimal gemeldet hat („die Tuer Richtung Ausgang 3F fehlt in der
     Zeichnung", „es fehlt die eingezeichnete Tuer unter der Treppe"). Lieber auf der
     Nachbaretage als nirgends.
5. **Die Kachel-Prüfung läuft auf dem NEUEN Blatt.** `kachel_zeigt_tuer(seite, rect, …)`
   muss *nach* der Umlegung greifen — sonst prüft man die falsche Kachel. Genau das macht
   das 3F-Blatt richtig: dort malt MAP05 die beiden Nischen schon, der Port zeichnet nichts
   darüber; auf ROOF malt MAP06 nichts, und der Port zeichnet dort jetzt auch nichts, weil
   beide Türen gar nicht mehr auf dieses Blatt gehen.
6. **Der alte Kopierschritt entfällt ersatzlos.** Mit Regel 2 legt jede Marke selbst fest,
   auf welches Blatt sie gehört; zusätzliches Kopieren stellt die Doppelung wieder her.

---

## 6. Nachmessung der Regel an den anderen mehrstöckigen Räumen

Es gibt genau drei Zonen mit mehr als einer Etagenzeile: ROOM1170 Zone 1, ROOM1060 Zone 0,
ROOM4070 Zone 0. Alle Änderungen der Markentabelle (Git-Diff gegen HEAD) sind damit erklärt.

### ROOM1060 (Treppenhaus, Bänder 0/2/4/6/8 → Seiten 2 „1F" / 3 „2F" / 4 „3F")

Drei Türdatensätze an **derselben** Weltposition (27100,25400) mit Bändern 8/4/0 und Zielen
ROOM1120 (S. 4) / ROOM10C0 (S. 3) / ROOM1040 (S. 2) — eine Tür je Etage. Acht
Treppendatensätze an nur **zwei** Positionen: Ort A = (25150,22000) trägt die Läufe 8↔6 und
4↔2, Ort B = (21400,22850) die Läufe 6↔4 und 2↔0.

Projektion (`rects(2)[9] = rects(3)[9] = (109,134,16,16)`, `rects(4)[0] = (127,137,16,16)`,
alle uv(168,40)):

| Ort | Seite 2 (1F) | Seite 3 (2F) | Seite 4 (3F) |
|---|---|---|---|
| Treppe A | (119,142) | (119,142) | (137,145) |
| Treppe B | (114,141) | (114,141) | (132,144) |
| Tür | (122,138) → gesnappt (118,138) | (118,138) | (140,141) → (136,141) |

Entfernt wurde `{ 4, 0, 132,144, 4, 5, 255 }` = **Treppe B auf dem 3F-Blatt**. Richtig:
Treppe Bs höchstes Band ist 6, von Band 8 (3F) erreicht man sie nicht. Auf Seite 4 bleibt
Treppe A (137,145) — der Lauf 8↔6 — und die Band-8-Tür. ✓

### ROOM4070 (Bänder 0..12 → Seiten 11 „B4" / 9 „B2" / 8 „B1")

Zwölf Treppendatensätze an zwei Positionen A = (-8800,-24950), B = (-8200,-19300).
Rechtecke `rects(8)[7] = (142,159,24,24)`, `rects(9)[14] = (138,174,24,24)`,
`rects(11)[3] = (158,153,24,24)`, alle uv(16,32).

| Ort | Seite 8 (Band 12) | Seite 9 (Band 8) | Seite 11 (Band 0) |
|---|---|---|---|
| A | (153,176) | (149,191) | (169,170) |
| B | (154,165) | (150,180) | (170,159) |

Entfernt wurde `{ 8, 7, 154,165, 5, 68, 255 }` = **Ort B auf dem Band-12-Blatt**. Richtig:
Ort Bs höchstes Band ist 10; von Band 12 erreicht man nur Ort A, und der bleibt. ✓

### ROOM1170 Zone 1

Entfernt wurden die drei in §3.2 pixelgenau nachgewiesenen Marken. ✓

---

## 7. ⛔ Was die Änderung als Nebenwirkung mitgenommen hat — ROOM10C0

Der Git-Diff enthält zwei Zeilen, die **nicht** aus der Band-Regel folgen:

```
-    {  3,  8,  180,   80, 1,  12, 255 },     ROOM10C0s Tuer nach Osten (Treppenhaus)
-    {  3,  9,  118,  138, 1,   5, 255 },
+    {  3,  9,  118,  138, 1,   5,  12 },     dieselbe Tuer, jetzt mit zid2 = 12
```

**Mechanismus.** Durchgang 2 des Generators verschmilzt einen Durchgang, der in zwei Räumen
je einen Datensatz hat, zu **einer** Marke — aber nur, wenn beide auf demselben Blatt
liegen (`if A['pg'] != B['pg']: continue`). ROOM10C0s Tür zum Treppenhaus liegt auf Seite 3
Rect 8; ROOM1060s Gegenstück (Band 4) lag **vorher** auf Seite 2 Rect 9 (dem 1F-Blatt) —
verschiedene Blätter, keine Verschmelzung, beide Symbole blieben stehen. Seit der Band-Regel
wandert ROOM1060s Band-4-Tür auf **Seite 3** Rect 9, damit sind beide auf Seite 3, und die
Verschmelzung greift.

Welches der beiden Symbole überlebt, entscheidet `_abstand` — „nimm die Position, die dem
Nachbar-Rechteck am nächsten liegt". Gemessen:

* ROOM10C0s Position (180,80) → Abstand zu `rects(3)[9] = (109,134,16,16)`: 56 + 54 = **110**
* ROOM1060s Position (118,138) → Abstand zu `rects(3)[8] = (164,77,24,24)`: 46 + 38 = **84**

ROOM1060 gewinnt. **Folge:** Auf dem 2F-Blatt zeigt ROOM10C0s Grundriss (x164..187,
y77..100) jetzt nur noch zwei Türen (West nach ROOM10D0, Nord nach ROOM1080); die dritte
sitzt 46 px entfernt im 16×16-Kästchen des Treppenhauses. Das Original malt auf Rect (3,8)
kein einziges Symbol (Katalog: 0 Einträge; unabhängig nachgezählt: 0 Innenlinien-Pixel), es
gibt dort also nichts, was den Verlust auffängt.

Die Voraussetzung von `_abstand` — „die beiden Rechtecke teilen sich eine Kante, eine der
Positionen sitzt darauf" — ist hier **nicht erfüllt**: 84 px und 110 px sind beide weit. Das
ist eine plausible Quelle für den nächsten „die Tür fehlt"-Report.

**Vorschlag.** Verschmelzen nur, wenn die beiden Rechtecke sich tatsächlich **berühren oder
überlappen** — reine Geometrieprüfung an der Rechteck-Tabelle @0x80076844, ohne freie Zahl.
Sonst beide Marken stehen lassen, jede in ihrem Rechteck. (Die weichere Variante — eine
Abstandsschranke — bräuchte eine gemessene Zahl: über alle Verschmelzungskandidaten des
Spiels den Abstand auftragen und den Bruch suchen, an dem „geteilte Kante" in „zwei
getrennte Zeichnungen" umschlägt. Solange die nicht gemessen ist, ist die Berührungsprüfung
die belegbare Regel.)

Ein weiterer Punkt an derselben Stelle, unverändert vorhanden: `belegt.add(_i)` /
`belegt.add(_j)` stehen **vor** dem `continue` für verschiedene Blätter. Ein
blattübergreifendes Paar verbraucht also beide Partner, obwohl es nichts tut — ein dritter
Datensatz, der mit einem der beiden hätte paaren können, kommt dann nicht mehr zum Zug.
Nicht durch diesen Fix verursacht, aber im selben Block.

---

## 8. Restposten (gemessen, nicht in diesem Auftrag gelöst)

1. **ROOM1170 hat eine Maßstabszeile — sie deckt nur Zone 0 ab.**
   Zeile 23 @0x80076968 = `{ox=100, oy=206, sx=2280, sy=2268}` (kein `{0,0,1,1}`-Stub).
   Mit der Original-Formel:
   * Zone 0 (Helipad): T0 → (175,104), das liegt in `rects(5)[1] = (148,101,48,56)` ✓.
     Die generierte Marke für dieselbe Tür steht bei (174,101) — 3 px daneben.
   * Zone 1 (Treppenblock): (114..146, 176..197) — **außerhalb aller Rechtecke der Seite 5**.
   * Von 150 SCA-Zellen fallen 50 in `rects(5)[1]`, 100 in gar kein Rechteck der Seite.

   Die ausgelieferte Zeile beschreibt also **nur Zone 0**; für Zone 1 gibt es im Original
   keine Abbildung. Was der Port dort tut (Bbox-Streckung), ist eine Port-Ergänzung ohne
   Vorbild — das erklärt auch die 5,4 px Restabstand der Türmarken zu den gemalten Symbolen
   (§4.2). Wer die Marken auf die gemalten Symbole setzen will, muss für Zone 1 eine eigene
   Abbildung herleiten (aus den zwei gemalten Symbolen + der L-Form der Kachel), nicht aus
   der Bbox. Ein Fit gegen die zwei Symbole selbst wäre wieder selbstbestätigend — als
   unabhängige Prüfgröße bieten sich die SCA-Zellen an (Anteil im Rechteck, Füllgrad).

2. **Band 2 hat kein Blatt.** Steht der Spieler auf dem Zwischenpodest von ROOM1170, zeigt
   die Karte das 3F-Blatt (Gleichstand → untere Etage). Das ist eine Festlegung, keine
   Messung; das Original kennt die Frage nicht. Sie ist wenigstens deterministisch und in
   Generator und Laufzeit gleich.

3. **Seite 2 Rect 9 trägt zwei Räume.** ROOM1060 (zid 5) und ROOM10A0 (zid 11) sind beide
   auf `rects(2)[9]` abgebildet; ihre Marken liegen gemischt in demselben 16×16-Kästchen
   ((113,134) k0 zid11, (114,141) k4 zid5, (114,142)/(114,143) k4 zid11, (118,138) k1 zid5,
   (119,141) k4 zid11, (119,142) k4 zid5). Eigene Untersuchung wert.

4. **Seite 5 hat im Original keine Etagen-Beschriftung.** „ROOF" ist eine Port-Ergänzung
   (`re15_inv_screen.c`, `if (st->map_page == 5)`); die Kachel uv(0,0) von MAP06 zeigt nur
   „POLICE STATION". Vom Nutzer beauftragt, hier nur als Fakt festgehalten.

---

## 9. Werkzeuge / Reproduktion

```bash
# Daten (Zonen, Tueren, Treppen, Projektion) - liest den GENERIERTEN Header
python -c "import sys;sys.path.insert(0,'analysis/karte_audit_0901');from sonde_lib import *;print(RDT[0x1170])"
```

```powershell
# Pixel-Abzug eines Blattes aus dem echten Durchlauf (~2 min, dann abbrechen)
$env:RE15_NO_INTRO="1"; $env:RE15_TITLE_SHOT="t.png"
$env:RE15_DEBUG_JUMP="1170@gp"; $env:RE15_INV_OPEN_AT="60#1170"
$env:RE15_MAP_SHOT_PAGE="5"          # hex; 5 = ROOF, 4 = 3F
$env:RE15_INV_FB_SHOT="roof.bmp"; $env:RE15_INV_FB_SHOT_AT="40"
& re15_port\build_fix\platform\pc\re15_pc.exe
```

**Adressen, die in diesem Dossier belegt sind:**
`FUN_800473f8` @0x800473f8-0x80047644 (Marker + count+2 Prims, sonst nichts) ·
Prim-Bauer `FUN_80046fd8` @0x80046fd8-0x800473e4 (2 feste Sprites + 1 Sprite je Rechteck) ·
Prim-Feld `&DAT_8019a900`, Schrittweite 0x28 @0x800475f8, Anzahl `count+2` @0x800475e0,
`AddPrim` @0x800475d8 / @0x80047608 ·
Maßstabszeile @0x800768b0 + 8·Slot (ROOM1170 = Slot 23 @0x80076968) ·
Rechteck-Tabelle @0x80076840 (count, gelesen @0x800474cc) / @0x80076844 (ptr) ·
Seiten-id-Tabelle @0x80074c4c · Seiten-Setzer @0x8004b568 ·
Tür-Band `pc[4]` = `obj[0x82]`, Interaktions-Gate FUN_8002bd44 @0x8002bf38 ·
Treppen-Handler LAB_80043500 (x @0x80043510-14) / LAB_800435cc (z @0x800435dc-e0),
`count` @0x800435b8 / @0x8004367c ·
AOT-Trefftest FUN_80042b64 @0x80042b68-98 · AOT-Verteiler überspringt `sce == 0`
@0x80042f48-50 · Band aus Y: `-(y / 0x708)` ·
RDT-Offsets ROOM1170: Kollision 0xA80, mainScd 0x11F4, Türen 0x001206 / 0x0012F8 /
0x001338 / 0x00135A, Treppen 0x00137A / 0x00138E / 0x0013A2 / 0x0013B6.
