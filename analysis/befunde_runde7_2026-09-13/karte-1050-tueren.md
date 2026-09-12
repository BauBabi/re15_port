# ROOM1050: 3 Tueren fehlen auf der Map + ein Stueck Wand fehlt — Befund 2026-09-13

Nutzer: *"in ROOM1050 fehlen 3 Tueren auf der Map. Da stehe ich davor. Dann fehlt auf
der Map auch ein Stueck Wand. Auch da stehe ich davor."* Marken 4-8,
`re15_port/build/platform/pc/befund.log` Zeilen 59607/59621/59683/59739/59779.

## 1. Die fuenf Marken (Log + Projektion)

Projektion = Zeilen-Pfad `re15_map_zones.c:277-292` mit der 1050-Zeile
`re15_map_zones.h:43` (ox 311, oy 59, sx 2428, sy 2229, flip 1,1; Rect 0 = (180,69) 32x96,
uv(0,32), Blatt 2 = MAP03.PIX). Nachgerechnet in Python, exakt die C-Arithmetik.

| Marke | Art | Welt (x,z) | Karte (px) | Ort |
|---|---|---|---|---|
| 4 (F239)  | Spielort | (14168, 9482)   | (204,147) | IN Tuer-Trigger slot1 (Rect z8200..10200) |
| 5 (F338)  | Spielort | (14168, 6513)   | (204,141) | IN Tuer-Trigger slot2 (z5100..7100) |
| 6 (F1143) | Spielort | (14168, -14341) | (204, 97) | IN Tuer-Trigger slot3 (z-14450..-12450) |
| 7 (F1862) | KARTE    | (20116, -22909) | (190, 78) | Suedkopf, neben 1090-Marke |
| 8 (F2342) | KARTE    | (14168, -14582) | (204, 96) | direkt an slot3-Tuer |

Gegenprobe am Screenshot: der gezeichnete Spielermarker (Farbe 120,112,0) steht in
Marke 7 bei Karte (189.3,77.3), in Marke 8 bei (203.3,95.3) — die Projektion oben ist
also exakt der Live-Codepfad (Bilddiff beider PNGs, Cluster-Zentroiden).

## 2. Befund A: die drei Tueren nach ROOM1000 fehlen

### 2.1 RDT-Tuerdump (Door_aot_set 0x3B, `shared_assets/PSX/STAGE1/ROOM1050.RDT`)

| Datei-Offset | Slot | Rect (x,z,w,d) | Mitte | Ziel | Projektion (1050-Zeile) |
|---|---|---|---|---|---|
| 0x00ADA | 0 | (21150,11350,1000,2000)  | (21650,12350)  | ROOM1030 | (187,153) |
| 0x00AFA | 1 | (13200,8200,1000,2000)   | (13700,9200)   | ROOM1000 | **(205,147)** |
| 0x00B1A | 2 | (13200,5100,1000,2000)   | (13700,6100)   | ROOM1000 | **(205,140)** |
| 0x00B3A | 3 | (13200,-14450,1000,2000) | (13700,-13450) | ROOM1000 | **(205, 98)** |
| 0x00B5A | 4 | (16700,-14700,1000,2000) | (17200,-13700) | ROOM10A0 | (197,98) |
| 0x00B7A | 5 | (18100,-23900,3500,1000) | (19850,-23400) | ROOM1090 | (191,77) |

Marken 4/5/6/8 stehen exakt auf den Projektionen von slot1/2/3. Die gemalte Ostwand-
Spalte der Rect-0-Kunst ist x=206 (MAP03.PIX, Index 4 durchgehend bei (u26,v96..100/
v103../v110..), also Karte (206,96..100)/(206,138..142)/(206,145..149) — alle drei
1x5-Balkenlagen liegen vollstaendig auf Wand).

### 2.2 Bestand nach Runde 5/6 (`re15_port/engine/src/re15_map_zones.h`)

Zone-6-Marken heute: (191,71) Z.337 [1090], (186,152) Z.338 [1030], (197,94) Z.343
[10A0] — und fuer die DREI 1000er-Tueren die Zeilen 330-332:

```
330:    {  2,  4,  180,  159, 1,   1,   6, 0 },
331:    {  2,  4,  180,  164, 1,   0,   6, 0 },
332:    {  2, 255,  185,  164, 0,   0,   6, 0 },
```

Am Karten-Abzug gemessen (Tick-Farbcluster 224,168,40 in Marke 7/8): gezeichnet sind
auf dem 1050-Komplex genau (180,159), (186,152), (191,71), (197,94) — **von den drei
1000er-Marken erscheint nur EINE, und die 25 px von der Tuer entfernt**. Zwei Defekte:

1. **Lage falsch.** x=180 ist die WESTKANTE von Rect 0. Die Zeichnung ist aber um 180
   Grad gedreht (flip 1,1): die Tueren der Welt-Westwand (x=13700) liegen auf der
   Karten-OSTWAND x=205/206. Der Kommentar Z.325-329 verraet die Ursache: der
   Generator-Snap wurde von der `unit_map_durchgang`-AUSSEN-Schiene auf die
   Kasten-Ueberdeckung gedrueckt ("die alten 169/176er ... lagen 11-14 px ausserhalb
   der Kasten-Ueberdeckung"). Das Schienen-Modell "Tuermarke sitzt in der Ueberdeckung
   der beiden Rechtecke" (test_map_durchgang.c:222-285) gilt fuer gespiegelt
   montierte Nachbarn NICHT — Rect-4-Kunst (Halle, Ostkante x186) und Rect-0-Streifen
   (Westwand x197) beruehren sich gar nicht, dazwischen ist Blau. Klasse: Memory
   reai-v2-schranke-strenger-als-befund.
2. **Sichtbarkeit falsch gebunden.** Z.331/332 tragen zid=0 (1000-z0) primaer und
   auf_partner=0. Sichtbarkeits-Gate `re15_map_zones.c:779-796`: primaere Zone
   unbesucht + auf_partner=0 => unsichtbar. Im Lauf des Nutzers war nur 1000-z1
   besucht (Log: Sitzungsstart F1 in R1000 bei (650,3250), das liegt in der
   z1-Box) => (180,164)/(185,164) werden NIE gezeichnet, obwohl er IN 1050 vor den
   Tueren steht. Der Spieler-in-1050-Fall haengt an Zone 6 — die kommt in den drei
   Zeilen nur als zid2 vor.

## 3. Befund B: das fehlende Wandstueck = Schwenk-Fresser-Beifang

Sichtbar in Marke 7 UND 8 (Kerbe rechts oben am Streifen, direkt ueber dem
Spielermarker von Marke 8): am Versatz Suedkopf-Ostwand (x203, y69..88) ->
Streifen-Ostwand (x206, y88..155) fehlen die zwei Verbindungs-Texel.

- **Auslieferungs-Asset:** `shared_assets/PSX/DATA/MAP03.PIX`, Zeile v=51
  (Karte y=88): Bytes @0x198C=0x44, @0x198D=0x04 => Texel u23..u26 = Index 4 (WAND)
  = Karte x203..206 durchgehende Wand.
- **Port zeichnet:** x203 Wand, **x204 braune Fuellung, x205 NICHTS (Blau)**, x206
  Wand (Pixel-Zensus Marke 8, Zellen-Mehrheitsfarbe bei 3x-Blit).
- **Mechanismus:** `re15_schwenke_entfernen` (inv_render_pc.c:409 ff, Saat
  `lauf<=S_SAAT=4` Z.466-468, Komponenten-Schranke NUR nach oben `>MAX_AUSD=10`
  Z.498-503, Ersatz durch haeufigsten Nicht-Wand-Nachbarn Z.515-530). (u24,v51) und
  (u25,v51) haben je: horizontaler Lauf 4 (u23..26), vertikaler Lauf 1 => lauf=4 =>
  Saat; die Nachbarn x203/x206 haben lange Vertikallaeufe (>10) und bleiben; die
  2-Texel-Komponente (Bbox 2x1) faellt unter die 10x10-Schranke => als "Tuerschwenk"
  ersetzt: (204,88) bekam Fuellindex 1, (205,88) Index 0 — exakt das gezeichnete Bild.
- **Statische Nachrechnung von Pass 1 ueber alle 13 Blaetter** (Rechtecke aus
  PSX.EXE @0x80076840, RECT_FIX Blatt 3 wie gen_map_zones.py, MAP0x.PIX): 102
  entfernte Komponenten, davon **18 Kruemel mit <=2 px** (u.a. exakt unsere @(24,51)
  Blatt 2; ferner 2x2er auf Blatt 8/9/10/11 bei (143..152,31..39), 2x1 Blatt 9
  (20,135), 1x2-Trio Blatt 12 (78,79/89/99)). **Der kleinste ECHTE Schwenk hat 7 px
  bei Bbox 3x4** (Blatt 7 @(50,93)); alle 84 echten sind in beiden Achsen >=3.
  Eine Mindestgroessen-Klausel (beide Bbox-Achsen >=3) trennt also sauber.

Frage 3 der Aufgabe ("endet die Kunst frueher als der Raum?"): Nein — gemalt ist
x180..206 / y69..155, die Raum-Bbox projiziert auf x176..207 / y75..161 (schematische
Kunst, bekannte 1-6-px-Raender). Beide markierten Orte (190,78) und (204,96) liegen
IN der bemalten Flaeche; es fehlt keine Rect- oder Floor-Zeile, nur die zwei
gefressenen Wand-Texel.

## 4. PATCH-PLAN (nichts ausgefuehrt)

1. **`re15_port/engine/src/re15_map_zones.h:330-332`** ersetzen (Kommentar Z.325-329
   mit anpassen):
   ```
   {  2,  0,  206,  147, 1,   6,   1, 0 },   /* 1050 slot1 <-> 1000-z1: Tuer (13700,9200)  -> (205,147), Wandspalte x206 */
   {  2,  0,  206,  140, 1,   6,   1, 0 },   /* 1050 slot2 <-> 1000-z1: Tuer (13700,6100)  -> (205,140) */
   {  2,  0,  206,   98, 1,   6,   0, 0 },   /* 1050 slot3 <-> 1000-z0: Tuer (13700,-13450)-> (205, 98) */
   ```
   Primaer zid=6 (sichtbar, sobald 1050 besucht und Rect 0 gezeichnet — das ist der
   gemeldete Fall), zid2 = 1000-Zone als Partner-Info, auf_partner=0 (Punkt liegt auf
   keiner fremden Kachel-Geometrie ausser Rect 0 selbst). kind=1 (senkrechte Wand,
   Renderer liest nur die Achse, re15_inv_screen.c:1975). Alle drei 1x5-Balken liegen
   nachgemessen komplett auf Wandindex 4 => unit_map_marke_auf_kunst-sicher.
2. **`re15_port/platform/pc/src/inv_render_pc.c`** — Mindestgroesse fuer "ist ein
   Schwenk", an BEIDEN Komponenten-Schranken (sonst frisst Pass 2, was Pass 1 nun
   stehen laesst, denn er saet dieselben lauf<=4-Pixel neu):
   - Z.498: `if (x1-x0+1 > MAX_AUSD || y1-y0+1 > MAX_AUSD || x1-x0+1 < 3 || y1-y0+1 < 3)`
   - Z.570: dieselbe Klausel zusaetzlich zu `|| n > 13`.
   Erwartung: exakt die 18 Kruemel (Liste oben, darunter die 1050-Kerbe) bleiben
   Wand; kein echter Schwenk (alle >=3x3) ist betroffen. Abnahme mit
   `re15_port/tools/tuersymbole.py` ueber alle 13 Blaetter diffen (Abtrags-Spanne
   muss in den dokumentierten 2,9-8,6 % bleiben); Pass-2-Nebenwirkungen sind damit
   mitgemessen.
3. **`re15_port/tests/unit/test_map_durchgang.c` Abschnitt (5)** (AUSSEN-Schiene,
   Z.222-285): Ausnahme fuer gespiegelt montierte Paare einziehen, z.B. nach dem
   Kasten-Holen: `if (za->flip_x != zb->flip_x || za->flip_z != zb->flip_z)
   continue;` mit Benennungs-printf. Ohne das feuert die Schiene auf die neuen
   Marken (Kaesten Rect 0 x180..211 vs Rect 4 x119..190 ueberlappen; gemessen
   d=16/16/43 px > Schranke 4) — genau der Druck, der die Marken auf x=180 verbogen
   hat. Danach `aussen`/`schlimmst` neu verankern (die drei 1010er-Randfaelle
   fallen mit derselben Ausnahme ebenfalls raus; Schranke ehrlich nachziehen,
   Memory reai-v2-pin-fixture-verschiebung).
4. **Doku-Seed:** im Kommentar `gen_map_zones.py:1286-1290` (1050-Block) die
   Paar-Snap-Falle nachtragen: Beruehrungs-Modell der Paar-Marken gilt nicht fuer
   flip-montierte Zonen; Hand-Stand im Header ist massgeblich (wie 1010).
5. **Verifikation:** local_build + ctest (unit_map_durchgang, _marke_auf_kunst,
   _mark_band, _tuerachse, _eingemessen); dann Nutzer-F9-Lauf 1030 -> 1050: drei
   gelbe Balken auf der Ostwand x206 bei y147/140/98 an seinen Standorten, Kerbe
   (204..205,88) geschlossen.

OFFEN (nicht raten): Hallen-seitige Gegenstuecke der drei Tueren auf der Rect-4-Kunst
(Ostkante x186). Die Kunst der Halle malt dort keine Tuersymbole, und die
1000-z1-Projektionen (167,159)/(167,168) liegen 19 px von der gemalten Wand — ob dort
Marken hingehoeren und wohin genau, braucht eine eigene Messung; Zeile 330-332 alt
war jedenfalls beides nicht (weder Tuerlage noch Wand).

## Fazit

1. Die drei "fehlenden" Tueren sind die West-Tueren nach ROOM1000 (RDT slot1/2/3,
   Datei-Offsets 0x00AFA/0x00B1A/0x00B3A); der Nutzer stand bei den Marken 4/5/6/8
   nachweislich IN deren Trigger-Rechtecken.
2. Mit der (korrekten) Runde-5-Zeile projizieren sie auf (205,147)/(205,140)/(205,98)
   — die OSTWAND x206 des gezeichneten Streifens, denn die Zeichnung ist um 180 Grad
   gedreht. Dort existiert keine einzige Marke.
3. Die drei Bestandszeilen re15_map_zones.h:330-332 sitzen stattdessen auf der
   WESTKANTE x180/185: ein Fehl-Snap unter dem Druck der AUSSEN-Schiene von
   unit_map_durchgang, deren Beruehrungs-Modell fuer flip-montierte Atlas-Nachbarn
   falsch ist (Rect-4- und Rect-0-Kunst beruehren sich nicht).
4. Zwei der drei sind obendrein unsichtbar, weil sie primaer an Zone 0 (1000-z0)
   haengen, die der Nutzer nie betrat (auf_partner=0, Gate re15_map_zones.c:779 ff);
   nur (180,159) wurde gezeichnet — 25 px von der Tuer, im Fuellbereich.
5. Das fehlende Wandstueck ist die 2-Texel-Stufe (204..205, y88) zwischen
   Suedkopf-Ostwand und Streifen-Ostwand: in MAP03.PIX vorhanden (Bytes
   @0x198C/0x198D, Index 4), im Port vom RE1.5-Tuerschwenk-Entferner
   (inv_render_pc.c:409 ff) als 2-px-"Schwenk" gefressen und durch Fuellung/Schwarz
   ersetzt — Saat lauf<=4, keine Mindestgroesse nach unten.
6. Statisch ueber alle 13 Blaetter nachgerechnet: 18 solcher <=2-px-Kruemel werden
   heute gefressen, der kleinste echte Schwenk hat 7 px bei Bbox 3x4 — eine
   Mindestgroessen-Klausel (beide Achsen >=3, an BEIDEN Paessen) trennt exakt.
7. Patch: drei Marken-Zeilen ersetzen (206,147/140/98, zid 6), Schwenk-Fresser-
   Untergrenze, AUSSEN-Schienen-Ausnahme fuer flip-Paare samt Neu-Verankerung;
   Hallen-seitige Gegenmarken bleiben OFFEN, bis ihre Wandlage gemessen ist.
