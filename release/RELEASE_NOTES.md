# RE1.5 Port — v0.7.4 (Early Preview)

**Aus deinen F9-Marken vom 07.09.** Der wichtigste Fund ist größer als die Wand, um die
es ging: mein Kartengenerator hat die Kollisionsdaten seit Monaten **invertiert** gelesen.

---

## Die Kollisionszellen sind die WÄNDE, nicht der Boden

Das steht seit dem 2026-06-07 in `re15_collision.c`, nach einem 135-Agenten-RE:

> *„the player walks in the band-MATCH-FREE complement … band-4 cells = walls"*

Der Generator hat daraus Grundrisse gebaut — also die Wände als begehbare Fläche
gezeichnet. An **3727 Standorten**, die du selbst abgelaufen bist (13 Räume aus
`befund.log`), liegen nur **2,9 %** in einer soliden Typ-1-Zelle des eigenen Bands; in
**10 der 13 Räume kein einziger**. Wären es Bodenzellen, müsste der Wert bei 100 % liegen.

Damit stand deine Trennwand die ganze Zeit in den Daten. Gegen deine fünf F9-Marken:

```
Wand y121..125  (x189..212)
   Marke 2 (190,126)   Marke 3 (212,129)      oberer Raum
Wand y130..136  (x189..212)   <-- deine Trennwand, gezeichnet bei y=134
   Marke 4 (190,137)   Marke 5 (212,140)      unterer Raum
Wand y141..144  (x189..212)
```

Zwei geometrische Regeln, keine gewählte Zahl: **Außenwände** werden nicht gezeichnet
(die malt die Kachel selbst — eine Innenwand hat beidseits Raum), und gezeichnet wird die
**Mittellinie**, nicht die Fläche (eine Weltwand ist ~2000 Einheiten = 5–7 px dick und
würde den Raum zumauern).

## „Zu lange Wand in der falschen Farbe"

**Länge:** jede Wand endet jetzt an der gemalten Fläche. Die alte lief y113..145 und
ragte 9 px ins Leere.

**Farbe:** sie trug rgb(176,176,176) — den Palettenindex, in dem der *Künstler* auf seine
Kachel malt. Der Port malt Rechtecke aber nach Zustand: aktuell rgb(192,24,24), besucht
rgb(40,144,40). Eine graue Linie im roten Raum ist sichtbar fremd. Die Innenwand nimmt
jetzt die Umrandungsfarbe ihres Raums.

## Die zwei Türmarken

| | deine F9-Marke | Türdaten von deiner Seite | vorher |
|---|---|---|---|
| Fahrstuhl | (134,146) | (135,146) — 1 px | (135,146), am Wandrand |
| Treppenhaus | (138,152) | (139,152) — 1 px | **(153,142)** — 18 px |

**Fahrstuhl:** die Marke ist **ungepaart** und lief an dem ganzen Block vorbei, in dem
überhaupt gemittet wird. Ihr Trigger ist 2000 Welteinheiten breit = Karte x133..137, die
gemalte Kabinenwand nur x127..136 — das Symbol saß am rechten Ende und ragte heraus.
Wandlauf 10 px, Mitte **x=131**. Die Mittigkeit gilt jetzt für alle Marken, mit derselben
am Original gemessenen 16-px-Grenze wie bisher.

**Treppenhaus:** die Marke wird von zwei Seiten gerechnet, und die Auswahl war ein echter
**Münzwurf**. Das Kriterium misst gegen das Nachbar-*Rechteck*; auf 3F liegt Rect 1
komplett in Rect 5, also 0 gegen 0 — und dann gewann stumpf die erste Seite. Die gemalten
*Flächen* überlappen nicht, sie ergänzen sich; ihre gemeinsame Grenze ist die Wand, in der
die Tür sitzt. Rect 5 liegt 2 px davon entfernt, Rect 1 vier.

---

## Zwei Dinge, die kein Fortschritt sind

**1. Ich überschreibe eine ältere Messung von dir.** Am 06.09. standst du an der
**2F**-Fahrstuhltür und hast (117,143) bestätigt — das ist *nicht* mittig. Heute sagst du
für dieselbe Stelle auf 3F „in die Mitte". Die Geometrie ist auf beiden Etagen identisch:

```
2F  Zeile 143:  #......##########      Wandlauf x109..118, Mitte 113, Marke auf 117
3F  Zeile 146:  #......##########      Wandlauf x127..136, Mitte 131, Marke auf 135
```

Es kann also nur eine der beiden gelten. Ich habe deine heutige, ausdrückliche Anweisung
genommen und beide Aussagen im Test hinterlegt.

**2. Ein Riegel steigt von 8 auf 9.** Die Umbauten haben 40 der 193 Marken bewegt, und
dabei ist ein zusätzliches Paar überlappender Türsymbole entstanden. Das Paar konnte ich
**nicht benennen**: in der statischen Marken-Tabelle liegt kein Symbolpaar dichter als
2 px beieinander — die neun entstehen erst zur Laufzeit. Ich habe die Schranke angehoben
und im Test hingeschrieben, dass das eine Verschlechterung ist und wie man sie
weiterverfolgt. Auf der Original-Kunst sind es 7; dorthin gehört das Ziel.

**Offen:** die 2F-Treppenhaustür steht jetzt bei (118,149) statt (135,139). Dafür habe ich
nur deine Zeichnung, keine Messung — ein F9 davor legt sie fest.

---

**280/280 Tests**, lokal und im Linux-Container.
