# Gegenpruefung EICHUNG: Farbe der Riesenform ueber Referenzen (Gegenpruefer, 2026-11-09)

Geprueft wird `MESSBLATT_EICHUNG.md` (Commits 9ca7a8b84, d9d580afa). Ich habe nur gemessen: kein Asset, keine Variante, kein Engine-Code und
kein Bau. Die Skripte des Messers habe ich nicht weiterverwendet. Eigener Code und eigene Ergebnisse liegen im Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gegen_eichung/`
(`py/g1.py` ... `py/g5.py`, `g2_gegenprobe.json`, `g3_abs.json`, `g3s_auswahl.json`, `g4_originale.json`, `g5_ganz.json`).

Wiederverwendet habe ich vom Messer nur die Renderer-Zwischenstaende aus seinem Scratch (`e5_b*.npz`: Schattierung, Materialkarte, Masken,
Albedo je Pixel) und die Konstanten von `form_video` (KA/KB/LO/HI/EX). Die Vorwaertskette habe ich neu geschrieben, mit Gleitkomma-Eingang.
Selbsttest gegen `video_filter`: MAE 0,25 V, max 0,5 V (Rundung des uint8-Eingangs).

Keine Zahl hier ist ein RE-Wert. Alle Werte beschreiben die Abfilmung und gehoeren nicht in die Engine.

## 0. Urteil

| Pruefpunkt | Ergebnis |
|---|---|
| (1) Bilder und Nachbarbilder | **bestaetigt.** vlcsnap 1 = Videobild 3445, vlcsnap 2 = 3483 (MAE 0, max. Abweichung 0). Beide Dias haben 24 Bilder. |
| (2) Farbmodell an eigener zurueckgehaltener Referenz | **teilweise.** Bestand: am dunklen Hintergrund gut, an der dunklen Frau 19-21 dE daneben. em_frei ist schlecht bestimmt (Uebersprechen an den Schranken). Leon-EX schwankt um +-7 %, der Fehler auf der zurueckgehaltenen Haelfte liegt bei 10-12 dE (Mediane). |
| (3) Umkehr und Lichtdivision nachgerechnet | **Rechnung bestaetigt in Bild 1** (dE 1,0-2,5) und **fuer den Buckel in Bild 2** (2,1-3,1). In Bild 2 traegt die Umkehr bei Haut/Gelenk/Hals rund +10 a\* Rot, das das Vorwaertsmodell nicht verlangt (dE 6-13). |
| (3b) Auswahl der gueltigen Pixel | **Hauptbefund, verzerrt.** Der Messer waehlt die Pixel nach dem beobachteten Videowert. Ohne diese Auswahl sinkt L\* in Bild 1 um 10-15 (Haut 20,9 -> 6-11). Das ist mehr als die angegebene Unsicherheit von +-8,6. |
| (3c) Licht in Bild 1 | Das Video folgt der Port-Schattierung nicht. Die Haut ist im Video gleich hell (G 23-31), egal ob die Schattierung 80 oder 210 betraegt. L0 aus 5080/2 ist in Bild 1 nicht zu sehen. |
| (4) Zielfarben neben G4/EM30 | **Haut plausibel** (Buntheit wie G4 von hinten), aber kuehler im Ton als beide Originale. **Gelenk blasser als jedes Original.** F1/Buckel/Kopf sind bunt wie G4 von vorn; dort ist Rot aber nur Obergrenze. Die Ziele sind Werte im Kettenraum, keine Texel-Albedo. |
| Rangfolge der Staende | **nicht haltbar** fuer Bild 1. Ueber alle Kernpixel, ohne Auswahl nach Videowert, trifft K5c das Video am besten (dE 10,2 gegen 28,3/31,3). In Bild 2 liegen alle drei gleichauf (11,2-12,5). |

**Gesamturteil: Die Zielfarben sind so nicht verwendbar.** Die Bildzuordnung und die Arithmetik der Umkehr stimmen. Die Zielhelligkeiten
sind aber durch die Pixelauswahl nach oben verzerrt, und die Unsicherheit ist rund um den Faktor 2 zu klein angegeben. Der Satz "K5c ist in
jedem messbaren Material zu dunkel" folgt aus dieser Auswahl und nicht aus dem Video. Robust ist nur: Die Haut ist in Bild 1 fast unbunt
(C\* ~2-4 bei jeder Auswahl), und Bild 2 ist waermer im Ton als alle drei Staende (h +8 Grad gegen -15 bis -25 Grad; Rot ist dort Obergrenze).
Der Helligkeitswiderspruch zwischen Bild 1 und Bild 2 hat eine gemessene Ursache: Die L0-Schattierung des Ports ist im Video nicht
vorhanden (3.3).

## 1. Bilder (eigene Extraktion)

`py/g1.py`. ffmpeg `trim=start_frame=3400:end_frame=3520` mit `-vsync 0` (120 Bilder, unabhaengig vom showinfo-Weg des Messers).

| | vlcsnap 1 | vlcsnap 2 |
|---|---|---|
| bestes Videobild | 3445, MAE 0,0000, max. Abweichung 0 | 3483, MAE 0,0000, max. Abweichung 0 |
| Dia (Sprung > 3 zum Vorbild) | 3432-3455, 24 Bilder | 3480-3503, 24 Bilder |
| Spruenge an den Grenzen | 3456: 66,2 | 3480: 78,6; 3504: 72,4 |
| Streuung je Pixel im Dia | 0,179 | 0,267 (vom Messer nicht genannt) |
| eigenes Mittel gegen Messer-PNG | MAE 0,002, max 1 | MAE 0,013, max 1 |

Die Zeit 114,948 s (Index/fps) gegen 114,982 s beim Messer ist nur der Start-pts-Versatz, also bedeutungslos. **Zuordnung bestaetigt.**
Nachbarbilder liefern keine neue Information; das Dia-Mittel senkt nur das Kompressionsrauschen.

## 2. Zurueckgehaltene Referenzen (eigene Teilung)

`py/g2.py`, `g2_gegenprobe.json`. Masse: dE76 Pixel-Median / dE76 der Mediane. Der Messer fasst meist mit "dE der Mediane" zusammen, und
das ist um den Faktor 2-8 schmeichelhafter als der Pixel-Median.

### 2.1 Bild 2, nach Helligkeit des Hintergrunds geschichtet (Port-e, sigma 2)

| Modell | BG e<15 | BG e15-30 | BG e30-50 | BG e>=50 | Frau dunkel | Kittel |
|---|---|---|---|---|---|---|
| Bestand (BG im Fit, Frau nicht) | 5,5 / 0,7 | 9,1 / 2,1 | 11,9 / 3,2 | 17,8 / 15,9 | **20,8 / 18,8** (R -32,5 V) | 16,6 / 15,3 |
| em_frei Messer (BG + Frau im Fit) | 7,0 / 4,3 | 10,5 / 3,0 | 13,5 / 8,1 | 19,8 / 17,3 | 9,3 / 6,0 (im Fit) | 11,4 / 4,1 (im Fit) |
| eigen em frei, Fit BG e>=30 | 6,2 / 1,7 | 9,0 / 3,3 | - | - | 30,0 / 28,6 | 57,4 / 57,8 |
| eigen em diag, Fit BG e>=30 | 14,8 / 13,5 | 14,8 / 12,8 | - | 47,2 / 30,5 | 25,9 / 23,4 | 61,1 / 59,4 |
| eigen em frei, Fit BG unten | 9,8 / 7,5 | 10,6 / 3,2 | 22,0 / 21,2 | 22,3 / 11,8 | 25,3 / 23,0 | 55,7 / 55,7 |

Raeumliche Teilung, Test auf der anderen Bildhaelfte (em frei): oben -> unten 12,2 / 13,3; unten -> oben 13,9 / 7,1.

- **Ueberanpassung em_frei:** In jedem meiner freien Fits liegen 5 bis 6 der 6 Uebersprechwerte an der Schranke +-0,6. Das Modell ist nicht
  bestimmt. Zur Frau traegt es nur, wenn die Frau im Fit ist. Damit ist seine Frau-Gueltigkeit (9,3 / 11,4) eine Trainingszahl.
- **Bestand:** Am dunklen Hintergrund gut, aber dieser Hintergrund war im Fit. An der zurueckgehaltenen dunklen Frau (Spiel-e ~5, nahe am
  Wesen-Bereich) liegt er 21 / 19 dE daneben. Er sagt R um 32 V zu niedrig voraus, also liefert seine Umkehr dort zu viel Rot. Das deckt sich
  mit "Rot ist Obergrenze" beim Messer.
- Mit der Kittel-Probe (16,6 / 15,3) bestaetigt sich die Messer-Zahl 15,0 (der Mediane).

### 2.2 Bild 1, Leon raeumlich geteilt (oben/unten am Zeilenmedian, je ~3 100 px, eigene Kette)

| Fit | EX | Test dE (Pixel / Mediane) | Bias Test (V) |
|---|---|---|---|
| oben -> unten | (1,047, 0,786, 0,750) | 14,9 / 10,2 | +12 / -14 / -16 |
| unten -> oben | (0,932, 0,924, 0,862) | 19,2 / 12,5 | -12 / +8 / +10 |
| EX Messer (ganz Leon, im Fit) | (0,951, 0,868, 0,803) | ganz 14,1 / 2,2 | |
| EX farbe.md | (0,935, 0,887, 0,908) | ganz 16,1 / 9,4 | |

EX ist mit 3 Werten nicht ueberangepasst, aber nur auf +-6-8 % je Kanal bestimmt. Auf der zurueckgehaltenen Haelfte betraegt der Fehler
10-12 dE (Mediane) bzw. 15-19 dE (Pixel). Die Klassen-Gegenprobe des Messers (3,4 / 6,5 / 11,9 der Mediane) ist dagegen optimistisch.
Die Pose von Leon wurde ausserdem am selben Abstandsmass gewaehlt, mit dem auch EX gefittet wurde.

## 3. Umkehr und Lichtdivision selbst nachgerechnet

### 3.1 Analyse durch Synthese (`py/g3.py`, `g3_abs.json`)

Das ist ein anderer Weg als beim Messer. Ich kehre nichts um und ziehe kein Streulicht ab. Ich setze eine Albedo je Material ein, multipliziere
sie mit der Port-Schattierung und schicke sie **vorwaerts** durch die Kette, mit dem Streulicht des ganzen Bildes. Dann fitte ich, bis die
Kette das Video an denselben gueltigen Pixeln trifft. Es gibt zwei Ansaetze: eine konstante Albedo je Material, oder die K5e_f1-Textur mal
einen Faktor je Kanal. Der Rest am Video liegt nach dem Fit bei 0-5 V.

| Bild / Material | Messer Median (Lab) | eigene konstante Albedo (Lab) | dE | Texturfaktor dE |
|---|---|---|---|---|
| 1 Haut | (55,49,52) 20,9 / 3,9 / -1,1 | (52,48,52) 20,2 / 3,3 / -2,0 | **1,3** | 2,3 |
| 1 Gelenk | (69,67,73) 28,8 | (62,66,73) 27,8 | **2,5** | 4,2 |
| 1 F1 | (132,85,103) 41,9 / 22,1 | (130,85,102) 41,4 / 21,2 | **1,0** | 11,3 |
| 2 Buckel | (123,72,87) 36,7 / 24,1 | (127,79,93) 39,3 / 22,4 | **3,1** | 2,1 |
| 2 Haut | (103,75,83) 35,0 / C 13,5 | (97,88,87) 38,2 / C 3,8 | 11,1 | 7,9 |
| 2 Gelenk | (118,88,99) 40,8 / C 13,9 | (117,111,114) 47,5 / C 2,7 | 13,1 | 11,9 |
| 2 Hals | (106,75,80) 35,4 / C 13,7 | (103,88,87) 38,6 / C 6,4 | 8,6 | 5,8 |
| 2 Kopf | (112,67,69) 33,7 / C 19,8 | (111,76,65) 35,6 / C 18,7 | 8,2 | 9,9 |

- **Bild 1:** Die Rechnung des Messers (Umkehr, Streulichtabzug, Division durch die Schattierung) ist reproduziert. Die Mediane der
  Albedo stimmen auf 1-2,5 dE.
- **Bild 2:** Beim Buckel stimmt es. Bei Haut, Gelenk und Hals liefert die Umkehr des Messers rund +10 a\* mehr Rot, als das Vorwaertsmodell
  braucht, um das Video zu treffen. Die Vorwaertskette verlangt dort fast unbunte Werte (C\* 3-6). Das ist ein Fehler der Umkehr, vermutlich
  aus der Rot-Mischregel von `video_umkehr` und dem Fuss der Rampe (G liegt bei EX 0,665 um LO_G = 17,7 e). Es ist keine Spielfarbe.

### 3.2 Verzerrung durch die Auswahl der Pixel (`py/g3s.py`, `g3s_auswahl.json`)

"Gueltig" heisst beim Messer: Streulichtanteil <= 50 %. Diesen Anteil berechnet er aus dem **beobachteten** Video. Dunkel beobachtete Pixel
fallen dadurch heraus, hell beobachtete bleiben. Das ist eine Auswahl an der abhaengigen Groesse.

Zum Vergleich habe ich dieselbe Messer-Albedo je Pixel (`e5 alb`) mit zwei anderen Auswahlen ausgewertet:
- Vorhersage-Auswahl: Der Streulichtanteil kommt aus einer Vorhersage mit konstanter Albedo, nicht aus dem beobachteten Pixel.
- ganzer Kern: alle Kernpixel ohne Uebersteuerung und Feuer.

| Bild / Material | Messer-Auswahl L\* / C\* | Vorhersage-Auswahl L\* / C\* | ganzer Kern L\* / C\* | Synthese, ganzer Kern L\* |
|---|---|---|---|---|
| 1 Haut | 20,9 / 4,1 (2 010 px) | **6,1** / 3,9 (3 227) | 10,7 / 4,2 (17 114) | 11,4 (C 1,9) |
| 1 Gelenk | 28,8 / 3,6 | 10,6 / 2,2 | 18,2 / 7,1 | 19,0 |
| 1 F1 | 41,9 / 22,2 | 25,6 / 9,2 | 25,6 / 7,6 | 29,5 |
| 2 Haut | 35,0 / 13,5 | 22,2 / 33,4 | 18,4 / 27,1 | 21,6 (C 21) |
| 2 Gelenk | 40,8 / 14,0 | 24,8 / 27,5 | 24,5 / 24,1 | 35,9 |
| 2 Buckel | 36,7 / 24,0 | 33,3 / 26,3 | 33,3 / 26,3 | 37,0 |
| 2 Hals | 35,4 / 13,9 | 18,8 / 27,2 | 20,6 / 24,8 | 29,1 |
| 2 Kopf | 33,7 / 21,2 | 25,5 / 38,0 | 17,3 / 28,1 | 15,9 |

- **Robust gegen die Auswahl** sind nur zwei Werte: der Buckel (Bild 2) und die geringe Buntheit der Haut in Bild 1 (C\* ~2-4).
- Alles andere verschiebt sich um 10-20 L\*. Die Unsicherheit des Messers (+-8,6 fuer Haut und Gelenk) deckt das nicht ab.
- Ganzer Kern in Bild 2: Die Haelfte der Pixel hat V_G < 8, liegt also im Fuss der Rampe. Dort gibt das Video nur eine Obergrenze her. Die
  hohen C\*-Werte der rechten Spalten sind darum ebenfalls unsicher (Rot-Bias, 2.1). Die Wahrheit liegt nicht einfach "rechts".
  Belegt ist nur: Die Spanne ist doppelt so gross wie angegeben.

### 3.3 Folgt das Video der Port-Schattierung? (Bild 1, Haut, alle Kernpixel)

| Port-Schattierung | Pixel | Video G Median | Messer-Albedo G Median | Anteil gueltig |
|---|---|---|---|---|
| 0-100 | 4 341 | 26 | 45,3 | 0,10 |
| 100-140 | 3 249 | 23 | 27,3 | 0,07 |
| 140-180 | 4 874 | 25 | 22,4 | 0,09 |
| 180-256 | 4 650 | 31 | 20,0 | 0,20 |

Die Schattierung im Port aendert sich um mehr als den Faktor 2, das Video kaum (23-31). Die Albedo je Pixel faellt darum mit der
Schattierung von 45 auf 20. **Der L0-Verlauf aus 5080/2 ist in Bild 1 nicht zu sehen.** Damit ist die Lichtdivision in Bild 1 an jedem
Pixel ein Artefakt der Schattierung und keine Materialmessung. Das ist die gemessene Ursache des Helligkeitswiderspruchs, den der Messer in
Offen 1 nicht aufgeloest hat. Die "2,1-fache Schattierung" in Bild 1 ist nicht belegt.

## 4. Zielfarben neben den Originalen (RE2-G4 Typ 0x34, EM30_HANGAR Haut ohne Kleidung)

`py/g4.py`, `g4_originale.json`, Bilder `g4_b{1,2}_{G4,EM30_Haut}.png`. Beide Originale stehen an Lage und Gier der K5e_f1 (Clip 0), mit
demselben Disc-Licht wie in e5, und laufen durch dieselbe eigene Vorwaertskette.

**Sichtbare Albedo** (L\* p10/50/90, C\* Median, C-gewichteter Ton, C/L):

| | Bild 1 (hinten) | Bild 2 (vorn) |
|---|---|---|
| G4 | 4,9 / 12,2 / 24,7; C 4,0; +14 Grad; C/L 0,23 | 8,2 / 27,8 / 65,6; C 13,2; +36 Grad; C/L 0,52 |
| EM30 Haut | 12,5 / 44,8 / 64,7; C 14,0; +24 Grad; C/L 0,31 | 13,1 / 39,6 / 64,7; C 12,8; +27 Grad; C/L 0,30 |

**Ziele des Messers:** Haut 23,4 / C 5,7 / -9 Grad (C/L 0,25). Gelenk 30,1 / 4,2 / -44 Grad (0,14). F1 41,9 / 22,2 / -5 Grad (0,53).
Buckel 36,7 / 24,1 / +1 Grad (0,66). Hals 35,4 / 13,9 / +9 Grad (0,39). Kopf 33,7 / 21,2 / +21 Grad (0,63).

- **Haut:** Die Buntheit entspricht G4 von hinten (C/L 0,25 gegen 0,23), das ist nicht bunt. L\* 23 liegt in der G4-Spanne, aber ueber dem
  G4-Median 12 hinten. Das passt zur Auswahl-Verzerrung (3.2). Der Ton -9 Grad ist **kuehler als beide Originale** (+14 bis +36 Grad).
- **Gelenk:** C/L 0,14 ist blasser als jedes Original. Wer das als Texel nimmt, erzeugt wieder "ausgeblasst".
- **F1, Buckel, Kopf:** bunt wie G4 von vorn (0,53-0,66) und ueber EM30 (0,30). Das ist nicht unplausibel. Rot ist dort aber nur
  Obergrenze, und die Gefahr von "zu bunt" haengt an genau dieser Grenze.
- **Video-Simulation der Originale** (Kern- und Wesen-Maske; die Silhouetten decken nur grob, IoU G4 0,41/0,33, EM30 0,25/0,19):
  - G4 Bild 1: dE 15,2, Sim L\* 10,1 gegen Video 12,5. Bild 2: dE 10,0.
  - EM30 Bild 1: dE 20,1, Sim L\* 22,9 gegen Video 11,0, also deutlich zu hell. Bild 2: dE 11,1.
  - Unter gleichem Licht passt das dunkle G4 zum Video. Die helle EM30-Haut passt in 5080 nicht.

**Ziele liegen im Kettenraum, nicht im Texelraum.** Aus e5 (`kette` gegen `textur_sichtbar` derselben Staende):
- Die Kette dreht den Ton um +4 bis +20 Grad ins Waermere.
- F1 wird in Bild 1 um 5-8 L\* heller. Dahinter steckt die unscharfe Nachbarschaft, geteilt durch die niedrige F1-Schattierung 101.
- K5f-Hals mit Textur-C\* 0 wird in Bild 2 zu C\* 21,7.

Fuer den **Vergleich** der Staende ist das fair, denn der Messer schickt sie durch dieselbe Kette. Die Zielfarben selbst sind aber keine
Albedo. Direkt als Materialfarbe gesetzt, kommen Ton und Helligkeit falsch heraus.

## 5. Staende ohne Auswahl nach Videowert (`py/g5.py`, `g5_ganz.json`)

Videosimulation der Staende (die Kette des Messers, `e5_b*_<Stand>.npz`) gegen das Video. dE76 Pixel-Median (dL\*):

| Szene, Pixel | K5c | K5e_f1 | K5f |
|---|---|---|---|
| Bild 1, alle Kernpixel (23 538) | **10,2 (-2,2)** | 31,3 (+23,8) | 28,3 (+20,7) |
| Bild 1, nur gueltig (4 550, Messer) | 18,7 (-13,4) | 20,8 (+13,7) | 20,2 (+9,3) |
| Bild 2, alle Kernpixel (20 521) | 11,8 (-2,7) | 12,5 (+2,2) | **11,2 (+0,6)** |
| Bild 2, nur gueltig (2 475, Messer) | 20,2 (-11,3) | **10,7** (-5,2) | 11,7 (-6,1) |

- Video-Ton Bild 2 (alle Kernpixel): h +7,9 Grad. Die Staende liegen bei -15,6 / -25,3 / -20,2 Grad.
- In Bild 1 sind 80 % der Wesen-Pixel im Video dunkel. K5e_f1 und K5f sagen dort +21 bis +24 L\* voraus, K5c trifft sie.
- **Vorbehalt:** Nach 3.3 ist die Port-Schattierung in Bild 1 nicht verlaesslich. K5c kann also auch nur deshalb passen, weil seine dunkle
  Textur eine zu starke L0-Schattierung ausgleicht. "K5c ist richtig" ist damit nicht belegt. Belegt ist, dass "K5c ist in jedem Material zu
  dunkel" ein Ergebnis der Auswahl ist.
- In Bild 2 trennt das Video die drei Staende in der Helligkeit nicht (11,2-12,5). Es trennt sie nur im Ton: Alle sind 23-33 Grad zu kuehl,
  wobei Rot in Bild 2 Obergrenze ist.

## 6. Folgerungen fuer das Messblatt

1. Abschnitt 1 (Bilder) und die Arithmetik von Abschnitt 4 (Bild 1) bleiben stehen.
2. **Zielfarben-Tabelle (0.4 / 5) zurueckziehen** oder als "obere Werte einer verzerrten Auswahl, im Kettenraum" kennzeichnen.
   - Die Unsicherheit ist mindestens die Spanne aus 3.2: L\* +-10-15 statt +-8,6.
   - Gelenk mit C\* 4 nicht als Ziel uebernehmen (4).
3. Den Satz "K5c ist in jedem messbaren Material zu dunkel" streichen (5). Die Aussage "K5e_f1 vor K5f am Hals" haengt an der Auswahl und
   an Rot als Obergrenze. Ohne Auswahl ist Bild 2 unentschieden.
4. **Offen 1 neu fassen:** Der Helligkeitswiderspruch hat eine gemessene Ursache. Das Video zeigt den L0-Verlauf aus 5080/2 nicht (3.3).
   Die Wesen-Lage oder das Licht von Bild 1 ist im Port-Modell falsch.
5. **Bild 2:** Die Umkehr hat einen Rot-Bias von rund +10 a\* bei Haut und Gelenk (3.1, gegen die Synthese). Das ist mehr als das
   "R +13-22 e" am schwarzen Oberteil ahnen laesst.
6. **Was sich ehrlich sagen laesst:**
   - Die Haut der Rueckseite ist im Video fast unbunt (C\* ~2-4, auswahlunabhaengig) und dunkel. Es gibt keinen Beleg fuer "heller als K5c".
   - Bild 2 ist waermer im Ton als alle drei Staende.
   - Der Buckel liegt bei L\* 33-39 / C\* 22-26 / h ~0 Grad (auswahlfest, Rot ist Obergrenze).
7. Die Nutzerhilfen des Messers bleiben richtig. Am wichtigsten ist Punkt 5 (ein Dia in 5090 ohne Feuer, also mit exakt bekanntem Licht).
   Dazu kommt bewegtes Material: Es ersetzt die Pixelauswahl durch Mittelung ueber Posen.

Nicht angefasst: `Version_80%/`, `pri/`, `synchro/`, die Dateien des K5f-Bauers (`generator/kf_k5f*`, `k5f/`), `build/` und die
Spiel-Assets. Es wurde nichts gebaut und nichts im Spiel gestartet.
