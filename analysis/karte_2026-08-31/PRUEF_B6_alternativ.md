# GEGENPRUEFUNG B6 — ALTERNATIV-ERKLAERER

Prueflauf 2026-08-31. Rolle: nicht den Beleg nachrechnen, sondern eine BESSERE Erklaerung
suchen. Alle Zahlen unten sind selbst gemessen (EXE-Bytes, PIX-Pixel, RDT-Bytes, Port-Sonde).
Sonden im Scratchpad `.../scratchpad/b6/`: `an.py, assign.py, tiles.py, marks.py, markfit.py,
doorsy.py, proj.py, s_b6.c/.exe`.

## Geprueft: B6

> Port-Ergaenzung: Karte in ROOM1170 schaltet auf **Seite 4 / Rect 3** um, sobald der Spieler
> nach dem Treppenlauf auf **Band 0** steht. Zone 1 behaelt Seite 5 / Rect 0. Rect 3 wird durch
> Entfernen des Artefakts ROOM1140-Zone-1 frei, ROOM1130 bekommt Rect 4.

**Ergebnis: WIDERLEGT** — nicht die Rect-IDENTIFIKATION (die haelt sogar besser als in B),
sondern die Entscheidung. Es gibt eine mindestens gleich gute Alternative mit weniger
Zusatzmechanik, und die Umsetzung tut, so wie formuliert, **messbar nicht das, was sie
behauptet**.

---

## 0. Was ich BESTAETIGE (unabhaengig nachgemessen)

| Behauptung aus B | mein Nachmessen |
|---|---|
| Rect-Tabellen Seite 4/5 | identisch reproduziert (@0x80076840 -> ptr 0x80076468 / 0x800764bc) |
| Seite-4-r3 == Seite-5-r0 | **22/1152** Pixel verschieden, **alle** (4->1), Silhouetten-IoU **1,000** |
| Seite = nur Raumnummer | 0x8004b568 `lh DAT_800b0fe2` -> Tabelle @0x8001103c[23] = 0x8004b75c -> `ori v0,zero,5` (@0x8004b768) -> `sb v0,0x260e` (@0x8004b88c). **7 Schreiber** von 0x800b260e (0x8004b7dc/88c/9bc/c84/dbc/f58/c040), alle in der Raum-Switch-Familie; **2 Leser** (0x80047048 Rect-Liste, 0x800474a0 Marker) |
| Karte liest kein Band | `DAT_800acad6` hat **27 Xrefs**, hoechste **0x80038edc** — keine im Kartenbereich 0x80046a1c-0x8004c3fc |
| ROOM1130 -> r4 | in **jeder** meiner Top-Zuordnungen; Groessen-Delta 3,3 px (32,0x83,3 gegen 32x80) |
| ROOM1140-Zone-1 ist ein Artefakt | comp2 x[-5250,7450] z[-17450,-11350] liegt **vollstaendig** in comp0 x[-10600,16700] z[-25600,1850], beide Band 0 |
| r3-Inhalt = ROOM1170 2. Bereich | **verschaerft bestaetigt**, siehe §3 |

---

## 1. Die Umsetzung flippt VOR der Treppe, nicht danach (Port-Sonde, gemessen)

Der Port waehlt die Zone ueber die **kleinste passende Bbox** (`re15_map_zones.c:88-91`,
ZONE_SLACK 1500). Eine zusaetzliche Band-0-Zone haette die Bbox der Band-0-Zellen
x[-28622,-17876] z[-28841,-17070] (skalierte Flaeche 30561) und schluege damit die comp1-Zone
x[-28900,-8784] z[-28841,-17070] (57462) ueberall im Ueberlapp.

**Band 0 und Band 4 sind in (x,z) nicht trennbar** — gemessen aus ROOM1170.RDT @0x0a80:

```
Band-4-Zellen, die in die Band-0-Bbox ragen:
  x[-21689, -9106] z[-28734,-27134]  -> Schnitt 3813 x 1600 =  6.100.800
  x[-21353, -8784] z[-24434,-23154]  -> Schnitt 3477 x 1280 =  4.450.560
  x[-21946,-20471] z[-28302,-22914]  -> Schnitt 1475 x 5388 =  7.947.300
Summe 18.498.660 Einheiten^2 = 6,6 % der comp1-Band-4-Flaeche (281.106.428)
```

Sonde `s_b6.exe` (gegen `libre15_engine.a` gelinkt), Seite die B6 liefern wuerde:

| Punkt (Band) | B6-Seite |
|---|---|
| Ankunft Selbsttuer (-11710,-26500) Band 4 | 5 |
| **Treppe Band 4 (-20690,-25595)** | **4 — falsch** |
| **Korridor West (-21000,-28000) Band 4** | **4 — falsch** |
| **Korridor Mitte (-19000,-27800) Band 4** | **4 — falsch** |
| Korridor Ost (-12000,-26800) Band 4 | 5 |
| Treppe Band 2 / Band 2 / Band 0 | 4 |

=> Die Karte schaltet bereits **oben am Treppenkopf** und mitten auf dem oberen Korridor um —
also **vor** dem Abstieg, genau entgegen der Formulierung "sobald der Spieler nach dem
Treppenlauf auf Band 0 steht". Ein *korrekter* Band-0-Test braucht das Spieler-Band
`DAT_800acad6` (entity+0x82) — das die Original-Kartenroutine nachweislich **nie** liest (§0).
B6 ist damit keine Datenzeile, sondern **eine neue Mechanik ohne Original-Vorbild**.

---

## 2. Die Praemisse "Rect 3 wuerde verwaisen" ist falsch — verwaiste Rects sind auf Seite 4 die REGEL

Ich habe **alle Rects aller 13 Seiten** nach gemeinsamem `(w,h,u,v)` gruppiert und die Kacheln
pixelweise verglichen (`tiles.py`). Es gibt **7 seitenuebergreifende Kachel-Paare mit
Silhouetten-IoU 1,000**, davon **5 mit 0 Pixel Unterschied**:

| uv | Groesse | Seiten | Unterschied |
|---|---|---|---|
| (0,32) | 16x16 | 8, 9, 10 | 0 / 256 |
| **(168,40)** | **16x16** | **2, 3, 4** | **0 / 256** |
| (16,32) | 24x24 | 8, 9, 11 | 0 / 576 |
| **(168,16)** | **24x24** | **2, 4** | **0 / 576** |
| (136,16) | 64x40 | 10, 11 | 0 / 2560 |
| (40,32) | 48x56 | 10, 11 | 28 / 2688 (nur Tuermarken) |
| **(192,16)** | **48x24** | **4, 5** | **22 / 1152 (nur Tuermarken)** |

**Seite 4 hat also DREI seitenuebergreifende Doppel-Kacheln: r0, r1 und r3** — bei nur
**fuenf** Raeumen (Index 18..22 = ROOM1120/1130/1140/1150/1160; Fall-Through
0x8004b6fc-0x8004b758 -> `ori v0,zero,4`) auf **sieben** Rects.
r0 ist byte-gleich Seite-2-r9 und Seite-3-r9, r1 byte-gleich Seite-2-r10.

=> Eine Kachel, die auf mehreren Blaettern steht, ist die **Konvention des Zeichners fuer
Raeume an der Blattgrenze** (vertikale Verbinder / Treppenhaeuser), kein "verwaistes" Rect,
das einen Besitzer braucht. Der Port zieht daraus schon heute eine falsche Konsequenz: er
vergibt Seite-2-r10 an ROOM1020-Zone-1 **und** die byte-gleiche Kachel Seite-4-r1 an ROOM1160
(`re15_map_zones.h:14,58`) — dieselbe Zeichnung an zwei verschiedene Raeume.

**Und:** das Original faerbt gar nicht ein — es zeichnet alle `count` Rects der Seite
unterschiedslos (Schleife @0x800475f8-61c, CLUT 0x7d50 @0x800473cc). Die Zustands-Einfaerbung
ist selbst eine dokumentierte Port-Erweiterung. Der originalnaechste Zustand fuer ein Rect
ohne Marker-Besitzer ist genau das, was der Port `RE15_MAP_RECT_UNMAPPED` nennt (Stock-Grau,
immer gezeichnet — `re15_inv_screen.c:1544-1548`).

---

## 3. Die Rect-IDENTIFIKATION haelt sogar besser als in B — und spricht GEGEN das Aufteilen

Ich habe die zwei grauen Innenmarken von Seite-4-r3 exakt ausgelesen (Index-4-Pixel, deren
8er-Nachbarschaft komplett ungleich 0 ist): Schwerpunkte **(13,4 / 3,2)** und **(11,6 / 17,8)**,
je 11 Pixel. Ihre Form ist ein **architektonisches Tuersymbol** (Blattlinie + Schwenkbogen):

```
  1 O##########O###O#O           16 O#########OOO#####...
  4 O###########O##O#O           18 O#########O###O###...
  5 O############OOO#O           21 OOOOOOOOOOOOOOOOOO...
```

**Tuerpaar-Test bei WAHREM Kartenmassstab** (sx=2280 / sy=2268, Zeile 23 @0x80076968 — keine
freie Skalierung, nur eine Verschiebung ist frei). Gemessener Marken-Versatz dx=+1,8 dy=-14,6.
Alle 16 Tuerpaare der Seite-4-Raeume + 1170-Bereich-2:

| Kandidat | Tuerpaar-Versatz | 2-D-Restfehler |
|---|---|---|
| **ROOM1170 B2: ->1130 & ->1140** | dx=+0,80 dy=-19,64 | **5,14 px (bester)** |
| ROOM1120: ->1060 & ->1080 | dx=-4,35 dy=-6,81 | 9,92 px |
| ROOM1120: ->1080 & ->1130 | dx=+21,20 dy=-20,12 | 20,2 px |
| ROOM1140: ->1130 & ->1170 | dx=-2,94 dy=+37,85 | 23,3 px (passt nicht in 24 px Hoehe) |
| ROOM1130: ->1150 & ->1170 | dx=-20,33 dy=-3,68 | 24,7 px |

=> **r3 = ROOM1170, 2. Bereich, mit seinen zwei 3F-Tueren** ist unter allen Kandidaten klar
der beste — Faktor 1,9 vor dem naechsten. Die Identifikation aus B ist damit **staerker**
belegt als dort.

**Aber genau das spricht gegen B6:** die Kachel zeigt **beide Ebenen in EINEM Bild** (linker
Block = Band 0/2, unterer Arm = Band 4; identische Silhouette zu Seite-5-r0). B6 will diese
eine Zeichnung nach Band **aufteilen** und ihre Haelften auf zwei Seiten legen — gegen die
Absicht, mit der sie gezeichnet wurde.

### Nebenbefund, der B6s Etagen-Argument zusaetzlich verdreht

Aus den Tuer-Datensaetzen (Ankunfts-Y; Band = `-(y/0x708)`, `re15_collision.c:173`):

```
ROOM1130 -> ROOM1170 : Y =     0  -> Band 0
ROOM1140 -> ROOM1170 : Y =     0  -> Band 0
ROOM1170 -> ROOM1170 (Selbst-Tuer aus dem Hof) : Y = -7200 -> Band 4
```

Band 0 ist also die **3F-Ebene** (dort haengen 1130/1140), Band 4 liegt **ueber** 3F (Hof-/
Dach-Niveau). Waere die Seite wirklich etagen-getrieben, braeuchte Band 4 ein "4F"-Blatt — das
es nicht gibt (Seite 5 traegt laut Titelkachel **gar keine** Etagenangabe). Das Original loest
genau das, indem es Raum 23 ein eigenes, etagenloses Blatt gibt, auf dem **beide** Ebenen in
einer Kachel stehen. B6 zerlegt diese Loesung und hat fuer Band 4 dann kein Blatt mehr.

---

## 4. Die Zuordnung ist mit B's eigenen Kriterien nicht eindeutig (Gegenkandidat mit Zahlen)

Optimierung ueber alle injektiven Zuordnungen (Kosten = Groessen-L1 in Kartenpixeln +
Tuergraph-Restfehler in Bildschirmpixeln, `assign.py`):

```
mit 1170-B2 auf Seite 4:   157,1  (Groesse 42,4 + Tuer 114,7)
                           1120->r5 1130->r4 1140->r6 1150->r2 1160->r1 1170B2->r3
ohne 1170-B2 (5 Raeume):   106,5  (Groesse 54,4 + Tuer  52,2)
                           1120->r3 1130->r4 1140->r6 1150->r2 1160->r5
```

Der **Tuergraph** bevorzugt also deutlich **ROOM1120 -> r3** (52,2 statt 114,7). Dieser
Kandidat verliert nur am Groessenmass (Delta 28,0 statt 5,8) und am Pixel-/Tuersymbol-Test aus
§3 — die Zuordnung ist also **kriterienabhaengig**, nicht eindeutig. Ebenso bleibt ROOM1160
offen: bestes Gesamt je Rect r5 = 106,5 / r1 = 108,9 / r3 = 129,8 — drei Kandidaten innerhalb
von 23 Punkten.

---

## 5. Ein messbarer Defekt, den B6 nicht adressiert

Die Marker-/Marken-Projektion des Ports streckt die comp1-Bbox linear auf das Rect:

* x: 20116 Einheiten -> wahr 20116*2280/2^20 = **43,74 px**, gestreckt auf **48** = **+9,7 %**
* z: 11771 Einheiten -> wahr 11771*2268/2^20 = **25,46 px**, gestaucht auf **24** = **-5,7 %**

Folge, gemessen gegen die Kachel-Pixel: von den **drei** Tuermarken, die der Port fuer Zone 27
zeichnet (`re15_map_zones.h:285,286,288`), landen **zwei auf TRANSPARENTEN Kachelpixeln**:

```
(158, 81) = Kachel(18, 1)  -> Zeile  1, Spalte 18 = '.'   ausserhalb der Zeichnung
(182,101) = Kachel(42,21)  -> Zeile 21, Spalte 42 = '.'   ausserhalb der Zeichnung
(157,100) = Kachel(17,20)  -> '#'  (drin)
```

Ebenso mappt die Suedkante des oberen Korridors auf Kachel-Zeile 22/23 — beide Zeilen sind
**komplett leer** (Sonde: (-21000,-28000) -> Kachel(18,22), (-19000,-27800) -> Kachel(23,21)).
Der Marker verlaesst dort die gezeichnete L-Form. Das ist **ohne jede Seiten-Umschaltung**
behebbar: Projektion auf den **wahren** Massstab (sx=2280/sy=2268 @0x80076968) statt
Bbox-Streckung, Ursprung an der Zeichnung ausgerichtet.

---

## 6. DIE ALTERNATIVE (mindestens gleich gut, mit weniger Zusatzmechanik)

**A. Seite bleibt raumgetrieben (Original).** Raum 23 -> Seite 5, immer
(@0x8004b75c-68 -> @0x8004b88c). Keine Band-Abfrage in der Kartenlogik — die es im Original
nicht gibt (27 Xrefs von DAT_800acad6, keine im Kartenbereich).

**B. Seite-4-r3 bekommt KEINEN Marker-Besitzer**, sondern bleibt — wie die beiden anderen
Doppel-Kacheln derselben Seite (r0, r1) — Stock-Grau/immer gezeichnet
(`RE15_MAP_RECT_UNMAPPED`). Das ist zugleich der originalnaechste Zustand, weil das Original
alle Rects einer Seite unterschiedslos zeichnet.

**C. Wenn beim Abstieg etwas Sichtbares passieren soll**, dann am Marker: Projektion auf den
wahren Massstab umstellen (§5). Dann wandert der Marker sichtbar und **innerhalb** der
Zeichnung vom rechten Ende des unteren Arms in den linken Block — genau die Wendeltreppe, die
die Kachel darstellt.

**D. ROOM1130 -> r4 und das Entfernen von ROOM1140-Zone-1** aus B6 sind davon unabhaengig
richtig und sollten bleiben (Delta 3,3 px; nachgewiesene Bbox-Verschachtelung).

---

## 7. Was ich NICHT klaeren konnte

* ROOM1160 (r1 vs r3 vs r5) bleibt auch mit Tuergraph + Groesse unentschieden
  (106,5 / 108,9 / 129,8).
* Ob der Zeichner die Doppel-Kacheln systematisch fuer **Treppenhaeuser** benutzt, habe ich nur
  fuer uv(192,16) belegt (4 Treppen-AOTs im 2. Bereich); fuer uv(168,40), uv(168,16), uv(0,32),
  uv(16,32), uv(136,16) ist der Raum-Bezug offen.
