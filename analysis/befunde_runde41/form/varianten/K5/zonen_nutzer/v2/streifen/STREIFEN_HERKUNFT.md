# Streifen-Herkunft (Runde 2 + Runde 3, Untersucher Streifen) – nur Messung

Stand 2026-11-09 (Runde 3 abends), Zweig r41/form. Nichts am Modell, an der Textur oder am Spiel-Asset geändert
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
| Rote Streifen Hals-Unterseite (K5e im Spiel) | Kein Durchstoßen: 6 echte Außenflächen des Kopfes (22, Vierecke 1, 8, 9, 194, 201, 209) mit roter Maul-Farbe (Texturzeile 0; in der Farbliste material.json unter "Maulinneres", das ist eine Farb-, keine Lageliste). Ursache: Farbzuordnung. |
| Gehören die 6 zum "geöffneten Maul"? (Runde 3, Prüfer r2 Nr. 11) | **Nein.** Sie liegen außen an der Kopf-Unterseite hinter dem Unterkiefer, Außen-Normale nach unten (n_y 0,85-0,99 im Knochenraum des Kopfes). Bei geschlossenem wie offenem Maul (0 / 17,1 / 18,7 Grad) aus 57-80 von 162 Richtungen gleich gut zu sehen; Maul-Inneres wird dagegen erst beim Öffnen sichtbar. Auch die übrigen 4 Kopfflächen der alten Zone 30 liegen außen (vom Hals verdeckt). Abschnitt R3.1. |
| Seit wann | Nur für das DURCHSTOSSEN gemessen: Schon K4 hat den Kopf im Hals/Buckel. K5c, K5e_f1 und Krallen-Kandidat sind an Kopf/Kiefer/Hals/Brust gleich. Kopf x Brust und Kopf x Hals schneiden sich in 799/799 Bildern; Keil-Bildpunkte in der Bild-2-Kamera gibt es in 782/799 Bildern, am stärksten in Clip 12 = Bild-2-Pose. Die ROTEN Hals-Streifen: siehe nächste Zeile. |
| Seit wann: rote Hals-Streifen (Runde 3, Prüfer r2 Nr. 17) | Jetzt über alle Bilder und alle Spiel-Stände gemessen (Abschnitt R3.2). Rot erst seit **K5c** (Asset a005b357a, 07.11.); in K3/K4 tragen die Flächen an dieser Stelle Texturzeile 3 ohne Rot (0 Bilder mit Rot), in den Ständen vom 02.11. davor ist der Kopf anders gegliedert (0 Bilder mit Rot). K5e_f1: Rot aus den 6 Flächen von unten in **654 von 799** Bildern, von der Spielkamera 5090 in **588 von 799**; in Clip 12 (Bild-2-Pose) von unten in 0 von 56. Krallen-Kandidat: dieselben Bilder. "Seit K4" stimmt für das Rot also nicht. |

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
- Die 6 Flächen 22|1|1, 8, 9, 194, 201, 209: alle Zeile 0, in der Farbliste unter "Maulinneres" (Lage: R3.1). Rot-Anteil 0,946 / 0,980 / 0,945 / 0,933 / 1,000 / 0,994.
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

Gilt für das Durchstoßen und die Keile (Abschnitt 1). Die roten Hals-Streifen (Abschnitt 3) sind hier NICHT über alle Bilder gemessen; das steht jetzt in Abschnitt R3.2 (alle Bilder, alle Spiel-Stände).

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
3. **Rote Hals-Streifen:** Die 6 Flächen 22|1|1, 8, 9, 194, 201, 209 von Zeile 0 (rote Maul-Farbe) auf eine Hautzeile legen.
   Bei 22|1|9 und 22|1|209 tragen die Spiegel-Flächen der linken Seite (22|1|0, 22|1|202) schon Zeile 1, 0 % Rot (`messung/r3_spiegel.json`);
   1/8 und 194/201 sind paarweise beide rot. Die 4 übrigen roten Kopfflächen der alten Zone 30 (22|0|0, 22|0|9, 22|1|210, 22|1|217) liegen
   ebenfalls außen, sind aber vom Hals verdeckt (Mittelstrahl trifft Hals 21 nach 83-281 Einheiten). Kiefer 23|0|3-6 (92-100 % rot) liegen am
   Kiefergelenk und sind nur bei fast geschlossenem Maul zu sehen (23-27 von 162 Richtungen bei 0 Grad, 0 bei 17-19 Grad).
4. **Zone 30 der Grundlage:** die 10 Kopfflächen der alten Zone 30 heraus (Außenhaut), Kiefergelenk 23|0|3-6 und 23|1|3 heraus.
   Die Grundlage Runde 3 (c8fbee2f6) hat das bereits so; gegengeprüft in R3.1. Diesen Plan nennt MESSBLATT_K5F 0.2 schon
   (K5f-Vorschau v7: 0 von 963 Zeile-0-Pixeln rot). An der Geometrie ist dafür nichts nötig.

## R3. Runde 3: Prüfer-Befunde r2 Nr. 11 und Nr. 17

Prüfbericht: `../PRUEFUNG_GRUNDLAGE_V2_r2.md`.
- Nr. 11: Zone 30 "geöffnetes Maul" enthält die 6 Hals-Streifen-Flächen. Bewertung: "falsch".
- Nr. 17: "Beides in allen 799 Bildern, seit K4" ist für das Rot nicht gemessen. Bewertung: "klein".

Gemessen wird die Zone 30 im geprüften Stand `zonen_v2.json` aus Commit 41585c197 (21 Flächen).
`r3_lage.py` liest sie fest aus diesem Commit.
Die Grundlage Runde 3 hat Zone 30 parallel neu belegt (Commit c8fbee2f6, 19:36). Diese neue Liste ist unten in R3.1 nur gegengeprüft.
Spiel-Asset vor und nach der Messung: EMD md5 6e00f105…, TIM md5 4668e504… (= K5e_f1).

### R3.1 Wo liegen die Flächen der (alten) Zone 30? (Nr. 11)

Skripte:
- `r3_lage.py`: Offenheit, Mittelstrahl, Nachbarn, Kieferwinkel
- `r3_kugel.py`: Sichtbarkeit aus 162 Richtungen
- `r3_kopfbild.py`, `r3_klassenbild.py`: Bilder
- `r3_spiegel.py`: Spiegel-Flächen
- `r3_gegenprobe_neu.py`: Gegenprobe der neuen Zone 30

Messdateien: `messung/r3_lage.json`, `r3_kugel_{zu,griff,bild2}.json`, `r3_kopf_griff.json`, `r3_spiegel.json`, `r3_gegenprobe_zone30_neu.json`.

Raum und Haltungen:
- Gemessen wird im Knochenraum des Kopfes 22 (x = Schnauze, y = unten positiv, z = Seite), ohne Kamera.
- Seiten: In der Ruhe-Haltung liegt Oberarm vorn links (11) bei z +1135 und Oberarm vorn rechts (16) bei z -1080.
  Also ist +z die linke Seite des Wesens.
- Kieferwinkel (Drehung Unterkiefer gegen Kopf) über alle 799 Bilder: 0 bis 28,83 Grad, Median 10,63.
- Gemessene Haltungen:
  - "zu": Clip 5 Bild 7, 0 Grad
  - "Griff": Clip 27 Bild 20, 17,11 Grad
  - "Bild 2": Clip 12 Bild 0, 18,74 Grad
- Zähne (Zone 41) liegen bei x 535 bis 1546.

**Maß 1: Sichtbarkeit aus 162 Richtungen.**
- Nur Kopf + Unterkiefer, orthografisch, Tiefenpuffer.
- Rückseiten werden weggelassen wie im Spiel (Außen = -Kreuzprodukt).
- Gezählt wird eine Richtung, wenn die Fläche dort mindestens 3 Bildpunkte hat.
- Maul-Inneres wird erst beim Öffnen sichtbar. Außenhaut ist offen wie zu gleich gut zu sehen.

**Maß 2: Mittelstrahl der Außen-Normale gegen das Gegenstück.**
- Kopf 22 wird gegen Unterkiefer 23 geprüft, Unterkiefer 23 gegen Kopf 22.
- Die Spalte "trifft nach" nennt den Abstand bis zum Treffer (Griff-Haltung).

| Fläche | Mitte (Kopfraum) | Normale | Richtungen zu / Griff / Bild 2 | trifft Gegenstück nach | Lage |
|---|---|---|---|---|---|
| 22\|1\|1 | 13, 208, 586 | -0,16 / 0,91 / 0,37 | 79 / 79 / 79 | – | außen, Kopf-Unterseite links hinter dem Kiefer |
| 22\|1\|8 | 15, 210, -586 | -0,16 / 0,91 / -0,37 | 80 / 80 / 80 | – | außen, rechts (Spiegel von 1) |
| 22\|1\|9 | 14, 238, -257 | -0,11 / 0,99 / 0,00 | 74 / 72 / 73 | – | außen, rechts |
| 22\|1\|209 | 0, 372, -156 | 0,00 / 0,92 / -0,38 | 78 / 78 / 78 | – | außen, rechts |
| 22\|1\|194 | 411, 301, 124 | 0,39 / 0,85 / 0,35 | 57 / 61 / 60 | 17 (Kiefer hinten) | über dem hinteren Kieferende, trotzdem von unten zu sehen |
| 22\|1\|201 | 412, 301, -126 | 0,39 / 0,85 / -0,35 | 59 / 62 / 63 | 16 | wie 194, rechts |
| 22\|0\|0, 22\|0\|9 | -290, 122, ±156 | -0,91 / 0,42 / 0 | 59-65 | – | außen am Hinterkopf, im Spiel vom Hals verdeckt |
| 22\|1\|210, 22\|1\|217 | -412, 301, ±126 | -0,39 / 0,85 / ±0,35 | 80-81 | – | außen, im Spiel vom Hals verdeckt |
| 23\|0\|3 - 23\|0\|6 | 468-472, 224-237, ±105-250 | nach oben-hinten | 23-27 / 0 / 0 | 23 bzw. 712 | Kiefergelenk, nur bei fast geschlossenem Maul zu sehen |
| 23\|1\|3 | 606, 222, 452 | 0,29 / -0,96 / 0 | 12 / 0 / 0 | 624 | Kiefergelenk |
| 23\|0\|10, 23\|0\|19 | 1690, 594, ±25 | nach oben | 0-3 | 358 (Gaumen) | Maul-Boden an der Kieferspitze |
| 23\|1\|50, 59, 60, 69 | 1511-1629, 613-623, ±51-83 | nach oben | 5-13 | 361-370 (Gaumen) | Maul-Boden an der Kieferspitze |

Die 6 Hals-Streifen-Flächen:
- Keine wird beim Öffnen besser sichtbar. 4 bleiben gleich (± 3 Richtungen), 194/201 schwanken um 4.
- Ihr Mittelstrahl trifft den Unterkiefer nicht, außer bei 194/201 nach 1-37 Einheiten (je nach Haltung) (hinteres Kieferende).
- Jede grenzt nur an Hautflächen bzw. an andere Zone-30-Flächen (`r3_lage.json`, nachbarn). Beispiel: 22|1|1 grenzt an 22|0|1, 22|1|0, 22|1|11 und 22|1|2, alle Haut.
- In Bild 2 hat keine der 21 Flächen sichtbare Bildpunkte (Zone 30 im geprüften Stand: sichtbar 0).

Seiten und Spiegel-Flächen:
- 22|1|9 und 22|1|209 liegen auf der rechten Seite des Wesens.
- Ihre Spiegel-Flächen links (22|1|0, 22|1|202) tragen Zeile 1 und 0 % Rot.
- Die Bemalung ist dort also einseitig.

Zusammenfassung der alten Zone 30:
- Mehr sichtbar beim Öffnen: 0 von 21.
- Gleich sichtbar: 14.
- Weniger sichtbar: 5 (23|0|3-6, 23|1|3).

Gegenprobe der neuen Zone 30 (c8fbee2f6, 21 Flächen hinter den Zähnen):
- Sie enthält keine der 6 und keine Fläche der alten Zone 30.
- Alle 21 werden beim Öffnen sichtbarer. Beispiele: 22|1|20 von 14 auf 41/42 Richtungen, 23|1|43 von 0 auf 32/32.
- Nach dem Maß "erst beim Öffnen sichtbar" ist die neue Liste Maul-Inneres.
- Die Klassen-Regel in `r3_kugel.py` nennt 8 Gaumenflächen dieser Liste in offener Haltung "außen", weil sie durch das offene Maul auch von unten zu sehen sind.
  Diese Regel ist für den Gaumen zu streng. Maßgeblich ist der Zuwachs beim Öffnen.

Hilfsmaß ohne Aussagekraft (nur der Vollständigkeit halber):
- Die "Offenheit" (Anteil freier Strahlen, `r3_lage.py`) trennt nicht.
- Auch Kopfhaut hat dort Median 0,63 und 10-%-Wert 0 (`r3_lage.json`, gruppen).

Antwort auf Nr. 11: **Der Befund des Prüfers stimmt.**
- Die 6 Flächen und die 4 übrigen Kopfflächen der alten Zone 30 sind Außenhaut der Kopf-Unterseite hinter dem Unterkiefer. Sie sind kein Maul-Inneres.
- Ihr Material darf nicht "Maul" heißen. Sie gehören zum Kopf ("Haut, Teil benannt, kein Farbwort").
- Kiefer 23|0|3-6 und 23|1|3 liegen am Kiefergelenk und werden beim Öffnen unsichtbar.
- In der heutigen Zone 30 (c8fbee2f6) ist das schon so umgesetzt; gegengeprüft wie oben.

### R3.2 Die roten Streifen über alle Bilder und alle Stände (Nr. 17)

Skript `r3_rot_alle.py`, Messdateien `messung/r3_rot_alle_*.json`, Zusammenfassung `r3_staende.py` → `messung/r3_staende.json`.

Verfahren je Bild:
- Ganze Figur, Texel ohne Licht, 640 x 480, Tiefenpuffer. Es zählt nur, was wirklich vorn liegt.
- Gerastert werden nur Dreiecke, deren Bildschirm-Rechteck das von Hals/Kopf/Kiefer berührt. Die Verdeckung dort bleibt vollständig.
- Rot-Regel wie `s_hals.py`: a* > 12, C* > 15, Farbton > -10 Grad.
- Gezählt wird nur das Rot aus den Flächen an der Stelle der 6 (Flächenmitte ≤ 40 Einheiten von der K5e_f1-Fläche).

Zwei Ansichten:
- "von unten": Kamera 40 Grad unter der Figur, wie `hals.json`.
- "Spielkamera 5090": Schnitt 11, Lage 941/0/-23776, Gier 3874.

Spiel-Stände: alle Commits des Spiel-Assets (`git log` auf BAHNSTEIG_FORM.EMD/.TIM). Ältere Stände: jedes 2. Bild.

| Stand | Commit | Datum | Schritt | rote Flächen an der Stelle | Texturzeile | Rot von unten (Bilder) | Rot Spielkamera 5090 (Bilder) |
|---|---|---|---|---|---|---|---|
| erste Riesenform | 0e10882f9 | 2026-11-02 | 2 | 0 (Kopf anders gegliedert) | – | 0 von 380 | 0 von 380 |
| Nachschliff | 6200b3d35 | 2026-11-02 | 2 | 0 (Kopf anders gegliedert) | – | 0 von 380 | 0 von 380 |
| K3 | a452893f1 | 2026-11-02 | 2 | 0 von 3 | 3 | 0 von 400 | 0 von 400 |
| K4 | 67408cfcb | 2026-11-03 | 2 | 0 von 3 | 3 | 0 von 400 | 0 von 400 |
| K5c | a005b357a | 2026-11-07 | 2 | 6 von 6 | 0 | 326 von 400 (Median 61, max 574 px) | 296 von 400 (Median 75, max 2349) |
| K5e_f1 (heute) | 62bd0001b | 2026-11-09 | 1 | 6 von 6 | 0 | **654 von 799** (Median 64, max 581) | **588 von 799** (Median 76, max 2349) |
| Krallen-Kandidat | – | – | 1 | 6 von 6 | 0 | 654 von 799 (Median 64, max 575) | 588 von 799 (Median 76, max 2349) |

- Krallen-Kandidat gegen K5e_f1:
  - In jedem Bild und jeder Ansicht ist gleich, ob Rot zu sehen ist.
  - Die Bildpunktzahl weicht in 25 von 1598 Ansichten ab, höchstens um 175 px. Dort liegen Krallen davor.
- Je Clip, K5e_f1, von unten (Bilder mit Rot / Bilder):
  - Clip 12 (Bild-2-Pose): 0 / 56.
  - Clip 10: 89 / 126. Clip 27: 70 / 100. Clip 8: 21 / 30. Clip 3: 48 / 60. Clip 13: 39 / 40.
  - Die übrigen Clips: in allen Bildern.
- Spielkamera 5090 (u. a.):
  - Clip 0: 0 / 18. Clip 12: 15 / 56, höchstens 5 px. Clip 11: 15 / 57. Clip 26: 17 / 35.
  - Clip 27 (Griff): 89 / 100, Median 1159 px.
- Die Rot-Messung der Runde 2 (Griff 5090 Clip 27/20) passt dazu. Hier zählt dieses Bild 1006 rote Bildpunkte aus den 6 Flächen, ohne Licht.
  Runde 2 maß 713 im Tiefenpuffer, mit Disc-Licht.
- K5c gegen K5e (`r3_k5c_rot.py`, `messung/r3_k5c_rot.json`), von unten, Clip 2 Bild 0:
  - K5c: 782 rote Bildpunkte auf Kopf/Kiefer. Sie kommen auch von Hautflächen (22|1|11: 186, 22|1|10: 95, 22|1|0: 66).
  - K5e_f1: 125, fast nur aus den 6 Flächen.
  - In K5e ist das übrige Rot unter dem Kopf weg; die 6 Flächen sind geblieben.

Antwort auf Nr. 17:
- Der Satz der Runde 2 "Beides ... in allen 799 Bildern ... seit K4" stimmt nur für Durchstoßen und Keile.
- Das Rot gibt es erst seit K5c (07.11.).
- In K5e_f1 ist es von unten in 654 von 799 Bildern zu sehen, von der Spielkamera 5090 in 588 von 799. Nicht in allen.
- In der Bild-2-Haltung ist es so gut wie nicht zu sehen.

### R3.3 Hinweis

Die weitergeleitete Nutzermeldung dieses Laufs betrifft den Zweibeiner ("Die Unterseite des großen Schwanzes ist fleischig ...").
Sie ist nicht Gegenstand dieser Streifen-Messung und hier nicht bearbeitet.

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
  - `s_tafel.py`: Tafel (Zeile 3 aus `r3_tafel.py`)
  - Runde 3: `r3_lage.py`, `r3_kugel.py`, `r3_kopfbild.py`, `r3_klassenbild.py`, `r3_spiegel.py`, `r3_gegenprobe_neu.py` (Lage, Nr. 11);
    `r3_rot_alle.py`, `r3_staende.py`, `r3_k5c_rot.py` (Rot über alle Bilder und Stände, Nr. 17).
    Die alten Spiel-Stände wurden für die Messung per `git show <commit>:re15_port/shared_assets/RE2/BAHNSTEIG_FORM.{EMD,TIM}` in den Arbeitsordner gelegt.
- Rohzahlen: `messung/*.json`. Spielbilder: `spiel/*_gdigrab.png`.
- Die Runde-1-Tafeln werden aus Commit 239eac698 gelesen (md5 gleich den Dateien, die der Nutzer gesehen hat: 8ba19871…, 0fb4ea69…).
  Der Nutzerordner 21_riesenform_grundlage_zonen wurde inzwischen umgeräumt (alt_runde1_vom_pruefer_nicht_bestanden); dort wurde nichts geschrieben.
