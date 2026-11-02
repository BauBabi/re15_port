# Bewertung Linse BILDVERGLEICH

Unabhaengiger Pruefer, an keinem Kandidaten beteiligt. Stand 2026-10-31. Pixel: Originalbild 640x480.
Belegbilder: `bewertung_bild/`. Kein Commit. Kein RE-Wert, nichts hiervon gehoert in die Engine.

## 1. Ergebnis

**Keiner sieht 1:1 aus wie die Screenshots. Am aehnlichsten ist C, mit 0,7 Punkten Abstand. A und B liegen praktisch gleichauf, aber aus entgegengesetzten Gruenden.**

| | Bild 1 | Bild 2 | Gesamt |
|---|---|---|---|
| A | 7,1 | 4,7 | **5,9** |
| B | 5,3 | 6,7 | **6,0** |
| C | 7,2 | 6,2 | **6,7** |

- Skala: 10 = vom Screenshot nicht zu unterscheiden. Gesamt = Mittel beider Bilder. Punkte sind mein Urteil am Bild, die Zahlen unten stuetzen es nur.
- Zaehlt Bild 1 doppelt, weil dort die Figur frei steht: A 6,3, B 5,8, C 6,8. C bleibt vorn, A und B tauschen.
- A gewinnt die Masse in Bild 1 und verliert Bild 2 durch den Block rechts. B gewinnt Bild 2 und ist in Bild 1 unten zu mager. C hat keinen schweren Totalausfall, ist aber nirgends am besten ausser bei Luecken und Ton.

## 2. Verfahren

- Tafeln nachgerechnet: `form_pruefung.py pruefe` fuer alle drei, Ausgabe im Scratchpad. Alle sechs `vergleich_bild*.png` sind pixelgleich zum Neulauf (MAE 0,0), die Original-Tafel ist `bild1.png`/`bild2.png` (MAE 0,0). Die Tafeln zeigen also die abgegebenen EMD und TIM.
- Vergroessert verglichen: Gesamtfigur 2-fach, Partien 3- bis 6-fach, dazu 1:1 fuer die Gestalt.
- **Einsatzprobe**, wichtigster Blick dieser Pruefung: Figur durch `video_filter`, Rand weich, direkt ins Original gesetzt. Reste der echten Figur um den Render zeigen fehlende Masse. So stoert weder der ausgemalte Hintergrund noch der fehlende Schatten oder das fehlende Feuer. `bewertung_bild/e1.png`, `e2.png`.
- Kontur jedes Renders aufs Original: `k1.png`, `k2.png`, `lu1.png`, `lu2.png`.
- Kennzahlen des Werkzeugs entscheiden hier nichts: IoU, Punkte und Zeichnung sind auf dieselben Masken gezogen (so sagt es auch Dossier C). Ich habe vor allem die grauen Zonen angesehen, die das Werkzeug nicht zaehlt.

## 3. Bild 1

Die Figur steht frei, darum wiegt dieses Bild am meisten. Oben: `e1.png`, `g1.png`.

| Punkt | A | B | C | Beleg am Bild |
|---|---|---|---|---|
| Umriss | 8 | 5,5 | 7,5 | Oben alle eng. Unterkante NL / NR: A 429 / 443, C 423 / 437, B 410 / 422. Soll B3 bis 426, C4 bis 448 (weich). B endet 16-26 px zu frueh |
| Massenverteilung | 8 | 5 | 7 | Dunkel im Original ohne Figur: NL unten A 1346, C 1940, B 2675 px; NR unten A 1865, C 2243, B 2696 px. Enthaelt Bodenschatten, gilt nur im Vergleich. In 1:1 wirkt B hager, A und C wuchtig wie das Original |
| Beine | 7,5 | 5 | 7,5 | FR: alle gut, B am saubersten, C mit rotem Fuss. NL: A und C zwei Lappen plus Innenglied, B nur kleine Pfote mit Kerbe. NR: C am besten (Taille, runde Masse), A mit Stiefelform, B mit gestufter "Daumen"-Kante statt der schraegen Oberkante (285,368)-(325,345) |
| Luecken | 7 | 4,5 | 8 | L1 Oberkante bei x=369 / 376: Original 321 / 320, C 313 / 309, B 308 / 302, A 303 / 299. L2: A Dreieck wie Original, Spitze y=314; C oben flach, y=318; B offen, Schlitz x 285-288 von y=372 bis zum Boden. Falsche Oeffnung NL in (222-262,372-412): B 539 px, C 88, A 41 |
| Zeichnung | 7 | 6 | 6 | Z1: A trifft Groesse und Winkel, B und C ein Drittel der Flaeche. Z3 mit Z4: B wie Original, C quer. Z7, Taille: C. Z5, Z8: B. Stoerzeichnung nur A und C. Haut: C am reichsten, B am flachsten. Einzelwerte Abschnitt 5 |
| Helligkeit | 5 | 6 | 7 | Gleiche Pixel (Ziel weiss): Y Original 42,5, C 42,3, B 39,4, A 37,2. Anteil Y<30: Original 0,40, C 0,47, B 0,54, A 0,58 |
| **Mittel** | **7,1** | **5,3** | **7,2** | |

## 4. Bild 2

Die Figur ist dunkel auf dunkel, lesbar sind Rand, Zeichnung und die Stellen vor hellem Grund. Oben: `e2.png`, `g2.png`.

| Punkt | A | B | C | Beleg am Bild |
|---|---|---|---|---|
| Umriss | 5 | 7,5 | 6 | Figur rechts von x=415: A 8301 px bis x=471, C 1672 px bis x=444, B 385 px bis x=426. B sitzt rechts genau am Feuerrand. Unten dagegen B bis y=479 ins Logo, C y=434, A y=400 |
| Massenverteilung | 4 | 6,5 | 6,5 | Mittel Rot Original gegen Filter. Flaeche Y: 102, B 99, C 133, A 212. Zone (345-405,345-380): 137, C 135, A 194, B 199. Band (210-250,376-384): 159, A 169, C 194, B 48 |
| Beine | 3,5 | 5,5 | 5 | Sichtbar ist kein Bein. A: Pfeiler im Feuer (Deckung 0,99 und 0,45), Knie auf der V-Form, Y bleibt leer. B: Saeule bis y=440 rund 60-73 px breit, kreuzt das Band. C: Spindel liegt auf Y, zwei getrennte duenne Beine rechts, Fuss FR schwebt (Dossier C) |
| Luecken | 4 | 6 | 7,5 | Querfuge (225-255,402-416), im Original verdeckt: A sichtbar (Deckung 0,00), B und C 1,00. Band frei: C 0,00 Deckung, A 0,24, B 1,00 |
| Zeichnung | 5,5 | 7,5 | 6 | Haken, helle Flaeche: Original 611 px, B 620, A 814, C 1282. Zug Q: B duenn und durchgehend, C gut bis Q5, A zu breit. G-Dicke im Blau: Original 12,9, A 12,0, B 14,6, C 9,6. B hat die unregelmaessigste Kontur, echte Segmente hat keiner. Q2: B flach, C mit Loechern |
| Helligkeit | 6 | 7 | 6 | Gleiche Pixel: Y Original 26,7, alle 22-23, kein Unterschied. Streuung 20,4 gegen A 17,8, B 20,0, C 20,7. A oben zu schwach, B Q1 und Q2 etwas zu hell, C Haken und Q2 zu hell |
| **Mittel** | **4,7** | **6,7** | **6,2** | |

Blau-Anhebung (B-R/2) gegen Original, nur Anhalt, die Toene sind geregelt: Zug Q rechts A 0,31, B 0,43, C 0,44; Q1 0,48, 0,69, 0,68; Haken 0,56, 0,64, 0,55; G 0,71, 0,63, 0,67.

## 5. Was jeder falsch hat

**A**
- Bild 2: Block rechts ist der schwerste Einzelfehler dieser Pruefung, die Figur wird dort zur zweiten Masse. Das Feuer im Spiel deckt ihn zum Teil, die Form bleibt fremd.
- Bild 2: Fuss Y fehlt, stattdessen Knie bei (236-289,360-400) auf der V-Form. Haken ein Drittel zu gross, fette 7 ohne rotes Inneres. Q1 und Q2 eckig und schwach. G glatt ohne Glieder.
- Bild 1: L1 steigt rechts als Finger bis y=299, Original 320. Dort ist das Original dunkel.
- Bild 1: helles Scheitelband (306-342,200-209) Y 70 gegen 48, Dreieck (350-360,211-225) Y 76 gegen 48. Das ist Bild-2-Zeichnung, die in Bild 1 durchscheint.
- Bild 1: Z1 rund 10 px zu hoch und zu gesaettigt. Taille in zwei Punkte zerfallen. Loch (242-246,378-386), 41 px. Figur zu dunkel.
- Sauber ansehen (`sa1.png`, `sa2.png`): Haut wirkt wie Fels mit grauen Scherben.

**B**
- Bild 1: unten zu kurz und zu mager, das ist der schwerste Fehler. Ball NL zu klein, offene Kerbe bei (222-261,374-411), wo das Original Figur zeigt.
- Bild 1: L2 ist kein geschlossenes Loch, der Schlitz reisst bis zum Boden durch. "Daumen" an NR gestuft.
- Bild 1: Z1 nur ein Drittel der Flaeche (219 gegen 670 px). Unter der Taille zweiter heller Fleck, im Original nur ein Band.
- Bild 2: Saeule kreuzt das helle Band (Deckung 1,00, Rot 48 gegen 159) und laeuft bis y=479. Rechts von Y bleibt x 257-282 unbedeckt.
- Bild 2: Q2 grosse flache Weinrot-Flaeche, P zweigeteilt.
- Rundum (`kandidat_b/rundum_hell.png`) wirkt die Figur wie ein Affe auf Faeusten. Reiner Seheindruck, nichts gemessen. Der Gorilla ist als Vorlage ausdruecklich verworfen, das sollte der Nutzer selbst ansehen.

**C**
- Bild 1: Kuppel ist ein glattes Ei, gesprenkelt statt zerkluefteter Massen. Stapel liest sich als Kiesel: Z3 liegt quer, Winkel 160 gegen 126.
- Bild 1: Z1 klein und falsch gedreht, 21x16 bei Winkel 156 gegen 44x21 bei 120. Fortsatz als Strich bis zum Scheitel, Y 95 gegen 82.
- Bild 1: L2 oben flach. Unter L2 fast derselbe Schlitz wie bei B, x 284-290, nur um y=380 geschlossen. Fuss FR rot statt dunkel.
- Bild 2: Haken ist ein gefuellter Ballon, doppelt so gross wie im Original, Rand ringsum statt nur oben und rechts. Das faellt in 1:1 sofort auf.
- Bild 2: G ein Viertel zu duenn und ohne Glieder. Q2 mit Loechern. Spindel unten links liest sich als Flosse. Beine rechts zu duenn.
- Rundum: Haken als Pilz auf Stiel, Rundling als Spindel (`bewertung_bild/rundum_hell_c.png`, neu gerechnet, die Abgabe hat nur `rundum.png`).

**Alle drei**
- Haut im Video zerfasert, mit vielen kleinen Kanten, in allen Renders zu flaechig. Typisches Feindetail Bild 1, Median |Bandpass Sigma 1-2| auf gleichen Innenpixeln: Original 1,05, C 0,55, A 0,44, B 0,31. Bei B steckt die Energie fast nur in wenigen Marken, Kurtosis 15 gegen Original 6, A 6,7, C 8.
- V-Form F und die Fortsetzung von G bis (278,398) fehlen.
- Zug Q ist in allen Renders deutlicher als im Video.
- Bild 2 im Mittel 4 Stufen Y zu dunkel. Das Beta-Licht ist nicht belegt.
- Kein Modellfehler: fehlender Bodenschatten, fehlendes Feuer, heller Fleck des ausgemalten Hintergrunds. Differenz-Tafeln dort nicht werten.

Einzelwerte Zeichnung Bild 1, Rosa mit R-G>38 und R>95, Weisskern mit Y>150, Laenge x Breite = 4 Sigma:

| | Original | A | B | C |
|---|---|---|---|---|
| Z1 Flaeche | 670 px | 682 | 219 | 231 |
| Z1 Mitte | (318,240) | (323,231) | (320,235) | (320,232) |
| Z1 Mass | 44x21 | 38x23 | 21x14 | 21x16 |
| Z1 Winkel | 120 | 120 | 119 | 156 |
| Z3 Flaeche | 223 px | 167 | 275 | 209 |
| Z3 Mass | 22x14 | 16x13 | 24x15 | 18x15 |
| Z3 Winkel | 126 | rund | 120 | 160 |

## 6. Das Beste zum Uebernehmen

**A**
- Untere Massen Bild 1: Kugel NL mit Innenglied, Rundling, Unterkanten. Kleinste fehlende Masse.
- L2 als geschlossenes Dreieck.
- Z1 in Groesse und Winkel samt magenta Auslaeufer, aber 10 px tiefer und weniger gesaettigt.
- Matte Querstreifen A15, A16 auf C3 in dieser Staerke.

**B**
- Zug Q als duenne durchgehende Linie, Q4, in Bild 2.
- Haken als duenne 7, Flaeche passt fast genau.
- Rechte Kante Bild 2: keine Masse im Feuer.
- G in Lage und unregelmaessiger Kontur. Segmente fehlen auch hier.
- Z3 und Z4 als Komet, Keil Z5, Reihe Z8, Fuss A2.
- Saubere Scheitelzone in Bild 1 ohne Stoerzeichnung.

**C**
- L1-Oberkante, am naechsten am Original.
- Ton Bild 1, Y 42,3 gegen 42,5.
- Taille als ein Band A14, Bogen Z7 auf der Kugel.
- Masse unten links in Bild 2: deckt Y zu 0,62 und laesst das Band frei, als einziger Kandidat beides.
- Dunkler Ton in Zone (345-405,345-380), als einziger getroffen. Form dort nicht uebernehmen.
- Gefuelltes rotbraunes Hakeninneres, das nur C hat, aber auf rund die Haelfte verkleinert und nur oben und rechts mit Rand.

**Haken bei der Uebernahme:** Zug Q, Haken und rechte Kante von B sitzen auf Ende F (Sicht Ende), bei A und C auf Seite R (Sicht Flanke). Die Bildlage gilt weiter, Netz und Textur sind nicht uebertragbar. Dort muss neu gemalt werden.
Zur Sichtfrage sagt mein Vergleich nur eines: B erreicht sein bestes Bild 2 fast mit der Pose aus Bild 1 (Dossier B), A und C brauchen starken Umbau. Das ist ein Plus fuer Sicht Ende, aber kein Beweis, das Formblatt laesst die Frage offen.

## 7. Korrekturen am eigenen Blick

- Z1: Am Bild hielt ich A fuer zu gross und B fuer richtig. Gemessen ueber fuenf Schwellen ist es umgekehrt, das Rosa des Originals ist groesser, als es wirkt. Bei den beiden hoechsten Schwellen hat A zu viel, B und C haben dort nichts mehr. Also: A richtig gross, aber zu kraeftig.
- Zone (345-405,345-380) in Bild 2: Die Beine von C wirkten falsch. Im Ton liegt dort aber nur C richtig. Die Form bleibt fraglich, das Video zeigt nur einen weichen Uebergang.
- G bei C: Y-Schwelle taugt nicht, der rote Boden liegt selbst darueber. Im Blau bestaetigt: zu duenn.
- G bei B: Ich las Gliederung, Dossier B sagt, die Segmente seien nicht gemalt. Sichtbar ist nur eine unregelmaessige Kontur.
- Haut: Ich hielt A fuer am kleinteiligsten. Gemessen ist es C. Die Gesamtenergie taeuschte, erst der Median zeigt, dass bei B und C wenige starke Marken sie tragen, bei C aber auch die Flaeche dazwischen mehr Detail hat.

## 8. Nicht bewertet, aber aufgefallen

- `kandidat_a/spiel_pose1_room5080_schnitt1.png` ist **komplett schwarz**: Fensterbereich Maximum 0, nur weisse Seitenstreifen. Dossier A fuehrt die Datei als Abgabe fuer Schnitt 1. Damit ist Pose 1 von A im Spiel mit diesem Bild nicht belegt.
- Spielbilder, Animation, Gelenke und Rahmen liegen ausserhalb dieser Linse. `rundum_hell` habe ich nur fuer die Uebernahmeliste angesehen.

## 9. Grenzen

- Die Punkte sind Urteil, nicht Messung. Der Abstand A zu B ist keine Aussage. Der Vorsprung von C ist klein und haengt am Fehlen eines schweren Totalausfalls.
- Bild 1: Zonenzaehlungen enthalten Bodenschatten. Am Boden heisst dunkel nicht Figur (Formblatt 1.2).
- Bild 2: Feuer steht im Spiel teils vor der Figur. Darum wiegen Stellen in Feuerzonen nur begrenzt. Das Band koennte Feuerschein vor der Figur sein, dann traefe der Einwand gegen B nicht. Das ist nicht entschieden.
- `video_filter` trifft das Blau der Figur in Bild 1 nicht (Original B 64, Render 52-58). Blau gehoert dem Video, ich habe es nicht gewertet.
- Geprueft sind nur die zwei Kameras. Rueckseite und Unterseite sagen die Screenshots nichts.

## 10. Dateien

| Datei in `bewertung_bild/` | Inhalt |
|---|---|
| `e1.png`, `e2.png` | Einsatzprobe, Figur ins Original gesetzt |
| `k1.png`, `k2.png` | Kontur auf dem Original |
| `lu1.png`, `lu2.png` | Luecken L1, L2, Unterform NL |
| `sch.png`, `stapel.png` | Scheitel, Z1, Stapel, Taille |
| `z2_kuppel.png`, `z2_haken.png`, `z2_g.png` | Bild 2 Rand, Haken, G |
| `g1.png`, `g2.png` | Gestalt 1:1 |
| `sa1.png`, `sa2.png` | saubere Renders |
| `rundum_hell_c.png` | Rundum C, aufgehellt, neu gerechnet |
