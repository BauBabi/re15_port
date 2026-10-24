# Runde 41 — Spur B "re2gator" — Dossier (fortlaufend)

Baum: `.claude/worktrees/r41_re2gator` (Zweig `r41/re2gator`, Basis master 116e8b0f).
Auftrag (AUFTRAG.md, Wortlaut): (1) RE2-Finisher + fehlende Animationen des Alligators extrahieren;
(2) Verfolgung wie RE2 (Animation, Schreie, Beschleunigung an gleichen Stellen), Waende wackeln + Putz
faellt, Klettern ueber die Plattform BLEIBT; (3) Kanister-Aufnahme + Sterbeanimation mit geplatztem
Kiefer GENAU wie RE2 (Mechanik mit Testhaken; die Szene baut Spur G).

Zeiten aus `date`. Jeder Schritt wird hier VOR dem Bau notiert.

## 0. Protokoll (neueste Eintraege oben)

* 2026-10-24 02:51-04:xx (FORTSETZUNG 3 nach Sitzungslimit ~01:00, neuer Agent): Baum sauber, HEAD a6653809 (Kamera-
  Programme, UNGEPRUEFT) -> gebaut (keine neue Warnung) + gezielt getestet; Schnittfolge am ECHTEN Ablauf gemessen (§9.4).
  Drei Befunde aus der Messung behoben: Kamera-Wahl mit RE2-Massstab-Grenze (74f247c1, §9.5), R1-Zeitlupe VSync 4
  (6592c742, §9.7), Putz am Ursprung statt um Leon / nie gezeichnet (fb8ed4da, KORREKTUR in §8 M2). §8.8 + §9.3-9.8 neu.
* 2026-10-23 22:0x-22:2x: Kiefer-Umbau gebaut (22:04, 783 Schritte, keine neue Warnung), Unit-Riegel 45/45 inkl.
  unit_r41_re2gator_kiefer, Integration r41_re2gator + r36_schuss 2/2 am neuen Stand. exe-Messungen (§4.3): Lauf A2
  (Verfolgung + RE2-Finisher, Clip-Folge 0/1/4/0/1/4/5/11 bitgleich in zwei Laeufen) und Laeufe B2/B3 (Kanister-
  Aufnahme im LURK, erster Pistolentreffer F70 -> R1: SE 4, Clip 9 37 Bilder, Ziel-Yaw +160 mit 75/Bild, Kiefer-
  Scherben, Leiche). Erster B-Versuch scheiterte an der Zielhilfe (rundum naechster Gegner = Spinne 2) - §4.3.
* 2026-10-23 21:5x (FORTSETZUNG nach Sitzungslimit, neuer Agent): Baum sauber, HEAD c488687a (= 86661849 +
  AUFTRAG-Entscheide). Bau: local_build.sh configure + build, danach alle beruehrten Quellen per touch neu
  uebersetzt (783 Ninja-Schritte, EXIT 0, keine Warnung in den Spur-B-Dateien; die drei main.c-Warnungen
  liegen an Z. 8803/10132/10512 = Altbestand) -> 86661849 kompiliert. Gezielte Unit-Tests (§5): 43/43 gruen.
  Orchestrator-Entscheide B1-B3 (AUFTRAG.md 21:37) uebernommen: kein Tempo-Deckel, Tod nur ueber den
  Kanister, Putz nur am Dash-Start — der Code entspricht dem bereits; §7 entsprechend abgeschlossen.
  KORREKTUR am OFFEN-Punkt "Kiefer": FUN_80104318 selbst disassembliert (§2.6) — a1 = `[em+0x198] + 1204`
  = Part-Record 7 des RE2-MODELLBLOCKS (Stride 172), die Felder +0x98/+0x9A/+0x9C/+0x9E/+0xA0/+0xA4/+0x70
  sind die Drift-/Burst-Felder des Records (FUN_80028DAC / FUN_8002D3C8), KEIN Kiefer-az: der Kiefer wird
  zum freifliegenden, als dunkelrote Scherbenwolke gezeichneten Teil (Flagwort 0x4A), steigt 10/Bild und
  verschwindet nach 29 Bildern. Der Port fuehrt genau diese Part-Bruecke schon (Zombie-Arm, Kraehe) ->
  Anschluss ueber einen 3-Zeilen-Haken in re15_re2z_gore_active + Seed/Stempel in gator_re2_r41.c; der
  falsche "-10 az" aus 86661849 ist entfernt. Riegel unit_r41_re2gator_kiefer neu.
* 2026-10-23 19:1x: Kanister-Sichtbarkeit GEMESSEN statt gedeutet (§4 Sichtlaeufe e-i): RE15_TRILOG zeigt
  110 Dreiecke/Bild auf TIM-Platz 53 am richtigen Ort und in richtiger Groesse (Durchmesser 534 -> 17 px bei
  z 11000, H = 350), Pixel-Differenz mit/ohne Kanister 34 px (liegend, Cut 7) bzw. 522..902 px (im Maul); die
  Lichtrechnung liefert hellste Scheitel 53..84 (von 128) wie fuer jeden Prop, die RE2-Textur des Tankkoerpers
  ist (41,41,41) grau -> im dunklen Becken ~17/255 = praktisch unsichtbar. Hoehe des Testhakens auf -1200
  (GB_WATER_Y) gesetzt; Messzeile [gator-re2] Kanister Stufe ... im PC-Zeichner. Neubau 19:05, unit 7/7,
  Regression 5/5, integration_r41_re2gator gruen (19,12 s).
* 2026-10-23 19:xx: Modul, Haken, PC-Zeichner, Raumbank, Tests gebaut; unit_r41_re2gator 6/6 gruen, gezielte
  Regression 35/35 (unit_gator_sweep nach Bank-Anpassung), Integration laeuft/siehe §5. Op 11/73 fuer den Putz
  in re2_fx.c nachgezogen (FUN_8001e4b4 / FUN_8002387c).
* 2026-10-23 17:5x: RE-Belege (Abschnitt 2) vollstaendig, Kanister-Modell + TIM nach shared_assets/RE2
  geschnitten. Naechster Schritt: Modul gator_re2_r41.c (Zonen-Dash, Hysterese, Ereignis-Schuetteln, Rumble,
  Putz, Kanister T2/R1, HURT-Selektor RE2, HP-Sockel), Haken in enemy_ai_boss_gator.c, PC-Zeichner, Tests.
* 2026-10-23 16:59 (Start): Baum sauber (status --porcelain leer, HEAD 116e8b0f). Pflichtlektuere: AUFTRAG.md,
  VERTRAG.md, Memory reai-v2-gator-bosskampf.md, analysis/befunde_2026-09-19/gator-finisher-sounds.md
  (RE2-EM23: SE 4 Bruell @0x80100d84, Lunge Clip 2->3, Bissfenster Clip 3 f>=114, Stufe B2 vermessen und
  NICHT eingebaut, weil der Port-Lunge eine zielsuchende Hatz ist und RE2s Ansturm die Wurzelbewegung
  der Clips 2/3 ist), analysis/befunde_runde6_2026-09-12/gator-vollausbau.md (Zustandstabelle),
  befunde_runde8_2026-09-13/gator-schrei-schaden.md (SE-Zensus, ROOM40A0-Raumbank Saetze 14/15).

## 1. Messung vorher (Stand master 116e8b0f, Code-Zensus + Messungen der Vorrunden)

- Clips des Port-Bosses je Phase (enemy_ai_boss_gator.c master): LURK/CHASE/GUARD/CROSS immer Clip 0
  (`e->motion = 0`), LUNGE Clip 4 (45 F), FLINCH Clip 10, FRESSEN 4 -> 5 -> 11, DIE Clip 7. Nie: 1 (Paddeln),
  2/3 (Lunge-Anlauf/-Biss), 6 (Durchbruch), 8, 9 (Explosion). Gemessen in Runde 8 (gator-schrei-schaden.md
  §1.4: "der Port faehrt nur die Clips 0/4/5/7/10/11") und Runde 16 (Stufe B2 vermessen, nicht eingebaut).
- Verfolgung: Design-Vortrieb 56/F, BURST 100/F fuer 75 F (v0.7.72), kein Zonenbezug; Angriffs-SE 4 nur am
  Lunge-Start (Phase 2 Runde 16). Kein Bildschirm-Schuetteln (Sce_shake_on 0x5C ist im Port ein 4-Byte-Stub,
  scd_vm.c), kein Putz, kein Rumble aus dem Boss. HP 3000 (DESIGN), 10 %-Flinch + 6-Treffer-Flinch, Beschuss
  toetet (GBP_DIE Clip 7).
- Kanister: nicht vorhanden (kein Modell, keine Aufnahme, kein R1). Finisher: seit Runde 6/16 der RE2-Finisher
  (Clip 4 -> 5 -> 11, Opfer-Paar 3, Anker-Paar skaliert) - unveraendert uebernommen (unit_gator_fress gruen).
- Erster Lauf der neuen Riegel am NEUEN Code mit der falschen Test-Lage (Leon 5000 vor dem Gator): LUNGE +
  Fressen statt Paddeln (Clip 11 im Protokoll) - das ist das heutige Biss-Gate (Maulpunkt 2600 + 2400) des
  Nutzer-Designs, kein Fehler; die Riegel messen deshalb mit 6800 Abstand.


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
  -> shared_assets/RE2/KANISTER40A0.MD1 = RDT 0x4024..0x5388 (4964 B, md5 d4c74fedc299d001c8995cb94b5e9c5f;
  das len-Feld 3444 endet GENAU am Quad-UV-Block @0xC+0xD68, der 95 x 16 B bis zum naechsten Modell reicht -
  der erste Schnitt nach dem len-Feld wurde von re15_md1_parse verworfen, gemessen im exe-Lauf 3c9fe9dd) +
  KANISTER40A0.TIM (33824 B, md5 3b56b9667312cdae4bc4f97da076446b), byteidentisch aus dem RDT.
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
  aktueller CUT 6/13 (KORRIGIERT §9.1: [0x800d4820] = Cut, nicht Waffe) & z < -13300 (@0x80102218-7c); `|= 4` (Schuetteln stark) @0x801022a8-b4; +0x226 = 13; +0x176 = 3678;
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

### 2.6 Geplatzter Kiefer = FUN_80104318 (Part-Record 7 des RE2-Modellblocks) — KORREKTUR zu 86661849
R1 P0 ruft `addu a0,s1,zero / jal 0x80104318 / addiu a1,s3,1204` @0x80102350-58 mit s3 = `lw s3,408(s1)`
@0x80102174 = em+0x198 = der RE2-MODELLBLOCK (Records a 172 Byte, Aufbau FUN_80028368); 1204 = 7 x 172 =
Part-Record 7 = Kiefer. FUN_80104318 (selbst disassembliert, re2_disasm.py dis 0x80104318 --bin EM23_OVL_0000.BIN):
    80104318 lui a2,0x20          8010431c lw v0,0(a1)        80104320 ori a2,a2,0x2038    80104324 ori v0,v0,0x4a
    80104328 sw v0,0(a1)          8010432c lhu v1,118(a0)     80104330 ori v0,zero,0x8000  80104334 sh v0,160(a1)
    80104338 addiu v0,zero,-10    8010433c sh zero,156(a1)    80104340 sh v0,154(a1)       80104344 sh zero,158(a1)
    80104348 sh zero,164(a1)      8010434c sw a2,112(a1)      80104350 addiu v1,v1,2048    80104354 jr ra
    80104358 sh v1,152(a1)
= Record 7: Flagwort |= 0x4A, +0x70 = 0x202038, +0x98 = yaw + 0x800, +0x9A = -10, +0x9C/+0x9E/+0xA4 = 0,
+0xA0 = 0x8000. Das sind die Felder des FREIFLIEGENDEN TEILS (dieselben wie beim abgerissenen Zombie-Arm,
re2z_arm_throw @0x80107554-74, und beim Kraehen-GIB 0x80102CA0): der Zeichner FUN_80027434 nimmt bei Bit 0x40
die Record-Matrix statt der Elternkette (`andi v0,s3,0x40` @0x80027498), laesst bei Bit 0x08 die Drift-Physik
FUN_80028DAC laufen (@0x80027B98: `+0x9A += +0x9E; t.y += +0x9A` @0x80028DF0-E14 = steigt 10/Bild; Lebensdauer
`andi 0x7fff / sltiu 0x1d` @0x80028DC8-CC, sonst `sw zero,0(s0)` @0x80028DDC = Flagwort 0 = Teil weg nach 29
Zaehlschritten; Vortrieb +0x9C += +0xA4 = 0) und zeichnet ein Bit-0x08-Teil NIE als intaktes Mesh, sondern
ueber FUN_8002D3C8/D718 (FUN_80027434.c:168-170): jedes Prim entlang seiner n0-Normale um
Skala[life>>15][life&0x7fff] versetzt (Tabelle @0x8009DC28 Zeile 0 = 30..465, @0x8009DC64 Zeile 1 = 180..2790;
0x8000 waehlt ZEILE 1, `lhu / srl 0xf` FUN_8002d3c8.c) und alle Farben flach = +0x70 (`lw t2,112(a2)`
@0x8002D508): eine dunkelrote (R 0x38 G 0x20 B 0x20) Scherbenwolke, die 30 Bilder lang auseinanderberstet und
dabei 300 Einheiten steigt — der "geplatzte Kiefer". Danach fehlt der Leiche der Unterkiefer (Record 7 Bit 0 = 0,
Walk @0x8002737C). Am Kiefer haengt kein Kind-Bone (EM23-Elternkette, Riegel unit_r41_re2gator_kiefer), also
keine Kaskade @0x80027480-94. 86661849 hatte +0x9A = -10 als Kiefer-az und +0x70 als Faerbung des intakten
Kiefers gelesen — falsch; beides ist ersetzt (§3.1).

## 3. Umsetzung

### 3.1 Dateien
- NEU `engine/src/gator_re2_r41.c` + `include/re15_gator_re2_r41.h` (alle Konstanten mit @0x, s. §2).
- NEU `platform/pc/src/gator_re2_pc.c`: Kanister-Prop (KANISTER40A0.MD1/.TIM, TIM-Platz 53, Licht des aktiven
  Cuts, RTPT/NCLIP ueber objpoly_r38 wie der Prop-Pass), Raumbank GATOR40A0.VBS (Laderpaar wie TUERSE), Init.
- NEU `tools/r41_re2gator/gator40a0_se_cut.py` -> `shared_assets/RE2/GATOR40A0.VBS` (34296 B, md5
  fc68d18c687fac207e52f9d0c71e71ed) + `engine/src/gen/r41_gator40a0_bank.inc`.
- NEU `shared_assets/RE2/KANISTER40A0.MD1` (4964 B) / `.TIM` (33824 B) - byteidentisch aus ROOM40A0.RDT.
- `enemy_ai_boss_gator.c` (Eigentuemer): GB_HP = 25300; Neuspawn-Reset -> re15_gr2_reset; Frame-Woerter ->
  re15_gr2_frame_wort; Takt nach dem Treffer-Absorb; gb_absorb_hit = RE2-Selektor (R0/R4/R1) + Unverwundbarkeit
  in FLINCH/TOD; LURK->INTRO (Clip 6 -> 3) wenn Bahn frei; CHASE: Zonen-Dash (Clip 2 -> 3), Clip-Wahl + Vortrieb
  aus re15_gr2_chase_clip (BURST entfaellt, Block als `if (0)` dokumentiert); neue Phasen GBP_RE2_INTRO /
  GBP_RE2_DASH / GBP_RE2_TOD; Spine-Haken addiert +0x22A/+0x228 (Bones 6/7); Biss -> Sperre 70; Sieger-Abzug
  laesst die RE2-Phasen aus. gb_bahn_frei = Insel + Aussenwand-Klemmlinien (1600) fuer Clip-Bahnen.
- `platform/pc/main.c` (3 kleine Haken "Runde 41 Spur B"): `re15_bg_blit(-dx,-dy)` mit dem Schuettel-Versatz,
  `re15_gr2_pc_zeichnen` nach dem Prop-Pass, `re15_pc_gator_re2_init` bei den Port-Bank-Inits.
- `engine/src/re2_fx.c`: Op 11 (FUN_8001e4b4, Skala/3000) und Op 73 (FUN_8002387c) - beide vom RE2-CORE-
  Effekt 4 (Putz) benoetigt, Block "RUNDE 41 SPUR B" am Dateiende.
- `enemy_ai_re2_zombie.c` (gemeinsame Datei, 3-Zeilen-Haken in re15_re2z_gore_active, Muster Kraehe Runde 36 Spur C):
  Typ 0x23 -> Part-Bruecke an, sobald Record 0 Bit 0 traegt (Seed NUR durch R1). Damit laufen
  re15_re2z_gore_resolve / _part_matrix / _part_burst aus main.c (@10280/10512/10624, unveraendert) auch fuer den
  Gator: eigene Matrix + Drift FUN_80028DAC + Scherbenzeichnung FUN_8002D3C8 fuer Part-Record 7 (§2.6).
- `gator_re2_r41.c` gr2_kiefer_platzen (R1 P0, Ruf @0x80102354-58): Seed des Blocks wie FUN_80028368 (16 Records
  Bit 0, +0x70 = 0x808080, Mesh = i) + Stempel FUN_80104318 (§2.6). re15_gr2_kiefer_farbe (Fehldeutung) entfernt,
  re15_gr2_kiefer_geplatzt (Messhaken) neu; re15_gr2_bone_az(7) ohne den falschen -10 (R1 setzt +0x228 = 0).
- `tests/unit/probe_gator_sweep.c` (bestehender Riegel, MIT BELEG angepasst): laedt jetzt die EM23-Bank wie
  probe_gator_fress.c, weil der Vortrieb der Verfolgung aus den Keyframe-Bytes 6..11 kommt (FUN_80015e7c);
  ohne Bank gab es keinen Vortrieb (3286/3286 "end == start"). Mit Bank: gruen in 1,9 s.

### 3.2 Was woher kommt (RE2 / NUTZER-VORGABE / PORT-WAHL)
- RE2 (byte-belegt, §2): HP-Sockel, Hysterese + Blend-Zaehler, authored Vortrieb je Bild, Dash-Clips + SE 4,
  Intro-Clips + Sperre 110, Biss-Sperre 70, Ereignis-Maschine (Rumble-Rampen, Bits 4/8 fuer 5/10 Bilder),
  Schuettel-Tabellen + Hintergrund-DR_MOVE, HURT-Selektor (R4-Wahrscheinlichkeit 1/4, Sperre 120..183,
  Unverwundbarkeit, Clip 10 Blend 3; R0 nur SE 5), Kanister-Fenster/Vorhalt/Stufen/Ease-Werte/Lokallage
  (1750,-210)/liegende Drehung (0x690,0,0xfc00), R1 komplett (SE 4, Raum-Satz 15, Rumble (8,3)+(50,255->0),
  Ereignis 1, Kiefer = Part-Record 7 als Burst-Teil (Flagwort 0x4A, Tinte 0x202038, Lebensdauer 0x8000,
  vy -10, Kurs yaw+0x800; §2.6), Fontaenen 64 Ticks, Spritzring, Clip 9, Vortrieb -500
  (+40, -2/F), Ziel-Yaw +160 mit 75/F, Entfernen nach 9 P1-Bildern, Endzustand Leiche), Putz-Emitter (Ids,
  Typen, Params, Richtungen, Hoehen, Pausen, 6 Durchlaeufe).
- NUTZER-VORGABE (bleibt): Klettern ueber die Rampe (gb_cross_*), GUARD-Belagerung, Wegfindung im Becken,
  Biss-/Schadens-Mechanik (Clip-4-Schnapp, Fenster 6..34, 50 Schaden, Knockdown, FRESSEN bei hp<0),
  Render-Scale 2/3 (GB_SCALE_Q12; alle Rig-Versaetze mit s skaliert).
- PORT-WAHL (keine RE2-Adresse, je begruendet im Code): Zonenlinien = die drei sce-8-Wasserzonen des RDT statt
  x<-17877/z>=-7322; Dash/Intro nur bei freier Bahn (gb_bahn_frei); kein `yaw += 0x400` nach Clip 3 (Korridor-
  Ecke; Root-Yaw der Clips 2/3 endet bei 0, EMR selbst geparst); Putz-Bursts beim Dash-Start um Leon (RE2:
  Kamera 9 am Korridorende, Hoehen -2000..-4400 ueber dem Steg = ueber Leons Watt-Ebene y=0); Kanister-
  Aufnahme laengs der Blickachse mit seitlicher Koerperbreite (RE2: Korridor); R1 auch aus dem Schwimm-Sub
  (RE2-Ausnahme +0x230 == 1 @0x80101f04 bewusst weggelassen - Nutzer: "sobald er den Kanister im Maul hat");
  Kanister-Hoehe des Testhakens y = -1200 (= Wasser-INIT-Y der 0x23-KI @0x8010c65c, GB_WATER_Y; Sichtlauf c
  zeigte den Kanister bei -267 komplett unter der gemalten Wasseroberflaeche); Kanister 1x gezeichnet (Leons
  Welt); Beleuchtung wie jeder RDT-Prop (Lichtsatz des aktiven Cuts, FUN_8002c18c-Weg des main.c-Prop-Passes).
- Entfallen: BURST-Design (RE2s Paddeln ist die Beschleunigung), 10 %-/6-Treffer-Flinch (RE2-Kadenz), Attrition-
  Tod durch Beschuss (Sockel); GBP_DIE (Clip 7) bleibt fuer e->state == 3 (GB_TEST=3 / Explosionen).

### 3.3 Frame-Latenz des Frame-Wort-Blocks
Der Boss-Tick liest das Frame-Wort des Bildes, das der VORIGE Tick gesetzt hat (Port-Struktur, gilt seit Runde 8
fuer alle Frame-Flag-SEs). RE2 verarbeitet das Wort im selben Tick nach dem Clip-Vorschub (@0x80100438 nach dem
Dispatch). Folge: Stroke-SE/Ereignis ein Bild spaeter als in RE2; gemessen in unit_r41_re2gator_dash (Bit 4 ab
+4 statt +3 relativ zum Wort-Bild).

## 4. Messung nachher (Riegel, Build re15_port/build, 2026-10-23)

| Messung | Wert |
|---|---|
| HP nach INIT | 25300 (RE2 INIT) |
| Verfolgung, Leon 6800 fern, Zone gleich | Clip 1 in 60/60 Bildern, Wrap nach 26 Bildern, Vortrieb = off[f]-off[f-1] x 2731/4096 in 30/30 Bildern (+-2) |
| Biss-Sperre 70 | Clip 0 in 70/70 Bildern, danach Clip 1 |
| Zonenwechsel in Leons Zone | SE 4 am Start, Clip 2 150 Bilder, Clip 3 150 Bilder, dann CHASE (Dash 45..345) |
| Stroke-Wort Clip 2 f50 | Bit 4 (Modus 2) 5 Bilder ab +4, Bit 8 (Modus 1) 10 Bilder +9..+18, Rumble-Ring belegt, Versatz 0..3 px |
| Intro (Aggro, Bahn frei) | Clip 6 125 Bilder, Clip 3 150 Bilder, Sperre 110 -> Clip 0 110/110, danach Clip 1 |
| 200 Pistolentreffer (je 30, RE1.5-Zeile) | HP 25300 -> lebt (kein Tod), geschwaecht ab Treffer 11, R4 (SE 0 + Clip 10) 48x mit Abstand >= 4 Treffer-Takte (Sperre 120..183 Bilder), R0 (SE 5) sonst, nie beides |
| Kanister 8800 x 2/3 vor der Schnauze | Stufe 1 sofort, Stufe 2 13 Bilder (Kopf-az 204, Kiefer-az -272), 3/4 je 6, 5/6 je 7, getragen ab Bild 40 (Kopf 34, Kiefer -161 = Ease-Summen), Lage folgt Part 6 |
| Treffer beim Tragen | R1: SE 4, Raum-Satz 15, Rumble beide Ringe, Clip 9, Stufe 13, Kanister 10 Bilder sichtbar (P0 + 9), Leiche (Clip-9-Endpose), weitere Treffer wirkungslos |
| Dash-Start | 54 Putz-Partikel (6 x 9) ueber re2fx, 0 unbekannte Ops (Op 0: 918, 11: 54, 73: 864 Rufe) |
| unit_gator_sweep (3286 Paare) | gruen, 1,9 s (vorher ohne Bank 3286 Blocker = kein Vortrieb) |
| unit_gator_fress (RE2-Finisher) | gruen (unveraendert) |

exe-Messungen (eigene exe-Kopie, ROOM2090 per Sprung @250, GB_TEST=1, Titel-Auto-Start wie r35):

| Lauf | Ergebnis |
|---|---|
| integration_r41_re2gator (Kanister -6000,-15000, 300 Bilder im Raum) | exe bis EXIT_AT (26 s); Kanister-Modell geladen (1 Mesh, TIM-Platz 53); 'RE2 KANISTER gesetzt'; Gator-Clips in 300 Bildern: mo=1 (Paddeln) 103, mo=4 (Schnapp) 68, mo=5 (FRESSEN P2) 89, mo=0 40; HP nie unter 25300 |
| Sichtlauf a (Kanister -7200,-23500 nahe der Tuernische, Framedump 1..700/50) | 'RE2 KANISTER Stufe 1 laengs=5963 quer=110' -> 'angehaengt lokal=(3683,2317,556)' -> 'getragen (Stufe 7)' - die Aufnahme laeuft im echten Spiel beim Vorbeischwimmen; Bilder scratchpad/sichtlauf_a/bild_*.png, Belegbilder analysis/befunde_runde41/bilder_r41b/ |

Sichtlaeufe Kanister (eigene exe-Kopie re15_pc_r41b_sicht.exe, RE15_FRAMEDUMP + RE15_TRILOG, 2026-10-23 19:0x-19:1x):

| Lauf | Aufbau | Messung |
|---|---|---|
| e/f | Leon Tuernische (Cut 0), Kanister (-8200,-24300,-1200) | TRILOG: 110 Dreiecke/Bild auf Platz 53 (55 Vierecke, 40 Rueckseiten per NCLIP weg), Kasten (98..115, 91..107), z 10875..13512 = der Kanister liegt laengs der Blickachse (Dir_set 0x690/0/0xfc00 = Laengsachse auf Welt-X, Cut 0 blickt nach Westen); Groesse stimmt: 534 Durchmesser x 350/11000 = 17 px; hellster Scheitel (81,81,81), Ambient (63,63,63), Tint 128; Textur-Upload 128x512 8bpp 2 CLUTs, 0,8 % transparent |
| g | Leon (-6000,-19000) per RE15_PLAYER_POS -> Cut 7 (Blick nach Norden) | Cut 7 nur 37 Bilder: Spinne wirft Leon nach (-5397,-19797) = Zone 22 -> Cut 0; Framedump ab 40 deshalb Cut 0 |
| h | Leon (-6000,-18520) -> Cut 7 fuer 68 Bilder, Kanister (-6500,-15700,-1200) | Stufe 1 laengs=5850 quer=-32, angehaengt lokal=(3640,890,131), getragen (Stufe 7); liegend Kasten (89..148, 118..132) = 60 x 15 px quer zur Blickachse, getragen (150..207, 120..167); hellster Scheitel 47..53, Ambient (63,73,63) |
| i | wie h OHNE Kanister (Kontrolle) | Gator-Bahn bitidentisch (Positionen Bild 10/30 gleich); Pixel-Differenz h-i: Bild 10: 34 px (31 im Kanister-Kasten), Bild 20: 47, Bild 30: 151, Bild 40: 662, Bild 50: 522, Bild 60: 902 px im Kasten |

Befund: der Kanister WIRD gezeichnet (Ort, Groesse, Dreieckszahl, Differenzpixel), ist im Becken aber kaum zu
sehen: der Tankkoerper der RE2-Textur (CLUT-Zeile 481) ist (41,41,41) grau, die Warnstreifen liegen nur an den
Enden (v 128..140 / 236..255); mit dem hellsten Scheitel 53 (Cut 7) bzw. 81 (Cut 0) von 128 bleiben ~17..26/255
auf dem schwarzen Wasser. Liegend in Cut 7 aendert er 34 Pixel, im Maul 500..900 (Bilder bilder_r41b/, die
Ausschnitte *_gain4 sind zur Sichtbarkeit 4x aufgehellt und so beschriftet). Das ist die RE2-Textur unter
ROOM2090-Licht, keine Zeichnerluecke; Spur G entscheidet Kamera/Lage der Szene (Cut 7 = Zone 2 des RDT:
x -10000..-1000, z -19500..-18500 von Cut 0 aus; zurueck nach Cut 0 ab z < -19500).

⛔ Gemessene Folge des RE2-Tempos: Leon in der Tuernische (-8200,-25550, 100 HP) ist in BEIDEN exe-Laeufen binnen
~210 Bildern gefressen (mo=5 ab ~Bild 210; Lauf 0d58130a mit EXIT_AT 1300: Game Over -> Titel, der Riegel lief in
den Timeout). Der Riegel misst deshalb 300 Bilder. Entscheidungsfrage §7.1.

### 4.3 exe-Messungen am gebauten Stand (25a3fc77 + Bau 22:04; eigene exe-Kopien re15_pc_r41b_lauf*.exe)
Umgebung aller Laeufe: RE15_NO_INTRO=1 RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=2090@250
RE15_AUDIO_CAP_SYNC=audio.raw (Mixer ohne Geraet, damit die SE-Zeilen entstehen; mit RE15_NOAUDIO bleibt
g_audio.initialized 0 und der ENEMSE-Pfad kehrt vor dem Log um) RE15_SE_DEBUG=1 RE15_STATE_LOG RE15_FRAMEDUMP.
Bildzaehler: g_engine.frame_count wird beim Raumeintritt genullt (main.c:8241); RE15_EXIT_AT=<n>#2090 muss VOR dem
Raumende liegen - im ersten A-Lauf (EXIT_AT 500) ging Leon bei F197 verloren, Game Over -> Titel -> Auto-Neustart,
und der Neustart ueberschrieb die Raum-Bilder 0..500 mit Vorspann-Bildern (Dateizeiten 22:05:41-58 = eine Folge).

**Lauf A2 - Verfolgung + Finisher** (GB_TEST=1, Leon in der Tuernische (-8200,-25550), EXIT_AT=460#2090; identisch
mit Lauf A, also deterministisch). Gator-Clips (state.log, Slot 20, t=23):

| Bilder | mo | Phase | Beleg |
|---|---|---|---|
| F1-F6 | 0 | INIT/LURK (Teleport Lauerplatz, Yaw zur Leiter) | - |
| F7-F79 | 1 | PADDELN Sub 2 (Wurzel-Distanz >= 0x65) | @0x80100b88, Vortrieb authored ~149/Bild (z -9406 -> -20900 in 73 Bildern) |
| F80-F122 | 4 | Schnapp (Nutzer-Design LUNGE, Clip 4 45 Bilder), Treffer F87 Leon 80 -> 30 | gator_boss.log BISS af=7 hr=1 |
| F123-F156 | 0 | SCHWIMMEN = Biss-Sperre +0x238 = 70 ab F87 (endet F157) | @0x801016b0-b4 |
| F157-F186 | 1 | PADDELN | |
| F187-F211 | 4 | Schnapp, Treffer F197 Leon 30 -> -20 = toedlich -> FRESSEN P0/P1 (Clip 4 bis f13) | @0x801010FC-1104 |
| F212-F331 | 5 | FRESSEN P2 Clip 5 (120 Bilder) + Opfer-Clip 1 synchron, FRESS-P2 Anker=(-5774,-23046) yaw=1344 | @0x80101168-78 |
| F332-F451 | 11 | FRESSEN P3 Clip 11 Kau-Schleife (120 Bilder) | @0x801011D8-E0 |
| F452- | 0 | Sieger-Abzug | |

Leon-HP: 100 -> 80 (F57, Spinne 2 - se=0 Bank 11) -> 30 (F87, Biss) -> -20 (F197, Biss). SE (debug.log [re2se]):
se=2 Bank 17 (Frame-Flag Clip 0/1, mehrfach), se=4 Bank 17 am Schnapp-Start (F80 / F187), Bank 11 = Spinne.
Keine HP-Abnahme des Gators (25300, kein Beschuss). Bilder: bilder_r41b/finisher_a2_f090_schnapp_an_der_leiter.png
(Schnapp an der Leiter), finisher_a2_f400_p3_kauschleife_youdied.png (P3 von oben, Leon im Maul unter YOU DIED);
die Kamera dazwischen ist die Port-Todeskamera (Nahaufnahme, §6 PORT-WAHL), kein RE2-Kameraskript.
KORRIGIERT (Nachbesserung 1, M4 - die urspruengliche Erklaerung "Leon und Gator blieben in derselben Beckenzone,
die Zonenlinie wurde nicht ueberschritten" war FALSCH): der Gator durchlief in diesem Lauf die Zonen 1 -> 2 -> 0
(state.log F40 z -14990 ... F61 z -18020 -> F62 (-7289,-18278) = Zonenlinie z = -18100 in Leons Zone 0), die
Vorbedingung des Dashes war erfuellt. Nicht gezuendet hat er, weil die damalige Bahn-Probe gb_bahn_frei (Ende-x
>= -8900 + 1600 = -7300 fuer die GANZE Clip-Bahn) auf der Westwand-Klemmlinie scheiterte: Clip-2-Ende x = -7309,
Clip-3-Ende mit dem RE2-Korridor-Seitversatz (-2775 x2/3) bei x ~ -9986 in der Wand; der Intro vom Lauerplatz
(Ende-x ~ -7830) ebenso. Ursache und Behebung: §8 (M1). Der Riegel unit_r41_re2gator_dash mass den Dash nur in
einer Geometrie nach Norden (Gator (-6500,-22000), Leon (-6500,-8000)), die im Spiel aus der Tuernische nie eintritt.

**Lauf B (erster Versuch, GB_TEST=1, Kanister (-7500,-16500), Leon Tuernische Blick Nord, Pistole, Skript
W1.0 + 20 x (M0.3,MA0.1)):** Aufnahme im Vorbeischwimmen (Stufe 1 laengs=5916 quer=-240 -> Stufe 7, Bilder
kanister_* des Vorgaengers bestaetigt), aber 0 Treffer auf den Gator: die byte-true Zielhilfe FUN_8003703c
(re15_player_aim_target, naechster Gegner RUNDUM, kein Bogen) drehte Leon beim Heben auf rot 3943 = Peilung zur
Spinne 2 (4633 entfernt; Gator 10600), und der Schuss-Streifen traf die Spinne. Peilungskonvention dabei
belegt: bearing = (atan2q(dz,dx) - 0x400) & 0xfff -> Ost 0, Sued 1024, West 2048, Nord 3072 (Spieler und
Gegner gleich; INIT-Yaw des Gators vom Lauerplatz zur Leiter = 1087, gemessen r1089).
Mit beiden Spinnen im Becken gibt es keinen Stand, der "Gator naechster Gegner" UND "noch nicht im Biss" erfuellt
(Biss-Commit bei Maul < 3200 = Wurzel ~5650, Spinne 2 immer naeher) -> Messaufbau B2.

**Laeufe B2/B3 - Kanister-Aufnahme -> Schuss -> Kiefer-Tod** (GB_TEST=5: Gator parkt bei (4997,-20318) ohne
Distanz-Aggro, INIT-Yaw 1087; Kanister 5870 voraus = (4430,-26160); Leon (6500,-25000) Blick Nord = naechster
Gegner (Gator 4917, Spinne 2 8869); RE15_GIVE=3:15,21:50 RE15_EQUIP=3; Skript W2.0 + 10 x (M0.3,MA0.1);
EXIT_AT=260#2090; B3 zusaetzlich RE15_FORCE_CUT=1 = Kamera SW-Ecke mit Blick auf den Parkplatz). Beide Laeufe
bitgleich im state.log:

| Bild | Ereignis | Beleg |
|---|---|---|
| F7 | KANISTER Stufe 1 laengs=5868 quer=17 (Fenster 5734..6000 x Scale, Rechnung 5870) | @0x80100ae4-ec |
| F8-F20 | Stufe 2 (13 Bilder), F20 angehaengt lokal=(4211,1178,5) an Part 6 | @0x80101860-8c, @0x801018d0-60 |
| F21-F46 | Stufen 3/4 (6/6), 5/6 (7/7), F47 "getragen (Stufe 7)"; Gator liegt still (LURK, mo=0) | @0x80101970-b58 |
| F61-F69 | Leon hebt (R1), Zielhilfe dreht auf den Gator (rot 3072 -> 2867 = Peilung) | FUN_8003703c/8001a8f8 |
| F70 | Schuss trifft (HP 25300 -> 25293 = Pistolenzeile 7, s_wpn_dmg_gator[3]) -> Aggro + HURT-Selektor R1 | @0x80101ef4-fc |
| F70 | R1 P0: SE 4 (re2se se=4 Bank 17), Kanister Stufe 13, Rumble, Kiefer-Record 7 gestempelt, Fontaenen, Spritzring | @0x801021c4-0x80102374 |
| F70-F106 | Clip 9 (af 1..36 = 37 Bilder), Yaw 1089 -> 1164 -> 1249 (Ziel +160, Schritt 75) | @0x8010239c-a4, @0x801025f8-608 |
| F70-F106 | Vortrieb: (4997,-20318) -> (3413,-25001) (Rueckstoss -500 + 40-2/Bild auf den Anker) | @0x8010246c-84, @0x8010257c-a0 |
| F70-F99 | Kiefer = Scherbenwolke 30 Bilder (Part-Bruecke), danach Record 7 Bit 0 = 0 | FUN_80028DAC/8002D3C8 |
| F79 | Kanister weg (10 Bilder sichtbar, Stufe 13 -> entfernt) | @0x80102394, @0x8010255c-78 |
| F107- | Leiche: mo=9 af=36 gehalten, Position fest, "R1 ende (Leiche)" | @0x80102654 |

Bilder (bilder_r41b/): kiefer_b2_f064_kanister_getragen.png (Cut 0, Kanister hell im Maul), kiefer_b2_f076_scherben.png
und _f084_scherben.png (Cut 0: dunkelrote Scherben fliegen durchs Bild - Zeile 1 der Skalentabelle, bis 4 x 2790
Einheiten), kiefer_b2_f160_leiche.png; kiefer_b3_cut1_f064/f076/f088/f160_*.png (Cut 1 von der SW-Ecke: Gator mit
Kanister, Scherben + Spritzring am Kopf, Fontaenen, Leiche) und die *_ausschnitt_x3_gain3.png (Ausschnitt um den
Gator, 3x vergroessert, 3x aufgehellt - nur zur Sichtung, so beschriftet). Der Raum-Satz 15 (GATOR40A0.VBS) laeuft
ueber den Raumbank-Haken (unit_r41_re2gator_kanister pinnt ihn); im [re2se]-Log erscheint nur die ENEMSE-Bank.

## 5. Tests
Neu (tests/unit/probes/r41_re2gator.cmake): unit_r41_re2gator_{clips,dash,intro,sockel,kanister,putz},
integration_r41_re2gator (tests/integration/test_r41_re2gator.cmake, eigene exe-Kopie re15_pc_r41b_*.exe).
Angepasst mit Beleg: unit_gator_sweep (EM23-Bank laden, §3.1).

Gezielt gelaufen (keine volle Suite):
- `ctest -R "unit_r41_re2gator"`: 100% tests passed, 0 tests failed out of 7 (modell, clips, dash, intro, sockel, kanister, putz);
  nach dem Neubau 19:05 (Hoehe -1200, Messzeile) erneut: 100% tests passed, 0 tests failed out of 7 (0.19 sec)
- `ctest -R "^unit_(gator_sweep|gator_fress|alligator_ai|re2_spider_water_2090|r34_re2fx)$"` (nach dem Neubau):
  100% tests passed, 0 tests failed out of 5 (2.06 sec)
- `ctest -R "^integration_r41_re2gator$"` (nach dem Neubau, Kanister-Hoehe -1200): 100% tests passed, 0 tests failed out of 1 (19.12 sec)
- `ctest -R "^unit_(alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|r34_re2fx|r35_granate|r36_granate|r30_granate|r38_wasser|r38_wasserzombie|montage_fx|r38_montage|r35_cut1150_montage)"`:
  97% tests passed, 1 tests failed out of 35 (unit_gator_sweep, VOR der Bank-Anpassung) -> danach
  `ctest -R "^unit_gator_sweep$"`: 100% tests passed, 0 tests failed out of 1 (1.91 sec)
- `ctest -R "^integration_(r35_granate|r38_wasser|r41_re2gator)$"` (Stapel 1): integration_r38_wasser Passed 180.00 sec;
  integration_r35_granate Failed (duenn_a exe exit=-1 bei Bild ~44, ROOM1220 ohne Gator) - Ursache: mein eigener
  local_build.sh-Lauf um 18:27 (taskkill //IM re15_pc.exe, r35 nutzt die exe unter Originalnamen); Wiederholung
  ohne Bau: `ctest -R "^integration_(r41_re2gator|r35_granate)$"`: integration_r35_granate Passed 162.92 sec
  (gator: HP 25300 -> 24300, birkin ok); integration_r41_re2gator zweimal rot durch den Riegel selbst (Titel ohne
  Auto-Start; dann EXIT_AT 1300 nach Leons Tod unerreichbar; dann MD1 ohne UV-Block), danach
  `ctest -R "^integration_r41_re2gator$"`: 100% tests passed, 0 tests failed out of 1 (26.39 sec).

Fortsetzung 2026-10-23 21:4x-22:2x (gebauter Stand, keine volle Suite):
- Stand c488687a (= 86661849) nach touch + Neubau: `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|sca_wall_hit|montage_fx|r34_re2fx.*|r30_granate|r35_granate|r36_granate|r38_wasser.*|r38_wasserzombie|r35_cut1150_montage.*|r38_montage.*)$"`:
  100% tests passed, 0 tests failed out of 43 (8.21 sec)
- Stand c488687a: `ctest -R "^integration_(r41_re2gator|r35_granate|r38_wasser|r34_granaten|r36_granate|r30_granate_laden|r38_montage|r38_wasserzombie)$"`:
  100% tests passed, 0 tests failed out of 8 (r30_granate_laden 82 s, r34_granaten 239 s, r35_granate 170 s,
  r36_granate 96 s, r38_montage 107 s, r38_wasser 185 s, r38_wasserzombie 168 s, r41_re2gator 27 s)
- Stand 25a3fc77 (Kiefer-Umbau + Haken in enemy_ai_re2_zombie.c), Bau 22:04: `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_spawn_seed|r36_schuss|r36_schuss_kraehe|r36_kleidung_kraehe|r35_fenster_kraehe|re2_crow_ai|crow_ai|crow_fixes|crow_death_corpse|re2_crow_kill_cmd3|r37_re2zombie_.*|r34_re2fx.*|montage_fx)$"`:
  100% tests passed, 0 tests failed out of 45 (unit_r41_re2gator_kiefer: "Flag 0x4b Tinte 0x202038 life 0x8000 vy -10
  Kurs 5120; Scherben 30 Bilder Skala 180..2790, weg ab 31, Kinder 0")
- Stand 25a3fc77: `ctest -R "^integration_(r41_re2gator|r36_schuss)$"`: 100% tests passed, 0 tests failed out of 2
  (integration_r36_schuss 161.53 sec, integration_r41_re2gator 27.58 sec)

Fortsetzung 3 2026-10-24 (Endstand fb8ed4da, Bau 03:5x, keine volle Suite):
- `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_.*|re2_hit_repeat_crow_spider|re2_crow_.*|crow_ai|crow_fixes|crow_death_corpse|r36_schuss|r36_schuss_kraehe|r36_kleidung_kraehe|r35_fenster_kraehe|r37_re2zombie_.*|r34_re2fx.*|montage_fx|r35_cut1150_montage.*|r38_montage.*|r38_wasser.*|r38_wasserzombie|r39_heliport1171_kraehen|r30_granate|r35_granate|r36_granate)$|^probe_(re2z_stomp|crow_kill)$"`
  (Regex der Abnahme 0 inkl. unit_r41_re2gator_wand/_kamera): **100% tests passed, 0 tests failed out of 89** (26,05 s).
- `ctest -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate)$"`: **100% tests passed, 0 tests failed out of 4**
  (r35_granate 193,89 s, r36_schuss 151,68 s, r38_wasser 193,60 s, r41_re2gator 32,58 s); Statuszeile r41_re2gator:
  `ok - ... RE2-Zeilen 49 (INTRO 1/1, DASH 1/1, Putz 6 x 9 = 54), Clip 6/2/3 Bilder 125/150/300, HP min 25300, Kamera
  DURCHBRUCH:6/0;SEITE:6/70;ANGRIFF:6/124;FREI:3/149;FINISHER:4/0, Putz gezeichnet bis 98 Quads/Bild in 20 Messbildern`.
- Riegel-Ausgaben: unit_r41_re2gator_kamera "R1-Fall 2: Winkelwahl ohne Grenze Cut 2 (massstab 0.0470, Grenze 0.0289),
  gewaehlt Cut 1"; unit_r41_re2gator_kanister "Zeitlupe VSync 4 ueber 38 Takte (R1 P0 .. vor P3)"; unit_r41_re2gator_putz
  "lebende Teilchen max 11/Bild, groesster xz-Abstand zu Leon 1560".

## 6. OFFEN (benannt, nicht geraten)

- (ERLEDIGT 21:5x) Part-7-Felder nach FUN_80104318: RE'd als Part-Record des Modellblocks (§2.6) und ueber
  die bestehende Part-Bruecke angeschlossen; der Scherben-Zeichner selbst (FUN_8002D3C8-Zwilling in main.c)
  ist seit Runde 5 (kopf-flug.md) byte-true und wurde nicht angefasst.
- Raum-Effekt 0x26 (ROOM40A0-ESP, erster Fontaenen-Tick `0x26032000` @0x80103f4c-6c) liegt nicht im Port
  (ROOM2090 hat eine andere ESP-Liste): der Spawn laeuft gegen die CORE-Bank ins Leere (re2fx_spawn -1).
  Die CORE-Effekte 0x00/0x08/0x03 der Ticks 2-8 und der Spritzring (Effekt 3) laufen. Wer den 0x26 will,
  schneidet ihn aus ROOM40A0.RDT (ESPid-Liste @0x59CC, EFF @0x65E4, TIM @0x221FC) nach shared_assets/RE2
  und registriert ihn ueber re2fx_register_raum.
- Frame-Wort-Latenz 1 Bild (§3.3) — Port-Struktur seit Runde 8, nicht in dieser Spur geaendert.
- (ERLEDIGT Nachtrag 4, §9) RE2-Kameraschnitte Intro/R1/Finisher auf die 2090-Zonenkameras; die Spieler-Sperre
  (Bit 0x10 Kino-Balken) bleibt PORT-WAHL: Leon behaelt die Kontrolle.
- Intro-Knick zur Westwand (Kopf ueber der Wand in Cut 7) und Finisher -> RE1.5-Todeskamera: §9.8, Fragen §7.
- Schaden je Waffe: der Port rechnet die RE1.5-Zeile je Typ (exe: Pistole 7 = s_wpn_dmg_gator[3],
  re15_damage.c:429; der Riegel sockel nutzt attack_type 0 = 30) — RE2 (0x800A4A28: 17) braeuchte eine
  RE1.5->RE2-Waffenzuordnung (nicht vorhanden). Folge: "geschwaecht" (< 25000) erst nach 43 Pistolentreffern.
- Die RE2-Hysterese-Schwellen 0x65/0x5a (Wurzel-Distanz < 101/90) sind im Becken praktisch "immer Paddeln"
  ausser in der Biss-Sperre — das IST RE2 (Wurzel liegt ~9000 hinter der Schnauze), aber der Gator ist damit
  mit 149/Bild (223 x 2/3) deutlich schneller als das fruehere Design 56/100. Entscheidungsfrage §7.
- Kanister im Becken kaum sichtbar (§4 Sichtlaeufe): RE2-Tanktextur (41,41,41) x ROOM2090-Licht (hellster
  Scheitel 53..84/128) = ~17..26/255. Gezeichnet wird er (34 bzw. 500..900 Differenzpixel). Keine Aufhellung
  eingebaut (keine RE2-Adresse dafuer; RE2 ROOM40A0 ist ein hell ausgeleuchteter Korridor). Spur G waehlt
  Kamera/Lage; falls gewuenscht: Prop-Licht wie die Irons-Tisch-Ausnahme (include/re15_irons_tisch.h) als
  PORT-WAHL mit Messtabelle.
- GUARD (Belagerung) und CROSS (Klettern) spielen weiter Clip 0 (Nutzer-Design unveraendert).
- Kein Original-Test mit DuckStation moeglich (info/re2leon ist ein Dateibaum, kein RE2-Image).

## 7. Fuer den Nutzer

Neue Assets (Paket-Gate, alle in `re15_port/shared_assets/RE2/`):
- `KANISTER40A0.MD1` (4964 B, mit UV-Block) + `KANISTER40A0.TIM` (33824 B): der RE2-Gaskanister (ROOM40A0 Modell 1).
- `GATOR40A0.VBS` (34296 B): RE2-Raumbrueller (Satz 0 = Tod/R2, Satz 1 = Explosion/R1) aus ROOM40A0.
Keine neuen Sprachdateien.

Bedienung / Testhaken:
- `RE15_GATOR_KANISTER=x,z[,y]` legt den Kanister ins Wasser von ROOM2090 (y Standard -1200 = GB_WATER_Y,
  Wasserkoerper-Hoehe des Gators; bei -267 lag er unter der gemalten Wasseroberflaeche). Spur G setzt dieselbe
  Funktion (`re15_gr2_kanister_setzen`) aus der Szene. Sichtlauf-Rezept (§4): `RE15_PLAYER_POS=-6000,-18520,0`
  + `RE15_GATOR_KANISTER=-6500,-15700` -> Cut 7 blickt nach Norden, der Gator kommt frontal mit dem Kanister
  quer im Maul; debug.log traegt die Messzeile `[gator-re2] Kanister Stufe N pos=... Kasten=... hellster=...`.
- Belegbilder `analysis/befunde_runde41/bilder_r41b/` (kanister_cut7_f10_liegend, kanister_cut7_f50_gator_frontal,
  kanister_cut0_f30_nische, *_ausschnitt_x4_gain4 = 4x vergroessert und 4x aufgehellt; finisher_a2_*,
  kiefer_b2_* (Cut 0), kiefer_b3_cut1_* (Cut 1) + *_ausschnitt_x3_gain3 / *_x2_gain25 = nur Sichtung, §4.3).
- Messrezept Kanister-Tod ohne Szene (§4.3): RE15_GB_TEST=5 "RE15_PLAYER_POS=6500,-25000,3072"
  "RE15_GATOR_KANISTER=4430,-26160" RE15_GIVE=3:15,21:50 RE15_EQUIP=3 RE15_INPUT_SCRIPT_BASIS=spiel
  RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W2.0,M0.3,MA0.1,..." (RE15_FORCE_CUT=1 fuer die Kamera auf den
  Parkplatz). Im Spiel gilt: die Zielhilfe (R1) dreht auf den NAECHSTEN Gegner - solange die Spinnen im Becken
  sind, trifft man den Gator nur, wenn er naeher als die Spinnen ist (Nutzer-Auftrag an Spur G: Spinnen entfernen).
- API fuer Spur F/G: `re15_gr2_geschwaecht()` (HP < 25000 = RE2-Schwaeche), `re15_gr2_treffer()`,
  `re15_gr2_traegt()`, `re15_gr2_tot()` (Leiche nach der Explosion), `re15_gr2_kiefer_geplatzt()`, `re15_gr2_intro_start/_tick`
  (Durchbruch Clip 6 + Schnapper Clip 3 fuer "springt aus dem Wasser").
- `gator_boss.log` traegt Zeilen "RE2 INTRO/DASH/KANISTER/R1 ...".

Entscheidungen (Orchestrator 2026-10-23 21:37 in AUFTRAG.md, Dossier-Stand beim Sitzungslimit; der Nutzer kann
widersprechen — der Code folgt diesen Entscheiden, nichts davon ist ein Schalter):
1. B1 Tempo: die RE2-Werte bleiben (Schwimmen Clip 0 = 24,5/Bild in der Biss-Sperre, Paddeln Clip 1 ~149/Bild,
   Zonen-Dashes Clip 2/3 mit SE 4), kein Deckel — Nutzer-Wortlaut "fast alles vom Aligator Fight von Resident
   Evil 2 uebernehmen ... an gleichen Stellen beschleunigt". Folge im Becken: Leon in der Tuernische ist binnen
   ~210 Bildern gefressen (§4). Der Nutzer testet das Tempo und entscheidet dann.
2. B2 Beschuss toetet nicht (RE2-Sockel 25300, Treffer-Reaktionen R0 SE 5 / R4 SE 0 + Clip 10 mit 25 % und Sperre
   120..183 Bilder wie RE2); der Tod kommt nur ueber den Kanister (Spur G): Aufnahme Stufen 1..7, Treffer beim
   Tragen -> R1 (Explosion, Clip 9, geplatzter Kiefer §2.6, Leiche).
3. B3 Putz nur an den RE2-Stellen = beim Dash-Start (ROOM40A0 sub13/sub14, 6 x 9 Partikel), nicht bei jedem
   starken Schuetteln.

Nachtrag 4 / Nachbesserung 1 (Fortsetzung 3) - was der Nutzer jetzt sieht:
- RE2-artige Kameraschnitte (§9): beim Durchbruch Cut 7 (Gator taucht auf und kommt frontal auf die Kamera zu), bei
  Clip-6-Bild 70 Cut 0 (seitlich), am Clip-6-Ende Cut 7, nach dem Schnapper zurueck auf Leons Kamera; bei der Kanister-
  Explosion ein Schnitt auf eine Totale, wenn Leons Kamera den Kopf nicht zeigt (zurueck nach Clip 9); beim toedlichen Biss
  ein Schnitt (Sperre bis Game Over). Jeder Schnitt steht mit Bildnummer und allen Kandidaten in gator_boss.log ("RE2 KAMERA").
- Die Kiefer-Explosion laeuft wie in RE2 in halber Bildrate (Zeitlupe, ~2,5 s).
- Der Putz faellt jetzt sichtbar um Leon (vorher entstand er unsichtbar am Raumursprung).
- Keine neuen Assets und keine Sprachdateien in dieser Fortsetzung.
Entscheidungsfragen (gemessen, nicht geaendert):
4. Finisher: RE2 bleibt bis zum Game Over auf dem Finisher-Schnitt; der Port zeigt ihn ~80 Bilder und blendet dann in die
   byte-true RE1.5-Todeskamera (Weissblende + Orbit, YOU DIED) - so lassen oder die RE1.5-Todeskamera im Gator-Finisher
   unterdruecken (RE2-Verhalten)?
5. Intro: der RE2-Knick (Clip 3 + yaw += 1024) dreht den Gator im Westkanal zur Westwand, der Kopf ragt in Cut 7 ueber die Wand
   (§9.8). So lassen (RE2-Animation unveraendert) oder den Intro-Start so drehen, dass der Knick zu Leon zeigt (neue PORT-WAHL)?
6. Explosion und Finisher sehen im Becken seitlich statt frontal aus: ROOM2090 hat keine Kamera vor dem Kopf in diesen Lagen
   (die einzige frontale fuer die Explosion laege 2,5-fach naeher als jeder RE2-Schnitt). Neue Kameras wuerden neue
   Hintergruende brauchen - nicht im Umfang.

## 8. Nachbesserung 1 (Abnahme 0, B_abnahme_0.md; Beginn 2026-10-23 23:1x, HEAD 18c0894b)

Orchestrator-Entscheid B5 (AUFTRAG.md master 23:06): die Haken in main.c / enemy_ai_re2_zombie.c / re2_fx.c sind
ANGENOMMEN (M3 kein Mangel mehr); Abnahme B 1 prueft zusaetzlich Nachtrag 4 (RE2-Kameraschnitte) - s. §9.

### M1 (Punkt 2c): RE2-Dash (Clip 2 -> 3) und Intro (Clip 6 -> 3) zuendeten im Spiel nie
- Ursache (Code, nicht Geometrie-Pech): beide Bahnen liefen ABSOLUT vom Clip-Anker (re15_clip_root_motion_abs_pub =
  FUN_8001ad68-Zwilling, der Griff-Platzierer) und wurden deshalb VORHER als Ganzes auf Freiheit geprueft
  (gb_bahn_frei: Insel + Aussenwand-Klemmlinien 1600 fuer den Endpunkt inkl. QUER-Komponente). Die CHASE-Route
  im Westkanal liegt auf der Klemmlinie x ~ -7290..-7434 (Leon in der Tuernische = Wand-Schatten); Clip-2-Ende
  x = -7309 < -7300 (um 9 gescheitert), Clip-3-Ende mit dem Seitversatz -2775 (x2/3) in der Westwand; Intro vom
  Lauerplatz (Yaw 1087) Ende-x ~ -7830. Alles gemessen von der Abnahme (Laeufe A/D1: 0 "RE2 "-Zeilen) und mit
  probe_bahn.c (nb1) nachvollzogen.
- Beleg der Clip-Geometrie (EM23.EMD aus CDEMD0.EMS, EMR-Keyframe-Bytes 6..11 und Root-Bone-Winkel, selbst geparst;
  Probe nb1/probe_bahn.c gegen libre15_engine.a, skaliert 2731/4096):
  * Clip 2 (150 Bilder): Summe (laengs, quer) = (2307, -90), groesster Einzelschritt 81; Root-ry 0 -> 438 (f40..f149).
  * Clip 3 (150 Bilder): Summe (1881, -2775); quer faellt ab f60 (roh z 22 -> -4042 bei f149); Root-ry 438 -> 1023
    (f70..f148) und im LETZTEN Bild f149 zurueck auf 0. Groesster Einzelschritt 70.
  * Clip 6 (125 Bilder): off[0] = (-7040, 0, 0) roh = Rueckversatz hinter die Wand (Korridor-Staging des
    ROOM40A0-Durchbruchs, RE2 Pos_set + Cut 0 @0x80100894-a4), danach +9498 roh laengs (6281 skaliert ohne f0);
    groesster Einzelschritt 320 (f29). Root-ry -47..+77, am Ende 317 (f124).
  * Die Quer-Komponente von Clip 3 IST die RE2-Korridor-Ecke: nach dem Clip addiert RE2 `yaw += 1024` -
    Dash @0x80100eb0-c0 `lhu v0,118(s1) / sb zero,549(s1) / addiu v0,v0,1024 / sh v0,118(s1)`, Intro @0x80100938-48
    `lhu v0,118(s0) / addiu v0,v0,1024 / sh v0,118(s0)` (beide selbst disassembliert, EM23_OVL_0000.BIN). Mit Yaw 2048
    (RE2-Korridor nach -x) zeigt quer -4042 nach +z = die neue Korridorrichtung (yaw 3072). Die Root-ry-Rampe auf
    1023 ist dieselbe Vierteldrehung als Pose; f149 = 0 ist das Bild, in dem der Entity-Yaw sie uebernimmt.
  * Zonenlinien-Pruefung Sub 1 Pre @0x80100964: `lw v0,56 / slti v0,v0,-17877` @0x80100990-98 -> Zone 1 + Routine
    0x301 (@0x801009a8-b0), `lw v0,64 / slti v0,v0,-7322` @0x801009d8-e0 -> Zone 2 + 0x301 (@0x801009ec-fc); liegt
    VOR dem Kanister-Vorpass (@0x80100a0c) und der Hysterese mit +0x238 (@0x80100b5c-64) -> der Dash zuendet auch in
    der Biss-Sperre (z.B. den 110 Bildern nach dem Intro).
  * RE2-Bewegung = FUN_8001a330 -> FUN_80015e7c (Delta `subu` @0x80015fcc/@0x80015fd4, Bild 0 `sw zero,524`
    @0x80015f14) + FUN_800152c8 (Rotation/Addition @0x80015314-34), info/re2leon/PSX.EXE (Zwilling in
    enemy_ai_common.c re15_clip_root_motion_delta). Die Dispatcher-Klemme des Ports fuer den 0x23er
    (enemy_ai_common.c:14873-14876, re15_collision_constrain_enemy nach dem Boss-Tick, hit_radius 1466) ist dieselbe,
    durch die der CHASE-Vortrieb laeuft.
- Aenderung (gator_re2_r41.c, enemy_ai_boss_gator.c, Header):
  * gr2_wurzel_schritt(): Dash/Intro bewegen bildweise (off[f]-off[f-1], laengs UND quer, um den Yaw gedreht) statt
    absolut; Bild 0 = Neu-Anker ohne Schritt (Port-Konvention; der Clip-6-Rueckversatz -7040 ist Korridor-Staging,
    im Becken ist der Lauerplatz der Startpunkt - PORT-WAHL). Jeder Schritt laeuft durch die Dispatcher-Klemme.
  * gb_bahn_frei entfernt. Intro: zuendet bei jedem Aggro (RE2 Sub 0 P0 wartet nur auf +0x1D4 @0x80100854-68).
    Dash: Zonenwechsel in Leons Zone + nur noch die LAENGS-Bahn (x-Netto 2+3) gegen Insel/Rampe (gb_seg_frei,
    GB_KOERPER_M) - damit er nicht 300 Bilder frontal gegen den Block drueckt (PORT-WAHL, Rest der alten Probe).
  * `yaw += 1024` nach Clip 3 (Dash @0x80100eb0-c0, Intro @0x80100938-48) jetzt uebernommen - die frueher als
    PORT-WAHL weggelassene Zeile liess den Koerper am Clip-Ende um 90 Grad zurueckpoppen (Root-ry 1023 -> 0).
  * Messhaken re15_gr2_im_intro(); Log-Zeilen "RE2 INTRO start ... bahn_ende", "RE2 DASH ende ... yaw".
- Unit-Riegel: NEU unit_r41_re2gator_wand (Intro vom Lauerplatz ohne Bahn-Gate + Dash auf der Westwand-Klemmlinie
  nach Sueden mit Leon in der Tuernische: SE 4, 150/150, Gator bleibt im Becken, laengs ~4188, yaw +1024);
  clips/dash/sockel/putz warten den jetzt immer zuendenden Intro per intro_abwarten() ab.
- Messung nachher: s. §8.7 (exe).

### M2 (Punkt 2e): Putz im Spiel nicht belegt, keine exe-Messzeile
- Ursache: (a) haengt an M1; (b) LATENTER LAGEFEHLER: g->putz_x/y/z wurden nie gesetzt (nur deklariert und in
  gr2_putz_takt gelesen) -> alle 54 Bursts um (0,0,0) + RE2-Versatz = noerdlich AUSSERHALB des Beckens (z > -5400);
  der Riegel unit_r41_re2gator_putz zaehlte nur Spawns.
- Aenderung: re15_gr2_dash_start(slot, e, pl) setzt die Lage auf Leons Stand beim Dash-Start (PORT-WAHL "um Leon",
  RE2: Kamera 9 am Korridorende, sub13 @0x0778/@0x0786); Messzeile je Durchlauf in gator_boss.log
  "RE2 PUTZ runde r/6 spawns=n gesamt=m um=(x,y,z) letzter=(x,y,z)" + Haken re15_gr2_putz_letzte(); der Riegel
  pinnt den letzten Spawn jeder Runde auf Leon + (-622, -2400, -50) (k_putz[8] @0x0900).
- Messung nachher: s. §8.7.
- KORREKTUR (Fortsetzung 3, 2026-10-24): die Messzeile und der Riegel pinnten nur die ANGEFORDERTE Lage (`s_putz_letzt`),
  nicht die wirksame. Gemessen mit der neuen Zeile `[gator-re2] Putz-Sicht` (gator_re2_pc.c, Stand 6592c742, Leon in der
  Tuernische): die Teilchen lagen bei `welt=(67,-2000,2)` .. `(-634,-4000,-722)` = am URSPRUNG, Bildlage x 394..560
  (ausserhalb), `quads=85 gezeichnet_vorbild=0` - der Putz war im Spiel nie zu sehen (Framedump F340-F372 ohne Staub).
  Ursache: gr2_fx uebergab eine STACK-Matrix (lokales m[32]); FUN_8001bf10 merkt sich den Zeiger (`sw a2,108(t0)` @0x8001cd6c,
  re2_fx.c mtx_src), FUN_8001d894 kopiert ihn bei Status-Bit 0x800 je Bild nach +0x4C (@0x8001d8ac-fc) -> haengender Zeiger;
  ausserdem stand die Lage in der Translation statt im Versatz. RE2 (selbst disassembliert): Sce_espr_on @0x800565a4 ruft
  FUN_8001bf10(a0, Richtung `lh a1,14(s1)` @0x80056610, a2 = FUN_80056a38(Byte 4 = 0), a3 = &{x,y,z} `addiu a3,sp,16`
  @0x80056618 aus den SCD-Bytes 8..13 @0x800565ec-0c); FUN_80056a38(0) -> Tafel @0x80011470[0] = 0x80056a78 `lui v0,0x800a /
  addiu v0,v0,-9404` = Einheitsmatrix 0x8009db44 (Bytes `00 10 00 00 .. 00 10`, Translation 0). Die Fontaene uebergibt die
  Part-Matrix des Modellblocks (`addiu a2,s2,1104` @0x80103f58), der Spritzring die Kanister-Matrix.
  Aenderung (fb8ed4da): Putz = `re2fx_einheitsmatrix` + Versatz (Lage um Leon); Fontaene/Spritzring = statische Quell-Matrizen
  (Part 6/7, Kanister in seiner AKTUELLEN Weltlage - vorher die alte freie Lage), je Takt nachgefuehrt wie der RE2-Modellblock.
  Messung nachher (Lauf putz3, gleicher Aufbau): `welt=(-8260,-3500,-25588)` .. `(-9434,-4000,-25937)`, Bildlage x 36..83 /
  y 25..59 (ueber Leons Kopf), `gezeichnet_vorbild` 27..98 Quads/Bild; Staubwolken ueber Leon in Cut 0 F350-F372
  (bilder_r41b/putz_m2_vorher_nachher_cut0.png, Ausschnitt x2 / x2,5 aufgehellt nur zur Sichtung). Riegel:
  unit_r41_re2gator_putz pinnt die wirksame Pool-Lage (Bank 4, +0x34/+0x38; groesster xz-Abstand zu Leon 1560, Soll <= 3000),
  integration_r41_re2gator die gezeichneten Quads ("Putz gezeichnet bis 98 Quads/Bild in 20 Messbildern") und die Lage um Leon.

### M3 (Vertrag §1.3): Haken in nicht zugeteilten Dateien
- Orchestrator-Entscheid B5: angenommen, Vertrag um B erweitert; Konflikte loest die Integration. master (bb08d04f)
  hat die drei Dateien seit der Basis 116e8b0f nicht angefasst (git diff 116e8b0f master --stat = leer).
  Haken: main.c @122 (include), @4008 (re15_pc_gator_re2_init), @5598-5599 (Schuettel-Versatz), @11094
  (re15_gr2_pc_zeichnen); enemy_ai_re2_zombie.c @4634-4637 (Typ 0x23 in re15_re2z_gore_active); re2_fx.c @880,
  @901-902 (Dispatch) + Block "RUNDE 41 SPUR B" ab @1560 (op_11/op_73).
  Fortsetzung 3: +1 Haken in main.c am Bild-Deckel (`budget = re15_gr2_pc_bildbudget(frame_budget_ms)`, R1-Zeitlupe §9.7).

### M4 (Dossier §4.3): falsche Erklaerung
- §4.3 Lauf A2 korrigiert: der Gator durchlief die Zonen 1 -> 2 -> 0 (Zonenlinie z = -18100 bei (-7289,-18278)
  in Leons Zone 0), die Vorbedingung war erfuellt; es scheiterte die Bahn-Probe (M1).

### M5 (Test): file(GLOB)+REMOVE aller re15_pc_r41b_*.exe
- test_r41_re2gator.cmake loescht keine fremden Kopien mehr; nur die eigene Kopie am Ende (file(REMOVE _exe_kopie)).

### M6 (Beleg-Luecke): Raum-Brueller Satz 15 in der exe nicht messbar
- gator_re2_pc.c raum_se(): debug.log-Zeile "[gator-re2] Raum-SE 15 -> GATOR40A0.VBS Satz 1" (bzw. "(Bank fehlt,
  stumm)") bei jedem Ruf des Hakens. Messung nachher: s. §8.7 (Lauf K).

### 8.7 Messungen nachher

**Fortsetzung 2026-10-24 00:07 (neuer Agent nach dem Fable-Limit 23:45):** Baum sauber, HEAD 8b8c9b5b (= 591041e9
UNGEPRUEFT + AUFTRAG/VERTRAG vom master). Bau: `local_build.sh configure` + `build` -> "ninja: no work to do" (exe
23:45 = Stand 591041e9); danach die 6 Spur-B-Quellen (gator_re2_r41.c, enemy_ai_boss_gator.c, gator_re2_pc.c,
re2_fx.c, enemy_ai_re2_zombie.c, test_r41_re2gator.c) per touch neu uebersetzt: 782 Schritte, `LOCAL-BUILD-OK`,
EXIT 0, keine Warnung -> 591041e9 kompiliert.

Gezielte Tests am Stand 591041e9 (Bau 00:08):
- `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_.*|re2_hit_repeat_crow_spider|re2_crow_.*|crow_ai|crow_fixes|crow_death_corpse|r36_schuss|r36_schuss_kraehe|r36_kleidung_kraehe|r35_fenster_kraehe|r37_re2zombie_.*|r34_re2fx.*|montage_fx|r35_cut1150_montage.*|r38_montage.*|r38_wasser.*|r38_wasserzombie|r39_heliport1171_kraehen|r30_granate|r35_granate|r36_granate)$|^probe_(re2z_stomp|crow_kill)$"`
  (= Regex der Abnahme 0, jetzt inkl. unit_r41_re2gator_wand): **100% tests passed, 0 tests failed out of 88** (7,34 s).
- `ctest -R "^integration_r41_re2gator$"`: **100% tests passed, 0 tests failed out of 1** (35,76 s); Statuszeile des
  Riegels (natuerlicher Ablauf, Leon nach dem Sprung in der Tuernische, GB_TEST=1, 700 Bilder):
  `r41_re2gator: ok — Kanister gesetzt, RE2-Zeilen 11 (INTRO 1/1, DASH 1/1, Putz 6 x 9 = 54), Clip 6/2/3 Bilder 125/150/300, HP min 25300`.
  gator_boss.log dieses Laufs (lauf_c01305e9):
  `RE2 INTRO start pos=(-7200,-9200) yaw=1089 bahn_ende=(-7847,-15498)` / `RE2 INTRO ende pos=(-7416,-17140) yaw=2113` /
  `RE2 DASH start pos=(-7300,-18109) yaw=1024 putz_um=(-8438,0,-25538)` / 6 x `RE2 PUTZ runde r/6 spawns=9 ...
  um=(-8438,0,-25538) letzter=(-9060,-2400,-25588)` / `RE2 DASH ende pos=(-7416,-22300) yaw=2048`.
  -> M1: Intro (Clip 6 -> 3) und Dash (Clip 2 -> 3, auf der Westwand-Klemmlinie nach Sueden) zuenden im normalen
  Ablauf; M2: 54 Putz-Spawns um Leons Stand (-8438,0,-25538), letzter Spawn = Leon + (-622,-2400,-50) (k_putz[8]).

**Fortsetzung 3 2026-10-24 02:5x-04:xx (neuer Agent nach dem Sitzungslimit ~01:00):** Baum sauber, HEAD a6653809 (UNGEPRUEFT).
`local_build.sh configure` + `build`: "no work to do" (exe 00:46:52 = Stand a6653809); die Spur-B-Quellen per touch neu
uebersetzt: 779 Schritte, keine neue Warnung -> a6653809 kompiliert; unit-Regex (89) und integration_r41_re2gator gruen.
Danach am ECHTEN Ablauf gemessen (§9.4) und drei Befunde behoben: (1) Kamera-Wahl ohne Massstab-Grenze -> EXPLOSION mit
dem Kopf 3201 vor der Linse (74f247c1, §9.5); (2) R1-Zeitlupe VSync 4 (vormals OFFEN, 6592c742, §9.7); (3) Putz am
Ursprung statt um Leon und nie gezeichnet (fb8ed4da, KORREKTUR unter M2). Endstand-Messungen:
- Lauf KA (GB_TEST=1, Tuernische): Kamera-Folge DURCHBRUCH F6 Cut 7 / SEITE F76 Cut 0 / ANGRIFF F130 Cut 7 / zurueck + frei F280
  / FINISHER F657 Cut 0 (§9.4); INTRO/DASH/Putz wie oben (`RE2 DASH start pos=(-7300,-18109) yaw=1024 putz_um=(-8438,0,-25538)`).
- Lauf putz3 (dto.): Putz-Teilchen um Leon, 27..98 Quads/Bild gezeichnet (M2).
- Lauf KK (GB_TEST=5): R1 F70 -> Cut 1 (EXPLOSION), zurueck F106, frei F108; VSync 4 F70..F108 = 66,6 ms/Bild; debug.log
  `[gator-re2] Raum-SE 15 -> GATOR40A0.VBS Satz 1` (M6).
- Lauf KF: FINISHER-Schnitt bis ~F738, dann RE1.5-Todeskamera.

### 8.8 Abschluss Nachbesserung 1 (je Mangel)
| Mangel | Ursache | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| M1 Dash/Intro zuenden nicht | absolute Bahn + Bahn-Probe gb_bahn_frei auf der Klemmlinie | Clip-Geometrie §8 M1, Zonenlinie `slti v0,v0,-17877` @0x80100990-98 / `-7322` @0x801009d8-e0, `yaw += 1024` @0x80100eb0-c0 / @0x80100938-48 | bildweise Wurzelbahn durch die Dispatcher-Klemme, Bahn-Probe weg, yaw += 1024 | INTRO 1/1, DASH 1/1 im exe-Riegel; Clip 6/2/3 = 125/150/300 Bilder |
| M2 Putz nicht belegt | haengt an M1; Lage nie gesetzt; Stack-Matrix + Lage in der Translation (Teilchen am Ursprung) | ROOM40A0 sub13/sub14 §2.4, Sce_espr_on @0x800565a4-0x80056618, Einheitsmatrix 0x8009db44 | Lage um Leon; Einheitsmatrix + Versatz wie RE2; dauerhafte Quell-Matrizen | 54 Spawns; Teilchen um Leon (<= 1560 xz); 27..98 Quads/Bild gezeichnet; Bild putz_m2_vorher_nachher_cut0.png |
| M3 gemeinsame Dateien | - | Entscheid B5 | angenommen; Fortsetzung 3: +1 Haken main.c Bild-Deckel (Zeitlupe) | - |
| M4 falsche Erklaerung | - | state.log Lauf A | §4.3 korrigiert | - |
| M5 GLOB+REMOVE | - | - | nur die eigene exe-Kopie | - |
| M6 Raum-SE 15 | kein Logpfad | Se_on 0x020f0001 @0x8010237c-88 | debug.log-Zeile in gator_re2_pc.c | `[gator-re2] Raum-SE 15 -> GATOR40A0.VBS Satz 1` (Lauf KK) |
| Nachtrag 4 Kameraschnitte | - | §9.1/§9.2 | Programme INTRO/R1/FINISHER, Wahl mit Massstab-Grenze | §9.4: Folge + Bildnummern im exe-Riegel gepinnt |

## 9. Nachtrag 4 — RE2-Kameraschnitte des Alligator-Kampfes (ROOM40A0) auf ROOM2090/2091

NUTZER-VORGABE (AUFTRAG.md 22:37, "Ja, ich moechte RE2-artige Kameraschnitte"): die Schnittfolge des RE2-Kampfes
(Durchbruch, R1, Finisher, Kanister-Aufnahme, Explosion) belegen und je RE2-Schnitt die 2090-Kamera mit gleicher
Blickbeziehung waehlen. Form = RE2 (Ausloeser, Bild, Dauer, Sperre), Zuordnung = PORT-WAHL.

### 9.1 RE2-Mechanik der Schnitte (selbst gelesen)
- Schnitt = FUN_8002c7ac(cut): `DAT_800cfbf2 = cut; DAT_800ce33c = FUN_8002c7dc()` (RE2_Quellcode_V2/FUN_8002c7ac.c;
  FUN_8002c7dc sucht die RVD-Gruppe des Cuts, Stride 0x14). Apply FUN_8002b968 -> FUN_8002bdf4: `DAT_800d4824 =
  DAT_800d4820; DAT_800d4820 = DAT_800cfbf2`, RID-Satz `*(rdt+0x24) + cut*0x20` (RE2_Quellcode_V2/FUN_8002bdf4.c:19-20,
  :74-75). **[0x800d4820] ist der AKTUELLE Cut** (G.Cut_no) — KORREKTUR zu §2.5: "Nahkamera 5 nur fuer Waffe 6/13"
  war falsch gelesen, es heisst "nur wenn der aktuelle Cut 6 oder 13 ist"; ebenso R2/R3 "[0x800d4820] == 8" =
  aktueller Cut 8 (@0x80101f30-3c `lui v1,0x800d / lh v1,18464(v1) / bne v1,v0(8)`).
- Zonen-Kamera (RVD) laeuft nur bei geloeschtem Bit 0x100 von 0x800cfbd8: Hauptschleife
  `lw v0,0x0(s0)=>DAT_800cfbd8 / andi v0,v0,0x100 / bne v0,zero,LAB_800263b0 / jal FUN_8002c6b4 / _clear a0`
  @0x80026394-ac (ghidra_re2_Leon.txt Z. 107189-107194). Bit 0x100 = Kamera-Sperre (= RE1.5 DAT_800aca3c Bit 0x100,
  Port g_scd.cut_auto_enabled = 0). Bit 0x10 = Kino-Balken (FUN_8002c378: DAT_800dfd55 +-0x10 je Bild bis 0xF0,
  zwei Balken-Prims; RE2_Quellcode_V2/FUN_8002c378.c:40-58).
- Zensus ALLER Schnitte des Overlays (`re2_disasm.py dis 0x80100000 4666 --bin EM23_OVL_0000.BIN`, grep
  `jal 0x8002c7ac`): genau 9 Rufe —
  | Adresse | Routine | Cut | Ausloeser (Bytes) |
  |---|---|---|---|
  | @0x801008a0 | Sub 0 INTRO P1 | 0 | `addiu v0,zero,70 / bne v1(+0x14D),v0 / jal 0x8002c7ac / addu a0,zero,zero` @0x80100894-a4: im Tick, in dem Clip-6-Bild 70 gezeigt wird |
  | @0x801008c4 | Sub 0 INTRO P1->P2 | 1 | `jal 0x8001a330 (Clip 6)` == 1 -> `jal 0x8002c7ac / addiu a0,zero,1` @0x801008bc-c8 (letztes Clip-6-Bild), dazu z = -21212 |
  | @0x80100ff4 | FRESSEN P0 | 7 bzw. 14 | `ori v0,v0,0x100` (Sperre) @0x80100fcc; 14 nur bei 0x800cfbd8 & 0x40000000 und Flag(a0 = 0x800cfbd8 + 0x4e54 = 0x800d4a2c, Bit 16; `addiu a0,a0,20052` im Delay-Slot @0x80100fdc) @0x80100fd4-f0, sonst `addiu a0,zero,7` @0x80100ff0 |
  | @0x80102268 | R1 P0 | 5 | `ori v0,v0,0x100` @0x8010220c; nur wenn aktueller Cut 6/13 (`addiu v0,zero,6 / beq v1,v0` / `addiu v0,zero,13 / bne v1,v0` @0x80102218-24) UND z < -13300 (`slti v0,v0,-13300` @0x80102234); dann +0x218 \|= 0x40, `jal 0x8002c7ac / addiu a0,zero,5` @0x80102268-6c |
  | @0x801024ec | R1 P1 (Clip-9-Ende) | 6 bzw. 13 | nur wenn +0x218 & 0x40 (`andi v0,v1,0x40` @0x801024ac): zurueck auf den Ausgangs-Cut @0x801024b8-ec |
  | @0x80102784 / @0x80102880 | R2 | 9, dann 8 | Tod durch Beschuss bei offenem Maul in Cut 8 — im Port nicht gebaut (Entscheid B2) |
  | @0x80102a7c / @0x80102aec | R3 | 7/14, dann 8 | dito |
  KEIN Schnitt in: T2 Kanister-Aufnahme (0x801017c4-0x80101d40), Sub-1-Pre mit Kanister-Fenster (0x80100964-0x80100c1c),
  Sub 3 Dash (0x80100d30-0x80100f34), R4 Grossreaktion (@0x80102bd0), R0. Sperre-Ende: Intro P2 `and v0,v0,-17 / and
  v0,v0,-257` @0x801008fc-918 (Bits 0x10/0x100 frei am Clip-3-Ende), R1 P3 `addiu v1,zero,-257 / and v0,v0,v1 / sw v0,0x800cfbd8` @0x80102688-94 (NUR Bit 0x100 frei; §2.5 nannte "&= ~0x101" - korrigiert). Zusatzbefund R1: `addiu v0,zero,4 / sb v0,0x800dfc1a` @0x8010235c-64 = VSync(4) statt 2 (FUN_8002b968 `VSync(DAT_800dfc1a)`) = halbe Bildrate (Zeitlupe) von P0 bis P3, P3 setzt 2 zurueck @0x80102668/7c (§9.7, in Fortsetzung 3 umgesetzt).
- SCD ROOM40A0 (Walker re15_port/tools/r41_re2gator/re2_scd_walk.py, Laengen information73.txt): Cut_chg NUR in sub02 = Durchbruch-Szene
  (Ausloeser AOT 5 `Aot_set_4p 67 05 05 41 ... ff 00 18 02` @SCD-main 0x04CA, sat 0x41 Betreten, einmalig ueber
  Flag 4/0x33 @0x00AE): `29 0a` @0x00CC (Cut 10) -> warten auf 5/0x0b (Leons Satz: sub03 Message_on 7 + Xa_on 0x20
  @0x015C-62, `Set 22 05 0b 01` @0x0169) -> `29 0b` @0x00DA (Cut 11) -> `Sleep 0x1e` @0x00DC (30 Bilder) -> `29 0a`
  @0x00E0 (Cut 10) -> `Sleep 0x3c` + `Sleep 0x12` @0x00E2/E6 (78 Bilder) -> `Evt_exec sub31/sub11/sub12` @0x00EA-F2
  (sub11 `Member_set2 35 17 10` @0x0730 = +0x1D4 des Gators -> INTRO P1 startet Clip 6 IN Cut 10). sub03 wartet
  danach `While Cmp var 0x1a != 1` @0x025E-62 auf den Overlay-Cut 1. Cut_replace 6->13 / 7->14 (`4b 06 0d` / `4b 07 0e`
  @0x0850/0x0853 in sub13 bzw. @0x040A/0x040D in sub05) tauscht nur die Hintergruende nach dem Putz (Schutt im Bild,
  Belegbild 9.2), kein Schnitt. Kein Cut_auto/Cut_old/Cut_be_set in ROOM40A0.

**RE2-Schnittfolge des Kampfes (Form):**
1. Durchbruch: [Szene: Cut 10 -> 11 (30 B.) -> 10 (78 B.)] -> Clip 6 Bild 0..69 in Cut 10 -> Bild 70: Cut 0 ->
   Clip-6-Ende (Bild 124): Cut 1 -> Clip 3 (150 B.) in Cut 1 -> Clip-3-Ende: Sperre + Balken aus (Zonen-Kamera).
2. Verfolgung/Dash/Grossreaktion/Kanister-Aufnahme: keine Schnitte (Zonen-Kamera).
3. Schuss -> Explosion R1: Sperre; Cut 5 nur, wenn die aktuelle Kamera (6/13) den Gator nicht zeigt; nach Clip 9
   (37 B.) zurueck auf 6/13; Sperre aus im Endzustand P3.
4. Finisher (toedlicher Biss, FRESSEN P0): Sperre + Cut 7 (14 nach dem Putz), kein Ende (Game Over).

### 9.2 Blickbeziehung je RE2-Schnitt (gemessen, Probe re15_port/tools/r41_re2gator/probe_kamera_re2.c)
Gator-Lage aus den Belegen: Intro = Pos_set sub11 (4557,-1800,-18237) @0x0736, yaw 0x400 (Sce_em_set @0x25A0), Wurzel(f)
= Pos + R*off6[f] (FUN_80015e7c/152c8), Clip 3 ab (x, -21212) @0x801008d0; R1 = Bein 2 x -23078, Kanister (-22816,-9500)
im Fenster 8601..9000 vor der Wurzel; FRESSEN = x -23078 @0x80101048, Leon (x+10643, -915) @0x8010106c-84. Kopf =
Bone 6, Blickachse = Bone 1 -> Bone 6 (xz) der EM23-Pose (Scale 1). "frontal" = cos(Blickachse, Kopf->Kamera).
| RE2-Schnitt | Bild | Kopf im Bild (x,y) | Tiefe | dist xz | frontal | Lesart |
|---|---|---|---|---|---|---|
| Cut 10 (Szene) | Clip 6 f0 | (140,126) | 11734 | 10890 | +1.00 (3 Grad) | Gator bricht frontal auf die Kamera zu |
| Cut 10 | Clip 6 f69 | (142,144) | 4188 | 3756 | +1.00 | naht bis 3700 heran |
| Cut 0 | Clip 6 f70 | (145,116) | 8898 | 8878 | +0.35 (69 Grad) | seitlich von vorn (quert das Bild) |
| Cut 1 | Clip 6 f124 | (183,109) | 16318 | 16379 | +0.83 (34 Grad) | Angriff: dreht in Clip 3 voll auf die Kamera (f110/148: +0.99) |
| Cut 5 | R1 Clip 9 f0 | (170,111) | 13113 | 12983 | +0.99 (9 Grad) | Explosion frontal; im Ausgangs-Cut 6 lag der Kopf bei x = -2 = AUSSERHALB |
| Cut 7 | FRESSEN P0 (Clip 4 f13) | (204,118) | 11175 | 11227 | +1.00 (2 Grad) | Finisher frontal, Leon VOR der Kamera im Bild (186,186), Tiefe 5040 |
Belegbilder der RE2-Hintergruende: info/re2leon/COMMON/BSS/ROOM40A/ROOM40A{10,11,00,01,05,06,07,13,14}.bmp
(Bildbogen bilder_r41b/re2_40a0_cuts_10_11_00_01_05.png und re2_40a0_cuts_06_07_08_13_14.png, halbe Groesse, Reihenfolge wie im Dateinamen: Cut 10 Durchbruchstelle mit Muellhaufen, Cut 11 Leiter/Gelaender,
Cut 0/1 Korridor Bein 1, Cut 5 Bein 2 nach Sueden, Cut 6/7 Bein 3, Cut 13/14 = 6/7 mit Schutt).

### 9.3 Umsetzung (a6653809 Kamera-Programme, 74f247c1 Massstab-Grenze, Fortsetzung 3)
Dateien: `engine/src/gator_re2_r41.c` (Kamera-Programme, Wahl, Messzeilen), `include/re15_gator_re2_r41.h` (API),
`engine/src/enemy_ai_boss_gator.c` (1 Haken in FRESSEN P0 -> `re15_gr2_kamera_finisher`, nach der Opfer-Platzierung),
Riegel `unit_r41_re2gator_kamera` + Kamera-Pins in `integration_r41_re2gator`.
- Sperre der Zonenkamera = `g_scd.cut_auto_enabled = 0` (RE2 `0x800cfbd8 |= 0x100`, Hauptschleife `andi v0,v0,0x100 / bne`
  @0x8002639c-a0; RE1.5 DAT_800aca3c Bit 0x100). Schnitt = RE1.5-Cut_chg-Zwilling (scd_vm.c op_cut_chg: cam_id_prev,
  cam_id, cam_change_pending, work_vars 0x0A/0x0C @0x800402c0-fc). Rueckkehr = Cut_old-Zwilling (FUN_8004032c) auf den
  Zonen-Cut VOR dem Programm, frei = `cut_auto_enabled = 1`.
- INTRO: DURCHBRUCH beim Intro-Start (Clip 6 Bild 0 = RE2 Cut 10, SCD sub02 `29 0a` @0x00E0), SEITE bei Clip-6-Bild 70
  (`addiu v0,zero,70 / bne v1,v0` @0x80100894-98), ANGRIFF am Clip-6-Ende (@0x801008bc-c8), zurueck + frei am Clip-3-Ende
  (`and v0,v0,-17 / and v0,v0,-257` @0x801008f8-918).
- R1: Sperre (`ori v0,v0,0x100` @0x8010220c); Schnitt EXPLOSION nur, wenn der aktuelle Cut den Kopf NICHT zeigt (RE2:
  aktueller Cut 6/13 @0x80102218-24 und z < -13300 @0x8010222c-34 = der Fall "Kopf ausserhalb", Probe: Bild-x -2 in
  Cut 6), dann +0x218 |= 0x40 (@0x80102240-4c); zurueck am Clip-9-Ende (@0x801024a4-ec); frei in P3 (@0x80102688-94).
- FINISHER: Sperre + Schnitt bei FRESSEN P0 (@0x80100fbc-ff4), kein Ende (RE2: Game Over).
- Kein Schnitt in Dash, R4, R0 und Kanister-Aufnahme (T2 0x801017c4-0x80101d40) - Zensus der 9 Rufe (§9.1).
- Messzeilen (gator_boss.log): `RE2 KAMERA <Programm> <Blick> RE2-Cut c -> Cut k F<n> clip= bild= frontal= soll= massstab=
  re2_m= grenze=`, `RE2 KAMERA <Programm> zurueck/frei ...`, je Schnitt alle Zonenkameras als `RE2 KAMERA-KANDIDAT ...`
  (Kopf-Bildlage, Massstab, frontal, Abstand zum Soll). Messhaken `re15_gr2_kamera_folge` (Riegel).
- Fortsetzung 3 (Mangel aus der exe-Messung §9.4 am Stand a6653809): Massstab-Grenze in der Wahl (§9.5).

### 9.4 Messung nachher (exe, eigene Kopien re15_pc_r41b_kam_*.exe, Basis-Umgebung wie §4.3 / Abnahme 0)
**Lauf KA** (`RE15_GB_TEST=1`, Leon in der Tuernische (-8200,-25550), `RE15_EXIT_AT=700#2090`, `RE15_FRAMEDUMP=0-700/1`),
gator_boss.log (gleich in 3 Laeufen und im Riegel integration_r41_re2gator):
```
RE2 KAMERA INTRO DURCHBRUCH RE2-Cut 10 -> Cut 7 F6 clip=6 bild=0 frontal=4098 soll=4092 massstab=661 re2_m=1161 grenze=1892
RE2 KAMERA INTRO SEITE RE2-Cut 0 -> Cut 0 F76 clip=6 bild=70 frontal=1876 soll=1445 massstab=679 re2_m=1892 grenze=1892
RE2 KAMERA INTRO ANGRIFF RE2-Cut 1 -> Cut 7 F130 clip=6 bild=124 frontal=1397 soll=3386 massstab=1150 re2_m=927 grenze=1892
RE2 KAMERA INTRO zurueck -> Cut 0 F280 clip=3 bild=149
RE2 KAMERA INTRO frei (Zonenkamera an) Cut 0 F280 clip=3 bild=149
RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 0 F657 clip=4 bild=0 frontal=-605 soll=4094 massstab=952 re2_m=1354 grenze=1892
```
state.log `cam=`: F1 0, F6 7, F76 0, F130 7, F280 0 (angeforderter Cut im selben Bild). Im Bild erscheint jeder Schnitt im
FOLGEbild (Framedump F6 noch Cut 0, F7 Cut 7; ebenso F76/F77, F130/F131, F280/F281). Das ist die RE2-Ordnung: FUN_8002b968
wendet den Cut erst am Bildende NACH dem VSync an (`jal VSync` @0x8002b998, `andi v0,v0,0x80` @0x8002b9e0, `lh v1,DAT_800d4820`
@0x8002b9f0 gegen `lbu v0,DAT_800cfbf2` @0x8002b9f8, `sb v0,DAT_800dfc1b` @0x8002ba0c, `jal FUN_8002bdf4` @0x8002ba50); das
Bild, in dessen Tick FUN_8002c7ac gerufen wurde, ist noch mit der alten Kamera gebaut.
Bildfolge (bilder_r41b/kamera_n4_re2_vs_port_je_schnitt.png): Cut 7 F7-F75 der Gator taucht links auf und kommt frontal auf die
Kamera zu (F20-F60); Cut 0 F77-F130 Gator seitlich im Hintergrund vor dem Gittertor; Cut 7 F131-F280 Clip 3; danach Leons
Zonenkamera Cut 0; der Dash F341-F640 laeuft ohne Schnitt (wie RE2).

**Lauf KK** (Kanister-Tod, Rezept §7: `RE15_GB_TEST=5`, Leon (6500,-25000), Kanister (4430,-26160), Pistole,
`RE15_EXIT_AT=260#2090`, OHNE `RE15_FORCE_CUT`): Leons Zonenkamera Cut 0 zeigt den Kopf nicht -> Schnitt (RE2-Fall Cut 6/13):
```
RE2 R1 VSync 4 (Zeitlupe) F70
RE2 R1 start pos=(4997,-20318) hp=25293
RE2 KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F70 clip=9 bild=0 frontal=1247 soll=4046 massstab=715 re2_m=1154 grenze=1892
RE2 KAMERA R1 zurueck -> Cut 0 F106 clip=9 bild=36
RE2 R1 ende (Leiche) pos=(3413,-25001)
RE2 R1 VSync 2 F108
RE2 KAMERA R1 frei (Zonenkamera an) Cut 0 F108 clip=9 bild=36
```
Bild F71-F106 (Cut 1, Totale aus der SW-Ecke): Blitz am Kopf (F71), weisse Fontaenen + rote Kiefer-Scherben (F73-F90), Leiche
mit offenem Maul (F100); F107 zurueck auf Cut 0. VOR der Grenze (Stand a6653809) schnitt die Wahl auf Cut 2: Kandidat
`Cut 2 kopf_bild=(273,178) massstab=2839` = Kopf 3201 vor der Linse, 2,5-facher RE2-Cut-5-Massstab; der Leib fuellte F71-F106
als Pixelblock (Lauf kam_k, gesichtet) -> §9.5. Kandidaten Lauf KK (Stufe 2 = Grenze aktiv):

| Cut | Kopf im Bild | Massstab (x 2^-16) | frontal | Abstand zum Soll |
|---|---|---|---|---|
| 0 | draussen | - | +3611 | - |
| 1 | (180,98) | 715 | +1247 | 2799 (gewaehlt) |
| 2 | (273,178) | 2839 > 1892 | +3395 | 651 (die Winkelwahl vor Fortsetzung 3) |
| 3 / 4 / 6 | (167,104) / (277,93) / (56,102) | 534 / 500 / 484 | -4007 / -3056 / -3056 | 8053 / 7102 / 7102 |
| 5 / 7 | draussen | - | - | - |

**Lauf KF** (wie KA, `RE15_EXIT_AT=925#2090`, Framedump 650-925/5; bilder_r41b/kamera_n4_finisher_dann_todeskamera.png):
FINISHER-Schnitt Cut 0 F658 bis ~F738 (Clip 4/5, Leon im Maul), F740 Weissblende, ab ~F745 die byte-true RE1.5-Todeskamera
(Orbit FUN_80015850, Memory reai-v2-death-camera) mit YOU DIED; Raumende F931 (Game Over). RE2 bleibt bis zum Game Over auf
Cut 7 - Entscheidungsfrage §7.

### 9.5 PORT-WAHL der Zuordnung (Form = RE2, Zuordnung = PORT-WAHL)
- Kandidaten: die ZONENKAMERAS von ROOM2090 (Cuts 0..7 = RVD-Gruppen mit Wechselsatz und Masken; 8..12 sind Sonderkameras
  ohne Zone/Masken). Bedingung: Kopf (Bone 6) im Bild (Projektion wie der Zeichner, re15_camera_build_view) UND Bildmassstab
  m = H x Skala / Kopf-Tiefe nicht groesser als der groesste RE2-Schnittmassstab; FINISHER zusaetzlich Kiefer (Part 7) im Bild
  (Opfer im Maul ab P1 @0x80101110-34). Wahl: kleinster Abstand |frontal - soll| (frontal = cos(Blickachse Bone 1 -> 6,
  Kopf -> Kamera), Q12). Stufen, falls leer: ohne Grenze, dann ohne Kiefer.
- Grenze = RE2-Messwert: m(RE2) = H / Kopf-Tiefe im Schnittbild (RE2 zeichnet EM23 1x), probe_kamera_re2.c (gcc -std=c11
  -DRE15_PLATFORM_PC=1 gegen libre15_engine.a + libre15_test_support.a; OHNE -DRE15_PLATFORM_PC=1 ist die Bank-Struktur
  verschoben und die Probe liefert falsche Clip-Laengen 45/26/165): Cut 10 208/11734 = 1161, Cut 0 257/8898 = 1892,
  Cut 1 231/16318 = 927, Cut 5 231/13113 = 1154, Cut 7 231/11175 = 1354 (x 2^-16; H = RID fov >> 7 = 26684/32946/29623 >> 7).
  RE2 schneidet an keiner Stelle naeher heran als Cut 0 (1892); die Grenze ist dieser Messwert.
- Ergebnis im Spiel (Laeufe KA/KK):

| RE2-Schnitt | RE2 frontal / m | Port-Cut | Port frontal / m | Lesart |
|---|---|---|---|---|
| Cut 10 DURCHBRUCH (Clip 6 f0) | +1.00 / 1161 | 7 | +1.00 / 661 | frontal, der Gator taucht auf und kommt auf die Kamera zu |
| Cut 0 SEITE (f70) | +0.35 / 1892 | 0 | +0.46 / 679 | seitlich (63 statt 69 Grad), Totale |
| Cut 1 ANGRIFF (f124) | +0.83 / 927 | 7 | +0.34 / 1150 | schraeg; RE2 dreht in Clip 3 auf die Kamera, im Becken dreht Clip 3 nach Westen (§9.8) |
| Cut 5 EXPLOSION (R1) | +0.99 / 1154 | 1 | +0.30 / 715 | seitliche Totale; die einzige frontale Kamera (Cut 2) laege 2,5-fach naeher |
| Cut 7 FINISHER | +1.00 / 1354 | 0 | -0.15 / 952 | seitlich; in der Tuernische steht keine Kamera vor dem Kopf (Cuts 1/7 bei x -7740 liegen 243/275 vom Kopf = im Leib) |

  Belegbild bilder_r41b/kamera_n4_re2_vs_port_je_schnitt.png: je Zeile der RE2-Hintergrund ROOM40A{10,00,01,05,07}.bmp mit
  dem RE2-Kopf (Probe) neben dem Port-Bild F+1 (Kreuz = Kopf beim Schnitt; x2,5 aufgehellt nur zur Sichtung) und einem
  spaeteren Bild desselben Schnitts.

### 9.6 Fuer Spur G (Nachtrag 5, Kanister-Wurf wie Adas Raketenwerfer-Wurf)
Die Wurf-Kamera ist Spur G. Sie nutzt dieselben Bausteine: `re15_gr2_kamera_schnitt(blick, gator, opfer, "Name")` (Sperre +
Cut_chg-Zwilling, liefert den Cut oder -1), `re15_gr2_kamera_frei("Name")` (zurueck + Zonenkamera an), `re15_gr2_kamera_wahl`
(nur waehlen), `re15_gr2_kamera_gesperrt()`, `re15_gr2_kamera_auto(0)` (die automatischen Programme INTRO/R1/FINISHER aus,
solange eine Szene selbst schneidet). Die Kanister-AUFNAHME hat in RE2 keinen Schnitt (T2 0x801017c4-0x80101d40); die
Explosion (R1) schneidet automatisch nach §9.3, wenn Leons Kamera den Kopf nicht zeigt.

### 9.7 R1-Zeitlupe (vormals OFFEN, jetzt umgesetzt)
RE2 (selbst disassembliert): die Hauptschleife wartet `VSync(DAT_800dfc1a)` (FUN_8002b968 `lui a0,0x800e / lbu a0,-0x3e6(a0) /
jal VSync` @0x8002b990-98); im Spiel 2 (`li v0,0x2 / sb v0,0x800dfc1a` @0x8003b9a8-b0, ebenso @0x80068e6c-74); R1 P0
`addiu v0,zero,4 / lui at,0x800e / sb v0,-998(at)` @0x8010235c-64, R1 P3 `addiu v0,zero,2` @0x80102668 + `sb v0,-998(at)`
@0x8010267c - die Explosion laeuft mit halber Bildrate. Port: `s_vsync` 2/4/2 an denselben Stellen (gator_re2_r41.c,
`re15_gr2_vsync()`), PC-Bildbudget `re15_gr2_pc_bildbudget` = Grundbudget x VSync / 2 (gator_re2_pc.c; 1 Haken am Bild-Deckel
in platform/pc/main.c, "Runde 41 Spur B"). Gemessen (Lauf KK): `[gator-re2] VSync 4 (Bildbudget 66 ms) ab F70 t=11117 ms` und
`VSync 2 (Bildbudget 33 ms) ab F108 t=13649 ms` = 38 Bilder in 2532 ms = 66,6 ms/Bild (sonst 33). Riegel
unit_r41_re2gator_kanister: "Zeitlupe VSync 4 ueber 38 Takte (R1 P0 .. vor P3)".

### 9.8 Beobachtungen / OFFEN (gemessen, nicht geaendert)
- Intro im Westkanal: der RE2-Korridor-Knick (Clip 3 Root-ry 438 -> 1023, danach `yaw += 1024` @0x80100938-48) dreht den Gator
  vom Lauerplatz (Yaw 1089, Sued) nach WESTEN (2113) - im Becken zur Westwand. Kopf (Bone 6) am Clip-6-Ende bei x = -9574
  (unit_r41_re2gator_kamera, ANGRIFF), Wandflaeche x = -8900; die Dispatcher-Klemme haelt nur die Wurzel. In Cut 7 ragt der
  Kopf (Bilder F160-F279) links ueber die Wand. Das 12,2-m-Modell (Bindpose 18313 x 2/3) im Kanal tut das bei jeder Wende; eine andere
  Startrichtung waere eine neue PORT-WAHL am abgenommenen M1-Ablauf -> Entscheidungsfrage §7.
- Finisher -> RE1.5-Todeskamera (Lauf KF): s. §9.4, Entscheidungsfrage §7.
- Der Port zeichnet den Gator 2/3 gross (Nutzer-Design v0.7.36, +0x166 = 2731): alle Port-Massstaebe liegen unter den RE2-Werten
  (0,36..1,24-fach); die Grenze verwirft nur zu NAHE Kameras.
