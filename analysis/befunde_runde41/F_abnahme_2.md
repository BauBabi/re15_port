# Runde 41 — Spur F "gatorszene": Abnahme 2 (unabhaengig)

Stand: Zweig `r41/gatorszene` HEAD `5c99bba8` (Basis r41/integration `9f10bdb0`, Abnahme 1 auf `a9472f02`), Baum
`.claude/worktrees/r41_gatorszene`. Pruefer: Abnahme 2 (2026-10-25). Ich glaube dem Bau-Agenten nichts, was ich nicht selbst
am gebauten Stand gemessen habe. Abgeschlossen 2026-10-25.

## 0. Bau und Lektuere

- Gelesen: AUFTRAG.md (bis Entscheid F-E1 2026-10-25 02:51), VERTRAG.md, Dossier F_gatorszene.md (Abschnitt Nachbesserung 2),
  F_abnahme_1.md, `git log master..HEAD` (9 Commits seit Abnahme 1), `git diff a9472f02..HEAD` (Nachbesserung 2: 15 Dateien,
  +579/-82), `git diff master...HEAD --stat` (F gesamt seit der Merge-Basis 9f10bdb0: 51 Dateien). `git diff master --stat`
  (zwei Punkte) zeigt zusaetzlich Loeschungen von Dateien, die master NACH der Basis bekam (H/E/C-Dokumente, kollision_r41.c,
  rennstopp_r41.c, werkstatt11b1_r41.c, synchro/.../main46.wav ...) - das sind keine Aenderungen von F.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to
  do.` / `=== LOCAL-BUILD-OK (build)`; Arbeitsbaum sauber (`git status --short` leer) -> die exe ist der HEAD-Stand.

Messordner (scratchpad, nicht im Repo):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\F41a2`
(`runs/<lauf>/debug.log|state.log|fd_*.ppm`, Helfer `tools/lauf.sh` = eigene exe-Kopie `re15_pc_f41a2_<lauf>.exe`, `timeout`,
danach geloescht). Alle Laeufe mit `RE15_WINDOW_SCALE=1`, Bilder nur per `RE15_FRAMEDUMP`.

## 1. Gezielte Tests

Regex = die Regex des Bau-Agenten (213 Tests, `ctest -N -R` zaehlt 213) - sie enthaelt die Riegel der Spur
(`unit_r41_gatorszene_*`, `integration_r41_gatorszene_*` inkl. der neuen `_tod_r1`/`_tod_r1_elza`), Spur B
(`unit_r41_re2gator_*`, `integration_r41_re2gator`), alle Kamera-/Zonen-Riegel (`unit_camera_view`, `unit_cut_old_displayed`,
`unit_r34n_b_cut_old`, `prop_zone_dispatch`, `prop_scd_pc_advance`, `unit_r38_montage_kameras`), Wasser, Tueren, Inventar/Item,
Spinnen, alle Laeufe durch 2090/2091/20A0 (Abnahme 1, `grep -rlE "209[01]|20[aA][01]|GB_TEST|GATOR_KANISTER|gatorszene|re15_gr2_"
re15_port/tests` gegen `ctest -N` abgeglichen) und die drei Fremd-Riegel von M1. Datei `F41a2/regex.txt`.
Befehl (PATH wie local_build.sh, msys64 zuerst):
`ctest --test-dir "<baum>/re15_port/build" -R "$(cat F41a2/regex.txt)" -j4 --timeout 1200 --output-on-failure`
Ergebnis (08:20-08:38, parallel zu zwei eigenen exe-Laeufen): **`100% tests passed, 0 tests failed out of 213`**
(`Total Test time (real) = 1131.01 sec`, Log `F41a2/ctest_all.log`). Darin u. a.: `integration_r37_itemband` Passed 943.82 s,
`integration_r38_wasser` Passed 223.77 s, `integration_r41_wasser20b0_npc` Passed 255.05 s (die drei Fremd-Riegel von M1),
`integration_r41_re2gator` Passed 312.90 s, `integration_r41_gatorszene_leon/_elza` 64.40/88.50 s, `_tod/_tod_elza`
81.20/123.13 s, `_tod_r1/_tod_r1_elza` 54.23/99.67 s, `integration_r36_tueren` 67.70 s, `integration_r35_granate` 197.16 s,
`integration_r38_wasserzombie` 136.67 s, `integration_r38_rolltor20a0` 318.69 s, `unit_r30_tuer_verschlossen` 17.16 s,
`unit_r36_tueren_schott`, `unit_r38_spinnen`, alle `unit_r41_gatorszene_*` Passed.
Die Behauptung des Bau-Agenten (213/213) ist damit bestaetigt.

## 2. Punkt 1a — Ankunftsszene (Leiter 2080 -> 2090; Wortlaut + Nachtraege 6/17/20/21) - Regression nach Nachbesserung 2

Nachbesserung 2 beruehrt die Ankunft nur ueber den Umbau `kam_uebergabe()` -> `cut_chg_zwilling(cut)` und `s_einst_slot = -1`
im Installer (Diff gelesen). Gemessen wie Abnahme 1:

- L1 Leon: `tools/lauf.sh L1 600 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080
  RE15_EXIT_AT=700#2090 RE15_FRAMEDUMP=300-700/2:fd_ RE15_AUDIO_CAP_SYNC=audio.raw` -> rc=0, 62 s. debug.log:
  `[aot] DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,hh=500) target_cut=0 spawn=(-8200,0,-25550)` (Leiter Slot 1 @0x00BC2),
  `ROOM2090 Slot 1 (Tor) gesperrt: Ereignis 19, Bit (9,113) = 0`, Folge `A F40` -> `P 1` -> `T F98 wert=0/1` -> `P 2 ROOM2080
  F142` + `M ROOM2080 F142 wert=8/2` -> `P 3 F156` -> `S F53` -> `G 1/7` -> `G 2 F176` -> `K 13 F189` -> `G 3 F190` -> `G 4 F310`
  -> `V F369` -> `G 5` -> `G 6 F376` -> `C ROOM2090 F405 wert=0/1` + `Kamera-Uebergabe: Cut 0 (Eintritts-Cut), Spielfigur im Bild
  (Fuss-Bildpunkt 88,126)` -> `G 7 F405` -> `B 1 F407` + `[bgm] stage=1 room=09 entry=FF3C -> MAIN3C` + `[bgm] loaded: 1 VAGs,
  SEQ 100B` (= RE2 MAIN1A-Container). Bildnummern Zeile fuer Zeile gleich Abnahme 1. state.log letzter Abschnitt (F1-F700):
  F405-F479 in 75 von 75 Bildern `cam=0`, Leon (-8392,-25550) im Bild, pm 2 -> 1 (F406) -> 0 (F420), erster Biss F466.
  Knall (Nachtrag 17) audio.raw (Takt = 1470 Stereo-Frames): Spitzen bis Takt 572 6138..7640, **Takt 573 18920**, 575 19687,
  580 19005, abklingend bis 593 8543 = Befund Abnahme 1. Bildbogen `F41a2/L1_kampfstart.png` (F310..F470): Leon an der Leiter,
  der Alligator davor.
- E1 Elza (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, 2081/2091, `RE15_NOAUDIO=1`):
  `[pl] Spieler-Familie PL04`, dieselbe Folge mit denselben Bildnummern (`C ROOM2091 F405 wert=0/1`, Kamera-Uebergabe Cut 0
  Spielfigur im Bild), F405-F479 75/75 `cam=0`, `EXIT_AT: Bild 700 in Raum 2091`. `T ... wert=0/0` = ohne Ton (erwartet).
  Bogen `F41a2/E1_kampfstart.png`.
- Spinnen (Punkt 2) in beiden Laeufen: je 2090/2091-Besuch zwei Zeilen `Spinne 0x25 bei (-100,-8296)|(-2300,-23896) verworfen`,
  state.log ` t=25 `: L1 0, E1 0.
- **Urteil 1a: erfuellt** (unveraendert gegenueber Abnahme 1; Riegel `integration_r41_gatorszene_leon/_elza` gruen, s. 1).

## 3. Punkt 1b — "Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe seine Leiche sein und blinken" (M2 der Abnahme 1)

Je Lauf ein Zwilling mit `RE15_SET_FLAG=...,9:112` (Taken-Bit vorab -> kein Schluessel), sonst bitgleiche Eingaben; Vergleich
Bild fuer Bild mit JEDEM Bild (`RE15_FRAMEDUMP=.../1`, `tools/diffserie.py`, jede Abweichung zaehlt; Kasten = Huelle der
abweichenden Pixel, hellster Pixel = max(R,G,B) im Lauf mit Schluessel).
Gemeinsam: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_DEBUG_JUMP=2090@250
RE15_STATE_LOG=state.log` (Elza: `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, 2091).
- R1 (Kanistertod, der EINZIGE Todesweg im Spiel: Beschuss toetet nicht, Entscheid B2 / HP-Sockel 25300): Rezept des Riegels
  `_tod_r1` = `RE15_GB_TEST=5 RE15_PLAYER_POS=-1000,-25550,0,0 RE15_GATOR_KANISTER=4430,-26160 RE15_GIVE=3:15,21:50 RE15_EQUIP=3
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=U1.2,(M0.3,MA0.1)x12" RE15_SET_FLAG=9:110
  RE15_EXIT_AT=420#2090 RE15_FRAMEDUMP=60-420/1:fd_` (KLm/KLo Leon, KEm/KEo Elza).
- Todesrolle (Testhaken `RE15_GB_TEST=3`, im Spiel nicht erreichbar): `RE15_GB_TEST=3 RE15_R39_MESS_SPIELER_HP=100
  RE15_EXIT_AT=760#2090 RE15_FRAMEDUMP=440-760/1:fd_` (TLm/TLo Leon, TEm/TEo Elza).

| Lauf | Kamera (debug.log) | Lage / Abstand | Einstellung (debug.log / state.log) | Bilder mit Schluessel in der Einstellung | Kasten | hellster Pixel | danach (laufende Kamera) |
|---|---|---|---|---|---|---|---|
| KLm/KLo Leon R1 | `Schluessel-Kandidat` Cut 1 (laufende) Tiefe 5766 = 6 px, Cut 2 2643 = 14, Cut 3/4/6 2-3 -> `Schluessel-Lage: Kamera 2 (groesstes Bild, laufende 1), Lage (1400,-1704,-23300) Tiefe 2643, 1001 von der Leiche`, Blink-Spitze 118 (<= 0x80) | Leiche (3413,-25001) (`RE2 R1 ende (Leiche)`), Abstand 1001 | `Schluessel-Einstellung: Cut 2 (vorher Cut 1) F84` ... `Ende: Cut 1 F176`; state.log cam 1 -> 2 F84, pm=2 F85-F174, cam 1 F175, pm 1 -> 0 F189 | 91/91 (F85-F175), je 29 px | (32,192)-(44,206) = 13 x 15 | 70 -> 103 (x1,47) | Cut 1 F176-F420: 2 px (164,105)-(166,106) = 3 x 2, 70 -> ~100, 245 Bilder |
| KEm/KEo Elza R1 (`Spieler-Familie PL04`) | dieselben Zeilen (ROOM2091) | dieselbe Lage | dieselben Bilder | 91/91, 29 px | 13 x 15 | 70 -> 103 | wie Leon (3 x 2) |
| TLm/TLo Leon Todesrolle | Cut 0 (laufende) 4835 = 7 px, Cut 1 2441 = 15, Cut 7 2717 = 13 ... -> `Kamera 1 (groesstes Bild, laufende 0), Lage (-5200,-1689,-26800) Tiefe 2441, 1698 von der Leiche` | 1698 | `Cut 1 (vorher Cut 0) F469` ... `Ende: Cut 0 F561`; state.log cam 1 F469, pm=2 F470-F559, cam 0 F560 | 91/91 (F470-F560), je 37 px | (286,195)-(302,206) = 17 x 12 | 72 -> 108 (x1,50) | Cut 0 F561-F760: **bitgleich** zum Zwilling (0 px, 200 Bilder) |
| TEm/TEo Elza Todesrolle | dieselben Zeilen (ROOM2091) | dieselbe | dieselben Bilder | 91/91, 37 px | 17 x 12 | 72 -> 108 | 0 px wie Leon |

- Kein Zwischenbild: KL F83 und F84 zeigen Cut 1 (F84 schon mit den 2 Schluessel-Pixeln der Cut-1-Lage), F85 ist das erste
  Cut-2-Bild, F176 wieder Cut 1. TL F469 Cut 0, F470 Cut 1, F561 Cut 0. Der Zwilling ohne Schluessel schneidet in denselben
  Bildern (gleiche Kamera, gleiche Bildnummern) - der Schnitt haengt nicht am Schluessel.
- Balken: KL F88-F89 fahren die Balken ein (6/24 -> 24/24 Zeilen), F184-F186 aus (6/24 -> 0/13 -> 0/0); der Schluessel liegt in der
  Einstellung bei y 192..206, ueber dem unteren Balken (ab y 216).
- Bilder: `F41a2/KL_einstellung.png` (F83/84/85/110/140/175/176/250), `F41a2/KL_key_zoom.png` (6-fach F90/F120/F150, rechts der
  Zwilling F150: Ring mit Bart, im Zwilling nichts), `F41a2/TL_einstellung.png`, `F41a2/TL_key_zoom.png` (Ring mit Bart).
- Befund zur Eingabe des R1-Rezepts (Riegel `_tod_r1`, Dossier "12 Schuesse"): `input_pc.c script_parse_once` kennt keine
  Wiederholung; debug.log `[input-script] Tick 36 -> F37 Tasten 0x0800`, `Tick 45 -> F46 Tasten 0x8800`, `Tick 48 -> F49 Tasten
  0x4000` (bis F408) = 1,2 s gehen, 0,3 s zielen, EIN Schuss (inv `03:15 -> 03:14` F46), dann 12 s CROSS ("x12" = X 12 s). Der
  Kanistertod kommt trotzdem (ein Treffer genuegt), die Messung gilt; nur die Beschreibung "12 Schuesse" stimmt nicht (Hinweis H1).
- **Aufnahme** (PKL2 Leon / PKE Elza, R1-Rezept + Fortsetzung `W0.4,L0.13,U1.2,A0.1,W3,A0.1,W1,A0.1,...`, `RE15_ITEMBAND_LOG`,
  `RE15_INV_LOG=1`): nach der Einstellung vier Bilder LEFT (rot 3471 -> 3087), 1,2 s vorwaerts (1325,-25550) -> (1361,-22886);
  itemband.txt `F461 room=2090|2091 ITEM slot 15 floor 0 item 0x34 tk 112 spieler-band 0 spieler=(1361,0,-22886) -> FREI`;
  debug.log `[inv-r39] F554 R2090|R2091 ... inv=03:14,15:50,34:1` = Water Key im Inventar (Leon und Elza). Bogen
  `F41a2/PKL_aufnahme.png` (F440..F520: Item-Bild, Drehung, "Will you take the Wa..."). Auf dem ganzen Weg bleibt die Kamera
  Cut 1 (state.log cam=1 F176-F520) - der Schluessel ist dort 2-3 Bildpunkte gross (s. Hinweis H2).
- **Andere Todeslagen** (Grenze "Leiche fern von allen Kameras", Bau-Agent OFFEN 2): KN (Leon steht per `RE15_PLAYER_POS=2000,-9000,1024,0`
  im Norden, Schuesse ab F151): dieselbe Leiche (3413,-25001), Cut 2, 13 x 15 / 29 px, danach in der Teleport-Kamera Cut 0
  2 x 8 px am Bildrand (318,176)-(319,183). KQ (dto., Schuesse ab F991 nach dem Losschwimmen): `R1 start pos=(4359,-17290)`,
  `R1 ende (Leiche) pos=(4528,-12323)` (Ostbecken Mitte) -> `Schluessel-Lage: Kamera 3 (groesstes Bild, laufende 0), Lage
  (1300,-1706,-9200) Tiefe 2667, 2346 von der Leiche`, Einstellung Cut 3 F1039-F1129: 36 px, Kasten (282,192)-(293,205) =
  12 x 14, hellster 47 -> 68 (x1,45, Lichtsatz Cut 3 dunkler: Ruhe 53 / Spitze 78), Ring mit Bart (`F41a2/KQ_key_zoom.png`).
  Die gerechneten "~6 Bildpunkte" traten hier nicht auf (der Schluessel liegt innerhalb des Kopf-Wurzel-Abstands nahe Cut 3).
- **Zwischenbild beim Explosionsschnitt (neuer Befund):** Hat Spur B's R1-Programm auf eine Explosions-Kamera geschnitten
  (`KAMERA R1 EXPLOSION ... -> Cut E`, r1_geschnitten = 1), kommt B's Rueckschnitt am Ende von P1 (gator_re2_r41.c:1726, RE2
  @0x801024a4-ec) zwei Bilder VOR dem Tot-Flag in P3 (gator_re2_r41.c:1734-1738, RE2 @0x80102654-64). F schneidet erst auf das
  Tot-Flag (key_tod_pruefen -> gator_tot -> re15_gr2_tot). Gemessen im NATUERLICHEN Fall ohne Teleport - KW Leon: Sprung-Spawn an
  der Leiter (natuerliche Cut 0), `RE15_GB_TEST=5 RE15_GATOR_KANISTER=4430,-26160 RE15_GIVE=3:15,21:50 RE15_EQUIP=3
  RE15_R39_MESS_SPIELER_HP=100 RE15_INPUT_SCRIPT=W5,(M0.3,MA0.1 x12 ausgeschrieben) RE15_SET_FLAG=9:110
  RE15_FRAMEDUMP=150-420/1`; KWE Elza (2091) ebenso:
  gator_boss.log `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F160`, `KAMERA R1 zurueck -> Cut 0 F196`, `R1 ende (Leiche)
  pos=(3413,-25001)`, `KAMERA R1 frei (Zonenkamera an) Cut 0 F198`; debug.log `Schluessel-Einstellung: Cut 2 (vorher Cut 0) F198`;
  state.log cam 1 F160-F195 -> **cam 0 F196-F197** -> cam 2 F198-F288 -> cam 0 F289. Bilder (`F41a2/KW_wechsel.png`): F195/F196
  Explosions-Cut 1, **F197/F198 Cut 0 mit Leon an der Leiter**, F199/F200 Schluessel-Cut 2; Pixel mit Aenderung > 40 zum Vorbild:
  F197 2682, F199 3471 (Elza: F197 2128, F199 3612). Ebenso KN (Cut 1 -> Cut 0 F196/F197 -> Cut 2) und KQ (Cut 3 -> Cut 0
  F1036/F1037 -> Cut 3: `KQ_wechsel.png`, dort mit Teleport-Kamera). Die Behauptung "Der Schnitt kommt im selben Bild wie der
  R1-Rueckschnitt der Spur B" gilt also nur, wenn B's R1 NICHT geschnitten hat (`KAMERA R1 Sperre ohne Schnitt: Kopf im Bild von
  Cut 1` - der einzige gemessene Fall des Bau-Agenten K4m und des Riegels `_tod_r1`). Kein Riegel deckt den Explosionsschnitt
  ab. -> Mangel M1 (Abschnitt 9).
- **Urteil 1b: teilweise.** Wortlaut erfuellt: der Schluessel liegt bei der Leiche (1001 / 1698 / 2346), blinkt (x1,45-1,50), ist
  in der Einstellung als Ring mit Bart erkennbar (12-17 Bildpunkte, Kasten >= Ankunfts-Schluessel), aufnehmbar (Leon + Elza);
  M2 der Abnahme 1 ist in der Einstellung behoben. Aber die Einstellung zeigt im Spielweg mit Explosionsschnitt (R1 von der
  Leiter aus, Leon und Elza) zwei Bilder der Zonenkamera zwischen Explosion und Schluessel (M1).

## 4. Punkt 2 — "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

- Code seit Abnahme 1 unveraendert (`re15_gatorszene_r41_spawn_weg`, Haken scd_vm.c op_sce_em_set nach dem Kill-Flag-Tor;
  `git diff a9472f02..HEAD` beruehrt ihn nicht). RE-Beleg (ROOM2090/2091 Saetze @0x00AB0/@0x00AC4, Kill-Flag-Tor
  @0x80042120-38) in Abnahme 1 selbst gelesen.
- exe, state.log ` t=25 ` (Typ 0x25) in den 2090/2091-Abschnitten: L1 0/1289, E1 0/1289, KLm 0/671, KEm 0/671, TLm 0/1011,
  TEm 0/1011, PKL2 0/1011, PKE 0/1011, KW 0/833, KWE 0/833, KQm 0/1551, G1b 0/451; je Besuch zwei Zeilen `[gatorszene]
  ROOM2090|ROOM2091: Spinne 0x25 bei (-100,-8296)|(-2300,-23896) verworfen`. In G2b stehen 120 ` t=25 `-Zeilen nur im
  20A0-Abschnitt (Schachtspinnen, nicht verlangt; Abschnitt 2090 0).
- Riegel `unit_r41_gatorszene_spinnen`, `unit_r38_spinnen`, `integration_r38_spinnen_echtlauf` gruen (Abschnitt 1).
- **Urteil Punkt 2: erfuellt** (Leon und Elza).

## 5. Nachtrag 21 (Leiter statt Tor, Tor mit dem Schluessel) - Regression

Gemeinsam `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_MSG_LOG=1`:
- G1b (2090, Bit 110, ohne Schluessel, `RE15_PLAYER_POS=6630,-16350,0,1 RE15_PAD_AT=60:A`): `ROOM2090 Slot 1 (Tor) gesperrt:
  Ereignis 19, Bit (9,113) = 0`, `[msgtext] room=2090 id=42 "It's locked."`, `Folge Z ROOM2090 F60`.
- G2b (dto. + `RE15_GIVE=0x34:1`, A F60/140/200/260/320/380/440): `[msgtext] room=2090 id=43 "You've opened the lock."`,
  `Folge U F60`, `inv ... 34:0` F141 (Wegwerf), `[aot] DOOR FIRE slot=1 rect=(7250,-16350,hw=500,hh=1750) target_cut=13
  spawn=(4950,-3600,4100)`, `[tuer] Sequenz Archiv 2 DOOR26 Variante 0 ... Seite S164`, `EXIT_AT: Bild 120 in Raum 20a0`.
  (Erster Versuch G2 mit nur vier Druecken: der vierte Druck schloss die Wegwerf-Frage, kein Durchgang - Eingabezeitpunkt, kein
  Fehler; mit weiteren Druecken oeffnet das Tor.)
- G3b (20A0 ohne Bit 113, `RE15_PLAYER_POS=4950,4100,3072,2 RE15_PAD_AT=60:A`, mit Ton): `ROOM20A0 Slot 0 (Tor) gesperrt:
  Ereignis 20`, `[msgtext] room=20a0 id=1 "It's locked."`, `Folge Z ROOM20A0 F60`, tuerse.log genau EINE Zeile `F60 raum=20A0
  nachricht=1 art=M weg=AOT satz=0(ZU_A) nr=1`.
- Leiter-Zugang: L1/E1 (Abschnitt 2). **Urteil: erfuellt.**

## 6. Mangel M1 der Abnahme 1 (drei Fremd-Riegel durch F rot)

- Diff gelesen (`git diff a9472f02..HEAD`): `test_r37_itemband.cmake` Lauf weg2090 `RE15_SET_FLAG=9:110,9:113`,
  `test_r38_wasser.cmake` leon2090 `RE15_SET_FLAG=9:110` (r38n_exe nimmt weitere Umgebung ueber ARGN),
  `test_r41_wasser20b0_npc.cmake` landung_leon/landung_elza `RE15_SET_FLAG=9:110,9:113` - je mit Querbezug-Kommentar
  (Nutzer-Wortlaut Nachtrag 21 bzw. "neue Cutscene", VERTRAG 1.1, Messung vorher/nachher). Die gemessenen Groessen der Riegel
  (Watschritt, Item-Band FREI, Landung + 104 Spritzer) bleiben unveraendert; nur der Zustand "nach Szene/Schluessel" wird gesetzt.
- Gezielt gefahren (Abschnitt 1): `integration_r37_itemband` Passed 943.82 s, `integration_r38_wasser` Passed 223.77 s,
  `integration_r41_wasser20b0_npc` Passed 255.05 s. **M1 der Abnahme 1: behoben.**

## 7. RE-Gate (Stichproben selbst disassembliert/gelesen, richtige Binaerdatei)

| Zitat im Diff (Nachbesserung 2) | selbst gelesen | Ergebnis |
|---|---|---|
| Cut_old FUN_8004032c @0x8004033c/@0x80040364/@0x80040374-84 (info/Re1.5/PSX.EXE) | re15_disasm.py: `8004033c lbu a0,16251(a0)` (0x800b3f7b), `80040344 lhu v1,4068(v1)` (0x800b0fe4), `80040354 sb v0,21591(at)` (=1), `8004035c sh v1,4072(at)` (0x800b0fe8), `80040364 sh a0,4068(at)`, `80040368 jal 0x800142f4`, `80040374 lw v0,-13764(v0)` / `80040378 addiu v1,zero,-257` / `8004037c and` / `80040384 sw` (0x800aca3c &= ~0x100), `80040390 addiu v1,v1,1` (1 Byte) | stimmt (GSZ_CUT_OLD `2a 00` = Cut_old + Nop) |
| Cut_chg-Zwilling LAB_800402a0 @0x800402c0/d4/e4/ec/fc | `800402c0 lbu a1,4068(a1)`, `800402d4 ori v0,v0,0x100` / `800402d8 sw`, `800402e4 sb a1,16251(at)`, `800402ec sh a2,4072(at)`, `800402f4 sb v0,21591(at)`, `800402fc sh a0,4068(at)`, `80040300 jal 0x800142f4` | stimmt (cut_chg_zwilling setzt cam_id_prev/cam_id/pending/work_vars[0x0C]/[0x0A]/cut_auto 0) |
| Form "Kamera auf einen Gegenstand" ROOM2010 sub03 @0x008EE-0x008F8, ROOM1180 sub02 @0x00B3C | xxd: ROOM2010 @0x8EE `29 05 2b 02 ff ff 02 00 2a 00 3c 01 01 00`, ROOM1180 @0xB3C `29 09 2b 01 ff ff 02 00 2a 00 3c 01 01 00`; scd_dump_room.py: sub03 = Cut_chg 5 / Message_on 2 / Evt_next / Nop / Cut_old / Nop / Cut_auto 1 / Evt_end; shared_assets == info/Re1.5 (cmp) | stimmt (Port: Halt statt Message_on, im Kommentar genannt, H3) |
| Riegel/Plc_ret ROOM1150 sub08 @0x01114/@0x01118/@0x012E0/@0x012E2/@0x012E4/@0x012E8 | scd_dump_room.py: Set 2/7/1, Set 1/27/1, Work_set 1/0 @0x0111E, Plc_ret @0x012E0, Cut_auto @0x012E2, Set 2/7/0, Set 1/27/0 | stimmt (Programm 4 = `22 02 07 01 22 01 1b 01 09 0a 5a 00 2a 00 2e 01 00 00 42 00 3c 01 22 02 07 00 22 01 1b 00 01 00`) |
| NCLIP-Verwurf FUN_800254a0 @0x800255a8, FUN_800256b0 @0x800257c0 | `80025580 .word 0x4b400006` (NCLIP), `8002559c swc2 24` (MAC0), `800255a0 lw v0,0(sp)`, `800255a8 bgez v0,0x800255d0` (>= 0 zeichnen), sonst `800255b0 addiu a1,a1,80` + `800255c8 j 0x80025694` (verworfen); `800257c0 bgez v0,0x800257e8` gleich | stimmt (erklaert 0 px in Cut 0 nach der Todesrollen-Einstellung) |
| Alligator-Kasten @0x80118b98 (STAGE2.BIN) | `read 0x80118b98 6 --w 2 --signed --bin STAGE2.BIN` -> `[1000, -720, 0, 2200, 720, 800]` | stimmt (Kugelradius 800 je Knochen = PORT-WAHL der Verdeckung aus diesem Wert) |
| Halt GSZ_LEICHE_BILDER 90 "1,5 Blinkperioden 0x3c" TITLE.BIN FUN_801028ec @0x80102918 | `801028fc sltiu v0,v1,0x1f`, `8010290c addiu a0,a0,2`, `80102910 addiu a0,a0,-2`, `80102918 ori v0,zero,0x3c`, `80102928 ori a0,zero,0x80` | Periode 60 stimmt; 90 = 1,5 x 60 ist PORT-WAHL (Block "PORT-WAHL der Inszenierung", seit Abnahme 0 so) |
| RE2 R1 P3 Tot-Flag @0x80102654-64, Kamera frei @0x80102688-94 (EM23_OVL_0000.BIN) | re2_disasm.py: `80102654 lhu v0,270(s1)` / `8010265c ori v0,v0,0x4000` / `80102664 sh v0,270(s1)`, `80102684 lw v0,-1064(v0)` (0x800cfbd8) / `80102688 addiu v1,zero,-257` / `8010268c and` / `80102694 sw` | stimmt - und zeigt den Abstand zum P1-Rueckschnitt (gator_re2_r41.c:1726, RE2 @0x801024a4-ec): zwei Takte (P2 @0x80102638-50) |

- Konstanten der Nachbesserung 2: GSZ_CUT_OLD (@0x008F6/@0x008F7), GSZ_KEY_SCHUSS_BILDER = GSZ_LEICHE_BILDER (PORT-WAHL),
  Kamerawahl "kleinste Tiefe, Gleichstand laufende" (PORT-WAHL im Kopf von key_lage_bei_tod), leiche_verdeckt mit
  GSZ_GATOR_HALBBREITE (@0x80118b98) - belegt oder gekennzeichnet. Riegel-Schranken 10 x 9 (= Ankunfts-Schluessel), 90..94
  Bilder (GSZ_LEICHE_BILDER + Faden-Ende) hergeleitet.
- Suche im Diff (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/
  TODO/vorerst/geschaetzt/geraten/ungefaehr/approx: 0 Treffer; kein getenv in gatorszene_r41.c / gatorszene_r41_pc.c.
- Erklaert der Fix den Befund? M1 ja (Zustand nach Szene/Schluessel). M2 ja fuer die Einstellung (Lage fuer die Kamera mit dem
  groessten Bild + Schnitt dorthin, gemessen 12-17 Bildpunkte); die Behauptung "kein Zwischenbild" erklaert den Explosionsfall
  nicht (M1 dieser Abnahme).
- Hinweis zur Zitat-Genauigkeit (kein Verhaltenswert): Kommentar `g_scd.cut_auto_enabled = 1; /* @0x8004032c */` im Rueckfall von
  key_einstellung und im Kopf "Zonenkamera an @0x8004032c" - 0x8004032c ist der Funktionsanfang, das Loeschen von 0x100 steht
  @0x80040374-84 (H4).

## 8. Vertrag

- Bits nur (9,110..113) (VERTRAG 1.1; test_r36_kartenbits.c k_port[] 4 Zeilen), Raum-Scratch (5,28/29/30) nur 2090; Nachrichten
  2090/2091 Ids 42/43 (zugeteilt 42-46); Ereignis 18 (Programm 4 laeuft ueber dasselbe Ereignis), 19/20, AOT-Plaetze 15/20 -
  Nachbesserung 2 belegt nichts Neues.
- Pfade `git diff master...HEAD --name-only` (Merge-Basis 9f10bdb0): keine release/, platform/android/, shared_assets/PSX/, pri/,
  synchro/, kein CMakeLists.txt (grep leer); neu nur shared_assets/RE2/SOUND/MAIN1A.BGM (seit Abnahme 0, im Dossier genannt).
  Nachbesserung 2 (a9472f02..HEAD) aendert ausser F-eigenen Dateien nur die drei M1-Fremd-Riegel (je Querbezug-Kommentar).
  Gemeinsame Dateien: keine neue Aenderung in Nachbesserung 2 (Haken wie Abnahme 1).
- Riegel je Punkt vorhanden und messend; neu `_tod_r1`/`_tod_r1_elza` (Lage-Zeile, Quadrat >= 10,4, Einstellung 90..94 Bilder in
  beiden Zwillingen gleich, state.log cam, bilddiff Kasten >= 10 x 9, Puls >= 1,40). Linux: test_r41_gatorszene.cmake hat
  `cmake_minimum_required(VERSION 3.21)`, Unit-Ziel linkt re15_test_support; exe-Kopie traegt fest ".exe" (wie Spur B und viele
  bestehende Riegel, Hinweis seit Abnahme 0).
- Nebenwirkungen: 213 gezielte Tests gruen (Abschnitt 1); Elza = Leon in allen Laeufen; neuer Befund M1 (Zwischenbild).

## 9. Maengel (nummeriert, nachpruefbar)

- **M1 Zwischenbild der Zonenkamera zwischen Explosionsschnitt und Schluessel-Einstellung (Punkt 1b, Leon und Elza).**
  Schneidet Spur B's R1-Programm beim Kanistertod auf eine Explosions-Kamera (der Kopf ist in der laufenden Kamera nicht im Bild,
  `KAMERA R1 EXPLOSION ... -> Cut E`), kehrt B am Ende von P1 auf die Zonenkamera zurueck (`KAMERA R1 zurueck -> Cut X`,
  gator_re2_r41.c:1726, RE2 @0x801024a4-ec) - zwei Takte VOR dem Tot-Flag in P3 (gator_re2_r41.c:1734-1738, RE2 @0x80102654-64),
  auf das F erst schneidet (key_tod_pruefen -> gator_tot -> re15_gr2_tot). Ergebnis: E -> X (2 Bilder) -> Schluessel-Kamera Y.
  Gemessen ohne Teleport mit Leon (KW) und Elza (KWE) an der Leiter: state.log cam 1 F160-F195 -> cam 0 F196-F197 -> cam 2
  ab F198, Bilder F197/F198 zeigen Cut 0 mit der Spielfigur zwischen Explosion (F195/F196) und Schluessel (F199/F200),
  `F41a2/KW_wechsel.png`; ebenso KN (Cut 1 -> 0 -> 2) und KQ (Cut 3 -> 0 -> 3, `KQ_wechsel.png`). Das widerspricht der
  Behauptung "Der Schnitt kommt im selben Bild wie der R1-Rueckschnitt der Spur B" / Dossier "(kein Zwischenbild)"; sie gilt nur
  ohne Explosionsschnitt (`Sperre ohne Schnitt`, Laeufe K4m/KLm und Riegel `_tod_r1`). Soll: Explosions-Kamera -> Schluessel-
  Kamera ohne Bild der Zonenkamera dazwischen (z. B. Schnitt im Bild des B-Rueckschnitts, wenn r1_geschnitten, oder den
  Rueckschnitt durch den Schluessel-Schnitt ersetzen), gemessen Bild fuer Bild wie hier (Leon + Elza), dazu ein Riegel fuer den
  Fall mit Explosionsschnitt (z. B. Rezept KW: Leiter-Spawn, `RE15_GB_TEST=5 RE15_GATOR_KANISTER=4430,-26160`, Schuesse ab F151).

Hinweise (keine Maengel):
- H1 Das R1-Rezept des Riegels `_tod_r1` und des Dossiers ("12 Schuesse", `U1.2,(M0.3,MA0.1)x12`) gibt EINEN Schuss und 12 s
  CROSS: input_pc.c script_parse_once kennt keine Wiederholung (debug.log `Tick 48 -> F49 Tasten 0x4000`, inv 03:15 -> 03:14
  F46). Die Messung gilt (ein Treffer genuegt), nur die Beschreibung stimmt nicht.
- H2 Nach der Einstellung ist der Schluessel in der laufenden Kamera klein: R1 Cut 1 2 px (3 x 2, Abnahme 1 dort noch 6 px, weil
  er jetzt fuer Cut 2 gekippt liegt), KN Cut 0 2 x 8 px am Bildrand, Todesrolle Cut 0 0 px (Rueckseite, NCLIP - Weg nur ueber
  den Testhaken GB_TEST=3). Auf dem Weg zum Schluessel bleibt Cut 1 (PKL2). Aufnahme funktioniert (34:1 F554, Leon + Elza).
- H3 Form: ROOM2010 sub03 / ROOM1180 sub02 zeigen den Gegenstand mit Message_on + Evt_next; der Port haelt 90 Bilder ohne Text
  (Ids 44-46 frei) - als PORT-WAHL im Kommentar genannt; Bestaetigung durch den Nutzer steht aus (Frage des Bau-Agenten).
- H4 Zitat "@0x8004032c" fuer "Zonenkamera an" (Funktionsanfang; die Stelle ist @0x80040374-84) in gatorszene_r41.c.
- H5 Unveraendert offen (Bau-Agent): MAIN2E nach dem Tod statt RE2-Ausblenden, Ersatzlage nach dem Laden/Wiederbetreten ohne
  Einstellung, Integration mit Spur G (90 Bilder Riegel nach dem R1-Tod), PSX-Bau ungeprueft, Schachtspinnen 20A0.

## 10. Urteile

| Punkt | Urteil | Beleg |
|---|---|---|
| 1a Ankunftsszene (Leiche + blinkender Schluessel, Knall, 2080-Zombies oben, erneut Leiche, Alligator aus dem Wasser frisst Leiche samt Schluessel, Schrei/Schuetteln zu Leon, RE2-Kampfmusik; Leon + Elza) | erfuellt | 2: L1/E1 Folge Zeile fuer Zeile gleich Abnahme 1, Kamera-Uebergabe Cut 0 75/75 Bilder, Knall Takt 573, MAIN1A |
| 1b Schluessel nach dem Tod bei der Leiche, blinkend | teilweise | 3: Einstellung Cut 2/Cut 1/Cut 3, 29-37 px, Kasten 13 x 15 / 17 x 12 / 12 x 14, Puls x1,45-1,50, Ring mit Bart, Aufnahme 34:1 (Leon + Elza); aber M1 (2 Bilder Zonenkamera zwischen Explosion und Schluessel im Weg mit Explosionsschnitt) |
| Nachtrag 21 Leiter statt Tor, Tor mit dem Schluessel | erfuellt | 5: G1b/G2b/G3b, tuerse.log 1 Zeile |
| 2 Spinnen aus 2090 entfernen | erfuellt | 4: 0 Spinnen-Zeilen in allen 2090/2091-Abschnitten, Leon + Elza |
| M1 der Abnahme 1 (Fremd-Riegel) | erfuellt | 6: drei Riegel mit Querbezug, gruen |
| RE-Gate | erfuellt | 7: 8 Stichproben stimmen, Konstanten belegt/gekennzeichnet, keine Rate-Woerter |
| Vertrag/Riegel | erfuellt | 1/8: 213/213 gruen, nur zugeteilte Ressourcen und erlaubte Pfade; Luecke: kein Riegel fuer den Explosionsfall (M1) |

**Ergebnis: NICHT bestanden** (M1).
