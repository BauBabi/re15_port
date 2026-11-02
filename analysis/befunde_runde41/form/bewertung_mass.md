# Bewertung Linse MESSUNG (unabhaengiger Pruefer)

Stand 2026-10-31. Kein Mitwirken an einem Kandidaten. Geprueft: `kandidat_a`, `kandidat_b`, `kandidat_c`, md5 EMD abc4f8bd / 96e68e30 / 8f6743ef, TIM 1b51a6e4 / 393fb680 / 054ea089.
Eigene Skripte und Belege: `bewertung_mass_zw/`. Pixel: Originalbild 640x480. Einheiten: Spiel, h = Hoehe ueber Boden.

## 1. Urteil

| Kandidat | Note | Kurzgrund |
|---|---|---|
| **A** Vorsprung | **6,5** | Einzige Figur, die in beiden Posen auf vier Fuessen steht, massig bleibt und beim Biegen dicht bleibt. Groesster Fehler: Bein FR steht in Bild 2 im Feuer, Rand 455-466 statt 416-420 |
| **C** Bildnah | **5,5** | Maskenehrlichste Pose 2, beste Toene, bestes Dossier. Aber die Beine sind Bretter quer zu Kamera 1, die Figur ist aus der Seite ein Panzer auf Stelzen, Pose 2 ist der staerkste Umbau |
| **B** Rumpfnaht | **4,5** | Beste Zeichnung in Bild 2 und echte Normalen. Aber die Figur steht in beiden Posen auf einem Zeh, Bein FL steckt in Pose 2 im eigenen Rumpf, Bein FR deckt das helle Band vollstaendig |

Keiner ist 1:1. Die Noten gelten nur fuer diese Linse. Die Werkzeugzahlen trennen die drei fast nicht, entschieden haben die Gegenmessungen in Abschnitt 3.
Alle drei haben auf genau den Masken und Punkten optimiert, die das Werkzeug abfragt. 33/33, 7/7, 9/9 und 7/7 sind deshalb kein unabhaengiger Beweis.

## 2. Nachrechnung

`form_pruefung.py pruefe` selbst ausgefuehrt, Ausgabe `bewertung_mass_zw/{a,b,c}/`.

| | A | B | C |
|---|---|---|---|
| IoU Bild 1 / 2 | 0,999 / 0,963 | 0,996 / 0,999 | 0,997 / 0,981 |
| Weiss gedeckt | 1,000 / 1,000 | 0,996 / 1,000 | 0,999 / 0,982 |
| Schwarz getroffen | 23 / 908 | 17 / 12 | 44 / 26 |
| Punkte | 33/33, 7/7 | 33/33, 7/7 | 33/33, 7/7 |
| Boden, Verdeckung | 2/3, 9/9 | 2/3, 9/9 | 3/3, 9/9 |
| Zeichnung, Verbote | 9/9, 7/7, 2/2 | 9/9, 7/7, 2/2 | 9/9, 7/7, 2/2 |
| MAE | 10,3 / 7,3 | 12,4 / 7,5 | 9,6 / 8,5 |
| NCC | 0,837 / 0,823 | 0,751 / 0,860 | 0,852 / 0,803 |
| Flaechen, Knochen | 1862, 23 | 1890, 21 | 1842, 21 |

Alle Dossierzahlen der Tabellen stimmen. `bericht.json/.txt` und die Blaetter sind bytegleich zu den Abgaben.
Masken und Grenzen stammen von 18:28, das Werkzeug von 18:31, die Kandidaten danach. Daran hat niemand gedreht.

Abweichungen zwischen Dossier und Befund:

| Kandidat | Dossier | gemessen |
|---|---|---|
| A | „Abgegeben ist Schnitt 1" | Datei `spiel_pose1_room5080_schnitt1.png` ist schwarz (nur Seitenbalken). Mit gdigrab neu gefahren: Figur da, Modell in Ordnung, `spiel_a_pose1_s1.png` |
| A | Schnitt 2 mehrfach ganz schwarz | nicht wiederholbar: Raum sichtbar, Max 251. Die Figur steht nicht im Bild, wie C schreibt. `spiel_a_pose1_s2.png` |
| A | „Rumpf in Pose 2 um 11 Grad nach vorn geneigt" | im EMD nicht vorhanden. Becken 17,0 Grad um eine fast senkrechte Achse (0,13 / -0,99 / -0,02), also Gier -16,8, Kippung 2,5, Rumpf 3,4 |
| A | Hals und Kopf rund 85 Grad | 79,3 Grad gegen Rumpf, passt ungefaehr |
| B | `lage.json` | `pruefe lage.json` bricht mit `KeyError 'emd'` ab. Erst `kandidat.json` laeuft, Werte gleich |
| B | Kuppel traegt rund +-9 Grad | Loecher ab 5 Grad: 70 bis 191 Lochpixel, 10 Grad 217 bis 514 |
| B | tiefster Punkt 0,0 / 0,4 / 0,0 | stimmt, verschweigt aber: das ist je Pose ein einziger Zeh |
| B | Bein FL „angehoben" | steckt zu 38-40 % der Oberflaeche im Rumpf, das steht nirgends |
| C | Biegeprobe „keine Loecher gesehen" | sah nur ein Blatt bei 10-12 Grad Rumpf. Gemessen 262 bis 486 Lochpixel bei 10 Grad |
| C | 3 Clips | Clip 2 ist derselbe Keyframe wie Clip 0 (Pool-Index 0). Eine eigene Ruhepose gibt es nicht, im Dossier nicht genannt |
| C | Glieder als „Schlaeuche" | Abflachung nicht genannt, siehe Abschnitt 3 |

Beschleuniger-Pfad: B und C konnten ihn wegen getrennter Sitzung nicht sehen. Jetzt nachgeholt, alle drei laden und zeichnen unter direct3d11 mit gdigrab, beide Posen (`spiel_*.png`).

## 3. Gegenmessungen

Skripte `geo.py` bis `geo9.py`, Rohausgabe `geo*.txt`. Nichts davon haben die Kandidaten als Ziel gehabt.

| Probe | A | B | C |
|---|---|---|---|
| Fuss h Pose 1 (FR, FL, NL, NR) | 0, 2, 13, 10 | **0, 104, 252, 122** | 0, 0, 64, 20 |
| Fuss h Pose 2 | 7, 8, 0, 5 | **127, 469, 149, 0** | **189**, 0, 115, 12 |
| Ecken unter h 30, Pose 1 / 2 | 27 / 29 | **3 / 3** | 44 / 18 |
| Standflaeche Mio E2 | 4,73 / 4,23 | **0,02 / 0,01** | 3,03 / **0,53** |
| Schwerpunkt, Abstand zum Standrand | +731 / +828 | **-1097 / -1073** | +463 / +60 |
| Volumen Mrd E3 | 8,5 | 6,4 | **5,4** |
| Glied Dicke/Breite, Median, kleinstes | 0,93, 0,77 | 0,90, 0,75 | **0,60, 0,44** |
| Bein im Rumpf Pose 1 > 2 | NR 12 > 17 % | **FL 10 > 40 %, 535 tief** | FR 3 > 13 % |
| Summe Gelenkdrehung P1 > P2 | 253 Grad | 228 Grad | **346 Grad** |
| Gelenke ueber 15 / 40 Grad | 5 / 1 | 4 / 2 | 7 / 4 |
| Groesste Einzelgelenke | Kopf 40, Hals 40, NR 29 | FL 55, 47, 40 | Kiefer 78, Huefte FR 57, Fuss FR 48, NL 47 |
| Gesehene Flaeche in beiden Bildern | 28 % | **2,9 %** | 35 % |
| Normale gegen Geometrie, Median, ueber 30 Grad | **22,3, 36 %** | 7,9, 4,5 % | **25,3, 38 %** |
| Lochpixel Ruhe, 14 Sichten, 16 E/px | **186**, Bauchschlitz | 30 | 35 |
| Rumpf 10 Grad, neue Lochpixel | 27 | **514** | **486** |
| Rumpf 30 Grad | 0 | **3740** | **2857** |
| Beine 60 Grad | NL 207, Hals 166, sonst unter 30 | fr_ober zerfaellt in 6 Sichten | fr_huefte zerfaellt in 4 Sichten |
| Gelenkkugel im Kindglied, Median | 203 | 137 | **39** |
| Bild 2 rechter Rand y 225-262, Formblatt 416-420 | **455-466** | 419-425 | 420-442 |
| Bild 2 rechter Feuerlappen gedeckt, Video R | **99 %, 190** | 10 % | 41 %, 177 |
| Bild 2 helles Band (210-250,376-384) gedeckt | 26 % | **100 %, R 159** | 0 % |
| Bild 2 Flaeche Y gedeckt | **6 %** | 81 % | 64 % |
| Render auf Grau, Video Y ueber 90, Bild 2 | **7099 px** | 2365 | 2231 |
| Gelesene Kante Bild 2 in der Grenze | 7/7 | 3/7, bis -23,7 px | 3/7, bis -18,1 px |
| Zeichnung mittleres dY, Bild 1 / 2 | **18,8 / 20,9** | 16,4 / 8,0 | 4,9 / 7,9 |
| Bandpass-NCC, Bild 1 / 2 | 0,767 / 0,616 | 0,670 / 0,638 | 0,759 / 0,556 |
| Helle Malerei ohne Sollstelle, px | 1239 / 2433 | 303 / 1020 | 1445 / 3189 |

Nicht ausgetrickst, gemessen:
- Koerpergroesse je Pose: nicht moeglich. Bindversaetze sind je Modell fest, Paare 2 und 3 leer, eine Wurzel. Scheitel 2948/2948, 2971/2990, 2966/2958, alle in der Formblatt-Spanne.
- Transparente Texel: keine, 0 Lochindizes. Der Umriss wird nicht ueber Texel geschnitten.
- Lage im Raum: 0 Ecken in soliden SCA-Rechtecken von ROOM5080 und ROOM5090, naechster Abstand zur Frau 686 / 1009 / 1081.
- Clips 0 bis 2 zerfallen bei keinem Kandidaten. Die scheinbare Luecke im Rundum von B Pose 2 ist Projektion, kein Riss. Eine Vermutung von mir, die die Messung widerlegt hat.
- A's „Stege" unter Z7 sind aus anderen Sichten unauffaellig (`nah_a_stege.png`).

NCC ist reine Malerei: Mit einfarbiger Textur faellt sie in Bild 1 auf -0,05 / 0,01 / -0,15, in Bild 2 auf 0,41 / 0,45 / 0,32. Alle drei tragen zurueckgerechnete Bildtoene in der Textur. Die NCC-Rangfolge sagt nichts ueber die Form.

## 4. Je Kandidat

### A, Note 6,5

**Am Bild:** Bild 1 trifft Kuppel, Schulter, Bein FR und Taille. Das Spielbild zeigt einen massigen Vierbeiner, der auch im Rundum eine Figur bleibt, kein Trick aus anderer Sicht. In Bild 2 stimmt die Kuppelkante als einzige auf allen gelesenen Punkten.
**Trick:** Bein FR und Huefte stehen in Bild 2 in der grauen Feuerzone: 99 % des rechten Lappens gedeckt, dort zeigt das Video weisses Feuer. Der Pruefpunkt RAND413 ist einseitig und laesst das durch. Dazu 908 px auf Schwarz, die einzige echte Maskenverletzung aller drei. Normalen fuer Kamera 1 gestaucht, Feinton aus dem Video in der Textur, Kopf 79 Grad weggedreht, damit der Stapel in Bild 2 verschwindet. Huefte, Normalen und Feinton nennt das Dossier selbst, nicht aber, dass fast der ganze Feuerlappen gedeckt ist und der Rand 40-50 px ueber dem Formblatt liegt.
**Weicht noch ab:** Rechts in Bild 2 dunkle Masse statt Feuer bis x 466. Flaeche Y nur 6 % gedeckt. Bild 1: Stapel Z2, Z4 viel zu dunkel (92 gegen 130, 77 gegen 141), Z1 wirkt laenger und satter, untere Massen ohne die blauen Flecken. Sichtloch (242-246,378-386), 41 px, erzwungen von einer falschen Maskeninsel. Bauchschlitz von unten.
**Uebernehmen:** Rumpfbau aus geschlossenen, ueberlappenden Stuecken (30 Grad ohne Loch). Vier Sohlen am Boden in beiden Posen. Runde, massige Glieder.

### C, Note 5,5

**Am Bild:** Bild 1 am naechsten bei den Toenen (dY 4,9), Pose 2 haelt sich als einzige aus Band und rechtem Feuer weitgehend heraus und deckt Flaeche Y mit der Hand. Dossier am ehrlichsten, es nennt seine eigene Selbstbestaetigung.
**Trick:** Die Beine sind abgeflacht, `fr_bein` 588 breit, 259 dick. Die Breite zeigt zu Kamera 1, in Bild 2 misst dasselbe Bein 363 statt 615. Aus der Seite ist die Figur ein Panzer auf Stelzen (`nah_c_beine.png`), das widerspricht „massig" und ist Bau fuer eine Kamera, nicht genannt. L2 besteht nur, weil sechs Ecken eine Kerbe in die dunkle Taille bis zu einem falschen Maskenpixel schneiden (`zoom_l2.png`). V3 besteht mit 0,52 Deckung nur durch das Endenspiel +-8. Kiefer 78 Grad eingezogen, damit Z3 und Z4 aus Bild 2 verschwinden. Zehenhoerner nur fuer Bild 1.
**Weicht noch ab:** Haken als runder Knopf mit umlaufendem Rand; im Mittel stimmt die Farbe (86,26,53 gegen 76,28,53), die Form nicht. Q5 fast weg (Groesse 0,43), G nur 0,72 auf der Figur. Zwei schmale Beine unten rechts in Bild 2. Hand C4 als flacher Fladen. Kuppel glattes Ei. Fuss FR schwebt 189 in Pose 2, Stand nur noch 60 vom Rand.
**Uebernehmen:** Tonregelung der Marken und `tim_genau` fuer die CLUT. Hand C4 als Deckung von Flaeche Y ohne das Band zu kreuzen. `quer.py` als feste Gegenprobe zwischen den Bildern.

### B, Note 4,5

**Am Bild:** Oberes Zwei-Drittel von Bild 2 am naechsten: Zug Q, Sichel und G (dY 8,0, Material bis 1,00), rechter Rand 419-425 trifft das Formblatt, Haken-Rand gut. Einziger mit echten Normalen. Bild 1 am schwaechsten, NCC 0,751, Bandpass 0,67.
**Trick:** Bein FL ist in Pose 2 in den Rumpf geschoben (`roentgen_b_fl.png`), sonst stuende es auf dem frei sichtbaren Boden BO. Das ist der Formblatt-Einwand gegen Sicht Ende, hier weggeraeumt statt geloest. In jeder Pose traegt nur ein Zeh, Schwerpunkt rund 1100 ausserhalb der Standflaeche. Bein FR deckt das helle Band vollstaendig (Video R 159), Fuss bei (245,474) unter dem Logo, 127 ueber dem Boden. Nur 2,9 % der gesehenen Flaeche liegen in beiden Bildern: Die IoU 0,999 beweist darum fast nichts, die Vorlagen binden sich hier kaum gegenseitig. Die Naht steht laut Dossier zur Kamera von Bild 1, nicht auf der Mittellinie.
**Weicht noch ab:** In Bild 2 dunkle Saeule bis unter das Logo, im Spiel liest sich die Figur dort nicht als Vierbeiner. In Bild 1 ist L2 unten offen (82 px Weiss fehlen, B4 und C3 beruehren sich nicht), NL endet 20 px ueber K2, Kerbe an Z7, Z3 zu hell, Z2 zu dunkel. Rumpf reisst schon bei 5 Grad. Kopf oder Vorsprung gibt es nicht, beweglich sind nur Lippen, das erfuellt den Rahmen nur knapp.
**Uebernehmen:** Malerei von Zug Q, Sichel und G. Normalen aus der Geometrie, ohne Stauchung. Ruhe-Clip als eigene Pose. Die Sichtmessung `zw/end_sicht.py` als Verfahren.

## 5. Maengel des Messwerkzeugs

| Stelle | Befund | Wirkung |
|---|---|---|
| Maske L2 | Einzelpixel (314,350) schwarz, Video Y 49, das ist Figur. Kern sonst 632 px | A faellt allein daran. B: dazu (312,349), Y 85, echte Kante. C besteht nur mit Kerbe. Die Spalte 2/3 gegen 3/3 sagt also nichts |
| Maske Bild 1 | Insel (242,378) 7x7, 33 px schwarz, Video RGB (54,85,125), das ist das Z7-Band | A Sichtloch, B Kerbe, bei C sind 33 der 44 Schwarz-Treffer diese Insel |
| RAND413 | nur untere Grenze | A besteht mit 455, Formblatt nennt 416-420 |
| V3 | Enden +-8 auf 24 px Strecke | C besteht mit 0,52 Deckung |
| Boden je Pose | nur tiefster Punkt ueber alles | ein Zeh genuegt, B besteht |
| Grau | heisst „frei" | dunkle Masse auf hellem Video zaehlt nicht. Vorschlag: Render auf Grau mit Video Y ueber 90 berichten |
| Zeichnung | 0,6 mit 3 px Spiel | Groesse 0,43 besteht |
| NCC | misst Malerei | nicht als Formmass lesen |

## 6. Grenzen dieser Messung

- Bild-1-Kamera und Weltlage sind geschaetzt, Fussorte in Bild 1 unter y rund 400 sind offen. Die Bodenwerte sagen, wo das Modell steht, nicht wo das Original stand.
- Stand ist Schwerpunkt gegen Huelle der Ecken unter h 30. Keine Physik. Mit Schwelle 130 kaeme B in Pose 1 auf drei Fuesse, schwebend bliebe es.
- Loch- und Biegeprobe: 14 Richtungen, 320 px, rund 16 E/px, Biegung um zwei Achsen je Gelenk ab Ruhepose. Feine Risse sieht sie nicht. Kein bewegter Clip.
- Windungszahl auf den geteilten Rumpfnetzen von B und C ist auf den Schnittkanten ungenau. Gelenkkugeln fuer Rumpf und Lippen darum nicht lesen. Die Medianwerte tragen.
- Sichtbarkeit aus eigenem Z-Puffer, das Spiel sortiert anders. Fuer Pixel je Teil genuegt das, Summen stimmen auf 0,3 % mit dem Render.
- Einfarbige NCC haengt an den Normalen, bei A und C also nicht an der reinen Geometrie.
- Spiel: Kein Schnitt zeigt die Sicht von Bild 1. Feuer steht zufaellig, Figurdetails dort nicht vergleichbar.
- Nicht nachgeprueft: C's Bytegleichheit bei `lauf4.py --neu` (haette Abgaben ueberschrieben), Tabelle `end_sicht` von B.

## 7. Dateien

`bewertung_mass_zw/`: `montage_bild1.png`, `montage_bild2.png` (Original, A, B, C), `roentgen_b_fl.png`, `roentgen_c_fr.png`, `roentgen_a_nr.png`, `zoom_l2.png`, `zoom_nl.png`, `zoom_haken.png`, `nah_c_beine.png`, `nah_a_stege.png`, `riss_{a,b,c}_10grad.png`, `loch_*_clip*.png`, `rund_*_clip*_hell.png`, `spiel_*.png`, `{a,b,c}/bericht.*`, `b_eingabe.json`, `geo*.py`, `geo*.txt`.
Geschrieben nur im Arbeitsbaum unter `form/`, eigene Laufordner unter `re15_port/build/form_spiel/` wieder entfernt, fremde PIDs und Laufordner nicht beruehrt. Kein Commit.
