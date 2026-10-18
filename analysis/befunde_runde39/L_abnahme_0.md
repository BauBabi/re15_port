# Runde 39 — Spur L "optik": Abnahme 0 (unabhaengig, 2026-10-18)

Baum `.claude/worktrees/r39_optik`, Zweig `r39/optik`, HEAD 9726b606 (Basis 3e49ff87). Alles hier selbst gemessen
(Messkopie `re15_port/build/platform/pc/re15_pc_r39l_abn0.exe`, Laeufe im Scratchpad `abn0/`); dem Bau-Agenten
wurde nichts geglaubt, was nicht am gebauten Stand reproduziert ist. Bericht fortlaufend geschrieben.

## 0. Bau

`bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
`LOCAL-BUILD-OK (build)` (exe 07:53, Quellen aelter; Baum sauber, `git status` leer). Diff gegen die BASIS 3e49ff87
(nicht gegen master: master ist seit der Basis um 387a7a69/8223bd19 weitergelaufen, daher zeigt `git diff master`
AUFTRAG/UEBERGABE/VERTRAG/orchestrator/make_package als "geloescht" — das ist master-Drift, nicht die Spur):
30 Dateien, 1674+/36-: `engine/src/optik_r39.c` (neu), `include/re15_optik_r39.h` (neu), `gen/itemorte_r38_lage.inc`,
`gen/itemorte_r38_md1.inc`, `platform/pc/main.c` (+2), `platform/psx/main.c` (+2), Tests (r39 neu: 2 Integrations-Skripte,
`probes/r39_optik.cmake`, `tests/unit/test_r39_optik.c`; angepasst: r37_pri1100 marke/elza, r38_itemorte .cmake/.c),
`tools/r38_itemorte/itemorte_export.py`, Dossier + Bilder/Skripte.

## 1. Punkt 1 — "Leon ist immer noch vom Boden ueberdeckt an den einen Streifen" (Marke F741)

### 1.1 Nutzer-Marke (vorher)
`analysis/befunde_runde39/nutzer/befund_marken_2026-10-17.txt` Zeile 37: `F741 R1100 C1 hp=85 pos=(-16007,0,-23658)
rot=3023 vz=10829/10699/10570 scr=(169,143) kasten=x158..180,y108..146 masken=52 | [42:(178,59)7x109 t=124 DECKT] ...
[48:(167,63)5x90 t=148 DECKT] [49:(167,71)4x81 t=152 DECKT] [50:(167,108)2x42 t=156 DECKT] [51:(166,120)2x28 t=160 DECKT]`.
Im Bild `befund_1100_F741_marke1.bmp` (960x720, selbst angesehen, Ausschnitt x6): Leons rechte Haelfte ist ab Spalte ~166
(320er) abgeschnitten; zwischen Schnittkante und der hellen Eckleiste der Flurwand liegt ein Streifen mit Boden und
Sockel des HINTEREN Gangs. Das ist der Befund.

### 1.2 Messung nachher (eigene Laeufe, exe des Baums)
Befehl (je Lauf eigener Ordner, `RE15_WINDOW_SCALE=1`, Bild 150 in ROOM1100):
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=1100@250 RE15_PLAYER_POS=-16007,-23658,3023 RE15_FORCE_CUT=1 RE15_GEGNER_POS=1,-14000,-11000,1024@5#1100
RE15_FRAMEDUMP=150:f.ppm RE15_EXIT_AT=150#1100 RE15_BEFUND_MARKE=140 re15_pc_r39l_abn0.exe`
A = so, B = dazu `RE15_NO_PRI=1`, C = Leon hinter die Kamera (`RE15_PLAYER_POS=-14000,-11000,3023`).
debug.log A: `[optik_r39] ROOM1100 Cut 1: Westwand an der gemalten Ecke gekappt: 52 -> 52 Rechtecke, 1395 Rechteckpunkte
jenseits der Ecke entfernt (Runde 39 Spur L)` dann `[pri] cut=1 pri_offset=0x60C masks=52 fg_atlas=1`.

`probe_r36_ppm_diff` (Schwelle 24, Zeilen 40..170):

| Fenster | A/C (Leon gezeichnet) | A/B (Maske ueber Leon) | B/C |
|---|---|---|---|
| Streifen x 166..172 | **140** | **1** | 141 |
| Wand x 173..180 | **0** | **95** | 95 |

Je Spalte A/B: `171:1 173:46 174:27 175:14 176:3 177:2 178:2 179:1` (vorher laut Marke/Dossier ab 166). Je Spalte A/C:
`160:14 ... 166:31 167:24 168:24 169:20 170:14 171:11 172:16` und ab 173: 0. Bild `abn0/p1_ABC_x5.png` (A | B | C)
selbst angesehen: Leon steht im Quergang, seine rechte Seite endet jetzt an der hellen Eckleiste; im Streifen davor
ist er ganz zu sehen; B (ohne Masken) zeigt ihn zusaetzlich UEBER der Wand (falsch, Masken aus) — A ist also genau
"bis zur gemalten Kante sichtbar, dahinter verdeckt". befund.log F140 (Lauf C, Leon hinter der Kamera): nur noch
`[48:(171,111)5x46 t=140 frei] [50:(171,111)3x44 t=144 frei] [51:(171,111)1x42 t=148 frei]` — die Streifen-Rechtecke
beginnen jetzt bei 171 (Sockelband) statt 166/167.

Gemalte Ecke selbst nachgemessen (Bild C = Szene ohne Leon; Masken kopieren BG-Punkte): Halbwert-Kante Rueckwand
(166..170) -> Eckleiste (173) in 71 Zeilen 40..110: **172,78** (172,23..173,08) -> Wand ab Spalte 173 (Bau-Agent: 172,84);
Sockelband 112..148 dunkelste Spalte **171** (L 7,7; 170: 12,1, 172: 17,4) -> Wand ab 171. Beide Fenster in
`k_fenster[]` stimmen mit dem Bild. Projektion mit der Cut-1-Kamera (RDT @0x80, fov 26684 -> H 208): Quaderecke
(-15690,0,-23200) -> Spalte 165,9 (y -4000: 166,3) — das Quaderende der Masken lag also 5..7 Spalten links der gemalten
Ecke, genau der Streifen der Marke. Leon (-16007,0,-23658) -> (169,5, 143,6) vz 10818 == Marke scr=(169,143).

### 1.3 Echter Weg (ohne RE15_FORCE_CUT)
Sprung auf die Marken-Lage ohne FORCE_CUT liefert **Cut 0** (`F140 R1100 C0 masken=0`): die Lage (-16007,-23658) liegt
9 Einheiten innerhalb der Cut-1-Zone (RVD Satz 3, Kante x = -16016), der Sprung waehlt aber den Cut neu, waehrend der
Nutzer von Cut 1 aus hineingelaufen ist (Marke F322 C1 bei (-15132,-23390), F337 C0 bei (-16986,-24280)). FORCE_CUT=1
stellt also die Nutzer-Szene nach, nicht eine erfundene. Lauf-Protokoll siehe 1.4.

## 2. RE-Gate (selbst disassembliert / Bytes selbst gelesen)

`python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis <addr> n` auf `info/Re1.5/PSX.EXE`, `xxd` auf den RDTs:

| Zitat des Bau-Agenten | Selbst gelesen | Urteil |
|---|---|---|
| Maske = SPRT mit tpage 0x95, @0x80039630/34 | `80039628 ori a1,zero,0x1 / 8003962c ori a2,zero,0x1 / 80039630 ori a3,zero,0x95 / 80039634 jal 0x80069858 / 80039638 sw zero,16(sp)` (SetDrawMode(p,1,1,0x95,0)), `80039640 jal 0x8006b958` (SPRT-Bau) | stimmt |
| OT[depth] @0x80039650..60 | `80039650 lh a0,2(s3) / 80039654 sll v0,v0,12 / 80039658 sll a0,a0,2 / 8003965c addu a0,a0,s6 / 80039660 jal 0x8006b538 / 80039664 addu a0,v0,a0` (AddPrim in OT[depth] des aktuellen Puffers) | stimmt |
| Figur OT-Wort otz>>4 @0x8002565c | `80025654 sra v0,v1,6 / 80025658 beq v0,zero,0x80025690 / 8002565c sra v1,v1,4 / 80025660 sll v1,v1,2` | stimmt |
| ZSF3 341 @0x80066c70 | `80066c70 addiu t0,zero,341 / 80066c74 .word 0x48c8e800` (ctc2 t0 -> ZSF3), danach `addiu t0,zero,256` (ZSF4) | stimmt |
| ROOM1100.RDT Cut 1 @0x80, Cut 2 @0xA0, Masken-Offsets 0x60C/0x610 = FF FF FF FF | `@0x80: 0000 3c68 becc ffff faf3 ffff d8ca ffff d8c1 ffff 46fc ffff ca9c ffff 0c06 0000` = fov 26684, Ort (-13122,-3078,-13608), Ziel (-15912,-954,-25398), pri 0x60C; `@0xA0: ... f4cc ffff ... a4aa ffff 6ac6 ffff ... 00d3 ffff 1006 0000` = Ort (-13068,-3078,-21852), Ziel (-14742,-954,-11520), pri 0x610; `@0x60C: ffff ffff ffff ffff` | stimmt (Original ohne Masken in Cut 1/2) |
| SCA-Zelle @0x6A4 `d0 07 5c 2b 40 bb 60 a5` = w 2000, d 11100, x -17600, z -23200 | `@0x6a4: d007 5c2b 40bb 60a5 01ff 0003`; Layout `re15_sca_entry_t` (re15_rdt.h: width, density, x, z, type, u0, u1, floor) | stimmt |
| ROOM1101.RDT bytegleich ROOM1100.RDT | `cmp` ohne Ausgabe | stimmt |
| Original-Maske ROOM1030 Cut 3 Nr. 2: dst (203,66) 24x64 Tiefe 127, endet Spalte 226; gemalte Pfeilerkante 227,18 | Cut-3-Satz @0xC0 -> pri `f80b` = 0xBF8: `0400 2800` (4 Gruppen, 40 Masken), Gruppe 0 `0900 0078 4300 c2ff` (dx 67, dy -62), Maske 2 @0xC30 `0800 8880 7f00 9500 1800 4000` = src (8,0), (136,128) -> dst (203,66), Tiefe 127, 24x64. Pfeilerkante im Engine-Dekoder-Bild (probe_bg_dump, ROOM10303.ppm) Halbwert-Kante Zeilen 66..129: **227,18** (224,71..228,61, n 64) | stimmt — die Original-Maske endet an der gemalten Kante |
| Fenster k_fenster[]: Cut 1 Kante 172,84 / Sockel 171; Cut 2 133,94 / 134,86 | BG-Dump ROOM11001.ppm: Eckleiste 40..110 **172,84** (172,24..173,08, n 71), Sockel 112..148 dunkelste Spalte **171** (L 7,3); ROOM11002.ppm: Wand 97..105 **134,07** (133,74..134,47, n 9 -> letzte Wandspalte 133), Sockel 108..122 **134,86** (134,46..135,12) | stimmt (Cut-2-Mittel 134,07 statt 133,94, gleiche Spanne, gleiche Spalte) |
| RE2 Obj_model_set: ROOM6150 @0x2C98 (-3654,-1872,-17226); Small Key 10F0 @0x0706 (-19675,-679,-24541), 20E0 @0x18F8 (-22150,-1600,-22050), @0x1A96 (-21100,-1300,-13600), 2020 @0x1C1E (-9350,-1400,-19050); Kameras @0x64 + 32*cut | `6150 @0x2c98: 2d08 ... 1000 baf1 b0f8 b6bc`; `10f0 @0x706: 2d04 ... 0a00 1000 25b3 59fd 23a0`; `20e0 @0x18f8: 2d02 ... 7aa9 c0f9 dea9`; `@0x1a96: 2d02 ... 94ad ecfa e0ca`; `2020 @0x1c1e: 2d06 ... 7adb 88fa 96b5`; 6150-Header: nCut @1 = 0x0E, Kamera-Offset @0x24 = 0x64 | stimmt (alle Ursprungswerte, Attribut 0x0010 = flach) |

Konstanten im Code: `k_fenster[]` (4 Zeilen, je mit Messwert im Kommentar, Datenquelle BSS/ROOM1100 Cut 1/2 — Bildmass,
kein Original-Mechanismus, als Messung gekennzeichnet); `RE15_R38K_{PARK,PRISON}_S10000` 24100/25623 und die
`RE15_R39L_*` erzeugt aus den RE2-Dateien (Offsets zitiert, oben geprueft) ueber `RE2_QUANTIL = 0.75` — das Quartil ist
eine gekennzeichnete FORM-WAHL (NUTZER-VORGABE "kleiner/subtiler" + Beta->Retail), mit Messreihe begruendet (Median = 23-
Pixel-Fleck, Maximum = 72 % der alten Breite); RE2 hat keinen haengenden Schluessel, der Port-Fix uebernimmt die einzig
uebertragbare RE2-Groesse (Bildlaenge). Kein Env-Schalter, kein "deferred/tunable/interim/for now/plausibel/TODO" im Diff
(grep leer). Der Fix ERKLAERT den Befund: die Streifen-Rechtecke 45..51 begannen bei 166..172 (Quaderende = SCA-Zelle
z -23200 -> Spalte 165,9), die gemalte Ecke liegt bei 173/171; genau diese 5..7 Spalten waren der Streifen der Marke,
und nach dem Kappen beginnt dort kein Rechteck mehr (befund.log 48/50/51 bei 171 im Sockelband).

## 3. Punkt 2 — "Der Schluessel im Raum 10F0 und 10F1 sollte schon kleiner/subtiler sein, wie er dran haengt, nicht so riesig"

### 3.1 Messung (eigene Laeufe, exe des Baums; Bild 150, Cut 5, Pinnwand-Fenster der Runde-38-Abnahme)
Leon: `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10F0@250 RE15_PLAYER_POS=0,4000,1024 RE15_FORCE_CUT=5
RE15_SET_FLAG=9:71,3:94 RE15_EXIT_AT=150#10F0` ("ohne" = dazu `9:82`), `RE15_WINDOW_SCALE` 1 bzw. 3.
Elza: `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_GOTO_ROOM=10F1 RE15_PLAYER_POS=0,4000,1024
RE15_FORCE_CUT=5 RE15_EXIT_AT=150#10F1` ("ohne" = `RE15_SET_FLAG=9:83`); debug.log `[pl] Spieler-Familie PL04` in allen
vier Elza-Laeufen. debug.log: `[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei
(1358,-2495,-3400) rot (3072,0,2048), Zone Slot 8` bzw. `ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) ... rot (1024,0,0)`.
VORHER = dieselben Szenen mit der exe des Hauptbaums (master 8223bd19, Stand v0.8.26 fuer Schluessel/Masken, gebaut 01:15,
als Kopie mit `RE15_ASSET_ROOT=<Baum>/re15_port/shared_assets/PSX`; debug.log ohne `[optik_r39]`-Zeile).
`probe_r38_itemorte_kontrast mit ohne x0 y0 x1 y1`:

| Lauf | nachher: Pixel, Huelle (B x H), Sum|dL| | vorher (master-exe) |
|---|---|---|
| Leon Skala 1 (x131..145 y75..87) | 7, x136..140 (5 x 2), 506 | (Runde 38: 55, 12 x 7) |
| Leon Skala 3 (x393..437 y225..263) | **69, x408..421 (14 x 8), 3731** | **491, x397..432 (36 x 23), 28852** |
| Elza Skala 1 | 5, x137..140 (4 x 2), 317 | (Runde 38: 27, 12 x 5) |
| Elza Skala 3 | **63, x408..423 (16 x 10), 3960** | **217, x397..432 (36 x 15), 11899** |

AUSSEN je 0 (nichts ausserhalb des Pinnwand-Fensters aendert sich). Alle Nachher-Zahlen bitgleich zu den Dossier-Zahlen
des Bau-Agenten; die Vorher-Zahlen bitgleich zur Runde-38-Abnahme. Breite im 320er Bild: Park 4,67 (14/3) bzw. 5 px,
Prison 5,33 bzw. 4 px — das Runde-38-Mass war 12 Spalten, die Zettel an der Pinnwand sind ~7 Spalten (21 Pixel bei
Skala 3). Beide Schluessel sind jetzt SCHMALER als die Zettel.

Augenschein (Bilder `abn0/p2_s3_vorher_nachher_x3.png`, `abn0/p2_s3_schluessel_nah_x8.png`, selbst angesehen): vorher ein
Schluessel, der breiter ist als jedes angeheftete Blatt (Parking Lot Key) bzw. ein grosser Ringschluessel (Prison Key);
nachher ein kleiner Schluessel neben dem unteren Zettel — Klinge + Kopf (Park) bzw. Ring + Schaft (Prison) sind bei 960x720
weiterhin als Schluessel lesbar, bei Skala 1 (320x240) nur noch ein paar Punkte (5 bzw. 4 breit), wie in RE2 selbst.
Lage, Drehung, Zone unveraendert (unit_r39_optik_schluessel, selbst gelaufen: alle PASS; export-Werkzeug ohne --schreiben
selbst gestartet: `RE2 oberes Quartil 4.45 px (Median 4.07) aus 2 Kameras -> s 2.4100`, `5.78 px (Median 4.26) aus 11
Kameras -> s 2.5623`, RE2-Kameraliste bitgleich zum Dossier).

### 3.2 Form-Urteil
"kleiner/subtiler ... nicht so riesig" ist eine NUTZER-VORGABE ohne Zahl; die Zahl kommt aus RE2 (Beta -> Retail):
Bildlaenge desselben Modells an RE2s eigenen Platzierungen (Bytes oben geprueft, Kameras aus der RE2-RDT @0x64+32*cut
mit `re15_camera_build_view`). Das obere Quartil statt des Medians ist begruendet und mit Messreihe belegt (Median:
Prison Key 23 Pixel ohne Ring; Maximum: 72 % der alten Breite). Aufnahme: siehe 3.3.

## 4. Vertrag (VERTRAG.md)

* Pfade (git diff 3e49ff87 --name-only): nur `analysis/befunde_runde39/L_optik.md` + `L_optik_bilder/`, `engine/src/optik_r39.c`,
  `include/re15_optik_r39.h`, `engine/src/gen/itemorte_r38_{lage,md1}.inc`, `platform/pc/main.c`, `platform/psx/main.c`,
  `tests/integration/test_r3{7_pri1100_marke,7_pri1100_elza,8_itemorte,9_optik_marke,9_optik_schluessel}.cmake`,
  `tests/unit/probes/r39_optik.cmake`, `tests/unit/test_r38_itemorte.c`, `tests/unit/test_r39_optik.c`,
  `tools/r38_itemorte/itemorte_export.py`. KEIN `release/`, `platform/android/`, `shared_assets/PSX/`, keine CMakeLists
  (Registrierung ueber den Probe-Glob `tests/unit/probes/*.cmake`, CMakeLists.txt:4060). Die MASKS/-Dateien bleiben
  unveraendert (Kappen beim Laden), `pri_stage1_hashes.inc` unveraendert -> Paket-Gate unberuehrt.
* Gemeinsame Dateien: `platform/pc/main.c` +2 Zeilen (Kommentar "Runde 39 Spur L"), `platform/psx/main.c` +2 Zeilen —
  Haken direkt hinter `re15_masken36_filter`, in dem Block, der bei JEDEM Wechsel (Raum, Cut, entladen-Generation) die
  Masken neu ableitet (`s_pri_room/s_pri_cut/s_pri_gen`, main.c ~6003), also auch bei Cut-Wechseln innerhalb des Raums.
  `render_pc.c` unangetastet.
* Bank-9-Bits / Nachrichten-Ids / Slots: keine neuen (grep ueber den Code-Diff leer); Aufhebe-Zone Slot 8 wie Runde 38.
* Tests je Punkt, messend: Punkt 1 `unit_r39_optik_{kappen,ecke,leon,elza}` + `integration_r39_optik_marke` (A/B/C-Laeufe,
  Schwellen gegen den Vorher-Wert 134), Punkt 2 `unit_r39_optik_schluessel` (rechnet das RE2-Quartil aus den RE2-Dateien nach)
  + `integration_r39_optik_schluessel` (Skala 1/3, Leon + Elza PL04, Aufnahme beider Schluessel). Angepasste Riegel
  (r37_pri1100 Wandfenster 168 -> 171, r38_itemorte Schwellen) sind Folge der beiden Aenderungen und im Kopf der Skripte
  begruendet; das r37-Fenster 168..170 war im Sockelband der hintere Gang (eigene Messung: Wand ab 171), ein Zombie dort
  ist zu Recht sichtbar.
* Parallel-/Linux-Festigkeit der neuen Skripte: `cmake_minimum_required` vorhanden, exe-Kopie mit eigenem Praefix + Zufalls-Id
  und Endung der Quelle (kein `.exe` fest), eigener WORKDIR je Lauf, `m` fuer Nicht-Windows, keine Backslash-Pfade.
  `RE15_RE2_DIR` (info/re2leon) als Compile-Definition wie bei bestehenden Riegeln (CMakeLists 1778, probes/r30_karte-3010).
* PSX-Haken: nur gegengelesen (kein PSn00bSDK hier) — gleiche Signatur/Reihenfolge wie PC, `re15_pri_masken_raum` ueber
  `re15_pri1100_r37.h` (psx/main.c:37) sichtbar, `optik_r39.c` ohne PC-spezifische Includes ausserhalb `#ifdef RE15_PLATFORM_PC`.

### 1.4 Echter Weg, Elza, Nebenwirkungen (Punkt 1)
* **Vorher-Beleg mit der v0.8.26-exe** (Hauptbaum-Kopie, s. 3.1): Marken-Szene -> befund.log `F140 R1100 C1 pos=(-16007,0,-23658)
  rot=3023 vz=10829/10699/10570 scr=(169,143) kasten=x158..180,y108..146 masken=52 | [42:(178,59)7x109 t=124 DECKT] ...
  [45:(172,61)6x99 t=136 DECKT] [46:(170,62)...` — **bitgleich zur Nutzer-Marke F741**; Streifen A/B = 134 (neu: 1).
  Bild `abn0/p1_marke_vorher_nachher_x5.png`: links der Streifen ueber Leons rechter Haelfte, rechts Leon bis zur Eckleiste.
  Neue exe, Marken-Szene: `F140 R1100 C1 ... masken=52 | [42:(178,59)7x109 t=124 DECKT] [43:(176,60)6x104 t=128 DECKT]
  [44:(174,61)6x101 t=132 DECKT] [45:(173,61)5x50 t=136 DECKT] ...` (Rechteck 45 beginnt bei 173 statt 172, 46..51 bei
  171 im Sockelband statt 166..170).
* **Gelaufener Weg** (Kamera waehlt selbst, `RE15_DEBUG_JUMP=1100@gp`, Start (-14700,-21800) rot 1024, `RE15_INPUT_SCRIPT=
  U0.9,R0.5,U0.6` mit `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30`, Bild 240 in ROOM1100): Leon am Suedende des
  Flurs (-15132,-23033) rot 2464, Cut 1 (walk-Log `F240 cut=1`), befund.log (periodisch) `kasten=x144..168,y110..151
  masken=52 | keine Maske am Koerper`; dieselbe Fahrt mit der v0.8.26-exe: `[48:(167,63)5x90 t=148 DECKT] [49:(167,71)4x81
  t=152 DECKT] [50:(167,108)2x42 frei] [51:(166,120)2x28 frei]`. Bild A (Masken) gegen B (RE15_NO_PRI=1) im Kasten: 0
  Punkte (neu wie alt — an dieser Lage streiften die Rechtecke nur den Mess-Kasten, nicht die Silhouette); Bild
  `abn0/p1_weg_vorher_nachher_ohne_x5.png`.
  Weiter bis zur Marken-Lage per Skript: jeder Versuch, der den Flur verlaesst und nach Westen geht (z -23975/-23966/
  -24122), kippt die Kamera nach Cut 0 (RVD-Viereck `1->0`, Kante bei z -23658 x = -16016, bei z -23975 x = -15944 —
  stimmt mit dem Dossier); die Nutzer-Lage liegt 9 Einheiten vor dieser Kante, d.h. der Nutzer stand am aeussersten Rand
  der Cut-1-Zone. Der Sprung dorthin ohne FORCE_CUT waehlt Cut 0 (Neuwahl beim Spawn). Die FORCE_CUT-Szene ist deshalb die
  einzige Nachstellung der Nutzer-Szene und mit der alten exe bitgleich zur Marke — gewertet wie der echte Weg.
* **Elza (ROOM1101)**: `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=1101@250 RE15_PLAYER_POS=-16007,-23658,3023
  RE15_FORCE_CUT=1 ...` (debug.log `[pl] Spieler-Familie PL04`, `[pri] ROOM1101: Masken der Leon-Variante MASKS/ROOM1100.MSK`,
  `[optik_r39] ROOM1100 Cut 1 ... 52 -> 52`). befund.log `R1101 C1 ... masken=52 | [42:(178,59)... DECKT] ... [45:(173,61)5x50
  ...]` (dieselbe Liste wie Leon). Diff (Zeilen 40..170): Streifen 166..172 A/C **170** (Elza gezeichnet), A/B **37**
  (Spalten 171:19, 172:18, Zeilen 111..131), Wand 173..180 A/B 77, A/C **0**; vorher (v0.8.26-exe) A/B im Streifen 188.
  Die 37 Restpunkte liegen im Sockelband auf der dunklen Kantenlinie 171 (L 8..16) und der Sockelflaeche 172 (L 24 = wie 173),
  also auf der GEMALTEN Wandbasis, nicht auf Boden (Boden des hinteren Gangs: Spalten <= 169/170, Zeilen 130..148, L 30..51 —
  dort A/B = 0). Ob die Kantenlinie 171 zur nahen Wand (Fenster des Bau-Agenten) oder zur Rueckwand gehoert, ist im Bild
  +-1 Spalte unentscheidbar (Spalte 171 traegt oberhalb Zeile 111 die Sockel-Stufe der Rueckwand bei Zeile 101, darunter
  die durchgehende dunkle Linie bis Zeile 148); die Original-Masken selbst streuen an gemalten Kanten -1,3..+2,6 px
  (ROOM1030 Cut 3, oben). Kein Mangel, als Hinweis vermerkt (H2). Bild `abn0/p1_elza_ABC_x5.png`.
* **Zombie-Szene F892 (Begruendung der r37-Fensteraenderung)**: Leon (-14914,-25645,392), Zombie 1 (-16157,-23876), Cut 1:
  mit Masken sichtbare Zombie-Punkte je Spalte (Zeilen 126..149) `155:2 ... 167:6 168:3 169:3 170:10`, ab 171: **0**; ohne
  Masken bis 183. Fenster NEU x171..191: A/C 0, B/C 84; Fenster ALT x168..191: A/C 16 (= die 168..170-Punkte des hinteren
  Gangs). Die Anpassung des r37-Fensters ist damit gemessen begruendet, nicht gewuerfelt.
* **Nebenwirkung Cut 2 (ebenfalls gekappt, Nordecke, Wand links)**: Leon (-16010,-11632) rot 1024, `RE15_FORCE_CUT=2`,
  Zombie 1 weg: debug.log `[optik_r39] ROOM1100 Cut 2: ... 53 -> 51 Rechtecke, 1149 Rechteckpunkte jenseits der Ecke
  entfernt`; A/B (Maske ueber Leon) je Spalte `122:4 ... 133:28 134:4`, A/C (Leon gezeichnet) ab `134:16 135:25 ...`, B/C
  (ohne Masken) 122..160 — Leon wird genau an der gemalten Ecke (133/134, Fenster `k_fenster[2..3]`) geschnitten, dahinter
  verdeckt, davor gezeichnet (Bild `abn0/p1_cut2_ABC_x4.png`). Andere Raeume/Cuts: `re15_optik_r39_kappen` kehrt ohne
  Fenster unveraendert zurueck (unit kappen: ROOM1100 Cut 0 / ROOM1110 Cut 1 -> -1); die masken36-/pri_masken-/pri_hashes-
  Riegel der Suite sind gruen (Abschnitt 5).

## 5. Tests (gezielter Lauf, KEINE volle Suite im Spurbaum)

`ctest --test-dir re15_port/build -j 2 --timeout 1200 --output-on-failure -R '^(unit_r39_optik_.*|integration_r39_optik_.*|
unit_r37_pri1100_.*|integration_r37_pri1100_.*|unit_r37_masken36_.*|integration_r37_masken36_.*|unit_pri_hashes|
integration_pri_masken|unit_r38_itemorte_.*|integration_r38_itemorte|unit_r36_schluessel_.*|integration_r36_schluessel.*|
unit_r35_cut10f0_.*|integration_r35_cut10f0|unit_r22_10f0_figur|unit_r36_klettern_10f0_.*|unit_r36_szenenleon_abgang10f0|
unit_r37_feuerloescher_.*|integration_r37_feuerloescher|unit_r37_kistenregel_.*|integration_r37_kistenregel|
integration_item_name_census|unit_r38_dining1141_schluessel1011)$'` (parallel zu meinen eigenen GUI-Laeufen, also unter Last):
**`100% tests passed, 0 tests failed out of 64`**, ctest exit 0, 1498 s (u.a. integration_r38_itemorte 382 s,
integration_r39_optik_schluessel 235 s, integration_r37_pri1100_marke 70 s / _elza 95 s, integration_r39_optik_marke 42 s,
integration_r37_masken36_stage3..6 + nb2, integration_pri_masken, unit_pri_hashes). Kein roter Haken, also nichts nachzufahren.
Die fuenf r39-Unit-Teile zusaetzlich direkt gestartet (`test_r39_optik.exe kappen|ecke|leon|elza|schluessel`): alle OK, mit
den Dossier-Zahlen (Cut 1 roh 474 / gekappt 0 jenseits der Ecke, 26975 Wandpunkte, 0 verloren; Cut 2 372 / 0, 23986; Leon
849 Koerperpunkte jenseits: roh 720 verdeckt, gekappt 0, hinter der Wand 303/303; RE2-Quartil 4,45 / 5,77 px aus 2 / 11
Kameras). `unit_r38_itemorte_lage` direkt: `gezeichnete Huelle 4.42 px breit == RE2 oberes Quartil 4.45 (+-0,5)`.

## 6. Urteile

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1 "Ich habe noch mal einen Marker mit F9 gemacht. Leon ist immer noch vom Boden ueberdeckt an den einen Streifen." | **erfuellt** | Marken-Szene (Lage/Cut/Kasten bitgleich zur Marke F741): Masken-Punkte ueber Leon im Streifen 166..172 **134 -> 1** (der eine Punkt: dunkle Sockel-Kantenlinie 171, kein Boden), Leon dort gezeichnet (140 Punkte), hinter der gemalten Wand weiter verdeckt (0 sichtbar, Maske aktiv 95); Elza ROOM1101 188 -> 37 (nur Wandbasis-Punkte 171/172, Boden-Spalten <= 170 frei); gelaufener Weg: `keine Maske am Koerper` (vorher 48/49 DECKT); Ursache belegt (Quaderende = SCA-Zelle @0x6A4 statt gemalter Ecke 173/171, selbst nachgemessen), Original-Mechanismus @0x80039630..60 / @0x8002565c / @0x80066c70 und Original-Form (ROOM1030 C3 Maske 2 endet an der gemalten Kante) selbst gelesen |
| 2 "Der Schluessel im Raum 10F0 und 10F1 sollte schon kleiner/subtiler sein, wie er dran haengt, nicht so riesig." | **erfuellt** | Huelle bei 960x720: Parking Lot Key **36 -> 14 px** breit (491 -> 69 Pixel), Prison Key **36 -> 16 px** (217 -> 63), beide schmaler als die Zettel (21 px), weiter als Schluessel lesbar (Bilder angesehen); Lage/Drehung/Zone unveraendert, Aufnahme beider Schluessel im gruenen `integration_r39_optik_schluessel`; Form = RE2-Bildgroesse (Obj_model_set-Bytes + Kameras selbst gelesen, Werkzeug reproduziert 4,45 / 5,78 px) |

Gates: Bau OK; RE-Gate OK (alle Stichproben stimmen, keine Guess-Tells, kein Env-Schalter, Fix erklaert den Befund);
Vertrag OK (Pfade, kleine Haken, keine Bits/Ids, messende Riegel je Punkt, beide Szenarien); gezielter Lauf 64/64.

**bestanden = true.** Keine Maengel.

## 7. Hinweise (keine Maengel)
* H1 (fuer den Nutzer, aus dem Dossier uebernommen): Groesse des Prison Key (Ring + Schaft, grau) ist bei RE2-Quartil 5,78 px
  dezent (16 Pixel breit bei 960x720); Hebel = `RE2_QUANTIL` in `tools/r38_itemorte/itemorte_export.py` (Maximum 1,0 = 9,0 px).
  Die Nutzer-Vorgabe "kleiner/subtiler" ist erfuellt; ob "zu klein", entscheidet der Nutzer.
* H2: Sockelband-Grenze Cut 1 (Spalte 171) ist im Bild +-1 Spalte unscharf (dunkle Kantenlinie zwischen Rueckwand-Sockel und
  nahem Sockel). Folge: bei Elza bleiben 37 Punkte auf der Linie/Sockelflaeche 171/172 (Zeilen 111..131) verdeckt, bei Leon 1.
  Das sind Wandbasis-Punkte, kein Boden; innerhalb der Streuung der Original-Masken (-1,3..+2,6 px an gemalten Kanten). Falls
  der Nutzer Elza dort als "angeschnitten" meldet, waere `k_fenster[1].x_min` 171 -> 172 der Hebel (Messung, keine Adresse).
* H3: Die zweite Marke `befund_1090_F1357_marke1.bmp` ist die Karte 2F (Positionsmarke ausserhalb des Raums) = AUFTRAG-Punkt
  "In ROOM 1090 ist Leons Positionsmarker ..." -> Spur G (selbst angesehen, Bild ist der Kartenbildschirm).
* H4: PSX-Haken (`platform/psx/main.c` +2) nicht gebaut (kein PSn00bSDK) — gegengelesen, gleiche Form wie der Runde-37-Haken.
* H5 (Integration): `test_r37_pri1100_marke/_elza.cmake` (Fenster 171) und `test_r38_itemorte.cmake/.c` (Schwellen) sind
  gewollte Anpassungen; bei Konflikten diese Fassung nehmen (Begruendung in 1.4 F892 und 3.1).
* Messordner (ausserhalb des Baums): Scratchpad `abn0/` (Laeufe `p1_*`, `e1_*`, `weg_*`, `k_*`, `v_*` (v0.8.26-exe), `z_*`,
  `c2_*`, Bilder `*.png`, BG-Dump `bg1/`, ctest-Log `ctest_ziel_abn0.log`).
