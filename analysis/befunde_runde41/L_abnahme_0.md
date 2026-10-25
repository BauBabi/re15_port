# Runde 41 — Spur L "spinnenanim" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r41_spinnenanim`, Zweig `r41/spinnenanim`, HEAD 915137a9 (Basis master 4c667c97).
Abnahme 2026-10-25 00:46 - 01:34 (Uhr). Ergebnis: **bestanden** (1/1 Punkt erfuellt, Gates halten, keine Maengel; Hinweise H1-H5).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md Z. 47)
> Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand stimmen noch nicht ganz.

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`
- `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`; exe 00:12 juenger als
  die letzte Engine-Quelle (spinnenanim_r41.c / main.c 00:08:52), Arbeitsbaum sauber -> exe == HEAD.

## 1. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`)

Stichproben (alle selbst aus PSX.EXE bzw. STAGE2.BIN gelesen; Sprungziele gegen `ghidra1_V2.txt` benannt):

| Behauptung | Adresse | selbst gelesen | Ergebnis |
|---|---|---|---|
| Zeichner dreht jede Figur mit RotMatrix(+0x68) -> +0x20 | PSX.EXE 0x8001e8e8-8f8 | `lw a1,0(s2)` (s2 = &0x800ac784), `addiu a0,a1,104`, `jal 0x80068098`, Delay `addiu a1,a1,32`; Ziel 0x80068098 = `RotMatrix` (ghidra1_V2.txt Z. 97208 `jal RotMatrix`, Funktionskopf Z. 209977 `lh t7,r->vx(a0)`) | bestaetigt |
| Zeichen-Schleife ruft das fuer jede Figur mit Bit0 | PSX.EXE 0x8001d0e8-168 | `lw a0,-14460(a0)` / `lw v0,0(a0)` / `andi v0,v0,0x1` / `beq` / `jal 0x8001e8c8` @0x8001d108, Stride `addiu v0,v0,500` @0x8001d160 | bestaetigt |
| Massstab: Flag 0x800 -> ScaleMatrix(+0x166) | PSX.EXE 0x8001e8fc-944 | `lw v0,0(s0)` / `andi v0,v0,0x800` @0x8001e904 / 3x `lh v0,358(v1)` @0x8001e91c/928/938 / `jal 0x80065ff0` @0x8001e940 = `ScaleMatrix` (ghidra1_V2.txt Z. 207078, XREF FUN_8001e8c8:8001e940) | bestaetigt |
| Spinnen-INIT setzt Flag 0x800 und +0x166 = 0xccc | STAGE2.BIN 0x80110de0-dfc | `lw v0,0(v1)` / `ori v0,v0,0x800` @0x80110de8 / `sw v0,0(v1)` @0x80110dec / `ori v0,zero,0xccc` @0x80110df8 / `sh v0,358(v1)` @0x80110dfc | bestaetigt |
| Schatten 1300 x 1400 im INIT | STAGE2.BIN 0x80110d68-d94 | `ori a2,zero,0x514` @0x80110d78, `ori a3,zero,0x578` @0x80110d84, `addiu v0,v0,176` @0x80110d8c (Record +0xb0), `jal 0x8001af5c` @0x80110d90; RE_15_Quellcode_V2/FUN_8001af5c.c: `+0xc = param_3`, `+0xe = param_4` | bestaetigt |
| Schatten je Takt auf +0x1ba | STAGE2.BIN 0x80110b4c-b54 | `lh a1,442(v0)` @0x80110b4c, `jal 0x8001b064` @0x80110b50, `addiu a0,a0,176` (Delay); FUN_8001b064.c: `t[1] = param_2` (Hoehe = a1), Ecken aus Record +0xc/+0xe | bestaetigt |
| anim_set waehlt das Bild mit +0x95 VOR dem Hochzaehlen | PSX.EXE 0x8001f324-38c | `lbu v0,148(t0)` (+0x94) @0x8001f324, `lbu v0,149(t0)` (+0x95) @0x8001f35c, `sw a2,360(t0)` @0x8001f36c, `jal 0x8001f3bc` @0x8001f38c | bestaetigt |
| Teile ketten an die Eltern-Matrix (Teil 0 an Figur+0x20) | PSX.EXE 0x8001e9ec-a28 | `lw s0,108(s2)` @0x8001ea0c (Eltern-Matrix-Zeiger Teil+0x6c), `addiu s1,s2,64`, `jal 0x80022da0` @0x8001ea24 mit a1 = Teil+0x18, a2 = Teil+0x40 (Weltmatrix) | bestaetigt (Live-RAM-Gegenprobe Teil0+0x6c == Figur+0x20 nicht selbst nachgelesen) |
| Pose mit +0x8f VOR dem Herunterzaehlen, +0x95 erst danach | PSX.EXE 0x8001f408-63c | `lbu s0,143(v1)` @0x8001f408, `jal 0x8001f664` @0x8001f534, `mult v0,v1` @0x8001f598, `addiu v0,v0,-1 / sb v0,143(v1)` @0x8001f5b0-b4, `jal 0x80020510` @0x8001f5d0 (a3 = 0x1000 - Gewicht), `lbu v0,149 / addiu v0,v0,1 / sb v0,149` @0x8001f610-1c, Umbruch `sb zero,149` @0x8001f63c | bestaetigt |

Sprungziele: 0x80068098 (RotMatrix) und 0x80065ff0 (ScaleMatrix) sind PsyQ-Signaturen im Ghidra-Dump der richtigen
Binaerdatei (RE1.5 PSX.EXE); 0x8001f3bc ist der direkte Aufruf aus 0x8001f314 @0x8001f38c. RotMatrix selbst nachgerechnet
(RE_15_Quellcode_V2/RotMatrix.c, Sinustabelle sin(1024) = 4096): RotMatrix(0,1024,1024) = [0,0,4096; 4096,0,0; 0,4096,0],
x ScaleMatrix(0xccc) = [0,0,3276; 3276,0,0; 0,3276,0] = gemessene Original-Figur-Matrix Takt 93 — statischer Code und
Laufzeitmessung stimmen ueberein.

Erster Giftklumpen des Originals: Runde-38-Log n136 `fx0 ... p5741,-320,4049` (alle drei Klumpen) = Konstante des Riegels B.

Original-Messung der Spur gegengeprueft gegen die UNABHAENGIGE Runde-38-Messung (anderer Agent, andere Sitzung):
`L_belege/orig_2060_gdb.log` vs `analysis/befunde_runde38/R_belege/orig_2060_schacht_gift.txt`, eigenes Skript
`vgl_r38_l41.py` (Scratch): 150/150 gemeinsame Zeilen gleich in Zustand, Drehung, Clip/Bild, +0x1ba und Lage (auch y
exakt) -> die Original-Grundlage der Spur ist reproduzierbar.

Wortsuche im Diff (`git diff master...HEAD -- re15_port`, dazu "approx", "geschaetzt", "vermut", "ungefaehr"):
"deferred", "tunable", "interim", "for now", "faithful", "plausibel", "TODO" -> **0 Treffer**. Kein Env-Schalter als
Abschluss (RE15_SPINNENANIM_LOG ist reine Messschiene, aendert kein Verhalten).

Erklaert der Fix den Befund? Ja: die Runde-38-Choreografie (Phasen @0x80114ec8) war schon gleich; falsch war das Zeichnen
(nur Gier statt RotMatrix(+0x68), kein ScaleMatrix 0xccc, Bild nach statt vor dem Vorschub, Schatten 500x600 auf Figur-y).
Genau diese vier Stellen aendert der Diff, und die Messung 2.x zeigt, dass danach Matrix, Bild und Ueberblendung je Takt dem
Original gleichen.

Zusatzbeleg Reihenfolge KI vor Zeichnen im Original (fuer Hinweis H1): Hauptschleife `jal 0x8001a50c` (Gegner-Schleife)
@0x8001ce04 vor `jal 0x8001e8c8` @0x8001d108 (Figuren zeichnen) - im selben Bild (ghidra1_V2.txt Z. 94379 / 94578).

Konstanten im Diff: 0x0ccc (@0x80110df8), 0x514/0x578 (@0x80110d78/@0x80110d84), Schwelle Zustand 4/+0x5 = 1
(`Zustandswort 0x104 @0x80110d14`), Landetakt Phase 8 (@0x80115338) — alle mit Adresse. Test-Toleranzen 7/2/2
(test_r41_spinnenanim.c) mit Herleitung aus gpf12/gpl12 @0x800664e0 im Testkopf.

## 2. Messung am Spiel

### 2.1 M1 — Leon, RE2-KI (Standard), ECHTER Tuerweg mit Aktionstaste (kein RE15_FIRE_AOT)
Befehl (Scratch `L_abn0/lauf.sh`, Messkopie `re15_pc_L_abn0_m1_leon_re2.exe`, eigener Laufordner):
`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=2030@100 RE15_PLAYER_POS=-7700,-7250,1024,9
RE15_PRESS=square@130 RE15_INPUT_SCRIPT=W5,U1.2,W8 RE15_INPUT_SCRIPT_START=0 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_SPINNENANIM_LOG=spinnenanim.log RE15_SPINNEN_LOG=spinnen.log RE15_FRAMEDUMP=100-460/1:fd_ RE15_EXIT_AT=460#2060`
debug.log: `[aot] DOOR FIRE slot=3 rect=(-7700,-7250,hw=500,hh=900) target_cut=0 spawn=(7600,0,3950)` (Viereck-Druck an
der Tuer), `[tuer] Sequenz ... DOOR1D`, `[room] PC loaded room2060.rdt`, Leon geht ab Raumbild 150 in den Cut 1,
`[flow] EXIT_AT: Bild 460 in Raum 2060`. Spinne Platz 1 (5800,-3600,6400): Phase 1 ab Raumbild F158.

Eigener Vergleicher `vgl_port_orig.py` (liest das Original-GDB-Log DIREKT, nicht die erzeugte .inc-Tabelle; Original-Takt K
gezeichnet = Clip/Bild-1/Ueberblendung/+0x20 aus Zeile n(K+1)), RE2-Clipnummern 15/14 -> 16/13:
- Figur-Matrix des Zeichners == Original +0x20: **73/73** Takte 62..134 (Wand rot_z = 1024: **40/40**)
- gezeichnetes Bild + Clip == Original: **73/73**; Ueberblendung == Original: **73/73**
- Nach der Landung (Takt >= 135) uebernimmt im RE2-Modus das RE2-Gehirn (Runde-38-Port-Wahl) -> Abweichung ab Takt 135
  erwartet (andere KI, andere Spielerlage), nicht Teil des Punkts.
- Boden-Referenz/Schatten: spinnen.log F188 `gy=-3600`, F189 (Takt 93) `gy=0` -> der Schatten faellt beim Uebergang auf die
  Wand auf den Boden wie im Original (Original n94 `gy0`).

### 2.2 M2 - Leon, RE1.5-KI, derselbe echte Tuerweg
Befehl wie M1 mit `RE15_AI_FLAVOR=re15`, `RE15_FRAMEDUMP=0-20/1:fd_`. debug.log: `[aot] DOOR FIRE slot=3 ...`,
`[room] PC loaded room2060.rdt`. Vergleicher: Ausloeser F158; Takte 62..134: Figur-Matrix **73/73** (Wand **40/40**),
Bild+Clip **73/73**, Ueberblendung **73/73**. Ab Takt 135 Abweichung (Port dreht sich zu Leon, Original spuckt) - Folge der
anderen Spielerlage (Original-Messung: Leon teleportiert auf (6000,3000)), nicht des Zeichnens.

### 2.3 M3 - Elza, RE2-KI, echter Tuerweg ROOM2031 -> ROOM2061
`RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=2031@100`, sonst wie M1. debug.log: `[pl] Spieler-Familie PL04 (character=4,
Elza-Bit=1)`, `[room] PC loaded room2031.rdt`, `[elzaems] Typ 0x25 aus EMD/CDEMD1.EMS ...`, `[aot] DOOR FIRE slot=3 ...`,
`[room] PC loaded room2061.rdt`. Vergleicher: Figur-Matrix **73/73** (Wand **40/40**), Bild+Clip **73/73**, Ueberblendung
**73/73**. Kontaktbogen `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/L_abn0/m3_bogen.png`.

### 2.4 Bildvergleich mit dem Original-Video (Zuordnung selbst nachgemessen)
Video `L_belege/orig_2060_schachtspinne.mp4` (vp9, 640x480, 201 Bilder) mit ffmpeg in Einzelbilder zerlegt; Bild-zu-Bild-
Differenz (eigenes Skript): Kamerawechsel Cut 0 -> 1 bei Videobild 29 (0-basiert, Differenz 20,41), danach Wechsel nur an
ungeraden Bildern (2 VSyncs je Takt), groesster Sprung im Gitter-/Wandausschnitt bei Bild 91 (0,82 = Phase-2-Sprung Takt 93)
und Bild 171 (0,63 = Absprung von der Wand Takt 133) -> **Videobild j = 29 + 2 (K - 62)** (0-basiert; = Angabe des Dossiers
1-basiert).
- M1 (Leon steht links, Spinne frei sichtbar): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/L_abn0/m1_bogen_a.png` (Takt 62-92),
  `m1_bogen_b.png` (93-122), `m1_bogen_c.png` (126-135): Kippen ueber die Gitterkante, Kopf-ueber-Lage mit dem Bauch an der
  Wand (Hinterleib oben), Abstieg, Landung wie im Original; Groesse passt in den Gitterrahmen.
- M4 = Szene der Original-Messung (`RE15_DEBUG_JUMP=2060@100 RE15_POCC_TP=150,6000,3000,1024`, nur fuer die Bilddeckung;
  Ausloeser F155, Vergleicher wieder 73/73, Wand 40/40, Bild 73/73, Ueberblendung 73/73): `m4_bogen_a.png` (Takt 76-96),
  `m4_bogen_b.png` (100-133): Lage, Groesse und Beinstellung der Spinne um Leon herum decken sich mit den Original-Videobildern.
  Landung in dieser Szene: spinnen.log F226 `@(5740,0,4332) r=(0,1024,0)`, F227 Spieler `PL(6018,0,2906)` = Original
  n134 `@5740,0,4332` / n135 `pl 6018 0 2906` (gleich).
- Landung/Uebergabe im echten Lauf M1 (RE2-KI): `m1_landung_F226-237.png` (F226-237): Wandfuss -> Boden -> RE2-Gehirn ohne
  sichtbaren Sprung in der Pose.
- Restunterschied ohne Bezug zur Animation: die Spinne wirkt im Port etwas heller/kontrastreicher als im Original-Video
  (Beleuchtung, nicht Teil des Punkts, nicht weiter untersucht).

### 2.5 Raumeintritt 2060 (Halten im Schacht, Cut 0) - Nebenbefund
M2 mit `RE15_FRAMEDUMP=0-20/1`: Helligkeit F0..F4 0,0-0,8 (Einblendung), **F5 14,5**, F6.. 27,5. Messschiene:
`L41 F5 s1 st0/0/0 c0 b0 f0 M0,0,4096,0,4096,0,-4096,0,0` (vor dem ersten KI-Takt: ohne Massstab, Clip-0-Pose),
`L41 F6 s1 st4/1/0 c15 b0 f0 M0,0,3276,...` (ab hier INIT-Pose mit 0.8). Bild `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/L_abn0/m2_eintritt_F4-7.png`
und `m2_eintritt_F5hell_F6.png`: im letzten Einblendbild F5 haengen fuer EIN Bild grosse Spinnenbeine aus dem Schacht oben
links, ab F6 nur noch die Beinspitzen (= Original, Dossier-Bild vgl_cut0_halten.jpg). Ursache ist der port-weite
Gegner-INIT-Takt (Gegner werden 6 Bilder vor ihrem ersten KI-Takt gezeichnet; im Original laeuft die Gegner-Schleife
`jal 0x8001a50c` @0x8001ce04 im selben Bild VOR `jal 0x8001e8c8` @0x8001d108) - bekannt als Spur-A-OFFEN "Gegner-INIT-Takt"
(UEBERGABE 3b, spaetere Runde). Vor Runde 41 hingen die grossen Beine dauerhaft heraus; jetzt nur noch in F5. -> Hinweis H1.

### 2.6 Zweite Spinne / andere Raeume
- ROOM20A0 (zweite Schachtspinne): ueber den Riegel C (Spielschritt gegen die Runde-38-Original-Messung
  orig_20a0_schacht.txt) - 90/90 Logik, gezeichnetes Bild, Figur-Matrix (Wand 57/57), Schatten-Hoehe, beide KI-Modi
  (eigener Lauf von test_r41_spinnenanim.exe, Abschnitt 3). Kein exe-Lauf in 20A0.
- ROOM2060 Spinne e1 (-6600,-3600,800): Ausloeser ist Cut 5 (sub01 `Cmp 23 00 0a 00 05 00` @0x01128 -> sub03 Member_set
  grid 0x42); Sonde M5 (Gleiten nach (-4500,1000)) erreichte Cut 5 nicht -> nicht gemessen. Gleicher Code-Pfad (Typ 0x25,
  Zustand 4/+0x5 = 1) wie e0.
- INIT-Zweige: STAGE2 0x80110c20-0x80110d14 verzweigen nach grid (<2, 2..31, 0x40, 0x41), laufen aber alle ab 0x80110d18
  linear durch anim_set @0x80110d60, Schatten @0x80110d90, Flag/Massstab @0x80110de8-dfc -> 0.8 und 1300x1400 gelten im
  Original fuer JEDE Adultspinne (auch Boden-Spinnen 2030 usw.); der Port setzt beides fuer jede 0x25 in beiden KI-Modi.
  Im RE2-Modus zeichnet der Port fuer 0x25 das Hybrid-Rig (RE1.5-Mesh + RE1.5-Bindlaengen, re2_hybrid_perm 0x25 = identisch,
  20 Knochen; main.c pc_enemy_load) -> der RE1.5-Massstab passt auch dort zur gezeichneten Geometrie.

## 3. Tests
Gezielte Gruppe (Spur + beruehrte Module: Spinnen 0x25/0x26/RE2-Spinne/Babys/Spiderman, RotMatrix, Gegner-/NPC-Schatten
(main.c-Schattenblock), Trefferkaesten, Zustand/Gift, 20A0-Rolltor, Dachkiste (Spinnen-Messung), Kopflauf):
`ctest --test-dir re15_port/build -R "unit_damage|unit_spider_ai|unit_adult_spider_ai|unit_rotmatrix|unit_re2_spider_ai|
unit_re2_spider_water_2090|unit_re2_hit_repeat_crow_spider|unit_re2_baby_spider_dmg|probe_spinne_elevation|r31_trefferboxen|
unit_r38_spiderman|unit_r38_griff_spiderman|unit_r38_spinnen|integration_r38_spinnen_echtlauf|r39_gegner_schatten|r39_zustand|
r41_spinnenanim|integration_headless_run|unit_re2_crow_shadow_release|r35_raeume_hundeschatten|r36_pri_schatten|
unit_ai_flavor_mixed|r36_dachkiste|r38_rolltor20a0|unit_r39_gegner|probe_p3_spider|probe_r16_spider|probe_re2_baby_spider|
probe_re2_hit_crow_spider|unit_anim_rate|npckante" --timeout 600 --output-on-failure -j 4`
-> **`100% tests passed, 0 tests failed out of 63`** (u.a. integration_r41_spinnenanim 132,8 s, integration_r38_spinnen_echtlauf
130,2 s, integration_r39_zustand_gift 107,8 s, integration_r39_gegner_schatten 63,0 s, integration_r36_dachkiste 1403,7 s unter
Last). Kein roter Test, nichts nachzufahren.

`test_r41_spinnenanim.exe` selbst gestartet: A RE1.5-KI und RE2-KI je A0-A5 ok (A1 73/73, Wand 40/40; A2 73/73; A3 Weltmatrix
max 6 <= 7, Weltlage max 1 <= 2; A5 73/73, Wand 40/40 am Boden; A4 Gegenprobe alter Weg verfehlt 40/40 Wand-Matrizen, Takt 93,
21 Ueberblendtakte), B erster Giftklumpen (5741,-320,4049) = Original, C 20A0 beide Modi 90/90 (Wand 57/57); `OK (0 FAIL)`.
Die erzeugte Original-Tabelle (test_r41_spinnenanim_orig.inc) stichprobenweise gegen das GDB-Log: Takt 93/133/134 = Zeilen
n94/n134/n135 (Lage, rot, Clip/Bild, M, P0W, P0T, Wurzel) gleich; mein Vergleicher liest das Log direkt und kommt zum selben
Ergebnis wie der Riegel -> Tabelle und Zuordnung stimmen.

Riegel je Punkt: unit_r41_spinnenanim (Mechanik gegen Original-Zahlen je Takt) + integration_r41_spinnenanim (echte exe,
Matrix, die main.c wirklich zeichnet). Parallel-/Linux-Festigkeit: eigene Laufordner (Zufalls-Id), exe-Kopie mit Praefix
`re15_pc_r41l_haken_`, Endung aus RE15_PC_EXE, `cmake_minimum_required` im -P-Skript, `m` gelinkt ausser WIN32.
Hinweis: der Integrationsriegel feuert die Tuer mit `RE15_FIRE_AOT=3@30` (Haken), nicht mit der Aktionstaste; meine Laeufe
M1-M3 haben die Tuer mit dem Viereck-Druck geoeffnet und dasselbe Ergebnis gemessen.

## 4. Vertrag / Pfade
- `git diff master...HEAD --name-only`: nur analysis/befunde_runde41/{L_spinnenanim.md, L_belege/*, AUFTRAG.md, VERTRAG.md
  (beide nur aus dem Orchestrator-Sync e5c914fe)} und re15_port/{engine/src/spinnenanim_r41.c, include/re15_spinnenanim_r41.h,
  engine/src/spinnen_r38.c, engine/src/enemy_ai_common.c, engine/src/scd_room_setup.c, platform/pc/main.c,
  tests/unit/test_r41_spinnenanim.c + 2 .inc, tests/unit/probes/r41_spinnenanim.cmake, tests/integration/test_r41_spinnenanim.cmake}.
  Kein release/, platform/android/, shared_assets/, pri/, synchro/, keine Test-CMakeLists. Keine Bank-9-Bits, keine
  Nachrichten-Ids, keine Assets, nichts unter shared_assets/RE2.
- Gemeinsame Dateien nur kleine Haken mit Kommentar "Runde 41 Spur L": enemy_ai_common.c +3 (Include + 2 Aufrufe),
  scd_room_setup.c +2 (Include + Installer-Zeile am Blockende), main.c 11 Zeilen an 6 Stellen (Include, 2 Schatten-Haken,
  Pose-Klammer, Matrix-Bedingung, Messschiene). VERTRAG 1.3 nennt main.c und scd_room_setup.c nicht fuer L - die Haken sind
  aber klein, markiert und fuer den Zeichner unvermeidbar (Hinweis H4). Probe-Merge `git merge-tree --write-tree master HEAD`:
  Code (enemy_ai_common.c, spinnen_r38.c, main.c) ohne Konflikt, Konflikt nur in AUFTRAG.md/VERTRAG.md (Orchestrator-Sync).
- Typ-Gate geprueft: Matrix-, Bild-, Schatten-Haken greifen nur fuer Typ 0x25 (spinnenanim_r41.c `SPINNE_TYP`); die
  Pose-Klammer in main.c laesst anim_frac aller anderen Typen unberuehrt (s_frac_spiel 0xffff). render_scale_q12 wird ausser
  im Zeichner nur von Gorilla-/Gator-/Affen-Pfaden gelesen (grep) -> keine Spielmechanik-Nebenwirkung der 0.8.
- Beide Szenarien gemessen (Leon 2060, Elza 2061); Nachbarraum 2030/2031 ueber integration_r38_spinnen_echtlauf (gruen).

## 5. Urteil

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| "Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand stimmen noch nicht ganz." | **erfuellt** | Echter Tuerweg (Viereck an Tuer 2030/2031 slot 3) fuer Leon RE2-KI, Leon RE1.5-KI und Elza: Figur-Matrix, gezeichnetes Bild und Ueberblendung der Schachtspinne in 73/73 Takten (Herausklettern 62-92, Wand 93-132 = 40/40, Landung 133/134) gleich dem Original-GDB-Log, das selbst mit der unabhaengigen Runde-38-Messung 150/150 uebereinstimmt; Bildvergleich mit dem Original-Video (Zuordnung selbst nachgemessen) deckt sich; Mechanismus an 9 Original-Stellen selbst disassembliert. |

Gates: RE-Gate haelt (alle Konstanten mit Adresse, 9 Stichproben bestaetigt, keine Rate-Woerter). Vertrag haelt (Pfade,
keine Ressourcen, kleine markierte Haken). Tests: gezielte Gruppe 63/63, Riegel der Spur messend.

**Maengel: keine.**

Hinweise (nicht blockierend, nachpruefbar):
- **H1** Raumeintritt 2060/2061: im letzten Einblendbild F5 wird die Spinne noch vor ihrem ersten KI-Takt gezeichnet
  (Messschiene `L41 F5 s1 st0/0/0 c0 b0 f0 M0,0,4096,...`): ein Bild lang ohne Massstab in der Clip-0-Pose, Beine haengen aus
  dem Schacht (2.5). Port-weiter Gegner-INIT-Takt (Original: `jal 0x8001a50c` @0x8001ce04 vor `jal 0x8001e8c8` @0x8001d108),
  bereits als Spur-A-OFFEN "Gegner-INIT-Takt" fuer eine spaetere Runde vorgemerkt.
- **H2** Nach der Landung (Takt >= 135) zeichnet der Port wieder das Bild NACH dem Vorschub (Dossier OFFEN 2, port-weit).
- **H3** Integration: Spur D (integration_r41_kleinkram_a_gift, ROOM2030 Giftklumpen) nach dem Merge von L im
  Integrationsbaum fahren - L aendert den Gift-Anker (RE1.5-KI) und den Massstab jeder 0x25.
- **H4** VERTRAG 1.3 listet main.c/scd_room_setup.c nicht fuer L (Haken klein und markiert, mergen konfliktfrei).
- **H5** Spinne im Port heller als im Original-Video (Beleuchtung, nicht Animation).

Messwerkzeuge dieser Abnahme (Scratch, nicht committet): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/L_abn0/lauf.sh` (Messkopie + Laufordner),
`vgl_port_orig.py` (Port-Messschiene gegen Original-GDB-Log), `vgl_r38_l41.py` (Original-Log gegen Runde-38-Messung),
`bogen.py` (Kontaktbogen Original-Video/Port); Laufordner m1_leon_re2, m2_leon_re15, m3_elza_re2, m4_szene_orig, m5_e1_sonde.
