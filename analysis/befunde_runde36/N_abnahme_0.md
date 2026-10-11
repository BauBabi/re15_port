# Runde 36 Spur N "kartenitems" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r36_kartenitems`, Zweig `r36/kartenitems`, Code-HEAD 3c0737c0 (danach nur
Bericht-Commits), Basis 78a4a748. Abschnitte 0-3.2 vom 2026-10-11 ~05:30-06:30 (Abbruch Wochenlimit);
3.3, 4, 5 Fortsetzung 2026-10-11 ~11:00-12:00 (Scratch `N_abn0b/`: m1_mess.py, figur_mess.py, runs/leon5_*,
leon5_l1_x3.png, ctest_regr.log).
Gemessen am selbst gebauten Stand (`local_build.sh configure` + `build`: "LOCAL-BUILD-OK", ninja "no work
to do" = exe 04:37 entspricht HEAD; nach 04:37 aenderten die Commits nur Tests/Dossier). Messkopie
`build/platform/pc/re15_pc_abn0n_kartenitems.exe` (md5 6f16291b = re15_pc.exe). Scratch:
`%TEMP%/claude/.../scratchpad/N_abn0/` (runs/, *_sheet.png). Bericht wird fortlaufend geschrieben.

## 0. Gelesen
AUFTRAG.md Z. 73-77 (Wortlaut), VERTRAG.md (N: Bits 87-91, Ereignis 28, IDs je Raum 2 ab 28 bzw. Reserve
30/31), Dossier N_kartenitems.md, `git log master..HEAD` (11 Commits), `git diff 78a4a748` (14 Dateien,
Code-Diff vollstaendig gelesen: kartenitems_r36.c/.h, gen/kartenitems_r36.inc (Tabellen/Texte), Haken in
scd_room_setup.c, scd_vm.c, main.c, re15_map_owned.c, map_hint_common.c, Tests, Generator).

## 1. Messung je Nutzer-Punkt

### 1.0 Welcher Raum ist map3F.bmp? (Vorfrage zu Punkt 1)
Eigener Bildvergleich (`probe_bg_dump` = Engine-BSS-Dekoder, alle 428 STAGE1-Hintergruende; mittlere
|dR|+|dG|+|dB| ausserhalb der roten Marke; `N_abn0/marken_match.py`):

| Marke | bester Cut | Abw. | Zweitbester | ROOM1150 bester |
|---|---|---|---|---|
| map3F.bmp (rot 110 px, Huelle 231..241 x 72..81) | **ROOM1140 Cut 5** | **8,5** | ROOM1230 Cut 2: 75,2 | Cut 4: 113,8 |
| map2F.bmp | ROOM10D0 Cut 6 | 2,0 | ROOM1210 Cut 1: 70,1 | — |
| map1F.bmp | ROOM1030 Cut 3 | 12,8 | ROOM1030 Cut 10: 15,1 | — |
| mapB1.bmp | ROOM11D0 Cut 0 | 1,3 | ROOM10A0 Cut 0: 57,1 | — |
| mapB2.bmp | ROOM1200 Cut 2 | 3,7 | ROOM11E0 Cut 9: 54,0 | — |

Bestaetigt: map3F.bmp ist der Konferenzraum ROOM1140 (Blatt 4 = 3F, re15_map_zones.h Zeile 171), nicht
ROOM1150. Die Annahme in AUFTRAG.md Z. 82 ist falsch, die Spur folgt dem Bild (Nutzer-Wortlaut "an die
Stelle von map3F.bmp") — richtig. Marken-Kopien in `marken/` = md5-gleich mit den Nutzer-Dateien der Wurzel.

### 1.1 Karte an der Marke (alle fuenf Raeume, zwei Ladewege)
(a) CONTINUE-Weg: `integration_r36_kartenitems_bild` selbst gefahren (gruen, 442 s); die Bilder des Laufs
`lauf_f1ceb6f7_*` selbst angesehen (Zoom `N_abn0/t_{3f,2f,1f,b2,b1}_zoom.png`, links Bit gesetzt = keine
Karte, Mitte ohne Bit = Karte, rechts Nutzer-Marke): in allen fuenf Raeumen sitzt das Kartenblatt genau auf
der roten Marke (3F x231..242 y72..79 auf der Tafel; 2F graues Blatt auf der Pinnwand unter dem Foto; 1F
helles Blatt an der Rueckwand; B2 auf der Ziegelwand rechts der Vitrinentuer; B1 als schmales, perspektivisch
verzerrtes Parallelogramm auf der schraegen Ziegelwand, Kanten folgen den Ziegelfugen).
(b) Raumwechsel-Weg (RE15_DEBUG_JUMP = re15_room_apply_pending -> scd_room_reenter, Leon): je Raum debug.log
`[kartenitems] ROOMxxxx: Karte Blatt n (Bit 9,8x) Prop obj 15 bei (...), Zone Slot 40` (1030: `Zone Slot 8
(Muenztelefon -> Slot 39)`) — der Installer laeuft auch auf dem Raumwechsel-Weg.

(c) ECHTER TUER-WEG (Leon), Lauf `runs/d3_1180_11d0`: Spielstand ROOM1180 (-4836,-15916) Blick zur Tuer,
`RE15_SET_FLAG=4:243` (main00 @0x009E6 `Ck(4,243,0)` waehlt sonst die Tuer nach 1160), Skript `A0.1` ->
`[tuer] Sequenz Archiv 2 DOOR1A ... (9 Skripte)` -> `[kartenitems] ROOM11D0: Karte Blatt 0 (Bit 9,91) ...
Zone Slot 40`; im natuerlichen Eintritts-Cut 0 (F10) unterscheidet sich das Bild vom Bit-gesetzt-Bild
(`lauf_*_b1_ohne`) im Fenster 215..265 x 50..110 in genau 169 Pixeln, Huelle (233,65)-(245,93) = die
Nutzer-Marke. Danach gelaufen (L0.15, U0.75, L0.1) und Aktion: `Ereignis 28 -> Port-Programm Karte Blatt
0`, F230 "A police B1 map. / Will you take it? > Yes  No", F320 "You've taken the / police B1 map.",
Kartenschirm B1 komplett (`N_abn0/d3_sheet.png`).
(d) ELZA (echter Weg vom Spielstart): `RE15_PSELECT_AUTO_SWITCH=1`, ROOM1241 -> Tuer -> ROOM1031:
`[pl] Spieler-Familie PL04`, `[kartenitems] ROOM1031: Karte Blatt 2 (Bit 9,89) ... Zone Slot 8 (Muenztelefon
-> Slot 39)` (auch nach dem Szenen-Wiedereintritt erneut, idempotent); `RE15_FORCE_CUT=3` F100: Kartenblatt an
der 1F-Marke (`N_abn0/e5_zoom.png`). Elza-Aufnahme an der exe in 10D0 (Elza-Figur, der Debug-Sprung laedt hier
noch Leons 10D0 — Spur I): `Ereignis 28`, msg 30/31 (`runs/e1_10d1`). Die Aufnahme in einem xxx1-RDT selbst
deckt `unit_r36_kartenitems` D (10D1, re15_game_step) ab; ein Autopilot-Lauf durch Elzas 1031-Lobby blieb am
Tresen haengen (kein Befund gegen die Spur).

### 1.2 Aufnahme mit Text, an der exe (alle fuenf Raeume, echter Aktionsknopf)
Lauf je Raum: `RE15_PSELECT_AUTO=1` (Leon, neues Spiel), `RE15_DEBUG_JUMP=<raum>@250`, `RE15_PLAYER_POS` vor
der Karte, `RE15_FORCE_CUT=<Nutzer-Cut>` (nur 10D0/1030/1200/11D0), Skript `A0.1,W3,A0.1,W3,A0.1,W2,S0.3,W2,Q0.3,W4`
(Aktion, Ja, Weg, START, L1), Framedump-Serie F150..F520/10, `RE15_INV_FB_SHOT` inv-Bild 140.

| Raum | Stand | debug.log | Bild (N_abn0/j*_sheet.png) | Kartenschirm |
|---|---|---|---|---|
| 1140 | (-2050,-9900) r3071 | `Ereignis 28 -> Port-Programm Karte Blatt 4`, msg 30, 31 | F290 "A police 3F map. / Will you take it? Yes No" (Name gruen), F380 "You've taken the / police 3F map." | POLICE STATION 3F: alle 3F-Raeume als Umriss, 1140 gefuellt |
| 10D0 | (2133,22544) r1024 | `... Blatt 3`, msg 30, 31 | dito "police 2F map" | POLICE STATION 2F komplett |
| 1030 | (-1691,-22660) r1024 | `... Blatt 2`, msg 30, 31 | dito "police 1F map" | POLICE STATION 1F komplett |
| 1200 | (-25410,-15682) r2048 | `... Blatt 1`, msg 30, 31 | dito "police B2 map" | POLICE STATION B2 komplett |
| 11D0 | (776,-18819) r225 | `... Blatt 0`, msg 30, 31 | dito "police B1 map" | POLICE STATION B1 komplett |

Texte gegen RE2 geprueft (eigene Dekodierung der RDT-Bytes): ROOM2130.RDT @0x1B32 `fa 02 1d 00 f9 01 "police B1
map" f9 00 01 fc "Will you take it" 1b fb` und @0x1D61 "...taken the fc f9 01 police B1 map f9 00 01 fe";
ROOM20B0.RDT @0x3B92 "A police station map. / Will you take it?" und @0x3DBF "You've taken a police station
map.". Der Port uebernimmt die B1-Form (Etagenname = PORT-WAHL, gekennzeichnet).

### 1.3 Modellwahl "Karte01 oder Karte02"
`extracted_re2_items/uebersicht.html` (die Seite, die der Nutzer zitiert) beschriftet: karte00 = police
station map / police B1 map (RE2 room20B0/2130 — eine ZUSAMMENGEROLLTE Karte), karte01 = sewage disposal
map / sewer map (room3060/4040, flaches Blatt), karte02 = factory map (flaches Blatt). Der Nutzer verlangt
woertlich karte01 oder karte02 (flache Blaetter zum Aufhaengen). Eingebacken ist karte01: MD1 md5 99dae133,
TIM md5 bf0fbcc7 = `extracted_re2_items/modelle/karte01_99dae133.{md1,tim}` (selbst nachgerechnet) — richtig.

### 1.4 Etagenkarte "wie in RE2"
* Ganze Etage nach der Aufnahme: Kartenschirm je Raum (Tabelle 1.2) zeigt nach der Aufnahme alle Raeume der
  Etage als Umriss (unbesucht), den besuchten Raum gefuellt; ohne Aufnahme nur den besuchten Raum
  (`N_abn0/t_weg_karte.png`, Integrationslauf `weg_kein`/`weg_auf`).
* Etagenwahl: Spielstand ROOM1140 mit Bit 88 (2F-Plan) vs. ohne, Kartenschirm + RUNTER (`runs/w_mit88`,
  `runs/w_ohne`, `N_abn0/w_karten.png`): ohne Plan bleibt RUNTER auf "POLICE STATION 3F" (2F nie betreten),
  mit Plan wechselt RUNTER auf "POLICE STATION 2F" mit allen 2F-Raeumen — = RE2 Bank 35 (@0x8006D934).
* Weltmodell nach der Aufnahme weg: Integrationslauf `weg_auf` F270 Tafel ohne Blatt, `weg_kein` mit Blatt
  (`N_abn0/t_weg_raum_zoom.png`).

### 1.5 ROOM1030 Muenztelefon (Nebenwirkung des Slot-Umzugs), an der exe
`RE15_AOT_DUMP=1`, Spielstand 1030 (CONTINUE):
* ohne Bit 89: `[aot]  8 GENERIC Mitte(-1692,-23280) halb(500,500) ev=28` und `[aot] 39 MESSAGE
  Mitte(-2208,-23844) halb(2048,2048) ev=3` (Telefon-Satz unveraendert umgezogen). Spieler (-3500,-22660)
  Blick -z (im Telefon-Rechteck, neben der Karte), Aktion F60: state.log `msg(a=1 fsm=1 id=3)` = "A payphone.
  The line appears to be down..." (`runs/p_tel`).
* mit Bit 89: `[aot]  8 MESSAGE ... ev=3` wie ausgeliefert, keine Karte; Spieler AUF der Kartenstelle
  (-1691,-22660), Aktion F60: `msg(a=1 fsm=1 id=3)` (`runs/p_nach`).
* RDT-Zensus selbst: `scd_dump_room.py` ROOM1030/1031 — Slot 8 nur in main00 `Aot_set` @0x01D2E/@0x01D9C
  (`2c 08 01 31 .. 60 ef dc 9a 00 10 00 10 03 00`), Aot_reset nur Slots 4/6/17, Aot_on nur 18/19 (1031); kein
  RDT der zehn Raeume benutzt Slot >= 20 oder nOmodel > 8 -> Slot 39/40 und obj 15 sind frei.

### 1.6 Karte in den anderen Kamera-Cuts (Tiefe/Ebene)
Projektion der Kartenmitte durch alle VIEW-Matrizen (`probe_r34n_e_dokumente kamera`), dann je Kandidaten-Cut ein
Framedump mit Karte (`RE15_FORCE_CUT`, `runs/cut_*`), gegen den nackten Hintergrund (`probe_bg_dump`):
1030 Cut 0 (Blatt an der Rueckwand zwischen den Aushaengen), 1200 Cut 1 (auf der Ziegelwand rechts der
Vitrine), 11D0 Cut 2 (nah, liegt auf der Wand) — jeweils plausibel auf der Flaeche, nichts schwebt. In 10D0 Cut
4/5/7 und 1030 Cut 7 wird das Blatt nicht gezeichnet (Region-Cull des Cuts, Engine-Verhalten fuer jedes Prop);
11D0 Cut 3 verdeckt Leon die Stelle. (`N_abn0/z_*.png`)

## 2. Urteil je Nutzer-Punkt (gegen den Wortlaut)

| # | Wortlaut (kurz) | Urteil | Beleg |
|---|---|---|---|
| 1 | Karte01/02 extrahieren, an map3F.bmp haengen, 3F freischalten wie RE2, mit Text | erfuellt | 1.0 (map3F = ROOM1140 Cut 5, 8,5 vs >= 113,8), 1.1 a/b, 1.2 (1140: Frage/Ja/Meldung, Kartenschirm 3F komplett), 1.3 (karte01 md5-gleich), 1.4 (ganze Etage + Etagenwahl) |
| 2a | 2F in ROOM10D0 bei map2F.bmp | erfuellt | 1.1 a/b (Pinnwand unter dem Foto), 1.2 (10D0), Elza-Figur 1.1 d |
| 2b | 1F in ROOM1030 bei map1F.bmp | erfuellt | 1.1 a/b/d (Leon + Elza 1031 echter Weg), 1.2 (1030), 1.5 (Telefon bleibt erreichbar) |
| 2c | B2 in ROOM1200 bei mapB2.bmp | erfuellt | 1.1 a/b, 1.2 (1200), 1.6 (Cut 1) |
| 2d | B1 in ROOM11D0 bei mapB1.bmp | erfuellt | 1.1 a/b/c (ECHTER Tuer-Weg 1180 -> 11D0 inkl. Aufnahme), 1.2 (11D0) |

Ort, Aufnahme, Text und Freischaltung sind in allen fuenf Raeumen an der exe gemessen. Das Urteil "erfuellt"
betrifft den Wortlaut; die Darstellung (Licht) ist ein eigener Mangel (Abschnitt 4, M1).

## 3. Gates

### 3.1 RE-Gate — Stichproben selbst disassembliert (richtige Binaerdatei)
| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| RE2 FUN_8006E120 @0x8006E66C Bank-33-Test | ghidra_re2_Leon.txt: `8006e664 lui a0,0x800d` / `8006e668 addiu a0,a0,0x4924` / `8006e66c jal FUN_80077360`, `8006e674 beq v0,zero,LAB_8006e730` | stimmt |
| @0x8006E71C CLUT 498 | `8006e718 jal FUN_80077360` (Bank 32 @0x8006e710) / `8006e71c _li s5,0x1f2`; Variante `8006e72c _li s5,0x1f7` | stimmt |
| @0x8006E744 ohne Plan nicht zeichnen | `8006e738..40` Bank 9 (0x800d490c), `8006e744 beq v0,zero,LAB_8006e768` | stimmt |
| Etagenwahl @0x8006D934/@0x8006D998 | `8006d934 jal FUN_80077360` + `8006d938 _addiu a0,..,0x4d94 =>DAT_800d4908`; `8006d994 addiu a0,a0,0x4908`, `8006d998 jal` | stimmt |
| Banken 33/35 | `800a794c 24 49 0d 80` -> 0x800D4924, `800a7954 08 .. -> 800d4908`, Bank 9 `800a78ec 0c 49 0d 80` | stimmt |
| RE2 room20B0 sub10 @0x037BA..0x0381A | `xxd ROOM20B0.RDT`: `2b00 0000 00ff 0200 0600 4400 210b 1f00 3604 0601 ...` `2221 0201`..`2223 0401` `2e04 08 00` `3200 204e 204e 204e` `0200 2b00 1100 00ff 0700 1200 3604 0501 ...` `0100` | stimmt |
| RE1.5 ROOM5030 sub02 @0x010AC / sub00 @0x0108A | `xxd` (md5 = info/Re1.5): `2b00 ffff 0200 0600 1e00 210c 1f00 4605 .. 090a 0100 2203 7301 2b01 ffff 0200 0800 0100`; `2c05 03b1 .. ff00 1802` | stimmt |
| ROOM1030 Telefon @0x01D2E / 1031 @0x01D9C | `2c 08 01 31 00 00 60 ef dc 9a 00 10 00 10 03 00 ff ff 00 00` | stimmt |
| Obj_model_set-Flags @0x80040998 | ghidra1_V2.txt `80040990 lhu v0,0x6(a2)` / `80040998 ori v0,v0,0x1` / `800409a4 sw v0,0x0(a1)` | stimmt |
| "kleinster Slot gewinnt" (FUN_80042bac) | RE_15_Quellcode_V2/FUN_80042bac.c: Zeigertabelle DAT_800ac9b0 aufsteigend, Aktions-Treffer `return (*handler)(..)` beim ersten Treffer; Vorwaertspunkt 0x26c = 620 | stimmt |

Wortsuche im Code-Diff (deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME): 0 Treffer; kein neues
getenv im Engine-/Plattform-Code. Konstanten: Bits/Ereignis/IDs = VERTRAG, Programmbytes je Zeile mit @0x,
Lage/Massstab/Rechteck/Slots als PORT-WAHL bzw. Zensus gekennzeichnet — bis auf das Licht (M1).

### 3.2 Vertrag
* Bank-9-Bits 87..91 (zugeteilt N), Ereignis 28 (N), Nachrichten 30/31 (VERTRAG 1.2 erlaubt Reserve 30/31;
  RDT-Zahl selbst gezaehlt: 1140 6, 1141 15, 10D0 28, 10D1 12, 1030 18, 1031 24, 1200/1201 0, 11D0/11D1 5 —
  30/31 ueberall frei; keine andere Port-Quelle setzt 30/31 in diesen Raeumen).
* Ereignis 28: kein frueherer Haken der scd_event_fire-Kette (rolltor/adaruf/cut10f0/irons_tod/fenster1120)
  liefert in diesen Raeumen ein Programm fuer 28.
* Pfade (`git diff 78a4a748 --name-only`): nur engine/src, include, platform/pc/main.c, tests/unit/probes/
  r36_kartenitems.cmake (GLOB), eigene Tests, tools/r36_kartenitems, Dossier. Kein release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists.
* Gemeinsame Dateien: scd_room_setup.c 1 Zeile + include (Blockende, "Runde 36 Spur N"), scd_vm.c 1 Zeile +
  include, re15_map_owned.c 2 Zeilen, map_hint_common.c 1 Zeile + include, main.c: include, CONTINUE-Installer 1
  Zeile, Licht-Haken 3 Zeilen, Prop-Lader-Block 6 Zeilen (r35-Muster; knapp ueber "1-5 Zeilen").
* Tests: `unit_r36_kartenitems` (201 Pruefungen, selbst gelaufen: 201 PASS, 0 FAIL) und
  `integration_r36_kartenitems_bild` (echte exe) — `ctest -R r36_kartenitems`: "100% tests passed, 0 tests
  failed out of 2" (442 s).
* Nebenwirkung Etagenwahl: re15_map_blatt_waehlbar gilt jetzt auch fuer den Laborplan (Flag 3,115). RE1.5 hat
  ueberhaupt kein Blaettern (menu_common.c: PORT-ERGAENZUNG nach RE2); RE2 setzt auch beim Laborplan Bank 35
  (lageplaene.csv room6120) — konsistent.

### 3.3 Regressionsgruppen (Fortsetzung der Abnahme nach dem Wochenlimit, 2026-10-11 ~11:05-11:53)
Stand neu gebaut (`local_build.sh configure` + `build`: "LOCAL-BUILD-OK", ninja "no work to do"; exe
04:37 md5 6f16291b = Messkopie; letzter Code-Commit 3c0737c0 05:22 aenderte nur Tests/Dossier, danach nur
Berichte). Gezielter Lauf (KEINE volle Suite, VERTRAG 1.5), Regex in `N_abn0b/regr_regex.txt`:
`karte|map|besitz|hinweis|r34n_e|r35_inhalt|r34n_a|r34n_d|fenster|cut10f0|r35_cut|1030|1140|10d0|1200|11d0|
item_stack|r36_kartenitems|irons_tisch|aot|msg_|action_msg|prop_|boot_bg|dark_start|relatch|save_counter|
weste_load|savepoint|continue` = alle Module/Raeume, die der Diff beruehrt (Karte/Besitz/Hinweis,
Raumstart-Installer scd_room_setup, scd_event_fire-Kette, Prop-Lader/Licht + Boot-/CONTINUE-Weg in main.c,
AOT/Nachrichten, die fuenf Raeume) + die flatterigen Fenster-Haken des Boot-/CONTINUE-Weges.
`ctest --test-dir re15_port/build -R "<Regex>" --timeout 600 --output-on-failure`
-> **"100% tests passed, 0 tests failed out of 134"**, Total Test time 1694,04 s, CTEST_EXIT=0
(`N_abn0b/ctest_regr.log`). Darin 16 exe-Haken, alle im ERSTEN Lauf gruen, obwohl die Maschine zeitgleich
die Abnahmen anderer Spuren fuhr (tasklist: 4x re15_pc_nb6f, 2x re15_pc_h36mess, re15_pc_g36abn0):
integration_weste_load_pin 6,8 s, r30_irons_tisch_laden/bild/licht 33/38/38 s, r34n_e_dokumente_bild 95,8 s,
r35_cut10f0 240,7 s, r35_cut1150 507,2 s (Timeout 900), r35_fenster 31,0 s, r35_karte 74,2 s,
r35_karte_fahrstuhl 175,4 s, **r36_kartenitems_bild 359,3 s**, map_uebergang 1,5 s, map_raum_live 12,7 s,
boot_bg_pin 3,5 s, dark_start_pin 9,0 s, relatch_pin 23,3 s, save_counter_pin 16,5 s; unit_r36_kartenitems
0,09 s (201 Pruefungen). Kein Nachfahren noetig, keine Regression. (Die zuvor abgebrochene Abnahme war bei
99/120 derselben Gruppe ebenfalls ohne Rot stehengeblieben, `N_abn0/ctest_regr.log`.)

## 4. Maengel

### M1 — Licht der Karten-Props: dritte, erfundene Lichtquelle statt eines Original-Weges (RE-Gate)
Betrifft: `platform/pc/main.c` (Haken im Prop-Zeichner, 3 Zeilen: `re15_kartenitems_r36_licht` ueberschreibt
`pr0..pb2`), `kartenitems_r36.c` (`re15_kartenitems_r36_licht`), `gen/kartenitems_r36.inc` (Spalte `licht`:
1140 (156,155,153), 10D0 (181,186,180), 1030 (47,70,104), 1200 (85,85,75), 11D0 (74,75,77)),
`tools/r36_kartenitems/karten_export.py` (`BG_REF`, "hellste 15 %").

**Was das Original tut (selbst gelesen):** FUN_8002c18c (RE_15_Quellcode_V2/FUN_8002c18c.c:26-56, Katalog
"Prop render dispatch") zeichnet JEDES Objekt des Pools so: `FUN_80053fc0(&DAT_800b3ff4+obj)` = Lichtsatz
des aktiven Cuts am Objektort (DAT_800b0fe4, Katalog Z. 219: Kamera-Cut == Licht-Cut), dann
`if ((pool+0x0c & 0x10) == 0) FUN_800254a0(...)` = NCCT-beleuchtet, `else FUN_80022f0c(...)` = FLACH ohne
NCCT, Farbe = Konstante obj+0x70 (Katalog Z. 215). Pool+0x0c ist das Flag-Wort pc[8..9] des Obj_model_set
(Katalog 0x80040914). Zwei Wege, beide mit Adresse. Der Port zeichnet Props ueber den NCCT-Weg
(`re15_light_setup_actor`/`re15_light_shade_vertex`, byte-true zu FUN_80053fc0/FUN_800542dc, main.c
"Lichtsatz des AKTIVEN Cuts (byte-true, FUN_8002c18c)"); den Flag-0x10-Weg fuehrt er fuer Props nicht
(main.c: kein Zweig auf FUN_80022f0c im Prop-Block).

**Was die Spur tut:** fuer obj 15 in den fuenf Raeumen ersetzt der Haken die NCCT-Vertexfarbe durch eine
Konstante je Raum = 128 * Ref / Median(Kartentextur), Ref = Mittel der hellsten 15 % eines Fensters im
BSS-Hintergrundbild des Nutzer-Cuts (karten_export.py Z. 28-33, 86-91). Kein Original-Gegenstueck, kein @0x;
"15 %" und die Fenstergroesse ("Huelle um je eine Huellenbreite erweitert") sind Zahlen nach Gefuehl
(CLAUDE.md Guess-Tell "runde Zahl nach Gefuehl"); gekennzeichnet als PORT-WAHL, ist aber weder Position noch
Text noch Choreografie (VERTRAG 1.6), sondern eine Render-Konstante mit vorhandenem Original-Mechanismus.

**Messung byte-true (probe_r34n_e_dokumente `licht` = re15_light_setup_actor + shade_vertex je Achsnormale;
Lichtblock der RDT selbst geparst, RDT+0x2C, 40 B/Cut):**
* ROOM1140 (Lichtblock @0x38C): alle 10 Cuts Lichter AUS (Farbe (0,0,0), Helligkeit 0, Typ (0,1,0)),
  Ambient (83,78,78) -> Vertexfarbe (41,39,39) fuer JEDE Normale — an der Karte (-2050,-2831,-8710) wie an
  Leons Stand (-2050,0,-9900) wie in der Raummitte (-2050,-1000,-5000). Jede Figur im Raum wird mit 41/128
  gezeichnet. Haken: (156,155,153)/128 = 3,8x.
* ROOM10D0 an der Karte (2133,-1928,21624), Normale +Z: (41,39,39) in Cut 0-10/12/13, Cut 11 (31,31,31).
  Haken (181,186,180) = 4,4x.
* ROOM1030 an der Karte (-1691,-2769,-23580), Normale +Z (Lichtblock @0x4DC: Punktlichter L0/L1 in
  Reichweite, cutabhaengig): Cut 0/2/4/6 (89,89,94), Cut 3 = Nutzer-Cut (54,54,59), Cut 7 (38,38,43),
  Cut 9-12 (44,44,49). Das Original-Licht wechselt mit dem Cut um den Faktor 2,3; der Haken bleibt in
  jedem Cut (47,70,104) und traegt einen Blaustich B:R = 2,2 gegen 1,09 des Raumlichts (Ambient (76,76,86)).
* ROOM1200 an der Karte (-26330,-2572,-15682), Normale +X: Cut 0 (40,40,40), Cut 1-3 (70,70,70); Haken
  (85,85,75).
* ROOM11D0 an der Karte (1642,-2199,-19130): (36,41,41) in allen 14 Cuts; Haken (74,75,77) = 2,1x.

**Messung am Bild (RE15_WINDOW_SCALE=1, Messkopie, CONTINUE-Spielstand aus probe_r35_inhalt_karte;
`N_abn0b/m1_mess.py` / `figur_mess.py`, Karte = Diff mit/ohne Bit im Marken-Fenster +-3 px):**
* Nutzer-Cuts (Integrationslauf lauf_f1ceb6f7): Karte MIT Mittel RGB 3F (106,107,97) auf Wand (90,83,73);
  2F (101,104,95) auf (70,66,45); 1F (38,57,78) auf (21,32,43); B2 (68,70,56) auf (45,40,30); B1 (61,64,60)
  auf (40,40,36). Im +-3-px-Fenster ist die Karte in 2F (Summe 300 gegen Maximum 210), 1F (173 gegen 147)
  und B1 (185 gegen 182) heller als jeder Hintergrundpixel neben ihr.
* ROOM1140 Cut 5, Leon bei (-1000,-9700) rechts neben der Tafel (Lauf `N_abn0b/runs/leon5_l1`, Karte
  unverdeckt: 0 Diff-Pixel im Marken-Fenster): Leon (860 Pixel, Huelle x256..285 y74..133) Mittel (19,22,27)
  Summe 68, hellste 10 % (52,42,38) Summe 132, hellstes 1 % 184, Maximum 210 — die Karte daneben Mittel
  Summe 310, Median 320: heller als Leons hellster Pixel, 2,4x heller als seine hellsten 10 %
  (`N_abn0b/leon5_l1_x3.png`). Mit dem NCCT-Weg laege die Karte bei 104*41/128 = (33,33,31) auf der Tafel
  (90,83,73): so dunkel wie Leon — das ist das Original-Verhalten eines Objekts an dieser Stelle (der Befund,
  den die Spur "fast schwarz" nannte, Dossier 3.3). Der Fix ERKLAERT den Befund nicht: er beantwortet nicht,
  warum das Licht dort so ist (Antwort: die RDT-Lichtsaetze von 1140/10D0/11D0 sind reine Ambient-Saetze),
  sondern ueberdeckt ihn mit einer Konstante.
* ROOM1030 Cut 3 (Lauf j3_1030 F150 gegen lauf_f1ceb6f7_1f_mit): Leon hellste 10 % (69,67,75) B:R 1,10;
  Karte (38,57,78) B:R 2,05 — die Karte ist blau, Leon daneben nicht.

**Forderung (nachpruefbar):** die Vertexfarbe der Karte kommt aus einem der beiden Original-Wege — NCCT
(dann ist die Karte so hell wie jede Figur am selben Ort; Test: Karte-Mittel/Leon-Mittel im selben Cut
<= 1,2) ODER der Flag-0x10-Weg FUN_80022f0c mit belegtem obj+0x70 (dann Konstante MIT Adresse, cut-
unabhaengig per Original) — und der Haken in main.c entfaellt bzw. traegt die Adresse. Welcher Weg: am
RE2-Gegenstueck belegen (room20B0 obj 8 Obj_model_set-Attributwort, Bit 0x10?) — Abschnitt 4.1.

#### 4.1 RE2 zeichnet seine Wandplaene FLACH (selbst aus den RDT-Bytes und dem RE2-Decompile gelesen)
RE2 Obj_model_set (0x2D, 38 Bytes: op, id, 18 Woerter) der beiden Polizei-Lageplaene:
* room20B0 obj 8 @0x03158 (lageplaene.csv sub00@0x03158): `2d 08 00 00 00 00 00 00 00 00 0a 00 10 00 20 4e
  20 4e 20 4e 00 00 c0 f8 00 00 ..` -> Woerter w4 = 0x000A, **w5 = 0x0010**, Lage (20000,20000,20000) (geparkt,
  wie lageplaene.csv), dir_y = -1856.
* room2130 obj 0 @0x00FDC (B1-Plan): `2d 00 00 00 00 00 00 00 00 00 0a 00 10 00 fc bd 50 fb 60 d7 00 00 80 fc ..`
  -> w4 = 0x000A, **w5 = 0x0010**, Lage (-16900,-1200,-10400) = lageplaene.csv.
Dieselbe Wortfolge wie RE1.5 pc[6..7] (Enable-Wort, Port RE15_R36K_PROP_FLAGS 0x000A|1) und pc[8..9]
(Flag-Wort -> Pool+0x0c, Katalog 0x80040914). RE2-Dispatcher FUN_800363f0 (RE2_Quellcode_V2/FUN_800363f0.c:
41-54, Gegenstueck zu RE1.5 FUN_8002c18c): `if ((puVar4[4] & 0x10) == 0) { FUN_80027bec/FUN_80027dbc
(beleuchtet) } else { FUN_8002d0e8/FUN_8002d244 (flach) }`. Das heisst: beide RE2-Wandplaene tragen das
Flach-Bit 0x10 und werden von RE2 OHNE Raumlicht gezeichnet — das ist der Original-Mechanismus fuer genau
diesen Gegenstand ("Beta -> Retail: RE2 ist das Ziel"), mit Adresse. Die Spur hat ihn nicht gesucht
(Dossier 6: "Kein Original-Gegenstueck — RE1.5 hat an diesen Stellen kein Objekt") und stattdessen eine
Konstante aus dem Hintergrundbild erfunden. Der byte-true Bau: Karten-Prop mit Flag-Wort 0x0010 ueber den
Flach-Weg zeichnen (Port: der Prop-Zeichner in main.c fuehrt diesen Zweig noch nicht — FUN_80022f0c:
Farbe = obj+0x70, siehe 4.2), nicht ueber eine Vertexfarbe aus "hellste 15 %".

#### 4.2 Die Farbe des Flach-Weges hat eine Adresse
RE1.5 FUN_8002b898 (Objekt-Init, RE_15_Quellcode_V2/FUN_8002b898.c:16, Katalog Z. 216):
`*(undefined4 *)(&DAT_800b4008 + iVar1) = 0x808080;` -> obj+0x70 = 0x808080 = NEUTRAL (128 je Kanal, Textur
unveraendert; FUN_80022f0c laedt es per gte_ldrgb, kein NCCT). Byte-true ist die Karte also im Flach-Weg
GENAU so hell wie ihre Textur (Median (104,104,96)) — in jedem Cut, ohne Raumlicht, ohne Blaustich — bzw. im
NCCT-Weg so hell wie jede Figur am Ort. RE2 identisch: FUN_80035710 (RE2_Quellcode_V2/FUN_80035710.c:16, Objekt-Init, Pool-Basis DAT_800d0324,
Stride 0x1F8): `(&DAT_800d03a0)[param_1 * 0x7e] = 0x808080;` = RE2-Pool +0x7C neutral; der Flach-Zeichner
FUN_8002d0e8 ist laut Katalog Z. 233 "RE2 UNLIT direct prim-emit loop (no color)". Beides hat eine Adresse;
(156,155,153)/(181,186,180)/(47,70,104)/(85,85,75)/(74,75,77) haben keine. Zuordnung RE2-Wort w5 -> RE2-Pool+0x10 (puVar4[4]) ist nicht einzeln
nachgezogen (RE2-Handler nicht aufgesucht); die RE1.5-Zuordnung pc[8..9] -> Pool+0x0c ist katalogisiert
(0x80040914), der RE1.5-Dispatcher liest genau dieses Wort (FUN_8002c18c.c:41).
Hinweis fuer den Nachbau: der Port speichert das Flag-Wort pc[8..9] im Prop nicht (re15_scd.h: `flags` =
Pool+0x00 Enable-Wort) und der Prop-Zeichner in main.c hat keinen Flach-Zweig — beides ist nachzuziehen
(Prop-Feld fuer Pool+0x0c, Zweig `& 0x10` -> flache Farbe 0x808080 statt re15_light_shade_vertex), dann
entfaellt der Licht-Haken samt `licht`-Spalte und BG_REF. Ein Test der Spur fuer die Licht-Konstante
existiert nicht (test_r36_kartenitems.c: 0 Treffer "licht"); der Nachbau braucht einen (Karte-Mittel im
Framedump = Texturfarbe +-Rundung, in zwei verschiedenen Cuts gleich).

Keine weiteren Maengel: Punkte 1-2 sind gegen den Wortlaut erfuellt (Abschnitt 2), Vertrag (3.2) und die
uebrigen RE-Gate-Stichproben (3.1) halten; Regressionsgruppen Abschnitt 3.3a.

## 5. Ergebnis
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1 Karte01/02 an map3F.bmp, 3F freischalten wie RE2, mit Text | erfuellt | 1.0-1.4: ROOM1140 Cut 5 Tafel, karte01 md5-gleich, Frage/Ja/Meldung, Blatt 3F komplett + waehlbar (RE2 Bank 33/35) |
| 2a 2F ROOM10D0 (map2F.bmp) | erfuellt | 1.1/1.2 |
| 2b 1F ROOM1030 (map1F.bmp) | erfuellt | 1.1/1.2/1.5 (Leon + Elza 1031, Telefon bleibt) |
| 2c B2 ROOM1200 (mapB2.bmp) | erfuellt | 1.1/1.2/1.6 |
| 2d B1 ROOM11D0 (mapB1.bmp) | erfuellt | 1.1 c (echter Tuer-Weg 1180 -> 11D0), 1.2 |

Gates: Vertrag haelt (3.2). RE-Gate haelt NICHT vollstaendig: M1 — die Licht-Konstanten der fuenf Karten-Props
(main.c-Haken, `licht`-Spalte, BG_REF "hellste 15 %") ersetzen einen vorhandenen Original-Mechanismus
(FUN_8002c18c: NCCT oder Flag 0x10 -> FUN_80022f0c mit 0x808080 aus FUN_8002b898; RE2 zeichnet seine
Wandplaene mit Attribut 0x0010 flach, FUN_800363f0) durch Zahlen nach Gefuehl ohne Adresse; gemessen: Karte
in 1140 Cut 5 heller als Leons hellster Pixel daneben (Median-Summe 320 gegen Maximum 210), in 1030 blau
(B:R 2,05) neben neutralem Leon (1,10), cut-unabhaengig trotz cutabhaengigem Raumlicht (38..89).

**Abnahme: NICHT bestanden** (bestanden = nur wenn alle Punkte erfuellt UND alle Gates halten). Nachbesserung:
M1 — Karten-Prop ueber den Flach-Weg (Flag-Wort 0x0010 -> 0x808080) oder byte-true NCCT zeichnen, Haken
und BG_REF entfernen, Test fuer die Kartenhelligkeit (zwei Cuts) ergaenzen. Alles andere kann unveraendert
bleiben; die drei Entscheidungsfragen des Dossiers (3F = ROOM1140, Groesse, Telefon-Ueberlapp) gehen an den
Nutzer.

