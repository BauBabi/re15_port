# RE1.5 Port — v0.7.29 (Early Preview)

**Die Stuhllehnen decken bis zur echten Höhe — Schwarz zählt als Stuhl.**

---

## Communication Room, dritte Runde (deine vier Marken)

Zwei gemessene Ursachen, warum Leon noch teilweise im Vordergrund war:

1. **Die schwarzen Lehnen sind höher als gedacht:** an drei Schwarz-Säulen aufgelöst
   liegen sie bei h≈−1950 (−1970/−1927/−1970) — meine −1100-Boxen endeten darunter,
   die Lehnenspitzen blieben frei.
2. **Dein Lasso hat schwarz-auf-schwarz-Lücken** (gemessen: bg-Farbsummen 4..32 an
   Pixeln, die nicht im Lasso sind — dieselbe Klasse wie beim 10D0-Klappstuhl). Mit
   „nur Kunst deckt" blieb genau dort nichts übrig.

**Fix:** Höhe −1950, und die Deck-Region ist jetzt *deine Kunst ODER Tiefschwarz*
(Farbsumme < 45). In diesem Raum ist Schwarz eindeutig Stuhl — der Boden ist hell
(Summe 180+), die Boden-Überblendungen aus der zweiten Runde bleiben damit
ausgeschlossen.

## Gemessen, an fünf Marken

```
schwarze Vordergrund-Stuhlfläche:  1790 / 747 / 878 / 1049 / 1711 Punkte
davon ungedeckt:                      0 /   0 /   0 /    0 /    0
Sichtproben F1195/F1321: Stuhlmechanik, dunkle und weiße Lehne sauber VOR der Figur
```

Tests: **282/282** (im Release-Container).
