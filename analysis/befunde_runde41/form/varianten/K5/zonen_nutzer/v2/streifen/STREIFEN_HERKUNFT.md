# Streifen-Herkunft (Runde 2, Untersucher Streifen) – nur Messung

Stand 2026-11-09, Zweig r41/form. Nichts am Modell, an der Textur oder am Spiel-Asset geändert
(`re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` md5 6e00f105…, `.TIM` md5 4668e504… = K5e_f1, vor und nach der Messung gleich).
Tafel: `23_streifen_herkunft.png`. Skripte: `skripte/`. Rohzahlen: `messung/`. Spielbilder (gdigrab, eigenes Fenster): `spiel/`.

Nutzer-Wortlaut:
- Tafel Runde 1: "Und bei den Huckel oben auf der Vorderseite hast du teilweise wieder grün streifen drin. Da frage ich micht wo die her kommen."
- Spiel K5e: "Und auf der Unterseite am Hals hat er komische rote streifen, die sollten da auch nicht sein."

## Kurzbefund

| Frage | Ergebnis |
|---|---|
| Grüne Keile auf dem Buckel (Tafel 00) / graue Keile in Zone 27 (Tafel 08) | Kopf 22 steckt im Raum in Brust/Buckel 10 und Hals 21 (Durchstoßen). Wo der Kopf dahinter liegt, malt die Zeichen-Reihenfolge (Mitteltiefe je Dreieck, fern zuerst) ihn trotzdem darüber. Kein Falschfarben- oder Zuordnungsfehler des Tafelzeichners, keine Textur. |
| Im Spiel? | Ja, dieselben Flächen. Das Spiel sortiert gleich (Dreiecks-Mitteltiefe, kein Tiefenpuffer). Im Spielbild sind die Stellen wegen der dunklen Haut nur schwach zu sehen. |
| Rote Streifen Hals-Unterseite (K5e im Spiel) | Kein Durchstoßen: 6 echte Außenflächen des Kopfes (22, Vierecke 1, 8, 9, 194, 201, 209) mit roter Maul-Farbe (Texturzeile 0, Material "Maulinneres"). Ursache: Farbzuordnung. |
| Seit wann | Schon K4 hat den Kopf im Hals/Buckel. K5c, K5e_f1 und Krallen-Kandidat sind an Kopf/Kiefer/Hals/Brust gleich. Tritt in 799/799 Bildern auf, am stärksten in Clip 12 = Bild-2-Pose. |

## 1. Welche Flächen bilden die Keile (Bild 2)

Verfahren (`skripte/s_basis.py`, `s_probe.py`, `s_schnitt.py`): Bild-2-Pose wie die Tafeln (Asset Clip 12 Bild 0, Lage 941/0/-23776, Gier 3874,
Kamera = RID 5090 Schnitt 11), im Grundlage-Raster der Tafeln (568 x 659). Zwei Rasterungen in einem Durchgang:
(a) **Maler-Ordnung** = Tafel-Zeichner (`v4f.rastern_id`) und Spiel (Dreiecks-Mitteltiefe, fern zuerst);
(b) **Tiefenpuffer**: je Bildpunkt die vorderste Fläche, 1/vz perspektivisch korrekt im Port-Bildschirm interpoliert.

- Gegenprobe: Die Maler-Kennung ist zu 100,000 % gleich der Tafel-Kennung `render_asset_2.npz` (0 abweichende Bildpunkte).
  Die Keile sind also genau das, was der Tafelzeichner gemalt hat. Falschfarbe und Zuordnung sind richtig: Grün in Tafel 00 und Grau in Tafel 08 sind Kopf 22.
- Maler ≠ Tiefenpuffer: 17.970 von 187.996 Figur-Bildpunkten. Davon sind 3.278 Bildpunkte "Kopf/Kiefer gemalt, obwohl Brust/Hals vorn liegt"
  (= die Keile) und 1.455 umgekehrt. Größter weiterer Posten ist 4.613 Bildpunkte "Brust gemalt, Hals liegt vorn". In den Tafeln sieht man ihn nicht,
  weil Zone 27 Brust und Hals umfasst.
- Keil-Flächen (`messung/schnitt_bild2.json`). Mitteltiefe = Sortierschlüssel, Abstand = wie weit die Kopffläche hinter der vorderen Fläche liegt:

| Kopf-Fläche (Teil 22) | Bildpunkte | Mitteltiefe | vorn liegt (Mitteltiefe) | Abstand Median / Max |
|---|---|---|---|---|
| Viereck 4, Hälfte 0 | 882 | 4974,0 | Hals 21 Viereck 10 (5038,7) | 46 / 94 |
| Viereck 15, Hälfte 0 | 622 | 4751,0 | Brust 10 Viereck 122 (4758,3) | 55 / 128 |
| Viereck 205, Hälfte 0 | 509 | 5046,7 | Hals 21 Viereck 18 (5057,7) | 66 / 88 |
| Viereck 12, Hälfte 1 | 431 | 4793,7 | Brust 10 Dreieck 61 (4876,0) | 55 / 119 |
| Viereck 13, Hälfte 1 | 255 | 4670,3 | Brust 10 Viereck 148 (4843,7) | 32 / 82 |
| Viereck 205, Hälfte 1 | 191 | 5127,0 | Brust 10 Viereck 91 (5149,3) | 77 / 96 |
| Viereck 14, Hälfte 1 | 138 | 4758,0 | Brust 10 Viereck 130 (4861,0) | 12 / 29 |

  In jeder Zeile hat die Kopffläche den kleineren Sortierschlüssel und wird deshalb später (oben) gemalt.
  Am Bildpunkt liegt aber Brust/Hals um 12-77 Einheiten (Median) davor. Einheiten = Modellraum; der Kopf ist 1.735 x 1.656 x 1.911 groß.
- **Durchstoßen, im Raum gemessen** (Dreieck-gegen-Dreieck-Schnitt, Möller, `skripte/s_geo.py`), Bild-2-Pose:
  - Kopf x Brust: 306 Paare (112 Kopf-, 109 Brustflächen); Kopf x Hals: 324 Paare.
  - Kiefer x Hals: 24 Paare; Kiefer x Brust: 28 Paare; Hals x Brust: 539 Paare.
  - Kopf-Ecken innerhalb Brust: 37 von 378, Tiefe Median 76, Max 296. Kopf-Ecken innerhalb Hals: 12, Max 155.
    Kiefer-Ecken innerhalb Brust: 5, Max 163.
  - Zählweise: Lot in die Fläche, auf der Innenseite der nächsten Fläche. Außen-Richtung gemessen: 2.906 von 3.199 sichtbaren
    Dreiecken haben ihr Kreuzprodukt von der Kamera weg, also Außen = -Kreuzprodukt.
- Ecken der Keil-Flächen einzeln (`messung/ecken_keil.json`), Ecken-Nummer im Teil 22 und Lage:
  - Viereck 4 = Ecken 4/15/5/16: innen 15, außen 113, innen 23, innen 32.
  - Viereck 15 = 16/27/17/28: innen 32, außen 186, innen 135, außen 117.
  - Viereck 205 = 379/380/387/388: innen 19 / 5 / 14, außen 31.
  - Viereck 12 = 13/24/14/25: innen 108, außen 497, innen 19, außen 489.
  - Vierecke 3 und 197 ähnlich.
  - Die Flächen sind keine reinen Innenflächen. Aus 960 Ansichten (28 Clips, jedes 10. Bild, 10 Richtungen) sind sie im Tiefenpuffer sichtbar,
    z. B. Viereck 13 mit 44.811 und Viereck 4 mit 8.548 Bildpunkten. Es ist die Hinterkopf-/Nackenhaut, die mit einem Teil ihrer Ecken im Buckel steckt.
- Schnittbild (Tafel Feld 4 unten): senkrechter Schnitt in Blickrichtung durch die Mitte von Kopf-Viereck 4. Die Umrisse von Kopf, Hals und Buckel liegen ineinander.

Antwort auf (1): **Durchstoßen + Zeichen-Reihenfolge.** Kein Zeichen- oder Zuordnungsfehler des Tafelzeichners (Kennung byte-gleich), keine Textur-Eigenschaft.

## 2. Im echten Spiel (K5e_f1)

Lauf (`skripte/spiel.sh`):
- eigene exe-Kopie (`re15_pc.exe` vom 2026-11-03; seit dem Bau keine Code-Commits unter engine/platform/include), echter Renderer;
- kein AUTOSHOT, kein SOFTWARE_RENDER, kein FRAMEDUMP;
- Start über RE15_TITLE_SHOT (nur ein Titelbild) und RE15_DEBUG_JUMP, Haltung über RE15_BB_POSE;
- Aufnahme mit `ffmpeg gdigrab -i hwnd=<hwnd>`. Das hwnd kommt über die eigene Windows-PID (`skripte/fenster.ps1`) und wird direkt vor der Aufnahme neu gelesen;
- Ende per taskkill /PID der eigenen PID.

Vergleich (`skripte/s_spielvergleich.py`): Vorhersage im Fenster-Raster (2 x Port-Pixel), mit Disc-Licht des Schnitts, einmal in Maler-Ordnung und einmal
mit Tiefenpuffer. Gemessen werden nur ruhige Bildpunkte (3x3 dasselbe Dreieck in beiden).

| Lauf | Bildpunkte, Maler = Tiefenpuffer: Abweichung | Ordnungsfehler-Bildpunkte: Spiel zu Maler / zu Tiefenpuffer | Anteil näher an Maler | Keil (Kopf über Brust/Hals): zu Maler / zu Tiefenpuffer |
|---|---|---|---|---|
| 5090 Schnitt 11, Bild-2-Pose (Clip 12/0), von vorn | 7,6 (Feuer vor der Figur) | 1,33 / 5,08 (803 px) | 95,9 % | 1,04 / 5,30 (147 px, 93,8 %) |
| 5080 Schnitt 1, Gier 2095 = von vorn (Clip 12/0) | 1,43 | 1,07 / 5,02 (1.736 px) | 91,3 % | 0,97 / 6,06 (258 px, 91,1 %) |
| 5090 Schnitt 11, Griff (Clip 27/20) | 1,55 | 1,23 / 4,37 (1.031 px) | 91,2 % | 2,34 / 10,08 (87 px, 86,9 %) |

Die Gier 2095 für 5080 ist gerechnet: dieselbe Blickrichtung zur Kamera wie in 5090 (5090: 3874 bei -13,3 Grad, 5080 Schnitt 1: 143,0 Grad).
Antwort auf (2): **Ja.** Das Spiel zeichnet an den Keil-Stellen dieselben Kopfflächen wie die Tafel. Der Grund ist dieselbe Regel: `render_pc.c` sortiert die
Dreiecke absteigend nach `s_textri_depth` (Einfügesortierung), und `main.c` setzt für Gegner `npc_avg_z = (wz0+wz1+wz2)/3`; einen Tiefenpuffer gibt es nicht.
Sichtbar ist es nur schwach, weil Kopf- und Buckelhaut im Spiellicht ähnlich dunkel sind (Spielbild in Tafel Feld 3, aufgehellt).

## 3. Rote Streifen an der Hals-Unterseite

`skripte/s_hals.py`, `s_hals_spiel.py`, `messung/hals.json`, `hals_spiel.json`. Rot-Regel wie MESSBLATT_K5F 0.2: a* > 12, C* > 15, Farbton > -10 Grad, am Texel ohne Licht.

- Rote Flächen im Asset (≥ 50 % rote Texel): 56. Davon 35 "Maulinneres" (Kopf 24, Kiefer 11), 6 "Auge", 15 Schwanzspitze. Texturzeilen 0 (41) und 7 (15).
- Die 6 Flächen 22|1|1, 8, 9, 194, 201, 209: alle Zeile 0, Material "Maulinneres". Rot-Anteil 0,946 / 0,980 / 0,945 / 0,933 / 1,000 / 0,994.
- Sichtbarkeit Maler gegen Tiefenpuffer:
  - Spiel-Griff 5090 (Clip 27/20): 724 gegen 713 rote Bildpunkte auf Hals/Kopf/Kiefer, beide nur aus 22|1|8, 9, 201, 209.
  - Von unten, Clip 2, 8 Richtungen: 1.753 gegen 1.895, nur die 6 Flächen und Kiefer-Dreiecke 3-6.
  - Im Tiefenpuffer bleiben sie also sichtbar: Es sind die vordersten Flächen, nichts sticht durch.
- Spielbild Griff (gdigrab): An den 626 Kern-Bildpunkten der 6 Flächen sind 87,7 % warm-rot (Lab-Mittel 7,5/14,5/5,3).
  In der Haut daneben (1.467 Bildpunkte, Hals/Kopf) sind es 5,6 % (Lab 6,7/4,1/-1,2).
- Die Ecken-Messung findet im Bild-2-Stand keine Kiefer-Ecke im Hals (0 von 130). Kopf x Hals schneidet sich zwar (324 Paare), aber die roten Flächen sind Außenflächen.

Antwort auf (3): **Textur/Farbzuordnung, kein Durchstoßen.** Das deckt sich mit MESSBLATT_K5F 0.2 (dort aus Material-Liste und Spiel-Schlüssellauf);
hier zusätzlich mit Tiefenpuffer und gdigrab gemessen.

## 4. Seit wann und in welchen Posen

`skripte/s_alle.py`, `messung/alle_K5e_f1.json`, `alle_Krallen-Kandidat.json`, `alle_K5c.json`. Je Bild:
- Schnitt im Raum: Paare von Kopf/Kiefer gegen Brust/Hals;
- Kopf-/Kiefer-Ecken innen, mit Tiefe;
- Keil-Bildpunkte in der Bild-2-Kamera, 640 x 480. Für die Keile werden nur die Teile 10/21/22/23 gerastert.

- **K5e_f1 und Krallen-Kandidat: alle 799 Bilder in allen Größen identisch.** Die Krallen ändern an Kopf/Hals/Brust nichts.
- **K5c** (3 Bilder): Clip 1 = Asset Clip 12/0, gleiche Werte (306/324/24/28 Paare, Ecken 43 innen bis 296, Kiefer 5 bis 163, Keil 742 px).
  K5c Clip 0: 151/309 Paare, Keil 643 px. K5c Clip 2: 148/293, Keil 36 px.
- **K4** (`stufe3/basis_n2`, Bild-2-Stand): 182/361 Paare, 96 Kopf-Ecken innen bis 310, Keil 204 px. K4 kroko: gleiche Werte.
  Der Kopf steckt also schon seit K4 im Hals. Ab K5c schneidet der Kopf mehr Brustflächen (Kopf x Brust 182 → 306), dafür liegen weniger Kopf-Ecken innen (96 → 43).
- Asset über alle 799 Bilder:
  - Kopf x Brust: 131-338 Paare (Median 149). Kopf x Hals: 244-360 (Median 269). Beides in **799/799** Bildern.
  - Kiefer x Hals: 16 Bilder; Kiefer x Brust: 76 Bilder (nur Clips 10 und 12).
  - Kopf-Ecken innen: 16-54, Tiefe Max 183-352 (Median 243).
  - Keil-Bildpunkte in der Bild-2-Kamera: > 0 in 782/799 Bildern, Median 141, Max 785.
- Je Clip (Kopf x Brust Paare, größte Ecken-Tiefe, Keil-px):
  - Clip 12 (Bild-2-Pose) ist am stärksten: 301-338 Paare, 269-344, Keil 1-742.
  - Clip 10: 145-310, 183-352, Keil 0-757.
  - Clips 0/2/3/4/5/8/9/11/13/26/27: 131-165 Paare, 191-282. Keil bis 785 (Clip 27) bzw. 784 (Clip 8).
  - Ein-Bild-Clips 1, 6, 7, 14-25: je 145 Paare, 238, Keil 33.

## 5. Was die Ursache beheben würde (nur benannt, nichts geändert)

1. **Buckel-Keile, Geometrie:** Die Ecken von Kopf 22 dürfen nicht in Brust 10 und Hals 21 liegen, mit etwas Rand.
   - Bild-2-Stand: Ecken 3, 4, 5, 13, 14, 16, 17, 186, 187, 372, 379, 380, 387 liegen 5-135 Einheiten innen.
   - Die nächsten Flächen sind Hals 21 Vierecke 1, 9, 11, 35, 49, 51 und Brust 10 Vierecke 121/122/129/130/132 sowie Dreiecke 5/42/43/60-62.
   - Über alle Clips reicht die tiefste Kopf-Ecke bis 352 Einheiten hinein, in Clip 12 bis 344.
   - Zwei Wege, die man messen müsste:
     - (a) Hinterkopf-/Nackenecken des Kopfes nach außen bzw. unten zurücknehmen, je um die Tiefe plus Rand;
     - (b) die Vorderfläche von Buckel/Hals über dem Hinterkopf zurücknehmen.
   - Weil diese Kopfflächen aus anderen Richtungen sichtbar sind (z. B. Viereck 13: 44.811 Bildpunkte in 960 Ansichten), hilft Löschen nicht.
   - Am stärksten wirkt es in Clip 12 und Clip 10.
   - Hinweis: Auch Brust 10 und Hals 21 durchdringen sich (539 Paare). Das gibt 4.613 Bildpunkte "Brust über Hals" in Bild 2. In den Zonen-Tafeln ist es unsichtbar, im Spiel als Texturkante möglich.
2. **Zeichen-Regel** (nur zur Einordnung): Mit Tiefenpuffer gäbe es die Keile nicht (Feld 4 oben). Das Spiel sortiert aber wie die PSX je Dreieck nach Mitteltiefe.
   Eine Änderung dort wäre eine Engine-Änderung ohne Original-Beleg, kein Modell-Fix.
3. **Rote Hals-Streifen:** Die 6 Flächen 22|1|1, 8, 9, 194, 201, 209 von Zeile 0 (Maulinneres) auf eine Hautzeile legen. Diesen Plan nennt MESSBLATT_K5F 0.2 schon
   (K5f-Vorschau v7: 0 von 963 Zeile-0-Pixeln rot). An der Geometrie ist dafür nichts nötig.

## Dateien

- Tafel: `23_streifen_herkunft.png`
- Skripte:
  - `skripte/s_basis.py`: Rasterung Maler + Tiefenpuffer
  - `s_probe.py`, `s_sicht.py`, `s_zoom.py`: Arbeitsbilder
  - `s_schnitt.py`: Bild 2, Keile, Schnitt, Ecken
  - `s_geo.py`: Dreieckschnitt, nächster Punkt
  - `s_ecken.py`: Keil-Ecken, Sichtbarkeit
  - `s_alle.py`: alle Clips
  - `s_hals.py`, `s_hals_spiel.py`, `material_lesen.py`: Hals
  - `spiel.sh`, `fenster.ps1`: Spiellauf + gdigrab
  - `s_spielvergleich.py`: Spiel gegen Vorhersage
  - `s_tafel.py`: Tafel
- Rohzahlen: `messung/*.json`. Spielbilder: `spiel/*_gdigrab.png`.
- Die Runde-1-Tafeln werden aus Commit 239eac698 gelesen (md5 gleich den Dateien, die der Nutzer gesehen hat: 8ba19871…, 0fb4ea69…).
  Der Nutzerordner 21_riesenform_grundlage_zonen wurde inzwischen umgeräumt (alt_runde1_vom_pruefer_nicht_bestanden); dort wurde nichts geschrieben.
