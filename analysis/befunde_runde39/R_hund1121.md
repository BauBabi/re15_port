# Runde 39 Spur R "hund1121" — Dossier (fortlaufend)

Baum: .claude/worktrees/r39_hund1121 (Zweig r39/hund1121, Basis 02dc7917 = master).
Nutzer-Punkt (AUFTRAG.md, Nachtrag 10, woertlich):
> "Und dann möchte ich, das in ROOM 1121, dort wo bei Leon die Krähe durch das Fenster fliegt, beim Durchlauf ein Hunde
> durch das Fenster rein springt."

Lesart Orchestrator (verbindlich): Ausloeser = Durchlauf ROOM1121 in der Elza-Panikphase (Bank-9-Bit 97, gesetzt NUR von
Spur P), einmal; Ort = Fenster von Leons Kraehen-Ereignis in ROOM1120 (Geometrie in 1121 belegen); danach normaler
Hunde-Gegner (Typ 0x20). Leon unveraendert, ohne Bit 97 kein Hund.

## Stand / Schritte
- [x] 1 Leons Kraehen-Ereignis 1120 belegt (R1) und am Bau gemessen (M1)
- [x] 2 Geometrie 1121: dasselbe RDT und Fenster, Wandzellen je Band (R5), Kamera Cut 1 (R5, N3)
- [x] 3 RE2-Form eines Hunde-Sprungs: ROOM21A0 sub07 + Modul Sub 14 (R2/R3), RE1.5-Gegenstueck (R4)
- [x] 4 Umsetzung (U1-U4)
- [x] 5 Messung am echten Weg mit Elza (CONTINUE + Tuer-Weg) + Riegel unit/integration (N1-N4)
- [~] 6 gezielte Tests gruen (T1); alle unit_/prop_/probe_ laufen (T2)

## Messung vorher

### M1 Leons Ereignis ROOM1120 am Bau 02dc7917 (`ctest -R "r35_fenster|r36_elza_fenster"`)
```
[fenster] ROOM1120 scharf: Slot 4 Band x 3500..6650 z 4300..6400 Ereignis 24
[fenster] Kraehe Slot 4 versteckt bei (5400,-2500,12100)
[fenster] Ausloeser: Spieler (5000,4325) im Band, Ereignis 24 0
[fenster] Splitter Bank 0x10 Platz 95 Lage x 4700 z 10200   ... 13 Splitter
[fenster] Knall 1: Raumbank-Satz 0x21 (RE2 Se_on 0x02210001) T+5 0
[fenster] Knall 2: Raumbank-Satz 0x21 (RE2 Se_on 0x02210001) T+10 0
[fenster] Zeitlinie fertig T+23 Splitter 13 Knalle 2 0
r35_fenster[A]: Schaden belegt in Bild(ern) 150;180;240;270;300;330
r36_elza_fenster: OK — 4 Kraehen (Slot 3 bei (5327,0,10217)), Schaden 309/309 Pixel nach Formel
100% tests passed, 0 tests failed out of 8
```
Kamera = Cut 1 (Gameplay-Kamera, kein Cut_chg im Ereignis); Glas-Ton = RE2-Raumbank room1090 Satz 0x21, zweimal.

### M2 Elza ROOM1121 mit (9,97) = 1 am Bau 02dc7917 (Spielstand + CONTINUE, Elza laeuft durchs Band)
`[spawn-diag]` nur die vier Kraehen (main00 Slot 0..2 und die Elza-Kraehe Slot 3 bei (5327,0,10217)), KEIN Typ 0x20, kein
Ereignis; `[fenster] ROOM1121 (Elza): Fenster kaputt, Kraehe Slot 3 Kill-Flag 0xAC`. Ausgangsstand: kein Hund.

## RE-Belege

### R1 Leons Kraehen-Ereignis ROOM1120 (Runde 35 M, im Code nachgelesen: engine/src/fenster_1120.c, include/re15_fenster1120.h)
* Ausloeser: AOT-Slot 4 AUTO (sce 3, sat 0x41, p0 0x00FF, p1 (24<<8)|0x18 = Satzform ROOM1050 @0x0C22..0x0C35),
  Band x 3500..6650 / z 4300..6400 quer ueber den Cut-1-Gang, Band 0; Tor (9,73)=1 und (9,79)=0; einmalig ueber (9,79).
* Fenster: Rueckwand z 11200 (SCA-Zelle (-2700,11200,11350,2000)); Rueckprojektion Cut 1 (Kamera fov 26684,
  P (5919,-2928,-1645), T (4280,-1744,5199)): Rahmen x 4084..5929 / y -3942..-1827, Scheiben x 4221..5808 /
  y -3049..-1953, Mitte (5021,-2511). Bildlage 320x240: Rahmen x 180..210 / y 68..103.
* Kraehe: versteckt bei (5400,-2500,12100) dir 0x0418 (= RE2 room1090 sub09 @0x0000 Kraehe 0, 180-Grad-Abbildung),
  frei bei T+0 (Member_set(0x17,4) = +0x1D4, RE2 sub15 @0x0012..0x002A).
* Glas: T+2 Rumble (sub15 @0x003A/@0x0040/@0x0046) + 13 RE2-Splitter (sub15 @0x0054..0x01D4, Banken 0x10..0x13 der
  Raum-ESP GLAS1090.ESP), Knall T+5 und T+10 = RE2-Raumbank Satz 0x21 (sub15 @0x0208/@0x0218, `36 02 21 01 ...`,
  EDT @0x84 `00 00 7c 60`), Hintergrund-Schaden Cut 1 ab T+2 (gen/fenster1120_schaden.inc).
* ROOM1121 (Elza, Runde 36 L): RDT bytegleich ROOM1120, Fenster IMMER beschaedigt, kein Ereignis, vierte Kraehe
  (Slot 3, Kill 0xAC) bei (5327,0,10217) ueber Port-Ereignis 26.

### R2 RE2-Form eines Hunde-Sprungs durch ein Hindernis (Beta -> Retail)
Zensus aller 495 RE2-Leon-RDTs (Sce_em_set 0x44 Typ 0x20, opcode-exakter Walk tools/re2_sicherung/re2_scd_walk.py
rdt_blocks): Hunde nur in Stage 2/3 + D/F (ROOM20F0/2110/2120/2160/21A0/3050/D070/D100/F080), KEINER in Stage 1.
Zensus Member_set (0x34) auf +4/+5/+6/+7/+0x10E: der einzige Hunde-Sprung per Skript ist **RE2 ROOM21A0** (Gasse
mit Kanalschacht):
```
sub07 @RDT+0x020F6  37 11 00 00              Sca_id_set(0x11,0)   Hindernis-Zelle aus
      @0x020FA      2e 03 00                 Work_set(Gegner 0)
      @0x020FE      34 02 01 00              Member_set(+4 = 1)   ACTIVE
      @0x02102      34 03 0e 00              Member_set(+5 = 14)  Sub 14 = Sprung A
      @0x02106      34 04 00 00              Member_set(+6 = 0)
      @0x0210A      34 05 00 00              Member_set(+7 = 0)   Phase 0 = WARTEN
      @0x0210E      32 00 d2 eb 00 00 a6 d5  Pos_set (-5166,0,-10842)
      @0x02116      33 00 00 00 00 08 00 00  Dir_set 0x0800
      @0x0211E      02                       Evt_next (ein Bild)
      @0x0211F      2e 03 00                 Work_set(Gegner 0)
      @0x02122      34 05 01 00              Member_set(+7 = 1)   FREIGABE
      @0x02126      36 03 13 00 03 00 ..     Se_on Bank 3 (ENEMSE) Satz 0x13 am Gegner 0
      @0x02132      04 ff 18 09              Evt_exec sub09 (Hindernis faellt)
      @0x02136      38 05 01                 Flr_set(5,1)
sub09 @0x0218E      09 / 0a 0a 00            Sleep 10
      @0x02192..    2e 04 01 / 32 .. / 33 .. Objekt 1 (Hindernis) Lage, Evt_next
      @0x021A8      36 02 0b 00 00 00 22 e1 f8 f8 00 d4   Se_on Raumbank Satz 0x0b am Hindernis (-7902,-1800,-11264)
      @0x021B4/BA   8a 00 07 .. / 8b ff 05 ..             Rumble
      ab @0x021C0   For-Schleifen = das Hindernis kippt in Sprungrichtung (Lage -7902 -> -11712, Drehung)
sub11 @0x023CC      34 05 01 00              Member_set(+7 = 1) fuer Gegner 1 (Spawn +0x10E = 8 -> Sub 15)
```
Se_on @0x02126: Bank 3 = ENEMSE-Slot 0x800dbb84 (FUN_8005ba28 `srl t1,a0,24` @0x8005ba30, EDT-Zeiger
`lw a1,-17544(at)` @0x8005ba8c), Bezug 3 = Gegner-Tabelle 0x800cfe1c[rec+5] (Se_on @0x800564a8-b8). In der
Hunde-Bank 6 ist Satz 0x13 NICHT belegt (selbst dekodiert, ENEMSE.VBS Bank 6 EDT @0x075800: live nur
0..8,10,11,12) -> der Ruf ist im Original stumm, er wird nicht nachgebildet.

### R3 RE2-Hundemodul EMD0G_MOD0.BIN (selbst disassembliert, re2_disasm.py --bin EMD0G_MOD0.BIN)
* Spawn-Tabelle @0x80105458 = `00 01 02 04 0a 0c 0e 0d 0f` (+0x10E&0x7F -> +5, `lbu v0,21592(at)` @0x80100320,
  `sb v0,5(s0)` @0x80100328): Spawn 6 -> Sub 14, 8 -> Sub 15. ACTIVE-Tabelle @0x80105464: [14] = 0x80102C28,
  [15] = 0x80102E58. Phasen Sub 14 @0x80105514 = {0x80102C78, 0x80102D98, 0x80102E0C}.
* Startmaschine 0x80102C78, Phase 0 @0x80102CD4-F4: `sb zero,537` (+0x219=0), `sb zero,557` (+0x22D=0),
  +0x1D3 |= 0x80, +0x1C0 |= 2 — **KEIN +7-Schreiber: Phase 0 WARTET** auf Member_set(+7 = 1) (Setter
  `sb a2,7(a0)` @0x80055D00, Member_set-Tabelle 0x80011228[5]). Sub 13 betritt Sub 15 deshalb mit +7 = 1
  (`sh 256,6` @0x80102BAC).
  ⛔ Port-Abweichung gefunden: re2d_windowjump_launch case 0 setzte +7 = 1 selbst (Phase 0 lief nach einem Bild
  weiter). Bisher folgenlos: Sub 14/15 hatten im Port keinen Produzenten (RE1.5-Raeume liefern Spawn 0).
* Phase 1 @0x80102CF8-D20: +7 = 2 (`sb v0,7(s0)` Delay-Slot @0x80102D0C), 0x80101380(65, -320), +0x1C0 &= 0xFD,
  dann **FAELLT IN DEN PHASE-2-RUMPF** (@0x80102D24 move, @0x80102D38 advance, @0x80102D40-54 +0x158--) im
  selben Bild. ⛔ Port-Abweichung: case 1 brach nach dem Start ab (Anlauf ein Bild spaeter, 65 Einheiten kuerzer).
* Startfeld 0x80101380: +0x158 = 8 (@0x80101398), +0x144 = a1, +0x146 = a2, Clip-Wort 0x70014 (Clip 20, @0x801013A8),
  +0x21E/+0x21F = 0, word0 = (word0 & 0xEFFFFFFF) | 0x4000000.
* Phase-2-Ende @0x80102D5C-78: +6 = 1, +0x144 = 280, +0x219 = 1, SE 3 (`jal 0x8005bd6c` @0x80102D74).
* Flug 0x80102D98: +0x22C = 1 (`sb v0,556(s0)` Delay @0x80102DC0 = kein Biss im Flug, Sonde 0x80104FF0), Kern
  0x80101488: 0x80015350 (3D-Schritt), +0x146 += 40 (@0x801014B8-C8), Ende sobald vy >= 0 ohne Kontakt
  (@0x801014D8-EC); dann Clip 21 (0xF0015 @0x80102DC8-CC), +6 = 2.
* Landung 0x80102E0C -> 0x80101574: Fall mit +40/Bild bis y >= +0x1C2, dann y = Boden, +0x219 = 0, SE 10
  (@0x801015FC); Rampe 64/240; danach @0x80102E28-40: +5 = 2 (RUN), +6/+7 = 0, +0x21D = 0, +0x22C = 0,
  +0x1D3 &= 0x7F -> normaler Hunde-Gegner.
* 0x80015350 (RE2 PSX.EXE): Drehmatrix aus (0, +0x76+a1, +0x78+a2) und (+0x144,+0x146,+0x148) auf x/y/z
  (@0x80015388-0x800153D8) = Port re2d_move3d.

### R4 RE1.5-Gegenstueck (reiner RE1.5-KI-Modus; Standard ist RE2, MIXED legt 0x20 ebenfalls auf RE2)
* RE1.5-Hunde-INIT STAGE1.BIN @0x8010db88-98: `lbu v1,9(a0)` / `ori v0,zero,0x40` / `bne` / `sw v0,4(a0)` (v0 = 4) ->
  grid 0x40 = Zustand 4 Sub 0 = Skript-Sprung, wartet; Tor @0x801113e4-ec `lbu v1,9(a0)` / `ori v0,zero,0x43` / `bne`.
* Freigabe im Original: ROOM1190 sub10 @0x027E6 `34 0c 43 00` (Member_set 0x0C = grid, 0x43) nach `2e 02 00` (Work_set).
* Sprung 0x80111398 (Clip 0x14 + Se(5), Steigen ab Bild 12, Landeclip 0x15, Ausgang Wort 0x201 @0x8011162c) = im Port
  re15_dog_state456 (enemy_ai_common.c); laeuft auch im RE2-Modus fuer die Raum-Skript-Hunde von ROOM1190.

### R5 Geometrie ROOM1121 (= ROOM1120.RDT bytegleich, Runde 36 L)
* SCA @0x2FC: Kopf `50 da 0e cb`, 5 Regionen zu je 39 Zellen (`27 00 00 00` x 5), 195 Zellen, Baender {0,1,2} zu je 65.
  Die Rueckwand steht in ALLEN drei Baendern; Band 1 = Region r Index 15 = `56 2c d0 07 74 f5 c0 2b 01 ff 00 13`
  (w 11350, d 2000, x -2700, z 11200, Typ 1, u0 0xFF, floor 0x13) @0x003C8 / @0x0059C / @0x00770 / @0x00944 / @0x00B18.
* Band eines Gegners = -(y / 0x708) (re15_collision_band_from_y); die Klemme prueft strikt band == floor>>4
  (re15_collision.c collision_constrain_impl). Der Hund auf der Bruestung (y -1827) ist Band 1.
* RE1.5-Satzform der Zellen-Freigabe: ROOM1130 sub02 @0x00AC8..@0x00AD8 `37 00 0a 00` .. `37 04 0a 00` (je Region ein
  Sca_id_set; Handler LAB_8004175c: u0 := Wert, Bit 0 = fest; Port op_sca_id_set in scd_vm.c).
* Kamera: Cut 0 an der Tuer-0-Ankunft (150,0,9550) (ROOM1061 main00 @0x00D12 Door_aot_set Slot 0 -> Raum 0x12 Cut 0),
  RVD-Wechsel auf Cut 1 im hinteren Quergang (gemessen x ~4185), Cut 1 = Gang nach Norden auf das Fenster
  (Runde 35 M §1), Cut 2 suedlich davon (gemessen ab z ~1600).

### R6 Zone-7-Bit fuer den Hund (Tot-Bit + Einmal-Riegel)
Zensus aller 144 RDTs STAGE1-3 (scd_dump_room.py, 0 Abbrueche; Sce_em_set pc[7] + Ck/Set Bank 7): 80 Bits frei, u.a.
0xA0-0xA3, 0xAC, 0xAE, 0xAF. Im Port belegt: 0x28-0x2B, 0x2E-0x31, 0x4B-0x4F, 0x53, 0x54, 0x5A, 0x5B, 0x5F (Montage,
re15_irons_tod.h) und 0xAC (Elza-Kraehe Runde 36). Gewaehlt **0xAE**. Bank 9 hat keinen freien Port-Platz mehr
(VERTRAG §1.1: 69 C, 70 I, 94 J, 95 Q, 96 O, 97 P).

### R7 Ton
* Glas = RE2 room1090 Raumbank Satz 0x21 (shared_assets/RE2/GLAS1090.EDT/.VH/.VB, Runde 35) ueber re15_fenster1120_se_hook.
* Hund: SE 3 beim Absprung (@0x80102D74), SE 10 bei der Landung (@0x801015FC), Clip-Ereignis SE 8 (ACTIVE-Schwanz
  @0x80100510-30) — ENEMSE-Bank 6 (RE2DOG_ENEMSE_BANK); alle drei belegt (selbst dekodiert, ENEMSE.VBS Bank 6 EDT
  @0x075800: id 3 = `034b0000`, 8 = `02990000`, 10 = `02b20000`).
* ROOM21A0 sub07 @0x02126 `36 03 13 00 03 00` = Bank 3 (ENEMSE) Satz 0x13: in Bank 6 nicht belegt -> im Original stumm.

## Umsetzung

### U1 Ereignis-Modul (NEU) engine/src/hund1121_r39.c + include/re15_hund1121_r39.h
| Konstante | Wert | Beleg |
|---|---|---|
| RE15_R39R_RAUM | 0x1121 | Elza-Variante (FUN_800396fc @0x800397e4 / @0x800397ec) |
| RE15_R39R_PANIK_BANK / _BIT | 9 / 97 | VERTRAG §1.1 (Spur P setzt und loescht, R liest nur) |
| RE15_R39R_EREIGNIS | 30 | Port-Ereignis < 32 (scd_event_fire); belegt 2..6, 13, 20..29 |
| RE15_R39R_SLOT | 4 | derselbe Platz wie Leons Band in 1120 (main00 0..3) |
| Band | x 3500..6650, z 4300..6400, sce 3, sat 0x41, p0 0x00FF, p1 (30 << 8) + 0x18 | re15_fenster1120.h (ROOM1050 @0x0C22, RE2 room1090 sub03 @0x0014) |
| RE15_R39R_EM | 4 (Aktor 5) | main00 0..2, Elza-Kraehe 3 |
| RE15_R39R_KILL | 0xAE | R6 |
| RE15_R39R_X / _Z / _DIR | 5400 / 12100 / 0x0418 | Start der Fensterkraehe (RE2 room1090 sub09 Kraehe 0, abgebildet) — PORT-WAHL |
| RE15_R39R_Y | -1827 | Rahmen-Unterkante, Rueckprojektion Cut 1 (Runde 35 M §1) — PORT-WAHL |
| RE15_R39R_GLAS_Z | 11200 | Rueckwand (R5) |
| RE15_R39R_WAND_INDEX / _REGIONEN | 15 / 5 | R5 |
| RE15_R39R_RE2_STATE / _SUB / _LOS | 1 / 14 / 1 | ROOM21A0 sub07 @0x020FE / @0x02102 / @0x02122 |
| RE15_R39R_RE15_GRID / _LOS | 0x40 / 0x43 | @0x8010db88-98 / @0x801113e4-ec, ROOM1190 sub10 @0x027E6 |
| RE15_R39R_T_KNALL1 / 2 | 3 / 8 | room1090 sub15 Sleep 3 @0x0204, Sleep 5 @0x0214 |

Programm (Ereignis 30, 56 B, RE1.5-Opcodes in der vorhandenen VM):
```
37 00 0f 00  37 01 0f 00  37 02 0f 00  37 03 0f 00  37 04 0f 00    Sca_id_set Band-1-Rueckwand (Form ROOM1130 @0x00AC8)
44 04 20 00 00 00 00 ae 18 15 dd f8 44 2f 00 00 18 04 00 00        Sce_em_set Hund (Form ROOM1120 main00 @0x00D0C)
22 07 ae 01                                                         Set(7,0xAE,1) (Form ROOM1120 sub01 @0x00D6C)
46 04 00 00 00 00 00 00 00 00                                       Aot_reset(4) (LAB_80040738)
01 00                                                               Evt_end
```
(Im reinen RE1.5-KI-Modus ist pc[3] des Sce_em_set 0x40.) Der Takt vor der KI (game_step_common.c) fuehrt die Schritte:
Spawn-Bild INIT -> Bild 1 Umstellung wie ROOM21A0 sub07 (+4 = 1, +5 = 14, +6 = 0, +7 = 0) -> Bild 2 Freigabe (+7 = 1;
RE1.5: grid 0x43) -> Glas im ersten Bild, das den Hund diesseits z 11200 sieht (Rumble @0x003A/@0x0040/@0x0046, 13 Splitter
@0x0054..0x01D4) -> Knall +3 und +8 -> Wand wieder fest, sobald z + Radius < 11200 -> fertig, wenn der Hund den Sprung
verlassen hat (Sub 14 -> 2 @0x80102E2C bzw. Zustand 4 -> 0x201 @0x8011162c). Messschiene RE15_HUND1121_LOG=<datei>.

PORT-WAHLEN (Form jeweils belegt): Lage auf der Anfluglinie der Kraehe (der Nutzer: "dort wo bei Leon die Kraehe durch
das Fenster fliegt"), Hoehe auf der Bruestung (tiefster Punkt, von dem der RE2-Sprung die Scheiben trifft: Fuesse beim
Durchgang y -2147..-2427 innerhalb der Scheiben y -1953..-3049); Glasbruch im Bild des Durchgangs (room1090: Splitter
kommen, wenn die Kraehe an der Scheibe ist; ROOM21A0: das Hindernis kippt im Sprung, sub09 Sleep 10); Wand nach dem
Durchsprung wieder fest (die Band-1-Zelle deckt die ganze Rueckwand x -2700..8650 ab, nicht nur das Fenster); kein
Bodenschatten jenseits der Glasebene (er laege hinter der Wand am Wandfuss — wie bei der versteckten Fensterkraehe der
Runde 35).

### U1b Treffer auf der Bruestung (Absicherung des erhoehten Starts)
Wird der Hund getroffen, bevor er abspringt (er steht 0,6 s in der Scheibe), laeuft RE2s HURT/DEATH-P0 mit dem Luft-Bit
+0x219 = 0 (Phase 0 loescht es, `sb zero,537` @0x80102CDC) in den BODEN-Zweig — der Hund bliebe auf Bruestungshoehe
(RE2s Daten stellen keinen Hund erhoeht auf). Der Takt setzt deshalb fuer einen Treffer ueber dem Boden (y < +0x1C2)
+0x219 = 1, und RE2s eigener Luft-Treffer-Zweig (@0x8010341C-28, P1 0x801034C8: Fall +40/Bild bis +0x1C2 @0x80103504)
bringt ihn herunter (gemessen: Treffer in T4, Boden nach 22 Bildern, Bildschritt <= 29) — PORT-WAHL nach der RE1.5-Regel "ueber dem Boden = in der Luft" (Hunde-HURT/DEATH-Dispatcher
@0x80110928-2c, der RE1.5-Hund faellt dort von selbst). Ohne Glasbruch abgebrochen, Wand wieder fest
(unit_r39_hund1121_treffer).

### U2 RE2-Startmaschine (enemy_ai_re2_dog.c, re2d_windowjump_launch, Kommentar "Runde 39 Spur R")
* Phase 0: die Port-Zeile `e->sub_state_3 = 1` entfernt — das Original hat dort KEINEN +7-Schreiber (@0x80102CD4-F4); die
  Phase wartet auf Member_set(+7 = 1) (Setter @0x80055D00).
* Phase 1: faellt in den Phase-2-Rumpf (@0x80102D20 -> @0x80102D24: Schritt, Advance, +0x158-- im selben Bild).
* Wirkung sonst keine: Sub 14/15 hatten im Port keinen Produzenten (RE1.5-Spawns liefern Spawn-Wort 0; Sub 15 nur aus
  Sub 13, das nur aus Sub 15 bzw. Spawn 7 kommt).
* NICHT modelliert: +0x1C0 |= 2 / &= 0xFD (@0x80102CE8 / @0x80102D18) — Leser nur in der Kopf-/Part-Verfolgung
  0x80017xxx, die der Port fuer den Hund dokumentiert offen fuehrt (0x80104B98 in enemy_ai_re2_dog.c).

### U3 Haken in gemeinsamen Dateien (je 1-3 Zeilen, Kommentar "Runde 39 Spur R")
scd_vm.c (Ereignis-Weiche + include), scd_room_setup.c (Installer am Blockende + include), platform/pc/main.c (Installer am
Boot-/CONTINUE-Weg, Schatten aus jenseits der Glasebene, include), game_step_common.c (Takt vor der KI + include),
enemy_ai_re2_dog.c (U2).

### U4 Modell im Raum
Kein eigener Lader noetig: der Original-Lader FUN_80022300 laedt beim Sce_em_set (`jal 0x80022300` @0x80042328), der Port
in der Bildschleife je aktivem Aktor (pc_enemy_load_ex, platform/pc/main.c ~7633) — im RE2-Modus die RE2-Bank EM020 aus
shared_assets/RE2/CDEMD0.EMS (Clip 20 = Anlauf, Clip 21 = Landung) mit RE1.5-Netz (Hybrid) und ENEMSE-Bank 6. Das INIT im
Spawn-Bild braucht die Bank nicht (re2d_clip schreibt nur Felder), ab Bild 1 ist sie geladen (Lauf A: Clip 20 ab F146).

## Messung nachher

### N1 Die erste Messung zeigte den Wandschub (Grund fuer R5 / Sca_id_set)
Probe ohne Zellen-Freigabe: `F79 Hund Slot 5 gesetzt bei (5400,-1827,12100)`, im Spawn-Bild auf (5400,10682) geschoben
(= 11200 - Radius, Klemme in Band 1). Mit Sca_id_set: `Freigabe ... Hund (5400,-1827,12100)` — er wartet auf der Bruestung.

### N2 Unit (test_r39_hund1121, Ausgabe R_belege/R_hund1121_unit.txt)
```
T  0 ( 5400, -1827, 12100) st1/ 0/0/0 clip 1             INIT
T  1 ( 5400, -1827, 12100) st1/14/0/0                    ROOM21A0 sub07 @0x020FE
T  2 ( 5397, -1827, 12036) st1/14/0/2 clip20 v 65        Freigabe @0x02122, Schritt im selben Bild (@0x80102D24)
T  3                                        SE 8         Clip-20-Ereignis (@0x80100510-30)
T  9 ( 5376, -1827, 11588) st1/14/1/0 clip20 v280 SE 3   Absprung nach 8 Anlaufbildern (@0x80101398, @0x80102D74)
T 10 ( 5365, -2147, 11309) .. T17 ( 5288, -3267, 9356) clip21   Steigflug -320/+40, Scheitel y0 - 1440
T 31 ( 5134,     0,  5450) st1/14/2/1 SE 10              Landung (@0x801015FC)
T 43 st1/ 2/0/0, T44 fertig                               Sub 2 = RUN (@0x80102E2C)
Glas T12 (Durchgang nach T11), Splitter 13, Knalle 2 Satz 0x21 (T15 / T20)
RE1.5: Zustand 4 1, Freigabe T2, Ausgang T31, Glas T18, Knalle 2, Zustand 1 Sub 2
Startmaschine: 60 Bilder Phase 0 ohne Bewegung; Freigabe-Bild +7 2 Clip 20 Speed 65 +0x158 7 z -65; Absprung nach 7 weiteren
```

### N3 Echter Weg, Elza, Spielstand + CONTINUE (integration_r39_hund1121 Lauf A; R_belege/R_hund1121_echtweg_hund.log)
Elza an der Tuer-0-Ankunft (150,0,9550) Gier 0, Weg `XU1.5,R0.35,XU2.5` (nach Osten, Drehung, den Gang nach Sueden):
```
F143 Ausloeser: Spieler (6020,6207) im Band, Ereignis 30
F144 Hund Slot 5 gesetzt bei (5400,-1827,12100) RE2-Hirn 1
F145 RE2: Sub 14 Phase 0       F146 Freigabe (RE2=1), Takt 2, Hund (5400,-1827,12100)
F156 Glas: Hund bei (5354,-2427,11030) erreicht die Glasebene z 11200, 13 Splitter
F158 Wand Band 1 wieder zu     F159 Knall 1     F164 Knall 2     F188 fertig T44: Hund (5024,2542) Zustand 1 Sub 2
```
Mitschnitt (RE15_AUDIO_CAP_SYNC, je 6 Bilder ab dem Knall-Bild): Lauf A Spitze 26325 (F160) und 29813 (F167), Median 1708;
derselbe Weg ohne (9,97) (Lauf B) 5742 und 2352, Median 857. Bild F146 (Wartebild) A gegen B: rechte Scheibe (x 196..207,
y 80..101) 93 verschiedene Pixel, linke Scheibe 0. Sichtpruefung (R_belege/R_hund1121_sprung_cut1.png und
R_hund1121_fenster_ausschnitt.png): F144-F154 steht der Hund in der rechten Scheibe, F156 springt er hoch durch das
Fenster, die RE2-Splitter spritzen, F158-F164 fliegt er ueber den Gang (Kraehen daneben), ab F168 Cut 2 (Elza ist
weitergerannt); danach greift er an (Tuer-Lauf E: Elza hp 100 -> 80 -> 75).

### N4 Gegenlaeufe und Tuer-Weg (integration_r39_hund1121 Laeufe B-E) und RE1.5-KI
* B ohne (9,97): `ROOM1121 aus: (9,97)=0 (7,0xAE)=0`, kein Typ 0x20.
* C (9,97) + (7,0xAE): `ROOM1121 aus: (9,97)=1 (7,0xAE)=1`, kein Typ 0x20.
* D Leon ROOM1120 mit (9,97) + (9,73): Leons Kraehen-Ereignis (`ROOM1120 scharf`, `Ausloeser`), kein Hund, kein hund.log.
* E Elza ROOM1061 -> die echte Tuer Slot 0 (RE15_FIRE_AOT, `[tuer] Sequenz ... DOOR07`) -> ROOM1121 (Installer am
  Tuer-Weg scd_room_setup.c, `F40 ROOM1121 scharf`) -> derselbe Weg: `F271 Ausloeser` .. `F316 fertig ... Zustand 1 Sub 2`.
* RE1.5-KI live (RE15_AI_FLAVOR=1.5): `behavior=0x40`, `RE1.5: Zustand 4 Sub 0 wartet auf grid 0x43`, Glas F163 bei
  y -1907, zwei Knalle, `fertig T32 ... Zustand 1 Sub 2`.

## Tests
| Test | misst |
|---|---|
| unit_r39_hund1121_ereignis | Tor (9,97) / (7,0xAE) / Raum, Slot 4 AUTO 30 sat 0x41 auf Leons Band, 56 Programm-Bytes, AOT-Scan -> VM: Wand Band 1 u0 0, Hund (5400,-1827,12100) 0x418 Kill 0xAE, (7,0xAE) = 1, Slot 4 sce 0; Leon 1120 ohne Hund |
| unit_r39_hund1121_sprung | RE2-Hirn + echte RE2-Bank: Warten ohne Wandschub, Freigabe-Schritt, Absprung T9 SE 3, Steigflug gegen unabhaengige Rechnung, Scheitel, Clip 21, Glas im Durchgangsbild, 13 Splitter, Knall +3/+8 Satz 0x21, Schatten aus nur jenseits z 11200, Wand wieder fest, Landung SE 10, danach Sub 2 |
| unit_r39_hund1121_re15 | RE1.5-KI: grid 0x40 -> Zustand 4, Freigabe 0x43 in T2, Ausgang Zustand 1, Glas + 2 Knalle |
| unit_r39_hund1121_einmal | Wiedereintritt ohne Ereignis; ohne (9,97) ohne Ereignis |
| unit_r39_hund1121_treffer | Treffer auf der Bruestung vor dem Absprung: Luft-Bit, Fall auf y 0, kein Glas/Knall, Wand fest |
| unit_r39_hund1121_startmaschine | RE2 Sub 14 direkt: Phase 0 wartet 60 Bilder, Phase 1 faellt in den Rumpf, Absprung nach 8 Anlaufbildern |
| integration_r39_hund1121 | echte exe, Elza: Lauf A (CONTINUE: Kette, Mitschnitt, Bild), B (ohne 97), C (0xAE gesetzt), D (Leon 1120), E (Tuer-Weg 1061 -> 1121) |

### T1 Gezielte Tests (Endstand, Bau 87307d18; Log R_belege/R_hund1121_ctest_gezielt.txt)
`ctest --test-dir re15_port/build -R "r39_hund1121|dog|hund|r35_fenster|r36_elza_fenster|1190|crow|kraehe|aot_edge|scd_aot_opcodes|aot_sce_census|r37_elzaems" --timeout 600 --output-on-failure`
(Hunde-KI RE1.5/RE2, Kraehen, Leons Fenster 1120 + Elza-Fenster 1121, ROOM1190-Hunde, AOT/SCD-Opcodes, Elza-EMS-Lader,
eigene Riegel):
```
100% tests passed, 0 tests failed out of 77
Total Test time (real) = 891.69 sec
```
integration_r39_hund1121 darin 144 s (Laeufe A-E).

### T2 Alle unit_/prop_/probe_ (VERTRAG/Auftrag: vor dem Abschluss)
(folgt)

## OFFEN
* Integration mit Spur P (setzt/loescht (9,97)): hier per Spielstand-Flag gemessen; im Spielablauf kommt das Bit aus P.
* Zone-7-Bit 0xAE, Ereignis 30 und AOT-Slot 4 in ROOM1121 bei der Integration gegen Spur Q pruefen (Q baut Zombie-
  Durchbrueche mit eigenen Spawns; falls Q ebenfalls 0xAE belegt: Zensus R6 liefert weitere freie Bits 0xA0-0xA3, 0xAF).
* +0x1C0 Bit 2 (@0x80102CE8 / @0x80102D18) nicht modelliert — Leser nur in der offenen Kopf-/Part-Verfolgung (U2).
* PSX-Ziel: Ereignis, Hund, Zeitlinie laufen in der Engine; Splitter-Zeichner und Knall sind PC-Plattform
  (glas1120_pc.c, wie Runde 35 M).
* Treffer auf der Bruestung (nur moeglich, wenn Elza beim ERSTEN Durchgang nach Norden zum Fenster blickt und in den
  0,33 s zwischen Freigabe und Absprung schiesst — der Schuss-Streifen FUN_80012574 ist unbegrenzt lang): der Luft-
  Treffer stoesst den Hund vom Schuetzen weg, also nach draussen; er faellt hinter der Wand auf y 0 und wird von der
  Band-0-Klemme in den Raum geschoben (gemessen z 11415 -> 10682 im Bild nach der Landung). Bewusst so gelassen: kein
  Original kennt einen erhoehten Hund; Alternativen (Hund draussen entfernen / unverwundbar machen) haetten keinen Beleg.
* Verhalten nach dem Sprung: einmalig wie Leons Fensterkraehe der Runde 35 — verlaesst Elza ROOM1121, ohne den Hund zu
  erlegen, ist er beim naechsten Betreten nicht mehr da (kein Raum-Skript spawnt ihn; das Zone-7-Bit sperrt das Ereignis).

## Fuer den Nutzer
* Ablauf: Elza in der Panikphase (nach Johns Ruf in ROOM1041, Spur P) durch ROOM1121 — im langen Gang (Kamera Richtung
  Fenster) springt beim Durchlaufen des Querbands ein Hund durch das schon zerbrochene Fenster: er steht kurz in der
  rechten Scheibe, setzt an, springt mit RE2-Glassplittern und zwei Glasknallen (wie Leons Kraehe) in den Gang, landet
  und greift als normaler Hund an. Nur einmal; ohne Panikphase kein Hund; Leons ROOM1120 unveraendert.
* Neue Assets fuer das Paket-Gate: KEINE (benutzt shared_assets/RE2/GLAS1090.* der Runde 35, CDEMD0.EMS, ENEMSE.VBS).
* Sprachdateien: keine.
* Messhaken: RE15_HUND1121_LOG=<datei> (Ereigniskette neben der exe).
