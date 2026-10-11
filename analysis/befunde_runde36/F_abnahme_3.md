# Runde 36 — Spur F "zombieki": Abnahme 3

Datum: 2026-10-11. Pruefer: unabhaengige Abnahme 3 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **43220d8d** (Basis master 78a4a748). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md,
Vorgaenger-Abnahme `F_abnahme_2.md` (Stand 4285214f: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3).

> **ERGEBNIS: NICHT BESTANDEN.** P1 erfuellt, P2 teilweise, P3 erfuellt. Bau, gezielte Regression (200/200), Spur-Riegel
> (46/46 u. a.), RE-Zitate (alle selbst gelesen) und Vertrag halten. Abnahme-2-Mangel 1 (Nordwest-Gang) ist behoben, die
> Widerlegung von Mangel 2 (Taschenplaetze liegen in den Zellen #13/#20/#21) stimmt. Offen (Abschnitt 6): am erreichbaren
> Nordwest-Rand stauen sich nach dem ersten Angriff beide Gorillas (einer nah, der zweite FERN dahinter, Umweg 0 — Gegenbeleg
> zu "fern 0"), und die nahe Rest-Blockade an den Stoss-Zielen (Ostgassen-Eingang, Nordwest-Ecke) bleibt lang und ist am
> Original nicht gemessen.

## 0. Stand, Bau, Messmittel

- `git log master..HEAD`: Abnahme-2-Stand + b7687fb1, 0a06f8c4, 8e754270, 067fcf1d, 24df7e42, b2afb58e, 43220d8d. Arbeitsbaum
  sauber. Eigener Diff des Zweigs = `git diff 78a4a748..HEAD` (master ist seit dem Abzweig um 836023f5/565da62b weiter; ein
  blankes `git diff master` zeigt deren Dateien als "geloescht" — nicht aus diesem Zweig).
- Code seit Abnahme 2 (`git diff 4285214f..HEAD -- re15_port`): NUR Gorilla-Teil — enemy_ai_common.c +1 Zeile
  (`re15_r36_affen_fest_takt(e)` im Gehirn-Schwanz nach den Timern +0x1dc/+0x1e1), zombieki_r36.c (Umweg: +0x1de-Zaehler,
  Frei-Test Vorlage+1, Kanten beidseitig, A*, Startknoten gerade erreichbar, Leon-Seite), Header-Doku, Riegel
  `test_r36_zombieki_affen_block.c`. P1-Code (Sicht, Wurf-Klemme, zonenlos) und P3-Code (Lache) unveraendert.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> "ninja: no work to do",
  `LOCAL-BUILD-OK (build)`. exe 01:25:02, letzte Quellaenderung (Header) 01:18:40, Baum sauber => exe = HEAD
  (md5 4b1c5208...). Messkopie `re15_pc_abn3f_neu.exe` (= HEAD). Vergleich: `re15_pc_abn0f_basis.exe` (v0.8.23-Spielcode,
  md5 2fd81d20...), `re15_pc_abn2f_neu.exe` (NB2 = Abnahme-2-Stand, md5 4bd3a634...).

## 1. RE-Gate (neue Zitate der NB3, selbst gelesen, richtige Binaerdatei)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (STAGE1.BIN roh @0x80100000, PSX.EXE t_addr 0x80010000).

| Zitat im Diff / Commit | Datei | gelesen | Urteil |
|---|---|---|---|
| +0x1de-Zaehler: `lhu v0,470(v1)` @0x80117448, `beq v0,zero,0x8011746c` @0x80117450, `lhu v0,478(v1)` @0x80117458, `addiu v0,v0,1` @0x80117460, `sh v0,478(v1)` @0x80117468 (Delay-Slot von `j 0x80117470`), `sh zero,478(v1)` @0x8011746c; INIT `sh zero,478(v0)` @0x8011706c | STAGE1.BIN | genau so | stimmt |
| "kein weiterer Zugriff auf 478 in 0x80116db8..0x8011c600" | STAGE1.BIN, eigener Scan aller Load/Store mit imm 0x1de | genau die 4 Stellen oben; Gorilla-Callees (34 jal-Ziele) greifen nicht zu (einziges Ziel im Bereich der EXE-Zugriffe 0x8004f3ec.., 0x8004ef90, endet @0x8004efb0 ohne Zugriff) | stimmt |
| FUN_8003bca8 Vorlagen-Vergleich `lh v0,0(a2)` / `lw a3,0(a1)` / `bne a3,v0` / `lh v0,4(a2)` / `lw a0,8(a1)` / `bne a0,v0,0x8003be8c` @0x8003be34-54; Rueckfall `lh v0,0(a2)` / `sw v0,0(a1)` / `lh v1,4(a2)` / `addu v0,zero,zero` / `sw v1,8(a1)` @0x8003be8c-a0 | PSX.EXE | genau so (Code 0: Lage == Vorlage -> Min-Achsen-Schub @0x8003be5c-88; Lage != Vorlage -> Rueckfall auf die Vorlage, Rueckgabe 0). Das "Artefakt" setzt also voraus, dass eine FRUEHERE Zelle desselben Klemmen-Durchlaufs die Lage verschoben hat — genau die Sonden-Kette des Dossiers (#13 -> 9730, #20 -> 13088, #21 zurueck auf 6800); Port-Zwilling `push_rect` re15_collision.c Z. 89-97 identisch | stimmt |
| Path B: `lbu v0,7(v1)` @0x80118ac0, `beq v0,zero,0x80118afc` @0x80118ac8, +0x6a = (+0x9f & 0xf0) << 4 @0x80118ad0-e0, `sb a0,130(v0)` (+0x82 = 1) @0x80118af0 | STAGE1.BIN | genau so: Band 1 nur bei +0x7 != 0 | stimmt |
| Fern-Tor `lhu v0,468(v1)` / `sltiu v0,v0,0x1771` @0x80117bc0-c8 | STAGE1.BIN | genau so | stimmt |
| Diagonal-Rueckfall @0x8003ced4-ceec | PSX.EXE | `lh v0,0(s4)` / `sw v0,0(t2)` / `lh v1,4(s4)` / `addu v0,zero,zero` / `j 0x8003cfdc` / `sw v1,8(t2)` | stimmt |
| Radius u16 @0x80121356 | STAGE1.BIN `00 00 60 fa 00 00 40 06` ab 0x80121350 | 0x0640 = 1600 | stimmt |
| ROOM11C0.RDT: #36 @0x015d8 (x 4900..9500, z -6080..-2180, u1 0x10), #47 @0x0165c (x -17397..-12096, z -6590..-1690, u1 0x10), #13 @0x014c4 (t1, x -28290..-17790, z -28550..8112, fl 03), #20 @0x01518 (t8, x -29390..-17790, z 6800..11470, fl 03), #21 @0x01524 (t1, x -30520..-24920, z 10000..14110, fl 03) | shared_assets/PSX/STAGE1/ROOM11C0.RDT (12-Byte-Saetze w,d,x,z,t,u0,u1,fl) | genau so | stimmt |
| PR = 450 (Leon-Knoten "auf Leons Seite") | re15_collision.c Z. 25/39 `#define PR 450` (DAT_80073e94[6]) | Verweis auf belegte Port-Konstante | stimmt |

- Suche im Code-Diff (`git diff 78a4a748..HEAD -- re15_port/engine re15_port/include re15_port/platform`, nur `+`-Zeilen) nach
  deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/approx/getenv/schaetz/ungefaehr/vorerst/vorlaeufig: **0 Treffer**.
  Schalter `*_enable` nur fuer Sonden.
- Neue Konstanten ohne @0x: `R36_FEST_START 45`, `R36_FEST_ANKER 400` — im Code als Teil der PORT-WAHL "Umweg" gekennzeichnet,
  Herleitung "halbe Mess-Definition der Abnahme (90 Bilder im Anker 400)" + Messreihe 1/15/30/45 -> 24353/22328/25112/20084
  (Radius/4). Hinweis: 45 ist der Rand der Messreihe (bester Wert am Rand, gemessen mit Schritt Radius/4, eingebaut ist
  Radius/2) — eine per Messung gewaehlte Stellgroesse der PORT-WAHL, kein Original-Wert. Weitere Umweg-Parameter (Ring 3/4,
  `3*r/2`, `2*r`, 4 Radien Vorschau, `kt[64]`, Achtel-Metrik 2/3) wie in Abnahme 2 unter der PORT-WAHL ohne Einzelkennzeichnung.

## 2. Gezielte Regression (keine Voll-Suite im Spurbaum)

Regex = Abnahme-2-Regex (Module/Raeume des Zweig-Diffs: enemy_ai_common.c = alle Gegner inkl. Zombie-Klemme, Navigator-Haken,
Gorilla-Zweig; enemy_ai_re2_zombie.c; main.c Schatten) — `affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|
1140|1210|1100|10d0|10b0|1010|1220|11c0|r35_raeume|r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|
r34n_f|r36_zombieki|nav|unit_enemy_|room1070_wake|ai_flavor|re2_room1090|ada_eintritt|r17_waffen_loop|schatten|shadow|grab|griff|
victim|re2_crawler|knockdown|stagger|hit_reaction|re2_livepath|re2_hit_repeat|sca_contact90|sca_wall_hit|npc_wall_clamp|
r16_aufstehen|r34_reaktion|r34_schaden|unit_damage|re2_gore|r26_mg_blut|re2_stepsize|re2_kfjump|poise|re2_hp_model|
room_probe_sweep|r35_granate|r35_redhawk|r35_werfer|r34_granaten`:
`ctest --test-dir re15_port/build -R "<regex>" --timeout 600 -j 6 --output-on-failure` (01:30:37-01:39:04) ->
**100% tests passed, 0 tests failed out of 200** (507,0 s; integration_r35_cut1150 507 s, r35_zgirl 254 s). Kein Fenster-Haken rot.
Darin alle 6 Spur-Tests (`unit_r36_zombieki_{wecker,szenen,affen,sicht,affen_block,eintritt}`), alle 20 `unit_r35_affen_*`
(Spur-J-Nahjagd bildgenau gegen GDB), `unit_maggot_*`, `unit_r17_waffen_loop_pin`. Ausgaben von wecker/szenen/affen einzeln
nachgefahren (`ctest -V`): wecker 2/2 (45 Zombies, 0 stehen mit Sicht), szenen 5/5 (erneuter Angriff Bild 419/366/84/153/282),
affen 9/9 (Tod Bild 90, 1700/1800, 91 Takte, Ende 2428/2528, Gegenprobe ohne Haken = Befund) — bitgleich zu Abnahme 2.

## 2a. Punkt 1 — "Zombies ... bleiben hier ewig beteiligungslos stehen, obwohl ich ziemlich in Ihrer Naehe vor ihnen bin, fast vor ihrer Nase. Auch wenn sie innerhalb eines Raumes sind."

P1-Code seit Abnahme 2 unveraendert (Abschnitt 0). Gemessen wird deshalb (a) ob das Verhalten am HEAD-Stand bitgleich zum
Abnahme-2-Stand ist (dort P1 erfuellt, mit Tuerweg, Lage-Zensus aller Stages, Abnahme-0/1-Faellen) und (b) ein zusaetzlicher
echter Tuerweg.
- exe HEAD, Kommandozeilen der Abnahme 2 (Basis `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_GEGNER_Y_LOG=gy.log`), gy.log byteweise gegen die NB2-Laeufe der Abnahme 2 (`cmp`):
  - ROOM1040 Westgang (`RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1040@250 RE15_PLAYER_POS=-29900,6000,0,0 RE15_EXIT_AT=900`):
    **bitgleich** (1151 Zeilen) — Ostgang ohne Sicht durch die Wand, Westgang-Zombies sehen Leon.
  - ROOM10B0 Terrasse (`RE15_DEBUG_JUMP=10B0@250 RE15_PLAYER_POS=-17000,-8500,2048,0 RE15_EXIT_AT=2400`): **bitgleich** (2651
    Zeilen); F2400 alle vier an der Kante vor Leon (x -18450/-17651/-16850/-16047, z -11832, d 3319..3633, los 1, st1/1/1).
  - **Echter Tuerweg ROOM1020 -> ROOM1010** (`RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1020@250 RE15_PLAYER_POS=-2950,-17700,1024,0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=U0.5,W0.3,A0.1,W8,U0.7,W60 RE15_EXIT_AT=2600`):
    **bitgleich** (5082 Zeilen); 44 Bilder 1020, 765 Bilder 1010 (per Tuer), dann Tod -> 1240. In 1010: F60 beide los 1,
    Slot 2 Griff (st1/3) F60-F120, Slot 1 Griff F240-F300, danach erneut Gang/Griff (Slot 2 F480 st1/3, F540 Sub 6, F660 Sub 8).
- Pruefer-Sonde Lage-Zensus ALLE Stages + Elza-Varianten (`F_abn3/probe_eintritt_alle3.c` = Kopie der Abnahme-2-Sonde, gegen die
  HEAD-Bibliotheken gelinkt): 410 Eintritte, **638 Faelle: PASSIV 1, KLEMME 1** — beide ROOM1091<-10F1 (Elza Band 5 / Zombies
  Band 1, andere Ebene, wie Abnahme 2); Taschenfall ROOM1010 mit Wurf-Klemme 0 Bilder auf der Stelle, Gegenprobe 450;
  ROOM10B0 3643 (Gegenprobe 13025). Fallzeilen bitgleich zur Abnahme 2.
- Abnahme-1-Fall ROOM1010 (`RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3750,4300,1024,0 RE15_EXIT_AT=1500#1010`;
  der Lauf endet erst am Zeitlimit, weil Leon stirbt und `#1010` nie mehr gilt — wie in Abnahme 2): der R1010-Teil (780 Zeilen)
  **bitgleich**; Slot 1 Griff F60, Wurf endet bei (3433,2983) VOR dem Keil, Sub 5 F120-F420, **erneuter Griff F480**, F540 Sub 6;
  Slot 2 los 1 ab F60, Griff F240-F300.
- **Zusaetzlicher echter Tuerweg ROOM1020 -> ROOM1040** (neu; Tuer-AOT aot2 (-27150,-8200) aus `test_r36_zombieki_eintritt aot
  1020`; `RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1020@250 RE15_PLAYER_POS=-26300,-8200,2048,0 RE15_INPUT_SCRIPT_BASIS=spiel
  RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=U0.3,W0.3,A0.1,W8,U1.5,W60 RE15_STATE_LOG=st.log RE15_EXIT_AT=2600`, HEAD und
  v0.8.23): 38 Bilder 1020, 1534 Bilder 1040 (per Tuer), Leon (-24869,16500); alle fuenf Zombies gehen sofort (st1/1/1), Slot 1/2
  mit Sicht von 15000 her den Gang herauf, Slot 2 Griff (st1/3/3) F810, Slot 1/2 Sub 5 F900, Slot 4 Griff F1170, Slot 2
  erneuter Griff F1260; Leon stirbt. HEAD und v0.8.23 unterscheiden sich nur in 35 Zeilen im Sicht-Bit (Punkt-1-Haken), nicht in Lage/Zustand.
- **Urteil P1: erfuellt.** Kein Zombie mit Leon vor der Nase bleibt passiv: Tuerwege 1020->1010 und 1020->1040 sofortige
  Reaktion und Griffe, Abnahme-0/1-Faelle behoben, Lage-Zensus aller Stages beider Szenarien ohne neuen Fall; HEAD bitgleich zum
  Stand, den Abnahme 2 mit "erfuellt" bewertet hat. Hinweise (wie Abnahme 2, kein Mangel): ROOM10B0-Terrassen-Zombies treten
  3,3..3,6 m vor Leon an der Kante (Abstieg per Daten unmoeglich); ROOM1091<-10F1 Ebenen-Abweichung (nicht Spur F).

## 3a. Punkt 3 — "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot sind."

- Code seit Abnahme 2 unveraendert (main.c-Haken, `re15_r36_affen_lache_{init,tick,render}`, INIT-/Leichen-Haken in
  enemy_ai_common.c); Riegel `unit_r36_zombieki_affen` 9/9 (Ausgabe oben).
- exe HEAD (`re15_pc_abn3f_neu.exe`, Kommandozeile der Abnahme 2: `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240
  RE15_PLAYER_POS=-9013,-11800,1024,0 RE15_GIVE=7:200 RE15_EQUIP=7 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20
  RE15_INPUT_SCRIPT=W0.1,(MA0.1,M0.6) x45 ausgeschrieben RE15_FORCE_CUT=5 RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=200-700/10:fd_
  RE15_EXIT_AT=700`, beschleunigter Renderer; Scratch `F_abn3/p3lache_abn3f_neu/`): Slot 2 Zustand 3 ab ~F270, **Leiche
  (Zustand 7) bei (-7478,-12875)**. Rot-dunkle Pixel (G,B <= 24, R >= G+14) im Leichenfenster (120..260, 90..160), Skript
  `F_abn3/lache_px.py`: F290 292, F300 294, F310 307, F320 322, F330 340, F340 355, F350 377, F360 422, F370 442, F380 465,
  **F390 494**, F400..F570 494 — monotones Wachstum ueber ~91 Bilder ab dem ersten Leichentakt, dann konstant; F200..F480
  bitgleich zur NB2-exe (Abnahme 2), danach kommt Slot 3 ins Bild (NB2 frueher, anderer Umweg). Bildstreifen
  `F_abn3/p3_lache_crop_nb3.png` (F280/F300/F330/F360/F400/F450): unter der Leiche erscheint eine dunkelrote Lache und waechst
  sichtbar (selbst angesehen).
- **Urteil P3: erfuellt.**

## 3. Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir irgendwie verbessern."

### 3.1 Riegel und Zufallsgang (selbst gefahren)
- `test_r36_zombieki_affen_block riegel` direkt (Ausgabe `F_abn3/riegel_affen_block.txt`): **46/46**. 16 Seeds AUS 38439 -> AN 5725
  (schlechtester +59), Griffe 73 -> 74. Szenen (Original-Regel | Umweg, erster Angriff G1/G2, blockiert, davon NACH dem ersten
  Angriff (fern)): 1 (-7000,12500) nie/nie | 291/721, 0/350 (0/4); 2 (-7000,12500) G1 ab (-5525,-14883) nie/nie | **G1 nie**/485,
  G1 blockiert 1433; 3 (7000,4000) nie/nie | 928/283; 4 (-24000,10000) nie/nie | 572/360, **G2 nach dem Angriff 1804 (fern 0)**;
  5 Nordwest-Gang (-14000,2000) G1 ab (-9013,-15461) **430/nie | 352/(Angriff)**, kein Pad #36; 6 (-24452,11938) nie/nie |
  601/361, G2 danach 1808 (fern 0); 7 (-22288,11938) nie/nie | 480/340, G2 danach 1840 (fern 0). 4b: Leon rennt 600 Bilder auf
  (-26000,7000)/(-27000,12000)/(-24000,10000)/(-22000,7000) zu und endet 5174/2548/1938/4678 davor; Raster der Tasche mit
  Vorlage = Knoten 87 "frei", mit Vorlage + 1 0.
- 52 Seeds x 6000 Bilder, je Lauf eigener Prozess (`test_r36_zombieki_affen_block eins <i> <0|1> 6000 <anker>`, 208 Laeufe,
  `F_abn3/seeds52_raw.txt`, Auswertung `seeds52_auswertung.txt`):

| Anker | AUS (Original-Regel) | AN (NB3) | besser / schlechter / gleich | schlechtester | Griffe AUS -> AN |
|---|---|---|---|---|---|
| 400 | 114029 (nah 19663 / fern 94366) | **17975** (nah 14662 / fern 3313) | 47 / 3 / 2 | 0x1d0b0823 +135 | 226 -> 245 |
| 40 | 75408 | **221** | 41 / 0 / 11 | +0 | 226 -> 245 |

  Bitgleich zum Dossier NB3. Die FERNE Blockade (Leon >= 0x1771) faellt um 96,5 %, die NAHE (Original-Nahjagd) von 19663 auf
  14662 (-25 %). Gegenueber NB2 (Abnahme 2: Anker 400 15857, Anker 40 1329) ist Anker 400 um 2118 Bilder hoeher, Anker 40 um
  1108 niedriger. Rest Anker 400: 82 % nah.

### 3.2 exe Kampf-Layout, Leon steht (14 Stellen x 3 Staende, je eigener Prozess)
`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700` (+ Basis-Haken), keine Eingabe; alle
42 Laeufe selbst gefahren (`F_abn3/affen_<x,z>_<exe>/`, `runs.log`, je rc=0). Auswertung = Skript der Abnahme 2
(`F_abn3/affen_ausw.py`, Kopie: erster Angriff = Sub 5/6/8/15; fest = aktiv, kein Angriff, >= 90 Bilder im Anker 400, Leon > 3000)
+ Aufteilung nah/fern (`F_abn3/fest_split.py`: fest-Bilder mit d >= 6001). Stellen 1-10 = Abnahme 2, 11-14 = neu: die zu Fuss
erreichbaren Nordwest-Raender (11/12 = Riegel-Szenen 6/7, 13 = Nordwest-Ecke, 14 = Nordstreifen West).

| # | Leon | v0.8.23 Slot 2 / 3 (fest; davon fern) | NB2 | **NB3 (HEAD)** |
|---|---|---|---|---|
| 1 | (-7000,12500) | nie / F1012 (1882/0; 1882/0) | F1514 / F606 (0/0) | **nie / F730** (825/0; 0/0) |
| 2 | (-14000,2000) NW-Gang | **F680** / nie (0/930; 0/930) | F2438 / F1600 (246/99) | **F680 / nie** (0/0) |
| 3 | (6000,-9000) | nie / F324 (1572/1874; 0/0) | F1246 / F339 (102/309) | **nie / F324 (1652/1874; 0/0)** |
| 4 | (-24000,10000)* | nie / nie (1818/1418; alle fern) | F1913 / F851 (719/0) | **F853 / F1570** (0/1192; 0/1) |
| 5 | (7000,4000) | nie / F18 (1011/0) | nie / F18 (98/0) | **nie / F18** (96/0; 96/0) |
| 6 | (9000,10000) | nie / F309 (1102/0) | F1045 / F309 (0/0) | **F1175 / F309** (0/0) |
| 7 | (-14746,-4140) West-Pad | F364 / nie (1834/0; 0/0) | F364 / F1186 (344/0) | **F364 / F1021** (179/108; 0/108) |
| 8 | (7200,-4130) Ost-Pad | F1246 / F274 (0/670) | F981 / F274 (0/396) | **F1050 / F274** (0/468; 0/0) |
| 9 | (-14000,4000) NW-Gang | **F704** / nie (0/1088) | F1939 / F1577 (242/0) | **F704 / F1496** (249/0) |
| 10 | (-14500,6000) NW-Gang | **F694** / F1256 (171/298) | F1806 / F1534 (107/0) | **F694 / F892** (0/0) |
| 11 | (-24452,11938) NW-Rand | nie / nie (1820/1588; alle fern) | F1471 / F853 (0/1384) | **F828 / nie (1528/1464; 0/1465)** |
| 12 | (-22288,11938) NW-Rand | nie / nie (1768/1671; alle fern) | F1892 / F828 (890/0) | **F868 / F1025 (759/893; 0/691)** |
| 13 | (-26500,15500) NW-Ecke | nie / **F1620** (1840/90; alle fern) | nie / F747 (787/0) | **F842 / nie (0/1649; 0/0)** |
| 14 | (-20000,14500) | nie / F1541 (1482/0) | F1534 / F778 (0/0) | **F883 / F896** (0/0) |

\* RE15_PLAYER_POS setzt Leon dort in die Zelle #20; er steht ab F300 auf (-24000,11938) (wie Riegel 4b).

- **Abnahme-2-Mangel 1 (Nordwest-Gang) behoben:** Slot 2 an 2/9/10 F680/F704/F694 = v0.8.23 (bitgleiche Spur an Stelle 2),
  Slot 3 greift an 9/10 neu bzw. frueher an (nie -> F1496, F1256 -> F892). An keiner der zehn Abnahme-2-Stellen greift ein
  Gorilla spaeter an als in v0.8.23 (Dossier-Tabelle NB3 erste Angriffe exakt bestaetigt).
- Summe fest, Stellen 1-10: v0.8.23 15668 (fern 9549) -> NB2 2662 (fern 43) -> **NB3 6643 (fern 205)**; alle 14 Stellen:
  25927 (19812) -> 5723 (44) -> **12936 (2361)**. Gegen v0.8.23 -58 % bzw. -50 %, gegen NB2 das 2,3-Fache.
- **Echter Ablauf** (Ada/Marvin-Szene, `RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11C0@250 RE15_PLAYER_POS=-7000,12500,2048,0
  RE15_STATE_LOG=st.log RE15_EXIT_AT=1700`, Leon nach der Szene (-7107,-12395) rot 651): v0.8.23 / NB2 / NB3 Spur bitgleich —
  Slot 2 aus dem Wagen F900, Sub 3 ab F1100, wartet F1200-F1500 bei (-5043,-14629) d 3041 ohne Sicht, Sub 5 F1600; Slot 3
  Sprung F1400, Sub 5 F1600.
- **Flucht durch die Westgasse** (Leon (-14500,-9000) rot 3072, `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20
  RE15_INPUT_SCRIPT=W15,XU6,W60`, `F_abn3/flucht_west_<exe>/`): nach dem ersten Griff (Slot 2 F277) rennt Leon durch die
  Westgasse auf den Nordstreifen (F600 (-15729,14335), F750 (-15726,17215)); Slot 2 folgt in allen Staenden; Slot 3 (Start
  Ostgasse): v0.8.23 erster Angriff F1399 (fest 144/132), NB2 F968, **NB3 F1084 (fest 0/0)**.

### 3.3 Nordwest-Rand nach dem ersten Angriff: Stau zweier Gorillas (exe + Pruefer-Sonde)
- Die Abnahme-2-Taschenplaetze (-26000,7000)/(-27000,12000)/(-22000,7000) liegen in #13/#20/#21 (Daten oben, Riegel 4b) —
  **die Widerlegung des Bau-Agenten stimmt**; fuer Leon erreichbar ist der Rand z >= 11938 und die Ecke x -26950..-24200,
  z >= 14560 (Leon-Raster `F_abn3/raster_leon_800.txt`).
- An den erreichbaren Raendern 11/12 (= Riegel-Szenen 6/7) laeuft in der exe: Slot 2 kommt per Umweg (F750 Sub 4 bei
  (-16172,7326)), greift an (F828 bzw. F868), stoesst Leon in die Ecke (-26606,14578) bzw. (-26438,14578); danach **steht Slot 2
  in Sub 3 an der #21-Ecke (-23302,14489) d 3305 (nah) und Slot 3 in Sub 4 an der Nordwand (-20579,16118) d 6180..6235 (FERN),
  ohne Bewegung (Stelle 11 F1260-F2700, Stelle 12 F2100-F2700)**; Gorilla-Abstand 3166..3209 (Koerperkontakt 2 x 1600 @0x80121356). Stelle 11: Slot 3 fern
  festgefahren 1465 Bilder, Stelle 12: 691.
- Pruefer-Sonde `F_abn3/probe_stau.c` (bindet den Spur-Riegel als Bibliothek ein, setzt Leon (-26606,14578), G1 (-23302,14489)
  Sub 3, G2 (-20579,16118) Sub 4; Ausgabe `stau_umweg_an.txt` / `stau_umweg_aus.txt`): **900 von 900 Bildern beide im Anker
  400, kein Angriff — mit Umweg UND ohne (Original-Regel)**; G2 `d1d4=6165..6228` (>= 0x1771), `klemme1d6=1`, Sicht 1,
  **`Umweg 0` in jedem Bild**. Ursache im Code: `r36_uw_route` kehrt mit 0 zurueck ("gerader Weg zu Leon frei -> Original",
  zombieki_r36.c Z. 519): der Test `r36_uw_strecke_frei(g, l)` prueft die Knotenstrecke entlang des Nordstreifens z 16100 nur
  gegen SCA-Zellen; der zweite Gorilla, der genau dort an der #21-Ecke wartet, zaehlt nicht. Das Original-B[4] steuert dann
  auf Leons LAGE (rot 1885 = WSW, in G1 hinein), nicht auf den Knoten.
- Stelle 13 (Nordwest-Ecke): Slot 2 greift per Umweg und Streifen wiederholt an (F842 ff.), Slot 3 wartet 1649 Bilder in Sub 3
  an der #21-Ecke (-23302,13088..13306) d 3600..4500 (nah, ohne Angriff). v0.8.23: Slot 3 greift dort an (F1620), Slot 2 steht
  fern in der Westgasse. Je Gorilla gezaehlt greift Slot 3 in NB3 spaeter an (nie) als in v0.8.23; der erste Angriff auf
  Leon kommt frueher (F842 statt F1620).
- Die Riegel-Pruefung "keine FERN-Blockade nach dem ersten Angriff" gilt fuer die sieben Riegel-Szenen (Sonden-Start F195,
  Leon fest); an denselben Leon-Lagen in der exe (Kampf-Layout-Start) gilt sie nicht (oben).

### 3.4 Urteil Punkt 2: **teilweise**
Verbessert: Zufallsgang -84 % (fern -96,5 %), Griffe 226 -> 245; keine Abnahme-2-Stelle mit spaeterem Angriff; Nordwest-Gang
wie v0.8.23; Flucht durch die Westgasse und Nordwest-Raender: die Gorillas kommen jetzt hin (v0.8.23 nie). Dem Wortlaut
"noch oft blockiert" stehen gemessene, reproduzierbare Rest-Blockaden entgegen, die genau dort auftreten, wohin ein Angriff
Leon stoesst: (1) Nordwest-Rand: nach dem ersten Angriff stehen BEIDE Gorillas still (Stelle 11: 1528/1464 Bilder, ~50 s; Stelle 12: 759/893), einer nah
an der #21-Ecke, der andere FERN dahinter im Koerperkontakt — der Umweg loest das FERNE Haengen nicht (`Umweg 0`), entgegen der
Angabe "Blockade nach dem ersten Angriff: fern 0"; (2) Ostgassen-Eingang (6000,-9000): beide Gorillas 1652/1874 Bilder nah
festgefahren (= v0.8.23); (3) Nahjagd insgesamt nur -25 % (Zufallsgang), an den 14 exe-Stellen bleiben 12936 fest-Bilder
(bezogen auf 14 x 2 Gorillas x 2700 Bilder: 17 %; v0.8.23 34 %, NB2 7,6 %). Dass die Nahjagd an diesen Stellen Original-Verhalten ist, ist statisch
(A[3]/B[3] @0x80117b40-c74/@0x80117d4c) und durch die Spur-J-GDB-Spur EINER Szene belegt, an den gemessenen Stellen aber nicht
(GDB-Messung steht laut Dossier aus).

## 4. Gates

- **Bau:** OK (`local_build.sh configure` + `build`, "ninja: no work to do", exe = HEAD, Abschnitt 0).
- **Spur-Tests / gezielte Regression:** 200/200 gruen (Abschnitt 2); `unit_r36_zombieki_affen_block riegel` direkt 46/46. Je
  Nutzer-Punkt ein messender Riegel mit Gegenprobe (P1 sicht/eintritt/wecker/szenen, P2 affen_block, P3 affen). Luecke P2: der
  Riegel prueft "keine FERN-Blockade nach dem ersten Angriff" nur fuer seine sieben Szenen (Sonden-Start F195, Leon fest); an
  den Leon-Lagen der Szenen 6/7 mit dem exe-Startlayout gilt es nicht (Abschnitt 3.3, Mangel 1).
- **RE-Gate:** alle neuen Zitate selbst gelesen und stimmen (Abschnitt 1, u. a. @0x80117448-6c / @0x8011706c, @0x8003be34-54 /
  @0x8003be8c-a0, @0x80118ac0-af0, @0x80117bc8, ROOM11C0.RDT 0x014c4/0x01518/0x01524/0x015d8/0x0165c); Adressen auch in den
  Commit-Messages 0a06f8c4 / 43220d8d. 0 Guess-Tells im Code-Diff, keine Env-Schalter als Abschluss; neue Stellgroessen
  (45/400) als PORT-WAHL mit Messreihe gekennzeichnet.
- **Fix erklaert den Befund?** Abnahme-2-Mangel 1: ja (ungewichtete Komponentensuche -> A*; exe Nordwest-Gang = v0.8.23).
  Abnahme-2-Mangel 2: die Widerlegung stimmt (Plaetze in #13/#20/#21, Leon erreicht sie nicht; Raster-Artefakt mit
  Vorlage = Knoten 87 -> 0); die Verallgemeinerung "Rest = Original-Nahjagd, fern 0" stimmt in der exe nicht (Mangel 1).
- **Vertrag:** Pfade `git diff 78a4a748..HEAD --name-only`: nur analysis/befunde_runde36/F_*, engine/src/{enemy_ai_common.c,
  enemy_ai_re2_zombie.c, zombieki_r36.c}, include/re15_zombieki_r36.h, platform/pc/main.c, tests/unit/probes/r36_zombieki.cmake,
  tests/unit/test_r36_zombieki_* — keine release/, platform/android/, shared_assets/PSX/, Test-CMakeLists, RELEASE_NOTES.
  Gemeinsame Dateien: enemy_ai_common.c +32/-11 (Haken je 1-2 Zeilen, zwei Exporte 5/6 Zeilen im Sichtfilter-Bereich, Spur F =
  Eigentuemer laut VERTRAG 1.4; NB3 +1 Zeile), enemy_ai_re2_zombie.c +2/-1, main.c +4. Keine Bank-9-Bits, Nachrichten-IDs,
  Ereignisse (zombieki_r36.c ohne flag_set/msg_install/scd_event_fire). Tests nur ueber `probes/r36_zombieki.cmake`.
- **Nebenwirkungen:** Zombie-Raeume bitgleich zum Abnahme-2-Stand (Abschnitt 2a); ROOM11C1 (Elza) hat keine Gorillas
  (Abnahme 2, Sonde `probe_affen_raum`); Gorilla-Zweig nur ROOM11C0. Echter Ablauf der Ada/Marvin-Szene bitgleich zu v0.8.23.
- **Volle Suite:** im Spurbaum nicht gefahren (Auftrag) — Orchestrator im Integrationsbaum.

## 5. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Zombies bleiben ewig beteiligungslos stehen, fast vor der Nase, auch innerhalb eines Raumes | **erfuellt** | P1-Code seit Abnahme 2 unveraendert; exe HEAD bitgleich zur NB2-exe (ROOM1010 Abnahme-Fall, Tuerweg 1020->1010, ROOM1040 Westgang, ROOM10B0); neuer Tuerweg 1020->1040: alle fuenf Zombies sofort, Griffe F810/F1170/F1260; Lage-Zensus alle Stages + Elza 638 Faelle ohne neuen Fall. |
| 2 Monkeys ROOM11C0 oft blockiert — verbessern | **teilweise** | Verbessert: Zufallsgang 114029 -> 17975 (fern -96,5 %), Nordwest-Gang = v0.8.23, keine Abnahme-2-Stelle spaeter, Westgassen-Flucht fest 0. Nicht erfuellt: Nordwest-Rand (Stellen 11/12, Riegel-Lagen 6/7) nach dem ersten Angriff Stau — ein Gorilla nah an der #21-Ecke, der zweite FERN dahinter 1465/691 Bilder, Umweg 0 in jedem Bild (Sonde 900/900 an = aus); Ostgassen-Eingang beide 1652/1874 Bilder nah fest; Nahjagd im Zufallsgang nur -25 %; 14 exe-Stellen 12936 fest-Bilder (17 % der Gorilla-Zeit). |
| 3 Blut der toten Monkeys laeuft nicht aus | **erfuellt** | exe HEAD: Leiche ab ~F294, Lache 292 -> 494 rot-dunkle Pixel ueber ~91 Bilder, dann konstant (bitgleich NB2 bis F480); Bildstreifen selbst angesehen; Riegel 9/9. |

## 6. Maengel (nummeriert, nachpruefbar)

1. **P2 FERN-Stau am Nordwest-Rand nach dem ersten Angriff (Gegenbeleg zu "Blockade nach dem ersten Angriff: fern 0").**
   exe `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-24452,11938,1024,0 RE15_EXIT_AT=2700` (und `-22288,11938`):
   Slot 2 greift F828 (F868) an und stoesst Leon in die Ecke (-26606,14578); danach Slot 2 Sub 3 an der #21-Ecke (-23302,14489)
   d 3305, Slot 3 Sub 4 an der Nordwand (-20579,16118) d 6180..6235, beide ohne Bewegung (F1260-F2700 bzw. F2100-F2700), Gorilla-Abstand
   3166..3209 (Koerperkontakt); Slot 3 FERN festgefahren 1465 (691) Bilder. Sonde `F_abn3/probe_stau.c` (Leon (-26606,14578),
   G1 (-23302,14489) Sub 3, G2 (-20579,16118) Sub 4): 900/900 Bilder beide fest, kein Angriff, mit Umweg = ohne Umweg; G2
   `d1d4` 6165..6228, `klemme1d6=1`, `Umweg 0` je Bild. Ursache: `r36_uw_route` (zombieki_r36.c Z. 519) gibt 0 zurueck, weil
   `r36_uw_strecke_frei(g, l)` die Knotenstrecke auf dem Nordstreifen z 16100 nur gegen SCA-Zellen prueft — der dort wartende
   zweite Gorilla zaehlt nicht —, und das Original-B[4] steuert dann auf Leons Lage (rot 1885) in G1 hinein. Abnahmekriterium:
   an beiden exe-Stellen kein Gorilla >= 90 Bilder FERN festgefahren nach dem ersten Angriff; ein Riegel pinnt die Stau-Lage
   (Gegenprobe = Befund); Dossier/Commit-Angabe "fern 0" berichtigen.
2. **P2 Nahe Rest-Blockade an den Stoss-Zielen nicht am Original belegt und nicht verringert.** Stelle (6000,-9000): beide
   Gorillas 1652/1874 Bilder nah fest (Slot 3 (5876,-8818) d 3102, Slot 2 (3040,-10330) d 5400, Sub 3, Sicht 0; = v0.8.23
   1572/1874); Nordwest-Ecke/-Rand (Stellen 4/11/12/13): der zweite Gorilla wartet 1100-1650 Bilder in Sub 3 an der #21-Ecke
   (-23302,13088..14500) d 3300..4500; Zufallsgang nah 19663 -> 14662. Die Begruendung "Original-Nahjagd" stuetzt sich auf
   A[3]/B[3] statisch und die Spur-J-GDB-Spur EINER Szene (griff/szene); an diesen Stellen ist das Original nicht gemessen
   (Dossier NB3 OFFEN). Abnahmekriterium: GDB-/DuckStation-Messung des Originals an (6000,-9000) und an der Nordwest-Ecke
   (Muster J_affen.md) — wartet das Original dort ebenso, im Dossier und unter "Fuer den Nutzer" ausdruecklich benennen; sonst
   (oder zusaetzlich) die nah-fest-Bilder an den Stellen 3/4/11/12/13 mit einem Gate senken, das die Spur-J-Riegel
   (`unit_r35_affen_griff/_szene`, bildgenau gegen GDB) gruen laesst, gemessen an denselben exe-Kommandozeilen.
3. **Voll-Suite** fuer den Endstand: vom Orchestrator im Integrationsbaum zu fahren (gezielte 200 Tests hier gruen).

Hinweise (kein Mangel): (a) Stelle 13 (-26500,15500): je Gorilla gezaehlt greift Slot 3 in NB3 nie an (v0.8.23 F1620); der
erste Angriff auf Leon kommt dort frueher (F842 statt F1620), die Zahl der angreifenden Gorillas ist gleich (1). (b) NB3
blockiert an den 14 exe-Stellen 2,3-mal so viele Bilder wie NB2 (12936 gegen 5723) — der Preis der engeren Startbedingung;
im Dossier als Abwaegung gefuehrt. (c) `R36_FEST_START 45` liegt am Rand der Messreihe (gemessen mit Schritt Radius/4,
eingebaut Radius/2). (d) PSX-Laufzeit: A* je Gorilla und Bild, statische Puffer ~144 KB (nachgerechnet: 4 x 12288 Byte +
4 x 24576 Byte) — als OFFEN mit Plan (Tabelle fuer ROOM11C0) gefuehrt. (e) ROOM10B0-Terrasse und ROOM1091-Ebene wie Abnahme 2.
(f) Nicht selbst nachgeprueft (ginge nur mit Code-Aenderung): die Angabe, ein Umlenken auch NAH (Haenge-Beginn bei Leon > 3000)
mache `unit_r35_affen_griff/_szene` rot. Gemessen ist nur, dass beide mit dem HEAD-Stand gruen sind.
(g) Messmittel im Pruefer-Scratch `F_abn3/` (nicht eingecheckt): `run.sh`, `affen_ausw.py`, `fest_split.py`, `tabelle.py`,
`lache_px.py`, `probe_stau.c`, `probe_eintritt_alle3.c`, `cc.sh`, Rohdaten je Lauf (`gy.log`, `st.log`, Framedumps).
