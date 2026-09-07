# RE1.5 Port — v0.7.12 (Early Preview)

**Die Liege in ROOM10E0 hat jetzt eine echte Entfernung — sie steht hinter dir, nicht vor dir.**

Dein Befund: *"Na, das komplette Bett überdeckt Leon da unten. Aber leon muss davor sein."*

```
Entfernung der Liege        fest 162   ->   gemessen 146..170
deine Entfernung an der Marke                          108
```

Gegen **deine eigenen 176 Standorte** in diesem Blickwinkel, aus `befund.log`:

```
Figur fast ganz verschluckt      17  ->  1
im Mittel verdeckter Körper    33,5 %  ->  23,6 %
```

Der eine Rest ist die **Schreibmaschine im Vordergrund** (Entfernung 97..102 gegen
deine 106) — dort stehst du dahinter, das darf verdecken.

---

## Warum eine feste Zahl hier nicht reichen konnte

Die Liege trug `"fuss": 162` — **eine** Entfernung für das ganze Objekt. Damit verdeckt
sie überall gleich, egal wo du stehst. Ein Möbelstück hat aber keine Entfernung, es hat
eine **Lage**; ob es dich verdeckt, entscheidet erst dein Standort.

Die Lage steht in den Kollisionsdaten des Raums. Ein Sehstrahl je Bildpunkt trifft die
Zelle und liefert die Entfernung exakt — dieselbe Rechnung, die letzte Woche die Holzbank
gelöst hat.

## ⛔ Dabei ist die Zellauswahl aufgefallen — und sie war zu naiv

Bisher galt: *"nimm die Zelle, die mindestens 90 % der Freistellung deckt, und davon
die nächste."* Für die Bank ging das gut. Für die Liege fiel es herein:

```
x -500..6350  z-4050..-1450   deckt 100 %   Entfernung  15..21   <- gewählt, falsch
x-5000.. -600 z 2300.. 3100   deckt  47 %   Entfernung 121..155  <- richtig
```

Eine Zelle ist im Sehstrahl eine **unendlich hohe Säule**. Eine große, nahe Säule deckt
darum leicht das halbe Bild — und gewinnt jede Deckungsprüfung, obwohl sie mit dem
Möbelstück nichts zu tun hat. Bei Entfernung 15..21 hätte sie dich überall verschluckt.

Neu entscheidet **Jaccard**: Schnittfläche geteilt durch Vereinigungsfläche. Das bestraft
genau den Überschuss, den die naive Deckung belohnt:

```
ROOM10E0 Liege   J=0,165 (richtig)   gegen   J=0,080 (die nahe Säule)
ROOM10C0 Bank    J=0,119 (richtig)   gegen   J=0,102
```

Beide Räume wählen damit die richtige Zelle — die Bank bleibt unverändert bei
520/0/0 über deine drei Marken.

---

## Kleinigkeit am Rande

Das Maskenwerkzeug warnte *"weder fuss noch ebene angegeben"* auch für Objekte, die ihre
Entfernung längst aus der Kollision beziehen — und brach beim Ausgeben des Warnzeichens
unter cp1252 ab. Die Bedingung kennt jetzt auch diesen Fall.

282/282 Tests.
