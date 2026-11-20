# Pruefung R9-Einbau (rechte Hand Variante 1) in K5g3 - Runde 2, Sicht

Datum: 2026-11-15. Pruefer: unabhaengig (Sicht). Nicht committet.

## Ergebnis: NICHT BESTANDEN - es gibt wieder keinen Einbau

- Zweig r41/form steht weiter auf 968001097. Seit Runde 1 gibt es keinen neuen Commit und keine geaenderte
  Datei unter re15_port/ oder K5g3/. Neu sind nur unversionierte Pruefberichte.
- Spiel-Asset im Worktree und im Hauptbaum: re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD sha f277916d
  (K5e_f1, Commit 62bd0001b). Der K5g3-Kandidat 1981f97e liegt nur unter varianten/K5/k5g/K5g3/ und ist
  ebenfalls nicht im Spiel.
- Keine neuen Tafeln (tafeln/ zuletzt 2026-11-14 20:06), kein neues kampf_5090.mp4.
- Der Bauer meldet selbst erreicht=false, commits=[].

Spiellaeufe per gdigrab: nicht gemacht. Ein Lauf haette nur K5e_f1 gezeigt. Darin steckt weder K5g3 noch
Variante 1, also sagt er nichts zu (a) bis (d).

## (a) bis (d)

- (a) Rechte Hand wie Variante 1 im Spiel und in Bild 2: nicht eingebaut. ROT.
- (b) Boden, Clip 9/10/12, Durchstossen: nichts zum Pruefen. ROT.
- (c) Rest wie K5g3 (Nacken B4, Textur, Augen, Schulter, Kopf): unveraendert, weil nichts gebaut wurde.
  K5g3-Dateien sind unveraendert (git status ohne Aenderung, sha 1981f97e wie in r3).
- (d) Tafeln und Film fuer diesen Einbau: es gibt keine. Nicht vorlegbar.

## Eigene Nachrechnung zur Frage des Nutzers ("vorher WAR er sauber connected")

Eigenes Skript (scratchpad/pruef_r9einbau_r2_sicht/eigen_r2.py, eigen_r2b.py), R9-EMD sha 2f807af5536e
gegen K5c sha 1ab56076:
- Rohwinkel ueber alle 771 Keys: verschieden nur bei Knochen 17 und 18 (je 78 Keys) sowie bei den
  Krallen 19, 20, 29 und 31 (je 43 Keys). Oberarm 16 ist in allen Keys bitgleich.
- Clip 12 Bild 0 (Key 542), Weltdrehung K5c->R9: Unterarm 17 53,7 Grad, Hand 18 19,2 Grad. Der Bauer
  nennt 53,6 und 18,7, das passt.
- Ellbogen (Ursprung 17) [1516,-1484,-350] in beiden Staenden. Handgelenk (Ursprung 18)
  [1666,-588,-992] -> [1400,-379,-333], genau wie vom Bauer angegeben.
- st10.py Zeile 145: `P('zone', (0.859 - z['az']) / 0.05, 1)`. Die Armzone hat das Gewicht 1.
- messen.log Zeile 12: Armzone 0,5648, Unterarm 17 nur 79 px sichtbar. Zeile 17: `K3e ROT`.
  Zeile 54: 13 rote Punkte.

Die Erklaerung des Bauers ist damit bestaetigt. Der Arm war vorher auch nur durch die Hand geschlossen,
die senkrecht unter dem Kiefer hing. R9 hat Unterarm und Hand nach hinten unter den Kiefer gedreht, und
die Armzone war in der Bewertung fast nichts wert. Das eigene R9-Protokoll hatte die leere Armzone schon
rot. Trotzdem wurde Variante 1 als Wahl vorgelegt.

Nebenbefund (eigen_r2b.py): In der Weltlage weichen in Key 542 auch die Knochen 21 bis 28 und 30 um 1 bis
6 Grad ab, obwohl ihre Rohwinkel bitgleich sind. Die Ursache habe ich nicht geklaert. Wahrscheinlich liegt
sie in der Posen-Berechnung von G.posen (zum Beispiel N52-Eintraege im EMD). Das ist nur fuer den
Vergleich wichtig und betrifft R9, das nicht im Spiel ist.

## Rote Punkte (wuerde der Nutzer ablehnen)
1. Variante 1 ist nicht im Spiel. Der Einbau, den ich pruefen sollte, existiert nicht.
2. Ein Einbau von Variante 1 so wie gezeigt brachte die leere obere Armzone ins Spiel (0,565, oben 0,08).
   Das ist genau die Stelle, die der Nutzer gerade beanstandet.

## Nur im Riegel/Vergleich
- Probe "K5c-Arm + V1-Krallen" fffe6abb (w3.out: Armzone 0,866, oben 0,82): ungebaut, nur Bild 2, nicht
  ueber alle Keys gerechnet. Sie ist ein Vorschlag, kein Pruefgegenstand.
- Abweichung der Knochen 21 bis 30 in der Weltlage (siehe Nebenbefund).
- Der K5g3-Kandidat 1981f97e ist nicht im Spiel (Spiel-Asset = K5e_f1). Das ist kein Fehler dieser Runde.

## Naechster Schritt
Die Nutzerentscheidung einholen, die der Bauer vorschlaegt: Arm senkrecht wie vorher, daran die
Vierer-Pfote von Variante 1. Danach bauen und neu pruefen (alle Keys, Boden, N52, Spielkameras 5080/5090,
gdigrab).
