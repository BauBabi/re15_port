# Runde 41 — Spur F "gatorszene": Abnahme 4 (unabhaengig)

Stand: Zweig `r41/gatorszene`, Code-Stand `877919e2` (letzte Code-Aenderung `fb64e68c`; Basis r41/integration `9f10bdb0`, Abnahme 3
auf `cc4a4646`). Waehrend der Abnahme legte der Orchestrator `e96d076e` + `3b2340c7` (nur AUFTRAG.md: **Entscheid F-E2**, 14:21) in den
Zweig - vier Minuten nach dem Abschluss-Commit des Bau-Agenten (877919e2, 14:17). AUFTRAG.md ist massgeblich, F-E2 gilt also fuer diese
Abnahme (UEBERGABE master 14:23: "F Abnahme 4 laeuft ... danach F-Lauf mit F-E2, falls nicht erfuellt"). Baum
`.claude/worktrees/r41_gatorszene`. Pruefer: Abnahme 4 (2026-10-25). Ich glaube dem Bau-Agenten nichts, was ich nicht selbst am gebauten
Stand gemessen habe.

## 0. Bau und Lektuere

- Gelesen: AUFTRAG.md (bis Entscheid F-E2 14:21; Zweig = master bis auf M-E2, das F nicht betrifft), VERTRAG.md, Dossier F_gatorszene.md
  (Schwerpunkt "Nachbesserung 4"), F_abnahme_3.md, UEBERGABE.md (master), `git log master..HEAD` (seit Abnahme 3: aa7b3e8a .. 877919e2,
  10 Commits, dazu e96d076e/3b2340c7), `git diff cc4a4646..877919e2 -- re15_port` WIRKLICH gelesen: Code nur
  `engine/src/gatorszene_r41.c` (+41/-31) und `include/re15_gatorszene_r41.h` (+2/-1) - ausschliesslich Konstanten und Kommentare
  (RE15_GSZ_LEICHE_OBJ 2 -> 3, GSZ_CUT_LEICHE 1 -> 5, GSZ_LEICHE_X/Z/ROT_Y (-5500,-23700,384) -> (700,-8200,2432), GSZ_GATOR_GIER
  1024 -> 3552, GSZ_KEY_AUF_RX/RZ 544/-464 -> 672/-144); dazu test_r41_gatorszene.c (+176/-17: Teil abstand/abstand_elza, Auswerter
  re2soll, nachgezogene Erwartungen leiche/programme/szene), test_r41_gatorszene.cmake (+155/-11: r41f_re2soll/r41f_kampf/r41f_folge_c,
  NEU flucht/flucht_elza), probes/r41_gatorszene.cmake (+9/-2), test_r41_re2gator.cmake (Spur B, +5/-1: EXIT_AT 420 -> 440 mit
  Querbezug (e)), tools/r41_gatorszene/leiche_lage_n4.py (+292), tools/r41_gatorszene/probe_re2_intro_ende.c (+140), Dossier, 3
  Belegbilder. `git diff master` (zwei Punkte) zeigt zusaetzlich Loeschungen von Dateien, die master NACH der Basis 9f10bdb0 bekam
  (kleinkram_b, kollision, stage3szenen_a, spinnenanim, synchro/.../main46.wav) - keine Aenderungen von F; massgeblich ist
  `git diff master...HEAD` (60 Dateien).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.` /
  `=== LOCAL-BUILD-OK (build)`; `git status --short` leer (bis auf diesen Bericht); gatorszene_r41.c 13:49:01 < gatorszene_r41.c.obj
  13:49:12 < re15_pc.exe 13:49:13 (letzte Code-Aenderung fb64e68c 13:50 committet, 877919e2 und die Orchestrator-Commits aendern keinen
  Code) -> die exe ist der Code-Stand HEAD; die Messlaeufe tragen die neue Buehnen-Zeile `Leiche (-206,-7594) ... S (-7379,-15540) Gier 3552`.

Messordner (scratchpad, nicht im Repo):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\F41a4`
(`runs/<lauf>/debug.log|state.log|gator_boss.log|fd_*.ppm`; Helfer `tools/lauf.sh` = eigene exe-Kopie `re15_pc_f41a4_<lauf>.exe`,
`timeout`, danach geloescht; eigene Auswerter `tools/kampf_a4.py` (Kampfbeginn = letzte `Folge C`, letzter Raum-Abschnitt von state.log,
Abstand, erster Clip 4, erster HP-Abfall, Clip-Wechsel), `tools/keypix.py` (graue Pixel max-min <= 14, Helligkeit > 25 im Ausschnitt: Zahl,
Kasten, hellster je Bild), `tools/gegenprobe.cmake` (die Riegel-Funktionen r41f_kampf/r41f_folge_c WORTGLEICH aus
test_r41_gatorszene.cmake herausgeschnitten), dazu diffserie.py/sheet.py/crop.py der Abnahme 3). Alle Laeufe `RE15_WINDOW_SCALE=1`,
Bilder nur per `RE15_FRAMEDUMP`.

## 1. Gezielte Tests

Regex = die Regex des Bau-Agenten (= Abnahme 3 `F41a3/regex2.txt`, Datei `F41a4/regex.txt`; `ctest -N -R` -> 221 = 217 + die vier neuen
abstand/abstand_elza/flucht/flucht_elza). Sie deckt alle Testdateien, die gatorszene/RE15_GSZ_/re15_gr2_/GB_TEST/GATOR_KANISTER oder
2090/2091 nennen und als Test registriert sind (grep ueber re15_port/tests, Abgleich mit `ctest -N`; die Gator-Sonden probe_gator_maul*,
probe_p3_*, probe_r16_* sind nur Programme). Befehl mit dem PATH von local_build.sh (`F41a4/tools/ctest_lauf.sh`):
`ctest --test-dir "<baum>/re15_port/build" -R "$(cat F41a4/regex.txt)" -j4 --timeout 1200 --output-on-failure`
Ergebnis (parallel zu eigenen exe-Laeufen): **`100% tests passed, 0 tests failed out of 221`** (`Total Test time (real) = 1065.00 sec`,
Log `F41a4/ctest_all.log`). Darin integration_r41_re2gator 264.66 s, integration_r41_gatorszene_leon/_elza 55.38/75.60 s,
_flucht/_flucht_elza 55.38/75.68 s, _tod/_tod_elza 65.74/107.30 s, _tod_r1/_tod_r1_elza 43.38/84.52 s, _tod_explosion/_elza 28.31/48.65 s,
unit_r41_gatorszene_abstand/_elza 0.16/0.17 s, unit_r30_tuer_verschlossen, unit_r36_tueren_schott, unit_r38_spinnen. Die Behauptung des
Bau-Agenten (221/221) ist bestaetigt; kein roter Fenster-Haken, nichts nachzufahren.

Messen die neuen Riegel? Selbst geprueft:
- `test_r41_gatorszene re2soll` -> `re2soll: abstand=11402 abstand2=130647809 bilder=121` (11402 = SquareRoot0 wie RE2 +0x1F0;
  sqrt(130647809) = 11430).
- `test_r41_gatorszene abstand` direkt: `Abstand Gator-Wurzel <-> Spielfigur am Kampfbeginn 17496 >= RE2 11402`, `Leon steht: erster Biss
  nach 470, erster Treffer nach 477 Bildern ... nicht vor RE2 121`, Gegenprobe Fassung 1 `erster Biss nach 48 Bildern < RE2 121`.
- Die exe-Auswertung des Riegels (r41f_kampf, wortgleich herausgeschnitten, `F41a4/tools/gegenprobe.cmake`, Soll 130647809 / 121) auf den
  ALTEN Laeufen der Abnahme 3 (`F41a3/runs/L1`, `/LW`, Stand da156ce6): beide `CMake Error ... r41_gatorszene: Abstandsquadrat
  Gator-Wurzel <-> Spielfigur am Kampfbeginn ...` (FEHLER); auf meinen neuen Laeufen L1/E1/LW/EW: `OK: ab2 307624213 ... biss F892
  treffer F899` bzw. `xb 6732 biss F947 treffer F954`. Der Riegel unterscheidet also vorher/nachher - aber er prueft nur UNTERE Grenzen
  (`_ab2 LESS re2_ab2`, Biss/Treffer `LESS_EQUAL _b_ende`); eine obere Grenze (F-E2: "Sollwert RE2 mit oberer UND unterer Grenze") fehlt
  (Mangel M4).

## 2. Punkt 1a — Ankunftsszene (Leiter 2080 -> 2090; Wortlaut + Nachtraege 6/17/20/21/29 + Entscheid F-E2)

Weg: `RE15_DEBUG_JUMP=2080@250` + `RE15_FIRE_AOT=1@40#2080|2081` = die Leiter-Tuer ROOM2080 Slot 1 ueber `re15_aot_fire_slot` (main.c:
"exakt der Pfad, den auch das Hineinlaufen nimmt (Scan -> aot_fire_door)"), `RE15_SET_FLAG=3:245` (Original-Rechner, Leiter frei).

### 2.1 Szenenfolge (Leon L1, Elza E1)
- L1: `tools/lauf.sh L1 400 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
  RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=1150#2090
  RE15_FRAMEDUMP=1-1150/3:fd_ RE15_AUDIO_CAP_SYNC=audio.raw` -> rc=0, 69 s. debug.log: `[aot] DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,
  hh=500) target_cut=0 spawn=(-8200,0,-25550)`, je 2090-Besuch zwei Zeilen `Spinne 0x25 bei (-100,-8296)|(-2300,-23896) verworfen`,
  `ROOM2090 Slot 1 (Tor) gesperrt: Ereignis 19, Bit (9,113) = 0`, Folge `A F40` -> `P 1` -> `T F98` -> `[aot] DOOR FIRE slot=20 ...
  target_cut=2 spawn=(-9950,-14400,-25700)` -> `P 2 ROOM2080 F142` + `M ROOM2080 F142 wert=8/2` -> `DOOR FIRE slot=20 ... target_cut=5
  spawn=(-8200,0,-25550)` -> `P 3 F156` -> `S F53` -> `Buehne: Leiche (-206,-7594) Kopf13-Versatz (2018,2290) Weg4 (891,982) Weg6
  (4264,4674) -> A (-3115,-10866) S (-7379,-15540) Gier 3552` -> `G 1/5 F53` -> `G 2/4 F176` -> `K 13 F189` -> `G 3 F190` -> `G 4 F310`
  -> `V F369` -> `G 5 F369` -> `G 6/4 F393` -> `C ROOM2090 F422 wert=0/1` + `Kamera-Uebergabe: Cut 0 (Eintritts-Cut), Spielfigur im Bild
  (Fuss-Bildpunkt 88,127)` -> `G 7 F422` -> `B 1 F424` + `[bgm] stage=1 room=09 entry=FF3C -> MAIN3C` + `[bgm] loaded: 1 VAGs, SEQ 100B`
  (= RE2 MAIN1A). gator_boss.log: `RE2 KAMERA SZENE DURCHBRUCH RE2-Cut 10 -> Cut 5 F53 clip=6 ... frontal=4085 soll=4092`,
  `RE2 KAMERA SZENE FINISHER RE2-Cut 7 -> Cut 4 F176 clip=4 bild=0 frontal=-1625 soll=4094 massstab=1282 ... grenze=1892` (Kandidat Cut 5:
  `massstab=1944 >grenze frontal=3933`), `RE2 KAMERA SZENE zurueck -> Cut 5 F422`, `frei (Zonenkamera an) Cut 5 F422`; state.log ab F422
  `cam=0`. = Behauptung des Bau-Agenten Zeile fuer Zeile.
- E1 (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_NOAUDIO=1`, 2081/2091): `[pl] Spieler-Familie PL04
  (character=4, Elza-Bit=1)`, dieselbe Folge mit denselben Bildnummern (`Folge C ROOM2091 F422`, Kamera-Uebergabe Cut 0, Spielfigur im
  Bild), `EXIT_AT: Bild 1150 in Raum 2091`.
- Bilder (`F41a4/L1_szene.png`, fd 4-601): Cut 5 auf die Leiche mit dem Ring (fd 31/52), der Gator taucht im Westkanal auf und kommt
  frontal auf die Leiche zu (fd 70-175, der Ring neben dem Kopf noch sichtbar), Schnitt auf Cut 4 (fd 178-421: Biss, Schuetteln, Kauen,
  Wende, Schrei), Cut 0 mit Leon an der Leiter (fd 424/502), der Gator erscheint in Cut 0 ab ~fd 601. Fress-Ausschnitte
  `F41a4/L1_fressen_zoom.png` / `L1_fressen_zoom2.png` (Cut 4, 2- bzw. 3-fach): Kopf am linken Bildrand, von der Leiche im Maul nur
  Glieder/Kleidung in einzelnen Bildern (fd 256/277/283/298/310), in fd 184-238 keine Leiche zu erkennen.
- Die Szenenfolge des Wortlauts und der Nachtraege 6/17/20/21 (Leiche mit blinkendem Schluessel -> Knall -> 2080 oben -> erneut die Leiche
  -> der Alligator aus dem Wasser frisst Leiche samt Schluessel -> Schrei/Schuetteln zu Leon -> RE2-Kampfmusik) ist erhalten (Leon + Elza).

### 2.2 Kampfbeginn (Nachtrag 29; F-E2 Punkt 1) - Leon/Elza, stehend und sofort weglaufend
Auswertung `tools/kampf_a4.py` (eigener Auswerter; Gegenprobe an den Abnahme-3-Laeufen F41a3/runs/L1|LW = Stand da156ce6):

| Lauf | Eingabe | Kampfbeginn (Folge C) | Spieler | Gator-Wurzel (Gier) | Abstand | erster Biss (Clip 4) | erster Treffer (HP 100 -> 50) | im RE2-Fenster F<c+1>..F<c+121> |
|---|---|---|---|---|---|---|---|---|
| A3 L1 (vorher, da156ce6) | stehend | F405 | (-8392,-25550) | (-4254,-20984) (1505) | 6162 | F459 (+54) | F466 (+61) | Biss JA, Treffer JA |
| A3 LW (vorher) | Osten rennen | F405 | (-8392,-25550) | (-4254,-20984) | 6162 | F431 (+26) | F438 (+33) | JA / JA |
| **L1** Leon | stehend | F422 | (-8200,-25550) | (-2178,-9077) (1253) | **17539** | F892 (**+470**) | F899 (**+477**) | nein / nein |
| **E1** Elza | stehend | F422 | (-8200,-25550) | (-2178,-9077) (1253) | **17539** | F892 (+470) | F899 (+477) | nein / nein |
| **LW** Leon | `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W13.5,UX4.6` (`Tick 405 -> F406 Tasten 0x4010`) | F422 | (-8200,-25550) -> (6732,-25550) ab F522 | (-2178,-9077) | **17539** | F947 (**+525**) | F954 (**+532**) | nein / nein |
| **EW** Elza | wie LW (2081/2091) | F422 | wie LW | (-2178,-9077) | **17539** | F947 (+525) | F954 (+532) | nein / nein |

Verlauf L1 (state.log, Clip-Wechsel des Gators): Clip 0 ab F423 (Schwimmen, Sperre 110), Clip 1 F532 (Paddeln durch den Westkanal),
**Clip 2 F591 -> Clip 3 F741 -> Clip 0 F891** (RE2-Zonen-Dash, 300 Bilder), Biss Clip 4 F892; HP 50 F899, 0 F1020, -50 F1127. LW: Leon
steht ab F522 an der Ostwand (x 6732), Dash Clip 2 F588 -> Clip 3 F738, Biss F947. Bildboegen `F41a4/L1_kampf.png` (fd 424-901: Leon
steht an der Leiter, der Gator kommt ab ~fd 601 ins Bild, Biss fd 892) und `F41a4/LW_flucht.png` (fd 424-952).
- **Nachtrag 29 (Wortlaut: "muss weiter weg sein, damit man am Start des Kampfes eine Chance hat"):** an der exe erfuellt - Leiche samt
  Kette liegt im Nordbecken, beim Kampfbeginn ist der Gator 17539 entfernt, stehend kommt der erste Biss nach 470, beim Wegrennen nach 525
  Bildern, waehrend des Rennens kein Treffer (Leon = Elza). Vorher 6162 / 54 / 26.
- **F-E2 Punkt 1 ("Kampfbeginn = RE2, nicht 'mindestens': so nah wie moeglich an F's eigenem RE2-Beleg ... Abstand 11430, erster Kontakt
  nach 121 Bildern ..., nicht darunter. 17539 und 470 Bilder sind fast das Vierfache und kein RE2-Kampfbeginn."):** NICHT erfuellt -
  gemessen 17539 = 1,53 x 11430 und 470 = 3,9 x 121 Bilder (stehend). Den Unterschied macht zum groessten Teil der Zonen-Dash
  (Clip 2/3 F591-F891 = 300 Bilder); F-E2 verlangt fuer diesen Fall eine Lage mit der RE2-Zonenbeziehung am Kampfbeginn (Beleg in RE2) -
  nicht gebaut (die Wahl in leiche_lage_n4.py nimmt den besten Schluessel-Wert UNTER der Bedingung Abstand >= 11430, ohne obere Grenze).
  -> Mangel M1.

### 2.3 Schluessel in der Ankunftseinstellung (Wortlaut "mit einen blinkenden Schluessel"; F-E2 Punkt 2)
- L1a (Leon): `tools/lauf.sh L1a 300 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080
  RE15_EXIT_AT=137#2090 RE15_FRAMEDUMP=1-137/1:fd_` -> rc=0; state.log 2090: `F1 cam=0`, `F7 cam=5` bis F137. E1a (Elza, 2081/2091)
  gleich.
- `tools/keypix.py <lauf> 10 137 150 108 186 135` (graue Pixel ueber dem Ruecken; der Kasten endet ueber der Leiche): Leon **hellster Pixel
  min 47 / max 70**, 2..16 Pixel je Bild, Kasten je Bild 3 x 3 .. 9 x 12 (Huelle aller Bilder (160,122)-(169,134) = 10 x 13); Elza
  identisch (47 / 70, 2..16, Huelle 10 x 13). Der Ring dreht mit dem Wippen der Leiche (sub03 @0x00B5E): in F38-F58 / F114-F134 nur 3-5
  Pixel.
- Bezug "abgenommene Ankunftseinstellung" mit DEMSELBEN Werkzeug auf dem Abnahme-3-Lauf `F41a3/runs/L1a` (Stand da156ce6 = Neigung
  aus Nachbesserung 1 M5, Cut 1, Ausschnitt 112..146 x 112..146 wie das Dossier N1.M5): hellster Pixel **min 72 / max 108** = genau die
  abgenommenen Werte (Dossier N1.M5 "min 72, max 108"). Vergleichsbild `F41a4/key_vorher_nachher.png` (6-fach: abgenommen Cut 1 F60/F90 =
  geschlossener Ring, neu Cut 5 F58/F90 = verstreute graue Punkte), dazu `F41a4/L1a_key_zoom.png` (F20-F112).
- **F-E2 Punkt 2 ("mindestens so gut sichtbar wie in der abgenommenen Ankunftseinstellung (hellster Pixel Ruhe >= 72, Blink-Spitze >= 108,
  Kasten >= 10 x 9), gemessen"):** NICHT erfuellt - Ruhe 47 < 72, Spitze 70 < 108 (beide -35 %), Kasten je Bild hoechstens 9 x 12
  (Breite 9 < 10), in einem Drittel der Bilder 3 x 3 .. 5 x 5. Der Bau-Agent nennt selbst 47 -> 66 / 9 x 12 und hatte die Original-Lage
  von obj 3 mit 48 -> 70 bei 2-9 Pixeln als "kaum zu sehen" verworfen - die gewaehlte Lage ist nach seinem eigenen Mass nicht besser.
  Der Unit-Riegel `unit_r41_gatorszene_leiche` prueft nur `Cut 5: Primitivfarbe Ruhe 51 -> Blink-Spitze 76` und "flach dunkler" - die
  abgenommene Fassung hatte `Cut 1: Ruhe 84 -> Spitze 125`; ein dunklerer Schluessel faellt dem Riegel nicht auf. -> Mangel M2.

### 2.4 Fressen (Wortlaut "den Zombie fressen inklusive dem Schluessel"; F-E2 Punkt 3)
- gator_boss.log L1 (und E1 gleich): `RE2 KAMERA SZENE FINISHER RE2-Cut 7 -> Cut 4 F176 clip=4 bild=0 frontal=-1625 soll=4094`. Die
  Fress-Einstellung ist also von hinten (Frontalwert < 0). Bilder (Abschnitt 2.1): der Kopf liegt am linken Bildrand von Cut 4, die Leiche
  im Maul ist nur in einzelnen Bildern als Glied/Kleidung zu sehen; Biss/Schuetteln/Kauen selbst sind zu sehen, und die Leiche ist danach
  weg (`V F369`).
- **F-E2 Punkt 3 ("die Leiche im Maul ist zu sehen (Fress-Einstellung nicht von hinten, Frontalwert > 0)"):** NICHT erfuellt
  (frontal -1625). Der Kandidat mit positivem Frontalwert, Cut 5 (3933), scheitert an der Massstab-Grenze (1944 > 1892, B8); F-E2 nennt
  als Freiheitsgrad die Lage der Leiche. -> Mangel M3.

### 2.5 Urteil 1a: teilweise
Szenenfolge erhalten (Leon + Elza) und der Nutzer-Befund von Nachtrag 29 an der exe behoben (Chance zum Ausweichen: kein Biss vor 470 /
525 Bildern). Nicht erfuellt sind die drei Punkte des massgeblichen Entscheids F-E2: Kampfbeginn nicht beim RE2-Sollwert (M1), Schluessel
der Ankunftseinstellung dunkler und kleiner als abgenommen (M2), Fressen von hinten (M3); die Riegel haben keine obere Grenze (M4).

## 3. Punkt 1b — Schluessel nach dem Tod bei der Leiche, blinkend (Regression; Code seit Abnahme 3 unveraendert)

Rezept KW/KWE der Abnahme 3 (wortgleich, nur Bilder 186-300): `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
RE15_DEBUG_JUMP=2090@250 RE15_STATE_LOG=state.log RE15_R39_MESS_SPIELER_HP=100 RE15_GB_TEST=5 RE15_GATOR_KANISTER=4430,-26160
RE15_GIVE=3:15,21:50 RE15_EQUIP=3 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_SET_FLAG=9:110 RE15_INPUT_SCRIPT=W5,(M0.3,MA0.1)x12
RE15_EXIT_AT=300#2090 RE15_FRAMEDUMP=186-300/1:fd_` (Leon `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`, Elza `RE15_TITLE_CONFIRM_MS=1500
RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, 2091); Zwilling ohne Schluessel `RE15_SET_FLAG=9:110,9:112`. Der Tod ist in diesem Baum nur
ueber Spur B's Testhaken erreichbar (der echte Weg = Kanister-Szene der Spur G, Integration).
- KW (Leon, PL00): gator_boss.log `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F160`, `KAMERA R1 zurueck -> Cut 0 F196`, `R1 ende (Leiche)
  pos=(3413,-25001)`; debug.log `Folge R ROOM2090 F196 wert=1/0`, `Schluessel-Schnitt statt R1-Rueckschnitt: Explosions-Cut 1 -> Ausgangs-Cut
  0 F196`, `Schluessel-Lage: Kamera 2 ... Lage (1400,-1704,-23300) Tiefe 2643, 1001 von der Leiche ... Primitivfarbe Ruhe 80 / Blink-Spitze
  118`, `Water Key (Item 0x34, Bits (9,111)/(9,112)) bei der Leiche ... Zone Slot 15`, `Schluessel-Einstellung: Cut 2 (vorher Cut 0) F196`
  ... `Ende: Cut 0 F288`; state.log cam=1 F160, **cam=2 F196**, cam=0 F287. diffserie KW gegen KWo: fd 197-287 **91/91 Bilder, je 29 px,
  Kasten (32,192)-(44,206) = 13 x 15, hellster 70 -> 103**; fd 196 2 px; danach (Cut 0) 8 px am Bildrand.
- KWE (Elza, PL04, ROOM2091): dieselben Zeilen (`Folge R ROOM2091 F196`, Einstellung Cut 2 F196 ... Ende F288); diffserie KWE gegen KWEo:
  fd 197-287 91/91, 29 px, 13 x 15, 70 -> 103.
- = Abnahme 3 Bild fuer Bild; dazu integration_r41_gatorszene_tod/_tod_elza/_tod_r1/_tod_r1_elza/_tod_explosion/_tod_explosion_elza gruen
  (Abschnitt 1). Die einzige N4-Aenderung mit Wirkung hier ist die Ersatzlage nach dem Laden eines Spielstands (Rumpfmitte von obj 3,
  jetzt (-206,-7594) statt (-4594,-24306)) - ohne Einstellung, an der exe nicht ohne Spielstand nachstellbar (Hinweis H3).
- **Urteil 1b: erfuellt** (Leon + Elza; unveraendert seit Abnahme 3).

## 4. Punkt 2 — "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

- Code seit Abnahme 3 unveraendert (`re15_gatorszene_r41_spawn_weg`, Haken scd_vm.c op_sce_em_set nach dem Kill-Flag-Tor).
- exe, state.log ` t=25 ` (Typ 0x25): L1/E1/LW/EW 0/1739, L1a/E1a 0/428, KW/KWo/KWE/KWEo 0/713, G1b 0; debug.log je 2090/2091-Besuch zwei
  Zeilen `Spinne 0x25 bei (-100,-8296)|(-2300,-23896) verworfen` (L1/E1/LW/EW je vier = zwei Besuche).
- Riegel unit_r41_gatorszene_spinnen, unit_r38_spinnen, integration_r38_spinnen_echtlauf gruen (Abschnitt 1).
- **Urteil Punkt 2: erfuellt** (Leon und Elza).

## 5. Nachtrag 21 (Leiter statt Tor, Tor mit dem Schluessel) - Regression

- Leiter-Zugang: L1/E1/LW/EW (Abschnitt 2, `DOOR FIRE slot=1 ... spawn=(-8200,0,-25550)`).
- G1b (`RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110 RE15_PLAYER_POS=6630,-16350,0,1 RE15_PAD_AT=60:A RE15_MSG_LOG=1`): `ROOM2090 Slot 1
  (Tor) gesperrt: Ereignis 19, Bit (9,113) = 0`, `[msgtext] room=2090 id=42 "It's locked."`, `Folge Z ROOM2090 F60`.
- G2b (dto. + `RE15_GIVE=0x34:1`, A F60/140/...): `[msgtext] room=2090 id=43 "You've opened the lock."`, `Folge U F60`, `[aot] DOOR FIRE
  slot=1 rect=(7250,-16350,hw=500,hh=1750) target_cut=13 spawn=(4950,-3600,4100)`, `[tuer] Sequenz Archiv 2 DOOR26 ... Seite S164`,
  `EXIT_AT: Bild 120 in Raum 20a0`. Dazu unit_r41_gatorszene_tor, unit_r36_tueren_schott, unit_r30_tuer_verschlossen, integration_r36_tueren
  gruen. **Urteil: erfuellt.**

## 6. RE-Gate (Stichproben selbst disassembliert/gelesen, richtige Binaerdatei)

`re2_disasm.py --bin EM23_OVL_0000.BIN` (info/re2leon/COMMON/BIN, geladen @0x80100000), `re2_disasm.py` ohne --bin = info/re2leon/PSX.EXE,
`tools/r41_re2gator/re2_scd_walk.py` auf info/re2leon/PL0/RDT/ROOM40A0.RDT, `tools/scd_dump_room.py` + Python-Lesen auf
shared_assets/PSX/STAGE2/ROOM2090.RDT:

| Zitat im Diff (Nachbesserung 4) | selbst gelesen | Ergebnis |
|---|---|---|
| RE2 Intro Clip 6 @0x801008ac-b8, Cut 1 @0x801008c4-c8, z = -21212 @0x801008d0-d4, Clip 3 @0x801008e4-ec, Sperre +0x238 = 110 @0x8010091c-20, Sub 1 @0x80100928-2c, Gier += 1024 @0x80100938-48 | `801008ac addiu a1,zero,6` / `801008b4 jal 0x8001a330`; `801008c4 jal 0x8002c7ac` / `801008c8 addiu a0,zero,1`; `801008d0 addiu v1,zero,-21212` / `801008d4 sw v1,64(s0)`; `801008e4 addiu a1,zero,3` / `801008ec jal 0x8001a330`; `801008f8 addiu v1,zero,-17` + `80100910 addiu v1,zero,-257` auf 0x800cfbd8; `8010091c addiu v0,zero,110` / `80100920 sb v0,568(s0)`; `80100928 addiu v1,zero,257` / `8010092c sw v1,4(s0)`; `80100938 lhu v0,118(s0)` / `80100940 addiu v0,v0,1024` / `80100948 sh v0,118(s0)` | stimmt |
| Hysterese Sub 1 -> Sub 2 @0x80100b5c-64 / @0x80100b88, Abzaehlen @0x801004a8-b8, Sub 1 Clip 0 Blend 32 @0x80100c34-40 | `80100b5c lbu v0,568(s0)` / `80100b64 bne v0,zero,0x80100c08`; `80100b80 lw v0,496(s0)` / `80100b88 sltiu v0,v0,0x65` (dto. @0x80100ba4) -> `sw 0x201,4(s0)`; `801004a8 lbu v0,568(s0)` / `801004b4 addiu v0,v0,-1` / `801004b8 sb`; `80100c34 addu a1,zero,zero` / `80100c38 addiu a2,zero,32` / `80100c3c jal 0x8001a330` | stimmt |
| Wurzelschritt FUN_8001a330 (RE2 PSX.EXE) @0x8001a360/@0x8001a374/@0x8001a3b0/@0x8001a3c0 | `8001a360 sb zero,333(s0)` (Bild 0 beim Clipwechsel), `8001a374 jal 0x80015e7c`, `8001a3a8 jal 0x8002959c`, `8001a3b0 bne v0,zero,0x8001a3cc` (Clip-Ende: kein Schritt, Rueckgabe 1), `8001a3c0 jal 0x800152c8` | stimmt |
| ROOM40A0 SCD: sub11 `Pos_set 32 00 cd 11 f8 f8 c3 b8` @0x0736, Ausloeser sub02 @0x00EE, Sce_em_set @RDT 0x25A0 Gier 0x400; Leon sub03 Pos_set @0x0250, Warten auf Cut 1 @0x025E-6A, Sleep @0x0276/@0x027E, `Plc_dest 40 00 08 0a d0 d5 38 9b` @0x0286, @0x029E, Plc_ret @0x02B6, Set @0x02B7 | `0736 Pos_set 3200cd11f8f8c3b8` = (4557,-1800,-18237); `00EE Evt_exec 04ff180b` (startet sub11); SCD-init @0x2500 + `00A0 Sce_em_set 440000230000011600004a16b80b3ceb0004...` = Typ 0x23, Gier 0x0400; `0250 Pos_set 320080daf8f8389b` = (-9600,-1800,-25800); `025E While` + `0262 Cmp 23001a050100`; `0276 Sleep 090a3c00`, `027E Sleep 090a3200`; `0286 Plc_dest 4000080ad0d5389b` = (-10800,-25800); `029E Plc_dest 4000150a00088000`; `02B6 Plc_ret`, `02B7 Set 22020700` | stimmt |
| RE1.5 ROOM2090: obj 3 `2d 03 00 00 02 00 0a 00 00 00 e7 ff 34 f9 01 e3 00 00 78 06` @0x00A8E, obj 2 @0x00A6C, Wippen sub03 @0x00B5E, Cut 5 RID @0x060 + 5 * 32, Lichtsatz @0x468 + 40 * 5 | `0x00A8E Obj_model_set 2d 03 00 00 02 00 0a 00 00 00 e7 ff 34 f9 01 e3 00 00 78 06 ...` = (-25,-1740,-7423) Gier 1656; `0x00A6C ... 43 f9 34 f9 25 ab 00 00 ad 02` = (-1725,-1740,-21723) Gier 685; `0x00B5E` sub03 `Work_set 2e 03 03` + Speed_set/Add_speed-Schleifen; RID @0x100 pos (2754,-3132,-6408) -> (-7992,-18,-10782); Lichtsatz @0x530 Umgebung `3f 49 3f` = 63/73/63 | stimmt |

- RE2-Sollwert: die Rechnung (Wurzel-Ende (503,-24100), Leon (-10800,-25800) -> dx 11303 / dz 1700 -> 11430) ist mit den gelesenen Stellen
  stimmig (Clip 3 verschiebt bei Gier 0x400 x um -4042 und z um -2884; z vorher fest -21212). Die EMR-Keyframe-Versaetze selbst (aus
  shared_assets/RE2/CDEMD0.EMS ueber den Port-Lader) habe ich nicht unabhaengig nachgerechnet; der Abstand 17539 liegt weit ausserhalb jedes
  solchen Fehlers. "121 Bilder" ist ein geometrisches Kontakt-Mass (vorderster Knochen auf Leons x), der Port misst Clip 4 / HP-Abfall -
  fuer eine obere Grenze (M4) muss auf beiden Seiten dasselbe Mass stehen (Hinweis H2).
- Konstanten im N4-Diff: RE15_GSZ_LEICHE_OBJ 3 (@0x00A8E), GSZ_CUT_LEICHE 5 (RID @0x060 + 5 * 32), GSZ_LEICHE_X/Z/ROT_Y, GSZ_GATOR_GIER,
  GSZ_KEY_AUF_RX/RZ - alle als PORT-WAHL der Inszenierung nach NUTZER-VORGABE Nachtrag 6/29 gekennzeichnet, mit Herleitung (leiche_lage_n4.py)
  und den Original-Bytes von obj 3 im Kommentar. Keine neue Verhaltenskonstante ohne Beleg.
- Suche in den `+`-Zeilen von `git diff cc4a4646..877919e2 -- re15_port` nach deferred/tunable/interim/for now/faithful/plausib/TODO/vorerst/
  geschaetzt/geraten/ungefaehr/approx/FIXME/HACK/nach Gefuehl: ein Treffer, verneint ("keine Zahl nach Gefuehl", probe_re2_intro_ende.c);
  ueber den ganzen F-Diff 0. Kein getenv in gatorszene_r41.c / gatorszene_r41_pc.c / re15_gatorszene_r41.h; kein Env-Schalter als Abschluss.
- Erklaert der Fix den Befund von Nachtrag 29? Ja: die Kette endete an der Leiche (-5500,-23700) rund 3800 von der Leiter, also begann der
  Kampf mit der Gator-Wurzel 6162 vor Leon; mit der Kette im Nordbecken beginnt er 17539 entfernt (gemessen, Abschnitt 2.2). Er ueberschiesst
  aber den RE2-Sollwert, den F-E2 als Ziel setzt (M1).

## 7. Vertrag

- Bits nur (9,110..113) (VERTRAG 1.1; test_r36_kartenbits.c k_port[] unveraendert), Nachrichten 2090/2091 Ids 42/43 (zugeteilt 42-46),
  Ereignisse 18/19/20, AOT-Plaetze 15/20 - Nachbesserung 4 belegt nichts Neues (der Code-Diff aendert nur Konstanten).
- Pfade `git diff master...HEAD --name-only` (60 Dateien, Merge-Basis 9f10bdb0): keine release/, platform/android/, shared_assets/PSX/, pri/,
  synchro/, kein CMakeLists.txt (grep leer). Nachbesserung 4 aendert nur F-eigene Dateien und `tests/integration/test_r41_re2gator.cmake` der
  Spur B (Lauf szene EXIT_AT 420 -> 440, Kommentar Querbezug (e) mit Nachtrag 29; integration_r41_re2gator steht in Entscheid F-E1 als von F
  nachzuziehender Riegel; Pruefungen unveraendert, gruen 264.66 s). Spur B's Modul nicht geaendert.
- Riegel: neu unit_r41_gatorszene_abstand/_abstand_elza und integration_r41_gatorszene_flucht/_flucht_elza, nachgezogen leon/elza/leiche/
  programme/szene mit Beleg (Nachtrag 29). Sie messen (Gegenprobe Abschnitt 1), aber nur nach unten begrenzt (M4) und ohne Untergrenze fuer die
  Schluessel-Sichtbarkeit (M2). Parallel-fest (Laufordner `lauf_<zufall>`, exe-Kopie je Lauf), Linux-Muster wie bisher
  (`cmake_minimum_required(VERSION 3.21)`, `_DEFAULT_SOURCE`, `m`).
- Nebenwirkungen: 221 gezielte Tests gruen; Leon = Elza in allen Laeufen (Bild fuer Bild gleiche Bildnummern); Nachbarraeume 2080 (Montage
  ueber die Szene, `M ROOM2080 F142 wert=8/2`) und 20A0 (Tor, G2b) gemessen. Querbezug G: Kampfbeginn jetzt F422 mit dem Gator im Nordbecken
  (-2178,-9077) - G's Kampf-/Kanister-Riegel liegen nur im G-Baum (Integration).

## 8. Maengel (nummeriert, nachpruefbar; Reihenfolge nach F-E2: 1 vor 2 vor 3)

- **M1 Kampfbeginn nicht beim RE2-Sollwert (Entscheid F-E2 Punkt 1, Leon und Elza).** Gemessen (Abschnitt 2.2, Laeufe F41a4/runs/L1, E1, LW,
  EW): Kampfbeginn F422, Abstand Gator-Wurzel <-> Spielfigur 17539 (RE2 11430 = 1,53-fach), erster Biss stehend nach 470 Bildern (RE2 121 =
  3,9-fach), erster Treffer nach 477; sofort nach Osten rennend Biss nach 525 / Treffer nach 532. Davon 300 Bilder Zonen-Dash (Clip 2 F591 ->
  Clip 3 F741 -> Clip 0 F891). Soll: so nah wie moeglich an 11430 / 121, nicht darunter; liegt es am Zonen-Dash, die Lage mit der
  Zonenbeziehung von RE2 am Intro-Ende waehlen (in RE2 belegen); Abweichungen mit Zahlen im Dossier. Pruefen: `kampf_a4.py` bzw. r41f_kampf
  auf state.log/debug.log eines Leiterlaufs (stehend und weglaufend, Leon + Elza).
- **M2 Schluessel der Ankunftseinstellung dunkler und kleiner als abgenommen (F-E2 Punkt 2, Leon und Elza).** Gemessen (Abschnitt 2.3, L1a/E1a,
  Cut 5, keypix.py Ausschnitt 150..186 x 108..135): hellster Pixel Ruhe 47 / Spitze 70, 2..16 Pixel je Bild, Kasten je Bild hoechstens 9 x 12
  (in einem Drittel der Bilder 3 x 3 .. 5 x 5). Abgenommen (dasselbe Werkzeug auf F41a3/runs/L1a, Cut 1): 72 / 108. Soll: Ruhe >= 72,
  Spitze >= 108, Kasten >= 10 x 9, gemessen; der Riegel `unit_r41_gatorszene_leiche` muss das (oder sein Aequivalent an der Primitivfarbe,
  abgenommen `Ruhe 84 -> Spitze 125`) als Untergrenze pruefen - heute verlangt er nur `Ruhe 51 -> Spitze 76` und "flach dunkler".
- **M3 Fress-Einstellung von hinten (F-E2 Punkt 3, Leon und Elza).** gator_boss.log `RE2 KAMERA SZENE FINISHER RE2-Cut 7 -> Cut 4 F176 ...
  frontal=-1625`; in Cut 4 ist die Leiche im Maul nur in einzelnen Bildern als Glied/Kleidung zu sehen (fd 256-310), in fd 184-238 gar nicht
  (F41a4/L1_fressen_zoom.png, L1_fressen_zoom2.png). Soll: Leiche im Maul sichtbar, Frontalwert > 0, B8 bleibt; Freiheitsgrad ist die Lage
  der Leiche (Nachtrag 6). Pruefen: FINISHER-Zeile in gator_boss.log + Bildbogen der Fress-Einstellung.
- **M4 Riegel ohne obere Grenze (F-E2, letzter Satz).** `unit_r41_gatorszene_abstand/_abstand_elza` (`ab >= soll_ab`, `an >= soll_bi`) und die
  exe-Auswertung r41f_kampf der Riegel `integration_r41_gatorszene_flucht/_flucht_elza/_leon/_elza` (`_ab2 LESS re2_ab2` -> FEHLER, Biss/Treffer
  `LESS_EQUAL _b_ende` -> FEHLER) begrenzen nur nach unten - 17539 / 470 gilt als gruen. Soll: Sollwert RE2 mit oberer UND unterer Grenze aus
  dem RE2-Beleg, auf beiden Seiten mit demselben Mass (Hinweis H2).

Hinweise (keine Maengel):
- H1 Der Nutzer-Befund von Nachtrag 29 selbst ist an der exe behoben (Ausweichen moeglich: kein Biss/Treffer vor 470 bzw. 525 Bildern, kein
  Treffer waehrend des Rennens, Leon + Elza); gemessen nur stehend und "sofort nach Osten" (dem einzigen freien Weg von der Leiter).
- H2 RE2-Sollwert: Adressen/Bytes selbst gelesen (Abschnitt 6), die EMR-Versaetze nicht unabhaengig nachgerechnet. "121" misst den
  vordersten Knochen auf Leons x (Kontakt-Mass), der Port den Biss (Clip 4) bzw. den Treffer - fuer eine obere Grenze vergleichbar machen.
- H3 Ersatzlage nach dem Laden eines Spielstands jetzt die Rumpfmitte von obj 3 (-206,-7594) im Nordbecken - weiterhin ohne Einstellung,
  an der exe nicht ohne Spielstand geprueft (unveraendert offen seit Nachbesserung 2).
- H4 Im Cut 0 des Kampfbeginns treibt obj 2 (Original-Lage @0x00A6C, ohne Schluessel); die Schluessel-Leiche ist jetzt obj 3 (Nachtrag 6
  erlaubt die Lage). Integration F+G: Kampfbeginn F422, Gator-Start (-2178,-9077) - G's Riegel im Integrationsbaum neu messen.
- H5 Unveraendert offen (Bau-Agent): MAIN2E nach dem Tod, PSX-Bau ungeprueft, Schachtspinnen 20A0, Nachrichten-Ids 44-46 frei, Form
  "Kameraschnitt auf den Schluessel nach dem Tod" ohne Nutzer-Bestaetigung.

## 9. Urteile

| Punkt | Urteil | Beleg |
|---|---|---|
| 1a Ankunftsszene (Leiche + blinkender Schluessel, Knall, 2080 oben, erneut Leiche, Alligator aus dem Wasser frisst Leiche samt Schluessel, Schrei/Schuetteln zu Leon, RE2-Kampfmusik; Nachtrag 29 + Entscheid F-E2) | teilweise | 2: Folge L1/E1 vollstaendig (C F422, Cut 0, MAIN1A), Nachtrag 29 behoben (17539, Biss +470/+525); aber F-E2 1-3 offen: 17539/470 statt 11430/121 (M1), Schluessel 47 -> 70 statt >= 72 -> 108 (M2), Fressen Cut 4 frontal -1625 (M3) |
| 1b Schluessel nach dem Tod bei der Leiche, blinkend | erfuellt | 3: KW/KWE Folge R F196, Einstellung Cut 2 F196-F287, 91/91 Bilder, 29 px, 13 x 15, 70 -> 103 (= Abnahme 3), tod-Riegel gruen |
| 2 Spinnen aus 2090 entfernen | erfuellt | 4: 0 t=25-Zeilen in allen 2090/2091-Abschnitten, zwei "verworfen"-Zeilen je Besuch, Leon + Elza |
| Nachtrag 21 Leiter statt Tor, Tor mit dem Schluessel | erfuellt | 5: Leiter-Ankunft, G1b "It's locked.", G2b "You've opened the lock." + Tuer nach 20A0 |
| RE-Gate | erfuellt | 6: 5 Stichproben-Bloecke (EM23 Intro/Hysterese, RE2 FUN_8001a330, ROOM40A0-SCD, ROOM2090-RDT) stimmen, Konstanten gekennzeichnet, keine Rate-Woerter, Fix erklaert den Befund |
| Vertrag/Riegel | teilweise | 1/7: 221/221 gruen, nur erlaubte Pfade und zugeteilte Ressourcen, neue Riegel messen nach unten; aber ohne obere Grenze und ohne Schluessel-Untergrenze (M4, M2) |

**Ergebnis: NICHT bestanden.** Nachtrag 29 ist an der exe behoben (Leon und Elza, Riegel messend), aber der vier Minuten nach dem
Abschluss-Commit eingegangene, massgebliche Entscheid F-E2 ist nicht umgesetzt: M1 (Kampfbeginn beim RE2-Sollwert), M2 (Schluessel nicht
dunkler als abgenommen), M3 (Fressen nicht von hinten), M4 (Riegel mit oberer und unterer Grenze).
