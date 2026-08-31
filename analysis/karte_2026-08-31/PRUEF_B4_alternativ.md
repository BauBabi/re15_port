# GEGENPRUEFUNG B4 — Rolle: ALTERNATIV-ERKLAERER

**Behauptung B4:** Seite 4 Rect 4 (144,80) 32x80 uv(0,32) gehoert ROOM1130
(32,0 x 83,3 px gegen 32x80, Delta 3 px, schaerfster Treffer der Seite);
ROOM1130 fehlt in `re15_map_zones.h`, r4 ist unbelegt — echte Luecke.

**ERGEBNIS: NICHT widerlegt.** Ich habe vier konkurrierende Erklaerungen gesucht und
jede mit demselben Mass gegen die Berichts-These gestellt. Keine haelt stand; zwei
voellig unabhaengige Masse (Tuer-Kohaerenz, ausgerichtete Formueberlappung) bestaetigen
r4 = ROOM1130 sogar staerker als die Groessen-Heuristik des Berichts.
**ABER: die vom Bericht angefuehrte BEGRUENDUNG ("Delta 3 px", "fester Weltmassstab")
ist falsch bzw. wertlos** — und der Bericht laesst einen Original-Befund weg, der fuer
die Umsetzung entscheidend ist (§6). Beides unten mit Zahlen.

Sonden: `scratchpad/B4/{tiles,dup,dup2,geo,doortest,full,census,shape,explain,align}.py`.
Adresse -> Datei-Offset: `off(a) = a - 0x80010000 + 0x800`.

---

## 0. Was ich zuerst nachgemessen habe (Grundlage)

* **Seite 4 = genau die Raeume 0x1120..0x1160.** Sprungtabelle @`0x8001103c` (Datei
  `0x183c`), Faelle 18..22 -> `0x8004b6fc/710/724/738/74c`; die ersten vier fallen
  durch bis `0x8004b74c` + 12: `ori v0,zero,0x4` (Bytes `04 00 02 34` @Datei `0x3bf58`),
  danach `j 0x8004b888` -> `sb v0,DAT_800b260e`. Raumindex = `(rid>>4)&0xFF - 0x100`,
  also 18=1120, 19=1130, 20=1140, 21=1150, 22=1160. **Fuenf Raeume, sieben Rechtecke.**
* **`re15_map_zones.h` hat wirklich keinen 0x1130-Eintrag** (grep: 0 Treffer); Seite 4
  belegt r1,r2,r3,r5,r6 — **r0 und r4 sind frei**. Der Generator meldet beim Lauf selbst
  `Seite 4: 5/6 Zonen zugeordnet (7 Rects)`: die 6. Zone (= ROOM1130) laesst seine
  Kostenfunktion bewusst leer. Die Luecke existiert also, sie ist kein Tippfehler.
* ROOM1130 ist **eine** Zone, nicht zwei: mit GAP 0/250/500/1000/1500/3000 immer
  1 Komponente, Bbox x[-8650,6000] z[-18400,19750] = 14650 x 38150. Die Bbox ist also
  kein Verschmelzungs-Artefakt.

---

## 1. Die BEGRUENDUNG des Berichts haelt nicht — der Massstab ist NICHT fest

Bericht §5: *"`sx/sy ~ 2280..2300` ist der feste Weltmassstab der Karte"*, daraus
*"Delta 3 px = der schaerfste Treffer der Seite"*.

Gemessen an der Tabelle @`0x800768b0` (Datei `0x670b0`) selbst — alle 13 kalibrierten
sx-Werte:

| sx | 1776 | 1920 | 2080 | 2229 | 2272 | 2280 | 2287 | 2296 | 2304 | 2340 | 2428 | 2496 | 3168 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|

**Spanne 1776..3168 = Faktor 1,78.** Anisotropie innerhalb einer Zeile bis
`sx/sy = 3168/2305 = 1,374` (ROOM1200 @`0x800769b0`) bzw. 1920/2301 = 0,834
(ROOM1190 @`0x80076978`). Mit einem Massstab, der um bis zu 38 % schwankt, ist die
Vorhersage "83,3 px" auf gut ±30 px unscharf — **ein Delta von 3 px gegen 8 px oder
17 px unterscheidet dann gar nichts.** Die Berichts-Rangfolge in §7
("Delta 3 / 8 / 8 / 17 / 31") ist als Praezisionsaussage wertlos und **kein**
tragfaehiger Beleg. Die These wird trotzdem getragen — von anderen Messungen (§2-§4).

Nebenbefund: die 32x80-**Box** ist nicht die gezeichnete Flaeche. Gemessen an MAP05
(4bpp, 128 B/Zeile) ist die Nicht-Null-Bbox von r4 **26x79**; dasselbe 6-px-Polster
haben r0 (10x10 in 16x16), r1 (18x22 in 24x24), r2 (26x40), r3 (42x22), r5 (34x40).
Wer die Box misst, misst 6 px Luft mit.

---

## 2. Konkurrent A — "ein anderer Raum der Seite 4": widerlegt

Alle Zonen der Seite 4 am Massstab 2290, gegen r4 = 32x80 (`census.py`):

| Zone | px | Verhaeltnis w/h | Delta zu 32x80 |
|---|---|---|---|
| **ROOM1130 z0** | **32,0 x 83,3** | **0,384** | **3,3** |
| ROOM1120 z0 | 41,9 x 45,9 | 0,914 | 44,0 |
| ROOM1140 z0 | 59,6 x 59,9 | 0,995 | 47,7 |
| ROOM1140 z1 | 27,7 x 13,3 | 2,082 | 71,0 |
| ROOM1150 z0 | 31,3 x 40,0 | 0,782 | 40,7 |
| ROOM1160 z0 | 36,6 x 28,6 | 1,279 | 56,0 |

Das Seitenverhaeltnis ist **massstabsfrei** (und damit immun gegen §1): r4 = 0,400,
ROOM1130 = 0,384; der naechste Raum liegt bei 0,782. Selbst die groesste gemessene
Anisotropie (1,374) bringt keinen anderen unter 0,57.

**Gegenprobe ueber den ganzen STAGE1** (alle Basisraeume, alle Zonen): innerhalb 15 px
von 32x80 liegen genau **zwei** Zonen — ROOM1130 (3,3) und ROOM1050 (29,3 x 87,7,
Delta 10,4). ROOM1050 liegt auf Seite 2 (1F) und hat Tueren nur zu 1030/1000/10A0/1090,
**keine** zu irgendeinem Raum der Seite 4. Der einzige Groessen-Konkurrent ist damit
geografisch ausgeschlossen.

**Massstabs-Plausibilitaet als hartes Sieb** — welchen sx/sy muesste ROOM1130 haben,
damit ein anderes Rechteck passt?

| Rect | noetiges sx | noetiges sy | in der Original-Spanne 1776..3168? |
|---|---|---|---|
| **r4 32x80** | **2290** | **2199** | **ja, beide** |
| r2 32x40 | 2290 | 1099 | nein |
| r5 40x40 | 2863 | 1099 | nein |
| r6 56x56 | 4008 | 1539 | nein |
| r3 48x24 | 3435 | 660 | nein |
| r1 24x24 | 1718 | 660 | nein |
| r0 16x16 | 1145 | 440 | nein |

Nur r4 haelt ROOM1130 in dem Wertebereich, den das Original in seiner eigenen Tabelle
tatsaechlich benutzt. **Das ist das Argument, das der Bericht haette fuehren muessen.**

---

## 3. Konkurrent B — "r4 ist eine FREMD-Kachel von einem anderen Blatt": widerlegt

Dieser Mechanismus existiert auf genau dieser Seite nachweislich (der Bericht nutzt ihn
selbst fuer r3 = Seite-5-r0). Ich habe **alle 119 Rechtecke aller 13 Seiten** als
Kacheln extrahiert und byte-verglichen (`dup.py`):

```
EXAKTE Duplikate im ganzen Spiel:
  16x16 : Seite2 r9 == Seite3 r9 == Seite4 r0     <- 100 % identisch
  24x24 : Seite2 r10 == Seite4 r1                 <- 100 % identisch
  48x24 : Seite4 r3 == Seite5 r0                  (Masken-IoU 1,000, 22/1152 Pixel Farbe)
  ... ausserdem Seite1 r4/r5 und r6/r7/r8; Seite8/9/10/11; Seite12 r1/r2/r3
```

**Seite-4-r4 kommt in keiner dieser Gruppen vor.** Beste Aehnlichkeit ueberhaupt:
0,42 Masken-IoU (gegen Seite2 r2 24x80 bzw. Seite5 r1) — gegen 1,000 bei den echten
Kopien. Auch die naheliegendsten Verdaechtigen sind es nicht: r4 vs Seite2 r0
(32x96, gleiche uv-Ecke) IoU 0,218; vs Seite3 r0 IoU 0,363.
**Der Fremd-Kachel-Mechanismus trifft auf r4 nicht zu.**

Nebenbefund (nicht Teil von B4, erklaert aber, warum r0 uebrigbleibt): r0 und r1 SIND
Fremd-Kacheln. Groessenprobe: ROOM1080 z1 = 13,5 x 13,3 px gegen r0 16x16 (Delta 5,2),
ROOM1060 = 24,2 x 27,8 px gegen r1 24x24 (Delta 4,0) — und genau diese beiden Raeume
sind die Ziele der zwei Tueren von ROOM1120 (`lokal(1300,6400)->0x1080`,
`lokal(-700,9550)->0x1060`), beide auf dem 1F-Blatt, wo dieselben zwei Kacheln als
Seite2 r9/r10 stehen. Damit blieben fuer die fuenf 3F-Raeume real nur r2/r4/r5/r6 —
was die Berichts-Zuordnung ROOM1160 -> r1 in Frage stellt, nicht aber ROOM1130 -> r4.

---

## 4. Zwei UNABHAENGIGE Masse — beide bestaetigen r4 = ROOM1130

### 4a. Tuer-Kohaerenz (benutzt keine Groesse)

Dieselbe Tuer muss unter den Rechtecken **beider** Raeume auf denselben Kartenpunkt
fallen. ROOM1130 ist der Knoten mit den meisten Kanten (4 Tueren: ->1120, ->1140,
->1150, ->1170 z1), das Mass ist dort am schaerfsten. Anker: ROOM1150 -> r2
(original-belegt ueber Skalenzeile 21 @`0x80076958`).

| Rect fuer ROOM1130 | mittlerer Tuer-Fehler (11 Kanten) |
|---|---|
| **r4** | **11,14 px** |
| r5 | 23,61 px |
| r2 | 27,57 px |
| r3 | 27,71 px |
| r1 | 28,38 px |
| r0 | 29,65 px |
| r6 | 31,90 px |

Vollstaendige Suche ueber alle 720 injektiven Zuordnungen der 7 Seite-4-Zonen auf die
7 Rechtecke (Anker 1150->r2): **die sechs besten Loesungen haben ALLE `1130 -> r4`**
(10,19 / 10,19 / 10,54 / 10,54 / 11,14 / 11,14 px). Erst Platz 7 weicht ab
(`1130 -> r5`, 18,05 px). Einzelkanten unter der Berichts-Zuordnung:
1130<->1150 = 3,3 / 3,4 px, 1130<->1170z1 = 2,5 / 2,5 px, 1130<->1140 = 4,0 / 4,1 px.

### 4b. Formueberlappung mit optimierter Ausrichtung (benutzt keine Tueren)

Gezeichnete Innenflaeche (Palettenindex 1) gegen die FREIE Flaeche zwischen den
SCA-Zellen, Versatz ±4 px und Massstab 0,80..1,10 optimiert (die Wanddicke faellt so
heraus). Gleiches Mass fuer alle Kandidaten:

| Raum (fuer die Kachel r4) | beste IoU |
|---|---|
| **ROOM1130 z0** | **0,491** |
| ROOM1150 z0 | 0,347 |
| ROOM1140 z0 | 0,334 |
| ROOM1080 z0 | 0,321 |
| ROOM1160 z0 | 0,319 |
| ROOM1230 / ROOM1180 | 0,313 |
| ROOM1120 z0 | 0,309 |
| ROOM1050 z0 | 0,268 |

Eichung mit demselben Mass: das **bewiesene** Paar ROOM1150 -> r2 erreicht 0,714,
ROOM1140 -> r6 0,656, ROOM1160 -> r1 0,541, ROOM1120 -> r5 0,519. 0,491 liegt also im
Band der plausiblen Paare, mit 0,144 Abstand zum Zweitplatzierten.

Qualitativ (ASCII-Gegenueberstellung in `shape.py`): beide zeigen dieselbe Z-Folge —
breiter Raum oben, schmaler Gang links hinunter, Versatz nach rechts auf Zeile ~30-39,
zweiter Gang rechts hinunter. Der Versatz sitzt in Kachel und Kollision auf derselben
Hoehe.

---

## 5. Was ich NICHT bestaetigen kann (offener Punkt der Seite, nicht von B4)

Die Tuer-Kohaerenz sagt zugleich, dass die Berichts-Zuordnung **ROOM1120 -> r5** mit
`1130 -> r4` **unvertraeglich** ist: die Kante 1120<->1130 hat 38,3 / 41,2 px Fehler,
waehrend die drei anderen 1130-Kanten bei 2,5..4,1 px liegen. Ein Rechteck, das beide
erfuellt, gibt es auf Seite 4 nicht. Das trifft die 1120-Zeile, nicht die 1130-Zeile.

---

## 6. Der Befund, den B4 auslaesst: das ORIGINAL kann ROOM1130 gar nicht markieren

Marker-Skalenzeile fuer Raumindex 19 (= ROOM1130) @`0x80076948` (Datei `0x67148`):
**`{ox 0, oy 0, sx 1, sy 1}` — Stub.** (Ebenso 1120 @`0x80076940` und 1160
@`0x80076970`; auf Seite 4 ist **nur** ROOM1150 @`0x80076958` kalibriert.)

Mit der Originalformel FUN_800473f8 @`0x8004741c`-`0x80047528`
(`mx = ox + ((((X+32000)*10*sx) >> 20) + 5)/10`, y gespiegelt) liefert sx=sy=1 fuer
JEDE Spielerposition `mx = 0, my = 0`: im Original steht die Marke in ROOM1130 in der
**Bildschirmecke (0,0)**, nie in r4.

Konsequenz: r4 = ROOM1130 ist die richtige *Zeichnung*, aber es gibt **keinen**
Original-Laufzeit-Beleg dafuer — der Beweis ist indirekt (Groesse+Massstab, Tuergraph,
Form). Ein Port-Eintrag `{0x1130, ..., 4, 4, ...}` macht den Port an dieser Stelle
**besser als das Original**, nicht byte-true — dieselbe Klasse Entscheidung wie beim
Save-Ortsnamen (Memory `reai-v2-save-room-name-stub`). Das gehoert in die
Commit-Message, sonst wird ein bewusster Zusatz spaeter fuer byte-true gehalten.

---

## 7. Fazit

`refuted = false`. Es gibt keine gleich gute Alternativerklaerung: kein anderer Raum der
Seite (§2), kein Raum des ganzen STAGE1 ausser dem geografisch unmoeglichen ROOM1050
(§2), keine Fremd-Kachel (§3) — und zwei von der Groesse unabhaengige Masse zeigen in
dieselbe Richtung (§4). Zu korrigieren ist die **Beweisfuehrung** (§1: der Massstab ist
nicht fest, "Delta 3 px" belegt nichts) und zu ergaenzen ist §6 (das Original hat fuer
ROOM1130 keine Markerabbildung).
