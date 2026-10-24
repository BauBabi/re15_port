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

#### DuckStation (Nachbesserung 2)
Messungen nur per GDB-Server (Port 2345, `<sp>/E41n2/gdb_cam*.py`, `gdb_geh*.py`; EnableGDBServer nur fuer den Lauf true).
Sicherung vor dem ersten Lauf: `<sp>/E41n2/ds_backup/` (settings.ini md5 b3e22060..., resume.sav 54717c88..., Slot 1
6f59edd4...). FALLE: die GDB-Treiber nach Muster F_nb4/gdb_affen.py schreiben settings.ini im TEXTMODUS zurueck -> unter
Windows CRLF statt LF (11702 statt 11177 Bytes); ihre Pruefung "settings.ini zurueck: True" vergleicht nur den Text. Nach
den Laeufen beide Dateien per Byte-Kopie aus der Sicherung zurueckgesetzt (md5 wieder b3e22060... / 54717c88...; Slot 1
unveraendert). ConfirmPowerOff = false, ToggleMediaCapture = Keyboard/F12, SaveSelectedSaveState = SDL-0/LeftShoulder,
EnableGDBServer = false wie vorher.
