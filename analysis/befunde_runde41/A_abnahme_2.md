# Runde 41 — Spur A "wasser20b0": ABNAHME 2 (unabhaengig, 2026-10-24)

Baum `.claude/worktrees/r41_wasser20b0`, Zweig `r41/wasser20b0`, HEAD 9171a10e (Code-Endstand der Nachbesserung 2), Merge-Basis
master 116e8b0f (master-HEAD 71b52f4c traegt seitdem nur Doku unter analysis/befunde_runde41/ - `git diff --name-only 116e8b0f master`).
Massstab: AUFTRAG.md (Wortlaut Punkt 1/2, Nachtraege 2/3, Entscheide E1/E2, A-E3/A-E4/A-E5), VERTRAG.md §1/§2, Maengel M1..M5 aus
`A_abnahme_1.md`. Der Bau-Agent wird nicht geglaubt; nur selbst Gemessenes zaehlt. Bericht FORTLAUFEND (Stand je Abschnitt).
Scratch (nicht versioniert): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/
scratchpad/A41abn2/` (unten kurz `A41abn2/...`; Logs, Disasm-Auszuege, Laufordner `mess/<lauf>/`). Messfahrten mit `A41abn2/lauf.sh`
(exe-Kopie `re15_pc_a41abn2_<lauf>.exe` neben der exe, `env -i`, timeout, Kopie danach geloescht); ctest mit `A41abn2/ct.sh`
(PATH msys64 zuerst, git am Ende).

## 0. Baustand (selbst gebaut)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (07:13). exe `re15_port/build/platform/pc/re15_pc.exe` mtime 2026-10-24 07:10:00 ist juenger als jede
  Quelle des Diffs (juengste: re15_wassersprung_r41.h 06:39:28, test_r41_wasser20b0.c 06:10:34, wassersprung_r41.c 06:08:04,
  audio_pc.c 05:56:49); `git status` sauber -> die exe ist der Code-Stand 9171a10e.
- Delta seit Abnahme 1 (`git diff d8c7334b HEAD --stat -- re15_port`): wassersprung_r41.c (+192), re15_wassersprung_r41.h (+49),
  audio_pc.c (+5/-1: 1 Include, 2 Zeilen, 1 geaenderte Zeile), test_r35_granate.cmake (+10/-1), test_r41_wasser20b0_npc.cmake (+155),
  probes/r41_wasser20b0.cmake (+14), test_r41_wasser20b0.c (+303). Gesamt-Diff gegen die Basis (`git diff master...HEAD --stat --
  re15_port`): 23 Dateien, +2722/-41 (Aufstellung Abschnitt 6).


## 1. Gezielte Tests (selbst gefahren)
- `ct.sh -R "^unit_r41_wasser20b0_" --timeout 600 -j 4` (07:14) -> `100% tests passed, 0 tests failed out of 11`. Einzellaeufe der
  neuen Teile: `test_r41_wasser20b0.exe ton` -> `ton: 4 Pruefungen, 0 Fehler` ("Zeile + Quelle des Ports = Erwartung aus den EDT-Bytes in
  allen 536 Faellen", "Zaehlung 52 Raeume: Original 35 / Retail 387 / stumm 114", "ohne Stempel-Zweig stumm: 8 (ROOM6030/6031 Boden
  0x17)"); `... lage` -> `lage: 12 Pruefungen, 0 Fehler` (PAN/ATT = PSX.EXE @0x80074728..0x80074827, CORDIC-Winkel @0x80078c90, drei
  Lagen von Hand, Pegel, "ROOM2000 Cut 0 ... danach keine haengende Lage"). Die Riegel lesen PSX.EXE ueber `RE15_R41A_PSX_EXE`
  (`ok : PSX.EXE geladen ... 718848 B`).
- `ct.sh -R "^integration_(r41_wasser20b0|r41_wasser20b0_npc|r35_granate)$" --timeout 1800 -V -j 3` (07:17-07:21) -> `100% tests passed,
  0 tests failed out of 3`: integration_r41_wasser20b0 Passed 99.11 s (Leon und Elza `EM024-Spawns=35 fx-Zeilen id=15: 2408 Cut7=1
  Cut6=1 frei(mo=1)=1 zeichnet0x24=0`), integration_r35_granate Passed 174.59 s (`r35_granate [gator]: ok — 23 Slot 35: HP 3000 -> 2000,
  Zustand 2`), integration_r41_wasser20b0_npc Passed 229.03 s (`npc2000: Schritte 64 ... Ton: Original 3, Retail 61, stumm 0;
  Mischer-Stimmen mit Lage (NPC-Lage) 64, davon seitlich 64; [foot] SE unterdrueckt 0`, `naechster Abstand 41 -> Pegel 104, fernster
  Abstand 125 -> Pegel 56`; `elza2001: Schritte 23 (Typ 0x49: 23, im Wasser 5) ... Retail 23 ... NPC-Lage 23, davon seitlich 23`;
  `npc11c0: Schritte 8 ... Retail 8 ... NPC-Lage 8`; `landung_leon/landung_elza: Landungen 1, fx-Zeilen id=6 sub=3: 104`).

## 2. RE-Gate (Stichproben selbst disassembliert; `re15_disasm.py` = RE1.5 PSX.EXE, `re2_disasm.py` = info/re2leon/PSX.EXE)
Auszuege: `A41abn2/dis_45700.txt`, `dis_45a64.txt`, `dis_59d3c.txt`, `dis_55d10.txt`, `re2_dis_5c040.txt`.
- **NPC-Zeile (M1, RE1.5 FUN_80045630):** `andi v0,s0,0x7f` @0x80045718, `addu a0,s3,v0` @0x80045724, `lbu v0,8(v1)` (Typ) @0x80045728,
  `sltiu v0,v0,0x40` @0x80045730, `bne v0,zero,0x80045760` @0x80045734, NPC: `lw a2,8(v0)` (EDT = *(DAT_800ac778+8)) @0x80045754,
  `j 0x80045794` @0x80045758 / `addiu s3,a0,-8` @0x8004575c (Verzoegerungsplatz). Stimme: `lbu v0,3(a2)` @0x800457a0, `andi s1,v0,0x1f`
  @0x800457a8, `addiu s1,s1,-16` @0x800457ac, `xori v0,v0,0x4` / `sltu v0,zero,v0` / `addu s1,s1,v0` @0x800457b4-bc. Satz =
  0x800b2420 + Stimme*18 (@0x80045838-4c); FUN_800458d4 pumpt genau 8 Saetze (`ori s4,zero,0x8` @0x800458dc, Zeiger ab 0x800b24b0,
  `addiu s1,s1,-18` @0x8004590c, `bne s4,zero,0x8004590c` @0x800459e4) -> eine Stimme ausserhalb 0..7 (leerer Satz: -16/-15) wird nie
  gekeyt. VAG-Pruefung in SsUtKeyOnV: `lhu a0,22(v0)` @0x80059f2c, `bne a0,zero,0x80059fa4` @0x80059f8c (sonst Rueckgabe -1). BELEGT.
- **Beta -> Retail (RE2 Leon FUN_8005c040):** der Typ waehlt die Stimme: `lbu v0,8(s3)` @0x8005c0e0, `sltiu v0,v0,0x10` @0x8005c0ec,
  Spieler `xori/sltu` @0x8005c110-18, sonst `addiu s6,zero,3` @0x8005c104 bzw. `addiu s6,zero,2` (Fuss 4) @0x8005c10c; Wasser
  `lh v0,268(s3)` @0x8005c11c -> Boden 26 @0x8005c12c. Zeile: `lhu t0,24(sp)` @0x8005c43c, `andi v1,t0,0x7f` @0x8005c44c,
  `addu s5,s5,v1` @0x8005c458 (a0 + Boden, KEIN Typ-Versatz); leer: `lw v0,0(s4)` / `addiu v1,zero,-1` / `beq v0,v1,0x8005c5b0`
  @0x8005c468-70. Aufrufer FUN_80066afc: Bildwort `lw v0,376(s0)` ... `andi v0,v1,0x4000` @0x80066b48-58, Fuss 4 + 3*Bit 0x2000
  @0x80066b64-74, `jal 0x8005c040` @0x80066b78; `table 0x800a83b8`[4] = 0x80065c88 und `table 0x800a4030`[4] = 0x80065c88. BELEGT.
  (Welche RE2-Typen die Tabelle @0x800a83b8 fuehren, ist auch hier nicht ausgelesen - Ghidra kennt keinen XREF auf die Basis; der Beleg
  "jeder Typ dieselbe Zeile" ist FUN_8005c040 selbst mit seinem eigenen Nicht-Spieler-Zweig. Ausreichend fuer die Regel Beta -> Retail.)
- **Lage (M2/A-E4, FUN_80045a64), Zeile fuer Zeile gegen wassersprung_r41.c gelesen:** Kamera `lh v0,4068(v0)` (DAT_800b0fe4 =
  work_vars[0x0A], re15_scd.h:592) @0x80045a70, `lw v1,36(v1)` (RDT+0x24) @0x80045a94, `sll v0,v0,5` @0x80045a98; |dx|^2 @0x80045aa0-c8,
  |dz|^2 @0x80045ac0-e8, |Kamera.y - y| @0x80045aec-b00; `jal 0x80065f60` (SquareRoot0) @0x80045b04 und @0x80045b10 (a0 = Summe vor dem
  Verzoegerungsplatz `addu s1,v0,zero` -> zweimal dieselbe Wurzel); `subu v1,v1,s0` @0x80045b20; dritte Wurzel @0x80045b34; `lui v1,0x1062`
  / `ori v1,v1,0x4dd3` / `multu` / `mfhi` / `srl s4,v0,4` @0x80045b3c-4c (/250); `slti v0,s4,128` / `ori s4,zero,0x7f` @0x80045b50-5c;
  vier `jal 0x80045d6c` @0x80045b70/88/a0/b8, `subu s0,s0,v0` @0x80045bbc, `subu a0,s1,v0` @0x80045bc0; int16-Vergleich `slt v1,v1,v0`
  @0x80045bd4 -> `addiu v0,s0,4096` / `subu a1,v0,a0` bzw. `subu a1,s0,a0` @0x80045bdc-e8; rel 0 / 0x1000 / 0x800 -> beide 0x89
  @0x80045bf4-c1c; Bit `andi v0,a1,0x800` @0x80045c08; a0/v1 = Bit / nicht Bit @0x80045c28-3c; `addiu v0,a1,-1025` / `andi v0,v0,0xffff`
  / `sltiu v0,v0,0x7ff` @0x80045c40-48; `srl a2,v0,3` @0x80045c54/64; `ori v0,zero,0x89` / `subu a1,v0,a2` @0x80045c78-7c; PAN `addiu
  at,at,18216` (0x80074728) / `lbu s1,0(at)` @0x80045c8c-94 (Index bis 0x89 liest in ATT - der Port fuehrt deshalb EIN 256-B-Feld);
  DAT_800b2826 = Bit ? PAN : 0x89 (`sh a1,10278(at)` @0x80045cd4), DAT_800b2824 = Bit ? 0x89 : PAN (`sh a0,10276(at)` @0x80045ce4);
  ATT `addiu at,at,18344` (0x800747a8) @0x80045cf4, `subu` + `andi v0,v0,0x7f` @0x80045d08-28 und @0x80045d30-3c (kein Deckel; der
  Ueberlauf `& 0x7f` ist der des Originals). FUN_80045d6c: dx == 0 -> `blez a0` 0x400 / 0xc00 @0x80045dd8-e0; `sll a0,a0,12` / `div a0,s0`
  @0x80045d84-88; `jal 0x800658fc` @0x80045db4; `bgez s0` 0x1000/0x800, `subu`, `andi v0,v0,0xfff` @0x80045dc0-d4. catan @0x800658fc:
  x = 0x1000 (`ori v0,zero,0x1000` @0x80065918), `bgez a0` @0x80065930, `srav` je Stufe (Worte 0x00c41807 / 0x00c21007 / 0x00c41007),
  x/y gleichzeitig aus den alten Werten (Felder sp+0/sp+56), `slti v0,a2,12` @0x800659b4, Rueckgabe `lw v0,160(sp)` = z[12] @0x800659c0;
  `read 0x80078c90 12 --w 4` = [511, 302, 159, 81, 41, 20, 10, 5, 3, 1, 0, 0] = W[] im Port. SquareRoot0 @0x80065f60: GTE-LZC
  (Worte 0x4884f000 / 0x4802f800), Tabelle `lh t5,-9852(t5)` = 0x8007d984. BELEGT - die Nachbildung ist Instruktion fuer Instruktion gleich.
- **Pegel (SsUtKeyOnV / SpuVmKeyOnNow):** FUN_800458d4 reicht Satz +14 (= DAT_800b2824) / +16 (= DAT_800b2826) als 7./8. Argument
  (`addiu at,at,9262` @0x8004598c / `sw v0,24(sp)` @0x8004599c; `addiu at,at,9264` @0x800459a4 / `sw v0,28(sp)` @0x800459b8; `jal
  0x80059d3c` @0x800459b4). SsUtKeyOnV: `lw s0,88(sp)` (voll) @0x80059d44, `lw s1,92(sp)` (volr) @0x80059d4c; gleich -> `ori v0,zero,0x40`
  @0x80059e10, Pegel voll @0x80059e20; `slt v0,v1,a0` (volr < voll) @0x80059e0c -> Pan (volr<<6)/voll @0x80059e30-6c, Pegel voll
  @0x80059e64; sonst 0x7f - (voll<<6)/volr, Pegel volr (@0x80059e78-bc, `sb s6,21272(at)` @0x80059eb0). SpuVmKeyOnNow: Tastenpan
  `lbu v1,21273(v1)` @0x80055f50, `sltiu v0,v1,0x40` @0x80055f58 -> a1 * Pan / 63 (`lui v1,0x410` / `ori v1,v1,0x4105` @0x80055f68-6c,
  `srl a1,v1,5` @0x80055f88), sonst a2 * (0x7f - Pan) / 63 @0x80055f8c-b8; a2 = links (`sh a2,0(v0)`, SPU-Stimme +0, @0x80056058),
  a1 = rechts (`sh a1,0(v1)`, +2, @0x80056070). BELEGT. Das SPU-Quadrat (`mult a2,a2` @0x80055fcc / `mult a1,a1` @0x80056008) ist im
  Port fuer alle SE-Pfade linear (vorbestehende Port-Einheit; Dossier N2.7 Punkt 3) -> Hinweis, kein Mangel dieser Spur. Die Schritt-Toene
  der gemessenen Raeume haben Ton-Pan 64 und Programm-Pan 64 (`A41abn2/py/tonpan.py`: ROOM2000/2001 Zeilen 22/26/29, ROOM11C0 Zeile 29),
  der im Port fehlende Ton-/Programm-Pan (@0x80055e68-f48) aendert dort also nichts.
- **Unabhaengige Nachrechnung (eigene Implementierung aus MEINER Lesung oben, nicht das Python des Bau-Agenten):**
  `A41abn2/py/lage_abn2.py` (SquareRoot0 mit LZC + Tabelle @0x8007d984, catan, FUN_80045d6c, FUN_80045a64, PAN/ATT aus PSX.EXE; Kamera aus
  RDT +0x24; Zeile/Quelle aus EDT +0x08 / VH +0x0C; Pegel nach SsUtKeyOnV + Tastenpan) gegen npc.log/debug.log meiner Laeufe (4.3):
  ROOM2000 `Schritte 64: Zeile+Quelle gleich 64/64, Lage+Abstand gleich 64/64, Pegel L/R gleich 64/64 (NPC-Lage-Zeilen 64), L!=R 64`;
  ROOM2001 (Elza/John) 23/23, 23/23, 23/23; ROOM11C0 (Marvin) 8/8, 8/8, 8/8.
- Konstanten des Deltas (re15_wassersprung_r41.h/.c): `_STIMME_BASIS 16` @0x800457ac, `_STIMMEN 8` @0x800458dc, `_LAGE_MITTE 0x40`
  @0x80059e10, `_PAN_TEILER 0x3f` @0x80055f68-6c, k_pan_att = PSX.EXE @0x80074728 (256 B, Riegel `lage` gegen die Datei), W[] @0x80078c90,
  0x89 @0x80045c0c/@0x80045c78, /250 @0x80045b3c-4c, Deckel 0x7f @0x80045b5c, `& 0x7f` @0x80045d28/@0x80045d3c. Alle mit Adresse, alle
  selbst nachgeprueft (oben).

## 3. Punkt 1 — 20B0-Wasserschwall (eigene Messung an der exe; Code seit Abnahme 1 unveraendert, wasser20b0_r41.c 2026-10-23 17:35)
- Laeufe `p20b0` (Leon) / `p20b1` (Elza, `RE15_TITLE_SHOT_CHAR=1` -> `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`), je
  `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=20b0@250 (20b1@250)
  RE15_FX_LOG=fx.log RE15_SPAWN_DIAG=1 RE15_FRAMEDUMP=0-330 RE15_EXIT_AT=340#20b0`, rc 0: `PC loaded room20b0.rdt` / `room20b1.rdt`,
  je 35 x `Sce_em_set type=0x24`, `[scd F105] Cut_chg(7)`, `[scd F146] Cut_chg(15)`, `[scd F228] Cut_chg(6)`, `[scd F242] letterbox
  closed -> gameplay (player_mode=0)`; fx.log je 1735 Zeilen `id=15`.
- **Zeitachse gegen die Original-Savestates (eigene Lesung, `A41abn2/py/orig_takt.py` auf scratchpad/orig/*.sav; Takt n aus der
  EM024-Drift +0x8c = 120 + n bzw. 161 + (+0x8c - 225) nach dem Umbruch)** gegen `RE15_STATE_LOG` des Laufs `p20b0_state`:
  | Takt n | Original (Cut, Spieler x/z, Gier, +4) | Port F = n + 5 (cam, PL x/z, rot, pst) |
  |---|---|---|
  | 1 (tf_5/tf_8) | 11, (2380,-6355), 2048, 1 | F6/F7: 11, (2380,-6355), 2048, 1 |
  | 48 (tf_7) | 13, (-5020,-6355), 2047, 4 | F53: 13, (-5020,-6355), 2047, 4 |
  | 100 (tf_9) | 7, (-15599,-7283), 2120, 4 | F105: 7, (-15599,-7283), 2120, 4 |
  | 131 (ts_10) | 7, (-20308,-7472), 1399, 4 | F136: 7, (-20308,-7472), 1399, 4 |
  | 181 (ts_12) | 15, (-20657,-9629), 2779, 4 | F186: 15, (-20657,-9629), 2779, 4 |
  | 233 (ts_14) | 6, (-20657,-9629), 2779, 1 | F238: 6, (-20657,-9629), 2779, 1 |
  Lauf, Schnitte und Spielerzustand der Szene sind an allen sechs Original-Zeitpunkten gleich (fester Versatz +5 = Sprungbilder).
- **Partikel-Pool:** `A41abn1/pool_vgl.py` (Original-ESP-Pool 0x800a73b8, Stride 0x84) gegen meinen fx.log: tf_7 (Takt 48) `22 aktive
  0x0f-Plaetze` / Port F54 `22 gezeichnete`, (sub, anim) Original = Port = {(0,5):4, (0,8):6, (0,11):4, (0,14):2, (7,7):6}; tf_8 (Takt 1)
  8 / Port F7 8. Der Emitter laeuft 1 Takt hinter dem Spieler (F = n + 6; Abnahme-1-OFFEN 3, Orchestrator 04:01: naechste Runde).
- **Video (Nachtrag 2):** Bogen `A41abn2/bogen_20b0_a.png` (o008/F8, o014/F14, o026/F26, o036/F36, o048/F48) und `bogen_20b0_b.png`
  (o075 ... o240): Wolke an der Osttuer und hinter Leon an gleicher Stelle und in gleicher Form/Textur; ab ~o100 laeuft das VIDEO hinter
  dem Port her (o120 noch im Gang, F120 schon Cut 7; o160 Cut 7, F160 Cut 15). Das ist die Aufnahme: die Savestates zeigen Cut 7 schon bei
  Takt 100 und Cut 15 bei Takt 181 - genau wie der Port (Tabelle oben). Die Videozeitachse ist durch Aufnahmestaus gedehnt (flut_d.txt:
  Aufnahme 50 s, Datei 19,98 s). Massgeblich sind Savestate-RAM und Framebuffer (so auch Abnahme 1, 3.4).
- **Urteil Punkt 1: ERFUELLT** (Original gemessen, Port repariert: Szene, Schnitte, Spielerweg und Partikelpool gleich; Restversatz
  Emitter +1 Takt / erstes Bild nach dem Sprung = OFFEN 1-3, per Orchestrator-Entscheid 04:01 naechste Runde).

## 4. Punkt 2 — Sprung von der Kante ins Wasser; NPCs teilen die Wasser-Effekte (+ Nachtrag 3, E1, A-E4)
### 4.1 Punkt 2a Landung (eigene Laeufe, echter Weg Tuer 20A0 -> 2090 bzw. 20A1 -> 2091)
- `landung_leon`: `RE15_DEBUG_JUMP=20a0@240 RE15_PLAYER_POS=4950,5000,2048,2 RE15_INPUT_SCRIPT=W9,A0.2,W4,R0.37,W0.3,U0.7,W0.2,A0.2,W0.3,
  A0.2,W0.3,A0.2,W0.3,A0.2,W4 RE15_INPUT_SCRIPT_START=0 RE15_CLIMB_TRACE=climb.log RE15_FX_LOG=fx.log RE15_KEIN_GATOR=1
  RE15_FRAMEDUMP=180-215 RE15_EXIT_AT=360#2090`: `[aot] DOOR FIRE slot=0 ... spawn=(6300,-1800,-16250)`, `PC loaded room2090.rdt`,
  climb.log `T459 room=2090 ... pos=(6363,-640,-14032) rot=3072 band=0 sub=10 kind=0x02 ph=4`, fx.log 104 Zeilen `id=6 sub=3` ab F192
  `w(6363,-640,-14032)` `-> sx=96 sy=148`. Bild `A41abn2/landung2090_bogen.png`: Spritzer um Leons Fuesse F196-F202, F206 verklungen.
- `landung_elza` (dieselbe Eingabe, `RE15_TITLE_SHOT_CHAR=1`, `20a1@240`, `#2091`): `Spieler-Familie PL04 ... Elza-Bit=1`, `PC loaded
  room2091.rdt`, `T459 room=2091 ... sub=10 kind=0x02 ph=4`, 104 Zeilen `id=6 sub=3`; Bild `A41abn2/landung2091_bogen.png`.
- `kante2000` (Gehweg -> Kanal ROOM2000, `RE15_PLAYER_POS=-4321,-21100,1024,1 RE15_PRESS=square@60`): `T319 ... pos=(-4321,-640,-22068)
  ... sub=10 kind=0x02 ph=4`, 104 Zeilen `id=6 sub=3` ab F79 (`-> sx=191 sy=168`); Bild `A41abn2/kante2000_bogen.png`: deutlicher
  gruen-weisser Spritzerkranz F81-F89. Mechanik @0x80038724-d0 hat Abnahme 1 selbst disassembliert (unveraendert).
- **2a: erfuellt.**
