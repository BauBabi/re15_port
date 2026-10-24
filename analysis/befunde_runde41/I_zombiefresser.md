# Runde 41 — Spur I "zombiefresser" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_zombiefresser`, Zweig `r41/zombiefresser`, Basis master-HEAD 4c667c97.
Start 2026-10-24 09:44 (Uhr). Vorgaengerlauf (wf_a8b604c5-d10) starb beim Start ohne Arbeit (UEBERGABE 02:50).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der Zombies durchgehend falsch. Sind sind am Boden und fressen,
> anstatt aufzustehen wenn man ihnen näher kommt. gleiches trifft zu bei den Zombies in STAGE3 in ROOM 3000, 3010
> etc. es ist durchgehend falsch.

## 0. Status

- [ ] Zensus Fresser-Spawns STAGE2/3
- [ ] Messung vorher (Port)
- [ ] RE-Belege (RE1.5 + RE2)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

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

### 2.2 Der Port-Defekt
- RE2-KI (Default): `re2z_init` (enemy_ai_re2_zombie.c, Zweig `sel == 6 || sel == 4 || sel == 5`) schickt
  0x84/0x85 in EXEC[8] FRESSEN (RE2-Fress-Clips 18/19/20) — begruendet mit "[5]/[6] = Fresser". Das Etikett
  war falsch: [4]/[5] haben den Liege-Wecker, und [6] wird vom Fresser nie erreicht (grid = 0 @0x80100d44).
  Spawn-Pose (`re15_re2z_spawn_pose_seed`) ebenfalls Fress-Clip 18 nur fuer sel 6; sel 4/5 ohne 0x80-Seite.
- RE1.5-KI: `re15_enemy_ai_live_feeding` (enemy_ai_common.c, case 4/5/6) spielt beim Wecken Clip 0x29
  (Aufstehen aus dem KNIEN, STAGE1-Sub-0xd) statt +0x94 = 0x12/0x13 (Aufstehen aus dem LIEGEN).

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
