# RE1.5 Port — v0.7.5 (Early Preview)

**Drei Korrekturen aus deinen F9-Marken vom 07.09.** — zwei davon Fehler, die ich selbst
in v0.7.4 eingebaut hatte.

---

## 1. „MASSENWEISE quatsch Wände, die es nicht gibt"

Berechtigt. In v0.7.4 hatte ich **355 Linien** auf 70 Rechtecke gemalt, bis zu **27 in
einem einzigen Raum**.

**Der Fehlschluss:** Aus „die SCA-Zellen sind die Wände" folgt *nicht*, dass jede solide
Zelle eine Wand der Karte ist. Ein Tisch, eine Säule, eine Kiste sind genau dieselbe Art
Zelle. Mein Filter („beidseits liegt gemalte Fläche") passt auf ein Möbelstück mitten im
Zimmer wortwörtlich genauso wie auf eine Trennwand. Dazu kommt: der Künstler malt seine
Innenwände selbst in die Kachel — alles, was der Port zusätzlich zeichnet, ist per
Definition eine Wand, die es im Original nicht gibt.

**Der Beleg, den ich jetzt verlange:** Die Zelle muss zwei Absetzpunkte von
**Selbst-Türen** desselben Raums *trennen*. Eine Selbst-Tür existiert nur, weil man von A
nach B nicht laufen kann; liegt eine Wandzelle zwischen ihren Enden, ist sie diese
Grenze. Ein Möbelstück ist es nie.

| Raum | Wandzellen | Selbst-Türen | gezeichnet |
|---|---|---|---|
| ROOM1110 | 12 | 4 | **2** — deine beiden aus `howto.png` |
| ROOM1130 (wo du standst) | 10 | 0 | **0** |
| ROOM1120 / 1140 / 10E0 / 10D0 | 12–22 | 0 | **0** |

**355 → 10 Linien im ganzen Spiel.**

⛔ Der eigentliche Befund: **kein Test hat die 355 bemerkt.** Neu ist deshalb
`test_map_innenwand` — er deckelt die Gesamtzahl auf 24 und die je Rechteck auf 6, gibt
alle Wände namentlich aus und pinnt ROOM1110s zwei fest. Wer dort anschlägt, hat wieder
Möbel für Wände gehalten; die Grenze ist dann *nicht* zu heben.

## 2. Das Treppenhaus war in beiden Achsen gespiegelt

Du standst im Treppenhaus auf 2F, **668 bzw. 413 Welteinheiten** von der Türmitte — also
praktisch in der Tür. Trotzdem lagen dein Marker (136,139) und die Tür (118,149)
**28 px** auseinander. Es war weder das eine noch das andere allein, sondern die
Orientierung der ganzen Zone:

| Spiegelung | Standort → Türmarke | Türprojektion → Türmarke |
|---|---|---|
| 0/0 (bisher) | 28 px | 29 px |
| 1/0 | 15 | 12 |
| **1/1** | **8** | **5** |

Die Gegenprobe stammt aus deinem eigenen Bildschirmabzug und ging nicht in die Herleitung
ein: mit 1/1 landen die zwei Treppensymbole auf (126,144) und (134,146) — im Bild liegen
sie bei x123..135, y143..149.

Betroffen sind nur 2F und 3F. Auf 1F rechnet die Original-Kartenzeile `@0x800768b0`, die
keine Spiegelung benutzt.

Festgeschrieben ist nicht eine Einzelposition, sondern die **Zusammengehörigkeit**: wer an
der Tür steht, muss auf der Karte an der Tür stehen.

## 3. Die Türmarken

- **Fahrstuhltür mittig.** Sie ist als einzige **ungepaart** und lief an dem ganzen Block
  vorbei, in dem gemittet wird. Ihr Trigger ist 2000 Welteinheiten breit (Karte
  x133..137), die gemalte Kabinenwand nur 10 px — das Symbol saß am Rand. Jetzt auf der
  Wandmitte, mit derselben am Original gemessenen 16-px-Grenze wie bisher.
- **Tür Richtung Treppenhaus.** Die Auswahl zwischen den beiden Seiten war ein
  **Münzwurf**: das Kriterium misst gegen das Nachbar-*Rechteck*, und auf 3F liegt Rect 1
  komplett in Rect 5 — also 0 gegen 0, und dann gewann stumpf die erste Seite mit 18 px
  Fehler. Die gemalten *Flächen* überlappen nicht; ihre gemeinsame Grenze entscheidet es
  jetzt. Deine F9-Marke (138,152), das Ergebnis (136,152).

---

## Offen und ehrlich

- Ein Riegel steht auf 9 statt 8 überlappenden Türsymbol-Paaren. Das zusätzliche Paar
  konnte ich **nicht benennen** — statisch liegt kein Symbolpaar dichter als 2 px, die
  neun entstehen erst zur Laufzeit. Begründung und Weiterweg stehen im Test.
- Die Mittigkeit überschreibt deine Messung vom 06.09. an der 2F-Fahrstuhltür (117,143 →
  113,143). Die Geometrie ist auf beiden Etagen identisch, es kann nur eine der beiden
  Aussagen gelten; ich habe deine spätere genommen.

**281/281 Tests**, lokal und im Linux-Container.
