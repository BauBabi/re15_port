# Abnahme 0 - Spur kartenbits (Bank-9-Kollision der Lageplaene), 2026-10-12

Zweig `r36/kartenbits`, Baum `.claude/worktrees/r36_kartenbits`, geprueft auf Stand **f31e28b2** (Basis r36/integration
7a0fd453). Unabhaengige Abnahme: eigene Disassembly, eigener Zensus mit eigenen Skripten (nicht `bank9_zensus.py` /
`bank9_zensus.txt` uebernommen, sondern danach dagegen verglichen), eigener sauberer Neubau, eigene Testlaeufe, eigene
Gegenprobe am Baum.

**Ergebnis: BESTANDEN.** Alle sechs Punkte erfuellt, keine Maengel. Vier Hinweise ohne Einfluss auf das Ergebnis stehen
in Abschnitt 8.

---

## 1. RE-Zitate selbst disassembliert (`re15_disasm.py` auf info/Re1.5/PSX.EXE)

| Behauptung | eigener Beleg |
|---|---|
| Opcode-Tabelle | `table 0x800744a8`: [0x21] -> 0x8003fcf4 (Ck), [0x22] -> 0x8003fdd0 (Set), [0x2C] -> 0x80040534 (Aot_set), [0x46] -> 0x80040738 (Aot_reset), [0x50] -> 0x80040644 (Item_aot_set), [0x58] -> 0x8003fd54, [0x59] -> 0x8003fe90 |
| Flag-Tabelle | `read 0x80074664 14 --w 4`: [9] @0x80074688 = **0x800b1078**, [10] = 0x800b1098 -> Bank 9 = 32 Byte = 256 Bit |
| Item_aot_set, Taken-Test | `80040654 lw a2,28(a0)`; `8004065c lbu v0,3(a2)` + `80040664 andi v0,v0,0x80` (Langform); `80040670 lhu a1,26(a2)`; `80040680 lhu a1,18(a2)`; `800406d4 lui a0,0x8007` / `800406d8 lw a0,18056(a0)` (= *(0x80074688)); `800406dc jal 0x8004efe4`; `800406e4 beq v0,zero,0x8004071c`; `800406f4 sb zero,0(v0)` (Satz still); `800406f8..80040718` Modell-Eintrag `0x800b3f98 + s1*0x94` = 0x80000000 |
| Bit-Helfer | FUN_8004efe4 (Test): `srl v1,a1,5 / sll v1,v1,2 / addu v1,v1,a0 / andi a1,a1,0x1f / lui v0,0x8000 / srlv / and`; FUN_8004ef90 setzt (`or`), FUN_8004efb8 loescht (`nor` + `and`) |
| Ck 0x21 | `8003fcfc lbu a2,1(v0)` Bank, `8003fd00 lhu a1,2(v0)` Bit (Wort `(a1>>3)&0x1c`, Maske `a1&0x1f`), Tabelle `lw v0,0(at)` @0x8003fd2c |
| Set 0x22 | `8003fdd8/dc/e0` Bank/Bit/Op, Tabelle `lw v1,0(at)` @0x8003fe04; Op 0 -> @0x8003fe4c, 1 -> @0x8003fe64, 7 -> @0x8003fe78 |
| 0x58 / 0x59 | Bit aus der Arbeitsvariablen 0x800b0fd0[pc[2]] (`lhu v1,0(at)` @0x8003fd7c / `lhu v0,0(at)` @0x8003feb8), Tabelle @0x8003fda0 / @0x8003fed0 |
| FLAG_CHG (AOT-Typ 4) | `table 0x8007469c`: [4] -> 0x80043120; `80043120 lhu v1,0(a0)` Bank, `80043124 lhu a1,2(a0)` Bit, `80043128 lhu a0,4(a0)` Wert, Tabelle @0x80043140. Der Dispatcher uebergibt a0 = Slot+12 (`8004300c addiu a0,s0,12`) bzw. Slot+20 (`80042f90 addiu a0,s0,20`, wenn Slot[1]&0x80); Slot = pc+2 -> Daten ab pc[14] bzw. pc[22] |
| Aot_reset 0x46 | `80040738..800407b0`: Slot pc[1], Sce pc[2] (`sb v0,0(a1)` @0x80040764), Daten pc[4]/pc[6]/pc[8] -> Slot+12 bzw. +20 |
| Cmp 0x23 (der Auftrag nannte "Ck/Set/Cmp") | 0x8003ff68: vergleicht die Arbeitsvariable 0x800b0fd0[pc[2]] mit pc[4..5], Operator pc[3] ueber 0x80010c3c - **keine Flag-Bank** |
| Aufnahme-Typ | AOT-Typ 9 (0x80043328) schreibt die Item-Nummer nach 0x800afbb6 (`sb v0,-1098(at)` @0x8004335c); die Aufnahme liest sie @0x8001e0a4 |

Alles, was Dossier 9.1, die Code-Kommentare (re15_kartenitems_r36.h, karten_export.py, test_r36_kartenbits.c,
r36_kartenbits.cmake) und die Commit-Messages zitieren, stimmt mit diesen Bytes ueberein.

**Wer im Original Bank 9 sonst anfasst - eigene Scans ueber EXE + STAGE1..6 + DEBUG + TITLE.BIN:**
* lui/Offset-Scan auf die Tabelle 0x80074664..0x8007469f: berechneter Index nur @0x8003fd24 (Ck), @0x8003fd98 (0x58),
  @0x8003fdfc (Set), @0x8003fec8 (0x59), @0x80043138 (FLAG_CHG); fester Index [9] nur @0x800406d8 (Item_aot_set). Kein
  Overlay greift auf die Tabelle zu.
* Vorwaerts-Konstantenverfolgung je Funktion auf 0x800b1078..0x800b1097: EXE @0x8001e0d4 (Aufnahme, `addiu a0,s0,162`
  mit s0 = 0x800b0fd6), @0x80021fd4 (Aufnahme, `addiu a0,a0,4216`), @0x8003e50c/@0x8003e538 (FUN_8003e4f4, Hinweis H3),
  @0x800406d8. Overlays: 0 Treffer.
* Alle 105 Aufrufe der Bit-Helfer (EXE 16, STAGE1 22, STAGE2 11, STAGE3 21, STAGE4 15, STAGE5 20) mit aufgeloestem a0:
  Bank 9 nur bei `jal 0x8004ef90` @0x8001e0d0 und @0x80021fd8 (Bit = `lhu a1,4(s)` @0x8001e0cc / @0x80021fcc mit
  s = Slot+12/+20, also pc[18]/pc[26] des Satzes) und bei `jal 0x8004efe4` @0x800406dc. Alle anderen treffen Bank 1, 5,
  7 oder 8: 44 Overlay-Stellen mit Basis 0x800b0fe0 und den Zweigen +88 (Bank 7) / +120 (Bank 8); die s0/s1-relativen
  Faelle `+0x45ce`, `+0x48a4`, `+0x45a0` ergeben 0x800b1028 (Bank 5), `+0x48d4` ergibt 0x800b1058 (Bank 8).

=> Ein Original-Bank-9-Bit entsteht nur aus Item_aot_set (Test und Aufnahme), aus AOTs mit Sce 9 (Aufnahme), aus Ck/Set
mit Bank 9, aus FLAG_CHG mit Bank 9 und aus 0x58/0x59 mit Bank 9. Das deckt sich mit Dossier 9.3 und der Zensus-Datei
(zu Sce 9 ausserhalb von Item_aot_set siehe H1).

## 2. P1 - Messen vorher: ERFUELLT

* Die vier Saetze selbst aus dem Auslieferungsbaum gelesen (info/Re1.5/PSX; alle 240 RDTs dort sind `cmp`-gleich zu
  shared_assets/PSX):
  `ROOM2040/2041 @0x1364: 50 07 09 31 03 00 a4 06 62 c6 e8 03 e8 03 32 00 01 00 5b 00 ff 00` (Item 0x32, Bit 0x5b = 91),
  `ROOM2090/2091 @0xA12: 50 05 09 31 01 00 e6 14 c4 be e8 03 e8 03 3f 00 01 00 5a 00 00 00` (Item 0x3F, Bit 0x5a = 90).
* Die Kollision an der Engine selbst reproduziert (Gegenprobe Abschnitt 5, Stand mit 90/91 gebaut und gefahren):
  `Plan Blatt 1 (ROOM1200) ... setzte (9,90)` -> `ROOM2090/2091 Slot 5 Item 0x3F ... STILL - das Item ist weg`;
  `Plan Blatt 0 (ROOM11D0) ... (9,91)` -> `ROOM2040/2041 Slot 7 Item 0x32 ... STILL`. Rueckrichtung: Items genommen ->
  `Besitz der Polizei-Blaetter 0..4 = 0x03`, die Plaene in ROOM1200/11D0 werden nicht angelegt.
* Die Messung kann den Fehler sehen: Teil V2 des Riegels meldet fuer alle vier Raeume "frisch scharf, mit Original-Bit
  still".
* Dossier 9.2 enthaelt Ergebnis und Adressen (@0x80040680, @0x800406dc, @0x800406f4, Satz-Offsets @0x01364/@0x00A12).
* Voraussetzung des Befunds geprueft, dass Bank 9 Raum- und Stage-Wechsel ueberlebt: geloescht wird sie nur in
  FUN_8003e4f4, dessen einziger Aufrufer FUN_8001d22c @0x8001d570 ist (Spielmodul-Init bei NEW GAME/LOAD; Raumwechsel
  laufen ueber FUN_8001d600, Port-Beleg re15_entladen.h). Der Port fuehrt Bank 9 in g_game.flags ueber Raeume hinweg.

## 3. P2 - Zensus: ERFUELLT

**Eigener Zensus des Originals.** Eigenes Skript; die Laengen liest es aus `s_opcode_sizes` in scd_vm.c. Abgleich mit der
Tabelle in scd_dump_room.py: fuer 0x00..0x5E gleich, ausser 0x2D, das der Motor dynamisch zu 34 aufloest. Regionen- und
Walk-Code sind selbst geschrieben.
* 240 RDTs (120 Leon xxx0, 120 Elza xxx1): 206 mit Kopf + 34 Stummel; 1579 Regionen, **40674 Opcodes**, 0 Desync vor
  Evt_end.
* **164 Item_aot_set** (162 mit Sce 9, 2 mit Sce 0: ROOM1190/1191 sub13 @0x02A30, Item 0x37, Bit 136), **54 Ck(9,n)**,
  **1 Set(9,21)**, 0 mal 0x58/0x59 mit Bank 9, 26 FLAG_CHG-Saetze (24 Aot_set + 2 Aot_reset), **alle Bank 5**,
  8 Aot_reset mit Sce 9 (Bits 14/15/16/136, siehe H1). Kein Taken-Bit >= 256 (Maximum 253).
* **85 von 256 Bits**: 2 6 10 11 12 14 15 16 18 19 20 21 22 52 85 86 90 91 92 98 99 104 105 108 109 134 135 136 138
  140..144 146..155 162..169 180..198 216 217 218 223..231 252 253.
* Bekannte Bits einzeln an den Bytes belegt: 85 = ROOM3040 @0x14B2 `50 09 09 31 .. 36 00 01 00 55 00` (Item 0x36),
  86 = ROOM4040 @0xF66 `.. 41 00 01 00 56 00` (Item 0x41), 92 = ROOM3050 @0x1EC6 `.. 07 00 06 00 5c 00` (Item 0x07, 6
  Stueck). Namensanfaenge aus inv_name_bank.inc: 0x07 "Super", 0x32 "Head", 0x36 "Green", 0x3F "Pocket", 0x41 "Spark".
* **Obermenge ohne Synchronisation** (jedes Byte der SCD-Sektionen: Item_aot_set mit jeder Sce und Slot < 48; Aot_set,
  Door_aot_set und Aot_reset mit Sce 4 oder 9; Ck/Set mit Bank 9): fuer **keines** der Port-Bits 53..63, 65, 71..84,
  87..89 und 93 ein echter Satz. Die wenigen Roh-Kandidaten (Bits 65, 71, 75, 78, 79, 80) liegen mitten in anderen
  Befehlen (Pos_set, Sleep) oder in Meldungstext (Bytes 0x3d..0x55). Fuer 90/91 liefert die Obermenge genau die vier
  bekannten Saetze.
* Die eingecheckte Datei `N_belege/bank9_zensus.txt` stimmt damit ueberein: gleicher Satz, ihre FREI-Liste ist genau das
  Komplement meines Satzes (171 Bits), und `bank9_zensus.py` reproduziert ihren Rohteil (ab Zeile 72) bytegleich (nach
  CR-Normalisierung).

**Eigener Zensus des Ports** (grep in include/, engine/src, platform): `re15_game_flag_*(9, ..)`, alle
`#define *BANK* 9`, alle Bit-Makros, SCD-Bytefolgen `0x21/0x22, 9|BANK, ..` in .c/.inc, synthetische Item-AOTs
(`re15_aot_set_item_tk_prop`, `re15_item_modal_start`, `re15_menu_request_doc`). Bank 9 = {53, 54, 55, 56, 57..60, 61,
62, 63, 65, 71, 72, 73, 74, 75, 76, 79, 80, 81, 82, 83, 84, 93} plus die Lageplaene {87, 88, 89, 77, 78}. Nicht Bank 9
(geprueft): RE15_CUT10F0_ZIEL1 (4,64), RE15_IT_IRONS und RE15_TUER1060_IRONS (3,94), RE15_R36Q_ZELLE (4,0xEC),
RE15_R36Q_STROM (4,0xF3), RE15_IT_BIT_1140_0/1070_0/1040_0 (Zone 7). Die uebrigen `0x22,0x09`-Treffer in
dokumente_props.inc, irons_tisch_props.inc, r35_schrot_prop.inc und re15_inv_ui_tables.c sind Modell- und Texturdaten.
Das Ergebnis ist gleich der Liste k_port[] + k_r36k_lage im Riegel.

**Schnitt Original x Port: vorher {90, 91}, nachher leer.** Die VERTRAG-Reserven 85/86/92 sind Original-Bits.

## 4. P3 - Fix: ERFUELLT

* Diff 7a0fd453..f31e28b2 gelesen. In `gen/kartenitems_r36.inc` aendern sich genau zwei Zeilen, nur Feld 3
  (`0x1200, 1, 90` -> `77`, `0x11D0, 0, 91` -> `78`).
* **Generator selbst gefahren** (Modul geladen, `ZIEL` in den Scratchpad umgelenkt, `--schreiben`): die Ausgabe ist
  bytegleich zum HEAD-Blob (md5 f00a12912d8a176049f7e58e0d15ce7a; im Arbeitsbaum liegt die Datei per autocrlf als CRLF).
* 77/78 sind frei: im Original (Zensus und Obermenge oben) und im Port (Zensus oben, Historie in Abschnitt 6); sie sind
  auch kein anderes Port-Bit.
* Alle verlangten Stellen sind geaendert: karten_export.py (RAEUME und Begruendung mit @0x80040680/@0x800406dc/
  @0x80074688, @0x00A12/@0x01364), re15_kartenitems_r36.h (Absatz BANK-9-BITS, Feld `bit`, Haken-Kommentar),
  re15_map_owned.c, test_r36_kartenitems.c (bits[], Teil S mit 87/78), test_r36_kartenitems_bild.cmake (Feld 7 von b2/b1,
  Kopf), VERTRAG.md 1.1 (N-Zeile 87-89/77/78, Vermerk 85/86/90/91/92 = Original-Taken-Bits, Liste "frei in Original UND
  Port", rechnerisch gleich Komplement(Original) minus Port), Dossier Abschnitt 9. `git grep` findet keinen veralteten
  Verweis auf 87..91 oder (9,90)/(9,91) ausser der beabsichtigten Gegenprobe.
* Die Commit-Messages (84d17bf8, f31e28b2) tragen die @0x-Adressen und Datei-Offsets.

## 5. P4 - Riegel: ERFUELLT

* `tests/unit/probes/r36_kartenbits.cmake` wird ueber `file(GLOB .../probes/*.cmake)` (tests/unit/CMakeLists.txt:4060)
  erfasst; ctest kennt `unit_r36_kartenbits`. scd_vm.c bleibt unveraendert, der Test benutzt das schon exportierte
  `scd_opcode_size_at`.
* (a) Zensus aller RDTs (Teil Z): 206 / 1579 / 40674 / 0 Desync / 164 / 85, dieselben Zahlen wie mein eigener Zensus.
  (b) Er findet 85, 86, 90, 91, 92 (dazu 52, 136, 138, 21). (c) Explizite Liste von 30 Port-Eintraegen mit Makro und
  Quelle plus die 5 Lageplan-Bits live aus k_r36k_lage: 0 Kollisionen. (d) Interne Gegenprobe: die alte Belegung ergibt
  genau {90, 91}. Verhaltens-Pin (Teil V): jeder Plan wird ueber das echte Port-Programm genommen (Ereignis 28, Ja), die
  vier Stage-2-Saetze bleiben danach scharf; Rueckrichtung Besitz 0x00, die Plaene haengen.
* Eigener Lauf nach sauberem Neubau: `ctest -R "^unit_r36_kartenbits$"` -> `100% tests passed, 0 tests failed out of 1`;
  das Programm direkt gefahren: **141 ok, 0 FAIL, exit 0**.
* **Gegenprobe selbst am Baum nachgestellt:** `kartenitems_r36.inc` gesichert (md5 8dbaa1e856cba8503e4aca862191c03a),
  B2/B1 auf 90/91 gesetzt, `local_build.sh build` (LOCAL-BUILD-OK, kartenitems_r36.c neu uebersetzt), Riegel gefahren:
  **exit 1, 13 FAIL**. Teil P: `Bit 90 ... KOLLISION mit ROOM2090 main00 @0x00A12 Item_aot_set`, `Bit 91 ... ROOM2040
  main00 @0x01364`, `35 Port-Bits gegen 85 Original-Bits: 2 Kollision(en)`. Teil V: viermal `STILL - das Item ist weg`,
  `Besitz ... = 0x03`, zweimal `haengt noch` FAIL. Teil S: `Maske 0x03` und zweimal `haengt` FAIL. Danach die Datei
  zurueckkopiert (md5 wieder 8dbaa1e8..., `git status` sauber), neu gebaut, Riegel **exit 0**.

## 6. P5 - Alt-Spielstaende: ERFUELLT

* **Historie.** Statt des sehr langsamen `git log --all -G` habe ich alle 9274 einzigartigen Quell-Blobs (.c/.h/.inc/.cmake
  ausserhalb info/ und der Decompilat-Ordner) aus allen 378 Refs bzw. 3882 Commits per `git cat-file --batch` durchsucht:
  * `re15_game_flag_*(9|*BANK*, 77|78|0x4D|0x4E)`: nur die neue Testzeile `re15_game_flag_set(RE15_R36K_BANK, 78, 1)`;
  * SCD-Bytes `0x21/0x22|33/34, 9, 0x4D/0x4E|77/78`: 0 Treffer;
  * `#define X 77|78|0x4D|0x4E`: 0 Treffer; `bit = 77|78`: 0 Treffer;
  * Tabellenpaare `9, 78` nur in Kartengeometrie-Spalten von re15_map_zones.h (keine Flags).
  Gegenprobe der Suche: dasselbe Muster mit 93 findet `#define RE15_R36Q_TUER_BIT 93` und die `(9, 93)`-Aufrufe.
* Spielstandformat: `git diff v0.8.23 HEAD -- re15_savedata.h re15_savedata.c` ist leer, RE15_SAVE_VERSION 9.
  `kartenitems_r36.c` fehlt in v0.8.21, v0.8.22, v0.8.23 und master. Die Lageplaene (und damit 90/91 als Planbit) wurden
  nie ausgeliefert.
* Karten gelesen: die Fixture `r30_karte_nutzer_2026-09-27.mcr` (4 Plaetze) und eine **Kopie** der aktuellen Nutzerkarte
  des Hauptbaums (`re15_card.mcr`, md5 4990b84fa07641edf7862c704c50a1a2, 5 Plaetze) ueber `RE15_R36KB_KARTE`. In keinem
  Platz ist 77 oder 78 gesetzt; die gesetzten Bank-9-Bits sind nur Original-Item-Bits und Port-Bits aus der Liste.
* v0.8.23-Stand mit genommenen Items 0x3F/0x32 (Teil S2, ueber die echte Speicherkarte): `kein Polizei-Blatt im Besitz
  (Maske 0x00)`, beide Plaene haengen (und setzen beim Nehmen 77/78), die Original-Items bleiben genommen.

## 7. P6 - Regression: ERFUELLT

* Sauberer Neubau: `local_build.sh clean`, `configure`, `build` -> `[1874/1874]`, `=== LOCAL-BUILD-OK (build)`. Die neuen
  und geaenderten Dateien erzeugen keine Warnung (die vorhandenen Warnungen in main.c und probe_*.c sind alt).
* `PATH="/c/msys64/mingw64/bin:$PATH" ctest --test-dir re15_port/build -R "r36_kartenitems|kartenitem|r36_kartenbits|map_|karte|r33_karte|r35_karte|unit_r36_|item" --timeout 900 -j 4`:
  ```
  15/123 Test #604: unit_r36_kartenbits ................   Passed    0.19 sec
  63/123 Test #605: unit_r36_kartenitems ...............   Passed    0.09 sec
  121/123 Test #547: integration_r35_karte_fahrstuhl ....   Passed  183.07 sec
  122/123 Test #603: integration_r36_karte ..............   Passed  191.15 sec
  123/123 Test #606: integration_r36_kartenitems_bild ...   Passed  446.32 sec

  100% tests passed, 0 tests failed out of 123

  Total Test time (real) = 448.94 sec
  ```
* unit_r36_kartenitems direkt gefahren: 217 PASS, 0 FAIL (`Zeile 3: ROOM1200 Bit 77`, `Zeile 4: ROOM11D0 Bit 78`,
  `Bits 87/78 aus dem Spielstand -> Blatt 4 (3F) und 0 (B1) im Besitz`).
* integration_r36_kartenitems_bild, debug.log der exe: `lauf_*_b2_mit` (Karte da, kein Bit) meldet
  `[kartenitems] ROOM1200: Karte Blatt 1 (Bit 9,77)`, `lauf_*_b1_mit` meldet `ROOM11D0: Karte Blatt 0 (Bit 9,78)`. Die
  Laeufe `_ohne` (Bit 77 bzw. 78 auf der Karte gesetzt) legen keine Karte an.
* Die Suite dieses Zweigs hat 694 Tests (`ctest -N`).
* Merge gegen das aktuelle r36/integration 7b2e56d9: `git merge-tree --write-tree` ist konfliktfrei, es gibt keine
  gemeinsame Datei, und der Diff 7a0fd453..7b2e56d9 (Spur R, NACHLAUF) fuegt keinen Bank-9-Zugriff hinzu.

## 8. Hinweise (keine Maengel)

* **H1 - Teil Z des Riegels kennt eine Bit-Quelle nicht.** Die Aufnahme (AOT-Typ 9 -> @0x8001e0d0/@0x80021fd8) setzt das
  Bit aus den Slot-Daten (Daten+4), egal welcher Opcode den Slot angelegt hat. Ausser Item_aot_set legen auch Aot_reset
  (0x46, Bit = pc[8]) und Aot_set (0x2C, Bit = Daten+4) mit Sce 9 solche Slots an. In den ausgelieferten RDTs gibt es 8
  Aot_reset mit Sce 9: ROOM40A0/40A1 sub02 @0x00FDA/@0x00FF8/@0x01016 (Bits 14, 15, 16) und ROOM1190/1191 sub10
  @0x027AA (Bit 136); Aot_set mit Sce 9 gibt es keinen. Alle vier Bits liegen schon aus anderen Quellen im Satz, **der
  Satz (85 Bits) und das Ergebnis aendern sich also nicht**. Empfehlung fuer spaeter: Teil Z und bank9_zensus.py um
  "AOT mit Sce 9 -> Daten+4" ergaenzen, damit ein kuenftiger Datenstand nicht durchrutscht. In diesem Punkt ist auch die
  Formulierung in Dossier 9.1/9.3 ("setzen das Bit aus dem Item-Satz") ungenau.
* **H2 - Zahl der Gegenprobe.** Dossier 9.5 und Commit f31e28b2 nennen fuer die Gegenprobe "10 FAIL". Am Endstand sind es
  13, weil Teil S (Commit 1365a163) erst nach der Gegenprobe des Bau-Agenten dazukam und mit 90/91 drei weitere FAIL
  liefert. Betrifft nur die Doku.
* **H3 - Vollstaendigkeit der XREF-Liste.** Die Zensus-Datei nennt als Zugriffe auf DAT_800b1078 nur 0x8001e0d4,
  0x80021fd4, 0x800406d8 und 0x80074688. Zusaetzlich loescht FUN_8003e4f4 (`addiu t0,v0,5316` @0x8003e50c mit
  v0 = 0x800afbb4, Schleife `sw zero,0(t0)` @0x8003e538, 8 Woerter) die Baenke 3, 7, 8 und 9 beim Spielstart; der einzige
  Aufrufer ist FUN_8001d22c @0x8001d570. Das ist keine Bitnummer und aendert den Zensus nicht, bestaetigt aber, dass
  Bank 9 nur bei NEW GAME/LOAD genullt wird.
* **H4 - Integration.** RE15_MIN_TESTS steht auf 592, die Suite hat hier 694 Tests (Sache der Integration, wie vom
  Bau-Agenten vermerkt). Den NACHLAUF-Eintrag 77 als erledigt markieren (Orchestrator).
