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

## 0. Arbeitsplan
1. Leons Form messen/belegen (Runde 35 L irons_tod_1150.c, Runde 38 O montage_r38.c, Runde 37 H zombieaktiv_r37.c).
2. Elza: Durchbruch 10C1 -> Flur, Zahl = Zombies, die in 10E1 leben; 1061 Aufstellung; 1071 wie Leon; 1031 ueber das Rollo.
3. Nebenwirkungen: Leon unveraendert; ohne Bit 97 nichts; Speichern/Laden.
4. Messung am echten Weg mit Elza, Riegel unit + integration.

## 1. Messung vorher
(laeuft, s. 1.1)

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
* "das gleiche nochmal wie bei Leon in den ROOM 1071, falls die Zombies nicht besiegt wurden" = Leons 1070-Schritt:
  leben in 1071 Zombies (Tot-Bits 0xc6..0xca = 0), Tuerknall, sie erscheinen in ROOM1031 Cut 7 vor der 1071-Tuer.
* "im ROOM 1031 ueber die Absperrung/Das Rollo, so wie bei der cutscene" = Leons Tor-Schritt: die Original-Szene
  1031 sub08 (s. 2.2), Kriecher durch das aufgebrochene Rollo, Cut 12.

## 3. Umsetzung
(offen)

## 4. Messung nachher
(offen)

## 5. Tests
(offen)

## 6. OFFEN
(offen)

## 7. Fuer den Nutzer
(offen)
