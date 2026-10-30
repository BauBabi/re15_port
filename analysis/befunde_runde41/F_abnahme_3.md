# Runde 41 — Spur F "gatorszene": Abnahme 3 (unabhaengig)

Stand: Zweig `r41/gatorszene`, Code-Stand `da156ce6` (Basis r41/integration `9f10bdb0`, Abnahme 2 auf `d5321663`); waehrend der
Abnahme legte der Orchestrator `0a1625af` (nur AUFTRAG.md: Nachtraege 26-29, Entscheide G-E1/G-E2) in den Zweig. Baum
`.claude/worktrees/r41_gatorszene`. Pruefer: Abnahme 3 (2026-10-25). Ich glaube dem Bau-Agenten nichts, was ich nicht selbst am
gebauten Stand gemessen habe.

## 0. Bau und Lektuere

- Gelesen: AUFTRAG.md (bis Nachtrag 29 und Entscheide G-E1/G-E2, 2026-10-25 12:08), VERTRAG.md, Dossier F_gatorszene.md (ganz,
  Schwerpunkt "Nachbesserung 3"), F_abnahme_2.md, `git log master..HEAD` (seit Abnahme 2: 6c8956ec .. da156ce6, dazu 0a1625af),
  `git diff d5321663..da156ce6` (Nachbesserung 3: Code nur `engine/src/gatorszene_r41.c` +80/-12, dazu test_r41_gatorszene.c (+183),
  test_r41_gatorszene.cmake (+168), probes/r41_gatorszene.cmake, Dossier, 2 Belegbilder), `git diff master...HEAD` (F gesamt seit der
  Merge-Basis 9f10bdb0: 54 Dateien). `git diff master` (zwei Punkte) zeigt zusaetzlich Loeschungen von Dateien, die master NACH der
  Basis bekam (kleinkram_b, kollision, stage3szenen_a, spinnenanim, synchro/.../main46.wav ...) - keine Aenderungen von F.
- Code gelesen: `key_rueckschnitt_pruefen`, `key_tod_offen`, `key_tod_setzen`, `key_tod_pruefen` (neu/umgebaut, gatorszene_r41.c
  :1304-1369), dazu die benutzte Spur-B-API in gator_re2_r41.c (`re15_gr2_r1_tick` P1-Ende `gr2_kam_zurueck` :1726, P3 `gr2_kam_frei`
  :1738, `re15_gr2_r1_laeuft` :1743, `re15_gr2_kamera_frei` -> `gr2_kam_frei` :846-866, `re15_gr2_tot` :2540) und die Aufrufreihenfolge
  im Boss-Takt (enemy_ai_boss_gator.c :1243-1256: `gb_takt` (darin R1 :2643) -> `re15_gr2_nachtakt` -> `re15_gatorszene_r41_nachtakt`).
  Spur B's Modul ist im Diff unveraendert (F ruft nur die oeffentliche API `re15_gr2_kamera_frei`).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.` /
  `=== LOCAL-BUILD-OK (build)`; `git status --short` leer; gatorszene_r41.c 10:36:58 < .obj 10:37:19 < re15_pc.exe 10:37:58, die exe traegt
  die neue Logzeile "Schluessel-Schnitt statt R1-Rueckschnitt" -> die exe ist der Code-Stand da156ce6 (0a1625af aendert keinen Code).

Messordner (scratchpad, nicht im Repo):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\F41a3`
(`runs/<lauf>/debug.log|state.log|gator_boss.log|fd_*.ppm`, Helfer `tools/lauf.sh` = eigene exe-Kopie `re15_pc_f41a3_<lauf>.exe`,
`timeout`, danach geloescht; eigener Bild-Auswerter `tools/kam.py` = mittlere |Delta| je Bild zu drei Referenzbildern + Wechsel-Pixel
zum Vorbild, unabhaengig vom Auswerter `kamfolge` des Bau-Agenten; `tools/diffserie.py` = Bild fuer Bild gegen einen Zwilling, jede
Abweichung zaehlt). Alle Laeufe mit `RE15_WINDOW_SCALE=1`, Bilder nur per `RE15_FRAMEDUMP`.

## 1. Gezielte Tests

Regex = die Regex des Bau-Agenten (= Abnahme 2, `ctest -N` 216) plus `integration_r36_kartenitems_bild$` (Kartenbild mit den
2090/2091-Taken-Bits; grep der Tests nach 2090/2091/20A0/GB_TEST/GATOR_KANISTER/gatorszene/re15_gr2_ gegen `ctest -N`) = 217, Datei
`F41a3/regex2.txt`. Befehl mit dem PATH von local_build.sh (msys64 zuerst, `F41a3/tools/ctest_lauf.sh`):
`ctest --test-dir "<baum>/re15_port/build" -R "$(cat F41a3/regex2.txt)" -j4 --timeout 1200 --output-on-failure`
Ergebnis (11:44-12:10, parallel zu eigenen exe-Laeufen): **`100% tests passed, 0 tests failed out of 217`** (`Total Test time (real) =
1589.87 sec`, Log `F41a3/ctest_all.log`). Darin u. a. `integration_r41_gatorszene_tod_explosion` 33.58 s / `_tod_explosion_elza`
49.67 s, `unit_r41_gatorszene_tod_explosion` 0.38 s, `_leon/_elza` 57.82/77.30 s, `_tod/_tod_elza` 70.03/105.05 s, `_tod_r1/_tod_r1_elza`
45.15/84.11 s, `integration_r41_re2gator` 333.63 s, `integration_r37_itemband` 838.32 s, `integration_r38_wasser` 191.86 s,
`integration_r41_wasser20b0_npc` 223.26 s, `integration_r38_rolltor20a0` 300.94 s, `integration_r36_tueren` 52.62 s,
`unit_r30_tuer_verschlossen` 38.84 s, `unit_r38_spinnen`, `unit_r36_tueren_schott`, `unit_r36_kartenbits`, `integration_r36_kartenitems_bild`
514.79 s. Die Behauptung des Bau-Agenten (216/216) ist bestaetigt. Kein roter Fenster-Haken, nichts nachzufahren.

Messen die neuen Riegel? Selbst geprueft:
- Auswerter `test_r41_gatorszene kamfolge` auf den ALTEN Bildern der Abnahme 2 (Lauf F41a2/runs/KW, Stand 5c99bba8, dieselben Parameter
  wie der Riegel: 165 206 178 241 303): `E 32, K 8, X 2, 2 Wechsel, Folge EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEXXKKKKKKKK -> FEHLER`; auf meinen
  neuen Bildern (F41a3/runs/KW): `E 32, K 10, X 0, 1 Wechsel ... -> OK`. Der exe-Riegel verlangt ausserdem die Ersatz-Zeile und state.log
  cam=K ab F<r> - am alten Stand beides nicht vorhanden.
- `test_r41_gatorszene tod_explosion` direkt: ROOM2090 und ROOM2091 `angezeigter Cut je Bild ab dem Treffer: 1 x 36, 2 x 90, 0 x 2`,
  Gegenprobe (9,111) vorab `1 x 36, 0 x 23` (B's Rueckschnitt unberuehrt), `tod_explosion: 0 Fehler`.

## 2. Punkt 1a — Ankunftsszene (Leiter 2080 -> 2090; Wortlaut + Nachtraege 6/17/20/21/29)

Nachbesserung 3 beruehrt die Ankunft nur ueber das Zuruecksetzen von `s_r1_cut`/`s_r1_lief` im Installer und den Aufruf
`key_rueckschnitt_pruefen` im Nachtakt (waehrend der Szene ist `re15_gr2_r1_laeuft` = 0 -> sofortige Rueckkehr). Gemessen wie Abnahme 2:

- L1 Leon: `tools/lauf.sh L1 600 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080
  RE15_EXIT_AT=700#2090 RE15_FRAMEDUMP=300-700/2:fd_ RE15_AUDIO_CAP_SYNC=audio.raw` -> rc=0, 51 s. debug.log:
  `[aot] DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,hh=500) target_cut=0 spawn=(-8200,0,-25550)` (Leiter ROOM2080 Slot 1),
  `ROOM2090 Slot 1 (Tor) gesperrt: Ereignis 19, Bit (9,113) = 0`, Folge `A F40` -> `P 1` -> `T F98 wert=0/1` -> `P 2 ROOM2080 F142` +
  `M ROOM2080 F142 wert=8/2` -> `P 3 F156` -> `S F53` -> Buehne `Leiche (-4595,-24305) ... S (-4624,-13590) Gier 1024` -> `G 1/7` ->
  `G 2 F176` -> `K 13 F189` -> `G 3 F190` -> `G 4 F310` -> `V F369` -> `G 5` -> `G 6 F376` -> `C ROOM2090 F405 wert=0/1` +
  `Kamera-Uebergabe: Cut 0 (Eintritts-Cut), Spielfigur im Bild (Fuss-Bildpunkt 88,126)` -> `G 7 F405` -> `B 1 F407` + `[bgm] stage=1
  room=09 entry=FF3C -> MAIN3C` + `[bgm] loaded: 1 VAGs, SEQ 100B` (= RE2 MAIN1A). Zeile fuer Zeile und mit denselben Bildnummern wie
  Abnahme 2. state.log letzter Abschnitt: F405-F479 75/75 `cam=0`. Knall (audio.raw, Takt = 1470 Stereo-Frames): bis Takt 572
  6289..7640, **Takt 573 18920**, 575 19687, 580 19005, abklingend bis 593 8543 (= Abnahme 2).
- Bilder: L1a (`RE15_EXIT_AT=135#2090 RE15_FRAMEDUMP=40-135/1`) und L1b (`RE15_EXIT_AT=150#2080 RE15_FRAMEDUMP=10-150/10`), Bogen
  `F41a3/L1_ankunft.png`: Leiche in Cut 1 mit dem Ring ueber dem Ruecken (fd 45/75/105/134), 2080 Cut 2 acht Zombies auf dem oberen
  Laufgang (fd 20/60/100/140, wie F_belege/f2_2080_oben.png); `F41a3/L1_kampfstart.png` (fd 310-520): Fressen/Schuetteln vor Leon,
  Kampfbeginn Cut 0 mit Leon an der Leiter, Biss.
- E1 Elza (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, 2081/2091, `RE15_NOAUDIO=1`): `[pl]
  Spieler-Familie PL04`, dieselbe Folge mit denselben Bildnummern (`C ROOM2091 F405 wert=0/1`, Kamera-Uebergabe Cut 0, Spielfigur im
  Bild), F405-F479 75/75 `cam=0`, `EXIT_AT: Bild 700 in Raum 2091`.
- **Nachtrag 29** (Wortlaut 12:08: "Die Zombie Leiche mit dem Schluessel ist zu nah an der Leiter. Das Krokodil frisst ihn, und ist damit
  direkt beim Start des Kampfes quasi unmittelbar an der Leiter. Man hat keine Chance direkt am Anfang vom Kampf den Aligator
  auszuweichen. Die Zombie Leiche inklusive der Kette - Schluessel - Aligator Fressen etc. muss weiter weg sein, damit man am Start des
  Kampfes eine Chance hat.") ist am Code-Stand **nicht umgesetzt** - er kam nach da156ce6; `GSZ_LEICHE_X/Z` = (-5500,-23700) und die
  Fressstelle (-4594,-24306) sind unveraendert (rund 3800 von der Leiter-Ankunft (-8200,-25550), = Messung des Orchestrators).
  Ist-Stand an der exe (state.log, Kampfbeginn = `Folge C ... F405`):

  | Lauf | Eingabe ab Kampfbeginn | Leon F405 | Gator-Wurzel F405 (Abstand) | Angriff (Clip 4 Bild 0) | erster Treffer (HP 100 -> 50) | danach |
  |---|---|---|---|---|---|---|
  | L1 Leon | stehend | (-8392,-25550) | (-4254,-20984), 6162 | F459 (+54 Bilder) | **F466 = 61 Bilder (2,0 s)** | HP 0 F573, -50 F680 |
  | E1 Elza | stehend | (-8392,-25550) | (-4254,-20984), 6162 | F459 | **F466 = 61 Bilder** | HP 0 F573 |
  | LW Leon | `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W13.5,UX4` = Rennen nach Osten ab F406 (`Tick 405 -> F406 Tasten 0x4010`) | (-8392,-25550) | (-4254,-20984) | F431 (+26) | **F438 = 33 Bilder**, im Vorbeilaufen bei (-2144,-26532) | HP 0 F545, -50 F652 |
  | EW Elza | wie LW (2081/2091) | (-8392,-25550) | (-4254,-20984) | F431 | **F438 = 33 Bilder** | HP 0 F545 |

  Bildbogen `F41a3/LW_ausweichen.png` (fd 404-448): beim Kampfbeginn liegt der Alligator mit dem Kopf neben der Leiter; Leon rennt los
  (der einzige freie Weg ist Osten, Leon blickt mit Gier 0 dorthin; Westen/Sueden Wand) und wird im Vorbeilaufen gebissen. Der Befund
  des Nutzers ist damit an der exe bestaetigt: Ausweichen gelingt weder stehend noch sofort weglaufend (Leon und Elza). Der Sollwert
  (RE2-Intro-Ende ROOM40A0: Abstand und Bilder bis zum ersten Angriff, Dash-/Biss-Schwellen mit @0x) ist nach dem Auftrag des
  Orchestrators von Spur F zu belegen; hier gemessen ist nur der Ist-Stand.
- **Urteil 1a: teilweise.** Die Szenenfolge (Leiche mit blinkendem Schluessel -> Knall -> 2080 oben -> erneut die Leiche -> Alligator aus
  dem Wasser frisst Leiche samt Schluessel -> Schrei/Schuetteln zu Leon -> RE2-Kampfmusik) ist unveraendert erfuellt (Leon + Elza), aber
  Nachtrag 29 (Leiche samt Kette weiter weg von der Leiter, Ausweichen am Kampfbeginn moeglich) ist offen -> Mangel M1.

## 3. Punkt 1b — Schluessel nach dem Tod (Mangel M1 der Abnahme 2: Zwischenbild beim Explosionsschnitt)

Rezept KW/KWE der Abnahme 2 (wortgleich): `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
RE15_DEBUG_JUMP=2090@250 RE15_STATE_LOG=state.log RE15_R39_MESS_SPIELER_HP=100 RE15_GB_TEST=5 RE15_GATOR_KANISTER=4430,-26160
RE15_GIVE=3:15,21:50 RE15_EQUIP=3 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_SET_FLAG=9:110
RE15_INPUT_SCRIPT=W5,(M0.3,MA0.1 zwoelfmal ausgeschrieben) RE15_EXIT_AT=420#2090 RE15_FRAMEDUMP=150-420/1:fd_` (Leon:
`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`; Elza: `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`,
2091). Zwilling ohne Schluessel: `RE15_SET_FLAG=9:110,9:112`. Der Tod ist in diesem Baum nur ueber Spur B's Testhaken erreichbar
(`RE15_GATOR_KANISTER` + `RE15_GB_TEST=5`); der echte Weg laeuft ueber die Kanister-Szene der Spur G (Integration).

### KW (Leon)
- rc=0, 30 s. debug.log `[pl] Spieler-Familie PL00`.
- gator_boss.log: `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F160 clip=9 bild=0`, `KAMERA R1 zurueck -> Cut 0 F196 clip=9 bild=36`,
  `KAMERA R1 frei (Zonenkamera an) Cut 0 F196 clip=-1 bild=-1` (Programm-Ende durch F), `R1 ende (Leiche) pos=(3413,-25001)`,
  `R1 VSync 2 F198`; keine KAMERA-R1-Zeile nach F196.
- debug.log: `Folge R ROOM2090 F196 wert=1/0`, `Schluessel-Schnitt statt R1-Rueckschnitt: Explosions-Cut 1 -> Ausgangs-Cut 0 F196`,
  `Schluessel-Lage: Kamera 2 (groesstes Bild, laufende 0), Lage (1400,-1704,-23300) Tiefe 2643, 1001 von der Leiche ... Blink-Spitze 118`,
  `Water Key (Item 0x34, Bits (9,111)/(9,112)) bei der Leiche (1400,-1704,-23300) ... Zone Slot 15`, `Folge E ROOM2090 F196 wert=2/0`,
  `Schluessel-Einstellung: Cut 2 (vorher Cut 0) F196`, `Folge B ROOM2090 F199 wert=0/0` (Musik aus), `Schluessel-Einstellung Ende:
  Cut 0 F288 (Einstellung Cut 2 ab F196, vorher Cut 0), Zonenkamera an`.
- state.log (Abschnitt 2090): cam=0 F1-F159, cam=1 F160-F195, **cam=2 F196-F286**, cam=0 ab F287 - kein cam=0 zwischen Explosion und
  Schluessel (Abnahme 2: cam=0 F196-F197).
- Bilder (`tools/kam.py`, Referenzen E = fd 180, K = fd 240, X = fd 300): fd 186-196 E (zu E 5,91-8,60 / zu X 10,64-15,43), **fd 197 K
  (K 2,44 / X 10,53 / E 12,52)**, fd 198-210 K (0,01-1,67) -> Folge `EEEEEEEEEEEKKKKKKKKKKKKKK`; Wechsel-Pixel (>40) zum Vorbild fd 197
  3220, danach 0-21. Bildbogen `F41a3/KW_wechsel.png` (fd 194/195/196 Explosion, fd 197/198/240/287 Schluessel-Einstellung, fd 288-300
  Cut 0 mit Leon an der Leiter): kein Bild der Zonenkamera zwischen Explosion und Schluessel.
- Schluessel (`tools/diffserie.py` KW gegen KWo, jede Abweichung zaehlt): fd 197-287 **91/91 Bilder**, je 29 px, Kasten (32,192)-(44,206)
  = 13 x 15, hellster Pixel 70 -> 103 (x1,47), Periode 60 Bilder (Spitzen in Bild 15 und 75 der Einstellung); fd 196 2 px (164,105)-(166,106)
  (im Takt des Schnitts in Cut 1 gelegt, angezeigt ab dem naechsten Bild); fd 150-195 bitgleich; ab fd 288 (Cut 0) 8 px am Bildrand
  (318,176)-(319,183) = 2 x 8.

### KWE (Elza)
- rc=0, 48 s. debug.log `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, ROOM2091.
- gator_boss.log / debug.log Zeile fuer Zeile wie KW (`KAMERA R1 EXPLOSION ... -> Cut 1 F160`, `KAMERA R1 zurueck -> Cut 0 F196`,
  `KAMERA R1 frei ... F196`, `R1 ende (Leiche) pos=(3413,-25001)`, `Folge R ROOM2091 F196 wert=1/0`, `Schluessel-Schnitt statt
  R1-Rueckschnitt: Explosions-Cut 1 -> Ausgangs-Cut 0 F196`, `Schluessel-Einstellung: Cut 2 (vorher Cut 0) F196`, `... Ende: Cut 0 F288`).
- state.log (2091): cam=0 F1-F159, cam=1 F160-F195, **cam=2 F196-F286**, cam=0 ab F287.
- Bilder (kam.py, E = fd 180, K = fd 240, X = fd 300): fd 186-196 E (3,26-5,16), **fd 197 K (K 2,44 / X 10,61 / E 10,87)**, fd 198-210 K
  -> `EEEEEEEEEEEKKKKKKKKKKKKKK`. Bogen `F41a3/KWE_wechsel.png` (fd 195/196 Explosion, 197/198/286/287 Schluessel, 288/300 Cut 0 mit
  Elza an der Leiter).
- Schluessel (diffserie KWE gegen KWEo): fd 197-287 91/91 Bilder, 29 px, 13 x 15, hellster 70 -> 103 - wie Leon. Zoom
  `F41a3/KW_key_zoom.png` (6-fach fd 200/212/240 Ring mit Bart, rechts Zwilling fd 212 ohne Schluessel).
- **M1 der Abnahme 2 ist an der exe behoben** (Leon und Elza): Explosion (Cut 1) -> Schluessel-Einstellung (Cut 2) ohne Bild der
  Zonenkamera, danach Cut_old auf Cut 0. Der Schluessel liegt in derselben Lage wie in Abnahme 2 (1400,-1704,-23300), zwei Takte frueher
  (F196 statt F198). Die Explosions-Einstellung behaelt ihre Laenge (F160-F195, wie Abnahme 2).

### Regression der uebrigen Todeslagen (Rezepte der Abnahme 2, wortgleich)
| Lauf | B's Kamera (gator_boss.log) | F (debug.log) | state.log 2090 | Bilder | Schluessel (diffserie mit Zwilling 9:112) |
|---|---|---|---|---|---|
| KLm/KLo Leon R1 ohne Explosionsschnitt (`RE15_PLAYER_POS=-1000,-25550,0,0`, `U1.2,(M0.3,MA0.1)x12`) | `KAMERA R1 Sperre ohne Schnitt: Kopf im Bild von Cut 1 bei (180,98) F46`, `R1 ende (Leiche)`, `R1 VSync 2 F84`, `KAMERA R1 frei ... Cut 1 F84` | keine Zeile R; `Schluessel-Einstellung: Cut 2 (vorher Cut 1) F84` ... `Ende: Cut 1 F176` (= Abnahme 2; s_r1_lief greift) | cam=1 F12-F83, cam=2 F84-F174, cam=1 ab F175 | kam.py fd 78-84 Cut 1, fd 85-90 Cut 2 | fd 85-175 91/91, 29 px, 13 x 15, 70 -> 103; fd 84/176 2 px (Cut 1) |
| TLm/TLo Leon Todesrolle (`RE15_GB_TEST=3`, Testhaken) | - | `Schluessel-Lage: Kamera 1 ... (-5200,-1689,-26800) Tiefe 2441, 1698 von der Leiche`, `Einstellung: Cut 1 (vorher Cut 0) F469` ... `Ende: Cut 0 F561` (= Abnahme 2) | cam=1 F469-F559, cam=0 ab F560 | - | fd 470-560 91/91, 37 px, 17 x 12, 72 -> 108; ab fd 561 bitgleich (Cut 0, H2 der Abnahme 2) |
| KN Leon Teleport Nord (`RE15_PLAYER_POS=2000,-9000,1024,0`, Schuesse ab F151) | `EXPLOSION ... -> Cut 1 F160`, `zurueck -> Cut 0 F196`, `frei ... F196` | `Folge R F196 wert=1/0`, Ersatz-Zeile F196, `Einstellung: Cut 2 (vorher Cut 0) F196` ... `Ende: Cut 0 F288` | cam=1 F160-F195, cam=2 F196-F286, cam=0 ab F287 | kam.py fd 186-206 `EEEEEEEEEEEKKKKKKKKKK` | (Lage = KW) |
| KQ Leon, Schuesse ab F991 (`W33,...`) | `R1 start pos=(4359,-17290)`, `EXPLOSION ... -> Cut 3 F1000`, `zurueck -> Cut 0 F1036`, `frei ... F1036`, `R1 ende (Leiche) pos=(4528,-12323)` | `Folge R F1036 wert=3/0`, `Schluessel-Lage: Kamera 3 ... (1300,-1706,-9200) Tiefe 2667, 2346 von der Leiche`, `Einstellung: Cut 3 (vorher Cut 0) F1036` ... `Ende: Cut 0 F1128` | cam=3 F1000-F1126 durchgehend, cam=0 ab F1127 | kam.py fd 1001-1127 (E = fd 1018, K = fd 1080, X = fd 1200): 0 Bilder X, Wechsel-Pixel <= 280 je Bild; fd 1128 X (4988 Wechsel-Pixel) | - |

### Aufnahme (Regression)
- PKL2 Leon / PKE Elza (Rezept der Abnahme 2: KL-Lage, danach `W0.4,L0.13,U1.2,A0.1,W3,A0.1,...`, `RE15_ITEMBAND_LOG=itemband.txt
  RE15_INV_LOG=1`): itemband.txt `F461 room=2090|2091 ITEM slot 15 floor 0 item 0x34 tk 112 spieler-band 0 spieler=(1361,0,-22886) -> FREI`,
  debug.log `[inv-r39] F554 R2090|R2091 ... inv=03:14,15:50,34:1` (Water Key im Inventar, Leon und Elza). Die Lage (1400,-1704,-23300) ist
  dieselbe wie im Explosionsfall KW/KWE (Zone Slot 15, 1000 x 1000).

- **Urteil 1b: erfuellt.** Der Schluessel liegt bei der Leiche (1001 / 1698 / 2346 Einheiten, innerhalb des Kopf-Wurzel-Abstands),
  blinkt (x1,45-1,50, Periode 60), ist in der Einstellung als Ring mit Bart zu sehen (29-37 Bildpunkte, 91/91 Bilder), ist aufnehmbar
  (Leon + Elza), und der Explosionsfall zeigt kein Zwischenbild mehr (M1 der Abnahme 2 behoben). Nach der Einstellung ist er in der
  laufenden Kamera klein (H1).

## 4. Punkt 2 — "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

- Code seit Abnahme 2 unveraendert (`re15_gatorszene_r41_spawn_weg`, Haken scd_vm.c op_sce_em_set nach dem Kill-Flag-Tor).
- exe, state.log ` t=25 ` (Typ 0x25): KW/KWo/KWE/KWEo 0/833, KLm/KLo 0/671, TLm/TLo 0/1011, KN 0/933, KQ 0/1551, L1/E1/LW/EW 0/1289,
  L1a 0/426, L1b 0/583, PKL2/PKE 0/1011, G1b 0/451; je 2090/2091-Besuch zwei Zeilen `[gatorszene] ROOM2090|ROOM2091: Spinne 0x25 bei
  (-100,-8296)|(-2300,-23896) verworfen` (L1/E1/LW/EW je vier = zwei Besuche). G2b: 120 ` t=25 `-Zeilen nur im 20A0-Abschnitt (120 Zeilen
  bis EXIT_AT, Schachtspinnen, nicht verlangt), im 2090-Abschnitt 0.
- RDT: shared_assets/PSX/STAGE2/ROOM2090.RDT und ROOM2091.RDT `cmp` gleich info/Re1.5 (Saetze @0x00AB0/@0x00AC4 in Abnahme 1 gelesen).
- Riegel `unit_r41_gatorszene_spinnen`, `unit_r38_spinnen`, `integration_r38_spinnen_echtlauf` gruen (Abschnitt 1).
- **Urteil Punkt 2: erfuellt** (Leon und Elza).

## 5. Nachtrag 21 (Leiter statt Tor, Tor mit dem Schluessel) - Regression

Gemeinsam `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_MSG_LOG=1`:
- G1b (2090, `RE15_NOAUDIO=1 RE15_SET_FLAG=9:110 RE15_PLAYER_POS=6630,-16350,0,1 RE15_PAD_AT=60:A`): `ROOM2090 Slot 1 (Tor) gesperrt:
  Ereignis 19, Bit (9,113) = 0`, `[msgtext] room=2090 id=42 "It's locked."`, `Folge Z ROOM2090 F60`.
- G2b (dto. + `RE15_GIVE=0x34:1`, A F60/140/200/260/320/380/440): `[msgtext] room=2090 id=43 "You've opened the lock."`, `Folge U F60`,
  `[aot] DOOR FIRE slot=1 rect=(7250,-16350,hw=500,hh=1750) target_cut=13 spawn=(4950,-3600,4100)`, `[tuer] Sequenz Archiv 2 DOOR26
  ... Seite S164`, `EXIT_AT: Bild 120 in Raum 20a0`.
- G3b (20A0 ohne Bit 113, mit Ton, `RE15_PLAYER_POS=4950,4100,3072,2 RE15_PAD_AT=60:A`): `ROOM20A0 Slot 0 (Tor) gesperrt: Ereignis 20`,
  `[msgtext] room=20a0 id=1 "It's locked."`, `Folge Z ROOM20A0 F60`, tuerse.log genau EINE Zeile `F60 raum=20A0 nachricht=1 art=M weg=AOT
  satz=0(ZU_A) nr=1`.
- Leiter-Zugang: L1/E1 (Abschnitt 2). **Urteil: erfuellt.**

## 6. RE-Gate (Stichproben selbst disassembliert, richtige Binaerdatei)

`re2_disasm.py --bin EM23_OVL_0000.BIN` (info/re2leon/COMMON/BIN, geladen @0x80100000), `re2_disasm.py` ohne --bin = info/re2leon/PSX.EXE,
`re15_disasm.py` = info/Re1.5/PSX.EXE:

| Zitat im Diff (Nachbesserung 3) | selbst gelesen | Ergebnis |
|---|---|---|
| RE2 R1 P0: Sperre @0x8010220c, +0x218 \|= 0x40 @0x80102248, Cut 5 @0x80102268 | `80102200 lw v0,-1064(v0)` (0x800cfbd8), `8010220c ori v0,v0,0x100`, `80102214 sw v0,-1064(at)`; `80102208 lh v1,18464(v1)` (0x800d4820) gegen 6/13 (`80102218-24`), `80102234 slti v0,v0,-13300` (z +0x40); `80102240 lhu v0,536(s1)` / `80102248 ori v0,v0,0x40` / `8010224c sh`; `80102268 jal 0x8002c7ac` + `8010226c addiu a0,zero,5` | stimmt |
| P1-Ende: `jal 0x8001a330` a1 = 9 @0x80102494, `beq v0,zero,0x8010252c` @0x8010249c, `lhu v1,536(s1)` @0x801024a4, `andi v0,v1,0x40` @0x801024ac, `beq` @0x801024b0, Rueckschnitt `jal 0x8002c7ac` @0x801024ec | `8010248c addiu a1,zero,9`, `80102494 jal 0x8001a330`, `8010249c beq v0,zero,0x8010252c`, `801024a4 lhu v1,536(s1)`, `801024ac andi v0,v1,0x40`, `801024b0 beq v0,zero,0x8010251c`, `801024b4 andi v0,v1,0xffbf` / `801024c0 sh`, `801024e4/e8` a0 = 13 / 6, `801024ec jal 0x8002c7ac`, `8010251c-28` +0x6 += 1 | stimmt |
| Cut setzen 0x800cfbf2 @0x8002c7b8 (RE2 PSX.EXE) | `8002c7b8 sb a0,-1038(at)` (0x800cfbf2), `8002c7bc jal 0x8002c7dc`, `8002c7c8 sw v0,-7364(at)` (0x800ce33c) | stimmt |
| P2 @0x80102638-50, P3 Tot-Flag @0x8010265c-64, Sperre frei @0x80102688-94 | `80102638 lbu v1,6(s1)` .. `80102640 addiu v1,v1,1` .. `80102650 sb v1,6(s1)` (+ `80102644 andi 0xfffb`); `80102654 lhu v0,270(s1)` / `8010265c ori v0,v0,0x4000` / `80102664 sh v0,270(s1)` (+ `jal 0x8007730c` a1 = 30); `80102668 addiu v0,zero,2` / `8010267c sb` (0x800dfc1a); `80102684 lw v0,-1064(v0)` / `80102688 addiu v1,zero,-257` / `8010268c and` / `80102694 sw` | stimmt: zwischen Rueckschnitt und Tot-Flag liegen genau zwei Takte |
| RE1.5 Cut_chg-Zwilling, work_vars[0x0A] @0x800402fc | `800402c0 lbu a1,4068(a1)` (0x800b0fe4), `800402d4 ori v0,v0,0x100` / `800402d8 sw` (0x800aca3c), `800402e4 sb a1,16251(at)` (0x800b3f7b), `800402ec sh a2,4072(at)` (0x800b0fe8), `800402fc sh a0,4068(at)` (0x800b0fe4) | stimmt |
| RE1.5 Cut_old, Zonenkamera an @0x80040374-84 (H4 der Abnahme 2 berichtigt) | `8004033c lbu a0,16251(a0)`, `80040364 sh a0,4068(at)`, `80040374 lw v0,-13764(v0)` (0x800aca3c) / `80040378 addiu v1,zero,-257` / `8004037c and` / `80040384 sw` | stimmt |
| Praemisse "ROOM2090/2091-SCD aendert den Cut nicht" (Erkennung ueber den Cut-Wechsel) | `scd_dump_room.py` ROOM2090/2091 (main00 + sub00..03): 0 Cut_chg/Cut_old/Cut_auto (Opcode-Zaehlung: Speed_set 18, Sleep 12, Add_speed 12, ...); RDTs `cmp` gleich info/Re1.5 | stimmt |

- Konstanten: Nachbesserung 3 fuehrt keine neue Verhaltenskonstante ein (nur Logik: Erkennung ueber `re15_gr2_r1_laeuft` +
  `re15_gr2_kamera_gesperrt` + Cut-Wechsel); die Schnittform ist als PORT-WAHL der Inszenierung gekennzeichnet (Kopf von
  key_rueckschnitt_pruefen, Form ROOM2010 sub03 @0x008EE). Riegel-Schranken (>= 90 Bilder = GSZ_LEICHE_BILDER, Halt 90..94, Tot-Flag
  zwei Takte nach dem Schnitt = P2/P3) hergeleitet.
- Suche im Diff (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/TODO/
  vorerst/geschaetzt/geraten/ungefaehr/approx/FIXME/HACK: 0 Treffer; kein getenv in gatorszene_r41.c / gatorszene_r41_pc.c /
  re15_gatorszene_r41.h; kein Env-Schalter als Abschluss.
- Erklaert der Fix den Befund? Ja: M1 entstand, weil F erst mit dem Tot-Flag (P3) schnitt, zwei Takte nach B's Rueckschnitt am
  Clip-9-Ende. Jetzt erkennt F den Rueckschnitt im selben Takt (Nachtakt nach `gb_takt`, vor dem Zeichnen) und ersetzt ihn - gemessen:
  `Folge R` im Bild von B's `zurueck` (F196 / F1036), state.log und Bilder ohne Cut 0 dazwischen.

## 7. Vertrag

- Bits nur (9,110..113) (VERTRAG 1.1; test_r36_kartenbits.c k_port[] 4 Zeilen), Raum-Scratch (5,28/29/30) nur 2090; Nachrichten
  2090/2091 Ids 42/43 (zugeteilt 42-46); Ereignisse 18/19/20, AOT-Plaetze 15/20 - Nachbesserung 3 belegt nichts Neues.
- Pfade `git diff master...HEAD --name-only` (Merge-Basis 9f10bdb0): keine release/, platform/android/, shared_assets/PSX/, pri/,
  synchro/, kein CMakeLists.txt (grep leer); neu nur shared_assets/RE2/SOUND/MAIN1A.BGM (seit Abnahme 0, im Dossier genannt).
  Nachbesserung 3 aendert nur F-eigene Dateien (gatorszene_r41.c, test_r41_gatorszene.c/.cmake, probes/r41_gatorszene.cmake, Dossier,
  Belegbilder); gemeinsame Dateien unveraendert seit Abnahme 2 (Haken je 1-5 Zeilen, markiert). Spur B's Modul nicht geaendert, nur
  dessen API `re15_gr2_kamera_frei` aufgerufen (FG-E1: Beruehrung ueber APIs).
- Riegel je Punkt vorhanden und messend: neu `unit_r41_gatorszene_tod_explosion`, `integration_r41_gatorszene_tod_explosion(_elza)` mit
  Auswerter `kamfolge` (Gegenprobe an alten Bildern -> FEHLER, Abschnitt 1). Parallel-fest (Laufordner `lauf_<zufall>`, exe-Kopie
  `re15_pc_r41f_<zufall>.exe`, Bilder am Ende geloescht); Linux: `cmake_minimum_required(VERSION 3.21)`, `_DEFAULT_SOURCE` fuer
  setenv/unsetenv, Windows-Zweig `_putenv`, `m` gelinkt. Die exe-Kopie traegt weiter fest ".exe" (Hinweis seit Abnahme 0, wie Spur B).
- Nebenwirkungen: 217 gezielte Tests gruen (Abschnitt 1); Leon = Elza in allen Laeufen; Nachbarraeume 2080 (Montage) und 20A0 (Tor)
  gemessen (Abschnitte 2/5).

## 8. Maengel (nummeriert, nachpruefbar)

- **M1 Nachtrag 29 nicht umgesetzt: Leiche samt Kette zu nah an der Leiter, kein Ausweichen am Kampfbeginn (Punkt 1a, Leon und Elza).**
  Nutzer-Wortlaut 2026-10-25 12:08 (AUFTRAG.md, "Auftrag an Spur F"). Am Code-Stand da156ce6 liegen Leiche und Fressstelle unveraendert
  bei (-5500,-23700) / (-4594,-24306), rund 3800 Einheiten von der Leiter-Ankunft (-8200,-25550). Gemessen (Abschnitt 2): beim Kampfbeginn
  F405 Gator-Wurzel (-4254,-20984), 6162 von Leon; stehend Angriff F459, erster Treffer F466 (61 Bilder, Leon L1 und Elza E1 gleich);
  sofort nach Osten rennend (`RE15_INPUT_SCRIPT=W13.5,UX4`) Angriff F431, Treffer F438 (33 Bilder, LW/EW), danach Tod (HP 0 F545 bzw.
  F573). Bildbogen `F41a3/LW_ausweichen.png`. Soll (Nachtrag 29 / Auftrag des Orchestrators): Leiche samt Kette weiter weg, Abstand und
  Bilder bis zum ersten Angriff mindestens wie am Ende des RE2-Intros in ROOM40A0 (mit @0x-Beleg) bzw. so weit die Wasserzonen erlauben,
  Ausweichen gelingt (stehend und sofort weglaufend, Leon und Elza), Schluessel-Sichtbarkeit an der neuen Lage neu gewaehlt und gemessen,
  Riegel fuer Abstand/Bilder bis zum ersten Angriff, bestehende F-Riegel mit Beleg nachgezogen.

Hinweise (keine Maengel):
- H1 Nach der Einstellung ist der Schluessel in der laufenden Kamera klein (KW/KN Cut 0: 8 px am Bildrand, KL Cut 1: 2 px, Todesrolle
  Cut 0: 0 px) - unveraendert seit Abnahme 2 (H2); die Einstellung zeigt ihn 91 Bilder lang deutlich, die Aufnahme funktioniert.
- H2 Die Form "Kameraschnitt auf den Schluessel nach dem Tod" (und jetzt "an der Stelle von B's Rueckschnitt") ist PORT-WAHL; die
  Bestaetigung durch den Nutzer steht weiter aus (Frage im Dossier).
- H3 Nebenbefund des Bau-Agenten (`s_r1_lief`, R1-Tod aus frueherem Raumbesuch) nur im Unit-Riegel gemessen (zweiter Fall im selben
  Prozess); an der exe nicht nachstellbar ohne Laden eines Spielstands. `musik_soll` nutzt weiter `re15_gr2_tot()` ueber alle Slots.
- H4 Integration F+G: im Explosionsfall stehen B's Zeilen `KAMERA R1 zurueck`/`KAMERA R1 frei` jetzt im Rueckschnitt-Takt (frei vorher
  erst in P3); G-E1 aendert die R1-Kamerawahl in B's Modul - der Querbezug ist im Integrationsbaum zu messen (Orchestrator G-E1).
- H5 Unveraendert offen (Bau-Agent): MAIN2E nach dem Tod statt RE2-Ausblenden, Ersatzlage nach dem Laden ohne Einstellung, PSX-Bau
  ungeprueft, Schachtspinnen 20A0, Nachrichten-Ids 44-46 frei.

## 9. Urteile

| Punkt | Urteil | Beleg |
|---|---|---|
| 1a Ankunftsszene (Leiche + blinkender Schluessel, Knall, 2080-Zombies oben, erneut Leiche, Alligator aus dem Wasser frisst Leiche samt Schluessel, Schrei/Schuetteln zu Leon, RE2-Kampfmusik; Nachtrag 29: Leiche samt Kette weiter weg, Ausweichen moeglich; Leon + Elza) | teilweise | 2: L1/E1 Folge Zeile fuer Zeile gleich Abnahme 2, Kamera-Uebergabe Cut 0 75/75, Knall Takt 573, MAIN1A, Bilder L1_ankunft/L1_kampfstart; aber Nachtrag 29 offen: erster Treffer nach 61 (stehend) bzw. 33 Bildern (weglaufend), LW_ausweichen.png (M1) |
| 1b Schluessel nach dem Tod bei der Leiche, blinkend (M1 der Abnahme 2) | erfuellt | 3: KW/KWE state.log cam 1 -> 2 ohne Cut 0, kam.py `E+K+`, 91/91 Bilder 29 px 13 x 15, Puls 70 -> 103; KL/TL/KN/KQ wie Abnahme 2 bzw. ohne Zwischenbild; Aufnahme 34:1 Leon + Elza |
| 2 Spinnen aus 2090 entfernen | erfuellt | 4: 0 Spinnen-Zeilen in allen 2090/2091-Abschnitten, Leon + Elza |
| Nachtrag 21 Leiter statt Tor, Tor mit dem Schluessel | erfuellt | 5: G1b/G2b/G3b, tuerse.log 1 Zeile |
| RE-Gate | erfuellt | 6: 7 Stichproben (RE2 EM23 P0/P1-Ende/P2/P3, RE2 PSX.EXE 0x8002c7ac, RE1.5 Cut_chg/Cut_old) stimmen, SCD-Praemisse belegt, keine Rate-Woerter, keine neue Konstante |
| Vertrag/Riegel | erfuellt | 1/7: 217/217 gruen, nur zugeteilte Ressourcen und erlaubte Pfade, neue Riegel messen (kamfolge an alten Bildern FEHLER) |

**Ergebnis: NICHT bestanden** - M1 der Abnahme 2 ist behoben (Leon und Elza, Riegel messend), offen ist M1 dieser Abnahme: der nach dem
Bau eingegangene Nutzer-Nachtrag 29 (Lage der Leiche samt Kette, Ausweichen am Kampfbeginn), an der exe als zutreffend gemessen.
