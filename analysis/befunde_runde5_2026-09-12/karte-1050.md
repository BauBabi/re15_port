# Karte 1F ab ROOM1050: „grosser brauner Block, Tueren schweben im Blau" — Befund 2026-09-12

Nutzer-Report: *„bis ROOM1050 sah die Karte von 1F richtig klasse aus. Ab 1050 nicht
mehr."* F9-Abzug `re15_port/build/platform/pc/befund_1050_F620_marke2.bmp`
(960x720 = 3x Blatt 2): Westfluegel gruen und korrekt; oestlich ein grosser brauner
Block ohne Innenwaende, Marker mittig; unten rechts zwei gelbe Tuer-Striche im Blau.

## Fazit vorweg

**ROOM1050 haengt am FALSCHEN Rechteck und seine hergeleitete Zeile ist doppelt
gespiegelt falsch.** Der Raum (der lange Ost-Korridor um die Haupthalle, 6 Tueren)
liegt heute auf Blatt 2 **Rect 4** — der grossen Hallen-Kachel, die er sich mit
ROOM1000 (z0+z1) und ROOM1030 teilt. Steht der Spieler in 1050, faerbt
`re15_map_rect_state` das GANZE Rect 4 CURRENT: die ganze Hallen-Kunst bekommt den
roten Schleier ueber dem Besucht-Gruen = der braune Block. Die EIGENE Zeichnung des
Raums — **Rect 0 = (180,69) 32x96, uv(0,32), der gemalte Ost-Korridor-Streifen** —
gehoert in der Zonentabelle NIEMANDEM (UNMAPPED) und wird darum NIE gezeichnet.
Zwei seiner Tuermarken (Zeilen y=188) sind Klemm-Anschlaege: die hergeleitete Zeile
(ox 63, oy 247, flip 0,0) projiziert die Sued-Tueren auf y=208/229, der Generator
klemmt an die Rect-Unterkante 188 — sie schweben unter der gemalten Kunst im Blau.
Korrektur: **Rect 4 -> Rect 0, flip (0,0) -> (1,1), ox 63 -> 311, oy 247 -> 59**
(Zeichnung = Raum um 180 Grad gedreht; gemessen, s. §2). Gleiche Fehlerklasse wie
karte-1010: der Herleitungs-Fit hatte nur EIN Tuerpaar zu einem ausgelieferten
Nachbarn und konnte die Spiegelung nicht sehen (`gen_marker_zeilen.py:205 ff`,
Ordnungs-Signatur braucht ZWEI erstklassige Zeugen).

---

## 1. IST-Lage (Frage 1+2)

### 1.1 Blatt 2 (1F), Zeilen oestlich des Westfluegels — `re15_port/engine/src/re15_map_zones.h`

Rechtecke der Seite aus der Original-Tabelle @0x80076840+2*8 -> Liste @0x8007636C
(PSX.EXE, t_addr 0x80010000; selbst geparst), Zeile je Raum @0x800768b0:

| Raum | .h-Zeile | Rect (x,y,w,h) | zid | ox/oy sx/sy | flip | Zeile echt? |
|---|---|---|---|---|---|---|
| 1000 z0 | 8-9   | **4** (119,125,72,64) | 0 | 0/0 0/0 (Bbox-Streckung) | 0,0 | Stub @0x800768b0+0 |
| 1000 z1 | 10-11 | **4** | 1 | 91/263 2428/3095 | 0,0 | hergeleitet |
| 1010 N/S | 25-28 | 8 / 7 | 2/100 | 244/20 2287/2319 | 1,1 | hergeleitet (karte-1010) |
| 1030 | 31-32 | **4** | 4 | 108/194 2428/2229 | 0,0 | **AUSGELIEFERT** (@0x800768c8) |
| 1050 | 35-36 | **4** | 6 | **63/247** 2428/2229 | **0,0** | hergeleitet — **DEFEKT** |
| 1060 | 37-38 | 10 (119,134,24,24) | 7 | — | 1,1 | — |
| 1070 | 39-40 | 3 (145,109,56,40) | 8 | 79/205 2229/2088 | 0,0 | AUSGELIEFERT |
| 10A0 | 43-44 | 6 (180,88,24,24) | 10 | — | 0,0 | — |
| 10B0 | 45-46 | 5 (180,59,48,32) | 11 | — | 0,0 | — |
| (Rect 0) | — | **0 (180,69,32,96) — KEIN Besitzer, UNMAPPED** | | | | |

(ROOM1100/1110 aus der Aufgabenliste liegen NICHT auf Blatt 2: .h-Zeilen 55-58 =
Blatt 3 Rect 6 bzw. Rect 5. ROOM1090 hat auf 1F kein Rechteck, s_map_floors rect 255.)

Soll-Layout gezeichnet: `analysis/befunde_runde5_2026-09-12/karte_1050_layout.png`
(alle Kacheln wie das Original sie stapelt, ROT = 1050 mit heutiger Zeile, CYAN =
korrigiert auf Rect 0, orange = Tuerlagen, gelb = die zwei Klemm-Marken).

### 1.2 Warum der Block braun ist und keine Innenwaende hat

- Zustand ist PRO RECHTECK: `re15_map_zones.c:322 re15_map_rect_state` — Zonen 0, 1,
  4, 6 teilen Rect 4; `cur->zid == 6` (Spieler in 1050) => ganzes Rect CURRENT.
- CURRENT = Kunst im Besucht-Ton (40,144,40) + halbtransparenter roter Schleier
  FILLMASK (200,16,16, abe=1) NUR auf bemalten Texeln:
  `re15_inv_screen.c:2324-2334` — gruene Kachel + roter Schleier = BRAUN, Aussenrand
  (Wandfarbe 176,176,176 der Kunst) = GELBLICH. Exakt der Nutzer-Befund.
- Innenwaende: die Kachel von Rect 4 (uv 184,56, 72x64, MAP03) ist EINE zusammen-
  haengende Flaeche ohne Innenlinien (Pixel-Zensus: nur Fill 0,72,0 + Rand
  176,176,176). Im ORIGINAL entstehen die Innenwaende des Ost-Komplexes dadurch,
  dass ALLE Rects der Seite immer gezeichnet werden (kein Besucht-Gate, Beleg im
  Code-Kommentar `re15_inv_screen.c:2247-2263`, Original @0x800472fc-0x800473e0 /
  @0x80047608) und Rect 0/3/5/6/7/8 ihre Umrisse UEBER die Hallen-Kachel legen.
  Der Port blendet Unbesuchtes aus (`re15_inv_screen.c:2244` UNVISITED -> continue)
  und ueberspringt UNMAPPED ganz (`:2267`) — Rect 0 (1050s eigene Zeichnung!) ist
  UNMAPPED und erscheint darum NIE, egal was besucht ist.
- `s_map_walls` (re15_map_zones.h:485) hat KEINE Blatt-2-Zeilen, `s_map_teile`
  (re15_map_zones.h:507) auch nicht — nichts zerteilt Rect 4.

### 1.3 Die zwei schwebenden Tuer-Striche

`re15_map_zones.h:320-321`:
`{ 2, 4, 177, 188, 3, 6, 255, 0 }` und `{ 2, 4, 183, 188, 0, 6, 0, 0 }`.
Projektion der 1050-Tueren mit der heutigen Zeile (Formel FUN_800473f8
@0x8004741c-0x80047528, Port re15_map_zones.c:276-292):
slot4 ->10A0 (17200,-13700) -> (176,**208**); slot5 ->1090 (19850,-23400) ->
(183,**229**). Rect 4 endet bei y=188 -> Klemmung (Generator to_map,
gen_map_zones.py:2790-2795) => beide Marken auf y=188. Die gemalte Kunst von Rect 4
endet dort laengst (letzte bemalte Zeile y=181, an x=177/183 sogar y=173, Pixel-
Zensus MAP03 uv 184,56) => Striche im Blau. y=188 ist der ANSCHLAG, kein Messwert.

## 2. RDT-Gegenprobe (Frage 3)

`shared_assets/PSX/STAGE1/ROOM1050.RDT`: 110 SCA-Zellen (100 solide Typ-1-Waende),
Bbox x 12700..26100, z -24350..15805; 6 Tueren (Door_aot_set 0x3B):
slot0 (21650,12350)->1030, slot1/2 (13700,9200/6100)->1000-z1,
slot3 (13700,-13450)->1000-z0, slot4 (17200,-13700)->10A0, slot5 (19850,-23400)->1090.
Begehbare Flaeche per Flutfuellung von den Tueren (16290 Rasterzellen a 100u):
West-Korridor + breites Nordende + breites Suedende
(`analysis/befunde_runde5_2026-09-12/room1050_reach_northup.png`).

**Formvergleich gegen die Kunst von Rect 0** (IoU der projizierten begehbaren
Flaeche gegen die bemalten Texel, Blatt-Massstab 2428/2229 wie 1030, freie Lage):

| flip (x,z) | bestes IoU | Lage |
|---|---|---|
| 0,0 (heute) | 0.339 | ox 91, oy 173 |
| 0,1 | 0.331 | ox 91, oy 56 |
| 1,0 | 0.507 | ox 312, oy 173 |
| **1,1** | **0.714** | **ox 312, oy 54** |

Tuer-Anker (der EINZIGE ausgelieferte Nachbar): 1030s Tuer nach 1050 projiziert
ueber 1030s Original-Zeile @0x800768c8 auf (187,153) — Marke steht auf (186,152),
re15_map_zones.h:322. Anker-Loesung fuer flip (1,1): **ox = 187+124 = 311,
oy = 153-94 = 59** (1 bzw. 5 px vom freien Form-Optimum: innerhalb des
5-px-Rauschbodens, analysis/karte_grundriss/BEFUND.md §29). Unabhaengige Zeugen:
- Tuer ->1030 (187,153) trifft die gemalte Tuernische der Rect-0-Kunst
  (Innen-Grau-Pixel bei (188..192,147..151), Pixel-Zensus uv(0,32)).
- Tuer ->10A0 landet auf (197,98); 10A0s EIGENE Ostwand-Marke steht auf (197,94)
  (re15_map_zones.h:327) — zwei unabhaengige Projektionen derselben Tuer, 4 px
  (Kriterium wie die ZONE_ORIENT-Eintraege 2026-09-07: <= 5 px).
- Tuer ->1090 landet auf (191,77) im breiten Nordkopf des Streifens.
- Raum-Spanne x 177..208 / y 75..161 gegen gemalt 180..211 / 69..164.
Mit der heutigen Zeile: Deckung der begehbaren Flaeche auf Rect-0-Kunst 0.046,
Tueren bei y bis 229 — 40+ px unter jeder Kunst. ROOM1090 als Alternativ-Besitzer
von Rect 0 gemessen und verworfen (bestes IoU 0.209 ueber alle 4 Spiegelungen).

## 3. PATCH-PLAN (nichts davon ausgefuehrt)

1. **`re15_port/engine/src/re15_map_zones.h:35-36`** (beide Szenario-Zeilen):
   ALT `{ 0x1050, 12700,-24350, 26100, 15805, 2, 4, 0, 6,  63, 247, 2428, 2229, 0, 0, 0, 0 }`
   NEU `{ 0x1050, 12700,-24350, 26100, 15805, 2, 0, 0, 6, 311,  59, 2428, 2229, 1, 1, 0, 0 }`
   (0x1051 identisch). Rect 4 -> **Rect 0**, ox/oy 63/247 -> **311/59**, flip -> **1,1**.
2. **Marken `re15_port/engine/src/re15_map_zones.h`:**
   - Zeile 320 `{2,4,177,188,3,6,255,0}` (Klemm-Artefakt slot4) **streichen**;
     stattdessen Zeile 327 `{2,6,197,94,1,10,255,0}` -> `{2,6,197,94,1,10,6,1}`
     (dieselbe physische Tuer; der Punkt liegt auf bemalter Flaeche BEIDER Kacheln,
     Rect 6 UND Rect 0 — auf_partner-Bedingung erfuellt).
   - Zeile 321 `{2,4,183,188,0,6,0,0}` (slot5 ->1090) -> `{2,0,191,71,0,6,255,0}`
     (projiziert (191,77), Nordwand-Snap der Rect-0-Kunst y=69/70; exakter Wert aus
     snap_wall beim Regenerieren).
   - Zeilen 317-319 (169,159)/(169,167)/(176,173), zid2=6, auf_partner=1:
     auf_partner -> **0** (Zone 6 liegt kuenftig auf Rect 0; die Punkte liegen dort
     nicht auf deren Kunst — sonst kehrt „This door is flying" 2026-09-05 zurueck).
     Optional die 1050-Seiten dieser drei Tueren neu auf Rect 0:
     (205,147)/(205,140)/(205,98) kind 1 zid 6 (Ostkante; Generator-Snap massgeblich).
3. **Generator-Seeds, damit ein Regen das nicht rueckgaengig macht**
   (`re15_port/tools/gen_map_zones.py`): `ZONE_FIX` (ab Zeile 1236) +=
   `(0x1050, 0): (2, 0)`; `ZONE_ORIENT` (ab Zeile 1707) += `(0x1050, 0): (1, 1)`;
   Kommentar Zeile 1271-1277 („Rect 4 traegt ... und ROOM1050") anpassen.
   Mechanik-Luecke wie bei karte-1010 dokumentieren: 1 Tuerpaar =>
   Ordnungs-Signatur (gen_marker_zeilen.py:208 ff) blind fuer die Spiegelung.
4. **Danach bleibt Rect 4 = 1000-z0/z1 + 1030** (drei Zonen, EINE Kachel — die Kunst
   selbst hat dort keine Innenlinien, wie im Original). Wer den Verschmelz-Ton auch
   dort trennen will: `s_map_teile`-Zeilen (Mechanik re15_inv_screen.c:2339 ff,
   Praezedenz Blatt 3 Rect 5) — fuer 1030 (Kachel-Ausschnitt via Original-Zeile:
   x117..193/y126..192) und 1000-z1 (149..173/138..190) messbar, fuer 1000-z0 NICHT
   (Stub-Zeile, eigene Projektion landet ausserhalb der Karte, s. Kommentar
   gen_map_zones.py:1269-1277) -> OFFEN, nicht raten.
5. Verifikation: `gen_map_zones.py`-Regen diffen (nur 1050-Zeilen + Marken), ctest
   (map-Suiten, bes. unit_map_durchgang/eingemessen: Paar 1050<->1030 = 1-2 px,
   1050<->10A0 = 4 px), dann F9-Lauf des Nutzers 1040 -> 1030 -> 1050.

Belege/Artefakte in `analysis/befunde_runde5_2026-09-12/`: karte_1050_layout.png
(Soll/Ist), room1050_reach_northup.png, tile_p2_r0.png, tile_p2_r4.png,
MAP03_unten_3x.png (Original-Zusammenbau untere Blatthaelfte),
befund_1050_F620_marke2.png (Nutzer-Abzug).
