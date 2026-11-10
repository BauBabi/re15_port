# K5g2 Runde 2 - Mulde (O1): Arbeitsnotiz des Mulden-Bauers

Stand: in Arbeit (Zwischenstand, wird fortgeschrieben).

## Wo liegt was

| Was | Pfad |
|---|---|
| Skript (Tabelle, EMD, Pruefung) | `K5/generator/kf_k5g2_mulde.py` |
| Messwerkzeug (Streifen, alle Paare, Dreieckszahl) | `K5/generator/kf_k5g2_mulde_mass.py` |
| Stufe 1: Netze 10, 21, 22 aus der Orchestrator-Rechnung m2_a | `K5/generator/k5g2_mulde_stufe1.json` |
| Tabelle: fertige Netze 10, 16, 21, 22 | `K5/generator/k5g2_mulde_tabelle.json` |
| Modell (K5e_f1 + nur diese Netze) | `K5/k5g/K5g2/geo/MULDE.EMD` |

Nachbau: `python kf_k5g2_mulde.py tabelle`, dann `emd`, dann `pruef` (zweiter Lauf byte-gleich).
Die Stufe 1 wird nicht neu gerechnet (77 Minuten, Orchestrator-Lauf mulde2.py Lauf a, Rand 16, drei Runden);
sie liegt als Datei bei. Die Krallen-Netze 19, 20, 28, 29, 31 fasst das Skript nie an (Basis ist K5e_f1).

## Befund 1: Die Schlieren auf dem Lappen sind Teil 16, nicht verzerrte Textur von Teil 10

Pflichtbild spiel5080_vorn Clip 11 Bild 5, gemaltes Teil je Bildpunkt (Werkzeug: Raster aus kf_k5g2_mulde_mass.raster):

- ohne Mulde (K5e_f1 + nur Kopf 22): Teil 10 malt 10663 Bildpunkte, Teil 16 malt 8896.
- mit Mulde: Teil 10 malt 1881, Teil 16 malt 17202. 8306 Bildpunkte wechseln von 10 auf 16.

Der Lappen ist in K5e_f1 eine Doppellage: Buckel 10 liegt knapp ueber der Schulterkugel 16 (in der
Vergleichsmessung ueber alle Bilder sind 16>10 mit 1026986 und 10>16 mit 703758 Bildpunkten die groessten
Streifenpaare des ganzen Modells). Die Mulde nimmt die Ecken von 10 unter die Flaeche von 16 zurueck; sichtbar
ist dort jetzt 16 mit seiner eigenen, unveraenderten Textur (Original-Texel, Original-UV). Der Umriss bleibt,
ein Loch ist nicht zu sehen, aber das Muster ist ein anderes als in K5e_f1.
Die 7729 geaenderten Bildpunkte des Bildvergleichs kommen deshalb fast ganz aus diesem Wechsel und nicht aus
gedehnter Textur; eine neue Texturlage auf 10 aendert sie nicht (7728).

## Befund 2: Teil 16 laesst sich nicht ohne sichtbaren Schaden ausnehmen

- Der Kopf dringt nur in den Clips 10 und 12 in Teil 16 ein (79 Bilder), dort aber bis 237 Einheiten tief;
  94 von 134 Ecken liegen im Schwenk (Rand 16).
- Teil 16 ist nicht nur die Kugel, sondern Kugel und Oberarm (Ausdehnung 944 x 2030 x 1126). Von 2720
  Gitterpunkten im Inneren (Raster 60) liegen nur 328 ausserhalb des Kopf-Schwenks: der Kopf ueberstreicht
  88 Prozent des Rauminhalts. Eine Mulde wie in 10/21 wuerde den Oberarm aufloesen.
- Nur 2 Ecken von 16 sind in allen Bildern von K5e_f1-Teil 10 verdeckt; Teil 16 dreht mit dem Arm aus dem
  Buckel heraus (715 verschiedene Lagen von 16 gegen 10 in 799 Bildern).
- Seit der Mulde in 10 ist 16 die sichtbare Lappenflaeche (Befund 1); jede Delle in 16 waere sichtbar.
- Nur teilen (ohne Zuruecknehmen) hilft kaum: an den 40 Ansichten mit den meisten Streifen 22/23 gegen 16
  faellt 22>16 von 3333 auf 3046 und 16>22 von 4993 auf 4377 (eine Stufe, +734 Dreiecke); zwei Stufen kosten
  +2438 Dreiecke und bringen nichts dazu.

## Textur auf der Mulde (O1 vii)

Mass: Jacobi-Matrix Textur -> Raum je Port-Dreieck, Singulaerwerte = Einheiten je Texel (laengste, kuerzeste),
Verzug = laengste / kuerzeste, und dasselbe relativ zur Mutterflaeche in K5e_f1 (`dehnung_bericht`).

Reparatur (`textur_neu`): je geteilter Mutterflaeche werden die Kinder im Texturvieleck der Mutter neu verteilt -
Randecken nach der Bogenlaenge der verformten Kante auf der Texturkante der Mutter, Innenecken nach
Mittelwert-Koordinaten, gerundet auf ganze Texel. Kein Texel ausserhalb der Mutterflaeche, CLUT-Zeile und
Texturseite bleiben, Normalen bleiben die der alten Flaeche. Muttern mit nicht konvexem Texturviereck bleiben
wie geerbt.

Die Liste aller Flaechen mit neuer Texturlage steht in der Tabelle unter `uv_neu` (je Teil Paare
[Art, Nummer]; Art 0 = Dreieck, 1 = Viereck).
