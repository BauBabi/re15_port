# Runde 39 — Spur H "elza": ABNAHME 0 (unabhaengig, 2026-10-18)

Baum `.claude/worktrees/r39_elza`, Zweig `r39/elza`, HEAD 564810c0 (5 Commits ueber Basis c71fa144; master-only
Commits de4d7b74/3eda01a4 sind reine Dokumente — der `git diff master`-Eintrag "UEBERGABE.md/args_welle2_forts.json
geloescht" ist KEINE Aenderung der Spur, sondern der Vorsprung von master; `git diff <merge-base>` enthaelt nur die
16 Spur-Dateien). Abnahme-Werkzeuge: eigene exe-Kopien `re15_pc_abn_*.exe` neben der exe, Laeufe im Scratchpad
`abn/`, kein taskkill.

## 0. Bau und Riegel (selbst gefahren)
* `local_build.sh configure` + `build`: LOCAL-BUILD-OK. Die vorhandene exe (12:28) war laut ninja aktuell; zur
  Sicherheit alle fuenf geaenderten Quelldateien angestossen und neu gebaut -> exe 12:44:28 (7 593 276 Byte) = HEAD.
* Unit, gezielt (Regex des Bau-Agenten + `r36_elza|r16_|r30_elza|rvd_scan`): **144/144 gruen** (24 s).
  Darunter `unit_r39_elza_hand` (11 Pruefungen), `unit_r39_elza_klettern_zensus` (4) / `_loescher` (14),
  `unit_r38_tagebuch_ablauf`, `unit_r37_feuerloescher_raum`.
* Integration `^integration_r39_elza_(hand|karte|loescher)$`: **3/3 gruen** (100 s / 50 s / 38 s), Riegel-Zeilen:
  hand `F452..F490: 39 Ticks Motion 11 / Kommandowort 4`, `F491..F521: 30 Bilder Clip 0 rueckwaerts, 0 vorwaerts;
  ab Cut_chg(6) F521: 8/8 Bilder Clip 5`; karte `Hinweis F504..F721 VOR Plc_motion F736 / Plc_ret F766 / Evt_end
  F766`; loescher `rot 1024/3072/0: kein Aufstieg`, `rot 0: msg 29`.
* Zensus selbst nachgerechnet (`test_r39_elza_klettern.exe zensus`): 240 RDT, 672 Obj_model_set-Records, 558
  Null-Box, Verteilung Band − Spielerband(y): +0:51, **+1:291**, +2:74 … — die Zahlen des Dossiers stimmen.

## 1. RE-Gate: Stichproben SELBST disassembliert (`re15_disasm.py dis`, PSX.EXE Auslieferung; RE2 `re2_disasm.py`)
| Behauptung im Diff | Bytes/Instruktionen gefunden | Urteil |
|---|---|---|
| Plc_motion-Handler: `+0x94 := Clip`, `+0x4 := 4` @0x80041bb0 | `80041ba4 ori v1,zero,0x4` / `80041ba8 sb a1,148(v0)` / `80041bb0 sb v1,4(v0)` / `80041bb4 sb zero,6(v0)` / `80041bc4 sb a2,5(v0)` | stimmt |
| Dispatcher FUN_80031c44: Kommandowort @0x80031c8c, Tabelle @0x80073f90, `jalr` @0x80031cb4, Pause-Gate @0x80031c54/c78 | `80031c54 lw a0,-13760(a0)` (g_pauseflags) / `80031c78 bltz a0` / `80031c8c lbu v1,-13736(v1)` (0x800aca58) / `80031c94 sll v1,v1,2` / `80031ca4 addiu at,at,16272` / `80031cac lw v0,0(at)` / `80031cb4 jalr v0` | stimmt |
| Tabelle @0x80073f90: [0] cmd-0, [1] Idle-FSM, [4] = 0x80030660 | `table 0x80073f90`: [0] 0x800318f8, [1] 0x80031de8, [4] 0x80030660 | stimmt |
| TURN Sub 9 @0x80031360: Phase := 1 @0x80031398, `+0x94 := 5` @0x800313a4, `+0x95 := 0` @0x800313b0, `+0x8f := 7` @0x800313b8; arc_test 0x60 @0x800313d4; Ankunft Sub := 6 @0x800313ec | `80031398 sb v0(=1),0(a0)` / `8003139c ori v0,zero,0x5` / `800313a4 sb v0,-13592(at)` (0x800acae8 = +0x94) / `800313b0 sb zero,-13591(at)` / `800313b8 sb v0(=7),-13597(at)` / `800313d0 jal arc_test` + `ori a2,0x60` / `800313ec sb v0(=6),-13735(at)` | stimmt |
| WALK Sub 4 @0x80030af0: `+0x94 := 5` @0x80030b58, `+0x95` @0x80030b64, `+0x8f := 7` @0x80030b6c, Phase 1→2 @0x80030b94, arc_test 0x15e | `80030b58 sb v0(=5),-13592(at)` / `80030b64 sb zero,-13591(at)` / `80030b6c sb v0(=7),-13597(at)` / `80030b84 ori a2,0x15e` / `80030b94 sb v0(=2),-13734(at)` | stimmt |
| RUN Sub 5 @0x80030d28: `+0x94 := 5` @0x80030d90; Phase 2: `+0x94 := 0` @0x80030dd8, `+0x95 := 0` @0x80030de0 | `80030d90 sb v0(=5),-13592(at)` / `80030dd8 sb zero,-13592(at)` / `80030de0 sb zero,-13591(at)` / `80030de8 sb v0(=7),-13597(at)` | stimmt |
| Plc_dest nullt `+0x1c4` @0x80041c4c | `80041c4c sh zero,452(a1)` | stimmt |
| FUN_8002d2c0: `andi 0x101` @0x8002d358, Band-Gate `lbu v1,0(s3)` / `lbu v0,130(s0)` / `bne` @0x8002d364-70, Reichweite `jal 0x8002d1e8` @0x8002d37c, Box `jal 0x8002da4c` mit a2 = −112 @0x8002d394/98 | `8002d358 andi v0,v0,0x101` / `8002d364 lbu v1,0(s3)` (s3 = 0x800aca3c+154 = 0x800acad6) / `8002d368 lbu v0,130(s0)` / `8002d370 bne v1,v0` / `8002d37c jal 0x8002d1e8` / `8002d394 jal 0x8002da4c` / `8002d398 addiu a2,zero,-112` | stimmt |
| FUN_8002da4c: `lhu v0,6(a2)` @0x8002da8c (vorzeichenlos), `+ Rand` @0x8002da94, `sll 1` @0x8002da9c, `sltu` @0x8002daa0; Band-Gate @0x8002da50-58 | `8002da50 lbu v0,130(t0)` / `8002da58 bne v0,a3` / `8002da8c lhu v0,6(a2)` / `8002da94 addu v0,v0,t1` / `8002da98 addu a1,a1,v0` / `8002da9c sll v0,v0,1` / `8002daa0 sltu v0,v0,a1` | stimmt |
| RE2 Opcode 0x84 @0x800591C4: Modus 4 @0x800591d4/dc, Anforderung @0x800591e8, Bit 0x8000 @0x800591fc, Nummer @0x80059210 | `800591d4 addiu v0,zero,4` / `800591dc sb v0,[0x800d5c00]` / `800591e8 sb v0(=1),[0x800df348]` / `800591ec ori v1,v1,0x8000` / `800591fc sw v1,[0x800cfbd8]` / `80059210 sb v0,[0x800d69f2]` | stimmt |
| RDT-Bytes: ROOM1091 @0x02202 = ROOM1090 @0x0237E `2d 02 00 00 02 00 08 00 10 00 6a 09 a3 f5 2e ff …` (Band 2, y −2653, Box 0); ROOM1010 @0x00930 `2d 00 00 00 01 00 0a 00 00 00 c8 00 c0 f9 7c 15 …` (Band 1, y −1600) | `xxd` der shared_assets-RDTs: Byte fuer Byte wie zitiert | stimmt |
| ROOM1031 sub13: Plc_motion(0,11) @0x029A4 `3f 00 0b 00`, Plc_flg 0x04 @0x029A8, Plc_motion(0,0) @0x029BE, Plc_flg 0x80 @0x029C2, Cut_chg(6) @0x029CA `29 06`, Plc_dest(0,9,0x20,-9097,-21855) @0x029CC `40 00 09 20 77 dc a1 aa` | `xxd -s 0x2982 ROOM1031.RDT` | stimmt |
| ROOM1011 sub07: Message_on 18 @0x00E64 `2b 12 ff ff`, Plc_motion(1,11) @0x00E6A, Plc_flg 0x80 @0x00E6E, Plc_ret @0x00E76, Evt_end @0x00E82 | `xxd -s 0xE60 ROOM1011.RDT`: `2b12 ffff 0200 3f01 0b00 4300 8000 090a 1e00 4200 4605 … 0100` | stimmt |

Guess-Tells im Diff (`deferred|tunable|interim|for now|faithful|plausibel|TODO|getenv`): **keine Treffer**.
Kennzeichnung: Punkt 2 Zeitpunkt = NUTZER-VORGABE, Form = RE2 0x84 (so im Code); Punkt 3 Regel "Spielerband(y)+1" =
PORT-WAHL in der Form der drei Vorlagen-Records (so im Code und im Dossier; der Zensus belegt, dass es kein
Original-Gesetz ist: 51/558 auf dem Spielerband).

## 2. Vertrag
* Geaenderte Pfade (gegen merge-base): nur `analysis/befunde_runde39/H_elza.md`, `engine/src/{actor_locomotion,
  elza_r39,feuerloescher_r37,player_common,tagebuch_r38}.c`, `include/re15_{elza_r39,feuerloescher_r37,tagebuch_r38}.h`,
  `tests/integration/test_r3{8_tagebuch,9_elza}.cmake`, `tests/unit/probes/r39_elza.cmake`,
  `tests/unit/test_r3{7_feuerloescher,8_tagebuch,9_elza_hand,9_elza_klettern}.c`. Keine release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists (Probe ueber GLOB `probes/r39_elza.cmake`).
* Gemeinsame Dateien: player_common.c (+1 Zeile Haken + Kommentar), actor_locomotion.c (+1 Include, +1 Aufruf,
  Kommentar) — kleine Haken. elza_r36.c (im Vertrag fuer H vorgesehen) nicht angefasst — nicht noetig.
* Bank-9-Bits / Nachrichten-Ids / Slots: keine neuen (Punkt 2 nutzt Bit (9,68) und msg 18 aus Runde 38; Punkt 3 den
  Prop obj 5 / Slots 3/6 aus Runde 37). Keine Assets, keine synchro-Dateien.
* Tests je Punkt vorhanden und messend (s. §0).

(Fortsetzung: §3 Messungen je Punkt, §4 Urteile, §5 Maengel — werden nach jedem gemessenen Punkt ergaenzt.)
