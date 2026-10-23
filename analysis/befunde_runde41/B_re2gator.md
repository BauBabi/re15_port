# Runde 41 — Spur B "re2gator" — Dossier (fortlaufend)

Baum: `.claude/worktrees/r41_re2gator` (Zweig `r41/re2gator`, Basis master 116e8b0f).
Auftrag (AUFTRAG.md, Wortlaut): (1) RE2-Finisher + fehlende Animationen des Alligators extrahieren;
(2) Verfolgung wie RE2 (Animation, Schreie, Beschleunigung an gleichen Stellen), Waende wackeln + Putz
faellt, Klettern ueber die Plattform BLEIBT; (3) Kanister-Aufnahme + Sterbeanimation mit geplatztem
Kiefer GENAU wie RE2 (Mechanik mit Testhaken; die Szene baut Spur G).

Zeiten aus `date`. Jeder Schritt wird hier VOR dem Bau notiert.

## 0. Protokoll (neueste Eintraege oben)

* 2026-10-23 17:5x: RE-Belege (Abschnitt 2) vollstaendig, Kanister-Modell + TIM nach shared_assets/RE2
  geschnitten. Naechster Schritt: Modul gator_re2_r41.c (Zonen-Dash, Hysterese, Ereignis-Schuetteln, Rumble,
  Putz, Kanister T2/R1, HURT-Selektor RE2, HP-Sockel), Haken in enemy_ai_boss_gator.c, PC-Zeichner, Tests.
* 2026-10-23 16:59 (Start): Baum sauber (status --porcelain leer, HEAD 116e8b0f). Pflichtlektuere: AUFTRAG.md,
  VERTRAG.md, Memory reai-v2-gator-bosskampf.md, analysis/befunde_2026-09-19/gator-finisher-sounds.md
  (RE2-EM23: SE 4 Bruell @0x80100d84, Lunge Clip 2->3, Bissfenster Clip 3 f>=114, Stufe B2 vermessen und
  NICHT eingebaut, weil der Port-Lunge eine zielsuchende Hatz ist und RE2s Ansturm die Wurzelbewegung
  der Clips 2/3 ist), analysis/befunde_runde6_2026-09-12/gator-vollausbau.md (Zustandstabelle),
  befunde_runde8_2026-09-13/gator-schrei-schaden.md (SE-Zensus, ROOM40A0-Raumbank Saetze 14/15).

## 1. Messung vorher
(folgt: Clip-Nutzung des Port-Bosses in einem Testlauf, Schuetteln = keines, Kanister = keiner)

## 2. RE-Belege (alle Adressen selbst mit re2_disasm.py aus EM23_OVL_0000.BIN / info/re2leon/PSX.EXE gelesen)

### 2.1 Der RE2-Raum: ROOM40A0.RDT (info/re2leon/PL0/RDT), der einzige 0x23-Spawn mit Bank 0x16
Sce_em_set `44 00 00 23 00 00 01 16 00 00 4a 16 b8 0b 3c eb 00 04 00 00 00 00` @Datei 0x25A0 (SCD-init),
Gator bei (5706, 3000, -5316) dir 0x400. Korridor: Bein 1 entlang -X auf z = -21212 (Intro setzt z
@0x801008d0-d4), Bein 2 entlang +Z auf x = -23078 (FRESSEN-P0 @0x80101048). RE2 SCD-Opcode-Laengen aus
info/Resident_Evil_und_Playstation_Information/information73.txt (Walker: scratchpad/tools/re2_scd_walk.py).

### 2.2 Zustandstabelle (Root @0x80100190, Dispatch +0x4 @0x8010461c; ACTIVE Pre @0x8010463c / Main @0x8010465c)
- INIT @0x801004e0: HP = 25000 + Tabelle[0x800cfb74 & 0x40/0x20] = 25300 (`[0x801045bc]` = 300, @0x80100518-580).
- Sub 1 SCHWIMMEN: Pre @0x80100964, Main @0x80100c20 = `FUN_8001a330(em, Clip 0, 32, 0)` @0x80100c34-40.
  Zonen (+0x21A): 0->1 bei x < -17877 (`slti v0,v0,-17877` @0x80100998) -> Routine 0x301 LUNGE, +0x21C =
  -17877 - x (@0x801009b4-c4); 1->2 bei z >= -7322 (`slti v0,v0,-7322` @0x801009e0, Sprung bei NICHT-kleiner
  @0x801009e4-ec) -> 0x301, +0x21E = z + 7323 (@0x80100a00-08). (Runde-6-Dossier hatte das Vorzeichen der
  Zone-2-Linie falsch.)
  Hysterese: Sub 1 -> Sub 2 (0x201) wenn +0x1F0 >= 0x65 (`sltiu v0,v0,0x65` @0x80100b88/@0x80100ba4) und
  +0x238 (Biss-CD) == 0 (@0x80100b5c-64) bzw. sofort bei +0x22C != 0 (@0x80100b48-58); Sub 2 -> Sub 1 bei
  +0x1F0 < 0x5a (@0x80100be4-f0, @0x80100ca4-b0) oder +0x238 != 0 (@0x80100bf4-c04, @0x80100cb4-c4).
  +0x1F0 = EXE-Distanz Wurzel<->Spieler in Welteinheiten: `lw v0,56(s0) / lw v1,56(s2) / subu / mult ...
  jal 0x8008d2f4 (GTE-SquareRoot0) / sw v0,496(s0)` @0x800265a4-e0, fuer jede aktive Entity ohne +0x10E Bit 15
  (@0x80026590-9c); EM23 schreibt +0x10E nur in R1/R2/R3 (|= 0x4000).
- Sub 2 PADDELN Main @0x80100cdc: `FUN_8001a330(em, Clip 1, 4, 0)` @0x80100cf8-d04; Clip-Ende -> 0x201 Selbst-
  Reset @0x80100d08-10.
- Sub 3 LUNGE @0x80100d30: P0 `addiu a0,zero,4` @0x80100d64 + `jal 0x8005bd6c` @0x80100d84 (ENEMSE SE 4), P1
  Clip 2 (16) @0x80100dac-b4 (Doppelruf nur bei +0x22C != 0 @0x80100da4-dd0), +0x225 = 1 @0x80100e04-0c, P2
  Clip 3 (0) @0x80100e20-28; Maul offen `+0x218 |= 0x10` bei Zone 2 & Clip 3 & Frame >= 114 (@0x80100e64-a4);
  Ende: +0x225 = 0, `yaw += 1024` @0x80100eb0-c0, Zone 2 -> 0x701 + +0x22E = 255 (@0x80100ed0-e0) sonst 0x101.
  Gleitkorrektur +6/F auf x (+0x21C) bzw. -6/F auf z (+0x21E) jeden Tick @0x80100ee8-f34.
- Sub 0 INTRO @0x80100804: P0 wartet +0x1D4 != 0 (@0x80100854-68, Setzer = ROOM40A0 sub11 Member_set2 0x17);
  P1 `0x800cfbd8 |= 0x110` + `+0x218 |= 2` @0x8010086c-90, Cut 0 bei Frame 70 @0x80100894-a4, Clip 6 (0)
  @0x801008ac-b8, fertig -> Cut 1, z = -21212 @0x801008c4-e0; P2 Clip 3 @0x801008e4-f0, fertig ->
  Bits 0x110 frei @0x801008f8-918, +0x238 = 110 @0x8010091c-20, Routine 0x101, +0x218 &= ~2, yaw += 0x400
  @0x80100938-48, +0x218 |= 4.
- Der RE2-Gator STEUERT DEN KOERPER NIE: Root ruft nur FUN_80017d98(em, yaw) (Hals-Part +0x1C1 = 5, @0x8010036c-74)
  bzw. FUN_800177c0 im Zielmodus +0x1C0&4 (@0x80100344-60). Bewegung = Wurzelversatz der Clips (FUN_8001a330 ->
  FUN_80015e7c -> FUN_800152c8), Richtung = Yaw (+0x400-Korrekturen nach Clip 3).
- Stroke-Ereignisse: Frame-Wort mit Top-5-Bits 0x28 -> +0x227 = 1 (`lui v1,0x2800 / and v0,v0,s1(0xf8000000) /
  bne / addiu v0,zero,1 / sb v0,551(s0)` @0x801003f0-40c). EDD1-Woerter (selbst geparst, EM23.EMD @0xC):
  Clip 0 f76 0x28000045 / f162 0x28000095; Clip 1 f12 0x280000a2 / f25 0x280000af; Clip 2 f50 0x280000e2 /
  f110 0x2800011e; Clip 3 f50 0x48000178 / f77 0x48000193; Clip 5 f2 0x1800020b / f80 0x38000259; Clip 6 f6
  0x48000287 / f30 0x2800029f / f34 0x280002a3; Clip 7 f4/f45/f115 0x48.., f25/f36/f44 0x28..; Clip 8 f8/f63
  0x48.., f90 0x28..; Clip 4/9/10/11 ohne Woerter.
- Ereignis-Maschine FUN @0x80101d44 (Tabelle @0x80100044): [1] @0x80101d80 `FUN_800395b8(10,120,50,0)`
  (grosser Motor Rampe 120->50 ueber 10 F), +0x16A = 2, Ereignis 2; [2] @0x80101da8 Zaehler -1, bei 0 -> 3;
  [3] @0x80101dcc +0x16A = 5, `0x800cfbd8 |= 4`, -> 4; [4] @0x80101dfc Zaehler -1, bei 0: `FUN_800395b8
  (8,85,40,0)`, +0x16A = 10, `0x800cfbd8 = (&~4) | 8`, -> 5; [5] @0x80101e60 Zaehler -1, bei 0: `&= ~8`, Ereignis 0.

### 2.3 Das "Wackeln der Waende" = RE2-Bildschirm-Schuetteln (EXE)
FUN_8002aa28 @0x8002aa28: `lw v1,0x800cfbd8 / andi v0,v1,0x8 -> Modus 1 / andi v0,v1,0x4 -> Modus 2 / jal
0x8002aa6c`. FUN_8002aa6c: `DAT_800dfd6b = (DAT_800dfd6b + 1) & 0xf`, Paar (dx,dy) = Tabelle[Modus][idx]:
Modus 1 @0x8009daf4: 00 01 00 00 00 00 00 01 01 01 00 00 00 01 00 00 00 00 00 00 00 01 00 01 00 00 00 00 01 01 00 00
Modus 2 @0x8009db14: 00 02 00 00 00 02 00 03 01 00 00 01 00 02 00 00 00 02 00 00 02 01 00 02 00 02 00 00 01 01 00 02
Ohne Bit 2 in 0x800cfbd8: FUN_8002ae10(0,7) + 240x FUN_8002ac94(i, -dx, dy, bgY) + FUN_8002addc = DR_MOVE je
Hintergrundzeile: Ziel (0, bgY+i) <- Quelle (dx, bgY+i+dy, 320-dx, 1) -> der HINTERGRUND rueckt um dx nach
links und dy nach oben, das 3D bleibt stehen (RE2_Quellcode_V2/FUN_8002ac94.c). Die beiden Ringe
0x800eaad8/0x800eac28 (FUN_8003947c/80039514/800395b8) sind die Pad-AKTUATOREN (FUN_80038bbc -> DAT_800cbc20/21,
re15_rumble.h), KEIN Shake.

### 2.4 Putz: ROOM40A0 sub14 @SCD-main 0x086E..0x0914 (9x Sce_espr_on id 4)
`3a 00 04 10 00 00 00 0e 78 a1 54 f2 14 05 38 03` = id 4 (RE2-CORE00.ESP, Id-Liste `03 05 00 01 02 06 07 04`),
Typ 0x10/0x08/0x00, Param 0x0e00/0x1000/0x1000/0x1200/0x0800/0x2000/0x1400/0x2200/0x1e00, Lagen (-24200,-3500,
1300) (-23800,-2000,1400) (-25500,-3800,1300) (-23800,-2000,1400) (-24200,-4400,1350) (-25400,-4000,1300)
(-23800,-3500,1300) (-24200,-3000,1500) (-25000,-2400,1300), Richtung 824/474/1024/474/1124/974/1074/1524/1024,
Pausen Sleep 2 / Evt_next / Evt_next / Evt_next / Sleep 2 / Sleep 2 / Sleep 5. Aufruf: sub13 (@0x0850, Cut-
Tausch 6->13/7->14, Sleep 170) `For 6: Gosub sub14` — sub13 startet sub11 bei Kamera 9 (`Cmp var 0x1a == 9`
@0x0778, Evt_exec 0x0d @0x0786): Ende von Bein 2 (Decke -2000..-4400 ueber dem Steg y=0). Kein Sce_shake_on
(0x5C) in ROOM40A0.

### 2.5 Kanister (T2-Maschine FUN @0x801017c4, Tabelle @0x80100024; Sub-1-Pre)
- Raumflag 0x800d4874 Bit 1 (SCD sub10 `Set 22 05 01 01`, Kanister faellt) -> Zielmodus +0x1C0 |= 4 mit Ziel
  (eigene x, y-800, obj[1].z) @0x80100a1c-70 (nur Hals-Tracking); Bit 0 (`Set 22 05 00 01`, liegt im Wasser)
  + Fenster obj[1].z - z - 8601 in [0, 0x18f) (`addiu v0,v0,-8601 / sltiu v0,v0,0x18f` @0x80100ae4-ec) +
  +0x226 == 0 -> +0x226 = 1 (@0x80100b00-08), Sub 2 -> 0x101 (@0x80100b10-18). Zielmodus aus, sobald
  obj[1].z - z < 8880 und Stufe 0 (@0x80100a74-b4).
- obj[1] = 0x800d051c (Basis 0x800d0324, Schritt 504 = FUN_80053394 @0x800533c8-d0; Zuordnung per Belegung in
  Reihenfolge, Id-Tag +0x106 = FUN_80037eac) = die ZWEITE Obj_model_set von ROOM40A0 sub05: `2d 01 00 00 00 00 00
  00 00 00 0a 00 00 00 b8 ac f8 f8 cc db ...` @0x0452 = Modell 1 bei (-21320,-1800,-9268) (Bein-2-Wand).
  Modell 1 = MD1 @RDT 0x4024 (3444 B, 104 Vertices, Zylinder r 267 x 2784 hoch mit Hals) + TIM @0x2fcfc
  (33824 B, 8 Bit, 2 CLUTs; CLUT 1 = schwarzer Tank mit gelb-schwarzen Warnstreifen und Umbrella-Logo,
  Quad-Texturrecord `5c 80 40 78 6a 80 80 00 ...` -> clut 0x7840 = CLUT-Zeile 481) = DER GASKANISTER.
  Modell 0 (@0x3918) ist das 9x8-Wassergitter (y = 0, 1800er Raster), Modell 2 (@0x5388) die Gitterplatte.
  -> shared_assets/RE2/KANISTER40A0.MD1 (md5 ded422132cc885d9c6bf1e143bcbe265) + KANISTER40A0.TIM
  (md5 3b56b9667312cdae4bc4f97da076446b), byteidentisch aus dem RDT geschnitten.
  Nach dem Fall: sub05 `Pos_set (-22816,-1984,-9500)` + `Dir_set (0x690, 0, 0xfc00)` (@0x0484-0x0493, liegend).
- Stufen (+0x226, Ease-Deltas 0x80101bac ease-in: A*(t^2-(t-1)^2)/N^2, 0x80101c74 ease-out: A*((N-t+1)^2-(N-t)^2)/N^2;
  +0x22A = Kopf-Part 6 az, +0x228 = Kiefer-Part 7 az, Root @0x8010037c-cc):
  [1] @0x8010183c Raumflag Bit 29 (`jal 0x8007730c` a1=29), [2] @0x80101860 13 Schritte (+240, -320), Kanister
  obj[1].z >= eigene z + 8100 erzwungen (@0x801018a4-c0), bei t=13: obj[1]+0x80 = &Part-6-Weltmatrix (mw+1104
  @0x801018d0/e0), Lokallage ueber FUN_8002d030 (Transpose*..., @0x801018f0), [3] @0x80101970 6 Schritte (-140,-60),
  x -> 1750 Schritt 70, [4] @0x801019dc ease-out 6 (-140,-60), x -> 1750/145, y -> -210/25, [5] @0x80101a64 7
  (+75,+100), x 100, y 55, [6] @0x80101aec ease-out 7, [7] @0x80101b58 Halten x 1750/100, y -210/30.
- HURT-Selektor @0x80101eac: HP < 25000 -> +0x218 |= 0x20 (@0x80101ebc-dc); Spieler-HP < 0 -> R0; +0x226 == 7
  -> R1 (ausser +0x230 == 1, @0x80101ef4-f18); Maul offen & [0x800d4820] == 8 & HP < 25000 -> R2/R3; sonst
  +0x225 == 0 & +0x22E == 0 & rnd&3 == 0 -> R4 mit +0x22E = (rnd & 0x3f) + 120 (@0x80101f8c-fd4); sonst R0.
  HURT-Eintritt @0x80101ff0: +0x230 = alter Sub, Frame -= 2 (@0x80102030-48). R0 @0x80102090: SE 5 wenn
  +0x232/+0x233 == 0 (@0x801020ac-c8), Treffer-VFX, Routine-Restore. R4 @0x80102bd0: SE 0 (Gate +0x232 == 0),
  +0x1D3 |= 0x80, Clip 10 (Blend 3), fertig -> 0x101.
- R1 KANISTER-EXPLOSION @0x80102158: P0 @0x801021ec: ENEMSE SE 4; `0x800cfbd8 |= 0x100`; Nahkamera 5 nur fuer
  Waffe 6/13 & z < -13300 (@0x80102218-7c); `|= 4` (Schuetteln stark) @0x801022a8-b4; +0x226 = 13; +0x176 = 3678;
  +0x227 = 1 (Ereigniskette); +0x22A/+0x228/+0x224 = 0; +0x218 |= 2; +0x1D3 |= 0x80; Kanister geloest (+0x80 =
  Einheitsmatrix 0x8009db44, Weltlage aus Matrix-Translation @0x80102300-24); Rumble `0x8003947c(8,3)` +
  `0x800395b8(50,255,0,0)` @0x80102328-44; FUN_80104318(em, Part 7 @mw+1204) = Kiefer: Flags |= 0x4a, +0x9a = -10,
  +0xa0 = 0x8000, +0x70 = 0x202038 (Farbe R 0x38 G 0x20 B 0x20), +0x98 = yaw+0x800 (@0x80104318-58);
  Fontaenen-Callback FUN_80103eb4 (64 Ticks: Tick 1 `0x26032000` an Part 6, Ticks 2-8 `0x00064000` an Part 7 +
  `0x00034000` + 2x `0x08024000` an Part 6, Streuung (int8 rnd)*2/4 + 300/200 @0x80103f20-4028); Spritzring
  FUN @0x8010338c: 6x `FUN_8001bf10(0x030f2000|0x030f3000, 0, obj[1]+0x54, ofs_k)`, ofs-Tabelle @0x801000a8
  (0,-2800,0) (300,-2100,0) (0,-700,0) (0,0,300) (0,-1400,-200) (-500,-400,200); Raum-SE `Se_on(0x020f0001)`
  = ROOM40A0-Bank Satz 15 @0x8010237c-88; +0x16B = 9 -> Kanister nach 9 Bildern entfernt (@0x8010255c-78);
  Rueckstoss +0x144 = -500, Schritt [0x801048c0] = 40, je Bild -2 (@0x8010246c-84, @0x8010257c-a0), Bewegung
  `FUN_800152c8(em, 0x800)` @0x801025a4-ac (rueckwaerts); Ziel-Yaw yaw+160 mit 75/Bild (@0x8010239c-a4,
  @0x801025f8-608); P1 Clip 9 (`FUN_8001a330(em,9,0,0)` @0x80102488-98), bei Frame 31 `|= 4` + Ereignis 1
  (@0x8010252c-58); Clip-Ende -> P2 -> P3 @0x80102654: +0x10E |= 0x4000, Raumflag Bit 30, `0x800dfc1a = 2`,
  `0x800cfbd8 &= ~0x101` — Endzustand (Leiche haelt die Clip-9-Endpose).

## 3. Umsetzung
(folgt)

## 4. Messung nachher
(folgt)

## 5. Tests
(folgt)

## 6. OFFEN
(folgt)

## 7. Fuer den Nutzer
(folgt)
