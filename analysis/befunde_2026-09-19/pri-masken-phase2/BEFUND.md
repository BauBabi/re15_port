# Phase 2: STAGE1-Vordergrundmasken aus den Freistellungen des Nutzers

Umsetzung des Fix-Plans aus `pri-masken-audit.md` §4 mit den Korrekturen aus
`pri-masken-audit.skeptiker.md`. Werkzeuge: `re15_port/tools/maske/{geometrie,abnahme,bau_p2,
kalib_geometrie,clut_schluessel,pri_hashes}.py` + `raum.py main_p2`; Sonden
`probe_p2_atlas_dump`, `probe_p2_floor_dump`; Pins `unit_pri_hashes`, `unit_pri_silhouette`,
`unit_pri_kopfschnitt` (Phase 2), `integration_pri_masken`.

## 1. Die Schwelle des Zeichners war einen OT-Bucket zu frueh

Belegt in der PSX.EXE (Commit "fix(pri): Maskenschwelle byte-true"):

| Adresse | Instruktion | Bedeutung |
|---|---|---|
| `@0x8002565c` | `sra v1,v1,4` | Figur-OT-Index = `otz>>4` (gleiche Sequenz in 5 Mesh-Zeichnern) |
| `@0x80066c70/74` | `addiu t0,zero,341` / `ctc2 t0,cr29` | ZSF3 = 341 -> `otz = (1023*vz)>>12` |
| `@0x80039650-60` | `lh` / `sll 2` / `addu` / `jal AddPrim` | Masken-OT-Index = `depth` x1 |
| `@0x8001ce54` | `jal 0x80039590` | Masken sind die LETZTEN Prims des Bildes; AddPrim haengt vorne ein -> im GLEICHEN Bucket liegt die Figur obenauf |

Also verdeckt eine Maske genau dann, wenn `depth < (1023*vz)>>16`, d.h. ab
`vz >= (depth+1)*65536/1023`. Bucket-Breite 64,0625 — nicht 64.
Beispiel `depth=100`: Original ab `vz` 6471, die alte PC-Regel (`vz > depth*64`) ab 6401.

Diese Regel gilt jetzt in `render_pc.c` (Merge-Walk), im `befund.log`-Urteil, in den
`poccscan`/`poccsweep`/`pocc`-Diagnosen, in `test_pri_eingemessen` und in
`test_pri_kopfschnitt` — und in allen Messschienen dieser Phase.

## 2. Kalibrierung an den Kuenstler-Masken (§4.6d) — VOR dem Bau der Nutzer-Cuts

`kalib_geometrie.py`, 107 STAGE1-Original-Cuts, 3055 Kuenstler-Rechtecke, Objekt = die
Kuenstler-GRUPPE (STAGE1: median 3 Gruppen je Cut, 5 Rechtecke und 4 verschiedene Tiefen je
Gruppe). Die Original-Atlanten kommen ueber den ENGINE-Dekoder (`probe_p2_atlas_dump`), weil
der Python-Nachbau an ROOM1020/1030/1040/1070 scheitert.

Fehler = Modelltiefe minus eingetragene Tiefe, in OT-Buckets:

| Faktor | Statistik | Median | Mittel | \|e\|<8 | zu NAH (e<-8) | zu FERN (e>8) |
|---|---|---|---|---|---|---|
| 0,90 | MAX | +0 | +4,4 | 18,1 % | 39,1 % | 40,4 % |
| 1,00 | MAX | **+11** | +19,3 | 20,5 % | **24,2 %** | 52,8 % |
| 1,00 | MEDIAN | +1 | +7,5 | 22,2 % | 35,0 % | 40,1 % |
| 1,00 | MIN | -5 | -0,4 | 19,0 % | 43,3 % | 35,2 % |

Und nach Tiefenquelle aufgeschluesselt (Faktor 1,00) — das ist der entscheidende Schnitt:

| Quelle | n | Statistik | Median | \|e\|<8 | zu NAH | zu FERN |
|---|---|---|---|---|---|---|
| **Zelle** (Regel i) | 2224 | MAX | **+5** | 26,3 % | **26,8 %** | 43,7 % |
| Zelle | 2224 | MEDIAN | -4 | 27,2 % | 41,2 % | 28,0 % |
| Zelle | 2224 | MIN | -9 | 21,9 % | 51,7 % | 23,2 % |
| Profil (Regel ii+iii) | 831 | MAX | +80 | 5,1 % | 17,1 % | 77,0 % |

**Gewaehlt: Faktor 1,00 und Statistik MAXIMUM.** Begruendung aus diesen Zahlen, nicht aus
dem Plan:
* Wo das Objekt eine Kollisionszelle IST — 2224 von 3055 Rechtecken, also die tragende
  Klasse — trifft das Modell mit MAXIMUM den Kuenstler-Wert am besten (Median +5 bei
  typischen Tiefen 40..200, das sind rund 5 %).
* MAXIMUM ist zugleich die Richtung, die den gemeldeten Fehler ausschliesst: "zu nah"
  bedeutet, die Maske verdeckt jemanden, der davor steht. Mit MEDIAN waeren 41,2 %, mit
  MINIMUM 51,7 % der Rechtecke mehr als 8 Buckets zu nah, mit MAXIMUM 26,8 %.
* Ein Faktor 0,90 bringt den Gesamtmedian auf 0, aber um den Preis von 39,1 % statt 24,2 %
  "zu nah" — er kauft die Mitte mit der schaedlichen Richtung. Die Geometrie braucht keinen
  Faktor; sie schneidet den Sehstrahl exakt mit der Zellwand.

**Kalibrierzahl vorher/nachher.** Die alte Kette (`geom.py`-Kopf, 5151 Rechtecke,
DEPTH_FACTOR 0,90 + Kachel-Median) mass: Medianfehler +2, \|e\|<8 20,7 %, zu nah 35,5 %, zu
fern 41,8 %. Die neue Regel (3055 Rechtecke, Faktor 1,00, MAXIMUM): Medianfehler +11,
\|e\|<8 20,5 %, zu nah **24,2 %**, zu fern 52,8 %; in der tragenden Zellen-Klasse
Medianfehler +5 bei 26,8 % zu nah.

⛔ **Das Ziel "Medianfehler ~0 und beide Richtungen ausgeglichen" ist NICHT erreicht.** Das
Modell liegt systematisch zu FERN. Was sich verbessert hat, ist die schaedliche Richtung
(35,5 % -> 24,2 %); was sich verschlechtert hat, ist die Mitte. Das ist eine bewusste Wahl
und keine Messung, die ~0 ergeben haette — die Restfehler stehen unten unter "offen".

Streuung der Modelltiefe INNERHALB eines Kuenstler-Rechtecks (Faktor 1,00): Median 11
Buckets, p90 44. Die Kuenstler arbeiten also NICHT mit einer Stufe je Rechteck — deshalb
waehlt der Bau je Cut die FEINSTE Stufe, die die Engine-Grenzen haelt (Stufe 1 haelt in 43
von 107 Kuenstler-Cuts, Stufe 16 in allen 107).

## 3. Die Begehbarkeit der alten Sonde war invertiert

Das Standplatz-Audit des Dossiers nahm `re15_collision_on_floor` als "begehbar". Das ist
FUN_8003b7f0, der Containment-Scan — und die SCA-Zellen sind die HINDERNISSE
(`re15_collision.c`-Kopf: "the player walks in the band-MATCH-FREE complement").

Gemessen an 1416 echten Standorten aus `befund.log` (21 Raeume), Abstand zum naechsten
Rasterpunkt <= 141 (halbe Rasterdiagonale):

| Punktmenge | Standorte getroffen |
|---|---|
| neu (`probe_p2_floor_dump`, Spieler-Klemmpfad `re15_collision_constrain`) | **1201 von 1416** |
| alt (`re15_collision_on_floor`) | 46 von 1416 |

Nachgezaehlt in ROOM1010/1140/10E0: 3350/12915/6636 der alten "Bodenpunkte" liegen zu
100 % IN soliden Band-Zellen. Die alte Schiene zaehlte an den unbegehbaren Stellen.

## 4. Ergebnis je Raum und Cut

Protokoll: `protokoll/bau2.log` (+ `bau_1000.log`), Pruefbilder `ROOM####_C<n>.png`
(magenta = Soll und Maske, rot = Loch, gelb = zu viel, cyan = Rechteck mit Tiefe).
Alle geschriebenen Cuts haben **Deckung == PNG bitgenau** (fehlt 0 / zuviel 0) und
**VORverd == 0**.

76 Cuts geschrieben. Nicht geschrieben (alte Sektion bleibt):

| Cut | Grund |
|---|---|
| ROOM10D0 C1 | `unit_pri_eingemessen`: die F9-Marke F423 des Nutzers (er steht HINTER der Liege) verlangt eine wirksame Maske ueber x156..176. Das Nutzer-Original 01.png als EIN Objekt mit reiner Geometrie gibt der Spalte 176 eine Tiefe >= 180; wirksam waere < 180 (Kopf-vz 11579, Schwelle 11595). Es fehlen 16 Einheiten — genau der Restfehler der Kalibrierung. Ein eingemessener Nutzer-Punkt schlaegt das Modell. |
| ROOM1000 C3 | 1 von 805 VOR-Standplaetzen voll verdeckt (`unit_pri_kopfschnitt`). Massstab-4-Freistellungen, unterste Zeile knapp unter dem Horizont -> Sehstrahl fast parallel zum Boden, Modelltiefen 249..394. |
| ROOM10A0 C2 | 1 von 2 VOR-Standplaetzen (Treppenband 3, Welt 22250/22150) voll verdeckt. |
| ROOM10E0 C7 | passt nicht in das 256x256-Atlasblatt: 41235 Sollpunkte, mindestens 51097 Atlaspunkte auch bei der groebsten Stufe. LAUT gemeldet statt vergroebert. |

Unangetastet: ROOM10F0 C4/C5 (Nutzer-Lassos + Buerostuhl-Quader, vom Nutzer am 2026-09-09
abgenommen) und ROOM1100 C1/C2 (reine Quader-Waende, kein Nutzer-PNG).

## 5. Sichtpruefung im laufenden Spiel

13 Bilder `sicht_<ROOM>_C<n>_{vor,hinter}.png`, erzeugt mit `RE15_FORCE_CUT` +
`RE15_POCC_TP` an einem gemessenen Standplatz (Protokoll `protokoll/sicht_alle2.log`).

⛔ Drei Fallen, alle gemessen, keine geraten:
1. Ein Tastenskript trifft die Stelle vor einem Moebel nicht — in ROOM10C0 blieb Leon im
   Spawn-Winkel (337 Bilder, nur Cut 0), in ROOM1050 stand er hinter der Kamera.
2. Ohne erzwungenen Winkel zeigt das Spiel an diesen Stellen einen NACHBAR-Winkel
   (ROOM10C0: Winkel 0 statt 3) — die Kamerazone gehoert dort einem anderen Cut.
3. `RE15_POCC_TP` setzt nur x, z und rot — die HOEHE behaelt der Spieler. Ein Standplatz
   auf einem anderen Band ist unbrauchbar: ROOM10A0 C8, Kandidat auf Band 1, Spieler auf
   y=-14400 -> `vz` -1144, also hinter der Kamera, und in beiden Bildern war niemand zu
   sehen. Das Band wird jetzt in einem Probelauf gemessen.

Das URTEIL wird ebenfalls gemessen (`protokoll/sicht_pruef.py`): Figurpunkte = Abweichung
vom reinen Hintergrund > 40, dazu die Maskenpunkte im Koerperkasten, die an dieser Stelle
nach der Original-Regel VERDECKEN.

| Cut | Bild | Welt | Fuss-vz | Figurpunkte | Maskenpunkte im Kasten | davon verdeckend | Urteil |
|---|---|---|---|---|---|---|---|
| ROOM10C0 C3 | vor | 200/-5850 | 5606 | 7 | 551 | **0** | Maske verdeckt nicht |
| ROOM10C0 C3 | hinter | -5400/-2250 | 10571 | 23 | 294 | 294 | verdeckt |
| ROOM10D0 C6 | vor | 4700/25550 | 4498 | 69 | 500 | **0** | sichtbar |
| ROOM10D0 C6 | hinter | 5300/25550 | 4957 | 85 | 487 | 179 | teils verdeckt |
| ROOM1050 C6 | vor | 23100/-19950 | 8991 | 81 | 568 | **0** | sichtbar |
| ROOM1050 C6 | hinter | 24300/-22550 | 11705 | 10 | 74 | 74 | verdeckt |
| ROOM1000 C5 | vor | -600/-1450 | 3896 | 1028 | 1688 | **0** | sichtbar |
| ROOM1000 C5 | hinter | 4200/-6050 | 8930 | 0 | 735 | 735 | ganz verdeckt |
| ROOM11F0 C6 | vor | 3100/-100 | 3140 | 894 | 980 | **0** | sichtbar |
| ROOM11F0 C6 | hinter | 4300/-900 | 3185 | 1140 | 3689 | 3417 | teils verdeckt |
| ROOM11F0 C1 | vor | -1100/-8900 | 3886 | 513 | 769 | **0** | sichtbar |
| ROOM11F0 C1 | hinter | -700/-8300 | 4570 | 301 | 431 | 424 | teils verdeckt |
| ROOM1000 C0 | hinter | 17600/-10450 | 4112 | 605 | 2938 | 2938 | teils verdeckt |

**In allen sechs VOR-Bildern verdeckt die Maske NULL Punkte**, in allen sieben
HINTER-Bildern verdeckt sie. Zwei Cuts liefern kein Paar: ROOM1000 C0 hat keinen
begehbaren Standplatz VOR seinen Spindbloecken (sie reichen bis an den Bildrand), und
ROOM10A0 C8 keinen auf dem Band, auf dem der Spieler nach dem Sprung steht — dort ist mit
diesem Aufbau NICHTS gezeigt, und es steht auch nichts Gegenteiliges im Bericht.

## 6. Offen — Nachfragen an den Nutzer

1. **Massstab-4-Freistellungen** ROOM1000 C1 (01_01..01_04), C3 (03_01..03_04) und
   ROOM11F0 C1 (01.png): sie passen nur zu 85-93 % auf den Hintergrund und verlieren beim
   Verkleinern duenne Teile (1000/01_01: 14061 Alpha-Punkte im PNG -> 877 im Bild). Sind sie
   als 1x-Freistellung gemeint? ROOM1000 C3 faellt allein daran durch die Abnahme.
2. **ROOM10D0 07_01.png**: die heutige Fassung ist mein Fix zur Marke F690 (Boden zwischen
   den Stuhlrohren wurde ueber den Spieler geblittet, `a56cff68`), nicht das Nutzer-Original.
   Gemessen: die 168 Luecken-Pixel des Originals liegen im neuen Modell bei Tiefe 40..59,
   die Bodentiefe an denselben Bildpunkten bei 61..185 — die Maske waere dort zu 100 % naeher
   als der Boden, und 14 von 15 Standorten aus `befund.log`, deren Koerper diese Pixel
   beruehrt, wuerden dort verdeckt. Die neue Tiefenregel faengt den Fehler also NICHT ab;
   deshalb bleibt meine Fassung eingebaut. Soll das Nutzer-Original gelten?
3. **ROOM10D0 06.png**: zurueckgesetzt auf das Nutzer-Original (30x46, 245 px); die
   65x120-Fassung im Baum war unbelegter Herkunft. Das ist eine Nutzer-Entscheidung.
4. **Eigene Quellen ohne Nutzer-PNG** (neu gebaut, aber nicht aus einer Freistellung):
   ROOM1000 C0/C2 und ROOM11F0 C0 (Polygone), ROOM10D0 C7 (ein Quader neben zwei PNGs),
   ROOM10D0 C6 "Pflanze links" (meine Huelle), ROOM1100 C1/C2 und ROOM10F0 C4/C5
   (unangetastet). Freistellungen dafuer?
5. **Alpha-Schwelle 110** und **Standplatz-Schranke 95 %** sind Heuristiken ohne Beleg.
6. **ROOM10E0 C7** braucht eine kleinere Freistellung oder ein zweites Atlasblatt.
7. Der PSX-Farbschluessel (CLUT 0x0000 -> 0x8000) ist aus psx-spx abgeleitet und gegen
   40 Original-Atlanten gegengeprueft (dort 0 solche Eintraege), aber nicht auf Hardware
   gemessen — das PSX-Target baut hier nicht.
8. ROOM10A0 C8 und ROOM1000 C0 haben keine Sichtpruefung bekommen (kein Standplatz auf dem
   Band des Spielers bzw. keiner VOR dem Objekt). Dort steht die Abnahme, aber kein Bild.

## 7. Abnahme

`ctest --test-dir re15_port/build_p2_pri --timeout 120`: **100% tests passed, 0 tests failed
out of 307** (168 s). Darunter `unit_pri_hashes`, `unit_pri_silhouette`,
`unit_pri_kopfschnitt` (Phase 2), `unit_pri_eingemessen`, `integration_pri_masken`.
