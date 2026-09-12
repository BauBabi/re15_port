# ROOM1010: „Der Raum wird auf der Karte nicht gezeichnet" — Befund 2026-09-12

Nutzer-Report: *„In ROOM 1010 wird mir der Raum auf der Karte noch nicht gezeichnet.
Siehe Marker."* F9-Abzug `re15_port/build/platform/pc/befund_1010_F353_marke1.bmp`
(960×720 = 3× Blatt 2), Welt (3650, 0, −3950), Weg 1040 → 1020 → 1010.

## Fazit vorweg

**Der Marker steht richtig — aber der ganze Raum ist in die ZEICHNUNG DES NACHBARN
hineinprojiziert.** ROOM1010s heutige Zeile (flip 0,1) bildet 100 % seiner begehbaren
Fläche auf die gemalte Kachel von **Rect 1 = ROOM1020** ab. Die EIGENEN Kacheln des
Raums — **Rect 8 (Nordkammer) und Rect 7 (Südkammer)**, direkt östlich an Rect 1
angrenzend — gehören in der Zonentabelle NIEMANDEM und bleiben darum schwarz
(UNVISITED). Der Nutzer sieht seinen Marker am Rand des roten Nachbar-Rechtecks
schwimmen, während „sein" Raum ungezeichnet bleibt. Korrektur: **flip_x 0 → 1,
ox 84 → 244, Raum in ZWEI Zonen teilen (Rect 8 + Rect 7)**. Die (0,1)-Kombination des
Altbefunds war also **halb richtig** (flip_z=1 stimmt), flip_x ist falsch — und der
2-Trigger-Fit KONNTE das nicht sehen, weil beide Türen auf einer Senkrechten liegen
(Spiegel-Pivot = Türwand).

---

## 1. IST-Lage, exakt vermessen

### 1.1 Heutige Zeile reproduziert den Nutzer-Marker

Zeile 12 in `re15_port/engine/src/re15_map_zones.h`:
`{ 0x1010, -4400,-7600, 6600,10350,  2, 1, 0, 2,  84, 20, 2287, 2319, 0, 1, 0, 0 }`
(page 2, rect 1, ox/oy 84/20, sx/sy 2287/2319, flip (0,1)).

Projektionsformel (Original FUN_800473f8 @0x8004741c–0x80047528, Port
`re15_map_zones.c:276–292`):
```
t  = ((x+32000)*10*sx) >> 20 ;  t2 = ((z+32000)*10*sy) >> 20
px = (flip_x ? -((t+5)/10) : (t+5)/10) + ox
py = (flip_z ?  ((t2+5)/10) : -((t2+5)/10)) + oy
```
Nutzer-Position (3650, −3950) → **(162, 82)**. Im F9-Bild steht der Marker bei
Blatt-Pixel ~(160–162, 80–82) → **IST-Lage reproduziert** (Marker = 8×8-Quad um den
Punkt).

Rot gezeichnete Fläche im F9 (Füllfarbe (96,40,32)-Cluster, pro Blattzeile gemessen):
x 109..163 für y 77..109, x 109..144 für y 110..125 — das ist **exakt die gemalte
Fläche der Rect-1-Kachel** (s. §2). Rect 1 ist rot, weil 1010 (aktueller Raum) heute
Rect 1 zugeordnet ist.

### 1.2 Türen — beide RDTs selbst geparst (Door_aot_set 0x3B)

`re15_port/shared_assets/PSX/STAGE1/ROOM1010.RDT` / `ROOM1020.RDT`, Parser-Logik wie
`tools/gen_map_zones.py read_rdt` (Payload: rect `<hhhh>@+6`, spawn `<hhh>@+14`,
stg/rmd @+22/23):

| Tür | 1010-Seite (slot, Rect, Mitte) | 1020-Seite (slot, Rect, Mitte) | Spawn in 1010 | Spawn in 1020 |
|---|---|---|---|---|
| **A (Nord)** | slot 1, (4150,−4950) 1000×2000, Mitte (4650,−3950) | slot 0, (−3500,−8400) 1500×2300, Mitte (−2750,−7250) | (3650,0,−3950) | (−3785,0,−7412) |
| **B (Süd)** | slot 0, (4150,5900) 1000×2000, Mitte (4650,6900) | slot 1, (−3700,−19600) 1500×2300, Mitte (−2950,−18450) | (3650,0,6900) | (−4000,0,−18000) |

Der Nutzer stand **exakt auf dem Spawn von Tür A** — er war gerade durchgegangen.

### 1.3 Anker: 1020s AUSGELIEFERTE Zeile (@0x800768b0-Familie; ox/oy 100/136, s 2287/2287, flip 0,0)

| Punkt | Karte |
|---|---|
| Tür A, 1020-Mitte (−2750,−7250) | **(164, 82)** |
| Tür B, 1020-Mitte (−2950,−18450) | **(163, 106)** |

Heutige 1010-Zeile, 1010-Seiten: Tür A → (164,82), Tür B → (164,106). **Beide Türen
passen pixelgenau — trotz falschem flip_x.** Grund: beide Türen liegen auf EINER
Senkrechten (lokal x=4650 → Kartenspalte 164). Der x-Spiegel pivotiert exakt um diese
Türwand; die Türprojektionen sind unter beiden flip_x-Werten identisch, nur das
**Rauminnere** kippt auf die andere Seite. Ein Fit auf 2 kollineare Trigger ist in
dieser Achse **unterbestimmt** — der Generator hat die falsche Münzseite gewählt
(Memory-Klasse „Münzwurf gehört dem Löser").

### 1.4 Wohin das Rauminnere gehört — SCA-Beweis

ROOM1010.RDT SCA (10 eindeutige solide Rechteck-Wandzellen, 5× dupliziert):
Außenwände x 4600..6600 (OST — **hier sitzen beide Türen**), x −4400..−2400 (West),
z −7600..−5600 (Nord), z 8350..10350 (Süd), sowie ein **Raumteiler voller Breite
z 1450..1550 (x −4400..6600)**. Begehbarer Kern: **x −2400..4600, z −5600..8350**,
durch den Teiler in **zwei nicht verbundene Kammern** getrennt (beide Türen führen
nach 1020; man wechselt die Kammer NUR über 1020).

Vom Türdurchgang ins Rauminnere heißt lokal −x (Spawn 3650 < Türmitte 4650). Auf der
Karte liegt das Innere ÖSTLICH der Türspalte 164 (Rect 8/7, s. §2) → lokal +x muss
auf Karten-−x gehen → **flip_x = 1**. Heute (flip_x=0) läuft der Marker vom
Türdurchgang aus nach WESTEN in 1020s Zeichnung hinein.

## 2. Die Kunst: Blatt 2 = MAP03.PIX (nicht MAP02!), Rect 1 zeigt NUR 1020

⛔ Aufgaben-Kontext nannte MAP02 — **off-by-one**: CD-Datei-id-Tabelle u16
@0x80074c4c = {12,13,14,…}, Blatt/page 2 → id **14** → `DATA/MAP03.PIX`
(id−11; Loader `platform/pc/src/inv_render_pc.c:344–367`). MAP02 = Blatt 1 = „B2".
MAP03 trägt „POLICE STATION 1F" — deckungsgleich mit dem F9-Titel.

Rect-Tabelle Blatt 2 (Paar @0x80076840+16 → count 11, Liste @0x8007636c, 12-B-Einträge
{s16 x,y,w,h; u8 u@+8,v@+10}; PSX.EXE Datei-Offset = addr−0x80010000+0x800):

| Rect | Blatt (x,y) w×h | uv | gemalte Fläche (Index≠0) |
|---|---|---|---|
| 1 | (109,77) 56×56 | (32,32) | L-Form x109..163/y77..109 + Arm x109..144/y110..125; **Türschwenk x159..163, y80..84 (= Tür A)** |
| 8 | (164,77) 24×24 | (128,40) | Kasten **x164..179, y77..93** (nur 16 von 24 Spalten bemalt) |
| 7 | (164,93) 16×24 | (152,16) | Kasten **x164..179, y93..109**; **Türschwenk x165..169, y102..106 (= Tür B)** |

**Rect 8 + Rect 7 = ROOM1010:** gemeinsame Silhouette x164..179 / y77..109 — der
begehbare Kern 7000×13950 Einheiten ergibt bei s=2287/2319 genau 15,3×30,9 px; die
gemalte Trennzeile y93 zwischen beiden Kacheln = der SCA-Teiler z1450..1550
(projiziert y94). Türschwenke: Tür A in Rect 1 bei y80..84 (öffnet nach 1020), Tür B
in Rect 7 bei y102..106 (öffnet nach 1010-Süd) — beide auf der gemeinsamen Wandspalte
163/164.

**Quantitativer Beweis** (begehbare 500er-Gitterpunkte, Anteil auf gemalter Fläche):

| Projektion | Rect 1 | Rect 8 | Rect 7 |
|---|---|---|---|
| 1020, ausgelieferte Zeile | **0,77** | 0,01 | 0,01 |
| 1010 HEUTE (84,20,·,·,0,1) | **1,00** | 0,00 | 0,00 |
| 1010 KORRIGIERT (244,20,·,·,1,1) | 0,00 | **Nord: 1,00** | **Süd: 1,00** |

Antwort auf Frage 2: Rect 1 deckt **nur ROOM1020**. „Der Raum wird nicht gezeichnet"
ist **KEIN Malflächen-Loch, sondern ein Projektions-/Zuordnungsfehler**: 1010s eigene
Kacheln (Rect 8/7) existieren, sind aber keiner Zone zugeordnet → im
RE2-Aufdeck-System des Ports bleiben sie UNVISITED/schwarz, während 1010s Zone
fälschlich Rect 1 rot färbt.

## 3. Korrekte Werte (alle Anker geprüft)

Zeile **(ox,oy,sx,sy,flip) = (244, 20, 2287, 2319, 1, 1)**; ox aus der
Spiegelrelation ox′ = 2·164 − 84 = 244 (Pivot = Türspalte).

| Anker | korrigiert | Soll | Beleg |
|---|---|---|---|
| Ostwand-Innenface x=4600 | Spalte 164 | Westwand Rect 8/7 (x164) | Kachel-Dump §2 |
| Westwand-Innenface x=−2400 | Spalte 179 | Ostwand Rect 8/7 (x179) | Kachel-Dump §2 |
| Tür A Mitte (4650,−3950) | (164,82) | 1020-Seite: (164,82) | §1.3 |
| Tür B Mitte (4650,6900) | (164,106) | 1020-Seite: (163,106) | §1.3, 1 px |
| Nordwand z=−5600 / Südwand z=8350 | y78 / y109 | Rect-8-Innenrand / Rect-7-Südwand | Kachel-Dump |
| Teiler z=1500 | y94 | Kachelgrenze Rect 8/7 (y93/94) | Kachel-Dump |
| Nutzer (3650,−3950) | **(166,82)** | Nordkammer-Innenfläche x165..178 | §2 |

## 4. Türmarken page 2 rect 1 (Frage 3)

`s_map_marks` (re15_map_zones.h:278f): `{2,1,163, 81, 3, 2,3,1}` und
`{2,1,163,106, 3, 2,3,1}`. **Die POSITIONEN sind richtig und bleiben:**
(163,81) = Tür A (Nord; Anker (164,82), Schwenk y80..84), (163,106) = Tür B (Süd;
Anker (163,106), Schwenk y102..106). Der Altbefund „mit falscher 1010-Zeile gesetzt"
trifft die heutigen Werte nicht mehr — die Türen sind vom Spiegel unberührt (Pivot).
**Zu ändern ist nur die zid-Bindung:** nach der Zonen-Teilung gehört (163,81) zur
Nordzone (zid 2 bleibt), (163,106) zur **neuen Südzone** (zid 100), damit das
„Zone gesehen"-Gating der Marke der richtigen Kammer folgt. `zid2=3` (1020) und
`auf_partner=1` bleiben (Marke liegt auf Rect 1s gemalter Wandspalte).

## 5. Plan mit fertigen Werten

### 5.1 Sofortfix `re15_port/engine/src/re15_map_zones.h`

Zeilen 12–13 (1010/1011, heute rect 1) ERSETZEN durch vier Zeilen
(Muster ROOM1170 idx 0/1; Besucht-Bit hängt an Raum+idx, `zone_bit`
re15_map_zones.c:77–89 — Einfügen ist Save-sicher; neue zid = 100 = max(99)+1):

```c
{ 0x1010,  -4400,  -7600,   6600,   1550,  2,   8, 0,   2,   244,    20,  2287,  2319, 1, 1,   0, 0 },
{ 0x1011,  -4400,  -7600,   6600,   1550,  2,   8, 0,   2,   244,    20,  2287,  2319, 1, 1,   0, 0 },
{ 0x1010,  -4400,   1450,   6600,  10350,  2,   7, 1, 100,   244,    20,  2287,  2319, 1, 1,   0, 0 },
{ 0x1011,  -4400,   1450,   6600,  10350,  2,   7, 1, 100,   244,    20,  2287,  2319, 1, 1,   0, 0 },
```
(Bbox-Schnitt am Teiler z1450/1550; Überlapp 100 Einheiten ist unbegehbar, die
kleinere Zone gewinnt in `zone_index_at`.)

Marken-Zeile 279: `{ 2, 1, 163, 106, 3, 2, 3, 1 }` → `{ 2, 1, 163, 106, 3, 100, 3, 1 }`.

`s_map_floors` braucht KEINE Einträge (1010 ist einbändig). Wirkung: Spieler in 1010
färbt Rect 8 bzw. 7 rot, Rect 1 bleibt grün (1020 besucht); Marker (166,82) statt
(162,82) — östlich der Tür, in der eigenen Zeichnung.

### 5.2 Dauerhaft `re15_port/tools/gen_map_zones.py`

1. **Zonen-Teilung 1010** (Tabelleneintrag analog ZONE_FIX, mit Beleg): Teiler-Wand
   voller Breite z1450..1550 im SCA + ZWEI getrennte Original-Kacheln (Rect 8/7).
   Abgrenzung zur 1110-Lektion („Innenwände trennen keine Zonen"): dort ist die
   Kachel EIN Block — hier hat der Künstler beide Kammern separat gemalt; das
   Teilungs-Gate ist also „beide Seiten treffen VERSCHIEDENE gemalte Rects".
2. **ZONE_FIX**: `(0x1010,0): (2,8)`, `(0x1010,1): (2,7)` — Beleg: Deckungstabelle §2
   (1,00/1,00), Türschwenk Tür B in Rect 7s Kachel.
3. **ZONE_ORIENT**: `(0x1010,0): (1,1)`, `(0x1010,1): (1,1)` — Beleg §1.3/§1.4.
4. **Allgemeine Flip-Erkennung für hergeleitete Zeilen**: sind alle Fit-Anker
   KOLLINEAR (hier: beide Türen auf Kartenspalte 164), ist der Spiegel um diese
   Linie residuenfrei — Fit darf flip nicht selbst wählen. Tie-Break durch dritte
   Beobachtung: begehbarer Schwerpunkt muss auf die GEGENSEITE der Türlinie vom
   Nachbarn fallen (bzw. maximale Deckung mit gemalten Kacheln, die der Nachbar
   nicht belegt — Messgröße wie §2). ox nach Flip-Wahl per Spiegelrelation
   ox′ = 2·P − ox (P = Ankerspalte) neu setzen.

### 5.3 Nebenbefunde (nicht dieser Fix)

- **Gast-Zeile ROOM10F0 auf (2, rect 8)** (re15_map_zones.h:212f, s_map_floors
  Band 1 → 2/8) + Marke `{2,8,171,77,0,15,255,0}`: der 3F-Funkraum wohnt auf 1010s
  Nord-Kachel. Rechteck-Teilen ist engine-legal (current > visited), aber die
  Zuordnung ist mutmaßlich ein Löser-Ausweichprodukt („Rect 8 war frei") und gehört
  nach dem Fix neu gemessen.
- `re15_map_zeilen.h` Slot 1 (84,167,2287,2287): flip-lose Messschienen-Zeile
  (nur RE15_KARTENZEILE=1, re15_inv_screen.c:711ff) — kann 1010s gespiegelten Frame
  prinzipiell nicht ausdrücken; bei Nutzung der Schiene für 1010 blind.

## Messwerkzeuge dieser Runde

Alle Zahlen aus: PSX.EXE-Tabellen direkt (`@0x80074c4c`, `@0x80076840`,
`@0x8007636c`), MAP03.PIX-Nibbles (4bpp, 128 B/Zeile), RDT-Parser aus
gen_map_zones.py (importiert, nicht nachgebaut), Projektionsformel 1:1 aus
re15_map_zones.c, F9-BMP pixelvermessen (Füllfarb-Cluster).
