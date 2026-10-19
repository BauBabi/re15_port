# Runde 39 Spur R "hund1121" — Abnahme 0 (unabhaengig)

Datum: 2026-10-19. Baum: .claude/worktrees/r39_hund1121, Zweig r39/hund1121, HEAD aead0d9b, Basis 02dc7917.
Pruefer: Abnahme-Instanz 0 (glaubt nichts, was nicht selbst am gebauten Stand gemessen ist).

**Ergebnis: BESTANDEN.** Der Nutzer-Punkt ist erfuellt (echter Tuerweg 1061 -> 1121 als Elza, auch als Neuspiel
ueber RE15_PSELECT_AUTO; Hund im Fenster, Durchbruch mit Splittern und zwei Glasknallen, Landung, Angriff; einmalig; ohne
Panik-Bit kein Hund; Leon 1120 unveraendert). RE-Gate und Vertrag halten, 83/83 gezielte Tests gruen. Hinweise H1-H4
(u. a. sehr hoher Sprungbogen ab der Bruestung) und Anmerkungen A1-A4 blockieren nicht.

Nutzer-Punkt (AUFTRAG.md, Nachtrag 10, woertlich):
> "Und dann möchte ich, das in ROOM 1121, dort wo bei Leon die Krähe durch das Fenster fliegt, beim Durchlauf ein Hunde
> durch das Fenster rein springt."

## 0. Stand der Pruefung (fortlaufend)
- [x] AUFTRAG/VERTRAG/Dossier gelesen, Code-Diff gelesen (master...HEAD, 18 Dateien)
- [x] Bau: local_build.sh configure + build -> LOCAL-BUILD-OK, "ninja: no work to do"; exe 01:32:40 juenger als die
      letzte Quellaenderung (hund1121_r39.c 01:32:22), Baum sauber -> gebauter Stand = HEAD
- [x] gezielte Tests (83/83 gruen, s. 3)
- [x] eigene Messung an der exe (A1, E1, P1, B1, R15, K1; s. 2)
- [x] RE-Gate Stichproben (s. 4)
- [x] Vertrag / Nebenwirkungen (s. 5)

## 1. Diff-Umfang
`git diff master...HEAD --name-only` (der Zwei-Punkt-Diff gegen master zeigt zusaetzlich die nach der Basis in master
gemergte Spur K als "geloescht" — das ist kein Eigenanteil der Spur):
analysis/befunde_runde39/R_* (Dossier, Belege), engine/src/hund1121_r39.c (neu), include/re15_hund1121_r39.h (neu),
tests/unit/test_r39_hund1121.c (neu), tests/unit/probes/r39_hund1121.cmake (neu, Sonden-GLOB), tests/integration/
test_r39_hund1121.cmake (neu), Haken: enemy_ai_re2_dog.c (Startmaschine), game_step_common.c (+2), scd_room_setup.c (+2),
scd_vm.c (+2), platform/pc/main.c (+3).

## 2. Eigene Messung an der exe (re15_port/build/platform/pc/re15_pc.exe, Stand HEAD)
Messkopien `re15_pc_abn0r_<zufall>.exe` im exe-Verzeichnis (Asset-Wurzel), eigene Arbeitsordner im Scratchpad, keine
geerbten RE15_*-Variablen (`env | grep -c ^RE15_` = 0). Spielstand je Lauf mit dem Werkzeug der Spur
(`test_r39_hund1121 karte re15_card.mcr <raum> <x> <z> <gier> [bank:bit]`, Figur = Raumvariante -> Elza bei xxx1).

### 2.1 Lauf A1 — Elza, ROOM1121, (9,97)=1, CONTINUE, derselbe Weg wie Lauf A, verlaengert bis F420
Befehl: karte `1121 150 9550 0 9:97`; RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=70 RE15_INPUT_SCRIPT=XU1.5,R0.35,XU2.5 RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=130-330/2:bild_
RE15_AUDIO_CAP_SYNC=cap.raw RE15_STATE_LOG=state.log RE15_HUND1121_LOG=hund.log RE15_SPAWN_DIAG=1 RE15_EXIT_AT=420#1121.
debug.log: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[save] CONTINUE: resumed in room 1121`,
`[spawn-diag] Sce_em_set type=0x20 behavior=0x00 slot=4 pos=(5400,-1827,12100) dir=1048`, Modell
`[elzaems] Typ 0x20 aus EMD/CDEMD1.EMS Sektor 678`.
hund.log (bildgleich mit dem Dossier N3): F143 Ausloeser (6020,6207), F144 Hund gesetzt, F145 Sub 14 Phase 0, F146
Freigabe, F156 Glas (13 Splitter), F158 Wand zu, F159 Knall 1, F164 Knall 2, F188 fertig T44 Zustand 1 Sub 2.
state.log: Kamera Cut 1 bis F163, Cut 2 ab F164; Hund F146-F152 Anlauf (Clip 20, z 12036 -> 11652), F153 Absprung,
Flug bis F174, Landung F175 bei (5134,5450), F187 Sub 2, danach Angriff: Elza hp 100 -> 80 (F210) -> 75 (F260)
-> 70 -> ... -> 25 (F390) (Elza steht nach dem Skript still); vier Kraehen bleiben (t=21 x 4).
Ton (eigene Auswertung cap.raw, Spitze je Spielbild, Ende = F420, Raum-Median 2559): F155-F159 6077..7249,
F160 26325, F165-F171 25566..29813 (Spitze F167), danach Abklingen -> zwei Glasknalle deutlich ueber dem Grund.
Bilder (scratchpad abn0/A1/sheet_A1.png, zoom2_A1.png): F144-F154 steht der Hund (Dobermann) in der rechten Scheibe
vor der Jalousie, F156 bricht er mit den RE2-Splittern (cyan) durch, F158-F162 fliegt er ueber dem Gang (auf Hoehe der
Deckenleuchte, s. Hinweis H1), ab F164 Cut 2; F190/F210 greift er Elza im Buero-Bild an.

### 2.2 Lauf E1 — ECHTER Tuerweg ROOM1061 (2F) -> ROOM1121 per Aktionstaste, dann derselbe Weg durchs Band
Befehl: karte `1061 27100 25400 0 9:97`; CONTINUE; RE15_DEBUG_JUMP=1061@60 RE15_PLAYER_POS=27100,25400,0,8 (Band 8 =
2F-Ebene, y -14400, Tuer-Slot 0 floor 8 @0x00D12); RE15_PAD_AT=110:A,120:A,130:A,140:A (Viereck = Aktion);
RE15_INPUT_SCRIPT_START=200 (der Bildzaehler beginnt je Raum neu, F200 wird erst in 1121 erreicht); Ton an,
RE15_SE_DEBUG=1, Bilder F260-F300.
debug.log: `[aot] DOOR FIRE slot=0 rect=(27100,25400,hw=500,hh=1100) target_cut=0 spawn=(150,0,9550)`,
`[tuer] Sequenz Archiv 2 DOOR07 ... (9 Skripte)`, `Sequenz fertig: 301 Bilder`, `PC loaded room1121.rdt`.
hund.log: F110 scharf (Installer am Tuer-Weg scd_room_setup.c), F271 Ausloeser (6038,6366), F272 Hund gesetzt, F274
Freigabe, F284 Glas 13 Splitter, F286 Wand zu, F287/F292 Knall 1/2, F316 fertig Zustand 1 Sub 2.
SE (debug.log): `[re2se] ENEMSE Bank 6 geladen`, se=8 (Anlauf-Clip), se=3 Bank 6 (Absprung, Bild zwischen den Dumps
280/282 = Spawn+9), zweimal `[se] Stimme: se=33` (Glas, Satz 0x21), se=10 Bank 6 (Landung, nach F300 = Spawn+31).
Bilder (abn0/E1/sheet_E1.png): F282 Hund in der rechten Scheibe, F284-F292 Durchbruch + Splitter + Flug, ab F294 Cut 2.

### 2.3 Geometrie "beim Durchlauf" (selbst aus ROOM1121.RDT gelesen)
SCA @0x2FC, Region 0 Band 0: Mittelblock i1 x -10500..3500 / z 1650..6400, Ostwand i3 x 6650..8650 / z -7750..13200,
Rueckwand i2 z 11200..13200. Tueren main00: Slot 0 (-1200,8550) -> ROOM1061 (Ankunft (150,9550)), Slot 1 (300,5900)
-> ROOM1081 (nur bei (4,243)), Slot 2 (-8950,-3900) -> Raum 0x13 = ROOM1131. Zwischen Nordteil (Tueren 0/1) und Suedteil
(Tuer 2) bleibt nur der Ostgang x 3500..6650; das Band x 3500..6650 / z 4300..6400 deckt ihn ganz ab. Der Weg
1061/1081 -> 1121 -> 1131 kreuzt das Band also zwingend (in beiden Richtungen).

### 2.4 Lauf P1 — NEUES SPIEL als Elza (VERTRAG: RE15_PSELECT_AUTO), echter Tuerweg
RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG_AT=9:97@40
RE15_DEBUG_JUMP=1061@gp RE15_PLAYER_POS=27100,25400,0,8 RE15_PAD_AT=110:A,120:A,130:A,140:A, Weg wie E1.
debug.log: `Elza-Bit=1`, Intro ROOM1031, `JUMP -> 106 UPPER STAIRS (ROOM1061) spawn=(27100,-14400,25400)`,
`DOOR FIRE slot=0 ... spawn=(150,0,9550)`, `DOOR07`, `PC loaded room1121.rdt`. hund.log bildgleich mit E1
(F271 Ausloeser, F284 Glas, F287/F292 Knall, F316 fertig Zustand 1 Sub 2).

### 2.5 Gegenlauf B1 — Elza, ROOM1121, OHNE (9,97), derselbe Weg
hund.log: `F0 [hund1121] ROOM1121 aus: (9,97)=0 (7,0xAE)=0`. fenster.log unveraendert Runde 36 L:
`[fenster] ROOM1121 (Elza): Fenster kaputt, Kraehe Slot 3 Kill-Flag 0xAC Programm 1`. spawn-diag nur die vier Kraehen
(Typ 0x21, Slots 0..3), state.log 0 Zeilen mit Typ 0x20. Ton: Raum-Median 857, keine Knall-Spitze (F160 719,
F167 1300; groesste Spitze im Fenster 5742 = Grundgeraeusch). -> ohne Panikphase kein Hund, Elzas Raum wie vorher.

### 2.6 Lauf R15 — reiner RE1.5-KI-Modus (RE15_AI_FLAVOR=1.5), sonst wie A1
spawn-diag `type=0x20 behavior=0x40`; hund.log: F144 Ausloeser (Verhalten 0x40), F146 `RE1.5: Zustand 4 Sub 0
wartet auf grid 0x43`, F147 Freigabe (RE2=0), F163 Glas bei y -1907 (13 Splitter), F166/F171 Knall 1/2, F177 fertig
T32 Zustand 1 Sub 2. state.log: g=43 waehrend des Sprungs, danach Zustand 1.

### 2.7 Leon ROOM1120 / Wiedereintritt / schon gesprungen
Gemessen ueber die Laeufe B-E des Integrationshakens (selbst gefahren in meiner ctest-Reihe, s. 3):
D = Leon ROOM1120 mit (9,97)+(9,73): Leons Kraehen-Ereignis (`ROOM1120 scharf`, `Ausloeser`), kein Hund, leeres
hund.log; C = (9,97)+(7,0xAE): `aus`, kein Hund. integration_r35_fenster (Leons Ereignis Runde 35) gruen.

## 3. Tests (selbst gefahren, gezielt; KEINE volle Suite im Spurbaum)
`ctest --test-dir re15_port/build -R "r39_hund1121|dog|hund|r35_fenster|r36_elza_fenster|1190|crow|kraehe|aot_edge|
scd_aot_opcodes|aot_sce_census|r37_elzaems|r33_tuer1120|r36_kartenbits|re2doc" --timeout 600 --output-on-failure`
(Regex des Bau-Agenten + 1120-Tuer-Riegel, Kartenbits, re2doc; seriell):
```
77/83 unit_r39_hund1121_ereignis ... 82/83 unit_r39_hund1121_startmaschine   Passed
83/83 Test #980: integration_r39_hund1121 .......... Passed 106.02 sec
43/83 Test #509: integration_r35_fenster ........... Passed  25.67 sec
46/83 Test #583: integration_r36_elza_fenster ...... Passed  47.24 sec
76/83 Test #972: integration_r39_gegner_hunde1190 .. Passed  42.56 sec
100% tests passed, 0 tests failed out of 83     Total Test time (real) = 795.73 sec
```
Kein roter Test, kein Nachfahren noetig. Riegel je Punkt vorhanden und messend: 6 Unit-Teile (Ereignis/Tor/Programm-
Bytes, Sprung gegen unabhaengige Bogenrechnung + Glas/Knall/Splitter/Wand/Schatten/Landung, RE1.5-Form, Einmaligkeit,
Treffer auf der Bruestung, Startmaschine) + Integration A-E an der echten exe (Kette, Ton gegen B, Bild gegen B,
Gegenlaeufe, Tuer-Weg per RE15_FIRE_AOT). Den Tuer-Weg per Aktionstaste habe ich zusaetzlich selbst gefahren (2.2/2.4).
Linux-Festigkeit: cmake_minimum_required, file(COPY_FILE) wie 83 andere Integrationshaken, exe-Endung ueber
`get_filename_component(... EXT)`, keine Backslash-Pfade, `m` verlinkt, Umgebung gesaeubert (`-E environment`).

## 4. RE-Gate (Stichproben selbst disassembliert / Bytes selbst gelesen)
| # | Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|---|
| 1 | RE2 EMD0G_MOD0.BIN Startmaschine 0x80102C78: Phase 0 @0x80102CD4-F4 ohne +7-Schreiber, Phase 1 faellt @0x80102D20 in den Rumpf @0x80102D24, Absprung 280 + SE 3 @0x80102D68/74 | `re2_disasm.py dis 0x80102C28 96 --bin EMD0G_MOD0.BIN`: Dispatch `lbu v1,7(s0)` @0x80102C94, 0->CD4, 1->CF8, 2->D24, sonst `j 0x80102d7c` (Ruecksprung). CD4-F4: `lbu 467/448`, `sb zero,537`, `sb zero,557`, `ori 0x80/0x2`, `j 0x80102d7c` — kein `sb ..,7`. CF8-D0C: a1=65, a2=-320, `jal 0x80101380`, Delay `sb v0,7(s0)` (v0=2); D10-1C `andi 0xfd` +0x1C0; D20 `addu a0,s0,zero`, D24 `jal 0x800152c8` = Rumpf. D5C-78: `sh 1,6(a1)`, `addiu v0,zero,280`/`sh 324`, `jal 0x8005bd6c` a0=3, Delay `sb 1,537` | stimmt; die Port-Aenderung `default:` -> `case 2:` trifft auch den Fall +7 >= 3 (Original: nichts) |
| 2 | RE2 ROOM21A0 sub07 @0x020F6..@0x02122 (Sca_id_set, Member_set +4/+5/+6/+7 = 1/14/0/0, Evt_next, +7 = 1) | Bytes ROOM21A0.RDT: `37 11 00 00` / `2e 03 00` / `34 02 01 00` / `34 03 0e 00` / `34 04 00 00` / `34 05 00 00` / `32 ..` / `33 ..` / `02` / `2e 03 00` / `34 05 01 00`; Blockgrenze sub07 = 0x020F6..0x0213C (rdt_blocks). Member_set-Tabelle RE2 PSX.EXE 0x80011228 = 0x80055CD4,+8,...: Index 2..5 -> `sb a2,4/5/6/7(a0)`, `sb a2,7(a0)` @0x80055D00 | stimmt |
| 3 | RE1.5 STAGE1.BIN Hunde-INIT @0x8010DB88-98 (grid 0x40 -> Zustand 4), Tor @0x801113E4-EC (grid 0x43) | `re15_disasm.py dis`: @0x8010DB88 `lbu v1,9(a0)`, `ori v0,zero,0x40`, `bne`, Delay `ori v0,zero,0x4`, `sw v0,4(a0)`; @0x801113E4 `lbu v1,9(a0)`, `ori v0,zero,0x43`, `bne v1,v0,0x80111648` | stimmt |
| 4 | RE2 EMD0G_MOD0 0x80101380 (+0x158 = 8 @0x80101398), Flug +40 @0x801014B8-C8, SE 10 @0x801015FC, Sub 2 @0x80102E2C; Tabellen @0x80105458/@0x80105464/@0x80105514 | @0x80101394/98 `addiu v0,zero,8`/`sh v0,344(a0)`; @0x801014B8-C8 `lhu 326`/`addiu 40`/`sh 326`; @0x801015D0 `addiu a0,zero,10` .. @0x801015FC `jal 0x8005bd6c`; @0x80102E24-2C `addiu v0,zero,2`/`sb v0,5(s0)`; Spawn-Tabelle [0,1,2,4,10,12,14,13,15], ACTIVE[14]=0x80102C28, [15]=0x80102E58, Phasen {0x80102C78,0x80102D98,0x80102E0C} | stimmt |
| 5 | Byte-Offsets RE1.5-RDT: ROOM1121 SCA @0x2FC + Index-15-Zellen @0x003C8/@0x0059C/@0x00770/@0x00944/@0x00B18; ROOM1130 @0x00AC8; ROOM1190 @0x027E6; ROOM1120 main00 @0x00D0C, sub01 @0x00D6C | ROOM1121.RDT == ROOM1120.RDT (86900 B); SCA-Kopf `50 da 0e cb` + 5 x `27 00 00 00`; alle fuenf Zellen `56 2c d0 07 74 f5 c0 2b 01 ff 00 13` (w 11350, d 2000, x -2700, z 11200, floor 0x13); 1130 `37 00 0a 00 .. 37 04 0a 00`; 1190 `34 0c 43 00`; 1120 `44 00 21 00 00 00 00 a7 ..`, `22 03 38 01` | stimmt |
| 6 | Knall-Takte 3/8 = RE2 room1090 sub15 Sleep 3 @0x0204, Sleep 5 @0x0214; Rumble @0x003A/40/46 | Walk sub15 (0x029A8..0x02BF4, Status ok): Splitter +0x54..+0x1F4, +0x0204 `09` + `0a 03 00`, +0x0208 `36 02 21 01 ..`, +0x0214 `09` + `0a 05 00`, +0x0218 `36 02 21 01 ..`; Rumble `8a 00 03 00 03 00` / `8a 00 03 00 07 00` / `8b fa 08 00 00 00` | stimmt (Kleinigkeit: der Satz bei +0x0204 beginnt mit `09`, das zitierte `0a 03 00` liegt bei +0x0205) |

Textsuche im Spur-Diff (re15_port) nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/approx/geschaetzt:
0 Treffer. Einziges getenv = Messpfad RE15_HUND1121_LOG (kein Verhalten, kein Env-Schalter als Abschluss).
Jede verhaltensrelevante Konstante traegt @0x/Byte-Offset oder ist als PORT-WAHL gekennzeichnet (Lage x/z/Richtung,
Hoehe -1827, Glasbruch-Augenblick, Wand wieder zu, Schatten, Luft-Treffer). Die Glas-Aufrufe sind zeichengleich mit
Runde 35 (fenster_1120.c:219, :265-267). Aot_reset 0x46 = 10 Byte in der VM (scd_vm.c:251).
Erklaert der Fix den Befund? Ja: N1 (Wandklemme schob den Hund im Spawn-Bild auf z 10682) -> Sca_id_set; die
Startmaschinen-Korrektur bildet die gelesenen Instruktionen nach (Warten bis +7 = 1, Anlauf ab dem Freigabe-Bild).
Sub 13/14/15 hatten vorher keinen Produzenten: re2d_init setzt immer re2d_spawn_tbl[0] (enemy_ai_re2_dog.c:2477),
Sub 15 nur aus Sub 13 und umgekehrt; die anderen Treffer fuer `sub_state_1 = 13/14/15` liegen in enemy_ai_common.c
bei anderen Gegnertypen (Birkin u. a.) -> keine Nebenwirkung auf andere Hunde.
Zusatzbefund RE2-Zensus (selbst, 250 RDTs PL0): Hunde mit Spawnwert 7/8 (Sub 13/15) nur in ROOM21A0 main00; einen
erhoehten Hunde-Start (Fensterbank) kennt RE2 nicht -> die Hoehe -1827 ist zu Recht PORT-WAHL.

## 5. Vertrag / Nebenwirkungen
* Bank 9: nur (9,97) GELESEN (VERTRAG §1.1 "Q und R lesen es"); Spur P setzt `Set(9,97)=1` am Ende von RUF1041 und
  loescht es bei Johns Rettung (r39/johnruf1041 johnruf_r39.c:227, re15_johnruf_r39.h:47) -> dasselbe Bit.
* Nachrichten-Ids: keine. Sprachdateien: keine. Assets: keine neuen.
* AOT-Slot 4 in ROOM1121: frei (main00 Slots 0..3 selbst gelesen; der Elza-Zweig von fenster_1120.c:153-161 belegt
  keinen Slot). Gegner-Slot 4: main00 0..2, Elza-Kraehe 3.
* Zone-7-Bit 0xAE: eigener Zensus 132 RDTs STAGE1-3 (+12 Platzhalter a 4 B): weder Sce_em_set pc[7] noch Ck/Set
  Bank 7; frei in 0xA0..0xAF: a0-a3, ac, ae, af; im Port sonst unbenutzt. Spur Q nutzt 124..133/144/145.
* Ereignis 30: Spur Q (r39/elzazombies) nutzt ebenfalls Nummer 30, aber raumgebunden (1041/1061/1031 bzw. Rueckkehr-
  raum, programm_bauen elzazombies_r39.c:536-576); beide Haken pruefen Raum + Zustand -> keine Ueberschneidung in 1121.
* Pfade: keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists (Registrierung ueber den
  Sonden-GLOB tests/unit/probes/r39_hund1121.cmake). Gemeinsame Dateien nur kleine Haken mit "Runde 39 Spur R":
  scd_vm.c +2, scd_room_setup.c +2 (Installer am Blockende), platform/pc/main.c +3, game_step_common.c +2 (nicht in
  der Liste §1.3, aber 2-Zeilen-Haken neben re15_fenster1120_tick), enemy_ai_re2_dog.c 3 wirksame Zeilen (in §1.3 fuer E
  gelistet; kein Konflikt mit master/r39/integration laut merge-tree).
* Probe-Merge (git merge-tree, ohne Refs): gegen master, r39/integration, r39/elzazombies, r39/johnruf1041 nur
  Textkonflikte "Nachbarzeile" in scd_room_setup.c (bzw. scd_vm.c/main.c) — Include/Installer beider Spuren behalten.
* Leon: ROOM1120 unveraendert (integration_r35_fenster, Lauf D). Elza ohne (9,97): unveraendert (B1). Andere Raeume:
  Haken pruefen Raum 0x1121; Hunde-/Kraehen-/1190-Riegel gruen.

### 5.1 Grenzfall Kamera (Lauf K1)
Weg `XU0.6,R0.35,XU2.5` (frueh nach Sueden abbiegen, dicht an der Nordost-Ecke des Mittelblocks): Cut 1 ist schon bei
x 3750 / z 9550 aktiv (state.log F87 cam=1); Elza bleibt an der Blockkante bei z 6868 haengen und kommt erst ab
x ~3968 (Ecke 3500 + Radius) ins Band. Eine Ausloesung unter Cut 0 (Fenster nicht im Bild) ist damit nicht moeglich.

## 6. Hinweise fuer Orchestrator/Nutzer (kein Mangel, Bewertung "erfuellt" bleibt)
* H1 Sprunghoehe: Der RE2-Bogen (vy -320, +40/Bild, Scheitel y0 - 1440, @0x80101380/@0x801014B8) startet hier auf der
  Bruestung y -1827 (PORT-WAHL; RE2 kennt keinen erhoehten Start, eigener Zensus). Wurzel-Scheitel y -3267, der Kopf
  erreicht im Bild die Hoehe der Deckenleuchte (abn0/A1/zoom2_A1.png F158-F162, abn0/E1/sheet_E1.png F288-F292).
  Wirkt als sehr hoher Satz in den Gang; Landung erst bei z 5450 (6,6 m vor der Wand).
* H2 Elzas Fenster ist seit Runde 36 L schon zerbrochen; der Hund schlaegt trotzdem 13 Splitter und zwei Glasknalle
  heraus (Restscheiben, wie Leons Kraehe). Im Dossier "Fuer den Nutzer" so beschrieben.
* H3 Laeuft Elza weiter, sieht man den Hund 12 Bilder in der Scheibe (F144-F155) und 8 Bilder im Flug (F156-F163);
  die Landung (F175) faellt schon in Cut 2 ausserhalb des Bildes, danach holt er Elza im Buero-Bild ein (F190/F210).
* H4 Integration: Bit 97 kommt erst mit Spur P (hier per Spielstand-Flag gemessen; P setzt/loescht dasselbe Bit);
  Ereignis-Nummer 30 auch bei Q (raumgetrennt); merge-tree-Konflikte nur Nachbarzeilen in scd_room_setup.c (+ scd_vm.c,
  main.c gegen P/Q) — beide Zeilen behalten.

## 7. Anmerkungen (nicht blockierend)
* A1 Zitat "Sleep 3 (@0x0204 `0a 03 00`)": der Satz beginnt bei +0x0204 mit `09`, `0a 03 00` liegt bei +0x0205
  (Aussage stimmt).
* A2 Dossier U1b "er steht 0,6 s in der Scheibe" / OFFEN "0,33 s zwischen Freigabe und Absprung": gemessen Spawn F144 ->
  Absprung F153 = 9 Bilder (0,3 s), Freigabe F146 -> Absprung F153 = 7 Bilder (0,23 s), Spawn -> Glas 12 Bilder (0,4 s).
* A3 Unversionierte Messkopie `re15_port/build/platform/pc/re15_pc_r39r_mess.exe` des Bau-Agenten liegt noch im
  Bauverzeichnis (kein Commit-Inhalt).
* A4 Ist das Sce_em_set des Programms erfolglos (Gegner-Slot 4 belegt), bliebe die Band-1-Rueckwand fuer den Rest des
  Raumbesuchs offen (die Schliess-Regel laeuft erst ab GESPAWNT). Heute nicht erreichbar (Slot 4 in 1121 frei); bei der
  Integration mit weiteren 1121-Spawns im Blick behalten.

## 8. Urteil
| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| "in ROOM 1121, dort wo bei Leon die Kraehe durch das Fenster fliegt, beim Durchlauf ein Hund durch das Fenster rein springt" | **erfuellt** | Echter Tuerweg 1061 -> 1121 (E1, P1 als Neuspiel-Elza) und CONTINUE (A1): Ausloesung auf Leons Band (das der Weg 1061/1081 -> 1131 zwingend kreuzt, SCA selbst gelesen), Hund steht in der rechten Scheibe von Leons Fenster, bricht mit RE2-Splittern + zwei Glasknallen (Ton gemessen) durch, landet und greift Elza an (hp 100 -> 80 -> ... -> 25); einmalig; ohne (9,97) kein Hund (B1); RE1.5-KI-Modus ebenso (R15); Leon 1120 unveraendert |

Gates: RE-Gate haelt (6 Stichproben selbst disassembliert/gelesen, alle stimmen; PORT-WAHLEN gekennzeichnet; keine
Rate-Signale). Vertrag haelt (nur (9,97) gelesen, keine Ids, Slot/Ereignis/Bit frei, erlaubte Pfade, kleine Haken).
Tests: 83/83 gezielt gruen, je Punkt messende Riegel vorhanden.

**bestanden = ja.** Keine Maengel. Hinweise H1-H4 und Anmerkungen A1-A4 sind fuer den Orchestrator/Nutzer, sie
blockieren die Abnahme nicht.

Belege (Scratchpad dieser Abnahme, nicht committet): abn0/A1 (hund.log, state.log, cap.raw, sheet_A1.png,
zoom2_A1.png), abn0/E1 (debug.log mit SE-Debug, sheet_E1.png), abn0/P1, abn0/B1, abn0/R15, abn0/K1; ctest-Protokoll
ctest_gezielt.log.
