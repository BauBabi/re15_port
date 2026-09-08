# RE1.5 Port — v0.7.19 (Early Preview)

**Eine flache Maske deckt nur noch da, wo niemand stehen kann.**

---

## Zwei deiner Marken widersprachen sich

Der Vordergrund-Teppich in ROOM10E0 (Cut 7) hat mich in beide Richtungen gefahren:

* **F1585** — mit der vollen Teppichmaske schnitt die Teppichkante eine **Leiche** ab, die
  auf dem Teppich lag.
* **F331 / F436** — ohne Teppichmaske **blitzte die Figur an 146 Punkten durch**.

Beides ist derselbe Mechanismus. Eine Maske verdeckt, wenn ihre Tiefe kleiner ist als die
des gezeichneten Dreiecks. Ein Teppich hat aber keine Vorderkante, hinter der alles gleich
weit weg wäre — und alles, was **auf** ihm liegt, hat praktisch **seine** Tiefe. Der
Vergleich wird zum Münzwurf und fällt zugunsten der Maske aus. Die Leiche verschwindet.

## Die Frage, die beides trennt

**„Kann dort überhaupt jemand stehen?"**

* Ein Bodenpunkt, der **begehbar** ist, darf nie maskiert werden — dort steht die Figur
  selbst, oder es liegt eine Leiche.
* Ein Bodenpunkt, der **in einer soliden Kollisionszelle** liegt (unter einem Möbel, in
  einer Wand), kann niemanden tragen — dort ist der Teppich echter Vordergrund und darf
  verdecken.

Beleg für „begehbar" sind die soliden Typ-1-Zellen des Bandes (Filter
`(type&0x0f)==1 && (u0&1) && (floor>>4)==band`, Kollisionsoffset RDT `0x20`) — an 3727
deiner eigenen Standorte zu 97,1 % bestätigt. Kein neuer Schätzwert.

## Gemessen

```
Teppichpunkte gesamt                        7700
davon vor einer soliden Zelle                518   -> maskiert (515 nach Kachelraster)
davon auf begehbarem Boden                  7182   -> bleiben frei
Cut 7 danach                     103 Rechtecke, 44,6 % Bildfläche, alle 10 Winkel intakt
```

Der volle Teppich als siebtes Objekt hätte den Kachelhaushalt gesprengt (nur noch **8**
statt 103 Rechtecke, 38583 Punkte der gewollten Fläche hätten gefehlt). Die Regel steckt
deshalb schon in der Freistellung; der Generator prüft sie erneut und meldet
„0 begehbar ausgelassen".

## ⛔ Was das NICHT löst — und warum das so bleiben muss

Von den 214 Durchbruchpunkten der Marken F331/F436 liegen **213 auf begehbarem Boden**
(einer in einer Zelle). Die neue Maske deckt **0** davon.

Das ist kein Versäumnis, sondern die Regel: dort steht der Spieler selbst. Diese Punkte zu
maskieren wäre exakt der Fehler, den du bei F1585 gemeldet hast. Wenn dort weiter etwas
durchblitzt, ist die Ursache **nicht** eine fehlende Maske — dann melde bitte eine Marke,
und ich messe an dieser Stelle den Zeichenpfad statt der Maske.

## Weiter offen

* Der Klappstuhl in ROOM10D0 Cut 7 braucht eine Freistellung von Hand — Sitzfläche und
  Gestell sind farblich nicht vom Boden zu trennen; automatisch freistellen hieße den
  Boden mitmaskieren.

Tests: **282/282** (im Release-Container).
