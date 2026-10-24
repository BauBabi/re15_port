# Runde 41 — Spur G "kanister2080" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, Basis `2adca3bf` (= r41/integration 9f10bdb0
master + A D I + B, plus AUFTRAG/VERTRAG Stand Nachtrag 16 / FG-E1).

Zuteilung (VERTRAG 1.1/1.2, FG-E1): Bits 114-119, Nachrichten-Ids 2080/2081 42-47 (42 = "Hey, Leon/Elza, try with this!").
G besitzt ROOM2080/2081; F besitzt 2090-Szene (Bit 110 = Szene gesehen). Kanister im 2090-Wasser ueber B's API
(`re15_gr2_kanister_setzen`), Treffer-Zaehler aus B's API.

Punkte (Wortlaut AUFTRAG.md):
1. Nach dem Start der Alligator-Fress-Szene: in 2080 oben Metalltuer-Schliess-SE, dann 8 gespawnte Zombies im Hauptbereich.
2. Gaskanister (RE2) am Rand der Reling in 2080 (Gas_Can.bmp).
3. Nach genug Treffern auf den Alligator: Szene — Tuer-SE, Marvin (Leon)/John (Elza) in 2080, 8 Kopfschuesse, Kanister
   aufnehmen (beide Haende), "Hey, Leon, try with this!", Wurf ueber die Reling (Kamera wie Adas Raketenwerfer-Wurf RE2,
   Nachtrag 5), Kanister in 2090 im Wasser, Alligator nimmt ihn auf (B), Schuss -> RE2-Tod (B).

## 0. Stand / Fortschritt
- 2026-10-24: Pflichtlektuere (AUFTRAG/VERTRAG/UEBERGABE, B-Dossier §2.5/§7/§9), Raum-RE 2080/2090, RE2-Zensus Raketenwerfer.

## 1. RE-Belege (selbst gelesen)

### 1.1 ROOM2080/2081 (RDT = shared_assets/PSX/STAGE2/ROOM2080.RDT, 173608 B; 2081 SCD bytegleich bis auf den Namen)
- Kameras (RID @0x60, 32 B je Cut, nCut 9): Cut 5 pos (-12826,-18695,-18043) -> (-5172,-15001,-17158) (Balkon, Blick
  nach Osten auf "SEPTIC POOL"); Cut 6 (-7315,-17332,-17514) -> (449,-10461,-16726) (vom Balkon hinab in die Grube);
  Cut 7 (8095,-18819,-9867) -> (-4415,-11874,-13977) (Totale von Osten: Grube unten, Laufgang/Kontrollraum oben);
  Cut 8 flag 1. Zonen (RVD @0x188): Laufgang-Gruppe Cut 2/3/4 x -17500..9500, Cut 5/6/8 x -11000..9500 (= dieselben
  Vierecke wie die Zonen von ROOM2090 @0x220) -> 2080 und 2090 teilen x/z.
- SCA @0x4BC (5 x 28 Saetze): Band 8 (floor-Byte 0x88) = Waende des oberen Laufgangs x -14300..-9200 / z -26900..-6750;
  Balkon nach Osten: Waende @0x588 (x -9200..-5700, z -16550..-15550), @0x594 (x -9200..-5700, z -19600..-18600),
  @0x5A0 (x -6700..-5700, z -19600..-15550 = Reling am Balkonende); Band 7 (0x78, u1 1): @0x510 Laufgang-Rechteck,
  @0x528 Balkon x -9200..-6700 z -18600..-16550; Band 5 (0x55/0x45): Treppenabsatz an der 2030-Tuer.
- Tueren (main00): Slot 0 @0x0BA2 -> ROOM2030 (Band 5, rect (-24400,-17700,1000,2000)); Slot 1 @0x0BC2 -> ROOM2090
  (Band 8, rect (-9650,-26150,1000,1000), Ziel (-8200,0,-25550) Cut 0 = Tuernische in 2090; DOOR16 V5 "hinab").
- sub00 @0x0BFE: (3,245)=0 -> Wasser-Objekt obj 0 Typ 3 Band 9 bei y -10800 + Tuer 2090 gesperrt (Aot_reset Slot 1);
  sonst bei y -3420 (@0x0C5E). sub02 @0x0C86 = Rechner (Cut 7 + Wasser senken, Cut 3, Cut_auto).
- Hintergruende: BSS/ROOM2080/BG00..BG09.BSS; Bildvergleich Gas_Can.bmp gegen extracted/PSX/STAGE2/ROOM208/ROOM20800..09.bmp
  (Hauptbaum): ROOM20805 mittlere Abweichung 0,00 und 100 % gleiche Pixel ausserhalb der roten Marke (alle anderen
  >= 54,7) -> **Gas_Can.bmp = Cut 5**. Rote Marke: x 159..175, y 149..163, Mitte (167,156) = Balkonspitze vor der Reling.

### 1.2 RE2-Vorbild des Wurfs (Nachtrag 5): ROOM7020 (info/re2leon/PL0/RDT/ROOM7020.RDT)
- Zensus aller 495 RE2-Leon-RDTs (re2_scd_walk.py, Item_aot_set 0x4E / _4p 0x69, Item 0x11 = Raketenwerfer): genau
  ROOM7020 SCD-main @0x00DA `4e 05 02 31 00 00 a8 99 60 a5 48 0d c4 09 11 00 02 00 28 00 00 01` (Aufnahme-AOT 5
  x -26200 z -23200 3400x2500) und ROOMD090 (Sondermodus, nicht Leons Szenario).
- sub00 @0x00F8: Ck(4,0x97)=0 -> Sce_em_set Ada `44 00 ff 41 00 02 06 00 00 ff a0 92 c8 ce 14 ba 40 06 ..` (Modell 0x41,
  (-28000,-12600,-17900) = obere Ebene), Tyrant `44 00 00 2b ..` .
- Ausloeser: sub10 @0x06AC `Sleep 0x0708` (1800 Bilder) -> Set(5,15); sub11 @0x06B6: Work_set(3,0) + Member_cmp(0x19==1)
  + Member_cmp(0x1a==1) -> Set(5,13) + Evt_exec sub12, ODER Ck(5,15)=1 + Member_cmp(0x1a==1) + Ck(5,13)=0 -> dito.
- sub12 @0x06F2 (der Wurf): Set(2,7,1) Set(1,27,1) @0x06F2/6; Lagen sichern @0x0706-0725; **Cut_chg 7 @0x0726**;
  Work_set(4,0) + Super_set `48 00 81 8a 00 00 58 02 ce ff 80 fc 00 00 00 04` @0x072C (Werfer an Ada);
  Ada Work_set(2,0) + Pos_set (-28000,-7500,-17900) @0x0740 + Dir_set 0x0e74 @0x0748 + Plc_motion `3f 0a 12 40` @0x0750,
  Sleep 1, Plc_dest `40 00 14 ..` @0x0758, **Xa_on 0 + Message_on 5 @0x0760/0x0764**, **Sleep 30 @0x076A**, Pos/Dir +
  Plc_motion erneut @0x0772-0782, **Sleep 20 @0x0786**, Work_set(4,0) + **Super_reset** `49 00 40 fb 40 02 00 04` @0x078E
  (loslassen), Evt_exec sub13 @0x0796, warten auf (5,0x0e) @0x079C-07A4, **Sleep 15 @0x07A8**, Leon Pos_set
  (-18600,0,-21700) @0x07B0, Tyrant Pos_set (-8000,0,-25000) @0x07C4, **Cut_chg 11 @0x07D4**, Xa_on 1 + Leons Zeile
  @0x07E2-07F4, Ada Plc_dest weg @0x0812, Wsleep, **Cut_chg 0 @0x0820**, Aot_reset 5 (Aufnahme frei) @0x0822, Gosub
  sub18 (Plc_ret, Set(2,7,0), Set(1,27,0), Cut_auto 1 @0x0A2C-0A38).
- sub13 @0x088A (der Fall): Work_set(4,0), Speed_set 2/1/0 = 10/20/25 @0x088E-0896, 8/7/6 = 0/20/5 @0x089A-08A2,
  Do { Evt_next, Add_speed, Add_aspeed } while Member_cmp(0x0c op 3 -800) @0x08A6-08B0, Set(5,0x0e) @0x08B6,
  **Se_on Satz 0x0e @0x08BA** (Aufprall), Pos_set (-24650,0,-21998) + Dir_set (0xfc00,0,0x0400) @0x08CA/08D2 (liegt).



### 1.3 RE2-Opcode-Semantik (selbst disassembliert, re2_disasm.py auf info/re2leon/PSX.EXE; Tafel @0x800a74c8)
- Speed_set (0x2F) @0x80055a84: `lbu v1,1(v0)` / `lhu a1,2(v0)` / `sh a1,344(v1)` @0x80055a8c/90/ac -> Faden+0x158+Achse*2
  (Achse 0..5 = Geschwindigkeit x,y,z,rx,ry,rz; 6..11 = Beschleunigung) - wie RE1.5 (scd_vm.c op_speed_set).
- Add_speed (0x30) @0x80055ab0: Objekt = Faden+0x154; +0x38/+0x3C/+0x40 += v[0..2] (@0x80055abc-e0), +0x74/+0x76/+0x78
  += v[3..5] (@0x80055aec-b10) - reine Welt-Addition, keine Drehung.
- Add_aspeed (0x31) @0x80055b2c: v[i] += a[i], i = 0..5 (+0x158.. += +0x164.., @0x80055b2c-b90).
- Work_set (0x2E) @0x80055904: `sw zero,344..364(a0)` = v und a geloescht.
- Super_reset (0x49) @0x800558b0: +0x80 = 0x8009db44 (Elternmatrix = Einheit, losgeloest), Lage +0x38.. = Weltlage
  +0x68/+0x6C/+0x70 (@0x800558b8-d8), Drehung = pc[2..7] (@0x800558dc-f4) -> der Gegenstand faellt ab der Handlage.
- Member_cmp (0x3E) @0x80055e8c: Mitglied = pc[2], Op = pc[3] (Tafel @0x800112d8), Wert = (s16)pc[4..5]; Mitglied 0x0c =
  `lw v0,60(a0)` @0x80056008 = y; Op 3 @0x80055f10 `slt v0,v1,s1` = y < Wert. -> ROOM7020 sub13 faellt, solange y < -800.
- Tuer-zu-Ton: @0x800141f0 `jal 0x8005ba28` mit `lui a0,0x1` im Verzoegerungsplatz @0x800141f4 = Se_on Tuerbank Satz 1
  (Door_exit; dieselbe Lesart wie Runde 36 J re15_szenenleon_r36.c, Tonteil DOOR1A Satz 1).

### 1.4 RE2-Vorbild "Kanister faellt ins Wasser": ROOM40A0 sub10 @SCD-main 0x0656
Work_set(4,1) (obj 1 = Kanister), Set(5,1,1) @0x0659, Fall: v_y -8, a[5] -4, v_x -40 @0x066A-0672, For 23 {Add_speed,
Add_aspeed, Evt_next} @0x0676; **Se_on Satz 0x0b `36 02 0b 01 04 01 ..` @0x0686 (Aufschlag im Wasser)**; Nachlauf
(Treiben/Absetzen) @0x0692-0x06FC; Set(5,0,1) "liegt im Wasser" @0x0712.

### 1.5 Lage des Kanisters (Rueckprojektion, tools/r41_kanister2080/probe_kanister_lage.c)
Projektion wie der Zeichner (re15_camera_build_view, Bild = 160 + H*vx/vz, 120 + H*vy/vz). Markenmitte (167,156) auf die
Balkon-Bodenebene y -14400 (Band 8) -> **Welt (-6745, -14400, -17625)**; Markenecken (159..175, 149..163) -> x -7185..-6258,
z -17933..-17285 (Unterkante-Mitte (-7154,-17660)). Liegt im Balkon (SCA @0x528: x -9200..-6700, z -18600..-16550; Reling-
Zelle @0x5A0 ab x -6700). Stehend = Drehung (0,0,0) wie RE2 ROOM40A0 @0x0452 `2d 01 .. b8 ac f8 f8 cc db 00 00 00 00 00 00`
(Obj_model_set Modell 1 ohne Drehung); MD1-Ausdehnung y -2784..0, r 267 -> Fuss auf dem Boden. Sichtbar (Fuss UND Kopf im Bild)
in 2080 Cut 1, **Cut 5 (Fuss (168,155), Kopf (170,77))**, Cut 7 (Fuss (126,73), Kopf (123,35)); in KEINER 2090-Kamera.

### 1.6 Zone-7-Tot-Bits (Sce_em_set pc[7], re15_em_status_zone Stage 1-3 = 7, Gate @0x80042128-38)
Zensus scd_dump_room.py ueber alle RDTs STAGE1-3 (Sce_em_set pc[7] + Ck/Set Bank 7): 176 belegt; frei u.a. 100-105, 109-112,
117-119, 124-133, 144, 145, 154, 159-163, 172, 174, 175, 180, 186, 187, 192, 193, 196, 197, 206-208, 216, 219, 220, 222,
223, **236-245**, 253-255. Port belegt davon 40-43, 46-49, 75-79, 83, 84, 90, 91, 95 (irons_tod_1150.c), 124-131, 133, 144,
145 (elzazombies_r39.c), 172 (fenster1120), 174 (hund1121_r39). -> Spur G: **236..243** (8 Zombies 2080/2081).

### 1.7 Original-Form "NPC schiesst" (RE1.5 ROOM1021 sub02, John 0x49 = Sce_em_set @0x01DF6)
Work_set(2,0) @0x020C2; **Plc_motion(2,6) Zielen @0x020C6**, Sleep 110 @0x020CA, **Plc_motion(2,7) Feuer @0x020CE**, Sleep 30
@0x020D2, Plc_motion(2,7) @0x020D6; Senken Plc_motion(2,6) + Plc_flg 0x80 @0x020EA/@0x020EE, Sleep 20 @0x020F2. Marvin (0x40)
traegt dieselbe Eigenbank (Sub 2 = 0x80050f00 auf +0x170/+0x174, ROOM10D0 Plc_motion(2,6,0), enemy_ai_common.c:10069).

## 2. Messung vorher (Stand 2adca3bf, Bau local_build.sh, exe-Kopie re15_pc_g41_mess.exe)
Lauf vorher1: `RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_FORCE_CUT=5 RE15_FRAMEDUMP=20-60/20 RE15_SPAWN_DIAG=1
RE15_EXIT_AT=60#2080` (Titel-Autostart wie die r41-Riegel).
- Punkt 1: debug.log 0 Zeilen `[spawn-diag]` in ROOM2080 (kein Gegner), kein Tuer-Ton (Bit 110 gesetzt, 2080 aendert sich nicht).
- Punkt 2: Cut 5 Bild 40 (G_belege/vorher_2080_cut5_f40_x2.png, 2x vergroessert): Balkon leer, kein Kanister.
- Punkt 3: kein Ausloeser, keine Szene (kein Code ausser B's Testhaken RE15_GATOR_KANISTER).

### 1.8 Clip-Zensus Marvin 0x40 / John 0x49 (tools/r41_kanister2080/probe_npc_clips.c, Pose = re15_skel_compute_pose)
CDEMD0.EMS Blob 0x40 @0x2CD800 / 0x49 @0x355800, je 9 Verzeichnis-Eintraege; Bank 0 (Loco, dir[1]/dir[2]) 24 Clips, Bank 1
(Eigenbank dir[3]/dir[4], spielt Sub 2/4/5/6/9, re15_emd.h:226-232) 14 Clips; Clip-Struktur und Bewegung 0x40 = 0x49 (diff
ohne Wurzelwerte leer). Armketten 0->9->10->11 / 0->12->13->14 (Haende = Bone 11/14), Kopf = Bone 8; Modell: +x = vorn,
y < 0 = oben. Volltabelle G_belege/npc40_clipzensus.txt. Belegte Lesarten:
- Eigen c6 (10 B.): Haende von der Huefte (-31,-1602) nach vorn/oben (764..917, -2534..-2458), Abstand 242 = beidhaendig
  = **Anheben/Zielen**; Eigen c7 (16 B.) haelt vorn oben = **Feuern** (ROOM1021 sub02 Plc_motion(2,6)/(2,7) @0x020C6/@0x020CE);
  c8 Halten; c9/c10 oben, c11/c12 unten.
- Eigen c0 (22 B.): beide Haende vorn auf Brusthoehe (350..683, -1886..-1821), Abstand 722..1005 = **Tragen vor der Brust**.
- Loco c2 (50 B.): Wurzel sinkt -1813 -> -1005 (f33), linke Hand am Boden (149,-328), dann zurueck = **Buecken + Aufheben**.
- Eigen c5 (30 B.): Gehen (Haende pendeln).

### 1.9 RE2-Ausloeser der Wurfszene (ROOM7020 sub10/sub11) und die Port-Form der Schwelle
- RE2 loest Adas Wurf aus, wenn der Boss Mitglied 0x19 == 1 UND 0x1a == 1 meldet (sub11 @0x06BE/@0x06C4) oder 60 s
  vergangen sind (sub10 Sleep 0x0708 @0x06AC -> (5,15)) und 0x1a == 1. Mitglied-Getter @0x800112f8: 0x19 = +0x1D8
  (`lh v0,472(a0)` @0x800560a4), 0x1a = +0x1DA (`lh v0,474(a0)` @0x800560b0) = Skript-Arbeitsfelder des Tyrant. Die
  Tyrant-Overlay-Datei liegt nicht in info/re2leon (COMMON/BIN hat nur EM23/EMOVL10/21/EMS25/26/EMZ0) -> wer +0x1D8/+0x1DA
  setzt, ist OFFEN (Abschnitt 6).
- NUTZER-VORGABE "oft genug angeschossen" -> Schwelle = die RE2-Schwaeche des Alligators (HURT-Selektor @0x80101ebc-dc:
  HP < 25000 -> +0x218 |= 0x20, B: re15_gr2_geschwaecht) = Form "Boss meldet Schaden" (+0x1D8); "freie Lage" = Boss in
  Verfolgung/Belagerung (re15_gator_boss_verfolgt), nicht im Biss/Finisher/R1/untergetaucht (Form +0x1DA). Den 60-s-Zweig
  baut der Port NICHT (Nutzer-Wortlaut nennt nur Treffer). Trefferzahl bis zur Schwelle mit der RE1.5-Schadenszeile des
  Alligators (re15_damage.c s_wpn_dmg_gator, Zeile @0x8006e0d0 + 0x23*0x58): Pistole 7 -> 43 Treffer, Schrotflinte 30 -> 11,
  Magnum 50 -> 7, Granatwerfer 70 -> 5 (HP 25300, Sockel 25000) -> Entscheidungsfrage Abschnitt 7.

## 3. Umsetzung

### 3.1 Dateien (alle neu ausser den markierten Haken)
- `include/re15_kanister2080_r41.h` - alle Konstanten mit Adresse/Herkunft.
- `engine/src/kanister2080_r41.c` - Installer (2080/2081/2090/2091), Programme (Original-Opcode-Formen, zur Laufzeit gebaut
  wie elzazombies_r39.c), Bildtakt (Signale Bank 5 Wort 0, Anvisieren, Kopfschuss, Kanister in den Haenden, RE2-Wurfbahn),
  Zeichner-Abfrage, Messhaken RE15_K2080_GATOR_HP.
- `platform/pc/src/kanister2080_pc.c` - Kanister-Zeichner (Modell/TIM-Platz 53 wie Spur B), Tuer-Ton (DOOR06-Tonteil ->
  RE2-Tuerbank Satz 1), Aufschlag (KANISTER2080.VBS), NPC-Schuss (re15_elza_se_hook RE15_ELZA_TON_SCHUSS).
- `engine/src/gen/kanister2080_r41_texte.inc` (tools/r41_kanister2080/texte_bauen.py), `engine/src/gen/r41_kanister2080_bank.inc`
  + NEUES ASSET `shared_assets/RE2/KANISTER2080.VBS` (9864 B, md5 848c40f6c077d937fb025df9ae1bc62b;
  tools/r41_kanister2080/kanister2080_se_cut.py = RE2 ROOM40A0 snd0 EDT-Satz 11 `00 01 83 00`, 1 VAG 6720 B).
- Haken (je 1 Zeile, "Runde 41 Spur G"): `scd_room_setup.c` Ende des Installer-Blocks; `scd_vm.c` Bildtakt nach
  re15_elzazombies_r39_tick; `platform/pc/main.c` Init nach re15_pc_gator_re2_init + Zeichnen nach re15_gr2_pc_zeichnen;
  `tests/unit/test_r36_kartenbits.c` k_port[] Bits 114/115/116 (+ include).
- Werkzeuge/Sonden: tools/r41_kanister2080/probe_kanister_lage.c, probe_lagen.c, probe_npc_clips.c.

### 3.2 Lagen (PORT-WAHL mit Messung; probe_lagen.c auf ROOM2080.RDT)
- Kanister (-6745,-14400,-17625) = Rueckprojektion der Markenmitte (1.5); in Cut 5 Fuss (168,155) Kopf (170,77) = Markenmitte.
- 8 Zombies Band 8 zwischen Treppenabsatz und Balkon (Tabelle k_zombies, je >= 700 von Band-8-Waenden, paarweise >= 1393, in
  Cut 2 und Cut 7 mit Fuss UND Kopf im Bild), Blick auf den Treppenabsatz (Gier = atan2(-dz,dx)).
- Marvin/John Start (-14800,-16500) Gier 0 (Treppenabsatz, in Cut 2 Bild (164,86..115)); Aufheben-Stand (-7245,-17625), RUN-Ziel
  250 dahinter (Ankunft d < 300 @0x80051764); Wurf-Stand (-7100,-17625) per WALK (d < 100 @0x800513f8): Reling-Zelle @0x5A0 ab
  x -6700, Radius 400. Relingstange: Bodenlinie x -6350 (Pfosten in Gas_Can.bmp (139,151)/(200,151) rueckprojiziert),
  Oberkante ~ -15443 (31 px bei Tiefe ~7000, H 208).
- Spieler-Parkplatz waehrend des 2080-Teils: (-23200,-9000,-16700) Band 5 = Eintritt von 2030 (re15_room_spawns.h).

### 3.3 Ablauf (Programm SZENE, zur Laufzeit gebaut; Zeilenform je Opcode im Dateikopf von kanister2080_r41.c)
| Schritt | Port | Vorbild |
|---|---|---|
| 2090: Riegel + Raumwechsel | Set(2,7,1) Set(1,27,1), Door_aot_set Slot 21 -> ROOM2080 Cut 2 Parkplatz, Aot_on | RE2 sub12 @0x06F2/6; Montage irons_tod_1150.c |
| 2080 Abschnitt 1 | Sce_em_set NPC (Slot 8, grid 0x40, Etage 8) + 8 Zombies (Tot-Gate) + Sleep 1 | ROOM2000 @0x017B4 / ROOM2020 @0x008F6 |
| Tuer-Ton | Signal -> DOOR06 Satz 1 | Door_exit @0x800141f0 |
| Anlegen | Work_set(2,8), Plc_motion(2,6), Sleep 110 | ROOM1021 @0x020C2/@0x020C6/@0x020CA |
| je Zombie | Signal ZIEL, Sleep 10 (Nachdrehen 0xC8/Bild), Plc_motion(2,7), Signal SCHUSS, Sleep 30 | ROOM1021 @0x020CE/@0x020D2; Gun-FSM @0x80033f90 |
| Senken | Plc_motion(2,6) + Plc_flg 0x80, Sleep 20 | ROOM1021 @0x020EA/@0x020EE/@0x020F2 |
| Lauf zum Kanister | Cut_chg 5, Plc_dest RUN + Ankunfts-Poll | ROOM1021 @0x020B2; ROOM1150 sub08 @0x0112A |
| Aufheben | Plc_motion(1,2) Loco c2, Sleep 33, Signal AUFHEBEN, Sleep 17 | Zensus 1.8 |
| an die Reling | Plc_dest WALK + Poll, Plc_motion(2,0) Tragen | Zensus 1.8 |
| Wurf (RE2 sub12) | Cut_chg 7, Plc_motion(2,6), Message_on 42, Sleep 30, Plc_motion(2,6), Sleep 20, Signal WURF, Poll GELANDET, Sleep 15 | @0x0726/0750/0764/076A/0782/0786/078E/079C/07A8 |
| Fall (RE2 sub13) | Takt: Lage += v, v += a; v (25,20,10), a (5,20,0) im Blickrahmen; Ende y >= Wasser - 800; Aufschlag-Ton | @0x088E-08B0, @0x80055ab0/@0x80055b2c, ROOM40A0 @0x0686 |
| Rueckkehr | Door_aot_set -> 2090 (Spielerlage/Cut beim Ausloesen), Aot_on | RE2 Cut_chg 11 @0x07D4 (Schnitt auf den Fangenden) |
| 2090 Ende | Sleep 30, Work_set(1,0)+Plc_ret, Set(2,7,0) Set(1,27,0), Cut_auto 1; Bits 115/116; Kanister ins Becken (B-API) | RE2 sub18 @0x0A2C-0A38 |

Nicht uebernommen (benannt): RE2 sub12 `Sleep 1` + `Plc_dest 40 00 14 ..` @0x0754/@0x0758 (Adas Mode 0x14, kein
Port-Gegenstueck); Adas Abgang Plc_dest @0x0812 (Marvin/John bleibt in 2080, der Schnitt geht nach 2090).
- Kopfschuss: re15_player_weapon_fire(7) mit Lage/Gier des NPC (Spielerfelder kurz getauscht und zurueckgesetzt) = derselbe
  Anwender wie ein Spielerschuss: Kritklasse w7 (@0x80012380-88) -> hp = -1 fuer Typ < 0x20 (@0x800124fc-51c), RE2-Zeile 5 ->
  FUN_801092C4 (Kopf ab, Part 8 |= 0x4A @0x80109460-98). Das Original hat keinen NPC-Schaden (John trifft Roy in ROOM1021 nur
  per Skript) -> PORT-WAHL der Schuetzenlage; Form der Wirkung = RE2.
- Kanister in den Haenden: Mitte der Hand-Bones 11/14 der gezeichneten Pose (NPC-Bank Eigen/Loco, re15_skel_compute_pose),
  quer (RotY(Gier)*RotX(1024)), Mitte der Laenge in den Haenden. Fall auf der Kanister-Mitte; MD1-Ursprung = Mitte - M*(0,-1392,0).
- Spielstand mit Bit 116 ohne Laufzeit-Landepunkt: dieselbe Bahn aus der Nennlage (Wurf-Stand + Eigen-c6-Ende (840,-2496)).
- Zombies im 2080-Teil: KEIN Halt (entity+0x9 |= 0x20) - gemessen (Lauf p3f): der angehaltene RE2-Steher spielte Clip 1 bis zum
  Ende und lag ab Bild 40 am Boden; ohne Halt bleiben die Steher stehen (Spieler-Parkplatz Band 5, >= 10000 entfernt).

## 4. Messung nachher (exe-Kopie re15_pc_g41_mess.exe, Bau 2026-10-24)
Grundumgebung: RE15_NO_INTRO=1 RE15_NOAUDIO=1 Titel-Autostart, RE15_WINDOW_SCALE=1.
- P1 (Lauf p1a) `RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_PLAYER_POS=-15000,-16500,0,8 RE15_FORCE_CUT=2`:
  `[k2080] F250 ROOM2080 nach Bit 110: 8 lebende Zombies (Tot-Bits 236..243)`, `Tuer-Ton DOOR06 Satz 1 ... Bit 114 = 1`, 8 x
  `[spawn-diag] Sce_em_set type=0x16 behavior=0x02 slot=0..7 pos=(.., -14400, ..)`; Bild G_belege/p1_2080_zombies_cut2.png
  (8 Zombies im Laufgang, Leon am Treppenabsatz; F125 umringen sie ihn).
- P3 Leon (Lauf p3h) `RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_GB_TEST=5 RE15_PLAYER_POS=6500,-25000,3072
  RE15_GIVE=3:15 RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT=W2.0,M0.3,MA0.1,..`:
  echter Pistolentreffer -> `F100 AUSLOESER geschwaecht ... Treffer 1` -> ROOM2080 Szene (Marvin 0x40, 8 Zombies) -> Tuer-Ton ->
  8 x `Schuss n: Treffer ... hp=-1 state=3 zeile=5` und 8 x `Kopf ab: Slot s (Part 8 Flagwort 0x4B ...)` (F149..F429, je 40
  Bilder) -> `F550 Kanister aufgehoben (Loco c2 Bild 33)` -> `F620 Wurf: Start (-6328,-16896,-17627) ... Landung
  (-2278,-3576,-17267) nach 36 Bildern` -> `F656 Aufschlag` -> `F671 ROOM2090 RUECKKEHR ... Zone-7-Tot-Bits 236..243 gesetzt:
  8 von 8` -> `Kanister im Becken (-2278,-1200,-17267)`; B zeichnet ihn in Cut 0 (`[gator-re2] Kanister Stufe 0
  pos=(-2278,-1200,-17267) Kasten=(254..317,98..120)`). Bildbogen G_belege/szene2080_bildbogen_p3g.png (Cut 2 Schuesse und
  Kopfwolken, Cut 5 an der Reling, Cut 7 mit "Marvin: Hey, Leon, try with this!"), 2090 danach G_belege/rueckkehr_2090_cut0.png.
- P3 Elza (Lauf p3e_elza) wie oben + RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1:
  ROOM2091 -> ROOM2081 `SZENE (John 0x49 ...)`, 8 Kopf ab, Wurf -> Landung (-2250,-3545,-17276), RUECKKEHR ROOM2091;
  Bild G_belege/szene2081_john_wurf_f600_x2.png ("John: Hey, Elza, try with this!").
- Landepunkt (-2278,-17267) liegt im Becken: Aussenwaende x[-8900..7200] z[-27000..-5400], 578 westlich des Plattform-Blocks
  x[-1700..1850] z[-20000..-12450] (gb_becken_draussen, enemy_ai_boss_gator.c) = Westkanal.

### 3.4 Kamera des Wurfs (Nachtrag 5 "GENAUSO ... von der Kamera") - Zuordnung RE2-Schnitt -> 2080/2090-Kamera
RE2 ROOM7020 (RID @RDT+0x24, 32 B je Cut): Cut 7 pos (-22158,-3852,-15174) -> (-28368,-7434,-18234) fov 26684 = der Werfer-Schnitt
(@0x0726, Ada steht bei (-28000,-7500,-17900) @0x0740 mit Gier 0x0e74 @0x0748); Cut 11 pos (-15948,-1584,-21978) ->
(-20592,-2448,-20790) fov 37165 = der Fangende (Leon, @0x07D4). Blickbeziehung (frontal = cos zwischen Blickrichtung des Werfers
und Werfer-Kopf -> Kamera; Kopf = Fuss - 2500; Rechnung im Chat-Protokoll dieser Spur, Werte):

| Kamera | frontal | Hoehe zum Werfer-Kopf | Abstand | Werfer im Bild |
|---|---|---|---|---|
| RE2 ROOM7020 Cut 7 (Vorbild) | 0,99 (10 Grad) | 43,6 Grad UNTER dem Kopf (von unten) | 8908 | ja (Vorbild) |
| 2080 Cut 7 (gewaehlt) | 0,89 (27 Grad) | 6,4 Grad ueber dem Kopf | 17168 | ja (Fuss (130,71) Probe) |
| 2080 Cut 8 (flag 1) | 0,90 | 44,4 Grad unter dem Kopf | 13757 | NEIN (blickt nach Osten weg) |
| 2080 Cut 2 | 0,93 | 49,8 Grad ueber | 3768 | nein (nur Kopf am Bildrand) |
| 2080 Cut 5 | -1,00 (von hinten) | 17,4 Grad ueber | 6015 | ja, von hinten |
| 2080 Cut 6 | -0,89 | 60,7 Grad ueber | 495 | nein |
| 2090 alle 13 Cuts | - | - | - | nein (Probe: Balkon in keinem 2090-Bild) |
PORT-WAHL der Zuordnung: Cut 7 ist die einzige Kamera, die den Werfer von der Seite des Fangenden (frontal) und ganz im Bild
zeigt; von unten wie RE2 sieht ihn keine vorhandene Kamera (keine neuen Hintergruende, Entscheid B8). RE2 Cut 11 (Schnitt auf den
Fangenden 15 Bilder nach dem Aufschlag) = Rueckkehr nach 2090 auf die Kamera, die Leon beim Ausloesen hatte. Die RE2-Schnitte des
Alligatorkampfes (Nachtrag 4: Aufnahme ohne Schnitt, Explosion R1 automatisch) laufen danach unveraendert in Spur B's Modul.
