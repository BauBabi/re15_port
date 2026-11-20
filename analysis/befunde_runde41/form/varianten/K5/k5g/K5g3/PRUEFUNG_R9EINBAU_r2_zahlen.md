# Prüfung R9-Einbau (Variante 1) in K5g3, Runde 2 – Zahlen

Unabhängiger Zahlen-Prüfer, 2026-11-15. Nicht committet.
Eigener Code: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/kr4/pruef_r9einbau_r2_zahlen/eigen2.py`. Das ist ein eigener EMD-, EMR- und EDD-Leser mit eigener Gleitkomma-Pose (Rx·Ry·Rz). Er nutzt keine Projekt-Bibliothek. Die Ausgabe steht in `eigen2.out` im selben Ordner.

## Ergebnis in einem Satz
Variante 1 ist nicht eingebaut. Deshalb lassen sich die Prüfpunkte 1 bis 8 nicht prüfen, und die Prüfung ist nicht bestanden. Die Antwort des Bauers auf die Frage „Wie kann es dazu kommen?“ habe ich mit eigenem Code nachgerechnet, und sie stimmt.

## Stand (gemessen)
- Zweig r41/form, HEAD 968001097. Es gibt keinen neuen Commit und unter re15_port/ keine Änderung.
- Das Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` hat sha f277916d (K5e_f1). Im Hauptbaum ist es dasselbe, zuletzt geändert in Commit 62bd0001b.
- Die Prüfpunkte (1) Boden, (2) N52, (3) Posen, (4) Netz 18, (5) Riegel und Generator, (6) Asset und ctest, (7) Dreiecke und 5090 sowie (8) Rampen entfallen: Es gibt keinen Kandidaten. ctest und 5090 habe ich nicht gestartet, weil es nichts Neues zu bauen gibt.

## Warum der Arm vorher verbunden war (eigener Leser)
- K5e_f1 (heute im Spiel) und K5c haben eine **bitgleiche** Sektion emr1, also dieselbe Haltung. Verschieden ist nur md1, also die Netze. „Vorher sauber verbunden“ bedeutet damit: die K5c-Haltung von Unterarm und Hand.
- Im Spiel (K5e_f1) sind die Knochen 16, 17 und 18 in Key 542 gegenüber K5c um 0,0 Grad gedreht.

## R9 (Variante 1) gegen K5c (eigener Leser)
- Pool 771 Keys, 32 Knochen, Eltern und Bindversätze gleich, Wurzel in keinem Key verschieden.
- Geändert sind die Knochen 17 und 18 in 78 Keys (309..556) und die Krallen 19, 20, 29 und 31 in 43 Keys (398..556).
- Der Oberarm 16 ist in **allen** Keys gleich.
- Bild 2 ist Clip 12, Bild 0, also Key 542 (EDD gleich).

| Knochen | Weltdrehung R9 gegen K5c | Gelenk K5c → R9 |
|---|---|---|
| 16 Oberarm | 0,0 Grad | [1417,-1915,-1009] unverändert |
| 17 Unterarm | 53,6 Grad | Ellbogen [1520,-1481,-349] unverändert |
| 18 Hand | 18,7 Grad | Handgelenk [1671,-584,-990] → [1405,-374,-331] |

Das Handgelenk wandert damit 741 Einheiten nach hinten unter den Kiefer. Die Zahlen des Bauers (53,6 / 18,7, rund 700, [1666,-588,-992] → [1400,-379,-333]) stimmen. Der Unterschied von höchstens 5 Einheiten kommt von Q12 gegen Gleitkomma.

## Ursache in der Bewertung (gelesen)
- In `r9/st10.py`, Zeile 145, steht `P('zone', (0.859 - z['az']) / 0.05, 1)`. Die Armzone hat also das Gewicht 1, und das bestätige ich.
- Im Protokoll `r9/messen.log` steht `K3e ROT Armzone 0.5648 < 0,859`. Dazu kommen die roten Punkte K3d, K3f, K3g, K3h, K9 und E6. R9 war also schon im eigenen Protokoll rot.
- In der LIESMICH zum Entscheidungsblatt, Zeile 8, steht nur in Worten: „oben in Ihrer roten Armzone ist KEIN Arm“.

## Armzone (Bild 2)
Die Füllgrade habe ich in dieser Runde nicht neu gerastert. Übernommen sind sie aus dem eigenen Tiefenpuffer des Prüfers r1 (`pruef_r9einbau_zahlen/eigen.py`), der ein anderes Verfahren nutzt als der Maler des Bauers. Die Werte stimmen mit dem Bauer überein:
- K5c: 0,896
- K5e_f1: 0,858
- R9: 0,565, oben 0,08, 1805 px leer, Arm ab Zeile 384

Die Probe fffe6abb ist in `w3.out` belegt: 0,866, oben 0,82. Mit K5c-Haltung und Spiegelhand: 0,904.

## Abweichungen, die der Nutzer ablehnen würde (ROT)
1. In Variante 1 sieht der Arm in Bild 2 abgerissen aus. Die Armzone ist zu 0,565 gefüllt (Grenze 0,859), oben nur zu 0,08. Die Ursache ist allein die neue Haltung von 17 und 18 (53,6 / 18,7 Grad, Handgelenk 741 Einheiten nach hinten). Ein Einbau so wie gewählt würde genau diesen Fehler ins Spiel bringen.
2. Es ist nichts eingebaut, im Spiel bleibt K5e_f1. Das ist richtig, weil die Frage des Nutzers vorgeht. Für diese Prüfung heißt es aber: nicht bestanden, nichts zum Abnehmen.

## Nur im Riegel/Vergleich sichtbar
- „Ober- und Unterarm fast ganz hinter dem Kopf“: Für 17 stimmt das (23 px). Vom Oberarm 16 sind aber rund 28 % sichtbar (2485 von 8767 px, r1).
- Auch das heutige Spiel liegt mit 0,858 knapp unter der Grenze 0,859. Sichtbar wird das nicht, weil der Arm ab Zeile 370 durchgeht.
- Die Probe fffe6abb ist nur für Key 542 gerechnet. Boden, N52 und Rampen in den übrigen Keys fehlen noch. Laut w3.out liegen die tiefsten Punkte in Key 542 bei 20: -24 und 29: -19, also knapp über dem Boden. Kralle 19 liegt nicht am gezeichneten Dorn.
- Die Gelenkorte weichen zwischen Q12 und Gleitkomma um höchstens 5 Einheiten ab.

## Offene Entscheidung (vom Bauer)
Soll der Arm wieder in der K5c-Haltung senkrecht unter den Kopf (wie in K5e_f1 heute), mit der Vierer-Pfote aus Variante 1? Dann liegen die Krallen etwas anders als in der Zeichnung.
