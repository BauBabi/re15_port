# Prüfung R9-Einbau (Variante 1) in K5g3, Runde 1 – Zahlen

Unabhängiger Zahlen-Prüfer, 2026-11-15. Nicht committet.
Eigener Code: `scratchpad/k5g2/kr4/pruef_r9einbau_zahlen/eigen.py`, `r16.py`.
Nachgerechnet habe ich außerdem w1/w2/w3 des Bauers, Ausgaben in `w1.out`, `w2.out` und `w3.out` im selben Ordner.

## Ergebnis in einem Satz
Ein Einbau hat nicht stattgefunden, deshalb lassen sich die Prüfpunkte 1 bis 8 nicht prüfen. Die Erklärung des Bauers, warum der Arm bei Variante 1 nicht mehr verbunden aussieht, habe ich mit eigenem Code nachgemessen. Sie stimmt bis auf eine Rundungsangabe, die nichts ändert.

## Stand (gemessen)
- Zweig r41/form, HEAD 968001097. Seit dem 14.11. gibt es keinen neuen Commit, unter re15_port/ gibt es keine Änderung.
- Das Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` hat sha f277916d, also weiter K5e_f1. Zuletzt geändert wurde es in Commit 62bd0001b.
- R9 (2f807af5) ist nicht im Spiel.
- Die Prüfpunkte (1) Boden, (2) N52, (3) Posen, (4) Netz 18, (5) Riegel/Generator, (6) Asset/ctest, (7) Dreiecke/5090 und (8) Rampen entfallen, weil es keinen Kandidaten gibt.

## Nachmessung der Ursache (eigener Code)
Der Winkel-Pool R9 gegen K5c, 771 Keys:
- Knochen 17 und 18 sind in 78 Keys geändert (309..556).
- Die Krallen 19/20/29/31 sind in 43 Keys geändert (398..556).
- Der Oberarm 16 ist in keinem Key geändert.

Bild 2 = Clip 12 Bild 0, Key 542:

| | K5c | R9 (Variante 1) |
|---|---|---|
| Weltdrehung 16 | – | 0,0 Grad (Matrix bitgleich) |
| Weltdrehung 17 | – | 53,6 Grad |
| Weltdrehung 18 | – | 18,7 Grad |
| Ellbogengelenk | [1516,-1484,-350] | unverändert |
| Handgelenk 3D | [1666,-588,-992] | [1400,-379,-333] |
| Handgelenk im Bild (Gelenkpunkt projiziert) | 277,7/371,3 (in der Zone x 207-279) | 307,0/363,5 (rechts außerhalb) |

Für die Armzone habe ich einen eigenen Tiefenpuffer gerechnet (nächstes vz je Pixel, also ein anderes Verfahren als der Maler des Bauers). Die Zone hat 4276 px.
- K5e_f1: gefüllt 0,858, leer 551, obere Zone (bis Zeile 390) 0,80, Arm ab Zeile 370
- K5c: gefüllt 0,896, leer 390, obere Zone 0,80, Arm ab Zeile 370
- R9: gefüllt 0,565, leer 1805, obere Zone 0,08, Arm ab Zeile 384

Das stimmt mit dem Bauer überein (0,565 / 1805 / 0,08 / Zeile 384) und mit dem R9-Messprotokoll (K3e ROT 0,5648).
Die R9-Gewichte in st10.py habe ich bestätigt: zone 1 (Zeile 145), boden 400, k4 400, n40* 100, n51x 100, n49* 100.
Die Nachrechnung von w1/w2/w3 ergibt dieselben Zahlen wie die Angaben des Bauers. Das gilt auch für die Probe „K5c-Arm + V1-Krallen“ (fffe6abb): Zone 0,866, obere Zone 0,82.

## Abweichungen, die der Nutzer ablehnen würde (ROT)
1. In Variante 1 (R9) sieht der Arm in Bild 2 nicht mehr verbunden aus. Die Armzone ist nur zu 0,565 gefüllt (Grenze 0,859), der obere Teil nur zu 0,08, und 1805 px zeigen Hintergrund. Das ist genau die Beschwerde des Nutzers, und ich habe sie mit eigenem Code bestätigt.
   - Ursache ist allein die neue Haltung von 17 und 18, also Unterarm und Hand. Das Handgelenk wandert dadurch rund 700 Einheiten nach hinten.
   - Der Tausch auf die Spiegelhand ist nicht die Ursache: Mit K5c-Haltung und Spiegelhand ist die Zone zu 0,904 gefüllt.
2. Kein Einbau, das Spiel hat weiter K5e_f1. Das ist richtig, weil die Frage des Nutzers vorgeht. Für diese Prüfung bedeutet es: nicht bestanden, nichts zum Abnehmen.

## Nur im Riegel/Vergleich sichtbar
- „Teil 16 3,3 Grad“ beim Bauer ist ein Rechenfehler. Die acos(Spur)-Formel ergibt auf Q12-Matrizen schon für R gegen sich selbst 3,2 bis 3,7 Grad. Richtig ist 16 = 0,0 Grad (bitgleich), 17 = 53,6 Grad (statt 53,7), 18 = 18,7 Grad (statt 19,2). Die Aussage bleibt gleich.
- „Ober- und Unterarm fast ganz hinter dem Kopf“: Für 17 stimmt das (23 von 3008 px sichtbar). Vom Oberarm 16 sind aber 2485 von 8767 px sichtbar, also 28 %.
- Die Bildkoordinate des Handgelenks hängt davon ab, wie man es misst: Der Bauer kommt auf 292,7/384,5 → 319,5/368, ich auf 277,7/371,3 → 307,0/363,5. In beiden Fällen wandert es nach rechts aus der Zone.
- Auch das heutige Spiel (K5e_f1) liegt mit 0,858 knapp unter der Grenze 0,859. Sichtbar wird das nicht, weil der Arm ab Zeile 370 durchgeht.
- Die Probe „K5c-Arm + V1-Krallen“ ist nur in Bild 2 gerechnet. Für Boden, N52 und Rampen in den übrigen Keys gibt es noch keine Rechnung. Kralle 19 liegt dabei nicht am gezeichneten Dorn.

## Offene Entscheidung (vom Bauer übernommen)
Soll der Arm wieder wie vorher senkrecht unter dem Kopf herunterkommen, mit der Vierer-Pfote aus Variante 1? Dann lägen die Krallen etwas anders als in der Zeichnung.
