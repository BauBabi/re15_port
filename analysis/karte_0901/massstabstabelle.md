# Die Karten-Maßstabstabelle @0x800768b0 — was RE1.5 wirklich mitliefert

**Datum:** 2026-09-01 · **Anlass:** Nutzer-Report „ab Police Station 2F ist die Karte
vollkommen im Eimer" (Punkt 8 von 8) · **Ergebnis:** eine selbst gerechnete Eichung
zurückgenommen, die ausgelieferte Tabelle eingesetzt.

## 1. Die Formel

`FUN_800473f8` @0x8004741c–0x80047528 rechnet Welt → Kartenpixel:

```
mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
```

Die Zeile `{s16 ox, s16 oy, u16 sx, u16 sy}` steht @0x800768b0, 8 Byte je Raum.
**Zeilenindex = Stage-Basis + Raumnummer**, mit denselben Basen wie der Seiten-Setzer
@0x8004b568: +0 / +38 / +50 / +65 / +77 / +98.
`sx`/`sy` werden mit `lhu` geladen — **vorzeichenlos**, es gibt also keine
gespiegelten Kacheln (der Verdacht ist damit ausgeräumt).

## 2. Nur 33 von 72 Karten-Räumen sind geeicht

| Stage | Räume mit Zeile | Räume gesamt |
|---|---|---|
| 1 | 10 | 29 |
| 2 | 1 | 7 |
| 3 | 7 | 11 |
| 4 | 4 | 7 |
| 5 | 9 | 14 |
| 6 | 2 | 4 |
| **Σ** | **33** | **72** |

Die übrigen 39 tragen den Stub `{0, 0, 1, 1}`. Mit `sx = sy = 1` kollabiert die Formel:

```
Welt(     0,     0) -> Karte(0,0)
Welt( -8000, 12000) -> Karte(0,0)
Welt( 20000, -5000) -> Karte(0,0)
```

**Der Auslieferungsstand zeigt in diesen 39 Räumen also gar keinen brauchbaren
Positionsmarker** — er klebt in der Bildschirmecke. Das Kartensystem war im Prototyp
unfertig. Was der Port dort tut (Bbox-Streckung), ist deshalb eine **Port-Ergänzung
ohne Original-Vorbild**, kein Nachbau. Sie ist bewusst so gebaut, dass sie die Eingabe
beschneidet und damit nie aus dem Rechteck läuft.

## 3. Die Zeile sagt auch, WELCHES Rechteck gemeint ist

Projiziert man die SCA-Zellen eines Raums mit seiner Zeile, fällt das Ergebnis auf genau
ein Rechteck seiner Seite (die Seite steht per @0x8004b568 fest). Damit ist die
Rechteck-Zuordnung **abgeleitet statt geraten**:

| Raum | Port hatte | Zeile sagt | Anteil Punkte im Rect | Füllung | Türen auf ihren Symbolen |
|---|---|---|---|---|---|
| 1020 | 2/3 | **2/1** | 92 % | 93 % | 4,0 px |
| 1070 | 2/6 | **2/3** | 97 % | 82 % | – |
| 1200 | 1/5 | **1/2** | 86 % | 100 % | 4,1 px |
| 1210 | 1/4 | **1/3** | 93 % | 53 % | 2,8 px |
| 1150 | 4/2 | 4/2 | 92 % | 89 % | – |
| 11B0 | 0/3 | 0/3 | 97 % | 87 % | 3,0 px |
| 40A0 | 8/11 | **8/10** | 94 % | 62 % | 2,0 px |
| 5040 | 9/0 | **9/4** | 78 % | 98 % | 2,2 px |
| 5100 | 11/1 | **11/0** | 100 % | 64 % | 2,2 px |

Die vier fett gesetzten STAGE1-Korrekturen decken sich exakt mit dem, was ein
unabhängiges Audit am selben Tag aus der Geometrie vorgeschlagen hatte.

## 4. Wache

Eine Zeile kommt nur in den Generator, wenn

* ≥ 75 % der begehbaren Rasterpunkte (SCA-Zellen, RDT +0x20) im Rechteck landen,
* die Projektion ≥ 50 % des Rechtecks füllt,
* und — wo Symbole gemalt sind — die Türen ≤ 8 px danebenliegen.

Teilen sich zwei Räume eine Zeile (5040/5120 und 50A0/5140 tun das), bekommt der
bessere Treffer das Rechteck, der andere fällt auf die Bbox-Streckung zurück.

## 5. Was vorher dastand — und warum es falsch war

Bis v0.3.70 rechnete der Generator sich die Eichung selbst aus: Maßstab aus der
gezeichneten Fläche, Versatz über eine Zuordnung Tür→Symbol, danach zwei
Partner-Durchgänge und ein Federmodell. Gemessen an den SCA-Zellen:

| | vorher | jetzt |
|---|---|---|
| Klemmrate über alle begehbaren Punkte | bis 100 % je Zone (37/37 geeichte Zonen klemmten, 6 davon zu 100 %) | **1,04 %** |
| Symbolabstand Median | wertlos (29 der 37 Zonen haben nur EIN Symbol → Fehler per Konstruktion 0,0 px) | **7,5 px** (Zeilen-Zonen 2,0–2,8 px) |

Die Lehre steht als eigener Merksatz: **ein Gütemaß, das auf dieselben Daten optimiert
wird, an denen es misst, misst nichts.**
