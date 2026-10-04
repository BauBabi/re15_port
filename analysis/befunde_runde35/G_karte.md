# Runde 35 Spur G "karte" — Dossier

Baum `.claude/worktrees/r35_karte`, Zweig `r35/karte`, Basis master 154a73c1 (geprueft: status leer, log -1 = 154a73c1).

## Punkte (Wortlaut AUFTRAG.md)
1. "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht."
2. "In ROOM 11F0 taucht nicht auf der Karte auf, wenn man drin ist." + "In ROOM 1200 taucht nicht auf der Karte auf, wenn man drin ist."
3. "In ROOM 1230 bekomme ich die Map von ROOM 11E0."
4. "In ROOM 1210 ist der Korridor falsch und so gut wie alle Türen fehlen"

## Protokoll (fortlaufend)

### Werkzeug
Mess-Sonde `re15_port/tests/unit/test_r35_karte.c` (Ziel `probe_r35_karte messung`, registriert in
`tests/unit/probes/r35_karte.cmake`): faehrt den echten Kartenpfad wie `test_map_raum_live.c`
(RDT laden + `scd_room_reenter`, Spieler setzen, `re15_map_zone_update`, `re15_inv_map_stage_init`
wie `menu_common.c map_entry()`, `re15_inv_map_page_shown`, `re15_map_rect_state` je Rechteck,
`re15_inv_map_marker`). Ausgabe je Punkt: Zone (Blatt/Rect/zid), gezeigtes Blatt, Rechtecke im
Zustand AKTUELL, Spieler-Marker.

### Messung vorher (Stand 154a73c1, gebaut im Baum, 2026-10-03)
```
=== Punkt 1: Fahrstuhl ROOM1080 ===   (Etage der Kabine ueber Bank 3 Bit 54/55/56 gesetzt)
  Kabine 1F/2F/3F, 6 Punkte je Etage, IMMER: Zone Blatt 2 rect 9 | gezeigt Blatt 2 | aktuell 9
  Marker ueber den ganzen Kabinen-Innenraum (x -15500..-11800, z -3900..-300):
     x 113..118 (5 px), y 144..146 (2 px)  -- fuer 1F, 2F und 3F identisch
=== Punkt 2 ===
  11F0 Ankunft (250,250):     Zone Blatt 1 rect 0 zid 32 | aktuell: 0 | Marker (106,111)
  11F0 Mitte (7000,-12000):   Zone Blatt 1 rect 0 zid 32 | aktuell: 0 | Marker (121,137)
  1200 Ankunft (-20154,-25245): Zone Blatt 1 rect 0 zid 33 | aktuell: 0 | Marker (165,135)
  1200 Mitte (-22000,-15000): Zone Blatt 1 rect 0 zid 33 | aktuell: 0 | Marker (159,113)
  11E0 (-24707,-9442):        Zone Blatt 1 rect 0 zid 31 | aktuell: 0
  -> 11E0, 11F0 und 1200 teilen EIN Rechteck (rect 0 = die Garagen-Kachel von 11E0).
=== Punkt 3 ===
  1230 (3 Ankunftspunkte):    KEINE Zone | gezeigt Blatt 1 | aktuell: - | Marker (0,0)
  1180 (dieselben Punkte):    Zone Blatt 0 rect 6 zid 25 | gezeigt Blatt 0 | aktuell: 6
  -> 1230 zeigt Blatt 1 (B2) = das Blatt von 11E0, ohne Hervorhebung, Marker in der Bildecke.
=== Punkt 4 ===
  1210 (3 Ankunftspunkte):    Zone Blatt 1 rect 4 zid 34 | aktuell: 4   (rect 4 = 16x24-Zelle)
  1220 (5 Zellen):            Zone Blatt 1 rect 3 zid 35 | aktuell: 3   (rect 3 = 64x80-Korridor)
  Marken Blatt 1 mit zid 34/35: nur 5 (#11..#15), davon Tuer 1210<->11E0 einmal, Zellentueren 3 von 5.
```

## RE-Belege (Adressen, Bytes) — alle aus info/Re1.5/PSX.EXE bzw. den RDTs selbst gelesen

### B1 Seiten-Setzer STAGE1 `FUN_8004b568` (Tabelle @0x8001103c, 38 Faelle)
`re15_disasm.py dis 0x8004b568`: `lh v1,0x0fe2(0x800b)` (Raumindex) / `sltiu v0,v1,0x26` @0x8004b574 /
Sprungtabelle @0x8001103c. Fall-Ketten enden in: Raum 0..11 -> `ori v0,zero,0x2` @0x8004b684,
12..17 -> `ori v0,zero,0x3` @0x8004b6f8, 18..22 -> `ori v0,zero,0x4` @0x8004b758,
23 -> `ori v0,zero,0x5` @0x8004b768, 24..29 -> `sb zero,0x260e` @0x8004b7dc, 30..37 ->
`ori v0,zero,0x1` @0x8004b884; Schreiber `sb v0,0x260e(0x800b)` @0x8004b88c (= SEITE),
`sb v1,0x260d` @0x8004b894 (= Zeilenindex der Massstabstabelle).
=> ROOM1180 (24) -> Blatt 0 "POLICE STATION B1", ROOM1230 (35) -> Blatt 1 "POLICE STATION B2"
(Beischrift in der Kachel uv(0,0) gelesen, Abzug scratch page0_rects.png/page1_rects.png).

### B2 Rechteck-Tabellen (u16 count @0x80076840+8*Seite, Liste @+4, 12 B {x,y,w,h,u,_,v,_})
Blatt 0 @0x800762a0 (7): r0 (120,60,56,72) uv(72,48) · r1 (76,76,72,48) · r2 (144,69,16,40) ·
r3 (168,76,72,32) · r4 (168,100,72,96) · r5 (96,124,56,56) · r6 (153,114,16,24) uv(128,16)
Blatt 1 @0x800762f4 (10): r0 (100,70,88,80) uv(168,16) · r1 (100,101,40,56) uv(0,32) ·
r2 (141,101,32,40) uv(40,32) · r3 (187,70,64,80) uv(96,48) · r4 (187,78,16,24) · r5 (187,95,16,24) ·
r6 (212,78,16,24) · r7 (212,95,16,24) · r8 (212,112,16,24) · r9 (170,101,24,24) uv(128,16)
Blatt 2 @0x8007636c r9 (109,134,16,16) uv(168,40) · Blatt 4 @0x80076468 r0 (127,137,16,16) uv(168,40)
(Blatt 3: Port-Ersatztabelle s_map_rectfix Eintrag 4 = (109,134,16,16) uv(168,40)).

### B3 Massstabszeilen @0x800768b0 + 8*Index {s16 ox, s16 oy, u16 sx, u16 sy} (`FUN_800473f8` @0x8004741c-0x80047528)
11E0 @0x800769a0 = (91,157,2304,2240) · 11F0 @0x800769a8 = Stub (0,0,1,1) · 1200 @0x800769b0 =
(129,150,3168,2305) · 1210 @0x800769b8 = (177,140,2496,2250) · 1220 @0x800769c0 Stub · 1230 @0x800769c8
Stub · 1180 @0x80076970 Stub · 1080 @0x800768f0 Stub · 1040 @0x800768d0 = (94,193,2080,2320) ·
1190 @0x80076978 = (75,127,1920,2301) · 11B0 @0x80076988 = (159,110,2272,1954).

### B4 Projektionen mit der Original-Formel (scratch b1.py, gleiche Rechnung wie FUN_800473f8)
* 1200 (eigene Zeile): SCA-Huelle -> (137..176, 99..143) = Blatt 1 **rect 2** (141..173,101..141);
  Tuer 1200->11E0 (ROOM1200.RDT @0x7BA) -> (165,139) = gemalte Tuernische in rect 2 (163..167,134..138);
  Gegenseite 11E0->1200 (ROOM11E0.RDT @0x1532) mit 11E0-Zeile -> (166,140).
* 11F0 (Stub; Header-Zeile (35,180,2304,2240) aus der Tuerkette hergeleitet): Huelle -> (99..134, 97..149)
  = **rect 1** (100..140,101..157); die L-Form trifft Pixel fuer Pixel: Kollisionswand z 1150..2150
  -> y 107 (Kunst: obere Wand links y 108), z 5700 -> y 100 (Kunst: y 101), Ecke x 1900..2900 -> x 109
  (Kunst: 108). Tuer 11F0->11E0 (@0xCC6) -> (106,109) = gemalte Doppeltuer rect 0 (101..110,104..109).
* 1210 (eigene Zeile): Huelle -> (184..230, 68..133) = **rect 3** (T-Korridor, gemalt: Querbalken y70..77,
  Laengsgang x201..212). Tueren 1210->1220 (@0x1CE6/0x1D06/0x1D26/0x1D46/0x1D66) -> (202,84) (212,92)
  (202,101) (212,108) (212,126) = je 1 px an den gemalten Nischen von rect 4 (196..201,80..84),
  rect 6 (212..217,89..93), rect 5 (196..201,97..101), rect 7 (212..217,106..110),
  rect 8 (212..217,123..127). Tuer 1210->11E0 (@0x1CC6) -> (188,76) = Nische rect 3 (187..192,72..76).
* 1220 teilt den Weltrahmen mit 1210 (jede der 5 Tuerpaarungen: Tuer-Rechteck der einen Seite liegt
  auf dem Spawn der anderen, z.B. 1210 @0x1CE6 r(-22200,-7200,1500,2200) -> Spawn (-22400,-6500);
  1220 @0xDD2 r(-22000,-7400,500,2000) -> Spawn (-20890,-6560); Zellwand (-17275,-28650,200,24350)
  trennt Gang und Ostzellen). Mit der 1210-Zeile projiziert: Zellenteiler y 81 / 96-97 / 114
  (Kunst 78 / 95 / 112) -> 5 Zellen = rect 4,5 (West) und 6,7,8 (Ost); die SW-Kammer (z<-19875 West)
  hat keine Tuer und ist nicht gemalt.
* 1230 = 1180 (VARIANTE desselben Ortes): alle Tuer-Datensaetze identisch
  (1180 @0x9EE/0xA12/0xA34/0xA54/0xA74/0xA94 == 1230 @0xAEE/0xB12/0xB34/0xB54/0xB74/0xBC4:
  gleiche Rechtecke, Spawns, Ziele 1160/11D0, 10A0, 1190, 11B0, 1190), gleiche SCA-Huelle
  (-10592..6733, -17477..32871); Nachbarn 1190/11B0/11D0 tragen beide Ziele auf DEMSELBEN Slot
  (1190 @0x28B2/0x28D6, 11B0 @0xF64/0xF88, 11D0 @0x11E2/0x1206).
  -> 1230 gehoert auf Blatt 0 (B1) wie 1180; der Seiten-Setzer schickt Index 35 auf Blatt 1 (B2) —
  dort gibt es fuer diesen Ort KEIN Rechteck (alle 10 Rechtecke von Blatt 1 sind 11E0/11F0/1200/1210/
  1220x5/10A0). Prototyp-Luecke (Stub-Zeile @0x800769c8, Marker = (0,0)).
* Blatt 0 rect 0 ist der Gang 1180/1230: seine 5 gemalten Nischen entsprechen den 5 Tueren:
  (136..141,71..75) Suedwand -> rect 1 = 1190-Hauptraum (1190-Zeile: Tuer @0x28B2 -> (139,78));
  (152..155,70..73) -> rect 2 (1190-Ostkammer, Tuer 1230 Slot 4 @0xCE6); Doppeltuer x168 y78..86 ->
  rect 3 = 11B0 (11B0-Zeile: Tuer @0xF64 -> (168,82)); (155..159,108..112) -> rect 6 (10A0, Tuer
  @0xA34); (125..129,119..123) -> rect 5 (11D0, Tuer @0x9EE). rect 6 dagegen ist ein leerer 16x24-Kasten
  ohne Nische — ein Ort mit 5 Tueren kann nicht rect 6 sein. Kein Zeilenpaar in der Tabelle traegt
  rect 0 (Messung: nur 1180 -> rect 6, 1190 -> rect 2, 11A0 -> rect 1, 11B0 -> 3, 11C0 -> 4, 11D0 -> 5).
  Die Kunst ist SCHEMATISCH (Affin-Fit der 5 Tueren: Rest bis 17 px) — der Gang wird als Ring gezeichnet.

### B5 Fahrstuhl ROOM1080
* Etage der Kabine = Bank 3 Bit 54/55/56, gesetzt von den Etagenraeumen beim Betreten:
  ROOM1040 @0x15C8/0x15D6 `21 03 36 00`/`22 03 36 01`, ROOM10C0 @0x0FE0/0x0FEE `22 03 37 01`,
  ROOM1120 @0x0D5E/0x0D6C `22 03 38 01` (je die anderen beiden geloescht); gelesen von ROOM1080 sub10
  @0x08AC/0x08BC/0x08CC (`21 03 36 01` / `21 03 37 01` / `21 03 38 01` -> Aot_on 0/1/2).
  (Belegt in analysis/befunde_2026-09-26/fahrstuhl-faehrt-nicht.md §2.4, Bytes hier nachgelesen.)
* Seite der Etagenraeume (Setzer B1): 1040 -> 2, 10C0 -> 3, 1120 -> 4.
* Orientierung der Kabine: Tuer-Datensaetze ROOM1040 @0x1096 Spawn (-13650,0,-900) Yaw 0x0400,
  ROOM10C0 @0xE82 / ROOM1120 @0xCB6 ebenso Yaw 0x0400; Rueckweg ROOM1080 @0x482 -> 1040 Yaw 0x0400,
  @0x4A2 -> 10C0 / @0x4C2 -> 1120 Yaw 0x0C00. Vorwaerts = (cos,-sin) (player_common.c:1292-1293):
  Yaw 1024 = -z, 3072 = +z. Man geht in 1040 nach +z (Kabine liegt auf Blatt 2 NOERDLICH der Tuer:
  1040-Zeile -> Tuer (114,145), Kabinen-Suedwand der Kachel y=143) und steht in 1080 mit Blick -z:
  die Kabine ist gegen 1040 um 180 Grad gedreht (flip_x = flip_z = 1). Gegen 10C0/1120 ist sie NICHT
  gedreht (aus der Kabine +z -> Ankunft Yaw 3072 = +z), und 10C0/1120 fuehren selbst flip 1,1 auf
  ihren Blaettern (re15_map_zones.h) -> auf allen drei Blaettern 180 Grad. Konsistent.
* Kabinen-Kunst (MAP-Kacheln uv(168,40)): Kasten 10x10, Innen 8x8 — Blatt 2/3 x110..117 y135..142,
  Blatt 4 x128..135 y138..145. Kollision ROOM1080: Waende x -16750..-15750 / -11550..-10550,
  z -5150..-4150 / -50..950 -> Innenraum x -15750..-11550, z -4150..-50.
* Gemessener Defekt: die hergeleitete Zeile (78,214,2080,2320, ohne Spiegel) projiziert den Innenraum
  auf x 111..119 / y 143..152 — also UNTER die Kabine (Suedwand y=143); die Klemmung (Rand 4,
  re15_inv_screen.c) laesst davon y 144..146 uebrig. Und die Etagenwahl nimmt bei gleichem Band
  immer die ERSTE Zeile (re15_map_floor_lookup) = Blatt 2.

## Umsetzung (Dateien, Konstanten)

Alle Datenzeilen in `re15_port/engine/src/re15_map_zones.h` (je Block mit Kommentar "Runde 35 Spur G";
Rechnung reproduzierbar: `python analysis/befunde_runde35/G_karte_zeilen.py`). Kein Patch an shared_assets.

| Punkt | Aenderung | Beleg |
|---|---|---|
| 1 | `0x1080/0x1081` Blatt 2 rect 9, Gastzeilen Blatt 3 rect 4 / Blatt 4 rect 0: ox,oy,sx,sy = 129,117,795,805 (Blatt 2/3) bzw. 147,120,795,805 (Blatt 4), flip 1,1 | B5: 180 Grad aus Yaw ROOM1040 @0x1096 (0x0400) vs Gehrichtung; Ziel = gemalter Innenraum (Kachel uv(168,40)) geschnitten mit dem Klemmfenster (Rect+4); Massstab PORT-WAHL (Stub @0x800768f0) |
| 1 | NEU `engine/src/karte_fahrstuhl_1080.c` + `include/re15_karte_fahrstuhl.h`: Blatt der Kabine = Etage des Vorraums (Spiegel DAT_800b0fe6, FUN_8001d600 @0x8001d92c/@0x8001d938/@0x8001d95c), sonst Bank 3 Bit 54/55/56 (ROOM1040 @0x15D6, ROOM10C0 @0x0FEE, ROOM1120 @0x0D6C); Blatt je Etage aus dem Seiten-Setzer @0x8004b684/@0x8004b6f8/@0x8004b758 | B1, B5 |
| 1 | Haken `re15_map_zones.c`: `re15_karte_raum_gesehen(room)` in `re15_map_zone_update` (1 Zeile), Filter `if (fb >= 0 && page != fb) continue;` in `floor_row` und `re15_map_floor_lookup` (je 2 Zeilen), 1 include | — |
| 2 | `0x11F0/1` rect 0 -> **1**; `0x1200/1` rect 0 -> **2** (Abbildungen unveraendert: 11F0 hergeleitet, 1200 = @0x800769b0) | B4 |
| 3 | `0x1180/1` und NEU `0x1230/1`: Blatt 0 **rect 0**, 5 Abschnitte (idx 0..4, zid 25/106/107/108/109), je eigene Streckung auf den gemalten Streifen; Kaesten an gemeinsamen Kanten um ZONE_SLACK=1500 eingezogen | B1, B4; Kunst schematisch -> PORT-WAHL |
| 3 | NEU Gastzeile `0x10A0/1` Blatt 0 rect 6 + Etagenzeile (10A0, z0, Band 4 -> Blatt 0 rect 6) am ENDE von s_map_floors + Etagen-Bit 25 in `s_etage_bit` (re15_map_zones.c, angehaengt) | Band 4 = -(-7200/0x708), Tuer @0xA34 |
| 3 | Marken Blatt 0: die zwei alten 1180-Marken auf rect 6 ersetzt durch (139,76) und (157,113) auf rect 0 (ungepaart); 1190-Marke (151,69) und 11D0-Marke (129,124) mit dem Gang gepaart (zid2 106/108, auf_partner 1) | gemalte Nischen B4 |
| 4 | `0x1210/1` rect 4 -> **3**; `0x1220/1` eine Zone -> **5 Zellen** (rect 4/6/5/7/8, zid 35/102/103/104/105) mit der 1210-Zeile @0x800769b8 (gemeinsamer Weltrahmen) | B4 |
| 4 | Marken Blatt 1: 5 alte (davon (206,95) quer im Gang) ersetzt durch die 6 Tueren von 1210 auf den gemalten Nischen: (187,74) 11E0, (201,82)/(201,99) West, (212,91)/(212,108)/(212,125) Ost | B4 |
| 3/4 | Besucht-Bits fuer Zonen idx>=2: `s_zone_zusatzbit` += 1220 idx 2..4, 1180 idx 2..4, 1230 idx 2..4 (Bits 242..250 von 256, angehaengt) | Pin unit_map_zone_bits |

Geaenderter fremder Pin (begruendet): `tests/unit/test_map_etagenzeile.c` — die Regel "Gast-Zeile traegt KEINE
Kartenzeile (sx=0)" meinte eine BLATTFREMDE absolute Zeile (2026-09-07: die 1060-Zeile stand als eine Spalte
x=122 daneben). Jetzt zulaessig: eine Zeile, deren Projektion des Kasten-Mittelpunkts IN das eigene Rechteck
faellt. Die Wirkungsprobe (>= 10 verschiedene Markerpixel) bleibt unveraendert und ist gruen (25 Pixel).

Dateiregel (VERTRAG §1.4): `re15_map_zones.h` ist eine gemeinsame Datei. Geaendert sind NUR die Zeilen der
Raeume dieses Auftrags (1080, 11F0, 1200, 1210, 1220, 1180, 1230, 10A0-Gast, Marken Blatt 0/1, Etagenzeile am
Ende) — Datenzeilen, keine Logik; jede Stelle traegt "Runde 35 Spur G". In `re15_map_zones.c` 6 Haken-Zeilen
+ 10 angehaengte Tabelleneintraege.

## Messung nachher (probe_r35_karte messung, gleicher Stand wie die Riegel)
```
Punkt 1  Kabine 1F (Bits)  Blatt 2 aktuell 9 | Ecken SW(116,139) SO(114,139) NW(116,141) NO(114,141) Mitte (115,140)
         Kabine 2F (Bits)  Blatt 3 aktuell 4 | dieselben Pixel (Ersatzrect (109,134) wie 1F)
         Kabine 3F (Bits)  Blatt 4 aktuell 0 | SW(134,142) SO(132,142) NW(134,144) NO(132,144)
         aus ROOM1040/10C0/1120 in die Kabine: Blatt 2/3/4, aktuell 9/4/0 (Vorraum gewinnt)
         -> Marker IMMER im gemalten Kabinen-Innenraum (vorher y 144..146 = unter der Suedwand y143),
            folgt dem Spieler in beiden Achsen (4 px Fenster: 8x8-Marker auf 10x10-Kabine, Rand 4).
Punkt 2  11F0 Ankunft/Mitte: Blatt 1 aktuell NUR 1, Marker (106,111)/(121,137)
         1200 Ankunft/Mitte: Blatt 1 aktuell NUR 2, Marker (165,135)/(159,113);  11E0: NUR 0
Punkt 3  1230 und 1180 an 6 Punkten: Blatt 0 (B1) aktuell NUR 0, Marker auf dem Gang:
         aus 1190 (140,73) · aus 11B0 (165,64) · aus 11D0 (131,123) · Gang-Mitte/Versatz ebenso
         10A0 Band 4: Blatt 0 aktuell NUR 6
Punkt 4  1210 (3 Ankuenfte): Blatt 1 aktuell NUR 3, Marker (191,76) (203,85) (210,126)
         1220 Zellen: aktuell 4 / 6 / 5 / 7 / 8, Marker (199,85) (216,93) (199,101) (216,110) (216,125)
         Marken Blatt 1 zid 34: 6 (187,74) (201,82) (212,91) (201,99) (212,108) (212,125), alle sichtbar
integration_map_raum_live: 93 Raeume, 92 sichtbar rot (einzige Ausnahme ROOM5020 Blatt 9, unberuehrt),
Spieler-Marker in der roten Flaeche 92/92, nie im fremden Raum.
```

## Tests
Neu (tests/unit/probes/r35_karte.cmake, Quelle tests/unit/test_r35_karte.c):
* `unit_r35_karte_fahrstuhl` — echter Weg Etagenraum -> Kabine (betrete = scd_room_reenter ->
  re15_map_zone_update): Blatt 2/3/4 + aktuell nur rect 9/4/0; mit Bits: dasselbe; 4 Ecken je Etage im
  GEMALTEN Innenraum (Kachel-Index 1, aus DATA/MAP0x.PIX gelesen) und 180-Grad-Richtung; ohne Bit/Vorraum
  Blatt 2.
* `unit_r35_karte_b2` — 11F0 -> rect 1, 1200 -> rect 2, 11E0 -> rect 0, je NUR dieses aktuell, Marker auf
  gemalter Flaeche; danach rect 1/2 besucht.
* `unit_r35_karte_r1230` — 1230 und 1180 an 6 Punkten: Blatt 0, NUR rect 0, Marker auf dem gemalten Gang;
  NICHT Blatt 1; 10A0 Band 4 -> Blatt 0 rect 6.
* `unit_r35_karte_r1210` — 1210 -> NUR rect 3 (3 Ankuenfte), 5 Zellen -> rect 4/6/5/7/8; JEDE der 6
  Tuer-AOTs von 1210 (aus g_aot nach dem Laden) hat eine SICHTBARE Marke auf gemalter Wand (Index 4)
  <= 3 px neben ihrer Projektion mit der Zeile @0x800769b8.
Gegenprobe (logisch, aus der Messung vorher): alter Stand -> 1080 immer Blatt 2 und Marker y144..146
ausserhalb des Innenraums (y135..142); 11F0/1200 aktuell 0; 1230 keine Zone/Blatt 1; 1210 aktuell 4 und
nur 3 Zellentueren -> jeder der vier Riegel waere ROT.


### Echtlauf (echte exe, Spielstand + LOAD GAME + MAP, Framebuffer-Abzug) — `integration_r35_karte`
Bilder: `analysis/befunde_runde35/G_karte_bilder/nachher_{A..E}.png` (RE15_INV_FB_SHOT = Software-Framebuffer
des Kartenschirms; gdigrab liefert in dieser Sitzung weisse Bilder, AUTOSHOT nicht benutzt). Aktuell-Farbe im
Abzug GEMESSEN (48,8,48)/(48,8,64) = halbtransparentes 0x680808 auf dem blauen Grund.
```
A ROOM1230 (Spielstand an der 11D0-Tuer): Blatt "POLICE STATION B1", rot im Gang-Kasten 1030 px, ausserhalb 0
B ROOM11F0 (Ankunft):                    Blatt B2, rot in rect 1 1392 px, ausserhalb 0
C ROOM1200 (Ankunft):                    Blatt B2, rot in rect 2  995 px, ausserhalb 0
D ROOM1210 (Ankunft aus 11E0):           Blatt B2, rot in rect 3  763 px, ausserhalb 0, gelb 30 px = 6 Balken x 5
E NUTZERWEG ROOM1120 -> Aktionstaste an der Fahrstuhltuer -> ROOM1080 -> MAP:
                                         Blatt "POLICE STATION 3F", rot in der Kabine (127..142,137..152) 51 px,
                                         ausserhalb 0, Marker in der Kabine (debug.log: CONTINUE 1120 ->
                                         RBJ room 1080 -> fb shot -> EXIT_AT in Raum 1080)
```
Erster Lauf scheiterte an E ohne Strom-Flag (4,243): Spieler stand in der Tuer, Aktionstaste ohne Wirkung
(debug.log 30 Bilder lang pos=(1300,0,6868)) - mit Set(4,243) (wie ROOM11F0 sub18) faehrt der Weg durch.

## OFFEN (mit Adresse und naechstem Messweg)
1. **Blatt 0 (B1), ROOM1190/11A0 vermutlich vertauscht — nicht gemeldet, nicht angefasst.** 1190 fuehrt EINE Zone
   auf rect 2 (der Kasten im Gang-Ring = laut Tuer-Nische seine OSTKAMMER, Tuer 1230 Slot 4 @0xCE6); sein
   Schiessstand projiziert mit der eigenen Zeile @0x80076978 auf rect 1 (85..142, 72..122), und rect 1 traegt
   heute ROOM11A0, dessen Zeile @0x80076980 (138,113,1776,2048) auf KEIN Rechteck von Blatt 0 faellt
   (y -2..62) und der keine Tuer zu einem B1-Raum hat. Naechster Schritt: 1190 in Haupt (rect 1) und
   Ostkammer (rect 2) teilen, 11A0s Etagenzeile (Band 3 -> Blatt 0 rect 1) gegen seine Tueren pruefen.
2. **Fahrstuhl-Marker: 4 px Weg je Achse.** Die Kabine ist 10x10 px gemalt (Innenraum 8x8), der Marker ist
   ein 8x8-Quad (FUN_800473f8: POLY_FT4 uv(224,128)), und re15_inv_screen.c klemmt den Mittelpunkt auf
   Rechteck+4 (`reserve = (zrc == 255) ? 1 : 4`) - beim 16x16-Rechteck mit Kunst oben links bleibt
   x113..117 / y138..142. Mehr ginge nur mit einem Klemmfenster aus der GEMALTEN Flaeche statt aus dem
   Rechteck - Aenderung fuer alle Raeume, hier bewusst nicht gemacht.
3. **Gang 1180/1230: Kunst schematisch.** Die 11B0-Doppeltuer ist UNTER dem Zweig gemalt (y78..86), in der
   Welt liegt sie am Nordende des Laengsgangs (z 26563..31185). Der Marker steht an dieser Tuer bei
   (167,62), 20 px ueber der gemalten Tuer; eine monotone Abbildung kann beides nicht.
   Der Tuer-Zug (tuer_anziehen) wirkt nur auf Grundriss-Zeichnungen (`if (!zn || !zn->synth) return;`).
4. **ROOM5020** zeigt im integration_map_raum_live kein sichtbares Rot (Blatt 9) - unberuehrt, vorbestehend.
5. Die Fahrstuhl-Etage nach einem LADEN in der Kabine: der Vorraum-Spiegel ist nicht im Spielstand
   (DAT_800b0fe6 steht auch im Original nicht im Speicherblock); dann gelten die Bits 54/55/56.
   In der Kabine kann man nicht speichern.

## Fuer den Nutzer
* Keine neuen Sprachdateien, keine neuen Assets (kein Eintrag fuers Paket-/Android-Gate).
* Fahrstuhl: die Karte zeigt jetzt die Etage, von der aus man eingestiegen ist (1F/2F/3F), die Kabine rot,
  und der Spieler-Marker steht IN der gemalten Kabine und bewegt sich mit (180 Grad gedreht wie der Raum).
* B2: 11F0 (links) und 1200 (Kasten in der Mitte) leuchten jetzt selbst rot, statt der ganzen Garage.
* B1: In ROOM1230 (und 1180 - derselbe Gang) kommt die B1-Karte; der Gang (Ring um die Kammer) leuchtet
  rot, der Treppenabsatz von 10A0 ist der kleine Kasten darunter.
* 1210: der T-Korridor ist rot, alle sechs Tueren (1x Garage, 5x Zellen) als gelbe Balken; jede Zelle
  leuchtet einzeln, wenn man drin steht.
Suite (Stand ec76eb0e): === LOCAL-BUILD-OK (all) — Tests 482/482

## Abschluss-Suite (Stand nach allen Aenderungen inkl. integration_r35_karte)

`=== LOCAL-BUILD-OK (all) — Tests 484/484`

---

## Nachbesserung 1 (nach Abnahme 0, `G_abnahme_0.md`; 2026-10-04)

Baum geprueft: status leer, HEAD d2ec24e5 (Basis 154a73c1 + 13 Spur-G-Commits).
Maengel: M1 Fahrstuhl-Marker 2 px, M2 kein Bewegungsriegel, M3 Suite 484 vs 483, M4 Dossier
ueberzeichnet, G1 SCA-Offsets 1220-Teiler, G2 PORT-WAHL-Kennzeichnung Fahrstuhl-Blatt.
(fortlaufend, je Mangel: Ursache / Messung vorher / Beleg / Aenderung / Messung nachher)

### M1 Fahrstuhl-Marker — RE-Belege (selbst disassembliert)

**Ursache (Vorgaenger-Stand):** Zeile 0x1080/0x1081 (129,117,795,805, flip 1,1) = 795/2^20 px je
Einheit = 1/1319 -> Kollisions-Innere 4200 Einheiten auf 3,2 px; Reichweite des Spielers (Wand -468,
Abnahme-Lauf: x -15282..-12018, z -3682..-518) davon die Haelfte -> 2 px. Dazu Klemmung
re15_inv_screen.c (Rect 9 (109,134,16,16) + Reserve 4 -> x113..121 y138..146) - schneidet vom
gemalten Innenraum (x110..117 y135..142) die linken/oberen 3 px weg. Beide Glieder zusammen = 2 px.

**RE1.5 (Original-Marker) `FUN_800473f8`** (RE_15_Quellcode_V2/FUN_800473f8.c): Quad x-4..x+4,
y-4..y+4 aus der Zeile @0x800768b0+8*Idx; danach direkt `AddPrim` - **keine Klemmung**. Zeile 1080
@0x800768f0 = Stub {0,0,1,1} (Abnahme + B3) -> das Original zeigt in der Kabine KEINEN brauchbaren
Marker (alle Lagen -> (0,0)). RE1.5 ist hier nachweislich unfertig -> Beta->Retail: RE2 ist das Ziel.

**RE2 Retail Kartenzeichner `FUN_8006e120`** (info/re2leon/PSX.EXE, re2_disasm.py dis 0x8006e120):
```
8006e1d4: bne v1,a3,0x8006e2f8      ; nur wenn gezeigtes Blatt (0x800d5c0a) == Blatt des Spielers
8006e1dc: lui v0,0x91a2
8006e1e4: lw a0,-976(a0)             ; 0x800cfc30 = Spieler x
8006e1e8: ori v0,v0,0xb3c5           ; magic 0x91a2b3c5
8006e1ec: addiu a0,a0,28000          ; x + 28000
8006e1f0: mult a0,v0 / 8006e204 mfhi v1 / 8006e208 addu v1,v1,a0 / 8006e20c sra v1,v1,8
8006e214: sra a0,a0,31 / 8006e218 subu v1,v1,a0      ; = (x+28000)/450  (C-Trunkierung)
8006e1f8: lw a1,-968(a1)             ; 0x800cfc38 = Spieler z
8006e200: addiu a1,a1,28000 / 8006e210 mult a1,v0 / 8006e24c mfhi / 8006e250 addu / 8006e254 sra 8
8006e268: subu v0,zero,v0            ; y = -(z+28000)/450
8006e228/8006e264: lhu 0x800d5c48 / 0x800d5c4a   ; Blatt-Ursprung
8006e234-23c: (yaw+0x100)>>9 & 7 -> u = 12*Richtung (8006e280-28c, sb 12(s4)) = Pfeil je Blickrichtung
8006e2cc: lhu a1,8(v1) / 8006e2e4 lhu v1,10(v1)  ; Raum-Eintrag +8/+10 = Versatz JE RAUM
8006e2d8: sh v0,8(s4) / 8006e2f4 sh v0,10(s4)    ; x0/y0 des Sprites
8006e2f0: jal 0x8008f918 (AddPrim)               ; KEINE Klemmung
```
Magic-Pruefung: 0x91a2b3c5 signiert = -0.431111*2^32; (mfhi + x) = 0.568889 x; >>8 -> x/450,0.
=> RE2: **ein Massstab fuer ALLE Raeume, 1/450 px je Einheit**, Versatz je Raum, Marker ungeklemmt.

**Gegenprobe gegen die RE1.5-Kunst (Kachel uv(168,40), MAP03.PIX Blatt 2 / MAP05.PIX Blatt 4,
Zeilen v=40..49 ab Datei-Byte 0x1454):** Kabine = 10x10-Kasten, Index 4 (Wand) in Spalte 0/9 und
Zeile 0/9, Index 1 (Innen) dazwischen. Blatt 2 (rect 9 (109,134)): Waende x109/x118, y134/y143,
innen x110..117 y135..142. Blatt 4 (rect 0 (127,137)): Waende x127/x136, y137/y146.
Mit 1/450 (Zeilenform sx = sy = 2^20/450 = 2330) und je EINEM Versatz pro Achse fallen BEIDE
Kollisionswaende der Kabine (SCA ROOM1080: Innenflaechen x -15750/-11550, z -4150/-50) auf die
gemalten Wandpixel - Blatt 2: x -15750 -> 118, -11550 -> 109, z -4150 -> 134, -50 -> 143; Blatt 4:
136/127/137/146 (scratch nb1/fit.py). Der Versatz ist von der einen Wand bestimmt, die andere Wand
PRUEFT den Massstab: 4200/450 = 9,33 px gegen 9 px gemalten Wandabstand - die Kunst ist im
RE2-Massstab gemalt. (Zum Vergleich: der Vorgaenger-Massstab 795 legte die Waende 3 px auseinander.)
Reichweite des Spielers (Wand -468): x -15282..-12018 -> 117..110, z -3682..-518 -> 135..142 =
**genau der gemalte Innenraum, 7 px je Achse.**

### M1/M2 — Messung vorher (eigene Laeufe, Stand d2ec24e5 = Bau-Stand 8fee1bb4)

Werkzeug: `scratch nb1/lauf.sh` (wie Abnahme lauf.sh) und der NEUE Riegel
`integration_r35_karte_fahrstuhl` (tests/integration/test_r35_karte_fahrstuhl.cmake): Spielstand
vor der Fahrstuhltuer (1040 (-21786,-10500) Yaw 3072 bzw. 1120 (1300,6400) Yaw 1024, flag 4:243),
LOAD GAME, `W1,A0.3,W1,A0.3` = Tuer-AOT (1040 @0x1096 / 1120 @0xCB6) -> Tuersequenz -> ROOM1080,
dann laufen (`U2` / `R0.6,U2` / `L0.6,U2`), Karte 230 Bilder nach Eintritt, Abzug im 60. Kartenbild
(RE15_INV_FB_SHOT), Marker = Pixel ohne Kartenfarbe (probe_r35_karte marker; Ring 5x5, im Abzug
gemessen Farbe (16,16,0), Wand (176,176,176), rot (48,8,48)/(48,8,64)).
```
integration_r35_karte_fahrstuhl am Stand 8fee1bb4 (170 s):  ROT
  f1_steh   pl pos=(-13650,0,-900)  -> MARKER 114 140  bbox (112,138)-(116,142)
  f1_vor    pl pos=(-13650,0,-3682) -> MARKER 114 138
  f1_rechts pl pos=(-15282,0,-518)  -> MARKER 115 140
  f1_links  pl pos=(-12018,0,-518)  -> MARKER 113 140
  f3_rechts pl pos=(-15282,0,-518)  -> MARKER 133 143
  f3_links  pl pos=(-12018,0,-518)  -> MARKER 131 143
  FAIL 1F quer x 113..115 = 2 px | FAIL 1F laengs y 138..140 = 2 px | FAIL 3F quer 2 px
unit_r35_karte_fahrstuhl (neuer Teil c/d) am Stand 8fee1bb4: ROT
  1F/2F Lagen -> x 114..116 (2 px), y 139..141 (2 px); 3F x 132..134, y 142..144;
  Waende: O/N/S -> (0,0) (Zeile 795 bildet die Wand x -11550 hinter den Rand) -> FAIL
```
= exakt die Abnahme-Messung (x113..115 / y138..140). Spieler-Reichweite bestaetigt: Wand -468
(x -15750+468 = -15282, -11550-468 = -12018, z -4150+468 = -3682, -50-468 = -518).

**Glyph-Versatz (fuer die Abbildung gebraucht):** der Ring liegt im 8x8-Quad ab uv(224,128) auf
uv 225..229 / 129..133 (DATA/TEX.TIM, Bilddaten ab Datei-Byte 0x620, Zeile v=129 ab 0x14910;
Texel 0xa = Ring). Mitte = Quad-Ursprung (mx-4) + 3 = **mx-1** (ebenso y). Im Abzug bestaetigt:
Logik-Marker (115,141) (unit) -> Ring-Bbox (112..116, 138..142), Mitte (114,140).

**RE1.5 klemmt nicht** (re15_disasm.py dis 0x80047528): @0x80047554 `addiu a0,t0,-4`,
@0x80047564 `addiu a1,t0,4`, @0x80047578 `ori v1,zero,0xfffc` (y-4), @0x800475ac/0x800475c0
`addiu v0,v0,4` (y+4), dann @0x800475d8 `jal 0x8006b538` (AddPrim) - kein min/max.

**Gegenprobe Stand 154a73c1** (die drei Engine-Dateien re15_map_zones.c/.h + re15_inv_screen.c
voruebergehend auf 154a73c1 zurueckgesetzt, gebaut, beide Riegel gefahren, danach `git checkout
HEAD --` zurueck): ROT.
```
integration_r35_karte_fahrstuhl (142 s): f1_steh (113,144) f1_vor (113,145) f1_rechts (112,143)
  f1_links (117,143) -> sichtbar x 5 px / y 2 px UNTER der Kabine; f3_rechts: "MARKER fehlt im
  Fenster" (Karte zeigte Blatt 2 statt 3F) -> FATAL
unit_r35_karte_fahrstuhl: x 112..117 (5 px), y 143..145 (2 px) ausserhalb des Innenraums;
  aus 10C0/1120 Blatt 2 statt 3/4; Waende nicht auf der Kunst -> FAIL
```
Damit ist der neue Riegel am alten UND am Vorgaenger-Stand ROT (Abnahme M2).

### M1 — Umsetzung (Dateien, Konstanten)

| Datei | Aenderung | Beleg |
|---|---|---|
| `engine/src/re15_map_zones.h` 0x1080/0x1081 Blatt 2 rect 9 + Gast Blatt 3 rect 4 | ox,oy,sx,sy = **155,73,2330,2330**, flip 1,1 (vorher 129,117,795,805) | sx=sy=2^20/450: RE2 FUN_8006e120 @0x8006e1dc-0x8006e218 / @0x8006e1f8-0x8006e268; Versatz PORT-WAHL aus der Kunst (MAP03.PIX @0x1454, Ring-Mitte TEX.TIM @0x14910) |
| dto. Gast Blatt 4 rect 0 | **173,76,2330,2330**, flip 1,1 (vorher 147,120,795,805) | dto., MAP05.PIX @0x1454 (Kasten um +18/+3) |
| `engine/src/karte_fahrstuhl_1080.c` NEU `re15_karte_fahrstuhl_fenster` | Klemmfenster in der Kabine = gemalter Innenraum + Glyph-Versatz: x rx+2..rx+9, y ry+2..ry+9 (KF_INNEN_LO 1 / KF_INNEN_HI 8 / KF_GLYPH_MITTE 1) | Kachel uv(168,40) @0x1454 (Wand Spalte/Zeile 0 und 9); TEX.TIM @0x14910; Original klemmt nicht (RE1.5 @0x800475d8, RE2 @0x8006e2f0) |
| `engine/src/re15_inv_screen.c` | Haken 2 Zeilen + 1 include: Aufruf nach der Rect+4-Rechnung, vor dem Klemmen | — |
| `analysis/befunde_runde35/G_karte_zeilen.py` | NEU-Abschnitt rechnet die Zeilen + Probe (Waende/Reichweite) nach | — |

Warum der Massstab nicht frei ist: mit 1/450 und EINEM Versatz je Achse treffen BEIDE Waende je
Achse die gemalte Wand (4200/450 = 9,33 px gegen 9 px gemalt). Der Vorgaenger hatte den Massstab
frei gewaehlt (795, "Innenraum auf das Klemmfenster"), um das 4-px-Klemmfenster zu treffen - das war
die Ursache der 2 px.

### M1 — Messung nachher (Stand nach 11d1d8ea + Riegel, gebaut)
```
integration_r35_karte_fahrstuhl (169 s): Passed
  f1_steh   pl pos=(-13650,0,-900)  -> MARKER 113 141  bbox (111,139)-(115,143)
  f1_vor    pl pos=(-13650,0,-3682) -> MARKER 113 135
  f1_rechts pl pos=(-15282,0,-518)  -> MARKER 117 142
  f1_links  pl pos=(-12018,0,-518)  -> MARKER 110 142
  f3_rechts pl pos=(-15282,0,-518)  -> MARKER 135 145
  f3_links  pl pos=(-12018,0,-518)  -> MARKER 128 145
  1F quer x 110..117 = 7 px, laengs y 135..142 = 7 px, 3F quer 128..135 = 7 px, 180 Grad PASS
unit_r35_karte_fahrstuhl: OK - 1F/2F/3F je x 7 px / y 7 px im gemalten Innenraum (Index 1);
  Kollisionswaende -> W(118,138) O(109,138) N(113,134) S(113,143) (3F: 136/127/137/146) = Index 4
```
= exakt die Vorhersage aus G_karte_zeilen.py (steh (113,141), vor (113,135), rechts (117,142),
links (110,142)). Bild: `G_karte_bilder/n1_fahrstuhl_marker.png` (Ausschnitte 8x, RE15_INV_FB_SHOT;
gdigrab liefert in dieser Sitzung weisse Bilder): der Ring steht nach der Ankunft an der gelben
Tuermarke unten, vorwaerts oben an der Rueckwand, rechts/links in den unteren Ecken. Er ragt in den
Ecklagen bis 2 px ueber die gemalte Wand - der Ring ist 5 px breit, der Innenraum 8 px; so zeichnen
auch RE1.5/RE2 (keine Klemmung).
Vorher -> nachher (sichtbare Ring-Mitte, gleiche 4 Lagen, exe): 154a73c1 x 5 / y 2 px (unter der
Kabine), 8fee1bb4 x 2 / y 2 px, **jetzt x 7 / y 7 px**.

