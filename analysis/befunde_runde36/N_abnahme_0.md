# Runde 36 Spur N "kartenitems" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r36_kartenitems`, Zweig `r36/kartenitems`, HEAD 3c0737c0, Basis 78a4a748.
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

(in Arbeit: Regressionsgruppen)
