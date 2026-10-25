# Runde 41 — Spur G "kanister2080" — UNABHAENGIGE ABNAHME 2 (2026-10-25)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, HEAD b9e97e3f (Basis r41/integration 9f10bdb0; letzter
Code-Commit e4121848). Pruefer: Abnahme-Agent 2 (glaubt nichts Ungemessenes). Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).
Laufordner/Bilder (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn`.

## 0. Stand der Abnahme
- [x] Lektuere AUFTRAG (inkl. Nachtraege 5/17/20-26, FG-E1..FG-E5; Nachtrag 27/28 kamen waehrend der Abnahme per 6712fd1d dazu), VERTRAG, Dossier §9, Abnahme 1, Code-Diff 9f10bdb0..HEAD
- [x] Bau (local_build.sh configure + build: `ninja: no work to do`, exe 09:15:21 = Stand e4121848, keine Quelle neuer)
- [x] gezielte Unit-Tests (46/46)
- [x] gezielte Integrations-Tests (9/9)
- [x] RE-Gate Stichproben (Abschnitt 4)
- [x] Messungen P1 / P2 / P3 (Leon + Elza; 27 exe-Laeufe)
- [x] Vertrag
- [x] Urteil: NICHT BESTANDEN (M1: R1-Tod in der Standard-Startlage nicht im Bild; M2: Werfer in Cut 7 beim Suedwurf von hinten; M3: Explosionseffekt fehlt, Nachtrag 28); N1/N2/N3 der Abnahme 1 und Nachtrag 26 (Hals, Kippen) erfuellt

## 1. Diff, Bau
- Massgeblich ist `git diff 9f10bdb0..HEAD` (merge-base; `git diff master` enthaelt master-Stand nach dem Abzweig). Seit Abnahme 1
  (`git diff ebe8445f..HEAD -- re15_port`): 9 Dateien +833/-76 - kanister2080_r41.c (+317), re15_kanister2080_r41.h (+68),
  enemy_ai_boss_gator.c (+10: Include, Export `re15_gator_boss_steuerziel`, CHASE-Haken `if (re15_k2080_r41_anschwimmen(..)) break;`,
  GUARD-Haken `oben = 0`), test_r36_kartenbits.c (k_port[] Bit 117), Riegel (unit szene/landung/kippen/hals + NEU
  test_r41_kanister2080_anschwimmen.c, integration szene/kampf). Kein Eingriff in skeleton_common.c (der Hals haengt sich verkettet
  an `g_anim_bone_angle_hook`).
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. Kontrolle: `find re15_port/{engine,include,platform,tests} -newer re15_pc.exe -name '*.c|*.h|*.inc|*.cmake'`
  leer, `git status` sauber; exe 2026-10-25 09:15:21 (+0100), letzter Code-Commit e4121848 09:14:19 -> die exe ist der Stand HEAD.
  Messkopien re15_pc_g41abn2{,b,c,d}.exe (md5 629cb2a2... = re15_pc.exe).

### 1a Gezielte Unit-Tests (Log <SP>/ctest_unit.log)
`ctest --test-dir re15_port/build -R "unit_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_|unit_msg_|unit_action_msg_gate|unit_alligator_ai|
unit_gator_|unit_g5_boss|unit_r35_granate|unit_r34_reaktion|unit_re2_spider_water_2090" --timeout 600 --output-on-failure -j3` (Regex des Bau-Agenten
+ unit_g5_boss als zweiter Nutzer von g_anim_bone_angle_hook) -> **`100% tests passed, 0 tests failed out of 46`** (61,62 s). Einzeln ausgegeben:
anschwimmen 5 Lagen Aufnahme Bild 131/419/407/424/319, `Biss davor nein`, Gegenprobe ohne Szene `KEINE Aufnahme` (Haken nur fuer die Szene);
hals `Slew 20/Bild bis (560,-312), zurueck auf 0`; kippen `Bild 1 0, Bild 12 1056, Bild 23 4048, Bild 24 4096`; landung Nord/Sued je Lage, Bit 117
in 2090/2091.

### 1b Gezielte Integrations-Tests (Log <SP>/ctest_integ.log; allein gefahren, keine parallelen Messlaeufe)
`ctest --test-dir re15_port/build -R "^integration_r41_kanister2080$|^integration_r41_kanister2080_kampf$|^integration_r41_re2gator$|
^integration_r35_granate$|^integration_r38_wasser$|^integration_r41_wasser20b0_npc$|^integration_r37_itemband$|^integration_r36_tueren$|
^integration_r36_kartenitems_bild$" --timeout 3000 --output-on-failure -j3` (G + B's Riegel + alle Riegel, die durch ROOM2080/2081/2090/2091
laufen, `grep -l "2080\|2081\|2090\|2091" tests/integration/*.cmake`) -> **`100% tests passed, 0 tests failed out of 9`** (1456,97 s;
kanister2080 219,76 s, kanister2080_kampf 749,24 s, re2gator 345,70 s, r37_itemband 874,00 s, r36_kartenitems_bild 523,52 s). Der kampf-Lauf
(build/tests/integration/r41_kanister2080_kampf_wd/lauf_0717c302) hat dieselben Zeilen wie k_leon (3.3), inkl. `R1 EXPLOSION ... -> Cut 2 F826`.

## 2. Messungen P1 / P2 (Laufordner <SP>/runs/<lauf>; Helfer <SP>/lauf.sh, <SP>/kampf.sh)
Gemeinsam: RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore; Leon RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2, Elza
RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1. P1/P2-Code ist seit Abnahme 1 unveraendert (Regression).

### 2.1 P1 Leon, echter Leiterweg 2090 -> 2080 (DOOR16) - Lauf p1_leon
`RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1 RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_KEIN_GATOR=1
RE15_PLAYER_POS=-8900,-25244,2048 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W1.0,A0.2,W2.0 RE15_SPAWN_DIAG=1
RE15_STATE_LOG=state.log RE15_FRAMEDUMP=0-400/20:f_ RE15_EXIT_AT=400#2080` -> rc 0, 36 s. (Bit 110 per RE15_SET_FLAG: Spur F fehlt in diesem Zweig.)
- debug.log Z.156 `[tuer] Sequenz Archiv 2 DOOR16 ...` -> Z.176 `[k2080] F31 Knall: Knall-Bank 19976 B in der Tuerbank, Satz 0 (RE2 ROOM1060
  Se_on 0x0a @0x0F48 ...)` + Z.177/178 `[se] Stimme: se=0 layer=0/1 vag=0 note=67 fine=0/1 center=91 vol=127 (11025 Hz)` -> Z.179 `Bit 114 = 1`
  -> Z.181 `ROOM2080 nach Bit 110: 8 lebende Zombies (Tot-Bits 236..243) ... Band 8` -> Z.189-196 8 x `Sce_em_set type=0x16 behavior=0x02
  slot=0..7 pos=(..,-14400,..)`; Messzeilen `Ebene: 8 Zombies ... y -14400..-14400, auf der Treppe (x < -16650) 0` F30..F390.
- Mitschnitt (`knall_im_mitschnitt.py`, Kopie <SP>/tools_k, RE15_INFO_DIR auf den Hauptbaum): 867 Bilder, `RE2_1060 (11025 Hz): t=15.57 s
  Bild 467 r=0.891`; 867 - 401 (2080-Besuch F0..F400) = 466 -> der Knall liegt im ersten 2080-Bild, vor den Zombies (die stehen ab F1).
  DOOR04/R1030: kein Treffer.
- Bild <SP>/p1_leon_bogen.png (Cut 4, F40..F400): die Zombies stehen oben am Regal/Rechner (oberer Hauptbereich).

### 2.2 P1 Elza, 2091 -> 2081 - Lauf p1_elza
Wie 2.1 mit Elza-Wahl, `RE15_DEBUG_JUMP=2091@250`, `RE15_FRAMEDUMP=0-400/40:f_`, `RE15_EXIT_AT=400#2081` -> rc 0. `[tuer] Sequenz ... DOOR16` ->
`F31 Knall ...` (2 Lagen) -> `ROOM2081 nach Bit 110: 8 lebende Zombies` -> 8 x Sce_em_set 0x16 y -14400. Mitschnitt `RE2_1060 ... Bild 1823
r=0.891` = 2223 - 401 + 1 (erstes 2081-Bild).

### 2.3 P2 Gaskanister an der Reling - Laeufe p2_cut5 / p2_cut5_elza
`RE15_NOAUDIO=1 RE15_DEBUG_JUMP=2080@250 (Elza 2081) RE15_PLAYER_POS=-12000,-24000,0,8 RE15_FORCE_CUT=5 RE15_FRAMEDUMP=40:f40.ppm
RE15_EXIT_AT=45#2080`: je `[k2080] F1 Kanister pos=(-6745,-14400,-17625) Cut 5 Kasten=(159..177,75..158) Dreiecke 46`. Bild
<SP>/p2_gascan_leon_elza_x2.png (links Gas_Can.bmp, Mitte Leon, rechts Elza): der Kanister steht aufrecht auf der roten Marke (Mitte
(167,156)) am Balkonende zwischen den Relingpfosten, bei Leon und Elza gleich.

## 3. Messungen P3 (echter Ablauf; Rezept wie integration_r41_kanister2080_kampf: <SP>/kampf.sh)
Gemeinsam: `RE15_NOAUDIO=1 RE15_DEBUG_JUMP=2090@250` (Elza 2091) `RE15_SET_FLAG=9:110,3:245 RE15_GIVE=3:60 RE15_EQUIP=3
RE15_R39_MESS_SPIELER_HP=100 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W<t>,n x (M0.6,MA0.1,M0.3,MA0.1,W3.0)
RE15_STATE_LOG=state.log RE15_PLAYER_POS=<x,z,gier>`; KEIN RE15_GB_TEST / RE15_GATOR_KANISTER / RE15_KEIN_GATOR / RE15_K2080_WURF_GIER
(lauf.sh loescht sie zusaetzlich aus der Umgebung). Messhaken RE15_K2080_GATOR_HP=24990 (ein echter Treffer statt 43) nur wo "Haken HP" steht.
Bit 110 per RE15_SET_FLAG (Spur F fehlt in diesem Zweig). Zeit "nach dem Loslassen" = 1 Bild bis zum Raumwechsel + Bild im Rueckkehr-Besuch.

### 3.1 Szene 2080/2081 mit Bildern (Laeufe s_leon / s_elza; Start (-6000,-17500,3072), Haken HP, `RE15_FRAMEDUMP=430-630/2:f_ RE15_EXIT_AT=630#2080`)
- debug.log s_leon: `F902 FG-E3: Alligator gesichert Slot 35 (-4889,-1200,-7442) ... hp 24980` -> `F902 AUSLOESER geschwaecht ..., Treffer 1` ->
  `F902 Wurfrichtung 1024 (Suedbecken, Alligator beim Ausloesen (-4889,-7442)) -> Nennlandung (-6720,-22930)` -> `ROOM2080 SZENE (Marvin 0x40,
  8 lebende Zombies, 606 B Programm)` -> `F8 Tuer-Ton DOOR06 Satz 1` -> 8 x `Schuss n: Treffer Slot s ... hp=-1 state=3 zeile=5` + `Kopf ab
  ... n von n` (F118..F352) -> `F465 Kanister aufgehoben (Loco c11 Bild 24 ...)` -> `F465 Kippen Bild 1: Bruch 0/4096` ... `F488 Bild 24:
  4096/4096` -> `F630 Zeile 42 stand F531..F629 (99 Bilder), Stimme aus` -> `F630 Wsleeping frei` (Wurf F631, s. 3.3). s_elza gleich
  (`ROOM2081 SZENE (John 0x49 ...)`, Zeile "John: Hey, Elza, try with this!").
- state.log Slot 9 (Marvin t=40 / John t=49): F437 Lauf (ss1 5) r93 -> F440 mo=11 af=0 r93 (Hinknien) ... af=24 r93 ... F500 af=0 (rueckwaerts,
  Aufstehen) ... F520 af=20 **r93** -> **F530 cam=7 mo=6 r1024** (Dir_set mit dem Schnitt), msg a=1 id=42 F530..F628 = **99 Bilder** (Leon und
  Elza je 99). In Cut 5 aendert sich die Gier nach dem Hinknien in keinem Bild (vorher F465 r3072 mitten in Cut 5).
- Bilder: <SP>/s_leon_griff_f436-530_x3.png (Cut 5 Ausschnitt x3, Helligkeit x1,8): F436-F446 Marvin von hinten an der Reling, F452-F466
  kniet er neben dem stehenden Kanister, F468-F484 kippt der Kanister beschleunigt in seine Haende, ab F488 liegt er quer in beiden Haenden,
  F494-F528 steht er damit auf - immer von hinten, **kein Klettern, kein Profil-Sprung in Cut 5**. <SP>/s_leon_griff_f458-530_x5.png (x5).
- **Apply-Bild F530** (<SP>/s_leon_schnitt_f522-540.png, <SP>/s_elza_schnitt_f526-534.png): F530 zeigt noch den Cut-5-Hintergrund mit dem
  schon gedrehten Werfer (Profil), den Kanister noch in der alten Lage und die Zeile 42; ab F532 Cut 7. Ein Bild lang (33 ms), Leon und Elza.
  Das ist die allgemeine Apply-Bild-Eigenschaft dieses Zweigs bei JEDEM Cut_chg (Basis 9f10bdb0 ohne Spur E); auf master haelt Spur E's
  kamerahalt_r41.c (`entscheiden`: `cam_change_pending` / `work_vars[0x0A] != cam_id` -> HALTEN, @0x80021558/@0x80021560) das vorige Bild.
  Kein Mangel dieser Spur, aber Pruefpunkt der Integration (H1).
- Cut 7 (<SP>/s_leon_cut7_f532-630_x4.png): Marvin stemmt den Kanister ueber die Reling (Eigen c6 af 0 -> 9), haelt ihn waehrend der Zeile;
  die geköpften Zombies liegen auf dem Laufgang.

### 3.2 N1 mit Sprachaufnahme (Lauf stimme_leon; eigene 5-s-Testdatei main42.wav (22050 Hz, 440 Hz) in einer Basis-Wurzel des Laufordners
`<SP>/runs/stimme_leon/synchro/STAGE2/room2080/` - NICHT im Baum; Ton an: `RE15_AUDIO_CAP_SYNC=cap.raw`, sonst wie 3.1, `RE15_EXIT_AT=1000#2090`)
- debug.log Z.472 `[voice] clip loaded: main42.wav (22050 Hz x1 -> 220500 @44100 Hz)` -> `F682 Zeile 42 stand F531..F681 (151 Bilder)` ->
  `F682 Wsleeping frei` -> `F683 Wurf: Start ...`. Mitschnitt: reiner 440-Hz-Anteil (Goertzel je Spielbild, Anteil > 0,6) in **150 Bildern**
  (Mitschnitt-Bild 1686..1835 = 5,0 s = volle Laenge der Datei), der Raumwechsel nach 2090 folgt danach. Damit wartet der Wurf wirklich auf
  das Ende von Zeile UND Stimme (Wsleeping-Form @0x80053ac0: 0x800cfbd8 & 0x20 / 0x800e873c & 0x80); ohne Datei 99 Bilder (3.1).

### 3.3 Wurf, Fall, Aufnahme, R1 - alle Verlaeufe (debug.log [k2080], gator_boss.log, state.log; Auswertung <SP>/runs/*)
| Lauf | Start / Schiess-Beginn | Ausloeser | Wurf -> Aufschlag | Zeile 42 | ANSCHWIMMEN an | Aufnahme (Stufe 1) / angehaengt | Biss davor | R1 / Kamera |
|---|---|---|---|---|---|---|---|---|
| **v1_natur_leon (kein G-Haken)** | (-6000,-17500) / 6 s | **F7375, 37 echte Treffer**, Gator (4664,-24479) | 3072 Nord -> (-7515,-12304) | 99, Wurf F631 | F50, 16925 | **F146 (4,9 s)** / F158 | 0 | F1549, Cut 0 ohne Schnitt, Kopf (150,93) - Tod im Bild |
| **v1_natur_elza (kein G-Haken)** | wie v1 Leon | F7375, 37 Treffer | 3072 -> (-7506,-12276) | 99, Wurf F631 | F50, 16940 | **F146 (4,9 s)** / F158 | 0 | F1549 (Logzeilen gleich) |
| k_leon (Riegel-Startlage) | (-6000,-17500) / 6 s, Haken HP | F902, (-4889,-7442) | 1024 Sued -> (-6821,-22914) | 99, Wurf F631 | F43, 15325 | F113 (3,8 s) / F125 | 0 | F826 -> **Cut 2, Kopf (8,106): Tod NICHT im Bild** (M1) |
| stimme_leon (5-s-Stimme, 3.2) | wie k_leon | F902 | 1024 Sued | 151, Wurf F683 | F43 | F113 / F125 | 0 | F826 -> Cut 2 wie k_leon |
| d15_leon (NEU: erst nach 15 s schiessen) | (-6000,-17500) / 15 s, Haken HP | F1495, (-7297,-7097) | 1024 Sued | 99 | F50, 15781 | F127 (4,3 s) / F139 | 0 | F727, Cut 0 ohne Schnitt, Kopf (296,81) = rechter Bildrand, kaum sichtbar |
| d30_elza (NEU: 30 s) | (-6000,-17500) / 30 s, Haken HP | F962, (-7416,-7068) | 1024 Sued -> (-6830,-22942) | 99 | F43, 15895 | F119 (4,0 s) / F131 | 0 | **kein R1 in 2500 Bildern** (stehender Test-Spieler: 337 Biss-Bilder, Giftkugeln der Spinnen, H3) |
| d45_leon (NEU: 45 s) | (-6000,-17500) / 45 s, Haken HP | F1616, (-7410,-9763) | 1024 Sued | 99 | F50, 12789 | F93 (3,1 s) / F105 | 0 | F2119, Cut 5 ohne Schnitt, Kopf (217,96) |
| n_plat_leon (NEU: Leon auf der Plattform (0,-16000)) | Plattform / 6 s, Haken HP | F2291, Gator (-4491,-22675), Leon (-2338,-15982) | 3072 Nord -> (-7515,-12304) | 99 | F43, 10627 | F82 (2,8 s) / F94 | 0 | F1306, Cut 0 ohne Schnitt, Kopf (286,93) |
| r1_ne_leon (Riegel nordost) | (0,-7000) / 6 s, Haken HP | F876, Treffer 2, Gator (5716,-7090) | 1024 Sued -> (-6821,-22914) | 99 | F43, 19719 | F154 (5,2 s) / F166 | 0 | F899 -> Cut 2, Kopf (189,102): klein, aber im Bild |
| r1_west_elza (Riegel west) | (-8000,-16000) / 6 s, Haken HP | F802, Treffer 4, Gator (-4940,-9716) | 1024 Sued -> (-6830,-22942) | 99 | F43, 13222 | F90 (3,0 s) / F102 | 0 | F814, Cut 0, Kopf (265,93): Explosion + Kiefer klar im Bild |
| n_se_leon / n_ne_elza (NEU) | (5000,-24000) / (5000,-9000) | **kein Ausloeser in 2500 Bildern**: der Alligator lauert unveraendert bei (-7200,-9200) (Spieler > 9000 vom Lauerplatz, Spur B) - nicht verwertbar | | | | | | |
- In allen 8 Verlaeufen mit Szene: `FG-E3: ROOM2090 derselbe Alligator in Slot 35` mit Lage/HP vom Ausloesen, kein `GATOR VERSCHWUNDEN`/
  `RE2 AUFTAUCHEN`/`INTRO ende` danach; zwischen `KANISTER gesetzt` und `KANISTER Stufe 1` keine einzige BISS/LUNGE-Zeile (RE2 Biss-Sperre im
  Zielmodus @0x8010134c-58). Aufnahme 2,8 bis 4,9 s nach dem Loslassen in JEDEM Verlauf, auch v1 ohne jeden Testhaken (Leon und Elza) und in
  vier neuen Kampflagen (d15/d30/d45/Plattform). Hals: `F43 HALS zum Kanister: Part 5 Akku Gier 20 Neigung 14 ...` -> `F114 HALS zurueck
  (Akku -> 0)` (k_leon), Bildvergleich des Bau-Agenten G_belege/nb2_n26_hals_vorher_nachher_leon_f48-78_x3_hell25.png zeigt den anders
  gerichteten Kopf.
- Fall (k_leon, `RE15_FRAMEDUMP=0-1000/2`): state.log Rueckkehr F1 cam=12 (pm 0), F7 pm 2 (gehalten), F58 cam=0; Bilder
  <SP>/k_leon_fall_cut12_f2-58.png / <SP>/k_leon_fall_f28-44_x2.png: F28-F40 faellt der Kanister von oben links (unter der Reling) an der
  Leiter entlang zur Wasserlinie (Suedwurf), F28 schwimmt der Alligator dicht an der Kamera vorbei, F43 `Aufschlag im Wasser bei
  (-6821,-2076,-22914)` + `Aufschlag-Ton KANISTER2080.VBS Satz 0` + `Kanister im Becken (-6821,-1200,-22914) = Aufschlagstelle (Bit 116)`.
- Aufnahme (k_leon) <SP>/k_leon_aufnahme_f60-130.png: Cut 0, der Alligator schwimmt hinten heran, F130 Kanister (gelb) im Maul; im Vordergrund
  greift eine Spinne (0x25) Leon an (Spinnen sind in diesem Zweig noch in 2090, Spur F).
- R1-Tod: im Bild bei v1 (<SP>/r1_v1_leon_f1548-1588_hell2.png: weiss-blauer Schimmer und Brocken am Kopf F1550-F1560 - KEIN Feuerball, M3 -,
  Leiche F1570-F1588), west_elza
  (<SP>/r1_west_elza_f812-852_hell2.png), ne_leon (<SP>/r1_ne_leon_f898-934_hell2.png, klein hinten); **NICHT im Bild in der Riegel-Startlage**
  (k_leon = integration kampf_leon/kampf_elza, Lauf lauf_daccaba3 des Bau-Agenten mit denselben Zeilen `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 2
  F826 ... kopf_bild=(8,106)`): <SP>/k_leon_r1_f818-828_x2.png / <SP>/k_leon_r1_f826-860_hell3.png - F818-F826 Cut 0 mit Alligator + Kanister
  oben rechts, F826 Blitz am rechten Rand, F828-F860 Cut 2: nur Maschine/Rohre, kein Alligator, keine Explosion, kein Kiefer (der Kopf
  liegt bei x 8 unter der Maschine = Vordergrund-Freistellung), F862 zurueck auf Cut 0 mit der Leiche. Ebenso d15_leon: Kopf am rechten
  Bildrand (296,81), vom Tod ist praktisch nichts zu sehen (<SP>/r1_d15_leon_f726-766_hell2.png).

### 3.4 Nachpruefungen (Laeufe k_leon_nopri, kopf_leon, nord_cut7_leon; Rezept 3)
- **R1-Schnitt ohne Freistellungen** (k_leon mit Messhaken `RE15_NO_PRI=1`, `RE15_FRAMEDUMP=820-862/2`; gleiche Zeile `R1 EXPLOSION RE2-Cut 5 -> Cut 2
  F826`): <SP>/k_leon_nopri_r1_f826-860_hell3.png - erst ohne PRI erscheinen in Cut 2 F828-F842 kleine Explosions-/Gore-Teile am LINKEN
  BILDRAND (x 0..30, y 90..110) ueber der Maschine; mit Freistellung (k_leon) ist davon nichts zu sehen. Der Kopf liegt also am Bildrand hinter
  der Maschinen-Freistellung (M1).
- **Kopfschuesse** (kopf_leon, `RE15_FRAMEDUMP=110-200/2 RE15_EXIT_AT=200#2080`): <SP>/kopf_leon_f112-160.png - Cut 2, F118/F152 Schuss,
  F120-F124 / F154-F160 platzt der Kopf (Blutwolke), der Zombie faellt; `Schuss 1: Treffer Slot 6 ... hp=-1 state=3 zeile=5`, `F119 Kopf ab:
  Slot 6 (Part 8 Flagwort 0x4B ...)`.
- **Blickbeziehung Werfer <-> 2080 Cut 7** (Formeln wie tools/r41_kanister2080/blick_wurf.py, eigenes Skript <SP>/blick_gier.py, Werfer
  (-7168,-17609) laut state.log): RE2 ROOM7020 Cut 7 Ada (Dir_set 0x0e74 @0x0748) **frontal 0,99**; Port Nordwurf Gier 3072 frontal 0,45 (63 Grad);
  **Port Suedwurf Gier 1024 frontal -0,45 (117 Grad = von hinten)**; keine 2080-Kamera zeigt Gier 1024 von vorn (alle < 0,5). Bilder
  <SP>/m2_cut7_sued_vs_nord_x6.png (oben s_leon Suedwurf F532-F600: Ruecken, Kanister auf der abgewandten Seite; unten nord_cut7_leon Nordwurf:
  schraeg von vorn, Kanister vor ihm). Der Suedwurf ist in 6 von 8 Verlaeufen dieser Abnahme gewaehlt (Alligator beim Ausloesen im Norden:
  kampf, d15, d30, d45, ne, west) - der Riegel-Standardfall spricht die Zeile 42 also mit dem Ruecken zur Kamera (M2).

## 4. RE-Gate (Stichproben selbst disassembliert/gelesen; Werkzeuge .claude/skills/re15-psx-disasm/scripts/re2_disasm.py / re15_disasm.py
aus dem Hauptbaum, nur lesend; RDT-Bytes per Python aus info/re2leon/PL0/RDT)
| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| RE2 Wsleep/Wsleeping, Tafel @0x800a74c8 [11]/[12] | `table 0x800a74c8`: [11] -> 0x80053a8c, [12] -> 0x80053ac0. Wsleep: `lb v1,2(a0)` / `lbu v0,8(v1)` / `addiu v0,v0,1` / `sb v0,8(v1)` @0x80053a8c-a4, pc+1, `jr ra / addiu v0,zero,1` @0x80053ab8/bc. Wsleeping: `lw v0,-1064(v0)` (0x800cfbd8) `andi v0,v0,0x20` `bne .. 0x80053b14` @0x80053ac4-d0; `lbu v0,-30916(v0)` (0x800e873c) `andi v0,v0,0x80` `bne .. 0x80053b14` @0x80053adc-e8; Ende `jr ra / addiu v0,zero,2` @0x80053b14/18 | stimmt |
| Xa_on setzt 0x800cfbd8 \| 0x20 @0x80058060; FUN_8002fe38 setzt 0x800e873c = 0x80 @0x8002fe80/88 | Tafel [0x59] @0x800a762c -> 0x80058028; `jal 0x800129b4` @0x80058048, `lw v0,0(v1)` (0x800cfbd8) / `ori v0,v0,0x20` / `sw v0,0(v1)` @0x80058058-64. FUN_8002fe38: `addiu t2,zero,128` @0x8002fe80, `sb t2,-30916(at)` (0x800e873c) @0x8002fe88 | stimmt |
| RE2 ROOM7020 sub12 @0x0726/0x0748/0x079A/0x079B (SCD-main @RDT 0x1128) | 0x0726 `29 07`, 0x073C `2e 02 00`, 0x0740 `32 00 a0 92 b4 e2 14 ba`, 0x0748 `33 00 00 00 74 0e 00 00`, 0x0750 `3f 0a 12 40`, 0x0760 `59 00 00 00`, 0x0764 `2b 00 05 00 00 00`, 0x076A `09 0a 1e 00`, 0x0782 `3f 0a 12 40`, 0x0786 `09 0a 14 00`, 0x078E `49 00 40 fb 40 02 00 04`, 0x0796 `04 ff 18 0d`, **0x079A `0b`, 0x079B `0c`**, 0x079C `11 00 08 00 02 00 12 04 21 05 0e 00`, 0x07A8 `09 0a 0f 00`, 0x07D4 `29 0b` | stimmt (Dir_set direkt nach dem Schnitt; Wsleep/Wsleeping nach dem Loslassen - der Port zieht es vor das Loslassen, Begruendung Raumwechsel, s. 4a) |
| RE1.5 Dir_set @0x80041080 (Tafel @0x800744a8[0x33]) | `table`: [0x33] @0x80074574 -> 0x80041080; `lw a1,340(a0)` @0x80041084, `lhu v0,2(v1)`/`sh v0,104(a1)`, `lhu v0,4(v1)`/`sh v0,106(a1)`, `lhu v0,6(v1)`/`sh v0,108(a1)` @0x80041088-a8, `addiu v1,v1,8` / `sw v1,28(a0)` | stimmt |
| EM23 Zielmodus @0x80100a34-5c, Ziel @0x80100a4c-70, Ende 9000-120 @0x80100a88-8c (EM23_OVL_0000.BIN) | `lbu v0,550(s0)` / `sltiu v0,v0,0x2` @0x80100a34-3c; `lbu v0,448(s0)` / `ori v0,v0,0x4` / `sb v0,448(s0)` @0x80100a48-5c; `lw a0,56(s0)` (eigene x) / `lw v1,60(s0)` / `addiu v1,v1,-800` / `sh a0,368` / `sh v1,370` / `lw v0,64(s1)` / `sh v0,372(s0)` @0x80100a4c-70 (s1 = 0x800d051c @0x80100980-84); `lw v0,1372(0x800d0000)` (= 0x800d055c = s1+0x40) / `lw v1,64(s0)` / `subu` / `addiu v0,v0,-9000` / `slti v0,v0,-120` @0x80100a78-8c; `andi v0,v0,0xfb` @0x80100ab0 | stimmt (RE2: Ziel x = EIGENE x, Ende laengs z; Port: Ziel x = Kanister-x und euklidischer Abstand - als PORT-WAHL der Abbildung benannt) |
| Biss-Sperre @0x8010134c-58 | `lbu v0,448(a0)` / `andi v0,v0,0x4` / `bne v0,zero,0x80101380` | stimmt |
| Hals: Part 5 @0x801001c4-cc, Schritt 20 / Klemmen 560/312 @0x801005a0-c4 | `addiu v0,zero,5` @0x801001c4, `sb v0,449(s0)` @0x801001cc; `addiu v0,zero,20` @0x801005a0, `sh v0,1016(v1)` / `sh v0,1018(v1)` @0x801005a8/ac, `addiu v0,zero,560` / `sh v0,1020(v1)` @0x801005b0/b4, `addiu v0,zero,312` @0x801005b8, `sh v0,1022(v1)` @0x801005c4 | stimmt |
| Kippen ROOM40A0 sub10 (SCD-main @RDT 0x25C0) @0x066E/0x0676/0x0682 | 0x0656 `2e 04 01`, 0x0659 `22 05 01 01`, 0x0666 `34 10 00 00`, 0x066E `2f 0b fc ff` (-4), 0x0672 `2f 00 d8 ff`, 0x0676 `0d 00 06 00 17 00` (For 23), 0x067C `30`, 0x067D `31`, 0x067E `02`, 0x0682 `34 10 00 fc` (-1024), 0x0686 `36 02 0b 01 04 01 ..`, 0x06A2 `2f 00 dc ff`, 0x0484 `32 00 e0 a6 40 f8 e4 da` | stimmt (Winkel(k) = -2k(k-1), Bild 23 -1012, dann -1024 = Port-Logzeilen 0/240/1056/2448/4096) |
| Steuerziel-Klemme -8900+1600 .. 7200-1600 / -27000+1600 .. -5400-1600, Wende 0x300 / Slew 0x60 / 0x30 | enemy_ai_boss_gator.c CHASE Z.1777-1780 (Klemme), Z.1813-1814 (`dlt > 0x300` -> `steer_point(.., 0x60)`), Z.1847 (`0x30`), als "DESIGN" (Nutzer 2026-09-11) der Spur B | stimmt (Uebernahme mit Verweis, keine eigene Zahl) |
| RE2 Add_speed = Weltaddition @0x80055abc-e0 (Grundlage M2) | `lh v1,344(a0)` / `lw a1,340(a0)` / `lh a2,346(a0)` @0x80055ab0-b8, `lw v0,56(a1)` / `addu v0,v0,v1` / `sw v0,56(a1)` @0x80055abc-c8, ebenso +60/+64 mit +346/+348 @0x80055acc-e0 - keine Drehung um die Gier des Objekts | stimmt (RE2 wirft in Weltachsen; Adas Blick @0x0748 ist davon unabhaengig) |
| Notbremse 300 Bilder | scd_vm.c Z.1868 `enum { RE15_VOICE_NOTBREMSE = 300 }`, msg_common.c Z.432 `#define RE15_UNTERTITEL_NACHHALL_DECKEL 300` | stimmt (Port-Schranke mit Verweis) |
- Suche im Code-Diff 9f10bdb0..HEAD -- re15_port nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/geschaetzt/ungefaehr/
  placeholder/platzhalter/approx/vorlaeufig: **0 Treffer**. 17 x PORT-WAHL/NUTZER-VORGABE/MESSHAKEN/DESIGN. Neue getenv nur die Messhaken
  RE15_K2080_WURF_GIER / RE15_K2080_GATOR_HP (env-gegatet, nur PC, kein Spielverhalten).
- 4a Erklaert der Fix den Befund? N1: ja - die Zeile endete vorher, weil der Raumwechsel 1 Bild nach dem Loslassen kam; jetzt pollt das
  Programm vor dem Loslassen (gemessen 99 Bilder ohne, 151 Bilder mit 5-s-Stimme, 3.1/3.2). Die Verlegung des Wartens VOR das Loslassen
  ist begruendet (Raumwechsel beendet Zeile/Stimme; RE1.5 reloziert den RDT-Kopf samt Nachrichtenblock @0x800397fc-834, RE2 CdlSetmode 0xa0
  @0x800130f0 - beides selbst nachgelesen, H6)
  und deckt sich mit dem Wortlaut ("sagt ... Dann schmeisst er"). N2: ja - der Takt dreht nicht mehr, Dir_set kommt mit dem Schnitt
  (gemessen r93 bis F529, F530 r1024); der WERT des Dir_set (Wurf-Gier statt Kamera-zugewandter Gier wie RE2 0x0e74) fuehrt aber zu M2. N3: ja - das Anschwimmen fuehrt den Alligator unabhaengig von der Kampflage zum Kanister (3.3).

## 5. Vertrag
- Bank-9-Bits 114/115/116/117 (Zuteilung G 114-119); Bit 117 neu (Suedwurf), je eine k_port[]-Zeile in test_r36_kartenbits.c (+ include),
  unit_r36_kartenbits gruen. Zone-7-Tot-Bits 236..243 unveraendert seit Abnahme 0.
- Signale Bank 5 (Raum-Scratch) 0x17..0x1F; neu 0x1F (WSLEEP). Andere Bank-5-Nutzer von Bit 31 (`grep game_flag_(set|get)(5,`):
  nur re15_ivy_ai_tick (enemy_ai_common.c Z.14013, Efeu-Gegner, nicht in 2080/2081/2090/2091) - kein Konflikt.
- Nachrichten-Id 42 in 2080/2081 (Zuteilung 42-47); AOT-Slot 21 fuer die Szenen-Tueren (unveraendert).
- Pfade (`git diff 9f10bdb0..HEAD --name-only`): nur G-eigene Dateien + Haken; keine release/, platform/android/, shared_assets/PSX/, pri/,
  synchro/, keine Test-CMakeLists (probes/r41_kanister2080.cmake = Spur-Registrierung). AUFTRAG.md/VERTRAG.md = vom Orchestrator gemergt.
- Gemeinsame Dateien: scd_room_setup.c 1 Zeile, scd_vm.c 1 Zeile, main.c 2 Zeilen (seit Abnahme 1 unveraendert); enemy_ai_boss_gator.c
  10 Zeilen (Include, 5-Zeilen-Export re15_gator_boss_steuerziel, 2 Haken-Zeilen + Kommentare), alle "Runde 41 Spur G" markiert - gedeckt
  durch FG-E3 ("kleine, markierte Haken ... Landepunkt/Fenster") und FG-E5 ("kleine markierte Haken/Funktionen in Spur B's Modulen").
  Kein Eingriff in skeleton_common.c; g_anim_bone_angle_hook wird verkettet (s_hals_vorher; G5 setzt den Haken in STAGE5 neu - getrennte
  Raeume).
- Tests je Punkt: unit lage/spawn/szene/start/bahn/texte/ebene/landung/kippen/hals + anschwimmen (echter Boss-Takt, 5 Lagen + Gegenprobe),
  integration kanister2080 (ankunft/ebene/kanister/szene_leon/szene_elza: N1 >= 97 Bilder + Folge ZWR, N2 Gier in Cut 5, Kipp-Folge) und
  kanister2080_kampf (4 Laeufe: Wurfrichtung, ANSCHWIMMEN, HALS, Aufnahme, R1). Parallel-fest (eigene Laufordner, exe-Kopien
  re15_pc_r41g(k)_<id>), Linux-fest (m, cmake_minimum_required). LUECKE: kein Riegel prueft, ob der R1-Tod im Bild ist (M1), und keiner die
  Wartezeit mit einer laengeren Stimme (3.2 nur hier gemessen).

## 6. Urteile (gegen den Wortlaut, AUFTRAG.md inkl. Nachtraege 5/17/20-28 und Entscheide FG-E1..FG-E5)
| Punkt | Urteil | Beleg |
|---|---|---|
| P1 erst Metalltuer-Sound (N17: Knall wie nach Irons), dann 8 gespawnte Zombies oben im Hauptbereich (N20) | **erfuellt** | 2.1/2.2: echter Leiterweg DOOR16, Leon + Elza: Knall-Bank Satz 0 (2 Lagen, Mitschnitt RE2_1060 r 0,891 im ersten 2080/2081-Bild) VOR 8 x Sce_em_set 0x16 y -14400, keiner auf der Treppe; Bild p1_leon_bogen.png |
| P2 Gaskanister am Rand der Reling (Gas_Can.bmp) | **erfuellt** | 2.3: Cut 5, Kasten (159..177,75..158) auf der roten Marke (167,156), Leon + Elza |
| P3 "oft genug angeschossen" -> Szene (N22: RE2-Schwelle 300) | **erfuellt** | v1 Leon + Elza ohne jeden G-Haken: F7375 nach 37 echten Treffern |
| P3 erneut schliessende Tuer, Marvin/John im Raum, 8 Kopfschuesse (Kopf platzt, tot) | **erfuellt** | 3.1: `Tuer-Ton DOOR06 Satz 1` F8 (N22 "Stahltuer-Ton lassen"), Marvin 0x40 / John 0x49, 8 x `hp=-1 state=3 zeile=5` + `Kopf ab ... 8 von 8`, in allen Laeufen |
| P3 / N24b "rennt hin, nimmt ihn auf in beide Haende" (Abnahme 1 N2: Gier-Sprung) + N26 "RE2-Kippen" | **erfuellt** | 3.1: Lauf (Sub 5) -> Hinknien Loco c11 -> Kanister kippt in 23 Bildern (Bruch 0/240/1056/2448/4096 = RE2 ROOM40A0 sub10) in beide Haende -> Aufstehen; Gier r93 in Cut 5 unveraendert, kein Profil, Leon + Elza (s_leon_griff_*, s_elza_griff_*). Apply-Bild F530 = Zweig-Eigenschaft ohne Spur E (H1), Kanister im Aufstehen in der Huefte (H2) |
| P3 sagt "Hey, Leon/Elza, try with this!" (Abnahme 1 N1) | **erfuellt** | 3.1/3.2: Zeile 99 Bilder (volle Dauer), mit 5-s-Stimme 151 Bilder und Stimme ungekuerzt (150 Bilder im Mitschnitt), Wurf erst danach |
| P3 Wurf die Reling runter, liegt in 2090 an der Stelle im Wasser (FG-E3/FG-E4, N26: Kamera 12) | **erfuellt** | 3.3: RE2-sub13-Bahn, Fall in 2090 Cut 12 (k_leon_fall_cut12_f2-58.png), Aufschlag + Ton, `Kanister im Becken ... = Aufschlagstelle`; Nord-/Suedwurf nach der Lage des Alligators |
| N5 / FG-E4 "GENAUSO ... von der Kamera" wie Adas Wurf (RE2-Schnittfolge: Cut 7 mit dem Werfer frontal, dann der Fall in 2090 Cut 12) | **teilweise** | Schnittfolge und Cut 12 wie entschieden (N26); aber beim Suedwurf (6 von 8 Verlaeufen, auch Riegel-Standard) steht der Werfer in Cut 7 mit dem Ruecken zur Kamera (frontal -0,45; RE2 0,99, Nordwurf 0,45) - Mangel M2 (3.4) |
| P3 / N23 / N26 "wenn der Aligator dran vorbei kommt, ... aufnehmen, GENAUSO wie RE2" (Abnahme 1 N3), RE2-Lenkung von Hals und Schwimmweg | **erfuellt** | 3.3: Aufnahme 2,8-5,2 s nach dem Loslassen in JEDEM Verlauf mit Szene (10 Laeufe in 8 Verlaeufen, davon 4 neue: d15/d30/d45/Plattform; v1 Leon + Elza ohne jeden Testhaken), kein Biss davor, derselbe Alligator (FG-E3), Hals-Akku zum Kanister und zurueck |
| P3 "Sobald er den Kanister im Maul hat, soll man auf ihn schiessen koennen" | **erfuellt** | R1 nach einem Treffer in 9 von 10 Laeufen; d30_elza ohne R1 in 2500 Bildern (stehender Test-Spieler unter Biss/Spinnen, H3) |
| P3 "die Sterbe Animation mit geplatzten Kiefer etc. ... genauso wie im Original" | **teilweise** | R1-Clip 9 + Leiche laufen ab (Spur B), im Bild bei v1, west, ne (klein), Plattform, d45; **in der Riegel-Startlage (Leon + Elza) zeigt der R1-Schnitt Cut 2 keinen Alligator** (Kopf (8,106) unter der Maschine), bei d15 liegt der Kopf am Bildrand (296,81) - Mangel M1; **der Explosionseffekt des Gaskanisters fehlt** (nur Schimmer; Nutzer Nachtrag 28 an genau diesen Laeufen) - Mangel M3 |
| RE-Gate | **haelt** | 4: 11 Zitatgruppen selbst disassembliert/gelesen, alle stimmen; 0 Rate-Woerter; Abweichungen von RE2 (Warten vor dem Loslassen, Ziel-x, euklidische Grenze, Kipp-Achsen, Anschwimm-Lenkung) benannt und begruendet |
| Vertrag | **haelt** | 5 |
| Tests | gruen: 46/46 Unit, 9/9 Integration (1a/1b) | LUECKE: kein Riegel fuer "Tod im Bild" (M1) und fuer die Blickbeziehung in Cut 7 (M2) |

## 7. Maengel (nummeriert, nachpruefbar)

**M1 (P3 "die Sterbe Animation mit geplatzten Kiefer ... soll genauso kommen wie im Original") - in der Standard-Startlage ist der Tod nicht
im Bild.** Lauf k_leon (= Startlage der Riegel kampf_leon/kampf_elza, (-6000,-17500), Rezept 3) und der Riegel-Lauf des Bau-Agenten
`re15_port/build/tests/integration/r41_kanister2080_kampf_wd/lauf_daccaba3/{kampf_leon,kampf_elza}/gator_boss.log`: `RE2 KAMERA-KANDIDAT EXPLOSION
stufe=2 Cut 2 kopf_bild=(8,106) massstab=614 frontal=4080` -> `RE2 KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 2 F826 ...` -> `KAMERA R1 zurueck -> Cut 0
F862`. Bilder <SP>/k_leon_r1_f818-828_x2.png und <SP>/k_leon_r1_f826-860_hell3.png: F818-F826 sieht man in Cut 0 den Alligator mit dem Kanister
oben rechts, F828-F860 (die ganze R1-Explosion/Kiefer-Phase, 34 Bilder) zeigt Cut 2 nur Maschine und Rohre - kein Alligator, keine Explosion,
kein Kiefer (der Kopf liegt bei x 8 unter der Maschine = Vordergrund-Freistellung ROOM2090); F862 liegt schon die Leiche. Zusaetzlich d15_leon
(`Sperre ohne Schnitt: Kopf im Bild von Cut 0 bei (296,81)`): Kopf am rechten Bildrand, vom Tod ist praktisch nichts zu sehen
(<SP>/r1_d15_leon_f726-766_hell2.png). Vorher (Nachbesserung 1, Abnahme 1 g1/k_leon_r1.png) zeigte dieselbe Startlage den Tod in Cut 5; erst
der Suedwurf der Nachbesserung 2 verlegt den R1 dieser Startlage in die verdeckte Lage, und kein Bild der Nachbesserung 2 zeigt den R1 (nur
nb1_fg3_r1_leon_f566-630.png aus dem alten Ablauf). Ursache: die R1-Kamerawahl der Spur B (gator_re2_r41.c gr2_kam_r1_start Z.877-891 / re15_gr2_kamera_wahl Z.759-797: geschnitten wird nur,
wenn der Kopf ausserhalb des laufenden Cuts liegt - Cut 0 `kopf_bild=(335,86) draussen`; gewaehlt wird dann der Kandidat mit dem besten
Frontal-Winkel, dessen Kopfpunkt `gr2_im_bild` ist - Cut 2 |f-soll| 34) prueft weder Randabstand noch Vordergrund-Freistellung; der Riegel kampf prueft nur Logzeilen (`R1 ende (Leiche)`). Forderung: der
R1-Tod ist im G-Ablauf in allen gemessenen Verlaeufen sichtbar (Kopf/Explosion im Bild, nicht unter einer Freistellung, nicht am Rand) -
Ort der Behebung per Orchestrator (Querbezug Spur B: Kamerawahl; oder Spur G), Messung Leon + Elza mindestens Riegel-Startlage, d15, v1 mit
Bildbogen der R1-Phase, Riegel "Tod im Bild" (z. B. projizierter Kopf mit Randabstand und ohne PRI-Deckung im R1-Schnitt).

**M2 (Nachtrag 5 / FG-E4 "Kamera wie Adas Wurf"; Nebenwirkung der Nachbesserung 2) - beim Suedwurf spricht der Werfer die Zeile 42 mit dem
Ruecken zur Kamera.** state.log s_leon/s_elza: F530 cam=7 Slot 9 r1024 bis F630; Blickbeziehung (3.4) 2080 Cut 7 frontal -0,45 (117 Grad) statt RE2
ROOM7020 Cut 7 frontal 0,99 (Ada, Dir_set `33 00 00 00 74 0e 00 00` @0x0748) bzw. 0,45 beim Nordwurf; Bild <SP>/m2_cut7_sued_vs_nord_x6.png. Ursache:
das Dir_set der Nachbesserung 2 schreibt die WURF-Gier (1024/3072, re15_k2080_wurf_gier_wahl) - RE2 setzt mit Dir_set @0x0748 dagegen die Gier,
mit der Ada der Kamera zugewandt ist, und wirft in Weltachsen (Add_speed = Weltaddition @0x80055abc-e0), unabhaengig von ihrer Blickrichtung.
Die Kopplung "Blick = Wurfrichtung" ist PORT-WAHL; mit dem neuen Suedwurf kehrt sie in der Mehrzahl der Kaempfe den Werfer von der Kamera ab.
Im Dossier (9.2/9.3/9.7) nicht benannt; der Nutzer hat in Nachtrag 26 Frage 2 zwischen "2090 Kamera 12" und "2080 Kamera 7 (frontal)" gewaehlt -
Kamera 7 ist beim Suedwurf nicht mehr frontal. Forderung: Entscheid Orchestrator/Nutzer; RE2-Form waere: Gier des Werfers in Cut 7 der Kamera
zugewandt (wie Ada @0x0748), Wurfbahn in Weltrichtung Nord/Sued (der Wurf selbst faellt ohnehin erst in 2090 Cut 12, FG-E4); Messung Leon + Elza,
Nord- und Suedwurf, Bildbogen Cut 7 F530-F630, Riegel auf die Blickbeziehung.

**M3 (P3 "Sterbe Animation ... genauso wie im Original" / Nachtrag 28, 2026-10-25 11:58, waehrend dieser Abnahme in den Baum gelegt) - der
Explosionseffekt des Gaskanisters fehlt.** Wortlaut: "Beim Ausschnitt gerade eben fehlte noch der Explosionseffekt vom Gaskanister vom Original."
Der Nutzer sah dabei die Kampflaeufe DIESER Abnahme (Messkopien re15_pc_g41abn2c/d). Selbst gemessen: r1_v1_leon F1548-F1588
(<SP>/r1_v1_leon_f1548-1588_hell2.png) und r1_west_elza / r1_d45_leon / r1_plat_leon zeigen am Kopf nur einen kleinen weiss-blauen Schimmer und die
Kiefer-Brocken, keinen Feuerball/Explosionsbild; debug.log r1_v1_leon Z.24 `[re2fx] RE2 CORE00.ESP 8572 B -> re2fx_register_core rc=0`, Z.26
`[glas1120] GLAS1090.ESP 3092 B -> re2fx_register_raum rc=0` - die Raum-ESP des RE2-Alligatorraums ROOM40A0 ist nicht registriert (deckt sich mit
der Orchestrator-Messung im Nachtrag). Forderung: der Auftrag an Spur G im Nachtrag 28 (RE2-Beleg der Effekte, Ursache, 1:1-Nachbau, Bildstreifen
gegen RE2-Referenz, Riegel "Effekt-Bank in 2090 registriert + gezeichnete Explosions-Quads > 0"); zusammen mit M1 messen (der Effekt muss im
gewaehlten R1-Schnitt auch zu sehen sein).

## 8. Hinweise (kein Mangel dieser Spur bzw. klein)
- H1 Apply-Bild F530 (3.1): ein Bild Cut-5-Hintergrund mit gedrehtem Werfer, Kanister in alter Lage und Zeile 42 - bei JEDEM Cut_chg dieses Zweigs
  (op_cut_chg setzt `cam_change_pending = 1`, scd_vm.c Z.1530); master hat Spur E's kamerahalt_r41.c (`entscheiden`: pending -> HALTEN,
  @0x80021558/@0x80021560). Bei der Integration mit RE15_KAMERAHALT_LOG pruefen, dass F529 stehen bleibt (Rezept 3.1).
- H2 Beim Aufstehen (F494-F528) liegt die Kanister-Mitte auf der Mitte der Haende = Hoehe der Huefte; von hinten (Cut 5) wird der Kanister ueber
  Huefte/Ruecken gezeichnet (<SP>/s_leon_griff_f458-530_x5.png F526/F528, John gleich) - wirkt, als halte er ihn hinter dem Ruecken. PORT-WAHL der
  Haltelage (RE2 hat keine Vorlage); bei 1x klein.
- H3 d30_elza: Aufnahme F119, danach 2500 Bilder kein R1 - der stehende Test-Spieler wird 337 Bilder gebissen, Giftkugeln der Spinnen (0x25, in
  diesem Zweig noch in 2090, Spur F) und die Zielhilfe auf die Spinnen. Nach der Integration von F nachmessen.
- H4 n_se_leon (5000,-24000) / n_ne_elza (5000,-9000): der Alligator taucht nicht auf (Lauerplatz (-7200,-9200), Spieler > 9000 entfernt, Spur B);
  in diesem Zweig ersetzt die fehlende 2090-Szene der Spur F den Kampfbeginn - kein G-Befund.
- H5 Nachtrag 26 "... sonst FUN_80017d98": der normale RE2-Halsmodus (Hals auf Leon) bleibt OFFEN (Spur B §11.3); G fuehrt den Akku nach dem
  Zielmodus auf 0 (SELBST-Zweig) - Hals zum Kanister ist umgesetzt.
- H6 Zitat RE1.5-Raumlader @0x800397fc-834 selbst nachgelesen: Schleife `addiu v1,v0,8` / `lw a0,0(v1)` / `addu v0,a0,v0` / `sw v0,0(v1)` bis +96
  ueber den RDT-Kopf (0x800ac778); RE2 @0x800130d4/@0x800130f0 CdlPause 9 / CdlSetmode 14 mit Modusbyte @0x8009a429 = `a0` (selbst gelesen).
- H7 Aufraeumen: Messkopien re15_pc_g41abn2{,b,c,d}.exe nach der Abnahme geloescht; Laufordner/Bilder nur im scratchpad g2abn; im Baum nur dieser Bericht.
