# ROOM10F0 Cut 4/5 — „Leon ist da großteils transparent"

**Nutzer-Marke 2026-09-21**, `befund.log` Zeile 15602, Bild
`re15_port/build/platform/pc/befund_10F0_F335_marke1.bmp` (Communication Room).
Bilder und Messreihen: `analysis/befunde_2026-09-21/10f0-quader-silhouette/`.

---

## Kurzfassung

Der Nutzer hat recht, und die Ursache ist **nicht** die grobe Kastenform der
Bürostuhl-Quader. Sie ist die **Tiefschwarz-Regel** `raum.py:327` + `raum.py:354-357`:
jeder Hintergrundpunkt mit Kanalsumme < 45, der in der projizierten Silhouette eines
Möbel-Quaders liegt, bekommt **dessen** Tiefe. In ROOM10F0 ist der Raum dunkel, die
Rückwand ist holzvertäfelt und tiefschwarz — also wird **die Wand hinter dem Stuhl zum
Vordergrund erklärt**.

| | Cut 4 | Cut 5 |
|---|---:|---:|
| Maskentexel ausgeliefert | 12068 | 9285 |
| davon aus der Freistellung des Nutzers | 2131 (17,7 %) | 3576 (38,5 %) |
| davon **erst durch die Tiefschwarz-Regel** | **9937 (82,3 %)** | **5709 (61,5 %)** |
| davon heller als Kanalsumme 45 | 0 | 0 |

An der Marke (`F335`, pos `(-448,0,14087)`) sind von 837 gezeichneten Figurpunkten
**426 = 50,9 %** verdeckt. Davon gehen **272 auf den Stuhl** (der steht wirklich vor
ihm, das ist richtig) und **154 allein auf die Tiefschwarz-Zugabe**. Der waagerechte
Schnitt quer durch die Hüfte, den man im Bild sieht, liegt in den Bildzeilen **80–84**
— und dort trägt die Freistellung des Nutzers **0** Punkte bei, die Verdeckung ist dort
zu **100 %** Tiefschwarz.

Über die 30 protokollierten Bilder, die der Nutzer wirklich in Cut 4 stand:
**12 Bilder mit mehr als der halben Figur ausgeblendet**, 17,00 % aller gezeichneten
Figurpunkte weg. Ohne die Tiefschwarz-Zugabe: **0 Bilder** über der Hälfte, 10,71 %.

Die drei Freistellungen des Nutzers sind in Ordnung — sie sind exakt die Stühle. Der
Vorschlag ist deshalb, die Zugabe fallen zu lassen, nicht die Silhouette neu zu bauen.
Eine Rückfrage bleibt offen (§8).

---

## 0. Womit gemessen wurde, und wie weit es trägt

### 0.1 Die Schiene

`abnahme.py` hat kein Kriterium für „Figur ausgeblendet" — und die Größe, mit der alle
bisherigen PRI-Messungen gerechnet haben, ist ein **Rechteck**, nicht die Figur. Deshalb
eine neue Sonde:

* `re15_port/tests/unit/probe_r22_10f0_figur.c` (Registrierung:
  `re15_port/tests/unit/probes/r22_10f0.cmake`)
* Sie lädt `PLD/PL00.MD1` + `PLD/PL00.EMR`, posiert das Skelett
  (`re15_skel_compute_pose`), transformiert jeden Vertex mit **demselben** Weg wie der
  Renderer (`main.c` `PROJECT_VERT`: `view × RotY(yaw) × Bone`, GTE-Divide,
  `re15_camera_compose_view_bone`) und rastert die Dreiecke in einen 320×240-Puffer mit
  Kamera-z je Bildpunkt.
* Dagegen steht die **texelgenaue** Maskendeckung: `MASKS/ROOM10F0.MSK` →
  `re15_pri_msk_section_offset` → `re15_pri_parse_section` (`draw_count` Rechtecke) →
  `MASKS/ROOM10F0_PRI04.TIM` (8 bpp + CLUT), Palettenindex 0 = durchsichtig
  (`bg_pc.c:145` `pri_publish_tim`). Das ist derselbe Leseweg wie in
  `test_pri_silhouette.c`.
* Urteil je Bildpunkt nach dem Original-Modell aus `re15_pri.h`:
  verdeckt ⇔ `depth < (1023·vz) >> 16` (`@0x8002565c` `sra v1,v1,4` auf
  `otz = AVSZ3` mit `ZSF3 = 341 @0x80066c70`; Maskenwort `@0x80039658` ×1).

### 0.2 Gegenprobe gegen das Bild des Nutzers — die Schiene erfindet nichts

Der Abzug `befund_10F0_F335_marke1.bmp` (960×720) gegen den reinen Hintergrund
desselben Winkels, 3×3-Mittel auf 320×240, Diff-Schwelle 10 auf der Kanalsumme:

```
Sonde: gezeichnet 837, davon sichtbar 411
Bild : 1005 veränderte Bildpunkte, davon 371 auf den 411 Sonden-Punkten,
       40 Sonden-Punkte ohne Bildänderung (dunkle Hose vor dunkler Wand)
```

Zeile für Zeile:

```
 Zeile | Sonde sichtbar | Bild verändert        Zeile | Sonde | Bild
    72 |       22       | 20                       80 |   7   |  7
    76 |       20       | 19                       81 |   2   |  2
    78 |       19       | 17                       84 |   4   |  3
    79 |       17       | 17                       88 |   2   |  2
```

Der Abriss bei Zeile 80 (20 → 7 → 2) steht im Bild und im Modell an derselben Stelle.
`werkzeug/b1_gegenprobe.py`.

### 0.3 Abdeckung dieser Schiene — ehrlich

* **2 von 439 Winkeln mit Masken** im Spiel = **0,46 %**. Innerhalb ROOM10F0 deckt sie
  die Anker-Zonen beider Winkel vollständig ab: 1287 begehbare Standplätze in Cut 4,
  2301 in Cut 5 (Klemmpfad `re15_collision_constrain`, PR = 450, Raster 200).
* Dazu die **50 wirklich gespielten Bilder** aus `befund.log` (30 in Cut 4, 20 in
  Cut 5) — das ist der Teil ohne Standplatz-Ersatzmaß.
* Die Anker-Zone eines Winkels ist der RVD-Eintrag `von=<cut> nach=0`; für Cut 4 ist das
  Eintrag 11, `x-2400..3600 / z3300..16800` — deckungsgleich mit der Anker-Angabe des
  Zensus Runde 19.
* **Posen-Empfindlichkeit** (kein freier Parameter, der das Ergebnis macht): über die
  Keyframes 0/1/2/5/10/20/30 liegt der Anteil an der Marke bei
  50,9 / 51,1 / 50,8 / 50,3 / 51,3 / 49,0 / 45,5 %.

---

## 1. Welches Objekt blendet Leon aus?

### 1.1 Die Rechtecke

`MASKS/ROOM10F0.MSK` (R15M, Kopf 12 B, danach `offset[cut]`; Cut 4 @`0x0EAC`,
Cut 5 @`0x16E4`) liefert für Cut 4 **105 Gruppen zu je 1 Maske, 105 deklariert, 105
gezeichnet**. Die zwölf Rechtecke, die den Kasten der Marke berühren, bilden zwei
deckungsgleiche 3×3-Raster über `x76..115 / y80..124`:

```
 0: dst(76,80) 16x16 t=154 src(236, 48)      51: dst(76,80)  24x24 t=154 src( 92,48)
 1: dst(92,80) 16x16 t=154 src( 48,168)      52: dst(100,80) 16x24 t=163 src(238,24)
 2: dst(108,80) 8x16 t=160 src(243, 72)      53: dst(76,104) 24x21 t=142 src( 24,168)
 3: dst(76,96) 16x16 t=140 src( 64,168)      54: dst(100,104)16x21 t=158 src(237,144)
 …
```

### 1.2 Was im ORIGINAL-Hintergrund an dieser Stelle steht

`10f0-quader-silhouette/02_hintergrund_block.png` — selbst angesehen: ein **Bürostuhl**
mit heller Lehne, links davon eine Schreibtischkante mit Telefon, **rechts und über der
Lehne die dunkel holzvertäfelte Rückwand des Raums**, darunter der Bodenbelag.

`03_maskendeckung_block.png` zeigt, was die Maske daraus macht: einen geschlossenen
Klumpen von **1244 opaken Texeln** über `x76..115 / y80..124`, der die Vertäfelung und
den Boden mitnimmt.

### 1.3 Herkunft jedes einzelnen Texels

`05_herkunft_block.png` / `04_herkunft_c4.png` — **grün** = Freistellung des Nutzers,
**rot** = erst durch die Tiefschwarz-Regel dazugekommen:

```
Cut 4 gesamt          12068 Texel
  Nutzer-Lasso         2131
  Zugabe               9937   davon Tiefschwarz (Kanalsumme < 45)  9937 = 100,0 %
                              davon heller als 45                     0

Block x76..115/y80..124  1244 Texel
  Nutzer-Lasso            541
  Zugabe                  703   (56,5 %), ausnahmslos Tiefschwarz
```

Die Freistellungen selbst (`pri/STAGE1/10F0/04_01.png` @ (88,84),
`04_02.png` @ (68,109), `04_03.png` @ (59,218), Alpha > 110) sind **exakt die drei
Stühle** — eine saubere, schmale Silhouette, 2131 Punkte. Die Rechtecke sind also
**nicht** grob: `test_pri_silhouette` prüft für 77 Cuts, dass die gelieferte Deckung
bitgenau die Ziel-Silhouette ist. Grob ist die **Ziel-Silhouette selbst**.

### 1.4 Die Tiefe ist zusätzlich zu nah

Raycast gegen die 12 soliden Typ-1-SCA-Zellen des Raums
(`geom.kollisionstiefe_schnell`, Einheit = Maskentiefe):

| Bildpunkt | Maskentiefe | Wand dahinter |
|---|---:|---:|
| (100, 82) | 154 | 173 |
| (105, 85) | 154 | 175 |
| (110, 88) | 160 | 176 |
| (112, 92) | 160 | 178 |

Maskentiefe 154 sperrt ab Kamera-z `155·65536/1023 = 9931`. Leon steht an der Marke bei
Kamera-z 9970…11141. Mit der Tiefe der Wand selbst (173) läge die Schranke bei 11146 —
**oberhalb** seines gesamten Körpers, er wäre dort vollständig frei.

---

## 2. Wie müsste die Silhouette aussehen? — und das Nullmodell

Die Segmentierung ist **nicht** das offene Problem: der Nutzer hat sie schon geliefert.
Gemessen wird deshalb gegen ein **Nullmodell** (gar keine Maske) und zwei Gegenmodelle,
alle gegen **dieselbe** gerasterte Figur (`werkzeug/a8_gegenmodelle.py`):

| Modell | Texel | Figur an der Marke verdeckt |
|---|---:|---:|
| Nullmodell (keine Maske) | 0 | 0 von 837 = **0,0 %** |
| **ausgeliefert** | 12068 | 426 = **50,9 %** |
| nur Nutzer-Lasso | 2131 | 272 = **32,5 %** |
| Lasso + Tiefschwarz-Saum ≤ 7 | 2963 | 418 = **49,9 %** |
| alles außer Tiefschwarz | 2131 | 243 = 29,0 % |

Die 32,5 % des Lasso-Modells sind **richtig**: der Quader `Buerostuhl x-1600 z12200`
(`auswahl.json`, ROOM10F0/"4") liegt bei Kamera-z 8484…10640 → Tiefe 132…166, die Figur
bei Bucket 155…173. Der Stuhl steht wirklich vor ihm und muss seine Beine decken.
`06_marke_herkunft.png` zeigt die Trennung: **orange** = Stuhl (richtig),
**rot** = Tiefschwarz (falsch). `07_marke_gegenmodell_lasso.png` zeigt, wie die Szene
ohne die Zugabe aussieht — eine normale RE-Szene, Leon steht hinter dem Stuhl.

Bemerkenswert und wichtig für die Abhilfe: **`Lasso + Saum ≤ 7` hilft fast nichts**
(49,9 %). Die schädlichen Texel liegen im Median **3** Bildpunkte (min 1, max 9) neben
der Freistellung — es ist die Wand **direkt** über der Lehne. Ein Abstandsfilter trennt
hier nicht.

---

## 3. Wie viel Figur geht heute verloren?

### 3.1 An der Marke

```
MARKE punkte=837  kasten=x94..116,y55..117
MARKE verdeckt=426 frei=411 anteil=50.9%
Zeilen 55..79 : 0 von 17..22 verdeckt      (Oberkörper, frei)
Zeile  80     : 10 von 17                  (Beginn des Blocks)
Zeilen 89..112: 100 % verdeckt
```

### 3.2 Auf dem wirklich gespielten Weg (`befund.log`, 104 Zeilen, je 15 Bilder)

```
PFADSUMME cut=4  davon_cut=30  mitkontakt=22  ueberhalb=12
                 figurpunkte=37570 verdeckt=6386  anteil=17,00 %
```

**12 der 30 Bilder** zeigen weniger als die halbe Figur. Die schlechtesten:
F178 55,2 %, F208 52,2 %, F193 50,7 %, dann elf Bilder in Folge bei ~50 %, weil der
Nutzer dort stehen blieb.

Mit dem Lasso-Modell: `mitkontakt=19, ueberhalb=0, anteil=10,71 %`.

### 3.3 Über die begehbaren Standplätze der Anker-Zone

| | ausgeliefert | nur Lasso |
|---|---:|---:|
| Standplätze in der Anker-Zone Cut 4 | 1287 | 1287 |
| davon mit Maskenkontakt | 152 | 21 |
| davon mehr als die Hälfte ausgeblendet | **1** | 0 |
| Anteil aller Figurpunkte | 0,20 % | 0,02 % |

**Das ist der ehrliche Teil:** über die ganze Zone gemittelt ist der Fehler klein — er
ist **örtlich**. Genau ein Rasterplatz (−400/14200, 51,5 %) und seine unmittelbaren
Nachbarn (−200/14200 32,9 %, −200/14000 27,9 %) sind betroffen, und der Nutzer stand
dort, weil das die Stelle ist, an der man in diesem Winkel an den Schreibtischen
vorbeigeht. Ein Flächenmittel über Standplätze hätte diesen Fall nie gemeldet — deshalb
steht in §3.2 der gespielte Weg an erster Stelle.

---

## 4. Gilt das auch für Cut 5?

Ja, dieselbe Ursache, **milder**:

```
Cut 5: Deckung 9285 | Lasso 3576 | Zugabe 5709 | davon Tiefschwarz 5709 (100 %)
PFADSUMME cut=5  davon_cut=20  mitkontakt=11  ueberhalb=0  anteil=8,04 %
   schlechteste Bilder: F1198 45,7 %, F1288 43,8 %, F1273 37,7 %, F1213 37,3 %
mit nur-Lasso:          mitkontakt=11  ueberhalb=0  anteil=5,99 %
Anker-Zone: 2301 Plätze, 516 mit Kontakt, 0 über der Hälfte, 1,03 % → 0,08 %
```

`08_herkunft_c5.png`: dasselbe Bild — die Freistellung ist der Stuhlblock in der
Bildmitte, rot ist die dunkle Hälfte der Schreibtische, des Regals und der Wand.

**Kein Bild in Cut 5 überschreitet die Hälfte.** Der Winkel wäre allein kein Befund
gewesen; er trägt denselben Defekt und gehört mit repariert.

---

## 5. Die Tiefschwarz-Regel — Anteil und Beleg

`re15_port/tools/maske/raum.py`:

```python
327:  _dk = _bgL.astype(int).sum(2) < 45
336:  # 1) KUNST-SAUM: Dunkel-Pixel im Abstand <= 7 zur Kunst einer Zelle
354:  _rest = _dk & (_dunkel_label < 0)
355:  for _ki, (_vzq, _trq) in _sil.items():
356:      _m = _rest & _trq & (_vzq < _naechstes)
357:      _dunkel_label[_m] = _ki
```

Regel 2 gibt **jedem** dunklen Bildpunkt, der in **irgendeiner** Möbel-Silhouette liegt,
die Tiefe der nächsten solchen Zelle — ohne Abstandsschranke zur Kunst. Die
Stuhl-Quader sind 1500×1500 groß und **1950 hoch** (`auswahl.json`); projiziert reichen
sie weit über die Lehne hinaus an die Wand. In einem dunklen Raum mit
Holzvertäfelung heißt das: die Wand wird Vordergrund.

Antwort auf die gestellte Frage: **die Regel ist nicht nur beteiligt, sie ist die
Ursache.** 82,3 % der ganzen Maske von Cut 4 und 56,5 % des Blocks über Leon stammen
aus ihr, und die Zeilen 80–84, in denen die Figur abreißt, zu 100 %.

Die Begründung im `_warum`-Block von `auswahl.json` („die schwarzen Lehnen laufen bis
h≈−1950 UND das Nutzer-Lasso hat dort schwarz-auf-schwarz-Lücken") ist für den **Saum**
plausibel; sie trägt aber nicht für Regel 2, die 9105 der 12068 Texel liefert — Punkte,
die weiter als 7 Bildpunkte von jeder Kunst entfernt liegen.

---

## 6. Warum hat Runde 19 den Fall nicht gesehen?

Drei unabhängige Gründe, alle nachprüfbar:

### 6.1 Es gab kein Symptom — und das war das Argument

Die Synthese sagt es wörtlich (`pri-runde19/SYNTHESE.md` §6.2/§6.3):
„Kein Symptom: `grep -c "R10F0" befund.log` = **0**; der Nutzer war 2026-09-21 nie in
ROOM10F0." und „Kein Bildbeleg existiert: ROOM10F0 kommt in 11470 protokollierten
Bildern nicht vor."

Das ist ein Schluss aus **fehlendem** Beleg, kein Beleg. Heute enthält `befund.log`
**104** Zeilen `R10F0`. Der Raum war nicht in Ordnung, er war **ungespielt**.

### 6.2 Die gemessene „Figur" ist halb so hoch wie die gezeichnete

Die Synthese nennt die Schwäche selbst („die ‚Figur' in den Belegbildern ist der
rechteckige Körperkasten, nicht die Silhouette"), aber unterschätzt sie. Gemessen am
posierten Modell:

```
MODELLHOEHE kf=0  yMin=-3009  yMax=59  hoehe=3068   (Sonden-Näherung 1700, Faktor 1,80)
```

Die Näherung steht an vier Stellen:
`main.c:5264` (`1700`, der Kasten im Befund-Log), `abnahme.py:30` (`KOPF = 1500`),
`test_pri_kopfschnitt.c:52` und `test_pri_eingemessen.c:31` (je `KOPF_HOCH 1500`).

Folge an der Marke: das Log meldet `kasten=x99..121,y81..118`, die **gezeichnete** Figur
liegt bei `x94..116,y55..117`. Der Messkasten fängt genau dort an, wo die Figur schon
zur Hälfte vorbei ist — der ganze frei gebliebene Oberkörper (Zeilen 55–80) liegt
**außerhalb** des Kastens, und der Kasten reicht unten 1 Zeile über die Füße hinaus. Ein
Maß, das „wie viel Figur ist weg" beantworten soll, sieht mit diesem Kasten nur die
untere Hälfte.

### 6.3 Das eigene Kriterium der Runde hätte den Fall gemeldet

Runde 19 rechtfertigte Verdeckung mit „solides Sperrfeld auf dem Sehstrahl" und kam auf
3470 von 73224 Punkten (4,74 %) ohne Rechtfertigung. Dasselbe Kriterium, an der Marke
auf die gerasterte Figur angewandt:

```
verdeckte Figurpunkte gesamt   426
  davon MIT solider Wand davor   2
  davon OHNE                   424  =  99,5 %
```

Der Unterschied ist kein Methodenstreit, sondern Mittelwert gegen Ort: 4,74 % über 604
Plätze, 99,5 % an dem einen, an dem der Nutzer stand.

**Merksatz für die nächste Runde:** ein Flächenmittel über Standplätze ist kein Maß für
„sieht kaputt aus". Was fehlt, ist eine Kennzahl **je Ort** mit einer Schranke —
z. B. „kein begehbarer Standplatz der Anker-Zone darf mehr als X % der gezeichneten
Figur verlieren". Diese Kennzahl liefert die Sonde jetzt (`PLATZ`-Zeilen,
`ueberhalb`-Zähler).

### 6.4 Nachtrag — diese zwei Cuts stehen außerdem außerhalb des Silhouetten-Riegels

`test_pri_silhouette` läuft über die `MASKS/*_PRI##.PBM`-Dateien
(`test_pri_silhouette.c:104`). Gemessen:

```
Cut-TIMs gesamt: 99   mit Original-Sektion im RDT: 0
ohne Original, MIT PBM : 77     ohne Original, OHNE PBM: 22
```

Der Test meldet selbst „77 PBM-Cuts: 77 bitgenau". Die 22 ungedeckten sind
**ROOM10F0 C4/C5, ROOM1100 C1/C2, ROOM2090 C0–C9, ROOM2091 C0–C7** — genau die Cuts,
die Runde 19 als „nicht anfassen" führte. Die Begründung im Testkopf („Cuts mit
Original-Sektion tragen keine PBM") trifft auf sie nicht zu: **keiner** dieser 99 Cuts
hat eine Original-Sektion. Ihnen fehlt schlicht der Riegel. `test_pri_kopfschnitt`
braucht zusätzlich die `.STAND`-Datei, die diesen 22 ebenfalls fehlt.

---

## 7. Die Kapazitäts-Hypothese: bestätigt als Verschwendung, widerlegt als Erklärung

Die Vermutung der Aufgabenstellung war, die Enge an der 105er-Grenze
(`RE15_PRI_MAX_MASKS_PER_CUT`, `re15_pri.h`) habe zu groben Kästen gezwungen.

**Bestätigt ist die Verschwendung** — und zwar schärfer als vermutet, weil sie auch auf
Texel-Ebene gilt (die Rechtecke haben verschiedene `src`-Adressen, tragen aber dasselbe
Bild):

```
Cut 4: 105 Rechtecke, 79 verschiedene Lagen, 26 Doppel (Lage+Größe+Tiefe gleich),
       davon 26 auch TEXELGLEICH — 4675 Texel doppelt gezeichnet
Cut 5: 105 Rechtecke, 100 verschiedene Lagen, 5 Doppel, davon 4 texelgleich
```

**Widerlegt ist die Erklärung.** Die Grobheit sitzt nicht in der Zerlegung: die
gelieferte Deckung ist punktgenau die Ziel-Silhouette (für die 77 gepinnten Cuts
bitgenau nachgewiesen, und für C4 hier von Hand nachgerechnet: 1244 opake Texel im
Block, nicht 1800 Rechteckfläche). Grob ist die **Ziel-Silhouette**, und die ist ein
Ergebnis der Tiefschwarz-Regel, nicht des Budgets. Ein größeres Budget hätte den Fehler
nicht behoben; ein kleineres hätte ihn nicht verursacht.

Nebenbefund: die 26 Doppel sind trotzdem zu entfernen. Sie sind die bitgleichen
Zwillinge aus Quader und Lasso, die Schritt 7 der Synthese schon benannt hat
(„9 beitragslose Kästen"), und sie kosten ein Viertel der Kapazität.

---

## 8. Vorschlag — und die offene Rückfrage

### 8.1 Was ich vorschlage (Bau-Lauf, nicht in dieser Phase)

1. **Regel 2 der Tiefschwarz-Etikettierung (`raum.py:354-357`) abschalten**, den
   Kunst-Saum (Regel 1, Abstand ≤ 7 **innerhalb derselben Zelle**) behalten. Wirkung
   gemessen: Cut 4 12 → 0 Bilder über der Hälfte, 17,00 → 10,71 % auf dem gespielten
   Weg; Cut 5 8,04 → 5,99 %.
   ⛔ **Ehrlich:** das ist eine Regel für **alle** Räume, nicht nur ROOM10F0. Ihre
   Wirkung in den anderen 97 Cuts habe ich **nicht** gemessen. Das gehört vor den
   Umbau, und es geht mit derselben Sonde.
2. **Die 26 texelgleichen Doppel in Cut 4 und die 4 in Cut 5 entfernen.**
3. **Für ROOM10F0 C4/C5 eine `.PBM` und `.STAND` schreiben**, damit sie unter
   `test_pri_silhouette` und `test_pri_kopfschnitt` fallen — und dasselbe für die
   übrigen 18 ungedeckten Cuts prüfen.
4. **Neue Abnahme-Kennzahl je Standplatz** („verlorener Figuranteil"), Schranke pro
   Anker-Zone statt Flächenmittel. Die Sonde liefert sie; die Schranke ist eine
   Nutzer-Entscheidung, keine Messgröße — deshalb schlage ich keine Zahl vor.

### 8.2 Die Rückfrage an den Nutzer

Bild: `10f0-quader-silhouette/09_nachfrage_c4.png` (ganzer Winkel, gelber Rahmen um den
Block) und `10_nachfrage_zoom.png` (Ausschnitt, 12×).
**Grün** = was du schon freigestellt hast. **Rot** = was die Tiefschwarz-Regel
zusätzlich als Vordergrund führt.

Die Frage ist eng: *Ist an irgendeiner Stelle im Roten echter Vordergrund, den das Lasso
übersehen hat?* Konkret vermute ich zwei Kandidaten, die ich nicht selbst entscheiden
will:

* die **schwarzen Lehnenspitzen** der Stühle (schwarz auf schwarz — der Grund, aus dem
  die Regel 2026-09-09 überhaupt eingeführt wurde),
* die **dunklen Schreibtischkanten** links im Bild, die vor dem Stuhl stehen.

Wenn ja: eine nachgezogene Freistellung für C4 und C5, die diese Teile enthält — dann
kann Regel 2 ersatzlos weg. Wenn nein: Regel 2 weg, ohne Ersatz.

---

## 9. Was ich NICHT gemessen habe

* **Die Wirkung von Vorschlag 8.1.1 auf die anderen 97 Cuts.** Der Befund hier trägt nur
  für ROOM10F0 C4/C5.
* **Die Pose des Nutzers an der Marke.** Die Sonde posiert `PL00.EMR` Keyframe 0; der
  Anteil schwankt über sieben Keyframes zwischen 45,5 und 51,3 % (§0.3). Welcher
  Keyframe an F335 lief, steht nicht im Log.
* **Die Etikettierung Regel 1 gegen Regel 2 im Original-Bauweg.** Meine Trennung
  „Saum ≤ 7" ist eine **Näherung** von `raum.py` Regel 1 (der Bau verlangt zusätzlich,
  dass der Punkt in der Silhouette **derselben** Zelle liegt). Für die Aussage in §5
  reicht sie — die schädlichen Texel liegen im Median 3 Bildpunkte neben der Kunst und
  fallen damit ohnehin in beide Regeln.
* **Die Bänder.** Der Standplatz-Sweep läuft über beide Bänder des Raums (1336 Plätze in
  Band 0, 1452 in Band 1). Die Marke liegt in Band 0; welches Band in ROOM10F0 wirklich
  begehbar ist, habe ich nicht geprüft.
* **Es wurden keine Masken-Assets geschrieben.** Diese Phase misst.

---

## Dateien

```
analysis/befunde_2026-09-21/10f0-quader-silhouette/
  01_marke_nutzer.png            der Abzug des Nutzers, Ausschnitt um Leon
  02_hintergrund_block.png       reiner Hintergrund derselben Stelle
  03_maskendeckung_block.png     die ausgelieferte Deckung darüber
  04_herkunft_c4.png             ganzer Winkel: grün Lasso, rot Tiefschwarz
  05_herkunft_block.png          derselbe Ausschnitt, 10×
  06_marke_herkunft.png          Figur: blau frei, orange Stuhl, rot Tiefschwarz
  07_marke_gegenmodell_lasso.png dieselbe Figur ohne die Tiefschwarz-Zugabe
  08_herkunft_c5.png             Cut 5
  09_nachfrage_c4.png            Rückfrage an den Nutzer (ganzer Winkel)
  10_nachfrage_zoom.png          Rückfrage, Ausschnitt 12×
  messung/   c4_ausgeliefert.txt, c4_gegenmodell_nur_lasso.txt,
             c5_ausgeliefert.txt, c5_gegenmodell_nur_lasso.txt,
             pfad_10f0_aus_befundlog.txt
  werkzeug/  msk.py (R15M+TIM-Leser) und die Auswertungen a5/a8/a9/b1/b2/c1..c4
re15_port/tests/unit/probe_r22_10f0_figur.c
re15_port/tests/unit/probes/r22_10f0.cmake
```
