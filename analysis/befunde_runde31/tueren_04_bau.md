# Runde 31 — Stufe 4: Tueren-Anschluss bauen (Dossier, laufend)

Zweig `r31/tueren`, Arbeitsbaum `.claude/worktrees/r31_tueren`.
Grundlage: `tueren_03_zuordnung.md` + `tueren_03/zuordnung.json`, `tueren_02_re2.md` §1-§6,
`tueren_01_zensus.md` §9.1, `analysis/tor_1170/09_sequenz.md`.

## Stand (laufend fortgeschrieben)

- [x] 0 Viereck-Tueren (40-B-Door_aot_set) — eigener Commit + Riegel `unit_r31_viereck`
- [ ] 1 Daten (re2_tuer_tabelle.inc, Archive nach shared_assets/RE2/DOOR, tuer_zuordnung.inc)
- [ ] 2 Maschine (Opcodes 0x34/0x35/0x3D, 0x8A-0x8C) + Simulator-Vergleich
- [ ] 3 Laeufer + Ton
- [ ] 4 Griff-Tausch T027/T033/T034
- [ ] 5 Anbindung Kreuz-Raum-Tueren
- [ ] 6 Pruefhaken + Kontaktbogen
- [ ] 7 Paket
- [ ] 8 Riegel r31_tueren
- [ ] 9 Echtes Spiel + Suite

## 0. Viereck-Tueren ROOM4030/4031

**Messung vorher.** `re15_port/tools/engine_tueren.txt` (vom Port selbst erzeugt, `test_map_uebergang.c`)
Zeilen 184/185: `4030 BF0A0 ...` und `4030 C5F00 ...` - der Port las die beiden 40-B-Saetze mit dem
32-B-Schema (Ziel-Stage/Raum aus pc[22]/pc[23] = `be 0a` bzw. `c4 f0`) und stellte Tueren ins Leere auf.
Satzbytes selbst gelesen (ROOM4030.RDT == ROOM4031.RDT an beiden Stellen):

```
@0x47E 3b 01 02 b1 00 00 | 82 a1 6e 9c ae 9d 40 98 fa 97 7c 9d 5e 9d 86 a2 | be 0a 00 00 86 24 00 06 | 03 04 0c 00 | 00..
       Slot 1, sat 0xB1, Band 0, Punkte (-24190,-25490) (-25170,-26560) (-26630,-25220) (-25250,-23930)
       Nutzlast: Lage (2750,0,9350) Richtung 1536, Stage 3 Raum 04 Cut 12 -> ROOM4040 (S225, T116)
@0x4A6 3b 02 02 b1 00 00 | f6 af f0 a1 38 b4 12 9e fc ae a4 98 b0 aa 68 9d | c4 f0 00 00 34 08 00 02 | 03 08 08 00 | 00..
       Slot 2, Punkte (-20490,-24080) (-19400,-25070) (-20740,-26460) (-21840,-25240)
       Nutzlast: Lage (-3900,0,2100) Richtung 512, Stage 3 Raum 08 Cut 8 -> ROOM4080 (S226, T117)
```

**Original (RE1.5 PSX.EXE, selbst disassembliert).**
- Installer `LAB_800405bc`: `80040618 lbu v0,3(v1)` / `80040620 andi v0,v0,0x80` / `80040630 addiu v0,v1,40`
  (sonst `80040634 addiu v0,v1,32`) - sat Bit 0x80 = 40-B-Form.
- Scan `FUN_80042bac`: Objektversatz `80042dc0 sw zero,40(sp)` (pc[5] & 0x80 = 0 bei allen Tueren),
  Viereck-Kopie `80042dd8..80042e5c` (lhu 4/6/8/10/12/14/16/18(s0) + Versatz -> sh 44..58(sp)), Wahl
  `80042f04 andi v0,v1,0x80` -> `80042f10 jal 0x80014368` (a1 = sp+40) statt `80042f20 jal 0x80042b64`;
  Nutzlast `80042f8c jalr v0` mit Delay `80042f90 addiu a0,s0,20` (= pc+22), Rechteckform `80042fb8 addiu a0,s0,12`.
- Trefftest `FUN_80014368` (a0 = Punkt x@0 z@8, a1+4.. = Punkte):
  ```
  80014368 lh t5,4(a1)   8001436c lh t0,6(a1)        ; x0 z0
  80014378 subu t1,v0,t0 8001437c subu t2,v1,t5      ; pz-z0, x1-x0
  80014380 mult t2,t1 -> a0                          ; (x1-x0)(pz-z0)
  80014390 subu a2,v0,t5 80014394 subu a3,v1,t0      ; px-x0, z1-z0
  80014398 mult a3,a2 -> v0                          ; (z1-z0)(px-x0)
  800143ac slt v0,v0,a0  800143b0 bne -> 0
  800143b8 mult t3,t1 / 800143c0 mult t4,a2          ; (x3-x0)(pz-z0) slt (z3-z0)(px-x0)
  800143c8 slt v0,v0,v1  800143cc bne -> 0
  800143d4..800143f8  Bezug auf Ecke 2 (pz-z2, x1-x2, px-x2, z1-z2), 80014400 subu t3,t3,v0 (x3-x2)
  80014408 slt a0,a0,v0  8001440c bne -> 0           ; (x1-x2)(pz-z2) < (z1-z2)(px-x2)
  80014410 subu t4,t4,v1 (z3-z2)
  80014424 slt v0,v0,v1  80014428 beq -> 1 (Delay 8001442c ori v0,zero,1), sonst 80014430 -> 0
  ```
  Feste Umlaufrichtung, 32-Bit-Produkte (mflo), vorzeichenbehafteter Vergleich. Der Port-Test
  `re15_aot_point_in_quad` (Umlaufrichtung frei) ist NICHT dieser - fuer Tuersaetze gibt es jetzt
  `re15_aot_point_in_quad_fun80014368` (aot_common.c), Befehl fuer Befehl.

**Bau.** `scd_vm.c op_door_aot_set`: Viereck-Punkte pc+6..21, Nutzlast ab pc+22 (sonst pc+14), `has_quad`
und die Punkte je Satz gesetzt (auch zurueckgesetzt fuer Rechtecksaetze), Huellrechteck fuer die Port-Stellen,
die mit Mitte/Halbmass rechnen. `aot_common.c`: der Tuer-Vorwaertstest nimmt fuer Viereck-Tueren den neuen Trefftest;
der Kommentar "non-door scan artifacts" ist korrigiert.

**Messung nachher** (`unit_r31_viereck`, Spielschritt-Geruest wie `probe_r30_tuer_verschlossen.c`: Raum booten,
einschwingen, Standplatz vor der Tuer suchen, QUADRAT):

```
ROOM4030 Slot 1: Typ 1 Viereck 1 Punkte gleich Ziel ROOM4040 Lage (2750,0,9350) Richtung 1536 Cut 12
  Durchgang: Standplatz gefunden, Raumwechsel 1 -> ROOM4040 Lage (2750,0,9350) Cut 12
ROOM4030 Slot 2: Typ 1 Viereck 1 Punkte gleich Ziel ROOM4080 Lage (-3900,0,2100) Richtung 512 Cut 8
  Durchgang: Standplatz gefunden, Raumwechsel 1 -> ROOM4080 Lage (-3900,0,2100) Cut 8
ROOM4031 Slot 1: ... Ziel ROOM4041 ...   Durchgang -> ROOM4041
ROOM4031 Slot 2: ... Ziel ROOM4081 ...   Durchgang -> ROOM4081
```
Suite nach Schritt 0: `=== LOCAL-BUILD-OK (test) — Tests 406/406` (587 s).

## Log

(2026-09-29) Dossier angelegt. Schritt 0 gebaut, gemessen, Suite 406/406.
