# Runde 41 — Spur G "kanister2080" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, Basis `2adca3bf` (= r41/integration 9f10bdb0
master + A D I + B, plus AUFTRAG/VERTRAG Stand Nachtrag 16 / FG-E1).

Zuteilung (VERTRAG 1.1/1.2, FG-E1): Bits 114-119, Nachrichten-Ids 2080/2081 42-47 (42 = "Hey, Leon/Elza, try with this!").
G besitzt ROOM2080/2081; F besitzt 2090-Szene (Bit 110 = Szene gesehen). Kanister im 2090-Wasser ueber B's API
(`re15_gr2_kanister_setzen`), Treffer-Zaehler aus B's API.

Punkte (Wortlaut AUFTRAG.md):
1. Nach dem Start der Alligator-Fress-Szene: in 2080 oben Metalltuer-Schliess-SE, dann 8 gespawnte Zombies im Hauptbereich.
   Nachtrag 17: statt des Schliess-Tons der KNALL wie nach Irons; Nachtrag 20: die Zombies oberhalb der Treppe, nicht unten.
2. Gaskanister (RE2) am Rand der Reling in 2080 (Gas_Can.bmp).
3. Nach genug Treffern auf den Alligator: Szene — Tuer-SE, Marvin (Leon)/John (Elza) in 2080, 8 Kopfschuesse, Kanister
   aufnehmen (beide Haende), "Hey, Leon, try with this!", Wurf ueber die Reling (Kamera wie Adas Raketenwerfer-Wurf RE2,
   Nachtrag 5), Kanister in 2090 im Wasser, Alligator nimmt ihn auf (B), Schuss -> RE2-Tod (B).

## 0. Stand / Fortschritt
- 2026-10-24: Pflichtlektuere (AUFTRAG/VERTRAG/UEBERGABE, B-Dossier §2.5/§7/§9), Raum-RE 2080/2090, RE2-Zensus Raketenwerfer.
- 2026-10-24 23:xx: P1 (Knall nach Nachtrag 17, dann 8 Zombies oben nach Nachtrag 20), P2 (Kanister an der Reling) und P3
  (Szene Marvin/John, Kopfschuesse, RE2-Wurf, Kanister im Becken) gebaut und an der exe gemessen (Abschnitte 4/4.1); Riegel
  gezielt 34/34 gruen (Abschnitt 5). Offen: Abschnitt 6, Fragen: Abschnitt 7.
- 2026-10-25: Nachbesserung 1 (Abnahme 0, Maengel M1-M5; Orchestrator-Entscheide FG-E3/FG-E4 waehrend der Arbeit) -
  Abschnitt 8 (je Mangel Ursache, Messung vorher, Beleg, Aenderung, Messung nachher; Tests 8.6, OFFEN 8.7, Nutzer 8.8).
- 2026-10-25: Nachbesserung 2 (Abnahme 1, Maengel N1-N3) - Abschnitt 9 (N1 Zeile 42 volle Zeit = RE2 Wsleep/Wsleeping vor dem
  Loslassen; N2 Dir_set mit dem Schnitt auf Cut 7; N3 Wurf in das vom Alligator weitere Becken + Anschwimmen = RE2-Zielmodus;
  Messung 9.4, Tests 9.5, OFFEN 9.6, Nutzer 9.7). Massgeblich fuer den Stand ist Abschnitt 9, dann 8.
- 2026-10-25 (ab 12:3x): Nachbesserung 3 (Abnahme 2 `G_abnahme_2.md`, Maengel M1-M3; Orchestrator-Entscheide G-E1/G-E2,
  Nachtrag 28) - Abschnitt 10. Massgeblich fuer den Stand ist Abschnitt 10, dann 9, dann 8.
- 2026-10-25 (ab 17:3x): Nachbesserung 4 (Abnahme 3 `G_abnahme_3.md`, Maengel M1-M3 + V1) - Abschnitt 11. Massgeblich fuer
  den Stand ist Abschnitt 11, dann 10, 9, 8.

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

### 1.8 Clip-Zensus Marvin 0x40 / John 0x49 (tools/r41_kanister2080/probe_npc_clips.c, Pose = re15_skel_compute_pose)
CDEMD0.EMS Blob 0x40 @0x2CD800 / 0x49 @0x355800, je 9 Verzeichnis-Eintraege; Bank 0 (Loco, dir[1]/dir[2]) 24 Clips, Bank 1
(Eigenbank dir[3]/dir[4], spielt Sub 2/4/5/6/9, re15_emd.h:226-232) 14 Clips; Clip-Struktur und Bewegung 0x40 = 0x49 (diff
ohne Wurzelwerte leer). Armketten 0->9->10->11 / 0->12->13->14 (Haende = Bone 11/14), Kopf = Bone 8; Modell: +x = vorn,
y < 0 = oben. Volltabelle G_belege/npc40_clipzensus.txt. Belegte Lesarten:
- Eigen c6 (10 B.): Haende von der Huefte (-31,-1602) nach vorn/oben (764..917, -2534..-2458), Abstand 242 = beidhaendig
  = **Anheben/Zielen**; Eigen c7 (16 B.) haelt vorn oben = **Feuern** (ROOM1021 sub02 Plc_motion(2,6)/(2,7) @0x020C6/@0x020CE);
  c8 Halten; c9/c10 oben, c11/c12 unten.
- Eigen c0 (22 B.): beide Haende vorn auf Brusthoehe (350..683, -1886..-1821), Abstand 722..1005 = **Tragen vor der Brust**.
- ~~Loco c2 (50 B.): Wurzel sinkt -1813 -> -1005 (f33), linke Hand am Boden (149,-328), dann zurueck = **Buecken + Aufheben**.~~
  **FALSCH (Nachbesserung 1, 8.1):** Loco c2 = PL00-KLETTERN (Aufstieg, @0x80038134); Eigen c0 (oben "Tragen") = RUN-Gait
  (@0x8005157c). Das Aufnehmen ist jetzt Loco c11 Hinknien/Aufstehen (ROOM11B0 sub06 @0x01650-0x01660).
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

### 1.10 Nachtrag 17 (19:44): der KNALL beim Hereinkommen der Zombies - Original-Stelle und Port-Datei
Wortlaut: "Muss natuerlich ein knallen sein, als ob die Zombies rein brechen, so wie bei STAGE1 von Elza/Leon am Ende in der
Cutscene nach Irons." (Bezug: Punkt 1, "Metalltuer-Ton und 8 Zombies").
- Die Szene nach Irons im Port: Leon `irons_tod_1150.c` k_p_szene endet mit `OP_KNALL(RE15_IT_SIG_KNALL_TUER)` + Sleep 45 + Tuer
  ("der Knall (aus dem Buero gehoert)"), Signal (5,28) -> Takt `re15_audio_re2_tuer_se(KNALL_SE_TUER)` (irons_tod_1150.c:1357);
  Elza `elzazombies_r39.c` Programm A (Auftakt ROOM1041) `OP_KNALL(RE15_R39Q_SIG_KNALL_TUER)` -> `re15_audio_re2_tuer_se(KNALL_SE_TUER)`
  (elzazombies_r39.c:1123). Beide laden vorher `re15_audio_re2_tuer_laden(k_knall_bank, KNALL_BANK_SIZE)` =
  `engine/src/gen/knall_bank.inc` (19976 B, Generator `tools/r35_l/knall_cut.py`, Runde 36 Spur K: Nutzer verwarf damals den
  Door_exit-Schliesston "Das ist kein Tuerknallen, das ist einfach nur der Sound von Tuer schliessen").
- **Satz 0 = KNALL_SE_TUER = RE2 ROOM1060** (info/re2leon/PL0/RDT/ROOM1060.RDT, 150996 B), selbst nachgelesen
  (`tools/r41_kanister2080/belege_n17_n20.py knall`): sub05 **@0x0F48 `36 02 0a 01 00 00 dc 88 f8 f8 3d 05`** = Se_on Bank 2 Satz 0x0a (der Schlag gegen
  die Huettentuer, hinter der die Zombies stehen); Raumbank-Kopf +0x08/+0x0C/+0x10 = EDT @0x3F58 / VH @0x4018 / VB @0x4C38;
  **EDT[0x0a] @0x3F80 `00 00 85 20`** (Programm 0, Tone 8 + Zusatzlage Tone 9); **VAG 5 @Datei 0x066B8, 8768 B, sha1
  192e09df824ba47854001928662ec82e9fd33522** = bytegleich die Bank-VB @0xC38 in knall_bank.inc (SE-Map Satz 0 `00 00 05 20`).
  RE2_SOUND_LISTE.md Z.122: "ROOM106.SAP - Cabin sounds - **Zombies breaking in**".
- Satz 1 (KNALL_SE_1030 = RE1.5 ROOM1030 sub08 @0x02776 Se_on 0x0c) ist der Rolltor-Knall der Montage (1040/1030), nicht der
  Tuer-Einbruch -> nicht genommen.
- Port: derselbe Ton = dieselbe Datei/derselbe Satz: kanister2080_pc.c laedt `gen/knall_bank.inc` in die Tuerbank und spielt
  Satz 0 (Abschnitt 3.5). Spur F soll fuer die 2090-Einstellung denselben Aufruf nehmen (VERTRAG: nur ueber Dateien/APIs).
- Abgrenzung Punkt 3: "Es gibt erneut die Geraeusche einer schliessenden Tuer. Marvin ... ist im ROOM 2080" = Marvin/John kommt
  durch die Tuer herein (kein Einbruch). Nachtrag 17 ist mit "Ton beim Hereinkommen der Zombies" ueberschrieben -> in der Szene
  bleibt der Schliess-Ton DOOR06 Satz 1 (Door_exit @0x800141f0); Entscheidungsfrage im Abschnitt 7.

### 1.11 Nachtrag 20 (21:41): Ebene der Zombies - "oberhalb von der Treppe, nicht unten"
Wortlaut: "In 2080, nachdem die Zombies durch die Tuer gebrochen sind, sollen sie oberhalb von der Treppe laufen, nicht unten."
- **Die Treppe** = ROOM2080 main00 **@0x0BE2 `2c 02 0c 31 80 00 54 a7 46 ba 9c 18 fc 08 00 00 03 00 00 00`**: Aot_set Slot 2
  sce 12 (Treppe in x), Band-Byte 0x80 = Wildcard (`andi v0,v0,0x80` @0x80042cac-cd4), Rechteck x -22700..-16400 z -17850..-15550,
  side 0 (rec+0xC: die Haelfte x < -19550 ist das UNTERE Ende, @0x80043500), Bandsprung 3 (rec+0xE) -> **unten Band 5
  (y -9000) an der 2030-Tuer, oben Band 8 (y -14400)**. Laufzeit: `[STAIRREG] slot=2 type=12 chain=128 side=0 count=3
  corner=-22700 extent=6300` und `[stair] START dir=UP cur_band=5 target_band=8` (Lauf n20v, stair_dbg.log).
- **Tueren** (main00): Slot 0 @0x0BA2 -> ROOM2030 auf Band 5 (Rechteck (-24400,-17700)) = Fuss der Treppe, RE2-Tuerarchiv DOOR06
  (gen/tuer_zuordnung.inc S161); Slot 1 @0x0BC2 -> ROOM2090 auf Band 8 (Rechteck (-9650,-26150)) = Sued-Ende des Laufgangs.
  Bilder (extracted ROOM208/ROOM20800..09): Cut 0 = Treppe von oben zur 2030-Tuer, Cut 1 = Treppe von unten, Cut 2 = oberer
  Hauptbereich mit Rechner, der rote Rahmen hinten = Oeffnung zum oberen Treppenabsatz.
- **SCA @0x4BC** (5 Gruppen je 28 Zellen, Dump `tools/r41_kanister2080/belege_n17_n20.py sca`): Band 8 (floor 0x88) umschliesst die OBERE Ebene vollstaendig:
  Laufgang x -14300..-9200 z -26900..-6750, Treppenabsatz x -16650..-14300 z -18300..-14750 (Westwand **@0x05C4 x -17650..-16650
  z -19300..-13750**, Sued @0x05AC, Nord @0x05B8), Balkon x -9200..-6700 z -18600..-16550 (Reling @0x05A0); Band 5 (0x55)
  umschliesst den Treppenfuss x -23900..-22450 z -17950..-15400 (@0x04E0..@0x0504). Die Treppe (x -22700..-16400) liegt westlich
  der Absatz-Westwand bei x -16650 -> fuer Band 8 nicht begehbar.
- **Gegner kollidieren mit den Zellen ihres Bandes**: FUN_8003b0a4 vergleicht das Zustandsbyte +0x82 mit floor>>4 jeder Zelle
  (`lbu v1,130(a3)` + `bne` @0x8003b228-3c; Port collision_constrain_impl, Band aus e->y ueber re15_collision_constrain_enemy in
  enemy_ai_common.c), Zombie-Maske 4 (`lbu a2,471(a0)` @0x80100624) trifft u0 0xff aller 2080-Zellen. Die 8 Zombies stehen mit Sce_em_set pc[4] = 8
  (y = -8*1800 @0x800421d4) auf Band 8 -> sie laufen nur oben (Laufgang/Absatz/Balkon), ein Abstieg ueber die Treppe ist
  geometrisch ausgeschlossen (Riegel unit_r41_kanister2080_ebene: Flutfuellung der Band-8-Freiflaeche ab jedem Zombie).
- "durch die Tuer gebrochen": die einzige Tuer, durch die Zombies von aussen kommen, ist Slot 0 am Treppenfuss (Slot 1 fuehrt ins
  Alligator-Becken 2090); oben ist die Oeffnung zum Treppenabsatz. Die Lage der 8 (Tabelle k_zombies, API
  `re15_k2080_zombie_lage`) ist die "dieselbe Lage" fuer Spur F's Einstellung.

### 1.12 Nachtrag 21 (22:25): Zugang zum Alligator ueber die Leiter - Folgen fuer Spur G
Wortlaut: "... man kommt ueber die Leiter zum Aligator, nicht ueber das Tor. Das Tor soll verschlossen sein, und dann mit den
Schluessel den man vom Aligator bekommt geoeffnet werden!" Orchestrator: "Spur G: die Kanister-Szene setzt voraus, dass der
Spieler ueber die Leiter in 2090 ist (nicht ueber 20A0)."
- Leiterweg = ROOM2080 main00 Slot 1 @0x0BC2 (Band 8, Rechteck (-9650,-26150)) -> ROOM2090 (-8200,0,-25550), gesperrt bis zum
  Rechner (sub00 @0x0C50 Aot_reset Slot 1 solange (3,245) = 0; sub02 @0x0CAC gibt ihn frei, Abschnitt 1.1); zurueck ROOM2090
  Tuer 0 @0x0996 -> 2080 (-9950,-14400,-25700) Cut 4 (gemessen `[aot] DOOR FIRE slot=0 ... target_cut=4 spawn=(-9950,-14400,
  -25700)`, DOOR16).
- Spur G braucht dafuer KEINE Aenderung: der Ausloeser (Abschnitt 3.3) verlangt den lebenden, geschwaechten Alligator; solange
  er lebt, ist das Tor 2090/20A0 verschlossen (Spur F) - in 2090 ist der Spieler dann nur ueber die Leiter. Die Szene sichert
  die Spielerlage beim Ausloesen und kehrt dorthin zurueck (kein Tor-Bezug). P1 (Knall + Zombies) faellt auf das erste
  Betreten von 2080 NACH Bit 110 - im Leiterweg also auf den Rueckweg die Leiter hinauf (Riegel `ankunft` laeuft genau diesen
  Weg 2090 -> 2080 ueber DOOR16) oder auf den Eintritt von 2030.

## 2. Messung vorher (Stand 2adca3bf, Bau local_build.sh, exe-Kopie re15_pc_g41_mess.exe)
Lauf vorher1: `RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_FORCE_CUT=5 RE15_FRAMEDUMP=20-60/20 RE15_SPAWN_DIAG=1
RE15_EXIT_AT=60#2080` (Titel-Autostart wie die r41-Riegel).
- Punkt 1: debug.log 0 Zeilen `[spawn-diag]` in ROOM2080 (kein Gegner), kein Tuer-Ton (Bit 110 gesetzt, 2080 aendert sich nicht).
- Punkt 2: Cut 5 Bild 40 (G_belege/vorher_2080_cut5_f40_x2.png, 2x vergroessert): Balkon leer, kein Kanister.
- Punkt 3: kein Ausloeser, keine Szene (kein Code ausser B's Testhaken RE15_GATOR_KANISTER).

### 2.1 Messung vorher zu Nachtrag 17/20 (Stand 17ee51a6, exe-Kopie re15_pc_g41_mess.exe)
Lauf n20v: `RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_PLAYER_POS=-23200,-16700,0,5` (Leon am Treppenfuss vor der
2030-Tuer, Band 5), `RE15_INPUT_SCRIPT=W6.0,A0.2,W10.0,L1.1,A0.2,W8.0` (Basis Spiel), RE15_STATE_LOG, RE15_STAIR_DBG,
RE15_R39_MESS_SPIELER_HP=100, Bildserie 30..900/30, Auswertung `tools/r41_kanister2080/belege_n17_n20.py zlage state.log`.
- Ton (Nachtrag 17): `[k2080] F250 Tuer-Ton: DOOR06-Tonteil 19608 B ... Satz 1 (Door_exit @0x800141f0)` = der Schliess-Ton, nicht
  der Knall.
- Ebene (Nachtrag 20): bis Bild 180 (Leon unten, Cut 0) stehen alle 8 still an ihren Plaetzen; Bild 181 `[stair] START dir=UP
  cur_band=5 target_band=8`, ab Bild ~300 ist Leon oben im Durchgang (Cut 2), die Zombies laufen auf ihn zu und umringen ihn im
  oberen Hauptbereich. 7160 Zombie-Saetze: **x -16120..-9556, z -20000..-14700, 0 westlich der Absatz-Westwand x -16650**.
  Der Abstieg (L1.1 + A0.2) kam nicht zustande (Leon war gepackt, Gier unveraendert) -> Messung "Leon geht wieder hinunter" im
  Nachher-Lauf. Bilder: Cut 0 (Leon unten, Treppe leer), Cut 1 (auf der Treppe), Cut 2 (oben, Zombies im Hauptbereich).

## 3. Umsetzung

### 3.1 Dateien (alle neu ausser den markierten Haken)
- `include/re15_kanister2080_r41.h` - alle Konstanten mit Adresse/Herkunft.
- `engine/src/kanister2080_r41.c` - Installer (2080/2081/2090/2091), Programme (Original-Opcode-Formen, zur Laufzeit gebaut
  wie elzazombies_r39.c), Bildtakt (Signale Bank 5 Wort 0, Anvisieren, Kopfschuss, Kanister in den Haenden, RE2-Wurfbahn),
  Zeichner-Abfrage, Messhaken RE15_K2080_GATOR_HP.
- `platform/pc/src/kanister2080_pc.c` - Kanister-Zeichner (Modell/TIM-Platz 53 wie Spur B), Knall P1 (gen/knall_bank.inc Satz 0,
  Nachtrag 17, Abschnitt 3.5), Tuer-Ton P3 (DOOR06-Tonteil -> RE2-Tuerbank Satz 1), Aufschlag (KANISTER2080.VBS), NPC-Schuss
  (re15_elza_se_hook RE15_ELZA_TON_SCHUSS).
- `engine/src/gen/kanister2080_r41_texte.inc` (tools/r41_kanister2080/texte_bauen.py), `engine/src/gen/r41_kanister2080_bank.inc`
  + NEUES ASSET `shared_assets/RE2/KANISTER2080.VBS` (9864 B, md5 848c40f6c077d937fb025df9ae1bc62b;
  tools/r41_kanister2080/kanister2080_se_cut.py = RE2 ROOM40A0 snd0 EDT-Satz 11 `00 01 83 00`, 1 VAG 6720 B).
- Haken (je 1 Zeile, "Runde 41 Spur G"): `scd_room_setup.c` Ende des Installer-Blocks; `scd_vm.c` Bildtakt nach
  re15_elzazombies_r39_tick; `platform/pc/main.c` Init nach re15_pc_gator_re2_init + Zeichnen nach re15_gr2_pc_zeichnen;
  `tests/unit/test_r36_kartenbits.c` k_port[] Bits 114/115/116 (+ include).
- Werkzeuge/Sonden: tools/r41_kanister2080/probe_kanister_lage.c, probe_lagen.c, probe_npc_clips.c, blick_wurf.py,
  belege_n17_n20.py (Knall-/SCA-/state.log-Belege).

### 3.2 Lagen (PORT-WAHL mit Messung; probe_lagen.c auf ROOM2080.RDT)
- Kanister (-6745,-14400,-17625) = Rueckprojektion der Markenmitte (1.5); in Cut 5 Fuss (168,155) Kopf (170,77) = Markenmitte.
- 8 Zombies Band 8 zwischen Treppenabsatz und Balkon (Tabelle k_zombies, je >= 700 von Band-8-Waenden, paarweise >= 1393, in
  Cut 2 und Cut 7 mit Fuss UND Kopf im Bild), Blick auf den Treppenabsatz (Gier = atan2(-dz,dx)).
- Marvin/John Start (-14800,-16500) Gier 0 (Treppenabsatz, in Cut 2 Bild (164,86..115)); Aufheben-Stand (-7245,-17625), RUN-Ziel
  250 dahinter (Ankunft d < 300 @0x80051764); ~~Wurf-Stand (-7100,-17625) per WALK~~ (seit Nachbesserung 1 wirft er vom Aufheb-Stand,
  -7100 bleibt nur Nennlage der Bahn-Nachrechnung) (d < 100 @0x800513f8): Reling-Zelle @0x5A0 ab
  x -6700, Radius 400. Relingstange: Bodenlinie x -6350 (Pfosten in Gas_Can.bmp (139,151)/(200,151) rueckprojiziert),
  Oberkante ~ -15443 (31 px bei Tiefe ~7000, H 208).
- Spieler-Parkplatz waehrend des 2080-Teils: (-23200,-9000,-16700) Band 5 = Eintritt von 2030 (re15_room_spawns.h).

### 3.3 Ablauf (Programm SZENE, zur Laufzeit gebaut; Zeilenform je Opcode im Dateikopf von kanister2080_r41.c)
| Schritt | Port | Vorbild |
|---|---|---|
| 2090: Riegel + Raumwechsel | Set(2,7,1) Set(1,27,1), Door_aot_set Slot 21 -> ROOM2080 Cut 2 Parkplatz, Aot_on | RE2 sub12 @0x06F2/6; Montage irons_tod_1150.c |
| 2080 Abschnitt 1 | Sce_em_set NPC (Slot 8, grid 0x40, Etage 8) + 8 Zombies (Tot-Gate) + Sleep 1 | ROOM2000 @0x017B4 / ROOM2020 @0x008F6 |
| Tuer-Ton | Signal -> DOOR06 Satz 1, kein Sleep danach (8.3) | Door_exit @0x800141f0; ROOM1021 @0x020C2-CA |
| Anlegen | Work_set(2,8), Plc_motion(2,6), Sleep 110 | ROOM1021 @0x020C2/@0x020C6/@0x020CA |
| je Zombie | Signal ZIEL, Poll AUSGERICHTET (Nachdrehen 0xC8/Bild, 8.3), Plc_motion(2,7), Signal SCHUSS, Sleep 30 | ROOM1021 @0x020CE/@0x020D2; Gun-FSM @0x80033f90 |
| Senken | Plc_motion(2,6) + Plc_flg 0x80, Sleep 20 | ROOM1021 @0x020EA/@0x020EE/@0x020F2 |
| Lauf zum Kanister | Cut_chg 5, Plc_dest RUN + Ankunfts-Poll | ROOM1021 @0x020B2; ROOM1150 sub08 @0x0112A |
| Aufheben (Nachbesserung 1) | Plc_motion(1,11) Hinknien, Sleep 24, Signal AUFHEBEN, Sleep 36, Plc_motion(1,11)+Plc_flg 0x80 Aufstehen, Sleep 30 | ROOM11B0 sub06 @0x01650/@0x01654/@0x01658/@0x0165C/@0x01660 (8.1) |
| ~~an die Reling~~ | entfernt (der Kanister steht an der Reling; vorher WALK + Plc_motion(2,0) = RUN-Gait) | 8.1 |
| Wurf (RE2 sub12) | Cut_chg 7, Plc_motion(2,6) (PORT-WAHL fuer Adas Clip `3f 0a 12 40`, 8.3), Message_on 42, Sleep 30, Plc_motion(2,6), Sleep 20, Signal WURF, Poll GELANDET, Sleep 15 | @0x0726/0750/0764/076A/0782/0786/078E/079C/07A8 |
| Fall (RE2 sub13) | Takt: Lage += v, v += a; v (25,20,10), a (5,20,0) im Blickrahmen; Ende y >= Wasser - 800; Aufschlag-Ton | @0x088E-08B0, @0x80055ab0/@0x80055b2c, ROOM40A0 @0x0686 |
| Rueckkehr | Door_aot_set -> 2090 (Spielerlage/Cut beim Ausloesen), Aot_on | RE2 Cut_chg 11 @0x07D4 (Schnitt auf den Fangenden) |
| 2090 Ende | Work_set(1,0)+Plc_ret, Set(2,7,0) Set(1,27,0), Cut_auto 1 (kein Sleep 30, 8.3); Bits 115/116; Kanister an die Spielstelle (B-API, 8.2) | RE2 sub18 @0x0A2C-0A38; Pos_set @0x08CA |

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

### 3.5 Umsetzung Nachtrag 17 / Nachtrag 20 (Bau build16)
- **Knall (P1, Nachtrag 17):** `kanister2080_r41.c` Installer, erstes Betreten von 2080/2081 nach Bit 110: ZUERST der neue Haken
  `re15_k2080_knall_hook` (Anstoss im Raumaufbau, vor dem ersten VM-Lauf), DANN das Spawn-Programm (8 x Sce_em_set im ersten
  VM-Lauf des Raumaufbaus) = Reihenfolge "erst Knall, dann Zombies" (Runde 36 K3). PC: `kanister2080_pc.c knall()` laedt
  `engine/src/gen/knall_bank.inc` (19976 B, dieselbe Datei wie irons_tod_1150.c / elzazombies_r39.c) mit
  `re15_audio_re2_tuer_laden` in die Tuerbank und spielt `re15_audio_re2_tuer_se(KNALL_SE_TUER)` = Satz 0 (Abschnitt 1.10).
- **Szene (P3):** bleibt der Schliess-Ton DOOR06 Satz 1 (Door_exit @0x800141f0) beim Hereinkommen von Marvin/John (Abgrenzung 1.10,
  Frage im Abschnitt 7).
- **Ebene (Nachtrag 20):** die 8 Lagen (k_zombies) liegen schon auf Band 8 oestlich der Absatzwand (Messung vorher 2.1) - keine
  Verhaltensaenderung. Neu im Header: `RE15_K2080_TREPPE_X0/X1/Z0/Z1` (Treppen-Aot @0x0BE2) und `RE15_K2080_ABSATZ_WAND_X`
  (Zelle @0x05C4); Messhaken `re15_k2080_zombies_oben` und Messzeile `[k2080] F.. Ebene: n Zombies x a..b y c..d, auf der Treppe
  (x < -16650) k, nicht auf y -14400: h | Spieler (x,y,z)` alle 30 Bilder in 2080/2081 (kein Verhalten).
- **Ton-Messung ohne Proxy:** die Zeile "SE-Stimmen aktiv" (Zaehlung der gepumpten SE-Stimmen SPU 16..23 zwei Bilder nach dem
  Anstoss) ist entfernt. Sie war ein Proxy: Saetze mit (byte3 & 0x1f) < 0x10 laufen im Direkt-Zweig FUN_80045024 (`SsUtKeyOnV`
  @0x8004522c, audio_pc.c se_play_layers) auf einer Pool-Stimme AUSSERHALB von SPU 16..23 - der Knall (Bank-Satz `00 00 05 20`)
  und der Aufschlag (`00 01 83 00`) gehoeren dazu; die fruehere Zeile "Aufschlag: n SE-Stimmen aktiv" zaehlte also fremde
  Stimmen. Jetzt: mit RE15_SE_DEBUG=1 schreibt se_play_layers je gespielter Lage `[se] Stimme: se.. layer.. vag.. note.. fine..
  center.. (.. Hz) -> SE-Stimme ..` direkt hinter die Ausloese-Zeile dieses Moduls (gleicher Aufruf, gleicher Strom).
- Werkzeug `tools/r41_kanister2080/belege_n17_n20.py knall|sca|zlage` (Belege 1.10/1.11, Auswertung state.log).
- **PSX-Variante des Moduls (build18):** `kanister2080_r41.c` rief `labs` ohne <stdlib.h> (das nur unter RE15_PLATFORM_PC
  eingebunden ist) -> eigener Betrag; die leere PSX-Form von K_LOG nimmt die Werte jetzt als Argumente (keine unbenutzten
  Log-Werte); die Messzeile Ebene laeuft nur auf dem PC (Abschnitt 5).

## 4. Messung nachher (exe-Kopie re15_pc_g41_mess.exe, Bau 2026-10-24)
Grundumgebung: RE15_NO_INTRO=1 RE15_NOAUDIO=1 Titel-Autostart, RE15_WINDOW_SCALE=1.
- P1 (Lauf p1a) `RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_PLAYER_POS=-15000,-16500,0,8 RE15_FORCE_CUT=2`:
  `[k2080] F250 ROOM2080 nach Bit 110: 8 lebende Zombies (Tot-Bits 236..243)`, `Tuer-Ton DOOR06 Satz 1 ... Bit 114 = 1` (Stand VOR Nachtrag 17; seit 3.5 der Knall, Abschnitt 4.1), 8 x
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

### 4.1 Messung nachher Nachtrag 17 / 20 (Bau build16, exe-Kopie re15_pc_g41_mess.exe)
- **Knall, echter Tuerweg 2090 -> 2080** (Lauf n20n_knall, RE15_AUDIO_CAP_SYNC=cap.raw, `RE15_INPUT_SCRIPT=W1.0,A0.2,W6.0` an der
  Leiter-Tuer): `[tuer] Sequenz Archiv 2 DOOR16 ...`, dann `[k2080] F31 Knall: Knall-Bank 19976 B in der Tuerbank, Satz 0 (RE2
  ROOM1060 Se_on 0x0a @0x0F48, wie die Szene nach Irons)`, `Knall (...) - erstes Betreten nach Bit 110, Bit 114 = 1`, erst
  DANACH die 8 `[spawn-diag] Sce_em_set type=0x16 behavior=0x02 slot=0..7 pos=(..,-14400,..)`. Mitschnitt gegen die selbst
  dekodierten Referenzen (`tools/r36_k/knall_im_mitschnitt.py cap.raw`, RE15_INFO_DIR = Hauptbaum, nur gelesen):
  **`RE2_1060 (11025 Hz): t=15.57 s Bild 467 r=0.891 Huelle-max 6853`**, DOOR04 und R1030 kein Treffer >= 0,5; Bild 467 von 557
  = das erste 2080-Bild (Ende EXIT_AT 90#2080: 557 - 90). Vergleich Runde 36 K (Ende der 1150-Szene): r 0,93, Huelle 6617.
- **Lauf n20n_sedbg** (dasselbe + RE15_SE_DEBUG=1): direkt hinter der Ausloese-Zeile `[se] Stimme: se=0 layer=0 vag=0 note=67
  fine=0 center=91 vol=127 pitch=0x400 (11025 Hz) -> SE-Stimme -16 (SPU 0)` und `... layer=1 vag=0 note=67 fine=1 center=91 ...`
  = Tone 8/9 der ROOM1060-Bank (center 91 / min 67 -> 11025 Hz, wie Runde 36 K §K1.2), Direkt-Zweig.
- **Szene Leon** (Lauf n20n_szene, CAP_SYNC + SE_DEBUG): Tuer-Ton `[se] Stimme: se=1 layer=0 vag=2 note=61 fine=57 center=79 vol=127
  pitch=0x5f3 (16397 Hz) -> SE-Stimme 7 (SPU 23)`; Aufschlag `[se] Stimme: se=0 layer=0 vag=0 note=65 fine=57 center=95 vol=127
  pitch=0x2f9 (8193 Hz) -> SE-Stimme -16 (SPU 0)`; alle Ebene-Zeilen F30..F660 `8 Zombies ... y -14400..-14400, auf der Treppe
  (x < -16650) 0`.
- **Ebene** (Lauf n20n_ebene: Leon oben am Absatz `RE15_PLAYER_POS=-16000,-16700,2048,8` Blick West, `W2.0,A0.2,W20.0`, Bilder
  30..720/30): `[stair] START dir=DOWN cur_band=8 target_band=5`, ab Bild 180 steht Leon unten (-22918,-9000,-16715); alle 24
  Messzeilen `Ebene: 8 Zombies ... y -14400..-14400, auf der Treppe (x < -16650) 0, nicht auf y -14400: 0`; die Verfolgung
  reicht bis x -15906 (Bild 180) an den Absatz, danach bleiben sie oben (x-Minimum -13920..-15791). Auswertung state.log
  (`belege_n17_n20.py zlage`): 5720 Zombie-Saetze, x -15908..-9900, keiner westlich x -16650.
- **Lage oben im Bild** (Laeufe n20n_cut7 / n20n_cut2: Leon unten am Treppenfuss, RE15_FORCE_CUT 7 bzw. 2, Bild 40):
  G_belege/n20_oben_cut7_cut2_x2.png - Cut 7 (Totale): die Zombies stehen oben auf dem Laufgang ueber der Grube; Cut 2: die 8
  im oberen Hauptbereich am Rechner. = die Lage, die Spur F fuer "oben in 2080 kommen die Zombies herein" zeigen soll
  (Tabelle k_zombies, API `re15_k2080_zombie_lage`). Bild **G_belege/n20_nachher_abstieg_cut0_1.png**: Bilder 30/60 Cut 0
  (Leon oben hinter der Kamera, Treppe leer), Bild 120 Cut 1 (Leon auf der Treppe, ein Zombie oben am Absatz), Bilder 180/390/600
  Cut 0 (Leon unten, Treppe leer).

## 5. Tests (gezielt, keine volle Suite; Bau build18 ueber local_build.sh, ctest mit msys64 zuerst im PATH)
`ctest --test-dir re15_port/build -R "unit_r41_kanister2080|integration_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_" --timeout 1500 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 34`** (Total Test time (real) = 232.10 sec; integration_r41_kanister2080 190.77 sec;
   erster Lauf auf build16 ebenso 34/34, 288.33 sec).
- Eigene Riegel (tests/unit/probes/r41_kanister2080.cmake): unit_r41_kanister2080_{lage, spawn, szene, start, bahn, texte, ebene}
  und integration_r41_kanister2080 (Laeufe ankunft / ebene / kanister / szene_leon / szene_elza, Kopf des Skripts).
  - `ebene` (neu, Nachtrag 20): `SCA @0x4BC: 140 Zellen, davon 100 auf Band 8; Flaeche ab Zombie 0: 37695 Raster, x -16625..-6725
    z -26875..-6775` / `Rasterrand 0, westlich der Absatzwand 0, auf der Treppe 0`; Treppen-Aot @0x0BE2 und Absatzwand @0x05C4
    gegen die Header-Konstanten.
  - integration `ankunft`: Knall statt Schliess-Ton, beide [se]-Lagen (center 91, 11025 Hz) direkt hinter der Ausloese-Zeile,
    der Knall VOR den 8 Sce_em_set; `ebene` (neu): Abstieg 8 -> 5, 24 Ebene-Zeilen ohne Zombie auf der Treppe / ausserhalb
    y -14400, Verfolgung bis x <= -15000, Leon unten; `szene_leon`: Tuer-Ton (center 79, 16397 Hz) und Aufschlag (center 95,
    8193 Hz) ueber die [se]-Zeile statt der Stimmenzaehlung; in beiden Szenen alle Ebene-Zeilen ohne Zombie auf der Treppe.
- Gruppen der beruehrten gemeinsamen Dateien: unit_r36_kartenbits (k_port[] Bits 114-116), unit_scd_* (7; Haken in
  scd_room_setup.c / scd_vm.c), unit_r41_re2gator_* (18; Spur-B-API re15_gr2_*, Kanister im Becken).
- Nicht gelaufen: integration_r41_re2gator (Spur B; dieser Zweig aendert B's Modul nicht, er ruft nur dessen API).
- PSX-Praeprozessorpfad (ohne RE15_PLATFORM_PC): `gcc -std=gnu11 -fsyntax-only -Wall -Wextra -Wno-comment -I include -I engine/src
  engine/src/kanister2080_r41.c` -> rc 0, 0 Warnungen (vorher, Stand 5d635136: `error: implicit declaration of function 'labs'`,
  <stdlib.h> nur unter RE15_PLATFORM_PC, und 5 x -Wunused-variable fuer reine Log-Werte; behoben mit eigenem Betrag und
  einer PSX-K_LOG-Form, die die Werte benutzt).

## 6. OFFEN (gemessen bzw. benannt, nicht geraten)
1. RE2-Ausloeser-Mitglieder +0x1D8/+0x1DA des Tyrant (ROOM7020 sub11 @0x06BE/@0x06C4, Getter @0x800560a4/@0x800560b0): der
   Schreiber liegt im Tyrant-Overlay, das in info/re2leon nicht vorliegt (Abschnitt 1.9). Der Port nimmt die Alligator-Schwaeche
   (@0x80101ebc-dc) als Form; den RE2-60-s-Zweig (sub10 Sleep 0x0708) baut er nicht.
2. Wurf-Kamera von unten wie RE2 Cut 7 (43,6 Grad unter dem Werfer) gibt es in ROOM2080/2090 nicht (Abschnitt 3.4); Cut 7 von
   der Grubenseite ist die naechste vorhandene (frontal 0,89). Neue Hintergruende werden nicht erfunden (Entscheid B8).
3. Alligator nach der Rueckkehr: der Raum 2090 wird neu aufgebaut, Spur B's Boss startet wieder untergetaucht (B9) und taucht beim
   naechsten Annaehern auf (Intro). ~~Gemessen (Lauf p3h mit RE15_GB_TEST=5): Gator wieder aufgetaucht an der Marker-Lage, verfolgt
   Leon.~~ **Berichtigt (Nachbesserung 1, M4):** p3h ist dafuer KEIN Beleg - RE15_GB_TEST=5 laesst den Boss aufgetaucht an der
   Marker-Lage (4997,-20318) warten und schaltet das Untertauchen ab (enemy_ai_boss_gator.c gb_untertauchen, B9). Im echten Ablauf
   (ohne Testhaken, Abnahme 0 n3_natur/n23_kanal) bleibt er bis zur Annaeherung untergetaucht und nahm den Kanister am alten
   Liegeplatz nicht auf. Ablauf, Liegestelle und Messung im echten Ablauf: Abschnitt 8.2.
4. RE2-Form "Werfer laeuft danach weg" (Adas Plc_dest @0x0812) und RE2 sub12 `Sleep 1` + `Plc_dest 40 00 14 ..` (@0x0754/0x0758,
   Mode 0x14) nicht uebernommen: Marvin/John bleibt in 2080, der Schnitt geht nach 2090 (Abschnitt 3.3).
5. Der Kanister ist RE2-gross (MD1 2784 lang, r 267, ROOM40A0 Modell 1 unveraendert) - in Marvins/Johns Haenden quer vor der
   Brust ragt er je 1392 zur Seite (Bild G_belege/szene2081_john_wurf_f600_x2.png). Kein Massstab erfunden.
6. Ton: alle Laeufe ohne Audiogeraet (RE15_NOAUDIO bzw. RE15_AUDIO_CAP_SYNC); gehoert hat ihn keiner. Belege sind die [se]-Zeilen
   der Mischkette (Satz, VAG, Tonhoehe) und fuer den Knall die Mitschnitt-Korrelation r 0,891 (Abschnitt 4.1).
7. Nachtrag 20 "nachdem die Zombies durch die Tuer gebrochen sind": den Durchbruch selbst zeigt Spur F in der 2090-Szene
   (FG-E1). In 2080 gibt es oben nur die Tuer zum Becken (Slot 1 @0x0BC2); die Zombies von Spur G stehen beim Betreten bereits
   oben (Knall, dann Sce_em_set). Dass F dieselbe Lage (k_zombies) und denselben Ton (gen/knall_bank.inc Satz 0) nimmt, ist in
   diesem Zweig nicht messbar (F's Modul liegt nicht hier).

## 7. Fuer den Nutzer
**Stand nach Nachbesserung 1: Abschnitt 8.8** (Aufnehmen = Hinknien, Kanister liegt im Nordbecken, neue Fragen). Der Text hier ist der
Stand der Abnahme 0.
Neue Assets (Paket-Gate):
- `re15_port/shared_assets/RE2/KANISTER2080.VBS` (9864 B, md5 848c40f6c077d937fb025df9ae1bc62b) = RE2 ROOM40A0 Raumbank Satz 11
  (der Gaskanister faellt ins Wasser).
- weiter benutzt (schon im Paket): `shared_assets/RE2/KANISTER40A0.MD1/.TIM` (Spur B), `shared_assets/RE2/DOOR/DOOR06.DO2`
  (Schliess-Ton der Szene), ELZASE.VBS (Schuss), `engine/src/gen/knall_bank.inc` (der Knall, eingebacken).
Ton-Dateien (Nachtrag 17: "Ton-Datei des Ports nennen"):
- Knall beim Hereinkommen der Zombies = `re15_port/engine/src/gen/knall_bank.inc` Satz 0 (Generator `tools/r35_l/knall_cut.py`,
  Quelle RE2 `PL0/RDT/ROOM1060.RDT` VAG 5 = "Zombies breaking in") - genau der Knall am Ende der Szene nach Irons (Leon und Elza).
Sprachdateien (vom Nutzer aufzunehmen, die Zeile laeuft bis dahin ohne Stimme):
- `synchro/STAGE2/room2080/main42.wav` = "Marvin: Hey, Leon, try with this!"
- `synchro/STAGE2/room2081/main42.wav` = "John: Hey, Elza, try with this!"
Bedienung / Ablauf im Spiel:
- Zugang zum Alligator ueber die Leiter (Nachtrag 21, Abschnitt 1.12).
- Nach der 2090-Szene (Spur F, Bit 110): beim ersten Betreten von 2080/2081 knallt es (wie nach Irons), oben im Hauptbereich
  stehen 8 Zombies. Sie bleiben oben (Laufgang, Rechner, Balkon) - die Treppe zur 2030-Tuer gehen sie nicht hinunter, auch nicht,
  wenn Leon/Elza oben war und wieder hinuntergeht. Der Gaskanister steht von Anfang an am Ende des Balkons (Gas_Can.bmp).
- In 2090: hat der Alligator 300 Schaden genommen (RE2-Schwaeche), loest die Szene aus: Schnitt nach 2080, Schliess-Ton der Tuer
  (Marvin/John kommt herein), Marvin (Leon) bzw. John (Elza) erschiesst die 8 Zombies (Kopf platzt), rennt zum Kanister, hebt ihn
  auf, sagt die Zeile, wirft ihn ueber die Reling; Aufschlag im Wasser; zurueck nach 2090, der Kanister liegt im Westkanal neben
  der Plattform (-2278,-17267). Schwimmt der Alligator vorbei, nimmt er ihn ins Maul (Spur B); ein Schuss auf den Kanister im
  Maul -> RE2-Explosionstod mit Kiefer.
- Messhaken nur fuer Messlaeufe: `RE15_K2080_GATOR_HP=<n>` setzt die HP des Alligators beim Raumeintritt (Schwelle mit einem
  echten Treffer statt 43).
Entscheidungsfragen:
1. Schwelle "oft genug angeschossen" = RE2-Schwaeche (300 Schaden): mit der Pistole 43 Treffer, Schrotflinte 11, Magnum 7,
   Granatwerfer 5. Passt das, oder soll es eine feste Trefferzahl (z. B. 10 Treffer jeder Waffe) sein?
2. Soll der Alligator nach der Szene sofort wieder auftauchen (RE2: Tyrant wird am Szenenende neu gesetzt und kaempft weiter),
   statt wie jetzt beim naechsten Annaehern?
3. In der Szene (Punkt 3 "erneut die Geraeusche einer schliessenden Tuer") kommt Marvin/John herein: dort spielt der
   Schliess-Ton der Stahltuer (DOOR06). Soll dort stattdessen ebenfalls der Knall kommen?

## 8. Nachbesserung 1 (Abnahme 0 `G_abnahme_0.md`, 2026-10-25)
Baum wie oben, Start auf 1608232e (Abnahme-Commit; Code-Stand = 489c0756). Mängel M1-M5 der Abnahme; Nachträge 22-24b
(AUFTRAG.md) massgeblich, FG-E2 ist widerrufen (keine Änderung in Spur B's Modulen gator_re2_r41.c / enemy_ai_boss_gator.c).

### 8.0 Plan / Stand
- [x] M1 RE: welcher Clip lief, was er ist, welcher Original-Clip "hinknien + aufnehmen + aufstehen" ist (8.1)
- [x] M1 Umsetzung + Bildbogen vorher/nachher Leon + Elza + Riegel (8.1, unit szene, integration szene/kampf)
- [x] M2 Messung vorher (8.2); nach FG-E3 umgebaut: derselbe Alligator kämpft weiter, der Kanister liegt an der
  Aufschlagstelle, Wurf-Richtung gemessen gewählt, ganze Folge Leon + Elza im echten Ablauf, Riegel kampf (8.9)
- [x] M3 Kameravorlage (8.4); nach FG-E4 umgebaut: Fall in 2090/2091 Cut 12 mit der RE2-Schnittfolge, Bildvergleich
  RE2 <-> Port und Bildbogen des Falls (8.9)
- [x] M4 Doku (6.3, k_port[] Bit 114, Haken-Kommentare, überholte Stellen markiert, 8.5)
- [x] M5 Dauern belegt bzw. entfernt, Adas Wurf-Clip als PORT-WAHL (8.3)

### 8.1 M1 - Kletter-Animation vor dem Aufnehmen (Nachtrag 24b)
**Ursache.** `baue_szene` spielte `Plc_motion(1, 2)` = Loco-Bank (Sub 1, dir[1]/dir[2]) Clip 2. Der Clip-Zensus 1.8 las ihn aus
vier Stichbildern (f0/f16/f33/f49) als "Bücken + Aufheben". Jedes Bild (Werkzeug `tools/r41_kanister2080/probe_npc_clip_bilder.c`,
Ausgabe `G_belege/nb1_clipbilder_40_49.txt`, Marvin 0x40 und John 0x49, Pose = re15_skel_compute_pose wie der NPC-Zeichner):
- **Loco c2 (50 B.) = KLETTERN (Aufstieg).** Bild 15-28: der rechte Fuss steigt von y -162 auf -1958 (Stufe ~1800 hoch), die
  Wurzel von -1813 auf -2614, der Kopf auf -2990; Bild 29 springt die Wurzel auf (-684,-846) = die zweite Hälfte kniet OBEN auf
  der Stufe (die Kletter-FSM hebt dabei die Spielerhöhe an). Das ist genau der PL00-Clip 2 des Spielers: `KLETTERN aus der
  PL00-Bank ... 231 Aufstieg = clip 2 @0x80038134 (50 Frames)` (`ori v0,zero,0x2` @0x80038134, include/re15_climb.h
  RE15_PLAYER_MOTION_CLIMB_UP, anim_select_common.c), und die NPC-Loco-Bank hat dieselbe Belegung (Bildzahlen c2 50 / c3 26 /
  c4 20 / c5 20 / c6 50 = PL00 Aufstieg / Absprung-Vorbereitung / Fall / Landung 5 / Landung 6; enemy_ai_common.c
  re15_actor_uses_loco_bank "Kletter-/Sprung-Clips 2..5 = +0x84/+0x16c", Runde 38 Spur M). Darum stieg Marvin/John in den
  Bildern 532-544 über die Reling-Höhe (Abnahme 3.9).
- **Eigen c0 (22 B.) = LAUFZYKLUS, nicht "Tragen".** Der Zensus 1.8 nannte ihn "Tragen vor der Brust"; jedes Bild zeigt die Füsse
  im Wechselschritt (FussL -1126 -> -275, FussR -183 -> -1123 = Laufschritte) und die Arme pumpend. Beleg: Sub 5 (Plc_dest
  RUN) setzt beim Ausrichten `+0x94 = 0` = RUN-GAIT CLIP 0 der Bank 1 (`sb zero,148` @0x8005157c, enemy_ai_common.c
  re15_npc_sub_walk). Der alte Ablauf spielte ihn mit `Plc_motion(2, 0)` an der Reling als "Tragen" - ebenfalls falsch.
- **Loco c11 (25 B.) = HINKNIEN (Stand -> Knie)**, rückwärts gespielt = AUFSTEHEN. Wurzel -1810 -> -761, rechter Fuss bleibt am
  Boden (-155), das linke Knie geht hinab (-157 -> -402), beide Hände gehen nach vorn unten: Bild 24 HR (249,-668,-509) /
  HL (593,-312,318) (John: (243,-642,-549) / (600,-270,334), dieselben Schlüsselbilder kf 236-260). Original-Verwendung
  (Plc_motion-Zensus über alle RE1.5-RDTs, `G_belege/nb1_plc_motion_zensus.txt`): **Marvin (0x40) ROOM11B0 sub06
  @0x01650 `3f 01 0b 00`** (Plc_motion(1,11), hinknien am Gully: "There's a manhole here..."), **Sleep 60 `09 0a 3c 00`
  @0x01654**, **@0x01658 `3f 01 0b 00` + @0x0165C `43 00 80 00`** (derselbe Clip mit Plc_flg 0x80 = rückwärts = aufstehen),
  **Sleep 30 `09 0a 1e 00` @0x01660**; John (0x49) ROOM1021 sub02 @0x02102 `3f 01 0b 00` (kniet bei Roy), ROOM5011 sub02/sub04
  (5x, bei Sherry); 0x42 ROOM4010/50B0/1090 (6x); der Spieler 54x (PL00 c11, ROOM1150 "stand->kneel fold", scd_vm.c).
  Einen eigenen "Aufheben"-Clip haben Marvin/John nicht: der Zensus zeigt für NPCs nur Plc_motion(1,7/11/22/23) und (2,6/7),
  und RE2 hat für das Aufnehmen eines Gegenstands keinen Clip (Ada hält den Raketenwerfer schon, Super_set @0x072C).
  -> Das Original-Hinknien/Aufstehen von Marvin ist die Form für "nimmt ihn auf in beide Hände": im Bild 24 liegen beide
  Hände seitlich am Kanister (z -509 / +318 um die Kanisterachse, Radius 267) in Höhe 312..668 über dem Boden.
- **Messung vorher** (Abnahme 0 §3.9, Läufe n24b_leon / n24b_elza auf 489c0756, Bilder übernommen):
  `G_belege/nb1_m1_vorher_marvin_f514-562_abnahme0.png`, `G_belege/nb1_m1_vorher_john_f526-562_abnahme0.png` - Bild 532-544
  Knie hoch und Aufstieg über die Reling-Höhe (= Loco c2 Bild 15-28, Fuss auf -1958), Bild 547 Hocke (Bild 29 ff.).
- **Änderung** (kanister2080_r41.c `baue_szene`, Header RE15_K2080_CLIP_KNIEN / KNIEN_BILDER / KNIEN_HALT / AUFSTEHEN_HALT /
  KNIEN_GRIFF): nach dem Lauf (Plc_dest RUN, Sub 5, Laufzyklus Eigen c0 @0x8005157c = das "Hinlaufen" des Originals)
  `3f 01 0b 00` (Hinknien, @0x01650) · Sleep 24 · Signal AUFHEBEN (Griff im Bild 24 = letztes Bild, beide Hände seitlich am
  Kanister) · Sleep 36 (zusammen Sleep 60 @0x01654) · `3f 01 0b 00` + `43 00 80 00` (Aufstehen rückwärts, @0x01658/@0x0165C) ·
  Sleep 30 (@0x01660); danach direkt der RE2-Wurfteil (der Kanister steht schon an der Reling, kein Gang mehr; der WALK an die
  Reling und `Plc_motion(2,0)` = RUN-Gait als "Tragen" sind entfernt). Die Hände-Lage für den Kanister liest jetzt den
  GEZEICHNETEN Slot (rückwärts fc-1-f, re15_actor_playback_slot @0x8001f34c-54), sonst hätte der Kanister beim Aufstehen die
  Vorwärts-Bilder gesehen.
- **Messung nachher** (Bau 2026-10-25 01:55, exe-Kopien re15_pc_g41nb1*.exe, Rezept wie Abnahme 3.5/3.9 mit RE15_GB_TEST=5 nur als
  Auslöser der Szene, Bilder 380..640/3, EXIT_AT 150#2090/2091): Läufe m1_leon / m1_elza:
  `F435 NPC Lauf Sub 5 Phase 2 (-8083,-17471)` -> `F465 Kanister aufgehoben (Loco c11 Bild 24, Hinknien ROOM11B0 sub06 @0x01650)`
  -> Aufstehen F501-F525 -> `F531` Cut 7 -> `F581 Wurf: Start (-6328,-16896,-17642)` -> `F617 Aufschlag` -> `F632 RUECKKEHR`
  (Elza: John 0x49, dieselben Bilder, Wurf-Start (-6300,-16865,-17651)). Bildbögen `G_belege/nb1_m1_nachher_marvin_f437-530.png`
  und `G_belege/nb1_m1_nachher_john_f437-530.png` (Cut 5, Ausschnitt 2x): 437 steht an der Reling, 446-464 kniet er sich hin
  (der stehende Kanister wird über ihm sichtbar), 467 hält er ihn quer mit beiden Händen vor den Knien, 506-530 steht er mit dem
  Kanister quer vor der Hüfte auf - kein Knieheben, kein Aufstieg, keine Hocke. Hinweis: im Griff-Bild 467 wechselt der Kanister
  von stehend auf quer in einem Bild (wie schon im alten Stand beim Greifen); eine Kipp-Bewegung hätte als RE2-Form das Kippen
  aus ROOM40A0 sub10 (`2f 0b fc ff` @0x066E, 23 Bilder @0x0676, `34 10 00 fc` @0x0682) - nicht gebaut (Frage 8.8).

### 8.2 M2 - Aufnahme im echten Ablauf (Nachtrag 23)
**Messung vorher** (Abnahme 0 §3.8/§3.10, Läufe n3_natur, n23_kanal, n23_kanal_ohne_ap auf 489c0756, ohne RE15_GB_TEST und ohne
RE15_GATOR_KANISTER): nach der Rückkehr ist der Alligator ein neuer Aktor (B9: untergetaucht am Lauerplatz (-7200,-9200),
Auftauchen bei Annäherung < 9000 = Nutzer-Entscheid N22.2 "Erst beim Annähern"); in n23_kanal 2700 Bilder Kampf ohne Aufnahme,
in n23_kanal_ohne_ap / n3_natur bleibt er unten (Leon > 9000 vom Lauerplatz). Eigene Nachrechnung (scratchpad
`g_nb1/fenster_replay.py` = B's Fenster aus gator_re2_r41.c gr2_kan_takt Stufe 0 / gr2_kan_vor, über die Gator-Lagen von
n23_kanal state.log): am alten Liegeplatz (-2278,-17267) **0 Bilder** im Fenster. Ursache: das Fenster ist ein schmaler Streifen
8601..9000 x Skala 2731/4096 = **5734..6000 vor der Wurzel** (= am Maul), quer < 1466 + 267; das Maul überstreicht den Kanister
nur, wenn der Gator in der Verfolgung GERADE auf ihn zuschwimmt und dabei weiter als 6000 Anlauf hat. Der Bahnendpunkt liegt 578
neben dem Plattformblock (x -1700), der Gator klemmt mit Radius 1466 bei x <= -3166 und schwimmt im Kanal bei x -7386..-3183
(n23_kanal) - quer >= 900, und er kommt dort nie von weit her gerade an.

**RE2-Form der Liegestelle.** In ROOM7020 liegt der geworfene Raketenwerfer NICHT am Bahnende:
- sub13 (selbst gelesen, re2_scd_walk.py): Speed_set 2/1/0 = 10/20/25 @0x088E-0896, 8/7/6 = 0/20/5 @0x089A-08A2, Do {Evt_next,
  Add_speed, Add_aspeed} while Member_cmp(y < -800) @0x08A6-08B0, Set(5,0x0e) @0x08B6, Se_on 0x0e @0x08BA, **dann Pos_set
  `32 00 b6 9f 00 00 10 aa` @0x08CA = (-24650,0,-21998)** + Dir_set `33 00 00 fc 00 00 00 04` @0x08D2 (liegt).
- Add_speed @0x80055ab0 ist reine Weltaddition (`lw v0,56(a1)` / `addu` / `sw v0,56(a1)` @0x80055abc-c8, +0x3C/+0x40
  @0x80055acc-e0, selbst disassembliert) -> z ändert sich im Flug um v_z 10/Bild (bis y -800 höchstens ~30 Bilder, ~300).
  Ada steht bei z -17900 (`32 00 a0 92 b4 e2 14 ba` @0x0740), die Spielstelle bei z -21998 = 4098 daneben: den Rest legt der
  Pos_set zurück.
- Die Spielstelle liegt im Item-AOT 5 des Raketenwerfers (`4e 05 02 31 00 00 a8 99 60 a5 48 0d c4 09 11 00 ..` SCD-main
  @0x00DA: x -26200..-22800, z -23200..-20700), der nach dem Wurf mit `Aot_reset 5` @0x0822 frei wird - RE2 legt den Gegenstand
  also an die Stelle, an der er im Spiel gebraucht wird.

**[ÜBERHOLT durch FG-E3, 8.9]** **Port (PORT-WAHL der Stelle, Form RE2 @0x08CA):** der Kanister liegt in 2090 an seiner Spielstelle = Mitte der Westkanal-
Wasserzone ROOM2090 main00 **Aot_set Slot 4 sce 8 @0x009FE `2c 04 08 47 00 00 3c dd 4c b9 20 1c 10 0e ..`** (x -8900..-1700,
z -18100..-14500) = **(-5300,-16300)**. Begründung aus dem Raum: die drei sce-8-Wasserzonen @0x09D6/@0x09EA/@0x09FE sind Süd-,
Nordbecken und dieser Kanal; Süd <-> Nord geht im Wasser nur durch den Kanal (Plattform x -1700..1850 z -20000..-12450 und Rampe
dazwischen sind SCA-Blöcke, Spur B gb_zone/Westumlauf-Ecken x -4300) - folgt der Alligator Leon von einem Becken ins andere,
schwimmt er hier geradeaus durch. Der Bahnendpunkt (Aufschlag im 2080-Bild, Ton) bleibt die RE2-Bahn.

### 8.3 M5 - Dauern ohne Beleg (selbst gelesen, alles RE2 info/re2leon/PSX.EXE bzw. RE1.5-RDTs)
| Stelle (alt) | Ursache | Beleg | jetzt |
|---|---|---|---|
| `OP_SLEEP(10)` zwischen zwei Zielen | 10 = geschätzte Drehzeit, zitiert war nur die Rate | Nachdrehen 0xC8/Bild `ori a1,zero,0xc8` @0x80033f90 (Gun-FSM Sub 5) | Poll auf Signal AUSGERICHTET (Bank 5 Bit 0x1E): der Takt setzt es im Bild, in dem der letzte Schritt (|Rest| <= 0xC8) das Ziel erreicht; kein Ziel mehr -> sofort. Gemessen m1_leon: 1-4 Bilder je Ziel (`Ziel Slot 4 ausgerichtet` F151 nach `Ziel 2` F148) |
| `OP_SLEEP(30)` nach dem Tür-Ton | RE2-Wert @0x076A aus anderem Zusammenhang (dort nach Message_on) | ROOM1021 sub02: Work_set @0x020C2 -> Plc_motion(2,6) @0x020C6 -> Sleep 110 @0x020CA, kein Warten davor | entfernt: Signal Tür-Ton, dann direkt Work_set + Anlegen; der Ton läuft in den 110 Bildern (Riegel szene: nach dem Signal folgt `2e`) |
| `OP_SLEEP(30)` in der Rückkehr | RE2 wartet dort mit Wsleep/Wsleeping @0x081A/@0x081B | Wsleeping @0x80053ac0: gibt ab (Rückgabe 2 @0x80053b18), solange `0x800cfbd8 & 0x20` (@0x80053acc) oder `0x800e873c & 0x80` (@0x80053adc-e4); 0x800e873c = 0x80 setzt der Nachrichten-Start FUN_8002fe38 (`addiu t2,zero,128` / `sb t2,-30916(at)` @0x8002fe80/88), hier für Leons Zeile Message_on @0x07E6/@0x07F4 | entfernt: der Port hat diese Zeile nicht -> 0 Bilder; Riegel start prüft die Form ohne Sleep |
| Adas Wurf-Clip `3f 0a 12 40` @0x0750 durch Eigen c6 ersetzt, ohne Vermerk | - | Marvin/John haben keinen Wurf-Clip (Zensus 8.1: NPC-Plc_motion nur (1,7/11/22/23), (2,6/7)) | PORT-WAHL im Header (Formen) und am Opcode benannt: Eigen c6 hebt beide Hände von der Hüfte nach vorn oben = Hochstemmen über die Reling |

### 8.4 M3 - Wurf-Kamera "GENAUSO" (Nachtrag 5): Entscheidungsvorlage (entschieden durch FG-E4, Umsetzung 8.9)
Vorbild RE2 ROOM7020 Cut 7 (RID @RDT+0x24): pos (-22158,-3852,-15174) -> (-28368,-7434,-18234), Ada bei (-28000,-7500,-17900)
mit Blick 0x0e74 (@0x0740/@0x0748): Kamera **43,6 Grad unter** Adas Kopf, Abstand 8908, frontal 0,99 (Dossier 3.4). Alle neun
2080-Kameras gegen den Werfer an der Reling (Kopf (-7168,-16900,-17642), Blick +x = über die Reling; RID @0x60, eigene Rechnung
in diesem Abschnitt, Werte aus ROOM2080.RDT):
| Cut | pos -> Ziel | Höhe zum Kopf | Abstand | frontal | Bahn im Bild (proj_bahn, Abnahme-Werkzeug) |
|---|---|---|---|---|---|
| 0 | (-18060,-16688,-17526) -> (-21498,-7096,-16344) | 1,1 Grad unter | 10895 | -1,00 (Rücken) | nein |
| 1 | (-23516,-11634,-17708) -> (-18791,-13598,-16767) | **17,9 Grad unter** | 17175 | -1,00 (Rücken) | ja, Bild 0-24 |
| 2 | (-4832,-19777,-18507) -> (-14827,-14428,-16815) | 49,1 Grad über | 3806 | 0,94 | nein |
| 3 | (-8446,-18889,-16448) -> (-11806,-16085,-11908) | 48,7 Grad über | 2649 | -0,73 | nein |
| 4 | (-8667,-18479,-15322) -> (-12249,-15302,-22535) | 29,8 Grad über | 3182 | -0,54 | nein |
| 5 | (-12826,-18695,-18043) -> (-5172,-15001,-17158) | 17,6 Grad über | 5949 | -1,00 (Rücken) | ja, Bild 0-36 |
| 6 | (-7315,-17332,-17514) -> (449,-10461,-16726) | 65,7 Grad über | 474 | -0,75 | ja, Bild 0-36 |
| **7 (gebaut)** | (8095,-18819,-9867) -> (-4415,-11874,-13977) | 6,4 Grad über | 17236 | **0,89** | ja, Bild 0-36 |
| 8 (flag 1) | (1728,-7272,-21942) -> (7128,-3942,-16218) | **44,3 Grad unter** | 13796 | 0,90 | **nein** (blickt nach Osten weg) |
In 2090 erreicht die Bahn erst ab Bild 30 von 36 ein Bild (Cut 12, Rand oben). **Keine vorhandene Kamera zeigt den Werfer
frontal von unten**: von unten sieht ihn nur Cut 1 - von hinten (17,9 Grad, 17175 weit); frontal sind nur Cut 2 (von oben, nah)
und Cut 7 (Totale, 6,4 Grad über dem Kopf). Neue Hintergründe werden nicht erfunden (Entscheid B8). Unverändert gebaut: Cut 7 (die
RE2-Schnittfolge Cut_chg 7 @0x0726 -> Wurf -> Aufschlag -> Sleep 15 @0x07A8 -> Schnitt auf den Fangenden wie @0x07D4 bleibt).
**Entscheidungsfrage an Orchestrator/Nutzer (Abschnitt 8.8, Frage 1):** A) Cut 7 frontal von der Grubenseite (gebaut), B) Cut 1
von unten, aber von hinten, C) ein anderer Wunsch. Bis zum Entscheid bleibt M3 offen. **[Entschieden: FG-E4 -> 2090 Cut 12 mit der RE2-Schnittfolge;
Umsetzung und Messung 8.9.]**

**[Die folgenden zwei Absätze gehören zu 8.2 (M2) und sind durch FG-E3 ÜBERHOLT: der Kanister liegt jetzt an der
Aufschlagstelle, derselbe Alligator kämpft weiter (8.9). Stehen gelassen als Herleitung der Becken-Rangfolge.]**

**Wahl der Spielstelle - gemessen (die Kanalmitte oben war die erste Annahme, die Messung hat sie verworfen):**
- Sonde `tools/r41_kanister2080/probe_liegestelle.c` (echter Boss-Tick wie `test_r41_re2gator.c setup(0)`, Zustand nach der
  Szene: untergetaucht am Lauerplatz, Auftauchen < 9000; Leon als Statue an 50 Wasser-Lagen < 9000 vom Lauerplatz; Kanister über
  `re15_gr2_kanister_setzen`; 1500 Bilder): Rasterkarte `G_belege/nb1_m2_liegestellen_raster.txt` - Nordbecken (z >= -13000)
  14-25/50 Aufnahmen, Kanal (z -18000..-14000) 4-15/50, Südbecken 0-10/50; feiner `G_belege/nb1_m2_liegestellen_fein.txt`:
  Plateau 25/50 um (-6500..-5500, -11000..-9500); Kanalmitte (-5300,-16300) 15/50 (`G_belege/nb1_m2_kanalmitte_je_leonlage.txt`).
  Grund: der Alligator taucht nach der Szene nur auf, wenn Leon < 9000 an den Lauerplatz kommt - der weitere Kampf läuft im
  Nordbecken, dort schwimmt er Leon geradeaus an.
- Echte exe (natürlicher Gator, KEIN RE15_GB_TEST, Szene gelaufen = Bits 110/115, Kanister per RE15_GATOR_KANISTER nur zur Wahl
  der Stelle, Leon/Elza schiessen alle ~4 s, 2400 Bilder; Läufe `x_*` im scratchpad g_nb1):
  | Stelle | L1 (-6000,-9000) | L2 (-8400,-6500) | L3 (-2500,-11000) | L4 (-4500,-14000) | L5 (-6000,-17500) | L6 (-4000,-7000) | Elza (-6000,-9000) |
  |---|---|---|---|---|---|---|---|
  | Kanalmitte (-5300,-16300) | - | - | Aufnahme F2315 | - | (E3) F347, R1 F1143 | | |
  | **Nordbecken (-6500,-11000)** | F355 / R1 F890 | F395 / R1 F568 | F715 / R1 F1146 | F859 / R1 F2056 | F474 / R1 F1564 | F332 / R1 F814 | F355 / R1 F890 |
  | Nordrand (-6328,-13600) = per Wurf ohne Versatz erreichbar | - | F325 / R1 F560 | - | - | | | |
  Leon von Süden auf die Kanalmitte zu (Leon nach RE2 @0x07B0 6057 vor den Kanister gestellt, Läufe `d_kanal_*`): 1/4.
- **Gewählt: (-6500,-11000)** (Header RE15_K2080_LIEGE_X/Z mit dieser Herleitung). Abstand zum sichtbaren Aufschlag (-2278,-17282)
  7560 - RE2 versetzt den Raketenwerfer per Pos_set ebenfalls um mehrere tausend (oben); die RE2-Wurfbahn (~4050 Vorlauf bei
  13300 Fallhöhe) erreicht das Nordbecken von der Reling aus nicht (Kreis ~4066 um die Hände, bester erreichbarer Punkt
  "Nordrand" 1/4).

**Messung nachher - die GANZE Folge im echten Ablauf** (Bau 2026-10-25 03:0x, exe-Kopien, KEIN RE15_GB_TEST, KEIN
RE15_GATOR_KANISTER; nur der Auslöser-Messhaken RE15_K2080_GATOR_HP=24990 = ein echter Treffer statt 43; Bits 110 + (3,245);
Leon/Elza schiessen alle ~4 s):
- **fk_leon** (Start (-6000,-17500)): `F902 AUSLOESER ... Treffer 1: Spieler (-5790,0,-6245)` -> Szene 2080 (Hinknien, Wurf
  `F581`, Aufschlag `F617`) -> `F632 RUECKKEHR` -> `ROOM2090 Kanister im Becken (-6500,-1200,-11000) = Spielstelle Nordbecken`
  -> gator_boss.log `GATOR VERSCHWUNDEN`, `RE2 AUFTAUCHEN F6`, **`RE2 KANISTER Stufe 1 laengs=5985 quer=-440 ... F355`**,
  `angehaengt ... F367`, **`RE2 R1 VSync 4 (Zeitlupe) F815`**, `R1 ende (Leiche) pos=(2619,-8209)`. Zeit seit dem Wurf bis zur
  Aufnahme: (632 - 581) + 355 = **406 Bilder = 13,5 s** (2080: Wurf F581, Rückkehr F632; 2090: Aufnahme F355).
  Bild der Aufnahme `G_belege/nb1_m2_aufnahme_fk_leon_f330-399_hell6.png` (Cut 0, Ausschnitt, Helligkeit x6 nur zur Lesbarkeit -
  das Becken zeichnet Spur B sehr dunkel): F378 hebt der Alligator den Kopf mit dem Kanister im Maul; R1
  `G_belege/nb1_m2_r1_fk_leon_f810-870.png` (B's R1-Schnitt Cut 2 ab F816, Blitz, zurück Cut 0 F852).
- **fk_elza** (dasselbe mit Elza, ROOM2091/2081, John): bitgleich dieselbe Folge (Aufnahme F355, R1 F815).
- **fk_leon_nw** (Start (-3500,-12000)): Auslöser F1799, Aufnahme F394, R1 F568, Leiche (-2239,-7005).
- Gegenprobe Kanalmitte (Bau davor, fc1b_leon/fc2_elza): Aufnahme erst F2157, R1 F3171; fc3: keine Aufnahme in 2400 Bildern.
- Grenze (gemessen, nicht verdeckt): fn_leon/fn_elza (Start (-6000,-9000)): der erste Kampf schiebt Leon nach Osten (Auslöser
  bei (-1989,-6363)), danach kämpft der Gator mit ihm im Ostteil (x 1449..6732) - in 2700 Bildern keine Aufnahme. Der Alligator
  lenkt nicht zum Kanister (RE2 lenkt nur den Hals, Hals-Zielmodus FUN_800177c0 ist bei Spur B OFFEN §11.3); eine Liegestelle,
  die JEDEN Kampfverlauf trifft, gibt es nicht (Frage 8.8).

### 8.5 M4 - Doku / Beleg
- Dossier 6.3 berichtigt (p3h mit RE15_GB_TEST=5 ist kein Beleg für das Spiel: der Haken lässt den Boss aufgetaucht an der
  Marker-Lage warten); der echte Ablauf steht jetzt in 8.2 und im Riegel integration_r41_kanister2080_kampf (Testhaken
  ausdrücklich gelöscht).
- k_port[] Bit 114 (tests/unit/test_r36_kartenbits.c): "2080: Knall (Knall-Bank Satz 0) beim ersten Betreten nach Bit 110".
- Haken-Kommentare: scd_room_setup.c "2080 Knall (Knall-Bank Satz 0) + Zombies / Kanister-Szene", platform/pc/main.c "Knall
  (gen/knall_bank.inc Satz 0), Tür-Ton DOOR06, Aufschlag, NPC-Schuss" (je nur der Kommentar der vorhandenen Zeile).
- Überholte Aussagen im Dossier markiert: 1.8 (c2/c0 falsch gedeutet), 3.2 (Wurf-Stand), 3.3 (Ablauf-Tabelle), 7 (Stand).

### 8.6 Tests (gezielt, keine volle Suite; Bau 2026-10-25 04:07 local_build.sh build, ctest mit msys64 zuerst im PATH)
- `ctest --test-dir re15_port/build -R "unit_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_" --timeout 600
  --output-on-failure` -> **`100% tests passed, 0 tests failed out of 34`** (Total Test time (real) = 29.23 sec). Darin
  geändert: unit_r41_kanister2080_szene (Aufnehmen Byte für Byte gegen ROOM11B0 sub06 @0x01650..0x01663, kein (1,2)/(2,0),
  Ausricht-Polls statt Sleep 10, nach dem Tür-Ton direkt Work_set; der Wurfteil endet mit Signal WURF, Sleep 1 und der Tür
  nach 2090 Cut 12), _start (Rückweg: Riegel, Poll auf den Aufschlag wie @0x079C-07A4, Sleep 15 @0x07A8, Schnitt auf Leons
  Cut wie @0x07D4, kein Sleep 30), NEU _landung (Nennbahn Gier 3072 endet im 2090-Wasser: `Aufschlag (-7480,-2076,-12320)
  nach 38 Bilder`, `ROOM2090 Cut 12: 14 von 39 Fall-Bildern im Bild`; der Installer legt den Kanister bei Bit 116 an die
  Aufschlagstelle, 2090 und 2091). Der Riegel _liege (Spielstelle Nordbecken) ist mit FG-E3 entfallen.
- `ctest --test-dir re15_port/build -R "^integration_r41_kanister2080$|^integration_r41_kanister2080_kampf$|^integration_r41_re2gator$"
  --timeout 1800 --output-on-failure` -> **`100% tests passed, 0 tests failed out of 3`** (Total Test time (real) = 263.15 sec):
  - integration_r41_kanister2080 (171 s): szene_leon/elza mit "Kanister aufgehoben (Loco c11 Bild 24 ...)", Plc_motion(1,11)
    zweimal, kein Kletter-/Lauf-Clip, Cut 12 in 2090/2091 nach dem Wurf (state.log), "Kanister im Becken (x,-1200,z) =
    Aufschlagstelle".
  - integration_r41_kanister2080_kampf (263 s): Leon + Elza OHNE RE15_GB_TEST/RE15_GATOR_KANISTER/RE15_KEIN_GATOR/
    RE15_K2080_WURF_GIER (ausdrücklich gelöscht): "FG-E3: Alligator gesichert" -> Auslöser -> Szene (Hinknien, Plc_motion(1,11)
    zweimal) -> Wurf "(Wasser 2090 y -1620)" -> "FG-E3: derselbe Alligator in Slot" -> Rückkehr -> state.log cam=12 ->
    Aufschlag -> "Kanister im Becken ... = Aufschlagstelle" -> gator_boss.log KANISTER gesetzt -> KANISTER Stufe 1 ->
    angehängt -> R1 start -> R1 ende (Leiche), und nach der Szene KEIN "GATOR VERSCHWUNDEN"/"RE2 AUFTAUCHEN"/"INTRO ende".
    In diesem Lauf: Leon Aufnahme F104, R1 F570, Rückschnitt F606; Elza Aufnahme F103, R1 F888, Rückschnitt F924.
  - integration_r41_re2gator (243 s, Spur B's Riegel): grün.

### 8.7 OFFEN (gemessen bzw. benannt, nicht geraten)
1. **Aufnahme in JEDEM Kampfverlauf:** mit der Wurf-Gier 3072 nimmt derselbe Alligator den Kanister in 5 von 6 gemessenen
   Verläufen auf (8.9). Der sechste (Start (-8000,-7000), Auslöser bei (-8432,-13539); der Alligator liegt beim Auslösen schon
   AUF der späteren Aufschlagstelle, (-7311,-12620)): der stehende, nur schiessende Test-Spieler wird vom Alligator-Körper an der
   West- und Südwand entlang bis an die Rampe geschoben (state.log F600 (-8438,-16066) -> F1500 (-8439,-26498) -> F3300
   (3964,-26405) -> ab F6600 (7668,-15549)), der Alligator folgt ihm im Süd- und Ostteil und kommt in **12000 Bildern (400 s)**
   nicht ins Nordbecken zurück (Kanister bei z -12304) - keine Aufnahme (Lauf `lang_L3`). Ursache wie 8.2: B's Aufnahme ist die
   RE2-Form (Fenster am Maul nur in der Verfolgung, Anschwimm-Sperre nur in der Zone des Kanisters, gator_re2_r41.c
   gr2_zielmodus_takt @0x801009c8-0x80100ab4); der Körper lenkt nicht zum Kanister, RE2 erzwingt die Aufnahme über den Korridor
   von ROOM40A0, das 2090-Becken hat keinen. Der RE2-Hals-Zielmodus (Root @0x80100344-60 -> FUN_800177c0) ist bei Spur B OFFEN
   (§11.3). FG-E3 erlaubt markierte Haken in B's Modulen für "Landepunkt/Fenster"; eine Lenkung des Körpers zum Kanister wäre
   neues Verhalten ohne RE2-Beleg und ist darum NICHT gebaut (Frage 8.8.1).
2. **Kamera (M3) per FG-E4 entschieden** (Hinweis an den Nutzer 8.8): 2090 Cut 12 liegt 59,6 Grad unter dem Werfer (RE2 Cut 7
   43,6 Grad), der Werfer selbst ist in Cut 12 nicht im Bild; die Zeile 42 bleibt in 2080 Cut 7 (Text-Id-Zuteilung).
3. Während des Falls in Cut 12 (F1-F57 nach der Rückkehr, Spieler gehalten pm=2) kämpft der Alligator weiter (FG-E3) und schiebt
   den gehaltenen Spieler mit dem Körper (bild_elza: (-5790,-6245) -> F34 (-4479,-5852) -> F58 (-5157,-5868)); kein Biss (gr=0).
   Das Original friert Gegner in Szenen nicht ein (Set(2,7)/(1,27) liest kein Gegner-Code: B_szenenschutz.md 2.1, RE1.5
   @0x800304f4-500 / @0x8001cd0c-20, RE2 @0x800391f8-21c); RE2 versetzt nach dem Wurf aber Leon und den Tyrant (Pos_set
   @0x07B0 / @0x07C4) - der Port lässt die Lagen (FG-E3 "Lage/Zustand bleiben"). Benannt, nicht geändert.
4. Unverändert aus Abschnitt 6: RE2-Auslöser-Mitglieder +0x1D8/+0x1DA (Tyrant-Overlay fehlt), Adas Abgang/Mode 0x14 nicht
   übernommen, Kanister in RE2-Grösse, Ton nur über die Mischkette belegt (kein Audiogerät).
5. Griff-Bild: der Kanister wechselt beim Greifen in einem Bild von stehend auf quer (8.1).

**Hinweise an den Orchestrator (andere Spuren, nicht in G geändert):**
- Spur B: der R1-Tod schiebt die Leiche in mehreren Läufen AUS dem Becken (Aussenwände x -8900..7200, z -27000..-5400): neu
  `R1 ende (Leiche) pos=(10684,-8412)` (Elza, integration_r41_kanister2080_kampf und bild_elza), `(-3482,-1916)` (k1_leon_gm768);
  früher `(-12384,-8319)` (x_nord_L1/E1), `(-4105,-1916)` (x_nord_L2), `(-10006,-1916)` (x_nord_L5), `(-10221,-1916)`
  (x_nordrand_L2), `(-10602,-30484)` (fc1b_leon; state.log F3180 (-7416,-25516) -> F3183 (-7416,-30484) -> F3207
  (-10602,-30484) im Clip 9). Die R1-Wurzelbahn läuft offenbar ohne Wand-Klemme.
- Spur F: in diesem Zweig sind die Spinnen (0x25) noch in 2090 (H4 der Abnahme): Giftkugel-Treffer auf Leon/Elza im ersten Kampf
  (bild_elza debug.log `[giftkugel] F228 ... F900`), eine Spinne steht in Cut 0 neben dem Spieler (Bilder nb1_fg3_*); die
  Zielhilfe trifft teils sie statt des Alligators - die gemessenen R1-Zeiten hängen davon ab.

### 8.8 Für den Nutzer (Stand Nachbesserung 1)
Neue Assets: keine (weiter `re15_port/shared_assets/RE2/KANISTER2080.VBS`, md5 848c40f6c077d937fb025df9ae1bc62b).
Sprachdateien (vom Nutzer aufzunehmen, unverändert): `synchro/STAGE2/room2080/main42.wav` ("Marvin: Hey, Leon, try with this!"),
`synchro/STAGE2/room2081/main42.wav` ("John: Hey, Elza, try with this!").
Was sich im Spiel ändert:
- Marvin (Leon) / John (Elza) rennt zum Kanister, **kniet sich hin, nimmt ihn mit beiden Händen und steht damit auf** (Marvins
  Hinknien/Aufstehen aus ROOM11B0, wo er sich zum Gully kniet) - kein Hochklettern mehr. Danach stemmt er ihn über die Reling.
- Nach dem Tür-Ton legt er sofort an (vorher 1 s Pause); zwischen zwei Zielen dreht er nur so lange nach, wie er braucht.
- **Nach dem Loslassen schneidet das Bild nach 2090 in Kamera 12** (Blick von unten an der Wand unter dem Balkon): dort fällt der
  Kanister ins Becken (Aufschlag-Ton), eine halbe Sekunde danach zurück auf Leon/Elza, dann hat man sofort wieder die Kontrolle.
  **Hinweis (Orchestrator-Entscheid FG-E4):** eine Kamera "genau wie Adas Raketenwerfer-Wurf" (die Werferin von unten) gibt es
  in 2080/2090 nicht - der Port hat nur die vorgerenderten Hintergründe der Original-Kameras; Kamera 12 ist die nächstliegende,
  Marvin/John selbst sieht man darin nicht. Die Zeile "Hey, Leon, try with this!" kommt noch in 2080. Bilder:
  `G_belege/nb1_fg4_re2c7_2090c12_2080c7.png` (RE2 <-> Port), `G_belege/nb1_fg4_fall_cut12_leon_f10-58.png` (der Fall).
- **Der Kampf geht mit DEMSELBEN Alligator weiter** (Lebenspunkte, Lage, kein neues Auftauchen/Intro). Der Kanister liegt dort,
  wo er aufschlägt (Nordbecken); der Alligator schnappt ihn im weiteren Kampf, wenn er dort vorbeischwimmt (gemessen 3,5 s bis
  2 min nach dem Wurf in 5 von 6 Verläufen), ein Treffer dann -> Explosion, Kiefer, Tod.
Entscheidungsfragen:
1. **Aufnahme in jedem Kampf:** in einem von sechs Verläufen schwamm der Alligator nicht mehr ins Nordbecken (er folgte dem
   Spieler in die Südost-Ecke; 400 s ohne Aufnahme). Soll Spur B dem Alligator die RE2-Hals-/Anschwimm-Lenkung zum Kanister
   geben (RE2 FUN_800177c0, B OFFEN §11.3), damit er ihn auch holt, wenn man weit weg kämpft - oder bleibt es wie in RE2 dabei,
   dass man ihn am Kanister vorbeilocken muss?
2. **Kamera 12 (FG-E4):** passt der Blick von unten auf den fallenden Kanister, oder lieber den ganzen Wurf in 2080 Cut 7
   (frontal auf Marvin/John, wie vor FG-E4)?
3. Soll der Kanister beim Greifen sichtbar kippen (RE2-Kippen aus ROOM40A0 sub10) statt in einem Bild quer zu liegen?

### 8.9 Orchestrator-Entscheide FG-E3 / FG-E4 (AUFTRAG.md 2026-10-25 00:56, während dieser Nachbesserung in den Baum gelegt)
Wortlaut-Kern: **FG-E3 (M2)** "Nach der Kanister-Szene setzt der Kampf mit DEMSELBEN Alligator fort (HP, Intro gesehen, Lage/Zustand
bleiben; kein erneuter Durchbruch), und der Kanister landet so, dass der Alligator ihn im normalen Rundlauf aufnimmt (B's
Aufnahme-Fenster)" - Haken in Spur B's Modulen erlaubt. **FG-E4 (M3)** "Gewählt wird die nächstliegende vorhandene Kamera (laut
Abnahme 2090 Cut 12, knapp daneben) mit der RE2-Schnittfolge des Wurfs ... PORT-WAHL im Dossier mit Bildvergleich RE2 <-> Port".
Folge: die Spielstelle Nordbecken (8.2, Relokation nach RE2 Pos_set @0x08CA) ist ÜBERHOLT - der Kanister liegt wieder dort, wo er
aufschlägt; die Messungen 8.2 bleiben als Herleitung stehen (Rangfolge der Becken-Bereiche).

**FG-E3 Umsetzung (ohne Änderung in Spur B's Dateien):** der Boss-Zustand liegt in Datei-Statics von Spur B (`s_gb[slot]`
enemy_ai_boss_gator.c gb_state_t: Phase, aggro, untergetaucht, Routen-Latches; die RE2-Bausteine je Slot in gator_re2_r41.c,
zurückgesetzt nur im Boss-INIT `re15_gr2_reset` beim Neuspawn). Der Raumwechsel löscht nur die Aktoren; der main.c-Portblock setzt
einen neuen Alligator nur, wenn KEIN Typ 0x23 aktiv ist (`_hat`), und der Boss-Takt nullt `s_gb` nur bei `state == 0 && hp <= 0`
(Neuspawn-Erkennung). G kopiert beim Auslösen den Aktor (`ausloesen`: `s_gator = g_actors[slot]`) und legt ihn beim Rückweg im
2090-Installer (scd_room_setup.c-Haken, vor dem Portblock) in DENSELBEN Slot zurück (`gator_zuruecklegen`) - der Boss-Takt sieht
state 1 / hp < 25000 und die alten Statics und kämpft weiter (kein INIT, kein Untertauchen, kein Intro).

**FG-E4 Umsetzung:** RE2 sub12/sub13 (8.4, 1.2) auf 2080 + 2090 verteilt: in 2080 Cut 7 (frontal) Plc_motion(2,6) @0x0750 ->
Message_on 42 @0x0764 -> Sleep 30 @0x076A -> Plc_motion(2,6) @0x0782 -> Sleep 20 @0x0786 -> Loslassen (Super_reset @0x078E) - die
Zeile muss in 2080 stehen (Text-Id 42 ist G nur in 2080/2081 zugeteilt, 2090 42-46 gehört F). Mit dem Loslassen der Raumwechsel
(Door_aot_set ohne Sequenz, 1 Bild) nach 2090 **Cut 12** (RID @0x60+12*32: pos (-1601,-2131,-11018) -> (-5184,-3926,-13767),
flag 1; 59,6 Grad unter dem Werfer-Kopf, RE2 Cut 7 43,6 Grad - Bildvergleich `G_belege/nb1_fg4_re2c7_2090c12_2080c7.png`: links
RE2 ROOM70207 (Ada-Laufgang von unten), Mitte 2090 Cut 12 (Wand unter dem Balkon von unten, oben links die Reling), rechts der
bisherige 2080 Cut 7); dort fällt der Kanister zu Ende (RE2 sub13-Bahn, Landefläche 2090-Wasser y -1620 = Aot_set sce 8 @0x009D6
p1 `ac f9`, Stopp 800 darüber @0x08B0), Aufschlag-Ton, dann wie RE2 Warten auf den Aufschlag @0x079C-07A4, Sleep 15 @0x07A8,
Schnitt auf den Fangenden @0x07D4 (= Leons Cut beim Auslösen), Plc_ret/Riegel los/Cut_auto (sub18 @0x0A2C-0A38). Der Werfer ist in
Cut 12 nicht im Bild (Balkonende y -14..-36 über dem Bildrand, eigene Projektion `g_nb1/src/projpt.c`).

**Wurf-Richtung (PORT-WAHL, gemessen; Header RE15_K2080_WURF_GIER = 3072).** RE2 wirft in Weltachsen nach +x (Add_speed =
Weltaddition @0x80055abc-e0); die Richtung über die 2080-Reling ist im Port frei, die RE2-Bahn v (25,20,10) a (5,20,0)
@0x088E-08A2 wird um die Gier des Werfers gedreht. Gemessen an der GANZEN Folge im echten Ablauf (kein RE15_GB_TEST, kein
RE15_GATOR_KANISTER; der Messhaken RE15_K2080_WURF_GIER nur zur Wahl; Auslöser-Messhaken RE15_K2080_GATOR_HP=24990 wie im
Riegel; Leon/Elza schiessen alle ~4 s; 4200 Bilder nach der Rückkehr; scratchpad `g_nb1/runs/k1_*`, `k3_*`, `g1024_*`,
`g896_*`, `lang_L3`). Start -> Auslöser-Lage: k1 (-6000,-17500) -> (-5790,-6245); k3 (-3500,-12000) -> (-8432,-5868);
L1 (-6000,-9000) -> (-1989,-6363); L2 (-2500,-11000) -> (-990,-11602); L3 (-8000,-7000) -> (-8432,-13539).

| Gier | Aufschlag | Aufnahme / R1 (Bilder nach der Rückkehr) |
|---|---|---|
| 1024 (Blick -z, Südbecken, Stand vor FG-E3) | (-6821,-22914) | k1 Leon: keine; k1 Elza: keine |
| 640 | (-4488,-22449) | k1: keine |
| 0 (+x wie RE2) | (-1863,-17262) | k1: keine |
| 3328 | (-6114,-13047) | k1: F336 / F581 |
| 3200 | (-7162,-12992) | k1: F105 / F245; L1: F1324 / - ; L2: keine; L3: F261 / F832 |
| **3072 (Blick +z, Nordbecken) = gewählt** | **(-7515,-12304)** | **k1: F104 / F570; k3: F619 / F833; L1: F3675 / F3882; L2: F3130 / F3454; L3: keine (auch in 12000 Bildern nicht, 8.7.1); Elza (k3-Start): F619 / F833** |
| 2816 | (-9586,-13287) | verworfen: ausserhalb des Beckens (x < -8900) |

Sichtbarkeit in 2090 Cut 12 (eigene Projektion wie der Zeichner, `g_nb1/src/proj12b.c`, Riegel unit landung): Gier 3072 zeigt
14 von 39 Fall-Bildern (Bild 24..37), der Aufschlag selbst liegt knapp unter dem Bildrand (y 247); die Südwürfe (Gier 768..1280)
zeigen 17-19 Bilder samt Aufschlag, werden aber im weiteren Kampf nicht aufgenommen. Gewählt nach dem Abnahmepunkt (Aufnahme im
echten Ablauf); die Sichtbarkeit (>= 10 Bilder) hält der Riegel unit_r41_kanister2080_landung.

**Messung nachher FG-E3/FG-E4** (Bau 2026-10-25 04:07, exe-Kopien re15_pc_g41nb1*.exe, Gier 3072 aus dem Header, Rezept des
Riegels kampf mit Bildern 2..640/2):
- Leon (Lauf `bild_leon`): `F902 FG-E3: Alligator gesichert Slot 35 (-4889,-1200,-7442) Gier 2365 hp 24980 Phase 2` ->
  Auslöser -> 2080 `F465 Kanister aufgehoben (Loco c11 Bild 24 ...)` -> `F581 Wurf: Start (-7135,-16896,-16769)
  v=(-10,20,25) a=(0,20,5) -> Landung (-7515,-2076,-12304) nach 38 Bildern (Wasser 2090 y -1620)` -> `F581 FG-E3: ROOM2090
  derselbe Alligator in Slot 35 (-4889,-1200,-7442) Gier 2365 hp 24980 Phase 2` -> 2090 state.log cam=12 F1..F57 -> `F43
  Aufschlag im Wasser bei (-7515,-2076,-12304) Bild 38` -> `F43 ROOM2090 Kanister im Becken (-7515,-1200,-12304) =
  Aufschlagstelle (Bit 116)` -> cam=0 ab F58 -> gator_boss.log `KANISTER gesetzt`, `Zielmodus 1 (Anschwimm-Sperre) ... F102`,
  **`KANISTER Stufe 1 laengs=5782 quer=428 sub 2 -> 1 F104`**, `angehaengt ... F116`, **`R1 VSync 4 (Zeitlupe) F570`**,
  `R1 start pos=(-7294,-6995) hp=24973`, `KAMERA R1 zurueck -> Cut 0 F606`, `R1 ende (Leiche)`. Nach der Szene KEIN
  `GATOR VERSCHWUNDEN`/`AUFTAUCHEN`/`INTRO ende` (diese Zeilen stehen nur im ersten Besuch vor dem Auslöser). Zeit vom
  Loslassen bis zur Aufnahme: 1 + 104 = **105 Bilder = 3,5 s**.
- Elza (Lauf `bild_elza`, ROOM2091/2081, John): dieselbe Folge, Wurf-Start (-7126,-16865,-16741) -> Aufschlag
  (-7506,-2045,-12276), Aufnahme F103, angehängt F115, R1 F888, Rückschnitt F924.
- Bilder: der Fall in Cut 12 `G_belege/nb1_fg4_fall_cut12_leon_f10-58.png` / `G_belege/nb1_fg4_fall_cut12_elza_f10-58.png`
  (F24/F28 schwimmt derselbe Alligator dicht an der Kamera vorbei, F32..F42 fällt der Kanister von oben rechts zur Wasserlinie,
  bis F58 Cut 12, der gezeichnete Rückschnitt folgt F59/F60); die Aufnahme
  `G_belege/nb1_fg3_aufnahme_leon_f100-134_cut0_zoom5_hell6.png` / `G_belege/nb1_fg3_aufnahme_elza_f100-134_cut0_zoom5_hell6.png`
  (Cut 0, Ausschnitt x 236..320 y 56..106 = die Ecke des Nordbeckens, 5x, Helligkeit x6 nur zur Lesbarkeit: F100..F118 schwimmt
  der Kopf heran, F120..F126 hebt er den Kanister (grau, gelbe Enden) aus dem Wasser); der Tod
  `G_belege/nb1_fg3_r1_leon_f566-630.png` (B's R1-Schnitt Cut 5 ab F570, Explosion F574/F578, zurück Cut 0 F606).

## 9. Nachbesserung 2 (Abnahme 1 `G_abnahme_1.md`, 2026-10-25)
Baum wie oben, Start auf 194bab1e (Abnahme-1-Commit; Code-Stand = ebe8445f). Maengel N1-N3 der Abnahme 1; die erfuellten Punkte
(P1, P2, Ausloeser, Kopfschuesse, Wurf/Aufschlag, FG-E3, FG-E4-Kamera, R1) bleiben, ihre Riegel bleiben gruen.
Laufordner/Bilder dieser Nachbesserung: `<SP>/g2` (scratchpad der Sitzung, nicht im Baum); Bildbelege im Baum unter `G_belege/nb2_*`.

### 9.0 Plan / Stand
- [x] N1 RE (RE2 Wsleep/Wsleeping, Raumwechsel beendet Zeile/Stimme), Umsetzung, Messung Leon + Elza, Riegel Standzeit (9.1, 9.4)
- [x] N2 RE (RE2 Dir_set nach Cut_chg 7), Umsetzung, Bildbogen F438-F530 des End-Baus Leon + Elza, Riegel Gier im Griffbild (9.2, 9.4)
- [x] N3 Entscheid zu Frage 8.8.1 (nach RE2), Umsetzung, Messung v1 / v3+v5 / v2+v4 / kampf / L3 (Leon + Elza) mit Bild der
      Aufnahme und Zeit seit dem Wurf, Riegel mit zwei weiteren Startlagen + Boss-Takt-Riegel (9.3, 9.4)
- [x] Tests gezielt (9.5), Dossier 9.6/9.7, Abschluss-Commit

### 9.1 N1 - Zeile 42 wird vom Raumwechsel abgeschnitten
**Messung vorher** (Abnahme 1 §3.1/§3.3, Laeufe s_leon / k_leon / ctest lauf_0d602269, Bau ebe8445f): state.log
`msg(a=1 fsm=1 id=42` in 52 Bildern F530..F581, in 2090 ab F1 `a=0` (Leon und Elza). Die volle Standzeit der Zeile ist 99 Bilder
(Abnahme 0, Lauf p3r_leon F569..F667: die Zeile endete dort von selbst). Ursache: `baue_szene` schreibt nach Signal WURF nur
`Sleep 1` und die Tuer nach 2090 Cut 12.

**RE-Belege (selbst disassembliert, `re2_disasm.py` auf info/re2leon/PSX.EXE; SCD mit `re2_scd_walk.py` auf ROOM7020.RDT):**
- RE2 ROOM7020 sub12 (SCD-main @RDT 0x1128): `29 07` Cut_chg 7 @0x0726 · `2e 02 00` Work_set(2,0) @0x073C · `32 00 a0 92 b4 e2 14 ba`
  Pos_set @0x0740 · `33 00 00 00 74 0e 00 00` Dir_set @0x0748 · `3f 0a 12 40` Plc_motion @0x0750 · `59 00 00 00` Xa_on 0 @0x0760 ·
  `2b 00 05 00 00 00` Message_on 5 @0x0764 · `09 0a 1e 00` Sleep 30 @0x076A · Pos_set/Dir_set erneut @0x0772/@0x077A · Plc_motion
  @0x0782 · `09 0a 14 00` Sleep 20 @0x0786 · `49 00 40 fb 40 02 00 04` Super_reset @0x078E · `04 ff 18 0d` Evt_exec sub13 @0x0796 ·
  **`0b` Wsleep @0x079A · `0c` Wsleeping @0x079B** · `11 00 08 00` / `02` / `12 04` / `21 05 0e 00` Do/Evt_next/Edwhile Ck(5,0x0e)
  @0x079C-07A4 · `09 0a 0f 00` Sleep 15 @0x07A8 · Leon Work_set(1,0) + Pos_set + Dir_set @0x07AC-07B8 · Tyrant Pos_set/Dir_set
  @0x07C0-07CC · `29 0b` Cut_chg 11 @0x07D4.
- Wsleep (Tafel @0x800a74c8 [11] = 0x80053a8c): `lb v1,2(a0)` / `lbu v0,8(v1)` / `addiu v0,v0,1` / `sb v0,8(v1)` @0x80053a8c-a4
  (Zaehler +1), pc + 1 @0x80053aa8-b4, `jr ra` / `addiu v0,zero,1` @0x80053ab8/bc.
- Wsleeping ([12] = 0x80053ac0): `lw v0,-1064(v0)` = 0x800cfbd8 @0x80053ac4, `andi v0,v0,0x20` @0x80053acc, `bne ... 0x80053b14`
  @0x80053ad0; `lbu v0,-30916(v0)` = 0x800e873c @0x80053adc, `andi v0,v0,0x80` @0x80053ae4, `bne ... 0x80053b14` @0x80053ae8;
  sonst pc + 1 / Zaehler - 1 @0x80053af0-b10; beide Wege `jr ra` / `addiu v0,zero,2` @0x80053b14/18 (Bild abgeben).
  - **0x800cfbd8 & 0x20 = die Sprachspur (XA) laeuft:** gesetzt NUR vom Xa_on-Handler ([0x59] = 0x80058028: `jal 0x800129b4`
    @0x80058048, `ori v0,v0,0x20` / `sw v0,0(v1)` @0x80058060/64), geloescht von den XA-Stopp-Wegen (`li a1,-0x21` / `and` /
    `sw` @0x80012914-20, @0x80012cc8-d4, @0x80012f90-9c; Ghidra-Dump-Suche ueber alle Schreiber von DAT_800cfbd8 mit Maske 0x20).
  - **0x800e873c & 0x80 = eine Nachricht steht:** einziger Setzer FUN_8002fe38 (`li t2,0x80` / `sb t2,-30916(at)` @0x8002fe80/88),
    geloescht @0x8002c68c (FUN_8002c610) und @0x80025890.
  -> RE2 wartet an dieser Stelle, bis Adas Stimme UND Adas Zeile zu Ende sind, erst dann der Aufschlag-Poll und der Schnitt.
- **Ein Raumwechsel beendet Zeile und Stimme** (deshalb kann die Zeile nicht ueber den Wechsel nach 2090 stehen bleiben):
  RE2-Raumlader FUN_80049e48 liest die neue RDT `jal 0x80012fb8` @0x8004a1c4 -> CdlPause (`addiu a0,zero,9` @0x800130d4 vor
  `jal 0x8008a380`), CdlSetmode mit dem Modusbyte @0x8009a429 = `a0` (`addiu a0,zero,14` @0x800130f0; Bit 0x40 XA-ADPCM aus),
  CdlReadN (`addiu a0,zero,6` @0x80013140) - danach erreicht keine XA-Stimme mehr die SPU. RE1.5-Raumlader reloziert die
  Zeiger des RDT-Kopfs (auch den Nachrichtenblock) auf die NEUE RDT (`addiu v1,v0,8` / `lw a0,0(v1)` / `addu v0,a0,v0` /
  `sw v0,0(v1)` Schleife bis +96 @0x800397fc-834); im Port entsprechend `re15_msg_clear_room_block` und
  `re15_audio_stimmen_entladen` beim Entladen (entladen_pc.c Schritte 5 und 9). Text-Id 42 gehoert in 2090 der Spur F (VERTRAG 1.2).

**Wahl (mit Beleg):** RE2 laesst los, startet den Fall und wartet DANN auf das Zeilenende. Im Port ist das Loslassen an den
Raumwechsel gebunden (FG-E4: der Fall laeuft in 2090 Cut 12), und der Raumwechsel beendet Zeile und Stimme (oben). Die RE2-Regel
"erst weiter, wenn Stimme und Zeile zu Ende sind" laesst sich deshalb nur VOR dem Raumwechsel halten: Wsleep/Wsleeping steht im
Port zwischen Sleep 20 (@0x0786) und dem Loslassen (Super_reset @0x078E). Die Reihenfolge "Zeile zu Ende, dann der Wurf" ist
zugleich die des Nutzer-Wortlauts ("sagt so etwas wie - 'Hey, Leon, try with this!' Dann schmeisst er den Gaskanister die Reling
runter"). Port-Form: neues Signal WSLEEP (Bank 5 Bit 0x1F, Raum-Scratch wie die anderen Signale) - der Bildtakt setzt es, sobald
keine Nachricht mehr steht (`g_scd.message_active` = 0, entspricht 0x800e873c & 0x80; schliesst den Nutzer-Nachhall einer
laengeren Sprachaufnahme ein) und keine Sprachaufnahme mehr laeuft (`g_re15_voice_laeuft`/`g_re15_voice_restbilder`,
entspricht 0x800cfbd8 & 0x20); das Programm pollt es (Do/Evt_next/Edwhile wie @0x079C-07A4, mindestens ein Bild wie Wsleeping,
das immer mit 2 zurueckkehrt).
**Aenderung:** kanister2080_r41.c `baue_szene`: nach `09 0a 14 00` (Sleep 20 @0x0786) `22 05 1f 00` (Signal WSLEEP loeschen) und
`11 00 08 00 02 00 12 04 21 05 1f 00` (Do/Evt_next/Edwhile Ck(5,0x1F)), dann erst `22 05 1b 01` (WURF = Super_reset @0x078E). Takt
(Raum 2080/2081, Szene laeuft): Bit 0x1F = 1, sobald `g_scd.message_active` = 0 und keine Sprachaufnahme laeuft; Messzeilen
"Zeile 42 stand Fa..Fb (n Bilder)" und "Wsleeping frei (RE2 @0x079B ...)". Notbremse fuer einen haengenden Sprachkanal 300 Bilder
nach dem Zeilenende = derselbe Deckel wie die Port-Stimmenschranke (scd_vm.c RE15_VOICE_NOTBREMSE, msg_common.c
RE15_UNTERTITEL_NACHHALL_DECKEL; RE2 hat keinen). Header RE15_K2080_SIG_WSLEEP.

### 9.2 N2 - Gier-Sprung um 98 Grad im Griffbild
**Messung vorher** (Abnahme 1 §3.1, Laeufe s_leon / s_elza, Bau ebe8445f): Slot 9 F464 `mo=11 af=24 r=93`, F465 `r=3072`, mitten
in Cut 5 (Bild `<SP>/g1/n2_griffbild_gier_sprung_x3.png`). Ursache: `re15_kanister2080_r41_tick` setzt ab dem Greifen jedes Bild
`n->rot_y = wurf_gier()` (seit d0c4f479).
**RE-Beleg:** RE2 dreht Ada erst NACH dem Schnitt: Cut_chg 7 @0x0726, dann Work_set(2,0) @0x073C, Pos_set @0x0740 und Dir_set
`33 00 00 00 74 0e 00 00` @0x0748 (und noch einmal @0x077A), alles im ersten Bild von Cut 7 (dazwischen kein Sleep/Evt_next).
RE1.5-Dir_set (Tafel @0x800744a8[0x33] = 0x80041080): `lw a1,340(a0)` (Arbeitsobjekt) @0x80041084, `lhu v0,2(v1)` / `sh v0,104(a1)`,
`lhu v0,4(v1)` / `sh v0,106(a1)`, `lhu v0,6(v1)` / `sh v0,108(a1)` @0x80041088-a8, pc + 8 @0x800410a4/b4 (Port scd_vm.c op_dir_set:
rot_x/rot_y/rot_z des Work_set-Aktors).
**Aenderung:** das Programm schreibt nach `29 07` (Cut_chg 7) `33 00 00 00 <Wurf-Gier LE16> 00 00` (Dir_set auf den Werfer; Work_set(2,
NPC) = Aktor 9 steht seit dem Anlegen, scd_vm.c scd_work_bind kind 2 -> idx + 1); der Takt (`re15_kanister2080_r41_tick`) setzt
die Gier nicht mehr, er haengt nur noch den Kanister an die Haende. Bis zum Schnitt behaelt der Werfer die Gier des Laufs
(Hinknien/Aufstehen mit Blick auf den Kanister), die Wurfbahn rechnet beim Loslassen mit der Gier aus dem Dir_set.

### 9.3 N3 - Aufnahme im weiteren Kampf (Frage 8.8.1)
**Messung vorher** (Abnahme 1 §3.4, Laeufe `<SP>/g1/runs/*`, eigene Auswertung mit `gator_bahn.py` ueber state.log, Kanister
(-7515,-12304)): v1 natuerlich (37 Treffer, Ausloeser F7375, Alligator SO-Ecke (5782,-26532)) - in 9000 Bildern nie naeher als
13098 (F5371 (-6513,-25364)), der Kampf bleibt im Sued-/Ostteil (F600 (5251,-24990), F3600 (2700,-23300), F7200 (5419,-25532));
v5/v3 Start West (Ausloeser bei Leon (-8432,-10447), Alligator (-4940,-9716)) - naechstens 1853 bei F47 (der Kanister landet
erst F43, der Alligator steht schon daneben), danach Nordost (F1800 (4105,-10063), F3000 (5681,-6939)); v2/v4 Start (0,-7000)
(Alligator (5716,-7090)) - Aufnahme erst F3835. Gemeinsame Ursache: die Landestelle ist fest (Gier 3072), und der Alligator
verfolgt nur den Spieler; liegt der Kanister nicht zufaellig auf seinem Weg, kommt er nicht vorbei.

**RE-Belege (selbst gelesen, `re2_disasm.py --bin EM23_OVL_0000.BIN`, `re2_scd_walk.py` ROOM40A0.RDT SCD-main @RDT 0x25C0):**
- RE2s Gator lenkt den Koerper NIE (Spur B §2.2): er schwimmt seine Korridorbahn, z. B. `addiu v0,zero,-23078` / `sw v0,56(s0)`
  @0x80101048-4c (x der Bahn), Richtungswechsel nur nach der Zonen-Lunge `yaw += 1024` @0x80100eb0-c0.
- Der Kanister landet AUF dieser Bahn: ROOM40A0 sub10 @0x0656 Fall `2f 00 d8 ff` (v_x -40) @0x0672 + For 23 @0x0676,
  Aufschlag-Se_on @0x0686, Treiben `2f 00 dc ff` (v_x -36) @0x06A2 + For 12 @0x06AA + For 4 @0x06BC (16 x 36 = 576 zur Bahn hin),
  Liegestelle sub05 Pos_set `32 00 e0 a6 40 f8 e4 da` = (-22816,-1984,-9500) @0x0484 = 262 neben x -23078.
- Zielmodus +0x1C0 & 4 (Sub-1/2-Pre, Spur B §11.0): an bei Bank 5 Bit 1 und Stufe < 2 (@0x80100a34-5c), Ziel (x, y - 800,
  obj[1].z) @0x80100a4c-70; aus, wenn obj[1].z - z < 9000 - 120 (`addiu v0,v0,-9000` / `slti v0,v0,-120` @0x80100a88-8c) und
  Stufe 0; solange er an ist, schaerft FUN_801012fc den Biss nicht (`lbu v0,448(a0)` / `andi v0,v0,0x4` / `bne` @0x8010134c-58).
- Fenster der Aufnahme nur laengs der Bahn: `lw v1,64(s0)` / `subu` / `addiu v0,v0,-8601` / `sltiu v0,v0,0x18f` @0x80100ad8-e8,
  Stufe 1 `sb v0,550(s0)` @0x80100b08.
-> In RE2 holt der Alligator den gefallenen Kanister IMMER beim naechsten Anschwimmen, und vorher beisst er nicht: der Kanister
  liegt auf der Bahn, die er ohnehin schwimmt.

**Entscheid zu Frage 8.8.1 (Regel "Original oder nicht", Memory reai-v2-original-oder-nicht - keine Wahlfrage an den Nutzer,
wo das Original die Antwort gibt):** Im offenen 2090-Becken hat der Port-Gator keine feste Bahn - seine Verfolgung ist das
Nutzer-Design der Spur B (enemy_ai_boss_gator.c "VERFOLG Ring um die Plattform"). Die RE2-Eigenschaft "der Kanister liegt auf der
Bahn, er wird beim naechsten Anschwimmen genommen, vorher kein Biss" laesst sich hier nur halten, wenn die Bahn ueber den Kanister
fuehrt: solange der geworfene Kanister frei liegt (Stufe 0) und mindestens 8880 x Zeichen-Skala entfernt ist (Ende des
Zielmodus @0x80100a88-8c), ist er das Steuerziel der Verfolgung (Kanten-Umweg, Klemmflaeche, Wende und Vortrieb wie die
Verfolgung, kein Biss). Unter 8880 uebernimmt die normale Verfolgung; das Fenster (Spur B) nimmt ihn dann aus dem Anlauf.
Die Alternative "Landestelle nach der Kampflage allein" reicht nicht: alle Wuerfe von der Reling landen auf einem Kreis von
~5330 um den Werfer (Westteil des Beckens); v1 (Kampf in der SO-Ecke) erreicht kein Landepunkt. Die Alternative "bleibt wie
in RE2 dabei, dass man ihn vorbeilocken muss" ist nicht RE2: dort lockt niemand, die Bahn fuehrt ihn hin.
Zusaetzlich (FG-E3 "der Kanister landet so, dass der Alligator ihn ... aufnimmt"): die Wurfrichtung wird beim Bau der Szene aus
den zwei gemessenen Wuerfen (Tabelle 8.9: Nord 3072 -> (-7515,-12304), Sued 1024 -> (-6821,-22914)) so gewaehlt, dass der
Landepunkt WEITER vom Alligator beim Ausloesen liegt - er braucht den Anlauf (>= 8880 voraus). Bit 117 merkt die Wahl fuer den
Raumaufbau ohne Laufzeit-Bahn.
Dies ist ein Entscheid der Spur G nach RE2 (Orchestrator/Nutzer koennen widersprechen); FG-E3 erlaubt kleine, markierte Haken in
Spur B's Modulen fuer "Landepunkt/Fenster", VERTRAG 1.3 G-Haken in enemy_ai_boss_gator.c.

**Umsetzung:** kanister2080_r41.c `re15_k2080_r41_anschwimmen` / `_aktiv`, `re15_k2080_wurf_gier_wahl`, `nenn_landung`, Header
RE15_K2080_ANSCHWIMM_AB (9000 - 120), RE15_K2080_WURF_GIER_NORD/_SUED, RE15_K2080_BIT_SUEDWURF 117; Haken enemy_ai_boss_gator.c:
Include (1 Zeile), Export `re15_gator_boss_steuerziel` (gb_kanten_umweg + Klemmflaeche der CHASE-Phase, 4 Zeilen), CHASE nach
der Aufnahme-Lenksperre `if (re15_k2080_r41_anschwimmen(slot, e, dist)) break;`, GUARD `oben = 0` waehrend des Anschwimmens.
Nur fuer den Kanister DIESER Szene (Bit 116, im Raum gelegt) - Spur B's eigene Riegel (RE15_GATOR_KANISTER-Weg) sehen den Haken
nicht (Gegenprobe im Riegel anschwimmen).

### 9.4 Messung nachher (Bau 2026-10-25 06:46, exe-Kopie re15_pc_g41nb2a.exe; Rezept des Riegels kampf, KEIN RE15_GB_TEST /
RE15_GATOR_KANISTER / RE15_KEIN_GATOR / RE15_K2080_WURF_GIER; Ausloeser-Messhaken RE15_K2080_GATOR_HP=24990 wo angegeben; Laeufe
`<SP>/g2/runs/<Lauf>`, Auswertung `<SP>/g2/auswertung.py`; Zeit seit dem Loslassen = 1 Bild bis zum Raumwechsel + Bild der Aufnahme
im Rueckkehr-Besuch)

| Lauf | Start | Ausloeser | Wurf-Gier -> Landung | Zeile 42 (state.log a=1) | Gier Slot 9 | ANSCHWIMMEN an | Aufnahme (Stufe 1) | R1 / Leiche |
|---|---|---|---|---|---|---|---|---|
| k_leon (Riegel kampf) | (-6000,-17500) | F902, Haken HP, Alligator (-4889,-7442) | 1024 Sued -> (-6821,-22914) | 99 Bilder F530..F628, Wsleeping frei F630, Wurf F631 | 93 ab dem Hinknien F440 bis F529, F530 (Cut 7) 1024 | F43, Abstand 15325 | **F113 = 114 Bilder = 3,8 s** nach dem Loslassen | R1 F826, Leiche (-5502,-11705) |
| k_elza (John) | (-6000,-17500) | F902, (-4889,-7442) | 1024 -> (-6830,-22942) | 99 F530..F628, Wurf F631 | 93 -> F530 1024 | F43, 15354 | **F113 = 3,8 s** | R1 F826 |
| **v1_natur_leon (ohne jeden Testhaken)** | (-6000,-17500) | **F7375, 37 echte Treffer**, Alligator (4664,-24479) (SO) | 3072 Nord -> (-7515,-12304) | 99 F530..F628, Wurf F631 | 93 -> F530 3072 | F50, 16925 | **F146 = 147 Bilder = 4,9 s** (vorher: keine Aufnahme in 9000 Bildern) | R1 F1549, Leiche (-6149,-20637) |
| v5_west_leon | (-8000,-16000) | F802, Haken HP, (-4940,-9716) | 1024 -> (-6821,-22914) | 99, Wurf F631 | 93 -> 1024 | F43, 13195 | **F90 = 3,0 s** (vorher keine in 4000) | R1 F814, Leiche (-4670,-18747) |
| v3_west_elza | (-8000,-16000) | F802, (-4940,-9716) | 1024 -> (-6830,-22942) | 99, Wurf F631 | 93 -> 1024 | F43, 13222 | **F90 = 3,0 s** (vorher keine in 4000) | R1 F814 |
| v2_no_leon | (0,-7000) | F876, Haken HP, (5716,-7090) (NO) | 1024 -> (-6821,-22914) | 99, Wurf F631 | 93 -> 1024 | F43, 19719 | **F154 = 5,2 s** (vorher F3835) | R1 F899, Leiche (5716,-9033) |
| v4_no_elza | (0,-7000) | F876, (5716,-7090) | 1024 -> (-6830,-22942) | 99, Wurf F631 | 93 -> 1024 | F43, 19747 | **F154 = 5,2 s** (vorher F3835) | R1 F899 |
| lang_L3 (Fall des Bau-Agenten) | (-8000,-7000) | F950, Haken HP, (-7264,-6972) | 1024 -> (-6821,-22914) | 99, Wurf F631 | 93 -> 1024 | F43, 15994 | **F122 = 4,1 s** (vorher keine in 12000) | R1 F679, Leiche (-712,-10966) |

- In allen acht Laeufen zwischen "KANISTER gesetzt" und "KANISTER Stufe 1" keine einzige BISS/LUNGE-Zeile im gator_boss.log
  (RE2: kein Biss, solange der Zielmodus steht, @0x8010134c-58).
- Kein "GATOR VERSCHWUNDEN"/"RE2 AUFTAUCHEN"/"INTRO ende" nach der Szene (FG-E3 unveraendert); "FG-E3: ROOM2090 derselbe Alligator
  in Slot 35" mit der Lage/HP vom Ausloesen in jedem Lauf.
- N2 in Zahlen: in Cut 5 aendert sich die Gier des Werfers nach dem Hinknien (F440) in keinem Bild mehr; der Wechsel auf die
  Wurf-Gier faellt in das erste Bild von Cut 7 (F530), dem Bild des Schnitts - vorher F464 -> F465 mitten in Cut 5.
- N1 in Zahlen: die Zeile 42 steht jetzt ihre volle Zeit (99 Bilder wie in Abnahme 0, vorher 52), das Loslassen folgt im Bild nach
  "Wsleeping frei"; der Fall laeuft weiter in 2090 Cut 12 (state.log cam=12 F1..F57, Aufschlag F43).

**Bildbelege (Laeufe mit RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1; gleiche Bilder wie die Tabelle, die Laeufe sind bildgenau
wiederholbar):**
- N2 `G_belege/nb2_n2_griff_marvin_f438-530_cut5_x2.png` / `G_belege/nb2_n2_griff_john_f438-530_cut5_x2.png` (Laeufe s3_leon /
  s3_elza, Cut 5, Ausschnitt 2x): F438-F446 steht er mit dem Ruecken zur Kamera an der Reling, F450-F462 kniet er (der stehende
  Kanister ueber ihm), F466-F510 haelt er ihn quer, F514-F526 steht er auf - in JEDEM Bild von hinten, kein Profil mehr (vorher
  F466 im Profil, Abnahme 1 `n2_griffbild_gier_sprung_x3.png`). F530 ist das Apply-Bild des Schnitts auf Cut 7: der Port dieses
  Zweigs zeichnet darin noch den alten Hintergrund mit dem neuen Zustand (gedreht, Zeile 42) - s. `G_belege/nb2_n2_schnitt_
  applybild_leon_f526-534.png` (F526/F528 Cut 5 von hinten, F530 Cut-5-Hintergrund + Profil + Zeile, F532 Cut 7). Das Original
  zeigt dieses Bild nicht: im Apply-Bild eines Kamerawechsels bleibt das vorige Bild stehen (Present FUN_8002137c @0x80021558
  `jal 0x80021bbc` + @0x80021560 `j 0x800215fc`, Spur E Nachbesserung 2 E-N1, auf master als kamerahalt_r41.c / bildhalt_r41_pc.c
  - dieser Zweig steht auf 9f10bdb0 ohne Spur E). Nach der Integration mit E bleibt F529 (von hinten) stehen, bis Cut 7 da ist.
- N1 `G_belege/nb2_n1_zeile42_leon_f530-630.png` / `..._elza_...` (Cut 7, je 10 Bilder): "Marvin: Hey, Leon, try with this!" bzw.
  "John: Hey, Elza, try with this!" steht F530-F620, F630 ist sie weg; das Loslassen folgt F631.
- N3 Fall: `G_belege/nb2_n3_fall_cut12_leon_f6-58.png` / `..._elza_...` (2090 Cut 12; der Suedwurf faellt F30-F42 von oben links
  zur Wasserlinie - 22 von 39 Fall-Bildern im Bild statt 14 beim Nordwurf, unit landung).
- N3 Aufnahme (Helligkeit x2 nur zur Lesbarkeit): kampf `G_belege/nb2_n3_aufnahme_kampf_leon_f60-138_cut0_hell2.png` /
  `..._elza_...` (Cut 0: der Alligator schwimmt an der Westwand nach Sueden, F126-F132 hebt er den Kanister - gelb/grau - aus dem
  Wasser); v1 `G_belege/nb2_n3_aufnahme_v1natur_leon_f112-184_cut1_hell2.png` (Cut 1, Kanister im Maul F178-F184 am linken Rand);
  v5/v3 `G_belege/nb2_n3_aufnahme_v5west_leon_f60-126_cut0_hell2.png`, `..._v3west_elza_...` (F108 Kanister im Maul); v2/v4
  `G_belege/nb2_n3_aufnahme_v2nordost_leon_f124-196_cut0_hell2.png`, `..._v4nordost_elza_...`. In allen Bildern stehen noch die
  Spinnen (0x25) dieses Zweigs im Becken (Spur F entfernt sie).

### 9.5 Tests (gezielt, keine volle Suite; Bau 2026-10-25 local_build.sh build, ctest mit msys64 zuerst im PATH)
Neu/geaendert: unit_r41_kanister2080_szene (Dir_set `33 00 00 00 00 0c 00 00` direkt nach `29 07`; `22 05 1f 00` + Poll Ck(5,0x1F)
zwischen Sleep 20 und WURF), _landung (Nord- und Suedwurf im Wasser, 14 bzw. 22 von 39 Fall-Bildern in Cut 12, Wahl fuer fuenf
Lagen, Bit 117 im Raumaufbau 2090/2091), NEU unit_r41_kanister2080_anschwimmen (echter Boss-Takt: fuenf Lagen beim Ausloesen ->
Aufnahme Bild 131/419/407/424/319, kein Biss davor; Gegenprobe ohne Szene: keine Aufnahme, Haken inaktiv), integration_r41_kanister2080
(szene_leon/elza: Zeile 42 >= 97 Bilder im Bild, Folge "Zeile 42 stand" -> "Wsleeping frei" -> "Wurf", Gier in Cut 5 ab dem
Hinknien unveraendert, erstes Cut-7-Bild = Wurf-Gier, Zeile "Wurfrichtung"), integration_r41_kanister2080_kampf (vier Laeufe:
kampf_leon/kampf_elza (-6000,-17500), nordost_leon (0,-7000), west_elza (-8000,-16000); je "Wurfrichtung", "ANSCHWIMMEN an",
Aufnahme, R1-Tod; gemessen Aufnahme F113/F113/F154/F90, Leiche (-5502,-11705)/(-5502,-11705)/(5716,-9033)/(-4670,-18747)).
- `ctest --test-dir re15_port/build -R "unit_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_|unit_msg_|unit_action_msg_gate|unit_alligator_ai|unit_gator_|unit_r35_granate|unit_r34_reaktion|unit_re2_spider_water_2090" --timeout 600 --output-on-failure -j4`
  -> **`100% tests passed, 0 tests failed out of 43`** (Total Test time (real) = 61.86 sec).
- `ctest --test-dir re15_port/build -R "^integration_r41_kanister2080$|^integration_r41_kanister2080_kampf$|^integration_r41_re2gator$|^integration_r35_granate$|^integration_r38_wasser$|^integration_r41_wasser20b0_npc$|^integration_r37_itemband$|^integration_r36_tueren$" --timeout 3000 --output-on-failure -j3`
  -> `88% tests passed, 1 tests failed out of 8` (Total Test time (real) = 968.51 sec): gruen kanister2080 (177,20 s),
  kanister2080_kampf (491,86 s), re2gator (226,18 s), r35_granate, r38_wasser, r41_wasser20b0_npc, r37_itemband (838,17 s);
  integration_r36_tueren "Endbild nicht erreicht (exit=Process terminated due to timeout)" - unter Last (-j3 plus vier parallele
  Messlaeufe), einzeln nachgefahren: `ctest --test-dir re15_port/build -R "^integration_r36_tueren$" --timeout 1100
  --output-on-failure` -> **`100% tests passed, 0 tests failed out of 1`** (Total Test time (real) = 63.97 sec).
- Spur B's Riegel (unit_r41_re2gator_* 18 Teile, integration_r41_re2gator) gruen: der Haken greift nur fuer den Kanister DIESER
  Szene (Bit 116), Spur B's RE15_GATOR_KANISTER-Weg sieht ihn nicht.
- Stand vor Nachtrag 26 (danach nur Kommentare): `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`,
  derselbe Unit-Lauf -> `100% tests passed, 0 tests failed out of 43` (Total Test time (real) = 57.27 sec).
- **Endstand mit Nachtrag 26** (Hals + Kippen; neue Teile unit_r41_kanister2080_kippen / _hals, integration szene prueft die
  Kipp-Folge "A 1:0 6:240 12:1056 18:2448 24:4096", kampf die Zeilen "HALS zum Kanister" / "HALS zurueck (Akku -> 0)"):
  `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`;
  `ctest --test-dir re15_port/build -R "unit_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_|unit_msg_|unit_action_msg_gate|unit_alligator_ai|unit_gator_|unit_r35_granate|unit_r34_reaktion|unit_re2_spider_water_2090" --timeout 600 --output-on-failure -j4`
  -> **`100% tests passed, 0 tests failed out of 45`** (Total Test time (real) = 54.76 sec);
  `ctest --test-dir re15_port/build -R "^integration_r41_kanister2080$|^integration_r41_kanister2080_kampf$|^integration_r41_re2gator$|^integration_r35_granate$|^integration_r38_wasser$|^integration_r41_wasser20b0_npc$|^integration_r37_itemband$|^integration_r36_tueren$" --timeout 3000 --output-on-failure -j3`
  -> **`100% tests passed, 0 tests failed out of 8`** (Total Test time (real) = 927.54 sec; kanister2080 204,86 s, kanister2080_kampf
  523,99 s, re2gator 248,31 s, r36_tueren 70,60 s, r37_itemband 927,46 s).
  Messkopien re15_pc_g41nb2a/b.exe geloescht; Laeufe/Bilder nur im scratchpad `<SP>/g2`, im Baum nur G_belege/nb2_*.

### 9.5a Nachtrag 26 (2026-10-25 08:16, waehrend dieser Nachbesserung in den Baum gelegt) - Nutzer-Antworten auf 8.8 + FG-E5
Wortlaut-Kern (AUFTRAG.md): 1. "Soll der Alligator die RE2-Hals-/Anschwimm-Lenkung zum Kanister bekommen?" - **"Ja, RE2-Lenkung
(Empfohlen)": im Kanister-Bereich lenkt er Hals und Schwimmweg zum Kanister und nimmt ihn verlaesslich auf**; 2. Wurf-Kamera -
**"2090 Kamera 12 (von unten)"** (wie gebaut, unveraendert); 3. "Soll der Kanister beim Greifen sichtbar kippen?" - **"Ja,
RE2-Kippen (Empfohlen)"**. FG-E5: G setzt 1. und 3. um, kleine markierte Haken in Spur B's Modulen erlaubt, B's Riegel gruen.
Damit ist der Entscheid 9.3 (Schwimmweg zum Kanister) vom Nutzer bestaetigt; neu dazu kommen Hals und Kippen.

**Hals - RE-Belege (selbst gelesen):** RE2_Quellcode_V2/FUN_800177c0.c (Zweig `(*(byte*)(p+0x1c0) & 4) != 0`): Ziel = (+0x170,
+0x172, +0x174); FUN_8001820c (RE2_Quellcode_V2/FUN_8001820c.c) rechnet Welt-Gier/-Neigung vom Part (+0x5c..) zum Ziel (dy =
Part.y - Ziel.y); das Gier-Residual zum Koerper (Ziel - Gier - Part-0-Keyframe) wird auf +-(+0xA0) begrenzt, der Akku +0x98
laeuft mit +-(+0x9C) auf (Ziel - Part-Keyframe +0x6A) und wird fuer RotMatrix auf den Part-Winkel addiert, danach wieder
abgezogen; Neigung ebenso mit +0x9A/+0x9E/+0xA2 auf +0x6C. EM23_OVL_0000.BIN: Ziel `lw a0,56(s0)` / `lw v1,60(s0)` / `addiu
v1,v1,-800` / `sh a0,368(s0)` / `sh v1,370(s0)` / `lw v0,64(s1)` / `sh v0,372(s0)` @0x80100a4c-70 = (eigene x, y - 800,
Kanister z); Hals-Part `addiu v0,zero,5` / `sb v0,449(s0)` @0x801001c4-cc; Schritt 20 `addiu v0,zero,20` / `sh v0,1016(v1)` /
`sh v0,1018(v1)` @0x801005a0-ac, Klemmen `addiu v0,zero,560` / `sh v0,1020(v1)` @0x801005b0-b4, `addiu v0,zero,312` / `sh
v0,1022(v1)` @0x801005b8/c4 (Part 5 = Datensatz 5 x 0xAC = 0x35C, +0x9C = 1016).
**Hals - Umsetzung:** kanister2080_r41.c `hals_takt` (einmal je Bild im 2090-Takt, nur mit Bit 116): im Zielmodus (= das
Anschwimmen 9.3) Abfrage-Pose des Alligators (Keyframe-Winkel Part 0/5 ueber eine Erfassungs-Funktion, Hals-Weltlage aus Part 5
x Zeichen-Skala), Akku-Ziel mit `re15_k2080_hals_ziel` (Konvention der Kopf-Fuehrung des Ports, skeleton_common.c FUN_8003790c),
Slew `re15_k2080_hals_schritt`; ausserhalb Akku -> 0 (SELBST-Zweig von FUN_800177c0: Ziel = Keyframe; der normale RE2-Halsmodus
FUN_80017d98 auf Leon bleibt wie bei Spur B OFFEN, B §11.3). Auf die Pose kommt der Akku ueber den verketteten
`g_anim_bone_angle_hook` (der Haken der Spur G5 wird vorher aufgerufen, dann +Akku auf Part 5 NUR dieses Alligators) - pro Bild
absolut wie die Part-Zusaetze der Spur B; der Akku selbst wird nur im Takt gerechnet (der Alligator wird je Bild mehrfach
posiert: Spur B's gezeichnete Pose, der NPC-Zeichner). PORT-WAHL der Abbildung: im RE2-Korridor ist "eigene x" = x des Kanisters
(262 daneben, 9.3); im offenen Becken steht dort der Kanister selbst.
**Kippen - RE-Beleg:** ROOM40A0 sub10 (re2_scd_walk.py): `2e 04 01` Work_set(4,1) @0x0656, `34 10 00 00` Member_set rz = 0 @0x0666,
`2f 0b fc ff` a[5] = -4 @0x066E, `0d 00 06 00 17 00` For 23 @0x0676 { `30` Add_speed (Winkel += v) @0x067C, `31` Add_aspeed (v += a)
@0x067D, `02` Evt_next }, `34 10 00 fc` Member_set rz = -1024 @0x0682 -> Winkel(k) = -2 k (k - 1), Bild 23 -1012, dann -1024.
**Kippen - Umsetzung:** `re15_k2080_kipp_bruch(k)` = Winkel(k)/1024 in Q12; `kanister_in_haenden` dreht ab dem Griff je Bild um
die Hochachse (Gier des Werfers x Bruch) und die Querachse (1024 x Bruch) und fuehrt die Mitte von der stehenden Lage an der Marke
(Fuss y -14400 + halbe Laenge) in die Haende; ab Bild 24 wie bisher quer in den Haenden. PORT-WAHL der Achsen/Mitte (RE2 kippt den
Kanister von der Wand ins Wasser, nicht in Haende).
**Messung (Bau 2026-10-25 09:15, exe-Kopie re15_pc_g41nb2b.exe, Rezept kampf, Laeufe `<SP>/g2/runs/{s4,k2}_{leon,elza}`):**
- Kippen, Leon und Elza: debug.log `F465 Kippen Bild 1: Bruch 0/4096` -> `F470 Bild 6: 240` -> `F476 Bild 12: 1056` -> `F482 Bild 18:
  2448` -> `F488 Bild 24: 4096` (= RE2-Winkel 0/60/264/612/1024). Bild `G_belege/nb2_n26_kippen_marvin_f462-492_cut5_x2.png` /
  `..._john_...` (Cut 5, 2x): F462-F470 steht der Kanister hochkant vor dem knienden Werfer, F472-F484 kippt er beschleunigt zur
  Seite (oben zuerst langsam, dann schnell), ab F486 liegt er quer in den Haenden - kein Sprung mehr in einem Bild.
- Hals, Leon und Elza: `F43 HALS zum Kanister: Part 5 Akku Gier 20 Neigung 14, Akku-Ziel (482,14)` (Alligator dreht sich gerade
  zum Kanister), `F60 Akku Gier 309 Neigung -19`, `F75 9/40`, `F90 -35/48`, `F105 -23/66`, `F114 HALS zurueck (Akku -> 0)` im Bild
  nach der Aufnahme (Stufe 1 F113). Aufnahme F113, angehaengt F125, R1 F826 wie ohne Hals (der Hals bewegt nur die Pose, nicht die
  Wurzel). Vergleich `G_belege/nb2_n26_hals_vorher_nachher_leon_f48-78_x3_hell25.png` (oben k_leon ohne Hals, unten k2_leon mit
  Hals, dieselben Bilder und Wurzellagen): F54 (Cut 12) und F66-F78 (Cut 0) zeigen den Kopf anders gedreht/geneigt.
- Spur B's Riegel sehen den Haken nicht: er wird erst in `hals_takt` eingehaengt, und der laeuft nur mit Bit 116 in 2090/2091.

### 9.6 OFFEN (gemessen bzw. benannt, nicht geraten)
1. **Apply-Bild F530** (N2): dieser Zweig (Basis 9f10bdb0) zeigt im Bild des Schnitts auf Cut 7 noch den alten Hintergrund mit dem
   schon gedrehten Werfer und der Zeile (`G_belege/nb2_n2_schnitt_applybild_leon_f526-534.png`). Das Original praesentiert dieses
   Bild nicht (@0x80021558/@0x80021560, Spur E E-N1); auf master liegt das als kamerahalt_r41.c / bildhalt_r41_pc.c. Bei der
   Integration pruefen: im Lauf s3_leon-Rezept bleibt F529 stehen, bis Cut 7 da ist (Messschiene RE15_KAMERAHALT_LOG).
2. (erledigt mit Nachtrag 26, 9.5a) Griff-Bild: der Kanister kippt jetzt in 23 Bildern nach ROOM40A0 sub10 in die Haende.
3. Anschwimmen: die Wende auf der Stelle (Fehlwinkel > 0x300, Slew 0x60) und der Slew 0x30 sind die DESIGN-Werte der Verfolgung
   der Spur B (enemy_ai_boss_gator.c "WENDE STATT BOGEN"), in kanister2080_r41.c mit Verweis uebernommen; der Kanten-Umweg und die
   Klemmflaeche kommen ueber den Export re15_gator_boss_steuerziel unveraendert aus Spur B. Steht Leon auf der Plattform, verlaesst
   der Alligator die Belagerung, solange der Kanister frei und >= 5920 entfernt liegt (GUARD-Haken).
4. Unveraendert aus 8.7: RE2-Ausloeser-Mitglieder +0x1D8/+0x1DA (Tyrant-Overlay fehlt), Kanister in RE2-Groesse, Ton nur ueber die
   Mischkette belegt; Werfer in 2090 Cut 12 nicht im Bild (FG-E4); waehrend des Falls schiebt der Alligator den gehaltenen
   Spieler (8.7.3).
5. Hinweis an Spur F: in diesem Zweig stehen die Spinnen (0x25) noch in 2090 (alle Aufnahme-Bilder 9.4).

### 9.7 Fuer den Nutzer (Stand Nachbesserung 2)
Neue Assets: keine (weiter `re15_port/shared_assets/RE2/KANISTER2080.VBS`). Sprachdateien (vom Nutzer aufzunehmen, unveraendert):
`synchro/STAGE2/room2080/main42.wav` ("Marvin: Hey, Leon, try with this!"), `synchro/STAGE2/room2081/main42.wav` ("John: Hey,
Elza, try with this!") - liegt eine Aufnahme dort, wartet Marvin/John mit dem Wurf, bis sie zu Ende ist (wie RE2 auf Adas Stimme).
Was sich im Spiel aendert:
- Die Zeile "Hey, Leon/Elza, try with this!" steht jetzt ganz (gut 3 s); erst danach wirft Marvin/John den Kanister - vorher brach
  der Wechsel nach 2090 sie nach 1,7 s ab.
- Beim Aufheben dreht sich Marvin/John nicht mehr ruckartig zur Seite; die Wurfrichtung nimmt er erst mit dem Schnitt auf die
  Totale ein.
- Der Kanister fliegt in das Becken, das weiter vom Alligator entfernt ist (Nord oder Sued). Der Alligator schwimmt danach sofort zu
  ihm und richtet dabei Hals und Kopf auf ihn (RE2-Lenkung, deine Auswahl in Nachtrag 26), beisst Leon/Elza bis dahin nicht und
  schnappt ihn etwa 3 bis 5 Sekunden nach dem Wurf; dann ein Treffer -> Explosion, Kiefer, Tod.
- Beim Aufheben kippt der stehende Kanister sichtbar in die Haende von Marvin/John (RE2-Kippen, deine Auswahl in Nachtrag 26).
Entscheidungsfragen: keine offenen (Nachtrag 26 hat 8.8 beantwortet; Kamera 12 bleibt).

## 10. Nachbesserung 3 (Abnahme 2 `G_abnahme_2.md`, 2026-10-25)
Baum wie oben, Start auf 53321b00 (AUFTRAG Nachtrag 29; Code-Stand = e4121848, exe 09:15). Maengel M1-M3 der Abnahme 2,
Entscheide G-E1 (M1: Kamerawahl des R1-Todes in Spur B's Modul, Mass = RE2-Cut-5-Lage des Kopfes) und G-E2 (M2: Werfer in
2080 Cut 7 der Kamera zugewandt wie Ada, Bahn in Weltrichtung), Nachtrag 28 (M3: Explosionseffekt des Gaskanisters).
Die erfuellten Punkte (P1, P2, Ausloeser, Kopfschuesse, Griff/Kippen, Zeile 42, Fall/Aufschlag, Anschwimmen/Hals, Aufnahme,
R1 nach einem Treffer) bleiben, ihre Riegel bleiben gruen. Nachtrag 29 (Leiche/Kette weiter weg von der Leiter) ist Spur F.
Laufordner/Bilder: `<SP>/g3` (scratchpad der Sitzung, nicht im Baum); Bildbelege im Baum unter `G_belege/nb3_*`.

### 10.0 Plan / Stand
- [x] M3 RE: Zusammensetzung der RE2-Kanister-Explosion (10.1)
- [x] M3 Messung vorher, Ursachen, Umsetzung (Raum-ESP ROOM40A0, Ops, Kern-Zeilen 485..498, Fontaene 1:1), Messung nachher,
      RE2-Referenzbild, Riegel (10.2); der Kanister selbst zerplatzt (FUN_80036eb4, 10.2a)
- [x] M1 Messung vorher / Umsetzung G-E1 / Messung von sieben Verlaeufen Leon + Elza / Riegel "Tod im Bild" (10.3)
- [x] M2 RE (Ada-Blick Cut 7) / Umsetzung G-E2 / Messung Nord- und Suedwurf Leon + Elza / Riegel Blickwert (10.4)
- [x] Riegel kampf an die neue Landestelle angepasst (10.4a), Tests gezielt (10.5), OFFEN (10.6), Fuer den Nutzer (10.7)

### 10.1 M3 - Explosionseffekt des Gaskanisters (Nachtrag 28): RE2-Beleg (selbst disassembliert/gelesen)
Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (info/re2leon/PSX.EXE, `--bin EM23_OVL_0000.BIN`), eigene
ESP-Zerleger (Spawn-Rechnung wie re2_fx.c spawn_kern = FUN_8001cbe8/FUN_8001bf10, Registrierung FUN_8001bca0), RDT-Bytes
info/re2leon/PL0/RDT/ROOM40A0.RDT. Ergebnis: Die Explosion des Kanisters ist KEIN einzelner Sprite, sondern die R1-Folge aus
fuenf Effekt-Quellen; vier davon brauchen die RAUM-ESP von ROOM40A0, die der Port in 2090/2091 nie registriert hat.

**R1 P0 @0x801021ec-0x80102398 (EM23, `+6 == 0`):** `jal 0x8005bd6c` (SE 4) @0x801021ec, `jal 0x80018fb0` @0x801021f4
(FUN_8007730c = Bit setzen in 0x800d489c/0x800d487c, kein Effekt), Sperre/Cut 5 @0x8010220c-6c, Schuetteln `ori v0,v0,0x4`
@0x801022b0, Kanister-Weltlage aus der Matrix @0x80102300-24 (+0x80 = Einheitsmatrix 0x8009db44), `jal 0x8007746c` @0x80102320
(Matrix -> Winkel, RE2_Quellcode_V2/FUN_8007746c.c), Rumble @0x8010232c/@0x80102340, **`jal 0x80036eb4` a0 = 0x800d051c
(Kanister-Objekt) @0x80102348**, Kiefer FUN_80104318 @0x80102354, VSync 4 @0x8010235c-64, **Fontaene FUN_80103eb4 @0x80102368**,
**Spritzring FUN_8010338c @0x80102374**, Raum-SE Se_on(0x020f0001) @0x8010237c-84, +0x16B = 9 @0x80102390-94.
- **FUN_80036eb4 (der Kanister fliegt abgedunkelt weg):** `ori v0,zero,0x8000 / sh v0,492(a0)` (+0x1EC), `addiu v0,zero,-10 /
  sh v0,486(a0)` (+0x1E6 = -10), `lui v1,0x40 / ori v1,v1,0x4040 / sw v1,444(a0)` (+0x1BC Farbe 0x404040), `sh zero,488/490/496(a0)`,
  `lw v0,332 / ori v0,v0,0x4a / sw v0,332(a0)` (+0x14C |= 0x4A = Flug wie der Kiefer-Part), `lhu v1,118 / addiu v1,v1,2048 /
  sh v1,484(a0)` (+0x1E4 = Gier + 0x800) @0x80036eb4-ef4: dieselben Felder wie FUN_80104318 fuer Part 7, im Objekt ab +0x14C.
- **Spritzring FUN_8010338c:** 6 x `jal 0x8001bf10` a0 = 0x030f2000/0x030f3000/0x030f3000/0x030f2000/0x030f2000/0x030f3000,
  a1 = 0, a2 = 0x800d051c + 84 (Kanister-Matrix) @0x8010344c-d0, Versaetze = SVECTOR-Tafel @0x801000a8 `[0,-2800,0] [300,-2100,0]
  [0,-700,0] [0,0,300] [0,-1400,-200] [-500,-400,200]` (laengs des Kanisters, Laenge 2784). Kern-Bank 3 (CORE00.ESP Id 03 @0x8,
  Kopf CLUT 0x7811 = Zeile 480 x 272, TPage 0x1E), Sub 0x0f = Skript 7 mit Palette +1 = CLUT-Zeile 481; Schritt 0 Op 1 (Status
  0xB003, Anim 11, TPage |= 0x20 = Mischmodus 1 B+F), Schritt 1 Op 0, Beschl. (0,-18,0) = steigt; Anim 11..22 = Wolke aus 3x3 bis
  4x4 Zellen (Zellen 0x16..0xa5, je 1 Bild).
- **Fontaene FUN_80103eb4** (Callback +0x234, Zaehler +0x231, 64 Ticks): Tick 0 = Einhaengen `sw v1,564(s1)` @0x80103f0c-1c.
  Tick 1 (`beq v1,v0,0x80103f20` @0x80103ed8): `jal 0x8001bf10` a0 = **0x26032000** (Raumbank 0x26, Sub 3, Skala 0x2000), a1 = 0,
  a2 = s2 + 1104 (Part-6-Weltmatrix), Versatz (rnd*2+300, rnd*2, rnd*4+200) @0x80103f20-6c - danach OHNE Sprung weiter in den Block
  der Ticks 2..8 @0x80103f74 (Tick 1 spawnt also alle fuenf). Ticks 1..8 (`slti v0,v1,9` @0x80103ef8): a0 = **0x00064000** (Kern-
  Bank 0 Sub 6) an s2 + 1276 (**Part 7**), Versatz (rnd*4+300, rnd*2, rnd*2+200), a1 = rnd*16 @0x80103f74-cc; a0 = **0x00034000**
  (Kern-Bank 0 Sub 3) ebenfalls an s2 + 1276 (**Part 7**: s0 wird erst im Verzoegerungsplatz @0x80104030 auf +1104 gesetzt),
  Versatz (rnd*4+300, rnd*2, rnd*2+200) @0x80103fd0-0x80104028; 2 x a0 = **0x08024000** (Raumbank 0x08, Sub 2) an s2 + 1104
  (Part 6), Versatz (rnd*2+300, rnd*2, rnd*4+200) (`sra v0,v0,22` @0x8010405c), a1 je rnd*16 @0x8010402c-0x801040a8. Tick 64:
  Callback aus `sw zero,564(s1)` @0x801040bc. (rnd = `jal 0x80015fe8` mit `sll 24 / sra 23|22|20` = int8 * 2|4|16.)

**Banken und Skripte (Datei-Offsets, ESP-Zerleger):**
| Quelle | Bank / Sub | Datei | Kopf CLUT / TPage | Skript | Bild |
|---|---|---|---|---|---|
| Spritzring | Kern 0x03 / 0x0f | CORE00.ESP @0x8 (Tafel @0x41C, Skript @0x5B4) | 0x7811 (Zeile 480) / 0x1E | Op 1 -> Anim 11, Op 0 | Wolke Zeile 481, additiv |
| Fontaene Tick 1 | Raum 0x26 / 3 | ROOM40A0-ESP @0x5A8 (Skript @0x808) | 0x7840 / 0 (Lader legt um) | **Op 69** = Sprueher | unsichtbarer Sender, 90 Bilder |
| Op-69-Kinder | Raum 0x26 / 0..2 | @0x760 / @0x798 / @0x7D0 | wie oben | Op 11 (Op 1 + Skala/3000), Op 0; v (50/110/80, -130/-90/-100, 0), a (0,10,0) | Blutspritzer esp26 Palette 0 ("CHI") |
| Fontaene Ticks 1-8 | Kern 0x00 / 6 | CORE00.ESP @0x88C (Skript @0x110C, 3 Teile) | 0x7A11 (Zeile 488) / 0x1F | Op 1 -> Anim 5; v (50,-90/-70,0/+-14), a (0,10,0) | Blut Zeile 488 |
| Fontaene Ticks 1-8 | Kern 0x00 / 3 | @0xDEC (9 Teile) | wie oben | Op 1, **Op 4** (Warten +3), **Op 6** (Lebensdauer +2) | Blutstropfen |
| Fontaene Ticks 1-8 | Raum 0x08 / 2 | ROOM40A0-ESP @0x2F0 (Skript @0x434) | 0x7840 / 0 | Op 1 -> Anim 0..7 (2x2 Zellen 24 px), **Op 41** | gluehende Brocken esp08 |
| Op-41/42-Kinder | Raum 0x09 / 8..12 | @0x470 (Skripte 0..4, Palette +1) | 0x7840 / 0 | Op 0 + Op 16 (Boden) -> Op 5 (Wasser: Spritzer 0x1A01) bzw. **Op 37/38** | Fleischbrocken esp09 Palette 1 |
| Op-5-Spritzer | Raum 0x1A / 1 | @0x82C | 0x7840 / 0 | **Op 78** (Op 2 + Skala/3000) | Wasserspritzer esp1A Palette 0 |

Neue Ops (RE2-Tafel 0x8009D868, selbst disassembliert):
- **Op 4** = 0x8001df50: `lbu v0,3 / beq / addiu v0,v0,-1 / sb v0,3`, sonst `jal 0x8001dc30` (= Op 1).
- **Op 6** = 0x8001e11c: `lbu v0,2 / beq / addiu v0,v0,-1 / sb v0,2`, sonst `sb zero,1 / sb zero,0 / sh zero,24` (Platz frei).
- **Op 37** = 0x80020660: `sw zero,0` (Op A/B/[2]/[3] := 0), `sb zero,9 / sb zero,8`, Anim := 6 (`addiu v0,zero,6 / sb v0,33`),
  `sh zero,14 / sh zero,12` (vy, vx), Countdown := Anim[6].Dauer (`lbu v0,2(v1) / sb v0,32`), Status &= 0xA001 (`andi v0,v0,0xa001`).
- **Op 38** = 0x800206e0: `sb zero,8 / sh zero,12 / sb zero,3`.
- **Op 41** = 0x800207e4: 3 x `jal 0x8001cbe8` a0 = 0x09<<24 | (step[2]+0|1|2)<<16 | 0x0800|0x1000|0x0c00, a1 = rnd << 4
  (`sll v0,v0,20 / sra a1,v0,16`), a2 = *(Platz+0x6C), a3 = Platz + 0x2C; danach Op A := 42 (`addiu v0,zero,42 / sb v0,0`).
- **Op 42** = 0x800208b8: 2 x Spawn Sub step[2]+3 / +4 mit Skala 0x0c00 / 0x0a00, danach Op A := 0.
- **Op 69** = 0x80022ebc: Phase +2 == 0 -> Status 0x8002, +2 := 1, +3 := 90 (`addiu v0,zero,90 / sb v0,3` @0x80022f08-24) und
  weiter; Phase 1 -> +3 == 0: Platz frei (@0x800231d4-dc); sonst 3 x `jal 0x8001cbe8` a0 = 0x26<<24 | ((rnd % 3) + (Sub & 0xf8))<<16
  | Skala(+0x3A) (`lui s3,0x5555 / ori 0x5556` = /3, `andi s2,v0,0xf8`), a1 = +0x22 + 400 * (rnd % 30) (`lui s4,0x8888 / ori 0x8889`
  = /30, x25 x16), a2 = *(Platz+0x6C), a3 = Platz + 0x2C @0x80022f44-0x80023158; ab +3 < 65 (`sltiu v0,v0,0x41` @0x80023170)
  Skala := Skala * 955 / 1000 (Magic 0x10624dd3 >> 38) @0x8002317c-b0; +3 -= 1 @0x800231c8.
- **Op 78** = 0x800241c8: `jal 0x8001dd2c` (= Op 2), dann +8/+9/+10 und +0xC/+0xE/+0x10 := Feld * Skala / 3000 wie Op 11
  (Magic 0x057619f1 @0x800241f8-0x80024340).

**Lader der Raum-ESP-Texturen = FUN_8001bd38** (RE2_Quellcode_V2/FUN_8001bd38.c, gerufen von FUN_8001bba4 mit RDT[21]/RDT[20]):
je registrierter Raumbank k (Registry 0x800eae50) die TIM k (Offset-Tafel rueckwaerts vor RDT[21] = 0x2fcfc, Basis RDT[20] =
0x221fc); Bild an x = Spalte * 64 (Spalte ab 15, `iVar10 = 0xf`), y = laufende Hoehe; passt die TIM nicht mehr (`0x100 < y + h`),
Spalte - 1 und y = 0; CLUT an (0x120, Zeile) ab Zeile 480 (`y = 0x1e0`), je TIM um ihre CLUT-Hoehe weiter; Bankkopf +4 :=
GetClut(0x120, Zeile), +6 (Byte) := GetTPage(0, 0, x, y), jedes Zell-v (+n1*8+9, je 4 Byte, n2 Zellen) += y. Fuer ROOM40A0
(Registry 16 19 08 09 26 1A 18 = RDT @0x59CC = effect.esp, 3100 B = RDT[0x59CC:0x65E8], sha1 bcce378c; TIMs RDT @0x221FC +
Offsets 0/0x10A0/0x38E0/0x5D20/0x6D80/0x9A20/0xC2A0 = esp16/19/08/09/26/1A/18.tim bytegleich): Spalte 15 (TPage 0x0F): 0x16 y 0-31,
0x19 y 32-111, 0x08 y 112-183, 0x09 y 184-215; Spalte 14 (TPage 0x0E): 0x26 y 0-87, 0x1A y 88-167, 0x18 y 168-215; CLUT-Zeilen
(x 288) 0x16 480-483, 0x19 484, 0x08 485, 0x09 486-487, 0x26 488-491, 0x1A 492-494, 0x18 495.
Kern-Banken (CORE00.ESP, ohne Umlegen): 0x03/0x04 Zeile 480, 0x05 484, **0x00 488**, 0x01 492, 0x02 494, 0x06 497, 0x07 498 (x 272).

**Port-Abweichungen der R1-Folge (Code-Lesung vor der Messung):**
1. Raum-ESP von ROOM40A0 nicht registriert (nur GLAS1090.ESP aus room1090) -> Spawns der Banken 0x26/0x08 (und ihrer Kinder 0x09/0x1A)
   laufen in `spawn_kern` gegen `s_bank_off[bank] < 0` -> -1.
2. Ops 4/6/37/38/41/42/69/78 fehlen in `op_rufen` (default: `s_op_unbekannt++`).
3. Zeichner re2fx_pc.c laedt nur die CLUT-Zeilen 480..484 von RE2 TEX.TIM und laesst jede andere Zeile aus (Kommentar N5) -> Kern-Bank 0
   (Zeile 488, Blut) wird nie gezeichnet.
4. gr2_fontaene_takt: Tick 1 nur 0x26032000 (`else if`, RE2 faellt durch); 0x00034000 an Part 6 mit Versatz (rnd*2+300, rnd*2,
   rnd*4+200) statt Part 7 mit (rnd*4+300, rnd*2, rnd*2+200); 0x08024000 Versatz z rnd*2+200 statt rnd*4+200.
5. FUN_80036eb4 (Kanister fliegt, Farbe 0x404040) fehlt: der Kanister steht 9 Bilder still und verschwindet.

### 10.2 M3 - Explosionseffekt: Messung vorher, Ursache, Aenderung, Messung nachher
**Messung vorher** (Bau e4121848 = Stand der Abnahme 2, Messkopie re15_pc_g41nb3a.exe; Lauf `<SP>/g3/runs/kk_vorher_leon`, Rezept der
Spur B fuer den Kanister-Tod ohne Szene: `RE15_GB_TEST=5 RE15_PLAYER_POS=6500,-25000,3072 RE15_GATOR_KANISTER=4430,-26160`, Pistole):
R1 F70 (`RE2 R1 start pos=(4997,-20318)`), Schnitt auf Cut 1. debug.log registriert nur `[re2fx] RE2 CORE00.ESP 8572 B ->
re2fx_register_core rc=0` und `[glas1120] GLAS1090.ESP 3092 B -> re2fx_register_raum rc=0` (Raum-ESP von room1090). Bilder F72-F80
(`G_belege/nb3_m3_explosion_vorher_nachher_kk_leon_f70-94.png`, obere Haelfte): nur ein weiss-blauer Schleier am Kopf. RE15_TRILOG im
Fenster (170,40)-(250,120), Bild F74: 144 Dreiecke auf Platz 52 (re2fx_pc) mit CLUT 0x7851 = Zeile 481, Bildbereich x 172..235,
y 62..122 = genau der Schleier; dieselben Dreiecke mit TEX.TIM Zeile 481 additiv nachgerechnet (Python) ergeben denselben Schleier
(6 Spritzring-Sprites ueberlagern sich). Der Schleier IST also der RE2-Spritzring (Kern-Bank 3 Sub 0x0f, 10.1) - alles andere fehlte.
**Ursachen (Code + Messung):**
1. Die Raum-ESP von ROOM40A0 war in 2090/2091 nicht registriert: Fontaene-Spawns 0x26032000 und 0x08024000 liefen in spawn_kern gegen
   `s_bank_off[bank] < 0` (Rueckgabe -1), ihre Kinder (0x09, 0x1A) entstanden nie.
2. Die Ops 4/6/37/38/41/42/69/78 fehlten im Verteiler (default `s_op_unbekannt++`).
3. re2fx_pc.c zeichnet von RE2 TEX.TIM nur die CLUT-Zeilen 480..484 (Kommentar N5: Texturhoehe); die Kern-Bank 0 (Blut, Kopf
   0x7A11 = Zeile 488) wurde ausgelassen.
4. gr2_fontaene_takt wich von FUN_80103eb4 ab (Tick 1 ohne die vier Spawns der Ticks 1..8, 0x00034000 an Part 6 statt 7, Versaetze).
**Aenderung (alle Konstanten mit Adresse im Code):**
- `engine/src/re2_fx.c` Block "RUNDE 41 SPUR G": Ops 4 (0x8001df50), 6 (0x8001e11c), 37 (0x80020660), 38 (0x800206e0), 41 (0x800207e4),
  42 (0x800208b8), 69 (0x80022ebc), 78 (0x800241c8) + Verteiler-Faelle + Kind-Spawn an *(Platz+0x6C) mit a3 = Platz + 0x2C.
- `engine/src/gator_re2_r41.c` gr2_fontaene_takt 1:1 nach FUN_80103eb4 (Durchfall Tick 1 @0x80103f74, Part 7 @0x80104020, `sra 22`
  @0x8010405c), markiert "Runde 41 Spur G (Nachbesserung 3, Nachtrag 28)".
- NEU `platform/pc/src/gator_fx_r41_pc.c`: (a) beim Betreten von 2090/2091 GATOR40A0.ESP registrieren (re2fx_register_raum = FUN_8001bca0
  mit Registry-Basis 8, wie der Raumlader FUN_8001bba4), beim Verlassen wieder GLAS1090.ESP (re15_pc_glas1120_init); (b) die Texturen und
  Bankkoepfe so umlegen wie FUN_8001bd38 (Spalte 15/14, y laufend, CLUT 0x120 ab Zeile 480, Bankkopf +4/+6, Zell-v += y) in Textur-Platz
  54; (c) RE2 TEX.TIM mit den Kern-Zeilen 485..498 in Platz 56; (d) Zeichner fuer genau die Quads, die re2fx_pc/glas1120 auslassen
  (Mischmodus wie re2fx_pc); (e) Messhaken RE15_GATOR_FX_LOG.
- `platform/pc/src/render_pc.c` RE15_TIM_SLOT_MAX 56 -> 57 (Platz 56), `platform/pc/main.c` 1 Aufruf nach re15_pc_glas1120_draw.
- NEUE ASSETS `shared_assets/RE2/GATOR40A0.ESP` (= ROOM40A0.RDT[0x59CC:0x65E8], sha1 bcce378c) und `GATOR40A0_16/19/08/09/26/1A/18.TIM`
  (= RDT[0x221FC + Offset], Offset-Tafel vor RDT[21] = 0x2FCFC; bytegleich info/re2leon/PL0/RDT/room40a0/espXX.tim).
**Messung nachher** (Messkopie re15_pc_g41nb3b.exe, Lauf `kk_n1_leon`, dasselbe Rezept): Startzeilen `[gator-fx] Raumbank 0x16: TIM 32
Zeilen -> TPage 0x0F y 0, CLUT-Zeilen 480..483` ... `0x18: TIM 48 Zeilen -> TPage 0x0E y 168, CLUT-Zeilen 495..495` (= Tabelle 10.1),
`F0 ROOM2090: Raum-ESP ROOM40A0 -> re2fx_register_raum rc=0 (Banken 0x26 1, 0x08 1, 0x09 1, 0x1A 1)`. R1-Bilder (Messzeilen):
`F70 ... Plaetze 24 (b00 12 b03 6 b08 2 b09 0 b26 4) ... gezeichnet G 0 ... re2fx_pc 27 | op_unbekannt 0 op69 1`,
`F72 ... Plaetze 74 (b00 36 b03 6 b08 6 b09 16 b26 10) Quads 110: kern480 54 kern485 13 raum 43 | gezeichnet G 56 (kern485 13 raum 43)`,
`F74 ... Plaetze 96 (b00 48 b03 6 b08 8 b09 24 b26 10) Quads 157: kern480 72 kern485 20 raum 65 | gezeichnet G 85`, bis F107 Quads > 0,
op_unbekannt immer 0 (Pool voll mit 96 Plaetzen wie RE2 0x800D8CF0). Bilder (untere Haelfte desselben Bildbogens): F72 Schleier +
Blutspritzer, F74-F78 grosser Blutausbruch mit Glut (orange), F80-F94 die Blut-Fontaene steigt vom Kopf auf.
**RE2-Referenz** (unabhaengig vom Port aus den RE2-Dateien gerendert, `re15_port/tools/r41_kanister2080/re2_explosion_referenz.py`,
Bild `G_belege/nb3_m3_re2_referenz_aus_re2_daten.png`): je Quelle die Anim-Bilder des Skripts wie FUN_80077ed0 (Zellen u/v/dx/dy,
Groesse aus dem Anim-Eintrag), Palette aus dem Bankkopf + (Sub >> 3), Mischmodus aus Status 0x1000 / TPage | step[0x14]; Kern-Banken
aus TEX.TIM, Raum-Banken aus IHRER RDT-TIM (ohne die VRAM-Umlage des Ports). Ergebnis: Spritzring = grau-weisse Wolke (Zeile 481,
additiv, 12 Bilder), Kern 0/6 und 0/3 = dunkelrotes Blut (Zeile 488), Raum 0x26/0..2 = rote Blutspritzer (14 Bilder), Raum 0x08/2 =
gluehende rot-orange Brocken (8 Bilder), Raum 0x09/8..12 = Fleischbrocken (Palette 1). Genau diese Formen und Farben zeigen die
Port-Bilder nachher (Schleier, Blut, Glut, Brocken); vorher nur der Schleier.

### 10.2a M3 - der Kanister selbst zerplatzt (FUN_80036eb4): Messung vorher, Beleg, Aenderung, Messung nachher
**Messung vorher** (Lauf `kk_n1_leon`, Messkopie re15_pc_g41nb3b.exe, Rezept 10.2): `[gator-re2] Kanister Stufe 13 pos=(4666,-1983,
-24535) ... Dreiecke 28 weg 67 hellster=(111,111,111)` - der Kanister bleibt nach dem Treffer zehn Bilder ein intaktes, beleuchtetes
Mesh und verschwindet dann (Bild `G_belege/nb3_m3_kanister_scherben_kk_leon_f70-80_x3.png` obere Reihe: dunkle Scheibe mitten in der
Spritzring-Wolke F72-F78; die dunkelroten Splitter daneben sind der Kiefer, Part 7 mit 0x202038).
**Beleg (selbst disassembliert, info/re2leon/PSX.EXE):**
- FUN_80036eb4 (17 Befehle, gerufen aus R1 P0 `jal 0x80036eb4` a0 = 0x800d051c @0x80102348): `lui v1,0x40` / `ori v0,zero,0x8000` /
  `sh v0,492(a0)` (+0x1EC = 0x8000) / `addiu v0,zero,-10` / `sh v0,486(a0)` / `lw v0,332(a0)` / `ori v1,v1,0x4040` / `sw v1,444(a0)`
  (+0x1BC = 0x404040) / `lhu v1,118(a0)` / `sh zero,488/490/496(a0)` / `ori v0,v0,0x4a` / `addiu v1,v1,2048` / `sw v0,332(a0)`
  (+0x14C |= 0x4A) / `jr ra` / `sh v1,484(a0)` @0x80036eb4-ef4.
- Objekt-Zeichner FUN_800363f0 (gerufen @0x80026964; Objekte ab 0x800d0324, Schritt 0x1F8, obj[1] = 0x800d051c): Regions-Test
  `jal 0x8002c820` @0x80036534 (Ausnahme Flag 0x10 @0x80036544-50), dann `lw v0,0x14c(s1) / andi v0,v0,0x8 / beq v0,zero` @0x80036558-64
  = Bit 0x08 -> statt des Meshes `jal 0x8002d3c8` @0x80036588 und `jal 0x8002d718` @0x800365b4 mit a2 = obj + 0x14C, danach
  `lhu v0,0x1ec(s1) / li v1,0x1e / addiu v0,v0,1 / sh v0,0x1ec(s1) / andi v0,v0,0xffff / bne v0,v1` -> `sw zero,0x0(s1)` @0x800365bc-dc.
  Der Record +0xA0 (= obj +0x1EC) ist der Bildzaehler der Scherben.
- FUN_8002d3c8 (Dreiecke) / FUN_8002d718 (Vierecke): `lhu a1,160(a2)` / `andi 0x7fff` / `srl 15` @0x8002d3f8-408 bzw. @0x8002d750-68,
  Tafel 0x8009dc28 + Index * 2 + Zeile * 60 (@0x8002d7ac-d4), Versatz je Prim = (int16)n0 >> 10 (`sll v0,v0,16 / sra v0,v0,26`) mal
  Skala @0x8002d7d8-ec, ueber MVMVA R*v + TR = Versatz im Modellraum; Farbe aller Scheitel = Record +0x70 (`lw t0,112(a2)` @0x8002d84c,
  GT4 `lui v0,0x3c00` @0x8002d858; GT3 | 0x34000000); RTPT @0x8002d5a4 + AVSZ3 @0x8002d5dc bzw. RTPT @0x8002d8ec + RTPS @0x8002d93c +
  AVSZ4 @0x8002d950 - KEIN NCLIP (0x4b400006) bis `jr ra` @0x8002d710 / @0x8002da78: jedes Prim kommt in die OT, beidseitig.
- Tafel (re2_disasm.py read 0x8009dc28, 2 x 30 int16): Zeile 1 @0x8009DC64 = 180 354 522 684 840 990 1134 1272 1404 1530 ... 2790.
- obj[1] verschwindet durch R1 P1 nach neun Bildern (+0x16B = 9 @0x80102390-94, `lb v0,363 / blez -> word0 = 0` @0x8010255c-78, im Port
  RE15_GR2_KAN_ENTFERN_F der Spur B) -> sichtbar sind Index 0..9; der Vergleich mit 0x1e greift mit Bit 15 nie.
- +0x1E6 = -10 und +0x1E4 = Gier + 0x800 liest nur die Teile-Physik der Gegner FUN_80028dac (aus FUN_80027434 @0x80027b98); der
  Objekt-Zeichner ruft sie nicht. **Berichtigung zu 10.1 Abweichung 5:** der Kanister "fliegt" nicht weg, er zerplatzt an seiner Lage.
**Aenderung** (markiert "Runde 41 Spur G (Nachbesserung 3, M3)"): `engine/src/gator_re2_r41.c` s_kan.scherben/scherben_farbe, Stempel im
R1 P0 nach dem Loesen, re15_gr2_kanister_scherben (Skala/Farbe/Index) und re15_gr2_kanister_scherben_weiter (+0x1EC += 1, Loeschen bei
0x1e); `platform/pc/src/gator_re2_pc.c` Scherben statt Mesh (Versatz entlang n0, flache Farbe, ohne NCLIP/OTZ-Untergrenze, nur die
GPU-Groessenschranke der Hardware) und Messzeile `[gator-re2] Kanister-Scherben F<n>: Bild <i> Skala <s> Farbe 0x404040, <p> Prims
gezeichnet, <w> weg`; Deklarationen in `include/re15_gator_re2_r41.h`.
**Messung nachher** (Lauf `kk_sch2_leon`, Messkopie re15_pc_g41nb3l.exe, dasselbe Rezept): `Kanister-Scherben F70: Bild 0 Skala 180
Farbe 0x404040, 93 Prims gezeichnet, 2 weg` ... `F79: Bild 9 Skala 1530 Farbe 0x404040, 95 Prims gezeichnet, 0 weg`, ab F80 kein
Kanister. (Erster Bau mit NCLIP: nur 17..38 von 95 Prims - die Rueckseiten fielen weg; nach dem Disassembly-Befund "kein NCLIP"
berichtigt.) Bild untere Reihe: an der Stelle der Scheibe dunkelgraue Scherben, die sich von F72 bis F78 vom Kanister weg ausbreiten.
Riegel: `unit_r41_kanister2080_scherben` (Teil "scherben" im Testprogramm der Spur B, echter Boss-Takt): getragen = intaktes Mesh,
nach dem Treffer zehn Bilder Index 0..9 Skala 180..1530 Farbe 0x404040, ab Bild 10 weg; `integration_r41_kanister2080_kampf` prueft
in allen vier Laeufen die zehn Skalen.

### 10.3 M1 - Tod nicht im Bild (G-E1): Ursache, Beleg, Aenderung, Messung
**Ursache:** (1) wie die Abnahme: gr2_kam_r1_start schnitt nur, wenn der KOPFPUNKT im laufenden Cut nicht im Bild lag, und waehlte dann
den besten Frontalwinkel - ohne Rand, ohne Masken; (2) neu gemessen: der Kopf wandert im Todes-Clip weit (Clip 9 + Rueckstoss, R1-Ende
bis 6100 Einheiten vom Start), und der Zeichner verwirft Gegner und Effekt-Plaetze ausserhalb des Regions-Vierecks des Cuts; (3) neu
gemessen (Lauf kg_elza, state.log Slot 35): die Leiche sprang durch die Beckenwand - F570..F578 haelt die Wandklemme sie bei
(-7416,-6884), der Anker laeuft weiter, F580 z -1916 (5000 in einem Bild), F592 x -9274 ... R1 ende (-9599,-1916): der Port setzt die
Lage im R1 absolut vom Anker, die Klemme korrigierte nur das Bild, nicht den Anker.
**Belege (selbst gelesen):** R1 P0 schneidet nur, wenn der laufende Cut den Kopf nicht zeigt (Cut 6/13 und z < -13300, @0x80102218-34);
RE2-Lage des Kopfes im R1-Schnitt Cut 5 = (170,111) (k_blick EXPLOSION, probe_kamera_re2.c der Spur B); Spritzring-Punkte = Kanister-
Matrix x Tafel @0x801000a8 (FUN_8010338c @0x8010344c-d0); groesstes Sprite-Bild CORE00.ESP Bank 3 Anim 17/18 @0x98/@0xA0 (Zellen x -36..36,
y -32..36 Texel, Zelltafel @0x190), Bildmass je Texel = Skala * camf * Aspekt / (SZ * 16 * 65536) (FUN_80077ed0, re2fx_quads), Aspekt
0x1000 (@0x5BC); Regions-Test der Effekte `jal 0x8002c820` @0x80077a30, der Gegner main.c npc_region_culled; Maskenurteil STRIKT
depth < Bucket(vz) @0x8001ce54; RE2 integriert die Lage im R1 selbst (FUN_800152c8 @0x801025a4-ac auf +0x38/+0x40).
**Aenderung** (`engine/src/gator_re2_r41.c`, markiert "Runde 41 Spur G, G-E1"): gr2_r1_pfad (Start: Kopf, Kiefer, 6 Spritzring-Punkte,
Wurzel; Vorausschau jedes zweiten Bildes von Clip 9 mit derselben Rechnung wie re15_gr2_r1_tick samt Wandklemme), gr2_r1_beurteilen
(Bild + Sprite-Rand + Viereck + Masken-Haken), gr2_r1_wahl (Rangfolge: gueltiger Start, sichtbare Proben, sichtbare Startpunkte, Abstand
der Kopf-Bildlage zu (170,111); Stufe 0 mit Spur B's Massstab-Grenze fuer Schnitte), gr2_kam_r1_start (laufender Cut bleibt, wenn er die
Wahl ist = RE2-Form, sonst Schnitt + Rueckkehr am Clip-Ende wie bisher). `platform/pc/src/gator_fx_r41_pc.c`: re15_gr2_maske_hook =
dieselbe Masken-Kette wie main.c (RDT-Sektion, sonst R15M-Container mit re15_masken36_filter / re15_optik_r39_kappen, draw_count).
`engine/src/enemy_ai_boss_gator.c` (Wandklemme am Takt-Ende, 3 Zeilen): im R1 die Korrektur der Klemme in den Anker uebernehmen (RE2-Form
der Lage-Integration).
**Messung nachher** (Messkopien re15_pc_g41nb3h/i/j.exe; Rezept des Riegels kampf mit 40 bzw. 120 Schussbloecken; Leon/Elza;
gator_boss.log; Bilder `G_belege/nb3_m1_r1_k_leon_k_elza_d15_plattform.png`, `nb3_m1_r1_k_elza_cut7_x2_hell.png`, `nb3_m1_r1_v1_leon_elza.png`):
| Verlauf | laufender Cut beim R1 | G-E1-Wahl | Kopf im Bild | Start / Todes-Clip | Leiche |
|---|---|---|---|---|---|
| Riegel-Startlage Leon (kh_leon) | Cut 2, Kopf (83,160) | Schnitt Cut 1 | (142,104) | 8/8, 19/19 | (3208,-21484) |
| Riegel-Startlage Elza (kh_elza) | Cut 0, Kopf (320,81) = am Rand | Schnitt Cut 7 | (110,101) | 6/8, 19/19 | (-7416,-6884) statt durch die Wand (-9599,-1916) |
| d15 Leon (d15h) | Cut 0, Kopf (302,84) = am Rand | Schnitt Cut 5 | (168,92) | 8/8, 19/19 | (-7416,-11910) |
| Plattform Leon (plath) | Cut 0, Kopf (299,82) | Schnitt Cut 5 | (208,91) | 5/8, 19/19 | (-7416,-6884) |
| West Elza (westg, Bau i) | Cut 0, Kopf (321,85) draussen | Schnitt Cut 5 | (175,96) | 8/8, 19/19 | |
| v1 Leon ohne Testhaken (v1b) | Cut 1, Kopf (265,100) | kein Schnitt (Cut 1 ist die Wahl) | (265,100) | 2/8, 19/19 | (5322,-25516) |
| v1 Elza ohne Testhaken (v1b) | Cut 1, Kopf (223,95) | kein Schnitt | (223,95) | 5/8, 19/19 | (5716,-25516) |
In allen Verlaeufen bleiben Kopf und Kiefer im Bild, im Viereck und unverdeckt, in jeder Probe des Todes-Clips; die Bildboegen zeigen die
Blut-Fontaene am Kopf bis zum Clip-Ende. "Start a/8" < 8 heisst: Spritzring-Punkte liegen ausserhalb des Regions-Vierecks (ihr Sprite
wird wie im Original verworfen, @0x80077a30); keine Zonenkamera hatte dort alle acht.

### 10.4 M2 - Werfer in Cut 7 mit dem Ruecken zur Kamera (G-E2)
**Messung vorher:** Abnahme 2 3.4 (Suedwurf frontal -0,45, Nordwurf 0,45; RE2 0,99). **Beleg:** RE2 ROOM7020 Ada Pos_set (-28000,-7500,
-17900) @0x0740, Dir_set 0x0e74 = 3700 @0x0748, Kamera Cut 7 RID @RDT 0x144 (-22158,-3852,-15174): Gier Ada -> Kamera 3811,4, Versatz
-111,4 (frontal 0,985) - unabhaengig nachgerechnet im Riegel unit_r41_kanister2080_blick. Add_speed = Weltaddition @0x80055abc-e0 (RE2
wirft in Weltachsen, unabhaengig von Adas Blick).
**Aenderung:** `engine/src/kanister2080_r41.c` re15_k2080_werfer_gier (Gier zur RID-Kamera Cut 7 vom Wurf-Stand + RE15_K2080_ADA_BLICK_VERSATZ
-111, Nennwert RE15_K2080_WERFER_GIER_NENN 3677), das Dir_set nach Cut_chg 7 schreibt sie; Wurfbahn, Nennlandung und Rechnung beim
Loslassen drehen die RE2-Vektoren um die Wurfrichtung (wurf_gier, Nord 3072 / Sued 1024), die Haende stehen vor dem Werfer; Messzeile
"Werfer Cut 7".
**Messung nachher** (Bilder `G_belege/nb3_m2_cut7_sued_oben_nord_unten_x3.png`, Laeufe s7_sued_leon / s7_nord_leon (Messhaken
RE15_K2080_WURF_GIER=3072), s_sued_elza, v1 Leon/Elza (Nordwurf ohne Testhaken)): in allen `Werfer Cut 7: Gier 3678, Gier zur Kamera 3791,
Versatz -113, frontal 4035/4096 (RE2 ROOM7020 Ada: Versatz -111, frontal 4034)`; Marvin/John halten den Kanister mit dem Gesicht zur
Kamera ueber die Reling, Nord- und Suedwurf zeigen dasselbe Bild. Wurf: Sued `Start (-6476,-16896,-17135) v=(10,20,-25) a=(0,20,-5) ->
Landung (-6096,-2076,-21600)`, Nord `v=(-10,20,25) a=(0,20,5) -> Landung (-6856,-2076,-12670)`; Fall in 2090 Cut 12 (unit landung:
Nord 13, Sued 18 von 39 Fall-Bildern im Bild, vorher 14/22), Aufschlag + Kanister im Becken, Anschwimmen und Aufnahme wie zuvor
(Laeufe s_sued_*: `ANSCHWIMMEN an ... Abstand 13931`, `KANISTER Stufe 1 ... F103`).

### 10.4a Riegel kampf: Laufdauer (Batch B, Stand 9167e087)
**Messung:** `integration_r41_kanister2080_kampf` brach in Batch B ab: `Lauf kampf_leon: Folge nach der Szene bricht ab vor 'R1 start'
(Stand 3 von 5)` (Lauf lauf_40a5780f/kampf_leon). Ursache gemessen: seit G-E2 stehen die Haende vor dem der Kamera zugewandten Werfer
(Gier 3678 statt 3072), der Wurf startet bei (-6476,-16896,-17135) statt (-7201,-16896,-18449) und landet bei (-6096,-21600) statt
(-6821,-22914); Aufnahme unveraendert (Stufe 1 F103). Danach zielte der passive Schuetze (Zielen/Feuern ohne Laufen) auf die Spinnen
(state.log F1500: Slot 1 hp 1, Slot 2 hp 76; vorher 111/110) und die 18 Schussbloecke endeten etwa 860 Bilder nach der Rueckkehr - kein
Treffer am Alligator, kein R1. Mit mehr Bloecken gemessen (R1 nach der Rueckkehr): kampf_leon F2290 (kh_leon), nordost_leon F1383
(no_leon, 60 Bloecke), kampf_elza F568, west_elza F322 - vorher F826/F899/F826/F814.
**Aenderung** (fc062fbc): 60 Schussbloecke (reichen ueber EXIT_AT hinaus), EXIT_AT 3000 Bilder nach der Rueckkehr (statt 1500). Kein
Spielverhalten - es ist der Testspieler, der den Alligator spaeter trifft.

### 10.5 Tests (gezielt, keine volle Suite; local_build.sh, ctest mit msys64 zuerst im PATH)
- Bau Stand 4c622ad4: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`,
  `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`.
- Unit (Stand 4c622ad4): `ctest --test-dir re15_port/build -R "unit_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_|unit_msg_|unit_action_msg_gate|unit_alligator_ai|unit_gator_|unit_g5_boss|unit_r35_granate|unit_r34_reaktion|unit_re2_spider_water_2090|unit_r34_re2fx|unit_r34_plattform|unit_r34_wasser|unit_r34_wurf|unit_r35_fenster|unit_r35_werfer|unit_r36_granate|unit_r36_schuss|unit_r39_hund1121" --timeout 600 -j2`
  -> **`100% tests passed, 0 tests failed out of 78`** (Total Test time (real) = 55.91 sec); darin neu unit_r41_kanister2080_fx_register /
  _op69 / _op41 / _op4_6 / _op37_78, _blick, _scherben, geaendert _szene/_landung (G-E2) und unit_r41_re2gator_kamera (R1-Faelle G-E1).
  Derselbe Ausdruck vor der Scherben-Aenderung (Stand 9167e087): `100% tests passed, 0 tests failed out of 77` (49.28 sec).
- Integration A (Stand 9167e087): `ctest --test-dir re15_port/build -R "^integration_r41_kanister2080$|^integration_r41_re2gator$|^integration_r35_fenster$|^integration_r35_werfer$|^integration_r35_werfer_ton$|^integration_r35_werfer_elza$|^integration_r35_werfer_form$|^integration_r34_granaten$|^integration_r34_plattform_takt$|^integration_r34_plattform_esp_eintritt$|^integration_r36_schuss$|^integration_r39_hund1121$" --timeout 3000 --output-on-failure -j3`
  -> **`100% tests passed, 0 tests failed out of 12`** (Total Test time (real) = 413.73 sec).
- Integration B (Stand 9167e087): `ctest --test-dir re15_port/build -R "^integration_r41_kanister2080_kampf$|^integration_r35_granate$|^integration_r38_wasser$|^integration_r41_wasser20b0_npc$|^integration_r37_itemband$|^integration_r36_tueren$|^integration_r36_kartenitems_bild$" --timeout 3000 --output-on-failure -j3`
  -> `86% tests passed, 1 tests failed out of 7`: gruen r41_wasser20b0_npc, r36_kartenitems_bild, r38_wasser, r36_tueren, r35_granate,
  r37_itemband; rot kanister2080_kampf (10.4a). Nach der Riegel-Aenderung: `ctest --test-dir re15_port/build -R "^integration_r41_kanister2080_kampf$" --timeout 3000 --output-on-failure`
  -> **`100% tests passed, 0 tests failed out of 1`** (Total Test time (real) = 746.65 sec).
- Integration C (Stand 4c622ad4, nach den Kanister-Scherben): `ctest --test-dir re15_port/build -R "^integration_r41_kanister2080_kampf$|^integration_r41_re2gator$|^integration_r41_kanister2080$" --timeout 3000 --output-on-failure -j3`
  -> **`100% tests passed, 0 tests failed out of 3`** (Total Test time (real) = 722.22 sec; kanister2080 170,40 s, re2gator 238,25 s,
  kanister2080_kampf 722,14 s). Im Lauf kampf je Lauf zehn Zeilen "Kanister-Scherben" Skala 180..1530, erstes Bild 95 Prims, 0 weg
  (kampf_leon F2290, kampf_elza F568, nordost_leon F1383, west_elza F322).
- Messkopien re15_pc_g41nb3a..l.exe geloescht; Laeufe/Bilder nur im scratchpad `<SP>/g3`, im Baum nur `G_belege/nb3_*`.

### 10.6 OFFEN (gemessen bzw. benannt, nicht geraten)
1. **Regions-Test des Kanister-Objekts** (Spur B): RE2 zeichnet obj[1] - intakt wie als Scherben - nur, wenn FUN_8002c820 seine Lage im
   Regions-Viereck des Cuts findet (`jal 0x8002c820` @0x80036534, Ausnahme Flag 0x10 @0x80036544-50), und zaehlt die Scherben nur dann
   weiter. Der Kanister-Zeichner der Spur B (gator_re2_pc.c) prueft das nicht - in einem Cut, dessen Viereck den Kanister nicht enthaelt,
   zeigt der Port ihn, RE2 nicht. Die G-E1-Wahl bevorzugt Cuts mit allen Startpunkten im Viereck (Spalte "Start a/8" in 10.3); nicht
   geaendert, weil es den Prop-Zeichner der Spur B betrifft.
2. "Start a/8" < 8 in einzelnen Verlaeufen (kampf_elza 6/8, v1 Leon 2/8, v1 Elza 5/8): diese Spritzring-Sprites liegen ausserhalb des
   Regions-Vierecks und werden wie im Original verworfen (@0x80077a30); keine Zonenkamera hatte dort alle acht.
3. Apply-Bild F530 (unveraendert aus 9.6.1): bei der Integration mit Spur E (kamerahalt_r41.c) pruefen.
4. Spinnen (0x25) stehen in diesem Zweig noch in 2090 (9.6.5); der Testspieler des Riegels kampf zielt auf sie (10.4a).
5. Querbezug fuer den Merge (alle Haken markiert "Runde 41 Spur G"): `engine/src/re2_fx.c` (Ops 4/6/37/38/41/42/69/78),
   `engine/src/gator_re2_r41.c` + `include/re15_gator_re2_r41.h` (G-E1-Kamerawahl, Fontaene 1:1, Kanister-Scherben),
   `engine/src/enemy_ai_boss_gator.c` (3 Zeilen: Anker-Nachfuehrung der Wandklemme nur in GBP_RE2_TOD - Spur F arbeitet fuer Nachtrag 29
   an der Startlage des Kampfes im selben Modul), `platform/pc/main.c` (1 Aufruf), `platform/pc/src/render_pc.c` (RE15_TIM_SLOT_MAX 57),
   `platform/pc/src/gator_re2_pc.c` (Scherben), `tests/unit/test_r41_re2gator.c` (R1-Faelle G-E1, Teil "scherben").
6. Unveraendert aus 8.7/9.6: RE2-Ausloeser-Mitglieder +0x1D8/+0x1DA (Tyrant-Overlay fehlt), Kanister in RE2-Groesse, Ton nur ueber
   die Mischkette belegt; Werfer in 2090 Cut 12 nicht im Bild (FG-E4).

### 10.7 Fuer den Nutzer (Stand Nachbesserung 3)
Neue Assets: `re15_port/shared_assets/RE2/GATOR40A0.ESP` und `GATOR40A0_08.TIM`, `_09`, `_16`, `_18`, `_19`, `_1A`, `_26.TIM` -
bytegleiche Kopien der Effekt-Datei und ihrer sieben Texturen aus RE2 ROOM40A0.RDT (dem RE2-Alligatorraum); ohne sie fehlt die
Explosion. Sprachdateien unveraendert (vom Nutzer aufzunehmen): `synchro/STAGE2/room2080/main42.wav` ("Marvin: Hey, Leon, try with
this!"), `synchro/STAGE2/room2081/main42.wav` ("John: Hey, Elza, try with this!").
Was sich im Spiel aendert:
- Trifft der Schuss den Kanister im Maul, laeuft jetzt die ganze RE2-Explosion: grau-weisse Wolke, dunkelroter Blutausbruch mit
  gluehenden Brocken und Fleischfetzen, Wasserspritzer, eine Blutfontaene steigt gut zwei Sekunden aus dem Kopf - und der Kanister
  selbst zerplatzt in dunkle Scherben (vorher nur die Wolke, der Kanister stand kurz still und verschwand).
- Der Tod des Alligators ist immer im Bild: Kopf und Kiefer bleiben in jeder Phase der Todes-Animation sichtbar und unverdeckt; die
  Kamera bleibt, wenn der laufende Blick das schon zeigt, sonst schneidet sie auf die Zonenkamera, die den Kopf am naechsten an RE2s
  Bildausschnitt zeigt, und am Ende zurueck.
- Der tote Alligator rutscht nicht mehr durch die Beckenwand.
- Marvin/John stehen beim Wurf mit dem Gesicht zur Kamera (wie Ada in RE2); der Kanister landet dadurch etwas anders (Suedbecken
  (-6096,-21600) statt (-6821,-22914), Nordbecken (-6856,-12670)).
Entscheidungsfragen: keine.

## 11. Nachbesserung 4 (Abnahme 3 `G_abnahme_3.md`, 2026-10-25)
Baum wie oben, Start auf cc089cde (Code-Stand = 8d7e738a/4c622ad4, exe 15:38 = Stand der Abnahme 3). Maengel der Abnahme 3:
M1 (G-E1 Plattform-Verlauf: Todes-Clip 6/19), M2 (Kern-Zeilen 485..498 nach dem ersten 2090-Besuch in jedem Raum), M3 (natuerlicher
Leon-Verlauf: Kanister/Kopf ausserhalb des Raums -> Spritzring-Wolke verworfen; Scherben ohne den Regions-Test @0x80036534),
V1 (render_pc.c RE15_TIM_SLOT_MAX 56 -> 57 ohne Freigabe). Die erfuellten Punkte (P1, P2, Ausloeser, Szene, Kopfschuesse, Griff,
Zeile 42, Wurf/Blick G-E2, Fall, Aufnahme, R1) bleiben, ihre Riegel bleiben gruen. Nachtrag 29 (Leiche/Kette weiter weg von der
Leiter = Startlage des Kampfes) ist Spur F; der Nutzer-Wortlaut, der diesen Lauf ausgeloest hat, ist Nachtrag 29 - G aendert die
Startlage nicht. Laufordner/Bilder: `<SP>/g4` (scratchpad, nicht im Baum); `<SP>/g3abn/runs` = Laeufe der Abnahme 3 (Messung vorher,
gleicher Code-Stand); Bildbelege im Baum unter `G_belege/nb4_*`.

### 11.0 Plan / Stand
- [x] M2: Kern-Zeilen 485..498 nur fuer die Plaetze der R1-Folge (11.1, Riegel integration_r41_kanister2080_raumfremd)
- [x] V1: render_pc.c unveraendert (beide Texturen als Atlas in Platz 54), Bildvergleich vorher/nachher 31/31 pixelgleich (11.2)
- [x] M3: Ursache belegt (Kopf/Kanister durch die Aussenwand), Kopf-Klemme + Regions-Test des Kanister-Zeichners (11.3, Riegel
      unit_r41_kanister2080_kopfklemme, kampf); Nachtrag waehrend der Messung: auch das andere Kanister-Ende (9009a99a), Endmessung 11.5
- [x] M1: Rangfolge voller Todes-Clip zuerst (11.4, Riegel unit_r41_kanister2080_r1wahl, kampf + Plattform-Lauf)
- [ ] Endmessung aller zehn Verlaeufe mit dem Endstand, Bildboegen, Tests gezielt, OFFEN, Fuer den Nutzer (11.5-11.8)

### 11.1 M2 - Kern-Zeilen 485..498 nach dem ersten 2090-Besuch in jedem Raum
**Ursache (Code):** `re15_pc_gator_fx_draw` (gator_fx_r41_pc.c) lief in jedem Raum; der Zweig "kern485" pruefte weder Raum noch Herkunft,
nur `s_kern_tex_ok`, das `laden()` beim ersten Betreten von 2090/2091 setzte und nie zuruecknahm. re2fx_pc.c laesst diese Zeilen in
allen Raeumen bewusst aus (Kommentar N5, Runde 34 Spur D).
**Messung vorher** (Abnahme 3 Abschnitt 5, Laeufe `<SP>/g3abn/runs/gl_2080_direkt` / `gl_2080_nach2090`, Granatwerfer Id 15, gleiche Lage
(-9950,-25700) Gier 2907 Cut 4): ohne 2090-Besuch `F200 ROOM2080 ... kern485 6 ... | gezeichnet G 0 (kern485 0 raum 0)`, nach dem Leiterweg
aus 2090 `F236 ROOM2080 ... kern485 6 ... | gezeichnet G 6 (kern485 6 raum 0)` (Muendungsblitz 0x01002000 = Kern-Bank 1 Zeile 492,
Granatkoerner 0x020c0a00 = Zeile 495).
**Beleg (selbst disassembliert, info/re2leon/PSX.EXE):** jeder RE2-Effektplatz merkt sich den Matrix-Zeiger seines Spawns (`sw a2,108(t0)`
@0x8001cd6c = +0x6C); Mehrteil-Skripte kopieren +0x18..+0x7B des Hauptplatzes in jeden Teil (16-Byte-Schleife `lw/sw ... addiu a2,a2,16 /
bne a2,t1` @0x8001ce38-60 + Restwort @0x8001ce64-6c), die Op-Kinder bekommen ihn als a2 (Op 41: `lw a2,108(v1)` @0x80020814 vor
`jal 0x8001cbe8` @0x8002081c; Op 42/69 ebenso = re2_fx.c kind_an_matrix). Die R1-Folge spawnt nur an drei Matrizen: Fontaene FUN_80103eb4 an
Part 6 (`addiu a2,s2,1104` @0x80103f58) und Part 7 (s2 + 1276), Spritzring FUN_8010338c an der Kanister-Matrix (@0x8010344c-d0). Die Kern-
Zeilen 485..498 der R1-Folge sind genau die Kern-Bank 0 der Fontaene (0x00064000/0x00034000 @0x80103f74-0x80104028; Kopf-CLUT 0x7A11 ->
Zeile 488, Sub 6/3 >> 3 = 0, die CLUT schreibt nur der Spawn @0x8001ccf4-d10).
**Aenderung (PORT-WAHL "auf die R1-Folge beschraenken", Form = RE2-Herkunft +0x6C):** `engine/src/re2_fx.c` `re2fx_platz_quelle(i)` (Zeiger
+0x6C, NULL = frei) + Deklaration in re2_fx.h; `engine/src/gator_re2_r41.c` `re15_gr2_fx_quelle_r1(m)` = einer der drei R1-Puffer
s_fx_mtx[TEIL6/TEIL7/KANISTER] (Putz nimmt die Einheitsmatrix); `platform/pc/src/gator_fx_r41_pc.c` zeichnet Kern-485-Quads nur noch fuer
Plaetze mit R1-Herkunft und zaehlt die uebrigen in der Messzeile ("kern485 fremd n"). Alle markiert "Runde 41 Spur G (Nachbesserung 4)".
**Messung nachher** (Messkopie re15_pc_g41nb4a.exe, dieselben Umgebungen, `<SP>/g4/runs/a_gl_2080_direkt` / `a_gl_2080_nach2090`):
direkt `F200 ROOM2080 ... kern485 6 ... | gezeichnet G 0 (kern485 0 raum 0) | kern485 fremd 6`, nach dem 2090-Besuch `F236 ROOM2080 ... kern485
6 ... | gezeichnet G 0 (kern485 0 raum 0) | kern485 fremd 6` - Bild fuer Bild F200..F211 bzw. F236..F247 dieselben Zahlen. In 2090 (Lauf
a_k_leon_r1): in allen 38 R1-Bildern `kern485 fremd 0`, die R1-Messzeilen sind Zahl fuer Zahl gleich denen der Abnahme 3 (diff leer).

### 11.2 V1 - render_pc.c (RE15_TIM_SLOT_MAX 56 -> 57) ohne Freigabe
**Ursache:** Nachbesserung 3 legte die Raum-Texturen (Platz 54) und die Kern-Zeilen (Platz 56) in zwei Textur-Plaetze und erhoehte dafuer
RE15_TIM_SLOT_MAX in render_pc.c (VERTRAG 1.3: nur Spur D).
**Aenderung:** render_pc.c ist wieder bytegleich mit der Basis 9f10bdb0 (`git checkout 9f10bdb0 -- re15_port/platform/pc/src/render_pc.c`,
RE15_TIM_SLOT_MAX 56). gator_fx_r41_pc.c legt beides in EINEN Platz 54 als 16-Bit-Atlas 512 x 3712: 16 Raum-Bloecke (CLUT-Zeile 480 + k) der
Hoehe 216 = die von FUN_8001bd38 belegte Fensterhoehe (Spalte 15: 32+80+72+32, Spalte 14: 88+80+48; Zeile `[gator-fx] ... Atlas Slot 54 ok:
512 x 3712 (16 Raum-Bloecke a 216 Zeilen, Kern-Zeile 488 ok)`), darunter EIN Kern-Block (TEX.TIM Seiten 0x1E/0x1F mit Zeile 488 - nach 11.1
die einzige Kern-Zeile, die gezeichnet wird). Die Texel sind vorab ueber die CLUT aufgeloest, dieselbe Wertregel re15_tim_texel_argb wie der
4-bpp-Pfad von render_pc.c; der Zeichner versetzt v um den Block statt ueber den CLUT-Slab. Hoehe 3712 < 4096 (GLES-Grenze N5; vorher Platz 54
genau 4096). Raum-Quads mit v > 216 werden gezaehlt ("aus_fenster"), nicht gezeichnet: in allen gemessenen Bildern 0.
**Messung nachher (Bildvergleich):** Lauf a_k_leon_r1 = Umgebung der Abnahme 3 k_leon_r1 (Haken HP, `RE15_FRAMEDUMP=2280-2340/2`) mit
re15_pc_g41nb4a.exe gegen die Bilder der Abnahme 3 (`<SP>/g3abn/runs/k_leon_r1`, alter Stand mit zwei Plaetzen): `Bilder 31, davon verschieden 0`
(`<SP>/g4/ppmdiff.py`, Pixel fuer Pixel, F2280..F2340 = Schnitt, Explosion, Fontaene, Scherben).

### 11.3 M3 - Kanister/Kopf ausserhalb des Raums (Spritzring-Wolke verworfen) und Regions-Test des Kanister-Zeichners
**Messung vorher** (Abnahme-3-Lauf `<SP>/g3abn/runs/v1_leon`, kein Testhaken; gleicher Code-Stand): nach der Rueckkehr steht Leon bei
(5782,0,-26532), 468 vor der Suedwand. Der Alligator traegt den Kanister (angehaengt F155, Stufe 7) und schwimmt auf Leon zu; das
Steuerziel endet auf der Klemmlinie z -25400 (re15_gator_boss_steuerziel: Kante -27000 + 1600), die Wand-Klemme der Takt-Huelle
(re15_gator_boss_tick (2), re15_collision_constrain_enemy mit hit_radius_min) haelt nur die WURZEL. state.log F444: Alligator
(4324,-25408) Gier 743 (Sued-Suedost) -> beim R1 `Kanister Stufe 13 pos=(5996,-2041,-29290)` = 2290 jenseits der Suedwand und 290 jenseits
des Regions-Vierecks (alle 13 Cuts von ROOM2090 haben den Anker x -11000..9500, z -29000..-3500). Ebenso in den anderen Abnahme-Laeufen
(Tabelle 2.7 der Abnahme): k_elza (-9986,-4622), plat_elza (-10422,-4136), west_leon (-10831,-27801), v1_elza (8714,-26043) - jedes Mal
ausserhalb der Aussenwaende x[-8900..7200] z[-27000..-5400] (SCA-Aussenwaende ROOM2090.RDT, enemy_ai_boss_gator.c Kopf). Folge:
Start 2/8 (6 ausserhalb) bzw. 3/8, 5/8, re2fx_pc 0 bzw. 16/48 - die Ring-Sprites (Kanister-Matrix x Tafel @0x801000a8) fielen durch den
Regions-Test von FUN_80077924 (`jal 0x8002c820` @0x80077a30).
**RE2 (selbst gelesen):**
- Der RE2-Alligator hat keine Wandklemme (Dossier B 13.0: Root @0x80100190-3a4 und Hauptschleife 0x800267c0-930 ohne Wand-Aufruf,
  FUN_80015cb8 setzt die Lage absolut ohne Wandkollision). Kopf und Kanister bleiben im Raum, weil er auf den Korridor-Beinen schwimmt
  (Bein 2 x -23078: `addiu v0,zero,-23078 / sw v0,56(s0)` @0x80101048-4c) und der Kanister auf diesem Bein liegt (ROOM40A0 sub05
  `Pos_set (-22816,-1984,-9500)` @SCD 0x0484). Die Regions-Vierecke der R1-Schnitte umfassen das Bein mit Rand (ROOM40A0.RDT RVD,
  Werkzeug B `tools/r41_re2gator/rvd_40a0.py`-Zerlegung: Cut 5 Anker @0x00388 (-28300,-28201) (-28899,4105) (-20878,2616) (-18491,-27607)
  -> bei z -10000 x -28638..-19883; Cut 6 Anker @0x003C4 x -29405..-10899 z -16001..5606) - in RE2 liegen Kanister und Spritzring beim R1
  immer im Viereck.
- Objekt-Zeichner FUN_800363f0 (info/re2leon/PSX.EXE, selbst disassembliert): je Objekt (Basis 0x800d0324, Schritt 504) Matrix
  zusammensetzen (`jal 0x8002ce94` @0x80036484 -> Weltmatrix +84, Lage +104), dann `lw a1,-8172(s4)` (Viereck des aktiven Cuts 0x800ce014)
  / `jal 0x8002c820` mit a0 = s3 = obj+104 @0x80036530-34; `bne v0,zero,0x80036558` (im Viereck -> zeichnen), sonst `lw v0,0(s1) /
  andi v0,v0,0x10 / beq v0,zero,0x800366a4` @0x80036544-50 = ohne Flag 0x10 weiter zum naechsten Objekt: KEIN Mesh, KEINE Scherben
  (@0x80036558-b4) und KEIN Scherben-Zaehler (+0x1EC += 1 erst @0x800365bc-c8). FUN_8002c820 = Punkt-im-Viereck (RE2_Quellcode_V2/
  FUN_8002c820.c), wie re15_aot_point_in_quad.
- Flagwort des Kanisters obj[1]: Obj_model_set-Handler 0x80055260 (Op-Tafel 0x800a74c8[0x2D]): `lhu v0,10(s2) / ori v0,v0,0x1 /
  sll v0,v0,16 / sra v0,v0,16 / sw v0,0(s1)` @0x800552d0-e4. ROOM40A0.RDT @0x2A12 `2d 01 00 00 00 00 00 00 00 00 0a 00 00 00 b8 ac ...`
  -> word0 = 0x000A | 1 = 0x000B: KEINE Ausnahme 0x10 - der Regions-Test gilt fuer den Kanister.
**Aenderung:**
1. KOPF-KLEMME (PORT-WAHL; Form = der Bahn-Massstab der Spur B gr2_bahn_frei "Hals/Kopf/Kiefer Bones 5/6/7 im Wasser", Werkzeug = die
   Wand-Klemme, die nur die Lage verschiebt): solange der Kanister im Maul haengt (Stufe 3..7 nach dem Anhaengen) und im R1-Tod liegen
   Hals, Kopf, Kiefer (Bones 5/6/7 der gezeichneten Pose, gr2_pose_gezeichnet) und der Kanister (Part 6 x Lokallage) innerhalb der
   Aussenwaende x[-8900..7200] z[-27000..-5400]; ragt ein Punkt hinaus, wird der ganze Alligator (Lage + Anker) um den Ueberstand nach
   innen geschoben. Ohne die Bloecke Plattform/Rampe (GUARD-Hochbiss und CROSS-Klettern bleiben unberuehrt), nicht im FRESSEN (eigene
   Buehne der Spur B) und nicht bei abgeschalteter Klemme (gb_klemme_aus: CROSS/Befreiung). Dateien: `engine/src/gator_re2_r41.c`
   re15_gr2_kopf_klemme / gr2_kk_rechnen / gr2_kk_takt (dieselbe Klemme in der R1-Vorausschau gr2_r1_pfad), Haken (2b) in
   `engine/src/enemy_ai_boss_gator.c` re15_gator_boss_tick nach der Wurzel-Klemme, vor re15_gr2_nachtakt (6 Zeilen, markiert);
   Messzeilen `KOPF-KLEMME an/aus` in gator_boss.log.
2. REGIONS-TEST des Kanister-Zeichners (`platform/pc/src/gator_re2_pc.c` re15_gr2_pc_zeichnen): ausserhalb des Vierecks des aktiven
   Cuts (re15_rdt_get_region_quad + re15_aot_point_in_quad = FUN_8002c820) weder Mesh noch Scherben noch Scherben-Zaehler; Messzeile
   `[gator-re2] Kanister F<n> im/AUSSERHALB des Regions-Viereck(s) von Cut <c>` bei jedem Wechsel.
**Messung nachher** (Messkopie re15_pc_g41nb4b.exe = Code dieses Abschnitts, Rezepte der Abnahme 3 unveraendert (`<SP>/g4/nach.sh` liest
deren env.txt); die vier Elza-Laeufe mit Haken HP erreichten den R1 nicht mehr in EXIT_AT 3000 (11.5) und wurden mit 250 Schussbloecken
und EXIT_AT 9000 nachgefahren (`<SP>/g4/runs/c_*`); Auswertung `<SP>/g4/r1sum.py`):

| Lauf | R1 (Bild nach der Rueckkehr) | Kanister beim R1 vorher (Abnahme 3) | Kanister beim R1 nachher | Start nachher | Spritzring re2fx_pc max vorher -> nachher | Kopf-Klemme-Episoden |
|---|---|---|---|---|---|---|
| k_leon | F814 | (1874,-20754) | (-1075,-6208) innen | 8/8 | 96 -> 96 | 10 |
| k_elza | F3766 | (-9986,-4622) AUSSEN | (-7172,-11242) innen | 8/8 | 64 -> 176 | 137 |
| d15_leon | F987 | (-7394,-11396) | (6466,-6345) innen | 8/8 | 96 -> 96 | 17 |
| d15_elza | F4651 | (-7374,-11400) | (-6097,-22283) innen | 8/8 | 96 -> 96 | 147 |
| west_leon | F322 | (-10831,-27801) AUSSEN | (-6615,-9182) innen | 8/8 | 16 -> 96 | 6 |
| west_elza | F3274 | (-3482,-10330) | (5304,-23991) innen | 8/8 | 96 -> 96 | 81 |
| plat_leon | F1204 | (5402,-11294) | (7200,-6844) innen | 8/8 | 96 -> 96 | 2 |
| plat_elza | F3397 | (-10422,-4136) AUSSEN | (7200,-9856) innen | 8/8 | 48 -> 96 | 116 |
| **v1_leon (kein Haken)** | F1215 | **(5996,-29290) AUSSEN** | (5940,-19569) innen | **8/8** | **0 -> 96** (12 Bilder) | 31 |
| v1_elza (kein Haken) | F2167 | (8714,-26043) AUSSEN | (7200,-22497) innen | 8/8 | 48 -> 96 | 81 |

In allen zehn Verlaeufen: Raum-ESP ROOM40A0 registriert, op_unbekannt 0, zehn Scherben-Bilder 180..1530, Regions-Test des Kanisters
"im Regions-Viereck" (kein AUSSERHALB in den R1-Bildern). Die R1-Lagen haben sich verschoben, weil der Alligator mit dem Kanister nicht
mehr mit dem Kopf durch die Wand schwimmt (k_leon R1 jetzt F814 im Nordbecken statt F2290 im Suedbecken).
Riegel: `unit_r41_kanister2080_kopfklemme` (Teil "kopfklemme" im Testprogramm der Spur B, echter Boss-Takt): vor dem Takt meldet die
Klemme fuer Wurzel (2000,-25400) Gier 0x400 mit Kanister den Ueberstand (0,3260) = Kopf/Kanister 3260 jenseits der Suedwand; nach jedem
der 30 Takte Kanister z >= -27000 (min -27000); R1-Start Kanister (2077,-1937,-26418) mit 6/6 Spritzring-Punkten im Regions-Viereck;
Todes-Clip 38 Bilder Bones 5/6/7 innerhalb; Gegenprobe ohne Kanister: keine Klemme. `integration_r41_kanister2080_kampf` prueft in allen
fuenf Laeufen Kanister beim R1 innerhalb der Aussenwaende und Spritzring-Quads > 0 (11.5).

### 11.4 M1 - Plattform-Verlauf: Todes-Clip nur 6/19 im Bild
**Ursache (Code + Abnahme-Lauf plat_leon F1278, gator_boss.log):** gr2_r1_wahl bewertete (1) gueltigen Start VOR (2) sichtbaren Proben, und
Stufe 0 schloss jeden Schnitt ueber Spur B's Massstab-Grenze aus. Kandidaten: laufender Cut 4 Start 8/8, Todes-Clip 6/19 (ab F1290 Kopf
hinter dem Maschinenblock); Cut 3 Start 8/8, 19/19, Massstab 2158 > Grenze 1892; Cut 2 Start 2/8, 16/19 -> Wahl Cut 4.
**Beleg / Mass:** G-E1 (AUFTRAG 12:08): "die Zonenkamera, in der Kopf und Kiefer mit Rand im Bild liegen und von keiner Maske verdeckt
sind, am naechsten an der RE2-Lage" - keine Massstab-Grenze; Entscheid B8 ("die Massstabsgrenze (groesster RE2-Schnitt) bleibt") gilt
fuer die Kamera-Programme der Spur B und bleibt hier Vorrang-Regel, nicht Sperre. RE2-Form: R1 P0 schneidet nur, wenn der laufende Cut
den Kopf nicht zeigt (`addiu v0,zero,6 / beq` ... `slti v0,v0,-13300` @0x80102218-34, +0x218 |= 0x40 @0x80102240-4c) - der Port
bleibt bei der Wahl = laufender Cut ohne Schnitt (unveraendert).
**Aenderung** (`engine/src/gator_re2_r41.c`, markiert "Nachbesserung 4 (M1)"): Rangfolge als reine Funktion re15_gr2_r1_rang (Header
re15_gator_re2_r41.h, Typ re15_gr2_r1_kand_t): VOLL = Kopf und Kiefer im Start und in allen Proben des Todes-Clips. Stufe 0: volle Cuts
unter der Grenze (oder der laufende); Stufe 1: volle Cuts ohne Grenze; Stufe 2 (kein voller Cut): die meisten Proben zuerst. Innerhalb:
gueltiger Start, meiste Start-Punkte, Kopf naechst (170,111). gr2_r1_wahl beurteilt jede Zonenkamera einmal (vorher je Stufe erneut) und
schreibt "Kandidat ... voll" und "Wahl Stufe s: Cut c" in gator_boss.log.
**Messung nachher:** Riegel `unit_r41_kanister2080_r1wahl` mit der Kandidatentabelle des Abnahme-Laufs plat_leon F1278 (Zahlen aus dessen
gator_boss.log): `r1wahl A (plat_leon F1278, laufender Cut 4 mit 6/19): Wahl Cut 3 Stufe 1` (vorher Cut 4); dazu drei Grenzfaelle (naeher
an RE2, Grenze bevorzugt, kein voller Cut -> meiste Proben). Im Spiel (Tabelle 11.3, alle zehn Verlaeufe): Kopf ok, Kiefer ok, Todes-Clip
19/19 Proben, gewaehlt in Stufe 0; plat_leon jetzt R1 F1204 bei (3541,-8901): laufender Cut 4 mit 8/8 und 19/19 -> ohne Schnitt; plat_elza
F3397: Schnitt Cut 4 8/8 19/19. Riegel `integration_r41_kanister2080_kampf` um den Lauf plattform_leon (Leon auf (0,-16000)) ergaenzt (11.5).

