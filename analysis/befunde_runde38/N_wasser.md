# Runde 38 — Spur N "wasser" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_wasser`, Zweig `r38/wasser`, Basis d70c234e (2026-10-16).

## Nutzer-Punkt (woertlich, AUFTRAG.md Z. 29)

> In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza im flachen Wasser bewegen.

## Stand

- [x] Messung vorher (Port d70c234e: 1260 Wasser-Ton da, Effekt fehlt; Becken 2090 Beton-Ton statt Watschritt)
- [x] RE-Beleg Original (B1-B10, selbst disassembliert; DuckStation-GDB-Gegenprobe 1260 + 2090, Bild + Ton-Index)
- [x] Raumliste (Zensus Datei-Bytes, neu gerechnet; Sichtlauf: FLR-0x97 = gruene Pfuetzen der Gaenge)
- [x] Umsetzung (FUN_80045630-Zwilling, Schritt am Bildwort des gezeichneten Bilds, op_aot_set-Breite)
- [x] Messung nachher (Leon + Elza, alle Wasser-Raeume des Zensus; Bild am Original, Ton per Mitschnitt-Korrelation)
- [x] Tests (unit_r38_wasser_* 7x, integration_r38_wasser; gezielte Gruppen)
- [x] Lauf 3 (Abschluss 2026-10-17): Endstand gebaut, 20A0-/2090-Band-Messung an den Zonen-Bytes eingeordnet (B11),
      Zensus op_aot_set auf Breite UND Tiefe erweitert, Elza ueber den echten Tuerweg 10B1 -> 1261, gezielte Tests am Endstand

## Messung vorher

Gebauter Stand d70c234e (Baum r38_wasser, `local_build.sh configure` + `build` OK), Messkopie
`re15_pc_r38w_mess.exe`, Lauf: `RE15_DEBUG_JUMP=1260@240 RE15_PLAYER_POS=-8600,-7900,0,1`
(Band 1 = Laufsteg y -1800, Sprung-Spawn des Debug-Menues ist (-8750,-1800,-6900)),
`RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT="W0.5,U1.5,W1" START=60`, `RE15_FOOT_DEBUG=1`,
`RE15_STATE_LOG`, `RE15_AUDIO_CAP_SYNC`, Framedump 90-150/6.

- Weg: F72 (-8404,-7246) -> F92 (-7054,-7246), Wand bei x -7068 (Kasten 12 endet bei -6600, Radius 450).
- Schritte (debug.log): `[foot] SE foot=7 type=29 vag=1`, dann `foot=4 type=151 vag=7`,
  `foot=7 type=151 vag=7` = zwei Schritte im FLR-Fleck P0 (x -7800..-6600, z -8700..-7100, Typ 0x97).
  **Ton: vorhanden** (0x97 & 0x7f = Satz 0x17 -> VAG 7, vab_common.c maskiert Bit 0x80 seit Audit wf_1db9c802).
- Effekt-Pool (state.log Feld `fx=` = re15_esp_fx_count): **0 in jedem Bild F72-F100** — kein Spritzer.
- Code-Befund: `re15_audio_footstep(foot, re15_rdt_floor_sound(...))` (game_step_common.c:2159) ist
  der EINZIGE Verbraucher; der FX-Zweig von FUN_80045630 fehlt im Port vollstaendig, `water_y` (+0x88)
  wird von aot_common.c:1363 gestempelt, aber von NIEMANDEM gelesen und NIE geloescht.

### Raum-Zensus (Datei-Bytes, alle 206 nichtleeren RDTs, Skript flr_census.py / sce8_scan.py)

FLR-Wasserboden (Typ-Byte +8 mit Bit 0x80, alle 0x97):
| Raum | Flecken 0x97 | ESP-Ids (RDT+0x4C) |
|---|---|---|
| ROOM1260/1261 | 6 | 05 06 07 0b 0c 0d 0f |
| ROOM2000/2001 | 6 (+1x 0x17 ohne Bit) | dito |
| ROOM2050/2051 | 7 | dito |
| ROOM20B0/20B1 | 6 (+1x 0x17) | dito |

sce-8-Wasserzonen (Aot_set 0x2C, sce 8, Flags 0x47; Nutzlast u16 = Wasserspiegel):
11A0/11A1 (2, -900), 1260/1261 (2, -900), 2000/2001 (2, -900), 2020/2021 (1, -900),
2070/2071 (2 Saetze in zwei Zweigen, -200), 2090/2091 (3, -1620), 20A0/20A1 (2, -540, eine Band 1),
20B0/20B1 (2, -900) = 32 Saetze (wie aot_common/scd_vm "32 zones").
In 1260/2000/2020/20B0 decken sich die Zonen mit SCA-Kaesten u0=0x01 u1=0x02 Ebene 0x10 (Rinnen);
in 2070, 2090, 20A0 (Zone 2) und 11A0 liegt KEIN SCA-Kasten im Zonenmittelpunkt.
Effekt-Id 6 ist in allen Raeumen mit Wasserboden in der Raumbank (CORE00 traegt nur 0,2,3,4,8).

## RE-Belege

Alles selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE).
RE1.5 hat den Mechanismus vollstaendig — kein RE2-Ersatz noetig.

### B1 Schritt-Funktion FUN_80045630(a0 = Tonversatz, a1 = Part-Index) — FX-Zweig
```
80045630 lw v0,-14460(v0)        g_entity(cur) (0x800ac784)
80045664 lw a0,52(v0) / 80045668 lw a1,60(v0)   x (+0x34), z (+0x3c)
8004566c jal 0x800437d4          FLR-Bodentyp an (x,z) -> s0
8004567c..800456a8               a2 = *(ent+0x188) + a1*0xac   (Part-Array, Stride 0xac)
800456ac andi v0,s0,0x80 / 800456b0 beq -> 800456dc
  Wasserboden (Bit 0x80):
800456b8 lui a0,0x605 / 800456bc ori a0,a0,0xd00      a0 = 0x06050D00 (Effekt 6, Sub 5, Skala 0x0D00)
800456c0 addiu a2,a2,64                               a2 = Part[a1] + 0x40 (Weltmatrix)
800456c4 sw zero,16(sp) / 800456c8 sw v0(=0x64),20(sp) / 800456cc sw zero,24(sp)   Versatz (0,100,0)
800456d0 lh a1,106(a1)                                a1 = ent+0x6a (Gier)
800456d4 j 0x80045710 / 800456d8 addiu a3,sp,16
  sonst:
800456dc lh v0,136(a1) / 800456e4 beq v0,zero,0x80045718   ent+0x88 (Wasserstempel) == 0 -> kein FX
800456e8 ori a1,zero,0x800                            a1 = 0x800
800456ec addiu s0,a0,-3                               Bodentyp - 3 (Watsatz)
800456f0 lui a0,0x603 / 800456f4 ori a0,a0,0x2000     a0 = 0x06032000 (Effekt 6, Sub 3, Skala 0x2000)
800456f8 addiu a2,a2,-108                             a2 = Part[a1]+0x40-0xac = Part[a1-1]+0x40
800456fc..8004570c                                    Versatz (0,100,0)
80045710 jal 0x80019700                               Spawn
```
### B2 Schritt-Funktion — Tonindex
```
80045718 andi v0,s0,0x7f / 80045724 addu a0,s3,v0     Index = a0 + (s0 & 0x7f)
80045728 lbu v0,8(v1) / 80045730 sltiu v0,v0,0x40      Typ < 0x40 (Spieler):
80045774 lw v0,-13764(v0) (0x800aca3c) / 80045780 andi 0x4000 / 80045790 ori s3,zero,0x9   auf Objekt -> 9
8004573c..8004575c                                     Typ >= 0x40 (NPC): Index - 8
80045784 lw a2,8(v1) (0x800ac778+8 = snd0-EDT) / 80045798 sll 2 / 8004579c addu   EDT[Index]
```
EDT snd0 (RDT+0x08) je Material drei Saetze: Gehen / +1 Laufen / +2 Fall. ROOM1260:
0x17/0x18/0x19 = `00007312`/`00008312`/`00009312` (Wasser), 0x1a/0x1b/0x1c = `00004312`/`00005312`/`00006312`
(Waten = 0x1d-3), 0x1d/0x1e/0x1f = `00001312`/`00002312`/`00003312` (Beton). Gleich in 2000/2050/2070/2090/20A0/11A0.

### B3 Aufrufer (Spieler) — a0/a1
Echtzeit-Bewegung = Kommando 1 (Tabelle @0x80073f90[1] = 0x80031de8), Sub-Update-Tabelle @0x80073ff0:
| Sub | Handler | Aufruf | a0 | a1 |
|---|---|---|---|---|
| 1 Gehen | 0x80032498 | @0x8003255c | 0 (`addu a0,zero,zero` @0x80032544) | 7 - 3*Bit12 des Bildworts (@0x80032548-60) |
| 2 Laufen | 0x80032778 | @0x80032868 | **1** (`ori a0,zero,0x1` @0x80032850) | dito |
| 3 Rueckwaerts | 0x80032ae4 | @0x80032b84 | 0 (@0x80032b6c) | dito |
| 4 Drehen | 0x80032d20 | @0x80032de8 | 0 (@0x80032dd0) | dito |
Plc-Skriptlauf (Kommando 4 = 0x80030660, Tabelle @0x80073e30): 0x80030af0 Gehen @0x80030bdc/c3c, 0x80030d28
Laufen @0x80030e30/e90, 0x80031080/0x800311f0/0x80031360 @0x80031124/1294/1478 — ALLE a0 = 0
(@0x80030e18, @0x80030e7c, @0x80031110, @0x80031280, @0x80031460).
Reihenfolge im Geh-Executor: Schritt @0x8003255c VOR anim_set @0x80032578 und pos_advance @0x800325ec
-> Boden-Abfrage und Part-Matrix sehen die Lage VOR der Bewegung dieses Bilds.
Nicht-Bewegung: `FUN_80045630(2,0)` = Koerperfall (Tod @0x80036804, Hunde-Finisher @0x80111da4,
Affen @0x8011c4f0) — nicht Teil dieses Punkts (OFFEN).

### B4 Spawner FUN_80019700 (a0 = Id<<24|Sub<<16|Skala, a1 = +0x2e, a2 = Matrix, a3 = Versatz)
```
800197d0 sb v0(=3),108(t0)          Flags 0x03
800197d4/d8/dc sb t8,112 / sb t7,113 / sh s1,114     Id, Sub, Skala
800197e4 sh s2,46(t0)               +0x2e = a1
800197e8..80019804                  Versatz a3[0..3] -> +0x40/44/48/4c
8001980c / 8001981c sw a2,116(t0)   +0x74 = Matrixzeiger (Folgen)
80019820..8001985c                  Matrix (32 Byte) -> +0x4c..+0x68 (R +0x4c, T +0x60/64/68)
```
### B5 Weltlage je Takt (FUN_80019e20, Flags & 0x80 == 0)
```
80019f54 andi v0,v1,0x4 / 80019f60 lw v0,116(a3) / 80019f68..80019fa4   Folgen: Matrix aus +0x74 neu kopieren
8001a16c..8001a1a0  RotMatrix(Euler + (0,+0x2e,0))  `jal 0x80068098`
8001a1e4            ApplyMatrix(.., Drift +0x34/38/3c)      -> +0x28/2a/2c
8001a248            ApplyMatrix(Anker.R +0x4c, Versatz +0x40/44/48)
8001a258..8001a2a4  +0x28.. += r2 + Anker.T (+0x60/64/68)
```
Port: esp_fx_weltlage (re15_esp.c) rechnet genau das, mit (x,y,z) = Anker.R*Versatz + Anker.T.

### B6 Part+0x40 ist die WELTmatrix
FUN_8001e9ec (Part-Zeichner): `m1 = param_1 + 0x10` (= +0x40), `FUN_80022da0(param_1[0x1b] (Eltern-Matrix),
param_1 + 6 (+0x18 lokal), m1)` = CompMatrix(Eltern-Welt, lokal) -> +0x40; danach erst
`FUN_80022da0(&DAT_800b5288 (Sicht), m1, ..)` fuer die Projektion. Sie wird beim ZEICHNEN gefuellt, also traegt
sie beim Schritt (Update) die Pose des zuletzt gezeichneten Bilds.

### B7 Wasserstempel +0x88 — Zeitfolge
Hauptschleife: `jal 0x8001a50c` @0x8001ce04 (Gegner; je Handler `sh zero,0x88` @0x8001a59c),
`jal 0x80031c44` @0x8001ce0c (Spieler: Kommando-Dispatch @0x80031cb4 = Schritt liest +0x88, dann
`sh zero,-0x3524(at)` = 0x800acadc = Spieler+0x88 @0x80031da4), `jal 0x8002bd44` @0x8001ce14 (Objekte),
`jal 0x800436a8` @0x8001ce1c (AUTO-Scan: sce 8 -> Handler @0x8007469c[8] = 0x8004330c `sh v0,136(entity)`),
`jal 0x80019e20` @0x8001ce2c (ESP-Takt). Der Schritt sieht also den Stempel des VORBILD-Scans.
Spieler-Tick setzt g_entity(cur) = 0x800aca54 (`addiu s0,v0,720` / `sw s0,0(v0)` @0x80031c64-6c).

### B8 Effekt 6 in den Raumbanken (Datei-Bytes ROOM1260.RDT, gleich in 1261/2000/2050/20B0)
Effekt-Id 6 = Index 1, Koerper @0x2dd4, count_a 45, count_b 41, Zeilenblock @0x2fe8, Sub-Tabelle
[4, 46, 110, 174, 196, 218, 240, 262]:
- Sub 3 @0x32a0: 1 Strom, 2 Zeilen; Zeile 0 A=10 B=0 acc (-3,10,0) Flags 0x07 (Bit 2 = FOLGEN) vel (120,-60,0)
  +0x16 = 0x60 (ABR 3); Zeile 1 A=0 acc (-3,10,0) vel (117,-50,0).
- Sub 5 @0x3350: 1 Strom, 2 Zeilen; Zeile 0 A=10 B=0 acc 0 Flags 0x03 vel 0 +0x16 = 0x60; Zeile 1 A=0.
Routine 10 (@0x800176b0: Flags := Zeile[0x0e] @0x800176c0/c8, TPAGE |= Zeile[0x16] @0x800176d8-ec, Vorschub)
und Routine 0 sind im Port vorhanden (re15_esp.c case 10). Kein Routine-B-Boden (B = 0) -> keine Bodenklemme.

## Fortsetzung (Lauf 2, 2026-10-17) — Pruefung des Vorgaenger-Stands

Vorgaenger (wf_9fc9967b-c4b) vom Sitzungslimit beendet; Stand 67aa565c. Geprueft, nicht wiederholt:

- **RE-Belege selbst nachdisassembliert** (re15_disasm.py, info/Re1.5/PSX.EXE): B1/B2 (FUN_80045630 @0x80045630-0x8004580c,
  jede zitierte Instruktion stimmt; Part-Stride `0xac` = Schiebe-/Subtraktionskette @0x8004567c-0x800456a4 = a1*172),
  B3 (Geh-Executor 0x80032498: `lw v0,-13380` = 0x800acbbc Bildwort-Zeiger, `andi 0x4000` @0x8003253c, a0 = 0
  @0x80032544, a1 = 7-3*Bit12 @0x80032548-60, Lauf 0x80032778 `ori a0,zero,0x1` @0x80032850, Tabellen @0x80073ff0
  [1..4] = 0x80032498/0x80032778/0x80032ae4/0x80032d20, @0x80073f90[1] = 0x80031de8), B4 (FUN_80019700: erster freier
  Platz `sltiu v0,t3,0x60` @0x8001978c, Flags 3 @0x800197b4/d0, +0x2e = a1 @0x800197e4, Versatz a3 -> +0x40..
  @0x800197e8-804, +0x74 = a2 @0x8001980c/1c, Matrix -> +0x4c..+0x68 @0x80019820-5c), B5 (Hauptlauf Flags&0x80 == 0
  @0x8001a118-0x8001a2a4: RotMatrix(Euler + (0,+0x2e,0)) * xlat + Anker.R*Versatz + Anker.T; FOLGEN @0x80019f54-a4),
  B6 (FUN_8001e9ec: m1 = Part+0x40 = CompMatrix(Eltern, lokal)), B7 (Hauptschleife @0x8001ce04-2c: Gegner, Spieler
  0x80031c44, Objekte, AOT-Scan 0x800436a8, ESP-Takt; Spieler+0x88 := 0 `sh zero,-13604(at)` @0x80031da4 NACH dem
  Dispatch @0x80031cb4; Bit 0x4000 von 0x800aca3c geloescht @0x80031cdc-e4, neu gesetzt @0x80031d24 = der Schritt
  sieht das Bit des Vorbilds). FLR-Abfrage FUN_800437d4 @0x800437d4-0x80043848: nur x/z (`lh`/`sltu`), KEINE Hoehe.
- **Raum-Zensus** neu gerechnet (scratchpad N38w/wasser_zensus.py, Baum-Assets): deckungsgleich mit dem Vorgaenger.
- **Was ist "flaches Wasser"?** Sichtlauf aller sechs FLR-0x97-Flecken von ROOM1260 (Leon auf die Fleckmitte, Kamera per
  RVD-Ankerregion, RE15_FORCE_CUT): P1 (Cut 1), P2 (Cut 7), P3 (Cut 8), P4/P5 (Cut 15) liegen genau auf den
  gruenlichen PFUETZEN der Gangboeden im Hintergrundbild. Die sce-8-Zonen (Spiegel -900, Band 0) sind die tiefen
  Rinnen neben den Gaengen (Kanalwasser, Gegner/Spur U). => Fuer Leon/Elza ist das flache Wasser der FLR-0x97-Zweig
  (Effekt 6 Sub 5 + Wasserschritt EDT 0x17); der Stempel-Zweig (Effekt 6 Sub 3 + Watschritt) greift dort, wo der
  Spieler in einer sce-8-Zone steht (2090: Becken, 20A0 Zone 5 auf Band 1).
- **Messung nachher reproduziert** (exe Stand 67aa565c, Leon 1260 P0): F97/F112 `zweig=boden code=0x06050d00`,
  Anker = Fuss-Part (Part 4/7 = linker/rechter Fuss, stair_common.c STAIR_FOOT_L/R) + (0,100,0).

### B9 Original-Gegenprobe DuckStation (Lauf 2, 2026-10-17) — Bild, Ton-Index, Zeitfolge

Werkzeug: scratchpad `N38w/ds_wasser.py` / `ds_paket.py` (MZD-Disc = Auslieferungs-EXE, Spielstand
`stage_saves/mzd_debugmenu.sav`, JUMP 0x126 per virtuellem Pad, GDB 127.0.0.1:2345, Leon per RAM auf (-8404,-7246)
Blick 0 gestellt = dieselbe Lage wie der Port-Lauf, UP gehalten; settings.ini gesichert und zurueckgeschrieben).
Raum belegt: FLR-Zeiger *(0x800ac778)+0x34 -> erster Eintrag x -7800 z -8700 w 1200 d 1600 Typ 0x97 (= ROOM1260 P0).
Haltepunkte @0x80045630 (Eintritt), @0x80045710 (Spawn-jal), @0x80045798 (Tonindex in v0), @0x800534c4/@0x800537b4
(Sprite-Bau, POLY_FT4 gelesen). Ergebnis (drei Laeufe, deterministisch gleich):
```
EIN   a0=0 a1=4 Leon=(-7068,-1800,-7246) clip(+0x94)=5 bild(+0x95)=23 +0x168=801d79cc wort=0000507c
SPAWN a0=06050d00 a1=0 versatz=(0,100,0) s0=0x97 anker=(-7555,-1873,-7385) T=(-7547,-1972,-7386)
TON   index(v0)=0x17
EIN   a0=0 a1=7 ... bild(+0x95)=8 +0x168=801d7990 wort=0000606d
SPAWN ... anker=(-7595,-1887,-7100) T=(-7586,-1986,-7097)
PAKET code=2c (deckend, kein ABE) rgb=808080 clut=0x7852 tpage=0x006f, je Bild c0 0..12:
      c0=0 xy (140,166)-(143,169) uv (224,96); c0=1/2 (138,162)-(145,169); c0=3 (136,160)-(147,171);
      c0=4..12 (135,159)-(148,172) uv (184,120)/(216,120)/(0,72)/(32,72)/../(192,72)   [v = Datei-v + 72 VRAM-Versatz]
```
Medienmitschnitt (Bild+Ton, `ds1260_c/*.mp4`): der Spritzer am hinteren Fuss waechst, wird weisslich und zerfaellt
in Tropfen, je Schritt ~13 Bilder.
**Zeitfolge-Befund:** *(+0x168) zeigt beim Schritt auf Bild (+0x95 - 1): anim_set (FUN_8001f314) setzt
`sw a2,360(t0)` @0x8001f36c auf das Bild +0x95 und schiebt erst danach vor (`lbu`/`addiu`/`sb v0,149` @0x8001f610-1c).
Der Schritt des Bilds N prueft also das Bild, das N-1 gezeichnet hat, mit dessen Pose (Part+0x40).
Der Vorgaenger-Stand pruefte das Bild NACH dem Vorschub dieses Takts (pl->anim_frame, re15_player_tick
`anim_frame++` player_common.c:860) = einen Takt zu frueh und mit der Pose davor: Anker (-7502,-1858,-7381) statt
(-7555,-1873,-7385); der Spritzer sass 53 Einheiten zu weit vorn und halb unter Leons Schatten (Schatten-Bucket
< Effekt-Bucket 99 -> Schatten darueber, re15_pri_schatten_r36.h) — darum wirkte er im Port kleiner und dunkler.
**CLUT:** FUN_800194f8 ueberschreibt das Kopf-CLUT-Wort (`jal 0x8006b3d8` GetClut(0x120, s4) / `sh v0,4(s1)`
@0x80019640-48, s4 = 480 + Summe der CLUT-Hoehen der vorigen Effekte @0x8001954c/0x800196c4) = erste Palettenzeile
des EIGENEN TIM-Blocks; der Port nimmt fuer Raumeffekte die erste Palette des Effekt-TIMs = dieselbe Zeile (kein Defekt).


### B10 sce-8-Zonen: Breite/Tiefe sind vorzeichenlos (FUN_80042b64)
```
80042b68 lh  v0,0(a1)    x  (s16)          80042b84 lh  v0,2(a1)    z  (s16)
80042b6c lw  v1,0(a2)    Figur x (s32)     80042b88 lw  v1,8(a2)    Figur z
80042b70 lhu a0,4(a1)    BREITE (u16!)     80042b8c lhu a0,6(a1)    TIEFE (u16!)
80042b74 subu v1,v1,v0   / 80042b78 sltu a0,a0,v1 -> ausserhalb, wenn Breite < (x - x0) vorzeichenlos
```
Der Port las im Kurzform-Aot_set (op_aot_set, scd_vm.c) Breite/Tiefe als s16. Zensus aller Kurzform-Saetze 0x2C/0x3B/0x50
(Walker tools/aot_sce_census.py): genau **8** Saetze mit Breite > 32767, **alle sce 8** — 11A0/11A1 @0x1108 (49050),
1260/1261 @0x11ca (38950) und @0x11de (46000), 2020/2021 @0x8ac (34100). Diese Wasserzonen stempelten im Port nie
(Mitte/Halbmass aus negativer Breite). Fuer Leon/Elza ohne Folge (Rinnen = SCA-Waende bzw. Band 0 unter dem Laufsteg),
fuer Figuren im Kanal (Gegner) ja; Stempel-Leser ist bisher nur der Schritt-Zwilling.
Lauf 3 (Zensus erweitert, `N_belege/breite_zensus.py` ueber aot_sce_census.walk): 1859 Kurzform-Saetze mit Rechteck
(0x2C 1046, 0x3B 649, 0x50 164); Breite ODER Tiefe > 32767 haben genau dieselben 8 Saetze, alle op 0x2C, alle sce 8, Tiefe
ueberall <= 3650. Fuer jeden anderen Satz ist u16 == s16 — die Aenderung wirkt nur auf diese 8 Wasserzonen.

### B11 Original-GDB ROOM20A0 (Band-1-Zone) + Band-Gegenprobe ROOM2090 — eingeordnet (Lauf 3)
Gemessen in Lauf 2 (`N_belege/gdb_20a0_band1_zone.txt`, `N_belege/gdb_2090_band1_gegenprobe.txt`; `ds_wasser.py` mit DS_BAND=1
schreibt +0x38/+0x42 = -1800 und +0x82 = 1). Zonen aus den Datei-Bytes (`N_belege/wasser_zensus.py 2090 20A0`, Aot_set 0x2C sce 8):

| Raum | Satz | Band (rec+2) | x | z | Spiegel |
|---|---|---|---|---|---|
| 2090 | main00 @0x9d6 slot 2 | 0 | -8900..7200 | -27000..-18100 | -1620 |
| 2090 | main00 @0x9ea slot 3 | 0 | -8900..7200 | -14500..-5400 | -1620 |
| 2090 | main00 @0x9fe slot 4 | 0 | -8900..-1700 | -18100..-14500 | -1620 |
| 20A0 | main00 @0x1810 slot 5 | 1 | -3600..20550 | -26900..-23400 | -540 |
| 20A0 | main00 @0x1824 slot 6 | 0 | 20550..24100 | -26900..50 | -540 |

- **20A0:** Leon auf Band 1, Schritte bei x 8696 / 9850 / 11032, z -25000 = in slot 5 (Band 1). Je Schritt +0x88 = -540,
  SPAWN a0 = 0x06032000, a1 = 0x800, a2 = Part[a1]+0x40-0xac (a1 = 7 -> 0x801d38e0, a1 = 4 -> 0x801d36dc, Abstand 0x204 = 3*0xac),
  Anker relativ zu Leon (-233,-836,+174) / (-216,-837,-166), TON v0 = 0x1a (= 0x1d - 3). Port (Raumliste unten): zweig=stempel
  -540, Part 6/3, edt 0x1a — gleich. Damit ist 20A0 der Raum, in dem der Stempel-Zweig auf dem LAUFSTEG (Band 1) greift.
- **2090 (Gegenprobe):** vorher Leon im Becken (-8200,0,-25550) Band 0 mit +0x88 = -1620; auf Band 1 gestellt laufen alle drei
  Schritte bei (6732,-1800,-25000) = INNERHALB des x/z-Rechtecks von slot 2 (x <= 7200), aber Band 1 != Zonen-Band 0
  -> +0x88 = 0, KEIN SPAWN, TON 0x1d. Mechanismus (selbst disassembliert, Lauf 3):
  ```
  80042cac lbu  v0,2(s0)        Band-Byte des Satzes (rec+2 = Aot_set pc[4])
  80042cb4 andi v0,v0,0x80 / 80042cb8 bne v0,zero,0x80042cd4     Bit 0x80 = Band egal
  80042cc0 lbu  v1,130(s1)      Figur +0x82 (Band)
  80042cc4 lbu  v0,2(s0) / 80042ccc bne v1,v0,0x8004301c          ungleich -> naechster Satz (vor der Geometrie @0x80042cd4)
  ```
  Port: aot_common.c Wasser-/Rampen-Zweig `if (!(a->band & 0x80) && g_actors[es].floor != a->band) continue;` — gleich.

## Umsetzung

Dateien (eigene): `re15_port/engine/src/wasser_r38.c`, `re15_port/include/re15_wasser_r38.h`.
Haken in gemeinsamen Dateien (klein, kommentiert "Runde 38 Spur N"):
- `engine/src/game_step_common.c`: `re15_wasser_r38_bildanfang(c)` nach dem Positions-Spiegel (Schnappschuss des
  gezeichneten Bilds, Stempel +0x88 latchen und loeschen @0x80031da4); der alte FOOTSTEP-Block (15 Zeilen) ist durch
  `re15_wasser_r38_fusstakt(c)` ersetzt (Logik jetzt in wasser_r38.c).
- `platform/pc/main.c`: `re15_wasser_r38_zeichner(...)` im Spieler-Zeichenblock neben der Hand-Fuetterung (Part+0x40
  aller Parts je Bild, wie FUN_8001e9ec).
- `engine/src/scd_vm.c` op_aot_set: `rect_w`/`rect_d` als u16 (B10), 2 Zeilen + Kommentar.

Mechanik (re15_wasser_r38.h, jede Konstante mit Adresse):

| Konstante | Wert | Beleg |
|---|---|---|
| Effekt Boden-Zweig | 0x06050D00 (Id 6, Sub 5, Skala 0x0D00) | `lui a0,0x605` / `ori a0,a0,0xd00` @0x800456b8/bc |
| Effekt Stempel-Zweig | 0x06032000 (Id 6, Sub 3, Skala 0x2000) | `lui a0,0x603` / `ori a0,a0,0x2000` @0x800456f0/f4 |
| Wasser-Bit | 0x80 | `andi v0,s0,0x80` @0x800456ac |
| Versatz | (0,100,0) | `ori v0,zero,0x64` @0x800456b4 / @0x80045700, `sw` @0x800456c4-cc |
| Gier Stempel | 0x800 | `ori a1,zero,0x800` @0x800456e8 |
| Part Stempel | a1 - 1 | `addiu a2,a2,-108` @0x800456f8 |
| Watsatz | Bodentyp - 3 | `addiu s0,a0,-3` @0x800456ec |
| Tonindex | a0 + (Typ & 0x7f), Objekt -> 9 | @0x80045718-24, `ori s3,zero,0x9` @0x80045790 |
| a0 Lauf | 1 (Echtzeit-Lauf), sonst 0 | `ori a0,zero,0x1` @0x80032850; 0 @0x80032544/b6c/dd0, Plc @0x80030e18/e7c |
| Schritt-Bild | Bildwort des gezeichneten Bilds | `sw a2,360(t0)` @0x8001f36c vor `sb v0,149` @0x8001f61c; Pruefung @0x8003252c-3c |

Spawn ueber den vorhandenen FUN_80019700-Zwilling `re15_esp_fx_spawn_rows` (Flags 3); Anker = Part-Weltmatrix des
gezeichneten Bilds * (0,100,0) + T (Zeichner-Fuetterung, Rueckfall: Pose des Schnappschusses mit derselben Bankwahl);
FOLGE-Plaetze (Sub 3, Zeilen-Flags 0x07 Bit 2) werden je Bildanfang auf den Part-Anker gesetzt (@0x80019f54-a4).

**Lauf-2-Korrektur am Vorgaenger-Stand:** Schritt-Erkennung + Pose am GEZEICHNETEN Bild (B9). Vorher pruefte der
Port das Bildwort nach dem Vorschub dieses Takts = einen Takt zu frueh mit der Pose davor (Anker 53 Einheiten daneben,
Spritzer halb unter Leons Schatten). Betrifft alle Schritt-Toene: sie kommen jetzt einen Takt spaeter, wie im Original.
Kein Regressions-Fall (Bisect entfaellt): der Effekt-Zweig fehlte im Port seit der ersten Schritt-Ton-Umsetzung.

## Messung nachher

Bau Stand 957e81f2 (Messkopie `re15_pc_r38w_mess.exe` = re15_pc.exe). Schienen: `RE15_WASSER_LOG` (je Schritt eine
Zeile, neu), `RE15_FX_LOG` (vorhandene Zeichen-Spur), `RE15_TRILOG` (Dreiecke), `RE15_FOOT_DEBUG` (`[foot] SE`),
`RE15_AUDIO_CAP_SYNC` (Ton-Mitschnitt), `RE15_FRAMEDUMP` (WINDOW_SCALE 1).

**Leon ROOM1260** (Sprung 1260@250, (-8600,-7900) Band 1 Blick 0, `W0.5,U1.5,W2` ab Bild 60):
```
F83  fuss=7 bild=7  akt=8  lage=(-7804,-1800,-7246) boden=0x1d zweig=trocken edt=0x1d
F98  fuss=4 bild=22 akt=23 lage=(-7068,-1800,-7246) boden=0x97 zweig=boden code=0x06050d00 part=4 anker=(-7555,-1873,-7385) edt=0x17
F113 fuss=7 bild=37 akt=38 lage=(-7068,-1800,-7246) boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-7595,-1887,-7100) edt=0x17
```
= Original-GDB (B9) Wert fuer Wert: Bild +0x95 = akt (8/23), Bildwort[bild], Part, Anker, Tonsatz.
RE15_TRILOG des Spritzers (Part-4-Schritt): c0=0 (140,166)-(143,169) uv(224,24); c0=1/2 (138,162)-(145,169);
c0=3 (136,160)-(147,171); c0=4..12 (135,159)-(148,172) — **deckungsgleich mit den Original-POLY_FT4** (Datei-v =
VRAM-v - 72; FUN_800194f8 addiert den VRAM-Versatz @0x80019680-8c). Bild: `N_belege/vergleich_original_leon_elza.png`
(oben Original-Mitschnitt, Mitte Port Leon, unten Port Elza; der Spritzer am hinteren Fuss waechst, wird weisslich,
zerfaellt in Tropfen, ~13 Bilder).

**Ton** (Mitschnitt RE15_AUDIO_CAP_SYNC, Korrelation mit den dekodierten VAGs der Raum-Bank snd0, `N_belege/vag_korr.py`):

| Lauf | Schritt | bester VAG (r) | naechster |
|---|---|---|---|
| Leon 1260 | F83 Beton | EDT 0x1d / VAG 1 (0.46) | 0.19 |
| Leon 1260 | F98 Pfuetze | EDT 0x17 / VAG 7 (0.48) | 0x18 = derselbe VAG, andere Tonhoehe (0.31); Beton 0.12 |
| Leon 1260 | F113 Pfuetze | EDT 0x17 / VAG 7 (0.43) | 0.28 |
| Elza 1261 | F84/F98/F114 Pfuetze | EDT 0x17 / VAG 7 (0.42/0.48/0.43) | Beton <= 0.12 |
| Leon 2090 | F38 Becken | EDT 0x1a / VAG 3 (0.48) | Beton VAG 1 0.04 (= was d70c234e dort spielte) |

Original-Tonindex (GDB @0x80045798, v0): 1260 Pfuetze 0x17, Beton 0x1d; Lauf 0x18 / 0x1e; Becken 0x1a.
Gegenprobe Band-Sperre im Original (`N_belege/gdb_2090_band1_gegenprobe.txt`): ROOM2090 auf Band 1 (y -1800) in der
Becken-Zone (Band 0) -> +0x88 = 0, kein Spawn, Tonindex 0x1d (Port: Band-Gate je Figur aot_common.c, gleich).

**Elza** (Spielerwahl, PL04, ROOM1261 gleiche Lage): F84 bild=8 Part 7 (x -7729: schon im Fleck), F98 bild=22 Part 4
anker=(-7578,-1862,-7336), F114 bild=38 — zweig=boden code=0x06050d00 edt=0x17; Zeichner id=6 sub=5 in 39 Bildern.

**Alle Wasser-Raeume des Zensus** (Leon / Elza, RE15_WASSER_LOG):

| Raum | Weg | Ergebnis |
|---|---|---|
| 1260 / 1261 | P0 (s.o.) | zweig=boden 0x06050d00, edt 0x17 |
| 2000 / 2001 | Fleck P2 (-7260,-6000) Blick 1024/3072 | zweig=boden, Anker y -73/-87 ueber Boden 0, edt 0x17 |
| 2050 / 2051 | Fleck P0 (1100,550) | zweig=boden im Fleck, danach trocken 0x1d |
| 20B0 | Raum-Skript (Flut-Szene, Leon rennt auf Band 1) | Fleck 0x97 -> boden; Fleck 0x17 OHNE Bit -> kein Effekt, Wasser-Ton 0x17 (Original-Daten) |
| 2090 / 2091 | Becken (-8438,-25550) | zweig=stempel -1620, 0x06032000 Part 6, edt 0x1a; Original-GDB: Anker relativ zu Leon (-233,-836,+174) = Port |
| 20A0 / 20A1 | Zone 5 Band 1 (8000,-25000) | zweig=stempel -540, Part 6/3, edt 0x1a; Original-GDB (`N_belege/gdb_20a0_band1_zone.txt`): +0x88 = -540, SPAWN 0x06032000, Anker relativ (-233,-836,+174) = Port, TON 0x1a |
| 11A0, 2020 | Zonen Band 0 unter dem Laufsteg | Spieler erreicht sie nicht (Sprung-Spawn Band 1, Gelaender); Stempel jetzt korrekt (B10) |
| 2070 (TUN. FLOOD DRY) | Zonen in zwei Skript-Zweigen | im Sprung-Zustand keine Zone aktiv, Schritt 0x1d |

**Lauf (Original-GDB):** a0 = 1, Bildwort[0] = 0x5000 und [12] = 0x700c -> beide Part 4, Anker (-6548,-1891,-7302) /
(-7872,-2875,-7442), Tonsatz 0x18 (Pfuetze) / 0x1e (Beton) — Port (unit_r38_wasser_lauf) Wert fuer Wert gleich.

### Lauf 3 — Endstand (Stand 53a33885 + Testerweiterung), keine Vorgaenger-Messung wiederholt
Bau: `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`, 718/718, keine Compiler-Warnung.

**Elza ueber den echten Weg** (neu; Spielerwahl RE15_PSELECT_AUTO=1/_SWITCH=1, Sprung nur nach 10B1 (-21370,3000) Blick 3072,
`U0.6,A0.2,U3` ab Bild 30, Tuer -> 1261, Ankunft vor Fleck P5 (x -20800..-19500, z -9550..-7350)):
```
F39  room=10B1 fuss=7 bild=8  lage=(-21370,0,3308)     boden=0x1d zweig=trocken edt=0x1d
F39  room=1261 fuss=7 bild=8  lage=(-20250,-1800,-7375) boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-20133,-1865,-6834) edt=0x17
F63  room=1261 fuss=7 bild=8  lage=(-20250,-1800,-8725) boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-20133,-1865,-8184) edt=0x17
F77..F137 room=1261 (z -9775 .. -14275, hinter dem Fleck)  boden=0x1d zweig=trocken edt=0x1d
[foot] SE foot=7 type=23 vag=7 (2x im Fleck), type=29 vag=1 (Beton); FX_LOG id=6 sub=5 in 26 Bildern (2 x 13), frame=0 bei F=39
```
**Bild im Schritt-Takt:** der Port zeichnet c0 = 0 im SELBEN Spielbild wie der Schritt (Leon 1260: Schritt F98, FX_LOG
`frame=0 ... F=98`, `frame=1 F=99`; Elza: F39/F39). Original ebenso: Spieler-Takt `jal 0x80031c44` @0x8001ce0c (Schritt +
Spawn) vor dem ESP-Takt `jal 0x80019e20` @0x8001ce2c im selben Hauptschleifen-Durchlauf (B7); im GDB-Paketlauf folgt auf
jedes SPAWN unmittelbar BAU c0 = 0 (`gdb_1260_paket.txt`).
Hinweis zum Vergleichsstreifen `vergleich_original_leon_elza.png`: er belegt Form/Farbe/Verlauf, NICHT den Zeitpunkt. Der
Original-Mitschnitt (Lauf 2, `ds1260_c`, Mitschnitt AN 37,0 s bis AUS 43,6 s) lief mit aktiven Haltepunkten @0x80045630 /
@0x80045710 / @0x80045798 ("Stopps: 12"), die Emulation hielt also genau in den Schrittbildern an; die Zuordnung der
Video-Bilder zum Spieltakt ist fuer diese Bilder nicht belegt. Im Streifen erscheint der Fleck in der Original-Zeile zwei
Spalten spaeter als in der Port-Zeile — das ist darum KEIN Zeitbefund. Der Zeitpunkt ist ueber Bildwort/Bildnummer (B9:
Original +0x95 = 23 mit Bildwort[22], Port akt = 23 / bild = 22), die c0-Folge des Paketlaufs und den FX_LOG (c0 = 0 im
Schrittbild) belegt.

## Tests

Riegel (registriert in `tests/unit/probes/r38_wasser.cmake`):
- `unit_r38_wasser_pfuetze` — 1260 P0: Effekt 6/5, Flags 3, Skala 0x0D00, Gier, Part 4/7, Anker = Original-GDB +-3, Tonsatz 0x17.
- `unit_r38_wasser_zeitfolge` — gezeichnet 21 / aktuell 22 -> kein Schritt; 22/23 -> Schritt (alt genau umgekehrt).
- `unit_r38_wasser_trocken` — Beton: kein Effekt, 0x1d.
- `unit_r38_wasser_stempel` — 2090: Effekt 6/3, Gier 0x800, Part 6, Anker relativ = Original, 0x1a, +0x88 geloescht.
- `unit_r38_wasser_lauf` — a0 = 1 -> 0x18 (Anker Bild 0/12 = Original), Plc-Lauf 0x17.
- `unit_r38_wasser_aotbreite` — 1260-Zone Breite 38950 stempelt -900 (vorher nie).
- `unit_r38_wasser_zensus` — FLR 0x97 an den sechs Flecken von 1260.
- `integration_r38_wasser` — echte exe: Leon 1260 (boden, Original-Anker, id=6 sub=5 >= 13 Bilder, VAG 7 / VAG 1),
  Elza 1261 (boden, id=6 sub=5, VAG 7), Leon 2090 (stempel, 0x06032000 Part 6, 0x1a, VAG 3), Leon Tuerweg 10B0 -> 1260
  (Fleck P5) und (Lauf 3) Elza Tuerweg 10B1 -> 1261 (Fleck P5: boden + id=6 sub=5 + VAG 7, Beton in 1261 trocken 0x1d).

Gegen v0.8.25 (2077bb8e) rot: re15_wasser_r38_* existiert nicht (Unit-Riegel bauen nicht), kein RE15_WASSER_LOG,
kein id=6 im FX_LOG, im Becken `[foot] ... type=29 vag=1`.

## OFFEN

- Objekt-Index 9 (Spieler steht auf einem Objekt, `ori s3,zero,0x9` @0x80045790) ist umgesetzt (Latch des
  Steh-Zustands re15_climb_standing_tick = @0x80031d24), aber nicht am Spiel gemessen (kein Wasser-Raum mit Kiste im Weg).
- NPC-Schritte (Typ >= 0x40: Index - 8, @0x8004573c-5c) und Koerperfall `FUN_80045630(2,0)` (Tod @0x80036804,
  Hunde @0x80111da4, Affen @0x8011c4f0) sind nicht Teil dieses Punkts (Nutzer: Leon/Elza) und nicht portiert.
- Beobachtung ohne Urteil: im Original-Mitschnitt 2090 bewegte sich Leon bei Blick 0 langsam nach +z (Port: +x mit
  Gehgeschwindigkeit); nicht untersucht (Bewegung/Kollision im Becken, nicht Teil dieses Punkts).
- Beobachtung ohne Urteil (Lauf 3, Fortbewegung, nicht Teil dieses Punkts): erster Geh-Schritt nach dem Losgehen bei
  (-8404,-7246) Blick 0 — Original (`gdb_1260_gehen.txt`) bei x -7710 (+0x95 = 8, schon im Fleck P0, nass), Port bei x -7804
  (akt = 8, 4 Einheiten vor dem Fleck, trocken). Gleiche Bildnummer, 94 Einheiten mehr Weg im Original; Port-Gehtempo
  WALK_SPEED_PER_FRAME 75 (player_common.c:33). Ob das am Anlauf des Original-Executors oder an der Stellung per RAM (Lage
  3 s vor dem Losgehen geschrieben) liegt, ist nicht untersucht. Die Schritte an der Wand (x -7068, Bild 22/37) sind Wert fuer Wert gleich.
- vab_common.c (re15_footstep_vag) traegt noch den alten Kommentar "-3 / ->9 / -8 ... wired when those exist";
  -3 und 9 sind jetzt in wasser_r38.c verdrahtet (Kommentar nicht angefasst, fremde Datei).

## Fuer den Nutzer

- Keine neuen Assets (Effekt 6 liegt in den Raum-ESP-Banken der Original-RDTs), keine Sprachdateien.
- Sichtbar: in den gruenlichen Pfuetzen der Kanal-Gaenge (1260, 2000, 2050, 20B0 + Elza-Varianten) spritzt bei jedem
  Schritt am Fuss Wasser auf, dazu der Wasser-Schritt-Ton; im Alligator-Becken 2090 und im Wasser von 20A0 spritzt es
  am Schienbein mit dem Wat-Ton. Laufen hat (wie im Original) einen eigenen Lauf-Ton.
- Alle Schritt-Toene kommen jetzt einen Takt (1/30 s) spaeter, im Gleichtakt mit dem Original.
