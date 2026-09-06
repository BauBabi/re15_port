# RE1.5 Port — v0.7.1 (Early Preview)

**Vier Befunde: der Wandpfeiler in ROOM1130 hat Leon nicht
verdeckt, und auf der 2F-Karte stand ein Türsymbol am falschen Ende des Flurs.**

Beide Male hat das laufende `befund.log` plus dein Tastendruck den Fall entschieden —
aus Screenshots allein wäre keiner von beiden rekonstruierbar gewesen.

---

## 1. ROOM1130: „wenn Leon rennt, sieht man noch seinen Arm durchblitzen"

Es lag **nicht an der Fläche**. Das war am 04.09. schon einmal die Antwort auf denselben
Befund, und sie hat nichts gebracht — damals gemessen: „grow=3 deckt 37/38 ab, verdeckt
aber **0**/38".

Deine Marke F506 (Welt −3818/0/3732, Kamera-Tiefe Fuß/Hüfte/Kopf 6398/6166/5935) am
Abzug nachgemessen: 820 Figurpunkte, 687 ungedeckt, darunter die Bildspalten **76..81** —
das Messer, über den Marmor gezeichnet.

### Die Ursache

Die Spaltenregel nimmt je Bildspalte den **untersten** Punkt der Freistellung als
Bodenkontakt. Bei dieser Wand ist das nur in den Spalten **94..115** wirklich der Boden.
Links davon ist die untere Kante die **seitliche Kante des Pfeilers** — eine senkrechte
Weltkante, die im Bild um 7,4 Zeilen je Bildpunkt ansteigt (Zeile 201 bei x=94 auf Zeile
0 bei x=67). Nachgerechnet mit der Kamera dieses Cuts (H=208):

| Spalte | 94–115 | 89 | 84 | 78 | 76 | 74 | 67–73 |
|--------|--------|----|----|----|----|----|-------|
| Tiefe  | **70–83** | 94 | 141 | 345 | 684 | 32523 | *trifft den Boden gar nicht* |

Verdeckt wird nur mit einer Tiefe unter 92,7 (deine Kopf-Tiefe 5935 / 64). Die Spalten
67..89 lagen **alle** darüber — und die sieben Spalten ganz links ließ das Werkzeug
stillschweigend leer.

Dass 94..115 der echte Wandfuß ist, steht unabhängig fest: ihre Weltpunkte liegen alle
auf **einer Geraden** (x −3734..−3946, z 4231..5189).

### Was geändert wurde

Neuer Tiefen-Modus `bodenkante` in der Freistellungs-Beschreibung: nur die genannten
Spalten tragen ihren eigenen Bodenkontakt, die übrigen erben den der nächsten davon.
Ergebnis: alle Spalten 68..115 tragen jetzt Tiefe **71–78** statt 72–1023.

**Gegenprobe nach echtem Neubau:** altes Modell → 10 offene Spalten (x78..87, genau dort
lag das Messer), neues Modell → 0.

Das Tiefenwerkzeug meldet ab jetzt seine **Abdeckung** („41 von 48 Spalten mit
Bodenkontakt; OHNE Bodenkontakt: 68-74") — vorher fiel so etwas lautlos weg.

⚠️ **Deine `pri/1130/03.png` hat beim Speichern die Transparenz verloren.** Die Datei ist
ein 320×240-RGB-Bild und unterscheidet sich vom Hintergrund nur an 458 Punkten (die
beiden flackernden Wandlampen) — also ein Bildschirmfoto. Ohne Alphakanal gilt die ganze
Bildfläche als Maske (79,1 % statt 19,8 %). Deine Datei ist unangetastet; gebaut wird aus
`pri/1130/03_wandkante.png`. Für eine neue Fassung bitte als PNG **mit Alphakanal**
speichern.

---

## 2. 2F-Karte: das Funkraum-Symbol stand bei den Spinden

Deine drei Marken in ROOM10D0:

| Marke | Welt | was dort ist | Karte |
|-------|------|--------------|-------|
| F9-1 | (1278, −7231) | Tür zum Funkraum | (155, 82) |
| F9-2 | (6960, 4816) | Doppeltür, grünes Kreuz | (143, 108) |
| F9-3 | (−8121, 25008) | Spinde, **keine Tür** | (177, 151) |

> „beim letzten F9 drücken ist eine Tür auf der Map, die nicht da sein sollte. Bei den
> anderen F9 in den Raum ist eine Tür die auf der Map fehlt."

Es war **ein einziges Symbol am falschen Ende**: das der Funkraumtür stand bei (177,155),
4 px neben den Spinden und 63 px von der Tür.

### Die Ursache

Der Zuordnungs-Löser hatte den Funkraum auf das Rechteck **unter** dem Flur gelegt und
ROOM10E0 auf ein schon vergebenes — während die beiden passenden Rechtecke leer blieben.
Drei unabhängige Messungen sagen, wohin sie gehören:

| | F9-1 Funkraumtür | F9-2 Doppeltür | F9-3 Spinde |
|---|---|---|---|
| nächstes **gemaltes** Rechteck | **R9 = 1 px** | **R8 = 3 px** | R2 = 4 px |
| zweitnächstes | R8 = 8 px | R5 = 9 px | R5 = 7 px |

R2 ist genau das Rechteck, auf dem der Funkraum lag — daher das Symbol bei den Spinden.

Dazu die **gemalte Kachel** des Flurs: sie trägt genau zwei Türnischen, bei (143,82) und
(195,149) — 14 bzw. 7 px neben unseren Projektionen, auf **denselben Kanten**. Der
Künstler malt die Tür also dorthin, wo wir sie hinrechnen; falsch war nur, welches
Rechteck daneben liegt. Und die **Größe**: ROOM10E0 misst 45×41 px, ROOM10F0 41×44 px —
in das alte Rechteck (56×32) passt keiner von beiden.

### Ergebnis

| deine Marke | vorher | jetzt |
|---|---|---|
| F9-1 Funkraumtür | Symbol 63 px weg | **1 px** |
| F9-2 Doppeltür | Symbol 70 px weg | **5 px** |
| F9-3 Spinde (keine Tür) | Symbol 4 px daneben | **26 px weg** |

Die Rechteck-Tabelle selbst war korrekt — sie trifft den Original-Grundriss zu 93,1 %.
Falsch war allein, welcher Raum welches Rechteck bekommt.

---

## Drei blinde Messwerkzeuge

Beim Nachgehen kam heraus, dass die Karten-Prüfung seit v0.6.5 **nichts** geprüft hat:

1. Sie las **0 von 192** Türmarken — die Tabellenzeile hat acht Felder, ihr Suchausdruck
   erwartete sieben. Jede Marken-Prüfung meldete deshalb „OK". Berichtigt, dazu bricht
   sie jetzt ab, wenn sie nicht alle Zeilen erkennt.
2. Die Prüfung „Türen ohne Symbol" sprang über **jede** Tür (sie verlangte eine
   Zeichnungsart, die es gar nicht mehr gibt) — und fragte zudem nur, ob der *Raum*
   irgendein Symbol hat, nicht ob *diese Tür* eines hat.
3. Neu dazu: „Türsymbol an einer anderen Wand als die Tür" und „ein Rechteck für zwei
   verschiedene Räume". Beide melden weitere Fälle, die als Nächstes drankommen.

Und der neue Haken für Verdeckungspunkte prüft **nicht** den Körperkasten: dein Messer
lag bei x 76..81, der Kasten reicht nur von 105 bis 133. Hätte er nur den Kasten geprüft,
wäre er auch vor dem Fix grün gewesen.


---

## 3. Der Drehstuhl in ROOM10E0 — Tiefe je BILDZEILE

> „der Drehstuhl muss den Charakter weiter oben überdecken, aber weiter unten vom
> Charakter überdeckt werden."

Mit **einer** Tiefe je Objekt geht das nicht — und zwar genau andersherum: die Kamera
schaut von oben, der **Kopf** der Figur ist also näher als ihre Füße (gemessen 469
Einheiten Unterschied). Eine einzelne Schwelle mitten in der Figur verdeckt deshalb
*immer* das untere Ende und lässt das obere frei.

Richtig ist ein Tiefen**profil** über die Bildzeilen, und das ist geschlossen
ausrechenbar: die Silhouette eines aufrecht stehenden Gegenstands ist die Projektion
einer **senkrechten Weltlinie**. Für den Stuhl: Standpunkt Welt(4011,−4359), Tiefe
**70 oben bis 79 unten** über alle 70 Zeilen.

Gemessen über 2140 begehbare Standplätze, deren Körper die Stuhlmaske berührt:

| Modell | ganz verdeckt | gar nicht | **nur unten verdeckt = verkehrt herum** |
|---|---|---|---|
| eine Tiefe (79) | 1785 | 310 | **45** |
| Zeilenprofil | 1830 | 310 | **0** |

## 4. Die Rückwand in ROOM10E0 — Zombies blocken, Spieler nicht

> „da es Zombies gibt die durch die ganze Wand clippen auf der anderen Seite. Die
> wollen wir blocken. Aber der Spielcharakter selber soll nicht davon geblockt werden."

Beides zugleich leistet genau die richtige **Tiefe**: der Zeichner verdeckt, was
*ferner* ist als die Maske. Wer hinter der Wand steht, verschwindet; wer davor steht,
bleibt sichtbar.

Dafür muss die Maske die Tiefe der **Wandfläche** tragen, nicht die ihres Bodens. Die
reine Spaltenregel gab jeder Bildzeile die Bodentiefe — die Wandoberkante wäre zu fern
gewesen. Jetzt trägt z. B. Spalte 112 **82 oben bis 95 unten** statt durchgehend 95.

Die 41 Spalten über der **Türöffnung** haben keinen eigenen Bodenkontakt (ihr Sehstrahl
landet hinter der Raumwand — nachprüfbar an der Hülle der Kollisionszellen) und erben
den Standpunkt der Nachbarspalte. Derselbe Test verwirft bei den vier anderen
Wand-Freistellungen **keine einzige** Spalte, produziert also keine Fehlalarme.

## Und 27 neue Vordergrundmasken

ROOM10C0 (3 Winkel), ROOM10D0 (5), ROOM10E0 (10). Alle deine Freistellungen sitzen bei
**100,0 % Übereinstimmung**, Maßstab 1 — pixelgenau geschnitten; jede Maske deckt ihre
Fläche punktgenau. Das Tiefenmodell steht je Gegenstand danach, *was er ist*, mit dem
gemessenen Tiefenbereich als Beleg.

Zwei Werkzeugfehler kamen dabei hoch: der Atlas brach bei kleinen Masken ab (die
Palette hatte 220 statt 255 Farben), und die Zensus-Schranke war eine absolute Summe —
sie wäre allein durch *gute* neue Masken gefallen. Maßstab ist jetzt das Original:
2,152 Schnitte je Maske dort, 3,206 bei uns = Faktor **1,49** (vorher 1,48).
