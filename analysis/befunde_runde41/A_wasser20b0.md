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
- Video-Vergleich (1.9): Ort und Takt der Spritzer stimmen, die Wolke des Ports ist 2-5x duenner/dunkler (Pixelzahl)
  und der erste Schub an der Osttuer (Original Raumbild 6-9) erscheint im Port erst ab Bild 11-15; Zeitachse des
  Originals durch Ladestaus gedehnt. Naechster Schritt: Helligkeit/Mischmodus der Effekt-0x0f-Sprites im PC-Zeichner
  (fx_plattform_pc) gegen VRAM/CLUT des Originals (ShowVRAM, re15_ss) messen; Sprite-Groesse 0x4000/0x3000 pruefen.

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

## Tests — Summenzeilen (woertlich, gezielte Laeufe, KEINE volle Suite)
- `ctest --test-dir re15_port/build -R "^(unit|probe|prop)_(r41_wasser20b0|r36_gegnerzahl|r38_wasser|r38_wasserzombie|r34_wasser|re2_spider_water|r38_npckante|npc_|marvin_|aot_|scd_aot|rvd_scan|1090_climb|1090_npc|r36_klettern|r39_elza_klettern|room1140_spawn|spawn_entered|1030_spawncap|re2z_spawn|r34n_d_adaruf|r27_ada|ada_eintritt|r35_affen_npcband|r35_affen_sprung|r39_hund1121_sprung)"`
  -> `100% tests passed, 0 tests failed out of 68` (nach dem letzten Bau).
- `ctest --test-dir re15_port/build -R "^integration_(r41_wasser20b0|r36_gegnerzahl)$"`
  -> `100% tests passed, 0 tests failed out of 2` (integration_r36_gegnerzahl 88 s, integration_r41_wasser20b0 98 s, nach dem Schatten-Haken).
- `ctest --test-dir re15_port/build -R "^integration_(r41_wasser20b0|r38_wasser|r36_gegnerzahl|r36_klettern)$"` (Lauf VOR dem
  Schatten-Haken): `75% tests passed, 1 tests failed out of 4` — integration_r36_klettern Passed 521.97 s, integration_r38_wasser
  Passed 183.31 s, integration_r41_wasser20b0 Passed 97.94 s; integration_r36_gegnerzahl Teil e (20 Emitter / 21 Schatten) war
  der Befund 1.8 und ist mit dem Haken + der Riegel-Anpassung gruen (Zeile darueber).

## Stand (Abschluss 2026-10-23)
Beide Nutzer-Punkte gebaut und gemessen; Entscheidungsfrage zu NPC-Schritten (Clip-5-Daten) im Abschnitt "Fuer den Nutzer".

### 1.9 Original-VIDEO der Flut (Nachtrag 2 des Nutzers) und Bild-fuer-Bild-Vergleich mit dem Port
Aufnahme: DuckStation Media Capture (settings.ini Backend = FFmpeg, Container = mp4 — Nachtrag 1), Hotkey
ToggleMediaCapture auf SDL-0/LeftShoulder (vgamepad), Ablauf Debug-Menue -> Dreieck (Stage 2) -> Rechts x11 (0x20B)
-> Quadrat (Laden) -> 0,8 s -> LB (Aufnahme an); Skript scratchpad/r41a_flut_capture.py (Fassung "Square -> LB";
die Reihenfolge LB -> Square schluckt den Quadrat-Druck, das Video flut_c zeigte nur das Intro). Ergebnis:
`A_belege/orig_20b0_flut_duckstation_2026-10-23-18-48-26.mp4` (19,98 s, 1196 Bilder bei 59,8 fps, 13,7 MB):
0,0-4,7 s schwarz (Laden, blackdetect), dann Cut 11 (Leon an der Osttuer), Lauf mit der Spritzerwolke, Westtuer
(Cut 7, Tuerblatt), Nahaufnahme (Cut 15), Leon frei an der Westtuer (Cut 6).
Bildreihen (30 fps = ein Bild je Spieltakt, 320x240 PNG):
- `A_belege/orig_20b0/oNNN.png` = Original, NNN = Raumbild ab dem ersten nicht-schwarzen Bild (Videobild 141 bei 30 fps).
- `A_belege/port_20b0/pNNN.png` = Port (RE15_FRAMEDUMP=0-320/1, RE15_WINDOW_SCALE=1, Leon, Debug-Sprung 20B0),
  NNN = Raumbild (g_engine.frame_count ab dem Laden).
- `A_belege/kontaktbogen_20b0_orig_links_port_rechts.png` (alle 10 Bilder, Original links / Port rechts),
  `A_belege/vergleich_20b0.tsv` (je Bild: Partikelpixel n, Schwerpunkt cx/cy, Bbox bw x bh, mittlere Helligkeit;
  Partikelpixel = hell und gruenlich: G >= 150, G > R+8, G > B+8, R > 60 — Lampen weiss/gelb, Leon dunkelblau).
  Werkzeug scratchpad/flut_vergleich.py (reines Python, raw RGB).
Ergebnis (vergleich_20b0.tsv, Raumbild 0..110 = Cut 11..14 bis zur Westtuer):
- ORT: die Spritzerwolke liegt in beiden Reihen an derselben Stelle hinter/neben Leon — Schwerpunkt Original
  (102..114, 140..157) / Port (102..116, 137..160); Bbox Original 63..101 x 44..66 px, Port 52..85 x 35..55 px.
- ZEITPUNKT/TAKT: Schuebe im ~10-11-Bild-Rhythmus (grid 0: 11 Takte je Phase) in beiden Reihen: Original F25-26,
  F33-37, F44-48, F64, F75, F83-86, F94-97, F104-106; Port F24-26, F33-37, F44-48, F55-59, F69-70, F78-81, F90-92,
  F99-103. Bis F48 bildgenau gleich, danach laeuft der Port 4-5 Bilder VOR dem Original (F78-81 vs F83-86, F90-92 vs
  F94-97, F99-103 vs F104-106) — das Original-Video enthaelt Ladestaus (F16-23 sind acht identische Videobilder,
  tf_5/tf_8 standen beim Laden bei Raumbild 1), die Zeitachse des Originals ist also um die Staus gedehnt.
- ZAHL DER SPAWNS: Port fx.log = 8 Plaetze bei F7 (6 grid 0 + 2 grid 1), danach je Takt-0/3/6/9/12 wie disassembliert
  (Tabellen 1.2); im Video sind die Einzelsprites in der Wolke nicht trennbar (Pixelzahl als Mass).
- GROESSE/DICHTE: die gruenlich-hellen Pixel je Schub sind im Original 2-5x zahlreicher (Original 150..750 px je Bild,
  z.B. F6-9 314/498/642/747 und F36 342, F48 463, F105 460; Port 30..145 px, F36 82, F48 83, F102 108). Der erste
  Schub an der Osttuer (Original F6-9, 300-750 px) ist im Port erst ab F11-15 mit 33-145 px zu sehen. Die Bboxen sind
  aehnlich gross, die Wolke des Ports ist also DUENNER/dunkler, nicht kleiner -> OFFEN (unten): Helligkeit/Mischung
  der 0x0f-Sprites im PC-Zeichner (Zeile 0 Routine 10: Flags row[0x0e]=7, TPAGE|=row[0x16]=0, keine Additiv-Marke).

## Nachbesserung 1 (nach Abnahme 0, 2026-10-23 22:30 ff.)

Massstab: `A_abnahme_0.md` §12 (M1..M7). Je Mangel: Ursache / Messung vorher / Beleg / Aenderung / Messung nachher.
Belege dieser Nachbesserung: `A_belege/nb1/` (Vergleichsbilder, ESP-Pool-Dumps mit korrektem Platz-Stride, OWN-Bank-Dump,
Kennlinie Framebuffer->Video). Werkzeuge im Scratch (nb1/): pool_dump.py (Stride 0x84), fbvgl.py (exakter 15-bit-Framebuffer aus
dem Savestate gegen Port-PPM), sonde_npc_own.c (OWN-Bank-Dump), edit_tests.py.

### N1.0 Ueberblick
| Mangel | Ergebnis |
|---|---|
| M1 Wolkendichte 5-30x | WIDERLEGT durch Messung am exakten Framebuffer: der Port zeichnet dieselben opaken CLUT-Texel wie das Original (Histogramm je CLUT-Index +-10 %); die Abnahme mass die Tiefpass-Glaettung der Videokette. Restabweichungen der Szene gemessen und als OFFEN benannt (1 Bild EM024-INIT, 1 Bild Sprites beim Kamerawechsel, RVD 12->13 2-3 Bilder). |
| M2 NPC-Schritte | BEHOBEN: der Bau-Stand las die falsche Bank. Die OWN-Bank (+0x170/+0x174), die die Geh-Subs spielen, TRAEGT Aufsetzer (Clip 5 Bilder 7/22 bzw. 8/22) -> byte-true fuer 0x40/0x42/0x45/0x49/0x4d; PORT-WAHL-Regel nur fuer 0x4b (keine Marke in keiner Bank). Beide Archive. |
| M3 Szenen-Sperre WATER | BEHOBEN: ENV-Stempel (sce 7/8) ohne in_cinematic-Gate (aot_common.c), Riegel `stempel`. |
| M4 Letterbox | WIDERLEGT: 24 px sind byte-true (POLY_F4 @0x80072ecc/@0x80072efc, ts_12-Framebuffer 24 schwarze Zeilen); das Video ist der Overscan-Ausschnitt (224 Zeilen auf 240 gestreckt) -> 16/17 px; die Balken F5-F14 des Ports sind die FUN_80021a0c-Rampe des Vor-Sprung-Zustands (ROOM1240 Set(1,27)). |
| M5 Riegel | GEBAUT: `unit_r41_wasser20b0_npc` (Zensus beider Archive + Regelpruefung), `_npc_wasser` (echter Geh-Sub je Typ durch den Wasserfleck), `_stempel`; `integration_r41_wasser20b0_npc` (2000-Szene Marvin/Ada an der exe; Landung ueber die Tuer 20A0->2090 Leon und 20A1->2091 Elza). |
| M6 main.c | BLEIBT (begruendet, N1.6): Schatten-Bedingung ist noetig; der 4. Wert von RE15_PLAYER_POS traegt den Tuer-Landungsriegel (Band 2 in 20A0). |
| M7 Hundemaske | BEHOBEN: uint64_t. PSX-BSS weiter OFFEN. |

### N1.1 M1 — Wolkendichte: Ursache Videokette, nicht Zeichner
**Korrektur des Werkzeugs.** Der ESP-Platz ist 0x84 Byte gross (FUN_80053240: `pbVar3 = pbVar3 + -0x84`, Schleife von 0x800aa538
abwaerts bis 0x800a73b8 = 12672 B = 96 Plaetze; re15_esp.c "s2 = s0+12672"). em024_dump.py (Dossier 1.3) las mit Stride 0x108 nur
jeden ZWEITEN Platz — "3..14 Plaetze" waren 6..28. Neuer Dump `nb1/esp_pool_*.txt`:
- tf_7 (Takt 48, +0x8c = 168): **22** aktive 0x0f-Plaetze: 16 x sub 0 = alle Spawns der grid-1-Phase 2 seit Takt 37 (Takt 0
  (200,1800,-1200)/0x2000, Takt 3 (400,1800,-600)+(400,2400,-1200)/0x2600, Takt 6 (600,1800,0)/0x2000+(600,2400,-600)/0x3000+
  (600,3000,-1200)/0x2c00, Takt 9 (800,600,0)+(800,3000,-600)/0x3200) je BEIDER grid-1-Emitter (+0x74 = 801237fc / 80124a8c),
  dazu 6 x sub 7 (grid 0, Phase 0, Takt 44: (-1000/-2000, 2700/1300, ...)/0x4000, 0x3000). Alle Plaetze: fl=07, defW=defH=0x1000,
  tpage 0x000c, clut 0x7b12, +0x6e (Anim-Index) = Alter + 3, +0x6f = 1.
- tf_8 / tf_5 (Takt 1): genau die 8 Plaetze des Ports bei F7 (6 x sub 7 + 2 x sub 0 (200,1800,0)/0x2000 a1=-200), Anim-Index 4 =
  Port `frame=4`. Der Spawn-Satz des Ports ist damit byte-gleich (Lage, Skala, a1, Alter).
**Textur und Palette sind byte-gleich.** Der Installer FUN_800194f8 stapelt die Effekt-TIMs in VRAM-Spalten von `sVar10 = 0xf`
(`<< 6` = x 960) abwaerts, Umbruch bei y + h > 256 (`0x100 <`), die CLUTs ab (0x120, 0x1e0) = (288,480) kumuliert (`uVar9 +=
crect->h`); die Koordinaten-Saetze werden um den y-Versatz gepatcht (`*pcVar4 += uVar8`). Fuer ROOM20B0 (TIM-Hoehen 72/128/32/
104/184/88/96) liegt 0x0f bei VRAM **(768, 88)**: Vergleich mit der Datei-TIM @0x21e34 = **6144/6144** Halbworte gleich; die CLUT
bei (288, 492) (clut 0x7b12 = x 0x12*16, y 0x1ec) = Datei-Zeile 0 (16/16: 0000 e39a d737 ced4 c272 ba0f adac a56a 9d28 9907 90c5
8ca4 8883 8462 8041 8020). Der Port laedt dieselbe TIM in Slot 42 (Zeile 0).
**Zeichnen ist opak und unmoduliert.** FUN_800534c4: Primitiv-Code = 0x2C + ((flags >> 3) & 2) (`local_38 = *(param_2+0x6c) >> 3 & 2`,
`+7 = (char)r0[4] + ','`); flags = 7 -> 0x2C = POLY_FT4 ohne ABE. Im exakten 15-bit-Framebuffer von tf_7 (oberer Puffer = Cut 12,
Takt 47) sind in der Wolkenregion (x 60..200, y 70..170) **3857 Pixel = 23 %** exakt CLUT-Farben (Indizes 3..12: (32,41,24) 906x,
(57,65,49) 826x, (41,49,32) 673x, (65,74,57) 393x, (82,90,74) 308x, (98,106,90) 236x), und nur **21** Pixel erfuellen das
Abnahme-Kriterium "gruenlich G>=100" (5 bei G>=150). Die Groessenformel des Ports (main.c pc_draw_effects) ist dieselbe wie im
Decompile (param_5 = S * scale16 * camf, / (sz<<4), w16 = defW * step, stepX = w16 / S, Zellen (u,v)..(u+S,v+S)); camf = fov
26684 >> 7 = 208 fuer ALLE Cuts 11..15 (RDT+0x60+cut*0x20+2, tf_7 DAT_800b0fe4=13 -> 208).
**Port = Original am exakten Framebuffer, gleicher Takt, gleicher Cut.** Kamera per RE15_FORCE_CUT=12 gehalten, Port F52 (EM024-
Takt 46, Spieler-Takt 47) gegen tf_7 oben (Takt 47, Anzeige = Zustand des Vortakts, s.u.), Region x 60..200 / y 70..170,
CLUT-Index-Histogramm (Port-Farben sind die CLUT-Werte in der `<<3`-Abbildung, Original in `*255/31`):
```
Index:   3    4    5    6    7    8    9   10   11   12
Orig :   5   21   40  240  321  432  925  779 1079    1   (3857 CLUT-Pixel)
Port :   5   22   57  242  336  470  910  809  743    1   (3597 CLUT-Pixel)
```
(`nb1/vgl_tf7_oben_F52cut12.png`: Original links, Port rechts — dieselbe Wolke an derselben Stelle mit derselben Loecherstruktur.)
**Opak-Emulation der Port-Zeichenliste.** Rasterung der 12 Sprites von F11 (Zellen nearest, Index 0 transparent) ergibt 7825
deckende Pixel, davon **90** "gruenlich hell" (Kriterium G > R+8 schliesst alle CLUT-Indizes >= 4 aus; Indizes 1..3 kommen in der
Textur nur 87-mal unter 24576 Texeln vor) — der Port zeigt gemessen 33..145 (`nb1/emu_F11.png`, `nb1/port_fx_F11.txt`). Mehr als
~100 solcher Pixel kann diese Textur opak nicht liefern.
**Kennlinie Framebuffer -> Video** (`nb1/kennlinie_fb_video.txt`, tf_7 oben gegen o045): Hintergrund (rechte Wand) 1:1 (FB G 32..47
-> Video 35.0, 96..111 -> 102.0), die Wolkentexel dagegen gemittelt: FB (32,41,24) -> Video (70,75,63), (57,65,49) -> (73,79,67),
(98,106,90) -> (84,91,78). Das 1-px-Muster aus hellen und dunklen Texeln wird von BilinearSmooth-Skalierung 320->640 (settings.ini
[Display] Scaling = BilinearSmooth), yuv420p-Chroma-Unterabtastung, VP9 6000 kbit/s und der Rueckskalierung auf 320 tiefpassgefiltert:
Region FB 21 gruenlich (G>=100) vs Video 1477. Der Abnahme-Massstab (Pixelzahl G>=100/150 im Video) mass also die Kodierung.
Bild-fuer-Bild identisch ist der Port am exakten Framebuffer; das Video kann es prinzipiell nicht sein.
**Zeitachsen (gemessen an der exe mit RE15_STATE_LOG/RE15_WAFFEN_LOG/Framedumps, `nb1/m1b..m1e`).** Nach dem Debug-Sprung: F1..F5
sind schwarz und der Spielschritt laeuft nicht (keine w.log-Zeilen F1..F5, fx=0), F6 = erstes Bild (EM024-INIT), F7 = EM024-Takt 1
(frame=4 wie tf_8), Takt 4 bei F10, Takt 7 bei F13; der Spieler steht bei F53 auf x=-5020 = Original Takt 48 (tf_7). Also Skript
F = Takt + 5, EM024 F = Takt + 6. Im Original laufen die Takte ab dem Ladebild (tf_8: Takt 1 bei noch SCHWARZEM Framebuffer), das
Bild erscheint bei o1 (Helligkeit 29). Der "fehlende erste Schub o5-9" der Abnahme ist der Schub p11-15 (Versatz 6 = 5 Schwarzbilder
des Sprung-Pfads + 1 Takt INIT): `nb1/vgl_erster_schub_zoom.png` (o6/o7/o8 links, p11/p12/p13 rechts).
**Fernbild (Cut 13).** Der untere Puffer von tf_7 (DAT_800aca34 = 0 = der gerade gebaute Puffer, Cut 13) zeigt Leon OHNE Wolke;
das Rohvideo o058 (erstes Bild nach dem Schnitt) ebenso, o060/o062 zeigen die grosse Wolke wie der Port bei F52/F54
(`nb1/vgl_fern_video_port.png`, `nb1/vgl_fern_zoom.png`). Das Original zeichnet die Sprites im ERSTEN Bild eines Kamerawechsels
nicht; der Port zeichnet sie (F50). Mechanismus nicht RE't (Kandidat: FUN_80052fd0 `if (DAT_800a73a4 != DAT_800aca34)` /
Reihenfolge beim Cut-Apply @0x80021c00) -> OFFEN (1 Bild je Schnitt).
**RVD 12 -> 13.** Port: Anforderung bei F49/F50 (x -4220 / -4420), sichtbar F51; Original: Takt 47/48 (x -4866 / -5020, tf_7
oben/unten). Zone = RVD-Satz 52 (from 12 to 13, x -5800..-4200): der Port schaltet an der Zonengrenze, das Original ~600-800
Einheiten tiefer -> 2-3 Bilder. Kamerazonen-Mechanik, nicht Spur A -> OFFEN (Orchestrator).
**Ergebnis M1:** als Zeichner-/Dichte-Mangel widerlegt (Textur, Palette, Code, Groesse, Zahl und Lage je Platz belegt gleich);
die drei verbleibenden Zeitabweichungen (INIT-Takt, Schnittbild, RVD) sind gemessen und OFFEN.

### N1.2 M4 — Letterbox: 24 px byte-true, 16/17 px = Overscan-Ausschnitt des Videos
- Original-Primitive (PSX.EXE-Daten, gelesen): DAT_80072ecc POLY_F4 (0,0)-(320,0)-(0,24)-(320,24), DAT_80072efc (0,216)-(320,216)-
  (0,240)-(320,240); zweiter Satz +0x18 gleich. = Port render_pc.c (24 px).
- Exakter Framebuffer ts_12 (Cut 15, DAT_800b5568 = 0xF0, Bit 0x10 gesetzt): **24 schwarze Zeilen oben und 24 unten** in beiden
  Puffern. ts_14 (Level 64, Bit frei): 24 dunkle Zeilen (Rampe).
- Video: DuckStation `CropMode = Overscan`, Aufnahme 640x480. Alignment-Test tf_7 (oben) gegen o045: Identitaet mean|diff| 8.79,
  "Zeilen 8..232 auf 240 gestreckt" **2.83** -> das Video ist der 224-Zeilen-Ausschnitt; 24 Balkenzeilen - 8 = 16 Zeilen,
  gestreckt 17.1 px = die 16/17 px der Abnahme.
- Port-Balken F5..F14 nach dem Debug-Sprung: Helligkeit der obersten 24 Zeilen 0.0 (F1..F10), 0.4 / 2.6 / 7.8 / 16.1 / 28.9
  (F11..F15) = die 15-stufige -0x10-Rampe von FUN_80021a0c ab 0xF0 (subtraktiv, Hintergrund 28.9). Der Level war vor dem Sprung
  0xF0, weil ROOM1240 (Startraum des Harness) `Set(1,0x1b,1)` traegt (ROOM1240.RDT @0x0055e) und der Raumlader nur das
  Richtungsbit loescht (FUN_800396fc). Im Original kam der JUMP aus dem Debug-Menue (Level 0; tf_7: DAT_800b5568 = 0). Beim
  echten Weg (Tuer) verhalten sich beide gleich (derselbe Mechanismus). -> kein Mangel der Szene, Harness-Artefakt.

### N1.3 M2 — NPC-Schritte: die falsche Bank
**Ursache.** `re15_wassersprung_r41_npc_takt` las das Bildwort aus `b->anim` (Container-Bank dir[1]); deren Clip 5 ist eine
20-Bild-Kniebeuge (Fuesse statisch, Wurzel -1874 -> -1513) ohne Marke. Die NPC-Geh-Subs rufen anim_set aber mit dem Paar
+0x170/+0x174 (`lw a0,368(v0)` / `lw a1,372(v0)` vor `jal 0x8001f314` @0x800512bc-c0, Sub 5 @0x80051714-18, Sub 6
@0x80051884-88) = OWN-Bank dir[3]/dir[4] (FUN_80022300 @0x800224b8/@0x800224c8; Port `b->anim_own`, Regel
`re15_actor_uses_own_bank`). +0x168 zeigt auf das Bildwort DIESER Bank.
**Beleg (Sonde nb1/sonde_npc_own.c, `nb1/npc_own_bank.txt`, beide Archive via TOC @0x80072f38/@0x80073178):**
```
Typ   Archiv        OWN Clip 5 (30 Bilder)  Marken        OWN Clip 0 (22 Bilder)  Marken
0x40  CDEMD0/1      2222222 6 0..0 5 2222222  7: 0x6 (Fuss 7), 22: 0x5 (Fuss 4)   5 0..0 2 2 2 7 2..2 0 0   0: 0x5, 12: 0x7
0x45  CDEMD0/1      wie 0x40                                                   wie 0x40
0x49  CDEMD0/1      wie 0x40                                                   wie 0x40
0x42  CDEMD0/1      222222 0 0 4 0..0 2 2 7 2222222  8: 0x4 (Fuss 7), 22: 0x7 (Fuss 4)   4 0..0 2 2 2 7 2..2   0: 0x4, 11: 0x7
0x4d  CDEMD0/1      wie 0x42                                                   wie 0x42
0x4b  CDEMD0/1      28 Bilder, alle 0 (auch Clips 0..8 und Container)  -> KEINE Marke
```
Die Marken sitzen an den Fersenaufsaetzen der Pose (0x40 Clip 5: Teil 4 erreicht seine Tiefstlage y -163 bei Bild 7, Teil 7 bei
Bild 22; `nb1/npc_own_bank.txt`). Die Entscheidungsfrage des Bau-Stands war damit gegenstandslos: im Original spritzen und
klingen Ada/Marvin/0x45/0x49/0x4d beim Gehen — der Port las die falsche Bank.
**PORT-WAHL nur fuer 0x4b** (Wurzelhoehe -1250 = Kind; NUTZER-VORGABE Nachtrag 3 / E1 "alle NPCs"): Aufsetzer = Bild, in dem der
Fuss-Teil 4 bzw. 7 nach der Schwungphase erstmals seine Tiefstlage erreicht (Pose-y >= max - (max-min)/10, Vorbild < Schwelle).
Form an den markierten Typen belegt: 0x40/0x45/0x49 Clip 5 Marken 7/22 -> Regel 7/20, Clip 0 Marken 0/12 -> 0/11; 0x42/0x4d
Clip 5 Marken 8/22 -> 5/18, Clip 0 0/11 -> 21(-1)/10. Ergebnis fuer 0x4b: Clip 5 Bilder 5 (Teil 4 -> Code 7) und 19 (Teil 7 ->
Code 4), Clip 0 Bilder 0 und 10. Fuss-Code wie in den Original-Daten (Teil 4 landet -> Bit 0x1000 frei -> Code 7).
**Aenderung** (`wassersprung_r41.c`): `re15_wassersprung_r41_npc_bank` (Own/Loco/Container nach der EINEN Regel),
`clip_hat_marken`, `portwahl_ableiten` (Pose-Regel, einmal je Bank/Clip), `re15_wassersprung_r41_aufsetzer` /
`_regel` (Riegel-API), Anker der Spritzer aus derselben Bank; Mess-Log `RE15_NPCSCHRITT_LOG=<datei>` (je Schritt: F, slot, typ,
clip, bild, fuss, boden, stempel, zweig, ton, plaetze, portwahl, pos).
**Messung nachher (Riegel, woertlich):** unit `npc_wasser` — echter Geh-Sub (state 4 / sub 4, Plc_dest-Felder @0x80041c14-24)
von (-4300,-3000) nach (-4300,-9000) durch den Fleck FLR 0x97 in ROOM2000, 150 Takte:
```
CDEMD0 0x40: z -3000 -> -8974, Boden 0x97, Schritte 6 (boden 6), Plaetze 6, Ton 15, letztes Bild 22 Clip 5
CDEMD0 0x42: z -3000 -> -8974, Schritte 6, Plaetze 6, Ton 15        CDEMD0 0x45: z -> -8926, Schritte 6, Plaetze 6, Ton 15
CDEMD1 0x49: z -3000 -> -8974, Schritte 6, Plaetze 6, Ton 15        CDEMD1 0x4d: z -> -8926, Schritte 6, Plaetze 6, Ton 15
CDEMD1 0x4b: z -3000 -> -8933, Schritte 9 (portwahl 9), Plaetze 9, Ton 15, letztes Bild 5 Clip 5
```
(Ton 15 = (0x97 & 0x7f) - 8 mit a0 = 0 im Sub 4 @0x80051290; Effekt 0x06050D00 je Schritt.)

### N1.4 M3 — ENV-Stempel ohne Szenen-Gate
`aot_common.c` (Z. 1169): die Szenen-Sperre `in_cinematic` ueberspringt WATER (sce 8) und RAMP (sce 7) nicht mehr. Original:
FUN_800436a8 wird @0x8001ce1c unbedingt gerufen (Spieler-Pool `ori a1,zero,0x1` / `jal 0x80042bac` @0x800436bc-cc, Gegner-Schleife
`sltu v0,s0,v0` @0x80043740 ueber alle aktiven Entities). Die Verallgemeinerung fuer die uebrigen Typen (AUTO/FLAG_CHG ausserhalb
20B0/20B1) bleibt OFFEN (volle Suite). Riegel `unit_r41_wasser20b0_stempel`: ROOM20B0 WATER-Satz Slot 5 (p0 = -900): NPC und
Spieler unter player_mode 2 gestempelt (+0x88 = -900), ebenso bei player_mode 0.

### N1.5 M5 — Riegel
- `unit_r41_wasser20b0_npc`: OWN-Bank-Zensus beider Archive (Clip 5/0 Bilder und Fuss-Codes je Typ wie Tabelle N1.3, Container-Clip 5
  ohne Marke), Regelpruefung (|Regel - Marke| <= 4 an 0x40/0x42/0x45/0x49/0x4d), Zweige von FUN_80045630, Takt an der OWN-Bank.
- `unit_r41_wasser20b0_npc_wasser`: s. N1.3 (6 Typen, beide Archive, echter Geh-Sub).
- `unit_r41_wasser20b0_stempel`: s. N1.4.
- `integration_r41_wasser20b0_npc` (echte exe): (a) ROOM2000 Leon, Flag (4,244): Marvin (Slot 1, 0x42) und Ada (Slot 2, 0x40) laufen
  per Plc_dest; RE15_NPCSCHRITT_LOG muss >= 4 Schritte je Slot (Clip 5, Ton >= 0) zeigen; (b) Landung ueber den ECHTEN Weg:
  Tuer 20A0 -> 2090 (Leon) und 20A1 -> 2091 (Elza; Tuersatz @0x1754 byte-gleich: `3b 00 02 31 02 00 7a0d 9411 b80b e803 9c18 f8f8
  86c0 0008 01 09 02 01`), Eingabeskript der Abnahme (E2), climb.log "Landung" + fx.log >= 8 Zeilen `id=6 sub=3`.

### N1.6 M6 — main.c
Beide Haken bleiben: die Schatten-Bedingung `npc->type != 0x24u` ist noetig (EM024 ohne `jal 0x8001b064`, Abnahme §4 bestaetigt);
der 4. Wert von RE15_PLAYER_POS (Band) traegt den Tuer-Landungsriegel (Spawn in 20A0 auf Band 2 vor der Tuer) und die Abnahme-
Laeufe B/D/E2. Konfliktrisiko mit Spur D gering (andere Stellen); zur Integration gemeldet.

### N1.7 M7
`s_dog_noclamp_mask` ist uint64_t (`(uint64_t)1u << slot`, enemy_ai_common.c). PSX-BSS (+28 KB) weiter OFFEN.

### N1.8 Weitere Belege (Disasm, am 2026-10-24 mit re15_disasm.py nachgeprueft, PSX.EXE)
- Sub 4 Phase 1 Fusstest @0x80051278-ac: `lw v0,360(v0)` (+0x168) / `lw v1,0(v0)` / `andi v0,v1,0x4000` @0x80051288 / `beq`
  @0x8005128c / `addu a0,zero,zero` @0x80051290 / `andi v0,v1,0x1000` / `srl v0,v0,12` / a1 = 7 - 3*bit (`ori v0,zero,0x7`
  @0x800512a4, `subu a1,v0,a1` @0x800512ac) / `jal 0x80045630` @0x800512a8. anim_set des Sub 4: `lw a0,368(v0)` / `lw a1,372(v0)`
  @0x800512bc-c0 vor `jal 0x8001f314` @0x800512c4 = +0x170/+0x174 = OWN-Bank.
- Sub 5 Phase 2 @0x80051640-74: `lw v0,360(a0)` / `andi v0,v1,0x4000` @0x80051650 / `andi v0,v1,0x1000` @0x80051658 (Verzoegerungs-
  platz) / `addu a0,zero,zero` @0x8005165c (a0 = 0 auch im Lauf-Sub, Phase 2) / `ori v0,zero,0x7` @0x8005166c / `jal 0x80045630`
  @0x80051670; anim_set des Sub 5: `lw a0,368(v0)` @0x80051714 / `lw a1,372(v0)` @0x80051718 / `jal 0x8001f314` @0x8005171c.
- Gleichzeitig gezeichnete 0x0f-Plaetze im Port: 6..28 je Bild (Abnahme 0 §5.6), bei Takt 47/48 (F53/F54) 22 = die 22 aktiven
  Plaetze von tf_7 (`nb1/esp_pool_tf_7.txt`).
- AOT-Zensus (re15_port/tools/aot_sce_census.py, 240 RDTs, 2295 Saetze): sce 7 (RAMP) nur ROOM5060/5061 (je 2), sce 8 (WATER)
  32 Saetze in 11A0/11A1, 1260/1261, 2000/2001, 2070/2071, 20A0/20A1, 20B0/20B1 (je 2), 2020/2021 (je 1), 2090/2091 (je 3) — der
  Wirkungsbereich der M3-Aufhebung.

### N1.9 Fortsetzung 2026-10-24: Riegel-Defekt im Teil landung_elza (Sitzungslimit der Vorgaengerin)
- Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, `... build` -> `ninja: no work to do.` /
  `=== LOCAL-BUILD-OK (build)`; re15_pc.exe 23:40:04 juenger als jede Quelle des Diffs (wassersprung_r41.c 23:39:33,
  aot_common.c/enemy_ai_common.c 23:34:17) -> der Stand 35860e86/42fbd21f kompiliert und ist gebaut.
- Befund (letzter Lauf der Vorgaengerin, integration_r41_wasser20b0_npc 390 s): npc2000 und landung_leon gruen, landung_elza
  "Process terminated due to timeout" + "Tuer 20a1 -> 2091 nicht genommen (DOOR FIRE=-1, geladen=-1)".
- Messung (eigener Lauf A_nb1c/elza1, dieselbe Umgebung wie der Riegel): debug.log `[input-script] Tick 270 -> F0 Tasten
  0x8000` ... `Tick 488 -> F0 Tasten 0x8000` — ALLE 7 Skript-Tasten fielen in den Vorspann (Spielbild F0), erst DANACH
  `AUTO-JUMP -> ROOM20A1 (Frame 240)`. Ursache: der Elza-Lauf startete ueber RE15_TITLE_CONFIRM_MS=1500 (main.c: Bestaetigung
  nach WANDUHR-Zeit) + RE15_PSELECT_AUTO, das Eingabeskript zaehlt aber gerenderte Bilder MIT Vorspann (input_pc.c
  s_input_ticks); ohne VSync laeuft der Titel mit 40..980 Bildern/s -> die Tick-Zeitachse des Skripts ist nach diesem Vorspann
  unbestimmt. Leon (RE15_TITLE_SHOT, AF=2) hat einen Vorspann fester Bildzahl: `Tick 270 -> F27` = Tuer.
  => RIEGEL-Defekt, kein Spiel-Defekt.
- Aenderung (tests/integration/test_r41_wasser20b0_npc.cmake): Elza startet wie Leon ueber RE15_TITLE_SHOT (+_AF=2) mit
  RE15_TITLE_SHOT_CHAR=1 = `re15_gameflow_new_game(1)`, derselbe Aufruf wie das Player-Select-Ende (main.c, Zweig NEW GAME
  `re15_gameflow_new_game(ch)`); zusaetzlich prueft der Riegel das Szenario am debug.log (`Elza-Bit=0` Leon / `Elza-Bit=1`
  Elza). Kontrolllauf A_nb1c/elza2: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `Tick 270 -> F27 Tasten 0x8000`,
  `[aot] DOOR FIRE slot=0 rect=(4950,5000,hw=1500,hh=500) target_cut=2 spawn=(6300,-1800,-16250)`, `PC loaded room2091.rdt`,
  climb.log `T459 room=2091 ... pos=(6363,-640,-14032) rot=3072 band=0 sub=10 kind=0x02 ph=4`, fx.log 104 Zeilen `id=6 sub=3`,
  exit 0 nach 27 s.
- Riegel danach: `ctest --test-dir re15_port/build -R "^integration_r41_wasser20b0_npc$" --timeout 600` -> `100% tests passed,
  0 tests failed out of 1` (118.03 s): npc2000 Schritte Slot 1 (0x42, Clip 5) = 7, Slot 2 (0x40, Clip 5) = 7, Toene >= 0;
  landung_leon Landungen 1 / 104 Zeilen id=6 sub=3; landung_elza Landungen 1 / 104 Zeilen id=6 sub=3.
- Nebenversuch (kein Riegel): eine Elza-Szene mit laufendem NPC an der exe gesucht — ROOM11C1 (Elza, Debug-Sprung, 1500
  Bilder, A_nb1c/e11c1) spawnt 5 Hunde 0x20 statt eines NPC (Leons 11C0: Marvin 0x42 laeuft per Plc_dest, Clip 0 Bilder 0/11);
  ROOM2001 mit (3,66)/(3,68) spawnt John 0x49 + 0x4B ohne Plc_dest (Lauf der Vorgaengerin, nb1/s2001). Beide Szenarien der
  NPC-Schritte bleiben am Unit-Riegel `npc_wasser` (CDEMD0 UND CDEMD1, echter Geh-Sub) belegt, an der exe Leons 2000-Szene.

### N1.10 Fortsetzung 2026-10-24: der NPC-Schritt in ALLEN NPC-Subs (eigener Befund, nicht aus der Abnahme)
- **Befund (gemessen).** Leons 11C0-Szene (Lauf der Vorgaengerin nb1/s11c0, RE15_NPCSCHRITT_LOG): `F1071 slot=1 typ=0x42 clip=0
  bild=0 fuss=7 ... ton=22` und `F1072 slot=1 typ=0x42 clip=0 bild=0 fuss=7 ... ton=21` - zwei Schritte aus DEMSELBEN Bild in zwei
  Folgetakten am Laufbeginn (Plc_dest Modus 0x05 -> Sub 5). Ausserdem ruft der Port den Schritt nur in den Subs 4/5/7/8.
- **RE (PSX.EXE, re15_disasm.py, ghidra1_V2 XREF[24] von FUN_80045630).** NPC-Sub-Tabelle @0x80076ca0 (`table`: [4] 0x80051148,
  [5] 0x80051484, [7] 0x80051908, [8] 0x80051b00, [9] 0x80051cf8, [18] = [22] 0x80052508, [19] 0x80052864). Aufrufe von
  FUN_80045630 aus diesen Subs: Sub 4 @0x800512a8 (Phase 1) / @0x80051304 (Phase 2), Sub 5 @0x80051614 / @0x80051670, Sub 7
  @0x80051a0c, Sub 8 @0x80051c04, **Sub 9 (Drehen) @0x80051e88**, **Kante 0x12/0x16 Phase 1 @0x800525d4**, **Schub 0x13 Phase 1
  @0x80052930**; a0 = 0 (`addu a0,zero,zero` @0x80051e70 / @0x800525bc / @0x80052918 / @0x800512f0 / @0x8005165c), nur Sub 5
  Phase 1 a0 = 1 (`ori a0,zero,0x1` @0x800515fc). Die STAGE-Overlays rufen FUN_80045630 nur aus Typ-Handlern (Wort 0x0C01158C:
  STAGE1 @0x80111da4/@0x8011c4f0, STAGE3 @0x80110884/@0x80116180, STAGE4 @0x80111820/@0x801162dc/@0x801166b4, STAGE5
  @0x801119a0/@0x8011645c/@0x80116834, STAGE2 keinen; STAGE1 @0x8011c4f0 haengt an Spieler-+0x95 == 0x3c) - nicht Teil der
  NPC-Geh-Subs.
- **Lage im Takt.** Sub 4 Phase 2: Test @0x800512d4-308 VOR `jal 0x8001aac4` @0x8005133c (Kurs) und pos_advance @0x80051344;
  Sub 5 Phase 2: @0x80051640-74 VOR @0x800516a8 / @0x800516b0 und vor der Kanten-Sonde @0x800516c4; Sub 7/8: pos_advance
  @0x800519c8 / @0x80051bc0 VOR dem Test @0x800519dc / @0x80051bd4; Sub 9: Kurs @0x80051e44 vor dem Test @0x80051e58; Kante/Schub:
  direkt vor anim_set @0x800525f0 / @0x8005294c. Der Port testete Sub 4/5 Phase 2 NACH Kurs und Vorschub.
- **Was gelesen wird.** *(+0x168) zeigt auf das Bildwort des Bilds, das anim_set POSIERT hat (`sw a2,360(t0)` @0x8001f36c,
  Vorschub erst @0x8001f618-1c), der Anker ist Part[fuss] der beim Zeichnen gebauten Matrizen (`lw a2,392(a1)` +0x188 + fuss*0xac
  @0x8004567c-a8). Beides gehoert zum GEZEICHNETEN Bild - auch wenn der Sub im selben Takt schon +0x94/+0x95 neu gesetzt hat
  (Sub 5 Phase 1 ausgerichtet: `sb zero,148` @0x8005157c / `sb zero,149` @0x8005158c VOR dem Test @0x800515e4; jede Phase 0).
  Der Port las Clip/Bild des Aktors im Takt (nach dem Neusetzen) -> 11C0: Clip 0 Bild 0 zweimal = der Doppelschritt.
- **Aenderung.** wassersprung_r41.c: Schnappschuss je Aktor am Bildanfang (Bank, Clip, Bild, Lage, Gier des gezeichneten Bilds;
  `s_gez`, gerufen aus re15_wasser_r38_bildanfang vor allen KI-Takten), Bildwort-Test und Part-Anker daraus; ohne Bildanfang
  (Riegel, die den Sub direkt rufen) wie bisher der Aktorzustand. Haken (je 1 Zeile, "Runde 41 Spur A"): enemy_ai_common.c
  re15_npc_sub_walk (Phase 2: Sub 4/5 VOR Kurs/Vorschub, Sub 7/8 NACH dem Vorschub), re15_npc_sub_turn (Sub 9 zwischen Kurs und
  anim), npckante_r38.c sub_kante Phase 1, szenenleon_r36.c re15_szl_schub_npc Phase 1 (+ je ein Include). Kopf (d) in
  include/re15_wassersprung_r41.h.
- **Riegel** `unit_r41_wasser20b0_npc_subs` (ROOM2000-Wasserfleck, Typ 0x40 OWN-Bank): Sub 9 aus Phase 0 -> 1 Schritt aus dem
  gezeichneten Bild 7, Anker = Part 7 der gezeichneten Pose (Gier 0, nach dem Kurs 4000; (-4828,-1886,-4855) beide); Kante 0x12
  und Schub 0x13 Phase 1 je 1 Schritt, Ton 15; Sub 5 aus dem Stand: Takt 1 kein Schritt, Takt 2 genau einer, Ton 15; Sub 4 Phase 2
  am Fleckrand z -9750 (dahinter 0x1d): Zweig boden, Ton 15 = Boden VOR dem Vorschub.
  **Gegenprobe** (Engine-Dateien auf e6e6869b zurueckgesetzt, nur der Riegel neu): `npc_subs: 9 Pruefungen, 6 Fehler` - Sub 9
  `schritte 0`, Anker (0,0,0), Kante `schritte 0`, Schub `schritte 0`, Sub 5 `Takt 1 1 Schritte ... Takt 2 gesamt 2`, Sub 4
  `Zweig boden 0, Ton 21` (trocken nach dem Vorschub). Mit dem neuen Stand `npc_subs: 9 Pruefungen, 0 Fehler`.
  npc_wasser unveraendert: 0x40/0x42/0x45/0x49/0x4d je 6 Schritte (Plaetze 6), 0x4b 9 (PORT-WAHL), Ton 15.

### N1.10b Fortsetzung 2026-10-24: der NPC-Stempel +0x88 ist ein Pro-Bild-Wert (eigener Befund)
- **Befund (Code + Riegel-Gegenprobe).** Der Port-AOT-Scan (aot_common.c, WATER-Schleife) SCHREIBT +0x88 nur; geloescht wurde er
  nur fuer den Spieler (wasser_r38.c @0x80031da4). Ein NPC, der einmal durch einen WATER-Satz ging, blieb fuer den Rest des Raums
  gestempelt und haette auf jedem trockenen Boden den Stempel-Zweig 0x06032000 gespritzt (Riegel-Gegenprobe ohne Loeschen:
  `Takt B ausserhalb: 1 Schritt, Zweig trocken (stempel 1, boden 0), +0x88 = -900` = FAIL; `A_belege/nb1c/gegenproben.txt`).
- **RE (PSX.EXE, re15_disasm.py, am 2026-10-24 nachgeprueft).** Entity-Takt FUN_8001a50c: `jalr v0` @0x8001a588 (Typ-Handler),
  danach `lw v0,-14460(v0)` @0x8001a594 (g_entity) / `sh zero,136(v0)` @0x8001a59c - +0x88 wird je Entity NACH ihrem Handler
  geloescht; der AUTO-Pass FUN_800436a8 (@0x8001ce1c) stempelt neu, wer im Satz steht. Der Wert, den ein Schritt liest, ist also
  immer der des letzten AUTO-Passes.
- **Aenderung.** wassersprung_r41.c re15_wassersprung_r41_bildanfang: `e->water_y = 0` fuer jeden aktiven Nicht-Spieler-Aktor.
  Lage: der Port-Scan laeuft im Schritt NACH dem Bildanfang und VOR den KI-Takten (game_step_common.c) - Loeschen am Bildanfang,
  Stempeln im Scan, Lesen im KI-Takt ergibt dieselbe Folge wie Handler -> Loeschen -> AUTO-Pass im Original. Leser von +0x88 eines
  Nicht-Spielers (grep water_y in engine/src, include, platform/pc): nur die NPC-Schritte (wassersprung_r41.c); aot_common.c
  schreibt, wasser_r38.c behandelt den Spieler. Die RE2-Spinne fuehrt ihren eigenen Stempel +0x10C (re2s_water_stamp, eigener
  Scan); `unit_re2_spider_water_2090` prueft water_y direkt nach re15_aot_scan und bleibt gruen (Unit-Gruppe N1.12).
- **Riegel** `unit_r41_wasser20b0_stempel` (erweitert): Takt A im WATER-Satz 5 von ROOM20B0 (p0 -900, Bild ohne Marke) -> +0x88 =
  -900; Takt B derselbe NPC bei (-26200,-5400) ausserhalb aller WATER-Saetze, trockener Boden, gezeichnetes Bild 7 (Marke) -> 1
  Schritt, Zweig trocken, +0x88 = 0. Mit dem Loeschen `stempel: 7 Pruefungen, 0 Fehler`, ohne `1 Fehler` (s.o.).

### N1.10c Dritte Fortsetzung 2026-10-24: RBJ-Marker-Binder bei RE15_ACTOR_MAX 36 (eigener Befund, Regression DIESES Diffs)
- **Messung vorher (Stand d77bf72a, gebaut: `ninja: no work to do.`).** Die Unit-Gruppe der Vorgaengerin (128 Tests) enthielt nicht
  alle Tests der beruehrten Module. Nachgezogen: (a) `git grep -l` der Stammnamen szenenleon / npckante / climb_common in
  re15_port/tests -> test_r39_heliport1171.c, test_r36_kartenbits.c, test_r38_werkstatt11b0.c, test_r39_stimmen.c,
  probe_r20_birkin_push.c (21 ctest-Eintraege); (b) ALLE ctest-Eintraege ausser integration_*, deren Quelle RE15_ACTOR_MAX nennt
  (Abbildung Quelle -> ctest ueber `ctest --show-only=json-v1`, 383 Eintraege, Liste `--tests-from-file`). Ergebnis (a)
  `95% tests passed, 1 tests failed out of 21`, (b) `99% tests passed, 1 tests failed out of 383` - in beiden derselbe eine Test,
  `unit_r39_heliport1171_binder`, woertlich:
  ```
  FEHLER: John nach 34 abgefragten Slots ohne Record: RBJ rec1 (-1 Clips)
  FEHLER: Skelett-Kanal +0x180 (EMR rec1) ebenfalls gebunden
  FEHLER: ROOM10D0: Marvin (Aktor 1) nach den Zombies an rec1 gebunden (-1 Clips, Marker 0x2)
  ```
- **Ursache.** enemy_common.c rbj_resolve_slot prueft `rbj_record_marker(r) & (1u << mslot)`. Der Riegel belegt ALLE Slots 2 ..
  RE15_ACTOR_MAX-1 mit Kraehen ohne Record; seit RE15_ACTOR_MAX 36 (diese Spur, Entscheid E2) also auch 32..35. `1u << 32..35`
  ist in C undefiniert, x86 `shl` maskiert die Weite auf 5 Bit -> Bits 0..3: die Kraehe in Slot 32 trifft rec0 (Marker 0x1),
  die in Slot 33 rec1 (Marker 0x2); beide belegen die zwei Cache-Plaetze (RE15_RBJ_CACHE_MAX 2) vor John -> John ohne RBJ-Kanal.
  Dieselbe Fehlerklasse wie M7 (s_dog_noclamp_mask); weder die Abnahme noch die Vorgaengerin hatten sie hier gefunden.
- **Beleg (PSX.EXE, re15_disasm.py `dis 0x8001b3f8 80`, 2026-10-24).** FUN_8001b3f8: `lw a0,0(v1)` @0x8001b438 = Markerwort
  (u32); Spieler-Bit `andi v0,a0,0x1` @0x8001b440 / `srl a0,a0,1` @0x8001b448; Gegner-Schleife `andi v0,a0,0x1` @0x8001b470 /
  `srl a0,a0,1` @0x8001b478, Index `andi v0,a3,0xff` @0x8001b47c, Stride (sll 5 / subu / sll 2 / addu / sll 2) = i * 0x1F4
  @0x8001b480-90, Ziel +0x180 `sw t1,0(at)` @0x8001b4a0 (0x800acdac) / +0x184 `sw v0,0(at)` @0x8001b4bc (0x800acdb0), Ende
  `bne a0,zero,0x8001b470` @0x8001b4c0 mit `addiu a3,a3,1` @0x8001b4c4 im Verzoegerungsplatz. Das Markerwort hat 32 Bit, die
  Schleife endet, wenn es leer geschoben ist: Bits 1..31 = Gegner 0..30. Ein Gegner-Slot >= 32 hat im Original KEIN Markerbit
  und wird nie gebunden.
- **Aenderung** (enemy_common.c, gemeinsame Datei ohne Eigentuemer in VERTRAG §1.3, 1-Zeilen-Haken "Runde 41 Spur A"):
  `if (mslot < 32 && (rbj_record_marker(r) & (1u << mslot)))` + Kommentar mit den Adressen oben.
- **Nebenbefund (nicht geaendert, Hinweis an den Orchestrator).** a3 (Gegner-Index) wird in FUN_8001b3f8 nur EINMAL auf 0 gesetzt
  (`addu a3,zero,zero` @0x8001b410, Verzoegerungsplatz von `beq` @0x8001b40c) und in der Record-Schleife (@0x8001b42c ..
  `bne` @0x8001b4d4) nicht zurueckgesetzt: im Original zaehlt der Gegner-Index von Record r+1 hinter den geschobenen Bits von
  Record r weiter; der Port wertet jeden Record ab Bit 1 aus. Ob ein ausgeliefertes RBJ davon betroffen ist (mehrere Records mit
  Gegner-Bits), ist NICHT gemessen -> OFFEN 9.

### N1.10d Dritte Fortsetzung 2026-10-24: PORT-WAHL nur fuer Fortbewegungs-Clips, Ableitung ohne Anim-Spur (eigener Befund)
- **Messung vorher (Stand d77bf72a).** Integrationskette (N1.12) `94% tests passed, 1 tests failed out of 17`: rot nur
  `integration_r37_roy_leon1170` - `FEHLER: Elliot Clip 2 = 52 Bilder (EM047-Bank 1 CDEMD0) bzw. INIT-Pose Loco Clip 2 Bild 0
  (@0x8011d978): 6/56 Zeilen, davon INIT-Pose 6`. Die 50 fremden Zeilen der Anim-Spur stehen alle im Bild 890, Aktor-Index
  1218860271 (kein g_actors-Platz), Typ 0x47, Clip 2, cur 0..49 (`A_belege/nb1d/roy_leon1170_rot.txt`): Elliot steht bis Bild
  889 in der INIT-Pose (LOCO Clip 2 Bild 0), im ersten Sub-Takt las der NPC-Schritt dieses gezeichnete Bild, Clip 2 traegt keine
  Marke -> die PORT-WAHL-Ableitung lief ueber alle 50 Bilder des Clips - mit re15_compute_actor_kf an einem Stapel-Aktor, und
  das schreibt je Aufruf eine RE15_ANIM_TRACE-Zeile.
- **Zwei Defekte.** (1) Die PORT-WAHL griff fuer JEDEN Clip ohne Marke (Code: `else if (!clip_hat_marken(...))`), obwohl das
  Dossier (N1.3) und der Riegel "PORT-WAHL nur 0x4b" sagen: auch INIT-Pose, Kniebeuge (Container c5), Gesten. (2) Die Ableitung
  schrieb in die Anim-Spur (Mess-Artefakt, aber es macht jeden Spur-Riegel einer NPC-Szene rot).
- **Beleg - Daten.** Sonde `A_belege/nb1d/sonde_npc_marken.c` -> `npc_marken.txt` (Bildwort-Bit 0x4000 je Bank und Clip, ALLE
  NPC-Typen der Archive: 0x40/0x42/0x45/0x47/0x49/0x4b/0x4d, CDEMD0 und CDEMD1): Marken tragen ausschliesslich OWN c0 (22 Bilder)
  + c5 (30) und LOCO/Container c0 (34, Bilder 8/25) + c1 (32, Bilder 7/22). 0x4b: keine Marke in keiner Bank. **0x47 aus CDEMD1
  (Roy, re15_typ47_ist_elliot nur fuer Leon): OWN c5 (61 Bilder) OHNE Marke, c0 markiert 0/12** - in Nachbesserung 1 noch nicht
  gezaehlt (die Sonde der Vorgaengerin kannte nur sechs Typen).
- **Beleg - Code (PSX.EXE, re15_disasm.py).** Jeder Schritt der NPC-Subs haengt am Bit 0x4000 des gezeichneten Bildworts
  (`andi v0,v1,0x4000` @0x80051288 Sub 4, @0x800519ec Sub 7 usw., N1.8/N1.10). Die Rueckwaerts-Subs posieren das LOCO-Paar:
  Sub 7 `sb v1,148(v0)` (+0x94 = 1) @0x8005196c, `lw a0,132(v0)` / `lw a1,364(v0)` @0x80051a20-24, `jal 0x8001f314` @0x80051a28;
  Sub 8 `lw a0,132(v0)` / `lw a1,364(v0)` @0x80051c18-1c, `jal 0x8001f314` @0x80051c20 (Kommentar im Bau-Stand "Subs {4,5,7,8}
  laden +0x170/+0x174" war fuer 7/8 falsch; der Port zeichnet dort den Container, dessen Clip 1 dieselben Marken 7/22 traegt).
- **Aenderung** (wassersprung_r41.c, Kopf (e) in include/re15_wassersprung_r41.h): `fortbewegungs_clip()` = OWN c0/c5 bzw.
  LOCO/Container c0/c1 (genau die Clips mit Marken laut Sonde); PORT-WAHL nur fuer einen Fortbewegungs-Clip ohne Marke (0x4b alle,
  0x47 CDEMD1 OWN c5), jeder andere Clip ohne Marke gibt wie im Original keinen Schritt (npc_takt und re15_wassersprung_r41_aufsetzer).
  Ableitung: Bild -> Keyframe wie re15_compute_actor_kf vorwaerts, aber ohne dessen Spur-Zeile; 0x8000-Zwischenbilder wie dort
  (A/B/Gewicht, FUN_8001f314 @0x8001f370-f37c -> FUN_8001f8b4; Roys c5 traegt sie in den Bildern 9/25/41/57), g_anim_kf_tween und
  g_anim_pose_actor gesichert (Muster affen_11c0.c). Regel-Hysterese "nach der Schwungphase": ein Aufsetzer zaehlt erst, nachdem der
  Fuss seit dem letzten ueber der halben Spanne war; zwei Umlaeufe, gezaehlt im zweiten. Gemessen an Roys c5 (`roy_c5.txt`): Teil 4
  steht in den Bildern 22..50 bei y -224..-230 um die Zehntel-Schwelle -226 - ohne Hysterese 4 Aufsetzer (19/45 + 2), mit ihr 2:
  **Bild 19 (Teil 4, Fuss 7) und Bild 50 (Teil 7, Fuss 4)** = je ein Aufsetzen pro Fuss im 61-Bild-Zyklus (Tiefstlagen -213 bei 21,
  -214 bei 52). Die Ergebnisse aller uebrigen Typen bleiben gleich (0x4b c5 5/19, c0 0/10; Regel 0x40-Familie 7/20, 0x42-Familie
  5/18; npc_wasser 6 bzw. 9 Schritte).
- **Riegel** `unit_r41_wasser20b0_npc` (erweitert): je Typ und Archiv Container-Clip 5 -> 0 Aufsetzer / keine PORT-WAHL, LOCO-Clip 2
  (INIT-Pose) -> 0/0, Container-Clip 1 (Rueckwaerts) -> Marken 7/22 bzw. PORT-WAHL fuer 0x4b; Typ 0x47: CDEMD0 Clip 5 Marken 7/22
  Fuss 7/4, CDEMD1 Clip 5 PORT-WAHL 19/50 Fuss 7/4, Clip 0 Marken 0/12, LOCO-Clip 2 0/0. **Gegenprobe** (alter Schritt-Code d1809dbf,
  neuer Riegel; `A_belege/nb1d/gegenprobe_portwahl.txt`): `npc: 90 Pruefungen, 27 Fehler` (Container-Clip 5 und INIT-Pose je 2..4
  erfundene Aufsetzer bei jedem Typ, Roy 19/45 Fuss 7/7); neuer Code `npc: 90 Pruefungen, 0 Fehler`.
