# Runde 39 Spur G "karten" — Dossier (fortlaufend)

Baum: .claude/worktrees/r39_karten (Zweig r39/karten, Basis 1038dc15). Start 2026-10-18.

## Punkte (Wortlaut AUFTRAG.md)
1. "Bei den Maps des Police Departement passt es noch nicht so ganz. [...] map3F_correct.png, map2F_correct.png,
   map1F_correct.png, mapB2_correct.png. Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich
   aussehen sollte. Keine Doppelwände, Tür immer genau auf Wand platziert, treppen nicht in der Wand platziert usw. Und
   natürlich muss der Spielermarker da entsprechend passen. Der Room B1 den habe ich nicht gemacht. [...] Die Karte von B1
   ist noch ziemlicher schrott."
2. "In ROOM 1090 ist Leons Positionsmarker der Map außerhalb des aktuellen Raums."

## Fortschritt (gegen die Commits abgeglichen, Fortsetzung 2026-10-18 ~17:10)
- [x] Bestand lesen (Karten-Code, Memory, Runde 36/33/27-28) - 997a2cec
- [x] Messung vorher 1F/2F/3F/B2 (M1/M2: Vorlagen = 3x-Abzuege unseres Stands, Soll-Unterschiede je Blatt) - 97dca68e
- [x] Umsetzung 1F/2F/3F/B2: Kachel-Korrekturen engine/src/karte_r39.c + Marken re15_map_zones.h (4 Haken
      re15_inv_screen.c) - ba258ae4 (vom Kompositor gegen das Soll gerechnet, NICHT gebaut/gemessen)
- [x] Reste des Limit-Abbruchs 716eda1d: test_r35_karte.c/.cmake (1210-Tuer 11E0 x187->x186) + test_r39_karten.c
      (Sonde marker) - die Sonde kompilierte NICHT (Zeilenumbrueche in drei String-Literalen), Fortsetzung
      behoben; Bau (configure+build) gruen
- [x] Messung nachher 1F/2F/3F/B2 an der ECHTEN exe (Sweep, 0 Abweichungen gegen das Soll) - Abschnitt N1
- [x] Punkt 2 ROOM1090: gemessen (160/1225 Rasterpunkte ausserhalb), Zeilen je Ebene aus den Waenden, nachher 0 - P2
- [ ] Messung vorher B1
- [ ] RE-Belege B1
- [ ] Umsetzung B1
- [ ] Messung nachher je Etage (FB_SHOT gegen die Soll-Haelfte) + Marker-Lage je Raum (Leon + Elza)
- [ ] Tests (test_r39_karten + angepasste r35-Riegel begruenden)

## Messung vorher

### M1 Die Nutzer-Vorlagen sind pixelgenaue 3x-Abzuege UNSERES Stands (gemessen, 2026-10-18)
Werkzeug `G_karten_werkzeug/` (halves.py, phase.py, reg.py, soll.py; Lauf `lauf.sh` = exe-Kopie + Spielstand aus
`probe_r35_karte karte` + `RE15_MAP_SHOT_SWEEP`, `RE15_WINDOW_SCALE=1`, Stand 1038dc15 im Baum gebaut):
* Jede Vorlage = zwei gleich grosse Haelften (links 0..954/959, rechts ab 1045..1164), Versatz links<->rechts (0,0),
  links JEDER 3x3-Block einfarbig (Gleichfoermigkeit 1,000) -> reine 3x-Nearest-Neighbour-Abzuege.
* Linke Haelfte, auf 320x240 zurueckgerechnet, gegen den Sweep-Abzug des Baums (Startraum ROOM1190):
  1F 100,0 % / B2 100,0 % / 3F 99,999 % / 2F 98,8 % pixelgleich (2F: der Nutzer stand in ROOM1090 oben, aktuelle
  Flaeche (48,8,48) - mit der Op-Liste aus ROOM1090 Band 6 ebenfalls deckungsgleich bis auf den Marker-Ring).
  => links = exakt unser Stand 1038dc15; rechts = Soll (vom Nutzer bearbeitet, 238 Farben -> auf die 36 Farben der
  linken Haelfte geschnappt, max. Farbabstand 8,4).
* Soll-/Ist-Bilder 320x240: `G_karten_bilder/{ist,soll}_map{1F,2F,3F,B2}.png`.

### M2 Was der Nutzer geaendert hat (Klassenbild: P Panel, B Raumkoerper, W Wand, X Koerper ueber Wand, D Tuer, S Treppe)
Kompositor `komp.py`/`modell.py` (rastert die echte Op-Liste von `probe_r39_karten ops` mit MAP0x.PIX + CLUT-Zeile 21,
STP-Mittelung wie der Rasterer; Schwenk-Tilgung des Renderers datengetrieben aus dem Ist-Bild) bildet das Ist nach:
3F 0 / B2 2 / 1F 1 / 2F 14 (Marker-Ring) abweichende Pixel. Unterschiede Ist -> Soll (`komps.py`, Klassen):
| Blatt | Pixel | Inhalt |
|---|---|---|
| 1F (Blatt 2) | 322 | Doppelwaende weg: x164 + x179 (Kacheln r7/r8 zwischen r1-Ostwand x163 und r0/r6-Westwand x180), x144 (r1), x196 (r3), x118 (r2/r9 gegen r10-Westwand x119), x187 y145..154 (r0 gegen r4-Ostwand x186), Loch (185,145) gefuellt; ROOM1000-Schemakaesten: Westkanten x207 (gegen Flur-Ostwand x206), Oberkante y89 (gegen y88), Nord-WC-Unterkante y138 (gegen y139); Tuer (120,149..153) -> x119; Treppen (127,144) -> (122,144), (135,146) -> (133,146) |
| 2F (Blatt 3) | 53 | Tuer (188,116..120) -> x186 (auf die Wand), (186,121) X -> W; Treppen (126,144) -> (121,144), (134,146) -> (132,146) |
| 3F (Blatt 4) | 98 | X-Streifen x144 y81..118 -> B (r4-Westwand unter r2-Koerper), x152/x153 y119..157 (Wand nach x153, X weg); Tuer (166..170,89) -> (164..168,89); Treppe senkrecht (168,106) -> (168,107) |
| B2 (Blatt 1) | 93 | Doppelwand x187 y79..112 weg (x186 bleibt); Tuer (187,72..76) -> x186; Tuer (163..167,139) -> y138; Treppen (177,115) -> (173,115), (186,112) -> (183,112) |


### N1 Messung nachher 1F/2F/3F/B2 (echte exe, Stand f6ef49cf + Bau 17:20)
`G_karten_werkzeug/lauf2.sh <ordner> 1190 300 1190 3174 -11182 0 -- RE15_MAP_SHOT_SWEEP=s` (Spielstand per
probe_r35_karte karte, exe-Kopie re15_pc_g39f_<id>.exe, RE15_WINDOW_SCALE=1, alle 13 Blaetter in einem Lauf), Vergleich
`vgl_voll.py <ordner>` (Klassenbild ganzer Schirm, aktuell = Koerper, Spieler-Ring ausgenommen):
```
map1F Abweichungen: 0   map2F Abweichungen: 0   map3F Abweichungen: 0   mapB2 Abweichungen: 0   GESAMT 0
```
Rohfarben: 1F/B2 0 Pixel, 3F 1 Pixel (Rahmen (0,238)), 2F nur der Zustand (Nutzer stand in ROOM1090 = aktuell rot)
und sein Marker-Ring (184..188,81..85). Die vier Blaetter der echten exe SIND die rechten Haelften der Vorlagen.

### P2 Punkt 2 ROOM1090 - Messung vorher
Nutzerstandort aus seinem befund.log (nutzer/befund_marken_2026-10-17.txt Zeile 125ff, F1126..F1351):
R1090 pos=(-12528,-9000,959) = obere Ebene (Band 5). Sonde `probe_r39_karten marker 1090 -12528 959 5 -9000`:
`Blatt 3, Zone Blatt 3 rect 7 zid 22, Marker (187,84), Kachel-Index am Marker 0` - der Ring (186,83) steht im
gruenen Nachbarraum neben dem roten Gamma, wie in der linken Haelfte von map2F_correct.png.
NEU `probe_r39_karten markerflaeche <raum> <band> <x> <z> <y>`: begehbare Flaeche per ECHTEM Kollisionsloeser
(re15_collision_constrain_contact_band, Port FUN_8003b0a4 mit Rechteck/Kreis/Diagonale, Spielerradius 450) ab dem
Ankunftspunkt, jeder 3. Rasterpunkt (300 Einheiten) durch den echten Kartenpfad, gezaehlt: Marker NICHT auf der
Zeichnung der eigenen Zone (Kachel-Index 0 bzw. ausserhalb der Schema-Zellen).
```
ROOM1090 Band 5 Saat (-13600,1300) y -9000: 1225 geprueft, Marker x 184..213 y 80..110, ausserhalb 160
ROOM1090 Band 1 Saat (-10000,6000) y -1800:  590 geprueft, Marker x 186..215 y 63..75,  ausserhalb 0
(ROOM1091 Elza: dieselben Zahlen)
```
URSACHE: beide Zeilen (Blatt 2 Hof, Blatt 3 Gast-Zeile obere Ebene) strecken die Huelle BEIDER Ebenen
(x -17500..10285, z -18700..11754) auf ihr Rechteck. Die obere Ebene (begehbar x -14600..2700, z -16392..3808)
landet damit auf x 184..214 / y 78..110 - das Querband (z 508..3808) auf y 78..84 statt auf die gemalten y 68..75,
also unter das gemalte Band in die durchsichtige Ecke der Kachel. Der Hof liegt gestaucht, aber innen.

### P2 RE-Belege
* Original-Zeile ROOM1090 = Index 9: PSX.EXE @0x800768F8 = `00 00 00 00 01 00 01 00` (Stub, jede Lage -> (0,0));
  der Stage-1-Seiten-Setzer kennt fuer 1090 nur Blatt 2 (Runde 10/30). Das Original hat hier KEINEN brauchbaren
  Marker - die Zeile ist eine PORT-ERGAENZUNG, die Form ist die des Originals:
* Formel FUN_800473f8 (re15_disasm.py dis 0x8004741c): `addiu v0,v0,32000` @0x80047428, x10 per `sll/addu/sll`
  @0x8004742c-48, `lhu v0,0(at)` sx @0x80047444 (0x800768b4+8i), `sra t0,t0,20` @0x8004745c, `addiu t0,t0,5`
  @0x80047460, /10 per 0x66666667 @0x80047450-54, `lh v0` ox @0x800474e8; z: `lhu a0` sy @0x8004747c,
  `subu v1,zero,v1` @0x800474b0 (negiert), `lhu v1` oy @0x80047518. Der Port rechnet sie in re15_map_zone_marker
  (Zonen-Zeile ox/oy/sx/sy).
* Kalibrierung je Ebene: jede Wandflaeche der begehbaren Flaeche (SCA-Zelle des Bands, Datei-Offset ROOM1090.RDT,
  `sca_liste.py 1090 <band>`) auf ihre gemalte Wandlinie (Kachel-Index 4, `kachel.py`), kleinste Quadrate
  (`zeile_fit.py`), ganzzahlig mit der exakten Formel nachgerechnet:
  - Hof Band 1, Blatt 2 rect 5 (180,59,48,32) MAP03.PIX uv(112,64): x -14546 @0x01E84 -> 180, -4414 @0x01EC0 -> 203,
    3972 @0x01E00 (Typ 4) -> 222 (Reste 0,0,0); z 7720 @0x01DF4 -> 59, 3800 @0x01E30 -> 69, -4214 @0x01F5C -> 88
    (Reste 0,-1,0) => ox 140, oy 155, sx 2376, sy 2535.
  - obere Ebene Band 5, Blatt 3 rect 7 (179,67,48,48) MAP04.PIX uv(112,74): x -14672 @0x01E54 -> 179, -6878 @0x01FBC
    -> 197, -4814 @0x01E48 -> 204, 2752 @0x01F38 -> 222 (Reste 0,1,-1,0); z 3404 @0x01E6C -> 67, 491 @0x01FC8 -> 76,
    -5276 @0x01F2C -> 87, -16403 @0x01F20 -> 114 (Reste 1,-1,1,0) => ox 136, oy 150, sx 2597, sy 2435.
  Acht unabhaengige Waende je Ebene treffen ihre Linie auf hoechstens 1 px - die Kachel ist massstabsgerecht
  gemalt, nur die Zeile fehlte. Probe Hof: Tuer -> ROOM1050 (Mitte (-10080,3060), ROOM1090.RDT Tuer slot 0) faellt auf
  (190,70) = das gemalte Tuerblatt (187..195,64..68) an der Marke (191,69).

### P2 Umsetzung
re15_port/engine/src/re15_map_zones.h: Zeilen 0x1090/0x1091 Blatt 2 (ox 140, oy 155, sx 2376, sy 2535) und die
Gast-Zeilen Blatt 3 (ox 136, oy 150, sx 2597, sy 2435), Kommentar "Runde 39 Spur G" mit allen Offsets.
Weltbox unveraendert (Zonen-Zugehoerigkeit: der Raum hat EINE Zone).

### P2 Messung nachher (Sonde, Bau 17:35)
```
ROOM1090 Band 5: 1225 geprueft, Marker x 183..221 y 71..111, ausserhalb 0     (vorher 160)
ROOM1090 Band 1:  590 geprueft, Marker x 184..219 y 63..86,  ausserhalb 0
ROOM1091 (Elza): dieselben Zahlen
Nutzerpunkt (-12528,-9000,959): Blatt 3 rect 7, Marker (184,73), Kachel-Index 1 (Querband)
```

## RE-Belege
(B1 folgt)

## Umsetzung
(folgt)

## Messung nachher
(folgt)

## Tests
(folgt)

## OFFEN
(folgt)

## Fuer den Nutzer
(folgt)
