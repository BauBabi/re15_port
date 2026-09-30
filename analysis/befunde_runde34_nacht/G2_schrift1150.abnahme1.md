# Spur G2 (ROOM1150 Irons Office, blinkende Schrift) — ABNAHME 1

Stufe: ABNAHME 1 (unabhaengig, nach dem Bau). Prueferin hat den Code nicht geschrieben.
Auftrag: versuchen zu WIDERLEGEN, dass Spur G fertig ist.

Status: IN ARBEIT (Geruest angelegt)

## 0. Urteil

(offen)

## 1. Stand gelesen (git log, Dossier, Gegenpruefung)

* Zweig `r34n/schrift1170`, HEAD `303094c8` (Dossier 9.7), 32 Commits seit Basis `cf0e68ba`.
  Code-Aenderung (`git diff cf0e68ba..HEAD -- engine platform include`): neu
  `engine/src/masken_gruppen.c` + `include/re15_masken_gruppen.h`; Haken je 1-9 Zeilen in
  `scd_vm.c` (Tabelle 0x45), `room_common.c` Schritt 9, `platform/pc/main.c` pc_cam_present_apply,
  `render_pc.c` Maskenliste, `platform/psx/main.c`, `platform/psx/src/render.c`; dazu Pins/Sonden
  und der Integrations-Riegel. `local_build.sh` weicht von master nur um den Bau-Fix 7d4d11dd ab
  (`git diff master -- re15_port/tools/local_build.sh` leer).
* Dossier `G2_schrift1150.md`: Lesart "HEAVEN"-Leuchtschrift Cut 2 hinter Irons' Schreibtisch;
  Mechanismus sub05 + Opcode 0x45 (Col_chg_set) schaltet sprite.pri-Gruppen 6..11; Original in
  DuckStation an 6 Staenden gemessen (G2_06); Bau + eigene Abnahme §9 (Lade-Weg, Debug-Sprung zu
  Fuss, Inventar, 8 weitere Raeume, DuckStation-Gegenprobe ROOM3000).
* Gegenpruefung `G2_schrift1150.gegenpruefung.md`: "haltbar mit Auflagen", 10 Auflagen; das
  Dossier meldet alle 10 erfuellt bzw. begruendet abgelehnt (Nr. 2 Optionsschirm).
* Nicht vom Bauer abgedeckte Wege (daher hier selbst gefahren, Abschnitt 3/4): echter Tuerweg
  ROOM1130 -> ROOM1150 und zu Fuss in Cut 2; Wiederbetreten von Cut 2 in einer AUS-Phase an der exe;
  Spielstand des NUTZERS (Hauptbaum `re15_card.mcr`, nur gelesen/kopiert); ROOM1151 zu Fuss statt
  per Karte direkt in Cut 2.

## 2. Eigener Bau + Suite-Zeile

(offen)

## 3. Nutzerpunkt an der echten exe (AUFTRAG.md, Nutzer-Korrektur ROOM1150)

(offen)

## 4. Gegenproben (vorher/nachher, Variante 1151, Wiederbetreten, Laden, Nachbarraeume)

(offen)

## 5. RE-Gate-Pruefung des Codes (Konstanten, Belege, Stubs)

### 5.1 Selbst disassembliert (re15_disasm.py gegen `info/Re1.5/PSX.EXE`, nicht aus dem Dossier uebernommen)

| Stelle | Instruktionen (selbst gelesen) | Code-Umsetzung | Urteil |
|---|---|---|---|
| Tabelle 0x45 | `table 0x800745bc` -> `0x800428d4` | `s_op_table[0x45] = op_col_chg_set` | stimmt |
| Handler 0x800428d4 | `800428ec lbu a0,1(v0)` / `800428f0 lbu a1,2(v0)` / `800428f4 jal 0x800396a8` / `800428f8 addiu a0,a0,1` / `80042900 ori v0,zero,0x1` / `80042904 addiu v1,v1,3` / `80042908 sw v1,28(s0)` | `re15_mg_setzen(pc[1]+1, pc[2]); pc += 3; return 1;` | stimmt |
| FUN_800396a8 | `800396b8 lbu a3,0(v0)` (RDT[0]) / `800396c0 lw v1,0x800b2584` / `800396c4 beq a3,zero` / `800396cc andi a0,a0,0xff` / `800396d0 lbu v0,1(v1)` / `800396d8 bne v0,a0` / `800396e0 sb a1,0(v1)` / `800396e4 sltu v0,a2,a3` / `800396ec addiu v1,v1,4` | Schleife i < Zahl, Byte1 == g1 -> Byte0 := wert (ganzes Byte) | stimmt |
| FUN_80039590 Test | `800395e8 lbu v0,0(s3)` / `800395f0 andi v0,v0,0x1` / `800395f4 beq v0,zero,0x80039668` | `re15_mg_sichtbar` = Byte0 & 1 | stimmt |
| FUN_800392d4 (ganz, 175 Instr.) | Cut aus `lh 0x800b0fe4` @0x800392e0, Sektion `lw t5,28(v0)` @0x8003931c; NULL `bne v1,-1` @0x8003932c -> `j 0x8003955c` mit `sb zero,0(a0)` @0x80039338; `srl t2,v1,16` @0x80039330 + `sb t2,0(a0)` @0x80039358; Loeschschleife `lbu a3,7(v0)` @0x8003936c, `sb zero,0(s5)` @0x80039378, `andi v0,a3,0xff`/`bne` @0x8003937c-80; Gruppe leer `beq v0,zero,0x80039540` @0x800393c0; je Maske `ori v0,v0,0x1`/`sb` @0x800393e0-e4, `addiu v0,a3,1`/`sb v0,-1(t1)` @0x800393e8-ec, `addiu s5,s5,4` @0x80039494; Innenschleife `andi v0,t2,0xff`/`sltu v0,v0,v1`(u16 Gruppenzahl) @0x8003950c-38 **ohne Ueberspring-Zweig** (jede Maske der Gruppe bekommt einen Record); Aussenschleife `addiu a3,a3,1` @0x80039544, `sltu v0,v0,t8` (t8 = kopf & 0xffff) @0x80039550 | `re15_mg_aufbauen`: gleiche Reihenfolge, Index = laufende Maske, Leer-Gruppen zaehlen mit, NULL -> Zahl 0 ohne Loeschen | stimmt |

Der Record-Index des Originals ist also die laufende Maske in Bau-Reihenfolge; `re15_pri_parse_section`
(pri_common.c Z.84-141) flacht in derselben Reihenfolge ohne Auslassung ab (Abbruch nur bei
Dateiende) — Renderindex i == Record i.

### 5.2 Konstanten im neuen Code

(folgt)

## 6. Maengelliste

(offen)
