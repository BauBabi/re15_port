# GEGENPRUEFUNG B4 (Rolle: Skeptiker) — "Seite 4 Rect 4 gehoert ROOM1130"

Datum 2026-08-31. Alles hier ist selbst nachgerechnet (EXE-Bytes, RDT-Bytes, PIX-Pixel,
eigener MIPS-Dekoder). Keine Zahl aus `B_kartenkacheln.md` uebernommen.
Sonden: `<scratchpad>/pruefB4/{dis.py,jt.py,tabs.py,rdt.py,cells.py,doors.py,proj.py,
plan.py,tiles.py,comp.py,brute.py,brute2.py,gen_run.py,gen_dbg2.py}`.

`off(a) = a - 0x80010000 + 0x800` — bestaetigt: `PSX.EXE[0x18] = 00 00 01 80` (dest 0x80010000),
`[0x1c] = 00 f0 0a 00` (0xaf000).

## ERGEBNIS

**Die Behauptung ist im Kern RICHTIG — aber die im Bericht genannte BEGRUENDUNG traegt sie nicht.**
Ich habe die Zuordnung mit einer voellig anderen, byte-harten Kette unabhaengig bewiesen
(§3) und musste dabei drei Fehler in der Beweisfuehrung des Berichts korrigieren (§4).

---

## 1. Was ich bestaetigen konnte (nachgerechnet)

| Behauptung | Status | Mein Beleg |
|---|---|---|
| Seite 4 Rect 4 = (144,80) 32x80 uv(0,32) | OK | `@0x80076498` (Datei 0x66c98) = `90 00 50 00 20 00 50 00 00 00 20 00` |
| ROOM1130 gehoert auf Seite 4 | OK | Sprungtabelle `@0x8001103c[19]` (Datei 0x1888) = `10 b7 04 80` -> `0x8004b710`; von dort Fall-Through ueber `0x8004b724`, `0x8004b738` nach `0x8004b74c`, dort `ori v0,zero,0x4 @0x8004b758` -> `sb v0,0x800b260e @0x8004b88c` |
| ROOM1130-Kollisions-Bbox = 14650 x 38150 | OK | ROOM1130.RDT collision @0x000598, counts (11,11,11,11,11); die 5 Gruppen sind byte-identische Kopien; x[-8650,6000] z[-18400,19750] |
| `0x1130` fehlt in `re15_map_zones.h` | OK | ganze Datei gelesen; Seite-4-Eintraege sind nur 1120->r5, 1140#0->r6, 1140#1->r3, 1150->r2, 1160->r1 |
| Rect 4 (und Rect 0) sind unbelegt | OK | s.o. |
| "echte Luecke" | OK | s.u. §5 |

Kontrolle: Ich habe den eingecheckten Generator (`git show HEAD:...gen_map_zones.py`,
Commit `cc6fa561`) mit umgebogenem Ausgabepfad laufen lassen — das Ergebnis ist
diff-identisch mit dem eingecheckten `re15_map_zones.h`. Der Header im Baum ist also
wirklich der Output dieses Generators (der Arbeitsbaum-Stand von `gen_map_zones.py` ist
inzwischen geaendert, der Header aber nicht).

---

## 2. WARUM der Generator ROOM1130 auslaesst — er ueberspringt ihn NICHT

Der Bericht laesst diese Frage offen. Antwort, gemessen:

* `0x1130` steht in `re15_port/include/re15_room_list.h:9`.
* `read_rdt(0x1130)` liefert 55 SCA-Zellen, `zones_of` genau eine Zone.
* Der Generator meldet fuer Seite 4: **`5/6 Zonen zugeordnet (7 Rects), Kosten 36.2`** —
  die 6. Zone IST ROOM1130, sie bekommt `None`.

Warum `None`? Die Kostenfunktion bestraft eine unbelegte Zone pauschal mit **30.0**.
Belegt man ROOM1130 mit r4, kosten die *lokalen* Terme fast nichts
(`aspect_pen = 0.041` — der mit Abstand beste Wert der Seite; Flaechen-Rang-Differenz 0),
aber es schalten sich sechs Tuer-Graph-Kanten ein:

```
0x1120#0(r5)@(124.4,149.7) -> 0x1130#0(r4)@(158.1,125.9)   41.2 px -> 40.0 (gekappt)
0x1130#0(r4)@(156.2,125.9) -> 0x1120#0(r5)@(126.1,149.7)   38.3 px -> 38.3
0x1130#0(r4)@(162.2,150.6) -> 0x1140#0(r6)@(166.2,149.7)    4.0 px
0x1140#0(r6)@(164.1,150.0) -> 0x1130#0(r4)@(160.1,150.7)    4.1 px
0x1130#0(r4)@(148.4, 87.1) -> 0x1150#0(r2)@(145.7, 85.2)    3.3 px
0x1150#0(r2)@(148.2, 84.2) -> 0x1130#0(r4)@(150.0, 87.1)    3.4 px
SUMME 93.1   =>  Gesamtkosten 36.23 -> 99.71 (+63.47)
```

**Vier der sechs Kanten sind ausgezeichnet (3.3 / 3.4 / 4.0 / 4.1 px). Die ganzen 78.3 px
Schaden kommen aus dem EINEN Paar mit ROOM1120 — und ROOM1120 sitzt auf r5.**
ROOM1130 ist der Knoten mit den meisten Tueren der Seite (4 Stueck) und wird vom Modell
genau dafuer bestraft: einen schlecht platzierten NACHBARN loest der Optimierer, indem er
den gut passenden HUB wegwirft. Das ist ein Defekt der Kostenfunktion, kein Argument
gegen r4.

Gegenprobe (vollstaendige Suche ueber alle 7*6*5*4*3*2 = 5040 Zuordnungen der 6 Seite-4-Zonen,
Score = reine Summe der Tuer-Graph-Abstaende, ohne Flaechen-/Seitenverhaeltnis-Terme):

```
Global bestes Layout:  1120->r3  1130->r4  1140#0->r6  1140#1->r0  1150->r2  1160->r1   Summe 52.2
```

Mit ROOM1150 auf seinem original-belegten r2 festgenagelt (§3b):

```
1130 -> r4 :  52.2      <- Optimum
1130 -> r0 : 138.8
1130 -> r1 : 165.9
1130 -> r3/r5/r6 : schlechter
```

Nebenbefund: Der Tuer-Graph will ROOM1120 auf r3, nicht auf r5. Das ist die eigentliche
Fehlstelle der Seite; sie ist die Ursache dafuer, dass 1130 herausfaellt.

---

## 3. Mein eigener Beweis, dass r4 = ROOM1130 ist (unabhaengig vom Groessen-Lineal)

### 3a. ROOM1150 hat GENAU EINE Tuer, und die geht nach ROOM1130

`ROOM1150.RDT` mainScd @0x000d5c, Record @Datei **0x000d5e** (op 0x3b, 32 B):

```
3b 00 02 31 00 00 | 60be c8ce dc05 9808 | f4e8 0000 de3f | 0000 00 13 06 ...
                     rx=-16800 rz=-12600 rw=1500 rd=2200   nx=-5900 ny=0 nz=16350   stg=0 room=0x13
                                                                                    -> ROOM1130
```

Roh-Scan aller 0x3b-Bytes im SCD-Bereich (0x000d5c..0x014460): genau ein plausibler
Record (die 4 weiteren Treffer liegen jenseits von subScd-Ende 0x12f0 in den Binaerbloecken).
Und ueber alle 240 RDTs des Spiels hat nur ROOM1130/ROOM1131 eine Tuer zurueck nach ROOM1150.
=> Auf der Karte gibt es nur eine einzige Tuer an ROOM1150, und dahinter liegt ROOM1130.

### 3b. Die Original-Skalenzeile von ROOM1150 ist kalibriert und stimmt

`@0x80076958` (Datei 0x67158) = `6f 00 82 00 f8 08 08 09` -> ox=111 oy=130 sx=2296 sy=2312.
ROOM1150-SCA-Bbox (alle 18 Zellen) x[-28658,-14330] z[-27884,-9560] durch FUN_800473f8
(instruktionsgetreu nachgebaut: `@0x8004741c` lw X / `@0x80047428` +32000 / `@0x8004745c` sra 20 /
`@0x80047460` +5 / `@0x80047464` mult 0x66666667 = /10 / `@0x800474f4` +ox; y ebenso mit
`@0x800474b0 subu v1,zero,v1`):

```
-> Schirm x[118,150] y[81,121]    vs. Seite-4-r2 (120,80) 32x40 = x[120,152] y[80,120]
```

<= 2 px auf allen vier Kanten. **ROOM1150 -> r2 ist original-belegt.** (Mein Nachbau der
Formel ist damit zugleich validiert.)

### 3c. DIE TUERMARKE IN DER PIX SITZT AUF DEM PIXEL

Dieselbe Original-Formel auf die Ecken des Tuer-Rechtecks von 3a:

```
(-16800,-12600) -> Schirm (144, 87)
(-15300,-10400) -> Schirm (148, 82)      => Tuer liegt bei Schirm x[144,148] y[82,87]
```

MAP05.PIX (Seite 4 -> id 16, id-Tabelle `@0x80074c4c`) zeichnet dort — in Palettenindex 4,
der Wand-/Tuerfarbe — eine Tuermarke:

```
Composite Seite 4, Ziffer = Rect das den Pixel malt, Buchstabe = dessen Fuellung
        144.....149
  y=82  cccccc444444eeeee...     c = Fuellung r2 (ROOM1150)
  y=83  cccccc4eeee4eeeee...     4 = Wandlinie, hier aus dem Tile von RECT 4
  y=84  cccccc4eeee4eeeee...     e = Fuellung r4
  y=85  cccccc4eee4eeeeee...
  y=86  cccccc4444eeeeeee...
```

Marke = Schirm **x[144,149] y[82,86]**, Tuer laut Original-Formel = **x[144,148] y[82,87]**.
**<= 1 px auf jeder Kante.** Die Pixel liegen physisch im Tile von Rect 4 (uv(0,32),
Tile-lokal x0..5, y2..6); das Tile von r2 ist nur 26 px breit und endet bei Schirm x=145.

=> Das Rechteck hinter ROOM1150s einziger Tuer ist Rect 4. ROOM1150s einzige Tuer geht nach
ROOM1130. **Rect 4 = ROOM1130.** Beteiligt sind ausschliesslich Original-Daten: Skalenzeile
`@0x80076958`, Formel `FUN_800473f8`, Tuer-Record `ROOM1150.RDT@0x000d5e`, MAP05.PIX-Pixel.

### 3d. Zusaetzlich: die GESTALT stimmt (der Bericht hat das nicht geprueft)

ROOM1130s 11 eindeutige SCA-Wandquader im Massstab 2296/2312 geplottet, freie Flaeche =
begehbar (z gespiegelt):

```
Zeilen  5..10  frei x 5..14 und 17..26   breiter Raum oben
Zeilen 11..34  frei x 5.. 9              schmaler Flur LINKS
Zeilen 35..40  frei x 5..17              Aufweitung / Knick
Zeilen 41..78  frei x13..17              schmaler Flur RECHTS
```

Das Tile von r4 (uv(0,32) 32x80), gemalte Flaeche:

```
Zeilen  0.. 9  x 0..25   breiter Raum oben (mit der Tuermarke aus 3c oben links)
Zeilen 10..29  x 1.. 7   schmaler Flur LINKS
Zeilen 30..38  x 1..15   Aufweitung / Knick
Zeilen 39..78  x 9..15   schmaler Flur RECHTS
```

Identische Topologie, gleiche Knickrichtung, gleiche Flurlaengen (24 vs 20 bzw. 38 vs 38
Zeilen). Kein anderes Tile der Seite und kein anderer Raum der Seite hat diese S-Form
(r0/r1/r2 = einfache Kaesten, r3 = L, r5 = Block+Schacht, r6 = Schacht+Grossraum;
1120/1140/1150/1160 s. Plots).

---

## 4. Was am Bericht NICHT traegt (drei Korrekturen)

### 4a. "sx/sy ~ 2280..2300 ist der feste Weltmassstab" — FALSCH

Tabelle `@0x800768b0` selbst gelesen. Die 13 kalibrierten sx-Werte sind
2287, 2428, 2080, 2229, 2296, 2280, 1920, **1776**, 2272, 2304, **3168**, 2496, 2340.
Spanne **1776 (`@0x80076984`, ROOM11A0) bis 3168 (`@0x800769b4`, ROOM1200) = Faktor 1,78**.
Nur 3 von 13 liegen im behaupteten Fenster [2280,2300]. Es gibt keinen festen Massstab.

Folge fuer B4: ROOM1130s eigene Zeile (idx 19 `@0x80076948`) ist der Stub `00 00 00 00 01 00 01 00`.
Die "32,0 x 83,3 px" entstehen aus einem geliehenen Divisor 458. Unter den tatsaechlich
belegten Extremwerten waere ROOM1130 24,8 x 64,6 px (sx=1776) oder 44,3 x 115,3 px (sx=3168).
**"Delta 3 px" ist eine Scheingenauigkeit, kein Mass.**

### 4b. "Rectgroesse 32x80" ist die Blit-Zelle, nicht der gezeichnete Raum

Gemessen in MAP05.PIX: r4 hat gezeichneten Inhalt nur in 26 x 79 Pixeln (Spalten 26..31
und Zeile 79 sind Palettenindex 0 = transparent). Beim original-belegten Paar
ROOM1150/r2 genauso: Rect 32x40, gezeichnet 26x40. Ein Welt-Bbox-Vergleich gegen die
gepolsterte Zellgroesse vergleicht gegen die falsche Zahl.

### 4c. "der schaerfste Treffer der ganzen Seite" — FALSCH, und intern widerspruechlich

Der Bericht misst in §7 mit "Welt-Bbox (Gruppe 0)". ROOM1150 ist der einzige Seite-4-Raum,
dessen 5 SCA-Gruppen KEINE identischen Kopien sind (counts 4/4/5/5/0, Gruppe 0 = 14292x17694,
alle = 14328x18324). Der Bericht setzt in §7 deshalb 31,2 x 38,6 px / "Delta 2" an —
waehrend er in §5a fuer denselben Raum die VOLLE Bbox benutzt (dort kommen die 32x40 heraus,
die r2 exakt treffen). Konsistent gemessen (volle Bbox, Divisor 458):

| Raum | Bbox | px | Rect | Delta |
|---|---|---|---|---|
| **ROOM1150** | 14328 x 18324 | 31,3 x 40,0 | r2 32x40 | **0,7** <- schaerfster Treffer |
| ROOM1130 | 14650 x 38150 | 32,0 x 83,3 | r4 32x80 | 3,3 |
| ROOM1140#0 | 27300 x 27450 | 59,6 x 59,9 | r6 56x56 | 7,5 |
| ROOM1120 | 19200 x 21000 | 41,9 x 45,9 | r5 40x40 | 7,8 |

ROOM1130->r4 ist damit der zweitbeste Groessentreffer, nicht der beste.

---

## 5. Fazit

* **refuted = false.** Die Sachaussagen (r4 = ROOM1130 / 0x1130 fehlt / r4 unbelegt /
  echte Luecke) sind alle wahr; ich habe sie unabhaengig bestaetigt.
* Der ANGEBOTENE Beleg ("Delta 3 px am festen Weltmassstab, schaerfster Treffer der Seite")
  haelt nicht: der Massstab ist nicht fest (Faktor 1,78), ROOM1130s Skalenzeile ist ein Stub,
  32x80 ist die gepolsterte Blit-Zelle, und der Superlativ ist durch eine inkonsistente
  Bbox-Wahl entstanden. Dieser Satz gehoert im Bericht ersetzt.
* Der belastbare Beleg ist §3c: Tuermarke in MAP05.PIX bei Schirm x[144,149] y[82,86] gegen
  ROOM1150s einzige Tuer bei x[144,148] y[82,87] aus der Original-Formel — <= 1 px, und die
  Marke gehoert zum Tile von Rect 4. Dazu die Gestalt (§3d) und die vollstaendige
  Tuer-Graph-Suche (§2).
* Der Grund fuer die Luecke ist NICHT, dass der Generator ROOM1130 uebergeht, sondern dass
  seine Kostenfunktion einen gut passenden, gut vernetzten Raum wegwirft, weil ein NACHBAR
  (ROOM1120 -> r5) falsch sitzt. Wer nur `0x1130 -> Seite 4, r4` nachtraegt, laesst die
  Ursache stehen; die Tuer-Kanten 1120<->1130 bleiben dann bei 41,2 / 38,3 px.
