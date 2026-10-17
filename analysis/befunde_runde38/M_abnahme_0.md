# Runde 38 — Spur M "npckante": Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r38_npckante`, Zweig `r38/npckante`, HEAD aebc671a, Basis f587d34e (merge-base mit master).
Abnahme 2026-10-17. Grundlage: AUFTRAG.md (Wortlaut), VERTRAG.md, Dossier M_npckante.md, `git diff master...HEAD`.
Stand: IN ARBEIT (wird fortlaufend geschrieben).

## Punkte (Wortlaut)
1. "Kendo springt am Ende von ROOM 10B0 nicht die Kante runter, wie im Original, sondern läuft in die Luft weiter"
2. "Bei Room 1260 Springt Kendo auch nicht die Kanten runter. Machen Marvin und Ada in Leons Szenario übrigens auch immer nicht. Das ist falsch."

Lesart (VERTRAG §0 Spur M, AUFTRAG "Lesarten"): Kendo = John (Typ 0x49, Elza-Szenario), Raeume ROOM10B1 / ROOM1261;
Marvin (0x40) / Ada (0x42) im Leon-Szenario ROOM1260 (+ ROOM2000, Zensus des Bau-Agenten).

## 0. Code-Diff gelesen (git diff master...HEAD -- re15_port)

| Datei | Inhalt | Bewertung |
|---|---|---|
| engine/src/npckante_r38.c (neu, 385 Z.) | Sonde (FUN_8002d7d8), Kanten-Sub (0x80052508), Sprung-Sub (0x80052c14), Kanal-/Alte-Lage-/Schatten-Auskunft, Messhaken RE15_NPCKANTE_LOG | jede Phase gegen meine eigene Disassembly geprueft (s. 1.) |
| include/re15_npckante_r38.h (neu) | Konstanten, jede mit @0x | ok |
| engine/src/enemy_ai_common.c (+14/-1) | Include; Bank-Auskunft; Sonde in re15_npc_sub_walk (Sub 5 vor, Sub 4 nach re15_npc_anim); Dispatch 0x12/0x16/0x17/0x18; Schwerkraft re15_schwerkraft_8001bd60(e,-10,0x14,r) vor dem Dispatch; Yield-Gate; Alte Lage vor der Klemme; Messhaken | kleine Haken, Logik in eigener Datei |
| engine/src/scd_vm.c (+1) | Sce_em_set: `fall_1c0 = 0` fuer jeden Spawn | @0x800421f4 selbst geprueft |
| platform/pc/main.c (+2) | Include + NPC-Schatten auf +0x1ba waehrend Kante/Fall | PORT-WAHL gekennzeichnet (nur waehrend Kante/Fall) |
| tests/unit/{test_r38_npckante.c, probe_r38_npckante_karte.c, probes/r38_npckante.cmake} | 8 Riegel + Karten-Werkzeug | eigene Probe-Datei, keine Test-CMakeLists angefasst |

## 1. RE-Gate: selbst disassembliert (re15_disasm.py, info/Re1.5/PSX.EXE bzw. PSX/BIN/STAGE1.BIN)

Alle Stichproben enthalten genau das Behauptete (Sprungziele/Tabellen mit aufgeloest):

| Stelle | gelesen | Diff-Behauptung |
|---|---|---|
| Tabelle @0x80076ca0 | [4]=0x80051148 [5]=0x80051484 [18]=[22]=0x80052508 [23]=[24]=0x80052c14 | = Header/Dispatch (case 0x12/0x16/0x17/0x18) |
| Sub 4 @0x80051344-b4 | `jal 0x800245d8` -> `jal 0x8001f314` (+0x170/+0x174) @0x80051360 -> `jal 0x8002d7d8` @0x80051374 -> `sb v0,159(v1)` @0x80051388 -> `ori v1,zero,0x12` @0x80051394 / `sb v1,5(v0)` @0x800513a4 / `j 0x80051470` + `sb zero,6(v0)` @0x800513b4 | Sonde NACH anim_set, Ankunft uebersprungen = Port-Haken nach re15_npc_anim |
| Sub 5 @0x800516b0-0x80051764 | `jal 0x800245d8` -> `jal 0x8002d7d8` @0x800516c4 -> +0x9f @0x800516d8 -> 0x16 @0x800516e4/f4, `sb zero,6` @0x80051704; anim_set erst @0x8005171c; Ankunft `slti v0,v0,300` @0x80051764 | Sonde VOR anim_set = Port-Haken vor re15_npc_anim |
| XREF FUN_8002d7d8 | ghidra1_V2.txt:120680 `XREF[2]: 80051374(c), 800516c4(c)` | nur Sub 4/5 |
| Sonde @0x8002d7d8-da44 | Vektor @0x800109c8 = [800,0,0,0]; `lhu a0,106(s2)` / `addiu a0,a0,512` / `and a0,a0,-1024` @0x8002d82c-838; `jal 0x8003b7f0` (a1=0, a2=+0x82) @0x8002d874; Bit 0 -> Objektschleife 0x800b3f98 (Stride 148, Objektbox +6 >>1, Band+1 / Band-2) -> 1 @0x8002d944; sonst Entity-Box +6 (Band / Band-1); Bit 1 -> `jal 0x8001c6e8` (a1=1, a3=0x100) @0x8002da04, `slt v1,v1,v0` @0x8002da14 -> 2 | npckante_r38.c re15_npckante_sonde Zeile fuer Zeile gleich |
| Kanten-Sub @0x80052508 | Tabelle @0x800112e4, `sltiu v0,v1,0x5`; [0] 0x4b @0x80052550, +0x94=5 @0x80052570, +0x8f=7; [1] Schritt-SE 0x4000 @0x800525b4 (nicht portiert, s. OFFEN), anim_set(+0x170,+0x174) @0x800525f0, Kurs rasten @0x80052604-2c, `andi 0x3e0` @0x80052644, `lb +0x9f == 2` @0x80052660-68 -> 0x18/0x17 (Delay-Slot `ori v0,zero,0x17` @0x8005267c / `ori v0,zero,0x18` @0x80052684); [2] +0x94=2; [3] +0x95==0x11/0x1d/0x25, `ori v0,zero,0x47c` @0x80052710, `addiu v0,v0,-1800` @0x80052730 und @0x8005274c, `addiu v0,v0,1` @0x80052768, +0x40/+0x44/+0x42 = Lage @0x80052784-b4, anim_set(+0x84,+0x16c) @0x800527f8; [4] 0x12 ? 4 : 5 | sub_kante identisch |
| Sprung-Sub @0x80052c14 | Tabelle @0x8001131c, `sltiu v0,v1,0x7`; [0] +0x94=3; [1] anim_set dann `+0x95 == 0xd` @0x80052cbc-c4; [2] +0x93 or 1, +0x94=4, +0x8f=0, `sh zero,156` @0x80052d3c, `ori v0,zero,0x320` @0x80052d48, `addiu v0,v0,500` @0x80052d68, room_coll(&+0x34, 0, +0x82, 0x100) @0x80052d84, Kehrwert 0x91a2b3c5 @0x80052da4-dd0 = -(+0x1ba/1800); [3] y += 90 + 50*t @0x80052e34-5c, `addiu v0,v0,-800` / `slt` @0x80052e90-94; [4] y = +0x1ba @0x80052efc, +0x93 and 0xfe; leere Zaehlschleife @0x80052f28-4c; [6] 0x18 ? 4 : 5 | sub_sprung identisch |
| NPC-Wurzel 0x49 @0x8011dc68 (STAGE1.BIN) | Gate `g_pauseflags & 0x20000000` / `+0x9 & 0x20`; `ori a1,zero,0x14` @0x8011dcac, `addiu a0,zero,-10` @0x8011dcb4, `jal 0x8001bd60` @0x8011dcc4 VOR Dispatch `jalr` @0x8011dd08; Klemme @0x8011dd50; Schatten `lh a1,442` @0x8011dd78 + `jal 0x8001b064` | Schwerkraft-Haken vor dem Dispatch, Schatten auf +0x1ba |
| Byte-Suche `jal 0x8001bd60` | STAGE1 9 Treffer (6 NPC-Wurzeln 0x8011c5ec/cbbc/d1e4/d778/dcc4/e29c mit -10/20), STAGE2/4 je 4, STAGE3 5, STAGE5 6, STAGE6 5 mit -10/20 | "alle NPC-Wurzeln" belegt |
| FUN_8001bd60 | FUN_8003b7f0(&+0x34, -box+6, +0x82) & 2 @0x8001bda4-b4, y == -(Band*1800) @0x8001bdc8-ec, +0x1c0 = 0x8000 or n<<13, Fall-Schleife +0x1ba += 1800, y += a0 + a1*t @0x8001be78-a4, Landung @0x8001becc, `andi 0x7fff` @0x8001bee4 | vorhandener Port re15_schwerkraft_8001bd60 (trage_1200.c) stimmt |
| Sce_em_set | `sh zero,448(s0)` @0x800421f4, `sh v0,442(s0)` @0x80042210 | scd_vm.c `a->fall_1c0 = 0` |

Guess-Tells im Code-Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|approx`): keine. Einziger Env-Schalter:
`RE15_NPCKANTE_LOG` (reiner Messhaken, kein Verhalten). Einzige PORT-WAHL: NPC-Schatten nur waehrend Kante/Fall auf
+0x1ba (sonst y) — gekennzeichnet und begruendet (re15_npckante_r38.h).

Zitierfehler (nur Dossier, kein Code): Dossier "OFFEN" nennt `addiu s3,a0,-8` @0x8005175c; dort steht `jal 0x80065f60`
(SquareRoot0, Sub 5). Die Instruktion steht @0x8004575c (`addu s3,a0,zero` @0x80045738, FUN_80045630) — selbst
disassembliert. Der Fehler ist aus einem master-Kommentar (enemy_ai_common.c:9964) uebernommen. Siehe Maengel.

## 2. Riegel der Spur

`ctest --test-dir re15_port/build -R "r38_npckante" --timeout 600 --output-on-failure -j4` -> **100% tests passed, 0
tests failed out of 8** (clips, sonde, john1261, ada1260, john10b1, gegen, j2000, schwerkraft). Bau vorher:
`local_build.sh configure` + `build` -> `LOCAL-BUILD-OK`, "ninja: no work to do" (exe 08:26 juenger als letzte
Code-Aenderung 2faafed8 08:25; Baum sauber, `git status` leer). Ausgaben direkt gelesen (john1261: Sprung F159 Band 1->0,
Klettern F455; ada1260: Ada F62/F342, Marvin F127/F391; john10b1: 5->1 einmal, 4 Spruenge auf x -7460, 3 Stufen auf
x -6314, Fallkurve 31 Bilder 0 Abweichungen) — die Riegel messen tatsaechlich Bewegung/Band/Fallkurve, nicht nur Flags.

## 3. Messung an der exe (eigene Messkopien re15_pc_abn0m_<lauf>.exe, Lade-Weg CONTINUE + Tuer)

Werkzeug: eigenes Skript (scratchpad `abn/lauf.sh`): Karte mit `probe_r38_npckante_karte`, exe-Kopie unter eigenem
Namen, RE15_CONTINUE_TEST/CARD_AUTO, Tuer ueber `RE15_FIRE_AOT` (re15_aot_fire_slot = derselbe Pfad wie Hineinlaufen,
main.c:7953), Log `RE15_NPCKANTE_LOG`, Framedump `RE15_WINDOW_SCALE=1`. Auswertung mit EIGENEM Skript (`abn/auswert.py`,
unabhaengig von luft_auswertung.py): Luft A = Sub 4/5/6/9 und y < Boden (room_coll unter dem NPC), Luft B = y != -(Band*1800),
Luft C = jeder Sub ausser Sprung mit y < Boden - 50; geparkte Aktoren (-30000,*,-30000) ausgenommen.
Startlagen absichtlich ANDERS als die des Bau-Agenten; Flags wie der echte Ablauf sie setzt (selbst aus dem SCD gelesen).
Scratchpad = C:\Users\mjoedicke\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\abn

### 3.1 Punkt 2 (Teil Leon): ROOM1260 Ada + Marvin — ueber die Tuer aus ROOM11B0

Flags wie ROOM11B0 SCD @0x01478-80 sie setzt: (3,0x83) (4,0xCD) (4,0xF4). Tuer 5 @0x01034 -> ROOM1260 (26750,-1800,9400) Cut 7.
```
lauf.sh l1260 11B0 0 0 -20000 3072 0 3:0x83 4:0xCD 4:0xF4 -- RE15_FIRE_AOT="5@40#11B0" RE15_EXIT_AT="1200#1260" RE15_FRAMEDUMP="44-440/4:..."
debug.log: [save] CONTINUE: resumed in room 11b0 / [fire-aot] slot=5 at F40 (Raum 11B0) / [room] PC loaded room1260.rdt
  [npckante] ROOM1260 slot 1 Typ 42: Sonde 2 bei (26750,-1800,14500) Band 1 -> Sub 0x12
  [npckante] ROOM1260 slot 2 Typ 40: Sonde 2 bei (25400,-1800,14500) Band 1 -> Sub 0x12
  [npckante] ROOM1260 slot 1 Typ 42: Sonde 1 bei (-7489,0,16038) Band 0 -> Sub 0x12
  [npckante] ROOM1260 slot 2 Typ 40: Sonde 1 bei (-8820,0,15980) Band 0 -> Sub 0x12
auswert.py: Ada  F54 Sub 0x18, F68 Band 1->0 (y -1210), F92 Sub 4 unten; F317 Kante, F348 Band 0->1, Ende (-7502,-1800,-6005) Band 1 | Luft A 0, B 0, C 0
            Marvin F119 Sub 0x18, F133 Band 1->0, F157 Sub 4; F366 Kante, F397 Band 0->1, Ende (-8901,-1800,-4453) Band 1 | Luft A 0, B 0, C 0
```
Bild (`abn/l1260_sprung.png`, `l1260_marvin.png`): Ada kommt auf die Kamera zu, Hocke F60-F64, Absprung F68, ab F72 nur
noch der Kopf am unteren Bildrand (im Kanal). Marvin dasselbe F128-F140.
**Gegenprobe v0.8.25** (release/pkg-win/re15_port_v0.8.25/re15_pc.exe, GLEICHE Karte, gleicher Weg; `abn/v1260_sprung.png`):
Ada geht in Steghoehe weiter auf die Kamera zu, keine Hocke, kein Absprung, F84-F92 ganz vorn im Bild in Steghoehe
= "in die Luft". Der Befund des Nutzers ist am alten Stand reproduziert und am neuen Stand weg.

### 3.2 Punkt 2 (Leon, "immer"): ROOM2000 Ada + Marvin — ueber die Tuer aus ROOM1260

```
lauf.sh l2000 1260 -9000 -1800 -8000 1024 0 4:0xF4 -- RE15_FIRE_AOT="1@40#1260" RE15_EXIT_AT="2400#2000" RE15_PAD_AT="60:A,85:A,...,2385:A" RE15_FRAMEDUMP="1500-1800/5:..."
debug.log: CONTINUE resumed in room 1260 / [fire-aot] slot=1 at F40 / PC loaded room2000.rdt
  Ada F1517 Sonde 2 (-5558,-1800,-20854) -> F1526 Sub 0x18, F1540 Band 1->0, F1605 Sonde 1 -> F1644 Band 0->1
  Marvin F1642 Sonde 2 (-4321,-1800,-20813) -> F1652 Sub 0x18, F1666 Band 1->0, F1737 Sonde 1 -> F1777 Band 0->1
```
Bild `abn/l2000_sprung.png`: Ada geht an die Stegkante (F1520-1535), springt in den Kanal (F1540-1550), Hocke/Klettern am
Gegenufer F1630-F1645, steht F1660 oben; Marvin springt F1665-F1670. Waehrend aller Kanten-Wege Luft A/B/C = 0.
Beobachtung (kein Mangel dieser Spur): nach dem Abgang setzt das ORIGINAL-SCD beide per Pos_set auf Band-1-Hoehe ohne
Boden darunter — Marvin sub02 @0x01A62 `32 00 60 09 F8 F8 48 0D` = (2400,-1800,3400), Ada sub03 @0x01AC6 `32 00 F4 1A F8 F8
48 0D` = (6900,-1800,3400); dort liegt keine SCA-Zelle (selbst geprueft), FUN_8001bd60 loest ohne Bit-1-Zelle nicht aus
(@0x8001bdb0-b4) — Original-Parklage, in meiner Auswertung ab F1926/F1996 als "Luft A" sichtbar, kein Kanten-Weg.

### 3.3 Punkt 2 (Teil Elza/Kendo): ROOM1261 John — ueber die Tuer aus ROOM10B1

Flags wie ROOM10B1 sub04 @0x0195A-62 sie setzt: (3,0x83) (3,0x41). Tuer 1 @0x016F2 -> ROOM1261 (-20250,-1800,-6700) Cut 15.
```
lauf.sh l1261 10B1 -21500 0 2800 3072 7 3:0x83 3:0x41 -- RE15_FIRE_AOT="1@40#10B1" RE15_EXIT_AT="1300#1261" RE15_FRAMEDUMP="130-200/3:..."
debug.log: CONTINUE resumed in room 10b1 / [fire-aot] slot=1 at F40 / PC loaded room1261.rdt
  [npckante] ROOM1261 slot 1 Typ 49: Sonde 2 bei (-20477,-1800,-17374) Band 1 -> Sub 0x12
  [npckante] ROOM1261 slot 1 Typ 49: Sonde 1 bei (20057,0,-18847) Band 0 -> Sub 0x12
auswert.py: John F151 Sub 0x18, F165 Band 1->0, F189 Sub 4 unten; F430 Kante, F461 Band 0->1; Ende F1300 (20885,-1800,-5250) Band 1 | Luft A 0, B 0, C 0
```
Bild `abn/l1261_sprung.png`: John geht auf die Kamera zu, Rast F148-F157, Hocke F160-F163, Absprung F166, ab F169 im Kanal.
**Gegenprobe v0.8.25** (gleiche Karte, gleicher Weg; exe-Kopie im Bau-Ordner des Spurbaums, `abn/v1261_sprung.png`): John geht in
Steghoehe ungebremst weiter auf die Kamera zu (F148-F175), keine Hocke, kein Sprung = Nutzer-Befund reproduziert; neu: Sprung F166.

### 3.4 Punkt 1: ROOM10B1 John am Stegende — ueber die Tuer aus ROOM1171 und zu Fuss bis Kameraschnitt 2

Echter Ablauf (selbst aus dem SCD gelesen): ROOM1171 sub02 setzt @0x01398-a4 (3,0xC2)=0, (3,0x80)=1, (4,0xE9)=1, (3,0x82)=1;
ROOM1171 Tuer 2 @0x0126C fuehrt nach ROOM10B1 (16490,-9000,-8110) Band 5 Cut 0. ROOM10B1 sub00 @0x0179E Ck(3,0x80) setzt John
auf (15600,-9000,-9884) und startet sub02 @0x0182E (Laeufe ueber den Steg, `While(Cmp(0x0A != 2))` @0x01888, dann Plc_dest 5 nach
(-6694,-7709) @0x01896). Kamerazonen (RVD @RDT+0x28, selbst gelesen): Cut 0 -> 1 bei x ~6400..14500, Cut 1 -> 2 bei x < ~2400 (z -11000).
Elza wird per Eingabeskript den Steg entlang nach Westen gefuehrt (kein Sprung, kein Teleport):
```
lauf.sh l10b1 1171 5600 -7200 -7000 0 0 3:0x80 3:0x82 4:0xE9 -- RE15_FIRE_AOT="2@40#1171" RE15_EXIT_AT="700#10B1"
        RE15_INPUT_SCRIPT="R0.3,XU0.65,R0.233,XU3.4" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_STATE_LOG=...
state.log: F109 PL(16379,-8276) cam 0 ... F154 PL(11361,-11259) cam 1 ... F199 PL(2406,-10854) cam 2 ... F244 PL(-2684,-10512) cam 2 (steht)
debug.log: [fire-aot] slot=2 at F40 (Raum 1171) / [room] PC loaded room10b1.rdt
  [npckante] ROOM10B1 slot 1 Typ 49: Sonde 2 bei (-3682,-9000,-8344) Band 5 -> Sub 0x16      (Original: (-3676,-9000,-8352))
auswert.py: F205 Sub 5 -> 0x16, F211 -> 0x17, F225 Band 5 -> 1 (y -8410), F260 Sub 5 auf Block [6] (y -1800)
            danach F271/F411/F555 Sprung (Band 1 -> 0 auf x -7460), F337/F478/F621 Klettern (Band 0 -> 1 auf x -6314)
auswert2.py: D (Mittelpunkt in Absprungzelle des eigenen Bandes) 0, B (y != Bandhoehe) 0
```
Bild (Wiederholungslauf `l10b1f`, gleiche Eingabe, `abn/l10b1_sprung.png`): Cut 2 — John am Stegende (F204), Hocke F210-F222, Sprung ueber
die Bruestung F225-F231, faellt aus dem Bild (F237 klein unten links); Elza laeuft hinten den Steg entlang.
**Gegenprobe v0.8.25** (identische Karte + identisches Eingabeskript, gleiche Spielerlagen F109..F214 im state.log; `abn/v10b1_sprung.png`):
John laeuft F204-F210 in Steghoehe ueber die Bruestung hinaus und links aus dem Bild — keine Hocke, kein Sprung = "laeuft in die Luft
weiter" (Wortlaut Punkt 1) reproduziert; neu: Hocke + Sprung + Fall.
Hinweis zur Auswertung A: John auf dem Steg (Band 5) zaehlt dort als "Luft A" (199 Bilder F6-F204, alle Band 5), weil room_coll unter
dem Steg keinen Block findet — der Steg ist Band-5-Boden ohne Block; Kriterium D/B (Zellen aus der RDT) ist das gueltige Mass.
Kriterium D ist an den VORHER-Logs des Bau-Agenten gegengeprueft (es findet den Befund): vorher_1261 249, vorher_1260 Ada 48 /
Marvin 45, vorher_2000 Ada 54 / Marvin 61, vorher_10b1 10 (danach Band 5 ueber Block [6], dort greift A) — nachher ueberall 0.

Der Wechsel Sprung/Klettern am Westrand von Block [6] (x -7460 / -6314) tritt auch in meinem Lauf auf, mit genau den Lagen der
DuckStation-Messung des Bau-Agenten (orig_10b1_john.log: Landung x -7460 t=4.12, Stufe x -6314 t=7.47, 12 Zyklen). Mechanismus selbst
nachgerechnet: Ziel (-6694,-7709) (sub02 @0x01896 `40 00 05 21 DA E5 E3 E1`) liegt 298 vom Westrand von Block [6] (x -6992, Zelle [6]
selbst gelesen); Sonde 800 voraus erreicht Zelle [18] (x < -7162, Band 1, u1 0x02) ab John.x < -6362, Ankunft verlangt < 300
(`slti v0,v0,300` @0x80051764), die Sonde laeuft aber VOR dem Ankunftstest (@0x800516c4) -> Sprung; unten liegt das Ziel oestlich,
Sonde trifft Block [6] (u1 0x01) -> Klettern (+0x47c = 1148 -> x -6314, oestlich des Ziels) -> wieder nach Westen. Ende erst mit Cut 8
(`While(Cmp(0x0A != 8))` @0x0189E -> sub03 Pos_set @0x018B6). Aus Cut 2 nicht sichtbar (Block [6] liegt unter dem Bildausschnitt,
F246/F258 ohne John). Original-Verhalten, kein Mangel.

## 4. Nebenwirkungen (beide Szenarien, alle NPC-Raeume)

Der Diff wirkt auf JEDE NPC 0x40..0x4d (Schwerkraft in der Wurzel, Sonde in Sub 4/5, Dispatch, Sce_em_set +0x1c0, Schatten).
- Unit/Probe, eigene breitere Auswahl (179 Tests: alle Namen mit npc|marvin|ada|elliot|elza|irons|sherry|john|roy|szene|1170|10d0|
  10b|11b0|126|1150|1141|1171|1011|1021|2000|schwer|trage|schatten|plc|walk|motion|spawn|neck|turn|dog|hund|crow|zombie|1090 ...,
  Ausdruck in scratchpad `abn/unit_regex.txt`, -j4): **100% tests passed, 0 tests failed out of 179** (Obermenge der 82 des Bau-Agenten,
  u.a. alle unit_1090_* (Kistenraum mit Absprungzellen), unit_elliot_1170_run, unit_r37_roy_leon1170, unit_r36_szenenleon_*).
- Integration seriell (16, `abn/integ_regex.txt`): ERGEBNIS UNTEN (Abschnitt 4a).
- Eigener Weg-Zensus `abn/fall_weg_zensus.py` (alle 34 RDTs mit Absprungzellen; jede Original-SCD-Plc_dest-Bahn Modus 4/5 mit Sonde und
  7/8 ohne Sonde einer NPC 0x40..0x4d; Kriterium FUN_8001bd60 Zellwort(&pos,-450,Band) & 2 und y == Bandhoehe, bevor die Sonde greift):
  **Treffer: 0**. Zusammen mit dem Stell-Zensus des Bau-Agenten (schwerkraft_zensus.py, selbst ausgefuehrt: **0**) faellt keine NPC
  einer Original-Szene durch die neue Schwerkraft, ausser ueber die Kanten-Subs.
- Byte-Suche `jal 0x8002d7d8` (`f6 b5 00 0c`) ueber PSX.EXE + alle STAGE/DEBUG/TITLE.BIN: nur 0x80051374 und 0x800516c4 — die Eskorte
  (State 1) und die Rueckwaerts-Subs rufen die Sonde auch im Original nicht; kein weiterer Original-Pfad fehlt im Port.
- Kanten-Zensus des Bau-Agenten selbst ausgefuehrt (kanten_zensus.py): Treffer nur ROOM10B1 (John sub2), ROOM1260 (Ada sub4, Marvin sub5),
  ROOM1261 (John sub4), ROOM2000 (Marvin sub2, Ada sub3) = genau die gemessenen Faelle; Leon-ROOM10B0 hat keinen NPC-Kantenweg.
- v0.8.25 -> Basis f587d34e: `git diff --stat 2077bb8e f587d34e -- re15_port/engine re15_port/platform re15_port/include` leer — die
  v0.8.25-exe ist die richtige Vorher-Referenz.

## 5. Vertrag (VERTRAG.md)

- Pfade (`git diff master...HEAD --name-only`): analysis/befunde_runde38/M_* + re15_port/{engine/src/enemy_ai_common.c, engine/src/npckante_r38.c,
  engine/src/scd_vm.c, include/re15_npckante_r38.h, platform/pc/main.c, tests/unit/{probe_r38_npckante_karte.c, probes/r38_npckante.cmake,
  test_r38_npckante.c}}. Kein release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists (probes/*.cmake wird per GLOB
  tests/unit/CMakeLists.txt:4060 eingesammelt). Gemeinsame Dateien nur Haken: enemy_ai_common.c +14/-1 (NPC-Code = Spur M laut §1.4),
  scd_vm.c +1, main.c +2, je mit "Runde 38 Spur M".
- Bank-9-Bits / Nachrichten-Ids / Items / Slots: keine belegt (`git diff` ohne flag_set/MSG_ID/Message/Se_on).
- Riegel je Punkt vorhanden und messend: john10b1 (Punkt 1), john1261 + ada1260 + j2000 (Punkt 2), dazu sonde/clips/gegen/schwerkraft.
  Ohne die Haken waeren seen_sprung/sprung10/hoch01 = 0 -> rot (Code gelesen; die build_bisect-Gegenprobe des Bau-Agenten liegt nicht
  mehr vor, die v0.8.25-exe-Gegenprobe oben zeigt den Befund am alten Stand).
- Integration: neue Quelldatei npckante_r38.c (GLOB; Android-Configure-Cache erneuern, s. Dossier); Include-Block enemy_ai_common.c
  teilt Spur B.

