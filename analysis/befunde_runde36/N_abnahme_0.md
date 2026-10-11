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

(in Arbeit)
