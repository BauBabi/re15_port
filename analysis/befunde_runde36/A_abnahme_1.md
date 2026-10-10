# Runde 36 — Spur A "klettern": Abnahme 1 (unabhaengig)

Datum 2026-10-10. Baum `.claude/worktrees/r36_klettern`, Zweig `r36/klettern`, HEAD bcd51fca (Basis master 78a4a748).
Geprueft wird der COMMITTETE Endstand nach "Nachbesserung 1"; am Code wird nichts geaendert. Der Bericht wird
fortlaufend geschrieben (Sitzungslimit).

## 0. Gegenstand

Wortlaut (AUFTRAG.md Z. 31):
> Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B.
> ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich

Abnahme 0 (42d5e7d0): Punkt 1 erfuellt, ein Mangel (widerlegte Null-Box-Aussage zu ROOM3091 obj 1 in Dossier und
Sondenkommentar). Behauptung Nachbesserung 1: Mangel behoben, Engine nur Kommentaraenderungen, neuer Riegel im
Zensus-Pin.

## 1. Diff gelesen

`git log --oneline master..HEAD`: 20 Commits (13 bis Abnahme 0, dann 42d5e7d0 Abnahme 0 und 7 Commits
Nachbesserung 1: 8d7257f4, 85b7d0cf, 1abd2e9b, cec67592, c7bfa539, d263f347, bcd51fca).

`git diff 42d5e7d0..HEAD -- re15_port` (nur die Nachbesserung) selbst gelesen:
- `engine/src/climb_common.c`: 5 Kommentarzeilen (3091 obj 1 jetzt "Null-Box ... trifft FUN_8002da4c ... genau aus
  schraeger Lage (|dx| >= 112 UND |dz| >= 112), nie achsgerade" statt "Tritte/Leitern ... 3091 (obj 1)").
- `engine/src/granate_1150.c:96`, `engine/src/sicherung_1150.c:132`: nur Kommentar (Abnahme 0 H2), Wert 0x0001 unveraendert.
- `tests/unit/probe_r36_klettern_zensus.c`: Kopfkommentar berichtigt; NEU `nullbox_tafel` (12 Punkte),
  `seite_quer` (Anlauf mit Querversatz q, Spieler-Band waehlbar), `nullbox_3091` (Riegel: Tafel 0 Abweichungen,
  achsgerade 4/4 ohne Objekt-Treffer, quer +-300 8/8 Start auf Band 1, quer +-300 vom Boden 8/8 ohne Treffer);
  `setvbuf(stdout, NULL, _IONBF, 0)`.
- `tests/unit/probes/r36_klettern.cmake`: nur Kommentar.

Gesamt-Diff gegen master (`git diff master -- re15_port`): die EINZIGE Verhaltenszeile bleibt climb_common.c:102
`if ((g_scd.props[p].flags & 0x101u) == 0u) continue;  /* @0x8002d358-35c */` (vorher `!= 0x101u`); dazu der
env-gegatete Trace-Zusatz (`t4=`) und der Messhaken `RE15_TITLE_SHOT_CHAR` (main.c) — beide unveraendert seit
Abnahme 0. scd_vm.c, re15_scd.h: nur Kommentare.

## 2. RE-Gate — Stichproben SELBST disassembliert

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE` des Baums
(md5 b55fdaa5b9ec0f84bdd8a6b1107ea0b0 = Hauptbaum; Auslieferungsstand geprueft: @0x80026e4c `08 00 e0 03 21 10 00 00`
= `jr ra` / `addu v0,zero,zero`, NICHT die PATCHED-EXE).

| # | zitiert | selbst gelesen | Urteil |
|---|---|---|---|
| 1 | FUN_8002da4c @0x8002da8c-c0 vorzeichenlos, Bytes `06 00 c2 94 ... 2b 10 45 00 ... 01 00 42 38` | `8002da8c lhu v0,6(a2)` / `8002da90 lhu v1,10(a2)` / `8002da94 addu v0,v0,t1` / `8002da98 addu a1,a1,v0` / `8002da9c sll v0,v0,1` / `8002daa0 sltu v0,v0,a1` / `8002daa4 beq v0,zero,0x8002dab4` / `8002daa8 addu a2,v1,t1` / `8002daac j 0x8002dac4` + `addu v0,zero,zero` / `8002dab4 addu v1,a0,a2` / `8002dab8 sll v0,a2,1` / `8002dabc sltu v0,v0,v1` / `8002dac0 xori v0,v0,0x1`. Bytes ab 8002da8c: `06 00 c2 94 0a 00 c3 94 21 10 49 00 21 28 a2 00 40 10 02 00 2b 10 45 00 03 00 40 10 21 30 69 00 b1 b6 00 08 21 10 00 00 21 18 86 00 40 10 06 00 2b 10 43 00 01 00 42 38`. Davor dx/dz = p - (box.c + obj.pos) (`lh 0/4(a2)`, `lw 52/60(t0)`), Band-Gate `8002da58 bne v0,a3`. | stimmt |
| 2 | Rand -112 im Delay-Slot @0x8002d398 (`90 ff 06 24`); Band-Gate @0x8002d370; Sperre @0x8002d2f0-f4 | Bytes @0x8002d398 `90 ff 06 24` = `addiu a2,zero,-112` (Delay-Slot von `8002d394 jal 0x8002da4c`, a3 = `lbu 130(s0)`). `8002d364 lbu v1,0(s3)` (s3 = 0x800aca3c+154 = 0x800acad6) / `8002d368 lbu v0,130(s0)` / `8002d370 bne v1,v0,0x8002d3a4`. `8002d2e8 lw v0,0(a0)` (a0 = 0x800aca3c) / `8002d2f0 andi v0,v0,0x4000` / `8002d2f4 beq v0,zero,0x8002d310` sonst `8002d2fc j 0x8002d3b4` mit v0 = 0. Kern-Gate erneut: `8002d358 andi v0,v0,0x101` / `8002d35c beq v0,zero,0x8002d3a4` (Ziel = `addiu s1,s1,-1` / `bgez` = naechstes Objekt). | stimmt |
| 3 | Standtest @0x80031ce8-d24, Loeschen @0x80031cd0-e4, Schub @0x80031cbc davor | `80031cbc jal 0x8002b544`; `80031cd0 addiu v1,zero,-16385` (= ~0x4000) / `80031cd8 sw zero -> 0x800ac788` / `80031cdc and v0,v0,v1` / `80031ce4 sw v0 -> 0x800aca3c`; `80031ce8 jal 0x8002d100` + Delay `ori a1,zero,0x12`; `80031cfc lbu v0,130(a0)` / `80031d08 addiu v0,v0,1` / `80031d0c bne v0,v1` (Objekt-Band+1 == Spieler-Band 0x800acad6); `80031d20 sw a0 -> 0x800ac788` / `80031d24 ori v0,v0,0x4000` / `80031d2c sw -> 0x800aca3c`; danach `80031d38 jal 0x8002dc48`. | stimmt |
| 4 | Aufstieg y -= 1800 @0x800381e4-ec, Band+1 @0x80038200-08; Sonde {800,0} DAT_800109c8 @0x8002d4a0-bc | `800381e4 addiu v0,v0,-1800` / `800381ec sw v0 -> 0x800aca8c`; `80038200 addiu v1,v1,1` / `80038208 sb v1 -> 0x800acad6`. `8002d498/9c a1 = 0x800109c8`, `8002d4a0-bc` lwl/lwr/swl/swr (Disassembler: op22/op26/op2a/op2e), Bytes @0x800109c8 `20 03 00 00 00 00 00 00` = {800, 0}; `8002d4d4 addiu v0,zero,-1024` / `8002d4d8 addiu a0,a0,512` / `8002d4dc and` = Kurs auf 90 Grad gerastet. | stimmt |
| 5 | Bit 0 bei jeder Installation @0x80040998, Zweitinstallation 0 @0x8004098c | `80040978 lw v0,0(a1)` / `80040980 beq v0,zero,0x80040990` / `8004098c sw zero,0(a1)` (Delay von `j 0x800409a8`) / `80040990 lhu v0,6(a2)` / `80040998 ori v0,v0,0x1` / `800409a4 sw v0,0(a1)` | stimmt |

**Eigene Nachrechnung der Wahrheitstafel** (eigenes Skript `scratchpad/abn1_A/tafel_eigen.py`, Emulation genau der
Instruktionen aus Zeile 1, unabhaengig vom Skript des Bau-Agenten): die 12 Punkte des Riegels + 4 weitere
(111/-112, -111/-111 -> 0; 30000/-30000, -32768/32767 -> 1) wie die geschlossene Form; 1D-Brute-force je Achse
ueber [-40000, 40000] bei h 0 / Rand -112: **0 Abweichungen** von "Treffer <=> |d| >= 112"; fuer 0 < h < 112
(h 1/45/90/111): 0 Abweichungen von "|d| >= 112 - h" (Dossier-Aussage zu den 14 Records stimmt); h >= 112: 0
Abweichungen von "|d| <= h - 112" (Normalfall). Port `re15_collision.c:300-312` rechnet dieselbe u32-Folge
(`(hx<<1) < dx + hx` -> 0; Z mit `? 0 : 1`).

## 3. Bau und Tests (Baum r36_klettern, HEAD bcd51fca)

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> EXIT 0,
  `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`. Konsistenz: `git status` leer; climb_common.c 19:18:07 <
  climb_common.c.obj 19:19:44 < re15_pc.exe 19:19:45; granate_1150.c/sicherung_1150.c 19:18:09/11 < .obj 19:19:44;
  probe_r36_klettern_zensus.c 19:26:00 < .obj/.exe 19:32:43 (letzter Commit bcd51fca 19:33:17 aendert keine Quelle mehr).
- Gezielt, schnelle Gruppe (Spur + jedes vom Diff beruehrte Modul: Klettern/1090/10F0, granate, sicherung, Null-Box-
  Module irons_tisch/hebetisch/plattform/dokumente/inhalt, Titel-Pfad main.c):
  `ctest -R "^(unit_r36_klettern_.*|unit_1090_.*|unit_rvd_scan_ungated_by_climb|unit_re2_room1090_ab|unit_r20_birkin_push_pin|
  unit_r22_10f0_figur|unit_r35_cut10f0_.*|unit_r35_inventar1050_kiste|unit_stair_1170_regression|unit_stair_hwtruth_1060|
  unit_r30_granate|unit_r35_granate|unit_sicherung_1150|unit_r30_sicherung_.*|unit_room1050_sicherung|unit_r30_irons_tisch|
  unit_r30_irons_tisch_licht|unit_r31_hebetisch|unit_r32_hebetisch_faecher|unit_r34_plattform|unit_r34_plattform_ton|
  unit_r34n_e_dokumente|unit_r35_inhalt_.*|r30_titel_blinken|unit_enemy_body_push|unit_plc_back_yaw_1090|unit_map_1090)$"`
  -> **49/49 bestanden** (10,00 s).
- Langlaeufer: siehe Abschnitt 7 (integration_r36_klettern u.a.).

## 4. Messung des Nutzer-Punkts an der exe (eigene Laeufe, eigene Eingaben)

Messkopie `re15_port/build/platform/pc/re15_pc_abn1.exe`, vor JEDEM Lauf frisch kopiert und per md5 gegen die
gebaute exe geprueft (`74a17d614423a4502cfd9be8b8bca52c`); Laufskript `scratchpad/abn1_A/lauf.sh`, Arbeitsordner
`scratchpad/abn1_A/laeufe/<lauf>` (env.txt, stderr.txt, debug.log, climb.txt). Basis-Umgebung je Lauf:
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_CLIMB_TRACE=climb.txt`. Raumdaten selbst gelesen
(`re15_port/tools/scd_dump_room.py ROOM10F0.RDT`): Tritt obj 0 Typ 4 sub00 @0x010F4 (Startlage (-6050,0,4950)) /
@0x0111A (Endlage (-4850,0,-850)), Box c(50,-900,0) h(675,900,600), pc[6..7] `00 00` -> Flags 0x0001; obj 1-3 Typ 0,
Band 1, Null-Box, pc[6..7] `0a 00` -> 0x000B; Tuer nach 1090 Slot 1 @0x00F52 floor 1 Rechteck (-5350,-2250,1200,1000).

**E1 — Startlage, Anlauf von der -Z-Seite (Abnahme 0 kam von Osten), hoch, ueber die Flaeche, Absprung an der +Z-
Kante, umdrehen, von +Z erneut hoch** — `RE15_DEBUG_JUMP=10f0@250 RE15_SET_FLAG=9:71 RE15_PLAYER_POS=-6000,3900,3072
RE15_INPUT_SCRIPT=W8,A0.1,W3,U0.5,W0.5,A0.1,W3,R0.74,W0.5,A0.1,W3 RE15_EXIT_AT=760#10f0` (exit 0, 34 s):
```
T492 room=10F0 pad=8000 pos=(-6000,0,3900) rot=3072 band=0 sub=9 kind=0x81 ph=0 obj=3 stand=-1 ... t4pos=(-6050,4950) t4fl=0x0001   <- Klettern (von -Z)
T523 room=10F0 pos=(-6000,-1800,5048) rot=3072 band=1 sub=9 ph=3 obj=3 stand=3                                          <- y -1800, Band 1
T544 room=10F0 pos=(-6000,-1800,5048) band=1 sub=0 stand=3                                                              <- oben
T585..T600 pos=(-6000,-1800,5123..5325) band=1 stand=3 mo=105                                                           <- Gang zur +Z-Kante
T615 pad=8000 sub=9 kind=0x02 -> T616 sub=10 -> T630 pos=(-6000,-1210,6125) band=0 -> T634 y=0 -> T654 sub=0          <- Absprung + Landung
T745 room=10F0 pad=8000 pos=(-6000,0,6125) band=0 sub=9 kind=0x81 obj=3 -> T748 rot=1024                               <- erneut Klettern (von +Z)
T778 pos=(-6000,-1800,4977) band=1 stand=3 -> T799 sub=0 stand=3                                                        <- wieder oben
```

**E2 — ECHTER WEG mit echter Tuer und EIGENER Schiebe-Reihenfolge** (Integrationstest und Abnahme 0: erst +X, dann
-Z; hier erst -Z, dann +X). Die Tuer 10D0 -> 10F0 haengt im Original an (3,50) (Abnahme 0 §3), deshalb
`RE15_SET_FLAG=3:50,9:71`. `RE15_DEBUG_JUMP=10d0@250 RE15_PLAYER_POS=1700,-7000,2048 RE15_INPUT_SCRIPT=W11,A0.1,W1,
<Flurweg wie Integrationslauf B bis (-7332,1441)>,XU0.78,W0.5,R0.37,W0.4,U0.66,W0.4,R0.37,W0.5,U11,W1,R0.37,W0.4,U0.56,
W0.4,L0.37,W0.4,U0.52,W0.4,L0.37,W0.5,U2.6,W1,A0.1,W2.5,R0.37,W0.4,U0.6,W0.3,A0.1,W3 RE15_EXIT_AT=60#1090`
(exit 0, 90 s; vorab Entwicklungslauf E2dev per Sprung mit derselben Reihenfolge: Tuer bei T1217, 1090 bei T1218):
```
[tuer] Sequenz Archiv 2 DOOR13 Variante 1 ...   -> [room] PC loaded room10f0.rdt
T583  room=10F0 pos=(8400,0,-350) rot=2048 band=0 ... t4pos=(-6050,4950) t4fl=0x0001     <- durch die Tuer, Tritt in der Startlage
T1449 room=10F0 pos=(-7125,0,6018) ... T1507 pos=(-5645,0,6000) ... rot=5216 (= -Z)     <- eigener Weg an die +Z-Seite des Tritts
T1568 room=10F0 pos=(-5744,0,6000) push=5 -> T1853 t4pos=(-6050,-1108)                   <- Tritt nach -Z geschoben (bis an die Wand)
T1945 pos=(-7350,0,61) -> T1996 pos=(-7335,0,-1123) -> T2019 rot=4095 (= +X)              <- aussen herum an die Westseite
T2062 room=10F0 pos=(-7125,0,-1123) push=5 -> T2112 t4pos=(-5026,-1108)                  <- Tritt nach +X geschoben
T2142 room=10F0 pad=8000 pos=(-6101,0,-1171) band=0 sub=9 kind=0x81 obj=3                <- Klettern
T2173 room=10F0 pos=(-4953,-1800,-1171) band=1 stand=3 -> T2194 sub=0 stand=3             <- oben
T2231 rot=1056 -> T2261 pos=(-5007,-1800,-1332) band=1 stand=3                            <- oben nach -Z in die floor-1-Zone der Tuer
[tuer] Sequenz Archiv 2 DOOR1E ...  -> T2271 room=1090 pos=(-13600,-9000,1300) band=5    <- ROOM1090 erreicht
[flow] EXIT_AT: Bild 60 in Raum 1090 erreicht -> exit
```

**Integrationslauf selbst gelesen** (`ctest -R "^integration_r36_klettern$" --timeout 1500` -> `Passed 521.84 sec`,
Arbeitsordner `build/tests/integration/r36_klettern_wd/lauf_d3909eee_*`):
```
A_leon_10f0 : T252 t4pos=(-6050,4950) | T572 push | T1121 pos=(-4899,0,-58) sub=9 kind=0x81 obj=3 | T1173 pos=(-4899,-1800,-1206) band=1 sub=0 stand=3 | PL00 | EXIT_AT: Bild 1400 in Raum 1090
B_elza_10f1 : T32 room=10F1 (8400,0,-350) | T590 push | T1137 sub=9 kind=0x81 | T1189 pos=(-5015,-1800,-1174) band=1 stand=3 | PL04 (character=4, Elza-Bit=1) | EXIT_AT: Bild 1320 in Raum 1091
C_leon_szene: "[cut10f0] ROOM10F0: Szene gestartet" + "Szene zu Ende" | T3754 push | T4299 sub=9 kind=0x81 | T4351 band=1 stand=3 | EXIT_AT: Bild 4550 in Raum 1090
```

## 5. NEUER BEFUND: Zensus-Pin faehrt fuer ROOM11A0/11A1 die zweite Lage NICHT

Beim Lesen der Zensus-Ausgabe (`A_belege/zensus_sonde_nachher.txt`, je Raum/Flag die erste Seite):
```
--- ROOM11A0 flag(5,6)=0:  ROOM11A0 flag=0 obj=0(prop 0) fl=0x0001 band=3 pos=(10050,-5400,8800)
--- ROOM11A0 flag(5,6)=1:  ROOM11A0 flag=1 obj=0(prop 0) fl=0x0001 band=3 pos=(10050,-5400,8800)   <- dieselbe Lage!
--- ROOM11A1 flag(5,6)=0:  ROOM11A1 flag=0 ... band=3 pos=(10050,-5400,8800)
--- ROOM11A1 flag(5,6)=1:  ROOM11A1 flag=1 ... band=3 pos=(10050,-5400,8800)                        <- dieselbe Lage!
(zum Vergleich: ROOM20A0 flag=0 band=4 pos=(12250,-7200,-12400) / flag=1 band=0 pos=(23150,0,-24400);
 ROOM5020 flag=0 band=4 (-11000,-7200,-23150) / flag=1 band=0 (-3400,0,-25300); 10F0/2060 ebenso verschieden)
```
Ursache (selbst gelesen): `reset_room` (probe_r36_klettern_zensus.c:112-125) setzt das Lage-Flag (`re15_game_flag_set`,
Z. 117) VOR `scd_room_reenter` (Z. 123); die Raum-Init nullt Bank 5 Wort 0 (`g_game.flags[5][0] = 0`,
scd_room_setup.c:297, byte-true FUN_8003ecec @0x8003ed74 `sw zero,0x800b1028`). Bank 3/4 (10F0, 2060, 20A0, 5020)
ueberleben, Bank 5 (11A0/11A1, Flag (5,6)) nicht. Die Lage (5,6)=1 = Band 0 (-19300,0,3300) (main00 @0x0107E, selbst
gelesen: `2d 00 04 00 00 00 00 00 00 00 9c b4 00 00 e4 0c ...`) wurde also NIE gefahren; der Riegel prueft nicht, dass
die gesetzte Lage wirkt (keine Soll-Position je Lage), und meldet trotzdem "OBEN".

Damit sind im Dossier falsch: Zensus-Pin-Tabelle Zeile 11A0/11A1 ("1 -> Band 0 (-19300,0,3300) @0x0107E" als gemessen),
"Zensus: 24/24 Objekte (Raum x Lage) kletterbar" (tatsaechlich 22 verschiedene Raum-x-Lage-Faelle, 2 Wiederholungen),
"Damit ist jedes Objekt der betroffenen Klasse in beiden Szenarien und beiden Lagen gemessen", "Fuer den Nutzer: ...
jetzt alle gemessen kletterbar (Zensus-Pin)"; ebenso die Testbeschreibung `unit_r36_klettern_zensus` ("je Lage (Flag
0/1)") und die Commit-Message fd9c0f48 ("je Lage per Flag ... (5,6) ... 24/24"). Spielseitig: wegen desselben Wischs
steht die Kiste beim Betreten von 11A0/11A1 immer in der Band-3-Lage @0x01058; (5,6) setzt erst sub01 @0x012B2-c4
(`Ck 5,6,0` / `Work_set 3,0` / `Member_cmp 15 == 6` / `Set 5,6,1`), wenn die Kiste die Schiebezone Slot 6 (sce 5,
floor 0, (-20150,2350,2050,2050) @0x01044) erreicht — die Band-0-Lage entsteht im Spiel nur dynamisch durch Schieben
innerhalb eines Besuchs, und genau die ist weder in der VM noch an der exe gemessen.

Wisch selbst disassembliert: `8003ed74 sw zero,4136(at)` (= 0x800b1028, Bank 5 Wort 0) in FUN_8003ecec; Aufruf
`8003ef84 jal 0x8003ecec` in FUN_8003ef6c (davor `8003ef7c jal 0x8003ebf4`), FUN_8003ef6c gerufen von der
Raumladekette `80039a00 jal 0x8003ef6c`; erst danach liest FUN_8003ef6c den Raumkopf (`8003ef90 lw v0,-14472(v0)` =
0x800ac778) und startet die SCD. Der Port bildet das ab (scd_room_setup.c:297). Ein Laufzeit-Bandwechsel eines
Objekts existiert im Port nicht (Suche nach Schreibzugriffen auf `.band` in engine/src: nur die Installationsstellen
scd_vm.c:4218, dokumente_r34.c:81, granate_1150.c:84, inhalt_r35.c:64, irons_tisch_1150.c:63, sicherung_1150.c:116;
Member_set auf Objekte schreibt nur x/y/z/rot, scd_vm.c:3403-3420). Im Port steht die 11A0/11A1-Kiste also in jedem
Besuch auf Band 3. Diese Lage ist gemessen: VM 4/4 Seiten (Zensus) und an der exe (E3 unten). Ob das Original die
Kiste dynamisch auf Band 0 bringt, ist ungemessen.

Eigener Lauf der Sonde am aktuellen Bau (`build/tests/unit/probe_r36_klettern_zensus.exe pin`, exit 0,
`scratchpad/abn1_A/zensus_eigenlauf.txt`): dieselben vier 11A0/11A1-Zeilen, `Zensus: 24/24 Objekte (Raum x Lage)
kletterbar`, `PASS`. Der Befund ist reproduziert.

**E3 — ROOM11A0 ("SEWER EXIT", Leons Stage 1), Kiste in der erreichbaren Band-3-Lage, an der exe:**
`RE15_DEBUG_JUMP=11a0@250 RE15_PLAYER_POS=10050,7450,3072,3 RE15_INPUT_SCRIPT=W8,A0.1,W4 RE15_EXIT_AT=500#11a0`
(exit 0, 25 s; Spawn auf Band 3, Bodenhoehe dort `rc_b8=-5400` = echter Band-3-Boden):
```
T252 room=11A0 pos=(10050,-5400,7450) rot=3072 band=3 sub=0 ... rc_b8=-5400 t4=0 t4pos=(10050,8800) t4fl=0x0001
T492 room=11A0 pad=8000 pos=(10050,-5400,7450) band=3 sub=9 kind=0x81 ph=0 obj=0                         <- Klettern
T523 room=11A0 pos=(10050,-7200,8598) band=4 sub=9 ph=3 obj=0 stand=0                                    <- y -7200, Band 4
T544 room=11A0 pos=(10050,-7200,8598) band=4 sub=0 stand=0  (bis Laufende T634 unveraendert oben)       <- oben
```

## 6. Gegenprobe ohne Fix (eigene, gleiche Eingabe wie E1)

exe des Hauptbaums (`re15_port/build/platform/pc/re15_pc.exe`, 2026-10-04 18:37:29, md5 2fd81d20...; master 78a4a748
enthaelt den Fix nicht, `git merge-base --is-ancestor 1c926683 master` = nein, Hauptbaum-climb_common.c traegt noch
`0x101u) != 0x101u`), nur in den Spurbaum kopiert (`re15_pc_abn1_vorher.exe`), Eingabe und Umgebung exakt wie E1:
```
T492 room=10F0 pad=8000 pos=(-6000,0,3900) rot=3072 band=0 sub=0 kind=0x00 ph=0 obj=-1 stand=-1     <- Aktionstaste, KEIN Klettern
grep -c "sub=9" climb.txt = 0 (von 1011 Zeilen)
```
-> Gleiche Eingabe: ohne Fix kein Start, mit Fix Substate 9 / KIND 0x81 (E1 T492). Der Fix (eine Gate-Zeile,
@0x8002d358-35c selbst disassembliert) erklaert den Befund.

## 7. Langlaeufer und Module des Diffs

- `integration_r36_klettern` **Passed 521,84 s** (Abschnitt 4, Arbeitsordner selbst gelesen).
- `ctest -R "^(integration_r35_cut10f0|integration_r30_granate_laden|integration_r30_sicherung_laden|
  integration_r30_sicherung_bild|integration_r30_titel_puls)$" --timeout 900` -> **5/5 bestanden** (r30_granate_laden
  84,18 s, r30_sicherung_laden 36,59 s, r30_sicherung_bild 10,87 s, r30_titel_puls 24,52 s, r35_cut10f0 240,50 s;
  396,69 s gesamt). Grund der Auswahl: granate_1150.c/sicherung_1150.c (Kommentar), Titelpfad main.c (Messhaken
  `RE15_TITLE_SHOT_CHAR`), Aktionstaste in ROOM10F0 (Runde-35-Szene).
- Kein roter Test in meinen Laeufen, kein Nachfahren noetig. Keine volle Suite im Spurbaum (Auftrag).

## 8. Weitere Nachpruefungen der Nachbesserung

- **Berichtigte Aussage 3091 stimmt jetzt** (Mangel 1 der Abnahme 0): ROOM3091 obj 1 @0x01508 selbst gelesen
  (`2d 01 04 00 01 00 80 00 ...` = Typ 4, Band 1, pc[6..7] `80 00` -> Flags 0x0081, Box c(0,-1800,0) h(0,0,0), Lage
  (-15072,950,-1010)); obj 0 @0x01448 Typ 4, Band 0, `00 01` -> 0x0101, (-23550,0,-8024); AOT-sce in 3091 nur 0/1/2/5
  (keine Treppen 12/13). Stichprobe Tueren nach 3091: ROOM3060 @0x00E96 und ROOM30D0 @0x01266 — Bytes [16..17] next_y
  `00 00`, next_floor 0, Ziel Stage-Index 02 / Raum 09. Null-Box-Riegel in meinem eigenen Sondenlauf: Tafel 12/12,
  achsgerade 4/4 `boxhit(-112)=0 try=0`, quer +-300 auf Band 1 8/8 `try=1 sub=9 kind=0x81 obj=1 ende=51
  end=(..,-3600,..) band=2 stand=-1`, quer +-300 vom Boden 8/8 `boxhit(-112)=1 try=0` (Band-Gate). Der Riegel misst,
  was er behauptet.
- **Standtest-Rand 0x12** (Grundlage "kein Standobjekt nach dem Null-Box-Aufstieg"): FUN_8002d100 selbst gelesen —
  Schleife ueber Objekte mit `andi v0,v0,0x1` @0x8002d170, `jal 0x8002da4c` @0x8002d188 mit Rand = a1 (Delay-Slot
  `addu a2,s5,zero` @0x8002d18c, a1 = 0x12 vom Aufrufer @0x80031cec); bei h 0 Treffer nur fuer |d| <= 18. Stimmt.
- **H2 erledigt**: granate_1150.c:96 / sicherung_1150.c:132 beschreiben jetzt das aktive Gate; beide Objekte Band 1
  (granate_1150.c:84, sicherung_1150.c:116) — "nur fuer einen Spieler auf Band 1 ohne Standobjekt" stimmt.
- **H1 im Dossier** unter OFFEN mit Adressen und Messweg (DuckStation, Auslieferungs-EXE, 0x800aca8c/0x800acad6/
  0x800ac788) — wie in Abnahme 0 verlangt.
- **Commit-Messages** der Nachbesserung (85b7d0cf, 1abd2e9b, cec67592, c7bfa539, bcd51fca) zitieren ihre Adressen
  (@0x8002da8c-c0 mit Bytes, @0x8002d398, @0x8002d370, @0x80031d24, @0x8002d2f0-f4, @0x800381e4, @0x80038200-08,
  @0x80040998, @0x8002d358-35c); 8d7257f4/d263f347 sind reine Dossier-Commits.

## 9. Vertrag und Dateien

- `git diff master --name-only` (ohne analysis/): climb_common.c, granate_1150.c, scd_vm.c, sicherung_1150.c,
  re15_scd.h, platform/pc/main.c, tests/integration/test_r36_klettern.cmake, tests/unit/probe_r36_klettern_10f0.c,
  tests/unit/probe_r36_klettern_zensus.c, tests/unit/probes/r36_klettern.cmake, tests/unit/test_1090_climb_pin.c.
  Keine release/, platform/android/, shared_assets/PSX/, tests/*/CMakeLists.txt, RELEASE_NOTES.
- Keine Bank-9-Bits, Nachrichten-IDs, Ereignisse: die einzigen `re15_game_flag_set` im Diff stehen in den Testsonden
  und setzen Original-Lageflags ((3,102) bzw. die Zensus-Flags).
- Signalwoerter (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vorlaeufig|geschaetzt`) in den
  hinzugefuegten Zeilen unter re15_port: keine.
- Gemeinsame Dateien: die Nachbesserung aendert dort nur Kommentare. Konflikte: `git merge-tree --write-tree` gegen
  jeden r36-Zweig, der dieselben Dateien beruehrt (r36/kleidung main.c; r36/szenenleon scd_vm.c + main.c; r36/zombieki
  main.c): rc 0, kein Konflikt. Kein anderer r36-Zweig beruehrt granate_1150.c, sicherung_1150.c, climb_common.c,
  re15_scd.h oder test_1090_climb_pin.c.

## 10. Urteil je Punkt

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1. "kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmoeglich" | **erfuellt** | E1 (10F0 Startlage, Anlauf von -Z und von +Z, Absprung), E2 (echte Tuer 10D0 -> 10F0, eigene Schiebe-Reihenfolge -Z/+X, Klettern, oben, Tuer -> ROOM1090), E3 (11A0 Kiste Band 3 an der exe), integration_r36_klettern A/B/C selbst gelesen (Leon 10F0 x2, Elza 10F1 -> 1091 mit PL04, Szene davor), Gegenprobe V1 ohne Fix: kein Start. Fix byte-true @0x8002d358-35c. |

Einschraenkung zum Plural "Leitern/Tritte": an der exe gemessen sind 10F0 (E1/E2, Integration A/C), 10F1 (Integration
B), 11A0 Band 3 (E3), 2060 (Abnahme 0); die uebrigen Raeume (2040, 20A0, 5020 + Elza-Spiegel) nur in der VM (Zensus).
Elza betritt 10F1 im Integrationslauf ueber `RE15_GOTO_ROOM` (Tuer-Spawn-Tabelle), nicht durch Gehen durch die Tuer
in 10D1; einen Positions-Haken nach GOTO_ROOM gibt es nicht, und der Debug-Sprung landet als Elza in Leons Raum
(Spur I). Die Kletter-Logik haengt nicht am Charakter (eine Gate-Zeile, Tritt-Record 10F1 = 10F0).

## 11. Maengel

1. **Zensus-Pin behauptet eine Messung, die er nicht macht (ROOM11A0/11A1, Lage (5,6)=1).** Nachpruefbar:
   `build/tests/unit/probe_r36_klettern_zensus.exe pin` -> die Zeilen `ROOM11A0 flag=1 ... band=3 pos=(10050,-5400,8800)`
   und `ROOM11A1 flag=1 ... band=3 pos=(10050,-5400,8800)` sind identisch mit flag=0; erwartet waere Band 0
   (-19300,0,3300) (main00 @0x0107E). Ursache: `reset_room` setzt das Flag (probe_r36_klettern_zensus.c:117) vor
   `scd_room_reenter` (Z. 123), die Raum-Init nullt Bank 5 Wort 0 (scd_room_setup.c:297 = @0x8003ed74, selbst
   disassembliert); der Riegel prueft die Lage nicht (keine Soll-Position je Flag, Z. 174-175), zaehlt die
   Wiederholung mit und meldet `24/24 Objekte (Raum x Lage)`. Damit falsch: Dossier-Tabelle Zeile 11A0/11A1 ("1 ->
   Band 0 (-19300,0,3300) @0x0107E" als gemessen), "24/24", "Damit ist jedes Objekt der betroffenen Klasse in
   beiden Szenarien und beiden Lagen gemessen", "Fuer den Nutzer: ... jetzt alle gemessen kletterbar", Testbeschreibung
   im Dossier ("je Lage (Flag 0/1)"), `tests/unit/probes/r36_klettern.cmake:22-26` ("je Lage (Flag 0/1) ... 11A0/11A1
   (Kiste, (5,6))") und der Sondenkopf `probe_r36_klettern_zensus.c:11/21` ("Lage per (5,6)", "Je Raum, je Lage (Flag
   0/1) ... werden die vier Anlaufseiten gefahren"). Abhilfe (kein Engine-Code noetig): (a) Aussagen
   berichtigen — tatsaechlich 22 verschiedene Raum-x-Lage-Faelle; fuer 11A0/11A1 belegen, dass @0x0107E beim Laden nie
   installiert wird (Bank-5-Wisch @0x8003ed74 vor dem SCD-Start @0x80039a00/@0x8003ef84) und die Band-0-Lage nur
   dynamisch ueber sub01 @0x012B2-c4 entsteht (im Port mangels Laufzeit-Bandwechsel gar nicht); (b) im Riegel je Lage
   die installierte Position gegen den RDT-Record pruefen, damit ein wirkungsloses Lage-Flag rot wird; (c) die Lage
   entweder wirklich fahren (Flag NACH dem Wisch setzen und main00 erneut laufen lassen) oder ausdruecklich als beim
   Laden unerreichbar ausnehmen.

## 12. Hinweise (kein Mangel der Spur)

- **H1 (unveraendert OFFEN, im Dossier gefuehrt):** ROOM2060 Spinne schiebt Leon nach dem Aufstieg von der Trittflaeche
  (Abnahme 0 §7); Reihenfolge @0x80031cbc vor @0x80031ce8 selbst nachgelesen (Abschnitt 2, Zeile 3); Erreichbarkeit im
  Original ungemessen.
- **H3 (unveraendert OFFEN):** Null-Box-/Kleinbox-Objekte auf Spieler-Band ohne Original-Abgleich; meine Stichproben der
  statischen Erreichbarkeit (3091) bestaetigen die Dossier-Aussagen.
- **H4:** climb_common.c traegt weiterhin mehr als 5 geaenderte Nicht-Kommentar-Zeilen (1 Logikzeile + env-gegateter
  Trace); die Nachbesserung fuegt dort nur Kommentar hinzu.
- **FUN_8002dc48** (Herunterfall-Substate 0xd, nur 0x100-Objekte) bleibt unportiert — ausserhalb des Wortlauts, im
  Dossier unter OFFEN.
- **Messkopien:** `re15_pc_abn1.exe` und `re15_pc_abn1_vorher.exe` lagen nur im Spurbaum (build/, ignoriert) und sind
  nach den Laeufen entfernt; im Hauptbaum wurde nichts geschrieben.

## 13. Ergebnis

**bestanden = NEIN** — wegen Mangel 1 (der Zensus-Pin und das Dossier geben fuer ROOM11A0/11A1 eine zweite Lage als
gemessen aus, die nie gefahren wurde; der Riegel erkennt das nicht). Der Nutzer-Punkt selbst ist **erfuellt**: eigener
echter Weg 10D0 -> 10F0 -> Schieben in eigener Reihenfolge -> Klettern -> Tuer nach ROOM1090 (E2), weitere Seiten (E1),
ein zweiter Stage-1-Raum an der exe (E3), Gegenprobe ohne Fix ohne Start (V1), Integrationslauf A/B/C bestanden und
selbst gelesen. Der Mangel 1 der Abnahme 0 ist behoben (Aussage berichtigt, Wahrheitstafel selbst nachgerechnet, Riegel
misst). RE-Gate: 6 Stellen selbst disassembliert, alle wie zitiert. Vertrag eingehalten.

## 14. Messprotokoll

| Lauf | Zweck | Kern-Umgebung | Ergebnis |
|---|---|---|---|
| E1 | 10F0 Startlage, Anlauf -Z, Absprung +Z, erneut von +Z | DEBUG_JUMP=10f0@250, SET_FLAG=9:71, PLAYER_POS=-6000,3900,3072, Skript W8,A0.1,W3,U0.5,W0.5,A0.1,W3,R0.74,W0.5,A0.1,W3 | T492 sub=9/0x81, T523 y -1800 Band 1, T615-T654 Absprung, T745 erneut sub=9, T778/T799 oben |
| E2dev | eigene Schiebe-Reihenfolge, per Sprung | DEBUG_JUMP=10f0@250, PLAYER_POS=-6000,6000,1024 | -Z T512-T798, +X T1008-T1059, Klettern T1089, Tuer T1217 -> 1090 T1218 (exit 124: EXIT_AT nannte 10F0) |
| E2 | echter Weg mit echter Tuer, eigene Reihenfolge | DEBUG_JUMP=10d0@250, SET_FLAG=3:50,9:71, PLAYER_POS=1700,-7000,2048, EXIT_AT=60#1090 | DOOR13 -> T583 10F0, -Z T1568-T1853, +X T2062-T2112, T2142 sub=9/0x81, T2173 oben, T2271 room=1090, EXIT_AT Bild 60 in 1090 |
| E3 | 11A0 Kiste Band 3 | DEBUG_JUMP=11a0@250, PLAYER_POS=10050,7450,3072,3 | T492 sub=9/0x81 obj=0, T523 y -7200 Band 4 stand=0, T544 oben |
| V1 | Gegenprobe ohne Fix | Hauptbaum-exe 2026-10-04, Eingabe wie E1 | T492 pad=8000 sub=0 obj=-1, 0 Zeilen sub=9 |
| Z | Zensus-Sonde selbst | probe_r36_klettern_zensus.exe pin | PASS, 24/24 gemeldet, 11A0/11A1 flag=1 = flag=0 (Mangel 1), Null-Box 12/12, 4/4, 8/8, 8/8 |

Alle exe-Laeufe: Messkopie im Spurbaum, Arbeitsordner `scratchpad/abn1_A/laeufe/<lauf>` (env.txt, climb.txt,
debug.log, stderr.txt); eigene Nachrechnung `scratchpad/abn1_A/tafel_eigen.py`.
