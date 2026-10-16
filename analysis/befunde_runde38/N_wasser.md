# Runde 38 — Spur N "wasser" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_wasser`, Zweig `r38/wasser`, Basis d70c234e (2026-10-16).

## Nutzer-Punkt (woertlich, AUFTRAG.md Z. 29)

> In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza im flachen Wasser bewegen.

## Stand

- [ ] Messung vorher (Port: was passiert heute in 1260 beim Gehen im Wasser? Bild + Ton + FX-Log)
- [ ] RE-Beleg Original (Mechanismus: Bodentyp -> Effekt + Schritt-Ton)
- [ ] Raumliste (Zensus)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

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

## Umsetzung

(folgt)

## Messung nachher

Messkopie `re15_pc_r38w_mess.exe` (Stand ace4282a + Umbenennung der Schiene auf `RE15_WASSER_LOG`), gleiche Laeufe wie vorher.
`RE15_WASSER_LOG` = neue Schritt-Schiene (je Schritt eine Zeile); `RE15_FX_LOG` = VORHANDENE Zeichen-Spur des
PC-Renderers (main.c:288, je gezeichnetem Partikel `id= sub= ... -> sx= sy= S=`).

Leon ROOM1260 (Sprung, Band 1, (-8600,-7900) rot 0, `W0.5,U1.5,W1`):
```
F82  schritt fuss=7 lage=(-7879,-1800,-7246) boden=0x1d zweig=trocken plaetze=0 edt=0x1d
F97  schritt fuss=4 lage=(-7068,-1800,-7246) boden=0x97 zweig=boden code=0x06050d00 part=4 anker=(-7502,-1858,-7381) quelle=zeichner plaetze=1 edt=0x17 fx=1
F112 schritt fuss=7 lage=(-7068,-1800,-7246) boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-7543,-1878,-7100) quelle=zeichner plaetze=1 edt=0x17 fx=1
```
debug.log: `[foot] SE foot=4 type=23 vag=7` (Wasserschritt, wie vorher). state.log `fx=1` F97-F109 (13 Anim-Saetze
x 1 Takt, Satz 13 = Ende), vorher 0. Zeichen-Spur: `id=6 sub=5 ... wpos=(-7502,-1858,-7381) ... fl=03 -> sx=141 sy=167
S=8..32`. Differenzbild Bild 102 vorher/nachher: 28 Pixel in (136..147, 162..171) direkt am Fuss, gruenlich-weiss
(ABR 3 = B+F/4, Zeile[0x16] = 0x60) — sonst bitgleich.

Elza ROOM1261 (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, debug.log
`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, Sprung 1261 gleiche Lage):
```
F83  boden=0x1d zweig=trocken edt=0x1d
F97  boden=0x97 zweig=boden code=0x06050d00 part=4 anker=(-7556,-1832,-7338) quelle=zeichner plaetze=1 edt=0x17
F113 boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-7596,-1841,-7135) quelle=zeichner plaetze=1 edt=0x17
```
(eigene Anker = Elzas Skelett), Zeichen-Spur 26x `id=6 sub=5` (2 Spritzer x 13 Bilder).

Leon ROOM2090 (Alligator-Becken, Sprung-Spawn (-8200,0,-25550), Band 0 in der sce-8-Zone, Spiegel -1620):
```
F87  boden=0x1d stempel=-1620 zweig=stempel code=0x06032000 part=6 anker=(-8632,-837,-26362) quelle=zeichner plaetze=1 edt=0x1a
F102 boden=0x1d stempel=-1620 zweig=stempel code=0x06032000 part=3 anker=(-8626,-832,-26695) quelle=zeichner plaetze=1 edt=0x1a
```
debug.log `[foot] SE foot=7 type=26 vag=3` — vorher spielte der Port hier den Betonschritt 0x1d (VAG 1).
Effekt-6-Zeilen von 2090/2000/2050/11A0 = byte-gleich zu 1260 (gleiche Sub 3/5).

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
