# Runde 19 / Marke 1 — ROOM10D0 Cut 1 (F162)

Nutzer-Marke F162, `befund.log` Zeile 13147, Bild `befund_10D0_F162_marke1.bmp`.
Spieler `(3575, 0, -4568)`, Bildschirmkasten `x150..170 y82..117`, 69 Masken.

**Befund in einem Satz:** Leons Beine liegen sichtbar AUF der grünen Tischplatte und
werden dann an der Tischzarge abgeschnitten — weil die Tischplatte und das Gestell
**desselben** Klapptischs Tiefen bekamen, die 5732 Welteinheiten auseinanderliegen und
den Spieler genau dazwischen einschließen.

---

## 1. Reproduktion am BILD

| Bild | Was es zeigt |
|---|---|
| `marke1-10D0-C1/01_vollbild_x3.png` | das Befundbild, 320x240 logisch, 3x |
| `marke1-10D0-C1/02_spieler_zoom.png` | Zuschnitt `x120..200 y75..160`, 8x — Beine auf der Platte, Abschnitt an der Zarge |
| `marke1-10D0-C1/04_hintergrund_zoom.png` | derselbe Zuschnitt aus dem Original-Hintergrund (ohne Figur) |
| `marke1-10D0-C1/05_teilung_zoom.png` | die Zwei-Wege-Teilung: **rot** = Platte (493 px), **blau** = Gestell (528 px) |
| `marke1-10D0-C1/06_befund_zoom.png` | Befundbild, gleicher Zuschnitt, zum Vergleich mit 04 |
| `marke1-10D0-C1/07_fehler_markiert.png` | **rot ausgefüllt = die 133 Figurpunkte, die auf der Tischplatte liegen**; grüner Umriss = Platte, blauer Umriss = Gestell |
| `marke1-10D0-C1/09_hintergrund_raster.png` | `x138..182 y82..152` mit 5-px-Raster, 12x |

Der Hintergrund kommt aus dem Engine-Dekoder selbst
(`tests/unit/probe_bg_dump.c` → `build/bg_ppm/ROOM10D001.ppm`, dieselbe
`bss_vlc`/`bss_mdec`-Kette wie im Spiel), nicht aus einem Fremdextrakt.

### Diff Befundbild ↔ Original-Hintergrund

Maß: `max|Befund − Hintergrund|` je Kanal. Die Maske blittet Hintergrund-Texel zurück,
also ist „Figur gezeichnet" = Abweichung > 16.

| Schwelle | Punkte im Vollbild | Anteil |
|---|---|---|
| > 0 | 1451 | 1,9 % |
| > 4 | 699 | 0,9 % |
| > 16 | 481 | 0,6 % |
| > 32 | 218 | 0,3 % |

### Zensus im Körperkasten `x150..170 y82..117` (756 Punkte)

| Größe | Wert |
|---|---|
| Figur gezeichnet | 255 |
| Platten-Region im Kasten | 290 |
| Gestell-Region im Kasten | 162 |
| **Figur ÜBER der Platte** | **133** (52 % der gezeichneten Figur) |
| Figur über dem Gestell | **0** |
| Gestell ohne Figur (verdeckt also vollständig) | 162 von 162 |

Zeilenprofil (Punkte in `x150..170`): die Platte deckt `y88..104` ab und dort stehen
durchgehend 3..13 Figurpunkte sichtbar darauf; ab `y105` übernimmt das Gestell und die
Figur ist auf 0..3 Punkte reduziert. Zwischen `y110` und `y114` blitzen 3..11 Punkte
**durch die Zwischenräume des Gestells** — das ist richtig, ein Rahmen ist durchsichtig,
und die Freistellung des Nutzers hat nur die Metallstäbe ausgeschnitten.

---

## 2. Die Ursache, mit Zahlen

Kamerasatz Cut 1: `R = [-4056,0,-571,-165,3922,1166,546,1178,-3884]`, `t = [2955,1074,7239]`,
`H = 208`. Kamerastandort Welt `(2003, -3110, 6972)`. Gegenprobe der Projektion gegen das
Log: `(3575,0,-4568) → vz 12047` (Log-Fuß 12057), `(3575,-1400,-4568) → vz 11644`
(Log-Kopf 11625) — die Projektion stimmt auf 10..20 Einheiten.

`analysis/esp_masken_2026-09-03/auswahl.json`, `ROOM10D0` Cut 1: **eine** Nutzer-Freistellung
`pri/STAGE1/10D0/01.png` (Lage `x=124 y=87`), per Farbregel in zwei Tiefen-Objekte geteilt.

| | Modell (vorher) | Standpunkt(e) Welt | Kamera-z | Tiefenstufe |
|---|---|---|---|---|
| Platte (493 px, `x129..174 y88..108`) | `"aufrecht": true` | **(4306, −5736)**, EIN Punkt | 12885..13252 | **201..206** |
| Gestell (528 px, `x126..176 y88..146`) | Spaltenregel, `bodenkante [147,175]` | (3522,−355) … (2667,−3675) | 7153..11079 | **111..172** |
| **Spieler an der Marke** | — | (3575, 0, −4568) | **11644..12047** | **181..188** |

Die beiden Teile sind **ein** physischer Gegenstand (Bild 05), aber ihre Standpunkte liegen
in z **5732 Welteinheiten** auseinander, und der Spieler liegt dazwischen. Deshalb verdeckt
das Gestell ihn vollständig und die Platte gar nicht.

### Warum die Platte falsch liegt

`geometrie.standpunkte` mit `aufrecht=True` nimmt die **unterste Zeile der Silhouette**
(hier Bildpunkt `(156, 108)`) und projiziert sie mit `geom.welt_am_boden(..., y0=0)` auf den
**FUSSBODEN**. Bei einer waagerechten Tischplatte ist diese unterste Zeile aber ihre
**eigene Vorderkante auf Plattenhöhe** — der Sehstrahl läuft unter der Platte weiter und
trifft den Boden weit dahinter: `scr(156.5, 108.5) → Welt(4306, −5736)`.

Die Spaltenregel auf demselben Fußboden ist genauso falsch, nur in der anderen Richtung:
gemessen **Tiefe 201..319** (die Dokumentation in `auswahl.json` nannte 234..319).

**Die Regel, die das verbietet, steht seit 2026-09-07 in derselben Datei**
(`ROOM10D0` Cut 7/8/9, `_warum`):

> ⛔ BERICHTIGT 2026-09-07 (Nutzer-Marke F4933, ROOM10D0 Cut 9): „aufrecht" gilt NUR für
> wirklich senkrechte Gegenstände (Stuhl, Pfosten, Schrankwand, Wand). Ich hatte es
> zusätzlich auf TISCHPLATTEN angewandt … Vier Objekte zurück auf die Spaltenregel:
> ROOM10C0 02_02, ROOM10D0 07_02/08/09_02.

Cut 1 stand nicht auf dieser Liste und trug `aufrecht: true` weiter. Der eigene
`grund`-Text gab das sogar zu: *„Entscheidung, nicht Messwert: die Platte trägt EINE
Tiefenstufe."*

### Die Naht-Probe — das entscheidende Maß

Ein starrer Gegenstand darf an seiner **eigenen Naht** nicht in der Tiefe springen.
70 Plattenpunkte grenzen direkt an 55 Gestellpunkte (8er-Nachbarschaft). Tiefensprung
über diese Naht:

| Modell der Platte | Median | Mittel | Max |
|---|---|---|---|
| `aufrecht: true` (IST) | **85 Stufen** | 78,1 | 95 |
| Sehstrahl ∩ Ebene y=−1525 | 10 Stufen | 15,8 | 57 |
| `ebene: -1525` + Spaltenregel | **2 Stufen** | 3,2 | 15 |

2 Stufen = 128 Kamera-z-Einheiten; die Künstler selbst streuen innerhalb EINES Rechtecks
median 9 Buckets (`kalib_geometrie.py`). 85 Stufen sind der Defekt.

---

## 3. Die Plattenebene ist GEMESSEN, dreifach

`"ebene"` ist kein neuer Schalter: 19 Objekte in `auswahl.json` benutzen ihn schon, und die
Begründung dort ist wörtlich dieser Fehlerfall (ROOM1140 Cut 0, Korrektur 2026-09-04):

> „fuss": 140 nahm den untersten Punkt der Freistellung als BODENkontakt - er liegt aber auf
> der TISCHPLATTE … Mit „ebene": -700 … ergibt sich Tiefe 112 < 133.

### (a) Grundriss-Deckung mit dem eigenen Gestell — zweiseitiges Minimum

Die Platte liegt DIREKT ÜBER dem Gestell, ihr Grundriss muss also mit dem Fußabdruck des
Gestells zusammenfallen. Fußabdruck des Gestells: 29 eigene Spalten,
`x 2644..3522, z −3675..58`. Pro Kandidatenhöhe der Median-Abstand |dz| in denselben
28 Spalten:

| h | −700 | −1000 | −1300 | −1400 | **−1525** | −1600 | −1800 | −2000 | −2400 |
|---|---|---|---|---|---|---|---|---|---|
| Median \|dz\| | 3878 | 2525 | 1113 | 582 | **132** | 457 | 1419 | 2401 | 4202 |

Kein monotones Besserwerden, sondern ein echtes Minimum bei **h = −1525** (Median 132).
**Nullmodell:** von 97 abgetasteten Höhen (−100 bis −2500 in 25er-Schritten) liegen nur
**2** innerhalb EINER Tiefenstufe (64,0625) des Bestwerts, nämlich −1500 und −1525. Die
Höhe ist damit auf 25 Welteinheiten bestimmt.

### (b) Spalte für Spalte, unabhängig gerechnet

Für jede Spalte: Fußzeile des Gestells → Bodenpunkt `(wx,wz)`; Unterkante der Platte in
derselben Spalte → gesuchtes `Y` auf der senkrechten Weltlinie durch `(wx,wz)`.
n = 28 Spalten, **Median Y = −1515, Mittel −1517, Spanne −1675..−1311, sd 65**.
Eichung des Verfahrens am Spieler: `Log-vz 12057 → Y = +34` (soll 0),
`Log-vz 11625 → Y = −1468`.

### (c) Vorwärtsprojektion gegen die gemalte Platte

`marke1-10D0-C1/12_cut1_ebene_700_vs_1525.png`: die Deckfläche des SCA-Grundrisses
`x2900..4200 z−4100..3800` bei **h=−1525 (cyan)** liegt auf der gemalten Tischplatte —
nahe linke Ecke, linke Längskante und ferne Ecke passen; bei **h=−700 (rot)** hängt sie
deutlich darunter und schneidet durch das Gestell und den Boden.
`11_alle_cuts_drahtgitter.png`: Cut 1 ist der **einzige** der 15 Kamerawinkel, der diese
Tischreihe zeigt — einen zweiten, kameraunabhängigen Winkel gibt es nicht.

---

## 4. Der Fix (umgesetzt) und die Gegenprobe

**Nur Daten, kein Werkzeug- und kein Engine-Code.**
`analysis/esp_masken_2026-09-03/auswahl.json`, `ROOM10D0` → `"1"` → Objekt „Klapptisch
hinten — Platte":

```diff
-     "aufrecht": true,
+     "ebene": -1525,
+     "zelle": false,
```

`"zelle": false` wie beim Gestell: `ebene_des_objekts` leitet aus `ebene` das Band ab
(`round(1525/0x708) = 1`), und die Zellen-Zuordnung hat auf Band 1 nichts zu suchen.

Neu gebaut mit
`python re15_port/tools/maske/raum.py ROOM10D0 1 --stufe 16 --statistik max --dump build/p2/dump_vorher.txt`.

### Generator-Abnahme, vorher/nachher

| Größe | vorher | nachher |
|---|---|---|
| Tiefe Platte | 201..206 | **102..162** |
| Tiefe Gestell (unverändert) | 111..172 | 111..172 |
| Tiefe Cut gesamt | 111..206 | **102..172** |
| Rechtecke / Atlaspunkte | 69 / 4024 | 93 / 4090 (Grenze 105 / 65536) |
| Soll-Deckung | 1021 px, fehlt 0, zuviel 0 | **1021 px, fehlt 0, zuviel 0** |
| Standplätze geprüft / berührt | 5796 / 809 (**Abdeckung 14,0 %**) | 5796 / 809 |
| VORn / VORverd / VORteil | 325 / **0** / 0 | 104 / **0** / 5 |
| HINTn / HINTfrei | 744 / **289 (38,8 %)** | 751 / **20 (2,7 %)** |

Das harte Tor `VORverd == 0` hält. Die 5 neuen VORteil-Plätze liegen alle am rechten
Rand des Laufstreifens direkt an der Tischkante (`x=2900/3100`, z −2650/−1650/−50) mit
Deckung 0,15..0,64 — bei einer 1525 hohen Platte ist eine Teilverdeckung dort richtig,
und keiner erreicht die Vollschwelle.

### Engine-seitige Schiene `unit_pri_kopfschnitt` (2 031 312 Standplätze, 439 Winkel mit Masken)

| | vorher | nachher |
|---|---|---|
| ROOM10D0 Cut 1: HINTn / HINTfrei | 483 / **91** | 483 / **1** |
| ROOM10D0 Cut 1: VORn / VORverd | 103 / 0 | 70 / 0 |
| spielweit HINTfrei | **1011** | **921** |

Dieser eine Cut trug 90 der 1011 spielweiten Fehlstellen; sie sind weg.

### Bild vorher/nachher

`marke1-10D0-C1/14_vorher_nachher_simulation.png` — links vorher, rechts nachher.
**Ehrlich zur Herkunft:** das ist keine neue Bildschirmaufnahme, sondern eine Simulation
mit demselben Maskenleser (`maskenbild.masken` + Atlas) und derselben Verdeckungsregel
wie `re15_pri.h` (`Schlüssel = (t+1)·64,0625`, verdeckt gdw. `t < (1023·vz)>>16`), die Figur
aus dem Befundbild des Nutzers. Ergebnis: von den 255 gezeichneten Figurpunkten im Kasten
werden **133 verdeckt** — genau die 133, die vorher auf der Platte lagen; 122 bleiben
sichtbar (Oberkörper über der fernen Plattenkante), was richtig ist.
Tiefenkarten: `15_tiefenkarte_vorher.png` / `15_tiefenkarte_nachher.png`.

### Riegel und Gegenprobe

Neu: `re15_port/tests/unit/test_r19_marke1_10d0_c1.c`, registriert in
**eigener** Datei `re15_port/tests/unit/probes/r19_marke1-10D0-C1.cmake` (GLOB, nicht in der
gemeinsamen `CMakeLists.txt`). Er liest den geschriebenen Container mit dem
**Engine-Parser** (`re15_pri_msk_section_offset` + `re15_pri_parse_section`) und prüft:

1. `(1023·11644)>>16 = 181` → keine Maske des Cuts darf Tiefe ≥ 181 tragen, sonst liegt sie
   hinter dem Spielerkopf an der Marke.
2. Die Soll-Deckung `ROOM10D0_PRI01.PBM` hat genau 1021 Punkte und **jeder** liegt in einem
   Maskenrechteck — die Freistellung des Nutzers kann kein Tiefenmodell heimlich verändern.

```
Gegenprobe (Skript dreht das Modell zurück, baut neu, lässt laufen, stellt her):
  alt  ("aufrecht": true)  Cut 1: 69 Masken, Tiefe 111..206 -> EXIT 1
       FAIL: 6 von 69 Masken mit Tiefe >= 181 (schlimmste 206)
  neu  ("ebene": -1525)    Cut 1: 93 Masken, Tiefe 102..172 -> EXIT 0
  Maskendateien bitgleich wiederhergestellt: True (MSK/TIM/PBM/STAND md5 identisch)
```

**Volle Testsuite im eigenen Baum `re15_port/build_r19_marke11001`: 331 von 331 grün**
(inkl. `r19_marke1_10d0_c1`, `unit_pri_kopfschnitt`, `unit_pri_silhouette`,
`unit_pri_hashes`, `unit_pri_eingemessen`, `integration_pri_masken`).

### Abdeckungsmessung der BESTEHENDEN Riegel — alle vier sind blind

Dieselbe Gegenprobe, aber mit allen PRI-Riegeln:

| Riegel | alt | neu | sieht den Befund? |
|---|---|---|---|
| `unit_pri_eingemessen` | 0 | 0 | **blind** |
| `unit_pri_silhouette` | 0 | 0 | **blind** |
| `unit_pri_hashes` | 0 | 0 | blind (pinnt die 117 Nutzer-PNGs, nicht die Tiefen — richtige Grenze) |
| `unit_pri_kopfschnitt` | 0 | 0 | **blind** (misst 1011→921, riegelt aber nur auf `VORverd`) |
| `r19_marke1_10d0_c1` (neu) | **1** | 0 | **sieht ihn** |

Besonders bitter: `unit_pri_eingemessen` enthält bereits eine eingemessene Nutzer-Marke für
**denselben Raum, denselben Cut, fast denselben Standort** —
*„ROOM10D0 C1 F9-Marke 1 (F423): Liege muss verdecken, er steht dahinter: Welt(3228,−4568)
… geprüft x156..176, davon 21 mit Maske auf Körperhöhe … 0 Spalten ohne wirksame Maske"* —
und sie stand auf PASS, während 133 Figurpunkte sichtbar auf der Platte lagen. Ihr Maß
zählt **Spalten mit irgendeiner wirksamen Maske**; das Gestell liefert in allen 21 Spalten
eine, also bleibt das Maß satt, obwohl das BILD falsch ist. Ein Spalten-Präsenz-Maß kann
diese Fehlerklasse grundsätzlich nicht sehen.

---

## 5. Was diese Messung WIDERLEGT

1. **„Die Zwei-Wege-Teilung von `01.png` verliert Punkte."** Nein. Gemessen:
   `01.png` (Alphaschwelle 110) = 1021 Punkte; Platte 493 + Gestell 528 = 1021;
   Vereinigung 1021, Schnitt 0, fehlt 0, zuviel 0. Die Teilung ist bitgenau.
   Die `ROOM10D0_PRI01.PBM` ist nach dem Fix **byte-identisch** zu vorher
   (`md5 b40929125d909de7…`) — die Silhouette wurde nicht angetastet.
2. **„`ALPHA_SCHWELLE = 110` ist die Ursache."** Nein, in diesem Cut nicht: die Vereinigung
   ist bitgenau die Vorlage, und der Fix ändert an der Silhouette nichts.
3. **„Die Tischplatte liegt richtig hinter dem Spieler."** Das behauptete `auswahl.json`
   wörtlich (*„Das ist richtig: eine Tischplatte auf Hüfthöhe, fast von der Kante gesehen,
   trifft den Boden erst weit hinten"*). Widerlegt: 133 gezeichnete Figurpunkte liegen
   sichtbar auf ihr, und der Spieler steht 468 Einheiten **jenseits** der Tischreihe.
4. **„`aufrecht` repariert das Tischplatten-Problem."** Nein — `aufrecht` projiziert
   genauso auf `y0 = 0`, nur mit EINEM statt vielen Standpunkten. Naht-Sprung 85 Stufen.
5. **„Der Spieler steht vor dem Tisch, die Verdeckung durch das Gestell ist der Fehler."**
   Nein. `(3575,0,−4568)` liegt 468 Einheiten hinter der fernen Kante der Tischreihen-Zelle
   (`z ≥ −4100`), und seine Kamera-z 11644..12047 ist größer als das Maximum des eigenen
   Tisch-Fußabdrucks (11081). Das Gestell verdeckt ihn zu Recht.
6. **„Die zwei Tiefengruppen im Log (201..206 und 111..119) sind zwei Gegenstände."**
   Nein — beide kommen aus derselben Nutzer-Freistellung `01.png`, nur farblich getrennt.
7. **„Die SCA-Zelle taugt als Quader-Maske für die Platte."** Nein: bestes IoU der Platte
   gegen die Quader-Silhouette der Zelle = **0,234** (die tragenden Zellen-Objekte in
   STAGE1 liegen bei 0,6..0,92). Die Zelle dient hier nur als Grundriss für die
   Vorwärtsprojektion (Beleg c), nicht als Maske.
8. **„Nach dem Fix bleiben 20 HINTfrei-Plätze, also stimmt die Tiefe noch nicht."** Nein:
   **19 von 20** haben in ihrem unverdeckten Körperkasten Punkte der grünen Plattenfläche
   des **VORDEREN** Klapptischs, für den es **überhaupt keine Freistellung gibt** (2255
   grüne Bildpunkte ohne jede Maske, `13_maskenloser_vorderer_tisch.png`, magenta). Bis
   642 Punkte je Platz. Das ist eine fehlende Freistellung, kein Tiefenfehler. Der 20.
   Platz `(2900,−1850)` ist zu 0,94 verdeckt und verfehlt die Vollschwelle knapp.
9. **„Es gibt einen zweiten Kamerawinkel, an dem man die Tischhöhe gegenprüfen kann."**
   Nein: von 15 Kamerawinkeln in ROOM10D0 zeigt nur Cut 1 diese Tischreihe
   (`11_alle_cuts_drahtgitter.png`).

---

## 6. Offen (nicht geraten, benannt)

* **Freistellung des VORDEREN Klapptischs in ROOM10D0 Cut 1 fehlt.** 2255 grüne
  Bildpunkte ohne Maske; erklärt 19 der 20 restlichen HINTfrei-Plätze. Der Gegenstand ist
  farblich nicht sicher vom hinteren Tisch zu trennen (beide dieselbe Plattenfarbe) und
  gehört deshalb per Lasso freigestellt — Rückfrage an den Nutzer, kein selbstgebauter
  Kasten.
* **`unit_pri_eingemessen` misst Spalten-Präsenz statt Punktdeckung** und ist damit für
  diese Fehlerklasse blind, auch an seiner eigenen Marke F423. Vorschlag: das Maß auf
  „Figurpunkte im Körperkasten, die unter KEINER wirksamen Maske liegen" umstellen und die
  Abdeckung mitschreiben. Nicht in diesem Auftrag angefasst, weil es alle 103 Räume betrifft.
* **`unit_pri_kopfschnitt` misst HINTfrei spielweit (921), riegelt aber nur auf `VORverd`.**
  Eine Schranke auf HINTfrei ist erst sinnvoll, wenn die fehlenden Freistellungen da sind.

## 7. Geänderte Dateien

* `analysis/esp_masken_2026-09-03/auswahl.json` — Platte: `aufrecht:true` → `ebene:-1525` + `zelle:false`, Begründung mit allen Messzahlen
* `re15_port/shared_assets/PSX/MASKS/ROOM10D0.MSK`, `…_PRI01.TIM`, `…_PRI01.STAND` — neu gebaut (`…_PRI01.PBM` unverändert)
* `re15_port/tests/unit/test_r19_marke1_10d0_c1.c` — neuer Riegel
* `re15_port/tests/unit/probes/r19_marke1-10D0-C1.cmake` — eigene Registrierung
* `analysis/befunde_2026-09-19/pri-masken-phase2/ROOM10D0_C1.png` — Prüfbild des Generators, neu
* `analysis/befunde_2026-09-21/pri-runde19/marke1-10D0-C1.md` + `marke1-10D0-C1/` — dieses Dossier
