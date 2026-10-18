# Runde 39 — Spur K "heliport1171": ABNAHME 1 (unabhaengig, 2026-10-18, nach Nachbesserung 1)

Baum `.claude/worktrees/r39_heliport1171`, Zweig `r39/heliport1171`, HEAD dc4ab998 (letzter Code-Commit 86956b9c, 21:01).
Merge-Basis mit master 22abaed9; master ist seither nur um Doku + `test_r38_itemorte.c` (96ab793c) weiter — die
Spur-Dateien = `git diff master...HEAD` (23 Dateien). Abnahme-Laeufe im Scratchpad `K39abn1/`, eigene exe-Kopien
`re15_pc_k39abn1_*.exe`, kein taskkill.

**Stand: in Arbeit** (Bericht wird fortlaufend geschrieben).

## 0. Bau
* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`
  / `=== LOCAL-BUILD-OK (build)`; exe `re15_port/build/platform/pc/re15_pc.exe` 21:01 (7 619 008 B) = Code-Stand 86956b9c
  (dc4ab998 aendert nur das Dossier); `git status --porcelain` leer.
* Spur-Riegel Unit `ctest --test-dir re15_port/build -R "^unit_r39_heliport1171_" --timeout 600 -V`: **10/10 gruen**
  (`100% tests passed, 0 tests failed out of 10`). Messzeilen der neuen Teile:
  - elza: `Sub-6-Lauf ab Bild 38/186/592/670/945: Clip 1 16 Bilder (Folge 0..), danach Clip 2 Bild 0`; `M3a: kein einzelnes
    Sub-6-Bild ... (0)`; `Ankunftsbilder ... (6 von 6 ...)`; `Schub: Bit-Takt Elza Bild 933, John Bild 933`.
  - kraehen: `Kraehe Slot 2: Start Clip 0 Bild 0, ORIENT ab Bild 96 (Folge 0..7,0,1), Landung Bild 386 (Clip 4), Zustand 7
    ab Bild 398 (Clip 10 Bild 11)`; Slot 3 entsprechend (ORIENT 107, Landung 434, Zustand 7 ab 446).
* Spur-Riegel Integration `ctest ... -R "^integration_r39_heliport1171_" -j 2 -V`: **2/2 gruen** (laden 67,7 s, weg 90,7 s).
  Messzeilen: laden `Nachbesserung 1: Elza Clip-1-Laeufe 5 (falsch: ); Clip 17 rueckw. ab Elza F934 / John F934; Kraehe 2:
  ORIENT ab Bild 0, Leiche 10/10/40/0 Bild 11; Kraehe 3: ...Bild 11`; weg dasselbe mit `Elza F1000 / John F1000`. Johns
  Abschnitt unveraendert gegen Abnahme 0: `INIT-Pose Loco 2/50 Bild 0 221 ... Heran-Winken (Clip 2/96) 128 Bilder,
  Kletter-Clip (2/50 nach Plc_motion) 0, RBJ-Clip 0 (20) 150, Bank-0-Clip 0 (34) 0`.
* Gezielte Unit/Probe-Gruppe aller beruehrten Module (Regex des Bau-Agenten erweitert um
  `anim|dest|locom|motion|dispatch|1031|1021|1011|10b1|1100|1040|wurf|opfer|victim|grab`, `-E "^integration_"`, -j 4):
  **`100% tests passed, 0 tests failed out of 267`** (29,5 s).

## 1. Code-Diff (gelesen, `git diff master...HEAD -- re15_port`, Nachbesserung 1 = Commits 7ec96a2b..86956b9c)
| Datei | Inhalt NB1 |
|---|---|
| engine/src/heliport1171_r39.c + include/re15_heliport1171_r39.h | Saat-Zustand Sub 6 `s_reach_saat` (0 aus/laufend, 1 = Phase 0 im naechsten Event-Reach-Takt, 2 = Dispatcher-Bild); `re15_r39k_reach_takt` posiert im Saattakt Clip 1 / Bild 0 / Blende 7 OHNE Vorschub und beendet einen gehaltenen Schub-Ausgang; Kraehen-Tabelle je Slot (+0x94/+0x95/+0x8f, `kraehe_fremd` fuer RE2-Hirn/Zwang 1120), `re15_r39k_kraehe_anim` = posieren (Aktorfelder) -> dann vorschieben; Installer loescht beide Tabellen. |
| game_step_common.c | `begin` (SCD-Modus 6, Kraehen-Wurf) -> Saat 1; NEU `begin_naechstes_bild` -> Saat 2; `end`/`cmd_zero` loeschen die Saat, `cmd_zero` beendet zusaetzlich Sub 0x13; `event_reach_clip` liefert im Dispatcher-Bild -1; der Takt ruft `reach_takt` vor dem Vorschub. |
| actor_locomotion.c | Spieler-Ankunft -> `begin_naechstes_bild`; im Walker-Takt (hinter dem Spieler-Pause-Gate) `reach_bildanfang` (2 -> 1). |
| player_common.c | pm-2-Leerlauf-Ruecksetzung (105/100 ohne Walker -> 200) nicht im Dispatcher-Bild. |
| elza_r39.c (Spur H) | Walker-Init setzt zusaetzlich `motion_init_delay = 1` (Bild 0 auch bei gleichem Clip). |
| szenenleon_r36.c | Spieler-Schub: Bit-Takt setzt nur +0x6 = 6 + `s_sp_saat`; Phase 6 setzt Clip 0x11 rueckwaerts im 1. Takt; Phase 7 haelt Bild fc-1 (rueckwaerts = Bild 0), Port-Merker `s_sp = 8`, `begin_naechstes_bild`. |
| enemy_ai_common.c | `re15_crow_clip` -> Tabelle (posiert nicht), `re15_crow_anim` -> `re15_r39k_kraehe_anim`; drei Bild-8-Abfragen lesen +0x95 (`re15_r39k_kraehe_bild`); ORIENT-Umlauf-Effekt mit der Pose vor dem f314; Kommentare Leiche 12 -> 11. |
| Tests | NEU `unit_r39_heliport1171_elza/_kraehen`, Integrationsblock am Ende von `test_r39_heliport1171.cmake`; fremde Riegel angepasst: `test_plc_back_yaw_1090.c`, `probe_crow_death.c` (12 -> 11), `test_r38_heliport1171.c` (flagwort-Effektpose), `test_r37_roy.c` (H3). |

Nachgerechnet (Port-Semantik, Saattakt): `begin` (Saat 1) -> derselbe `event_reach_tick` posiert Bild 0 (kein `++`); die
folgenden Takte `anim_frame+1 < 16 -> ++` zeigen 1..15; im 17. Takt `+1 >= 16` -> Clip 2 Bild 0 / Blende 7. Das deckt sich
Takt fuer Takt mit Sub 6 (Phase 0 faellt in f314, Rueckgabe 1 im Takt von Bild 15, Phase 2 posiert Clip 2 Bild 0, §2).

## 2. RE-Gate (Stichproben SELBST disassembliert)
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE` (md5 b55fdaa5; Auslieferungs-Stub
@0x80026e4c `08 00 e0 03 21 10 00 00` = NICHT die gepatchte EXE) und `info/Re1.5/PSX/BIN/STAGE1.BIN` (md5 a482d196).

| Behauptung (Diff/Dossier) | Selbst gefundene Instruktionen | Urteil |
|---|---|---|
| M1 Sub 6 0x800517f0 (Spieler-Tabelle @0x80073e30[6]): Phase 0 +0x94 = 1 @0x80051854, +0x95 = 0 @0x80051864, +0x8f = 7 @0x80051874, faellt in f314 @0x8005188c | `table 0x80073e30`: [4] 0x80030af0, [6] 0x800517f0, [9] 0x80031360, [19] 0x80052864 / `80051818 beq v1,zero,0x80051844` + Delay `ori v1,zero,0x1` / `80051844 sb v1,6(a0)` / `80051854 sb v1,148(v0)` / `80051864 sb zero,149(v0)` / `80051874 sb v0(=7),143(v1)` / OHNE Sprung `80051884 lw a0,368` / `80051888 lw a1,372` / `8005188c jal 0x8001f314` (a2 = 0 @0x80051880) / `800518a8 addu v1,v1,v0` / `800518b0 sb v1,6(a0)`; Phase 2 `800518b8 sb v0(=3),6` / `800518c8 sb v0(=2),148` / `800518d8 sb v0(=7),143` -> `800518f0 jal 0x8001f314` | stimmt |
| f314 posiert +0x95 vor dem Vorschub, +0x8f-- im Blend | `8001f324 lbu v0,148(t0)` / `8001f35c lbu v0,149(t0)` (rueckwaerts `8001f344-54 subu; addiu -1`) / `8001f36c sw a2,360(t0)` / `8001f38c jal 0x8001f3bc`; darin `8001f5a8 lbu v0,143` / `8001f5b0 addiu v0,v0,-1` / `8001f5b4 sb v0,143`, danach erst `8001f610 lbu v0,149(v1)` (Vorschub) | stimmt |
| M3a Gang [4] 0x80030af0: f314 @0x80030c78 VOR der Ankunft +0x5 = 6 @0x80030cf0 | Phase 2: `80030c5c jal 0x800245d8` (pos_advance) / `80030c78 jal 0x8001f314` (+0x84-Paar 0x800acbc4/c8) / `80030cbc jal SquareRoot0` / `80030cc4 slti v0,v0,100` / `80030cd8 jal 0x8004ef90` (Ankunfts-Bit) / `80030cf0 sb v0(=6),-13735(at)` (0x800aca59) / `80030cfc sb zero,0(s0)` (+0x6) | stimmt |
| M3a Drehen [9] 0x80031360: +0x5 = 6 @0x800313ec VOR f314 @0x80031494 | `800313d0 jal arc_test` / `800313ec sb v0(=6),-13735(at)` / `800313f4 sb zero,-13734(at)` / `800313f8 jal 0x8004ef90` / ... `80031494 jal 0x8001f314` (gleiches Paar) im SELBEN Takt | stimmt |
| M3a Bildfolge FUN_8001c6e8: SCD @0x8001cdec vor Gegnern @0x8001ce04 vor Spieler-Dispatcher @0x8001ce0c; Kommando 4 = 0x80030660 dispatcht +0x5 einmal | `8001cdec jal 0x8003f038` / `8001ce04 jal 0x8001a50c` / `8001ce0c jal 0x80031c44`; `8003068c lbu v0,-13735(v0)` (0x800aca59) / `8003069c addiu at,at,15920` (0x80073e30) / `800306ac jalr v0` | stimmt |
| M3a SCD-Rueckgabewerte: While kehrt mit 1 zurueck (@0x8003f840), Evt_end im Gosub mit 1 (@0x8003f22c), Evt_next mit 2 | Executor `8003f0a0`: `8003f10c jalr v0` / `8003f118 beq v1,s3(=1),0x8003f0ec` (1 = naechster Opcode im SELBEN Takt) / `8003f120 beq v1,2,0x8003f17c` (2 = naechster Thread); `table 0x800744a8`: [1] 0x8003f1f0, [2] 0x8003f258, [15] 0x8003f6f4, [64] 0x80041be4. Evt_end: Ebene 0 -> `8003f20c ori v0,zero,0x2`, sonst Ruecksprung `8003f220 sw v0,28(a1)` + `8003f22c ori v0,zero,0x1`; Evt_next `8003f26c ori v0,zero,0x2`; While: wahr `8003f818 bne s2,zero` + Delay `ori v0,zero,0x1`, falsch PC = Schleifenende `8003f838 sw v0,28(s1)` + `8003f840 ori v0,zero,0x1`; Plc_dest `80041c14 sb v0(=4),4(a1)` / `80041c18 sb a2,5(a1)` / `80041c1c sb zero,6(a1)` | stimmt — ist das Ankunfts-Bit gesetzt, laufen While (falsch) -> Evt_end -> naechster Plc_dest im SELBEN SCD-Takt, also VOR dem Dispatcher: kein Sub-6-Bild |
| M2 Schub 0x80052864 Phase 5/6/7 | Phase 5 `80052af4 jal 0x8001f314` (a2 = 0, +0x84/+0x16c) VOR `80052b44 jal 0x8004efe4` (Bit); `80052b60 sb v0(=6),6` / `80052b70 sb v0(=0x11),148` / `80052b80 sb zero,149` (Delay von `j`); Phase 6 `80052b8c ori a2,zero,0x1` / `80052b98 jal 0x8001f314` / `80052bb4 addu v1,v1,v0`; Phase 7 `80052bcc jal 0x80045024` (SE 0x02070000) / `80052be0 sb v0(=6),5(v1)` / `80052bf0 sb zero,6(v0)`, KEIN f314 | stimmt |
| M3b Clip-Setzer 0x80115d94 (STAGE1) posiert nicht | `80115da0 sb a0,148(v0)` / `80115db0 sb zero,149(v0)` / `80115dc0 jr ra` + Delay `80115dc4 sb v0(=7),143(v1)` — kein jal | stimmt |
| M3b Tod: Landung @0x801148f4 ohne f314, FINISH f314 @0x8011493c, Timer 11..0, Zustand 7 | `801148e4 sw v1,56(a0)` / `801148f4 jal 0x80115d94` (a0 = 0xa) / `8011490c jal 0x800453d0` (Se 5) / `80114910 sb v0(=11),469(v1)` / `80114928-30` +0x7++ / `8011492c j 0x801149b4` (kein f314); FINISH `8011493c jal 0x8001f314` (+0x84/+0x16c) / `80114950 lbu a0,469` / `80114958 addiu v1,a0,255` / `8011495c bne a0,zero` / `80114974 jal 0x80115d74` (a0 = 0) / `80114978 sb v0(=7),4(v1)` | stimmt — 12 FINISH-Takte posieren Bild 0..11, die Leiche haelt 11 |
| M3b ORIENT 0x801152e0: Clip 8 @0x80115310, Effekt @0x80115334, f314 @0x8011536c, Effekt am Umlauf @0x80115394 | `80115310 jal 0x80115d94` (a0 = 8) / `80115334 jal 0x80019700` (a0 = 0x1000, a2 = [+0x188]+408) / `80115354 sb +0x6++` / `8011536c jal 0x8001f314` / `80115374 beq v0,zero` / `80115394 jal 0x80019700`; Zeichenschleife `8001d108 jal 0x8001e8c8` (je Entity, nach der Logik) | stimmt (dass 0x8001e8c8 die Teil-2-Matrix schreibt, nicht selbst nachgeprueft; betrifft nur den Effekt-Ort) |

Guess-Tells in den hinzugefuegten Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|approx|geschaetzt|vermutlich|getenv`):
siehe §2.1.

### 2.1 Guess-Tells / Konstanten
`git diff master...HEAD -- re15_port | grep '^+' | grep -iE 'deferred|tunable|interim|for now|faithful|plausib|TODO|approx|geschaetzt|vermutlich|getenv'`:
**0 Treffer** (auch kein neuer Env-Schalter). Neue verhaltensrelevante Konstanten: Sub-6-Saat Clip 1 / Bild 0 / Blende 7
(@0x80051854/64/74), Kraehen-Setzer Bild 0 / Blende 7 (@0x80115db0/dc4), Schub-Phase-7-Halt = Bild fc-1 rueckwaerts
(Phase 7 ohne anim_set @0x80052bc0-bf0) — alle mit Adresse. Port-Zustaende ohne Spielwert und als solche gekennzeichnet:
`s_reach_saat` 0/1/2, `s_sp = 8` ("Port-Merker"), `RE15_R39K_SCHUB_SAAT 0xFE` (K4, Abnahme 0).

## 3. Messung Nutzer-Punkt (selbst, an der exe)

Wortlaut: "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink
Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen."
VERTRAG 0 (Spur K): "John 1171 "Elza hurry up": Heran-Winken statt Klettern; uebrige Szenen-Animationen 1171".

### 3.1 ECHTER WEG am Endstand (Nutzerweg 1131 -> 1141 -> Dachtuer -> 1171 -> Tor -> Szene -> 10B1)
Befehl (Scratchpad `K39abn1/lauf.sh` = Kopie von `K39abn0/lauf.sh`, Wegplan `K39abn0/wegplan_1141_1171.txt` =
`integration_r38_dining1141` Teil D bis zur Dachtuer): `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500
RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_MSG_LOG=1 RE15_EVT_TRACE=1 RE15_STATE_LOG=state.txt
RE15_ANIM_TRACE=anim.txt RE15_WEGPLAN=wegplan.txt "RE15_EXIT_AT=60#10b1" RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78
RE15_GIVE=0x43:1 RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0 RE15_R38F_GORILLAS_TOT=600 RE15_R38F_GORILLAS_STARR=1
RE15_SCD_TRACE=1 "RE15_FIRE_AOT=5@60#1171" "RE15_FRAMEDUMP=60-1150/1:fd_"`, exe-Kopie `re15_pc_k39abn1_A.exe` (sha256
cc5086f4... = build/platform/pc/re15_pc.exe 21:01), beschleunigter Renderer (kein SOFTWARE_RENDER). Lauf `K39abn1/echtA`,
EXIT=0. debug.log: `DOOR FIRE slot=0` (1131 -> 1141), `DOOR FIRE slot=1 ... target_cut=9` (1141 -> 1171),
`DOOR FIRE slot=5 rect=(-11065,-27850,hw=875,hh=600)` (Tor) -> `F61 room=1171 Evt_exec sub=2` -> `[msg] room=1171 id=0` /
`id=1` / `id=3` -> `PC loaded room10b1.rdt`. Vergleichslaeufe gleicher Weg (Abnahme 0, selbe Befehlszeile ohne SCD-Trace):
`K39abn0/runA_neu` (Stand 6d027394 = vor NB1), `K39abn0/runB_v0826` (Nutzer-exe v0.8.26). Auswertung `K39abn1/runs.py`,
`elza_cmp.py`, `kraehe_cmp.py` (gerendert = letzte Pose-Zeile je Bild/Aktor). ROOM1171 = Abschnitt 4 des Pose-Stroms.

**John (Aktor 1, Kernbefund).** F530-F657 ("Elza, hurry up!", Plc_motion(0,2,0) @0x014CE): `mo=2 clip=2 fc=96 loco=0`
Bild 0..95, dann Halten 95 = RBJ rec1 Clip 2 (Heran-Winken). v0.8.26 an derselben Stelle: `mo 2 / fc 50 Bild 1..49`
(Bank-0-Kletter-Clip). Johns 1082 Zeilen sind **bytegleich** mit dem Abnahme-0-Stand (md5 c0350dd2 beide, 0 Differenzzeilen).
BILD `K39abn1/hurry_up_head_vs_v0826.png` (eigene Framedumps 320x240): HEAD F532-F580 John dreht sich, hebt den Arm und
winkt heran; v0.8.26 F548/F564 John knickt ein und geht in die Hocke (= "komische Kletter Animation").

**Elza (Aktor 0) — M1/M2/M3a der Abnahme 0, echter Weg.** `runs.py echtA/anim.txt 0 4` (HEAD) gegen `K39abn0/runA_neu`
(vorher) — `elza_cmp.py`: 732 von 1143 Bildern anders, AUSSCHLIESSLICH in diesen Bereichen:

| Bild | vorher (6d027394) | HEAD | Original (§2) |
|---|---|---|---|
| F67-F69 | F67 Drehen 0, **F68 Clip 1 Bild 1** (Einzelbild), F69 Gang 0 | F67-F68 Drehen 0,1 -> F69 Gang 0 | Drehen-Ankunft F68 (+0x5 = 6 @0x800313ec, f314 @0x80031494); F69 SCD: While falsch -> Evt_end -> Plc_dest vor dem Dispatcher -> kein Sub-6-Bild |
| F104-F121 | F104-F118 Clip 1 **1..15** (15), F119 Clip 2 | F104 Gang 13 (Ankunft), F105-F120 Clip 1 **0..15** (16), F121 Clip 2 Bild 0 | Ankunft hinter f314 @0x80030c78 -> @0x80030cf0; Sub 6 Phase 0 im Folgebild, posiert Bild 0 (@0x8005188c) |
| F252-F268 | Clip 1 1..15, F267 Clip 2 | Clip 1 0..15 (F252-F267), F268 Clip 2 | Wurf P4 Umlauf @0x80115cf0-d18 -> Sub 6 Phase 0 im Folgebild = Bild nach Wurf-Bild 29 (F251) |
| F657-F674 / F735-F752 | Clip 1 1..15 ab F657 / F735 | F657 Gang 13 bzw. F735 Drehen 5 (Ankunft), Clip 1 0..15 ab F658 / F736 | wie F104 |
| F906, F912 | je **Clip 1 Bild 1** (Einzelbild) | Drehen Bild 4 bzw. 5 (Ankunft); F907 neues Drehen ab **Bild 0** (vorher 1, Walker-Init gleicher Clip) | wie F68; Phase 0 von [9] setzt +0x95 = 0 @0x800313b0 und posiert @0x80031494 |
| F999-F1011 | F999 Clip 17 rueckw. 9 (Bit-Takt), F1009 Clip 1 Bild 1 | F999 **Clip 18 Bild 0** (Bit-Takt), F1000-F1009 Clip 17 rueckw. 9..0, F1010 haelt 0 (Phase 7), F1011 Clip 1 **0..15** | Phase 5 f314 @0x80052af4 vor Bit @0x80052b44; Phase 6 @0x80052b98; Phase 7 ohne anim_set; Sub 6 im Folgebild |

Rohzeilen (Auszug `K39abn1/echtA/anim.txt`, Spalten Bild Aktor Typ mo Clip fc Zaehler Slot ...): `999 0 0 18 18 25 0 0` /
`999 1 73 18 18 25 0 0`; `1000 0 0 17 17 10 0 9 0 0 1` / `1000 1 73 17 17 10 0 9 10 1 1`; ... `1009 0 ... 9 0 ... 1` /
`1009 1 ... 9 0 ... 1`; `1010 0 0 17 17 10 9 0 0 0 1` / `1010 1 73 17 17 10 0 0 10 1 0` (beide Slot 0); `1011 0 0 1 1 16 0 0`
/ `1011 1 73 1 1 16 0 0` -> Elza und John lassen im selben Bild los. Lade-Weg (`integration_r39_heliport1171_laden`,
`laden_79ab6884/anim.txt`) identisch um -66 Bilder: F0-F1 Drehen 0,1 -> F2 Gang 0; F37 Gang 13 -> F38 Clip 1 Bild 0;
F840 Drehen 4 -> F841 Drehen 0; F846 Drehen 5 -> F847 Schub; F933 Clip 18 Bild 0; F934-F943 9..0; F944 haelt; F945 Clip 1 0.
SCD-Spur am Ankunftsbild (`RE15_SCD_TRACE`, Thread 10): `F68 @0x16A0 op=0x0F / @0x16A8 op=0x02` (Bit noch nicht gesetzt,
Evt_next) -> `F69 @0x16A0 op=0x0F / @0x16AC op=0x01 / @0x13D4 op=0x40` (While falsch, Evt_end, naechster Plc_dest im SELBEN
Takt) = genau die Original-Folge (§2: Rueckgabe 1 = weiter im selben Takt).
Nicht durch NB1 veraendert und fuer den Nutzer unauffaellig: F913 `mo=5 clip=5 fc=20 Bild 0` (EIN Bild zwischen Drehen-
Ankunft und Schub-Ansatz) = Schub Phase 0 faellt in Phase 1 und posiert W-Clip 5 Bild 0 (`800528c0 sb v0(=5),148` /
`800528d0 sb zero,149` / `8005294c jal 0x8001f314` mit +0x170/+0x174), Kurs sofort gerastet -> Phase 2 im Folgebild; = Original.

**Kraehen (Aktoren 2/3) — M3b, echter Weg.** `kraehe_cmp.py echtA runA_neu 4 2|3`: Kraehe 2 `Bilder 1082/1082: gleich 0,
alt = neu+1 1081, sonst 1  F442 neu=(4,10,8) alt=(10,40,0)`; Kraehe 3 `... alt = neu+1 1081, sonst 1  F491 neu=(4,10,1)
alt=(10,40,0)`. D.h. jede gerenderte Kraehen-Pose zeigt jetzt das Bild VOR dem Vorschub (vorher immer eins voraus), die
einzige andere Abweichung ist das Landebild (Fall-Pose statt Clip 0x0a Bild 0). Rohzeilen Kraehe 2: Sturz F436-F441
`4 4 10 8 8` (kein f314 im Sturz, Normal-Lane 0x80114738: nur `jal 0x800453d0` / `jal 0x800245d8`), Landung F442 `4 4 10 8 8`
(state.log `st=3 ss3=2 mo=4 af=8`), FINISH F443-F454 `10 10 40 0..11`, ab F454 `st=7 mo=10 af=11` (Leiche haelt 11);
ORIENT F162 zwei Zeilen `4 4 10 3 3` (Effekt-Abfrage mit der HOVER-Pose des Vorbilds) und `8 8 8 0 0` (gerendert),
F163-F168 1..6, F169 `6` (Effekt am Umlauf, Vorbild) + `7` (gerendert), F170 0. Start: F61-F66 Clip 0 Bild 0, HOVER "0,0,1,2":
ASCEND posiert Clip 4 Bild 0, wechselt im selben Takt nach HOVER; HOVER Phase 0 setzt Clip 4 erneut (Setzer @0x80115164
+ f314 im selben Takt) -> Bild 0 zweimal = Original-Folge (vorher "1,1" = dieselbe Doppelung um eins verschoben).

**Kraehen — NEUER BEFUND (gegen das Original geprueft, NICHT behoben): HOVER-Phase 0.** Die Ereignis-Kraehen fliegen nach
dem ASCEND ~95 Bilder im HOVER (FLIGHT-2 Sub 1, state.log Kraehe 2 `F67 st4/ss1=1` bis `F161 ss1=2`, Kraehe 3 F76-F172), bevor
sie Elza greifen. Original HOVER 0x80115130 (STAGE1.BIN, selbst disassembliert): `80115144 lbu a0,6(v1)` / `8011514c beq
a0,zero,0x80115164` (Phase 0) / `80115154 beq a0,v0(=1),0x801151e0` (Phase 1). Phase 0: `8011516c jal 0x80115d94` (Clip 4,
+0x95 = 0, +0x8f = 7) + Delay `80115170 sh v0(=0xb4),140(v1)`, `8011518c sh v0,484(v1)` (+0x1e4 = Zufall & 0x3f, ggf.
negiert @0x801151b8-bc), `801151d4 addiu v0,v0,1` / `801151d8 j 0x801152cc` + Delay `801151dc sb v0,6(v1)` -> **Epilog
@0x801152cc, KEIN f314, keine Lenkung, kein pos_advance, keine Kontaktpruefung in diesem Takt.** Erst Phase 1: `801151e8 jal
0x8001f314` (Rueckgabe 1 -> `8011520c addiu v0,v0,-1` = +0x6 = 0), `80115238 jal 0x8001a8f8` (Lenkung), `80115240 jal
0x800245d8` (pos_advance), `80115254 lbu v0,464(a0)` (Kontakt +0x1d0) -> Griff `801152a0`/`801152ac`/`801152b8`. (Zum
Vergleich fallen ASCEND 0x80114fb8 Phase 0 -> 1 -> 2 @0x8011502c/@0x80115068 und SPIN 0x801153ac Phase 0 -> 1 @0x80115450
OHNE Sprung in ihren f314 @0x801150f0 / @0x80115524, ORIENT ebenso @0x80115354 -> @0x8011536c.)
Port `engine/src/enemy_ai_common.c:6589-6605` (`case 1: /* HOVER 0x80115130 */`, seit 74121bb81 2026-07-06, von K nicht
angefasst): der Phase-0-Block setzt `sub_state_2 = 1` und faellt im SELBEN Takt in `if (re15_crow_anim(e))` + Lenkung +
`e->y += vvel` + `re15_crow_advance` + Kontakt. Gemessen (echtA, Kraehe 2, gerendert): `67:4/0 68:4/0 69:4/1 ... 77:4/9
78:4/0 ... 87:4/9 88:4/0` (Zyklus 10 Bilder, kein Halt); state.log `F67 ss1=1 ss2=0 mo=4 af=0` -> `F68 ss2=1 af=0` -> `F69
af=1`. Original-Folge nach den Instruktionen: F67 0 (ASCEND-f314), **F68 0 (Phase 0, Halt)**, F69 0 (erstes Phase-1-f314),
F70 1, ..., F78 9, **F79 9 (Halt)**, F80 0 -> Zyklus 11 Bilder; dazu bewegt sich die Port-Kraehe in jedem 11. Takt einen
Schritt zu viel (der Griff-Zeitpunkt F162/F173 und damit Elzas Opfer-/Wurf-Folge koennen im Original spaeter liegen).
Gleiche Klasse wie das Landebild (Clip-Setzer ohne f314 -> die vorige Pose bleibt), das NB1 behoben hat — der HOVER-Halt
wurde dabei uebersehen; das Dossier 12.3/12.6 meldet "Start HOVER 0,0,1,2" ohne Vergleich gegen das Original.
Gegenprobe mit dem Ghidra-Dump (`RE_15_Quellcode_Overlays/STAGE1_full/FUN_80115130.c`): `if (+6 == 0) { +0x8c = 0xb4;
FUN_80115d94(4); +0x1e4 = ...; +6 += 1; } else if (+6 == 1) { f314(...); ... 0x8001a8f8(...,0x32); 0x800245d8(0); Kontakt }`
— Phase 0 und Phase 1 schliessen sich aus.

**Kraehen — ZWEITER NEUER BEFUND (Bewegung, gleiche Szene): SPIN-Phase 2 dreht weiter.** Nach dem Wurf (F222) fliegen beide
Kraehen im SPIN (FLIGHT-2 Sub 3) bis zu ihrem Tod (state.log Kraehe 2 `F222 ss1=3` ... `F295 ss1=1` -> `F296 ss1=3` ... `F429
st3`). Original SPIN 0x801153ac: Phase 0 `801153f8`-`80115450` faellt in Phase 1 `80115454`: `80115460 lhu v0,106(v1)` /
`80115468 addiu v0,v0,60` / `8011546c sh v0,106(v1)` (Gier +60), Zeitgeber 32 -> Phase 2 (`801154b8` +0x1e4 = 10, `801154cc`
Zeitgeber 40); Phase 2 `801154d0`-`801154e8` zaehlt NUR den Zeitgeber herunter (bei 0 `jal 0x80115d74` a0 = 1 -> HOVER) und
springt in den Schwanz `801154ec` (y += +0x1e4, `80115508 jal 0x800245d8`, `80115524 jal 0x8001f314`) — **ohne Gier-Aenderung**
(Ghidra `FUN_801153ac.c`: Fall 2 `goto LAB_801154ec` an `+0x6a += 0x3c` vorbei). Port `enemy_ai_common.c:6624`
(`e->rot_y = (int16_t)(e->rot_y + 60);` steht VOR der Phasen-Unterscheidung, seit 74121bb81) dreht in Phase 2 weiter.
Gemessen (echtA, Kraehe 2, ss2 = 2 = Phase 2): `F254 rot 5511`, `F255 5571`, ... `F294 7911` (+60 je Bild ueber alle 41
Phase-2-Bilder). Original: Gier bleibt in Phase 2 stehen -> die Kraehe fliegt 41 Bilder geradeaus (pos_advance entlang
der Gier, +0x8c = 180), statt im Kreis. Das ist Flugbahn/Ausrichtung, kein Clip-Bild; es betrifft aber genau die
Kraehen-Ablaeufe "Wurf" der Szene, die Abnahme 0 M3(b) gegen das Original verlangt hat.

## 4. Vertrag
* Pfade (`git diff master...HEAD --name-only`, 23 Dateien): `analysis/befunde_runde39/K_{heliport1171,abnahme_0}.md`,
  `K_belege/*.png`, `engine/src/{actor_locomotion,elza_r39,enemy_ai_common,enemy_common,game_step_common,heliport1171_r39,
  player_common,scd_room_setup,szenenleon_r36}.c`, `include/re15_heliport1171_r39.h`, `tests/integration/
  test_r39_heliport1171.cmake`, `tests/unit/{probe_crow_death,test_plc_back_yaw_1090,test_r37_roy,test_r38_heliport1171,
  test_r39_heliport1171}.c`, `tests/unit/probes/r39_heliport1171.cmake`. Keine release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists (Registrierung ueber den GLOB probes/*.cmake). OK.
* Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse / Sprachdateien: keine. Die drei `re15_game_flag_set` der
  hinzugefuegten Zeilen stehen nur in `test_r39_heliport1171.c` (Testaufbau (3,0xC2)/(3,0x7D)); im Motor keine. OK.
* Gemeinsame Dateien, Zeilen gesamt (`--numstat master...HEAD`) / davon NB1 (`a2d1e1dc..HEAD`): actor_locomotion.c +6/-2
  (NB1), elza_r39.c +2 (NB1, **Datei der Spur H**), enemy_ai_common.c +52/-25 (NB1 +21/-13), enemy_common.c +3/-1 (K1),
  game_step_common.c +11/-3 (NB1), player_common.c +2/-1 (NB1), scd_room_setup.c +2 (Include + Installer am Blockende),
  szenenleon_r36.c +30/-10 (NB1 +22/-8). Jede Stelle traegt "Runde 39 Spur K"/"Spur K NB1"; die groessten Eingriffe sind der
  Spieler-Schub Phase 5/6/7 (szenenleon_r36.c) und der ORIENT-Effekt-Block (enemy_ai_common.c, ~10 Zeilen). Laut VERTRAG
  1.3 sind K nur scd_room_setup.c und heliport1171_r38.c zugeteilt; game_step_common.c, actor_locomotion.c, player_common.c,
  elza_r39.c, szenenleon_r36.c, enemy_common.c stehen dort nicht (enemy_ai_common.c = D/E). Form = kleine kommentierte
  Haken, Logik in der eigenen Datei heliport1171_r39.c — haelt, mit Hinweis fuer die Integration (s. H1).
* Fremde Riegel angepasst, je mit Beleg — selbst nachgeprueft: `test_plc_back_yaw_1090.c` misst "nach der Ankunft" erst
  ab dem Folgebild; Beleg Rueckwaerts-Gang [8] `800312b0 jal 0x8001f314` VOR `80031318 sb v0(=6),-13735(at)` (selbst
  disassembliert) -> stimmt; die Kernpruefungen (Sub 6 Clip 1 nach der Ankunft, kein Rueckwaerts-Bild danach, genau ein
  Schritt) bleiben. `probe_crow_death.c` Leiche 12 -> 11 (FINISH 12 f314 posieren 0..11, §2) -> stimmt.
  `test_r38_heliport1171.c` flagwort: Effekt-Pose = Kraehen-Aktorfelder vor dem ORIENT-Bild (ORIENT `80115334` vor
  `8011536c`; die Teil-2-Matrix [+0x188]+408 schreibt die Zeichenschleife: `8001d108 jal 0x8001e8c8`, darin `8001e8ec lw
  s1,392(s0)` (+0x188) und `jal 0x8001e9ec` — Matrixinhalt nicht im Einzelnen verfolgt). `test_r37_roy.c` spur (H3 der
  Abnahme 0): INIT-Form nur als Lauf ab der ersten Elliot-Zeile (`anders`-Merker) -> umgesetzt wie empfohlen.
* Neue Tests: Unit-Teile `elza`/`kraehen` im bestehenden Test-exe (eigene Laufordner je Teil); Integrationsblock im
  bestehenden -P-Skript (Laufordner + exe-Kopie je Lauf, RE15_*-Umgebung geloescht, `cmake_minimum_required`). OK.

## 5. Riegel — Gegenprobe SELBST (alter Motor, neue Testquellen)
`git archive a2d1e1dc re15_port ":(exclude)re15_port/shared_assets"` nach Scratchpad `K39abn1/alt/` (Motor = Abnahme-0-Stand,
vor NB1; nichts im Baum veraendert), darin `tests/unit/test_r39_heliport1171.c` + `probes/r39_heliport1171.cmake` von HEAD
(Asset-Pfad auf den Baum umgebogen), eigener Configure (`cmake -G Ninja -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON`, PATH mit
msys64 zuerst) und Bau NUR des Ziels `test_r39_heliport1171` (186 Schritte, EXIT 0). Laeufe:
* `test_r39_heliport1171.exe elza` -> **EXIT 1, `FEHLER (4 Fehler)`**: `Sub-6-Lauf ab Bild 1: Clip 1 1 Bilder (Folge
  FALSCH)`, `ab Bild 37/186/591/669/943: Clip 1 15 Bilder (Folge FALSCH)`, `ab Bild 840/846: Clip 1 1 Bilder`; `FEHLER: M1
  ... (8 Laeufe: Start 0, 16 Bilder 0, Clip 2 ab 0 5)`, `FEHLER: M3a: kein einzelnes Sub-6-Bild ... (3)`, `FEHLER: M3a:
  Ankunftsbilder ... (0 von 6 ...)`, `FEHLER: M2: ... (Elza 933, John 933, Folge FALSCH)`.
* `test_r39_heliport1171.exe kraehen` -> **EXIT 1, `FEHLER (4 Fehler)`**: `Kraehe Slot 2: Start Clip 0 Bild 1, ORIENT ab
  Bild 96 (Folge FALSCH), Landung Bild 386 (Clip 10), Zustand 7 ab Bild 398 (Clip 10 Bild 12)`, Slot 3 entsprechend.
* Am Endstand (§0) dieselben Teile gruen. Die Integrationsbloecke werten genau die Groessen aus, die sich zwischen
  `K39abn0/runA_neu` (alt) und `K39abn1/echtA` (neu) unterscheiden (Clip-1-Laeufe 1..15 statt 0..15 + Einzelbilder, Clip 17
  rueckw. Elza F999 vs. John F1000, ORIENT ab 1, Leiche 12) -> am alten Motor rot. Riegel **vorhanden und messend**.
  Nicht abgedeckt (naturgemaess): der HOVER-Halt und die SPIN-Gier (§3.1, neue Befunde).
