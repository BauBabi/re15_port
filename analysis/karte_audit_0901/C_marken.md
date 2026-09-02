# C — Warum sind Türen doppelt oder fehlen?

Audit 2026-09-01. Alles unten ist **gemessen**, nicht modelliert. Werkzeuge und Rohdaten:

| Messung | Werkzeug | Ergebnis-Datei (Scratchpad) |
|---|---|---|
| Generator-Zwischenstände (`vor`, `supp`, `marks`, `floors`, `assign`, `eichung`) | instrumentierte Kopie `gen_dbg.py` | `dump.json` |
| Gegenprobe: Kopie erzeugt **byte-identischen** `re15_map_zones.h` | `diff` | IDENTISCH |
| Live-AOTs nach `scd_room_reenter` für alle 103 Basisräume | `c_doors.c` / `c_all.c` (linkt `libre15_engine.a`) | `live_doors.txt` |
| Op-Liste des Kartenschirms je Seite | `c_ops.c` → `re15_inv_screen_build()` | `ops_p0..p12.txt` |
| Pixel-Rasterung der Op-Liste (CLUT 13 = TEX.TIM clut-Zeile 21, `mod5`) | `render_p.py` | `karte_p4.png`, `karte_p5.png`, `p4_vergleich.png`, `p5_vergleich.png` |

---

## 1. Zählung je Seite

```
Seite Titel      Kachel-  Port-   Port-    Generator  LIVE-DOOR  davon      eindeutige  unter-
                 Symbole  Türen   Treppen  statisch   gesamt     begehbar   Durchgänge  drückt
   0  B1              6       6        0         23         12        12           9       9
   1  B2              9       1        0         25         21        20           9      19
   2  1F              9       6        5         33         34        31          16       9
   3  2F              4       8        2         18         14        14           8       1
   4  3F              6       5        4         13          9         9           6      10
   5  (Dach)          0       3        2          4          4         3           2       0
   6  DRAINS B2       6      11        0         34         28        28          14       5
   7  FACTORY        15      11        2         37         32        27          16       6
   8  LAB B1          7       7        2         36         39        34          15      10
   9  LAB B2          7      14        1         41         36        35          19       7
  10  LAB B3          1       2        0          7          6         6           4       1
  11  LAB B4          2       2        2          3          3         3           3       1
  12  SUBWAY          3       7        5         14         11        11           7       1
 SUM                 75      83       25        288        249       233         128      79
```

* „Kachel-Symbole" = `analysis/kartensymbole/symbolkatalog.csv`, Typ `TUER`/`TUER2`.
* „Generator statisch" = Datensätze aus dem linearen SCD-Walk (`read_rdt`, gen_map_zones.py:164).
* „LIVE-DOOR" = AOT-Slots vom Typ `RE15_AOT_TYPE_DOOR` nach `scd_room_reenter(rdt,0,0,0)`.
* „begehbar" = LIVE-DOOR mit `half_w>0 || half_h>0` (Rechteck 0×0 = Skript-Warp).
* „eindeutige Durchgänge" = Raumpaare, jeder Durchgang einmal gezählt.

### Wo es nicht zusammenpasst

**(a) Der Generator sieht 288 Türen, der laufende Raum installiert 249 DOOR-AOTs — 42 von 103 Räumen weichen ab** (34× Generator > Live, 8× Generator < Live).
Der Generator-Walk ist ein **linearer** Byte-Walk über `mainScd`+`subScd` ohne Verzweigungslogik. Er nimmt jeden `Door_aot_set`-Datensatz, auch die, die im Auslieferungszustand nie ausgeführt werden — und er verpasst die, die erst ein Sub installiert (ROOM1080: Generator 0, Live 3).

Beleg an ROOM1170 (`ROOM1170.RDT`, `mainScd @Datei 0x11F4`):

```
@0x001318  op 0x06 (IF)      06 00 1c 00
@0x00131C  op 0x21 (Bedingung) 21 04 c3 00
@0x001320  op 0x2C Aot_set    slot=5 sce=1  rect(-22740,-27610) 2280x1210   <- MESSAGE
@0x001334  op 0x07 (ELSE)     07 00 26 00
@0x001338  op 0x3B Door_aot_set slot=5 sce=2 gleiches Rechteck -> ROOM1140  <- TÜR
```

Live-Ergebnis (`c_all.exe 1170`): `slot 5 typ=MESSAGE x=-21600 z=-27005 hw=1140 hh=605 ev=12`.
→ Im Auslieferungszustand ist die Pit-Tür zu ROOM1140 **keine Tür, sondern ein Examine-Text**. Der Generator hat sie trotzdem als Tür in die Karte geschrieben.

Dasselbe in ROOM1140 (`mainScd @0xA50`): `@0x000A7A` `Door_aot_set slot=1 sce=0` (inert) und `@0x000AB2` `Door_aot_set slot=1 sce=2` auf demselben Rechteck; live wird daraus `slot 1 typ=NONE` + `slot 2 typ=GENERIC` — ebenfalls **keine begehbare Tür**.

**(b) 12 von 96 gezeichneten Tür­marken sind Phantome** (kein DOOR-AOT innerhalb 50 Welt-Einheiten im laufenden Raum):

```
ROOM10C0->1080  pg3 r8 (178, 80)  Welt(1500,6400)
ROOM1100->1110  pg3 r6 (180, 91)  Welt(-26880,-10936)
ROOM1120->1080  pg4 r5 (159,119)  Welt(1300,6400)
ROOM1170->1140  pg5 r0 (152,101)  Welt(-21600,-27005)   <- die MESSAGE-Zone von oben
ROOM1170->1140  pg4 r3 (164,110)  Welt(-21600,-27005)   <- deren Zweitzeichnung
ROOM11A0->2070  pg0 r0 (140,124)  Welt(6500,-3850)
ROOM11A0->11A0  pg0 r0 (149,104)  Welt(15300,2750)
ROOM11B0->1260  pg0 r3 (224, 76)  Welt(-62,-20288)   (2x)
ROOM50C0->4020  pg10 r0 (160, 85) Welt(-7700,-9500)
ROOM6010->6000  pg12 r5 (263,115) Welt(-5215,-20638) (2x)
```

**(c) 118 Live-Türen haben überhaupt keine Entsprechung im Generator** — fast alle, weil ihre Zone kein Karten­rechteck zugeordnet bekommen hat (Seite 2: 14/18 Zonen zugeordnet, Seite 7: 13/20, Seite 9: 13/19). Diese Räume werden auf der Karte gar nicht gezeichnet, insofern ist es konsistent — aber die Karte ist unvollständig.

**(d) Treppen stimmen: 56 statisch = 56 live** (Aot_set Typ 12/13).

---

## 2. Die konkreten Doppelungen in ROOM1130 und ROOM1140

### Es sind ZWEI Zeichnungen: eine aus der Kachel, eine aus `s_map_marks`

Exhaustive Prüfung aller 83 Tür­marken gegen alle 75 Kachelsymbole (Kriterium: Marke liegt innerhalb `5 px + halbe Symbolgröße` um das Symbolzentrum) ergibt **genau zwei** Treffer, beide auf Seite 4 Rechteck 3:

```
Seite 4 r3  Marke (164, 89) kind 0 (Nordwand)  |  Kachelsymbol Zentrum (165, 92) 5x5 N  d = (-1,-3)
Seite 4 r3  Marke (164,110) kind 2 (Südwand)   |  Kachelsymbol Zentrum (164,107) 5x5 S  d = ( 0,+3)
```

Gerenderte Pixel (aus `ops_p4.txt`, Ops 128–140 bzw. 141–153):

| | Kachel malt | Port malt zusätzlich |
|---|---|---|
| Nordtür (→ROOM1130) | x 163..167, y 90..94 (MAP05.PIX, `uv(192,16)`, rel. 11..15/1..5, Palettenindex 4) | x 162..166, y 89..94 (13 FILL-Ops, rgb 48,192,48) |
| Südtür (→ROOM1140) | x 162..166, y 105..109 (rel. 10..14/16..20) | x 162..166, y 105..110 (13 FILL-Ops) |

Um 1 px versetzt und in einer anderen Farbe → genau das „komisch überlagert".
Bildbeleg: `p4_vergleich.png` (links nur Kachel, rechts mit Marken, 20-fach, Pixelraster).

### Wo die zweite Zeichnung herkommt

`gen_map_zones.py:1111-1127`, „MARKEN AUF DIE ZWEIT-ZEICHNUNG MITNEHMEN":

```python
for (froom, fzi, fband, fpg, fr) in floors:
    ...
    R0 = rects(v['pg'])[v['r']]; R1 = rects(fpg)[fr]
    zusatz.append(dict(v, pg=fpg, r=fr,
                       mx=v['mx'] + (R1[0] - R0[0]),
                       my=v['my'] + (R1[1] - R0[1])))
```

Diese Schleife kopiert eine Marke auf jedes Etagenblatt der Zone — **ohne `kachel_zeigt_tuer` erneut zu prüfen**. Der Unterdrückungs-Test steht nur im ersten Durchgang (`gen_map_zones.py:1042`).

Warum das gerade hier zuschlägt, ist byte-belegt: Seite 5 Rect 0 und Seite 4 Rect 3 sind **dieselbe Kachel-Region** (`uv(192,16)`, 48×24) aus **verschiedenen Dateien**. Pixel-Diff MAP05.PIX ↔ MAP06.PIX über diese Region: **22 von 1152 Pixeln unterschiedlich — und das sind exakt die beiden Türnischen**:

```
MAP05 (Seite 4 r3)                        MAP06 (Seite 5 r0)
 ##################                        ##################
 #1111111111#111#1#   <- Nordtür           #1111111111111111#   <- keine Tür
 #1111111111#111#1#                        #1111111111111111#
 #11111111111#11#1#                        #1111111111111111#
 #111111111111###1#                        #1111111111111111#
 ...                                       ...
 #111111111###1111...  <- Südtür           #1111111111111111...  <- keine Tür
 #111111111#11#111...                      #1111111111111111...
 #111111111#111#11...                      #1111111111111111...
```

Der Künstler hat den Pit auf dem Dach-Blatt **ohne** Türen gezeichnet (von dort oben erreicht man sie nicht) und auf dem 3F-Blatt **mit**. Der Port berechnet die Marken für Seite 5 (dort korrekt, weil die Kachel nichts malt) und kopiert sie dann blind auf Seite 4, wo die Kachel sie schon hat.

**Gegenprobe:** In den Nutzer-Räumen ROOM1130 und ROOM1140 selbst zeichnet der Port **keine einzige** eigene Marke — alle 4 Türen von ROOM1130 und alle 3 Datensätze von ROOM1140 werden korrekt unterdrückt:

```
ROOM1130 -> 1140  Marke pg4 r4 (160,147) unterdrückt von Symbol r6 (163,151) 5x9
ROOM1130 -> 1120  Marke pg4 r4 (152,122) unterdrückt von Symbol r5 (150,126) 5x5
ROOM1130 -> 1150  Marke pg4 r4 (144, 84) unterdrückt von Symbol r4 (147, 84) 5x5
ROOM1130 -> 1170  Marke pg4 r4 (164, 89) unterdrückt von Symbol r3 (165, 92) 5x5   <<<
ROOM1140 -> 1130  Marke pg4 r6 (160,151) unterdrückt von Symbol r6 (163,151) 5x9
ROOM1140 -> 1170  Marke pg4 r6 (166,110) unterdrückt von Symbol r3 (164,107) 5x5   <<< (2x)
```

Die mit `<<<` markierten Zeilen sind der Witz: **derselbe Durchgang wird im Raum ROOM1130/ROOM1140 richtig unterdrückt und dann über die Etagen-Kopie von ROOM1170 doch wieder gezeichnet.**

Die Karten­rechtecke auf Seite 4 überlappen sich stark, deshalb liegt die Doppelung optisch **in** ROOM1130 bzw. am Oberrand von ROOM1140:

```
r2 ROOM1150  x120..151  y 80..119
r3 ROOM1170.z1 x152..199 y 89..112     <- hier stehen die Doppelungen
r4 ROOM1130  x144..175  y 80..159      <- (164,89) und (164,110) liegen INNERHALB
r5 ROOM1120  x120..159  y119..158
r6 ROOM1140  x160..215  y110..165      <- (164,110) liegt auf der Oberkante
```

### Zweiter Kandidat für „in ROOM1140 ist die Eingangstür 2 mal vorhanden"

ROOM1140s Tür zu ROOM1130 ist in MAP05.PIX als **Doppel-Türblatt** gemalt (Katalog: `typ=TUER2`, `maske 5x9 doppelt`, Bildschirm (161,147) 5×9, Rechteck 6):

```
147 ######
148 #++++#     obere Türhälfte
149 #++++#
150 #+++#
151 ####       Knick
152 #+++#
153 #++++#     untere Türhälfte
154 #++++#
155 ######
```

Das sind **zwei Türblätter für einen Durchgang** — und es ist **Original-Grafik**, byte-identisch in MAP05.PIX. Der Port malt dort nichts dazu (`p4_1140tuer.png`: links ohne, rechts mit Marken — identisch). Falls der Nutzer das meint, ist es kein Port-Fehler; falls er die überlagerte Nische bei (164,105..110) meint, ist es der Fehler aus Abschnitt 2.

---

## 3. ROOM1170 — „die Tür unter der Treppe fehlt"

**Es ist die Südtür des Pits, Ziel ROOM1140, Welt (-21600,-27005).**
Kartenposition: Seite 4 Rect 3 (162..166, 105..109), auf dem Dach-Blatt Seite 5 Rect 0 (150..154, 96..100).

**Sie fehlt, weil der Treppen-Marker sie zudeckt.** Der Treppen-Marker ist ein **deckendes 7×7-Rechteck** (`re15_inv_screen.c:1567-1569`):

```c
bg->x = (int16_t)(mx - 3); bg->y = (int16_t)(my - 3);
bg->w = 7; bg->h = 7;
bg->r = 64; bg->g = 64; bg->b = 56;
```

Treppenmarke auf Seite 4 Rect 3 steht bei **(163,106)** → Grund deckt **x 160..166, y 103..109**.
Das Kachelsymbol der Tür liegt bei **x 162..166, y 105..109** → **25 von 25 Pixeln zugedeckt (100 %)**.

Die Marken werden vor den Rechtecken in die Op-Liste geschrieben, und die Liste wird von hinten gerastert (`inv_render_pc.c:842` `for (i = n - 1; i >= 0; i--) raster_op(&ops[i]);`) → früherer Eintrag liegt oben. Reihenfolge in `ops_p4.txt`: Op 127 = Treppengrund, Ops 141–153 = Türnische. Op 127 steht **früher** → der Treppengrund liegt über der Türnische **und** über der Kachel.

Auf Seite 5 (Dach-Blatt) genauso: Treppe (151,97) → Grund x 148..154, y 94..100; Türmarke (152,101) belegt x 150..154, y 96..101 → 5 von 6 Zeilen verdeckt (`p5_vergleich.png`).

Also: **weder unterdrückt noch wegdedupliziert — überpinselt.**

Zweiter, kleinerer Fall desselben Musters: Seite 9 Rect 12, Treppe (194,168) deckt das Symbol (191,169) 9×5 zu **21 von 45 Pixeln** zu.

**Systemisch:** die 25 Treppenmarken decken game-weit **1093 Kachelpixel** zu, davon **224 Wand-/Türlinien (Palettenindex 4)**. Die schlimmste Stelle ist Seite 2 Rect 9 — ein **16×16 px** großes Rechteck (ROOM1060, Treppenhaus), auf dem **5 Treppenmarken** liegen und 245 Pixel überdecken:

```
Seite  2 Rect  9 (16x16): 5 Treppenmarken bei (119,142) (114,141) (114,142) (119,141) (114,143)
Seite 12 Rect  5 (80x80): 4
Seite  4 Rect  0 (16x16): 2
Seite  5 Rect  0 (48x24): 2
Seite  4 Rect  3 (48x24): 2
Seite  3 Rect  9 (16x16): 2
Seite  8 Rect  7 (24x24): 2
Seite 11 Rect  3 (24x24): 2
```

Dazu: **13 Stellen**, an denen ein Treppengrund eine Port-**Tür**marke überdeckt (Liste in `an_c5.py`-Ausgabe), u.a. Seite 2/3 (118,138), Seite 4 (136,141) und (164,110), Seite 5 (152,101), Seite 7 (240,128) und (224,146).

---

## 4. Prüfung der Unterdrückungs-Toleranz (5 px + halbe Symbolgröße)

79 Marken werden unterdrückt. Abstand Marke → Symbolzentrum (Chebyshev):

```
1px: 3   2px: 6   3px: 37   4px: 17   5px: 8   6px: 3   7px: 5     (min 1, max 7)
Schwelle senken:  tol=5 -> 79/79   tol=4 -> 74/79   tol=3 -> 71/79
                  tol=2 -> 64/79   tol=1 -> 47/79   tol=0 -> 11/79
```

Die Toleranz ist also **nicht zu großzügig gewählt, sondern schlicht nötig** — die Projektion streut real bis 7 px. Eine Senkung auf 4 px würde 5 korrekt unterdrückte Marken wieder als Doppelung sichtbar machen.

### Falsch unterdrückt?

Kriterium: die Wandachse. Eine Marke trägt ihre Wandseite (`kind` 0=N/1=O/2=S/3=W, aus `snap_wall` an der gezeichneten Kachel), das Katalog-Symbol trägt seine (`wand`). Gleiche Wand = plausibel; gegenüberliegende Wand = derselbe Durchgang vom Nachbarraum gesehen = plausibel; **orthogonale** Wand = das Symbol gehört zu einer *anderen* Tür.

```
gleiche Wand      : 40
gegenüber         : 21
ORTHOGONAL        : 14   <- Verdacht auf Fehl-Unterdrückung
Symbol ohne Wandangabe: 4
```

Die 14 Verdachtsfälle:

```
ROOM10A0->1180  pg2 r9  (113,134) N  | Symbol (115,130) E   d=(-4,+2)
ROOM10A0->1230  pg2 r9  (113,134) N  | Symbol (115,130) E   d=(-4,+2)
ROOM10A0->11E0  pg2 r9  (113,134) N  | Symbol (115,130) E   d=(-4,+2)
ROOM11A0->20A0  pg0 r0  (153,115) O  | Symbol (157,110) S   d=(-6,+3)
ROOM11B0->11C0  pg0 r3  (168, 97) W  | Symbol (173,103) N   d=(-7,-8)
ROOM11F0->11E0  pg1 r8  (212,112) N  | Symbol (215,108) W   d=(-5,+2)
ROOM1230->1160  pg1 r3  (187, 78) S  | Symbol (190, 74) W   d=(-5,+2)
ROOM1230->11D0  pg1 r3  (187, 78) S  | Symbol (190, 74) W   d=(-5,+2)
ROOM1230->10A0  pg1 r3  (187, 78) S  | Symbol (190, 74) W   d=(-5,+2)
ROOM1230->1190  pg1 r3  (187, 78) S  | Symbol (190, 74) W   d=(-5,+2)  (3x)
ROOM30A0->3090  pg7 r3  (204,126) W  | Symbol (208,126) S   d=(-6,-2)
ROOM40A0->4080  pg8 r11 (171,159) N  | Symbol (175,163) O   d=(-6,-7)
```

Und ein **Symbol schluckt mehrere verschiedene Durchgänge** an 11 Stellen, am krassesten:

```
Seite 1 Symbol (190,74) 5x5 W  -> 6 verschiedene Durchgänge (ROOM1210->11E0, ROOM1230->1160/11D0/10A0/1190/11B0)
Seite 2 Symbol (115,130) 5x5 E -> 4 verschiedene Durchgänge (ROOM10A0->1180/1230/11E0, ROOM10B0->1260)
Seite 8 Symbol (141,163) 4x4 E -> 3 (ROOM4070->4040/50A0/5100)
Seite 4 Symbol (150,126) 5x5 E -> 3 (ROOM1120->1060, ROOM1120->1130, ROOM1130->1120)
```

Achtung bei der Bewertung: ROOM1230 und ROOM10A0 sind Räume, deren **Türen alle auf denselben Kartenpunkt projiziert werden** (Marken alle bei (187,78) bzw. (113,134)) — das ist ein **Projektions**-Defekt (keine Eichung, Bbox-Streckung), kein Toleranz-Defekt. Die Unterdrückung ist dort nur die Folge.

**Falsch stehengeblieben** (Durchgang anderswo unterdrückt, Marke bleibt trotzdem): 26 Fälle, darunter die zwei Seite-4-Doppelungen. Der Rest sind Marken auf einem **anderen Blatt** (z.B. ROOM1170->1130 auf Seite 5, wo die Kachel nichts malt) und ist damit korrekt.

`kachel_zeigt_tuer` (gen_map_zones.py:127) prüft gegen die **seitenweite** Symbolliste `GLYPHEN[pg]`, nicht gegen die rechteckweise `GLYPHEN[(pg,rect)]`, die derselbe Code in Zeile 114-117 extra aufbaut. 32 der 79 Unterdrückungen kommen dadurch von einem Symbol aus einem **anderen** Rechteck. Das ist überwiegend **richtig so** (der Künstler malt eine gemeinsame Tür nur in einer der beiden Kacheln), aber es ist genau der Mechanismus, über den die 14 orthogonalen Fälle durchrutschen.

---

## 5. Empfehlung

### F1 — die Doppelung (behebt Nutzerpunkt 1, belegt und gegengeprüft)

`re15_port/tools/gen_map_zones.py:1121-1127`: Unterdrückungs-Test in der Etagen-Kopie nachziehen.

```python
R0 = rects(v['pg'])[v['r']]; R1 = rects(fpg)[fr]
nx = v['mx'] + (R1[0] - R0[0]); ny = v['my'] + (R1[1] - R0[1])
if v['kind'] == 0 and kachel_zeigt_tuer(fpg, nx, ny):
    continue          # das Zweit-Blatt malt die Tür selbst
zusatz.append(dict(v, pg=fpg, r=fr, mx=nx, my=ny))
```

**Gegengeprüft:** Generator mit dieser Änderung neu laufen lassen, Diff gegen den ausgelieferten `re15_map_zones.h`:

```
251,252d250
<     {  4,  3,  164,   89, 0,  26, 255 },
<     {  4,  3,  164,  110, 2,  26, 255 },
```

Exakt die zwei Doppelungen fallen weg, **sonst ändert sich game-weit nichts**.

### F2 — die zugedeckte Tür (behebt Nutzerpunkt 3)

`re15_port/engine/src/re15_inv_screen.c:1565-1570`: den **deckenden 7×7-Grund** der Treppenmarke streichen. Er ist eine Port-Erfindung ohne Original-Beleg (RE1.5 liefert kein Treppen-Icon; das Original hat auf seinen Karten überhaupt keine Treppenmarke). Nur die drei Sprossen zeichnen, in Wandfarbe wie die Türnische (`wr/wg/wb`), dann liegen 15 statt 49 Pixel auf der Kachel und die Türnischen bleiben stehen.

Falls ein Kontrast-Hintergrund gewollt ist: ihn auf die 3 Sprossen-Zeilen/Spalten beschränken (5×5 statt 7×7 reicht **nicht** — das Symbol auf Seite 4 liegt bei (162..166,105..109) und die Treppe bei (163,106), Abstand 1 px), oder die Sprossen so verschieben, dass sie nicht auf einem Kachelsymbol liegen.

Zusätzlich: bei mehreren Treppenmarken im selben Rechteck (Seite 2 Rect 9 hat 5 auf 16×16 px) die Marken zusammenfassen — die Paarbildung in `gen_map_zones.py:994-1010` greift nur innerhalb eines Raumes, ROOM1060 und ROOM10A0 landen aber auf **derselben** Kachel.

### F3 — Phantomtüren (behebt Nutzerpunkt „Türen, die es nicht gibt")

`read_rdt` (gen_map_zones.py:164) kennt keine Verzweigungen und liest deshalb Türen, die im Auslieferungszustand `sce=1`/`sce=0` sind. Der Port hat die Wahrheit bereits zur Hand: **`scd_room_reenter` liefert die tatsächlichen AOT-Typen**. Zwei Wege:

1. (sauber, empfohlen) Die Marken-Tabelle **nicht** statisch erzeugen, sondern beim Raumbetritt aus `g_aot.slots[]` befüllen — dann stimmt die Karte auch, wenn ein Skript eine Tür später scharf schaltet (`Aot_reset`, ROOM1140 subScd[02] `@0x000C9A` `Aot_reset slot=1 sce=2`).
2. (kleiner Eingriff) Im Generator die IF/ELSE-Struktur mitlesen: ein `Door_aot_set`, dessen Slot im selben Sub kurz vorher per `Aot_set sce=1` auf dieselbe Geometrie gelegt wird, ist im Startzustand **kein** begehbarer Durchgang.

Vorsicht: die Kachel malt die Tür trotzdem (MAP05 r3 Süd) — das ist Original-Verhalten und darf **nicht** wegretuschiert werden. Es geht nur darum, keine **zweite** Marke draufzulegen.

### F4 — Toleranz

**Nicht senken.** Gemessene Streuung bis 7 px, tol=4 würde 5 korrekte Unterdrückungen kaputt machen. Stattdessen den Test **schärfen statt enger machen**: zusätzlich zur Distanz die Wandachse verlangen (Marke `kind` ∈ {gleiche, gegenüberliegende} Wand des Symbols). Das entfernt die 14 orthogonalen Fehl-Unterdrückungen, ohne einen der 61 richtigen zu verlieren.

---

## Offen / nicht entscheidbar aus diesen Daten

* **Welche der beiden Lesarten der Nutzer bei ROOM1140 meint** (die überlagerte Nische bei (164,105..110) oder das originale Doppel-Türblatt bei (161,147) 5×9). Nächster Weg: gdigrab-Aufnahme des echten Fensters in ROOM1140 mit offener Karte und Pixelvergleich gegen `karte_p4.png`.
* **ROOM1230 / ROOM10A0**: alle Türen projizieren auf denselben Punkt. Das ist ein Eichungs-/Projektionsproblem (Bereich B), nicht Marken-Logik — hier nur als Ursache der Sammel-Unterdrückungen vermerkt.
* **Ob das Original die Rechtecke in derselben Reihenfolge stapelt wie der Port** (auf Seite 4 überdeckt ROOM1130s Kachel die linke Spalte von ROOM1140s Doppeltür-Nische bei x=160). Nächster Weg: `FUN_800473f8` Rect-Schleife `@0x800475f8-61c` gegen die Op-Reihenfolge in `inv_render_pc.c` halten.
