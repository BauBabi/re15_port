# RE1.5 Port — v0.7.17 (Early Preview)

**Zwei deiner fünf Marken zeigen denselben grundsätzlichen Fehler — und er war in meinen
Messungen unsichtbar.**

---

## Marke 2: der Teppich schnitt die Leiche ab

Auf deinem Abzug läuft eine harte Kante quer durch die Leiche auf dem Boden; rechts davon
ist sie weg. Das ist keine Justage, sondern ein Denkfehler von mir:

> Eine Vordergrundmaske verdeckt, was **ferner** ist als sie. Eine flach liegende Fläche
> trägt die Entfernung des **Bodens** — und alles, was auf ihr liegt, hat **dieselbe**
> Entfernung. Der Vergleich wird zum Münzwurf und fällt systematisch zugunsten der Maske aus.

Ein Teppich kann per Konstruktion gar nichts verdecken: wer davor steht, ist näher; wer
dahinter steht, steht auf dem Boden *hinter* der Kante und überlappt sie im Bild nicht
einmal. Übrig bleibt nur der Schaden.

Aus der Freistellung bleiben die drei **stehenden** Dinge — Schrank rechts, Stuhllehne,
Kachelkante. Der flach liegende Rest fliegt raus.

```
Maskenpunkte über der Leiche       3483  ->  1371   (der Rest ist die Stuhllehne,
                                                     die dort wirklich davorsteht)
dein Median über 246 Standorte    12,3 %  ->  8,2 %
Schreibmaschine deckt weiter       3271  ->  2923
Atlasverbrauch                    64482  ->  50363 von 65536
```

## Marke 1: deine Füße standen in der Leiche

Gleicher Blickwinkel, gleiche Ursache, andere Richtung. Die Leiche am Boden trug ebenfalls
„flach", was ihr Entfernungen von 57 bis **1023** gab — die 1023 ist ein Sehstrahl fast
parallel zum Boden, also ein Ausreißer, kein Messwert. Damit verdeckte sie dich nicht, und
du standest mitten in ihr.

Eine liegende Leiche ist aber **kein Bodenbelag**: sie hat Volumen, und ihre unterste
Silhouettenkante ist je Bildspalte echter Bodenkontakt. Mit `spalten` (58…78):

```
an deiner Marke F401     241  ->  625 verdeckende Punkte, 0 zu fern
über 132 Standorte       Median 0,2 % -> 0,3 %, nichts verschluckt
```

## ⛔ Warum ich das nicht selbst gefunden habe

Mein ganzer Zensus misst **nur deinen Körperkasten**. Die Masken verdecken aber *jede*
Figur — Leichen, Gegner, abgelegte Gegenstände. Der Teppichfehler war in allen 215
Standort-Messungen dieses Blickwinkels unsichtbar und ist erst an deinem Abzug aufgefallen.
Das steht jetzt als Regel in meiner Arbeitsablage: bei jedem Maskenbefund auch fragen, was
sonst noch in diesem Bereich liegt.

Ich habe daraufhin **alle** Maskenobjekte durchgeprüft. Es bleibt genau eines auf „flach":
die Tischplatte in ROOM10D0 — und die zu Recht, sie liegt auf Hüfthöhe bei Entfernung
202…306 und deckt nur den Flur dahinter ab.

## Deine anderen drei Marken

**Marke 5** (grüne Tische, weiter hinten im Flur): du bist vollständig sichtbar, die Tische
verdecken nichts mehr. Der Fix aus v0.7.16 sitzt.
**Marke 3 und 4** habe ich angesehen, aber keinen klaren Fehler gefunden — Pflanze und
Klappstuhl stehen dort tatsächlich vor dir. Wenn dir dort doch etwas auffällt, sag mir bitte
kurz was, dann messe ich gezielt nach.

282/282 Tests.
