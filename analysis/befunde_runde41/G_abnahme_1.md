# Runde 41 — Spur G "kanister2080" — UNABHAENGIGE ABNAHME 1 (2026-10-25)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, HEAD ebe8445f (Basis r41/integration 9f10bdb0).
Pruefer: Abnahme-Agent 1 (glaubt nichts Ungemessenes). Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).
Laufordner/Bilder (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g1`.

## 0. Stand der Abnahme
- [x] Lektuere AUFTRAG (inkl. Nachtraege 22-24b, FG-E3/FG-E4), VERTRAG, Dossier §8, Abnahme 0, Code-Diff 9f10bdb0..HEAD
- [x] Bau (local_build.sh configure + build)
- [x] gezielte Unit-Tests
- [x] gezielte Integrations-Tests (8/8)
- [x] RE-Gate Stichproben (13 Stellen selbst gelesen/disassembliert)
- [x] Messungen P1 / P2 / P3 (Leon + Elza; 12 exe-Laeufe + 2 ctest-Laeufe)
- [x] Vertrag
- [x] Urteil: NICHT BESTANDEN (N1 Zeile 42 abgeschnitten, N2 Gier-Sprung im Griffbild, N3 Aufnahme im echten Ablauf nicht verlaesslich)

## 1. Diff, Bau, gezielte Tests
- `git diff master --stat` zeigt 166 Dateien inkl. Loeschungen (kamerahalt_r41.c, synchro/STAGE2/room2030/main46.wav ...): das ist master
  NACH dem Abzweig (645fa45f), nicht G. Massgeblich ist `git diff 9f10bdb0..HEAD` (merge-base): 56 Dateien, davon 25 unter re15_port
  (+3161/-0); Code: kanister2080_r41.c (824), re15_kanister2080_r41.h (228), kanister2080_pc.c (253), gen/*.inc, Haken
  scd_room_setup.c (1 Zeile), scd_vm.c (1 Zeile), platform/pc/main.c (2 Zeilen), test_r36_kartenbits.c (+4).
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. Kontrolle: `find re15_port/{engine,include,platform} -newer re15_pc.exe -name '*.c|*.h|*.inc'` leer,
  `git status` sauber -> die exe (04:00) ist der Stand HEAD ebe8445f (letzter Code-Commit ebe8445f).
- `ctest -R "unit_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_|unit_msg_|unit_action_msg_gate|unit_alligator_ai|unit_gator_"
  --timeout 600 -j4` -> **`100% tests passed, 0 tests failed out of 39`** (80,35 s). Log <SP>/ctest_unit.log.
- `ctest -R "^integration_r41_kanister2080$|^integration_r41_kanister2080_kampf$|^integration_r41_re2gator$|^integration_r35_granate$|
  ^integration_r38_wasser$|^integration_r41_wasser20b0_npc$|^integration_r37_itemband$|^integration_r36_tueren$" --timeout 1800 -j3`
  (G + B's Riegel + alle Riegel, die durch ROOM2090/2091 laufen) -> **`100% tests passed, 0 tests failed out of 8`** (1083,82 s;
  kanister2080 163,77 s, kanister2080_kampf 249,84 s, re2gator 230,32 s, r37_itemband 833,92 s). Log <SP>/ctest_integ.log.
  Der kampf-Lauf dieses ctest (build/tests/integration/r41_kanister2080_kampf_wd/lauf_0d602269) ist zugleich Messung 3.4.

## 2. Messungen P1 / P2 (Messkopie re15_pc_g41abn1.exe; Laufordner <SP>/runs/<lauf>; Helfer <SP>/lauf.sh)
Gemeinsam: RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore, Leon RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2,
Elza RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1. Die P1/P2-Pfade (Installer 2080, Knall, Spawn,
Prop) sind seit Abnahme 0 unveraendert (`git diff 489c0756..HEAD` beruehrt nur Szene/Wurf/Rueckweg); hier Regressionspruefung.

### 2.1 P1 Leon, echter Leiterweg 2090 -> 2080 (DOOR16) - Lauf p1_leon
Befehl: `RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1 RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_KEIN_GATOR=1
RE15_PLAYER_POS=-8900,-25244,2048 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W1.0,A0.2,W2.0
RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log RE15_FRAMEDUMP=0-400/20:f_ RE15_EXIT_AT=400#2080` -> rc 0, 31 s.
(Erster Versuch ohne INPUT_SCRIPT_BASIS=spiel: der Aktionsdruck fiel in den Vorspann, Leon blieb unten, die Spinnen (0x25, in diesem
Zweig noch in 2090) toeteten ihn -> Titel, rc 124. Messfehler des Aufbaus, kein Befund.)
- debug.log Z.151 `[tuer] Sequenz Archiv 2 DOOR16 ...` -> Z.171 `[k2080] F31 Knall: Knall-Bank 19976 B in der Tuerbank, Satz 0 (RE2 ROOM1060
  Se_on 0x0a @0x0F48 ...)` + Z.172/173 `[se] Stimme: se=0 layer=0/1 vag=0 note=67 ... center=91 vol=127 (11025 Hz)` -> Z.174 `Bit 114 = 1`
  -> Z.176 `ROOM2080 nach Bit 110: 8 lebende Zombies` -> Z.184-191 8 x `Sce_em_set type=0x16 behavior=0x02 slot=0..7 pos=(..,-14400,..)`.
- Mitschnitt (`knall_im_mitschnitt.py`, Kopie <SP>/tools_k mit RE15_INFO_DIR = Hauptbaum): `RE2_1060 (11025 Hz): t=15.57 s Bild 467
  r=0.891`; 867 - 400 = Bild 467 = erstes 2080-Bild. DOOR04/R1030 kein Treffer.
- Bild <SP>/p1_leon_2080_bogen.png (Cut 4, F40..F400): die Zombies stehen oben am Regal/Rechner.

### 2.2 P1 Elza, 2091 -> 2081 - Lauf p1_elza
Wie 2.1 mit Elza-Wahl, `RE15_DEBUG_JUMP=2091@250`, `RE15_EXIT_AT=400#2081` -> rc 0. `[tuer] Sequenz ... DOOR16` -> `F31 Knall ...` ->
`ROOM2081 nach Bit 110: 8 lebende Zombies` -> 8 x Sce_em_set 0x16 y -14400. Mitschnitt `RE2_1060 ... Bild 1175 r=0.891` = 1575 - 400.

### 2.3 P2 Gaskanister an der Reling - Lauf p2_cut5
`RE15_NOAUDIO=1 RE15_DEBUG_JUMP=2080@250 RE15_PLAYER_POS=-12000,-24000,0,8 RE15_FORCE_CUT=5 RE15_FRAMEDUMP=40:f40.ppm RE15_EXIT_AT=45#2080`:
`[k2080] F1 Kanister pos=(-6745,-14400,-17625) Cut 5 Kasten=(159..177,75..158) Dreiecke 46`. Bilder <SP>/p2_gascan_vs_port_x2.png,
<SP>/p2_marke_zoom_x4.png: der Kanister steht aufrecht auf der roten Marke am Balkonende zwischen den Relingpfosten.

## 3. Messungen P3 (echter Ablauf; Rezept des Riegels kampf: <SP>/kampf.sh)
Gemeinsam (wie integration_r41_kanister2080_kampf): `RE15_DEBUG_JUMP=2090@250` (Elza 2091) `RE15_SET_FLAG=9:110,3:245 RE15_GIVE=3:60
RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W6.0,18 x
(M0.6,MA0.1,M0.3,MA0.1,W3.0) RE15_STATE_LOG=state.log RE15_PLAYER_POS=<x,z,gier>`, KEIN RE15_GB_TEST / RE15_GATOR_KANISTER /
RE15_KEIN_GATOR / RE15_K2080_WURF_GIER. Messhaken RE15_K2080_GATOR_HP=24990 (ein echter Treffer statt 43) nur wo angegeben.
Bit 110 kommt per RE15_SET_FLAG, weil Spur F (setzt Bit 110) in diesem Zweig fehlt.

### 3.1 Szene 2080 Leon (Lauf s_leon, Start (-6000,-17500,3072), Haken GATOR_HP, `RE15_FRAMEDUMP=380-580/2:f_ RE15_EXIT_AT=580#2080`)
- debug.log: `F902 FG-E3: Alligator gesichert Slot 35 (-4889,-1200,-7442) Gier 2365 hp 24980 Phase 2` -> `F902 AUSLOESER geschwaecht ...
  Treffer 1` -> `ROOM2080 SZENE (Marvin 0x40, 8 lebende Zombies, 582 B Programm)` -> `F8 Tuer-Ton DOOR06 Satz 1` -> 8 x `Schuss n: Treffer
  Slot s ... hp=-1 state=3 zeile=5` + `Kopf ab ... n von n` (F118..F352, Ziel-Poll 1-4 Bilder je Ziel, `Ziel Slot 4 ausgerichtet` F151 nach
  `Ziel 2` F148) -> `F405..F435 NPC Lauf Sub 5` -> Plc_motion(1,11) F440 -> `F465 Kanister aufgehoben (Loco c11 Bild 24 ...)` ->
  Plc_motion(1,11)+Plc_flg 0x80 F500 -> Cut_chg(7) F530 -> Zeile 42.
- state.log Marvin (Slot 9): F440 mo=11 af=0 r=93, F464 mo=11 af=24 **r=93**, **F465 r=3072** (ein Bild), F500 af=0 (rueckwaerts), F525 af=24,
  F530 Eigenbank mo=6.
- Bilder <SP>/s_leon_knien_wurf_zoom2.png (Cut 5 Ausschnitt 2x, F440..F528; Cut 7 F530..F578): F440 Marvin von hinten an der Reling, F452-F464
  kniet er (der stehende Kanister ragt ueber ihm auf) - **kein Klettern mehr** (Nachtrag 24b erfuellt); **F466: Marvin steht ploetzlich im
  Profil** (Sprung der Gier 93 -> 3072 = 98 Grad in einem Bild), der Kanister zeigt auf die Kamera; F468-F528 kniet/steht er im Profil.
  Der Bildbogen des Bau-Agenten `G_belege/nb1_m1_nachher_marvin_f437-530.png` stammt aus dem Bau 01:55 (vor d0c4f479: damals Gier 0 =
  Blick ueber die End-Reling) und zeigt diesen Sprung deshalb nicht - er ist NICHT der gebaute Endstand.
- John (Lauf s_elza, dasselbe mit Elza/2081): F464 r=93 -> F465 r=3072, Bilder <SP>/s_elza_knien_zoom2.png (F464 von hinten, F466 Profil).
- Zeile 42: state.log `msg(a=1 fsm=1 id=42` in **52 Bildern (F530..F581)**, danach in 2090 a=0 (Lauf k_leon). Im Bau von Abnahme 0 stand
  dieselbe Zeile **99 Bilder** (g0/runs/p3r_leon F569..F667, endete von selbst 4 Bilder vor dem Raumwechsel). Jetzt schneidet die Tuer
  nach 2090 (1 Bild nach dem Loslassen) die Zeile nach 52 von 99 Bildern ab (s. Mangel N1).

### 3.2 RE2-Folge nach dem Loslassen selbst gelesen (info/re2leon/PL0/RDT/ROOM7020.RDT, SCD-main-Basis @RDT 0x1128, Fundstelle des
Musters `49 00 40 fb 40 02 00 04 04 ff 18 0d 0b 0c 11 00 08 00` einmalig @RDT 0x18B6)
| SCD | Bytes | Bedeutung |
|---|---|---|
| 0x0764 | `2b 00 05 00 00 00` | Message_on 5 (Adas Zeile) |
| 0x076A / 0x0786 | `09 0a 1e 00` / `09 0a 14 00` | Sleep 30 / Sleep 20 |
| 0x078E | `49 00 40 fb 40 02 00 04` | Super_reset (loslassen) |
| 0x0796 | `04 ff 18 0d` | Evt_exec sub13 (Fall) |
| **0x079A / 0x079B** | **`0b` / `0c`** | **Wsleep / Wsleeping: warten, bis Adas Zeile zu ist** |
| 0x079C-0x07A4 | `11 00 08 00 02 00 12 04 21 05 0e 00` | Do/Evt_next/Edwhile Ck(5,0x0e) (Aufschlag) |
| 0x07A8 | `09 0a 0f 00` | Sleep 15 |
| 0x07D4 | `29 0b` | Cut_chg 11 (Fangender) |
| 0x081A / 0x081B | `0b` / `0c` | Wsleep / Wsleeping nach Leons Zeile @0x07E6/@0x07F4 |
Opcode-Tafel @0x800a74c8 (re2_disasm.py table): [11] -> 0x80053a8c, [12] -> 0x80053ac0. Wsleep @0x80053a8c: Zaehler +1
(`lbu/addiu/sb v0,8(v1)` @0x80053a98-a4), pc+1, Rueckgabe 1. Wsleeping @0x80053ac0: `lw v0,-1064` = 0x800cfbd8 `andi 0x20`
@0x80053acc, `lbu v0,-30916` = 0x800e873c `andi 0x80` @0x80053ae4 -> je `bne ... 0x80053b14` (Rueckgabe 2 ohne pc+1 = weiter warten);
sonst pc+1, Zaehler -1. 0x800e873c = 0x80 setzt FUN_8002fe38 (`addiu t2,zero,128` / `sb t2,-30916(at)` @0x8002fe80/88, selbst
disassembliert). **Der Port uebernimmt das Warten @0x081A (Dossier 8.3: 0 Bilder, Leons Zeile fehlt), nicht aber @0x079A**:
`baue_szene` schreibt nach dem Signal WURF nur `Sleep 1` + Tuer nach 2090 Cut 12 (kanister2080_r41.c Z. 279-287), `baue_rueck` pollt
den Aufschlag ohne Wsleep. Folge gemessen (3.1): Zeile 42 steht 52 statt 99 Bilder und verschwindet mit dem Raumwechsel.

### 3.3 Fall, Aufnahme, R1 Leon (Lauf k_leon, wie 3.1, `RE15_FRAMEDUMP=0-640/2:f_ RE15_EXIT_AT=1000#2090`)
- debug.log: `F581 Wurf: Start (-7135,-16896,-16769) v=(-10,20,25) a=(0,20,5) -> Landung (-7515,-2076,-12304) nach 38 Bildern (Wasser 2090
  y -1620)` -> `F581 FG-E3: ROOM2090 derselbe Alligator in Slot 35 (-4889,-1200,-7442) Gier 2365 hp 24980 Phase 2` (= Lage/HP beim Ausloesen
  F902) -> `F581 ROOM2090 RUECKKEHR ... 8 von 8` -> 2090 `F43 Aufschlag-Ton KANISTER2080.VBS Satz 0`, `F43 Aufschlag im Wasser bei
  (-7515,-2076,-12304) Bild 38`, `F43 ROOM2090 Kanister im Becken (-7515,-1200,-12304) = Aufschlagstelle (Bit 116)` -> `F609 Kanister
  verbraucht (R1-Tod) -> Bit 116 = 0`.
- gator_boss.log nach der Szene: `KANISTER gesetzt (-7515,-1200,-12304)` -> `Zielmodus 1 (Anschwimm-Sperre) laengs=6221 F102` ->
  `KANISTER Stufe 1 laengs=5782 quer=428 sub 2 -> 1 F104` -> `angehaengt ... F116` -> `getragen (Stufe 7)` -> `R1 VSync 4 (Zeitlupe) F570`
  -> `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 5 F570` -> `KAMERA R1 zurueck -> Cut 0 F606` -> `R1 ende (Leiche) pos=(-2856,-6884)`. Kein
  `GATOR VERSCHWUNDEN`/`RE2 AUFTAUCHEN`/`INTRO ende` nach der Szene (nur im ersten Besuch Z.13/41). Zeit Loslassen -> Aufnahme 1 + 104 =
  105 Bilder (3,5 s) - wie der Bau-Agent.
- state.log 2090 F1-F57 cam=12 (Spieler gehalten pm=2 ab F9; F1-F5 pm=0), F60 cam=0. Der Alligator schwimmt waehrend des Falls weiter
  (F6 mo=1 ...) und schiebt den Spieler (-5790,-6245) -> F33 (-4479,-5852), gr=0 (vom Bau-Agenten benannt, OFFEN 3).
- Bilder: <SP>/k_leon_fall_cut12.png (F2/F4 schwarz = Raumwechsel, F6-F58 Cut 12; F26/F30 schwimmt der Alligator dicht vor der Kamera,
  F34-F42 faellt der Kanister von rechts oben zur Wasserlinie, F60 Rueckschnitt Cut 0), <SP>/k_leon_aufnahme_cut0.png (F96-F130 Cut 0:
  die Aufnahme liegt in der dunklen rechten Bildecke und ist ohne Aufhellung kaum zu sehen; im Vordergrund greift eine Spinne 0x25 Leon an),
  <SP>/k_leon_r1.png (F570-F600: Schnitt auf Cut 5, Explosion am Kopf, F610 zurueck Cut 0).
- Elza (ctest-Lauf lauf_0d602269/kampf_elza): `KANISTER Stufe 1 ... F103`, `angehaengt F115`, `R1 VSync 4 F888`, `R1 ende (Leiche)
  pos=(10684,-8412)` (Leiche ausserhalb des Beckens x > 7200 - Spur B, vom Bau-Agenten gemeldet).

### 3.4 Nachtrag 23 - Aufnahme im weiteren Kampf, eigene Verlaeufe (ohne RE15_GB_TEST/RE15_GATOR_KANISTER/RE15_K2080_WURF_GIER)
Schiess-Skript lang (60 Bloecke, <SP>/schiess_lang.txt), Auswertung <SP>/gator_bahn.py (Gator-Wurzel je Bild aus state.log, Abstand zum
Kanister). Der Test-Spieler steht und schiesst (wie im Riegel kampf und bei allen Laeufen des Bau-Agenten).
| Lauf | Start (x,z) | Ausloeser | Lage beim Ausloesen | nach der Rueckkehr |
|---|---|---|---|---|
| k_leon / ctest kampf_leon | (-6000,-17500) | Haken HP, F902, Treffer 1 | (-5790,-6245) | Aufnahme **F104**, R1 F570 |
| ctest kampf_elza | (-6000,-17500) | Haken HP, F902 | (-5790,-6245) | Aufnahme **F103**, R1 F888 |
| v2_no_leon / v4_no_elza | (0,-7000) | Haken HP, F876, Treffer 2 | (6670,-5935) | Aufnahme **F3835** (2,1 min), Lauf endet F4000 |
| v5_west_leon / v3_west_elza | (-8000,-16000) | Haken HP, F802, Treffer 4 | (-8432,-10447) | **keine Aufnahme in 4000 Bildern**; Gator naechstens 1825 bei F47 (Kanister landet F43), dann schiebt er den Spieler nach Osten (F600 (-2162,-13139), ab F1500 Ostteil x 2175..6745), Abstand zum Kanister 10000..14200 |
| **v1_natur_leon** | (-6000,-17500) | **ohne jeden G-Haken**: echte RE2-Schwelle, F7375 nach **37 Treffern** | (5782,-26532) (SO-Ecke) | **keine Aufnahme in 9000 Bildern (5 min)**; Kampf bleibt im Sueden (Gator z -22424..-25532), Abstand zum Kanister nie unter 13098 |
Spinnen (0x25) sind in diesem Zweig noch in 2090 (Spur F) und binden einen Teil der Zielhilfe; die Zahlen haengen davon ab.
Befund: der Kanister faellt immer an dieselbe Stelle (Wurf-Gier 3072 fest -> (-7515,-12304), Nordwestbecken), gleich wo der Kampf
steht; der Alligator (Spur B) verfolgt den Spieler und lenkt nicht zum Kanister (vom Bau-Agenten benannt, 8.7.1 / Frage 8.8.1). Steht der
Kampf beim Ausloesen im Osten oder Sueden, kommt er nicht mehr vorbei. Die Angabe "Aufnahme in 5 von 6 Verlaeufen" ist mit unabhaengigen
Startlagen nicht nachvollziehbar: hier 2 von 4 Verlaeufen, und im einzigen Lauf ganz ohne Testhaken (v1) keine Aufnahme.
Sicht: der liegende Kanister ist in 2090 Cut 0 ein Kasten von 35 x 11 Bildpunkten (248..283, 85..96), hellster Wert (53,53,53)
(debug.log `[gator-re2] Kanister Stufe 2 ...`), die Aufnahme selbst hat wie in RE2 keinen Schnitt (B-Dossier: T2 0x801017c4-0x80101d40).

## 4. RE-Gate (Stichproben selbst disassembliert/gelesen; Werkzeuge .claude/skills/re15-psx-disasm/scripts/re15_disasm.py / re2_disasm.py)
| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| ROOM11B0 sub06 @0x01650-0x01663 (Hinknien/Aufstehen) | RDT-Datei-Offsets: 01650 `3f 01 0b 00`, 01654 `09 0a 3c 00`, 01658 `3f 01 0b 00`, 0165C `43 00 80 00`, 01660 `09 0a 1e 00`; davor @0x01642 `2e 02 00 00` (Work_set(2,0)), Zensus-Typ 0x40; Datei bytegleich zu info/Re1.5 | stimmt |
| Loco c2 = PL00-Klettern @0x80038134 (RE1.5 PSX.EXE) | `80038134: ori v0,zero,0x2` + `8003813c: sb v0,-13592(at)` = 0x800acae8 = Spieler+0x94 (Clip) | stimmt (Clip 2 des Spielers) |
| RUN-Gait Clip 0 @0x8005157c | `8005157c: sb zero,148(v0)` (g_entity+0x94 = 0), davor `sb v0,6(v1)` | stimmt |
| Nachdrehen 0xC8/Bild @0x80033f90 | `80033f90: ori a1,zero,0xc8` -> `jal 0x8001a8f8` | stimmt |
| Rueckwaerts-Bild fc-1-f @0x8001f34c-54 | `8001f34c: subu v0,t1,v0` / `8001f354: addiu v0,v0,-1` (Zweig t2 != 0) | stimmt |
| RE2 Wsleeping @0x80053ac0-b18, FUN_8002fe38 @0x8002fe80/88 | s. 3.2 (`andi 0x20` @0x80053acc, `andi 0x80` @0x80053ae4, `jr ra / addiu v0,zero,2` @0x80053b14/18; `addiu t2,zero,128` / `sb t2,-30916(at)`) | stimmt - aber dasselbe Warten @0x079A fehlt im Port (Mangel N1) |
| RE2 Add_speed @0x80055abc-e0 | `lw a1,340(a0)` ... `lw v0,56(a1)`/`addu`/`sw v0,56(a1)`, +60/+64 ebenso | stimmt (reine Weltaddition) |
| ROOM1021 sub02 @0x020C2-CA, @0x020CE-F2 | `2e 02 00 00`, `3f 02 06 00`, `09 0a 6e 00` (110), `3f 02 07 00`, `09 0a 1e 00` (30), ..., `3f 02 06 00`, `43 00 80 00`, `09 0a 14 00` (20) | stimmt; Hinweis: @0x020BE steht `09 0a 28 00` (Sleep 40) direkt vor dem Work_set - gehoert zum Plc_dest-Gang @0x020B2 (`40 00 04 22 ..`), den der Port nicht hat; die Dossier-Aussage "kein Warten davor" ist insofern ungenau, die Form ab @0x020C2 stimmt |
| ROOM2090 Aot_set sce 8 @0x009D6 p1 `ac f9` | `2c 02 08 47 00 00 3c dd 88 96 e4 3e c4 22 ac f9 ..` (-1620); @0x009EA/@0x009FE ebenso | stimmt |
| ROOM2090 RID Cut 12 @0x60+12*32 | @0x1E0 `01 00 3c 68 bf f9 ff ff ad f7 ...` = flag 1, pos (-1601,-2131,-11018) -> (-5184,-3926,-13767) | stimmt |
| ROOM2090 Tuer Slot 0 @0x0996 / ROOM2080 Slot 1 @0x0BC2 | rect (-9400,-26000,1000,1000) -> 2080 (-9950,-14400,-25700) Cut 4 Etage 8; rect (-9650,-26150,1000,1000) -> 2090 (-8200,0,-25550) Cut 0; +22/+23 `01 09` | stimmt |
| GB_WATER_Y -1200 @0x8010c65c (STAGE2.BIN) | `8010c65c: addiu v0,zero,-1200` / `8010c660: sw v0,56(v1)` | stimmt |
| RE2 ROOM7020 sub12 @0x0764-0x07D4 | s. 3.2 | stimmt |
| RE2 ROOM7020 sub12 @0x0726-0x0750 (Wurfrichtung) | SCD 0x0726 `29 07` (Cut_chg 7), 0x0740 `32 00 a0 92 b4 e2 14 ba` (Pos_set), 0x0748 `33 00 00 00 74 0e 00 00` (Dir_set 0x0e74), 0x0750 `3f 0a 12 40` | RE2 dreht Ada NACH dem Schnitt; der Port dreht im Griffbild in Cut 5 (Mangel N2) |
- Suche im Code-Diff 9f10bdb0..HEAD -- re15_port nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/geschaetzt/ungefaehr/
  placeholder/approx: **0 Treffer**. 15 x PORT-WAHL/NUTZER-VORGABE/MESSHAKEN gekennzeichnet; Messhaken RE15_K2080_GATOR_HP /
  RE15_K2080_WURF_GIER env-gegatet nur PC.
- Abnahme-0-Maengel M5 (Dauern ohne Beleg): Sleep 10 -> Ausricht-Poll (gemessen 1-4 Bilder je Ziel, s_leon F148 -> F151); Sleep 30 nach dem
  Tuer-Ton und im Rueckweg entfernt; Adas Clip @0x0750 als PORT-WAHL benannt (Kopf + Z. 271-274) - **erledigt**.
- Konstante ohne eigene Kennzeichnung (klein): Kopfkommentar der Lagen-Defines (Header Z. 122-124) sagt "Stand am Kanister = 700 vor der
  Markenmitte", der Define RE15_K2080_NPC_STAND_X = -7245 ist 500 davor (Z. 127 sagt 500) - Kommentar veraltet (Hinweis).
- Erklaert der Fix den Befund? M1: ja - der Kletterclip Loco c2 ist durch den Original-Clip Loco c11 ersetzt, das Klettern ist im Bild weg
  (3.1). Aber die spaetere Wurf-Gier (d0c4f479) fuehrt im Griff-Bild einen neuen sichtbaren Fehler ein (Mangel N2), den die Nachher-Bilder
  des Bau-Agenten (aus dem Bau davor) nicht zeigen.

## 5. Vertrag
- Bank-9-Bits 114/115/116 (Zuteilung G 114-119), je eine k_port[]-Zeile in test_r36_kartenbits.c (+1 include), unit_r36_kartenbits gruen;
  Bit-114-Text berichtigt ("Knall (Knall-Bank Satz 0)", M4 erledigt). Zone-7-Tot-Bits 236..243 wie in Abnahme 0 (frei, Zensus dort).
- Signale Bank 5 Bits 0x17..0x1E (Raum-Scratch): andere Bank-5-Schreiber im Baum (`grep game_flag_(set|get)(5,`): Plc_dest-Ankunftsbits
  (actor_locomotion.c/enemy_ai_common.c, `walk_flag_bit` = vom SCD gesetzt, G nutzt 0x17 fuer den eigenen Plc_dest), Birkin z5:0x1C
  (enemy_ai_common.c:12216, nur STAGE3/5), elzazombies_r39/irons_tod_1150 (STAGE1-Raeume), panel_zeiger_common - keiner in 2080/2090.
- Nachrichten-Id 42 in 2080/2081 (Zuteilung 42-47); AOT-Slot 21 fuer die Szenen-Tueren.
- Pfade (`git diff 9f10bdb0..HEAD --name-only`): nur G-eigene Dateien + Haken; keine release/, platform/android/, shared_assets/PSX/,
  pri/, synchro/, keine Test-CMakeLists (probes/r41_kanister2080.cmake = Spur-Registrierung). AUFTRAG.md/VERTRAG.md-Aenderungen = vom
  Orchestrator gemergte Dokumente. Gemeinsame Dateien: scd_room_setup.c 1 Zeile, scd_vm.c 1 Zeile, main.c 2 Zeilen, je "Runde 41 Spur G".
  FG-E3 ohne Aenderung in Spur B's Dateien (Aktor-Kopie in G's Modul): re15_actor_t hat keine Zeigerfelder (Suche nach Zeiger-
  Deklarationen in include/re15_actor.h Z. 46-1143: keine), die Struktur-Kopie ist also raumwechselfest.
- Tests je Punkt: unit lage/spawn/szene/start/bahn/texte/ebene/landung + integration kanister2080 / kanister2080_kampf (ganze Folge ohne
  Testhaken fuer den Kanister, Leon + Elza). Parallel-fest (eigener Laufordner, exe-Kopie re15_pc_r41gk_<id>), Linux-fest (m, cmake_minimum_required,
  keine .exe auf Linux). LUECKE: kein Riegel prueft das Griff-Bild (Gier-Sprung, N2) oder die Standzeit der Zeile 42 (N1).
- Nebenwirkungen: integration_r41_re2gator, r35_granate (Gator 2090), r38_wasser (2090), r41_wasser20b0_npc (Landung 2090/2091),
  r37_itemband (weg2090), r36_tueren (S164 2090) gruen; P1/P2 unveraendert (2.1-2.3); Elza-Varianten 2081/2091 gemessen.

## 6. Urteile (gegen den Wortlaut, AUFTRAG.md inkl. Nachtraege 5/17/20-24b und Entscheide FG-E1/FG-E3/FG-E4)
| Punkt | Urteil | Beleg |
|---|---|---|
| P1 Knall (N17), dann 8 Zombies oben im Hauptbereich (N20) | **erfuellt** | 2.1/2.2: echter Leiterweg DOOR16, Knall-Bank Satz 0 vor 8 x Sce_em_set y -14400, Mitschnitt RE2_1060 r 0,891 im ersten 2080/2081-Bild; Ebene unveraendert seit Abnahme 0 (66-s-Messung dort) |
| P2 Gaskanister an der Reling (Gas_Can.bmp) | **erfuellt** | 2.3: Cut 5, Kanister stehend auf der roten Marke |
| P3 "oft genug angeschossen" -> Szene (N22: RE2-Schwelle 300) | **erfuellt** | v1: ohne Haken nach 37 echten Treffern F7375; Schwelle @0x80101ebc-dc (Abnahme 0) |
| P3 Tuer-Ton, Marvin/John im Raum, 8 Kopfschuesse (Kopf platzt, tot) | **erfuellt** | 3.1: `Tuer-Ton DOOR06 Satz 1` F8, 8 x `hp=-1 state=3 zeile=5` + `Kopf ab ... 8 von 8`, Leon + Elza (auch v1 ohne Haken) |
| P3 / N24b "rennt hin, nimmt ihn auf in beide Haende" | **teilweise** | Klettern weg (Loco c11 = Marvins Hinknien ROOM11B0 @0x01650, 3.1), aber neuer sichtbarer Fehler im Griffbild: Gier 93 -> 3072 (98 Grad) in EINEM Bild, F464 von hinten / F466 Profil, Leon + Elza (<SP>/n2_griffbild_gier_sprung_x3.png); die Nachher-Bilder des Bau-Agenten stammen aus dem Bau vor d0c4f479 |
| P3 "sagt ... 'Hey, Leon, try with this!'" | **teilweise** | Zeile steht, aber nur 52 statt 99 Bilder: der Raumwechsel 1 Bild nach dem Loslassen schneidet sie ab (Leon + Elza, ctest-Laeufe und k_leon); RE2 wartet mit Wsleep/Wsleeping @0x079A/@0x079B (3.2) |
| P3 Wurf die Reling runter, liegt in 2090 an der Stelle im Wasser (FG-E3: Aufschlagstelle) | **erfuellt** | 3.3: Bahn RE2 sub13, Aufschlag (-7515,-2076,-12304) F43 mit Ton, `Kanister im Becken ... = Aufschlagstelle`; Elza (-7506,-12276) |
| N5 Kamera wie Adas Wurf (Entscheid FG-E4: 2090 Cut 12 + RE2-Schnittfolge, Nutzer-Hinweis) | **erfuellt per FG-E4** (mit N1) | Fall in Cut 12 F6-F58, sichtbar F30-F43, Rueckschnitt F60 (<SP>/k_leon_fall_cut12.png); Werfer nicht im Bild - so entschieden und im Dossier 8.8 dem Nutzer genannt; die RE2-Folge ist bis auf @0x079A uebernommen (N1) |
| FG-E3 derselbe Alligator, kein neues Auftauchen/Intro | **erfuellt** | 3.3: Slot 35, Lage/Gier/HP/Phase gleich vor und nach der Szene; nach der Szene keine AUFTAUCHEN/INTRO-Zeile (alle Verlaeufe) |
| P3 / N23 "Wenn der Aligator dran vorbei kommt, ... aufnehmen" / "nimmt irgendwann im weiteren Rundlauf auf" | **teilweise** | 3.4: Aufnahme in 2 von 4 unabhaengigen Verlaeufen (F104 / F3835); im einzigen Lauf ohne jeden Testhaken (v1) keine Aufnahme in 5 min, bei Start West (v3/v5) keine in 2,2 min |
| P3 Schuss auf den Kanister im Maul -> Sterbeanimation mit geplatztem Kiefer (Spur B) | **erfuellt, wo aufgenommen** | 3.3: `R1 VSync 4 F570`, Schnitt Cut 5, Explosion am Kopf, `R1 ende (Leiche)` (<SP>/k_leon_r1.png) |
| RE-Gate | haelt bis auf N1 (RE2-Warten @0x079A ausgelassen, obwohl die Folge als RE2-Form zitiert ist) und die Beleg-Luecke der M1-Bilder (N2) | 4 |
| Vertrag | haelt | 5 |
| Tests | 39 Unit + 8 Integration gruen; Luecke: kein Riegel fuer Griffbild-Gier, Standzeit der Zeile 42 und Aufnahme bei anderer Kampflage | 1 / 5 |

**bestanden = false.**

## 7. Maengel (nummeriert, nachpruefbar)

**N1 (P3 Zeile 42 / N5 RE2-Schnittfolge) - die Zeile wird vom Raumwechsel abgeschnitten.** state.log: `msg(a=1 fsm=1 id=42` F530..F581 =
52 Bilder, in 2090 ab F1 `a=0` (Leon + Elza, ctest lauf_0d602269, k_leon); im Bau von Abnahme 0 stand dieselbe Zeile 99 Bilder
(g0/runs/p3r_leon F569..F667) und endete von selbst. Ursache: `baue_szene` schreibt nach dem Signal WURF nur `Sleep 1` + Tuer nach 2090
(kanister2080_r41.c Z. 279-287); RE2 ROOM7020 sub12 wartet nach Super_reset/Evt_exec mit **Wsleep `0b` @0x079A + Wsleeping `0c`
@0x079B** (Tafel @0x800a74c8 [11]/[12] -> 0x80053a8c / 0x80053ac0; Warten solange 0x800e873c & 0x80) auf das Ende von Adas Zeile, bevor
es den Aufschlag pollt (@0x079C-07A4). Der Bau-Agent hat dasselbe Warten @0x081A belegt, @0x079A aber nicht uebernommen. Mit der
geplanten Sprachdatei synchro/STAGE2/room2080/main42.wav wuerde auch die Stimme nach 1,7 s abbrechen. Forderung: RE2-Form @0x079A
umsetzen (Zeile laeuft zu Ende, z. B. Warten auf das Zeilenende vor dem Raumwechsel oder die Zeile ueber den Wechsel stehen lassen - Wahl
mit Beleg), Messung Leon + Elza (Zeile in voller Dauer, Fall weiter in Cut 12), Riegel auf die Standzeit.

**N2 (P3 / Nachtrag 24b) - Gier-Sprung um 98 Grad im Griffbild.** state.log Slot 9: F464 `mo=11 af=24 ... r93`, F465 `r3072` (Leon und
Elza gleich); Bild <SP>/n2_griffbild_gier_sprung_x3.png: F464 kniet Marvin/John mit dem Ruecken zur Kamera vor dem stehenden Kanister, F466
kniet er im Profil, der Kanister zeigt auf die Kamera - mitten in Cut 5, genau in dem Moment, den der Nutzer in Nachtrag 24b beanstandet
hat. Ursache: kanister2080_r41.c Z. 706-709 setzt ab dem Greifen jedes Bild `n->rot_y = wurf_gier()` (seit d0c4f479; vorher 0 = 93 Einheiten
neben dem Laufblick, kaum sichtbar). RE2 setzt Adas Wurfrichtung mit Dir_set `33 00 00 00 74 0e 00 00` @0x0748 erst NACH dem Schnitt
Cut_chg 7 @0x0726 (unsichtbar). Dazu springt der Kanister im selben Bild von stehend auf quer (vom Bau-Agenten als OFFEN 5 benannt).
Die Nachher-Bilder `G_belege/nb1_m1_nachher_marvin_f437-530.png` / `..._john_...` (Commit 16bacda6, 02:04 +0200) zeigen den Stand VOR
d0c4f479 (03:14 +0100) und belegen den gebauten Endstand nicht. Forderung: Wurfrichtung belegt setzen (RE2: mit dem Schnitt auf Cut 7),
Bildbogen F437-F530 des END-Baus fuer Leon + Elza neu, Riegel (Gier im Griffbild = Gier davor).

**N3 (P3 / Nachtrag 23 / FG-E3) - Aufnahme im weiteren Kampf nicht verlaesslich.** 3.4: v1 (natuerliche Schwelle, kein G-Haken) - nach
der Szene 9000 Bilder ohne Aufnahme, Gator nie naeher als 13098 am Kanister; v3_west_elza / v5_west_leon - 4000 Bilder ohne Aufnahme;
v2/v4 erst nach 3835 Bildern; nur die Startlage des Riegels kampf liefert F103/F104. Ursache: feste Landestelle (Wurf-Gier 3072 ->
Nordwestbecken (-7515,-12304)) unabhaengig von der Kampflage beim Ausloesen; der Alligator verfolgt den Spieler und lenkt nicht zum
Kanister (Bau-Agent 8.7.1, Frage 8.8.1). FG-E3 verlangt "der Kanister landet so, dass der Alligator ihn im normalen Rundlauf aufnimmt",
N23 "wird im normalen Kampf tatsaechlich aufgenommen". Forderung: Entscheid Orchestrator/Nutzer zu Frage 8.8.1 (Landestelle nach der
Kampflage / Lenkung / anderes), Umsetzung mit Beleg, Messung mindestens der Verlaeufe v1, v3/v5, v2/v4 und kampf (Leon + Elza) mit Bild
der Aufnahme und Zeit seit dem Wurf, Riegel mit einer zweiten Startlage.

## 8. Hinweise (kein Mangel dieser Spur bzw. klein)
- H1 Spinnen 0x25 noch in 2090 (Spur F): Giftkugeln, Zielhilfe; in Cut 0 greift eine Spinne Leon waehrend der Aufnahme an (<SP>/k_leon_aufnahme_cut0.png).
- H2 Spur B: R1-Leiche ausserhalb des Beckens (Elza `R1 ende (Leiche) pos=(10684,-8412)`, ctest kampf_elza) - wie vom Bau-Agenten gemeldet.
- H3 Die Aufnahme liegt in Cut 0 am rechten Bildrand, der liegende Kanister ist 35 x 11 Bildpunkte bei Helligkeit 53 - im Spiel kaum zu sehen
  (RE2-Form ohne Schnitt, Spur B). Der Bau-Agent zeigt sie nur mit Aufhellung x6.
- H4 Raumwechsel 2080 Cut 7 -> 2090 Cut 12: F2/F4 schwarz (Mittel 0,0/0,1), Cut 12 ab F6; F1-F5 pm=0 (Riegel erst nach Sleep 1 des
  Rueckweg-Programms). Waehrend des Falls schiebt der Alligator den gehaltenen Spieler (F1 (-5790,-6245) -> F33 (-4479,-5852), gr=0; OFFEN 3).
- H5 Header Z. 122-124 "Stand am Kanister = 700 vor der Markenmitte" widerspricht RE15_K2080_NPC_STAND_X (500, Z. 127).
- H6 Dossier 8.3 "ROOM1021 ... kein Warten davor": @0x020BE steht Sleep 40 (`09 0a 28 00`) direkt vor dem Work_set; er gehoert zum Gang
  Plc_dest @0x020B2, den der Port nicht hat - Aussage praezisieren.
- H7 Dossier 6.2/6.3 beschreiben noch den Stand vor FG-E3/FG-E4 (Cut 7, untergetauchter neuer Alligator); massgeblich ist Abschnitt 8.
- H8 Aufraeumen: Messkopien re15_pc_g41abn1*.exe geloescht; Laufordner/Bilder nur im scratchpad g1; im Baum nur dieser Bericht.
