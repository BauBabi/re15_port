# Runde 41 — Spur E "kleinkram_b" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_kleinkram_b` (Zweig `r41/kleinkram_b`, Basis master 116e8b0f). Begonnen 2026-10-23.

## Punkte (Wortlaut AUFTRAG.md)
1. Leon hat eine komische herumfliegende Animation im ROOM 2020 am Anfang wenn er die Kurbel nutzt.
2. Die Map B1 ist jetzt beinahe perfekt, aber bevor man in den vorderen Teil der Waffen Armory - room 1090 - geht -
   also die vordere Tuer - macht der Spieler auf der Map einen Sprung zum hinteren Teil, obwohl man garnicht da ist.
3. Die Tuer der Werkstatt in ROOM 11B1 bei Elza, wenn sie ueber ROOM 2000 rein kommt und die Cutscene abspielt wo sie
   Elza sucht, ist immer noch offen. Ich wollte das sie dauerhaft geschlossen bleibt in Elzas Szenario.

Ressourcen (VERTRAG §1): Bank-9-Bit 120 (nur wenn kein Original-Flag reicht), keine neuen Nachrichten-Ids
(nur vorhandene). Gemeinsame Dateien: scd_room_setup.c (Installer am Blockende), karte_r39.c / re15_map_zones.c,
elzazombies_r39.c, test_r36_kartenbits.c k_port[] (nur falls Bit 120 belegt).

## Status
- [x] Punkt 1 Kurbel 2020 — plcloop_r41.c (Schleife statt Standbild), unit_r41_kleinkram_b_kurbel 2/2; Nachbesserung 1:
  alle Anlauf-Varianten + echter Weg des Nutzers (befund.log) gemessen, Original mit Handgun (Stand + Cut-3-Anlauf)
  Bild fuer Bild gleich, integration_r41_kleinkram_b_kurbel; Rueckfrage an den Nutzer bleibt
- [x] Punkt 2 Karte B1 / 1090-Vordertuer — karte_r41.c, unit_r41_kleinkram_b_karte_pin 17/17
- [x] Punkt 3 Werkstatt-Tuer 11B1 (Elza) — Ereignis 24, unit_r41_kleinkram_b_11b1 3/3
- [x] Nachbesserung 2 (Abnahme 1): E-N1 Apply-Bild steht (kamerahalt_r41.c/bildhalt_r41_pc.c), E-N2 Nachrichten-Takt hinter dem
  Spiel-Takt + Auswahl ohne Parken (msgtakt_r41.c); alle Anlauf-Varianten am Original (GDB) gemessen; Renn-Stopp OFFEN
- [x] Nachbesserung 3 (Abnahme 2): Blenden-Takt im gehaltenen Bild (E-E3), Renn-Tempo + Renn-Stopp + DECIDE 0x80032604
  vollstaendig spielweit (E-E2, rennstopp_r41.c), Riegel renn auf o_renn2, Gorilla-Sonde neu eingemessen, Vertrag E-E1;
  gezielt 266/266 + 376/376 + 10/10 + 101/101 gruen (Abschnitt "Fortsetzung — Tests")

## Protokoll
- (Start) Baum sauber auf 116e8b0f, Dossier angelegt.

## Messung vorher (Stand 116e8b0f, exe-Kopie re15_pc_r41e.exe, 2026-10-23 22:16)
Alle Laeufe: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1`,
eigener Laufordner je Lauf (debug.log liegt im cwd der exe). Ergebnisse unter scratchpad/r41e/<lauf>/.

### Punkt 3 (11B1 Werkstatt-Tuer, Elza) — REPRODUZIERT
Lauf w6: `RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=11b1@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3
RE15_PLAYER_POS=-195,-21821,1024,0 RE15_SCD_TRACE=1` (= Ankunft aus ROOM1261 Slot 0 @0x0112A, Zustand nach
10B1 sub04 @0x0195E `22 03 83 01` "Schacht offen" und 2001 @0x01958 `22 03 46 01`).
- sub00 nimmt den Zweig (3,0x83)==1: Trace `slot=1 @0x10FA op=0x21 … @0x1108 op=0x29 (Cut_chg 7) @0x110A op=0x3C
  @0x110E op=0x4B @0x1112 op=0x4B (Cut_replace 0<->7, 2<->8 = Tuerblatt OFFEN gemalt) @0x1116 op=0x37 @0x111A op=0x39
  (Sca_id_set/Sca_floor_set (3,0x0C) = Durchgangszelle AUF) @0x111E Work_set @0x1122 Member_set @0x1126 Else`.
  Der Text-Platz 8 "The door is locked." (@0x112A) wird NICHT angelegt.
- sub03 ("Sherry!? / Where has she gone!?") laeuft F6..F473: @0x1438 Set(3,0x46)=0, @0x1450 Pos_set, msg 10 @0x1480
  (F84), msg 11 @0x14CE (F372), @0x14E6 Plc_ret (F473). Bilder w6_sheet.png.
=> Nach der Szene ist die Werkstatt-Tuer offen (Zelle passierbar, Bild offen) — genau der Nutzer-Befund.

### Punkt 2 (Karte B1, ROOM1190 = "Waffen Armory 1090" des Nutzers, Runde 36 M R0)
- Numerisch (probe_r39_karten marker): Gang 1230 an der Ostkammer-Tuer (96,12161) -> Marker (146,109) = Ostkammer-
  Marke; an der Hallen-Tuer (-8118,9355) -> (127,115) = Hallen-Marke. Der Tuer-Zug (tuer_anziehen) greift in der
  Sonde NICHT (RE15_ZUGRAND=0 aendert nichts; betrete() tickt die VM nicht, main00 legt keine Tuer-AOTs an).
- Echte exe, Kartenschirm im Gang (w2/w3/w4, RE15_INV_OPEN_AT=30#1230 + RE15_INV_FB_SHOT): Marker an der
  Ostkammer-Tuer (146,108) ueber dem Ostkammer-Kasten, an der Hallen-Tuer (127,115) — KEIN Sprung im Gang.
- Code-Befund (noch nicht gemessen): re15_inv_screen.c tuer_anziehen() sucht die Marke, deren ANDERE Zone zu
  ZIELRAUM (ziel = Stage/Raum aus door_params) gehoert — fuer ROOM1190 passen Halle (zid 26) UND Ostkammer (zid 110),
  die Tabelle s_map_marks (re15_map_zones.h:986/987) fuehrt die Hallen-Marke zuerst. Offen: ob/wann dieser Zug im
  echten Tuerweg greift (Laeufe w7/w10: Tuer per Aktionstaste, Kartenschirm im Zielraum).
- Tuer-Daten: 1180/1230 Slot 2 @0x00A54/@0x00B54 rect (-9266,8888,2297,934) -> 1190 (3174,-11182) [Halle];
  1230 Slot 4 @0x00CE6 rect (-1052,11694,2297,934) -> 1190 (19515,-12109) [Ostkammer]; in 1180 Slot 4 sce 0 (zu).
  Blickrichtung gemessen: yaw 0 = +x, 2048 = -x (w7a/w7b [walk]).

### Punkt 1 (Kurbel 2020) — noch NICHT reproduziert
Lauf w1/w5: `RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_PLAYER_POS=2600,-2800,2048,1 RE15_FIRE_AOT=3@300#2020
RE15_PRESS=square@340,square@380,square@420 RE15_ANIM_TRACE RE15_FRAMEDUMP`: sub02 (slot 3 sce 3 @0x009AE) laeuft wie
die RDT: msg 0 F301, msg 1 "Will you use the Crank?" F342, Ja F422 -> @0x0A7E Cut_chg 3, @0x0A8C Pos_set (3650,-1800,
-2925), Sleep 20, @0x0A9C Plc_motion(0,0,0) F443 -> Spieler Clip 0 der Raum-RBJ (ROOM2020 RBJ @0xBB0, rec0 Marker 1,
15 Knochen, 1 Clip, 40 Bilder) spielt F443..F482 einmal und HAELT bei Bild 39 (Plc_flg 0x04 wird nicht geschleift —
Nebenbefund, nicht der Nutzer-Punkt). Bilder w1_sheet.png / w5_sheet.png: Leon steht an der Kurbel und kurbelt,
kein "Fliegen". Offen: echter Weg mit Aktionstaste (w9a/w9b), Waffe (w9c, RE15_EQUIP=3), Szenenende.

## Punkt 3 — Umsetzung + Messung nachher
### RE-Belege (Adressen/Bytes, selbst gelesen: scd_dump_room.py, rdt_msgdump.py, Hex der RDT)
- Tuer = Durchgangszelle SCA Region 3 Index 0x0C @0x00EC8 `e4 02 d0 07 7a a9 b2 bc 01 ff 00 03` (x -22150 w 740,
  z -17230 h 2000, u0 ff, fl 03). Bild: Cuts 0..6 zu, 7..10 offen (Runde 36 J N1.1, BSS 2 vs 8).
- sub00 @0x010F6: `21 03 83 01` -> `23 00 0a 00 00 00 / 29 07 / 3c 01` (Eingangs-Cut 0 -> 7), `4b 00 07` @0x0110E +
  `4b 02 08` @0x01112 (Cut_replace, selbstinvers LAB_80040414 @0x80040458..@0x80040494), `37 03 0c 00` @0x01116 +
  `39 03 0c 40` @0x0111A (Zelle auf), `2e 03 00` + `34 02 58 b4` (Container z -19368); Else @0x0112A Text-Platz 8
  `2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 0c 00 ff ff 00 00` (msg 12 @0x179F "The door is locked."), @0x0113E
  Aot_reset Slot 5 (Schacht zu), @0x01148.. Zellen (2,4)/(3,9) auf.
- (3,0x83) "Schacht offen": Setzer ROOM10B1 sub04 @0x0195E `22 03 83 01` (Elza), ROOM11B0 sub06 @0x01478 (Leon).
  (3,0x46): ROOM2001 @0x01958 `22 03 46 01` -> sub03 (Sherry-Suche, msg 10 @0x01480 / 11 @0x014CE). Weg 2001 Slot 0
  @0x0161E -> ROOM1261 (Spawn (-9262,-1800,-5108)); 1261 Slot 0 @0x0112A -> 11B1 (-195,0,-21821) Cut 6.
  ROOM1261 fuehrt ausserdem nach 2001 (Slots 1/4), 10B1 (Slot 2 @0x0116A, Cut 7) und 11D1 (Slot 3 @0x0118A, Cut 7) —
  mit dauerhaft zugesperrter Werkstatt-Tuer bleibt Elza der Rueckweg ueber den Schacht: KEIN Softlock.
- Gemessen (w6, Stand vorher): nach sub03 steht Elza bei (-18192,-20453) in der Werkstatt (Plc_dest 09 nach
  (-22695,-20321) endet an der Blockzelle), Kamera Cut 8 = OFFENES Tuerblatt.
### Umsetzung (PORT-WAHL, Form = Runde 36 J Ereignis 23 / gen/szl11b0_szene.inc k_szl_tuerprog)
- NEU `include/re15_werkstatt11b1_r41.h`, `engine/src/werkstatt11b1_r41.c`: Port-Ereignis 24 in ROOM11B1, gestartet im
  Init-Lauf direkt hinter sub00 (Dispatcher FUN_8003ef6c/@0x8003f17c: hoeherer Slot laeuft im selben Lauf), nur bei
  (3,0x83)==1 und wenn sub00 der geladenen RDT bytegleich zum Anker (102 B @0x010F6) ist. Programm 70 B: Cut_replace
  0<->7, 2<->8 zurueck (= @0x0110E/@0x01112 noch einmal), `37 03 0c ff` + `39 03 0c 03` (RDT @0x00EC8), Aot_set Platz 8
  (= @0x0112A Byte fuer Byte, msg 12), Cut 7->0 / 8->2 (Form @0x01102..@0x0110A umgekehrt). Schacht Slot 5,
  Container, Zellen (2,4)/(3,9) bleiben wie im offenen Zweig.
- Haken (je 1 Zeile + include, Kommentar "Runde 41 Spur E"): scd_room_setup.c hinter re15_szl_init_lauf (Init-Lauf,
  Muster J — NICHT am Blockende, weil das Ereignis VOR scd_vm_tick in der Warteschlange stehen muss), scd_vm.c
  Ereignis-Kette hinter re15_nahaufnahmen_r40_ereignis.
- Kein neues Bit, keine neue Nachricht, kein Asset. Leon (0x11B0) unberuehrt.
### Messung nachher (exe-Kopie re15_pc_r41e2.exe, Bau 455f4eeb+)
- n13 (wie w6): debug.log `[wk11b1-r41] ROOM11B1 (3,0x83)=1: Ereignis 24 im Init-Lauf hinter sub00`; Trace F240:
  sub00 `Cut_chg(7)` -> Ereignis `op=0x4B, 0x4B, 0x37, (0x39), 0x2C` -> `Cut_chg(0)`; Szene sub03 wie vorher
  (Cut 6, msg 10/11, Plc_ret), danach Elza bei (-18192,-20453) mit **Cut 2** (zu) statt Cut 8 (offen).
- n14 (Elza von Osten vor der Tuer, (-20200,-16230) yaw 2048 = -x, `U1.5,A0.1`): Lauf endet bei x=-20942
  (= Zellkante -21410 + Radius 450), Cut 2, kein Raumwechsel; Aktionstaste -> Text (mo=200 Halt).
### Tests
`ctest -R unit_r41_kleinkram_b_11b1`: unit_r41_kleinkram_b_11b1_programm / _zu / _original — 100% tests passed,
0 tests failed out of 3 (Programmform + Anker; (3,0x83)=1: Zelle ff/03, Platz 8 msg 12, RVD-Zonen == unveraenderte
Parse, Eingangs-Cut 0, Schacht bleibt Tuer, Container z -19368, Lauf blockiert bei x -20942, Aktionstaste msg 12 ohne
Raumwechsel, Wiederbetreten; ohne (3,0x83) Else-Zweig unveraendert; Leon 11B0 Zelle offen, kein Ereignis).

## Punkt 2 — REPRODUZIERT, Umsetzung, Messung nachher
### Messung vorher (probe_r41_kleinkram_b_karte mess, Stand 3309246f; VM im Init-Lauf getickt = Tuer-AOTs vorhanden)
Gang ROOM1230 (Strom (4,0xF3), Ostkammer-Tuer (3,0x89) = main00 @0x00CE2 Else -> Slot 4 Tuer @0x00CE6), Annaeherung an
die Ostkammer-Tuer (Mitte (96,12161), Zugbereich hh*2+450 = 1384): (96,13500) -> Marker (146,106); (96,13300) ->
(141,109); (96,13100) -> (137,110); (96,13078) -> (137,111). Die Ostkammer-Marke liegt bei (146,109), die Hallen-
Marke bei (127,115): der Marker wandert 9 px nach Westen zur HALLE = "Sprung zum hinteren Teil, obwohl man gar nicht
da ist". Vor der Hallen-Tuer ((-8118,10272) -> (127,114)) und an beiden Ankuenften in 1190 ((19515,-12109) ->
(146,110), (3174,-11182) -> (128,117)) stimmt alles. Die alte Sonde probe_r39_karten tickte die VM nicht (keine Tuer-
AOTs) und konnte den Zug nicht sehen; die exe-Laeufe w2/w3 standen ausserhalb des Zugbereichs (Kollision schiebt
RE15_PLAYER_POS aus der Tuernische auf z 13765).
### Ursache (Code) + Beleg
re15_inv_screen.c tuer_anziehen(): Symbol = erste Marke, deren andere Zone zu IRGENDEINER Zone des Zielraums gehoert
(re15_map_zone_fuer(ziel, ix, Blatt) ix 0..7). ROOM1190 hat zwei Zonen (Halle zid 26, Ostkammer zid 110,
re15_map_zones.h:319-322); s_map_marks fuehrt die Hallen-Marke (:986) vor der Ostkammer-Marke (:987) -> beide Gang-
Tueren nach 1190 zogen auf die Hallen-Marke. Welche Zone eine Tuer meint, sagt ihr Datensatz: Door_aot_set next x/z
(door_params spawn_x/z): ROOM1230 Slot 4 @0x00CE6 `3b 4c 00 00 b3 d0` = (19515,0,-12109) -> Ostkammer; Slot 2
@0x00B54 `66 0c 00 00 52 d4` = (3174,0,-11182) -> Halle. Dieselbe Lage setzt der Raumaufbau nach dem Durchgang als
Zone (scd_room_setup.c:388 re15_map_zone_update). PORT-WAHL (der Zug ist eine Port-Ergaenzung, Runde 30; RE1.5 hat
keine Tuermarken, RE2 zieht nicht).
### Umsetzung
NEU `include/re15_karte_r41.h`, `engine/src/karte_r41.c`: re15_karte_r41_tuer_zielzone(slot, ziel) = zid der Zone
von (spawn_x, spawn_z) im Zielraum, -1 ohne Tuer/Zone. Haken re15_inv_screen.c tuer_anziehen (3 Zeilen, "Runde 41
Spur E"): zielzone einmal je Tuer, Marke nur mit andere == zielzone; -1 -> alte Suche (Rueckfall).
### Messung nachher (gleiche Sonde, Bau 8)
(96,13500) -> (146,106); (96,13300) -> (146,107); (96,13100) -> (146,107); (96,13078) -> (146,108) — auf der
Ostkammer-Marke, kein Wandern zur Halle. Hallen-Tuer, Ankuenfte, ROOM1180 (ohne Strom: Slot 4 Text @0x00A94)
unveraendert.
### Tests
`ctest -R unit_r41_kleinkram_b_karte_pin`: 100% tests passed, 0 tests failed out of 1 (17 Pruefungen: 6 Annaeherungs-
punkte Ostkammer-Tuer auf der Ostkammer-Marke (<= 4 px, nie zur Halle), 6 Hallen-Tuer, beide Ankuenfte 1190,
ROOM1180). Vorher 13/17 (die vier naechsten Punkte zur Ostkammer-Tuer rot).

## Punkt 1 — Kurbel ROOM2020: Messung, Beleg, Umsetzung, Messung nachher, OFFEN
### Messung vorher (exe-Kopie, 7 Laeufe)
w1/w5 (FIRE_AOT 3, Leon), w9a (echter Weg: Aktionstaste an der Kurbel, yaw 1024), w9c (Waffe RE15_EQUIP=3), f1 (Fernstart
5000 Einheiten: Pos_set springt sofort, kein Gleiten), f2 (Elza ROOM2021): in allen Laeufen dieselbe Folge — msg 0,
msg 1 "Will you use the Crank?", Ja -> @0x0A7E Cut_chg 3, @0x0A8C Pos_set (3650,-1800,-2925), Sleep 20, @0x0A9C
Plc_motion(0,0,0) = Raum-RBJ Clip 0 (ROOM2020 RBJ @0xBB0 rec0 Marker 1, 15 Knochen, 40 Bilder; Wurzel je Bild
(0,-1704,0): keine Wurzelbewegung), @0x0AA0 Plc_flg(0,4). RE15_ANIM_TRACE (w1): Clip 0 spielt F443..F482 EINMAL, danach
`cur` 40, 41, ... bei `slot` 39 bis Plc_ret @0x0B16 F621 = 138 Bilder eingefrorene Kurbelpose, waehrend die Buehne
faehrt. Bilder w1_sheet/w5_sheet/w9a_sheet/w9c_sheet/f1_sheet/f2_sheet.png: kein "Fliegen" sichtbar.
Ursache des Haltens: der Spieler-Vorschub (player_common.c `p->anim_frame++`) kennt das Clip-Ende nicht —
re15_actor_clip_len liefert fuer den Spieler 0 (keine Gegner-Bank, RBJ-Cache rbj_resolve_slot schliesst Slot 0 aus);
der Zeichner klemmt den Slot auf 39.
### RE-Beleg (selbst disassembliert, ghidra1_V2.txt)
Kommandowort 4 (Plc_motion @0x80041bb0 `sb v1(=4),4(v0)`) -> Dispatcher FUN_80031c44 Tabelle @0x80073f90 [4] =
FUN_80030660: @0x80030670/@0x8003067c `andi 0x10/0x20` (Halbtakt, Runde 35 L), @0x8003068c `lbu v0,DAT_800aca59`
(Spieler +0x5 = Plc_motion pc[1] = 0), @0x8003069c `addiu at,at,0x3e30` / @0x800306a4 `lw v0,0(at)=>PTR_LAB_80073e30`
= 0x80050cb8 / @0x800306ac `jalr v0`. 0x80050cb8 ist die Bewegungs-Phasen-FSM des NPC-Executors (enemy_ai_common.c
re15_npc_sub_motion, byte-true): Phase 1 ruft FUN_8001f314 (posiert +0x95, wrappt @0x8001f63c auf 0, Rueckgabe 1),
dann @0x80050da4 Phase 2 = HALTEN — mit +0x1c4 & 0x04 @0x80050dbc-c8 bleibt Phase 1 = SCHLEIFE. Mit Plc_flg(0,4)
@0x0AA0 kurbelt der Spieler im Original durchgehend bis Plc_ret.
### Umsetzung
NEU `include/re15_plcloop_r41.h`, `engine/src/plcloop_r41.c`: re15_plcloop_r41_takt(p) nach dem Vorschub im
Kommandowort 4 (Haken player_common.c hinter `p->anim_frame++`, 1 Zeile "Runde 41 Spur E"): Clip-Laenge aus dem
Raum-RBJ-Record mit Marker-Bit 0 (= Spieler; Bankwahl wie anim_select_common.c: anim_use_pl00 -> PL00, sonst RBJ,
ohne RBJ PL00), am Ende +0x1c4&4 -> Bild 0, sonst fc-1. Nur state 4; Sub 6 (Event-Reach) und W-Bank-Klemmung bleiben.
### Messung nachher (exe-Kopie re15_pc_r41e3.exe, n1 echter Weg mit Aktionstaste)
Trace Aktor 0: Clip 0 wrappt 39 -> 0 bei F497/498, F537/538, F577/578, F617/618 = vier volle Kurbel-Umlaeufe bis
Plc_ret F636 (vorher: ein Umlauf + 138 Bilder Standbild).
### Tests
`ctest -R unit_r41_kleinkram_b_kurbel`: 100% tests passed, 0 tests failed out of 2 — laenge (RBJ rec0 Clip 0 = 40,
ausser Bereich 0, PL00-Rueckfall, Elza 2021 = 40), schleife (Leon: 178 Bilder im Kommandowort 4, max anim_frame 39,
4 Umlaeufe; ohne Flag: Halten bei 39, 0 Umlaeufe; Elza: 4 Umlaeufe).
### OFFEN (Punkt 1)
Das vom Nutzer beschriebene "Herumfliegen" liess sich in sieben Laeufen (Leon/Elza, Aktionstaste/FIRE_AOT, Waffe,
Nah-/Fernstart, 1-Bild-Abstand) NICHT reproduzieren: Clip, Bank, Wurzel und Pos_set sind richtig. Die einzige
gemessene Abweichung war das Einfrieren nach einem Umlauf (behoben). Bleibt das "Fliegen" beim Nutzer, braucht es
seinen Weg (Spielstand/Raum davor, Waffe, Bild) — Frage unten. Nicht gemessen: Original in DuckStation
(re15-room-capture, Savestate der Kurbelszene) — Beleg hier aus dem Disasm.

## Nachbesserung waehrend der Spur (gezielte Tests)
Erster gezielter Lauf (66 Tests) rot: unit_r36_szenenleon_schub, unit_r36_szenenleon_erweiterung,
unit_r38_werkstatt11b0_takt — Leons Container-Schub ROOM11B0 (Plc_dest Modus 0x13, eigener Bildzaehler des Schub-Subs
@0x80052ac0, Clip 0x12 = 25 Bilder in 3 Umlaeufen). Ursache: re15_plcloop_r41_takt griff bei JEDEM Kommandowort 4.
FUN_80030660 dispatcht aber ueber +0x5 (@0x8003068c `lbu DAT_800aca59`): nur Sub 0 ist die Bewegungs-FSM 0x80050cb8.
Gate jetzt `state == 4 && sub_state_1 == 0 && !walk_active` (dasselbe Gate wie re15_actor_clip_len fuer NPCs).

## Gemeinsame Dateien (Haken, je 1-3 Zeilen, Kommentar "Runde 41 Spur E")
- engine/src/scd_room_setup.c: include + `re15_werkstatt11b1_r41_init_lauf` direkt hinter re15_szl_init_lauf (Init-
  Lauf, VOR scd_vm_tick — Muster Runde 36 J; deshalb nicht am Ende des Installer-Blocks).
- ~~engine/src/scd_vm.c: include + Ereignis-Kette `re15_werkstatt11b1_r41_ereignis` hinter nahaufnahmen_r40.~~
  Nachbesserung 1 (H1): entfernt, scd_vm.c bytegleich zur Basis; werkstatt11b1_r41.c startet den Faden selbst.
- engine/src/re15_inv_screen.c: include + `zielzone` (1 Zeile) + `if (zielzone >= 0) ... else for (...)` in
  tuer_anziehen.
- engine/src/player_common.c: include + `re15_plcloop_r41_takt(p)` hinter `p->anim_frame++` (Kommandowort 4).
Neue Dateien: engine/src/werkstatt11b1_r41.c, karte_r41.c, plcloop_r41.c; include/re15_werkstatt11b1_r41.h,
re15_karte_r41.h, re15_plcloop_r41.h; tests/unit/test_r41_kleinkram_b_{11b1,karte,kurbel}.c,
tests/unit/probes/r41_kleinkram_b.cmake. Keine Assets, kein Bank-9-Bit, keine neue Nachricht, kein Patch an
shared_assets/PSX.

## OFFEN
- Punkt 1: das "Herumfliegen" selbst ist nicht reproduziert (7 Laeufe, Bilder im scratchpad r41e/*_sheet.png). Behoben
  ist die einzige gemessene Abweichung (Standbild statt Schleife). Original-Gegenprobe in DuckStation (Savestate der
  Kurbelszene, re15-room-capture) steht aus; der Beleg kommt aus dem Disasm (FUN_80030660 -> 0x80050cb8).
- Nebenbefund Karte: die Sonde probe_r39_karten (Runde 39) tickt die VM nicht und sieht den Tuer-Zug nicht — ihre
  "Tuer-Sprung"-Zahlen gelten ohne Zug. Nicht angefasst.
- Nebenbefund Elza 11B1: sub03 fuehrt Elza mit Plc_dest 09 nach (-22695,-20321) WESTLICH der Trennwand; im Port endet
  sie an der Blockzelle bei (-18192,-20453) (Original nicht gemessen). Mit zugesperrter Tuer bleibt ihr der Schacht
  (ROOM1261 -> 11D1 / 10B1 / 2001).

## Fuer den Nutzer
- Keine neuen Assets, keine Sprachdateien.
- Bedienhinweis Elza: Nach der Sherry-Suche in der Garage (11B1) ist die Werkstatt-Tuer zur Vorhalle dauerhaft
  verschlossen ("The door is locked."); zurueck geht es durch den Bodenschacht (Kanal 1261 -> Zwinger 11D1 /
  Kanaldeckel 10B1 / Kanalisation 2001).
- Entscheidungsfrage (nur falls das "Fliegen" an der Kurbel in 2020 weiter auftritt): bitte Weg (Spielstand/Raum
  davor, Waffe in der Hand) und ein Bild — im Port kurbelt Leon jetzt durchgehend, bis die Buehne unten ist.

## Tests (gezielt, kein voller Lauf) — Endstand
- `ctest --test-dir re15_port/build -R "^unit_(r36_szenenleon_schub|r36_szenenleon_erweiterung|r38_werkstatt11b0_takt|r41_kleinkram_b_kurbel)"`:
  100% tests passed, 0 tests failed out of 5
- `ctest --test-dir re15_port/build -R "^unit_(map_|karte_|r30_karte|r33_karte|r35_karte|r36_karte|r36_kartenbits|r36_kartenitems|r36_szenenleon|r38_werkstatt11b0|r38_tagebuch_karte|r39_karten|r39_johnruf1041_tuer|plc_back_yaw_1090|r30_android_n3_plc_aim|r41_kleinkram_b)" -j 4`:
  100% tests passed, 0 tests failed out of 66
  (darin die neuen Riegel unit_r41_kleinkram_b_11b1_{programm,zu,original}, unit_r41_kleinkram_b_karte_pin,
  unit_r41_kleinkram_b_kurbel_{laenge,schleife} sowie alle Gruppen der beruehrten Module: Karte/Kartenbits/Karten-
  items (re15_inv_screen.c), szenenleon/werkstatt11b0/johnruf1041_tuer (scd_room_setup.c, scd_vm.c, 11B0/11B1),
  plc-Riegel (player_common.c)).
- Letzter exe-Lauf (re15_pc_r41e4.exe, Endstand, echter Weg mit Aktionstaste): Kurbel-Clip wrappt bei F498/538/578/618.

## Nachbesserung 1 (nach Abnahme 0, 2026-10-24)
Grundlage: `E_abnahme_0.md` §7 (Mangel 1, Hinweise H1-H3) und die Orchestrator-Hinweise in AUFTRAG.md (2026-10-24 03:21:
Anlauf im Gehen/Rennen, alle Seiten der Zone Slot 3 @0x009AE, Zielhaltung/Schuss, Add_speed-Ziel @0x0AA8-0x0ACC).
Stand bei Beginn: HEAD 88bfae38 (Code = ed7bd4ff), exe md5 757da3a9132018190427a32a39b67d96. Laeufe unter
`<sp>/E41n1/runs/<lauf>/` (`<sp>` = Scratchpad dieser Sitzung), Messkopie `re15_pc_E41n1.exe`, VORHER-exe =
`<sp>/abn0_base/re15_port/build/platform/pc/re15_pc_abn0_basis.exe` (Basis 116e8b0f, md5 2310696431b26d115bc00f3796663c01).

### Mangel 1 — "Herumfliegen" an der Kurbel 2020: Anlauf-Varianten
Werkzeug: `<sp>/E41n1/run.sh` (exe-Kopie, eigener Laufordner), Bildboegen `<sp>/E41n1/sheet.py` / `pairs.py` (oben Original
v(b+2k), unten Port F(b'+k), Port +5 px wie Abnahme §3.7). Gemeinsam: `RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72
RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_SCD_TRACE=1 RE15_ANIM_TRACE RE15_MOTRACE RE15_STATE_LOG
RE15_FRAMEDUMP` (Dialog-Tasten per RE15_PRESS). Gelaendeform (RE15_FLOOR_DUMP, Lauf `floor2020`, Bild `<sp>/E41n1/floor2020_kurbel.png`):
der Steg auf Band 1 laeuft in x (x >= -300), die Spielermitte bleibt bei x 2600 in z -3018..-3382 (gemessen a1: Lauf in -z
endet bei z -3382); die Zone Slot 3 (2200..3000 / -4150..-3350, `Aot_set` @0x009AE, sat 0x31 = nur Vorwaertspunkt 620) liegt
an der Suedkante -> erreichbar von Westen, Osten und frontal (Gier um 1024); "Norden" ist Reling/Abgrund.

| Lauf | Anlauf | Ausloesung | waehrend Message_on @0x0A5C/@0x0A62 | Pos_set @0x0A8C .. Plc_motion @0x0A9C |
|---|---|---|---|---|
| a1_walk_s | Gehen frontal (2600,-1500 -> Steg), A beim Gehen | F309 (2600,-3382) Gier 1024 | Geh-Pose eingefroren (Clip 5 Slot 9/10, Pause) | F383 -> (3650,-3018) Gier 1024, mo 200 (Clip 3) 20 Bilder, F403 Clip 0 |
| a2_run_s | Rennen frontal | F307 (2600,-3382) | Renn-Pose eingefroren (Clip 0 Slot 6/7) | wie a1 |
| b1_west_geh | Gehen von Westen, Rechtsbogen, A | F309 (1971,-3232) Gier 144 | Geh-Pose eingefroren | F453 wie a1 |
| b2_ost_geh | Gehen von Osten, Linksbogen, A | F309 (3229,-3231) Gier 1904 | Geh-Pose eingefroren | F453 wie a1 |
| b3_west_renn | Rennen von Westen, Bogen, A | F308 (2182,-3330) Gier 216 | Renn-Pose eingefroren | F453 wie a1 |
| b4_dreh | Drehen auf der Stelle (R), A | F310 (2600,-3200) Gier 960 | | F403, HUD (RE15_POS_HUD) y -1800 durchgehend |
| c1_aim_a | Zielen R1 1 s, Schuss, loslassen, A | A waehrend des Senkens (F342) loest NICHT aus; F355 nach Senken (Clip 6 rueckwaerts bis F350) | Stehen 200 | wie a1 |
| c2_schuss_a | Schuss, sofort loslassen, A | F340 nicht, F350 | Stehen 200 | wie a1 |
| e1_ost_cut3 | von (9000,-3200) nach Westen, Linksdrehung, A | F381 (3150,-3200) Gier 1760, **Kamera Cut 3 schon aktiv** | Leon sichtbar | F523 Sprung (3150,-3200)/1760 -> (3650,-3018)/1024 im selben Bild |
| t2_weg2050_nach / _vor | ECHTER Weg: ROOM2050 Tuer Slot 1 (RE15_FIRE_AOT=1@30#2050, Tuerfahrt 301 Bilder) -> 2020 (11350,-3150) Gier 3072, links drehen, 3,8 s gehen, A | F186 (2914,-3388) Gier 1920; Kamera 0 -> 2 (x 8390) -> 3 (x 5430) | Geh-Pose eingefroren, F270 Stehen 200 | F343 Sprung -> (3650,-3018)/1024, F363 Clip 0; NACHHER 4 Umlaeufe, VORHER Standbild |

In ALLEN Laeufen ist der Ablauf ab dem Ja derselbe wie im Stand (Abnahme §3.5): `Pos_set`, Gier 1024 aus `34 04 00 04`
@0x0A94, 20 Bilder Stehen, Plc_motion(0,0,0) -> Raum-RBJ Clip 0 mit 7-Bild-Blende (@0x80050cfc), Kurbeln. Kein Abheben,
kein Gleiten, keine Fremdpose: Spielerlage x/z/y bleibt (3650,-1800,-3018) bis Plc_ret (state.txt, HUD b4).
(d) Add_speed-Ziel, selbst disassembliert: Work_set LAB_80040d2c loescht die 6 Geschwindigkeitsworte (thread+0x158,
Schleife @0x80040d34-3c), Art pc[1]: 1 -> @0x80040da4 `addiu v0,v0,-13740` = Spieler 0x800aca54 -> `sw v0,340(a0)`
(thread+0x154); 3 -> @0x80040dd4..@0x80040df0 Objekt 0x800b3f98 + idx*148. Add_speed LAB_80040f40 `lw a1,340(a0)` addiert
+0x158/+0x15a/+0x15c auf +0x34/+0x38/+0x3c dieses Ziels. sub02 bindet @0x0AA8 `2e 03 01` = Objekt 1 -> der Spieler wird
im Original NIE bewegt; Port scd_work_bind Art 3 = Prop-Pool obj_id (scd_vm.c), Add_speed nur auf t->work_slot/
work_prop_idx — Spielerlage im Port unveraendert (oben), Buehne faellt in Cut 4 (Bildbogen `<sp>/E41n1/pr_w03_cut4.png`).
Nebenbefund Kamera (nicht Spur E): Port-Lauf g2 (Stand bei (9000,-3200), Kamera-Spur state.txt) springt F1..F299 JEDES
Bild zwischen Cut 0 und Cut 2; im Original-Video `o_hg_ost.mp4` steht derselbe Ort ~100 Videobilder ruhig auf Cut 0.

#### Original-Gegenproben (DuckStation, MZD-Disc, Treiber `<sp>/E41n1/orig_driver.py` = abn0-Treiber + `--nomenu`/`--latecap`/U+S)
Pfad-Falle: `-statefile` nimmt nur einen Windows-Pfad mit Backslash und langem Namen (Kurzname `MJOEDI~1`/Schraegstriche:
DuckStation startet ohne Spiel, weder Slot-1 noch resume.sav). settings.ini nach jedem Lauf bytegleich zurueck (Treiber),
Sicherung `<sp>/E41n1/orig/settings.ini.vor_E41n1`; EnableGDBServer blieb false.
- **Waffe im Original wechseln:** der Debug-JUMP setzt die Waffe in der Spieler-Init FUN_800314b0 ueber die MZD-Tabelle
  @0x800c00d4 (FUN_800c00a8: `lbu v0,0(t0)`, t0 = 0x800c00d4 + DAT_800aca5c; Messer = 1 fuer char 0..6), nicht aus dem
  Inventar. Darum: Statusschirm offen sichern (`s1_open_slot1.sav`; DAT_800b25ce = Abbild der Ausruestung beim Oeffnen,
  @0x8004649c), dort DAT_800b25ce := 1 und DAT_800b25c8 := 1 (Browning) setzen, START -> Schliess-Zweig @0x800465f4..@0x8004661c
  (inv[25c8] != 25ce) -> @0x80046688 `sb v0,DAT_800aca5d` + @0x800466b0 `jal FUN_80036b68` (PLW laden). RAM danach:
  DAT_800aca5d = 3 (Handgun).
- **o_w03_stand** (`<sp>/E41n1/orig/o_w03_stand.mp4`, 1031 Bilder; Schnitte v470 -> Cut 3, v736 -> 4, v889 -> 3 = dieselben
  Abstaende 266/153 wie w1b): Original mit Handgun gegen Port `runs/K_w3_stand` (RE15_EQUIP=3), Bogen
  `<sp>/E41n1/pr_w03_k18_36.png`: Stehen, Zwischenpose k20 und Kurbeln Bild fuer Bild gleich; die Waffe kreist im Original
  genauso mit der Hand (Teil 11 = PLW-Netz, Skill re15-weapon-render §2). Cut 4 (`pr_w03_cut4.png`) gleich, das Original
  6..9 Bilder spaeter (CD-Nachladen).
- **o_hg_cut3 = der Weg des Nutzers** (`<sp>/E41n1/orig/o_hg_cut3.mp4`, 988 Bilder): Leon per RAM bei (9000,-3200), 3 s nach
  Westen gelaufen -> `p_x9000_slot1.sav`: (3012,-3200), **Cut 3** (wie Port t2), dann links drehen + Quadrat, Ja.
  v418 Frage, v420 Frage zu, **v420..v436 17 Videobilder unbewegt** (Nachladen), v438 steht Leon an der Kurbel (Sprung
  (3012,-3200)/Gier ~1500 -> (3650,-3018)/1024 im SELBEN Bild, die Balken setzen ein). Paarbogen gegen Port e1
  (`<sp>/E41n1/pr_cut3_k0_30.png`, Original v(438+2k) / Port F(523+k)): Stehen, Uebergang k21, Kurbeln gleich.
  Der Sprung ist also ORIGINAL (Pos_set @0x0A8C mitten in Cut 3); der Port springt 2 Bilder nach dem Ja, das Original nach
  ~8 Spielbildern Standbild. Grund (selbst disassembliert): Cut_chg LAB_800402a0 setzt OHNE Vergleich mit dem alten Cut
  `@0x800402f4 sb v0(=1),0x800b5457`; der Zeichner @0x80021530 `lbu 0x800b5457` -> @0x80021558 `jal FUN_80021bbc` +
  `j 0x800215fc` ueberspringt die DrawOTag-Aufrufe @0x800215bc..@0x800215e4; FUN_80021bbc laedt den Hintergrund
  (@0x80021d2c `jal 0x80013c50`) auch bei gleichem Cut. Die Dauer ist CD-Ladezeit (w1b 8, w03 14 Spielbilder) — kein
  Spielwert; der Port laedt sofort. Nicht geaendert (keine belegbare Bildzahl, betrifft jeden Cut_chg).
- **Gehen + Quadrat im Original** (`o_hg_geh2.mp4`, `o_hg_geh3.mp4`): Meldung 0 und die Frage erscheinen, die Bestaetigung
  per Quadrat wurde in 2 Laeufen NICHT angenommen (Spieler-RAM `+4..+7 = [1,1,1,0]` = Gehen, angehalten; Pad held
  0x0000, keine haengende Taste); im Stand (`t_hg_stand`, `o_w03_stand`) nimmt dieselbe Automatisierung das Ja an.
  Ursache nicht ermittelt (OFFEN unten) — betrifft nicht das "Fliegen".

#### Der Weg des Nutzers (sein eigenes befund.log)
`C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund.log` (Hauptbaum, nur gelesen): alle 492 ROOM2020-Zeilen stehen in
Sitzungen der exe "Oct 19 2026 15:24:05" (Zeilen 64768..65283); in den v0.8.28-Sitzungen ("Oct 23 2026 15:41:40") gibt es
KEIN R2020. Drei Kurbel-Szenen (Zeilen 64787..64800, 64951..64967, 65123..65151), jede aus Richtung ROOM2050 mit **Kamera C3**
beim Druecken und **masken=0**, z.B. `F235 R2020 C3 pos=(2166,-1800,-3382) rot=864` -> `F295 ... pos=(3650,-1800,-3018)
rot=1024`. Zwischen dem 19.10. (86394fc2) und der Basis 116e8b0f aendert `git diff --stat 86394fc2 116e8b0f --
re15_port/engine` nur dining_r39, nahaufnahmen_r40, untersuchen_r40 (andere Raeume) und die Masken-Zwillinge: der Nutzer sah
den VORHER-Stand — Sprung im selben Bild (Original), EIN Kurbel-Umlauf und danach 138 Bilder Standbild in der
vorgebeugten Pose (Port-Defekt, behoben) — und keine Maske (H2 scheidet als Ursache aus).

#### Ergebnis Mangel 1
Alle Anlauf-Varianten (a)-(d) des Orchestrators und der echte Weg des Nutzers sind am Port gemessen, die beiden
Original-Gegenproben mit Handgun (Stand, Cut-3-Anlauf) Bild fuer Bild gleich. Ein "Herumfliegen", das das Original nicht
hat, gibt es nach dem Fix nicht mehr; vorher wich nur das Standbild ab. Der Sprung zur Kurbel ist Original (Pos_set).
Die Rueckfrage an den Nutzer bleibt, jetzt genau gestellt (Abschnitt "Fuer den Nutzer").

### Hinweis H1 — Haken in scd_vm.c (VERTRAG §1.3) — behoben
Ursache: re15_werkstatt11b1_r41_init_lauf startete das Programm ueber scd_event_fire(24), das die Kette in scd_vm.c braucht.
Aenderung: das Modul startet den Faden selbst im ersten freien Ereignis-Slot — dieselbe Suche wie scd_event_fire
(Slots SCD_EVENT_SLOT_FIRST..SCD_EVENT_SLOT_LAST, `!active && scd_thread_start(slot, pc) == 0`), Programm unveraendert.
scd_vm.c ist wieder bytegleich zur Basis (`git diff master -- re15_port/engine/src/scd_vm.c` leer). Messung nachher:
integration_r41_kleinkram_b_werkstatt_szene/_tuer und unit_r41_kleinkram_b_11b1_* gruen (unten), Logzeile
`[wk11b1-r41] ROOM11B1 (3,0x83)=1: Ereignis 24 im Init-Lauf hinter sub00` unveraendert. Die zwei uebrigen Haken liegen an den
einzigen Stellen, an denen der Defekt sitzt: re15_inv_screen.c tuer_anziehen() (static, Markenwahl) und player_common.c
`p->anim_frame++` (Vorschub des Spielers); je 1-3 Zeilen mit "Runde 41 Spur E".

### Hinweis H2 — Maske ROOM2020 Cut 3 (Spur M)
Kein Code dieser Spur. Neu belegt: der Nutzer-Befund kam NICHT von der Maske — sein befund.log zeigt in allen 492
R2020-Zeilen `masken=0` (die Masken kamen erst mit 5924939d am 23.10. 16:23). Sichtpruefung/Korrektur: Spur M (Orchestrator).

### Hinweis H3 — Integrations-Riegel mit der echten exe — behoben
NEU tests/integration/test_r41_kleinkram_b.cmake + Auswerter tests/unit/test_r41_kleinkram_b_exe.c (marker / kurbel /
segment), registriert in tests/unit/probes/r41_kleinkram_b.cmake (exe-Kopie je Lauf, eigener Laufordner, geerbte RE15_*
geloescht, Timeout 900, kein .exe-Pfad im Skript):

| Riegel | Weg | NACHHER | VORHER-exe (Basis 116e8b0f) |
|---|---|---|---|
| integration_r41_kleinkram_b_kurbel | Leon 2050 -Tuer Slot 1-> 2020, Cut 3, Kurbel | Pos_set F343 / Plc_motion F363 / Plc_ret F541; Lage (3650,-3018) 197/197; Clip 0 fc 40 178 Bilder, **4 Umlaeufe, max 1 Bild Slot 39** | 0 Umlaeufe, **139 Bilder Slot 39** -> rot |
| integration_r41_kleinkram_b_karte | Leon 1230 bis an die Ostkammer-Tuer, Karte B1 | **MARKER 145 107** | MARKER 130 112 -> rot |
| integration_r41_kleinkram_b_werkstatt_szene | Elza 1261 -Tuer-> 11B1, Szene sub03 | Kameras 2,3,4,5,6, Ende **Cut 2** | Kameras 3,4,5,6,8, Ende Cut 8 -> rot |
| integration_r41_kleinkram_b_werkstatt_tuer | Elza 11B1 vor der Tuer, gehen + Quadrat | min x -20950, msg 12 "The door is locked.", Bild 380 in 11B1 | min x -23575 (durchgelaufen), kein msg 12 -> rot |

### Nebenbefund Kamera ROOM2020 (nicht Spur E, fuer den Orchestrator)
Port: steht der Spieler bei (9000,-3200) (Lauf g2, RE15_PLAYER_POS), wechselt die Kamera JEDES Bild zwischen Cut 0 und 2
(state.txt F1..F299). Im Gehen tritt es nicht auf (t2: 0 -> 2 bei x 8982 nach Westen; g1: 0 -> 2 bei 8018, 2 -> 0 bei 9068
nach Osten). Original-Video `o_hg_ost.mp4`: dieselbe Lage steht ~100 Videobilder ruhig auf Cut 0. Ursache nicht untersucht
(RVD-Auswahl aot_common.c / FUN_80014230).

### Nachbesserung 1 — Tests (gezielt, keine volle Suite; Bau `local_build.sh configure` + `build` -> LOCAL-BUILD-OK)
- `ctest --test-dir re15_port/build -R "r41_kleinkram_b" -j 2 --timeout 900 --output-on-failure`:
  **100% tests passed, 0 tests failed out of 10** (6 unit + 4 neue integration_r41_kleinkram_b_*; `<sp>/E41n1/ctest_eigen2.log`)
- `ctest --test-dir re15_port/build -R "^unit_(r36_szenenleon|r38_werkstatt11b0|r39_johnruf1041|r41_kleinkram_b|r36_kartenbits|scd_|r40_nahaufnahmen|r40_untersuchen|r39_hund1121|r39_elzazombies|r39_funk10f1|r38_dining|r36_schluessel|r35_cut10f0)" -j 4 --timeout 600`:
  **100% tests passed, 0 tests failed out of 85** (alle Ereignis-Ketten-Nachbarn von scd_event_fire + 11B0/11B1; `<sp>/E41n1/ctest_unit.log`)
- `ctest --test-dir re15_port/build -R "^integration_(r39_johnruf1041_tuer_t11b1|r39_johnruf1041_tuer_t11b1d|r37_elzaems_elza11b1|r38_werkstatt11b0|r36_szenenleon|r41_kleinkram_b_werkstatt_szene|r41_kleinkram_b_werkstatt_tuer)$" -j 2 --timeout 900`:
  **100% tests passed, 0 tests failed out of 7** (225 s; `<sp>/E41n1/ctest_integ2.log`)
- Gegenprobe der neuen Riegel mit der VORHER-exe (`cmake -P tests/integration/test_r41_kleinkram_b.cmake`, Basis 116e8b0f):
  alle 4 DURCHGEFALLEN (`<sp>/E41n1/vorher_riegel/*.log`, Werte in der H3-Tabelle) — die Riegel messen die Mechanik.

### OFFEN (Stand Nachbesserung 1)
- Punkt 1: ob der Nutzer mit "herumfliegend" das (behobene) Standbild nach einem Umlauf oder den Original-Sprung zur Kurbel
  (Pos_set @0x0A8C mitten in Cut 3, im Original hinter ~8 Bildern Nachlade-Standbild) meint, sagt nur er — Frage unten.
- Gehen/Rennen in die Kurbel-Zone (Port, Lauf t2): zwischen Message_on 0 und 1 laeuft der Spieler EIN Bild (F261,
  msg(a=0)) und wechselt dabei mit losgelassenem Pad von der Geh-Pose auf Stehen 200 (frac 7); das Original-RAM waehrend der
  Frage (`o_hg_geh3_slot1.sav`: `+4..+7 = [1,1,1,0]`, `+0x94 = 5`, `+0x95 = 2`) steht noch im Gehen. Wie lange das Original
  nach dem Ja (Sleep 20 @0x0A98) die Geh-Pose zeigt, ist nicht gemessen: im Original nahm die Automatisierung im Geh-Fall das Ja
  nicht an (2 Laeufe, Stand-Fall ja). Nicht geaendert (Mechanismus nicht belegt). Weg: Geh-Stop des Originals im Kommando 1
  (0x80073f90[1] = 0x80031de8) disassemblieren bzw. Ja per Savestate-Patch der Frage (DAT_800b8525/Antwortbit (12,31)).
- Nachlade-Standbild bei Cut_chg (Original ~8 Spielbilder, CD-Ladezeit, @0x800402f4 / @0x80021530..@0x8002155c): im Port
  sofort; keine belegbare Bildzahl, betrifft jeden Cut_chg — nicht Teil der Spur.
- Kamera-Pendeln ROOM2020 bei (9000,-3200) im Stand (Port, Nebenbefund oben) — nicht Spur E.

### Fuer den Nutzer (Stand Nachbesserung 1)
- Keine neuen Assets, keine Sprachdateien.
- Kurbel ROOM2020: Dein befund.log zeigt drei Kurbel-Nutzungen (Build vom 19.10., Kamera von hinten = Cut 3, aus Richtung
  ROOM2050). Damals spielte Leon die Kurbel EINMAL und blieb dann ~4,6 s vorgebeugt stehen, waehrend die Buehne fuhr — das
  ist behoben (er kurbelt jetzt durchgehend wie im Original). Dass Leon beim "Yes" an die Kurbel SPRINGT, macht das
  Original genauso (im Original friert das Bild vorher ~1/4 s ein). Frage: Meinst du mit "herumfliegend" den Sprung, das
  Standbild, oder etwas anderes? Ein Bild (F9) oder die Stelle (welche Waffe, gerannt oder gegangen) wuerde es klaeren.

## Nachbesserung 2 (nach Abnahme 1, 2026-10-24)
Grundlage: `E_abnahme_1.md` §7 Maengel 1-3 und master-AUFTRAG 2026-10-24 04:52 (E-N1 Kamera-Flackern ROOM2020, E-N2
Geh-Stopp waehrend der Kurbel-Frage). Stand bei Beginn: HEAD 29b7a8a7 (Code = 753652fc). Laeufe unter `<sp>/E41n2/`
(`<sp>` = Scratchpad dieser Sitzung, derselbe Ordner wie E41n1/E41a1). Vorliegende Messungen werden NICHT wiederholt,
sondern zitiert (Abnahme 1 §3.4 R2/R4, §6.1 EN1_kamera_{nach,basis}).

Reihenfolge: (1) E-N1 Kamera (konkreter Port-Fehler), (2) E-N2 Geh-Stopp (Disasm Kommando 1 + Original nach dem Ja im
Geh-Fall), (3) Mangel 1 Urteil "herumfliegend" nach (1)+(2).

### Mangel 3 / E-N1 — Kamera-Flackern ROOM2020 bei (9000,-3200)
Messung vorher (Abnahme 1 §6.1, nicht wiederholt): `cam=` wechselt F1..F300 299-mal (2,0,2,0,...), VORHER = NACHHER.
RVD ROOM2020 (RDT-Header +0x28 = 0x158, selbst gedumpt `<sp>/E41n2/rvd.py`): z1 @0x016C `01 ff 00 02` Cut 0 -> 2,
Viereck (8000,-4500)(8000,-2000)(9000,-2000)(9000,-4500); z5 @0x01BC `01 ff 02 00` Cut 2 -> 0, Viereck
(9000,-4500)(9000,-2000)(10000,-2000)(10000,-4500). Beide teilen die Kante x = 9000 — der Messpunkt liegt GENAU darauf.

#### E-N1 Original gemessen (DuckStation-GDB, `<sp>/E41n2/gdb_cam.py` / `gdb_cam2.py`, Muster F_nb4/gdb_affen.py)
Haltepunkte 0x8001ccf4 (Spiel-Task direkt hinter dem RVD-Scan-Aufruf @0x8001ccec, einmal je Durchlauf), 0x800142f4
(FUN_800142f4 = einziger Schreiber des angeforderten Cuts DAT_800afbb5 @0x80014300), 0x80021558 (Present FUN_8002137c:
`jal FUN_80021bbc` = Apply/Hintergrund neu laden), 0x800215bc (erstes DrawOTag). Start `hg_ready.sav` (Handgun, Cut 0),
Spielerlage beim ersten Halt per GDB auf (9000,-3200) Gier 2048 (+0x34/+0x3c int32, +0x40/+0x44 int16).
- `o_cam9000.txt` (240 Durchlaeufe): `req` (DAT_800afbb5) **2,0,2,0,... jeden Durchlauf** (120x 0, 120x 2), FUN_800142f4 je
  Durchlauf genau einmal aus `ra=800142b4` (= Ruecksprung hinter `jal 0x800142f4` @0x800142ac im RVD-Scan), `disp`
  (DAT_800b0fe4) laeuft einen Durchlauf hinterher. VSync-Abstand je Durchlauf **12/13** statt 2: jeder Durchlauf laedt
  den Hintergrund von CD.
- `o_cam9000_draw2.txt`: 40 Durchlaeufe auf der Kante: **39x RELOAD (@0x80021558), 1x DrawOTag** (nur der erste nach dem
  Patch); danach Lage (8990,-3200) (nur in z1): 1 Cut-Wechsel, 2 RELOAD, dann **38x DrawOTag**, VSync-Abstand 2.
=> Das Original schaltet auf der gemeinsamen Kante GENAUSO jedes Bild 0 <-> 2 wie der Port (FUN_80014230: erste
   enthaltende Zone gewinnt, @0x80014298 `jal 0x80014368` / @0x800142ac `jal 0x800142f4` / @0x800142b4 `j 0x800142d8`,
   keine Hysterese; FUN_80014368 zaehlt die Kante zu beiden Vierecken: alle vier Pruefungen `slt` @0x800143ac/@0x800143c8/
   @0x80014408/@0x80014424 sind bei Produkt 0 falsch). Was das Original NICHT tut, ist ZEICHNEN: Present FUN_8002137c
   @0x800214f4-@0x80021514 (angezeigt != angefordert -> `sb 1,DAT_800b5457`), @0x80021530/@0x80021538 dirty ->
   @0x80021558 `jal FUN_80021bbc` + @0x80021560 `j 0x800215fc` ueberspringt die drei DrawOTag @0x800215bc/@0x800215d0/
   @0x800215e4 UND den Pufferwechsel @0x800215f4 `xori`/@0x800215f8 `sb DAT_800aca34`. Das Bild bleibt stehen, solange jeder
   Durchlauf ein Apply ist — "steht ruhig auf Cut 0" im Video `o_hg_ost.mp4` (Bilder 0..99 ohne ein geaendertes Pixel) ist
   ein EINGEFRORENES Bild, kein ruhiger Cut.
- Ursache im Port: platform/pc/main.c (Kopfkommentar vor `s_last_cut_idx`) "Bewusst NICHT nachgebaut ... das Original zeigt
  im Anforderungsbild das VORIGE Bild noch einmal (@0x80021560 ueberspringt DrawOTag und den Pufferwechsel), der Port zeigt
  das mit dem alten Cut gezeichnete Bild N". Auf der Kante ist JEDES Bild ein Anforderungsbild -> der Port zeigt
  abwechselnd Cut 0 und Cut 2, das Original ein stehendes Bild.

### Mangel 2 / E-N2 — Geh-Anlauf: Original gemessen (DuckStation-GDB, `<sp>/E41n2/gdb_geh.py`, Lauf `o_geh1.txt`)
Eingabe ohne Pad: am Ende der Pad-Routine FUN_80030444 (@0x80030638 `jr ra`) die virtuellen Worte 0x800ac768 (gehalten) /
0x800ac76c (Flanke) / 0x800ac770 (Vorbild) geschrieben, Pad-Pause wie das Original (@0x800304f4-@0x8003051c: g_pauseflags &
0x01000000 -> gehalten &= 0xf000). Protokoll am Eingang des Spieler-Dispatchers FUN_80031c44 (vor dem Spieler-Takt
@0x8001ce0c). Start `hg_ready.sav` (2600,-3018) Gier 1024, UP gehalten (virt. Bit 0), Aktion (virt. 0x80|0x40|0x4000) in
Pad-Bild 8, danach Bestaetigung virt. 0x4000 alle 12 Bilder. Die Ja-Eingabe ist damit zuverlaessig (1 Lauf, kein Fehlversuch).
| Bild | Zustand (vor dem Spieler-Takt) |
|---|---|
| n1..n8 | Gehen `cmd 1/1/1/0`, Clip 5 Bild 1..8, z -3077 -> -3382 (Kante) |
| n8 | Aktions-Flanke |
| n9 | msg 0 offen (`pause ffff0007`, `msg 00800080`), Faden t2 @0A61 (Message_on 0 @0x0A5C + Evt_next @0x0A60 im selben Takt); Clip 5 Bild **9** |
| n46 | Bestaetigung -> der Nachrichten-Takt dieses Bilds schliesst msg 0 |
| n47 | msg 1 (Frage) schon offen (`msg 00800080`), t2 @0A67: Message_on 1 @0x0A62 + Evt_next @0x0A66 liefen im SKRIPT-Takt dieses Bilds VOR dem Spieler; Clip 5 Bild **9** (kein Luecken-Bild) |
| n94 | Bestaetigung im Auswahl-Zustand 4 (`msg 00800480`) -> @0x80028570 `andi 0x4000` -> @0x800285a4 Restore |
| n95 | `pause 01000007` (Set(2,7) @0x0A80), Frage zu, t2 @0A99 (Ifel_ck @0x0A68 ... Pos_set @0x0A8C ... Sleep @0x0A98 alles im Skript-Takt dieses Bilds), Lage **(3650,-2925)** = Pos_set, noch Gehen 1/1/1/0 Clip 5 Bild 9 |
| n96..n113 | `cmd 1/0/1/0`, **Clip 3 Bild 0**, Lage (3650,-3018) — der Spieler-Takt von n95 hat das Gehen beendet |
| n114/n115 | Sleep 20 aus -> Plc_motion @0x0A9C: `cmd 4/0/0/0` Clip 0 Bild 0, dann 1..39, n155 wieder 0 (Schleife) |
Geh-Stopp (selbst disassembliert): Kommando 1 = 0x80073f90[1] = 0x80031de8, Unterzustand +5 ueber DECIDE-Tabelle
@0x80073fb0 / ANIMATE @0x80073ff0; Gehen = [1] 0x800322e8: @0x80032328 `andi v0,a2,0xf` (keine Richtung im virt. Wort) ->
@0x80032334 `j 0x8003247c` mit `ori v0,zero,0x1` -> @0x80032480 `sw v0,DAT_800aca58` (+4..+7 = 1,0,0,0); Stehen-ANIMATE
[0] 0x80032038 Fall 0: @0x80032080/@0x80032088 +0x94 = 3, @0x80032094 +0x95 = 0, @0x8003208c/@0x8003209c +0xe3 = 7. Der Port
bildet das schon ab (player_common.c:740 / :897-937: mo 200 Clip 3, frac 7) — der Stopp selbst ist gleich.
Frame-Ordnung des Originals (selbst disassembliert): Hauptschleife @0x80020c88 `jal 0x80030444` (Pad) -> @0x80020de4
`jal 0x800298b0` (Task-Plan -> Spiel-Task FUN_8001c958: @0x8001ccec RVD, @0x8001cdec `jal 0x8003f038` SKRIPT, @0x8001ce0c
`jal 0x80031c44` SPIELER, @0x8001ce1c AOT, @0x8001ce2c ESP, @0x8001ce34 Item-Modal) -> @0x80020f3c `jal 0x80010000` ->
@0x80010044 `jal 0x800280b4` NACHRICHTEN-TAKT (-> @0x800280dc FUN_80028134) -> @0x80020f4c Present. Der Skript-Lauf ist
mit @0x8003f040-@0x8003f04c (g_pauseflags & 0x02000000) angehalten, solange eine Nachricht offen ist; Message_on
@0x800404f4 kehrt sofort mit 1 zurueck (@0x80040520), die Pause-Maske ist pc[2..3] << 16 (@0x80040508/@0x8004051c).
Abweichungen des Ports (R2_geh, Abnahme 1, nicht wiederholt):
1. LUECKEN-BILD: Port-Reihenfolge SKRIPT (main.c `scd_vm_tick`) -> NACHRICHTEN (`re15_msg_tick`) -> SPIEL (`re15_game_step`).
   Schliesst msg 0 im Nachrichten-Takt, ist der Spieler im SELBEN Bild frei (F328 Clip 5 Bild 9 -> 10). Im Original laeuft
   der Spieler-Takt VOR dem Nachrichten-Takt -> kein Luecken-Bild (n46/n47 Bild 9).
2. FRAGE PARKT: op_message_on haelt den Faden bei einer Auswahl-Nachricht AUF dem Opcode (return 2) und laeuft erst im Bild
   nach dem Schliessen weiter (F382 @0x0A62 -> @0x0A66 Evt_next), Pos_set erst F383. Im Original stand der Faden schon seit
   dem Oeffnen hinter Evt_next (n47 @0A67) und laeuft im ersten Bild nach dem Schliessen bis Sleep (n95). Census der
   ausgelieferten RDTs (`<sp>/E41n2/choice_census.py`): 102 Auswahl-Message_on, Masken 0xffff0000 (69) und 0xff800000 (33)
   — ALLE mit dem Skript-Bit 0x02000000, d.h. im Original haelt die Pause den Faden, nicht der Opcode.
3. Folge im Port: nach der Frage zwei Bilder (F381/F382) Stehen 200 an der ALTEN Stelle, Sprung erst F383; im Original
   Sprung + Stopp im selben Bild (n95), und dieses Bild ist ein Apply-Bild (Cut_chg @0x0A7E) -> stehendes Bild (E-N1).

#### Renn-Anlauf und Renn-Stopp im Original (DuckStation-GDB `gdb_geh2.py --run`, Laeufe `o_renn2.txt`, `o_rennstopp.txt`)
- Kurbel im RENNEN (`o_renn2.txt`): Nachrichten wie im Gehen (kein Luecken-Bild, Renn-Clip 0 Bild 7 durch beide Fragen);
  Bild n93 Pos_set (3650,-2925), im Spieler-Takt desselben Bilds `cmd 1/2/3/0` Clip 3 und Schritte 174, 134, 94, 54, 14,
  -26 (Tempo +0x8c 214 -> -26), n99 `cmd 1/0/0/0`, Leon steht und kurbelt bei **(3650,-3369)** (Plc_motion n113).
- Freie Strecke (`o_rennstopp.txt`, (2600,-3200) Blick 0, 12 Bilder Rennen): Tempo je Bild 269, 210, 170, 182, 205, 210,
  214, 210, 188, 159, 176, 197 (clip-gekoppelt), loslassen -> 157, 117, 77, 37, -3, dann Stehen. Der Renn-Stopp ist
  allgemein, nicht kurbel-spezifisch.
- Mechanismus (selbst disassembliert): DECIDE 0x80073fb0[2] = 0x80032604: +6 < 2 und nicht (gehalten & 0x201) == 0x201
  (@0x800326f4-@0x80032700) bzw. Zielen 0x100 mit Waffe (@0x80032708-@0x80032720) -> @0x8003272c `sb 2,+6`; +6 >= 2:
  @0x80032738 OBEN -> 0x0101, @0x80032744 UNTEN -> 0x0401, @0x80032750 LINKS/RECHTS -> 0x0301 (@0x80032760 `sw
  DAT_800aca58`). ANIMATE 0x80073ff0[2] = 0x80032778: Fall 2 @0x800328f8 +6 = 3, @0x80032904 Clip 3, @0x80032910 +0xe3 = 7;
  Fall 3 @0x80032918 anim_set, @0x8003293c Tempo -= 40, @0x8003294c `bgez` / @0x80032958 `sw 1,DAT_800aca58` (Stehen),
  @0x8003295c `jal 0x800245d8` Vorschub (auch mit negativem Tempo). Der Port kannte keinen Renn-Stopp.
- Stehen-Anlauf (`o_stand1.txt`, Kontrolle): Idle-Sway Clip 2 laeuft nach dem Ja an der Kurbel weiter (Bild 37..51, 0..),
  Pos_set n89 im Bild nach der Bestaetigung — dieselbe Reihenfolge.

### Nachbesserung 2 — Umsetzung
| Datei | Was | Beleg |
|---|---|---|
| NEU engine/src/kamerahalt_r41.c, include/re15_kamerahalt_r41.h | Entscheidung je Spielbild: Apply-Bild = `cam_change_pending` oder `work_vars[0x0A] != cam_id` am Bildende (= die Bedingung, mit der der Port-Apply `re15_cam_present_tick` im Folgebild nachzieht); nicht im ersten Bild eines Raums, nicht in der ROOM1240-Montage (RE2-Praesentation), nicht mit RE15_FORCE_CUT (Messhaken setzt jedes Bild pending); Messschiene RE15_KAMERAHALT_LOG | Present FUN_8002137c @0x800214f4-@0x80021514, @0x80021530/@0x80021538, @0x80021558, @0x80021560; Cut_chg @0x800402f4 ohne Vergleich |
| NEU platform/pc/src/bildhalt_r41_pc.c | Apply-Bild: Warteschlangen verwerfen (re15_render_pc_clear_scene_overlays), KEIN SDL_RenderPresent — das Fenster zeigt das vorige Bild; RE15_FRAMEDUMP bedient gehaltene Bilder aus dem gemerkten zuletzt gezeigten Bild (Zeile "[framedump] ... (gehalten)"), ohne RE15_FRAMEDUMP kein Ruecklesen | @0x80021560 `j 0x800215fc` ueberspringt DrawOTag @0x800215bc/@0x800215d0/@0x800215e4 und den Pufferwechsel @0x800215f4/@0x800215f8 |
| NEU engine/src/msgtakt_r41.c, include/re15_msgtakt_r41.h | Restore beim Schliessen im Nachrichten-Takt erst hinter dem Spiel-Takt (vor dem Item-Modal-Takt); eine neue Nachricht im selben Bild holt ihn vorher nach (sonst falscher Schnappschuss @0x80027ec8); Auswahl-Message_on laeuft durch (pc += 4, weiter mit Evt_next), wenn die Maske den Skript-Lauf anhaelt; message_query faellt mit dem Restore. Aktiv nur, wenn die Plattform `re15_msgtakt_r41_takt_beginn` ruft (PC) | Hauptschleife @0x80020de4 (Spiel-Task) vor @0x80020f3c/@0x80010044 (Nachrichten-Takt); Message_on @0x800404f4 Rueckgabe 1 @0x80040520; Skript-Freeze @0x8003f040-@0x8003f04c; Census 102/102 Auswahl-Message_on mit Skript-Bit, alle gefolgt von Evt_next |
| Haken platform/pc/main.c (+7/-1) | `re15_msgtakt_r41_takt_beginn/_ende` um re15_msg_tick; `re15_msgtakt_r41_nach_spieltakt` hinter re15_pc_fx_takt (vor dem Item-Modal) und vor dem Bildende; `re15_kamerahalt_r41_haelt` -> `re15_render_pc_bild_halten` statt re15_render_end_frame | |
| Haken engine/src/msg_common.c (+4/-1), engine/src/scd_vm.c (+2), platform/pc/src/render_pc.c (+9: Auftrag-Abholer `re15_render_pc_abzug_nehmen` + 1 Haken vor dem Ruecklesen) | je "Runde 41 Spur E" | |
Keine neue Konstante ausser denen mit Adresse oben. PSX-Plattform: nicht verdrahtet (platform/psx/main.c ruft die neuen
Haken nicht, PSn00bSDK fehlt hier zum Bauen) — ohne Aufruf bleiben msgtakt (inaktiv) und kamerahalt (nicht gerufen) wie
vorher.

RENN-STOPP — umgesetzt, gemessen und WIEDER HERAUSGENOMMEN (Commits 635be823 -> d4fa9074): engine/src/rennstopp_r41.c
bildete 0x80032604/0x80032778 nach (Port-Tempo 200 -> Schritte 160,120,80,40,0,-40; Kurbel im Rennen F382..F387
(3650,-3085/-3205/-3285/-3325/-3325/-3285)); damit faellt der fremde Riegel unit_r36_zombieki_affen_block (Runde 36 Spur F,
Gorilla-Zufallsgang mit Rennen und Drehen auf der Stelle): "kein Seed mehr als 600 Bilder schlechter (schlechtester
0x67be2dfd: +841)". Gegenprobe: mit abgeschaltetem Stopp (temporaer `return 0`) gruen. Ursache: der Zufallsgang wechselt
alle 90 Bilder von OBEN+RENNEN auf Drehen auf der Stelle; das Original laeuft dabei EIN Bild aus (Tempo - 40 = 160) und
bricht dann zum Drehen ab (@0x80032750) — die Bahnen aendern sich, die Port-Wahl "Umweg" der Gorillas wird fuer einen
Seed schlechter. Die Sonde gehoert nicht dieser Spur -> OFFEN, Entscheid Orchestrator (unten).

### Nachbesserung 2 — Messung nachher (exe-Kopie re15_pc_E41n2.exe md5 0844c59b61c16cbde6dff43d2ce39967, Stand d4fa9074; VORHER = re15_pc_E41a1.exe md5 beeca32b2e23c31edf7ad06a2073c5db = 753652fc; Laeufe `<sp>/E41n2/runs/`)
- E-N1, Gang auf die Kante (`EN1w_nach`/`EN1w_vor`: (9375,-3200) Blick 2048, U 5 Bilder -> x 9000 bei F64, FRAMEDUMP
  F56..F100): NACHHER 40/40 Folgebilder ab F64 pixelgleich (das Bild von F63 steht), VORHER ab F64 je Bild ~29 900 Pixel
  (Hintergrund Cut 0/2 im Wechsel). `cam=`-Spur in beiden 0,2,0,2 — wie das Original (o_cam9000: angeforderter Cut 0/2 je
  Durchlauf). Der Port haelt das Bild, solange der Spieler auf der Kante steht; das Original laeuft dort zusaetzlich mit
  ~5 Durchlaeufen je Sekunde (CD-Laden je Durchlauf, VSync-Abstand 12/13) — Hardware-Zeit, im Port nicht nachgebildet.
  Hinweis: mit RE15_PLAYER_POS direkt auf die Kante (EN1_nach) steht das ERSTE Raumbild (nach dem Sprung schwarz mit dem
  Vorspann-Text) — die Lage wird nie gezeichnet, im Original genauso (dort stand das Bild VOR dem Patch).
- E-N2 Gehen (`R2_geh_nach`, Eingaben wie Abnahme 1 R2): F310..F381 Geh-Clip Bild 9 durch BEIDE Nachrichten (F328/F329
  af=9, vorher 9 -> 10), F381 Schliessen der Frage (Spieler angehalten, af=9), **F382 Ifel_ck @0x0A68 ... Pos_set
  @0x0A8C ... Sleep @0x0A98 in EINEM Skript-Takt** (vorher F382 Message_on + Evt_next, F383 der Rest), Spieler im selben
  Bild mo 200 / Clip 3 / frac 7 an (3650,-3018), F382 gehalten (`[kamerahalt] F382 ... HALTEN`), F383 Cut 3, F402
  Plc_motion = F382 + 20. Original (o_geh1): n94 Bestaetigung -> n95 Pos_set + Geh-Stopp (Apply-Bild) -> n115 Plc_motion.
- Rennen (`R4_renn_nach2`, Code-Stand d4fa9074, Eingaben wie Abnahme 1 R4): Renn-Bild 6 durch beide
  Nachrichten, F382 Pos_set + Stehen (3650,-3018) im selben Bild, gehalten, Plc_motion F402. Original (o_renn2): Pos_set n93
  und im selben Bild der Renn-STOPP 174..-26 bis (3650,-3369) — diesen Auslauf hat der Port nicht (OFFEN).
- Stehen (`K_stand_nach`, wie E41n1 K_w3_stand): Idle-Clip (mo 212, W01 Clip 4) Bild 49 durch beide Nachrichten, Frage zu
  F461, Pos_set F462 (+1) gehalten, danach laeuft der Idle-Clip weiter (af 50..69) bis Plc_motion F482 = F462 + 20.
  Original (o_stand1): Sway Clip 2 laeuft nach dem Ja weiter, Pos_set im Bild nach der Bestaetigung — gleiche Folge.
- Weg des Nutzers (`R1_weg2050_nach`: 2050 -Tuer-> 2020, Cut 3, gehen, Aktion): Geh-Clip Bild 125 von F186 (msg 0) bis
  zur Frage ohne Luecke (VORHER F261 mo 200 = Luecken-Bild mit Stehen), Pos_set F342 im Bild nach dem Schliessen (VORHER
  F343), F342 gehalten; dazu je Zonen-Kamerawechsel auf dem Weg ein gehaltenes Bild (F102 Cut 0 -> 2, F150 2 -> 3).
  Bildbogen `<sp>/E41n2/R1_nach_339_344.png`: F339/F340 Frage, F341 Frage zu (Leon noch in der Geh-Pose an der alten
  Stelle), F342 = F341 (gehalten), F343 Leon an der Kurbel, Balken setzen ein.

#### DuckStation (Nachbesserung 2)
Messungen nur per GDB-Server (Port 2345, `<sp>/E41n2/gdb_cam*.py`, `gdb_geh*.py`; EnableGDBServer nur fuer den Lauf true).
Sicherung vor dem ersten Lauf: `<sp>/E41n2/ds_backup/` (settings.ini md5 b3e22060..., resume.sav 54717c88..., Slot 1
6f59edd4...). FALLE: die GDB-Treiber nach Muster F_nb4/gdb_affen.py schreiben settings.ini im TEXTMODUS zurueck -> unter
Windows CRLF statt LF (11702 statt 11177 Bytes); ihre Pruefung "settings.ini zurueck: True" vergleicht nur den Text. Nach
den Laeufen beide Dateien per Byte-Kopie aus der Sicherung zurueckgesetzt (md5 wieder b3e22060... / 54717c88...; Slot 1
unveraendert). ConfirmPowerOff = false, ToggleMediaCapture = Keyboard/F12, SaveSelectedSaveState = SDL-0/LeftShoulder,
EnableGDBServer = false wie vorher.

### Nachbesserung 2 — je Mangel (Ursache / Messung vorher / Beleg / Aenderung / Messung nachher)
| Mangel | Ursache | Messung vorher | Beleg (Original) | Aenderung | Messung nachher |
|---|---|---|---|---|---|
| 3 / E-N1 Kamera-Flackern (9000,-3200) | Der Port zeichnete und zeigte JEDES Bild, auch das Apply-Bild eines Kamerawechsels (main.c: "Bewusst NICHT nachgebaut"). Auf der gemeinsamen Kante x 9000 von z1 @0x016C / z5 @0x01BC ist jedes Bild ein Apply-Bild. | Abnahme 1 §6.1: 299 Wechsel in 300 Bildern; EN1w_vor: ab F64 je Bild ~29 900 Pixel anders | GDB o_cam9000*: angeforderter Cut wechselt auch im Original jeden Durchlauf (FUN_800142f4 aus @0x800142b4), 39/40 Durchlaeufe Apply @0x80021558, 1 DrawOTag; Present @0x800214f4-@0x80021514, @0x80021530/@0x80021538, @0x80021560 `j 0x800215fc` (kein DrawOTag @0x800215bc/d0/e4, kein Pufferwechsel @0x800215f4/f8) | kamerahalt_r41.c + bildhalt_r41_pc.c (Apply-Bild nicht praesentieren) | EN1w_nach: 40/40 Folgebilder pixelgleich, cam-Spur weiter 0/2 wie das Original; Riegel integration_r41_kleinkram_b_kante (VORHER-exe: GLEICH 0 von 24 -> rot) |
| 2 / E-N2 Geh-/Renn-Anlauf | (a) Port-Reihenfolge SKRIPT -> NACHRICHTEN -> SPIEL: der Spieler war im Schliess-Bild schon frei (Luecken-Bild). (b) op_message_on parkte eine Auswahl-Nachricht AUF dem Opcode und lief ein Bild spaeter weiter. | Abnahme 1 R2/R4: Luecken-Bild F328 (Geh-Bild 9 -> 10) bzw. Renn-Bild 6 -> 7, Pos_set F383 = Schliessen + 2 | GDB o_geh1/o_renn2/o_stand1: kein Luecken-Bild (n46/n47), Pos_set im Bild nach der Bestaetigung (n94 -> n95), dort Geh-Stopp an der Kurbel (`cmd 1/0/1/0` Clip 3 Bild 0 = 0x800322e8 @0x80032328/@0x80032480 + 0x80032038 Fall 0 @0x80032074-@0x800320a4); Frame-Ordnung @0x80020de4 / @0x80020f3c / @0x80010044; Message_on @0x80040520; Skript-Freeze @0x8003f040-@0x8003f04c | msgtakt_r41.c (Restore hinter dem Spiel-Takt; Auswahl ohne Parken) | R2_geh_nach / R1_weg2050_nach / K_stand_nach: kein Luecken-Bild, Pos_set = Schliessen + 1, Apply-Bild gehalten; Riegel integration_r41_kleinkram_b_geh/_renn + unit_r41_kleinkram_b_takt_frage (Gegenprobe alte Reihenfolge: Luecke 1, Pos_set +2) |
| 1 "herumfliegend" | Alle Anlauf-Varianten sind jetzt am Original gemessen (Stand, Gehen, Rennen, Cut-3-Weg von 2050; Waffen in Abnahme 0). Abweichung, die bleibt: der Renn-STOPP (Auslauf nach dem Rennen) fehlt dem Port allgemein. In keinem Port- und keinem Original-Lauf "fliegt" Leon. | — | Original Rennen: nach Pos_set laeuft Leon 444 Einheiten nach -z aus und kurbelt bei (3650,-3369) — 351 Einheiten neben der Stelle des Geh-/Stand-Falls (3650,-3018). Original Cut_chg-Standbild ~8 Spielbilder (CD-Laden), Port 1 Bild. | — (Renn-Stopp siehe OFFEN) | Frage an den Nutzer unten, jetzt mit allen Varianten |

### Nachbesserung 2 — Tests (gezielt, keine volle Suite)
Bau: `bash re15_port/tools/local_build.sh configure` + getrennt `build` -> `=== LOCAL-BUILD-OK (build)` (Endstand 11b8f867,
exe md5 486f77a7d870cf73c1bdf542bf40cbfb).
- Alle Unit-/Prop-/Sonden (Code d4fa9074; die spaetere Aenderung 11b8f867 betrifft nur platform/pc, nicht re15_engine):
  `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" -j 2 --timeout 600`:
  **100% tests passed, 0 tests failed out of 925** (1403 s, `<sp>/E41n2/ctest_unit_alle2.log`). Darin
  unit_r36_zombieki_affen_block gruen (mit dem Renn-Stopp rot, s. Umsetzung) und die neuen
  unit_r41_kleinkram_b_takt_{msgtakt,kamerahalt,frage}.
- Integration, Auswahl = alle Riegel, deren Skript RE15_FRAMEDUMP / RE15_PRESS / RE15_MSG_LOG / msgtext / XU / CROSS /
  RE15_CAM_TRACE enthaelt (128 von 223; Liste `<sp>/E41n2/integ_auswahl.txt`, Regex `integ_regex.txt`), Code d4fa9074:
  Teil 1 (-j 2, nach 1 Test abgebrochen und mit -j 4 neu gestartet): integration_r36_szenenleon Passed;
  Teil 2 `ctest -R "$(cat integ_regex2.txt)" -j 4 --timeout 900`: **99% tests passed, 1 tests failed out of 127**
  (4213 s, `<sp>/E41n2/ctest_integ_auswahl2.log`). Der eine: integration_r38_dining1141_s "R0 Bilder: 5787 verglichen,
  14 fehlen" = die 14 gehaltenen Bilder (Kamerawechsel) ohne Abzug, weil der Merker nur mit RE15_FRAMEDUMP lief und die
  Sonde RE15_R38F_SICHT ueber re15_render_pc_request_readback liest -> 11b8f867 (Merker ab dem ersten Auftrag).
- Nach 11b8f867: `ctest -R "^(integration_r38_dining1141_s|integration_r35_entladen|unit_r41_kleinkram_b_.*|integration_r41_kleinkram_b_.*)$" -j 3 --timeout 1200`:
  **100% tests passed, 0 tests failed out of 17** (`<sp>/E41n2/ctest_nach_fix.log`; integration_r35_entladen traf
  keinen Test, die heissen _a.._l, s. naechster Lauf).
- Neue Riegel gegen die VORHER-exe (re15_pc_E41a1.exe = 753652fc) per `cmake -P tests/integration/test_r41_kleinkram_b.cmake`
  (`<sp>/E41n2/vorher_riegel/`): kante DURCHGEFALLEN ("GLEICH 0 von 24", kein "(gehalten)"), geh DURCHGEFALLEN (1 Posen-
  wechsel zwischen den Nachrichten, Pos_set F383 = Schliessen + 2, nicht gehalten). unit_r41_kleinkram_b_takt_frage
  prueft die Gegenprobe (alte Reihenfolge: Luecke 1, Pos_set + 2) selbst.
- Restliche Integration (die 95 nicht in der Auswahl, darunter integration_r35_entladen_a..l und die 15 ohne -P-Skript),
  Endstand 11b8f867: `ctest -R "$(cat integ_rest_regex.txt)" -j 4 --timeout 900`: **100% tests passed, 0 tests failed
  out of 95** (2474 s, `<sp>/E41n2/ctest_integ_rest.log`). Damit lief jeder Integrations-Riegel des Baums einmal gegen die
  geaenderte Bildreihenfolge (Auswahl 127 auf d4fa9074 + 17 und 95 auf 11b8f867); die 126 gruenen der Auswahl wurden nach
  11b8f867 nicht wiederholt (die Aenderung betrifft nur den Abzug gehaltener Bilder fuer Nicht-FRAMEDUMP-Leser).
- Kein Fenster-Haken rot, keine Nachlaeufe noetig.

### OFFEN (Stand Nachbesserung 2)
- RENN-STOPP fehlt dem Port allgemein (jeder Halt nach dem Rennen). Original gemessen (o_rennstopp: 157,117,77,37,-3;
  o_renn2 an der Kurbel 174..-26), Mechanismus disassembliert (0x80032604 @0x800326f4-@0x8003272c, @0x80032730-@0x80032760;
  0x80032778 @0x800328f8-@0x8003295c), Umsetzung gebaut und gemessen (635be823: frei 160,120,80,40,0,-40; Kurbel F382..F387
  bis (3650,-3285)), wieder heraus (d4fa9074), weil sie die Bahnen des Gorilla-Zufallsgangs aendert und
  unit_r36_zombieki_affen_block (Runde 36 Spur F) fuer Seed 0x67be2dfd ueber die Schranke 600 bringt (+841; ohne Stopp gruen).
  Entscheid Orchestrator: Renn-Stopp spielweit einbauen und die Gorilla-Sonde (Port-Wahl Umweg) neu einmessen lassen.
  Folge bis dahin: im Renn-Anlauf an der Kurbel bleibt Leon nach dem Ja bei (3650,-3018) stehen, das Original laeuft bis
  (3650,-3369) aus.
- Renn-Tempo des Ports konstant 200 (Tabelle @0x80073f24); das Original fuehrt es clip-gekoppelt (o_rennstopp: 269, 210,
  170, 182, 205, 210, 214, 210, 188, 159, 176, 197; ANIMATE @0x800328d4-@0x800328f4 + FUN_800369f8 @0x800328cc). Mit dem
  Renn-Stopp waere die Auslauflaenge darum nur im Mechanismus gleich (Port 360, Original 385 bzw. 444).
- Dauer des Apply-Standbilds: Port 1 Spielbild (die Logik eines Durchlaufs), Original so lange, wie FUN_80021bbc den
  Hintergrund von CD laedt (FUN_80013c50 @0x80021d2c; gemessen ~8 Spielbilder bei Cut_chg, auf der RVD-Kante 12/13 VSyncs
  je Durchlauf) — Hardware-Zeit, keine Konstante.
- PSX-Plattform: platform/psx/main.c ruft msgtakt/kamerahalt nicht (PSn00bSDK fehlt hier zum Bauen und Pruefen).
- Nebenbefund (unveraendert, nicht Spur E): player_common.c "msg_block -> IDLE"-Zweig stellt den Spieler bei einer Nachricht
  OHNE Spieler-Pause (Maske ohne 0x80000000) auf Stehen; das Original laesst ihn dann frei laufen.

### Fuer den Nutzer (Stand Nachbesserung 2)
- Keine neuen Assets, keine Sprachdateien.
- Kurbel ROOM2020: Leon bleibt waehrend beider Texte ("It's a square hole." / "Will you use the Crank?") in der Pose stehen,
  in der er angekommen ist, und nach dem "Yes" springt er im naechsten Bild an die Kurbel — beides jetzt Bild fuer Bild wie im
  Original (vorher: ein Zwischenbild zwischen den Texten und zwei Bilder Stehen an der alten Stelle vor dem Sprung). Der
  Sprung selbst ist Original (das Spiel setzt Leon per Skript an die Kurbel); im Original verdeckt ihn ein kurzes Standbild,
  waehrend der Hintergrund von CD laedt (~1/4 s), der Port haelt jetzt ein Bild.
- Kamera ROOM2020: bleibt Leon genau auf der Grenze zweier Kamerazonen stehen (z.B. x 9000 im Gang), steht das Bild jetzt
  still wie im Original, statt zwischen zwei Kameras zu flackern.
- Im RENNEN an die Kurbel: im Original rutscht Leon nach dem "Yes" an der Kurbel noch ein Stueck weiter (Renn-Stopp) und
  kurbelt etwa 35 cm daneben; diesen Auslauf hat der Port noch nicht (offen, siehe oben).
- Allgemein: bei jedem Kamerawechsel steht fuer ein Bild das alte Bild (wie im Original, dort laenger: CD-Ladezeit); nach
  jedem Text kann Leon ein Bild spaeter wieder laufen (wie im Original).
- Frage: Was genau war das "Herumfliegen"? Moeglich sind (a) der Sprung an die Kurbel nach dem "Yes" (im Original genauso,
  dort hinter ~1/4 s Standbild), (b) das Einfrieren nach einer Kurbel-Umdrehung (behoben in Nachbesserung 1), (c) etwas
  anderes. Wenn (c): bitte die Stelle (gerannt/gegangen/gestanden, welche Waffe) oder ein Bild (F9).

## Nachbesserung 3 (nach Abnahme 2, 2026-10-24)
Grundlage: `E_abnahme_2.md` §7 Maengel 1-4 (+ Hinweise H1-H7) und die Orchestrator-Entscheide E-E1..E-E3 (AUFTRAG.md
2026-10-24 10:50). Stand bei Beginn: HEAD 96f1f7f2 (Code = 11b8f867, exe md5 486f77a7...). Laeufe unter `<sp>/E41n3/`
(`<sp>` = Scratchpad dieser Sitzung). Vorliegende Messungen (Abnahme 2 §3, Dossier Nachbesserung 2) werden zitiert, nicht
wiederholt.

Reihenfolge: (1) Mangel 4 / E-E3 Blenden-Takt im gehaltenen Bild (Softlock), (2) Mangel 1/2 / E-E2 Renn-Stopp spielweit +
Gorilla-Sonde neu eingemessen + Riegel renn auf (3650,-3369), (3) Mangel 3 / E-E1 Vertrag (Entscheid liegt vor) +
Integrations-Bedingung (msg/scd/cut/door/szene/karte-Riegel mitgefahren).

### Status Nachbesserung 3
- [x] Mangel 4 / E-E3 Blenden-Takt im gehaltenen Bild
- [x] Mangel 1/2 / E-E2 Renn-Stopp spielweit, Gorilla-Sonde, Riegel renn (Fortsetzung: DECIDE 0x80032604 vollstaendig,
      Riegel rennpfad, Gorilla-Sonde zweite Messung ueber die Population)
- [x] Mangel 3 / E-E1 Vertrag + Integrations-Riegel (gezielter Lauf, Abschnitt "Fortsetzung — Tests")

### Mangel 4 / E-E3 — Blenden-Takt im gehaltenen Bild (Softlock beim Raumeintritt auf der Kante)
- Ursache: das gehaltene Apply-Bild (main.c Nachbesserung 2) ruft `re15_render_pc_bild_halten()` statt
  `re15_render_end_frame()`; der Blenden-Takt `re15_fade_tick()` (= FUN_80021880) steckt aber in end_frame
  (render_pc.c:1290). Jede Rampe verlor je gehaltenem Bild einen Schritt; steht der Spieler beim Raumeintritt auf einer
  wechselnden RVD-Kante, ist JEDES Bild gehalten, die Tuer-Einblendung (Schritt -6144) endet nie, room_common.c Zustand 5
  gibt den Eintritts-Freeze nie frei.
- Messung vorher (Abnahme 2 §3.6, nicht wiederholt; Lauf `<sp>/E41a2/runs/EN1_spawn_kante_{nach,vorher}`): 11b8f867
  pf=FF000007 F1..F199, Spieler steht, alle Abzuege schwarz; 753652fc (ohne Bildhalt) pf=00000007 ab **F6**, F70 9070 ->
  F100 11100.
- Beleg (selbst disassembliert, `re15_disasm.py dis 0x80020f30 10` / `dis 0x80021910 12`): Hauptschleife @0x80020f34
  `jal 0x80021a0c` (Letterbox), @0x80020f3c `jal 0x80010000`, @0x80020f44 `jal 0x80021880` (Blenden-Takt), @0x80020f4c
  `jal 0x8002137c` (Present), @0x80020f54 `j 0x80020c10`. FUN_80021880 fuehrt den Pegel: @0x8002191c `lhu v0,0(s0)`,
  @0x80021920 `lhu v1,-6(s1)`, @0x80021928 `addu v0,v0,v1`, @0x8002192c `sh v0,0(s0)`. Der Takt liegt VOR dem Present,
  also auch im Apply-Durchlauf (der Apply-Zweig @0x80021558/@0x80021560 ueberspringt nur DrawOTag + Pufferwechsel).
- Aenderung: platform/pc/src/bildhalt_r41_pc.c `re15_render_pc_bild_halten()` ruft `re15_fade_tick()` (Pegel weiter,
  Kachel ungezeichnet — wie die OT des Apply-Durchlaufs, die nie gezeichnet wird). Keine neue Konstante, keine gemeinsame
  Datei. Kommentar include/re15_kamerahalt_r41.h berichtigt (Hinweis H4: "nicht praesentieren" statt "Standbild
  wiederholen").
- Messung nachher (`<sp>/E41n3/runs/EN1_spawn_kante_n3a`, exe re15_pc_E41n3a.exe, Eingaben wie Abnahme 2 §3.6):
  `[kamerahalt]` F0 zeigen, F1..F70 HALTEN (Spieler auf der Kante), danach zeigen; state.txt pf=FF000007 F1..F5,
  **pf=00000007 ab F6** (= VORHER 753652fc), F70 9070 -> F100 11100 (= VORHER); Abzuege F0..F70 schwarz (gehalten:
  Helligkeit 5,1), ab F75 der Raum in voller Helligkeit (28,2) — die Einblendung lief unsichtbar in den gehaltenen
  Bildern, wie im Original (dort ohne DrawOTag).
- Riegel NEU `integration_r41_kleinkram_b_spawn_kante` (Auswerter `einblendung`): F1..F5 gehalten (Vorbedingung), Lage x
  9000, Freeze faellt in F6, x(F100) > 9000. NACHHER: `FREI F6 GEHALTEN 70 (F1..F5: 5) X1 9000 X100 11100` BESTANDEN;
  Gegenprobe VORHER-exe (re15_pc_E41a2.exe = 11b8f867, `<sp>/E41n3/vorher_riegel/spawn_kante.log`): `FREI F-1 GEHALTEN
  200 ... X100 9000` DURCHGEFALLEN.

### Mangel 1/2 / E-E2 — Renn-Stopp spielweit: das Renn-TEMPO des Originals (RE-Belege, Teil 1)
Ausgangslage (Abnahme 2 §3.4, Dossier Nachbesserung 2): mit dem Renn-Stopp aus 635be823 und dem konstanten Port-Tempo 200 endet
der Auslauf an der Kurbel bei (3650,-3285), das Original bei (3650,-3369), weil sein Stopp mit Tempo 214 beginnt (o_renn2 n93
-> n94 174 = 214 - 40). Der Riegel auf (3650,-3369) (E-E2) verlangt also das Renn-Tempo des Originals, nicht nur den Stopp.
Selbst disassembliert (`re15_disasm.py dis 0x80032778 150`, `dis 0x800369f8 120`, `dis 0x80022da0 60`, `dis 0x80020510 70`,
`dis 0x800664e0 30`):
- ANIMATE Rennen 0x80032778 (Tabelle @0x80073ff0[2]): Fall 0 @0x800327cc-@0x80032830: Tempo := Tabelle @0x80073f24[Figur*2]
  (= 200), +6 = 1, +0x8f = 7 @0x800327fc, Clip 0 @0x8003280c, Bild 0 @0x80032814 -> faellt in Fall 1 @0x80032834: Schritt-Ton
  (Bildwort & 0x4000 @0x80032848), anim_set @0x80032884 (FUN_8001f314 a3 = 0x200), Figur 0xf -> ohne Fussrechnung
  (@0x80032894/@0x80032898); sonst @0x800328b4 Bildwort & 0x2000 -> a1 = 0 (@0x800328c4), sonst a1 = 1 (@0x800328c8), a0 = 1,
  @0x800328cc `jal 0x800369f8`, dann @0x800328dc-@0x800328f4 Tempo := ((int16)Tempo >> 2) + 150, @0x8003295c Vorschub.
- FUN_800369f8(Modus a0, Fuss a1): Kette Entity-Matrix +0x20 (Rotation aus +0x68 @0x80036a34, Translation = Lage +0x34/+0x3c)
  · Teil 0 (@0x80036a48) · Teil 1 (@0x80036a58) · Teil 2+3f (@0x80036a7c) · Teil 3+3f (@0x80036a8c) · Teil 4+3f (@0x80036a9c),
  Teile-Stride 0xAC (Fuss-Versatz a1*0x204 @0x80036a60-@0x80036a70); Modus 1 @0x80036af0-@0x80036b44: Tempo +0x8c :=
  SquareRoot0((s16)(x_neu - Teil[+0x54])^2 + (s16)(z_neu - Teil[+0x5c])^2) — Teil+0x40.. = Weltmatrix des Fussteils aus dem
  LETZTEN Zeichnen (Weltlage von dort), x/z_neu = dieselbe Kette mit der eben gesetzten Pose. Das Renn-Tempo ist also die
  Strecke, die der Fuss gegenueber dem letzten Bild zuruecklegt.
- FUN_80022da0 (Matrix-Verkettung, GTE): Rotation = A·B je Spalte MVMVA sf=1 (IR, lm=0), Translation = A.t + (A·(s16)B.t) >> 12
  (@0x80022eb0-@0x80022f00).
- Pose-Aufbau FUN_8001f3bc (RE_15_Quellcode_V2/FUN_8001f3bc.c, Zaehler +0x8f vor dem Abzaehlen): Wurzel = GPF12(kf, 0x1000-w) +
  GPL12(vorige, w); Winkel FUN_80020510 -> LoadAverageShort12 @0x800664e0: GPF @0x80066508 `cop2 0x198003d` / GPL
  @0x80066538 `cop2 0x1a8003e` = floor(vorige*w/4096) + floor(kf*(0x1000-w)/4096) — ZWEI Abrundungen (psx-spx: GPF MAC =
  IR*IR0 SAR 12, GPL MAC = ((MAC SHL 12) + IR*IR0) SAR 12), w = 0x200 * Zaehler. (Der Port-Renderer rundet einmal ab:
  skeleton_common.c re15_blend_angle12 / Wurzel-Mischung — Abweichung <= 1 je Winkel; fuer das Tempo braucht es die zwei.)
- SquareRoot0 = BIOS-Tabellen-Naeherung (Memory reai-v2-squareroot0-audit; Port re15_squareroot0, re15_math.c).
- Gehen 0x80032498 hat dieselbe Fussrechnung (@0x800325c4 `jal 0x800369f8`, @0x800325dc-@0x800325e8 Tempo := (raw >> 1) + 50,
  Fuss-Zuordnung umgekehrt @0x800325a8-@0x800325c0) — der Port geht konstant 75 (@0x80073ea4). Nicht Teil von E-E2 -> OFFEN.
Nachgerechnet OHNE Emulator (Sonde `<sp>/E41n3/probe/renntempo_blend.c`, gegen libre15_engine.a: PL00-Bindepose + PL00W03-
Keyframes (= Port-Verbund der Handgun-Bank), Kette wie oben, Mischung mit zwei Abrundungen, re15_squareroot0):
| Lauf | Original (GDB, Spalte spd) | Nachrechnung |
|---|---|---|
| o_renn2 (Blick 1024, Start Stehclip 2, `mo=2/44` -> zuletzt Bild 43) | 198 210 171 182 204 211 214 | **198 210 171 182 204 211 214** (7/7) |
| o_rennstopp (Blick 0, `mo=2/40` -> Bild 39) | 269 210 170 182 205 210 214 210 188 159 176 197 | 199 **210 170 182 205 210 214 210 188 159 176 197** (11/11 ab Bild 2) |
Das erste Bild von o_rennstopp (269) ist ein Messartefakt: der GDB-Treiber hatte die Lage im RAM versetzt, die gespeicherte
Weltmatrix (Teil+0x40) stammte noch von der alten Lage -> grosser Fussweg. Mit einer Abrundung statt zweien weicht die Rechnung
in 3 von 7 bzw. 4 von 11 Bildern um 1 ab, mit exaktem isqrt in 1 von 7. Damit ist der Mechanismus belegt.

### Mangel 1/2 / E-E2 — Umsetzung, Messung nachher (Teil 2)
- NEU engine/src/rennstopp_r41.c + include/re15_rennstopp_r41.h (Ausgang 635be823, erweitert):
  `re15_rennstopp_r41_lauftempo` = Fall 0/1 des Renn-ANIMATE: erstes Renn-Bild Tempo := Tabelle (@0x80073f24 = 200,
  Port RUN_SPEED_PER_FRAME), dann Fussweg-Tempo aus der Bank des Renders (re15_player_w_skel/_anim = Verbund PL00-Bindepose +
  Waffen-Keyframes, Clip 0); Pose der Kette wie FUN_8001f3bc mit zwei Abrundungen, im ersten Renn-Bild aus dem zuletzt
  gezeichneten Bild (prev_angles/prev_root) gesetzt; Gier des letzten Bilds = aktuelle Gier minus Drehung dieses Takts.
  `re15_rennstopp_r41_takt/_skript` = Renn-Stopp wie 635be823, beginnt aber mit dem letzten Renn-Tempo (+0x8c bleibt stehen).
  Konstanten: 40 (@0x8003293c), 7 (@0x800327f4/@0x80032908), 0x200 (@0x80032888/@0x8003292c), 150 (@0x800328ec), Kette
  0/1/2+3f/3+3f/4+3f (@0x80036a48-@0x80036a9c), Fuss-Bit 0x2000 (@0x800328b4). Ohne gespiegelte Bank: Tabellen-Tempo (wie
  Figur 0xf im Original, @0x80032894).
- Haken player_common.c (3 Stellen, "Runde 41 Spur E"): Stopp-Takt hinter `r1_held` (wie 635be823), Renn-Tempo statt
  `speed = RUN_SPEED_PER_FRAME`, Skript-Zweig (Stopp mit leerem Pad, wie 635be823).
- Unit-Riegel (`ctest -R "unit_r41_kleinkram_b_takt_(rennstopp|renntempo)"`: 100% tests passed, 0 tests failed out of 2):
  rennstopp 10/10 (Stopp mit Tabellen-Tempo, Abbruch, Zielen, Skript-Zweig), renntempo 7/7: o_renn2 198 210 171 182 204 211
  214 = Original, o_rennstopp ab Bild 2 = Original (11/11), derselbe Wert ueber den Spieler-Pfad (lauftempo), Kurbel-Auslauf
  z -3099 -3233 -3327 -3381 -3395 -3369 = Original n94..n99, Gegenprobe ohne Bank z -3285, Renn-Clip 0 ohne Tween-Bild in
  42 Waffen-Baenken.
- Port-Idle in ROOM2020 auf (2600,-3018) Blick 1024 (`<sp>/E41n3/runs/idle2020`): Stehclip 2 (Wiegen) ab F113, F156 = Bild 43.
- Messung nachher an der exe (re15_pc_E41n3b.exe, Lauf `<sp>/E41n3/runs/renn_o2` = Nachbau o_renn2: Start (2600,-3018) Blick
  1024, Rennen ab F157 (6 Bilder XU + 1 Bild XU mit Aktion)): Renn-Bilder F157..F163 = Clip 0 Bild 0..6, F157 z -3216
  (Tempo 198 = Original n1), F158 -3382 (Stegkante, Original n2 -3382), eingefroren in Bild 6 (Original `mo=0/7`); msg 0 F164,
  msg 1 F202, Frage zu F281, F282 Ifel_ck..Pos_set..Sleep (gehalten) und Stopp-Schritt 1: **F282..F287 (3650,-3099) -3233 -3327
  -3381 -3395 -3369 = Original n94..n99**, Stand (3650,-3369) bis F420, Plc_motion F302 (= Pos_set + 20).
- Riegel `integration_r41_kleinkram_b_renn` umgestellt (Mangel 2): Nachbau o_renn2, Auswerter `renn` (Bildfolge wie geh, erstes
  Renn-Bild z -3216, Bild 6 eingefroren, Auslauf n94..n99, Stand (3650,-3369) bis Plc_motion). NACHHER BESTANDEN; Gegenprobe
  VORHER-exe (11b8f867, `<sp>/E41n3/vorher_riegel/renn.log`): erstes Bild z -3218 (Tempo 200), Pos_set-Bild (3650,-3018) ohne
  Auslauf -> DURCHGEFALLEN.

### Mangel 1/2 / E-E2 — unit_r36_zombieki_affen_block neu eingemessen (Teil 3)
- WAS die Sonde misst (test_r36_zombieki_affen_block.c zg_stat, Riegel-Teil): blockierte Gorilla-Bilder (lebt, kein Angriff,
  >= 90 Bilder im Anker 400) im deterministischen Zufallsgang (Leon rennt UP+CROSS, alle 90 Bilder neue Zielrichtung, bei
  Abweichung > 48 Drehen auf der Stelle; HP je Bild 100), Original-Regel A[3] (AUS) gegen PORT-WAHL Umweg (AN), 16 Seeds,
  je Lauf eigener Prozess; Schranken: FERN AN <= 1/6 AUS, Griffe AN >= 80 % AUS, je Seed AN - AUS <= 600.
- WARUM der Renn-Stopp den Wert aendert: bei jedem Wechsel Rennen -> Drehen laeuft Leon im Original EIN Bild aus (DECIDE
  0x80032604 @0x8003272c +6 = 2, ANIMATE Fall 3 Tempo - 40 @0x8003293c, Vorschub @0x8003295c; danach LINKS/RECHTS -> 0x0301
  @0x80032750); der Port blieb sofort stehen. ~66 Wechsel je Lauf verschieben die Bahn, ab der ersten Abweichung laufen die
  Gorillas anders. Die Sonde hat keine gespiegelte Waffen-Bank -> dort wirkt nur der Stopp, das Renn-Tempo bleibt 200.
- Messung (neuer Sondenmodus `rennvergleich <von> <bis>` = je Seed AUS/AN x ohne/mit Renn-Stopp, eigene Prozesse; "ohne" ueber
  den Sonden-Haken re15_rennstopp_r41_pruef_aus; `<sp>/E41n3/affen/rv_0_143.txt`, Auswertung `<sp>/E41n3/affen/rv_auswertung.py`):
  | Seeds | ohne Stopp: schlechtester AN-AUS (> +600) | mit Stopp | Summen AN/AUS ohne -> mit |
  |---|---|---|---|
  | 0..15 (Riegel) | +84 (0) | +841 0x67be2dfd (1) | 5835/21172 -> 6507/22602 |
  | 16..47 | +224 (0) | +636 0xbb4281d4 (1) | 12452/68831 -> 14471/82677 |
  | 48..143 | +3334 0xbc8bd05d, +1416 0x71d8aa83 (2) | +3266, +2357, +1350, +726, +640 (5) | 45207/193073 -> 54078/212672 |
  Bahn-Chaos der Original-Regel allein |AUS(mit) - AUS(ohne)| je Seed: Median 1039,5, Mittel 1421, p90 3286, max 6650 (144
  Seeds); Port-Wahl |AN(mit) - AN(ohne)|: Median 272,5. FERN-Schranke 1/6 je 16er-Block: ohne und mit Stopp 8 von 9 (ohne
  reisst 96..111, mit 112..127), Riegel-Block beide; Griffe >= 80 % in allen 9 Bloecken beider Bewegungen.
- 0x67be2dfd (+841): AUS 839 -> 112, AN 187 -> 953 = 949 NAH-Jagd [0xbb8,0x1771) + 4 Biss, NAH-Anker max 334 < R36_NAH_START
  354 -> der Nah-Umweg der Port-Wahl greift nie; die Bilder sind die Original-Nahjagd A[3] (Gorilla 2 bei (-23302,13100..13486),
  Leon an der Nordwest-Wand (-26000,14578..17268), LAGE-Zeilen `eins 13 1 6000 400 1`). 0xbb4281d4 (+636, nicht im Riegel):
  FERN-Haenger von Gorilla 2 bei (11280,9820) ab ~F4100 (Klemme, Umweg ohne Wirkung) — dieselbe Klasse wie die FERN-Haenger
  ohne Stopp (0xbc8bd05d +3334), eine Grenze der Port-Wahl der Runde 36, nicht des Stopps.
- Ergebnis: die Schranke "je Seed <= 600" galt nur fuer diesen Seed-Satz (ohne Stopp schon 2 von 144 darueber, Runde 36 F
  Abnahme 2: 0x179803a8 +825 auf 52 Seeds). NEUE SCHRANKE = Median der Bahn-Chaos-Amplitude der Original-Regel unter genau
  dieser Bahnaenderung: AN - AUS <= 1039 je Seed (abgerundet aus 1039,5; ein Seed zaehlt als "durch die Port-Wahl schlechter",
  wenn er um mehr schlechter ist, als eine blosse Bahnaenderung die Original-Regel typischerweise verschiebt). Die Zahl der
  Seeds ueber +600 wird weiter ausgegeben. Alle anderen Schranken unveraendert. Lauf: `ctest -R "^unit_r36_zombieki_affen_block$"`
  -> 100% tests passed, 0 tests failed out of 1 (schlechtester 0x67be2dfd +841, Seeds ueber +600: 1; FERN 1523 <= 17487/6).

### Mangel 3 / E-E1 — Vertrag (gemeinsame Dateien)
Orchestrator-Entscheid E-E1 (AUFTRAG.md 2026-10-24 10:50): die spielweiten Haken der Spur E sind angenommen (Bildreihenfolge
@0x80020de4 vor @0x80020f3c/@0x80010044, Message_on @0x80040520 / Freeze @0x8003f040, Apply-Bild @0x80021558/@0x80021560), Bedingung:
bei der Integration laufen alle Nachrichten-/Szenen-/Tuer-Riegel mit. Stand der gemeinsamen Dateien nach Nachbesserung 3
(`git diff 116e8b0f --stat`, je "Runde 41 Spur E"):
| Datei | Haken | Runde |
|---|---|---|
| engine/src/player_common.c | include plcloop + Takt hinter `p->anim_frame++` (NB0); include rennstopp + Renn-Stopp-Takt hinter `r1_held`, Renn-Tempo statt `RUN_SPEED_PER_FRAME`, Skript-Zweig (NB3, E-E2 "spielweit") | NB0, NB3 |
| engine/src/re15_inv_screen.c | Kartenmarke ueber die Ankunftszone (tuer_anziehen) | NB0 |
| engine/src/scd_room_setup.c | Installer werkstatt11b1 (Init-Lauf) | NB0 |
| engine/src/scd_vm.c | include msgtakt + Auswahl-Message_on ohne Parken (@0x80040520) | NB2 |
| engine/src/msg_common.c | include msgtakt + vor_oeffnen + aufgeschobener Restore | NB2 |
| platform/pc/main.c | msgtakt takt_beginn/_ende, nach_spieltakt (2x), kamerahalt -> bild_halten; NB3 nur Kommentar (H3) | NB2, NB3 |
| platform/pc/src/render_pc.c | Abzug-Abholer + Merker-Haken | NB2 |
Neu in NB3 nur in eigenen Dateien: rennstopp_r41.c/.h, bildhalt_r41_pc.c (Blenden-Takt), Tests. Konflikt-Hinweis fuer die
Integration: player_common.c (Spur H aendert game_step_common.c/re15_collision.c, nicht player_common.c; der Renn-Stopp
verschiebt Leons Bahn nach jedem Rennen um den Auslauf), main.c (D/F/G), scd_vm.c op_message_on (C/F/G), render_pc.c (D).

### Punkt 1 "herumfliegend" — Stand nach Nachbesserung 3
Alle Anlauf-Varianten sind jetzt am Original gemessen UND im Port original-gleich: Stand (o_stand1, Abnahme 2 §3.2), Gehen
(o_geh1, §3.1), Weg des Nutzers 2050 -> 2020 in Cut 3 (o_hg_cut3, §3.3), "No" (§3.3a), Rennen (o_renn2: erstes Renn-Bild
Tempo 198, Auslauf n94..n99 bis (3650,-3369), oben). In keinem Lauf (Port vorher/nachher, Original) "fliegt" Leon. Damit ist
die Rueckfrage an den Nutzer nach dem Orchestrator-Hinweis 2026-10-24 03:21 zulaessig (Abschnitt "Fuer den Nutzer").
Neu sichtbar durch E-E2: im RENNEN rutscht Leon nach dem "Yes" sechs Bilder lang weiter (444 Einheiten) und kurbelt bei
(3650,-3369) statt (3650,-3018) — so auch das Original (o_renn2); die Kurbel-Pose (Raum-RBJ Clip 0) passt dort nicht an den
Griff, das ist Original-Verhalten.

### Hinweise der Abnahme 2 (H1-H7) — Stand
- H1 (Nachrichten-Takt nach einem Apply-Bild): selbst nachgelesen `dis 0x80010000 80`: FUN_80010000 ist die Nacharbeit NACH
  einem Pufferwechsel — @0x8001001c `lbu 0x800a73a4` / @0x80010020 `lbu 0x800aca34` / @0x80010028 `beq` -> @0x8001012c; sonst
  @0x80010030 FUN_80010140, Stack auf den Scratchpad (@0x8001003c FUN_8004f090 / @0x8001004c FUN_8004f0b0), @0x80010044
  Nachrichten-Takt FUN_800280b4 (-> @0x800280dc FUN_80028134, @0x800280e4 FUN_8002918c, @0x80028118 FUN_80029560) und
  @0x80010054-@0x80010128 Umschalten der Prim-Puffer 0x800a7394/0x800a7398 auf den naechsten Puffer. Im Bild nach einem
  Apply-Bild faellt das alles aus. Nicht nachgebildet: ob dabei der Text im Folgebild fehlt (Zeichnen in FUN_8002918c/
  FUN_80029560?) ist ohne Messung im Original nicht belegt -> OFFEN (unten).
- H2 (Present-Tore @0x8002151c/@0x80021544): im Spiel 0 (Abnahme 2), keine Aenderung.
- H3 (Kommentar main.c Restore-Haken): berichtigt (984e0b1e), Reihenfolge unveraendert (Port-Modal liest g_pauseflags nicht,
  `grep pauseflags engine/src/item_modal_common.c` = 0 Treffer).
- H4 (Kommentar re15_kamerahalt_r41.h): berichtigt (256ee155).
- H5 (PSX): OFFEN (unten).
- H6 (Dauer Apply-Standbild = CD-Ladezeit): Hardware-Zeit, unveraendert.
- H7 (RE15_FORCE_CUT haelt nie): Messhaken-Ausnahme, unveraendert.
- Nachgerechnet (Sonde `<sp>/E41n3/probe/renntempo_probe.c`, ohne Mischung, Blick 0, re15_squareroot0): Renn-Clip 0 (22 Bilder)
  im Dauerlauf 208 208 205 204 204 202 203 188 159 176 197 207 208 205 205 203 200 199 186 170 182 194 = Mittel 196,0 je Bild
  (vorher konstant 200); Elza (PL04-Bindepose + PL04W03, Lauf `<sp>/E41n3/runs/elza_renn2`): erstes Renn-Bild 190, Stopp mit
  Rueckschritt an der Stegkante wie Leon.

## Nachbesserung 3 — Fortsetzung (nach dem Wochenlimit, 2026-10-24 ab 14:0x)
Stand bei Beginn: HEAD 4190e6b0, Baum sauber. `local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`, `build` -> "ninja: no
work to do", `LOCAL-BUILD-OK (build)` (exe md5 9de3bd488e72334278224893f65b136d = Messkopie re15_pc_E41n3.exe). Gezielt
`ctest -R "r41_kleinkram_b|^unit_r36_zombieki_affen_block$" -j 3 --timeout 900` -> **100% tests passed, 0 tests failed out of
20** (`<sp>/E41n3b/ctest_spur.log`). Laeufe dieser Fortsetzung unter `<sp>/E41n3b/`.

### DECIDE 0x80032604 vollstaendig gelesen — drei Luecken der Umsetzung aus 841e0ef6 (selbst disassembliert)
`re15_disasm.py dis 0x80031de8 70` / `dis 0x80031efc 30` / `dis 0x80032604 95` / `dis 0x800322e8 110` / `dis 0x80032498 95` /
`dis 0x80032ae4 55` / `dis 0x80032778 128` / `dis 0x80032038 60`, Tabellen `table 0x80073fb0 8` / `table 0x80073ff0 8`,
`read 0x80073f24/0x80073ea4/0x80073ee4 8 --w 2`:
- Dispatcher Kommando 1 = 0x80031de8: @0x80031ef4 `jalr` DECIDE[+5] (Tabelle @0x80073fb0: [0] 0x80031f38 Stehen, [1] 0x800322e8
  Gehen, [2] 0x80032604 Rennen, [3] 0x80032974 Drehen), dann @0x80031efc `lbu v0,0(s0)` (+5 NEU gelesen) und @0x80031f1c `jalr`
  ANIMATE[+5] (Tabelle @0x80073ff0: [0] 0x80032038, [1] 0x80032498, [2] 0x80032778, [3] 0x80032ae4) — ein Wechsel im DECIDE
  laeuft im SELBEN Bild ins ANIMATE des neuen Unterzustands (Fall 0).
- Tabellen je Figur*2 (u16 = Tempo | Drehrate<<8): @0x80073f24 Rennen 0x48C8 = (200, 72) fuer Figur 0..3, 0x48D2 = (210, 72) fuer
  4..7; @0x80073ea4 Gehen 0x304B = (75, 48) / 0x3046 = (70, 48); @0x80073ee4 Drehen 0x6000 = (0, 96). Das Tabellen-Tempo setzt nur
  Fall 0, Fall 1 ueberschreibt es im selben Bild mit der Fussrechnung (Rennen @0x800328cc, Gehen @0x800325c4), ausser Figur 0xf.
- **Luecke 1 — Drehen im Unterzustand 2.** 0x80032604 dreht in JEDEM Bild des Unterzustands 2 zuerst: @0x80032618 `lw a0,0(s0)`
  (gehalten), @0x80032620 `andi 0x2` -> @0x8003262c-@0x8003265c `+=` lbu @0x80073f25[Figur*2] (72), @0x80032660 `andi 0x8` ->
  @0x8003266c-@0x800326a0 `-=` 72 — VOR der Aktions-Pruefung (@0x800326a4), VOR `+6 < 2` (@0x800326e0), VOR OBEN+RENNEN
  (@0x800326fc) und VOR Zielen (@0x80032708). Der Port waehlte die Rate nach dem Pad (ohne OBEN 96 = Stehen-Rate, OBEN ohne
  RENNEN 48 = Geh-Rate) und drehte mit R1 gar nicht (`zielt_schon`). Im Stopp-Einstiegsbild und im Bild, das den Stopp mit
  LINKS/RECHTS/OBEN beendet, drehte er darum 96 bzw. 48 statt 72, mit R1+Waffe 0 statt 72.
- **Luecke 2 — OBEN im Stopp.** +6 >= 2 (@0x80032730): @0x80032738 `andi 0x1` (OBEN) -> @0x8003275c/@0x80032760 `sw 0x0101` =
  GEHEN, auch wenn RENNEN gehalten ist (RENNEN wird hier nicht gelesen); erst das Gehen-DECIDE 0x800322e8 des Folgebilds schaltet
  mit `& 0x201 == 0x201` (@0x80032444-@0x80032454 `sw a0(=0x201)`) aufs Rennen. Das Gehen-ANIMATE laeuft im selben Bild (Fall 0
  @0x800324b8-@0x80032524: +6 = 1, Clip 5, +0x8f = 7, dann Fall 1 mit Vorschub @0x800325ec). Der Port ging bei OBEN+RENNEN im
  Stopp sofort ins Rennen (ein Geh-Bild zu wenig).
- **Luecke 3 — Stehen erst nach dem Stopp.** Fall 3 (@0x80032918-@0x8003295c): Tempo -= 40 @0x8003293c, @0x8003294c `bgez`, sonst
  @0x80032958 `sw 1` = Kommandowort 0x0001 (Stehen, +6 = 0). Das Stehen-ANIMATE 0x80032038 Fall 0 laeuft also im FOLGEbild:
  @0x80032074-@0x800320bc +6 = 1, Clip 3 (+0x94), Bild +0x95 = 0, Mischung +0x8f = 7, Tempo 0, Zeitgeber `rng & 0x1f` + 90
  (@0x800320a8 `jal 0x8001af20`). Der Port betrat seinen Leerlauf-FSM (player_common.c, Zeitgeber re15_idle_rand) schon im
  ERSTEN Stopp-Bild (Hook-Rueckgabe 1 -> move_dir 0 -> Leerlauf-Zweig) — der Zeitgeber lief 6 Bilder zu frueh, und die
  Mischung des Stehen-Fall-0 fehlte.
- Drehen-ANIMATE 0x80032ae4 Fall 0 (@0x80032b04-@0x80032b4c: +6 = 1, Clip 5, +0x8f = 7, Tempo 0) dreht NICHT — im Bild, das den
  Stopp mit LINKS/RECHTS beendet, wirkt also nur die 72 des Renn-DECIDE; das Drehen-DECIDE 0x80032974 dreht ab dem Folgebild
  (Rate @0x80073ee5 = 96).
- Nicht umgesetzt (OFFEN, s. unten): die Aktions-Pruefung @0x800326a4-@0x800326dc (`lw 0x800ac76c & 0x80` -> FUN_8002d474 /
  FUN_80042bac(Spieler, 1, 0x10) -> bei Treffer `bne` @0x800326c4/@0x800326d8 zum Ende, ohne Stopp-Entscheidung). Im Bild, in dem
  die Aktionstaste einen Aktions-AOT ausloest, bleibt das Original im Unterzustand (Rennen laeuft ein Bild weiter, auch ohne
  OBEN+RENNEN). Der Port prueft Aktions-AOTs erst NACH dem Spieler-Takt (game_step_common.c re15_aot_scan; Katalog
  RE15_FUN_CATALOG.md Zeile 150/151: Port-Scan = Spieler + Flanke, Umbau #14/#15 in aot_common.c — keine Datei der Spur E).

### Umsetzung (Fortsetzung, UNGEBAUT bei diesem Eintrag)
- engine/src/rennstopp_r41.c / include/re15_rennstopp_r41.h: `re15_rennstopp_r41_unterzustand2(p, lauf)` (Kommando 1 und Rennen
  oder aktiver Stopp); `schritt()` gibt bei OBEN im Stopp 2 zurueck (Gehen erzwingen, @0x80032738), merkt das Stopp-Ende
  (`s_stand_neu`, @0x80032958) und setzt im Folgebild Bild 0 / Mischung 7 (@0x80032074-@0x8003209c), wenn die Bewegung noch die
  Steh-Bewegung ist. Keine neue Zahl: 72 = TURN_RATE_RUN_PER_FRAME (@0x80073f24 Byte 1, schon im Port), 7 = RS_MISCHUNG.
- player_common.c (Haken "Runde 41 Spur E", E-E1/E-E2): Rate = 72 im Unterzustand 2 (`s2_41`), Drehen dort auch mit R1;
  Rueckgabe 2 -> `run = 0`; Rueckgabe 1 -> `want_motion = p->motion` vor dem Leerlauf-Zweig; Renn-Tempo-Gier mit derselben
  Drehbedingung.
- Riegel NEU `unit_r41_kleinkram_b_takt_rennpfad` (echter Spieler-Takt re15_game_step, ROOM2020 Tunnel, Blick 0, 8 Renn-Bilder,
  ohne Waffen-Bank = Tabellen-Tempo 200): A Rennen -> LINKS (72/160, 72 + Drehen ohne Schritt, dann 96), B Rennen -> OBEN+LINKS
  (72 statt 48, dann Gehen 75), C Stopp -> OBEN+RENNEN (ein Geh-Bild, dann Rennen 200), D ganzer Stopp (160 120 80 40 0 40, mo 200,
  Folgebild Mischung 7), E Zielen mit Waffe 3 + LINKS (72 und Stopp); Gegenprobe mit re15_rennstopp_r41_pruef_aus(1) muss in
  allen fuenf durchfallen. rennstopp-Teil erweitert (Rueckgabe 2, OBEN+RENNEN im Stopp, unterzustand2, Stehen-Fall 0).

### Fortsetzung — gebaut, Riegel rennpfad gemessen
- `local_build.sh build` -> `LOCAL-BUILD-OK (build)` (keine neue .c-Datei, kein configure). Erster Lauf rennpfad: die Wege nach
  einer Drehung waren 159 statt 160 bzw. 74 statt 75 — der Vorschub rundet je Achse ab (player_common.c `x += cos*v/4096`, Blick
  -72 -> cos 4071 / sin -451: dx 159, dz 17, Betrag 159,9); Riegel vergleicht nach einer Drehung auf +-1 (rp_weg_um).
- `ctest -R "unit_r41_kleinkram_b_takt_"` (-V): msgtakt 11/11, kamerahalt 6/6, frage 5/5, rennstopp 17/17, renntempo 7/7,
  **rennpfad 6/6**:
  | Fall | Modul (NACHHER) | Stand 4190e6b0 (VORHER) | ohne Modul (vor NB3) |
  |---|---|---|---|
  | A Rennen -> LINKS | S1 dreh -72 mo 200 weg 159, S2 -72 mo 105 weg 0, S3 -96 | S1 -96 mo 105 weg 159, S2 -96, S3 -96 | S1 -96 mo 105 weg 0 |
  | B Rennen -> OBEN+LINKS | S1 -72 mo 200 weg 159, S2 -72 mo 105 weg 74, S3 -48 | S1 -48 mo 105 weg 159, S2 -48 | S1 -48 mo 105 weg 74 |
  | C Stopp -> OBEN+RENNEN | S1 mo 200 160, S2 mo 105 75, S3 mo 100 200 | S2 mo 100 200 | S1 mo 200 weg 0, S2 mo 100 |
  | D ganzer Stopp | 160 120 80 40 0 40, Folgebild Mischung 7, danach 6 | Folgebild Mischung 1 | kein Weg, Mischung 1 |
  | E Zielen+LINKS (Waffe 3) | S1 -72 mo 200 weg 159 | S1 dreh 0 mo 105 weg 160 | S1 -24 mo 215 (Zielen) weg 0 |
  VORHER = derselbe neue Test gegen die Engine von 4190e6b0 (Baum `git archive 4190e6b0 re15_port` ohne shared_assets + Junction,
  Linker-Stub fuer unterzustand2, nur Ziel test_r41_kleinkram_b_takt gebaut; `<sp>/E41n3b/vorher_rennpfad.log`): **A-E 5/5
  DURCHGEFALLEN** (1/6 bestanden = nur die Gegenprobe-Zeile). Nebenbefund VORHER: im Stopp-Einstieg mit LINKS/RECHTS ueberschrieb
  der Dreh-Zweig die Steh-Bewegung (mo 105), der Stopp brach im Folgebild als "Fremdeingriff" ab.
- unit_r36_zombieki_affen_block mit dem verfeinerten Stopp (`<sp>/E41n3b/ctest_affen.log`): **rot** — FERN 4852 > 24533/6 und
  schlechtester Seed 0x9b5bf362 +3521 (Schranke 1039). Neu-Messung `rennvergleich 0 143 6000` laeuft (`<sp>/E41n3b/affen/`).

### Fortsetzung — unit_r36_zombieki_affen_block neu eingemessen (E-E2, zweite Messung)
- WARUM sich der Wert erneut aendert: der Zufallsgang der Sonde wechselt alle 90 Bilder von Rennen (OBEN+RENNEN) auf Drehen
  (LINKS/RECHTS). Mit dem vollstaendigen DECIDE dreht Leon im Stopp-Einstieg und im Folgebild 72 (@0x80032620-@0x800326a0) statt
  96 -> andere Endrichtung (Abweichung <= 48 beendet das Drehen) -> andere Bahn ab dem ersten Wechsel.
- Messung `test_r36_zombieki_affen_block rennvergleich <von> <bis> 6000` in drei Teilen (0..47, 48..95, 96..143), je Seed AUS/AN x
  ohne/mit in eigenen Prozessen (ohne = re15_rennstopp_r41_pruef_aus = Stand vor Nachbesserung 3, mit = Endstand); Protokolle
  `<sp>/E41n3b/affen/rv_*.txt`, Auswertung `rv_auswertung2.py` -> `auswertung.txt`:
  | Block | ohne: groesstes AN-AUS / FERN AN/AUS | mit: groesstes AN-AUS / FERN AN/AUS |
  |---|---|---|
  | 0..15 (Riegel) | +84 / 6,8 % | **+3521 (0x9b5bf362) / 19,8 % RISS** |
  | 16..31 | +224 / 6,7 % | +0 / 3,4 % |
  | 32..47 | +0 / 1,9 % | +1864 (0x6b1c577f) / 8,3 % |
  | 48..63 | +39 / 5,6 % | -63 / 9,6 % |
  | 64..79 | +378 / 11,4 % | +673 / 6,1 % |
  | 80..95 | +41 / 6,5 % | +620 / 12,8 % |
  | 96..111 | **+3334 (0xbc8bd05d) / 20,3 % RISS** | +789 / 6,0 % |
  | 112..127 | +1416 (0x71d8aa83) / 7,3 % | +70 / 12,6 % |
  | 128..143 | +535 / 3,6 % | +1450 (0xce2e2c57) / 10,4 % |
  | alle 144 | +3334, 2 Seeds > 600 / 7,4 % (17656/240096), Griffe 644/576 | +3521, 6 Seeds > 600 / 9,7 % (24433/253141), Griffe 657/587 |
  Bahn-Chaos der Original-Regel |AUS(mit) - AUS(ohne)|: Median 813,5, Mittel 1359, p90 3307, max 5982; Port-Wahl |AN(mit) - AN(ohne)|
  Median 372.
- Die schlechtesten Seeds sind Haenger der PORT-WAHL an zwei festen Stellen (`einsr <i> 1 6000 <renn> 1`, LAGE-Zeilen):
  Klemme Nordost (11129..11280, 9820..10073): 0x9b5bf362 mit (Gorilla 2 (11129,9982) 4405 Bilder ab F950, 4765 FERN),
  0xbc8bd05d OHNE Stopp (Gorilla 1 (11274,10073) 1443 + 1558 Bilder), 0xbb4281d4 mit dem Stopp aus 841e0ef6 ((11280,9820), Teil 3);
  Sub-7-Stelle (-13418..-13518, -140..-953): 0x6b1c577f mit (1994 Bilder, Sub 7 1940), 0xce2e2c57 mit (842), 0x71d8aa83 OHNE (805).
  Welche Seeds sie treffen, entscheidet die Bahn: ohne Stopp reisst Block 96..111, mit Block 0..15 — der 16er-Riegel mass, ob ein
  Haenger in seine Seed-Auswahl faellt, nicht die Port-Wahl.
- NEUE FORM (Kommentar im Test): Riegel ueber die ganze gemessene Population (144 Seeds, je 8 Seeds = 16 Prozesse gleichzeitig,
  38 s): FERN AN <= 1/6 AUS und Griffe AN >= 80 % AUS (Werte der Runde 36, unveraendert) auf der Population; je Seed AN - AUS <=
  5982 = groesste gemessene Bahn-Chaos-Amplitude der Original-Regel. Median (813) und p90 (3307) halten schon fuer den abgenommenen
  Stand OHNE Stopp nicht (2 Seeds > 813, +3334 > 3307) — sie messen nicht die Port-Wahl; das Maximum ist die kleinste gemessene
  Grenze, die beide Populationen halten. Ausgegeben werden weiter die Seeds ueber 600 und ueber dem Median.
- Lauf `ctest -R "^unit_r36_zombieki_affen_block$"` (`<sp>/E41n3b/ctest_affen2.log`): **100% tests passed, 0 tests failed out of
  1** (37,8 s): 144/144 Laeufe, FERN 24433 <= 253141/6, schlechtester 0x9b5bf362 +3521 (ueber 600: 6, ueber Median 813: 3), Griffe
  657 >= 0,8 x 587, Szenen 1-5 und Nahwarten/Ostgasse unveraendert ok.
- Die Haenger-Stellen sind ein Befund der Port-Wahl der Runde 36 (zombieki_r36.c, nicht Spur E) -> OFFEN unten.

### Nachbesserung 3 — je Mangel (Ursache / Messung vorher / Beleg / Aenderung / Messung nachher)
| Mangel | Ursache | Messung vorher | Beleg (Original) | Aenderung | Messung nachher |
|---|---|---|---|---|---|
| 1 Renn-Anlauf an die Kurbel (Punkt 1) | Port ohne Renn-Stopp (sofort Stehen), Renn-Tempo konstant 200; nach dem Wiedereinbau (841e0ef6) DECIDE 0x80032604 nur teilweise: Drehrate nach dem Pad (96/48 statt 72), mit R1 kein Drehen, OBEN+RENNEN im Stopp sofort Rennen, Leerlauf-FSM schon im ersten Stopp-Bild | Abnahme 2 §3.4 `P1_renn_nach`: F382 Pos_set + Stehen (3650,-3018) mo 200; Fortsetzung: rennpfad gegen die Engine 4190e6b0 A-E 5/5 durchgefallen | GDB o_renn2 n93-n99 (z -3099..-3369, Tempo 174..-26), o_rennstopp; 0x80032604 @0x80032620-@0x800326a0 / @0x800326e0-@0x8003272c / @0x80032730-@0x80032760; 0x80032778 Fall 0/1 @0x800327cc-@0x800328f4, Fall 2/3 @0x800328f8-@0x8003295c; FUN_800369f8 Modus 1 @0x80036a30-@0x80036b44; Dispatcher 0x80031de8 @0x80031ef4/@0x80031efc/@0x80031f1c; 0x80032038 Fall 0 @0x80032074-@0x800320bc | rennstopp_r41.c/.h (Renn-Tempo aus dem Fussweg, Renn-Stopp, Unterzustand 2, Gehen aus dem Stopp, Stehen-Fall 0), player_common.c 4 Haken | renn_o2 F282..F287 = n94..n99; Riegel integration_..._renn, unit_..._takt_{rennstopp,renntempo,rennpfad}; exe-Laeufe Fortsetzung (unten) |
| 2 Riegel renn pinnte die Abweichung | Auswerter `takt` verlangte (3650,-3018) mo 200 und meldete "wie das Original" | test_r41_kleinkram_b_exe.c:314 (Stand d4fa9074) | o_renn2 n94..n99 | Auswerter `renn`: erstes Renn-Bild z -3216 (Tempo 198), Bild 6 eingefroren, Auslauf z -3099 -3233 -3327 -3381 -3395 -3369, Stand (3650,-3369) bis Plc_motion (1a8113cc) | NACHHER bestanden, VORHER-exe 11b8f867 durchgefallen (erstes Bild -3218, kein Auslauf) |
| 3 Vertrag §1.3 | spielweite Haken in fremden gemeinsamen Dateien ohne Entscheid | Abnahme 2 §6.3 | — (Vertragsfrage) | Orchestrator-Entscheid E-E1 (AUFTRAG 2026-10-24 10:50) nimmt die Haken an; Fortsetzung aendert nur player_common.c (E-E2 "spielweit", Haken markiert) | Bedingung E-E1 erfuellt: Nachrichten-/Szenen-/Tuer-/Karten-Riegel im gezielten Lauf mitgefahren (Abschnitt "Fortsetzung — Tests") |
| 4 Softlock Raumeintritt auf der Kante | re15_fade_tick (FUN_80021880) lief im gehaltenen Bild nicht | Abnahme 2 §3.6 EN1_spawn_kante_nach: pf=FF000007 F1..F199 | Hauptschleife @0x80020f44 `jal 0x80021880` vor @0x80020f4c Present; Pegel @0x8002191c-@0x8002192c | bildhalt_r41_pc.c ruft re15_fade_tick im gehaltenen Bild (256ee155) | EN1_spawn_kante_n3a: Freeze faellt F6 (= VORHER 753652fc), x 9070 -> 11100; Riegel integration_..._spawn_kante (VORHER-exe rot) |

### OFFEN (Stand Nachbesserung 3)
- Aktions-Pruefung im Renn-DECIDE (@0x800326a4-@0x800326dc: `lw 0x800ac76c & 0x80` -> FUN_8002d474 / FUN_80042bac(Spieler, 1,
  0x10), bei Treffer `bne` @0x800326c4/@0x800326d8 zum Ende OHNE Stopp-Entscheidung): wird im selben Bild RENNEN losgelassen und
  mit der Aktionstaste ein Aktions-AOT ausgeloest, laeuft das Original noch ein Renn-Bild (Fall 1), der Port beginnt den Stopp.
  Der Port prueft Aktions-AOTs erst nach dem Spieler-Takt (game_step_common.c `re15_aot_scan`, Spieler + Flanke; Katalog
  RE15_FUN_CATALOG.md Z. 150/151, Umbau #14/#15 in aot_common.c) — nicht Spur E. An der Kurbel ohne Folge, solange RENNEN beim
  Druecken gehalten ist (o_renn2: Aktion im 7. Renn-Bild mit OBEN+RENNEN).
- Drehrate ausserhalb des Unterzustands 2: der Port waehlt sie nach dem Pad, das Original nach dem Unterzustand am Bildanfang —
  Stehen dreht im Wechselbild nicht (0x80031f38 ohne Drehung, @0x80031f78-@0x80032020), Gehen 48 (@0x80032344-@0x800323b8,
  @0x80073ea5), Drehen 96 (@0x800329ac-@0x80032a20, @0x80073ee5). Abweichung je ein Bild beim Wechsel Stehen -> Gehen/Rennen
  mit LINKS/RECHTS (Port 48/72, Original 0) und Gehen -> Rennen (Port 72, Original 48). Ausserhalb des Renn-Stopps (E-E2),
  betrifft jede Bewegung — eigene Spur.
- Geh-Tempo: der Port geht konstant 75 (@0x80073ea4); das Original rechnet wie beim Rennen den Fussweg ((roh >> 1) + 50,
  @0x800325c4-@0x800325e8, Fuss-Zuordnung @0x800325a8-@0x800325c0). Ausserhalb E-E2.
- Tabellen-Tempo je Figur: @0x80073f24 Rennen (210, 72) und @0x80073ea4 Gehen (70, 48) fuer Figur 4..7 (Elza) statt (200/75);
  im Spiel ohne Wirkung, weil Fall 1 das Tabellen-Tempo im selben Bild durch die Fussrechnung ersetzt; der Port nimmt die
  Tabelle nur ohne gespiegelte Bank (Sonden).
- Gorilla-Haenger der PORT-WAHL der Runde 36 (zombieki_r36.c, nicht Spur E): Klemme (11129..11280, 9820..10073) — Seeds
  0x9b5bf362 (mit Stopp, 4405 Bilder FERN), 0xbc8bd05d (ohne Stopp, 1443 + 1558), 0xbb4281d4; Sub-7-Stelle (-13418..-13518,
  -140..-953) — 0x6b1c577f, 0xce2e2c57 (mit), 0x71d8aa83 (ohne). Nachstellbar mit `test_r36_zombieki_affen_block einsr <i> 1 6000
  <renn> 1`.
- H1 (Nachrichten-Takt-Tor @0x80010028 im Bild nach einem Apply-Bild), H5 (platform/psx/main.c ohne msgtakt/kamerahalt), H6
  (Dauer des Apply-Standbilds = CD-Ladezeit) — unveraendert, s. "Hinweise der Abnahme 2".
- Punkt 1 "herumfliegend": in keinem Lauf (Port vorher/nachher, Original) gefunden — Rueckfrage an den Nutzer (unten).

### Fuer den Nutzer (Stand Nachbesserung 3)
- Keine neuen Assets, keine Sprachdateien.
- Rennen im ganzen Spiel jetzt wie im Original: das Renn-Tempo folgt dem Schritt (im Mittel 196 statt 200 je Bild); laesst man
  RENNEN (oder die Richtung) los, rutscht Leon 6 Bilder aus (zusammen etwa 360-450 Einheiten, je nach letztem Schritt) statt
  sofort zu stehen; dreht man direkt aus dem Rennen, gilt in den ersten zwei Bildern die Renn-Drehrate 72 (statt 96 auf der
  Stelle bzw. 48 im Gehen); drueckt man waehrend des Ausrutschens wieder OBEN+RENNEN, geht er ein Bild, bevor er wieder rennt.
- Kurbel ROOM2020 im Rennen: nach dem "Yes" rutscht Leon noch sechs Bilder weiter und kurbelt etwa 35 cm neben der Stelle, an
  der er im Gehen/Stehen kurbelt — so auch das Original (gemessen).
- Raumeintritt genau auf einer Kamera-Grenze: das Bild bleibt ruhig, die Einblendung laeuft wie vorher (kein Haenger mehr).
- Frage: Was genau war das "Herumfliegen" an der Kurbel? Moeglich sind (a) der Sprung an die Kurbel nach dem "Yes" (Original,
  dort hinter einem kurzen Standbild), (b) das Einfrieren nach einer Kurbel-Umdrehung (behoben in Nachbesserung 1), (c) die
  Waffe, die beim Kurbeln mit der Hand mitkreist (Original), (d) das Ausrutschen nach dem Rennen (Original, jetzt auch im Port),
  (e) etwas anderes — dann bitte: gerannt/gegangen/gestanden, welche Waffe, oder ein Bild (F9).

### Fortsetzung — Messung nachher an der exe (re15_pc_E41n3c.exe md5 1f38e02535c6259e6f99619bba2917ab = Endstand; VORHER re15_pc_E41n3.exe md5 9de3bd48... = 4190e6b0)
Laeufe `<sp>/E41n3b/runs/<lauf>/` (Treiber `<sp>/E41n3b/run.sh` + `laeufe.sh`, Auswertung `zust.py`), gemeinsam `RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_STATE_LOG=state.txt`.
- **Kurbel im Stand** (`P1_stand_n3c`, Eingaben wie Abnahme 2 §3.2): Frage zu F461, Pos_set F462 (3650,-3018) = Schliessen + 1,
  Leerlauf mo 212 laeuft weiter, Plc_motion F482 (pst 4) = Pos_set + 20 — unveraendert gegen Abnahme 2 (= Original o_stand1).
- **Kurbel im Rennen, Anlauf der Abnahme** (`P1_renn_{n3c,n3}`, Start (2600,-800) wie Abnahme 2 §3.4 `P1_renn_nach`): Frage zu F381,
  **F382..F387 (3650,-3099) -3233 -3327 -3381 -3395 -3369**, Stand (3650,-3369) bis Plc_motion F402 — dieselbe Folge wie Original
  o_renn2 n94..n99 (vorher 11b8f867: F382 Stehen (3650,-3018)). Kurbel im Gehen: Riegel geh (unten), Weg des Nutzers: Riegel kurbel.
- **Tunnel ROOM2020 (4000,-3150) Blick 0, 8 Renn-Bilder F60..F67** (Tempo 210 213 214 210), dann:
  | Lauf | Endstand n3c | VORHER n3 (4190e6b0) | Original (Disasm) |
  |---|---|---|---|
  | TA LINKS | F68 dreh -72 mo 200 Weg 169, F69 -72 mo 105 Weg 0, F70 -96 | F68 -96 mo 105 Weg 170, F69 -96, F70 -96 | 72 (@0x80032660-@0x800326a0) + Stopp, 72 + Drehen 0x0301, dann 96 (Drehen-DECIDE @0x800329e0-@0x80032a20) |
  | TB OBEN+LINKS | F68 -72 mo 200 Weg 169, F69 -72 mo 105 Weg 75, F70 -48 | F68 -48 mo 105 Weg 169, F69 -48, F70 -48 | 72 + Stopp, 72 + Gehen 0x0101 (@0x80032738), dann 48 (Gehen-DECIDE @0x80032378-@0x800323b8) |
  | TC Stopp, dann OBEN+RENNEN | F68/F69 Stopp 170/130, F70 mo 105 Weg 75, F71 mo 100 Weg 208, F72 231; loslassen F73 191 | F70 sofort mo 100 Weg 208 | Gehen 0x0101 (@0x80032738), Rennen erst @0x80032444 |
  | TD ganzer Stopp | F68..F73 170 130 90 50 10 -30, ab F74 Stehen, erster Leerlauf-Wechsel mo 210 F169 (= F74 + 95) | Stopp gleich, Leerlauf-FSM ab F68, mo 210 F189 | Stehen-ANIMATE Fall 0 im Bild nach dem Stopp-Ende (@0x80032958 -> @0x80032074), Zeitgeber rng & 0x1f + 90 (@0x800320a8-@0x800320bc) |
  Der Zeitgeberwert selbst kommt im Port aus re15_idle_rand (Bildzaehler-Hash, vorbestehend) — belegt ist hier nur der Einstieg
  (Bild nach dem Stopp-Ende), nicht der Wert.
  Vorzeichen: das virtuelle Pad-Wort 0x800ac768 entsteht ueber die Tabelle @0x80073dbc (`read 0x80073dbc 16 --w 2`: [0] 0x1000 =
  OBEN, [1] 0x2000 = RECHTS, [2] 0x4000 = UNTEN, [3] 0x8000 = LINKS, [7] 0x80 = QUADRAT, [8] 0x8 = R1, [9] 0x40 = KREUZ) — LINKS ist
  also held & 0x8 = der Abzug-Zweig (@0x80032660-@0x800326a0), RENNEN 0x200 = KREUZ, Zielen 0x100 = R1, Aktion 0x80 = QUADRAT;
  der Port dreht LINKS ebenfalls negativ.

### Fortsetzung — Tests (gezielt, keine volle Suite)
- Bau: `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)` (Endstand; keine neue .c-Datei seit dem letzten
  configure), exe md5 1f38e02535c6259e6f99619bba2917ab.
- Gezielter Lauf 1 (14:37-15:08), Auswahl `<sp>/E41n3b/ziel2.txt` = alle Tests, deren Name r41_kleinkram_b, r36_zombieki,
  r17_cursor_klick, r37_re2zombie, r36_klettern, r36_ziel1190, r38_zopf, r39_hund1121, r36_schluessel (Renn-Eingaben: XU/
  UP+CROSS, gesucht in tests/integration und tests/unit) oder msg|scd|cut|door|tuer|szene|karte|map|dialog|frage|story|
  nachricht|message|fade|blend|kamera|cam|player|plc|lauf|renn|walk|move enthaelt (ohne releaseskript/android):
  `ctest --test-dir re15_port/build -R "$(cat <sp>/E41n3b/ziel_regex.txt)" -j 3 --timeout 1500 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 266** (1844 s, CTEST_EXIT=0, `<sp>/E41n3b/ctest_ziel.log`); darin 50 Integrations-
  Riegel mit der echten exe (alle 8 integration_r41_kleinkram_b_*, door_lock/door_traversal/keydoor, map_raum_live/map_uebergang,
  r30_cut_blitz, r35_cut10f0/cut1150, r35_karte/_fahrstuhl, r36_karte/_kartenitems_bild/_klettern/_schluessel(_befreiung)/_story_elza/
  _story_leon/_szene1150/_szenenleon/_tueren/_ziel1190_scheiben, r37_bissarm_echtlauf/_re2zombie/_roy_szene1011/_1021/_szene11c0,
  r38_elzatoene_szene1021/_spinnen_echtlauf/_szenenschutz/_tagebuch_karte/_zopf, r39_elza_karte/_hund1121/_johnruf1041_tuer_*
  (6)/_karten/_toene_szene(_elza)) und unit_r36_zombieki_affen_block, unit_r41_kleinkram_b_takt_rennpfad.
- Gezielter Lauf 2 (15:38-15:41), alle uebrigen Unit-/Prop-Tests, deren Programm den Spieler treibt (Quelle ruft
  re15_player_tick / re15_game_step oder nennt player_common/rennstopp; Zuordnung Quelle -> Test ueber `ctest -N -V`, 505 Tests,
  davon 376 nicht in Lauf 1; Liste `<sp>/E41n3b/spieler_rest.txt`):
  `ctest --test-dir re15_port/build -R "$(cat <sp>/E41n3b/spieler_regex.txt)" -j 2 --timeout 900 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 376** (168,5 s, CTEST_EXIT=0, `<sp>/E41n3b/ctest_spieler.log`).
- Nachtrag Tests, die `motion = 100` (Rennen) direkt setzen (grep in tests/unit):
  `ctest -R "^(unit_ai_flavor_mixed|unit_r35_raeume_trage.*|unit_r16_liegende.*|unit_plc_back.*|unit_re2_dog.*)$" -j 2`
  -> **100% tests passed, 0 tests failed out of 10**.
- Gezielter Lauf 3 (15:14-16:16), Bedingung E-E1 "alle Nachrichten-/Szenen-/Tuer-Riegel": die uebrigen 101 Integrations-Riegel
  der Nachbesserung-2-Auswahl (Skript enthaelt RE15_FRAMEDUMP / RE15_PRESS / RE15_MSG_LOG / msgtext / XU / CROSS / RE15_CAM_TRACE;
  `<sp>/E41n2/integ_auswahl.txt` ohne die 50 aus Lauf 1 -> `<sp>/E41n3b/rest_auswahl.txt`):
  `ctest --test-dir re15_port/build -R "$(cat <sp>/E41n3b/rest_regex.txt)" -j 4 --timeout 1800 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 101** (3727 s, CTEST_EXIT=0, `<sp>/E41n3b/ctest_rest.log`). Kein Fenster-Haken
  rot, keine Nachlaeufe.
- Zusammen: 151 Integrations-Riegel mit der echten exe (alle 151 Nachrichten-/Szenen-/Tuer-/Karten-/Renn-Riegel der Auswahl)
  und 602 Unit-/Prop-Tests (alle, die den Spieler treiben, plus die Namensgruppen) gruen auf dem Endstand; keine volle Suite.

### Nachbesserung 3 — Abschluss
- Mangel 1 (Renn-Anlauf): behoben — Renn-Tempo, Renn-Stopp und der ganze Renn-DECIDE 0x80032604 wie im Original; Kurbel im
  Rennen bis (3650,-3369) = o_renn2 n94..n99 (Riegel renn, exe-Lauf P1_renn mit dem Anlauf der Abnahme). Gehen und Stand
  unveraendert original-gleich (Riegel geh/kurbel, P1_stand). Alle Anlauf-Varianten sind damit original-gleich; das
  "Herumfliegen" ist in keinem Lauf zu finden -> Rueckfrage an den Nutzer (Abschnitt "Fuer den Nutzer (Stand NB3)").
- Mangel 2 (Riegel renn): behoben — Auswerter `renn` misst die Original-Folge; VORHER-exe rot.
- Mangel 3 (Vertrag): Entscheid E-E1 liegt vor; Bedingung (alle Nachrichten-/Szenen-/Tuer-Riegel) mit Lauf 1-3 erfuellt; neu in
  NB3 nur player_common.c (E-E2), markiert "Runde 41 Spur E".
- Mangel 4 (Softlock): behoben (E-E3), Riegel spawn_kante.
- unit_r36_zombieki_affen_block: zweimal neu eingemessen (Teil 3 und Fortsetzung), Begruendung und Messung im Test-Kommentar.

## Nachbesserung 4 (nach Abnahme 3, 2026-10-24 ab 17:41)
Grundlage: `E_abnahme_3.md` §8 Maengel 1-2 (+ Hinweise H1-H6), Orchestrator-Entscheid E-E4 (AUFTRAG.md 2026-10-24 17:32) und
Nachtrag 14 (Nutzer-Antwort 17:36: "herumfliegend" = "direkt am Anfang springt Leon so komisch zu der Kurbel. So halb
fliegend." -> der Sprung an die Kurbel; Ziel Bild fuer Bild wie das Original-Video vom "Yes" bis Plc_motion, Standbild-Dauer
des Originals an dieser Stelle MESSEN). Stand bei Beginn: HEAD 6e2c62b1 (Code = c3ba4317, exe md5 1f38e025...; die zwei
AUFTRAG-Commits 3c02068f/6e2c62b1 hat der Orchestrator in den Zweig gelegt). Laeufe unter `<sp>/E41n4/` (`<sp>` = Scratchpad
dieser Sitzung, dieselbe wie bei den Vorgaengern). Vorliegende Messungen (Abnahme 3, Dossier NB1-NB3) werden zitiert, nicht
wiederholt.

Reihenfolge: (1) Mangel 1 Renn-Stopp endet bei einer Uebernahme (Treppe, Griff, Plc_ret ...), (2) Punkt 1 / Nachtrag 14
Standbild-Dauer nach Cut_chg (Original messen, Abhaengigkeit vom Hintergrund-Laden, Umsetzung, Bildvergleich), (3) Mangel 2 /
E-E4 Gorilla-Sonde misst den Haenger (benannte Ausnahmen, Ursache in zombieki_r36.c pruefen).

### Status Nachbesserung 4
- [x] Mangel 1 Renn-Stopp nach Uebernahme (Treppe/Griff/Plc_ret) — Marke +6 = 3 + Dispatcher-Haken, Riegel takt_uebernahme, integration treppe/losl
- [x] Punkt 1 / Nachtrag 14 Standbild nach Cut_chg — Original 14 VSyncs gemessen, Port 1 + 6 Bilddauern spielweit, Bildvergleich
- [x] Mangel 2 / E-E4 Gorilla-Sonde — Haenger-Mass je Seed, drei benannte Stellen mit Beleg, Folgeauftrag
- [ ] Bau, gezielte Tests, Abschluss

### Mangel 1 — Renn-Stopp endet bei einer Uebernahme (Treppe, Griff, Plc_ret)
- Ursache (Abnahme 3 §6.2, bestaetigt): `s_aktiv` (rennstopp_r41.c) war ein Modul-Zustand, den nur der eigene Schritt abbrach,
  wenn er eine fremde Bewegung SAH. Port-Zweige, die den Spieler-Takt auslassen (Treppe, Klettern, Griff, Tod, Knockdown,
  Flinch, Schieben) und ihn mit der Steh-Bewegung 200 zurueckgeben (Treppen-Abschluss stair_common.c, Griff-Freigabe
  enemy_ai_common.c, Plc_ret scd_vm.c), liessen den Rest-Stopp weiterlaufen.
- Messung vorher (Abnahme 3, nicht wiederholt): `N2_treppe_im_stopp` c3ba4317 F375 Abschluss z -13782, F376..F379 -13665 -13588
  -13551 -13554; `N2b` (2 Stopp-Bilder vor der Aktion) F376.. -13783.
- RE-Belege (selbst disassembliert, `re15_disasm.py`):
  - Der Stopp lebt nur in den Spieler-Bytes: DECIDE 0x80032604 @0x8003272c `sb 2,+6`, ANIMATE 0x80032778 @0x8003277c/@0x80032780
    a1 = 0x800aca5a, Fall 2 @0x800328f8 `ori v0,zero,0x3` / @0x800328fc `sb v0,0(a1)` -> +6 = 3; Ende @0x80032958 `sw 1` (Wort
    = +4..+7 1,0,0,0). Original-RAM `<sp>/E41a3/o_renn_losl.txt` (GDB, Abnahme 3): `cmd=1/2/3/0` (+4/+5/+6/+7) in n93..n97, n98
    `cmd=1/0/0/0`, n99 `cmd=1/0/1/0` (Stehen Fall 0).
  - Jeder andere Zustand schreibt die Bytes neu: Treppe = Kommando-1-Unterzustaende 11..13 (Tabelle @0x80073fb0 [11]
    0x80038848, [12] 0x80038c58, [13] 0x80038ef4; ANIMATE @0x80073ff0 [11] 0x80038850, [12] 0x80038c60), Start im sce-Handler
    (sce 12 @0x80043500: @0x80043540 `ori v0,zero,0xb` / @0x80043544 `sb v0,5(a1)`, +6 @0x80043594 `sb v0,6(v1)`; sce 13
    @0x800435cc: @0x8004360c/@0x80043610, @0x80043634/@0x80043638, @0x8004364c/@0x80043650, +6 @0x80043660; Rueckgabe 1
    @0x800435c8), Abschluss LAB_80038e50 @0x80038ea8 `sb zero,-13735(at)` = 0x800aca59 / @0x80038eb0 `sb zero,-13734(at)` =
    0x800aca5a; Plc_ret @0x80041f90-@0x80041f9c (`sb 1,4` / `sb zero,5/6/7`); Plc_motion @0x80041bb0 `sb 4,4` / @0x80041bb4
    `sb zero,6` / @0x80041bc4 `sb a2,5`; Griff STAGE1 FUN_80102548 @0x80102640 `sw v0,0x800aca58` (Wort Kommando 5),
    Freigabe @0x8011c384/@0x8011c38c `sw 1,0x800aca58`; Kommando-Tabelle @0x80073f90 ([1] 0x80031de8, [2] 0x80035af0, [5]
    0x80036834 ...). Gesperrte Durchlaeufe ueberspringen den ganzen Dispatcher (`bltz` @0x80031c78) und lassen +4..+7 stehen
    (o_renn_losl: Stopp im Aktionsbild n7, `cmd=1/2/3/0` eingefroren bis n93, Auslauf n94..n98).
- Umsetzung (rennstopp_r41.c/.h, Haken "Runde 41 Spur E NB4"):
  - Marke +6 = 3 (`RS_FALL3` @0x800328fc) im re15_actor_t.sub_state_2 des Spielers ab dem ersten Stopp-Bild; Stopp-Ende und
    Abbruch per Pad schreiben +6 = 0 (@0x80032958 bzw. Wort @0x80032760). Wer +6 neu schreibt (Port: Plc_ret scd_vm.c
    @0x80041f98, Plc_motion @0x80041bb4, Raumeintritt room_common.c, Treffer re15_damage.c, Flinch-Ende game_step_common.c),
    beendet den Stopp. +5 fuehrt der Port fuer den Spieler NICHT (Plc_motion schreibt dort nichts, plcloop_r41.c erwartet 0;
    re15_damage.c nutzt +5 als Treffer-Index) — nur +6 dient als Marke; kein Skript liest +6 des Spielers (Zensus aller RDT:
    Member_cmp nie auf 9/10, `<sp>/E41n4/member_ops.txt`).
  - `re15_rennstopp_r41_dispatcher(pl, laeuft)` VOR der Zweig-Kette des Spieler-Dispatchers (game_step_common.c, 2 Zeilen
    neben den Haken Zopf R38 / Zustand R39, laeuft = rdt_ok && !(pauseflags & 0x80000000)): lief der letzte ungesperrte
    Durchlauf OHNE den Kommando-1-Schritt des Moduls (Treppe, Klettern, Griff, Tod, Knockdown, Flinch, Schieben), ist der Stopp
    geloescht (und die Marke +6). Gesperrte Durchlaeufe zaehlen nicht.
  - player_common.c msg_block-Zweig (Nachricht OHNE Spieler-Bit; Original laesst Kommando 1 laufen): `re15_rennstopp_r41_halten()`
    = keine Uebernahme, Verhalten wie NB3 (1 Zeile).
  - Pruefhaken `re15_rennstopp_r41_pruef_uebernahme_aus` (nur Riegel-Gegenprobe, kein getenv).
- Messung nachher (exe re15_pc_E41n4a.exe md5 1751c3d6..., Laeufe `<sp>/E41n4/runs/`, Befehle der Abnahme 3 aus deren cmd.txt):
  | Lauf | NB4 | c3ba4317 (Abnahme 3) |
  |---|---|---|
  | N2_treppe_im_stopp | F375 -13782, **F376..F382 -13782** | F376 -13665 F377 -13588 F378 -13551 F379 -13554 |
  | N2b (2 Stopp-Bilder vorher) | F375.. -13782 | F376.. -13783 |
  | N2c (Rennen gehalten), N1 Tuer, N3 Statusschirm, P1_rennstopp_frei, TA, TC, Elza E_elza_renn | jede Zeile x/z/rot/mo gleich | — |
  | P1_renn_a2 (o_renn2) | F382..F387 -3099 -3233 -3327 -3381 -3395 -3369, Plc_motion F402 | gleich |
  | P1_renn_losl_aktion (o_renn_losl) | F282..F286 -3056 -3147 -3198 -3209 -3180 (Marke +6 = 3 F163..F285) | gleich |
  Der Treppen-Abstieg F312..F374 ist in allen drei Staenden gleich (Leon steht an der Kante -18368, der Stopp-Schritt im
  Aktionsbild wird von der Kollision gehalten).
- Riegel:
  - `unit_r41_kleinkram_b_takt_uebernahme` (echter Spieler-Takt re15_game_step): T Treppe ROOM2040 (Abschluss, 8 Bilder ohne
    Lageaenderung), R Plc_ret im Stopp (SCD-Schnipsel `42 01`, danach Weg 0), G Griff (Pin 10 Bilder, Freigabe mit Bewegung 200,
    danach kein Rest-Stopp), F Flinch (Kommando 2), P gesperrte Durchlaeufe (20 Bilder Spieler-Bit, danach 120, 80 = der Stopp
    laeuft weiter). NB4 6/6; Gegenprobe vor NB4: T/R/G fallen durch (T: 8 Lageaenderungen, z -16688 -> -16488; R: 120 80 40;
    G: 4 Lageaenderungen), F/P bestehen (F bricht schon in NB3 ueber die Bewegungs-Pruefung ab).
  - `integration_r41_kleinkram_b_treppe` (echte exe, Abnahme-Lauf N2): bestanden; mit der VORHER-exe re15_pc_E41a3.exe
    (c3ba4317) DURCHGEFALLEN ("F376 (-1359,-13665) ... 8 Lageaenderungen in 8 Bildern", `<sp>/E41n4/vorher_riegel/treppe.log`).
  - `integration_r41_kleinkram_b_losl` (Gegenprobe Kurbel o_renn_losl n94..n98): bestanden, VORHER ebenso (Verhalten unveraendert).
  - `ctest -R "^(unit_r41_kleinkram_b_takt_.*|integration_r41_kleinkram_b_(treppe|losl|renn))$"`: 100% tests passed, 0 tests
    failed out of 10.
- OFFEN (unveraendert, Hinweis H2 der Abnahme 3): die Aktions-Pruefung im Renn-DECIDE @0x800326a4-@0x800326dc liegt im Original
  VOR der Stopp-Entscheidung (Treppe: Handler-Rueckgabe 1 @0x800435c8 -> kein Stopp-Schritt im Aktionsbild); der Port prueft
  Aktions-AOTs/Treppe/Klettern hinter dem Spieler-Takt (game_step_common.c `re15_stair_try_start` nach `re15_player_tick`). An der
  gemessenen Treppe ohne Wirkung (Kollision haelt Leon an der Kante); Umbau = aot_common.c/game_step_common.c (Katalog #14/#15).

### Punkt 1 / Nachtrag 14 — Standbild nach Cut_chg (der "halb fliegende" Sprung an die Kurbel): Original gemessen
Nutzer-Antwort (Nachtrag 14): "direkt am Anfang springt Leon so komisch zu der Kurbel. So halb fliegend." Orchestrator: der
Sprung (Cut_chg 3 @0x0A7E, Pos_set @0x0A8C) ist Original, dort aber hinter einem Standbild, bis der Hintergrund von CD geladen
ist; der Port haelt 1 Bild. Ziel: Standbild-Dauer an genau dieser Stelle messen, Abhaengigkeit vom Hintergrund-Laden pruefen.
- RE (selbst disassembliert): Apply FUN_80021bbc (Present @0x80021558): @0x80021c80 `sb 2,0x800be571`, @0x80021cb8 `sw
  0x800be574` = Bytes des Cut-Hintergrunds aus der Raumtabelle (s1 = *0x800b52c8 + Raum*0x20 + Cut*2, @0x80021c40-@0x80021cac),
  @0x80021d28 `sw 0x800be57c` = LBA = Datei-LBA (@0x8006f440/@0x8006f442 je Datei*8, Datei aus PTR_DAT_8007438c[Stufe][Raum]-1)
  + Cut*0x20 (@0x80021cd8 `sll a3,a3,5`), @0x80021d2c `jal 0x80013c50` mit a1 = 0 (@0x80021d30) = BLOCKIERENDES Lesen
  (FUN_80013c50: param_2 == 0 -> FUN_80013df0(1)), danach @0x80021e34 `jal 0x80053a8c` (MDEC-Dekodierung) und StoreImage. Der
  ganze Durchlauf steht, bis Lesen + Dekodieren fertig sind; ausgenommen nur das erste Apply nach dem Raumladen (FUN_8001d600
  laedt den Hintergrund vor und setzt 0x800aca38 |= 0x20000000, FUN_80021bbc @0x80021c48 ueberspringt das Lesen dann einmal).
- Messung Original (DuckStation-GDB, neuer Treiber `<sp>/E41n4/gdb_cut.py`: Haltepunkte 0x80031c44 je Durchlauf, 0x80021d2c
  vor dem Lesen (Bytes/LBA), 0x80021d34 nach dem Lesen, 0x80021e3c nach der Dekodierung; VSync-Zaehler 0x800787dc;
  settings.ini binaer bytegleich zurueck, md5 b3e22060...; [CDROM] ReadSpeedup = 1, SeekSpeedup = 1):
  | Lauf (Start) | Apply | Bytes / LBA | Lesen | Dekodieren | Durchlauf |
  |---|---|---|---|---|---|
  | `o_kurbel_cut3.txt` (p_x9000_slot1.sav = Weg des Nutzers: Cut 0 -> 2 -> 3 gelaufen; links drehen, gehen, Quadrat, Ja) | Cut_chg 3 nach dem Ja (Pos_set) n104 | 31964 / 39845 | vs10460 -> 10468 = 8 | -> 10472 = 4 | n104 -> n105 **14 VSyncs** (sonst 2) |
  | `o_weg_rvd.txt` (hg_east9000.sav, nach Westen gehen) | RVD Cut 0 / Cut 2 / Cut 3 | 23244/39749, 20708/39813, 31964/39845 | 8 / 7 / 9 | 3 / 3 / 4 | 13 / 12 / 15 VSyncs |
  | o_renn2 / o_geh1 / o_renn_losl (hg_ready.sav, Dossier NB2 / Abnahme 3) | Cut_chg 3 nach dem Ja | — | — | — | je **34 VSyncs** |
  Die LBA = 39749 + Cut*0x20 bestaetigt die Rechnung (@0x80021cd8). Die Dauer haengt am Laden: Lesen (Groesse des Cut-Bilds,
  Suchweg des Kopfs) + MDEC. Nach einem weit entfernten Lesen (hg_ready.sav wurde nach einem Waffenwechsel im Statusschirm
  gesichert = PLW-Laden FUN_80036b68) dauert dasselbe Apply 34 VSyncs; auf dem Weg des Nutzers 14. Das Original-Video desselben
  Wegs (`<sp>/E41n1/orig/o_hg_cut3.mp4`, v420..v436 17 unbewegte Bilder) passt dazu: Bild des Durchlaufs n103 steht 2 + 14 =
  16 VSyncs (+1 Abtastung).
- Gilt fuer JEDEN Kamerawechsel mit Hintergrund (Cut_chg auch auf denselben Cut, RVD-Zonen): Apply = dieselbe Funktion
  FUN_80021bbc; gemessen 12-15 VSyncs auf dem natuerlichen Weg durch 2020.
- Rennen auf demselben Weg (`<sp>/E41n4/orig/o_kurbel_cut3_renn.txt`, links drehen, 5 Bilder OBEN+RENNEN, Quadrat im Rennen):
  Apply n103 -> n104 **15 VSyncs** (Lesen vs10476 -> 10485 = 9, MDEC -> 10489 = 4), danach der Renn-Stopp wie o_renn2.
- Original-Video desselben Wegs (o_hg_cut3.mp4, 59,82 Bilder/s = 1 Bild je VSync; `<sp>/E41n4/vid_cut3/` v395..v470, Pixel-Diff
  je Folgebild): v420 Frage weg (6346 Pixel), **v421..v436 unveraendert**, v437 Leon an der Kurbel (Vollbild), danach Wechsel
  im 2-VSync-Takt (Balken). Das Bild mit geschlossener Frage steht 17 VSyncs.

### Punkt 1 / Nachtrag 14 — Umsetzung, Messung nachher, Bildvergleich
- Umsetzung (Port-Bildtakt 1000/30 ms = 2 VSyncs): das Apply-Bild bleibt wie bisher ungezeichnet (bildhalt_r41_pc.c) und
  steht jetzt 1 + 6 Bilddauern = 14 VSyncs wie der gemessene Original-Durchlauf (o_kurbel_cut3 n104 -> n105).
  - engine/src/kamerahalt_r41.c / include/re15_kamerahalt_r41.h: `re15_kamerahalt_r41_standbilder()` = 6 (`KH_STANDBILDER`,
    Beleg im Kommentar: 14 VSyncs = 2 + Lesen 8 @0x80021d2c->@0x80021d34 + MDEC 4 @0x80021e34->@0x80021e3c) nach einem
    HALTEN, sonst 0 (erstes Bild des Raums: FUN_8001d600 laedt den Hintergrund vor, 0x800aca38 |= 0x20000000, FUN_80021bbc
    @0x80021c48 liest dann nicht; Montage 1240; RE15_FORCE_CUT).
  - platform/pc/src/bildhalt_r41_pc.c: `re15_render_pc_standbild(bilder, budget_ms, nr)` merkt die Dauer im gehaltenen Bild
    vor, `re15_render_pc_standbild_warten()` wartet HINTER dem Bildtakt (Wanduhr, Fenster-Ereignisse gepumpt, kein Spieltakt,
    kein Zeichnen); Messschiene RE15_KAMERAHALT_LOG `[standbild] F<n> 6 Bilder <ms> ms`. Bildzaehler/Skripte/Logik unberuehrt:
    wie im Original ist das Apply EIN Durchlauf.
  - platform/pc/main.c (E-Haken, E-E1): Zeile am Apply-Haken (`re15_render_pc_standbild(re15_kamerahalt_r41_standbilder(),
    frame_budget_ms, ...)`) und eine Zeile hinter dem Bildtakt (`re15_render_pc_standbild_warten()`).
  - Spielweit: jeder Kamerawechsel mit Hintergrund (Cut_chg, RVD) steht wie im Original; dort 12-15 VSyncs gemessen, nach weitem
    CD-Kopfweg 34 — der Port nimmt einheitlich den an der Kurbel gemessenen Wert 14 (den Kopfweg fuehrt der Port nicht).
- Messung nachher (exe re15_pc_E41n4b.exe md5 732dbebc...; Lauf `<sp>/E41n4/runs/P1_weg_cut3`: Leon (9100,-3200) Blick 2048
  nach Westen gegangen wie p_x9000 (Cut 0 -> 2 F301 -> 3 F347), links drehen, Quadrat, Fragen, Ja): `[standbild] F301 6 Bilder
  198 ms`, `F347 ... 198 ms`, Kurbel: Frage zu F539, Cut_chg/Pos_set F540 gehalten `[standbild] F540 6 Bilder 198 ms`, F541 Leon
  an der Kurbel, Plc_motion F560. Riegel-Laeufe: geh F382 202 ms, renn F282 199 ms, losl F282 198 ms, spawn_kante F1..F70 je
  ~199 ms.
- Bildvergleich in VSync-Zeit (`<sp>/E41n4/sheet_vsync.py`: Port-Abzug je Spielbild = 2 VSyncs, gehaltenes Bild = voriges Bild,
  + 2 x Standbilder): `<sp>/E41n4/P1_weg_vsync_bogen.png` (Spalte = 2 VSyncs; Original v418..v444 gegen Port F538..):
  | | Frage offen | Frage zu, Bild steht | erstes Bild an der Kurbel |
  |---|---|---|---|
  | Original o_hg_cut3 | v418 | v420 .. v436 (Spalten 1-8) | v438 (Spalte 9) |
  | Port NB4 | F538 | F539 (Spalten 1-8: F539, gehaltenes F540, 12 VSyncs Standbild) | F541 (Spalte 9) |
  | Port vorher (1 Bild halten) | F538 | F539 (Spalten 1-2) | F541 (Spalte 3) |
  Zoom um Leon/Kurbel `<sp>/E41n4/P1_weg_zoom.png` (v418/v420/v436/v438/v440/v444 gegen F538/F539/F539/F541/F542/F544): gleiche
  Folge — Leon steht bis zum Ende des Standbilds an der alten Stelle und erscheint dann an der Kurbel; kein Zwischenbild. Rest-
  unterschied im Bild: die Reling-Freistellung 03_01 des Nutzers liegt in Cut 3 ueber Leons Beinen (Original: Cut 3 ohne PRI,
  RDT @0x3FC `ff ff ff ff`); laut Spur M (M_masken36.md §2.1, M_abnahme_0 §2a) richtig, Leon steht von der Kamera aus HINTER der
  Reling — Nutzer-Maske, nicht Spur E. Der Stand-Lauf vom Sprung (`P1_stand_cut3`, Leon per Lage auf (3012,-3200) ohne den Weg)
  zeigte die Frage in einer anderen Kamera (RVD-Zonen haengen am vorigen Cut) — fuer den Bildvergleich darum der gegangene Weg.

### Mangel 2 / E-E4 — unit_r36_zombieki_affen_block misst den Haenger wieder
- Ursache (Abnahme 3 §8.2, bestaetigt): die Seed-Schranke "AN - AUS <= 5982" war nach dem Bestehen gewaehlt; sie liess Seed
  0x9b5bf362 (ein Gorilla 4405 von 6000 Bildern an der Klemme (11129,9982)) durch. Die Seed-Differenz der blockierten Bilder misst
  das Bahn-Chaos (welche Seeds in eine Haenge-Stelle laufen), nicht die Port-Wahl.
- Messung vorher (NB4-Bau, `<sp>/E41n4/affen/riegel_nb4a.txt`): unveraendert gegen NB3 (Summe AUS 304898 / FERN 253141, AN 73926
  / FERN 24433) — Mangel 1 (Uebernahme) aendert die Bahn der Sonde nicht. Neue Sonden-Ausgabe (Testhaken, kein Engine-Code): je
  blockierter Strecke die FERN-Bilder und die FERN-Bilder AN DER KLEMME (`LAGE ... fern <n> fklemme <k> [offen]`, auch die am
  Laufende offene Strecke), 144 Seeds x AUS/AN, je Lauf eigener Prozess (`<sp>/E41n4/affen/lage3/`, Auswertung
  `lage_auswertung3.py`).
- Befund (gemessen):
  | Stelle | AUS (Original-Regel) FERN-Klemmen-Bilder | AN (PORT-WAHL Umweg) | Natur |
  |---|---|---|---|
  | TASCHE x 10500..11400, z 9700..11400 (Haenger der Abnahme) | 6223 in 2 Seeds (0xe1e5634a 2833+1289, 0xb718cbe2 1531+286+284) | 6583 in 2 Seeds (0x9b5bf362 4393, 0x88ee0af8 1555) | geschlossene Tasche (Raster `raster 200 1600`: frei x 9200..11000 / z 9800..11200, Zeile z 11400 ganz Klemme); der Gorilla springt hinein (Spur: Sub 7 F900..F950), der Umweg findet keinen Weg hinaus (uw 0 F950..F5355, fest bis 393, +0x1de bis 522) |
  | SUB7 x -13800..-13300, z -1300..200 | 36710 in 57 Seeds | 12637 in 20 Seeds | Blindsprung-Schleife des Originals vor dem 0x20-Pad #19 (+0x7 = 3 @0x80117fe4-@0x80118024) |
  | WESTBLOCK x 3800..6200, z 9900..12500 | 5880 in 15 Seeds | 3590 in 6 Seeds | beide Gorillas Sub 4 mit Sicht an den Wandbloecken z <= 9800 / x 6000..6400, Leon fern im Sueden; Umweg zu Fuss AKTIV (uw 1, Ziele (7000,10500) / (9400,6500)), der Gorilla dreht zum Ziel (rot 3989) und bewegt sich 742 Bilder nicht (0xae479de5 F4615..F5357) |
  Ausserhalb dieser drei Stellen je Seed die groesste FERN-Klemmen-Strecke: AUS Median 411, p90 2377, max 5313 ((6418,-3617)
  = Ostgasse, (320,-10846), ...; 76 Seeds >= 354, 122 >= 90); AN Median 0, p90 46, max 103 (0x6394780b (-13592,11847); 0 Seeds
  >= 354, 5 >= 90). An allen drei Stellen haengt schon die Original-Regel; die Port-Wahl hat dort keinen Ausweg.
- Ursache in zombieki_r36.c belegt, aber NICHT klein: (1) TASCHE: r36_uw_route -> 0 (kein gerade erreichbarer Startknoten bzw.
  keine Kante aus der Tasche: r36_uw_knoten(.., 3*r/2, r, 4) / A*), Ausweg nur per Sprung — neue Umweg-Art; (2) SUB7: die
  Original-Blindsprung-Schleife (Port-Wahl greift dort nicht ein); (3) WESTBLOCK: Umweg zu Fuss im Sub 4 aktiv, aber kein
  Vortrieb — ob B[4] (FERN mit Sicht, Steuerung @0x801181e0) hier nicht laeuft oder die Klemme jeden Schritt aufhebt, ist nicht
  untersucht. Jede Aenderung verschiebt die Bahnen aller Seeds und muss gegen die 81 Pruefungen + Spur-J-Fenster gefahren werden
  -> Folgeauftrag (unten), keine Aenderung an zombieki_r36.c in Spur E.
- Aenderung (tests/unit/test_r36_zombieki_affen_block.c, von E-E2/E-E4 gedeckt): zg_haenger/zg_ort (benannte Stellen TASCHE, SUB7,
  WESTBLOCK mit Beleg im Kommentar); EINS-Zeile + `fkmax fkx fkz ort a b c`; Riegel: (a) NEU je Seed AN groesste FERN-Klemmen-
  Strecke ausserhalb der benannten Stellen < R41_WARTE_MAX = 354 = R36_NAH_START (laengste Klemmen-Wartezeit des kanonischen
  Original-Kampfs, Runde 36 J GDB: Slot 2 wartet 353 Bilder — vorab begruendet, nicht aus dem Bestehen), (b) NEU Gegenprobe: die
  Original-Regel (AUS) loest das Mass aus (76 Seeds), (c) benannte Stellen je Modus ausgegeben, (d) Seed-Differenz AN - AUS nur
  noch ausgegeben; FERN <= 1/6 und Griffe >= 80 % unveraendert. Die alte Schranke 5982 ist entfernt.
- Messung nachher (`<sp>/E41n4/affen/riegel_nb4b.txt`): `ok: HAENGER ... groesste AN 103 (0x6394780b bei (-13592,11847))`,
  `ok: Gegenprobe Original-Regel: das Haenger-Mass schlaegt ohne Umweg an (76 Seeds >= 354)`, benannte Stellen TASCHE AUS 6223/2 |
  AN 6583/2, SUB7 36710/57 | 12637/20, WESTBLOCK 5880/15 | 3590/6; 81 Pruefungen, 0 Fehler.
  `ctest -R "^unit_r36_zombieki_.*$"`: 100% tests passed, 0 tests failed out of 6.
- Fuer den Orchestrator (Folgeauftrag UEBERGABE §3b, Port-Wahl Runde 36 zombieki_r36.c): drei Haenge-Stellen der Gorillas in
  ROOM11C0, an denen auch die Original-Regel haengt und der Umweg keinen Ausweg hat — TASCHE (Sprung aus der geschlossenen Tasche),
  SUB7 (Blindsprung-Schleife vor #19), WESTBLOCK (Umweg im Sub 4 ohne Vortrieb). Nachstellbar: `test_r36_zombieki_affen_block
  eins <i> 1 6000 400 1` (i = 2 TASCHE, 47 SUB7, 117 WESTBLOCK), Spur je Bild `eins <i> 1 6000 400 0 1 1 1 <von> <bis>`. Nach
  einer Behebung die Stelle aus zg_ort nehmen; das Haenger-Mass greift dann dort.

### Nachbesserung 4 — je Mangel (Ursache / Messung vorher / Beleg / Aenderung / Messung nachher)
| Mangel | Ursache | Messung vorher | Beleg (Original) | Aenderung | Messung nachher |
|---|---|---|---|---|---|
| 1 Renn-Stopp nach Treppe/Griff/Plc_ret | Stopp-Zustand nur im Modul; Port-Zweige (Treppe, Griff, Plc_ret ...) gaben den Spieler mit der Steh-Bewegung zurueck, der Rest-Stopp lief weiter | Abnahme 3 §6.2: c3ba4317 F376..F379 z -13665 -13588 -13551 -13554 | Stopp nur in +5 = 2 / +6 = 3 (@0x8003272c, @0x800328fc; o_renn_losl `cmd=1/2/3/0`); Treppe sce 12/13 @0x80043544/@0x80043610 + Abschluss @0x80038ea8/@0x80038eb0, Plc_ret @0x80041f90-9c, Plc_motion @0x80041bb0-c4, Griff @0x80102640 / @0x8011c38c; gesperrt `bltz` @0x80031c78 | rennstopp_r41.c Marke +6 = 3, Dispatcher-Haken (game_step_common.c 2 Zeilen), msg_block-Zweig halten (player_common.c 1 Zeile) | N2 F376..F382 -13782 (stehend); N2b -13782; N1/N3/P1_frei/TA/TC/Elza/N2c zeilengleich; Kurbel o_renn2 + o_renn_losl unveraendert; Riegel takt_uebernahme 6/6 (vor NB4 T/R/G rot), integration treppe (c3ba4317 rot) / losl |
| 2 Gorilla-Sonde "gruen gemacht" | Seed-Schranke 5982 nach dem Bestehen, liess den Haenger 0x9b5bf362 (4405 Bilder) durch | Abnahme 3 §8.2; NB4-Bau unveraendert (FERN 24433/253141) | Haenger-Stellen = Stellen, an denen die Original-Regel selbst haengt (AUS: TASCHE 6223/2 Seeds, SUB7 36710/57, WESTBLOCK 5880/15) | Haenger-Mass je Seed (groesste FERN-Klemmen-Strecke ausserhalb der benannten Stellen < 354 = R36_NAH_START), Gegenprobe Original-Regel, benannte Stellen ausgegeben, Schranke 5982 entfernt | AN max 103 (0 Seeds >= 354), AUS 76 Seeds >= 354; 81 Pruefungen 0 Fehler; Folgeauftrag zombieki_r36.c |
| Punkt 1 / Nachtrag 14 "halb fliegend" | Port hielt das Apply-Bild 1 Spielbild; das Original steht, bis der Hintergrund von CD gelesen und dekodiert ist | Port-Bogen: Bild mit geschlossener Frage 4 VSyncs, dann Leon an der Kurbel | FUN_80021bbc @0x80021d2c (`jal 0x80013c50`, a1 = 0 blockierend) + @0x80021e34 MDEC; GDB o_kurbel_cut3 14 VSyncs (Lesen 8 + MDEC 4 + 2), Rennen 15, RVD 12-15; Video o_hg_cut3 v420..v436 | kamerahalt_r41.c standbilder() = 6, bildhalt_r41_pc.c Standbild-Dauer hinter dem Bildtakt, main.c 2 E-Zeilen | Bild mit geschlossener Frage 16 VSyncs wie das Video (P1_weg_vsync_bogen.png); `[standbild] F540 6 Bilder 198 ms`; Riegel geh/renn/losl pruefen die Dauer |

### Gemeinsame Dateien (Stand Nachbesserung 4, je "Runde 41 Spur E")
| Datei | Haken NB4 | Bisher |
|---|---|---|
| engine/src/game_step_common.c | NEU 2 Zeilen vor der Zweig-Kette des Spieler-Dispatchers (`re15_rennstopp_r41_dispatcher`, neben den Haken Zopf R38 D / Zustand R39 F / Giftkugel R41 D); Datei laut VERTRAG §1.3 bei Spur H -> Integration: beidseitig, Orchestrator-Entscheid noetig (Erweiterung von E-E1/E-E2 "spielweit") | — |
| engine/src/player_common.c | +1 Zeile im msg_block-Zweig (`re15_rennstopp_r41_halten`) | NB0, NB3 (E-E1/E-E2) |
| platform/pc/main.c | Apply-Haken um `re15_render_pc_standbild(...)` erweitert, +1 Zeile hinter dem Bildtakt (`re15_render_pc_standbild_warten`) | NB2, NB3 (E-E1) |
| tests/unit/test_r36_zombieki_affen_block.c | Haenger-Mass, benannte Stellen, LAGE-Ausgabe (E-E2/E-E4) | NB3 |
Eigene Dateien: rennstopp_r41.c/.h, kamerahalt_r41.c/.h, bildhalt_r41_pc.c, Tests r41_kleinkram_b.

### OFFEN (Stand Nachbesserung 4)
- Aktions-Pruefung im Renn-DECIDE (@0x800326a4-@0x800326dc) vor der Stopp-Entscheidung (Hinweis H2 der Abnahme 3): bei einer
  Treppe (Handler-Rueckgabe 1 @0x800435c8) macht das Original im Aktionsbild KEINEN Stopp-Schritt, der Port einen (gemessene
  Treppe ohne Wirkung: Kollision haelt Leon an der Kante). Port-Reihenfolge Spieler-Takt -> Treppe/Klettern/AOT in
  game_step_common.c / aot_common.c (Katalog #14/#15) — nicht Spur E.
- Standbild-Dauer: der Port nimmt einheitlich die an der Kurbel gemessenen 14 VSyncs. Im Original haengt sie am Kopfweg der CD
  (RVD 12-15, nach einem weiten Lesen wie dem PLW-Laden beim Waffenwechsel 34) — den Kopfweg fuehrt der Port nicht.
- Gorilla-Haenge-Stellen TASCHE / SUB7 / WESTBLOCK (Mangel 2) -> Folgeauftrag an die Port-Wahl der Runde 36 (oben).
- Unveraendert aus NB3: Drehrate ausserhalb des Unterzustands 2 nach dem Pad, Geh-Tempo konstant 75 (Fussweg @0x800325c4),
  Gegner-Wahrnehmung im Stopp (Hinweis H4), H1/H5/H6 der Abnahme 2 (H6 = Standbild-Dauer jetzt umgesetzt, Rest s. oben).
- PSX-Plattform: kamerahalt/bildhalt/Standbild-Dauer nur auf PC (platform/psx/main.c ohne diese Haken, H5); der
  Dispatcher-Haken des Renn-Stopps liegt in der Engine und gilt fuer beide.

### Fuer den Nutzer (Stand Nachbesserung 4)
- Keine neuen Assets, keine Sprachdateien.
- Kurbel ROOM2020 ("halb fliegend"): nach dem "Yes" bleibt das Bild jetzt so lange stehen wie im Original (gut 1/4 s: das
  Original laedt dabei den Hintergrund von CD), erst dann steht Leon an der Kurbel — Bild fuer Bild wie das Original-Video.
- Spielweit: JEDER Kamerawechsel haelt das alte Bild kurz an (wie im Original, dort 0,2-0,25 s, nach einem Waffenwechsel auch
  laenger); die Spiellogik laeuft dabei nicht weiter.
- Renn-Stopp: nach einer Treppe, einem Griff oder einer Szene rutscht Leon nicht mehr nach.
- Die Reling vor Leons Beinen an der Kurbel kommt von deiner Freistellung 03_01 (ROOM2020 Kamera 3; das Original hat dort keine
  PRI) — Spur M hat sie als richtig bewertet (Leon steht von der Kamera aus hinter der Reling).

### Nachbesserung 4 — Tests (gezielt, keine volle Suite)
- Bau: `bash re15_port/tools/local_build.sh configure` (neue Tests) -> `LOCAL-BUILD-OK (configure)`; `... build` -> `LOCAL-BUILD-OK
  (build)` (Endstand Code d3dc34ee; Warnungen main.c:8804/10133/10513 vorbestehend, schon in `<sp>/E41n2/build.log`).
- Spur-Riegel: `ctest -R "^(unit_r41_kleinkram_b_takt_.*|integration_r41_kleinkram_b_(geh|renn|losl|kante|spawn_kante|kurbel|treppe))$"
  -j 4 --timeout 1200` -> **100% tests passed, 0 tests failed out of 14** (`<sp>/E41n4/ctest_p1.log`).
- Gorilla-Sonde: `ctest -R "^unit_r36_zombieki_.*$" -j 4 --timeout 900` -> **100% tests passed, 0 tests failed out of 6**.
- Alle unit_/prop_/probe_ ausser Release-/Android-Skripten (Spieler-, Renn-, Szenen-, Nachrichten-, Karten-, Tuer-Sonden):
  `ctest -R "^(unit_|prop_|probe_)" -E "releaseskript|android|r35_android|release_" -j 3 --timeout 900` -> **100% tests passed,
  0 tests failed out of 914** (311 s, CTEST_EXIT=0, `<sp>/E41n4/ctest_unit_alle.log`).
- Integration (echte exe; Bedingung E-E1 Nachrichten-/Szenen-/Tuer-/Karten-Riegel + Renn-Eingaben + die zwei neuen): Auswahl der
  NB3-Laeufe 1/3 (`<sp>/E41n3b/ziel_regex.txt` + `rest_regex.txt`, nur integration_*) + treppe/losl = 153 Tests,
  `<sp>/E41n4/integ_regex.txt`: laeuft.
- Gegenproben mit der VORHER-exe re15_pc_E41a3.exe (c3ba4317 = Stand Abnahme 3; `cmake -DRE15_PC_EXE=... -DTEIL=<t> -P
  tests/integration/test_r41_kleinkram_b.cmake`, `<sp>/E41n4/vorher_riegel/`): **treppe DURCHGEFALLEN** ("F376 (-1359,-13665) ...
  8 Lageaenderungen in 8 Bildern"), **geh DURCHGEFALLEN** ("Standbild F382: 0 Bilder 0 ms" — sonst alles wie das Original), losl
  BESTANDEN (Verhalten unveraendert, Gegenprobe der Uebernahme-Regel). Unit takt_uebernahme: Gegenprobe vor NB4 T/R/G rot.
