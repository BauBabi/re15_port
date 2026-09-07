# RE1.5 Port — v0.7.10 (Early Preview)

**Die Holzbank in ROOM10C0 verdeckt jetzt am Bankende — und nur dort.** Zwei deiner
F9-Marken, zwei entgegengesetzte Anforderungen, beide erfüllt.

---

## Der Befund

*„Ich stehe am Ende der Bank, aber sie überdeckt mich nicht."* (Marke F1438)
*„Geht zu weit, wenn ich daneben stehe, darf es nicht decken."* (Marke F468)

| | verdeckende Punkte im Körperkasten | soll |
|---|---|---|
| **am Ende der Bank** | 0 → **520** | viel |
| **daneben** | 516 → **180** | wenig |

## Warum es zweimal falsch war

**1. Zu fern.** Die Silhouettenregel nimmt je Bildspalte den untersten sichtbaren Punkt
als Bodenkontakt. Am rechten Bankende ist das die **Seitenkante** der Bank, nicht ihr Fuß
— der Sehstrahl landet weit dahinter:

```
Spalte 64 -> Tiefe 150      Spalte 80 -> 174      Spalte 88 -> 187
```

Die Bank steht in den Kollisionsdaten dagegen bei **81..150**, also näher als du. Deshalb
`"tiefe": "kollision"`.

**2. Dann zu nah.** Der erste Wurf schoss die Sehstrahlen gegen **alle** Wandzellen des
Raums, und jede gilt darin als unendlich hohe Säule. In der Tiefenkarte sichtbar: bis
Bild-x 93 liegt sie bei 102..137 (die Bank), ab x=94 springt sie auf **56..62** — eine
ganz andere Zelle, die der Strahl weit über ihrem Boden trifft. Weil die Kachelung je
12×12-Feld die kleinste Tiefe nimmt, riss ein einziger Falschtreffer das ganze Feld nach
vorn.

**Jetzt** wird die Zelle gesucht, die das freigestellte Objekt **ist** — über die Deckung
der Freistellung (≥90 %); gibt es mehrere, gewinnt die **nächste**, denn ein
Vordergrundobjekt ist das, was man sieht:

```
x -4000..550   z -6250..-5250   deckt  99 %, Tiefe  81..150   <- gewaehlt (die Bank)
x -9600..8750  z -8250..-6250   deckt 100 %, Tiefe 233..236   (die Wand dahinter)
```

## Der Mechanismus

`geom.kollisionstiefe()` — ein Sehstrahl je Bildpunkt gegen die Kollisionszellen:

```
Kameraort = R^-1 * (-t)        Richtung = R^-1 * (sx-160, sy-120, H)
Schnitt mit den vier senkrechten Seitenflaechen, naechster Treffer gewinnt.
```

Selbstprüfung: der Treffpunkt Welt(−3950,−1813,5658) projiziert exakt auf Bild (85,140)
zurück; eine Spielerposition auf (122,6/201,9) gegen (123/201) aus dem Log, Tiefe 78,1
gegen 78.

Das ersetzt eine Ableitung, die von der gezeichneten Unterkante abhing, durch eine aus der
Raumgeometrie — nützlich überall dort, wo eine Freistellung an einer **Seitenkante** endet
statt am Boden.

---

**282/282 Tests**, lokal und im Linux-Container.
