# Runde 41 — Spur H "kollision" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_kollision`, Zweig `r41/kollision`, Basis master-HEAD 4c667c97 (2026-10-24).

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide fest."
2. "In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch"

## Stand
- 2026-10-24: Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- Punkt 2: Messung vorher + RE-Belege (FUN_8002bd44 @0x8002c03c-c0a8, FUN_8002cdf4), Umsetzung, Messung nachher.
- Punkt 1: Messung vorher (3070 Leon, 3001 Elza), RE (28 NPC-Wurzeln), Umsetzung, Messung nachher.
- Riegel probes/r41_kollision.cmake gruen, Kisten-Raetsel loesbar (Leon + Elza), Original-Messungen beider Punkte,
  Regressionen gefahren (ein Querbezug R39 J angepasst), master (A + D) gemergt und nachgemessen.
- 2026-10-24 Nachbesserung 1: V1/V2 der Abnahme H 0 dokumentiert (Orchestrator-Entscheid H-E1: beide angenommen),
  neuer Beleg zu den aec4-Gates fuer NPC/Gorilla (V2); Code unveraendert. Abschnitt 'Nachbesserung 1' am Ende.

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

Original: ALLE 28 NPC-Wurzeln (STAGE1 6, STAGE2-4 je 4, STAGE5/6 je 5) (Registrierung `sw v0,0x2cXX(at)` auf 0x80072bac+typ*4, Scanner ueber STAGE1..6.BIN)
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
| `npc3001b_vorher` / `_nachher` (Elza bei (-18000,-13378) Blick Ost, Skript W25,U1.5,W12,U1.5,W12 — der Riegel-Lauf) | John/Sherry (750) | vorher **16** -> nachher **740** | vorher 1671 -> nachher 0 Bilder < 700 | nachher 752 |

Die Restunterschreitung in 3070 (min 884) ist die Wandklemme NACH dem Schub, wie im Original (b0a4 @0x8011cc64 hinter
b544 @0x8011cc44): F609 Schub Marvin (-5776,-16595) -> (-5723,-16588) (`kollision.txt`), danach Klemme auf (-5741,-16588)
(state.txt) = 18 Einheiten zurueck Richtung Ada. Und dX*pen/(dist+1) laesst je Schub < 1 Einheit Rest (899 statt 900).
Bild `H_belege/npc3070_nachher/f1450_nachher.png`: zwei getrennte Figuren nebeneinander.

Nebenbefund am Lauf `npc3001_nachher` (Elza stand ZWISCHEN den beiden, Sherry kam von Osten): John und Sherry bleiben
1300-1700 auseinander, Sherry "geht auf der Stelle" vor Elza — aec4(&Spieler, NPC) @0x8011cc3c schiebt sie radial aus
Elza heraus, sie kommt nicht durch Elza hindurch (vorher lief sie durch Elza durch und stand mit John ineinander).
Das ist die Original-Mechanik (der Schub wirkt auf den Spieler und die NPC gegenseitig: Spieler-Tick b544 @0x80031cbc,
NPC-Tick aec4 @0x8011cc3c), kein Fehler; der Riegel npc3001 stellt Elza deshalb oestlich der beiden auf.

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

## Querbezug R39 J: integration_r39_dining1141_kampf (gemessen, angepasst)
Im gezielten Lauf rot: `NB2 (a): 0 Treffer, waehrend Elza im Griff ist (gr=1); kein Fehlschuss wegen des Opferzustands (0)`.
Mit der exe OHNE Haken (re15_pc_h41_vorher.exe) gruen (2 Treffer bei gr=1) — Ursache ist also Runde 41: John (0x49,
Wurzel STAGE1 0x8011dc68, aec4 @0x8011dd28 / b544 @0x8011dd30) wird jetzt von einem Gorilla (Kasten 1600) in die Ecke
geschoben (vorher (-7535,-21362), nachher (-8143,-21636) ab F3600), die Gorilla-Wege und damit die Griff-Fenster
verschieben sich. Johns Salven kommen im festen Takt (~371 Bilder: F3446/3821/4192/4563/4934/5305), die Griffe dauern ~120
Bilder (nachher 3561-3684, 3960-4083, 4337-4449, 4708-4820) — jede Salve liegt ~140 Bilder VOR einem Griff, keine faellt in ein
Fenster. Kein Schuss ist am Opferzustand gescheitert; (a) ist eine Zufalls-Koinzidenz der Zeitachse. Messreihe (Elzas WARTE vor
dem letzten GEHE, sonst unveraendert, `scratchpad dining_var`):

| WARTE | vorher (ohne Haken) | nachher |
|---|---|---|
| 60 (bisher) | (a) 2 Treffer bei gr=1, gruen | (a) 0, rot |
| 180 | — | (a) 0, rot |
| 240 | (a) 3, alles gruen | (a) 3, alles gruen |
| 300 | (a) 0 + Querungs-Wache rot (Gorilla x -7368, 1427 Bilder) | dasselbe (1416 Bilder) |

Anpassung (Kommentar "Runde 41 Spur H" im Riegel): WARTE 60 -> 240, Kriterien unveraendert; der Riegel unterscheidet weiter
(eine wiederkehrende Opferzustands-Sperre machte die Schuesse bei gr=1 zu Fehlschuessen). Nebenbefund fuer den Orchestrator
(nicht Runde 41, mit beiden exe gleich): bei WARTE 300 steht ein Gorilla 1400 Bilder lang bei x -7368 (Wand-Grenze des Riegels
-7100, Querungs-Wache der Spur E R39).

## Gemeinsame Dateien (Haken, je "Runde 41 Spur H")
- `engine/src/enemy_ai_common.c`: 1 Include (hinter re15_npckante_r38.h) + 1 Zeile in re15_npc_ai_tick vor
  `re15_npc_wall_clamp` (`re15_kollision_r41_npc_schwanz(slot)`). ⛔ NICHT in der Haken-Liste der Spur H (VERTRAG §1.3
  nennt re15_collision.c / game_step_common.c / aot_common.c) und abweichend vom Auftragstext "nur Haken in
  game_step_common.c": die Original-Reihenfolge Dispatch -> aec4/b544 -> Wandklemme (@0x8011cc1c / @0x8011cc3c-44 /
  @0x8011cc64) liegt INNERHALB des NPC-Ticks; ein Haken nach run_all in game_step_common.c liefe hinter der Klemme
  (eine geschobene NPC stuende in der Wand) und hinter allen anderen Ticks. Keine Ueberschneidung mit Spur A (deren
  NPC-Haken sitzen in re15_npc_sub_turn/re15_npc_sub_walk, Include hinter re15_esp.h) — fuer die Integration vermerkt.
- `engine/src/aot_common.c`: 1 Include (hinter re15_zellschub_r36.h, nicht neben Spur A's Include) + Objekt-Pass in
  re15_prop_push_tick (3 Zeilen + berichtigter Kopfkommentar).
- `engine/src/game_step_common.c`: 1 Include + 1 Zeile `re15_kollision_r41_objspiegel()` nach re15_objfall_nach_pass.
- `tests/integration/test_r39_dining1141_kampf.cmake` (Riegel der Spur J R39): WARTE 60 -> 240 (s. Querbezug oben).
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine Assets, kein Eingriff in shared_assets/pri/synchro/release.
- Messhaken (kein Spielverhalten, env): `RE15_KOLLISION_LOG=<datei>` (NPC-SCHUB / OBJEKT-GESPERRT / SETZE),
  `RE15_KOLLISION_SETZE="<obj>:<x>,<y>,<z>,<band>@<bild>"` (Objekt einmal je Raum versetzen = Gegenstueck der
  RAM-Schreibung der Original-Messung).

## Tests
Riegel `re15_port/tests/unit/probes/r41_kollision.cmake` (Quellen tests/unit/test_r41_kollision.c,
tests/unit/probe_r41_kollision_abstand.c, tests/integration/test_r41_kollision.cmake):
- `unit_r41_kollision_npc` — Schub-Formel mit SquareRoot0 (SquareRoot0(250000) = 499 -> Marvin x 901; Spieler-Schub
  (0,-305) mit SquareRoot0(360000) = 596), nur a1 bewegt sich, Gates toter Spieler / Leiche / Y-Band (3060 strikt), Kasten
  0x4b 300.
- `unit_r41_kollision_raum3070` — echte ROOM3070, Ada + Marvin Eskorte: 900 Bilder, Weg 4190/5128, min 899, Ende 900,
  870 Kontaktbilder.
- `unit_r41_kollision_raum3001` — echte ROOM3001 (Elza-Raum), John + Sherry: min 732 (>= 750-20), Ende 750.
- `unit_r41_kollision_kiste` — ROOM2050: Grenze |dX| 1800 gesperrt / 1801 frei, Durchlauf, Gate 0x40, anderes Band frei,
  vertikaler Eintritt y 1801, Raetsel-Endlage ohne Sperre, obj 1 bei z -3687 gesperrt.
- `integration_r41_kollision_kiste` — exe: SCHUB-GESPERRT + OBJEKT-GESPERRT gegen obj 3, Lage obj 2 nie >= 2448.
- `integration_r41_kollision_raetsel_leon` / `_raetsel_elza` — exe: (5,0..2) = 1, Endlagen |dZ| > 1800.
- `integration_r41_kollision_npc3070` (Leon) / `_npc3001` (Elza) — exe: Kontakt ja, kein Bild < Summe-50, min >=
  Summe-20, Ende >= Summe-5, NPC-SCHUB im Log.
Gezielte Laeufe (Summenzeilen woertlich): s. unten "Ergebnisse".

### Ergebnisse der gezielten Laeufe (ctest-Summenzeilen woertlich)
Stand vor dem master-Merge (Commit 5f7dca2e, Build = dieser Stand):
- `ctest -R "^(unit_|prop_|probe_)" -j 6 --timeout 600` -> `100% tests passed, 0 tests failed out of 920`
  (`Total Test time (real) = 798.36 sec`; enthaelt unit_r41_kollision_npc/raum3070/raum3001/kiste).
- `ctest -R "^integration_r41_kollision" -j 3` -> erster Lauf 3/5 (kiste: Pruefung las das ZIEL statt der LAGE — eine
  SCHUB-OK-Zeile ausserhalb Substate 8 wird @0x8002c0b8 zurueckgesetzt; npc3001: Elza stand zwischen den Eskorten, s. o.),
  nach den Riegel-Korrekturen `-R "^integration_r41_kollision_(kiste|npc3001)$"` -> `100% tests passed, 0 tests failed out of 2`.
- NPC-/Kisten-Regressionen, `-R "^integration_(r35_cut10f0|r35_cut1150|r36_szene1150|r36_szenenleon|r37_szene11c0|
  r37_sperre1050|r39_parkplatz11c0_[kwnei]|r37_roy_szene1011|r37_roy_szene1021|r37_roy_leon1170|r38_dining1141_[wzdlsra]|
  r39_dining1141_.*|r38_heliport1171_.*|r39_heliport1171_.*|r39_johnruf1041_.*|r39_funk10f1_.*|r38_werkstatt11b0|
  r39_stimmen_.*|r38_elzatoene_szene1021|r39_toene_szene.*|r36_elza_irons|r38_kleinkram_irons|r38_szenenschutz|
  r30_irons_tisch_.*|r30_cut_blitz|r34n_g_schrift1150|r35_inventar1050|r39_elzazombies_oben|r36_dachkiste|r36_klettern|
  r37_kiste|r37_kiste2feld|r37_kistenregel)$" -j 3` -> `98% tests passed, 1 tests failed out of 64`
  (`Total Test time (real) = 2987.37 sec`; rot nur integration_r39_dining1141_kampf, s. Querbezug R39 J), danach
  `-R "^integration_r39_dining1141_kampf$"` -> `100% tests passed, 0 tests failed out of 1`.
Nach dem Merge von master (370d8c31: Spur A + D integriert), neu gebaut:
- `ctest -R "^unit_r41_" -j 4 --timeout 600` -> `100% tests passed, 0 tests failed out of 16`.
- `ctest -R "^integration_r41_" -j 3 --timeout 1800` -> `100% tests passed, 0 tests failed out of 10`
  (`Total Test time (real) = 1071.69 sec`; Spur H 5 + Spur A integration_r41_wasser20b0(_npc) + Spur D 3 — der NPC-Schub
  vertraegt sich mit Spur A's NPC-Wasserschritten).
- `ctest -R "^integration_(r36_szenenleon|r35_cut10f0|r39_parkplatz11c0_k|r39_dining1141_kampf|r37_szene11c0|r36_dachkiste)$"
  -j 3 --timeout 2700` -> `100% tests passed, 0 tests failed out of 6` (`Total Test time (real) = 1441.11 sec`).

## OFFEN
1. **Entity-Durchlauf der Schubkette @0x8002bfec-c038** (FUN_8002cabc(Entitaet, obj, 1) fuer jede aktive Entitaet:
   Kiste wird aus einem Gegner/NPC herausgesetzt, Zaehler obj+0x8C = 10 `sb s7,140(s2)` @0x8002c030, KEINE
   Ruecksetzung) — nicht portiert (nicht Teil des Nutzer-Befunds; der Port schiebt umgekehrt die Aktoren aus den Kisten,
   re15_actor_prop_pushout @0x8002be0c-4c). Kopfkommentar in aot_common.c nennt ihn.
2. **Zaehler-Ueberlauf obj+0x8C**: das Original zaehlt als Byte (`lbu v0,140(s2)` / `addiu v0,v0,1` / `sb v0,140(s2)`
   @0x8002bf60-70) und laeuft nach 255 auf 0 -> bei dauerhaft gehaltenem UP alle 256 Bilder ein neuer Schubversuch;
   der Port saettigt bei 255 (aot_common.c `if (s_prop_push_count[p] < 255)`). Vorbestehend, nicht Teil des Befunds.
3. **Spiegel +0x42 (y)**: per Bild-Schnappschuss nach dem Objekt-Pass; abweichend vom Original nur fuer ein Objekt,
   dessen y sich im laufenden Pass VOR dem Schubtest aendert (im selben Bild fallende Nachbarkiste). In 2050 faellt nur
   obj 6 (Band 3 -> 2), die Raetsel-Kisten stehen in Band 0 -> kein ausgelieferter Fall.
4. **Yield-Zweige** von re15_npc_ai_tick (walk_active = Elliot 0x47 Altweg; SCD-besetzter Slot ausserhalb der
   Executor-Subs) kehren vor dem Schwanz zurueck — wie bisher auch vor der Wandklemme. Im Original laeuft der Schwanz
   (ausser bei den Freeze-Toren @0x8011cb90/@0x8011cbb0) jedes Bild.
5. **Querbezug fuer die Integration** (Memory reai-v2-gezielte-integration-querbezug): der Schub wirkt jetzt in JEDEM
   NPC-Tick, auch in Szenen. Spuren mit NPC-Szenen nach dem Merge nachfahren: G (2080 Marvin/John + 8 Zombies +
   Kanister: Marvin wird aus Zombies/Leon geschoben), J/K (STAGE3-Szenen 3060/3061/30C1/3091), A (NPC-Wasserschritte,
   RE15_ACTOR_MAX 36 — die Schleifen hier nutzen RE15_ACTOR_MAX). Ein Plc_dest-Ziel, das naeher als die Radien-Summe
   an einer anderen Figur liegt, wird nie erreicht (Ankunft 100/300 @actor_locomotion) — im Original genauso.
6. Die im Auftragstext genannten `integration_r37_kiste*` sind die KISTEN-(Item-Box-)Riegel der Runde 37, nicht das
   Schiebe-Raetsel; sie sind trotzdem im gezielten Lauf (unten).

## Fuer den Nutzer
- **Neue Assets:** keine (nichts unter shared_assets/RE2, keine Sprachdateien, keine Bits/Texte).
- **NPCs sind fest** (Ada, Marvin, John, Sherry, Irons, Annette, Katherine, Ben, ...): zwei NPCs laufen nicht mehr
  ineinander, auch nicht in Gegner; laeuft Leon/Elza in eine NPC hinein, weicht sie aus — wie im Original (Schub im
  Wurzel-Schwanz aller 28 NPC-Wurzeln, am Original gemessen: Ada/Marvin in ROOM3070 nie naeher als 901). Zum Ansehen:
  ROOM3070 (Lagerhalle, Ada + Marvin folgen Leon, nach dem G-Birkin-Kampf) und ROOM3001 (Elza, John + Sherry folgen).
  Folge, ebenfalls wie im Original: eine folgende NPC kommt nicht mehr DURCH Leon/Elza hindurch, sie bleibt vor ihr stehen.
- **Kisten in ROOM2050/2051 sperren sich gegenseitig** (am Original gemessen, Port = Original). Das Raetsel bleibt
  loesbar, braucht aber Genauigkeit: die drei Kerben an der Westwand liegen 1900 bzw. 1850 auseinander, die Kisten sind
  1800 tief. Erprobter Weg: (1) die mittlere Kiste gerade nach Westen in die suedliche Kerbe; (2) die suedoestliche Kiste
  nach Norden bis knapp hinter die Linie der Kerben-Mitte und dann nach Westen in die mittlere Kerbe; (3) die noerdliche
  Kiste nach Sueden nur bis zur Linie der noerdlichen Kerbe (schiebt man sie zu weit, stoesst sie beim Westschub an die
  mittlere Kiste) und dann nach Westen. Steckt eine Kiste fest, beruehrt sie eine andere.
- Entscheidungsfragen: keine.

## Nachbesserung 1 (2026-10-24, nach Abnahme H 0)
Ausgangslage: Abnahme H 0 (`H_abnahme_0.md`, 46b38db9) — beide Nutzer-Punkte **erfuellt**, "bestanden = NEIN" nur wegen der
formalen Vertragsabweichungen V1/V2. Orchestrator-Entscheid **H-E1** (AUFTRAG.md "2026-10-24 17:38", master 200184bc, vom
Orchestrator in diesen Zweig gespiegelt mit 91e19ffa): V1 und V2 **angenommen**; "Die Nachbesserung H 1 dokumentiert beides im
Dossier; sonst ist nichts zu tun." Deshalb: kein Code geaendert, keine Messung der Abnahme wiederholt. Neu ist nur ein Beleg zu
V2 (die aec4-Gates fuer das Paar NPC/Gorilla), den die Abnahme in RE-Gate 3 offen gelassen hatte ("Bewertung s. Mangelliste").
Arbeitsordner (Skripte, Logs): `scratchpad/H41nb1/`; Belege im Zweig: `H_belege/nb1_*`.

### V1 — Haken in enemy_ai_common.c (nicht in der Zuteilung VERTRAG §1.3)
- **Ursache:** VERTRAG §1.3 teilt Spur H re15_collision.c / game_step_common.c / aot_common.c zu. Die Original-Reihenfolge
  Zustand -> aec4(Spieler, NPC) -> b544 -> Wandklemme liegt IN der NPC-Wurzel (Ada STAGE1: `8011cc1c jalr v0`,
  `8011cc3c jal 0x8002aec4`, `8011cc44 jal 0x8002b544`, `8011cc64 jal 0x8003b0a4`; alle 28 Wurzeln, Abnahme RE-Gate 1). Das
  Port-Gegenstueck ist `re15_npc_ai_tick` (enemy_ai_common.c:10572); die Klemme `re15_npc_wall_clamp` ist `static` in derselben
  Datei (enemy_ai_common.c:10301) und klemmt gegen die Lage vom Tick-Anfang (`npc_ox/npc_oz`, Kern
  `re15_collision_constrain_contact_band` re15_collision.c:820). Ein Haken in einer H-Datei (game_step_common.c nach
  `re15_enemy_ai_run_all`) liefe NACH der Klemme und nach den Ticks aller spaeteren Aktoren; ein zweiter Klemmaufruf von dort
  waere eine im Original nicht vorhandene zweite Klemme. Es gibt keine Stelle ausserhalb enemy_ai_common.c, die die Folge
  @0x8011cc44 -> @0x8011cc64 einhaelt.
- **Messung vorher** (Stand 46b38db9): `git diff 7b59e297 HEAD -- re15_port/engine/src/enemy_ai_common.c` = 2 Zeilen:
  Z. 59 `#include "re15_kollision_r41.h"`, Z. 10710 `re15_kollision_r41_npc_schwanz(slot);` (zwischen `re15_npckante_altlage`
  und `re15_npc_wall_clamp`).
- **Beleg:** Adressen oben (Abnahme RE-Gate 1 selbst gescannt, 28/28). Konfliktprobe (nur rechnerisch, `git merge-tree
  --write-tree`, kein Checkout): r41/kollision gegen master -> sauber (exit 0); gegen r41/integration, r41/zombiefresser (Spur I,
  eigenes Include in enemy_ai_common.c Z. 61), r41/wasser20b0, r41/spinnenanim, r41/stage3szenen_a/_b -> enemy_ai_common.c
  OHNE Konflikt. Gemeldet werden nur die Doku-Abgleiche AUFTRAG.md/VERTRAG.md der jeweiligen Zweige und bei
  r41/stage3szenen_a enemy_common.c — die aendert Spur H nicht (`git diff 7b59e297 r41/kollision -- .../enemy_common.c` leer);
  derselbe Konflikt entsteht schon bei master gegen r41/stage3szenen_a (Spur A in master).
- **Aenderung:** keine (H-E1: "V1 angenommen").
- **Messung nachher:** Diff unveraendert (2 Zeilen); Riegel gruen (s. "Bau und gezielte Tests").

### V2 — fremder Riegel test_r39_dining1141_kampf.cmake (Runde 39 Spur J), WARTE 60 -> 240
- **Ursache:** Kriterium (a) des Riegels braucht eine Salve Johns IN einem Griff-Fenster eines Gorillas — eine Deckung der
  Zeitachse. Mit dem NPC-Wurzel-Schwanz schiebt b544 im Tick von John (0x49, STAGE1-Wurzel 0x8011dc68: `8011dd28 jal 0x8002aec4`
  / `8011dd30 jal 0x8002b544`) John aus dem Gorilla-Kasten (r 1600, `0x80121350 = {0,-1440,0,1600,1440,1600}`); die
  Gorilla-Wege und damit die Griff-Fenster verschieben sich.
- **Messung vorher:** Abnahme 5.2 (nicht wiederholt): alte Fassung (WARTE 60) mit der vor-exe gruen (2 Treffer im Griff), mit
  der nach-exe rot (0, nur (a)); neue Fassung (WARTE 240) mit der nach-exe gruen (Abnahme 5.4, `Passed 307.60 sec`).
- **Beleg NEU — ist "John wird aus dem Gorilla geschoben" Original?** FUN_8002aec4 prueft drei Bits im Wort +0x0 (selbst
  disassembliert): `8002af04 or v0,a0,v1 / 8002af08 andi v0,v0,0x2 / 8002af0c bne -> 0x8002af30 (return 0)` (Bit 2 an einem),
  `8002af14 and v0,a0,v1 / 8002af18 andi v0,v0,0x1000 / 8002af1c bne -> 0x8002b464 (return 0)` (Bit 0x1000 an beiden),
  `8002af24 andi v0,v1,0x4 / 8002af28 beq v0,zero,0x8002af38 (weiter) / 8002af30 j 0x8002b464 (return 0)` (Bit 4 am
  Geschobenen a1 = John). Wer setzt diese Bits?
  - Spawn Sce_em_set (PSX.EXE): `80042298 sw s3,0(s0)` mit s3 = 1 (`80042284` bzw. `8004228c ori s3,zero,0x1`), bei pc[3] != 0
    `800422ac ori v0,zero,0x2001 / 800422b0 sw v0,0(s0)`. ROOM1141 sub00 @0x00B06: John `44 00 49 40 ...` (pc[3] = 0x40),
    Gorillas @0x00B1A/@0x00B2E `44 01 27 30 ...` / `44 02 27 30 ...` (bzw. @0x00B54/@0x00B68 mit 0x10) -> Wort0 = 0x2001.
  - NPC-INIT 0x49 (0x8011dd98): `8011ddbc lw v0,0(a0) / 8011ddc0 lui v1,0x4000 / 8011ddc4 or v0,v0,v1 / 8011ddc8 sw v0,0(a0)`
    -> Wort0 |= 0x40000000.
  - Bit-Scan `H_belege/nb1_flagscan.py` (jedes `ori` mit Gate-Bit auf ein geladenes Wort mit Speicher auf +0/+1 innerhalb von
    4 Instruktionen, dazu die `and`-Loeschmasken; PSX.EXE + STAGE1..6.BIN; Ausgabe `H_belege/nb1_flagscan.txt`):
    Bit 0x2 nur in Sterbe-/Leichen-Zweigen, fuer den Gorilla nur `8011beac ori v0,v0,0x2 / 8011beb0 sw v0,0(v1)` in Zustand 7
    (Tabelle `0x801213c8[7] = 0x8011be54`; Port: enemy_ai_common.c Gorilla case 7 setzt `flags |= 0x02` @0x8011beac-b0,
    kollision_r41.c schliesst Zustand 7 aus). Bit 0x1000 nur in Griff-Paaren (Gorilla `8011ac34` setzen / `8011ad8c` loeschen),
    KEIN Schreiber im NPC-Bereich von STAGE1 (ab 0x8011b000 einziger Treffer ueberhaupt 0x8011beac). Bit 0x4: kein Schreiber im
    Spiel-Code (einzige Treffer PSX.EXE `8006b214 ori v1,v1,0x7` / `8006b2a0 ori v0,v0,0x504` = Bibliotheks-Schreibzugriffe
    0x10000007 / 0x20000504 ueber Zeiger, keine Entitaet). Grenze des Scans: Konstanten-Speicher (ori mit zero + sb auf +0)
    lassen sich keiner Entitaet zuordnen und sind nicht gewertet — dafuer die Laufzeitmessung unten.
  - ROOM1141-SCD (`re15_port/tools/scd_dump_room.py`): Member_set nur mit Id 0x04 (rot_y, @0x00C3A) und 0x0C (+0x9 = 0x10/0x30,
    @0x00E40..@0x010C2), keine Id 6 (= Wort +0x0, actor_common.c case 6).
  - **Laufzeit** (Auslieferungs-Savestate `stage_saves/mzd_stage1_maggot_heavy.sav`, `@0x80026e4c = 03e00008`, Stage/Raum
    0x800b0fe0/fe2 = 0/28 = ROOM11C0; `H_belege/nb1_savestate_flags.py`, Ausgabe `H_belege/nb1_savestate_flags_11c0.txt`):
    `E0 typ=0x42 Wort0=40000001 Gate-Bits=keine`, `E1 typ=0x27 Wort0=60000801 Gate-Bits=keine Zustand=1/6`,
    `E2 typ=0x27 Wort0=60000811 Gate-Bits=keine Zustand=1/6` — NPC und lebende Gorillas tragen keines der Gate-Bits.
  => Im Original schiebt b544 im NPC-Tick (`8011dd30` -> `8002b598 jal 0x8002aec4`) John aus jedem lebenden Gorilla, den er
  ueberlappt. Ausgenommen sind nur der Freeze (`8011dca4 andi v0,v0,0x20 / 8011dca8 bne -> 0x8011dd68` = ganzer Schwanz
  uebersprungen; 1141 setzt +0x9 = 0x30 per Member_set 0x0C; Port: re15_npc_ai_tick kehrt bei `grid_id & RE15_AI_GRID_SKIP`
  VOR dem Haken zurueck) und die Gorilla-Leiche (Bit 2; Port: Zustand 7 und flags 0x02). Die Verschiebung in 1141 ist
  Original-Verhalten; WARTE 60 hing an der alten Port-Abweichung (NPC nie geschoben).
- **Aenderung:** keine (H-E1: "V2 angenommen als belegter Querbezug").
- **Messung nachher:** `integration_r39_dining1141_kampf` (Fassung WARTE 240) im gezielten Lauf unten.

### Bau und gezielte Tests (Nachbesserung 1)
(laeuft)
