# Runde 41 - Spur G2 "gator_re2" - UNABHAENGIGE ABNAHME 4 (2026-11-06): G2-RE2 NB2 "Vorschlag 4"

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Kopf c432eb1d2 (Vorschlag 4: 4b794e625 Bau, 77d9a9ebc Riegel, c06bcfefa Pause/exe-Riegel,
bd3236909 tragvergleich, c432eb1d2 Dossier/Belege). Pruefer: Abnahme-Agent, glaubt dem Bau nichts, misst selbst. Kein Spielcode geaendert.
Hinweis zur Zaehlung: auf dem verworfenen Zweig r41/gatorfix gibt es bereits eine `G2_abnahme_4.md` (29cb53610, Abnahme des NB5-Stands bc5fb7c9c, 2026-11-02,
die Quelle der Laeufe west4/west5/frei4); diese Datei hier ist die vierte Abnahme auf r41/gator_re2 und wird vom Auftrag so benannt.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn4`
(je Lauf `runs/<neu|ohne>_<lauf>/` mit video.mp4, fd_NNNNNN.ppm (Framedump-Serie), debug.log, state.log, gator_boss.log, gator_trag.log, gator_zeit.log, climb.log,
env.txt, pid.txt, exe_md5.txt). Belege: `G2_belege/abn4/` (Bildleisten, `messung_abnahme4.txt`, `ctest_*.txt`, `fin/`, `werkzeug/`).

Spezifikation = Dossier G2_gatorfix.md Abschnitt "Nutzerentscheid (Koordinator, 2026-11-03)" (Vorschlag 4): (1) der getragene Kanister zaehlt in keiner
Bewegungs-, Kollisions- und Wegrechnung; (2) der Kanister sitzt in jedem Bild synchron am Kiefer, Versatz 0; (3) auf der Rampe rammt er wie gehabt, keine Deckung;
(4) Rueckwaerts/Tauchen nur als Rueckfall. Dazu die Regel "gelobter Stand ist Referenz" (NB5 bc5fb7c9c, Gang ohne Kanister bitgleich; kein Umbau ohne Ja).

## 0. Urteil

**BESTANDEN** (nicht blockierende Restmaengel und drei Nutzerfragen, Abschnitt 5). Begruendung in Kurzform:
- Punkt 1 (Kanister zaehlt nicht): im Harness Bild fuer Bild mit = ohne Kanister in 9 Wasser-Lagen x 900 Bildern (0 abweichend, Leon und Elza), Gegenprobe
  Huelle aktiv rot (11 697 abweichend); im exe (eigene Laeufe, eigene exe-Kopien) mit = ohne bis zur ersten Querung mit Rammen bzw. bis zum Rueckfall
  (frei4_leon/frei4_elza 0 abweichende Bilder in 7805/7454; west4_leon erste Abweichung F868 in GUARD (Rammen F880), west5_elza F1934 nach dem Rammen F1103,
  west4_elza F1444 nach dem Rammen F1183).
- Punkt 2 (Halterung): Harness 2 x 11 700 getragene Bilder Lage 0 / Drehung 0, Gegenprobe "ein Bild alte Pose" rot (300/300, max 12 281); exe "Kanister-Sitz"
  Abweichung max 0 in allen eigenen Laeufen (2610-7830 angehaengte Bilder je Lauf); im Bild (Framedumps, 30 B/s) haengt der Kanister beim Hochbiss, beim Wenden,
  in der Querung und beim Rammen am Kiefer, kein Nachziehen, kein Springen (Abschnitt 3, Bildleisten).
- Punkt 3 (Rampe rammen, keine Deckung): west4_leon Rammen F880 und F1731 (2 Querungen mit Treffer, hp 100 -> 50, Niederwurf), west5_elza Rammen F1103 (Elza von
  der Rampe ins Wasser geworfen) und F1864; Harness: Treffer in jeder Rampen-Lage, Rampe-x2318 19 Treffer gegen 3 ohne Kanister.
- Punkt 4 (Rueckfall): Harness 0 Bilder Rueckwaerts/Tauchen in 13 Lagen; exe: Tauchen 0 in allen Laeufen, Rueckwaerts nur in west4_elza (2 Durchgaenge, 2,1 %)
  nach dem Rammen im Nordbecken-Ost; sonst 0.
- Gelobter Stand: Gang ohne Kanister = NB5 (Riegel gang 0/2700 abweichend); vor der Aufnahme sind die Spielerereignisse (Bisse, Klettern F639/F640) in den
  NB5-Videos und in den neuen Laeufen bildgleich; kein Rueckschritt gegen NB5 gefunden (Abschnitt 3).
- Kein Absturz (alle Laeufe exit 0 bis RE15_EXIT_AT, keine error/assert-Zeile), kein Haenger im Sinn der Vorgabe (laengste Zeit ohne Fortschritt zur Figur
  mit Kanister 4,1-11,1 s, Drehen auf der Stelle ganz fern der Figur 0 % ausser west4_elza, dort nach dem Rammen - Abschnitt 4 Offen 2), kein fehlender Punkt.

| Punkt | Soll | Ist (eigene Messung) | Bewertung |
|---|---|---|---|
| 1 Kanister zaehlt nicht | Bewegung mit = ohne Kanister (bitgleich), Rampe bis zur Querung | Harness 0/8100 Wasser-Bilder abweichend, Rampe ab Bild 766/769 (Querung); exe 0 abweichend (frei4), sonst erste Abweichung in GUARD/nach Rammen | erfuellt |
| 2 Halterung Versatz 0 | jedes Bild Part 6 x Lokallage, Lage + Drehung | Harness 0/0 in 23 400 Bildern, Gegenprobe rot; exe max 0 in 5 + 5 Laeufen; Bild: am Kiefer in Hochbiss/Querung/Rammen (30 B/s) | erfuellt |
| 3 Rampe rammen, keine Deckung | Treffer/Rammen wie gehabt | exe west4_leon 2 Rammen, west5_elza 2 Rammen (f942: 1 / 7 - andere Pause, Nutzerfrage F3); Harness 4/4 Rampenlagen Treffer | erfuellt (Takt der Querungen = Nutzerfrage) |
| 4 Rueckfall nur als Rueckfall | Rueckwaerts/Tauchen nur, wenn er auch ohne Kanister haengt | Tauchen 0; Rueckwaerts 2 Durchgaenge in west4_elza (F2129/F2452, Haenge-Fenster 150 Bilder) - dort haengt er ohne Kanister nicht (ohne: 0 Fenster), Abschnitt 4 Offen 2 | erfuellt mit Rest (nicht blockierend) |
| Gang ohne Kanister = NB5 | bitgleich | Riegel gang/gang_elza 0/2700; Gegenprobe RE1.5-Tempo 2699 | erfuellt |
| Riegel unit_r41_gatorfix_* | gruen | `100% tests passed, 0 tests failed out of 24` (40,69 s) | gruen |
| exe-Riegel integration_r41_gatorfix_{rampe,rampe_elza,abn2_wand_elza,frei,frei_elza}, integration_r41_re2gator | gruen | `100% tests passed, 0 tests failed out of 6` (1599,82 s, -j 2, kein Lauf wiederholt) | gruen |
| Bild (Framedumps + NB5-Video) | Kanister am Kiefer, kein Rueckschritt gegen NB5 | 8 Bildleisten (Abschnitt 3): Kanister in jedem Bild am Kiefer (auch 30 B/s), auf der Rampe Querung + Rammen, in west4_elza/frei4_leon Angriff statt der NB5-Haenger (70,2 s / 14,4 s) | erfuellt |

## 1. Bau, Messweg

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`; exe
  `re15_port/build/platform/pc/re15_pc.exe` 2026-11-06 18:37:52, md5 d872c8e64bb6914619ec2d14d48c3433; juengste Quelle enemy_ai_boss_gator.c 17:56:52,
  gator_re2_r41.c 17:41, gator_rw_r41.c 17:11, gator_re2_pc.c 17:36 - alle vor dem Bau; der Kopf c432eb1d2 (19:53) aendert nur Dossier/Belege, c06bcfefa
  (18:41) wurde vor dem Bau 18:37 im Arbeitsbaum geschrieben (Quellen 17:56). Arbeitsbaum sauber. Der Bauer mass mit einer exe aus einem zweiten Baum
  (build_v4b, md5 f7037901...); meine Laeufe mit der exe des Hauptbaus sind in den TRAG-Zeilen bitgleich zu seinen (Abschnitt 2).
- Harness: `ctest --test-dir re15_port/build -R "^unit_r41_gatorfix_" -j 3 --timeout 900` (Git-Bash, `C:/msys64/mingw64/bin` vorn, SDL_ASSERT=always_ignore);
  dazu `test_r41_gatorfix.exe v4 / v4_elza / re2 / re2_elza / gang / gang_elza` direkt (Ausgaben `G2_belege/abn4/fin/`), Riegel-Zeilen ohne Rechenzeiten
  wortgleich mit den Bauer-Belegen `G2_belege/v4/fin_*.txt`.
- exe-Laeufe: Eingaben/Umgebung der Abnahme-4-Laeufe west4_leon, west5_elza, west4_elza, frei4_leon, frei4_elza (Stapel `werkzeug/batch2.sh` = wortgleicher
  Kern des Bauer-Stapels `G2_belege/v4/werkzeug/batch.sh`; Eingaben west_input.txt, frei_leon4.txt, frei_elza4.txt md5 96797cb6 / 14cd3db6 / 40e23a92),
  je einmal mit Kanister ("neu") und einmal mit dem Messhaken RE15_G2_OHNE_KANISTER=1 ("ohne"). Je Lauf eigene exe-Kopie `re15_pc_g2abn4_<lauf>.exe`
  (nach dem Lauf geloescht), gdigrab auf das hwnd DIESES Prozesses, 30 Bilder/s, RE15_WINDOW_SCALE=2 (640 x 480), kein AUTOSHOT, kein SOFTWARE_RENDER,
  Ton an; nur eigene PIDs beendet (eine erste Serie wurde nach fuenf Laeufen abgebrochen, s. u.; eigene msys-PIDs per kill, eigene exe per PID).
- **Befund Messweg (gdigrab):** `-i hwnd=<eigenes Fenster>` liefert den BILDSCHIRMAUSSCHNITT des Fensters, nicht dessen Inhalt - im Lauf neu_west4_leon
  der ersten Serie (Fenster zentriert wie bei allen Agenten) lag ab ~55 s ein fremdes, stehendes Spielfenster (parallel laufen Birkin-Workflows und eine
  Integration, zu dem Zeitpunkt 5 fremde re15_pc-Prozesse) ueber meinem: das Video zeigte 60 s lang ein unbewegtes Cut-2-Bild, waehrend die Logs
  (bitgleich zum Bauer-Lauf) Querung, Rammen und Hochbiss enthielten. Abhilfe (zweite Serie, alle hier gewerteten Bilder): das Fenster wird nach dem
  Erscheinen per SetWindowPos in die rechte obere Ecke (1264,0)-(1920,519) geschoben (`werkzeug/lauf2.sh`), wo kein zentriertes Fenster (640x480 oder
  320x240 der ctest-Laeufe) hineinragt; zusaetzlich je Lauf eine RE15_FRAMEDUMP-Serie (komponiertes Bild unmittelbar VOR dem Present zurueckgelesen,
  platform/pc/main.c, render_pc.c re15_render_pc_request_readback - nicht AUTOSHOT, nicht SOFTWARE_RENDER), damit die Bildleisten Spielbild-genau (F)
  und von fremden Fenstern unabhaengig sind. Die Logs beider Serien sind identisch (deterministisch). Das Video <-> Bild-Mass der Bauer-Leisten
  (t = F/30 + Versatz) traegt +-1-2 s (Spiel 29-30 Bilder/s unter Last); meine Leisten der neuen Laeufe sind darum Framedumps, die NB5-Spalte (altes Video
  der Abnahme 4 vom 2026-11-02, exe 0b6bea7f) ist per Anker (Kletter-Animation mo=231 F639/F640 bzw. cam-Wechsel) auf +-1 s zugeordnet.
- Auswerter: `werkzeug/mass_v4.py` (Bauer-Werkzeug, 1:1 uebernommen; Kopfpunkt 3236 vor der Wurzel, Drehen "ganz fern" = 120-Bilder-Fenster mit >= 360 Grad
  Gier, Wurzel-Kasten < 3000, Kopf im ganzen Fenster > 4500), eigener `werkzeug/fenster.py` (Kennzahlen eines Bildfensters, z. B. nach dem Rammen),
  `test_r41_gatorfix.exe tragvergleich / traglog` (Bauer-Werkzeug aus dem eigenen Bau) als Gegenrechnung; NB5-Referenz mit demselben Mass auf den alten
  Laeufen (`mass_nb5.txt`).

## 2. Determinismus und Zahlen der eigenen exe-Laeufe

Alle TRAG-Zeilen (Phase, Clip, Bild, Gier, Lage je Bild) meiner Laeufe sind bitgleich mit den Bauer-Laeufen gleicher Eingabe (scratchpad g2v4/runs, exe
f7037901 aus build_v4b; frei4-Laeufe des Bauers mit der exe d2290ddb, Stand 4b794e625): neu/ohne west4_leon (2650 Bilder), west5_elza (2596), west4_elza
(2688) - zweimal gemessen (erste und zweite Serie) -, frei4_leon (7805), frei4_elza (7454); der Burst-Lauf b_neu_west4_leon ist bitgleich mit neu_west4_leon.
Alle 11 Laeufe exit 0 bis RE15_EXIT_AT, 0 Zeilen error/assert/abort, "Kanister ausserhalb Regions-Viereck" 0 Zeilen, Kanister-Sitz-Abweichung max 0
(`messung_abnahme4.txt`). Die Zahlen des Bauers in `exe_mass.txt` sind damit reproduziert, nicht nur gelesen. tragvergleich (Bauer-Werkzeug, eigener Bau) je
Paar exit 0: frei4_leon/frei4_elza 0 abweichend; west4_leon F868 (5/5, Rammen F880); west5_elza F1934 (2/2, Rammen F1103); west4_elza F1444 (2/2, Rammen
F1183, erster Rueckfall F3036).

Mass `mass_v4.py` (eigene Laeufe; NB5 = gelobter Stand bc5fb7c9c, alte Laeufe mit demselben Mass; "ohne" = Messhaken):

| Lauf | ohne Fortschritt zur Figur max | Drehen ganz fern | rueckw. / Tauch | Kopf < 2500 vor / hinter Kamera | Rammen / Querungen | Kanister-Sitz max | ausserhalb Viereck |
|---|---|---|---|---|---|---|---|
| west4_leon NB5 | 8,9 s | 0,0 % | 0 / 0 | 27,6 % / 0,0 % | 0 / 0 | 0 (2670 B.) | 0 |
| west4_leon ohne | 3,0 s | 0,0 % | 0 / 0 | 1,5 % / 0,1 % | 0 / 1 | - | 0 |
| west4_leon neu | 11,1 s | 0,0 % | 0 / 0 | 3,7 % / 0,1 % | 2 / 4 | 0 (2670 B.) | 0 |
| west5_elza NB5 | 10,9 s | 0,0 % | 0 / 0 | 25,3 % / 0,0 % | 0 / 0 | 0 (2610 B.) | 0 |
| west5_elza ohne | 9,6 s | 0,0 % | 0 / 0 | 1,1 % / 0,0 % | 0 / 2 | - | 0 |
| west5_elza neu | 9,6 s | 0,0 % | 0 / 0 | 1,1 % / 0,0 % | 2 / 2 | 0 (2610 B.) | 0 |
| west4_elza NB5 | **70,2 s** | **75,7 %** | 0 / 0 | 8,7 % / 2,2 % | 0 / 0 | 0 (2700 B.) | 0 |
| west4_elza ohne | 7,3 s | 17,4 % | 0 / 0 | 10,4 % / 4,3 % | 0 / 1 | - | 0 |
| west4_elza neu | 7,3 s | 29,8 % | 2,1 % (2) / 0 | 24,3 % / 7,3 % | 1 / 1 | 0 (2700 B.) | 0 |
| frei4_leon NB5 | 21,5 s | 4,9 % | 0 / 0 | 14,0 % / 0,1 % | 0 / 0 | 0 (7830 B.) | 0 |
| frei4_leon ohne | 4,1 s | 1,6 % | 0 / 0 | 6,9 % / 0,0 % | 0 / 0 | - | 0 |
| frei4_leon neu | 4,1 s | 1,6 % | 0 / 0 | 6,9 % / 0,0 % | 0 / 0 | 0 (7830 B.) | 0 |
| frei4_elza NB5 | 6,6 s | 1,7 % | 0 / 0 | 0,0 % / 0,0 % | 0 / 0 | 0 (7470 B.) | 0 |
| frei4_elza ohne | 7,4 s | 0,0 % | 0 / 0 | 12,2 % / 0,5 % | 0 / 1 | - | 0 |
| frei4_elza neu | 7,4 s | 0,0 % | 0 / 0 | 12,2 % / 0,5 % | 0 / 1 | 0 (7470 B.) | 0 |

Die Werte west4_leon/west5_elza/west4_elza neu und ohne sind bis auf west4_elza 29,8 % (Bauer 29,8 %) und alle anderen Spalten identisch mit `exe_mass.txt`
des Bauers (reproduziert). Erste Abweichung mit gegen ohne: west4_leon F868 (Phase 5/5, Rammen F880), west5_elza F1934 (2/2, Rammen F1103 und F1864),
west4_elza F1444 (2/2, Rammen F1183) - alle nach Belagerung/Querung bzw. nach dem ersten Rammen, wie es `tragvergleich` verlangt.

west4_elza nach dem Rammen (eigenes Fenster-Mass `fenster.py`, F1183-F3300 = 70,6 s): neu: ohne Fortschritt max 5,8 s, Drehen auf der Stelle ganz fern
15,7 %, 2-s-Fenster still+fern 43, Kopf <= 4500 an der Figur 64,2 % der Bilder, Kopf-Abstand min 211 / mittel 3349 / max 7799; ohne (kein Rammen, Elza
laeuft weiter): max 1,1 s, 0 %, 0 Fenster, 90,2 % an der Figur. Vor dem Rammen (F587-F1183, mit = ohne = NB5-Verfolgung): Drehen ganz fern 82,0 %,
ohne Fortschritt max 7,3 s, Kopf-Abstand mittel 9210 - die 29,8 % des ganzen Laufs stammen zu 468 von 800 Bildern aus diesem NB5-identischen Abschnitt.

## 3. Bild (Framedumps F-genau, NB5-Video per Anker) - Bildleisten in `G2_belege/abn4/`

Spalten jeder Leiste: NB5 gelobt (bc5fb7c9c, Video der Abnahme 4 vom 2026-11-02, t = Anker + F/30, +-1 s; rot = ausserhalb des 125-s-Videos) | neu ohne Kanister
(Messhaken, Framedump) | neu mit Kanister (Framedump). Zeilen = Spielbild F (Framedumps exakt).
- `west4_leon_rampe_querung_rammen_F860-F1000.jpg`: NB5 bleibt im Suedbecken mit dem Kanister vor der Kamera (Stadion, Kiefer bildfuellend), Leon oben
  unbehelligt; neu ohne: Querung ueber die Rampe ohne Treffer, Leon klettert danach weiter (cam 3 ab F1012); neu mit: F875 Belagerung von Sueden, F880 Koerper
  auf der Rampe (Rammen, Leon hp 100 -> 50, Niederwurf, nach x 2318 gestossen), F885-F960 der Koerper schiebt sich mit dem Kanister am Kopf nach Norden,
  F980-F1000 Leon steht wieder, der Gator liegt noerdlich.
- `west4_leon_hochbiss_F1120-F1180.jpg`: neu mit: Hochbiss von Norden an der Rampenkante (Kopf steigt F1130-F1160), der Kanister haengt in jedem Bild
  zwischen den Kiefern; NB5: Belagerung im Suedbecken (Koerper vor der Kamera); ohne: Leon ist weiter geklettert (cam 3), der Gator wartet im Wasser.
- `west5_elza_rampe_rammen_F1050-F1960.jpg`: NB5: Stadion vor der Kamera (t 83-113 s, Kiefer bildfuellend, Elza unbehelligt); ohne: Hochbiss von Sueden ohne
  Treffer; mit: F1103 Rammen wirft Elza von der Rampe ins Wasser (F1110 weg vom Deck, F1140-F1200 liegt sie rot links unten), der Gator liegt mit dem
  Kanister auf der Rampe; F1864 zweites Rammen im Nordbecken (cam 4), Elza zur Nordwand geworfen.
- `west4_elza_nach_rammen_F1180-F2700.jpg` (Offen 2): NB5: Elza 35 s allein im Bild (cam 3), kein Gator (der M4-Haenger im Westkanal, 70,2 s ohne Fortschritt);
  ohne: Elza an der Rampen-Nordkante, der Gator kreist und beisst (17 Treffer nach F1183); mit: F1200-F1900 Elza im Wasser (cam 4) mit dem Gator daneben,
  16 Treffer (hp 100 -> 50 je ~3,7 s: F1376, 1512, 1619, 1726, 1839, 1955, 2062, 2177, 2284, 2401, 2509, 2616, 2743, 2850, 3089), 6 Niederwuerfe;
  F2000-F2700 (cam 3) der Koerper des Gators fuellt beim Anlauf das Bild (Kopf < 2500 vor der Kamera 24,3 %, hinter 7,3 % - Schuppenwand), Elza klein dahinter.
  Rueckfall F3035/F3274 (je 150 Bilder Wurzel in einem Kasten, Kopf 6636/6811 von der Figur, Rueckwaerts F3036-F3065 und F3275): Pausen von 5 s, kein Haenger.
- `frei4_leon_plattform_suedkante_F5700-F6300.jpg` (alter M4-Haenger): NB5: der Gator liegt mit dem Kanister (grau/gelber Zylinder, in Cut 1 gross und
  deutlich im Maul) 20 s an der Plattform-Suedkante, Leon steht oben (t 226-242 s), dann Cut 1 an der Leiter; Fenster-Mass F5700-F6300 NB5: ohne Fortschritt
  max 14,4 s, 321 2-s-Fenster still+fern, Kopf nur 3,2 % der Bilder an der Figur. Neu (ohne = mit, 0 abweichende Bilder in 7805): max 1,4 s, 0 Fenster, Kopf
  84,2 % der Bilder <= 4500 an der Figur (Abstand min 192 = Biss); Figur und Gator sind im Nordwesten (-7320,-7099)..(2789,-10613), in Cut 0 nicht im Bild -
  darum zeigen die neuen Spalten den leeren Blick auf Tor/Leiter (die Zahlen stehen oben, das Bild zeigt es nicht; so auch der Bauer).
- `frei4_elza_dash_F3300-F4000.jpg` (Offen 3): ohne und mit bildgleich (nur der Kanister im Maul unterscheidet sie, z. B. F3300/F3370/F3720 dunkler Zylinder
  am Kopf): F3300-F3370 Biss an der Leiter (Cut 1), F3440-F3580 der Gator liegt flach neben Elza und dreht vor (RE2-Zonen-Dash Phase 12, Clip 2 F3370-F3469,
  Clip 3 bis F3619), F3650-F4000 (Cut 3) zweiter Dash (Phase 12 ab F3700, Clip 2/3 bis F3851+). Fenster-Mass F3377-F3977 neu = ohne: ohne Fortschritt max
  5,8 s, Drehen 0 %, 32 2-s-Fenster still (Vordrehen), Kopf-Abstand min 4414; NB5 (im selben Fenster an anderer Stelle, Cut 0 leer): max 4,9 s. Kein Haengen,
  RE2-Dash wie im gelobten Gang.
- `kanister_sitz_30fps_west4_leon_hochbiss_F1115-F1175.jpg` (Burst-Lauf, jedes Spielbild, Ausschnitt Kopf/Rampe): F1115-F1127 der Kopf kommt von rechts an
  die Rampenkante, F1128-F1145 er steigt mit offenen Kiefern hoch, F1146-F1156 er dreht/kommt herunter, F1157-F1175 er liegt mit dem Maul auf dem Deck -
  der Kanister (dunkler Zylinder, gelbes Band) liegt in jedem Bild zwischen den Kiefern und folgt der Kopfdrehung Bild fuer Bild; kein Bild mit Versatz,
  Nachziehen oder Springen (deckt sich mit "Kanister-Sitz" Abweichung max 0 in 2670 Bildern dieses Laufs).

Bereits gesehen (neu_west4_leon, Framedumps 640 x 480, Ausschnitt Kopf/Rampe):
- Hochbiss F1125-F1170 (Cut 2, Figur auf der Rampe): der Kopf steigt an der Rampenkante hoch, die Kiefer oeffnen sich; der Kanister (dunkler Zylinder mit
  gelben Baendern - in Cut 2 dunkel beleuchtet, in Cut 0/1 der NB5-Videos grau/gelb) haengt in jedem Bild quer zwischen den Kiefern, auch beim
  Herunterkommen (F1155-F1170) - kein Versatz, kein Springen.
- Querung mit Rammen F875-F920: F875 Belagerung von Sueden (Kopf vor der Kamera), F880 der Koerper liegt auf der Rampe (Anlaufpunkt-Sprung, Offen 5), Leon
  getroffen (hp 100 -> 50, mo=12 Niederwurf, von z -17263 nach -14968 und x 4428 -> 2318 gestossen), F885-F920 der Koerper schiebt sich nach Norden ueber
  das Deck, der Kanister bleibt am Kopf. Danach (F1121-F1163) Hochbiss von Norden, zweites Rammen F1731.
- west5_elza F1100-F1200: Rammen F1103 wirft Elza von der Rampe (y -1800) ins Wasser (1382,-18617; mo=15), der Gator liegt F1120-F1200 mit dem Kanister
  auf der Rampe; zweites Rammen F1864 im Nordbecken (Elza im Wasser, zur Nordwand geworfen).

## 4. Die acht offenen Punkte des Bauers - Einstufung (eigene Messung)

1. **Kopf < 2500 vor / hinter der Kamera (Harness neu 11,6 % / 2,3 %, ohne 12,5 % / 2,5 %): Nutzerfrage, nicht blockierend.** Die NB1-Schranke (<= 8 % /
   <= 0,1 %) stammt aus der G2-RE2-Klemme fuer Kopf + Kanister; ohne Kanister (NB5, gelobt) gab es sie nie. Mit Vorschlag 4 ist die Bewegung mit Kanister
   die ohne Kanister - eine absolute Schranke mit Kanister waere ohne Umbau des gelobten Gangs nicht zu halten (Umbau ohne Ja verboten). Die Umstellung
   des Riegels auf "mit Kanister nicht haeufiger als ohne" ist darum sachlich richtig, kein Weichmachen ohne Beleg: der Beleg ist die Messung des
   Stands ohne Kanister (12,5 % / 2,5 %) im selben Riegel, und die absoluten Werte stehen weiter in der Ausgabe. Im exe ist der gelobte Stand sogar
   deutlich naeher an der Kamera (NB5 west4_leon 27,6 %, west5_elza 25,3 %, neu 3,7 % / 1,1 %); in west4_elza neu nach dem Rammen 24,3 % / 7,3 %
   (NB5 8,7 % / 2,2 %, ohne 10,4 % / 4,3 %) - der Kopf steht dort haeufiger dicht vor und hinter der Kamera (Schuppenwand im Bild), weil er Elza an der
   Rampen-Nordkante angreift. Ob eine Kamera-Grenze fuer den Kopf auch ohne Kanister gelten soll, kann nur der Nutzer entscheiden (Abschnitt 5, F1).
2. **Nach dem Rammen in west4_elza (Drehen fern 29,8 %, 103 2-s-Fenster, 2 Rueckwaerts-Durchgaenge): nicht blockierend.** Gemessen im Fenster F1183-F3300:
   laengste Zeit ohne Fortschritt 5,8 s, Kopf in 64 % der Bilder <= 4500 an der Figur (Bisse, Phase 3, Abstand 211), Drehen ganz fern 15,7 %; der Rest der
   29,8 % liegt VOR dem Rammen im NB5-identischen Abschnitt (82 %, Elza laeuft an der Ostwand ins Nordbecken, der Gator wendet im Westkanal - so auch im
   gelobten Stand, dort anschliessend 70,2 s ohne Fortschritt). Kein Haenger nach der Vorgabe (keine laengere Zeit ohne Fortschritt, kein Drehen fern der
   Figur ueber laengere Zeit; die beiden Rueckwaerts-Durchgaenge F2129/F2452 je ~1 s sind der Rueckfall nach 5 s im Kasten). Bild: Abschnitt 3.
3. **RE2-Zonen-Dash 20 s Vordrehen (frei4_elza F3377-F3977), gleich ohne Kanister: nicht blockierend** - mit = ohne bildgleich (0 abweichende Bilder im ganzen
   Lauf), zwei Dash-Zyklen (Phase 12, Clip 2/3) je ~8 s neben der Figur, laengste Zeit ohne Fortschritt 5,8 s, 0 % Drehen; das Vordrehen ist der RE2-Dash
   des gelobten Gangs, kein Haengen (Abschnitt 3 Leiste frei4_elza_dash).
4. **Hysterese-Ruhe beim Tragen (RE2 @0x80100ab8-c8) entfaellt: nicht blockierend, dokumentierte PORT-WAHL** nach dem Nutzerentscheid ("Tu so als ob er
   nicht da waere"); der Pruefhaken `re15_gr2_v4_alt_setzen(1)` behaelt RE2, der Riegel re2gator aufnahme prueft beides.
5. **Querungsbeginn springt (Anlaufpunkt, bis 1413 je Bild): nicht blockierend, Rest aus NB5.** Im Bild (F875 -> F880 west4_leon) springt die Wurzel 1536
   Einheiten auf die Rampe; dasselbe Verhalten hat die Querung ohne Kanister (Spur B, NB5) - kein Rueckschritt, aber sichtbar (Nutzerfrage F2 optional).
6. **PSX-Bau nicht gemessen: nicht blockierend** (geaenderte Pfade plattformneutral, Zeichner nur PC; PSn00bSDK hier nicht eingerichtet, bekannte Luecke).
7. **Haengen an der Plattform-Suedkante auch ohne Kanister (exe rampe/rampe_elza/abn2_wand_elza, 150 Bilder im Kasten, Kopf 6830-9820 von der Figur): nicht
   blockierend, aber Nutzerfrage.** Es ist der Ort des alten M4-Haengers, die Ursache liegt in der NB5-Verfolgung (gelobt); mit Kanister greift dort jetzt der
   Rueckfall (Rueckwaerts, kein Tauchen). Ein Umbau der Verfolgung ohne Kanister braucht das Ja des Nutzers (Abschnitt 5, F4).
8. **exe rampe: die Figur kommt nicht auf die Rampe, der fruehere Riegel "Ueberquerung >= 100 Bilder, Rammen >= 1" nur noch Ausgabe: nicht blockierend**
   (Testdesign; das Rammen auf der Rampe belegen Riegel re2 und meine Laeufe west4_leon F880/F1731, west5_elza F1103/F1864).

## 5. Konstanten im Diff f942f56c0..c432eb1d2 (gator_re2_r41.c, gator_rw_r41.c, enemy_ai_boss_gator.c, gator_re2_pc.c)

Keine neue Zahl ohne Beleg. Die neue Seitenwahl-Regel der Querung (enemy_ai_boss_gator.c GBP_GUARD) setzt nur vorhandene Groessen zusammen:
| Zahl | Beleg | Herkunft |
|---|---|---|
| 5644 (Kopf vor der Wurzel) | NB6 Messung 1 (gator_re2_r41.c:2392) | Messung |
| 1793 (oberster Scheitel ueber der Wurzel) | GR2_RW_KOERPER_OBEN, gemessen r41/gatorfix NB7 (gator_re2_r41.c:2734) | Messung |
| 2500 (GR2_RW_KAM_NAH, vor der Kamera) | PORT-WAHL Auftrag Punkt 4c, r41/gatorfix a14ff425a (gator_re2_r41.c:2732), vorhanden seit 199a63c95 | Orchestrator-Auftrag |
| 3350 / GB_RAMP_X1 - 700 (Lande-x-Klemme) | gb_cross_begin (vorhanden) | NB5 |
| Pause 300 nach der Querung (statt GB_KREUZ_PAUSE 45) | Spur B "Leon OBEN: 10 s auf der Seite bleiben (souveraene Belagerung statt Dauer-Pendelei, Nutzer ...)" enemy_ai_boss_gator.c:2624, der Wert OHNE Kanister seit NB5; 45 nur noch mit Pruefhaken; Messung des Bauers (45: Kopf < 2500 vor der Kamera 8,2 % gegen 1,5 %) | NB5-Wert = "wie ohne Kanister" (Nutzerentscheid), s. Nutzerfrage F3 |
| Haenge-Fenster 150 Bilder / Kasten 2500 / Kopf 4500 (RW_HAENG_BILDER, RW_HAENG_KASTEN, RW_NAH) | gator_rw_r41.c:49-52, seit 199a63c95, im Modulkopf als PORT-WAHL erklaert ("Ausloeser, Dauer, Drehrate, Tauch-Schwelle ... PORT-WAHL; das Mittel (Clip 10, Blend 3) ist RE2" @0x80102c40-48) | PORT-WAHL (vorhanden), Rueckfall vom Nutzer erlaubt ("Mal kurz tauchen waere ok, wenn er mal haengt") |
| 16-Bilder-Fenster "drueckt sich fest" (RW_DRUECK_N) | jetzt ohne Pruefhaken abgeschaltet (gator_rw_r41.c), Messung Harness Mitte-Nord 8 % rueckwaerts im Angriff | PORT-WAHL (Abschaltung) |
| 6 Bilder Heranfuehren der Lokallage (Stufe 7) | RE2 @0x80101b58-8c | RE2 |
| Kanister-Halterung | RE2 `addiu a0,s0,1104` @0x801018d0 / `sw a0,128(s2)` @0x801018e0, Trefferziel T2 @0x801017c4, HURT @0x80101ef4-fc | RE2 |
Nutzerfragen (fuer die Release Notes "Zu entscheiden", Sie-Form):
- F1: "Soll der Kopf des Alligators auch OHNE Kanister nie naeher als 2500 vor die Kamera der Figur kommen bzw. nie hinter sie geraten (heute im gelobten
  Stand 12,5 % / 2,5 % der Bilder im Harness, im Fenster west4_leon 27,6 %)? Das waere ein Umbau des gelobten Gangs."
- F2 (optional): "Darf der Querungsbeginn ueber die Rampe weiter mit dem Sprung an den Anlaufpunkt (bis ~1500 Einheiten in einem Bild) laufen, wie im
  gelobten Stand ohne Kanister, oder soll er anlaufen?"
- F3: "Mit dem Kanister im Maul quert er die Rampe jetzt wie ohne Kanister alle 10 s (NB5-Pause 300) mit einem Rammstoss je Querung, statt alle ~1,5-6 s wie
  im Stand vom 2026-11-03 (west5_elza: 2 statt 7 Rammstoesse in 1,4 min). Ist dieser Takt gemeint mit 'rammt wie gehabt'?"
- F4: "Ohne Kanister haengt er an der Plattform-Suedkante bis 5 s im gelobten Stand (Riegel rampe/rampe_elza/abn2_wand_elza); mit Kanister setzt dort jetzt
  der Rueckfall (Rueckwaerts) ein. Soll die Verfolgung ohne Kanister dort nachgebessert werden (Umbau des gelobten Stands) oder bleibt es?"

## 6. Tests (Summenzeilen woertlich)

- Harness: `ctest --test-dir re15_port/build -R "^unit_r41_gatorfix_" -j 3 --timeout 900` (20:0x Uhr, parallel zu einem eigenen exe-Lauf):
  **`100% tests passed, 0 tests failed out of 24`**, `Total Test time (real) =  40.69 sec` (`G2_belege/abn4/ctest_unit.txt`: v4 14,29 s, v4_elza 14,40 s,
  re2 23,34 s, re2_elza 23,55 s, fuzz 15,0 s, maul 3,8 s, gang 1,3 s, alle anderen < 1,2 s).
- Direkt: `test_r41_gatorfix.exe v4 / v4_elza / re2 / re2_elza / gang / gang_elza`: je `0 Fehler`; Kennzahlen = Bauer-Belege (v4: 9 Wasser-Lagen 0 abweichend,
  Rampe-x2318 ab Bild 769 / Rampe-Nord ab 766, Gegenprobe Huelle 11 697, Halterung 11 700 Bilder 0/0, Gegenprobe alte Pose 300/300 max 12 281; re2: ohne
  Fortschritt max neu 10,7 s / f942 55,9 s, Drehen fern je Lage 0 % (f942 45 % Nordbecken-Ost), Kopf < 2500 vor/hinter neu 2726/537 gegen ohne 2920/595,
  Rueckfall 0 %, Tauchen 0, Rampe 4 Lagen Treffer, Rammen 5, x2318 19 > 3, ohne Kanister 0 Rammen; gang 0/2700).
- exe-Riegel `ctest --test-dir re15_port/build -R "^integration_r41_gatorfix_(rampe|rampe_elza|abn2_wand_elza|frei|frei_elza)$|^integration_r41_re2gator" -j 2
  --timeout 1800 -V` (20:25-20:52, parallel zu meinen Fensterlaeufen und zu fremden Agenten; kein Lauf musste wiederholt werden):
  **`100% tests passed, 0 tests failed out of 6`**, `Total Test time (real) = 1599.82 sec` - abn2_wand_elza 680,99 s, frei_elza 686,82 s, rampe_elza 343,86 s,
  frei 649,93 s, rampe 306,56 s, re2gator 268,82 s (`G2_belege/abn4/ctest_integration_auszug.txt`). Riegel-Meldungen (eigener Lauf, wortgleich zur Lesart des
  Bauers): rampe und rampe_elza `tragvergleich: 2895 Bilder mit Kanister ... abweichend 951, erste Abweichung F2650 (Phase mit 2 / ohne 2, erstes Rammen F-1,
  erster Rueckfall F2650); Kanister-Sitz 2910 Bilder, Abweichung max 0 ... Klettern sub=9 0 Zeilen, Ueberquerung mit Kanister 0 Bilder, Rammen 0` (Leon = Elza
  bitgleich); abn2_wand_elza `abweichend 3472, erste Abweichung F4929 (... erster Rueckfall F4929); Kanister-Sitz 8220 Bilder, Abweichung max 0`; frei `7750
  Bilder mit Kanister ... abweichend 0 ... Kanister-Sitz 7770 Bilder, Abweichung max 0`; frei_elza `abweichend 3933, erste Abweichung F4468 (... erstes Rammen
  F4244 ...); Kanister-Sitz 7860 Bilder, Abweichung max 0`; re2gator `ok — nische: Maul-Sitz 240 Bilder ... Finisher Cut 6`.

## 7. Nicht geprueft

PSX-Bau; Suite ausserhalb der genannten Gruppen (der Bauer: Gruppe 127 Tests 98 %, danach 27/27 gruen - nicht nachgefahren, nur die geforderten Riegel);
Hoeren; Raumwechsel mitten im Tragen; Wiedereintritt nach dem R1-Tod (Abnahme 3 bestanden, hier unveraendert).
