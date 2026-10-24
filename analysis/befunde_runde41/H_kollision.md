# Runde 41 — Spur H "kollision" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_kollision`, Zweig `r41/kollision`, Basis master-HEAD 4c667c97 (2026-10-24).

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide fest."
2. "In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch"

## Stand
- 2026-10-24: Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- Punkt 2: Messung vorher + RE-Belege (FUN_8002bd44 @0x8002c03c-c0a8, FUN_8002cdf4), Umsetzung, Messung nachher.
- Punkt 1: Messung vorher (3070 Leon, 3001 Elza), RE (24 NPC-Wurzeln), Umsetzung, Messung nachher.
- Naechster Schritt: Riegel (probes/r41_kollision.cmake), Kisten-Raetsel loesbar, Regressionen der Szenen.

## Punkt 1 — NPC <-> NPC
### Messung vorher (exe dieses Baums, Stand 4c667c97 = re15_pc_h41_vorher.exe: Haken abgeschaltet, sonst gleich)
Werkzeug: RE15_STATE_LOG (Lage aller Aktoren je Bild) + `H_belege/npcdist.py` (Paarabstand der NPC-Familie
0x40..0x4d je Bild gegen die Radien-Summe; geparkte NPC (-30000,-30000) ausgenommen).

Wo laufen zwei NPC frei nebeneinander? Zensus aller Sce_em_set mit Typ 0x40..0x4d (scd_dump_room.py ueber alle 240 RDTs):
Ada (0x42) + Marvin (0x40) gemeinsam in 11B0, 1260, 2000, 3070, 3080, 30E0, 4000; als ESKORTE (Verhaltensbyte 0x00 ->
INIT `sb 1,4(v1)` @0x8011ccc0 = Zustand 1, folgt dem Spieler) BEIDE nur in **ROOM3070** sub00 @0x0343C/@0x03450
(`44 00 42 00 ...` Ada (-13350,-16780), `44 01 40 00 ...` Marvin (-13250,-15410), Bedingung Ck(3,180)=1 nach
(4,215)=1 und (4,236)=1). Elza-Gegenstueck: **ROOM3001** sub00 @0x015A2/@0x015B6 John 0x49 + Sherry 0x4b, beide
Verhaltensbyte 0x00, Bedingung Ck(4,199)=1 (nach (4,198)=1).
Beide Eskorten rechnen DENSELBEN Folgepunkt: Wurzel 0x8011ce54 `ori a0,zero,0x5dc` (1500) @0x8011ce78 ->
FUN_8005070c(1500, w, -w) @0x8011ce80/@0x8011ceb0 — 1500 hinter dem Spieler, fuer beide gleich. Getrennt werden sie
im Original NUR durch den Koerper-Schub des Wurzel-Schwanzes (s. RE-Belege).

| Lauf | Werkzeug/Weg | Paar | min. Abstand | Bilder < Radien-Summe |
|---|---|---|---|---|
| `npc3070_vorher` (Leon) | JUMP 3070@250, SET_FLAG 4:215,4:236,3:180, Skript W8,U3,W6,R0.5,U3,W8, 1500 Bilder | Ada/Marvin (Summe 900) | **14** (F644), Ende F1450 **23** | **1471** |
| `npc3001_vorher` (Elza, PSELECT_AUTO_SWITCH) | JUMP 3001@250, SET_FLAG 4:198,4:199, dasselbe Skript | John/Sherry (Summe 750) | **4** (F309) | **1449** |
| `npc10f0_vorher` (Leon, echte Tuer 10D0->10F0, Szene Runde 35) | Karte + CONTINUE, A0.2 | Ada/Marvin | 2724 | 0 |
| `npc11b0_vorher` (Leon, sub06 Ada+Marvin+Kanaldeckel) | JUMP 11B0, SET_FLAG 3:0x43,3:0x82,4:0xf3 bis ROOM1260 | Ada/Marvin | 971 | 0 |

Bild `H_belege/npc3070_vorher/f1450_vorher.png` (RE15_FRAMEDUMP=1450, WINDOW_SCALE=1): Ada und Marvin stehen als EINE
Figur ineinander (State F1450: Ada (-8510,-16750), Marvin (-8533,-16746)). **Nutzer-Befund reproduziert** (Eskorten-Paar,
beide Szenarien); in den Szenen 10F0/11B0 kommen sich die beiden zufaellig nicht nahe genug.

### RE-Belege (selbst disassembliert)
Port vorher: die NPC-Familie lief in `re15_enemy_ai_run_all` nur ueber `re15_npc_ai_tick` (enemy_ai_common.c), dessen
Schwanz NUR die Wandklemme hat (`re15_npc_wall_clamp`, @0x8011cc64). Spieler-Pass (re15_body_push_player) und
Gegner-Schwanz (re15_enemy_body_push_tail) schieben Spieler/Gegner aus den NPCs — die NPC selbst wurde nie geschoben.

Original: ALLE 24 NPC-Wurzeln (Registrierung `sw v0,0x2cXX(at)` auf 0x80072bac+typ*4, Scanner ueber STAGE1..6.BIN)
tragen nach dem Zustands-`jalr` dieselbe Kette (Ausgabe des Scans, alle `jal`/`jalr` in Reihenfolge):
```
STAGE1 0x40 0x8011c5a0: jalr, b498, aec4 @8011c66c (Delay: addiu a0,s0,20 @8011c670), b544 @8011c674, b0a4
STAGE1 0x42 0x8011cb70: jalr, b498, aec4 @8011cc3c (Delay @8011cc40), b544 @8011cc44, b0a4 @8011cc64
STAGE1 0x45/0x47/0x49/0x4b: aec4 @8011d248/@8011d7dc/@8011dd28/@8011e2f0, b544 je +8, b0a4 @8011d270/d804/dd50/e318
STAGE2 0x40 0x80115750 aec4 @8011581c b544 @80115824 | 0x42 0x80115d20 @80115dec/@80115df4 | 0x49 @801163b0 | 0x4b @80116978
STAGE3 0x40 0x8011b724 aec4 @8011b7f0 b544 @8011b7f8 | 0x42 0x8011bcf4 @8011bdc0/@8011bdc8 | 0x49 @8011c384 | 0x4b @8011c94c
STAGE4 0x40 @80116cb0 | 0x42 @80117280 | 0x49 @80117844 | 0x4b @80117e0c   (jeweils aec4; b544 +8)
STAGE5 0x40 @8011c004 | 0x42 @8011c5d4 | 0x49 @8011cb98 | 0x4b @8011d160 | 0x4d 0x8011d6d4 @8011d7dc
STAGE6 0x40 0x80100004 @801000d0 | 0x42 @801006a0 | 0x49 @80100c64 | 0x4b @8010122c | 0x4d 0x801017a0 @801018a8
```
Ada STAGE1 im Wortlaut:
```
8011cc1c: jalr v0                      Zustand (+0x4) aus 0x80121668
8011cc2c: jal 0x8002b498               Kasten-Versatz/Kontakt
8011cc34: lui a1,0x800b / lw a1,-14460(a1)   a1 = g_entity(cur) = die NPC
8011cc3c: jal 0x8002aec4
8011cc40: addiu a0,s0,20               a0 = 0x800aca40+20 = 0x800aca54 = SPIELER
8011cc44: jal 0x8002b544               NPC aus allen anderen Entitaeten
8011cc58: lw v0,120(a0) / 8011cc60: lhu a1,6(v0) / 8011cc64: jal 0x8003b0a4   Wandklemme (Radius Kasten+6)
```
Freeze-Tore davor: `lui v1,0x2000 / and / bne v0,zero,0x8011cc7c` @0x8011cb88-90 und `lbu +9 / andi 0x20 / bne
0x8011cc7c` @0x8011cba4-b0 springen ueber den GANZEN Schwanz.
FUN_8002b544 @0x8002b544-b5c8: `lbu s1,-13746(s1)` (g_active_count 0x800aca4e) @0x8002b550, s0 = 0x800acc2c @0x8002b564,
`andi v0,v0,0x1` @0x8002b578 (aktiv), `beq a1,s0` @0x8002b590 (nicht sich selbst), `jal 0x8002aec4` @0x8002b598 mit
`addu a0,s0,zero` im Delay-Slot (a0 = die ANDERE, a1 = g_entity) — KEIN Typfilter.
FUN_8002aec4 BEWEGT a1: Gates `or/andi 0x2` @0x8002af04-0c (Bit 2 an einem), `and/andi 0x1000` @0x8002af14-1c (beide),
`andi v1,0x4` @0x8002af24 (Geschobener); Radien Kasten+6 (`lhu t4,6(s6)` @0x8002af40, `lhu t4,6(s5)` @0x8002af4c);
dist = SquareRoot0(dX^2+dZ^2) `jal 0x80065f60` @0x8002b1d8; pen = R - dist `subu s0,s0,a2` @0x8002b1e4, `blez` @0x8002b1e8;
Y-Band @0x8002b1f0-b228 (Kasten +2/+8 beider Seiten); Schub `mult s1,s0` / `addiu a2,a2,1` / `div v0,a2` @0x8002b22c-238
= dX*pen/(dist+1) (Port: re15_body_push, seit Runde 7 byte-true).
Kasten der NPC: 0x42 `lw v0,5732(v0)` @0x8011ccec -> 0x80121658 = `00 00 06 fa 00 00 c2 01 fa 05 c2 01` = r 450 / h 1530;
0x45 r 500 @0x80121728; 0x4b r 300 @0x801218c8 (re15_damage.c re15_enemy_apply_hitbox).

**Original-Messung (dynamisch, DuckStation-GDB, `H_belege/orig_3070_eskorte.py`, Log `H_belege/orig_3070_eskorte/log.txt`):**
Spielstand stage_saves/mzd_debugmenu.sav (Auslieferungs-EXE: `@0x80026e4c = 0800e003`), Flags per RAM gesetzt (Bank-Tabelle
0x80074664: Bank 3 -> 0x800b0ff8, Bank 4 -> 0x800b1018; (4,215) @0x800b1030 |= 0x100, (4,236) @0x800b1034 |= 0x80000,
(3,180) @0x800b100c |= 0x800), Debug-Menue-Cursor direkt gesetzt (Stage 0x800BBE5E = 2, Index 0x800BBE61 = 7 -> 0x307;
Lauf 1 mit Tastenzaehlen landete in 0x306 = ROOM3060, `orig_3070_eskorte_lauf1_0x306/`), Square -> Laden.
Ergebnis: `Raum/Stage 0x800b0fe0/fe2 = 2/7`, Entity 0 Typ 0x42 Zustand 1, Entity 1 Typ 0x40 Zustand 1 (= die beiden
Eskorten); Leon steht 25 s, geht zweimal 1,5 s: **187 Proben, min. Abstand 901, letzter 1086** (Radien-Summe 900) —
z.B. `STEHEN 0.0s ... E0 typ=0x42 st=1 sub=0 (-13382,-16785) | E1 typ=0x40 st=1 sub=1 (-12777,-16117) | d=901`:
Marvin will weiter (Sub 1 = gehen), steht aber bei 901 an Ada. **Im Original sind beide fest.**
Port-Spiegel desselben Ablaufs (JUMP 3070, gleiche Flags, Skript W25,U1.5,W12,U1.5,W12, `H_belege/npc3070_wieorig_*`):
vorher **min 3** (1671 Bilder < 900, F1690 Ada (-7527,-17130) / Marvin (-7528,-17133)), nachher **min 897** (Ende 901).
### Umsetzung
- NEU `engine/src/kollision_r41.c` + `include/re15_kollision_r41.h`: `re15_kollision_r41_npc_schwanz(slot)` =
  aec4(&Spieler, NPC) (`re15_body_push(pl, pl->hit_radius_min, npc, npc->hit_radius_min)`, Gate: Spieler aktiv und
  hp >= 0 wie re15_enemy_selfpush_from_player) + b544 (aufsteigend ueber alle anderen aktiven Aktoren mit Kasten,
  Leichen ausgenommen = dieselbe Port-Abbildung des Bit-2-Gates wie re15_enemy_body_push_tail). Keine neue Konstante:
  Radien = hit_radius_min (Kasten +6), Formel = re15_body_push.
- Haken (1 Zeile) in `enemy_ai_common.c` re15_npc_ai_tick zwischen `re15_npckante_altlage` und `re15_npc_wall_clamp`
  (= nach dem Zustands-`jalr`, vor der Klemme @0x8011cc64, hinter den Freeze-/SCD-Yield-Ruecksprungen).
  ⛔ ABWEICHUNG VOM AUFTRAGSTEXT ("nur Haken in game_step_common.c"): die Reihenfolge Dispatch -> Schub -> Wandklemme
  liegt INNERHALB des NPC-Ticks; aus game_step_common.c (nach run_all) liefe der Schub nach der Klemme und nach allen
  anderen Ticks (Waende verloeren, andere Verzahnung). Die Zeile beruehrt keine Stelle der Spur A (deren NPC-Haken sitzen
  in re15_npc_sub_turn/re15_npc_sub_walk); Include-Zeile hinter re15_npckante_r38.h (nicht neben A's Include).
- NICHT im Schwanz: die Yield-Zweige des Ports (walk_active = Elliot 0x47 Altweg; SCD-besessener Slot ausserhalb der
  Executor-Subs) kehren VOR dem Schwanz zurueck, wie schon bisher fuer die Wandklemme — s. OFFEN.
### Messung nachher (re15_pc_h41_mess.exe = Stand mit Haken, sonst identisch)
| Lauf | Paar | min. Abstand | Bilder < Radien-Summe | Ende |
|---|---|---|---|---|
| `npc3070_nachher` (Leon, gleicher Weg) | Ada/Marvin (900) | **884** (F609) | 307 (alle 884..899) | F1450 Ada (-8496,-16912) / Marvin (-9233,-16393) = 901 |
| `npc3001_nachher` (Elza, gleicher Weg) | John/Sherry (750) | **837** | **0** | — |

Die Restunterschreitung in 3070 (min 884) ist die Wandklemme NACH dem Schub, wie im Original (b0a4 @0x8011cc64 hinter
b544 @0x8011cc44): F609 Schub Marvin (-5776,-16595) -> (-5723,-16588) (`kollision.txt`), danach Klemme auf (-5741,-16588)
(state.txt) = 18 Einheiten zurueck Richtung Ada. Und dX*pen/(dist+1) laesst je Schub < 1 Einheit Rest (899 statt 900).
Bild `H_belege/npc3070_nachher/f1450_nachher.png`: zwei getrennte Figuren nebeneinander.

## Punkt 2 — Kisten 2050
### Messung vorher (exe dieses Baums, Stand 4c667c97, Kopie re15_pc_h41_mess.exe)
Werkzeug: Wegplan-Treiber RE15_WEGPLAN (dachkiste_r36.c, geschlossen geregelt) + RE15_PUSH_LOG/RE15_OBJFALL_LOG.
Lauf `H_belege/kiste_vorher/` (wegplan.txt, wegplan_log.txt, push_schub.txt, objfall_ende.txt):
`RE15_DEBUG_JUMP=2050@250 RE15_PLAYER_POS=-2700,-64,0,0` (Band 0, westlich von obj 2, Blick Ost), `OBJ 2`, `GEGNERWEG`
(die zwei Adult Spider 0x25 frei), `SCHIEB 0 X >= 4500 1200`.

Raum: ROOM2050.RDT sub00 installiert die drei Raetsel-Kisten (Typ 4, Band 0, Box c(0,-900,0) h(900,900,900), pc[6..7] =
00 01 -> Pool-Wort 0x0101): obj 1 @0x01DB4 (2052,0,-5552), obj 2 @0x01DD6 (-1256,0,-64), obj 3 @0x01DF8 (4248,0,1400)
(Raetsel: die drei Kisten in die Kerben Aot slot 3/4/5 @0x01CAA/@0x01CBE/@0x01CD2, sce 5, x -7600..-5550).

Ergebnis: 289 Zeilen `SCHUB-OK`, keine einzige `SCHUB-GESPERRT`; obj 2 laeuft von x -1256 bis `F360 ... lage=(4497,-64)
ziel=(4506,-64)`, Bedingung `obj.x = 4506` erfuellt. Endlage `F1500 ... p3 (4506,0,-64) b0 | p4 (4248,0,1400) b0`:
obj 2 steckt in obj 3 (|dx| = 258 <= 1800 = hx+hx, |dz| = 1464 <= 1800, beide y 0) = **Kisten ineinander geschoben,
Nutzer-Befund reproduziert.**

### RE-Belege (selbst disassembliert, re15_disasm.py auf info/Re1.5/PSX.EXE)
Schubkette FUN_8002bd44, Zweig nach angenommenem Zellentest:
```
8002bfa4: jal 0x8002cabc ; ori a2,zero,0x1   FUN_8002cabc(Spieler,obj,1) setzt die Kiste (Spieler +- (hx+450))
8002bfb0: jal 0x8003b558 ; ori a1,zero,0x2   FUN_8003b558(obj,2) Zellentest
8002bfb8: bne v0,zero,0x8002c0b0             Zelle -> gesperrt
8002bfc0: sb v0(=8),140(s2)                  obj+0x8C = 8
8002bfd4: ori v0,v0,0x2000 / sw 0x800aca3c   Kontaktbit
8002bfe4: bne v1(=player+5),8,0x8002c0b8     nicht Schub-Substate -> Ruecksetzung
8002bfec..c038: Entity-Durchlauf (g_active_count, 0x800acc2c, Stride 500): FUN_8002cabc(entity,obj,1) != 0 -> sb s7,140(s2)
8002c03c: lw v0,0x800ac778 / lbu v0,2(v0)    Objektzahl = RDT-Kopf +2 (nOmodel)
8002c050: beq v0,zero,0x8002c0d8             keine Objekte -> Schub bleibt
8002c058: addu s0,fp,zero                    s0 = Pool-Anfang (0x800b3f98)
8002c05c: beq s2,s0,0x8002c088               das geschobene Objekt selbst ueberspringen
8002c064: lw v0,0(s0) / andi v0,v0,0x1 / beq -> 0x8002c088   nur aktive Objekte
8002c078: jal 0x8002cdf4 ; addu a1,s2,zero   FUN_8002cdf4(anderes, geschobenes)
8002c080: bne v0,zero,0x8002c0b0             UEBERLAPPUNG -> GESPERRT
8002c09c: sltu v0,s1,v0 / bne -> 0x8002c05c ; addiu s0,s0,148   naechstes Objekt (Stride 0x94)
8002c0a8: j 0x8002c0d8                       kein Treffer -> Schub bleibt
8002c0b0: sb s7(=10),140(s2)                 obj+0x8C = 10
8002c0bc..c0d4: obj+0x34/+0x3C = gesicherte Lage (s4/s5 @0x8002beec), jal 0x8003b558(obj,2) (Ruhe-Zweig)
8002c0ec..c10c: Spiegel obj+0x42 = y, +0x40 = x, +0x44 = z (lhu 56/52/60 -> sh 66/64/68)
```
FUN_8002cdf4(a0 = anderes Objekt t2, a1 = geschobenes Objekt a3) — AABB mit Halbmass-Summen, beide Boxen aus +0x78:
```
8002ce0c: lw v0,0(t2) / lw v1,0(a3) / or / andi 0x40 / bne -> return 0     Bit 0x40 in einem der Woerter: kein Test
8002ce30..ce68: dX = (a3.box.cx + a3.x) - (t2.box.cx + t2.x), dZ dito (box +0 / +4, Lage +0x34 / +0x3C)
8002ce78: a1 = t2.hx + a3.hx (box +6)      8002ce7c: t3 = t2.hz + a3.hz (box +10)
8002ce80..ce8c: sltu (2*sumX) < (dX+sumX) -> return 0   (= |dX| <= sumX, inklusive)
8002ce94..cea0: dito Z
8002cea8..cee0: dY = (a3.box.cy + a3.y) - (t2.box.cy + t2.y) (+2 / +0x38), sumY = hy+hy (box +8); keine Y-Ueberlappung -> 0
8002cee8..cf10: Vor-Y aus dem Spiegel +0x42 beider Objekte: lag die Y-Ueberlappung im Vorbild NICHT vor (vertikaler
                Eintritt), setzt @0x8002cf18-3c a3.y = t2-Mitte +- (sumY+1) - a3.box.cy und gibt 1 zurueck
8002cf14..cf68: sonst Achse: |dX*sumZ| (mult t7,t3) gegen |dZ*sumX| (mult t5,a1); X wenn |dZ*sumX| < |dX*sumZ|
8002cf70..cf94: a3.x = t2.x +- (sumX+1) (+ bei dX >= 0)   8002cf98..cfb4: a3.z = t2.z +- (sumZ+1)
8002cfb8: ori v0,zero,0x1 -> return 1
```
Der Aufrufer setzt danach x/z auf die gesicherte Lage zurueck (@0x8002c0bc-d4) — die Ausschiebung von FUN_8002cdf4 in
x/z bleibt also ohne Wirkung, nur die Rueckgabe zaehlt. Port: `re15_prop_push_tick` (aot_common.c) fuehrte den
Durchlauf @0x8002c03c-c0a8 nicht (Kommentar dort: "Objekt-gegen-Objekt-Pass FUN_8002cdf4 ... Nicht portiert", mit der
1090-Begruendung zweier Kisten in verschiedenen Baendern — fuer 2050 mit drei Kisten in Band 0 falsch).

**Original-Messung (dynamisch, DuckStation-GDB, `H_belege/orig_2050_kiste.py`, Log `H_belege/orig_2050_kiste/log.txt`):**
Spielstand analysis/befunde_runde36/R_belege/room2050_orig.sav (Auslieferungs-EXE `0800e003`), Spinnen aus (Aktiv-Bit +
g_active_count), Raetsel-Kiste obj 1 per RAM SUEDLICH vor die Band-3-Kiste obj 6 gesetzt (Pool 0x800b3f98+0x94: +0x34/
+0x38/+0x3C = (-1600,-5400,-6948), +0x82 = 3, Spiegel +0x40..+0x44), Kaesten beider (0,-900,0,900,900,900) aus *(+0x78)
gelesen; Leon hinter obj 6, Schub nach Sueden in drei Runden. Haltepunkte @0x8002bfc0 / @0x8002c0b0 / @0x8002cfb8 /
@0x8003da78. Ergebnis: `SUMME: Schub angenommen 83, gesperrt 3, davon Objekt-Treffer 3, Zellen-Treffer 0`; jeder
gesperrte Schub ist `OBJEKT-TREFFER @0x8002cfb8 t2(anderes)=800b402c a3(geschoben)=800b4310` (= Pool + 0x94*1 = obj 1,
Pool + 0x94*6 = obj 6), danach `SCHUB-GESPERRT` @0x8002c0b0; obj 6 bleibt bei z -8760 / -8757 / -8753 stehen (|dZ| 1812 /
1809 / 1805 > 1800). (Die am Haltepunkt gelesene Lage -8749 ist schon die Ausschiebung von FUN_8002cdf4 —
`sw v0,60(a3)` @0x8002cfb4 liegt VOR `ori v0,zero,0x1` @0x8002cfb8 — und wird von @0x8002c0bc-d4 wieder verworfen.)
**Im Original sperrt eine Kiste die andere.**
Port-Gegenstueck mit derselben Aufstellung (Messhaken RE15_KOLLISION_SETZE=1:-1600,-5400,-6948,3@5, Wegplan SCHIEB
3072, `H_belege/kiste_obj6_port/`): 69 SCHUB-OK, 3 SCHUB-GESPERRT, alle drei `OBJEKT-GESPERRT p=0 obj=6 ... gegen p=2
obj=1 lage=(-1600,-5400,-6948) dZ=-1787/-1791/-1782`, obj 6 bleibt bei -8768 / -8759 / -8750 (|dZ| 1820 / 1811 / 1802)
— **Port = Original** (gleiche Mechanik, gleicher Endabstand 1802..1820).
### Umsetzung
- `kollision_r41.c`: `re15_kollision_r41_8002cdf4(a, b, bx, bz, alt_ya, alt_yb, &neu_yb)` = FUN_8002cdf4 Zeile fuer Zeile
  (Gate 0x40 @0x8002ce0c-24, AABB mit Halbmass-Summen inklusive @0x8002ce80-a0, Y @0x8002ced0-e0, vertikaler Eintritt aus
  dem Spiegel +0x42 @0x8002cee8-cf3c, sonst 1 @0x8002cfb8); `re15_kollision_r41_kisten_pass(rdt, p, nx, nz)` = Durchlauf
  @0x8002c03c-c0a8 ueber obj-Nummer 0..nOmodel-1 (RDT-Kopf +2), erster Treffer gewinnt.
- Haken `aot_common.c` re15_prop_push_tick: `objgesperrt` nur nach angenommenem Zellentest und im Schub-Substate (wie
  @0x8002bfb8/@0x8002bfe4); Treffer -> Zaehler 10 + Ruhe-Zweig (= @0x8002c0b0 -> @0x8002c0b8-d4), Kontaktbit bleibt
  gesetzt (@0x8002bfd4 lief vorher). RE15_PUSH_LOG meldet den gesperrten Schub als SCHUB-GESPERRT (der Wegplan-Treiber
  zaehlt damit die Sperre); Detailzeile OBJEKT-GESPERRT in RE15_KOLLISION_LOG.
- Spiegel +0x42 (y): `re15_kollision_r41_objspiegel()` per Haken in game_step_common.c nach dem Objekt-Pass des Bildes
  (nach re15_objfall_nach_pass) — "y am Ende des letzten Passes" (@0x8002c0f8).
- Kommentar in aot_common.c berichtigt (der Pass hiess dort "nicht portiert", Begruendung nur fuer 1090).
### Messung nachher (re15_pc_h41_mess.exe, derselbe Wegplan, `H_belege/kiste_nachher/`)
`F260 ... lage=(2405,-64) ziel=(2414,-64)` ... `F261 ... SCHUB-OK ... ziel=(2434,-64)`, dann
`F262 room=2050 p=3 obj=2 SCHUB-GESPERRT spieler=(1112,-25) rot=0 lage=(2434,-64) ziel=(2462,-64) band=0` und
`F262 room=2050 OBJEKT-GESPERRT p=3 obj=2 ziel=(2462,-64) gegen p=4 obj=3 lage=(4248,0,1400) dX=-1786 dZ=-1464 sumX=1800
sumZ=1800` (|dX| 1786 <= 1800 und |dZ| 1464 <= 1800 -> Treffer). Endlage `p3 (2434,0,-64) | p4 (4248,0,1400)`:
|dX| = 1814 > 1800 = die Kisten beruehren sich nicht. 190 SCHUB-OK, 1 SCHUB-GESPERRT; danach steht der Zaehler auf 10
und laeuft bei gehaltenem UP weiter (11, 12, ... — kein neuer Versuch, bis UP losgelassen wird: @0x8002bf60-74).

**Raetsel bleibt loesbar (exe, Wegplan, `H_belege/raetsel_leon/`, `H_belege/raetsel_elza/`):** obj 2 nach Westen in Kerbe 5
(z -64), obj 3 nach Norden bis z <= -1880 und nach Westen in Kerbe 4, obj 1 nach Sueden bis z >= -3740 und nach Westen in
Kerbe 3 -> `FLAG (5,0) = 1`, `(5,1) = 1`, `(5,2) = 1`; Endlagen Leon `p2 (-6636,0,-3718) | p3 (-6614,0,-64) | p4
(-6644,0,-1887)`, Elza ROOM2051 `p2 (-6608,0,-3730) | p3 (-6617,0,-64) | p4 (-6622,0,-1883)` (|dZ| 1831/1823 bzw.
1847/1819). Ein erster Versuch mit obj 1 bei z -3668 wurde vom Pass gesperrt (`OBJEKT-GESPERRT p=2 obj=1 ziel=(-4845,-3668)
gegen p=4 obj=3 ... dX=1799 dZ=-1781`): die Kerben liegen im Raster 1900/1850, die Kisten sind 1800 tief — dazwischen
bleiben 100/50 Einheiten Spiel, genau wie im Original (FUN_8002cdf4 ist dieselbe Pruefung). Fenster fuer obj 1:
z in (-3750, z3 - 1800) (r0[0] @0x00910 sperrt die Kiste bei z < -3750, Zellentest FUN_8003b558).

## Tests
(offen)

## OFFEN
(offen)

## Fuer den Nutzer
(offen)
