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
- [x] Nachbesserung 1 (§8, nach Abnahme 0): M1 Liegepose (+ Zusatzbefund M1b P2-Takt) und I-E2 LADEN 2000/2001 behoben,
      V1 durch Orchestrator-Entscheid I-E1 erledigt; Bau + gezielte Laeufe §8.8

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
| M1 RE1.5-KI: Liegepose vor dem Wecken Clip 0x12 f0 statt INIT-Pose 0x0c/0x0e f0 (auch 0x87/0x88) | behoben (8.1, 8.3, 8.4) + Zusatzbefund M1b (P2-Takt) |
| V1 Vertrag | erledigt durch Orchestrator-Entscheid I-E1 |
| I-E2 Laden in 2000/2001 -> keine Zombies | behoben (8.2, 8.3, 8.4) |

### 8.1 M1 — RE1.5-KI: Liegepose vor dem Wecken (gilt fuer 0x84/0x85 UND 0x87/0x88)

**Ursache (Port):** der Port hat fuer +0x94 und die angezeigte Pose EIN Feld (`motion`) und posiert jedes Bild aus
(`motion`, `anim_frame`). Im Original ist die Pose aber der Zustand des Skeletts, den nur ein `f314`-Aufruf aendert:
INIT posiert per `f314` Clip 0x0c/0x0e Bild 0 und schreibt DANACH `+0x94 = 0x12/0x13` OHNE `f314`; P0/P1 rufen kein
`f314` — das Skelett bleibt bis zum ersten `f314` in P2 auf Clip 0x0c/0x0e Bild 0. Der Port zeigte Clip 0x12/0x13 Bild 0.

**Messung vorher:** Abnahme S5 (`abn/s5_re15_3000`, state.log `st=1 ss1=0 ss2=0 g=84 mo=18 af=0` = Keyframe 0x190) —
nicht wiederholt.

**Belege** (STAGE2-Zitate selbst disassembliert von der Abnahme, I_abnahme_0.md §2 / §6.2 M1; hier NEU dazu):
```
STAGE2 (re15_disasm.py --bin STAGE2.BIN dis 0x80100b20):
80100b24  jal 0x8001f314 (a1 = lw 364 = +0x16c, a3 = 0x100)       ; Vor-Pose aus Bank 0 (vor dem Decoder)
80100b40  andi v0,v1,0x80 / 80100b44 beq v0,zero,0x80100cc4       ; Tor Bit 0x80
80100b4c..b74  sel {4,7,9}  -> ori v0,zero,0xc / sb v0,148(v1)     ; +0x94 = 0x0c
80100b90..bb8  sel {5,8,a}  -> ori v0,zero,0xe / sb v0,148(v1)     ; +0x94 = 0x0e
80100bd4..bfc  sel {1,3}    -> +0x94 = 0x0c, +0x5 = 5               ; (kein FINAL -> nicht betroffen)
80100c0c  sb zero,149 / 80100c1c sb zero,143                      ; +0x95 = 0, +0x8f = 0
80100c28  addu a2,zero,zero / c2c lw a0,368 / c30 lw a1,372 / c34 jal 0x8001f314 / c38 ori a3,zero,0x200
80100c54..c7c  sel {4,7,9}  -> sb 0x12,148 ; 80100c98..cc0 sel {5,8,a} -> sb 0x13,148   (FINAL, ohne f314)
STAGE1 dieselbe Folge: 80100d78 sb zero,149 / 80100d88 sb zero,143 / 80100d94 addu a2,zero,zero /
       80100d98 lw a0,368 / 80100d9c lw a1,372 / 80100da0 jal 0x8001f314 / 80100da4 ori a3,zero,0x200; FINAL ab 80100dbc
STAGE3: 80100e64 sb zero,149 / 80100e74 sb zero,143 / 80100e8c jal 0x8001f314 / 80100e90 ori a3,zero,0x200; FINAL ab 80100ea8
```
- FUN_8001f3bc (RE_15_Quellcode_V2/FUN_8001f3bc.c): `+0x8f == 0` -> Keyframe DIREKT (Wurzel + Winkel kopiert, RotMatrix);
  danach `+0x95 += 1`, `if (param_4 <= +0x95) +0x95 = 0` -> bei einem 1-Bild-Clip steht +0x95 wieder auf 0 (passt zum
  Original-RAM in Ruhe `+0x95 = 0`, Dossier 2.1b). P1 setzt `+0x8f = 0xf` (@0x80103998), P2 ruft `f314(+0x170,+0x174,0,0x100)`
  (@0x801039a8): Zweig `+0x8f != 0` mischt Alt-Pose (Gewicht `0x100 * +0x8f` = 0xf00/0x1000) mit dem neuen Bild und
  zaehlt +0x8f herunter (FUN_80020510) — der Aufsteh-Clip blendet also aus Clip 0x0c/0x0e Bild 0 heraus.
- Bankdaten (CDEMD0.EMS, Bank 1 = dir[3]/dir[4], `tools/r41_zombiefresser/clip_hoehe.py lade(typ,1)`, eigener Lauf), fuer
  ALLE Zombie-Typen 0x10/0x11/0x12/0x16/0x18 wortgleich:
  `Clip 0x0c n=1 f0 = 0x0000012d (Hoehe -234)`, `Clip 0x0e n=1 f0 = 0x00000149 (-437)`,
  `Clip 0x12 n=98 f0 = 0x00000190 (-457)`, `Clip 0x13 n=120 f0 = 0x000001c2 (-590)`.
  => Soll vor dem ersten P2-`f314`: Keyframe 0x12d (sel 4/7/9 = 0x84/0x87/0x89) bzw. 0x149 (sel 5/8/a = 0x85/0x88/0x8a).
- Port-Blende: skeleton_common.c `re15_skel_compute_pose` mischt `prev_angles` (= zuletzt GEZEICHNETE Pose) mit
  Gewicht `anim_frac * anim_blend_rate` — dieselbe Formel wie FUN_8001f3bc; zeigt der Port bis P2 Clip 0x0c/0x0e Bild 0,
  blendet P2 (anim_frac 0xf, Rate 0x100) automatisch aus dieser Pose heraus.

**Zusatzbefund M1b (gemessen am Zwischenstand mit Halt, `scratchpad/I41nb1/runs/re15_2000/pose.log`, RE1.5-KI, LADEN 2000,
Kamera 7):** F0..F10 `mo=18 af=0 frac=0 kf=301` (Halt = Clip 0x0c Bild 0, richtig); F11 (P1 -> P2) `kf=400 frac=15` mit
unveraenderten Knochen (Rate 0 -> Vorgabe 0x200, Gewicht geklemmt 0x1000 = Halt-Pose); **F12 `af=1 frac=14 kf=401`** —
der Port zeigt im ersten P2-Takt schon Bild 1 mit +0x8f 14, das Original Bild 0 mit +0x8f 0xf. Ursache: P1 kehrt im
Original OHNE anim_set zurueck, P2 ist ein eigener Fall der +0x6-Weiche (STAGE2, eigener Lauf
`re15_disasm.py --bin STAGE2.BIN dis 0x801038ec 76`):
```
801038fc lbu v1,6(a1) / 80103904 beq v1,1 -> 80103964 (P1) / 80103914 beq v1,0 -> 80103940 (P0)
80103928 beq v1,2 -> 8010399c (P2) / 80103930 beq v1,3 -> 801039d0 (P3)
P1 80103964 lhu v1,156 / 8010396c addiu v0,v1,-1 / 80103970 bne v1,zero,0x80103a10 / 80103974 sh v0,156
   80103984 sb v0(=2),6 / 80103994 j 0x80103a10 / 80103998 sb v0(=0xf),143      ; zurueck, KEIN f314
P2 8010399c addu a2,zero,zero / 801039a0 lw a0,368 / 801039a4 lw a1,372 / 801039a8 jal 0x8001f314 / 801039ac ori a3,0x100
   801039bc lbu v1,6 / 801039c4 addu v1,v1,v0 / 801039cc sb v1,6                 ; +0x6 += fertig
```
=> Takt T: P1 setzt +0x6 = 2 / +0x8f = 0xf, Skelett unveraendert. T+1..T+98: 98 anim_set (Bild 0..97, Gewicht
0xf00, 0xe00, ...), Wrap im 98. -> +0x6 = 3 in T+98, P3 in T+99. Der Port schob ab T+1 vor (Ordnung Vorschub -> KI ->
Zeichnen, player_common.c) -> P2 einen Takt zu kurz (Abnahme S5: ss2=2 F83..F179 = 97 Takte, ss2=3 F180). Die
Skript-Weckung 9/10 ist NICHT betroffen: STAGE1-DECIDE 0x801039fc setzt +0x6 = 2 (@0x80103a1c) / +0x8f = 0xf
(@0x80103a30), und ANIMATE (P2) laeuft im selben Takt (eigener Lauf `re15_disasm.py --bin STAGE1.BIN dis 0x801039fc 20`).
Abhilfe im Port-Vokabular: `motion_init_delay = 1` ("Saat-Takt posiert Bild 0", wie re15_npc_seed_clip /
game_step_common.c) genau beim Uebergang P1 -> P2, und der Halt gilt bis zu diesem ersten P2-anim_set.

### 8.2 I-E2 — LADEN in ROOM2000/2001: keine Zombies

**Messung vorher** (Abnahme S8, nicht wiederholt; debug.log selbst gelesen): `abn/s8_laden2000/debug.log`
```
[scd] thread start slot=0 first_op=0x3B      ; main00
[save] CONTINUE: resumed in room 2000 (hp=100)
[scd] thread start slot=10 first_op=0x3A     ; sub07 (Sce_espr_on), gestartet von main00 Evt_exec @0x0177c
                                             ; KEIN slot=1 -> sub00 (Spawns @0x017cc/@0x017e0) laeuft nie
```
Tuerweg (Abnahme S1, `abn/s1_tuer2000/debug.log`): `slot=0 first_op=0x3B`, `slot=1 first_op=0x06` (sub00 @0x01794 `If`),
`slot=10 first_op=0x3A`.

**Zensus** (eigenes Skript `scratchpad/I41nb1/main00_evt.py`, Walker aot_sce_census.py): Evt_exec im main00 haben nur
1021 (@0x01e1e, in If), 1031 (@0x0200e/02072/020a2/020b2, in If), 1170 (@0x012bc sub03, @0x012ec sub11 = Erzaehler),
**2000 (@0x0177c `04 ff 18 07`, UNBEDINGT)**, **2001 (@0x0177c `04 ff 18 08`, UNBEDINGT)**, 4010, 4031, 5011 (je in If).

**Original** (ghidra1_V2.txt FUN_8003ef6c = SCD-Raum-Init, einziger Aufrufer FUN_800396fc @0x80039a00):
```
8003efa0 lw v0,0x40(v0) / 8003efac sw v0,DAT_800b3f70 / 8003efb0 jal FUN_8003ee3c (a0 = 0 @8003ef94, a1 = 0 @8003efb4) ; Slot 0 = main00
8003efc0 ori a0,zero,0x1 / 8003efc4 lw v0,0x44(v0) / 8003efd0 sw v0,DAT_800b3f70
8003efd4 jal FUN_8003ee3c / 8003efd8 _clear a1                                                          ; Slot 1 = sub00
8003f018 jal FUN_8003f0a0                                                                               ; Init-Lauf
```
ohne jede Bedingung zwischen Slot 0 und Slot 1. FUN_800396fc hat genau zwei Aufrufer: LADEN/Session-Start
`8001d5ac: jal 0x800396fc` (eigener Lauf `re15_disasm.py dis 0x8001d594`, Ghidra hat den Bereich nur als Daten) und Tuer
`8001d988 jal FUN_800396fc` (ghidra1_V2.txt:95878). => Im Original laeuft sub00 beim LADEN genau wie beim Tuerweg.

**Port-Ursache:** platform/pc/main.c, Boot-/LADE-Pfad nach dem main00-Init-Tick: "laeuft irgendein Ereignis-Thread ->
Pre-Intro -> sub00 zurueckhalten" (seit aa346af1, fuer den 1170-Erzaehler). Der Tuerweg (main.c, Raumwechsel-Block
"PRE-INTRO-Kette UEBER Raeume") ist seit dem Irons-Befund bereits auf ROOM1170 eingegrenzt ("the sole room whose main00
spawns the narrator sub11 into an event slot"); der Boot-Pfad nicht. In 2000/2001 startet main00 UNBEDINGT sub07/sub08
-> Fehlerkennung -> sub00 nie (die Rueckfall-Neubetretung feuert erst beim Ende des Threads/Bildgrenze).

### 8.3 Umsetzung (Nachbesserung 1)

| Datei | Aenderung |
|---|---|
| `re15_port/engine/src/zombiefresser_r41.c` / `include/re15_zombiefresser_r41.h` | NEU `re15_r41_liege_skelett_clip(a)`: Clip, den das Skelett eines RE1.5-Liegenden traegt, solange er von +0x94 abweicht — 0x0c (sel 4/7/9) bzw. 0x0e (sel 5/8/0xa) bei Zustand 0 (Sce_em_set ruft die Wurzel sofort @0x8004259c, INIT posiert im Spawnbild) und bei Zustand 1 / Sub 0 / +0x6 < 2 sowie im P1->P2-Takt; sonst -1. Nur Zombie-Wurzel-Typen 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f, nicht unter dem RE2-Hirn. NEU `re15_r41_liege_p2_saat(e, ph_vor)`: P1 -> P2 setzt `motion_init_delay = 1` (Port-Vokabular "Saat-Takt posiert Bild 0") — das erste P2-anim_set kommt wie im Original einen Takt spaeter. |
| `re15_port/engine/src/anim_select_common.c` (Gegnerzweig von `re15_actor_anim_select`) | 1 Haken (3 Zeilen): `clip_override = re15_r41_liege_skelett_clip(a)`, wenn >= 0. Gilt fuer beide Renderer (platform/pc und platform/psx rufen dieselbe Auswahl). |
| `re15_port/engine/src/enemy_ai_common.c` (Grid [4]/[5]/[6], Haken aus I-E1) | von 2 auf 3 Zeilen: +0x6 vor dem ANIMATE merken, danach `re15_r41_liege_p2_saat`. |
| `re15_port/platform/pc/main.c` (Boot-/LADE-Pfad nach dem main00-Init-Takt) | 2 Zeilen: die Pre-Intro-Erkennung gilt nur noch fuer ROOM1170 (wie der Tuerweg); sonst startet sub00 wie FUN_8003ef6c. |
| `re15_port/tests/unit/probe_r41_zombiefresser_karte.c` | Option `cam=<n>` (Kamera des Spielstands; `re15_savedata_capture` speichert `g_scd.cam_id`, CONTINUE stellt sie her). |

Konstanten: 0x0C @0x80100b70-74, 0x0E @0x80100bb4-b8 (STAGE2; STAGE1/3 verschoben, f314 @0x80100da0/@0x80100e8c); Soll-
Keyframes 0x12d/0x149 = Bankdaten (Bild 0 von Clip 0x0c/0x0e, kein Code-Literal); Raum 0x1170 = vorhandene Tuerweg-Regel
(main.c Raumwechsel-Block), kein neuer Wert. +0x94 (`motion`) bleibt RAM-treu 0x12/0x13 (Original-RAM Dossier 2.1b).
Gegenprobe der Rate: der P1->P2-Takt zeigt die Halt-Pose (Renderer-Halt), das erste P2-Bild mischt mit `anim_frac 0xf *
anim_blend_rate 0x100` = 0xf00 (P2 setzt die Rate im selben Takt vor dem Zeichnen) = FUN_8001f3bc mit a3 = 0x100.

### 8.4 Messung nachher (gebauter Stand, Messkopien `re15_pc_r41nb1_<lauf>.exe`, Laufordner `scratchpad/I41nb1/runs/`)

**I-E2, LADEN in 2000 (Leon)** — Karte `2000 -20600 -10600 2048 1 leon cam=7`, `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`,
Standard-KI (`I_belege/nb1_laden_logs.txt`, `nb1_laden2000_kontakt.png`, `nb1_laden2000_zoom.png`):
```
[scd] thread start slot=0 first_op=0x3B / [save] CONTINUE: resumed in room 2000 / [scd] thread start slot=10 first_op=0x3A
[scd] thread start slot=1 first_op=0x06                         <- sub00 laeuft (vorher fehlte diese Zeile, Abnahme S8)
F0 slot=3/4 INIT grid=84 Wort=1/7/0 f10e=4002 ; F1 WECKT d=3593/2011 warte=10/5 ; F6/F11 STEHT AUF
state.log: F0 beide ss1=7 mo=23 (liegen), Clip 8 ab F7/F12, Gang F76/F84, Hochziehen F107.., Griff F319 (gr=1), HP 100->80->60
```
Bild: F0/F10 beide flach im Kanal, F30 richten sie sich auf, F90 stehen sie, F120 Kante, F280..F420 Griff — wie Tuerweg S1.
**I-E2, LADEN in 2001 (Elza)** — Karte `2001 ... elza 3:0x45 cam=7`: `Spieler-Familie PL04`, sub00 startet (slot=1),
F0 INIT Slots 1/2, F1 WECKT, F6/F11 STEHT AUF, Griff F319 — identisch (`nb1_laden2001_elza_kontakt.png`).

**M1, RE1.5-KI, LADEN in 2000** (`RE15_AI_FLAVOR=re15`, `RE15_POSE_DUMP`, `I_belege/nb1_re15_2000_pose.txt`,
`nb1_re15_2000_halt_blende.png`):
```
F0..F10  slot3/4 ss1=0 mo=18 af=0 frac=0  kf=301   (Halt: Clip 0x0c Bild 0; +0x94 = 18 = 0x12)
F11      P2-SAAT                frac=15 kf=301   (P1 -> P2: Skelett unveraendert, wie Original ohne f314)
F12      mo=18 af=0 frac=15 kf=400  b9(276,-75,-953) vs Halt b9(324,-83,-981)  (erstes P2-anim_set, Gewicht 0xf00)
F13      af=1 frac=14 kf=401 ; ... ; ss2=2 F11..F108 = 98 Takte ; ss2=3 F109 ; F110 ss1=2 g=00 (P3, 0x201)
```
Vorher (Abnahme S5): `mo=18 af=0` = Keyframe 0x190 bis zum Wecken; Zwischenstand ohne Saat-Takt: F12 schon `af=1 frac=14`,
P2 97 Takte. Bild: F0/F11 beide flach mit abgespreizten Armen (Clip 0x0c), ab F12 Blende in das Aufstehen 0x12.
**M1, RE1.5-KI, LADEN in 3000** (Kamera 3, `nb1_re15_3000_kontakt.png`): 5 Liegende sichtbar, alle `kf=301`; der geweckte
Slot 1: F1 WECKT, F2 P2-SAAT, F3 erstes P2-Bild `kf=400 af=0 frac=15`, die 4 fernen 200 Bilder `kf=301`.

### 8.5 Tests (Nachbesserung 1)

- `unit_r41_zombiefresser` NEU Teil G (Renderer-Weg `re15_actor_anim_select` + `re15_compute_actor_kf`, echte RDT/SCD):
  G1 2000 (RE1.5, MIXED, RE2) und 3010 (RE1.5, RE2): Spawnbild und Ruhe — RE1.5/MIXED: Renderer-Clip 0x0c/0x0e, Keyframe
  0x12d/0x149 (= Bankbild 0), +0x94 18/19, Bild 0; RE2: kein Halt, Clip 23/22. G2 Wecken 2000 (0x84), 3010 (0x85), 3000:
  Halt bis zum Saat-Takt, erstes P2-Bild genau einen Takt danach mit Bild 0 / +0x8f 15, Blende aus der Halt-Pose (Abstand
  335 zur Halt-Pose / 4612 zum ungemischten Bild, nach 16 Bildern 0), +0x6 = 2 fuer 98 bzw. 120 Takte (= Cliplaenge).
  G3 1070 (0x87/0x88): Halt 0x12d/0x149, nach Member_set(12,0x89/0x8a) sofort +0x94 Bild 0 +0x8f 15 ohne Saat-Takt.
  Gemessen (Auszug): `[G2 2000 RE1.5] slot 4 grid 84: 5 Bilder Halt (falsch 0), Saat-Takt F4, erstes P2-Bild F5 Clip 18
  Bild 0 +0x8f 15, ... +0x6 = 2 fuer 98 Takte (Cliplaenge 98), steht F103`, `[G2 3010-0x85 RE1.5] ... Clip 19 ... 120 Takte`.
- `integration_r41_zombiefresser_laden2000` / `_laden2001` (NEU, I-E2): LADE-Weg Leon/Elza mit Kamera 7: sub00 startet nach
  dem CONTINUE, erstes Bild 2 Liegende grid 84, 2 INIT (RE2), Wecken d < 4000, Wartezeit <= 16, Aufstehen, Angriff
  (gemessen F319), nie Fressen; laden2001 zusaetzlich `Spieler-Familie PL04`.
- `integration_r41_zombiefresser_re15_2000` (NEU, M1) und `_re15_3000` (Kamera 3 + Pose): RE15_POSE_DUMP — jeder Liegende
  zeigt bis zu seinem ersten P2-anim_set Keyframe 301 (0x12d) bzw. 329 (0x149); das erste P2-Bild ist Keyframe 400/450,
  Bild 0, +0x8f 15, genau einen Takt nach dem `P2-SAAT` des zf.log. Gemessen: `re15_2000: Pose Slot 3/4: 12 Bilder Keyframe
  301, erstes P2-Bild F12 Keyframe 400`, `re15_3000: Pose Slot 1: 3 Bilder ..., F3 Keyframe 400; Slots 3/5/6/7 je 360
  Bilder Halt 301`.

### 8.6 Vertrag (Nachbesserung 1)

- Neue Haken ausserhalb der Spur-I-Zuteilung (VERTRAG §1.3), je klein und markiert "Runde 41 Spur I":
  - `engine/src/anim_select_common.c` (laut §1.3 Spur J/K): 3 Zeilen im Gegnerzweig von `re15_actor_anim_select` — die EINE
    Auswahlstelle, die beide Renderer (platform/pc und platform/psx) fuer Gegner benutzen; die angezeigte Pose laesst sich
    sonst nicht von +0x94 trennen, ohne +0x94 (RAM-treu) zu verbiegen. Bei der Integration mit J/K gegenlesen.
  - `platform/pc/main.c` (laut §1.3 D/F/G): 2 Zeilen im Boot-/LADE-Pfad (I-E2 ist vom Orchestrator an Spur I gegeben;
    die Ursache liegt genau dort). Bei der Integration mit D (Lautstaerke) und F/G (Szenen-Haken) gegenlesen.
  - `engine/src/enemy_ai_common.c`: der in I-E1 angenommene Grid-[4..6]-Haken waechst von 2 auf 3 Zeilen.
- Keine Bank-9-Bits, keine Nachrichten-Ids, kein shared_assets/pri/synchro/release/android.

### 8.7 OFFEN (Stand Nachbesserung 1)

1. (unveraendert, = Abnahme H1) RE2-KI im Kanal 2000/2001: Spieler direkt ueber dem Zombie (< 2000) -> RE2-Etagen-Schnapper
   DECISION[2] Block A @0x8010205C-94 / EXEC[14] @0x80104D74 = RE2-Retail-Verhalten.
2. ~~LADE-Weg 2000/2001~~ -> behoben (8.2/8.4, I-E2). Wirkung auch auf die anderen Raeume mit main00-Evt_exec beim Laden
   (1021, 1031, 4010, 4031, 5011 — Zensus 8.2): dort startet sub00 jetzt ebenfalls wie im Original (FUN_8003ef6c); 1170
   behaelt die Erzaehler-Halte (Port-Vorspann). Gegenproben: Integrationsgruppe 8.8.
3. ~~RE1.5-KI Pose vor dem Wecken~~ -> behoben (M1).
4. (unveraendert) RE2-KI: Aufstehen RE2-Clip 8/9 (80 Bilder) statt RE1.5-Clip 0x12 (98) — RE2-Praesentation der RE2-KI.
5. (Abnahme H3, nicht Teil dieser Nachbesserung, nicht selbst disassembliert) nach dem Loesen aus dem RE2-Griff fallen
   die Kanal-Zombies (`ss1=5`) und rutschen bis ~2 500 Einheiten (EXEC[9] Schub +0x144 = 400 @0x80103F60-64 laut
   Port-Kommentar). Eigener Auftrag, falls gewuenscht.
6. Messhinweis (kein Verhalten): `RE15_ANIM_TRACE` druckt waehrend des Halts `fc_render` 1 (Clip 0x0c) neben `fc_clock`
   98 (+0x94 = 0x12); der Bildzaehler steht dort fest auf 0 (player_common.c Liege-Pin), die Uhr wirkt erst ab P2. Der
   Blutanker-Zwilling `schuss_r36.c zeichen_bank` kennt den Halt nicht — Liegende sind in P0..P2 unschiessbar
   (+0x93 Bit 0 @0x80103940-4c), es entsteht dort kein Treffer.

### 8.8 Gezielte Laeufe (Nachbesserung 1; KEINE volle Suite)

Fortsetzung nach dem Wochenlimit (Kontowechsel 2026-10-24 ~14:0x): `bash re15_port/tools/local_build.sh build` ->
`ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` — exe `re15_pc.exe` 13:30:09 = Codestand 3083b2da (letzte
Quellaenderung 13:29:11, danach nur Doku). Beruehrte Code-Dateien (`git diff master...HEAD`): anim_select_common.c
(Renderer-Auswahl der Gegner), enemy_ai_common.c (Grid [4..6]), enemy_ai_re2_zombie.c, player_common.c (Vorschub-Pin),
zombiefresser_r41.c, platform/pc/main.c (Boot-/LADE-Pfad). Der Haken in anim_select_common.c greift nur fuer
0x10/0x11/0x12/0x16/0x18/0x1c-0x1f mit Grid-Bit 0x80 und sel 4/5/7/8/9/0xa unter einem RE1.5-Hirn (RE1.5 oder MIXED);
unter der Standard-KI gehoeren 0x10/0x11/0x12/0x13/0x16/0x18 dem RE2-Hirn (`re15_re2z_owns_type`) -> Rueckgabe -1. Liegende
mit diesen Deskriptoren (Zensus `fresser_zensus.py`): 1020/1021, 1070/1071, 10E0/10E1, 1100/1101, 1110/1111, 1140 (Slot 0,
0x88), 11F0/11F1, 1200/1201, 2000/2001, 3000/3001, 3010/3011 — nur Typen 0x10/0x11/0x16.

1. **Unit-/Probe-Gruppe** (eigener Lauf, `-j 4`, Log `I_belege/nb1_ctest_unit_gezielt.log`): Abnahme-Regex `rx_u` + alle
   registrierten Tests, deren Quelle `re15_actor_anim_select` / `re15_compute_actor_kf` / `re15_actors_anim_advance` ruft,
   + Lade-/Speicher-Units:
   ```
   ctest -R "^(unit_(damage|enemy_ai|enemy_shootable|enemy_body_push|enemy_attack_reach|hit_reaction|getup_advance|anim_.*|zombie_.*|room1140_.*|re2_.*|re2z_.*|re15_re2z_.*|re15_zombie_death_anim|writher_.*|1140_straight_shot|ai_flavor_mixed|r16_.*|r19_.*|r36_zombieki_.*|r36_gegnerzahl_.*|r36_schuss.*|r36_kleidung_.*|r37_re2zombie_.*|r37_zombieaktiv_.*|r37_bissarm_.*|r38_wasserzombie|r38_griff_zombie.*|r38_npckante_.*|r38_dining1141_.*|r39_elzazombies_.*|r39_treffer_.*|r39_gegner_.*|r41_zombiefresser|r41_wasser20b0_.*|p3_restposten_pin|aot_sce_census|r35_inhalt_zombies|lane_b1_t3b_getupsfx|1090_.*|knockdown_.*|npc_scd_pose|plc_back_yaw_1090|irons_neck_1150|dog_grab_anchor|r30_hinweis_.*|r35_affen_.*|r35_granate|r35_raeume_.*|r36_szenenleon_.*|r37_roy_.*|r37_beretta|r37_elzaems_.*|r38_spiderman|r38_spinnen|r38_szenenschutz.*|r38_werkstatt11b0_.*|r38_heliport1171.*|r39_elza_.*|r39_heliport1171_.*|r39_hund1121_.*|r39_zustand_.*|save_.*|savedata_.*|continue_.*|text_freeze)|probe_(10d0_.*|re2z_.*|1090_.*|elevator_1080|p2_birkin_g5|r16_tuer1040_.*)|r19_marke1_10d0_c1|r30_hund_hitbox_stauchung|r31_trefferboxen.*)$" -j 4 --timeout 600
   100% tests passed, 0 tests failed out of 323        (Total Test time (real) = 24.86 sec, exit=0)
   ```
   `unit_r41_zombiefresser -V` gelesen (keine Schein-Pruefung): `[G1 2000 RE1.5 Spawnbild] slot 3 typ 16 grid 84 Wort 0/0/0
   +0x94 18 -> Renderer Clip 12 Keyframe 0x12d (Halt 12)`, `[G1 2000 MIXED Ruhe] ... Clip 12 Keyframe 0x12d`, `[G1 2000 RE2
   Ruhe] ... Wort 1/7/1 +0x94 23 -> Renderer Clip 23 Keyframe 0x2dc (Halt -1)`, `[G1 3010 RE1.5 Ruhe] slot 4 typ 11 grid 85
   ... +0x94 19 -> Renderer Clip 14 Keyframe 0x149 (Halt 14)`, `[G2 2000 RE1.5] slot 4 grid 84: 5 Bilder Halt (falsch 0),
   Saat-Takt F4, erstes P2-Bild F5 Clip 18 Bild 0 +0x8f 15 ... +0x6 = 2 fuer 98 Takte`, G3 1070 (0x89/0x8a ohne Saat-Takt),
   `== OK ==`. Vorgaenger 13:24-13:29 zusaetzlich breite Unit-Ebene (unit_/probe_/prop_ ohne Android-/Release-Skript-Tests):
   `100% tests passed, 0 tests failed out of 922` (Regex dort nicht protokolliert, deshalb oben der eigene Lauf).
2. **Integration: LADE-/Boot-Pfad + Zombie-Module + eigene Teile** (Lauf des Vorgaengers 13:30:46-14:00:13 auf der exe
   13:30:09, `-j 3`, Log `I_belege/nb1_ctest_integration.log`):
   ```
   ctest -R "^integration_(elza_vollstart|r36_elza_.*|r36_gegnerzahl|r36_schuss|r37_re2zombie|r37_roy_.*|r37_zombieaktiv|r38_elzaintro_.*|r38_elzatoene_.*|r38_heliport1171_.*|r38_itemorte|r38_montage|r38_wasser|r38_wasserzombie|r39_elza_.*|r39_elzazombies_.*|r39_gegner_.*|r39_heliport1171_.*|r39_hund1121|r39_johnruf1041_.*|r41_wasser20b0.*|r41_zombiefresser_.*|boot_bg_pin|save_counter_pin)$" -j 3 --timeout 600
   100% tests passed, 0 tests failed out of 61         (Total Test time (real) = 1766.80 sec, exit=0)
   ```
   Darin alle 8 eigenen Teile (tuer2000 40.3 s, elza2001 40.2 s, leon3000 18.1 s, leon3010 18.1 s, re15_3000 18.4 s,
   laden2000 18.2 s, laden2001 18.1 s, re15_2000 26.2 s), die Abnahme-Gruppe `rx_i` (r38_wasserzombie, r37_zombieaktiv,
   r37_re2zombie, r39_elzazombies_*, r36_gegnerzahl, r36_schuss, r36_elza_schuss, r39_gegner_*, r41_wasser20b0*) und die
   LADE-Wege der Raeume mit main00-Evt_exec (Zensus 8.2): 1021 (r36_elza_stoehnen, r37_roy_szene1021, r39_johnruf1041_a/b),
   1031 (r38_elzatoene_*, r38_itemorte), 1170/1171 = Erzaehler-Halte unveraendert (r37_roy_leon1170,
   r38/r39_heliport1171_laden, r38_elzaintro_*, boot_bg_pin, elza_vollstart).
3. **Integration-Ergaenzung** (eigener Lauf, `-j 3`, Log `I_belege/nb1_ctest_integration2.log`): RE1.5-KI mit dem
   0x88-Liegenden in 1140 (r37_beretta, Teil leon_re15_w5), Vorspann-/Boot-Weg 1240 -> 1170 und CONTINUE in 1170
   (r35_entladen_d/e/f/h), Liege-Raeume 1100/1101/1141 (r37_elzaems_leon1100/elza1101/elza1141), room_probe_sweep:
   ```
   ctest -R "^integration_(r37_beretta|room_probe_sweep|r35_entladen_[defh]|r37_elzaems_(leon1100|elza1101|elza1141))$" -j 3 --timeout 600
   100% tests passed, 0 tests failed out of 9          (Total Test time (real) = 164.81 sec, exit=0)
   ```
   Einzeln: r35_entladen_d 93.2 s, _e 39.2 s, _f 61.5 s, _h 23.7 s, r37_elzaems_elza1101 18.8 s, _elza1141 11.3 s,
   _leon1100 19.4 s, room_probe_sweep 0.16 s (laedt die Raeume, Ausgabe gelesen), r37_beretta 164.8 s.

### 8.9 Fuer den Nutzer (Nachbesserung 1)

- Spielstand IN ROOM2000 (Leon) bzw. ROOM2001 (Elza) laden: die beiden Kanal-Zombies sind jetzt da, liegen flach und stehen
  auf, wenn man naeher kommt — genau wie beim Betreten durch die Tuer (vorher fehlten sie nach dem Laden ganz).
- Mit RE1.5-KI (Optionsmenue) liegen die Zombies in 2000/2001/3000/3010 (und die Liegenden in STAGE1, z.B. 1070/1140) vor
  dem Aufstehen in der Original-Liegepose (flach, Arme abgespreizt) und gehen fliessend in das Aufstehen ueber; mit der
  Standard-KI (RE2) aendert sich nichts.
- Keine neuen Assets, keine Sprachdateien, keine Entscheidungsfragen.
