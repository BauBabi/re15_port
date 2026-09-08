# RE1.5 Port — v0.7.20 (Early Preview)

**Die Schreibmaschine schnitt dich weg — und die Tischplatte hatte gar keine Maske.**

---

## Marke 1: „Leon blitzt durch die Schreibmaschine" (ROOM10E0, F376)

Es war das Gegenteil. Die Schreibmaschine verdeckte **zu viel**.

Du stehst bei Welt (1232, 0, −982). Deine Körperlinie liegt in den Bildzeilen der
Maschine bei Tiefe **61,2…68,2**; die Maschine steht geometrisch bei **64,4…71,5** — du
bist also **davor**. Ihre Maske trug aber **58,0…64,4** und schnitt dich über **1840
Bildpunkte** ihrer Silhouette weg. Nach dem Fix: **0**.

**Ursache: der Eichfaktor 0,90.** Der korrigiert die *Silhouetten*-Schätzung (Bodenkontakt
je Bildspalte) — deshalb verzichtet die Kollisionstiefe schon immer darauf. Er lief aber
auch über `aufrecht`, und das rechnet geometrisch: gemessener Standpunkt, exakte
Trigonometrie mit eingebauter Gegenprobe. Dort ist er schlicht falsch.

Dass 1,00 richtig ist, ist zweifach gemessen:

```
Quadertiefe gegen 260858 Punkte aus Capcoms EIGENEN Maskenrechtecken (31 Winkel)
   Faktor   Medianfehler   zu nah    zu fern
    0.90       -8.6        52.7 %     9.9 %
    1.00       -1.0        21.7 %    21.6 %     <- ausgeglichen
```

## Marke 2: „Leons Bein blitzt durch die Tischplatte" (ROOM10D0, F3218)

Die Holztischplatte hatte **überhaupt keine Maske**. Sie ist farblich nicht vom dunklen
Raum zu trennen (43, 30, 11) — jede Farbfreistellung hätte den Boden mitgenommen.

**Neues Modell: die Kollisionszelle als QUADER.** Der bisherige Sehstrahl kennt nur
unendlich hohe *Säulen ohne Deckel*: bei einem Hindernis, über das die Kamera hinwegsieht,
fällt er durch und trifft erst die gegenüberliegende Innenseite — viel zu fern, und eine
Silhouette gibt es gar nicht. Mit Deckfläche liefert die Zelle **beides** ohne eine einzige
Farbentscheidung.

```
ROOM10D0 Cut 7, Zelle x-1350..-50 z26300..27850, Höhe -1100
   Höhe gemessen an der Deckfläche: IoU 0,561, 786 der 836 braunen Plattenpunkte
   Ergebnis: 411 von 411 Figurpunkten unterhalb der Tischkante verdeckt (vorher 0)
```

Dazu in ROOM10E0 die **Trennwand rechts** (Zelle x1700..4100 z−4700..−900, Höhe −1400),
zweifach eingemessen: Kantenlage 53,99 gegen 15,64 beim zweitbesten Wert, und die
Deckfläche trifft bei −1400 alle 836 Punkte des hellen Bands.

## ⛔ Warum ich es zweimal nicht gefunden habe

Meine Messschiene zählte nur Bildpunkte, an denen die Figur **gezeichnet** wurde. Wo eine
Maske sie zu Unrecht wegschneidet, ist das Bild gleich dem Hintergrund — der Fehler fällt
aus der Zählung heraus. Deshalb meldete ich zweimal „der Renderer ist sauber", während du
den Fehler die ganze Zeit gesehen hast.

`selbsttest.geometrische_tiefen()` prüft jetzt für jedes geometrische Modell, dass die
Tiefe am Bodenkontakt gleich `vz_at_floor` ist. 11 Objekte — mit dem alten Faktor wären
**alle** durchgefallen.

## Noch offen

Oberhalb der Tischkante in ROOM10D0 bleibt dein Oberschenkel sichtbar — das ist richtig,
der Tisch ist 1,1 m hoch und du stehst dahinter. Sollte dort trotzdem etwas verdecken
müssen, ist es die **zweite, hellere Klappstuhl-Lehne** dahinter; die braucht eine
Freistellung von Hand und ist noch nicht drin.

Tests: **282/282** (im Release-Container).
