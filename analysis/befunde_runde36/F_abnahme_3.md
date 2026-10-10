# Runde 36 — Spur F "zombieki": Abnahme 3

Datum: 2026-10-11. Pruefer: unabhaengige Abnahme 3 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **43220d8d** (Basis master 78a4a748). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md,
Vorgaenger-Abnahme `F_abnahme_2.md` (Stand 4285214f: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3).

> **ERGEBNIS:** (wird fortlaufend geschrieben — Zwischenstand)

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
  14662 (die Original-Regel blockiert nah also MEHR als NB3). Gegenueber NB2 (Abnahme 2: Anker 400 15857, Anker 40 1329) ist
  Anker 400 um 2118 Bilder hoeher, Anker 40 um 1108 niedriger.
