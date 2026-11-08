# Runde 41 - Spur G2 "gator_re2" - UNABHAENGIGE ABNAHME 5 (2026-11-07): G2-RE2 NB3 (Vorgabe A "oben nicht sicher", Vorgabe B "kein Haenger")

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Kopf 6b9f7dac4 (NB3: a7415b225 Bau A + B, 8bfcf864c Riegel nb3 / obenlog / haenglog / rampe_oben,
466901ffe Figur im Block, d3762d778 obenlog ab dem Anhaengen, bb2803202 Patch 9 + Fenster re2gator, 6b9f7dac4 Dossier/Belege; Messblatt e92356e8e).
Pruefer: Abnahme-Agent, glaubt dem Bau nichts, misst selbst. Kein Spielcode geaendert. Zweiter Versuch dieser Abnahme: der erste brach am Modell-Limit ab; dessen
Scratch-Reste (`g2nb3/abn5/`) sind nicht verwendet, alles hier ist neu gemessen.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn5r2`
(je Lauf `runs/<lauf>/` mit state.log, climb.log, gator_trag.log, gator_boss.log, debug.log, env.txt, pid.txt, exe_md5.txt, fd_NNNNNN.ppm; Harness `harness/<lauf>/`).
Belege: `G2_belege/abn5/` (`messung_abnahme5.txt` = alle Auswertungen, `harness/` + `harness_vergleich.txt`, `ctest_*.txt`, 5 Bildleisten, `werkzeug/`).

Spezifikation: Nutzerentscheid 2026-11-06 ~21:15 (Dossier G2_gatorfix.md "G2-RE2 NB3", woertlich) - Vorgabe A: "Er soll nur den Spieler oben auf der Rampe
auch rammen koennen, das der Spieler oben nicht ausweichen kann. Alternativ kann er auch hoch beissen oder sowas, aber er darf halt oben nicht sicher sein." (Takt
frei: "Das kann er gerne machen."); Vorgabe B: "Wenn er haengt muessen wir nachbessern denke ich. Er darf nicht zu lange haengen. Wenn es kurz ist, kann man mit
dem tauchen abhilfe schaffen, aber das hilft natuerlich nicht bei der Platform."; F1 "noe" (keine Kameragrenze), F2 Querungsbeginn unveraendert. Alles andere ohne
Kanister bleibt der gelobte Stand (a661eb8aa als Vergleich). Riegel des Messblatts G2_MESSBLATT_NB3.md Abschnitt 4 (A1, A2, B1, B2, B3, B4).

## 0. Urteil

**NICHT BESTANDEN.** Vorgabe A und B sind im Kern umgesetzt und messbar besser als a661eb8aa (Abschnitt 3), aber es gibt drei blockierende Befunde. Alle drei
fand der Bauer nicht: zwei liegen in exe-Riegeln, die er nicht gefahren hat (er fuhr 8 der 18 exe-Riegel), einer in Oben-Lagen, die seine Riegel nicht abdecken
(Harness haelt die Figur je Bild fest, exe-Szenario rampe_oben trifft den Fall zufaellig nicht).

- **M1 Absturz (blockierend).** exe-Riegel `integration_r41_gatorfix_abn2_wand` rot: der Lauf ohne Kanister endet mit `AUSNAHME 0xC0000005 ... Adresse +0x17c2f4`
  (exit 139) in F7374. Zweimal unabhaengig gemessen (ctest-Gruppe und eigene Messkopie, gleiche Adresse, gleiches Bild): `re15_squareroot0`+0x90, gerufen aus
  `re15_autolook_scan` <- `re15_game_step` (nm/addr2line auf die exe), Leon steht dabei bei (90674,0,-96244), rund 100 000 Einheiten ausserhalb des Raums. Weg dorthin:
  F1045 Biss im Band vor der Rampe (Vorgabe B, neu) -> F1046 Leon in der Ostnische (7668,-17081) -> F1500 steht er ohne Eingabe in einem Bild 2000 weiter hinter
  der Nischen-Rueckwand (9668,-16031) -> die blinde Eingabe laeuft ihn ins Leere. Alter Stand a661eb8aa (exe d872c8e6, eigener Lauf, gleiche Eingabe): Leon nie in
  der Nische, nie ausserhalb, exit 0 bis F8400. Mit Kanister (neu) ebenfalls ab F1773 bis Laufende ausserhalb (140743,-127360), ohne Absturz.
- **M2 Wurf aus dem Raum durch die Angriffe oben (blockierend, Vorgabe A).** Ein Treffer oben (CROSS-Biss oder Rammen der Vorgabe A) wirft die Figur im Niederwurf
  auf Rampenhoehe (y -1800) quer ueber das Becken und durch die Aussenwand; sie bleibt bis Laufende ausserhalb (2775-2986 Bilder = 92-100 s) und wird nie mehr
  getroffen - oben ist sie danach dauerhaft "sicher", und sie steht ausserhalb des Levels. Gemessen in 4 von 11 eigenen Laeufen mit Figur oben: west5_elza
  (Abnahme-4-Lauf, mit = ohne Kanister bitgleich: F2752 CROSS-Biss an (2315,-17642) -> F2765 durch die Suedwand, bis (2318,-1800,-29914), 536 Bilder bis
  Laufende), eigene Oben-Lagen ohne Kanister Leon und Elza (2318,-16300) (F1201 -> Nordwand, (2318,-1800,-2694)) und Elza (6500,-15000) (Rammen F1209 ->
  Nordwand, (6740,-1800,2580)). Mechanik in Abschnitt 4. Der Defekt ist nicht neu (Abnahme 4: neu_west4_leon mit Kanister F2982-F3300 ausserhalb, damals nicht
  gemeldet), aber NB3 macht ihn haeufig und loest ihn jetzt auch ohne Kanister aus (a661eb8aa west5_elza: nie ausserhalb).
- **M3 exe-Riegel `integration_r41_gatorfix_abn2_rampe_west` rot (blockierend als roter Riegel, kein Spielabsturz).** Der Lauf mit Kanister erreicht
  `RE15_EXIT_AT=8000#2090` nicht (900-s-Grenze): die Vorgabe-B-Bisse im Band werfen Leon im Niederwurf quer durchs Suedbecken (F231-F236: 5600 Einheiten in 6
  Bildern) bis an die SW-Tuer (-8432,-26532); das blinde Skript drueckt dort Aktion (F934), `DOOR FIRE slot=0` F937 -> ROOM2080. Eigener Nachlauf mit Abbruch Bild
  1300: neu Tuer + Exit in 2080, alter Stand bleibt in 2090 (Exit F1300 in 2090).

| Punkt | Soll | Ist (eigene Messung) | Bewertung |
|---|---|---|---|
| A Rampe oben nicht sicher (Leon, Elza, mit/ohne) | Treffer oben (Rammen/Hochbiss), Takt frei; Messblatt A1/A2: erster <= 600, Pause <= 600 | Harness 81 Rampenlagen (18 Riegel + 63 eigene): 0 ohne Treffer, erster spaetestens t450, Pause max 537, Rammen ohne Kanister; exe: in allen 11 Oben-Szenarien getroffen, erster Treffer 83-402 Bilder nach Kampfbeginn bzw. erstem Oben-Bild | im Kern erfuellt |
| A ohne Rueckschritt | oben bleibt im Raum | M2: 4 von 11 Oben-Laeufen enden ausserhalb des Raums (Wurf), danach 0 Treffer | **nicht erfuellt (blockierend)** |
| A2 Pause oben <= 600 (Messblatt) | keine Pause > 600, solange oben | Bauer-Szenario ab erstem Oben-Bild 856 / 854 (Kanister holen, F483-F1339); eigene Lagen 632-849 (Klemmlage (1382,-14013), Umweg West); ab dem Anhaengen 343 | Rest, Nutzerfrage (Takt frei) |
| B kein Haenger vor Plattform-/Rampenkante | Haenge-Fenster 0, ohne Fortschritt <= 450, kein Rueckfall | Harness 54 eigene Bandlagen + 11 Riegel-Lagen: 0 Fenster (alt 86 in 11 Lagen); exe 5 eigene Kanten-Laeufe + 10 Abnahme-4-Laeufe: 0 Fenster, max 297 Bilder, 0 HAENGT/RUECKWAERTS/TAUCHEN | erfuellt |
| B Plattform-Westkante (neu gemessen) | dto. | alter Stand: 0 Treffer in 3600 Bildern, 18 Fenster (exe) / 22-23 (Harness); neu: 24 Bisse (exe) / 306-390 (Harness), 0 Fenster | erfuellt (Verbesserung) |
| Bitgleich ausserhalb der beiden Orte | ohne Kanister wie a661eb8aa, ausser oben / im Band | 10 Abnahme-4-Laeufe: Titel, erster 2090-Besuch, 2080 bitgleich; Rueckkehr bitgleich bis die Figur ins Band tritt (frei4_leon F1492, frei4_elza F4104, west* ab F1); Harness 9 von 13 Fernlagen 3600/3600 bitgleich, 4 weichen ab, wenn der Koerper-Stoss die Figur ins Band schiebt | erfuellt |
| Kanister-Sitz / Rueckfall | Sitz 0, Tauchen nur Rueckfall | Sitz max 0 in allen Laeufen mit Kanister (2520-7830 Bilder); 0 Zeilen RUECKWAERTS an / TAUCHEN an in allen eigenen Laeufen | erfuellt |
| Absturz | keiner | M1: exe-Riegel abn2_wand, Access violation F7374 (reproduziert) | **nicht erfuellt (blockierend)** |
| Riegel unit_r41_gatorfix_* | gruen | `100% tests passed, 0 tests failed out of 26` | gruen |
| exe-Riegel integration_r41_gatorfix_* / re2gator (18) | gruen | siehe Abschnitt 2 (abn2_wand und abn2_rampe_west rot) | **rot** |
| Konstanten | @0x..., Messung oder PORT-WAHL + Wortlaut | alle Zahlen belegt (Abschnitt 7); formal: Kommentar `NB3_OHNE_MAX 450` ohne "PORT-WAHL"/Nutzer-Wortlaut | erfuellt mit formalem Rest |

## 1. Bau, Messweg

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`. exe
  `re15_port/build/platform/pc/re15_pc.exe` 2026-11-07 01:04:01, md5 029efac644499f92ef0a5572ee3aa43d (= Bauer-exe nach Patch 9); juengste Quelle
  enemy_ai_boss_gator.c 01:03:49 (Patch 9 lag vor dem Bau im Arbeitsbaum, Commit bb2803202 02:16), re15_boss_gator.h 22:43, Tests 23:55; Kopf 6b9f7dac4 aendert
  nur Dossier/Belege. Arbeitsbaum sauber.
- exe-Laeufe: `werkzeug/lauf5.sh` (Messkopie `re15_pc_g2abn5.exe` = Kopie der Bau-exe; fuer den alten Stand `re15_pc_g2abn5_alt.exe` = Kopie von
  `g2nb3/exe_alt/re15_pc_alt_a661eb8aa.exe`, md5 d872c8e6 = Abnahme-4-exe), `batch5.sh` (Laufdefinitionen), `alle5.sh` (2-3 parallel, nur eigene PIDs, kein
  taskkill). Kein gdigrab; Bilder nur als RE15_FRAMEDUMP-Serie (F = Raum-Bildzahl, wie state.log). 40 eigene exe-Laeufe (38 Laufordner; der Probelauf und die
  Determinismus-Wiederholung haben je einen Namen ueberschrieben), alle exit 0 bis RE15_EXIT_AT, ausser dem Absturzlauf ohne_abn2wand_leon (exit 139). Determinismus: ohne_west5_elza zweimal gefahren, alle Kennzahlen gleich; Kamera-Unabhaengigkeit: Westkanten-Lauf in
  Cut 0 und Cut 7 mit gleichen Zahlen.
- Lauf-Arten: (c) die Abnahme-4-Laeufe west4_leon / west5_elza / west4_elza / frei4_leon / frei4_elza (Eingaben md5 96797cb6 / 14cd3db6 / 40e23a92, Umgebung
  wortgleich `G2_belege/abn4/werkzeug/batch2.sh`) je mit Kanister ("neu") und Messhaken ohne Kanister ("ohne"), Vergleich gegen die Abnahme-4-Laeufe der alten exe
  (`g2abn4/runs/`); (a) das Szenario des exe-Riegels rampe_oben (Leon/Elza, mit/ohne) und eigene Oben-Lagen: Figur per `RE15_PLAYER_POS=x,z,1024,1` (Band 1 =
  y -1800) direkt oben, Szene gesehen + Tor offen (`RE15_SET_FLAG=9:110,9:113`), Kampf sofort (`RE15_GB_TEST=1`, wie die re2gator-Laeufe), RE2-Intro bis F280; ein
  liegender Kanister (`RE15_GATOR_KANISTER=2500,-10000`) wurde in diesem Aufbau nie aufgenommen - diese Laeufe zaehlen als "ohne"; (b) eigene Rueckkehr-Lagen im
  Wasser (`RE15_K2080_RUECK_LAGE`) vor Plattform-West-, Plattform-Sued- und Rampen-Suedkante, Figur steht; dazu abn2_wand und abn2_rampe_west mit neuer und alter exe.
- Auswerter (eigene, nicht die des Bauers): `mass5.py` (A: Oben-Abschnitte, Treffer = hp-Flanke, Art aus RAMMEN-Zeile / TRAG-Phase / Clip, laengste Pause ab dem
  ersten Oben-Bild UND ab dem Anhaengen; B: Haenge-Fenster nach gator_rw_r41.c (b), ohne Fortschritt nach mass_v4.py, laengste Pause ohne Treffer, Rueckfall-Zeilen;
  Kanister-Sitz aus debug.log), `aussen5.py` (Figur ausserhalb der SCA-Aussenwaende x -8900..7200, z -27000..-5400, Ostnische getrennt), `vergleich5.py` (Bild fuer
  Bild je Besuch: Figur + Gator-Block + TRAG, erste Abweichung mit Ort der Figur). Gegenrechnung: meine Zahlen fuer das Bauer-Szenario rampe_oben sind gleich seinen
  oben3.py-/obenlog-Zahlen (856 / 343 / 20 Treffer).
- Harness: `test_r41_gatorfix.exe nb3_oben` / `nb3_haengt` (Messwerkzeuge des Messers) mit eigenen Lagen-Listen, neu und alter Stand (`G2_NB3_ALT=1` =
  Pruefhaken a661eb8aa), Spur je Bild (`G2_NB3_SPUR`), `werkzeug/harness5.sh`; dazu `test_r41_gatorfix.exe nb3` direkt (SUMME-/ok-Zeilen gleich dem Bauer-Beleg
  `nb3/bau/riegel_nb3.txt`).

## 2. Riegel (Summenzeilen woertlich)

- Harness: `ctest --test-dir re15_port/build -R "^unit_r41_gatorfix_" -j 2 --timeout 900` (09:50-09:51): **`100% tests passed, 0 tests failed out of 26`**,
  `Total Test time (real) =  52.87 sec` (`ctest_unit.txt`). Direkt `test_r41_gatorfix.exe nb3`: `nb3: 0 Fehler`, SUMME A1 0/18 ohne Treffer, erster t450, Pause 537,
  Rammen 36; B1 Fenster 0, ohne Fortschritt 327, Pause 520/530; Gegenproben a661eb8aa rot wie behauptet (R2318/-16300 kein Treffer, W4200/-18800/c2 3 Fenster,
  Gang-Lagen 579/900 und 29/900 abweichend).
- exe-Riegel: `ctest --test-dir re15_port/build -R "^integration_r41_gatorfix_|^integration_r41_re2gator" -j 2 --timeout 1800 -V` (ab 09:51, parallel zu meinen
  Laeufen und fremden Agenten, 09:51-10:57): **`89% tests passed, 2 tests failed out of 18`**, `Total Test time (real) = 3970.21 sec` (`ctest_exe.txt`).
  `The following tests FAILED: 1186 - integration_r41_gatorfix_abn2_rampe_west (Failed)`, `1189 - integration_r41_gatorfix_abn2_wand (Failed)`. Gruen: abn2_wand_elza
  688,74 s, frei_elza 703,77, abn2_rampe_west_elza 659,10, frei 642,69, abn2_leiterecke 616,73, abn2_ecken 572,75, abn2_nordbecken 553,35, rampe_oben_elza 391,75,
  rampe_oben 368,40, rampe_elza 346,03, rampe 362,19, gang_montage_elza 61,84, gang_szene_elza 45,90, re2gator 340,12, gang_szene 33,98, gang_montage 41,15. Rot:
  abn2_wand 606,13 s (`r41_gatorfix: Lauf abn2_wand - exe (ohne Kanister) exit=Access violation (erwartet 0 bis RE15_EXIT_AT)`), abn2_rampe_west 900,22 s (`exe
  exit=Process terminated due to timeout (erwartet 0 bis RE15_EXIT_AT)`). Der Bauer fuhr nur 8 dieser 18 (rampe, rampe_elza, abn2_wand_elza, rampe_oben,
  rampe_oben_elza, frei, frei_elza, re2gator); die beiden roten sind unter den 10 nicht gefahrenen. Riegel-Meldungen der gruenen (wortgleich zum Bauer): rampe_oben
  `obenlog: ... Kanister angehaengt F1013 (gezaehlt ab dann, davor 930 Bilder oben) ... oben 18 (Rammen 5, CROSS-Biss 4, Hochbiss 9 ...); erster Treffer oben nach
  326 Bildern (Rammen F1339); laengste Pause ohne Treffer oben 343 Bilder ... -> ok (Schranke 600)` (mit und ohne Kanister gleich); rampe `haenglog: ... Haenge-Fenster
  0 ...; ohne Fortschritt max 16 Bilder = 0.5 s ...; Treffer 31; Zeilen HAENGT 0, RUECKWAERTS an 0, TAUCHEN an 0 ... -> ok`; re2gator `kanister: 240 angehaengte Bilder,
  Abweichung max 0`, belagerung `GIER-WACHE bis F581: 300 Bildpaare CHASE/GUARD`.
- Rote Riegel einzeln nachgefahren (`ctest ... -R "^integration_r41_gatorfix_abn2_(wand|rampe_west)$" -j 2 --timeout 1800 --output-on-failure`, 10:57-11:13,
  `ctest_exe_einzeln.txt`): **`0% tests passed, 2 tests failed out of 2`**, `Total Test time (real) = 914.30 sec` - abn2_wand 642,17 s (`exe (ohne Kanister)
  exit=Access violation`), abn2_rampe_west 914,26 s (`exe exit=Process terminated due to timeout`). Mit der Gruppe und den eigenen Messkopie-Laeufen (abn2_wand ohne:
  exit 139 in F7374, gleiche Adresse; abn2_rampe_west: Tuer F937 -> 2080) je dreimal rot, deterministisch - kein Last-Flackern.

## 3. Eigene Messung

### 3a Vorgabe A - Figur oben (Rampe)

Harness (eigenes Raster 9 x 7 = 63 Rampenlagen x 3600 Bilder, x 1950..7100, z -18000..-14600; `harness/oben_rampe_*.txt`): neu mit und ohne Kanister und
Elza = Leon: **0 von 63 Lagen ohne Treffer, spaetester erster Treffer t450, laengste Pause 537** (R1950/-16300 und R2318/-16300); Rammen auch ohne Kanister
(Mittelband z -16300 bei x <= 3800: 6-7 je Lage). Alter Stand: 4 Lagen ohne Treffer (R1950/R2318/R2600/R3000 bei z -16300), 11 Lagen mit Pause > 600. Plattform (24 Lagen): auf Band 1
nicht begehbar (`begehbar 2090 1`: (500,-15500) -> (2318,-15500), (0,-16000) -> (-2168,-16000) weggeschoben) - nur informativ (3 Lagen Pause > 600, bis 3099).

exe, Szenario des Riegels rampe_oben (Klettern F48-F82, oben ab F83; Kanister liegt nach der Szene im Becken), mit = ohne Kanister bitgleich (state + TRAG):

| Lauf | Treffer oben (Art) | erster Treffer ab erstem Oben-Bild | laengste Pause ab erstem Oben-Bild | ab Anhaengen (F1013): erster / Pause | ausserhalb |
|---|---|---|---|---|---|
| Leon (mit = ohne) | 20 (Rammen 6, CROSS-Biss 4, Hochbiss 9, 1 sonst) | 300 (Rammen F383) | **856 = 28,5 s** (F483-F1339, Gator holt den Kanister) | 326 / 343 | nie |
| Elza (mit = ohne) | 20 (dto.) | 300 (Rammen F383) | **854** (F483-F1337) | 324 / 343 | nie |

Eigene Oben-Lagen (Figur steht ab F1 oben, RE2-Intro bis F280, Kampf ab F281; nie ein Kanister im Maul):

| Lage, Figur | erster Treffer (nach Kampfbeginn) | Treffer oben | laengste Pause oben | Ende |
|---|---|---|---|---|
| (2318,-16300) Leon | F643 (362) CROSS-Biss | 4 (Rammen F735, Hochbiss F840, F1201) | 362 | **F1201 Wurf nach Norden aus dem Raum**, 2986 Bilder ausserhalb, 0 Treffer |
| (2318,-16300) Elza | identisch (Leon = Elza) | 4 | 362 | **F1201 Wurf, 2986 Bilder ausserhalb** |
| (2800,-16300) Elza | F622 (341) | 28 | **698** (F848-F1546) | im Raum; ab F1546 Hochbiss alle ~105 Bilder in der Klemmlage (1382,-14013) |
| (2318,-17600) Leon | F422 (141) | 28 | **849** (F619-F1468) | im Raum; Klemmlage (1382,-14028) |
| (4428,-16300) Leon | F582 (301) Rammen | 22 | **693** (F1817-F2510) | im Raum; Klemmlage |
| (6500,-15000) Elza | F364 (83) Hochbiss | 6 | 301 | **Rammen F1209 -> Wurf nach Norden**, 2775 Bilder ausserhalb, 0 Treffer |
| (3350,-15000) Elza | F387 (106) | 27 | **632** (F956-F1588) | im Raum; Klemmlage |

Abnahme-4-Laeufe mit Figur oben: west5_elza (mit = ohne) oben F1838-F2752, erster Treffer nach 379 (CROSS-Biss F2217), 4 Treffer, dann **Wurf aus dem Raum**
(Abschnitt 4); west4_elza oben F668-F1318 (erster nach 402, Rammen F1070, 3 Treffer) und F1658-F1791 (Hochbiss nach 8); west4_leon kommt nicht mehr hinauf (26 Bisse
im Band vor dem Klettern - Folge von Vorgabe B).

Lesart: Vorgabe A im engen Sinn ("oben wird er gerammt/gebissen") ist erfuellt - in jeder Lage, fuer Leon und Elza, mit und ohne Kanister, und auch ohne Kanister
wird gerammt. Die "Klemmlage" (1382,-14013): ein Hochbiss stoesst die Figur von der NW-Ecke der Rampe auf die Klemmlinie des Plattform-Grundrisses (y -1800); die
Querung der Vorgabe A erreicht x 1382 nicht (Bahn-x >= 2550), der Gator schwimmt ~700 Bilder ueber Sued- und Westkanal an die Plattform-Nordkante und beisst von dort
alle ~105 Bilder. "ohne Fortschritt" waehrend dieses Umwegs 454-515 Bilder (B3 gilt laut Messblatt fuer die Wasserlagen; hier nur Ausgabe).

### 3b Vorgabe B - Plattform- und Rampen-Suedkante, Plattform-Westkante

Harness (eigene 54 Wasserlagen 400/1000/1600 vor Rampen-Sued-, Plattform-Sued-, Plattform-West-, Rampen-Nord- und Plattform-Nordkante; `harness/band_*.txt`):
neu ohne/mit Kanister, Elza = Leon: **Haenge-Fenster 0 in allen 54 Lagen** (SUMME je Lauf "hoechstens 0"); laengste Pause ohne Treffer 600 (W-2100/-19000, = Schranke B1b, alter Stand dort 645);
ohne Fortschritt max 399 Bilder = 13,3 s (W-2100/-19000, alter Stand gleich 13,3 s). Alter Stand: **86 Fenster in 11 Lagen** (Rampen-Sued x 4000-6000: 1-3 je Lage, Rampen-Nord W4500/-14100: 1), an der Plattform-Westkante
(W-2100/-2700/-3300, z -16000) je 22-23 Fenster und **0 Treffer in 3600 Bildern** (Pendel an der NW-Ringecke (-4300,-9850)), neu dort 306-390 Bisse.

exe, eigene Rueckkehr-Lagen (Figur steht, 3600 Bilder):

| Lage (Kante), Figur, Stand | Treffer | Haenge-Fenster | ohne Fortschritt max | laengste Pause | Rueckfall-Zeilen |
|---|---|---|---|---|---|
| (-2300,-16000) Plattform-West, Leon ohne, a661eb8aa | **0** | **18** (F864-...) | 234 | 3600 | 0 |
| dto. neu (Cut 0 und Cut 7 gleich) | 24 | 0 | 262 = 8,7 s | 418 | 0 |
| (4073,-19322) Rampen-Sued (Messer-Haengerlage), Elza ohne, a661eb8aa | 22 | **2** (F1207, F1545) | 218 | 285 | 0 |
| dto. neu | 31 | 0 | 26 | 278 | 0 |
| (1000,-21000) Plattform-Sued, Leon ohne, alt / neu | 31 / 31 | 0 / 0 | 36 / 42 | 162 / 247 | 0 |
| (-1000,-21300) Plattform-Sued, Elza mit Kanister, neu | 32 | 0 | 41 | 212 | 0 (Sitz 2940 Bilder, max 0) |
| (6500,-19000) Rampen-Sued Ost, Leon mit Kanister, neu | 30 | 0 | 43 | 262 | 0 (Sitz 2910, max 0) |

Abnahme-4-Laeufe (neu, mit und ohne): Haenge-Fenster 0, ohne Fortschritt max 39 (west4_leon) / 270 (west5_elza) / 289-297 (west4_elza) / 197 (frei4_leon) /
221 (frei4_elza) Bilder, je 0 Zeilen HAENGT / RUECKWAERTS an / TAUCHEN an. exe-Riegel rampe_elza / abn2_wand_elza gruen (haenglog: Fenster 0, abn2_wand_elza ohne
Fortschritt max 56 Bilder, 71 Treffer). Obergrenze B3 (450) in allen Wasser-Szenarien eingehalten.

### 3c Bitgleich ausserhalb der beiden Orte (gegen a661eb8aa)

- Abnahme-4-Laeufe (neu gegen die Abnahme-4-Laeufe der exe d872c8e6, gleiche Eingabe, `vergleich5.py`): in allen 10 Paaren Besuch 1 (Titel), Besuch 2 (erster
  2090/2091-Besuch, Szene) und Besuch 3 (2080/2081) bitgleich. Rueckkehr-Besuch: west4_leon / west4_elza / west5_elza stehen ab F1 im Band (Rueckkehr-Lagen 700 /
  600 / 650 vor der Rampen-Suedkante) - erste Abweichung F10 / F11 / F57, Vorgabe B; frei4_leon bitgleich bis F1491, erste Abweichung F1492 beim Eintritt ins Band vor
  der Plattform-Nordkante (498,-10669); frei4_elza bis F4103, erste Abweichung F4104 beim Eintritt ins Band vor der Rampen-Nordkante (6732,-12972). abn2_wand (eigene
  Laeufe alt/neu): erste Abweichung F210, die Figur ist seit F179 im Band.
- Harness, 13 Wasserlagen mehr als 1800 vor jeder Kante (`harness_vergleich.txt`), Spur je Bild alt gegen neu (Leon ohne / Leon mit / Elza ohne): 9 Lagen
  3600/3600 bitgleich. 4 Lagen 700-1200 vor dem Band (W5500/-11500, W4000/-21000, W-4500/-13500, W-4200/-19500) weichen ab t456-t803 ab: jeweils liegt der Gator an
  der Figur (Kopf 211-2900), und das neue Steuerziel liegt im Band bei der Figur ((5500,-13196), (3519,-20339), ...) - der Koerper-Stoss schiebt die (nur am
  Bildanfang festgehaltene) Figur ins Band, dann greift die Band-Regel. Im exe tritt das nur auf, wenn die Figur dort wirklich steht (3c oben).
- Mit gegen ohne Kanister (neue exe): oben_leon, oben_elza, west5_elza, frei4_leon, frei4_elza bitgleich; west4_elza ab F1982 (mit Kanister rammt er auch eine Figur
  im Wasser: Rammen 2 gegen 1 - die in Abnahme 4 angenommene Abweichung).

## 4. Blockierende Befunde - Mechanik

**M2 Wurf aus dem Raum.** Der Treffer ruft den Niederwurf (`re15_player_knockdown_begin`); dessen Flug laeuft mit `s_kd_speed = 1000` und `-= 15 * Bild`
(game_step_common.c, kommentiert byte-true @0x8003615c / @0x800361c8-e0) - gemessen in ohne_west5_elza: Stoss-Bild F2753 2062, dann 985, 955, 910, 850, 775,
685, 580, 460, 325 (Clip 13), 325, 310, 295, ... (Clip 14) Einheiten je Bild, 12 272 Einheiten bis zum Stillstand (F2752 -> F2800, (2318,-1800,-29914)). Steht die Figur oben (y -1800), bleibt sie waehrend des ganzen Flugs auf y -1800 (kein
Fall auf Wasserhoehe), und die Kollision fuer Band 1 kennt ausserhalb der Rampe keine Aussenwaende: `test_r41_gatorfix begehbar 2090 1` meldet (2318,-29914),
(2318,-27159), (2318,-5449), (2318,-2694) als "begehbar" (Band 0 klemmt (2318,-27159) -> (2318,-26532) und (2318,-5449) -> (2318,-5868)). Der Flug geht in
Querungsrichtung (Nord <-> Sued), aus Lagen nahe der Nord-/Suedkante der Rampe also durch das Becken bis hinter die Wand. Die Deck-Klemme des Koerper-Pushs (NB8,
`re15_gr2_rw_an()`) ist im Spiel seit Vorschlag 4 aus (`re15_gr2_traegt_lenkung()` = nur mit Pruefhaken v4_alt) und wuerde den Niederwurf ohnehin nicht halten. Der
Mechanismus gehoert zur Spieler-Kollision/Niederwurf (wie Offen 1 des Bauers), ausgeloest wird er von den neuen Angriffen oben (CROSS-Biss F2752, F1201; Rammen F1209).

**M1 Absturz abn2_wand.** Kette: neue Band-Bisse (Vorgabe B) -> Biss-Stoss in die Ostnische (F1046) -> Kollisionsloch der Nischen-Rueckwand: auf Band 0 ist
(8500,-16000) -> (9668,-16000) "weggeschoben" (auf die Aussenseite), (9676,-17955) und (12000,-17000) "begehbar"; F1499 -> F1500 versetzt es die stehende Figur
(pad 0000, Clip 200) von (7668,-15802) nach (9668,-16031) -> blinde Eingabe 5600 Bilder ins Leere -> `re15_squareroot0` (Abstand ueberlaeuft bei ~100 000
Einheiten) in `re15_autolook_scan`. Der Absturz selbst ist ein alter Fehler (Wurzel aus einem Ueberlauf), die Lage ausserhalb ein altes Kollisionsloch - erreicht
wird beides erst durch das neue Verhalten in diesem Riegel (alter Stand: nie in der Nische).

**M3 abn2_rampe_west.** Kein Spielfehler im engen Sinn (eine Figur, die an einer Tuer Aktion drueckt, geht hinaus), aber der Riegel ist rot und das Szenario misst
nicht mehr, was es messen soll; Ursache ist derselbe Niederwurf-Flug wie in M2 (5600 Einheiten in 6 Bildern ueber das Wasser bis in die SW-Ecke).

**Warum die Riegel des Bauers M1-M3 nicht sehen:** A1/B1 (Harness) setzen die Figur jedes Bild zurueck - kein Niederwurf-Flug, kein Wurf aus dem Raum; A2 (exe
rampe_oben) zaehlt nur Bilder "oben" und endet ohne Fehler, wenn ein Treffer die Figur aus dem Oben-Grundriss wirft (der Rest nach dem letzten Treffer zaehlt
nicht); abn2_wand / abn2_rampe_west hat er nicht gefahren. Ein Riegel fuer Runde 2 braucht: (1) "Figur nie ausserhalb der SCA-Aussenwaende" (wie `aussen5.py`, oder
die AUSSEN-Zeilen der Engine) in allen Oben- und Kanten-Laeufen, (2) exe-Lagen mit der Figur oben an Nord- und Suedkante der Rampe (z. B. (2318,-16300),
(6500,-15000) per `RE15_PLAYER_POS=x,z,rot,1`), (3) alle 18 exe-Riegel gruen.

## 5. Bild (RE15_FRAMEDUMP-Serien, F-genau, selbst angesehen) - `G2_belege/abn5/`

- `abn5_oben_leon_F90-F1590.jpg` / `abn5_oben_elza_F90-F1590.jpg` (Riegel-Szenario rampe_oben, Spalten ohne | mit Kanister, Cut 2): F90 Figur oben, Gator im
  Suedbecken; F390 / F480 der Koerper quert ueber die Rampe (Rammen F383, CROSS-Biss F483); **F750 und F1020 steht die Figur allein oben, kein Gator im Bild** (das
  Fenster F483-F1339, er holt den Kanister); F1350 liegt die Figur (Rammen F1339 / F1337); F1440 CROSS-Biss, in der Spalte "mit" der gelbe Kanister am Kiefer; F1590
  Hochbiss, Kopf ueber dem Deck neben der Figur. Leon und Elza bildgleich im Ablauf.
- `abn5_west5_elza_wurf_aus_dem_raum_F2745-F2830.jpg` (Abnahme-4-Lauf west5_elza ohne Kanister; mit Kanister bitgleich): F2745 / F2750 Elza oben an der SW-Ecke der
  Rampe, der Gator-Kopf neben ihr; F2755 der Stoss; **F2760-F2770 Elza fliegt in Niederwurf-Haltung waagerecht in Rampenhoehe ueber das Wasser zur Wand**; ab F2780
  ist sie verschwunden (ausserhalb der Suedwand), F2830 nur noch der Gator.
- `abn5_westkante_cut7_alt_neu_F300-F3450.jpg` (Rueckkehr (-2300,-16000), Leon ohne Kanister, Cut 7; a661eb8aa | neu): alt steht Leon von F750 bis F3450 allein
  neben der Maschine, kein Gator in der Naehe (0 Treffer, 18 Haenge-Fenster); neu liegt der Gator ab F300 an Leon, Leon wird wiederholt niedergeworfen (F1200, F3000)
  und am Boden gebissen (F3450).
- `abn5_abn2wand_nische_F1380-F1520.jpg` (Absturzlauf abn2_wand ohne Kanister, Cut 2): F1380-F1490 Leon klein rechts in der Ostnische, der Gator davor; ab F1500 ist
  Leon verschwunden (hinter die Nischen-Rueckwand versetzt).
- Die eigenen Oben-Lagen (Spawn) behalten Cut 0 (Nordwand mit Leiter und Tor), die Figur oben ist dort nicht im Bild - fuer sie gelten nur die Logs (Lage je Bild,
  AUSSEN-Zeilen der Engine selbst in gator_boss.log).

## 6. Die offenen Punkte des Bauers - Einstufung (eigene Messung)

1. Spieler-Wegstoss in den Pier: gleiche Familie wie M1/M2 (Niederwurf-Flug + Kollisionsloecher). **Blockierend in der Form M2** (Wurf aus dem Raum), der Pier allein
   nicht gemessen.
2. exe-Szenario wand_elza degeneriert: bestaetigt (abn2_wand_elza gruen, aber Szenario haengt in der Taschen-Ecke); dazu degenerieren jetzt auch abn2_wand (M1) und
   abn2_rampe_west (M3) - **die blinden Wand-Touren tragen mit Vorgabe B nicht mehr.**
3. Kanister-Zielmodus waehrend der Belagerung: bestaetigt (856 / 854 Bilder ab dem ersten Oben-Bild, Bild F750/F1020). RE2 belegt (`lbu v0,550(s0)` / `sltiu
   v0,v0,0x2` / `ori v0,v0,0x4` / `sb v0,448(s0)` @0x80100a34-5c nach `jal 0x80077360` @0x80100a24). Nicht blockierend, **Nutzerfrage F1**. Dass `obenlog` erst ab dem
   Anhaengen zaehlt, ist eine Aenderung gegenueber Messblatt A2 ("ab dem ersten Oben-Bild") - offen dokumentiert, aber der Riegel sieht dieses Fenster nicht.
4. Takt am Rand der Schranke 600: bestaetigt (Harness 537 / 530); in eigenen exe-Lagen darueber (632-849, Klemmlage) - **Nutzerfrage F1**.
5. Kopf vor der Kamera: F1 "noe" - kein Riegel; nicht blockierend.
6. Fuzz-Ausnahme "Kopf < 2400": 2400 ist der vorhandene LUNGE-Ausloeser (`gb_maul_dist(e, pl) < 2400`, enemy_ai_boss_gator.c:2224/2295); nicht blockierend.
7. PSX-Bau nicht gemessen: bestaetigt (nicht gemessen).
8. Querbezug re2gator (EXIT_AT 1700, Patch 9): gruen in meiner Gruppe (340,12 s; kanister 240 angehaengte Bilder, Abweichung 0; belagerung GIER-WACHE bis F581,
   300 Bildpaare). Ob das Fenster 1700 bleibt, entscheiden die Spur-Eigner (alligator2 / gatorszene); nicht blockierend.
9. Runde 2: steht aus (dieses Urteil).

## 7. Konstanten im Diff e92356e8e..6b9f7dac4 (enemy_ai_boss_gator.c, re15_boss_gator.h, test_r41_gatorfix.c, test_r41_gatorfix.cmake, test_r41_re2gator.cmake)

| Zahl | Stelle | Beleg | Bewertung |
|---|---|---|---|
| `GB_OBEN_OHNE 300` | enemy_ai_boss_gator.c (Kopf) | PORT-WAHL mit Nutzer-Wortlaut im Kommentar; = NB5-Pause `cross_cd = (pl->y < -900) ? (re15_gr2_rw_an() ? GB_KREUZ_PAUSE : 300) : 90` (Z. 2797; `re15_gr2_rw_an()` im Spiel 0, `re15_gr2_traegt_lenkung()` = `s_v4_alt && ...`) | belegt |
| `y < -900` / `y >= -900` (gb_pl_oben, gb_pl_im_block) | neu | = `player_on_platform` (Z. 1898-1912, `(pl->y < -900)`) | belegt |
| 2600 (gb_wand_dazwischen, "naeher als der Maulpunkt") | neu | Maulpunkt derselben Funktion (`kx = e->x + fc*2600`) | belegt |
| gb_quer_trifft: Start-x [GB_RAMP_X0+GB_RING_M .. GB_RAMP_X1-700], Lande-x 3350, cz +-GB_BAHN_M, Laenge 64er-Schritte < 30000, Dauer len/32 in [90..240], Fenster-Marge 300 | neu | Zahl fuer Zahl gleich gb_cross_begin (Z. ~1180-1220); Bahn-Ende im Fenster GB_RAMP_X0+700 = GBP_CROSS (b) (Z. 2782) | belegt (Nachbildung; die Klammer `t_aus <= t_ein -> +1` fehlt, ohne Wirkung) |
| Rammen-Schaden 50 | Kommentar GBP_CROSS | `DAT_8006f418[5]` @0x8006f422 = 0x32 in info/Re1.5/PSX.EXE (Tafel {10,20,1000,1000,1000,50,100,200,300,1000}, Leser @0x80012e54) - selbst ausgelesen | belegt |
| `oben_ohne < 30000` | neu | Saettigung des int16-Zaehlers, kein Verhalten | ohne Belang |
| Phasenliste gb_phase_kampf | Patch 9 | Messung exe-Riegel re2gator belagerung (Dossier) | belegt |
| `NB3_PAUSE_MAX 600` | test | Messblatt A1: 300 + 240 + 45 = 585, aufgerundet; PORT-WAHL "darf halt oben nicht sicher sein" | belegt |
| `NB3_OHNE_MAX 450` | test | Messblatt B3 (3 Haenge-Fenster, PORT-WAHL nach "Er darf nicht zu lange haengen") | belegt im Messblatt; **formal: der Code-Kommentar nennt weder "PORT-WAHL" noch den Nutzer-Wortlaut** (Auftrag verlangt beides im Code-Kommentar) |
| Fuzz "kopf < 2400" | test | vorhandener LUNGE-Ausloeser (Z. 2224/2295) | belegt |
| gang-Lagen (4944,0,-5868), (6734,0,-26532) | test | Lagen des Riegels re2 (`k_re2_lagen`: NO-Ecke, SO-Ecke) | belegt; der Kommentar in `tests/unit/probes/r41_gatorfix.cmake` ("Nordbecken 2500 vor der Kante, Suedbecken 2500 vor der Plattform") passt nicht zu den Lagen |
| re2gator `RE15_EXIT_AT` 1500 -> 1700 | test | gemessene Verschiebung F1227 -> F1447 (+220), Schranke >= 200 angehaengte Bilder bleibt | belegt |
| obenlog "ab KANISTER angehaengt" | test | RE2 Zielmodus @0x80100a34-5c (selbst in re2_em23.dis nachgelesen) | belegt, aber Riegel-Aenderung gegen Messblatt A2 (Abschnitt 6 Punkt 3) |

## 8. Fragen an den Nutzer (fuer die Release Notes "Zu entscheiden", Sie-Form)

- F1: "Wenn der Kanister nach der Szene noch im Wasser liegt, holt der Alligator ihn zuerst (wie in RE2). In dieser Zeit greift er Sie oben auf der Rampe nicht
  an - im Messlauf bis zu 28 Sekunden lang, nach einem ersten Rammstoss. Ebenso braucht er bis zu 28 Sekunden, wenn ein Treffer Sie in die Ecke zwischen Rampe und
  Plattform stoesst, weil er dann um die Plattform herumschwimmen muss. Gilt das noch als 'oben nicht sicher' (Takt frei), oder soll er Sie in diesen Faellen
  schneller wieder angreifen?"

## 9. Tests (Summenzeilen woertlich)

- `ctest --test-dir re15_port/build -R "^unit_r41_gatorfix_" -j 2 --timeout 900`: **`100% tests passed, 0 tests failed out of 26`**, `Total Test time (real) =  52.87 sec`.
- `test_r41_gatorfix.exe nb3` direkt: `nb3: 0 Fehler`, exit 0 (SUMME/ok-Zeilen = `nb3/bau/riegel_nb3.txt`).
- exe-Riegel, Gruppe (`-R "^integration_r41_gatorfix_|^integration_r41_re2gator" -j 2 --timeout 1800 -V`): **`89% tests passed, 2 tests failed out of 18`**,
  `Total Test time (real) = 3970.21 sec`; `The following tests FAILED: 1186 - integration_r41_gatorfix_abn2_rampe_west (Failed)`, `1189 -
  integration_r41_gatorfix_abn2_wand (Failed)`.
- exe-Riegel, die zwei roten einzeln: **`0% tests passed, 2 tests failed out of 2`**, `Total Test time (real) = 914.30 sec`.
- Harness-Messwerkzeuge (eigene Lagen, `harness5.sh`): nb3_oben / nb3_haengt je exit 0 (`harness/*.txt`).

## 10. Nicht geprueft

PSX-Bau; Raumwechsel mitten im Kampf; Hoeren; ein laufender (nicht stehender) Spieler oben; Elza in den Kanten-Lagen der Plattform-Westkante (Leon = Elza im
Harness); die Oben-Lagen mit Kanister im Maul in eigenem Aufbau (der liegende Kanister wurde im Spawn-Aufbau nie aufgenommen; "mit Kanister oben" ist nur durch das
Riegel-Szenario rampe_oben und west4_elza / west5_elza belegt).
