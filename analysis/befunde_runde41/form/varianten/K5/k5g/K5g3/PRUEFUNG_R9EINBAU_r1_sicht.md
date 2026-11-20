# Pruefung R9-Einbau (rechte Hand Variante 1) in K5g3 - Runde 1, Sicht

Datum: 2026-11-15. Pruefer: unabhaengig (Sicht). Nicht committet.

## Ergebnis: NICHT BESTANDEN - es gibt keinen Einbau, den man pruefen kann

- Zweig r41/form steht weiter auf 968001097 (K5g3 Textur Runde 3). Es gibt keinen neuen Commit und keine
  geaenderte Datei im Port (git status zeigt nur unversionierte Pruefberichte und Generator-Skripte).
- Der Bauer meldet selbst `erreicht=false`, `commits=[]`: Variante 1 wurde absichtlich NICHT eingebaut.
- Damit gibt es keinen Spielstand, keine Tafeln und kein kampf_5090.mp4 mit Variante 1. Spiellaeufe per
  gdigrab haette ich nur mit dem alten Stand (K5e_f1-Hand) machen koennen, und die sagen ueber Variante 1
  nichts aus. Deshalb habe ich keine gemacht.

## Zu (a)-(d)

- (a) Aussehen wie Variante 1 im Spiel: nicht pruefbar, weil nicht eingebaut. ROT.
- (b) Boden und Bewegung Clip 9/10/12: nicht pruefbar. ROT.
- (c) Rest wie K5g3: unveraendert, weil nichts gebaut wurde (HEAD 968001097 = Kandidat-Linie von K5g3 r3).
- (d) Tafeln oder Film fuer den Nutzer: es gibt keine fuer diesen Einbau. Nicht vorlegbar.

## Geprueft: die Erklaerung des Bauers zur Frage des Nutzers ("vorher WAR er ja sauber connected")

Mit eigenem Blick in die R9-Dateien geprueft:
- `scratchpad/k5g2/kr4/r9/st10.py` Zeile 145: `P('zone', (0.859 - z['az']) / 0.05, 1)`. Die Regel
  "Armzone gefuellt" hat Gewicht 1. Die Krallen-Regeln (n49f1, n47) haben 100, n50 60. Der Rechner durfte
  die Armzone also billig gegen die Krallenlage tauschen. Das stimmt.
- `r9/messen.log` Zeile 12: in Bild 2 Armzone 0,5648, Unterarm 17 nur 79 Pixel sichtbar.
  Zeile 17: `K3e ROT Armzone 0.5648 < 0,859`. Zeile 54: insgesamt 13 rote Punkte (K3d, K3e, K9, E6, K11,
  N23, K14, N27, N48, N50, N21, N28, N53). R9 hat also schon in der eigenen Messung die Armzone rot gefuehrt.
  Trotzdem wurde das Blatt mit Variante 1 vorgelegt.

Das ist die Antwort auf die Frage: Der Arm ist nicht "abgerissen". Unterarm und Hand wurden in R9 so stark
gedreht (laut Bauer 54 bzw. 19 Grad), dass die Hand unter den Kiefer wandert und oben in der Armzone der
Hintergrund durchscheint. Vorher hing die Hand senkrecht unter dem Kopf und verdeckte die Luecke. Die
Messzahlen fuer die Drehwinkel und Pixel (w1-w3.py) habe ich nicht selbst nachgerechnet. Belegt sind nur
die Gewichtung in st10.py und das rote K3e im R9-Protokoll.

## Rote Punkte
1. Variante 1 ist nicht im Spiel. Der Auftrag "Einbau pruefen" kann nicht erfuellt werden.
2. Das Nutzerblatt Variante 1 zeigte eine Haltung mit rotem K3e (Armzone 0,565). Ein Einbau 1:1 wuerde genau
   die Stelle bringen, die der Nutzer gerade beanstandet.

## Nur im Riegel/Vergleich
- Die Probe "K5c-Arm + V1-Krallen" (w3.py, Armzone 0,866) ist ungebaut und nicht durch alle Keys gerechnet.
  Sie ist ein Vorschlag fuer die offene Entscheidung, kein Pruefgegenstand.

## Naechster Schritt
Die Nutzerentscheidung einholen, die der Bauer vorgeschlagen hat: Arm senkrecht wie vorher und die
Vierer-Pfote von Variante 1 daran, wobei die Krallen dann etwas anders liegen als gezeichnet. Danach neu
bauen und neu pruefen (Spielkameras 5080/5090, Bild 2, Clip 9/10/12, Boden).
