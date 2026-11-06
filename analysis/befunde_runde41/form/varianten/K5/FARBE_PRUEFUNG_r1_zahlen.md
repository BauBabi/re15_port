# Pruefung K5c Farbe Runde 1, Zahlen-Pruefer (2026-11-06)

Geprueft wurden die Commits bd5e5580c, 425d7c2cc und c46a9d2fc auf r41/form. Die Variante liegt unter `farbe/K5c_farbe/` und ist dieselbe wie `formfarbe/emd/K5C_FARBE.EMD/.TIM` im Scratchpad.

Eigener Code (nicht committet), Ordner `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/pr1/code/`:
- `eigen.py`: eigener Leser fuer EMD-MD1 und TIM, eigene Texelsammlung.
- `geo.py`: Vergleich der Geometrie.
- `atlas.py`: A1 bis A12.
- `kette.py`: K1 bis K10 mit eigenen Zonen und eigener Statistik.
- `zusatz.py`: Zusatzmessungen.

Die Kette nutzt die gemeinsamen Werkzeuge `form_pruefung.Kandidat.rendern` und `form_video`. Das ist die Definition aus dem Messblatt, nicht der Code des Bauers.

## Urteil: NICHT BESTANDEN

Grund ist nur Punkt 4: Konstanten ohne Quelle, davon eine wirksam. Alle 24 Riegel sind mit eigenem Code nachgemessen GRUEN.

## 1. Riegel nachgemessen (eigener Code)

| Riegel | eigen neu | Bauer neu | eigen K5c | Soll | Bewertung |
|---|---|---|---|---|---|
| A1 C* Koerper | 12,93 | 12,9 | 4,88 | 9,5-21 | GRUEN |
| A2 sigma_ab je Teil (26 Teile) | 11,85 (min 6,17) | 11,9 | 3,94 | >= 7,0 | GRUEN |
| A3 Fleischrot | 15,1 % | 15,1 % | 0,5 % | 8-40 % | GRUEN |
| A4 Rot-Farbton | 19,1 Grad | 19 | -9,9 | 15-50 | GRUEN, knapp: Median 16 Grad, 47 % der Rottexel unter 15 Grad (Familie 25-48) |
| A5 Neutralweiss | 0,00 % | 0,00 % | 0,80 % | <= 0,20 % | GRUEN, aber nur durch Stauchung, siehe 6.1 |
| A6 HF_fft Koerper/Kopf/Kiefer/Kralle | 0,164/0,109/0,069/0,063 | 0,164/0,109/0,068/0,063 | Kiefer 0,462, Kralle 0,322 | 0,04-0,26 | GRUEN |
| A7 HF_rest | max 0,092 | max 0,092 | 0,437 | <= 0,19 | GRUEN |
| A8 Kiefer Zahn/Rot/Dunkel | 2,7/36,3/43,6 % | 2,7/33,8/44,5 | 77,1/0/0 | 2-20/>=20/>=20 | GRUEN; die 2,5 Punkte Abstand beim Rot kommen aus der 5-Bit-Umrechnung (x255/31 gegen x8) |
| A9 Auge L*/C* | 13,3/17,9 | 13,1/18,0 | 56,3/53,5 | <=30/<=30 | GRUEN |
| A10 Texeldichte | 0,64-2,33 | gleich | gleich | 0,5-2,7 | GRUEN (UV bytegleich) |
| A11 Schwanzsprung | 0,46 (Glieder 10,9/10,9/10,5/10,0/10,4, Spitze 17,2) | 0,5 | 3,35 | <= 4,0 | GRUEN |
| A12 Farbkorn | 4,38 | 4,40 | 2,62 | <= 5,7 | GRUEN |
| K1 Bild 1 C* | 8,61 (Original 9,25) | 8,6 | 6,31 | 7,9-11,1 | GRUEN |
| K2 Bild 1 ab-Streuung | 9,85 (Original 9,83) | 9,9 | 6,35 | >= 8,36 | GRUEN |
| K3 Bild 1 dE je Zone | max 1,33 (Hinterbein l (W)) | 1,3 | 8,24 | <= 6 | GRUEN |
| K4 Bild 1 Rotklasse | 6,86 % (Original 5,25) | 6,9 | 2,49 | 3,15-7,87 | GRUEN |
| K5 Ruecken hell / geclippt | 6,8 / 0,10 % (Original 9,9 / 0,54) | 6,8/0,1 | 21,9/1,0 | <=14,9 / <=0,80 | GRUEN |
| K6 Bild 2 dE / C* | 2,73 / 22,7 (Original 25,6) | 2,7/22,7 | 9,86/16,7 | <=6 / >=21,7 | GRUEN |
| K7 Unterkiefer dE / a* | 0,10 / 24,8 | 0,2/24,8 | 12,3/13,7 | <=6 / >=21,2 | GRUEN |
| K8 Oberkiefer dE | 0,64 | 0,6 | 12,1 | <= 6 | GRUEN |
| K9 Klauen a* r/l (W) | 24,1 / 22,5 | 24,1/22,5 | 13,8/22,8 | >=18,1 / >=16,6 | GRUEN |
| K10 Augen L* max | 13,5 / 15,9 | 13,5/15,9 | 51,5/36,2 | <= 30 | GRUEN |
| N1 neutral C* | 10,6 / 9,7 / 10,6 (EM30 9,9/12,5/10,4) | 10,7/9,8/10,7 | 4,6/4,9/4,0 | >= 0,75 x EM30 | GRUEN, Seite knapp (9,7 gegen 9,4) |
| N2 neutral L*>85 | 0 / 0 / 0 % | 0/0/0 | 0,81/1,29/0,11 | <= 0,10 % | GRUEN |

**Robustheit.** Die Kette mit einem anderen Rausch-Saatwert (Saat 12345) ist ebenfalls 10/10 GRUEN (K10 15,4/16,4).

**Gegenlauf mit dem Bauer-Skript.** `riegel_farbe.py` habe ich selbst laufen lassen: 24/24 GRUEN, Gegenprobe 3/24, Exit 0. Das ist identisch mit der Meldung des Bauers.

**Gegenprobe K5c heute** (eigene Messung):
- ROT bei A1-A9, K1-K10, N1 und N2.
- GRUEN nur bei A10-A12; das sind die Halteriegel.
- Der Riegel misst also tatsaechlich etwas.

## 2. Geometrie und Spiel-Asset

**FORM.EMD**
- sha256 a71f8d08... ist gleich fuer alle vier Dateien: K5c, K5c_farbe, farbe/K5C_FARBE.EMD und die EMD im Scratchpad.
- Eigener Parser: 32 Meshes, 3828 Primitive (6386 Dreiecke).
- Vertex-Diff 0,0; Primitiv- und UV-Bytes ohne Unterschied.
- EMD-Abschnitte 0-7 (Skelett, Clips, Modell) sind bytegleich.

**FORM.TIM**
- Gleiches Layout: 8 bpp, 512 x 256, CLUT 4 x 256 bei (0,480).
- 85 948 genutzte Texel. Kein Texel nutzt mehr als eine CLUT-Zeile, die Messung sieht also genau die Spielfarbe.
- Kein genutzter Texel liegt auf Index 0; ungenutzte Texel stehen unveraendert auf 0.
- Zwei CLUT-Eintraege sind 0x8000. Das ist Schwarz mit STP, also opak und kein Loch.

**Spiel-Asset** `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`
- Blob 1fba8a34 / 4e7db099, gleich in 19a9b2b17, HEAD und Arbeitsbaum.
- Kein Diff unter re15_port seit 19a9b2b17.

## 3. Tests

- Bau: `bash re15_port/tools/local_build.sh build` ergibt LOCAL-BUILD-OK.
- `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600`: 11/11 bestanden, nichts wiederholt, 95 s.
- Diese Tests pruefen den unveraenderten Bestand, nicht die neue Textur.

## 4. Konstanten im Generator-Diff (3215ec1da..HEAD, generator/)

**Mit Quelle** (Messblatt, Familie oder Bild):
- SIG_GROB, G_AB 1,48, L_MAX_KOERPER 70
- G4_HAUT, G4_MAUL, G4_KRALLE
- H_MIN, H_WARM, H_MAX, H_BLAU, C_VIO, C_REST
- ZAHN_LAB, AUGE_LAB, AUGE_PUNKTE, AUGE_R
- Zahnverlauf 0,6; Grenzen N1 0,75 und N2 0,10

**PORT-WAHL nur mit Laufbeleg, ohne Nutzerentscheid:**
- K_FEIN 0,5, G_L 1,0
- SIG_ERG 6, SIG_AB_FEIN 0,7, SIG_NAEHE 2, SIG_FLEISCH 1,5
- K_VERDECKT 1,0
- Kiefer naehe^3
- Daempfung 0,8
- Augen-Reserve 26

**Ohne jede Quelle** (kf_mal.py):
- `np.clip(g, 0, 2.5)` in `eichen` **greift im Ergebnis**: Hinterbein l (W) und Klaue l (W) stehen in bau.json auf g = 2,5. Ein Grenzwert ohne Beleg bestimmt also die Aderstaerke zweier Zonen.
- `np.clip(fleisch_k, 0.2, 1.5)` und `max(4.0, auge_L ...)` greifen derzeit nicht.
- Die Schwellen `deck > 0.2`, `deck / 0.5`, `cov / 0.5` ("ab 50 % voll") und `gd / 0.05`. Fuer 0,05 steht im Kommentar nur "~ Gewicht eines einzelnen Bildpunkts", gemessen ist der Wert nicht.
- Die Stauchung oberhalb L* 70 mit Steigung 0,25.
- Die Kachel-Versaetze `7 * i`, `np.roll(P, 11 * j)` x5, `np.roll(P, 9 * j)` x7, `auf_hoehe(..., 5)` und die Ueberblendung `blend=4`.
- D_MAX (45, 20, 20) ist mit "groesster gebrauchter Wert" begruendet, also selbstbezueglich. Greift derzeit nicht.

Der Auftrag verlangt fuer jede Konstante eine Quelle oder eine PORT-WAHL mit Nutzerentscheid. Bei diesen Konstanten fehlt der Nachweis, deshalb nicht bestanden.

## 5. Tafeln (Darstellungsregel Messblatt 7)

**kf_tafeln.py**
- Kein Gamma: render.blick laeuft mit gamma=None, jede Kachel ist mit "roh, kein Gamma" beschriftet.
- 01/02: Original | K4 | neu. Zeile 1 mit sauberem Spiellicht, Zeile 2 durch video_filter. Regel 7.2 erfuellt.
- 03: EM30 | G5 | G4 | neu | K4 mit gleichem Renderer und Licht, Umkugel 0,46, 3 Winkel, L*/C*/L*>85 je Kachel. Regel 7.3 erfuellt.
- 04: Atlas in gleicher Texelskala.
- 05: Rundum mit EM30 unter gleichem Licht.
- 06: Bild-2-Ausschnitt x 200-360, y 290-420, 3-fach: Original | T | neu Kette | K5c Kette. Regel 7.4 erfuellt.

**07_spiel.png**
- Alle 5 Kacheln sind pixelgleich mit `farbe/spiel/*.png` (mittlere Abweichung 0), also roh.
- Das Skript, das 07 zusammensetzt, ist nicht versioniert.
- 07 zeigt kein Vergleichsbild (Original oder K4) daneben.

**spiel.log**
- Belegt sind nur 4 Laeufe; "5/5 Laeufe rc=0" ist im Log also mit 4/4 belegt.
- `spiel_pose1_nachstellung_r5080.png` (18:51:20) fehlt im Log, weil `[ -f ] ||` den Lauf uebersprang.
- Zeitlich stammt das Bild aber aus der Endtextur (TIM 18:35:41).

## 6. Sachbefunde jenseits der Riegel (fuer Runde 2)

1. **Glanzflecken sind nur abgedunkelt, nicht entfernt.**
   - Die 493 Koerpertexel, die in K5c neutralweiss waren, liegen jetzt bei Lab (72,9, 7,9, 1,5), p90 L* 80,2.
   - 96,6 % davon haben noch L* > 60.
   - Koerper mit L* > 70: 1,43 % (K5c 1,18 %).
   - A5 und N2 bestehen also durch die Stauchung oberhalb L* 70. Der rueckprojizierte Glanzfleck bleibt als hellrosa Fleck, sichtbar in 03/05 am Ruecken in der 3/4-Ansicht von vorn.
2. **Die K-Riegel sind Eichziele.**
   - `kf_mal.py eichen` regelt je Nutzerzone Lab-Versatz und Aderstaerke direkt auf die Pruefer-Kette (dE 0,1-1,3). K1-K10 sind damit Ergebnis der Anpassung, kein unabhaengiger Beleg.
   - Augenumfeld: Alle Texel unter r <= 6 px um die E24-Punkte bekommen die Augenfarbe. Das ist an die Pose von Bild 2 gebunden.
3. **Rot-Farbton am unteren Rand:** Mittel 19, Median 16 Grad (Familie 25-48). Ursache ist das Drehen von Magenta auf 10 Grad.
4. **Neutraler Render dunkler als die ganze Familie:** L* 16,1/13,8/14,3 gegen EM30 24-29 und G4 17-21,6 (Tafel 03).
5. **Abstaende in Bild 2 ausserhalb der Riegel:**
   - Figur ab-Streuung 11,7 gegen Original 15,4 (0,76-fach).
   - Unterkiefer ab-Streuung 14,0 gegen 21,2.
   - Klauen b* 10,6/8,7 gegen 21,9/19,6. Laut Messblatt Feuerschein und kein Riegel.
6. **Beiger Streifen im Kopffeld:** x 500-511, 1212 Texel, Lab (60,7, 11,7, 16,6). Das ist ein Rest des K5c-Gaumenstreifens, sichtbar am rechten Rand von Atlas 04.
7. **Rotklasse in Bild 1 je Zone:** Schwanz 9,5 % gegen Original 5,9 %, Hinterbeine 0 % gegen 1,1-1,9 %. Fuer die ganze Figur haelt der Riegel.
