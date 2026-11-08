# Runde 41 - Spur G2 "gator_re2" - UNABHAENGIGE ABNAHME 8 (2026-11-08): G2-RE2 NB4 Runde 2 (Abnahme 7 M1: Zweitbiss im selben Biss-Clip)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Kopf 9c3659bf7 (Bau-exe md5 42a1c8f76e83e13818bb43648c0f825a vom 2026-11-07 21:56:42, Testprogramm
md5 d078d22d76b32b1f9bdbb32ce065e577). Gepruefte Nachbesserung: d8bf728b0 (Biss im Wasser, Biss B2 und Rammen fragen die "RE2-Bisssperre" +0x238 wie der Hochbiss),
4437cdfd1 (Riegel Bisssperre in nb4(_elza) und gtrlog), 3bd9e1b05 (Riegel fuzz mit der Trefferreaktion des Stands f942f56c0), ff0f2571a (Belege), cf9a2ac3f
(integration_r41_re2gator Lauf nische EXIT_AT 560 -> 770), 9c3659bf7 (Dossier). Vorgaenger: Abnahme 7 (47ab6f1fa, NICHT BESTANDEN wegen M1 Zweitbiss).
Pruefer: Abnahme-Agent, glaubt dem Bau nichts, misst selbst. Ein frueherer Anlauf dieser Abnahme brach am Modell-Limit ab; seine unversionierten Reste unter
`G2_belege/abn7/runde2/` habe ich NICHT verwendet, sondern unveraendert in den Laufordner verschoben (`<SP>/vorgaenger_runde2/`) und den Ordner mit eigenen Belegen
neu gefuellt. Kein Spiel- und kein Testcode geaendert; dieser Commit bringt nur diesen Bericht und `G2_belege/abn7/runde2/`.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn8`
(`runs/<lauf>/` mit state.log, climb.log, gator_boss.log, gator_trag.log, debug.log, env.txt, pid.txt, exe_md5.txt, fd_NNNNNN.ppm).
Belege im Baum `G2_belege/abn7/runde2/`: `messung_abnahme8.txt` (alle Auswertungen), `disasm/` (eigene RE2-Disassembly), `harness/` (nb4/nb4_elza), `werkzeug/`
(Laufskripte, Auswerter), `laeufe/<lauf>/` (env, pid, md5, GTR-/Kanister-Auszug), `leisten/`, `bahnen/`, `ctest_unit.txt`, `ctest_exe.txt`, `build_check.txt`,
`messkopie_md5.txt`.

Vorgaben (Wortlaut aus dem Auftrag, unveraendert): **(T)** fuer JEDEN Alligator-Angriff, der die Figur trifft, die Trefferreaktion der Figur aus RE2 (Weg, Richtung,
Dauer, Animation, Fallen/Taumeln, Schaden, Unverwundbarkeit) statt des RE1.5-Niederwurfs. **(W)** Nutzer 2026-11-07 ~15:00 woertlich: "er kann den Spieler ruhig In
das Wasser stoßen." **(K)** Nutzer 2026-11-07 ~15:00 woertlich: "Die Frage stellt sich garnicht, da leon mit den alligator ganz normal seine runden drehen soll.
Das heißt der alligator sollte Ganz natürlich irgendwann zum Kanister kommen, während er leo. Auf dem rundweg verfolgt...." Bleibt: Gang ohne Kanister bitgleich
NB5, Vorgabe A (oben nicht sicher), Vorgabe B (kein Haenger), Raumhalt Band 0, Kanister-Sitz 0, Rueckfall nur aus dem Haenge-Fenster.

## 0. Urteil

**NICHT BESTANDEN - ein blockierender Befund (M1): Die "RE2-Bisssperre" +0x238, auf der die ganze Nachbesserung beruht, ist in RE2 keine Bisssperre. Der
RE2-Beleg traegt nicht (RE-Gate, Auftrag Punkt 2 und 5), und die Sperre erzeugt im Spiel eine neue, sichtbare Abweichung von RE2: Biss-Anlaeufe mit dem Maul an
der Figur ohne Biss nach dem Intro.** Der Befund der Abnahme 7 (Zweitbiss im selben Biss-Clip) ist im Ergebnis behoben; (T), (W), (K), Vorgabe A und B und der
Gang ohne Kanister sind erfuellt; die Riegel sind gruen.

- **Abnahme-7-M1 im Ergebnis behoben.** 27 eigene exe-Laeufe (13 Leon, 14 Elza): 1164 Treffer, kleinster Abstand zweier Treffer 105 Bilder, 0 Treffer unter
  70 Bildern, 0 Zweitbisse im selben Gator-Clip-Lauf, 0 Bilder RE1.5-Niederwurf. In 9 von 10 Laeufen mit Abnahme-7-Gegenstueck (gleiche Umgebung, Bau 6f4314fb)
  ist die erste abweichende state.log-Zeile genau das Bild des frueheren Zweitbisses (tod_leon F962, oben_leon / oben_elza / ohne_oben_elza F776, frei4_leon
  und ohne_frei4_leon F579, frei4_elza F160, west4_leon F250, west4_elza F159), davor bytegleich. Echte Trefferpunkte (tod_leon, tod_elza): F942 Biss 100 -> 40,
  Taumeln bis F962, kein Zweitbiss (das Maul ist F962-F970 wieder an der Figur), naechster Biss F1049 (Runde 1: F962 Zweitbiss -20 = Tod in 20 Bildern).
  Harness nb4/nb4_elza selbst gefahren: 589 Treffer-Paare, kleinster Abstand 70, 0 Zweitbisse; Gegenprobe 'Sperre alt' 27 Zweitbisse (rot ohne den Bau).
- **M1 (blockierend): RE2 sperrt mit +0x238 den Spurt, nicht den Biss** (eigene Disassembly EM23_OVL_0000.BIN, md5 0293e099bbeae989776264eb14c81bc9, Abschnitt 3):
  - Der RE2-Biss ist die Kiefer-Maschine +0x224. Scharf macht sie T1 0x801012fc (`sb v0(1),0x224(a0)` @0x8010137c) bei Koerperkontakt PL+0xE (= 0x800cfc06,
    `lhu v0,-0x3fa(v0)` @0x80101300, `beqz` @0x80101308), 0x800cfbd8 & 0x10 == 0 (@0x80101310-20), PL+0x1D3 == 0 (@0x80101328-34), +0x224 == 0 (@0x8010133c-44),
    +0x1C0 & 4 == 0 (@0x8010134c-58), +0x226 in {0, 7} (@0x80101360-74) - **+0x238 kommt nicht vor**. T1 laeuft jedes Bild am Anfang der Vorstufe von Sub 1 UND
    Sub 2 (`jal 0x801012fc` @0x80100974; Vorstufen-Tafel @0x80104640/44 = 0x80100964), in Sub 3 (@0x80100d40) und Sub 7 (@0x8010125c). Der Treffer-Einstieg
    0x80101578 (aus den Stufen 4/5, `jal` @0x8010155c) fragt nur Kontakt (@0x80101590-98) und PL+0x1D3 (@0x801015a0-ac). Die Kontakt-Bits PL+0xE schreibt die
    RE2-EXE im Spieltakt (FUN_800355c4 `sh s3,0xe(s1)` @0x80035658, Aufruf @0x80026628 mit dem Spieler 0x800cfbf8, je Gegner FUN_80034d0c).
  - +0x238 wird nur gelesen @0x80100b5c (Sub 1: kein Wechsel nach Sub 2 = Paddeln/Spurt, Clip 1), @0x80100bf4-c04 (Sub 2 -> Sub 1) und @0x80100cb4-c4. Die
    letzte Stelle liegt in 0x80100c54, auf die weder eine Tafel noch ein `jal` noch ein Datenwort zeigt (toter Code, `disasm/dis8_verweise_80100c54.txt`).
    Die vom Bau zitierte "Pruefstelle @0x80100cb4-c4" ist damit kein Beleg.
  - Entscheidend: am Intro-Ende loescht RE2 im selben Schritt die Biss-Bremse 0x800cfbd8 Bit 0x10 (`addiu v1,zero,-0x11 / and / sw` @0x801008f8-918, die T1
    @0x80101310-20 fragt) und setzt +0x238 = 110 (@0x8010091c-20). Nach dem Intro beisst RE2 also bei Kontakt, obwohl +0x238 > 0 ist. Nach einem Treffer ist in
    RE2 der naechste Biss bei Kontakt moeglich, sobald die Reaktionssperre faellt (Sub 2 `sb zero,0x1d3` @0x801031d4, Sub 3 @0x801032f0) und die Kiefer-Maschine
    neu gelaufen ist (Scharf-Bild + 8 Bilder bis zur ersten Stufe-4-Pruefung, Stufen @0x801013c4 / 0x801013d4 / 0x80101418 / 0x80101488): fruehestens rund
    28 bzw. 30 Bilder nach dem ersten Treffer - nicht "fruehestens 70".
  - Die Behauptungen des Baus "laesst keinen Angriff zu, solange sie != 0 ist ... ein Treffer je Biss, der naechste fruehestens 70 Bilder spaeter"
    (Kommentar d8bf728b0 in enemy_ai_boss_gator.c), "T3 Unverwundbarkeit / Angriffstakt wie RE2 ... kein Treffer, solange die Bisssperre +0x238 > 0 ist"
    (Riegel nb4, gtrlog) und die Herleitung der Abnahme 7 ("Biss-Maschine nur in den Angriffs-Unterzustaenden scharf, Aufrufe @0x80100c60 / @0x80100d40" - der
    Aufruf @0x80100974 fehlt dort, @0x80100c60 ist toter Code) sind durch die Bytes widerlegt. Die Zahl 70 selbst stimmt (@0x801016b0-b4); ihre Verwendung als
    Bissbedingung ist weder RE2 noch eine PORT-WAHL mit Nutzer-Wortlaut.
  - **Sichtbare Folge:** Der Anlauf (LUNGE-Start) fragt weiter nur hit_stun, der Biss die 110/70-Sperre. Riegel-Szenario nische (eigener Lauf a8_nische2,
    Umgebung = integration_r41_re2gator): nach dem Intro-Ende F280 zwei Biss-Anlaeufe (Clip 4) F284-F326 und F347-F389 mit dem Maul an der Figur (maul 1328
    bzw. 864; Bissbedingung ausser der Sperre in 8 bzw. 29 Bildern erfuellt, im zweiten Anlauf das ganze Fenster Bild 6..34 einschliesslich offenem Kiefer
    Bild 6..24) - kein Biss; erster Biss F416 (Runde 1 laut Dossier und cmake-Kommentar: F290). Wiedereintritt: in west5_elza (bis F91 bytegleich zu Runde 1)
    beisst Runde 1 F92, Runde 2 erst F113 im selben Anlauf - +0x238 war im dritten Besuch bis rund F110 noch gesetzt. Leiste
    `leisten/a8_nische_leerangriffe_intro_sperre_F286-F384_hell.jpg`. RE2 wuerde in diesem Fenster nicht spurten und bei Kontakt beissen.
  - Kleinster belegter Fix (Vorschlag, nicht Teil dieser Abnahme): ein Treffer je Anlauf wie in RE2 ein Treffer je Kiefer-Lauf (die Reaktion setzt PL+0x1D3,
    Sub 2 @0x801030e8-80103100, die restlichen Stufe-4/5-Aufrufe scheitern @0x801015a0-ac, der Lauf endet mit +0x224 = 0 @0x80101558, ein neuer braucht T1
    @0x8010133c-44 und 8 Bilder): `g->bite_done = 1` im Treffer (gb_biss_abschluss) statt der +0x238-Bedingung in LUNGE / B2 / Rammen / Hochbiss; der Anlauf
    bleibt an hit_stun. Kommentare und Riegel-Texte auf den Befund oben berichtigen (kein "wie RE2: kein Treffer vor 70 Bildern", @0x80100cb4-c4 streichen).
- **(T) sonst erfuellt:** jede Reaktion = RE2-Modell (Sub 2: 20 Bilder, Clips 18/19; Sub 3: 54 Bilder, Clips 20/17/11 rw; Ausnahme siehe Beobachtung B1),
  Schaden 60 / 30 (geschwaecht), 0 Bilder RE1.5-Niederwurf, Haken-Weg <= 3735/3080 (+468 im Sturz), Luftlinie mit Koerper-/Boss-Schub bis 5052 (wie Abnahme 7).
- **(W) erfuellt:** 8 Stuerze (Nord- und Suedkante, Leon und Elza), y -1210/-1070/-880/-640/0 Bild fuer Bild, Band 1 -> 0, gelandet; nie ausserhalb des Raums.
- **(K) erfuellt:** 0 Zeilen RUNDE / ANSCHWIMMEN / Zielmodus 1 in allen Laeufen; Aufnahme nur im Fenster (laengs 2396..5920, |quer| <= 1719) und nur in der
  Verfolgung; in 10 von 17 Laeufen mit liegendem Kanister nach 6-172 s, in 7 bleibt er liegen (Figur kam nicht vorbei bzw. der Gator querte im Dash).
- **Bleibt erfuellt:** Vorgabe A (erster Treffer oben 8..300 Bilder, laengste Pause oben <= 333), Vorgabe B (Haenge-Fenster 0, ohne Fortschritt max 295 <= 450),
  Gang ohne Kanister (unit gang/gang_elza gruen; ohne-Laeufe bis zum ersten Treffer bytegleich zur Abnahme 6), Kanister-Sitz 0, Riegel unit 48/48, exe 20/20.

| Punkt | Soll | Ist (eigene Messung) | Bewertung |
|---|---|---|---|
| Bau | LOCAL-BUILD-OK, exe = Messstand | `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`; exe md5 42a1c8f7 (= Bauer-Messkopie), Testprogramm d078d22d | erfuellt |
| Abnahme-7-M1 Zweitbiss | kein zweiter Treffer im selben Biss-Clip | 27 Laeufe, 1164 Treffer, kleinster Abstand 105, 0 unter 70, 0 im selben Clip-Lauf; tod_leon/elza F942 -> F1049 statt F962 TOT | behoben |
| RE2-Beleg der Sperre (Auftrag 2 und 5) | Mechanismus mit Bytes belegt oder PORT-WAHL mit Nutzer-Wortlaut | +0x238 sperrt in RE2 nur Sub 1 -> 2 (@0x80100b5c-64, @0x80100bf4-c04); Biss T1 @0x801012fc ohne +0x238; Intro-Ende loescht die Biss-Bremse und setzt 110 im selben Schritt (@0x801008f8-920); @0x80100cb4-c4 toter Code | **M1 - nicht erfuellt** |
| Folge im Spiel | kein Biss-Anlauf ohne Biss, wo RE2 beisst | nische: 2 Anlaeufe nach dem Intro mit Maul an der Figur ohne Biss (8 + 29 Bilder), erster Biss F416 statt F290; west5_elza erster Biss F113 statt F92 | **M1 - nicht erfuellt** |
| (T) Reaktion | RE2-Modell, 0 RE1.5-Niederwurf | Sub 2 356x 20 Bilder (18/19), Sub 3 805x 54 Bilder (20/17/11), gtrlog "Abweichungen 0" in allen Laeufen, 0 Niederwurf-Bilder; Harness 605 Reaktionen, 0 Abweichungen | erfuellt |
| (T) Weg | Haken <= 3735/3080 (+468 Sturz) | Haken max 4021 (Sturz) / 3735; Luftlinie mit Schub bis 5052 (Runde 1: 4954) | erfuellt (Schub offen wie Abnahme 7) |
| (W) Kante -> Wasser | Sturz wie Substate 10, nie aus dem Raum | 8 Stuerze, je -1210/-1070/-880/-640/0, Band 1 -> 0; aussenlog 26x "aussen 0, Deck verlassen 0, im Block 0" (3 weitere zaehlen nur Finisher-Bilder nach dem Tod) | erfuellt |
| (K) kein Holmodus | 0 RUNDE / ANSCHWIMMEN / Zielmodus 1 | 0/0/0 in allen 29 Laeufen; Harness K1 0 von 6 abweichend | erfuellt |
| (K) Aufnahme im Vorbeikommen | nur im Fenster, sonst liegen lassen | 10 von 17 Laeufen, F185..F5167 (6..172 s), laengs 2396..5920, quer -1719..1628, alle in der Verfolgung; 7 Laeufe ohne Aufnahme | erfuellt |
| Vorgabe A | erster Treffer oben / Pause <= 600 | 8 Laeufe: erster Treffer oben 8..300, laengste Pause 110..333 | erfuellt |
| Vorgabe B | Haenge-Fenster 0, ohne Fortschritt <= 450 | 20 Laeufe: Fenster 0, max 295 Bilder | erfuellt |
| Gang ohne Kanister | bitgleich NB5 | unit gang/gang_elza gruen; ohne_frei4_leon / ohne_oben_elza bis F139 / F383 (erster Treffer) bytegleich zur Abnahme 6 | erfuellt |
| Riegel | unit + exe gruen | unit `100% tests passed, 0 tests failed out of 48`; exe siehe Abschnitt 2 | erfuellt |
| Konstanten im Diff | @0x oder PORT-WAHL + Wortlaut | keine neue Zahl; 70 belegt, aber als Bisssperre falsch verwendet (M1); EXIT_AT 770 gemessen | siehe M1 |

## 1. Bau, Messweg

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build` (00:08): `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
  (`build_check.txt`). Arbeitsbaum sauber bis auf die Vorgaenger-Reste (verschoben, s. o.); exe 2026-11-07 21:56:42 nach der letzten Quell-Aenderung
  (enemy_ai_boss_gator.c 21:56:17), Testprogramm 22:01:29 nach test_r41_gatorfix.c (22:01:27).
- Messkopien (eigene Namen, nur eigene PIDs): `re15_pc_g2abn8.exe` und `tests/unit/test_r41_gatorfix_g2abn8.exe` (`messkopie_md5.txt`). Kein gdigrab; Bilder nur als
  RE15_FRAMEDUMP-Serie (F = Raum-Bildzaehler wie state.log). Parallel liefen meine exe-Riegel-Gruppe und fremde Agenten (r41_integration).
- 29 eigene exe-Laeufe in 5 Wellen (`werkzeug/welle1.sh`..`welle5.sh`, `lauf8.sh`), alle `exit=0` bis RE15_EXIT_AT. Ein weiterer Lauf (`a8_nische`, EXIT_AT 800)
  lag hinter dem Game Over (F798) - die Folgeraeume ueberschrieben die Bildnummern; eigene PID beendet (exit 143), verworfen, als `a8_nische2` mit EXIT_AT 780
  wiederholt.
- Determinismus: tod_leon, tod_elza, oben_leon, oben_elza, frei4_leon, frei4_elza, aufn_elza sind zu den Bauer-Laeufen r2_* (`g2nb4/r2/runs/`) in state.log und
  gator_boss.log **bytegleich**; die Bildserien-Wiederholungen west5_elza_kletter und frei4_elza_schub sind zu west5_elza / frei4_elza bytegleich bis Laufende.
- Auswerter: eigene, von den GTR-Zeilen des Bauers unabhaengige Werkzeuge `a8.py` (state.log + climb.log: hp-Flanken, Abstand, gleicher Gator-Clip-Lauf,
  Reaktion, Dauer, Clips, Weg, Sturz), `a8biss.py` (Biss-Telemetrie Bild fuer Bild auf die Clip-4-Laeufe ausgerichtet; gueltig, wo die Zahl der BISS-Zeilen
  gleich der Zahl der Clip-4-Bilder ist - in Laeufen mit Hochbiss-Fenstern ohne BISS-Zeilen nicht), `a8schub.py`, `a8bahn.py`, `a8fenster.py`, `erst8.py`,
  `lst8.py`, `dis8.py` / `suche8.py` (capstone, Basis 0x80100000); dazu die Riegel-Auswerter gtrlog / aussenlog / haenglog / obenlog aus der Kopie des
  Testprogramms.

## 2. Riegel (Summenzeilen woertlich)

- `ctest --test-dir "<Baum>/re15_port/build" -R "^unit_r41_(gatorfix|re2gator)_" -j 2 --timeout 1800` (00:08-00:09:30, `ctest_unit.txt`):
  **`100% tests passed, 0 tests failed out of 48`**, `Total Test time (real) =  96.38 sec`.
- `test_r41_gatorfix_g2abn8.exe nb4` / `nb4_elza` direkt (`harness/nb4_auszug.txt`, `nb4_elza_auszug.txt`): `nb4: 0 Fehler` / `nb4_elza: 0 Fehler`. Teil A
  13/13; Teil B 605 Reaktionen, 0 Abweichungen, Rammen 55 / CROSS-Biss 22 / Hochbiss 11 / Biss 517 / B2 0; Bisssperre 589 Paare, kleinster Abstand 70,
  0 Zweitbisse; Gegenprobe 'Sperre alt' 56 Paare, kleinster Abstand 20, 27 Zweitbisse; Gegenprobe 'Treffer alt' 1804 Niederwurf-Bilder; K1 0 von 6.
- exe-Riegel: `ctest --test-dir "<Baum>/re15_port/build" -R "^integration_r41_(gatorfix_|re2gator)" -j 2 --timeout 1800 -V` (`ctest_exe.txt`):
  **`100% tests passed, 0 tests failed out of 20`**, `Total Test time (real) = 4438.12 sec` (00:09:36-01:23:34, parallel zu meinen exe-Laeufen und fremden
  Agenten). Einzelzeiten: abn2_wand_elza 673,64 s, frei_elza 674,81, abn2_wand 637,20, frei 651,42, abn2_leiterecke 622,90, abn2_rampe_west 614,87,
  abn2_nordbecken 565,41, abn2_ecken 583,73, oben_kante_elza 526,92, abn2_rampe_west_elza 677,05, oben_kante 466,98, rampe_oben_elza 412,30, rampe_oben 371,77,
  rampe_elza 369,95, gang_montage_elza 52,97, gang_szene_elza 47,23, gang_montage 32,85, gang_szene 27,36, rampe 327,96, re2gator 299,70 (alle Passed).
  0 Zeilen "VERFEHLT"; gtrlog 32x "Treffer waehrend +0x238 > 0 0 -> ok"; `r41_re2gator: ok` (nische Maul-Sitz 240 Bilder, Finisher Cut 6). Kein roter Riegel,
  nichts einzeln nachzufahren.
- Die Riegel sind gruen, obwohl M1 besteht: nb4 Teil B und gtrlog pruefen "kein Treffer, solange +0x238 > 0" - genau die Regel, die RE2 nicht hat. Ein
  Riegel fuer "kein Biss-Anlauf mit Maul an der Figur ohne Biss" fehlt; der re2gator-Lauf nische wurde auf den spaeteren Biss F416 umgestellt (cf9a2ac3f),
  statt den Anlauf ohne Biss F284-F389 zu beanstanden.

## 3. Eigene RE2-Disassembly (`disasm/`)

Quelle `C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN` (18 664 B, md5 0293e099bbeae989776264eb14c81bc9, Datei-Offset = Adresse - 0x80100000),
eigenes Werkzeug `werkzeug/dis8.py` (capstone) und `suche8.py` (Muster- und Datenwort-Suche); RE2-EXE aus `ghidra_re2_Leon.txt`.

### 3a Stichprobe der zitierten Adressen - Bytes bestaetigt (`dis8_stichprobe.txt`)

| Zitat des Baus | Bytes / Befehl | Bewertung |
|---|---|---|
| 70 im Treffer @0x801016b0-b4 | `46000224 addiu v0,zero,0x46` / `380282a0 sb v0,0x238(a0)` | Bytes richtig |
| je Bild -1 @0x801004a8-b8 | `lbu v0,0x238(s0)` / `beqz` / `addiu v0,v0,-1` / `sb v0,0x238(s0)` | Bytes richtig |
| @0x80100b5c / b64 | `lbu v0,0x238(s0)` / `bnez v0,0x80100c08` - in Sub 1 kein Wechsel nach Sub 2 (0x201) | Bytes richtig; sperrt den Spurt, nicht den Biss |
| @0x80100bf4-c04 | `lbu` / `beqz` / `addiu v0,zero,0x101` / `sw v0,4(s0)` - Sub 2 zurueck nach Sub 1 | Bytes richtig; dito |
| @0x80100cb4-c4 | `lbu` / `beqz` / `addiu 0x101` / `sw 4(s0)` in Funktion 0x80100c54 | Bytes richtig, aber **toter Code** (keine Tafel, kein jal, kein Datenwort; `dis8_verweise_80100c54.txt`) |
| `jal 0x80101578` @0x8010155c | `5e05040c` | Bytes richtig |
| 110 nach dem Intro @0x8010091c-20 | `addiu v0,zero,0x6e` / `sb v0,0x238(s0)` | Bytes richtig; **im selben Schritt** Biss-Bremse 0x800cfbd8 & ~0x10 (@0x801008f8-918) |
| FUN_8001569c(.., 0x600) @0x80101684-88 | `jal 0x8001569c` / `addiu a2,zero,0x600` | Bytes richtig |

### 3b Was +0x238 in RE2 sperrt - und was nicht (M1)

- Dispatch (`dis8_routine1_80100764.txt`, `dis8_tafeln_80104600.txt`): Routine 1 0x80100764 ruft je Bild die Vorstufe aus Tafel @0x8010463c (+5 = Sub;
  [1] = [2] = 0x80100964, [3] = 0x80100d28 `jr ra`) und danach die Hauptstufe aus Tafel @0x8010465c ([1] 0x80100c20 Clip 0 Blend 32, [2] 0x80100cdc Clip 1
  Blend 4, [3] 0x80100d30 Clips 2/3). 0x80100c54 steht in keiner Tafel.
- Biss (`dis8_t1_bissmaschine.txt`, `dis8_bissmaschine_stufen.txt`, `dis8_treffer_einstieg_80101578.txt`, `dis8_t1_weitere_aufrufer.txt`): T1 0x801012fc wie
  im Urteil; Aufrufe `jal 0x801012fc` @0x80100974 (erste Anweisung der Sub-1/2-Vorstufe), @0x80100d40 (Sub 3), @0x8010125c (Sub 7), @0x80100c60 (toter Code).
  Die Kiefer-Maschine 0x80101388 laeuft jedes Bild aus dem Root (`jal 0x80101388` @0x80100328, `dis8_root_bissmaschine_t1.txt`): Stufe 1 setzt 2 und faellt in
  Stufe 2 (Zaehler +0x158 bis 4), Stufe 3 (bis 4), Stufe 4/5 rufen den Treffer-Einstieg je Bild (3 + 3), danach +0x224 = 0 @0x80101558. Der Treffer-Einstieg
  fragt Kontakt `lhu v0,0(a1=0x800cfc06) / beqz` @0x80101590-98 und `lbu v0,-0x235(v0=0x800cfdcb) / bnez` @0x801015a0-ac - +0x238 wird dort nur gesetzt.
- Kontakt (`re2exe_FUN_800355c4_kontaktbits.txt`): RE2-EXE FUN_800355c4 (Aufruf @0x80026628, a0 = s3 + 0x3a10 = Spieler 0x800cfbf8) setzt je beruehrendem
  Gegner ein Bit (FUN_80034d0c) und schreibt sie `sh s3,0xe(s1)` @0x80035658 = 0x800cfc06. Biss in RE2 = Kiefer schnappt bei Koerperkontakt.
- Reaktionssperre (`dis8_reaktionssperre_1d3.txt`): Sub 2 setzt PL+0x1D3 |= 0x80 @0x801030e8-80103100, loescht sie im Ende-Bild @0x801031d4; Sub 3 loescht sie
  zu Beginn des Liegens @0x801032f0.
- Intro-Ende (`dis8_intro_ende_801008e0.txt`): `addiu v1,zero,-0x11` @0x801008f8, `lw/and` 0x800cfbd8 @0x801008fc-90c, `addiu v1,zero,-0x101 / and / sw`
  @0x80100910-918, dann `addiu v0,zero,0x6e / sb v0,0x238(s0)` @0x8010091c-20 und `sw 0x101,4(s0)` @0x80100928-2c (Sub 1). Bit 0x10 ist genau die Bremse,
  die T1 @0x80101310-20 fragt: RE2 gibt mit dem Intro-Ende den Biss frei und sperrt nur den Spurt fuer 110 Bilder.
- Ergebnis: In RE2 gilt "ein Treffer je Kiefer-Lauf" und "kein Treffer waehrend der Reaktion"; ein zweiter Treffer ist bei andauerndem Koerperkontakt rund
  28/30 Bilder nach dem ersten moeglich. Der Runde-1-Zweitbiss 20/22 Bilder danach im selben Fenster war damit tatsaechlich nicht RE2 (die Kiefer-Latenz fehlte),
  die Runde-2-Regel "kein Treffer vor 70 Bildern" ist es aber ebenso wenig - sie ist strenger als RE2 und sperrt zusaetzlich die Bisse, die RE2 nach dem Intro
  zulaesst.

## 4. Eigene Messung (`messung_abnahme8.txt`)

### 4a Abnahme-7-M1 (Zweitbiss) - behoben

| Lauf (Figur) | Treffer | kleinster Abstand | unter 70 | Zweitbiss gleicher Clip-Lauf | gesperrte Biss-Bilder nach dem Treffer (frueher Zweitbiss) |
|---|---|---|---|---|---|
| tod_leon / tod_elza (echte HP) | 1 + TOT | 107 | 0 | 0 | je 1 Lauf / 9 Bilder (F962-F970) |
| oben_leon / oben_elza | 35 / 33 | 107 | 0 | 0 | 32 / 30 Laeufe |
| frei4_leon / frei4_elza | 68 / 75 | 107 | 0 | 0 | 53 / 72 Laeufe |
| west4_leon / west4_elza / west5_elza | 28 / 30 / 25 | 107 | 0 | 0 | 22 / 30 Laeufe / (west5 nicht ausgerichtet, Hochbiss-Fenster) |
| ohne_frei4_leon / ohne_oben_elza | 68 / 33 | 107 | 0 | 0 | 39 / 30 Laeufe |
| (K)-Laeufe k_leon_w4..w8, k_elza_w4..w8, k_*_eingabe_* (10) und aufn_elza | 51..74 je Lauf, 8 | 107 | 0 | 0 | 32..73 je Lauf, 6 |
| Spawn oben (4) | 20..33 | 105 (ost_leon) | 0 | 0 | (3 von 4 nicht ausgerichtet) |
| **Summe 27 Laeufe** | **1164** (Sub 2 356, Sub 3 805, 3 TOT) | **105** | **0** | **0** | 847 Clip-4-Laeufe / 6220 Bilder (23 ausgerichtete Laeufe) |

Lesart: In 847 Biss-Anlaeufen war die Bissbedingung nach dem Treffer (Maul an der Figur, keine Wand, Reaktion zu Ende) noch erfuellt - in Runde 1 waere dort der
Zweitbiss gekommen, jetzt nicht. Bild: `leisten/a8_ohne_oben_elza_kein_zweitbiss_F752-F800_hell.jpg` (F756 Biss, F758-F774 Taumeln, F776-F784 Schnauze wieder an
Elza, Kiefer zu, kein Biss), `a8_tod_leon_ein_biss_F938-F984_hell.jpg`, `a8_tod_elza_ein_biss_F938-F984_hell.jpg`.

### 4b Folge der Sperre: Anlaeufe ohne Biss / verschobene Erstbisse (M1)

- nische (`a8_nische2`, Umgebung wortgleich integration_r41_re2gator Lauf nische, EXIT_AT 780): Intro-Ende F280 (+0x238 = 110 bis F390). Anlauf 1 F284-F326:
  Bissbedingung ohne Sperre F311-F318 (maul 1328); Anlauf 2 F347-F389: Bissbedingung ohne Sperre in allen 29 Fensterbildern F353-F381 (maul 864, dist 3444) -
  beide ohne Biss. Erster Biss F416 (hp 60 -> 0, Sub 2), toedlicher Biss F523 (Abstand 107), Finisher. Runde 1 (Dossier NB4 Runde 2 und cmake-Kommentar
  cf9a2ac3f, Lauf nische): erster Biss F290. Leisten `a8_nische_leerangriffe_intro_sperre_F286-F384_hell.jpg` (F350-F384: der Kopf liegt an Leons Beinen,
  nichts passiert) und `a8_nische_erster_biss_F410-F440_hell.jpg`.
- Wiedereintritt (`a8_west5_elza` gegen `a7_west5_elza`): bis F91 bytegleich; Runde 1 beisst F92 (Clip 4 Bild 7), Runde 2 erst F113 (Bild 28) im selben Anlauf.
  Der einzige Code-Unterschied ist gb_biss_frei - +0x238 war im dritten Besuch bis rund F110 gesetzt (Intro-Ende des ersten Besuchs F282 mit 110, Raumwechsel
  F284; ob der Zustand weiterlief oder neu gesetzt wurde, ist aus den Logs nicht zu trennen).
- In den Laeufen ohne Intro-Naehe (Szene, Rundweg, Rampe) kein Anlauf ohne Biss mit Maul an der Figur (a8biss "0 Laeufe" in allen 22 uebrigen ausgerichteten
  Laeufen). Nach einem Treffer deckt hit_stun 100 > 70 den Anlauf ab - die Folge betrifft das Intro- bzw. Wiedereintritts-Fenster. In spawn_ost_leon beisst
  ein Hochbiss F390 genau mit dem Ablauf der Intro-Sperre (F280 + 110), Bild 10 seines Fensters.

### 4c (T) Reaktion, Weg, Dauer, Animation

- Alle vollstaendigen Reaktionen: Sub 2 = 20 Bilder (Clips 18 -> 19), Sub 3 = 54 Bilder (Clips 20 -> 17 -> 11 rueckwaerts); abweichende Dauern nur am
  Laufende (abgeschnitten) und B1 (Treffer beim Klettern, 67 Bilder). gtrlog in allen 29 Laeufen "Abweichungen 0 (Unterzustand 0, Dauer 0, Schritte 0,
  Haken-Weg 0, Sturz 0)", "RE1.5-Niederwurf-Bilder 0". Schaden 30 in den Laeufen mit GATOR_HP-Messhaken (geschwaecht), 60 mit voller Gator-HP (tod, Spawn,
  nische). Angriffsarten in den 27 exe-Laeufen (GTR START): Biss 1144 (+3 toedlich), Rammen 8, Hochbiss 7, CROSS-Biss 2; im Harness alle vier (Rammen 55,
  CROSS 22, Hochbiss 11, Biss 517).
- Weg: Haken je Reaktion <= 3735 (Sub 2) / 3080 (Sub 3), mit Sturz 4021; Luftlinie mit Koerper-/Boss-Schub bis 5052 (frei4_leon / ohne_frei4_leon),
  Summe der Bildschritte bis 11 112 in einer Sub-3-Reaktion (k_elza_eingabe_leon). Ursache wie in Abnahme 7: der drehende Gator-Koerper schiebt die liegende
  Figur (frei4_elza F904-F915 je rund 300 Einheiten bei Haken-Schritt 0, Gator steht und dreht 80 je Bild). Gegen Runde 1 (gleiche Eingaben, `a8schub.py`):
  Reaktionen mit Schub-Weg > 1500 zusammen 23 statt 31 - keine Verschlechterung, offen wie in Abnahme 7.

### 4d (W) Kante, Wand, Raum

| Lauf | Treffer | Kante | Landung | y-Folge |
|---|---|---|---|---|
| oben_leon / oben_elza / ohne_oben_elza | F383 Rammen, Sub 2 | F385 Nordkante, Band 1 -> 0 | F389 (x,-12797) | -1210/-1070/-880/-640/0 |
| west5_elza | F1551 Rammen, Sub 2 | F1553 Nordkante | F1557 (3482,-12797) | dito |
| spawn_nord_elza / spawn_sued_leon / spawn_ost_leon / spawn_mitte_elza | F732 / F390 / F905 / F796 Rammen, Sub 3 | Suedkante F733 / F391 / F906 / F797 | (x,-20568) F737 / F395 / F910 / F801 | dito |

Leisten `a8_oben_leon_rammen_kante_F376-F418_hell.jpg` / `a8_oben_elza_...`: F384 Taumeln an der Kante, F386/F388 im Fall, F390 im Wasser, F392/F394 Spritzer,
danach steht die Figur im Nordbecken. Alle 8 Stuerze kamen vom Rammen. Die 7 uebrigen Treffer auf dem Deck (alle 7 Hochbisse; die 2 CROSS-Bisse trafen im
Wasser) schoben die Figur nur ueber das Deck: ein Sturz kommt erst, wenn ein einzelner Hakenschritt vom Deckrand aus das Abwaerts-Feld erreicht (gemessen: Schritt 800 / 650 ja - spawn_ost F906,
west5_elza F1553; 560 / 440 / 320 / 185 nein - spawn_ost F603-F607 bis z -18072, west5_elza F1251-F1263), sonst haelt der Raumhalt Band 1 die Figur am Rand
(-17632 bzw. -14968). aussenlog in 26 Laeufen "aussen 0, Deck verlassen 0, im Block 0"; in tod_leon/tod_elza/nische2 zaehlt er nur
die Finisher-Bilder nach dem Tod (Figur im Maul bei y -1200 ab F1051 bzw. F525) - kein Raumfehler.

### 4e (K) Kanister

| Lauf (Eingabe) | Stufe 1 | laengs / quer | Gator-Clip | Bemerkung |
|---|---|---|---|---|
| frei4_leon (= ohne_frei4_leon) | F2851 (95 s) | 5920 / 1451 | Verfolgung | wie Bauer |
| frei4_elza | keine (280 s) | - | - | Gator-Wurzel nie unter 2954 am Kanister |
| k_leon_w4 / w5 / w7 / w8 (Leon-Eingabe, Start 4/5/7/8 s) | keine / F1590 / F1238 / F185 | - / 5591, 1021 / 5482, 1515 / 5476, -1706 | Verfolgung | |
| k_elza_w4 / w5 / w7 / w8 (Elza-Eingabe) | F1480 / F5167 / keine / F197 | 5830, 1144 / 5070, -1439 / - / 5429, -1719 | Verfolgung | w7: Gator quert im Dash (Clip 2/3) 794 am Kanister vorbei - RE2 prueft das Fenster nur in der Sub-1/2-Vorstufe (Sub 3 Vorstufe 0x80100d28 = `jr ra`) |
| k_leon_eingabe_elza / k_elza_eingabe_leon (= aufn_elza) | F1656 / F1209 | 5407, 1628 / 2396, 93 | Verfolgung | |
| west5_elza | F2669 (89 s) | 5470 / 1536 | Verfolgung | |
| oben_leon / oben_elza / west4_leon / west4_elza | keine | - | - | Figur passiv im Nordbecken bzw. an der Rampe |

Summe: 10 von 17 Laeufen mit liegendem Kanister nehmen ihn im Vorbeikommen (6-172 s, Median rund 50 s), 7 lassen ihn liegen; 0 Aufnahmen ausserhalb des
Fensters, alle aus der Verfolgung (Gator-Clip 0/1 im Bild davor), 0 RUNDE / ANSCHWIMMEN / Zielmodus 1; Kanister-Sitz in allen 11 Aufnahme-Laeufen (mit
aufn_elza) "Abweichung max 0" ueber 180-8190 angehaengte Bilder, 0 Rueckfall-Zeilen. Bahnen `bahnen/bahn_a8_*.png`. Leisten der Aufnahme
`a8_frei4_leon_kanister_vorbeikommen_F2832-F2890_hell.jpg`, `a8_aufn_elza_kanister_vorbeikommen_F1192-F1250_hell.jpg` (Kamera Cut 0, die Szene ist klein am Rand;
Beleg sind die Logzeilen).

### 4f Vorgabe A, Vorgabe B, Gang

- Vorgabe A (obenlog): oben_leon / oben_elza / ohne_oben_elza erster Treffer oben nach 300 Bildern (Rammen F383), laengste Pause 300; west5_elza 8 (Hochbiss
  F1126) / 306; Spawn nord_elza 119 / 333, sued_leon 110 / 110, ost_leon 110 / 305, mitte_elza 213 / 303 - alle <= 600.
- Vorgabe B (haenglog mit): 20 Laeufe, Haenge-Fenster 0, ohne Fortschritt max 295 Bilder (k_leon_w8), 0 Zeilen HAENGT / RUECKWAERTS / TAUCHEN.
- Gang ohne Kanister: unit gang/gang_elza gruen (in den 48); ohne_frei4_leon / ohne_oben_elza gegen die Abnahme-6-Laeufe (Bau 5910fc24, Stand 3d57083b1)
  bytegleich bis zum ersten Treffer F139 / F383 (dort RE2- statt RE1.5-Reaktion), gegen Abnahme 7 bis zum frueheren Zweitbiss F579 / F776.

## 5. Weitere Beobachtungen (nicht blockierend)

- **B1 Treffer beim Hinaufklettern** (west5_elza F1126, Hochbiss): Elza klettert an der Suedost-Ecke auf das Deck (climb sub 9, Clip 231). Der Treffer setzt
  sofort die RE2-Gier (3072 -> 2048), die Kletter-Animation laeuft 13 Bilder seitlich weiter, die Reaktion (Sub 3) beginnt erst F1140 - Dauer 67 statt 54.
  Im Bild dreht sie sich mitten im Hochziehen um 90 Grad (`leisten/a8_west5_elza_hochbiss_beim_klettern_F1118-F1156_hell.jpg`, Ausschnitt F1124-F1132).
  Selten (1 von 7 Hochbissen in 27 Laeufen), RE2 kennt im Raum kein Klettern; bisher in keinem Riegel.
- **B2 Schub** (4c): wie Abnahme 7 offen; Gesamtweg je Reaktion bis 11 112, Luftlinie bis 5052.
- **B3 Nebenbeobachtung Kamera/Spritzer**: in den Spawn-Laeufen bleibt die Kamera auf Cut 0, die Landung im Suedbecken liegt nahe der Kamera - der Spritzer wird
  als grosser Pixelblock am Bildrand gezeichnet (`leisten/a8_spawn_sued_leon_F396_spritzer_an_der_kamera.png`). Folge des Testaufbaus (Spawn ohne Zonenwechsel),
  nicht bewertet.
- **fuzz-Riegel (3bd9e1b05)**: die Trag-Lenkung, die er prueft, gilt nur mit dem Pruefhaken v4_alt (`gr2_kan_zaehlt` = `s_v4_alt && ...`, gator_re2_r41.c :147;
  im Spiel nie gesetzt). Ihn auch mit der Trefferreaktion jenes Stands zu fahren ist schluessig; die Spiel-Reaktion pruefen nb4/raum/nb3/re2/v4 und meine
  exe-Laeufe (haenglog). Vertretbar.

## 6. Die offenen Punkte des Bauers - Einstufung

1. A90 (O-T1): weiter PORT-WAHL ohne Nutzer-Wortlaut (Frage F1). Folge gemessen (4d): jedes Rammen oben stiess die Figur ins Wasser (8 von 8), die Hochbisse
   schoben sie nur ueber das Deck (7 von 7).
2. fuzz: vertretbar (Abschnitt 5).
3. "RE2 bricht den Angriffs-Unterzustand nach dem Treffer sofort ab (@0x80100cc4)": beruht auf dem toten Code 0x80100c54. Belegt ist nur Sub 2 -> Sub 1
   @0x80100bf4-c04 (Spurt endet); der Port-Anlauf laeuft bis Bild 42 aus - im Bild Schnauze an der Figur bei geschlossenem Kiefer, unkritisch.
4. Elzas Aufnahme im Anlauf: nachgemessen, alle Aufnahmen im Fenster und in der Verfolgung (4e).
5. Intro-Sperre: **ist M1** - nicht "bewusst nicht umgebaut", sondern Folge einer Regel, die RE2 nicht hat (Abschnitt 3b).
6. Sturz-Animation (O-W1): im Bild unauffaellig (4d), Sichtpruefung durch den Nutzer (Frage F2); Schub B2; B2-Biss nur per env; PSX nicht gebaut.
7. Attributionszeile: Commits enden mit "Co-Authored-By: Claude Opus 5.5" - Vorgabe des Orchestrators fuer diese Abnahme ebenso.

## 7. Konstanten im Diff 47ab6f1fa..9c3659bf7 (selbst nachgelesen)

| Zahl / Stelle | Beleg | Bewertung |
|---|---|---|
| RE15_GR2_BISS_CD_BISS 70 in gb_biss_frei (LUNGE / B2 / Rammen) | Wert @0x801016b0-b4 richtig; Verwendung als Bissbedingung: RE2 fragt +0x238 nur fuer Sub 1/2 (@0x80100b5c-64, @0x80100bf4-c04), nie im Biss (T1 @0x801012fc, Einstieg @0x80101578); Zitat @0x80100cb4-c4 = toter Code | **M1** |
| 110 (Intro, unveraendert) als Biss-Sperre | RE2 gibt am Intro-Ende den Biss frei (@0x801008f8-918) und sperrt nur den Spurt | **M1** |
| EXIT_AT 560 -> 770 (test_r41_re2gator.cmake) | gemessen (Tod F523, Finisher bis F784); bestaetigt mit eigenem Lauf (a8_nische2 TOT F523) | belegt |
| Riegel: 70 als Abstand, `S.paare >= 50`, Gegenprobe-Lagen (4200,-18700) / (6732,-18598) | Test-Schwellen und -Lagen | 70 faellt mit M1; Rest Test-Aufbau |

Sonst keine neue Zahl.

## 8. Fragen an den Nutzer (Sie-Form)

- F1: "Wenn der Alligator Ihre Figur oben auf der Rampe rammt, stoesst er sie jetzt ins Wasser (wie gewuenscht) - in meinen Laeufen jedes Mal. Beisst er von
  unten hinauf, taumelt die Figur dagegen nur ueber die Rampe und bleibt oben. Ist diese Stossrichtung fuer Sie in Ordnung?" (Resident Evil 2 hat im
  Alligator-Raum keine Rampe; die Richtung ist eine Port-Wahl ohne Ihren Wortlaut.)
- F2: "Waehrend des Sturzes von der Rampe taumelt die Figur in der Resident-Evil-2-Haltung weiter (Bildleisten a8_oben_leon/elza_rammen_kante). Ist dieser
  Anblick fuer Sie in Ordnung?"

## 9. Tests (Summenzeilen woertlich)

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
- `ctest -R "^unit_r41_(gatorfix|re2gator)_" -j 2 --timeout 1800`: `100% tests passed, 0 tests failed out of 48`, `Total Test time (real) =  96.38 sec`.
- `test_r41_gatorfix_g2abn8.exe nb4` / `nb4_elza`: `nb4: 0 Fehler` / `nb4_elza: 0 Fehler`, exit 0.
- exe-Riegel `ctest -R "^integration_r41_(gatorfix_|re2gator)" -j 2 --timeout 1800 -V`: `100% tests passed, 0 tests failed out of 20`,
  `Total Test time (real) = 4438.12 sec` (`ctest_exe.txt`).
- 29 eigene exe-Laeufe, alle `exit=0` bis RE15_EXIT_AT (`messung_abnahme8.txt` Abschnitt 1); 1 verworfener Lauf (a8_nische, EXIT_AT hinter dem Game Over).

## 10. Nicht geprueft

PSX-Bau; der B2-Biss (env RE15_GB_LUNGE_B2); RE2 dynamisch (wie oft der Kontakt nach einem Treffer in RE2 tatsaechlich anhaelt - nur die Regel ist statisch
belegt); Raumwechsel mitten im Kampf ausser dem Wiedereintritt der Szenen-Laeufe; Hoeren (SE nur im Log); Kamera-Wahl in den Spawn-Laeufen.
