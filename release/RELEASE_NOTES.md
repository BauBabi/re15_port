# RE1.5 Port — v0.7.2 (Early Preview)

**Nachtrag zu v0.7.1, aus deinen F9-Marken vom 07.09.:** die Tür in ROOM1100 stand an der
Seite statt am Ende des Gangs, und das neue Zeilenprofil hatte ich zu breit angewandt —
auf Tischplatten, wo es nicht hingehört.

---

## 1. „Die Tür befindet sich am Ende des Ganges, nicht an der Seite"

Du standest bei Welt(−26232, −10781) = Karte **(192,121)**, unmittelbar an der Tür nach
ROOM1110. Von *deiner* Seite projiziert diese Tür auf **(190,121)** — 2 px daneben. Die
gesetzte Marke lag aber auf **(197,114)**, auf der Oberkante.

**Ursache:** ROOM1110 lag auf dem Rechteck *über* ROOM1100. Der Paar-Zusammenzug schob die
Marke deshalb auf die dortige gemeinsame Kante. Die beiden Seiten derselben Tür
widersprachen sich dabei um 23 px.

Vier unabhängige Messungen zeigen, wohin ROOM1110 gehört:

| | Rect 5 | Rect 7 (bisher) |
|---|---|---|
| Abstand deines Standorts zur **gemalten** Fläche | **1 px** | 12 px |
| Seitenverhältnis (ROOM1110 ist 2,02:1) | 2,25:1 | 1,00:1 |
| Größe (erwartet 66×33 px) | 72×32 | 48×48 |
| Übereinstimmung beider Türseiten | **4 px** | 23 px |

Die Marke steht jetzt auf **(187,122)** — Westwand, also am Gangende, 4 px von deinem
Standort.

## 2. Das Zeilenprofil war zu breit angewandt

`aufrecht` (Tiefe je Bildzeile) ist nur für **wirklich senkrechte** Gegenstände richtig.
Ich hatte es auch auf Tischplatten gelegt, mit der Begründung „große Tiefenspanne der
Spaltenregel". Das war ein Fehlschluss: die Spanne kommt dort nicht von der Höhe, sondern
davon, dass die untere Silhouettenkante die eigene Plattenkante ist und nicht der Boden.
Ein senkrechtes Profil durch diesen Punkt macht die ganze Fläche viel zu nah.

An deiner Marke F4933 gemessen (872 Punkte des Körperkastens):

| Modell | verdeckende Punkte |
|---|---|
| `aufrecht` | **872** — du warst vollständig verschwunden |
| Spaltenregel | **43** |
| eine Tiefe | 872 |

Vier Objekte zurückgestellt: ROOM10C0 02_02, ROOM10D0 07_02, 08, 09_02.

## 3. Die Liege in ROOM10E0 schnitt dich von der Seite

In der Freistellung `07_02` steckt **neben der Wand auch die Liege davor**. Die
Spaltenregel nimmt je Spalte den untersten Punkt — bei den Liegen-Spalten ist das deren
Rolle, also ein Standpunkt *vor* der Wand. Der Standpunkt von Spalte 110 ist
Welt(−2351,−421); du standest bei (−2360,−164), **9 Einheiten daneben**. Das senkrechte
Profil lief damit genau durch dich hindurch.

Die Bodenlinie der **Wand** ist an den Spalten 164…188 und 236…300 als Gerade ablesbar;
die Liegen-Spalten brechen sie um bis zu 34 Zeilen. Mit dieser Begrenzung:

| | in deinem Körperkasten |
|---|---|
| vorher | 971 verdeckende Punkte, Tiefe 78…103 |
| jetzt | **0**, Tiefe 117…127 |

Die Wand blockt also weiter alles dahinter.

⚠️ **Offen:** die Liege verdeckt damit gar nicht mehr, obwohl man hinter sie kommt
(gemessen 7315 begehbare Punkte zwischen Liege und Wand). Alle vier Modelle sind an deiner
Marke durchgerechnet — keines kann beides in *einer* Datei, weil die Wand senkrecht und
die Liege waagerecht ist. Sie braucht eine eigene Freistellung.

## Deine zwei Vergleichsmarken (ROOM10D0)

Am Abzug gegen den Hintergrund gemessen — nicht am Körperkasten, denn der ist mit ±450 um
die Hüfte breiter als die Figur und überlappt die Maske auch dort, wo nichts gezeichnet
ist:

| | gezeichnete Figurpunkte | davon unter der Maske |
|---|---|---|
| dahinter | 256, x157…175 | **0** — die Liege verdeckt dich dort wirklich |
| daneben | 400, x167…188 | **0** — ihre Fläche endet bei x=177 |

Beide Bedingungen waren bereits erfüllt. Sie sind jetzt als Prüfung festgeschrieben: die
erste über die Tiefe, die zweite über die **Breite** der Maske — wächst sie nach rechts,
fällt die Bedingung, und das würde keine Tiefenprüfung bemerken.

---

**279/279 Tests**, lokal und im Linux-Container.
