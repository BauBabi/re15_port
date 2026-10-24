# Runde 41 — Spur I "zombiefresser" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_zombiefresser`, Zweig `r41/zombiefresser`, Basis master-HEAD 4c667c97.
Start 2026-10-24 09:44 (Uhr). Vorgaengerlauf (wf_a8b604c5-d10) starb beim Start ohne Arbeit (UEBERGABE 02:50).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der Zombies durchgehend falsch. Sind sind am Boden und fressen,
> anstatt aufzustehen wenn man ihnen näher kommt. gleiches trifft zu bei den Zombies in STAGE3 in ROOM 3000, 3010
> etc. es ist durchgehend falsch.

## 0. Status

- [x] Zensus Fresser-Spawns STAGE2/3 (§1.1)
- [x] Messung vorher (Port, §1.2) und Original (DuckStation, §2.1b)
- [x] RE-Belege (RE1.5 STAGE1/2/3 + RE2 EMZ0, §2)
- [x] Umsetzung (§3)
- [x] Messung nachher (§4)
- [x] Tests (§5)

## 1. Messung vorher

### 1.1 Zensus der Spawns (RDT, opcode-exakt)
Werkzeug `re15_port/tools/r41_zombiefresser/fresser_zensus.py` (Walker = `tools/aot_sce_census.py`).
Sce_em_set (0x44, 20 B: [op, slot, typ, Deskriptor=+0x9, ...]) der Zombie-Familie 0x10..0x1f, alle RDTs:
Deskriptor-Verteilung 0x00:92 0x02:91 0x06:8 0x0d:142 0x0e:2 0x81:12 0x83:2 **0x84:28 0x85:2** 0x86:4 0x87:20 0x88:33 0xa1:2 0xae:1.

| Raum | Skript @Offset | Slot | Typ | Deskriptor | Pos (x,y,z) |
|---|---|---|---|---|---|
| 2000 | sub00 @0x017cc / @0x017e0 (Else-Zweig von `If Ck(4,0xF4)` @0x01794) | 2, 3 | 0x16 | 0x84 | (-24200,0,-10700), (-22550,0,-10100) |
| 2001 | sub00 @0x0182e / @0x01842 | 0, 1 | 0x16 | 0x84 | wie 2000 |
| 3000/3001 | main00 @0x014ca..0x01542 | 0..6 | 0x10 x3, 0x11 x4 | 0x84 (alle 7) | y=0 |
| 3010/3011 | main00 @0x02122/36/4a | 0, 1, 2 | 0x10, 0x10, 0x11 | 0x84 | |
| 3010/3011 | main00 @0x021ac / @0x021c0 (Else von `If Cmp(0,0x0b,..)` @0x0215e) | 3, 4 | 0x11 | 0x85, 0x84 | |
| 3010/3011 | main00 @0x021d6 | 5 | 0x11 | 0x84 | |

Die echten FRESSER (sel 6) stehen nur in STAGE1: 0x86 ROOM1140 x4, 0x06 ROOM10E0/10E1 x4. In STAGE2/3 gibt es
KEINEN Fresser-Deskriptor — dort stehen ausschliesslich 0x84/0x85.

### 1.2 Port am gebauten Stand (master 4c667c97, RE2-KI = Default)
Lauf `RE15_TITLE_SHOT=t.bmp RE15_DEBUG_JUMP=2000@5 RE15_STATE_LOG` (Messkopie re15_pc_r41i_mess.exe):
- Leon am Sprungpunkt (4350,-1800,-20500): beide 0x84-Zombies (Slots 3/4) 400 Bilder lang `st=1 ss1=8` =
  RE2-EXEC[8] **FRESSEN**, Clip 18/19/20 (die RE2-Fress-Schleife), d=30168/28816.
- Leon per `RE15_PLAYER_POS=-20000,-10500,1024,0`: Slot 4 (d=3187) steht ueber EXEC[8] P3 (Clip 0x15) auf,
  Slot 3 (d=4802) frisst weiter. Port-Bild: Zombie FRISST am Boden, bis Leon < 4000 kommt.
- Bild `scratchpad/I41/m3/fd_000120.png` (Kamera 0 vom Sprungpunkt).

## 2. RE-Belege

### 2.1 RE1.5 — Deskriptor 0x84/0x85 ist ein LIEGENDER mit Naehe-Wecker, KEIN Fresser
Zombie-Wurzel je Stage (Registrierung `sw handler,0x80072bac+typ*4`, eigener Scan): STAGE1 0x80100424,
STAGE2 0x801002b8, STAGE3 0x80100510 (Typen 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f). Der Code ist eine
verschobene Kopie (STAGE2 = STAGE1 - 0x16c, STAGE3 = STAGE1 + 0xec; Tabellen gegengelesen).

INIT-Posen-Decoder STAGE2 (roh, `re15_disasm.py --bin STAGE2.BIN`):
```
80100b38: lbu v1,9(v0) / 80100b40: andi v0,v1,0x80 / 80100b44: beq v0,zero,0x80100cc4   ; Tor 0x80
80100b4c..b74: sel {4,7,9}  -> ori v0,zero,0xc ; sb v0,148(v1)      ; +0x94 = 0x0c
80100b90..bb8: sel {5,8,0xa}-> ori v0,zero,0xe ; sb v0,148(v1)      ; +0x94 = 0x0e
80100c0c: sb zero,149(v0) / 80100c1c: sb zero,143(v0)                ; +0x95 = 0, +0x8f = 0
80100c34: jal 0x8001f314 (a0=+0x170, a1=+0x174, a2=0, a3=0x200)     ; Pose = Bank1 Clip 0x0c/0x0e Bild 0
80100c54..c7c: sel {4,7,9}  -> ori v0,zero,0x12 ; sb v0,148(v1)     ; +0x94 = 0x12 (FINAL, ohne f314)
80100c98..cc0: sel {5,8,0xa}-> ori v0,zero,0x13 ; sb v0,148(v1)     ; +0x94 = 0x13
80100cc4..cf8: sel 6 -> +0x94 = 0x27, sw 0x20c01,4  ; 80100d44: sb zero,9(v0)   ; FRESSER: grid = 0!
```
STAGE3 identisch verschoben: Tor @0x80100d90-9c, FINAL 0x12 @0x80100ed0-d4. Folge: 0x84/0x85 behalten
grid 0x84/0x85 -> Grid-Dispatcher @0x80117910[4]/[5] (STAGE2; STAGE1 @0x8011f80c, STAGE3 @0x8011d95c);
der echte Fresser 0x86 erreicht [6] NIE (grid = 0 @0x80100d44, Schleife Modus-0-Sub 0xc).

Grid-[4]-Dispatcher STAGE2 0x80101710: DECIDE = @0x80117acc[+0x5] = 0x80103814, ANIMATE =
@0x80117ad0[+0x5] = 0x801038ec (STAGE1 0x80103980/0x80103a58, STAGE3 0x80103a6c/0x80103b44).
Wecker STAGE2 0x80103814 (byte-gleich zu STAGE1 0x80103980):
```
80103824: lw v0,464(v1)        ; +0x1d0 = Distanz (2D, SquareRoot0 im Wurzel-Prolog)
8010382c: sltiu v0,v0,0xfa0    ; < 4000
80103838: lbu v0,6(v1) / 80103840: bne v0,zero ; nur bei +0x6 == 0
8010384c: sb v0,6(v1)  (v0=1)  ; +0x6 = 1
80103858/60: andi v0,v0,0xf ; sh v0,156(v1)    ; +0x9c = rand & 0xf  (Wartezeit 0..15 Takte)
80103864..74: andi 3 ; bne ; jal 0x800453d0 a0=5 ; 1/4: Raum-SE 5 (Stoehnen)
```
Liege-Phasenmaschine STAGE2 0x801038ec:
```
P0 80103940-60: +0x93 |= 1 ; +0x1b8 = 1                       ; liegt, unverwundbar (Ein-Treffer-Riegel)
P1 80103964-98: lhu +0x9c ; sh (v-1) ; bne v,zero -> warten ; sonst +0x6 = 2, +0x8f = 0xf
P2 8010399c-cc: jal 0x8001f314 (+0x170,+0x174, a2=0, a3=0x100) ; +0x6 += fertig  ; spielt +0x94 = 0x12
P3 801039d0-0c: sb zero,9 (grid=0) ; sw 0x201,4 ; +0x93 &= 0xfe ; +0x1b8 = 0 ; -> Kampf (Modus 0, Sub 2)
```
Clip-Inhalt (CDEMD0.EMS, Typ 0x16, Bank 1 = dir[3]/dir[4], `tools/r41_zombiefresser/clip_hoehe.py`,
Hoehe = kleinstes Welt-Y der Knochen): Clip 0x0c 1 Bild top -234 (flach liegend); **Clip 0x12 98 Bilder,
f0 -457 -> f97 -2361 = AUFSTEHEN AUS DEM LIEGEN**; Clip 0x13 120 Bilder -590 -> -2384 (andere Seite);
Fress-Schleifen 0x27/0x28 (69/82 Bilder, -680..-1197 = kniend); 0x29 Aufstehen aus dem Knien.
**Im Original liegt ein 0x84-Zombie also flach am Boden (Pose 0x0c), frisst NICHT, und steht auf, sobald
Leon naeher als 4000 kommt (nach 0..15 Takten): Aufsteh-Clip 0x12 (98 Bilder), danach Angriff.**

### 2.1b ORIGINAL dynamisch (DuckStation, MZD-Disc = Auslieferungs-EXE, kein PATCHED-Save)
Ruhezustand `stage_saves/mzd_demo/mzd_stage2_room.sav` (ROOM2000, Leon (4350,-1800,-20500)), Entity-Array
@0x800acc2c Stride 0x1f4, Slots 2/3 (Typ 0x16): `+0x4..7 = 1/0/0/0`, `+0x9 = 0x84`, **`+0x94 = 0x12`**,
`+0x95 = 0`, `+0x8f = 0`, **`+0x93 = 0x01`** (Phase-0-Riegel = unschiessbar), `+0x1b8 = 0x01`, d = 30168/28816
(Port am Sprungpunkt: dieselben d-Werte). Also Liege-Phasenmaschine P0, NICHT Fress-Schleife.

Filmstreifen (Werkzeug `tools/r41_zombiefresser/orig_weck_film.py`: RAM-Patch Slot 3 nach (1350,-20500) =
d 3000 < 4000, DuckStation `-statefile`, je Lauf EINE Probe ueber SaveStateOnExit nach `--lead` s, Ende nur
per eigener PID). Belege `scratchpad/I41/orig2/resume_lead*.sav`:

| lead | Slot 3 (`+0x4/5/6/7`, grid, Clip, Bild, +0x8f, +0x93, +0x9c, d) | Slot 2 (fern) |
|---|---|---|
| 1.0 s | 1/0/**2**/0 g=84 Clip **0x12** f7 8f=8 93=01 9c=**-1** d=3197 | 1/0/0/0 g=84 0x12 f0 93=01 |
| 2.0 s | 1/0/2/0 g=84 Clip 0x12 f32 93=01 | unveraendert |
| 3.0 s | 1/0/2/0 g=84 Clip 0x12 f65 93=01 | unveraendert |
| 4.5 s | **1/2/1/0 g=00** Clip 0x02 f9 **93=00** | unveraendert |
| 6.0 s | 1/2/1/0 g=00 Clip 0x02 d=3974 | unveraendert |
| 9.0 s | 1/2/1/0 g=00 d=6036 (Leon auf Ebene -1800, Zombie y=0 — Weg fuehrt weg) | unveraendert |

=> Original: liegt (P0, unschiessbar) -> d < 4000 -> Wartezeit (+0x9c laeuft auf -1, Post-Dekrement
@0x80103974) -> P2 Aufstehen Clip 0x12 (1 Bild/Takt, 98 Bilder, Riegel bleibt) -> P3 grid 0 + 0x201 +
Riegel frei -> Kampf-Gang (Bank0 Clip 2). Der ferne Zombie (d 30168) bleibt die ganze Zeit liegen.

### 2.2 Der Port-Defekt
- RE2-KI (Default): `re2z_init` (enemy_ai_re2_zombie.c, Zweig `sel == 6 || sel == 4 || sel == 5`) schickt
  0x84/0x85 in EXEC[8] FRESSEN (RE2-Fress-Clips 18/19/20) — begruendet mit "[5]/[6] = Fresser". Das Etikett
  war falsch: [4]/[5] haben den Liege-Wecker, und [6] wird vom Fresser nie erreicht (grid = 0 @0x80100d44).
  Spawn-Pose (`re15_re2z_spawn_pose_seed`) ebenfalls Fress-Clip 18 nur fuer sel 6; sel 4/5 ohne 0x80-Seite.
- RE1.5-KI: `re15_enemy_ai_live_feeding` (enemy_ai_common.c, case 4/5/6) spielt beim Wecken Clip 0x29
  (Aufstehen aus dem KNIEN, STAGE1-Sub-0xd) statt +0x94 = 0x12/0x13 (Aufstehen aus dem LIEGEN).

### 2.3 RE2 — warum die RE2-KI hier nur den Executor, nicht den Zeitpunkt liefert
RE2-INIT (EMZ0.BIN, `re2_disasm.py`), Deskriptor `+0x10E & 0x3F`: 2 -> Clip 22 + 0x701 + Seite B
(@0x801009C4-F0), 4 -> Clip 23 + 0x701 (@0x801009F4-A10), 5/7 -> wie 2/4 PLUS Latch 0x4002/0x4004 und HP -1
(@0x80100A14-60 / @0x80100A64-A4 = Leichen), 6 -> 0x101 Gang, 8 -> Clip 18 + 0x801 FRESSEN (@0x80100AC0-DC).
EXEC[7] (0x80103780): P0 Clip 23/22 (@0x801037CC-E4), +0x1D3 |= 0x80 (@0x80103804-14), haelt bei
`+0x10E & 0x4000` (@0x8010381C-28), P2 Boden-Aufstehclip 8/9 mit Zufallsbild + Stoehnen 12/10/11
(@0x80103838-D4), P3 ausspielen (@0x801038D8-FC), P4 0x101 + `+0x1D3 &= 0x7f` (@0x80103900-2C).
RE2 HAT KEINEN Naehe-Wecker fuer den Latch (einziger Clear @0x80104F0C, Skript-/EXE-seitig; RE2-Liegende ohne
Latch stehen sofort auf). RE1.5 hat das System VOLLSTAENDIG (Wecker + Wartezeit + Aufsteh-Clip) -> RE1.5 bleibt
massgeblich fuer den ZEITPUNKT; die RE2-KI liefert den Executor (Pose/Aufstehen/Ton). Einordnung nach
Memory reai-v2-beta-zu-retail: "fertig und anders" -> RE1.5 halten.

## 3. Umsetzung

Neue Dateien: `re15_port/engine/src/zombiefresser_r41.c`, `re15_port/include/re15_zombiefresser_r41.h`
(Belege im Kopf), Werkzeuge `re15_port/tools/r41_zombiefresser/` (fresser_zensus.py, scd_dump.py,
clip_hoehe.py, orig_weck_film.py). Messschiene `RE15_ZOMBIEFRESSER_LOG=<datei>` (env, kein Verhalten).

Haken (je 1-3 Zeilen, Kommentar "Runde 41 Spur I"):
| Datei | Stelle | Haken |
|---|---|---|
| enemy_ai_re2_zombie.c | Include | `re15_zombiefresser_r41.h` |
| enemy_ai_re2_zombie.c | re2z_init Fresser-Zweig | `sel == 6 || ((sel == 4 || sel == 5) && !re15_r41_liegewecker_deskriptor(beh))` |
| enemy_ai_re2_zombie.c | re2z_init Liege-Zweig | `|| re15_r41_liegewecker_deskriptor(beh)` + `re15_r41_liegewecker_init(e)` nach `0x701` |
| enemy_ai_re2_zombie.c | re2z_exec_lying Port-Wecker 0xBB8 | `!re15_r41_liegewecker_re2_tick(e, pl) &&` davor |
| enemy_ai_re2_zombie.c | re15_re2z_hit_filter_apply | `passive_lyer ... || re15_r41_liegewecker_liegt(e)` |
| enemy_ai_common.c | Grid-Zweig `case 5: case 6:` (case 4 faellt durch) | `re15_r41_liegewecker_re15_decide(e)` -> `re15_enemy_ai_lying_phase(e)` |
| player_common.c | Pin-Freigabe der Liege-Phasenmaschine | `nib >= 7` -> `nib >= 4` |

Konstanten (alle mit Adresse): Wecker-Distanz 0xFA0 @0x8010382c (STAGE2) / @0x80103998 (STAGE1) /
@0x80103a84 (STAGE3); Wartezeit `rand & 0xf` @0x80103858-60; Post-Dekrement @0x80103974; Stoehnen 1/4 SE 5
@0x80103864-74 (nur RE1.5-KI); Latch-Clear 0x4000 @0x80104F0C (RE2); Sentinel -1 = 0xffff (Wert von +0x9c nach
der Wartezeit im Original, @0x80103974).

PORT-WAHL (RE2-KI): kein RE1.5-SE 5 beim Wecken; das RE2-Stoehnen aus EXEC[7] P2 (@0x80103884-D4) faellt beim
Aufstehen (Vorbild: der Skript-Wecker D15.2 im selben Executor, ebenfalls ohne SE 5). Seite: sel 4 -> Clip 23/8,
sel 5 -> Clip 22/9 (bestehende Abbildung re15_re2z_spawn_pose_seed, RE1.5-Spaltung 0x12/0x13).

## 4. Messung nachher (gebauter Stand, Messkopie re15_pc_r41i_mess.exe)

Nach dem Merge von master (Spur A + D integriert, ce430484) unveraendert (integration_r41_zombiefresser_*).

ROOM2000, RE2-KI, Leon Oststeg (-20300,-1800,-11600) (`I_belege/nachher_2000_zf.log`):
```
F6  slot=3/4 INIT grid=84 Wort=1/7/0 f10e=4002      (EXEC[7] liegend, nicht mehr 1/8 Fressen)
F7  slot=3 WECKT d=3982 warte=10 ; slot=4 WECKT d=2684 warte=5
F12 slot=4 STEHT AUF ; F17 slot=3 STEHT AUF          (Latch frei nach warte+1 Takten)
F14 slot 4 ss2=3 Clip 8 (RE2-Aufstehen aus dem Liegen) ... F94 beide st=1 ss1=1 grid=00 (Gang)
F114.. slot 4 ss1=10 Clip 22 = Hochziehen an der Kante (Runde 38 U) ... F294 oben auf dem Steg (-20913,-9408)
```
ROOM3000, RE2-KI, Leon (-8750,0,-14800) (`I_belege/nachher_3000_zf.log`): alle 7 Zombies INIT 1/7/0 f10e=4002;
Slot 1 (d=1144) F7 WECKT warte=1, F8 STEHT AUF, Clip 8 bis ~F100, F114 st=1 ss1=3 = GRIFF (Leon hp 100->80->60);
die 6 fernen (d 5992..14838) liegen 600 Bilder lang (Clip 23 Bild 0, Latch, unschiessbar).
Bilder: `I_belege/vorher_nachher_3000.png` (oben vorher: kniend/fressend; unten nachher: flach liegend),
`I_belege/vorher_nachher_2000.png` (Kamera 6/13/8, vorher Fress-Silhouetten am Bildrand, nachher flach im Kanal),
`I_belege/nachher_2000_cut13_aufstehen.png` (Bild 10..130: Aufstehen und Gang).

## 5. Tests

Neu (registriert NUR in `re15_port/tests/unit/probes/r41_zombiefresser.cmake`):
- `unit_r41_zombiefresser` (`tests/unit/test_r41_zombiefresser.c`, echte RDT/SCD/SCA + game_step, beide KI):
  A Zensus 2000 (2), 2001 (2), 3000 (7), 3010 (6) liegen (RE2 1/7 Latch 4002 Clip 23/22; RE1.5 grid 84/85
  Sub 0 Clip 0x12/0x13), keiner frisst. B Ruhe 300 Bilder bei d >= 4000: liegend + unschiessbar (+0x93 Bit 0).
  C je Raum Spieler 500..1500 vor einem Zombie: Wecken im ersten Takt, Wartezeit <= 16, Aufsteh-Clip ganz
  (RE2 Clip 8, 80 Bilder; RE1.5 Clip 0x12, 98 Bilder), aufrecht, Angriff. Gemessen (Auszug):
  `[C 3000 RE2] d0=1144 weckt F0 frei F1 Aufstehen F2 Clip 8 Laenge 80 steht F73 angriff F76`,
  `[C 3000 RE1.5] ... Clip 18 Laenge 98 steht F99 angriff F103`, `[C 3010 RE2] ... steht F82 angriff F128`,
  `[C 2000 RE1.5] d0=1366 ... steht F102 angriff F323` (zieht sich hoch), `[C 2000-Steg RE2] d0=2684 ...
  steht F74 angriff F306`. D Grenze am GEMESSENEN +0x1d0 (SquareRoot0 trifft nicht jeden Wert: Versatz 4008 ->
  4007 weckt 300 Bilder nicht, 4007 -> 3998 weckt). E 48 Seeds: 15 verschiedene Wartezeiten in 1..16.
  F ROOM1140: die vier 0x86-Fresser bleiben Fresser (RE2 Sub 8, RE1.5 grid 0 / Sub 0xc).
- `integration_r41_zombiefresser_{tuer2000,elza2001,leon3000,leon3010,re15_3000}` (echte exe, Messkopie
  `re15_pc_r41i_<teil>_*`, `tests/integration/test_r41_zombiefresser.cmake`): tuer2000/elza2001 = ECHTER Tuerweg
  2070/2071 -> 2000/2001 (Wegplan, Elza mit RE15_TITLE_SHOT_CHAR=1 + Spawn-Bit 3:0x45); leon3000/leon3010/re15_3000
  = Spielstand + CONTINUE (Kartenwerkzeug `probe_r41_zombiefresser_karte`). Gemessen: tuer2000 F61 WECKT d=3949
  warte=10 -> F71 STEHT AUF, F94/F99 Slot 3, Griff F371 (hp 80); elza2001 identisch als Elza; leon3000 F1/F2,
  Griff F77; leon3010 F1/F2, Griff F129; re15_3000 F1 WECKT(RE1.5), Griff F109.

Nachgezogene alte Pins (ihre Praemisse "Grid [4]/[5]/[6] = Fresser" ist durch §2.1/§2.1b widerlegt):
- `unit_p3_restposten_pin` [D]: jetzt 0x84/0x85 -> EXEC[7] mit Latch, 0x86 -> EXEC[8].
- `unit_anim_rate` PIN 4: das Paar Clip 0x29 / +0x8f 7 kommt vom ECHTEN Fresser (grid 0 / Sub 0xc ->
  decide[0xc] @0x801048bc -> 0xd01), nicht mehr vom Grid-5-Zweig.
- `unit_room1140_combat` (13b): Grid 0x84 spielt in P2 das vorhandene +0x94 = 0x12 aus (+0x8f 0xf), kein Clip-Wechsel.
- `integration_r38_wasserzombie` Teil e: Endbild 406 -> 560 (das Partner-Domino-Zwischenbild liegt mit dem
  Liege-Aufstehen bei Bild 461/462; gemessen `F461 ... zwischenbild st=1/9/0 mo=22 af=59`, `F462 abbruch af=60`).

Zusatz: A/C auch fuer die Elza-Raeume 3001 (7 Liegende) und 3011 (6), gleiche Saetze und Bedingungen
(3001 main00 @0x014b6 `Ck(4,9,0)`, 3011 main00 @0x02112 `Ck(4,0x0A,0)`); `[C 3001 RE2] steht F73 angriff F76`,
`[C 3011 RE2] steht F82 angriff F128`. Jeder Teil startet mit frischen Flags (re15_game_state_init).

### 5.1 Gezielte Laeufe (KEINE volle Suite; Baum = Spurzweig + master-Merge ce430484)

Diff gegen die Merge-Basis: enemy_ai_common.c, enemy_ai_re2_zombie.c, player_common.c (+ eigene Dateien und die vier
nachgezogenen Tests). Weil enemy_ai_common.c/player_common.c in fast jedem Unit-Test stecken, lief die GANZE
Unit-Ebene (ohne Integrationstests) mit:

```
ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_|r[23][0-9]b?_)" -j 6 --timeout 600
100% tests passed, 0 tests failed out of 937
```
Integrationstests der beruehrten Zombie-Module/Raeume:
```
ctest --test-dir re15_port/build -R "^integration_(r41_zombiefresser|r38_wasserzombie|r37_zombieaktiv|r39_elzazombies|r36_gegnerzahl|r36_schuss|r37_re2zombie|r36_zombieki|r39_gegner|r37_beretta|r34_granaten)" -j 4 --timeout 600
100% tests passed, 0 tests failed out of 23
```
Nach der letzten Aenderung am eigenen Riegel (Elza-Raeume 3001/3011, frische Flags):
```
ctest --test-dir re15_port/build -R "^unit_(r41_zombiefresser|anim_rate|room1140_combat|p3_restposten_pin|r38_wasserzombie|re2_lyer_1140|r16_liegende_unschiessbar|re2z_spawn_seed)$" -j 4 --timeout 600
100% tests passed, 0 tests failed out of 8
```
Erster Lauf der Integrationsgruppe (vor dem Nachziehen von r38 Teil e): `99% tests passed, 1 tests failed out of 162`
(integration_r38_wasserzombie e, Zeitfenster — §5), danach einzeln `100% tests passed, 0 tests failed out of 1`.

## 6. OFFEN

1. **RE2-KI, Kanalraum 2000/2001: Spieler direkt UEBER dem Zombie** (Oststeg, Abstand < 2000): der aufgestandene
   Zombie schnappt nach oben statt zur Kante zu laufen. Das ist RE2-Retail-Verhalten, kein Rest des Fixes: Block A
   der RE2-DECISION[2] @0x8010205C-94 (`a1300 == 0 && dist < 0x7d0 && +0x106 != Spieler-Etage && kein Griff ->
   0x0E01` = EXEC[14] Schnappen @0x80104D74). Gemessen (unit_r41_zombiefresser C 2000 RE2, Spieler (-21300,-10100)):
   Ende Wort 1/2/1 @(-22143,0,-10029), +0x90 = 0, keine Kante vorn. Steht der Spieler weiter weg (Oststeg
   (-20300,-11600)), laeuft er zur Kante, zieht sich hoch (Runde 38 U) und greift (F306). Die RE1.5-KI hat keinen
   Etagen-Schnapper (Engage-Decide prueft dieselbe Etage nur fuer den Griff) und zieht sich auch von direkt unten hoch.
2. **LADE-Weg in ROOM2000/2001 (Nebenbefund, nicht Teil des Auftrags):** wird ein Spielstand IN 2000/2001 geladen
   (RE15_CONTINUE_TEST/Karte), laeuft sub00 nicht -> die beiden Zombies (und die 2000-Szene sub02) fehlen. Ursache
   im Boot-Pfad platform/pc/main.c (Pre-Intro-Erkennung nach dem ersten main00-Takt: laeuft ein Ereignis-Thread,
   wird sub00 zurueckgehalten); 2000/2001 main00 endet mit `Evt_exec 0xff,0x18,0x07/0x08` (@0x0177c). Das Original
   startet sub00 in der SCD-Raum-Init immer (FUN_8003ef6c @0x8003efd8, Zitat main.c). Tuerweg und Debug-Sprung sind
   nicht betroffen (gemessen). Naechster Weg: die Pre-Intro-Erkennung auf ROOM1170/den Erzaehler (sub11) eingrenzen.
3. **RE1.5-KI, Pose vor dem Wecken:** der Port zeigt Clip 0x12 Bild 0 (Hoehe -457), das Original die INIT-Pose
   Clip 0x0c Bild 0 (Hoehe -234; f314 @0x80100c34 VOR dem FINAL +0x94 = 0x12 @0x80100c7c), weil der Port Pose und
   +0x94 nicht trennt. Beide liegend; vorbestehend auch fuer 0x87/0x88. Nur RE1.5-KI.
4. **RE2-KI, Aufsteh-Dauer:** RE2-Clip 8/9 (80 Bilder, Startbild rand&0xf @0x80103838-4C) statt RE1.5-Clip
   0x12 (98 Bilder) — RE2-Praesentation der RE2-KI, gewollt (Executor = RE2, Zeitpunkt = RE1.5).

## 7. Fuer den Nutzer

- Keine neuen Assets, keine Sprachdateien, keine Nachrichten-Ids, keine Bank-9-Bits.
- Bedienhinweis: In ROOM2000/2001 liegen die beiden Zombies flach im Abwasserkanal, in ROOM3000/3010 auf dem
  Boden (wie im Original) — sie fressen NICHT mehr. Kommt Leon/Elza naeher als 4000 Einheiten, stehen sie nach
  hoechstens einer halben Sekunde auf und greifen an; im Kanal ziehen sie sich dazu an der Kante hoch. Solange sie
  liegen, sind sie (wie im Original) nicht zu treffen. Die echten Fresser im Speisesaal (ROOM1140) und in 10E0
  bleiben Fresser.
- Mit RE2-KI schnappt ein Kanal-Zombie, der direkt unter dem Spieler steht, nach oben (RE2-Verhalten, §6.1);
  einen Schritt vom Kanalrand weg laeuft er zur Kante und klettert hoch.
- Keine Entscheidungsfragen.

## 8. Nachbesserung 1 (nach Abnahme 0, Start 2026-10-24 ~12:50)

Grundlage: `analysis/befunde_runde41/I_abnahme_0.md` (M1, V1, H2) + AUFTRAG.md 12:43 (I-E1: V1 erledigt, Haken angenommen;
I-E2: Laden IN 2000/2001 ohne Zombies -> Spur I). Vorliegende Messungen/Belege der Abnahme werden NICHT wiederholt.

| Mangel | Status |
|---|---|
| M1 RE1.5-KI: Liegepose vor dem Wecken Clip 0x12 f0 statt INIT-Pose 0x0c/0x0e f0 (auch 0x87/0x88) | in Arbeit |
| V1 Vertrag | erledigt durch Orchestrator-Entscheid I-E1 |
| I-E2 Laden in 2000/2001 -> keine Zombies | in Arbeit |
