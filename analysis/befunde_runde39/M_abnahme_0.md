# Runde 39 — Spur M "schluessel2": Abnahme 0 (unabhaengig, fortlaufend geschrieben)

Baum `.claude/worktrees/r39_schluessel2` (Zweig `r39/schluessel2`, Basis f70f6ec4, Kopf 50e8be0e). Pruefer: Abnahme 0,
2026-10-18 ab 20:53. Alles unten selbst gemessen am Stand dieses Baums; Behauptungen des Bau-Agenten nur als Vergleich.

## 0. Gelesen / Bau
* AUFTRAG.md (Nachtrag 5, Wortlaut), VERTRAG.md, Dossier M_schluessel2.md; `git log master..HEAD` (9 Commits eae6405a..50e8be0e),
  `git diff master...HEAD --stat` (18 Dateien; `git diff master --stat` zeigt zusaetzlich AUFTRAG.md/UEBERGABE.md mit
  Loeschungen — das sind nur die NEUEREN master-Commits nach der Basis, der Zweig selbst aendert sie nicht).
* Code-Diff gelesen: `tools/r38_itemorte/itemorte_export.py` (NUTZER_FAKTOR_M, gte_ecken, Bisektion), `gen/itemorte_r38_lage.inc`
  (RE15_R38K_PARK_S10000 24100 -> 8063, PRISON 25623 -> 12073, neue RE15_R39M_*), `gen/itemorte_r38_md1.inc` (nur die 5 Ecken
  @0x44 je MD1), Tests (neu: test_r39_schluessel2.c, test_r39_schluessel2.cmake, probes/r39_schluessel2.cmake; nachgezogen:
  test_r38_itemorte.c/.cmake, test_r39_optik.c, test_r39_optik_schluessel.cmake). Kein Engine-/Plattform-Code geaendert.
* Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`, EXIT 0, 768/768 Schritte,
  exe 20:54 (md5 1623d459...).

## 1. Daten-Aenderung selbst nachgeprueft
* MD1-Bytes (eigenes Skript, `git show f70f6ec4:...itemorte_r38_md1.inc` gegen Kopf, gegen RE2-Original `info/re2leon`
  PL0/RDT/ROOM6150.RDT @0x4FBC (md5 3c9176ee...) bzw. ROOM10F0.RDT @0x1650 (md5 c5d49ef6...)): je MD1 236 B, Unterschied
  NUR in 0x44..0x69 (die 5 Ecken). Ecken x/z: Park alt (-181..186, -95..88) = Spanne 367 x 183 -> neu (-60..62, -32..29) =
  122 x 61 (33,2 % / 33,3 %); Prison alt 389 x 195 -> neu 183 x 91 (47,0 % / 46,7 %); y bleibt 0; alles andere == RE2.
* Welche exe welchen Stand traegt (Suche der 236-B-Bloecke im Binaerbild): Kopf-exe 20:54 = Spur-M-MD1; `re15_pc_r39m_vorher.exe`
  = Spur-L-MD1; **Hauptbaum-exe 18:45 (die der Nutzer um 19:29 vor sich hatte) = Spur-L-MD1** -> der Bezug "immer noch zu
  gross" ist der Spur-L-Stand, wie im Vertrag angenommen. PE-Sektionen: Kopf-exe .text 02595202 == `_nach2` .text, .data
  58e1af2f in allen drei gleich (Bau-Agent-Angabe bestaetigt).
* Generator selbst gelaufen (Kopie in eine Scratch-Spiegelung, damit der Baum unberuehrt bleibt; Eingaben = Baum-Dateien):
  ohne `--schreiben` exakt die Dossier-Zeilen (Park s_oben 0.8931 / s_unten 0.7195 -> s 0.8063, GTE 2 x 1, bindet
  GTE-Raster; Prison s_oben 1.2809 / s_unten 1.1338 -> s 1.2073, GTE 3 x 2, bindet stetig); mit `--schreiben` beide
  gen-Dateien **bytegleich** zum Commit (md5 lage c289f40a..., md1 bb8c307a...).

## 2. RE-Gate (Stichproben selbst disassembliert, `re15_disasm.py`, info/Re1.5/PSX.EXE md5 b55fdaa5...)
| Zitat im Diff/Dossier | selbst gelesen | Urteil |
|---|---|---|
| H = fov >> 7 @0x80021e68/6c/70 | `80021e58 lh v0,0x0fe4(v0)` (Cut), `80021e60 sll v0,v0,5`, `80021e68 lhu a0,2(v0)`, `80021e6c jal 0x80066c30`, `80021e70 srl a0,a0,7` (Verzoegerungsplatz) | stimmt |
| SetGeomScreen @0x80066c30 | `80066c30: 0x48c4d000` = ctc2 a0,$26 (H), `jr ra` | stimmt |
| zweite Stelle @0x80046120..28 | `80046120 lhu a0,2(v0)`, `80046124 jal 0x80066c30`, `80046128 srl a0,a0,7` | stimmt |
| SetGeomOffset @0x8001643c/40/44 | `ori a0,zero,0xa0` / `jal 0x80066d60` / `ori a1,zero,0x78`; 0x80066d60: `sll a0,16; sll a1,16; ctc2 a0,$24 (OFX); ctc2 a1,$25 (OFY)` | stimmt (160/120) |
| FUN_80022f0c RTPT @0x80022fb0 | Bytes `30 00 28 4a` = 0x4a280030 = COP2 RTPT (sf=1) | stimmt |
| SXY ins Polygon @0x80022fd8..e0 | Bytes `08 00 8c e9 14 00 8d e9 20 00 8e e9` = swc2 $12(SXY0),8(t4) / $13(SXY1),0x14(t4) / $14(SXY2),0x20(t4) = xy0/xy1/xy2 eines POLY_GT3 in a1 (Code `(a3<<1) | 0x34` @0x80022f18/1c ins Farbwort a2+3 @0x80022f38, je Polygon kopiert `lw v0,0(a2)` @0x80023024 / `sw v0,4(a1)` @0x8002302c; naechstes Polygon `addiu a1,a1,80` @0x80023084) | stimmt |
| NCLIP @0x80022fec, bltz @0x80023010, AVSZ3 @0x80023020 | `06 00 40 4b` NCLIP; swc2 MAC0,0(sp) @0x80023004 + `bltz v0` @0x80023010; 0x4b58002d AVSZ3 | stimmt |
* Bezug FUN_80022f0c -> Schluessel: RE15_FUN_CATALOG.md Z. 213/215: FUN_8002c18c (Prop-Zeichnen) ruft fuer flag-0x10-Props
  FUN_80022f0c/98 (flach, ungelichtet) — die Schluessel tragen genau Licht/Attribut 0x0010 (RE15_R38K_SCHL_ATTR). Der
  zitierte Zeichner ist also der fuer diese Prop-Art zustaendige.
* Port-Seite: `objpoly_r38.c:29-31` RTPS mit `re15_gte_divide` und Ganzzahl-SX/SY; Prop-Zeichner main.c:10942 ruft
  `re15_objpoly_r38_tri_mesh(..., cx, cy, ...)` mit `cx = SCREEN_XRES / 2`, `cy = SCREEN_YRES / 2` (main.c:5961/5962, 160/120).
* Konstanten im Diff: `NUTZER_FAKTOR_M = 0.5` / `RE15_R39M_FAKTOR100 50` als NUTZER-VORGABE gekennzeichnet (Werkzeug-Kopf,
  inc-Kommentar "kein RE-Wert"); alle uebrigen neuen Zahlen sind ERZEUGT (Werkzeug, reproduziert, s. 1.) bzw. Spur-L-Basis.
  Suche im Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO: keine Treffer in Code/Werkzeug/Tests
  (nur Dossier-Prosa ohne diese Woerter). Kein Env-Schalter eingefuehrt.
* Hinweis (kein Sperrgrund): Dossier Z. 151 nennt fuer die Bildmitte "`const int cx = 160, cy = 120` main.c:2077" — das
  ist `pselect_render_model` (Spielerauswahl), nicht der Prop-Zeichner; der zustaendige Wert steht in main.c:5961/5962
  (`SCREEN_XRES / 2`). Zahl stimmt, Fundstelle falsch.

## 3. Punkt 1 — "Und die Schlüssel im Communicationsroom sind immer noch zu groß, die sollten maximal halb so groß sein."

### 3.1 Messprotokoll (selbst, eigene exe-Kopien `re15_pc_m0abn_neu.exe` = Kopf-Bau 20:54 md5 1623d459, `re15_pc_m0abn_vor.exe`
= Basis-Bau (Spur-L-MD1, s. 1.); eigene Laufordner im Scratchpad; Auswertung mit EIGENEM Python-Differenzbild (nicht dem
Werkzeug des Bau-Agenten): Fenster = Pinnwand-Fenster x Skala (x 131s..145s+s-1, y 75s..87s+s-1), Pixel mit
|dR|+|dG|+|dB| > 30 (Mass der Abnahmen L0/M) und zusaetzlich jede Abweichung > 0, Huelle, Sum|dL| (Rec. 601), aussen.)
Laeufe je mit Schluessel ("mit") und mit gesetztem Genommen-Bit (9,82)/(9,83) ("ohne"), Cut 5 (Nutzer-Cut key_location.bmp):
* **Sprung** (Bild 150): Leon `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10F0@250 RE15_PLAYER_POS=0,4000,1024
  RE15_FORCE_CUT=5 RE15_EXIT_AT=150#10F0 RE15_SET_FLAG=9:71,3:94[,9:82]`; Elza `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
  RE15_PSELECT_AUTO_SWITCH=1 RE15_GOTO_ROOM=10F1 RE15_PLAYER_POS=0,4000,1024 RE15_FORCE_CUT=5 RE15_EXIT_AT=150#10F1 [RE15_SET_FLAG=9:83]`;
  Skala 1/2/3/4.
* **Spielstand + CONTINUE** (Bild 100, Skala 3): `probe_r37_feuerloescher_karte re15_card.mcr 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94
  [flag:9:82]` bzw. `10F1 0 4000 1024 cut:5 elza [flag:9:83]`, `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 ... RE15_EXIT_AT=120`.
* **ECHTER Tuer-Eintritt** (neu in dieser Abnahme, Skala 3): Spielstand im Flur vor der Tuer `10D0 1900 -7000 2048 flag:3:50
  flag:4:247 flag:9:71 flag:3:94 [flag:9:82]` (Leon) bzw. `10D1 1900 -7000 2048 elza flag:3:50 flag:4:247 [flag:9:83]` (Elza),
  CONTINUE, `RE15_INPUT_SCRIPT=A0.2,W12` (Aktionstaste an der Tuer), Kamera danach mit `RE15_FORCE_CUT=5` auf die Pinnwand
  (reiner Kamerahaken), Bild 350 nach dem Eintritt. debug.log: `[aot] DOOR FIRE slot=0 rect=(800,-7050,hw=500,hh=1000)
  target_cut=0 spawn=(8400,0,-350)`, `[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei
  (1358,-2495,-3400) rot (3072,0,2048), Zone Slot 8` bzw. Elza `[pl] Spieler-Familie PL04`, `CONTINUE: resumed in room 10d1`,
  `[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) ...`, `EXIT_AT: Bild 360 in Raum 10f1`.

| Lauf | VORHER (Spur L) > 30 | NACHHER (Spur M) > 30 | Breite / Hoehe | jede Abweichung > 0 nachher |
|---|---|---|---|---|
| Leon Skala 3 (960x720) Sprung | 69 Px, x408..421 y239..246 = **14 x 8**, Sum 3731 | 9 Px, x413..416 y241..243 = **4 x 3**, Sum 403 | **29 % / 38 %** | 9 Px, 4 x 3 |
| Leon Skala 3 CONTINUE | 69 / 14 x 8 / 3731 | 9 / 4 x 3 / 403 | 29 % / 38 % | 4 x 3 |
| Leon Skala 3 **Tuer-Eintritt** | 69 / 14 x 8 / 3731 | 9 / 4 x 3 / 403 | 29 % / 38 % | 4 x 3 |
| Elza Skala 3 Sprung | 63 Px, x408..423 y239..248 = **16 x 10**, Sum 3960 | 16 Px, x410..417 y242..246 = **8 x 5**, Sum 748 | **50 % / 50 %** | 19 Px, 8 x **6** |
| Elza Skala 3 CONTINUE | 63 / 16 x 10 / 3960 | 16 / 8 x 5 / 748 | 50 % / 50 % | 8 x 6 |
| Elza Skala 3 **Tuer-Eintritt** | 63 / 16 x 10 / 3960 | 16 / 8 x 5 / 748 | 50 % / 50 % | 8 x 6 |
| Leon Skala 4 (1280x960) | 125 / 18 x 11 / 6311 | 13 / 6 x 3 / 571 | 33 % / 27 % | 6 x 3 |
| Elza Skala 4 | 118 / 22 x 14 / 5524 | 28 / 10 x 7 / 1215 | 45 % / 50 % | 10 x 7 |
| Leon Skala 2 (640x480) | 36 / 9 x 6 / 1942 | 5 / 3 x 2 / 331 | 33 % / 33 % | 3 x 2 |
| Elza Skala 2 | 26 / 11 x 7 / 1500 | 9 / 6 x 3 / 504 | **55 %** / 43 % | 6 x 3 |
| Leon Skala 1 (320x240) | 7 / 5 x 2 / 506 | **0** (max abs d = 0, bitgleich) | - | 0 |
| Elza Skala 1 | 5 / 4 x 2 / 317 | **0** (bitgleich) | - | 0 |

Aussen (ausserhalb des Fensters) in ALLEN Laeufen 0 (auch bei > 0). Die VORHER-Werte sind bitgleich zur Abnahme L0
(69 / 14 x 8 / 3731, 63 / 16 x 10 / 3960) — Basis bestaetigt. Alle drei Wege (Sprung, CONTINUE, Tuer) liefern dasselbe Bild.
Elza "> 0" 8 x 6: der eine Zusatzpunkt (Zeile 241, x 411) hat |d| = 26 (unter 30, ~9 je Kanal, nicht wahrnehmbar);
VORHER hat "> 0" dieselbe Huelle 16 x 10.

**Augenschein** (selbst angesehen; Bilder nur im Scratchpad: `...\scratchpad\m0\schluessel_s3_nah_x10.png`,
`leon_s3_pinnwand_x4.png`, `elza_s3_pinnwand_x4.png`): Leon vorher heller Bart + dunkler Kopf (14 x 8), nachher ein heller
3 x 2-Fleck mit dunklem 1-Pixel-Strich rechts (4 x 3) — als Schluessel nur noch zu ahnen, aber an der Stelle der Marke
sichtbar; Elza vorher Ring + Schaft + Bart (16 x 10), nachher dieselbe Form halb so gross (Ring, Schaft, Bart erkennbar).
Beide deutlich kleiner als die Zettel der Pinnwand.

**Aufnahme** (CONTINUE, Spielstand (1300,-2900) Blick -z, Skript `W1,A0.1,W3.5,A0.1,W4,A0.1,W6`, Skala 1): Leon
`[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)`, danach `[msgtext] room=10f0 id=4 "Several files from past
cases.Nothing useful here."`; Elza (`PL04`) `[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)`, `Prison Key genommen
-> Flag (4,0xEC) = 1 (Zelle ROOM1211)`, msg 4. Aufheben unveraendert.

**Zweite Kamera auf die Pinnwand (Cut 6, Nebenwirkung derselben Daten)**, Sprung wie oben mit `RE15_FORCE_CUT=6`, Fenster
x 76s..103s+s-1 / y 62s..81s+s-1: Skala 3 Leon 126 Px / 20 x 11 / 6226 -> **12 Px / 6 x 5** / 524 (30 % / 45 %), Elza
119 / 25 x 11 / 6542 -> **23 / 12 x 5** / 878 (48 % / 45 %); Skala 1 Leon 11 / 6 x 3 / 458 -> 2 / 2 x 1 / 111, Elza
13 / 8 x 3 / 646 -> 3 / 2 x 2 / 94 (= Dossier-Angabe "Cut 6 Park 2 / 111, Prison 3 / 94"); aussen 0.

### 3.2 GTE-Raster (Mechanismus der Spur) an der exe
Aus meinem Lauf von `integration_r39_schluessel2` (RE15_TRILOG im echten Lade-Weg): `R_park: GTE-Viereck x 137..139 y 80..81
(2 x 1)`, `R_prison: x 136..139 y 80..82 (3 x 2)`. GEGENPROBE: dasselbe Skript mit der Basis-exe (`-DRE15_PC_EXE=...re15_pc_m0abn_vor.exe`,
sonst gleiche Argumente) -> `R_park: x 135..141 y 79..82 (6 x 3)`, `R_prison: x 135..141 y 79..83 (6 x 4)`, `H_leon: 69 Pixel ...
Huelle x408..421 y239..246`, `H_elza: 63 Pixel ... x408..423 y239..248` -> **FEHLER** (rc 1). Der neue Riegel misst also den
Unterschied (6 x 3 -> 2 x 1, 6 x 4 -> 3 x 2) und faellt mit dem alten Stand.

### 3.3 Urteil Punkt 1: **erfuellt**
In der Fenstergroesse, in der der Nutzer spielt (Hauptbaum-debug.log, Sitzung 20:53 ohne Testhaken, Start ROOM1240:
`[window] windowed 960x720`; Fensterwahl
render_pc.c:625-631 = groesste ganze Skala <= 90 % des Bildschirms), ist der Parking Lot Key (Leon) **4 x 3 statt 14 x 8**
(29 % / 38 %, Flaeche 9 statt 69 Pixel) und der Prison Key (Elza) **8 x 5 statt 16 x 10** (genau 50 % / 50 %, 16 statt 63
Pixel) — gemessen gleich ueber Sprung, Spielstand + CONTINUE und echten Tuer-Eintritt, Cut 5 und Cut 6, bei 1280x960 ebenso
(<= 50 %). Lage/Drehung/Aufhebe-Zone unveraendert, Aufnahme Leon + Elza funktioniert. "Maximal halb so gross" ist eingehalten.
Grenzfaelle siehe Hinweise H2/H3 (kleinere Fenster).

## 4. Tests (selbst gelaufen, KEINE volle Suite)
* `ctest -R "^(unit_r39_schluessel2_.*|integration_r39_schluessel2)$" -V` -> **100% tests passed, 0 tests failed out of 4**
  (halb 10/10, raster 12/12, konst 8/8 Pruefungen; integration 60,9 s, Messzeilen wie Dossier: H_leon `9 Pixel, SUMDL 403,
  Huelle x413..416 y241..243`, H_elza `16 Pixel, SUMDL 748, Huelle x410..417 y242..246`).
* Beruehrte Gruppen (Regex des Bau-Agenten, nachgeprueft per `ctest -N | grep` + `grep -rl itemorte/R38K/R39L/R39M/10F1` in
  tests/engine/platform: Gen-Daten -> r38_itemorte, r39_optik_schluessel, r36_schluessel, r38_cursor (E/F Pinnwand),
  r37_feuerloescher; Raum 10F0/10F1 -> r35_cut10f0, r22_10f0, r36_klettern_10f0; Namen -> item_name_census):
  `ctest -j 2 --timeout 1200 --output-on-failure -R "^(unit_r39_schluessel2_.*|integration_r39_schluessel2|unit_r39_optik_schluessel|
  integration_r39_optik_schluessel|unit_r38_itemorte_.*|integration_r38_itemorte|unit_r36_schluessel_.*|integration_r36_schluessel.*|
  unit_r38_cursor_.*|integration_r38_cursor|unit_r35_cut10f0_.*|integration_r35_cut10f0|unit_r22_10f0_figur|unit_r36_klettern_10f0_.*|
  unit_r37_feuerloescher_.*|integration_r37_feuerloescher|integration_item_name_census)$"` -> **100% tests passed, 0 tests failed
  out of 45** (1090,7 s unter Last paralleler Agenten; integration_r38_itemorte 528 s, integration_r37_feuerloescher 525 s).
* Nachgezogene Alt-Riegel gelesen: Lockerungen (Skala-1-Mindestwerte 0, B3V entfaellt, Mindestpixel Skala 3 neu) folgen
  direkt aus der Messung "Skala 1 bitgleich" und sind im Kopf je Datei begruendet; `k_kontrast` mit Mindestwert 0 prueft
  weiter AUSSEN == 0. Kein Riegel wurde auf "immer gruen" gestellt; die Sichtbarkeit bei 960x720 sichern B4/B5,
  integration_r39_optik_schluessel und integration_r39_schluessel2 (>= 5 / 9 Pixel).
* Riegel-Form: parallel-fest (exe-Kopie `re15_pc_r39m_schl2_<zufall>` mit eigenem Praefix, eigener WORKDIR je Lauf, RE15_*-
  Umgebung des Aufrufers geloescht), Linux-fest (`cmake_minimum_required` im -P-Skript, `m` fuer nicht-WIN32 gelinkt,
  Endung der exe aus der Quelle, kein Backslash-Pfad). Registrierung nur in `tests/unit/probes/r39_schluessel2.cmake`
  (tests/unit/CMakeLists.txt:4060 globt probes/*.cmake).

## 5. Vertrag
* Pfade (`git diff master...HEAD --name-only`): nur analysis/befunde_runde39/M_*, gen/itemorte_r38_{lage,md1}.inc (erzeugt),
  tools/r38_itemorte/itemorte_export.py, Tests (neu + 4 nachgezogene). Kein release/, platform/android/, shared_assets/PSX/,
  keine CMakeLists, keine gemeinsame Datei aus VERTRAG 1.3 beruehrt, kein Engine-/Plattform-Code.
* Keine Bank-9-Bits, keine Nachrichten-Ids, kein neuer Slot (Slot 8 unveraendert), keine Sprachdatei, kein Asset.
* Beide Szenarien geprueft (Leon ROOM10F0 PL00, Elza ROOM10F1 PL04, je Sprung / CONTINUE / Tuer).
* Das skalierte MD1 wird nur ueber `re15_schluessel_r36_md1_bytes` (main.c:1577, Raum-Prop-Lader 10F0/10F1) genutzt; das
  Inventar nutzt ITPS-Bilder, kein MD1 -> keine Nebenwirkung auf Inventar/Pruefansicht.

## 6. Maengel
Keine sperrenden Maengel.

## 7. Hinweise (nicht sperrend, fuer Orchestrator/Nutzer)
* **H1 (Dossier-Fundstelle):** M_schluessel2.md Z. 151 belegt die Bildmitte 160/120 mit "`const int cx = 160, cy = 120`
  main.c:2077" — das ist `pselect_render_model` (Spielerauswahl, Funktion ab main.c:2045). Der Prop-Zeichner (main.c:10942
  `re15_objpoly_r38_tri_mesh(..., cx, cy, ...)`) bekommt `cx = SCREEN_XRES / 2`, `cy = SCREEN_YRES / 2` aus main.c:5961/5962.
  Wert und Original-Beleg (@0x8001643c/@0x80016444, selbst gelesen) stimmen; nur die Port-Zeile ist falsch zitiert.
* **H2 (640x480):** bei Fensterskala 2 ist der Prison Key 6 statt 11 Pixel breit (55 %, Hoehe 43 %, Flaeche 9 statt 26 Pixel)
  — einen Bildpunkt ueber der Haelfte. Skala 2 ist die automatische Fenstergroesse auf Bildschirmen mit nutzbarer Hoehe
  < 800 px (render_pc.c:627-629, z.B. 1366x768). Der Nutzer spielt bei 960x720; dort genau 50 %.
* **H3 (320x240):** bei Fensterskala 1 sind beide Schluessel unsichtbar (Bilder mit/ohne bitgleich, max |d| = 0; vorher 7 / 5
  Pixel). Der Bau-Agent hat das offen gemeldet. Folge fuer Messungen nach VERTRAG §2 ("Framedump RE15_WINDOW_SCALE=1"):
  Schluessel-Sichtbarkeit nur noch ab Skala 2 messbar.
* **H4 (Parking Lot Key ~1/3):** bei 960x720 4 x 3 Pixel, als Schluessel nur noch zu ahnen. Grund (selbst bestaetigt ueber
  TRILOG + Werkzeug): an dieser Stelle springt das ganzzahlige GTE-Viereck von 2 x 1 direkt auf 2 x 2 (Hoehe 2/3 von 3) —
  "halb so gross" (3 x 1) gibt es bei unveraenderter Lage nicht. Der Hebel `NUTZER_FAKTOR_M` liefert die naechste Stufe erst
  ab 2/3 und waere dann in der Hoehe ueber der Haelfte (gemessene stetige Variante: 4 x 6 bei 960x720). Ob der Nutzer 1/3
  akzeptiert, ist seine Entscheidung (steht im "Fuer den Nutzer"-Teil des Dossiers).
* **H5 (Messschwelle):** mit "jede Abweichung > 0" statt "> 30" ist die Elza-Huelle bei 960x720 8 x 6 (Hoehe 60 %); der
  sechste Punkt (Zeile 241) hat |dR|+|dG|+|dB| = 26 und ist nicht wahrnehmbar. Mit der Schwelle der Abnahmen L0/M: 8 x 5.
* **H6 (Integration):** Spur I (funk10f1, Szene in ROOM10F1) ist nicht in diesem Zweig; falls deren Riegel Bilder der Pinnwand
  10F1 vergleichen, aendert sich dort der Prison Key (GTE 6 x 4 -> 3 x 2).

## 8. Ergebnis
Punkt 1 **erfuellt**; RE-Gate haelt (7 Zitate selbst disassembliert, Faktor als NUTZER-VORGABE gekennzeichnet, Daten
reproduzierbar erzeugt); Vertrag eingehalten; Riegel vorhanden, messend (Gegenprobe mit Basis-exe faellt) und gruen
(4/4 + 45/45 gezielt). **Abnahme 0: bestanden.**
