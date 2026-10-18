# Runde 39 — Spur F "zustand" — Abnahme 0 (unabhaengig, 2026-10-18)

Pruefer: Abnahme-Agent 0 (glaubt dem Bau-Agenten nichts Ungemessenes). Baum `.claude/worktrees/r39_zustand`,
Zweig `r39/zustand`, HEAD b8f18185, Basis c71fa144 (merge-base mit master bestaetigt; master hat nur die zwei
Doku-Commits de4d7b74/3eda01a4 voraus). Bericht fortlaufend geschrieben (Sitzungslimit).

## 0. Pruefstand
- `git status --short`: Zweig sauber bis auf 7 UNVERSIONIERTE Messreste in `analysis/befunde_runde39/F_belege/`
  (g_nachher.out, g_unter.out, k_n_{leon,elza}_{o,w}.out, laeufe/g_vorher/ekg_zoom_1560.png) — "Baum sauber" stimmt
  nur fuer den Index (Hinweis H1, kein Mangel).
- Bau (Abnahme selbst): `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`,
  `ninja: no work to do` — die exe `re15_port/build/platform/pc/re15_pc.exe` (11:06:04) entspricht dem letzten
  Code-Stand 87180698 (die vier spaeteren Commits aendern nur Tests/Dossier: `git show --stat`). Messkopie der
  Abnahme: `re15_pc_r39f_abn0.exe` (+ Werkzeugkopien `test_r39_zustand_abn0.exe`, `test_r36_kleidung_abn0.exe`).
- Code-Diff (`git diff master -- re15_port`) vollstaendig gelesen: zustand_r39.c (neu, 130 Z.), re15_zustand_r39.h
  (neu), Haken in game_step_common.c (+3/-4), item_use_common.c (+2), re15_inv_screen.c (+2), spinnen_r38.c (+4/-1),
  platform/pc/main.c (+25: pc_player_skin_reload + 2-Zeilen-Haken), Tests test_r39_zustand.c (neu),
  test_r38_spinnen.c (RE1.5- -> RE2-Form), probes/r39_zustand.cmake, zwei Integrations-Skripte.

## 1. RE-Gate (Adressen selbst nachgelesen)
Werkzeuge: `ghidra_re2_Leon.txt` / `ghidra1_V2.txt` (Listing) + Rohbytes direkt aus `info/re2leon/PSX.EXE`
(Basis 0x80010000, Groesse 0xf0800) und `info/Re1.5/PSX.EXE` (Basis 0x80010000) per Python.

| Behauptung (Diff/Dossier) | Selbst gelesen | Urteil |
|---|---|---|
| RE2 Pause-Tor FUN_8003bfac `DAT_800cfbdc < 0 -> return` @0x8003bfac-c0 | `8003bfb0 lw v0,-0x424(v0)=>DAT_800cfbdc` / `8003bfc0 bltz v0,LAB_8003c1fc`. DAT_800cfbdc ist das RE2-Pausewort: Schreiber `lui v1,0xef00 / or / sw` @0x80025cb0-bc setzt u.a. das Vorzeichenbit — Gegenstueck zum RE1.5-`bltz` @0x80031c78 auf 0x800aca40 (Port RE15_PAUSE_PLAYER) | bestaetigt |
| RE2 Gift-Takt @0x8003c12c-180: Giftwort 0x800cfe10, Zaehler 0x800cfe12 u8 -1, bei 0 := 30, `slti v0,v0,0x2` Untergrenze, HP(0x156) -1 | Listing Zeilen 138726-138747: `8003c130 lhu v0,-0x1f0(v0)=>DAT_800cfe10`, `8003c138 beq`, `8003c144 lbu v1,-0x1ee(v1)=>DAT_800cfe12`, `8003c14c addiu v0,v1,0xff`, `8003c154 sb`, `8003c158 bne v1,zero`, `8003c15c _li v0,0x1e`, `8003c164 sb`, `8003c168 lh v0,0x156(s0)`, `8003c174 slti v0,v0,0x2`, `8003c178 bne`, `8003c17c _addiu v0,v1,-0x1`, `8003c180 sh v0,0x156(s0)`; danach @0x8003c188-1a4 der Zustands-Dispatch `lbu 0x4(s0)` / `jalr` ueber PTR_LAB_800a4030 = Spielertask (Aufrufer @0x80025c64/@0x80026620/@0x8004a32c) | bestaetigt (Periode 31, -1 HP, nie < 1) |
| RE2 Op 32 setzt Giftwort 0x1518 und Zaehler 30 @0x80020120-40 | `80020120 _li v0,0x1518`, `80020130 sh v0,-0x1f0(at)=>DAT_800cfe10`, `80020138 li v1,0x1e`, `80020140 sb v1,-0x1ee(at)=>DAT_800cfe12` | bestaetigt |
| RE2 Statusschirm-Klassifikator: Giftwort != 0 -> 4 vor allem anderen (FUN_80069538) | `80069544 lhu v0,DAT_800cfe10` / `80069550 bne v0,zero,LAB_80069588` mit `_li v0,0x4`; dann `srl a0,1 / sltu` (HP > max/2 -> 0), `sltiu 0x29` -> 1, `sltiu 0x15` -> 2, sonst 3 | bestaetigt |
| RE2 EKG-Farbtabelle @0x800ab160, 6 Bytes je Zustand, POISON @0x800ab178 = ff 20 ff / 08 01 08 | Rohbytes: cond0 `20 ff 20 01 08 01`, cond1 `ff ff 20 08 08 01`, cond2 `ff 80 20 08 04 01`, cond3 `ff 20 20 08 01 01`, **cond4 @800ab178 `ff 20 ff 08 01 08`** | bestaetigt |
| RE2 Zeichner FUN_800754ac: Farbe = base - step * s1, s1 = 31..0, Kopf (s1 = 0) = base | `800754d4 li s1,0x20`, Schleifenkopf `800755a4 addiu s1,s1,-0x1`, `800755a8 subu a2,v0,s1` (x = sweep - s1), `800755c0 lbu DAT_800ab163[..]`, `800755c8 mult v0,s1`, `800755d4 lbu DAT_800ab160[..]`, `800755dc subu v0,v0,a3`, `800755e0 sb`; Schleifenende `80075684 bne s1,zero,LAB_800755a0`. Port-Linie i (0 = Kopf, x = sweep-(i+1)) <-> s1 | bestaetigt |
| RE1.5 Heilspray-Wert 100 @0x8004ae14 | `8004ae14 64 00 10 34 _ori s0,zero,0x64` (Dossier schreibt `addiu s0,zero,100` — gleicher Wert, falscher Mnemonic; Hinweis H2) | Wert bestaetigt |
| RE1.5 Gift loeschen @0x8004af8c | `8004af84 lhu v0,DAT_800acaec` / `8004af8c andi v0,v0,0xfffd` / `8004af94 _sh` | bestaetigt |
| RE1.5 EKG-Farbtabelle @0x8007622c: Eintrag 3 (Gift) = 20 ff 20 02 = FINE-Gruen | Rohbytes: `20 ff 20 02 / 80 80 20 07 / 20 ff 20 07 / 20 ff 20 02 / 20 20 20 07 / 20 20 20 07` | bestaetigt (Gift in der Beta unfertig: gleiche Farbe wie FINE) |
| RE1.5 Wellen: Tabellen 1..5 bytegleich, != Tabelle 0 (Zeiger @0x80076214) | Zeiger 0x80075774/592c/5ae4/5cec/5ea4/605c; 77*4 Bytes verglichen: 1==2==3==4==5, 0 anders; y 39..55 / h 0..8 | bestaetigt |
| RE2 POISON-Welle @0x800aad58 (80 Spalten u8 {y,h}) | y 4..26, h 0..10 (Zeiger @0x800ab180: aaad8/aab78/aac18/aacb8/aad58) | bestaetigt |

Guess-Tells im Code-Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|approx`): keine. Env-Schalter:
nur Messschiene (RE15_R39_GIFT_AT / RE15_R39_LOG), kein Verhaltensschalter. Alle Verhaltenskonstanten tragen
`@0x...`: 30/31 (@0x80020138-40, @0x8003c144-64), Untergrenze 2 (@0x8003c174), -1 (@0x8003c17c-80), 100 (@0x8004ae14),
Farbe ff/20/ff und Schritt 8/1/8 (@0x800ab178-7d), cond 3 (FUN_8004ed6c). Punkt 1 ist korrekt als NUTZER-VORGABE
(PORT-WAHL) gekennzeichnet (Original loescht Decals nie; Form = Spieler-Lade-Pfad FUN_800314b0/FUN_8004ee78).

Gift-Quellen im Port (`status_flags |= 0x2`): nur spinnen_r38.c:449 (Giftklumpen) + der Messhaken — beide setzen
den RE2-Zaehler (30). Einziger Aufrufer des Heil-Appliers: menu_common.c:726. `re15_zustand_r39_reset` wird nur in
Tests gerufen (kein Produktions-Reset des Takt-Zaehlers — s. Hinweis H5).

## 2. Vertrag (VERTRAG.md)
- Pfade: keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists (Registrierung ueber
  tests/unit/probes/r39_zustand.cmake, GLOB). Keine Bank-9-Bits, keine Nachrichten-Ids, keine Assets.
- Gemeinsame Dateien: item_use_common.c (+2), re15_inv_screen.c (+2, F zugeteilt), game_step_common.c (+3/-4,
  Austausch des eigenen r38-Hakens), spinnen_r38.c (+4/-1). **platform/pc/main.c: +25 Zeilen** (neue statische
  Funktion pc_player_skin_reload, 19 Z., + 2-Zeilen-Haken) — main.c ist im Vertrag nur A/B zugeteilt und fuer
  gemeinsame Dateien gelten "1-5 Zeilen" -> Vertragsabweichung (Mangel M1, s. 6; Integration muss aufpassen).
- Tests je Punkt vorhanden und messend (s. 5).
- Linux-Festigkeit der neuen Skripte: `cmake_minimum_required` in beiden -P-Skripten, exe-Kopien mit Praefix
  (r39fk/r39fg), eigene Laufordner, `m` fuer Nicht-WIN32, keine Backslash-Pfade — gelesen, in Ordnung.

## 3. Punkt 1 — volle Heilung = ganze Kleidung
Messschiene der Abnahme (Scratchpad abn_lauf.sh, eigene exe-Kopie re15_pc_r39f_abn0.exe): Spielstand per Werkzeug
`test_r39_zustand karte` (ROOM1130 Leon / ROOM1131 Elza, Lage (-1024,0,-3336) rot 856, Cut 2, HP 20, Platz 0 =
Heilspray 0x22), CONTINUE (RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0), Spray ueber den ECHTEN
Statusschirm (RE15_PAD_AT=200:S,260:A,300:A,340:A,500:S), Framedumps 150 (vor) / 650 (nach), je Figur einmal mit allen
8 Wund-Panels Stufe 2 (`_w`) und einmal ohne (`_o`); Mass = `test_r39_zustand bild` (Pixel-Differenz im Figurenkasten
x177..247 / y81..205). RE15_STATE_LOG fuer den HP-Verlauf.

⛔ Messfalle der Abnahme (Umgebung, kein Spurfehler): ein fremder "Xbox 360 Controller" (Slot 0, debug.log Zeile 6)
hielt zeitweise R1 (pad=0800, Leon zielt) — 4 von 12 Laeufen der ersten Serie waren dadurch nicht vergleichbar
(k_leon_o 495 Zeilen, k_elza_w 237, k_leon_teil_o 78, k_leon_eq_w 696). Alle Laeufe wurden mit Pad-Zensus
(state.log: keine Zeile mit R1-Bit) wiederholt, bis sie sauber waren; nur saubere Laeufe werden gewertet.

| Lauf (sauber) | HP (state.log) | Bild 150 w vs o | Bild 650 w vs o (nach dem Spray) | debug.log |
|---|---|---|---|---|
| k_leon_w / k_leon_o (Software-Renderer) | 20 -> 100 in F375 | 737 Pixel, Kasten (192,96)-(231,179), ausserhalb 0 | **0 Pixel** | `Heilung 0x22: HP 100 >= 100 voll -> Wund-Tabelle genullt (hatte Wunden), Spielerhaut neu` / `Spielerhaut PL00.TIM neu geladen (384x256) - Kleidung ganz` |
| k_elza_w / k_elza_o (ROOM1131, Charakter 4) | 20 -> 100 in F375 | 462 Pixel, (193,110)-(223,180), ausserhalb 0 | **0 Pixel** | dito, `Spielerhaut PL04.TIM neu geladen` |
| k_leon_teil_w / _o (Gruenes Kraut 0x24, Negativprobe) | 20 -> 45 in F375 | 737 Pixel | **806 Pixel** (Wunden bleiben), keine r39-Zeile | Teilheilung stellt NICHT wieder her — wie die Vorgabe "vollstaendig" |
| k_leon_gl_w / _o (**GL-Renderer** direct3d, ABR2 ok) | 20 -> 100 | 732 Pixel | **0 Pixel** | dito, PL00.TIM neu (visuell: saubere Uniform) |
| k_leon_eq_w / _o (**Handgun 0x03 ausgeruestet**, RE15_EQUIP=3) | 20 -> 100 | 737 Pixel | **0 Pixel** | der Skin-Reload + Waffen-Composite beschaedigt die Kleidung nicht |
| Echter Weg ROOM1190 ungedreht (Hundebisse, Save vom Spiel, Spray ueber Statusschirm) | 40 -> 100 in F285 | 3 Pixel (Wunde Panel 5 bei dieser Pose verdeckt) | **0 Pixel** | r39-Zeilen + wound-sync; Spray im echten Menue (Cursor 0->2, st 0->6) |
| Echter Weg ROOM1190 **gedreht** (R4 ab F8, Wunde zur Kamera) | 40 -> 100 | 47 Pixel (Blut sichtbar) | **5 Pixel** am Silhouetten-Rand (166-168,88-93), visuell identisch = Rotations-/Antialiasing-Rauschen, kein Decal |

Die Messwerte 737/462 (vorher) und 0 (nachher) sind identisch mit den Zahlen des Dossiers 1.4 — unabhaengig
reproduziert. Mechanik im Code (gelesen): Heil-Applier -> HP >= 100 (+Weste) -> re15_wound_reset + Anforderung ->
main.c pc_player_skin_reload laedt PL%02X.TIM der aktuellen Familie frisch in Slot 0 (re15_render_pc_upload_tim_slot),
der Blut-Decal-Sync findet die genullte Tabelle (kein Blit), der Waffen-Composite haengt an der Slot-0-Generation.

## 4. Punkt 2 — Gift zieht Energie wie RE2, EKG zeigt es
Messschiene: Leon ROOM1130 (Spielstand HP 100 bzw. 5), Gift per Messhaken RE15_R39_GIFT_AT=300 (Bit 0x2 + Zaehler 30
wie Op 32), RE15_R39_LOG=gift.log (je Spielbild hp/status/Zaehler/cond/menu/abzug), Statusschirm per START, Framedumps,
Auswertung `test_r39_zustand hp` / `ekg` (Kasten x26..103 / y118..140 = die 32 EKG-Linien).

| Lauf | Ergebnis |
|---|---|
| g_100 (HP 100, START @1500, Dumps 1540-1600) | **38 Takte, Abzug je 1 HP, erster Takt F330, Abstaende 31: 37 von 37**, HP-Minimum 62 (F1477), letztes Spielbild F1499 (im Statusschirm kein Spielerschritt). EKG: Bild 1540 hell 23 / magenta 23 / gruen 0; 1560 hell 33 / magenta 27 / sonst 6 (dunkle Schwanzlinien); 1580 19/19; 1600 5/5 — gruen = gelb = rot = 0 |
| g_5 (HP 5, Untergrenze) | 4 Takte F330/361/392/423 (5 -> 1), danach bis F900 kein Abzug: **HP-Minimum 1** (`slti v0,v0,0x2` @0x8003c174) |
| g_menu (Statusschirm F600-F700 mitten im Gift) | 25 Takte; Zaehler steht im Menue (z2 = 18 bei F590, 8 bei F714 — die Bildnummern laufen im Menue weiter, der Takt nicht); ein Abstand entsprechend > 31, alle anderen 31 (23 von 24) |
| g_cure (Gegengift 0x23 im Platz 0, Menue F600, USE, zu F900, Menue erneut F1100) | 9 Takte bis F578, dann Gegengift: status 0x0002 -> 0x0000, HP 91 -> 116 (+25 @0x8004af78, kein Clamp), **kein weiterer Abzug bis F1099**; EKG danach Bild 1140 hell 17 / gruen 17, 1160 hell 33 / gruen 33, magenta 0 (FINE) |
| Echter Weg (Giftklumpen der Spinne, integration_r38_spinnen_echtlauf, Laufe lauf_07ae4a07_E/_L aus meinem ctest-Lauf) | aus spinnen.log selbst ausgezaehlt: Takte gz 0 -> 30 mit HP -1 ab F50 alle 31 Bilder (F81, 112, 143, 174, 205, ... 887), 27 Takte je Szenario, dazwischen die Spuck-Treffer als fremde Abzuege; beide Szenarien gleich |

Alle Zahlen des Dossiers 2.4 (38 Takte, F330, 37/37, magenta 23/27/19, Untergrenze 1) unabhaengig reproduziert; die
Gegengift- und Menue-Faelle sind zusaetzliche Proben der Abnahme. Nebenwirkung Gegengift bei HP 91: +25 = 116 >= 100
zaehlt als volle Heilung und stellt die Kleidung her (`Heilung 0x23: HP 116 >= 100 voll`) — konsistent mit der Form
der Vorgabe (additive Heilung ueber die Schwelle).

## 5. Tests
- `ctest --test-dir re15_port/build -R "r39_zustand" --timeout 900 --output-on-failure` (Abnahme, 12:36-12:40):
  **100% tests passed, 0 tests failed out of 3** — unit_r39_zustand 0.01 s, integration_r39_zustand_kleidung
  116.13 s, integration_r39_zustand_gift 98.54 s (Log: Scratchpad ctest_r39_zustand_abn.log).
- Modulgruppe `-R "r38_spinnen|r36_kleidung|unit_item_use|unit_inv_fsm|unit_wound_accumulator|r39_inventar_status|unit_inv_grant|unit_em_status_persist|unit_r36_blut_pin|unit_scd_zone2_pauseflags|r38_zopf"` (Abnahme, parallel zu den Messlaeufen):
  **100% tests passed, 0 tests failed out of 35** (darunter integration_r38_spinnen_echtlauf, integration_r36_kleidung_bild/spielweg/hundeweg, integration_r38_zopf, unit_r38_spinnen mit den umgestellten B/B3/B4/B5; Log Scratchpad ctest_module_abn.log). Keine Wiederholung noetig, kein Flattern.

## 6. Maengel / Hinweise
**M1 (Vertrag, Integration — behebungsbeduerftig).** `platform/pc/main.c` ist im VERTRAG.md (1.3) nur den Spuren
**A und B** zugeteilt, und gemeinsame Dateien duerfen laut Vertrag nur "kleine Haken, 1-5 Zeilen" tragen. Spur F fuegt
dort **25 Zeilen** ein: die neue 19-Zeilen-Funktion `pc_player_skin_reload()` (Zeile ~1390) + einen 2-Zeilen-Haken im
Blut-Decal-Sync (Zeile ~9459). Der 2-Zeilen-Haken ist vertragskonform; die 19-Zeilen-Funktion im gemeinsamen main.c ist
es nicht. Projekt-Standard waere eine eigene PC-Datei der Spur (Vorbild `platform/pc/src/dining_r38_pc.c` der Runde 38).
Kein Korrektheits-/Byte-True-Problem (isolierte neue static-Funktion, byte-true belegt), aber eine echte
Vertragsabweichung; die Integration muss den main.c-Merge mit A/B pruefen (Risiko gering: A/B sind Welle 1 und bereits
in der Basis c71fa144, F haengt an fremden Code-Stellen an). **Behebung:** Funktion nach platform/pc/src/<spur>_pc.c
verschieben ODER Orchestrator gibt main.c fuer F frei und dokumentiert es.

**H1 (Hinweis).** `git status` zeigt 7 UNVERSIONIERTE Messreste in `F_belege/` (g_nachher.out, g_unter.out,
k_n_*.out, laeufe/g_vorher/ekg_zoom_1560.png). Der Index ist sauber, der Arbeitsbaum nicht ganz ("Baum sauber" im
Dossier gilt nur fuer das Committete). Kein Spurfehler.

**H2 (Hinweis, Zitat-Mnemonic).** Dossier/Code zitieren den Heilwert als `addiu s0,zero,100` @0x8004ae14; die Binaerdatei
hat dort `ori s0,zero,0x64` (= derselbe Wert 100, anderer Mnemonic). Der byte-true Wert stimmt, nur die Instruktionsform
im Zitat ist ungenau.

**H3 (offen, vom Bau-Agenten selbst dokumentiert — kein Mangel).** Die EKG-WELLENFORM unter Gift ist die RE1.5-Welle
(Tabelle 3 @0x80075cec, bytegleich mit den Tabellen 1/2/4/5) in der RE2-POISON-FARBE; die eigene RE2-POISON-Welle
(@0x800aad58, 80 Spalten {y,h}, Hub 4..36) wird NICHT uebernommen, weil sie nicht ohne erfundene Skalierung in den
RE1.5-Monitor passt (77 Spalten, Hub 39..55). Das ist **byte-true-konform** (vorhandene belegte Welle, KEINE erfundene
Zahl) und als Entscheidungsfrage an den Nutzer offengelegt (Dossier 5). Fuer den gemeldeten Fehler ("EKG zeigt das Gift
nicht an") irrelevant: Farbe (magenta) und Wort ("Poison") zeigen es jetzt. Selbst disassembliert und bestaetigt.

**H4 (offen, PSX — PC-Port ist primaerer Fokus).** `pc_player_skin_reload` ist nur im PC-Backend; `platform/psx` pollt
`re15_zustand_r39_haut_neu_abholen` nicht. Auf PSX bliebe die Wund-Tabelle genullt, die VRAM-Decals aber bis zum
naechsten Spieler-Laden stehen. Fuer den PC-Port (CLAUDE.md: primaerer Fokus) ist Punkt 1 vollstaendig.

**H5 (offen, ohne Folge).** Der RE2-Takt-Zaehler `s_takt` wird bei neuem Spiel/Laden nicht genullt — RE2 kennt dafuer
keinen Schreiber (selbst geprueft: Writer nur Op 32 @0x80020140, Takt @0x8003c154/64, Skript Poison_clr). Ohne Folge,
weil jeder Gifttreffer ihn auf 30 setzt.

## 7. Urteil
| Nutzer-Punkt | Urteil | Kern-Beleg |
|---|---|---|
| 1. Volle Heilung = ganze Kleidung | **erfuellt** | Nach Heilspray (HP->100) 0 Restpixel w vs o — Leon 737->0, Elza 462->0 (Software), GL-Renderer 732->0, echter Hundebiss-Weg ->0; Teilheilung (Gruenes Kraut, HP 45) laesst 806 Pixel Wunden stehen (= "vollstaendig"). NUTZER-VORGABE, Form byte-true (Spieler-Lade-Pfad FUN_800314b0/FUN_8004ee78, s0=100 @0x8004ae14). Visuell bestaetigt. |
| 2. Gift zieht Energie wie RE2, EKG zeigt es | **erfuellt** | -1 HP alle 31 Bilder (erster Takt F330, 37/37 Abstaende 31), Untergrenze HP 1, in jedem Zustand, nicht im Menue, Gegengift beendet — byte-true RE2 FUN_8003bfac @0x8003c12c-180 (selbst disassembliert). EKG magenta ff/20/ff + "Poison" (@0x800ab178), nach Gegengift gruen "Fine". Echter Spinnengift-Weg gleiche Periode. Visuell bestaetigt. |

**Gates:** RE-Gate haelt (8 zentrale Adressen selbst aus beiden PSX.EXE / Ghidra-Dumps verifiziert; keine Guess-Tells;
jede Verhaltenskonstante @0x... oder als NUTZER-VORGABE gekennzeichnet; keine erfundene Skalierung bei der offenen
Wellenform). Verbotene Pfade (release/, android/, shared_assets/PSX/, Test-CMakeLists): nicht beruehrt. Tests: je Punkt
messend und gruen (unit_r39_zustand + 2 Echtlauf-Riegel 3/3; Modulgruppe 35/35, kein Flattern).
**Vertrag: NICHT vollstaendig gehalten** — M1 (main.c, 19-Zeilen-Funktion in gemeinsamer Datei, F nicht zugeteilt).

**bestanden = false.** Begruendung: **Beide Nutzerpunkte sind funktional und byte-true erfuellt** und unabhaengig
nachgemessen (beide Figuren, beide Renderer, echter Weg). Der einzige behebungsbeduerftige Punkt ist die
**Vertragsabweichung M1** (das explizit als Gate geforderte "gemeinsame Dateien nur kleine Haken" haelt nicht). Das ist
ein Integrations-/Ablage-Thema ohne Korrektheitsrisiko; nach dem Verschieben der Funktion in eine eigene PC-Datei (oder
einer dokumentierten Freigabe von main.c fuer F durch den Orchestrator) ist die Spur gruen. Die EKG-Wellenform (H3) ist
eine legitime, offengelegte Entscheidungsfrage, KEIN Mangel.
