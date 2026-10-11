# Runde 36 — Spur M "karte" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_karte`, Zweig `r36/karte`, Basis master 78a4a748 (geprueft: status leer,
log -1 = 78a4a748). Zuteilung (VERTRAG.md §0/§1.4): M = Karte, NUR Zonen/Tuermarken/Besucht-Latches
(N = Items + Freischaltung). Kein Bank-9-Bit, keine Nachrichten-ID, kein Ereignis noetig.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md Z. 61-66)

1. "Im Treppenhaus 10A0 auf Etage B1 fehlen die Treppen eingezeichnet."
2. "In ROOM 11D0 lande ich auf der Map direkt in der Mitte, sobald ich durch die Tuer gehe, statt am Tuereingang zu stehen..."
3. "Bei ROOM 1090 auf der Map, gibt es 2 Tueren, eine am Ende des Ganges, wo die Hunde durch die Luke kommen, und eine wo man die Weste bekommt. Das muss als 2 separate Bereich in der Map eingeblendet sein und sauber getrennt."
4. "Die Tuer von ROOM 11C0 ist auf der Map um 90° verdreht."
5. "Die kleinen Tuerbereiche von ROOM 1110 werden schon als besucht angezeigt, obwohl ich dort noch garnicht drin war"
6. "Die Fahrstuhl Etagen werden als besucht angezeigt, obwohl ich auf den verschiedenen Etagen noch nicht im Fahrstuhl war."

## Werkzeug

* Mess-Sonde `re15_port/tests/unit/test_r36_karte.c` (Ziel `probe_r36_karte`, registriert in
  `tests/unit/probes/r36_karte.cmake`), Teil `messung`: faehrt den echten Kartenpfad wie
  `test_r35_karte.c` (RDT laden + `scd_room_reenter` -> `re15_map_zone_update`, Kartenschirm mit
  Seiten-Setzer je Stage, Zustand je Rechteck/Teil, Marken, Spieler-Marker).
* Analyse-Skripte (Python, aus `re15_port/tools/gen_map_zones.py` read_rdt/zones_of):
  `analysis/befunde_runde36/M_karte_werkzeug/` — `rdt_info.py` (Tueren/Treppen/SCA je Raum),
  `flut.py` (Flutfuellung des begehbaren Komplements im Port-Kollisionsmodell: Zellen mit
  `(floor>>4)==Band` und `u0&1` sind Waende, re15_collision.c; Spielerradius 450 = DAT_80073e94[6]),
  `rects.py` (Rechteck-Tabellen @0x80076840), `kachel.py` (Kachel-Indizes aus DATA/MAP0x.PIX),
  `re2_areas.py` / `re2_area_bild.py` (RE2-Kartenraeume @0x800aaa38 + MAPS.PIX), `sca_bild.py`,
  `jalscan.py` (jal-Aufrufer), `lauf.sh` (echte exe-Kopie: Spielstand + LOAD + MAP-Abzug).
* Echtlauf-Abzug: `RE15_MAP_SHOT_SWEEP` (alle 13 Blaetter, aufgedeckt) mit `RE15_WINDOW_SCALE=1`,
  Spielstand aus `probe_r35_karte karte`. Bild: `M_karte_bilder/vorher_B1_alles_aufgedeckt_x3.png`.

## Messung vorher (Stand 78a4a748, gebaut im Baum, `probe_r36_karte messung`, 2026-10-11)

```
=== Punkt 1: Treppenhaus ROOM10A0 auf B1 (Blatt 0) ===
  10A0 B1 (Band 4)  ROOM10A0 (21200,25500): Zone Blatt 2 rect 6 zid 10 | gezeigt Blatt 0 | aktuell: 6 | Marker (157,119)
    Blatt 0 rect 6 Zustand AKTUELL; Treppenmarken (sichtbar) auf Blatt 0: 0
      Marke #32 Blatt 2 rect 6 (187,100) kind 4 / #33 (196,99) kind 4      <- 1F: 2 Treppen
      Marke #19 Blatt 1 rect 9 (177,115) kind 4 / #20 (186,112) kind 4     <- B2: 2 Treppen
=== Punkt 2: ROOM11D0 durch die Tuer ===
  1230 an der 11D0-Tuer  ROOM1230 (-4837,-16839): Blatt 0 rect 0 zid 108 | Marker (130,123)
  11D0 Ankunft           ROOM11D0 (  -300,-17400): Blatt 0 rect 5 zid 30 | Marker (129,151)   <- MITTE
  11D0 im Tuer-Trigger   ROOM11D0 (  -300,-16700): Marker (129,148)
  11D0 an der 1260-Tuer  ROOM11D0 (  2600,-22000): Marker (137,171)
    Tuermarken Blatt 0 rect 5: (129,124) kind 0 zid 30/108 | (137,172) kind 2 zid 30
=== Punkt 3: ROOM1190 (Hunde/Luke + Weste) ===
  1190 aus Gang (Hunde)  ROOM1190 ( 3174,-11182): Zone Blatt 0 rect 2 zid 26 | aktuell: 2 | Marker (148,81)
    nach NUR Haupthalle: rect 1 unbesucht, rect 2 AKTUELL
  1190 aus Gang (Weste)  ROOM1190 (19515,-12109): Zone Blatt 0 rect 2 zid 26 | aktuell: 2 | Marker (156,83)
    nach NUR Ostkammer:  rect 1 unbesucht, rect 2 AKTUELL
  11A0 (Tuer 2070)       ROOM11A0 ( 6500,-3850): Zone Blatt 0 rect 1 zid 27 | gezeigt Blatt 6 | aktuell: 2
  11A0 Band 3            ROOM11A0 ( 7500,10650): Zone Blatt 0 rect 1 zid 27 | gezeigt Blatt 0 | aktuell: 1
=== Punkt 4: Tuer ROOM11B0 <-> ROOM11C0 auf B1 ===
    Marke #3 rect 3 (172,100) kind 1 = SENKRECHTER Balken
    gemaltes Original (symbolkatalog.csv): Blatt 0 rect 4 (171,101) 5x5 Wand N waagerecht
=== Punkt 5: ROOM1110 Teilbereiche ===
  1110 Ankunft aus 1100  ROOM1110 (-2400,-5000): Blatt 3 rect 5 zid 17
    Teil 0 (146,114 42x31): AKTUELL | Teil 1 (189,122 24x11): besucht | Teil 2 (189,134 24x11): besucht
=== Punkt 6: Fahrstuhl-Etagen ===
    1080 aus 1F (1040): 1F rect 9 AKTUELL | 2F rect 4 besucht | 3F rect 0 besucht
    danach in 1120 (3F): 3F-Kabine rect 0 besucht, 2F-Kabine rect 4 besucht
    4020 aus 4000 (Blatt 8): Blatt 8 rect 2 AKTUELL | Blatt 9 rect 13 besucht | Blatt 10 rect 3 besucht
```
Alle sechs Befunde reproduziert. Bildpunkt Punkt 4 im Sweep-Abzug (Blatt 0): Wandzeile y=100
(x 168..181 = (176,176,176)), der gelbe Balken (224,168,40) steht SENKRECHT bei x=172, y 98..102.

## RE-Belege (Adressen, Bytes)

### R0 Welche Raeume gemeint sind (Punkt 3: "ROOM 1090" = ROOM1190)
* ROOM1090.RDT hat KEINE Hunde und KEINE Weste: 2 Tueren (slot 0 Band 1 -> ROOM1050 @0x0211A,
  slot 1 Band 6 -> ROOM1100 @0x0213A), `Sce_em_set` nur Typ 0x26 (Feuer, @0x02214..0x0228C) und
  0x42 (@0x022CC).
* ROOM1190.RDT: Hunde Typ 0x20 `Sce_em_set` @0x02900/0x02914/0x02928 (sub13), Luke = Obj 4
  `Obj_model_set` @0x02A06 bei (310,-4500,-26946) (sub13; R35 H_raeume.md: "sub11 dreht die Luke
  (obj 4)"); Weste: sub14 `Aot_set` slot 16 sce 3 @0x02D90 (19700,-22000,1000,1500) -> sub15 @0x02DA6
  `Set 22 03 75 01` @0x02DD0 (Flag (3,0x75) = Weste an, analysis/nutzer_batch_2026-08-29/weste-effekt.md).
* ROOM1190 ist ZWEI getrennte Bereiche (flut.py, Port-Kollisionsmodell):
  Spawn der Gang-Tuer (1180/1230 slot 2 -> (3174,-11182)) -> x -20804..3996, z -26004..-10904;
  Spawn der Ostkammer-Tuer (ROOM1230 slot 4 sce 2 @0x00CE6 bzw. slot 6 sce 0 @0x00BC4 + Aot_reset
  sce 2 @0x01450; in ROOM1180 nur slot 4 sce 0 @0x00A94 = zu) -> (19515,0,-12109) -> x 14496..20796,
  z -25004..-12004; `verbunden: False`. Haupthalle = Tuer slot 4 (1900,-10400,2200,1500) @0x028B2
  (Gang-Zweig-Ende, "am Ende des Ganges"), Ostkammer = Tuer slot 6 (18200,-12300,2600,1500) @0x02B1C.
  Elzas ROOM1191: identische Tueren und Flutergebnis.

### R-Grundlage: was RE1.5 und RE2 an der Karte ueberhaupt fuehren
* RE1.5 (info/Re1.5/PSX.EXE) hat KEIN Besucht, KEINE Tuer-/Treppenmarken und fuer 39 von 72
  Raeumen nur die Stub-Massstabszeile {0,0,1,1} @0x800768b0+8*Idx (Memory karte-massstabstabelle);
  der Zeichner FUN_800473f8 malt jedes Rechteck der Seite ohne Zustand. Alle sechs Punkte betreffen
  also Port-Ergaenzungen nach RE2-Vorbild (Beta -> Retail).
* RE2 Retail (info/re2leon/PSX.EXE, selbst disassembliert mit re2_disasm.py):
  - BESUCHT wird beim BETRETEN eines RAUMS gesetzt: Raumaufbau FUN_80049e48 ruft als letzten
    Schritt `jal 0x8006931c` @0x8004a39c (jalscan: einziger Aufrufer). FUN_8006931c: Stage-Basis
    per Sprungtabelle @0x80011b2c (`sltiu v0,a0,0x7` @0x80069330; 30/58/72/89/99/123),
    `addiu a0,s0,238` @0x80069390 (= 0x800d490c Bank 9), `jal 0x8007730c` (SET) @0x80069394 mit
    Basis+Raum; danach Bereich-Bit Bank 35 (`addiu a0,s0,234` @0x800693b0, jal @0x800693b4).
  - Der Zeichner FUN_8006e120 prueft JE KACHEL: `lbu a1,12(s1)` (Kachel +0x0C = Besucht-Bit)
    @0x8006e67c / @0x8006e730, Test Bank 9 `jal 0x80077360` @0x8006e688 / @0x8006e73c, ohne Karte
    `beq v0,zero,0x8006e768` @0x8006e744 = unbesuchte Kachel wird NICHT gezeichnet; aktueller
    Raum `bne s2,a3` @0x8006e640 -> `addiu s5,s5,1` @0x8006e648 (CLUT 501 -> 502).
  - Raumkacheln @0x800aaa38 (20 Bereiche x {ptr,count,...}, 16 B je Kachel): ein Raum, der auf
    mehreren Blaettern gezeichnet ist, traegt auf allen DASSELBE Bit - ROOM601 auf Area 13/16/18
    (vis 100), ROOM505 auf Area 10/12/13/16 (vis 94), ROOM400/503/703/704 auf je zwei
    (re2_areas.py). RE2 wuerde so eine Kabine also auf allen Etagen als besucht zeigen.
  - Treppen: RE2 malt sie in die Kachel JEDES Etagenblatts (MAPS.PIX; Area 2 RPD 1F Kachel 7 vis
    37 und Kachel 17 vis 47, Area 3 RPD 2F Kachel 6 vis 16 und Kachel 13 vis 20 - Bilder
    `M_karte_bilder/re2_area2_RPD1F.png`, `re2_area3_RPD2F.png`). Tueren sind gelbe Punkte IN der
    Kachel (map-re2-system.md §5.1), liegen also in der Wand.

### R1 Treppen 10A0 B1
* ROOM10A0.RDT Treppen-Zonen (Aot_set sce 13 = Achse Z, Handler LAB_800435cc liest z
  @0x800435dc-e0; Band = pc[4]): @0x00E40 slot 3 Band 8 / @0x00E54 slot 4 Band 6 (Lauf 8<->6,
  Mitte (21450,21650)), @0x00E68 slot 5 Band 6 / @0x00E7C slot 6 Band 4 (Lauf 6<->4, (25350,22350)),
  @0x00E90 slot 7 Band 4 / @0x00EA4 slot 8 Band 3 (Lauf 4<->3, (21450,20650)), @0x00EB8 slot 9
  Band 3 / @0x00ECC slot 10 Band 1 (Lauf 3<->1, (25350,22350)).
* Port-Regel (gen_map_zones.py, "jedes Treppenende auf das Blatt SEINES Bandes", Glyph kind 4 fuer
  sce 13): 1F (Band 8/6) und B2 (Band 4/3/1 - der Generator kannte Band 4 -> B1 nicht) tragen je
  zwei Marken; die B1-Zeile (Blatt 0 rect 6, Band 4) hat Runde 35 Spur G von Hand angelegt, ohne
  Marken. B1 bekommt die beiden Band-4-Enden @0x00E7C/@0x00E90, projiziert mit der B1-Abbildung.
* B1-Kachel MAP01.PIX uv(128,16) 16x24: Wand Zeile v16/v36 und Spalte 0/15 (Index 4), Innen Index
  1 in x154..167 y115..133 (kachel.py).
* Herkunft (statt Bisect - es ist keine Regression einer vorhandenen Treppe): bis v0.8.22 zeigte der
  B1-Absatz (Band 4) das B2-Blatt (naechstes Band der Etagentabelle: |4-1| < |4-8|) MIT dessen zwei
  Treppen; seit 0549cfae (Runde 35 Spur G, in v0.8.23) steht er richtig auf B1 - aber ohne Marken.
* Die uebrigen fuenf Befunde bestehen seit Einfuehrung der jeweiligen Port-Ergaenzung (git log -S):
  11D0-Kasten bis z -10950 seit dem Zonenmodell 70683f1d (v0.3.43); 1190 als EINE Zone auf Blatt 0
  rect 2 seit der Kunst-Umstellung f77126ff (v0.5.4); Marke (172,100) kind 1 seit 4a3c983b (v0.7.32);
  Teilbereiche mit Zonen-Bit seit 2026-09-07; gemeinsame Kabinen-Etagen-Bits seit Runde 30 (v9) -
  keine Regression aus v0.8.21..v0.8.23, deshalb kein Bisect.

### R2 ROOM11D0
* ROOM11D0.RDT SCA (Band 0, Zellen @0x089C..): Tuerwand zum Gang @0x08CC (x -2200..7750,
  z -16500..-15500), Suedwand @0x08A8 (x -9000..7750, z -24050..-23050), Ostwand @0x089C
  (x 6750..7750); Zwinger-Block bis z -10950 (@0x08D8). Gang-Tuer Trigger (-1300,-17000,2000,1000),
  Spawn der Gang-Seite in 11D0 (-300,-17400) (ROOM1180 @0x00A12 / ROOM1230 @0x00B12, nach
  Ck(4,243) @0x00AEA).
* Kachel MAP01.PIX uv(128,56): ein LEERER Kasten (kachel.py) - Waende Zeile v56 / v104 = y124 /
  y172, Spalte u128 / u181 = x96 / x149, keine gemalte Tuer. Stadt-Zeile @0x80076998 = {0,0,1,1}.
* Ursache des Befunds: Bbox-Streckung (re15_map_zone_marker) der SCA-HUELLE z -24050..-10950; die
  Tuerwand liegt bei 57 % der Hoehe -> Mitte.

### R3 ROOM1190 / ROOM11A0
* Rechtecke Blatt 0 @0x800762a0 (rects.py): r1 (76,76,72,48) uv(0,32), r2 (144,69,16,40) uv(152,16).
  r2-Kachel hat die einzige Tuernische oben rechts (MAP01.PIX uv(160..164,17..21) = (152..156,70..74));
  die Gang-Kachel r0 traegt die Nische der Haupthallen-Tuer (137..141,71..75) ueber r1s Oberkante y76.
* Ausgelieferte 1190-Zeile @0x80076978 = (75,127,1920,2301) (bytes `4b 00 7f 00 80 07 fd 08`):
  Haupthallen-Innenwaende (SCA @0x1C70 x-24536, @0x1C34 x4624, @0x1C7C z-26498, @0x1C28 z-10280)
  -> x89..142 y79..115 = rect 1; Ostkammer -> x159..174 (liegt NICHT auf ihrer Kachel).
* ROOM11A0: Zeile @0x80076980 = (138,113,1776,2048) -> y -2..62, ueber allen B1-Rechtecken; Tueren
  nur nach 2070/20A0/3000 + Selbst-Tueren (rdt_info.py) - keine zum B1. Seine Kachel: Blatt 6
  ("DRAINS B2") rect 2 (90,58,48,48), dort stand er schon als Gast-Zeile.

### R4 Tuer 11B0 <-> 11C0
* Gemalte Original-Nische: MAP01.PIX rect-4-Kachel uv(184,16), Zeilen v17..21, Spalten u187..191
  ("4...4" mit Blatt), u187 v17 = oberes Nibble von Datei-Byte 0x8DD = 4 -> Schirm (171..175,
  101..105) in der NORDwand von 11C0; Katalog `analysis/kartensymbole/symbolkatalog.csv` "0,MAP01.PIX,
  4,...,171,101,...,N,waagerecht". Gemeinsame Wandzeile y100 (rect 3 Unterkante v104, rect 4
  Oberkante v16).
* 11B0-Zeile @0x80076988 = (159,110,2272,1954): 11B0-Tuer slot 1 Mitte (-25480,-28190) -> (173,103),
  Trigger x -26730..-24230 -> x170..176. 11C0-Zeile @0x80076990 = (174,196,1,1) = Stub.
* Ursache: die Generator-Wandachse kam aus dem 11B0-Trigger (2500x3500, in z gestreckt ->
  "senkrecht"); der 11C0-Trigger (4000x2700) und die gemalte Nische sagen waagerecht.

### R5 ROOM1110 Teilbereiche
* Eingang: einziger Tuer-Spawn von aussen ROOM1100 slot 1 @0x009BA (ROOM1101 ebenso) -> (-2400,-5000)
  = Teil 0 (Evidence Room); die kleinen Raeume NUR ueber Selbst-Tueren ROOM1110 slot 1 @0x00A6E ->
  (-4550,-650) (Teil 1) und slot 3 @0x00AAE -> (-4550,4350) (Teil 2) (Rohsuche aller Tuersaetze).
  ROOM5090 (zweiter Raum mit Teilen): Eingang ROOM6030 slot 3 @0x00C92 -> (27200,4900) = Teil 1.
* Ursache: teil_zustand (re15_map_zones.c) nahm fuer JEDEN Teil das Zonen-Bit des Raums.

### R6 Fahrstuhl
* Kabine 1080 auf Blatt 2/3/4, Kabine 4020 auf Blatt 8/9/10, alle Etagenzeilen Band 0
  (s_map_floors). re15_map_visited_mark_at setzte bei "gleichen Baendern" ALLE Etagen-Bits.
* Etage der Kabine (Runde 35 Spur G, re15_karte_fahrstuhl_blatt): Vorraum 1040/10C0/1120 bzw.
  Bits (3,54/55/56). NEU 4020: Vorraum ROOM4000 @0x0107A / ROOM5000 @0x008A4 / ROOM50C0 @0x0094A
  (Tuersaetze -> 4020 Spawn (-21400,0,-23400)); Blatt aus dem Seiten-Setzer: Stage 3 `ori v1,zero,
  0x8` @0x8004bdb4, ROOM5000 (Tabelle @0x800111b4 [0]) `ori v1,zero,0x9` @0x8004bf50, ROOM50C0
  ([12] -> 0x8004beb0 -> 0x8004bed8) `ori v1,zero,0xa` @0x8004bee4; Etagen-Bits des Spiels Bank 3
  0x93/0x94/0x95 (ROOM4000 @0x013CC `22 03 93 01`, ROOM5000 @0x008FC `22 03 94 01`, ROOM50C0
  @0x00A34 `22 03 95 01`, Leser ROOM4020 sub07 @0x00906 `21 03 93 00`).
* NUTZER-VORGABE gegen RE2: RE2 setzt EIN Bit je Raum (s.o., ROOM601 auf drei Blaettern). Der
  Nutzer verlangt "besucht" je Etage, auf der er im Fahrstuhl war. Uebertragen wird RE2s FORM
  (Bit beim Betreten setzen, je Kachel pruefen), Schluessel = (Kabine, Etage).
* "Auf der Etage im Fahrstuhl" heisst: dort EINGESTIEGEN oder dort mit ihm ANGEKOMMEN. Die Fahrt
  endet im Original mit einer Skript-Tuer aus der Kabine direkt in den Etagenraum (ROOM1080 main00
  @0x0482/@0x04A2/@0x04C2, Null-Rechteck, per Aot_on gefeuert; analysis/befunde_2026-09-26/
  fahrstuhl-faehrt-nicht.md §2.1), und der Etagenraum vermerkt die Kabine dann selbst auf seiner
  Etage (Bits 54/55/56 bzw. 0x93..0x95 unter dem Ankunfts-Cut). Port: Raumfolge Kabine ->
  Etagenraum (Vorraum-Spiegel FUN_8001d600 @0x8001d938, Runde 35) setzt das Bit dieser Etage.
  Laden ist keine Ankunft: der Spiegel wird beim Laden/Neustart geloescht (er steht auch im
  Original nicht im Speicherblock, Runde 35 G OFFEN 5).

## Umsetzung (Dateien, Konstanten)

| Punkt | Datei | Aenderung | Beleg |
|---|---|---|---|
| 1 | `engine/src/re15_map_zones.h` s_map_marks (am Ende angehaengt) | `{0,6,158,128,4,10,255,0}`, `{0,6,163,125,4,10,255,0}` | R1 @0x00E7C/@0x00E90 projiziert (unit_r36_karte_treppe misst die Projektion aus g_aot) |
| 2 | dto. s_map_zones 0x11D0/0x11D1 | wz1 -10950 -> **-16500** (Innenkante Tuerwand @0x08CC); wz0 bleibt -24050 (Aussenkante Suedwand @0x08A8 -> Innenkante -23050 faellt auf y172) | R2; PORT-ERGAENZUNG (Stub @0x80076998) |
| 3 | dto. s_map_zones 0x1190/0x1191 | EINE Zone rect 2 -> **idx0 Haupthalle rect 1** (Zeile @0x80076978 unveraendert, Kasten SCA-Innenkanten) + **idx1 Ostkammer rect 2, zid 110** (Kasten @0x1D00/@0x1CE8/@0x1D24/@0x1CDC) | R0, R3 |
| 3 | dto. 0x11A0/0x11A1 | Hauptzeile Blatt 0 rect 1 -> **Blatt 6 rect 2**, Gast-Zeilen Blatt 6 entfallen; s_map_floors Band 3 -> (6,2) statt (0,1) (an ihrem Platz, Pin: Zeile 48 = 50D0) | R3 |
| 3 | dto. s_map_marks | (0,1,116,116,..,27) entfaellt; (151,69) zid 26 -> **110**; (139,76) gepaart mit zid 26 (auf_partner 1) | R3 |
| 3 | `engine/src/re15_map_zones.c` s_etage_bit | Bit 26 `{0x11A0,0,3,6}` angehaengt (Bit 15 unbenutzt) | Pin unit_map_speichern_laden |
| 4 | `re15_map_zones.h` s_map_marks | (172,100) kind 1 -> **(173,100) kind 2** | R4 |
| 5 | NEU `engine/src/karte_r36.c` + `include/re15_karte_r36.h` | Bit-Tabelle je Teil (explizit, nur anhaengen), Bits 96..103 (Platz bis 111) im Etagen-Feld visited_floor (v9, kein neues Format), Eingangs-Flag | R5, RE2-Form R-Grundlage |
| 5 | `re15_map_zones.c` | `teil_drin`/`teile_merken` (Bit setzen, solange der Spieler in der Teil-Weltbox steht, +-250 wie AKTUELL); `teil_zustand`: besucht = Teil-Bit, Zonen-Bit nur fuer Eingang/ohne Tabellenzeile; reveal_page setzt Teil-Bits | R5 |
| 6 | `re15_map_zones.c` re15_map_visited_mark_at | "gleiche Baender"-Regel nur noch bei UNBEKANNTER Kabinen-Etage (`re15_karte_fahrstuhl_blatt(...) < 0`) | R6 |
| 6 | `engine/src/karte_fahrstuhl_1080.c` + Header | 4020: Vorraum 4000/5000/50C0 -> Blatt 8/9/10, sonst Bits (3,0x93/0x94/0x95) | R6 |
| 6 | `karte_fahrstuhl_1080.c` `re15_karte_fahrstuhl_ankunft` / `re15_karte_raum_vergessen` + Haken in re15_map_zone_update / re15_map_visited_reset / re15_map_visited_import | Ankunft mit dem Fahrstuhl (Raumfolge Kabine -> Etagenraum) setzt das Kabinen-Bit dieser Etage; Laden/Neustart loescht den Vorraum-Spiegel | R6 |
| 6 | `karte_r36.c` + `re15_map_zones.c` etage_bit() | Kabinen-Zeilen (1080 Blatt 2/3/4, 4020 Blatt 8/9/10) bekommen EIGENE Bits 112..117; die alten 3/4/5 und 17/18/19 sind unbenutzt. Grund: die alten wurden immer gemeinsam gesetzt und stehen so in den Spielstaenden - "auf allen Etagen eingestiegen" ist davon nicht zu unterscheiden; ein alter Stand zeigt die Kabine erst nach dem naechsten Einstieg auf der Etage (nichts geraten). Feldgroesse 128 (re15_map_floor_bit_kapazitaet wieder 128) | R6 |

Gemeinsame Dateien (VERTRAG §1.4): `re15_map_zones.h` nur Datenzeilen mit Kommentar "Runde 36 Spur M";
`re15_map_zones.c` ist Kartenmodul (M/N), Aenderungen als kommentierte Bloecke. scd_room_setup.c,
main.c etc. unberuehrt. Kein Patch an shared_assets.

Konflikte mit anderen Spuren (Stand 2026-10-11 04:1x, `git diff` aller r36/*-Zweige gegen die Basis):
keine andere Spur aendert re15_map_zones.c/.h, karte_fahrstuhl_1080.c, re15_map_owned.c oder die
beiden angepassten Pins. Spur N (karten-items, r36/kartenitems abd06d8e) hat bisher nur Dossier/
Messung. Beruehrungspunkt fuer N: wer eine Etagenkarte besitzt (re15_map_owned_page), sieht
UNBESUCHTE Rechtecke/Teile/Kabinen in RE2s Unbesucht-Zeile (CLUT 498, re15_inv_screen.c) - die
neuen Zustaende (Teil/Kabine unbesucht) fuegen sich dort ohne Aenderung ein. Bitbelegung in
visited_floor: Etagen-Tabelle 0..95, Teile 96..111, Kabinen 112..119 (re15_karte_r36.h) - falls N
Bits im Etagen-Feld braucht: 120..127 sind frei.

Angepasste fremde Pins (begruendet):
* `tests/unit/test_map_teilbereich.c` (2026-09-07): verlangte schon im Evidence Room "die anderen beiden
  sind GRUEN" = genau der gemeldete Fehler. Jetzt: Teil 0 (Eingang) besucht, Teil 1/2 erst nach Betreten.
* `tests/unit/test_r30_karte_nutzerstand.c` B4 ("alles begangen"): begeht jetzt zusaetzlich jeden Teil
  (Mitte seiner Weltbox) - ein Punkt je Zone traf nur einen Teil; eine Innenwand zwischen zwei
  unbesuchten Teilen wird richtig nicht gezeichnet.

## Messung nachher (`probe_r36_karte messung`, Stand 15fc56a5; Fahrstuhl-Ankunft s. Riegel 6 unten)

```
Punkt 1  10A0 B1: Treppenmarken (sichtbar) auf Blatt 0: 2 -> #176 (158,128) kind 4, #177 (163,125) kind 4
         B2 2 / 1F 2 unveraendert
Punkt 2  1230 an der 11D0-Tuer Marker (130,123) | 11D0 Ankunft (129,130) | im Trigger (129,128)
         an der 1260-Tuer (2600,-22000) (137,164); Tuermarken (129,124) / (137,172)
Punkt 3  1190 aus Gang (Hunde)  Zone Blatt 0 rect 1 zid 26  | aktuell: 1 | Marker (139,81)
           nach NUR Haupthalle: rect 1 AKTUELL, rect 2 unbesucht
         1190 aus Gang (Weste)  Zone Blatt 0 rect 2 zid 110 | aktuell: 2 | Marker (155,73)
           nach NUR Ostkammer:  rect 1 unbesucht, rect 2 AKTUELL
         11A0 Band 0 / Band 3   Zone Blatt 6 rect 2 | gezeigt Blatt 6 | aktuell: 2
Punkt 4  Marke #2 rect 3 (173,100) kind 2 = WAAGERECHTER Balken
Punkt 5  1110 Ankunft aus 1100: Teil 0 AKTUELL | Teil 1 unbesucht | Teil 2 unbesucht
         kleiner Raum oben:     Teil 0 besucht | Teil 1 AKTUELL | Teil 2 unbesucht
Punkt 6  1080 aus 1F (1040): 1F rect 9 AKTUELL | 2F rect 4 unbesucht | 3F rect 0 unbesucht
         danach in 1120 (3F): 3F-Kabine unbesucht, 2F-Kabine unbesucht
         4020 aus 4000: Blatt 8 rect 2 AKTUELL | Blatt 9 rect 13 unbesucht | Blatt 10 rect 3 unbesucht
```

## Messung nachher - ECHTE exe (`integration_r36_karte`, Spielstand + LOAD + Tuer per Aktionstaste + MAP)

Abzug RE15_INV_FB_SHOT bei RE15_WINDOW_SCALE=1, Bilder `M_karte_bilder/nachher_echtlauf_{A..G}.png`
(x2). Farbklassen im Abzug gemessen: aktuell (48,8,48)/(48,8,64), besucht (16,56,40)/(16,56,56),
Wand (176,176,176), Tuerbalken (224,168,40), Treppensprosse (240,240,216).
```
Endstand da5ae967 (Abzuege des Abschluss-Laufs, probe_r36_karte farben/marker):
A 1230 -> Tuer slot 1 -> 10A0 (B1): rect 6 (153,114)-(168,134) sprosse=30 (vorher 0)
         Tuer 11B0/11C0: (171..175,100) gelb=5 (waagerecht), x173 y96..99 / y101..104 gelb=0
B 1230 -> Tuer slot 0 -> 11D0:      MARKER 128 129 bbox (126,127)-(130,131)  (Tuermarke y124; vorher ~y150)
C 1230 -> slot 2 -> 1190 Haupthalle: rect 1 (77,77)-(142,113) rot=2429, rect 2 (145,70)-(158,103) rot+besucht=0
D 1230 -> slot 4 -> 1190 Ostkammer:  rect 2 rot=463, rect 1 rot+besucht=0
E 1100 -> slot 1 -> 1110:           Evidence Room rot=1144, kleine Raeume (190..211,123..144) rot+besucht=0
F Fahrt 1F->3F, zu Fuss nach 10C0, MAP auf 2F: 2F-Kabine (110..117,135..142) rot+besucht=0 (vorher besucht)
H Fahrt 1F->3F, MAP in 1120:        3F-Kabine (128..135,138..145) besucht=60 (Ankunft zaehlt)
G 1040 -> Fahrstuhltuer -> 1080:    1F-Kabine (110..117,135..142) rot=54
integration_r36_karte ... Passed 150.86 sec (8 Laeufe)
```
(Erster Lauf: D scheiterte, weil die Hunde aus ROOM1230 main00 (Else-Zweig Ck(3,0x89), Sce_em_set
@0x00D08/@0x00D1C) den Spieler vor der Tuer umrissen - Kill-Flags Zone 7 Bit 0x88/0x89 gesetzt; der
zweite Lauf zaehlte im Haupthallen-Kasten 22 "besucht"-Punkte des Gang-Rings (y115..117, x>=120) -
Kasten auf y<=113 begrenzt. Beides Messfehler der Schiene, nicht der Karte.)

## Tests

Neu (`tests/unit/probes/r36_karte.cmake`, Quelle `tests/unit/test_r36_karte.c` /
`tests/integration/test_r36_karte.cmake`):

| Test | misst | 78a4a748 (Gegenprobe build_bisect) | jetzt |
|---|---|---|---|
| unit_r36_karte_treppe | echter Weg 1230 -> 10A0 B1; genau 2 Treppen auf Blatt 0 rect 6, je <= 1 px auf der Projektion eines Band-4-STAIR-AOT aus g_aot, Sprossen auf gemaltem Innenraum; nur-1F -> keine; B2/1F je 2 | ROT ("sichtbar - 0") | gruen |
| unit_r36_karte_marker11d0 | echter Weg 1230 -> 11D0: Marker <= 6 px unter der Tuermarke, Sprung <= 8 px, an der 1260-Tuer <= 4 px ueber der gemalten Suedwand | ROT ((129,151), Sprung 28 px) | gruen |
| unit_r36_karte_r1190 | zwei Zonen/zwei Bits; Haupthalle NUR rect 1 (auch an den Zielscheiben), Ostkammer NUR rect 2 (Marker an der gemalten Nische), danach Haupthalle besucht; 11A0 auf Blatt 6, B1 rect 1 bleibt unbesucht | ROT (9 FAIL) | gruen |
| unit_r36_karte_tuer11c0 | Marke 11B0<->11C0 waagerecht, alle 5 Punkte auf der gemalten Wandzeile beider Kacheln, ueber der Nische | ROT (kind 1) | gruen |
| unit_r36_karte_teile1110 | echter Weg 1100 -> 1110 + Selbst-Tueren: kleine Raeume unbesucht bis betreten; Speichern/Laden; alter Stand (nur Zonen-Bit) | ROT (5 FAIL) | gruen |
| unit_r36_karte_fahrstuhl | echter Weg 1040 -> Kabine: nur 1F; Laden ist keine Ankunft; Kabine -> 1120: 3F besucht (Ankunft), 2F nicht; zu Fuss 10C0: 2F bleibt unbesucht; 1120 -> Kabine: 3F aktuell; Speichern/Laden; alter Stand mit Bits 3/4/5; Feld-Belegung; 4020 aus 5000 | ROT (5 FAIL, Stand vor der Ankunfts-Regel) | gruen |
| integration_r36_karte | echte exe, 8 Laeufe A..H (s.o.) | - | gruen |

Gegenprobe: `re15_map_zones.c/.h`, `karte_fahrstuhl_1080.c`, `re15_karte_fahrstuhl.h` voruebergehend auf
78a4a748, eigenes Bauverzeichnis `re15_port/build_bisect`, Riegel gefahren, danach `git checkout HEAD --`
(Ausgabe `M_karte_werkzeug/gegenprobe_78a4a748.txt`): alle sechs Riegel ROT, je genau am gemeldeten
Befund.

### Abschluss-Lauf (gezielte Suite, KEINE volle Suite - VERTRAG/Auftrag)

Stand da5ae967 (Endstand der Engine; danach nur Dossier/Bilder), im Baum gebaut
(`local_build.sh build`), Log `M_karte_werkzeug/ctest_gezielt_final.log`:
```
ctest --test-dir re15_port/build -R "map|karte|fahrstuhl|elevator|1190|weste|teilbereich|save|speicher|memcard|continue" --timeout 1500 --output-on-failure
100% tests passed, 0 tests failed out of 56
Total Test time (real) = 517.16 sec
```
Darin: alle unit_map_* (21), unit_r30/r33/r35_karte*, unit_fahrstuhl_1080/4020_etagen,
probe_elevator_1080, unit_1190_weste/_ruestung, unit_re2_room1190_ab, unit_weste_effekt,
unit_savedata, unit_savepoint, unit_map_speichern_laden, unit_r33_speichern, unit_r34n_d_adaruf_speicher,
prop_savestate_cmp, die sechs unit_r36_karte_*, integration_map_raum_live / _uebergang,
integration_r35_karte (74 s) / _fahrstuhl (176 s), integration_r36_karte (151 s),
integration_r33_speichern, integration_weste_load_pin, integration_save_counter_pin.
Kein Fenster-Haken musste nachgefahren werden. Der Lauf davor (Stand d9af50f5, vor der
Ankunfts-Regel) war ebenfalls 56/56.

## OFFEN

* Der Zwinger-Block von ROOM11D0 noerdlich der Tuerwand (z -16500..-10950, Kaefig-Gang) hat in der
  Kachel keinen Platz (der gemalte Kasten endet an der Tuerwand); dort klemmt der Marker an die
  Oberkante. Die Kachel ist ein leerer Platzhalter des Prototyps (MAP01.PIX uv(128,56)); eine
  bessere Lage braeuchte eine neue Kachel - nicht Teil des Auftrags.
* Blatt 0 rect 2 (Ostkammer) ist schematisch: die Kammer liegt in der Welt oestlich der Halle, die
  Kachel im Gang-Ring. Die 1190-Zeile @0x80076978 legt die Ostkammer auf x159..174 (nicht auf rect 2);
  benutzt wird die Bbox-Streckung (Tuer an der gemalten Nische, Marker bis 4 px an die Waende).
* R35-OFFEN 3 (Gang 1180/1230 schematisch, 11B0-Tuer unter dem Zweig gemalt) unveraendert.

## Fuer den Nutzer

* Punkt 3: Du schreibst "ROOM 1090" - die Hunde, die durch die Luke springen, und die Weste sind in
  ROOM1190 (Schiessstand, Untergeschoss B1). ROOM1090 ist der Hinterhof mit dem brennenden
  Transporter (keine Hunde, keine Weste). Umgesetzt ist es fuer ROOM1190: die Halle mit den
  Zielscheiben/Hunden ist jetzt der grosse Kasten links, die Kammer mit der Weste der schmale
  Kasten im Gang-Ring; jeder leuchtet nur, wenn du drin bist, und wird erst besucht, wenn du
  drin warst. Falls du doch ROOM1090 meinst: bitte mit einem Screenshot der Karte melden.
* Dafuer musste ROOM11A0 (der Raum mit der Leiter in die Kanalisation, erreichbar erst aus den
  Drains) von Blatt B1 auf sein eigenes Kartenstueck im Blatt "DRAINS B2" umziehen - auf B1 gibt
  es fuer ihn kein Rechteck (die Original-Kartenzeile legt ihn ueber den Blattrand).
* Fahrstuhl: die Kabine wird nur auf den Etagen als besucht gezeichnet, auf denen du eingestiegen
  oder mit ihr angekommen bist (1F -> 3F: 1F und 3F, nicht 2F). Das gilt auch fuer den zweiten
  Fahrstuhl ROOM4020 (Labor). In RE2 wird ein Raum, der auf mehreren Etagenkarten steht, auf
  allen gleichzeitig besucht - hier gilt deine Vorgabe.
* Alte Spielstaende: die kleinen Raeume in ROOM1110 erscheinen erst wieder, wenn du sie
  betrittst (der alte Stand weiss nicht, ob du drin warst); der Evidence Room bleibt sichtbar.
  Ebenso die Ostkammer von ROOM1190. Die Fahrstuhlkabine erscheint in einem alten Stand auf
  KEINER Etage, bis du wieder einsteigst - dann nur auf dieser Etage (der alte Stand hatte alle
  drei Etagen gemeinsam markiert; welche du wirklich benutzt hast, steht nirgends).
* Keine neuen Assets, keine Sprachdateien, kein neues Speicherformat (v9 bleibt).

## Stand / Fortschritt

- [x] Lektuere (AUFTRAG, VERTRAG, R35 G_karte + Abnahmen, Memories karte-*)
- [x] Messung vorher (alle 6 reproduziert)
- [x] P1..P6 umgesetzt (Daten + Latch), Messung nachher am Kartenpfad
- [x] Riegel unit_r36_karte_* (6) gruen, Gegenprobe am alten Motor ROT
- [x] integration_r36_karte (echte exe) gruen
- [x] gezielte Suite (siehe Abschluss-Lauf)

## Riegel 6 nach der Ankunfts-Regel (`probe_r36_karte fahrstuhl`, Stand da5ae967)

```
PASS: aus 1F eingestiegen: 1F-Kabine AKTUELL, 2F unbesucht, 3F unbesucht
PASS: Stand aus der Kabine geladen, im 3F-Raum: 3F-Kabine unbesucht (keine Fahrt)
PASS: mit dem Fahrstuhl auf 3F angekommen: 3F-Kabine besucht, 2F-Kabine unbesucht (nie dort)
PASS: zu Fuss auf 2F: 2F-Kabine unbesucht
PASS: aus 3F eingestiegen: Blatt 4, 3F-Kabine AKTUELL, 2F weiter UNBESUCHT
PASS: nach Speichern/Laden: 1F + 3F besucht, 2F unbesucht
PASS: alter Stand (alte Kabinen-Bits 3/4/5): 1F unbesucht, 2F unbesucht, 3F unbesucht - keine Kabine als besucht
PASS: Etagen-Tabelle (27 Bits) bleibt unter den Teil-Bits (96), Teil-Tabelle (8) unter den Fahrstuhl-Bits (112), Fahrstuhl-Tabelle (6) im Feld (128)
PASS: 4020 aus ROOM5000: Blatt 9, Blatt 9 AKTUELL, Blatt 8 unbesucht, Blatt 10 unbesucht
```
Riegel 2 zusaetzlich: Ankunft aus der Kanalisation (ROOM1260 Tuer slot 3 @0x0118A -> (3900,-22000))
-> Marker (141,164) an der unteren Tuermarke (137,172) (vorher (141,171)). Riegel 3 zusaetzlich:
ROOM1090 (Wortlaut) Hof Band 1 -> Blatt 2 NUR rect 5, Dach Band 6 -> Blatt 3 NUR rect 7 - die beiden
Bereiche von 1090 waren schon getrennt.
