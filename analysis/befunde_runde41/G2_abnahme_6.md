# Runde 41 - Spur G2 "gator_re2" - UNABHAENGIGE ABNAHME 6 (2026-11-07): G2-RE2 NB3 Runde 2 (Raumhalt, Anlauf ohne Ruhezone; Vorgabe A "oben nicht sicher", Vorgabe B "kein Haenger")

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Kopf 9b83eadfe (NB3 Runde 2 auf 10603425b = Abnahme 5 NICHT BESTANDEN): 5a94392bf Bau (Raumhalt
`re15_gator_boss_raum_halt`, Anlauf der Zaehler-Querung ohne Ruhezone), aa41ec95e Riegel raum(_elza) / aussenlog / oben_kante(_elza) / abn2_rampe_west neu,
5a4ee385f aussenlog ohne Kletterbilder / obenlog kampf, 9b83eadfe Dossier + Belege. NB3 Runde 1: a7415b225..6b9f7dac4, Messblatt e92356e8e.
Pruefer: Abnahme-Agent, glaubt dem Bau nichts, misst selbst. Kein Spielcode geaendert.
Zaehlung: Der Auftrag nennt `G2_abnahme_5.md` / `G2_belege/abn5/`. Diese Namen gehoeren der Abnahme 5 (NB3 Runde 1, Commit 10603425b, NICHT BESTANDEN) und
bleiben unveraendert; diese Abnahme der NB3 Runde 2 ist die sechste auf r41/gator_re2 und heisst darum `G2_abnahme_6.md`, Belege `G2_belege/abn6/`.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn6`
(je Lauf `runs/<lauf>/` mit state.log, climb.log, gator_boss.log, gator_trag.log, debug.log, env.txt, pid.txt, exe_md5.txt, fd_NNNNNN.ppm; Harness `harness/`).
Belege: `G2_belege/abn6/` (`messung_abnahme6.txt` = alle Auswertungen, `harness/`, `ctest_*.txt`, `build_check.txt`, `sca_2090.txt`, Bildleisten, `werkzeug/`).

Spezifikation: Nutzerentscheid 2026-11-06 ~21:15 (woertlich): F1 "Zu F1, noe, wozu wenn es gut funktioniert?" (keine Kameragrenze); F2 "Verstehe ich
nicht..." (Querungsbeginn bleibt); F3 / Vorgabe A "Er soll nur den Spieler oben auf der Rampe auch rammen koennen, das der Spieler oben nicht ausweichen kann.
Alternativ kann er auch hoch beissen oder sowas, aber er darf halt oben nicht sicher sein." (Takt frei: "Das kann er gerne machen."); F4 / Vorgabe B "Wenn er
haengt muessen wir nachbessern denke ich. Er darf nicht zu lange haengen. Wenn es kurz ist, kann man mit dem tauchen abhilfe schaffen, aber das hilft natuerlich
nicht bei der Platform." Zahlen: Messblatt NB3 Abschnitt 4 (A1/A2 erster Treffer und Pause oben <= 600 Bilder; B1-B3 Haenge-Fenster 0, ohne Fortschritt <= 450
Bilder = 15 s, PORT-WAHL). Dazu die Befunde der Abnahme 5: M1 Absturz abn2_wand, M2 Wurf aus dem Raum, M3 abn2_rampe_west rot, und ihr Riegel-Wunsch (1) Figur nie
ausserhalb in allen Oben-/Kanten-Laeufen, (2) exe-Lagen oben an Nord- und Suedkante, (3) alle exe-Riegel gruen. Ausserhalb der beiden Orte (oben auf der Rampe,
Wasser naeher als 1800 an einer Blockkante, Messblatt B4) gilt der gelobte Stand a661eb8aa (exe d872c8e6) bitgleich.

## 0. Urteil

**BESTANDEN.** Keine blockierenden Befunde (kein Absturz, kein Haenger ueber der Obergrenze, kein Rueckschritt gegen a661eb8aa ausserhalb der beiden Orte,
Konstanten belegt); drei Nutzerfragen (F1 aus Abnahme 5 weiter offen, F2, F3) und ein formaler Hinweis. Kurz:
- **Vorgabe A (oben nicht sicher) - erfuellt, Leon und Elza, mit und ohne Kanister.** In jeder gemessenen Oben-Lage - zehn eigene (alle Blickrichtungen, zwei
  davon gehend), die drei Wurf-Lagen der Abnahme 5, das Riegel-Szenario rampe_oben, die Westkante abn2_rampe_west, west4_elza / west5_elza - wird die Figur
  getroffen: gerammt, hochgebissen und von der Querung gebissen, jeder Treffer 50 Schaden, in den eigenen Lagen 18-25 Treffer in den ersten 120 s nach
  Kampfbeginn; der erste Treffer spaetestens 402 Bilder nach Kampfbeginn bzw. nach dem Hinaufsteigen, keine Pause ueber 447 Bilder (Harness raum mit freier
  Figur: 492; Schranke des Messblatts 600). Einzige Ausnahme ist das
  Kanister-Holen zu Kampfbeginn (851-856 Bilder ohne Treffer, unveraendert seit Runde 1, RE2-Zielmodus) - nicht blockierend, Nutzerfrage F1 (seit Abnahme 5 offen).
- **M1/M2 der Abnahme 5 - behoben.** Kein Absturz mehr (abn2_wand exit 0 bis F8400), kein Bild mit der Figur ausserhalb von Deck oder Raum in 46 eigenen Laeufen und
  den Riegel-Laeufen; der Raumhalt greift nur in einer Trefferreaktion (eigene Laeufe 134 Zeilen, alle im Niederwurf; Riegel-Laeufe 164 Niederwurf + 2
  Treffer-Clip 11), nie beim Gehen, Klettern, Absteigen. M3: Riegel gruen; die Ursache (lange RE1.5-
  Niederwuerfe nach den Bissen im Band) bleibt im Spiel - Nutzerfrage F2.
- **Vorgabe B (kein Haenger an Plattform-/Rampen-Suedkante) - erfuellt, Leon und Elza, mit und ohne Kanister.** Haenge-Fenster 0, laengste Zeit ohne Fortschritt
  326 Bilder (Obergrenze 450 aus dem Messblatt), kein Rueckfall (0 Rueckwaerts/Tauchen); an denselben Lagen im gelobten Stand 13-23 Bisse und bis 1123 Bilder ohne
  Biss, jetzt 20-32 Bisse.
- **Kein Rueckschritt gegen a661eb8aa ausserhalb der beiden Orte:** die Abnahme-4-Laeufe sind bis zum Eintritt der Figur ins Band bitgleich; gegen Runde 1 aendert
  Runde 2 nur die Bilder ab dem ersten Raumhalt-Eingriff.
- **Konstanten belegt** (selbst nachgelesen); ein formaler Hinweis (`GB_PR` zitiert die gleichwertige Sonden-Last @0x80036210 statt der Klemmen-Last @0x80031d6c).
- Riegel: unit 28/28 gruen; exe 20/20 gruen (`100% tests passed, 0 tests failed out of 20`).

| Punkt | Soll | Ist (eigene Messung) | Bewertung |
|---|---|---|---|
| Vorgabe A: oben nicht sicher (Leon, Elza, mit/ohne Kanister, stehend/gehend) | oben gerammt / hochgebissen, mit Schaden; Messblatt A1/A2: erster Treffer und Pause <= 600 | zehn eigene Oben-Lagen (alle Blickrichtungen, zwei gehend): 19-28 Treffer in 140 s, erster nach Kampfbeginn 83-302 Bilder, laengste Pause 200-447; rampe_oben 20 Treffer (16 in den ersten 120 s), abn2_rampe_west 37; jeder Treffer 50 Schaden; Harness raum (frei, 2400/3600): 0 von 16 Lagen verlassen, erster <= t450, Pause <= 492 | erfuellt |
| Vorgabe A: Pause beim Kanister-Holen | (Messblatt A2: keine Pause > 600 ab dem ersten Oben-Bild) | 851-856 Bilder = 28 s zu Kampfbeginn, solange der Gator den Kanister aus dem Becken holt (RE2-Zielmodus); unveraendert seit Runde 1 | Rest, nicht blockierend - Nutzerfrage F1 (seit Abnahme 5 offen) |
| M2 Wurf aus dem Raum | Figur nie ausserhalb Deck/Raum | 46 eigene Laeufe + 2 ctest-Laeufe: 0 Bilder; west5_elza F2753 RAUMHALT statt Flug durch die Suedwand | behoben |
| M1 Absturz abn2_wand | kein Absturz | eigener Lauf mit/ohne Kanister exit 0 bis F8400 (Runde 1: exit 139 in F7374); exe-Riegel gruen | behoben |
| M3 abn2_rampe_west | Riegel gruen, misst "Leon oben an der Rampe West" | gruen (616,99 s), oben 7897 Bilder, 37 Treffer; die alte Eingabe endet weiter an der SW-Tuer (bitgleich Runde 1) | erfuellt; Ursache im Spiel = Nutzerfrage F2 |
| Vorgabe B: kein Haenger Plattform-/Rampen-Suedkante (Leon, Elza, mit/ohne) | Haenge-Fenster 0, ohne Fortschritt <= 450 (15 s), kein Rueckfall | 12 eigene Kanten-Laeufe + 10 Abnahme-4-Laeufe + abn2_wand: Fenster 0, ohne Fortschritt max 326 (RE2-Dash), 0 Zeilen Rueckwaerts/Tauchen; Plattform-Sued 20-32 Bisse (gelobter Stand an denselben Lagen 13-23, bis 1123 Bilder ohne Biss) | erfuellt |
| Bitgleich ausserhalb der beiden Orte (a661eb8aa) | ohne Kanister bitgleich ausser oben / im Band | 10 Abnahme-4-Laeufe: Besuche 1-3 bitgleich, Rueckkehr bis zum Eintritt ins Band (frei4_leon F1492, frei4_elza F4104; west* starten im Band); gegen Runde 1 bitgleich bis zum ersten RAUMHALT-Bild | erfuellt |
| Raumhalt nur in den Loechern | kein Eingriff beim Gehen, Klettern, Absteigen | eigene Laeufe 134 RAUMHALT-Zeilen, alle im Niederwurf (Clip 13); Riegel-Laeufe 166, davon 2 im Treffer-Clip 11; 0 in 2618 Gehen-Bildern oben; Klettern/Abstieg unberuehrt | erfuellt |
| Kanister-Sitz / Rueckfall | Sitz 0, Tauchen nur Rueckfall | "Abweichung max 0" in allen Laeufen mit Kanister (2520-7830 Bilder); 0 Rueckwaerts/Tauchen/HAENGT in 46 Laeufen | erfuellt |
| Riegel unit_r41_gatorfix_* | gruen | `100% tests passed, 0 tests failed out of 28` | gruen |
| exe-Riegel integration_r41_gatorfix_* / re2gator (20) | gruen | `100% tests passed, 0 tests failed out of 20`, `Total Test time (real) = 4384.99 sec` | gruen |
| Konstanten | @0x..., Messung oder PORT-WAHL + Wortlaut | 450, 0x12, SCA-Grenzen selbst nachgeprueft; Raumhalt PORT-WAHL mit Wortlaut; NB3_OHNE_MAX jetzt mit PORT-WAHL | belegt; formaler Hinweis: `GB_PR` zitiert die Sonden-Last @0x80036210, die Klemme laedt denselben Wert @0x80031d6c |
| Bild an der Deckkante | - | Raumhalt oben: Clip 13/14 im Stand, Figur taumelt, liegt nicht | kosmetisch, Nutzerfrage F3 |

## 1. Bau, Messweg

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build` (13:17): `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)` (`build_check.txt`).
  exe `re15_port/build/platform/pc/re15_pc.exe` 2026-11-07 11:47:22, md5 5910fc246fc82eaaac9f64795492585a (= Bauer-exe); juengste Engine-Quelle
  enemy_ai_boss_gator.c 11:47:18 (committet in 5a94392bf 11:54), keine Aenderung an engine/ platform/ include/ nach 5a94392bf; test_r41_gatorfix.exe 12:17:26 nach
  test_r41_gatorfix.c 12:17:07. Arbeitsbaum sauber, Kopf 9b83eadfe.
- Messkopien (eigene, nur eigene PIDs, kein taskkill): `re15_pc_g2abn6.exe` = Kopie der Bau-exe (5910fc24); `re15_pc_g2abn6_alt.exe` = Kopie von
  `g2nb3/exe_alt/re15_pc_alt_a661eb8aa.exe` (d872c8e6, gelobter Stand). Runde-1-Vergleiche gegen die Laeufe der Abnahme 5 (`g2nb3/abn5r2/runs/`, exe 029efac6,
  gleiche Eingaben); Vergleichslaeufe des gelobten Stands fuer die Abnahme-4-Eingaben: `g2abn4/runs/` (Abnahme 4, exe d872c8e6, Umgebung wortgleich), dazu
  eigene alt-Laeufe an den Kanten.
- Laeufe: `werkzeug/lauf6.sh` / `batch6.sh` / `alle6.sh` (Listen `welle1..4.txt`), Umgebung wortgleich zu den Abnahmen 4/5 bzw. zum cmake der exe-Riegel. Kein
  gdigrab; Bilder nur als RE15_FRAMEDUMP-Serie (F = Raum-Bildzahl wie state.log). 46 eigene exe-Laeufe (42 mit der Bau-exe 5910fc24, 4 mit der exe des
  gelobten Stands d872c8e6), alle exit 0 bis RE15_EXIT_AT, 2-5 gleichzeitig neben dem ctest-Lauf und fremden Agenten; dazu die beiden ctest-Laeufe
  abn2_rampe_west (Arbeitsordner des Riegels) mit den eigenen Auswertern.
- Lauf-Arten: (a) Szenario des exe-Riegels rampe_oben (Leon/Elza, mit/ohne Kanister) und zehn EIGENE Oben-Lagen (keine Lage der exe-Riegel oder der Abnahme 5;
  (2300,-14950) liegt nahe der Harness-Lage (2318,-15000) des Bauers):
  Figur per `RE15_PLAYER_POS=x,z,rot,1` auf dem Deck (Band 1, y -1800), Szene gesehen + Tor offen (`RE15_SET_FLAG=9:110,9:113`), Kampf sofort (`RE15_GB_TEST=1`,
  RE2-Intro bis F280), 4200 Bilder, Blick 0/1024/2048/3072, acht davon stehend, zwei mit einer Gehen-Eingabe ueber das ganze Deck (x 2302..6749, z -17650..-14951);
  (b) eigene Rueckkehr-Lagen im Wasser vor der Plattform-Suedkante (6), Rampen-Suedkante (3) und Plattform-Westkante (Elza), Figur steht, 3600 Bilder, mit und ohne
  Kanister, an drei Lagen dazu der gelobte Stand; (c) die Abnahme-4-Laeufe west4_leon / west4_elza / west5_elza / frei4_leon / frei4_elza (Eingaben md5 96797cb6 /
  14cd3db6 / 40e23a92) mit und ohne Kanister; (d) abn2_wand mit/ohne bis F8400 (M1), abn2_wand fein (Framedump), die ALTE Eingabe von abn2_rampe_west (M3).
- Auswerter (eigene; Grundlage die Werkzeuge der Abnahme 5, siehe unten): `aussen6.py` (jedes Bild in 2090/2091: oben ausserhalb des Rampen-Grundrisses SCA [5] =
  "Deck verlassen", unten ausserhalb der Aussenwaende / Ostnische / Mittelpunkt in [4]/[5]; Kletterbilder climb.log sub != 0 getrennt; RAUMHALT-Zeilen; Ausnahme-
  Zeilen), `mass6.py` (Treffer = hp-Flanke mit Schaden, Art, Oben-Abschnitte ab erstem Oben-Bild / ab Kampfbeginn = Intro-Ende / ab Anhaengen, Haenge-Fenster nach
  gator_rw_r41.c (b), ohne Fortschritt nach mass_v4.py, Rueckfall-Zeilen, Kanister-Sitz), `vergleich6.py` (Bild fuer Bild je Besuch: Figur + Gator-Block + TRAG,
  erste Abweichung mit Ort, erste RAUMHALT-Zeile), `flug6.py` (Weg nach Treffern), `sca6.py` (SCA selbst geparst). `auswerten6.sh` schreibt alles nach
  `messung_abnahme6.txt`.
- Werkzeugfehler gefunden und behoben: die Auswerter der Abnahme 5 (mass5/aussen5/vergleich5, Muster `rot=(\d+)`) ueberspringen Bilder mit negativem Blick
  (`rot=-96` u. a.; in den Abnahme-5-Laeufen abn2_wand 2341-4109 Zeilen, frei4 je 37 Zeilen). `*6.py` lesen `rot=(-?\d+)`; alle Zahlen hier mit dem korrigierten
  Muster. Die Urteile der Abnahme 5 aendert das nicht (ihre Befunde lagen in gelesenen Bildern).
- Determinismus: west5_elza ohne zweimal (mit verschiedener Framedump-Serie) und die Oben-Lage (4000,-16000) zweimal - jeweils alle Bilder bitgleich (3300/3300,
  4200/4200, RAUMHALT-Zeilen gleich). Framedump aendert das Spiel nicht.
- Harness: `test_r41_gatorfix.exe raum` / `raum_elza` direkt (2400 Bilder wie der Riegel und 3600 = 120 s mit `G2_RAUM_BILDER=3600`), `nb3` / `nb3_elza`, `gang` /
  `gang_elza` direkt, `nb3_oben` mit den eigenen Oben-Lagen.
- Riegel-Laeufe nachgerechnet: die Arbeitsordner meiner ctest-Laeufe rampe_oben(_elza), rampe(_elza), oben_kante(_elza), abn2_rampe_west mit `aussen6.py` /
  `mass6.py` ausgewertet - gleiche Zahlen wie die Statuszeilen der Riegel und wie meine eigenen Laeufe gleicher Eingabe (rampe_oben: 20 Treffer, 300 / 856,
  ab Anhaengen 326 / 343). `riegel6.py` zieht die Kernzahlen aus `ctest_exe.txt`.

## 2. Riegel (Summenzeilen woertlich)

- Harness: `ctest --test-dir "<Baum>/re15_port/build" -R "^unit_r41_gatorfix_" -j 2 --timeout 900` (13:16:47-13:18:17, `ctest_unit.txt`):
  **`100% tests passed, 0 tests failed out of 28`**, `Total Test time (real) =  89.29 sec` (neu gegenueber Abnahme 5: raum 12,75 s, raum_elza 12,70 s).
- exe-Riegel: `ctest --test-dir "<Baum>/re15_port/build" -R "^integration_r41_gatorfix_|^integration_r41_re2gator" -j 2 --timeout 1800 -V` (13:18:30-14:31:36, die
  ersten ~50 min parallel zu meinen eigenen Laeufen und zu fremden Agenten; `ctest_exe.txt`): **`100% tests passed, 0 tests failed out of 20`**, `Total Test time (real) = 4384.99 sec`.
  Einzelzeiten: abn2_wand_elza 666,74 s, frei_elza 686,18, frei 644,52, abn2_wand 643,21, abn2_rampe_west 616,99, abn2_leiterecke 627,86, abn2_ecken 586,30,
  abn2_nordbecken 565,91, oben_kante 488,19, oben_kante_elza 526,95, rampe_oben 369,96, rampe_oben_elza 403,77, rampe 306,83, rampe_elza 339,12, re2gator 275,99,
  gang_montage_elza 43,46, gang_szene_elza 42,90, gang_montage 47,31, gang_szene 31,22, abn2_rampe_west_elza 662,97 s (Passed).
- Statuszeilen (`werkzeug/riegel6.py` auf `ctest_exe.txt`): aussenlog in allen Laeufen frei / rampe / abn2_* / rampe_oben / oben_kante, mit und ohne Kanister:
  `aussen 0, Deck verlassen 0, im Block 0`; oben_kante(_elza) erster Treffer oben nach 363 / 84 / 142 Bildern, laengste Pause 375 / 345 / 380 (Leon = Elza);
  rampe_oben(_elza) ab dem Anhaengen erster nach 326 / 324, Pause 343 (`-> ok (Schranke 600)`); rampe / rampe_elza / abn2_wand_elza haenglog: Haenge-Fenster 0,
  ohne Fortschritt max 16 / 16 / 56 Bilder, `Zeilen HAENGT 0, RUECKWAERTS an 0, TAUCHEN an 0` (`-> ok (Fenster 0, ohne Fortschritt <= 450)`); tragvergleich
  abweichend 0 und Kanister-Sitz Abweichung max 0 in allen Laeufen mit Kanister; re2gator `r41_re2gator: ok` (kanister 240 angehaengte Bilder, Abweichung max 0).
- Kein roter Riegel - nichts einzeln nachzufahren. abn2_rampe_west_elza (in der Gruppe des Bauers rot durch den Auswerter, Kletterbilder als "Deck verlassen") ist hier gruen; mein Auswerter auf dem damals roten Lauf `lauf_5e76769d` (mit/ohne): nie ausserhalb, 91 Kletterbilder, 98 Bilder oben.

## 3. Eigene Messung

### 3a Vorgabe A - Figur oben auf der Rampe

Riegel-Szenario rampe_oben (Klettern F48-F82, oben ab F83, Kanister liegt nach der Szene im Becken; Cut 2), Figur steht 144 s oben:

| Lauf | Treffer oben (je 50 Schaden) | in den ersten 120 s | erster ab erstem Oben-Bild | laengste Pause ab erstem Oben-Bild | ab Anhaengen (F1013): erster / Pause | ausserhalb |
|---|---|---|---|---|---|---|
| Leon mit = ohne (bitgleich) | 20: Rammen 6, CROSS-Biss 4, Hochbiss 9, 1 Querungsbiss | 16 | 300 (Rammen F383) | **856 = 28,5 s** (F483-F1339, Gator holt den Kanister) | 326 / 343 | nie |
| Elza mit = ohne (bitgleich) | 20 (dto.) | 16 | 300 (Rammen F383) | **854** (F483-F1337) | 324 / 343 | nie |

Runde 2 = Runde 1 Bild fuer Bild (alle vier Laeufe, state 4400/4400 + TRAG 3362/3362): der Raumhalt greift hier nie, der Anlauf-Fix aendert nichts.

Eigene Oben-Lagen (ohne Kanister im Maul; der Kampf beginnt mit dem Intro-Ende F280; "ab F1" schliesst die 280 Intro-Bilder ein):

| Lage (Blick), Figur | Treffer (je 50 Schaden) | erster nach Kampfbeginn (ab F1) | laengste Pause nach Kampfbeginn | ausserhalb | RAUMHALT |
|---|---|---|---|---|---|
| (4000,-16000) r1024 Leon | 22: Hochbiss 11, Rammen 5, sonst 6 | 302 (581) | 380 | nie | 23 |
| (5800,-17650) r1024 Elza (Suedkante) | 24: Hochbiss 11, Rammen 7, sonst 6 | 88 (367) | 353 | nie | 5 |
| (3000,-14950) r1024 Leon (Nordkante) | 19: Hochbiss 10, Rammen 4, sonst 5 | 119 (398) | 369 | nie | 18 |
| (6750,-16300) r1024 Elza (Ostende) | 23: Hochbiss 10, Rammen 7, sonst 6 | 148 (427) | 305 | nie | 3 |
| (2300,-14950) r3072 Elza (NW-Ecke) | 21: Hochbiss 9, Rammen 6, sonst 6 | 227 (506) | 380 | nie | 26 |
| (5000,-15200) r3072 Leon | 22: Hochbiss 9, Rammen 7, sonst 6 | 83 (362) | 305 | nie | 8 |
| (6000,-17400) r2048 Leon | 19: Hochbiss 12, Rammen 6, sonst 1 | 84 (363) | 447 | nie | 0 |
| (2600,-16800) r0 Elza | 28: Hochbiss 26, Rammen 1, sonst 1 | 136 (415) | 200 | nie | 0 |
| (4400,-16200) r0 Leon **gehend** (1166 Bilder Eingabe) | 22: Hochbiss 16, Rammen 4, sonst 2 | 156 (435) | 340 | nie | 0 |
| (3600,-15800) r2048 Elza **gehend** (1452 Bilder Eingabe) | 22: Hochbiss 12, Rammen 6, sonst 4 | 103 (382) | 317 | nie | 0 |

("sonst" = Treffer ohne TRAG-Zeile mit Gator-Clip 0, die Querung ohne Kanister.) In den ersten 120 s nach Kampfbeginn (F280-F3880) je Lage 18-25 Treffer.
Weitere Laeufe mit Figur oben:
- exe-Riegel abn2_rampe_west (Leon an der Westkante x 2318, eigene ctest-Laeufe `lauf_aee28c9a` / `_ohne`, mit = ohne): oben 7918 Bilder (264 s), 37 Treffer
  (CROSS-Biss 8, Hochbiss 16, Rammen 12, 1 Querungsbiss), erster 375 nach dem ersten Oben-Bild, laengste Pause **851** (F526-F1377, Kanister holen), ab dem
  Anhaengen F1002 hoechstens 375; nie ausserhalb.
- Abnahme-4-Laeufe: west5_elza (mit = ohne) oben F1838-F3300: 7 Treffer, erster nach 379, Pause hoechstens 379; F2752 CROSS-Biss an der SW-Ecke des Decks ->
  F2753 RAUMHALT (2315,-19568) -> (2315,-17632), sie bleibt oben und wird F2817 (Rammen), F2924, F3029 (Hochbiss) wieder getroffen. west4_elza oben F668-F1641:
  5 Treffer, erster nach 402, Pause 402; F1319 RAUMHALT (7668,-18587) -> (6732,-17632); F1627 steigt sie per Aktionstaste an der Nordkante ab (sub 9/10, F1642-F1666):
  der Abstieg geht mit dem Raumhalt. west4_leon kommt (wie in Abnahme 5) nicht hinauf.
- Harness raum (freie Figur): 2400 Bilder (Riegel) und 3600 Bilder (120 s), Leon = Elza, mit = ohne: 0 von 16 Lagen verlassen Deck/Raum, oben erster Treffer
  spaetestens t450 (R2318/-16300), laengste Pause oben 492 (R2800/-16300), wenigste Treffer je Oben-Lage 9 (2400) bzw. 13 (3600); Raumhalt 385 bzw. 624 Eingriffe.
- Harness nb3_oben (Messwerkzeug des Messers, Figur je Bild gesetzt) an meinen zehn Oben-Lagen, Leon ohne und Elza mit Kanister, 3600 Bilder: `SUMME ...: 0 von
  10 Lagen ohne Treffer; spaetester erster Treffer t300 (R6750/-16300); laengste Pause ohne Treffer 362 B. = 12.1 s (R2600/-16800)` (beide gleich;
  `harness/nb3_oben_eigen_*.txt`).

Lesart Vorgabe A: in jeder Lage, fuer Leon und Elza, stehend und gehend, mit und ohne Kanister wird die Figur oben getroffen - gerammt, hochgebissen und von der
Querung gebissen, jeder Treffer mit Schaden (hp 100 -> 50) - und sie bleibt dabei im Raum. In den eigenen Lagen nach Kampfbeginn hoechstens 302 Bilder bis zum
ersten Treffer (west4_elza / west5_elza nach dem Hinaufsteigen 402 / 379) und hoechstens 447 Bilder Pause. Die einzige Pause ueber 600 ist das Fenster, in dem der Gator
den nach der Szene im Becken liegenden Kanister holt (851-856 Bilder,
RE2-Zielmodus, Abnahme 5 F1) - unveraendert seit Runde 1, Nutzerfrage (Abschnitt 7).

### 3b Vorgabe B - Plattform- und Rampen-Suedkante

Eigene Rueckkehr-Lagen im Wasser, Figur steht (Schuesse, dann 150 s Warten), 3600 Bilder. Haenge-Fenster und "ohne Fortschritt" ueber die TRAG-Bilder (ab dem
Aufnehmen des Kanisters, F384-F691 bis F3600; davor holt der Gator den Kanister), wie haenglog des Messblatts; Bisse und Pausen ueber den ganzen Besuch:

| Lage (Kante), Figur, Kanister | Bisse | Haenge-Fenster | ohne Fortschritt max | laengste Pause ohne Treffer | Rueckfall-Zeilen / Kanister-Sitz |
|---|---|---|---|---|---|
| (0,-20900) Plattform-Sued, Leon ohne | 32 | 0 | 14 = 0,5 s | 247 | 0 |
| dto. Leon mit (= ohne bitgleich) | 32 | 0 | 14 | 247 | 0 / 2910 Bilder, Abweichung max 0 |
| dto. Leon ohne, **a661eb8aa** | 19 | 0 | 295 = 9,8 s | 428 | 0 |
| (100,-20900) Plattform-Sued, Leon ohne | 20 | 0 | 77 | 719 (zwei RE2-Dashes F1974-F2583) | 0 |
| dto. Leon ohne, **a661eb8aa** | 23 | 0 | 299 = 10,0 s | 743 (Dashes) | 0 |
| (-1300,-20700) Plattform-Sued, Elza ohne | 30 | 0 | 35 | 311 | 0 |
| dto. Elza mit | 30 | 0 | 35 | 311 | 0 / 2970, max 0 |
| dto. Elza ohne, **a661eb8aa** | 23 | 0 | 82 | **1123 = 37 s** (F196-F1319, RE2-Dashes, Figur nach dem ersten Biss westlich der Plattform) | 0 |
| (-800,-21200) Plattform-Sued, Elza mit | 31 | 0 | 67 | 171 | 0 / 2970, max 0 |
| dto. Elza mit, **a661eb8aa** | 13 | 0 | 288 = 9,6 s | 870 | 0 / 3030, max 0 |
| (1500,-21400) Plattform-Sued, Elza ohne | 31 | 0 | 48 | 220 | 0 |
| (-500,-21800) Plattform-Sued, Leon mit | 22 | 0 | **326 = 10,9 s** (RE2-Dash vom Ziel weg, F1124-F1435) | 753 (zwei Dashes) | 0 / 3210, max 0 |
| (4073,-19322) Rampen-Sued (Haengerlage des Messblatts), Leon ohne | 30 | 0 | 51 | 277 | 0 |
| (5000,-18900) Rampen-Sued, Elza ohne | 29 | 0 | 43 | 292 | 0 |
| (2800,-18700) Rampen-Sued, Leon mit | 31 | 0 | 39 | 280 | 0 / 2940, max 0 |
| (-2300,-16000) Plattform-West, Elza ohne | 24 | 0 | 262 = 8,7 s | 418 | 0 |

Abnahme-4-Laeufe (Rueckkehr im Band vor der Rampen-Suedkante bzw. frei): Haenge-Fenster 0 in allen 10; ohne Fortschritt max 39 (west4_leon) / 157 (west4_elza) /
270 (west5_elza) / 197 (frei4_leon) / 221 (frei4_elza) Bilder; abn2_wand 47; 0 Zeilen HAENGT / RUECKWAERTS an / TAUCHEN an in jedem eigenen Lauf (auch mit
Kanister). Harness nb3 (B1/B3, Figur je Bild gesetzt) = Runde 1: Haenge-Fenster 0, ohne Fortschritt hoechstens 327, Pause 520 (mit) / 530 (ohne).

Lesart Vorgabe B: an der Plattform-Suedkante und an der Rampen-Suedkante kein Haenger - Fenster 0 ueberall, laengste Zeit ohne Fortschritt hoechstens
326 Bilder = 10,9 s (RE2-Dash, mit Kanister) gegen die Obergrenze 450 = 15 s (Messblatt B3, PORT-WAHL "Er darf nicht zu lange haengen"), ohne Rueckfall (0
Rueckwaerts/Tauchen). Gegen den gelobten Stand an denselben Lagen deutlich besser: (0,-20900) Leon 19 -> 32 Bisse, ohne
Fortschritt 295 -> 14; (-1300,-20700) Elza 23 -> 30 Bisse, laengste Pause 1123 (37 s ohne Biss) -> 311; (-800,-21200) Elza mit Kanister 13 -> 31
Bisse, 288 -> 67. Laengere Pausen ohne Biss im Wasser (bis 753 Bilder) bestehen aus zwei RE2-Dashes hintereinander (Phase 12 GBP_RE2_DASH, je 150 + 150 Bilder
Clip 2/3, gator_boss.log "RE2 DASH Vordrehung"); der Gator bewegt sich dabei, die Zeit ohne Fortschritt bleibt <= 326; dasselbe Muster zeigt der gelobte Stand
an denselben Lagen (743, 870 und 1123 Bilder). Kein Haenger im Sinn der Vorgabe B.

### 3c Bitgleich ausserhalb der beiden Orte (gegen a661eb8aa) und gegen Runde 1

- Abnahme-4-Laeufe gegen den gelobten Stand (`g2abn4/runs/`, exe d872c8e6, gleiche Eingabe; `vergleich6.py`, Abschnitt 3 in `messung_abnahme6.txt`): in allen
  10 Paaren (5 Laeufe x mit/ohne) Besuch 1 (Titel 1240/1241), Besuch 2 (erster 2090/2091-Besuch mit Szene) und Besuch 3 (2080/2081) **bitgleich**. Rueckkehr-
  Besuch: frei4_leon (mit und ohne) bitgleich bis F1491, erste Abweichung F1492, als die Figur ins Band vor der Plattform-Nordkante tritt (498,-10669); frei4_elza
  (mit und ohne) bitgleich bis F4103, erste Abweichung F4104 im Band vor der Rampen-Nordkante (6732,-12972); west4_leon / west4_elza / west5_elza beginnen im Band
  (Rueckkehr-Lagen 600-700 vor der Rampen-Suedkante), erste Abweichung F10 / F11 / F57 = Vorgabe B. Gleich wie Runde 1 (Abnahme 5 Abschnitt 3c).
- Gegen Runde 1 (Laeufe der Abnahme 5, exe 029efac6, gleiche Eingabe): frei4_leon, frei4_elza, west4_leon, rampe_oben Leon und Elza - je mit und ohne Kanister -
  Bild fuer Bild **bitgleich** (state + TRAG, 8400/3300/4400 Bilder), dazu die alte abn2_rampe_west-Eingabe ueber alle fuenf Besuche. Abweichungen nur in west5_elza
  (mit, ohne) ab F2753, west4_elza (mit, ohne) ab F1319 und abn2_wand (mit, ohne) ab F1046 - jeweils genau im Bild der ersten RAUMHALT-Zeile. Der Bau aendert
  das Spiel in diesen Laeufen nur dort, wo der Raumhalt eingreift (der Anlauf-Fix wirkt nur bei Figur oben mit Zaehler >= 300; vor dem ersten Raumhalt-Bild
  aendert er hier nichts).
- Mit gegen ohne Kanister (neue exe): frei4_leon, frei4_elza, west4_leon, west5_elza, rampe_oben Leon/Elza, abn2_wand, Kante (0,-20900) und die ctest-Laeufe
  abn2_rampe_west bitgleich; west4_elza ab F1673 (Figur im Wasser vor der Nordkante; mit Kanister rammt er auch im Wasser - so seit Abnahme 4).
- Kanister-Sitz in allen eigenen Laeufen mit Kanister "Abweichung max 0" (2520-7830 angehaengte Bilder). Rueckfall: 0 Zeilen `G2RE2 HAENGT` / `RUECKWAERTS an` /
  `TAUCHEN an` in allen 46 eigenen Laeufen und in den beiden ctest-Laeufen abn2_rampe_west - Tauchen tritt nicht auf, also auch nicht ausserhalb des Rueckfalls.
- Riegel gang / gang_elza (Harness, Lagen ausserhalb der Baender gegen die Pruefhaken rw_alt + nb3_alt) direkt: `ok: ohne Kanister Bild fuer Bild gleich NB5 ...:
  0 von 2700 abweichend`, `gang: 0 Fehler` / `gang_elza: 0 Fehler` (`harness/gang*.txt`); die exe-Riegel gang_szene / gang_montage (Leon, Elza) siehe Abschnitt 2.
  Hinweis: die Pruefhaken dieses Riegels schalten den Raumhalt nicht ab - dass der Raumhalt ausserhalb der Loecher nichts aendert, belegen die exe-Vergleiche
  mit der echten exe des gelobten Stands und mit Runde 1 (oben).

### 3d Die Befunde der Abnahme 5 nachgemessen

- **M1 Absturz abn2_wand - behoben.** Eigener Lauf mit der Eingabe des exe-Riegels (Umgebung wortgleich) bis F8400: ohne Kanister exit 0 (Runde 1: exit 139,
  `0xC0000005` in F7374), mit Kanister exit 0; Figur in keinem Bild ausserhalb (Runde 1, mit dem korrigierten Auswerter gezaehlt: ohne 5602 Bilder aussen + 569
  Ostnische bis zum Absturz, mit 6628 + 569). Einziger Eingriff: `RAUMHALT F1046 Band 0 (7668,-17599) -> (6732,-18598) vorher (6732,-18598) hr=1 mo=13` - genau der
  Biss-Niederwurf, der in Runde 1 in die Ostnische fuehrte (gleiches Bild, gleiche Lage; bis F1045 bitgleich zu Runde 1). Danach steht Leon in der SO-Tasche bzw. an der Suedwand
  und wird weiter gebissen (74 Bisse, ohne Fortschritt max 47). exe-Riegel abn2_wand gruen (643,21 s).
- **M2 Wurf aus dem Raum - behoben.** In keinem der 46 eigenen Laeufe ein Bild ausserhalb des Decks oder des Raums (`aussen6.py` ueber alle Besuche von 2090/2091),
  auch nicht in den zehn eigenen Oben-Lagen (stehend und gehend, alle Blickrichtungen) und in den beiden ctest-Laeufen abn2_rampe_west. west5_elza: F2752
  CROSS-Biss an der SW-Ecke des Decks (2315,-17642), F2753 RAUMHALT -> (2315,-17632); Runde 1 flog sie hier durch die Suedwand (548 Bilder ausserhalb). Die
  Oben-Lagen der Abnahme 5 mit Wurf ((2318,-16300), (6500,-15000), (2318,-17600)) prueft der exe-Riegel oben_kante(_elza): gruen (488,19 s / 526,95 s), in meinem
  Lauf mit den eigenen Auswertern nachgerechnet (Arbeitsordner `lauf_6519ae3d` / `lauf_2d5590b4`), Leon = Elza Bild fuer Bild: 0 Bilder ausserhalb; 20 / 22 / 22
  Treffer, erster nach Kampfbeginn 363 / 84 / 142, laengste Pause 375 / 345 / 380; an (2318,-16300) greift der Raumhalt ab F1202 - das Bild nach dem Treffer, mit
  dem die Figur in Runde 1 durch die Nordwand flog.
- **M3 abn2_rampe_west - Riegel gruen, Ursache im Spiel bleibt.** exe-Riegel gruen (616,99 s); er misst jetzt Leon oben an der Rampe West (oben 7897 Bilder,
  37 Treffer, Abschnitt 3a). Die ALTE Eingabe (Rueckkehr (4200,-18700), 20 s im Band, dann Kletterversuche, Abbruch Bild 1300) mit der neuen exe: Rueckkehr-Besuch
  937 Bilder, 7 Bisse tragen Leon im RE1.5-Niederwurf von (4959,-18568) ueber (6098,-23886) bis (-7830,-26532) in die SW-Ecke, die blinde Aktionstaste oeffnet dort
  die Tuer (`DOOR FIRE slot=0 rect=(-8900,-25500,...)`) -> ROOM2080 - Bild fuer Bild gleich Runde 1. Der gelobte Stand bleibt mit dieser Eingabe 1300 Bilder in
  2090 (Lauf der Abnahme 5): er beisst dort im Band nur dreimal (F153, F289, F397), Leon bleibt vor der Rampe, klettert hinauf (oben ab ~F800) und wird oben
  gebissen (F1121). M3 ist also im Szenario geloest, nicht im Spiel (wie der Bauer in Offen 1 selbst schreibt); kein Spielfehler im engen Sinn (Abnahme 5), aber
  eine Folge der haeufigeren Bisse im Band (Vorgabe B) zusammen mit dem langen RE1.5-Niederwurf - Frage F2 (Abschnitt 7).

### 3e Wann der Raumhalt eingreift

- Alle RAUMHALT-Zeilen aller eigenen neuen Laeufe (`messung_abnahme6.txt` Abschnitt 4): 134 Zeilen, **ausschliesslich waehrend eines Niederwurfs** (hr=1, Clip 13):
  131 auf Band 1 (oben), 3 auf Band 0 (dieselbe Ostnischen-Stelle F1046 in den drei abn2_wand-Laeufen). Keine Zeile beim Gehen, Stehen, Klettern oder Absteigen:
  die zwei Gehen-Laeufe oben (2618 Bilder mit Pad-Eingabe, Figur ueber das ganze Deck x 2302..6749, z -17650..-14951) haben 0 Eingriffe, das Klettern
  (rampe_oben F48-F82, abn2_rampe_west) und der Abstieg per Aktionstaste (west4_elza F1627-F1666) laufen unberuehrt. (hr = Treffer-Sperrbit +0x93 bit0x1, d. h.
  die Figur wurde in diesem Angriff getroffen.) In den Arbeitsordnern meiner ctest-Laeufe dazu 166 RAUMHALT-Zeilen: 164 im Niederwurf (Clip 13), 2 im
  Treffer-Clip 11 (Riegel frei mit und ohne Kanister, F2098: Stoss aus der Taschen-Ecke (2318,-18568) in die Plattform (1382,-17613), auf die Lage davor
  zurueckgesetzt; Pad-Eingabe wirkungslos, die Figur wird geschoben) - also auch dort nur in einer Trefferreaktion, nie in einem Geh- oder Lauf-Clip (105 / 100).
- Oben (Band 1): ein Niederwurf, der die Figur ueber die Deckkante (bzw. per Leistenschub auf die Klemmlage (1382,-18617) / (1382,-14013)) tragen wuerde, endet auf
  der Klemmlage der Deckkante (2318 bzw. 6732, -17632 bzw. -14968). Im Bild spielt die Figur dort Clip 13/14 im Stand ab und wirkt wie taumelnd - sie liegt nicht
  und fliegt nicht (Leiste `abn6_west5_elza_r2_pose_zoom.jpg`, F2750-F2815; vgl. Offen 3 des Bauers, Frage F3).
- Unten (Band 0): die langen Niederwuerfe im Wasser bleiben unangetastet (RE1.5, `s_kd_speed` 1000 @0x8003615c): `flug6.py` misst nach einem Biss bis 13 059
  Einheiten Luftlinie bis zum Stillstand (frei4_leon F1082, im bitgleichen Teil; an den Kanten bis 11 323), im gelobten Stand bis 16 343; der Raumhalt stoppt nur
  den Schritt durch eine Aussenwand bzw. in einen Block.

## 4. Bild (RE15_FRAMEDUMP-Serien, F-genau, selbst angesehen) - `G2_belege/abn6/`

"_hell" = Belegkopie aufgehellt (PIL Brightness x2,8-3,0, `werkzeug/hell.py`), Originale im Laufordner `<SP>/leisten/`.
- `abn6_oben_leon_elza_mit_ohne_F90-F4320.jpg` (Riegel-Szenario rampe_oben, Spalten Leon ohne | Leon mit | Elza ohne | Elza mit, Cut 2): F90 Figur oben vor der
  Maschine, Gator-Kopf vorn im Wasser; F390 Rammen (Koerper quert unter der Figur, F383); F480 Querungsbiss F483; **F750 und F1020 Figur allein oben** (das
  Kanister-Fenster F483-F1339); F1350 Figur liegt (Rammen F1339); F1440 CROSS-Biss, in den Spalten "mit" der Kanister am Kiefer; F1590 der Gator richtet sich
  neben der Figur ueber dem Deck auf (Hochbiss F1585); F1710 Figur liegt, der Kopf riesig vor der Kamera (Hochbiss F1690); F2430/F2520/F2970/F3750 Rammen bzw.
  CROSS-Biss an der Figur; F3090 Kopf mit rotem Maul an der Deckkante (Figur verdeckt); F4320 Figur steht oben. Die Figur ist in jedem Bild auf dem Deck; mit =
  ohne bildgleich bis auf den Kanister, Leon = Elza im Ablauf. `abn6_oben_ausschnitt_hell_F1350-F1710.jpg` = Zeilen F1350-F2430 aufgehellt.
- `abn6_west4_elza_west5_elza_oben_hell.jpg` (Abnahme-4-Laeufe; Spalten west4_elza ohne | mit | west5_elza ohne | mit): west4_elza F690-F1530 Elza oben am Ostende -
  F1080 Rammen, F1200 CROSS-Biss, **F1320 steht sie auf dem Deck**, ein Bild nach RAUMHALT F1319 (Runde 1 hier (7668,-1800,-18587), ausserhalb des Decks), F1440
  und F1530 Hochbiss; F1650 Schnitt (Cut 3), Elza nach dem Abstieg F1627 im Nordbecken, ab F1860 Bisse im Wasser. west5_elza: ab F1860 Elza oben an der SW-Ecke
  des Decks, F2220 CROSS-Biss, F2760 nach CROSS-Biss + Raumhalt F2752/F2753 weiter oben, F2820 Rammen (Spalte "mit": Kanister am Kiefer), F2940/F3030 Hochbiss.
- `abn6_west5_elza_ohne_runde1_runde2_F2745-F2830_hell.jpg` (Spalten 10603425b | 9b83eadfe): F2745/F2750 gleich (Elza an der SW-Ecke, Gator-Kopf daneben); Runde 1
  **F2755-F2770 fliegt Elza waagerecht ueber das Wasser zur Wand**, ab F2780 ist sie weg; Runde 2 bleibt sie an der Maschine stehen, der Gator liegt an ihr,
  F2800/F2830 sein Kopf mit rotem Maul an ihr (Rammen F2817).
- `abn6_west5_elza_r2_pose_zoom.jpg` (Runde 2, Ausschnitt x2): F2750 steht; F2755-F2790 Clip 13/14 im Stand - geduckt, eingeknickt, taumelnd; F2800 wieder
  aufrecht. Sie liegt nicht und fliegt nicht (Offen 3, Frage F3).
- `abn6_abn2wand_leon_ohne_runde1_runde2_F1380-F1520_hell.jpg` (Riegel-Szenario abn2_wand ohne Kanister): Runde 1 Leon klein rechts in der Ostnische, ab F1500
  verschwunden (hinter die Nischen-Rueckwand versetzt); Runde 2 Leon an der Suedwand neben dem Gator (Kamera der Suedwand), in jedem Bild im Raum.
- `abn6_plattform_sued_leon_ohne_gelobt_jetzt_F150-F1110_hell.jpg` (Lage (100,-20900), Spalten a661eb8aa | 9b83eadfe): F150 in beiden der Gator an der
  Plattform-Suedkante, neu beisst er Leon; F270 neu steht Leon gebissen neben dem Gator, alt ist die Figur aus dem Bild getragen (leerer Blick bis F1110); neu
  folgen die Kameras Leon (Cut 1/0) an Ost- und Suedwand, der Gator jeweils an ihm. In beiden Staenden tragen die Biss-Niederwuerfe die Figur von der Kante weg.
- Die eigenen Oben-Lagen (Spawn) behalten Cut 0 (Nordwand mit Leiter und Tor), die Figur oben ist dort nicht im Bild - dafuer gelten nur die Logs (wie Abnahme 5).

## 5. Die offenen Punkte des Bauers - Einstufung (eigene Messung)

1. M3 ueber das Szenario geloest, Niederwurf im Wasser bleibt RE1.5: **bestaetigt** (Abschnitt 3d: alte Eingabe -> SW-Tuer, bitgleich Runde 1). Die langen
   Niederwuerfe sind auch im gelobten Stand da (Abschnitt 3e). Die RE2-Stelle selbst nachgelesen (`analysis/befunde_runde41/herkunft/alligator_werkzeuge/
   re2_em23.dis`): @0x80101684 `jal 0x8001569c`, @0x80101688 `addiu a2,zero,1536`, a1 = 0/1024/2048/3072 je nach Lage von s1 (`slti v0,v0,-8999` /
   `slti v0,v0,-15599` @0x801015f8 / @0x80101630); FUN_8001569c (RE2_Quellcode_V2/FUN_8001569c.c) dreht den Winkel +0x76 seines ersten Arguments um hoechstens
   param_3 auf param_2 zu. Dass das "die RE2-Trefferreaktion" ist, ist die Lesart des Bauers - ein Weg in Einheiten ist dort nicht belegt. Nicht blockierend;
   Nutzerfrage F2.
2. `re15_squareroot0` liest bei Ueberlauf ausserhalb der Tafel: **bestaetigt als nicht mehr erreicht** - alle 46 eigenen Laeufe exit 0, keine Ausnahme-Zeile, keine
   Figur ausserhalb. Der Nachbau ist unveraendert (latenter Fehler nur bei ~100 000 Einheiten Abstand). Nicht blockierend.
3. Raumhalt stoppt den Niederwurf oben ohne Wand-Slam: **bestaetigt und im Bild gesehen** (Clip 13/14 im Stand, wirkt wie Taumeln, Abschnitte 3e/4). Die Sonde
   FUN_8001c2dc setzt fuer die Leisten Flag 0 (`andi v0,v1,0x2` @0x8001c390 ... `sb zero,0x0(s2)` @0x8001c3ac, selbst nachgelesen) - kein Slam ist byte-true.
   Kosmetisch, nicht blockierend; Nutzerfrage F3.
4. `gb_pl_nische` / `gb_pl_im_block` toter Code: plausibel - in keinem eigenen Lauf ein Bild in der Ostnische oder im Block. Nicht blockierend (aufraeumen spaeter).
5. Kanister-Zielmodus waehrend der Belagerung: **bestaetigt** - 856 / 854 / 851 Bilder ohne Treffer oben (rampe_oben Leon/Elza, abn2_rampe_west), solange der Gator
   den Kanister aus dem Becken holt; danach hoechstens 343-375. Unveraendert seit Runde 1, Nutzerfrage F1 (aus Abnahme 5 offen).
6. Blinde Wand-Touren treffen ihre alten Orte nicht mehr: bestaetigt (abn2_wand bitgleich Runde 1 bis F1045); die Riegel laufen als "kein Absturz, Figur im Raum,
   mit = ohne" - gruen. Nicht blockierend.
7. Spawn-Lagen in Cut 0: bestaetigt (auch meine zehn Oben-Lagen); die Bilder kommen aus den Szenarien mit Cut 2. Nicht blockierend.
8. Gegenprobe raum schaltet Raumhalt und Anlauf-Fix nur gemeinsam ab: bestaetigt im Code (`quer_anl = quer_frei && !s_gb_raum_alt`). Den Anlauf-Fix allein belegt nur
   die Harness-Messung des Bauers (Pause 1195 vorher). In meinen Laeufen mit Runde-1-Vergleich aendert er vor dem ersten Raumhalt-Bild nichts (rampe_oben
   bitgleich); ob er in den eigenen Oben-Lagen griff, zeigen die Logs nicht (QUERPRUEF-Zeilen gibt es dort 0-68 je Lauf) - dort ist die Pause <= 447. Nicht
   blockierend.
9. PSX-Bau nicht gemessen: bestaetigt (auch hier nicht).
10. Querbezug re2gator (EXIT_AT 1700): **bestaetigt gruen** in meiner Gruppe (275,99 s, `r41_re2gator: ok`; kanister: 240 angehaengte Bilder, Abweichung max 0;
    belagerung: GIER-WACHE bis F581, 300 Bildpaare CHASE/GUARD). Ob das Fenster 1700 bleibt, entscheiden die Spur-Eigner (alligator2 / gatorszene). Nicht blockierend.

## 6. Konstanten im Diff e92356e8e..9b83eadfe (neu in Runde 2: 10603425b..9b83eadfe; Runde 1 hat die Abnahme 5 Abschnitt 7 geprueft)

Engine (geaenderte Zeilen laut `git diff --stat`): enemy_ai_boss_gator.c 86, game_step_common.c 9, re15_boss_gator.h 6; Tests: test_r41_gatorfix.c 210, test_r41_gatorfix.cmake 65,
probes/r41_gatorfix.cmake 12. Jede Zahl selbst nachgeprueft (ghidra1_V2.txt, PSX.EXE, ROOM2090.RDT):

| Zahl | Stelle | Beleg (selbst nachgelesen) | Bewertung |
|---|---|---|---|
| `GB_PR 450` (Spielerradius) | Raumhalt: Deck = Rampe [5] +- 450, Wasser = Aussenwand - 450 | DAT_80073e94 = {0,-1530,0,450,1530,450} (PSX.EXE Datei 0x64694, selbst gelesen), +6 = 450; Zeiger DAT_800acacc = &DAT_80073e94 (`sw v1,-0x3534(at)` @0x80031664); zitiert: `lhu a1,0x6(v0)` @0x80036210 | belegt. Formal: @0x80036210 ist die Last fuer die Niederwurf-Sonde FUN_8001c2dc; die Klemme FUN_8003b0a4 bekommt denselben Wert ueber `lhu a1,0x6(v0)` @0x80031d6c vor `jal FUN_8003b0a4` @0x80031d70 (gleiches Feld, gleicher Wert) |
| `GB_PRK = GB_PR + 18` | Klemmlage | FUN_8003bca8: `addiu t1,v0,0x12` @0x8003bce0, `addiu a0,a0,-0x12` @0x8003bce8 (x); `addiu t0,v0,0x12` @0x8003bd20, `addiu a0,a0,-0x12` @0x8003bd2c (z) | belegt |
| -8900 / 7200 / -27000 / -5400 | `gb_wasser_gueltig`; Test raum / aussenlog | ROOM2090.RDT = ROOM2091.RDT (md5 a48d11a4), SCA-Kopf @0x6dc (Zeiger RDT+0x20), Zellen je 12 B: [0] @0x6f4 x -10900..-8900, [1] @0x700 z -29000..-27000, [2] @0x70c z -5400..-3400, [3] @0x718 x 7200..9200 (`werkzeug/sca6.py`, `sca_2090.txt`) | belegt |
| Deck/Block-Rechtecke (`GB_RAMP_*`, `GB_PLAT_*` bestehend; `RM_*` im Test) | Raumhalt, Test | [5] @0x730 x 1850..7200 z -18100..-14500; [4] @0x724 x -1700..1850 z -20000..-12450; Band 1 nur [6] @0x73c Ostwand, [7] @0x748 Plattform, [8] @0x754 z -14500..-13500, [9] @0x760 z -19100..-18100 (u1 0x02, floor 0x10) | belegt |
| Raumhalt als Regel (oben auf das Deck klemmen, unten Lage vor dem Schritt) | spieler_klemme, Boss-Push | PORT-WAHL mit Nutzer-Wortlaut Vorgabe A im Code-Kommentar (enemy_ai_boss_gator.c ueber `GB_PR`, game_step_common.c) und in der Commit-Nachricht 5a94392bf | belegt (PORT-WAHL) |
| Anlauf ohne Ruhezone (`quer_anl`) | GUARD | keine neue Zahl: nimmt nur die bestehende 600er-Ruhezone (`gdx*gdx + gdz*gdz > 600*600`) fuer den Anlauf der Zaehler-Querung heraus; `GB_OBEN_OHNE 300` bestehend (Abnahme 5) | ohne neue Zahl |
| Kommentar-Zitate: Niederwurf 1000, -15/Bild, Sonde Bit 0x2 | Kommentare | `ori v0,zero,0x3e8` @0x8003615c; `lbu t0,..`/`sll v0,t0,0x4`/`subu v0,v0,t0`/`subu v1,v1,v0` @0x800361c8-d8, `sh` @0x800361e0; `andi v0,v1,0x2` @0x8001c390 -> `sb zero,0x0(s2)` @0x8001c3ac | belegt |
| `NB3_OHNE_MAX 450` | Test | Kommentar jetzt mit "PORT-WAHL" und dem Nutzer-Wortlaut der Vorgabe B (formaler Rest der Abnahme 5 erledigt) | belegt |
| `NB3_PAUSE_MAX 600` | Test | Messblatt A1 (300 + 240 + 45 = 585, aufgerundet), PORT-WAHL im Messblatt (unveraendert, Abnahme 5) | belegt |
| raum: 9 + 7 Lagen, Blick 1024/3072/1035, 2400 Bilder (`G2_RAUM_BILDER`) | Test | Lagen der Abnahme 5 bzw. ihrer exe-Laeufe; Laufdauer ohne Verhalten | belegt / ohne Belang |
| aussenlog: Klettern = climb.log `sub != 0` | Test | climb_common.c Substates 9/10; gemessen auch hier (west4_elza F1627-F1666 sub 9/10) | belegt |
| obenlog kampf: "KAMERA INTRO frei ... F<n>" | Test | Log-Zeile von gr2_kam_frei; in allen eigenen Laeufen F280/F282 | belegt |
| oben_kante-Lagen (2318,-16300), (6500,-15000), (2318,-17600) | cmake | Lagen der Abnahme 5 (Wurf F1201 / F1209, Klemmlage) | belegt |
| abn2_rampe_west neu: Eingabe wie rampe_oben + W260, Rueckkehr (2318,-18750), Ende 8000 | cmake | Szenario-Parameter (Kletterpose 650 vor der Kante wie rampe_oben, Lage x 2318 des Abnahme-2-Laufs plat7_leon) | Szenario, kein Verhalten |

Die Commit-Nachricht 5a94392bf nennt @0x80036210, @0x8003bce0-e8, @0x8003bd20-2c, @0x8003615c, @0x800361c8-e0, @0x8001c390-ac und die SCA-Offsets. Keine
Zahl ohne Beleg gefunden.

## 7. Fragen an den Nutzer (fuer die Release Notes "Zu entscheiden", Sie-Form)

- F1 (seit Abnahme 5 offen): "Nach der Szene liegt der Kanister im Becken. Solange der Alligator ihn holt, greift er Sie oben auf der Rampe nicht an - in den
  Messlaeufen knapp 28 Sekunden lang, nach den ersten ein bis zwei Treffern. Danach greift er Sie oben spaetestens alle 11 bis 13 Sekunden an. Ist diese eine
  Pause fuer Sie in Ordnung (den Takt haben Sie freigegeben), oder soll er Sie auch angreifen, waehrend er den Kanister holt?"
- F2: "Ein Biss im Wasser wirft Ihre Figur weit weg (Niederwurf aus Resident Evil 1.5, in den Messlaeufen bis gut 13 000 Einheiten, mehr als die halbe
  Beckenlaenge) - oft bis an eine Wand, in einem Messlauf bis vor die Suedwest-Tuer. Das war im gelobten Stand schon so; weil er an den Kanten jetzt beisst statt
  zu haengen, passiert es oefter. Soll das so bleiben, oder soll der Biss die Figur nur kurz wegstossen (die Trefferreaktion aus Resident Evil 2 waere dafuer
  erst nachzumessen)?"
- F3: "Wird Ihre Figur oben an der Rampenkante gerammt oder gebissen, bleibt sie auf der Rampe: sie taumelt kurz an der Kante, faellt aber weder hin noch ins
  Wasser. Soll das so bleiben, oder soll ein solcher Treffer die Figur von der Rampe ins Wasser stossen?"

## 8. Tests (Summenzeilen woertlich)

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`build_check.txt`).
- `ctest --test-dir "<Baum>/re15_port/build" -R "^unit_r41_gatorfix_" -j 2 --timeout 900` (13:16:47-13:18:17, `ctest_unit.txt`): 
  **`100% tests passed, 0 tests failed out of 28`**, `Total Test time (real) =  89.29 sec`.
- `test_r41_gatorfix.exe raum` / `raum_elza` direkt (2400 und mit `G2_RAUM_BILDER=3600`): je `raum: 0 Fehler` bzw. `raum_elza: 0 Fehler`, exit 0 (`harness/raum*.txt`);
  `test_r41_gatorfix.exe nb3` / `nb3_elza` direkt: `nb3: 0 Fehler` / `nb3_elza: 0 Fehler`, SUMME-Zeilen woertlich gleich dem Beleg der Abnahme 5
  (`G2_belege/abn5/harness/riegel_nb3_direkt.txt`).
- `ctest --test-dir "<Baum>/re15_port/build" -R "^integration_r41_gatorfix_|^integration_r41_re2gator" -j 2 --timeout 1800 -V` (13:18:30-14:31:36, `ctest_exe.txt`): **`100% tests passed, 0 tests failed out of 20`**, `Total Test time (real) = 4384.99 sec`.
- Eigene exe-Laeufe: 46, alle `exit=0` (`messung_abnahme6.txt` Abschnitt 1).

## 9. Nicht geprueft

PSX-Bau; Raumwechsel mitten im Kampf; Hoeren; der Spieler stirbt (alle Laeufe mit dem hp-Messhaken 100); Oben-Lagen mit dem Kanister im Maul im eigenen
Spawn-Aufbau (dort wird kein Kanister aufgenommen; "mit Kanister oben" belegen das Riegel-Szenario rampe_oben, abn2_rampe_west und west4_elza / west5_elza); die
Kanister-Explosion (Tod des Gators) mit Figur oben; Figur oben auf der Plattform (Band 1 dort nicht begehbar, Abnahme 5).
