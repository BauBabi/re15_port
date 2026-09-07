# RE1.5 Port — v0.7.7 (Early Preview)

**Deine drei Befunde aus `error1..3` — sie hatten eine gemeinsame Ursache**, plus ein
zweiter Fehler, den ich selbst verursacht hatte.

---

## 1. „this half should be green" — der andere Teil war gar nicht gemalt

Aus deinen Abzügen gerastert:

```
error2 (du stehst oben):   y122..133 rot,  y134..144 LEER
error3 (du stehst unten):  y134..145 rot,  y123..133 LEER
```

Nur die Kachel-Umrandung stand da, keine Fläche.

**Ursache:** Die nicht-aktiven Teile gingen in den *zweiten* Zeichendurchgang — und dort
überdeckt sie **Rect 3 (ROOM10D0, x135..206, y76..163)**. Es belegt denselben
Bildbereich und hat den **kleineren Index**, wird also früher eingetragen; und früher
heißt *oben*, weil die Op-Liste von hinten gerastert wird.

Der Zwei-Durchgang-Mechanismus löst genau dieses Problem für ganze Rechtecke — für ein
Rechteck mit **gemischten** Teilen reicht er nicht. Dessen Teile gehören alle zum
aktuellen Raum und gehen jetzt zusammen in den ersten Durchgang. Untereinander können sie
sich nicht verdecken; dass ihre Ausschnitte disjunkt sind, prüft `unit_map_teilbereich`
bereits.

## 2. Die Trennwand leuchtete rot mit

Im Evidence Room stehend war die Wand zwischen den beiden kleinen Räumen (Karte y=133)
rot, obwohl **beide** angrenzenden Bereiche grün sind — sie nahm den Zustand des ganzen
Rechtecks. Hat ein Rechteck Teile, gilt jetzt der stärkste Zustand der Teile, die die Wand
**berührt**.

---

## In eigener Sache

Beim Patchen habe ich `re15_inv_screen.c` auf **0 Bytes** gekürzt: ein Python-Skript
öffnete die Datei zum Schreiben (was sofort leert) und scheiterte danach an einem
Sonderzeichen. **Denselben Fehler hatte ich in dieser Sitzung schon einmal** an
`re15_savedata.c`. Aus git wiederhergestellt; die ausgelieferte v0.7.6 war nicht betroffen
(16:18 gebaut, die Datei ging um 16:39 kaputt). Meine Patch-Skripte lesen und schreiben ab
jetzt binär und prüfen vorher die Dateigröße.

Außerdem war die neue `.exe` byte-genau so groß wie die vorige — ein Warnsignal, dem ich
nachgegangen bin: ein zweiter Build meldete `ninja: no work to do`, der Fix ist also drin
und die Größengleichheit Zufall.

---

**282/282 Tests**, lokal und im Linux-Container. Alle drei Teilbereiche zusätzlich direkt
im ausgelieferten Binary nachgewiesen.
