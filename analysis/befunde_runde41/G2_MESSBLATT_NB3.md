# Runde 41 - Spur G2-RE2 - MESSBLATT NB3 (Messer, 2026-11-06): Rampe oben (Vorgabe A) und Haenger Plattform-/Rampen-Suedkante (Vorgabe B)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Kopf a661eb8aa (Abnahme 4 BESTANDEN; aendert gegen c06bcfefa nur Dossier/Belege). Rolle: Messer -
Ist-Stand und Riegel VOR dem Bau; kein Spielcode geaendert. Einzige Codeaenderung dieses Commits: Messwerkzeuge `nb3_oben` / `nb3_haengt` in
`re15_port/tests/unit/test_r41_gatorfix.c` (keine Riegel; die bestehenden Riegel sind unveraendert, `ctest -R "^unit_r41_gatorfix_"`: **100% tests passed, 0 tests
failed out of 24**, 47,28 s).

Nutzerentscheid 2026-11-06 ~21:15 (woertlich, bindend):
- **F3 -> Vorgabe A:** "Er soll nur den Spieler oben auf der Rampe auch rammen koennen, das der Spieler oben nicht ausweichen kann. Alternativ kann er auch hoch beissen
  oder sowas, aber er darf halt oben nicht sicher sein." (Takt frei: "Das kann er gerne machen.")
- **F4 -> Vorgabe B:** "Wenn er haengt muessen wir nachbessern denke ich. Er darf nicht zu lange haengen. Wenn es kurz ist, kann man mit dem tauchen abhilfe schaffen,
  aber das hilft natuerlich nicht bei der Platform."
- F1 ("noe, wozu wenn es gut funktioniert"): keine Kameragrenze fuer den Kopf; F2: Querungsbeginn bleibt (nicht anfassen). Alles andere ohne Kanister bleibt NB5
  (Memory reai-v2-gelobter-stand-ist-referenz).

## 0. Messwege, Laeufe, Belege

- exe: `re15_port/build/platform/pc/re15_pc.exe` md5 d872c8e64bb6914619ec2d14d48c3433 = die exe der Abnahme 4 (Quellen seit c06bcfefa unveraendert). Je Lauf eine
  eigene Kopie `re15_pc_g2nb3_<lauf>.exe`, nur eigene PIDs, kein gdigrab; Bildbelege als RE15_FRAMEDUMP-Serie (F-genau). Skripte `G2_belege/nb3/werkzeug/lauf3.sh`,
  `batch3.sh`, `alle3.sh`; Laufordner (nicht im Baum) scratchpad `g2nb3/runs/<lauf>/` (state.log, climb.log, gator_trag.log, gator_boss.log, debug.log, env.txt,
  pid.txt, exe_md5.txt, fd_NNNNNN.ppm). Alle 8 Laeufe exit 0 bis RE15_EXIT_AT, 0 Zeilen error/assert/abort (`mess_laeufe.txt`).
  - `rampe_leon` (ohne + mit Kanister), `rampe_elza` (ohne), `wand_elza` (ohne) = Eingabe/Umgebung der exe-Riegel integration_r41_gatorfix_rampe / _rampe_elza /
    _abn2_wand_elza (test_r41_gatorfix.cmake, wortgleich; RE15_NOAUDIO=1, RE15_WINDOW_SCALE=1); "ohne" = Messhaken RE15_G2_OHNE_KANISTER=1 = Gang ohne Kanister (NB5).
  - `oben_leon` / `oben_elza` (je ohne + mit): Eingabe der Abnahme-4-Laeufe west4_leon / west5_elza bis zum Klettern (`W6.0,M0.6,MA0.1,M0.3,MA0.1,W14.0,U0.1,A0.2`,
    Klettern mo=231 ab F640), dann `W2.0,U1.2,W170` (auf die Rampe gehen, stehen bleiben), RE15_K2080_RUECK_LAGE 4200,-18800,3072,2 bzw. 3600,-18750,3072,2 (Cut 2),
    RE15_EXIT_AT 4400, RE15_WINDOW_SCALE=2, Framedumps F600..F4400 je 30 Bilder.
  - Auswerter `werkzeug/oben3.py` (Figur oben = climb.log y < -900 und Grundriss Rampe/Plattform; Treffer = hp-Flanke < 100 in state.log; Art = Phase aus gator_trag.log:
    7 = Querung (RAMMEN-Zeile in gator_boss.log = Rammen, sonst CROSS-Biss), 5 = Hochbiss, 3 = Biss) und `werkzeug/haeng3.py` (Haenge-Fenster = das Mass von
    gator_rw_r41.c re15_gr2_rw_nachtakt (b): 150 Bilder in Phase 2 ohne Biss/Treffer, Wurzel-Kasten < 2500 x 2500, Kopf (3236 vor der Wurzel, Clip 0 NB5 M2a) > 4500
    von der Figur, netto < 600 in 60 Bildern; "ohne Fortschritt" = Mass des Riegels re2 / mass_v4.py; TICK-Kontext aus gator_boss.log ueber pos=(x,z)).
- Harness: `test_r41_gatorfix.exe nb3_oben <leon|elza> <neu|ohne> [bilder] [x,z ...]` und `nb3_haengt <leon|elza> <neu|ohne> [bilder] [x,z,cam ...]` (Kopf der
  Werkzeuge in test_r41_gatorfix.c). Je Lage ein frischer Kampf (szene_bis_kampf, Kanister ins Maul bis Stufe 7; "ohne" = `re15_gr2_kanister_entfernen()` danach =
  Riegel-re2-Stand "ohne"), Figur steht still, Start des Alligators = fernste der fuenf Beckenlagen (wie Riegel re2). Treffer = pl->hp < 100 nach dem Bild (hp je Bild
  auf 100 gesetzt, jedes Trefferbild = ein Treffer), Art wie oben (Rammen = Zaehler re15_gator_boss_rammen). Leon und Elza bitgleich in allen Laeufen.
  Ausgaben `G2_belege/nb3/mess_harness_*.txt`; Spur je Bild (scratchpad `g2nb3/harness/<lauf>/spur.txt`).
- Belege `G2_belege/nb3/`: `mess_exe_oben.txt`, `mess_exe_haengt.txt`, `mess_harness_oben_{leon,elza}_{ohne,neu}.txt`, `mess_harness_haengt_{leon_ohne,leon_neu,
  elza_ohne}.txt`, `mess_laeufe.txt`, Leisten `nb3_oben_leon_F660-F2100.jpg`, `nb3_oben_elza_F660-F1680.jpg`, `nb3_oben_leon_hochbiss_plattform_F1980-F2130.jpg`,
  `werkzeug/`.

## 1. Vorgabe A - Figur oben auf der Rampe: Ist-Stand

### 1.1 Welche Angriffe eine Figur oben erreichen (Code gelesen, enemy_ai_boss_gator.c @a661eb8aa)

| Angriff | Phase | Ausloeser / Lage des Alligators | Treffer-Bedingung | mit / ohne Kanister | Quelle |
|---|---|---|---|---|---|
| Hochbiss (Belagerung) | GBP_GUARD | Kantenziel GB_GPAT_M 1900 vor der Kante auf "Leons naeherer Kante" (gb_seite_latch Z. 1014-1031: Totband +-800 um die Rampenmitte -16300, Seitenwechsel erst nach 300 Bildern stabiler Gegenseite), gx = Figur-x geklemmt auf [4450..6500] (GB_RAMP_X0 + GB_RING_M .. GB_RAMP_X1 - 700, Z. 2366-2367), Pendel +-1500 alle 90 Bilder; Aufrichten + Clip 4 bei dist < 4200 (Z. 2550) | Clip-4-Bild 6..34, hit_react 0, **dist < 3900** (Z. 2557-2558); dist = re15_enemy_player_dist = nur x/z (re15_damage.c:3924-3930) - keine Hoehenbedingung; danach hit_stun 0x64 (Z. 463) -> Takt 105 Bilder | gleich | Nutzer-Design 2026-09-10 ("nach oben schnappen"), kein RE2-Vorbild |
| CROSS-Biss (Querung) | GBP_CROSS | Querung aus der Routenwahl (Zone Sued<->Nord, Z. 1926-1937) oder QUER-CROSS der Belagerung, nur wenn die Zone des Alligators != Leons Kante (Z. 2339); Bahn-Ende folgt der Figur (x ueber der Rampe [2550..6500], Z. 2605-2608; Landeklemme 3350 Z. 1120); Pause danach cross_cd 300 (Z. 2621, Spur B "Leon OBEN: 10 s auf der Seite bleiben", PORT-WAHL NB5) | Maulpunkt (2600 voraus) <= 1500, hit_stun 0, biss_erlaubt (Z. 2740-2744) | gleich | Spur B (v0.7.55), NB5 |
| Rammen (Querung) | GBP_CROSS | wie CROSS-Biss; die Querung wiederholt sich mit dem Kanister (zielzone = !zg2 bei cross_cd == 0, Z. 2330-2331), **aber nur, wenn sie von der Kamera weg fuehrt** (Kopf 5644 vor der Wurzel >= 2500 vor der Kamera, Z. 2332-2337; Cut 2 = Kamera der Rampe West, Blick nach Norden: Sued->Nord erlaubt, Nord->Sued nicht) | gb_koerper_kontakt (5 Push-Kreise + 450, nur x/z, Z. 969-983), einmal je Querung, **nur re15_gr2_rw_traegt()** (Z. 2728) | nur mit Kanister | Vorschlag 4 (Nutzer 2026-11-02 "gerammelt werden"), PORT-WAHL |
| Biss (LUNGE) | GBP_CHASE | dist < 3200 oder Maul < 2400 | im Wasser; oben nicht (gb_wand_dazwischen blockt die Rampe, Z. 617-628; bei Figur oben laeuft GUARD) | gleich | RE2 Lunge (Port-Form) |

Reichweite des Hochbisses von der Kantenlinie (1900 vor der Kante) in die Rampe: sqrt(3900^2 - dx^2) - 1900 = 2000 bei dx 0, **1366 bei dx 2132** (Figur x 2318 gegen
die Klemme 4450). Die Rampe ist 3600 tief (z -18100..-14500): eine Figur im Mittelband (1366..2234 von der belagerten Kante) bei x < ~3400 ist fuer den Hochbiss
unerreichbar; eine Querung dorthin gibt es nicht, solange der Alligator auf "Leons Kante" liegt (ohne Kanister nie, mit Kanister nur von der Kamera weg).

### 1.2 Harness-Raster (`nb3_oben`, 18 Lagen x 3600 Bilder, Cut 2 = Kamera des Wegs von Sueden; Leon = Elza bitgleich)

| Reihe (z) | x | ohne Kanister: erster Treffer / Treffer (Art) / laengste Pause ohne Treffer | mit Kanister (Vorschlag 4) | Wurzel ab t600 |
|---|---|---|---|---|
| -17600 (Suedkante + 500) | 2318 | t291 CROSS-Biss / 249 (248 Hochbiss) / 291 B. = 9,7 s | t291 CROSS-Biss / 80 (3 Rammen, 3 CROSS-Biss, 74 Hochbiss) / **918 B. = 30,6 s** | ohne: Sued-Kantenlinie (4171..5678, -21938..-19688); mit: ganzes Becken (-1100..5678, -21938..-9921) |
| | 2800 | t288 / 307 / 288 B. = 9,6 s | t247 Rammen / 90 (4 Rammen) / 864 B. = 28,8 s | |
| | 3350 | t278 / 312 / 278 B. = 9,3 s | t233 Rammen / 119 (6 Rammen) / 697 B. = 23,2 s | |
| | 4428 | t274 / 317 / 274 B. = 9,1 s | t230 Rammen / 132 (6 Rammen) / 776 B. = 25,9 s | |
| | 5500 | t282 / 323 / 282 B. = 9,4 s | t237 Rammen / 121 (6 Rammen) / 798 B. = 26,6 s | |
| | 6500 | t298 / 343 / 298 B. = 9,9 s | t253 Rammen / 110 (5 Rammen) / 959 B. = 32,0 s | |
| -16300 (Mitte) | 2318 | **KEIN TREFFER in 120 s** | **KEIN TREFFER** (identisch) | Nord-Kantenlinie (4164..5598, -12855..-10540), Kopf min 1624 |
| | 2800 | **KEIN TREFFER** | **KEIN TREFFER** | dto., Kopf min 1372 |
| | 3350 | t1236 Hochbiss (41,2 s) / 16 / 1236 B. | identisch | dto., Kopf min 1086 |
| | 4428 | t371 Hochbiss / 187 / 371 B. = 12,4 s | identisch | (4174..5729, -12887..-10532) |
| | 5500 | t293 / 190 / 293 B. | identisch | (4951..5716, -12740..-12273) |
| | 6500 | t300 / 222 / 300 B. | identisch | (4951..5732, -12780..-12273) |
| -15000 (Nordkante + 500) | 2318 | t288 CROSS-Biss / 243 / 288 B. = 9,6 s | t288 CROSS-Biss / 244 (1 Rammen) / 288 B. | Nord-Kantenlinie (4165..5694, -12912..-10387) |
| | 2800 | t284 / 279 / 284 B. | t243 Rammen / 280 / 243 B. | |
| | 3350 | t274 / 261 / 274 B. | t230 Rammen / 262 / 230 B. | |
| | 4428 | t270 / 330 / 270 B. | t227 Rammen / 331 / 227 B. | |
| | 5500 | t280 / 349 / 280 B. | t235 Rammen / 350 / 235 B. | |
| | 6500 | t296 / 345 / 296 B. | t252 Rammen / 346 / 252 B. | |

SUMME je Stand (Leon = Elza): **2 von 18 Lagen ohne Treffer** (x 2318 / 2800 in der Rampenmitte), spaetester erster Treffer t1236 (x 3350 Mitte); ohne Kanister
sonst erster Treffer <= 300 Bilder nach Beginn (= Anfahrt + erste Querung mit CROSS-Biss) und danach Hochbiss alle 105 Bilder (laengste Pause <= 300 B.); mit Kanister
in der Suedreihe Pausen 697-959 Bilder (23-32 s): nach der Querung (Rammen) belagert er 300 Bilder die FERNE Kante (Kopf-Abstand mittel 4687-4990, Hochbiss reicht
nicht), quert zurueck (Rammen nur bei Koerperkontakt), usw. In der Mittelreihe sperrt die Kamera-Regel (Z. 2332-2337) die Querung Nord->Sued (zur Cut-2-Kamera hin):
mit = ohne.

Wo steht der Alligator: Sued-/Nordreihe auf der belagerten Kantenlinie z = -20000 bzw. -12600 (GB_RAMP_Z0 - 1900 / GB_RAMP_Z1 + 1900), x 4164..5733 (Klemme 4450 +
Pendel 1500); Mittelreihe stets die Nordkante (Start im Norden, Totband haelt die Seite). Haenge-Fenster (RW-Mass) in allen 72 Lagen-Laeufen: 0.

### 1.3 exe-Laeufe (Figur klettert bei F640, steht ab F668 oben; Cut 2; `mess_exe_oben.txt`, Leisten)

| Lauf | Figur oben | erster Treffer oben nach | Treffer oben (Art, Ort) | laengste Pause ohne Treffer oben | Ende |
|---|---|---|---|---|---|
| neu_oben_leon (Kanister ab F625) | F668..F4400 = 124,4 s | **212 B. = 7,1 s**: Rammen F880, Wurzel (4450,-20664) Gier 3069 beim Querungsbeginn, Leon (4428,-17263) -> Niederwurf nach (4428,-14968) | 24: 1 Rammen, 23 Hochbiss (F1152 an (2300,-15920); ab F1989 alle 105 B. an (1382,-18587) = Leon war auf die PLATTFORM gestossen worden, Hochbiss von Sueden, Wurzel (437..3039, -22318..-21905)) | **837 B. = 27,9 s** (F1152..F1989) | bleibt oben (Plattform) |
| ohne_oben_leon (NB5-Gang) | F668..F1850 = 39,4 s | **410 B. = 13,7 s**: CROSS-Biss F1078 (Querung Sued->Nord, Wurzel (5345,-18904), Leon (4428,-14968) an der Nordkante) | 5: 2 CROSS-Biss (F1078, F1850), 3 Hochbiss (F1276/F1381/F1486 an (2307,-14993), Wurzel (3360..4336, -12132..-11937)) | 410 B. = 13,7 s | CROSS-Biss F1850 wirft ihn von der Rampe ins Wasser |
| neu_oben_elza = ohne_oben_elza (bitgleich, Kanister ab F679) | F668..F1637 = 32,3 s | **586 B. = 19,5 s**: Hochbiss F1254 an (2318,-14968) von Norden, Wurzel (5303,-12525) | 2: Hochbiss F1254, CROSS-Biss F1637 | 586 B. = 19,5 s | CROSS-Biss F1637 wirft sie ins Wasser (Leiste F1650-F1710, y 0 danach) |

Leon ohne/mit weichen ab F868 ab (wie Abnahme 4: Rammen nur mit Kanister); Elza: mit = ohne, weil der Alligator an der Nordkante liegt und die Querung nach Sueden zur
Kamera hin fuehrt (Kamera-Regel). Bild: `nb3_oben_leon_F660-F2100.jpg` (Spalten ohne | mit; F870-F930 Rammen und Niederwurf nur mit Kanister, F1080-F1170 CROSS-Biss
ohne / Hochbiss mit, F1980-F2100 Hochbiss auf der Plattform), `nb3_oben_elza_F660-F1680.jpg` (F1230-F1290 Hochbiss, F1620-F1710 CROSS-Biss und Sturz ins Wasser).

### 1.4 Befund A

1. Sicher ist die Figur im **Mittelband der Rampe (z -16300 +- ~400) bei x < ~3400** (Westdrittel): kein Hochbiss (3900 ab Kantenlinie 1900 + x-Klemme 4450), keine
   Querung (ohne Kanister liegt er nach der ersten Querung auf Leons Kante und quert nie wieder; mit Kanister sperrt die Kamera-Regel die Querung zur Kamera hin). Bei
   x 3350 nur alle 41 s ein Treffer.
2. Ueberall sonst wird sie getroffen; ohne Kanister erster Treffer 274-371 Bilder nach Beginn (Harness) bzw. 410 / 586 Bilder nach dem Klettern (exe Leon / Elza),
   danach Hochbiss alle 105 Bilder. Mit Kanister erster Treffer frueher (Rammen t227-253), aber Pausen bis 32 s, wenn er nach der Querung 300 Bilder die ferne Kante
   belagert (exe Leon 27,9 s).
3. Rammen (Koerperkontakt, Niederwurf) gibt es nur mit Kanister (Z. 2728); ohne Kanister trifft die Querung nur ueber das Maul (CROSS-Biss) und nur beim Seitenwechsel.
4. Ein Treffer kann die Figur von der Rampe werfen (exe: Elza F1637, Leon ohne F1850) - danach Verfolgung im Wasser; die Riegel zaehlen nur, solange sie oben ist.

## 2. Vorgabe B - Haenger an der Plattform-/Rampen-Suedkante: Ist-Stand

### 2.1 exe-Reproduktion (`mess_exe_haengt.txt`; "ohne" = NB5-Gang)

| Lauf | Haenge-Fenster (RW-Mass) | Wurzel-Kasten | Figur dabei | Kopf von der Figur | laengste Zeit ohne Fortschritt | Treffer |
|---|---|---|---|---|---|---|
| ohne_rampe_leon | 1: F2500-F2649 | (3459..4747, -23087..-22323) | (4073, 0, -19322) im Wasser, 1222 suedlich der Rampen-Suedkante, Cut 0 | 7026 (3236-Proxy; Modul mit Kanister: 6830) | 229 B. = 7,6 s (bis F2745) | 22 Bisse (Phase 3), alle 105-300 B.; die Figur wird je Biss versetzt |
| neu_rampe_leon (Kanister ab F680) | 2: F2649 (identisch) und F3104 (3279..4879, -23582..-22511), Figur (4252,-19953) | | | Modul: "HAENGT F2649 ... Kopf 6830", "HAENGT F3104 ... Kopf 4720" -> RUECKWAERTS F2650 und F3105 (Rueckfall greift hier) | 202 B. = 6,7 s | 21 |
| ohne_rampe_elza | 1: F2649, bitgleich Leon | (3459..4747, -23087..-22323) | (4073,0,-19322) | 7026 | 229 B. | 22 |
| ohne_wand_elza | 1: F4779-F4928 | (2314..4342, -23184..-21690) | (7668, 0, -16403) = Ostnische (Zone 2, zwischen Rampen-Sued- und Nordkante), Cut 2 | 10057 (Abnahme: 9820) | 191 B. = 6,4 s | 58 |

TICK-Kontext (gator_boss.log, alle 30 Takte) in den Fenstern: Phase 2 (CHASE), rt=0, Steuerziel wechselt zwischen **zw=5 ziel=(4450,-22600)** und **zw=6
ziel=(-4300,-22600)** (rampe: 206 WECHSEL-Zeilen im Lauf, Folge zw 6,5,6,5,0,6,5,6,5,6,5,6 ...; wand: dazu zw=11 = gb_kanten_umweg Ecke 1 = (4450,-22600)), dist 3788-3848
(rampe) bzw. 6695-9158 (wand), Clip 1 (Paddeln). Kein NOTFREI im Fenster (rampe 1 im ganzen Lauf, NOTFREI-G 0).

### 2.2 Harness-Nachstellung (`nb3_haengt`, 11 Wasser-Lagen x 3600 Bilder; Leon = Elza, ohne = mit bis auf das Rammen)

| Lage (x,z,Cut) | Haenge-Fenster | Kasten / Steuerziel | laengste Zeit ohne Fortschritt | Treffer (erster) |
|---|---|---|---|---|
| W4073/-19322/c0 (Figur des exe-Haengers) | 1 (t2421; mit Kanister 2: t2421, t2827) | (3215..3596, -24085..-22255), Kopf 7798, Ziel (4450,-22600); t2827 Ziel (-4300,-22600) | 9,4 s | 249 (t217) |
| W4200/-18800/c2 (west4_leon RUECK_LAGE) | **3** (t1764, t3139, t3376; mit Kanister 2) | (3198..3838, -23716..-22598), Kopf 7419-7489, Ziel (4450,-22600) | - | 234 (t206) |
| W4428/-18951/c2 (west4 Figur beim Klettern) | 1 (t2744) | (3156..3703, -23543..-22173), **Kopf 6830** (= Zahl der Abnahme), Ziel (4450,-22600) | 10,6 s | 257 (t206) |
| W3000/-18800/c0 = c2 (rampe RUECK_LAGE; bitgleich in beiden Kameras) | 0 | - | 10,7 s (= Dash, s. u.) | 286 (t209) |
| W4252/-19953/c0, W3600/-18750/c2, W6400/-18700/c2, W2318/-18568/c2, W1000/-20700/c2, W3000/-21000/c0 | 0 | - | 3,5-10,9 s | 190-319 |

Die laengste Zeit ohne Fortschritt (10,9 s = 327 Bilder an W2318/-18568, t348-t674; 10,7 s an W3000/-18800) ist NICHT der Haenger, sondern der **RE2-Zonen-Dash**
(Phase 12, Clip 2/3) unmittelbar nach der Querung: er dasht von (3455,-19966) nach (-130,-24546), von der Figur weg (Kopf-Abstand 4434 -> 7611), und beginnt dann die
Verfolgung neu (Abnahme 4 Offen 3, mit = ohne). Die Haenge-Fenster selbst dauern 5 s (per Definition) und enden mit einem Biss; zwischen den Bissen liegen bis 18,6 s.

### 2.3 Ursache (Code gelesen, enemy_ai_boss_gator.c @a661eb8aa; Nachstellung = `nb3_haengt`, Spur je Bild)

1. **`gb_seg_hits_rect` (Z. 373-407) meldet "Punkt innen -> Schnitt" (Z. 384-386)**: liegt der ENDPUNKT der Strecke (die Figur) im um m aufgeblasenen Block-Rechteck,
   gilt die Strecke als blockiert - auch wenn der Endpunkt im Wasser liegt. Mit m = GB_KOERPER_M 1800 (Z. 115) ist das Rampen-Rechteck x [50..9000] z [-19900..-12700]
   und das Plattform-Rechteck x [-3500..3650] z [-21800..-10650]. Die Figur (4073,-19322) steht 1222 vor der Rampen-Suedkante, also im aufgeblasenen Rampen-Rechteck;
   ebenso (7668,-16403) in der Ostnische und (4252,-19953).
2. **CHASE-Zweig "verbunden" (gleiche Zone, Z. 1950-1962)**: `gb_seg_hits_platform(e, zp, 1800)` wahr -> `gb_ring_target` -> Ringecke C[1] = (GB_PLAT_X1 + GB_RING_M,
   GB_PLAT_Z0 - GB_RING_M) = **(4450,-22600)** (zw 5); sonst `gb_seg_hits_ramp(e, zp, 1800)` -> "Punkt innen" -> Westumlauf-Ecke (GB_PLAT_X0 - GB_RING_M,
   GB_PLAT_Z0 - GB_RING_M) = **(-4300,-22600)** (zw 6). Welcher Zweig greift, haengt nur davon ab, ob die Strecke Alligator->Figur die Plattform-Ecke des
   aufgeblasenen Rechtecks streift (Wurzel x < ~3650): x < 3650 -> Ziel die Ringecke direkt neben ihm, x > 3650 -> Ziel 8000 weiter westlich. Er schwimmt nach Westen,
   unterschreitet x 3650, das Ziel springt zurueck, er wendet - Pendel im Kasten (3156..4879, -24085..-21690), Kopf nach Sueden/Westen, die Figur 3800 entfernt im
   offenen Wasser. Beendet wird das Fenster nur durch den Biss-Commit (dist < 3200, Z. 2137-2141), wenn eine Wende ihn zufaellig nahe genug bringt (alle 105-300 Bilder).
3. **`gb_kanten_umweg` (Z. 670-738)**: der Dijkstra ueber die vier Ecken bekommt fuer jede Strecke Ecke->Figur dieselbe "Punkt innen"-Meldung -> "Ziel unerreichbar
   (Schattenlage)" (Z. 722-737) -> naechstgelegene freie Ecke = Ecke 1 (4450,-22600) (zw 11 im wand-Lauf). Der FORTSCHRITTS-WAECHTER (Z. 1967-1997) greift nicht
   (die Wurzel kommt je 90 Bilder > 300 voran - er pendelt ja).
4. Nicht beteiligt: Kamera (Zweig ohne Kamera-Abfrage; Harness W3000/-18800 in Cut 0 und Cut 2 bitgleich), Kollision (Wurzel bleibt > 1466 von der Kante, die Klemme
   greift nicht), Bogen-/Wende-Regel (fuehren nur das springende Ziel aus), Kanister (mit = ohne bis zum Rueckfall; Riegel tragvergleich der Abnahme 4).
5. Mit Kanister greift genau hier der Rueckfall (gator_rw_r41.c:49-52 Haenge-Fenster, Z. 277 Rueckwaerts Clip 10: exe F2650 / F3105) - "Nachbessern in der Verfolgung"
   heisst: dieser Zweig, nicht der Rueckfall.
6. Dieselbe Mechanik wirkt an jeder Wasserlage naeher als 1800 an einer Blockkante: in Riegel re2 ist das die Lage "Nordbecken-Ost" (6732,-13687) (813 vor der Rampen-
   Nordkante): dort "Drehen 76 %" im Stand ohne Kanister (fin_re2.txt der Abnahme 4) - kein Haenge-Fenster, weil die Bisse das Fenster abbrechen.

### 2.4 "Nicht zu lange" in Zahlen (Ist)
- Haenge-Fenster (RW-Mass, 150 Bilder = 5 s, PORT-WAHL seit 199a63c95): exe 1-2 je Lauf, Harness 1-3 je Lage an W4073/W4200/W4428; mit Kanister loest jedes Fenster den
  Rueckfall Rueckwaerts aus (exe F2650, F3105), Tauchen 0.
- laengste Zeit ohne Fortschritt zur Figur an diesen Lagen: exe 6,4-7,6 s (ohne), 6,7 s (mit); Harness 3,5-10,9 s, davon alles > 9,4 s der Dash nach der Querung.
- Zum Vergleich: mit Kanister heute 4,1-11,1 s (Abnahme 4, alle Laeufe), NB5 west4_elza 70,2 s (der alte M4-Haenger).

## 3. RE2-Vorbild (EM23-Overlay ROOM40A0, `analysis/befunde_runde41/herkunft/alligator_werkzeuge/re2_em23.dis`, EXE ghidra_re2_Leon.txt)

- Angriffsausloeser ist die LAGE DES ALLIGATORS im Korridor, nicht die Figur: Zonenbyte +0x21A 0 -> 1, wenn x < -17877 (`lw v0,56(s0) / slti v0,v0,-17877` @0x80100990-98,
  `sb v0,538(s0)` @0x801009a8) -> Routine 0x301 (`addiu v0,zero,769 / sw v0,4(s0)` @0x801009ac-b0); 1 -> 2, wenn z < -7322 (`lw v0,64(s0) / slti v0,v0,-7322`
  @0x801009d8-e0, @0x801009ec-fc). Routine 3 = Dash (Clip 2 Doppel-Advance @0x80100db4/dcc, Clip 3 @0x80100e28/e40); das Bissfenster setzt +0x218 |= 0x10 in Zone 2 ab
  Clip-3-Bild 114 (`lbu v0,333(s1) / sltiu v0,v0,0x72` @0x80100e84-8c, `ori v0,v0,0x10 / sh v0,536(s1)` @0x80100e98-ea4); am Ende Gier += 1024 (@0x80100eb0-c0).
- Der Treffer selbst kommt von der EXE-Kollision: das Overlay liest in seinem Spieler-Treffer-Einstieg @0x80101578 (Aufruf @0x8010155c) nur die Spieler-Wortmarke
  DAT_800cfc06 != 0 (`lhu v0,0(a1) / beq v0,zero` @0x80101590-98) und DAT_800cfdcb == 0 (@0x801015a4-ac), richtet den Wegstoss nach der eigenen Korridorlage (z < -8999
  @0x801015f0-f8, x < -15599 @0x80101628-30, Blick FUN_80015910) ueber FUN_8001569c(pl, Richtung, 1536) (@0x80101684-88) und gibt Schaden 60 bzw. 30 bei +0x218 Bit 5
  (`andi v0,v0,0x20 ... addiu a0,zero,60 / addiu a0,zero,30` @0x8010168c-a0) ueber @0x801038e4 -> FUN_800401d4 (einziger Aufruf im Overlay @0x80103918); danach
  Biss-Sperre +0x238 = 70 (@0x801016b0-b4, = RE15_GR2_BISS_CD_BISS).
- **Keine Hoehen- oder Abstandsbedingung im Overlay**: es gibt keinen Leser der Spieler-Koordinaten (0x800d0554/0x800d0558 nie gelesen; die drei Leser von 0x800d055c
  @0x80100a78/@0x80100ad4/@0x80100c7c sind obj[1].z des Kanisters, Zielmodus 8880/8601..9000, re15_kanister2080_r41.h). Die Beruehrungs-Pruefung liegt in der EXE
  (Koerper-Push mit Y-Band @0x8002b1f0-b228, im Port enemy_ai_common.c:4488-4502 byte-true: Push nur bei |dy| < Summe der Halbhoehen). RE2 kennt im Alligator-Raum keine
  erhoehte Flaeche - **Hochbiss und Rammen sind PORT-WAHL** (Nutzer-Design 2026-09-10 "nach oben schnappen", 2026-11-02 "gerammelt werden", 2026-11-06 Vorgabe A), die
  Treffer-Tests des Ports sind bereits hoehenfrei (re15_enemy_player_dist x/z, gb_koerper_kontakt x/z, Maulpunkt x/z).
- Das Tragen endet in RE2 nur durch den Schuss (Dossier NB1, Stufe 7 ohne Zaehler @0x80101b58-8c); ein Haenge-/Rueckfall-Zustand existiert in keinem Original - Vorgabe B
  ist PORT-WAHL nach dem Nutzer-Wortlaut.

## 4. Riegel (Soll; Bauer und Abnehmer messen mit denselben Werkzeugen dieselbe Zahl)

| Riegel | Messung (Werkzeug, Lagen, Dauer) | Soll | Herleitung | Ist heute |
|---|---|---|---|---|
| **A1 Harness Rampe oben** | `test_r41_gatorfix nb3_oben <leon\|elza> <neu\|ohne> 3600` (18 Raster-Lagen, Cut des Wegs von Sueden), 4 Laeufe | in JEDER Lage erster Treffer <= **600 Bilder** nach Beginn UND laengste Pause ohne Treffer <= 600 Bilder; Treffer = Rammen, CROSS-Biss oder Hochbiss mit Schaden (hp < 100) | 600 = Pause nach der Querung 300 (Z. 2621, Spur B "Leon OBEN 10 s", PORT-WAHL NB5) + laengste Querung 240 (gb_cross_begin Klemme 90..240, Z. 1139-1140) + Hochbiss-Fenster 45 (Clip 4, Z. 2566) = 585, aufgerundet; RE2 liefert keinen Takt fuer eine Figur ausserhalb des Korridors (Abschnitt 3) - PORT-WAHL nach "darf halt oben nicht sicher sein" | ohne: 2 Lagen nie, x3350/-16300 erst 1236; mit: dazu Suedreihe Pausen 697-959 |
| **A2 exe Rampe oben** | `batch3.sh oben_leon\|oben_elza neu\|ohne` + `oben3.py` (4 Laeufe) | solange die Figur oben ist: erster Treffer <= 600 Bilder nach dem ersten Oben-Bild (F668) und keine Pause > 600 Bilder; die Zeit nach einem Sturz ins Wasser zaehlt nicht | wie A1 | Leon ohne 410/410 ok, Leon mit 212/**837**, Elza 586/586 (knapp) |
| **B1 Harness Suedkante** | `test_r41_gatorfix nb3_haengt <leon\|elza> <neu\|ohne> 3600 3000,-18800,0 4073,-19322,0 4252,-19953,0 3000,-18800,2 4200,-18800,2 3600,-18750,2 6400,-18700,2 2318,-18568,2 4428,-18951,2 1000,-20700,2 3000,-21000,0` (11 Lagen) | Haenge-Fenster (RW-Mass) = **0** in jeder Lage, mit und ohne Kanister (= an diesen Orten loest der Rueckfall nie aus) | das Fenster ist die Definition des Rueckfalls (gator_rw_r41.c:49-52); "das hilft natuerlich nicht bei der Platform" -> die Verfolgung selbst darf das Fenster dort nicht fuellen | 1-3 an W4073, W4200, W4428 |
| **B2 exe Suedkante** | `batch3.sh rampe_leon\|rampe_elza\|wand_elza ohne\|neu` + `haeng3.py` (6 Laeufe) | Haenge-Fenster = 0; mit Kanister 0 Zeilen "G2RE2 HAENGT"/"RUECKWAERTS an"/"TAUCHEN an" in gator_boss.log | wie B1 | ohne 1/1/1, mit (rampe_leon) 2 Fenster + 2 Rueckwaerts |
| **B3 Obergrenze ohne Fortschritt** | B1- und B2-Laeufe, Mass "laengste Zeit ohne Fortschritt zur Figur" (Riegel re2 / mass_v4.py / haeng3.py) | <= **450 Bilder = 15 s** = 3 Haenge-Fenster | PORT-WAHL nach "Er darf nicht zu lange haengen": nach 150 Bildern ruft der Port Rueckwaerts, nach weiteren 150 (+30 Rueckwaerts) den Tauchgang (gator_rw_r41.c:277-280) - 450 ist die Zeit, nach der der Rueckfall selbst aufgegeben haette; die gemessenen "guten" Werte (mit Kanister 4,1-11,1 s, Dash 10,9 s = 327 B.) liegen darunter, der M4-Haenger (70,2 s) weit darueber | exe 6,4-7,6 s, Harness <= 10,9 s (gruen) |
| **B4 Bitgleich ausserhalb** | Riegel gang / v4 / re2 wie heute | gruen; ohne Kanister ausserhalb der Rampe-oben-Lagen und der Wasserlagen naeher als 1800 an einer Blockkante bitgleich NB5 | Nutzerentscheid: A und B sind die einzigen Aenderungen ohne Kanister | s. Abschnitt 5, Anpassungen |

Zusatz (Dokumentation, kein Riegel): Treffer je Lage und Art, Kopf-Abstand min/mittel, Wurzel-Kasten (nb3_oben), damit der Takt sichtbar bleibt ("Takt frei").

## 5. Vorschlag fuer den Bau (kleinster Eingriff, je Punkt mit Quelle; der Bauer entscheidet die Form)

**A - oben nicht sicher (beide Orte: Rampe und Plattform, mit und ohne Kanister):**
1. GUARD, Z. 2330-2337: den periodischen Seitenwechsel der Belagerung (`zielzone = cross_cd == 0 ? !zg2 : zg2`) fuer ALLE (nicht nur `re15_gr2_rw_traegt()`), und die
   Kamera-Bedingung Z. 2332-2337 streichen (Nutzer F1 "noe" / F3 "Das kann er gerne machen"): alle cross_cd 300 Bilder (Z. 2621, NB5-Wert) eine QUER-CROSS ueber die Figur
   (Z. 2339-2360), das Bahn-Ende folgt ihr (Z. 2605-2608) - damit gibt es auch in der Rampenmitte bei x 2318 eine Bahn, deren Koerper sie ueberstreicht (Bahn-x >= 2550,
   Kontaktkreis 1050 + 450 > 232).
2. CROSS, Z. 2728: `re15_gr2_rw_traegt() &&` streichen -> Rammen (gb_koerper_kontakt, Schaden gb_biss_abschluss = DAT_8006f418[5] = 50 @0x8006f422 + Niederwurf) auch ohne
   Kanister; der CROSS-Biss (Z. 2740-2744) bleibt. Messhaken s_gb_kreuz_kan (Z. 2358) nur noch Zaehlung.
3. Nicht noetig, nur falls A1 in der Mittelreihe dennoch rot bleibt: die GPAT-x-Klemme 4450 (Z. 2366, 2432; Kommentar Z. 1122-1127 rechnet noch mit dem ungeskalierten
   Radius 2200 -> Schatten 3950) auf GB_RAMP_X0 + hit_radius_min 1466 (2200 x 2731/4096, Z. 1527-1530) + Rand setzen -> Hochbiss-Reichweite bei x 2318 von 1366 auf ~1900.
4. Riegel-Folgen: `teil_gang` (dritte Lage (4428,-1800,-16300), test_r41_gatorfix.c:2392): diese Lage weicht nach A ab (Querung nach 300 Bildern) - Lage streichen oder nur
   bis Bild 300 vergleichen; `teil_re2`: "Gegenprobe ohne Kanister: kein Rammen auf der Rampe (r0 == 0)" und "Rampe-x2318: Treffer mit Kanister > ohne" (test_r41_gatorfix.c:2358, 2360)
   widersprechen Vorgabe A per Konstruktion -> umschreiben (Rammen >= 1 in beiden Staenden; x2318 >= 1 in beiden); `teil_v4` (mit = ohne bis zur Querung) bleibt.

**B - Haenger in der Verfolgung beseitigen (ohne und mit Kanister):**
1. Im CHASE-Zweig "verbunden" (Z. 1950-1962) und in gb_kanten_umweg (Z. 683, 722-737) den Fall "Figur selbst im aufgeblasenen Rechteck" (gb_seg_hits_rect "Punkt innen",
   Z. 384-386) nicht als Umweg behandeln: ist die Strecke zur Figur gegen den UNaufgeblasenen Block frei (`gb_seg_frei(e, zp, 0)`), Ziel = die Figur (Zweig 1); die
   SCA-Klemme (hit_radius_min 1466, re15_collision_constrain_enemy) haelt die Wurzel ohnehin von der Kante, der Biss reicht hinueber (Maul 2600 + 1500, LUNGE dist < 3200).
   Ist auch die unaufgeblasene Strecke blockiert (Ostnische (7668,-16403): die Rampe liegt dazwischen), Kantenziel wie die Belagerung (GUARD-Kantenziel Z. 2361-2369:
   Figur-x geklemmt, Kante + GB_GPAT_M) statt Westumlauf; von dort erreicht der Maulpunkt (6500,-17400) die Nische (1397 < 1500).
2. Pruefen mit `nb3_haengt` (B1: Fenster 0; Kopf-Abstand/Treffer in der Ausgabe) und den exe-Riegeln (B2); Gegenprobe: Stand a661eb8aa gibt 1-3 Fenster.
3. Riegel-Folgen: `teil_gang` zweite Lage (6732,0,-13687) liegt 813 vor der Rampen-Nordkante (im 1800-Band) und aendert sich mit B - Lage durch eine Wasserlage ausserhalb
   der Baender ersetzen (z. B. (6732,0,-12000)) oder dort nur bis zur ersten Abweichung vergleichen; `teil_re2` Lage "Nordbecken-Ost" (dort heute "Drehen 76 %") aendert
   sich ebenfalls (nur Ausgabe, kein Riegelwert ausser den Summen).
4. Nicht anfassen (Vorgabe): Rueckfall-Modul (bleibt fuer andere Orte), Querungsbeginn (F2), Dash nach der Querung (Offen 3 der Abnahme 4; 10,9 s ohne Fortschritt, unter
   B3).

## 6. Grenzen / nicht gemessen
- PSX-Bau; Raumwechsel; Elza mit Kanister in Lagen, in denen die Kamera-Regel die Querung erlaubt haette (der exe-Lauf blieb an der Nordkante: mit = ohne).
- Die Figur steht in Harness und exe still; ein laufender Spieler aendert den Takt (Takt frei laut Nutzer; die Riegel messen die stehende Figur).
- Hochbiss-Reichweite ist aus den Konstanten gerechnet (3900, 1900, 4450) und durch das Raster bestaetigt (x 3350 Mitte = Grenzfall 16 Treffer); nicht jede Zwischenlage
  gemessen.
