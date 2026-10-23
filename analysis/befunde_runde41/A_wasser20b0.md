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
