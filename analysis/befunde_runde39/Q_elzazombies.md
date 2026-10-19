# Runde 39 Spur Q "elzazombies" — Dossier (laufend)

Baum `.claude/worktrees/r39_elzazombies`, Zweig `r39/elzazombies`, Basis 02dc7917 (master).
Wortlaut (AUFTRAG.md, Nachtrag 10, 2026-10-19 zwischen 23:46 und 00:06):

> "Und auch hier sollen jetzt Zombies durchbrechen - einmal in den Flur hinein von 10C1 aus - so viele wie im medical
> room - also 10E1 am Leben gelassen wurden., die sollen dann, wenn man in ROOM 1061 die Etage von unten über die Treppe
> erreicht aber möglichst weit weg an der Seite stehen. Dann das gleiche nochmal wie bei Leon in den ROOM 1071, falls die
> Zombies nicht besiegt wurden und im ROOM 1031 über die Absperrung/Das Rollo, so wie bei der cutscene."

Lesart des Orchestrators (VERBINDLICH): Elza-Raeume = ROOMxxx1; Ausloeser = steigende Flanke von Bank-9-Bit 97 (setzt NUR
Spur P); Montage-Form wie Leon nach Irons' Tod; Gegner-Verhalten RE2-KI wie bisher; Bit 95 nur, wenn kein Original-/Port-
Flag reicht.

## 0a. NUTZER-NACHTRAG 11 (AUFTRAG.md, vom Orchestrator um 00:24 in diesen Zweig committet: cceb31ef/50e00d74)
> "Ich korrigiere, die Zombies sollen nicht in ROM 1071 noch durchbrechen, sondern dann noch wie bei leon im ROOM 1041."

Orchestrator (verbindlich): KEIN Durchbruch aus/in 1071; statt dessen wie Leons Durchbruch in ROOM1040 (Rolltor-Schnitt
Cut 1, include/re15_montage_r38.h) in ROOM1041. "falls die Zombies nicht besiegt wurden" = die aus 10C1/1061. Zeitpunkt
belegen und begruenden (Elza steht beim Start der Panikphase selbst in 1041), spielbare Form (z.B. Durchbruch durchs
Rolltor beim naechsten Betreten von 1041, solange diese Zombies leben), Leons Form 1040 als Vorbild.
ERST UM 02:00 BEMERKT (beim master-Merge): bis dahin war der 1071-Schritt nach Nachtrag 10 gebaut (Abschnitte 2.3, 3,
4.1). Umbau 0b unten; die Messungen 4.1 (Cut-7-Teil) gelten nur noch fuer den Stand vor dem Umbau.

### 0b. Umbau nach Nachtrag 11 (Plan, Begruendung)
* Montage beim Start der Panikphase: Auftakt 1041 -> [1061 Cut 2, wenn 10E1-Ueberlebende] -> 1031 Rollo (Cut 12) ->
  Rueckkehr. Der 1031-Cut-7-Teil (1071-Kopien) und der Umzug der 1071-Records entfallen; ROOM1071 bleibt unberuehrt.
* DURCHBRUCH 1041 = Leons 1040-Schritt (irons_tod_1150.c k_p_1040_*; ROOM1041 = ROOM1040 in RID/SCA/BLK/RVD, sub08 Byte fuer
  Byte, Versatz +0x2A: Rolltor-Fahrt @0x019DC..@0x01A50; Rolltor = Objekt 0 main00 @0x01190 (-25200,0,-360); Schalter-
  Rechteck Slot 4 @0x01118 = Leons Parkplatz (-24950,-14900); Gleichzeitig-Limit Save(0x12,5) @0x01218).
  ZEITPUNKT: Elza steht beim Start der Panikphase selbst in 1041 (Spur P), und die Bedingung "falls die Zombies nicht
  besiegt wurden" betrifft die Zombies aus 10C1/1061 — die hatte sie dann noch gar nicht vor sich. Deshalb beim NAECHSTEN
  Betreten von 1041 nach der Montage (Tuer oder Laden, nicht die Rueckkehr der Montage selbst), solange mindestens eine der
  1061-Kopien lebt; nur beim Eintritt im Suedteil (vor dem Rolltor, z < -360: Tueren 1061/1031/Aufzug), damit die Zombies
  durchs Rolltor auf sie zukommen. Die ueberlebenden 1061-Kopien ziehen um (1061-Bit 1, 1041-Bit 0), erscheinen hinter
  dem Rolltor (Leons Aufstell-Gassen), Elza wartet unsichtbar auf Leons Parkplatz hinter der Kamera Cut 1, Knall wie 1030,
  Rolltor hoch (falls zu), die Zombies kommen durch, danach Elza zurueck an ihren Eintrittspunkt mit Steuerung.
  Einmalig von selbst: danach lebt keine 1061-Kopie mehr (umgezogen); spaeteres Betreten von 1041 = Nachspawn der
  1041-Kopien (wie Leons k_p_1040_nach).
* Bits: 1041-Kopien = 128..131 (die bisherigen 1071-Kopie-Bits, Zensus 2.4 frei), 132 wieder frei.

## 0. Arbeitsplan
1. Leons Form messen/belegen (Runde 35 L irons_tod_1150.c, Runde 38 O montage_r38.c, Runde 37 H zombieaktiv_r37.c).
2. Elza: Durchbruch 10C1 -> Flur, Zahl = Zombies, die in 10E1 leben; 1061 Aufstellung; 1071 wie Leon; 1031 ueber das Rollo.
3. Nebenwirkungen: Leon unveraendert; ohne Bit 97 nichts; Speichern/Laden.
4. Messung am echten Weg mit Elza, Riegel unit + integration.

## 1. Messung vorher

### 1.1 Stand master 02dc7917 (Baum gebaut 00:35): (9,97) loest nichts aus
Lauf m_vorher (scratchpad/q/m_vorher): Elza-Spielstand ROOM1041 (-21008,-13134), CONTINUE, `RE15_SET_FLAG_AT=9:97@200`,
Ende Bild 700. debug.log: `[setflag-at] Frame 200: flag(9,97/0x61) = 1`, danach bis Bild 700 nur `[walk]`-Zeilen im
selben Raum, kein Raumwechsel, kein Spawn. `grep` im Code: kein Leser von (9,97) in master.

### 1.2 ROOM1061 gemessen (Grundlage der Lagen)
* Cut 2 (Tuer-Schnitt aus 10C1) = der 2F-Treppenabsatz: Fenster links, "F2", Tuer zu 10C1 rechts, Treppe abwaerts
  hinter dem Gelaender (Lauf B), Treppe aufwaerts unten links (Lauf A). Bilder: Q_belege/q_1061_cut2_tuer.png (Elza an der
  Tuer-Ankunft (26000,-7200,25300)), Q_belege/q_1061_westseite_treppenkopf.png (links (20600,26200) = Westseite unter dem
  Fenster, rechts (25000,24600) = Treppenkopf von unten).
* Aufstieg von unten (Lauf e1061e): Elza Etage 2 (25000,-3600,19000) Blick +z, Vorwaerts + Aktion -> Treppe (mo 221) von
  z 19532 nach z 24568, Ende Bild 165 bei (25023,24568), Kamera 4 -> 3 -> 2. = ANKUNFTSPUNKT von unten (Etage 4).
* Treppen brauchen die Aktionstaste (Sonde 620 vor der Blickrichtung, stair_common.c `@0x80042bd0`; Band-Gate).


## 2. RE-Belege

### 2.1 Leons Form (Vorbild, Port-Code gelesen; Original-Zeilen dort zitiert)
Quelle: engine/src/irons_tod_1150.c + include/re15_irons_tod.h (Runde 35 L, Kette), engine/src/montage_r38.c +
include/re15_montage_r38.h (Runde 38 O, Verhalten in den Schnitten), engine/src/zombieaktiv_r37.c (Runde 37 H).
* KETTE (Leon): 1150-Szene -> Knall -> [1130 Cut 0, nur wenn in 1140 noch Zombies leben] -> Knall -> 1040 Cut 1 (Rolltor,
  5 Zombies) -> [1030 Cut 7, nur wenn in 1070 Zombies leben: Tuerknall + Kopien vor der 1070-Tuer] -> Original-Knall
  ROOM1030 sub08 @0x02776 + Tor bricht + Kriecher -> Cut 12 -> 11C0 Cut 13 -> Rueckkehr 1150.
* RAUMWECHSEL in der Szene = Door_aot_set (0x3B) + Aot_on (0x47) "fire now" (LAB_800407bc, Handler @0x8004082c ->
  sce-2 @0x800430bc -> Warp FUN_8001d600; Vorbilder ROOM1240 sub02/03, ROOM1080 sub07..10). Spieler wird auf einen
  Parkplatz HINTER der Kamera des Ziel-Schnitts gesetzt (Spawn des Tuersatzes). Zielraum: der Installer startet das
  naechste Programm (Ereignis 21) nach dem Init-Lauf; erster Abschnitt (Spawns) im Raumaufbau vor dem ersten Bild
  (erster_abschnitt_sofort, Form FUN_8003ef6c -> FUN_8003f0a0 @0x8003f018). sub01-Reseed waehrend eines Schritts aus
  (FUN_8003f038 @0x8003f064-84).
* KNALL = Port-Signal Set(5,28)/(5,29) -> Knall-Bank (gen/knall_bank.inc Satz 0 = RE2 ROOM1060 Se_on 0x0a, Satz 1 =
  ROOM1030 Se_on 0x0c Tone 7); im 1030-Schritt der Original-Se_on `36 02 0c 00 00 00 cc dd f8 f8 f0 a7` @0x02776.
* KOPIEN: Zahl = lebende Records des Quellraums (Tot-Bit = Sce_em_set pc[7] in Zone 7, re15_em_status_zone Stage 1-3,
  Gate @0x80042128-38). Umzug beim Kettenstart (umzug_vorbereiten): Quell-Record stillgelegt (Tot-Bit 1), Kopie mit
  NEUEM Tot-Bit im Zielraum scharf; schon tote bekommen eine stille Kopie (Kopie-Bit 1).
  1140 -> 1130: Bits 0xd3..0xd7 -> 40,41,42,43,46; 1070 -> 1030: 0xc6..0xca -> 47,48,49,75,76; Kriecher 77,78,79.
* FORM DER KOPIEN im Schritt: Durchbruch-Form grid 0x0d (STAGE1.BIN `ori v0,zero,0x201`/`sw v0,4(v1)` @0x80100f40-44;
  RE2-KI Deskriptor 6 = Gang `addiu v0,zero,257`/`sw v0,4(s2)` @0x80100ab8-abc), Blick zur Kamera; Ziel = der geparkte
  Spieler (FUN_80039e7c gleiche Zone `bne a0,a1` @0x80039fbc -> Spielerlage); Einzelstopp hinter der Kamera
  (entity+0x9 |= 0x20, `lbu v0,9(a0)` @0x80100450 / `andi v0,v0,0x20` @0x80100458 / `bne` @0x8010045c).
* SPAETERES BETRETEN (Nachspawn): dieselben Kopien (dieselben Tot-Bits) erscheinen, solange sie leben; Freigabe ueber
  Bank-9-Bits (9,74)/(9,76) (Spawn verbraucht) bzw. (9,73) (Szene gesehen).
* "NICHT BESIEGT" = Tot-Bit des Quell-Records 0 (re15_irons_tod_lebend: `!re15_game_flag_get(7, basis+i)`).

### 2.2 Elza-Raeume (ausgelieferte RDTs, re15_port/tools/scd_dump_room.py, eigener Leser scratchpad/tools/raum.py)
Raumnamen DEBUG.BIN (include/debug_jump_table.h, Stage 0): 0x03 LOBBY, 0x04 WEST CORRIDOR, 0x06 UPPER STAIRS,
0x07 LOBBY OFFICE, 0x0C 2F WEST SIDE, 0x0D 2F CORRIDOR, 0x0E MED. EXAM ROOM (= "medical room").
* ROOM10E1 (MED. EXAM ROOM; SCD = ROOM10E0 Byte fuer Byte): sub00 @0x00D26 `Ifel_ck`/`Ck(4,246)=0` -> 4 FRESSER
  Typ 0x10 grid 0x06 Tot-Bits 0xe0 @0x00D56, 0xe1 @0x00D6A, 0xe2 @0x00D7E, 0xe3 @0x00D92 (+ Leiche Typ 0x10 grid 0x88
  Tot-Bit 0xff = ohne Gate @0x00D42); Else @0x00DA6 (nach dem Ereignis sub02 @0x00E14 `22 04 f6 01`): dieselben vier
  Bits als Steher grid 0x02 @0x00DBE/@0x00DD2/@0x00DE6/@0x00DFA. => "am Leben gelassen" = Tot-Bits (7,0xe0..0xe3) = 0.
* ROOM10C1 (2F WEST SIDE): Tuer Slot 0 @0x00E42 -> Raum 0x06 (1061), Spawn (26000,-7200,25300) Gier 2048 Cut 2;
  Slot 2 @0x00EA4 -> 0x0d (10D1); Gegner nur drei Kraehen Typ 0x21 (Bits 0x6a..0x6c @0x00F8C..). Kein Zombie in 10C1.
* ROOM1061 (UPPER STAIRS, Treppenhaus): drei Tueren AUF DERSELBEN Flaeche (26600,24300,1000,2200) @0x00D12/@0x00D32/
  @0x00D52, Etage 8 -> 0x12 (1121), Etage 4 -> 0x0c (10C1, Spawn (300,9550) Cut 0), Etage 0 -> 0x04 (1041). Treppen-AOTs
  sce 13 @0x00D72..@0x00DFE: Lauf A (20200,20700,2400,4300) Etagen 0/2/4/6, Lauf B (23800,19800,2700,4400) Etagen 2/4/6/8.
  Kameras (RID @0x60): Cut 2 @0x000A0 pos (20199,-12628,21273) -> (24670,-7088,27767) = 2F, Tuer-Cut aus 10C1;
  Cut 3 @0x000C0 (26262,-11447,25466) -> (24812,-7088,21359). Eine BLK-Zone @0x00CF4 (x 19500..27200, z 17900..27300).
* ROOM1071 (LOBBY OFFICE; SCD = ROOM1070): sub00 @0x015AE `Ck(4,197)=0` -> 5 Liegende Typen 0x10/0x10/0x10/0x11/0x11
  Tot-Bits 0xc6..0xca @0x015CA..@0x0161A; Else @0x0162E Steher grid 0x00 @0x01632..@0x01682 = Leons 1070 Byte fuer Byte.
* ROOM1031 (LOBBY): RID Cut 0..12 = ROOM1030 (Cut 7 @0x00140 (-26820,-3114,-2574) -> (-15858,-936,-5040), Cut 12
  @0x001E0); SCA und BLK inhaltsgleich mit 1030 (Diff 0 Zeilen; BLK Zone 7 @0x01C74, Zone 0 @0x01C20, Zone 10
  @0x01C98); sub04..sub10 Byte fuer Byte = 1030 (nur Dateiversatz +0xEC). DIE CUTSCENE "ueber die Absperrung/das
  Rollo" = sub08 @0x02850: Riegel @0x02850/@0x02854, Aot_reset 17 @0x02858, Se_on `36 02 0c 00 00 00 cc dd f8 f8 f0 a7`
  @0x02862 (der Knall), Cut_replace 0<->9 3<->10 4<->11 6<->12 @0x0286E..@0x02877 (Tor/Rollo aufgebrochen),
  Sca_id_set 2/3 -> 0xf7 @0x0287A/@0x0287E, Save var5/var7 @0x02882/@0x02886, Gosub sub09 (Kriech-Regel) @0x0288A..,
  Cut_chg 12 @0x02896, Set(5,20) @0x028C6; ausgeloest von sub01 @0x0226C (Ck(3,116)=1, Ck(4,15)=0 -> Set(4,15)=1 +
  Evt_exec sub08 @0x02288). Meldungen 1031: msg 0 "This should keep those things out...For now...", msg 2 "The
  emergency shutters have already been activated." = das Rollo; Elzas eigene Szene sub13 @0x02982 hat KEINE Zombies.
  20 Raum-Zombies main00 @0x01E5C..@0x01FD8 Typ 0x16 grid 0x0d Tot-Bits 0x00..0x13, Limit Save(0x12,6) @0x01E58.
  Tueren: Slot 1 @0x01CF8 -> 0x07 (1071), Slot 2 @0x01D18 -> 0x04 (1041).

### 2.3 Lesart (dokumentiert, Form aus 2.1/2.2)
* "einmal in den Flur hinein von 10C1 aus ... wenn man in ROOM 1061 die Etage von unten ueber die Treppe erreicht":
  der Flur ist das Treppenhaus ROOM1061 auf Etage 4 (2F); die Zombies kommen durch die 10C1-Tuer (Etage 4) herein.
  10C1 selbst hat keine Zombies und liegt nicht auf Elzas Weg 1041 -> 1061 -> 1121.
* ERSETZT durch Nachtrag 11 (0a/0b): "das gleiche nochmal wie bei Leon im ROOM 1041, falls die Zombies nicht besiegt
  wurden" = Leons 1040-Rolltor-Schritt in ROOM1041, Bedingung = die Zombies aus 10C1/1061 leben (Orchestrator), beim
  naechsten Betreten von 1041. (Bis 02:00 gebaut: Leons 1070-Schritt fuer 1071 -> 1031 Cut 7; zurueckgebaut.)
* "im ROOM 1031 ueber die Absperrung/Das Rollo, so wie bei der cutscene" = Leons Tor-Schritt: die Original-Szene
  1031 sub08 (s. 2.2), Kriecher durch das aufgebrochene Rollo, Cut 12.

### 2.4 Bits, Plaetze, Ereignis (Zensus)
* Zone-7-Tot-Bits: Zensus aller Sce_em_set in STAGE1-3 (132 RDTs mit SCD, scd_walk_lib): 176 belegt; frei u.a. 100..105,
  109..112, 117..119, 124..133, 144, 145. Leons Port-Kopien: 40..43, 46..49, 75..79, 83, 84, 90, 91, 95. Port-Programme mit
  Sce_em_set sonst nur 0xff (NPCs) und 0xcb/0xcc (1141-Gorillas, Original-Bits). GEWAEHLT: 124..127 (1061), 128..132 (1031
  Cut 7), 133/144/145 (Kriecher).
* Bank 9 Bit 95 (VERTRAG 1.1 an Q): kein Ck/Set (9,95) in STAGE1; Riegel unit_r36_kartenbits prueft gegen den Original-Satz.
* Tuer-Slot 20 frei in 1041 (0..8), 1061 (0..10, 16), 1031 (0..19). Ereignis 30 frei (Port: 2..6, 13, 20..29).
  Bank 5 Bits 28/29 frei in 1041/1031 (0..20, 32..35) und 1061 (keins).
* ROOM1031 Tonbank = ROOM1030 (alle 6 Bank-Abschnitte RDT+0x08..0x1C byte-gleich, 0x200 + 0x4000 B verglichen) -> der
  Original-Se_on `36 02 0c ..` @0x02862 klingt wie Leons @0x02776.
* ROOM1041 Tueren: Slot 1 @0x010B8 -> 0x03 (1031), Slot 2 @0x010D8 -> 0x06 (1061, Spawn (26000,0,25300) Cut 7).
* Elza-Intro in 1031: main00 @0x0204A `Ck(3,193)=0` -> Tuer Slot 18 nach 0x24 + sub12 (Set(3,193)) = Intro-Weiterleitung;
  die Rollo-Zombies (main00 @0x01E58..) nur bei (3,125)=1 (@0x01E54). Im Spiel nach dem Intro gilt (3,125)=(3,193)=1 —
  Messlaeufe muessen das im Spielstand tragen (Lauf k1 ohne: Sprung nach ROOM1241 = Intro, Kette unterbrochen).

### 2.5 Lagen ROOM1061 (aus 1.2 und der SCA, Regel statt Zahl)
* Freie Flaeche Etage 4 = SCA-Zellen floor-Byte 0x43 (Waende @0x00584/@0x00590/@0x0059C/@0x005B4, Kern @0x005A8, Lauf-B-
  Block @0x005C0), Gegner-Radius 400, Abstand >= 800.
* Montage-Spawn: dichteste Packung um die Tuer-Ankunft (26000,25300) (ROOM10C1 @0x00E42): (26000,25300), (25200,25300),
  (26000,24500), (26000,26100); Gier 2048 (Eintrittsrichtung der Tuer, @0x00E42 Nutzlast `00 08`).
* Nachspawn "moeglichst weit weg an der Seite": weiteste Packung vom gemessenen Ankunftspunkt (25023,24568):
  (20200,26550) 5214, (20200,25750) 4966, (21000,26550) 4485, (20900,25350) 4197 = Westseite unter dem Fenster.
* Parkplatz Cut 2: hinter der Bildebene, Etage 4, kleinster Winkel zur Linie Tuer -> Kamera: (20200,20400), Tiefe -718,
  5,4 Grad; ausserhalb der RVD-Zonen von Cut 2; eine BLK-Zone -> Navigator-Ziel = Spielerlage.

## 3. Umsetzung (Stand nach Nachtrag 11)
* NEU engine/src/elzazombies_r39.c + include/re15_elzazombies_r39.h (alle Konstanten mit Beleg im Kopf).
* Haken (je 1-2 Zeilen, "Runde 39 Spur Q"): scd_vm.c (Ereignis 30 in der Ereigniskette, sub01-Sperre im Montage-Schritt
  und im Durchbruch, Takt am Kopf des VM-Laufs), scd_room_setup.c (Installer am Ende des Blocks, nach Spur K's Zeile),
  platform/pc/main.c (CONTINUE-Weg: re15_elzazombies_r39_geladen = Laden ist keine Flanke, kein Durchbruch beim Laden),
  tests/unit/test_r36_kartenbits.c (k_port: (9,95)). master 2963579d (Spur K) in den Zweig gemergt (1a4e20b1; Konflikt
  scd_room_setup.c: beide Installer-Zeilen behalten, Q am Ende).
* Kette: Flanke (9,97) 0->1 im Spiel, Elza, (9,95)=0 -> ausstehend; Start sobald frei (re15_cine_active = 0, player_mode
  != 2, kein Raumwechsel, kein Kartenhinweis angefordert), nicht in 1061/1031 selbst. Start: Zahl der 10E1-Ueberlebenden,
  Auftakt-Faden, DANN Umzug (10E1-Quellen 1, 1061-Kopien 0 bzw. 1 fuer Tote, 1041-Kopien 1, Kriecher 0, (9,95)=1),
  Rueckkehr-Stelle merken. Programme: A Auftakt (Riegel, Sleep 60, Knall, Sleep 45, Tuer); B 1061 Cut 2 (4 Saetze, Gate
  entscheidet, am Ende Knall Satz 1); C 1031 = Leons Tor-Teil ohne Cut-7-Teil (Bruch bei (4,15)=0, Kriecher, Cut 12,
  Umparken, Tuer zurueck); D Rueckkehr.
* DURCHBRUCH 1041 (Programm G, Nachtrag 11): Installer 1041 im Tuer-Weg, Kette AUS, (9,95)=1, mindestens eine 1061-Kopie
  lebt, Eintritt im Suedteil (z < -360) -> Umzug 1061 -> 1041 (Bits 124..127 -> 1, 128..131 -> 0 fuer die Lebenden),
  Eintrittspunkt/-schnitt merken, Programm G: Limit 20, Kopien in Leons Aufstell-Gassen, Elza auf Leons Parkplatz, Cut 1,
  Riegel, Knall Satz 1, Vorlauf 34, Rolltor-Fahrt (ROOM1041 sub08 @0x019DC..@0x01A50) bei (4,5)=0, (4,5)/(4,4)=1, Warten
  auf (5,30), Sleep 90, Elza zurueck + Schnitt zurueck, Plc_ret, Cut_auto 1, Riegel los. Takt: Einzelstopp hinter der
  Kamera Cut 1 (Baustein 3), "alle durch" = alle Kopien z < -360 (oder Kappe 1532), am Ende Haltebits los.
* Verhalten in den Schnitten (Leons Bausteine 3/4) im eigenen Takt (Leons montage_r38.c bleibt unberuehrt).
* Nachspawns: 1061 (E) die vier Saetze an der Westseite, gehalten (entity+0x9 |= 0x20) solange die Spielerin nicht auf
  Etage 4 steht und die Treppe nicht zu Ende ist; 1031 (F) die Kriecher; 1041 (H) die Durchgebrochenen in den Gassen.
* Nachbesserungen aus den Messungen: (1) Kartenhinweis-Bedingung `re15_map_hint_pending() < 0` (-1 = keiner; Lauf k1 blieb
  sonst ausstehend); (2) 1061-Kopien erst frei, wenn die Treppe zu Ende ist (re15_stair_active() = 0, Laeufe t6 -> t7);
  (3) Umzug und (9,95) erst NACH erfolgreichem Start des Auftakt-Fadens; (4) "alle durch" im Durchbruch nur ueber die
  Kopien und ohne HP-Pruefung (unit durch1041, 4.6).
* Mess-Haken (env, kein Spielverhalten): RE15_R39Q_KNALL_STUMM (Form Leons RE15_IT_KNALL_STUMM); Pruefhaken im Header
  (Spuren, Freigabe, HP, Durchbruch-Zahl, Gegenprobe-Schalter re15_elzazombies_r39_enable).

## 4. Messung nachher

### 4.1 Erstlauf der Kette (Lauf k2, exe aus cc66567d + Kartenhinweis-Fix; STAND VOR NACHTRAG 11 - mit Cut-7-Teil)
Elza-Spielstand ROOM1041 (-21008,-13134) mit (3,125),(3,193),(3,116) (Intro durch, 1011 besucht), 10E1/1071 nicht
betreten (alle leben), `RE15_SET_FLAG_AT=9:97@200`. debug.log (gekuerzt):
```
[elzazombies-r39] Flanke (9,97) 0->1 in ROOM1041 Takt 203 -> Montage ausstehend
[elzazombies-r39] Umzug: 10E1 lebend 4 -> 1061, 1071 lebend 5 -> 1031 Cut 7; (9,95)=1
[elzazombies-r39] Folge #1 K ROOM1041 T61 0          (Tuerknall)
[room] PC loaded room1061.rdt                        Folge S +4, C 2
ROOM1061 Cut 2 Bild 176: Bit 124 Abstand zur Kamera 7062 -> 6245, Bit 125 6421 -> 4849, Bit 126 6638 -> 5476,
                         Bit 127 7547 -> 5009; Spieler-HP min 100
[room] PC loaded room1031.rdt                        Folge S +5, C 7
ROOM1031 Cut 7 Bild 207: Bits 128..132 8272 -> 4671, 6235 -> 1586, 4644 -> 1401, 7222 -> 2406, 5599 -> 729
Folge K 2 (Original-Se_on @0x02862) T457, S +3 (Kriecher) T467, C 12 T489
ROOM1031 Cut 12 Bild 335: Raum-Zombies Bits 0..5 12906 -> 8254 .. 14332 -> 9008; Kriecher 9827 -> 5634, 10558 -> 6319,
                          9560 -> 5262; 1071-Kopien hinter der Kamera angehalten (Baustein 3); Spieler-HP min 100
[room] PC loaded room1041.rdt -> Rueckkehr (Programm D) -> "Rueckkehr beendet, Steuerung frei" bei (-21008,0,-13134) Cut 0
```
Bilder Q_belege/q_kette_erstlauf.png: Reihe 1 = 1061 Cut 2 Bilder 10..170 (vier Zombies an der 10C1-Tuer, gehen nach links
zur Kamera), Reihe 2 links = 1031 Cut 7 Bilder 40..160 (Kopien aus dem 1071-Gang auf die Kamera zu), danach Cut 12 Bilder
220..520 (aufgebrochenes Rollo, Kriecher unter dem Rollo durch auf die Kamera zu).

### 4.2 Nachspawn ROOM1061, echter Weg (Tuer 1041 -> 1061, Treppe A und B)
* Lauf t6 (Freigabe noch bei Band 4 allein): Band 4 springt mitten im Treppen-Clip (Elza bei z 23503, Ankunft z 24568);
  frei ab dort liefen die Kopien ~1 s, bevor Elza die Steuerung hatte, und standen bei der Ankunft schon halb heran (Bild
  540 in Q_belege/q_1061_aufstieg_vorher_freigabe_im_clip.png). => Freigabe erst, wenn die Treppe zu Ende ist
  (re15_stair_active() = 0; "Etage erreicht").
* Lauf t7 / t8 (t8 = echter Weg: Spielstand 1041 an der Tuer Slot 2, Aktion -> DOOR FIRE slot=2 -> ROOM1061 -> Nachspawn;
  dasselbe Eingabeskript laeuft je Raum von vorn): Kopien bei der Freigabe (20218,26533), (20218,25732), (21019,26532),
  (20916,25341) = Westseite unter dem Fenster; Elza bei (25424,24568) = Treppenende; Abstaende 5565/5335/4823/4574.
  Bild 500 (Q_belege/q_1061_ankunft_von_unten.png, q_integration_treppe_ankunft.png): Elza rechts an der Tuer, die vier
  links unter dem Fenster; Bilder 540..620: danach gehen sie auf sie zu (RE2-KI Gang). Waehrend Etage 0 und 2 standen sie
  (gehalten, Lage bei der Freigabe = Spawn +-18).

### 4.3 Nachspawn ROOM1031 (Lauf n1031, CONTINUE mit (9,95), (4,15)=1)
Aktoren 7..11 = Typen 0x10/0x10/0x10/0x11/0x11 (1071-Kopien, Bits 128..132), 12..14 Typ 0x16 (Kriecher 133/144/145), 1..6
die Rollo-Zombies des Raums. Wie Leons k_p_1030_nach: die Kopien stehen im Gang zwischen 1041- und 1071-Tuer.

### 4.4 Nebenwirkungen
* Leon (Lauf leon): Spielstand ROOM1040 Figur 0, Flanke (9,97) bei Bild 200 -> bis Bild 500 keine [elzazombies-r39]-
  Zeile, kein Raumwechsel. Leons Montage-Riegel (unit_r35_cut1150_*, unit_r38_montage_*, integration_r35_cut1150,
  integration_r38_montage) s. 5.
* Laden (Lauf geladen97): Elza-Spielstand mit (9,97)=1 -> CONTINUE -> keine Flanke, keine Kette (re15_elzazombies_r39_
  geladen setzt den Flanken-Speicher auf das geladene Bit).
* Speichern: (9,95) und die Kopie-Bits (Zone 7) liegen in g_game.flags (re15_savedata_capture) -> Nachspawn nach dem
  Laden (Laeufe n1031, t5..t8 = Spielstand mit (9,95)).
* Ohne (9,97): keine Kette (Messung vorher 1.1 = Verhalten ohne Flanke; unit _ohne).

### 4.5 Ton (Laeufe ton_laut / ton_stumm, RE15_AUDIO_CAP_SYNC, Mess-Haken RE15_R39Q_KNALL_STUMM)
Vor Nachtrag 11: Differenz der Mitschnitte = Bilder 624..665 (42) und 805..846 (42), Differenz-RMS je 6567 = zweimal der
Tuerknall (Knall-Bank Satz 0, Folge #1 K ROOM1041 und #5 K ROOM1061). NACH dem Umbau (integration_r39_elzazombies_knall,
Auswertung auswerten_ton): Bilder 624..665 (42) RMS 6567 = Tuerknall Satz 0 (Auftakt 1041) und 806..849 (44) RMS 4657 =
Knall Satz 1 wie ROOM1030 (Ende 1061, vor dem Rollo-Schritt).

### 4.6 Nach dem Umbau (Nachtrag 11)
* Kette (Lauf k5, exe nach ea6e4cab): Flanke -> "Umzug: 10E1 lebend 4 -> 1061; (9,95)=1" -> Tuerknall 1041 (Folge #1 K T61 0)
  -> ROOM1061 (S +4, C 2, T105) -> Knall Satz 1 (K T237 1) -> ROOM1031 (S +3 Kriecher, erstes Bild C 12, T281) -> ROOM1041
  -> "Rueckkehr beendet, Steuerung frei". Abstaende 1061 Cut 2 wie 4.1 (7062 -> 6245, 6421 -> 4849, 6638 -> 5476,
  7547 -> 5009); 1031 Cut 12 Kriecher 9790 -> 7561, 10524 -> 5579, 9522 -> 4765, Raum-Zombies hinter dem Rollo
  12906 -> 8997 .. 15058 -> 9823; Spieler-HP min 100 in beiden Schnitten. Keine Spur mit Bit 128..132; ROOM1071 bleibt
  unberuehrt (unit kette_*: Tot-Bits 0xc6..0xca = 0 nach der Kette).
* Durchbruch 1041, echter Weg (Lauf d1): Spielstand ROOM1061 Etage 0 an der Tuer nach 1041 mit (9,95), 10E1-Quellen und
  1041-Kopien still, vier 1061-Kopien leben; Aktion -> `DOOR FIRE slot=2 ... spawn=(-21008,0,-13134)` -> ROOM1041 ->
  "Durchbruch 1041: 4 Zombies aus 1061 (nicht besiegt) -> hinter das Rolltor" -> erstes Bild Cut 1 (Folge C T0 1) ->
  Knall Satz 1 (T2) -> Rolltor faehrt hoch -> "Durchbruch Bild 266: alle durch das Rolltor" -> Szene bis Bild 356 ->
  "Durchbruch beendet, Steuerung frei (Haltebits geloest)", Elza wieder bei (-21008,0,-13134) Cut 5. Abstaende zur Kamera
  Cut 1: Kopien 12391 -> 6201, 13004 -> 8316, 13941 -> 6828, 14897 -> 9031; Spieler-HP min 100. Bilder
  Q_belege/q_1041_durchbruch.png (Bild 20 Rolltor zu, 100 Fuesse unter dem Rolltor, 180-300 die Zombies kommen durch auf
  die Kamera zu, ab 380 Elza an der 1061-Tuer, Cut 5).
* Gemessen und behoben: (1) "alle durch" kam im ersten Lauf schon in Bild 1 — die Lebend-Pruefung ueber die HP taugt nicht
  (Sce_em_set setzt hp 0, die INIT erst im ersten KI-Takt); (2) mit allen Zombies hinter dem Rolltor in der Zaehlung haette
  erst die Kappe geendet (drei der fuenf Raum-Records liefen VOM Tor weg, Abstand 24186 -> 28413 usw.) -> gezaehlt werden
  die umgezogenen Kopien (unit durch1041: "alle durch" bei Bild 295 > Rolltor 210, keine Kopie mehr hinter dem Rolltor).
* CONTINUE in 1041 loest den Durchbruch NICHT aus (der CONTINUE-Weg stellt den Schnitt des Spielstands nach den Installern
  her, main.c "FE-4"); er bleibt fuer das naechste Betreten durch eine Tuer.

## 5. Tests
Neue Riegel (re15_port/tests/unit/probes/r39_elzazombies.cmake), Stand nach Nachtrag 11:
* unit_r39_elzazombies_zensus — Quell-Records 10E1, Kopie-Bits frei (132 RDTs, 706 Sce_em_set, 176 Bits, 0 Desync), 1031-
  Zeilen woertlich, sub06/07/09 = ROOM1030, ROOM1041 = ROOM1040 (sub08 180 B, Schalter-Rechteck @0x01118, Rolltor @0x01190,
  Limit @0x01218, RID Cut 1), Programm G traegt die 1041-sub08-Zeilen woertlich, Kameras = RID, Tuer 10C1 -> 1061,
  Slot/Ereignis/Bank-5-Bits 28/29/30 frei.
* unit_r39_elzazombies_kette_4/_0/_2 — die Kette im Prozess je Ueberlebenden-Fall (4, 0, 2): Raumfolge ohne 1071-Schritt,
  Schnitte, Zahl der Kopien = lebende, jede Kopie >= 400 naeher, HP 100, Rueckkehr, (9,95), 1071 unberuehrt, 1041-Kopien
  bis zum Durchbruch still, zweite Flanke wirkungslos; kette_4 mit GEGENPROBE (Modul aus = keine Kette).
* unit_r39_elzazombies_nach1061 — Westseite, 200 Bilder gehalten, frei auf Etage 4 (kleinster Abstand 4179 >= 4000), kommen
  danach naeher (4179 -> 2670), getoetete Kopie fehlt.
* unit_r39_elzazombies_nach1031 — mit (9,95) drei Kriecher und keine Kopie; ohne (9,95) nichts.
* unit_r39_elzazombies_durch1041 — Tuer 1061 -> 1041 mit 2 lebenden von 3 Kopien (eine in 1061 getoetet): 2 ziehen um,
  erstes Bild Cut 1, Elza auf Leons Parkplatz, Rolltor hoch ab Bild 210, "alle durch" erst bei Bild 295 und ohne Kopie
  hinter dem Rolltor, Szene 386 Bilder ohne Raumwechsel, Kopien 12391 -> 5577 / 13941 -> 8860 naeher, HP 100, Elza zurueck
  am Eintrittspunkt, Riegel los, Haltebits los; naechstes Betreten: kein zweiter Durchbruch, 2 Durchgebrochene erscheinen;
  Eintritt im Nordteil: nichts; alle 1061-Kopien besiegt: nichts.
* unit_r39_elzazombies_ohne — Laden mit (9,97), ohne (9,97), Leon 1040/1060 (auch mit (9,95): kein Durchbruch).
* integration_r39_elzazombies_kette / _knall / _treppe / _durch / _ohne — echte exe (4.2, 4.4, 4.5, 4.6).

### 5.1 Abschlusslaeufe
* VOR Nachtrag 11 (Baum 05d03c9f): `ctest -R "^(unit_|prop_|probe_)" -j 4` -> "100% tests passed, 0 tests failed out of 845";
  gezielt 35/35 (Leons Montage, Elza-Raeume, Speichern) — ueberholt durch den Lauf unten.
* ENDSTAND (Baum 0f730673 = master 2963579d gemergt + Nachtrag-11-Umbau, gebaut 02:3x), EIN Lauf mit `-j 4`, Regex
  `^(unit_|prop_|probe_)|r39_elzazombies|^integration_r35_cut1150$|^integration_r38_montage$|^integration_r37_zombieaktiv$|^integration_elza_vollstart$|^integration_r38_elzaintro_|^integration_r39_elza_|^integration_r38_elzatoene_|^integration_r38_tagebuch_|^integration_r37_elzaems_elza10c1kraehe$|^integration_r36_story_elza$|^integration_r36_elza_|^integration_r33_speichern$|^integration_r39_heliport1171`
  (= alle 857 schnellen Tests + eigene 13 + Gruppen der beruehrten Module/Raeume: Leons Montage/Zombie-Abbildung, Elza-
  Raeume 1031/1041/10C1, Tagebuch/Karte, Speichern, Spur K nach dem Merge) ->
  "100% tests passed, 0 tests failed out of 886" (1107 s).

## 6. OFFEN (fuer die Integration, keine Luecke im Spurbaum)
* Spur P setzt (9,97) (VERTRAG 1.1). Meine Kette startet bei der steigenden Flanke, sobald keine Szene laeuft
  (re15_cine_active = 0, player_mode != 2, keine Karten-Anforderung). Mit P zusammen zu pruefen: (a) P's 1041-Szene darf
  beim Wiedereintritt durch meine Rueckkehr-Tuer nicht neu starten (P's eigenes Gesehen-Flag); (b) P's Panikmusik: laeuft
  sie ab dem Bit, klingt sie ueber die Montage-Raeume (Raumwechsel per Tuersatz wie jede Tuer); wartet sie auf "keine
  Szene" (wie Leons MAIN01-Fenster, cut_10f0.c), beginnt sie erst nach der Rueckkehr — P's Wahl; (c) Ereignis 30 und
  Bank-5-Bits 28/29/30 nur zustandsgebunden belegt (jede Abfrage prueft Raum UND Zustand) — P's 1041-Programm sollte
  Bank-5-Bit 30 nicht als eigenes Signal nutzen, solange mein Durchbruch laeuft (er laeuft nie gleichzeitig mit P's Szene:
  der Durchbruch kommt erst beim naechsten Betreten von 1041).
* Spur R (Hund 1121): falls R einen Zone-7-Tot-Bit braucht, nicht 124..131/133/144/145 (hier belegt, Zensus 2.4).
* Ankunft von OBEN (1121 -> 1061 Etage 8 -> 6 -> 4, Treppe A endet an der Westseite): die Kopien stehen dort, wo sie von
  unten "moeglichst weit weg" sind, und werden auf Etage 4 ebenso frei. Vom Nutzer nicht verlangt (nur "von unten").
* Durchbruch 1041 nach dem LADEN in 1041: wartet bis zum naechsten Betreten durch eine Tuer (4.6).

## 7. Fuer den Nutzer
* So laeuft es jetzt (Elza, Panikphase): Nach Johns Ruf und der Karte (Spur P) eine kurze Pause, ein Tuerknall, Schnitt
  ins Treppenhaus 2F: die Zombies, die du im Medical Room (10E1) uebrig gelassen hast, sind durch die Tuer von 10C1
  hereingebrochen und gehen auf die Kamera zu. Ein zweiter Knall, Schnitt in die Lobby (1031): das Notfall-Rollo bricht
  wie in der Original-Szene, Zombies kriechen darunter durch. Dann bist du wieder an deiner Stelle in 1041. Hast du im
  Medical Room alle getoetet, entfaellt der Treppenhaus-Schnitt.
* Steigst du danach von unten die Treppe in 1061 hinauf, stehen die Zombies am 2F-Absatz links unter dem Fenster (am
  weitesten weg vom Treppenkopf) und laufen erst los, wenn du oben angekommen bist.
* ROOM1041 (Nachtrag 11, "wie bei Leon"): Laesst du die Zombies aus dem Treppenhaus unbesiegt zurueck und betrittst
  1041 wieder (durch die Tuer von 1061, der Lobby oder den Aufzug), bricht das Rolltor wie bei Leon: Knall, das Rolltor
  faehrt hoch, genau diese Zombies kommen hindurch auf die Kamera zu, danach hast du wieder die Steuerung an der Tuer, durch
  die du gekommen bist. Warum nicht sofort beim Start der Panikphase: da stehst du selbst in 1041, und die Zombies aus dem
  Treppenhaus hattest du noch gar nicht vor dir — "falls die Zombies nicht besiegt wurden" kann erst danach gelten. Hast du
  sie im Treppenhaus besiegt, passiert in 1041 nichts. In ROOM1071 bricht nichts durch.
* Keine neuen Assets, keine Sprachdateien. Zeiten (Pause 60, Knall -> Schnitt 45, Schnittlaengen wie Leon) = Leons Montage.
* Entscheidungsfrage (optional): Sollen die Zombies am 2F-Absatz auch bei Ankunft von OBEN (Rueckweg aus 1121) "weit weg"
  stehen? Dann wuerden sie fuer diesen Fall an die Tuer-Seite gestellt.
