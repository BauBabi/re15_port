# Runde 41 - Spur G2 "gator_re2" - UNABHAENGIGE ABNAHME 7 (2026-11-07): G2-RE2 NB4 Runde 1 (Trefferreaktion wie RE2 (T), Stoss von der Rampe ins Wasser (W), kein Kanister-Holmodus (K))

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Kopf b4dfc23a9 (Bau 6f4314fb = cfa775ae8, danach nur Doku): 00b5d6986 Fallschritte als Helfer (W),
67b3b2356 kein Holmodus (K), 3b381e825 Trefferreaktion wie RE2 + Stoss ins Wasser (T)+(W), b89edc54e Riegel nb4(_elza)/gtrlog, d0f2bcc3e Dossier, 88ff3f990 Hochbiss
haelt die RE2-Bisssperre, cfa775ae8 / b4dfc23a9 Doku. Messblatt 365a6ef7b (`G2_MESSBLATT_NB4.md`), Vorgaenger-Abnahme 6 (3d57083b1, BESTANDEN).
Pruefer: Abnahme-Agent, glaubt dem Bau nichts, misst selbst. Kein Spiel- und kein Testcode geaendert; dieser Commit bringt nur diesen Bericht und `G2_belege/abn7/`.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn7`
(`runs/<lauf>/` mit state.log, climb.log, gator_boss.log, gator_trag.log, debug.log, env.txt, pid.txt, exe_md5.txt, fd_NNNNNN.ppm; `harness/`, `leisten/`).
Belege im Baum: `G2_belege/abn7/` (`messung_abnahme7.txt` = alle Auswertungen, `disasm/` = eigene RE2-Disassembly, `harness/`, `werkzeug/`, `laeufe/`, Bildleisten,
`ctest_unit.txt`, `ctest_exe.txt`, `build_check.txt`).

Vorgaben (Wortlaut aus dem Auftrag): **(T)** fuer JEDEN Alligator-Angriff, der die Figur trifft, die Trefferreaktion der Figur aus RE2 (Weg, Richtung, Dauer,
Animation, Fallen/Taumeln, Schaden, Unverwundbarkeit) statt des RE1.5-Niederwurfs. **(W)** Nutzer 2026-11-07 ~15:00 woertlich: "er kann den Spieler ruhig In das
Wasser stoßen." **(K)** Nutzer 2026-11-07 ~15:00 woertlich: "Die Frage stellt sich garnicht, da leon mit den alligator ganz normal seine runden drehen soll. Das heißt
der alligator sollte Ganz natürlich irgendwann zum Kanister kommen, während er leo. Auf dem rundweg verfolgt...." Bleibt: Gang ohne Kanister bitgleich NB5,
Vorgabe A (oben nicht sicher) und B (kein Haenger) aus NB3, Raumhalt Band 0, Kanister-Sitz 0, Rueckfall nur aus dem Haenge-Fenster.

## 0. Urteil

**NICHT BESTANDEN - ein blockierender Befund (M1), sonst erfuellt.**

- **M1 (blockierend, Vorgabe T "Unverwundbarkeit"): Zweitbiss im selben Biss-Clip.** Der Biss im Wasser (LUNGE, Clip 4, Bissfenster Bild 6..34) trifft die Figur
  ein zweites Mal, sobald die RE2-Sperre +0x1D3 faellt: 20 Bilder nach dem ersten Treffer (Sub 2, Ende der Reaktion) bzw. 22 Bilder (Sub 3, Bild 21 = Liegen,
  +0x1D3 = 0 @0x801032f0) - im SELBEN Clip-4-Lauf des Gators (state.log Gator mo=4, af 7 -> 27/29, gator_trag.log Phase 3). In meinen 15 Laeufen (Leon und
  Elza, mit und ohne Kanister, Rundweg / Rampe / Kanten-Lagen) sind **400 von 895 Treffern solche Zweitbisse** (45 %); die Bauer-Laeufe c4_* zeigen dasselbe (bytegleich). In RE2 ist das nicht moeglich (eigene Disassembly
  EM23_OVL_0000.BIN, Abschnitt 3a): der Treffer-Einstieg 0x80101578 wird nur in den Schliess-Stufen 4/5 der Biss-Maschine +0x224 geprueft (Tafel @0x80100004,
  `jal 0x80101578` @0x8010155c, 3 + 3 Bilder), die Biss-Maschine wird nur in den Angriffs-Unterzustaenden scharf (T1 0x801012fc, Aufrufe @0x80100c60 / @0x80100d40),
  und JEDER neue Angriff braucht +0x238 == 0 (gesetzt 70 im Treffer @0x801016b0-b4, je Bild -1 @0x801004a8-b8, Pruefstellen `lbu v0,0x238 / beqz / sw 0x101,4(s0)`
  @0x80100bf4-0x80100c04 und @0x80100cb4-c4, `bnez -> kein Angriffswechsel` @0x80100b5c). Im Port fragt die LUNGE-Bissbedingung (enemy_ai_boss_gator.c :2452-2458)
  weder `hit_stun` noch `re15_gr2_biss_cd_rest` noch einen Latch (`bite_done = 1` setzt nur das Rammen :3006); der Bauer hat nur den Hochbiss gesperrt (88ff3f990) und
  schreibt "LUNGE und CROSS-Biss sind durch hit_stun 0x64 ... ohnehin laenger gesperrt" - fuer die LUNGE widerlegt (hit_stun 80 im Zweitbiss-Bild, state.log
  `stun=80` F775 -> `stun=99` F776). Der RE1.5-Niederwurf (41-92 Bilder) hatte das verdeckt. **Folge mit echten Trefferpunkten** (eigener Lauf `a7_tod_leon`, erster
  Besuch, Gator mit voller HP = 60 je Biss, Leon im Suedbecken): F942 Biss hp 100 -> 40, F962 Zweitbiss hp 40 -> -20 = TOT (FUN_800401d4: -20 < -14 -> 2), Fressen -
  **ein einziger Schnapper toetet die Figur aus voller Gesundheit in 20 Bildern**; nach der Szene (Gator geschwaecht, 30 je Biss) 100 -> 40 nach einem Schnapper,
  der zweite toetet. Die Riegel des Bauers zaehlen diese Zweitbisse als "abgebrochen" (Harness Teil B 171 von 888, gtrlog) und lassen sie durch. Kleinster belegter
  Fix (nicht Teil dieser Abnahme): die LUNGE-Bissbedingung (und B2) fragt die RE2-Bisssperre wie der Hochbiss (`re15_gr2_biss_cd_rest(slot) == 0`, @0x80100cb4-c4),
  und gtrlog/nb4 werten einen Treffer waehrend +0x238 > 0 als Fehler.
- **(T) sonst erfuellt:** jede Reaktion = RE2-Modell (Unterzustand FUN_80015910, Schritte -800/+150/+15 bzw. +800/-120, Dauer 20/54, Clips 18/19 bzw. 20/17/PL00 11
  rueckwaerts, Sperre, SE, Schaden 60/30), 0 Bilder RE1.5-Niederwurf in ueber 70 000 Bildern eigener Laeufe, Haken-Weg je Reaktion nie ueber 3735/3080 (+468
  Fall-Klemme im Sturz), Luftlinie gesamt mit Koerper-/Boss-Schub bis 4954 (vorher bis 13 059). RE2-Adressen des Baus stichprobenweise selbst disassembliert - alle bestaetigt (Abschnitt 6).
- **(W) erfuellt:** Rammen an der Nordkante stoesst Leon und Elza von der Rampe (F383 -> KANTE F385 -> LANDUNG F389, y -1800 -> -1210/-1070/-880/-640/0, Band 1 -> 0
  vor der Klemme, 0 Raumhalt-Eingriffe im Sturz), Spritzer im Wasser, Figur danach im Nordbecken; Ostwand/Plattform bleiben Wand (Harness Teil A 13/13, eigene
  Kanten-Lagen Abschnitt 3c); 0 Bilder ausserhalb des Raums in allen Laeufen; Abstieg per Aktionstaste bytegleich zum Stand 3d57083b1 (eigener A/B-Lauf).
- **(K) erfuellt:** 0 Zeilen RUNDE / ANSCHWIMMEN / Zielmodus 1 in allen Laeufen mit liegendem Kanister; der Gator verfolgt bis zum ersten Treffer bitgleich wie im Stand
  3d57083b1 (west4/west5/frei4/oben: erste Abweichung exakt im Trefferbild); Aufnahme nur im Vorbeikommen (laengs 1708..5583, |quer| <= 1707): Rundweg frei4 Leon F5689
  / Elza F178, oben_leon F1857 (nach dem Sturz im Wasser), in oben_elza / west4_leon / west4_elza / west5_elza (Figur an der Rampe) bleibt der Kanister 3300-4400
  Bilder liegen - wie (K) es verlangt. Keine 28-s-Pause oben mehr (erster Treffer oben nach 300 Bildern).
- **Bleibt:** Gang ohne Kanister bitgleich (unit gang/gang_elza gruen; eigene ohne-Laeufe bis zum ersten Treffer bitgleich zur Abnahme 6); Vorgabe B Haenge-Fenster 0,
  ohne Fortschritt max 293 (<= 450), 0 Rueckfall-Zeilen in 7 Kanister-Laeufen; Kanister-Sitz 0; Riegel unit 48/48 gruen, exe-Riegel 20: siehe Abschnitt 2.

| Punkt | Soll | Ist (eigene Messung) | Bewertung |
|---|---|---|---|
| (T) Trefferreaktion je Angriff = RE2 | Sub 2/3 nach FUN_80015910, Schritte/Dauer/Clips/Sperre/SE/Schaden wie RE2, 0 RE1.5-Niederwurf | 11 eigene exe-Laeufe + 2 Harness-Riegel: alle vollstaendigen Reaktionen Sub 2 = 20 Bilder (18@0, 19@4), Sub 3 = 54 (20@0, 17@21, 11@29 rw), Schaden 30 (Messhaken) / 60 (tod_leon), 0 Niederwurf-Clips; gtrlog "Abweichungen 0" in jedem Lauf; Rammen/CROSS/Hochbiss/Biss je mindestens einmal (Harness) | erfuellt |
| (T) Unverwundbarkeit / Angriffstakt wie RE2 | nach einem Treffer kein neuer Biss vor +0x238 = 70 Bildern (@0x801016b0-b4, @0x80100cb4-c4) | Zweitbiss im selben Clip-4-Lauf 20/22 Bilder nach dem Treffer: 400 von 895 Treffern (15 Laeufe, Leon und Elza); tod_leon: 100 -> 40 -> -20 TOT in 20 Bildern | **M1 - NICHT erfuellt (blockierend)** |
| (T) Weg | Haken <= 3735 / 3080 (+468 Fall-Klemme), Luftlinie gesamt nur ausgegeben | Haken max 4153 (nur mit Sturz, Schranke 4203), ohne Sturz 3735; Luftlinie gesamt max 4954 (Kanten-Lage Sued Leon, Boss-Schub) - vorher bis 13 059 | erfuellt |
| (W) Kante -> Wasser | Band 1 -> 0 vor der Klemme, y -1210/-1070/-880/-640/0, Landung + Spritzer, 0 Raumhalt im Sturz | oben_leon / oben_elza / ohne_oben_elza: F383 Rammen -> F385 KANTE -> F389 LANDUNG, y-Folge exakt, Spritzer im Bild (F392/F394); Harness Teil A 13/13, Teil B 64 Stuerze richtig; eigene Kanten-Lagen Leon/Elza (Abschnitt 3c) | erfuellt |
| (W) Wand bleibt Wand / nie aus dem Raum | Ostwand [6], Plattform [7] Klemme; aussenlog 0 | Harness Teil A Deck Ost/West/Mitte-West: Figur bleibt y -1800 Band 1; aussenlog in allen eigenen Laeufen "aussen 0, Deck verlassen 0, im Block 0" | erfuellt |
| (W) Abstieg per Aktionstaste unveraendert | climb.log bytegleich | eigener A/B (Elza Nordkante, RE15_KEIN_GATOR): exe 6f4314fb gegen 5910fc24 (3d57083b1) climb.log bytegleich, 39 sub != 0 Bilder | erfuellt |
| (K) kein Holmodus, Verfolgung wie ohne Kanister | 0 RUNDE/ANSCHWIMMEN/Zielmodus 1; bitgleich bis zur Aufnahme | 0/0/0 in allen Laeufen; west4_leon/west4_elza/west5_elza/ohne_oben_elza/ohne_frei4_leon gegen Abnahme-6-Laeufe (3d57083b1) bitgleich bis zum ersten Trefferbild (F123/F137/F92/F383/F139); Harness K1 6 Lagen 0 abweichend | erfuellt |
| (K) Aufnahme nur im Vorbeikommen | laengs in [0,6000), quer < 1733 | 9 Aufnahmen in 19 Laeufen mit liegendem Kanister (eigene: frei4_leon F5689: 1708/-1156; frei4_elza F178: 5583/1707; oben_leon F1857: 5444/1672; exe-Riegel-Ordner: F143 x3, F152, F164, F888), 0 ausserhalb; in 10 Laeufen (Figur an der Rampe, Rundweg ohne Vorbeikommen) bleibt er liegen | erfuellt |
| Vorgabe A (oben nicht sicher) | erster Treffer oben <= 600, Pause <= 600 | rampe_oben Leon/Elza: erster Treffer oben nach 300 Bildern (Rammen), danach Sturz ins Wasser; Harness Teil B t450/450; eigene Kanten-Lagen Abschnitt 3c | erfuellt |
| Vorgabe B (kein Haenger) | Haenge-Fenster 0, ohne Fortschritt <= 450, 0 Rueckfall | 7 Kanister-Laeufe: Fenster 0, ohne Fortschritt max 293 (frei4_elza), 0 HAENGT/RUECKWAERTS/TAUCHEN | erfuellt |
| Gang ohne Kanister bitgleich NB5 | unit gang/gang_elza, exe gang_* gruen | unit 48/48 gruen; ohne_frei4_leon / ohne_oben_elza bitgleich zur Abnahme 6 bis zum ersten Treffer | erfuellt |
| Riegel | unit + exe gruen | unit `100% tests passed, 0 tests failed out of 48`; exe: Abschnitt 2 | siehe 2 |
| Konstanten | @0x... oder PORT-WAHL + Wortlaut | RE2-Adressen selbst disassembliert und bestaetigt; RE1.5-Fallkonstanten in ghidra1_V2.txt nachgelesen; GATOR40A0.RBJ bytegleich RDT @0x65E8; A90 = PORT-WAHL ohne Nutzer-Wortlaut (Entscheid Orchestrator, vom Bauer so gemeldet) | belegt; ein formaler Rest (A90) |

## 1. Bau, Messweg

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build` (20:15): `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`build_check.txt`).
  exe `re15_port/build/platform/pc/re15_pc.exe` 2026-11-07 18:11, md5 6f4314fbf2904e210126a8315e05b4d1 (= Dossier-Bau); Arbeitsbaum sauber, Kopf b4dfc23a9.
- Messkopien (eigene, nur eigene PIDs, kein taskkill auf fremde Prozesse): `re15_pc_g2abn7.exe` (6f4314fb) und fuer den Abstiegs-A/B `re15_pc_g2abn7_alt.exe` = Kopie von
  `re15_pc_g2nb4.exe` (5910fc24, Stand 3d57083b1); Auswerter-Kopie `tests/unit/test_r41_gatorfix_g2abn7.exe`. Skripte `werkzeug/lauf7.sh`, `welle1.sh`..`welle3.sh`,
  `welle1b.sh`, `welle1c.sh`. Kein gdigrab; Bilder nur als RE15_FRAMEDUMP-Serie (F = Raum-Bildzahl wie state.log). Parallel liefen fremde Agenten (bis 7 exe) und
  meine exe-Riegel-Gruppe.
- Eigene exe-Laeufe (Umgebung wortgleich den Bauer-/Abnahme-6-Laeufen, nur Framedump-Fenster anders): oben_leon / oben_elza (Riegel-Szenario rampe_oben, Kanister nach
  der Szene im Becken, 4400 Bilder), frei4_leon / frei4_elza (Abnahme-4-Rundweg, 8400), west4_leon / west4_elza / west5_elza (Abnahme-4-Eingaben, 3300), ohne_frei4_leon
  / ohne_oben_elza (RE15_G2_OHNE_KANISTER=1), tod_leon (erster Besuch, Kampf sofort, Leon im Suedbecken (-3000,-23000), KEIN HP-Messhaken, KEIN GATOR_HP-Messhaken),
  w4_abstieg_neu / _alt (Bauer-Umgebung w4_abstieg, Elza Nordkante, RE15_KEIN_GATOR), sechs eigene Kanten-Lagen (Abschnitt 3c). Alle exit 0 bis RE15_EXIT_AT
  (`messung_abnahme7.txt` Abschnitt 1). Werkzeugfehler unterwegs: zwei Elza-Laeufe mit RE15_TITLE_SHOT booteten als Leon (ROOM1240/2090; TITLE_SHOT stoert den
  PSELECT-Wechsel) und wurden verworfen und ohne TITLE_SHOT wiederholt; die Tod-Probe ohne TITLE_SHOT blieb im Titel und wurde mit TITLE_SHOT wiederholt.
- Determinismus / Vergleich mit dem Bauer: oben_leon, oben_elza, frei4_leon, frei4_elza sind zu den Bauer-Laeufen c4_* (scratchpad `g2nb4/runs/`, exe 3d53ca60 =
  Bau vor cfa775ae8) in state.log, climb.log, gator_boss.log und gator_trag.log **bytegleich** (cmp). Die Tod-Probe lief zweimal (a7_tod_leon, a7_tod_leon_bild) mit
  gleichen Trefferbildern F942/F962.
- Auswerter: die exe-Auswerter der Riegel (gtrlog / aussenlog / obenlog / haenglog, eigene Kopie) UND eigene, von den GTR-Zeilen des Bauers unabhaengige Werkzeuge:
  `t7.py` (state.log + climb.log Zeile fuer Zeile: je hp-Flanke Unterzustand aus dem ersten Reaktions-Clip, Dauer aus den Spieler-Clips 18/19 bzw. 20/17/11,
  Luftlinie, Bildschritt, Sturz-y-Folge, RE1.5-Clips 12..16), `dbl7.py` (Doppelbisse: zwei hp-Flanken <= 25 Bilder mit durchlaufendem Gator-Clip), `kanister7.py`
  (Lebenslauf des Szenen-Kanisters), `r7.py` (Bildleisten), `dis_em23.py` (eigene capstone-Disassembly des RE2-Overlays, Basis 0x80100000).
- Harness: `test_r41_gatorfix_g2abn7.exe nb4` / `nb4_elza` direkt: `nb4: 0 Fehler` / `nb4_elza: 0 Fehler`, SUMME-Zeilen woertlich gleich dem Bauer-Beleg
  (`harness/nb4*.txt`: Teil A 13/13, Teil B 888 Reaktionen / 717 vollstaendig / 171 abgebrochen, 64 Stuerze, K1 0 von 6 abweichend).

## 2. Riegel (Summenzeilen woertlich)

- Harness: `ctest --test-dir "<Baum>/re15_port/build" -R "^unit_r41_(gatorfix|re2gator)_" -j 2 --timeout 1800` (20:17:01-20:19:02, `ctest_unit.txt`):
  **`100% tests passed, 0 tests failed out of 48`**, `Total Test time (real) = 120.51 sec` (nb4 21,11 s, nb4_elza 21,17 s).
- exe-Riegel: `ctest --test-dir "<Baum>/re15_port/build" -R "^integration_r41_(gatorfix_|re2gator)" -j 2 --timeout 1800 -V` (20:20:14-21:32:45, parallel zu meinen
  Laeufen und fremden Agenten; `ctest_exe.txt`): **`100% tests passed, 0 tests failed out of 20`**, `Total Test time (real) = 4350.57 sec`. Einzelzeiten: frei_elza
  683,86 s, abn2_wand_elza 684,27, abn2_wand 654,91, frei 657,79, abn2_leiterecke 616,46, abn2_rampe_west 625,54, abn2_nordbecken 564,31, abn2_ecken 577,05,
  oben_kante 478,70, oben_kante_elza 539,06, rampe_oben_elza 411,01, abn2_rampe_west_elza 682,29, rampe_oben 376,71, rampe_elza 361,52, rampe 320,30,
  gang_montage_elza 53,33, gang_szene_elza 47,30, gang_montage 32,84, gang_szene 27,39, re2gator 292,81 (alle Passed). 0 Zeilen "VERFEHLT" im Protokoll; gtrlog in
  jedem Lauf "Abweichungen 0 ... -> ok" (z. B. frei 130 Reaktionen, davon 40 "abgebrochen"; abn2_leiterecke 118/3; rampe 55/28); aussenlog ueberall 0/0/0;
  `r41_re2gator: ok` (nische Maul-Sitz 240 Bilder, Finisher Cut 6). Kein roter Riegel - nichts einzeln nachzufahren.
- Die Riegel sind gruen, OBWOHL M1 im Spiel ist: gtrlog und der Harness-Teil B zaehlen einen Treffer waehrend der RE2-Reaktion als "abgebrochen" und pruefen die
  Bisssperre +0x238 nicht; ein Riegel fuer "kein neuer Treffer vor 70 Bildern nach einem Treffer" fehlt.

## 3. Eigene Messung

### 3a M1 - Zweitbiss im selben Biss-Clip (T, Unverwundbarkeit)

Messung (`dbl7.py`, state.log letzter Besuch; Paar = zwei hp-Flanken <= 25 Bilder, dazwischen Gator-Clip gleich und af steigend; Art aus gator_trag.log Phase 3):

| Lauf | Treffer | Zweitbisse im selben Clip-4-Lauf | Abstand |
|---|---|---|---|
| a7_oben_leon | 49 | 18 | 18 x 20 (Sub 2) |
| a7_oben_elza | 59 | 29 | 1 x 20, 28 x 22 (Sub 3) |
| a7_frei4_leon | 101 | 43 | 20 und 22 |
| a7_frei4_elza | 93 | 42 | 8 x 20, 34 x 22 |
| a7_west4_leon | 46 | 21 | 20/22 |
| a7_west4_elza | 45 | 20 | 20/22 |
| a7_west5_elza | 50 | 24 | 20/22 |
| a7_ohne_frei4_leon / a7_ohne_oben_elza | 101 / 59 | 43 / 29 | 20/22 |
| sechs Kanten-Lagen (Abschnitt 3c, nach dem Sturz im Wasser) | 292 | 131 | 88 x 20, 43 x 22 |
| **Summe (15 eigene Laeufe)** | **895** | **400 (45 %)** | alle Paare Biss -> Biss (LUNGE, Gator-Clip 4); 0 Paare bei Rammen / CROSS-Biss / Hochbiss |
| dazu die juengsten Arbeitsordner der 17 exe-Riegel (Bau 6f4314fb, "mit"-Ordner; `messung_abnahme7.txt` Abschnitt 6) | 1149 | 514 (45 %) | 20/22, alle Biss -> Biss |

Bild fuer Bild (a7_oben_leon, state.log): F756 Treffer (Gator mo=4 af=7, stun 99), F757-F775 RE2 Sub 2 (Spieler-Clip 18/19, Gator af 8..26, stun 98..80),
F776 zweiter Treffer (Gator mo=4 **af=27**, derselbe Clip-4-Lauf, hp 100 -> 70, stun 99). Der zweite Treffer faellt genau auf das Bild, in dem die RE2-Sperre
+0x1D3 faellt (Sub 2: Ende-Bild 19 -> frei; Sub 3: Bild 21 = Clip 17 @0x801032f0). Leiste `abn7_ohne_oben_elza_biss_wasser_F740-F800_hell.jpg`: F756 der Kopf an
Elza, F758-F772 sie taumelt rueckwaerts (Clip 18/19), F776 der Kopf wieder an ihr, F778-F796 zweites Taumeln.

Mechanismus im Port (gelesen): die LUNGE-Bissbedingung enemy_ai_boss_gator.c :2452-2458 ist `!bite_done && anim_frame 6..34 && pl->hit_react == 0 && Maul/Abstand/
Kontakt && !Wand`; `bite_done = 1` wird nur beim Rammen (:3006) gesetzt; `hit_stun` (0x64 nach dem Treffer, :494) gilt fuer den START einer neuen LUNGE (:2307/:2378)
und den CROSS-Biss (:3015), nicht fuer den laufenden Clip; die RE2-Bisssperre +0x238 (gator_re2_r41.c `biss_cd`, je Bild -1 :1293 wie @0x801004a8-b8) steuert im
Port nur die Schwimm-Clipwahl (:1306/:1308, @0x80100b5c-64 / @0x80100be4-c04) und seit 88ff3f990 den Hochbiss (:2828) - weder den Start der LUNGE noch ihren
Biss. Vorher deckte der RE1.5-Niederwurf (41-92 Bilder gesperrt) den Rest des Clip-4-Laufs ab - jetzt sperrt die RE2-Reaktion 19 bzw. 21 Bilder, das Fenster
laeuft bis Bild 34.

Mechanismus in RE2 (eigene Disassembly `disasm/dis_t1_801012fc.txt`, `dis_kontakt_80101388.txt`, `dis_bisssperre_gates.txt`, `dis_bisssperre_kontext.txt`,
`dis_xrefs_biss.txt`):
- T1 0x801012fc macht die Biss-Maschine scharf (`sb v0(1),0x224(a0)` @0x8010137c) nur wenn Kontakt DAT_800cfc06 != 0, DAT_800cfbd8 & 0x10 == 0, PL+0x1D3 == 0,
  +0x224 == 0, +0x1C0 & 4 == 0, Stufe 0 oder 7; aufgerufen nur in den Angriffs-Unterzustaenden (`jal 0x801012fc` @0x80100c60 und @0x80100d40).
- Biss-Maschine 0x80101388 (aus dem Root je Bild, `jal 0x80101388` @0x80100328): Stufentafel @0x80100004 = 0x80101564 (aus), 0x801013c4, 0x801013d4 (Zaehler bis 4),
  0x80101418 (bis 4), 0x80101488 (bis 3), 0x801014f8 (bis 3, dann +0x224 = 0 @0x80101558); der Treffer-Einstieg `jal 0x80101578` @0x8010155c wird nur aus den
  Stufen 4 und 5 erreicht (3 + 3 Bilder) - die Stufen 2/3 springen nach 0x80101564 daran vorbei. Ein Treffer setzt PL+0x1D3 (0x801030e8-100 / 0x80103264-7c) -
  die restlichen Stufenbilder treffen nicht mehr: **ein Treffer je Biss**.
- Bisssperre +0x238: 70 im Treffer (`addiu v0,zero,0x46 / sb v0,0x238(a0)` @0x801016b0-b4), je Bild -1 im Root (`lbu / beqz / addiu -1 / sb` @0x801004a8-b8),
  110 nach dem Intro (@0x80100920), 0 bei INIT (@0x8010069c). Pruefstellen: @0x80100b5c `lbu v0,0x238(s0) / bnez v0,0x80100c08` (kein Wechsel in den Angriff),
  @0x80100bf4-0x80100c04 und @0x80100cb4-c4 `lbu v0,0x238(s0) / beqz / addiu v0,zero,0x101 / sw v0,4(s0)` (zurueck zum Schwimmen) - **kein neuer Biss vor 70 Bildern**.
- Folge: in RE2 liegen zwischen zwei Treffern mindestens 70 Bilder plus Anlauf; im Port 20/22.

Folge mit echten Trefferpunkten (`a7_tod_leon`, `a7_tod_leon_bild`; Leiste `abn7_tod_leon_doppelbiss_F936-F996_hell.jpg`): F942 Biss (Sub 2, Schaden 60) hp
100 -> 40; F943-F961 Taumeln rueckwaerts (Clip 18/19); F962 Zweitbiss im selben Clip-4-Lauf hp 40 -> -20 -> `GTR F962 TOT`, F963 Fress-Sequenz (FRESS-P2, RE2 FRESS P3
F1097). FUN_800401d4 (RE2_Quellcode_V2): HP - 60 = -20 < -14 -> Rueckgabe 2 = Tod, kein One-Save. Ein Schnapper = Game Over aus voller Gesundheit, bevor die Figur
einen Schritt tun kann. Nach der Szene (Gator +0x218 & 0x20, Schaden 30): 100 -> 70 -> 40 im ersten Schnapper, 10 -> -20 im zweiten (~4 s spaeter).

### 3b (T) Reaktion, Weg, Dauer, Animation (eigener Auswerter t7 + gtrlog)

| Lauf (Figur) | Treffer | Sub 2 / Sub 3 | vollstaendige Reaktionen: Dauer | Luftlinie max (Mittel) | Bildschritt max | RE1.5-Clips 12..16 | gtrlog |
|---|---|---|---|---|---|---|---|
| oben_leon | 49 (1 Rammen, 48 Biss) | 49 / 0 | Sub 2: 30 x 20 (+ Laufende 5) | 3980 (620) | 1017 | 0 | 49 Reaktionen, Abweichungen 0, ok |
| oben_elza | 59 (1 Rammen, 58 Biss) | 3 / 56 | Sub 2: 2 x 20; Sub 3: 27 x 54 | 3980 (97) | 1007 | 0 | ok |
| frei4_leon | 101 Biss | 42 / 59 | Sub 2: 24 x 20; Sub 3: 34 x 54 | 4766 (748) | 1039 | 0 | ok, Haken max 3735 |
| frei4_elza | 93 Biss | 18 / 75 | Sub 2: 10 x 20; Sub 3: 40 x 54 | 4239 (515) | 955 | 0 | ok |
| west4_leon / west4_elza / west5_elza | 46 / 45 / 50 Biss | 98 / 43 | Sub 2: 53 x 20; Sub 3: 22 x 54 | 4824 (740) | 967 | 0 | ok |
| ohne_oben_elza / ohne_frei4_leon | 59 / 101 | wie oben_elza / frei4_leon (bitgleich bis auf den Kanister) | | 3980 / 4766 | | 0 | ok |
| sechs Kanten-Lagen (Abschnitt 3c) | 292 (4 Rammen, 7 oben) | 193 / 99 | Sub 2: 104 x 20; Sub 3: 54 x 54 (Rest Laufende) | 4954 (519) | 1168 | 0 | ok, Haken max 4153 (Sturz) |

Die Bildschritte sind die RE2-Schritte (800, 680/650, ...) plus Koerper-/Boss-Schub; jede Luftlinie ueber 3735 stammt aus dem Schub (gtrlog: Haken-Weg ohne Sturz
nie ueber 3735, mit Sturz bis 4153 = Fall-Klemme; Luftlinie gesamt bis 4954). Die Clipfolge im Bild: Sub 2 18@0 -> 19@4 (Taumeln rueckwaerts, faellt nicht), Sub 3 20@0 -> 17@21 (liegt) -> 11@29 rueckwaerts
(steht auf) - Leon und Elza (Elza aus Record 0, Leisten `abn7_oben_elza_*`, `abn7_frei4_elza_*`: sie taumelt/liegt wie Leon). Elza posiert aus dem Leon-Record (O-T2) -
im Bild unauffaellig.

### 3c (W) Kante, Wand, Raum

- Rampe oben (Riegel-Szenario rampe_oben, Leon und Elza): F383 Rammen (Gator quert unter der Figur), Reaktion Sub 2 rueckwaerts Richtung Nord (A90: Gator-Gier 2969 ->
  Achse 3072 + 2048 = 1024, rueckwaerts = Nord), F384 Klemmlage -14968, F385 KANTE (Schritt -650 in die Nordleiste [8], Band 1 -> 0, y -1800 -> -1300 -> -1210,
  Klemme auf -14032), F386 -1070, F387 -880, F388 -640, F389 LANDUNG y 0 + Spritzer (Bild F392/F394), danach Leon/Elza im Nordbecken (Cut 3). Raumhalt-Zeilen im
  Sturz 0 (gtrlog "Sturz 0" Abweichungen). Leisten `abn7_oben_leon_rammen_kante_F376-F414_hell.jpg`, `abn7_oben_elza_rammen_kante_F376-F414_hell.jpg`.
- Eigene Kanten-Lagen (Spawn `RE15_PLAYER_POS=x,z,rot,1`, Kampf sofort `RE15_GB_TEST=1`, ohne Kanister, 4200 Bilder, Umgebung wie die Abnahme-6-Spawn-Laeufe; Cut 0,
  darum nur Logs). In allen sechs Lagen stoesst der ERSTE Treffer oben die Figur ins Wasser (Sturz Bild fuer Bild -1210/-1070/-880/-640/0, gelandet, Band 0,
  0 Raumhalt, aussenlog 0/0/0); kein Lauf verlaesst das Deck anders als im Sturz:

| Lage (Blick), Figur | erster Treffer oben nach Kampfbeginn (F280) | Art | Reaktion | Sturz | Landung | Treffer gesamt (4200) |
|---|---|---|---|---|---|---|
| Nordkante (3000,-14950) r1024 Leon | 119 Bilder (F399 Querungsbiss, Figur bleibt oben), F732 Rammen -> Sturz Sued | Rammen | Sub 3 | ja | (3000,0,-20848) | 36 |
| Suedkante (5800,-17650) r1024 Elza | 88 (F368) | Rammen | Sub 3 | ja | (5800,0,-20848) | 52 |
| Suedkante (6000,-17400) r2048 Leon | 84 (F364) | Rammen | Sub 2 | ja | (6000,0,-21503) | 49 |
| NW-Ecke (2300,-14950) r3072 Elza | 227 (F507 Querungsbiss, bleibt oben), F981 -> Sturz | Querungsbiss | Sub 2 | ja | (2314,0,-21655) | 48 |
| Ostende (6750,-16300) r1024 Elza | 148 (F428, bleibt oben), F533 -> Sturz Nord | Querungsbiss | Sub 3 | ja | (6732,0,-14035) | 59 |
| Mitte (4400,-16200) r0 Leon | 221 (F501, bleibt oben), F950 Rammen -> Sturz Sued | Rammen | Sub 2 | ja | (2318,0,-18568) | 48 |

  (obenlog kampf: erster Treffer oben 84..227 Bilder nach Kampfbeginn, laengste Pause oben 88..449 - Schranke 600; Abschnitt endet mit dem Sturz.) Hochbisse kamen
  in diesen Laeufen nicht vor (die Figur ist nach dem ersten Rammen/Querungsbiss unten); die +0x238-Sperre des Hochbisses (88ff3f990) belegt nur der Harness
  (Teil B "Oben halten": Hochbiss 8, 0 Abweichungen).
- Wand bleibt Wand: Harness Teil A "Deck Ost (Wand [6])" Ende (6732,-1800,-16300) Band 1, "Deck West (Plattf. [7])" (2318,-1800,-16300), "Deck Mitte Sub 2 West"
  (2313,-1800,-16300) - selbst gefahren (`harness/nb4.txt`).
- Nie aus dem Raum: aussenlog in allen eigenen Laeufen `aussen 0, Deck verlassen 0, im Block 0` (`messung_abnahme7.txt` Abschnitt 2).
- Abstieg per Aktionstaste (W4): eigener A/B mit der Bauer-Umgebung (Elza (6732,-14968) Band 1, `W2.0,A0.3,W4.0`, RE15_KEIN_GATOR=1): exe 6f4314fb und 5910fc24
  (3d57083b1) climb.log **bytegleich**, je 39 Bilder sub != 0 (`laeufe/w4_abstieg_*`).

### 3d (K) Kanister

`kanister7.py` (gator_boss.log / debug.log / state.log, letzter Besuch; Liegestelle Leon (-6870,-1200,-12651), Elza (-6842,-1200,-12641)):

| Lauf | Aufnahme Stufe 1 | laengs / quer | angehaengt | RUNDE / ANSCHWIMMEN / Zielmodus 1 | Treffer vor der Aufnahme |
|---|---|---|---|---|---|
| oben_leon (Figur oben, nach F389 im Nordbecken) | F1857 | 5444 / 1672 | F1869 | 0 / 0 / 0 | 16 |
| oben_elza | keine (4400) | - | - | 0 / 0 / 0 | 59 |
| frei4_leon (Rundweg) | F5689 | 1708 / -1156 | F5701 | 0 / 0 / 0 | 74 |
| frei4_elza (Rundweg) | F178 | 5583 / 1707 | F190 | 0 / 0 / 0 | 2 |
| west4_leon / west4_elza / west5_elza (Figur an der Rampe) | keine (3300) | - | - | 0 / 0 / 0 | 46 / 45 / 50 |
| exe-Riegel (juengste Arbeitsordner lauf_*, Bau 6f4314fb, mit = ohne bytegleich): abn2_ecken / abn2_leiterecke / frei_elza | F143 / F143 / F143 | 5825 / 147, 5825 / 147, 5826 / 117 | F155 | 0 / 0 / 0 | 1 / 1 / 1 |
| exe-Riegel abn2_wand / abn2_wand_elza / abn2_rampe_west | F164 / F152 / F888 | 5996 / -762, 5928 / -617, 5535 / 1277 | F176 / F164 / F900 | 0 / 0 / 0 | 2 / 1 / 5 |
| exe-Riegel frei (8400) / rampe (3600) / rampe_elza (3600) / rampe_oben_elza (4400) / abn2_rampe_west_elza (8100) / abn2_nordbecken (755) | keine | - | - | 0 / 0 / 0 | alle |

Lesart: in 9 von 19 Laeufen mit liegendem Kanister nimmt der Gator ihn im Vorbeikommen auf, davon 7 in den ersten 6 s des Rueckkehr-Besuchs (F143..F178: die
Verfolgung fuehrt zu Beginn durch das Nordwestbecken an der Liegestelle vorbei), sonst F888 / F1857 / F5689; in 10 Laeufen bleibt er liegen (Figur an der Rampe oder
Rundweg ohne Vorbeikommen, 3300-8400 Bilder). Alle Aufnahmen laengs 1708..5996, |quer| <= 1707 - im Fenster. Genau wie (K) es verlangt ("kommt er nie daran
vorbei, bleibt der Kanister liegen"). Bitgleich bis zum ersten Treffer gegen den Stand 3d57083b1 (Abnahme-6-Laeufe
neu_west4_leon / neu_west4_elza / neu_west5_elza / ohne_oben_elza / ohne_frei4_leon, exe 5910fc24): erste abweichende state.log-Zeile = das Trefferbild (F123 / F137 /
F92 / F383 / F139; dort rot 0 statt 3072 = FUN_8001569c, hp 70 statt 50 = RE2-Schaden 30 statt Tafel 50). Die Pause "erst der Kanister" (Abnahme 6: 851-856 Bilder)
gibt es nicht mehr: oben_leon/oben_elza erster Treffer oben nach 300 Bildern (obenlog ab0).

## 4. Bild (RE15_FRAMEDUMP-Serien, F-genau, selbst angesehen) - `G2_belege/abn7/`

- `abn7_oben_leon_rammen_kante_F376-F414_hell.jpg` / `abn7_oben_elza_rammen_kante_F376-F414_hell.jpg`: F376-F382 Figur oben an der Nordkante, der Gator-Koerper kommt
  unter ihr hoch; F384 Taumeln rueckwaerts an der Kante (Clip 18); F386/F388 in der Luft (y -1070 / -640); F390 im Wasser (y 0); F392/F394 Spritzer um die Figur;
  F396-F414 steht im Nordbecken, der Gator neben ihr (Cut 3). Kein RE1.5-Niederwurf, kein Liegen.
- `abn7_frei4_leon_biss_wasser_F136-F196_hell.jpg` (Cut 0, Figur klein/dunkel - Beleg die Lagezeilen): F139 Biss, Sub 3: -13033 -> -11392 -> -9392 -> -9112 (800+680+
  560+440 / 320+200+80), F164 liegt (Clip 17), F172-F192 steht auf (Clip 11 rw), F196 steht.
- `abn7_frei4_elza_biss_wasser_aufnahme_F136-F196_hell.jpg`: F138 Biss (Sub 3 vorwaerts -2464 -> -3498), **F160 zweiter Biss im selben Clip (hp 70)**, F164-F188 liegt,
  F192 steht auf; der Gator nimmt den Kanister im Vorbeikommen (Stufe 1 F178).
- `abn7_ohne_oben_elza_biss_wasser_F740-F800_hell.jpg`: der Doppelbiss im Bild (Abschnitt 3a).
- `abn7_tod_leon_doppelbiss_F936-F996_hell.jpg`: F942 Biss (hp 40), F944-F960 Taumeln rueckwaerts (Clip 18/19), F962 Zweitbiss hp -20, F964 ff. Fress-Sequenz (Leon vor
  dem Maul, Kamera-Schnitt).
- `abn7_west4_elza_F1600-F1700_hell.jpg`: Elza in der SO-Ecke (6732,-26532), Sub-3-Reaktion (Clip 20 -> 17 -> 11), der Gator an ihr; kein Niederwurf.

## 5. Die offenen Punkte des Bauers - Einstufung

1. **A90 (O-T1)**: PORT-WAHL ohne Nutzer-Wortlaut, im Code und Messblatt als solche gekennzeichnet; RE2 uebergibt die vier Achsen 0/1024/2048/3072 nach der
   Gator-Lage im Korridor (@0x801015f0-80, selbst nachgelesen). Entscheid des Orchestrators; nicht blockierend. Folge im Spiel: fast jeder Treffer oben stoesst die
   Figur ueber eine Kante (eigene Laeufe: der erste Oben-Treffer beendet den Oben-Abschnitt).
2. **Animation im Sturz (O-W1)**: RE2-Haltung laeuft im Fall weiter - im Bild (F386/F388) wirkt es als Rueckwaerts-Taumeln ueber die Kante; kein Bruch sichtbar.
   Sichtpruefung Nutzer, nicht blockierend.
3. **Luftlinie mit Koerper-/Boss-Schub** bis 4954 (Kanten-Lage Sued Leon; west4_elza 4824) - gegen 13 059 vorher; nicht blockierend.
4. **B2-Biss** (env RE15_GB_LUNGE_B2) in keinem Riegel - teilt die LUNGE-Stelle und damit M1 (Bissfenster f >= 114 ohne Sperre).
5. PSX nicht gebaut/gemessen - auch hier nicht.
6. Vibration/Gator-Effekt/Nacken-Bits nicht Teil der Reaktion - bestaetigt, nicht blockierend.
7. Elza posiert aus Record 0 - im Bild unauffaellig (Abschnitt 4).
8. west4_elza steigt nicht mehr hinauf - bestaetigt (eigener Lauf: Elza bleibt im Wasser, SO-Ecke); W4 ueber den eigenen Abstiegs-A/B belegt.

## 6. Konstanten im Diff 3d57083b1..b4dfc23a9 (selbst nachgelesen)

| Zahl | Stelle | Beleg (eigene Disassembly `disasm/`) | Bewertung |
|---|---|---|---|
| 0x205 / +1, FUN_80015910 | Einstieg, Unterzustand | `addiu v0,zero,0x205` @0x801015b4, `sw v0,-0x404(at)` = PL+4 @0x801015c4, `jal 0x80015910` @0x801015c8, +5 += 1 @0x801015dc-ec | belegt |
| Achsen 0/0x800/0xc00/0x400, FUN_8001569c(.., 1536) | Richtung | `slti v0,v0,-0x2327` @0x801015f8, `slti -0x3cef` @0x80101630, a1 = 0/0x800/0xc00/0x400 @0x80101610-80, `jal 0x8001569c / addiu a2,zero,0x600` @0x80101684-88 | belegt; A90-Rasterung = PORT-WAHL (Offen 1) |
| 60 / 30, Bit 0x20, 25000 | Schaden | `lhu v0,0x218(s1) / andi 0x20 / addiu a0,zero,0x3c / 0x1e` @0x8010168c-a0; `lh v0,0x156 / slti 0x61a8 / ori 0x20` @0x80101ebc-dc; `jal 0x800401d4` @0x80103918, Bit 0 -> PL+6 = 4 @0x80103924-34, Bit 1 -> PL+4 = 3 @0x80103938-48 | belegt |
| 70 | Bisssperre | `addiu v0,zero,0x46 / sb v0,0x238(a0)` @0x801016b0-b4 | belegt - aber nur der Hochbiss fragt sie (M1) |
| -800 / +150 / +15, Clip 18/19, Blend 3, SE 0x04010001, 1024 | Sub 2 | `addiu v0,zero,-0x320 / sh 0x144` @0x801030e0-e4, `addiu v0,v0,0x96` @0x80103144, `addiu v1,v1,0xf` @0x801031ac + `blez` @0x801031b8, `lui v0,3 / ori 0x12 / sw 0x14c` @0x801030cc-d4, `ori 0x13` @0x80103154, `lui a0,0x401 / ori 1 / jal 0x8005ba28` @0x801030bc-c4, `addiu a3,zero,0x400` @0x80103138; Tafel @0x80100074 | belegt |
| +800 / -120, Clip 20/17, Basis-Clip 6 rw (0x10400), SE 0x04020001 | Sub 3 | `addiu v0,zero,0x320` @0x8010325c, `addiu v0,v0,-0x78` @0x801032bc + `bgez` @0x801032cc, `ori 0x14` @0x8010324c, `ori 0x11` @0x801032e0, `sb zero,0x1d3` @0x801032f0, `lui v1,3 / ori 6` @0x80103328-2c, `lw a1,0x108 / lw a2,0x17c / lui a3,1 / ori 0x400` @0x80103344-54; Tafel @0x8010008c | belegt |
| +0x1D3 = 0 am Ende, +0x1C0 &= 0xED, PL+4 = 1 | Ende | @0x801031c8-dc / @0x80103358-70 | belegt |
| Haken 0x80102D08, Tafel @0x80104690 [2]/[3] | Spieler-Haken | `lui v0,0x8010 / addiu 0x2d08 / sw v0,-0x1c74(at)` @0x801004f4-500; Tafel 80102d44 80102f48 8010307c 801031f8 | belegt |
| Zielmodus-Bit, Stufe-1-Fenster 8601 + 0x18f | (K) | `lbu 0x226 / sltiu 2 / ori 4 / sb 0x1c0` @0x80100a34-5c, aus bei -9000 / -120 @0x80100a88-8c, `addiu -0x2199 / sltiu 0x18f` @0x80100ae4-e8; T1 `andi 4 / bne` @0x8010134c-58 | belegt; Abschalten fuer den Szenen-Kanister = PORT-WAHL mit Nutzer-Wortlaut (K) |
| 500 / 90 / 50 t / -800 / y = Boden | Fallschritte (W) | ghidra1_V2.txt: `addiu v0,v0,0x1f4` @0x8003843c, `addiu t0,t0,0x5a` @0x8003866c, 50 t = `sll 1 / addu / sll 3 / addu / sll 1` @0x80038680-94, `addiu v0,v0,-0x320 / slt` @0x800386b4-b8, `lh v0,0x1ba / sw v0,0x38` @0x80038724-2c | belegt (unveraendert aus climb_sub10) |
| Abwaerts-Bit 0x2, Leisten [8]/[9] | Kante | climb_common.c @0x8002d74c-50 (Abnahme 6 nachgelesen), ROOM2090.RDT @0x754/@0x760 u1 0x02 (Abnahme 6 sca) | belegt |
| GATOR40A0.RBJ 14 328 B, Clips 17-20 = 16/10/30/21 | Raumbank | Datei bytegleich ROOM40A0.RDT[0x65E8:0x9DE0] (RDT+0x60 -> 0x65E8), md5 2858954641c305aa342b01205c4d1090; EDD @0x29A4: Clip 17 16, 18 10, 19 30, 20 21 Bilder, 21 Clips | belegt |
| 1000 (s_kd_speed) im Kommentar | Herleitung | `ori v0,zero,0x3e8` @0x8003615c | belegt |
| RUNDE/ANSCHWIMMEN aus, Zonen-Ruhe (a) aus | (K) | PORT-WAHL mit dem Nutzer-Wortlaut (K) im Code (kanister2080_r41.c, gator_re2_r41.c) und in 67b3b2356 | belegt (PORT-WAHL) |
| Riegel: 6000 / 1733 (Aufnahmefenster), 3735/3080, +468, NB3_PAUSE_MAX 600 | Tests | bestehend (NB1/NB2/NB3), 3735/3080 = Summen der RE2-Schritte, 468 = GB_PRK (Abnahme 6) | belegt |

Keine Zahl ohne Beleg gefunden; der einzige formale Rest ist die A90-Rasterung (PORT-WAHL ohne Nutzer-Wortlaut, vom Bauer offen gemeldet).

## 7. Fragen an den Nutzer (Sie-Form)

- F1: "Nach dem Treffer an der Rampenkante faellt Ihre Figur jetzt ins Wasser (wie gewuenscht). Mit der gewaehlten Stossrichtung stoesst praktisch jeder Treffer oben
  die Figur von der Rampe - oben bleibt man also nur bis zum ersten Treffer. Ist das so in Ordnung?"
- F2: "Waehrend des Sturzes taumelt die Figur in der Resident-Evil-2-Haltung weiter (kein eigener Fall-Clip, weil Resident Evil 2 an dieser Stelle keinen Sturz kennt).
  Bitte schauen Sie sich die Bildleisten abn7_oben_leon/elza_rammen_kante an: ist dieser Anblick fuer Sie in Ordnung?"

## 8. Tests (Summenzeilen woertlich)

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`build_check.txt`).
- `ctest -R "^unit_r41_(gatorfix|re2gator)_" -j 2 --timeout 1800`: **`100% tests passed, 0 tests failed out of 48`**, `Total Test time (real) = 120.51 sec`.
- `test_r41_gatorfix_g2abn7.exe nb4` / `nb4_elza` direkt: `nb4: 0 Fehler` / `nb4_elza: 0 Fehler`, exit 0.
- exe-Riegel `ctest -R "^integration_r41_(gatorfix_|re2gator)" -j 2 --timeout 1800 -V` (20:20:14-21:32:45, `ctest_exe.txt`): **`100% tests passed, 0 tests failed out of 20`**,
  `Total Test time (real) = 4350.57 sec`; Arbeitsordner der Riegel-Laeufe mit gtrlog/dbl7/kanister7 nachgerechnet (`messung_abnahme7.txt` Abschnitt 6: 17 Ordner,
  1149 Treffer, 514 Zweitbisse im selben Clip).
- Eigene exe-Laeufe: 19 gewertete (oben_leon/elza, frei4_leon/elza, west4_leon/elza, west5_elza, ohne_frei4_leon, ohne_oben_elza, tod_leon, tod_leon_bild,
  w4_abstieg_neu/alt, sechs Kanten-Lagen), alle `exit=0` bis RE15_EXIT_AT (`messung_abnahme7.txt` Abschnitt 1); drei verworfene (zwei Elza-Laeufe mit
  RE15_TITLE_SHOT booteten als Leon, eine Tod-Probe ohne TITLE_SHOT blieb im Titel - eigene Prozesse beendet, Laeufe wiederholt).

## 9. Nicht geprueft

PSX-Bau; der B2-Biss (env RE15_GB_LUNGE_B2); Raumwechsel mitten im Kampf; Hoeren (SE 1/2 nur im Log); der Tod mit Figur oben; Elza in der Tod-Probe (Leon gemessen,
Mechanismus figurunabhaengig); Kanister-Explosion mit Figur im Wasser nach dem Sturz.
