# RE1.5 Port — v0.7.24 (Early Preview)

**Dein Original deckt als Ganzes: das Pult ist ein Tresen.**

---

## Marke F591: „Leons komplette Beine schauen immer noch durch"

Meine vier Teil-Freistellungen deckten nur die *Gegenstände* (Maschine, Schrank,
Lehne, Kante) — die grüne Pultfläche darunter blieb frei, und deine Beine zeichneten
sich davor. Das war der letzte fehlende Schritt zu dem, was du von Anfang an gesagt
hast.

Jetzt ist es **ein** Objekt: dein unverändertes Original (`07_01.png`, 14303 Punkte,
der komplette Pultbereich) mit fester naher Tiefe. Wer im Pultbereich steht, steht
**hinter dem Tresen** — die Beine verschwinden, der Oberkörper schaut heraus. Dieselbe
Optik wie der Tresen in ROOM1120. An deiner Marke simuliert: links Beine über dem
Pult, rechts vollständig verschluckt.

## Ein Zerleger-Fehler, den deine Marke aufgedeckt hat

Die 105er-Schranke der Engine griff nur auf die *Schätzung* der Kachelwahl; Packung und
Zerlegung vermehrten die Kästen nachträglich (104 geschätzt, **117 gebaut**). Die Engine
liest hart 105 — der Rest fiel **still** weg, mit Löchern in den zuletzt gepackten
Objekten. Jetzt wird das Ergebnis nachgezählt und bei Überlauf neu gewählt (117 → 86).

## Bewusste Folge (deine Entscheidung)

Auch eine Leiche oder eine zweite Figur **im** Pultbereich wird unterhalb der Pultkante
verdeckt — die alte Leichen-Abwägung (F1585) fällt zugunsten des Immer-Deckens.

ROOM10D0 unverändert: Tisch-Quader deckt, der Klappstuhl wartet auf dein Lasso.

Tests: **282/282** (im Release-Container).
