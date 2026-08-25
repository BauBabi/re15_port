# EM36 (RE2-Endgegner Typ 0x36, ROOM7040/G040) — Zustand 0 = INIT, FUN 0x801003CC, Datei build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN, Ladebasis 0x80100000, Dateioffset 0x03CC..0x0784 (230 Instruktionen, 0x801003CC..0x80100780)

0x801003CC ist eine VOLLSTAENDIG LINEARE Ein-Tick-INIT-Routine ohne jeden Unter-Dispatch: kein `lbu +0x5`, kein `lbu +0x6`, kein `sll 2`/`lw TABELLE` in der ganzen Funktion. Sie enthaelt genau ZWEI bedingte Sprünge (HP-Wahl @0x80100410, Modell-Alloc-Gate @0x80100644) und vier `jal`. Sie setzt in dieser Reihenfolge: Zustandswort +0x04=1 (Wort-Store, loescht +0x5/+0x6/+0x7 mit), Timer/Trefferfelder +0x1C8/+0x1CC/+0x1D0/+0x1D3=0, HP +0x156 = 600 bzw. 400 bei `[0x800CFB74]&0x20`, die drei Zerlege-Pools +0x151/+0x152/+0x153 = -126, +0x1C0=1 / +0x1C1=1 (Wurzel-Part-Index 1), Yaw +0x76=0, ZWEI Kollisions-/Hitbox-Records (Basis Entity+0x84, Stride 0x20, Zaehler +0x1E8 = 2) mit r=6000/h=1500/Offset X=-2000 bzw. r=1300/h=1500/Offset X=+2200, die Schuss-Rasterhoehe +0x1EE=5700, den Startclip +0x14C=0 (Wort: Clip 0, Frame 0, Rate 0) und den Spielerabstand +0x1F0=30000. Danach installiert sie die beiden EXE-Spielerhaken 0x800CE300[0x36]=0x80103880 (Griff/Struggle) und 0x800CE400[0x36]=0x801038CC (Gefressen/Kollaps), den Zeichen-Callback +0x208=0x801003AC, allokiert den Bodenschatten (4000x4000) ueber FUN_80016480 nach +0x16C, laedt einmalig die zwei Zusatz-Modelle ueber 0x80103AA0 aus dem Bump-Allokator 0x800CE32C, und verkettet FUENF Zusatz-Parts (2,3 an Part 0; 4,5,6 an Part 2) mit ihren lokalen Ankeroffsets. Letzte Instruktion vor dem Epilog ist `sb 2,5(s0)` @0x8010076C: der naechste Tick laeuft in Zustand 1 (0x80100784), der NICHT auf +0x5, sondern auf `+0x10E & 0x3F` dispatcht (Tabelle @0x80105648).

## Konstanten

| Name | Wert | Bedeutung | Adresse |
|---|---|---|---|
| state_word | 1 (Wort -> +0x04=1, +0x05=0, +0x06=0, +0x07=0) | Feld +0x04 Zustand; Wort-Store loescht Unterzustand/Phase mit | @0x801003D8 (addiu v0,zero,1) / @0x801003E8 (sw v0,4(s0)) |
| f1d0_clear | 0 | Feld +0x1D0 (RE2-Trefferrichtungs-Stempel) — im ganzen Overlay sonst NIE geschrieben | @0x801003EC (sh zero,464(s0)) |
| f1cc_clear | 0 | Feld +0x1CC (Speed_set-Skalar speed_h) | @0x801003F0 (sh zero,460(s0)) |
| f1c8_clear | 0 | Feld +0x1C8 | @0x801003F4 (sh zero,456(s0)) |
| f1d3_clear | 0 | Feld +0x1D3 Timer/Riegel (die AI-Wurzel dekrementiert (+0x1D3 & 0x7F) jeden Tick @0x801000EC-0x80100100) | @0x801003F8 (sb zero,467(s0)) |
| HP_default | 600 | Feld +0x156 HP | @0x801003FC (addiu v0,zero,600) / @0x80100400 (sh v0,342(s0)) |
| HP_alt | 400 | Feld +0x156 HP, nur wenn [0x800CFB74] & 0x20 | @0x80100414 (addiu v0,zero,400) / @0x80100418 (sh v0,342(s0)); Gate @0x8010040C andi 0x20 / @0x80100410 beq |
| anim_flag_a3 | 256 (0x100) | a3 = param_4 von FUN_8002959C; (param_4 & 0xFFFF0000)==0 -> Frame wird VORWAERTS aus +0x14D genommen (kein Rueckwaerts-Zweig) | @0x80100420 (addiu a3,zero,256) |
| pool151 | -126 (0x82) | Feld +0x151 Zerlege-Pool hohe Zone (Region 2 des EXE-Appliers @0x80041994-0x800419CC). Im Overlay NIE gelesen | @0x80100424 (addiu v0,zero,-126) / @0x80100428 (sb v0,337(s0)) |
| pool152 | -126 (0x82) | Feld +0x152 Zerlege-Pool mittlere Zone (Region 1, Abzug @0x80041954-0x80041988) | @0x8010042C (sb v0,338(s0)) |
| pool153 | -126 (0x82) | Feld +0x153 Zerlege-Pool tiefe Zone (Region 0, Abzug @0x80041910-0x80041944) | @0x80100430 (sb v0,339(s0)) |
| f1c0 | 1 | Feld +0x1C0 Bit 0 (Koerper-Schub-Flagbyte; FUN_800177C0 testet `+0x1C0 & 1` @0x800177DC) | @0x80100434 (addiu v0,zero,1) / @0x80100438 (sb v0,448(s0)) |
| f1c1_rootpart | 1 | Feld +0x1C1 = Index des Parts, dessen Matrix-Translation (Part*0xAC+0x5C) die WELTPOSITION der Entity ist (`pool + +0x1C1*0xac + 0x5c`, FUN_800177C0 / FUN_8003DB38 / FUN_800410CC) | @0x8010043C (sb v0,449(s0)) |
| yaw | 0 | Feld +0x76 Yaw | @0x8010044C (sh zero,118(s0)) |
| part1_98 | 0 | Modell-Pool [+0x198] Part 1 +0x98 (Part-Hitbox-/Physik-Metadaten; gleiche Familie wie der RE2-Hund-INIT @0x801002D0-F8) | @0x80100468 (sh zero,324(v1)); v1 = [Entity+0x198] @0x80100444 |
| part1_9A | 0 | Modell-Pool Part 1 +0x9A | @0x8010046C (sh zero,326(v1)) |
| part1_9C | 8 | Modell-Pool Part 1 +0x9C | @0x80100440 (addiu v0,zero,8) / @0x80100450 (sh v0,328(v1)) |
| part1_9E | 8 | Modell-Pool Part 1 +0x9E | @0x80100454 (sh v0,330(v1)) |
| part1_A0 | 212 | Modell-Pool Part 1 +0xA0 | @0x80100458 (addiu v0,zero,212) / @0x8010045C (sh v0,332(v1)) |
| part1_A2 | 212 | Modell-Pool Part 1 +0xA2 | @0x80100460 (sh v0,334(v1)) |
| f218 | 0 | Feld +0x218 Arbeitsbyte (Overlay-Schreiber u.a. @0x80100928, @0x80101208, @0x80101780, @0x80101D20, @0x801030E4) | @0x80100474 (sb zero,536(s0)) |
| f219 | 0 | Feld +0x219 Arbeitsbyte (42 Zugriffe im Overlay) | @0x80100478 (sb zero,537(s0)) |
| f21A | 0 | Feld +0x21A — im ganzen Overlay NUR hier geschrieben, nie gelesen | @0x8010047C (sb zero,538(s0)) |
| f21C | 0 (Halbwort) | Feld +0x21C/+0x21D Arbeitswort (36 Zugriffe) | @0x80100480 (sh zero,540(s0)) |
| f21B | 0 | Feld +0x21B Arbeitsbyte (31 Zugriffe) | @0x80100484 (sb zero,539(s0)) |
| f223 | 0 | Feld +0x223 (Leser/Schreiber @0x80104EC4-0x80104FA0) | @0x80100488 (sb zero,547(s0)) |
| f224 | 0 | Feld +0x224 (Leser/Schreiber @0x80104ED4-0x80104FE4) | @0x8010048C (sb zero,548(s0)) |
| f225 | 0 | Feld +0x225 (Schreiber @0x801029D0, @0x80102FF8, @0x801040B4) | @0x80100490 (sb zero,549(s0)) |
| f228 | 0 | Feld +0x228 (Leser @0x801009F8, @0x80100AF4, @0x80100C00, @0x80101998, @0x801033C4, @0x80103590) | @0x80100494 (sb zero,552(s0)) |
| f229 | 0 | Feld +0x229 = allgemeiner Tick-Countdown; die AI-Wurzel dekrementiert ihn NACH dem Zustands-jalr @0x80100188-0x80100198 | @0x80100498 (sb zero,553(s0)) |
| ai_dist_seed | 30000 | Feld +0x1F0 Spielerabstand — Startwert 'sehr weit'; alle Abstands-Vergleiche des Overlays lesen 496(sX) | @0x80100464 (addiu v0,zero,30000) / @0x8010049C (sw v0,496(s0)) |
| bss_rec0_clear | 0 | Overlay-BSS-Record 0 @0x80105610: 4 Bytes +0x00..+0x03 und 4 Halbworte +0x04/+0x06/+0x08/+0x0A. Der Record ist 0x1C gross; +0x0C = 0x801055EC (Byteliste {9|03 04 7A 7B 85 96 C2 C9 CD}), +0x10 = 0x801055F8 ({2|DA DB}), +0x14 = 0x0FF10F02, +0x18 = 0x000000F1 | @0x801004A4 / @0x801004AC / @0x801004B4 / @0x801004BC (sb) ; @0x801004C4 / @0x801004CC / @0x801004D4 / @0x801004DC (sh) |
| bss_rec1_clear | 0 | Overlay-BSS-Record 1 @0x8010562C (Stride 0x1C): dieselben 12 Bytes. +0x0C = 0x801055FC ({8|00 01 02 3B 3C 61 BD BE}), +0x10 = 0x80105608 ({4|27 35 5B 5C}), +0x14 = 0x0FF10F02, +0x18 = 0x000000F1 | @0x801004E4 / @0x801004EC / @0x801004F4 / @0x801004FC (sb) ; @0x80100504 / @0x8010050C / @0x80100514 / @0x8010051C (sh) |
| hb0_yoff | -1500 | Kollisions-Record 0 (Basis Entity+0x84, Stride 0x20) rec+0x14 = Entity+0x98 = Y-Mittenversatz. Beleg der Struktur: Doppelschleife @0x80034D74/@0x80034DB0 mit Cursor s5=Entity+0x84, Zaehler Entity+0x1E8, Increment `addiu s5,s5,0x20` @0x80035100 | @0x80100470 (addiu v1,zero,-1500) / @0x80100520 (sh v1,152(s0)) |
| hb1_yoff | -1500 | Kollisions-Record 1 rec+0x14 = Entity+0xB8 | @0x80100524 (sh v1,184(s0)) |
| hb_count | 2 | Feld +0x1E8 = ANZAHL der Kollisions-Records ab Entity+0x84 (Spieler-INIT FUN_8003BAF0 setzt hier 1 @0x8003BDDC und fuellt nur Record 0) | @0x8010052C (addiu v0,zero,2) / @0x80100530 (sw v0,488(s0)) |
| hb0_offX | -2000 | Kollisions-Record 0 rec+0x10 = Entity+0x94 = lokaler Mittenversatz X (Spieler: 0) | @0x80100534 (addiu v0,zero,-2000) / @0x80100538 (sh v0,148(s0)) |
| hb0_radius | 6000 | Kollisions-Record 0 rec+0x16 = Entity+0x9A = RADIUS (gelesen `lhu s7,0x16(s5)` @0x80034DAC; Spieler 450); rec+0x18 = Entity+0x9C = zweiter Radius | @0x8010053C (addiu v0,zero,6000) / @0x80100540 (sh v0,154(s0) = +0x9A) / @0x80100544 (sh v0,156(s0) = +0x9C) |
| hb0_pushradius | 6000 | Kollisions-Record 0 rec+0x0C/+0x0E = Entity+0x90/+0x92 = zweites Radiuspaar (Spieler-INIT setzt hier 0x1C2/0x1C2 @0x8003BDCC/@0x8003BDD4) | @0x80100548 (sh v0,144(s0)) / @0x8010054C (sh v0,146(s0)) |
| hb1_offX | 2200 | Kollisions-Record 1 rec+0x10 = Entity+0xB4 = lokaler Mittenversatz X | @0x80100550 (addiu v0,zero,2200) / @0x80100554 (sh v0,180(s0)) |
| hb1_radius | 1300 | Kollisions-Record 1 rec+0x16/+0x18 = Entity+0xBA/+0xBC = RADIUS | @0x80100558 (addiu v0,zero,1300) / @0x8010055C (sh v0,186(s0)) / @0x80100560 (sh v0,188(s0)) |
| hb1_pushradius | 1300 | Kollisions-Record 1 rec+0x0C/+0x0E = Entity+0xB0/+0xB2 | @0x80100564 (sh v0,176(s0)) / @0x80100568 (sh v0,178(s0)) |
| flags_or_8 | +0x00 |= 0x00000008 | Entity-Flagwort +0x00 | @0x80100528 (lw v1,0(s0)) / @0x8010056C (ori v1,v1,0x8) / @0x80100570 (sw v1,0(s0)) |
| hb0_offZ | 0 | Kollisions-Record 0 rec+0x12 = Entity+0x96 = lokaler Mittenversatz Z | @0x8010057C (sh zero,150(s0)) |
| hb0_height | 1500 | Kollisions-Record 0 rec+0x1A = Entity+0x9E = HOEHE (gelesen `lhu a0,0x1a(s2)` / `lhu v0,0x1a(s5)` @0x80034E50/@0x80034E5C; Spieler 1530) | @0x80100448 (addiu t0,zero,1500) / @0x80100580 (sh t0,158(s0)) |
| hb1_offZ | 0 | Kollisions-Record 1 rec+0x12 = Entity+0xB6 | @0x80100584 (sh zero,182(s0)) |
| hb1_height | 1500 | Kollisions-Record 1 rec+0x1A = Entity+0xBE = HOEHE | @0x80100588 (sh t0,190(s0)) |
| aim_height | 5700 | Feld +0x1EE — der EXE-Schuss-/Zielaufloeser FUN_800410CC hebt das Trefferrechteck um (+0x1EE >> 2) an (`lhu v0,0x1ee(s2)` @0x8004133C, @0x800417A0; Zwillinge @0x800471BC/D4, @0x800473DC/F4). Im Overlay NIE gelesen | @0x80100578 (addiu v0,zero,5700) / @0x8010058C (sh v0,494(s0)) |
| start_clip | 0 (Wort 0x00000000) | Feld +0x14C Clip-Wort = rate<<16 | frame<<8 | clip -> Clip 0, Frame 0, Rate 0. (+0x14C = Clip, +0x14D = Frame; Beleg FUN_8002959C liest param_1+0x14C/+0x14D, FUN_80029614 liest DAT_800CE330+0x14C) | @0x80100590 (sw zero,332(s0)) |
| flags_or_2 | +0x00 |= 0x00000002 | Entity-Flagwort +0x00. Bit 0x2 schliesst das Paar aus der Kollisions-Doppelschleife aus (`or v0,a0,v1 / andi v0,v0,0x2 / bne -> skip` @0x80034D48-0x80034D50) | @0x80100574 (lw v1,0(s0)) / @0x80100594 (ori v1,v1,0x2) / @0x80100598 (sw v1,0(s0)) |
| flags_or_0C000000 | +0x00 |= 0x0C000000 | Entity-Flagwort +0x00 Bits 26/27 -> (+0x00 >> 26) & 7 == 3; der Schuss-Aufloeser FUN_800410CC benutzt genau diesen 3-Bit-Wert als Trefferzonen-Klasse (`srl v0,v0,0x1a` @0x800413CC) | @0x8010059C (lw v0,0(s0)) / @0x801005A0 (lui v1,0xc00) / @0x801005A4 (or v0,v0,v1) / @0x801005AC (sw v0,0(s0), Delay-Slot des jal) |
| shadow_anchor | [Entity+0x198] + 92 (= Part0 +0x5C = Weltmatrix-Translation von Part 0) | 5. Argument (sp+16) von FUN_80016480 -> Record[5] (=rec+0x14) | @0x80100608 (addiu v1,v1,92) / @0x80100614 (sw v1,16(sp)) |
| shadow_size | 0x0FA00FA0 (= 4000 / 4000) | a2 = param_3 von FUN_80016480 -> Record[1]; Bodenschatten-Ausdehnung | @0x801005B8 (lui a2,0x0fa0) / @0x801005BC (ori a2,a2,0xfa0) |
| hook_cmd5 | 0x80103880 | 0x800CE300 + Typ*4 (Typ = Entity+0x08 = 0x36 -> 0x800CE3D8) = Spieler-Handler Griff/Struggle | @0x801005C0/@0x801005C4 (lui/addiu v1 = 0x80103880) / @0x801005C8 (lbu v0,8(s0)) / @0x801005DC (sw v1,-7424(at)) |
| hook_cmd6 | 0x801038CC | 0x800CE400 + Typ*4 (-> 0x800CE4D8) = Spieler-Handler Gefressen/Kollaps | @0x801005E4/@0x801005E8 (lui/addiu v1 = 0x801038CC) / @0x801005E0 (lbu v0,8(s0)) / @0x801005F8 (sw v1,-7168(at)) |
| flags_or_4 | +0x00 |= 0x00000004 | Entity-Flagwort +0x00 | @0x801005FC (lw v0,0(s0)) / @0x80100604 (ori v0,v0,0x4) / @0x8010060C (sw v0,0(s0)) |
| part0_flag_4000 | Part0 +0x00 |= 0x4000 | Flagwort des Modell-Parts 0 ([Entity+0x198] +0x00). Zwilling im Spieler-INIT: `**(uint**)(param_1+0x198) |= 0x4000` (FUN_8003BAF0) | @0x80100618 (lw v1,408(s0)) / @0x80100620 (lw v0,0(v1)) / @0x80100628 (ori v0,v0,0x4000) / @0x8010062C (sw v0,0(v1)) |
| draw_callback | 0x801003AC | Feld +0x208 = Pro-Frame-Callback (Thunk -> 0x80103AF4). Zwilling: Spieler-INIT setzt +0x208 = 0x8003BF7C | @0x80100664/@0x80100668 (lui/addiu v0 = 0x801003AC) / @0x8010066C (sw v0,520(s0)) |
| part2_link | Part2 +0x74 = Pool+72 (= Part0 +0x48), Part2 +0x94 = Pool+0 (= Part0), Part2 +0x00 = 1 | Zusatz-Part 2 haengt an Part 0. +0x74 = Zeiger auf die ELTERN-MATRIX (Part+0x48), +0x94 = Zeiger auf den ELTERN-RECORD (Test `(*eltern & 0x21) == 0x20` @0x80027470-0x80027488, SetRotMatrix/SetTransMatrix auf +0x74 @0x800274A4/@0x800274CC), +0x00 Bit 0 = Part vorhanden | @0x80100678 (addiu v1,v0,72) / @0x8010067C (sw v1,460(v0)) / @0x80100690 (sw v0,492(v0)) / @0x80100674 (addiu a2,zero,1) / @0x8010068C (sw a2,344(v0)) |
| part2_anchor | X=1800, Y=4500, Z=0 | Zusatz-Part 2 lokaler Ankeroffset +0x2C/+0x30/+0x34 (Leser: `lw v0,0x2c(s1)` @0x800275F8, `lw v0,0x30(s1)` @0x80027648, `lw v0,0x34(s1)` @0x8010766C; jeweils * Eltern+0x8C/+0x8E/+0x90 >> 12) | @0x80100680 (addiu v1,zero,1800) / @0x80100684 (sw v1,388(v0)) ; @0x80100688 (addiu v1,zero,4500) / @0x80100694 (sw v1,392(v0)) ; @0x80100698 (sw zero,396(v0)) |
| part3_link | Part3 +0x74 = Pool+72 (Part0+0x48), Part3 +0x94 = Pool+0 (Part0), Part3 +0x00 = 1 | Zusatz-Part 3 haengt ebenfalls an Part 0 | @0x801006A4 (addiu v1,v0,72) / @0x801006A8 (sw v1,632(v0)) / @0x801006BC (sw v0,664(v0)) / @0x801006B8 (sw a2,516(v0)) |
| part3_anchor | X=800, Y=500, Z=0 | Zusatz-Part 3 lokaler Ankeroffset +0x2C/+0x30/+0x34 | @0x801006AC (addiu v1,zero,800) / @0x801006B0 (sw v1,560(v0)) ; @0x801006B4 (addiu v1,zero,500) / @0x801006C0 (sw v1,564(v0)) ; @0x801006C4 (sw zero,568(v0)) |
| part4_link | Part4 +0x74 = Pool+416 (= Part2 +0x48), Part4 +0x94 = Pool+344 (= Part2), Part4 +0x00 = 1 | Zusatz-Part 4 haengt an Part 2 (zweite Kettenstufe) | @0x801006D0 (addiu a1,v0,416) / @0x801006D4 (addiu a0,v0,344) / @0x801006E4 (sw a1,804(v0)) / @0x801006E8 (sw a0,836(v0)) / @0x801006E0 (sw a2,688(v0)) |
| part4_anchor | X=1698, Y=-1398, Z=0 | Zusatz-Part 4 lokaler Ankeroffset +0x2C/+0x30/+0x34 | @0x801006CC (addiu v1,zero,1698) / @0x801006D8 (sw v1,732(v0)) ; @0x801006DC (addiu v1,zero,-1398) / @0x801006EC (sw v1,736(v0)) ; @0x801006F0 (sw zero,740(v0)) |
| part5_link | Part5 +0x74 = Pool+416 (Part2+0x48), Part5 +0x94 = Pool+344 (Part2), Part5 +0x00 = 1 | Zusatz-Part 5 haengt an Part 2 | @0x8010070C (sw a2,860(v1)) / @0x80100710 (sw a1,976(v1)) / @0x80100714 (sw a0,1008(v1)) |
| part5_anchor | X=1432, Y=-2691, Z=1023 | Zusatz-Part 5 lokaler Ankeroffset +0x2C/+0x30/+0x34 | @0x801006F8 (addiu v0,zero,1432) / @0x801006FC (sw v0,904(v1)) ; @0x80100700 (addiu v0,zero,-2691) / @0x80100704 (sw v0,908(v1)) ; @0x80100708 (addiu v0,zero,1023) / @0x80100718 (sw v0,912(v1)) |
| part6_link | Part6 +0x74 = Pool+416 (Part2+0x48), Part6 +0x94 = Pool+344 (Part2), Part6 +0x00 = 1 | Zusatz-Part 6 haengt an Part 2 | @0x80100724 (sw a2,1032(v1)) / @0x80100728 (sw a1,1148(v1)) / @0x8010072C (sw a0,1180(v1)) |
| part6_anchor | X=1430, Y=-2284, Z=-2078 | Zusatz-Part 6 lokaler Ankeroffset +0x2C/+0x30/+0x34 | @0x80100720 (addiu v0,zero,1430) / @0x80100730 (sw v0,1076(v1)) ; @0x80100734 (addiu v0,zero,-2284) / @0x80100738 (sw v0,1080(v1)) ; @0x8010073C (addiu v0,zero,-2078) / @0x80100740 (sw v0,1084(v1)) |
| part2_sub_clear | 0 / 0 | [Part2 +0x88] +0x0C = 0 und [Part2 +0x88] +0x10 = 0. Part2+0x88 ist ein Zeiger; genau ihn liest FUN_8004BAF0 als erstes (`piVar21 = *(int**)(param_2+0x88)`) | @0x80100744 (lw a1,408(s0)) / @0x8010074C (lw v0,480(a1)) / @0x80100754 (sh zero,12(v0)) / @0x80100758 (lw v0,480(a1)) / @0x80100764 (sh zero,16(v0)) |
| handoff_sub | 2 | Feld +0x05 Unterzustand fuer den naechsten Tick (Zustand 1) | @0x80100768 (addiu v0,zero,2) / @0x8010076C (sb v0,5(s0)) |
| state_table | {0x801003CC, 0x80100784, 0x801025BC, 0x80102BBC, 0x80103834, 0x00000000, 0x00000000, 0x80103878} | Zustandstabelle, indiziert mit Entity+0x04 | @0x801055CC (Dispatch @0x80100164-0x80100180) |
| variant_table | {0x801007C4, 0x80100868, 0x80100D08, 0x80101108, 0x80101808, 0x80101AA0, 0x80100960, 0x80100D10, 0x80101110, 0x80101810, ...} | Tabelle von Zustand 1, indiziert mit (Entity+0x10E & 0x3F) | @0x80105648 (Dispatch @0x8010078C-0x801007AC) |
| grab5_table | {0x801038BC (leerer Stub: jr ra/nop), 0x80103908} | Untertabelle des cmd-5-Handlers, indiziert mit SPIELER+0x05 | @0x801056E8 (Dispatch @0x80103888-0x801038A4) |
| grab6_table | {0x80103908, ...} | Untertabelle des cmd-6-Handlers, indiziert mit SPIELER+0x05 | @0x801056EC (Dispatch @0x801038D4-0x801038F0) |
| model_desc | 0x801059D8 und 0x80105A44 | Die zwei Modelldeskriptoren, die 0x80103AA0 (Laden, FUN_80019628) und 0x80103AF4 (Zeichnen, FUN_800197F4) benutzen | @0x80103ABC / @0x80103AD0 (Laden) ; @0x80103B6C / @0x80103B80 (Zeichnen) |

## Uebergaenge

- **z0 p0 (INIT, Einsprung ueber Tabelle @0x801055CC[0]) -> z1 p2 (Handler 0x80100784)** wenn unbedingt, genau ein Tick
  - 801003d8: addiu v0,zero,1 / 801003e8: sw v0,4(s0)  -> +0x04=1, +0x05=0, +0x06=0, +0x07=0 ; danach am Funktionsende 80100768: addiu v0,zero,2 / 8010076c: sb v0,5(s0) -> +0x05=2
- **z1 (0x80100784) -> Variante = Tabelle @0x80105648[+0x10E & 0x3F]** wenn KEIN +0x5-Dispatch. z1 waehlt ueber den Spawn-Deskriptor +0x10E. Startwert +0x10E=0x8000 -> 0x8000&0x3F = 0 -> 0x801007C4
  - 8010078c: lhu v0,270(a0) / 80100794: andi v0,v0,0x3f / 80100798: sll v0,v0,2 / 801007a4: lw v0,22088(at)  ; Tabelle 0x80105648 = {0x801007C4,0x80100868,0x80100D08,0x80101108,0x80101808,0x80101AA0,0x80100960,0x80100D10,0x80101110,0x80101810,...}
- **z0 innen: HP-Wahl -> HP 600 ODER 400** wenn [0x800CFB74] & 0x20 == 0 -> 600 bleibt stehen; != 0 -> 400 ueberschreibt
  - 801003fc: addiu v0,zero,600 / 80100400: sh v0,342(s0) / 80100404: lw v0,0(v1) (v1=0x800CFB74 @801003dc-e0) / 8010040c: andi v0,v0,0x20 / 80100410: beq v0,zero,0x8010041c / 80100414: addiu v0,zero,400 (Delay-Slot) / 80100418: sh v0,342(s0)
- **z0 innen: Modell-Nachladen -> jal 0x80103AA0 ODER uebersprungen** wenn [[0x800CFE20]+0x3C] != 0 -> Sprung nach 0x80100664, Alloc entfaellt; == 0 -> laden
  - 80100630: lui v0,0x800d / 80100634: lw v0,-480(v0)  (=[0x800CFE20], Slot 3 der Entity-Liste 0x800CFE14) / 8010063c: lw v0,60(v0) / 80100644: bne v0,zero,0x80100664
- **z0 (installiert) -> Spieler-Griff-Handler** wenn Der EXE-Griff-Dispatcher ruft 0x800CE300[kind] bzw. 0x800CE400[kind]; kind = Entity+0x08 = 0x36 -> Eintraege 0x800CE3D8 / 0x800CE4D8
  - 801005c8: lbu v0,8(s0) / 801005d0: sll v0,v0,2 / 801005dc: sw v1,-7424(at)  (v1=0x80103880 @801005c0-c4)  ; 801005e0: lbu v0,8(s0) / 801005f8: sw v1,-7168(at) (v1=0x801038CC @801005e4-e8)
- **Griff-Handler 0x80103880 (cmd 5) -> Untermaschine ueber Spieler+0x5** wenn Tabelle @0x801056E8 + PL+0x5 * 4 = {0x801038BC (leerer jr-ra-Stub), 0x80103908}
  - 80103888: lbu v0,5(a0) / 80103890: sll v0,v0,2 / 8010389c: lw v0,22248(at) / 801038a4: jalr v0
- **Kollaps-Handler 0x801038CC (cmd 6) -> Untermaschine ueber Spieler+0x5** wenn Tabelle @0x801056EC + PL+0x5 * 4 = {0x80103908, ...}
  - 801038d4: lbu v0,5(a0) / 801038dc: sll v0,v0,2 / 801038e8: lw v0,22252(at) / 801038f0: jalr v0
- **z0 (installiert) -> Zeichen-/Pose-Callback jeden Frame** wenn +0x208 = 0x801003AC; 0x801003AC ist ein Thunk auf 0x80103AF4 (RotMatrix 0x8008E1F4 aus Entity+0x24, dann 4x FUN_8002CE94 fuer die Part-Ketten, dann 2x FUN_800197F4 mit den Modelldeskriptoren 0x801059D8 / 0x80105A44, dann FUN_80019CD0)
  - 80100664: lui v0,0x8010 / 80100668: addiu v0,v0,940 (=0x801003AC) / 8010066c: sw v0,520(s0)  ; 801003b4: jal 0x80103af4

## Aufrufe

- `@0x801005A8` -> `0x8002959C (Anim-/Keyframe-Aufloeser)` (a0 = Entity — gesetzt @0x8010041C (addu a0,s0,zero), s0 = a0 des INIT @0x801003D4. a1 = [Entity+0x108] — gesetzt vom AUFRUFER (AI-Wurzel) @0x80100160 (lw a1,264(s3)). a2 = [Entity+0x17C] — Aufrufer @0x80100168 (lw a2,380(s3)). a3 = 256 — @0x80100420. Delay-Slot @0x801005AC: sw v0,0(s0) (commit +0x00 |= 0x0C000000).) — FUN_8002959C: tbl = a2; e = *(u32*)(tbl + e[+0x14C]*4); da (a3 & 0xFFFF0000)==0 -> idx = e[+0x14D]; Entity+0x178 = tbl + (e>>16) + idx*4; dann FUN_80029614(a1). Setzt also den Keyframe-Zeiger fuer Clip 0 / Frame 0.
- `@0x80100610` -> `0x80016480 (Schatten-/Sprite-Record-Allokator, Pool 0x800CE698, 50 Slots a 104 Byte, Frei-Marker +0x76)` (a0 = Entity+0x16C — @0x801005B0 (addiu a0,s0,364). a1 = 0 — @0x801005B4 (addu a1,zero,zero). a2 = 0x0FA00FA0 — @0x801005B8 (lui a2,0x0fa0) + @0x801005BC (ori a2,a2,0xfa0). a3 = 0 — @0x801005CC (addu a3,zero,zero). arg5 = sp+16 = [Entity+0x198]+92 — @0x80100600 (lw v1,408(s0)) + @0x80100608 (addiu v1,v1,92) + @0x80100614 (sw v1,16(sp), Delay-Slot).) — Legt Entity+0x16C = Zeiger auf einen freien Pool-Record; rec+0x0E = 5, rec+0x14 = Part0+0x5C (Weltposition), rec+0x00 = 0, rec+0x04 = 0x0FA00FA0 (4000x4000), rec+0x1C und rec+0x44 Farbe = 0x00808080 (weil a3 == 0). Bodenschatten des Bosses.
- `@0x80100654` -> `0x80103AA0 (Overlay-lokal; ruft 2x FUN_80019628)` (a0 = Entity — @0x80100658 (addu a0,s0,zero, Delay-Slot). a1 = [0x800CE32C] (Bump-Allokator-Spitze) — @0x8010064C/@0x80100650 (lui a1,0x800d / lw a1,-7380(a1)). Rueckgabe v0 -> [0x800CE32C] @0x8010065C/@0x80100660. Nur ausgefuehrt, wenn [[0x800CFE20]+0x3C] == 0 (Gate @0x80100630-@0x80100648).) — 0x80103AA0 ruft FUN_80019628(a0=Entity, a1=[Entity+0x198], a2=0x801059D8, a3=Heap) @0x80103AC0 und danach FUN_80019628(a0=Entity, a1=[Entity+0x198], a2=0x80105A44, a3=Rueckgabe des ersten) @0x80103AD8. Laedt die beiden Zusatz-Modelle des Bosses in den Arbeitsspeicher und gibt die neue Heap-Spitze zurueck.
- `@0x80100760` -> `0x8004BAF0 (EXE; liest param_2+0x88 und param_1+0x108 / param_1+0x1BC)` (a0 = Entity — @0x80100750 (addu a0,s0,zero). a1 = [Entity+0x198] + 344 = Part 2 — @0x80100744 (lw a1,408(s0)) + @0x8010075C (addiu a1,a1,344). Delay-Slot @0x80100764: sh zero,16(v0) (letztes Clear von [Part2+0x88]+0x10).) — Abschliessender Skelett-/Pose-Aufbau der Zusatz-Part-Kette, verankert an Part 2 (dem Elternteil der Parts 4/5/6).
- `@0x801003B4 (im Callback-Thunk 0x801003AC, den INIT nach +0x208 schreibt)` -> `0x80103AF4` (a0 = Entity (durchgereicht). In 0x80103AF4: s2 = Entity, s0 = [Entity+0x198]; RotMatrix(0x8008E1F4)(a0=Entity+116, a1=Entity+36) @0x80103B18; dann 4x FUN_8002CE94(a0=Pool+72, a1/a2 = {Pool+24 | Pool+196/244 | Pool+368/416 | Pool+540/588}) @0x80103B2C/@0x80103B3C/@0x80103B4C/@0x80103B5C; dann FUN_800197F4(a0=Entity, a1=Pool, a2=0x801059D8) @0x80103B70 und FUN_800197F4(a0=Entity, a1=Pool, a2=0x80105A44) @0x80103B84; dann FUN_80019CD0(a0=Entity) @0x80103B90.) — Pro-Frame-Zeichnen/Posieren des Bosses inkl. der beiden Zusatz-Modelle.

## Offen
- 0x801003CC hat KEINEN Unter-Dispatch: kein `lbu +0x5`/`lbu +0x6`, kein `sll 2`/`lw TABELLE` zwischen 0x801003CC und 0x80100780. Der im Auftrag erwartete Unterzustands-Baum beginnt erst in Zustand 1 (0x80100784) und dispatcht dort auf `+0x10E & 0x3F`, NICHT auf +0x5.
- Der Wert -126 (0x82) in +0x151/+0x152/+0x153 ist SIGNED negativ. Beim RE2-Zombie startet dieser Pool bei +13 und `(s8)+0x152 < 0` ist das Zerleger-Gate. Der Boss startet also bereits 'unter Null'. Ob der EXE-Applier fuer Typ 0x36 diese Felder ueberhaupt auswertet, ist NICHT belegt — im Overlay wird 337/338/339 NUR im INIT geschrieben und nie gelesen.
- Die Semantik von Modell-Pool Part 1 +0x98/+0x9A/+0x9C/+0x9E/+0xA0/+0xA2 (0,0,8,8,212,212) ist nicht abschliessend belegt. Zwei belegte Kandidaten-Leser derselben Offsets im Part-Record: FUN_8002940C (Farbpaar, aktiv bei Part-Flag 0x80, `lw a1,0x98(s1)`/`lw a2,0x9c(s1)` @0x80027B14/@0x80027B18) und FUN_80028DAC (Wurf-Physik: +0x98 Kurs, +0x9A vy, +0x9C Vortrieb, +0x9E vy-Zuwachs, +0xA0 Lebensdauer cap 0x1D, +0xA4 Vortriebs-Zuwachs) @0x80028DC0-@0x80028E50. 212 sprengt die 0x1D-Klemme, also passt FUN_80028DAC nicht.
- Bedeutung von Entity+0x1C8 (@0x801003F4 auf 0 gesetzt) ist offen — im Overlay gibt es keinen weiteren Zugriff mit der Entity als Basis (die Treffer @0x801036E4-@0x8010372C laufen ueber ein anderes Basisregister v1).
- Bedeutung des Gates [[0x800CFE20]+0x3C] != 0 (@0x80100630-0x80100648) ist offen. 0x800CFE20 ist Index 3 der Entity-Zeigerliste, die bei DAT_800CFE14 beginnt (Beleg: FUN_800177C0 `(&DAT_800cfe14)[local_30]`, FUN_8003513C `s3 = 0x800CC1E8 + 0x3C2C = 0x800CFE14` @0x80035168-@0x80035170). Was Entity+0x3C dort bedeutet, ist nicht ermittelt.
- Bedeutung der Entity-Flagbits +0x00 = 0x4 und 0x8 (@0x80100604, @0x8010056C) ist nicht belegt. Belegt sind nur Bit 0x1 (aktiv/zielbar, FUN_800410CC), Bit 0x2 (schliesst das Paar aus der Kollisionsschleife aus @0x80034D48-50) und die Bits 26..28 (Trefferzonen-Klasse, `srl v0,v0,0x1a` @0x800413CC).
- Bedeutung der beiden Overlay-BSS-Records @0x80105610 / @0x8010562C (Stride 0x1C, INIT loescht je die ersten 12 Byte) ist offen. Belegt ist nur ihr Inhalt: +0x0C/+0x10 zeigen auf laengenpraefixierte Byte-Listen (0x801055EC = {9: 03 04 7A 7B 85 96 C2 C9 CD}, 0x801055F8 = {2: DA DB}, 0x801055FC = {8: 00 01 02 3B 3C 61 BD BE}, 0x80105608 = {4: 27 35 5B 5C}), +0x14 = 0x0FF10F02, +0x18 = 0x000000F1.
- Bedeutung der Kollisions-Record-Felder rec+0x0C/+0x0E (Entity+0x90/+0x92 bzw. +0xB0/+0xB2) ist offen. Belegt gelesen werden in der Doppelschleife 0x80034D74ff nur rec+0x00/+0x04/+0x08 (Welt-XYZ), rec+0x14 (Y-Versatz), rec+0x16 (Radius) und rec+0x1A (Hoehe). Der Spieler-INIT FUN_8003BAF0 setzt rec+0x0C/+0x0E ebenfalls auf den Radius (450/450).
- Bedeutung von [Part2+0x88]+0x0C und +0x10, die INIT @0x80100754/@0x80100764 nullt, ist offen — belegt ist nur, dass FUN_8004BAF0 genau diesen Zeiger als erstes liest.
- Was 0x800CFB74 Bit 0x20 bedeutet (600 vs 400 HP), ist nicht ermittelt. Dasselbe Global mit den Bits 0x40/0x20 waehlt beim RE2-Hund die HP-Tabelle (@0x80105340/60/B0/D0) — im Port existiert kein Produzent dafuer, Default = beide Bits 0 (also 600 HP).
- Die Untertabellen der Griff-Handler sind sehr kurz und ueberlappen: 0x801056E8[0]=0x801038BC (leerer Stub), 0x801056E8[1]=0x80103908; 0x801056EC[0]=0x80103908; ab 0x801056F0 steht mit 0x1000002B kein Code-Zeiger mehr. Das heisst SPIELER+0x5 kann in diesen beiden Handlern hoechstens 0 bzw. 1 sein — nicht verifiziert.
- 0x80103908 (die eigentliche Griff-/Kollaps-Maschine des Bosses) ist in diesem Auftrag NICHT zerlegt.
- Der INIT setzt +0x1C1 = 1 (Wurzel-Part = Part 1), verankert den Schatten aber an Part 0 +0x5C (@0x80100608). Ob das ein Widerspruch oder Absicht ist, ist nicht geklaert.
- Zustand 5 und 6 der Zustandstabelle @0x801055CC sind NULL — ein Dispatch mit +0x04 == 5 oder 6 wuerde nach 0x00000000 springen. Wer diese Werte verhindert, ist nicht ermittelt.
- Entity+0x1EC (das Byte, das FUN_800410CC vor der Zielsuche gegen -1 prueft) wird von diesem INIT NICHT gesetzt — Herkunft nicht ermittelt.


---

# RE2 Endgegner EM36 — Zustand 1 (Haupt-Kampfhirn) @0x80100784, Datei build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN, Ladebasis 0x80100000. Codebereich 0x80100784..0x80101d48 (kein Zustand-1-Code jenseits 0x80101d48; 0x80101d4c..0x80101d98 = 10 leere `jr ra`-Stubs OHNE jede Xref, 0x80101d9c gehoert zu Zustand 2/3, belegt durch jal @0x80102978 und @0x80102fb0).

0x80100784 ist ein reiner Verteiler: er liest Entity+0x10E, maskiert &0x3F, indiziert die Tabelle @0x80105648 und ruft den Eintrag per jalr (a0/a1/a2 unveraendert). Im Auslieferungsfall ist (+0x10E & 0x3F) = 0 (Boss startet mit +0x10E = 0x8000; im gesamten Overlay existiert KEIN Schreibzugriff auf +0x10E, nur der Lesezugriff @0x8010078c), also landet jeder Aufruf bei 0x801007c4. 0x801007c4 ist der eigentliche Rumpf: erst der Ausstieg nach Zustand 3, wenn (+0x226 & 6) == 4; sonst ein DOPPELTER Unter-Dispatch auf Entity+0x05 — zuerst Tabelle A @0x8010564c (nur a0 = Entity, "Denken"), danach Tabelle B @0x80105660 (a0/a1/a2, "Handeln"), wobei +0x05 zwischen beiden Aufrufen NEU gelesen wird. Beide Tabellen zeigen in dieselbe 11-Wort-Tabelle @0x80105648 hinein (A = &tab[1], B = &tab[6]); es gibt genau 5 Unterzustaende 0..4 und KEINE Bereichspruefung auf +0x05. Unterzustand 0 = Patrouille/Kommandozentrale (verteilt Befehle an die vier EM37-Begleiter ueber 0x80104E9C und wuerfelt den naechsten Unterzustand aus), 1 = Sturmlauf mit Begleiter-Choreographie, 2 = 15-phasige Beschwoerungssequenz (nur ueber +0x1D4 & 1 startbar), 3 = beschleunigter Rammlauf mit direkter X-Verschiebung, 4 = terminale Frisstoetung des Spielers in Anim-Bank 7 ohne jeden Ausgang. Alles Kampfverhalten ausser dem Anim-Fortschritt haengt an Entity+0x38 (Welt-X, feste Schwellen 4001/7000/8000/9001/10000/12000) und Entity+0x1F0 (Spielerabstand, Schwellen 6000/7000/10001/11001).

## Konstanten

| Name | Wert | Bedeutung | Adresse |
|---|---|---|---|
| DISPATCH_MASK_10E | 0x3F | Feld: Maske auf Entity+0x10E fuer die Wurzeltabelle @0x80105648 | @0x80100794 |
| TAB_Z1_ROOT | 0x80105648 = {0x801007c4, 0x80100868, 0x80100d08, 0x80101108, 0x80101808, 0x80101aa0, 0x80100960, 0x80100d10, 0x80101110, 0x80101810, 0x80101aa8} | Feld: 11 gueltige Zeiger; ab Eintrag 11 stehen bereits Daten (0x00000301) | @0x801007a4 (lw v0,22088(at)) |
| TAB_Z1_A | 0x8010564C = {0x80100868, 0x80100d08, 0x80101108, 0x80101808, 0x80101aa0} | Feld: A-Tabelle (Denken), Index +0x05; Eintraege 1..4 sind leere jr-ra-Stubs | @0x80100814 |
| TAB_Z1_B | 0x80105660 = {0x80100960, 0x80100d10, 0x80101110, 0x80101810, 0x80101aa8} | Feld: B-Tabelle (Handeln), Index +0x05 | @0x8010083c |
| Z3_GATE_226 | (+0x226 & 6) == 4 | Feld: Ausstieg nach Zustand 3 | @0x801007e4 / @0x801007e8 / @0x801007ec |
| STATE_WORD_Z3 | 3 | Feld: sw auf +0x04 -> Zustand 3 / Unterzustand 0 / Phase 0 | @0x801007f4 / @0x801007fc |
| FINISH_ARC | 128 | Distanz: Halboeffnung des Facing-Tests 0x80015614 (12-Bit-Winkel, 4096 = 360 Grad) | @0x80100894 |
| FINISH_X_MIN | 9001 | Distanz: +0x38 (Welt-X) muss >= 9001 sein | @0x801008a0 |
| FINISH_DIST_MAX | 0x1770 = 6000 | Distanz: +0x1F0 muss < 6000 sein | @0x801008ac |
| FINISH_PLAYER_LATCH | [0x800CFDCB] == 0 | Feld: Spieler+0x1D3 (Greif-Riegel) muss frei sein | @0x801008c4 / @0x801008cc |
| FINISH_PLAYER_HP | [0x800CFD4E] <= 0 | Feld: Spieler-HP (+0x156); bgtz springt bei HP > 0 zum Ende | @0x801008d8 / @0x801008e0 |
| FINISH_BLOCK_226 | (+0x226 & 4) == 0 | Feld: Bit2 sperrt den Finisher | @0x801008f0 / @0x801008f4 |
| FINISH_DAMAGE | 500 | Schaden: a0 von jal 0x800401D4 (a1 = 0) | @0x801008f8 / @0x801008fc |
| STATE_WORD_S4 | 1025 = 0x401 | Feld: +0x04 = 1, +0x05 = 4, +0x06 = 0 | @0x80100900 / @0x80100908 |
| FINISH_SIDE_EFFECTS | [0x800CFDCB] |= 0x80 ; +0x218 = 4 ; +0x219 = 0 ; [0x800CFB74] |= 0x100 | Feld: Riegel, Trefferzonen-Index und globale Ereignisflagge | @0x80100918/@0x80100920 / @0x80100924/@0x80100928 / @0x8010092c / @0x8010093c/@0x80100944 |
| CLIP_S0_IDLE | 0x001F0000 | Clip: sw auf +0x14C -> +0x14C = 0, +0x14D = 0, +0x14E = 0x1F | @0x801009a4 / @0x801009a8 |
| S0_TIMER_BASE | 120 + (rng & 0x1F)  (120..151) | Dauer: +0x158 in Unterzustand 0 Phase 0 | @0x801009b8 / @0x801009c0 / @0x801009c4 |
| S0_X_GATE | 8000 | Distanz: +0x38 < 8000 ueberspringt den Begleiter-Scan | @0x801009c8 |
| S0_DIST_GATE | 0x2AF9 = 11001 | Distanz: +0x1F0 < 11001 ueberspringt den Begleiter-Scan | @0x801009dc |
| COMPANION_SCAN | 4 Durchlaeufe; Kandidat wenn ((+0x228 >> i) & 1) == 0 UND ((+0x228 >> (i+4)) & 1) == 0 | Feld: Begleiter-Verfuegbarkeitsmaske, Ergebnis als Indexliste ab sp+16 | @0x801009e8 / @0x801009fc / @0x80100a0c / @0x80100a10 / @0x80100a24 (Zweitfassungen @0x80100ae4.. und @0x80101988..) |
| S0_ALL_FREE | 4 | Feld: Trefferzahl == 4 -> Sprung 0x80100ce0 (Unterzustand 1) | @0x80100a3c / @0x80100a40 |
| S0_SHORT_TIMER | 10 | Dauer: +0x158 wenn nicht alle Begleiter frei sind (+0x16B = 1) | @0x80100a44 / @0x80100a48 / @0x80100a4c |
| S0_ROW_RNG | +0x16A = (rng1 >> (rng2 & 3)) & 7 | Feld: Zeilenindex in TAB_COMP_CMD | @0x80100a6c / @0x80100a74 / @0x80100a80 |
| S0_ROW_RESET_DIST | 0x1B58 = 7000 | Distanz: +0x1F0 < 7000 -> +0x16A = 0 (zweite Stelle @0x80100b48/@0x80100b54) | @0x80100a78 / @0x80100a84 |
| S0_SE_CHANCE | (rng & 3) != 0 | Feld: 3/4-Chance auf SE 9 am Clip-Ende | @0x80100aac / @0x80100ab0 |
| S0_CMD_PERIOD | (+0x158 & 0xF) == 0 | Dauer: Begleiter-Befehle nur alle 16 Frames | @0x80100ac8 / @0x80100acc |
| S0_CMD_LOCK | +0x16B & 1 | Feld: gesetztes Bit0 sperrt die Befehlsausgabe; gesetzt @0x80100c04/@0x80100c08 | @0x80100adc / @0x80100ae0 |
| TAB_COMP_CMD | 0x80105674, 8 Zeilen a 2 Worte: r0={0x0301,0x0301} r1={0x0D01,0x0D01} r2={0x0301,0x0301} r3={0x0D01,0x0301} r4={0x0D01,0x0301} r5={0x0D01,0x0D01} r6={0x0D01,0x0D01} r7={0x0301,0x0D01} | Feld: Begleiter-Zustandswort; Zeile = +0x16A (Unterzustand 0) bzw. +0x16B (Unterzustand 3), Spalte = ((rng>>(rng&3))&1)*4 | @0x80100b98 / @0x80101a3c (addiu a0,a0,22132); Auswahl @0x80100ba4/@0x80100bb0/@0x80100bb4 |
| S0_DECIDE_DEFAULT | 769 = 0x301 | Feld: +0x04 -> Zustand1 / Unterzustand3 / Phase0 (Vorgabe) | @0x80100c44 / @0x80100c4c |
| S0_DECIDE_X_1 | 10000 | Distanz: +0x38 < 10000 (mit (rng&3)!=0) -> 257 | @0x80100c64 |
| S0_DECIDE_DIST | 0x2711 = 10001 | Distanz: +0x1F0 >= 10001 -> 257 | @0x80100c7c |
| S0_DECIDE_X_2 | 9001 | Distanz: +0x38 >= 9001 (mit (rng&1)==0) -> 769 | @0x80100c94 |
| S0_DECIDE_X_3 | 7000 | Distanz: +0x38 < 7000 (mit (rng&1)==0) -> 257 | @0x80100cc0 |
| STATE_WORD_S1 | 257 = 0x101 | Feld: +0x04 = 1, +0x05 = 1, +0x06 = 0 | @0x80100c6c / @0x80100c84 / @0x80100ce0 / @0x801017c8 |
| TAB_PERM_4x4 | 0x80100004, Rohbytes 00 02 01 03 | 01 02 03 00 | 03 00 02 01 | 03 01 02 00 | Feld: Begleiter-Permutation, wird nach sp+16 kopiert; Zeile = +0x16B (0..3) | @0x80100d34 (addiu a1,a1,4), Kopie @0x80100d38..@0x80100d78 |
| S1_FLAG_226 | |= 0x2 | Feld: +0x226 Bit1 markiert Unterzustand 1 (blockiert das Zustand-3-Tor); Gegenstueck @0x8010183c/@0x80101844 in Unterzustand 3 | @0x80100d84 / @0x80100d88 |
| CLIP_S1_RUN | 0x001F0005 | Clip: +0x14C = 5 | @0x80100dd0 / @0x80100dd8 |
| S1_CMD_ALL | 2817 = 0x0B01 | Feld: alle vier Begleiter +0x04 = Zustand1/Unterzustand11/Phase0 | @0x80100dd4 (a0 fuer 0x80104e5c @0x80100de0) |
| S1_ROW_RNG | +0x16B = (rng1 >> (rng2 & 3)) & 3 | Feld: Zeile in TAB_PERM_4x4 | @0x80100e0c / @0x80100e10 / @0x80100e14 |
| S1_P1_FRAMES | 0, 7, 10, 15 -> Spalte 0/1/2/3 mit 0x0B01 | Frame: +0x14D-Marken in Unterzustand 1 Phase 1 | @0x80100e20 / @0x80100e4c / @0x80100e78 / @0x80100ea4 |
| S1_P1_FRAME_55 | 55 -> Spalte 0 und 2 mit 0x00070B01, danach SE 9 | Frame | @0x80100ed0 (Wert @0x80100ed8 / @0x80100ef4) |
| S1_P1_FRAME_61 | 61 -> Spalte 1 und 3 mit 0x00070B01 | Frame | @0x80100f28 (Wert @0x80100f30 / @0x80100f4c) |
| S1_P1_FRAME_90 | 90 -> SE 11 | Frame | @0x80100f74 / @0x80100f7c |
| S1_SPEED_MUL | +0x144 += ((s16)+0x144 >> 1)   (Faktor 1.5 pro Frame) | Feld: Vorwaertsgeschwindigkeit | @0x80100fb0 / @0x80100fb4 / @0x80100fb8 / @0x80100fc0 |
| S1_MOVE_X_LIMIT | 12000 | Distanz: ab +0x38 >= 12000 keine Vorwaertsbewegung mehr | @0x80100fd0 / @0x80100fd4 |
| CLIP_S1_BRAKE | 0x001F0002 | Clip: +0x14C = 2 (Unterzustand 1 Phase 2) | @0x80100ff0 / @0x80100ff4 / @0x80101000 |
| S1_P3_FRAMES | 85 -> zurueck nach Unterzustand 0 (+0x226 &= ~2); 25 -> Spalte 0 und 2 mit 0x00090B01 + SE 9; 15 -> Spalte 1 und 3 mit 0x00090B01 | Frame: +0x14D-Marken in Unterzustand 1 Phase 3 | @0x80101028 / @0x8010104c / @0x801010a4 (Wert 0x00090B01 @0x80101058 / @0x8010107c / @0x801010ac / @0x801010d0) |
| TAB_S2_PHASE | 0x80100014 = {0x80101164, 0x80101214, 0x801012ec, 0x8010136c, 0x80101390, 0x8010141c, 0x80101530, 0x801015b8, 0x8010173c, 0x8010160c, 0x8010162c, 0x80101700, 0x8010173c, 0x80101764, 0x80101798} | Feld: 15 Phasenziele fuer Unterzustand 2, erreicht per jr (kein jalr); Phasen 8 und 12 teilen sich 0x8010173c | @0x80101154 / @0x8010115c, Schranke sltiu 0xf @0x8010113c |
| COMPANION_PTRS | 0x800CFE20, 0x800CFE24, 0x800CFE28, 0x800CFE2C (Boss-Zeiger selbst @0x800CFE1C) | Feld: die vier EM37-Entity-Zeiger; 0x800CFE1C belegt in CDEMD0_EM37_ai1.BIN @0x80101464 und @0x801017fc | @0x80101138 (s3 = 0x800CC1E8) + 15416/15420/15424/15428; identisch @0x80104e60..@0x80104e90 |
| CLIP_S2_P0 | 0x001F0001 | Clip: +0x14C = 1 | @0x80101164 / @0x8010116c |
| S2_GATE_1D4 | +0x1D4 & 1 | Feld: Startfreigabe der Beschwoerungssequenz | @0x801011b0 / @0x801011b8 / @0x801011bc |
| S2_SPAWN_POS | +0x38 = -9000, +0x40 = -23400 | Distanz: Zwangsposition beim Eintritt in Phase 1 | @0x801011d0 / @0x801011d4 / @0x801011d8 / @0x801011dc |
| S2_P0_CLEARS | +0x1C0 &= ~2 ; +0x00 &= ~2 ; +0x158 = 0 ; +0x218 = 3 ; +0x219 = 0 | Feld: Aufraeumen beim Sequenzstart | @0x80101178/@0x8010117c / @0x801011e4/@0x801011ec/@0x801011f0 / @0x801011e8 / @0x80101208 / @0x80101210 |
| S2_Y_ANCHOR | 0x80105BB0 | Feld: Overlay-Global, sichert [+0x198]+0x30 als Y-Referenz | @0x80101200 (sh), gelesen @0x80101334 und @0x80101578 |
| S2_Y_SCALE_A | (4415 * d) >> 11  (logische Schiebung) | Feld: [[+0x198]+0x1E0]+0x10 in Phase 2; 4415 = ((((d<<4)+d)<<2)+d)<<6 - d | @0x80101344..@0x80101360 / @0x80101368 |
| S2_Y_SCALE_B | (8830 * d) >> 12  (arithmetisch, Kurzschluss bei d == 0) | Feld: [[+0x198]+0x1E0]+0x10 in Phase 6 | @0x80101588..@0x801015a8 / @0x801015b4 |
| S2_P1_TIMERS | 10, 30, 40, 90 | Dauer: +0x158-Marken (Begleiter0 / Begleiter2 / Begleiter3+SE9 / Ende Phase 1) | @0x8010122c / @0x80101250 / @0x80101278 / @0x801012a4 |
| S2_P4_TIMERS | 30 (alle Begleiter +0x06 = 8, SE 0), 90 (-> Phase 5) | Dauer | @0x801013ac / @0x801013b0 / @0x80101400 |
| S2_P5_TIMERS | 20, 40, 45, 56 (Begleiter 0/1/2/3 -> 2305 = 0x0901), 110 (-> Phase 6) | Dauer | @0x80101438 / @0x80101460 / @0x80101484 / @0x801014ac / @0x801014d0; Wert @0x8010144c |
| CLIP_S2_P6 | 0x001F0004, alle Begleiter +0x06 = 7, SE 11 | Clip: +0x14C = 4 | @0x801014e4 / @0x801014e8 / @0x801014f8 / @0x80101500 |
| S2_P10_TIMERS | 20, 35, 40, 50 (Begleiter 0/2/1/3 -> 2561 = 0x0A01), 110 (-> Phase 11) | Dauer | @0x80101648 / @0x80101670 / @0x80101694 / @0x801016bc / @0x801016e0; Wert @0x8010165c |
| S2_P11_CMD | 0x00040A01, nur Begleiter 0 und 2 | Feld: Begleiter -> Unterzustand 10, Phase 4 | @0x80101708 / @0x8010170c / @0x80101724 / @0x80101730 |
| S2_P13 | Timer 60 ; +0x218 = 0 ; +0x219 = 0 ; alle Begleiter +0x04 = 1537 = 0x0601 | Dauer/Feld: Abschluss der Sequenz (Ablauf ueber 61 Frames) | @0x8010177c / @0x8010178c / @0x80101780 / @0x80101784 / @0x80101794 |
| CLIP_S3 | 0x001F0002 | Clip: +0x14C = 2 in Unterzustand 3 Phase 0 | @0x8010185c / @0x80101860 / @0x8010186c |
| S3_ROW_RNG | +0x16B = (rng1 >> (rng2 & 3)) & 7 | Feld: Zeile in TAB_COMP_CMD | @0x8010189c / @0x801018a0 / @0x801018a4 |
| S3_ACCEL_TIME | 50 | Dauer: +0x158 hoch bis 50, dann +0x16A |= 1 | @0x801018b4 / @0x801018d0 / @0x801018e0 |
| S3_MOVE_X_MIN | 4001 | Distanz: unterhalb +0x38 < 4001 keine X-Verschiebung mehr | @0x80101934 |
| S3_CMD_PERIOD | (+0x15A & 0x1F) == 0 | Dauer: Begleiter-Befehle alle 32 Frames | @0x80101980 / @0x80101984 |
| S3_ROW_RESET_DIST | 0x1B58 = 7000 | Distanz: +0x1F0 < 7000 -> +0x16B = 0 | @0x801019ec / @0x801019f8 |
| CLIP_S4_GRAB | 0x00070009 | Clip: +0x14C = 9, +0x14E = 7 (andere Bank als 0x1F) | @0x80101aec / @0x80101b14 / @0x80101b30 |
| S4_PLAYER_BASE | 0x800CFBF8 | Feld: Spieler-Entity; belegt ueber 0x800401D4 (@0x800401e0 a2 = 0x800CFBF8, @0x80040248 lhu 342(a2) = 0x800CFD4E, @0x8004025c sh) | @0x80101b1c (s0 = 0x800CFDAC) / @0x80101b20 (a0 = s0 - 436) |
| S4_PLAYER_WRITES | +0x04 = 6 ; +0x00 |= 0x1002 ; +0x1D3 |= 0x80 ; +0x15A = Boss+0x76 + 2048 ; +0x188/+0x18C = Boss+0x188/+0x18C ; [0x800CFDAC] = Boss | Feld: Uebernahme des Spielers | @0x80101b50 / @0x80101ba0 / @0x80101b70 / @0x80101bac+@0x80101bb4 / @0x80101b60+@0x80101b78 / @0x80101b40 |
| S4_BOSS_WRITES | Boss+0x00 |= 0x1000 ; Boss+0x1D3 |= 0x80 | Feld: Greif-Riegel am Boss | @0x80101b88 / @0x80101b90 / @0x80101b84 / @0x80101b8c |
| S4_BLOOD_FRAMES | Frame 10 -> SE 7 ; Frames 16..29 -> Effekt | Frame: Test (+0x14D - 16) < 14 | @0x80101bc4 / @0x80101be0 / @0x80101be4 |
| S4_BLOOD_VEC | 256 - 2 * (rng & 0xFF) je Achse (sp+16 / sp+18 / sp+20) | Feld: Streuvektor fuer 0x8001bf10 | @0x80101bf8 / @0x80101c00 / @0x80101c04 / @0x80101c0c / @0x80101c20 / @0x80101c34 |
| S4_BLOOD_ID | ((rng & 1) << 16) | (((rng & 0xFF) << 3) + 6096)   (6096 = 0x17D0) | Feld: a0 von 0x8001bf10 | @0x80101c40 / @0x80101c44 / @0x80101c4c / @0x80101c50 / @0x80101c54 |
| S4_ANIM_FLAGS | 512 | Feld: a3 von 0x8002959c in Unterzustand 4 (ueberall sonst 128) | @0x80101c8c / @0x80101cd8 |
| S4_HOLD_TIMER | 150 | Dauer: +0x15A in Phase 3 | @0x80101cc0 / @0x80101cc4 |
| S4_RELEASE_Y | 6000 | Distanz: [[+0x198]+0x1E0]+0x0C < 6000 -> +0x218 = 0, +0x219 = 0 | @0x80101d0c / @0x80101d14 / @0x80101d20 / @0x80101d24 |
| SE_IDS | 0, 6, 7, 9, 10, 11 | Feld: alle in Zustand 1 verwendeten a0-Werte von jal 0x8005BD6C (29 Aufrufstellen) | @0x801013d0 (0) / @0x80101b98 (6) / @0x80101bcc (7) / @0x80100ab4 (9) / @0x80100de8 (10) / @0x80100f7c (11) |

## Uebergaenge

- **z1 (Verteiler 0x80100784) -> A[+0x05] dann B[+0x05]** wenn immer, wenn (+0x226 & 6) != 4; kein Range-Check auf +0x05
  - @0x801007e0 lhu v0,550(s0) / @0x801007e8 andi v0,v0,0x6 / @0x801007ec bne v0,v1(=4),0x80100800 / @0x80100800 lbu v0,5(s0) / @0x80100814 lw v0,22092(at)=0x8010564c / @0x8010081c jalr v0 (a0=s0) / @0x80100828 lbu v0,5(s0) NEU / @0x8010083c lw v0,22112(at)=0x80105660 / @0x80100844 jalr v0
- **z1 beliebig -> z3 s0 p0** wenn (+0x226 & 6) == 4  (Bit2 gesetzt UND Bit1 frei; Bit1 halten die Unterzustaende 1 und 3, Bit2 setzt Zustand 3 @0x80103010)
  - @0x801007e4 addiu v1,zero,4 / @0x801007ec bne v0,v1,0x80100800 / @0x801007f4 addiu v0,zero,3 / @0x801007fc sw v0,4(s0)
- **z1 s0 (A-Teil 0x80100868) -> z1 s4 p0** wenn 0x80015614(a3=128) == 0 UND +0x38 >= 9001 UND +0x1F0 < 6000 UND Spieler+0x1D3 (0x800CFDCB) == 0 UND Spieler-HP (0x800CFD4E) <= 0 UND (+0x226 & 4) == 0
  - @0x80100890 jal 0x80015614 / @0x801008a0 slti v1,v1,9001 / @0x801008a4 bne v1,zero,ende / @0x801008ac sltiu v0,s1,0x1770 / @0x801008b0 beq v0,zero,ende / @0x801008b8 bne v0,zero,ende / @0x801008cc bne v0,zero,ende / @0x801008e0 bgtz v0,ende / @0x801008f4 bne v0,zero,ende / @0x80100900 addiu v0,zero,1025 / @0x80100908 sw v0,4(s0)
- **z1 s0 p0 (B 0x801009a4) -> z1 s0 p1** wenn immer im ersten Frame; Clip 0, +0x158 = 120 + (rng & 0x1F)
  - @0x801009a8 sw v0,332(s2) / @0x801009b0 jal 0x80015fe8 / @0x801009b4 sb v0(=1),6(s2) / @0x801009b8 andi v0,v0,0x1f / @0x801009c0 addiu v0,v0,120 / @0x801009c4 sh v0,344(s2)
- **z1 s0 p0 -> z1 s1 p0** wenn +0x38 >= 8000 UND +0x1F0 >= 11001 UND alle 4 Begleiter frei (Scan ueber +0x228: Bit i und Bit i+4 beide 0, Trefferzahl == 4)
  - @0x801009c8 slti v1,v1,8000 / @0x801009cc bne v1,zero,0x801009e8 / @0x801009dc sltiu v0,v0,0x2af9 / @0x801009e0 bne v0,zero,0x80100a54 / @0x801009f8..@0x80100a34 Scan / @0x80100a3c addiu v0,zero,4 / @0x80100a40 beq v1,v0,0x80100ce0 / @0x80100ce0 addiu v0,zero,257 / @0x80100ce4 sw v0,4(s2)
- **z1 s0 p0 -> z1 s0 p1 (Kurzschleife)** wenn Scan liefert nicht 4 -> +0x16B = 1 und +0x158 = 10
  - @0x80100a44 addiu v0,zero,1 / @0x80100a48 sb v0,363(s2) / @0x80100a4c addiu v0,zero,10 / @0x80100a50 sh v0,344(s2)
- **z1 s0 p1 -> z1 s3 p0 (Vorgabe)** wenn +0x158 hat 0 erreicht UND kein Begleiter mit (Bit i frei UND Bit i+4 GESETZT); setzt zuerst unbedingt 769
  - @0x80100bcc lh v0,344(s2) / @0x80100be8 bne v0,zero,ende / @0x80100c08 sb v0,363(s2) (+0x16B|=1) / @0x80100c24 beq v0,zero,skip / @0x80100c40 bne v0,zero,ende / @0x80100c44 addiu v0,zero,769 / @0x80100c4c sw v0,4(s2)
- **z1 s0 p1 (Entscheidung 2) -> z1 s1 p0** wenn (rng & 3) != 0 UND +0x38 < 10000
  - @0x80100c48 jal 0x80015fe8 / @0x80100c50 andi v0,v0,0x3 / @0x80100c54 beq v0,zero,0x80100c74 / @0x80100c64 slti v0,v0,10000 / @0x80100c68 beq v0,zero,0x80100c74 / @0x80100c6c addiu v0,zero,257 / @0x80100c70 sw v0,4(s2)
- **z1 s0 p1 (Entscheidung 3) -> z1 s1 p0** wenn +0x1F0 >= 10001
  - @0x80100c74 lw v0,496(s2) / @0x80100c7c sltiu v0,v0,0x2711 / @0x80100c80 bne v0,zero,0x80100c8c / @0x80100c84 addiu v0,zero,257 / @0x80100c88 sw v0,4(s2)
- **z1 s0 p1 (Entscheidung 4) -> z1 s3 p0** wenn +0x38 >= 9001 UND (rng & 1) == 0
  - @0x80100c94 slti v0,v0,9001 / @0x80100c98 bne v0,zero,0x80100cb8 / @0x80100ca0 jal 0x80015fe8 / @0x80100ca8 andi v0,v0,0x1 / @0x80100cac bne v0,zero,0x80100cb8 / @0x80100cb0 addiu v0,zero,769 / @0x80100cb4 sw v0,4(s2)
- **z1 s0 p1 (Entscheidung 5, letzte Ueberschreibung) -> z1 s1 p0** wenn +0x38 < 7000 UND (rng & 1) == 0
  - @0x80100cc0 slti v0,v0,7000 / @0x80100cc4 beq v0,zero,ende / @0x80100ccc jal 0x80015fe8 / @0x80100cd4 andi v0,v0,0x1 / @0x80100cd8 bne v0,zero,ende / @0x80100ce0 addiu v0,zero,257 / @0x80100ce4 sw v0,4(s2)
- **z1 s1 p0 (B 0x80100dd0) -> z1 s1 p1** wenn immer; Clip 5, alle 4 Begleiter -> 0x0B01, SE 10, +0x16B = (rng >> (rng & 3)) & 3
  - @0x80100dd0 ori v0,v0,0x5 / @0x80100dd8 sw v0,332(s1) / @0x80100de0 jal 0x80104e5c (a0=2817) / @0x80100de4 sb v0(=1),6(s1) / @0x80100dec jal 0x8005bd6c (a0=10) / @0x80100e0c srav s0,s0,v0 / @0x80100e10 andi s0,s0,0x3 / @0x80100e14 sb s0,363(s1)
- **z1 s1 p1 -> z1 s1 p2** wenn +0x06 += Rueckgabe 0x8002959c. Der sb liegt im Delay-Slot und laeuft IMMER; nur die Vorwaertsbewegung entfaellt bei +0x38 >= 12000
  - @0x80100fbc jal 0x8002959c / @0x80100fc4 lbu v1,6(s1) / @0x80100fcc addu v1,v1,v0 / @0x80100fd0 slti a0,a0,12000 / @0x80100fd4 beq a0,zero,0x801010e8 / @0x80100fd8 sb v1,6(s1) / @0x80100fe0 jal 0x800152c8 (a1=0)
- **z1 s1 p2 (0x80100ff0) -> z1 s1 p3** wenn immer, im selben Frame durchfallend; Clip 2, SE 10
  - @0x80100ff0 lui v0,0x1f / @0x80100ff4 ori v0,v0,0x2 / @0x80101000 sw v0,332(s1) / @0x80101008 jal 0x8005bd6c (a0=10) / @0x8010100c sb v0(=3),6(s1)
- **z1 s1 p3 -> z1 s0 p0** wenn Anim-Frame +0x14D == 85; loescht +0x226 Bit1
  - @0x80101024 lbu v1,333(s1) / @0x80101028 addiu v0,zero,85 / @0x8010102c bne v1,v0,0x80101050 / @0x80101038 addiu v1,zero,1 / @0x8010103c sw v1,4(s1) / @0x80101040 andi v0,v0,0xfffd / @0x80101044 sh v0,550(s1)
- **z1 s2 p0 (B 0x80101164) -> z1 s2 p1** wenn (+0x1D4 & 1) != 0. Ohne das Bit bleibt p0 stehen (Clip 1, +0x1C0 &= ~2, vier Halbworte in [[+0x198]+0x1E0]+0x0C/0x10/0x14/0x18 = 0)
  - @0x801011b0 lhu v0,468(s0) / @0x801011b8 andi v0,v0,0x1 / @0x801011bc beq v0,zero,0x801017e8 / @0x801011cc sb v0(=1),6(s0) / @0x801011d4 sw v0(=-9000),56(s0) / @0x801011dc sw v0(=-23400),64(s0) / @0x801011ec and v0,v0,v1(=-3) / @0x80101200 sh v0,23472(at)=0x80105bb0 / @0x80101208 sb v0(=3),536(a1)
- **z1 s2 p1 -> z1 s2 p2** wenn +0x158 hoch: 10 -> Begleiter0.+0x06=1; 30 -> Begleiter2.+0x06=1; 40 -> Begleiter3.+0x06=1 + SE 9; 90 -> p2, alle vier Begleiter +0x06 = 6, SE 11 + SE 10
  - @0x8010121c addiu v0,v0,1 / @0x8010122c bne v0,v1(=10),0x80101248 / @0x80101240 sb v0,6(a2=[0x800CFE20]) / @0x80101250 bne v1,v0(=30) -> [0x800CFE28] / @0x80101278 bne v1,v0(=40) -> [0x800CFE2C] / @0x801012a4 bne v1,v0(=90),0x801017e8 / @0x801012ac sb v0(=2),6(s0) / @0x801012b8 sb v0(=6),6(a2)
- **z1 s2 p2 (0x801012ec) -> z1 s2 p3** wenn +0x06 += Rueckgabe 0x8002959c; parallel [[+0x198]+0x1E0]+0x10 = (4415 * ([+0x198]+0x30 - [0x80105bb0])) >> 11
  - @0x801012f8 jal 0x80015e7c (a3=0) / @0x8010130c jal 0x8002959c (a3=128) / @0x80101320 addu v1,v1,v0 / @0x80101324 jal 0x800152c8 / @0x80101328 sb v1,6(s0) / @0x80101344..@0x8010135c 4415er-Kette / @0x80101360 srl a0,v0,11 / @0x80101368 sh a0,16(v1)
- **z1 s2 p3 (0x8010136c) -> z1 s2 p4** wenn immer, durchfallend; Clip 3, Timer 0, SE 9
  - @0x8010136c lui v1,0x1f / @0x80101370 ori v1,v1,0x3 / @0x80101380 sh zero,344(s0) / @0x80101384 sb v0(=4),6(s0) / @0x80101388 jal 0x8005bd6c (a0=9) / @0x8010138c sw v1,332(s0)
- **z1 s2 p4 -> z1 s2 p5** wenn Timer 30 -> alle vier Begleiter +0x06 = 8 und SE 0; Timer 90 -> p5, Timer 0, SE 10
  - @0x801013ac bne v1,v0(=30),0x801013e8 / @0x801013b0 addiu v0,zero,8 / @0x801013bc sb v0,6(a2) / @0x801013e0 jal 0x8005bd6c (a0=0) / @0x801013ec addiu v1,zero,90 / @0x80101400 bne v0,v1,0x801017e8 / @0x80101410 sb v0(=5),6(a1) / @0x80101418 sh zero,344(a1)
- **z1 s2 p5 -> z1 s2 p6** wenn Timer 20/40/45/56 -> Begleiter 0/1/2/3 +0x04 = 2305 (0x0901) + SE 0; Timer 110 -> p6, Clip 4, Timer 0, alle Begleiter +0x06 = 7, SE 11
  - @0x80101438 bne v1,v0(=20) / @0x80101454 sw v0(=2305),4(a2) / @0x80101460 bne v1,v0(=40) / @0x80101484 bne v1,v0(=45) / @0x801014ac bne v1,v0(=56) / @0x801014cc addiu v1,zero,110 / @0x801014e0 bne v0,v1,0x801017e8 / @0x801014e8 ori v0,v0,0x4 / @0x801014f0 sb v1(=6),6(s0) / @0x80101504 sb v0(=7),6(a2)
- **z1 s2 p6 (0x80101530) -> z1 s2 p7** wenn +0x06 += Rueckgabe 0x8002959c; Y-Skalierung hier (8830 * d) >> 12 arithmetisch, mit Kurzschluss bei d == 0
  - @0x8010153c jal 0x80015e7c / @0x80101550 jal 0x8002959c / @0x80101568 jal 0x800152c8 / @0x8010156c sb v1,6(s0) / @0x80101588 beq a0,zero,0x801015ac / @0x801015a4 sll v0,v0,1 / @0x801015a8 sra a0,v0,12 / @0x801015b4 sh a0,16(v0)
- **z1 s2 p7 (0x801015b8) -> z1 s2 p8** wenn immer; [[+0x198]+0x1E0]+0x10 = 0, Clip 2, alle vier Begleiter +0x06 = 9, SE 10
  - @0x801015c8 sh zero,16(v0) / @0x801015d0 sw v1(=0x001F0002),332(s0) / @0x801015d4 sb v0(=8),6(s0) / @0x801015e0 sb v0(=9),6(a2) / @0x80101604 j 0x80101734 / @0x80101734 jal 0x8005bd6c (a0=10)
- **z1 s2 p8 (0x8010173c) -> z1 s2 p9** wenn +0x06 += Rueckgabe 0x8002959c (derselbe Codeblock bedient p8 und p12)
  - @0x80101748 jal 0x8002959c (a3=128) / @0x80101750 lbu v1,6(s0) / @0x80101758 addu v1,v1,v0 / @0x80101760 sb v1,6(s0)
- **z1 s2 p9 (0x8010160c) -> z1 s2 p10** wenn immer; Clip 0, Timer 0, SE 9
  - @0x80101614 lui v0,0x1f / @0x80101618 sw v0,332(s0) / @0x80101620 sb v0(=10),6(s0) / @0x80101624 jal 0x8005bd6c (a0=9) / @0x80101628 sh zero,344(s0)
- **z1 s2 p10 -> z1 s2 p11** wenn Timer 20/35/40/50 -> Begleiter 0/2/1/3 +0x04 = 2561 (0x0A01) + SE 0; Timer 110 -> p11
  - @0x80101648 bne v1,v0(=20) -> [0x800CFE20] / @0x80101670 bne v1,v0(=35) -> [0x800CFE28] / @0x80101694 bne v1,v0(=40) -> [0x800CFE24] / @0x801016bc bne v1,v0(=50) -> [0x800CFE2C] / @0x8010165c addiu v0,zero,2561 / @0x801016dc addiu v1,zero,110 / @0x801016f0 bne v0,v1,0x801017e8 / @0x801016fc sb v0(=11),6(s0)
- **z1 s2 p11 (0x80101700) -> z1 s2 p12** wenn immer; Clip 2, NUR Begleiter 0 und 2 bekommen +0x04 = 0x00040A01, SE 10
  - @0x80101700 lui v0,0x1f / @0x80101708 lui v1,0x4 / @0x8010170c ori v1,v1,0xa01 / @0x80101718 sb v0(=12),6(s0) / @0x80101724 sw v1,4(a2=[0x800CFE20]) / @0x80101730 sw v1,4(a2=[0x800CFE28]) / @0x80101734 jal 0x8005bd6c
- **z1 s2 p12 (0x8010173c) -> z1 s2 p13** wenn +0x06 += Rueckgabe 0x8002959c
  - @0x80101748 jal 0x8002959c / @0x80101760 sb v1,6(s0)
- **z1 s2 p13 (0x80101764) -> z1 s2 p14** wenn immer; Clip 0, +0x218 = 0, +0x219 = 0, Timer 60, SE 9, alle vier Begleiter +0x04 = 1537 (0x0601)
  - @0x8010176c lui v0,0x1f / @0x80101770 sw v0,332(s0) / @0x80101778 sb v0(=14),6(s0) / @0x80101780 sb zero,536(s0) / @0x80101784 sb zero,537(s0) / @0x8010178c sh v0(=60),344(s0) / @0x80101790 jal 0x80104e5c / @0x80101794 addiu a0,zero,1537
- **z1 s2 p14 -> z1 s1 p0** wenn +0x158 laeuft von 60 abwaerts; im Frame mit +0x158 == 0 (also 61 Frames) -> +0x04 = 257, +0x1C0 = 0, SE 10 + SE 9
  - @0x801017ac lhu v1,344(s0) / @0x801017b4 addiu v0,v1,-1 / @0x801017b8 bne v1,zero,0x801017e8 / @0x801017bc sh v0,344(s0) / @0x801017c8 addiu v0,zero,257 / @0x801017cc sw v0,4(s0) / @0x801017d4 sb zero,448(s0)
- **z1 s3 p0 (B 0x8010185c) -> z1 s3 p1** wenn immer; +0x226 |= 2, Clip 2, +0x158 = 0, +0x15A = 0, +0x16A = 0, +0x16B = (rng >> (rng & 3)) & 7, SE 11
  - @0x8010183c ori v0,v0,0x2 / @0x80101844 sh v0,550(s2) / @0x80101860 ori v0,v0,0x2 / @0x8010186c sw v0,332(s2) / @0x80101874 sb v0(=1),6(s2) / @0x80101878 sh zero,344(s2) / @0x80101880 sb zero,362(s2) / @0x80101888 sh zero,346(s2) / @0x8010189c srav s0,s0,v0 / @0x801018a0 andi s0,s0,0x7 / @0x801018a4 sb s0,363(s2)
- **z1 s3 p1, +0x16A = 0 (Beschleunigen) -> z1 s3 p1, +0x16A = 1 (Bremsen)** wenn +0x158 zaehlt hoch bis 50; dann +0x16A |= 1 und SE 11
  - @0x801018b0 bne v0,zero,0x801018ec / @0x801018b4 addiu v1,zero,50 / @0x801018c0 addiu v0,v0,1 / @0x801018d0 bne v0,v1,0x801018ec / @0x801018e0 ori v0,v0,0x1 / @0x801018e8 sb v0,362(s2)
- **z1 s3 p1, +0x16A = 1 -> z1 s3 p1, +0x16A = 2 (Ausrollen)** wenn +0x158 zaehlt abwaerts bis 0; dann +0x16A = 2 und SE 10
  - @0x801018f4 bne v1,v0(=1),0x80101928 / @0x80101904 addiu v0,v0,-1 / @0x80101910 bne v0,zero,0x80101928 / @0x80101914 addiu v0,zero,2 / @0x80101918 sb v0,362(s2)
- **z1 s3 p1 (jeden Frame) -> — (Positions-Update)** wenn +0x144 = +0x158; wenn +0x38 >= 4001 dann +0x38 -= +0x158
  - @0x80101928 lhu v0,344(s2) / @0x80101930 sh v0,324(s2) / @0x80101934 slti v0,v1,4001 / @0x80101938 bne v0,zero,0x80101950 / @0x80101948 subu v0,v1,v0 / @0x8010194c sw v0,56(s2)
- **z1 s3 p1 -> z1 s0 p0** wenn Rueckgabe 0x8002959c != 0 (Clip zu Ende); loescht +0x226 Bit1
  - @0x80101958 jal 0x8002959c (a3=128) / @0x80101960 beq v0,zero,0x80101978 / @0x80101964 addiu v1,zero,1 / @0x8010196c sw v1,4(s2) / @0x80101970 andi v0,v0,0xfffd / @0x80101974 sh v0,550(s2)
- **z1 s4 p0 (B 0x80101b14) -> z1 s4 p1** wenn immer; Bank 7 Clip 9; Spieler wird uebernommen: Spieler+0x04 = 6, Spieler+0x00 |= 0x1002, Spieler+0x1D3 |= 0x80, Spieler+0x15A = Boss+0x76 + 2048, Spieler+0x188/+0x18C = Boss+0x188/+0x18C, [0x800CFDAC] = Boss; Boss+0x00 |= 0x1000, Boss+0x1D3 |= 0x80, SE 6
  - @0x80101b14 ori v0,v0,0x9 (=0x00070009) / @0x80101b30 sw v0,332(s2) / @0x80101b38 sb v0(=1),6(s2) / @0x80101b3c jal 0x80015b94 (a0=0x800CFBF8) / @0x80101b40 sw s2,0(s0)=0x800CFDAC / @0x80101b50 sw v0(=6),-1028(at)=0x800CFBFC / @0x80101b70 sb v0,-565(at)=0x800CFDCB / @0x80101b8c sb v0,467(s2) / @0x80101b90 sw v1,0(s2) / @0x80101ba0 sw v0,-436(s0)=0x800CFBF8 / @0x80101bac addiu v0,v0,2048 / @0x80101bb4 sh v0,-686(at)=0x800CFD52
- **z1 s4 p1 -> z1 s4 p2** wenn Rueckgabe 0x8002959c != 0 (a3 = 512). Frame 10 -> SE 7; Frames 16..29 -> pro Frame ein Blut-/Partikeleffekt ueber 0x8001bf10
  - @0x80101bc8 bne v1,v0(=10),0x80101bd8 / @0x80101be0 addiu v0,v0,-16 / @0x80101be4 sltiu v0,v0,0xe / @0x80101be8 beq v0,zero,0x80101c6c / @0x80101c60 jal 0x8001bf10 / @0x80101c88 jal 0x8002959c / @0x80101c8c addiu a3,zero,512 / @0x80101c90 beq v0,zero,0x80101d28 / @0x80101c9c sb v0(=2),6(s2)
- **z1 s4 p2 (0x80101ca0) -> z1 s4 p3** wenn immer; Bank 7 Clip 2, +0x15A = 150, SE 10
  - @0x80101ca0 lui v0,0x7 / @0x80101ca4 ori v0,v0,0x2 / @0x80101cb0 sw v0,332(s2) / @0x80101cb8 jal 0x8005bd6c (a0=10) / @0x80101cbc sb v0(=3),6(s2) / @0x80101cc0 addiu v0,zero,150 / @0x80101cc4 sh v0,346(s2)
- **z1 s4 p3 -> — (ENDZUSTAND, kein Ausgang)** wenn +0x15A zaehlt von 150 auf 0; bei 0 UND [[+0x198]+0x1E0]+0x0C < 6000 werden +0x218 und +0x219 auf 0 gesetzt. In diesem Unterzustand existiert KEIN Schreibzugriff auf +0x04/+0x05
  - @0x80101cd4 jal 0x8002959c (a3=512) / @0x80101ce4 beq v0,zero,0x80101cf8 / @0x80101cf0 sh v0,346(s2) / @0x80101cfc bne v0,zero,0x80101d28 / @0x80101d0c lh v0,12(v0) / @0x80101d14 slti v0,v0,6000 / @0x80101d18 beq v0,zero,0x80101d28 / @0x80101d20 sb zero,536(s2) / @0x80101d24 sb zero,537(s2)

## Aufrufe

- `@0x801007ac` -> `jalr v0 = [0x80105648 + (+0x10E & 0x3F)*4]` (a0/a1/a2 unveraendert durchgereicht (Entity, EDD-Zeiger, EMR-Zeiger); v0 @0x801007a4, Index @0x8010078c/@0x80100794) — Wurzeldispatch Zustand 1; im Normalfall Index 0 = 0x801007c4
- `@0x8010081c` -> `jalr v0 = A[+0x05] aus 0x8010564c` (a0 = s0 = Entity (@0x80100820, Delay-Slot); v0 @0x80100814; +0x05 @0x80100800) — Denk-/Uebergangsteil des Unterzustands
- `@0x80100844` -> `jalr v0 = B[+0x05] aus 0x80105660` (a0 = s0 (@0x80100824), a1 = s1 (@0x8010082c), a2 = s2 (@0x80100848); +0x05 NEU gelesen @0x80100828) — Ausfuehrungsteil des Unterzustands
- `@0x80100890` -> `0x80015614 (Facing/Turn-Test)` (a0 = Entity (Parameter), a1 = [0x800CFC30] Spieler-X (@0x80100878), a2 = [0x800CFC38] Spieler-Z (@0x80100880), a3 = 128 (@0x80100894)) — Rueckgabe 0 = Blick innerhalb +-a3 (Bedingung fuer den Finisher), sonst +a3/-a3 als Drehrichtung (@0x8001566c..@0x80015680)
- `@0x80100904` -> `0x800401D4 (Spielerschaden)` (a0 = 500 (@0x801008f8), a1 = 0 (@0x801008fc)) — Toetungsschlag unmittelbar vor dem Wechsel in Unterzustand 4
- `@0x801009b0, @0x80100a54, @0x80100a5c, @0x80100aa4, @0x80100b58, @0x80100b84, @0x80100b8c, @0x80100c48, @0x80100ca0, @0x80100ccc, @0x80100df4, @0x80100dfc, @0x80101884, @0x8010188c, @0x801019fc, @0x80101a28, @0x80101a30, @0x80101bf4, @0x80101c08, @0x80101c1c, @0x80101c30, @0x80101c38` -> `0x80015FE8 (RNG)` (keine) — Alle Zufallsentscheidungen. Wiederkehrendes Muster: zwei RNG-Zuege, s0 = (rng1 & 0xFF) >> (rng2 & 3), danach Maske (&7 / &3 / &1)
- `@0x80100a94` -> `0x8002959C (Anim-Advance)` (a0 = s2 (@0x80100a88), a1 = s1 (@0x80100a8c), a2 = s3 (@0x80100a90), a3 = 128 (@0x80100a98)) — Unterzustand 0: Clip fortschalten, Rueckgabe != 0 = Clip zu Ende
- `@0x80100ab8` -> `0x8005BD6C (Gegner-SE)` (a0 = 9 (@0x80100ab4), a1 = s2 = Entity (@0x80100a8c)) — SE 9 mit 3/4-Chance am Clip-Ende
- `@0x80100bc4` -> `0x80104E9C (Befehl an EINEN Begleiter)` (a0 = s1 = buf[rng % n] (@0x80100bc8, Delay-Slot; buf ab sp+16 gefuellt @0x80100b20, Auswahl @0x80100b80), a1 = TAB_COMP_CMD[+0x16A][(rng>>(rng&3))&1] (@0x80100bc0)) — Begleiter[a0]+0x04 = a1 (0x0301 oder 0x0D01); Rumpf: @0x80104e9c sll a0,a0,2 / lw v0,-480(at)=0x800CFE20 / sw a1,4(v0)
- `@0x80100de0` -> `0x80104E5C (Befehl an ALLE vier Begleiter)` (a0 = 2817 = 0x0B01 (@0x80100dd4)) — Begleiter[0..3]+0x04 = 0x0B01; Rumpf schreibt 0x800CFE20/24/28/2C (@0x80104e60..@0x80104e98)
- `@0x80100dec` -> `0x8005BD6C` (a0 = 10 (@0x80100de8), a1 = s1 = Entity (@0x80100df0)) — SE 10 beim Start von Unterzustand 1
- `@0x80100e3c, @0x80100e68, @0x80100e94, @0x80100ec0` -> `0x80104E9C` (a0 = TAB_PERM_4x4[+0x16B][0..3] (@0x80100e38 lbu 16(v0) / @0x80100e64 lbu 17 / @0x80100e90 lbu 18 / @0x80100ebc lbu 19, Basis sp+16), a1 = 2817 (@0x80100e40 / @0x80100e6c / @0x80100e98 / @0x80100ec4)) — Anim-Frames 0/7/10/15 in Unterzustand 1 Phase 1
- `@0x80100ef0, @0x80100f0c` -> `0x80104E9C` (a0 = TAB_PERM_4x4[+0x16B][0] (@0x80100eec) bzw. [2] (@0x80100f08), a1 = 0x00070B01 (@0x80100ed8 lui 0x7 + @0x80100ef4 ori 0xb01 bzw. @0x80100efc + @0x80100f10)) — Anim-Frame 55
- `@0x80100f48, @0x80100f64` -> `0x80104E9C` (a0 = TAB_PERM_4x4[+0x16B][1] (@0x80100f44) bzw. [3] (@0x80100f60), a1 = 0x00070B01 (@0x80100f30 + @0x80100f4c bzw. @0x80100f54 + @0x80100f68)) — Anim-Frame 61
- `@0x80100f18, @0x80100f80, @0x80101098` -> `0x8005BD6C` (a0 = 9 (@0x80100f14) / 11 (@0x80100f7c) / 9 (@0x80101094); a1 = 0x00070B01 (Restwert @0x80100f10) / 0x00070B01 (@0x80100f68) / 0x00090B01 (@0x80101090) — in allen drei Faellen KEIN Entity-Zeiger) — SE 9 in Frame 55, SE 11 in Frame 90, SE 9 in Frame 25; 0x8005BD6C dereferenziert a1 (@0x8005bd90 lw v0,0(s2) / andi 0x2000)
- `@0x80100f94` -> `0x80015E7C` (a0 = s1 (@0x80100f88), a1 = s2 (@0x80100f8c), a2 = s3 (@0x80100f90), a3 = 0 (@0x80100f98)) — Liest +0x14C/+0x14D (@0x80015e80 / @0x80015ea0) und wertet die Wurzelbewegung des Clips aus
- `@0x80100fbc` -> `0x8002959C` (a0 = s1 (@0x80100f9c), a1 = s2 (@0x80100fa0), a2 = s3 (@0x80100fa4), a3 = 128 (@0x80100fac)) — Clip fortschalten; Rueckgabe wird auf +0x06 addiert
- `@0x80100fe0` -> `0x800152C8 (Vorwaerts bewegen)` (a0 = s1 (@0x80100fdc), a1 = 0 (@0x80100fe4)) — Rotiert den Vektor ab +0x144 (@0x80015304 addiu a1,s1,324) um Yaw+a1 und addiert ihn auf +0x38/+0x40
- `@0x80101008` -> `0x8005BD6C` (a0 = 10 (@0x80100ff8), a1 = s1 (@0x80100ffc)) — SE 10 beim Uebergang in die Bremsphase
- `@0x8010101c` -> `0x8002959C` (a0 = s1 (@0x80101010), a1 = s2 (@0x80101014), a2 = s3 (@0x80101018), a3 = 128 (@0x80101020)) — Bremsclip fortschalten
- `@0x80101070, @0x8010108c` -> `0x80104E9C` (a0 = TAB_PERM_4x4[+0x16B][0] (@0x8010106c) bzw. [2] (@0x80101088), a1 = 0x00090B01 (@0x80101058 + @0x80101074 bzw. @0x8010107c + @0x80101090)) — Anim-Frame 25 in Unterzustand 1 Phase 3
- `@0x801010c4, @0x801010e0` -> `0x80104E9C` (a0 = TAB_PERM_4x4[+0x16B][1] (@0x801010c0) bzw. [3] (@0x801010dc), a1 = 0x00090B01 (@0x801010ac + @0x801010c8 bzw. @0x801010d0 + @0x801010e4)) — Anim-Frame 15 in Unterzustand 1 Phase 3
- `@0x8010115c` -> `jr v0 = TAB_S2_PHASE[+0x06]` (berechneter Sprung, kein Aufruf; v0 @0x80101154, Index +0x06 @0x80101130, Schranke sltiu 0xf @0x8010113c) — Phasenverteiler Unterzustand 2 (0..14)
- `@0x80101294` -> `0x8005BD6C` (a0 = 9 (@0x8010127c), a1 = s0 = Entity (@0x80101284)) — SE 9 bei Timer 40 in Unterzustand 2 Phase 1
- `@0x801012dc` -> `0x8005BD6C` (a0 = 11 (@0x801012cc), a1 = s0 (@0x801012d8)) — SE 11 bei Timer 90 (Ende Phase 1)
- `@0x801017e0 (per j @0x801012e4)` -> `0x8005BD6C` (a0 = 10 (@0x801012e8), a1 = s0 (@0x801017dc)) — SE 10 direkt nach SE 11
- `@0x801012f8 (0x80015E7C) / @0x8010130c (0x8002959C) / @0x80101324 (0x800152C8)` -> `Bewegungs-Trio Unterzustand 2 Phase 2` (je a0 = s0 (@0x801012ec / @0x80101300 / @0x80101314), a1 = s1 (@0x801012f0 / @0x80101304), a2 = s2 (@0x801012f4 / @0x80101308), a3 = 0 (@0x801012fc) bzw. 128 (@0x80101310); 0x800152C8 mit a1 = 0 (@0x8010131c)) — Wurzelbewegung, Clip-Fortschritt (Rueckgabe auf +0x06 @0x80101320/@0x80101328) und Vorwaertsbewegung
- `@0x80101388` -> `0x8005BD6C` (a0 = 9 (@0x80101374), a1 = s0 (@0x80101378)) — SE 9 beim Eintritt in Phase 4
- `@0x8010139c, @0x80101428, @0x80101638` -> `0x8002959C` (a0 = s0 (@0x80101390 / @0x8010141c / @0x8010162c), a1 = s1 (@0x80101394 / @0x80101420 / @0x80101630), a2 = s2 (@0x80101398 / @0x80101424 / @0x80101634), a3 = 128 (@0x801013a0 / @0x8010142c / @0x8010163c)) — Clip-Fortschritt in den Unterzustand-2-Phasen 4, 5 und 10 (Rueckgabe wird dort NICHT ausgewertet, die Phasen enden ueber +0x158)
- `@0x801013e0` -> `0x8005BD6C` (a0 = 0 (@0x801013d0), a1 = s0 (@0x801013dc)) — SE 0 bei Timer 30 (alle Begleiter +0x06 = 8)
- `@0x80101414 -> @0x801017e0` -> `0x8005BD6C` (a0 = 10 (@0x80101404), a1 = s0 (@0x80101408)) — SE 10 beim Uebergang Phase 4 -> 5
- `@0x80101450, @0x80101474, @0x8010149c, @0x801014c0` -> `0x8005BD6C` (a0 = 0 (@0x80101440 / @0x80101464 / @0x8010148c / @0x801014b0), a1 = s0 (@0x80101444 / @0x80101468 / @0x80101490 / @0x801014b4)) — SE 0 bei Timer 20/40/45/56, jeweils zusammen mit Begleiterbefehl 0x0901 (sw @0x80101454 / @0x80101478 / @0x801014a0 / @0x801014c4)
- `@0x80101528 -> @0x801017e0` -> `0x8005BD6C` (a0 = 11 (@0x80101518), a1 = s0 (@0x80101524)) — SE 11 beim Uebergang Phase 5 -> 6
- `@0x8010153c (0x80015E7C) / @0x80101550 (0x8002959C) / @0x80101568 (0x800152C8)` -> `Bewegungs-Trio Unterzustand 2 Phase 6` (je a0 = s0 (@0x80101530 / @0x80101544 / @0x80101558), a1 = s1 (@0x80101534 / @0x80101548), a2 = s2 (@0x80101538 / @0x8010154c), a3 = 0 (@0x80101540) bzw. 128 (@0x80101554); 0x800152C8 mit a1 = 0 (@0x80101560)) — wie Phase 2; Rueckgabe auf +0x06 @0x80101564/@0x8010156c
- `@0x80101734 (per j @0x80101604)` -> `0x8005BD6C` (a0 = 10 (@0x801015f4), a1 = s0 (@0x80101600)) — SE 10 beim Uebergang Phase 7 -> 8
- `@0x80101624` -> `0x8005BD6C` (a0 = 9 (@0x8010160c), a1 = s0 (@0x80101610)) — SE 9 beim Eintritt in Phase 10
- `@0x80101660, @0x80101684, @0x801016ac, @0x801016d0` -> `0x8005BD6C` (a0 = 0 (@0x80101650 / @0x80101674 / @0x8010169c / @0x801016c0), a1 = s0 (@0x80101654 / @0x80101678 / @0x801016a0 / @0x801016c4)) — SE 0 bei Timer 20/35/40/50, jeweils mit Begleiterbefehl 0x0A01 (sw @0x80101664 / @0x80101688 / @0x801016b0 / @0x801016d4)
- `@0x80101734` -> `0x8005BD6C` (a0 = 10 (@0x80101720), a1 = s0 (@0x8010172c)) — SE 10 beim Eintritt in Phase 12
- `@0x80101748` -> `0x8002959C` (a0 = s0 (@0x8010173c), a1 = s1 (@0x80101740), a2 = s2 (@0x80101744), a3 = 128 (@0x8010174c)) — gemeinsamer Clip-Block der Phasen 8 und 12; Rueckgabe auf +0x06 (@0x80101758 / @0x80101760)
- `@0x80101788` -> `0x8005BD6C` (a0 = 9 (@0x80101764), a1 = s0 (@0x80101768)) — SE 9 beim Eintritt in Phase 14
- `@0x80101790` -> `0x80104E5C` (a0 = 1537 = 0x0601 (@0x80101794, Delay-Slot)) — Begleiter[0..3]+0x04 = 0x0601 am Sequenzende
- `@0x801017a4` -> `0x8002959C` (a0 = s0 (@0x80101798), a1 = s1 (@0x8010179c), a2 = s2 (@0x801017a0), a3 = 128 (@0x801017a8)) — Clip Phase 14
- `@0x801017d0` -> `0x8005BD6C` (a0 = 10 (@0x801017c0), a1 = s0 (@0x801017c4)) — SE 10 beim Ruecksprung nach Unterzustand 1 (direkt danach @0x801017e0 SE 9 mit a0 = 9 @0x801017d8, a1 = s0 @0x801017dc)
- `@0x8010187c` -> `0x8005BD6C` (a0 = 11 (@0x80101864), a1 = s2 = Entity (@0x80101868)) — SE 11 beim Start von Unterzustand 3
- `@0x801018e4` -> `0x8005BD6C` (a0 = 11 (@0x801018d4), a1 = s2 (@0x801018dc)) — SE 11 bei Timer 50 (+0x16A 0 -> 1)
- `@0x80101920` -> `0x8005BD6C` (a0 = 10 (@0x8010191c), a1 = s2 (@0x80101924, Delay-Slot)) — SE 10 bei +0x16A 1 -> 2
- `@0x80101958` -> `0x8002959C` (a0 = s2 (@0x8010193c), a1 = s1 (@0x80101950), a2 = s3 (@0x80101954), a3 = 128 (@0x8010195c)) — Clip Unterzustand 3; Rueckgabe != 0 -> Unterzustand 0
- `@0x80101a68` -> `0x80104E9C` (a0 = s1 = buf[rng % n] (@0x80101a6c, Delay-Slot; buf ab sp+16 @0x801019c4, Auswahl @0x80101a24), a1 = TAB_COMP_CMD[+0x16B][(rng>>(rng&3))&1] (@0x80101a64)) — Begleiterbefehl alle 32 Frames waehrend des Rammlaufs
- `@0x80101b3c` -> `0x80015B94` (a0 = 0x800CFBF8 = Spieler-Entity (@0x80101b20), a1 = s3 (@0x80101b24), a2 = s4 (@0x80101b28), a3 = 0 (@0x80101b2c)) — Spieler an die Greifpose binden (ruft intern 0x80015DB0 wie 0x80015CB8)
- `@0x80101bb8` -> `0x8005BD6C` (a0 = 6 (@0x80101b98), a1 = s2 = Entity (@0x80101bbc, Delay-Slot)) — SE 6 beim Zugriff
- `@0x80101bd0` -> `0x8005BD6C` (a0 = 7 (@0x80101bcc), a1 = s2 (@0x80101bd4, Delay-Slot)) — SE 7 in Anim-Frame 10
- `@0x80101c60` -> `0x8001BF10 (Effekt/Partikel)` (a0 = ((rng&1)<<16) | (((rng&0xFF)<<3)+6096) (@0x80101c54), a1 = Boss+0x76 (@0x80101c5c), a2 = [+0x198]+0x2F8 (@0x80101c64), a3 = sp+16 Streuvektor (@0x80101c58)) — Blutausstoss in den Anim-Frames 16..29
- `@0x80101c74` -> `0x80015CB8` (a0 = s2 (@0x80101c68), a1 = s3 (@0x80101c6c), a2 = s4 (@0x80101c70), a3 = 0 (@0x80101c78)) — Posen-/Matrixaufbau fuer den Greifzustand
- `@0x80101c88` -> `0x8002959C` (a0 = s2 (@0x80101c7c), a1 = s3 (@0x80101c80), a2 = s4 (@0x80101c84), a3 = 512 (@0x80101c8c)) — Greifclip fortschalten; Rueckgabe != 0 -> Phase 2
- `@0x80101cb8` -> `0x8005BD6C` (a0 = 10 (@0x80101ca8), a1 = s2 (@0x80101cac)) — SE 10 beim Uebergang in Phase 3
- `@0x80101cd4` -> `0x8002959C` (a0 = s2 (@0x80101cc8), a1 = s3 (@0x80101ccc), a2 = s4 (@0x80101cd0), a3 = 512 (@0x80101cd8)) — Clip der Endphase

## Offen
- Entity+0x228 Bits 0..3: In BEIDEN Overlays existiert kein Setzer. CDEMD0_EM36_ai1.BIN loescht das Byte nur einmal @0x80100494; CDEMD0_EM37_ai1.BIN hat genau 8 Zugriffe (@0x8010146c/147c, @0x80101808/1814, @0x80102b40/2b4c, @0x801033c8/33d4, @0x801034ac/34b8, @0x801035bc/35cc, @0x801038e0/38ec, @0x801039b0/39bc) und ALLE nutzen Bit (eigenes +0x218 + 4), also 4..7. Wer Bits 0..3 setzt (vermutlich der EXE-Todes-/Despawn-Pfad) ist NICHT belegt — solange sie nie gesetzt werden, ist der Teiltest 'Bit i frei' im Scan immer wahr.
- Entity+0x1D4 Bit0 (Startfreigabe der Beschwoerungssequenz, gelesen @0x801011b8): kein Schreibzugriff im EM36-Overlay. Die einzigen +0x1D4-Zugriffe im Chunk sind @0x801011b0 (lesen), @0x80103064 (lesen) und @0x80103080 (schreiben, aber |= 0x2). Der Setzer von Bit0 liegt ausserhalb (PSX.EXE oder RDT/SCD).
- Entity+0x10E: im gesamten EM36-Overlay NUR gelesen (@0x8010078c), kein Setzer. Solange (+0x10E & 0x3F) != 0 nicht vorkommt, sind die Tabelleneintraege 1..10 @0x80105648 nie Ziel des Wurzeldispatchers — sie sind physisch identisch mit den A/B-Untertabellen. Fuer den Port genuegt Index 0; abweichende Werte wuerden A/B-Funktionen mit falscher Signatur aufrufen.
- Entity+0x14E = 0x1F wird bei jedem Clipwechsel als oberes Halbwort des sw auf +0x14C mitgeschrieben (@0x801009a8, @0x80100dd8, @0x80101000, @0x8010116c, @0x8010138c, @0x801014f8, @0x801015d0, @0x80101618, @0x80101710, @0x80101770, @0x8010186c); in Unterzustand 4 steht dort 0x0007 (@0x80101b30, @0x80101cb0). Weder EM36 noch EM37 lesen +0x14E/+0x14F — der Leser liegt in der PSX.EXE und ist nicht belegt. Die Bank-Semantik ist erschlossen, nicht bewiesen.
- Entity+0x144 wird in Unterzustand 1 Phase 1 nur multipliziert (@0x80100fb8/@0x80100fc0), nie initialisiert. Der einzige Setzer im Zustand 1 ist @0x80101930 (Unterzustand 3, = +0x158). Dass 0x80015E7C den Wert aus der Wurzelbewegung des Clips schreibt, ist NICHT belegt (nur @0x80015e7c..@0x80015ec8 gelesen).
- Die Funktionsrumpfe von 0x80015E7C, 0x80015B94, 0x80015CB8 (beide rufen 0x80015DB0) und 0x8001BF10 wurden nicht zu Ende disassembliert; ihre Argumentbedeutung ist aus dem Aufrufkontext erschlossen.
- Bei drei SE-Aufrufen ist a1 KEIN Entity-Zeiger, sondern der Restwert des vorangegangenen 0x80104E9C-Aufrufs: @0x80100f18 (0x00070B01 aus @0x80100f10), @0x80100f80 (0x00070B01 aus @0x80100f68), @0x80101098 (0x00090B01 aus @0x80101090). 0x8005BD6C dereferenziert a1 (@0x8005bd90). Ob das ein Originalfehler ist, ist nicht geklaert; byte-true muss der Port dieselben Werte uebergeben.
- Die Bedeutung der Begleiter-Zustandsworte 0x0301, 0x0601, 0x0901, 0x0A01, 0x0B01, 0x0D01, 0x00040A01, 0x00070B01, 0x00090B01 sowie der Begleiter-Phasenwerte 1, 6, 7, 8, 9 liegt in CDEMD0_EM37_ai1.BIN und wurde hier NICHT zerlegt.
- Das Ziel [[+0x198]+0x1E0]+0x0C/+0x10/+0x14/+0x18 (Halbworte; geschrieben @0x80101188, @0x80101194, @0x801011a0, @0x801011ac, @0x80101368, @0x801015b4, @0x801015c8; gelesen @0x80101d0c) ist nicht identifiziert. Faktor 4415/8830 und Schiebeweiten 11/12 sind woertlich belegt, die physikalische Bedeutung nicht.
- Overlay-Global 0x80105BB0 (sh @0x80101200, lh @0x80101334 und @0x80101578) liegt im allerletzten Wort des 23476-Byte-Chunks; ob im Original weitere Overlay-Daten anschliessen, ist nicht geprueft.
- Zustand 3 (0x80102BBC) setzt +0x226 |= 4 @0x80103010 (zusammen mit HP = 0 @0x80103004 und +0x04 = [+0x1FC] @0x80103008) und ruft Zustand 1 direkt per jal @0x80103028 sowie @0x80102a08 und @0x80102a70 auf. Diese Wiedereintritte wurden nur so weit gelesen, wie es fuer das Zustand-1-Tor noetig war; die Zustaende 0/2/3/7 sind nicht Teil dieses Auftrags.
- Die 10 leeren Funktionen 0x80101D4C, 0x80101D54, 0x80101D5C, 0x80101D64, 0x80101D6C, 0x80101D74, 0x80101D7C, 0x80101D84, 0x80101D8C, 0x80101D94 (je 'jr ra; nop') haben im gesamten Chunk WEDER jal/j-Xref NOCH Datenwort-Xref (alle 5869 Worte geprueft). Ihr Zweck bleibt offen.
- Die Datenbloecke @0x801056B4..@0x801056E7 (0x05050505, 0x140E140E, 0x050E0E0E, 0x01011405, 0x00000114 und vier Koordinatentripel 0x076F/0xFBEA/0xF6FA, 0x06A7/0xF5AA/0xFD12, 0x0959/0xFDDE/0x0582, 0x05A3/0xF726/0x0582) sowie die Zeiger @0x801056E8 = 0x801038BC und @0x801056EC = 0x80103908 werden von Zustand 1 NICHT gelesen; die einzigen Zugriffe auf 0x80105674 sind @0x80100B98 und @0x80101A3C. Ein Byte-Zugriff auf 0x801056B3 erfolgt aus Zustand 2 @0x80102A28 und wurde dort nicht weiterverfolgt.
- Der Finisher-Pfad verlangt Spieler-HP <= 0 (@0x801008e0 bgtz -> Ende) und schlaegt DANN nochmal 500 Schaden zu. Dass 0x800CFD4E die Spieler-HP ist, ist ueber 0x800401D4 hart belegt (@0x800401e0 a2 = 0x800CFBF8, @0x80040248 lhu 342(a2), @0x80040250 subu, @0x8004025c sh). Warum die Bedingung HP <= 0 und nicht 'HP niedrig' lautet, wurde nicht weiter untersucht — es ist woertlich so codiert.


---

# RE2 EM36 (Endgegner Typ 0x36) — Zustand 2 = TREFFERREAKTION. Wurzel 0x801000BC, Zustandstabelle @0x801055CC[2] = 0x801025BC. Datei build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN, Ladebasis 0x80100000. Umfang: Hauptfunktion 0x801025BC..0x80102ACC (jr ra @0x80102AC8), Stagger-Executor 0x80102AD0..0x80102BB8 (jr ra @0x80102BB4), plus die vier Blut-Emitter 0x80101D9C / 0x80101FF0 / 0x8010221C / 0x801022D0. Nachfolgefunktion in der Tabelle: 0x80102BBC = Zustand 3 (Tod).

Zustand 2 ist byte-belegt die Trefferreaktion: der gemeinsame EXE-Schadensaufloeser schreibt @0x80047288 `sw v0,4(s1)` mit v0=2 (WORT, damit werden +0x5/+0x6/+0x7 genullt), sichert vorher das alte Zustandswort nach +0x1FC (@0x80047278, unterdrueckt wenn das alte Wort == 0x0C02 ist, @0x8004726C-70) und setzt danach +0x5 := s5 (Treffer-Reaktions-Id) @0x80047324 sowie +0x1D2 := Hoehenklasse(0/1/2) + 3*Trefferzone @0x80047330. Die Funktion 0x801025BC hat KEINE Sprungtabelle, sondern zwei Phasen ueber +0x6 (0 = frischer Treffer, !=0 = Stagger laeuft) und eine Kette von Direktvergleichen auf +0x5. Phase 0 macht drei Dinge: (a) Blutspritzer — sie schneidet den Blickstrahl des SPIELERS (Yaw @0x800CFC6E, Position @0x800CFC30/0x800CFC38) mit der Ebene X = Gegner+0x38+3183, klemmt Z auf [-26000,-20500] und setzt dort zwei Partikelemissionen ab; liegt das Z im Band [-23999,-22501], gilt der Treffer als Koerpertreffer und es folgen zwei weitere Emissionen an der Wundposition +0xA4/+0xA8/+0xAC plus SE 8. (b) Sie ruft je nach +0x5 einen der vier grossen Blut-Emitter (9/17 -> 0x8010221C, 10 -> 0x80101D9C, 11 -> 0x80101FF0, 14 -> 0x801022D0). (c) Sie addiert das Stagger-Gewicht aus der Bytetabelle @0x801056B3[+0x5] auf den Akkumulator +0x222; solange +0x222 < 15 wird der alte Zustand aus +0x1FC zurueckgeschrieben und die normale Aktion 0x80100784 noch im selben Frame weitergefahren — Zustand 2 dauert also genau EINEN Frame pro Treffer. Erreicht +0x222 >= 15, werden Akku/Timer/Bit0 geloescht und der Stagger-Executor 0x80102AD0 uebernimmt: er setzt +0x226|=2, +0x14C-Wort := 0x001F0008 (Clip 8, Frame 0, 31 Frames Crossfade), +0x6 := 1, spielt SE 12, bei Animationsframe +0x14D == 15 zusaetzlich SE 9, und schaltet nach Clip-Ende auf +0x4 := 1 (Wort) zurueck. Der Akku zerfaellt in der KI-Wurzel um 1 alle 16 Frames (@0x80100118-0x8010015C).

## Konstanten

| Name | Wert | Bedeutung | Adresse |
|---|---|---|---|
| STAGGER_WEIGHT_TABLE | u8[19] @0x801056B3 = {0,5,5,5,5,14,20,14,20,14,14,14,5,5,20,1,1,20,1} (danach 0,0 @0x801056C6/C7) | Feld: Stagger-Gewicht je Treffer-Reaktions-Id +0x5, wird auf +0x222 addiert | @0x801029bc lbu v0,22195(at) und @0x80102a28 lbu v1,22195(at) |
| STAGGER_FLASH_THRESHOLD | 11 (sltiu 0xb) | Feld: Gewicht >= 11 => +0x225 := 7 | @0x801029c4 sltiu v0,v0,0xb |
| HIT_FLASH_FRAMES | 7 | Dauer: +0x225 (Trefferblitz-Zaehler) | @0x801029cc addiu v0,zero,7 / @0x801029d0 sb v0,549(s3) |
| STAGGER_ACC_THRESHOLD | 15 (sltiu 0xf) | Feld: +0x222 >= 15 loest den Stagger aus | @0x80102a58 sltiu v0,v0,0xf |
| STAGGER_DECAY_RELOAD | 15 | Dauer: +0x221 Nachladewert, ergibt Zerfall 1 Punkt je 16 Frames | @0x80102a48 addiu v0,zero,15 (Erstsetzung) und @0x80100144 addiu v0,zero,15 (Nachladen) |
| STAGGER_CLIP_WORD | 0x001F0008 => +0x14C(Clip)=8, +0x14D(Frame)=0, +0x14E(Crossfade)=31, +0x14F=0 | Clip + Frame + Ueberblendzaehler des Stagger | @0x80102b18 lui v0,0x1f / @0x80102b1c ori v0,v0,0x8 / @0x80102b28 sw v0,332(s0) |
| STAGGER_ANIM_BLEND_STEP | 128 (a3 von 0x8002959C) | Feld: Crossfade-Schrittgewicht, multipliziert mit +0x14E @0x800296bc; +0x14E wird je Frame dekrementiert @0x800299c0-@0x800299cc | @0x80102b78 addiu a3,zero,128 |
| SE_MID_ANIM_FRAME | 15 | Frame: +0x14D == 15 => SE 9 | @0x80102b3c addiu v0,zero,15 / @0x80102b40 bne v1,v0,0x80102b58 |
| SE_HIT_BODY | 8 (+16 falls Entity+0x00 & 0x2000, @0x8005bd98/@0x8005bda4) | SE-Id beim Koerpertreffer | @0x8010293c addiu a0,zero,8 |
| SE_STAGGER_START | 12 (+16 falls Entity+0x00 & 0x2000) | SE-Id beim Stagger-Beginn | @0x80102b20 addiu a0,zero,12 |
| SE_STAGGER_MID | 9 (+16 falls Entity+0x00 & 0x2000) | SE-Id bei Animationsframe 15 | @0x80102b48 addiu a0,zero,9 |
| AIM_RAY_LEN | 4096 (Q12 = 1.0) | Distanz: Laenge des rotierten Blickvektors (SVECTOR.x vor ApplyMatrixSV) | @0x80102628 addiu v0,zero,4096 / @0x8010262c sh v0,16(sp) |
| AIM_DIRX_GATE | -64 | Schwelle: rotiertes dirX muss < -64 sein, sonst kein Blutblock (schuetzt zugleich die Division) | @0x80102640 slti v0,a1,-64 |
| PLANE_X_OFFSET | 3183 | Distanz: Schnittebene X = Gegner+0x38 + 3183 (als -3183 auf SpielerX gerechnet) | @0x8010267c addiu v1,v1,-3183 und @0x801026d4 addiu v0,v0,3183 |
| BLOOD_PLANE_Y | -1400 | Distanz: feste Y-Hoehe der Ebenen-Blutspritzer (vecB.y) | @0x801026cc addiu v1,zero,-1400 / @0x801026d0 sh v1,26(sp) |
| HITHEIGHT_Y_LOW | -700 (nur wenn +0x1D2 % 3 == 0) | Feld: geschrieben nach sp+18 = SVECTOR-A.y (Richtungsvektor) — TOTE ZUWEISUNG, siehe offen[] | @0x80102708 addiu v0,zero,-700 / @0x8010270c sh v0,18(sp) |
| HITHEIGHT_Y_HIGH | -2000 (nur wenn +0x1D2 % 3 == 2) | Feld: dito, tote Zuweisung nach sp+18 | @0x8010273c addiu v0,zero,-2000 / @0x80102740 sh v0,18(sp) |
| HITHEIGHT_MOD | 3 (Magic 0xAAAAAAAB, mfhi>>1) | Feld: +0x1D2 mod 3 = Hoehenklasse (0=tief, 1=mitte, 2=hoch) | @0x801026e0-@0x80102700 und @0x80102718-@0x80102730 |
| BLOOD_Z_CLAMP_MAX | -20500 (Test slti < -20499) | Distanz: obere Klemme fuer vecB.z | @0x80102760 slti v0,v0,-20499 / @0x80102768 addiu v0,zero,-20500 / @0x8010276c sh v0,28(sp) |
| BLOOD_Z_CLAMP_MIN | -26000 | Distanz: untere Klemme fuer vecB.z | @0x80102778 slti v0,v0,-26000 / @0x80102780 addiu v0,zero,-26000 / @0x80102784 sh v0,28(sp) |
| SUB1_Z_OVERRIDE | [[0x800CFD90] + 1992] (Wort) | Feld: bei +0x5 == 1 wird vecB.z durch diesen globalen Wert ersetzt | @0x80102788 lbu v1,5(s3) / @0x8010278c addiu v0,zero,1 / @0x80102798-@0x801027ac (lw v0,-624(0x800d0000) ; lw v0,1992(v0) ; sh v0,28(sp)) |
| BODY_HIT_BAND | vecB.z + 23999 < 1499  ==> z in [-23999,-22501] | Distanz: Trefferband auf der Schnittebene, setzt s4 = 1 | @0x80102858 addiu v0,v0,23999 / @0x80102860 sltiu v0,v0,0x5db |
| BLOOD_X_STEP | +500 | Distanz: vecB.x-Versatz zwischen den beiden Ebenen-Emissionen | @0x801027fc addiu v0,v0,500 / @0x80102804 sh v0,24(sp) |
| BLOOD_Z_JITTER | vecB.z -= ((rng & 0xff) - 128) | Distanz: Streuung +/-128 | @0x80102808 andi v0,v0,0xff / @0x80102810 addiu v0,v0,-128 / @0x80102814 subu v1,v1,v0 / @0x8010281c sh v1,28(sp) |
| FX_ID_PLANE_1 | ((rng1 & 1) << 16) | (8096 + (rng2 & 0xff) * 8) | Effekt-Id (Kategorie 0, Variante rng1&1, Param 8096..10136) | @0x801027c0-@0x801027d4 |
| FX_ID_PLANE_2 | ((rng4 & 1) << 16) | (8096 + (rng5 & 0xff) * 4) | Effekt-Id (Kategorie 0, Param 8096..9116) | @0x80102828-@0x8010283c |
| WOUND_POS | vecB = (+0xA4 + 500, +0xA8, +0xAC) | Feld: Wundposition fuer die Koerpertreffer-Emission | @0x801028a0 lw v0,164(s3) / @0x801028a8 addiu v0,v0,500 / @0x801028ac sh v0,24(sp) / @0x801028b0 lw v0,168(s3) / @0x801028b8 sh v0,26(sp) / @0x801028bc lw v0,172(s3) / @0x801028c4 sh v0,28(sp) |
| FX_ID_WOUND_1 | 8096 + (rng & 0xff) * 8 | Effekt-Id (Kategorie 0, Variante 0) | @0x801028c8 andi v0,v0,0xff / @0x801028cc sll v0,v0,3 / @0x801028d0 addiu a0,v0,8096 |
| FX_ID_WOUND_2 | (8096 + (rng & 0xff) * 8) | 0x00010000 | Effekt-Id (Kategorie 0, Variante 1) | @0x8010290c-@0x8010291c (andi 0xff ; sll 3 ; addiu 8096 ; lui a0,0x1 ; or a0,v0,a0) |
| FX_MATRIX_PTR | 0x8009DB44 (32-Byte-MATRIX, wird nach FX+76..+104 kopiert @0x8001c03c-@0x8001c078) | Feld: Emitter-Koordinatensystem aller vier Emissionen in z2 p0 | @0x801027dc lui s2,0x800a / @0x801027e0 addiu s2,s2,-9404 |
| GUARD_STATE_WORD | 3074 = 0x0C02 (Zustand 2, Unterzustand 12, Phase 0); Zwilling prueft 3074..3075 | Feld: unterdrueckt das Ueberschreiben von +0x1FC beim Nachtreffer | @0x8004726c addiu v0,zero,3074 / @0x80047270 beq v1,v0,0x8004727c ; @0x8004748c addiu v0,a0,-3074 / @0x80047490 sltiu v0,v0,0x2 |
| DEATH_FLASH_FRAMES | 240 | Dauer: +0x225 im Todeszustand z3 (Vergleichswert zu HIT_FLASH_FRAMES) | @0x80102fec addiu v1,zero,240 / @0x80102ff8 sb v1,549(s3) |
| EMIT_9_17_FX_A | a0 = 0x05042710 (Kat 5, Var 4, Param 10000) | Effekt-Id, Emitter 0x8010221C erste Emission | @0x80102228 lui a0,0x504 / @0x80102244 ori a0,a0,0x2710 |
| EMIT_9_17_FX_B | a0 = 0x040F1000 | ((rng & 0xff) << 2) | Effekt-Id, Emitter 0x8010221C zweite Emission | @0x80102294 lui a0,0x40f / @0x80102298 ori a0,a0,0x1000 / @0x8010229c-@0x801022a4 |
| EMIT_BONE_ANCHOR | vec = ([0x800CFB88] - part+0x1B4, [0x800CFB8C] - part+0x1B8, [0x800CFB90] - part+0x1BC), part = Entity+0x198; a2 = part + 416 (0x1A0) | Feld: Ankerpunkt der Emitter 0x8010221C / 0x80101D9C / 0x80101FF0 | @0x80102234 lw s0,408(s1) / @0x8010223c lw v0,-1144(v0) / @0x80102240 lw v1,436(s0) / @0x8010224c sh v0,16(sp) / @0x80102254+@0x80102258+@0x80102264 / @0x8010226c+@0x80102270+@0x8010227c / @0x80102274 addiu s0,s0,416 |
| EMIT_10_SEQ | Emitter 0x80101D9C, 8 Emissionen. a0: 0x05042710 ; 0x040F1000|r*4 ; 0x05042710 (nach vec.x+=800) ; 0x05040000|(8000+r*4) (nach vec.x-=800) ; 0x05040000|(8000+r*4) (vec.z-=812, vec.y+=256-2r) ; 0x05040000|(4000+r*4) (vec.z-=412, vec.y+=256-2r) ; 0x05040000|(8000+r*4) (vec.z+=2036, vec.y+=256-2r) ; 0x05040000|(4000+r*4) (vec.z+=412, vec.y+=256-2r) | Effekt-Ids und Distanz-Offsets von Unterzustand 10 | @0x80101da8+@0x80101dc8 ; @0x80101e18+@0x80101e1c ; @0x80101e3c+@0x80101e40+@0x80101e4c ; @0x80101e68+@0x80101e7c ; @0x80101e9c+@0x80101eb4+@0x80101ecc ; @0x80101eec+@0x80101f04+@0x80101f1c ; @0x80101f3c+@0x80101f54+@0x80101f6c ; @0x80101f8c+@0x80101fa4+@0x80101fbc |
| EMIT_11_SEQ | Emitter 0x80101FF0, 6 Emissionen. a0: 0x040F0000|(10096+r*4) ; 0x040F0000|(10000+r*4) (vec.x+=800) ; 0x040F0000|(8000+r*4) (vec.x-=800, vec.z-=612) ; 0x040F0000|(8000+r*4) (vec.z+=1224, vec.y+=256-2r) ; 0x040F0000|(6000+r*4) (vec.z+=-868+2r, vec.y+=256-2r) ; 0x040F0000|(6000+r*4) (vec.z+=256-2r, vec.y+=256-2r) | Effekt-Ids und Distanz-Offsets von Unterzustand 11 | @0x80102060+@0x80102064 ; @0x80102088+@0x8010209c ; @0x801020bc+@0x801020c0+@0x801020d8 ; @0x801020f8+@0x80102110+@0x80102128 ; @0x80102154+@0x80102170+@0x80102188 ; @0x801021b4+@0x801021d0+@0x801021e8 |
| EMIT_14_SEQ | Emitter 0x801022D0, 8 Emissionen mit ABSOLUTEN Vektoren; a2 = Entity+0x198 + 72 (0x48), ab der 7. Emission a2 = Entity+0x198 + 244 (0xF4). vec: (4241-2r, 2356-2r, 256-2r) ; dito ; (4241-2r, 3756-2r, 1296-2r) ; (3441-2r, 2856-2r, -1104-2r) ; dito ; (1741-2r, 756-2r, 196-2r) ; (1720-2r, 756-4r, 196-4r) ; dito. a0: 0x06000000|(10296+r*4) ; 0x040F0000|(6000+r*4) ; 0x06000000|(10296+r*4) ; 0x06000000|(10296+r*4) ; 0x040F0000|(6000+r*4) ; 0x06000000|(10296+r*4) ; 0x06000000|(6696+r*4) ; 0x040F0000|(4000+r*4) | Effekt-Ids und Distanzen von Unterzustand 14 | @0x801022fc addiu s0,zero,4241 ; @0x8010231c addiu v1,zero,2356 ; @0x80102334 addiu v1,zero,256 ; @0x8010234c addiu v0,v0,10296 + @0x80102350 lui s4,0x600 ; @0x80102360 addiu s2,s2,72 ; @0x8010237c addiu v0,v0,6000 + @0x80102370 lui s5,0x40f ; @0x80102398 addiu s1,zero,756 ; @0x801023b8 addiu v1,zero,3756 ; @0x801023d0 addiu v1,zero,1296 ; @0x80102404 addiu s0,zero,196 ; @0x80102410 addiu v1,zero,3441 ; @0x80102428 addiu v1,zero,2856 ; @0x80102440 addiu v1,zero,-1104 ; @0x801024a8 addiu v1,zero,1741 ; @0x80102508 addiu s2,s2,244 ; @0x80102514 addiu v1,zero,1720 ; @0x80102554 addiu v0,v0,6696 ; @0x8010257c addiu v0,v0,4000 |

## Uebergaenge

- **z1 (bzw. beliebig) p* -> z2 p0 sub=s5** wenn EXE-Schadensaufloeser: HP(+0x156) -= dmg, danach HP >= 0
  - 0x80047258 lhu v0,342(s1) / 0x80047260 subu v0,v0,v1 / 0x80047264 lw v1,4(s1) / 0x80047268 sh v0,342(s1) / 0x8004726c addiu v0,zero,3074 / 0x80047270 beq v1,v0,0x8004727c / 0x80047278 sw v1,508(s1) / 0x8004727c lh v1,342(s1) / 0x80047280 addiu v0,zero,2 / 0x80047284 bgez v1,0x80047294 / 0x80047288 sw v0,4(s1) ; Zwilling: 0x8004748c-0x800474ac (Guard a0-3074 sltiu <2) und 0x800418e4-0x800418f4
- **z2 p0 (frisch gesetzt) -> z2 p0, +0x5 := s5, +0x1D2 := Hoehenklasse+3*Zone** wenn immer, unmittelbar nach dem sw 2
  - 0x80047294 addiu v0,zero,1 / 0x80047298 sb v0,466(s1) / 0x800472ac lhu v0,152(s1) / 0x800472b8 sra a0,v0,17 / 0x800472bc lw v0,60(s1) / 0x800472c8 slt v0,v0,v1 / 0x800472d4 sb zero,466(s1) / 0x80047308 addiu v0,zero,2 / 0x8004730c sb v0,466(s1) / 0x80047324 sb s5,5(s1) / 0x80047328 addu v0,v0,v1 (v1 = s6*3) / 0x80047330 sb v0,466(s1) / 0x80047334 sb a0,467(s1)
- **beliebig -> z3 (0x80102BBC, Tod)** wenn HP nach Abzug < 0
  - 0x8004727c lh v1,342(s1) / 0x80047284 bgez v1,0x80047294 / 0x8004728c addiu v0,zero,3 / 0x80047290 sw v0,4(s1)
- **z2 Eintritt -> Stagger-Executor 0x80102AD0** wenn +0x6 != 0
  - 0x801025e8 lbu v0,6(s3) / 0x801025f0 bne v0,zero,0x80102a94 / 0x80102a94 addu a0,s3,zero / 0x80102a9c jal 0x80102ad0
- **z2 p0 -> Blutblock 0x801025F8-0x8010286C uebersprungen** wenn rotierte Blickrichtung dirX >= -64
  - 0x80102638 lh a1,16(sp) / 0x80102640 slti v0,a1,-64 / 0x80102644 beq v0,zero,0x80102870 / 0x80102648 addu s4,zero,zero
- **z2 p0 -> Blutblock uebersprungen** wenn +0x5 == 16
  - 0x8010264c lbu a0,5(s3) / 0x80102650 addiu v0,zero,16 / 0x80102654 andi v1,a0,0xff / 0x80102658 beq v1,v0,0x80102870
- **z2 p0 -> Blutblock uebersprungen** wenn +0x5 in [9,12] (9,10,11,12)
  - 0x8010265c addiu v0,a0,-9 / 0x80102660 sltiu v0,v0,0x4 / 0x80102664 bne v0,zero,0x80102870
- **z2 p0 Blutblock -> s4 := 1 (Koerpertreffer)** wenn (u16)(vecB.z + 23999) < 1499  ==> vecB.z in [-23999,-22501]
  - 0x80102850 lhu v0,28(sp) / 0x80102858 addiu v0,v0,23999 / 0x8010285c andi v0,v0,0xffff / 0x80102860 sltiu v0,v0,0x5db / 0x80102864 beq v0,zero,0x80102870 / 0x8010286c addiu s4,zero,1
- **z2 p0, s4==1 -> Wund-Emission + SE 8** wenn s4 == 1; SE nur wenn +0x5 != 1
  - 0x80102894 addiu v0,zero,1 / 0x80102898 bne s4,v0,0x80102948 / 0x80102930 lbu v0,5(s3) / 0x80102938 beq v0,s4,0x80102948 / 0x8010293c addiu a0,zero,8 / 0x80102940 jal 0x8005bd6c
- **z2 p0 -> Emitter 0x8010221C** wenn +0x5 == 9 ODER +0x5 == 17
  - 0x80102948 lbu v1,5(s3) / 0x8010294c addiu v0,zero,9 / 0x80102950 beq v1,v0,0x80102960 / 0x80102954 addiu v0,zero,17 / 0x80102958 bne v1,v0,0x80102970 / 0x80102960 jal 0x8010221c
- **z2 p0 -> Emitter 0x80101D9C** wenn +0x5 == 10
  - 0x80102968 lbu v1,5(s3) / 0x8010296c addiu v0,zero,10 / 0x80102970 bne v1,v0,0x80102980 / 0x80102978 jal 0x80101d9c
- **z2 p0 -> Emitter 0x80101FF0** wenn +0x5 == 11
  - 0x80102980 lbu v1,5(s3) / 0x80102984 addiu v0,zero,11 / 0x80102988 bne v1,v0,0x801029a0 / 0x80102990 jal 0x80101ff0
- **z2 p0 -> Emitter 0x801022D0** wenn +0x5 == 14
  - 0x80102998 lbu v1,5(s3) / 0x8010299c addiu v0,zero,14 / 0x801029a0 bne v1,v0,0x801029b0 / 0x801029a8 jal 0x801022d0
- **z2 p0 -> +0x225 := 7 (Trefferblitz)** wenn tab[0x801056B3 + +0x5] >= 11
  - 0x801029b0 lbu v0,5(s3) / 0x801029b4 lui at,0x8010 / 0x801029b8 addu at,at,v0 / 0x801029bc lbu v0,22195(at) [=0x801056B3] / 0x801029c4 sltiu v0,v0,0xb / 0x801029c8 bne v0,zero,0x801029d4 / 0x801029cc addiu v0,zero,7 / 0x801029d0 sb v0,549(s3)
- **z2 p0 -> +0x4 := +0x1FC, dann 0x80100784 ODER 0x80102AD0** wenn +0x226 & 0x2 gesetzt (Stagger scharf, erneuter Treffer)
  - 0x801029d4 lhu v0,550(s3) / 0x801029dc andi v0,v0,0x2 / 0x801029e0 beq v0,zero,0x80102a18 / 0x801029e8 lw v0,508(s3) / 0x801029f0 sw v0,4(s3) / 0x801029f4 lbu v1,4(s3) / 0x801029f8 addiu v0,zero,1 / 0x801029fc bne v1,v0,0x80102a98 / 0x80102a08 jal 0x80100784 / 0x80102a98 (sonst) jal 0x80102ad0
- **z2 p0 -> +0x222 += tab[+0x5]; +0x226 |= 1; +0x221 := 15** wenn +0x226 & 0x2 == 0; Bit0/+0x221 nur wenn Bit0 noch 0
  - 0x80102a18 lbu v1,5(s3) / 0x80102a1c lbu v0,546(s3) / 0x80102a28 lbu v1,22195(at) / 0x80102a2c lhu a0,550(s3) / 0x80102a30 addu v0,v0,v1 / 0x80102a34 sb v0,546(s3) / 0x80102a38 andi v0,a0,0x1 / 0x80102a3c bne v0,zero,0x80102a50 / 0x80102a40 ori v0,a0,0x1 / 0x80102a44 sh v0,550(s3) / 0x80102a48 addiu v0,zero,15 / 0x80102a4c sb v0,545(s3)
- **z2 p0 -> z(alt): +0x4 := +0x1FC, danach 0x80100784 im selben Frame** wenn +0x222 < 15
  - 0x80102a50 lbu v0,546(s3) / 0x80102a58 sltiu v0,v0,0xf / 0x80102a5c beq v0,zero,0x80102a80 / 0x80102a68 lw v0,508(a0) / 0x80102a70 jal 0x80100784 / 0x80102a74 sw v0,4(a0) / 0x80102a78 j 0x80102aa4
- **z2 p0 -> z2 p1 (Stagger)** wenn +0x222 >= 15 -> +0x222 := 0, +0x221 := 0, +0x226 &= ~1, Durchfall nach 0x80102A94
  - 0x80102a80 lhu v0,550(s3) / 0x80102a84 sb zero,546(s3) / 0x80102a88 sb zero,545(s3) / 0x80102a8c andi v0,v0,0xfffe / 0x80102a90 sh v0,550(s3) / 0x80102a94 addu a0,s3,zero / 0x80102a9c jal 0x80102ad0
- **z2 (Executor-Eintritt) -> +0x226 |= 2** wenn immer, vor der Phasenweiche
  - 0x80102af0 lhu v0,550(s0) / 0x80102af8 ori v0,v0,0x2 / 0x80102b00 sh v0,550(s0)
- **z2 p0 (im Executor) -> z2 p1** wenn +0x6 == 0 -> Clipwort +0x14C := 0x001F0008, SE 12, +0x6 := 1
  - 0x80102af4 lbu v1,6(s0) / 0x80102afc beq v1,zero,0x80102b18 / 0x80102b18 lui v0,0x1f / 0x80102b1c ori v0,v0,0x8 / 0x80102b20 addiu a0,zero,12 / 0x80102b24 addu a1,s0,zero / 0x80102b28 sw v0,332(s0) / 0x80102b2c addiu v0,zero,1 / 0x80102b30 jal 0x8005bd6c / 0x80102b34 sb v0,6(s0)
- **z2 p1 -> SE 9** wenn +0x14D (Animationsframe) == 15
  - 0x80102b38 lbu v1,333(s0) / 0x80102b3c addiu v0,zero,15 / 0x80102b40 bne v1,v0,0x80102b58 / 0x80102b48 addiu a0,zero,9 / 0x80102b4c jal 0x8005bd6c / 0x80102b50 addu a1,s0,zero
- **z2 p1 -> z1 p0 sub0** wenn 0x8002959C liefert v0 != 0 (Clip 8 zu Ende) -> +0x4 := 1 (Wort!), +0x226 &= ~2
  - 0x80102b74 jal 0x8002959c / 0x80102b78 addiu a3,zero,128 / 0x80102b7c beq v0,zero,0x80102b94 / 0x80102b80 addiu v1,zero,1 / 0x80102b84 lhu v0,550(s0) / 0x80102b88 sw v1,4(s0) / 0x80102b8c andi v0,v0,0xfffd / 0x80102b90 sh v0,550(s0)
- **z2 p>=2 -> sofortiger Ruecksprung (kein Code)** wenn +0x6 != 0 und != 1
  - 0x80102b04 addiu v0,zero,1 / 0x80102b08 beq v1,v0,0x80102b38 / 0x80102b10 j 0x80102ba0
- **KI-Wurzel, jeder Frame -> Akku-Zerfall** wenn +0x226 & 1: +0x221 -= 1 (Bytewrap); bei Wrap +0x222 -= 1 und +0x221 := 15; +0x222 == 0 -> +0x226 &= ~1
  - 0x80100104 lhu v0,550(s3) / 0x8010010c andi v0,v0,0x1 / 0x80100110 beq v0,zero,0x80100160 / 0x80100118 lbu v1,545(s3) / 0x80100120 addiu v0,v1,255 / 0x80100124 bne v1,zero,0x80100160 / 0x80100128 sb v0,545(s3) / 0x8010012c lbu v0,546(s3) / 0x80100134 beq v0,zero,0x80100140 / 0x80100138 addiu v0,v0,-1 / 0x8010013c sb v0,546(s3) / 0x80100140 lbu v1,546(s3) / 0x80100144 addiu v0,zero,15 / 0x80100148 bne v1,zero,0x80100160 / 0x8010014c sb v0,545(s3) / 0x80100158 andi v0,v0,0xfffe / 0x8010015c sh v0,550(s3)
- **KI-Wurzel -> gesamte KI uebersprungen** wenn global [0x800CFBDC] & 0x20000000
  - 0x801000cc lw v0,-1060(v0) [0x800cfbdc] / 0x801000d0 lui v1,0x2000 / 0x801000e0 and v0,v0,v1 / 0x801000e4 bne v0,zero,0x8010038c
- **z1-Vorstufe 0x801007C4 -> z3** wenn (+0x226 & 6) == 4
  - 0x801007e0 lhu v0,550(s0) / 0x801007e4 addiu v1,zero,4 / 0x801007e8 andi v0,v0,0x6 / 0x801007ec bne v0,v1,0x80100800 / 0x801007f4 addiu v0,zero,3 / 0x801007fc sw v0,4(s0)
- **z3 (0x80102BBC, Gegenstueck zu z2) -> z3-Abschluss** wenn +0x226 & 2 -> +0x225 := 240, HP := 0, +0x4 := +0x1FC, +0x226 |= 4
  - 0x80102fe8 lhu v0,550(s3) / 0x80102fec addiu v1,zero,240 / 0x80102ff0 andi v0,v0,0x2 / 0x80102ff4 beq v0,zero,0x8010304c / 0x80102ff8 sb v1,549(s3) / 0x80102ffc lw v1,508(s3) / 0x80103004 sh zero,342(s3) / 0x80103008 sw v1,4(s3) / 0x80103010 ori v0,v0,0x4 / 0x80103014 sh v0,550(s3)

## Aufrufe

- `0x80102614` -> `0x8008E1F4 (PsyQ RotMatrix)` (a0 = sp+16 (SVECTOR A) @0x801025F8 addiu a0,sp,16 ; a1 = sp+32 (MATRIX) @0x801025FC addiu s0,sp,32 + @0x80102608 addu a1,s0,zero. A.x=0 @0x8010260C ; A.y=[0x800CFC6E] = Spieler-Yaw (+0x76 der Spieler-Entity @0x800CFBF8) @0x80102604+@0x80102618 ; A.z=0 @0x80102610) — Rotationsmatrix aus dem Blickwinkel des SPIELERS bauen
- `0x80102630` -> `0x8008DBA4 (PsyQ ApplyMatrixSV)` (a0 = MATRIX sp+32 @0x8010261C ; a1 = SVECTOR sp+16 @0x80102620 ; a2 = sp+16 (in-place) @0x80102624 ; A.x := 4096 @0x80102628+@0x8010262C ; A.y := 0 @0x80102634) — Blickrichtung als (dirX@sp+16, dirY@sp+18, dirZ@sp+20)
- `0x801027B0 / 0x801027B8 / 0x80102800 / 0x80102818 / 0x80102820` -> `0x80015FE8 (RNG)` (keine Argumente; Ergebnisse @0x801027BC (rng1), v0 (rng2), @0x80102808 (rng3), @0x80102824 (rng4), v0 (rng5)) — Varianten- und Jitter-Wuerfe fuer die beiden Ebenen-Emissionen
- `0x801027EC` -> `0x8001BF10 (Partikel-/Effekt-Spawn)` (a0 = ((rng1&1)<<16)|(8096+(rng2&0xff)*8) @0x801027C0-@0x801027D4 ; a1 = 0 @0x801027D8 ; a2 = 0x8009DB44 @0x801027DC+@0x801027E0 ; a3 = sp+24 (vecB = Ebenenschnittpunkt) @0x801027E8+@0x801027F0) — Blutspritzer 1 am Schnittpunkt Blickstrahl x Ebene X=Gegner+0x38+3183
- `0x80102848` -> `0x8001BF10` (a0 = ((rng4&1)<<16)|(8096+(rng5&0xff)*4) @0x80102828-@0x8010283C ; a1 = 0 @0x80102840 ; a2 = 0x8009DB44 @0x80102844 ; a3 = sp+24 @0x8010284C) — Blutspritzer 2, vecB.x zuvor @0x801027FC um 500 versetzt
- `0x80102888` -> `0x800154AC (Winkel XZ, Ergebnis 0..4095)` (a0 = Entity+0x38 (X) @0x80102870 lh a0,56(s3) ; a1 = Entity+0x40 (Z) @0x80102874 lh a1,64(s3) ; a2 = (s16)[0x800CFC30] (Spieler-X) @0x8010287C ; a3 = (s16)[0x800CFC38] (Spieler-Z) @0x80102884) — Winkel Gegner->Spieler; Ergebnis s0 wird a1 der beiden Wund-Emissionen
- `0x801028C0` -> `0x80015FE8 (RNG)` (keine) — Variantenwurf fuer FX_ID_WOUND_1
- `0x801028F0` -> `0x8001BF10` (a0 = 8096+(rng&0xff)*8 @0x801028C8-@0x801028D0 ; a1 = (s16)s0 = Winkel zum Spieler @0x801028D4-@0x801028DC ; a2 = 0x8009DB44 @0x801028E0+@0x801028E4 ; a3 = sp+24 (Wundposition) @0x801028EC+@0x801028F4) — Wund-Blut 1, nur bei Koerpertreffer (s4==1)
- `0x80102904` -> `0x80015FE8 (RNG)` (keine) — Variantenwurf fuer FX_ID_WOUND_2
- `0x80102928` -> `0x8001BF10` (a0 = (8096+(rng&0xff)*8)|0x10000 @0x8010290C-@0x8010291C ; a1 = s0 (Winkel) @0x80102920 ; a2 = 0x8009DB44 @0x80102924 ; a3 = sp+24 @0x8010292C) — Wund-Blut 2
- `0x80102940` -> `0x8005BD6C (Gegner-SE)` (a0 = 8 @0x8010293C ; a1 = Entity @0x80102944. Bedingung +0x5 != 1 @0x80102930+@0x80102938) — Trefferlaut am Koerper
- `0x80102960` -> `0x8010221C (Blut-Emitter A)` (a0 = Entity @0x80102964. Bedingung +0x5 == 9 oder 17) — 2 Emissionen am Knochenanker Entity+0x198
- `0x80102978` -> `0x80101D9C (Blut-Emitter B)` (a0 = Entity @0x8010297C. Bedingung +0x5 == 10) — 8 Emissionen am Knochenanker
- `0x80102990` -> `0x80101FF0 (Blut-Emitter C)` (a0 = Entity @0x80102994. Bedingung +0x5 == 11) — 6 Emissionen am Knochenanker
- `0x801029A8` -> `0x801022D0 (Blut-Emitter D)` (a0 = Entity @0x801029AC. Bedingung +0x5 == 14) — 8 Emissionen mit absoluten Offsets an Entity+0x198+0x48 bzw. +0xF4
- `0x80102A08` -> `0x80100784 (Zustand-1-Wurzel)` (a0 = Entity @0x80102A00 ; a1 = s5 = Entity+0x108 (Anim-Datenzeiger, in der KI-Wurzel geladen @0x80100160 lw a1,264(s3)) @0x80102A04 ; a2 = s6 = Entity+0x17C (Anim-Tabellenzeiger @0x80100168 lw a2,380(s3)) @0x80102A0C) — Pfad +0x226&2: nach Wiederherstellung aus +0x1FC die Aktion im selben Frame weiterfahren
- `0x80102A70` -> `0x80100784` (a0 = Entity @0x80102A60 ; a1 = s5 @0x80102A64 ; a2 = s6 @0x80102A6C ; +0x4 := +0x1FC unmittelbar davor @0x80102A68+@0x80102A74) — Akku < 15: Treffer wird nur gezaehlt, Aktion laeuft weiter
- `0x80102A9C` -> `0x80102AD0 (Stagger-Executor)` (a0 = Entity @0x80102A94 bzw. @0x80102A00 ; a1 = s5 @0x80102A98 ; a2 = s6 @0x80102AA0) — Stagger ausfuehren
- `0x80102B30` -> `0x8005BD6C (Gegner-SE)` (a0 = 12 @0x80102B20 ; a1 = Entity @0x80102B24 ; Delay-Slot setzt +0x6 := 1 @0x80102B34) — Stagger-Startlaut
- `0x80102B4C` -> `0x8005BD6C` (a0 = 9 @0x80102B48 ; a1 = Entity @0x80102B50. Bedingung +0x14D == 15 @0x80102B38-@0x80102B40) — Stagger-Zwischenlaut
- `0x80102B60` -> `0x80015E7C (Pose/Root-Motion aus Clip +0x14C, Frame +0x14D)` (a0 = Entity @0x80102B54 ; a1 = s5 (Entity+0x108) @0x80102B58 ; a2 = s6 (Entity+0x17C) @0x80102B5C ; a3 = 0 @0x80102B64) — Wurzelbewegung des Stagger-Clips nach +0x144 legen
- `0x80102B74` -> `0x8002959C (Anim-Advance)` (a0 = Entity @0x80102B68 ; a1 = s5 @0x80102B6C ; a2 = s6 @0x80102B70 ; a3 = 128 @0x80102B78) — Clip 8 weiterschalten; Rueckgabe != 0 = Clip zu Ende -> zurueck in Zustand 1
- `0x80102B98` -> `0x800152C8 (Position entlang Yaw+a1 um +0x144 versetzen)` (a0 = Entity @0x80102B94 ; a1 = 0 @0x80102B9C. Wirkung: +0x38 += rot(+0x144).x @0x80015320, +0x40 += rot(+0x144).z @0x80015334) — Stagger-Rueckstoss in Weltkoordinaten anwenden
- `0x80102284 / 0x801022B0` -> `0x8001BF10 (im Emitter 0x8010221C)` (a0 = 0x05042710 bzw. 0x040F1000|((rng&0xff)<<2) ; a1 = (s16)Entity+0x76 @0x80102280 / @0x801022AC lh a1,118(s1) ; a2 = Entity+0x198 + 416 @0x80102274 ; a3 = sp+16 @0x8010225C / @0x801022B4) — Blutwolke am Knochenanker (Unterzustand 9 und 17)
- `0x80101E08 bis 0x80101FCC` -> `0x8001BF10 x8 (Emitter 0x80101D9C)` (a1 durchgehend (s16)Entity+0x76 (lh a1,118(s1) @0x80101E04/@0x80101E30/@0x80101E54/@0x80101E88/@0x80101ED8/@0x80101F28/@0x80101F78/@0x80101FC8) ; a2 = Entity+0x198 + 416 @0x80101DF8 ; a3 = sp+16 ; a0-Folge siehe Konstante EMIT_10_SEQ) — grosse Blutfontaene, Unterzustand 10
- `0x80102078 bis 0x801021F8` -> `0x8001BF10 x6 (Emitter 0x80101FF0)` (a1 = (s16)Entity+0x76 ; a2 = Entity+0x198 + 416 @0x8010206C ; a3 = sp+16 ; a0-Folge siehe EMIT_11_SEQ) — Blutfontaene, Unterzustand 11
- `0x80102364 bis 0x8010258C` -> `0x8001BF10 x8 (Emitter 0x801022D0)` (a1 = (s16)Entity+0x76 (lh a1,118(s3)) ; a2 = Entity+0x198 + 72 @0x80102360, ab @0x80102500+@0x80102508 = Entity+0x198 + 244 ; a3 = sp+16 (absolute Vektoren) ; a0-Folge siehe EMIT_14_SEQ) — Blutfontaene, Unterzustand 14

## Offen
- TOTE ZUWEISUNG im Original (byte-true nachbauen heisst: NICHTS tun): @0x8010270C und @0x80102740 schreiben -700 / -2000 nach sp+18. sp+18 ist SVECTOR-A.y (der rotierte Blickvektor), NICHT die Blutposition (die liegt in sp+24/26/28, gesetzt @0x801026C4, @0x801026D0, @0x801026D8). Rohbytes geprueft: 0x80102708 = 44 fd 02 24 ; 0x8010270C = 12 00 a2 a7 (sh v0,0x12(sp)) gegen 0x801026D0 = 1a 00 a3 a7 (sh v1,0x1a(sp)). Die hoehenabhaengige Blut-Y (+0x1D2 mod 3) hat im Auslieferungsstand also KEINE Wirkung.
- ZWEITE TOTE ZUWEISUNG: @0x801028F8-@0x80102908 liest lhu 16(sp), addiert 500 und schreibt nach 16(sp) — ebenfalls SVECTOR-A.x. Das Gegenstueck im s4==0-Pfad (@0x801027F4-@0x80102804) arbeitet korrekt auf 24(sp). Rohbytes: 0x801028F8 = 10 00 a2 97, 0x80102908 = 10 00 a2 a7.
- Bedeutung der Globalen 0x800CFB88 / 0x800CFB8C / 0x800CFB90 (Ankerposition der drei Knochen-Emitter) noch nicht belegt — nur ihre Verwendung als Minuend gegen part+0x1B4/+0x1B8/+0x1BC (part = Entity+0x198).
- Bedeutung des globalen Zeigers [0x800CFD90]+1992 (@0x801027A4), der bei +0x5 == 1 die Blut-Z ueberschreibt, noch nicht belegt.
- Konsument von +0x225 liegt in 0x80104EB4 (dekrementiert +0x225 @0x80104ED8-@0x80104EFC, setzt +0x223|=1 und ruft 0x80039514(a0=3,a1=100,a2=0) sowie 0x800395B8(a0=20,a1=100,a2=0,a3=3)); der Aufrufer von 0x80104EB4 ist noch nicht zurueckverfolgt.
- +0x14F (High-Byte des Clipworts 0x001F0008) wird im Overlay nirgends gelesen; ob die EXE es nutzt, ist nicht geprueft. +0x14E ist als Crossfade-Zaehler belegt (@0x800296A8 lbu t3,334(s2), @0x800299C0-@0x800299CC dekrementiert).
- Die Herkunft von s5 im EXE-Aufloeser (= der geschriebene Unterzustand +0x5, @0x80047324 / @0x80047574) ist nicht zurueckverfolgt — es steht damit noch nicht fest, welche Waffe/Trefferzone welche Reaktions-Id 0..18 erzeugt. Die Gewichtstabelle @0x801056B3 selbst ist vollstaendig gelesen.
- Der Guard-Wert 0x0C02 (Zustand 2 / Unterzustand 12) @0x8004726C stammt aus gemeinsamem EXE-Code; ob Unterzustand 12 fuer Typ 0x36 tatsaechlich erzeugt wird, ist nicht belegt (tab[12] = 5, und 12 ist im Blutblock ausgeschlossen).
- Die Zustand-1-Dispatch-Tabellen @0x8010564C (0x80100868, 0x80100D08, 0x80101108, 0x80101808, 0x80101AA0) und @0x80105660 (0x80100960, 0x80100D10, 0x80101110, 0x80101810, 0x80101AA8) fassen nur 5 Eintraege, waehrend Zustand 2 Unterzustaende bis 18 kennt; die Aufloesung laeuft ueber die Vorstufe 0x801007C4 und die Tabelle @0x80105648 (Index (+0x10E & 0x3F)). Dieser zweistufige Aufbau ist noch nicht vollstaendig zerlegt und gehoert zum Auftrag 'Zustand 1'.


---

# RE2 EM36 (Endgegner Typ 0x36) — Zustand 3 = TOD/KOLLAPS. Einsprung 0x80102BBC (reiner Trampolin-Wrapper), Rumpf 0x80102BDC..0x80103830. Phasen-Dispatch auf Entity+0x06 gegen Tabelle @0x80100054 (6 Eintraege, Schranke sltiu <6 @0x80102c0c). Datei: build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN, Ladebasis 0x80100000.

0x80102BBC ist nur ein Wrapper (addiu sp,-24 / jal 0x80102BDC / jr ra); der Rumpf liegt bei 0x80102BDC und wird vom Wurzel-Dispatcher mit a0=Entity, a1=Entity+0x108 (Skelett/EMR), a2=Entity+0x17C (Anim/EDD) gerufen. Er verzweigt ueber Entity+0x06 (Phase, NICHT +0x05) in 6 Phasen: p0 = Todes-Eintritt (Blutfontaenen, substate-abhaengige Gore-Emitter, HP-Entscheidung), p1 = Zusammenbruch mit interpoliertem Blutpfad, p2 = 250-Frame-Zucken + Explosion, p3 = halbtaktige Anim-Fortschaltung, p4 = Farb-Ausbleichen auf (48,48,64) + Absinken in den Boden ueber 2950 Einheiten, p5 = leerer Endzustand (identisch mit dem Epilog 0x8010380C, der auch der Default fuer +0x06>=6 ist). Die beiden HP-Stores sind das Herz der Funktion: @0x80103004 schreibt HP(+0x156)=0 und stellt das gesicherte Zustandswort aus +0x1FC nach +0x04 zurueck — das passiert NUR wenn +0x226 & 0x2 gesetzt ist (Treffer-/Taumel-Reaktion laeuft gerade, gesetzt von 0x80102AD0/0x80100D7C/0x80101834); zusaetzlich wird +0x226 |= 0x4 gesetzt (Tod vorgemerkt), das Zustand 1 @0x801007e0 spaeter als (+0x226 & 0x6)==0x4 auswertet und dann Entity+0x04 := 3 schreibt, also erneut in Zustand 3 eintritt. @0x80103060 schreibt HP(+0x156) = -1 (endgueltig tot/unverwundbar) — das ist der ECHTE Tod, wenn +0x226 & 0x2 klar ist; dort startet Motion 6, Phase:=1, SE 10, 7 Blut-ESPs (eine je Modellteil) und die Bildschirm-Blitzkette. Zustand 3 schreibt +0x05 NIE selbst; +0x05 ist der zum Todeszeitpunkt geerbte Unterzustand und waehlt nur, welcher Gore-Emitter (0x8010221C / 0x80101D9C / 0x80101FF0 / 0x801022D0) laeuft.

## Konstanten

| Name | Wert | Bedeutung | Adresse |
|---|---|---|---|
| PHASENTABELLE_Z3 | 0x80100054, 6 Eintraege: {0x80102C34, 0x801031C0, 0x8010340C, 0x80103634, 0x80103668, 0x8010380C} | Feld: Dispatch auf Entity+0x06 | @0x80102c24 (lw v0,84(at)); Schranke @0x80102c0c sltiu v0,v1,0x6 |
| ANIM_A3 | 128 | Frame: 4. Argument von anim_advance 0x8002959C (kein Rueckwaerts-/Spiegel-Bit) | @0x801031d0, @0x8010341c, @0x80103654 |
| UNITVEC_LEN | 4096 | Distanz: Laenge des mit der Spieler-Yaw rotierten Richtungsvektors | @0x80102c68 addiu v0,zero,4096 / @0x80102c6c sh v0,24(sp) |
| DX_SCHWELLE | -64 | Distanz: Blutfontaene A nur wenn rotierter Vektor.x < -64 | @0x80102c80 slti v0,a1,-64 |
| SUB_SKIP_16 | 16 | Feld: Entity+0x05 == 16 ueberspringt Fontaene A | @0x80102c90 / @0x80102c98 |
| SUB_SKIP_9_12 | 9..12 (a0-9 < 4) | Feld: Entity+0x05 in [9,12] ueberspringt Fontaene A | @0x80102c9c addiu v0,a0,-9 / @0x80102ca0 sltiu v0,v0,0x4 |
| X_OFFSET_MINUS | -3183 | Distanz: SpielerX - 3183 als Strahl-Ursprung | @0x80102cbc addiu v1,v1,-3183 |
| X_OFFSET_PLUS | +3183 | Distanz: Spawn-X = Entity+0x38 + 3183 | @0x80102d14 addiu v0,v0,3183 / @0x80102d18 sh v0,16(sp) |
| SPAWN_Y | -1400 | Distanz: feste Spawn-Hoehe der Fontaene A | @0x80102d0c addiu v1,zero,-1400 / @0x80102d10 sh v1,18(sp) |
| TOTER_CODE_SP26 | sp+26 := -700 wenn (Entity+0x1D2 % 3)==0 ; := -2000 wenn ==2 | Feld: TOTER CODE — sp+26 wird in p0 nie gelesen (a3 ist stets sp+16). Modulo-3 via Magic 0xAAAAAAAB | @0x80102d1c lbu v1,466(s3) / @0x80102d48 addiu v0,zero,-700 / @0x80102d4c sh v0,26(sp) / @0x80102d7c addiu v0,zero,-2000 / @0x80102d80 sh v0,26(sp) |
| Z_CLAMP_MAX | -20500 (Test slti < -20499) | Distanz: obere Klemme des projizierten Z | @0x80102da0 slti v0,v0,-20499 / @0x80102da8 addiu v0,zero,-20500 / @0x80102dac sh v0,20(sp) |
| Z_CLAMP_MIN | -26000 | Distanz: untere Klemme des projizierten Z | @0x80102db8 slti v0,v0,-26000 / @0x80102dc0 addiu v0,zero,-26000 / @0x80102dc4 sh v0,20(sp) |
| SUB1_Z_QUELLE | *(s32*)(*(u32*)0x800cfd90 + 1992)   (1992 = 0x7C8) | Feld: Z-Ersatz wenn Entity+0x05 == 1; 0x800cfd90 = Spieler+0x198 (Teil-Array-Zeiger) | @0x80102ddc lw v0,-624(v0) / @0x80102de4 lw v0,1992(v0) |
| ESP_BASIS_8096 | 8096 (0x1FA0) | Feld: Basiswert des ESP-Parameters (a0 low16) | @0x80102e10, @0x80102e74, @0x80102f0c, @0x80102f4c, @0x801030b4, @0x80103368 |
| ESP_STREUUNG | + (rng()&0xff) << 3   bzw.  << 2 | Feld: Streuung des ESP-Parameters; <<3 an @0x80102e0c/@0x80102f08/@0x80103364, <<2 an @0x80102e70/@0x801030b0 | @0x80102e0c, @0x80102e70, @0x80102f08, @0x801030b0, @0x80103364 |
| ESP_UNTERTYP_BIT | ((rng() & 1) << 16)  bzw. fest | 0x10000 | Feld: ESP-Untertyp (a0 Bit 16); fest gesetzt nur beim 2. Sprite der Fontaene B | @0x80102e00/@0x80102e04, @0x80102e64/@0x80102e68, @0x801030a4/@0x801030a8, @0x80103358/@0x8010335c; fest @0x80102f50 lui a0,0x1 / @0x80102f54 |
| SPAWN_X_SCHRITT | +500 | Distanz: X der Fontaene A vor dem 2. ESP | @0x80102e38 addiu v0,v0,500 / @0x80102e40 sh v0,16(sp) |
| SPAWN_Z_JITTER | Z -= ((rng()&0xff) - 128) | Distanz: Z-Jitter vor dem 2. ESP der Fontaene A | @0x80102e4c addiu v0,v0,-128 / @0x80102e50 subu v1,v1,v0 / @0x80102e58 sh v1,20(sp) |
| FENSTER_Z | ((Z + 23999) & 0xFFFF) < 1499 (0x5DB) | Distanz: Z im Bereich [-23999,-22501] schaltet Fontaene B frei | @0x80102e94 addiu v0,v0,23999 / @0x80102e9c sltiu v0,v0,0x5db / @0x80102ea8 addiu s2,zero,1 |
| FONTAENE_B_X | (s16)(Entity+0xA4) + 500 | Distanz: Spawn-X der Fontaene B | @0x80102edc lw v0,164(s3) / @0x80102ee4 addiu v0,v0,500 / @0x80102ee8 sh v0,16(sp) |
| FONTAENE_B_Y | (s16)(Entity+0xA8) | Distanz: Spawn-Y der Fontaene B | @0x80102eec lw v0,168(s3) / @0x80102ef4 sh v0,18(sp) |
| FONTAENE_B_Z | (s16)(Entity+0xAC) | Distanz: Spawn-Z der Fontaene B | @0x80102ef8 lw v0,172(s3) / @0x80102f00 sh v0,20(sp) |
| QUIRK_SP24 | sp+24 += 500 (statt sp+16) | Feld: ORIGINAL-BUG — der 2. ESP der Fontaene B benutzt a3=sp+16, das unveraendert bleibt; beide Sprites sitzen an derselben Stelle. Byte-true nachbauen | @0x80102f30 lhu v0,24(sp) / @0x80102f38 addiu v0,v0,500 / @0x80102f40 sh v0,24(sp) — Verbraucher @0x80102f64 addiu a3,sp,16 |
| SE_TREFFER_TOD | 8 | Feld: Gegner-SE-Id (nur wenn Entity+0x05 != 1) | @0x80102f74 addiu a0,zero,8 |
| FLAG_226_BIT2 | 0x0002 | Feld: Taumel-/Trefferreaktion aktiv (gesetzt @0x80102af8, @0x80100d84, @0x8010183c; geloescht @0x80102b8c, @0x80101040, @0x80101970) | @0x80102ff0 andi v0,v0,0x2 / @0x80102ff4 beq v0,zero,0x8010304c |
| FLASH_PULSE_TOD | 240 | Dauer: Entity+0x225 = 240 Rot-Blitz-Pulse (Verbraucher 0x80104EB4); steht im Delay-Slot, laeuft in BEIDEN Zweigen | @0x80102fec addiu v1,zero,240 / @0x80102ff8 sb v1,549(s3) |
| HP_STORE_1 | Entity+0x156 := 0 | Feld: HP-Store #1 — nur im aufgeschobenen Tod (+0x226 & 2 gesetzt) | @0x80103004 sh zero,342(s3) |
| ZUSTAND_RESTORE | Entity+0x04 (32 bit) := *(u32*)(Entity+0x1FC) | Feld: stellt {Zustand,Unterzustand,Phase,+0x07} vor dem Treffer wieder her | @0x80102ffc lw v1,508(s3) / @0x80103008 sw v1,4(s3) |
| FLAG_226_BIT4 | |= 0x0004 | Feld: 'Tod vorgemerkt'; von Zustand 1 @0x801007e8 als (x&6)==4 ausgewertet | @0x80103010 ori v0,v0,0x4 / @0x80103014 sh v0,550(s3) |
| HP_STORE_2 | Entity+0x156 := -1 | Feld: HP-Store #2 — ECHTER Tod, macht den Boss unverwundbar (gleicher Wert wie Typ 0x37 @0x80100530) | @0x8010305c addiu v0,zero,-1 / @0x80103060 sh v0,342(s3) |
| MOTION_TOD_6 | Entity+0x14C := 0x001F0006  (Motion 6, Frame 0, Crossfade 31) | Clip: Todes-Startclip | @0x8010304c lui v1,0x1f / @0x80103050 ori v1,v1,0x6 / @0x8010306c sw v1,332(s3) |
| PHASE_1 | Entity+0x06 := 1 | Feld: Phasenwechsel im echten Tod | @0x80103070 addiu v1,zero,1 / @0x80103074 sb v1,6(s3) |
| SE_TOD | 10 | Feld: Gegner-SE-Id beim echten Tod | @0x80103054 addiu a0,zero,10 / jal @0x8010307c |
| FLAGS_1D3_1D4 | Entity+0x1D4 |= 0x0002 ; Entity+0x1D3 |= 0x80 | Feld: beide beim echten Tod gesetzt | @0x80103078 ori v0,v0,0x2 / @0x80103080 sh v0,468(s3) ; @0x8010308c ori v0,v0,0x80 / @0x80103090 sb v0,467(s3) |
| BLUT_TEILE_ANZAHL | 7 | Feld: 7 Blut-ESPs, je einer an der Matrix von Teil 0..6 | @0x80103068 addiu s1,zero,7 / Schleife @0x80103094..@0x801030d0 |
| TEIL_STRIDE | 172 (0xAC) | Feld: Groesse eines Modellteil-Eintrags in Entity+0x198 | @0x801030cc addiu s2,s2,172 |
| TEIL_MATRIX_OFF | +72 (0x48) | Feld: MATRIX (32 Byte) des Modellteils; t[0..2] liegen bei +0x5C/+0x60/+0x64 | @0x801030c0 addiu a2,s2,72 |
| F_218 | Entity+0x218 := 5, Entity+0x219 := 0 | Feld: Sub-Aktions-Paar beim echten Tod | @0x801030e0 addiu v0,zero,5 / @0x801030e4 sb v0,536(s3) / @0x801030e8 sb zero,537(s3) |
| CURSOR_16B | Entity+0x16B := 0 | Feld: Blutspur-Cursor (Hi-Nibble = Segment 0..2, Lo-Nibble = Teilschritt 0..15) | @0x801030ec sb zero,363(s3) |
| FLAG_1C0_BIT1 | Entity+0x1C0 |= 0x02 | Feld: gesetzt beim echten Tod | @0x801030f0 ori v1,v1,0x2 / @0x801030f8 sb v1,448(s3) |
| SE_KOLLAPS | 13 | Feld: Gegner-SE-Id, a0 im Delay-Slot @0x801030d4 gesetzt | @0x801030d4 addiu a0,zero,13 / jal @0x801030f4 |
| BLITZKETTE_P0 | 0x80039514(250,180,0); dann 0x800395b8 mit (150,180,0,250)(10,180,220,20)(10,220,180,30)(10,180,220,40)(10,220,180,50)(10,180,220,65)(10,220,180,75)(10,180,220,230)(10,220,180,240) | Dauer/Farbe: Bildschirm-Blitzkette beim echten Tod | @0x801030fc, @0x8010310c, @0x80103120, @0x80103134, @0x80103148, @0x8010315c, @0x80103170, @0x80103184, @0x80103198, @0x801031ac |
| MOTION_KOLLAPS_7 | Entity+0x14C := 0x001F0007 | Clip: Kollaps-Clip, gesetzt am Ende von p1 | @0x801031d8 lui v0,0x1f / @0x801031dc ori v0,v0,0x7 / @0x801031e4 sw v0,332(s3) |
| PHASE_2 | Entity+0x06 := 2 | Feld | @0x801031e0 addiu v1,zero,2 / @0x801031ec sb v1,6(s3) |
| TIMER_P2 | Entity+0x158 := 250 | Dauer: 250 Frames Zucken; Explosion im 251. p2-Frame, wenn der Zaehler 0 erreicht | @0x801031e8 addiu v0,zero,250 / @0x801031f0 sh v0,344(s3) |
| BLUTPFAD_TABELLE | 0x801056C8, 4 Eintraege a 8 Byte {s16 x,y,z,pad}: [0]=(1903,-1046,-2310) [1]=(1703,-2646,-750) [2]=(2393,-546,1410) [3]=(1443,-2266,1410) | Distanz: lokale Emitter-Wegpunkte relativ zur Matrix von Teil[2] | Lesezugriffe @0x8010321c/@0x80103230/@0x80103244/@0x80103258/@0x80103264/@0x8010327c/@0x80103288/@0x801032a0/@0x801032ac; Stride 8 @0x80103210 sll a2,a0,3 |
| SEGMENT_LIMIT | 3 | Feld: Blutspur emittiert nur solange ((s8)+0x16B)>>4 < 3 | @0x80103204 slti v0,a0,3 |
| INTERPOL_RUNDUNG | d < 0 ? d+15 : d, dann >>4 (arithmetisch) | Distanz: Segment-Delta pro Teilschritt | @0x801032c0 addiu v1,v1,15 / @0x801032c8 sra v1,v1,4 (analog @0x801032e8/@0x801032f0, @0x80103310/@0x80103318) |
| INTERPOL_FAKTOR | (Entity+0x16B) & 0x0F | Feld: Teilschritt 0..15 innerhalb des Segments | @0x801032cc andi v0,v0,0xf (auch @0x801032f4, @0x8010331c) |
| TEIL2_MATRIX | Entity+0x198 + 416 | Feld: Matrix von Teil[2] (416 = 2*172 + 72) | @0x8010337c addiu a2,s1,416 |
| CURSOR_RESET_FRAME | Entity+0x14D == 60 | Frame: Anim-Frame, bei dem der Blutspur-Cursor auf 0 zurueckgesetzt wird | @0x80103394 addiu v0,zero,60 / @0x801033a0 sb zero,363(s3) |
| BEGLEITER_TAKT | (Entity+0x14D & 3) == 0 | Frame: nur jeder 4. Anim-Frame darf einen Begleiter kommandieren | @0x801033ac andi v0,v0,0x3 (p1) / @0x80103578 andi v0,v0,0x3 (p2) |
| BEGLEITER_MASKE | Entity+0x228, Bits i und i+4 (i=0..3) | Feld: Kandidat i ist gueltig, wenn beide Bits 0 sind | @0x801033c4 lbu a0,552(s3) / @0x801033cc srav v0,a0,v1 / @0x801033d8 addiu v0,v1,4 / @0x801033dc srav v0,a0,v0 ; Kopie @0x80103590/@0x80103598/@0x801035a8 |
| BEGLEITER_SCHLEIFE | 4 Iterationen | Feld: 4 Begleiter Typ 0x37 | @0x801033b4 addiu s1,zero,4 (p1) / @0x80103580 addiu s1,zero,4 (p2) |
| BEGLEITER_ZUSTANDSWORT | 3585 = 0x00000E01 → Zustand 1, Unterzustand 14, Phase 0 | Feld: nach [0x800cfe20 + idx*4] -> +0x04 geschrieben | @0x8010360c addiu a1,zero,3585 ; Schreiber @0x80104eb0 sw a1,4(v0) |
| BEGLEITER_EFFEKTE | 0x8003947c(3,0) und 0x80039514(15,250,0) | Dauer: nach dem Begleiter-Kommando | @0x80103610/@0x80103618 bzw. @0x8010361c/@0x80103620/@0x80103628 |
| MOTION_ZUCK_6 | Entity+0x14C := 0x001F0006 | Clip: Standard-Neuanstoss in p2 | @0x80103424 lui v0,0x1f / @0x80103428 ori v0,v0,0x6 / @0x80103430 sw v0,332(s3) |
| MOTION_ZUCK_8 | Entity+0x14C := 0x001F0008 bei (rng()&3)==0 | Clip: 1-von-4-Variante in p2 | @0x80103434 andi v0,v0,0x3 / @0x8010343c lui v0,0x1f / @0x80103440 ori v0,v0,0x8 / @0x80103444 sw v0,332(s3) |
| PHASE_3 | Entity+0x06 := 3 | Feld | @0x8010346c addiu v1,zero,3 / @0x80103470 sb v1,6(s3) |
| FLAG_226_BIT3 | Entity+0x226 |= 0x0008 | Feld: 'Explosion erreicht'; Wurzel @0x801001a4 testet Bit 3 und ueberspringt dann den Zufalls-Wobble-Treiber | @0x80103474 ori v0,v0,0x8 / @0x8010347c sh v0,550(s3) |
| WOBBLE_AUS | 0x8010503C(0,0,0) und 0x8010503C(1,0,0) | Feld: nullt Byte +2 und +3 der 28-Byte-Saetze 0 und 1 der Tabelle @0x80105610 | @0x80103478 (a0=0 @0x8010345c, a1=0 @0x80103460, a2=0 @0x80103464) und @0x80103488 (a0=1 @0x80103480, a1=0 @0x80103484, a2=0 @0x8010348c) |
| MOTION_EXPLOSION_10 | Entity+0x14C := 0x001F000A | Clip: Explosions-/Endclip | @0x80103490 lui t0,0x1f / @0x80103494 ori t0,t0,0xa / @0x801034c8 sw t0,332(s3) |
| TEIL2_SKALA | Teil[2]+0x9C = +0x9E = +0xA0 := 4096 | Feld: Skala-SVECTOR (Absolutoffsets 484/486/488 ab Entity+0x198) | @0x801034ac addiu v0,zero,4096 / @0x801034b0 / @0x801034b4 / @0x801034b8 |
| TEIL2_FLAG_400 | Teil[2]+0x00 |= 0x400 | Feld: Renderflag (Absolutoffset 344 ab Entity+0x198) | @0x801034a8 lw a3,344(v1) / @0x801034bc ori a3,a3,0x400 / @0x801034c0 sw a3,344(v1) |
| EXPLOSION_BLITZ | 0x80039514(5,200,0) ; 0x800395b8(200,200,0,5) | Dauer/Farbe: Bildschirmblitz der Explosion | @0x801034c4 (a0=5 @0x80103498, a1=200 @0x8010349c, a2=0 @0x801034a4) / @0x801034d8 (a0=200 @0x801034cc, a1=200 @0x801034d0, a2=0 @0x801034d4, a3=5 @0x801034dc) |
| EXPLOSION_SHAKE | 0x8003947c-Paare (a0,a1): (3,0)(3,5)(2,15)(3,60)(3,65)(2,75)(3,90)(3,95)(2,105)(2,110)(2,115)(1,120) | Dauer: 12 zeitversetzte Ruettel-/Effekt-Auftraege | @0x801034e4 @0x801034f0 @0x801034fc @0x80103508 @0x80103514 @0x80103520 @0x8010352c @0x80103538 @0x80103544 @0x80103550 @0x8010355c @0x80103568 |
| P3_HALBTAKT | (Entity+0x158 & 1) == 0 | Frame: Anim wird in p3 nur jeden zweiten Frame fortgeschaltet (beim Eintritt steht +0x158 = 0xFFFF) | @0x8010363c andi v0,v0,0x1 / @0x80103640 bne v0,zero,0x80103668 |
| P4_TIMER_INC | Entity+0x158 += 1 | Frame: p4-Taktzaehler; Farbabbau nur bei ungeradem Wert, Absinken jeden Frame | @0x80103670 addiu v0,v0,1 / @0x80103674 sh v0,344(s3) / @0x80103678 andi v0,v0,0x1 |
| FARBZIEL_R | 48 (0x30) | Farbe: R zaehlt um 1 pro Takt bis 48 | @0x80103680 addiu t0,zero,48 / @0x80103698 beq v0,t0 / @0x8010369c addiu v0,v1,-1 / @0x801036f0 beq v0,t0 |
| FARBZIEL_G | 48 (Wortmaske 0x3000) | Farbe: G zaehlt um 0x100 pro Takt bis 0x3000 | @0x801036a8 addiu a3,zero,12288 / @0x801036b0 beq v0,a3 / @0x801036b4 addiu v0,v1,-256 |
| FARBZIEL_B | 64 (Wortmaske 0x400000) | Farbe: B zaehlt um 0x10000 pro Takt bis 0x400000 | @0x801036c4 lui a1,0x40 / @0x801036cc beq v0,a1 / @0x801036d0 lui v0,0xffff / @0x801036d4 addu v0,v1,v0 |
| FARBFELDER | Teil[0]+0x70 (Absolutoffset 112) und Teil[2]+0x70 (Absolutoffset 456) | Farbe: gepacktes 0x00BBGGRR | @0x8010368c lw v1,112(s2) / @0x801036e4 lw a0,456(v1) |
| SINK_SCHRITT | t < 1025 → +16 ; 1025<=t<2049 → +12 ; 2049<=t<3073 → +8 ; t>=3073 → +4 | Frame: Schrittweite von t = *(s16*)(*(u32*)(Teil[2]+0x88) + 0x14) | @0x8010373c addiu s0,zero,16 / @0x80103748 slti v0,a0,3073 / @0x80103758 addiu s0,zero,4 / @0x8010375c slti v0,a0,2049 / @0x8010376c addiu s0,zero,8 / @0x80103764 slti v0,a0,1025 / @0x80103778 addiu s0,zero,12 ; Addition @0x8010378c/@0x80103790 |
| SINK_ENDE | t >= 4097 → Entity+0x06 := 5 | Frame: Ende des Absinkens | @0x801037a4 slti v0,v0,4097 / @0x801037ac addiu v0,zero,5 / @0x801037b0 sb v0,6(s3) |
| SINK_FAKTOR | K = (2950 * t + (Zwischenwert<0 ? 4095 : 0)) >> 12 | Distanz: Absinkhoehe; 2950 als Shift-Kette 2t,3t,24t,23t,368t,369t,1476t,1475t,2950t | @0x801037c4..@0x801037e4 (sll/addu/subu-Kette) / @0x801037f0 addiu v0,v0,4095 / @0x801037f4 sra v0,v0,12 |
| SINK_BASIS | 4500 | Distanz: Teil[2]+0x30 := 4500 - K ; Entity+0x3C (Y) := K | @0x801037f8 addiu v1,zero,4500 / @0x801037fc subu v0,v1,v0 / @0x80103800 subu v1,v1,v0 / @0x80103804 sw v0,48(s2) / @0x80103808 sw v1,60(s3) |
| IDENTITAETSMATRIX | 0x8009DB44 = {m00=4096, m11=4096, m22=4096, t={0,0,0}} | Feld: a2 fuer ESP-Spawns, deren Weltposition komplett in a3 steht | @0x80102e1c lui s1,0x800a / @0x80102e20 addiu s1,s1,-9404 ; Bytes @0x8009db44: 00 10 00 00 00 00 00 00 00 10 00 00 ... |
| P5_ENDZUSTAND | 0x8010380C | Feld: Phase 5 = reiner Epilog (lw ra/s5/s4/s3/s2/s1/s0, addiu sp,104, jr ra) — kein Verhalten mehr | @0x8010380c..@0x80103830 |
| ENTITY_FELDER_Z3 | +0x38 X(s32) +0x3C Y(s32) +0x40 Z(s32) +0x76 Yaw +0x14C Motion +0x14D Animframe +0x156 HP +0x158 Timer(u16) +0x16B Blutcursor +0x198 Teil-Array +0x1C0 +0x1D2 +0x1D3 +0x1D4 +0x1FC gesich.Zustandswort +0x218/+0x219 +0x225 Blitzpulse +0x226 Flags(u16) +0x228 Begleitermaske | Feld: alle in Zustand 3 beruehrten Offsets | Belege ueber die gesamte Funktion @0x80102bdc..@0x80103830 |

## Uebergaenge

- **z3 (beliebige Phase) -> Epilog 0x8010380C (return, kein Effekt)** wenn Entity+0x06 >= 6
  - 0x80102c04 lbu v1,6(s3) | 0x80102c0c sltiu v0,v1,0x6 | 0x80102c10 beq v0,zero,0x8010380c
- **z3 Dispatch -> p0=0x80102C34 p1=0x801031C0 p2=0x8010340C p3=0x80103634 p4=0x80103668 p5=0x8010380C** wenn Tabelle @0x80100054 [Entity+0x06]
  - 0x80102c18 sll v0,v1,2 | 0x80102c1c lui at,0x8010 | 0x80102c20 addu at,at,v0 | 0x80102c24 lw v0,84(at) | 0x80102c2c jr v0
- **z3 p0 -> Gore-Emitter 0x8010221C** wenn Entity+0x05 == 9 ODER == 17
  - 0x80102f80 lbu v1,5(s3) | 0x80102f84 addiu v0,zero,9 | 0x80102f88 beq v1,v0,0x80102f98 | 0x80102f8c addiu v0,zero,17 | 0x80102f90 bne v1,v0,0x80102fa8 | 0x80102f98 jal 0x8010221c
- **z3 p0 -> Gore-Emitter 0x80101D9C** wenn Entity+0x05 == 10
  - 0x80102fa0 lbu v1,5(s3) | 0x80102fa4 addiu v0,zero,10 | 0x80102fa8 bne v1,v0,0x80102fb8 | 0x80102fb0 jal 0x80101d9c
- **z3 p0 -> Gore-Emitter 0x80101FF0** wenn Entity+0x05 == 11
  - 0x80102fb8 lbu v1,5(s3) | 0x80102fbc addiu v0,zero,11 | 0x80102fc0 bne v1,v0,0x80102fd8 | 0x80102fc8 jal 0x80101ff0
- **z3 p0 -> Gore-Emitter 0x801022D0** wenn Entity+0x05 == 14
  - 0x80102fd0 lbu v1,5(s3) | 0x80102fd4 addiu v0,zero,14 | 0x80102fd8 bne v1,v0,0x80102fe8 | 0x80102fe0 jal 0x801022d0
- **z3 p0 -> AUFGESCHOBENER TOD: Entity+0x04 := *(u32*)(Entity+0x1FC), HP(+0x156):=0, +0x226 |= 0x4** wenn (Entity+0x226 & 0x2) != 0  (Treffer-/Taumel-Reaktion laeuft)
  - 0x80102fe8 lhu v0,550(s3) | 0x80102ff0 andi v0,v0,0x2 | 0x80102ff4 beq v0,zero,0x8010304c | 0x80102ffc lw v1,508(s3) | 0x80103004 sh zero,342(s3) | 0x80103008 sw v1,4(s3) | 0x80103010 ori v0,v0,0x4 | 0x80103014 sh v0,550(s3)
- **z3 p0 (aufgeschobener Tod) -> sofortiger Tail-Call Zustand 1 (0x80100784) im selben Frame** wenn restauriertes Entity+0x04 == 1
  - 0x8010300c lbu v1,4(s3) | 0x80103018 addiu v0,zero,1 | 0x8010301c bne v1,v0,0x80103038 | 0x80103028 jal 0x80100784 (a0=s3@0x80103020, a1=s4@0x80103024, a2=s5@0x8010302c) | 0x80103030 j 0x8010380c
- **z3 p0 (aufgeschobener Tod) -> sofortiger Tail-Call Taumel-Maschine 0x80102AD0 im selben Frame** wenn restauriertes Entity+0x04 != 1
  - 0x8010301c bne v1,v0,0x80103038 | 0x8010303c jal 0x80102ad0 (a0=s3, a1=s4@0x80103038, a2=s5@0x80103040) | 0x80103044 j 0x8010380c
- **z1 (0x80100784) -> z3 p0 (Entity+0x04 := 3, damit +0x05=0/+0x06=0)** wenn (Entity+0x226 & 0x6) == 0x4  — Tod vorgemerkt UND Taumel beendet
  - 0x801007e0 lhu v0,550(s0) | 0x801007e4 addiu v1,zero,4 | 0x801007e8 andi v0,v0,0x6 | 0x801007ec bne v0,v1,0x80100800 | 0x801007f4 addiu v0,zero,3 | 0x801007fc sw v0,4(s0)
- **z3 p0 -> z3 p1 (ECHTER TOD): HP(+0x156) := -1, Motion 6, Phase:=1** wenn (Entity+0x226 & 0x2) == 0
  - 0x80102ff4 beq v0,zero,0x8010304c | 0x8010304c lui v1,0x1f | 0x80103050 ori v1,v1,0x6 | 0x8010305c addiu v0,zero,-1 | 0x80103060 sh v0,342(s3) | 0x8010306c sw v1,332(s3) | 0x80103070 addiu v1,zero,1 | 0x80103074 sb v1,6(s3)
- **z3 p0 (echter Tod) -> faellt OHNE Sprung in den p1-Rumpf 0x801031C0 (Anim-Advance laeuft schon im selben Frame)** wenn immer (kein j/b nach 0x801031b8)
  - 0x801031b8 jal 0x800395b8 | 0x801031bc addiu a3,zero,240 | 0x801031c0 addu a0,s3,zero (= p1-Einsprung)
- **z3 p1 -> z3 p2: Motion 7, Phase:=2, Timer +0x158 := 250** wenn anim_advance(0x8002959C) liefert v0 != 0 (Clip umgeschlagen)
  - 0x801031cc jal 0x8002959c | 0x801031d4 beq v0,zero,0x801031f4 | 0x801031dc ori v0,v0,0x7 | 0x801031e0 addiu v1,zero,2 | 0x801031e4 sw v0,332(s3) | 0x801031e8 addiu v0,zero,250 | 0x801031ec sb v1,6(s3) | 0x801031f0 sh v0,344(s3)
- **z3 p1 -> Blutpfad-Cursor +0x16B := 0 (Neustart der 3-Segment-Spur)** wenn Anim-Frame Entity+0x14D == 60
  - 0x80103390 lbu v1,333(s3) | 0x80103394 addiu v0,zero,60 | 0x80103398 bne v1,v0,0x801033a4 | 0x801033a0 sb zero,363(s3)
- **z3 p1 / z3 p2 -> Begleiter-Kommando (gemeinsamer Schwanz 0x801035D4)** wenn (Entity+0x14D & 3) == 0
  - p1: 0x801033a4 lbu v0,333(s3) | 0x801033ac andi v0,v0,0x3 | 0x801033b0 bne v0,zero,0x8010380c   —   p2: 0x80103570 lbu v0,333(s3) | 0x80103578 andi v0,v0,0x3 | 0x8010357c bne v0,zero,0x8010380c
- **gemeinsamer Schwanz 0x801035D4 -> zufaellig gewaehlter Begleiter (Typ 0x37) Entity+0x04 := 0x00000E01 (Zustand 1, Unterzustand 14)** wenn Kandidatenzahl s0 != 0; Kandidat i gilt, wenn ((Entity+0x228 >> i) & 1)==0 UND ((Entity+0x228 >> (i+4)) & 1)==0, i=0..3
  - 0x801033c4 lbu a0,552(s3) | 0x801033cc srav v0,a0,v1 | 0x801033d0 andi v0,v0,0x1 | 0x801033dc srav v0,a0,v0 (v0=v1+4) | 0x801033f0 sb v1,0(v0) | 0x801035d4 beq s0,zero,0x8010380c | 0x801035e8 divu v0,s0 | 0x801035f8 mfhi v1 | 0x80103604 lbu a0,64(v0) | 0x80103608 jal 0x80104e9c | 0x8010360c addiu a1,zero,3585
- **z3 p2 -> Motion 6 neu setzen (Zuck-Schleife)** wenn anim_advance liefert v0 != 0
  - 0x80103418 jal 0x8002959c | 0x80103420 beq v0,zero,0x80103448 | 0x80103424 lui v0,0x1f | 0x80103428 ori v0,v0,0x6 | 0x80103430 sw v0,332(s3)
- **z3 p2 -> stattdessen Motion 8** wenn anim_advance != 0 UND (rng() & 3) == 0
  - 0x8010342c jal 0x80015fe8 | 0x80103434 andi v0,v0,0x3 | 0x80103438 bne v0,zero,0x80103448 | 0x8010343c lui v0,0x1f | 0x80103440 ori v0,v0,0x8 | 0x80103444 sw v0,332(s3)
- **z3 p2 -> z3 p3 + EXPLOSION: Phase:=3, +0x226 |= 0x8, Motion 10, Teil[2].flags |= 0x400, Teil[2]-Skala 4096/4096/4096** wenn Timer Entity+0x158 == 0 (Zaehler laeuft 250 -> 0; die Speicherung von v1-1 im Delay-Slot erfolgt immer)
  - 0x80103448 lhu v1,344(s3) | 0x80103450 addiu v0,v1,-1 | 0x80103454 bne v1,zero,0x80103570 | 0x80103458 sh v0,344(s3) | 0x8010346c addiu v1,zero,3 | 0x80103470 sb v1,6(s3) | 0x80103474 ori v0,v0,0x8 | 0x8010347c sh v0,550(s3) | 0x801034b0-b8 sh 4096,484/486/488(v1) | 0x801034bc ori a3,a3,0x400 | 0x801034c0 sw a3,344(v1) | 0x801034c8 sw t0,332(s3) (0x001f000a)
- **z3 p3 -> Anim fortschalten (halbe Rate)** wenn (Entity+0x158 & 1) == 0
  - 0x80103634 lhu v0,344(s3) | 0x8010363c andi v0,v0,0x1 | 0x80103640 bne v0,zero,0x80103668 | 0x80103650 jal 0x8002959c | 0x80103654 addiu a3,zero,128
- **z3 p3 -> z3 p4 (Entity+0x06 += Rueckgabe von anim_advance)** wenn anim_advance liefert 1
  - 0x80103658 lbu v1,6(s3) | 0x80103660 addu v1,v1,v0 | 0x80103664 sb v1,6(s3)
- **z3 p3 -> faellt OHNE Sprung in p4-Rumpf 0x80103668** wenn immer
  - 0x80103664 sb v1,6(s3) | 0x80103668 lhu v0,344(s3) (= p4-Einsprung)
- **z3 p4 -> Farbabbau der Teile 0 und 2 auf (R,G,B)=(48,48,64)** wenn ((Entity+0x158)+1 & 1) != 0, d.h. nur bei ungeradem neuen Timerwert
  - 0x80103670 addiu v0,v0,1 | 0x80103674 sh v0,344(s3) | 0x80103678 andi v0,v0,0x1 | 0x8010367c beq v0,zero,0x80103730 | 0x80103698 beq v0,t0(48) | 0x801036b0 beq v0,a3(12288) | 0x801036cc beq v0,a1(0x400000)
- **z3 p4 -> z3 p5 (Endzustand, Handler tut nichts)** wenn *(s16*)(*(u32*)(Teil[2]+0x88) + 0x14) >= 4097
  - 0x80103794 lw v0,136(s2) | 0x8010379c lh v0,20(v0) | 0x801037a4 slti v0,v0,4097 | 0x801037a8 bne v0,zero,0x801037b4 | 0x801037ac addiu v0,zero,5 | 0x801037b0 sb v0,6(s3)
- **z3 p0 Blutfontaene A -> Emission (2 ESPs an einer aus der Spielerblickrichtung projizierten Position)** wenn rotierter Blickvektor.x < -64 UND Entity+0x05 != 16 UND Entity+0x05 NICHT in [9..12]
  - 0x80102c80 slti v0,a1,-64 | 0x80102c84 beq v0,zero,0x80102eac | 0x80102c90 addiu v0,zero,16 | 0x80102c98 beq v1,v0,0x80102eac | 0x80102c9c addiu v0,a0,-9 | 0x80102ca0 sltiu v0,v0,0x4 | 0x80102ca4 bne v0,zero,0x80102eac
- **z3 p0 Blutfontaene B -> 2 weitere ESPs an Entity+0xA4/+0xA8/+0xAC mit Spielerwinkel; danach SE 8** wenn s2==1, d.h. ((berechnetes Z + 23999) & 0xFFFF) < 0x5DB (1499); SE 8 nur wenn Entity+0x05 != 1
  - 0x80102e94 addiu v0,v0,23999 | 0x80102e98 andi v0,v0,0xffff | 0x80102e9c sltiu v0,v0,0x5db | 0x80102ea0 beq v0,zero,0x80102eac | 0x80102ea8 addiu s2,zero,1 | 0x80102ed4 bne s2,v0,0x80102f80 | 0x80102f70 beq v0,s2,0x80102f80 | 0x80102f78 jal 0x8005bd6c
- **z3 p0 Ziel-Z-Sonderfall -> berechnetes Z := *(s32*)(*(u32*)(Spieler+0x198) + 0x7C8)** wenn Entity+0x05 == 1
  - 0x80102dc8 lbu v1,5(s3) | 0x80102dcc addiu v0,zero,1 | 0x80102dd0 bne v1,v0,0x80102df0 | 0x80102ddc lw v0,-624(v0) [0x800cfd90] | 0x80102de4 lw v0,1992(v0) | 0x80102dec sh v0,20(sp)
- **z3 p1 Blutspur -> 1 ESP pro Frame an interpolierter Position aus Tabelle 0x801056C8** wenn ((s8)(Entity+0x16B)) >> 4  < 3
  - 0x801031f4 lbu v0,363(s3) | 0x801031fc sll v0,v0,24 | 0x80103200 sra a0,v0,28 | 0x80103204 slti v0,a0,3 | 0x80103208 beq v0,zero,0x80103390 | 0x80103378 jal 0x8001bf10 | 0x80103388 addiu v0,v0,1 | 0x8010338c sb v0,363(s3)

## Aufrufe

- `0x80102bc4` -> `0x80102BDC (Rumpf)` (a0/a1/a2 unveraendert durchgereicht (Wrapper legt nur ra ab)) — Trampolin; 0x80102BBC steht in der Zustandstabelle @0x801055CC[3]
- `0x80102c54` -> `0x8008E1F4 = RotMatrix (libgte)` (a0 = sp+24 @0x80102c38 (SVECTOR {0 @0x80102c4c, Spieler-Yaw [0x800cfc6e] @0x80102c44/@0x80102c58, 0 @0x80102c50}); a1 = sp+32 @0x80102c48 (MATRIX out)) — Y-Rotationsmatrix aus der Spieler-Blickrichtung (0x800cfc6e = Spieler+0x76, Spielerbasis 0x800cfbf8)
- `0x80102c70` -> `0x8008DBA4 = ApplyMatrixLV (libgte)` (a0 = sp+32 @0x80102c5c; a1 = sp+24 @0x80102c60; a2 = sp+24 @0x80102c64; Eingabevektor (4096 @0x80102c6c, 0 @0x80102c74, 0 @0x80102c50)) — rotierter Einheitsvektor; dessen .x wird gegen -64 @0x80102c80 geprueft, .z ist der Strahl-Steigungsterm
- `0x80102df0` -> `0x80015FE8 = rng` (keine; v0 nach s0 @0x80102dfc) — ESP-Untertyp-Bit (rng&1)<<16
- `0x80102df8` -> `0x80015FE8 = rng` (keine; v0 & 0xff @0x80102e08) — ESP-Parameterstreuung <<3
- `0x80102e28` -> `0x8001BF10 = ESP-Spawn(a0=id, a1=yaw, a2=MATRIX*, a3=SVECTOR* Offset|NULL)` (a0 = s0|(8096+(rng&0xff)*8) @0x80102e14; a1 = 0 @0x80102e18; a2 = 0x8009DB44 @0x80102e24; a3 = sp+16 @0x80102e2c (x=Entity+0x38+3183 @0x80102d18, y=-1400 @0x80102d10, z=clamp(SpielerZ+Projektion) @0x80102d94..@0x80102dc4)) — Blutfontaene A, 1. Sprite
- `0x80102e3c` -> `0x80015FE8 = rng` (keine; Delay-Slot @0x80102e40 schreibt sp+16 += 500) — Z-Jitter des 2. Sprites
- `0x80102e54` -> `0x80015FE8 = rng` (keine; Delay-Slot @0x80102e58 schreibt sp+20) — ESP-Untertyp-Bit
- `0x80102e5c` -> `0x80015FE8 = rng` (keine; v0&0xff <<2 @0x80102e6c/@0x80102e70) — ESP-Parameterstreuung <<2
- `0x80102e84` -> `0x8001BF10 = ESP-Spawn` (a0 = s0|(8096+(rng&0xff)*4) @0x80102e78; a1 = 0 @0x80102e7c; a2 = 0x8009DB44 @0x80102e80; a3 = sp+16 @0x80102e88) — Blutfontaene A, 2. Sprite
- `0x80102ec4` -> `0x800154AC = Winkel(x1,z1,x2,z2) -> 12-bit Yaw` (a0 = (s16)Entity+0x38 @0x80102eac; a1 = (s16)Entity+0x40 @0x80102eb0; a2 = (s16)[0x800cfc30] SpielerX @0x80102eb8; a3 = (s16)[0x800cfc38] SpielerZ @0x80102ec0) — Yaw Gegner->Spieler; Ergebnis s0 @0x80102ecc, s16-erweitert @0x80102f10/@0x80102f14
- `0x80102efc` -> `0x80015FE8 = rng` (keine; Delay-Slot @0x80102f00 schreibt sp+20 = Entity+0xAC) — ESP-Parameterstreuung
- `0x80102f28` -> `0x8001BF10 = ESP-Spawn` (a0 = 8096+(rng&0xff)*8 @0x80102f04..@0x80102f0c; a1 = s0 (Yaw) @0x80102f18; a2 = 0x8009DB44 @0x80102f24; a3 = sp+16 @0x80102f2c) — Blutfontaene B, 1. Sprite (nur wenn s2==1)
- `0x80102f3c` -> `0x80015FE8 = rng` (keine; Delay-Slot @0x80102f40 schreibt sp+24 (QUIRK, nie gelesen)) — ESP-Parameterstreuung
- `0x80102f60` -> `0x8001BF10 = ESP-Spawn` (a0 = (8096+(rng&0xff)*8)|0x10000 @0x80102f44..@0x80102f54; a1 = s0 @0x80102f58; a2 = 0x8009DB44 @0x80102f5c; a3 = sp+16 @0x80102f64 (UNVERAENDERT — siehe QUIRK_SP24)) — Blutfontaene B, 2. Sprite
- `0x80102f78` -> `0x8005BD6C = Gegner-SE` (a0 = 8 @0x80102f74; a1 = Entity @0x80102f7c) — Todes-SE der Fontaene B; nur wenn Entity+0x05 != 1 (@0x80102f68/@0x80102f70)
- `0x80102f98` -> `0x8010221C = Gore-Emitter kurz (2 ESPs an Teil[2]-Matrix, ids 0x05042710 und 0x040F1000+(rng&0xff)*4)` (a0 = Entity @0x80102f9c) — Unterzustand 9 oder 17 (@0x80102f88/@0x80102f90)
- `0x80102fb0` -> `0x80101D9C = Gore-Emitter lang (8 ESPs, Parameterbasen 10000/8000/4000, id-Banken 0x0504/0x040F)` (a0 = Entity @0x80102fb4) — Unterzustand 10 (@0x80102fa8)
- `0x80102fc8` -> `0x80101FF0 = Gore-Emitter (7 ESPs, Parameterbasen 10096/10000/8000/6000, id-Bank 0x040F)` (a0 = Entity @0x80102fcc) — Unterzustand 11 (@0x80102fc0)
- `0x80102fe0` -> `0x801022D0 = Gore-Emitter (8 ESPs an Teil[0]+0x48 und Teil[1]+0x48, Basen 10296/6696/6000/4000, id-Banken 0x0600/0x040F)` (a0 = Entity @0x80102fe4) — Unterzustand 14 (@0x80102fd8)
- `0x80103028` -> `0x80100784 = Zustand-1-Handler` (a0 = Entity @0x80103020 (Delay-Slot, gilt fuer beide Zweige); a1 = s4 = Entity+0x108 @0x80103024; a2 = s5 = Entity+0x17C @0x8010302c) — aufgeschobener Tod: restauriertes Entity+0x04 == 1 → Zustand 1 sofort weiterfahren
- `0x8010303c` -> `0x80102AD0 = Taumel-/Trefferreaktions-Maschine (setzt +0x226|=2 @0x80102af8, Motion 0x001F0008, SE 12 @0x80102b20, SE 9 bei Frame 15 @0x80102b48; am Anim-Ende Entity+0x04:=1 und +0x226&=~2)` (a0 = Entity @0x80103020; a1 = s4 @0x80103038; a2 = s5 @0x80103040) — aufgeschobener Tod: restauriertes Entity+0x04 != 1
- `0x8010307c` -> `0x8005BD6C = Gegner-SE` (a0 = 10 @0x80103054; a1 = Entity @0x80103058) — Todes-SE beim echten Tod
- `0x80103094` -> `0x80015FE8 = rng (Schleifenkopf, 7 Durchlaeufe)` (keine; Delay-Slot @0x80103098 dekrementiert s1) — ESP-Untertyp-Bit je Modellteil
- `0x8010309c` -> `0x80015FE8 = rng` (keine; v0&0xff <<2 @0x801030ac/@0x801030b0) — ESP-Parameterstreuung
- `0x801030c8` -> `0x8001BF10 = ESP-Spawn` (a0 = ((rng&1)<<16)|(8096+(rng&0xff)*4) @0x801030b8; a1 = (s16)Entity+0x76 @0x801030bc; a2 = Teil[i]+0x48 @0x801030c0 (Basis Entity+0x198 @0x80103088, Stride 172 @0x801030cc); a3 = 0 @0x801030c4) — 7 Blutwolken, je eine an Teil 0..6
- `0x801030f4` -> `0x8005BD6C = Gegner-SE` (a0 = 13 @0x801030d4; a1 = Entity @0x801030d8) — Kollaps-SE
- `0x801030fc` -> `0x80039514 = Blitz/Fade SETZEN (Auftragsliste @0x800EAC28)` (a0 = 250 @0x801030fc; a1 = 180 @0x80103100; a2 = 0 @0x80103108) — Bildschirmblitz beim Tod
- `0x8010310c` -> `0x800395B8 = Blitz/Fade RAMPE (Auftragsliste @0x800EAC28)` (a0 = 150 @0x8010310c; a1 = 180 @0x80103110; a2 = 0 @0x80103114; a3 = 250 @0x8010311c) — Abklingen des Todesblitzes
- `0x80103120..0x801031b8` -> `0x800395B8 x8` ((a0,a1,a2,a3) = (10,180,220,20)@0x80103120 (10,220,180,30)@0x80103134 (10,180,220,40)@0x80103148 (10,220,180,50)@0x8010315c (10,180,220,65)@0x80103170 (10,220,180,75)@0x80103184 (10,180,220,230)@0x80103198 (10,220,180,240)@0x801031ac) — Flacker-Kette; faellt danach ohne Sprung in den p1-Rumpf @0x801031c0
- `0x801031cc` -> `0x8002959C = anim_advance (liest Entity+0x14C Motion, inkrementiert Entity+0x14D @0x80029b30, Rueckgabe 1 bei Clip-Umlauf @0x80029b3c..@0x80029b4c)` (a0 = Entity @0x801031c0; a1 = Entity+0x108 @0x801031c4; a2 = Entity+0x17C @0x801031c8; a3 = 128 @0x801031d0) — p1 Kollaps-Anim
- `0x80103348` -> `0x80015FE8 = rng` (keine; v0 nach s0 @0x80103354) — ESP-Untertyp-Bit
- `0x80103350` -> `0x80015FE8 = rng` (keine; v0&0xff <<3 @0x80103360/@0x80103364) — ESP-Parameterstreuung
- `0x80103378` -> `0x8001BF10 = ESP-Spawn` (a0 = s0|(8096+(rng&0xff)*8) @0x8010336c; a1 = (s16)Entity+0x76 @0x80103374; a2 = Entity+0x198+416 (Teil[2]-Matrix) @0x8010337c; a3 = sp+16 @0x80103370 (interpolierter Wegpunkt aus 0x801056C8)) — Blutspur waehrend des Kollapses, 1 Sprite pro Frame
- `0x801035dc` -> `0x80015FE8 = rng` (keine; v0&0xff @0x801035e4, divu durch Kandidatenzahl s0 @0x801035e8, Rest mfhi @0x801035f8) — Auswahl eines lebenden Begleiters aus sp+64[]
- `0x80103608` -> `0x80104E9C = set_companion_state(idx, word) → *(u32*)([0x800cfe20 + idx*4] + 4) = word` (a0 = sp+64[rng % s0] @0x80103604; a1 = 3585 (0x00000E01) @0x8010360c) — Begleiter (Typ 0x37) auf Zustand 1 / Unterzustand 14 setzen
- `0x80103614` -> `0x8003947C = Ruettel-/Effektauftrag (Auftragsliste @0x800EAAD8)` (a0 = 3 @0x80103610; a1 = 0 @0x80103618) — Erschuetterung beim Begleiter-Kommando
- `0x80103624` -> `0x80039514 = Blitz/Fade SETZEN` (a0 = 15 @0x8010361c; a1 = 250 @0x80103620; a2 = 0 @0x80103628) — kurzer Blitz beim Begleiter-Kommando
- `0x80103418` -> `0x8002959C = anim_advance` (a0 = Entity @0x8010340c; a1 = Entity+0x108 @0x80103410; a2 = Entity+0x17C @0x80103414; a3 = 128 @0x8010341c) — p2 Zuck-Anim
- `0x8010342c` -> `0x80015FE8 = rng` (keine; Delay-Slot @0x80103430 schreibt bereits Motion 6; danach v0&3 @0x80103434) — 1-von-4-Entscheidung fuer Motion 8
- `0x80103478` -> `0x8010503C = wobble_set(idx, b2, b3) → Bytes +2/+3 des 28-Byte-Satzes @0x80105610+idx*28` (a0 = 0 @0x8010345c; a1 = 0 @0x80103460; a2 = 0 @0x80103464) — Wobble-Kanal 0 abschalten (Explosion)
- `0x80103488` -> `0x8010503C = wobble_set` (a0 = 1 @0x80103480; a1 = 0 @0x80103484; a2 = 0 @0x8010348c) — Wobble-Kanal 1 abschalten
- `0x801034c4` -> `0x80039514 = Blitz/Fade SETZEN` (a0 = 5 @0x80103498; a1 = 200 @0x8010349c; a2 = 0 @0x801034a4) — Explosionsblitz
- `0x801034d8` -> `0x800395B8 = Blitz/Fade RAMPE` (a0 = 200 @0x801034cc; a1 = 200 @0x801034d0; a2 = 0 @0x801034d4; a3 = 5 @0x801034dc) — Abklingen des Explosionsblitzes
- `0x801034e4..0x80103568` -> `0x8003947C x12` ((a0,a1) = (3,0)@0x801034e4 (3,5)@0x801034f0 (2,15)@0x801034fc (3,60)@0x80103508 (3,65)@0x80103514 (2,75)@0x80103520 (3,90)@0x8010352c (3,95)@0x80103538 (2,105)@0x80103544 (2,110)@0x80103550 (2,115)@0x8010355c (1,120)@0x80103568) — 12 zeitversetzte Erschuetterungen der Explosion
- `0x80103650` -> `0x8002959C = anim_advance` (a0 = Entity @0x80103644; a1 = Entity+0x108 @0x80103648; a2 = Entity+0x17C @0x8010364c; a3 = 128 @0x80103654) — p3 Explosionsclip; Rueckgabe wird auf Entity+0x06 addiert (@0x80103660)

## Offen
- Wer setzt Entity+0x04 = 3 beim NORMALEN Tod (HP<=0)? In CDEMD0_EM36_ai1.BIN gibt es genau EINEN Setzer: @0x801007fc (aufgeschobener Tod, (+0x226&6)==4). Der regulaere Pfad muss aus der RE2-PSX.EXE kommen (Schadensaufloeser um 0x800265A4). NICHT geraten — noch zu belegen.
- Wer SETZT die Bits in Entity+0x228 (Begleiter-Kandidatenmaske)? Im gesamten EM36-Blob nur Lesezugriffe (@0x801009f8, @0x80100af4, @0x80100c00, @0x80101998, @0x801033c4, @0x80103590) und ein sb zero,552(s0) @0x80100494. Setzer vermutlich in CDEMD0_EM37_ai1.BIN oder in der EXE.
- Entity+0x1FC (gesichertes Zustandswort vor dem Treffer): im Blob nur Lesezugriffe (@0x801029e8, @0x80102a68, @0x80102ffc). Der Schreiber liegt ausserhalb.
- Struktur hinter Teil[2]+0x88 (Absolutoffset 480 ab Entity+0x198) mit dem s16-Absink-Parameter bei +0x14: Typ, Initialisierung und Nullstellung vor p4 nicht belegt.
- Offset 1992 (0x7C8) in *(u32*)(Spieler+0x198) @0x80102de4 laesst sich nicht auf das 172-Byte-Teil-Raster abbilden (1992 = 11*172 + 100). Feldbedeutung ungeklaert — literal uebernehmen.
- Globale Vektor-Tripel 0x800cfb88/0x800cfb8c/0x800cfb90 (Weltzielpunkt in den Gore-Emittern 0x80101d9c/0x80101ff0/0x8010221c) sind unbenannt; sie liegen ausserhalb der Spielerstruktur (Spielerbasis = 0x800cfbf8, belegt via @0x80100308 sw 0x800cfbf8,436(s3)).
- Bedeutung der ESP-Parameter-Low16 (8096 + n*4/8) und der Untertyp-Bits 16..23 in 0x8001BF10 ist nur strukturell belegt (a0>>24 = Bank-Index in [0x800d4e18+bank*4], (a0>>16)&7 = Sprite-Index, a0<<16 → Slot+56, a2 = MATRIX 8 Worte → Slot+76..104, a3 = 2 Worte → Slot+44/48). Welche Sprites das konkret sind, ist offen.
- Entity+0x225 (240 Blitzpulse) wird von 0x80104EB4 verbraucht, das dabei Entity+0x144 gegen 121 und 21 testet. Bedeutung von +0x144 ungeklaert (nicht die HP — die liegen bei +0x156).
- Randfall Blutspur: Entity+0x16B wird in p1 pro Frame inkrementiert und nur bei Anim-Frame 60 genullt. Erreicht es >= 0x80, wird ((s8)x)>>4 negativ, slti <3 bleibt wahr und Tabelle 0x801056C8 wird UNTERHALB ihres Anfangs gelesen. Ob das im Original eintritt, haengt an der Laenge von Motion 7 — nicht gemessen.
- Die Namen 'Blitz/Fade' fuer 0x80039514 / 0x800395B8 und 'Ruettel' fuer 0x8003947C sind aus der Struktur abgeleitet (8-Byte-Auftragssaetze in zwei Listen @0x800EAC28 bzw. @0x800EAAD8; rec[0]=1, rec[1]=a1 bzw. (a0!=0), rec[2..3]=Verzoegerung, rec[4..5]=Wert, 0x800395B8 rechnet zusaetzlich ((a2-a1)<<7)/a0 @0x80039610-40). Der Konsument (Bildschirmeffekt vs. Kamera-Shake) ist NICHT disassembliert.
- p5 (0x8010380C) ist mit dem Epilog identisch — unbelegt, ob der Boss danach von aussen entfernt wird oder dauerhaft in Phase 5 verbleibt.
- Die 28-Byte-Saetze @0x80105610 (Oszillator/Wobble, Konsument 0x80105064) sind fuer Zustand 3 nur als 'abschalten' relevant; ihre Feldbedeutung (+0, +2, +8, +10, +20) ist nicht zerlegt.
- Der Modulo-3-Block auf Entity+0x1D2 @0x80102d1c..@0x80102d80 ist toter Code (Ziel sp+26 wird nie gelesen). Falls der Port ihn weglaesst, bleibt das Verhalten identisch — der RNG wird dort NICHT gezogen, also gibt es auch keine RNG-Verschiebung.


---

# EM36 / RE2-Endgegner Typ 0x36 (CDEMD0_EM36_ai1.BIN, Ladebasis 0x80100000) — Zustand 4 = Zustandstabelle 0x801055CC[4] = 0x80103834; dazu der komplette Unterzustands-Zweig 0x80103908 (Phasen 0/1/2/3) und der Leerzustand 7 = 0x80103878.

0x80103834 ist NICHT der Zustandskoerper, sondern ein 56-Byte-Unter-Dispatcher: er liest +0x05 und springt ueber die Zeigertabelle @0x801056E8 (genau 2 Eintraege: [0]=0x801038BC = reines "jr ra", [1]=0x80103908 = der Koerper). Kein Bereichstest. Der Koerper 0x80103908 ist eine 3-Phasen-Maschine auf +0x06: Phase 0 setzt einmalig Clip 0 mit Crossfade 7 (+0x14C = 0x00070000), setzt das globale Riegelbit *0x800CFBD8 |= 0x40, +0x1D3 |= 0x80, +0x1C0 = 1, spielt den positionierten SE 0x04010001 (Bank 4, Record 1) und faellt OHNE Sprung direkt in Phase 1. Phase 1 zieht die Weltposition jeden Frame um die HALBE Restdistanz an den Anker (+0x164/+0x168) heran (arithmetisches >>1), dreht +0x76 mit 256/Frame (=22.5 Grad) auf den Zielwinkel +0x15A und laesst die Animation vorwaerts laufen; erreicht der Animationsframe +0x14D exakt den Wert 8, wird der SE 0x04010001 erneut gespielt, +0x76 hart auf +0x15A gesetzt und Phase 2 betreten. Phase 2 wendet Root-Motion an (0x80015CB8) waehrend der globale Winkel *0x800CFC6E um 2048 (=180 Grad) verdreht ist, laesst danach den Clip weiterlaufen und schaltet bei Clip-Umschlag (Rueckgabe != 0 von 0x8002959C) auf Phase 3. Phase 3 (und jede Phase >= 3) tut NICHTS — der Zustand 4 verlaesst sich selbst nie, es gibt in der gesamten 23476-Byte-KI keinen einzigen Schreiber von +0x04 = 4. In Zustand 4 gibt es KEINEN Abstandstest gegen +0x1F0, KEINEN Schadensaufruf 0x800401D4 und KEINEN RNG-Aufruf.

## Konstanten

| Name | Wert | Bedeutung | Adresse |
|---|---|---|---|
| SUB_TABLE_Z4 | 0x801056E8 = {0x801038BC, 0x80103908} | Feld: Unterzustands-Zeigertabelle fuer +0x05, genau 2 Eintraege | @0x80103850 (lw v0,0x56e8(at)); Rohbytes @0x801056E8: bc 38 10 80 08 39 10 80 |
| Z4_U0_STUB | 0x801038BC | Feld: Unterzustand 0 ist ein reines jr ra (tut nichts) | @0x801038bc (08 00 e0 03) |
| Z7_STUB | 0x80103878 | Feld: Zustand 7 ist ein reines jr ra | @0x80103878 (08 00 e0 03) |
| ANIMWORT_P0 | 0x00070000 | Clip: +0x14C = 0x00 (Clip 0), +0x14D = 0x00 (Frame 0), +0x14E = 0x07 (Crossfade-Zaehler, wird in 0x800299C0-CC pro Aufruf dekrementiert) | @0x80103968 (lui v0,0x7) + @0x80103978 (sw v0,332(s1)) |
| PHASE_P0 | 1 | Feld: +0x06 := 1 | @0x8010396c (addiu a2,zero,1) + @0x8010397c (sb a2,6(s1)) |
| GLOBALRIEGEL_BIT | 0x40 auf *(u32*)0x800CFBD8 | Feld: globales Riegelbit wird gesetzt (Gegenstueck: EM37 loescht es mit -65 = ~0x40 @0x80104650-5C) | @0x80103970/74 (lui/addiu -> 0x800CFBD8), @0x80103980 lw, @0x80103988 (ori v0,v0,0x40), @0x8010398c sw |
| FLAG_1D3 | |= 0x80 | Feld: +0x1D3 Bit 7 setzen (Gegenstueck: EM37 &= 0x7F @0x80104638) | @0x80103990 (lbu v0,467(s1)), @0x8010399c (ori v0,v0,0x80), @0x801039a4 (sb v0,467(s1)) |
| FLAG_1C0 | 1 (glatter Byte-Store, kein OR) | Feld: +0x1C0 := 1 (Gegenstueck: EM37 sb zero,448 @0x80104630) | @0x80103998 (sb a2,448(s1)), a2 = 1 @0x8010396c |
| SE_ID_P0 | 0x04010001 | SE-Id fuer 0x8005BA28: Bank = a0>>24 = 4 (srl t1,a0,24 @0x8005ba30), Record = (a0>>16)&0xFF = 1 (@0x8005ba7c-80), Parameter = a0&0xFF = 1 (@0x8005ba9c) | @0x80103948 (lui a0,0x401) + @0x80103984 (ori a0,a0,0x1) |
| SE_POSITION | &Entity+0x38 | Feld: a1 von 0x8005BA28 = Zeiger auf die Weltposition (X/Y/Z ab +0x38) | @0x80103994 (addiu a1,s1,56) |
| DREHSCHRITT_P1 | 256 (= 256/4096 = 22.5 Grad pro Frame) | Dauer/Rate: Schrittweite fuer den Yaw-Dreh-Helfer 0x8001569C | @0x801039ac (addiu a2,zero,256) |
| ZIELWINKEL_P1 | (s16)Entity+0x15A | Feld: Zielwinkel fuer +0x76; in Zustand 4 als Winkel benutzt (in Zustand 1/2 ist dasselbe Feld ein Zaehler, z.B. 150 @0x80101cc0) | @0x801039b8 (lh a1,346(s1)) |
| POS_LERP_SHIFT | >>1 (sra, also HALBE Restdistanz pro Frame) | Distanz: X und Z werden pro Frame um die halbe Differenz an den Anker herangezogen | @0x801039c8 (sra v1,v1,1) und @0x801039d4 (sra v0,v0,1) |
| ANKER_X | (s16)Entity+0x164 | Distanz/Feld: Ziel-X; wird im gesamten EM36-Overlay NUR hier gelesen und nie geschrieben | @0x801039b0 (lh v1,356(s1)) |
| ANKER_Z | (s16)Entity+0x168 | Distanz/Feld: Ziel-Z; wird im gesamten EM36-Overlay NUR hier gelesen und nie geschrieben | @0x801039bc (lh v0,360(s1)) |
| POS_X | (s32)Entity+0x38 | Feld: Weltposition X, wird zurueckgeschrieben | @0x801039b4 (lw t0,56(s1)) / @0x801039dc (sw t0,56(s1)) |
| POS_Z | (s32)Entity+0x40 | Feld: Weltposition Z, wird zurueckgeschrieben | @0x801039c0 (lw a3,64(s1)) / @0x801039e4 (sw a3,64(s1)) |
| ANIM_FLAG_P1 | 512 (0x200) | Feld: a3 von 0x8002959C; nur die OBERE Halbwortmaske zaehlt (lui v0,0xffff / and v0,a3,v0 @0x800295b8-C0) -> 0x200 = VORWAERTS | @0x801039f8 (addiu a3,zero,512) |
| FRAME_TRIGGER_P1 | 8 | Frame: exakter Gleichheitstest auf +0x14D (Animationsframe) fuer den Phasenwechsel 1->2 | @0x80103a00 (addiu v0,zero,8) + @0x80103a04 (bne v1,v0,0x80103a80) |
| SE_ID_P1 | 0x04010001 | SE-Id, identisch zu Phase 0 (zweiter Schlag/Aufprall bei Frame 8) | @0x80103a08 (lui a0,0x401, Delay-Slot) + @0x80103a0c (ori a0,a0,0x1) |
| PHASE_P1 | 2 | Feld: +0x06 := 2 | @0x80103a18 (addiu v0,zero,2) + @0x80103a1c (sb v0,6(s1)) |
| YAW_SNAP | +0x76 := (u16)+0x15A | Feld: harter Yaw-Snap beim Phasenwechsel 1->2 | @0x80103a14 (lhu v1,346(s1)) + @0x80103a24 (sh v1,118(s1), Delay-Slot) |
| GLOBALWINKEL_OFFSET | +2048 (= 180 Grad bei 4096 = 360 Grad) | Feld: *(u16*)0x800CFC6E wird VOR dem Root-Motion-Aufruf um 2048 erhoeht (Delay-Slot-Store) | @0x80103a48 (addiu v0,v0,2048) + @0x80103a50 (sh v0,0(s0), Delay-Slot des jal) |
| GLOBALWINKEL_ADRESSE | 0x800CFC6E | Feld: globaler 16-Bit-Winkel; wird sonst in Zustand 2 (@0x80102604) und Zustand 3 (@0x80102c44) an 0x8008E1F4 gereicht | @0x80103a38 (lui s0,0x800d) + @0x80103a3c (addiu s0,s0,-914) |
| ROOTMOTION_FLAG_P2 | 0 | Feld: a3 von 0x80015CB8 = 0 -> Vorwaerts-Zweig in 0x80015DB0 (beq a1,zero,0x80015df0 @0x80015dd0) | @0x80103a44 (addu a3,zero,zero) |
| ANIM_FLAG_P2 | 512 (0x200) | Feld: a3 von 0x8002959C in Phase 2 -> ebenfalls vorwaerts | @0x80103a64 (addiu a3,zero,512) |
| GLOBALWINKEL_RUECKNAHME | -2048, ABER im Delay-Slot -> der Store danach schreibt den RUECKGABEWERT | Feld: 80103a6c (addiu v0,v0,-2048) liegt im Delay-Slot des jal und wird von der Rueckgabe ueberschrieben; 80103a70 (sh v0,0(s0)) laeuft NACH dem Call und legt daher 0 oder 1 in 0x800CFC6E ab | @0x80103a6c / @0x80103a70; Rohbytes @0x80103a68: 67 a5 00 0c (jal 0x8002959c) | 00 f8 42 24 (addiu v0,v0,-2048) | 00 00 02 a6 (sh v0,0(s0)) |
| PHASE_P2 | 3 | Feld: +0x06 := 3 bei Clip-Umschlag | @0x80103a78 (addiu v0,zero,3, Delay-Slot) + @0x80103a7c (sb v0,6(s1)) |
| STACKRAHMEN_DISPATCH | 24 Byte, ra @sp+16 | Feld: Rahmen von 0x80103834 | @0x80103834 (addiu sp,sp,-24) / @0x80103838 (sw ra,16(sp)) |
| STACKRAHMEN_KOERPER | 48 Byte; s0@24 s1@28 s2@32 s3@36 ra@40 | Feld: Rahmen von 0x80103908; s1 = Entity (a0), s2 = a1, s3 = a2 | @0x80103908..@0x80103924 und @0x80103a80..@0x80103a94 |
| CLIP_FELD | +0x14C (u8) = Clip, +0x14D (u8) = Frame, +0x14E (u8) = Crossfade | Clip/Frame: von 0x8002959C bzw. 0x80015DB0 so gelesen | @0x800295a8 (lbu v0,332(a0), Rohbytes '4c 01 82 90'), @0x800295d0 (lbu v0,333(a0)), @0x800299c0 (lbu v0,334(s2)) / @0x800299cc (sb v0,334(s2)) |
| DREHREGEL_8001569C | delta = (Ziel - +0x76 + Schritt) & 0xFFF; delta < 2*Schritt -> +0x76 = Ziel; delta > 0x800 -> +0x76 -= Schritt; sonst +0x76 += Schritt | Distanz/Winkel: exakte Drehregel des Helfers | @0x800156c0-@0x80015704 (lhu v1,118(a0) | subu v0,t0,v1 | addu v0,a3,v0 | andi a1,v0,0xfff | sra v0,v0,15 | slt v0,a1,v0 | sltiu v0,a1,0x801) |
| NEGATIVER_DREHSCHRITT | Schritt < 0 -> Ziel = (Ziel + 2048) & 0xFFF, Schritt = |Schritt| | Winkel: Wegdrehen statt Hindrehen (in Zustand 4 NICHT benutzt, Schritt ist +256) | @0x800156ac-@0x800156bc (bgez a2 | subu a3,zero,v1 | addiu v0,a1,2048 | andi t0,v0,0xfff) |

## Uebergaenge

- **z4 (Eintritt, Dispatcher 0x80103834) -> z4 u0 = Leerzweig 0x801038BC** wenn +0x05 == 0
  - 80103834: addiu sp,sp,-24 | 80103838: sw ra,16(sp) | 8010383c: lbu v0,5(a0) | 80103844: sll v0,v0,2 | 80103848: lui at,0x8010 | 8010384c: addu at,at,v0 | 80103850: lw v0,0x56e8(at) | 80103858: jalr v0 | Tabelle 0x801056E8[0] Bytes 'bc 38 10 80' = 0x801038BC ; 0x801038bc: 08 00 e0 03 = jr ra / 0x801038c0: nop
- **z4 (Eintritt, Dispatcher 0x80103834) -> z4 u1 = Koerper 0x80103908** wenn +0x05 == 1 (kein Bereichstest; jeder andere Wert springt in Fremddaten)
  - 80103850: lw v0,0x56e8(at) | 80103858: jalr v0 ; Tabelle 0x801056E8[1] Bytes '08 39 10 80' = 0x80103908 ; naechstes Wort 0x801056F0 = 0x1000002B, kein RAM-Zeiger -> Tabelle hat genau 2 Eintraege
- **z4 u1 p0 -> z4 u1 p1 (im SELBEN Frame, Durchfall ohne Sprung)** wenn +0x06 == 0 — unbedingt
  - 80103928: lbu v1,6(s1) | 8010392c: addiu v0,zero,1 | 80103930: beq v1,v0,0x801039a8 | 80103938: slti v0,v1,2 | 8010393c: beq v0,zero,0x80103954 | 80103944: beq v1,zero,0x80103968 | 80103948: lui a0,0x401 (ds) ... 8010397c: sb a2,6(s1) [+0x06 = 1] ... 801039a0: jal 0x8005ba28 | 801039a4: sb v0,467(s1) (ds) ; danach KEIN Sprung, 801039a8 ist das Phase-1-Label
- **z4 u1 p1 -> z4 u1 p1 (Verbleib)** wenn +0x14D != 8 nach dem Anim-Advance
  - 801039fc: lbu v1,333(s1) | 80103a00: addiu v0,zero,8 | 80103a04: bne v1,v0,0x80103a80 | 80103a08: lui a0,0x401 (ds) ; 0x80103a80 = Epilog
- **z4 u1 p1 -> z4 u1 p2** wenn +0x14D == 8 (Animations-Frame-Trigger, exakter Gleichheitstest)
  - 80103a04: bne v1,v0,0x80103a80 | 80103a0c: ori a0,a0,0x1 | 80103a10: addiu a1,s1,56 | 80103a14: lhu v1,346(s1) | 80103a18: addiu v0,zero,2 | 80103a1c: sb v0,6(s1) [+0x06 = 2] | 80103a20: jal 0x8005ba28 | 80103a24: sh v1,118(s1) (ds) [+0x76 = +0x15A] | 80103a28: j 0x80103a80
- **z4 u1 p2 -> z4 u1 p2 (Verbleib)** wenn Rueckgabe von 0x8002959C == 0 (Clip laeuft noch)
  - 80103a68: jal 0x8002959c | 80103a6c: addiu v0,v0,-2048 (ds) | 80103a70: sh v0,0(s0) | 80103a74: beq v0,zero,0x80103a80 | 80103a78: addiu v0,zero,3 (ds, wird verworfen)
- **z4 u1 p2 -> z4 u1 p3** wenn Rueckgabe von 0x8002959C != 0, d.h. +0x14D hat die Framezahl des Clips erreicht und wurde auf 0 zurueckgesetzt (80029b30: addiu v0,v0,1 | 80029b34: sb v0,333(s2) | 80029b3c: sltu v0,v0,s3 | 80029b40: bne v0,zero,0x80029b50 | 80029b44: addu v0,zero,zero (ds) | 80029b48: sb zero,333(s2) | 80029b4c: addiu v0,zero,1)
  - 80103a74: beq v0,zero,0x80103a80 | 80103a78: addiu v0,zero,3 | 80103a7c: sb v0,6(s1) [+0x06 = 3]
- **z4 u1 p3 (und jede Phase >= 3) -> z4 u1 p3 (Endlager, kein Selbstaustritt)** wenn +0x06 >= 3 — der Dispatcher faellt in den Epilog
  - 80103954: addiu v0,zero,2 | 80103958: beq v1,v0,0x80103a30 | 8010395c: addu a0,s1,zero (ds) | 80103960: j 0x80103a80 | 80103964: nop ; 0x80103a80: lw ra,40(sp) ... 80103a98: jr ra
- **z4 (alle Phasen) -> — kein Zustandswechsel** wenn 0x80103908..0x80103A9C schreibt NIE +0x04; Ausstieg aus Zustand 4 nur von aussen
  - grep ueber die vollstaendige Disassemblierung 0x80100000..0x80105BF4: kein 'sb x,4(...)', kein 'sh x,4(...)'; alle 'sw x,4(...)' schreiben 1 (801003e8), 3 (801007fc), 0x401=1025 (80100908, 80103ea4), 0x301=769 (80100c4c, 80100cb4), 0x101=257 (80100c70, 80100c88, 80100ce4, 801017cc), 1 (8010103c, 80101968, 80102b88), oder kopieren +0x1FC (801029f0, 80103008) bzw. ein Argument (80102a74, 80104e68/78/88/98/b0). Kein Literal 4 und kein Literal 0x104=260 im gesamten Overlay.
- **z7 (Zustandstabelle 0x801055CC[7] = 0x80103878) -> z7 (Leerzustand)** wenn unbedingt
  - 80103878: 08 00 e0 03 = jr ra | 8010387c: 00 00 00 00 = nop ; Tabelle 0x801055CC Bytes: cc 03 10 80 | 84 07 10 80 | bc 25 10 80 | bc 2b 10 80 | 34 38 10 80 | 00 00 00 00 | 00 00 00 00 | 78 38 10 80
- **z5 / z6 -> Absturz** wenn Tabelleneintraege 0x801055CC[5] und [6] sind 0x00000000 — jalr 0 (im Original unerreichbar)
  - 801055dc: 34 38 10 80 00 00 00 00 00 00 00 00 78 38 10 80

## Aufrufe

- `@0x80103858 (jalr v0)` -> `*(0x801056E8 + (+0x05)*4) -> 0x801038BC oder 0x80103908` (a0 = Entity (unveraendert vom Aufrufer, @0x8010383c wird nur gelesen), a1 = unveraendert, a2 = unveraendert — zwischen Funktionseintritt und jalr wird kein Argumentregister beschrieben) — Unter-Dispatch auf +0x05
- `@0x801039a0 (jal 0x8005ba28)` -> `0x8005BA28 = Se_on (positionierter Gegner-SE; Bank = a0>>24 @0x8005ba30, Record = (a0>>16)&0xFF @0x8005ba7c-80, Parameter = a0&0xFF @0x8005ba9c; Abbruch wenn 0x800D4C48[Bank] == -1 @0x8005ba64-6c)` (a0 = 0x04010001 (Setzer: lui a0,0x401 @0x80103948 im Delay-Slot von beq, ori a0,a0,0x1 @0x80103984) ; a1 = Entity+0x38 (Setzer: addiu a1,s1,56 @0x80103994)) — Phase 0: Start-SE des Zustands (Bank 4, Record 1) an der eigenen Position
- `@0x801039e0 (jal 0x8001569c)` -> `0x8001569C = Yaw-Dreh-Helfer auf +0x76` (a0 = Entity (Setzer: addu a0,s1,zero @0x801039a8) ; a1 = (s16)Entity+0x15A = Zielwinkel (Setzer: lh a1,346(s1) @0x801039b8) ; a2 = 256 = Schrittweite (Setzer: addiu a2,zero,256 @0x801039ac) ; a3 = neuer Z-Wert (Setzer: lw a3,64(s1) @0x801039c0, addu a3,a3,v0 @0x801039d8) — 0x8001569C liest a3 NICHT, das Register ist toter Rest) — Phase 1: +0x76 dreht mit 22.5 Grad/Frame auf +0x15A
- `@0x801039f4 (jal 0x8002959c)` -> `0x8002959C = Anim-Advance (setzt +0x178 = Posenzeiger @0x800295f8, ruft 0x80029614; Rueckgabe 1 genau in dem Frame, in dem +0x14D die Framezahl des Clips erreicht und auf 0 gesetzt wird, @0x80029b28-4C)` (a0 = Entity (Setzer: addu a0,s1,zero @0x801039e8) ; a1 = Aufrufer-a1 (Setzer: addu a1,s2,zero @0x801039ec, s2 = a1 @0x80103918) ; a2 = Aufrufer-a2 = EDD-Basis (Setzer: addu a2,s3,zero @0x801039f0, s3 = a2 @0x80103934 im Delay-Slot) ; a3 = 512 (Setzer: addiu a3,zero,512 @0x801039f8, Delay-Slot)) — Phase 1: Clip vorwaerts weiterschalten; Rueckgabe wird in Phase 1 NICHT ausgewertet
- `@0x80103a20 (jal 0x8005ba28)` -> `0x8005BA28 = Se_on` (a0 = 0x04010001 (Setzer: lui a0,0x401 @0x80103a08 im Delay-Slot von bne, ori a0,a0,0x1 @0x80103a0c) ; a1 = Entity+0x38 (Setzer: addiu a1,s1,56 @0x80103a10)) — Phase 1 -> 2: identischer SE bei Animationsframe +0x14D == 8
- `@0x80103a4c (jal 0x80015cb8)` -> `0x80015CB8 = Root-Motion-Anwender: holt ueber 0x80015DB0 den Wurzelversatz des aktuellen Clips (Clip/Frame aus *(0x800CE330)+0x14C/+0x14D @0x80015dbc/@0x80015df4), dreht ihn mit +0x76 (lh a0,118(s1) @0x80015d3c, 0x8008E8B4/0x8008DBA4) und schreibt Position = (+0x164,+0x166,+0x168) + Versatz nach +0x38/+0x3C/+0x40 (@0x80015d58-@0x80015d90)` (a0 = Entity (Setzer: addu a0,s1,zero @0x8010395c, Delay-Slot von beq v1,2) ; a1 = Aufrufer-a1 (Setzer: addu a1,s2,zero @0x80103a30) ; a2 = Aufrufer-a2 (Setzer: addu a2,s3,zero @0x80103a34) ; a3 = 0 (Setzer: addu a3,zero,zero @0x80103a44)) — Phase 2: Wurzelbewegung der Animation auf die Weltposition anwenden, waehrend *0x800CFC6E um 180 Grad verdreht ist
- `@0x80103a68 (jal 0x8002959c)` -> `0x8002959C = Anim-Advance` (a0 = Entity (Setzer: addu a0,s1,zero @0x80103a54) ; a1 = Aufrufer-a1 (Setzer: addu a1,s2,zero @0x80103a58) ; a2 = Aufrufer-a2 (Setzer: addu a2,s3,zero @0x80103a5c) ; a3 = 512 (Setzer: addiu a3,zero,512 @0x80103a64)) — Phase 2: Clip weiterschalten; Rueckgabe != 0 -> Phase 3. Achtung: die Rueckgabe wird zusaetzlich per sh v0,0(s0) @0x80103a70 in 0x800CFC6E abgelegt

## Offen
- Wer setzt +0x04 = 4? In der GESAMTEN EM36-KI (0x80100000..0x80105BF4, 23476 B) existiert kein Schreiber: kein 'sb x,4(...)', kein 'sh x,4(...)', kein Literal 4 bzw. 0x104 vor einem 'sw x,4(...)'. Ebenso keiner in CDEMD0_EM37_ai1.BIN. Kandidaten, die noch zu pruefen sind: (a) die Wiederherstellung aus +0x1FC ('lw v0,508(s3) | sw v0,4(s3)' @0x801029e8/f0 und @0x80102ffc/0x80103008 — wer +0x1FC beschreibt, ist NICHT im Overlay), (b) die Setzer-Helfer 0x80104E5C (setzt das Zustandswort aller vier Begleiter aus 0x800CFE20/24/28/2C) und 0x80104E9C (a0 = Index, a1 = Zustandswort), (c) das Raum-SCD von ROOM7040/ROOMG040, (d) die RE2-PSX.EXE.
- Wer setzt +0x15A (Zielwinkel) fuer Zustand 4? Im EM36-Overlay wird +0x15A nur an 0x80101888 (=0), 0x80101a7c (++), 0x80101cc4 (=150) und 0x80101cf0 (--) geschrieben — alles Zaehler-Semantik in anderen Zustaenden, nicht als Winkel. Der Winkelwert fuer Zustand 4 kommt von aussen.
- Wer setzt +0x164/+0x166/+0x168 (Anker-/Zielposition)? Diese Felder werden im EM36-Overlay ausschliesslich gelesen (@0x801039b0, @0x801039bc) und nie geschrieben; 0x80015CB8 liest sie ebenfalls nur (@0x80015d58/@0x80015d6c/@0x80015d80). Setzer liegt in der EXE oder im Skript.
- Bedeutung des globalen Riegelbits *0x800CFBD8 & 0x40: gesetzt @0x8010398c (EM36 z4 p0) und @0x801044d8 (EM37), geloescht @0x8010465C (EM37, and mit -65). Welcher Leser das Bit auswertet, ist noch nicht ermittelt (kein Leser im EM36/EM37-Overlay ausser diesen Schreibern).
- Klangliche Zuordnung von SE-Bank 4 / Record 1: die Aufloesung laeuft ueber 0x800D4C48[Bank] (s8) und die Zeigertabelle 0x800DBB78[Bank] (@0x8005ba64 / @0x8005ba8c) — beides Laufzeitdaten, im Chunk nicht enthalten. Fuer den Port muss die konkrete Sample-Id noch aus einem RAM-Abzug gelesen werden.
- Der Store @0x80103a70 (sh v0,0(s0)) laeuft nach dem Call und schreibt daher die Rueckgabe von 0x8002959C (0 oder 1) in 0x800CFC6E statt der beabsichtigten Ruecknahme um 2048. Das ist woertlich so im Original (Bytes @0x80103a68: 67 a5 00 0c | 00 f8 42 24 | 00 00 02 a6). Ob dieser Nebeneffekt in einem echten Durchlauf sichtbar wird, haengt vom Startwert von 0x800CFC6E ab und ist noch nicht dynamisch gemessen.
- Semantik von +0x14E = 7 (Crossfade-Startwert in Phase 0) gegenueber den 0x1F-Werten in den anderen EM36-Zustaenden (z.B. @0x801009a4 lui v0,0x1f): der Zaehler wird in 0x800299C0-CC dekrementiert und steuert die Posen-Interpolation; die genaue Interpolationsformel (0x800299D0-@0x80029AF0) ist hier nicht zerlegt.
- Unterzustand +0x05 == 0 ist ein Leerzweig (0x801038BC). Ob Zustand 4 jemals mit +0x05 = 0 betreten wird (z.B. ein Frame lang, bevor jemand +0x05 = 1 setzt), ist ungeklaert — der Setzer von +0x05 fuer Zustand 4 fehlt aus demselben Grund wie der von +0x04.


---

# EM36 (RE2-Endgegner Typ 0x36) — Zustand 7, Handler @0x80103878, Datei build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN (md5 b57f8315dc8ae5583df13b4699395f6d, Ladebasis 0x80100000, File-Offset = Addr-0x80100000)

Zustand 7 ist ein LEERER STUB. Der Handler @0x80103878 besteht aus exakt ZWEI Instruktionen: `jr ra` / `nop` (File-Offset 0x3878, Bytes 08 00 e0 03 00 00 00 00). Kein Stack-Frame, kein Unter-Dispatch, kein einziger jal, keine Konstante, kein Schreibzugriff auf irgendein Entity-Feld. Wenn +0x04 == 7 wird, laufen Root-Prolog und Root-Epilog (0x801000BC-0x801003CC) weiter, die Zustandslogik selbst tut NICHTS. Die Slots 5 und 6 der Zustandstabelle @0x801055CC sind literal 0x00000000; der Root-Dispatch @0x80100164-0x80100184 macht KEINE Bereichspruefung (lbu v0,4(s3) / sll v0,v0,2 / lw v0,0x801055CC(at) / jalr v0), also fuehrt Zustand 5 oder 6 zu `jalr 0` = Sprung nach PC=0x00000000 (PSX-Kernel-Vektorbereich) = undefiniert/Absturz; dasselbe gilt fuer jeden Zustand >= 8, weil lbu 0..255 liefert und ab 0x801055EC nur Daten stehen. Ein exhaustiver Scan aller Stores nach Offset +4 (sb/sh/sw, Immediate rueckwaerts bis 16 Instruktionen aufgeloest) ueber die RE2-PSX.EXE UND alle 112 *_ai1.BIN-Chunks ergibt: NIEMAND schreibt 5 oder 6 in EM36 (die einzigen 5er im gesamten Satz stehen in CDEMD0/1_EM2A_ai1.BIN @0x80102168, Wert 0x0205 — anderer Typ), und NIEMAND schreibt 7 in EM36_ai1, EM37_ai1 oder der EXE (alle 40 Fundstellen fuer 7 liegen in EM10/EM25/EM27/EM2E/EM2F/EM39/EM3A). Zustand 5/6/7 sind fuer EM36 damit unerreichbarer bzw. toter Code. Einzige Restluecke: die drei indirekten Restores `sw <+0x1FC>,4(ent)` @0x801029F0, @0x80102A74, @0x80103008; +0x1FC wird ausschliesslich vom EXE-Schadensapplier (@0x80047278, @0x8004749C, @0x800418F4) mit einem vorher gueltigen Zustandswort gefuellt.

## Konstanten

| Name | Wert | Bedeutung | Adresse |
|---|---|---|---|
| Z7_HANDLER_BODY | 0x03E00008 0x00000000 (jr ra; nop) | Feld - der komplette Rumpf von Zustand 7, File-Offset 0x3878 | @0x80103878 |
| STATE_TABLE_BASE | 0x801055CC | Feld - Zustandstabelle, 8 belegte Slots | @0x80100178 (lw v0,21964(at), at = lui 0x8010) |
| STATE_TBL[0] | 0x801003CC | Feld - INIT | @0x801055CC (File 0x55CC) |
| STATE_TBL[1] | 0x80100784 | Feld - Hauptkampf | @0x801055D0 |
| STATE_TBL[2] | 0x801025BC | Feld - HURT, nur vom EXE-Applier gesetzt | @0x801055D4 |
| STATE_TBL[3] | 0x80102BBC | Feld - DEATH, nur vom EXE-Applier gesetzt | @0x801055D8 |
| STATE_TBL[4] | 0x80103834 | Feld - Unter-Dispatcher auf +0x05, Tabelle 0x801056E8 | @0x801055DC |
| STATE_TBL[5] | 0x00000000 | Feld - NULL, kein Handler | @0x801055E0 |
| STATE_TBL[6] | 0x00000000 | Feld - NULL, kein Handler | @0x801055E4 |
| STATE_TBL[7] | 0x80103878 | Feld - leerer Stub | @0x801055E8 |
| STATE_TBL[8] (ausserhalb) | 0x7A040309 | Feld - Daten, keine Codeadresse; das ungepruefte lbu erlaubt Index bis 255 | @0x801055EC |
| SUB_TABLE_Z4 | 0x801056E8 | Feld - Untertabelle fuer Zustand 4 und fuer den 0x800CE300-Callback | @0x80103850 und @0x8010389C (lw v0,22248(at)) |
| SUB_TBL[0] | 0x801038BC | Feld - jr ra / nop, Stub (File 0x38BC) | @0x801056E8 |
| SUB_TBL[1] | 0x80103908 | Feld - echte Routine (Phasenmaschine auf +0x06) | @0x801056EC |
| SUB_TBL[2] (ausserhalb) | 0x1000002B | Feld - Daten, keine Codeadresse | @0x801056F0 |
| SUB_TABLE_CE400 | 0x801056EC | Feld - Untertabelle des 0x800CE400-Callbacks, um 4 Byte versetzt, ueberlappt SUB_TABLE_Z4 | @0x801038E8 (Rohbytes ec 56 22 8c @File 0x38E8) |
| CALLBACK_TBL_A | 0x800CE300 + Typ*4, Eintrag = 0x80103880 | Feld - Registrierung im INIT | @0x801005DC (sw v1,-7424(at)); Wert @0x801005C4 |
| CALLBACK_TBL_B | 0x800CE400 + Typ*4, Eintrag = 0x801038CC | Feld - Registrierung im INIT | @0x801005F8 (sw v1,-7168(at)); Wert @0x801005E8 |
| ANIM_DESC_INIT | 0x00070000 | Clip - nach +0x14C geschrieben beim Eintritt in Phase 0 von 0x80103908 | @0x80103968 (lui v0,0x7), Store @0x80103978 |
| ANIM_END_FRAME | 8 | Frame - +0x14D == 8 beendet Phase 1 | @0x80103A00 (addiu v0,zero,8), Vergleich @0x80103A04 |
| GLOBAL_0x800CFBD8_BIT40 | 0x40 | Feld - 0x800CFBD8 |= 0x40 in Phase 0 | @0x80103988 (ori v0,v0,0x40), Store @0x8010398C |
| FLAG_1D3_BIT80 | 0x80 | Feld - +0x1D3 |= 0x80 | @0x8010399C (ori v0,v0,0x80), Store @0x801039A4 |
| FLAG_1C0_SET1 | 1 | Feld - +0x1C0 = 1 | @0x8010396C (addiu a2,zero,1), Store @0x80103998 |
| CAMSHAKE_DELTA | +2048 / -2048 auf 0x800CFC6E | Feld - Halbwort, in Phase 2 erst +2048 vor jal 0x80015CB8, dann -2048 vor jal 0x8002959C | @0x80103A48 (addiu v0,v0,2048) und @0x80103A68 (addiu v0,v0,-2048) |
| ANIM_A3_512 | 512 (0x200) | Feld - a3 von jal 0x8002959C in 0x80103908 | @0x801039F8 und @0x80103A64 |
| SE_ARG_04010001 | 0x04010001 | Feld - a0 von jal 0x8005BA28 | @0x80103948 (lui a0,0x401) + @0x80103984 (ori a0,a0,0x1); zweite Stelle @0x80103A08 + @0x80103A0C |
| TURN_LIMIT_256 | 256 | Feld - a2 von jal 0x8001569C | @0x801039AC (addiu a2,zero,256) |
| APPROACH_SHIFT | >> 1 (Halbierung pro Frame) | Distanz - +0x38 += (+0x164 - +0x38) >> 1 und +0x40 += (+0x168 - +0x40) >> 1 in Phase 1 | @0x801039C4/@0x801039C8/@0x801039CC (X) und @0x801039D0/@0x801039D4/@0x801039D8 (Z); Stores @0x801039DC und @0x801039E4 |
| DMG_500 | 500 | Schaden - a0 von jal 0x800401D4 (einziger Spielerschaden im gesamten EM36-Chunk) | @0x801008F8 (addiu a0,zero,500), Aufruf @0x80100904 |
| SE_ID_10 | 10 | Feld - a0 von jal 0x8005BD6C | @0x801017C0 und @0x80101720 |
| STATEWORD_z1s4 | 0x00000401 | Feld - Zustand 1, +0x05 = 4 | @0x80100900, Store @0x80100908 |
| STATEWORD_z1s3 | 0x00000301 | Feld - Zustand 1, +0x05 = 3 | @0x80100C44 und @0x80100CB0 |
| STATEWORD_z1s1 | 0x00000101 | Feld - Zustand 1, +0x05 = 1 | @0x80100C6C, @0x80100C84, @0x80100CE0, @0x801017C8 |
| STATEWORD_z1s9_BEGLEITER | 0x00000901 | Feld - in die vier EM37-Begleiter (Zeiger 0x800D3C38/3C3C/3C40/3C44) geschrieben | @0x8010144C, @0x80101470, @0x80101498, @0x801014BC; Stores @0x80101454, @0x80101478, @0x801014A0, @0x801014C4 |
| STATEWORD_z1s10_BEGLEITER | 0x00000A01 | Feld - dito | @0x8010165C, @0x80101680, @0x801016A8, @0x801016CC; Stores @0x80101664, @0x80101688, @0x801016B0, @0x801016D4; ori-Variante @0x8010170C mit Stores @0x80101724 und @0x80101730 |
| DIST_THRESH_10000 | 10000 | Distanz - slti gegen +0x38 (lw 56) | @0x80100C64 |
| DIST_THRESH_10001 | 0x2711 = 10001 | Distanz - sltiu gegen +0x1F0 (vorberechneter Spielerabstand) | @0x80100C7C |
| TIMER_222_LT15 | 15 (0xF) | Dauer - sltiu gegen +0x222 | @0x80102A58 |
| TIMER_222_LT11 | 11 (0xB) | Dauer - sltiu gegen +0x222 | @0x801029C4 |
| TIMER_225_SET7 | 7 | Dauer - +0x225 = 7 | @0x801029CC, Store @0x801029D0 |
| TIMER_225_SET240 | 240 | Dauer - +0x225 = 240 im DEATH-Zweig | @0x80102FEC, Store @0x80102FF8 |
| TIMER_221_SET15 | 15 | Dauer - +0x221 = 15 | @0x80100144 (Root-Prolog, Store @0x8010014C) und @0x80102A48 (Store @0x80102A4C) |
| HURT_STATEWORD_MARKER | 3074 (0x0C02) | Feld - der EXE-Applier sichert +0x1FC nur, wenn das aktuelle Zustandswort NICHT 0x0C02 ist | @0x8004726C (Vergleich @0x80047270); Variante @0x8004748C (addiu v0,a0,-3074) + @0x80047490 (sltiu v0,v0,0x2) |
| ANIM_KEY_85 | 85 | Frame - +0x14D == 85 loest Rueckfall nach Zustand 1 aus | @0x80101028, Vergleich @0x8010102C |
| ROOT_GATE_0x20000000 | 0x20000000 | Feld - 0x800CFBDC & 0x20000000 != 0 ueberspringt die gesamte KI (Sprung nach 0x8010038C) | @0x801000CC (lw v0,-1060(v0) = 0x800CFBDC), @0x801000D0 (lui v1,0x2000), Verzweigung @0x801000E4 |
| STATE5_WRITER_FREMDTYP | 0x00000205 | Feld - der EINZIGE Schreiber eines Zustands 5 im gesamten Chunk-Satz; gehoert zu Typ 0x2A, NICHT zu EM36 | @0x80102164 (Immediate) / @0x80102168 (sw) in CDEMD0_EM2A_ai1.BIN und CDEMD1_EM2A_ai1.BIN |

## Uebergaenge

- **beliebig (Root-Dispatch) -> z7** wenn +0x04 == 7
  - 80100164: lbu v0,4(s3) | 8010016c: sll v0,v0,2 | 80100170: lui at,0x8010 | 80100174: addu at,at,v0 | 80100178: lw v0,21964(at) [=0x801055CC] | 8010017c: nop | 80100180: jalr v0 | 80100184: addu a0,s3,zero   (a1 = lw 264(s3) = +0x108 @0x80100160, a2 = lw 380(s3) = +0x17C @0x80100168). KEINE Bereichspruefung.
- **z7 -> z7 (unveraendert)** wenn immer - der Handler ist leer
  - 80103878: jr ra | 8010387c: nop   (Rohbytes @File 0x3878: 08 00 e0 03 00 00 00 00). Kein sb/sh/sw, kein jal, kein lbu +0x5, kein lbu +0x6, kein Stack-Frame.
- **beliebig -> z5 / z6 = CRASH** wenn +0x04 == 5 oder == 6
  - Tabelle @0x801055E0 = 0x00000000, @0x801055E4 = 0x00000000 (Rohbytes @File 0x55DC: 34 38 10 80 | 00 00 00 00 | 00 00 00 00 | 78 38 10 80). Root laedt 0 und fuehrt 80100180: jalr v0 aus -> PC = 0x00000000.
- **beliebig -> z>=8 = CRASH** wenn +0x04 >= 8 (lbu liefert 0..255, ungeprueft)
  - Tabellenende: 0x801055EC = 0x7A040309, 0x801055F0 = 0xC296857B, 0x801055F4 = 0x0000CDC9, 0x801055F8 = 0x00DBDA02 - reine Daten, keine Codeadressen.
- **z0 (INIT) -> z1** wenn INIT-Abschluss
  - 801003d8: addiu v0,zero,1 | 801003e8: sw v0,4(s0)  (schreibt +0x04..+0x07 = 0x00000001 -> Zustand 1, +0x5=0, +0x6=0) | 801003ec: sh zero,464(s0) [+0x1D0 = 0]
- **z1 -> z3** wenn 0x800CFB74 & 0x6 == 4
  - 801007e4: addiu v1,zero,4 | 801007e8: andi v0,v0,0x6 | 801007ec: bne v0,v1,0x80100800 | 801007f4: addiu v0,zero,3 | 801007f8: j 0x8010084c | 801007fc: sw v0,4(s0)
- **z1 -> z1 s4 + Spielerschaden 500** wenn (...) & 0x4 == 0
  - 801008f0: andi v0,v0,0x4 | 801008f4: bne v0,zero,0x80100948 | 801008f8: addiu a0,zero,500 | 801008fc: addu a1,zero,zero | 80100900: addiu v0,zero,1025 [=0x0401] | 80100904: jal 0x800401d4 | 80100908: sw v0,4(s0)
- **z1 s? -> z1 s3** wenn RNG-Zweig
  - 80100c44: addiu v0,zero,769 [=0x0301] | 80100c48: jal 0x80015fe8 | 80100c4c: sw v0,4(s2)  (Delay-Slot, v0 noch das Immediate);  zweite Stelle: 80100cb0: addiu v0,zero,769 | 80100cb4: sw v0,4(s2)
- **z1 s? -> z1 s1** wenn +0x38 (lw 56) < 10000
  - 80100c5c: lw v0,56(s2) | 80100c64: slti v0,v0,10000 | 80100c68: beq v0,zero,0x80100c74 | 80100c6c: addiu v0,zero,257 [=0x0101] | 80100c70: sw v0,4(s2)
- **z1 s? -> z1 s1** wenn +0x1F0 (Spielerabstand) < 0x2711 (10001)
  - 80100c74: lw v0,496(s2) | 80100c7c: sltiu v0,v0,0x2711 | 80100c80: bne v0,zero,0x80100c8c | 80100c84: addiu v0,zero,257 | 80100c88: sw v0,4(s2)
- **z1 s? -> z1 s1** wenn RNG & 1 == 0
  - 80100ccc: jal 0x80015fe8 | 80100cd4: andi v0,v0,0x1 | 80100cd8: bne v0,zero,0x80100ce8 | 80100ce0: addiu v0,zero,257 | 80100ce4: sw v0,4(s2)
- **z? -> z1 (+0x226 Bit 1 geloescht)** wenn +0x14D == 85
  - 80101024: lbu v1,333(s1) [+0x14D] | 80101028: addiu v0,zero,85 | 8010102c: bne v1,v0,0x80101050 | 80101034: lhu v0,550(s1) [+0x226] | 80101038: addiu v1,zero,1 | 8010103c: sw v1,4(s1) | 80101040: andi v0,v0,0xfffd
- **z? -> z1 s1** wenn +0x158 (Timer, sh 344) laeuft auf 0
  - 801017b4: addiu v0,v1,-1 | 801017b8: bne v1,zero,0x801017e8 | 801017bc: sh v0,344(s0) | 801017c0: addiu a0,zero,10 | 801017c8: addiu v0,zero,257 | 801017cc: sw v0,4(s0) | 801017d0: jal 0x8005bd6c (SE-Id 10)
- **z? -> z1** wenn jal 0x8002959c (Anim-Advance, a3=128) liefert != 0
  - 80101958: jal 0x8002959c | 8010195c: addiu a3,zero,128 | 80101960: beq v0,zero,0x80101978 | 80101964: addiu v1,zero,1 | 8010196c: sw v1,4(s2) | 80101970: andi v0,v0,0xfffd
- **z2 (HURT) -> z<gesichert aus +0x1FC>** wenn +0x226 & 0x2 != 0
  - 801029d4: lhu v0,550(s3) | 801029dc: andi v0,v0,0x2 | 801029e0: beq v0,zero,0x80102a18 | 801029e8: lw v0,508(s3) [+0x1FC] | 801029f0: sw v0,4(s3) | 801029f4: lbu v1,4(s3)
- **z2 -> z<gesichert> + Sofortaufruf des z1-Handlers** wenn +0x222 (lbu 546) < 15
  - 80102a50: lbu v0,546(s3) | 80102a58: sltiu v0,v0,0xf | 80102a5c: beq v0,zero,0x80102a80 | 80102a60: addu a0,s3,zero | 80102a68: lw v0,508(a0) | 80102a70: jal 0x80100784 | 80102a74: sw v0,4(a0)
- **z3 (DEATH) -> z<gesichert>, +0x156 (HP) := 0** wenn +0x226 & 0x2 != 0
  - 80102fe8: lhu v0,550(s3) | 80102fec: addiu v1,zero,240 | 80102ff0: andi v0,v0,0x2 | 80102ff4: beq v0,zero,0x8010304c | 80102ff8: sb v1,549(s3) [+0x225 = 240] | 80102ffc: lw v1,508(s3) | 80103004: sh zero,342(s3) [+0x156 = 0] | 80103008: sw v1,4(s3)
- **z3 -> z1** wenn jal 0x8002959c (a3=128) liefert != 0
  - 80102b74: jal 0x8002959c | 80102b78: addiu a3,zero,128 | 80102b7c: beq v0,zero,0x80102b94 | 80102b80: addiu v1,zero,1 | 80102b88: sw v1,4(s0)
- **EXTERN: EXE-Schadensapplier -> z2 (HP >= 0) bzw. z3 (HP < 0)** wenn Treffer; vorher Sicherung des alten Zustandsworts nach +0x1FC, ausser es ist bereits 0x0C02
  - 80047264: lw v1,4(s1) | 8004726c: addiu v0,zero,3074 [0x0C02] | 80047270: beq v1,v0,0x8004727c | 80047278: sw v1,508(s1) | 8004727c: lh v1,342(s1) [+0x156] | 80047280: addiu v0,zero,2 | 80047284: bgez v1,0x80047294 | 80047288: sw v0,4(s1) | 8004728c: addiu v0,zero,3 | 80047290: sw v0,4(s1) | 80047294: addiu v0,zero,1 | 80047298: sb v0,466(s1) [+0x1D2 = 1].  Zwillinge: 0x8004748C-0x800474BC und 0x800418DC-0x800418FC.
- **z4 @0x80103834 -> Unter-Dispatch ueber +0x05** wenn immer
  - 80103834: addiu sp,sp,-24 | 80103838: sw ra,16(sp) | 8010383c: lbu v0,5(a0) | 80103844: sll v0,v0,2 | 80103848: lui at,0x8010 | 8010384c: addu at,at,v0 | 80103850: lw v0,22248(at) [=0x801056E8] | 80103858: jalr v0 | 80103860: lw ra,16(sp) | 80103868: jr ra.  Tabelle: [0]=0x801038BC (jr ra/nop, Stub), [1]=0x80103908, [2]=0x1000002B (KEINE Codeadresse).
- **Callback 0x800CE300[Typ] (0x80103880) -> Unter-Dispatch ueber +0x05, Tabelle 0x801056E8** wenn im INIT registriert
  - 801005c0: lui v1,0x8010 | 801005c4: addiu v1,v1,14464 [=0x80103880] | 801005c8: lbu v0,8(s0) [Typ] | 801005d0: sll v0,v0,2 | 801005d4: lui at,0x800d | 801005d8: addu at,at,v0 | 801005dc: sw v1,-7424(at) [=0x800CE300 + Typ*4].  0x80103880 ist instruktionsgleich mit 0x80103834 (Rohbytes @File 0x3880 == @File 0x3834).
- **Callback 0x800CE400[Typ] (0x801038CC) -> Unter-Dispatch ueber +0x05, Tabelle 0x801056EC** wenn im INIT registriert
  - 801005e4: lui v1,0x8010 | 801005e8: addiu v1,v1,14540 [=0x801038CC] | 801005f8: sw v1,-7168(at) [=0x800CE400 + Typ*4].  801038d4: lbu v0,5(a0) | 801038e8: lw v0,22252(at) [=0x801056EC] (Rohbytes @File 0x38E8: ec 56 22 8c) -> [0]=0x80103908, [1]=0x1000002B (ungueltig).
- **0x80103908 Phase 0 (+0x06 == 0) -> Phase 1 (faellt sofort durch)** wenn +0x06 == 0
  - 80103928: lbu v1,6(s1) | 8010392c: addiu v0,zero,1 | 80103930: beq v1,v0,0x801039a8 | 80103938: slti v0,v1,2 | 8010393c: beq v0,zero,0x80103954 | 80103944: beq v1,zero,0x80103968 | 80103968: lui v0,0x7 | 8010396c: addiu a2,zero,1 | 80103978: sw v0,332(s1) [+0x14C = 0x00070000] | 8010397c: sb a2,6(s1) [+0x06 = 1] | 80103988: ori v0,v0,0x40 | 8010398c: sw v0,0(v1) [0x800CFBD8 |= 0x40] | 80103998: sb a2,448(s1) [+0x1C0 = 1] | 8010399c: ori v0,v0,0x80 | 801039a0: jal 0x8005ba28 | 801039a4: sb v0,467(s1) [+0x1D3 |= 0x80]
- **0x80103908 Phase 1 (+0x06 == 1) -> Phase 2** wenn +0x14D == 8
  - 801039fc: lbu v1,333(s1) | 80103a00: addiu v0,zero,8 | 80103a04: bne v1,v0,0x80103a80 | 80103a08: lui a0,0x401 | 80103a0c: ori a0,a0,0x1 | 80103a10: addiu a1,s1,56 | 80103a14: lhu v1,346(s1) [+0x15A] | 80103a18: addiu v0,zero,2 | 80103a1c: sb v0,6(s1) [+0x06 = 2] | 80103a20: jal 0x8005ba28 | 80103a24: sh v1,118(s1) [+0x76 := +0x15A]
- **0x80103908 Phase 2 (+0x06 == 2) -> Phase 3 (terminal)** wenn jal 0x8002959c (a3=512) liefert != 0
  - 80103a30: addu a1,s2,zero | 80103a38: lui s0,0x800d | 80103a3c: addiu s0,s0,-914 [=0x800CFC6E] | 80103a40: lhu v0,0(s0) | 80103a44: addu a3,zero,zero | 80103a48: addiu v0,v0,2048 | 80103a4c: jal 0x80015cb8 | 80103a50: sh v0,0(s0) | 80103a60: lhu v0,0(s0) | 80103a64: addiu a3,zero,512 | 80103a68: addiu v0,v0,-2048 | 80103a6c: jal 0x8002959c | 80103a70: sh v0,0(s0) | 80103a74: beq v0,zero,0x80103a80 | 80103a78: addiu v0,zero,3 | 80103a7c: sb v0,6(s1) [+0x06 = 3]
- **0x80103908 Phase 3 (+0x06 >= 3) -> Phase 3** wenn immer - terminal, kein Code
  - 80103954: addiu v0,zero,2 | 80103958: beq v1,v0,0x80103a30 | 8010395c: addu a0,s1,zero | 80103960: j 0x80103a80 (Epilog).  Ebenso der unerreichbare Zweig 8010394c: j 0x80103a80 fuer v1<2 && v1!=0 && v1!=1.

## Aufrufe

- `@0x80103878` -> `KEINER` (KEINE) — Zustand 7 enthaelt keinen einzigen jal/jalr. Die folgenden Eintraege betreffen den Root-Dispatch und die unmittelbare Nachbarschaft (0x80103834 / 0x80103880 / 0x801038CC / 0x80103908), die der Port zusammen mit Zustand 7 braucht.
- `@0x80100180` -> `jalr v0 = STATE_TBL[+0x04] (0x801055CC + state*4)` (a0 = Entity (addu a0,s3,zero @0x80100184, Delay-Slot); a1 = lw 264(s3) = +0x108 (@0x80100160); a2 = lw 380(s3) = +0x17C (@0x80100168); v0 = lw 21964(at) (@0x80100178)) — Root-Zustandsdispatch, UNGEPRUEFT. state == 5 oder 6 -> v0 = 0 -> jalr 0 -> PC = 0x00000000.
- `@0x80103858` -> `jalr v0 = SUB_TBL[+0x05] (0x801056E8 + sub*4)` (a0 = Entity, unveraendert vom Root durchgereicht; v0 = lw 22248(at) (@0x80103850)) — Zustand-4-Unterdispatch. sub 0 -> 0x801038BC (Stub), sub 1 -> 0x80103908, sub >= 2 -> Datenwort.
- `@0x801038A4` -> `jalr v0 = SUB_TBL[+0x05] (0x801056E8 + sub*4)` (a0 = Entity; v0 = lw 22248(at) (@0x8010389C)) — Callback 0x800CE300[Typ], instruktionsgleicher Klon von 0x80103834.
- `@0x801038F0` -> `jalr v0 = *(0x801056EC + sub*4)` (a0 = Entity; v0 = lw 22252(at) (@0x801038E8)) — Callback 0x800CE400[Typ]. sub 0 -> 0x80103908; sub 1 -> 0x1000002B (ungueltig).
- `@0x801039A0` -> `jal 0x8005BA28` (a0 = 0x04010001 (lui a0,0x401 @0x80103948 + ori a0,a0,0x1 @0x80103984); a1 = &Entity+0x38 (addiu a1,s1,56 @0x80103994); a2 = 1 (addiu a2,zero,1 @0x8010396C)) — Effekt/SE beim Eintritt in Phase 0 von 0x80103908; a1 zeigt auf die Entity-Position +0x38.
- `@0x801039E0` -> `jal 0x8001569C` (a0 = Entity (addu a0,s1,zero @0x801039A8); a1 = lh 346(s1) = +0x15A (@0x801039B8); a2 = 256 (addiu a2,zero,256 @0x801039AC); a3 = +0x40 + ((+0x168 - +0x40) >> 1) (lw a3,64(s1) @0x801039C0, subu @0x801039D0, sra @0x801039D4, addu @0x801039D8)) — Bewegung/Drehung in Phase 1. Parallel: +0x38 += (+0x164 - +0x38) >> 1 (Store @0x801039DC), +0x40-Store @0x801039E4.
- `@0x801039F4` -> `jal 0x8002959C (Anim-Advance)` (a0 = Entity (@0x801039E8); a1 = s2 = a1 des Aufrufers = +0x108 (@0x801039EC); a2 = s3 = a2 des Aufrufers = +0x17C (@0x801039F0); a3 = 512 (@0x801039F8)) — Animationsvorschub in Phase 1; Rueckgabe ungeprueft, der Phasenwechsel haengt an +0x14D == 8.
- `@0x80103A20` -> `jal 0x8005BA28` (a0 = 0x04010001 (lui a0,0x401 @0x80103A08 + ori a0,a0,0x1 @0x80103A0C); a1 = &Entity+0x38 (addiu a1,s1,56 @0x80103A10)) — Effekt/SE beim Uebergang Phase 1 -> Phase 2.
- `@0x80103A4C` -> `jal 0x80015CB8` (a0 = Entity (addu a0,s1,zero @0x8010395C, Delay-Slot des beq nach 0x80103A30); a1 = s2 = +0x108 (@0x80103A30); a2 = s3 = +0x17C (@0x80103A34); a3 = 0 (addu a3,zero,zero @0x80103A44)) — Phase 2; direkt davor 0x800CFC6E += 2048 (@0x80103A40 lhu, @0x80103A48 addiu, @0x80103A50 sh).
- `@0x80103A6C` -> `jal 0x8002959C (Anim-Advance)` (a0 = Entity (addu a0,s1,zero @0x80103A54); a1 = s2 (@0x80103A58); a2 = s3 (@0x80103A5C); a3 = 512 (@0x80103A64)) — Phase 2; Rueckgabe != 0 -> +0x06 = 3 (@0x80103A78/@0x80103A7C). Vorher 0x800CFC6E -= 2048 (@0x80103A68, Store @0x80103A70).
- `@0x80100904` -> `jal 0x800401D4 (Spielerschaden)` (a0 = 500 (addiu a0,zero,500 @0x801008F8); a1 = 0 (addu a1,zero,zero @0x801008FC)) — Einziger Schadensaufruf an den Spieler im gesamten EM36-Chunk; im Delay-Slot @0x80100908 zugleich das Zustandswort 0x0401.
- `@0x80102A70` -> `jal 0x80100784 (= Zustandsfunktion 1)` (a0 = Entity (addu a0,s3,zero @0x80102A60); a1 = s5 (@0x80102A64); a2 = s6 (@0x80102A6C)) — Zustand 2 ruft den Zustand-1-Handler im selben Frame direkt auf und schreibt gleichzeitig +0x1FC nach +0x04 (@0x80102A74, Delay-Slot).
- `@0x801017D0` -> `jal 0x8005BD6C (Gegner-SE)` (a0 = 10 (addiu a0,zero,10 @0x801017C0); a1 = Entity (addu a1,s0,zero @0x801017C4)) — SE beim Ablauf des +0x158-Timers, zusammen mit dem Zustandswort 0x0101 (@0x801017CC).
- `@0x80101734` -> `jal 0x8005BD6C (Gegner-SE)` (a0 = 10 (addiu a0,zero,10 @0x80101720); a1 = Entity (addu a1,s0,zero @0x8010172C)) — SE beim Setzen von +0x06 = 12 (@0x80101714/@0x80101718) und dem Zustandswort mit ori 0xA01 (@0x8010170C) in die Begleiter 0x800D3C38/0x800D3C40.
- `@0x80101450 / @0x80101474 / @0x8010149C / @0x801014C0` -> `jal 0x8005BD6C (Gegner-SE)` (a0 = 0 (addu a0,zero,zero @0x80101440/@0x80101464/@0x8010148C/@0x801014B0); a1 = Entity (addu a1,s0,zero); a2 = lw 15416/15420/15424/15428(s3) = 0x800D3C38/3C3C/3C40/3C44 (die vier EM37-Begleiter)) — SE + gleichzeitiges Setzen des Begleiter-Zustandsworts 0x0901 in den jeweiligen Delay-Slots @0x80101454/78/A0/C4. Schwellen davor: 40 (@0x8010143C) und 56 (@0x80101488) gegen lh 344(s0) = +0x158.
- `@0x80101660 / @0x80101684 / @0x801016AC / @0x801016D0` -> `jal 0x8005BD6C (Gegner-SE)` (a0 = 0; a1 = Entity; a2 = 0x800D3C38/3C40/3C3C/3C44) — Wie oben, aber Begleiter-Zustandswort 0x0A01 (Stores @0x80101664/88/B0/D4). Schwellen: 35 (@0x8010164C) und 50 (@0x80101698) gegen +0x158.

## Offen
- +0x1FC wird in EM36_ai1 NUR gelesen (@0x801029E8, @0x80102A68, @0x80102FFC), nie geschrieben. Geschrieben wird es ausschliesslich vom EXE-Schadensapplier (@0x80047278, @0x8004749C, @0x800418F4). Damit kann der Restore nur vorher gueltige Zustandsworte reproduzieren - ich habe aber NICHT bewiesen, dass +0x1FC beim Raumeintritt/Spawn genullt wird. Traegt es Muell, waere z5/z6/z7 ueber den Restore theoretisch erreichbar.
- Der Scan auf Schreibzugriffe nach +0x04 deckt info/re2leon/PSX.EXE und alle 112 build/extracted/re2_ems/CDEMD0|1_*_ai1.BIN ab (sb/sh/sw mit Offset 4, Immediate bis 16 Instruktionen rueckwaerts aufgeloest). NICHT gescannt: die Raum-Overlays von ROOM7040/ROOMG040 und deren RDT/SCD-Skript. Ein SCD-Opcode, der +0x04 setzt, waere von diesem Scan nicht erfasst. Die *_ai0.BIN sind derselbe Code gegen 0x8010D000 gelinkt und damit redundant.
- In der EXE bleiben 6 indirekte sw-Stores nach +0x04 uebrig, deren Quellregister ich nicht bis zu einem Immediate zurueckverfolgt habe. Ich habe nur belegt, dass keiner davon ein Immediate 5/6/7 traegt - nicht, welchen Wert sie tatsaechlich schreiben.
- Die Leser der Tabellen 0x800CE300[] / 0x800CE400[] sind in der RE2-PSX.EXE NICHT gefunden: die einzigen Treffer auf Immediate 0xE300 (@0x80038EB0, @0x800392AC, @0x800392C4, @0x800392E0, @0x800392F8) benutzen 0x800CE300 als BITFELD (and/or mit 0xFFFFDFFF bzw. 0xFFFF7FFF), nicht als Zeigerfeld; auf 0xE400 gibt es gar keinen Lesetreffer. Die SEMANTIK der beiden im INIT registrierten Callbacks (0x80103880, 0x801038CC) ist damit UNBELEGT. Registriert werden sie in genau 8 Typen: EM10, EM27, EM28, EM30, EM31, EM34, EM36, EM37.
- Warum der 0x800CE400-Callback (0x801038CC) seine Untertabelle bei 0x801056EC statt 0x801056E8 liest, ist ein Linker-Artefakt ueberlappender Tabellen. Ob +0x05 bei diesem Callback jemals != 0 wird, ist ungeprueft - bei +0x05 == 1 spraenge er nach 0x1000002B.
- +0x14C/+0x14D: Phase 0 schreibt 0x00070000 nach +0x14C, Phase 1 vergleicht +0x14D gegen 8, eine andere Stelle (@0x80101024) gegen 85. Die Feldaufteilung von +0x14C (Clip-Id vs. Frame) ist damit NICHT eindeutig belegt; der Auftragskopf nennt +0x94/+0x95, was hier nicht passt. Vor der Portierung dieser Vergleiche muss die +0x14C-Struktur ueber jal 0x8002959C aufgeloest werden.
- Zustand 4 (0x80103834) wurde mit demselben Verfahren wie 5/6/7 geprueft: die 6 EXE-Fundstellen fuer Zustand 4 sind @0x8005701C (Wert 0x1404, Ziel = lw 340(a0)), @0x80058E54 (Wert 0x0604), @0x8005D814 (sb 4, dazu sb 6 nach +0x05 @0x8005D818), @0x8005E1D8 (sb 4, dazu sb 20 nach +0x05 @0x8005E1E8), @0x8006B1F8, @0x8006C258. Alle setzen Unterzustaende 6, 0x14 oder 20 - die EM36-Untertabelle hat aber nur 2 Eintraege. Ob Zustand 4 fuer EM36 ueberhaupt legal erreichbar ist, ist damit OFFEN.
- Nicht zerlegt: der Root-Epilog ab @0x80100188. Er laeuft auch bei leerem Zustand-7-Handler weiter (+0x229 Dekrement @0x80100188-98, Zweig auf +0x226 & 0x8 @0x801001A4/@0x801001A8 nach 0x801002B8). Was der Boss waehrend +0x04 == 7 sichtbar tut, haengt an diesem Epilog und ist ungemessen.


---

# EM37 / Typ 0x37 — KI der vier Begleit-Tentakel des Endgegners Typ 0x36 (ROOM7040 / ROOMG040, Slots 1..4). Datei build/extracted/re2_ems/CDEMD0_EM37_ai1.BIN, Ladebasis 0x80100000 (die im Spiel resident geladene Kopie ist ai0 @0x8010D000; alle Adressen unten +0xD000 addieren). Wurzel 0x80100178.

Typ 0x37 ist KEIN eigenstaendiger Gegner, sondern ein am Boss 0x36 haengender 4-Segment-Tentakel: die Wurzel 0x80100178 setzt die Weltposition JEDEN Frame aus der Bone-Matrix des Bosses (Boss=*(0x800cfe1c), Matrix Boss->model+0x1A0, Trans +0x1B4/1B8/1BC) plus einem festen Rotations-Offset aus der Tabelle @0x80105668 (@0x80100230-88). HP := -1 (@0x80100530-34), er kann also nicht sterben — Treffer wirken nur ueber die Ruecksto&-Routine 0x801042c4. Bewegung entsteht ausschliesslich durch Rotation (+0x74 rotX, +0x76 rotY, +0x78 rotZ; ent+0x74 wird per 0x80104d64 in der (Y,-Z)-Ebene auf den Spieler *(0x800cfd90)+0x60/+0x64 gedreht) und durch Skalierung: model+0x8C 0..4096 = Auszugslaenge, per Root auf die Segmente +0x138/+0x1E4/+0x290 kopiert (@0x801003d0-ec). Die Zustandsmaschine ist zweistufig: ent+0x4 = Zustand (Tabelle @0x80105688, 42 Eintraege), Zustand 1 verzweigt ueber (+0x10E & 0x3F)+8 auf 0x80100934, das ent+0x5 = Substate 0..15 auf Tabelle[10+sub] (alle leer, `jr ra`) und Tabelle[26+sub] (der echte Handler = Tabelle[10+sub]+8) dispatcht; ent+0x6 = Phase. Angriffe: Substate 3 (Biss, 15 Schaden @0x801016d0), Substate 11 (Griff+Wegschleifen, 15 Schaden @0x80102d78), Substate 13 (Sturz-Angriff, 15 Schaden @0x80103730 bzw. 10 Schaden @0x801037e8); jeder Treffer setzt den Spieler-Griff-Handshake *(0x800cfdcb)|=0x80, *(0x800cfdac)=self, *(0x800cfbfc)=Spieler-Aktionswort. Getrieben werden die vier vom Boss: EM36 schreibt ueber FUN_80104e5c (alle vier) / FUN_80104e9c (einer) das Wort ent+0x4 = 0x0301 (Biss), 0x0B01 / 0x00070B01 (Griff), 0x0D01 (Sturz), 0x0601 (Einziehen), 0x0E01 (Tod/Aufloesung). Es werden genau 5 SE-Ids benutzt: 0, 1, 2, 3, 15.

## Konstanten

| Name | Wert | Bedeutung | Adresse |
|---|---|---|---|
| HP_INVULNERABEL | -1 (ent+0x156) | Feld/HP — Typ 0x37 ist unzerstoerbar | @0x80100530 addiu v0,zero,-1 / @0x80100534 sh v0,342(s1) |
| HITZONE_INIT | -126 (0xFF82) in +0x151/+0x152/+0x153 | Feld (Trefferzonen-Init) | @0x80100538-44 |
| STATE_TAB | 0x80105688, 42 Eintraege, [5]=[6]=0 | Feld (Zustands-Dispatch, ent+0x4) | @0x801003a4 |
| SUB_TAB_PRE | 0x801056b0 (=STATE_TAB[10]) — alle 16 Eintraege `jr ra` | Feld (leerer 1. Durchlauf) | @0x80100964 |
| SUB_TAB_ACT | 0x801056f0 (=STATE_TAB[26]) — jeweils PRE+8 | Feld (echter Substate-Handler) | @0x8010098c |
| ATTACH_OFF idx0 | X=965 Y=-2243 Z=-1933 (4. HW 0) | Distanz (Ansatzpunkt am Boss) | @0x80105668 (gelesen @0x80100250) |
| ATTACH_OFF idx1 | X=1615 Y=-706 Z=-2440 (4. HW 0) | Distanz | @0x80105670 |
| ATTACH_OFF idx2 | X=1409 Y=-683 Z=2495 (4. HW 2048) | Distanz | @0x80105678 |
| ATTACH_OFF idx3 | X=957 Y=-2426 Z=1958 (4. HW 2048) | Distanz | @0x80105680 |
| ATTACH_Y_BIAS_A | -1300 (idx 0 und 3) | Distanz (Y-Versatz bei +0x219==2) | @0x80100318 |
| ATTACH_Y_BIAS_B | -1500 (idx 1 und 2) | Distanz | @0x80100338 |
| CLIP_EMERGE[4] | {14, 14, 16, 15} | Clip (s8 p0, pro Begleiter) | @0x80100048 (gelesen @0x80101cf4 / @0x80101d44) |
| BISS_PITCH[4] | {896, 1088, 2976, 3200} (ent+0x74) | Feld (Winkel, s3 p0) | @0x80105738 (gelesen @0x80101454) |
| S12_PITCH_OFFSET | BISS_PITCH[idx] - 1024 | Feld (Winkel, s12 p0) | @0x801033b4 |
| BISS_FRAMEFENSTER var0 | {30, 55} und {60, 70} (exklusiv, gegen ent+0x14D) | Frame (s3 p2/p3) | @0x80105730..0x80105733 |
| BISS_FRAMEFENSTER var1 | {38, 50} und {52, 60} | Frame | @0x80105734..0x80105737 |
| S10_ROT[4] (Tab A) | idx0(768,0,32) idx1(1536,0,32) idx2(2560,0,32) idx3(3584,0,32) | Feld (+0x74/76/78, s10 p0) | @0x80105740 + idx*12 (gelesen @0x801027a8) |
| S9_ROT[4] (Tab B) | idx0(2304,0,128) idx1(2816,0,128) idx2(64,0,-64) idx3(1920,0,128) | Feld (+0x74/76/78, s9 p0) | @0x80105746 + idx*12 (gelesen @0x801020b8) |
| S11_ROT[16] | (1920,0,128)(64,0,-64)(64,0,-64)(1536,0,128)(2048,0,128)(2560,0,128)(896,0,128)(2048,0,128)(2944,0,128)(3072,0,128)(896,0,128)(1280,0,128)(2304,0,128)(2816,0,128)(64,0,-64)(1920,0,128) | Feld (+0x74/76/78, Index = idx + (rng&3)*4) | @0x80105770 + n*8 (gelesen @0x80102aac) |
| S13_TAB[4] | idx0(clip_lo=23, rotZ=-128) idx1(21,-256) idx2(21,-256) idx3(23,-128) | Clip / Feld (s13 p0/p1) | @0x801057f0 + idx*6 (gelesen @0x80103578, @0x80103650) |
| BOSS_PTR | *(0x800cfe1c) | Feld (Endgegner-Entity) | @0x80100234-3c, @0x80101464 |
| COMPANION_PTR[4] | 0x800cfe20, 0x800cfe24, 0x800cfe28, 0x800cfe2c | Feld (Slot-Zeiger; Scan bestimmt ent+0x218) | @0x80100634 (Scan) / EM36 @0x80104e60-90 |
| COMPANION_OBJ[4] | 0x800cfe30 + idx*4 | Feld (verknuepftes Objekt, Flags |1 / |0x100000 / |2) | @0x80100414, @0x80100440, @0x801039dc, @0x801040d0 |
| PARENT_PTR (Kind) | *(0x800cfe10 + idx*4) | Feld (Kind-Entity holt Matrizen des Eltern-Tentakels) | @0x80104f84 |
| PLAYER_WORK | *(0x800cfd90); Pos X=+0x5C Y=+0x60 Z=+0x64 | Feld (Ziel aller Dreh-/Abstandsrechnungen) | @0x80100da8, @0x8010190c, @0x80101c70 |
| PLAYER_ENTITY | 0x800CFBF8 (Aktionswort = 0x800cfbfc) | Feld | @0x801016fc (a1=s1-467), @0x80102d88, @0x80104f30 |
| PLAYER_GRAB_FLAG | *(u8*)0x800cfdcb, Bit 0x80 | Feld (Spieler ist gegriffen; Gate fuer alle Griffe) | @0x80100fd4, @0x80100ff8, @0x8010171c-70, @0x80102d68, @0x80103720 |
| PLAYER_GRABBER | *(u32*)0x800cfdac = self | Feld | @0x80101000, @0x80101768, @0x80102da0, @0x80103744 |
| GRAB_ANCHOR | *(0x800cfd80)=ent+0x188, *(0x800cfd84)=ent+0x18C | Feld (Anhaengepunkt) | @0x80101780/@0x8010178c, @0x80102db8/@0x80102dd0, @0x80103758/@0x80103770 |
| PLAYER_ACT s2-Griff | 514 (0x0202) | Feld (Spieler-Zustandswort) | @0x80100fe8, @0x801011a8 |
| PLAYER_ACT s3 nah | 3 (bei Rueckgabe 2 von 0x800401d4), sonst 5 oder 261 (0x105) | Feld | @0x801016e4, @0x80101710 |
| PLAYER_ACT s3 fern | 517 (0x205) ab Distanz ent+0x1F0 >= 6001 (0x1771) | Distanz / Feld | @0x8010171c sltiu 0x1771 / @0x80101724 addiu 517 |
| GRAB_PARAM_0x800cfc6e | 3456, bzw. 640 wenn ent+0x218 < 2 | Distanz | @0x80101730, @0x8010174c |
| PLAYER_ACT s11 | 5 oder 261 (0x105) | Feld | @0x80102dc0 |
| PLAYER_ACT s13 A | 773 (0x305) | Feld | @0x80103760 |
| PLAYER_ACT s13 B | 1029 (0x405) | Feld | @0x80103818 |
| S13_GATE_B | *(s16*)0x800cfd4e >= 0 | Feld (Vorbedingung fuer den 10er-Schaden) | @0x801037d8 lh v0,-690(v0) / @0x801037e0 bltz |
| DAMAGE_BISS | 15 (a1=0) | Schaden | @0x801016cc / @0x801016d0 |
| DAMAGE_SCHLEIF | 15 (a1=1) | Schaden | @0x80102d74 / @0x80102d78 |
| DAMAGE_STURZ_A | 15 (a1=1) | Schaden | @0x8010372c / @0x80103730 |
| DAMAGE_STURZ_B | 10 (a1=1) | Schaden | @0x801037e4 / @0x801037e8 |
| WELT_Z_MIN | -27000 (Tip.Z darunter -> +0x76 -= 32 bzw. Abbruch) | Distanz | @0x80101024, @0x801010f4, @0x801022f8, @0x80102d04 |
| WELT_Z_MAX | -19799 (Tip.Z darueber -> +0x76 += 32 bzw. Abbruch) | Distanz | @0x80101048, @0x80101118, @0x80102320, @0x80102d2c |
| WELT_Y_LIMIT_s2 | -399 (Tip.Y darunter -> Rueckzugsphase) | Distanz | @0x80100f78, @0x8010113c |
| WELT_Y_LIMIT_s9_s11 | -5000 | Distanz | @0x801022d0, @0x80102cdc |
| TIP_LAENGE | (1500, 0, 0), rotiert mit model+0x24C, plus model+0x264/+0x268 | Distanz (Spitzenposition des Tentakels) | @0x80100f3c/@0x80100f4c, @0x80102270/@0x80102280, @0x80102c7c/@0x80102c8c |
| SCALE_MAX | 4096 (model+0x8C, voll ausgefahren) | Feld (Auszugslaenge) | @0x80100c74, @0x80100ed8, @0x8010154c, @0x80101b48, @0x80102210 |
| SCALE_SCHWELLE_2049 | 2049 (s1 p1, s2 Zwischenmarke, s5 p1); 2048 in s8 p8 | Feld | @0x80100c2c, @0x80100e44, @0x80101af8, @0x80102004 |
| SCALE_SCHWELLE_3073 | 3073 (s3 p1, s9 p1, s11 p1) | Feld | @0x801014ac, @0x80102184, @0x80102b90 |
| SCALE_SCHWELLE_3549 | 3549 (s2 p1) | Feld | @0x80100e20 |
| SCALE_SCHWELLE_3573 | 3573 (s13 p1) | Feld | @0x80103628 |
| SCALE_SCHWELLE_2025 | 2025 (s3 p1, Beginn Pitch-Anhebung) | Feld | @0x801014e0 |
| ANIM_SPEED | 0x20 / 0x40 / 0x7D(125) / 0x80 / 0x100 / 0x200 / 0x400 / 0x1200 | Dauer (a3 von jal 0x8002959c) | @0x80103fa4(32) @0x80100dec(64) @0x80100cd0(125) @0x80101524(128) @0x80101db8(256) @0x80101fb8(512) @0x80104514(1024) @0x80104674(0x1200) |
| TIMER_s1p2 | (rng&0xF)+10 | Dauer | @0x80100c88-8c |
| TIMER_s2p2_p3 | (rng&0xF)+30 bzw. (rng&0xF)+20 | Dauer | @0x80100fa4 / @0x8010116c |
| TIMER_s0 | (rng&0x3F)+60 | Dauer | @0x80100a80-88 |
| TIMER_s4_s10 | (rng&0xF)+60 bzw. (rng&0xF)+30 | Dauer | @0x801018f8, @0x801019dc / @0x80102a08 |
| TIMER_s14 | (rng&0x1F)+100 ; s14 p4-Ende: 150 | Dauer | @0x80103b20-28 / @0x80103d70 |
| CUTSCENE_s8_FRAMES | p4: t<5 X-=t*10, 6<=t<=34 X+=10, t==36 -> p5 | p6: t<20 X-=t, 16<=t<=179 Jitter, t>=161 X+=5, t==180 -> p7 | Frame / Distanz | @0x80101e2c(5) @0x80101e54(-6/0x1d) @0x80101e78(36) @0x80101e9c(20) @0x80101ec0(-16/0xa4) @0x80101f64(161) @0x80101f84(180) |
| CUTSCENE_s9_FRAMES | p7: t<10 Gelenke, 8<=t<=174 Jitter, t>=166 Rueckbiegung, t==180 -> p8 | Frame | @0x80102510(10) @0x8010257c(-8/0xa7) @0x80102620(166) @0x8010266c(180) |
| CUTSCENE_s11_FRAMES | p7: t<10, 6<=t<=79 Jitter, t>=81 Rueckbiegung, t==90 -> p8; p5-Ende +0x21A = ent.X + 8192 | Frame / Distanz | @0x80103018(10) @0x80103084(-6/0x4a) @0x80103128(81) @0x80103174(90) @0x80102e5c(8192) |
| GELENK_CLAMP | Summe(+0x78 + model+0x6C + +0x118 + +0x1C4 + +0x270) & 0xFFF, verboten wenn (Summe-2049) unsigned < 0x7FF | Feld (Biegungs-Anschlag) | @0x801012b0-b8, @0x80102484-8c, @0x80102f20-28 |
| JITTER_AMPLITUDE | (rng & 0xF), Vorzeichen (Timer&1)?-1:+1, auf ent.X/Y/Z | Distanz | @0x80101ed8-f58, @0x80102594-614, @0x8010309c-11c |
| ANIM_FELD +0x14C | 32 Bit: [31:16]=Maske (0x00/0x03/0x07/0x0F/0x1F/0x3F/0x7F), [15:8]=Startframe, [7:0]=Clip | Clip | Beleg: @0x801018e8 addu s0,s0,v0 mit s0=(rng%90)<<8 und v0=0x003F0009+(rng&3) |
| CLIPS s0/s1/s2 | s0: 0x003F0000 | s1: 9, dann 0x000F0000 | s2: 0, 0x003F0001, 0x003F0009, 0x003F0000 | Clip | @0x80100a64 / @0x80100b18, @0x80100cb4 / @0x80100d84, @0x80100e28, @0x80100e5c, @0x80101200 |
| CLIPS s3/s5/s6/s7 | s3: 0, 0x001F0007+(rng&1), 0x001F0000 | s5: 0, 0x001F0009 | s6: 0x001F0000 | s7: 0x001F0007 | Clip | @0x80101414, @0x801014c0, @0x801015e4 / @0x80101a70, @0x80101b00 / @0x80101bb4 / @0x80101c54 |
| CLIPS s9/s10/s11 | s9: 0, 0x001F0002, 0x003F0000 | s10: 0, 0x001F0009+(idx%3), 0x003F0009+(rng%90)<<8+(rng%3), 0x001F0007+(rng&1) | s11: 0, 0x001F0002, 0x003F0000 | Clip | @0x801020a0, @0x80102190, @0x80102398 / @0x80102774, @0x80102834, @0x80102924, @0x8010298c / @0x80102a8c, @0x80102b98, @0x80102e48 |
| CLIPS s12/s13/s14/s15 | s12: 0, 0x001F0016, 0x003F0000 | s13: 0, 0x001F0000+Tab, 0x003F0000 | s14: 0x001F0011+(rng&3 bzw. &1), 0x003F0013+(rng&1), 0x007F0009+(rng%3) | s15: 13 | Clip | @0x80103374, @0x8010340c, @0x8010345c / @0x80103544, @0x80103654, @0x80103888 / @0x80103a34, @0x80103bec, @0x80103ec4 / @0x80104084 |
| BOSS_FLAG_ANGRIFF | Boss+0x228 |= 1 << (idx+4) | Feld (Begleiter i greift an) | setzen @0x80101470-78, @0x801033cc, @0x801035c0, @0x801039b4 ; loeschen @0x80101804, @0x801034a8, @0x801038dc, @0x80102b3c |
| BOSS_FLAG_RUMBLE | Boss+0x223 |= 2 << idx (nur solange ent+0x222 != 0) | Feld | @0x801054e8, @0x8010552c |
| SHAKE_TIMER | ent+0x222 = 7 | Dauer | @0x80101e1c-20, @0x80102374-78, @0x80102e20-24 |
| COOLDOWN +0x1D3 | untere 7 Bit werden pro Frame dekrementiert, Bit 0x80 separat | Dauer | @0x801001cc-e0 |
| HITBIT_QUELLE | ent+0x220 Bit1/Bit2 = FUN_80034d0c(0x800CFBF8, ent) << 1 | Feld (Treffer-Erkennung Spieler<->Segment) | @0x80104f38-4c |
| SCALE_PROPAGATION | model+0x138 = +0x1E4 = +0x290 = model+0x8C, ausser (ent+0x220 & 1) | Feld (4 Segmente gleich lang) | @0x801003bc-ec |
| UNREFERENZIERT_A | [300,300,250,450,300,300,220,300,400,300,400,350,180,340,220,240,175,240,310,240,310,300,250,200] | Feld (24 s16, aus EM37-Code NICHT gelesen) | @0x80105608 |
| UNREFERENZIERT_B | [240,240,250,450,300,300,220,300,400,300,400,350,180,340,220,240,175,240,310,240,310,300,250,200] | Feld (24 s16, aus EM37-Code NICHT gelesen) | @0x80105638 |
| MESH_DESKRIPTOREN | 0x801058b8, 0x80105940, 0x801059b8 (je mit Datenzeiger bei +0x34 auf 0x80105840 / 0x801058f0 / 0x80105978) | Feld (3 Modellteile, registriert in Zustand 0) | @0x80104ca0, @0x80104cb4, @0x80104ccc ; Relokations-Beleg @0x801058ec/@0x80105974/@0x801059ec |
| KIND_ENTITY | Typ 52 (0x34), +0x10E = 8 | Feld (jeder Tentakel spawnt ein Kind, das nur Matrizen erbt) | @0x80100880 addiu a0,zero,52 / @0x801008ac sh 8,270(a0) |
| FREEZE_GATE | *(u32*)0x800cfbdc & 0x20000000 | Feld | @0x80100188 / @0x80100194 |
| AI_TABELLEN_RELOKATION | nur 0x80105688..0x8010572f, 0x8010582c..0x8010583f, 0x801058ec, 0x80105974, 0x801059ec (Delta 0xD000) | Feld (Beweis, welche .rodata-Worte Zeiger sind) | Diff CDEMD0_EM37_ai0.BIN gegen ai1.BIN, 362 abweichende Worte |

## Uebergaenge

- **root 0x80100178 (Eintritt) -> return, ent[0] &= ~2** wenn ent+0x10E == 8 (Kind-Entity, Typ 52) — dann NUR jal 0x80104f64 (Matrizen vom Eltern-Tentakel *(0x800cfe10+idx*4) kopieren)
  - @0x801001a0 lhu v1,270(s1) / @0x801001a4 addiu v0,zero,8 / @0x801001a8 bne v1,v0,0x801001cc / @0x801001b0 jal 0x80104f64 / @0x801001c0 and v0,v0,-2
- **root 0x80100178 -> sofortiges return (globaler Freeze)** wenn *(u32*)0x800cfbdc & 0x20000000
  - @0x80100184-98 lui v0,0x800d / lw v0,-1060(v0) / lui v1,0x2000 / and v0,v0,v1 / bne v0,zero,0x8010049c
- **root, Attach-Phase +0x219 == 0 -> ent.pos(+0x38/+0x3C/+0x40) = Boss->model+0x1B4/1B8/1BC + Rot(off[idx])** wenn ent+0x4 != 0 && ent+0x219 == 0; off[idx] aus Tabelle @0x80105668 (8 B Stride)
  - @0x801001e4 lbu v0,4(s1) / @0x801001f4 lbu v1,537(s1) / @0x80100230-84 lwl/lwr 0x80105668+idx*8 -> sp+16 / @0x80100348 jal 0x8008dba4 (a0=Boss->model+0x1A0) / @0x80100350-88 sw -> 56/60/64(s1)
- **root, Attach-Phase +0x219 == 2 -> wie oben, aber off.Z /= 2 und off.Y = off.Y/2 - 1300 (idx 0,3) bzw. -1500 (idx 1,2)** wenn ent+0x219 == 2
  - @0x801002e4-1c sra v1,v1,1 (Z/2) / sra v1,v0,1 (Y/2) / addiu v0,v1,-1300 / @0x80100328-38 addiu v0,v0,-1 / sltiu v0,v0,0x2 / addiu v0,v1,-1500
- **root, Zustands-Dispatch -> handler = *(0x80105688 + ent+0x4 *4)(ent, ent+0x108, ent+0x17C)** wenn immer; Tabelle: [0]=0x801004f0 [1]=0x801008f4 [2]=0x80104184 [3]=0x801041e4 [4]=0x8010423c [5]=0 [6]=0 [7]=0x80104280
  - @0x8010038c lw a1,264(s1) / @0x80100390 lbu v0,4(s1) / @0x80100394 lw a2,380(s1) / @0x801003a4 lw v0,22152(at) ;0x80105688 / @0x801003ac jalr v0
- **Zustand 1 (0x801008f4) -> handler = *(0x80105688 + ((ent+0x10E & 0x3F)+8)*4); Varianten 0 und 1 -> beide 0x80100934** wenn 0x80100934 ruft dann *(0x801056b0+sub*4) [immer nur `jr ra`] UND *(0x801056f0+sub*4) [= voriger +8, der echte Handler], sub = ent+0x5
  - @0x801008fc lhu v0,270(a0) / @0x80100904 andi v0,v0,0x3f / @0x80100914 lw v0,22184(at) ;0x801056a8 / @0x80100950 lbu v0,5(s0) / @0x80100964 lw 22192(at) ;0x801056b0 / @0x8010098c lw 22256(at) ;0x801056f0
- **Zustand 0 (INIT, 0x801004f0) -> z1, HP=-1, Index +0x218 = Slot-Nummer** wenn beim Spawn
  - @0x8010052c sw v0(=1),4(s1) / @0x80100534 sh -1,342(s1) [HP] / @0x80100640-58 Scan 0x800cfe20[n] auf self, s0=3+n / @0x80100664 sb s0-3,536(s1)
- **Zustand 0 Ende -> z1 s8 p0 (Regelfall) bzw. z1 s15 p0 +0x219=1 (Variante 1)** wenn vorher wird Kind-Entity Typ 52 mit +0x10E=8 gespawnt und rekursiv initialisiert
  - @0x80100880 addiu a0,zero,52 / @0x8010088c jal 0x801050bc / @0x80100890 sb 8,5(s1) / @0x801008a8 jal 0x801004f0 / @0x801008b0-cc andi 0xfff / bne v0,1 / sb 15,5(s1) / sb 1,537(s1)
- **z1 s0 p0 (IDLE, 0x80100a44) -> z1 s0 p1 (terminal, wartet auf Boss-Befehl)** wenn p==0
  - @0x80100a64 sw 0x003F0000,332(s0) / @0x80100a70 sb 1,6(s0) / @0x80100a74 andi v0,v0,0xfd [+0x1C0] / @0x80100a8c sh (rng&0x3F)+60,344(s0) / @0x80100a98 model[0] &= ~1
- **z1 s1 p0/p1 (0x80100b14, Auftauchen+Zielen) -> p2** wenn model+0x8C >= 2049 (Wachstum +256/Frame, Anim-Speed 0x40)
  - @0x80100b18 sw 9,332(s1) [Clip 9] / @0x80100bc0 jal 0x8008d190 -> sh v0,120(s1) / @0x80100bf4 jal 0x800154ac -> sh v0,116(s1) / @0x80100c1c addiu v0,v0,256 / @0x80100c2c slti v0,v0,2049 / @0x80100c34 sb 2,6(s1) / 0x80100cb4 sw 0x000F0000,332
- **z1 s1 p2 -> p3, +0x158 = (rng&0xF)+10** wenn model+0x8C >= 4097 -> clamp 4096 (Anim-Speed 0x100, +256/Frame)
  - @0x80100c4c jal 0x8002959c a3=256 / @0x80100c6c slti 4097 / @0x80100c78 sh 4096 / @0x80100c84 sb 3,6(s1) / @0x80100c8c andi 0xf / addiu 10
- **z1 s1 p3 -> p4 -> z1 s0 p0 -> Ende** wenn p3: Timer +0x158 laeuft auf 0; p4: model+0x8C -= 512 (Anim-Speed 125) < 0
  - @0x80100c98-cb0 lhu 344 / bne / sb 4,6(s1) / @0x80100cd0 addiu a3,zero,125 / @0x80100cdc addiu v0,v0,-512 / @0x80100ce8 bgez / @0x80100cf4 sw 1,4(s1)
- **z1 s2 p0/p1 (0x80100d78, Ausstrecken zum Spieler) -> p2** wenn model+0x8C >= 3549; Zwischenmarke: >= 2049 && +0x16B==0 -> +0x14C=0x003F0009, +0x16B=1
  - @0x80100db8 jal 0x80015558 a3=2048 (Yaw auf Spieler X/Z) / @0x80100dc0 sh -64,120(s1) / @0x80100dcc sh 40,346(s1) / @0x80100e20 slti 3549 / @0x80100e30 sb 2,6(s1) / @0x80100e44 slti 2049
- **z1 s2 p2/p3 -> p4 (+0x16A=1, +0x15A=64, +0x158=(rng&0xF)+30 bzw. +20)** wenn Tip = Rot((1500,0,0), model+0x24C) + model+0x264/+0x268: Tip.Y < -399 && +0x16A==0. Zusaetzlich Weltgrenzen: Tip.Z < -27000 -> +0x76 -= 32; Tip.Z >= -19799 -> +0x76 += 32
  - @0x80100f28 jal 0x80104ba4 / @0x80100f3c sh 1500,16(sp) / @0x80100f4c jal 0x8008dba4 (a0=model+588) / @0x80100f78 slti v1,v1,-399 / @0x80100fa4 addiu 30 / @0x80101024 slti -27000 / @0x80101048 slti -19799
- **z1 s2 p2/p3 GRIFF -> Spieler-Aktion 0x202** wenn (ent+0x220 & 2) && *(u8*)0x800cfdcb == 0
  - @0x80100fc4 andi v0,v0,0x2 / @0x80100fd8 lbu v1,0(a2) ;0x800cfdcb / @0x80100fe8 addiu v0,zero,514 / @0x80100ff0 sw v0,-1028(at) ;0x800cfbfc / @0x80100ff8 ori 0x80 / @0x80101000 sw s1,0x800cfdac / @0x80101004 jal 0x8003947c(3,0) / @0x80101014 jal 0x80039514(10,250,0)
- **z1 s2 p4 -> p5 -> z1 s0 p0 -> Ende** wenn p4: Timer 0 -> p5 (+0x14C=0x003F0000, +0x158=8), Gelenk-Biegung model+0x6C/+0x118/+0x1C4/+0x270 mit Clamp Summe&0xFFF in [2049,4095]; p5: model+0x8C -= +0x158 (+8/Frame) < 0
  - @0x801011e4-20c / @0x80101210-64 sh +/-346 / @0x8010127c jal 0x80104c30 / @0x801012b4 addiu v0,v0,-2049 / @0x801012b8 sltiu v0,v0,0x7ff / @0x8010137c bgez / @0x80101388 sw 1,4(s1)
- **z1 s3 p0/p1 (BISS, 0x80101404) -> p2, +0x16B = rng&1, +0x14C = 0x001F0007 + (rng&1)** wenn model+0x8C >= 3073 (+64/Frame, Anim 0x40). p0 setzt +0x78=-576, +0x76=0, +0x74=Tab[idx]@0x80105738, Boss+0x228 |= 1<<(idx+4), SE 0
  - @0x8010143c sh -576,120(s0) / @0x80101454 lhu v0,22328(at) ;0x80105738 / @0x80101470-78 sllv/or -> sb v1,552(a3) [Boss+0x228] / @0x80101478 jal 0x8005bd6c a0=0 / @0x801014ac slti 3073 / @0x801014cc sb (rng&1),363(s0) / @0x801014d0 addu 0x001F0007
- **z1 s3 p2 -> p3, +0x158 = (rng&0xF)+10** wenn model+0x8C >= 4097 -> clamp 4096 (Anim 0x80). Frame-Fenster: 0x80105730[var*4+0] < ent+0x14D < [+1] -> +0x78 += 8
  - @0x80101544 slti 4097 / @0x8010155c sb 3,6(s0) / @0x80101580 lbu v0,22320(at) ;0x80105730 / @0x8010159c lbu v0,22321(at) / @0x801015b8 addiu v0,v0,8
- **z1 s3 p3 TREFFER -> Spieler 15 Schaden + Griff** wenn Fenster [0x80105732],[0x80105733] -> +0x78 += 4; dann (ent+0x220 & 4) && *(0x800cfdcb)==0 && Boss->+0x156 >= 0
  - @0x80101654-7c lbu 0x80105732/33 / @0x80101688 andi v1,v1,0x4 / @0x801016a4 lbu v0,0(s1) ;0x800cfdcb / @0x801016c0 lh v0,342(v0) [Boss HP] / @0x801016cc addiu a0,zero,15 / @0x801016d0 jal 0x800401d4 (a1=0)
- **z1 s3 p3 TREFFER-Folge -> Spieler-Aktionswort** wenn Rueckgabe==2 -> 3; sonst 5 oder 0x105 je FUN_80015910(ent,0x800CFBF8); wenn ent+0x1F0 >= 0x1771 (6001) -> 0x205 und *(u16)0x800cfc6e = 3456 (bzw. 640 falls idx < 2)
  - @0x801016dc bne v0,3(=2) / @0x801016ec sw 3,0x800cfbfc / @0x801016f8 jal 0x80015910 a1=0x800CFBF8 / @0x80101710 addiu v0,v0,5 / @0x8010171c sltiu v1,v1,0x1771 / @0x80101724 addiu 517 / @0x80101730 addiu 3456 / @0x80101744 sltiu v0,v0,0x2 / @0x8010174c addiu 640
- **z1 s3 p4 -> z1 s0 p0 -> Ende, Boss+0x228 &= ~(1<<(idx+4))** wenn model+0x8C -= 64 (Anim 0x80) < 0
  - @0x801017c8 jal 0x8002959c a3=128 / @0x801017d8 addiu v0,v0,-64 / @0x801017e4 bgez / @0x801017f4 sw 1,4(s0) / @0x80101804-14 sllv/nor/and -> sb v0,552(a0)
- **z1 s4 p0/p1 (Zufalls-Idle, 0x80101840) -> bleibt in p1 (Endlosschleife)** wenn p0: +0x14C = 0x003F0009 + ((rng1&0xFF)%90)<<8 + (rng2&3), +0x15A=(rng&0xF)+60, +0x16A=3. p1: 0x80104d64(ent, Spieler+0x60, Spieler+0x64, Rate 8) + Anim 0x40; Timer +0x15A auf 0 -> neues +0x14C (Modulo 3 statt 4) und +0x158=(rng&0xF)+60
  - @0x80101884-f0 zwei jal 0x80015fe8, magic 0xB60B60B7>>5 = /90, lui a1,0x3f|ori 9 / @0x8010191c jal 0x80104d64 a3=8 / @0x80101944 bne / @0x8010197c magic 0xAAAAAAAB = /3 / @0x801019e0 sh (rng&0xf)+60,344
- **z1 s5 (0x80101a0c) -> z1 s4 p0** wenn p0: +0x74 = ratan2(entY,-entZ,SpielerY,-SpielerZ), +0x78=-32; p1: model+0x8C += 64 bis >= 2049 -> p2 (+0x14C=0x001F0009); p2: += 64 bis >= 4097 -> clamp 4096 und ent+0x4 = 0x401
  - @0x80101a90 sh -32,120(s0) / @0x80101ac0 jal 0x800154ac / @0x80101af8 slti 2049 / @0x80101b00 lui 0x1f|ori 9 / @0x80101b40 slti 4097 / @0x80101b50 addiu v0,zero,1025 / @0x80101b54 sw v0,4(s0)
- **z1 s6 (EINZIEHEN, 0x80101b80) / z1 s7 (0x80101c14) -> s6 -> z1 s0 p0 ; s7 -> z1 s4 p0** wenn s6 p0: +0x14C=0x001F0000; p1: Anim 0x80, model+0x8C -= 64 < 0 -> 0, ent+0x4 = 1. s7 p0: +0x14C=0x001F0007; p1: 0x80104d64(ent, Spieler+0x60-500, Spieler+0x64, Rate 64), Anim 0x80 != 0 -> ent+0x4 = 0x401
  - @0x80101bb4 lui v0,0x1f / @0x80101bc8 jal 0x8002959c a3=128 / @0x80101bd8 addiu v0,v0,-64 / @0x80101bf0 sw 1,4(s0) || @0x80101c54 lui 0x1f|ori 7 / @0x80101c80 jal 0x80104d64 / @0x80101c84 addiu a1,a1,-500 / @0x80101c9c beq v0,zero / @0x80101ca0 addiu v0,zero,1025
- **z1 s8 (AUFTAUCH-CUTSCENE, 0x80101ccc, 10 Phasen @0x8010004c) -> p0 (terminal) -> p1 -> p2 -> p3 -> p4 -> p5 -> p6 -> p7 -> p8 -> p9** wenn p0: +0x14C = Byte[0x80100048+idx] = {14,14,16,15}, bei idx==1 zusaetzlich model[0]|=0x4001 und +0x219=2. p1: SE 0, +0x219=1, p=2. p2: p += Anim(0x100)-Rueckgabe, model+0x8C += 256 clamp 4096. p3: SE 2, +0x14C=0x00070000, +0x222=7, p=4. p4: t<5 -> X -= t*10; 6<=t<=34 -> X += 10; t==36 -> p=5. p5: Timer=0. p6: t<20 -> X -= t; 16<=t<=179 -> X/Y/Z += (rng&0xF)*(t&1?-1:+1); t>=161 -> X += 5; t==180 -> p=7. p8: Anim 0x200, model+0x8C -= 64, +0x76 +=16 (idx>=2) / -=16 (idx<2), < 2048 -> 0, SE 0, p=9
  - @0x80101cf4 lwl/lwr 0x80100048 / @0x80101d44 lbu v0,24(v0) / @0x80101d6c ori 0x4001 / @0x80101da4 jal 0x8005bd6c a0=0 / @0x80101dcc addu v1,v1,v0 / @0x80101e04 lui 0x7 / @0x80101e20 sb 7,546 / @0x80101e7c bne v1,36 / @0x80101ec4 sltiu 0xa4 / @0x80101f84 addiu 180 / @0x80101fb8 a3=512 / @0x80102004 slti 2048
- **z1 s9 (GRIFF-CUTSCENE, 0x8010204c, 10 Phasen @0x80100074) -> p0 -> p1 -> p2 -> p3 -> p4 -> p5 -> p6 -> p7 -> p9** wenn p0: +0x74/76/78 = Tab-B @0x80105746+idx*12, +0x15A=64, +0x219=2, +0x1E8=4, SE 0. p1: model+0x8C += +0x158, +0x158+=8, >=3073 -> p2 (+0x14C=0x001F0002); +0x78 -= +0x15A, +0x15A -= 4. p2: >=4097 -> clamp, SE 15, p=3. p2/p3 Flucht-Test (Tip.Y>0 | Tip.Y<-5000 | Tip.Z<-27000 | Tip.Z>=-19799) && +0x16A==0 -> p=4. p4: +0x15A=64,+0x158=60,+0x16A=1,+0x219=1,SE 2,+0x222=7,p=5. p5: Timer 0 -> p6, +0x21A=model+0x260-model+0x5C, +0x21C=model+0x8C, +0x21E=model+0x5C. p7: t<10 Gelenke, 8<=t<=174 Jitter, t>=166 Rueckbiegung, t==180 -> p=8. p9: Anim 0x40, model+0x8C -= +0x158 (+4/Frame), +0x78 += 8, <0 -> SE 0, p=10 (ausserhalb der Tabelle)
  - @0x801020b8 lhu 22342(at) ;0x80105746 / @0x8010213c jal SE a0=0 / @0x80102184 slti 3073 / @0x80102200 slti 4097 / @0x8010221c jal SE a0=15 / @0x801022ac blez / @0x801022d0 slti -5000 / @0x801022f8 slti -27000 / @0x80102320 slti -19799 / @0x8010236c jal SE a0=2 / @0x801023a8-d0 sh +0x21A/1C/1E / @0x80102578 sltiu 0xa7 / @0x8010266c addiu 180 / @0x801026ec jal SE a0=0
- **z1 s10 (0x8010271c, 6 Phasen @0x8010009c) -> p0 -> p1 -> p2 -> p3 (Schleife); p4 -> p5 -> p3** wenn p0: +0x74/76/78 = Tab-A @0x80105740+idx*12, +0x219=0, SE 0. p1: model+0x8C += 64 >= 2049 -> p2, +0x14C = 0x001F0009 + (idx%3). p2: += 64 >= 4097 -> clamp, p3, +0x158=(rng&0xF)+30. p3: Timer 0 -> neues Zufalls-+0x14C (0x003F0009 + (rng%90)<<8 + rng%3) und +0x158=(rng&0xF)+60. p4: p=5, +0x14C=0x001F0007+(rng&1). p5: Anim 0x80 != 0 -> p=3, +0x14C=0x003F0009+(rng%3), +0x158=(rng&0xF)+30
  - @0x801027a8 lhu 22336(at) ;0x80105740 / @0x801027f8 jal SE a0=0 / @0x8010282c slti 2049 / @0x80102844 magic 0xAAAAAAAB / @0x801028a4 slti 4097 / @0x801028e0 bne / @0x80102908 magic 0xB60B60B7 / @0x80102994 andi 0x1 / @0x801029b4 beq v0,zero / @0x80102a08 addiu 30
- **z1 s11 p0..p3 (SCHLEIF-GRIFF, 0x80102a38, 11 Phasen @0x801000b4) -> p1 -> p2 -> p3** wenn p0: +0x74/76/78 = Tab @0x80105770 + (idx + (rng&3)*4)*8, +0x15A=64, +0x219=0, SE 0, Boss+0x228 &= ~(1<<(idx+4)). p1: model+0x8C += +0x158 (+8), >=3073 -> p2 (+0x14C=0x001F0002). p2: += +0x158 (+16), >=4097 -> clamp, SE 15, p3. p2/p3: 4 Flucht-Tests wie s9 -> p=4
  - @0x80102a90 andi v0,v0,0x3 / @0x80102aac lhu 22384(at) ;0x80105770 / @0x80102b20 jal SE a0=0 / @0x80102b3c-4c sllv/nor/and 552(a0) / @0x80102b90 slti 3073 / @0x80102c0c slti 4097 / @0x80102c28 jal SE a0=15 / @0x80102cb8 blez / @0x80102cdc slti -5000 / @0x80102d04 slti -27000 / @0x80102d2c slti -19799
- **z1 s11 p3 TREFFER -> 15 Schaden + Griff, Spieler-Aktion 5 oder 0x105** wenn (ent+0x220 & 2) && *(0x800cfdcb) == 0
  - @0x80102d54 andi v0,v0,0x2 / @0x80102d68 lbu v0,0(s0) ;0x800cfdcb / @0x80102d74 addiu a0,zero,15 / @0x80102d78 jal 0x800401d4 (a1=1) / @0x80102d84 jal 0x80015910 a1=0x800CFBF8 / @0x80102dc0 addiu v0,v0,5 / @0x80102dc8 sw v0,0x800cfbfc / @0x80102dd4 jal 0x8003947c(3,0) / @0x80102de4 jal 0x80039514(5,250,0)
- **z1 s11 p4..p10 -> z1 s0 p0** wenn p4: +0x15A=64,+0x158=60,p=5,+0x16A=1,SE 2,+0x222=7,+0x223=1. p5: Timer 0 -> p6, +0x14C=0x003F0000, +0x158=8, +0x21A=ent.X+8192, +0x21C=model+0x8C; Gelenkbiegung+Clamp. p6: +0x220|=1, model[0]=(model[0]&~0x400)|0x2000, model+0xAC &= ~0x800, model+0x8C = ((+0x21A-X)<<8 >>13)*+0x21C >>8. p7: t<10 Gelenke, 6<=t<=79 Jitter, t>=81 Rueckbiegung, t==90 -> p=8. p9: +0x78 -=1, model+0x8C += 128 bis +0x21C, model[0]|=0x400 &~0x2000, model+0xAC |= 0x800, p=10, +0x220 &= ~1, SE 0. p10: model+0x8C -= +0x158 (+1/Frame) < 0 -> ent+0x4 = 1
  - @0x80102e00 sh 64,346 / @0x80102e08 sh 60,344 / @0x80102e5c addiu v0,v0,8192 / @0x80102fa4 ori 0x1 / @0x80102fb0-bc -1025/0x2000 / @0x80102fc4 -2049 / @0x80102ff4 sra v0,v0,13 / @0x80103000 sra v0,t0,8 / @0x80103084 sltiu 0x4a / @0x80103128 slti 81 / @0x80103174 addiu 90 / @0x80103228 addiu 128 / @0x80103260 ori 0x400 / @0x80103284 jal SE a0=0 / @0x801032fc sw 1,4(s1)
- **z1 s12 (0x80103328, 5 Phasen @0x801000e4) -> z1 s0 p0** wenn p0: +0x78=64, +0x74 = Tab@0x80105738[idx] - 1024, Boss+0x228 |= 1<<(idx+4). p1: model+0x8C += 64 >= 4097 -> +0x14C = 0x001F0016, p=2, clamp 4096. p2: p += Anim(0x80)-Rueckgabe, +0x78 += 8. p3: p=4, +0x14C=0x003F0000. p4: Anim 0x40, model+0x8C -= 128 < 0 -> ent+0x4 = 1, Boss+0x228 &= ~(1<<(idx+4)), +0x78 -= 4
  - @0x80103394 addiu v0,zero,64 / @0x801033ac lhu 22328(at) ;0x80105738 / @0x801033b4 addiu v0,v0,-1024 / @0x801033cc-d4 sllv/or 552 / @0x80103400 slti 4097 / @0x8010340c ori 0x16 / @0x8010347c addiu -128 / @0x80103498 sw 1,4(s0)
- **z1 s13 p0..p2 (STURZ-ANGRIFF, 0x801034ec, 6 Phasen @0x801000fc) -> p1 -> p2 -> p3** wenn p0: +0x78 = Tab@0x801057f2+idx*6, Drehung 0x80015558(ent, *(0x800cfc30), *(0x800cfc38), Rate 16), +0x74=0, Boss+0x228 |= 1<<(idx+4), SE 0, ent[0] |= 0x100000. p1: model+0x8C += 64 >= 3573 -> p2, +0x14C = 0x001F0000 + s16@0x801057f0+idx*6, +0x15A=0; +0x78 -= 8. p2: += 64 >= 4097 -> p3
  - @0x80103578 lhu 22514(at) ;0x801057f2 / @0x80103594 jal 0x80015558 a3=16 / @0x801035c8 jal SE a0=0 / @0x801035d8 ori 0x100000 / @0x80103628 slti 3573 / @0x80103650 lh 22512(at) ;0x801057f0 / @0x80103690 slti 4097
- **z1 s13 p3 TREFFER -> 15 Schaden (Aktion 0x305) bzw. 10 Schaden (Aktion 0x405)** wenn ent+0x14D < 21 -> +0x78 += +0x15A (+8/Frame), clamp <= 0. 26 <= ent+0x14D <= 32 && (+0x220 & 4): erste Variante wenn *(0x800cfdcb)==0 -> 15 Schaden; zweite wenn *(0x800cfdcb)!=0 && +0x16B==0 && *(s16*)0x800cfd4e >= 0 -> 10 Schaden
  - @0x801036b4 sltiu v0,v0,0x15 / @0x801036f4 addiu v0,v0,-26 / @0x801036f8 sltiu v0,v0,0x7 / @0x8010370c andi 0x4 / @0x8010372c addiu a0,zero,15 / @0x80103730 jal 0x800401d4 (a1=1) / @0x80103760 addiu v0,zero,773 / @0x80103774 jal SE a0=3 / @0x801037d8 lh v0,-690(v0) ;0x800cfd4e / @0x801037e4 addiu a0,zero,10 / @0x801037e8 jal 0x800401d4 (a1=1) / @0x80103818 addiu v0,zero,1029
- **z1 s13 p4/p5 -> z1 s0 p0 -> Ende** wenn p3: p += Anim(0x80)-Rueckgabe. p4: p=5, +0x14C=0x003F0000. p5: Anim 0x40, model+0x8C -= 128 < 0 -> ent+0x4 = 1, Boss+0x228 &= ~(1<<(idx+4)), ent[0] &= ~0x100000
  - @0x80103864 jal 0x8002959c a3=128 / @0x80103874 addu v1,v1,v0 / @0x80103884 sb 5,6(s0) / @0x801038ac addiu -128 / @0x801038cc sw 1,4(s0) / @0x801038f8 and v0,v0,0xffefffff
- **z1 s14 (TOD/AUFLOESUNG, 0x80103928, 8 Phasen @0x80100114) -> p0..p6, endet in p7 (= return)** wenn p0: +0x74/76/78 = 0, Boss+0x228 |= 1<<(idx+4), ent[0] |= 2, *(0x800cfe30[idx]) |= 2. p1: model+0x8C += 256 >= 2049 -> p2, +0x14C = 0x001F0011 + (rng&3). p2: += 256 >= 4097 -> p3, +0x14C = 0x001F0011+(rng&1), +0x219=1, +0x158=(rng&0x1F)+100, +0x15A=0, +0x16A=3. p3: Anim 0x80 != 0 -> neues +0x14C; Timer 0 -> p4, +0x14C = 0x003F0013+(rng&1), +0x16A=3. p4: Y += +0x158, +0x158 += 2 bis > 60 dann +32; bei Y >= -49 -> Y -= +0x158, +0x158 = -(+0x158)>>4, +0x16A--, bei 0 -> p=5, +0x158=150; 3x Blut + SE 1. p5: Anim 0x40 != 0 -> +0x14C=0x003F0013+(rng&1); Timer 0 -> p6, +0x14C = 0x007F0009 + (rng1+rng2)%3. p6: Anim 0x20, model+0x8C -= 6, +0x8E -= 32, +0x90 += 16, Farbabbau model+0x70; ent+0x14E == 64 -> p=7
  - @0x801039b4-bc sllv/or 552 / @0x801039c8 ori 0x2 / @0x801039e4-f0 0x800cfe30 |= 2 / @0x80103a0c addiu 256 / @0x80103a34 ori 0x11 / @0x80103b24 addiu 100 / @0x80103bec ori 0x13 / @0x80103c44 slti 61 / @0x80103d2c slti a0,-49 / @0x80103d54 sra v0,v0,20 / @0x80103d74 addiu 150 / @0x80103e24 jal SE a0=1 / @0x80103ec4 ori 0x9 (0x007F0009) / @0x80103fb4 addiu -6 / @0x80104020 addiu v0,zero,64
- **z1 s15 (SCHLAF, 0x80104054) -> z1 s8 p0** wenn p0: +0x14C=13, +0x74/76/78=0, ent[0] |= 2, *(0x800cfe30[idx]) |= 2, model+0x8C = 4096; Weckbit (ent+0x1D4 & 1) -> p=1. p1: p += Anim(0x40)-Rueckgabe. p2: ent+0x4 = 0x801 (z1 s8 p0), +0x219=0, model+0x8C=0, ent[0] &= ~2, *(0x800cfe30[idx]) &= ~2
  - @0x80104084 addiu v0,zero,13 / @0x801040bc ori 0x2 / @0x801040d0-e4 0x800cfe30 |= 2 / @0x801040e8 addiu 4096 / @0x801040f8 andi v0,v0,0x1 [ent+0x1D4] / @0x80104098 addiu v0,zero,2049 / @0x8010412c sw v0,4(s0)
- **Zustand 4 / Hook 0x800ce300[0x37] (0x8010423c bzw. 0x80104288) -> *(0x8010582c + ent+0x5 *4): {0x801042c4, 0x801042c4, 0x80104454, 0x801046ac, 0x801048f4}** wenn Ruecksto&-/Aufprall-Routinen; 0x801042c4 p0 setzt +0x1D3 |= 0x80, +0x1C0 |= 0x12, +0x14C=0x00030003+(+0x5&1), Wurfrichtung ratan2(ent,Boss) mit Betrag 800; p2 -> ent+0x4 = 1
  - @0x80104258 lw v0,22572(at) ;0x8010582c / @0x80104358 jal 0x8005ba28 / @0x8010434c addu v0,v0,a2 (0x00030003) / @0x80104368 ori 0x12 / @0x801043b4 addiu v0,zero,800 / @0x8010442c sw a0(=1),4(s1)
- **Zustaende 2, 3 und Hook 0x800ce400[0x37] = 0x80104b68 -> TOTER CODE** wenn deren Sprungtabellen (0x801057e4, 0x80105808, 0x80105840) enthalten KEINE Zeiger — der ai0/ai1-Relokations-Diff (Delta 0xD000) markiert nur 0x80105688..0x8010572f, 0x8010582c..0x8010583f, 0x801058ec, 0x80105974, 0x801059ec als Zeiger
  - @0x80104194 addiu a3,a3,22500 ;0x801057e4 -> Wort 0x0000ffc0 / @0x801041f4 ;0x80105808 -> 0x01020201 / @0x80104b84 ;0x80105840 -> 0x10000001; ai0-Diff zeigt an diesen Adressen KEINE +0xD000-Verschiebung
- **Boss EM36 -> Begleiter -> Kommandowort in ent+0x4** wenn FUN_80104e5c(a0) schreibt in alle vier *(0x800cfe20[0..3])+0x4; FUN_80104e9c(a0=idx, a1) in einen. Werte: 0x0301 = z1 s3 (Biss), 0x0D01 = z1 s13 (Sturz), 0x0B01 = z1 s11 (Schleif-Griff), 0x00070B01 = z1 s11 p7, 0x0601 = z1 s6 (Einziehen), 0x0E01 = z1 s14 (Tod)
  - EM36 @0x80104e5c-98 sw a0,4(v0) x4 / EM36 @0x80104e9c-b0 sll a0,2 / lw 0x800cfe20(at) / sw a1,4(v0) / EM36 @0x80100dd4 addiu a0,zero,2817 / @0x80100e40 addiu a1,zero,2817 / @0x80100ef4 lui a1,0x7 | ori 0xb01 / @0x80101790 addiu a0,zero,1537 / @0x80103608 addiu a1,zero,3585 / Tabelle EM36 @0x80105674 = {0x0301,0x0301,0x0D01,0x0D01,0x0301,0x0301,0x0D01,0x0301,0x0D01,0x0301,0x0D01,0x0D01}
- **root, Nachlauf jeden Frame -> Rumble + Kollisionspunkte + Trefferbits** wenn 0x801054cc: Boss+0x223 &= ~(2<<idx); wenn ent+0x222 != 0 -> --, Boss+0x223 |= 2<<idx, jal 0x80039514(3,130,0) und jal 0x800395b8(25,130,0,3). 0x80104e08: setzt ent+0x84..0xEC aus model+0x108/0x5C, ent+0xD = 255, und ent+0x220 bits1..2 = FUN_80034d0c(0x800CFBF8, ent) << 1 — DAS sind die in s2/s11/s13 abgefragten Treffer-Bits
  - @0x801003b4 jal 0x801054cc / @0x801003f0 jal 0x80016028 / @0x801003f8 jal 0x80104e08 / @0x801054dc-f8 sllv 2<<idx / @0x80105534 jal 0x80039514 / @0x80105548 jal 0x800395b8 / @0x80104f38 jal 0x80034d0c / @0x80104f44 sll v0,v0,1 / @0x80104f4c sh v1,544(s0)

## Aufrufe

- `@0x801003ac` -> `jalr v0 = *(0x80105688 + ent+0x4 *4)` (a0 = ent (s1) @0x801003b0, a1 = ent+0x108 @0x8010038c, a2 = ent+0x17C @0x80100394) — Zustands-Dispatch
- `@0x8010091c` -> `jalr v0 = *(0x801056a8 + (ent+0x10E & 0x3F)*4)` (a0/a1/a2 durchgereicht) — Varianten-Dispatch in Zustand 1
- `@0x8010096c / @0x80100994` -> `jalr *(0x801056b0+sub*4) / *(0x801056f0+sub*4)` (a0 = ent, a1, a2) — Substate-Dispatch (1. Tabelle leer, 2. = echter Handler)
- `@0x80100348` -> `jal 0x8008dba4 (ApplyMatrixLV)` (a0 = Boss->model+0x1A0 (@0x8010028c/@0x80100340), a1 = a2 = sp+16 (Offset aus 0x80105668)) — Ansatz-Offset in Weltkoordinaten drehen
- `@0x80100628` -> `jal 0x80016480` (a0 = ent+0x16C @0x8010050c, a1=a2=a3=0, sp+16 = ent->model+0x5C @0x80100624) — Modell-/Anim-Setup in Zustand 0
- `@0x80100820` -> `jal 0x8008e1f4 (RotMatrix)` (a0 = Boss+0x74 (@0x80100818/@0x80100824), a1 = sp+32) — Startposition: Boss-Rotation in Matrix
- `@0x80100830` -> `jal 0x8008dba4` (a0 = sp+32, a1 = a2 = sp+24 (Offset 0x80105650+s0*8)) — Startposition der Begleiter
- `@0x8010088c` -> `jal 0x801050bc` (a0 = 52 @0x80100880, a1 = ent @0x80100884) — Kind-Entity (Typ 52) allokieren; kopiert +0x1BC/+0x17C/+0x108/+0x184/+0x180/+0x18C/+0x188/+0x194/+0x190 vom Erzeuger
- `@0x801008a8` -> `jal 0x801004f0 (rekursiv Zustand-0-Init)` (a0 = neues Kind, a1 = s3, a2 = s4) — Kind initialisieren (+0x10E==8 -> Kurzpfad)
- `@0x801007a8` -> `jal 0x80104c84` (a0 = ent, a1 = *(0x800ce32c)) — 3 Modellteile registrieren (0x80019628 x3 mit 0x801058b8/0x80105940/0x801059b8)
- `@0x801003b4` -> `jal 0x801054cc` (a0 = ent) — Boss+0x223-Bit + Rumble solange ent+0x222 != 0
- `@0x801003f0` -> `jal 0x80016028` (a0 = ent) — EXE-Standard-Nachlauf pro Entity
- `@0x801003f8` -> `jal 0x80104e08` (a0 = ent) — Kollisionspunkte +0x84..+0xEC setzen, ent+0x220 Bit1/2 = FUN_80034d0c(0x800CFBF8,ent)<<1
- `@0x801001b0` -> `jal 0x80104f64` (a0 = Kind-Entity) — Kind erbt Matrix/Position vom Eltern-Tentakel *(0x800cfe10+idx*4)
- `@0x80100b7c` -> `jal 0x8008d2f4 (SquareRoot)` (a0 = dY*dY + dZ*dZ (@0x80100b4c-80)) — s1 p0: Abstand in der YZ-Ebene
- `@0x80100bc0` -> `jal 0x8008d190 (ratan)` (a0 = (sqrt<<12)/dX (@0x80100b8c-bc)) — s1 p0: ent+0x78 = Neigung zum Spieler
- `@0x80100bf4` -> `jal 0x800154ac (ratan2)` (a0 = (s16)ent.Y, a1 = -(ent.Z), a2 = (s16)Spieler+0x60, a3 = -(Spieler+0x64)) — ent+0x74 = Winkel zum Spieler in der (Y,-Z)-Ebene
- `@0x80100db8` -> `jal 0x80015558` (a0 = ent, a1 = Spieler+0x5C, a2 = Spieler+0x64, a3 = 2048) — s2 p0: Yaw (X/Z-Ebene) hart auf den Spieler
- `@0x80103594 / @0x801035f4` -> `jal 0x80015558` (a0 = ent, a1 = *(0x800cfc30), a2 = *(0x800cfc38), a3 = 16) — s13: Yaw auf einen globalen Weltpunkt drehen (Rate 16)
- `@0x8010191c` -> `jal 0x80104d64` (a0 = ent, a1 = Spieler+0x60, a2 = Spieler+0x64, a3 = 8) — s4 p1: ent+0x74 mit Rate 8 auf den Spieler nachfuehren
- `@0x80101c80` -> `jal 0x80104d64` (a0 = ent, a1 = Spieler+0x60 - 500 (@0x80101c84), a2 = Spieler+0x64, a3 = 64) — s7 p1: Nachfuehren mit Hoehenversatz -500, Rate 64
- `@0x80100f28 / @0x8010225c / @0x80102c68` -> `jal 0x80104ba4` (a0 = ent, a1 = ent->model) — Kette der 4 Segment-Matrizen neu bauen (RotMatrix ent+0x74 + 3x MulMatrix 0x8002ce94)
- `@0x8010127c / @0x80102450 / @0x80102eec` -> `jal 0x80104c30` (a0 = ent, a1 = ent->model) — 4x RotMatrix aus model+0x68/+0x114/+0x1C0/+0x26C — uebernimmt die Gelenkbiegung
- `@0x801016d0` -> `jal 0x800401d4` (a0 = 15 (@0x801016cc), a1 = 0 (@0x801016d4)) — s3 p3: Spielerschaden Biss; Rueckgabe 2 = Spieler stirbt -> Aktion 3
- `@0x80102d78` -> `jal 0x800401d4` (a0 = 15 (@0x80102d74), a1 = 1 (@0x80102d7c)) — s11 p3: Spielerschaden beim Schleif-Griff
- `@0x80103730` -> `jal 0x800401d4` (a0 = 15 (@0x8010372c), a1 = 1 (@0x80103734)) — s13 p3: Spielerschaden Sturz-Angriff (erster Treffer)
- `@0x801037e8` -> `jal 0x800401d4` (a0 = 10 (@0x801037e4), a1 = 1 (@0x801037ec)) — s13 p3: Nachschlag, nur wenn Spieler schon gegriffen und *(s16*)0x800cfd4e >= 0
- `@0x801016f8 / @0x80102d84` -> `jal 0x80015910` (a0 = ent, a1 = 0x800CFBF8 (Spieler-Entity)) — Blickrichtungs-Vergleich +0x76 gegen +0x76 — waehlt Spieler-Aktion 5 (von vorn) vs 0x105 (von hinten)
- `@0x80101478` -> `jal 0x8005bd6c` (a0 = 0 (@0x80101410), a1 = ent) — SE 0 — Ausfahr-/Zisch-Laut (s3 p0)
- `@0x80101790` -> `jal 0x8005bd6c` (a0 = 3 (@0x80101778), a1 = ent) — SE 3 — Treffer/Griff (s3 p3)
- `@0x80101da4` -> `jal 0x8005bd6c` (a0 = 0 (@0x80101d90), a1 = ent) — SE 0 (s8 p1)
- `@0x80101e14` -> `jal 0x8005bd6c` (a0 = 2 (@0x80101dfc), a1 = ent) — SE 2 — Aufschlag/Erschuetterung (s8 p3)
- `@0x8010201c` -> `jal 0x8005bd6c` (a0 = 0 (@0x8010200c), a1 = ent) — SE 0 (s8 p8, Einzug)
- `@0x8010213c` -> `jal 0x8005bd6c` (a0 = 0 (@0x801020e0), a1 = ent) — SE 0 (s9 p0)
- `@0x8010221c` -> `jal 0x8005bd6c` (a0 = 15 (@0x80102208), a1 = ent) — SE 15 — voll ausgefahren (s9 p2)
- `@0x8010236c` -> `jal 0x8005bd6c` (a0 = 2 (@0x80102344), a1 = ent) — SE 2 (s9 p4)
- `@0x801026ec` -> `jal 0x8005bd6c` (a0 = 0 (@0x801026dc), a1 = ent) — SE 0 (s9 p9)
- `@0x801027f8` -> `jal 0x8005bd6c` (a0 = 0 (@0x801027d0), a1 = ent) — SE 0 (s10 p0)
- `@0x80102b20` -> `jal 0x8005bd6c` (a0 = 0 (@0x80102ad4), a1 = ent) — SE 0 (s11 p0)
- `@0x80102c28` -> `jal 0x8005bd6c` (a0 = 15 (@0x80102c14), a1 = ent) — SE 15 (s11 p2)
- `@0x80102e18` -> `jal 0x8005bd6c` (a0 = 2 (@0x80102df4), a1 = ent) — SE 2 (s11 p4)
- `@0x80103284` -> `jal 0x8005bd6c` (a0 = 0 (@0x80103248), a1 = ent) — SE 0 (s11 p9)
- `@0x801035c8` -> `jal 0x8005bd6c` (a0 = 0 (@0x8010359c), a1 = ent) — SE 0 (s13 p0)
- `@0x80103774` -> `jal 0x8005bd6c` (a0 = 3 (@0x8010373c), a1 = ent) — SE 3 (s13 p3, erster Treffer)
- `@0x8010382c` -> `jal 0x8005bd6c` (a0 = 3 (@0x801037f4), a1 = ent) — SE 3 (s13 p3, Nachschlag)
- `@0x80103e24` -> `jal 0x8005bd6c` (a0 = 1 (@0x80103e20), a1 = ent) — SE 1 — Aufprall beim Sterben (s14 p4). Damit sind es GENAU 5 SE-Ids: 0, 1, 2, 3, 15 (18 Aufrufstellen)
- `@0x80101004, @0x801011c4, @0x8010179c, @0x80102dd4, @0x80103788, @0x80103840, @0x80103e30, @0x801045a8` -> `jal 0x8003947c` (a0 = 3, a1 = 0 (jedes Mal)) — Effekt-Trigger bei jedem Griff und beim Tod
- `@0x80101014, @0x801011d4, @0x801017ac, @0x80103798, @0x80103850, @0x80103e40` -> `jal 0x80039514` (a0 = 10, a1 = 250, a2 = 0) — Bildschirm-Ruettler nach dem Griff
- `@0x80102de4` -> `jal 0x80039514` (a0 = 5, a1 = 250, a2 = 0) — schwaecherer Ruettler beim Schleif-Griff (s11)
- `@0x801045b8` -> `jal 0x80039514` (a0 = 6, a1 = 250, a2 = 0) — Ruettler beim Aufschlag (Hook-Substate 2)
- `@0x80105534 / @0x80105548` -> `jal 0x80039514 / jal 0x800395b8` ((3, 130, 0) / (25, 130, 0, 3)) — Dauer-Rumble solange ent+0x222 > 0
- `@0x80103c9c, @0x80103d1c, @0x80103da8, @0x80103de0, @0x80103e18, @0x80103f88` -> `jal 0x8001bf10` (a0 = 0x003F1FA0 + (rng&0xFF)*8 mit Bit16 = rng&1, a1 = ent+0x76, a2 = model+0x48 / +0xF4 / +0x1A0, a3 = 0 oder sp+16) — Blut-/Gore-Partikel an den 3 Segment-Matrizen (Todes-Sequenz s14)
- `@0x80104358, @0x801044f8, @0x8010458c, @0x8010459c` -> `jal 0x8005ba28` (a0 = 0x04000001 | (((+0x5&1)+1)<<16) bzw. 0x04010001 / 0x04020001 / 0x02290001, a1 = ent+0x38) — Aufprall-/Splatter-Effekt der Ruecksto&-Routinen
- `@0x80100a78 u.a. (65 Stellen)` -> `jal 0x80015fe8 (RNG)` (keine) — alle Zufallswerte (Timer, Clipwahl, Jitter, S11-Rotationsindex)
- `EM36 @0x80104e5c` -> `FUN_80104e5c` (a0 = Kommandowort, geschrieben in alle *(0x800cfe20[0..3])+0x4) — Boss befiehlt allen vier Begleitern (0x0B01 @EM36 0x80100dd4, 0x0601 @EM36 0x80101790)
- `EM36 @0x80104e9c` -> `FUN_80104e9c` (a0 = Slot-Index, a1 = Kommandowort) — Boss befiehlt einem Begleiter (0x0B01, 0x00070B01, 0x0301/0x0D01 aus Tabelle EM36 0x80105674, 0x0E01 @EM36 0x80103608)

## Offen
- Zustaende 2 (0x80104184) und 3 (0x801041e4) sowie der Hook 0x800ce400[0x37] = 0x80104b68 lesen Sprungtabellen an 0x801057e4 / 0x80105808 / 0x80105840, an denen KEINE relozierten Zeiger stehen (ai0/ai1-Diff). Sie sind entweder toter Code oder werden nie mit diesen Substates betreten — nicht abschliessend belegt, welcher Fall gilt.
- Die beiden 24-s16-Bloecke @0x80105608 und @0x80105638 (nur im ersten Wertepaar verschieden: 300,300 gegen 240,240) werden vom EM37-Code NICHT gelesen. Verbraucher noch nicht gefunden (Kandidaten: EMD/Modell-Deskriptoren, EXE-Kollisionslader).
- ent+0x14C: Aufteilung [31:16]=Maske / [15:8]=Startframe / [7:0]=Clip ist aus den Schreibmustern abgeleitet (s4: (rng%90)<<8 + 0x3F0009+(rng&3), s8 p0: reine Clip-Bytes 14/14/16/15). Die LESENDE Stelle liegt in der EXE (nicht in diesem Chunk) und wurde nicht disassembliert; die Maskenwerte 0x00/0x03/0x07/0x0F/0x1F/0x3F/0x7F sind unerklaert.
- Die genaue Semantik von FUN_80016480 (Zustand 0, a0=ent+0x16C, 5. Argument ent->model+0x5C) ist nicht aufgeloest.
- FUN_801050bc: der Zweck von a0 = 52 ist nicht belegt — im disassemblierten Anfangsteil (0x801050bc..0x80105230) wird a0 nicht gelesen; die Funktion allokiert aus dem Pool 0x800cc1e8+0x3C34 / +0x2144 / +0x214C und kopiert Felder von a1.
- 0x800cfe10[idx] (von der Kind-Entity in 0x80104f64 gelesen) liefert fuer idx 0..3 die Adressen 0x800cfe10/14/18/1c — 0x800cfe1c ist aber der BOSS. Das passt nur, wenn 0x801050bc dem Kind +0x218 = Eltern-idx + 4 setzt; das ist noch nicht disassembliert belegt.
- Der Zahlenwert der 5 SE-Ids (0,1,2,3,15) ist der Index in die typ-eigene SE-Bank; welche VAB-Programm-/Sample-Nummern dahinterstehen, steht nicht in diesem Chunk.
- *(0x800cfc30) / *(0x800cfc38) (Zielpunkt des Sturz-Angriffs s13) und *(s16*)0x800cfd4e (Gate fuer den 10er-Schaden) sind Globals, die dieser Chunk nur liest — die Schreiber (Boss-Overlay oder EXE) sind nicht identifiziert.
- Der Handler von Substate 9 setzt in p9 die Phase auf 10, was ausserhalb der 10-Eintrag-Tabelle liegt (sltiu 0xa @0x80102074) — der Tentakel bleibt danach dauerhaft ohne Handler. Ob das Absicht ist (Boss uebernimmt) oder ein Original-Bug, ist nicht belegt.
- Die Weltgrenzen -27000 / -19799 / -5000 / -399 sind raumfeste Konstanten; sie wurden NICHT gegen die Kollisions-/Kamerageometrie von ROOM7040 bzw. ROOMG040 gegengeprueft.
- Substate 0 und Substate 8 p0 sind terminal (setzen keine Phase weiter). Sie warten auf ein Boss-Kommando; die vollstaendige Liste der Boss-Trigger-BEDINGUNGEN wurde nur soweit verfolgt, wie EM36 ent+0x4 schreibt (FUN_80104e5c / FUN_80104e9c).
- ent+0x1D4 Bit0 (Weckbit fuer Substate 15) wird in diesem Chunk nur gelesen (@0x801040f8), nie geschrieben — der Schreiber ist nicht gefunden.


---

# Modell + Animation Endgegner Typ 0x36 / Begleiter 0x37 (RE2 CDEMD0.EMS) — EMD/EMR/EDD/MD1/TIM-Aufschluesselung fuer den Port-Renderer

Beide EMDs sind Standard-RE2-EMD mit dir_cnt=8; der vorhandene Port-Parser re2_emd_parse_bank() liest sie OHNE Aenderung (dir_cnt-Pruefung ==8 erfuellt, alle Bounds geprueft, MAX_FRAMES/MAX_BONES/MD1_MAX_MESHES reichen), und der TOC in gen/re2_ems_toc.inc fuehrt kind 0x36/0x37 bereits mit exakt passenden Groessen. Die Ueberraschung liegt in der GEOMETRIE-ZU-SKELETT-Relation: EM36 hat 7 Meshes, aber nur 2 Bones (byte-hart doppelt bestaetigt: kf_size 24 = 12 + ceil(2*3*12bit) und keyframe_count 1360 == EDD-Framesumme 1360). Der Renderer des Ports zeichnet mit `nmi = nbi` genau bone_count Meshes — der 635-Vertex-KOERPER (Mesh 2) und Mesh 3 fielen damit weg. RE2 selbst baut in FUN_80028368 pro MESH (entity+0x107 = nObj/2) ein 172-Byte-Work-Struct und haengt Meshes ab Index bone_count an die globale Identitaets-Matrix @0x8009db44, mit Bind-Offsets, die weiter im 6-Byte-Takt aus dem EMR gelesen werden (also HINTER die Positionstabelle, gemessene Werte unten). EM37 ist sauber: 4 Meshes == 4 Bones. Zusaetzlich: dir[0] von EM36 ist ein 19112-Byte-MORPH-Block (5 Ziele a 635 Vertices, Ziel 0 byte-identisch mit der MD1-Basis von Mesh 2) — er wird von FUN_8004b3b8 an work+136 gebunden, aber die Anwender-Funktion FUN_8004b434 ist im RE2-Leon-Retail-Image NIRGENDS referenziert (Byte-Suche nach 0x8004b434 = 0 Treffer), d.h. fuer byte-true Zeichnen genuegt die MD1-Basisgeometrie. Fehlend im Port sind drei Dinge: das Lade-Tor (re15_re2_owns_type kennt 0x36/0x37 nicht, und 0x36 kollidiert mit dem RE1.5-Typ 0x36), der bedingungslose Hybrid-Aufruf (es gibt kein RE1.5-Gegenstueck) und die Mesh-Schleife im Renderer.

## Posten

### EMD-Kopf
RE2-EMD-Kopf = {u32 dir_off, u32 dir_cnt}; das Verzeichnis liegt AM DATEIENDE bei dir_off und enthaelt dir_cnt u32 = absolute Datei-Offsets der Sektionen. dir[0] ist immer 8 (= direkt hinter dem Kopf), die Sektionsgroesse ergibt sich aus dir[i+1]-dir[i], die letzte aus dir_off-dir[7].

**Beleg:** EM36.EMD@0x00 = B4 B6 01 00 (0x1B6B4) / 08 00 00 00; Dir @0x1B6B4 = {8, 0x4AB0, 0x6020, 0xDFC0, 0xDFC4, 0xDFC8, 0xE228, 0x11110}; 0x1B6B4+32 = 112340 = Dateigroesse. EM37.EMD@0x00 = D0 E7 00 00 / 08 00 00 00; Dir @0xE7D0 = {8, 0xC, 0x134C, 0xAA60, 0xAA64, 0xAA68, 0xACCC, 0xD984}; 0xE7D0+32 = 59376.

**Umsetzung:** Keine Aenderung: re15_port/engine/src/re2_ems.c:re2_emd_parse_bank() liest genau das (rd_u32(emd+0), rd_u32(emd+4), Pruefung dir_cnt != 8 -> -2). Beide Dateien erfuellen die Pruefung.

### EMD-Verzeichnis / Rollen
Rollen der 8 dir-Eintraege, byte-belegt aus dem RE2-Modell-Binder: dir[0]->entity+0x1BC (Morph-Block, optional), dir[1]->+0x17C (EDD Paar 1), dir[2]->+0x108 (EMR Struktur, wird von FUN_80028368 als a2 benutzt), dir[3]->+0x184 (EDD Paar 2), dir[4]->+0x180 (EMR-Pool 2), dir[5]->+0x18C (EDD Paar 3), dir[6]->+0x188 (EMR-Pool 3), dir[7]->+0x14 (MD1).

**Beleg:** FUN_8001aaa8: 0x8001ab88 lw v1,0(s2) (dir_off) / 0x8001ab90 addu s0,s2,v1 (Dir-Zeiger); dann 0x8001ab94-0x8001ac14: lw 0(s0)->sw 444(s1)=+0x1BC, lw 4->sw 380=+0x17C, lw 8->sw 264=+0x108, lw 12->sw 388=+0x184, lw 16->sw 384=+0x180, lw 20->sw 396=+0x18C, lw 24->sw 392=+0x188, lw 28->sw 20=+0x14.

**Umsetzung:** Bestaetigt die vorhandene Zuordnung in re2_emd_parse_bank (D[1]/D[2] loco, D[3]/D[4] own, D[5]/D[6] victim, D[7] MD1). NEU: D[0] wird vom Port ignoriert — das ist fuer EM36 der Morph-Block (eigener Posten).

### EMD-Verzeichnis EM36 (Offset/Groesse/Rolle)
D[0]=0x000008 19112 B MORPH-BLOCK | D[1]=0x004AB0 5488 B EDD Paar1 (11 Clips) | D[2]=0x006020 32672 B EMR Struktur+Keyframes (2 Bones, kf 24 B, 1360 kf) | D[3]=0x00DFC0 4 B LEER | D[4]=0x00DFC4 4 B LEER | D[5]=0x00DFC8 608 B EDD Paar3 (1 Clip, 150 f) | D[6]=0x00E228 12008 B EMR-Pool Paar3 (15 Bones, kf 80 B, 150 kf) | D[7]=0x011110 42404 B MD1 (7 Meshes). Also: 3 Paar-Slots, davon 2 belegt (Paar 1 = Gegner-Animation, Paar 3 = Opfer/Leon-Animation), Paar 2 leer.

**Beleg:** Dir @EM36.EMD:0x1B6B4 (s.o.); Sektionsgroessen = Differenz der Nachbarn; Kopfworte: 0x4AB0 = 64 00 2C 00 (EDD), 0x6020 = 14 00 20 00 02 00 18 00 (EMR), 0xDFC0/0xDFC4 = 00 00 00 00, 0xDFC8 = 96 00 04 00, 0xE228 = 00 00 08 00 0F 00 50 00, 0x11110 = 0C 6F 00 00 / 00 00 00 00 / 0E 00 00 00.

**Umsetzung:** re2_emd_parse_bank liefert damit: loco_ok=1 (D[1]/D[2]), own_ok=0 (D[3] hat offset0==0 -> re15_emd_parse_animation gibt -3), victim_ok=1, best-Paar = Paar 1 mit 11 Clips -> eb->skel/eb->anim. Kein Codeaenderung noetig.

### EMD-Verzeichnis EM37 (Offset/Groesse/Rolle)
D[0]=0x000008 4 B LEER | D[1]=0x00000C 4928 B EDD Paar1 (24 Clips) | D[2]=0x00134C 38676 B EMR Struktur+Keyframes (4 Bones, kf 32 B, 1207 kf) | D[3]=0x00AA60 4 B LEER | D[4]=0x00AA64 4 B LEER | D[5]=0x00AA68 612 B EDD Paar3 (6 Clips, 146 f) | D[6]=0x00ACCC 11448 B EMR-Pool Paar3 (15 Bones, kf 80 B, 143 kf) | D[7]=0x00D984 3660 B MD1 (4 Meshes). EM37 hat KEINEN Morph-Block (D[0][0] == 0).

**Beleg:** Dir @EM37.EMD:0xE7D0; Kopfworte: 0x0C = 01 00 60 00 (EDD), 0x134C = 20 00 34 00 04 00 20 00 (EMR), 0xAA68 = 18 00 18 00, 0xACCC = 00 00 08 00 0F 00 50 00, 0xD984 = 20 0A 00 00 / 00 00 00 00 / 08 00 00 00.

**Umsetzung:** Wie EM36: loco_ok=1, own_ok=0, victim_ok=1, best = Paar 1 (24 Clips). re2_emd_parse_bank gibt 0 zurueck.

### EMR-Kopf-Layout (Bestaetigung)
EMR-Kopf = u16 bones_table_off, u16 keyframes_off, u16 bone_count, u16 keyframe_size. Danach bone_count*6 Byte Bind-Offsets (s16 x,y,z) ab +8, dann bone_count*4 Byte {u16 child_count, u16 child_list_off} ab bones_table_off, dann die flache Child-Byte-Liste, dann die Keyframes. keyframe_size = 12 + aufgerundet(bone_count*3*12 bit) auf 4 Byte.

**Beleg:** EM36 D[2]: {0x14,0x20,2,0x18}: 8+2*6=0x14 == bones_table_off; 0x14+2*4=0x1C; Child-Bytes 0x1C..0x20 = 01 00 00 00; kf ab 0x20; 12+ceil(2*3*12/8)=12+9 -> 24 = 0x18. EM37 D[2]: {0x20,0x34,4,0x20}: 8+4*6=0x20; 0x20+4*4=0x30; Child-Bytes 0x30..0x34 = 01 02 03 00; 12+ceil(4*3*12/8)=12+18=30 -> 32. Unabhaengige Gegenprobe: (Sektionsgroesse - kf_off)/kf_size == EDD-Framesumme: EM36 (32672-32)/24 = 1360 == 1360; EM37 (38676-52)/32 = 1207 == 1207.

**Umsetzung:** Deckt sich 1:1 mit re15_emd_parse_skeleton (emd_common.c:130-197). Keine Aenderung.

### SKELETT EM36 (2 Bones)
bone_count = 2, keyframe_size = 24 B, keyframe_count = 1360. parent[] = {-1, 0}. Bind-Offsets (s16 x,y,z): Bone0 = (0, -4534, 0), Bone1 = (2146, 807, 0). Child-Tabelle = {(count 1, off 8), (count 0, off 9)}, Child-Bytes = 01 00 00 00.

**Beleg:** EM36.EMD@0x6020 = 14 00 20 00 02 00 18 00; Bind ab @0x6028: 00 00 4A EE 00 00 | 62 08 27 03 00 00; Child-Tabelle @0x6034: 01 00 08 00 | 00 00 09 00; Child-Bytes @0x603C: 01 00 00 00.

**Umsetzung:** re15_emd_parse_skeleton liefert genau das. ACHTUNG: bone_relative_pos[0] wird vom Port nie gelesen (Wurzel kommt aus dem Keyframe) — der Wert -4534 in Y steckt zusaetzlich in JEDEM Keyframe-Wurzeloffset (kf0 = 00 00 4A EE 00 00), also nicht doppelt anwenden.

### SKELETT EM37 (4 Bones, offene Kette)
bone_count = 4, keyframe_size = 32 B, keyframe_count = 1207. parent[] = {-1, 0, 1, 2} (reine Kette). Bind-Offsets: Bone0 = (0,0,0), Bone1 = (3750,0,0), Bone2 = (3003,0,0), Bone3 = (1508,0,0) — Glieder entlang +X, Gesamtlaenge 8261. Child-Tabelle = {(1,16),(1,17),(1,18),(0,19)}, Child-Bytes = 01 02 03 00.

**Beleg:** EM37.EMD@0x134C = 20 00 34 00 04 00 20 00; Bind ab @0x1354; Child-Tabelle @0x136C; Child-Bytes @0x137C = 01 02 03 00. Keyframe-Beispiel kf1 @0x1380+32: Winkel nur um Z (Bone1 z=1043, Bone2 z=777, Bone3 z=310) -> Biegung in EINER Ebene.

**Umsetzung:** Unveraendert parsebar. Fuer den Renderer relevant: 4 Bones == 4 Meshes, also greift `mesh_idx == bone_idx` bei EM37 korrekt.

### EMR-Pool Paar 3 (Opfer/Leon) — beide Typen
D[6] ist ein reiner Keyframe-POOL ohne Struktur: Kopf {bones_table_off = 0, kf_off = 8, bone_count = 15, kf_size = 0x50}. 15 Bones = Leons PL00-Rig; die Struktur muss von ausserhalb kommen.

**Beleg:** EM36.EMD@0xE228 = 00 00 08 00 0F 00 50 00, (12008-8)/80 = 150 == EDD-Framesumme von D[5] (150). EM37.EMD@0xACCC = 00 00 08 00 0F 00 50 00, (11448-8)/80 = 143.

**Umsetzung:** BUG im Port: re2_parse_pair(emd, size, D[5], D[2], D[6]) nimmt die STRUKTUR aus D[2] — das sind hier 2 (EM36) bzw. 4 (EM37) Bones statt 15. Die Opfer-Pose wird also mit 2/4 Bones aus einem 80-Byte-Keyframe gelesen = falsches Rig. Fuer EM010 fiel das nicht auf, weil dessen D[2] selbst 15 Bones hat. Fix: fuer Paar 3 die Struktur aus dem PL00-Skelett des Spielers nehmen (der Port haelt Leons Rig ohnehin), nicht aus D[2]; oder eb->victim_ok = 0 setzen, wenn D[2].bone_count != D[6].bone_count.

### ANIMATION EM36 — vollstaendige Clip-Laengentabelle (Paar 1, D[1] @0x4AB0)
11 Clips, 1360 Frames gesamt. clip:frames(Framelisten-Offset rel. EDD-Start) = 0:100(0x002C) 1:180(0x01BC) 2:150(0x048C) 3:100(0x06E4) 4:180(0x0874) 5:150(0x0B44) 6:100(0x0D9C) 7:50(0x0F2C) 8:50(0x0FF4) 9:150(0x10BC) 10:150(0x1314). Erste Keyframe-Indizes je Clip (kumulativ): 0,100,280,430,530,710,860,960,1010,1060,1210; letzter Index 1359.

**Beleg:** EM36.EMD@0x4AB0 ff: 64 00 2C 00 | B4 00 BC 01 | 96 00 8C 04 | 64 00 E4 06 | B4 00 74 08 | 96 00 44 0B | 64 00 9C 0D | 32 00 2C 0F | 32 00 F4 0F | 96 00 BC 10 | 96 00 14 13. Kettenprobe: 0x2C + 100*4 = 0x1BC == Offset Clip 1. Summe 1360 == keyframe_count aus D[2].

**Umsetzung:** Das ist die Tabelle, gegen die die KI-Straenge ihre Clip-Indizes deuten muessen (gueltig 0..10). Im Port landet sie in eb->anim.clips[] via re15_emd_parse_animation; RE15_EMD_MAX_CLIPS=64 und RE15_EMD_MAX_FRAMES=1664 reichen (1360).

### ANIMATION EM37 — vollstaendige Clip-Laengentabelle (Paar 1, D[1] @0x0C)
24 Clips, 1207 Frames gesamt. clip:frames(Offset) = 0:1(0x0060) 1:1(0x0064) 2:1(0x0068) 3:1(0x006C) 4:1(0x0070) 5:1(0x0074) 6:1(0x0078) 7:100(0x007C) 8:100(0x020C) 9:100(0x039C) 10:140(0x052C) 11:120(0x075C) 12:60(0x093C) 13:120(0x0A2C) 14:30(0x0C0C) 15:30(0x0C84) 16:30(0x0CFC) 17:50(0x0D74) 18:50(0x0E3C) 19:50(0x0F04) 20:40(0x0FCC) 21:75(0x106C) 22:30(0x1198) 23:75(0x1210). Clips 0..6 sind Ein-Frame-Posen (Keyframes 0..6).

**Beleg:** EM37.EMD@0x0C ff: 01 00 60 00 | 01 00 64 00 | 01 00 68 00 | 01 00 6C 00 | 01 00 70 00 | 01 00 74 00 | 01 00 78 00 | 64 00 7C 00 | 64 00 0C 02 | 64 00 9C 03 | 8C 00 2C 05 | 78 00 5C 07 | 3C 00 3C 09 | ... ; Summe 1207 == keyframe_count aus D[2] (38676-52)/32.

**Umsetzung:** Gueltiger Clip-Index-Bereich der KI 0x37 = 0..23. Der Port parst das ohne Aenderung.

### ANIMATION Opfer-Clips (Paar 3)
EM36 D[5] @0xDFC8: 1 Clip, 150 Frames, Framelisten-Offset 0x0004. EM37 D[5] @0xAA68: 6 Clips: (24,0x18) (16,0x78) (10,0xB8) (25,0xE0) (21,0x144) (50,0x198), Summe 146 Frames.

**Beleg:** EM36.EMD@0xDFC8 = 96 00 04 00; 4 + 150*4 = 604 < 608 (Sektionsgroesse). EM37.EMD@0xAA68 = 18 00 18 00 | 10 00 78 00 | 0A 00 B8 00 | 19 00 E0 00 | 15 00 44 01 | 32 00 98 01.

**Umsetzung:** Diese Clips posieren LEON (15-Bone-Pool D[6]); erst nutzbar, wenn der Struktur-Bug aus dem Pool-Posten behoben ist.

### KEYFRAME-Layout (beide Typen)
Keyframe = [+0 s16 root_x][+2 s16 root_y][+4 s16 root_z][+6..+11 6 B Speed/Reserve][+12.. gepackte 12-Bit-Euler, 3 pro Bone, LSB-first, Zweierkomplement]. ROOT-TRANSLATION STECKT ALSO IM KEYFRAME. Bei EM36 ist root = (0, ~-4534, 0) und wandert langsam (kf0 -4534, kf5 -4529), Speed-Feld = 0. Bei EM37 sind alle 32 Byte von kf0 = 0.

**Beleg:** EM36.EMD@0x6040 (kf0) = 00 00 4A EE 00 00 | 00 00 00 00 00 00 | 00 00 00 00 10 00 FE 0F 00 00 00 00 -> Winkel ab +12: Bone0 (0,0,0), Bone1 (1,-2,0); kf2..kf5 ergeben fuer Bone1 x = 1,4,8,13,18 und z = 0,2,5,10,15 (glatte Kurven) — das beweist den Start bei +12, nicht bei +15. EM37 kf1 (@0x1380+32): Winkel ab +12 -> Bone1 z=1043, Bone2 z=777, Bone3 z=310.

**Umsetzung:** Identisch zu re15_emd_get_keyframe_angles (emd_common.c:219, bit_offset = bone_index*36, Basis frame_base+12, Sign-Extend bei 0x800) und re15_emd_get_keyframe_position (+0..+5). Keine Aenderung.

### EDD-Framewort + Ereignis-Bits
Jedes Framewort ist u32; Bits 0..11 = Keyframe-Index (bei beiden Typen streng aufsteigend, also 1:1-Abbildung Clipframe->Keyframe). EM36 hat KEINE gesetzten Oberbits. EM37 traegt Ereignis-Bits im obersten Byte: 0x08000000 (Marker, immer gesetzt) plus 0x10000000/0x20000000/0x40000000/0x80000000; vorkommende Masken 0x18000000 (2x), 0x48000000 (11x), 0x58000000 (8x), 0xF8000000 (5x).

**Beleg:** EM36 Clip0-Woerter @0x4AB0+0x2C = 00..09 lueckenlos, letzter Wert Clip10 = 0x54F = 1359. EM37 markierte Frames: Clip7 f32=0x48000000|39, f62=0xF8000000|69; Clip8 f38=0x48..|145, f51=0xF8..|158; Clip13 f3/f30/f43/f55/f72/f108; Clip14 f23; Clip15 f23; Clip16 f21; Clip17 f11/f31/f37; Clip18 f22/f35; Clip19 f3/f16; Clip20 f7/f17; Clip21 f27/f35; Clip23 f25/f31.

**Umsetzung:** re15_emd_get_frame_entry (emd_common.c:311) liefert das rohe Wort — die KI-Straenge koennen die Oberbits direkt fuer SE-/Angriffs-Trigger von 0x37 auswerten. Keine Parser-Aenderung.

### Animations-Fortschaltung (fuer die KI-Straenge)
FUN_8002959C(a0 = entity, a1 = Work-Arena, a2 = EDD-Basis, a3 = Flags) liest den CLIP aus entity+0x14C und den FRAME-IM-CLIP aus entity+0x14D (NICHT +0x94/+0x95 wie in RE1.5), holt aus EDD[clip] {u16 frame_count, u16 list_off} und legt den Zeiger auf das aktuelle Framewort nach entity+0x178. a3 & 0xFFFF0000 != 0 = RUECKWAERTS (Index = frame_count - entity+0x14D - 1).

**Beleg:** 0x800295a8 lbu v0,332(a0); 0x800295b0 sll v0,v0,2; 0x800295bc lw v1,0(a2); 0x800295c8 andi a2,v1,0xffff; 0x800295cc srl v1,v1,16; 0x800295c0/c4 Rueckwaerts-Gate; 0x800295d0 lbu v0,333(a0); 0x800295f8 sw a2,376(a0). Die EDD-Basis ist in den Overlays entity+0x17C (= dir[1]): CDEMD0_EM37_ai1.BIN, 4 Aufrufstellen mit `lw a2,380(s1)`.

**Umsetzung:** Belegt, dass die Clip-Indizes der beiden KI-Overlays gegen die Paar-1-Tabellen oben zu deuten sind (EM36 0..10, EM37 0..23). Aufrufhaeufigkeit: jal 0x8002959C 20x in EM36, 55x in EM37.

### MESH EM36 (MD1 @0x11110)
length = 0x6F0C, unknown = 0, nObj = 14 -> 7 Meshes. Pro Mesh (Vertex/Normal-Block wird von Tri UND Quad geteilt, also NICHT doppelt zaehlen): Mesh0 199 V / 84 Tri + 134 Quad; Mesh1 4 V / 0+1 (DUMMY); Mesh2 635 V / 248 Tri + 467 Quad (der KOERPER); Mesh3 28 V / 26 Tri + 0; Mesh4 4 V / 0+1 (DUMMY); Mesh5 4 V / 0+1 (DUMMY); Mesh6 4 V / 0+1 (DUMMY). Summe 963 Faces.

**Beleg:** EM36.EMD@0x11110 = 0C 6F 00 00 / 00 00 00 00 / 0E 00 00 00; Mesh-Header ab @0x1111C, Stride 56. Mesh0-Header: tri_v_off=0x00188 nv=199, tri_n_off=0x01CF8, tri_f_off=0x03868 nf=84, tri_uv=0x06F00, quad_v_off=0x00188 (IDENTISCH mit tri_v_off), quad_f_off=0x03C58 nf=134, quad_uv=0x072F0. Mesh2: v_off=0x007E0 nv=635, tri_f=0x044C8 nf=248, quad_f=0x05068 nf=467.

**Umsetzung:** re15_md1_parse (md1_common.c:44) liest das fehlerfrei (alle Bounds innerhalb 42404). MD1_MAX_MESHES=32 reicht. Die Falle tri_vertex_count/quad_vertex_count auf demselben Block ist im Port bereits korrekt (beide Zeiger auf base+tv_off bzw. base+qv_off, hier gleich).

### MESH EM37 (MD1 @0xD984)
length = 0xA20, nObj = 8 -> 4 Meshes: Mesh0 66 V / 5 Tri + 60 Quad (das einzige echte Mesh), Mesh1/2/3 je 4 V / 1 Quad (DUMMY). Summe 68 Faces. Mesh-Zahl == Bone-Zahl (4), also greift mesh_idx == bone_idx.

**Beleg:** EM37.EMD@0xD984 = 20 0A 00 00 / 00 00 00 00 / 08 00 00 00; Mesh0-Header @0xD990: tri_v_off=0x000E0 nv=66, tri_f=0x005E8 nf=5, quad_f=0x00624 nf=60, quad_uv=0x00A50 (+60*16 = 0x0E10, +12 = 3612 < 3660).

**Umsetzung:** Ohne Aenderung zeichenbar, sobald der Ladeweg existiert.

### DUMMY-Meshes erkennen
Die 4-Vertex-Meshes sind Capcoms Leerplatz-Fueller und duerfen NICHT als Geometrie gelten: identische Vertexliste (-5,0,5)(-5,0,-5)(5,0,5)(5,0,-5) und ein einziges Quad (1,1,3,0,0,3,2,2). Betroffen: EM36 Mesh 1,4,5,6 und EM37 Mesh 1,2,3.

**Beleg:** EM36 Mesh1 Vertices @0x1111C+56*1 -> v_off 0x007C0: FB FF 00 00 05 00 00 00 | FB FF 00 00 FB FF 00 00 | 05 00 00 00 05 00 00 00 | 05 00 00 00 FB FF 00 00; Quad-UV = 70 FE 00 78 70 F6 80 00 77 FE 00 00 77 F6 00 00 (byte-gleich in Mesh 4/5/6). EM37 Mesh1-3 identisch mit UV = 7A 03 00 78 7A 00 80 00 7E 03 00 00 7E 00 00 00.

**Umsetzung:** Optional im Renderer ueberspringen (10x10-Quad, im Original subpixelig). Kein Muss — sie schaden nicht, kosten aber je 1 Face.

### ⛔ MESH-ZU-BONE-BINDUNG bei 7 Meshes / 2 Bones (EM36)
RE2 baut die Modellinstanz NICHT pro Bone, sondern PRO MESH: Schleifenzaehler = entity+0x107 = (*(u32*)(MD1_hdr-4)) >> 1 = nObj/2 = 7. Pro Iteration i: Work-Struct-Stride 172, MD1-Mesh-Header-Stride 56, Bind-Offset = 6-Byte-Lesung aus dem EMR-Positionsstrom (laeuft ueber die Positionstabelle HINAUS), Eltern-Matrix nur fuer i < bone_count aus der Child-Traversierung; ab i >= bone_count wird die globale IDENTITAETS-MATRIX @0x8009db44 eingetragen. Gemessene Bind-Offsets fuer EM36: work0 (0,-4534,0), work1 (2146,807,0), work2 (1,8,0), work3 (9,1,0), work4 (0,-4534,0), work5 (0,0,0), work6 (0,0,0).

**Beleg:** Zaehler: 0x80028324-38 (lw v0,20(a0); lw v0,-4(v0); srl v0,v0,1; sb v0,263(a0)). Schleife: 0x800283ac lbu s4,263(a0); 0x80028500-2c Positionslesung (lh 0(t2) / lh -2(s3) / lh 0(s3), t2 += 6, s3 += 6); 0x8002853c beq bone_count,zero -> 0x800285dc; 0x800285dc-e4 lui/addiu 0x8009db44 + sw v0,-32(s1); 0x800285ec-f4 s1 += 172, s2 += 172, s5 += 56. Identitaet @0x8009db44 = 00 10 00 00 | 00 00 00 00 | 00 10 00 00 | 00 00 00 00 | 00 10 00 00 (Q12-Einheitsmatrix, t = 0). Bind-Offsets abgelesen aus EM36.EMD@0x6028 + i*6 (i=2..6 liest 0x6034 ff = Child-Tabelle und Keyframe 0).

**Umsetzung:** main.c (Gegner-/NPC-Zeichenschleife, `int nmi = nbi;` + `if (nmi >= npc_md1->mesh_count) continue;`, ~Zeile 7143-7150 und die Zwillingsstellen ~6296/6521) muss fuer diese Bank ueber MESHES statt Bones laufen: fuer i < skel.bone_count wie bisher die Bone-Matrix, fuer i >= bone_count die Einheitsmatrix + den obigen Bind-Offset, additiv zur Entity-Weltmatrix. Ohne das fehlen bei EM36 die Meshes 2..6 — darunter der 635-Vertex-Koerper.

### dir[0] von EM36 = MORPH-Block (5 Ziele)
19112 B, Aufbau: [+0 u32 Mesh-Bitmaske = 0x00000004 -> nur Mesh 2][+4 u32 0x28][+8 u32 0x0C][+12 Eintrag: u32 quelle_off = 0x34, u32 param = 0xEE4, u32 count = 4, danach count*4 B, davon u16 @+12 = Blend-Gewicht 0..4095 (Startwert 0)] ... ab rel 0x34 fuenf Vertex-Ziele a 635 Vertices (s16 x,y,z, 6 B/Vertex, 3810 B auf 3812 gepaddet). Ziel 0 ist BYTE-IDENTISCH mit den MD1-Basisvertices von Mesh 2.

**Beleg:** EM36.EMD@0x08 = 04 00 00 00 | 28 00 00 00 | 0C 00 00 00 | 34 00 00 00 | E4 0E 00 00 | 04 00 00 00. Zieloffsets (Datei): 0x003C, 0x0F20, 0x1E04, 0x2CE8, 0x3BCC; 0x3BCC+0xEE4 = 0x4AB0 == D[1] (Sektion geht exakt auf). Ziel0 == MD1-Mesh2-Vertices (635/635 Tripel gleich, verglichen gegen 0x11110+12+0x7E0). Binder: FUN_8004b3b8 @0x8004b3b8 (sw a1,444(a0); beq block[0],zero -> Ende; a1 = block+12; Schleife ueber (MD1-4)>>1 Meshes, Bit-Test `sllv v1,t1,a3` / `and v0,v0,v1`, Trefferziel a0 = work+136, Weiterschaltung a1 += entry[2]*4 + 12). Aufrufer: 0x8001b214, 0x80057674, 0x80057d08.

**Umsetzung:** Fuer die ERSTE zeichnende Fassung IGNORIEREN — siehe naechster Posten. Wenn spaeter gebraucht: Blend = (4095-w)*ZielA + w*ZielB, 8-Byte-Zielvertexpuffer, 6-Byte-Quellvertices (FUN_8004b434 @0x8004b444-0x8004b560).

### ⛔ Der Morph-Anwender ist im RE2-Retail TOTER CODE
FUN_8004b434 (die Funktion, die den Morph tatsaechlich in den Vertexpuffer rechnet) wird im RE2-Leon-Image NIRGENDS aufgerufen — weder per jal, noch als Wort-Konstante, noch aus den beiden KI-Overlays. Der Block wird also gebunden (work+136), aber nie angewendet.

**Beleg:** jal-Scan ueber info/re2leon/PSX.EXE, SLUS_007.48 und alle Dateien unter info/re2leon/ + build/extracted/re2_ems/*ai1.BIN: 0 Treffer auf 0x8004b434 (zur Kontrolle: 0x8004b3b8 liefert 3 Treffer). Byte-Suche nach 34 B4 04 80 in PSX.EXE: 0 Treffer. Kein `lw rX,136(rY)` mit anschliessendem jalr auf rX im gesamten EXE.

**Umsetzung:** Der Port zeichnet byte-true, wenn er Mesh 2 mit den MD1-Basisvertices (== Morph-Ziel 0) zeichnet. Kein Morph-Code noetig; dir[0] darf weiter ignoriert werden.

### TEXTUR EM36 (132640 B)
TIM, magic 0x10, flag 0x09 = 8 bpp + CLUT. CLUT-Block: 1548 B, VRAM (0,480), 256 x 3 -> DREI CLUTs (Zeilen 480/481/482). Pixelblock: 131084 B, VRAM (0,0), data_w = 256 Halbwoerter = 512 Pixel breit, 256 hoch. Keine Restbytes.

**Beleg:** CDEMD0_EM36_tim.TIM@0x00 = 10 00 00 00 / 09 00 00 00; @0x08 = 0C 06 00 00 (1548) / 00 00 / E0 01 (480) / 00 01 (256) / 03 00 (3); Pixelheader @0x614 = 0C 00 02 00 (131084) / 00 00 / 00 00 / 00 01 (256) / 00 01 (256). 8+1548+131084 = 132640 = Dateigroesse.

**Umsetzung:** re15_tim_parse + re15_render_pc_upload_tim_slot koennen das: n_cluts = clut_entries/256 = 768/256 = 3, gestapelte Textur 512 x 768. CLUT-Auswahl im Renderer (render_pc.c:2067 `clut_y = (clut>>6)&0x1FF; clut_idx = clut_y - base_y`) liefert fuer die vorkommenden Worte 0x7800/0x7840/0x7880 die Indizes 0/1/2. Keine Aenderung.

### TEXTUR EM37 (33312 B)
TIM, flag 0x09 = 8 bpp + CLUT. CLUT: 524 B, VRAM (0,480), 256 x 1 -> EINE CLUT. Pixel: 32780 B, VRAM (0,0), 64 Halbwoerter = 128 Pixel breit, 256 hoch.

**Beleg:** CDEMD0_EM37_tim.TIM@0x00 = 10 00 00 00 / 09 00 00 00; CLUT-Header @0x08 = 0C 02 00 00 / 00 00 / E0 01 / 00 01 / 01 00; Pixelheader @0x214 = 0C 80 00 00 / 00 00 / 00 00 / 40 00 (64) / 00 01 (256). 8+524+32780 = 33312.

**Umsetzung:** Ohne Aenderung ladbar.

### Texturseiten / CLUT-Worte je Mesh
EM36: Mesh0 cluts {0x7840, 0x7880} pages {0x82, 0x83}; Mesh2 cluts {0x7800, 0x7880} pages {0x80, 0x83}; Mesh3 clut {0x7840} page {0x82}; Dummies clut 0x7800 page 0x80. EM37: alle Faces clut 0x7800 page 0x80. Umrechnung: page & 0x0F -> x-Offset * 128 Pixel bei 8 bpp (0x80 -> 0, 0x82 -> 256, 0x83 -> 384), passt in die 512 Pixel breite EM36-Textur; clut -> y = (clut>>6)&0x1FF = 480/481/482 -> Slab 0/1/2.

**Beleg:** Ausgelesen aus allen Tri-UV (+2 clut, +6 page) und Quad-UV-Records beider MD1: EM36 @0x11110+12+{0x06F00,0x072F0,0x07B60,0x08700,0x0A430,...}; EM37 @0xD984+12+{0x00A14,0x00A50,0x00E10,0x00E20,0x00E30}. Port-Umrechnung: main.c `page_off = (uv->page & 0x000F) * 128`.

**Umsetzung:** Deckt sich mit dem vorhandenen Renderer. Wichtig: EM36 braucht die 3-CLUT-Stapelung, sonst faerben Mesh0/Mesh3 falsch.

### PORT-ABGLEICH: Parser liest beide Baenke
re2_emd_parse_bank() gibt fuer BEIDE Dateien 0 zurueck. Schritt fuer Schritt: dir_cnt == 8 OK; dir_off+32 <= Dateigroesse OK; re15_md1_parse(D[7]) OK (alle Blockgrenzen innerhalb der Sektion, groesster Zugriff EM36 0x0A568+12 < 42404, EM37 0x00E40+12 = 3660 <= 3660); loco-Paar OK; own-Paar liefert -3 (offset0 == 0) -> own_ok = 0, harmlos; victim-Paar OK (aber falsches Rig, s. eigener Posten); Bestwahl ueber rd_u16(edd+2)/4 = 11 (EM36) bzw. 24 (EM37) gegen 0 und 1 bzw. 0 und 6 -> Paar 1 gewinnt.

**Beleg:** re15_port/engine/src/re2_ems.c:97-140 (re2_emd_parse_bank), emd_common.c:49-99 (parse_animation, Grenzen RE15_EMD_MAX_CLIPS=64 / RE15_EMD_MAX_FRAMES=1664 gegen 11/1360 und 24/1207), emd_common.c:130-197 (parse_skeleton, RE15_EMD_MAX_BONES=32 gegen 2/4), md1_common.c:44-121 (MD1_MAX_MESHES=32 gegen 7/4).

**Umsetzung:** KEINE Parser-Aenderung noetig. Die dir_cnt-Pruefung ist erfuellt.

### PORT-ABGLEICH: TOC fuehrt 0x36/0x37 bereits
gen/re2_ems_toc.inc enthaelt die Records: kind 0x36 rec2 (TIM) Sektor 3468 / 132640 B / Offset 0x6C6000, rec3 (EMD) Sektor 3533 / 112340 B / Offset 0x6E6800; kind 0x37 rec2 Sektor 3612 / 33312 B / 0x70E000, rec3 Sektor 3629 / 59376 B / 0x716800. Die Groessen stimmen exakt mit den ausgeschnittenen Chunks ueberein.

**Beleg:** Indexformel re2_ems_toc_entry: ((kind-0x10)*4 + rec)*2 in s_re2_cdemd0_toc[600]; ausgelesene Werte s.o. Chunk-Groessen: CDEMD0_EM36_emd.EMD = 112340, _tim.TIM = 132640, CDEMD0_EM37_emd.EMD = 59376, _tim.TIM = 33312.

**Umsetzung:** re2_ems_load_bank(ems, sz, 0x36, eb, &tim) bzw. 0x37 funktioniert sofort — es fehlt nur der Aufrufer.

### ⛔ PORT-LUECKE 1: das Lade-Tor kennt 0x36/0x37 nicht
pc_enemy_load_ex() betritt den RE2-Zweig nur bei `allow_re2 && re15_ai_re2_for_type(type) && re15_re2_owns_type(type)`. re15_re2_owns_type() deckt nur die Zombie-Familie + 0x20/0x21/0x25/0x26 ab — 0x36/0x37 fallen durch und landen im RE1.5-Pfad.

**Beleg:** re15_port/platform/pc/main.c:713 (Gate) ruft re15_re2_owns_type aus re15_port/engine/src/enemy_ai_re2_dog.c:72-78 (`return re15_re2z_owns_type(type) || type == 0x20u || 0x21u || 0x25u || 0x26u;`).

**Umsetzung:** NICHT einfach 0x36 in re15_re2_owns_type eintragen: Typ 0x36 EXISTIERT in RE1.5 (re15_ems.c:s_ems_order Index 17 = 0x36) und wuerde dort die Bank hijacken — exakt die Falle, die bei 0x26 schon einmal zugeschlagen hat. Stattdessen ein eigenes Tor (analog zum `allow_re2`-Parameter) fuer die Boss-Population, das nur greift, wenn der Raum die RE2-Boss-Records fuehrt.

### ⛔ PORT-LUECKE 2: pc_enemy_hybrid_re15_models laeuft bedingungslos
Nach erfolgreichem pc_enemy_load_re2 ruft main.c bedingungslos pc_enemy_hybrid_re15_models(). Fuer 0x37 gibt es KEIN RE1.5-Gegenstueck (weder EMD/EM37.EMD noch ein s_ems_order-Eintrag) -> pc_enemy_read_re15_emd liefert NULL -> stderr `Hybrid EM37: RE1.5-Modell nicht gefunden`. Fuer 0x36 wuerde die RE1.5-Bank zwar gefunden, aber re2_hybrid_perm(0x36) gibt -1 -> re2_hybrid_apply -2 -> stderr `rc=-2`. In beiden Faellen bleibt die reine RE2-Bank stehen (funktional richtig, aber der Hybrid-Aufruf ist sinnlos und der 0x36-Fall liest unnoetig ein fremdes Modell).

**Beleg:** main.c:719-723 (`if (pc_enemy_load_re2(type, eb)) { pc_enemy_hybrid_re15_models(type, eb); return; }`); pc_enemy_hybrid_re15_models main.c:551-... ; re2_hybrid_perm (re15_port/engine/src/re2_ems.c:186-201) hat kein case 0x36/0x37 -> n = -1; re2_hybrid_apply prueft `if (n <= 0 || !perm) return -2`.

**Umsetzung:** In main.c den Hybrid-Aufruf gaten: `if (re2_hybrid_perm((int)type, NULL) > 0) pc_enemy_hybrid_re15_models(type, eb);` — dann bleibt fuer 0x36/0x37 die reine RE2-Bank ohne Fehlermeldung und ohne Fremd-EMD-Lesung.

### ⛔ PORT-LUECKE 3: Renderer zeichnet nur bone_count Meshes
Die Gegner-Zeichenschleife iteriert Bones und setzt mesh_idx = bone_idx, mit `if (nmi >= npc_md1->mesh_count) continue;`. Bei EM36 (2 Bones / 7 Meshes) zeichnet sie Mesh 0 und Mesh 1 (Letzteres ein 10x10-Dummy) — der Koerper (Mesh 2, 635 V / 715 Faces) und Mesh 3 (28 V / 26 Faces) bleiben unsichtbar. Bei EM37 (4/4) ist die Schleife korrekt.

**Beleg:** re15_port/platform/pc/main.c ~7143 `int nmi = nbi;` / ~7150 `if (nmi >= npc_md1->mesh_count) continue;` (Zwillingsstellen fuer Spieler/Waffe ~6296 und ~6521); emd_common.c:190 setzt bone_mesh_index[b] = b (1:1-Regel).

**Umsetzung:** Zusatzdurchlauf nach der Bone-Schleife: fuer mesh_idx = bone_count .. mesh_count-1 dieselbe Tri/Quad-Ausgabe, aber mit Rotationsmatrix = Q12-Einheit (0x1000-Diagonale) und Translation = Entity-Weltposition + Bind-Offset aus dem 6-Byte-Ueberlauf (EM36: (1,8,0), (9,1,0), (0,-4534,0), (0,0,0), (0,0,0) fuer Mesh 2..6) — genau das, was FUN_80028368 baut.

### PORT-DETAIL: keyframe_count wird zu gross berechnet
re15_emd_parse_skeleton bekommt emr_size = emd_size - struct_off (Rest der DATEI, nicht der Sektion) und rechnet keyframe_count = (emr_size - kf_off)/kf_size. Fuer EM36 ergibt das 3654 statt 1360, fuer EM37 1207 (zufaellig richtig? nein: (59376-0x134C-0x34)/32 = 1806).

**Beleg:** re2_ems.c:66-70 (`re15_emd_parse_skeleton(emd + struct_off, emd_size - struct_off, out_skel)`), emd_common.c:196 (`keyframe_count = keyframe_data_size / keyframe_size`). Echte Werte: EM36 (32672-32)/24 = 1360; EM37 (38676-52)/32 = 1207.

**Umsetzung:** Nur permissiv, kein Absturz (die EDD-Indizes bleiben <= 1359 bzw. <= 1206). Sauber waere, re2_emd_parse_bank die SEKTIONSGROESSE (D[i+1]-D[i]) statt des Dateirests zu uebergeben — dann pinnt keyframe_count die echten 1360/1207 und schuetzt gegen fehlerhafte Clip-Indizes.

### Kapazitaets-Check der Port-Grenzen
Alle statischen Grenzen reichen: Clips 11/24 gegen RE15_EMD_MAX_CLIPS = 64; Frames 1360/1207 gegen RE15_EMD_MAX_FRAMES = 1664 (EM36 nutzt 82 % — knapp, aber ausreichend); Bones 2/4 gegen RE15_EMD_MAX_BONES = 32; Meshes 7/4 gegen MD1_MAX_MESHES = 32; Textur-Slot = 11 + Bank-Index muss < 24 bleiben.

**Beleg:** re15_port/include/re15_emd.h:47-49, re15_port/include/re15_md1.h:41; main.c:pc_enemy_load_re2 (`int slot = 11 + (int)(eb - g_enemy); if (... && slot < 24)`).

**Umsetzung:** Keine Aenderung. Nur beachten: EM36 belegt 1360 der 1664 Frame-Slots — ein weiterer grosser Bank-Parse in denselben Puffer wuerde nicht passen (die Struktur ist aber pro Bank).

## Offen
- Warum RE2 den 635-Vertex-Koerper (EM36 Mesh 2) an die IDENTITAETS-Matrix @0x8009db44 haengt statt an die Entity-Matrix, ist NICHT geklaert. FUN_80028368 setzt work[0]+116 = entity+0x24 (Entity-Matrix), fuer Meshes >= bone_count aber 0x8009db44. Entweder ueberschreibt boss-spezifischer Code work[i]+116 pro Frame, oder die Zeichenroutine behandelt den Identitaets-Zeiger als 'im Entity-Raum'. Ungeklaert, weil die eigentliche Zeichenroutine der Work-Kette (Verbraucher von work+116/+148) noch nicht disassembliert ist. Naechster Weg: FUN_8002cbc4 / FUN_8002cd24 (beide aus dem Builder @0x800284e0 / @0x800284f0 aufgerufen) und FUN_80029614 (aus FUN_8002959C @0x800295fc) verfolgen.
- Der Cluster 0x8004b770 / 0x8004b8d0 / 0x8004b980 / 0x8004bb28 / 0x8004be30 / 0x8004bfa8 liest ebenfalls entity+0x1BC (den Morph-Block). Diese Adressen liegen INNERHALB groesserer Funktionen, deren Einsprungpunkte noch nicht bestimmt sind. Wenn dort doch eine lebende Anwendung des Morphs steckt, waere die Aussage 'Morph = toter Code' einzuschraenken. Byte-Stand heute: kein einziger Verweis auf 0x8004b434 im gesamten Image.
- entity+0x1BC ist doppelt belegt: der Binder schreibt dort den Morph-Block-Zeiger, waehrend im KI-Overlay von 0x36 dieselbe Byte-Position als Z-Translation einer MATRIX bei +0x1A0 gelesen wird (CDEMD0_EM36_ai1.BIN @0x80101DF4 / 0x80102044 / 0x80102270, jeweils lw v1,436/440/444(s0) mit anschliessendem addiu s0,s0,416). Welche der beiden Deutungen fuer welchen Basiszeiger gilt, ist offen — s0 ist dort nachweislich NICHT die Entity (s1 ist es, `lh a1,118(s1)` = +0x76 Yaw).
- Wie die beiden KI-Overlays ihren Clip setzen, ist nicht gefunden: es gibt in CDEMD0_EM36_ai1.BIN und CDEMD0_EM37_ai1.BIN KEIN direktes `sb rX,332(rY)` (entity+0x14C) und kein `sb rX,148(rY)` (+0x94). Der Setzer laeuft ueber einen Helfer mit indirektem Basiszeiger. Fuer die reine Zeichnen-Aufgabe irrelevant, fuer die KI-Straenge zu klaeren (Kandidaten: die overlay-lokalen 0x80104E5C / 0x80105064 / 0x8010503C in EM36 bzw. 0x80104BA4 / 0x80104C30 in EM37).
- Die Bedeutung der Felder block[1] = 0x28 und block[2] = 0x0C im Morph-Block wird von FUN_8004b3b8/FUN_8004b434 nicht gelesen; ebenso ungeklaert, warum FUN_8004b434 die zweite Blend-Quelle bei a2 + 2*entry[1] (= Ziel 2) statt bei a2 + entry[1] (= Ziel 1) ansetzt. Nur relevant, falls der Morph doch portiert werden soll.
- Die vier u32 hinter entry[2] (Datei-Offsets 0x18..0x27 in EM36.EMD) sind im Auslieferungsstand alle 0; welcher Code das Blend-Gewicht (u16 @0x20) zur Laufzeit schreibt, ist nicht gefunden.
- Fuer den Opfer-Pfad (Paar 3, 15-Bone-Pool) fehlt die Quelle der STRUKTUR. Im Original kommt sie nicht aus D[2] dieser EMDs. Zu klaeren, welches Rig RE2 dort einsetzt (Verdacht: Leons PL00-EMR, wie es der Port fuer die anderen RE2-Gegner ohnehin annimmt) — sonst bleibt die Griff-/Kill-Animation des Bosses ungezeichnet.


---

# Endkampf-Sound RE2 (Typ 0x36 + 0x37, ROOM7040): ENEMSE-Bank 25 + BGM MAIN23/SUB_2C/2F/30

Die Raum-Sound-Bank des Endkampfs ist ENEMSE-Bank 25. Beweiskette: der Sce_em_set-Record in info/re2leon/PL0/RDT/room7040/scd/sub00.scd traegt bei allen 5 Gegnern (0x36 Slot0, 4x 0x37) das Byte +7 = 0x27; FUN_80052b38 vergleicht GENAU dieses Byte (ueber entity+0x1FA) gegen die Paar-Tabelle @0x800a7400, deren Zeile 25 = {0x27,0x00} ist -> Bank 25, erste Haelfte, also flag2000 = 0. Bank 25 (TOC-Record @0x800a7cac) fuehrt in ihrem EDT GENAU die Ids 0..15 live und 16..31 als 0xFFFFFFFF - und EM36 ruft {0,6,7,8,9,10,11,12,13,14}, EM37 ruft {0,1,2,3,15}; zusammen 0..15 ohne 4/5. Der Zensus zaehlt 38 jal 0x8005BD6C im 0x36-Chunk und 18 im 0x37-Chunk, alle a0 rueckwaerts aufgeloest und jeder Stelle Zustand(+0x04)/Verhalten(+0x05)/Unterzustand(+0x06) bzw. Aktion(+0x218/+0x219) zugeordnet. Der Weg SE-Id -> EDT-Record -> prog/tone -> VagAtr.vag -> ENEMSE_NNNNN.wav ist ausgerechnet UND ueber alle 73 Baenke / 980 wav-Dateien byte-genau gegengeprueft (wav-data-Bytes == (VAG_Bytes/16 - 2)*56, 980/980 exakt): Bank 25 = ENEMSE_00279.wav .. ENEMSE_00292.wav. BGM: MAIN23.BGM + SUB_2C.BGM + SUB_2F.BGM + SUB_30.BGM liegen in info/re2leon/COMMON/SOUND/, Container-Layout (SEQ1@0, [SEQ2], VH, VB) an allen vier Dateien durch den 16-Byte-Trailer verifiziert. Port-Seite: die komplette ENEMSE-Maschine existiert bereits in audio_pc.c; es fehlt nur (a) re15_audio_re2_enemy_bank(25) fuer ROOM7040/ROOMG040, (b) die SE-Aufrufe in den (noch nicht vorhandenen) EM36/EM37-Brains, (c) die Korrektur, dass re2_enemse_room_add_kind die SOUND-ID (+7) und nicht den AI-kind bekommt.

## Posten

### Bankwahl ROOM7040
Der Sce_em_set-Record (Opcode 0x44, 22 B) fuer den Boss lautet: 44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00. Byte[3] = 0x36 (AI-Typ), Byte[7] = 0x27 (SOUND-ID). Die vier Begleiter: 44 00 0N 37 .. 00 27 .. (N=1..4), Byte[7] ebenfalls 0x27.

**Beleg:** info/re2leon/PL0/RDT/room7040/scd/sub00.scd, Datei-Offset 0x6C (Boss) sowie 0x82/0x98/0xAE/0xC4 (Begleiter). Gegenprobe der Feldlage: record+0x0A/0x0C/0x0E = 0x8300/0x0000/0x8300 = (-32000,0,-32000), uebernommen nach entity+68/+56 bzw. +70/+60 @0x800572c8-0x800572f4.

**Umsetzung:** Der Port darf NICHT den AI-kind (0x36/0x37) in re2_enemse_room_add_kind() geben, sondern 0x27. Damit ist der Kommentar in re15_port/engine/src/re2_ems.c ('die RE2-Raumdaten liegen nicht im Repo') widerlegt - info/re2leon/PL0/RDT/*/scd/*.scd enthaelt die Records.

### Bankwahl ROOM7040
FUN_80052b38 vergleicht die Raum-SOUND-IDs (DAT_800d8cd0/0xd8cd1) gegen die Paar-Tabelle @0x800a7400 (2 B je Zeile, Abbruch bei kindA==0xFF); der Zeilenindex IST die Bank. Zeile 25 = {0x27, 0x00}.

**Beleg:** Tabelle @0x800a7400: Zeile 24 = 26 00, Zeile 25 = 27 00 (Bytes @0x800a7430: 26 00 27 00 1a 00 28 00). Schleife @0x80052b7c-0x80052bd8, t0-Inkrement im Delay-Slot @0x80052be4, Treffer-Store `sb t0,0x800d424b` @0x80052c18.

**Umsetzung:** re15_audio_re2_enemy_bank(25) beim Betreten von ROOM7040 / ROOMG040. re2_enemse_select_bank(0x27, 0, &flag) liefert das schon korrekt (Zeile 25, out_flag_kind = k1 = 0).

### Bankwahl ROOM7040
flag2000 = 0 fuer beide Typen: Paarzeile 25 hat kindB = 0x00, es gibt also keine 'zweite Haelfte', kein Entity bekommt word0 |= 0x2000, und die EDT-Ids 16..31 der Bank 25 sind alle 0xFFFFFFFF.

**Beleg:** Zeile 25 = {0x27,0x00} @0x800a7434; EDT Bank 25 Bytes 0x40..0x7F (Datei ENEMSE.VBS @0x1F2840) = 0xFF durchgaengig. FUN_8005bd6c-Kopf @0x8005bd90-a4: `lw v0,0(s2); andi 0x2000; bne -> addiu s4,a0,16`.

**Umsetzung:** re15_audio_re2_enemy_se(se_id, 0) - flag2000 hart 0 fuer EM36/EM37 in ROOM7040.

### ENEMSE-Bank-TOC-Layout
TOC @0x800a7b1c, 16 B je Bank, 73 Baenke (0..72). Felder: +0x00 u32 EDT+VH-Blobgroesse; +0x04 u24 Sektor-Offset des Blobs (relativ zur ENEMSE.VBS-LBA); +0x07 u8 CD-Flag; +0x08 u32 VB-Groesse; +0x0C u24 VB-Sektor-Offset; +0x0F u8 Flag.

**Beleg:** Lader FUN_8005a09c: `lw v1,0x800a7b1c+bank*16` @0x8005a148 -> DAT_800d5308; `lhu 0x800a7b20` + `lbu 0x800a7b22 << 16` @0x8005a180-98 -> + LBA (u16@0x800993b0 + u8@0x800993b2<<16) -> DAT_800d5314; `lbu 0x800a7b23` @0x8005a1ac -> DAT_800d531e. Zweiter Block identisch mit t1 = bank*16|8 @0x8005a264-0x8005a2ec. Selbstprobe: Bank n+1 Blob-Sektor == Bank n VB-Sektor + ceil(vb_size/2048), fuer alle 73 Baenke; letzte Bank endet auf Sektor 3269 == ENEMSE.VBS 6694912 B / 2048 exakt.

**Umsetzung:** Bereits korrekt in re15_port/engine/src/gen/re2_ems_toc.inc (s_re2_enemse_toc[292]) und re2_enemse_toc_entry(). Nichts zu tun.

### ENEMSE-Blob-Layout
Der Blob = EDT (128 B = 32 Records) + VAB-VH + 8-B-Trailer. Der Trailer-u32 @[blobgroesse-8] ist der VH-Offset. Bank 25: 0x00000080 -> VH ('pBAV') bei Blob+0x80.

**Beleg:** ENEMSE.VBS @0x1F2800 (Sektor 997) Bytes 0x80..0x83 = 70 42 41 56 ('pBAV'); Blob[3752-8..3751] = 80 00 00 00 00 00 00 00. Groessenprobe: 128 + 32 + 2048 + 512*ps + 512 + 8 = 0xCA8 fuer ps=1 und 0xEA8 fuer ps=2 - passt auf alle 73 TOC-Eintraege.

**Umsetzung:** Der Port liest den Trailer bereits so (audio_pc.c load_re2_enemy_se_pc, `vh_off = u32 @ edt[rec.edt_size-8]`), s_re2se_map_count wird 0x80/4 = 32. Passt.

### ENEMSE-Bank 25 (ROOM7040)
TOC-Record Bank 25 @0x800a7cac = a8 0e 00 00 | e5 03 00 00 | 10 7a 01 00 | e7 03 00 2a. EDT+VH-Groesse 0xEA8 = 3752 B, Blob-Sektor 997 (Datei-Offset 0x1F2800 = 2041856), VB-Groesse 0x17A10 = 96784 B, VB-Sektor 999 (Datei-Offset 0x1F3800 = 2045952).

**Beleg:** PSX.EXE @0x800a7cac (= 0x800a7b1c + 25*16). VBS-Selbstprobe: 999 = 997+2; 999 + ceil(96784/2048)=48 = 1047 = Blob-Sektor Bank 26 (@0x800a7cbc: 17 04 00 00).

**Umsetzung:** Konstanten fuer den Port bzw. Testfixture; re2_enemse_toc_entry(25,...) liefert edt_off=2041856, edt_size=3752, vbd_off=2045952, vbd_size=96784.

### ENEMSE-Bank 25 VAB-Kopf
VAB 'pBAV' @ Datei 0x1F2880: ver=7, fsize=100400, nProg(ps)=2, nTone(ts)=19, nVAG(vs)=15. ProgAtr[0].tones=16, ProgAtr[1].tones=3.

**Beleg:** ENEMSE.VBS @0x1F2880: 70 42 41 56 07 00 00 00 00 00 00 00 30 88 01 00 | ee ee 02 00 13 00 0f 00 7f 40 00 00 ff ff ff ff; ProgAtr @0x1F28A0 = 10 7f ff ff 40 ff .. (0x10=16 Tones), @0x1F28B0 = 03 7f .. (3 Tones). VAG-Offset-Tabelle (VH+2080+512*2) summiert exakt auf 96784 == VB-Groesse.

**Umsetzung:** Testpin fuer den Port: nach load_re2_enemy_se_pc(25) muss s_re2se_vab.vag_count == 15 sein.

### EDT-Record-Layout (4 B je SE-Id)
byte0: Bit7 -> VAB-Handle-Override = byte0&0x7F, sonst globales Handle DAT_800d4c4b. byte1&0x7F = PROGRAMM. byte2>>4 = TONE. byte2&0x0F = Prio-Nibble (Argument des Kanal-Gates FUN_8005c92c). byte3&0x1F = SPU-Kanal. byte3>>5 = Anzahl ZUSAETZLICHER Layer (je Layer tone+1 UND chan+1). Record == 0xFFFFFFFF -> nichts.

**Beleg:** FUN_8005bd6c: `lw v0,0(s0); beq v0,-1 -> ende` @0x8005bdc0-cc; `lbu v1,0(s0); andi 0x80; andi s3,v1,0x7f` @0x8005bddc-f0; `lbu v0,3(s0); lbu v1,2(s0); andi s1,v0,0x1f; andi s5,v1,0xf; jal 0x8005c92c(a0=s1,a1=s5)` @0x8005bdf4-0c; `srl t0,v0,4` (tone) @0x8005be34; `andi t2,v1,0x7f` (prog) @0x8005be38; `srl t1,v0,5` (extra) @0x8005be44; Tone-Adresse `v1 = (prog<<9) + (tone<<5) + 2080` @0x8005be40-50, addiert auf DAT_800d75ac (VH-Basis) - 2080 = 32 (VabHdr) + 128*16 (ProgAtr), 512 = 16 VagAtr a 32 B. Layer-Schleife @0x8005bf14-0x8005bfc0 mit `addiu s1,s1,1` @0x8005bf3c und `addiu t0,t0,1` @0x8005bf44.

**Umsetzung:** re2_enemse_decode_entry() in re15_port/engine/src/re2_ems.c bildet das bereits 1:1 ab. Keine Aenderung noetig.

### Voice-Record von FUN_8005bd6c
Der Trigger schreibt in DAT_800d4f18 + chan*32: +0x00 = 1, +0x01 = SE-Id (a0, +16 bei entity-word0 & 0x2000), +0x04 = VAB-Handle, +0x06 = prog, +0x08 = tone, +0x0A = entity+0x06, +0x0C = entity+0x05, +0x0E/+0x10 = entity+0x02, +0x12 = 1, +0x14/+0x18/+0x1C = entity+0x38/+0x3C/+0x40 (Weltposition). Zusaetzlich Prio-Latch `sb (grp&7), 0x800d4ca0[chan*2]` und `sb chan, 0x800d4ca1[chan*2]`.

**Beleg:** @0x8005be78-0x8005bf18 (sb s4,1(a0) / sh s3,4 / sh t2,6 / sh t0,8 / sh entity+6,10 / sh entity+5,12 / sh entity+2,14+16 / sw entity+56,20 / sw entity+60,24 / sw entity+64,28 / sh 1,18) sowie @0x8005be98/0x8005bea8.

**Umsetzung:** Die Position (+0x14/+0x18/+0x1C) ist der Hebel fuer 3D-Lautstaerke/Panning, den der Port noch nicht nutzt (audio_pc.c nutzt Tone-vol/pan). Fuer den Endkampf zunaechst hinnehmbar.

### SE-Id -> wav-Datei, Rechenweg
wavIndex(bank, vag) = cumOffset(bank) + (vag - 2) mit cumOffset(bank) = Summe(vs_k - 1) fuer k < bank. Pro Bank fehlt genau VAG 1 (der 48-B-Dummy) in der Extraktion; deshalb 1053 VAGs, aber 980 wav-Dateien (1053 - 73 = 980).

**Beleg:** Groessenprobe ueber ALLE 73 Baenke und ALLE 980 Dateien: wav-data-Chunk-Bytes == (VAG_Bytes/16 - 2) * 56 - 980/980 exakt, ENEMSE_00980.wav existiert nicht. Beispiel Bank 0 VAG 2 = 4496 B = 281 Bloecke -> (281-2)*56 = 15624 == data-Chunk von ENEMSE_00000.wav (Header 46 B, RIFF-Groesse 0x3D4A, data 0x3D08 = 15624).

**Umsetzung:** Fuer Bank 25: cumOffset = 279. Also VAG v -> ENEMSE_%05d.wav mit 279 + (v-2), gueltig fuer v = 2..15. VAG 1 hat KEINE Datei (Dummy).

### ENEMSE-Bank 25: EDT-Rohbytes
Ids 0..15 (Datei ENEMSE.VBS @0x1F2800): 00 00 13 03 | 00 01 13 03 | 00 00 33 02 | 00 00 43 02 | 00 00 53 03 | 00 00 63 03 | 00 00 73 04 | 00 00 83 04 | 00 00 93 03 | 00 00 a3 04 | 00 00 b3 04 | 00 00 c3 03 | 00 00 d3 04 | 00 00 e3 04 | 00 00 f3 03 | 00 01 23 03. Ids 16..31 = FF FF FF FF.

**Beleg:** ENEMSE.VBS Datei-Offset 0x1F2800..0x1F287F.

**Umsetzung:** Alle 16 Records: kein VAB-Override (byte0=0), Prio-Nibble ueberall 3, extra-Layer ueberall 0. Kanaele nur 2/3/4 -> passt in MIXER_RE2SE_CH_COUNT=8.

### ENEMSE-Bank 25: SE-Id 0
prog 0, tone 1, Prio-Nibble 3, Kanal 3, extra 0 -> VagAtr prog0/tone1: VAG 3, vol 110, pan 64, center 83, shift 57, min=max=60, adsr 80ff/1fc0 -> Datei ENEMSE_00280.wav (VAG 3 = 9008 B = 563 Bloecke, wav-data 31416 B - verifiziert).

**Beleg:** EDT-Record @0x1F2800 = 00 00 13 03; VagAtr @ VH+2080+0*512+1*32 (Datei 0x1F2880+0x820+0x20 = 0x1F30C0), vag-Feld u16 @+0x16 = 3.

**Umsetzung:** Haeufigste Id des 0x36 (10 Aufrufe) und des 0x37 (8 Aufrufe).

### ENEMSE-Bank 25: SE-Id 1
prog 1, tone 1, Prio 3, Kanal 3 -> VAG 12, vol 110, center 91, shift 0, min=max=60 -> ENEMSE_00289.wav (VAG 12 = 4448 B, wav-data 15456 B - verifiziert).

**Beleg:** EDT @0x1F2804 = 00 01 13 03; VagAtr prog1/tone1 @ Datei 0x1F32C0+0x20, vag = 12.

**Umsetzung:** Nur EM37 ruft Id 1 (genau 1x, 0x80103e24).

### ENEMSE-Bank 25: SE-Id 2
prog 0, tone 3, Prio 3, Kanal 2 -> VAG 13, vol 100, center 76, shift 0, min=max=62 -> ENEMSE_00290.wav (VAG 13 = 4288 B, wav-data 14896 B - verifiziert).

**Beleg:** EDT @0x1F2808 = 00 00 33 02; VagAtr prog0/tone3, vag = 13.

**Umsetzung:** Nur EM37 (3 Aufrufe).

### ENEMSE-Bank 25: SE-Id 3
prog 0, tone 4, Prio 3, Kanal 2 -> VAG 4, vol 127, center 81, shift 57, min=max=63 -> ENEMSE_00281.wav (VAG 4 = 5024 B, wav-data 17472 B - verifiziert).

**Beleg:** EDT @0x1F280C = 00 00 43 02; VagAtr prog0/tone4, vag = 4.

**Umsetzung:** Nur EM37 (3 Aufrufe).

### ENEMSE-Bank 25: SE-Id 4 (UNBENUTZT)
prog 0, tone 5, Prio 3, Kanal 3 -> VAG 9, vol 100, center 85, shift 0, min=max=64 -> ENEMSE_00286.wav (VAG 9 = 2672 B, wav-data 9240 B - verifiziert). Wird von EM36 UND EM37 nie gerufen.

**Beleg:** EDT @0x1F2810 = 00 00 53 03; Zensus: kein jal 0x8005BD6C mit a0=4 in beiden Chunks.

**Umsetzung:** Nicht verdrahten. Ausloeser vermutlich SCD Se_on oder EXE - offen.

### ENEMSE-Bank 25: SE-Id 5 (UNBENUTZT)
prog 0, tone 6, Prio 3, Kanal 3 -> VAG 15, vol 100, center 93, shift 0, min=max=65 -> ENEMSE_00292.wav (VAG 15 = 4352 B, wav-data 15120 B - verifiziert). Von EM36/EM37 nie gerufen.

**Beleg:** EDT @0x1F2814 = 00 00 63 03; Zensus ohne a0=5.

**Umsetzung:** Nicht verdrahten.

### ENEMSE-Bank 25: SE-Id 6
prog 0, tone 7, Prio 3, Kanal 4 -> VAG 6, vol 105, center 84, shift 57, min=max=66 -> ENEMSE_00283.wav (VAG 6 = 5792 B, wav-data 20160 B - verifiziert).

**Beleg:** EDT @0x1F2818 = 00 00 73 04; VagAtr prog0/tone7, vag = 6.

**Umsetzung:** 3 Aufrufe in EM36: 0x80101bb8 (+0x5=4/+0x6=0), 0x80104058 (+0x218=1/+0x219=2), 0x80104a0c (+0x218=4/+0x219=2).

### ENEMSE-Bank 25: SE-Id 7
prog 0, tone 8, Prio 3, Kanal 4 -> VAG 7, vol 120, center 91, shift 57, min=max=67 -> ENEMSE_00284.wav (VAG 7 = 17392 B = 1087 Bloecke, wav-data 60760 B - verifiziert; das laengste Sample der Bank).

**Beleg:** EDT @0x1F281C = 00 00 83 04; VagAtr prog0/tone8, vag = 7.

**Umsetzung:** Genau 1 Aufruf in EM36: 0x80101bd0 (+0x5=4 / +0x6=1).

### ENEMSE-Bank 25: SE-Id 8
prog 0, tone 9, Prio 3, Kanal 3 -> VAG 2, vol 127, center 92, shift 0, min=max=68 -> ENEMSE_00279.wav (VAG 2 = 3904 B, wav-data 13552 B - verifiziert).

**Beleg:** EDT @0x1F2820 = 00 00 93 03; VagAtr prog0/tone9, vag = 2.

**Umsetzung:** 2 Aufrufe in EM36: 0x80102940 (Zustand 2, +0x6=0) und 0x80102f78 (Zustand 3, +0x6=0).

### ENEMSE-Bank 25: SE-Id 9
prog 0, tone 10, Prio 3, Kanal 4 -> VAG 8, vol 127, center 99, shift 57, min=max=69 -> ENEMSE_00285.wav (VAG 8 = 11824 B, wav-data 41272 B - verifiziert).

**Beleg:** EDT @0x1F2824 = 00 00 a3 04; VagAtr prog0/tone10, vag = 8.

**Umsetzung:** 10 Aufrufe in EM36 - die haeufigste Id des Bosses.

### ENEMSE-Bank 25: SE-Id 10
prog 0, tone 11, Prio 3, Kanal 4 -> VAG 5, vol 127, center 100, shift 57, min=max=70 -> ENEMSE_00282.wav (VAG 5 = 8448 B, wav-data 29456 B - verifiziert).

**Beleg:** EDT @0x1F2828 = 00 00 b3 04; VagAtr prog0/tone11, vag = 5.

**Umsetzung:** 8 Aufrufe in EM36.

### ENEMSE-Bank 25: SE-Id 11
prog 0, tone 12, Prio 3, Kanal 3 -> VAG 14, vol 127, center 93, shift 0, min=max=71 -> ENEMSE_00291.wav (VAG 14 = 10560 B, wav-data 36848 B - verifiziert).

**Beleg:** EDT @0x1F282C = 00 00 c3 03; VagAtr prog0/tone12, vag = 14.

**Umsetzung:** 4 Aufrufe in EM36: 0x80100f80, 0x801012dc, 0x8010187c, 0x801018e4.

### ENEMSE-Bank 25: SE-Id 12
prog 0, tone 13, Prio 3, Kanal 4 -> VAG 10, vol 127, center 102, shift 0, min=max=72 -> ENEMSE_00287.wav (VAG 10 = 4928 B, wav-data 17136 B - verifiziert).

**Beleg:** EDT @0x1F2830 = 00 00 d3 04; VagAtr prog0/tone13, vag = 10.

**Umsetzung:** Genau 1 Aufruf in EM36: 0x80102b30 (Fn 0x80102ad0).

### ENEMSE-Bank 25: SE-Id 13 = STUMM
prog 0, tone 14, Prio 3, Kanal 4 -> VagAtr prog0/tone14 hat vol = 0 und vag = 1; VAG 1 ist der 48-B-Dummy (3 Bloecke), fuer den es KEINE extrahierte wav-Datei gibt. Die Id ist byte-true LAUTLOS.

**Beleg:** EDT @0x1F2834 = 00 00 e3 04; VagAtr prog0/tone14 = prior 0, mode 0, vol 0, pan 64, center 83, shift 59, min=max=73, vag=1. Bank-25-Offsettabelle: VAG 1 = 48 B.

**Umsetzung:** WICHTIG: Der Port darf hier KEIN Ersatz-Sample spielen. Aufrufstelle: 0x801030f4 (Zustand 3, +0x6=0).

### ENEMSE-Bank 25: SE-Id 14
prog 0, tone 15, Prio 3, Kanal 3 -> VAG 11, vol 110, center 101, shift 0, min=max=74 -> ENEMSE_00288.wav (VAG 11 = 4096 B, wav-data 14224 B - verifiziert).

**Beleg:** EDT @0x1F2838 = 00 00 f3 03; VagAtr prog0/tone15, vag = 11.

**Umsetzung:** Genau 1 Aufruf in EM36: 0x801041a8 (+0x218=1 / +0x219=2).

### ENEMSE-Bank 25: SE-Id 15
prog 1, tone 2, Prio 3, Kanal 3 -> VAG 9, vol 115, center 85, shift 0, min=max=61 -> ENEMSE_00286.wav (dieselbe Datei wie Id 4, aber andere vol/center).

**Beleg:** EDT @0x1F283C = 00 01 23 03; VagAtr prog1/tone2, vag = 9.

**Umsetzung:** 2 Aufrufe in EM37: 0x8010221c (+0x5=9/+0x6=2) und 0x80102c28 (+0x5=11/+0x6=2).

### ENEMSE-Bank 25: VAG-Offsets im VB
VB beginnt bei Datei 0x1F3800 (2045952). Startoffsets/Groessen (Bytes) je VAG: 1@0/48, 2@48/3904, 3@3952/9008, 4@12960/5024, 5@17984/8448, 6@26432/5792, 7@32224/17392, 8@49616/11824, 9@61440/2672, 10@64112/4928, 11@69040/4096, 12@73136/4448, 13@77584/4288, 14@81872/10560, 15@92432/4352. Summe = 96784 = VB-Groesse.

**Beleg:** VAG-Offset-Tabelle @ VH+2080+512*2 (Datei 0x1F2880+0xC20 = 0x1F34A0), 256 u16, Wert*8 = Bytes.

**Umsetzung:** Fuer eine direkte VBS-Extraktion im Port bzw. als Fixture-Pin.

### Zensus 0x36: Uebersicht
38 Stellen `jal 0x8005BD6C` im Chunk CDEMD0_EM36_ai1.BIN. Verwendete Ids und Haeufigkeit: 0 (9x), 6 (3x), 7 (1x), 8 (2x), 9 (10x), 10 (8x), 11 (4x), 12 (1x), 13 (1x), 14 (1x). a0 wird an JEDER Stelle als Konstante gesetzt (addiu a0,zero,N bzw. addu a0,zero,zero fuer Id 0), es gibt keinen dynamischen SE-Index.

**Beleg:** Voller Scan des Chunks build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN, Ladebasis 0x80100000; a0 rueckwaerts bis zum letzten Schreiber aufgeloest, Delay-Slot mitgeprueft.

**Umsetzung:** Der Port-Brain fuer 0x36 braucht genau diese 10 Ids.

### Zensus 0x36: Zustandsbaum
Doppelte Maschine pro Frame. (a) Zustand +0x04 -> Tabelle @0x801055CC. (b) Aktion +0x218 -> Tabelle @0x80105ABC = {0x80103C18, 0x80103EFC, 0x80104440, 0x801044FC, 0x80104848, 0x80104B94}, Unterindex +0x219. In Zustand 1 (0x80100784) dispatcht +0x10E&0x3F auf @0x80105648; Index 0 = 0x801007C4, das pro Frame ZWEI Tabellen mit +0x05 anfaehrt: Vor-Tabelle @0x8010564C = {0x80100868, 0x80100D08, 0x80101108, 0x80101808, 0x80101AA0} (letzte vier sind `jr ra`-Stubs) und Haupt-Tabelle @0x80105660 = {0x80100960, 0x80100D10, 0x80101110, 0x80101810, 0x80101AA8}. Der Unterzustand ist dann +0x06.

**Beleg:** `lhu v0,270(a0); andi 0x3f; lw 0x80105648+z*4` @0x8010078c-a4; `lbu v0,5(s0); lw 0x8010564c+z*4` @0x80100800-14 und `lw 0x80105660+z*4` @0x8010083c; `lbu v0,536(s0); lw 0x80105abc+z*4; jalr` @0x80103bc8-e4 (Aufruf aus der KI-Wurzel @0x801002f0).

**Umsetzung:** Port-Feldbelegung: +0x04 Zustand, +0x05 Verhalten (0..4), +0x06 Unterzustand, +0x218/+0x219 Aktions-Maschine.

### Zensus 0x36: Verhalten +0x05=0
Fn 0x80100960 (Kette: Zustand 1 / +0x05=0). Dispatch: +0x06==0 -> 0x801009A4, ==1 -> 0x80100A8C, sonst Ende 0x80100CE8. EINE SE-Stelle: 0x80100AB8 = SE 9, in +0x06==1.

**Beleg:** Dispatch @0x80100980-9c; jal @0x80100ab8 mit `addiu a0,zero,9` @0x80100ab4. Letzter +0x14C-Store davor: 0x001F0000 @0x801009a8.

**Umsetzung:** SE 9 -> ENEMSE_00285.wav, Kanal 4, Prio-Nibble 3.

### Zensus 0x36: Verhalten +0x05=1
Fn 0x80100D10. Dispatch: +0x06==0 -> 0x80100DD0, ==1 -> 0x80100E18, ==2 -> 0x80100FF0, ==3 -> 0x80101014, sonst 0x801010E8. SE-Stellen: 0x80100DEC = SE 10 (+0x06=0); 0x80100F18 = SE 9 und 0x80100F80 = SE 11 (beide +0x06=1); 0x80101008 = SE 10 (+0x06=2); 0x80101098 = SE 9 (+0x06=3).

**Beleg:** Dispatch @0x80100d80-cc; a0-Setzer @0x80100de8 (10), @0x80100f14 (9), @0x80100f7c (11), @0x80100ff8 (10), @0x80101094 (9). +0x14C-Store 0x00000007 @0x80100dd8 (Fall 0), 0x001F0002 @0x80101000 (Faelle 2/3).

**Umsetzung:** 5 Trigger in dieser Kette.

### Zensus 0x36: Verhalten +0x05=2 (Sprungtabelle)
Fn 0x80101110, `sltiu v1,0xf` + Sprungtabelle @0x80100014 (15 Eintraege, Index = +0x06): 0->0x80101164, 1->0x80101214, 2->0x801012EC, 3->0x8010136C, 4->0x80101390, 5->0x8010141C, 6->0x80101530, 7->0x801015B8, 8->0x8010173C, 9->0x8010160C, 10->0x8010162C, 11->0x80101700, 12->0x8010173C, 13->0x80101764, 14->0x80101798; Default 0x801017E8.

**Beleg:** @0x80101130-5c (`lbu v1,6(s0)`, `sltiu v0,v1,0xf`, `lw v0,20(at)` mit at=0x80100000). Tabellenwoerter direkt aus dem Chunk-Kopf gelesen.

**Umsetzung:** Diese Kette traegt 13 der 38 SE-Stellen - das Herz des Kampfes.

### Zensus 0x36: +0x05=2 / +0x06=1
0x80101294 = SE 9 und 0x801012DC = SE 11.

**Beleg:** a0-Setzer `addiu a0,zero,9` @0x8010127c und `addiu a0,zero,11` @0x801012cc; beide im Bereich [0x80101214, 0x801012EC) = Sprungtabellen-Fall 1.

**Umsetzung:** SE 9 -> ENEMSE_00285.wav; SE 11 -> ENEMSE_00291.wav.

### Zensus 0x36: +0x05=2 / +0x06=3
0x80101388 = SE 9.

**Beleg:** `addiu a0,zero,9` @0x80101374; Bereich [0x8010136C, 0x80101390). +0x14C-Store 0x001F0003 @0x8010138c.

**Umsetzung:** -

### Zensus 0x36: +0x05=2 / +0x06=4
0x801013E0 = SE 0.

**Beleg:** `addu a0,zero,zero` @0x801013d0 (kein jal dazwischen, Kontext @0x801013b0-e4 geprueft); Bereich [0x80101390, 0x8010141C).

**Umsetzung:** SE 0 -> ENEMSE_00280.wav, Kanal 3.

### Zensus 0x36: +0x05=2 / +0x06=5
VIER SE-0-Stellen: 0x80101450, 0x80101474, 0x8010149C, 0x801014C0.

**Beleg:** a0-Setzer `addu a0,zero,zero` @0x80101440 / 0x80101464 / 0x8010148c / 0x801014b0; alle im Bereich [0x8010141C, 0x80101530).

**Umsetzung:** Vier aufeinanderfolgende Trigger derselben Id - im Original schlaegt das Kanal-Gate (Kanal 3, Prio 3) zu; der Port hat das Gate bereits (re15_se_prio_gate).

### Zensus 0x36: +0x05=2 / +0x06=9
0x80101624 = SE 9.

**Beleg:** `addiu a0,zero,9` @0x8010160c; Bereich [0x8010160C, 0x8010162C). +0x14C-Store 0x001F0000 @0x80101618.

**Umsetzung:** -

### Zensus 0x36: +0x05=2 / +0x06=10
VIER SE-0-Stellen: 0x80101660, 0x80101684, 0x801016AC, 0x801016D0.

**Beleg:** a0-Setzer @0x80101650 / 0x80101674 / 0x8010169c / 0x801016c0; Bereich [0x8010162C, 0x80101700).

**Umsetzung:** Spiegelbild zu +0x06=5 (gleiche Choreografie, andere Phase).

### Zensus 0x36: +0x05=2 / +0x06=11
0x80101734 = SE 10.

**Beleg:** `addiu a0,zero,10` @0x80101720; Bereich [0x80101700, 0x8010173C). +0x14C-Store 0x001F0002 @0x80101710.

**Umsetzung:** -

### Zensus 0x36: +0x05=2 / +0x06=13
0x80101788 = SE 9.

**Beleg:** `addiu a0,zero,9` @0x80101764... genauer: a0-Setzer @0x80101764, jal @0x80101788; Bereich [0x80101764, 0x80101798).

**Umsetzung:** -

### Zensus 0x36: +0x05=2 / +0x06=14
0x801017D0 = SE 10 und 0x801017E0 = SE 9.

**Beleg:** a0-Setzer @0x801017c0 (10) und @0x801017d8 (9); Bereich [0x80101798, 0x801017E8 = Default-Ausgang). +0x14C-Store 0x001F0000 @0x80101770.

**Umsetzung:** -

### Zensus 0x36: Verhalten +0x05=3
Fn 0x80101810. Dispatch: +0x06==0 -> 0x8010185C, ==1 -> 0x801018A8, sonst 0x80101A80. SE-Stellen: 0x8010187C = SE 11 (+0x06=0); 0x801018E4 = SE 11 und 0x80101920 = SE 10 (beide +0x06=1).

**Beleg:** Dispatch @0x80101838-58; a0-Setzer @0x80101864 (11), @0x801018d4 (11), @0x8010191c (10). Zusaetzlich setzt der Kopf `+0x226 |= 2` @0x8010183c-44. +0x14C-Store 0x001F0002 @0x8010186c.

**Umsetzung:** -

### Zensus 0x36: Verhalten +0x05=4
Fn 0x80101AA8. Dispatch: +0x06==1 -> 0x80101BC0, ==0 -> 0x80101B14, ==2 -> 0x80101CA0, ==3 -> 0x80101CCC, sonst 0x80101D28. SE-Stellen: 0x80101BB8 = SE 6 (+0x06=0); 0x80101BD0 = SE 7 (+0x06=1); 0x80101CB8 = SE 10 (+0x06=2).

**Beleg:** Dispatch @0x80101acc-0x80101b10; a0-Setzer @0x80101b98 (6), @0x80101bcc (7), @0x80101ca8 (10). +0x14C-Store 0x0000000B @0x80101b30 (Fall 0/1) und 0x00070002 @0x80101cb0 (Fall 2).

**Umsetzung:** Nur hier feuert SE 7 (ENEMSE_00284.wav, 60760 B = das laengste Sample) - genau EIN Aufruf im ganzen Chunk.

### Zensus 0x36: Zustand 2
Wurzel 0x801025BC (Zustandstabelle @0x801055CC Index 2). `lbu v0,6(s3); bne v0,zero -> 0x80102A94`. SE-Stelle 0x80102940 = SE 8 liegt im Zweig +0x06 == 0.

**Beleg:** @0x801025e8-f4; a0-Setzer `addiu a0,zero,8` @0x8010293c.

**Umsetzung:** -

### Zensus 0x36: geteilte Fn 0x80102AD0
Zwei SE-Stellen: 0x80102B30 = SE 12, 0x80102B4C = SE 9. Die Funktion wird von ZWEI Stellen gerufen: aus Zustand 2 (Zweig +0x06 != 0) @0x80102A9C und aus Zustand 3 / +0x06 == 0 @0x8010303C.

**Beleg:** a0-Setzer @0x80102b20 (12) und @0x80102b48 (9); jal-Zensus auf 0x80102AD0 findet genau die zwei Aufrufer. +0x14C-Store 0x001F0008 @0x80102b28.

**Umsetzung:** Im Port als gemeinsame Hilfsfunktion abbilden, nicht doppelt einbauen. SE 12 wird NUR hier gerufen.

### Zensus 0x36: Zustand 3
Wurzel 0x80102BBC -> Rumpf 0x80102BDC, `sltiu v1,0x6` + Sprungtabelle @0x80100054 (Index = +0x06): 0->0x80102C34, 1->0x801031C0, 2->0x8010340C, 3->0x80103634, 4->0x80103668, 5->0x8010380C; Default 0x8010380C. Alle drei SE-Stellen liegen in Fall 0: 0x80102F78 = SE 8, 0x8010307C = SE 10, 0x801030F4 = SE 13 (stumm).

**Beleg:** @0x80102c04-30; a0-Setzer @0x80102f74 (8), @0x80103054 (10), @0x801030d4 (13). +0x14C-Store 0x001F0006 @0x8010306c.

**Umsetzung:** SE 13 ist byte-true lautlos (s. eigener Posten) - nicht ersetzen.

### Zensus 0x36: Aktion +0x218=1
Fn 0x80103EFC, `sltiu v1,0x5` auf +0x219 + Sprungtabelle @0x8010006C: 0->0x80103F44, 1->0x80103F58, 2->0x80103FE0, 3->0x801041E8, 4->0x80104228; Default 0x801042D8. SE-Stellen: 0x80104058 = SE 6 und 0x801041A8 = SE 14, beide in +0x219 == 2.

**Beleg:** @0x80103f14-3c (`lbu v1,537(s2)`); a0-Setzer @0x80104044 (6) und @0x80104174 (14).

**Umsetzung:** SE 14 wird NUR hier gerufen (ENEMSE_00288.wav).

### Zensus 0x36: Aktion +0x218=4
Fn 0x80104848, Vergleichskette auf +0x219: ==1 -> 0x801048DC, ==0 -> 0x801048B0, ==2 -> 0x80104950, ==3 -> 0x80104B0C, sonst 0x80104B70. SE-Stelle: 0x80104A0C = SE 6, in +0x219 == 2.

**Beleg:** @0x80104868-a4; a0-Setzer `addiu a0,zero,6` @0x80104a08.

**Umsetzung:** -

### Zensus 0x37: Uebersicht
18 Stellen `jal 0x8005BD6C` im Chunk CDEMD0_EM37_ai1.BIN. Verwendete Ids: 0 (8x), 1 (1x), 2 (3x), 3 (3x), 15 (2x). a0 ueberall konstant.

**Beleg:** Voller Scan von build/extracted/re2_ems/CDEMD0_EM37_ai1.BIN (Ladebasis 0x80100000).

**Umsetzung:** Der Begleiter-Brain braucht 5 Ids.

### Zensus 0x37: Zustandsbaum
Zustand +0x04 -> Tabelle @0x80105688 = {0:0x801004F0, 1:0x801008F4, 2:0x80104184, 3:0x801041E4, 4:0x8010423C, 5:0, 6:0, 7:0x80104280}. Zustand 1 dispatcht +0x10E -> @0x801056A8 (Index 0/1 = 0x80100934). 0x80100934 faehrt pro Frame zwei +0x05-Tabellen an: Vor-Tabelle @0x801056B0 und Haupt-Tabelle @0x801056F0, je 16 Eintraege (Verhalten 0..15).

**Beleg:** `lbu v0,4(..); lw 0x80105688+z*4` @0x801003a4; `lhu +0x10e; lw 0x801056a8+z*4` @0x80100914; `lbu +0x5; lw 0x801056b0+z*4` @0x80100964 und `lw 0x801056f0+z*4` @0x8010098c. Haupt-Tabelle @0x801056F0 = {0x80100A44, 0x80100ABC, 0x80100D28, 0x801013B4, 0x80101840, 0x80101A0C, 0x80101B80, 0x80101C14, 0x80101CCC, 0x8010204C, 0x8010271C, 0x80102A38, 0x80103328, 0x801034EC, 0x80103928, 0x80104054}.

**Umsetzung:** Der Begleiter hat 16 Verhalten (+0x05), gegen 5 beim Boss.

### Zensus 0x37: +0x05=3
Fn 0x801013B4, Sprungtabelle @0x80100034 auf +0x06 (5 Faelle): 0->0x80101404, 1->0x80101480, 2->0x80101518, 3->0x801015C4, 4->0x801017BC. SE-Stellen: 0x80101478 = SE 0 (+0x06=0); 0x80101790 = SE 3 (+0x06=3).

**Beleg:** Dispatch @0x801013f4; a0-Setzer @0x80101410 (`addu a0,zero,zero`) und @0x80101778 (`addiu a0,zero,3`).

**Umsetzung:** -

### Zensus 0x37: +0x05=8
Fn 0x80101CCC, Sprungtabelle @0x8010004C auf +0x06 (10 Faelle): 0->0x80101D38, 1->0x80101D90, 2->0x80101DAC, 3->0x80101DFC, 4->0x80101E24, 5->0x80101E8C, 6->0x80101E94, 7->0x80102024, 8->0x80101FA8, 9->0x80102024. SE-Stellen: 0x80101DA4 = SE 0 (+0x06=1); 0x80101E14 = SE 2 (+0x06=3); 0x8010201C = SE 0 (+0x06=8).

**Beleg:** Dispatch @0x80101d28; a0-Setzer @0x80101d90, @0x80101dfc (2), @0x8010200c. +0x14C-Store 0x00000401 @0x80101d4c bzw. 0x00070000 @0x80101e08.

**Umsetzung:** -

### Zensus 0x37: +0x05=9
Fn 0x8010204C, Sprungtabelle @0x80100074 auf +0x06 (10 Faelle): 0->0x8010209C, 1->0x80102144, 2->0x801021C0, 3->0x80102224, 4->0x80102344, 5->0x8010237C, 6->0x80102500, 7->0x80102508, 8->0x801026F4, 9->0x80102690. SE-Stellen: 0x8010213C = SE 0 (+0x06=0); 0x8010221C = SE 15 (+0x06=2); 0x8010236C = SE 2 (+0x06=4); 0x801026EC = SE 0 (+0x06=9).

**Beleg:** Dispatch @0x8010208c; a0-Setzer @0x801020e0 (Kontext @0x801020d8-0x80102140 geprueft, kein jal dazwischen), @0x80102208 (15), @0x80102344 (2), @0x801026dc. +0x14C-Store 0x001F0002 @0x8010219c.

**Umsetzung:** -

### Zensus 0x37: +0x05=10
Fn 0x8010271C, Sprungtabelle @0x8010009C auf +0x06 (6 Faelle): 0->0x8010276C, 1->0x80102800, 2->0x80102878, 3->0x801028C0, 4->0x80102980, 5->0x801029A0. Eine SE-Stelle: 0x801027F8 = SE 0 (+0x06=0).

**Beleg:** Dispatch @0x8010275c; a0-Setzer @0x801027d0.

**Umsetzung:** -

### Zensus 0x37: +0x05=11
Fn 0x80102A38, Sprungtabelle @0x801000B4 auf +0x06 (11 Faelle): 0->0x80102A88, 1->0x80102B50, 2->0x80102BCC, 3->0x80102C30, 4->0x80102DF4, 5->0x80102E2C, 6->0x80102F9C, 7->0x80103010, 8->0x801031CC, 9->0x80103210, 10->0x801032A8. SE-Stellen: 0x80102B20 = SE 0 (+0x06=0); 0x80102C28 = SE 15 (+0x06=2); 0x80102E18 = SE 2 (+0x06=4); 0x80103284 = SE 0 (+0x06=9).

**Beleg:** Dispatch @0x80102a78; a0-Setzer @0x80102ad4, @0x80102c14 (15), @0x80102df4 (2), @0x80103248. +0x14C-Store 0x001F0002 @0x80102ba8 bzw. 0x003F0000 @0x801031dc.

**Umsetzung:** Struktur-Zwilling zu +0x05=9 (dieselbe SE-Reihenfolge 0/15/2/0) - im Port als eine parametrisierte Kette moeglich.

### Zensus 0x37: +0x05=13
Fn 0x801034EC, Sprungtabelle @0x801000FC auf +0x06 (6 Faelle): 0->0x8010353C, 1->0x801035E0, 2->0x80103678, 3->0x801036AC, 4->0x80103880, 5->0x80103890. SE-Stellen: 0x801035C8 = SE 0 (+0x06=0); 0x80103774 = SE 3 und 0x8010382C = SE 3 (beide +0x06=3).

**Beleg:** Dispatch @0x8010352c; a0-Setzer @0x8010359c, @0x8010373c (3), @0x801037f4 (3).

**Umsetzung:** -

### Zensus 0x37: +0x05=14
Fn 0x80103928, Sprungtabelle @0x80100114 auf +0x06 (8 Faelle): 0->0x80103970, 1->0x801039F4, 2->0x80103AB4, 3->0x80103B94, 4->0x80103C10, 5->0x80103E50, 6->0x80103F98, 7->0x80104030. Eine SE-Stelle: 0x80103E24 = SE 1 (+0x06=4).

**Beleg:** Dispatch @0x80103960; a0-Setzer `addiu a0,zero,1` @0x80103e20.

**Umsetzung:** Die einzige Stelle im ganzen Endkampf, die SE 1 (ENEMSE_00289.wav, prog 1) spielt.

### Kreuzprobe Bank 25
Die von den beiden KI-Chunks gerufene Id-Menge {0,1,2,3,6,7,8,9,10,11,12,13,14,15} ist eine Teilmenge der in Bank 25 belegten Ids {0..15}, und Bank 25 hat GENAU 16 belegte Records und 16 Leerrecords. Keine gerufene Id trifft auf 0xFFFFFFFF. Das ist die unabhaengige Bestaetigung der Bankwahl.

**Beleg:** Zensus beider Chunks + EDT Bank 25 @0x1F2800..0x1F287F.

**Umsetzung:** Als Unit-Test pinnen: fuer jede der 14 gerufenen Ids muss re2_enemse_decode_entry(EDT[id]) silent==0 liefern.

### Kanalbelegung Bank 25
Nur SPU-Kanaele 2, 3 und 4 kommen vor (byte3&0x1F). Kanal 2: Ids 2,3. Kanal 3: Ids 0,1,4,5,8,11,14,15. Kanal 4: Ids 6,7,9,10,12,13. Prio-Nibble ueberall 3, extra-Layer ueberall 0.

**Beleg:** EDT-Rohbytes Bank 25.

**Umsetzung:** Passt in MIXER_RE2SE_CH_COUNT=8 / MIXER_RE2SE_CH_FIRST des Ports; das bestehende Gate re15_se_prio_gate + s_re2se_prio[chan] greift unveraendert. Kein Bedarf fuer den freien Pool-Zweig.

### BGM: MAIN-Datei
MAIN-Id 0x23 -> CD-Index 389 -> Datei info/re2leon/COMMON/SOUND/MAIN23.BGM (26296 B).

**Beleg:** Tabelle @0x800a8168 (u16, Index = byte&0x3F): Eintrag 0 = 0x0162 = 354, linear bis Eintrag 0x31; Eintrag 0x23 liegt @0x800a81ae = 0x0185 = 389. Leser: `lbu v0,0(0x800dfd64); andi 0x3f; sll 1; lhu 0x800a8168+..` @0x8005abb0-c8, danach `jal 0x80012fb8` (CD-Read). Anker: die ENEMSE.VBS selbst hat Datei-Id 353 (`addiu a0,zero,353` @0x8005a128) - MAIN00.BGM ist also 354.

**Umsetzung:** Fuer den Port: MAIN23.BGM als Endkampf-Haupt-BGM.

### BGM: SUB-Dateien
SUB-Ids 0x2C / 0x2F / 0x30 -> info/re2leon/COMMON/SOUND/SUB_2C.BGM (110248 B), SUB_2F.BGM (130980 B), SUB_30.BGM (227836 B). Alle drei existieren im Repo.

**Beleg:** Verzeichnis info/re2leon/COMMON/SOUND enthaelt SUB_00.BGM..SUB_37.BGM (56 Dateien) - der Id-Raum 0x00..0x37 deckt sich exakt mit dem Dateinamensraum.

**Umsetzung:** Es gibt KEINE separaten .VB/.SEQ-Dateien; alles steckt in den .BGM-Containern.

### BGM-Container-Layout (RE2)
Aufbau: SEQ#1 ab Offset 0 ('pQES'), optional SEQ#2, dann VAB-VH ('pBAV'), dann VB. Die letzten 16 Bytes der Datei sind vier u32: {0, VB-Offset, VH-Offset, SEQ#2-Offset (0 = keiner)}.

**Beleg:** Lader @0x8005abdc-0x8005ac10: `a3 = buf + size - 4; lw v1,0(a3)` (SEQ#2), `a3 -= 4; lw v0,0(a3)` (VH), `s1 = v0 + 0x801f2e00`. Messwerte: MAIN23.BGM Trailer = 00000000 / 00001068 / 00000244 / 00000000, 'pBAV' liegt bei 580 = 0x244. SUB_2C.BGM: 00003744 / 00000d20 / 00000d08, 'pBAV' @3360 = 0xd20, zweites 'pQES' @3336 = 0xd08. SUB_2F.BGM: 00005010 / 000031ec / 0000006c (VAB @12780, SEQ#2 @108). SUB_30.BGM: 000052b8 / 00002e94 / 00002e7c (VAB @11924, SEQ#2 @11900).

**Umsetzung:** Der Port muss NICHT rechnen - VH und VB stehen explizit im Trailer. Das erklaert auch das bekannte '4 Byte Pad vor dem VB': VH-Bereich = 2080 + 512*nProg + 512 + 4 (an allen vier Dateien geprueft).

### BGM: VAB-Kennwerte
MAIN23.BGM: VAB @580, nProg 2, nTone 4, nVAG 2, fsize 25696. SUB_2C.BGM: VAB @3360, nProg 16, nTone 16, nVAG 9, fsize 106864. SUB_2F.BGM: VAB @12780, nProg 10, nTone 23, nVAG 11, fsize 118176. SUB_30.BGM: VAB @11924, nProg 13, nTone 27, nVAG 17, fsize 215888.

**Beleg:** VabHdr-Felder +0x0C fsize, +0x12 ps, +0x14 ts, +0x16 vs, direkt aus den Dateien gelesen.

**Umsetzung:** Groessenprobe fuer den Port-Parser: VH-Bereich = VB_off - VH_off; MAIN23 3620 = 2080+1024+512+4; SUB_2C 10788 = 2080+8192+512+4; SUB_2F 7716; SUB_30 9252 - alle passen.

### PORT: Ist-Stand ENEMSE
Die komplette ENEMSE-Maschine existiert bereits und ist byte-belegt: Bank-TOC (gen/re2_ems_toc.inc, 73 Baenke), Paar-Tabelle (s_re2_enemse_pairs[148]), Bank-Wahl (re2_enemse_select_bank), Record-Dekodierung (re2_enemse_decode_entry), Lader (load_re2_enemy_se_pc), Kanal-/Prio-Gate (re15_se_prio_gate + s_re2se_prio) und Frame-Pumpe (re2se_voice_pump). ENEMSE.VBS liegt in re15_port/shared_assets/RE2/.

**Beleg:** re15_port/platform/pc/src/audio_pc.c Zeilen ~994-1230; re15_port/engine/src/re2_ems.c Zeilen 379-500; re15_port/engine/src/gen/re2_ems_toc.inc; ls re15_port/shared_assets/RE2 -> CDEMD0.EMS, ENEMSE.VBS.

**Umsetzung:** Fuer den Endkampf ist KEIN neuer Audio-Code noetig - nur Bankwahl + Aufrufe.

### PORT: was fehlt (1) Bankwahl
re15_audio_re2_enemy_bank() wird heute nur in pc_enemy_load_ex fuer die Typen 0x20 (Hund), 0x21 (Kraehe), 0x25/0x26 (Spinne) und die Zombie-Familie gesetzt. Fuer 0x36/0x37 gibt es keinen Zweig -> s_re2se_bank_sel bliebe auf dem Wert des letzten Raums oder -1 ('keine ENEMSE-Bank gewaehlt -> stumm').

**Beleg:** re15_port/platform/pc/main.c Zeilen ~690-770 (die if/else-Kette in pc_enemy_load_ex).

**Umsetzung:** In pc_enemy_load_ex einen Zweig `else if (type == 0x36 || type == 0x37) <em36_audio_hook>(re15_audio_re2_enemy_se, re15_audio_re2_enemy_bank)` ergaenzen und im Hook Bank 25 latchen (Beleg: Paarzeile 25 = {0x27,0x00}, Sce_em_set +7 = 0x27 in room7040/scd/sub00.scd @0x73).

### PORT: was fehlt (2) Sound-Id statt AI-kind
re2_enemse_room_add_kind() erwartet laut Kommentar den 'kind'. Das Original vergleicht aber entity+0x1FA, und das ist der Sce_em_set-Record-Byte +7 (SOUND-ID), NICHT der AI-Typ aus Byte +3.

**Beleg:** `lbu v0,7(v0)` @0x80057274 (v0 = *0x800d5be8 = laufender Sce_em_set-Record) -> `sb v0,506(s0)` = entity+0x1FA @0x80057280 -> Vergleich `lbu v1,0(0x800d8cd0)` @0x8005728c-0x800572b8. Fuer ROOM7040 ist dieses Byte 0x27, waehrend die AI-Typen 0x36/0x37 in der Paar-Tabelle @0x800a7400 (Maximum 0x34) GAR NICHT vorkommen - eine kind-basierte Wahl liefert dort 0xFF (= keine Bank, alles stumm).

**Umsetzung:** Der Port muss entity+0x1FA (Sound-Id) fuehren und an re2_enemse_room_add_kind geben. Der bestehende Kommentar in re2_ems.c ('RE2-Raumdaten liegen nicht im Repo') ist widerlegt: info/re2leon/PL0/RDT/room7040/scd/sub00.scd.

### PORT: was fehlt (3) SE-Aufrufe im Brain
Alle 56 SE-Stellen (38 in 0x36, 18 in 0x37) muessen in den neuen Brains als re15_audio_re2_enemy_se(id, 0) an der jeweiligen Zustand/Unterzustand-Stelle sitzen. Der bestehende Umweg pc_re2z_se_re15 (RE1.5-Sound-Mapper) gilt NUR fuer die Zombie-Familie und darf hier nicht verwendet werden - fuer 0x36/0x37 gibt es keine RE1.5-Zwillinge.

**Beleg:** re15_port/platform/pc/main.c Zeile ~640-667 (pc_re2z_se_re15 mappt nur die Zombie-Ids 0..13 auf re15_audio_room_se).

**Umsetzung:** Direkt re15_audio_re2_enemy_se verdrahten, Bank 25.

### PORT: Erwartete Ladeausgabe
Nach korrekter Verdrahtung muss audio_pc.c melden: '[re2se] ENEMSE Bank 25 geladen: 15 VAGs, Map 32 Eintraege'.

**Beleg:** vs = 15 aus dem VabHdr @0x1F2896; Map-Eintraege = vh_off/4 = 0x80/4 = 32 (Trailer-u32 @Blob+3744 = 0x00000080).

**Umsetzung:** Als Abnahmekriterium/Testpin verwenden.

### PORT: Tonhoehe
Die Tones der Bank 25 sind ein chromatischer Keymap: min == max und laeuft prog0 tone0..15 durch die Noten 59..74, prog1 tone0..2 durch 59..61. center weicht davon ab (76..102), es wird also transponiert. Beispiel Id 8: min/max 68, center 92 -> -24 Halbtoene.

**Beleg:** VagAtr-Felder +0x04 center, +0x05 shift, +0x06 min, +0x07 max, ausgelesen aus ENEMSE.VBS Bank 25 (Datei 0x1F30A0 ff.).

**Umsetzung:** Die vorhandene Formel re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift) in audio_pc.c bildet genau das ab - nicht anfassen. Die extrahierten .wav sind 22050 Hz mono 16-bit und muessen entsprechend resampled werden, nicht 1:1 abgespielt.

## Offen
- SUB-BGM: die CD-Index-Tabelle fuer die SUB-Ids ist NICHT lokalisiert. Belegt ist nur die Tabelle @0x800a8168 (MAIN, Index & 0x3F, Eintrag 0 = Datei-Id 354). Die Dateinamen SUB_2C/2F/30.BGM sind ueber die Namenskonvention und den exakt passenden Id-Raum 0x00..0x37 (56 Dateien) sicher, die Datei-Ids aber nicht byte-belegt.
- BGM-Trigger fuer ROOM7040: room7040/scd/*.scd enthaelt KEIN Byte 0x57 als Opcode. Der Aufruf MAIN 0x23 / SUB 0x2C,0x2F,0x30 kommt also von aussen (ROOMG040.RDT-SCD oder Stage-Entry) - nicht disassembliert. Die Zuordnung stammt aus der Auftrags-Vorgabe, nicht aus meiner Messung.
- SE-Ids 4 und 5 der Bank 25 sind live (VAG 9 bzw. 15, ENEMSE_00286/00292.wav), werden aber von KEINEM der beiden KI-Chunks gerufen. Ausloeser unbekannt (SCD Se_on? EXE-Seite? die anderen Slots?).
- Positionale Lautstaerke/Panning: FUN_8005bd6c legt entity+0x38/+0x3C/+0x40 in den Voice-Record (@0x8005bef0-0x8005bf08). Wer daraus Vol/Pan rechnet (der SPU-Service ueber DAT_800d4f18[]) ist nicht disassembliert - der Port nutzt weiter die Tone-eigene vol/pan.
- Globales VAB-Handle DAT_800d4c4b (Voice-Record +0x04): Herkunft belegt (Rueckgabe von jal 0x80085368 @0x8005a210), Laufzeitwert nicht gemessen. Fuer Bank 25 irrelevant, weil kein Record ein Override-Bit traegt.
- Aktions-Maschine +0x218 des Bosses: nur die zwei SE-tragenden Zweige (1 und 4) sind aufgeloest. Die Zweige 0 (0x80103C18), 2 (0x80104440), 3 (0x801044FC) und 5 (0x80104B94) sind nicht analysiert.
- Die Bedeutung der Zustaende 2 (0x801025BC), 3 (0x80102BBC), 4 (0x80103834) und 7 (0x80103878) des 0x36 ist NICHT benannt - ich habe sie nur als Adressen gefuehrt, keine Etiketten geraten.
- Der 32-bit-Store auf entity+0x14C (z.B. 0x001F0002, 0x0000000B) ist als Kontext roh mitgeliefert; seine Semantik (Anim-Bank/Clip?) ist nicht byte-belegt aufgeloest.
- entity-word0 & 0x2000 = 0 fuer ROOM7040 ist aus Paarzeile 25 = {0x27,0x00} HERGELEITET (Schleife @LAB_80052c2c setzt das Bit nur fuer die zweite Haelfte), nicht am laufenden Spiel gemessen.
- Die 0x37-Zustaende 2/3/4/7 (0x80104184 / 0x801041E4 / 0x8010423C / 0x80104280) und die Verhalten +0x05 = 0,1,2,4,5,6,7,12,15 tragen keine SE-Stelle und sind nicht weiter analysiert.


---

# RE2→RE1.5 Sound-Import-Liste (Menue + Raetsel/Raum), byte-belegt aus PSX.EXE, COMMON/BIN-Overlays, CORE*.EDH und 489 RDT-Sound-Tabellen

FUNDAMENT (selbst gemessen, gilt fuer die ganze Liste):
(1) Bank-Semantik ist in BEIDEN Spielen identisch. RE1.5 Se_on=FUN_80045024, Bank-Sprungtabelle @0x80010e70 (6 Eintraege, `sltiu v1,0x6` @0x80045094): 0=Resident-Blob @0x801fdd00, 1=ARMS/Waffe @0x801fcd00, 2=RDT snd0 (RDT+0x8 @0x8004513c), 3=RDT snd1 (RDT+0x14 @0x80045110, `sltiu 0x19`), 4=CORE @0x801fbd00, 5=RDT snd0. RE2 Se_on=FUN_8005ba28, flache Tabelle 0x800dbb78[bank] : [0]=0x801fb700 (Tag "DOOR SOUND" @0x8001084c, Lader FUN_80014cd0 @0x80014eb4), [1]=0x801faa00 (ARMS, @0x80059780), [2]/[5]=RDT snd0 (`lw v1,8(0x800ce324)` @0x8005a37c → @0x8005a38c/@0x8005a384), [3]=ENEMSE @0x801f8e10 (@0x8005a13c), [4]=CORE @0x801f9d10 (@0x80059a90, Tag "CORE EDH" @0x80011548). ⇒ Bank 4 = CORE in BEIDEN.
(2) ⛔ DIE CORE-BANK MUSS NICHT IMPORTIERT WERDEN. info/Re1.5/PSX/SOUND/CORE00.EDH und info/re2leon/COMMON/SOUND/CORE00.EDH sind md5-IDENTISCH (9b0e0627500b50eaca5f8bc4124635d9), ebenso die .VB (cdcb61fb, 40464 B). RE1.5 CORE00-03/0A-0D sind alle dieselbe Datei; RE1.5 CORE04-09 == RE2 CORE01 (md5 82f06386 / VB ab90ba10, 40432 B). Record-Tabelle CORE00.EDH Byte 0x00-0x3F, 4 B je Record, erster LEER-Record = 0x0B (@0x2C = ff ff ff ff), VAB-Magic "pBAV" @0x40: Recs 0x00-0x0A gueltig. ⇒ JEDER RE2-Menue-SE mit Bank 4 / Record 0x00-0x0A ist reiner CODE-Import, null Asset.
(3) Record-Bedeutungen (aus RE1.5s EIGENEN Callsites, nicht geraten): 0x04=Cursor (@0x8004a478/4a0/4c8/4f0, DEBUG.BIN @0x800c62d8/632c/63ac/6420), 0x05=Abbruch (@0x8004a660, @0x800c6550), 0x06=Bestaetigen (@0x8004a51c, @0x800c6488), 0x07=fehlgeschlagen (in RE1.5 NIRGENDS gerufen, existiert als CORE00.EDH @0x1C = 00 00 83 00), 0x08=Seitenwechsel (@0x800c71d8/@0x800c7264), 0x09=Menue-OEFFNEN (@0x8004a158, pad&0x100), 0x0A=Equip/Unequip (@0x800c64ec), 0x00-0x03=Spielerstimme (Stage-Overlays @0x8010a3e4/@0x8010a7b8/@0x8010a864).
(4) ⛔ RAUM-BANK HAT KEINEN FREIEN SLOT. RE1.5-RDT snd0-EDT = 0x80 B = 32 Records; Zensus ueber 206 RE1.5-Raeume: Recs 0x00-0x1F sind BELEGT (0x1D-0x1F in 206/206 Raeumen, 0x1A-0x1C in 202/206). RE2-RDT snd0-EDT = 0xC0 B = 48 Records (239 Raeume). RE1.5s Se_on-Bank-2-Schranke ist `sltiu s4,0x21` @0x800450e4 → Rec ≤ 0x20. ⇒ JEDER RE2-Raum-SE mit Record ≥ 0x20 (Pickup 0x25/0x26/0x27, Save-Raum 0x22/0x23/0x24) ist in RE1.5s Bankmodell NICHT ausdrueckbar. Import MUSS ueber eine zusaetzliche residente Port-Bank laufen (Muster existiert bereits: audio_pc.c:1010-1015 `re15_pc_read_re2()` / shared_assets/RE2/).
(5) RE2-Overlay-Ladeadressen aus der EXE-Tabelle @0x800107e8-0x80010810: TITLE.BIN/DIEDEMO.BIN=0x80190000, ENDING.BIN=0x8011A000, RESULT.BIN=0x80158000, CONFIG.BIN/MEM_CARD.BIN/SELECT.BIN/OPENING.BIN=0x801BFA18 (@0x800107f4). ⛔ KORREKTUR zu analysis/typewriter_sounds.md §5.2: dort steht 0x801c0000 — falsch. Gegenprobe: CONFIG.BIN Datei-Offset 0x110 = `addiu sp,sp,-24` (Prolog) = 0x801BFB28 = der kleinste Selbstzeiger der Datei; mit 0x801c0000 landet MEM_CARDs Header-Zeiger 0x801c0264 auf Datenmuell. RE1.5 DEBUG.BIN=0x800C0000, RE1.5 TITLE.BIN=0x80100000 (beide bestaetigt).
(6) Se_on-Gesamtzahlen (eigener Voll-Scan, jal-Encoding 0x0C016E8A bzw. 0x0C011409): RE2 PSX.EXE 138 · CONFIG.BIN 34 · MEM_CARD.BIN 24 · TITLE.BIN 47 · SELECT.BIN 4 · ENDING.BIN 2 · DIEDEMO.BIN 1 · RESULT.BIN 0 · OPENING.BIN 0. RE1.5 PSX.EXE 41 · DEBUG.BIN 23 · TITLE.BIN 4 · STAGE1 11 · STAGE2 4 · STAGE3 16 · STAGE4 12 · STAGE5 19 · STAGE6 0.

## Posten

### MENUE — OPTIONS/Controller-CONFIG: kompletter Schirm stumm
RE2s CONFIG.BIN beept an 34 Stellen (Record 0x04 Cursor x22, 0x05 Abbruch x7, 0x06 Bestaetigen x5). RE1.5s OPTIONS-Task ist zu 100% stumm und der Port ebenfalls. Das ist die groesste einzelne Luecke.

**Beleg:** RE2: CONFIG.BIN (Basis 0x801BFA18 @0x800107f4). Rec 0x04 @0x801c0948(+0xF30) 0x801c0a1c 0x801c0d48 0x801c0ec4 0x801c1038 0x801c11b4 0x801c16ac 0x801c1788 0x801c1864 0x801c1928 0x801c1b68 0x801c1c88 0x801c1d74 0x801c1fac 0x801c2064 0x801c2180 0x801c23bc 0x801c2474 0x801c2a88 0x801c2b30 0x801c2bf4 0x801c2ca0 ; Rec 0x05 @0x801c0444 0x801c0554 0x801c0c24 0x801c0cd8 0x801c1540 0x801c1a14 0x801c2934 ; Rec 0x06 @0x801c0b0c 0x801c0b78 0x801c15b8 0x801c28c0 0x801c2950. Muster z.B. @0x801c0944 `lui a0,0x404` / @0x801c0948 `jal 0x8005ba28` / @0x801c094c `addu a1,zero,zero`, unmittelbar davor der Cursor-Aenderungstest @0x801c0928 `bne v1,v0` und danach der Bestaetigungstest @0x801c095c `andi v0,v0,0x4000`. RE1.5: NEIN — Task @0x8002dde4 liegt in der Se_on-freien Luecke 0x8002c97c..0x8003338c (Voll-Scan aller 41 jal 0x80045024).

**Umsetzung:** re15_port/platform/pc/main.c, Funktion pc_run_config (deklariert Z.1692, gerufen Z.2485): re15_audio_core_se(4) bei jeder Cursorbewegung (Tab- und Zeilenwechsel), (6) bei Bestaetigen, (5) bei Abbruch/Zurueck. Bank 4 = CORE00 ist im Port bereits resident (re15_audio_core_se, include/re15_audio.h:169). Kein Asset noetig — CORE00 md5-identisch.

### MENUE — Inventar-Reiterleiste (ITEM/FILE/MAP/EXIT): stumm
RE2 beept beim Reiterwechsel, beim Oeffnen eines Reiters und beim Verlassen. RE1.5s Reiter-FSM enthaelt NULL Se_on; der Port uebernimmt diese Stille.

**Beleg:** RE2 FUN_8006a7f0: Cursor unveraendert -> Sprung @0x8006a88c `beq a0,v0,0x8006a89c`, Delay-Slot @0x8006a890 `lui a0,0x404`, @0x8006a894 `jal 0x8005ba28` = Rec 0x04. Bestaetigen: Gate @0x8006a8a4 `andi v0,v0,0x1000`; Reiter 0..2 -> @0x8006a8c8 `lui a0,0x406` (Rec 0x06), Reiter 3 (EXIT) -> @0x8006a8d4 `lui a0,0x405` (Rec 0x05), gemeinsamer Aufruf @0x8006a918. Abbruch: Gate @0x8006a928 `andi 0x2000` -> @0x8006a930 `lui a0,0x405`, @0x8006a934. Zweiter Abbruchpfad: Gate @0x8006a94c `andi 0x4000` + `lbu 0x800d5c06 == 1` @0x8006a95c-64 -> @0x8006a968 `lui a0,0x405`, @0x8006a96c. RE1.5: NEIN — kein Se_on im Bereich 0x8004974c-0x80049a58 und in FUN_80046540 (mein Voll-Scan: der naechste RE1.5-EXE-Callsite nach 0x80041730 ist erst 0x8004a158).

**Umsetzung:** re15_port/engine/src/menu_common.c, Reiter-Dispatch bei Z.1659 (s_substate 0=Reiter): se4(4) bei Reiterwechsel, se4(6) beim Oeffnen von ITEM/FILE/MAP, se4(5) bei EXIT und bei Cancel. se4() existiert Z.110.

### MENUE — Item-Get-Modal (Aufnahme-Modal mit YES/NO): stumm
RE2 spielt beim Aufnehmen einen Bank-4-Bestaetigungston (Rec 0x06). RE1.5s Modal FUN_8001db28 ist per adversarialem jal-Zensus komplett sound-frei, der Port ebenso.

**Beleg:** RE2: @0x80026528 `beq v1,v0` (Modus-Gate, `lbu v1,0x800d5c00` @0x80026518, v0=2), Delay-Slot @0x8002652c `lui a0,0x406`, @0x80026530 `jal 0x8005ba28`, davor `sb 1 -> 0x800df348` @0x800264f8 und `sw v1 -> 0x800df38c` @0x80026508. RE1.5: NEIN — FUN_8001db28 liegt in der 80080-Byte-Luecke 0x80018e3c..0x8002c70c; Call-Zensus (analysis/typewriter_sounds.md §3) = FUN_8001e1c8/FUN_80029a98/FUN_8004dc4c/FUN_8004df2c/FUN_8004ee78/FUN_8004ef90, kein Sound. Port: NEIN — re15_port/engine/src/item_modal_common.c enthaelt 0 Zeilen mit re15_audio.

**Umsetzung:** re15_port/engine/src/item_modal_common.c: im Uebergang nach state7 (Grant) re15_audio_core_se(6); bei NO/Abbruch re15_audio_core_se(5); bei YES/NO-Toggle re15_audio_core_se(4). Kein Asset noetig.

### MENUE — YES/NO-Box der Message-VM: stumm
RE1.5s YES/NO-Dialog (FSM-case 4 in FUN_80028134) hat weder Toggle- noch Confirm-Ton. Ein direkter RE2-Zwilling ist NICHT lokalisiert; die Konvention 4/5/6 ist aber game-weit belegt.

**Beleg:** RE1.5: NEIN — jal-Zensus FUN_80028134 (420 Instr., vollstaendig): @0x800283f4 jal 0x80028840, @0x800284f0 jal 0x800c69bc, @0x800284f8/@0x80028654/@0x8002867c jal 0x800279c8, @0x800285a8 jal 0x8002877c, @0x80028750 jal 0x80028868 — kein Sound-Ziel (analysis/typewriter_sounds.md §3). Toggle liest DAT_800ac76c&0x3000, Confirm &0x4000. Port: NEIN — msg_common.c 0 Treffer re15_audio.

**Umsetzung:** re15_port/engine/src/msg_common.c: beim YES/NO-Toggle (0x3000-Kante) re15_audio_core_se(4), beim Confirm (0x4000-Kante) re15_audio_core_se(6). ⛔ ALS NACHRUESTUNG KENNZEICHNEN (wie der Combine-Sound in menu_common.c:1090ff) — RE2-Adressbeleg fehlt, siehe offen.

### MENUE — Game-Over-Schirm: stumm
RE2s DIEDEMO.BIN spielt genau EINEN SE (Bank 4, Record 0x00) beim Erreichen des Zaehlerendes. RE1.5s Game-Over FUN_8001500c ist stumm.

**Beleg:** RE2 DIEDEMO.BIN (Basis 0x80190000 @0x800107e8), Datei-Offset 0x2A64 = RAM 0x80192A64: @0x80192A40 `jal 0x80085dd0`, @0x80192A44 `subu s0,s1,v0` (s1=100 @0x80192A3C), @0x80192A4C `beq v0,zero,+`, @0x80192A5C `bne v0,s1,+`, Delay-Slot @0x80192A60 `lui a0,0x400`, @0x80192A64 `jal 0x8005ba28`. RE1.5: NEIN — FUN_8001500c in der Luecke 0x80018e3c..0x8002c70c. Port: NEIN — re15_gameflow.c 0 Treffer re15_audio.

**Umsetzung:** re15_port/engine/src/re15_gameflow.c beim Eintritt in den Game-Over-Screen re15_audio_core_se(0). ⛔ VORBEHALT: welche CORE-Bank waehrend DIEDEMO resident ist, ist NICHT ermittelt (Setter des Index in 0x800a80b8[idx] nicht verfolgt) — Record 0x00 ist in CORE00 die Charakterstimme, das kann eine andere Bank sein. Erst klaeren, siehe offen.

### MENUE — Save-/Memory-Card-Screen: Rest-Luecke Oeffnungston
Die Bank-4-Triade 4/5/6 ist im Port bereits umgesetzt. Offen bleibt der Menue-OEFFNEN-Ton (Rec 0x09), den RE1.5 selbst an anderer Stelle spielt.

**Beleg:** RE1.5 Menue-Open: @0x8004a154 `lui a0,0x409` + @0x8004a158 `jal 0x80045024` (Gate pad&0x100). RE1.5s eigener Card-Screen FUN_80025c00 ist stumm (jal-Zensus 680 Instr., analysis/typewriter_sounds.md §4). RE2 MEM_CARD.BIN (Basis 0x801BFA18): Rec 0x04 x5 @Datei 0x5E0/0x604/0x778/0x7BC/0x166C, Rec 0x05 x8 @0x9C4/0xCD0/0x1568/0x1598/0x1768/0x18DC/0x1964/0x19AC, Rec 0x06 x9 @0x8DC/0x938/0xC2C/0x1580/0x1728/0x1744/0x1850/0x1870/0x18B8. Port: JA fuer 4/5/6 — re15_port/platform/pc/main.c:1167-1169.

**Umsetzung:** re15_port/platform/pc/main.c, pc_run_memcard_screen (ab Z.568): beim Eintritt in den Screen re15_audio_core_se(9). Kein Asset noetig.

### MENUE — Save-Screen: die zwei RE2-Bank-2-Specials (Schreibmaschine)
RE2s Card-Screen spielt zusaetzlich zwei RAUM-Bank-SEs: Bank 2 Record 0x22 und 0x24. Das ist das Schreibmaschinen-Paar des RE2-Save-Raums. RE1.5 kann diese Records strukturell NICHT spielen.

**Beleg:** RE2 MEM_CARD.BIN Datei-Offset 0x2780 `a0=0x02220000` und 0x2794 `a0=0x02240000`. Quelle: RDT snd0. Zensus ueber 239 RE2-Raeume: Rec 0x22 in 30 Raeumen (haeufigster Wert 00007316), 0x23 in 30 (00008317), 0x24 in 30 (00009316) — exakt die Save-Raeume. Dieselben drei Eintraege liegen als Alias in COMMON/SOUND/CORE15.EDH: Rec 0x22=00006316, 0x23=00007317, 0x24=00008316 (und dupliziert als 0x0A/0x0B/0x0C). RE1.5: NEIN — Bank-2-Schranke `sltiu s4,0x21` @0x800450e4 und RDT snd0-EDT nur 0x80 B = 32 Records (Zensus 206 Raeume: 0x00-0x1F durchgehend belegt, kein freier Slot).

**Umsetzung:** ASSET-IMPORT noetig. info/re2leon/COMMON/SOUND/CORE15.EDH (3304 B) + CORE15.VB (63904 B) nach re15_port/shared_assets/RE2/ kopieren (Lade-Muster existiert: platform/pc/src/audio_pc.c:1010-1015 re15_pc_read_re2). Neue residente Bank `RE15_SE_BANK_RE2SYS` in include/re15_audio.h neben RE15_SE_BANK_CORE; in pc_run_memcard_screen Records 0x22 (Save-Start) und 0x24 (Save-Ende) spielen.

### MENUE — Titel/LOAD-Screen: 4 von 47 Toenen vorhanden
RE1.5s TITLE.BIN ruft Se_on genau 4x, RE2s TITLE.BIN 47x (Rec 0x04 x22, 0x06 x10, 0x05 x7, 0x00 x3 sowie 5x Bank 0 Rec 0x00 mit low=4). Alle Unterschirme (LOAD-Liste, Slot-Wahl, Zurueck) sind in RE1.5 stumm.

**Beleg:** RE1.5 TITLE.BIN (Basis 0x80100000): @0x80102ac8 Rec 0x06, @0x80102b20 Rec 0x04, @0x80102b70 Rec 0x04, @0x80102c24 Rec 0x00 — das war's. RE2 TITLE.BIN (Basis 0x80190000) Datei-Offsets Rec 0x04: 0x4FC 0x548 0x8F8 0x944 0x984 0x9C4 0x256C 0x25A4 0x2828 0x2860 0x28A4 0x28DC 0x2C00 0x2C38 0x2F1C 0x2F54 0x2FA8 0x2FEC 0x39C4 0x39F8 0x3C2C 0x3C5C ; Rec 0x05: 0xA78 0x21C0 0x2908 0x2C64 0x3018 0x38A4 0x3CD8 ; Rec 0x06: 0x650 0x1F50 0x2360 0x2684 0x2698 0x3084 0x363C 0x367C 0x3CA0 0x3CB0 ; Rec 0x00: 0x16D0 0x22B8 0x383C ; Bank 0 (a0=0x00000004, `li a0,4`): 0x628 0x63C 0xA5C 0x29E8 0x2D1C. Port: TEILWEISE — main.c:2385 (Rec 0x04 Cursor) und main.c:2400 (Rec 0x00 Ansager); LOAD-Liste laeuft ueber pc_run_memcard_screen (main.c:1167-1169, 4/5/6 vorhanden).

**Umsetzung:** re15_port/platform/pc/main.c Titel-FSM: se(6) beim Bestaetigen eines Menuepunkts ZUSAETZLICH zum Ansager Rec 0x00 (RE2 hat beides), se(5) beim Zurueck aus jedem Unterschirm. Kein Asset noetig (Recs 0x00/0x04/0x05/0x06 alle in CORE00). ⛔ Die 5 Bank-0-Aufrufe (a0=0x00000004) NICHT nachziehen — Bank 0 zeigt in RE1.5 auf 0x801fdd00 und wird von RE1.5 NIRGENDS benutzt (Voll-Scan EXE+DEBUG.BIN+alle STAGE-BINs+TITLE.BIN: nur Baenke 1,2,3,4).

### MENUE — Player-Select: Tipp-/Scroll-Ton (Record 0x10)
RE2s SELECT.BIN spielt einen Bank-4-Record 0x10, der in KEINER RE1.5-CORE-Bank existiert. Das ist der einzige Menue-Ton, der wirklich ein Fremd-Sample braucht.

**Beleg:** RE2 SELECT.BIN (Basis 0x801BFA18) Datei-Offset 0x2ADC = RAM 0x801C24F4: Zaehler @0x2AB0-0x2AB4, @0x2AC0 `sltiu v0,v0,0x14`, @0x2AC4 `bne v0,zero,0x2ADC` mit Delay-Slot @0x2AC8 `lui a0,0x410`, @0x2ADC `jal 0x8005ba28`. Weitere SELECT-Toene: 0x1038 Rec 0x06, 0x10CC/0x113C Rec 0x04 (alle in CORE00 vorhanden). Record 0x10 existiert NUR in COMMON/SOUND/CORE15.EDH (48-Record-Tabelle, Tabellenlaenge 0xC0, Rec 0x10 = 00009317); alle RE1.5-CORE-Baenke haben eine 0x40-Tabelle mit gueltigen Records nur bis 0x0A.

**Umsetzung:** ASSET-IMPORT: CORE15.EDH/.VB (siehe Posten Save-Specials — dieselbe Bank). Danach im Player-Select des Ports (platform/pc/main.c, RE15_PSELECT-Pfad) den Tick-Sound Record 0x10 aus der RE2-Systembank spielen.

### MENUE — Inventar-Kernbedienung: KEINE Luecke
Grid-Navigation, Kommando-Cluster, CHECK, FILE-Seitenwechsel, Equip und Combine sind in RE1.5 vollstaendig vertont und im Port umgesetzt. Hier ist NICHTS zu importieren.

**Beleg:** RE1.5 EXE: @0x8004a158 Rec 0x09, @0x8004a47c/4a4/4cc/4f4 Rec 0x04, @0x8004a520 Rec 0x06, @0x8004a664 Rec 0x05. RE1.5 DEBUG.BIN (Basis 0x800C0000, 23 Aufrufe): @0x800c62d8/632c/63ac/6420 Rec 0x04, @0x800c6488 Rec 0x06, @0x800c64ec Rec 0x0A, @0x800c6550/657c/679c/6e04 Rec 0x05, @0x800c6e54/6e90 Rec 0x04, @0x800c6ea4/7050 Rec 0x06, @0x800c7080 Rec 0x05, @0x800c709c/70d4 Rec 0x04, @0x800c7174 Rec 0x05, @0x800c71d8 Rec 0x08, @0x800c71f0/7230 Rec 0x04, @0x800c7264 Rec 0x08. Port: JA — menu_common.c se4() Z.261-366, 767, 1509-1606; re15_itembox.c:332.

**Umsetzung:** Nichts. Nur als Abgrenzung in der Liste fuehren, damit niemand hier doppelt nachruestet.

### RAUM — Tuer-Oeffnungsgeraeusch aus einer eigenen TUER-Bank
RE2 hat eine dedizierte, pro Tuer nachgeladene SE-Bank (Bank 0, Debug-Tag "DOOR SOUND") und spielt daraus beim Raumeintritt POSITIONSBEZOGEN Record 0x01. RE1.5 hat diese Bank zwar im Bankmodell, ruft sie aber NIE auf — RE1.5-Tueren klingen nur, wenn das Raum-SCD selbst ein Se_on hat.

**Beleg:** RE2 FUN_8001417c: Fade-Aufbau @0x800141ac `jal 0x8002c1a0` und @0x800141c4 `jal 0x8002c2b0`, dann @0x800141d0 `lw v0,0x800c3a80`, @0x800141d8 `lhu v0,584(v0)`, @0x800141e0 `beq v0,zero,+`, @0x800141e8-ec `a1 = 0x800cfc30` (Positionsvektor), @0x800141f4 `lui a0,0x1` (= Bank 0, Rec 0x01), @0x800141f0 `jal 0x8005ba28`. Bank-0-Lader FUN_80014cd0, Tag-String "DOOR SOUND" @0x8001084c (referenziert @0x80014dc8), Tabelleneintrag 0x800dbb78[0] = 0x801fb700 @0x80014eb4. RE1.5: NEIN — Bank-0-Fall @0x800450bc laedt 0x801fdd00, aber mein Voll-Scan aller 41 EXE- + 23 DEBUG.BIN- + 66 Overlay-Callsites findet NULL Se_on mit Bank 0. Port: NEIN — aot_common.c enthaelt keinen einzigen re15_audio-Aufruf (nur den Include in Z.22).

**Umsetzung:** ⛔ NICHT blind nachbauen. Zuerst entscheiden, ob der Port RE2s Tuer-Bank uebernehmen soll; die Sample-Quelle ist eine PRO-TUER geladene VAB (Datei-Id aus der Tuer-Struktur), kein Repo-Standalone. Kurzweg fuer den Port: den vorhandenen Raum-SCD-Se_on-Pfad beibehalten und diesen Posten als Architektur-Entscheidung offenlassen.

### RAUM — Schloss-Piepser auf/zu (Bank 0, Records 0x01 und 0x02)
RE2 hat in einem Sonderschirm-Handler ein Piepser-Paar: Record 0x01 fuer den einen, Record 0x02 fuer den anderen Zustand, je zweimal ausgeloest. Das ist die RE2-Entsprechung zu "Schloss auf / Schloss zu". RE1.5 hat es nicht.

**Beleg:** RE2 FUN@0x80033d60 (Eintrag [15] der Handler-Tabelle @0x800a243c; die Tabelle 0x800a242c..0x800a243c enthaelt 0x80033674, 0x800339a4, 0x80033b58, 0x80033d08, 0x80033d60). Zweig A: @0x80034020 `lbu v1,0x800a23c0[v0]`, @0x8003402c `sb v1,0x800c3ac0[v0]`, @0x80034044 `bne v0,s4,+` mit Delay @0x80034048 `lui a0,0x1` -> @0x8003404c `jal 0x8005ba28` (Bank 0 Rec 0x01) und Zustand s0=6 @0x80034058; sonst @0x80034064 `sb s4,0x800c3ac1[v1]`, @0x8003407c `bne v0,zero,+`, @0x80034084 `lui a0,0x2` -> @0x80034088 `jal` (Bank 0 Rec 0x02) und s0=5 @0x80034094; zweiter Aufruf @0x80034098. Zweig B identisch mit dem Array 0x800c3ad8/0x800a23e7/0x800a2406: @0x80034170 `sb v0,0x800c3ad8[v1]`, @0x80034188 `bne v0,s4,+`, @0x8003418c `lui a0,0x1` -> @0x80034190; Rec 0x02 @0x800341d4 und @0x800341e4. RE1.5: NEIN (keine Bank-0-Callsite im ganzen Spiel, s.o.). Port: NEIN.

**Umsetzung:** Zwei Toene aus der RE2-Tuer-Bank. ⛔ Vor der Umsetzung muss der AUFRUFER der Tabelle @0x800a242c ermittelt werden — welcher Bildschirm/welches Raetsel das ist, ist NICHT belegt (siehe offen). Bis dahin nur als Kandidat fuehren, nicht implementieren.

### RAUM — Item aufnehmen / Item passt nicht: 5 SE-Slots, in RE1.5 nicht ausdrueckbar
RE2s Aufnahme-/Untersuchen-Handler spielt 5 verschiedene Raum-Bank-SEs, unterschieden nach Item-Id und Erfolg. RE1.5s Aufnahme ist C-seitig komplett stumm; nur ein raumeigenes SCD-Se_on kann klingen.

**Beleg:** RE2 FUN@0x80051514: Item-Id 0xFE (`addiu v0,zero,254` @0x800515D4, `bne s0,v0` @0x800515D8) -> @0x800515F4 `lui a0,0x226` + @0x800515F8 `jal 0x8005ba28` = Bank 2 Rec 0x26. Item-Id 0xFF -> @0x8005160C `lui a0,0x216` + @0x80051610 = Bank 2 Rec 0x16. Sonst @0x80051628 `jal 0x800696cc` (Inventar-Insert); Erfolg (`bltz s1` @0x80051634 nicht genommen) -> @0x80051654 `lui a0,0x225` + @0x80051658 = Bank 2 Rec 0x25. Fehlschlag -> @0x800516a4 Bank 2 Rec 0x16. Weiter @0x80051cf8 Bank 2 Rec 0x15 und @0x800523a4 Bank 2 Rec 0x27. Verfuegbarkeit im RE2-Raumzensus (239 Raeume): Rec 0x15 in 28, 0x16 in 50, 0x25 in 27, 0x26 in 11, 0x27 in 10 Raeumen. RE1.5: NEIN — Records ≥0x20 sind durch `sltiu s4,0x21` @0x800450e4 gesperrt UND die 32-Record-Tabelle endet bei 0x1F. Port: NEIN — aot_common.c:1284 sagt ausdruecklich "The pickup SE is the room's own SCD Se_on".

**Umsetzung:** Nur ueber eine ZUSAETZLICHE residente Port-Bank machbar (nicht ueber die RE1.5-Raumbank). Konkret: die fuenf Records 0x15/0x16/0x25/0x26/0x27 aus einem RE2-Raum extrahieren, der sie fuehrt (RDT-Header snd0-EDT @+0x08, VH @+0x0c, VB @+0x10), als eigene VAB nach shared_assets/RE2/ legen und in re15_port/engine/src/aot_common.c am Item-AOT haengen: Erfolg=0x25, voll/abgelehnt=0x16, Sonder-Ids 0xFE=0x26 / 0xFF=0x16.

### RAUM — Save-Raum-Trias (Schreibmaschinen-Klang) als Raum-Records
RE2 fuehrt in genau 30 Raeumen ein festes Dreier-Set an den Slots 0x22/0x23/0x24 — das sind die Save-Raum-Geraeusche. RE1.5-Raeume haben diese Slots nicht.

**Beleg:** Zensus ueber 239 RE2-RDTs mit 48-Record-snd0 (Tabellengrenze aus RDT+0x08 bis RDT+0x0c): Rec 0x22 in 30 Raeumen, 0x23 in 30, 0x24 in 30 — haeufigste Eintraege 00007316 / 00008317 / 00009316. Identische Eintraege als Alias in COMMON/SOUND/CORE15.EDH: 0x22=00006316, 0x23=00007317, 0x24=00008316 (Duplikate auf 0x0A/0x0B/0x0C). Verbraucher: MEM_CARD.BIN Datei 0x2780 (0x22) und 0x2794 (0x24) sowie ENDING.BIN Datei 0x36B0 (0x23) und 0x374C (0x22). RE1.5: NEIN — 206-Raum-Zensus, snd0-EDT immer 0x80 B, Slots 0x00-0x1F durchgehend belegt.

**Umsetzung:** Deckungsgleich mit dem Posten "Save-Screen Bank-2-Specials": CORE15.EDH/.VB importieren, drei Records 0x22/0x23/0x24 als RE2-Systembank verfuegbar machen, am Save-Punkt (re15_port/engine/src/re15_savepoint.c) und am Save-Screen ausloesen. EIN Import deckt beide Posten.

### RAUM — Terminal-/Tastenfeld-Tippgeraeusch
RE2s Computerterminal (DOOR-LOCK-SERVICE, Benutzernamen-Eingabe) tippt hoerbar: Bank 2 Record 0x12, dreimal ausgeloest. RE1.5 hat kein Terminal — der Ton ist nur relevant, falls der Port ein Tastenfeld-Raetsel nachbaut.

**Beleg:** RE2 FUN@0x8003a9f4 (einziger Aufrufer @0x80039860; Text-Xrefs: @0x8003ab98 -> "DOOR LOCK SERVICE]---" @0x80010bcc, @0x8003ac14 -> "Hall side doors: |2RELEASED|0]" @0x80010c00, @0x8003ac84 -> "...LOCKED..." @0x80010c24, @0x8003b0a0 -> "]OK!]Hall side doors lock released." @0x80010ca8, @0x8003b4ec -> "Enter your user name.]>" @0x80010db4). SE-Callsites: @0x8003af8c, @0x8003afe4, @0x8003b050 — alle `lui a0,0x212` = Bank 2 Rec 0x12. Weitere Sonderschirm-SEs derselben Dispatcher-Gruppe: @0x80039cdc Bank 2 Rec 0x10, @0x80039e08 Rec 0x09, @0x80039f10/@0x80039fac Rec 0x06, @0x80039f68/@0x80039fd8 Rec 0x07, @0x8003a504 Rec 0x10, @0x8003b8ac Rec 0x06, @0x8003b934 Rec 0x07. RE1.5: NEIN (kein Terminal, kein entsprechender Handler). Port: NEIN.

**Umsetzung:** NUR umsetzen, wenn ein Tastenfeld/Terminal im Port entsteht. Sample-Quelle: RE2-Raum-snd0 Rec 0x12 des Terminalraums; im RE1.5-Bankmodell nicht abbildbar, also ebenfalls ueber die residente RE2-Systembank.

### BANK-INVENTAR — welche CORE-Baenke RE2 hat und RE1.5 nicht
Von 13 gemeinsamen CORE-Nummern sind 4 byte-gleich (00, 0F, 12, 13), CORE10 und CORE11 sind zwischen den Spielen VERTAUSCHT, und RE2 hat zwei Baenke ohne RE1.5-Gegenstueck: CORE14 und CORE15. Nur CORE15 traegt Records, die RE1.5 nirgends hat.

**Beleg:** md5 EDH/VB: CORE00 9b0e0627 / cdcb61fb in BEIDEN. CORE0F 0d1e2c88 / 13206a1a in beiden. CORE12 7a3073c7 / 7e686e0d in beiden. CORE13 e6220f07 / 48c092db in beiden. RE1.5-CORE10 8fa58cb6 == RE2-CORE11; RE1.5-CORE11 e3ca8272 == RE2-CORE10 (beidseitig, EDH und VB). RE2-only: CORE14 (EDH d93bb3d4, VB 199056 B, Tabelle 0x40, Records 0x00/0x04/0x05/0x06) und CORE15 (EDH 7009fa8d, VB 63904 B, Tabelle 0xC0 = 48 Records, gueltig 0x04, 0x06, 0x0A, 0x0B, 0x0C, 0x10, 0x22, 0x23, 0x24). Ausserdem: RE1.5-CORE04..09 == RE2-CORE01 (82f06386 / ab90ba10). RE2-CORE0D fuehrt zwei Records mehr als jede RE1.5-Bank (0x0B=0000c316, 0x0C=0000d316) — werden von RE2 aber NIRGENDS gerufen (Voll-Scan: hoechster Bank-4-Record im ganzen RE2-Code ist 0x10).

**Umsetzung:** Zu importieren ist GENAU EINE Datei-Paarung: info/re2leon/COMMON/SOUND/CORE15.EDH + CORE15.VB nach re15_port/shared_assets/RE2/. CORE14 NICHT importieren (nur Records 0x00/0x04/0x05/0x06 = dieselbe Rolle wie RE1.5-CORE12/13). Alle uebrigen Menue-Toene der Liste sind reiner Code.

## Offen
- Welche CORE-Bank waehrend DIEDEMO.BIN / TITLE.BIN / SELECT.BIN / MEM_CARD.BIN resident ist, ist NICHT belegt. Der Bankwaehler ist FUN@0x80059a40 mit s3 als Index in die u16-Tabelle @0x800a80b8 (Index->EDH-Datei-Id: 0->327, 1->329, 2->327, 3->329, 4->327, 5->329, 6->327, 7->329, 8->327, 9->329, 0x0A->327, 0x0B->331, 0x0C->333, 0x0D->335, 0x0E->337, 0x0F->339, 0x10->341, 0x11->343, 0x12->345, 0x13->347, 0x14->349, 0x15->351 = CORE00/01/0B..15). WER s3 setzt, wurde nicht verfolgt. Folge: Record 0x00 auf dem Game-Over-Schirm ist als SAMPLE nicht identifiziert.
- Der AUFRUFER der 5-Handler-Tabelle @0x800a242c (0x80033674 / 0x800339a4 / 0x80033b58 / 0x80033d08 / 0x80033d60) ist nicht gefunden — die Tabelle wird nur als Datenwort referenziert (Selbstzeiger @0x800a243c). Damit ist der Ereignisname der Bank-0-Piepser 0x01/0x02 (Posten 'Schloss-Piepser') UNBELEGT. Naechster Weg: Xref auf 0x800a242c per lui/addiu-Paar oder Laufzeit-Beobachtung.
- Kein RE2-Zwilling fuer die YES/NO-Box der Message-VM lokalisiert (RE1.5 FUN_80028134 case 4). Naechster Weg: RE2s Message-FSM ueber die Aufrufer von 0x8002c1a0 / den RE2-Message-Open suchen.
- Die SCD-Se_on-Opcodes in den 250 RE2-RDTs wurden NICHT gezaehlt (nur die EDT-Slot-Belegung). Damit fehlen die pro-Raum-Ereignisse 'Tuer verschlossen', 'Raetsel geloest (Jingle)', 'Hebel/Schalter', 'Aufzug', 'Kisten schieben', 'Ventil' als KONKRETE Callsites. Voraussetzung: RE2s SCD-Opcode-Nummer fuer Se_on und die Operandenlaenge bestaetigen (RE1.5 = 0x36, 12 B, Packer LAB_80041624 / EXE-Callsite @0x80041730).
- Die 34 CONFIG.BIN-Callsites sind NICHT den vier Unterschirmen (CONFIG / SOUND / Tastenbelegung / EXIT) einzeln zugeordnet. Fuer die Umsetzung reicht die Regel 4/5/6, aber eine 1:1-Zuordnung fehlt.
- Die Bedeutung der Pad-Masken im CONFIG-Virtualwort (0x1000/0x2000/0x4000/0x8000 an @0x801c0e84/@0x801c0ff4/@0x801c1170/@0x801c1628) ist nicht aufgeloest — bewusst NICHT als Up/Down/Left/Right etikettiert.
- Rolle der einzelnen MEM_CARD-Specials: dass 0x22 der Start- und 0x24 der Endton des Speicherns ist, ist eine Reihenfolge-Annahme aus den Datei-Offsets (0x2780 vor 0x2794), nicht aus dem FSM abgeleitet.
- Nicht geprueft, ob der RE1.5-Bank-0-Zeiger 0x801fdd00 zur Laufzeit ueberhaupt befuellt wird. Falls er dauerhaft -1 traegt, bricht Se_on schon @0x8004506c ab — dann ist 'Bank 0 in RE1.5 unbenutzt' nicht nur ungerufen, sondern nicht benutzbar.
- RE2-Rec-0x0B/0x0C in CORE0D (0000c316 / 0000d316) haben keinen einzigen Aufrufer im gescannten RE2-Code. Ob sie ueber einen hier nicht erfassten Pfad (RDT-SCD, weitere Overlays) gespielt werden, ist offen.
- RESULT.BIN und OPENING.BIN haben 0 Se_on-Aufrufe — nicht weiter geprueft, ob deren Toene ueber einen anderen Player (BGM/XA) laufen.


---

# RE2-Dokument-Vorlagen (FILES.TIM) portierbar machen — Container-Struktur, Anzeige-Mechanismus, RE1.5-Gegenstueck, Port-Weg

FILES.TIM ist KEINE einzelne TIM, sondern ein Container aus 216 aneinandergehaengten TIMs, gruppiert in 191 sektor-ausgerichtete Slots und 25 Dokumente. Alle Offsets/Groessen stehen als Tabellen in der RE2-PSX.EXE (Slot-Records @0x800a94b4, Dokument-Startslot @0x800a9ad0, Dokument-Record @0x800aa144). Ein Dokument = 1x 8bpp-Illustration 128x256 + N x 4bpp-Textseite 256xH (H aus 112/128/144/176). Der FILE-Leser zeichnet beide als SPRT in eine 256x256-Flaeche: Text auf Zeilen 0..H-1 (CLUT (0,490)), Illustration 128 breit auf Zeilen H..255 (CLUT (0,489)). RE1.5 hat KEINE Dokumentbilder — dessen FILE-Screen ist reiner Text (7-Seiten-Blob @0x800ccd34 in DEBUG.BIN); ITPS.ITP (72x 112x72 CHECK-Fotos) und STPIC_*.TIM (16x 116x103 Status-Portraits) sind etwas anderes. Fuer eigene Dokumente braucht der Port eine neue Bild-Ebene im FILE-Leser plus zwei Werkzeuge (Schneider — bereits geschrieben und ausgefuehrt: re15_port/tools/re2_files_cut.py, 216 TIMs + toc.csv unter build/extracted/re2_files/ — und ein noch fehlendes PNG-zu-4bpp/8bpp-TIM-Autorenwerkzeug).

## Posten

### FILES.TIM — Grundstruktur
info/re2leon/COMMON/DATA/FILES.TIM (5.257.216 B = 0x503800) ist ein Container aus 216 vollstaendigen PSX-TIMs. Kein Container-Header, keine Magic vorne ausser der ersten TIM selbst: Datei-Offset 0x0 = 10 00 00 00 09 00 00 00 (TIM-Magic 0x10, Flags 0x09 = 8bpp + CLUT).

**Beleg:** xxd -l 16 FILES.TIM = '1000 0000 0900 0000 0c02 0000 0000 e001'; Kettenlauf ueber alle 216 Bloecke deckt genau 216 TIMs auf (Skript re15_port/tools/re2_files_cut.py)

**Umsetzung:** Kein eigener Container-Parser noetig: 216 Einzel-TIMs liegen bereits geschnitten in build/extracted/re2_files/ (216 .TIM + toc.csv). Port laedt sie ueber das vorhandene re15_tim_parse (include/re15_tim.h:60, engine/src/tim_common.c).

### FILES.TIM — Slot-Gitter
Die 216 TIMs sind in 191 SLOTS gruppiert. Ein Slot enthaelt 1 oder 2 TIMs, byte-kontiguierlich ohne Padding dazwischen; der naechste Slot beginnt an der naechsten 0x800-Grenze (CD-Sektor). 25 Slots enthalten 2 TIMs (Illustration + Titelseite), 166 Slots enthalten 1 TIM (Folgeseite).

**Beleg:** Slot 0 @0x000000: TIM0 33312 B endet 0x8220, TIM1 startet unmittelbar 0x8220, endet 0xCA60 -> naechster Slot @0xD000. Deckt sich 1:1 mit der EXE-Tabelle @0x800a94b4 (size/sector je Slot).

**Umsetzung:** Beim Nachbau eigener Container: TIMs eines Slots direkt aneinander, danach mit 0x00 bis zum naechsten 0x800-Vielfachen auffuellen.

### EXE-Tabelle: Slot-Records
Slot-Record-Tabelle @0x800a94b4 (RE2-PSX.EXE, Datei-Offset 0x99CB4), 191 Eintraege a 8 Byte: {u32 groesse_in_bytes; u16 sektor_lo; u8 sektor_hi; u8 pad}. byteoffset_in_FILES.TIM = (sektor_lo | sektor_hi<<16) * 0x800.

**Beleg:** 0x8006d4d8 'lw v0,-27468(at)' (at=0x800b0000+slot*8) = +0 groesse -> 0x800d5308; 0x8006d518 'lhu a0,-27464(at)' = +4; 0x8006d50c 'lbu v0,-27462(at)' = +6, sll 16; 0x8006d4f0 'lbu v0,-27461(at)' = +7 -> 0x800d531e

**Umsetzung:** Tabelle 1:1 als u32/u32-Paare in eine .inc generieren (Muster: engine/src/gen/re2_ems_toc.inc). Bereits als CSV: build/extracted/re2_files/toc.csv Spalten file_off/tim_bytes.

### EXE-Tabelle: Dokument -> erster Slot
@0x800a9ad0 (Datei-Offset 0x9A2D0), 25 x u8 = {0,7,14,17,21,32,43,54,62,68,76,79,83,88,92,97,107,115,126,133,136,144,151,157,174}. Danach 0x00 — es gibt genau 25 Dokumente.

**Beleg:** 0x8006d480 'lbu a0,-25904(at)' mit at = 0x800b0000 + docid; Bytefolge im EXE-Dump @0x800a9ad0 = 00 07 0e 11 15 20 2b 36 3e 44 4c 4f 53 58 5c 61 6b 73 7e 85 88 90 97 9d ae 00

**Umsetzung:** static const uint8_t re2_doc_first_slot[25] = {0,7,14,17,21,32,43,54,62,68,76,79,83,88,92,97,107,115,126,133,136,144,151,157,174};

### EXE-Tabelle: Dokument-Record
@0x800aa144, 25 Eintraege a 4 Byte: {u16 max_page; u8 y_off; u8 pad}. seitenzahl = max_page+1. seitenhoehe H = 256 - y_off.

**Beleg:** max_page: 0x80076224 'lhu s4,-24252(at)' (at=0x800b0000+docid*4), Vergleich gegen Seitenzeiger 0x800d5c03 @0x800762c8; y_off: 0x8007603c 'lhu s3,-24250(at)' gefolgt von 0x80076040 'addiu s1,zero,256' / 0x80076044 'subu s5,s1,s3' -> s5 = 256-y_off = Sprite-Hoehe

**Umsetzung:** static const struct {uint16_t max_page; uint8_t page_h;} — page_h = 256 - y_off. Werte in build/extracted/re2_files/toc.csv Spalten doc_max_page/doc_page_h.

### Slot-Auswahl (Seite -> Slot)
slot = doc_first_slot[docid] wenn Flag @0x800d5c05 == 0 (Titelseite); sonst slot = doc_first_slot[docid] + 1 + page, page = u8 @0x800d5c03. docid = u8 @0x800d4b68[ sel_row*8 + sel_col ], sel_row = s8 @0x800d5c01, sel_col = s8 @0x800d5c02.

**Beleg:** 0x8006d448-50 laden sel_row/sel_col; 0x8006d44c 'sll v0,v0,3'; 0x8006d46c 'lbu v0,19304(at)' = 0x800d4b68; 0x8006d484 'beq v1,zero,0x8006d49c' (Flagtest); 0x8006d488 'addiu v0,a0,1'; 0x8006d498 'addu a0,v0,v1'

**Umsetzung:** Portseitig: doc_slot(doc,page) { return page<0 ? first[doc] : first[doc]+1+page; }  page==-1 bezeichnet die Titelseite (2-TIM-Slot).

### CD-Anbindung (nur PSX-Original relevant)
FILES.TIM ist CD-Datei-Id 166 (0xA6), Basis-LBA 42049. Geladen wird immer nur EIN Slot: groesse -> 0x800d5308, LBA = 42049 + slot_sektor -> 0x800d5314, id 166 -> 0x800d531c, dann jal 0x80012fb8 (a0=166). Zielpuffer wird ueber den Debug-Tag 'FILE ETC' @0x80011c7c alloziert.

**Beleg:** 0x8006d4b4 'sh v0(=166),21276(at)'; 0x8006d4a8/0x8006d4bc lesen 0x80098dda(u8)/0x80098dd8(u16) = 0|42049; 0x8006d52c 'sw v1,21268(at)'; 0x8006d530 'jal 0x80012fb8' + delay 'addiu a0,zero,166'; 0x800   6d4f8 'addiu a3,a3,7292' = 0x80011c7c

**Umsetzung:** Im PC-Port entfaellt CD-Streaming: die 216 geschnittenen TIMs direkt als Dateien mitliefern (oder als ein Blob mit generierter TOC).

### Anzeige — Geometrie (Kern)
Der FILE-Leser zeichnet ZWEI SPRT-Prims (GPU-Code 0x66, rgb 128/128/128, setlen 4) in eine 256x256-Flaeche:  [1] Textseite 4bpp: u=0, v=0, w=256, h=H, CLUT = GetClut(0,490).  [2] Illustration 8bpp: u=0, v=H, w=128, h=256-H, CLUT = GetClut(0,489).

**Beleg:** FUN 0x80075fd0. Prim1: 0x80076068 'sb zero,-2(s0)'(u0=0), 0x8007606c 'sb zero,-1(s0)'(v0=0), 0x80076070 'sh s1(=256),2(s0)'(w), 0x80076078 'sh s5(=256-y_off),4(s0)'(h), 0x80076048/4c GetClut(0,490). Prim2: 0x800760b8 'subu v0,zero,s3' -> v0-Byte = H, 0x800760d0 'sb v0,-1(s0)', 0x800760d4 'sh s1(=128),2(s0)'(w), 0x800760dc 'sh s3,4(s0)'(h=256-H), 0x800760a8/ac GetClut(0,489). s0 zeigt auf SPRT+14 (clut) — bestaetigt durch 'sh v0,0(s0)' direkt nach dem GetClut-jal.

**Umsetzung:** emit_file_reader() im Port: sprt(page=DOC4, clut=DOCPAGE, x, y,   256, H, 0, 0, 128,128,128, 0) und sprt(page=DOCPAPER8, clut=DOCPAPER, x, y+H, 128, 256-H, 0, H, 128,128,128, 0).

### Anzeige — visuell verifiziert
Die 8bpp-Illustration ist eine 128x256-Leinwand, deren Motiv genau in den Zeilen H..255 liegt. Zusammensetzung 'Textseite oben (256xH) + Illustration darunter (128x(256-H), aus Quellzeilen H..255)' ergibt exakt die bekannte RE2-Dokumentansicht (Buch/Notizzettel unter dem Titel).

**Beleg:** Komposit aus FILES_0014.bmp (doc0 paper) + FILES_0015.bmp (doc0 title page, H=144) und FILES_0091/0092 (doc9, H=176) gerendert und betrachtet: Motiv beginnt exakt bei Zeile H, unten buendig bei 255, kein Beschnitt.

**Umsetzung:** Beim Autorenwerkzeug: das Illustrationsbild MUSS auf einer 128x256-Leinwand unten buendig liegen; die oberen (256-H) Zeilen bleiben leer (Index 0).

### Anzeige — Endseiten-Marker
Wenn page == max_page zeichnet der Leser einen Zusatz-Sprite: x=280, y=110, u=56, v=12, w=42, h=14, CLUT = GetClut(256,490).

**Beleg:** 0x800762c8 'bne v0(page),s4(max_page),0x8007632c'; 0x800762dc 'sh 42,16(s0)'; 0x800762e4 'sh 14,18(s0)'; 0x800762ec 'sb 56,12(s0)'; 0x800762f4 'sb 12,13(s0)'; 0x800762fc 'sh 280,8(s0)'; 0x80076304 'sh 110,10(s0)'; 0x800762d0/d4 GetClut(256,490)

**Umsetzung:** Optional. Im RE1.5-Port existiert bereits ein eigener Pfeil-/Fusszeilen-Satz (emit_file_arrows / emit_file_footer) — nicht doppelt zeichnen.

### FILE-Liste (Metadaten zur Laufzeit)
Die Liste ist ein u8-Array 0x800d4b68[24], 0xFF = leerer Platz. Beim Aufsammeln haengt FUN 0x800692dc die docid an den ersten freien Platz an. Es gibt KEINE Titel-Strings zu den Dokumenten in der EXE — der Titel steht als Pixel in der Titelseite.

**Beleg:** Loeschschleife 0x800682dc-f8 ('addiu a1,zero,24' / 'sb 255,19304(at)'); Anhaengefunktion 0x800692dc-0x80069314 ('sltiu v0,a1,0x18' = Grenze 24, 'sb a0,0(v1)')

**Umsetzung:** Portseitig: uint8_t file_list[24] mit 0xFF-Init; file_list_add(docid). Titel = Bild, also KEIN Namensband noetig — anders als RE1.5, das Zeilennamen als Textcodes fuehrt.

### Rookie-Hinweisdatei — Sonderfall
Dokument 23 wird abhaengig von zwei Flag-Bits an Listenplatz 0 gesetzt UND seine Seitenzahl zur Laufzeit auf max_page=1 gepatcht.

**Beleg:** 0x80068324-38 'lw a0,0x800cfbd8' & 0x04000000; 0x8006833c 'lw v0,0x800cfb74' & 0x00200000; 0x80068350 'lui v0,0x4000' & 0x40000000; sonst: 0x80068368 'sh 1,-24160(at)' -> 0x800aa1a0 (= 0x800aa144 + 23*4) und 0x80068374 'sb 23,19304(at)' -> 0x800d4b68[0]

**Umsetzung:** Zeigt: max_page ist ein RAM-Wert und darf zur Laufzeit wachsen. Im Port die Seitenzahl je Dokument als veraenderliche Variable fuehren, nicht als const.

### Dokument-Inhaltstabelle 00-06
doc00 CHRIS'S DIARY: Slots 0-6, 6 Seiten (max_page 5), H=144, Datei-Offset 0x000000 | doc01 FEDERAL POLICE DEPT. -INTERNAL INVESTIGATION REPORT: 7-13, 6 S., H=176, 0x02b000 | doc02 MEMO TO LEON: 14-16, 2 S., H=176, 0x05d000 | doc03 POLICE MEMORANDUM: 17-20, 3 S., H=176, 0x077000 | doc04 OPERATION REPORT: 21-31, 10 S., H=176, 0x097000 | doc05 MAIL TO THE CHIEF: 32-42, 10 S., H=176, 0x0e1000 | doc06 MAIL TO THE CHIEF: 43-53, 10 S., H=176, 0x12b000

**Beleg:** Slots+Offsets aus 0x800a9ad0 / 0x800a94b4; Seitenzahl/H aus 0x800aa144; Titel aus den Pixeln der jeweiligen Titelseiten (FILES_NNNN.bmp, N = 14 + laufender TIM-Index) gerendert und gelesen

**Umsetzung:** Namen als Kommentar in die generierte TOC. Dateien: build/extracted/re2_files/FILE00_*.TIM ... FILE06_*.TIM

### Dokument-Inhaltstabelle 07-13
doc07 SECRETARY'S DIARY A: Slots 54-61, 7 Seiten, H=144, 0x175000 | doc08 SECRETARY'S DIARY B: 62-67, 5 S., H=144, 0x1a5000 | doc09 OPERATION REPORT 2: 68-75, 5 S. (max_page 4) bei 8 Slots -> 2 Slots ungenutzt, H=176, 0x1cb000 | doc10 USER REGISTRATION: 76-78, 2 S., H=128, 0x203000 | doc11 FILM A: 79-82, 3 S., H=112, 0x218800 | doc12 FILM B: 83-87, 4 S., H=112, 0x230800 | doc13 FILM C: 88-91, 3 S., H=112, 0x24c800

**Beleg:** wie oben; Titel per Pixel-Render der Titelseiten verifiziert

**Umsetzung:** doc09: Slots 74/75 sind Reserve-Seiten, im Original nie erreichbar.

### Dokument-Inhaltstabelle 14-19
doc14 PATROL REPORT: Slots 92-96, 4 Seiten, H=144, 0x264800 | doc15 WATCHMAN'S DIARY: 97-106, 9 S., H=176, 0x285800 | doc16 CHIEF'S DIARY: 107-114, 7 S., H=176, 0x2c9800 | doc17 SEWER MANAGER DIARY: 115-125, 10 S., H=176, 0x301800 | doc18 SEWER MANAGER FAX: 126-132, 6 S., H=176, 0x34b800 | doc19 RECRUIT: 133-135, 2 S., H=112, 0x37d800

**Beleg:** wie oben

**Umsetzung:** —

### Dokument-Inhaltstabelle 20-24
doc20 INSTRUCTIONS FOR SYNTHESIS OF THE G-VIRUS ANTIGEN: G-VACCINE. CODE NAMED "DEVIL.": Slots 136-143, 7 Seiten, H=176, 0x391800 | doc21 LABORATORY SECURITY MANUAL: 144-150, 6 S., H=176, 0x3c9800 | doc22 INVESTIGATIVE REPORT ON P-EPSILON GAS: 151-156, 5 S., H=176, 0x3fb800 | doc23 HINT FILES FOR THE ROOKIE MODE: 157-173, 17 Slots aber max_page 2 -> 14 Slots Reserve, H=176, 0x427800 | doc24 HINT FILES FOR THE ROOKIE MODE: 174-190, 17 Slots, max_page 2, H=176, 0x495800

**Beleg:** wie oben; Inhalt der Reserve-Slots per Pixel-Render geprueft (FILES_0196/0200/0209/0211 usw.): echte Hinweistexte ('A gas light in the press room.', 'Elevator leading to the transport facility...') — also progressiv freigeschaltete Hinweisseiten

**Umsetzung:** Beste Vorlagen-Quelle fuer eigene Texte: doc23/doc24 liefern 34 fertig gesetzte 256x176-Textseiten mit identischer Palette.

### Seiten-TIM — Pflichtformat (4bpp)
Flags 0x08 (pmode 0 = 4bpp, CLUT-Bit gesetzt). CLUT-Block: len=44, rect (x=0, y=480, w=16, h=1), 16 x BGR555. Bild-Block: len = 12 + 128*H, rect (x=0, y=0, w=64 Halbworte, h=H). Breite IMMER 256 Pixel (= 64 Halbworte). Gesamtgroesse = 8 + 44 + 12 + 128*H.

**Beleg:** Gemessen an allen 191 Seiten-TIMs; H=112 -> 14400 B, H=128 -> 16448 B, H=144 -> 18496 B, H=176 -> 22592 B (alle vier Werte kommen vor)

**Umsetzung:** Autorenwerkzeug muss exakt diese Header schreiben. Nur die vier Hoehen 112/128/144/176 sind belegt; H wird ueber den Dokument-Record (256-y_off) transportiert.

### Seiten-Palette — Textrampe
Die Textseiten-CLUT ist ueber Dokumente hinweg identisch in den Eintraegen 0..8: 0=0x0000 (transparent), 1=0x677b, 2=0x5f39, 3=0x4eb5, 4=0x4231, 5=0x35ce, 6=0x294a, 7=0x8000, 8=0x1063; Rest 0x8000-Fueller (dokumentweise abweichend, z.B. doc23: 14=0x188c, 15=0x34e6).

**Beleg:** FILE00_title_page.TIM und FILE23_p00_page.TIM, CLUT-Daten ab Datei-Offset 0x14, ausgelesen

**Umsetzung:** Fuer eigene Textseiten diese 9er-Rampe uebernehmen -> Antialiasing sieht identisch aus. Index 0 MUSS transparent bleiben (Port: inv_render_pc.c behandelt CLUT-Wert 0 als transparent).

### Illustrations-TIM — Pflichtformat (8bpp)
Flags 0x09 (pmode 1 = 8bpp + CLUT). CLUT-Block: len=524, rect (0, 480, 256, 1), 256 x BGR555. Bild-Block: len=32780, rect (0, 0, 64, 256) = 128 Pixel breit, 256 hoch. Gesamtgroesse IMMER 33312 B. Alle 25 Illustrationen sind byte-identisch im Header.

**Beleg:** Gemessen an allen 25 8bpp-TIMs im Container; FILE00_title_paper.TIM: 33312 B, flags=0x09, clut 524 (0,480,256,1), img 32780 (0,0,64,256)

**Umsetzung:** Feste Groesse -> im Port als 128x256-Fenster (uint8_t s_docpaper8[256][128]) ablegen.

### CLUT-Zielzeilen im VRAM
Beide TIMs tragen im Kopf die CLUT-Rechteck-Zeile 480, das Spiel benutzt beim Zeichnen aber 489 (Illustration, 256 Eintraege) und 490 (Textseite, 16 Eintraege). Der Lader verschiebt die CLUTs also beim Upload.

**Beleg:** TIM-Kopf: crect (0,480,...) in allen 216 Bloecken; Zeichencode: 0x800760ac 'addiu a1,zero,489' und 0x8007604c 'addiu a1,zero,490' als GetClut-y

**Umsetzung:** Fuer den PC-Port irrelevant (kein VRAM-Modell noetig), aber fuer ein PSX-Target: 8bpp-CLUT nach (0,489), 4bpp-CLUT nach (0,490) laden — nicht die Kopf-Rechtecke verwenden.

### RE1.5: ITPS.ITP ist NICHT Dokument
info/Re1.5/PSX/ITEM/ITPS.ITP (884736 B) = 72 Slots a 0x3000 B, jeder ein 8bpp-TIM 112x72, CLUT 256 Eintraege, VRAM prect (832,256) 56 Halbworte x 72, crect (0,489). Das sind die CHECK-/Untersuchen-Fotos je Gegenstand.

**Beleg:** Kettenlauf ueber ITPS.ITP: 72 TIMs, jeweils 8608 B Nutzdaten in 0x3000-Raster; Portseite byte-verifiziert in re15_port/include/re15_itps.h und re15_port/platform/pc/src/inv_render_pc.c:295-336 (crect-Test (0,489), prect-Test (832,256,56,72))

**Umsetzung:** NICHT als Dokumentquelle verwenden. Das 0x3000-Raster ist aber die beste Vorlage fuer einen eigenen Dokument-Container mit fester Schrittweite.

### RE1.5: STPIC_*.TIM ist NICHT Dokument
info/Re1.5/PSX/ITEM/STPIC_00..0F.TIM (16 Dateien a 12492 B) = 8bpp-TIM 116x103, CLUT 256 Eintraege crect (0,485), prect (640,406) 58 Halbworte x 103. Status-/Zustandsportraits.

**Beleg:** Kopf-Parse: clut 524 (0,485,256,1), img 11960 (640,406,58,103), identisch fuer STPIC_00 und STPIC_0F

**Umsetzung:** NICHT als Dokumentquelle verwenden.

### RE1.5 hat keine Dokumentbilder
Im gesamten RE1.5-CD-Baum info/Re1.5/PSX/ (BIN, DATA, DOOR, EMD, ITEM, MOVIE, PLD, SOUND, STAGE1-6, VOICE) existiert kein FILES-Aequivalent. DATA/ enthaelt nur AAA.TIM, CONFIG.TIM, CORE00.ESP, C_BACK2.TIM, ITEMALL.PIX, MAP01-0D.PIX, MIXITEM.PIX, ROOM115U/506U.SCD, SELECT*.TIM, ST_00.TIM, TEX.TIM, TITLEJ/U.TIM, TMOJI.TIM, TYPE00.TIM, YOUDIED.TIM.

**Beleg:** Verzeichnislisting info/Re1.5/PSX/ und info/Re1.5/PSX/DATA/

**Umsetzung:** Die Bild-Ebene ist im Port ein NEUBAU, keine Reaktivierung.

### RE1.5 FILE-Screen ist reiner Text
Der RE1.5-Leser druckt Textcodes: Dokument-Blob @0x800ccd34 (DEBUG.BIN), 7 Seiten, Seitenzahl = u16 @0x800ccd34 >> 1; Leser FUN 0x800c6ca0; Glyphdrucker FUN_80013160 / 0x80028f6c; Textursprung x=0x28, y=0x20; Fusszeile zentriert x = 0xa0 - breite/2, y = 0xd2.

**Beleg:** re15_port/engine/src/gen/inv_file_doc.inc Kopf (Generator tools/gen_inv_file_doc.py) und re15_port/engine/src/re15_inv_screen.c:561-700 mit den zitierten Adressen

**Umsetzung:** Bild-Ebene ergaenzen, Textebene NICHT ersetzen — sonst faellt die byte-true RE1.5-Darstellung weg.

### RE1.5 FILE-Liste — Umfang
3 Listenseiten, Sichtbarkeitsmasken u16[3] @0x800c6c98 = {0x0001, 0xffff, 0xffff}, Zeilenbasen u8[3] @0x800c7370 = {0x48, 0x52, 0x5c}, 30 Zeilennamen (Ids 0x48..0x65) aus den Baenken @0x800c495c / 0x800c4a28, Titel @0x800c78e4, Unterstrich-String @0x800c7916.

**Beleg:** re15_port/engine/src/gen/inv_file_doc.inc: re15_inv_file_mask[3], re15_inv_file_rowbase[3], re15_inv_file_name_off[30]

**Umsetzung:** RE1.5 hat also Platz fuer 30 Dokumenteintraege — mehr als RE2s 24 Listenplaetze und mehr als die 25 RE2-Dokumente. Passt.

### Port: Prim-Schnittstelle
Der Inventar-Zeichner arbeitet mit re15_inv_op_t {kind, page, clut, abe, x,y,w,h (int16), u,v (uint8), r,g,b} und der Hilfsfunktion sprt(e,page,clut,x,y,w,h,u,v,r,g,b,abe). Seiten-Enum RE15_INV_PAGE_* aktuell 0..6, CLUT-Selektoren 0..16, s_clut[17][256], RE15_INV_MAX_OPS 768.

**Beleg:** re15_port/include/re15_inv_screen.h:42-63 (Pages), :80-110 (CLUTs), :113-122 (op-Struct), :127; re15_port/engine/src/re15_inv_screen.c:394-405 (sprt); re15_port/platform/pc/src/inv_render_pc.c:70 (s_clut)

**Umsetzung:** Zwei neue Page-Ids ergaenzen (z.B. RE15_INV_PAGE_DOC4 = 7, RE15_INV_PAGE_DOCPAPER8 = 8) und zwei neue CLUT-Ids (17 = Seiten-16er, 18 = Illustrations-256er); s_clut auf [19][256] vergroessern.

### Port: Rasterizer-Falle u-Wrap
Der SPRT-Rasterizer maskiert die Texturkoordinaten mit &255: 'int v = (o->v + py) & 255;' und 'int u = (o->u + pxx) & 255;'. Eine 256 Pixel breite Seite geht daher nur mit u=0 und einem [256]-breiten Fenster.

**Beleg:** re15_port/platform/pc/src/inv_render_pc.c:642-644

**Umsetzung:** Fenster als uint8_t s_doc4[256][256] (4bpp entpackt auf 1 Byte/Pixel) und uint8_t s_docpaper8[256][128] anlegen; im Page-Selektor der Zeile 645-651 zwei Zweige ergaenzen: DOC4 -> s_doc4[v][u], DOCPAPER8 -> (u<128 ? s_docpaper8[v][u] : 0).

### Port: Transparenz-Regel deckt sich
Der Rasterizer verwirft CLUT-Wert 0 als transparent ('if (c == 0) continue;'). Die RE2-Seiten-CLUT hat Eintrag 0 = 0x0000, die Illustrations-CLUT ebenfalls 0x0000. Passt ohne Sonderbehandlung.

**Beleg:** re15_port/platform/pc/src/inv_render_pc.c:653; CLUT-Dump FILE00_title_page.TIM Eintrag0 = 0000, FILE00_title_paper.TIM Eintrag0 = 0000

**Umsetzung:** Keine Aenderung noetig.

### Port: Einbau in emit_file_reader
emit_file_reader(e, st, x) zeichnet heute nur Text (x, 0x20) plus Fusszeile. Hier die zwei Bild-SPRTs VOR dem Text einreihen (Ops werden in Reihenfolge rasterisiert, spaeter = oben).

**Beleg:** re15_port/engine/src/re15_inv_screen.c:656-661 (emit_file_reader), :978-995 (Aufrufkette, FILE-Zustand 25c1==2)

**Umsetzung:** if (doc_has_image(doc)) { sprt(e, PAGE_DOCPAPER8, CLUT_DOCPAPER, ox, oy+H, 128, 256-H, 0, H, 128,128,128, 0); sprt(e, PAGE_DOC4, CLUT_DOCPAGE, ox, oy, 256, H, 0, 0, 128,128,128, 0); } — Ursprung ox/oy muss neu festgelegt werden (siehe offene Punkte).

### Werkzeug 1 (GESCHRIEBEN + AUSGEFUEHRT): Schneider
re15_port/tools/re2_files_cut.py schneidet FILES.TIM anhand der drei EXE-Tabellen in 216 eigenstaendige TIMs plus toc.csv. Ausgabe liegt in build/extracted/re2_files/ (5,3 MB, 216 .TIM + toc.csv). Namensschema FILE<doc:02d>_<title|pNN>_<page|paper>.TIM.

**Beleg:** Lauf: '216 TIMs, 25 Dokumente -> C:\workspace\git\reAi_v2\build\extracted\re2_files'; toc.csv hat 217 Zeilen (Kopf + 216)

**Umsetzung:** Nichts mehr zu tun. Aufruf: python re15_port/tools/re2_files_cut.py [zielverzeichnis]

### Werkzeug 2 (FEHLT): PNG -> Dokument-TIM
Fuer EIGENE Dokumente fehlt ein Autorenwerkzeug 'doc_png_to_tim.py': (a) Textseite 256xH RGB-PNG -> 4bpp-TIM mit fester 9-Farb-Rampe (Index 0 transparent); (b) Illustration 128x256 RGB-PNG -> 8bpp-TIM mit 256er-Palette. Das vorhandene tools/vram_png_to_tim.py taugt NICHT: es liest ein DuckStation-VRAM-PNG und rekonstruiert rohe VRAM-Halbworte, es quantisiert kein beliebiges RGB-Bild.

**Beleg:** re15_port/tools/vram_png_to_tim.py Kopfkommentar und main(): Parameter --tpage/--clut, Funktion v15(x,y) liest VRAM-Pixel; keine Palettenbildung

**Umsetzung:** Neu schreiben. Pflichtkonstanten: 4bpp -> flags 0x08, clut_len 44, crect (0,480,16,1), prect (0,0,64,H), Bilddaten 128*H Byte, zwei Pixel je Byte (niederes Nibble = linker Pixel). 8bpp -> flags 0x09, clut_len 524, crect (0,480,256,1), prect (0,0,64,256), 32768 Byte.

### Werkzeug 3 (FEHLT): Packer + TOC-Generator
Ein 'doc_pack.py' muss die Einzel-TIMs wieder zu einem Container mit 0x800-Slot-Raster zusammenlegen UND die drei Tabellen als .inc erzeugen (Slot-Records, doc->first_slot, doc-Record {max_page, 256-H}).

**Beleg:** Slot-Regel gemessen (Slot 0: 0x0 + 33312 + 18496 = 0xCA60 -> naechster Slot 0xD000); Tabellenformate wie oben belegt

**Umsetzung:** Muster: re15_port/tools/gen_re2_ems_toc.py (erzeugt engine/src/gen/re2_ems_toc.inc). Ergebnis: engine/src/gen/re2_filedoc_toc.inc.

### Werkzeug 4 (optional): Textsatz
Die RE2-Seiten sind fertig gesetzter Text als Pixel (Schrift ca. 8x16, Grossbuchstaben, Zeilenabstand 16, linksbuendig ab x=0 bzw. zentrierte Titel). Fuer eigene Dokumente ist ein Textsetzer sinnvoll, der Klartext -> 256xH-PNG rendert.

**Beleg:** Sichtprobe der gerenderten Seiten (z.B. FILES_0200.bmp: 'A gas light in the press room.' / mehrzeilige Absaetze mit konstantem Zeilenabstand)

**Umsetzung:** Entweder eigene Bitmapschrift, oder — byte-true guenstiger — die RE1.5-Textebene weiterverwenden und nur die Illustration als Bild einspeisen.

### BMP-Querverweis
Die 216 bereits im Repo liegenden FILES_NNNN.bmp entsprechen 1:1 den 216 TIMs in Reihenfolge: NNNN = 14 + laufender TIM-Index (0-basiert). Also FILES_0014.bmp = erster TIM (doc0 Illustration), FILES_0229.bmp = letzter (doc24 Seite 15).

**Beleg:** 216 Dateien FILES_0014..FILES_0229; Groessenhistogramm 16x86070 (128x256), 28x98358, 26x110646 (256x144), 146x135222 (256x176) — deckt sich mit dem TIM-Kettenlauf; Stichprobe FILES_0194 (Index 180 = doc23 Illustration) ist 98358 B = 128x256

**Umsetzung:** Fuer Sichtpruefungen die vorhandenen BMPs nutzen statt neu zu dekodieren.

## Offen
- VRAM-Upload nicht lokalisiert: die LoadImage-/RectAngle-Stelle, die die beiden TIMs eines Slots ins VRAM legt, wurde nicht gefunden. Belegt sind nur die CLUT-Zielzeilen (489/8bpp, 490/4bpp) und die Sampling-Geometrie am Zeichner FUN 0x80075fd0. Fuer ein PSX-Target des Ports muss das noch RE't werden (Einstieg: CD-Fertig-Rueckruf zu jal 0x80012fb8 mit a0=166).
- Byte +7 jedes Slot-Records (0x800a94b4+slot*8+7) wird nach 0x800d531e geschrieben (@0x8006d500), die Werte sehen wie Fuellmuell aus (36, 98, 83, f7, 30, ... , einmal 00). Zweck ungeklaert — moeglicherweise unbenutzt.
- Wie die Rookie-Hinweisdatei (doc23/doc24) ihre Seiten 1..16 freischaltet, ist ungeklaert. Statisch gefunden wurde nur EIN Schreibzugriff auf 0x800aa1a0 (Wert 1, @0x80068368). 14+14 Slots bleiben ueber die statischen Tabellen unerreichbar; ebenso 2 Slots von doc09.
- Zuordnung Gegenstand/Aufsammel-Ereignis -> docid nicht ermittelt. Gefunden ist nur die Anhaengefunktion FUN 0x800692dc (a0 = docid); die Aufrufer wurden nicht verfolgt.
- Bildschirm-Ursprung der RE2-Leseansicht (x0/y0 der beiden SPRTs) steht in einer statischen Prim-Vorlage im RAM (~0x8019c100) und wurde nicht aufgeloest. Der Port muss ox/oy selbst festlegen: RE1.5-Inventar ist 320x240 (inv_render_pc.c:47-48), eine 256+80=256 Zeilen hohe Komposition passt nicht ohne Verschiebung/Beschnitt — Entscheidung noetig.
- Der Endseiten-Marker-Sprite steht bei x=280, w=42 (@0x800762fc/0x800762dc), also teilweise ausserhalb eines 320 Pixel breiten Schirms. Entweder wird das Prim spaeter verschoben oder meine Feldzuordnung an dieser Stelle ist falsch — vor Uebernahme gegenpruefen.
- Grundsatzentscheidung fuer den Nutzer offen: RE1.5-Textebene BEIBEHALTEN und nur die RE2-Illustration einblenden (byte-true zu RE1.5, wenig Aufwand) ODER auf RE2-Bildseiten umstellen (eigene Texte muessen dann gerendert werden). Beide Wege sind mit dem obigen Datensatz baubar; ich habe keine Praeferenz vorgegriffen.
- Konturschrift der RE2-Seiten wurde nicht vermessen (Glyphenbreiten, Zeilenabstand, Randabstaende) — noetig nur, falls Werkzeug 4 (Textsetzer) gebaut wird.


---
