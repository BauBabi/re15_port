# RE1.5 Port — v0.7.21 (Early Preview)

**Diesmal war ich es dreifach: Profil, Median, und eine Freistellung ohne Löcher.**

---

Deine beiden Marken (F315, F690) zeigten dieselbe Fehlerklasse aus drei Quellen. Der
Kern: eine Masken-Kachel trägt **eine** Tiefe — und sobald du direkt am Gegenstand
stehst, überstreicht dein Körper selbst ein ganzes Tiefenband (an F315: Kamera-z
3486…4498). Jede Kacheltiefe, die in diesem Band landet, schneidet irgendein Dreieck
aus dir heraus.

## 1. Schreibmaschine (ROOM10E0): eine Tiefe statt Zeilenprofil

An F315 wurden dir **Papier und Walze über den Bauch geblittet**. Die Maschine hat
keine Kollisionszelle — du kannst bis in ihre Standfläche laufen. Damit liegt *jede*
Zeile ihres Tiefenprofils (64…71) mitten in deinem Körperband; irgendetwas verliert
immer. Die einzige Tiefe, die dich davor verschont **und** dich dahinter verdeckt, ist
ihr gemessener Bodenkontakt: Kamera-z 4577 → **Tiefe 71, einheitlich**.

Gemessen an deiner Marke: dein fernstes gezeichnetes Dreieck 4498 < 4544 → **0 von
2330** Maskenpunkten in deinem Kasten schneiden noch (vorher die komplette
Maschinen-Silhouette).

## 2. Tisch-Quader (ROOM10D0): Maximum statt Median, aufrunden statt kappen

Die Kacheln am Tischrand erbten per **Median** die näheren Deckflächen-Punkte (63…67
gegen deine Körperlinie 67,7…73,7) und `int()` kappte zusätzlich bis zu 63 Einheiten in
die schädliche Richtung. Jetzt gilt dieselbe Regel wie bei der Kollisionstiefe: **eine
Kachel verdeckt nur, wenn sie ganz vor dir liegt** (Maximum je Feld, aufgerundet).

## 3. Stuhl (ROOM10D0): die Rahmen-Lücken sind jetzt offen

Das war dein „**Leon dahinter transparent**": die Stuhl-Freistellung füllte die offenen
Lücken zwischen den Rohren. Stehst du dahinter, bist du durch die Lücken sichtbar — und
genau dort wurde der **Boden** über dich geblittet. 245 von 2898 Punkten entfernt; die
Lücken zeigen jetzt dich statt Fliesen.

## Gemessen, an deinen drei letzten Marken

```
F315   0 schneidende Maskenpunkte im Spielerkasten          (vorher: ganze Maschine)
F690   re-geblittete Patches 57 -> 30, und die 30 liegen AUF dem Stuhl
       (Lehnenoberkante, hinteres Rohr - der ist bei dieser Kamera wirklich
        vor dir; das ist korrekte Verdeckung, keine Lücke)
F3218  hinter dem Tisch bleibst du verdeckt: 408 von 411 Beinpunkten
       (3 Randpunkte kostet die Aufrundung - die harmlose Richtung)
```

Tests: **282/282** (im Release-Container), `selbsttest.geometrische_tiefen` 10/10.
