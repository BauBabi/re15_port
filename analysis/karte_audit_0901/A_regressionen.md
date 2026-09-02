# A — Hat meine Eichung Raeume verschlechtert?

Audit vom 2026-09-01. Alles unten ist gemessen; jede Konstante traegt ihre Adresse oder
ihren Datei-Byte-Offset. Es wurde **nichts** an Produktivdateien geaendert.

Bilder: `seite2_ist.png`, `seite3_ist.png`, `seite4_ist.png`
(blau = Karten-Rechteck, gelb = gemaltes Tuersymbol, **rot = projizierte Zonen-Bbox mit
meiner Eichung**, gruen = ohne Eichung / Bbox-Streckung).

---

## 0. Der wichtigste Befund zuerst: das Original HAT die Eichung, der Generator ignoriert sie

### 0.1 Formel und Tabelle, vollstaendig disassembliert

`FUN_800473f8`, Marker-Umrechnung:

```
800473f8  addiu sp,sp,-0x28
80047424  lbu   a2, DAT_800b260d(a2)      ; ZEILEN-INDEX
80047434  sll   a2,a2,0x3                 ; *8  -> 8 BYTE JE ZEILE
8004743c  addiu at,at,0x68b4              ; Basis 0x800768b0, Feld +4
80047444  lhu   v0,0x0(at)                ; sx = u16 @Zeile+4     <-- UNSIGNED
8004747c  lhu   a0,0x0(at)                ; sy = u16 @Zeile+6     <-- UNSIGNED
800474b0  subu  v1,zero,v1                ; die EINZIGE Negation, nur im z-Zweig
800474e8  lh    v0,0x0(at)                ; ox = s16 @Zeile+0
80047518  lhu   v1,0x0(at)                ; oy = u16 @Zeile+2
80047528  sh    v0, DAT_800b2606(at)      ; Ergebnis my
```

also

```
mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
```

Zeilenformat 8 Byte: `s16 ox, u16 oy, u16 sx, u16 sy`, Basis **0x800768b0**
(Datei-Offset 0x670b0).

### 0.2 Zeilen-Index = RAUM-INDEX (belegt, nicht vermutet)

Setzer `LAB_8004b568` (Zeiger @0x80074c0c), Sprungtabelle @0x8001103c, 0x26 Eintraege:

```
8004b56c  lh    v3, DAT_800b0fe2      ; Raumindex innerhalb der Stage
8004b574  sltiu v0,v1,0x26            ; 38 Raeume in STAGE1
8004b59c..8004b878  je Fall: sb (byte)DAT_800b0fe2 -> DAT_800b260d
8004b888  sb v0, DAT_800b260e         ; = KARTENSEITE
8004b894  sb v1, DAT_800b260d         ; = ZEILEN-INDEX
```

Fuer STAGE1 gilt also **Zeile == Raumindex, ohne Versatz**. STAGE2 addiert 0x26:
`8004b9c0  addiu v0,v0,0x26`. (Die Versaetze der Stages 3-6 habe ich NICHT nachgelesen —
fuer diese Aussage nicht noetig, und ich verkaufe sie deshalb auch nicht als belegt.)

Damit gleichzeitig verifiziert: `page_of()` im Generator stimmt fuer STAGE1
**byte-genau** mit dem Schalter ueberein (38/38 Raeume, 0 Abweichungen). Seite 3 =
Raeume 12..17 = ROOM10C0..ROOM1110, Seite 4 = Raeume 18..22 = ROOM1120..ROOM1160.

### 0.3 Die 13 echten STAGE1-Zeilen

| Zeile | Adresse | Raum | ox | oy | sx | sy | Welt/px |
|---|---|---|---|---|---|---|---|
| 2 | 0x800768C0 | ROOM1020 | 100 | 136 | 2287 | 2287 | 458 x 458 |
| 3 | 0x800768C8 | ROOM1030 | 108 | 194 | 2428 | 2229 | 432 x 470 |
| 4 | 0x800768D0 | ROOM1040 | 94 | 193 | 2080 | 2320 | 504 x 452 |
| 7 | 0x800768E8 | ROOM1070 | 79 | 205 | 2229 | 2088 | 470 x 502 |
| 21 | 0x80076958 | ROOM1150 | 111 | 130 | 2296 | 2312 | 457 x 454 |
| 23 | 0x80076968 | ROOM1170 | 100 | 206 | 2280 | 2268 | 460 x 462 |
| 25 | 0x80076978 | ROOM1190 | 75 | 127 | 1920 | 2301 | 546 x 456 |
| 26 | 0x80076980 | ROOM11A0 | 138 | 113 | 1776 | 2048 | 590 x 512 |
| 27 | 0x80076988 | ROOM11B0 | 159 | 110 | 2272 | 1954 | 462 x 537 |
| 28 | 0x80076990 | ROOM11C0 | 174 | 196 | **1** | **1** | Stub-Massstab |
| 30 | 0x800769A0 | ROOM11E0 | 91 | 157 | 2304 | 2240 | 455 x 468 |
| 32 | 0x800769B0 | ROOM1200 | 129 | 150 | 3168 | 2305 | 331 x 455 |
| 33 | 0x800769B8 | ROOM1210 | 177 | 140 | 2496 | 2250 | 420 x 466 |

Zeile 28 (ROOM11C0) hat nur Versatz, Massstab bleibt Stub: mit sx=sy=1 wird der
Skalenterm **immer 0** (`(64000*10*1)>>20 = 0`), der Marker steht also fest auf
(174,196). Als Beleg fuer einen Versatz taugt die Zeile deshalb nicht.

Alle uebrigen 25 STAGE1-Zeilen sind der Stub `{0,0,1,1}` — dort zeichnet auch das
Original keinen brauchbaren Marker (Ergebnis konstant (0,0)). Genau die Seiten 3 und 4,
ueber die sich der Nutzer beschwert, bestehen **ausschliesslich** aus Stub-Zeilen.

### 0.4 Gegenprobe: die Original-Zeilen TREFFEN die gemalten Tuersymbole

Tueren aus dem RDT, projiziert mit der Original-Zeile, Abstand zum naechsten gemalten
Tuersymbol der Seite (`analysis/kartensymbole/symbolkatalog.csv`):

```
ROOM1020  4.0 / 4.5 / 4.0 px
ROOM1070  6.1 px
ROOM1150  1.4 px
ROOM1190  5.0 / 5.0 / 2.0 / 2.0 px
ROOM11B0  3.0 / 3.0 / 0.0 px  (+2 Tueren nach ROOM1260: 57 px, anderer Bereich)
ROOM11E0  3.2 / 5.0 / 3.6 / 6.7 / 6.7 px
ROOM1200  4.1 px
ROOM1210  2.8 / 4.5 / 3.2 / 4.5 / 3.0 / 3.2 px
```

Der Tuerstempel ist 5x5 px gross — die Original-Zeilen setzen die Tueren also **auf**
ihr Symbol. Damit unabhaengig bestaetigt: Formel richtig, Symbolkatalog richtig,
Kacheln weltgleich orientiert.
(Ausreisser ROOM11A0, 40..80 px: Hof mit Ausgaengen in andere Stages; siehe §7.)

### 0.5 Und das steht meiner erzeugten Eichung gegenueber

| Raum | ORIGINAL ox/oy/sx/sy | MEINE Eichung | Abweichung |
|---|---|---|---|
| ROOM1020 z1 | 100/136/2287/2287 | 71/279/1825/**5430** | oy +143 px, sy **x2,37** |
| ROOM1070 z0 | 79/205/2229/2088 | 170/137/**757**/1228 | ox +91, oy −68, sx **x0,34** |
| ROOM1150 z0 | 111/130/2296/2312 | 114/130/1903/2289 | ox +3, sx x0,83 |
| ROOM1170 z0 | 100/206/2280/2268 | 123/193/1658/2013 | ox +23, sx x0,73 |
| ROOM1170 z1 | 100/206/2280/2268 | 130/109/2189/1960 | oy −97 |
| ROOM11A0 z0 | 138/113/1776/2048 | 105/181/966/2078 | oy +68, sx x0,54 |
| ROOM11B0 z0 | 159/110/2272/1954 | 160/103/2104/1649 | sy x0,84 |
| ROOM11E0 z1 | 91/157/2304/2240 | 157/185/1313/2344 | ox +66, sx x0,57 |
| ROOM1210 z0 | 177/140/2496/2250 | 190/98/**821**/**629** | sx x0,33, sy x0,28 |
| ROOM1020 z0+z2, ROOM1070 z1+z2, ROOM1190, ROOM1200 | vorhanden | **keine** (Bbox) | Zeile ungenutzt |

Massstabs-Abweichung meiner Eichung gegen das Original: **Median 19 %, Mittel 33 %,
max 137 %** (n = 18 Achsen).

**Fazit 0:** fuer 13 STAGE1-Raeume liegt die Eichung byte-true in der EXE, und der
Generator liest sie nicht. Das ist kein Feinschliff, das ist ein Datenfehler.

---

## 1. Aufgabe 1 — Abstand zu den gemalten Tuersymbolen, Eichung vs. Bbox

Zuordnung ueber **Seite+Rect** (nicht geometrisch), wie gefordert.

Ergebnis der wortwoertlichen Messung: **genau EINE Zone wird schlechter** —
`ROOM11B0 z0` (Seite 0 Rect 3): Eichung 4,24 px, Bbox 1,41 px (+2,83 px).
Alle anderen sind gleich oder besser.

### Diese Messung ist wertlos, und das laesst sich zeigen

Von den 37 geeichten Zonen haben **29 genau EIN** gemaltes Tuersymbol in ihrem
Rechteck, 3 haben gar keines, nur **5** haben zwei oder mehr. Bei genau einem Symbol
bestimmt der Generator den Versatz aus der Gleichung "diese eine Tuer liegt auf diesem
einen Symbol" — der Restfehler ist danach **konstruktionsbedingt 0,00 px**. Genau das
steht auch in der Messreihe: 1090, 10B0, 10E0, 1120, 1130, 1140, 1210, 1220, 1230,
2050, 2090, 3070, 4010, 4090, 5020, 5090, 5100, 5130 — alle exakt 0.00.

Die Metrik misst also, wie gut ein Fit seine eigene Stuetzstelle trifft. Sie kann
Regressionen prinzipiell nicht sehen. Deshalb unten zwei unabhaengige Masse.

---

## 2. Das aussagekraeftige Mass: bleibt der Marker ueberhaupt im Rechteck?

`re15_map_zones.c:143-147` **klemmt** die geeichte Projektion hart ins Rechteck:

```c
if (px < rx) px = rx;
if (px > rx + rw - 1) px = rx + rw - 1;
if (py < ry) py = ry;
if (py > ry + rh - 1) py = ry + rh - 1;
```

Liegt die projizierte Raumflaeche neben ihrem Rechteck, klebt der Marker an der Kante:
er bewegt sich auf einer Achse gar nicht mehr — genau das, was der Nutzer als
"laeuft in die falsche Richtung" beschreibt.

Gemessen auf der **begehbaren** Flaeche (SCA-Zellen der Zone, 8x8-Raster je Zelle,
Zonenzuordnung per `zone_at` wie im Generator):

| Zone | Seite/Rect | geklemmt | nur X | nur Y |
|---|---|---|---|---|
| ROOM2060 z0 | 6/7 | **100 %** | 100 % | 62 % |
| ROOM2050 z1 | 6/1 | **100 %** | 31 % | 100 % |
| ROOM2040 z1 | 6/0 | **100 %** | 100 % | 100 % |
| ROOM11E0 z1 | 1/2 | **100 %** | 50 % | 100 % |
| **ROOM10D0 z1** | **3/3 (2F)** | **100 %** | 62 % | 100 % |
| ROOM1020 z1 | 2/10 | **100 %** | 100 % | 100 % |
| ROOM1090 z0 | 2/1 | 97 % | 49 % | 89 % |
| ROOM2090 z0 | 6/6 | 90 % | 48 % | 82 % |
| ROOM5130 z0 | 9/4 | 88 % | 88 % | 32 % |
| ROOM1230 z0 | 1/3 | 88 % | 88 % | 0 % |
| ROOM5020 z0 | 9/12 | 84 % | 62 % | 62 % |
| ROOM2050 z0 | 6/2 | 73 % | 1 % | 73 % |
| ROOM1000 z0 | 2/7 | 72 % | 66 % | 16 % |
| ROOM1000 z1 | 2/7 | 71 % | 71 % | 0 % |
| ROOM4090 z1 | 8/5 | 71 % | 0 % | 71 % |
| **ROOM1120 z0** | **4/5 (3F)** | **70 %** | 58 % | 37 % |
| ROOM5100 z0 | 11/1 | 67 % | 62 % | 11 % |
| ROOM3070 z0 | 7/5 | 65 % | 59 % | 30 % |
| ROOM10B0 z0 | 2/4 | 64 % | 64 % | 16 % |
| ROOM4010 z0 | 8/6 | 63 % | 59 % | 8 % |
| **ROOM10E0 z0** | **3/4 (2F)** | **49 %** | 0 % | 49 % |
| ROOM5070 z0 | 9/7 | 49 % | 48 % | 2 % |
| ROOM1070 z0 | 2/6 | 47 % | 24 % | 32 % |
| ROOM5090 z0 | 9/10 | 46 % | 2 % | 46 % |
| ROOM1220 z0 | 1/4 | 43 % | 43 % | 0 % |
| ROOM30D0 z0 | 7/9 | 41 % | 29 % | 25 % |
| ROOM1210 z0 | 1/4 | 17 % | 17 % | 0 % |
| ROOM1170 z1 | 5/0 | 15 % | 15 % | 0 % |
| ROOM3000 z0 | 7/2 | 15 % | 13 % | 2 % |
| ROOM11B0 z0 | 0/3 | 12 % | 2 % | 11 % |
| ROOM11A0 z0 | 0/0 | 10 % | 10 % | 0 % |
| ROOM1150 z0 | 4/2 | 8 % | 0 % | 8 % |
| ROOM1130 z0 | 4/4 | 8 % | 0 % | 8 % |
| ROOM11C0 z0 | 0/4 | 5 % | 2 % | 3 % |
| ROOM3060 z0 | 7/6 | 5 % | 5 % | 0 % |
| ROOM1170 z0 | 5/1 | 4 % | 0 % | 4 % |
| ROOM1140 z0 | 4/6 | 3 % | 1 % | 2 % |

**37 von 37 geeichten Zonen klemmen**; 31 davon auf mehr als 10 % der begehbaren
Flaeche, 6 auf 100 %. Bei der reinen Bbox-Streckung kann das **nicht** passieren: dort
wird die Weltkoordinate vor der Streckung geklemmt, das Ergebnis liegt also immer im
Rechteck. Die einzige ungeeichte Zone mit einem Wert > 2 % (ROOM11F0 z2, 20 %) ist ein
Messartefakt der GAP-Toleranz von 1500 Welteinheiten, keine Klemmung.

**Das ist die Regressionsliste.** Jede geeichte Zone in dieser Tabelle mit einem Wert
ueber ~10 % ist gegenueber der Bbox-Streckung eine Verschlechterung.

---

## 3. Aufgabe 2 — Richtung: gibt es gespiegelte oder gedrehte Kacheln?

### 3.1 Antwort: NEIN. Belegt, nicht angenommen

1. **Das Original kann es gar nicht ausdruecken.** `lhu` @0x80047444 und @0x8004747c
   laden sx/sy **vorzeichenlos**; die einzige Negation ist das feste
   `subu v1,zero,v1` @0x800474b0 im z-Zweig. Es gibt keinen zweiten Pfad. Die
   Kartenorientierung ist damit fest verdrahtet: **+x → +mx (rechts), +z → −my (nach
   oben)**. Waere eine Kachel gespiegelt, koennte fuer ihren Raum keine Zeile existieren.
2. **Und fuer die 13 Raeume mit Zeile passt genau diese ungespiegelte Formel** — §0.4:
   1,4 bis 6,7 px auf den gemalten Symbolen.
3. **Ordnungstest ueber alle Zonen.** Je Zone bis zu 200 begehbare Punkte, paarweise
   verglichen (nur Paare mit > 800 Welteinheiten Abstand), Achse fuer Achse, **nach**
   der Klemmung: **0 Zonen mit echter Umkehr in X, 0 in Y.**

Der Port braucht also **kein** Vorzeichen an sx/sy. Wer eines einbaut, baut einen
Fehler ein, den das Original nicht hat.

Eine schwaechere Gegenprobe (Wandseite der Tuer aus der Welt-Bbox gegen die Wandseite
des gemalten Symbols) liefert 24 Widersprueche bei 40 Zonen — die zaehle ich
**ausdruecklich nicht** als Spiegel-Beleg: die Ableitung "naechste Bbox-Kante = Wand"
versagt in L-foermigen Raeumen, und ein Teil der Widersprueche geht auf falsche
Rechteck-Zuordnungen zurueck (§5).

### 3.2 Was der Nutzer stattdessen sieht — ROOM1120, Zahl fuer Zahl

Eichung im Header: `ox=108 oy=181 sx=1857 sy=1997`; Seite 4 Rect 5 = x120..159, y119..158.

Lauf nach OSTEN auf der Raummitte (wz = 2700):

```
wx = -10500 -> roh (146,115) -> gezeichnet (146,119)
wx =  -5700 -> roh (155,115) -> gezeichnet (155,119)
wx =  -2500 -> roh (160,115) -> gezeichnet (159,119)   ab hier steht er
wx =      0 -> roh (163,115) -> gezeichnet (159,119)
wx =  +7100 -> roh (177,115) -> gezeichnet (159,119)
```

Lauf nach SUEDEN (wx = -950):

```
wz =  -7800 -> roh (163,135) -> gezeichnet (159,135)
wz =  -2550 -> roh (163,125) -> gezeichnet (159,125)
wz =   +950 -> roh (163,118) -> gezeichnet (159,119)   ab hier steht er
wz = +13200 -> roh (163, 95) -> gezeichnet (159,119)
```

Der Marker sitzt ueber 58 % der Raumbreite und 60 % der Raumtiefe **fest in der oberen
rechten Ecke** von Rect 5. Das ist exakt der Report: "kommt von der richtigen Seite
rein, laeuft danach in die falsche Richtung."

Dieselbe Strecke ohne Eichung (Bbox-Streckung): x 120 → 126 → 133 → 140 → 146 → 153,
y 158 → 152 → 145 → 138 → 132 → 125 → 118. Sauber monoton ueber die volle
Rechteckbreite.

Ursache ist **nicht** die Richtung, sondern Massstab + Versatz: die projizierte
Raumbox ist x146..180 / y95..135, das Rechteck x120..159 / y119..158 — Ueberlappung
nur 40 % in x und 41 % in y.

---

## 4. Aufgabe 3 — Seite 3 ("2F") und Seite 4 ("3F") Rechteck fuer Rechteck

Seiten-Zuordnung byte-verifiziert (§0.2). Kachel: Seite 3 = MAP04.PIX, Seite 4 =
MAP05.PIX (id-Tabelle @0x80074c4c). Massstabs-Erwartung 460 Welt/px = Median der
12 echten STAGE1-Zeilen (Spanne 331..590).

### Seite 3 — "2F", 10 Rechtecke

| Rect | Lage/Groesse | uv | belegt durch | Beurteilung |
|---|---|---|---|---|
| 0 | (180,69) 32x96 | (0,32) | — | **grau** |
| 1 | (109,77) 56x56 | (32,32) | ROOM10F0 z0 (Bbox) | Raum 46x48 px, Rect 56x56; gezeichnet sind aber nur **16x9 px** — die Kachel ist hier fast leer |
| 2 | (102,77) 24x80 | (88,48) | — | **grau** |
| 3 | (145,109) 56x40 | (192,16) | ROOM10D0 **z1** (Eichung) | Groesse passt (52x37 px), **Versatz kaputt**: Projektion x105..156 / y163..203 gegen Rect x145..200 / y109..148 → 0 % Ueberlappung in y, **100 % geklemmt** |
| 4 | (119,125) 72x64 | (184,56) | ROOM10E0 z0 (Eichung) | Raum nur 50x46 px, Rect 72x64 → Eichung erzwingt 345x327 Welt/px, **ausserhalb** der Original-Spanne; 49 % geklemmt |
| 5 | (180,59) 48x32 | (112,64) | ROOM1100 z0 (Bbox) | plausibel (52x37 px) |
| 6 | (180,88) 24x24 | (128,16) | ROOM1100 z1 (Bbox) | Raum 41x47 px auf 24x24 → 779x904 Welt/px, unplausibel |
| 7 | (164,93) 16x24 | (152,16) | — | **grau** |
| 8 | (164,77) 24x24 | (128,40) | ROOM10C0 z0 (Bbox) | Raum 40x46 px auf 24x24 → 765x890 Welt/px, unplausibel |
| 9 | (109,134) 16x16 | (168,40) | — | **grau** |

**Die beiden schwersten Loecher auf 2F:**

* `ROOM10D0 z0` — die **grosse** Zone (x −21495..10650, z −10512..30850 = 70x90 px)
  ist **gar nicht zugeordnet**. Alle **5 Tueren** des Raums (nach 10F0 zweimal, 10E0,
  10C0, 1100) liegen in z0; die zugeordnete z1 enthaelt **keine einzige**. Der Flur,
  den der Nutzer betritt, hat also kein Rechteck — gezeichnet und geklemmt wird ein
  Nebenteil.
* `ROOM1110` — **beide** Zonen unzugeordnet (z0 wird in `gen_map_zones.py` per
  `ZONE_DROP` ausdruecklich entfernt, z1 faellt durch die Kostenheuristik). Der Raum
  hat auf der Karte **keine Existenz**.

Dazu 4 von 10 Rechtecken ohne Zone → sie werden dauerhaft grau gemalt.

### Seite 4 — "3F", 7 Rechtecke

| Rect | Lage/Groesse | uv | belegt durch | Beurteilung |
|---|---|---|---|---|
| 0 | (127,137) 16x16 | (168,40) | — | **grau** |
| 1 | (136,137) 24x24 | (168,16) | ROOM1160 z0 (Bbox) | Raum 36x28 px auf 24x24 → 698x546 Welt/px, grenzwertig |
| 2 | (120,80) 32x40 | (96,48) | ROOM1150 z0 (Eichung) | Zuordnung **durch das Original bestaetigt** (IoU 0,80). Eichung aber x0,83 daneben → Original-Zeile 21 nehmen |
| 3 | (152,89) 48x24 | (192,16) | — | **grau** |
| 4 | (144,80) 32x80 | (0,32) | ROOM1130 z0 (Eichung) | Groesse passt (32x83 px), nur 8 % geklemmt — die beste geeichte Zone der Seite |
| 5 | (120,119) 40x40 | (128,16) | ROOM1120 z0 (Eichung) | Groesse passt (42x46 px), **Versatz kaputt** → 70 % geklemmt (§3.2) |
| 6 | (160,110) 56x56 | (32,32) | ROOM1140 z0 (Eichung) | Groesse passt (59x60 px), 3 % geklemmt — in Ordnung |

Seite 4 ist deutlich gesuender als Seite 3: die vier grossen Raeume sitzen auf
groessenmaessig passenden Rechtecken, nur ROOM1120s Versatz ist falsch. Das deckt sich
mit dem Report ("ab 2F ist die Karte im Eimer" plus ein Einzelfehler in 1120).

---

## 5. Warum die Eichung schiefgeht — die Wurzel

Die Annahme im Generator lautet:

> "MASSSTAB aus der gezeichneten Flaeche: das Rechteck IST die Zeichnung des Raums,
> also bildet dessen Kollisions-Bbox auf die nicht-transparenten Pixel der Kachel ab."

Diese Annahme ist **falsch**, und die Original-Zeilen widerlegen sie direkt:

* **ROOM1150:** Kollisions-Bbox 14328 x 18324. Original sx=2296 entspricht **31,4 px**
  Breite — das ist die **volle Rechteckbreite (32)**. Die gezeichnete Tinte ist nur
  **26 px** breit. Aus 26/14328 folgt sx=1903, also genau der zu kleine Wert im Header.
* Systematisch: mein Massstab liegt bei 12 von 18 Achsen **unter** dem Original
  (Faktoren 0,28 bis 1,05), weil die Kollisionsflaeche regelmaessig ueber die
  gezeichnete Kachel hinausgeht (ROOM1170 z0: Original-Projektion 65x63 px auf einem
  48x56-Rect).
* Mein Wertebereich: sx 757..3240, sy 629..5430 = **193..1667 Welt/px**. Das Original
  ueber alle Stages: **263..819 Welt/px**. **14 von 37** erzeugten Zeilen liegen
  ausserhalb des gesamten je im Original beobachteten Bereichs:
  1000 z0/z1, 1020 z1, 1070 z0, 11A0 z0, 1210 z0, 1220 z0, 1230 z0, 2040 z1, 2050 z1,
  4090 z1, 5020 z0, 5090 z0, 5130 z0.

Gegenprobe, wie gut die **einfache Bbox-Streckung** (Zonen-Bbox → volles Rechteck) den
Original-Massstab trifft, wenn das Rechteck stimmt: **Median 8 %, Mittel 12 %, max 40 %**
(n = 16 Achsen). Also rund 2,5x besser als meine Eichung — und ohne jede Klemmung.

Zweite Wurzel: **die Rechteck-Zuordnung selbst.** Projiziert man die Zonen-Bbox mit der
Original-Zeile und sucht das ueberlappungsstaerkste Rechteck (nur eindeutige Faelle,
IoU >= 0,4), stimmt meine Zuordnung in **4 von 8** Faellen:

```
ROOM1020 z0 -> Seite 2 Rect 1 (IoU 0,79)   ich: Rect 3   FALSCH
ROOM1070 z0 -> Seite 2 Rect 3 (IoU 0,74)   ich: Rect 6   FALSCH
ROOM1150 z0 -> Seite 4 Rect 2 (IoU 0,80)   ich: Rect 2   richtig
ROOM1170 z0 -> Seite 5 Rect 1 (IoU 0,63)   ich: Rect 1   richtig
ROOM1190 z0 -> Seite 0 Rect 1 (IoU 0,56)   ich: Rect 1   richtig
ROOM11B0 z0 -> Seite 0 Rect 3 (IoU 0,81)   ich: Rect 3   richtig
ROOM1200 z0 -> Seite 1 Rect 2 (IoU 0,68)   ich: Rect 5   FALSCH
ROOM1210 z0 -> Seite 1 Rect 3 (IoU 0,50)   ich: Rect 4   FALSCH
```

Auf einem falschen Rechteck kann keine Projektion stimmen — und die Eichung
"repariert" den Fehler dann optisch, indem sie eine Tuer auf ein Symbol zwingt und
alles andere aus dem Rechteck schiebt.

---

## 6. Empfehlung

### 6.1 Sofort zuruecknehmen

**Alle 37 erzeugten Eichungen** in `s_map_zones[]` (Felder ox/oy/sx/sy auf 0 =
Rueckfall auf die Bbox-Streckung). Begruendung: 37/37 klemmen, 31/37 auf mehr als 10 %
der begehbaren Flaeche, 6 auf 100 %; die Bbox-Streckung klemmt nie und trifft den
Original-Massstab bei richtigem Rechteck im Median auf 8 %.

Wer nur die schlimmsten nehmen will — die Grenze bei 10 % Klemmung laesst genau diese
sechs stehen: `ROOM1130 z0` (8 %), `ROOM1150 z0` (8 %), `ROOM11C0 z0` (5 %),
`ROOM3060 z0` (5 %), `ROOM1170 z0` (4 %), `ROOM1140 z0` (3 %). Davon haben 1150, 1170
und 11C0 aber eine **Original-Zeile**, die anders lautet — die gehoert vorgezogen.
Es blieben also nur `ROOM1130 z0`, `ROOM1140 z0`, `ROOM3060 z0`.

### 6.2 Statt dessen einbauen, in dieser Reihenfolge

1. **Die Original-Zeilen uebernehmen**, wo sie existieren: `0x800768b0 + Zeile*8`,
   `s16 ox, u16 oy, u16 sx, u16 sy`; Zeile = Raumindex (STAGE1) bzw. Raumindex + 0x26
   (STAGE2, `addiu` @0x8004b9c0). Fuer STAGE3-6 vorher den jeweiligen `addiu` im
   Schalter hinter den Zeigern @0x80074c14/18/1c/20 lesen — nicht raten.
   Zeile 28 (ROOM11C0) NICHT nehmen: sx=sy=1 macht den Marker ortsfest.
   Diese Zeilen gelten pro **Raum**, nicht pro Zone; fuer Mehr-Zonen-Raeume ist die
   Uebertragung offen (das Original kennt das Zonen-Modell nicht).
2. **Die Rechteck-Zuordnung reparieren, bevor irgendetwas geeicht wird.** Die
   Original-Zeilen liefern dafuer 8 harte Stuetzstellen (§5); die vier falschen sind
   sofort korrigierbar. Auf Seite 3 zusaetzlich: `ROOM10D0 z0` und `ROOM1110` brauchen
   ueberhaupt erst ein Rechteck.
3. **Erst danach** fehlende Zeilen bestimmen — mit festem, aus den Original-Zeilen
   genommenem Massstab (STAGE1: sx-Median 2283, sy 1954..2320 / Median 2259) und nur
   zwei Freiheitsgraden fuer den Versatz.

### 6.3 Pflicht-Wache fuer jede kuenftige Eichung

Nicht der Symbolabstand ist das Kriterium (bei 29 von 37 Zonen konstruktionsbedingt
0,00 px), sondern:

> Die mit der Eichung projizierte Zonen-Bbox muss ihr Rechteck zu mindestens 90 %
> ueberdecken, **und** auf der begehbaren Flaeche duerfen unter 2 % der Punkte klemmen.

Diese Wache faellt bei ALLEN 37 aktuellen Eichungen durch.

### 6.4 Spiegelungen

Keine vorhanden (§3.1: 0 Umkehrungen in X, 0 in Y; das Original kann sie wegen `lhu`
gar nicht darstellen). **Kein Vorzeichen an sx/sy einbauen.** Sollte der Verdacht je
wieder aufkommen, ist der automatische Test der Ordnungstest aus §3.1 — er braucht nur
die Kollisionszellen und die Projektion, keine Symbole.

---

## 7. Offen / nicht entscheidbar

* **ROOM11A0** (Zeile 26): die Original-Zeile setzt die Tueren 40..80 px neben jedes
  gemalte Symbol. Entweder ist der Hof nur teilweise gezeichnet, oder die Zeile gehoert
  zu einem anderen Bereich. Naechster Weg: Savestate im Hof ziehen und das
  Marker-Ergebnis `DAT_800b2606` (`sh` @0x80047528) direkt auslesen.
* **Seiten 3/4 ohne Original-Zeile**: fuer ROOM10C0..ROOM1160 ist jede Zeile Stub. Ohne
  reparierte Rechteck-Zuordnung ist dort jede Eichung geraten. Naechster Weg: die
  Rechtecke ueber den Tuergraph (Partner-Tuer + Wandachse) statt ueber Flaechenraenge
  zuordnen, und den Massstab aus den 12 belegten STAGE1-Zeilen festhalten.
* **Stage-Versaetze 3-6** der Skalentabelle sind nicht nachgelesen. Zeilen fuer
  ROOM3xxx/4xxx/5xxx/6xxx sind daher **unbestaetigt**; der Generator-Wert
  `BASES = [0, 38, 50, 65, 77, 98]` kann so nicht stimmen (Stage 3 mit 0x20 Raeumen ab
  50 reicht bis 81 und ueberlappt Stage 4 ab 65).
* Der Etagen-Zweitzeichnung (`s_map_floors[]`) liegt ein Vergleich
  `rect_uv(zp,i) == uv0` **ueber Seiten hinweg** zugrunde. Jede Seite hat aber ihre
  eigene PIX (Seite 2 = MAP03, 3 = MAP04, 4 = MAP05; id-Tabelle @0x80074c4c) — gleiche
  uv heisst dort nicht automatisch gleiche Zeichnung. Der Generator-Kommentar behauptet
  Pixelgleichheit fuer drei konkrete Faelle; als allgemeine Regel ist das ungeprueft.
