# RE1.5 Port — v0.7.3 (Early Preview)

**Aus deiner Zeichnung `fehler/howto.png` und deinem `befund.log` vom 07.09.:** der
Spielermarker bewegte sich im Treppenhaus nicht mit. Dahinter steckte ein Fehler, der
gleich mehrere deiner Meldungen erklärt.

---

## Der Marker stand still — und die Treppe stand in der Ecke

Du hast es im Treppenhaus auf 2F gesehen: *„Da habe ich auch gesehen das sich der
Marker nicht bewegt, wenn sich der Spieler bewegt."*

Das Original führt für 33 seiner 72 Karten-Räume eine eigene **Kartenzeile**
(`@0x800768b0`, ausgewertet in `FUN_800473f8 @0x8004741c-0x80047528`):

```
mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
```

Diese Zeile bildet **absolut** auf ein Blatt ab — nämlich auf das, wo das *Original*
den Raum zeichnet. Das Treppenhaus steht dort auf 1F (Seite 2).

Die Etagen-Umschaltung ist dagegen eine Ergänzung von mir: ich zeichne das
Treppenhaus zusätzlich auf 2F und 3F, dort an anderer Stelle. Dieselbe absolute Zeile
zeigt daneben — der Marker wird an die Rechteckkante geklemmt und steht.

An **deinen eigenen 522 Standorten** in ROOM1060 gemessen:

| Blatt | Zeile trifft das Rechteck | verschiedene Markerpixel |
|---|---|---|
| Seite 2 (1F, Originalblatt) | 275/522 | 45 |
| Seite 3 (**2F**) | **0/522** | **6** — alle in *einer* Spalte, x=122 |
| Seite 4 (3F) | 0/522 | 9 |

Betroffen waren 14 Zeilen: Treppenhaus, **Fahrstuhl** (auch auf 2F/3F), ROOM4020,
ROOM50D0. Über die begehbaren Punkte gemessen:

| | mit Zeile | ohne Zeile |
|---|---|---|
| ROOM1060 2F / 3F | 6 / 9 Pixel | **80 / 80** |
| ROOM1080 2F / 3F | 17 / 5 | 12 / 12 |
| ROOM4020 | 6 / 3 | 13 / 13 |
| ROOM50D0 | 53 | **244** |

Auf dem Originalblatt bleibt die Zeile unangetastet — dort ist sie die Vorgabe des
Originals und damit der Maßstab.

**Dieselbe Ursache traf die Marken.** Die Treppe im Treppenhaus stand auf 2F wörtlich
in der Ecke bei (118,157). Jetzt sind es **zwei** Treppenmarken bei (125,145) und
(133,147) — und du hattest zwei Leitersymbole bei etwa (121,142) und (131,142)
gezeichnet. 4–5 px, ohne dass deine Zeichnung in die Herleitung eingegangen wäre.

## Was ich NICHT belegen konnte

Deine zwei Räume rechts, *„separat und mit eigener Wand"*. Die senkrechte Wand bei
Karten-x 189 steht — sie folgt aus ROOM1110s Selbst-Türpaar. Für eine **waagerechte**
Trennung zwischen deinen beiden Kästen finde ich keinen Beleg:

* die **gemalte Kachel** des Originals (Rect 5, uv 184/48) zeigt dort eine
  durchgehende Fläche — Wandindex 4 nur außen herum;
* die **Kollisionszellen** haben in diesem Bereich keine Lücke, die eine Linie ergäbe;
* die **Türdaten** kennen nur die eine Wand, in der beide Türpaare sitzen.

Ich trage sie deshalb nicht ein. Was mir helfen würde: an welcher Stelle im Raum
stehst du, wenn du auf der einen bzw. der anderen Seite dieser Wand bist? Zwei
F9-Marken — je eine links und rechts davon — legen die Linie fest, so wie deine
Türmarken es beim Gangende getan haben.

---

**280/280 Tests**, lokal und im Linux-Container. Neu: `test_map_etagenzeile` prüft die
Regel *und* ihre Wirkung — gegen den alten Stand fällt er mit 6 Fehlschlägen durch.
