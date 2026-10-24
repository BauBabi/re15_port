# Runde 41 — Spur B "re2gator" — Dossier (fortlaufend)

Baum: `.claude/worktrees/r41_re2gator` (Zweig `r41/re2gator`, Basis master 116e8b0f).
Auftrag (AUFTRAG.md, Wortlaut): (1) RE2-Finisher + fehlende Animationen des Alligators extrahieren;
(2) Verfolgung wie RE2 (Animation, Schreie, Beschleunigung an gleichen Stellen), Waende wackeln + Putz
faellt, Klettern ueber die Plattform BLEIBT; (3) Kanister-Aufnahme + Sterbeanimation mit geplatztem
Kiefer GENAU wie RE2 (Mechanik mit Testhaken; die Szene baut Spur G).

Zeiten aus `date`. Jeder Schritt wird hier VOR dem Bau notiert.

## 0. Protokoll (neueste Eintraege oben)

* 2026-10-24 10:14-11:4x (NACHBESSERUNG 4, Abnahme 3 M1/M2): M1 Kanister + Effekt-Quellen aus der GEZEICHNETEN Pose am
  Takt-Ende (Takt-Huelle: Wand-Klemme bitgleich in den Boss-Takt, re15_gr2_nachtakt; RE2 Kind der Part-6-Weltmatrix
  @0x801018d0-e0), Nebenbefund RE2-Ueberblendung (Rate 4096/(n+1) @0x8001a37c-ac, Abbau @0x800299c0-cc; Dash war eingefroren);
  M2 Befreiung nur im echten Gefaengnis, als Schub ohne Drehung (PORT-WAHL; RE2 ohne Gier-Schreiber in Sub 1/2). Riegel sitz +
  gier (Gegenproben schlagen an), integration Laeufe kanister + belagerung, Messzeilen Kanister-Sitz / GIER-WACHE. §12.
* 2026-10-24 08:1x-09:3x (NACHBESSERUNG 3, Abnahme 2 M1-M3, Entscheid B9): M1 Gator vor dem Durchbruch untergetaucht wie RE2
  (Sce_em_set y = 3000 @RDT 0x25A0, Pos_set erst im Intro @SCD 0x0736) - Lauf N F1-F201 pixelgleich mit "ohne Gator",
  main.c +1 Haken; M2 keine Koerperlenkung von Stufe 1 bis zum Anhaengen (kein Gier-Schreiber in Sub 1/2/T2, Schub nur laengs
  @0x801018a4-c0), Stufe 1 im Fenster-Takt (@0x8010183c-5c -> @0x80101860), Quer-Ausgleich (PORT-WAHL) nach Messung C3;
  M3 Anschwimm-Sperre +0x1C0 & 4 (@0x80100a34-b4, T1 @0x8010134c-58), Hals-Zielmodus OFFEN mit Beleg. Riegel lauer/
  schleppen/anschwimmen + exe Lauf 3 "natuerlich"; Zensus r34 angepasst. §11.
* 2026-10-24 05:23-07:2x (NACHBESSERUNG 2, Abnahme 1 M1-M4, Entscheide B6-B8): M4 Belegpflege; M3 Kanister-Aufnahme
  wie RE2 (Sub 2 -> 1, Hysterese-Ruhe, kein Biss in Stufe 1..6); M1 b Finisher-Schnitt bis Game Over (RE1.5-Todesbild
  im Boss-Takt zurueckgenommen), M1 a mit RVD-Beleg widerlegt; M2 RE2-Wende im Becken (Intro-Startlage Pos_set-Zwilling,
  Dash-Vordrehung, Probe probe_wende.c). Riegel aufnahme NEU, wand/kamera/integration neu gefasst. §10.
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
  x<-17877/z>=-7322; ~~Dash/Intro nur bei freier Bahn (gb_bahn_frei); kein `yaw += 0x400` nach Clip 3~~ (UEBERHOLT
  seit Nachbesserung 1, Abnahme 1 M4: kein Bahn-Gate mehr, der Knick `yaw += 1024` ist RE2 @0x80100eb0-c0 /
  @0x80100938-48, §8 M1; Intro-Startlage Nachbesserung 2 §10); Putz-Bursts beim Dash-Start um Leon (RE2:
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
Entscheidungsfragen (gemessen, nicht geaendert) - ENTSCHIEDEN durch den Orchestrator (AUFTRAG.md 03:58, B6 = Frage 4,
B7 = Frage 5, B8 = Frage 6) und umgesetzt in Nachbesserung 2 (§10.3/§10.4):
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

## 10. Nachbesserung 2 (Abnahme 1, B_abnahme_1.md; Beginn 2026-10-24 05:23, HEAD 393bb59b)

Neuer Agent (Nachbesserung 2). Baum sauber (`status --porcelain` leer), HEAD 393bb59b (= c59802b5 + AUFTRAG-Entscheide
B6-B8 d53a6305 + Abnahmebericht). Pflichtlektuere: AUFTRAG.md (inkl. B6 Finisher-Kamera = RE2 bis Game Over, B7 Intro ohne
Wanddurchdringung, B8 seitliche Sicht bleibt), VERTRAG.md, B_abnahme_1.md (M1-M4), dieses Dossier §0-§9, Memory
reai-v2-gator-bosskampf.md / reai-v2-death-camera.md. Messdaten der Abnahme 1 liegen im selben Scratchpad
(`.../scratchpad/B41abn1/mess/<Lauf>/`, gator_boss.log/state.log/debug.log/fd) und werden NICHT wiederholt; eigene Laeufe
unter `.../scratchpad/B41nb2/`.

Plan: M4 (Beleg-Pflege) -> M3 (Kanister: Sub-2 -> Sub-1 + Hysterese-Ruhe, RE2 @0x80100964-0x80100c1c selbst lesen) ->
M1 (Finisher: sichtbarer Schnitt + Sperre bis Game Over, RE1.5-Todeskamera im Gator-Finisher aus, B6) -> M2 (Intro-/Dash-
Wende: PORT-WAHL der Startlage/-gier mit Rechnung, B7; Riegel auf die Kopflage Bone 6) -> Bau -> gezielte Tests -> exe-
Messungen nachher -> Abschluss-Commit.

### 10.1 M4 (Beleg-Pflege) — erledigt
- re2_fx.c Op 73: Tafelplatz selbst nachgelesen (re2_disasm.py read, info/re2leon/PSX.EXE): `0x8009d98c: 0x8002387c`
  (= Op-Tabelle @0x8009d868 + 73 x 4) und `0x8009d99c: 0x80023c70` (= Platz [77]); Op 11 `0x8009d894: 0x8001e4b4`.
  Kommentar korrigiert auf @0x8009d98c.
- gator_re2_r41.c Kopf + Dossier §3.2: die ueberholten PORT-WAHLEN ("kein yaw += 0x400", "nur bei freier Bahn") als
  ueberholt markiert (Knick = RE2 @0x80100eb0-c0 / @0x80100938-48, kein Bahn-Gate seit Nachbesserung 1).
- test_r41_re2gator.cmake: Todesursache korrigiert (Abnahme-1-Lauf A, state.log selbst ausgewertet mit
  B41abn1/statelog.py: Leon-HP F57 100->80, F175 ->60, F387 ->40, F499 ->20 (Spinnen-Gift), F656 20 -> -30 = BISS,
  FRESSEN ab F657).

### 10.2 M3 (Punkt 3, Kanister-Aufnahme GENAU wie RE2) — behoben
- **Ursache:** gr2_kan_takt setzte beim Fenstertreffer nur die Stufe; re15_gr2_chase_clip kannte den Kanister nicht, der
  Nutzer-Design-Biss fragte die Stufe nicht ab.
- **Messung vorher** (Abnahme 1, Lauf C3, state.log selbst ausgewertet): Stufe 1/2 vor `[walk] F660`, Stufen 2..7 waehrend
  des Schnapps Clip 4 F660-F702 - der Gator beisst beim Aufnehmen.
- **RE-Beleg** (EM23_OVL_0000.BIN, Sub-1/2-Pre @0x80100964 selbst gelesen, `B41abn1/em23_full.dis` Z. 603-777):
  - Kanister-Block nur in Zone 1: `lbu v1,538 / addiu v0,zero,1 / bne v1,v0,0x80100b20` @0x801009c8-d0.
  - Liegt im Wasser (Raumflag Bank 5 Bit 0): `jal 0x80077360 / addu a1,zero,zero / beq v0,zero,0x80100b20` @0x80100ab8-c8;
    Fenster `addiu v0,v0,-8601 / sltiu v0,v0,0x18f / beq v0,zero,0x80100c08` @0x80100ae4-ec (verfehlt -> Ende OHNE
    Hysterese), Stufe != 0 -> `bne v0,zero,0x80100c08` @0x80100afc (Ende), Stufe 0 -> `sb v0(1),550(s0)` @0x80100b08 und
    bei Sub 2 `addiu v0,zero,257 / j 0x80100c04` @0x80100b14-18 -> `sw v0,4(s0)` @0x80100c04 = Sub 1 (Schwimmen). Der
    Dispatcher liest +5 nach dem Pre neu (`lbu v0,5(a0)` @0x801007b8) und ruft im selben Tick Sub-1-Main @0x80100c20 =
    `FUN_8001a330(em, 0, 32, 0)` @0x80100c34-40. Das Bit setzt ROOM40A0 sub10 einmal (`Set 22 05 00 01` @SCD 0x0712),
    ein `Set 22 05 00 00` gibt es im Raumskript nicht (re2_scd_walk.py) - die Ruhe gilt in Zone 1 also auch beim Tragen.
  - Hysterese-Sperre bei Stufe != 0/7: `lbu a0,550 / beq a0,zero,0x80100b38 / addiu v0,zero,7 / bne a0,v0,0x80100c08`
    @0x80100b20-30.
  - Kein Biss in Stufe 1..6: FUN_801012fc macht die Biss-Maschine T1 nur bei Stufe 0/7 scharf: `lbu v1,550(a0) / beq
    v1,zero,0x80101378 / addiu v0,zero,7 / bne v1,v0,0x80101380` @0x80101360-70 vor `sb v0(1),548(a0)` @0x8010137c.
- **Aenderung** (gator_re2_r41.c, Haken enemy_ai_boss_gator.c):
  - Fenstertreffer: Paddeln -> Schwimmen (`letzter_clip` 1 -> 0; der Clipwechsel Bild 0 / Blend 32 passiert im naechsten
    re15_gr2_chase_clip desselben Takts), Logzeile `KANISTER Stufe 1 ... sub 2 -> 1 F<n>`.
  - re15_gr2_hysterese_ruht: Stufe 1..6 (@0x80100b20-30) oder Gator in der Zone der Liegestelle, solange der Kanister im
    Raum ist (@0x80100ab8-c8/aec/afc). PORT-WAHL der Zone wie beim Dash: RE2-Zone 1 = das Korridorbein des Kanisters ->
    die Beckenzone (gb_zone) seiner Liegestelle (re15_gr2_zonen_quelle(gb_zone) beim Boss-INIT).
  - re15_gr2_biss_erlaubt vor allen vier Biss-Starts des Boss-Moduls (Not-Schnapp, Biss-Commit, GUARD-Hochbiss,
    CROSS-Biss).
- **Messung nachher:** Lauf C3 (eigene exe-Kopie, Rezept der Abnahme, `B41nb2/mess/C3`): `KANISTER Stufe 1 laengs=5868
  quer=1593 sub 2 -> 1 F1098`; state.log Clip 0 F1096-F1162 (Stufen 1..6 liegen darin, kein Clip 4), erster Biss erst
  F1163 bei Stufe 7 (RE2 erlaubt ihn dort), danach in der Kanister-Zone weiter Schwimmen (Clip 0 F1206-F1289). Bildbogen
  `bilder_r41b/nb2_m3_aufnahme_schwimmen_C3_x2.png` (2x aufgehellt): F1090 Kanister liegt, F1098-F1112 Aufnahme, F1155/
  F1199 Kanister im Maul. Lauf K (Kanister-Tod, Rezept Dossier §7) unveraendert: Stufe 1 F6, R1 F70 (Cut 1), Leiche,
  VSync 2 F108.
- **Riegel:** NEU unit_r41_re2gator_aufnahme: `Paddeln 12/12 -> Stufe 1 Clip 0 Blend 32, Stufe 1..6 39/39 Clip 0, Biss 0,
  Stufe 7 Zone 0 30/30 Clip 0, Zone 2 Paddeln 12/12, Biss Stufe 7 nach 0 Takten`.

### 10.3 M1 (Nachtrag 4, Finisher-Kamera) — (b) behoben, (a) mit Beleg widerlegt
- **Messung vorher** (Abnahme 1 Lauf A, gator_boss.log Z. 82-90 selbst gelesen): FINISHER waehlte den aktiven Cut 0
  (frontal -605); alle anderen Zonenkameras zeigten den Gator von HINTEN (Cut 3 -3932 mit Kopf am Bildrand x 316, Cut 6
  -4050) oder lagen ausserhalb/zu nah (Cut 1/2/4/5/7). F735-F745 Weissblende, ab F750 RE1.5-Todeskamera.
- **RE-Beleg:** FRESSEN P0 (Sub 4 Main @0x80100f58, selbst gelesen): `lui a0,0x800d / addiu a0,a0,-1064 / lw v0,0(a0) /
  ori v0,v0,0x100 / sw v0,0(a0)` @0x80100fbc-d0 (Sperre), Cut 14 nur bei `0x800cfbd8 & 0x40000000` und Flag Bit 16
  (@0x80100fd4-ec), sonst `addiu a0,zero,7` @0x80100ff0, `jal 0x8002c7ac` @0x80100ff4. Sub 4 (@0x80100f58-0x801011fc)
  gibt Bit 0x100 nie frei - die vier `addiu v1,zero,-257` des Overlays stehen in Intro P2 @0x80100910, R1 P3 @0x80102688,
  R2 @0x801028ac, R3 @0x80102b1c. FRESSEN wird nur aus Sub 7 erreicht (`addiu v0,zero,1025 / sw v0,4(s0)` @0x801012e0-e4,
  Zone 2 = Korridorende) und stellt Leon auf (Gator-x + 10643, -915) @0x8010106c-84.
- **(a) Widerlegung "Wahl darf den aktiven Cut nicht als Schnitt zaehlen":** RE2 schneidet beim Finisher auf die Kamera
  der INSZENIERTEN Lage, nicht zwingend auf eine andere: Leons Finisher-Lage (-12435,-915) liegt in ROOM40A0 in der RVD-
  Zone "von Cut 6 nach Cut 7" (Satz @RDT 0x3EC) und in keinem Wechselsatz "von Cut 7" (nur im Such-Anker @0x400) - Cut 7
  IST dort Leons Zonenkamera (gemessen mit `re15_port/tools/r41_re2gator/rvd_40a0.py info/re2leon/PL0/RDT/ROOM40A0.RDT`,
  RVD = Offset-Tabelle [8] @RDT 0x25C, 36 Saetze, Punkttest wie re15_aot_point_in_quad). FUN_8002c7ac(7)
  aendert das Bild also nur, wenn Leon aus dem Bereich von Cut 6 kam. Die Port-Form (beste Zonenkamera mit Kopf + Kiefer
  unter der RE2-Massstabsgrenze, frontal am naechsten, Entscheid B8 "seitliche Sicht bleibt") nimmt den aktiven Cut nur,
  wenn er der beste ist; im Lauf der Abnahme waere jede andere Kamera eine Rueckansicht gewesen (s.o.). Mit der Intro-Lage
  aus M2 faellt der Biss jetzt frueher und anders: Lauf A (Nachbesserung 2) F403 FINISHER Cut 0 -> **Cut 6** (frontal
  +1666 = +0,41, Kopf (222,106) im Bild) - ein sichtbarer Schnitt.
- **(b) Ursache + Aenderung** (Entscheid B6): die RE1.5-Todes-Praesentation (game_step_common.c re15_gameover_fsm_tick =
  FUN_8001500c) startet ab dem Fress-Latch und setzt Weissblende (sub0/1, FUN_800217b0/80021880), Schwarzgrund
  (FUN_80021634(2,0)) und die kreisende Kamera (FUN_80015850, main.c g_death_cam). gr2_kam_finisher_halten (im Boss-Takt,
  der in re15_game_step NACH der FSM laeuft: FSM @game_step_common.c:1137-1138, Entity-Schleife am Ende) nimmt diese drei
  Werte (+ g_death_glow) zurueck, solange die FRESSEN-Sperre steht, und haelt cut_auto_enabled = 0. Keine neue Zeile in
  einer gemeinsamen Datei. YOU-DIED-Buchstaben, Schluss-Abblende und Titel = Game-Over-Folge des Ports bleiben.
- **Messung nachher** (Lauf A, `B41nb2/mess/A`, EXIT_AT 672 vor dem Game Over ~F677): state.log cam= F403..F672 = 6 ohne
  Wechsel; 13 Zeilen `RE2 KAMERA FINISHER Halt Cut 6 F404 .. F664` (je 20 Bilder, B6-Messung);
  `KAMERA FINISHER haelt Cut 6: RE1.5-Todesbild unterdrueckt (cam=0 weiss=1 schwarz=0) ab F467` (die Weissblende waere ab
  F467 angelaufen, Schwarzgrund/Orbit wurden ab F494/F495 gesetzt und sofort zurueckgenommen). Bildbogen
  `bilder_r41b/nb2_m1_finisher_halt_cut6_je20.png` (F400 Cut 0, F404-F672 Cut 6 je 20 Bilder): Gator mit Leon im Maul,
  YOU DIED ab ~F520 ueber dem Finisher-Bild, Abblende F660-F672, keine Weissblende, kein Orbit. Elza (ROOM2091, Lauf EZ)
  identisch (Cut 6 ab F403, 13 Halt-Zeilen).
- **Riegel:** unit_r41_re2gator_kamera (c) `FINISHER-Halt: 400/400 Takte Cut 0 gesperrt, RE1.5-Todesbild (Orbit/Weiss/
  Schwarz) aus`; integration_r41_re2gator Lauf nische `Finisher Cut 6 ab F403 gehalten 248 Bilder / 13 Halt-Zeilen`.

### 10.4 M2 (Punkt 2, RE2-Wende in die Westwand) — behoben (Entscheid B7)
- **Ursache:** Clip 3 dreht rechts um 90 Grad (Root-ry 438 -> 1023, danach `yaw += 1024` Intro @0x80100938-48, Dash
  @0x80100eb0-c0) - in ROOM40A0 die Kurve in das naechste Korridorbein. Vom Lauerplatz (-7200,-9200) mit Gier 1089 (Sued)
  und am Suedausgang des Westkanals (Gier ~1024) zeigt dieselbe Rechtswende im Becken in die Westwand.
- **Messung vorher** (Probe `re15_port/tools/r41_re2gator/probe_wende.c` gegen libre15_engine.a, rechnet die Bahn wie
  gr2_wurzel_schritt/gr2_clip_tick; gegen Abnahme 1 Lauf A geeicht: Kopf am Clip-6-Ende -10022 bei ungeklemmter Wurzel
  -7882 = mit der Klemme -7434 genau die gemessenen -9574): Intro vom Lauerplatz Kopf 4988 ausserhalb (Wand x -8900, Kopf
  x -13888 am Clip-3-Ende); Dash an den Startlagen der Abnahme (A (-7300,-18109) 1024 / C2 (-6767,-18110) 695 / N
  (-7247,-18259) 1059) Kopf 4340 / 1048 / 4504 ausserhalb. Bild vorher `bilder_r41b/nb2_m2_vorher_abnahme1_A_intro.png`
  (Abnahme 1).
- **RE-Form, die bleibt:** Clip 6 -> Clip 3 bzw. Clip 2 -> Clip 3 und `yaw += 1024` unveraendert. RE2 stellt den Gator VOR
  dem Intro auf eine feste Lage: ROOM40A0 sub11 `Pos_set 32 00 cd 11 f8 f8 c3 b8` @SCD 0x0736 = (4557,-1800,-18237), Gier
  0x400 aus Sce_em_set @RDT 0x25A0 - eine Lage, auf der die Rechtswende in den Korridor passt.
- **PORT-WAHL (Rechnung):**
  - INTRO (gr2_intro_lage, Pos_set-Zwilling, vom Boss-Modul beim Aggro VOR re15_gr2_intro_start gerufen): gesucht wird
    Startlage + Gier (Raster Gier 32, Ende-Abstand zu Leon 250 in [1000,24000]) mit (1) Rumpf 0/1, Hals 5, Kopf 6, Kiefer 7
    in jedem 3. und im letzten der 275 Bilder im Wasser (Beckengeometrie des Boss-Moduls gb_becken_draussen = ROOM2090-SCA
    [0]-[5]: Becken x[-8900,7200] z[-27000,-5400], Plattform x[-1700,1850] z[-20000,-12450], Rampe x[1850,7200]
    z[-18100,-14500]), Wurzel ausserhalb der Klemmzone 1466 (damit gilt die Bahn exakt), (2) End-Gier = Peilung vom echten
    Bahnende zu Leon (Toleranz 128, dann 256/512), (3) kein Rumpf-/Kopf-Bone naeher als 1350 an Leon (SEG[0].r 900 + 0x1c2)
    und Leon in keinem Bild in einem Koerperlaengen-Push-Kreis (k_gb_seg) - sonst setzt der Push g->maul_kontakt und der
    Nutzer-Design-Biss faellt im ersten CHASE-Takt (gemessen unit intro); RE2 liest die Intro-Sperre +0x238 = 110 nur in der
    Hysterese (@0x80100b5c, @0x80100bf4, @0x80100cb4), nicht in T1, (4) kleinste Kosten Startabstand + 4 x Gier-Drehung +
    16 x Toleranz. Der Schwanz (Bones 11-15) darf wie in der ganzen Nutzer-Design-Verfolgung an die Wand reichen (12,2-m-Tier
    im 7,2-m-Kanal). Die Bahn laeuft Bild fuer Bild wie gr2_wurzel_schritt (eine einmal gedrehte Summe wich ~180 ab). Der
    Sprung auf die Lage faellt in den DURCHBRUCH-Schnitt und den Posenwechsel Clip 6 Bild 0 mit Blend 0 (@0x801008ac-b8).
    Ergebnis-Speicher (32 Eintraege) fuer gleiche Eingaben (Sweep-Riegel: 3342 Paare, sonst 49-184 s statt 27 s).
    Rechnung der Regel (probe_wende.c Modi `kreis`/`regel`, `B41nb2/wende_intro_regel_final.txt`; die Probe dreht die
    Wurzelsumme einmal, das Spiel Bild fuer Bild - daher Abweichungen um ein Rasterfeld): Leon in der Tuernische
    -> Start (-7237,-13449) Gier 608 (Abstand 4249; im Spiel (-7345,-13277) Gier 576); Leon an der Osttuer (6300,-16250) -> Start 1170 vom Lauerplatz; Leon im
    Suedbecken Ost (2500,-25000) -> 566 vom Lauerplatz; Leon am Suedende des Westkanals (-8440,-18171) -> Start (5222,-19590)
    im Suedbecken Ost: die Rechtswende passt dort nur von Osten kommend (vom Lauerplatz aus endet jede freie Bahn ohne
    Beruehrung mindestens 45 Grad neben Leon, Modus `nah`).
  - DASH (gr2_dash_planen + Vordrehung): der Dash beginnt, wo der Zonenwechsel passiert (RE2 an der Zonenlinie
    @0x80100990-a8 / @0x801009d8-fc); gewaehlt wird die Start-Gier (Raster 16, hoechstens 0x400 neben der aktuellen) mit
    (1) und kleinster Drehung, die Ausrichtung auf Leon entscheidet nur bei gleicher Drehung (RE2: die Wende folgt dem
    Korridor, Abnahme 1 M2 "Dash-Yaw so, dass die RE2-Wende vom Rand weg zeigt"). Gedreht wird VOR Clip 2 mit 0x60/Bild (die
    "zackige Wende" der Nutzer-Design-Verfolgung, re15_enemy_steer_point(...,0x60)), dann SE 4 + Clip 2 wie RE2. Ohne
    zulaessige Gier kein Dash an diesem Zonenwechsel (Logzeile `DASH entfaellt`). Die Laengs-Probe gb_seg_frei ist durch
    diese bone-genaue Bahn ersetzt.
- **Messung nachher** (exe, eigene Kopien, Messzeilen `INTRO/DASH ende ... wand_draussen_max= kopf_x_min=` aus
  gr2_wand_messen = gezeichnete Bones mit Skala 2731):
  - Lauf A (Tuernische, GB_TEST=1): `INTRO Lage ... -> (-7345,-13277) yaw 576, End-Gier 1600, Peilung zu Leon Abw. |95|`,
    `INTRO ende pos=(-4506,-21480) yaw=1600 wand_draussen_max=0 kopf_x_min=-6889`; Kamera DURCHBRUCH Cut 0 (frontal 4101) ->
    SEITE Cut 1 -> ANGRIFF Cut 7 (frontal 3577, soll 3386; vorher 1397) -> frei F280. Bildbogen
    `bilder_r41b/nb2_m2_intro_nische_kopf_im_becken_x16.png` (1,6x aufgehellt): Gator taucht F40 frontal auf, F131-F279
    (Cut 7) Kopf und Leib im Becken, F300 Gator vor Leon.
  - Lauf N (natuerlicher Aggro F201, ohne GB_TEST): Start (5085,-19880) Gier 1536 -> End-Gier 2560 (Abw. 1), wand 0, Kopf x
    min -4965; DURCHBRUCH Cut 1: der Gator bricht hinter Leon im Suedbecken auf (`nb2_m2_intro_natuerlich_N_x16.png`).
  - Lauf D2 (Leon (-4000,-24000), Messhaken HP 100): Intro endet im Nordbecken (wand 0), Dash am Suedausgang
    `DASH Planung: an (-6052,-18113) yaw 1139 -> Start-Gier 624 (Drehung 515), End-Gier 1648, Abw. zu Leon 5`, Vordrehung 6
    Bilder, `DASH ende ... wand_draussen_max=0 kopf_x_min=-8642`, Putz 54 um Leon, 27..98 Quads/Bild gezeichnet
    (`nb2_m2_dash_vordrehung_D2_x16.png`: F487-F495 Vordrehung, F521-F601 Dash auf Leon mit Putzwolke).
  - Lauf D (Leon (2500,-25000)): Klettern ueber die Rampe (`CROSS start=(4450,-11900) ende=(4578,-20700)`) bleibt, Dash
    danach mit Drehung 5. Lauf EZ (Elza, ROOM2091): wie A.
- **Riegel:** unit_r41_re2gator_wand NEU gefasst: `intro Start (-7345,-13277) yaw 576 Clip6 125 Clip3 150 -> yaw 1600
  (Peilung Leon 1505, Abw 95), draussen 0, Kopf x min -6889 | dash Vordrehung 6 Bilder 1024 -> 512, ab f40 Clip2 150 Clip3
  150, Bahn (797,-5119), draussen 0, Kopf x min -8701, yaw 1536` (Bone-Lage mit Zeichen-Skala, re15_gr2_bone_welt_pub);
  integration_r41_re2gator: Lauf nische `Intro ... wand 0`, Lauf dash `DASH 1/1 wand 0`. Mit Beleg angepasst:
  probe_gator_fress (stellt den Gator nach dem Intro mit Blick auf Leon hin, weil der Intro jetzt mit Leon-Gier endet;
  Sitz dRef (-2,-1,-1)), unit_r41_re2gator_aufnahme (Biss-Pruefung ohne Zonenwechsel in Leons Zone).

### 10.5 Tests (gebauter Stand, keine volle Suite)
- `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_.*|re2_hit_repeat_crow_spider|re2_crow_.*|crow_ai|crow_fixes|crow_death_corpse|r36_schuss|r36_schuss_kraehe|r36_kleidung_kraehe|r35_fenster_kraehe|r37_re2zombie_.*|r34_re2fx.*|montage_fx|r35_cut1150_montage.*|r38_montage.*|r38_wasser.*|r38_wasserzombie|r39_heliport1171_kraehen|r30_granate|r35_granate|r36_granate|r36_kartenbits)$|^probe_(re2z_stomp|crow_kill)$" --timeout 600 -j 6`
  -> **100% tests passed, 0 tests failed out of 91** (114.62 s; unit_gator_sweep 114,53 s unter Last, einzeln 26,8 s).
- `ctest -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate)$" --timeout 900 -j 4`
  -> **100% tests passed, 0 tests failed out of 4** (190.83 s; r41_re2gator 85,96 s, r36_schuss 154,44 s, r35_granate
  177,28 s, r38_wasser 190,74 s).

### 10.6 OFFEN (gemessen, benannt)
- Intro-Lage fuer Leon am Suedende des Westkanals (natuerliches Annaehern aus der Tuernische) liegt im Suedbecken Ost
  (~16 000 vom Lauerplatz): der Gator bricht hinter Leon auf. Vom Lauerplatz aus gibt es fuer diese Leon-Lagen keine
  Rechtswende, die frei bleibt und auf Leon endet (probe_wende.c `nah`); die Regel stellt die Ausrichtung (B7) vor die Naehe.
- Dash: die Rechtswende endet bei Zonenwechseln, deren Korridor-Gegenstueck nach links zeigt, nicht auf Leon (Lauf D:
  Abw. 2039); RE2-Form, die Ausrichtung ist dort nur Gleichstands-Regel.
- Wie §9.8/§6: Raum-Effekt 0x26 fehlt, Frame-Wort-Latenz 1 Bild, Waffenschaden ueber die RE1.5-Zeile, Kanister im dunklen
  Becken kaum sichtbar, GUARD/CROSS spielen Clip 0.

### 10.7 Fuer den Nutzer (Nachbesserung 2)
- Keine neuen Assets, keine Sprachdateien. Gemeinsame Dateien: keine neue Zeile (M1 b laeuft ueber den Boss-Takt).
- Was sich im Spiel aendert: (1) der Alligator nimmt den Kanister schwimmend auf (kein Schnappen dabei) und schwimmt mit dem
  Kanister in der Zone, in der er lag, langsam weiter - Zeit fuer den Schuss; (2) der Durchbruch beginnt dort, wo die
  RE2-Wende ins Becken passt und auf Leon zeigt - der Kopf ragt nicht mehr in die Wand; (3) vor einem Dash dreht der Gator
  kurz (6 Bilder), damit die RE2-Wende vom Rand weg zeigt; (4) beim toedlichen Biss bleibt die Finisher-Kamera bis zum Game
  Over stehen, keine Weissblende und keine kreisende Todeskamera mehr, YOU DIED erscheint ueber dem Fress-Bild.
- gator_boss.log: neue Zeilen `RE2 INTRO Lage ...`, `RE2 DASH Planung/Vordrehung/entfaellt ...`, `... wand_draussen_max=`,
  `RE2 KAMERA FINISHER Halt Cut n Fm`, `KANISTER Stufe 1 ... sub a -> 1`.

## 11. Nachbesserung 3 (Abnahme 2, B_abnahme_2.md; Beginn 2026-10-24 08:1x, HEAD 0a90a394)

Neuer Agent (Nachbesserung 3). Baum sauber (`status --porcelain` leer), HEAD 0a90a394 (= 06bc6578 + Abnahmebericht 2
65cd71c7 + AUFTRAG-Entscheid B9). Code-Stand = der von der Abnahme 2 gebaute und gemessene Stand (06bc6578, nur Doku
danach) - deren Messungen (Prüfer-Scratchpad `B41abn2/mess/<Lauf>/`) gelten als "Messung vorher" und werden nicht
wiederholt. Pflichtlektüre: AUFTRAG.md (inkl. B9 08:06: vor dem Intro KEIN sichtbarer Gator, RE2-Form "untergetaucht",
Lage frei, dann Pos_set auf die B7-Startlage; Riegel: im natürlichen Ablauf Lauf N kein Bild mit sichtbarem Gator vor
dem Durchbruch, kein sichtbarer Lagesprung), VERTRAG.md, B_abnahme_2.md (M1-M3), dieses Dossier §2, §9.1, §10, Memory
reai-v2-gator-bosskampf.md. Eigene Läufe unter `.../scratchpad/B41nb3/`.

Plan: M1 (B9: untergetaucht bis zum Durchbruch) -> M2 (keine Körperlenkung von Stufe 1 bis zum Anhängen) -> M3 (RE2-
Anschwimm-Sperre +0x1C0 & 4, Hals-Zielmodus OFFEN mit Beleg) -> Bau -> gezielte Tests -> exe-Messungen nachher -> Abschluss.

### 11.0 RE-Belege (selbst gelesen: `re2_disasm.py` / `B41abn1/em23_full.dis` aus EM23_OVL_0000.BIN, ROOM40A0.RDT mit
`re15_port/tools/r41_re2gator/re2_scd_walk.py`)
- **Vor dem Intro (M1, B9):** SCD-init @RDT 0x2500, Satz @0x00A0 (= Datei 0x25A0)
  `44 00 00 23 00 00 01 16 00 00 4a 16 b8 0b 3c eb 00 04 00 00 00 00` = Sce_em_set Em_no 0, Id 0x23, nFloor 1, Bank 0x16,
  Pos (0x164a, **0x0bb8 = +3000**, 0xeb3c) = (5706, 3000, -5316), Cdir 0x400: der Gator steht 3000 UNTER dem Boden
  (Leons Ebene y = 0, das Wassergitter Modell 0 liegt bei y = 0, §2.5). Sub 0 P0 (@0x80100854-68: `lh v0,468(s0) / beq
  v0,zero,0x80100950 / addiu v0,a0,1 / ... sb v0,6(s0)`) wartet NUR auf +0x1D4 - kein FUN_8001a330 (keine Animation, also
  keine Bildwort-SE/-Stroke), der Sub-0-Pre ist leer (Tafel @0x8010463c[0] = 0x801007fc = `jr ra`: kein Kanister-Fenster,
  kein T1-Schärfen FUN_801012fc). Auftauchen: sub11 `Member_set2 35 17 10` @SCD-main 0x0730 (+0x1D4 -> P1), `Evt_next`
  @0x0734, im NÄCHSTEN Bild `Pos_set 32 00 cd 11 f8 f8 c3 b8` @0x0736 = (4557, -1800, -18237) - der Sprung von
  (5706,3000,-5316) dorthin (13 000 Einheiten) ist in RE2 unsichtbar, weil der Gator vorher unter dem Boden steht.
  P1 (@0x8010086c-b8) startet im selben Bild Clip 6 (`FUN_8001a330(em,6,0,0)`).
- **Kanister-Aufnahme ohne Lenkung (M2):** Gier-Schreiber `sh ...,118(...)` im Overlay nur @0x80100280/2a0 (Root,
  Rückschreiben um FUN_80035408), Intro-Knick @0x80100948, Dash-Knick @0x80100ec0, R1 @0x80102404/@0x80102440, R2/R3
  @0x801027c8/@0x80102a34 (Abnahme 2 §3.3 c) - in Sub 1/2 (Pre @0x80100964-c1c, Main @0x80100c20/@0x80100cdc) und T2
  (@0x801017c4-0x80101d40) keiner. T2-Stufentafel @0x80100024 (`table`): [1] 0x8010183c, [2] 0x80101860 - Stufe 1
  (`jal 0x8007730c` a1 = 29, `sh zero,346(s1)`, `lbu/addiu/sb v0,550(s1)` @0x8010183c-5c) läuft OHNE Sprung in den
  Stufe-2-Code @0x80101860 weiter (selbes Bild, t = 0). Stufe 2 schiebt NUR die Korridor-Achse: `lw v0,64(s2) / lw
  v1,64(s1) / subu / slti v0,v0,8100 / beq v0,zero,+ / addiu v0,v1,8100 / sw v0,64(s2)` @0x801018a4-c0 (obj[1].z =
  z + 8100, x unberührt); Anhängen bei t == 13 (`lh v1,346 / addiu v0,zero,13 / bne` @0x801018c4-cc), dabei Zielmodus
  aus (`lbu v0,448(s1) / andi v0,v0,0xfb / ... sb v0,448(s1)` @0x801018e4-f4).
- **Anschwimm-Sperre (M3):** Sub-1/2-Pre @0x80100964 nur in Zone 1 (`lbu v1,538 / bne v1,1,0x80100b20` @0x801009c8-d0),
  vor der Zone-2-Linie (@0x801009d8-e4) und bei +0x22C == 0 (`lbu v0,556 / bne v0,zero,0x80100c08` @0x80100a0c-14):
  (a) Bank 5 Bit 1 (`jal 0x80077360` a0 = 0x800d4874, a1 = 1 @0x80100a1c-2c; ROOM40A0 sub10 `Set 22 05 01 01` @SCD 0x0659,
  Kanister fällt) und Stufe < 2 (`lbu v0,550 / sltiu v0,v0,0x2 / beq` @0x80100a34-40) -> `+0x1C0 |= 4` (@0x80100a48-5c),
  Ziel +0x170/+0x172/+0x174 = (x, y - 800, obj[1].z) (@0x80100a4c-70); (b) DANACH, unabhängig von (a): obj[1].z - z
  - 9000 < -120 (= obj[1].z - z < 8880; `lw v0,0x800d055c / lw v1,64(s0) / subu / addiu v0,v0,-9000 / slti v0,v0,-120`
  @0x80100a74-8c) UND Stufe 0 (`lbu v0,550 / bne v0,zero` @0x80100a98-a0) -> `+0x1C0 &= 0xfb` (@0x80100aa8-b4). FUN_801012fc
  schärft T1 (+0x224 = 1 @0x80101378-7c) nur bei [0x800cfc06] != 0, (0x800cfbd8 & 0x10) == 0, [0x800cfdcb] == 0, +0x224 == 0,
  **(+0x1C0 & 4) == 0** (`lbu v0,448(a0) / andi v0,v0,0x4 / bne v0,zero,0x80101380` @0x8010134c-58) und Stufe 0/7
  (@0x80101360-70). Hals: Root @0x80100330-74 ruft bei +0x218 & 4 (nach dem Intro gesetzt @0x80100944-4c) mit +0x1C0 & 4
  `FUN_800177c0(em, yaw)` (Ziel = Punkt +0x170.., RE2_Quellcode_V2/FUN_800177c0.c Zweig `(*(byte*)(p+0x1c0) & 4) != 0`),
  sonst `FUN_80017d98(em, yaw)` (nicht dekompiliert, ghidra_re2_Leon.txt Z. 85839 Rohbytes); Hals-Part +0x1C1 = 5 (@0x801001c4-cc
  `sb v0(5),449(s0)`), Grenzen aus dem INIT: Part 5 +0x9C/+0x9E = 20 (Rate), +0xA0 = 560 (Gier), +0xA2 = 312 (Neigung)
  (`sh v0,1016/1018/1020/1022(v1)` @0x801005a0-c4, 1012 = 5 x 172 + 152).

### 11.1 M1 (Punkt 2, B9): der lauernde Gator ist bis zum Durchbruch untergetaucht - behoben
- **Ursache:** seit Nachbesserung 2 versetzt der Pos_set-Zwilling `re15_gr2_intro_lage` den SICHTBAR lauernden Gator beim
  Aggro auf die gerechnete Intro-Startlage (Nutzer-Design 2026-09-10: Lauern an der Leiter, sichtbar). RE2 kennt diesen
  sichtbaren Sprung nicht, weil sein Gator vorher 3000 unter dem Boden steht (§11.0).
- **Messung vorher** (Abnahme 2, Lauf N = gleicher Code-Stand 06bc6578, `B41abn2/mess/N`, nicht wiederholt):
  gator_boss.log `RE2 INTRO Lage (Pos_set-Zwilling ...) von (-7200,-9200) yaw 1089 -> (5085,-19880) yaw 1536` = 16 278;
  Bilder F190/F200 (Cut 7) Gator am Kanalende neben Leons Kopf sichtbar, F201 weg, ab F202 Durchbruch im Suedostbecken.
  Dazu das alte Ein-Bild-Aufblitzen an der Spawn-Lage (-5300,-16300) vor dem Boss-INIT (Abnahme 2 §5, A2 F5).
- **RE-Beleg:** §11.0 (Sce_em_set y = +3000 @RDT 0x25A0; Sub 0 P0 @0x80100854-68 ohne FUN_8001a330, Sub-0-Pre `jr ra`
  @0x801007fc; Auftauchen sub11 `Member_set2` @SCD 0x0730 -> `Pos_set` @SCD 0x0736 im naechsten Bild, P1 Clip 6
  @0x801008ac-b8).
- **Aenderung** (Form = RE2 "untergetaucht", B9 "Lage frei"; der Lauerplatz bleibt der Aggro-Bezug GB_AGGRO 9000):
  - enemy_ai_boss_gator.c `gb_untertauchen` im Boss-INIT (ausser RE15_GB_TEST=5 = Marker-Repro mitten im Kampf):
    Trefferbox hit_radius_min/max = 0 (wie der verborgene RE2-Zellenarm: kein Ziel der Zielhilfe `hit_radius_min <= 0`,
    kein Schuss-Kandidat, kein Spieler-Push `hit_radius_min == 0`; RE2 erreicht dasselbe ueber die Lage 3000 unter dem
    Boden), keine Animation (LURK `anim_frame++` nur aufgetaucht: keine Clip-0-Bildwort-SE f76/f162, keine 0x28-Strokes =
    kein Schuetteln aus dem Versteck), kein Kanister-Vorpass, keine Selbst-Befreiung, kein Sieger-Abzug, kein
    Koerperlaengen-Push; ein Treffer aus dem Versteck (nur der Granaten-Kasten erreicht ihn, re15_re2_gl_kandidat nimmt
    eine Null-Box als 1) loest weder R4/Clip 10 noch Blut noch SE 0/5 aus, er weckt ihn (aggro) - der Durchbruch folgt.
  - `gb_auftauchen` nach dem Phasen-Switch, sobald das Lauern endet oder ein Intro laeuft (auch der Szenen-Aufruf
    re15_gr2_intro_start von Spur F): Trefferbox zurueck (1466), Logzeile `RE2 AUFTAUCHEN F<n> ...`.
  - `re15_gator_boss_untergetaucht(e)` (re15_boss_gator.h): 1 untergetaucht oder Spawn-Bild vor dem INIT (state 0).
  - Zeichner: EIN markierter Haken in main.c (NPC-Schleife, an der Stelle des RE2-Zellenarm-Ausstiegs, vor Schatten und
    Mesh): `if (re15_gr2_pc_untergetaucht(npc, npc_region_culled)) continue;` - gator_re2_pc.c fragt den Boss ab und
    schreibt die Messzeilen `[gator-re2] Sicht F<n> untergetaucht|aufgetaucht ...`, `Sicht erstes Bild F<n> gezeichnet ...`,
    `Sicht Lagesprung max <d> ...` (groesster Wurzelschritt zwischen zwei aufgetauchten Bildern). Gemeinsame Datei main.c:
    +1 Zeile (Entscheid B5: Haken der Spur B in main.c angenommen; jetzt 5 statt 4).
- **Messung nachher** (exe, eigene Kopie, `B41nb3/mess/N`, Rezept wie Abnahme 2 Lauf N, EXIT_AT 600, Framedump 1-600):
  debug.log `Sicht F1 untergetaucht pos=(-5300,0,-16300)` (Spawn-Bild vor dem INIT) ... `Sicht F201 aufgetaucht
  pos=(5085,-1200,-19880) clip=6 bild=0`, `Sicht erstes Bild F201 gezeichnet ... (untergetaucht vorher 200 Bilder)`,
  groesster Bildschritt 338 (F229, Clip 6 Bild 28); gator_boss.log `RE2 AUFTAUCHEN F201 pos=(5085,-19880) yaw=1536
  phase=11 clip=6 bild=0 r=1466`. **Pixelvergleich** gegen denselben Lauf mit RE15_KEIN_GATOR=1 (`B41nb3/mess/N0`,
  `vergleich.py`): **F1-F201 pixelgleich (201/201)**, ab F202 verschieden (der DURCHBRUCH-Schnitt Cut 1 wirkt ab F202) -
  kein Bild mit sichtbarem Gator vor dem Durchbruch, auch kein Spawn-Aufblitzen. Bildbelege
  `bilder_r41b/nb3_m1_N_lauer_vorher_nachher_x3_gain3.png` (F190/F200 Ausschnitt 3x, oben vorher mit Gator neben Leons
  Kopf, unten nachher ohne) und `bilder_r41b/nb3_m1_N_vorher_nachher_F190_F230.png`.
- **Riegel:** unit_r41_re2gator_lauer (NEU) `untergetaucht 123 Takte am Lauerplatz (-7200,-9200), Auftauchen Takt 123 auf
  der Intro-Lage (5085,-19894) - Pos_set-Sprung 16287 im untergetauchten Takt; Intro 274 Schritte, groesster 339,
  Grenze 340 (authored Clip 6/3/0/1 340/72/104/259 x Skala)` - prueft je Takt Box 0, keine Bewegung/Animation, kein Ziel
  der Zielhilfe, Auftauchen mit Clip 6 Bild 0, Box 1466 danach. integration_r41_re2gator Lauf 3 "natuerlich" (NEU, Lauf-N-
  Rezept): `natuerlich: untergetaucht bis F201 (Lauerplatz -7200,-9200), Auftauchen Clip/Bild 6/0, erstes Bild F201
  Clip 6, groesster Bildschritt 338`.
- Riegel mit Beleg angepasst (der Gator taucht im natuerlichen Lauern nicht mehr auf, also kein Kanister-Fenster dort):
  unit_r41_re2gator_kanister/kiefer und die R1-Faelle von _kamera laufen mit RE15_GB_TEST=5 (aufgetaucht wartend, wie das
  K-Rezept §7); probe_r34_reaktion (unit_r34_reaktion, Zensus B12) laesst den Boss vor der Messung auftauchen (Leon einmal
  6000 vor den Lauerplatz) - Zeile `Gator-Boss 0x23@2090 HT HT HT HT` (ohne das: `?? ?? ?? ??` = unerwartet nicht
  getroffen, 16 Fehler).

### 11.2 M2 (Punkt 3): der Kanister wurde beim Aufnehmen im Kreis geschleppt - behoben
- **Ursache:** vom Fenstertreffer (Stufe 1) bis zum Anhaengen (Stufe 2, t = 13) lenkte der Boss-Takt weiter - CHASE-Wende
  0x60/Bild bzw. Steuerung 0x30 und an der Westwand die Selbst-Befreiung (Sofort-Snap der Gier, Saegezahn r4095 -> r95 ...
  r575 -> r4095) - und `gr2_kan_takt` Stufe 2 schob den Kanister laengs der JEWEILIGEN Blickachse nach.
- **Messung vorher** (Abnahme 2, nicht wiederholt): C3 F1098 `KANISTER Stufe 1 laengs=5868 quer=1593 sub 2 -> 1`; state.log
  F1097 r1487 -> r4095 (Befreiung an der Westwand, x -7416 im Klemm-Slab) -> F1098 r95, r191, r239 ... F1107 r575, F1108 r4095
  (selbst nachgerechnet mit `B41nb3/aufnahme_auswerten.py` auf `B41abn2/mess/C3`); der Kanister rutscht vor die Kamera
  (Abnahme-Bild C3_stufe2.png), beim Anhaengen `lokal=(7611,1258,1513)`, Stufe 3 in der Welt bei (843,-25116) = 2900 von der
  Liegestelle. Pruefer-Sonde 'schleppen': Gier 4000 -> 2992, Gator-Weg 101, Kanister-Weg 6157.
- **RE-Beleg:** §11.0 (kein Gier-Schreiber in Sub 1/2/T2; Stufe 1 faellt ohne Sprung in den Stufe-2-Code; Schub nur
  `obj[1].z = z + 8100` @0x801018a4-c0; Anhaengen t == 13 @0x801018c4-cc). Zusaetzlich gelesen (Stufen 3-7 @0x80101970-
  0x80101b8c): nach dem Anhaengen stehen in obj[1]+0x38/+0x3C/+0x40 die LOKALLAGE zur Part-6-Matrix (`sw v0,56(s2) / sw
  v1,60(s2) / sw a3,64(s2)` @0x80101910-18 nach FUN_8002d030); RE2 fuehrt nur +0x38 (x -> 1750, Schritt 70/145/100/100
  @0x801019c4/@0x80101a30/@0x80101ab8/@0x80101b68) und +0x3C (y -> -210, 25/55/30 @0x80101a4c/@0x80101ad4/@0x80101b84)
  heran, +0x40 (z) nie.
- **Aenderung:**
  - Lenksperre: gator_re2_r41.c `re15_gr2_aufnahme_lenksperre(slot)` = Kanister in Stufe 1/2 und noch nicht angehaengt;
    enemy_ai_boss_gator.c CHASE: solange sie gilt, nur `re15_gr2_chase_clip` (Clip 0, Sub 1) + Vortrieb geradeaus - kein
    GUARD-Wechsel, kein Dash, keine Routen-/Wendelogik, keine Steuerung; die Selbst-Befreiung ist in dieser Zeit aus.
    Stufe 2 schiebt damit laengs der festen Gier = RE2 "nur die Korridorachse".
  - Stufen-Zeit byte-genau: das Fenster laeuft im selben Takt in Stufe 1 und den ersten Stufe-2-Schritt (t = 0) weiter (RE2:
    Routine @0x80100200, dann T2 `jal 0x801017c4` @0x80100320 im selben Root-Takt; Stufe 1 @0x8010183c-5c faellt in
    @0x80101860, Tafel @0x80100024). Vorher kostete der Port zwei Takte mehr. Pins angepasst: kanister `Stufe 1/2 0/12`
    (vorher 13 in Stufe 2), aufnahme `Stufe 1..6 37/37` (vorher 39), Fenster-Takt endet in Stufe 2; K/kiefer/kamera
    unveraendert gruen.
  - Quer-Ausgleich (PORT-WAHL, aus der Messung): ohne Lenkung bleibt die Querlage des Fenstertreffers erhalten - exe C3:
    `angehaengt lokal=(4277,1187,2372)`, der getragene Kanister hing ~1600 neben dem Kopf (Bild unten). RE2 braucht keinen
    Ausgleich: der Kanister liegt nach dem Fall bei x -22816 (sub05 Pos_set @SCD 0x0484), der Gator schwimmt Bein 2 auf
    x -23078 (@0x80101048) = 262 neben der Achse, das Fenster prueft nur laengs (@0x80100ae4-ec). Das Port-Fenster laesst bis
    Koerperradius + 267 seitlich zu ("dran vorbei kommt", PORT-WAHL seit Abnahme 0). Port: z wird in Stufe 3/4/5/7 mit
    DENSELBEN Schritten wie x (70/145/100/100) auf die Symmetrieebene des Kopfes (z 0) gefuehrt - der Kanister endet wie in
    RE2 zwischen den Kiefern.
- **Messung nachher** (exe, Rezept C3 der Abnahme 2, `B41nb3/mess/C3` ohne / `C3b` mit Quer-Ausgleich, Framedump 1-1400):
  `KANISTER Stufe 1 laengs=5868 quer=1593 sub 2 -> 1 F1098`, `KANISTER angehaengt lokal=(4277,1187,2372) F1110` (12 Takte nach
  dem Fenster = RE2); state.log Gier F1097-F1110 durchgehend r4095, Wurzel nur laengs x (z bleibt -24993), Gator-Weg 208
  (Clip-0-Schwimmzyklus vor/zurueck); Kanister bis zum Anhaengen unbewegt bei (-1500,-1200,-23400), beim Anhaengen in der
  Welt bei (-1512,-1206,-23410) = kein Sprung; erst danach (Stufe 3-7) wandert er in den Kopf. Bilder
  `bilder_r41b/nb3_m2_C3_stufe2_vorher_nachher_x2.png` (F1098-F1110: oben vorher - Kanister vor die Kamera geschoben, unten
  nachher - liegt still, der Gator oeffnet das Maul) und `bilder_r41b/nb3_m2_C3_getragen_quer_ohne_mit_x3.png` (F1215/F1260:
  ohne Quer-Ausgleich neben dem Kopf, mit Ausgleich zwischen den Kiefern). Klettern unveraendert (`CROSS start=(4450,-11900)
  ende=(3350,-20700) frames=240`), Dash + Putz 54 unveraendert. K (GB_TEST=5): Fenster F6, angehaengt F18 (`lokal=(4212,1187,
  11)`), R1 F70, Cut 1, Leiche, VSync 4 -> 2 F108 - wie Abnahme 2.
- **Riegel:** unit_r41_re2gator_schleppen (NEU): `Fall 1 (Clip 0): Gier fest 1, Gator-Weg 198, Kanister-Weg 0, Anhaengen 12 Takte
  nach dem Fenster` | `Fall 2 (+300 je Takt): Gier fest 1, Gator-Weg 3248, Kanister-Weg 2861 (laengs, quer-Abweichung 0)` |
  `Fall 3 (quer 1200): Lokallage beim Anhaengen (4199,1186,1783) -> nach Stufe 7 + 40 Takte (1750,-210,0)`; dazu
  "Lenksperre aus nach dem Anhaengen" und "danach lenkt die Verfolgung wieder".

### 11.3 M3 (Punkt 3): RE2-Anschwimm-Sperre (Zielmodus +0x1C0 & 4) - Biss-Sperre umgesetzt, Hals-Zielmodus OFFEN mit Beleg
- **Ursache:** `re15_gr2_biss_erlaubt` bildete aus FUN_801012fc nur die Stufen-Bedingung (@0x80101360-70) ab, nicht das
  Zielmodus-Bit (@0x8010134c-58), das der Sub-1/2-Pre in der Kanister-Zone setzt.
- **Messung vorher** (Pruefer-Sonde 'anschwimmen' der Abnahme 2, nicht wiederholt): Kanister 10000 / 14000 RE2-Einheiten
  voraus, Leon 2000 vor der Wurzel -> `Biss-Start nach 0 Takten, Stufe 0, biss_erlaubt 1` (ohne Kanister ebenso).
- **RE-Beleg:** §11.0 (Setzen @0x80100a34-5c bei Bank 5 Bit 1 + Stufe < 2, Loeschen @0x80100a74-b4 bei Stufe 0 und
  obj[1].z - z < 9000 - 120 = 8880, beim Anhaengen @0x801018e4-f4; T1 nicht scharf bei Bit 2 @0x8010134c-58).
- **Aenderung** (gator_re2_r41.c `gr2_zielmodus_takt`, je Takt vor dem Kanister-Takt): in der Beckenzone der Liegestelle
  (PORT-WAHL "RE2-Zone 1" wie die Hysterese-Ruhe) bei liegendem Kanister: Stufe < 2 -> Bit an; Stufe 0 und laengs
  < (9000 - 120) x Skala = 5920 -> Bit aus; Anhaengen -> Bit aus. `re15_gr2_biss_erlaubt` liefert 0, solange das Bit steht
  (wirkt auf alle vier Biss-Starts des Boss-Moduls). Logzeile `RE2 KANISTER Zielmodus 0|1 (Anschwimm-Sperre) laengs=.. Stufe ..
  F..`. PORT-WAHLEN: ausserhalb der Kanister-Zone und ausserhalb der Schwimm-Verfolgung (GUARD/CROSS, Nutzer-Design ohne
  RE2-Gegenstueck) ist das Bit aus - RE2 verlaesst Zone 1 nur ueber die Zone-2-Linie z >= -7322 (@0x801009e0), der Kanister
  liegt dort bei z -9500 (sub05 Pos_set @SCD 0x0484), also laengst hinter der 8880-Grenze: RE2 traegt das Bit nie aus Zone 1
  hinaus. Folge fuer den Abnahme-Fall "14000 voraus": vom Gator bei z -25000 aus liegt der Kanister bei z -15666 =
  Westkanal-Streifen (gb_zone 2), nicht in der Zone des Gators (0) -> keine Sperre (wie die Hysterese-Ruhe).
- **Messung nachher:** unit_r41_re2gator_anschwimmen (NEU, Lage wie die Pruefer-Sonde): `Kanister 10000 voraus (>= 8880),
  gleiche Zone: Zielmodus 1, Biss-Start nie (40 Takte)` | `12000 voraus (Gator bei -26500), gleiche Zone: Zielmodus 1, nie` |
  `8000 voraus (< 8880, vor dem Fenster 8601): Zielmodus 0, Biss nach 0` | `hinter dem Gator: 0, nach 0` | `14000 voraus im
  Westkanal (andere Zone): 0, nach 0` | `ohne Kanister: 0, nach 0`. Im exe-Lauf C3 entstand die Lage nicht (der Gator naeherte
  sich dem Kanister mit laengs 5868 < 5920 direkt ins Fenster, keine `Zielmodus`-Zeile im Log).
- **Hals-Zielmodus = OFFEN (Beleg):** Root @0x80100330-74: bei +0x218 & 4 (gesetzt am Intro-Ende @0x80100944-4c) ruft RE2 mit
  +0x1C0 & 4 `FUN_800177c0(em, yaw)` (Hals-Part +0x1C1 = 5 @0x801001c4-cc, Ziel = Punkt +0x170/+0x172/+0x174 = (x, y - 800,
  obj[1].z) @0x80100a4c-70 - also geradeaus laengs der Korridorachse auf Hoehe des Kanisters; RE2_Quellcode_V2/FUN_800177c0.c
  Zweig `(*(byte*)(p+0x1c0) & 4) != 0`, Rate Part 5 +0x9C/+0x9E = 20, Grenzen +0xA0 = 560 Gier / +0xA2 = 312 Neigung aus dem
  INIT @0x801005a0-c4) und sonst `FUN_80017d98(em, yaw)` (nicht dekompiliert, ghidra_re2_Leon.txt Z. 85839 Rohbytes). Der Port
  fuehrt die RE2-Halsfuehrung des Gators NICHT (auch nicht die normale FUN_80017d98 auf Leon, Abnahme 2 §5): der Boss-Haken
  re15_gator_spine_arc_vz liefert nur einen az-Zuschlag je Bone (skeleton_common.c), ein Gier-Zuschlag (ay) auf Part 5
  braeuchte den globalen g_anim_bone_angle_hook (gehoert dem G5-Endkampf, enemy_ai_boss_g5.c) oder einen neuen Haken in
  skeleton_common.c (keine Spur-B-Datei, VERTRAG §1.3); ausserdem verschoebe ein Halswinkel Bone 5/6/7 und damit die gepinnten
  Kopf-Messungen (wand, kamera-Wahl, Maulpunkt des Finishers). Naechster Schritt (Folge-Runde): FUN_80017d98 disassemblieren
  (FUN_8001820c liegt dekompiliert vor), beide Halsmodi ueber einen eigenen ay/az-Kanal fuer Part 5 bauen, Kopf-Riegel
  nachziehen.

### 11.4 Tests (gebauter Stand 8fd08bdc, keine volle Suite)
- NEU: unit_r41_re2gator_lauer, _schleppen, _anschwimmen (tests/unit/test_r41_re2gator.c, registriert in
  tests/unit/probes/r41_re2gator.cmake); integration_r41_re2gator Lauf 3 "natuerlich".
- Mit Beleg angepasst: unit_r41_re2gator_kanister/_kiefer/_kamera (R1-Faelle) auf RE15_GB_TEST=5 (aufgetaucht wartend, §11.1),
  kanister Stufe-Pins 13 -> 12, aufnahme 39 -> 37 und Fenster-Takt endet in Stufe 2 (§11.2); probe_r34_reaktion Zensus laesst
  den Boss vor der Messung auftauchen (§11.1).
- `ctest --test-dir re15_port/build -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|
  sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_.*|
  re2_hit_repeat_crow_spider|re2_crow_.*|crow_ai|crow_fixes|crow_death_corpse|r36_schuss|r36_schuss_kraehe|r36_kleidung_kraehe|
  r35_fenster_kraehe|r37_re2zombie_.*|r34_re2fx.*|montage_fx|r35_cut1150_montage.*|r38_montage.*|r38_wasser.*|r38_wasserzombie|
  r39_heliport1171_kraehen|r30_granate|r35_granate|r36_granate|r36_kartenbits|text_freeze|r34_reaktion|r34_schaden|g5_boss|
  load_after_death|r35_affen_finisher|r36_tueren_.*|r37_itemband|r36_kartenitems)$|^probe_(re2z_stomp|crow_kill)$|
  ^r31_trefferboxen.*$" --timeout 600 -j 6` -> **100% tests passed, 0 tests failed out of 108** (Endstand, 211 s; mit dem
  ersten Stand 1 Fehler unit_r34_reaktion Zensus `Gator-Boss ?? ?? ?? ??` -> behoben, s. §11.1).
- `ctest --test-dir re15_port/build -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate|r37_itemband|r36_tueren)$"
  --timeout 1200 -j 3` -> **100% tests passed, 0 tests failed out of 6** (Endstand, 829.68 s: r41_re2gator 112,8 s,
  r36_schuss 146,9 s, r35_granate 168,0 s, r38_wasser 166,8 s, r36_tueren 37,3 s, r37_itemband 829,6 s unter Last; derselbe
  Satz vor dem Quer-Ausgleich ebenfalls 6/6, 893 s). integration_r41_re2gator -V (Stand a1fefc53): `nische: RE2-Zeilen 57, Intro
  Lage/Start/Ende 1/1/1 wand 0, Clip 6/3 125/150, Kamera DURCHBRUCH:6/0;SEITE:6/70;ANGRIFF:6/124;FREI:3/149;FINISHER:4/0,
  Finisher Cut 6 ab F403 gehalten 248 Bilder / 13 Halt-Zeilen, HP min 25300 | natuerlich: untergetaucht bis F201 (Lauerplatz
  -7200,-9200), Auftauchen Clip/Bild 6/0, erstes Bild F201 Clip 6, groesster Bildschritt 338 | dash: DASH 1/1 wand 0, Putz 6 x 9
  = 54, gezeichnet bis 98 Quads/Bild in 20 Messbildern, Clip 2/3 150/300`.
- Nach dem letzten Kommentar-Commit neu gebaut (nur gator_re2_r41.c uebersetzt, keine Warnung) und `ctest -R "^unit_r41_re2gator_"`
  -> **100% tests passed, 0 tests failed out of 14**.
- exe-Laeufe (eigene Kopien re15_pc_b41n3_*.exe, `B41nb3/mess/`): N (natuerlich, Leon), N0 (dasselbe ohne Gator, Pixelvergleich),
  NZ (Elza ROOM2091, natuerlich: identisch, Auftauchen F201, Schritt 338), C3 / C3b (Kanister-Aufnahme ohne/mit Quer-Ausgleich),
  K (Kanister-Tod, GB_TEST=5).

### 11.5 OFFEN (gemessen, benannt)
- **Hals-Zielmodus** (RE2 FUN_800177c0 bei +0x1C0 & 4) und die normale RE2-Halsfuehrung FUN_80017d98: nicht gebaut, Beleg und
  Weg in §11.3 (kein ay-Kanal fuer Part 5 in Spur-B-Dateien, Kopf-Riegel betroffen).
- **Granate erreicht den untergetauchten Gator:** re15_re2_gl_kandidat (Granaten-Kasten, re15_damage.c) nimmt eine Null-Box als 1
  und hat kein Gegenstueck zur RE2-Lage 3000 unter dem Boden; Folge: ein Granaten-Treffer am Lauerplatz weckt ihn (aggro ->
  Durchbruch), ohne Reaktion/Blut/Ruf aus dem Versteck (§11.1). Gemessen gruen: integration_r35_granate Lauf "gator"
  (RE15_FORCE_EXPLOSION am Gator, Schaden 1000) - der Riegel misst genau diese Zustellung.
- **Intro-Startlage im natuerlichen Westkanal-Zugang** (aus §10.6): der Durchbruch liegt im Suedostbecken (5085,-19880),
  16 000 vom Lauerplatz; der Sprung ist jetzt unsichtbar (B9), der Durchbruch beginnt hinter Leon, Leon bleibt waehrend
  der 275 Intro-Bilder steuerbar (PORT-WAHL seit Abnahme 0: keine Spieler-Sperre Bit 0x10). Unveraendert.
- **Nicht nahe am Lauerplatz = kein Kampf:** wer 2090 nur ueber die Osttuer/Plattform begeht und nie naeher als 9000 an den
  Lauerplatz (-7200,-9200) kommt (Abnahme-2-Laeufe E3/E4), sieht den Gator jetzt gar nicht (vorher lag er sichtbar am
  Lauerplatz). Form = B9; Spur F setzt den Auftritt ueber ihre Szene ("springt aus dem Wasser", re15_gr2_intro_start).
- Wie §10.6/§9.8/§6: Raum-Effekt 0x26 fehlt, Frame-Wort-Latenz 1 Bild, Waffenschaden ueber die RE1.5-Zeile, Kanister im
  dunklen Becken kaum sichtbar, GUARD/CROSS spielen Clip 0, Dash-Ausrichtung nur Gleichstands-Regel.

### 11.6 Fuer den Nutzer (Nachbesserung 3)
- Keine neuen Assets, keine Sprachdateien. Gemeinsame Datei: platform/pc/main.c +1 markierte Zeile (NPC-Zeichner,
  `re15_gr2_pc_untergetaucht`, Kommentar "Runde 41 Spur B (Entscheid B9)"); tests/unit/probe_r34_reaktion.c (Zensus-Zeile des
  Gator-Bosses, +10 Zeilen mit Beleg).
- Was sich im Spiel aendert:
  1. Der Alligator ist vor dem Kampf nicht mehr zu sehen (wie RE2: er liegt "unter Wasser"), auch kein Aufblitzen beim
     Betreten; er bricht erst aus dem Wasser, wenn Leon/Elza naeher als ~9000 an seine Ecke kommen (oder per Szene von Spur F).
     Vorher kann man ihn weder anvisieren noch anschiessen.
  2. Beim Aufnehmen des Kanisters schwimmt er geradeaus darauf zu, der Kanister liegt still bis er ihn packt (kein Kreisen mehr),
     danach liegt der Kanister zwischen den Kiefern (auch wenn er seitlich aufgenommen wurde). Die Aufnahme ist 2 Bilder kuerzer
     (RE2-Takt).
  3. Liegt der Kanister in seiner Zone vor ihm (weiter als ~5900), schnappt er nicht nach Leon (RE2-Anschwimm-Sperre); er
     verfolgt dabei weiter wie bisher (RE2 lenkt den Koerper nicht zum Kanister, nur den Hals - der Hals ist OFFEN, §11.3).
- Messzeilen: debug.log `[gator-re2] Sicht ...` (untergetaucht/aufgetaucht, erstes Bild, Lagesprung), gator_boss.log
  `RE2 AUFTAUCHEN F<n> ...`, `RE2 KANISTER Zielmodus 0|1 ...`, `RE2 KANISTER angehaengt ... F<n>`.
- Testhaken: RE15_GB_TEST=5 (Marker-Repro) laesst den Gator aufgetaucht an der Marker-Lage warten (fuer Kanister-Rezepte ohne
  Kampf, z.B. das K-Rezept §7) - das natuerliche Lauern taucht unter.
- Entscheidungsfragen: (1) Granaten-Treffer am Lauerplatz weckt den untergetauchten Gator (RE2 kennt den Fall nicht) - so lassen?
  (2) Soll der Gator auch ohne Annaeherung irgendwann auftauchen (z.B. bei Betreten ueber die Osttuer), oder bleibt das Spur F
  (Szene) vorbehalten?

### 11.7 Abschluss Nachbesserung 3 (je Mangel)
- **M1** behoben (B9): untergetaucht bis zum Durchbruch, Lauf N F1-F201 pixelgleich mit "ohne Gator", Auftauchen F201 auf der
  Pos_set-Lage mit Clip 6 Bild 0, kein Bildschritt > 338 (authored 340); Elza (ROOM2091) identisch. Riegel lauer + exe Lauf 3.
- **M2** behoben: Gier fest vom Fenster bis zum Anhaengen (C3 F1097-F1110 r4095), Kanister unbewegt bis zum Anhaengen, Anhaengen
  12 Takte nach dem Fenster wie RE2, Quer-Ausgleich -> Kanister zwischen den Kiefern. Riegel schleppen (3 Faelle).
- **M3** Biss-Sperre umgesetzt (Riegel anschwimmen, 6 Faelle), Hals-Zielmodus OFFEN mit Beleg (§11.3).

### 11.8 Paralleler Schreiber im Zweig + Nachtrag 6 (nur vermerkt)
- Waehrend dieser Nachbesserung landete der Orchestrator-Commit 2fa4c43c (09:17, `doc(r41): AUFTRAG - Nachtrag 6 ...`, nur
  AUFTRAG.md) zwischen 8fd08bdc und 1d3f0776 in r41/re2gator. Keine Code-Datei betroffen, kein Konflikt.
- Nachtrag 6 (Wortlaut des Nutzers) gibt fuer den Alligator freie Hand bei Startlage und Leichenlage und legt die
  Ankunftsszene fest (Spur F mit G): Leiche mit Schluessel -> Zombies oben in 2080 -> Leiche -> Alligator frisst sie samt
  Schluessel -> Schrei/Schuetteln zu Leon -> Kampf mit RE2-Kampfmusik. Diese Nachbesserung setzt davon nichts um (nicht
  zugeteilt). Vertraeglich mit §11.1: bis zum Szenen-Auftritt bleibt der Gator untergetaucht; die Szene taucht ihn mit
  `re15_gr2_intro_start` (oder einer eigenen Lage + Clip) auf - `re15_gator_boss_untergetaucht` meldet dann 0 (im_intro), der
  Boss-Takt stellt die Trefferbox im selben Takt wieder her (gb_auftauchen). Bausteine fuer "Schrei und Schuetteln": SE 4
  (ENEMSE Bank 17, @0x80100d64/84) und die Ereignis-Maschine (Stroke-Ereignis 1 -> Rumble + Schuetteln @0x80101d44, Tafeln
  @0x8009db14/@0x8009daf4). Fuer die RE2-Kampfmusik (Spur F): ROOM40A0 sub11 startet die Musik mit dem Intro -
  `Sce_bgm_control 51 00 01 00 78 40` @SCD-main 0x071F (vor `Member_set2` @0x0730), weitere Aufrufe `51 00 05 00 00 00`
  @0x0762 / @0x078A (Walker re2_scd_walk.py; Bedeutung der Bytes nicht in dieser Spur belegt).

## 12. Nachbesserung 4 (Abnahme 3, B_abnahme_3.md; Beginn 2026-10-24 10:14, HEAD d4e36ca6)

Neuer Agent (Nachbesserung 4). Baum sauber (`status --porcelain` leer), HEAD d4e36ca6 (= 47efb932 + Abnahmebericht 3).
Code-Stand = der von der Abnahme 3 gebaute und gemessene Stand (47efb932) - deren Messungen (Pruefer-Scratchpad
`B41abn3/mess/<Lauf>/`, Sonde `B41abn3/probe/probe_abnahme3.c`) gelten als "Messung vorher", sofern hier nicht neu gemessen.
Eigene Laeufe unter `.../scratchpad/B41nb4/`. Pflichtlektuere: B_abnahme_3.md (M1/M2), AUFTRAG.md (Wortlaut, B1-B9,
Nachtrag 6), VERTRAG.md, Dossier §2/§10/§11, Memory reai-v2-gator-bosskampf.md (Selbst-Befreiung = Sweep-Klasse (3)
v0.7.65, Garantie unit_gator_sweep 0/3286).

Plan: M1 (Kanister-Weltlage + Effekt-Quellmatrizen aus der Pose des gezeichneten Bildes; Riegel "gezeichnete Lage =
Part6(Bild) x lokal" bei Wende, Biss, Befreiung; Bildbeleg C3 F1276-F1284 vorher/nachher) -> M2 (Befreiung ohne
Sofort-Snap, PORT-WAHL benannt; in CHASE/GUARD keine Gier-Aenderung > 0x60 je Bild ausser den RE2-Knicken +1024;
Riegel auf die Lagen E1 und C3 ueber >= 300 Bilder; Bildbeleg) -> Bau -> gezielte Tests (Sweep!) -> exe nachher -> Abschluss.

### 12.0 RE-Belege (selbst gelesen, `re2_disasm.py` auf info/re2leon/PSX.EXE bzw. `--bin EM23_OVL_0000.BIN`)
- **Kanister = Kind der GEZEICHNETEN Part-6-Matrix (M1).** T2 @0x801018c4-cc `lh v1,346(s1) / addiu v0,zero,13 / bne` (t == 13),
  @0x801018d0 `addiu a0,s0,1104` (s0 = Part-Tafel +0x198, 1104 = 6 x 172 + 0x48 = Weltmatrix des Part-Records 6), @0x801018e0
  `sw a0,128(s2)` (obj[1]+0x80 = Eltern-Zeiger), @0x801018f0 `jal 0x8002d030` (a0 Part-6-Matrix, a1 = obj[1]+84, a2 = sp+16 =
  Lokalmatrix), @0x80101910-18 `sw v0,56(s2) / sw v1,60(s2) / sw a3,64(s2)` (Lokallage). Root-Reihenfolge je Bild @0x80100200
  `jalr v0` (Routine: Sub-Pre/Main mit FUN_8001a330 = Bildvorschub + Wurzelschritt) -> @0x80100284 `jal 0x80035408` -> @0x80100320
  `jal 0x801017c4` (T2) -> @0x8010037c-90 `lhu v0,1140(s2) / lhu v1,554(s0) / addiu a1,s2,1056 / addu / jal 0x8008e1f4` (Part 6
  az += +0x22A, RotMatrix in die LOKALE Part-6-Matrix +0x18, danach Winkel zurueck @0x80100394-a4). Die Weltmatrix +0x48, an der
  der Kanister haengt, enthaelt also die Pose DIESES Bildes samt +0x22A - der Kanister steht in jedem Bild an der gezeichneten
  Kopf-Matrix x Lokallage, ohne Nachlauf.
- **Ueberblendung FUN_8001a330 (EXE), fuer die gezeichnete Pose.** @0x8001a34c-64 `lbu v0,332(s0) / beq v0,a1 / sb a1,332(s0) /
  sb zero,333(s0) / sb s1,334(s0)` (Clipwechsel: +0x14E = Blend-Argument), @0x8001a37c-ac `addiu v1,s1,1 / addiu v0,zero,4096 /
  divu v0,v1 / mflo v0 / sll a3,s2,16 / ... jal 0x8002959c / or a3,v0,a3` (Rate = 4096 / (Blend + 1) im unteren Halbwort);
  0x80029614 @0x800296a8-ec `lbu t3,334(s2) / beq t3,zero / andi v0,a3,0xffff / mult v0,t3 / ... addiu v0,zero,4096 / mflo t1 /
  subu v0,v0,t1` (Vorbild-Gewicht = Rate x Zaehler, GTE-Lerp), @0x80029994 `bne t3,zero,0x800299c0`, @0x800299c0-cc `lbu v0,334(s2)
  / addiu v0,v0,-1 / sb v0,334(s2)` (Zaehler -1 je Aufruf). Der Port-Zeichner rechnet dasselbe (skeleton_common.c: wp =
  anim_frac x anim_blend_rate), der Gator setzte aber nur anim_frac (32/4/16/3) - ohne Rate (Default 0x200) und ohne Abbau
  (Typ 0x23 ist in re15_type_self_advances_anim, der globale Abbau laeuft nicht). GEMESSEN (exe C3, RE15_POSE_DUMP,
  `B41nb4/mess/C3v/pose.log`): frac 4 ab F390 bis F644 und ab F946 bis Laufende (= dauerhaft 50 % Vorbild), frac 16 F645-F794 =
  wp 0x2000 -> 0x1000 = Pose EINGEFROREN: Bone 9/13 in allen 150 Dash-Bildern F645-F794 bitgleich (2072,-813,1455)/(-3781,-588,494),
  obwohl kf 176..325 laeuft - der RE2-Dash (Clip 2) war im Spiel nie zu sehen. Nebenbefund dieser Nachbesserung (Punkt 2
  "gleiches Verhalten bei Animation"), mit behoben.
- **Keine Koerperdrehung in der RE2-Verfolgung (M2).** Gier-Schreiber im ganzen EM23-Overlay (Abnahme 3 §2, selbst
  nachgesehen @0x80100280 `sh zero,118(s0)` / @0x801002a0 `sh v0,118(s0)` = Sichern/Rueckschreiben um FUN_80035408 @0x80100284):
  @0x80100280/2a0/948/ec0/2404/2440/27c8/2a34 - keiner in Sub 1/2 (Pre @0x80100964, Main @0x80100c20/@0x80100cdc) oder T2. Die
  Selbst-Befreiung (Sofort-Snap re15_enemy_steer_point(.., 0x800), enemy_ai_boss_gator.c) ist Port-Design (probe_gator_sweep
  2026-09-11, Klasse (3) "Klemm-Slab-Gefaengnis"); ihr Zweck laut Kommentar: das ZENTRUM steckt im Slab und die Box-Klemme
  verwirft von innen JEDE Bewegung. Gemessen (Abnahme 3, gier.py > 0x60): alle Spruenge in CHASE/GUARD sind Befreiungen an einer
  WAND, an der die Klemme Bewegung nach aussen sehr wohl zulaesst (Wand-Druck, kein Gefaengnis): C3 F1097/1116/1207/1218/1327/1338/
  1344 Westwand (Ziel Leon in der Tuernische), A2 F379 Westwand, D2 F1004/1075/1083 Westwand, N F1057 Nordwand, E1 F426/458/490/522/
  546 Ostwand (GUARD-Ziel GPAT (6500,-12600) liegt 700 vor der Ostwand = hinter der Klemmlinie 7200-1466), dazu C3 F634 nach der
  CROSS-Landung (Plattform-Slab). Ausgenommen (kein CHASE/GUARD-Bild bzw. RE2-Knick): Intro-Start (Pos_set-Zwilling, unsichtbar
  davor), CROSS-Start (Bahn-Blick, Nutzer-Design), +1024 am Clip-3-Ende (@0x80100938-48 / @0x80100eb0-c0, Lunge-Ende ebenso).

### 12.1 M1 (Punkt 3): der getragene Kanister lief dem Kopf ein Bild hinterher - behoben
- **Ursache (bestaetigt, zwei Teile):** (1) ZEITPUNKT - `gr2_kan_folgen` lief in `re15_gr2_takt` am ANFANG des Boss-Takts, vor
  Befreiung, Wende, `re15_ai_advance`, Bildvorschub und Dispatcher-Klemme; gezeichnet wurde diese Lage zur Pose NACH dem Takt.
  (2) POSE - `gr2_bone_welt` rechnet die Keyframe-Pose ohne Zusaetze (g_anim_pose_actor = NULL): ohne Kopf-az +0x22A (in Stufe
  2..7 bis ~+204) und ohne Ueberblendung, waehrend der Zeichner beides zeichnet. Dieselben zwei Fehler trugen die Effekt-
  Quellmatrizen Part 6 / Part 7 / Kanister (nachgefuehrt am Takt-Anfang aus der Keyframe-Pose).
- **Messung vorher:** Abnahme 3 (Pruefer-Sonde 'nachlauf': Wende 0x60/Bild 619-637 je Bild, Biss bis 409, > 300 in 51/240 Bildern;
  exe C3 F1206->F1207, F1278->F1282, F1337->F1338). Eigener Lauf `B41nb4/mess/C3v` (Stand 47efb932, Abnahme-Rezept C3 +
  RE15_POSE_DUMP, Framedump F1090-F1345): Kanister F1280-F1284 als grauer Zylinder AUF/HINTER dem Kopf (Bild unten, obere Reihe).
  Nebenbefund (§12.0): gezeichnete Pose im Dash 150 Bilder eingefroren, sonst dauerhaft 50 % Vorbild.
- **RE-Beleg:** §12.0 (Kind der Part-6-Weltmatrix @0x801018d0/@0x801018e0, FUN_8002d030 @0x801018f0, Lokallage @0x80101910-18,
  Root-Reihenfolge @0x80100200 -> @0x80100284 -> @0x80100320 -> Kopf-az @0x8010037c-90; Ueberblendung @0x8001a34c-ac,
  @0x800296a8-ec, @0x800299c0-cc).
- **Aenderung:**
  - enemy_ai_boss_gator.c `re15_gator_boss_tick` = Takt-HUELLE um den bisherigen Takt (`gb_takt`): (1) Ueberblend-Zaehler -1 je
    Takt VOR dem Takt (@0x800299c0-cc; das Bild eines Clipwechsels zeigt den vollen Zaehler n wie RE2), (2) die Wand-Klemme, die
    bisher der Dispatcher nach dem Takt rechnete (enemy_ai_common.c Typ 0x23, re15_collision_constrain_enemy(&g_room_rdt, al_ox,
    al_oz, .., hit_radius_min, y, 4)), BITGLEICH am Takt-Ende (gleiche Funktion, gleiche Argumente, gleiche Ausnahme CROSS/befrei);
    `re15_gator_boss_skip_clamp` meldet fuer den aktiven Boss jetzt 1 (der Dispatcher und die Sonden-Nachbauten klemmen nicht
    doppelt; enemy_ai_common.c unveraendert), (3) `re15_gr2_nachtakt` (gator_re2_r41.c).
  - `re15_gr2_nachtakt`: Pose wie der NPC-Zeichner in diesem Bild (main.c NPC-Pass: clip_override = (anim_flags & 4) ? motion : -1,
    re15_compute_actor_kf, re15_skel_compute_pose mit g_anim_pose_actor = Aktor -> Ueberblendung + re15_gator_spine_arc_vz
    (+0x22A/+0x228/Bogen)), Ueberblend-Schnappschuss prev_root/prev_angles/anim_prev_valid gesichert und zurueckgeschrieben
    (der Zeichner rechnet danach aus demselben Schnappschuss genau diese Pose). Daraus: Anhaengen (t == 13 markiert in Stufe 2,
    FUN_8002d030 gegen die GEZEICHNETE Part-6-Matrix dieses Bildes -> kein Sprung), Kanister-Weltlage = Part6 x Skala x Lokallage,
    Effekt-Quellen Part 6 / Part 7 / Kanister (Status-0x800-Kopie FUN_8001d894 @0x8001d8ac-fc in re2fx_tick NACH dem Spielschritt).
  - R1-Start (`re15_gr2_r1_start`) nimmt die zuletzt GEZEICHNETE Kanister-Matrix (@0x80102300-24), Fontaene (FUN_80103eb4,
    `gr2_fontaene_takt`) die von `re15_gr2_nachtakt` gefuehrten Puffer (kein Nachrechnen am Takt-Anfang mehr).
  - Ueberblendung (Nebenbefund): `gr2_blend(e, n)` setzt anim_frac = n UND anim_blend_rate = 4096 / (n + 1) (@0x8001a37c-ac) fuer
    Clip 0/1 (32/4 @0x80100c38/@0x80100cfc) und Dash-Clip 2 (16 @0x80100dac-b4); Grossreaktion Clip 10 (3, @0x80102c44) Rate 1024.
  - Messzeile (PC, gator_re2_pc.c nach dem NPC-Pass, `re15_gr2_kanister_sitz`): aus dem Ueberblend-Schnappschuss, den der Zeichner
    eben geschrieben hat (Vorbild-Gewicht 0x1000 = genau die gezeichneten Winkel, Zusaetze unveraendert), Part6(gezeichnet) x
    Lokallage gegen die gezeichnete Kanister-Lage; debug.log `[gator-re2] Kanister-Sitz F<n>: angehaengt <k> Bilder, Abweichung
    max <m> ...` alle 30 Bilder und bei jedem neuen Hoechstwert. Ohne Wirkung auf den Aktor.
- **Messung nachher:**
  - Riegel `unit_r41_re2gator_sitz` (NEU; Lage der Pruefer-Sonde, Zeichner-Nachbau nach jedem Takt): `(a) Aufnahme Stufe 3..7 + 40:
    66 angehaengte Bilder, groesster Abstand zu Part6(Bild) x lokal 0` | `(b) 240 Bilder, Wende 105 (max 0), Biss 43 (max 0),
    groesster Abstand 0` | `(c) Befreiung 17 Schub-Bilder, Gier fest 1, groesster Abstand 0` | `(d) Ueberblendung 16 Bilder (Zaehler
    bis 16), groesster Abstand 0; Effekt-Quelle Part 6 falsch in 0 Bildern`.
  - GEGENPROBE (Riegel scharf?): derselbe Riegel gegen einen Bau, der `re15_gr2_nachtakt` am Takt-ANFANG ruft (= alter Zeitpunkt):
    `Wende max 626, Biss max 342` (Abnahme 3: 619-637 / 409), Befreiung 52, Ueberblendung 583, Effekt-Quelle Part 6 falsch in 424
    Bildern -> 5 Fehler. Danach zurueckgebaut (Datei-Kopie), neu gebaut.
  - exe, NUR-M1-Messbau (die M2-Befreiung voruebergehend auf den alten Snap zurueckgesetzt, nicht eingecheckt) `B41nb4/mess/C3m1`,
    Rezept C3: Zustandslog F1-F1400 BITGLEICH mit C3v (Leon, Gator-Clip/Bild/Lage/Gier - `vgl_bahn.py`: 1400/1400), also derselbe
    Ablauf; `[gator-re2] Kanister-Sitz F1379: angehaengt 270 Bilder, Abweichung max 0 (F1110)` - in jedem gezeichneten Bild sitzt der
    Kanister an Part6(Bild) x Lokallage, auch in den Wenden, Bissen und den (in diesem Messbau noch alten) Befreiungs-Snaps.
    Bild `bilder_r41b/nb4_m1_C3_F1276-1284_vorher_nachher_x3_gain2.png` (Ausschnitt 3x, Aufhellung 2,2): oben vorher (C3v) der
    Zylinder auf/hinter dem Kopf ab F1280, unten nachher (C3m1) das gelbe Kanister-Ende zwischen den Kiefern, ab F1282 vom Kiefer
    verdeckt (er steckt im Maul).

### 12.2 M2 (Punkt 2): Gier-Spruenge von 100-180 Grad in Verfolgung und Belagerung - behoben
- **Ursache (bestaetigt):** die Selbst-Befreiung (Sweep-Klasse (3), 2026-09-11) loeste aus, sobald das Zentrum im Slab (Kante +
  Koerperradius + 60) stand und sich < 8 bewegt hatte, und lenkte dann mit `re15_enemy_steer_point(e, bx, bz, 0x800)` (Sofort-Snap)
  zum Slab-Ausgang. Diese Bedingung erfuellt jeder Gator, der von AUSSEN gegen eine Wand drueckt (Klemmlinie = Kante - Radius, also
  im +60-Rand): Ziel Leon in der Tuernische (C3/A2/D2, Westwand), Leon im Norden (N, Nordwand), GUARD-Ziel GPAT (6500,-12600) 700
  vor der Ostwand = hinter der Klemmlinie 7200 - 1466 (E1). Danach drehte die Verfolgung mit 0x40/0x60 zurueck an die Wand ->
  der naechste Snap (E1: alle 32 Bilder = 2048 / 0x40).
- **Messung vorher:** Abnahme 3 (`gier.py`): E1 F426 +1944, F458 +1984, F490 -1984, F522 +1984, F546 -1472; C3 F1097 -1488, F1207
  -1474, F1327 -1490 (dazu F1116/1218/1338/1344 -336..-576 und F634 -1114 nach der CROSS-Landung); A2 F379 +1650; D2 F1004 -478,
  F1075 -1302, F1083 -480; N F1057 +1557. Bilder der Abnahme: beleg_gier_kanister.png, E1_snap.png, A2_snap.png.
- **RE-Beleg:** §12.0 (RE2: kein Gier-Schreiber in Sub 1/2 / T2; Schreiber nur @0x80100280/2a0 (Root-Sichern/-Rueckschreiben um
  FUN_80035408 @0x80100284), @0x80100948 / @0x80100ec0 (+1024-Knicke), @0x80102404/2440/27c8/2a34 (R1-R3)). Die Befreiung ist
  Port-Design ohne RE2-Gegenstueck.
- **Aenderung (PORT-WAHL, Form = die Wand-Klemme selbst, sie verschiebt nur die Lage):** enemy_ai_boss_gator.c
  - `gb_gefangen(e, bx, bz)`: Befreiung nur noch im echten GEFAENGNIS = die Wand-Klemme (re15_collision_constrain_enemy, dieselben
    Argumente wie die Takt-Klemme) verwirft schon einen Schritt GB_SWIM_SPEED (48) zum Slab-Ausgang (push_rect Fall 0 setzt auf die
    Vorlage zurueck, re15_collision.c FUN_8003bca8). Wand-DRUCK von aussen ist kein Gefaengnis (der Schritt hinaus ist frei) - dort
    bleibt der Gator an der Wand und lenkt weiter mit seiner Wenderate auf sein Ziel.
  - `gb_befrei_beginn` / `gb_befrei_schritt`: die Befreiung ist ein SCHUB ohne Drehung - je Takt GB_SWIM_SPEED zum Ausgang (derselbe
    Weg je Bild wie bisher re15_ai_advance(e, GB_SWIM_SPEED)), Gier unveraendert, Klemme ausgesetzt (befrei = 2 wie bisher),
    Schwimm-Clip 0 laeuft weiter, bis der Ausgang (Slab-Rand + 120 wie bisher) erreicht ist. gator_boss.log `BEFREI start/ende`.
  - Messzeile GIER-WACHE (Takt-Huelle): je Bildpaar mit beiden Takten in CHASE/GUARD das Gier-Delta; `GIER-SPRUNG F.. d=..` je
    Delta > 0x60 ausser +1024, `GIER-WACHE bis F..: n Bildpaare, max |Gier-Delta| m (ohne +1024), Spruenge k, Befreiungen b` alle 300
    Paare. Messhaken re15_gator_boss_gier_wache / _befreiungen / _verfolgt / _phase (re15_boss_gator.h).
  - Ausgenommen wie verlangt: CROSS (Bahn-Blick, Nutzer-Design), Intro-Start (Pos_set-Zwilling, davor untergetaucht), +1024-Knick.
- **Messung nachher:**
  - Riegel `unit_r41_re2gator_gier` (NEU): `E1 Plattform-Belagerung (Leon oben auf der Rampe) 800 Bildpaare CHASE/GUARD (GUARD 800,
    an der Wand 461), max |Gier-Delta| 64 (ohne +1024), Spruenge > 0x60 0, Befreiungen 0` | `C3 Westwand vor der Tuernische (Leon in
    der Nische) 536 Bildpaare CHASE/GUARD (GUARD 0, an der Wand 284), max |Gier-Delta| 96, Spruenge > 0x60 0, Befreiungen 0`.
    `unit_r41_re2gator_sitz` (c): Gefaengnis (Zentrum 600 im aufgeblasenen Plattform-Slab) -> 1 Befreiung, 17 Schub-Bilder, Gier fest.
  - GEGENPROBE: derselbe Riegel gegen einen Bau mit der alten Befreiung (gb_gefangen = 1, Snap 0x800 + advance): `E1 ... max |Gier-
    Delta| 1984, Spruenge 15, Befreiungen 15` (Takte 202/234/266 = alle 32 wie Abnahme-E1), `C3 ... max 1954, Spruenge 9` -> FAIL;
    zurueckgebaut, neu gebaut.
  - Konvergenz-Garantie unveraendert: unit_gator_sweep `gator-biss: 80 Faelle, 0 ohne Treffer` / `gator-sweep: 3342 Paare, 0 nicht
    konvergiert` (66 s).
  - exe (eigene Kopien re15_pc_b41n4_*.exe, Rezepte der Abnahme 3, Endstand): `gier.py > 0x60` + gator_boss.log:
    E1n (Tuerweg 20A0) `GIER-WACHE bis F581: 300 Bildpaare CHASE/GUARD, max |Gier-Delta| 64, Spruenge 0, Befreiungen 0`, Spruenge im
    Zustandslog nur F6 (Intro-Start -352) und F280 (+1024); C3n `GIER-WACHE bis F1302: 300 Bildpaare, max 96, Spruenge 0, Befreiungen
    0`, sonst nur F280/F934 (+1024) und F393 (+348 CROSS-Start); A2n2 nur F6 (Intro-Start) und F280 (+1024), Finisher-Schnitt F398
    (vorher F403 - der Snap F379 und die Rueckdrehung fehlen); D2n `GIER-WACHE bis F980: 300 Bildpaare, max 96, Spruenge 0`, Dash/Putz
    unveraendert (Planung (-6052,-18113) 1139 -> 624, Putz 54, wand 0); Nn nur F6 (Lauer-INIT, untergetaucht), F201 (+447 Auftauchen),
    F475/F931 (+1024); NZn (Elza, ROOM2091) Auftauchen F201 Clip 6 Bild 0, Lagesprung max 338 F229 wie Abnahme 3, keine Spruenge.
  - Bilder: `bilder_r41b/nb4_m2_A2_westwand_F377-381_vorher_nachher_x3_gain2.png` (gleicher Ablauf bis F378; oben vorher F379 dreht
    der Gator in einem Bild mit offenem Maul zur Kamera, unten nachher bleibt er laengs der Wand), `bilder_r41b/nb4_m2_E1_belagerung_
    vorher_nachher_x2_gain18.png` (Belagerung an der Rampe: oben vorher F425 -> F427 Umschlag des Koerpers, unten nachher F420-F430
    aufgerichtet zu Leon ohne Umschlag).
- **Nebenwirkung K (Kanister-Tod, GB_TEST=5) geprueft:** Kn `KANISTER Stufe 1 ... F6`, `angehaengt ... F18`, `R1 VSync 4 F70`,
  Raum-SE 15, `R1 ende (Leiche)`, `VSync 2 F108` (2521 ms), Explosionsbilder F70-F88 wie Abnahme 3 (Blitz/Scherben am Kopf, Leiche);
  `Kanister-Sitz F47: angehaengt 30 Bilder, Abweichung max 0`.
- **Nebenwirkung (gemessen, erwuenscht):** das Wand-Klemm-Zittern (Abnahme 3 §6: Wurzel an der Westwand springt je Bild 30-60
  hin und her) kam von der Befreiung (befrei = 2 setzt die Klemme aus, die Verfolgung schob ungeklemmt zurueck). C3 vorher
  (`B41abn3/mess/C3`): 251 Bilder an der Westwand (x < -7300), 7 Richtungswechsel von z >= 20 (max 50); nachher (`B41nb4/mess/C3n`):
  155 Bilder, 0 Richtungswechsel.
- **Nachtrag im Code (nach den exe-Laeufen, 7831a6d5):** ein laufender Befreiungs-Schub gilt nur in denselben Lagen, in denen er
  beginnen darf (LURK/CHASE/GUARD, nicht untergetaucht, keine Kanister-Lenksperre) - ein Treffer (R1/R4) waehrend des Schubs bricht
  ihn ab, statt den Takt (und damit den R1-Ablauf) zu verschlucken. In allen Messlaeufen trat kein Schub auf (Befreiungen 0), die
  Riegel sitz (c) / gier und der Sweep liefen nach dem Neubau erneut (§12.3).

### 12.3 Tests (gebauter Stand, keine volle Suite)
- NEU: `unit_r41_re2gator_sitz` (M1: Kanister = Part6(gezeichnet) x Lokallage bei Aufnahme, Wende, Biss, Befreiung, Ueberblendung;
  Effekt-Quelle Part 6 = dieselbe Matrix), `unit_r41_re2gator_gier` (M2: E1 + C3 je 800 Takte, kein Delta > 0x60 ausser +1024,
  keine Befreiung an der Wand) - beide in tests/unit/test_r41_re2gator.c, registriert in tests/unit/probes/r41_re2gator.cmake;
  integration_r41_re2gator Laeufe 4 "kanister" (Kanister-Sitz max 0 ueber >= 200 Bilder) und 5 "belagerung" (Tuerweg 20A0,
  GIER-WACHE 300 Paare ohne Sprung/Befreiung, GUARD erreicht) + GIER-SPRUNG-Pruefung in allen fuenf Laeufen.
- Mit Beleg angepasst: unit_r41_re2gator_schleppen Fall 3 - die Querlage beim Anhaengen bezieht sich jetzt auf die GEZEICHNETE
  Part-6-Matrix (mit Kopf-az +0x22A ~+204 nach Stufe 2, @0x8010037c-90): `Lokallage beim Anhaengen (4430,959,487)` statt
  (4199,1186,1783); Pruefung "Querlage != 0" statt "> 1000", Endlage (1750,-210,0) unveraendert gepinnt.
- Gegenproben (Riegel schlagen gegen den alten Stand an): §12.1 (sitz: 5 Fehler, Wende 626) und §12.2 (gier: 5 Fehler, +-1984).
- Laeufe (gebauter Stand, eigener Baum re15_port/build):
  - `ctest --test-dir re15_port/build -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|
    sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_.*|
    re2_hit_repeat_crow_spider|re2_crow_.*|crow_ai|crow_fixes|crow_death_corpse|r36_schuss|r36_schuss_kraehe|r36_kleidung_kraehe|
    r35_fenster_kraehe|r37_re2zombie_.*|r34_re2fx.*|montage_fx|r35_cut1150_montage.*|r38_montage.*|r38_wasser.*|r38_wasserzombie|
    r39_heliport1171_kraehen|r30_granate|r35_granate|r36_granate|r36_kartenbits|text_freeze|r34_reaktion|r34_schaden|g5_boss|
    load_after_death|r35_affen_finisher|r36_tueren_.*|r37_itemband|r36_kartenitems)$|^probe_(re2z_stomp|crow_kill)$|
    ^r31_trefferboxen.*$" --timeout 600 -j 6` -> **100% tests passed, 0 tests failed out of 110** (11:12-11:13, Stand 9fd7d545;
    nach dem Neubau mit 7831a6d5 erneut 11:30-11:31: **100% tests passed, 0 tests failed out of 110**, 103.79 s). Darin
    unit_gator_sweep (`3342 Paare, 0 nicht konvergiert`, `80 Faelle, 0 ohne Treffer`), unit_gator_fress, unit_r34_reaktion,
    unit_g5_boss und alle 16 unit_r41_re2gator_*.
  - `ctest --test-dir re15_port/build -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate|r37_itemband|r36_tueren|
    r37_re2zombie)$" --timeout 1500 -j 3 --output-on-failure` -> **100% tests passed, 0 tests failed out of 7** (11:13-11:29,
    Stand 9fd7d545; r41_re2gator 263,8 s, r35_granate 214,5 s, r38_wasser 169,4 s, r36_schuss 142,3 s, r37_re2zombie 54,6 s,
    r36_tueren 51,8 s, r37_itemband 935,5 s).
  - nach dem Neubau mit 7831a6d5: `ctest --test-dir re15_port/build -R "^integration_(r41_re2gator|r35_granate)$" --timeout 1500
    -j 2 -V` -> **100% tests passed, 0 tests failed out of 2** (212,39 s). Status-Zeile integration_r41_re2gator: `nische: RE2-Zeilen
    57, Intro Lage/Start/Ende 1/1/1 wand 0, Clip 6/3 125/150, Kamera DURCHBRUCH:6/0;SEITE:6/70;ANGRIFF:6/124;FREI:3/149;FINISHER:4/0,
    Finisher Cut 6 ab F398 gehalten 253 Bilder / 13 Halt-Zeilen, HP min 25300, kein Gier-Sprung | natuerlich: untergetaucht bis F201
    (Lauerplatz -7200,-9200), Auftauchen Clip/Bild 6/0, erstes Bild F201 Clip 6, groesster Bildschritt 338, kein Gier-Sprung |
    kanister: 270 angehaengte Bilder, Abweichung max 0, GIER-WACHE bis F1302: 300 Bildpaare CHASE/GUARD, max |Gier-Delta| 96 (ohne
    +1024), Spruenge > 0x60: 0, Befreiungen 0 | belagerung: GIER-WACHE bis F581: 300 Bildpaare CHASE/GUARD, max |Gier-Delta| 64 (ohne
    +1024), Spruenge > 0x60: 0, Befreiungen 0, GUARD-Zeilen 11 | dash: DASH 1/1 wand 0, Putz 6 x 9 = 54, gezeichnet bis 98 Quads/Bild
    in 20 Messbildern, Clip 2/3 150/300, GIER-WACHE bis F980: 300 Bildpaare CHASE/GUARD, max |Gier-Delta| 96 (ohne +1024), Spruenge >
    0x60: 0, Befreiungen 0`. (Finisher-Schnitt jetzt F398 statt F403: ohne den Befreiungs-Snap F379 und seine Rueckdrehung kommt
    der Biss 5 Bilder frueher; Kamerafolge, Halt bis zum Raumende und HP-Sockel unveraendert.)
- Bau: `local_build.sh build` (keine neue .c-Datei, kein configure noetig; CMake hat sich wegen der Riegel-Dateien selbst neu
  erzeugt), EXIT 0, Warnungen nur die drei bekannten in main.c (8803/10133/10513, Altbestand), keine in Spur-B-Dateien.

### 12.4 OFFEN (gemessen, benannt)
- **Hals-Zielmodus / RE2-Halsfuehrung** (FUN_80017d98 / FUN_800177c0, Root @0x80100330-74): unveraendert OFFEN wie §11.3/§11.5 -
  Entscheid des Orchestrators zum Part-5-Gier-Kanal ausserhalb der Spur-B-Dateien steht aus (Abnahme 3 §3.3 e).
- **Wand-Druck statt Abprall (Folge von M2, PORT-WAHL):** wo das Ziel hinter einer Wand liegt (Leon in der Tuernische; GUARD-Ziel
  GPAT (6500,-12600) 700 vor der Ostwand bei Leon auf der Rampe), bleibt der Gator jetzt an der Wand und lenkt mit seiner
  Wenderate weiter auf das Ziel (vorher: Snap weg, Rueckdrehung, erneuter Snap). Die Belagerungs-Zielwahl selbst (Nutzer-Design
  v0.7.54-63) ist unveraendert; dass ihr Ziel hinter der Klemmlinie liegt, ist ALT (gemessen E1: GPAT-Zeilen 6500,-12600).
- **Befreiungs-Schub:** im echten Gefaengnis (Zentrum im Plattform-/Rampen-Slab, nur nach CROSS-Landungen erreichbar) gleitet der
  Gator ohne Drehung mit 48 je Bild aus dem Slab (Riegel sitz (c): 17 Bilder). In keinem exe-Lauf dieser Nachbesserung aufgetreten
  (Befreiungen 0 in C3n/E1n/D2n/A2n2/Nn/NZn/Kn); Sichtpruefung dieses Falls im Spiel offen.
- **Effekt-Quellmatrizen:** der Port fuehrt sie jetzt aus der Pose desselben gezeichneten Bildes (Abnahme-Forderung). Ob RE2s Pumpe
  (@0x80026980, hinter der Gegner-Schleife) die Part-Weltmatrix (+0x48) dieses oder des vorigen Bildes kopiert, ist nicht belegt
  (der Zeitpunkt der Modellmatrix-Komposition im RE2-Zeichenpfad wurde nicht disassembliert).
- **RE15_FPS != 30** (Entwickler-Schalter, main.c: "RE15_FPS=60 still available for future re-tuning"): der NPC-Zeichner posiert
  dann anim_frame >> 1, re15_gr2_nachtakt anim_frame - der Kanister saesse dort neben dem Kopf (die Messzeile Kanister-Sitz zeigte es).
  Im Spiel (30 Bilder/s) nicht erreichbar.
- Unveraendert aus §11.5 / Abnahme 3 §6 (Nebenbefunde, nicht Teil von M1/M2): Granate erreicht den untergetauchten Gator, Intro-
  Startlage beim Westkanal-Zugang im Suedostbecken, Zielmodus-Messzeile ohne "Zielmodus 0" beim Loeschen im !im_chase-Zweig/beim
  Anhaengen, Kanister in Stufe 3-6 noch vor der Schnauze (C3 F1136 hinter der Suedwand - RE2 zieht ihn erst mit den Lokallage-
  Schritten @0x801019c4-0x80101b8c ins Maul), E1-Wurzelsprung nach R4 (861), Raum-Effekt 0x26, Frame-Wort-Latenz 1 Bild,
  Waffenschaden ueber die RE1.5-Zeile, Kanister im dunklen Becken kaum sichtbar, GUARD/CROSS spielen Clip 0.
- Behoben als Nebenwirkung: Wand-Klemm-Zittern an der Westwand (Abnahme 3 §6) - §12.2.

### 12.5 Fuer den Nutzer (Nachbesserung 4)
- Keine neuen Assets, keine Sprachdateien, keine neuen Umgebungsschalter. Gemeinsame Dateien: KEINE neuen Haken (main.c,
  enemy_ai_common.c, skeleton_common.c unveraendert); geaendert nur Spur-B-Dateien (enemy_ai_boss_gator.c, gator_re2_r41.c,
  platform/pc/src/gator_re2_pc.c, re15_boss_gator.h, re15_gator_re2_r41.h, Riegel).
- Was sich im Spiel aendert:
  1. Der Kanister im Maul des Alligators sitzt in jedem Bild zwischen den Kiefern - auch wenn er sich dreht, zubeisst oder
     schwimmt (vorher hing er beim Drehen ein Bild hinterher und stand neben/auf dem Kopf).
  2. Der Alligator dreht sich in der Verfolgung und an der Plattform nicht mehr ruckartig um (vorher bis 180 Grad in einem Bild,
     an der Plattform alle ~1 s). Wo er Leon nicht erreicht (Tuernische, oben auf der Rampe), bleibt er an der Wand und schaut zu ihm.
  3. Die RE2-Ueberblendung der Animation laeuft wie in RE2: der Beschleunigungs-Ansturm (Dash, Clip 2) ist jetzt sichtbar animiert
     (vorher 150 Bilder eingefroren), die Wechsel Schwimmen/Paddeln blenden in 32 bzw. 4 Bildern ueber statt dauerhaft halb verzoegert.
- Messzeilen: debug.log `[gator-re2] Kanister-Sitz F<n>: angehaengt <k> Bilder, Abweichung max <m> ...`; gator_boss.log
  `GIER-WACHE bis F<n>: ...`, `GIER-SPRUNG F<n> d=...` (sollte nie erscheinen), `BEFREI start/ende ...`.
- Entscheidungsfrage: Soll der Alligator, wenn Leon unerreichbar hinter einer Wand steht (Tuernische, Rampe oben), an der Wand
  bleiben (jetzt, ohne Drehruck) oder sich - wie vorher, aber weich - immer wieder abwenden und neu anschwimmen?

### 12.6 Abschluss Nachbesserung 4 (je Mangel)
- **M1** behoben: Kanister-Weltlage und Effekt-Quellmatrizen werden am Takt-Ende nach Wand-Klemme aus der Pose gerechnet, die der
  Zeichner in diesem Bild zeichnet (samt Kopf-az +0x22A und RE2-Ueberblendung); Anhaengen gegen dieselbe Matrix (kein Sprung).
  Riegel sitz: 0 Abweichung bei Aufnahme/Wende/Biss/Befreiung/Ueberblendung (Gegenprobe alter Zeitpunkt: Wende 626). exe C3m1
  (bitgleicher Ablauf) und C3n/Kn/integration "kanister": Kanister-Sitz max 0 in 270 bzw. 52 Bildern. Bildbeleg C3 F1276-F1284.
  Mit behoben: RE2-Ueberblendung (Rate 4096/(n+1), Abbau je Bild) - Dash nicht mehr eingefroren.
- **M2** behoben: keine Gier-Aenderung > 0x60 je Bild in CHASE/GUARD ausser +1024 (Riegel gier E1/C3 je 800 Takte; exe E1n/C3n/
  A2n2/D2n/Nn/NZn und integration nische/natuerlich/kanister/belagerung/dash); Befreiung nur im echten Gefaengnis als Schub ohne
  Drehung (PORT-WAHL, Form = Wand-Klemme). Sweep-Garantie unveraendert (3342/0, 80/0). Bildbelege A2 F377-F381 und E1.

## 13. Nachbesserung 5 (Abnahme 4, B_abnahme_4.md; Beginn 2026-10-24 13:16, HEAD 71192fbc)

Neuer Agent (Nachbesserung 5). Baum sauber (`status --porcelain` leer), HEAD 71192fbc (= Code-Stand 7831a6d5/b20f1496 +
Abnahmebericht 4 + AUFTRAG/VERTRAG Nachtrag 6-11). Der gebaute Stand ist der von der Abnahme 4 gemessene (re15_pc.exe 11:40,
seither kein Code-Commit); ihre Laeufe (Pruefer-Scratchpad `B41abn4/mess/<Lauf>/`, `B41abn4f/`) gelten als "Messung vorher",
sofern hier nicht neu gemessen. Eigene Laeufe/Sonden unter `.../scratchpad/B41nb5/`.
Pflichtlektuere: B_abnahme_4.md (vollstaendig, M1 + H1-H3), AUFTRAG.md (Wortlaut, B1-B9, Nachtrag 6 "Betreff Aligator darfst du
alles machen was du willst"), VERTRAG.md, Dossier §12, Memory reai-v2-gator-bosskampf.md (FRESS-Finisher, Klemm-Lehren).

Mangel M1 (Punkt 1): nach einem toedlichen GUARD-Hochbiss liegt Leon im RE2-Finisher nicht im Maul (E1/RN Tuerweg-Ankunft
Ostrampe, MN2 Rampenmitte; im Vorher-Bau MA2 ohne Gator im Bild). Verlangt: nach JEDEM toedlichen Biss (CHASE und GUARD: Rampe
Ost, Rampenmitte, Plattformblock; Leon und Elza) liegt das Opfer ueber P1-P3 bis zum Game Over im Maul wie in A2, der gehaltene
Finisher-Schnitt zeigt Gator und Opfer; Riegel unit_gator_fress um eine GUARD-Lage (gezeichneter Kiefer MIT pitch/jaw-Haken
gegen Leon in jedem P1-P3-Bild), integration "belagerung" prueft nach dem Tod; Bildbeleg E1 vorher/nachher F628-F870.

Plan: (1) MESSEN - Sonde mit dem echten Boss-Takt: je P1-P3-Bild Leons gezeichnete Wurzel/Koerper im GEZEICHNETEN Kopf-Rahmen
(Part 6 mit allen Haken) gegen die authored Relation (CHASE-Tod im offenen Wasser = A2 = RE2-Korridor), dazu pitch_vz, Klemm-
Versatz je Takt, Lage gegen die Waende - fuer CHASE, GUARD Ostrampe (6300,-16250), Rampenmitte (4000,-15000), Plattformblock.
(2) Ursache(n) aus der Messung, RE2-Gegenstueck (FRESSEN P0-P3 @0x80100fbc-0x801011e0, Root @0x80100200-3a4) selbst lesen.
(3) Umsetzung (PORT-WAHL nach Nachtrag 6, RE2-Teile mit @0x). (4) Riegel + Messzeile im Spiel. (5) exe E1/MN2/Plattform/Elza
vorher/nachher, Bildbeleg F628-F870. (6) gezielte Tests, Abschluss.

### 13.0 RE-Belege (selbst gelesen, `re2_disasm.py --bin EM23_OVL_0000.BIN` bzw. info/re2leon/PSX.EXE)
- **FRESSEN = Sub 4 @0x80100f58** (Phasenbyte +6: `lbu v1,6(s0)` @0x80100f74, 0 -> P0 @0x80100fbc, 1 -> P1 @0x801010e8,
  2 -> P2 @0x80101168, 3 -> P3 @0x801011c4). Eingang nur aus Sub 7 am Korridorende (`addiu v0,zero,1025 / sw v0,4(s0)`
  @0x801012e0-e4, §10.3).
- **P0 @0x80100fbc-0x801010e4 inszeniert den Finisher:** Zonenkamera-Sperre `ori v0,v0,0x100 / sw v0,0(a0)` @0x80100fcc-d0,
  Schnitt Cut 14/7 (`addiu a0,zero,14` @0x80100fec / `addiu a0,zero,7` @0x80100ff0, `jal 0x8002c7ac` @0x80100ff4); Kanister-Objekt
  aus (`lui at,0x800d / sw zero,1308(at)` @0x80101008-0c = obj[1] word0 @0x800d051c); Spieler-HP -1 (`sh v0,-690(at)` @0x80101040);
  **der GATOR wird umgesetzt**: `addiu v0,zero,-23078 / sw v0,56(s0)` @0x80101048-4c (x auf die Korridor-Achse); **die
  Zusatzwinkel werden genullt**: `sh zero,554(s0)` @0x80101050 (+0x22A Kopf-az Part 6), `sh zero,552(s0)` @0x80101054 (+0x228
  Kiefer-az Part 7), `sb zero,548(s0)` @0x80101058 (+0x224); Spieler x = Gator-x + 10643 (`lw v0,56(s0) / addiu v0,v0,10643 /
  sw v0,-976(at)` @0x8010106c/7c/84), z = -915 (`addiu v1,zero,-915 / sw v1,-968(at)` @0x80101070-78), KEIN y-Schreiber (nur
  0x800cfc30/0x800cfc38 = Spieler +0x38/+0x40). Die RE2-Finisherlage ist also fest: Gator auf der Korridor-Achse, Leon
  10643 davor (-12435, -915), Cut 7 zeigt sie frontal (§9.2: frontal +1,00 (2 Grad), Kopf (204,118) Tiefe 11175, Leon VOR der
  Kamera (186,186) Tiefe 5040).
- **P1/P2/P3 platzieren das Paar absolut, ohne Wand:** P1 FUN_8001a330(em,4,0,0) @0x801010ec-f4, bei Bild 13 (@0x801010fc-104)
  Anker FUN_80015b94 @0x80101130, Spieler-Gier = Gator-Gier @0x80101154-5c; P2 FUN_80015cb8 @0x80101174 + Clip 5 @0x80101188;
  P3 FUN_80015cb8 @0x801011cc + FUN_8001a240(em,11) @0x801011e0. FUN_80015cb8 (EXE @0x80015cb8-dac): kf 0x80015db0, RotMatrixY
  0x8008e8b4 (Gier +0x76), ApplyMatrix 0x8008dba4, Lage = Anker +356/+358/+360 + Versatz `sw v0,56(s1)` @0x80015d68 / `sw
  v0,60(s1)` @0x80015d7c / `sw v0,64(s1)` @0x80015d90 - **keine Wandkollision**. Root des Gators @0x80100190-3a4: jal 0x8001a308
  @0x8010020c, 0x80035408 @0x80100284 (= Trefferboxen nachfuehren, RE2_Quellcode_V2/FUN_80035408.c: Box-Lagen um die Gier, kein
  SCA), 0x80034d0c @0x80100304 (Koerper-Push Entity gegen Entity, Schleife ueber 0x800ce334), T2 0x801017c4 @0x80100320,
  0x80101388 @0x80100328, Hals 0x800177c0/0x80017d98, RotMatrix 0x8008e1f4 @0x8010038c - keine Wand-Klemme. Hauptschleife
  0x800267c0-930: 0x8004fba0 (Bodenabfrage, `sh v0,450(s0)`) @0x80026850/6c VOR der KI, KI `jalr v0` @0x80026888, Zeichnen
  0x80027160 @0x8002689c, danach 0x8002eaf8 / 0x800168b4 / `jalr` +520 - keine Wand-Klemme nach der KI.
- **Port-Gegenstueck (gelesen):** FRESSEN P0 (enemy_ai_boss_gator.c `case GBP_FRESSEN`, timer == 1) setzt nur Leon
  (Gator + RotY(Gier) x (10643 x s, -915), PORT-VERALLGEMEINERUNG), laesst den Gator, wo der Biss fiel, und nullt die
  Zusatzwinkel NICHT: `pitch_vz` (GUARD -350 x guard_t/24, Z. 2232) bleibt durch P0-P3 (genullt erst am FRESSEN-Ende),
  `gb_biss_abschluss` nullt nur `arc_vz`; die Takt-Huelle (NB4) klemmt den Gator auch in FRESSEN (`gb_klemme_aus` nur CROSS/
  befrei) - Leon folgt dem UNgeklemmten Anker (gb_opfer_skalieren um e->x vor der Klemme).

### 13.1 Messung vorher (eigene Sonde, gebauter Stand 7831a6d5)
Sonde `B41nb5/probe/probe_maul.c` (Harness = Kopf von test_r41_re2gator.c: echter Boss-Takt mit Takt-Huelle, EM23-Bank,
Leon-Statue bis zum Tod, danach fuehrt der Finisher Leon). Je FRESSEN-Bild: gezeichnete Pose wie der NPC-Pass
(g_anim_pose_actor = Gator: pitch/arc/jaw + RE2-az), Leons Koerper (PL00-Knochen + Opfer-Keyframes, Gier x render_scale,
776 Vertices), Maulraum = AABB Mesh 6+7 der gezeichneten Pose im Gator-Rahmen; "Kopf-lokal" = Leons gezeichnete Wurzel im
Rahmen von Bone 6 gegen dieselbe Groesse authored (Keyframe-Pose ohne Haken, Gator an der Anker-Sollage); Klemm-Versatz =
Gator-Lage - Anker-Sollage.

| Lage | Tod (Takt) | Gator beim Tod | pitch | Kopf-lokal Abw. max (P1/P2/P3) | Klemm max | Bilder mit >= 50 % im Maul (P1/P2/P3) |
|---|---|---|---|---|---|---|
| A CHASE offen (Leon (-4000,-22000), = A2-Bezug) | 7 | (-5793,-22000) Gier 4095 | 0 | 0 / 541* / 0 | 0 | 0/15, 106/120, 120/120 |
| B GUARD Ostrampe (6300,-16250,-1800) | 137 | (5716,-12428) Gier 151 | -291 | 4729 / 4651 / 2574 | 1129 | 0, 0, 0 |
| E GUARD Ostrampe E1-Lage (6732,-16250,-1800) | 140 | (5716,-12498) Gier 89 | -350 | 5649 / 5290 / 3048 | 1116 | 0, 0, 0 |
| C GUARD Rampenmitte (4000,-15000,-1800) | 97 | (5107,-11375) Gier 631 | -189 | 3088 / 2509 / 1525 | 281 | 0, 17, 82 |
| D GUARD Plattformblock (0,-16250,-1800) | 325 | (-3647,-14997) Gier 1011 | -350 | 5651 / 4403 / 2718 | 110 | 0, 0, 0 |

\* A: das eine Bild sf = 120 (Ruecksprung des Gators auf Clip-5-Bild 0 fuer EINEN Takt, Original `sb zero,333(s2)`
@0x80029b48; Leon wird dort nicht neu gesetzt) - Bezugsartefakt, im Bild 100 % im Maul. P1 ist auch im Bezug nicht "im Maul":
Leon steht 7096 vor der Wurzel (Hochheben, Opfer-Clip 0) bis zum Anker bei Bild 13.
- **Ursache 1 (Neigung):** E P1 Takt 2: Leon kopf-lokal (5296,3937,-1496) gegen authored (5957,-1712,-1691) bei Klemm 0 - allein
  die Wurzel-Neigung pitch -350 (~-31 Grad, Bone 0) hebt den Kopf ~5600 von Leon weg.
- **Ursache 2 (Klemme):** E P2: Klemm-Versatz 62 (Takt 16) -> 985 (Takt 40) -> 385 (Takt 102), P3 326-337: der Gator schiebt
  entlang des Clip-5-Versatzes in die Ostwand, die Takt-Klemme haelt ihn an der Linie x 5716, Leon (am ungeklemmten Anker) nicht.
- **Ursache 3 (Ort):** auch ohne 1 und 2 liegt die authored Konfiguration in E1 HINTER der Ostwand (Leon P1 bei (12620,-14370),
  P2 x 8800-11000, Wand x 7200; Offline-Suche `B41nb5/probe/probe_buehne.c` aus dem Bezugspfad A: "am Ort mit eigener Gier
  passt=0") - die Kamera (E1: Cut 2, Rueckansicht frontal -1147) sieht Gator und Leon von hinten bzw. durch die Wand.

### 13.2 Fortsetzung (neuer Agent ab 2026-10-24 14:0x, Kontowechsel; HEAD a06c52e0)
Der Vorgaenger wurde vom Wochenlimit ~13:5x beendet; sein Code-Stand ist 0c101dd8 + a06c52e0 (vom Orchestrator gesichert). Bau-
stand geprueft: re15_pc.exe 13:56:59 und libre15_engine.a 13:56:57 sind juenger als jede Quelle (letzte Aenderung
enemy_ai_boss_gator.c 13:56:53) und der Baum ist sauber -> der gebaute Stand IST a06c52e0 (build5.log `LOCAL-BUILD-OK (build)`).
Damit gelten diese Messungen des Vorgaengers fuer den Endstand (nicht wiederholt):
- Sonde `B41nb5/probe/probe_maul.c` gegen die Bibliothek 13:56:57 (Ordner `n2_A..n2_E`, 13:59): ab P0 in ALLEN fuenf Lagen
  Kopf-lokal-Abweichung 0 und Klemm-Versatz 0, Bilder mit >= 50 % von Leons Koerper im Maulraum P2 106/120, P3 120/120 - bitgleich
  mit dem CHASE-Bezug A (vorher B/E/C/D: 0/0/0, 0/0/0, 0/17/82, 0/0/0, Tabelle §13.1). Die einzige Abweichung (B 1180, C 1113,
  D 1364, E 1515) steht im Biss-Bild selbst (Phase schon FRESSEN, Timer 0, GUARD-Pose pitch -350, P0 noch nicht gelaufen) - das
  Bild zeigt noch die alte Kamera mit dem Biss (s. u.), es ist kein P1-P3-Bild.
- exe `B41nb5/mess/E1b` (13:57, Endstand, Tuerweg 20A0 -> 2090, Framedump F622-F640): `RE2 FINISHER Buehne (Nachbesserung 5,
  RE2-P0 @0x80101048-4c): von (5763,-12762) Gier 115 -> (5638,-12546) Gier 2323 Cut 4, Versatz 249 Drehung 1888` und
  `KAMERA FINISHER ... -> Cut 4 F627 ... (Buehne)` im Biss-Bild; Bildbogen `B41nb5/E1_schnitt2.png`: F627 zeigt den Biss noch in
  Cut 2 (Leon oben auf der Rampe), F628 den umgesetzten Gator in Cut 4 - kein Lagesprung in der alten Kamera.
- exe `B41nb5/mess/E1` (13:52, Vorstand mit Schnitt in P0, sonst gleich) F600-F880: Messzeile `Maul-Sitz F867: Bilder 240 (P1 14
  P2 120 P3 106), Abweichung max 0, Kopf+Leon im Bild 230/240 (Cut 4), im Wasser 240/240`; Bilder `B41nb5/E1_nachher_zoom.png`:
  F650-F690 Leon (blau) im Maul, F700-F720 Wurf nach oben (RE2-P2-Bild 60-75), F740 wieder im Maul, ab ~F760 YOU DIED.
  Die 10 Bilder "nicht im Bild" sind der Wurf (Leon oben aus dem Bild; in RE2 Cut 7 ebenso, Dossier §13.0 / gr2_fin_sicht).
  Nebenbeobachtung: in P1 (F628-F640) verdeckt eine der beiden Raum-Spinnen (braun, liegt im Vordergrund) Leons Oberkoerper; die
  Spinnen entfernt Spur F (AUFTRAG), die Buehnen-Wahl beruecksichtigt keine anderen Aktoren.

Neu gemessen (dieser Agent): Buehnen-Garantie, Sonde `B41nb5c/probe_bsweep.c` (echte re15_gr2_finisher_buehne gegen die
Bibliothek 13:56:57, Gator-Wurzel im Raster, 8 Gierwinkel je Lage):
- Wasser (Wurzel ausserhalb der Klemmzone 1466), Raster 250: **21320 Lagen, 0 ohne Buehne**, Versatz max 7250, Drehung max 1991,
  1,31 ms je Suche (Raster 1000: 1480/0, 1,18 ms).
- zusaetzlich alle Lagen IN Plattform/Rampe/Wandsaum (CROSS-Biss), Raster 250: **24256 Lagen, 0 ohne Buehne**, Versatz max 8997
  (unter der Suchgrenze GR2_FIN_WEIT 9000; die weitesten Faelle sind Wurzeln mitten auf der Rampe x 2100-4600, Buehne im Westkanal
  x ~-4200), 1,44 ms je Suche.
- Folge: der Rueckfall "keine Buehne - am Ort" tritt fuer keine erreichbare Gator-Lage auf.

Offen fuer diesen Agenten: Riegel `maul` (Maulraum mit gezeichnetem Kiefer, GUARD-Lagen gegen den CHASE-Bezug je Bild) und
`buehne` (Garantie), Messzeile um den Abstand Leon <-> gezeichneter Kiefer erweitern, integration "belagerung" bis nach dem Tod
pruefen, exe-Laeufe E1/MN2/Plattformblock/Elza/A2 am Endstand, Abschluss.

### 13.3 RE-Belege der Fortsetzung (selbst gelesen)
- Stichprobe der Vorgaenger-Zitate (`re2_disasm.py --bin EM23_OVL_0000.BIN` bzw. PSX.EXE): @0x801010e8-f8 `addu a0,s0,zero / addiu
  a1,zero,4 / addu a2,zero,zero / jal 0x8001a330 / addu a3,zero,zero` = FUN_8001a330(em,4,0,0); @0x8001a34c-64 `lbu v0,332(s0) /
  beq v0,a1,0x8001a368 / sb a1,332(s0) / sb zero,333(s0) / sb s1,334(s0)` (Blend-Zaehler +0x14E = a2 nur bei Clip-Wechsel);
  @0x80101048-58 `addiu v0,zero,-23078 / sw v0,56(s0) / sh zero,554(s0) / sh zero,552(s0) / sb zero,548(s0)`. Stimmt.
- **Die Spieler-Wandklemme laeuft auch im Tod:** FUN_80031c44 (im Port-Kopf von re15_player_body_and_walls, game_step_common.c,
  selbst disassembliert dokumentiert) ruft nach JEDEM Kommando-Handler (Tabelle @0x80073f90, [3]/[7] = Tod) den Koerper-Schub
  `jal 0x8002b544` @0x80031cbc und die Wandklemme `jal 0x8003b0a4` @0x80031d70 (a2 = Solid-Maske 1, Radius 0x1c2); geklemmt wird am
  Spieler-Band +0x82 (= DAT_800acad6). Im Port laeuft sie vor der Gegner-KI (exe BL2: das P0-Bild zeigt Leon noch auf der P0-Lage,
  erst das naechste verschoben).
- **Das Band folgt der Hoehe:** Kletter-Landung `mult 0x91a2b3c5 / mfhi / sra 10 / subu` = Band -(h / 1800) @0x80038460-bc
  (climb_common.c:506), ebenso der Tuer-Spawn (aot_common.c:777, band_from_y(spawn_y)).
- **SCA ROOM2090** (Sonde `B41nb5c/probe_sca.c`, 50 Eintraege = 5 Quadranten-Gruppen x 10): Band 0 (floor >> 4 = 0) = [0]-[3]
  Aussenwaende x[-8900..7200] z[-27000..-5400], [4] Plattform x[-1700..1850] z[-20000..-12450], [5] Rampe x[1850..7200]
  z[-18100..-14500] - **genau die Beckenquelle gb_becken_draussen** der Buehne; Band 1 zusaetzlich [6] Ostwand, [7] Plattform, [8]
  Streifen x[1850..7200] z[-14500..-13500] (u1 02), [9] Streifen z[-19100..-18100] (u1 02). Sonde `probe_klemme.c`
  (re15_collision_constrain_contact_band, Radius 450, Maske 1, alt = neu): Band 1 schiebt (2564,-14017) -> (2564,-14968), Band 0
  laesst (2564,-14017) stehen.

### 13.4 Umsetzung (Endstand)
Vom Vorgaenger (0c101dd8, a06c52e0; Belege §13.0):
- enemy_ai_boss_gator.c: `gb_klemme_aus` auch in FRESSEN ab P0 (RE2 FUN_80015cb8 ohne Wand, @0x80015d68/@0x80015d90); P0: pitch/arc/
  jaw = 0 (RE2 +0x22A/+0x228 = 0 @0x80101050-54), anim_frac = 0 (FUN_8001a330(em,4,0,0) @0x801010ec-f4), Kanister-Objekt aus
  (@0x80101008-0c), Gator auf die Buehne (Form @0x80101048-4c), Leon auf den Beckenboden (pl->y = 0); Buehne planen und
  Schnitt-Auftrag im Biss-Bild (gb_biss_abschluss), Messhaken re15_gator_boss_im_fressen.
- gator_re2_r41.c: authored Finisher-Bahn gr2_fin_tabelle (P1 Clip 4 Bild 1..14, P2 sf 1..120, P3 Clip 11 Bild 0..58),
  Buehnen-Suche re15_gr2_finisher_buehne (**PORT-WAHL** nach Nachtrag 6; Form = RE2-P0, das den Gator auf die Korridor-Achse setzt und
  auf Cut 7 schneidet, dessen Bild die Lage frontal zeigt, §9.2): naechste Lage + Gier nach Kosten Versatz + 4 x Drehung (wie die
  Intro-Lage), bei der die ganze Bahn im Wasser liegt (Wurzel ausserhalb der Klemmzone 1466, Bones 1/5/6/7, Leons Wurzel mit
  Spielerradius 0x1c2), der Gator frontal zu einer Zonenkamera steht und Kopf + Leon im Bild sind (Leon nur oben hinaus), Kopf-
  Massstab <= RE2-Grenze 1892; re15_gr2_kamera_finisher schneidet auf die Buehnen-Kamera; Messhaken re15_gr2_fress_sitz.
Dieser Agent (242ac48f, 2c2bbcdd, f605f433 + Abschluss):
- **Band-Haken P0** (enemy_ai_boss_gator.c): nach `pl->y = 0` `re15_collision_set_band(re15_collision_band_from_y(pl->y))`
  (Band -(y/1800) @0x80038460-bc). Ursache des exe-Befunds BL2 (§13.5).
- **Buehne prueft jedes Bild** (gr2_fin_frei/gr2_fin_sicht): vorher Stichproben (Sicht erst ab dem Koppelbild, dann jedes 10. Bild,
  Wasser ab P2 jedes 2.); der Riegel `maul` fand im CHASE-Bezug P1 Bild 12 Leons Wurzel bei Bildlage (106,240), eine Zeile unter dem
  Bild. Jetzt erst Stichprobe (jedes 8. + letztes Bild), dann jedes Bild - dieselben Bedingungen; in der Raster-Sonde ist das
  Ergebnis gleich der reinen Jedes-Bild-Pruefung (Versatz-Verteilung 16609/2946/1339/337/89), 1,35 statt 2,97 ms je Suche.
- **GR2_FIN_WEIT 9000 -> 27000** = Becken-Diagonale der Aussenwaende (16100 x 21600 -> 26940, aufs Raster 250 aufgerundet). Der alte
  Wert (Kommentar "halbe Becken-Diagonale", die rechnerisch 13470 ist) lag mit 9000 nur knapp ueber dem gemessenen Bedarf 8997
  (Wurzel mitten auf der Rampe); die Suche bricht bei r >= besten Kosten ab - Wasserlagen unveraendert (Sonde: gleiche Verteilung,
  max 7494), Plattform/Rampe max 9996.
- re15_gr2_fress_sitz liefert `*kiefer_abw` = |(Leons Wurzel - Part 7) gezeichnet - dieselbe Groesse der authored Bahn| (Bahnbild k:
  P1 Clip-4-Bild 1..14 -> 0..13, P2 sf 1..120 -> 14..133, P3 Clip-11-Bild 0..58 -> 134..192; y: P1 Leon auf 0, Gator auf Buehnen-y).
- gator_re2_pc.c Messzeile `[gator-re2] Maul-Sitz F<n>: Bilder .. (P1 P2 P3), Abweichung max .., Kiefer-Abstand max .., Kopf im Bild
  ../.., Leon im Bild ../.. + oben hinaus .. (Cut ..), im Wasser ../..` und die Summenzeile `Maul-Sitz Ende` im ersten Bild nach P3.
- Keine neue Datei, keine neuen Assets, kein getenv. Gemeinsame Dateien: keine (enemy_ai_boss_gator.c = Eigentuemer B); der Riegel
  probe_gator_fress.c (unit_gator_fress, im Zweig seit NB1 angepasst) mit Beleg angepasst (§13.6).

### 13.5 Messung nachher (Laeufe `B41nb5c/mess/<Lauf>`, eigene exe-Kopien re15_pc_b41n5c(z)_<Lauf>.exe)
Riegel `unit_r41_re2gator_maul` (echter Boss-Takt, Spieler-Wandklemme vor jedem Takt wie im Spiel, Leon-Band bis zum Tod wie gesetzt;
je Bild ab P0 gegen den CHASE-Bezug im selben Bahnbild):

| Lage | Tod aus | Cut | P1/P2/P3 | Maul >= 50 % P1/P2/P3 | Kopf-lokal / Maulraum gg. Bezug | Kiefer-Abstand | Wasser / Bild aus | Klemme / Band falsch |
|---|---|---|---|---|---|---|---|---|
| CHASE-Bezug (-4000,-22000) | LUNGE Takt 7 | 0 | 14/120/120 | 0/106/120 | 0 / 0 | 3 | 0 / 0 (18 oben) | 0 / 0 |
| GUARD Rampe Ost (6300,-16250) | GUARD Takt 135 | 4 | 14/120/120 | 0/106/120 | 5 / 10 Promille | 2 | 0 / 0 (10 oben) | 0 / 0 |
| GUARD E1-Lage (6732,-16250) | GUARD Takt 138 | 4 | 14/120/120 | 0/106/120 | 5 / 10 | 2 | 0 / 0 (10 oben) | 0 / 0 |
| GUARD Rampenmitte (4000,-15000) | GUARD Takt 95 | 2 | 14/120/120 | 0/106/120 | 9 / 11 | 3 | 0 / 0 | 0 / 0 |
| GUARD Plattformblock (0,-16250) | GUARD Takt 323 | 7 | 14/120/120 | 0/106/120 | 8 / 11 | 2 | 0 / 0 (14 oben) | 0 / 0 |
| CHASE, Leon noch Band 1 (exe BL2) | LUNGE Takt 7 | 2 | 14/120/120 | 0/106/120 | 14 / 13 | 2 | 0 / 0 | 0 / 0 |

Vorher (Stand 7831a6d5, §13.1, gleiche Groessen): GUARD Kopf-lokal bis 5649, Maul P2/P3 0/0 bzw. 17/82. Gegenprobe Band-Haken
(Riegel gegen den Stand 242ac48f ohne Haken): BL2-Lage Klemme 930, Leon ausserhalb des Wassers, Band falsch in 254 Bildern -> 10 Fehler.
P1 = 0/14 auch im Bezug: Leon steht dort wie in RE2 (@0x8010106c-84) 7096 vor der Wurzel und wird in Bild 13 gepackt (@0x801010fc-104).
`unit_r41_re2gator_buehne`: 2128 Gator-Lagen (Raster 1000 x 8 Gier, Wasser + Plattform/Rampe), 0 ohne Buehne, 0 falsch, Versatz max 8984.

exe (Endstand; "Bau 14:31" = vor Band-Haken und Suchradius, an diesen Lagen gleich, da Leons Bahn dort in keiner Band-1-Zelle liegt und
die Kosten unter 9000 bleiben):

| Lauf | Rezept (Basis wie Abnahme 4 §3) | Tod | Buehne Versatz/Drehung -> Cut | Maul-Sitz Ende |
|---|---|---|---|---|
| E1 (Bau 14:31) | Tuerweg `DEBUG_JUMP=20A0@250 FIRE_AOT=0@40#20A0 GB_TEST=1 EXIT_AT=890#2090` | F627 GUARD | 249/1888 -> Cut 4 | F882: 254 (14/120/120), Abw. 0, Kiefer 2, Kopf 254/254, Leon 244/254 + 10 oben, Wasser 254/254 |
| MN3 | `DEBUG_JUMP=2090@250 PLAYER_POS=4000,-15000,2048,1 GB_TEST=1` | F587 GUARD | 0/1082 -> Cut 4 | F842: 254, Abw. 0, Kiefer 2, Kopf 254/254, Leon 244 + 10 oben, Wasser 254/254 |
| EZ3 (Elza, ROOM2091) | `PSELECT_AUTO(_SWITCH)`, `PLAYER_POS=6300,-16250,2048,1 GB_TEST=1` | F627 GUARD | 249/1888 -> Cut 4 | F882: wie E1 |
| A2 | Tuernische `DEBUG_JUMP=2090@250 GB_TEST=1` | F397 LUNGE | 749/458 -> Cut 6 | F652: 254, Abw. 0, Kiefer 2, Kopf/Leon 254/254, Wasser 254/254 |
| BL2 (Bau 14:31, OHNE Band-Haken) | `PLAYER_POS=0,-13200,2048,1 GB_TEST=1` | F526 LUNGE | 4222/850 -> Cut 2 | F781: **Kiefer 949 (F528), Wasser 241/254** - Leon (Band 1) von der Wandklemme aus SCA [8] in die Rampe geschoben |
| BL3 (Endstand) | wie BL2 | F526 LUNGE | 4222/850 -> Cut 2 | F781: 254, Abw. 0, **Kiefer 2**, Kopf/Leon 254/254, **Wasser 254/254** |
| BL1 | `PLAYER_POS=0,-16250,2048,1 GB_TEST=1`, 1500 Bilder | - | - | kein Tod: eine Raum-Spinne warf Leon ~F300 nieder, die Band-1-Klemme schob ihn aus der Plattformzelle [7] nach (-2168,-16250), der Gator blieb in CHASE (Altbestand, Spinnen = Spur F); der GUARD-Tod auf dem Plattformblock ist im Riegel `maul` (Lage D) gemessen |

Bildbelege (Framedump, RE15_WINDOW_SCALE=1, Ausschnitte 2x, Aufhellung nur zur Sichtung): **E1 vorher** = Abnahme 4
`B41abn4/beleg_E1_finisher.png`, `E1_fress_zoom2.png`, `E1_fress_p3_zoom.png` (Maul leer, Leon hinter der Ostwand); **E1 nachher**
`B41nb5c/E1_nachher_maul.png` (F626-F870) und `B41nb5c/E1_nachher_kopf.png` (F640 Schnapp, F650-F690 Leon im Maul, F700-F740 Wurf
und wieder im Maul, F750-F880 aufgerichtet kauend). Ebenso `MN3_nachher_kopf.png` (vorher Abnahme `B41abn4f/MN2_kopf_zoom.png`: Leon
neben/ueber dem Kopf), `BL3_nachher_kopf.png`, `EZ3_nachher_kopf.png` (Elza rot im Maul F650-F690, F740/F800), `A2_nachher_kopf.png`
(CHASE-Tod: Leon F398-F404 vor dem Gator, F416-F456 im Maul, F488-F504 im aufgerichteten Maul). In P1 steht Leon wie in RE2 vor dem
Gator; bei E1/MN3/EZ3 verdeckt eine Raum-Spinne im Vordergrund Teile von ihm (Spinnen entfernt Spur F).
A2 zeigt seit der Buehne die Frontalansicht (Cut 6, frontal 4118) statt der Schraegsicht (vorher Cut 6, frontal 2541) bei aehnlichem
Kopf-Massstab (712 gegen 691) - der RE2-Finisher-Schnitt Cut 7 ist frontal (+1,00, §9.2).
