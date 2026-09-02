# A — Die eingezeichneten Symbole in den Karten-Kacheln (MAP01..MAP0D.PIX)

Untersuchung 2026-08-31. Auftrag: "In ROOM 1130 sieht man das Tuer Symbol oben in der
Karte. Nutze lieber das als die gelben Balken."

**Kernbefund: Die Original-Kacheln enthalten 90 gezeichnete TUERBLAETTER (60 Einzel- +
15 Doppeltueren) und 9 TREPPEN-Symbole. Der Tuer-Befund ist byte-true belegt, der
Treppen-Befund stark, aber nicht positionsbelegt. Der Port malt sie nicht — er malt
daneben eigene gelbe Balken, die im Median 13,0 px neben dem gezeichneten Symbol sitzen.**

---

## 0. Grundlagen (nachgeprueft, nicht uebernommen)

| Sache | Beleg |
|---|---|
| Adresse -> Datei-Offset | `off(a) = a - 0x80010000 + 0x800`, `info/Re1.5/PSX.EXE` (718848 B) |
| Paartabelle | `@0x80076840` (Datei `0x67040`), 14 Seiten `{u16 count, u16 pad, u32 ptr}`, Summe **119 Rechtecke**. Roh: `07 00 00 00 a0 62 07 80 ...` |
| Rechteck-Eintrag | 12 B `{s16 x, s16 y, s16 w, s16 h, u8 u, pad, u8 v, pad}` — bestaetigt am Zeichner `FUN_80046fd8`: `puVar14[0]=x`, `puVar9[0..4]=y,w,h,u,v`, `puVar14 += 6` (u16) = 12 B |
| Sprite-Prim | code `0x64 | 2` = SPRT **semi-transparent**, rgb `0x80,0x80,0x80` neutral, CLUT `GetClut(0x100,0x1f5)` `@0x80046fdc` = VRAM(256,501) = id `0x7d50` |
| Seite -> Textur | CD-id-Tabelle `@0x80074c4c` (Datei `0x6544c`): `0c 00 0d 00 ... 18 00 00 00` -> Seite 0..12 = id 12..24 = **MAP01..MAP0D.PIX**; **Seite 13 -> id 0, keine Textur** (1 Rechteck ohne Bild) |
| Kachel | headerlos, 32768 B = 256x256 4bpp, 128 B/Zeile, **unteres Nibble = linkes Pixel** |
| CLUT-Zeile 21 | `DATA/TEX.TIM`, CLUT-Block (256,480) 32x24, Zeile 21 -> siehe Palette unten |
| Markerformel | `FUN_800473f8`: `x = ((((wx+32000)*10*sx) >> 20) + 5)/10 + ox`, `y = -((((wz+32000)*sy*10) >> 20) + 5)/10 + oy` |
| Skalentabelle | `@0x800768b0` (Datei `0x670b0`), 8 B `{s16 ox, s16 oy, u16 sx, u16 sy}`. **Sie ist laenger als 38 Zeilen** — Index = GLOBALER Raum-Index (`BASES = 0/38/50/65/77/98`), gueltig bis Index ~105, ab 106 steht ASCII-Muell (`0x304B`). **39 Zeilen sind echt kalibriert** (nicht 13): idx 2,3,4,7,21,23,25,26,27,30,32,33,38,50,51,53,54,56,57,58,59,64,65,66,68,74,75,78,80,81,83,87,89,93,94,95,97,101,102. idx 28 (ROOM11C0) hat ox/oy, aber sx=sy=1 (kaputt). |

### Palette (TEX.TIM CLUT-Zeile 21 = VRAM(256,501) = `0x7d50`)

| idx | raw | RGB8 | STP | Verwendung in den Rechtecken |
|---|---|---|---|---|
| 0 | 0x0000 | (0,0,0) | 0 | transparent (ausserhalb des Grundrisses) — 93 372 px |
| 1 | 0x81a4 | (32,104,0) | 1 | gruene Raumfuellung — 107 136 px |
| 2 | 0x00e1 | (8,56,0) | 0 | dunkelgruene Innenlinie (Sonderwaende) — 2 435 px |
| 3 | 0x0081 | (8,32,0) | 0 | ungenutzt |
| **4** | **0x5ad6** | **(176,176,176)** | 0 | **Wand-/Tuer-/Treppenlinie — 18 561 px** |
| 5..9 | 0x56b5..0x1484 | Graustufen | 0 | ungenutzt |
| 10..15 | 0x0db8, 0x02b8, 0x55a8, 0x3505, 0x1476, 0x146d | orange/gelb/blau/rot | 0 | **ungenutzt** |

STOP: Ueber alle 119 Rechteck-Ausschnitte kommen **nur die Indizes 0, 1, 2 und 4** vor.
Es gibt in den Kacheln **kein Gelb, kein Rot, kein Blau**. Die Symbole sind in derselben
Farbe wie die Waende gezeichnet: Index 4 = rgb(176,176,176).

Ausserdem: **Kein einziges Fuellpixel grenzt direkt an Transparenz** (0 Treffer ueber alle
119 Rechtecke). Die Wandlinie ist ueberall geschlossen — **eine Tuer wird NIE als Luecke in
der Wand gezeichnet**, sondern immer als eingezeichneter Stempel nach innen.

---

## 1. Was gefunden wurde

Detektor: Pixel mit Index 4, deren **volle 8er-Nachbarschaft nicht transparent** ist
(= Linie im Rauminneren, nicht Aussenkontur). 8-verbundene Gruppen mit Toleranz 2.
Ergebnis: **92 Gruppen**. Klassifikation gegen eine Maskenbibliothek (jede Maske aus dem
Bild abgelesen, die 8 Orientierungen per D4-Gruppe erzeugt):

| Typ | Anzahl | Aufschluesselung |
|---|---|---|
| TUER (einfach) | 60 | 5x5 Standard 40, 4x4 klein 12, 4x5 3, 4x4 gestutzt 3, 3x4 1, 5x4 an Schraegwand 1 |
| TUER2 (doppelt) | 15 | = 30 Tuerblaetter |
| TREPPE (Stufenband) | 6 | |
| TREPPENHAUS (15x15-Schacht) | 3 | |
| SONST | 4 | 3x Trennlinie 1x12 (p12r1/2/3), 1x Sprossenband 2x22 (p12r5) |
| ARTEFAKT | 4 | 1-2 px an Schraegwaenden (p9r8/r9), kein Symbol |
| **Summe** | **92** | 100 % klassifiziert, kein Rest |

Je Seite: 0: 5T+1D | 1: 8T+1D | 2: 8T+1D | 3: 3T+1D | **4: 5T+1D** | **5: keine** |
6: 4T+2D+5Tr | 7: 14T+1D+1Tr | 8: 5T+2D+1Th | 9: 4T+3D+1Th | 10: 1D | 11: 1T+1D+1Th |
12: 3T+4S | 13: keine Textur.
**53 der 118 bebilderten Rechtecke tragen gar kein Symbol.**

---

## 2. BEWEIS, dass der 5x5-Stempel eine TUER ist

Nicht "sieht so aus": gemessen mit der **Original-Formel** `FUN_800473f8` + der
**Original-Skalentabelle** `@0x800768b0`, angewandt auf die **Tuer-Datensaetze der RDTs**
(SCD-Opcode `0x3b`/`0x68`, Mitte des Tuer-Rechtecks), fuer alle 39 Raeume mit echter
Skalenzeile und Karten-Seite:

```
TUEREN  n=107    Median  3,0 px   <=3px 52,3 %   <=1px 26,2 %
ZUFALL  n=11400  Median 19,4 px   <=3px  6,1 %   (gleichverteilte Punkte im selben Raum)
```

Nur STAGE1, wo die Rechteck-Zuordnung am saubersten ist:
```
n=38  Median 2,1 px  <=3px 68,4 %   (ohne ROOM11A0: n=33, Median 2,0, <=3px 78,8 %)
```

Einzelne Volltreffer (Abstand 0 px = die Tuer projiziert IN den Glyph hinein):
- `ROOM1150` einzige Tuer -> Screen (146,85) -> Glyph p4**r4** @(145..149,82..86): **d = 0,0 px**.
  Das ist genau das Symbol, das der Nutzer "oben in ROOM 1130" sieht.
- `ROOM1210` hat 6 Tueren -> 6 **verschiedene** Glyphen, Abstaende 0/2/1/2/1/1 px.
- `ROOM11B0` -> `ROOM11C0`: d = 0,0 px. `ROOM1190` -> Doppel-Glyph p0r3 (5x9): d = 0,0 px.

**Zweiter, unabhaengiger Beweis (Rueckwaerts-Fit).** ROOM1130 hat *keine* Skalenzeile
(Index 19 = Stub `{0,0,1,1}`). Sucht man die Zeile so, dass die 4 Tueren des Raums auf
4 verschiedene Tuer-Glyphen der Seite 4 fallen, gibt es genau eine konsistente Loesung:

```
ROOM1130 (ABGELEITET, NICHT aus dem ROM): ox=90  oy=189  sx=2299  sy=2223
   Tuer->ROOM1140 (  -300,-13900) -> (160,151)  Glyph p4r6 (Doppeltuer)  d=3,0
   Tuer->ROOM1120 ( -3050, -2150) -> (153,126)  Glyph p4r5               d=3,0
   Tuer->ROOM1150 ( -6650, 16350) -> (146, 86)  Glyph p4r4               d=2,2
   Tuer->ROOM1170 (  2700, 14650) -> (166, 90)  Glyph p4r3               d=2,2
```
sx/sy liegen mitten im Wertebereich der echten Zeilen (Nachbar ROOM1150: 2296/2312).
Das bestaetigt gleichzeitig **ROOM1130 -> Seite 4, Rechteck 4**.

**Dritter Beweis (die Kachel-Doublette).** Seite 4 r3 (MAP05) und Seite 5 r0 (MAP06) sind
dieselbe Zeichnung, uv(192,16) 48x24, und unterscheiden sich in genau **22 von 1152 px**,
alle Index 4 -> 1. Die 22 Pixel zerfallen in genau zwei Gruppen, und beide sind **exakte**
D4-Instanzen der Tuer-Grundmaske:
- uv x[203..207] y[17..21] — Maske `X...X|X...X|X...X|.X..X|..XXX`, Wand N
- uv x[202..206] y[32..36] — Maske `XXX..|X..X.|X...X|X...X|X...X`, Wand S

Der Zeichner hat auf MAP06 dieselbe Grundflaeche ohne die zwei Tuersymbole nochmal
abgelegt. **Seite 5 traegt kein einziges Symbol** — dort ist nichts zu uebernehmen.

---

## 3. Das exakte Pixelmuster der TUER

Ein Stempel ist **5 px lang (entlang der Wand) x 5 px tief (ins Rauminnere)**; die
**Wandlinie selbst schliesst ihn als 6. Reihe** — die offene Seite des Stempels ist
immer die Wandseite. Alles in **Palettenindex 4 = rgb(176,176,176)**, also exakt die
Wandfarbe.

Aufbau in wandlokalen Koordinaten (a = entlang der Wand 0..4, d = Tiefe 1..5, Wand bei d=0):

```
Langseite   a=0 (oder a=4), d=1..5      5 px
Kappe       d=5, a=0..2                 3 px
Fase        (a=3, d=4)                  1 px   <- die 45-Grad-Ecke
Kurzseite   a=4 (bzw. a=0), d=1..3      3 px
                                       11 px gesamt
```

### Die 8 Orientierungen, wie sie tatsaechlich vorkommen (# = Index 4)

**SENKRECHTE Wand** (Wand links = W, 14x; Wand rechts = E, 14x):

```
   Wand W (Wand = Spalte LINKS daneben)        Wand E (Wand = Spalte RECHTS daneben)
      #####  (8x)     ###..  (6x)                 #####  (10x)    ..###  (4x)
      ....#           ...#.                       #....           .#...
      ....#           ....#                       #....           #....
      ...#.           ....#                       .#...           #....
      ###..           #####                       ..###           #####
```

**WAAGERECHTE Wand** (Wand oben = N, 5x; Wand unten = S, 7x):

```
   Wand N (Wand = Zeile DARUEBER)               Wand S (Wand = Zeile DARUNTER)
      #...#  (4x)     #...#  (1x)                  ###..  (5x)    ..###  (2x)
      #...#           #...#                        #..#.          .#..#
      #...#           #...#                        #...#          #...#
      .#..#           #..#.                        #...#          #...#
      ..###           ###..                        #...#          #...#
```

**Unterschied waagerecht <-> senkrecht:** identische Maske, um 90 Grad gedreht. Die offene
Kante (die Kante, auf der nur die beiden Endpunkte liegen) zeigt immer zur Wand. Die
zweite Variante je Wandseite ist die **Spiegelung** — vermutlich die Anschlagseite
(welche Haelfte die 5 px lange Langseite traegt). Eine Korrelation der Spiegelung mit
Daten aus dem RDT ist **nicht** geprueft -> offen.

### Doppeltuer (15x, = 30 Blaetter)

Zwei Stempel, an der Kappenkante gespiegelt aneinandergesetzt: 5 tief x 9 lang.

```
   Wand W:   #####          Wand S:   ###...###
             ....#                    #..#.#..#
             ....#                    #...#...#
             ...#.                    #...#...#
             ###..                    #...#...#
             ...#.
             ....#
             ....#
             #####
```

### Kleinere Varianten (fuer enge Raeume)

4x4 (8 px, 12x), 4x5 (9 px, 3x), 4x4 gestutzt (9 px, 3x), 3x4 (7 px, 1x),
5x4 an einer Schraegwand (9 px, 1x). Alle Masken stehen in `masken.txt` und als
Bitmaske im CSV.

---

## 4. ROOM1130 = Seite 4, Rechteck 4 — was die Kachel wirklich traegt

Rect `(144,80) 32x80`, uv `(0,32)`, Textur `MAP05.PIX`. Der Grundriss ist L-foermig
(3 Bloecke). **Genau EIN Symbol:**

| Feld | Wert |
|---|---|
| Typ | TUER, Maske "5x5 Standard" |
| Lage im Rechteck | (1,2), 5x5 px |
| Screen | (145,82)..(149,86) |
| uv in MAP05.PIX | (1,34)..(5,38) |
| Wand | **W** (senkrecht, die linke Aussenwand des Raums) |
| Ausrichtung | **senkrecht** |
| Maske | `XXXXX / ....X / ....X / ...X. / XXX..` |
| Bedeutung | die Tuer **ROOM1130 <-> ROOM1150** (Irons' Buero). Original-Projektion von ROOM1150s Tuer: Screen (146,85) -> **d = 0,0 px** |

ASCII des Rechtecks, Zeilen 0..9 (`+` = Index 4, `#` = Index 1, `.` = transparent):

```
      01234567890123456789012345678901
  0   ++++++++++++++++++++++++++......
  1   +########################+......
  2   ++++++###################+......   <- Kappe der Tuer
  3   +####+###################+......
  4   +####+###################+......
  5   +###+####################+......   <- Fase
  6   ++++#####################+......   <- Kurzseite
  7   +########################+......
  8   +########################+......
  9   +#######++++++++++++++++++......
```

Der Nutzer sieht es "oben" — es liegt 2 px unter der Oberkante des Rechtecks.

**Die uebrigen drei Tueren von ROOM1130 sind auf den NACHBAR-Kacheln gezeichnet**
(->1170 auf r3, ->1120 auf r5, ->1140 auf r6 als Doppeltuer). Ein Tuerdurchgang wird
**einmal** gezeichnet, nicht auf beiden Seiten. Deshalb ist z. B. ROOM1150s eigenes
Rechteck (p4r2, 32x40) voellig leer, obwohl der Raum eine Tuer hat.

**Der Port zeichnet in p4r4 vier gelbe Balken** (`s_map_marks`: (144,87) kind 2,
(152,125) kind 2, (160,150) kind 2, (168,89) kind 0). Der naechste liegt 5 px unter dem
gezeichneten Symbol, die anderen drei stehen dort, wo die Kachel nichts zeigt.

---

## 5. TREPPEN — die Behauptung "kein einziges Rechteck traegt ein Treppensymbol" ist FALSCH

Es gibt **9** Treppen-Zeichnungen, alle im gleichen Zeichenstil (parallele Stufenlinien,
Index 4):

**a) Treppenhaus-Schacht 15x15, 87 px — 3x** in einem 24x24-Rechteck, uv(16,32), auf
**drei verschiedenen Seiten**: p8r7 (MAP09), p9r14 (MAP0A), p11r3 (MAP0C). Dieselbe
Zeichnung auf drei Etagen-Seiten = derselbe Treppenschacht, je Etage einmal.

```
....+.+.+.+....      oben/unten je 4 senkrechte Stufenlinien (Abstand 2),
....+.+.+.+....      links/rechts je 2 waagerechte Stufenlinien,
....+.+.+.+....      in der Mitte das offene Auge des Schachts
....+.+.+.+....
+++++++++++++++
....+.....+....
+++++.....+++++
....+.....+....
+++++.....+++++
....+.....+....
+++++++.+++++++
....+.+.+.+....
....+.+.+.+....
....+.+.+.+....
....+.+.+.+....
```

p8r7 ist dem Raum **ROOM4070** zugeordnet — dem Raum mit **12 Band-Wechsel-Zonen**
(`Aot_set` Typ 12/13), mehr als jeder andere Raum im Spiel. Die 12 Zonen fallen in
**zwei** Cluster; die Zeichnung hat **zwei** Stufenbloecke (oben/unten).

**b) Stufenband 7..8 px breit, 2 Teilerlinien im Abstand 3, mit Abschlusslinie — 6x**:
p6r0 (3x), p6r6, p6r9, sowie p7r8 als 3-sprossige Leiter in einem 8x16-Raum.

```
   ++++++++        ..+..+..
   ..+..+..        ..+..+..
   ..+..+..   und  ..+..+..
   ..+..+..        ..+..+..
   ..+..+..        ++++++++
```

p6r0 = ROOM2040 (2 Band-Wechsel-Zonen), p6r9 = ROOM2080 (1 Zone), p7r8 = ROOM30B0.

**c) 1x Sprossenband 2x22** in p12r5 = ROOM6010 (8 Band-Wechsel-Zonen): eine 2 px
breite Rinne entlang der Wand mit Querstegen alle 10 px.

STOP — **Konfidenz.** Die Zuordnung "Stufenband = Treppe" ist **nicht byte-true belegt**,
weil **kein** Raum mit Band-Wechsel-Zonen gleichzeitig eine echte Skalenzeile hat
(ROOM2040/2080/4070/6010 sind alle Stubs). Von den 39 kalibrierten Raeumen haben nur
ROOM3010/3040 Treppen-AOTs, und deren Projektion liegt 69..183 px neben dem einzigen
Treppen-Symbol der Seite 7 — die Rechteck-Zuordnung dieser beiden Raeume ist also falsch
oder das Symbol gehoert einem anderen Raum. Belegt ist nur: der Zeichenstil (parallele
Stufen, kein einzelner Klecks), die Wiederverwendung derselben Schacht-Kachel auf drei
Etagen-Seiten, und die Haeufung bei ROOM4070.
**Naechster Weg:** ROOM4070/2040/2080 per Rueckwaerts-Fit (Verfahren aus Abschnitt 2)
ueber ihre Tueren kalibrieren, dann die Treppen-AOTs projizieren.

**Nicht-Symbole:** die 3 senkrechten 1x12-Linien in p12r1/2/3 sind Raumteiler (dieselbe
Kachel uv(128,64) 3x benutzt); die 4 Ein- und Zwei-Pixel-Reste in p9r8/r9 sind
Treppchen einer schraegen Aussenwand.

---

## 6. EMPFEHLUNG fuer den Port

**Der Port soll fuer eine Tuer, die die Kachel schon zeigt, GAR NICHTS zeichnen.**
Begruendung, nicht Geschmack:

1. Das Symbol ist bereits Teil des SPRT, den der Port ohnehin zeichnet
   (`re15_inv_screen.c`, Paartabellen-Schleife um Zeile 1562). Es kommt automatisch mit,
   inklusive der Zustands-Einfaerbung.
2. Die gelben Balken haben **keinen** `@0x...`-Beleg. rgb(240,200,64) existiert in der
   Karten-CLUT nicht (Zeile 21 hat 0/1/2/4 in Benutzung, kein Gelb).
3. Sie sitzen falsch: gemessen ueber alle 144 Tuermarken des Ports —
   **Median 13,0 px** neben dem naechsten gezeichneten Tuersymbol, **84 von 144 weiter
   als 10 px** weg, nur 33 innerhalb von 3 px. Ursache ist nicht die Tuer-Datenquelle
   (die ist richtig), sondern die **Projektion**: `gen_map_zones.py` streckt die
   Zonen-Bbox linear ins Rechteck, das Original benutzt `FUN_800473f8` + die
   Skalentabelle. Mit der Original-Formel liegt derselbe Tuer-Datensatz im Median
   **2,1 px** (STAGE1) am gezeichneten Symbol.

**Wenn trotzdem markiert werden soll** (53 Rechtecke tragen gar kein Symbol, und viele
Tueren sind ungezeichnet), dann mit dem Original-Stempel, nicht mit Balken:

```
Farbe:    Palettenindex 4 der Karten-CLUT = 0x5ad6 = rgb(176,176,176)
Groesse:  5 x 5 px, offene Kante an der Wand
Zeichnen: deckend (RE15_INV_OP_FILL), nicht additiv

SENKRECHTE Wand, Wand links (W):        SENKRECHTE Wand, Wand rechts (E):
   #####                                   #####
   ....#                                   #....
   ....#                                   #....
   ...#.                                   .#...
   ###..                                   ..###

WAAGERECHTE Wand, Wand oben (N):        WAAGERECHTE Wand, Wand unten (S):
   #...#                                   ###..
   #...#                                   #..#.
   #...#                                   #...#
   .#..#                                   #...#
   ..###                                   #...#

Doppeltuer: denselben Stempel an der Kappenkante gespiegelt anhaengen -> 5 x 9.
```

**Reihenfolge der Umsetzung, damit nichts doppelt gemalt wird:**

1. `symbolkatalog.csv` als Datenquelle einlesen: dort steht je `(seite, rect)`, wo die
   Kachel bereits ein Symbol traegt (`screen_x`, `screen_y`, `px_w`, `px_h`, `typ`).
2. Eine Port-Tuermarke unterdruecken, wenn ihr Mittelpunkt hoechstens 4 px von einem
   `typ = TUER/TUER2`-Eintrag derselben Seite entfernt ist.
3. Fuer den Rest den 5x5-Stempel oben verwenden; die Wandseite steckt schon im
   bestehenden `kind` (0 = waagerechte Wand -> N/S-Form, 2 = senkrechte Wand -> W/E-Form).
4. Vorher die **Projektion** auf `FUN_800473f8` + `@0x800768b0` umstellen; wo die Zeile
   ein Stub ist, die Zeile per Rueckwaerts-Fit an den gezeichneten Tuersymbolen
   bestimmen (Verfahren in Abschnitt 2, fuer ROOM1130 vorgerechnet). Ohne diesen Schritt
   sitzen auch die neuen Stempel falsch.

**Treppen:** solange Abschnitt 5 nicht byte-true geschlossen ist, keine eigene
Treppen-Grafik erfinden. Die 9 gezeichneten Treppen kommen ohnehin mit der Kachel.

---

## 7. Gelieferte Dateien

| Pfad | Inhalt |
|---|---|
| `analysis/kartensymbole/symbolkatalog.csv` | **die Lieferung**: 92 Zeilen, je Symbol Seite/PIX/Rect/uv/Screen/Position im Rechteck/Typ/Maskenname/Wandseite/Ausrichtung/Palettenindex/zugeordnete Raeume/Bitmaske |
| `analysis/kartensymbole/symbolkatalog.json` | dasselbe + Quellenangaben + volle 16er-Palette |
| `analysis/kartensymbole/masken.txt` | alle vorkommenden Masken als ASCII-Raster, nach Typ und Orientierung, mit Fundstellen |
| `analysis/kartensymbole/rechtecke/*.png` | 118 Ausschnitte, 8x vergroessert, Palette 0x7d50, gefundene Symbole farbig umrandet (gelb = Tuer, orange = Doppeltuer, cyan = Treppe) |
| `analysis/kartensymbole/seiten/seite00..12.png` | Uebersicht je Seite: alle Rechtecke an ihrer Screen-Position, mit rN-Beschriftung und markierten Symbolen |
| `analysis/kartensymbole/kacheln/*.png` | die 13 vollen 256x256-Kacheln, 3x vergroessert |
