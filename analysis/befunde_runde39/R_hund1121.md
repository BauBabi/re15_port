# Runde 39 Spur R "hund1121" — Dossier (fortlaufend)

Baum: .claude/worktrees/r39_hund1121 (Zweig r39/hund1121, Basis 02dc7917 = master).
Nutzer-Punkt (AUFTRAG.md, Nachtrag 10, woertlich):
> "Und dann möchte ich, das in ROOM 1121, dort wo bei Leon die Krähe durch das Fenster fliegt, beim Durchlauf ein Hunde
> durch das Fenster rein springt."

Lesart Orchestrator (verbindlich): Ausloeser = Durchlauf ROOM1121 in der Elza-Panikphase (Bank-9-Bit 97, gesetzt NUR von
Spur P), einmal; Ort = Fenster von Leons Kraehen-Ereignis in ROOM1120 (Geometrie in 1121 belegen); danach normaler
Hunde-Gegner (Typ 0x20). Leon unveraendert, ohne Bit 97 kein Hund.

## Stand / Schritte
- [~] 1 Leons Kraehen-Ereignis 1120 belegt (R1); Messung am Bau folgt
- [ ] 2 Geometrie 1121 (gleiches Fenster? Kameras, Kollision)
- [x] 3 RE2-Form eines Hunde-Sprungs: ROOM21A0 sub07 + Modul Sub 14 (R2/R3)
- [ ] 4 Umsetzung (Hund-EMD im Raum, Sprung-Clip, Glas-Ton, danach Gegner)
- [ ] 5 Messung am echten Weg mit Elza + Riegel unit/integration
- [ ] 6 gezielte Tests (ctest -R)

## Messung vorher
(folgt)

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


## Umsetzung
(folgt)

## Messung nachher
(folgt)

## Tests
(folgt)

## OFFEN
(folgt)

## Fuer den Nutzer
(folgt)
