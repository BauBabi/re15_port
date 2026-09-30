# Runde 34 (Granaten) — GEGENPRUEFUNG Spur A (Skeptiker am gebauten Stand)

Stand: 2026-09-30, Zweig `r34g/a-granate` @ 85320e45 (Basis C0 8d8651e4), Arbeitsbaum `.claude/worktrees/r34g_a`.
Geprueft: `git diff 8d8651e4..HEAD` (Spur-A-Anteil; `master...HEAD` enthaelt zusaetzlich den C0-Vertrag, der nicht Teil
dieser Pruefung ist). Ich aendere KEINEN Code; nur diese Datei.

STATUS: IN ARBEIT (fortlaufend geschrieben)

## 0. Vorgehen

1. Jede tragende Konstante selbst mit `re15_disasm.py` (dis/read/bytes) nachgelesen (§1).
2. Semantik gegen BAUPLAN §1.1/§1.2/K1-K3/K9 (Delay-Slots, Reihenfolge, Vorzeichen, s16/s32, Bildzaehlung) (§2).
3. Sonde: echte Engine? Erwartung unabhaengig? Negativ-Kontrollen? EIGENE Mutationsproben (§3).
4. Dateibesitz (§4), Sonden + Bestandstests im Bauverzeichnis des Baums (§5), Regressionsrisiko (§6), Auftragsluecken (§7).

## 1. Konstanten — selbst disassembliert (re15_disasm.py, info/Re1.5/PSX.EXE)

Alle unten gelisteten Werte in DIESER Sitzung nachgelesen; Ergebnis = stimmt, sofern nicht anders vermerkt.

| Bereich | Port-Stelle | Original (eigene Lesung) | Urteil |
|---|---|---|---|
| R30 Satz/Flags/B/A/Zuender | re15_esp.c case 30 | `ori v0,zero,0x17; sb v0,110(v1)` @0x80018448/50; `ori 0x3; sb 108` @0x8001845c/60; `ori 0x1d; sh 2` @0x8001846c/70; `sh zero,0` @0x80018478; `ori 0x2a` @0x80018474 / `sh v0,30` @0x8001847c | stimmt |
| R30 HOCH/MITTE/TIEF | case 30 | 0x17c/-110/0x15 @0x80018494-a8, acc -2 im Delay von `j` @0x800184b0 -> `sh v0,8(v1)` im Delay von `jal 0x8001af20` @0x800184dc; 0x118/-50/0x18, -1 @0x800184bc-d4; 0x50/0/1, -1, Zaehler 5 @0x80018518-38 | stimmt (Delay-Slots richtig gelesen) |
| R30 Zaehler | case 30 | RNG FUN_8001af20: `lhu t1` @0x8001af28 wird NIE gelesen; `srl v1,a0,7 / andi 0xff / addu / andi 0xff` @0x8001af30-3c, Rueckgabe `andi v0,a0,0xff` @0x8001af4c; R30: `bgez`/`sra 2`/`sll 2`/`subu` = & 3, `addiu 7` @0x80018504, `sh v0,38(a0)` @0x8001850c | stimmt |
| RNG-Zustand 0x800ac774 | (nicht gefuehrt) | Xref (eigener lui/imm-Scan, EXE + STAGE1..6): nur `addiu v0,v0,-14476` @0x8001af24 und Seed `sw v0,-14476(at)` @0x80031634 | Bauer-Schluss (kein Leser) bestaetigt |
| R29 komplett | esp_fx_dispatch_b_29 | `lh t1,42(t0)` @0x80018330, `blez` @0x80018338; Liegen SE @0x80018350-58 (a1 = sp+16 unbeschrieben), `ori 0x63; sb 108` @0x80018368-6c, `ori 0x1f; sh 0` @0x80018378-7c, `sh zero,2` im Delay @0x80018384; Abprall: vx `lhu`+`sll/sra 16` = s16, 0x55555556-Idiom (mfhi - Vorzeichen = trunc), `subu a3,a3,a2; sh a3,16` @0x800183c8-cc; Zaehler `addiu -1; sh 38` @0x800183d0-d4; `lw 56; subu t1; sw 56` @0x800183c4/dc/e0; vy `subu zero; sh 18` @0x800183f4-f8; SE-Punkt `lh 40/42/44` @0x800183d8/0x80018400/0x8001840c, Code `lhu 38; sll 8; or 0x010a0001` @0x80018410-28 | stimmt (s16-Lesung, Trunkierung, Reihenfolge) |
| R31 komplett | case 31 | `lhu v1,30(a1)` @0x80018560; `beq v1,zero,0x80018688` @0x80018568; `ori v0,zero,0x1` im Delay von `bne` @0x80018574 -> Latch `sb` @0x8001857c; `ori 0x61; sb 108` @0x80018580-84; P `lh 40/42/44`, `addiu -500` @0x800185a8; `ori a0,zero,0x1f4` @0x80018598, `ori a2,zero,0x2` @0x800185b4, `jal 0x80012d60` @0x800185b8 (`sw v0,24(sp)` im Delay); Kind 0x03195000 a1 = `lh 46` / a2 = 0x80072d4c / a3 = &P @0x800185c0-e0; SE 0x04080001 @0x800185e4-ec; Z2 @0x80018600-64 (Zuender NEU gelesen @0x80018600); Abzug `addiu -1` @0x8001867c + `sh` im Delay @0x80018684; Z0: `sb zero,108(a1)` @0x800186b0 VOR `jal 0x800199d4` @0x800186c8, kein Abzug | stimmt |
| Tick zwei Durchgaenge | re15_esp_fx_tick | Schleife 1 @0x80019e64-c4 (`andi 0x1` @0x80019e78); Kind-Init `andi 0x8` @0x80019ef4 / `xori 0x9` im Delay @0x80019efc / `sb` @0x80019f00 / `jalr` @0x80019f30 VOR dem Lebend-Gate @0x80019f44-50; Weltlage @0x8001a118-2a4 VOR Routine B @0x8001a2b4-d4; Physik nach Neulesen `lbu 108` @0x8001a2e8: euler += angvel `lhu/addu/sh` @0x8001a2fc-320 (16 Bit), xlat (`lw`, s32) += vel (`lh`, s16) @0x8001a324-360, DANACH vel += acc @0x8001a354-388; Anim @0x8001a38c-47c | stimmt |
| Weltlage Flags&0x80==0 | esp_fx_weltlage | Einheitsmatrix 0x80072d4c -> 0x1f800000; SVECTOR (euler.x, euler.y + `lhu 46`, euler.z) @0x8001a16c-19c; RotMatrix @0x8001a1a0; xlat `lhu 52/56/60` (lo16) @0x8001a1b8-d4; ApplyMatrix @0x8001a1e4; `sh` +0x28/2a/2c @0x8001a1fc/10/20; zweite ApplyMatrix(Anker +0x4c, Versatz +0x40/44/48) @0x8001a248; `addu`+`sh` mit +0x60/64/68 @0x8001a258-2a4 (16 Bit) | stimmt (Port traegt Anker.R*Versatz+Anker.T in x/y/z; 16-Bit-Summe = (int16)(r + x)) |
| RotMatrix-Zwilling | esp_rotmatrix/esp_trig | FUN_80068098 vollstaendig gelesen: Negativzweig `subu t7,zero,t7` + `andi 0xfff` im Delay, sin negiert (`subu t3,zero,t8` @0x800680d0 / `subu t6,zero,t4` @0x80068134 / `subu t5,zero,t8` @0x800681c0), cos unveraendert; alle 9 `sh` (@0x8006816c/80/94|d4, @0x8006820c/2c, @0x80068274/a4/ec, @0x80068318) mit derselben Rundung (Negation VOR `sra 12`) | stimmt; `multu` = gleiche Low-32 wie signed (|Produkt| < 2^25) |
| Sinus-Tabelle | re15_trig_lut.c | 4096 Worte @0x800794c4 gegen `re15_trig_lut[]` verglichen (eigenes Skript): identisch | stimmt |
| ApplyMatrix | esp_applymatrix | @0x800661c0: `ctc2` RT, `lwc2` VXY0/VZ0, `0x4a486012` = MVMVA sf=1, mx=RT, v=V0, cv=3 (keine), lm=0; `swc2` Reg 25..27 = MAC1..3 | stimmt |
| Spawn-Gate/Code/Versatz/Gier | game_step_common.c:1965-1981 | `lbu -13731` / `ori 0x9` / `bne` @0x80033684-8c; 0x13/0x16/0x18 @0x80033690/0x800336a4/0x80033758; Bits 0x8000/0x4000/0x2000 @0x800336b4/0x80033714/0x80033770; `lui a0,0x40d; ori 0x1000`; a1 = `lh -13634` (0x800acabe) @0x800336cc/2c/88; a2 = [0x800acbdc]+0x7a4; Versatz {0,0x12c,0x320}/{0,0,0x1f4}/{0,0,0x12c} | stimmt |
| Recoil-Break | player_common.c (Bestand) | `andi v0,a0,0x100` @0x80033600, Byte2 @0x80074092+(w-1)*5, `sltu v0,v0,a0` @0x8003363c (Bild > Byte2), `sh 3 -> 0x800aca5a` @0x8003364c, dann `j 0x800337ac` = KEIN Spawn | stimmt |
| Drehen im Zielen | player_common.c:1023-1034 | Gun-FSM: RAISE `andi 0x8` -> `subu` Byte0 @0x80033000-48, `andi 0x2` -> `addu` @0x80033050-94; HOLD Byte1 (0x80074091) @0x800333a0-434; ABZUG Byte1 `srl 1` @0x8003355c-fc; LOWER `addiu -24/+24` @0x80033cd8-d1c; RELOAD @0x80033de8-e2c; Melee-FSM gleiche Richtung (`subu` Byte0 @0x80034fd0-5064); virtuelles Bit 3 = LINKS / Bit 1 = RECHTS (pad_common.c-Tabelle = Preset 0x80073dbc); Parametersatz aller Waffen 1,3..16: `18 30 ..` (read 0x80074090) | stimmt (Rate 24/48 fuer alle Waffen gleich) |
| Resolver-Tabellen | re15_damage.c (Bestand) | `read 0x8006f418 11 --w 2 --signed` = [10,20,1000,1000,1000,50,100,200,300,1000,0]; `read 0x8006f430 11 --w 1` = [3,3,9,10,11,14,15,16,17,18,20] | stimmt |
| Item-Debug | menu_common.c:1283-1328 | @0x8004a138-0x8004a35c komplett gelesen (siehe §2.4) | stimmt bis auf §2.4 (Ruecksetzen beim Oeffnen fehlt) |
| Kind-Spawner | esp_fx_spawn_kind | FUN_800199d4: Suche ab 0 `sltiu v0,t3,0x60` @0x80019a60, `lbu 108` / `beq` @0x80019a7c-84, `ori v0,zero,0xa` @0x80019a88 / `sb` @0x80019aa4, `sh s2,46(t0)` @0x80019ab8, Versatz a3 -> +0x40.. @0x80019abc-dc, a2 -> +0x74 und Matrix -> +0x4c..0x68 @0x80019af4-b30, +0x28/2a/2c := 0 @0x80019b44-4c | stimmt (spawn_ex memset nullt wpos/granate_art ebenfalls) |

