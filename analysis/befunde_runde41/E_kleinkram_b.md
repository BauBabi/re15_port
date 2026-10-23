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
- [x] Punkt 1 Kurbel 2020 — plcloop_r41.c (Schleife statt Standbild), unit_r41_kleinkram_b_kurbel 2/2; "Fliegen" nicht reproduziert (OFFEN)
- [x] Punkt 2 Karte B1 / 1090-Vordertuer — karte_r41.c, unit_r41_kleinkram_b_karte_pin 17/17
- [x] Punkt 3 Werkstatt-Tuer 11B1 (Elza) — Ereignis 24, unit_r41_kleinkram_b_11b1 3/3

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
- engine/src/scd_vm.c: include + Ereignis-Kette `re15_werkstatt11b1_r41_ereignis` hinter nahaufnahmen_r40.
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
