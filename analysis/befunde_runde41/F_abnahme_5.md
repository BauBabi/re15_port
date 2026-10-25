# Runde 41 — Spur F "gatorszene": Abnahme 5 (unabhaengig)

Stand: Zweig `r41/gatorszene`, Code-Stand `f83f619b` (letzte Code-Aenderung `45096984`; Basis r41/integration `9f10bdb0`,
Abnahme 4 auf `4b875538`). Waehrend dieser Abnahme legte der Orchestrator `b32b6507` (nur AUFTRAG.md: **Nachtrag 31**, 17:55 —
Nutzer-Antworten auf die Fragen der Nachbesserung 5) in den Zweig, vier Minuten nach dem Abschluss-Commit des Bau-Agenten
(f83f619b, 17:51). AUFTRAG.md ist massgeblich, Nachtrag 31 gilt also fuer diese Abnahme (wie F-E2 fuer Abnahme 4). Baum
`.claude/worktrees/r41_gatorszene`. Pruefer: Abnahme 5 (2026-10-25). Ich glaube dem Bau-Agenten nichts, was ich nicht selbst am
gebauten Stand gemessen habe.

Nachtrag 31 (Wortlaut-Auszug AUFTRAG.md): (1) kurzer Durchbruch "So lassen" (PORT-WAHL angenommen); (2) Kampfbeginn **"Ändern: nicht
vor der Kamera"**: "Startlage so waehlen, dass er beim Kampfbeginn im Becken VOR Leon zu sehen ist und nicht direkt vor der Linse der
laufenden Kamera (Cut 0); der RE2-Kampfbeginn (F-E2 ...) bleibt. Mass: Abstand der Gator-Teile zur Kamera / Bildanteil je Bild ueber
die ersten ~150 Bilder (Riegel), Bildbogen Leon + Elza."; (3) Schluesselgroesse "Ja, gut genug" - "F-E2 Punkt 2 gilt fuer die Groesse
damit als erfuellt (Helligkeit bleibt Pflicht)".

## 0. Bau und Lektuere

- Gelesen: AUFTRAG.md (Zweig, bis Nachtrag 31; master hat fuer F nichts Neueres als F-E2 14:21), VERTRAG.md, Dossier F_gatorszene.md
  (Schwerpunkt "Nachbesserung 5"), F_abnahme_4.md, `git log master..HEAD` (seit Abnahme 4: bf4220d3 .. f83f619b, 9 Commits, dazu
  b32b6507), `git diff 4b875538..f83f619b -- re15_port` WIRKLICH gelesen: Code nur `engine/src/gatorszene_r41.c` (+159/-31) und
  `include/re15_gatorszene_r41.h` (+22/-2): Lage als Laufzeit-Struktur `re15_gsz_lage_t` (Standard = PORT-WAHL: obj 2 bei (0,-26000)
  Gier 2944, Anlauf-Gier 1952, Durchbruch bis Clip-6-Bild 22, Schluessel lokal (1090,-420,-100) rx -48 / rz -608, Ankunfts-Cut 0;
  Setzer `re15_gatorszene_r41_lage_setzen` nur fuer Sonden), Messhaken Fress-Einstellung / Kampfbeginn / Kontakt (Logzeilen; der
  Kontakt-Haken nur `#ifdef RE15_PLATFORM_PC`), Abbruch des Durchbruchs `anim_frame >= ga_durchbruch_bis`; Tests test_r41_gatorszene.c
  (+185/-44), test_r41_gatorszene.cmake (+75/-21), probes/r41_gatorszene.cmake (+6); Werkzeuge leiche_lage_n5.py (+265),
  probe_clip6_n5.c (+64), probe_re2_band_n5.c (+165); Dossier, 3 Belegbilder. `git diff master` (zwei Punkte) zeigt zusaetzlich
  Loeschungen von Dateien, die master NACH der Basis 9f10bdb0 bekam - keine Aenderungen von F; massgeblich ist `git diff 9f10bdb0..HEAD`
  (67 Dateien).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.` /
  `=== LOCAL-BUILD-OK (build)`. gatorszene_r41.c 17:42:01 < re15_pc.exe 17:49:18; f83f619b und b32b6507 aendern keinen Code -> die exe
  ist der Code-Stand HEAD (die Laeufe tragen die neue Buehnen-Zeile `Leiche (-212,-24931) ... S (5655,-24390) Gier 1952`).

Messordner (scratchpad, nicht im Repo):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\F41a5`
(`runs/<lauf>/debug.log|state.log|gator_boss.log|fd_*.ppm`; Helfer `tools/lauf.sh` = eigene exe-Kopie `re15_pc_f41a5_<lauf>.exe`,
`timeout`, danach geloescht; eigene Auswerter `tools/kampf_a5.py` (Kampfbeginn = letzte `Folge C`, letzter Raum-Abschnitt von
state.log, Abstand, erster Clip 4, erster HP-Abfall, Clip-Wechsel, Abstand Gator-Wurzel <-> Kamera Cut 0), `tools/anteil.py`
(Vordergrund-Anteil je Bild gegen den Median-Hintergrund derselben Kamera), `tools/ring_a5.py` (Schluesselring: graue Pixel wie
Abnahme 4, Komponenten, Ruecken der Leiche abgetrennt), `tools/sheet.py` / `tools/zoom.py` (Bildboegen)). Alle Laeufe
`RE15_WINDOW_SCALE=1`, Bilder nur per `RE15_FRAMEDUMP`.

## 1. Gezielte Tests

Regex = die Regex der Abnahme 4 / des Bau-Agenten (`F41a5/regex.txt` = `F41a4/regex.txt` = `F41n5/regex.txt`, `ctest -N -R` -> **Total Tests:
221**). Sie deckt alle Testdateien, die gatorszene/RE15_GSZ_ nennen (`git grep -l` ueber re15_port/tests: test_r37_itemband.cmake,
test_r38_wasser.cmake, test_r41_gatorszene.cmake, test_r41_re2gator.cmake, test_r41_wasser20b0_npc.cmake, probe_r26_inventar.c,
probe_r36_tueren.c, probe_sca_wall_hit.c, probes/r41_gatorszene.cmake, test_r30_tuer.c, test_r36_kartenbits.c, test_r38_spinnen.c,
test_r41_gatorszene.c), die 2090-Laeufe anderer Spuren (integration_r35_granate, integration_r36_kartenitems_bild, integration_r37_feuerloescher,
integration_r36_tueren) und Spur B's Riegel (unit_r41_re2gator_*, integration_r41_re2gator). Befehl mit dem PATH von local_build.sh
(`F41a5/tools/ctest_lauf.sh`): `ctest --test-dir "<baum>/re15_port/build" -R "$(cat F41a5/regex.txt)" -j3 --timeout 1200 --output-on-failure`
Ergebnis (parallel zu eigenen exe-Laeufen): **`100% tests passed, 0 tests failed out of 221`** (`Total Test time (real) = 1546.50 sec`,
Log `F41a5/ctest_all.log`). Darin integration_r37_itemband 813.75 s, integration_r36_kartenitems_bild 431.20 s, integration_r41_re2gator
288.35 s, integration_r38_rolltor20a0 258.81 s, integration_r41_wasser20b0_npc 221.03 s, integration_r38_wasser 183.02 s,
integration_r41_gatorszene_leon/_elza 56.12/72.19 s, _flucht/_flucht_elza 56.09/68.45 s, _tod/_tod_elza 67.65/134.56 s, _tod_r1/_tod_r1_elza
44.79/96.23 s, _tod_explosion/_elza 28.93/47.58 s, unit_r41_gatorszene_abstand/_elza 0.85/0.83 s. Kein roter Fenster-Haken, nichts
nachzufahren. Die Behauptung des Bau-Agenten (221/221) ist bestaetigt. (Die vom Lauf integration_r39_inventar_discard liegen gelassene
exe-Kopie `re15_pc_r39c_discard_f28189fa.exe`, 18:36:58, habe ich danach geloescht - Testdatei der Runde 39, nicht F.)

Messen die Riegel? Siehe Abschnitt 7 (Gegenprobe mit den wortgleich herausgeschnittenen Funktionen auf alten und neuen Laeufen) und die
Direktlaeufe `test_r41_gatorszene re2soll` -> `re2soll: abstand=11402 abstand2=130647809 bilder=121 abstand_max=13890 abstand2_max=193864141
bilder_max=158`, `test_r41_gatorszene abstand` -> `abstand: 0 Fehler` (11448 im Band, Zone 0/0, Biss 131 im Band, Gegenproben 48 < 121 und
270 > 158), `test_r41_gatorszene leiche` -> `leiche: 0 Fehler` (Primitivfarbe Cut 0 Ruhe 85 -> Spitze 126 >= 84/125). Fuer Nachtrag 31
Punkt 2 gibt es (noch) keinen Riegel.

## 2. Punkt 1a — Ankunftsszene (Leiter 2080 -> 2090; Wortlaut + Nachtraege 6/17/20/21/29 + F-E2 + Nachtrag 31)

Weg: `RE15_DEBUG_JUMP=2080@250` + `RE15_FIRE_AOT=1@40#2080|2081` = die Leiter-Tuer ROOM2080 Slot 1 ueber `re15_aot_fire_slot`
(main.c: "exakt der Pfad, den auch das Hineinlaufen nimmt (Scan -> aot_fire_door)"), `RE15_SET_FLAG=3:245` (Original-Rechner).

### 2.1 Szenenfolge (Leon L1, Elza E1)
- L1: `tools/lauf.sh L1 300 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore SDL_AUDIODRIVER=dummy RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080
  RE15_EXIT_AT=650#2090 RE15_FRAMEDUMP=1-650/1:fd_` -> rc=0, 49 s. debug.log: `[aot] DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,hh=500)
  target_cut=0 spawn=(-8200,0,-25550)`, je 2090-Besuch `Spinne 0x25 bei (-100,-8296)|(-2300,-23896) verworfen`, `ROOM2090 Slot 1 (Tor)
  gesperrt: Ereignis 19, Bit (9,113) = 0`, `Folge A ROOM2090 F40` -> `P 1` -> `T F98` -> `DOOR FIRE slot=20 ... target_cut=2
  spawn=(-9950,-14400,-25700)` -> `P 2 ROOM2080 F142` + `M ROOM2080 F142 wert=8/2` -> `DOOR FIRE slot=20 ... target_cut=0
  spawn=(-8200,0,-25550)` -> `P 3 F156` -> `S F53` -> `Buehne: Leiche (-212,-24931) Kopf13-Versatz (-3016,-489) Weg4 (-1314,-193)
  Weg6 (-1537,141) -> A (4118,-24249) S (5655,-24390) Gier 1952` -> `G 1 F53` -> `Fress-Einstellung: Cut 1 frontal 4116 (Kopf
  (1064,-24692), Achse (-1517,-223))` -> `G 2 F74` -> `K 13 F87` -> `G 3 F88` -> `G 4 F208` -> `V F267` -> `G 5 F267` -> `G 6 F268` ->
  `C ROOM2090 F297` + `Kamera-Uebergabe: Cut 0 (Eintritts-Cut), Spielfigur im Bild (Fuss-Bildpunkt 88,127)` + `Kampfbeginn F297: Gator
  (3197,-24407) Gier 1981 Zone 0, Spielfigur (-8200,-25550) Zone 0, Abstand 11448` -> `G 7 F297` -> `B 1 F299` + `[bgm] stage=1 room=09
  entry=FF3C -> MAIN3C` + `[bgm] loaded: 1 VAGs, SEQ 100B` (= RE2 MAIN1A) -> `Kontakt F507: +210`. Vor der Szene `[bgm] ... MAIN2E`
  (Raummusik 2090).
- E1 (`RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, 2081/2091): `[pl] Spieler-Familie PL04
  (character=4, Elza-Bit=1)`, dieselbe Folge mit denselben Bildnummern (`Folge C ROOM2091 F297`, `Kampfbeginn F297 ... Abstand 11448`,
  `Kontakt F507`), rc=0, 64 s.
- Bilder: `F41a5/L1_szene_fressen.png` (F10/F40 Cut 0: Leon an der Leiter, die Leiche treibt vorn mit dem Ring ueber dem Ruecken;
  F55-F290 Cut 1: der Gator steigt rechts aus dem Wasser (F62/F68 Kopf senkrecht), beisst zu (F74-F87), schuettelt mit der Leiche im
  Maul (F99/F130/F160/F201/F240 Leiche mit haengenden Gliedern zu sehen), schluckt (F270), dreht sich zur Kamera/zu Leon und schreit
  (F290)).
- Die Szenenfolge des Wortlauts und der Nachtraege 6/17/20/21 ist erhalten (Leon + Elza).

### 2.2 Kampfbeginn (Nachtrag 29; F-E2 Punkt 1) - Leon/Elza, stehend und sofort weglaufend
Auswertung `tools/kampf_a5.py` (eigener Auswerter, Abstand = hypot der Wurzel-Lagen aus state.log):

| Lauf | Eingabe | Kampfbeginn (Folge C) | Spieler | Gator-Wurzel (Gier) | Zonen (Logzeile) | Abstand | erster Biss (Clip 4) | erster Treffer | Clip-Folge danach |
|---|---|---|---|---|---|---|---|---|---|
| **L1** Leon | stehend | F297, cam 0 | (-8200,-25550) | (3197,-24407) (1981) | 0 / 0 | **11454** (Logzeile SquareRoot0 11448) | F428 (**+131**) | F435 (+138) | 0 F298, 1 F407, 4 F428 - kein Dash (Clip 2/3) |
| **E1** Elza | stehend | F297, cam 0 | (-8200,-25550) | (3197,-24407) (1981) | 0 / 0 | 11454 | F428 (+131) | F435 (+138) | wie L1 |
| **LW** Leon | `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W10,L0.6,UX5` (`Tick 300 -> F301 Tasten 0x0080`, `Tick 318 -> F319 Tasten 0x4010`) | F297 | (-8200,-25550) -> (-8432,-11450) bei F470 | (3197,-24407) | 0 / 0 | 11454 | F487 (**+190**) | F494 (+197) | 0, 1 F407, 4 F487 |
| **EW** Elza | wie LW (2081/2091) | F297 | wie LW | wie LW | 0 / 0 | 11454 | F487 (+190) | F494 (+197) | wie LW |

- RE2-Band (Dossier/F-E2: Abstand 11430..13924, erster Kontakt 121..158 Bilder): Abstand +24 ueber 11430, erster Angriff stehend +131 =
  im Band, kein Zonen-Dash mehr vor dem Biss (Abnahme 4: 17539 / +470 mit 300 Bildern Dash). Beim Wegrennen nach Norden durch den
  Westkanal (14100 Einheiten bis F470) kein Treffer vor +197 -> Ausweichen moeglich (Nachtrag 29).
- Zonen-Massstab byte-true aus dem RDT gelesen (ROOM2090.RDT, 2091 bytegleich): Aot_set sce 8 @0x09D6 `2c 02 08 47 00 00 3c dd 88 96 e4
  3e c4 22 ac f9` = x -8900 / z -27000, 16100 x 8900 -> Suedbecken z <= -18100; @0x09EA `... 5c c7 e4 3e 8c 23 ...` = z -14500..-5400
  (Nord); @0x09FE `... 4c b9 20 1c 10 0e ...` = x -8900..-1700, z -18100..-14500 (Westkanal). `gsz_zone` (gatorszene_r41.c) und
  `gb_zone` (enemy_ai_boss_gator.c, GB_RAMP_Z0/Z1 -18100/-14500) benutzen dieselben Grenzen.
- **F-E2 Punkt 1: erfuellt** (Leon + Elza, stehend + weglaufend).

### 2.3 Kampfbeginn vor der Kamera (Nachtrag 31 Punkt 2) - NICHT umgesetzt
Kamera Cut 0 aus dem RDT (ROOM2090.RDT RID @0x0060: `00 00 3c 68 c2 0a 00 00 c4 f3 ff ff b2 99 ff ff c8 e0 ff ff ee ff ff ff c4 ab ff ff`
= Lage (2754,-3132,-26190) -> Ziel (-7992,-18,-21564), Blick nach Westnordwest). Gemessen an L1 (state.log, Wurzel) und an den
Bildern (`tools/anteil.py`: Vordergrund = max-Kanal-Differenz > 10 gegen den Median-Hintergrund von Cut 0 ueber F298..F650):
- Gator-Wurzel <-> Kamera Cut 0 (xz): **1837 am Kampfbeginn**, 111 Bilder lang (F297..F407) unter 2500 (kleinster Abstand 1756), Tiefe
  laengs der Blickachse 284..2045 (Leon steht in rund 11000 Tiefe); erst ab dem Paddeln (Clip 1, F407) entfernt er sich (F417 3836,
  F447 6643).
- Vordergrund-Anteil der rechten Bildhaelfte: > 25 % in **121 Bildern (F298..F429**; F297 = Schnittbild nicht gezaehlt), > 50 % in 23 Bildern (F310..F392), Spitze 51,9 %
  (F347); ganzes Bild 25..35 % in F307..F407. Elza (E1) Bild fuer Bild gleich (F297..F450: kein Bild mit > 2 % abweichenden Pixeln).
- Bildbogen `F41a5/L1_kampfbeginn.png` und `F41a5/E1_kampfbeginn.png` (F298..F447 je 10 Bilder): ab F307 fuellen Ruecken und Schwanz des
  Gators gross die rechte Bildhaelfte (direkt vor der Linse), Leon steht klein an der Leiter; erst ab ~F417 ist der Gator im Becken vor
  Leon zu sehen. Das ist genau das Bild, das der Bau-Agent selbst als OFFEN 3 meldet und das der Nutzer mit Nachtrag 31 Punkt 2
  ("Ändern: nicht vor der Kamera") zurueckgewiesen hat.
- Ursache: die Kette (Leiche (0,-26000), Anlauf-Gier 1952 von Osten, Kau-Lage (4118,-24249)) endet mit dem Gator 300 vor der Kamera
  Cut 0 (Kamera bei x 2754, Gator bei x 3197, beide an der Suedwand); die Kamera-Uebergabe geht am Kampfbeginn auf den Eintritts-Cut 0
  der Leiter. Kein Riegel misst Abstand Kamera <-> Gator-Teile oder Bildanteil (Nachtrag 31 verlangt ihn).
- **Nachtrag 31 Punkt 2: nicht erfuellt** -> Mangel M1.

### 2.4 Schluessel in der Ankunftseinstellung (Wortlaut "mit einen blinkenden Schluessel"; F-E2 Punkt 2 / Nachtrag 31 Punkt 3)
- L1a (Leon): `tools/lauf.sh L1a 240 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=140#2090
  RE15_FRAMEDUMP=1-140/1:fd_` -> rc=0, 21 s; state.log 2090: cam 0 ab F1 bis F140; `Folge A F40`, `T F98`. E1a (Elza, 2081/2091) gleich.
- Bilder `F41a5/L1a_ankunft.png` (F10..F140): Leon an der Leiter, die Leiche treibt vorn im Suedbecken, der Ring ueber dem Ruecken
  (Bildpunkt ~160,124..131); `F41a5/L1a_key_zoom.png` (6-fach): geschlossener Ring in F10/F82, in den Bildern dazwischen dreht der Ring mit
  dem Wippen der Leiche (sub03 @0x00B5E) und zeigt nur einen Bogen.
- Gegenprobe Zwilling: `RE15_SET_FLAG=3:245,9:112` (L1a0/E1a0) zeichnet Bild fuer Bild GLEICH (`[setflag] flag(9,112/0x70) = 1`, 0
  abweichende Pixel in F10/F40/F90/F130): key_auf_leiche (gatorszene_r41.c) prueft Bit 112 nicht (nur key_bei_leiche nach dem Tod) - in
  der Szene ist das ohne Folgen (Bit 112 ist vor der Szene nie gesetzt), taugt aber nicht als Zwilling. Darum Mass wie Abnahme 4 (graue
  Pixel, max-min <= 14, max > 25) mit abgetrenntem Ruecken: `tools/ring_a5.py` (Komponenten; der Ruecken = Komponente breiter als 14,
  Ring = kleinere Komponenten darueber; Maskenbild `F41a5/L1a_key_maske.png`), Ausschnitt 140..190 x 100..140, F10..F137.
- Ergebnis Leon = Elza: Ring in 128/128 Bildern, **hellster Ring-Pixel min 76 / max 112**; Bilder mit ganzem Ring (>= 12 Pixel) 38, dort
  79..110, Kasten 7 x 9 (vereinzelt 11 x 9 mit einem angrenzenden Fleck). Puls sichtbar: F30 112 -> F62 79 -> F90 109 -> F122 79
  (Periode ~60 Bilder = 0x3c, re15_blink_r41). Soll F-E2 Punkt 2: Ruhe >= 72, Spitze >= 108 -> **erfuellt**; Groesse ~7 x 9 = vom Nutzer
  angenommen (Nachtrag 31 Punkt 3). Bau-Agent: 86..110 / 6 x 8..7 x 9 (anderer Ausschnitt; gleiche Aussage).
- Der Ring sitzt auch in der Rueckkehr-Einstellung "erneut Kamera auf Zombie mit Schluessel" (L1 F10/F40 Cut 0) auf der Leiche.

### 2.5 Fressen (Wortlaut "den Zombie fressen inklusive dem Schluessel"; F-E2 Punkt 3)
- gator_boss.log/debug.log L1 = E1: `Fress-Einstellung: Cut 1 frontal 4116`; Cut 1 aus dem RDT (RID @0x0080: Lage (-7740,-3132,-25956) ->
  Ziel (5724,-18,-21654), Blick nach Osten auf den von Osten kommenden Gator).
- Bilder `F41a5/L1_fressen_zoom.png` (2-fach, F74..F285) und `F41a5/L1_fressen_zoom4.png` (4-fach, F160/F220): Biss von vorn (F74-F87),
  Schuetteln mit der Leiche zwischen den Kiefern (F99/F120/F140), Kopf hoch mit der Leiche am Hals (F160/F180), Kau-Kreis Clip 11 (F201-F266:
  die Leiche haengt links am Hals, der Kopf mit offenem Maul oben), F268 Leiche + Schluessel weg (`V F267`), F285 Schrei zur Kamera.
- **F-E2 Punkt 3: erfuellt** (Frontalwert 4116 > 0, Leiche in allen Fress-Bildern zu sehen; Abnahme 4: Cut 4 frontal -1625). Hinweis H1:
  im Kau-Kreis liegt die Leiche sichtbar NEBEN dem offenen Maul (Kind der Part-6-Matrix mit eingefrorener Relation aus Koppelbild 13,
  RE2-Kanister-Form @0x801018d0-f0, seit Abnahme 0 unveraendert) - kein Mangel nach F-E2, fuer den Nutzer zu beurteilen.

### 2.6 Echter Weg: Leiter in 2080 per Aktionstaste (nicht nur RE15_FIRE_AOT)
- LT3: `tools/lauf.sh LT3 150 ... RE15_SET_FLAG=3:245 RE15_DEBUG_JUMP=2080@250 RE15_PLAYER_POS=-9950,-25650,0,8 RE15_PAD_AT=45:A,75:A
  RE15_EXIT_AT=320#2090` (Leon steht in 2080 auf dem oberen Laufgang westlich der Leiter, Blick nach Osten, drueckt die Aktionstaste;
  Vorwaertspunkt 620 im Rechteck der Leiter-Tuer Slot 1 @0x0BC2) -> rc=0, 38 s: `[aot] DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,
  hh=500) target_cut=0 spawn=(-8200,0,-25550)` -> dieselbe Folge -> `Folge C ROOM2090 F297`, `Kampfbeginn F297: Gator (3197,-24407) Gier
  1981 Zone 0, Spielfigur (-8200,-25550) Zone 0, Abstand 11448` = L1 (die Aktionsdruecke F45/F75 in der Szene aendern nichts).
  (LT/LT2 mit Blick nach Sueden bei x -9150/-9300: die Kollision schiebt Leon auf x -9668, Vorwaertspunkt 18 neben dem Rechteck - keine
  Tuer; Bedienweg von Westen ist der richtige.)

### 2.7 Knall (Nachtrag 17), 2080 oben (Nachtrag 20), RE2-Kampfmusik (Nachtrag 6) - Regression
- L1 (SDL_AUDIODRIVER=dummy): `Folge T ROOM2090 F98 wert=0/1` (Knall-Bank Satz 0 gespielt); L1a (RE15_NOAUDIO) `wert=0/0`.
- 2080-Montage (L1 state.log, Abschnitt 2080 nach `P 2 ROOM2080 F142`): 8 Aktoren t=16, erstes Bild (-12800,-19600) .. (-9900,-14700),
  letztes Bild (-13898,-19753) .. (-10801,-19158) - alle laufen, alle x > -16650 (oberer Laufgang, Band 8).
- Musik: `[bgm] stage=1 room=09 entry=FF3C -> MAIN3C` + `[bgm] loaded: 1 VAGs, SEQ 100B, PPQN=480 tempo=372670us` ab `B 1 F299`
  (= shared_assets/RE2/SOUND/MAIN1A.BGM), vorher MAIN2E.
- **Urteil 1a (Ankunftsszene): teilweise** - Folge, Schluessel-Helligkeit (F-E2 2), Fressen von vorn (F-E2 3) und Kampfbeginn im RE2-Band
  (F-E2 1, Leon + Elza, stehend + weglaufend) an der exe erfuellt; NICHT erfuellt ist Nachtrag 31 Punkt 2 (Gator am Kampfbeginn direkt vor
  der Linse von Cut 0, 111 Bilder unter 2500 von der Kamera, rechte Bildhaelfte 121 Bilder > 25 % bedeckt) -> M1; Riegel dazu fehlt -> M2.

## 3. Punkt 1b — Schluessel nach dem Tod bei der Leiche, blinkend (Regression)

Rezept der Riegel `integration_r41_gatorszene_tod_explosion*` (12 Schuesse ausgeschrieben): `RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110
RE15_R39_MESS_SPIELER_HP=100 RE15_GB_TEST=5 RE15_GATOR_KANISTER=4430,-26160 RE15_GIVE=3:15,21:50 RE15_EQUIP=3 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W5,M0.3,MA0.1,... (12x) RE15_EXIT_AT=330#2090 RE15_FRAMEDUMP=150-330/1:fd_`; Zwilling ohne
Schluessel `RE15_SET_FLAG=9:110,9:112`. Der Tod ist in diesem Baum nur ueber Spur B's Testhaken erreichbar (der echte Weg = Kanister-Szene
der Spur G, Integration).
- KW (Leon): gator_boss.log `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F160`, `R1 ende (Leiche) pos=(3413,-25001)`; debug.log `Folge R ROOM2090
  F196`, `Schluessel-Schnitt statt R1-Rueckschnitt ... F196`, `Water Key (Item 0x34, Bits (9,111)/(9,112)) bei der Leiche (1400,-1704,-23300)
  ... Zone Slot 15`, `Schluessel-Einstellung: Cut 2 (vorher Cut 0) F196 ... Ende: Cut 0 F288`; state.log cam 1 F160, cam 2 F196, cam 0 F287.
  `tools/diffserie.py KW KWo 150 330`: 135 Bilder mit Differenz, in der Einstellung (F197-F287) je **29 px, Kasten (32,192) 13 x 15,
  hellster 70 -> 102** (Puls), danach in Cut 0 8 px am Bildrand (318,176).
- KWE (Elza, PL04, 2091) gegen KWEo: dieselben Zeilen und Zahlen (`Folge R ROOM2091 F196`, 29 px, 13 x 15, 70 -> 102; die 112 erst danach am Bildrand von Cut 0).
- = Abnahme 4 (29 px, 13 x 15, 70 -> 103). **Urteil 1b: erfuellt** (Leon + Elza).

## 4. Punkt 2 — "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

- state.log ` t=25 ` (Typ 0x25) in allen 2090/2091-Abschnitten: L1/E1/LW/EW/L1a/E1a/KW/KWE/G1b/LT3 = 0; G2b 120 Zeilen, alle erst ab dem
  20A0-Abschnitt (Zeile 572 `F1 ... PL(4950,4100,...)`, Aktoren `[1 t=25 ... @(22600,-29000,...)]` = die Schachtspinnen von 20A0, nicht
  verlangt). debug.log je 2090/2091-Besuch zwei Zeilen `Spinne 0x25 bei (-100,-8296)|(-2300,-23896) verworfen`.
- **Urteil Punkt 2: erfuellt** (Leon und Elza).

## 5. Nachtrag 21 (Leiter statt Tor, Tor mit dem Schluessel) - Regression

- Leiter-Zugang: L1/E1/LW/EW (FIRE_AOT) und LT3 (Aktionstaste) - Abschnitte 2.1/2.6.
- G1b (`RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110 RE15_PLAYER_POS=6630,-16350,0,1 RE15_PAD_AT=60:A RE15_MSG_LOG=1`): `ROOM2090 Slot 1
  (Tor) gesperrt: Ereignis 19, Bit (9,113) = 0`, `[msgtext] room=2090 id=42 "It's locked."`, `Folge Z ROOM2090 F60`.
- G2b (dto. + `RE15_GIVE=0x34:1`, A F60/140/...): `[msgtext] room=2090 id=43 "You've opened the lock."`, `Folge U F60`, `[aot] DOOR FIRE slot=1
  rect=(7250,-16350,hw=500,hh=1750) target_cut=13 spawn=(4950,-3600,4100)`, `[tuer] Sequenz Archiv 2 DOOR26 ...`, `EXIT_AT: Bild 120 in
  Raum 20a0`. **Urteil: erfuellt.**

## 6. RE-Gate (Stichproben selbst disassembliert/gelesen, richtige Binaerdatei)

`re2_disasm.py --bin EM23_OVL_0000.BIN` (info/re2leon/COMMON/BIN, geladen @0x80100000), `re2_disasm.py` ohne --bin = info/re2leon/PSX.EXE,
`tools/r41_re2gator/re2_scd_walk.py` auf info/re2leon/PL0/RDT/ROOM40A0.RDT (SCD-main @0x25C0), Python-Lesen von
shared_assets/PSX/STAGE2/ROOM2090.RDT (2091 bytegleich):

| Zitat im N5-Diff | selbst gelesen | Ergebnis |
|---|---|---|
| Zonenbyte +0x21A aus INIT 0 `sb zero,538(s0)` @0x80100654 | `80100654: sb zero,538(s0)` (zwischen `sh zero,536(s0)` und `sh zero,542(s0)`) | stimmt |
| Zone 0 -> 1 nur ueber die GATOR-Lage `lw v0,56(s0) / slti v0,v0,-17877` @0x80100990-98 | `8010097c lbu v0,538(s0)`, `80100988 bne v0,zero,0x801009c8`, `80100990 lw v0,56(s0)`, `80100998 slti v0,v0,-17877`, `801009a0 addiu v0,zero,1`, `801009a8 sb v0,538(s0)`, `801009ac addiu v0,zero,769` / `801009b0 sw v0,4(s0)` (Routine 0x301) | stimmt; s0 = der Gator selbst (+0x38 x), keine Spielerlage gelesen |
| Zone 1 -> 2 `lw v0,64(s0) / slti v0,v0,-7322` @0x801009d8-ec | `801009c8 lbu v1,538(s0)`, `801009d8 lw v0,64(s0)`, `801009e0 slti v0,v0,-7322`, `801009e8 addiu v0,zero,2`, `801009ec sb v0,538(s0)`, `801009fc sw v1,4(s0)` (v1 = 769) | stimmt |
| RE2 Intro: Clip 6 bis zum Ende, dann Cut 1 @0x801008ac-c8 (Begruendung der PORT-WAHL "Durchbruch bis Bild 22") | `801008ac addiu a1,zero,6`, `801008b4 jal 0x8001a330`, `801008bc beq v0,zero,0x80100950` (weiter, solange der Clip laeuft), `801008c4 jal 0x8002c7ac` / `801008c8 addiu a0,zero,1` | stimmt (RE2 spielt Clip 6 ganz; die Kuerzung ist PORT-WAHL, vom Nutzer angenommen: Nachtrag 31 Punkt 1) |
| FRESSEN P1 Clip 4 @0x801010ec-f4 | `801010ec addiu a1,zero,4`, `801010f0 addu a2,zero,zero`, `801010f4 jal 0x8001a330` | stimmt |
| SquareRoot0 wie RE2 +0x1F0 `jal 0x8008d2f4` @0x800265d4 (PSX.EXE) | `800265cc mult v0,v0`, `800265d4 jal 0x8008d2f4` (a0 = dx^2 + dz^2 im Verzoegerungsplatz), `800265e0 sw v0,496(s0)` | stimmt |
| ROOM40A0 sub03: Pos_set @0x0250, While/Cmp Cut 1 @0x025E-6A, Sleep 0x3c @0x0276, Sleep 0x32 @0x027E, Plc_dest + While @0x0286-98 und @0x029E-B0, Plc_ret @0x02B6, Set(2,7,0) @0x02B7 | `0250 Pos_set 320080daf8f8389b`, `025E While 0f060a00` + `0262 Cmp 23001a050100`, `0276 Sleep 090a3c00`, `027E Sleep 090a3200`, `0286 Plc_dest 4000080ad0d5389b` + `028E While`, `029E Plc_dest 4000150a00088000` + `02A6 While`, `02B6 Plc_ret 42`, `02B7 Set 22020700` (sub03 = 0x010A..0x02BD) | stimmt; 1 + 60 + 50 + 1 + 1 = fruehestens Cut 1 + 113 |
| ROOM2090: Kamera Cut 0 RID @0x060, Lichtsatz @0x468, obj 2 @0x00A6C, Zonen @0x009D6/@0x009EA | RID `@0x0060 ... c2 0a 00 00 c4 f3 ff ff b2 99 ff ff c8 e0 ff ff ee ff ff ff c4 ab ff ff` = (2754,-3132,-26190) -> (-7992,-18,-21564); Cut 1 @0x0080 = (-7740,-3132,-25956) -> (5724,-18,-21654); Licht @0x468 Licht 0 `ce 09 60 f0 e9 a1` = (2510,-4000,-24087); `@0x0A6C 2d 02 00 00 02 00 0a 00 00 00 43 f9 34 f9 25 ab 00 00 ad 02` = obj 2 (-1725,-1740,-21723) Gier 685; Aot_set sce 8 @0x09D6/@0x09EA/@0x09FE (Abschnitt 2.2) | stimmt |

- Konstanten im N5-Diff: GSZ_CUT_LEICHE 0 (RID @0x060, Lichtsatz @0x468), GSZ_LEICHE_X/Z/ROT_Y_STD, GSZ_GATOR_GIER_STD, GSZ_DURCHBRUCH_BIS_STD 22,
  GSZ_KEY_AUF_RX/RZ_STD - alle im Abschnitt "PORT-WAHL der Inszenierung (Nachtrag 6 ...)" mit Herleitung (lagesuche, leiche_lage_n5.py) und
  Original-Bytes von obj 2; GSZ_ZONE_SUED_Z/NORD_Z mit RDT-Offset (@0x009D6/@0x009EA, selbst gelesen). Keine Verhaltenskonstante ohne Beleg.
- Suche in den `+`-Zeilen von `git diff 4b875538..f83f619b -- re15_port` nach deferred/tunable/interim/for now/faithful/plausib/TODO/vorerst/
  geschaetzt/geraten/ungefaehr/approx/FIXME/HACK/nach Gefuehl/Platzhalter/sieht richtig: 0 Treffer; ueber den ganzen F-Diff 1 Treffer, verneint
  ("keine Zahl nach Gefuehl", probe_re2_intro_ende.c). Kein getenv in gatorszene_r41.c / gatorszene_r41_pc.c / re15_gatorszene_r41.h; der
  Lage-Setzer ist nur fuer Sonden (kein Spielweg, kein Env-Schalter).
- Erklaert der Fix den Befund? M1 der Abnahme 4 (Dash): ja - der Gator beginnt in Leons Beckenzone 0, die Clip-Folge danach hat keinen Clip 2/3
  vor dem Biss (L1: 0 F298, 1 F407, 4 F428). M3 (Fressen von hinten): ja - Anlauf nach Westen, Spur B's FINISHER-Wahl findet Cut 1 vor dem Kopf
  (frontal 4116). M2 (Helligkeit): ja - Cut 0 hat das Licht (2510,-4000,-24087) ueber dem Suedbecken.
- Doku-Hinweis H2: der Kommentar ueber `key_auf_leiche` (gatorszene_r41.c:427) und das Dossier N5 M2 nennen noch "Kind von obj 3 (wippt mit
  ihr, sub03 @0x00B5E)" - die Szenen-Leiche ist seit N5 obj 2, deren Wippen ist sub02 @0x00AF0 (`02 00 2e 03 02 00 2f 07 ...`, selbst gelesen;
  @0x00B5E `2e 03 03` = obj 3). Kein Verhaltensfehler.
- **RE-Gate: erfuellt.**

## 7. Vertrag

- Bits nur (9,110..113) (VERTRAG 1.1; test_r36_kartenbits.c k_port[] mit vier F-Zeilen), Nachrichten 2090/2091 Ids 42/43 (zugeteilt 42-46),
  Ereignisse 18/19/20, AOT-Platz 15 - Nachbesserung 5 belegt nichts Neues (Code-Diff nur gatorszene_r41.c/.h).
- Pfade `git diff 9f10bdb0..HEAD --name-only` (67 Dateien): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, kein
  CMakeLists.txt (0 Treffer). Neue RE2-Datei nur shared_assets/RE2/SOUND/MAIN1A.BGM (erlaubt, im Dossier fuer das Paket-Gate genannt).
  Nachbesserung 5 aendert nur F-eigene Dateien (gatorszene_r41.c/.h, test_r41_gatorszene.c/.cmake, probes/r41_gatorszene.cmake, Werkzeuge).
- Gemeinsame Dateien (seit Abnahme 4 unveraendert): scd_vm.c (4 Zeilen), scd_room_setup.c (2), enemy_ai_boss_gator.c (8), main.c (10),
  audio_pc.c (4), item_*/itps/inv-Haken (je 1-4) - alle als "Runde 41 Spur F" markiert.
- Riegel (Abschnitt 1): unit_r41_gatorszene_abstand/_elza und r41f_kampf (leon/elza/flucht/flucht_elza) pruefen jetzt das RE2-Band mit
  oberer UND unterer Grenze und die Zonen; Gegenprobe selbst (Funktionen r41f_re2soll/r41f_kampf/r41f_folge_c WORTGLEICH aus
  test_r41_gatorszene.cmake Zeilen 80-234 in `F41a5/tools/gegenprobe.cmake`): meine Laeufe L1/E1 `OK ... ab2=131198058 biss=F428
  treffer=F435 k=131`, LW/EW `k=190`; Abnahme-4-Lauf `F41a4/runs/L1` FEHLER `F422 307624213 nicht im RE2-Band 130647809..193864141`;
  Abnahme-3-Lauf `F41a3/runs/L1` FEHLER `F405 37971400 nicht im RE2-Band`. `test_r41_gatorszene abstand` direkt: 11448 im Band, Zone 0/0, Biss
  131 im Band 121..158, Gegenproben Fassung 1 (48 < 121) und N4-Lage (270 > 158); `... leiche`: Primitivfarbe 85 -> 126 >= 84/125.
  Es fehlt ein Riegel fuer Nachtrag 31 Punkt 2 (Abstand Kamera <-> Gator-Teile / Bildanteil) -> M2.
- Nebenwirkungen: Leon = Elza in allen Laeufen; Nachbarraeume 2080 (Montage, 8 Zombies oben) und 20A0 (Tor G2b, Schachtspinnen bleiben) gemessen.
  Querbezug Spur G: Kampfbeginn jetzt F297 mit dem Gator im Suedbecken (3197,-24407) - G's Kampf-/Kanister-Riegel liegen nur im G-Baum
  (Integration).

## 8. Maengel (nummeriert, nachpruefbar)

- **M1 Kampfbeginn direkt vor der Linse von Cut 0 (Nachtrag 31 Punkt 2, Leon und Elza).** Gemessen (Abschnitt 2.3, Laeufe F41a5/runs/L1 und
  E1, Leiterweg, Stand f83f619b): Kamera-Uebergabe in F297 auf Cut 0 (RID @0x0060: Kamera (2754,-3132,-26190)); die Gator-Wurzel beginnt
  1837 von der Kamera (298 Tiefe laengs der Blickachse, 1813 seitlich) und bleibt 111 Bilder (F297..F407) unter 2500 (kleinster Abstand 1756);
  Vordergrund-Anteil der rechten Bildhaelfte (max-Kanal-Differenz > 10 gegen den Median-Hintergrund von Cut 0) > 25 % in 121 Bildern
  (F298..F429), > 50 % in 23 Bildern (F310..F392), Spitze 51,9 % in F347; Bildboegen `F41a5/L1_kampfbeginn.png` / `E1_kampfbeginn.png`
  (Ruecken und Schwanz gross im Vordergrund, Leon klein an der Leiter; erst ab ~F417 schwimmt der Gator im Becken vor Leon). Ursache: die
  Kette (Leiche (0,-26000), Anlauf 1952 von Osten) endet mit dem Gator an der Suedwand neben der Kamera Cut 0, und der Kampf beginnt auf
  dem Eintritts-Cut 0. Soll (Nachtrag 31): Startlage so, dass der Gator beim Kampfbeginn im Becken VOR Leon zu sehen ist und nicht direkt
  vor der Linse der laufenden Kamera; der RE2-Kampfbeginn (Abstand 11430..13924, erster Kontakt/Angriff 121..158, Zonen 0/0) bleibt;
  gemessen ueber die ersten ~150 Bilder (Abstand der Gator-Teile zur Kamera / Bildanteil je Bild), Bildbogen Leon + Elza. Pruefen:
  `kampf_a5.py` (Spalte "kam") und `anteil.py <lauf> 298 650 297 450 10` auf einem Leiterlauf.
- **M2 Riegel fuer Nachtrag 31 Punkt 2 fehlt.** Weder test_r41_gatorszene.c noch test_r41_gatorszene.cmake pruefen Abstand Kamera <->
  Gator-Teile oder Bildanteil am Kampfbeginn (grep "Linse|anteil|Kamera.*Abstand": nur eine fremde Zeile der Schluessel-Einstellung nach
  dem Tod). Nachtrag 31 verlangt ihn ("Mass: ... je Bild ueber die ersten ~150 Bilder (Riegel)"); die heutige Lage (1837 / 121 Bilder)
  muss ihn rot machen (Gegenprobe), Leon und Elza.

Hinweise (keine Maengel):
- H1 Fressen: im Kau-Kreis (Clip 11, F201-F266) liegt die Leiche sichtbar neben dem offenen Maul statt zwischen den Kiefern (Kind der
  Part-6-Matrix mit der in Koppelbild 13 eingefrorenen Relation, RE2-Kanister-Form @0x801018d0-f0, seit Abnahme 0 so) - F-E2 Punkt 3 ist
  erfuellt; der Nutzer kann die Form am Bild `F41a5/L1_fressen_zoom.png` beurteilen.
- H2 Doku: Kommentar ueber `key_auf_leiche` (gatorszene_r41.c:427) und Dossier N5 M2 nennen "obj 3 / sub03 @0x00B5E"; seit N5 ist es obj 2,
  Wippen sub02 @0x00AF0.
- H3 `key_auf_leiche` prueft Bit (9,112) nicht (nur `key_bei_leiche`) - im Spiel ohne Folgen (112 kann vor der Szene nicht gesetzt sein),
  als Mess-Zwilling taugt `RE15_SET_FLAG=9:112` deshalb fuer die Ankunft nicht.
- H4 Integration F+G: Kampfbeginn jetzt F297, Gator-Start (3197,-24407) im Suedbecken; G's Kampf-/Kanister-Riegel im Integrationsbaum neu
  messen (aendert sich mit M1 erneut).
- H5 Unveraendert offen (Bau-Agent): MAIN2E nach dem Tod, PSX-Bau ungeprueft, Schachtspinnen 20A0, Nachrichten-Ids 44-46 frei, nach dem Laden
  eines Spielstands keine Einstellung auf den Schluessel (Ersatzlage (-212,-24931)), Tod in diesem Baum nur ueber Spur B's Testhaken.

## 9. Urteile

| Punkt | Urteil | Beleg |
|---|---|---|
| 1a Ankunftsszene (Leiche + blinkender Schluessel, Knall, 2080 oben, erneut Leiche, Alligator aus dem Wasser frisst Leiche samt Schluessel, Schrei/Schuetteln zu Leon, RE2-Kampfmusik; Nachtraege 6/17/20/21/29, F-E2, Nachtrag 31) | teilweise | 2.1-2.7: Folge L1/E1/LT3 vollstaendig (C F297, MAIN1A), F-E2 1 erfuellt (11454 / Biss +131, Zone 0/0, kein Dash; weglaufend +190), F-E2 2 Helligkeit erfuellt (Ring 76 -> 112, Groesse 7 x 9 vom Nutzer angenommen), F-E2 3 erfuellt (Cut 1 frontal 4116, Leiche im Maul zu sehen); NICHT erfuellt Nachtrag 31 Punkt 2: Gator am Kampfbeginn direkt vor der Linse von Cut 0 (1837, 111 Bilder < 2500, 121 Bilder > 25 % der rechten Bildhaelfte) (M1), ohne Riegel (M2) |
| 1b Schluessel nach dem Tod bei der Leiche, blinkend | erfuellt | 3: KW/KWE Folge R F196, Einstellung Cut 2 F196-F287, 29 px, 13 x 15, 70 -> 102 (= Abnahme 4), tod-Riegel gruen |
| 2 Spinnen aus 2090 entfernen | erfuellt | 4: 0 t=25-Zeilen in allen 2090/2091-Abschnitten, zwei "verworfen"-Zeilen je Besuch, Leon + Elza |
| Nachtrag 21 Leiter statt Tor, Tor mit dem Schluessel | erfuellt | 2.6/5: Leiter per Aktionstaste (LT3), G1b "It's locked.", G2b "You've opened the lock." + Tuer nach 20A0 |
| RE-Gate | erfuellt | 6: 8 Stichproben (EM23 Zonenbyte INIT/Zone 0->1/Zone 1->2, Intro Clip 6, FRESSEN Clip 4, RE2 SquareRoot0, ROOM40A0 sub03, ROOM2090-RDT) stimmen, Konstanten gekennzeichnet, keine Rate-Woerter, Fix erklaert die Befunde der Abnahme 4 |
| Vertrag/Riegel | teilweise | 1/7: 221/221 gruen, nur erlaubte Pfade und zugeteilte Ressourcen, Riegel mit oberer und unterer Grenze messen (Gegenprobe); Riegel fuer Nachtrag 31 Punkt 2 fehlt (M2) |

**Ergebnis: NICHT bestanden.** Die Maengel M1-M4 der Abnahme 4 (= F-E2) sind an der exe behoben (Leon und Elza; Riegel mit beiden Grenzen,
selbst gegengeprobt), aber der vier Minuten nach dem Abschluss-Commit eingegangene, massgebliche Nachtrag 31 Punkt 2 ("Ändern: nicht vor der
Kamera") ist nicht umgesetzt: M1 (Gator am Kampfbeginn direkt vor der Linse von Cut 0) und M2 (Riegel dafuer). Nachtrag 31 Punkt 1
(Durchbruch-Kuerzung) und Punkt 3 (Schluesselgroesse) sind vom Nutzer angenommen.
