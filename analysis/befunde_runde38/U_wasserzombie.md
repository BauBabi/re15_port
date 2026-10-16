# Runde 38 Spur U — wasserzombie (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_wasserzombie`, Zweig `r38/wasserzombie`, Basis `cb782bda`.

## Nutzer-Punkt (woertlich, AUFTRAG.md)

> Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand
> hochziehen können, wie im Original Resident Evil 1.5

## Stand

- 2026-10-16 18:16: Dossier angelegt (Vorgaenger, Sitzungslimit nach 3 min).
- 2026-10-16 (Fortsetzung): Zensus + RE-Belege (Abschnitte 0 und 2).

## 0. Zensus — welche Stage-2-Zombies stehen im Wasser?

Werkzeug: Sce_em_set-Walker (0x44, 20 B; Laengen/Regionen aus `re15_port/tools/aot_sce_census.py`) ueber alle
`shared_assets/PSX/STAGE2/ROOM*.RDT` (ROOM20C0-20F1 sind 4-Byte-Stummel). Zombie-Typen (0x10-0x1F) kommen in Stage 2
NUR in ROOM2000/2001 und ROOM2020/2021 vor, alle Typ 0x16:

| Raum | Record | Slot | Deskriptor pc[3] | Band pc[4] | Position (x,y,z) |
|---|---|---|---|---|---|
| 2000/2001 | sub00 @0x17cc / @0x182e | 2 / 0 | 0x84 | 0 | (-24200, 0, -10700) |
| 2000/2001 | sub00 @0x17e0 / @0x1842 | 3 / 1 | 0x84 | 0 | (-22550, 0, -10100) |
| 2020/2021 | main00 @0x08f6 | 0 | 0x02 | **1** | (-6700, -1800, 2000) |
| 2020/2021 | main00 @0x090a | 1 | 0x02 | **1** | (-11250, -1800, -3100) |
| 2020/2021 | main00 @0x091e | 2 | 0x02 | 0 | (-9350, 0, -1500) |
| 2020/2021 | main00 @0x0932 | 3 | 0x02 | 0 | (-11300, 0, 300) |

Rohbytes ROOM2020 @0x91e: `44 02 16 02 00 00 01 be 7a db 00 00 24 fa ...` (pc[4] = 0 = Band 0, y = 0).
Band pc[4] -> +0x82 (@0x800421c8/d0) und +0x1ba = -(pc[4]*1800) (@0x80042210, Port scd_vm.c).

SCA-Geometrie (RDT u32 @0x20, 5 Quadranten-Partitionen, je identisch; Zelle 12 B `w,d,x,z,type,u0,u1,floor`):
- ROOM2020: Band 0 (Wasserkanal z in (-2050..1500)) ist von Zellen mit **u1 = 0x01** eingefasst:
  [4] x=-21000..-2200 z=-3850..-2050, [5] x=-21000..-2200 z=1500..3300, [6] x=-2200..-400 z=-3850..3300 (floor 0x01).
  Band 1 (Stege y=-1800) sieht den Kanal als Zellen mit **u1 = 0x02**: [13] x=-21000..-2200 z=-2050..1500,
  [14] x=-400..13100 z=-2050..1500 (floor 0x10).
- ROOM2000: Band-0-Kanal (L-Form) eingefasst von u1 = 0x01-Zellen [2] x=-27000..-25200, [3] x=-21650..-19850,
  [4] z=-27000..-25200, [5] z=-21600..-19800; Band 1 sieht ihn als u1 = 0x02 ([12], [13]).
- Also: die Band-0-Zombies (2000: beide, 2020: Slots 2/3) stehen IM Kanal; die Kanalwand ist eine u1=1-Kante.

## 1. Messung vorher (Basis cb782bda, echte exe, Messkopie `re15_pc_u_mess.exe`)

Lauf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=2020@250 RE15_PLAYER_POS=-11300,2400,1024,1 RE15_GEGNER_Y_LOG=gy.log RE15_EXIT_AT=1400#2020`
(Leon auf dem Suedsteg, Band 1, vor dem Wasserzombie Slot 3 (-11300,0,300) Blick +Z) — je einmal mit
`RE15_AI_FLAVOR=re15` und `=re2`. ROOM2000: `RE15_DEBUG_JUMP=2000@250 RE15_PLAYER_POS=-20750,-10400,2048,1`
(Oststeg). Ausgewertet wurde gy.log (re15_trage1200_mess: Zustand/Clip/Bild/Lage/Band/+0x1ba je Bild).

**RE2-KI (Standard) — der Nutzerbefund, reproduziert:** ROOM2020 Aktor 4 (Wasserzombie) laeuft ab F7 auf Leon zu,
steht ab F101 an der Kanalwand z = 1082 (Wand z = 1500 minus Radius) und pendelt bis zum Raumende (F1296) zwischen
RE2-Gang 1/1 und 1/14 an derselben Stelle; y bleibt 0, Band 0. Aktor 3 genauso ab F549 (z = 1029..1093).
ROOM2000: beide 0x84-Zombies wachen auf (1/8 -> 1/1, F63), laufen gegen die Ostwand x = -22068 und bleiben dort
(F103..F1100, y = 0). **Kein Hochziehen, nie.**

**RE1.5-KI — Hochziehen wird ausgeloest, aber kaputt:** ROOM2020 Aktor 4: F10 Engage, F64 Kontakt -> 1/10
(Hochziehen), F71 Clip 22 (0x16), F229 Clip fertig: y = -1800, Band 1 — aber x/z unveraendert (-11486, 1082) =
UEBER dem Kanal, +0x1ba unveraendert 0. F230 greift die Schwerkraft (Band-1-Zelle u1=2 unter ihm): +0x1ba += 1800
-> **+1800** (falscher Ausgangswert), Band 0, der Zombie faellt bis **y = +1800 (1800 UNTER den Wasserboden)**,
loest im Fall erneut 1/10 aus, F397 y = 0 bei Band 1, F563 y = -1800 bei **Band 2**, laeuft danach auf "Band 2" weiter.
ROOM2000 identisch (Aktor 4: F102 1/10, F270 y=-1800 ueber dem Kanal, F271-290 Sturz bis y = +1800).

Ursache (Vergleich Port `re15_enemy_ai_live_edge_fall` mit 0x8010325c, Abschnitt 2): es fehlen
(a) `+0x1ba -= 1800` bei Bild 0x46 (@0x801033f8) und (b) der Vorschub FUN_800245d8(0) mit +0x8c = 0x474 am
Clip-Ende (@0x80103474, `jal 0x800245d8`; a0 = 0 aus dem Delay-Slot @0x80103420). Ohne (b) endet der Zombie
1140 Einheiten zu weit hinten, also ueber dem Kanal statt auf dem Steg.

## 2. RE-Belege (selbst disassembliert, STAGE2.BIN laedt roh @0x80100000)

Rosterscan (`re15_reg_scan.py STAGE2.BIN`): Typ 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f -> gemeinsame Wurzel **0x801002b8**.

Wurzel 0x801002b8: Zustandstabelle `lui at,0x8011 / addiu at,at,30904` = **@0x801178b8** [+0x4]
([0] 0x8010051c INIT, [1] 0x801010b8 ACTIVE, [2] 0x80105920, [3] 0x80106a38, [4] 0x80109030, [7] 0x801093e8).
ACTIVE-Ende: Grid-Modus `lbu v0,9 / andi 0xf` -> Tabelle **@0x80117910** (@0x801014e0-0x80101504).
Je Modus ein Doppel-Dispatch DECIDE/ANIMATE auf +0x5 (lui/addiu-Paare selbst gelesen):
Modus 0 -> 0x80117944 / 0x80117994, Modus 2 -> 0x80117a64 / **0x80117994** (dieselbe ANIMATE-Tabelle),
Modus 4 -> 0x80117acc / 0x80117ad0.

**Der Ausloeser (ENGAGE-Decide, +0x5 = 2, Modus 0 UND 2: @0x8011794c / @0x80117a6c -> 0x80101eec):**
```
80101f00: lbu  a0,144(a1)        ; +0x90 Kontaktbyte (SCA-Klemme FUN_8003b0a4)
80101f08: andi v0,a0,0x3         ; Bits 0-1 = cell.u1 & 3 (@0x8003b4c0-dc)
80101f0c: beq  v0,zero,0x80101f48
80101f10: andi v0,a0,0xf0
80101f14: lh   v1,106(a1)        ; +0x6a Gierwinkel
80101f18: sll  v0,v0,4           ; Kantenrichtung = (+0x90 & 0xf0) << 4
80101f1c: subu v0,v0,v1
80101f20: addiu v0,v0,512
80101f24: andi v0,v0,0xfff
80101f28: slti v0,v0,1024        ; Kante innerhalb +-512 vor dem Zombie
80101f2c: beq  v0,zero,0x80101f48
80101f30: andi v0,a0,0x1         ; u1 Bit 0
80101f34: addiu v0,v0,9
80101f38: sll  v0,v0,8
80101f3c: ori  v0,v0,0x1
80101f40: j    0x80102078
80101f44: sw   v0,4(a1)          ; +0x4-Wort = ((u1&1)+9)<<8 | 1  -> u1==1: 0xA01 (+0x5 = 0x0A)
```
Dasselbe Muster `addiu v0,v0,9 / sll v0,v0,8` steht noch @0x80102df8 (Decide [8], Ansturm) und @0x801054f8
(Decide [0x13]); sonst nirgends in STAGE2.BIN (Wortsuche).

**Das Hochziehen (ANIMATE [0x0A] @0x801179bc -> 0x8010325c; einzige Tabellenreferenz, Wortsuche):**
```
+0x6 == 0 (801032b0-e0): +0x6 = 1, +0x94 = 1 (Clip 1), +0x95 = 0, +0x8f = 7
+0x6 == 1 (801032e4-354): anim_set(+0x170,+0x174,0,0x200); d = FUN_8001aa68((+0x90&0xf0)<<4, 0x10);
                          +0x6a += d; d == 0 -> +0x6 = 2              (zur Kante drehen, 0x10/Bild)
+0x6 == 2 (80103358-3bc): +0x6 = 3, +0x94 = 0x16, +0x95 = 0, +0x8f = 7,
                          +0x8c = 0x474 (@0x80103398-9c), +0x6a = (+0x90&0xf0)<<4 (@0x801033ac-bc)
+0x6 == 3 (801033c0-478): Bild +0x95 == 0x46 (@0x801033d0): +0xb0 = 0x474 (@0x801033dc-e0),
                          +0x1ba -= 1800 (`addiu v0,v0,-1800` @0x801033f8, `sh v0,442` @0x801033fc);
                          anim_set(+0x170,+0x174,0,0x200); Clip fertig ->
                          +0x82 += 1 (@0x80103430-3c), +0x38 -= 1800 (`addiu v0,v0,-1800` @0x80103454,
                          `sw v0,56` @0x80103458), +0x4-Wort = 1 (@0x80103464-68), +0xb0 = 0 (@0x80103478)
```
PSX-Y waechst nach UNTEN: `+0x38 -= 1800` + `+0x82 += 1` hebt den Zombie EIN Band HOCH (Band 0 y=0 -> Band 1
y=-1800, vgl. Spawn-Records oben). Der Port fuehrt diesen Zustand als `re15_enemy_ai_live_edge_fall`
(enemy_ai_common.c, STAGE1-Zwilling 0x801033c8) — der Name "EDGE-FALL / tumbles over the edge" ist falsch gelesen:
es ist das HOCHZIEHEN an einer u1=1-Kante.

**Modus 4 (Deskriptor 0x84, ROOM2000):** Decide @0x80117acc[0] = 0x80103814: `+0x1d0 < 0xfa0` (4000) und +0x6 == 0
-> +0x6 = 1, +0x9c = rand&0xf, 1/4 Se(5). Animate @0x80117ad0[0] = 0x801038ec: +0x6 == 3 -> `sb zero,9` (Modus 0!)
+ `+0x4 = 0x201` (@0x801039d0-e0) = ENGAGE. Die ROOM2000-Zombies landen also im selben Engage-Decide 0x80101eec.

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
