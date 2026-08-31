# GEGENPRUEFUNG B1 — Rolle: ALTERNATIV-ERKLAERER

Geprueft: **[B1] Seite 5 Rect 0 (140,80) 48x24 uv(192,16) IST der zweite Bereich von ROOM1170 —
und zwar VOLLSTAENDIG (linker Block = Band 0 + Band 2, unterer Arm = Band 4); comp1 = 43,9 x 25,7 px
gegen Rect 48x24 = Delta 5,9 px, bester Treffer aller 119 Rechtecke des Spiels.**

Alle Zahlen unten sind SELBST gemessen (eigene Skripte im Scratchpad `B1/`:
`tab.py, mipsdis.py, pagemap.py, lib.py, p4.py, law.py, rooms.py, shapefit.py, ascii.py,
walk.py, walk2.py, marks.py, tiles.py, tiles2.py, doors.py, dupes.py, assign.py, calib.py`).
Adresse -> Datei-Offset: `off(a) = a - 0x80010000 + 0x800`.

## ERGEBNIS IN EINEM SATZ

**Die ZUORDNUNG haelt** — ich habe keine gleich gute Alternative gefunden, und ein
*unabhaengiges, an der bewiesenen Paarung geeichtes* Verfahren bestaetigt sie sogar besser als
Untersuchung B selbst. **Die BEGRUENDUNG haelt nicht**: die genannte Kennzahl "Delta 5,9 px,
bester Treffer aller 119 Rechtecke" stammt aus einem Mass, das die EINZIGE bewiesene Paarung um
21,2 px verfehlt und sie auf **Platz 15 von 119** setzt; und das Argument "dieselbe Zeichnung auf
Seite 4 ⇒ derselbe Ort" (§6f) ist durch 13 seitenuebergreifende Kachel-Dubletten widerlegt,
sechs davon **pixelgleich (0 Unterschiede)** ueber verschiedene Etagen hinweg.

---

## 1. Was ich unabhaengig bestaetigt habe

### 1a. Raum -> Seite, selbst disassembliert (`0x8004b568`, Sprungtabelle `0x8001103c`)

Fall-Through-Switch, 38 Eintraege. Die Kaskaden enden bei:
`ori v0,2 @0x8004b684` / `ori v0,3 @0x8004b6f8` / `ori v0,4 @0x8004b758` /
`ori v0,5 @0x8004b768` / `sb zero,9742(at) @0x8004b7dc` / `@0x8004b884`.

| idx | Raeume | Seite |
|---|---|---|
| 0..11 | ROOM1000..ROOM10B0 | 2 (1F) |
| 12..17 | ROOM10C0..ROOM1110 | 3 (2F) |
| 18..22 | ROOM1120..ROOM1160 | 4 (3F) |
| **23** | **ROOM1170** | **5** |
| 24..29 | ROOM1180..ROOM11D0 | 0 (B1) |
| 30..37 | ROOM11E0..ROOM1250 | 1 (B2) |

Seite 5 wird also **nur** in ROOM1170 gezeigt. Sie hat **2** Rechtecke
(count/ptr @0x80076868 = 2 / 0x800764bc). ROOM1170 hat **genau 2** zusammenhaengende
Kollisions-Komponenten — und zwar bei **jeder** Verschmelz-Toleranz von GAP=0 bis GAP=5000
(gemessen). Rect 1 ist per Originalformel als Hof bewiesen. Also bleibt fuer r0 nur comp1.
**Das ist der tragfaehige Kern von B1 — nicht der Groessenvergleich.**

### 1b. Kachel-Inhalt (selbst dekodiert, 4bpp, 128 B/Zeile)

`page5 r0` ist eine **voellig merkmalslose** L-Form: 42x22 gezeichnet, nur Umriss (Index 4) +
Fuellung (Index 1), **INNER-grey = 0**. `page4 r3` ist dieselbe Silhouette **plus zwei graue
Figuren**: x11..15/y0..5 (oeffnet durch die OBERKANTE) und x10..14/y16..21 (oeffnet durch die
UNTERKANTE). Es sind **Gang-Stummel** (zwei parallele Linien, ~3 px lichte Weite), keine
"Tuermarken". Genau diese 22 Pixel sind der Unterschied (Index 4 -> 1, Richtung Seite4 -> Seite5).

### 1c. Tuer-Test — er stuetzt B1 und schlaegt jede Alternative

Tuerpositionen aller Seite-4-Raeume, in die 42x22-Kachel normiert:

| Kandidat | Tueren (tile-x, tile-y von 42x22) |
|---|---|
| **ROOM1170 comp1** | -> 1130 **(16,0 / 1,6) OBERKANTE**; -> 1140 **(15,2 / 18,6) unten**; -> 1170-Hof (37,2 / 20,1) UNTERKANTE |
| ROOM1160 comp0 | 2 Datensaetze, **derselbe Punkt** (21,8 / 9,3), innen |
| ROOM1120 comp0 | 3 Tueren, alle innen |
| ROOM1140 comp0 | 3 Tueren: 2x deckungsgleich oben, 1 innen |
| ROOM1150 comp0 | 1 Tuer |
| ROOM1130 comp0 | 4 Tueren (2 oben, 1 unten, 1 innen) |

Gezeichnete Stummel: oben x~13, unten x~12. **Nur ROOM1170 comp1** liefert das Muster
"eine Tuer durch die Oberkante + eine Tuer unten", Abweichung ~3 px in x.
=> `page4 r3` gehoert tatsaechlich ROOM1170 comp1, nicht ROOM1160.

**Aber:** comp1 hat **DREI** scharfe Tueren; die dritte (zurueck zum Hof, Kachel 37,2/20,1,
rechtes Armende) ist auf **keiner** der beiden Kacheln gezeichnet — dort steht durchgehend
Umrisslinie (Index 4 bei x=41, y15..20). §6f/§6g spricht von "den zwei Ausgaengen" und laesst
die dritte weg.

---

## 2. Was NICHT haelt — Befund 1: das Groessenmass ist unkalibriert und faellt am eigenen Beweis durch

Untersuchung B misst **Wandzellen-Bbox gegen Rechteck-Box**. Angewandt auf die **einzige
Paarung, die B selbst beweist** (ROOM1170 comp0 = Hof = Seite5 r1, §6c, Abweichung <= 2 px):

* comp0 Wandzellen-Bbox = **63,2 x 62,0 px**, echtes Rechteck `page5 r1` = **48x56**
  -> **Delta 21,2 px**.
* Unter genau diesem Mass waere der Hof **page7 r4/r5 (64x64, Delta 2,8)** zuzuordnen;
  sein WAHRES Rechteck landet auf **Platz 15 von 119**.

Ein Mass, das die bekannte richtige Antwort auf Platz 15 setzt, kann den Superlativ
"bester Treffer aller 119 Rechtecke" nicht tragen. Zusaetzlich mischt B zwei Groessen:
verglichen wird gegen die **Rect-Box 48x24**, gezeichnet sind aber nur **42x22**
(6x2 px der Box sind Index 0 = transparent).

### 2a. Das geeichte Mass — und es bestaetigt B1 trotzdem

Der Hof zeigt, woran die Kachel wirklich haengt: nicht an der Wand-Bbox, sondern an der
**begehbaren Innenflaeche + Wandstaerke**.

| ROOM1170 comp0 (Hof) | px |
|---|---|
| Wandzellen-Bbox (Mass von B) | 63,2 x 62,0 |
| Band-3-Boden-Slab | 45,8 x 53,6 |
| eingeschlossene Lauf-Flaeche (eigene Flutfuellung je Band) | 42,4 x 50,6 |
| **gezeichneter Inhalt r1** | **46,0 x 55,0** |

=> Eichung: gezeichnet = Lauf-Flaeche **+3,6 / +4,4 px** (die Wandstaerke).
Damit ueber ALLE 125 Raum-Komponenten des Spiels neu gerechnet:

* gegen `page5 r1` (46x55): **ROOM1170 comp0 Platz 1/125, Delta 0,0** (Eichprobe besteht)
* gegen `page5 r0` / `page4 r3` (42x22): **ROOM1170 comp1 Platz 1/125, Delta 4,1**
  * Platz 2 ROOM11D0 c0 (8,1) — Raum 29, gehoert per Dispatch auf Seite 0
  * Platz 3 ROOM1160 c0 (12,5), Platz 4 ROOM5050 c0 (15,1)

**Die Schlussfolgerung ueberlebt die Korrektur des Masses; die Zahl 5,9 und der Superlativ
tun es nicht.** Richtig ist **Delta 4,1 px, Platz 1 von 125 Komponenten** — mit einem Mass,
das die Eichprobe besteht.

---

## 3. Was NICHT haelt — Befund 2: "dieselbe Zeichnung ⇒ derselbe Ort" ist widerlegt

§6f schliesst aus der Pixelgleichheit von `page4 r3` und `page5 r0` (22/1152 = 1,91 %) auf
denselben Ort. Kachel-Wiederverwendung ist in diesem Asset aber die **Regel**:

| Unterschied | Kachel A | Kachel B | Bedeutung |
|---|---|---|---|
| **0,00 % (0/256)** | page2 r9 uv(168,40) | page3 r9 **und** page4 r0 | **1F = 2F = 3F**, identisch |
| **0,00 % (0/576)** | page2 r10 uv(168,16) | page4 r1 | 1F = 3F |
| **0,00 % (0/2560)** | page10 r2 uv(136,16) 64x40 | page11 r2 | LAB B3 = LAB B4 |
| **0,00 % (0/256)** | page8 r2 uv(0,32) | page9 r13, page10 r3 | drei Seiten |
| **0,00 % (0/576)** | page8 r7 uv(16,32) | page9 r14, page11 r3 | drei Seiten |
| 1,04 % (28/2688) | page10 r1 | page11 r1 | |
| 1,48 % (19/1280) | page1 r2 uv(40,32) | page6 r7 uv(**0,32**) | sogar ueber Stages, andere uv |
| **1,91 % (22/1152)** | **page4 r3** | **page5 r0** | **Rang 14** dieser Liste |
| innerhalb einer Seite | page12 r1 = r2 = r3 (0/1536) | page1 r6 = r7 = r8 (0/384) | drei Rects, eine Kachel |

Die untersuchte Uebereinstimmung ist also **nicht auffaellig, sondern unterdurchschnittlich** —
13 Paare sind gleicher, sechs davon exakt. Die Zeichner-Praxis "Standardkachel wiederverwenden"
erklaert das Datum mindestens genauso gut wie "derselbe Ort".

**Konsequenz:** die Tuer-Stummel auf `page4 r3` duerfen nicht allein wegen der Kachelgleichheit
auf `page5 r0` uebertragen werden. `page5 r0` selbst traegt **null** Innenzeichnung.
Dass die Zuordnung trotzdem haelt, liegt an §1a (Seite 5 = nur ROOM1170, 2 Rects, 2 Komponenten,
r1 bewiesen) und am Tuer-Test §1c — **nicht** an §6f.

---

## 4. Was NICHT haelt — Befund 3: "VOLLSTAENDIG" ist zu stark

Lauf-Flaechen von comp1 (eigene Flutfuellung je Band), in Kachel-Koordinaten
(Ursprung x=-28900, z1=-17070, sx=2280, sy=2268, y gespiegelt):

| Band | Welt (begehbar) | -> Kachel | Lage in der L-Form |
|---|---|---|---|
| 4 oberer Korridor | x[-20471,-10432] z[-27134,-24434] | x 18,3..40,2 / y 15,9..21,8 | **unterer Arm** OK |
| 2 Absatz | x[-26900,-24063] z[-26800,-24085] | x 4,3..10,5 / **y 15,2..21,0** | liegt im **ARM**-Streifen, nicht "im linken Block" |
| 0 Hauptgang | x[-24183,-20000] z[-26881,-18272] | x 10,3..19,3 / y 2,6..21,2 | linker Block, ragt 1,3 px darueber hinaus |
| 0 West-Anbau | x[-26926,-24095] z[-20437,-18272] | x 4,5..10,9 / y 2,6..7,3 | linker Block, oben |

* Der **Absatz (Band 2) liegt im y-Streifen des Arms** (14..21), nicht im linken Block —
  er sitzt in der Ecke, wo beide Schenkel zusammenlaufen. Die Formulierung
  "der linke Block ... ist die unterste Ebene und der Absatz" ist irrefuehrend.
* Der linke Block ist **x0..17 ueber y0..21**; begehbare Geometrie gibt es dort nur bei
  x 4,5..19,3. Der Bereich **x0..10 / y8..15** (~60 der 588 gezeichneten Pixel, ~10 %)
  hat **keine** Entsprechung in der Kollision.
* Auch B's eigene Tabelle (§6e) sagt Band 0 = "x 0,6..24,0 / y 0..25,5" — also 24 px breit
  bei einem 18 px breiten linken Block und ueber die volle Kachelhoehe. Die verbale Zuordnung
  deckt sich nicht mit den eigenen Zahlen.
* Die dritte Tuer (Hof-Rueckweg, Kachel 37,2/20,1) fehlt in der Zeichnung.

---

## 5. Geprueft und VERWORFEN (die Konkurrenten, mit demselben Mass gemessen)

| Alternative | geeichtes Delta gegen 42x22 | Tuer-Test | Urteil |
|---|---|---|---|
| **ROOM1170 comp1** | **4,1 (Platz 1/125)** | oben+unten, ~3 px | **beste Erklaerung** |
| ROOM11D0 comp0 | 8,1 (Platz 2) | — | Raum 29 -> Seite 0, dort eigenes Rect |
| ROOM1160 comp0 | 12,5 (Platz 3) | 2 Tueren auf EINEM Innenpunkt | faellt am Tuer-Test |
| ROOM1130 comp0 | 31,9 x 83,0 | 4 Tueren | gehoert r4 (32x80) |
| ROOM1140 comp2 | 27,6 x 13,3 | — | zu klein |
| "Standardkachel, Ort unbestimmt" | — | — | erklaert die Pixelgleichheit, nicht die Rect-Liste |
| "Seite 4 zeichnet nur eigene Raeume, r3 = 1160" (vollstaendige Hungarian-Zuordnung, Gesamtkosten 77,7 gegen 88,7 fuer die B-Lesart) | — | **faellt am Tuer-Test** | verworfen |

Die "Zaehl-Regel" (#Rects == #Komponenten) ist **kein** Gesetz: bei GAP=1500 stimmt sie fuer
Seiten 3/4/5, aber Seite 0 (7 vs 6), Seite 1 (10 vs 14) und Seite 2 (11 vs 21) nicht.
Nur fuer Seite 5 ist sie **toleranzstabil** (2 vs 2 bei GAP 0..5000) — und genau dort wird sie
gebraucht.

---

## 6. Fazit fuer den Auftraggeber

* **Zuordnung r0 = ROOM1170 comp1: bestaetigt**, unabhaengig und mit einem geeichten Mass
  besser als bisher (Delta **4,1 px, Platz 1/125** statt "5,9 px" aus einem Mass, das die
  Eichprobe auf Platz 15/119 setzt).
* **Drei Belege in B sind zu streichen bzw. zu ersetzen:**
  1. "Delta 5,9 px / bester Treffer aller 119 Rechtecke" — Mass unkalibriert (§2).
  2. "§6f dieselbe Zeichnung ⇒ derselbe Ort" — durch 13 Dubletten widerlegt (§3);
     der Beleg fuer `page4 r3` kommt aus dem **Tuer-Test**, nicht aus der Kachelgleichheit.
  3. "VOLLSTAENDIG" + Band-Zuordnung — Absatz liegt im Arm, ~10 % des linken Blocks ohne
     Geometrie, dritte Tuer ungezeichnet (§4).
* Fuer den Port folgt daraus **keine** Aenderung an `re15_map_zones.h` Zeile 62
  (`{0x1170, -28900,-28841, -8784,-17070, 5, 0, 1, 27}`) — die Zeile bleibt richtig.
