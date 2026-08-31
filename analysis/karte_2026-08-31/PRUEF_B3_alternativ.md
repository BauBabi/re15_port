# GEGENPRUEFUNG B3 — Rolle: ALTERNATIV-ERKLAERER

**Behauptung B3:** Seite-4-Rect **r3** gehoert NICHT ROOM1140 Zone 1 (wie `re15_map_zones.h:58`
behauptet); ROOM1140 Zone 1 sei ueberhaupt kein getrennter ORT, sondern ein Zerlegungs-Artefakt
des Generators.

**Ergebnis: NICHT widerlegt.** Ich habe drei konkurrierende Erklaerungen gesucht und alle drei
mit eigenen Messungen verworfen. B3 haelt sogar staerker, als der Bericht es belegt — die dort
gegebene BEGRUENDUNG (Bbox-Verschachtelung + gleiches Band) traegt naemlich nicht, das Ergebnis
aber schon. Einen echten Konkurrenten gibt es nur bei einer NEBEN-Frage (wem r3 stattdessen
gehoert: ROOM1170-Bereich-2 vs. ROOM1160).

Alle Zahlen unten sind selbst gemessen. Sonden im Scratchpad: `b3/cells.py, zones.py, gapscan.py,
scale.py, req.py, doors.py, predict.py` sowie die C-Sonden `s_b3.c/s_b3b.c/s_b3c.c/s_b3d.c`
(gegen `libre15_engine.a`, also gegen die byte-true Kollision FUN_8003b0a4).

---

## 1. Was ROOM1140 "Zone 1" wirklich ist (gemessen, nicht modelliert)

`ROOM1140.RDT` SCA-Block @Datei-0x570: Kopf {u16 ceilX, u16 ceilZ, u32 count[5]}, danach
12-B-Zellen `{u16 w, u16 d, s16 x, s16 z, u8 type, u8 u0, u8 u1, u8 floor}`
(Layout = `re15_rdt.h:61-71`, identisch zu dem, was `gen_map_zones.py:112` liest).

* counts = (19,19,19,19,19) = 95 Eintraege, aber nur **19 unterschiedliche** Zellen —
  die 5 Quadranten-Gruppen sind hier byte-gleich.
* **Zone 1 besteht aus genau ZWEI Zellen:**
  `x=-4650 z=-17450 w=12100 d=6100 type=1 floor=0x03` und
  `x=-5250 z=-14400 w=1000 d=1000 type=3 floor=0x03`.
* Alle 19 Zellen haben `u0=0xFF` (Bit0 = solide) und `floor=0x03` -> Band 0.
  Alle drei Tuer-Eintritte nach ROOM1140 haben `y=0` -> Spieler-Band 0.
  Damit sind **alle** Zellen fuer den Spieler push-out-solide (`re15_collision.c` Kopf-Kommentar:
  Zellen mit `band == Spielerband` werden weggedrueckt).

### Sonde: ist die "Zone" begehbar?

`re15_collision_constrain` (= FUN_8003b0a4) auf ein Raster, Punkt gegen sich selbst; jede
Verschiebung = der Punkt liegt in/an einer soliden Zelle.

| Flaeche | Raster | FREI |
|---|---|---|
| **ROOM1140 "Zone 1"** x[-5250,7450] z[-17450,-11350] | 1984 Pkt | **23 = 1,2 %** |
| ROOM1140 ganzer Raum | 4761 Pkt | 1032 = 21,7 % |
| **ROOM1170 Zone 1** (der Split, den B als echt fuehrt) | 5959 Pkt | **3932 = 66,0 %** (Band 0) |
| ROOM1170 Zone 0 (Hof, Band 4) | 5256 Pkt | 54,1 % |
| ROOM1150 / 1160 / 1130 / 1120 (je 1 Zone) | — | 33,0 / 59,3 / 32,3 / 37,7 % |

Die begehbaren Punkte in "Zone 1" bilden einen **100 x 6100** Welt-Streifen am Westrand
(= 0,2 x 13,3 Kartenpixel). Mitte (1100,-14400) wird um **+3518** in z hinausgedrueckt.

**Flood-Fill der Bodenflaeche** (4er-Nachbarschaft, Schritt 100): ROOM1140 hat **eine**
begehbare Hauptkomponente mit 93,1 % aller freien Felder, x[-8100,14200] z[-23100,-7700] —
sie laeuft um den Zone-1-Kasten HERUM. Der Kasten ist ein Loch im Boden, kein Ort.

**Fazit:** Zone 1 ist der solide Moebel-/Podest-Block in der Mitte des Briefing-Raums.
Der Generator clustert **Kollisions-Zellen** (Hindernisse), nicht Bodenflaeche — deshalb wird
ein Block in der Raummitte zu einer "Zone". Das ist der Mechanismus; B3s Ergebnis stimmt.

### Warum B3s eigene Begruendung nicht traegt

"Bbox liegt vollstaendig in Zone 0" und "beide Band 0" beweisen nichts:
verschachtelte Bboxen koennen sehr wohl zwei echte Orte sein, und ROOM1170 Zone 0 vereint
umgekehrt die Baender 3 UND 4 in EINEM Ort. Der Beweis ist die **Begehbarkeit** (1,2 % vs. 66 %).

### Schwellen-Test (zeigt, wie fragil der Split ist)

Zonenzahl als Funktion von `GAP` (Generator: GAP=1500):

| Raum | Split faellt bei GAP = | Reserve gegenueber 1500 |
|---|---|---|
| **ROOM1140** | **2175** | +45 % — kippt bei jeder kleinen Aenderung |
| ROOM1170 | **6175** | +312 % — robust |

(ROOM1150 hat bei GAP<1200 sogar 3 "Zonen", bei 1500 nur noch 1 — dieselbe Artefakt-Klasse.)

---

## 2. Konkurrent A: "Zone 1 ist ein echter Ort, nur das Rechteck ist falsch"

Waere sie ein echter Ort, muesste ein Seite-4-Rect zu ihr passen. Pruefmass ohne freie
Parameter: die Original-Markerskala `@0x800768b0` (8 B/Zeile `{s16 ox, s16 oy, u16 sx, u16 sy}`).
Die 13 kalibrierten Zeilen benutzen **sx 1776..3168, sy 1954..2320**. Fuer eine Zone der
Weltbreite W auf ein Rect der Breite w gilt `sx = w * 2^20 / W`.

ROOM1140 Zone 1 = 12700 x 6100:

| Rect | r0 16x16 | r1 24x24 | r2 32x40 | **r3 48x24** | r4 32x80 | r5 40x40 | r6 56x56 |
|---|---|---|---|---|---|---|---|
| noetiges sx/sy | 1321/2750 | 1982/4126 | 2642/6876 | **3963/4126** | 2642/13752 | 3303/6876 | 4624/9626 |

**Kein einziges Rect der Seite liegt im Original-Bereich.** Fuer r3 braeuchte es sy = 4126 —
**78 % ueber** dem groessten sy, das das Original ueberhaupt je verwendet (2320), und sx = 3963,
25 % ueber dem groessten sx (3168). Zum Vergleich: ROOM1170-Bereich-2 -> r3 verlangt
**2502/2138** — beides mitten im Original-Bereich.
Konkurrent A faellt: die Zone ist auf Seite 4 gar nicht zeichenbar.

## 3. Konkurrent B: "Die Karte ist nicht massstaeblich, also taugt Groesse nicht als Mass"

Halb richtig — B's Satz "sx ~ 2280..2300 ist der feste Weltmassstab" ist zu stark
(die kalibrierten Zeilen streuen sx 1776..3168, also Faktor 1,78). Aber genau deshalb habe ich
oben mit dem **gemessenen Original-Intervall** statt mit einem Mittelwert geprueft — und
1140z1 -> r3 liegt auch dann noch weit ausserhalb. Konkurrent B aendert das Ergebnis nicht.

## 4. Konkurrent C (der ernsteste): unabhaengige Vorhersage der ganzen Seite aus dem Tuergraphen

Voellig anderes Verfahren, nur Original-Daten, keine Port-Tabelle:
Markerformel FUN_800473f8 (`mx = trunc((((X+32000)*10*sx)>>20 + 5)/10) + ox`,
`my = trunc(-(((Z+32000)*10*sy)>>20 + 5)/10) + oy`), **verifiziert** an der einzigen
kalibrierten Seite-4-Zeile 21 (ROOM1150 @0x80076958, ox=111 oy=130 sx=2296 sy=2312):

```
ROOM1150 begehbar x[-28658,-14358] z[-27684,-10684] -> Bildschirm x[118,150] y[83,120]
Seite-4 r2 (120,80) 32x40                           -> x[120,152] y[ 80,120]   (<=3 px, alle 4 Kanten)
```

Damit ist ROOM1150 der Anker; ueber die RDT-Tuerpaare (jede Tuer muss auf beiden Seiten
denselben Kartenpunkt treffen) und den Median-Massstab 2290 wird die Seite fortgesetzt:

| Zone | vorhergesagte Lage | bestes Rect | IoU | 2. Platz |
|---|---|---|---|---|
| ROOM1130 (ueber 1150) | x[140,172] y[84,161] | **r4** (144,80) 32x80 | **0,735** | r5 0,245 |
| ROOM1140 z0 (ueber 1130) | x[157,206] y[114,163] | **r6** (160,110) 56x56 | **0,687** | r4 0,214 |
| **ROOM1170 Bereich 2** (ueber 1130) | x[148,192] y[86,111] | **r3** (152,89) 48x24 | **0,641** | r4 0,236 |
| ROOM1170 Bereich 2 (ueber 1140, Gegenprobe) | x[144,188] y[95,120] | **r3** | 0,404 | r4 0,280 |
| **ROOM1140 z1** (dieselbe Abbildung wie z0 — ein Raum hat im Original genau EINE) | x[163,191] y[137,150] | r6 | 0,116 | — |
| **ROOM1140 z1 gegen r3** | — | **r3** | **0,000** | kein Ueberlapp |

Die beiden 1170-Wege stimmen auf **(4,-9) px** ueberein.
**ROOM1140 Zone 1 landet 24 px unterhalb von r3 und beruehrt es nicht einmal.**
Das Rect, das der Port ihr gibt, wird von zwei unabhaengigen Tueren ROOM1170-Bereich-2
zugewiesen. Konkurrent C bestaetigt B3, statt ihn zu ersetzen.

## 5. Wo es WIRKLICH einen Konkurrenten gibt (Neben-Frage, nicht B3)

Wem gehoert r3, wenn nicht 1170-Bereich-2? Einziger ernstzunehmender Zweitkandidat ist
**ROOM1160** (begehbar 16400x13100 -> 35,8 x 28,6 px):

| Kandidat fuer r3 (48x24) | noetiges sx/sy | im Original-Bereich? | Tuergraph-Vorhersage |
|---|---|---|---|
| ROOM1170 Bereich 2 | 2502 / 2138 | **ja / ja** | IoU 0,64 bzw. 0,40 (zwei Tueren) |
| ROOM1160 | 3005 / **1921** | ja / **nein** (1,7 % unter 1954) | **nicht bestimmbar** |
| ROOM1140 Zone 1 | 3963 / 4126 | **nein / nein** | IoU 0,000 |

ROOM1160 ist ueber Tueren gar nicht platzierbar: seine **beiden** Tuersaetze zeigen auf
ROOM1180 und ROOM1230 — es hat **keine einzige Tuer zu einem Seite-4-Raum**. Diese Luecke
bleibt offen (B sagt das in §7.5 selbst). Sie beruehrt B3 aber nicht: beide Kandidaten
schlagen ROOM1140 Zone 1 um Groessenordnungen.

---

## 6. Konsequenz fuer den Port (nur Befund, nichts geaendert)

`re15_map_zones.h:58/59` (`{ 0x1140, -5250, -17450, 7450, -11350, 4, 3, 1, 23 }`) faerbt und
bemarkert ein Rechteck fuer einen soliden Moebelblock. Der Fehler entsteht in
`gen_map_zones.py` **cost()**: dort zaehlen nur `aspect_pen` und `zrank/rrank`, **nie die
absolute Groesse**. 1140z1 hat Seitenverhaeltnis 12700/6100 = 2,08, r3 hat 2,00 ->
aspect_pen = 0,04 -> Kosten 1,83, waehrend das flaechenrichtige r1 auf 5,85 kommt.
Die Zone selbst entsteht, weil `zones_of()` **Kollisions-Zellen** verschmilzt statt Bodenflaeche.

## 7. Datenherkunft

* Rect-Paartabelle `@0x80076840` (Datei 0x67040), Seite 4 ptr `0x80076468` (Datei 0x66c68),
  Seite 5 ptr `0x800764bc`. Selbst gelesen, deckungsgleich mit Bericht B §2.
* Markerskala `@0x800768b0`; Zeile 20 (ROOM1140) `@0x80076950` = `0,0,1,1` = **STUB**
  (das Original hat fuer ROOM1140 ueberhaupt keine Markerabbildung), Zeile 21 (ROOM1150)
  `@0x80076958` und Zeile 23 (ROOM1170) `@0x80076968` sind kalibriert.
* Markerformel FUN_800473f8, Rumpf `@0x8004741c-0x80047528`; die `+32000`-Vorspannung gilt
  fuer BEIDE Achsen (in Bericht B §4c ist die z-Zeile ausgelassen) — empirisch verifiziert,
  weil nur damit ROOM1150 auf r2 faellt (ohne sie landet y bei 190 statt 83..120).
* Kollision: `re15_collision_constrain` = FUN_8003b0a4, Spielerradius PR=450
  (`re15_collision.c:39`).
