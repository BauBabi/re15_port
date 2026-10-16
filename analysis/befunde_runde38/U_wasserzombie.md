# Runde 38 Spur U — wasserzombie (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_wasserzombie`, Zweig `r38/wasserzombie`, Basis `cb782bda`.

## Nutzer-Punkt (woertlich, AUFTRAG.md)

> Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand
> hochziehen können, wie im Original Resident Evil 1.5

## Stand

- 2026-10-16 18:16: Dossier angelegt (Vorgaenger, Sitzungslimit nach 3 min).
- 2026-10-16 (Fortsetzung): Zensus + RE-Belege (Abschnitte 0 und 2), Messung vorher (1), Umsetzung (3),
  Messung nachher (4), Riegel unit_ + integration_r38_wasserzombie gruen (5).

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

**Weitere Belege (selbst gelesen):**
- FUN_8001aa68 (PSX.EXE @0x8001aa68-c0): `rel = (Ziel - +0x6a + Rate) & 0xfff` (@0x8001aa74-84); `rel < 2*Rate -> 0`
  (Totband, @0x8001aa90-98); `rel <= 0x800 -> +Rate` (`sltiu 0x801` @0x8001aaa0), sonst `-Rate`. Schreibt +0x6a nicht;
  der Aufrufer addiert (`addu v1,v0,v1 / sh v1,106(a0)` @0x80103334-40, Delay-Slot = immer).
- FUN_800245d8(a0) (RE_15_Quellcode_V2/FUN_800245d8.c + Katalog): RotMatrixY(+0x6a + a0), GTE (+0x8c,0,0) ->
  X += IR1, Z += IR3. Am Clip-Ende `jal 0x800245d8` @0x80103474 mit a0 = 0 (`addu a0,zero,zero` im Delay-Slot
  @0x80103420, laeuft in beiden Zweigen) — also 0x474 = 1140 Einheiten entlang der Kante (Port-Zwilling
  re15_npc_pos_advance: x += cos*s>>12, z -= sin*s>>12).
- Schatten: die Zombie-Wurzel zeichnet ihn auf +0x1ba: `lh a1,442(v0)` @0x801004fc -> `jal 0x8001b064` @0x80100500
  (a0 = +0xb0 = Schattenblock). Ab Bild 0x46 liegt der Schatten also schon auf dem Steg.
- +0x1ba-Schreiber: STAGE2.BIN-Zombiebereich (0x80100000-0x8010a75c) nur @0x801033fc; PSX.EXE `sh ...,442(.)`:
  @0x8001be30 (Schwerkraft), @0x8004122c (Member_set 19), @0x80042210 (Sce_em_set), @0x80043300, @0x80052750,
  @0x80052d98 — kein Zombie-HURT/-Zustand setzt ihn zurueck.
- +0xb0: KEIN Leser. Wortsuche aller Lasten (lb/lbu/lh/lhu/lw) mit Offset 0xb0/0xb1 und Basis != sp: PSX.EXE nur
  @0x8003a660 `lbu s2,176(v0)` (v0 = sp+Index, Stapelpuffer), STAGE2.BIN keine; FUN_8001b064 liest vom Block nur
  +8/+10/+12/+14 (@0x8001b0f8/108/184/19c). Die zwei Stores sind ohne Wirkung -> nicht abgebildet.
- Clip 0x16 der RE1.5-Aktionsbank (Bank 1 = +0x170/+0x174 = dir[3]/dir[4], im Port b->anim, 43 Clips; gemessen mit
  re15_emd_parse_container ueber CDEMD0.EMS): 159 Bilder; Wurzel-Keyframe f0 (0,-1817,0), f100 (698,-2252,10),
  f150 (1444,-3278,111), f158 (0,-1698,0); Bild 130 traegt Bit 23 (= SE 1, FUN_8001b38c `srl s0,v0,22` @0x8001b3b4).
  Fuer Typ 0x10/0x11/0x12/0x16/0x18 identisch (Clip-Tabelle + Wurzelbahn); Skelett 0x10 == 0x16, 0x11/0x12 weichen ab
  (b0 -1885 statt -1751). Das IST das Hochziehen: Huefte steigt ~1460 und wandert 1444 nach vorn ueber die Kante.
- RE2 Retail (Gegenprobe "Beta -> Retail"): EMZ0.BIN (Zombie-Overlay) enthaelt keinen einzigen `addiu/ori` mit
  +-1800 (0x0708/0xf8f8) — RE2s Zombie zieht sich nicht hoch. Ziel ist hier also RE1.5 (Nutzer: "wie im Original
  Resident Evil 1.5"), auch fuer die Standard-KI (RE2).

**Zensus der Wirkung (alle 240 RDTs, u1==1-Zellen + Zombie-Spawns):** 1090/1091, 10B0/10B1, 2000/2001, 2020/2021,
3010/3011, 5020. Der Mechanismus ist im Original raumunabhaengig (jede u1=1-Kante); der Port bildet ihn ebenso ab.

## 3. Umsetzung

Neue Dateien:
- `re15_port/engine/src/wasserzombie_r38.c`, `re15_port/include/re15_wasserzombie_r38.h`:
  - `re15_wasserzombie_hochziehen` = ANIMATE [0x0A] 0x8010325c byte-true, beide KI-Varianten. Konstanten (Kopf):
    `RE15_WZ_CLIP_DREH` 1 (@0x801032c0), `RE15_WZ_CLIP_KLETTER` 0x16 (@0x80103368), `RE15_WZ_FRAC` 7
    (@0x801032e0/@0x8010338c), `RE15_WZ_RATE` 0x200 (@0x801032fc/@0x80103418), `RE15_WZ_DREHSCHRITT` 0x10
    (@0x80103310), `RE15_WZ_TEMPO` 0x474 (@0x80103398), `RE15_WZ_BODENBILD` 0x46 (@0x801033d0), `RE15_WZ_BAND` 1800
    (@0x801033f8/@0x80103454), `RE15_WZ_SUB` 0x0A (@0x80101f30-44).
    PORT-EMULATION (Konvention des Ports, wie Engage/Wander in enemy_ai_common.c): LOOP-Bit 0x04 in Phase 0/1
    (Phase 1 ruft anim_set jedes Bild = Wrap @0x8001f638-3c), in Phase 2 geloescht (Play-once, Ende im selben Bild);
    Rueckwaerts-Bit 0x80 geloescht (f314 a2 = 0 @0x801032ec/@0x80103400).
  - `re15_wasserzombie_kante_vorn` = Ausloeser @0x80101f00-44.
  - `re15_wasserzombie_re2_dispatch` (RE2-KI): ersetzt in `re15_re2z_tick` NUR den Zustands-Dispatch (Prolog mit
    +0x1D3/Sicht und Epilog mit Trefferfilter laufen weiter). Eintritt: RE2-Gang (Zustand 1/Sub 1, aufrecht) +
    `kante_vorn` + gemeldete RE1.5-Bank des eigenen Typs -> Wort 0xA01 (@0x80101f44) und dieselbe Maschine.
    Ende: Wort 1 (@0x80103464) und `re15_re2z_exec_only` im selben Bild (Muster SITZ-IMPORT 10D0, Original liest +0x5
    nach der DECISION neu @0x801011d0). Abbruch (Treffer, RE2-Partner-Domino 0x901): Marke faellt; PORT-WAHL nur RE2-KI:
    +0x1ba zurueck auf den Wert vor dem Hochziehen (RE2 fuehrt den Boden als Lage, `re2z_ground_y = y` in Zustand 0/1).
    PORT-WAHL: Rig-Wechsel RE2 <-> RE1.5 ohne Ueberblendung (`anim_prev_valid = 0`, die Rigs ordnen die Knochen anders).
    SE: Frame-Wort-SE der RE1.5-Bank wie FUN_8001b38c (im RE2-Zweig laeuft die RE1.5-Wurzel nicht).
  - RE1.5-Aktionsbank je Typ (`re15_wasserzombie_bank_merken`, PC-malloc, 8 Plaetze), Abfrage
    `re15_wasserzombie_bank` fuer Renderer/Uhr/Schuss-Anker, Schatten `re15_wasserzombie_schatten_y` (+0x1ba waehrend
    des Hochziehens), Messschiene `RE15_WASSERZOMBIE_LOG` (env-gegatet), Testhaken `re15_wasserzombie_letzte_landung`.
Haken in gemeinsamen Dateien (alle mit "Runde 38 Spur U"):
- `enemy_ai_common.c`: Include; Aufruf ANIMATE[0x0A] `re15_enemy_ai_live_edge_fall` -> `re15_wasserzombie_hochziehen`
  (alte Funktion bleibt unbenutzt, mit Hinweiszeile); `re15_actor_clip_len` taktet das RE2-KI-Hochziehen auf der
  RE1.5-Bank (2 Zeilen).
- `enemy_ai_re2_zombie.c`: Include + `if (!re15_wasserzombie_re2_dispatch(slot, e))` vor dem Zustands-`switch`
  (2 Zeilen; Spur L aendert re2z_hit_main case 3 — andere Stelle).
- `schuss_r36.c`: Include + Bankwahl des Schuss-Ankers (2 Zeilen).
- `platform/pc/main.c`: Include; Bank-Meldung im Hybrid-Lader (1 Zeile nach dem 0x10-Sitz-Block); Pose im
  NPC-Renderer (5 Zeilen nach dem SITZ-IMPORT-Zweig); Schatten (1 Zeile nach dem Hunde-Schatten).
Keine neuen Assets, keine Sprachdateien, keine Flags/Nachrichten-Ids.

## 4. Messung nachher

Echte exe, gleiche Laeufe wie Abschnitt 1, RE15_WASSERZOMBIE_LOG (Folge drehen/klettern/boden-1800/oben):
- ROOM2020 RE1.5-KI: F64 drehen (Kontakt 0xc9 = Richtung 0xC, u1=1) @(-11486,0,1082) -> F70 Clip 22 -> F140 Bild 70
  +0x1ba -1800 (y noch 0) -> F228 oben (-11486,-1800,2222) Band 1 +0x1ba -1800 Blick 3072. Danach Engage/Griff auf
  dem Steg (gy.log: y bleibt -1800, Band 1). Vorher: Sturz bis y = +1800, Band 2.
- ROOM2020 RE2-KI: F74 re2-eintritt -> F78 Clip 22 -> F148 +0x1ba -1800 -> F236 oben (-11221,-1800,2222) Band 1 ->
  re2-uebergabe (RE2 1/0, Clip 0). Vorher: stand dauerhaft an der Wand.
- ROOM2000 RE1.5-KI: beide 0x84-Zombies: Ost (-20928,-1800,-10118) F269, West (-25922,-1800,-9069) F331 (Blick 2048).
  RE2-KI: Ost (-20928,-1800,-9715) F275; der zweite wurde bei Bild 9 seines Clips vom Partner-Domino (0x901) des
  ersten unterbrochen (Abschnitt 6) und setzt danach neu an.
- Bild (Framedumps RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, RE15_FORCE_CUT=6, ROOM2020): F60 Zombie geht im Kanal auf die
  Kante zu, F80-F140 Haende auf dem Steg / Hochstemmen, F170-F200 Oberkoerper ueber der Kante, F220-F230 steht
  auf dem Steg neben Leon — in beiden KI-Varianten (RE2-KI: RE1.5-Pose auf dem Hybrid-Netz, keine zerrissene
  Geometrie). Kontaktbogen lokal: scratchpad sheet_re15.png / sheet_re2.png (nicht eingecheckt).

## 5. Tests

- `unit_r38_wasserzombie` (tests/unit/test_r38_wasserzombie.c, registriert in tests/unit/probes/r38_wasserzombie.cmake):
  A Clip 0x16 = Hochziehen (Bilder/Wurzelbahn/SE-Bit, 5 Typen); B Ausloeser inkl. Kegelraender +0x200/-0x1ff;
  C RE1.5-KI ROOM2020 (echte RDT/SCA/Spawns, game_step): Clipfolge 1 -> 0x16, Uhr 159, Bild 0x46 y 0/+0x1ba -1800/
  Schatten -1800, Landung y -1800/Band 1/+0x1ba -1800, Vorschub genau (0,+1140) entlang 0xC00, 30 Bilder stabil,
  120 Bilder nie unter den Wasserboden/ueber Band 1; D RE2-KI ROOM2020 dasselbe + RE1.5-Bank im Klettern, Ende RE2 1/0
  mit RE2-Clip; Gegenprobe ohne gemeldete Bank = kein Hochziehen; E ROOM2000 beide KI (Ost, Vorschub (+1140,0));
  F RE2-Abbruch per Partner-Domino 0x901 nach Bild 0x46 -> +0x1ba 0, y 0, Band 0, Schatten nicht mehr auf +0x1ba.
- `integration_r38_wasserzombie` (tests/integration/test_r38_wasserzombie.cmake, Werkzeug probe_r38_wasserzombie_karte):
  echte exe (Messkopie re15_pc_r38u_haken_*), LADE-Weg CONTINUE: a ROOM2020 Leon Standard-KI, b ROOM2020 Leon RE1.5-KI,
  c ROOM2021 Elza Standard-KI, d ROOM2000 ueber den Raumlader (Debug-Sprung); Log-Folge + Invariante "kein Zombie
  unter dem Wasserboden, keiner auf Band 2".

Gezielte Laeufe (KEINE volle Suite), Ergebnis woertlich:
- `ctest --test-dir re15_port/build -R "^integration_r38_wasserzombie$" --timeout 1500` ->
  `100% tests passed, 0 tests failed out of 1`
- Zombie-/Raum-Gruppen der beruehrten Module (enemy_ai_common.c, enemy_ai_re2_zombie.c, schuss_r36.c, Schwerkraft/
  Schatten), `-R '^(unit_enemy_ai|unit_room1140_spawn|unit_room1140_combat|unit_zombie_.*|unit_re2_zombie_.*|unit_re2_room.*|unit_re2z_.*|unit_knockdown_.*|unit_re15_zombie_death_anim|unit_re15_re2z_.*|unit_1090_.*|unit_1030_.*|unit_cam_1030_reentry|unit_re2_stagger_knockdown|unit_dog_grab_anchor|unit_re2_dog_grab_anchor|unit_rvd_scan_ungated_by_climb|unit_plc_back_yaw_1090|probe_10d0_situp_handoff|probe_1090_.*|probe_re2z_stomp|unit_map_1090|unit_r16_liegende_unschiessbar|unit_r16_aufstehen_schuss|r19_marke1_10d0_c1|unit_r35_cut1150_montage_1030|unit_r35_inhalt_zombies|unit_r35_raeume_.*|unit_r36_schuss.*|unit_r36_elza_schuss|unit_r36_blut_pin_.*|unit_r36_zombieki_.*|unit_r36_gegnerzahl_raum1030|unit_r37_beretta|unit_r37_zombieaktiv_.*|unit_r38_wasserzombie)$'`
  -> `100% tests passed, 0 tests failed out of 98`
- Integrations-Gruppen (Zombie-Schuss/RE2-Dispatch/Renderer-Schatten): siehe unten (folgt).

## 6. OFFEN

- RE1.5-KI, byte-true belassen: wird das Hochziehen NACH Bild 0x46 unterbrochen (Treffer), bleibt +0x1ba auf dem
  Steg (-1800), der Zombie steht im Wasser (kein Schreiber setzt es zurueck, Zensus Abschnitt 2). Folge im Original
  wie im Port: Schatten 1800 hoeher bis zum naechsten Hochziehen. Statisch hergeleitet, nicht dynamisch (DuckStation)
  gegengeprueft.
- RE2-KI, vorbestehend (nicht Spur U): der Partner-Domino der Griffphase P8 (`sw 0x901` @0x80102E90-98) nimmt den
  Partner aus `contact_slot` (Port-Abbildung von +0xD, dessen Produzent nicht RE'd ist) ohne Aktualitaetspruefung —
  ein frueherer Koerperkontakt im Wasser reicht. Gemessen ROOM2000 RE2-KI: der zweite Wasserzombie wird bei F408
  mitten im Hochziehen per 0x901 unterbrochen und setzt spaeter neu an. Gehoert zur RE2-Zombie-Spur.
- Vorbestehend (nicht Spur U), CONTINUE-Weg: `platform/pc/main.c` (s_preintro) haelt sub00 zurueck, sobald main00
  beim Boot-Tick ein Ereignis startet. ROOM2000 main00 @0x0177c `04 ff 18 07` (Evt_exec sub07) -> sub00 (traegt die
  Zombie-Spawns @0x017cc/@0x017e0 und die Ada/John-Szene) laeuft nach CONTINUE in ROOM2000 nie: KEIN Gegner im Raum
  (gemessen, gy.log ohne Aktoren). Ueber die Tuer/den Raumlader spawnen sie. Gehoert zum Lade-Weg.

## 7. Fuer den Nutzer

- Keine neuen Assets, keine Sprachdateien, kein Paket-Gate-Eintrag.
- Bedienung: keine. In ROOM2000/2020 (Leon) bzw. 2001/2021 (Elza) im Kanal stehende Zombies laufen auf dich zu, drehen
  sich an der Kante zur Wand, stemmen sich hoch und stehen danach auf dem Steg — in beiden KI-Einstellungen (RE2 und
  RE1.5). Dasselbe gilt (wie im Original) an jeder anderen als Kletterkante markierten Stelle (1090, 10B0, 3010, 5020).
- Hinweis (im Port gemessen, im Original nicht dynamisch geprueft): steht der Spieler genau auf dem Landeplatz, schiebt
  der Koerperstoss (FUN_8002aec4) den Zombie zurueck; liegt er dann ueber der Absturzkante (u1=2), faellt er per
  Schwerkraft (FUN_8001bd60) ins Wasser und setzt erneut an.
