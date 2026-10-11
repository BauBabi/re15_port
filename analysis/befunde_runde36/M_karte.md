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

(fortlaufend: R1..R6 je Punkt folgen)

## Stand / Fortschritt

- [x] Lektuere (AUFTRAG, VERTRAG, R35 G_karte + Abnahmen, Memories karte-*)
- [x] Messung vorher (alle 6 reproduziert)
- [ ] P1 Treppen 10A0 B1
- [ ] P2 Marker 11D0
- [ ] P3 ROOM1190 zwei Bereiche (+ 11A0 raus aus B1 rect 1)
- [ ] P4 Tuer 11C0 90 Grad
- [ ] P5 1110 Teilbereiche
- [ ] P6 Fahrstuhl-Etagen (1080 + 4020)
- [ ] Tests, Abschluss
