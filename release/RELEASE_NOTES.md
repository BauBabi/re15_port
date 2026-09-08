# RE1.5 Port — v0.7.18 (Early Preview)

**Die Krähen sehen jetzt durch das, was sie blockiert, nicht mehr hindurch.**

---

## Der Krähen-Befund

Du: *„sie folgen Leon stumpf linear, wenn ein Tresen im Weg ist, fliegen sie stumpf linear
Richtung Leon, auch wenn der Tresen zwischen beiden die Krähe blockt. Die Krähe müsste
erkennen, dass sie lange geblockt wurde, und dann etwas anderes probieren."*

**Das Original hat genau diesen Ausweg — er hängt an der Sichtlinie.** Die zwei
Angriffs-Zustände haben nur zwei geometrische Ausgänge: den Blickkegel auf dich und ein
Sicht-Bit. Der Wand-Prober, der ein Hindernis erkennt und umlenkt, läuft **nur** in den
Flug-Zuständen davor. Im Original macht das nichts, weil der Sichtstrahl dieselbe Kulisse
abtastet, die auch die Bewegung stoppt: was den Flug blockt, bricht auch die Sicht → das
Bit fällt → die Krähe fällt in den Flugzustand zurück → dort weicht sie aus.

**Der Port hatte diese Kopplung nicht.** Der Sichtstrahl lief über die *Boden-Regionen*,
die Klemme über die *Kollisionszellen* — zwei verschiedene Mengen. Meldet der Strahl „frei",
wo die Zelle den Flug festhält, fliegt die Krähe stur weiter gegen den Tresen.

**Dass es diese Lücke wirklich gibt, ist jetzt gemessen** (Gitter aus Standort-Paaren je
Raum, nur Paare in Krähen-Reichweite). Anteil der Paare, die von den Zellen blockiert
werden, vom alten Strahl aber nicht:

```
ROOM1140  33,6 %      ROOM10C0  24,1 %      ROOM1130  23,6 %      ROOM1120  18,8 %
ROOM1110  16,6 %      ROOM10E0  14,0 %      ROOM10D0   4,2 %      ROOM1170   1,5 %
```

Umgekehrt (nur der alte Strahl blockt) praktisch nie — 0 bis 21 von Zehntausenden.

An einem solchen Paar über 3600 Ticks:

```
                        vorher                     jetzt
Zustände               enthält 13 und 14          keine Angriffs-Zustände
Anflug-Ticks                   75                          0
Sicht-Bit          1 („frei" durch den Tresen)              0
```

Die Krähe legt sich also nicht mehr auf eine Angriffslinie **durch** ein Hindernis fest.

⚠️ **Ehrlich dazu:** der von dir beschriebene *Stillstand* ist damit nicht reproduziert — in
beiden Sondenläufen bewegt sich die Krähe. Belegt ist der Mechanismus-Wechsel, nicht das
Verschwinden des Symptoms. Dein nächster Durchlauf entscheidet; die F9-Marke schreibt jetzt
ja den Krähen-Zustand mit.

Und ein Fehler von mir, den du wissen sollst: mein **erster** Nachweis bewies gar nichts.
Ich hatte ein Standort-Paar gewählt, an dem *beide* Strahlen blockieren — beide Stände
lieferten identische Zahlen. Die Sonde sucht sich das Paar jetzt selbst aus der
Divergenzmenge.

---

## Masken

**Zombies durch die Wand** (dein Marker in ROOM10E0). Mein Fehler aus v0.7.13: die
automatische Zellwahl nahm für die Rückwand die *hinterste* Zelle (Entfernung 176…263) —
damit ist die Wand ferner als die Zombies dahinter und verdeckt sie nicht. Die Auswahl misst
nur die Bildübereinstimmung; *welche* Zelle die Wand ist, entscheiden erst die Standorte der
Figuren davor und dahinter:

```
                      dein Kopf      Zombie dahinter      Türöffnung
                      soll >103,3    soll <166            soll frei
alte Zelle            245..256       228..248  zu fern    221..225
neue Zelle            126..131       117..131  richtig    kein Treffer = frei
```

Der Zombie wird jetzt auf 803 von 1071 Punkten verdeckt, die Türöffnung bleibt auf 982 von
1173 frei, und dein Kopf bleibt an allen drei Marken in **jeder Zeile** unangetastet.

**Die Pflanze** (dein „kleines Stück durchblitzen"). Der Blickwinkel hatte nur eine
797-Punkte-Freistellung — der Strauch links war gar nicht erfasst. Die größte Blattgruppe war
zu 22 % gedeckt; in deinem Körperkasten hatten 126 von 194 Blattpunkten keine Maske. Die
dünnen Blätter sind farblich kaum vom Putz zu trennen, deshalb die **Hülle** statt der
Blattkontur — das ist hier zulässig, weil die Maske Hintergrundpunkte über dich malt, ein
gefüllter Zwischenraum also genau das zeigt, was man durch die Pflanze sähe. 126 → **0**.

⛔ **Nicht gelöst: der Klappstuhl.** Gemessen: seine Freistellung ist nur ein schmales Band
entlang *einer* Strebe — Sitzfläche und übriges Gestell haben keine Maske, allein in deinem
Körperkasten 2278 Punkte Stuhlmaterial. Der Tisch daneben endet wirklich bei Bildspalte 49,
seine Freistellung stimmt. Eine Nachbildung per Farbe scheitert: Sitzfläche und **Fußboden**
rechts davon sind nahezu gleich dunkelgrün, jede Schwelle nimmt den Boden mit — und ein
maskierter Boden würde dich beim Danebenstehen verschlucken. Das braucht eine gezeichnete
Freistellung von dir.

282/282 Tests.
