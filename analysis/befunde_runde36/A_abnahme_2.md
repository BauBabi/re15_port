# Runde 36 — Spur A "klettern": Abnahme 2 (unabhaengig)

Datum 2026-10-10 (Beginn 20:21). Baum `.claude/worktrees/r36_klettern`, Zweig `r36/klettern`, HEAD 9db621d7 (Basis
master 78a4a748). Geprueft wird der COMMITTETE Endstand nach "Nachbesserung 2"; am Code wird nichts geaendert. Der
Bericht wird fortlaufend geschrieben (Sitzungslimit). Eigene Skripte/Logs: Scratchpad `abn2_A/` (Pfade unten).

## 0. Gegenstand

Wortlaut (AUFTRAG.md Z. 31):
> Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B.
> ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich

Abnahme 1 (5f4e3ae1): Punkt 1 erfuellt, EIN Mangel (Zensus-Pin gab fuer ROOM11A0/11A1 die Lage (5,6)=1 @0x0107E als
gemessen aus, fuhr sie aber nie). Behauptung Nachbesserung 2: Mangel behoben, kein Engine-Code geaendert, Sonde je
Record mit Lage-Pruefung, 11A0/11A1 @0x0107E erzwungen + getrennt gezaehlt, Gegenprobe; dazu neue RE-Belege 1-4
(Bank-5-Wisch, ROOM11A0-SCD, "Objekt-Band aendert sich zur Laufzeit nicht", "Latch sub01 tot").

## 1. Diff gelesen

`git log --oneline master..HEAD`: 29 Commits; Nachbesserung 2 = 0395b3da, eeca9a89, feda502d, df2c65dc, 03e5f2b7,
d7218a20, b0f1d204, 9db621d7. `git diff 5f4e3ae1..HEAD --stat`: NUR `tests/unit/probe_r36_klettern_zensus.c` (+309/-96
gesamt mit cmake), `tests/unit/probes/r36_klettern.cmake` (Kommentar), Dossier + `A_belege/nb2_*`. Engine/include/
platform unveraendert gegenueber Abnahme 1.

Gesamt-Diff gegen master (`git diff master -- re15_port/engine re15_port/include re15_port/platform`, selbst gelesen):
EINE Verhaltenszeile `climb_common.c:102` `if ((g_scd.props[p].flags & 0x101u) == 0u) continue;  /* @0x8002d358-35c */`
(vorher `!= 0x101u`); env-gegateter Trace-Zusatz `t4=/t4pos=/t4fl=` (`RE15_CLIMB_TRACE`); Messhaken
`RE15_TITLE_SHOT_CHAR` (main.c, 2 Zeilen); sonst Kommentare (scd_vm.c, re15_scd.h, granate_1150.c, sicherung_1150.c).

Sonde `probe_r36_klettern_zensus.c` (519 Z.) vollstaendig gelesen: Tabelle `R[]` je Record (24), `reset_room_modus`
(Lage-Flag Bank 3/4 vor `scd_room_reenter`; Bank 5 ueber den Rueckruf `re15_scd_set_player_model_sync` nach dem Wisch),
`lage_pruefen` (liest die RDT-Bytes am Offset: Opcode 0x2D, Typ 4, obj, Band pc[4], x/y/z pc[10..15], Flags pc[6..7]|1,
Box pc[22..33] gegen den Pool), Kandidatenzaehlung, vier Seiten je Record, Gegenprobe (Flag vor dem Init), Null-Box-Teil
unveraendert. Port-Seite des Rueckrufs selbst nachgelesen: `scd_room_setup.c:297` `g_game.flags[5][0] = 0;` ->
`:354` `if (s_player_model_sync) s_player_model_sync();` -> `:359` `scd_thread_start(0, rdt->main_scd)` — der Rueckruf
liegt also wirklich zwischen Wisch und main00.

## 2. Bau und Tests

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work
  to do.` / `=== LOCAL-BUILD-OK (build)` (Logs `abn2_A/configure.log`, `abn2_A/build.log`).
- Konsistenz: `git status` sauber (nur dieser Bericht); `re15_pc.exe` md5 `74a17d614423a4502cfd9be8b8bca52c` (= von
  Abnahme 1 gemessene exe), mtime 19:19:45; `climb_common.c` 19:18:07 < `climb_common.c.obj` 19:19:44;
  `probe_r36_klettern_zensus.c` 20:17:35 < `.obj` 20:19:57 < `.exe` 20:19:58.
- Schnelle Gruppe (Spur + jedes vom Diff beruehrte Modul + 1190-Nutzer des Rueckrufs + 1090-Sonden):
  `ctest -R "^(unit_r36_klettern_.*|unit_1090_.*|unit_rvd_scan_ungated_by_climb|unit_re2_room1090_ab|unit_r20_birkin_push_pin|
  unit_r22_10f0_figur|unit_r35_cut10f0_.*|unit_r35_inventar1050_kiste|unit_stair_1170_regression|unit_stair_hwtruth_1060|
  unit_r30_granate|unit_r35_granate|unit_sicherung_1150|unit_r30_sicherung_.*|unit_room1050_sicherung|unit_r30_irons_tisch|
  unit_r30_irons_tisch_licht|unit_r31_hebetisch|unit_r32_hebetisch_faecher|unit_r34_plattform|unit_r34_plattform_ton|
  unit_r34n_e_dokumente|unit_r35_inhalt_.*|r30_titel_blinken|unit_enemy_body_push|unit_plc_back_yaw_1090|unit_map_1090|
  unit_1190_ruestung|unit_1190_weste|unit_re2_room1190_ab|unit_re2z_pushoff_crawl|probe_1090_.*)$" --timeout 600`
  -> **55/55 bestanden** (10,25 s, `abn2_A/ctest_schnell.log`).
- `ctest -R "^integration_r36_klettern$" --timeout 1500 -V` -> **Passed 519,43 s**; Ausgabe: `[A_leon_10f0] start=1
  schieben=1 klettern=1 oben=1 zone=1 ziel=1`, `[B_elza_10f1] ...=1 ... ziel=1`, `[C_leon_szene] ...=1 ... ziel=1`
  (`abn2_A/ctest_integration.log`).

## 3. Mangel 1 der Abnahme 1 — nachgemessen

Eigener Lauf `build/tests/unit/probe_r36_klettern_zensus.exe pin` (exit 0, `abn2_A/zensus_eigenlauf.txt`):
```
--- ROOM11A0 Record @0x01058 flag(5,6)=0 beim Laden: Flag nach dem Raum-Init 0; Lage OK (obj 0 Band 3 (10050,-5400,8800) ... = Record)
--- ROOM11A0 Record @0x0107E flag(5,6)=1 ERZWUNGEN (Flag nach dem Wisch @0x8003ed74): Flag nach dem Raum-Init 1; Lage OK (obj 0 Band 0 (-19300,0,3300) ... = Record)
    ROOM11A0 lage=@0x0107E flag=1(ERZWUNGEN) ... band=0 pos=(-19300,0,3300) yaw=3072 ... try=1 sub=9 kind=0x81 obj=0 ende=51 oben=(-19300,-1800,3098)(soll y -1800) band=1(soll 1) stand=0 -> OBEN
    (11A1 identisch; je erzwungenem Record 4/4 Seiten OBEN)
Zensus: 24/24 Records (Raum x Objekt x Lage) wie der RDT-Record installiert und kletterbar; davon beim Laden erreichbar 22/22, nur ERZWUNGEN (11A0/11A1 @0x0107E, Bank 5) 2/2
--- GEGENPROBE ROOM11A0: flag(5,6)=1 VOR dem Raum-Init -> Flag danach 0, Lage-Pruefung @0x0107E: FALSCH (soll Band 0 (-19300,0,3300) fl 0x0001, ist aktiv 1 Band 3 (10050,-5400,8800) fl 0x0001)
--- GEGENPROBE ROOM11A1: (dasselbe)
Null-Box: Tafel 0 Abweichungen, achsgerade ohne Objekt-Treffer 4/4, Querversatz +-300 mit Start 8/8 (Spieler Band 1), vom Boden (Band 0) ohne Objekt-Treffer 8/8
PASS
```
96 Zeilen "-> OBEN" (24 x 4). Die 24 Offsets in `R[]` UNABHAENGIG an den RDT-Bytes geprueft (eigenes Skript
`abn2_A/skripte/rec_check.py`, shared_assets = info/Re1.5 per md5 fuer 10F0/11A0): alle 24 sind `2D <obj> 04` mit
Flags 0x0001 und den in der Sonde gemeldeten Lagen (u.a. 5021 @0x01262/@0x01288 — andere Offsets als 5020, korrekt).
**Der Messteil von Mangel 1 ist behoben**: die Lage @0x0107E wird jetzt wirklich installiert und gefahren, jede Lage
wird gegen ihren Record geprueft, und die Gegenprobe zeigt, dass die Pruefung den alten Fehler erkennt.

## 4. RE-Gate — Stichproben SELBST disassembliert

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE` des Baums (md5
b55fdaa5b9ec0f84bdd8a6b1107ea0b0 = Hauptbaum; Auslieferungsstand: @0x80026e4c `08 00 e0 03 21 10 00 00`, NICHT die
PATCHED-EXE).

| # | zitiert (Nachbesserung 2 bzw. Fix) | selbst gelesen | Urteil |
|---|---|---|---|
| 1 | Bank-5-Wisch @0x8003ed74 in FUN_8003ecec | `8003ed70 lui at,0x800b` / `8003ed74 sw zero,4136(at)` -> 0x800b1028, Bytes `0b 80 01 3c 28 10 20 ac`; Bank-Tabelle @0x80074664[5] = 0x800b1028 (selbst gelesen, [3] = 0x800b0ff8, [4] = 0x800b1018); Bit-Funktionen @0x8004ef90/b8/e4 `srl a1,5` / `0x80000000 >> (bit & 31)` -> (5,6) = Wort 0, Maske 0x02000000 | stimmt |
| 2 | Kette @0x80039a00 -> FUN_8003ef6c (@0x8003ef84 Wisch, main00 @0x8003efa0-b0, Dispatcher @0x8003f018) | `80039a00 jal 0x8003ef6c`; `8003ef74 jal 0x8003ea3c` / `8003ef7c jal 0x8003ebf4` / `8003ef84 jal 0x8003ecec` / `8003ef90 lw v0,-14472(v0)` (0x800ac778) / `8003efa0 lw v0,64(v0)` / `8003efac sw v0 -> 0x800b3f70` / `8003efb0 jal 0x8003ee3c` (a1 = 0) / `8003efc4 lw v0,68(v0)` ... `8003efd4 jal 0x8003ee3c` / `8003f018 jal 0x8003f0a0` | stimmt |
| 3 | Installer @0x80040974, Member 18 @0x80041228 (Tabelle @0x80010c8c) | `8004096c lbu v0,4(a2)` / `80040974 sb v0,130(a1)`; Tabelle @0x80010c8c [15] = 0x8004120c (`sb a2,11(a0)` @0x80041210), [18] = 0x80041224 (`j` + `sb a2,130(a0)` @0x80041228), [1] = `sw a2,56(a0)` | stimmt |
| 4 | Band-Gate der Objekt-Abtastung @0x80042cac-ccc vor den Stempeln @0x80042f5c/@0x80042fc4 | Objekt-Pass @0x8004378c `jal 0x80042bac` (a1 = 4, a2 = 0, s2 = 0x800b3f98 + 148 i; davor @0x80043788 `sb zero` -> obj+0x0B); in FUN_80042bac: s0 = AOT-Zeiger mit s0[0] = sce (Index in die SCE-Tabelle @0x8007469c @0x80042f74-8c), `80042cac lbu v0,2(s0)` / `80042cb4 andi 0x80` / `80042cc0 lbu v1,130(s1)` / `80042ccc bne v1,v0,0x8004301c`; Stempel `80042f58 addiu v0,s2,255` / `80042f5c sb v0,11(s1)` (Bytes `ff 00 42 26 0b 00 22 a2`), AUTO `80042fc4 sb v0,11(s1)`. Kein Sprung von vor dem Gate hinter das Gate (Zweigliste geprueft) | stimmt |
| 5 | Kern-Fix @0x8002d358-35c | `8002d350 lw v0,0(s0)` / `8002d358 andi v0,v0,0x101` / `8002d35c beq v0,zero,0x8002d3a4`, Bytes `01 01 42 30 11 00 40 10` | stimmt |
| 6 | **"Das Objekt-Band (+0x82) schreiben NUR der Installer und Member 18"; @0x8002c6c4 = "Spieler-Fall"** (Dossier Beleg 3, Sondenkopf Z. 50-51, `probes/r36_klettern.cmake`, Commit eeca9a89) | **FALSCH** — siehe 4.1 | **widerlegt** |

### 4.1 Gegenbefund: @0x8002c6c4 ist der FALL eines Typ-4-OBJEKTS, nicht der Spieler-Fall

Selbst disassembliert (PSX.EXE), Kette vollstaendig:
```
FUN_8002bd44 (Objekt-Pass, Pool fp = 0x800b3f98, Stride 148, je aktivem Objekt):
  8002bde0: lbu  v0,8(s2)            ; obj+0x08 = Objekttyp (Installer @0x80040940/5c: Record-Byte 2)
  8002bde8: beq  v0,zero,0x8002be0c
  8002bdf0: lui  at,0x8007 / 8002bdf4: addiu at,at,15472   ; Typ-Tabelle 0x80073c70
  8002bdfc: lw   v0,0(at) / 8002be04: jalr v0 / 8002be08: addu a0,s2,zero   ; Handler(obj)
  Tabelle @0x80073c70: [1] 0x8002c3f4 (jr ra) [2] 0x8002c3fc (jr ra) [3] 0x8002c404 (jr ra) [4] 0x8002c40c [5] 0x8002cab4
  ... (jeder aktive Durchlauf erreicht @0x8002c0ec; einziger Sprung am Ende vorbei: inaktiv @0x8002bdd8)
  8002c0fc: lbu  v1,8(s2) / 8002c104: ori v0,zero,0x4 / 8002c108: bne v1,v0,0x8002c150   ; nur Typ 4
  8002c110: lbu  v0,4(s2) / 8002c118: bne v0,zero,0x8002c150                           ; faellt noch nicht
  8002c120: addiu a0,s2,52 / 8002c124: lbu a2,130(s2) / 8002c128: jal 0x8003b7f0 / 8002c12c: ori a1,zero,0x1c2
                                      ; SCA-Attributwort an der Objektposition, Objekt-Band, Rand 450
  8002c130: andi v0,v0,0x2 / 8002c134: beq v0,zero,0x8002c150                          ; Absprungzelle (Wort & 2)
  8002c13c: jal  0x8002d3d8 (a0 = obj)                                                   ; Fallrichtung vom Spieler weg
  8002c144: ori v0,zero,0x1 / 8002c148: sb v0,4(s2) / 8002c14c: sb zero,5(s2)           ; Bytes 01 00 02 34 04 00 42 a2 05 00 40 a2
FUN_8002c40c (Typ-4-Handler): 8002c414 lbu v1,4(a0) ... == 1 -> 8002c42c jal 0x8002c444
FUN_8002c444 (s1 = a0 = OBJEKT; einziger Aufrufer @0x8002c42c — jal-Suche in PSX.EXE und allen info/Re1.5/PSX/BIN/*.BIN;
  FUN_8002c40c nur als Tabellenwort @0x80073c80; Zustand obj+5 0..3, Zaehler obj+6):
  kippen + vorwaerts (0x800b526c/27c aus FUN_8002d3d8), Schwerkraft (0x8008f68c += 160), bei obj+6 == 13:
  8002c67c: jal 0x8001c6e8 (Bodenhoehe an obj+0x34) ... 8002c6c4: sb v1,130(s1)   ; obj+0x82 = -(Boden/1800)
  danach Landen (obj+5 = 2), Nachfedern, obj+4 = 0 (Decompilat RE_15_Quellcode_V2/FUN_8002c444.c deckungsgleich:
  `*(char *)(param_1 + 0x82) = ... (int)sVar3 / 0x708 ...`)
```
Damit gibt es im Original einen Laufzeit-Schreiber des Objekt-Bands: ein Typ-4-Objekt (Tritt/Kiste), das auf eine
SCA-Zelle seines Bandes mit Wort & 2 geschoben wird, kippt, faellt und bekommt das Band des Bodens darunter. Der Port
hat dafuer kein Gegenstueck (`grep` in engine/src: kein Bezug auf 0x8002c444/0x8002c0fc-14c; `props[].band` wird nur
bei der Installation geschrieben: scd_vm.c:4218, dokumente_r34.c:81, granate_1150.c:84, inhalt_r35.c:64,
irons_tisch_1150.c:63, sicherung_1150.c:116; aot_common.c:1639-1666 dokumentiert den Objekt-Pass nur bis zur
Ruecksetzung @0x8002c0b8-d4).

Uebrige Schreiber der Liste `A_belege/nb2_scans.txt` selbst eingeordnet: @0x8001be54 (FUN_8001bd60, nur aus den
Stage-Overlays gerufen, Felder +442/+448 = Aktor), @0x8005276c/@0x80052dd0 (g_entity(cur), Felder +442 = Aktor),
@0x800421d0 (Sce_em_set, Felder +448/+454 = Aktor — im Dossier gar nicht genannt), @0x8001f1b4/@0x8001f254 (Halbwort,
Struktur mit +152..+156 = Effekt), STAGE1.BIN alle 11 ueber g_entity(cur) mit Aktorfeldern (+442/+470/+480) bzw.
@0x80108664 eine Grossstruktur (+1321). Falsch eingeordnet ist also genau EINER: @0x8002c6c4.

**Folgen fuer die Aussagen der Nachbesserung 2:**
- Beleg 3 ("das Objekt-Band aendert sich zur Laufzeit nicht", "Im Original steht die Kiste in JEDEM Besuch auf Band 3
  ... weder beim Laden noch zur Laufzeit erreichbar") ist in seiner Begruendung falsch. Beleg 4 ("die Kiste muesste
  dafuer auf Band 0 gelangen, was kein Code tut") ebenso. Die Antwort auf die Frage der Abnahme 1 ("ob das Original
  die Kiste dynamisch auf Band 0 bringt") lautet NICHT "statisch NEIN", sondern: der Mechanismus existiert
  (FUN_8002c444), ob er fuer die 11A0-Kiste greift, entscheidet die Raumgeometrie.
- Fuer ROOM11A0/11A1 haelt das Ergebnis vermutlich auf ANDEREM Grund (eigene SCA-Auswertung `abn2_A/skripte/
  sca_band.py`, NICHT am Original gemessen): die einzige Band-3-Absprungzelle (x 5944..13525, z -253..2667, Wort
  0x3102, u0 1) liegt hinter der Band-3-Kantenwand (x 5944..13525, z 1797..3028, Wort 0x3300, u0 255); die
  Band-3-Flaeche ist rundum von u0-255-Zellen umschlossen, die nach der Port-Semantik von FUN_8003b558(obj,2)
  ((2 & u0) != 0, Radius box_hx 900, re15_collision.c:1064-1087 — @0x8003b558 nicht selbst disassembliert) Objekte
  stoppen; der Kistenmittelpunkt kommt nicht unter z ~3928, der Ausloeser braucht z <= 3117.
- Fuer ROOM20A0/20A1 und ROOM5020/5021 sind die Kisten-Lagen (3,204)=1 bzw. (4,203)=1 (Records @0x0187A,
  @0x01268/@0x01288, Band 0) im Original NUR ueber diesen Fall erreichbar: das Latch (20A0 sub01 @0x01AD4-e2
  `Work_set 3,0` / `Member_cmp 15 == 7` / `Set 3,204,1`, Slot 7 Floor 0; 5020 @0x012F4-302 `Work_set 3,0` /
  `Member_cmp 15 == 4` / `Set 4,203,1`, Slot 4 Floor 0) braucht die Kiste auf Band 0 (Band-Gate @0x80042ccc).
  Objektsperre selbst nachgelesen: FUN_8003b558 @0x8003b634-660 (`lhu v0,8(s0)` Band-Nibble gegen obj+0x82,
  `lhu v0,6(s0)` / `sra 24` = u0, `and v0,s6,v0` mit Klasse 2) — u0 255 sperrt Objekte, u0 1 und u0 253 nicht.
  5020 Band 4: die Ostkante z -26266..-24025 traegt NUR die Absprungzelle x -7675..-7175 (Wort 0x4002, u0 1); die
  Kiste kommt dort hin (gemessen, M5 unten). 20A0 Band 4: Tasche x 15800..17600 vor der Absprungzelle x 15800..17600,
  z -22000..-21000 (Wort 0x4206, u0 253) ist genau kistenbreit (1800) — ob die Kiste hineinpasst, ist ein Grenzfall
  der Sperrpruefung (nicht gemessen). Im Port kann keine dieser Kisten herunterfallen — die Band-0-Lagen, die der
  Zensus als "beim Laden erreichbar" fuehrt und mit gesetztem Flag misst, entstehen im Port im Spiel nie. Ob ein
  Fortschritt im Original davon abhaengt, ist ungemessen (die Flags gaten im SCD nur Kistenlage und Schiebezone).

## 5. Messung des Nutzer-Punkts an der exe (eigene Laeufe, eigene Eingaben)

Laufskript `abn2_A/skripte/lauf.sh`: Messkopie `re15_port/build/platform/pc/re15_pc_abn2a_<lauf>.exe` (md5 je Lauf in
`laeufe/<lauf>/md5.txt` gegen die gebaute exe geprueft, Kopie nach dem Lauf geloescht), Arbeitsordner
`abn2_A/laeufe/<lauf>/` (env.txt, debug.log, climb.txt, stderr.txt, ergebnis.txt). Basis-Umgebung: `RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=1 RE15_CLIMB_TRACE=climb.txt`.

**M2 — ROOM10F0, Tritt in der ENDLAGE ((3,102)=1, (-4850,0,-850)), Anlauf von WESTEN (Blick +X; Abnahme 0/1 und Bau-
Agent kamen von Osten/Sueden/-Z/+Z), oben drehen, zur Tuer, Tuer nach ROOM1090** — `RE15_DEBUG_JUMP=10f0@250
RE15_SET_FLAG=3:102,9:71 RE15_PLAYER_POS=-5925,-850,0 RE15_INPUT_SCRIPT=W8,A0.1,W3,R0.37,W0.5,U0.5,W0.3,A0.1,W4
RE15_EXIT_AT=60#1090` (exit 0, 30 s; md5 74a17d61...):
```
T492 room=10F0 pad=8000 pos=(-5925,0,-850) rot=0 band=0 sub=9 kind=0x81 ph=0 obj=3 stand=-1 ... t4pos=(-4850,-850) t4fl=0x0001   <- Klettern
T523 room=10F0 pos=(-4777,-1800,-850) rot=0 band=1 sub=9 ph=3 obj=3 stand=3                     <- y -1800, Band 1, Standobjekt Tritt
T544 room=10F0 pos=(-4777,-1800,-850) band=1 sub=0 obj=3 stand=3                                 <- oben
T588..T600 pad=0020 rot 384 -> 960 -> 1056 (oben gedreht)  T612..T624 pad=0010 z -998 -> -1225   <- oben nach -Z
T635 room=10F0 pad=8000 pos=(-4822,-1800,-1225) band=1 stand=3 -> T636 room=1090 pos=(-13600,-9000,1300) band=5
debug.log: [tuer] Sequenz Archiv 2 DOOR1E ... / [room] PC loaded room1090.rdt / [flow] EXIT_AT: Bild 60 in Raum 1090 erreicht
```

**V1 — Gegenprobe ohne Fix, GLEICHE Eingabe wie M2**: Hauptbaum-exe `re15_port/build/platform/pc/re15_pc.exe` (md5
2fd81d200fcdcd5db99b9d58efa0742b, 2026-10-04 18:37; master enthaelt den Fix nicht), nur als Messkopie in den Spurbaum
kopiert, `RE15_EXIT_AT=700#10f0` (exit 0, 33 s):
```
T492 room=10F0 pad=8000 pos=(-5925,0,-850) rot=0 band=0 sub=0 kind=0x00 ph=0 obj=-1 stand=-1   <- Aktionstaste, KEIN Klettern
grep -c "sub=9" climb.txt = 0
```
-> Gleiche Position, gleiches Bild, gleiche Taste: ohne Fix kein Start, mit Fix Substate 9 / KIND 0x81 / Objekt 3.

**M1 — ECHTER WEG: ROOM10D0 -> Tuer -> ROOM10F0 (Tritt in der Startlage) -> Schieben (+X, -Z) -> Klettern -> oben zur
Tuer -> ROOM1090** — `RE15_DEBUG_JUMP=10d0@250 RE15_SET_FLAG=3:50,9:71 RE15_PLAYER_POS=1700,-7000,2048
RE15_INPUT_SCRIPT=W11,A0.1,W1,<Tuerweg des Integrationslaufs B>,<Schluss des Integrationslaufs> RE15_EXIT_AT=60#1090`
(exit 0, 86 s; md5 74a17d61...):
```
[debug-menu] JUMP -> 10d 2F CORRIDOR (ROOM10D0) spawn=(1700,0,-7000)
[tuer] Sequenz Archiv 2 DOOR13 Variante 1 ... -> [room] PC loaded room10f0.rdt
T583  room=10F0 pos=(8400,0,-350) rot=2048 band=0 ... t4pos=(-6050,4950) t4fl=0x0001     <- durch die Tuer, Tritt in der Startlage
T1504 room=10F0 pad=0010 pos=(-7125,0,4960) push=5                                         <- Schieben +X beginnt
T2012 room=10F0 pad=0010 pos=(-5013,0,-58) push=5 t4pos=(-4900,-1108)                     <- Schieben -Z bis an die Wand
T2051 room=10F0 pad=8000 pos=(-5013,0,-58) band=0 sub=9 kind=0x81 ph=0 obj=3               <- Klettern
T2103 room=10F0 pos=(-5013,-1800,-1206) rot=1024 band=1 sub=0 obj=3 stand=3                <- oben
T2156 room=10F0 pad=8000 pos=(-5013,-1800,-1332) band=1 stand=3 -> T2157 room=1090 pos=(-13600,-9000,1300) band=5
[tuer] Sequenz Archiv 2 DOOR1E ... / [room] PC loaded room1090.rdt / [flow] EXIT_AT: Bild 60 in Raum 1090 erreicht -> exit
```
-> Der Fortschritt, den der Nutzer vermisst (ROOM10F0 -> ROOM1090 ueber den Tritt), ist an der exe ueber den echten
Tuerweg hergestellt.

**M3 — ROOM5020 (Stage 5, Kiste Band 4, Lage (4,203)=0 @0x01242) an der exe** — `RE15_DEBUG_JUMP=5020@250
RE15_PLAYER_POS=-12350,-23150,0,4 RE15_INPUT_SCRIPT=W8,A0.1,W4 RE15_EXIT_AT=500#5020` (exit 0, 26 s):
```
[debug-menu] JUMP -> 502 CARGO STORAGE (ROOM5020) spawn=(-12350,-7200,-23150)
T492 room=5020 pad=8000 pos=(-12350,-7200,-23150) rot=0 band=4 sub=9 kind=0x81 obj=0 ... t4pos=(-11000,-23150) t4fl=0x0001
T523 room=5020 pos=(-11202,-9000,-23150) band=5 sub=9 ph=3 obj=0 stand=0
T544 room=5020 pos=(-11202,-9000,-23150) band=5 sub=0 obj=0 stand=0   (unveraendert bis Laufende T751)
```
**M4 — ROOM20A0 (Stage 2, Kiste Band 4, Lage (3,204)=0 @0x01854) an der exe** — `RE15_DEBUG_JUMP=20a0@250
RE15_PLAYER_POS=10900,-12400,0,4 RE15_INPUT_SCRIPT=W8,A0.1,W4 RE15_EXIT_AT=500#20a0` (exit 0, 26 s):
```
[debug-menu] JUMP -> 20a SERVICE TUNNEL (ROOM20A0) spawn=(10900,-7200,-12400)
T492 room=20A0 pad=8000 pos=(10900,-7200,-12400) rot=0 band=4 sub=9 kind=0x81 obj=0 ... t4pos=(12250,-12400) t4fl=0x0001
T523 room=20A0 pos=(12048,-9000,-12400) band=5 sub=9 ph=3 obj=0 stand=0
T544 room=20A0 pos=(12048,-9000,-12400) band=5 sub=0 obj=0 stand=0   (unveraendert bis Laufende T751)
```
-> Auch ausserhalb von Stage 1 sind die Kisten ohne Bit 0x100 an der exe kletterbar (bisher nur VM-Zensus).

**M5 — ROOM5020: Kiste ueber die Ostkante von Band 4 schieben (Ausloesezone des Original-Falls @0x8002c120-14c) —
was tut der Port?** `RE15_DEBUG_JUMP=5020@250 RE15_PLAYER_POS=-11000,-21800,1024,4
RE15_INPUT_SCRIPT=W8,U5,W0.5,R0.37,W0.3,U2.0,W0.3,L0.37,W0.3,U1.8,W0.3,L0.37,W0.5,U8,W2 RE15_EXIT_AT=1100#5020`
(exit 0, 46 s; Leon noerdlich der Kiste, schiebt -Z bis an die Suedwand, geht aussen herum, schiebt +X):
```
T540  room=5020 pos=(-10991,-7200,-22380) band=4 push=5 t4pos=(-11000,-23730)     <- Schieben -Z
T630  room=5020 pos=(-10975,-7200,-23982) band=4 push=-1 t4pos=(-11000,-25332)    <- Kiste an der Suedwand (z -26266 + 900)
T930  room=5020 pos=(-11749,-7200,-25742) band=4 push=5 t4pos=(-10399,-25332)     <- Schieben +X
T1050 room=5020 pos=(-9278,-7200,-25816)  band=4 push=5 t4pos=(-7928,-25332)      <- Kistenmitte in der Absprungzone (x >= -8125 = -7675 - 450)
T1080 room=5020 pos=(-8677,-7200,-25805)  band=4 push=5 t4pos=(-7327,-25332)
T1110 room=5020 pos=(-8254,-7200,-25810)  band=4 pad=0000 t4pos=(-6904,-25332)    <- Kistenmitte 271 HINTER der Band-4-Kante (x -7175)
T1110..T1350 unveraendert: Kiste bleibt dort stehen (Port: kein Laufzeit-Schreiber von props[].band, also Band 4 / y -7200)
```
Im Original setzt der Objekt-Pass beim ersten Bild mit der Kistenmitte in der um 450 erweiterten Absprungzelle (Wort
0x4002 & 2) obj+4 = 1 (@0x8002c148), FUN_8002c444 kippt die Kiste und setzt ihr Band auf den Boden darunter
(@0x8002c6c4). Im Port faehrt die Kiste ueber die Kante hinaus und bleibt in Band-4-Hoehe ueber dem Abgrund stehen.
(Nur Trace gemessen, kein Bild; die Kistenhoehe folgt aus dem fehlenden Schreiber, s. 4.1.)

## 6. Vertrag und Dateien

- `git diff master --name-only` ohne analysis/: climb_common.c, granate_1150.c, scd_vm.c, sicherung_1150.c,
  re15_scd.h, platform/pc/main.c, tests/integration/test_r36_klettern.cmake, tests/unit/probe_r36_klettern_10f0.c,
  tests/unit/probe_r36_klettern_zensus.c, tests/unit/probes/r36_klettern.cmake, tests/unit/test_1090_climb_pin.c. Keine
  release/, platform/android/, shared_assets/PSX/, tests/*/CMakeLists.txt, RELEASE_NOTES.
- Keine Bank-9-Bits, Nachrichten-IDs, Ereignisse; `re15_game_flag_set` im Diff nur in Testsonden mit Original-Lageflags.
- Signalwoerter (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vorlaeufig|geschaetzt`) in den
  hinzugefuegten re15_port-Zeilen: keine.
- Gemeinsame Dateien: unveraendert gegenueber Abnahme 1 (eine Logikzeile + env-gegateter Trace in climb_common.c,
  2-Zeilen-Haken main.c).
- Tests je Punkt vorhanden und messend: `unit_r36_klettern_10f0_endlage/_startlage`, `unit_r36_klettern_zensus`
  (jetzt mit Lage-Pruefung + Gegenprobe), `integration_r36_klettern` (echte exe, A/B/C) — alle gruen (Abschnitt 2).

## 7. Urteil je Punkt

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1. "kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmoeglich" | **erfuellt** | M1: echter Tuerweg 10D0 -> 10F0, Tritt aus der Startlage geschoben (+X, -Z), geklettert (T2051 sub=9/0x81), oben (T2103 y -1800, Band 1, stand=3), Tuer -> ROOM1090 (T2157). M2: Endlage, Anlauf von Westen (neue Seite), oben gedreht, Tuer -> ROOM1090 (T636). V1: dieselbe Eingabe mit der exe ohne Fix -> kein Start (0 Zeilen sub=9). M3/M4: Kisten ROOM5020 und ROOM20A0 (Band 4) an der exe erklettert (y -9000, Band 5, Standobjekt). integration_r36_klettern A/B/C gruen (Leon 10F0 x2, Elza 10F1 -> 1091, PL04). Zensus 24/24 Records mit Lage-Pruefung. Fix byte-true @0x8002d358-35c (selbst gelesen). |

Der Nutzer-Punkt ist erfuellt. Das Klettern selbst ist an jeder Kiste/jedem Tritt ohne Bit 0x100 in jeder installierten
Lage hergestellt. Getrennt davon fehlt der Original-Mechanismus "Kiste von der Kante schieben -> sie faellt" (4.1, M5).
Das ist kein Klettern und keine Regression (der Port hatte ihn nie). Er betrifft aber genau diese Objekte. Siehe
Mangel 2.

## 8. Maengel

1. **RE-Beleg der Nachbesserung 2 ist falsch: @0x8002c6c4 ist kein "Spieler-Fall", sondern der Fall eines Typ-4-OBJEKTS;
   das Objekt-Band aendert sich im Original zur Laufzeit.** Nachpruefbar mit `re15_disasm.py dis 0x8002bdc8 230` (Typ-
   Tabelle @0x8002bdf0-be08 -> @0x80073c70[4] = 0x8002c40c; Ausloeser @0x8002c0fc-c14c: Typ 4, obj+4 == 0,
   `jal 0x8003b7f0` mit Rand 0x1c2 auf dem Objekt-Band, `andi 0x2`, `jal 0x8002d3d8`, `sb v0,4(s2)` = 1) und
   `dis 0x8002c444 ...` (s1 = a0 = Objekt, @0x8002c67c `jal 0x8001c6e8`, @0x8002c6c4 `sb v1,130(s1)`; Decompilat
   `RE_15_Quellcode_V2/FUN_8002c444.c` Z. 89-95). Falsch sind damit: Dossier "Beleg 3" (A_klettern.md Z. 547 Ueberschrift
   "das Objekt-Band (+0x82) aendert sich zur Laufzeit nicht", Z. 554 Einordnung "den Spieler-Fall (@0x8002c6c4)",
   Schluss "weder beim Laden noch zur Laufzeit erreichbar"), "Beleg 4" ("die Kiste muesste dafuer auf Band 0 gelangen,
   was kein Code tut"), "OFFEN (Stand Nachbesserung 2)" ("statisch NEIN (Beleg 3: kein Laufzeit-Schreiber von obj+0x82
   ausser Member 18 ...)", erwartetes Ergebnis des dynamischen Gegenchecks), der Sondenkopf
   `probe_r36_klettern_zensus.c:50-51` ("das Objekt-Band +0x82 schreiben nur der Installer @0x80040974 und Member 18
   @0x80041228") und `tests/unit/probes/r36_klettern.cmake:36-37` ("das Objekt-Band aendert nur der Installer
   @0x80040974 bzw. Member 18 @0x80041228"); die Commit-Message eeca9a89 ist Historie und im Dossier als ueberholt zu
   vermerken. (Dazu fehlt in
   der Einordnung @0x800421d0 = Sce_em_set ganz.) Abhilfe: Aussagen berichtigen; fuer ROOM11A0/11A1 das Ergebnis
   "Kiste bleibt auf Band 3" — falls es gilt — mit dem RICHTIGEN Grund belegen (Band-3-Flaeche rundum von u0-255-Zellen
   umschlossen, Objektsperre FUN_8003b558 @0x8003b634-660 Klasse 2; die einzige Band-3-Absprungzelle x 5944..13525,
   z -253..2667 liegt hinter der Kantenwand z 1797..3028) oder als ungemessen fuehren. Kein Engine-Code noetig.

2. **OFFEN und "Fuer den Nutzer" verschweigen den fehlenden Objekt-Fall, der genau die Kisten-Lagen erzeugt, die der
   Zensus als "beim Laden erreichbar" fuehrt.** Der Port hat kein Gegenstueck zu @0x8002c0fc-c14c / FUN_8002c444
   (`grep` engine/src; `props[].band` nur bei der Installation geschrieben). Gemessen (M5, ROOM5020): die Kiste faehrt
   im Port ueber die Band-4-Kante hinaus (Mitte x -6904, Kante x -7175) und bleibt in Band-4-Hoehe stehen, wo das
   Original sie ab x >= -8125 kippen und fallen laesst. Folge: die Lagen (4,203)=1 (ROOM5020/5021 @0x01268/@0x01288)
   und — falls die Kiste in die kistenbreite Tasche passt — (3,204)=1 (ROOM20A0/20A1 @0x0187A) entstehen im Port im
   Spiel nie (Latch braucht Band 0: 5020 @0x012F4-302, 20A0 @0x01AD4-e2, Band-Gate @0x80042ccc). Das Dossier nennt
   diese 4 Records "beim Laden erreichbar" und sagt dem Nutzer "jetzt in jeder Lage ... kletterbar", ohne zu sagen,
   dass der Port die Kiste dorthin nie bringen kann. Abhilfe: unter OFFEN mit Adressen und Messweg fuehren (Original:
   DuckStation mit der Auslieferungs-EXE, ROOM5020 per Debug-Menue, Kiste bei z ~ -25300 nach Osten schieben, Bild fuer
   Bild obj 0 = 0x800b3f98 +0x04/+0x05/+0x82/+0x38 und Bank 4 Wort 6 (0x800b1030, Maske 0x00100000 = Bit 203) lesen);
   in "Fuer den Nutzer" die Grenze nennen (Kisten in 5020/20A0 und den Elza-Spiegeln fallen im Port nicht von der
   Kante, sie bleiben in der Luft stehen) und als Entscheidungsfrage stellen, ob der Fall portiert werden soll. Ob ein
   Fortschritt im Original davon abhaengt, ist ungemessen.

## 9. Hinweise (kein Mangel)

- H1/H3 und FUN_8002dc48 bleiben wie im Dossier gefuehrt OFFEN (Adressen und Messwege stehen dort, unveraendert).
- Mit dem Fix wird auch die im Port ueber den Abgrund geschobene 5020-Kiste (M5) besteigbar; ein so erreichter
  Zustand (Spieler auf einer schwebenden Kiste) faellt unter Mangel 2.
- ROOM1090: die Kiste t4 (Flags 0x0101) faehrt nach dem Betreten ohne Spielerschub von x -8136 auf -9836 (M1/M2 und
  Integrationslauf A gleich). Das ist der vorhandene NPC-Schub (Pin `unit_1090_npc_prop_push`, gruen) und hat mit dem
  Fix nichts zu tun.
- Abnahme-1-Mangel: der Messteil ist behoben (Abschnitt 3); offen bleibt nur die Begruendung (Mangel 1).

## 10. Ergebnis

**bestanden = NEIN.** Der Nutzer-Punkt ist **erfuellt**: eigener echter Tuerweg 10D0 -> 10F0 -> Schieben -> Klettern ->
Tuer nach ROOM1090 (M1), eine neue Anlaufseite in der Endlage mit Tuer (M2), die Gegenprobe ohne Fix startet bei
gleicher Eingabe nicht (V1), zwei Raeume ausserhalb von Stage 1 an der exe (M3 5020, M4 20A0), Integrationslauf
A/B/C und alle 55 gezielten Unit-Tests gruen. Der Mangel der Abnahme 1 ist im Messteil behoben (Lage @0x0107E wirklich
gefahren, Lage-Pruefung je Record, Gegenprobe erkennt den alten Fehler). Das RE-Gate haelt aber nicht: der neue Beleg 3
ordnet @0x8002c6c4 falsch ein, und die daraus gezogenen Aussagen (Code, Kommentar, Dossier) sind falsch (Mangel 1). Die
Folge ist ein nicht dokumentierter Original-Mechanismus, den der Port nicht hat (Mangel 2, an der exe gemessen). Beide
Maengel lassen sich ohne Engine-Code beheben (Text, Kommentare, OFFEN, Nutzerhinweis).

## 11. Messprotokoll

| Lauf | Zweck | Kern-Umgebung | Ergebnis |
|---|---|---|---|
| Bau | configure + build | local_build.sh | OK, `ninja: no work to do`, exe md5 74a17d61... |
| ctest schnell | Spur + Module des Diffs | Regex Abschnitt 2 | 55/55, 10,25 s |
| ctest integration | echte exe A/B/C | `^integration_r36_klettern$` | Passed 519,43 s |
| Z | Zensus-Sonde selbst | `probe_r36_klettern_zensus.exe pin` | PASS, 24/24 (22 + 2 erzwungen), Gegenprobe FALSCH wie verlangt, 96 x OBEN |
| M1 | echter Weg 10D0 -> 10F0 -> 1090 | DEBUG_JUMP=10d0@250, SET_FLAG=3:50,9:71, PLAYER_POS=1700,-7000,2048 | T583 10F0, T1504/T2012 push, T2051 sub=9/0x81, T2103 oben, T2157 room=1090 |
| M2 | Endlage von Westen + Tuer | DEBUG_JUMP=10f0@250, SET_FLAG=3:102,9:71, PLAYER_POS=-5925,-850,0 | T492 sub=9/0x81, T523 Band 1 stand=3, T636 room=1090 |
| V1 | Gegenprobe ohne Fix | Hauptbaum-exe 2fd81d20..., Eingabe wie M2 | T492 pad=8000 sub=0 obj=-1, 0 Zeilen sub=9 |
| M3 | 5020 Kiste Band 4 | DEBUG_JUMP=5020@250, PLAYER_POS=-12350,-23150,0,4 | T492 sub=9/0x81, T544 y -9000 Band 5 stand=0 |
| M4 | 20A0 Kiste Band 4 | DEBUG_JUMP=20a0@250, PLAYER_POS=10900,-12400,0,4 | T492 sub=9/0x81, T544 y -9000 Band 5 stand=0 |
| M5 | 5020 Kiste ueber die Kante | DEBUG_JUMP=5020@250, PLAYER_POS=-11000,-21800,1024,4, Skript Abschnitt 5 | Kiste bis x -6904 (Kante -7175), bleibt Band 4 |

Alle exe-Laeufe: Messkopie `re15_pc_abn2a_<lauf>.exe` im Spurbaum (danach geloescht), Arbeitsordner
`<scratchpad>/abn2_A/laeufe/<lauf>/`; Skripte `<scratchpad>/abn2_A/skripte/` (lauf.sh, rec_check.py, sca_drop.py,
sca_drop_kurz.py, sca_band.py, jal_callers.py, jal_callers_bin.py, imm_scan.py, sb4_scan.py). Im Hauptbaum wurde nichts
geschrieben (nur die exe gelesen/kopiert).
