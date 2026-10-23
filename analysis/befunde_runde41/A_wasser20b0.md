# Runde 41 — Spur A: wasser20b0 (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_wasser20b0`, Zweig `r41/wasser20b0`, Basis master 116e8b0f.
Punkte (AUFTRAG.md):
1. 20B0-Wasserschwall im Original messen + EM024 (Typ 0x24) im Port reparieren.
2. STAGE2: Wassereffekt beim Sprung von der Kante ins Wasser; NPCs teilen die Wasser-Effekte.

## Stand / Protokoll (neueste Einträge unten)

### 2026-10-23 Start
- Baum sauber (`git status --porcelain` leer), HEAD 116e8b0f.
- Pflichtlektüre: AUFTRAG.md, VERTRAG.md, UEBERGABE.md (R40, Befund STAGE2) gelesen.
- Port-Stand: `enemy_ai_common.c` ab Z. 13563 "AMBIENT FX-EMITTER (type 0x24, EM024)":
  grid-0 (0x8010f130) / grid-1 (0x8010f3fc) Partikel-Sequencer -> 0x80019700 stehen als OFFEN (nicht gebaut).

## Punkt 1 — 20B0-Wasserschwall

### 1.1 Messung vorher (Port, Stand 116e8b0f + Messkopie re15_pc_r41a_mess.exe)
Lauf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=20B0@240 RE15_FX_LOG=fx.log RE15_ENEMY_DBG=1 RE15_SPAWN_DIAG=1 RE15_FRAMEDUMP=250-700/30:f
RE15_EXIT_AT=700#20b0` (Leon, Flag (3,0xEB)=0).
- debug.log: 35x `Sce_em_set type=0x24` (pos (7200,-1800,-8150..-4750), behavior 05 ausser slot 8/26 = 01,
  slot 17 = 00), `EM24 loaded: 1 meshes, 1 bones, 3 clips`, ESP-Bank 20B0 = ids 05 06 07 0b 0c 0d 0f
  (0x0f -> TIM-Slot 42, 256x96).
- Ablauf: F240 Cut_chg(11), Plc_dest(-15920,-6355) mode 5; Spieler laeuft -420 -> -6420 -> -12420 -> -15820
  (F30..F120 Raumzeit), danach mo=2 (Pad-Sperre (2,7)) bis F700, Cut 14, KEIN weiteres SCD-Ereignis:
  sub05 (Aot 7 -> Westtuer fliegt, Set (3,0xEB)) feuert NICHT -> Szene haengt.
- fx.log: KEIN einziger ESP-Partikel in 20B0 (die 39 Zeilen id=6 sub=5 stammen aus ROOM1240 F17..F55).
- Framedumps f000250..f000700: Spieler steht vor der West-Doppeltuer, KEIN Wasser sichtbar, die 35 EM024
  sind nirgends zu sehen (enemy_dbg: "ZEICHNE Typ 0x24: 1 Teile").
=> Zwei Defekte: (a) keine Partikel (grid 0/1-Sequencer nicht gebaut), (b) die EM024-Welle unsichtbar/
   wirkungslos, (c) sub05 feuert nicht (Aot-7-Zone; der Spieler steht in der Zone -17400..-15150).

### 1.2 RE-Belege Original (STAGE2.BIN, re15_disasm.py --bin STAGE2.BIN)
SCD ROOM20B0 sub00 @0x1AA6: `Ck(3,0xEB)==0` -> 35x `44 nn 24 gg 01 00 00 ff 20 1c f8 f8 zz zz 00 00..`
(x=0x1c20=7200, y=0xf8f8=-1800, z=0xe02a+nn*100 = -8150..-4750; grid gg: 05 ausser nn=08/1a -> 01, nn=11 -> 00),
dann Aot_set 7 (sce 3, Rect -17400,-8700 / 2250x4600, Ereignis `ff 18 05` = sub05) und `Evt_exec ff 18 02`
(sub02 SOFORT): Pad-Sperre (2,7), Cut 11, Pos_set Spieler (2380,-1800,-6355) Gier 2048, sub03/sub04 = Osttuer
obj 5/6 schwingt auf (Speed_set 4 +-0x80 x 8), Plc_dest (-15920,-6355) mode 5, Gosub 08 (warten (5,32)==0).
sub05 (Aot 7): Aot_reset 7, Cut 7, Pos_set (-15400,-1800,-7305), Plc_dest, sub06 (obj 4 schwingt -0x1c0 x 4),
sub07 (obj 3 = Tuerblatt fliegt: Pos_set/Dir_set-Folge), Cut 15, ..., Sleep 60, Cut 6, Plc_ret, Set (3,0xEB)=1,
(2,7)=0, (1,0x1b)=0, Sca_id_set 0..4 x 0x13/0x14 -> ff.
ROOM20B1 (Elza): byte-identische em_set-Liste (cmp: keine Differenz in den 35 Records).

EM024 (Typ 0x24), Wurzel 0x8010ee9c: Pause-Gate `g_pauseflags & 0x20000000` / `+0x9 & 0x20` @0x8010eea4-d0,
Zustandstabelle @0x80118d50: [0]=INIT 0x8010ef1c, [1]=ACTIVE 0x8010f020 (`lbu v0,4(v0)` @0x8010eee4).
INIT 0x8010ef1c: DAT_800acc0c:=1 @0x8010ef34; +0x1b8=0, +0x1b9=0; +0x93=1 @0x8010ef58; +0x8c=0x78 @0x8010ef6c;
  FUN_8011089c (+0x94=0, +0x95=0, +0x8f=0); anim_set(+0x84,+0x16c,0,0x200) @0x8010ef84; +0x1d0..+0x1ec (8 Worte)=0
  @0x8010efa8; *(+0x188) &= ~1 @0x8010efe4 (Kollision aus); +0x4=1 @0x8010eff4; +0x5 := +0x9 (grid) @0x8010f00c.
ACTIVE 0x8010f020: +0x1d4 := SquareRoot0(dx^2+dz^2) zum Spieler @0x8010f078; jalr Tabelle @0x80118d58[+0x5]
  @0x8010f0a8; +0x8c++ @0x8010f0c8; +0x34 (x) -= +0x8c @0x8010f0e8; x < -25000 (`slti -25000` @0x8010f100)
  -> x = 0x4e20 (20000) @0x8010f10c, +0x8c = 0xe1 (225) @0x8010f11c.
Sub-Tabelle @0x80118d58: [0]=0x8010f130 [1]=0x8010f3fc [2]=0x8010f910 [3]=0x8010fe24 [4]=0x80110338
  [5]=[6]=[7]=0x8011084c (`jr ra`). 20B0 nutzt grid 0, 1, 5.
grid 0 (0x8010f130): Phasentabelle @0x80118d88[+0x6]: [0]=0x8010f178, [1]=0x8010f2c0.
  Phase 0 @0x8010f178: wenn +0x1d0==0: 6 Spawns (Versatz/Spawn):
    (-1000,0xa8c=2700,-1200)/0x4000 ; (-2000,2700,0x4b0=1200)/0x4000 ; (-1000,2700,0)/0x3000 ;
    (-2000,0x514=1300,-1200)/0x4000 ; (-1000,1300,1200)/0x4000 ; (-2000,1300,0)/0x4000, jeweils a1=0, a2=7.
    +0x1d0++ @0x8010f26c; >= 11 (`sltiu 0xb` @0x8010f284) -> +0x1d0=0, +0x6++ @0x8010f2ac.
  Phase 1 @0x8010f2c0: wenn +0x1d0==0: 6 Spawns mit y 0x9c4=2500 / 0x44c=1100, alle /0x3000, a2=7:
    (-2000,2500,-1200) (-1000,2500,1200) (-2000,2500,0) (-1000,1100,-1200) (-2000,1100,1200) (-1000,1100,0);
    +0x1d0 >= 11 -> +0x1d0=0, +0x6=0 @0x8010f3e8.
grid 1 (0x8010f3fc): Phasentabelle @0x80118d90[+0x6]: [0]=0x8010f444 [1]=0x8010f64c [2]=0x8010f6b0 [3]=0x8010f8b8.
  Phase 0 @0x8010f444 (Takt +0x1d0 0..25, `sltiu 0x1a` @0x8010f610, dann +0x6++):
    Takt 0: (200,0x708=1800,0)/0x2000 a1=-200 ; Takt 3: (400,1800,-600)/0x2600 a1=-100, (400,0x960=2400,0)/0x2600 a1=-100 ;
    Takt 6: (600,1800,-1200)/0x2c00, (600,2400,-600)/0x3000, (600,0xbb8=3000,0)/0x2c00 ;
    Takt 9: (800,2400,-1200)/0x3200, (800,3000,-600)/0x3200 ; Takt 12: (1000,3000,-1200)/0x3800 ; alle a2=0.
  Phase 1 @0x8010f64c: nur Takt 0..10 (`sltiu 0xb`) -> +0x6++.
  Phase 2 @0x8010f6b0 (Takt 0..25): Takt 0: (200,1800,-1200)/0x2000 ; Takt 3: (400,1800,-600)/0x2600 a1=0x64,
    (400,2400,-1200)/0x2600 ; Takt 6: (600,1800,0)/0x2000 a1=0xc8, (600,2400,-600)/0x3000, (600,3000,-1200)/0x2c00 ;
    Takt 9: (800,0x258=600,0)/0x3200 a1=0x64, (800,3000,-600)/0x3200 ; Takt 12: (1000,3000,0)/0x3800.
  Phase 3 @0x8010f8b8: Takt 0..10 -> +0x6=0.
Helfer: 0x801108cc(x,y,z) = Versatzvektor s16->s32 nach 0x8011931c/20/24; 0x80110988(a0,a1,a2):
  a0 = 0x0f000000 | (a2<<16) | (a0&0xffff) (`lui v0,0xf00` @0x801109b8) -> Effekt-Id 0x0f, Sub a2, Skala a0;
  a1 = s16; a2 = *(+0x188)+0x40 (Teil-0-Matrix); a3 = &0x8011931c; `jal 0x80019700` @0x801109cc.
  (0x80110904 = Zufallsstreuung des Versatzes, wird von grid 0/1 NICHT gerufen.)
Effekt 0x0f der Raum-Bank (ROOM20B0.RDT idh @0x3450, ids 05 06 07 0b 0c 0d 0f; 0x0f eff_start 0x4bac,
  36 Anim-Saetze / 17 Zellen, TIM @0x21e34, rowblk 0x4d18, Sub-Offsets [4,26,48,70,92,114,136,158]):
  sub 0: 1 Strom, 2 Zeilen: Zeile0 A=10 (gate 4, p0e=7), Zeile1 A=0 acc=(0,10,0) vel=(120,-80,0);
  sub 1: vel (90,-180,0); sub 2: vel (60,-30,0); sub 3: vel (100,0,0) acc 0; (sub 4..7 s.u.).

### 1.3 Original-Messung (DuckStation, MZD-Disc, Debug-JUMP Stage 2 Raum 0x20B "L TUNNEL FLOOD")
Werkzeug: `re15_quickload.py --triangle 1 --right 11 --postload N` (Basis stage_saves/mzd_debugmenu.sav, Flag
(3,0xEB)=0), Savestates ts_10/12/14/16.sav + flut_a.sav (~50 s); Auswertung em024_dump.py (Entity-Feld
+0x34/+0x8c/+0x5/+0x6/+0x1d0, ESP-Pool 0x800a73b8 Stride 0x108) und re15_ss.py (Framebuffer).
Vorab: settings.ini hatte `ConfirmPowerOff = true` (Close haengt im Dialog, kein resume.sav) -> auf false
gesetzt (Skill-Voraussetzung); Media-Capture per Pad-Hotkey lieferte nur 0,9 s -> Savestate-Reihe statt Video.
Befund (widerlegt die R38-Notiz "Original-Debug-Sprung startet die Flut nicht"): der JUMP startet die Szene
sofort (sub02), Leon steht am Ende bei (-20657,-1800,-9629) Gier 2779 = Ziel des letzten Plc_dest in sub05
(0xaf74,-0x25e9 = -20620,-9705); flut_a.sav: Spielerzustand 1 (frei), alle 35 EM024 leben weiter (Zustand 1).
EM024-RAM (alle 35 gleich): +0x8c und x folgen exakt der Drift: ts_10 x=-17166/+0x8c=251 = Bild 131
(7200 - 131*(121+251)/2), erster Umbruch bei Bild 161 (x<-25000 -> x=20000, +0x8c=225), ts_12 x=15290/245
(Bild 181), ts_14 x=1172/297 (Bild 233), flut_a x=-3716/313 (Bild n*k...). +0x93=1, +0x94=0, +0x95=1 KONSTANT
(kein Bildvorschub), +0x5 = grid (5/1/0), +0x6 = Phase (grid 1: 0..3, grid 0: 0..1), +0x1d0 = Takt.
Part-Record *(+0x188): Wort 0 = 0 (Bit 0 = ZEICHENMASKE geloescht @0x8010efe4 -> das EM024-Mesh wird NIE
gezeichnet; nur die Partikel sind sichtbar), Matrix +0x40 = Einheitsrotation, T = (x, y-3329, z) =
Knochen 0 der Pose (0,-3329,0) + Entity-Lage.
ESP-Pool: nur Effekt 0x0f, st=7 (Flags lebend|sichtbar|Folge), sub 00 (grid 1) / 07 (grid 0), Skalen
0x2000/0x2600/0x2c00/0x3000/0x3200/0x3800/0x4000, +0x2e = a1 (-200/-100/0/100), Versatz +0x40 = die
Disasm-Vektoren, z.B. (200,1800,-1200), (-1000,2700,-1200), (800,600,0). Zu jedem Zeitpunkt 3..14 Plaetze
mit 0x0f aktiv. Weltlage (y) der Partikel -2389..-4879 = ueber dem Boden (-1800), Bewegung nach oben/vorn.
Bilder: ts_10 (Bild 131) Cut auf die Westtuer: ein Tuerblatt (obj 3) liegt schraeg quer, Leon in der Tuer;
ts_12 Nahaufnahme (Cut 15), ts_14 Leon frei neben dem Rahmen. Die Partikel stehen zu diesen Zeitpunkten
im Ostteil (x 1300..2800) und sind in den West-Cuts nicht im Bild -> Fruehphase (Bild 0..130, Osttuer, Cut 11)
noch zu messen (Serie 5..9 s).
Original-AOT-Mechanik fuer sub05: FUN_800436a8 (AUTO-Pass, a2=0 fuer Spieler Pool 1 @0x800436bc-cc und
jeden Gegner Pool 2 @0x80043720-28) wird UNBEDINGT je Bild aus der Hauptschleife gerufen (`jal 0x800436a8`
@0x8001ce1c, direkt nach FUN_80031c44/FUN_8002bd44) - kein Gate auf Pad-Sperre/Spielermodus. Port:
aot_common.c Z.1169 `if (in_cinematic && a->type != CAM_SWITCH) continue;` (BL-round 2026-05-29, Port-Wahl
ohne Adresse) blockiert den AUTO-Satz 7 (0x41), solange (2,7)=1 den Spieler auf player_mode 2 haelt.

### 1.4 Umsetzung
Neue Dateien: `re15_port/include/re15_wasser20b0_r41.h`, `re15_port/engine/src/wasser20b0_r41.c`.
- `re15_wasser20b0_r41_init(e)`: no_draw := 1 (Part-Wort Bit 0 geloescht @0x8010efc8-e4 -> Mesh nie gezeichnet),
  anim_frame := 1 (+0x95 = 1, Original-RAM), Takt (+0x1d0 -> sub_state_3) := 0 (@0x8010efa8).
- `re15_wasser20b0_r41_sub(e)`: der jalr @0x8010f0a8 ueber @0x80118d58[+0x5]: grid 0 (Phasen @0x80118d88, 2 x 11
  Takte, 6 Spawns bei Takt 0, sub 7) und grid 1 (Phasen @0x80118d90, 26/11/26/11 Takte, Spawns Takt 0/3/6/9/12,
  sub 0, a1 -200/-100/0/100/200); grid 5..7 = `jr ra` @0x8011084c; grid 2..4 OFFEN (kein RDT spawnt sie).
  Spawn = re15_esp_fx_spawn_rows(Raum-Bank, 0x0f, sub, Skala, Anker, INT32_MAX, a1) = FUN_80019700
  (a0 = 0x0f000000|sub<<16|Skala @0x801109b8-c0, a1 -> slot+0x2e, a3 = Versatz, a2 = Teil-0-Matrix).
- `re15_wasser20b0_r41_anker(e, off, out)`: Pose Clip 0 Bild 1 des EM024-Skeletts (re15_compute_actor_kf +
  re15_skel_compute_pose), Knochen 0 rot*off + trans, dann Gier + Lage (re15_skel_bone_to_world) = *(+0x188)+0x40.
  Erwartung aus dem Original-RAM: trans = (0,-3329,0), rot = Einheit.
- `re15_wasser20b0_r41_auto_aot(room, type)`: AUTO_EVENT-Saetze in 20B0/20B1 uebergehen die Port-Sperre
  in_cinematic (FUN_800436a8 @0x8001ce1c ohne Gate); bewusst auf die Flut-Raeume begrenzt (OFFEN: allgemein).
Haken (gemeinsame Dateien, "Runde 41 Spur A"): enemy_ai_common.c re15_fx_emitter_ai_tick (Init-Aufruf, Sub-Aufruf
VOR der Drift, Bildvorschub entfernt), aot_common.c Z. ~1169 (Bedingung + Include).
Tests: tests/unit/test_r41_wasser20b0.c (bytes = Walker ueber STAGE2.BIN gegen die Port-Tabellen; modell =
Teil-0-Matrix; sequencer = Takte/Phasen/Codes/Anker; raum = 20B0 + 20B1 am VM mit Drift-Pins ts_10/Umbruch/ts_14),
probes/r41_wasser20b0.cmake, tests/integration/test_r41_wasser20b0.cmake (echte exe Leon 20B0 + Elza 20B1:
fx.log id=15, Cut 7/6 = sub05, Spieler frei, kein "ZEICHNE Typ 0x24").

### 1.5 Nachmessung Port (Stand nach den Haken) und zwei Korrekturen
- Integrationslauf 1 (Leon 20B0, Debug-Sprung): fx.log 1844 Zeilen `id=15 sub=0|7` (Partikel 0x0f werden gezeichnet),
  `Cut_chg(7)` F~150 und `Cut_chg(6)` F228 (sub05 lief: Westtuer obj 3 fliegt, Set (3,0xEB)), F242 "letterbox
  closed -> gameplay (player_mode=0)" = Spieler frei (Original: Zustand 1 zwischen ts_12 Bild 181 und ts_14 Bild 233).
- Korrektur A (Zeichenmaske): enemy_dbg meldete "ZEICHNE Typ 0x24" in den Bildern 0..2 des Raums (Lauf
  RE15_EXIT_AT=3#20b0) — der Port zeichnet das frisch gespawnte EM024 VOR seinem INIT-Takt; das Original setzt
  die Maske im INIT des ERSTEN Entity-Takts (@0x8010efe4, Takt vor dem Zeichnen @0x8001ce04 < @0x8001ce34;
  Original-RAM tf_5/tf_8 Bild 1: Part-Wort 0). Port: no_draw schon beim Spawn (scd_vm.c op_sce_em_set, 1 Zeile).
- Korrektur B (Aktor-Plaetze): unit raum zeigte nur 20 EM024 (RE15_ACTOR_MAX 21): die Saetze 20..34 fielen weg,
  darunter der zweite grid-1-Emitter (Satz 0x1A, z=-5550) = die halbe Partikel-Kaskade. Original-RAM: alle 35 leben
  (DAT_800aca4e = 35, Slot 26 driftet und spawnt: Platz off (400,1800,-600) bei z=-6150). Die Runde-36-Notiz "Slots
  20..34 hinter dem 20er-Feld verworfen" (test_r36_gegnerzahl.c) ist damit widerlegt: `sltiu 0x14` @0x800372b4 ist die
  Schleife von FUN_80037250, Sce_em_set legt ohne Grenzpruefung ab (@0x800420dc/@0x80042118) und der Entity-Takt
  fuehrt alle. RE15_ACTOR_MAX := 36 (re15_actor.h; sizeof(re15_actor_t) = 1888 B -> +28 KB). Zensus aller RDTs:
  nur 20B0/20B1 (35) ueberschreiten 20 ungedeckelt; 1040/1041 (40) und 1030/1031 (20) haben Save(0x12)-Deckel 5/6.
  test_r36_gegnerzahl.c: Masken uint64_t, Erwartung 35 Aktoren (Belegkommentar im Kopf).
- Riegel: unit_r41_wasser20b0_{bytes,modell,sequencer,raum} + unit_r36_gegnerzahl_* (7) = 11/11 gruen.

## Punkt 2 — Wassereffekt beim Sprung von der Kante ins Wasser; NPCs teilen die Wasser-Effekte

### 2.1 Messung vorher (Port)
- Landung: climb_common.c Phase 4 (@0x800386d4) fuehrte den Block @0x80038758-cc als "Landestaub ... OPEN" (Dateikopf:
  "FUN_80019700(0x6033000, ...) @0x80038794/@0x800387bc — der Landestaub (8 Partikel)") — nicht gebaut. Lauf
  ROOM2090 (RE15_DEBUG_JUMP=2090, RE15_PLAYER_POS=4500,-15200,3072, RE15_PRESS=square@60): der Sprung setzt den
  Spieler mit y=0 INS Becken (RE15_PLAYER_POS kannte kein y, Band -1); fx.log ohne Effekt 6. Der Port-Steg liegt bei
  y=-1800 (SCA ROOM2090: Band-1-Zelle x 1850..7200 / z -18100..-14500, Kantenzellen Band 16 u1=02 bei z -19100..-18100
  und -14500..-13500; Becken Band 0 y=0, Zonen slot 2/3 Stempel -1620).
- NPC-Schritte: enemy_ai_common.c re15_npc_sub_walk fuehrt den Schritt als "OPEN (Footstep-SE der Phase 1, byte-genau
  lokalisiert, NICHT portiert)" — kein Wassereffekt, kein Schritt-Ton fuer Typen >= 0x40. Zensus STAGE2 (Skript):
  NPC-Spawns nur in 2000 (0x42, 0x40), 2001 (0x49, 0x49, 0x4b), 2011 (0x4b); kein ausgeliefertes Plc_dest-Ziel liegt
  in einem Wasserfleck (FLR 0x97) oder einer sce-8-Zone; die Mechanik ist fuer die neuen Szenen (Spur C/G) noetig.

### 2.2 RE-Belege (PSX.EXE, selbst disassembliert; Adressen auch im Kopf von include/re15_wassersprung_r41.h)
- Landung @0x80038724-d0: +0x38 := +0x1ba (Boden) @0x80038724-2c; `lh v1,-13604(v1)` (Spieler+0x88 Stempel) @0x8003873c;
  `lw a0,-13684(a0)` (Spieler+0x38) @0x80038744; `slt v1,v1,a0` @0x8003874c; `beq v1,zero` @0x80038758 -> nur wenn
  Stempel < y (Boden unter dem Wasserspiegel). Schleife `ori s1,zero,0x4` @0x8003875c / `addiu s1,s1,-1` @0x800387d0,
  je Durchlauf 2x `jal 0x80019700` (@0x80038794, @0x800387bc) mit a0 = 0x06033000 (`lui a0,0x603`/`ori 0x3000`
  @0x8003876c-70), a1 = +0x6a + ((s1-2)<<8) (@0x8003878c-98) bzw. +2048 (`addiu a1,a1,2048` @0x800387c0), a2 =
  Spieler+0x20 (`addiu s3,a2,25`, a2 = 0x800aca5b -> 0x800aca74 = Entity-Matrix, T = zuletzt gezeichnete Lage),
  a3 = 0x8007428c = (0,0,0,0) (`read 0x8007428c 4 --w 4`). Der Stempel ist in der Landung noch da: FUN_80031c44
  loescht +0x88 erst @0x80031da4, NACH dem Executor-`jalr` @0x80031cb4. "Landestaub" war eine Fehldeutung — es ist
  der Wasser-Spritzer (Effekt 6 = die Wasser-Sprites der Raum-Banken, Sub 3 wie der Stempel-Schritt).
- NPC-Schritt: Geh-Subs 4/5 Phase 1 @0x80051278-a8 / @0x800515e4-614 (a0 = 0 @0x80051290 bzw. 1 @0x800515fc),
  Phase 2 @0x800512d4-308, rueckwaerts @0x800519dc-a10 / @0x80051bd4-c08 (alle a0 = 0): Bildwort *(+0x168) Bit 0x4000,
  Fuss 7 - 3*((Wort>>12)&1), `jal 0x80045630`. FUN_80045630: Effekt-Zweig vor der Typweiche (Bit 0x80 -> 0x06050D00
  @0x800456b8-d0; Stempel -> 0x06032000 @0x800456e8-710), Typ >= 0x40 (`sltiu v0,v0,0x40` @0x80045730) -> Tonindex
  a0 + (Boden&0x7f) - 8 (`addiu s3,a0,-8` @0x8004575c), kein Objekt-Override. +0x88 wird je Entity NACH dem Dispatch
  geloescht (`sh zero,136(v0)` @0x8001a59c hinter `jalr` @0x8001a588) und vom AUTO-Pass Pool 2 (@0x80043720-28) neu
  gestempelt.

### 2.3 Umsetzung
Neue Dateien include/re15_wassersprung_r41.h, engine/src/wassersprung_r41.c:
- re15_wassersprung_r41_landung(p, stempel, t_y): Gate stempel < p->y, 8 Spawns 0x06033000 (re15_esp_fx_spawn_rows,
  Effekt 6 Sub 3 Skala 0x3000) bei (x, t_y, z) mit Gier rot_y + {512,2560,256,2304,0,2048,-256,1792}.
- re15_wassersprung_r41_npc_schritt(e, a0, fuss, rdt): Bodentyp an +0x34/+0x3c, Zweig Bit 0x80 / Stempel (water_y),
  Anker = Part-Weltmatrix * (0,100,0) (re15_wasser_r38_anker mit der NPC-Bank), Ton a0 + (Boden&0x7f) - 8 ueber
  re15_audio_footstep; Folge-Plaetze (Sub 3, Flags 0x04) je Bild nachgefuehrt (re15_wassersprung_r41_bildanfang).
- re15_wassersprung_r41_npc_takt(e, a0): Bildwort-Test (re15_actor_footstep auf der NPC-Bank, Bild des gezeichneten Takts).
Haken: climb_common.c Phase 4 (t_y vor dem Bodenschreiben, Aufruf nach hit_react), enemy_ai_common.c
re15_npc_sub_walk (Phase 1 a0 = sub==5, Phase 2 a0 = 0, je vor re15_npc_anim), wasser_r38.c (Getter
re15_wasser_r38_stempel + Folge-Haken im Bildanfang), platform/pc/main.c RE15_PLAYER_POS 4. Wert = y (Band -y/1800).
Riegel: unit_r41_wasser20b0_landung (2090: 8 Plaetze/Gier-Faecher/Anker, trocken 0), unit_r41_wasser20b0_npc (2000:
Fleck 0x97 -> 0x06050D00 Ton 15, Stempel -> 0x06032000 Ton 13, trocken 0, Bildwort-Takt).

### 2.4 Messung nachher (Port)
- Landung (exe, ROOM2090, Steg Band 1): `RE15_DEBUG_JUMP=2090@240 RE15_PLAYER_POS=4500,-14700,3072,1 RE15_PRESS=square@60
  RE15_CLIMB_TRACE RE15_FX_LOG RE15_KEIN_GATOR=1`: climb.log sub 9 (kind 0x02, 1 Bild) -> sub 10 (14 Bilder Band 1, 24 Bilder
  Band 0) -> Landung (4500,0,-14032); fx.log: 104 Zeilen `id=6 sub=3` (8 Plaetze Effekt 6 Sub 3 ueber ihre Lebensdauer),
  erster Spawn F79 bei w(4500,-640,-14032) fl=07 — y = die zuletzt gezeichnete Fall-Lage (Entity-Matrix +0x20), nicht
  der Boden. Vorher (gleicher Lauf ohne Haken): kein `id=6`. Framedumps r41a_m5/f000080/090 (Scratch).
- NPC-Schritt: nur am Riegel messbar (kein ausgeliefertes STAGE2-Skript fuehrt einen NPC durch Wasser, 2.1). Riegel
  unit_r41_wasser20b0_npc: Fleck 0x97 -> Platz 0x06050D00 am Fuss-Part (Anker y -2151 bei Lage -1800 = Pose),
  Ton 0x17-8 = 15; Trockenfleck 0x17 + Stempel -1620 -> 0x06032000, Ton 1+0x14-8 = 13; trocken -> kein Platz, Ton 15.
- BEFUND (Datenlage, Original): die NPC-Geh-Subs setzen +0x94 := 5 (`ori v0,zero,0x5` / `sb v0,148(v1)` @0x80051d38-3c)
  und pruefen das Bildwort dieses Clips. Zensus der ausgelieferten NPC-EDDs (CDEMD0.EMS): Typ 0x40 (Ada), 0x42 (Marvin),
  0x45: Clip 5 = 20 Bilder, KEIN Bildwort mit Bit 0x4000 (Aufsetzer; Clip 0 traegt 0x5 bei Bild 8, Clip 1 bei 7/22) —
  im Original erreicht ein Skript-Lauf dieser NPCs FUN_80045630 NIE (kein Schritt-Ton, kein Wasser-Effekt). Typen
  0x49/0x4b/0x4d (2001/2011) liegen nicht in CDEMD0.EMS (Elza-Archiv CDEMD1.EMS; nicht gezaehlt). Die Mechanik ist
  portiert und greift, sobald ein Geh-Clip Aufsetzer traegt; fuer die ausgelieferten NPC-Clips bleibt das Verhalten
  byte-gleich zum Original (keine Effekte) -> Entscheidungsfrage an den Nutzer (unten).

### 1.6 Messung nachher Punkt 1 (Integrationslauf 3, nach allen Korrekturen)
- integration_r41_wasser20b0: Leon 20B0 (room20b0.rdt) UND Elza 20B1 (PL04, room20b1.rdt, EM024 aus CDEMD1.EMS Sektor 795):
  je 35 EM024-Spawns, fx.log 2408 Zeilen `id=15 sub=0|7` (Partikel 0x0f gezeichnet), KEIN "ZEICHNE Typ 0x24" (Mesh nie
  gezeichnet), Cut_chg(7) F105, Cut_chg(6) F228, "letterbox closed -> gameplay (player_mode=0)" F242 (Original: frei
  zwischen Bild 181 und 233). Laufzeit 103 s.
- Drift-Pins am VM (unit raum): Bild 131 x=-17166/+0x8c=251 = ts_10.sav, Bild 161 Umbruch 20000/225, Bild 233 1172/297 =
  ts_14.sav — byte-gleich zum Original-RAM; alle 35 Emitter ohne Zeichnung; grid 0 132 + grid 1 124 Spawns in 234 Takten.

## Tests (Namen, Ergebnis)
- unit_r41_wasser20b0_bytes / _modell / _sequencer / _raum / _landung / _npc — gruen.
- unit_r36_gegnerzahl_{feld,raum1030,raum1031,raum1040,raum1041,raum20b0,kein_platz} (angepasst auf 36 Plaetze) — gruen.
- integration_r41_wasser20b0 (Leon 20B0 + Elza 20B1, echte exe) — gruen (103 s).
- Gezielte Gruppen der beruehrten Module: siehe Summenzeile am Ende.

## OFFEN
- Allgemeine Aufhebung der Port-Sperre `in_cinematic` fuer AUTO-Ereignis-Saetze (FUN_800436a8 @0x8001ce1c laeuft im
  Original ohne Gate): hier bewusst auf 20B0/20B1 begrenzt; die Verallgemeinerung braucht die volle Suite (andere Szenen
  mit AUTO-Saetzen unter Pad-Sperre).
- EM024 grid 2..4 (0x8010f910 / 0x8010fe24 / 0x80110338): nicht gebaut — kein ausgeliefertes RDT spawnt sie.
- DAT_800acc0c := 1 @0x8010ef34 (Spieler-Sekundaerbewegung, Konsument FUN_80024c30) weiter OFFEN (wie zuvor).
- Original-Fruehphase (Bild 0..130) nur als RAM gemessen (tf_5/tf_8 = Bild 1, tf_7 = Bild 48), kein Video: DuckStation-
  Media-Capture per Pad-Hotkey lieferte nur 0,9 s (erster LB-Druck wird geschluckt); Bild-Serie der Fruehphase steht aus.
- PSX-Ziel: RE15_ACTOR_MAX 36 kostet +28 KB BSS (PSX-Laufzeitbudget pruefen, wenn der PSX-Bau wieder gebaut wird).
- NPC-Schritt-Ton (-8) ist mit dem Effekt portiert (dieselbe Funktion); hoerbar erst mit Clips, die Aufsetzer tragen.

## Fuer den Nutzer
- Keine neuen Assets (Effekte 0x0f/6 liegen in den Raum-ESP-Banken der Original-RDTs), keine Sprachdateien, kein
  Patch an shared_assets.
- Sichtbar: in 20B0/20B1 spritzt beim Betreten das Wasser aus der Osttuer (zwei Kaskaden + sechs grosse Spritzer je
  Front), die Front laeuft den Gang hinunter und kehrt wieder; die Szene laeuft jetzt zu Ende (Westtuer fliegt, Leon/Elza
  wieder frei). Beim Sprung von der Kante ins Becken (2090) spritzt es an der Landestelle (8 Spritzer im Faecher).
- Bedienhinweis Messung: RE15_PLAYER_POS nimmt auch beim Boot den 4. Wert als Band (y = -Band*1800).
- DuckStation settings.ini: ConfirmPowerOff stand auf true (Capture-Skripte brauchen false, Skill re15-room-capture);
  fuer die Messungen auf false gesetzt und danach auf true zurueckgestellt.
- ENTSCHEIDUNGSFRAGE: Die ausgelieferten NPC-Geh-Clips (Ada/Marvin, Clip 5) tragen keine Fussaufsetzer — im Original
  spritzt und klingt kein NPC beim Skriptlauf durch Wasser. Soll der Port NPC-Schritte trotzdem erzeugen (PORT-WAHL;
  Form-Vorschlag: die Aufsetzer-Bilder des Geh-Clips 0 derselben EDD, Bild 8 links / Bild 22 rechts bei 0x40, auf den
  20-Bild-Clip 5 abgebildet) oder bleibt es byte-gleich zum Original?

### 1.7 Bildvergleich Original / Port (Fruehphase der Flut)
- Original tf_7.sav (Bild 48 nach dem Sprung, Cut 11/12): Leon rennt den Gang hinunter, hinter/neben ihm die
  gruenlich-weisse Spritzerwolke (Effekt 0x0f) der Wasserfront; im zweiten Framebuffer die Front weit im Gang.
- Port (Integrationslauf 3, Leon, Framedumps f000030/f000060 = Raumbild 30/60, Cut 12/13): Leon rennt, hinter ihm die
  gleiche gruenlich-weisse Spritzerwolke, bei Bild 30 noch an der Osttuer, bei Bild 60 im Gang.
  (Bilder im Scratch: orig/tf_7.png, r41_wasser20b0_wd/lauf_ca6f84d2_leon/f000030.png, f000060.png.)

### 1.8 Schatten der EM024-Emitter (Korrektur C, gefunden ueber integration_r36_gegnerzahl Teil e)
- Teil e des Runde-36-Riegels erwartete "20 Emitter, 21 Schatten (Spieler + 20)". Der Port warf fuer JEDEN Aktor einen
  Figurenschatten (main.c NPC-Schleife: "FUN_8001b064 is called PER-ENTITY ... each on-stage actor gets one").
- Beleg Original: der Schatten ist ein Aufruf aus dem Typ-Code des Overlays, nicht Teil des Zeichners. STAGE2.BIN enthaelt
  `jal 0x8001b064` an genau 8 Stellen (0x80100500, 0x8010a9a4, 0x8010c550, 0x80110b50, 0x80115870, 0x80115e40,
  0x80116404, 0x801169cc; Muster Feuer-Emitter 0x26: `addiu a1,zero,-1800` / `jal 0x8001b064` / `addiu a0,a0,176`
  @0x8011673c-44 in STAGE1.BIN) — KEINE davon im EM024-Bereich 0x8010ee9c..0x80110a00. Die unsichtbaren Emitter
  werfen im Original also keinen Schatten; der Port zeichnete 20 (jetzt 35) dunkle Ellipsen, die mit der Front durch
  den Gang gelaufen waeren.
- Port: main.c NPC-Schatten `&& npc->type != 0x24u` (Runde 41 Spur A, 1 Bedingung). test_r36_gegnerzahl.cmake Teil e:
  35 Emitter (Aktor 1..35), Schattenliste "gezeichnet 1 (Spieler), verworfen 0 (Kapazitaet 36)".
