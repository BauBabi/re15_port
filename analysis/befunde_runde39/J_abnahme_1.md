# Runde 39 — Spur J "dining1141": Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Baum `.claude/worktrees/r39_dining1141`, Zweig `r39/dining1141`, Stand f744e5ad (Merge-Basis mit master 22abaed9;
master = 60734cd7). Gepruefter Diff: `git diff master...HEAD` (23 Dateien, nur Zweig-Aenderungen; die 7 weiteren Dateien in
`git diff master` - UEBERGABE.md, VERTRAG.md (Spur M), orchestrator/*, test_r38_itemorte.c - sind master-Commits nach
dem Abzweig). Code-Aenderung seit Abnahme 0 (b2a2f267): 17 Dateien, audio_pc.c (Punkt 9) unveraendert.
Massstab: AUFTRAG.md (Punkte 1-9 der Liste "Mit Room 1141 ..." + "Wenn man ROOM 1141 danach verlaesst ...", dazu die
Nutzer-Nachtraege 4 (N4.1-N4.4), 6 (N6) und 7 (Heranwinken wie 1171)), VERTRAG.md, Abnahme 0 (`J_abnahme_0.md`, Maengel
M1-M3). Messordner (nicht committet): scratchpad `J39abn1/` (Laeufe `l_<name>`, Werkzeuge `lauf.sh` = eigene exe-Kopie
`re15_pc_j39abn1_<name>.exe` neben der Baum-exe, danach geloescht; `cap.py` Pegel je Takt; `bogen.py`/`crop.py` Bilder).

## 0. Stand der Abnahme (fortlaufend)
- 22:14 Abnahme begonnen; AUFTRAG/VERTRAG/Dossier Abschnitt 8/Abnahme 0 gelesen, Code-Diff gelesen.
- 22:16 Bau: `local_build.sh configure` + `build` -> `LOCAL-BUILD-OK`, `ninja: no work to do` (exe 21:47:36 = Stand
  751e3f4d; f744e5ad aendert nur Kommentare/Riegel-Text, test_r39_dining1141.exe 22:12:56).
- 22:18 Generator `programme_bauen.py --pruefen` -> `dining_r39_programme.inc ok`; Unit gezielt 164/164 gruen.
- 22:20-22:36 Laeufe A (echter Weg 1131 -> 1141 -> 1171, Messer), A0 (Fruehphase 1141), P (Pistole im Inventar),
  L (lebende Gorillas, Pistole) gemessen; RE-Stichproben (8 Stellen) selbst gelesen.
- 22:37 Integrationsriegel (gezielt, 18 Tests, -j 4) gestartet; Laeufe L2/L3 (lebende Gorillas, andere Lage Elzas).
- 22:40-22:56 Laeufe F/E/R (Reaktion/M1), G2 (Wiederbetreten), A2 (dichte Bilder Cut 7), Q (ausgeruestete Schrotflinte)
  ausgewertet; Integration 18/18 gruen (22:51).

## 1. Bau und gezielte Tests
- `bash re15_port/tools/local_build.sh configure` + `build`: `LOCAL-BUILD-OK (configure)`, `LOCAL-BUILD-OK (build)`,
  `ninja: no work to do` (exe 21:47:36; Quellen dining_r39.c 21:47:30, gen/*.inc 21:44:32 aelter).
- Generator: `python re15_port/tools/r39_dining/programme_bauen.py --pruefen` -> `...dining_r39_programme.inc ok`
  (Listing scratchpad `J39abn1/listing.txt`).
- Unit gezielt (Module des Diffs: dining_r38/39, Kartenbits, Gorilla-Hirn enemy_ai_common.c, RBJ-Kanal enemy_common.c,
  Schaden re15_damage.c, scd_vm.c (Plc_motion/Message_on/VM-Takt), NPC-/Anim-/Spieler-Ziel-Pfad, Ton/Stimme/BGM
  audio_pc.c, Nachrichten-FSM, Heliport 1171, Gegner-/Treffer-/Elza-Gruppen der Runde 39), Regex in `J39abn1/regex_unit.txt`
  (= Regex des Bau-Agenten + `unit_scd_|prop_scd_pc_advance|unit_npc_|unit_r38_npckante|unit_anim_|unit_re2_weapon_rows|
  unit_r17_waffen_loop_pin|unit_enemy_attack_reach|unit_r39_elza|unit_r37_elzaems|unit_r35_zgirl_messer|
  r28_messer_dauerschlag|unit_1090_push_anim|unit_1090_npc_prop_push|unit_r39_treffer|unit_r39_zustand`):
  **100% tests passed, 0 tests failed out of 164** (16,2 s, -j 6).
- `test_r39_dining1141 {texte,programme,stamm,phase_h,fenster}` einzeln: je `0 Fehler` (Ausgaben `J39abn1/u_*.txt`).
- Integration gezielt (Spur-Riegel, alle R38-dining-Teile, BGM-Wechsel 10F0, Weg nach 1171, Stimmen-Pfad, Elza in 1141,
  Elza-Schuss, Gorilla-Riegel der Runde 38 wegen des Gorilla-Hirn-Hakens), Regex `J39abn1/regex_integ.txt`:
  `ctest -R "integration_r39_dining1141_d|integration_r38_dining1141|integration_r35_cut10f0|integration_r38_heliport1171_weg_re15|
  integration_r39_stimmen_1171|integration_r38_rekorder|integration_r38_elzaintro_stimme|integration_r37_elzaems_elza1141|
  integration_r36_elza_schuss|integration_r38_affen3" -j 4 --timeout 3600` -> **100% tests passed, 0 tests failed out of 18**
  (636,7 s; integration_r39_dining1141_d 337,5 s, Lauf `r39_dining1141_d_wd/lauf_5765100f`: Riegel F1599, Schuesse
  F3857/F3887, Heranwinken F4119 - bildgleich zu meinem Lauf A; integration_r38_dining1141_s 636,7 s; _a/_d 0,1 s =
  Platzhalter "ersetzt" wie in Abnahme 0, Hinweis H4).

## 2. RE-Gate: Stichproben selbst gelesen (richtige Binaerdatei, Sprungziel)
| # | Zitat im Diff | selbst gelesen | Befund |
|---|---|---|---|
| 1 | Spieler-Plc_motion(2,c) = Sub 0x80073e30[2] = 0x80050f00 (@0x8003068c/@0x8003069c), Handler @0x80041bc4 | `re15_disasm.py dis 0x80041b90`: `80041b98 lbu a2,1(v0)` ... `80041bb0 sb v1,4(v0)` (+0x4=4) ... `80041bc4 sb a2,5(v0)` (+0x5 = pc[1]); Zustands-Tabelle: `80031c8c lbu v1,-13736` (aca58 = Spieler+0x4) / `80031ca4 addiu at,at,16272` (0x80073f90) / `jalr`, `table 0x80073f90`: [4] = 0x80030660; `dis 0x80030660`: `8003068c lbu v0,-13735(v0)` (aca59 = +0x5), `8003069c addiu at,at,15920` (0x80073e30), `jalr`; `table 0x80073e30`: [2] = 0x80050f00; `dis 0x80050f00`: `80050f88 lw a0,368(v0)` / `80050f90 lw a1,372(v0)` / `jal 0x8001f314` (Kanal +0x170/+0x174), Ende `80050fec sb v0,6(v1)` (+0x6 = 2 halten) | stimmt |
| 2 | W-Bank = Kanal +0x170/+0x174 des Spielers, Lader @0x80036be4/@0x80036c04 | `dis 0x80036b68`: `80036b84 lbu v0,-13732` (aca5c Figur), `80036b8c lbu a0,-13731` (aca5d Waffe), `jal 0x80013b60` (PLW laden nach 0x801d7700), `80036be4 sw v0,-13368(at)` = 0x800acbc8 = 0x800aca54+0x174, `80036c04 sw v0,-13372(at)` = 0x800acbc4 = +0x170 | stimmt |
| 3 | Johns Schaden = Gorilla-Zeile Spalte 3 @0x8006ee44 = 12 | `read 0x8006ee38 22 --w 4` (0x8006e0d0 + 0x27*0x58): `[0, 7, 30, 12, ...]`, Spalte 3 = **12** | stimmt |
| 4 | RBJ = Raumkopf+0x5C, Lader FUN_8001b3f8 @0x8001b404; Marker 0x2 = Gegner 0 | `dis 0x8001b3f8`: `8001b404 lw a2,92(v0)`; Gegner-Binder `8001b494 addiu at,at,-12884` (0x800acdac = 0x800acc2c+0x180) mit Schritt 500 = 0x1F4 je Gegner; Sub [0] 0x80050cb8 liest `lw a0,384(v0)`/`lw a1,388(v0)` (+0x180/+0x184) | stimmt |
| 5 | ROOM1171 RBJ rec 1 = Marker 0x2, EDD @RBJ+0x3E34, Clips {20,30,96} | eigener Parser auf `info/Re1.5/PSX/STAGE1/ROOM1171.RDT` (= shared_assets, bytegleich): RBJ @0x188C, `[0]=0x408C count=2`; rec0 Marker 0x1 Clips [20,30]; **rec1 Marker 0x2, EDD RBJ+0x3E34, Clips [20,30,96]**; John ist in 1141 und 1171 Gegner 0 Typ 0x49 (`44 00 49 ..` 1141 @0x00B06, 1171 @0x016C8) = dasselbe Skelett | stimmt |
| 6 | Messer DRAW = Melee-Sub @0x80035538 Clip 0x0D, Ton 0x1080001 @0x800355f8 | `dis 0x80035570`: `8003558c ori v0,zero,0xd` / `80035594 sb v0,-13592(at)` (0x800acae8 = Spieler+0x94 Clip); `800355d4 lui a0,0x108` / `800355d8 ori a0,a0,0x1` / `800355f8 jal 0x80045024` | stimmt |
| 7 | Trefferkegel 0xC8 = Gun-FSM @0x80032fe0; Messer-Rueckfall @0x8004666c/@0x80046688 | `80032fe0 ori a1,zero,0xc8`; `8004666c ori v0,zero,0x1`, `80046688 sb v0,-13731(at)` (aca5d) | stimmt (0xC8 als PORT-WAHL fuer Johns Treffer gekennzeichnet) |
| 8 | RDT-Formen: 1141 sub05 @0x00FC8..@0x00FE6, main00 @0x00A72/@0x00A76/@0x00A9A, sub06 @0x00FE8/@0x010C8; ROOM3060 @0x0108E/@0x010BA/@0x010BE/@0x010C2; ROOM1021 @0x020C6..@0x020F2; ROOM1171 @0x014BA..@0x014D2 | Dateien selbst gelesen (shared_assets == info/Re1.5/PSX, bytegleich): `46 02 00 00 ..` / `22 04 c3 01` / `2b 0c ff ff` / `02 00` / `46 01 02 31 ..` / `01 00`; `06 00 3c 00` / `21 04 c3 00` / `2c 02 03 31 .. ff 00 18 05`; `2b 0d 80 ff` / `22 02 07 00 22 01 1b 00 3c 01 2e 01 00 00 42 00 08 00`; 3060 `40 00 09 00` / `2e 01 00 00` / `3f 02 06 00` / `09 0a 28 00`; 1021 `3f 02 06 00 09 0a 6e 00 3f 02 07 00 09 0a 1e 00 3f 02 07 00 .. 3f 02 06 00 43 00 80 00 09 0a 14 00`; 1171 `2b 00 00 00 2e 02 00 00 3f 02 06 00 43 00 80 00 09 0a 14 00 3f 00 02 00 09 0a 3c 00` | stimmt |
Bank-5-Merker (5,0x10)/(5,0x11): `tools/scd_dump_room.py ROOM1141.RDT` -> Bank 5 nur Bit 0/1 (sub03/sub04 @0x00FA4/@0x00FAC/
@0x00FB6/@0x00FBE); Bank 5 Wort 0 ist der pro-Raum-Scratch (scd_room_setup.c FUN_8003ecec); Plc_dest-Warte-Bits der
Programme = Bit 0/1 - keine Kollision. (VERTRAG 1.1 regelt nur Bank 9.)
Suche im Code-Diff (`+`-Zeilen) nach `deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|geschaetzt|
approx|ungefaehr|platzhalter|placeholder`: **0 Treffer**. Neue getenv: nur `RE15_R39J_LOG` (Messschiene) und `RE15_SE_DEBUG`.
Konstanten ohne Original-Adresse sind gekennzeichnet: N4.3-Takt (Sleep 90+90), Kegel 0xC8, Dauer 180, NAH 2000, Takt
30/Schritt 300 = PORT-WAHL; Lagen = PORT-WAHL mit Bild-/SCA-Beleg; Texte = NUTZER-VORGABE; Sleeps = Formen mit Adresse.

## 3. Messlaeufe (echte exe, eigene Kopie je Lauf)
Gemeinsame Umgebung (`J39abn1/lauf.sh`): RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_PSELECT_AUTO(_SWITCH)=1 RE15_MSG_LOG=1
RE15_SE_DEBUG=1 RE15_EVT_TRACE=1 RE15_BGM_CTL_DEBUG=1 RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw
RE15_R39J_LOG=r39.txt RE15_WEGPLAN=<plan> RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78 (Nach-1021-Flags wie der
Riegel) RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0 RE15_FRAMEDUMP=<bereich>.
Bildnummern = Bilder seit Raumeintritt (main.c:8222 setzt frame_count je Raum zurueck); Takt = Mitschnitt-Takt
(1141: Takt = F + 3668 in Lauf A, F + 3665 in Lauf P).
| Lauf | Wegplan / Umgebung | Ergebnis |
|---|---|---|
| A | `plan_a.txt` = Wegplan von integration_r39_dining1141_d (1131 Schluessel+Tuer, 1141 WARTE 420, Gemaelde, 200 freie Bilder, Riegel, Szene, Doppeltuer msg 41, Kampf, Dachtuer -> 1171); RE15_GIVE=0x43:1 (nur Schluessel = Messer); RE15_R38F_GORILLAS_TOT=600 + _STARR=1; EXIT 600#1171; FD 0-14000/5 | rv=0, 407 s, 1131 -> 1141 -> 1171 |
| A0 | wie A, EXIT 700#1141, FD 0-700/5 (Fruehphase 1141 ohne Ueberschreiben durch 1171-Bilder) | rv=0, 150 s |
| P | wie A, RE15_GIVE=0x43:1,0x03:15,0x15:50 (Browning im Inventar, NICHT ausgeruestet); FD 650-14000/5 | rv=0, 410 s, 1131 -> 1141 -> 1171 |
| L | wie P, Gorillas LEBEND (kein _STARR), RE15_R38F_GORILLAS_TOT=2400, EXIT 5500#1141 | rv=0, 347 s (Elza im Kampf festgeklemmt bei (-11068,-19459), Wegplan "Klemme") |

### Punkt 1 — keine Zombie-Geraeusche
- debug.log Abschnitt 1141 (Lauf A Z. 1426..3450, P, L), alle Skript-Toene gezaehlt (`SCD Se_on: bank=`): 18x bank 62 Satz 0
  (Klopfen), 6 Hilferufe (Satz 49..53 = 0x31..0x35; help03 zweimal), 2x Satz 33 (Glas), 3x bank 59 (Rolladen 0x3B),
  2x2 bank 14 (DOOR1A). **0 Zeilen** `bank=0x3C`, `bank=0x3D`, `bank=60 `, `bank=61 `, `main42` (Ausnahme: der
  Infotext `[dining39] ... statt Zombie-Stoehnen/"Help me out."`). In Lauf L kommen im Kampf nur die Gorilla-Stimmen
  (EM27-Bank, 11025 Hz) dazu.
- **Urteil: erfuellt.**

### Punkt 2 — John durch die Tuer; Kamera vorne auf Elza, Tuer-zu-Ton, dann Kamera auf die Tuer mit John
- Lauf P (debug.log + r39.txt + state.txt): `[scd F1707] Cut_chg(7)` (angezeigt F1708, Elza laeuft ins Bild), Elza
  dreht sich F1722..F1744 zur Dachtuer (rot 847 -> 3101) und hebt die Waffe ab F1744; `Se_on bank=14 id=0` (DOOR1A auf)
  zwischen fd_001780 und fd_001785; John erstmals ausserhalb der Parklage F1784 bei (-7350,-1300) (Cut 7, hinter der
  Kamera); `Se_on bank=14 id=1` (zu) zwischen fd_001825 und fd_001830; `[scd F1853] Cut_chg(6)`, angezeigt F1854: John
  steht im Dachgang vor der Tuer (Bild `J39abn1/p_tuer.png` F1855..F1870); F1874 Cut 5, John laeuft heraus.
- Mitschnitt Lauf P: Tuer-auf Takt 5452..5468 (Spitze 11320 ueber MAIN16-Grund 780..1160), Tuer-zu Takt 5492..5504
  (Spitze 7741); danach MAIN16-Stopp (op=2 Takt 5521) -> Takt 5536 RMS 8,6.
- Kein Bild zeigt John vor Cut 6 (Lauf A/P r39.txt: John geparkt bis F1783, ab F1784 nur Cut 7 = hinter der Kamera).
- **Urteil: erfuellt.**

### Punkt 3 — kein "shout" fuer John
- 0 Zeilen `bank=0x3D`/`main42.wav` in den 1141-Abschnitten von A, P, L. Stimmen in Phase H nur help01..05 (Nutzer-Dateien).
- **Urteil: erfuellt.**

### Punkt 4 — help01..05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen
- Lauf A debug.log/r39.txt: Klopfen F35/F40/F45 -> help03 F75 -> [Reaktion] -> Klopfen F355/360/365 -> help01 F395 ->
  Klopfen F575/580/585 -> help04 F615 -> (Gemaelde F788, Rolladen) Klopfen F984/989/994 -> help02 F1024 -> Klopfen
  F1204/1209/1214 -> help05 F1244 -> Klopfen F1424/1429/1434 -> help03 F1464 (Elza laeuft schon frei durch den Raum) ->
  Riegel F1599: danach kein Schlag/Ruf mehr (Zaehler se=18/0/6 bleibt). Reihenfolge 3,1,4,2,5,3 (PORT-WAHL MIX),
  Abstand 220 Bilder, vor jedem Ruf drei Schlaege im Abstand 5.
- **Urteil: erfuellt** (Klangwahl RE2 DOOR0F-Holzschlag bleibt Hinweis an den Nutzer, Abnahme 0 H1).

### Punkt 6 — Glasbruch vor "What was that?!"
- Lauf A: msg 5 (John, main05.wav) zu F2538; `GLAS1090 Glasbruch` F2538 (Takt 6206) + F2543 (Takt 6211); msg 6
  `"Elza: What was that?!"` offen ab F2564. Mitschnitt: Takt 6198..6204 RMS 87 -> 0 (Johns Satz zu Ende, Stille),
  Takt 6206..6218 RMS 8200..10100, Spitze 28672 (Glas), danach Ausklang; main06 ab ~Takt 6232.
- **Urteil: erfuellt.**

### Punkt 7 — John schaut ab "...some kind of gorilla..." zu Elza
- Lauf A und P (r39.txt, `blick` = Abweichung Johns Blick zur Peilung auf Elza, 4096 = 360 Grad): msg 9 169 Bilder
  F2931..F3099 |blick| <= 10 (0,9 Grad); msg 10 99 Bilder |blick| <= 10; msg 11 147 Bilder, nur die letzten 12 Bilder
  > 128 = John laeuft los in seine Ecke (s1=5, Original sub02 @0x00F8E).
- **Urteil: erfuellt.**

### Punkt 9 — keine Panik-Musik beim Wechsel 1141 -> 1171
- Werkzeug des Riegels auf meine Laeufe: `test_r39_dining1141 pegel J39abn1/l_a` -> `ROOM1171 main00 Sce_bgm_control op=1
  bei Takt 9980`, `Takte 10010..10035 Pegel 0, Takte 10050..10080 Pegel 559`, `pegel: 0 Fehler` (Lauf P: op=1 Takt 9977,
  Pegel 0 / 559).
- Mitschnitt Lauf A (`cap.py l_a/cap.raw 9860 10100 6`): Tuer-auf Takt 9866..9878, Stille 9908..9968, Tuer-zu
  9974..9998, Stille 10010..10028, MAIN32 ab 10034 (RMS 7 -> 1100). Kein MAIN16-Nachhall. (In 1141 stoppt die Kampfmusik
  beim Tod der Gorillas, op=2 Takt 7615.)
- Mechanismus/Gegenprobe: audio_pc.c ist seit Abnahme 0 unveraendert (`git diff b2a2f267 HEAD -- .../audio_pc.c` leer);
  die Gegenprobe V/W der Abnahme 0 (ohne Haken Pegel 496 = MAIN16) gilt weiter.
- **Urteil: erfuellt.**

### Punkt 5 — Elzas Reaktion beim ersten Hoeren (mit M1 der Abnahme 0)
Weitere Laeufe (Debug-Sprung direkt in 1141, Phase-H-Flags + (9,66), Elza vor dem Gemaelde `RE15_PLAYER_POS=-8150,-10250,2048`):
| Lauf | Wegplan | Ergebnis (r39.txt / debug.log) |
|---|---|---|
| A, A0 | echter Weg, 420 Bilder stehen | help03 F75; Pad-Sperre F166; msg 50 offen **F176..F275**, (9,94)=1 F266; einzige Reaktion im Raum |
| F (M1-Fenster der Abnahme 0) | `plan_f.txt` (= jabn/plan_f.txt): WARTE 100, dann Gemaelde | help03 F76, Frage msg 13 F133..F248, "ja" -> (3,0x6A) + Rolladen pm=2 **F250..F470**, Reaktion: pm 1->2 F471, **msg 50 F481..F580** (vorher Abnahme 0: F291, mitten im Hochfahren). Bild `J39abn1/f_m1.png`: F450/F465 Rolladen oben, Balken weg; F480 Text erscheint bei offenem Rolladen |
| E (Gemaelde VOR dem ersten Ruf) | `plan_e.txt`: Gemaelde ab F31 | Frage F33..F148; Rolladen F150..F370; Klopfen laeuft jetzt WEITER (F151/156/161, help03 F191 waehrend des Hochfahrens, N4.1); msg 50 **F381..F480** nach dem Rolladen; danach Schleife weiter (help01 F601 ...) |
| R (schnellster Weg zum Riegel) | `plan_r.txt`: Gemaelde F31, sofort zur Dachtuer | wie E bis F485 (Reaktion F381..F480 unter Pad-Sperre), dann frei, Riegel F685 (`Dachtuer entriegelt`), msg 12 F705: die Reaktion kommt auch beim schnellsten Spieler genau einmal und VOR dem Entriegeln |
- M1 behoben: die Reaktion erscheint in keinem Lauf waehrend einer Szene; ohne Gemaelde kommt sie wie bisher 101 Bilder
  nach dem ersten Ruf. Wortlaut `"Elza: Oh, no, this is John! He is in trouble, I have to help him!"` (N6 klein).
- **Urteil: erfuellt.**

### Punkt 8 — nach dem Tod der Gorillas zwei Zeilen Johns mit passenden Animationen (mit M2/M3 der Abnahme 0)
- Lauf A (r39.txt/debug.log): `F3947 MESSHAKEN RE15_R38F_GORILLAS_TOT: 2 Gorillas heraus` (Kill-Flags, keine
  Schusszahl); Johns letzte Salve war zu Ende; F3978 Cut 8 + Pad-Sperre; msg 51 `"John: Puuuh, that was close! I thought
  we end up as gorilla meal...."` offen F4000..F4098, msg 52 `"John: Elza, follow me, I think I might have found a way out
  of here!"` offen F4120..F4218; `[scd F4219] Cut_chg(7)` (msg 52 ist zu, Abnahme-0-H3 behoben), John laeuft zur
  Dachtuer, F4347 geparkt, (9,67)=1, F4376 Steuerung frei.
- Blick: msg 51/52 je 99 Bilder `blick=0` (John schaut Elza an).
- Gesten: msg 51 s1/mo = (0,15) (Original-Geste zu msg 1 "I'm glad to see you're still alive", sub02 @0x00CB8), danach
  (0,23); msg 52 alle 99 Bilder (0,2) = **Heranwinken**: `[rbj] ROOM1141: Animationsblock von ROOM1171 fuer Johns
  Heranwinken gelesen (16540 B)`, `F4119 John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)`.
  Bilder `J39abn1/a_gespraech.png` (F3975..F4230) und Ausschnitt `a_wink_zoom.png` (F4120..F4235 je 5 Bilder): John dreht
  den Oberkoerper und schwingt den linken Arm von vorn ueber die Schulter zurueck (zweimal), die Pistole haengt in der
  rechten Hand nach unten - dieselbe Bewegung wie das korrigierte "Elza, hurry up!" der Spur K
  (scratchpad `K39abn1/hurry_up_head_vs_v0826.png`, HEAD F532..F580). Kein Bild mit auf Elza gerichteter Waffe
  (Sub 2 Clip 6 im Gespraech: 0 Bilder).
- M3: groesster Schritt Johns je Bild ohne Kameraschnitt und ohne Lauf-Sub in A, P und L: **0** (Pos_set im
  Schnittbild: F3978 erstes Bild Cut 8 mit John (-7550,-21500) und Elza (-5800,-21500); F3975 noch Cut 0).
- **Urteil: erfuellt** (Zeilen, Reihenfolge, Blick, Gesten; Heranwinken = Nutzer-Nachtrag 7).

### N6 (Nachtrag 6) — Zeilen korrekt geschrieben
- debug.log A/P/F/E/R: `"... He is in trouble, I have to help him!"`, `"... we end up as gorilla meal...."`; Bilder
  `f_m1.png` F480..F540, `a_gespraech.png` F4000..F4090. Riegel unit_r39_dining1141_texte und integration (pruef_nicht
  "He is in Trouble"/"as Gorilla meal").
- **Urteil: erfuellt.**

### N4.1 (Nachtrag 4) — Rolltor oeffnen und danach ganz normal laufen
- Lauf A: Gemaelde F788, Frage bis F979, Rolladen pm=2 F981..F1214, ab F1215 pm=0 (Balken weg, Kamera-Automatik); Elza
  steht 200 freie Bilder und geht dann durch die Rolladen-Oeffnung zur Dachtuer (F1425..F1570), John klopft und ruft
  weiter (F1424..F1464). Bild `a_rolltor_frei.png` (F790..F1640).
- Lauf G2 (Wiederbetreten, `plan_g2.txt`, Debug-Sprung 1141): Reaktion F176, Gemaelde F646, Rolladen F837..F1071, Elza
  geht durch die Doppeltuer nach 1131 und zurueck; zweiter Besuch: John geparkt, Klopfen + help03 ab F36/F76, keine
  zweite Reaktion ((9,94)=1), Rolladen offen ((3,0x6A)=1), Riegel F801 -> msg 12 -> Szene laeuft.
- **Urteil: erfuellt.**

### N4.2 (Nachtrag 4) — Szene erst nach dem Entriegeln; Elza rennt zur anderen Kamera, dreht sich zur Tuer, hebt die Waffe
- Lauf A (Messer) / P (Pistole im Inventar, nicht ausgeruestet): Riegel F1599 -> `Dachtuer entriegelt: Elza hebt Waffe 1
  (vorher 1, Messer)` bzw. `... Waffe 3 (vorher 1, Schusswaffe)`, msg 12 F1601..F1644; ab F1645 Pad-Sperre; Elza
  RENNT (state.txt mo=100, 1200 Einheiten je 6 Bilder) durch den Gang nach Sueden (Cut 6 -> Cut 5), Schnitt auf Cut 7
  F1708 (Kamera am Gangende mit Blick nach Sueden, die Tuer liegt hinter der Kamera); Elza laeuft ins Bild (dichte Bilder
  `a2_cut7.png` F1704..F1726: sie kommt unter der Kamera hervor, kein Cut-0-Blitz), dreht F1722..F1740 zur Tuer (rot 847 ->
  3101) und hebt F1744 die Waffe: Lauf P Pistole beidhaendig hoch (`p_heben_zoom.png` F1750..F1850, gehalten bis Cut 6),
  Lauf A Messer per DRAW gezogen (`a_messer_zoom.png`). Danach Tuer auf/zu, Cut 6 mit John, Cut 5 Waffe senken (P: w 6
  rueckwaerts F1874..F1919). Wortlaut "Dann kommt alles das, was jetzt schon drin ist": msg 40, 3..11 wie zuvor.
- Wiederholbarkeit: A2 (Wiederholung von A) r39.txt F1600..F1799 bytegleich zu A.
- **Urteil: erfuellt.**

### N4.3 (Nachtrag 4) — "John sollte auch ab und an mal auf die Gorillas schiessen ... und dann ihre Aufmerksamkeit erhalten, sonst ist es komisch"
Mit eingefrorenen Gorillas (Laeufe A/P = Riegel-Umgebung) ist nur der Schuss messbar: 2 Schuesse F3857/F3887, 2 Treffer,
Aufmerksamkeit endet nach 29 Bildern (Gorillas starr). Der Nutzerfall ist der Kampf mit LEBENDEN Gorillas - drei eigene
Laeufe (Pistole im Inventar, ohne _STARR, RE15_R38F_GORILLAS_TOT=2400; Elza kaempft nicht, der Wegplan laesst sie stehen):
L = `plan_a.txt` (Elza klemmt bei (-11068,-19459)), L2 = `plan_l2.txt` (Elza im Gang (-6213,-4618)), L3 = `plan_l3.txt`
(Elza neben John (-5800,-20500), stirbt F5319). Schuesse aus debug.log (`[dining39] F.. John schiesst ... Treffer|daneben`),
Gorilla-Zustand/Lage/Drehung aus state.txt, Aufmerksamkeit aus r39.txt (Werkzeug `J39abn1/aufmerk.py`):
| Salve (je 2 Schuss) | Lauf | Wirkung |
|---|---|---|
| F3771, F4142, F4513 | L | **kein Treffer**: Elza im Griff (state.txt gr=1 F3682..F3805 / F4054..F4177 / F4501..F4613, Gorilla 2 Sub 15 Phase 3); Ziel nachgerechnet (atan2_q12/arc_test wie re15_damage.c) im Kegel: rel -43/-54, -14/-25, -10/-10 (Kegel +-200), Ziel nicht in einem Angriffs-Sub (st=1, ss1=2/3) -> es sperrt nur `re15_player_victim_state() != 0`; F4142/F4513 zielten auf Gorilla 3, der Elza NICHT hielt |
| F4884 | L | Treffer: Gorilla 3 dreht r1935 -> r1199 (zu John) und rueckt 960 naeher (z -18426 -> -19384), 89 Bilder, klemmt dann bei x -6982 |
| F5256 | L | Treffer: Drehung r2031 -> r1428, keine Bewegung (x -6982), 2 x 29 Bilder |
| F3765 | L2 | Treffer: Gorilla 2 rueckt 3700 naeher (x -3290 -> -6982), 119 Bilder |
| F4137 | L2 | **kein Treffer**: John drehte F4026 korrekt auf die damalige Lage des Ziels (yaw 3723), das Ziel lief in den 110 Zielbildern weg (Schuss bei (-6804,-12570), rel -599/-612, ausserhalb +-0xC8) |
| F4514, F4885, F5256, F5627 | L2 | 8 Treffer auf Gorilla 3: **weder Drehung noch Bewegung** (r3157 -> r3163, Lage (-6982,-12515) fest), je 29 Bilder Aufmerksamkeit, nur das Zucken |
| F3771, F4142 | L3 | **kein Treffer**: Elza im Griff (gr=1 F3700..F3823 / F4060..F4183); Ziel im Kegel (rel +21/+26, -57/-18); F3771 Ziel selbst im Griff-Sub 15 Phase 5, F3801/F4142/F4172 Ziel in Sub 2 -> nur der Opferzustand sperrt |
| F4513 | L3 | Treffer: Drehung r4073 -> r1759 und Weg zu John/Elza (-6268,-18560) -> (-5674,-20495) |
| F4891 | L3 | Treffer: Drehung und 7000 Einheiten Weg entlang z -21982 auf John zu, 179 Bilder |
| F5263 | L3 | Treffer: Drehung r3973 -> r288, keine Annaeherung (12745 -> 13317) |
- Summe 16 Salven: **6 ohne Treffer** (5x Elza im Griff, 1x Ziel aus dem Kegel gelaufen), **4 mit Treffer, aber ohne jede
  Zuwendung** (nur das Zucken), **2 nur Drehung**, **4 Drehung + sichtbares Heranruecken** (960..7000 Einheiten, endet
  2000..2700 vor John an der Grenze x -6982 oder nach 30 Bildern ohne 300 Fortschritt; danach wieder Elza). Kein Gorilla
  greift John an (PORT-WAHL).
- John ist beim Schiessen in keinem Lauf im Bild (Kamera folgt Elza; `l_schuss.png` F3760..F5290) - der Spieler hoert die
  Schuesse und sieht hoechstens das Zucken oder das Abwenden eines Gorillas.
- Gerade wenn ein Gorilla Elza packt - die Lage, in der Johns Ablenkung sichtbar werden muesste - bleiben alle Schuesse
  wirkungslos, auch auf den ANDEREN Gorilla (Code-Kommentar dining_r39.c "das Original kennt keinen Dritten, der einen
  Angriff auf den Spieler unterbricht"); diese Regel steht nicht in "Fuer den Nutzer" (Dossier 8.6).
- **Urteil: teilweise** — John schiesst ab und an (Takt ~12,4 s, hoerbar, Treffer mit Pistolenschaden 12 @0x8006ee44),
  die Aufmerksamkeit kommt aber nur in 6 von 16 Salven sichtbar an (4 davon mit Heranruecken); in 10 von 16 Salven bleibt
  ausser einem Zucken nichts (Mangel M1).

### Zusatz N4.2 — Waffenregel (AUFTRAG Nachtrag 4: Schusswaffe im Inventar, sonst Messer)
| Lauf | Inventar / Ausruestung | debug.log | Bild |
|---|---|---|---|
| A | nur Schluessel (Messer) | `Dachtuer entriegelt: Elza hebt Waffe 1 (vorher 1, Messer)`, W-Clip 13 (DRAW) | `a_messer_zoom.png` |
| P | Browning im Inventar, Messer ausgeruestet | `... hebt Waffe 3 (vorher 1, Schusswaffe)` (aus dem Inventar ausgeruestet) | `p_heben_zoom.png` |
| Q | Schrotflinte 0x08 AUSGERUESTET (RE15_EQUIP=8) + Browning im Inventar; Kurzweg Phase H2 (`plan_q.txt`) | `... hebt Waffe 8 (vorher 8, Schusswaffe)` (die ausgeruestete bleibt) | `q_schrot_zoom.png` F345..F440 |

## 4. Weitere Beobachtungen aus den Laeufen
- Punkt 4 nach dem Rolladen hoerbar (Lauf A, Mitschnitt): Schlaege Takt 5093/5098/5103 Spitze 11398/8949/11671 ueber
  MAIN16 (RMS 1000..2000).
- Johns Schuss-Ton: je Schuss `[se] Stimme: se=1 ... 16397 Hz` im Schussbild (debug.log Lauf L Z. 2967); im Kampflaerm
  (Kampfmusik + Gorilla-Stimmen) nur als Pegelspitze zu erkennen (Lauf L Takt 8549..8553 RMS 3300..4100, Spitze 13792).
- Lauf R: Riegel F685, msg 12 erst F705 - die Nachricht wartete auf das Ende von help01 (F601, 3,42 s; Stimmen-Riegel
  op_message_on), d. h. "You've opened the lock." kommt 0,6 s spaeter (Hinweis H5, = H6 der Abnahme 0, jetzt gemessen).

## 5. Vertrag (VERTRAG.md Runde 39)
- Pfade (`git diff master...HEAD --name-only`, 23 Dateien): `analysis/befunde_runde39/{AUFTRAG.md (nur Orchestrator-Commits
  c4e692ac, 4d7c7b34, 2f889c9f, 18f4306a, f60715a8, b6f25fdf, 363626f4), J_abnahme_0.md, J_dining1141.md}` und
  `re15_port/{engine/src, include, platform/pc, platform/pc/src, tests/unit, tests/unit/probes, tests/integration,
  tools/r39_dining}`. Kein `release/`, `platform/android/`, `shared_assets/PSX/`, `synchro/`, keine Test-CMakeLists
  (Riegel ueber `tests/unit/probes/r39_dining1141.cmake`).
- Bank 9: nur Bit 94 (k_port[] +1 Zeile, unit_r36_kartenbits gruen); Bit 95 ungenutzt. Bank 5 (Raum-Scratch): 0x10/0x11,
  Original-1141 nutzt nur Bit 0/1 (Abschnitt 2).
- Nachrichten-Ids: 50/51/52 (Text), 53..57 (Sprach-Ids help01..05, ohne Text), 58 (Warte-Zeile) - im Bereich 50..59;
  `synchro/STAGE1/room1141/` hat keine main50..58.wav (nichts umgewidmet), Nutzer-Dateien unveraendert.
- Gemeinsame Dateien, je Haken mit "Runde 39 Spur J" und auf ROOM1141 begrenzt: scd_vm.c (+7: VM-Anker, Message_on 58,
  Plc_motion), platform/pc/main.c (+2), dining_r38.c (J-eigen), dining_r38_pc.c (+1), test_r36_kartenbits.c (+2) -
  zugeteilt; **ausserhalb der Zuteilung 1.3**: audio_pc.c +5 (A/B), enemy_ai_common.c +7/-2 (D/E, zwei Haken im
  Gorilla-Hirn), enemy_common.c +4 (nicht gelistet), game_step_common.c +1 (nicht gelistet), re15_damage.c +8 (nicht
  gelistet; neue Zugriffsfunktion). Klein und offengelegt (Dossier 8.5) -> Hinweis H2 fuer die Integration, kein Mangel.
- Riegel parallel-/Linux-fest: eigene exe-Kopie `re15_pc_r39j_d_<id>` + eigener Laufordner, `cmake_minimum_required` im
  -P-Skript, `m` fuer Nicht-Windows, keine `.exe`-/Backslash-Pfade.

## 6. Maengel (nummeriert, nachpruefbar)
- **M1 (N4.3, mittel): Johns Schuesse holen die Aufmerksamkeit der Gorillas im echten Kampf nur selten.** Drei Laeufe mit
  lebenden Gorillas (L/L2/L3, je ~1500 Kampfbilder, 16 Salven): 6 Salven ohne Treffer, 4 mit Treffer ohne jede Zuwendung,
  2 nur Drehung, 4 mit Drehung + Heranruecken (Tabelle Abschnitt 3 N4.3). Ursachen im Code, je gemessen:
  (a) `schuss()` (dining_r39.c) wertet KEINEN Treffer, solange `re15_player_victim_state() != 0` - auch nicht auf den
  Gorilla, der Elza nicht haelt: 5 von 16 Salven (L F3771/F4142/F4513, L3 F3771/F4142; Ziel nachgerechnet im Kegel, Ziel
  nicht im Angriffs-Sub; state.txt gr=1). Gerade wenn Elza gepackt wird, bleibt Johns "Ablenkung" wirkungslos.
  (b) Die Richtung wird nur VOR den 110 Zielbildern bestimmt (Anker ZIEL); ein schneller Gorilla verlaesst den Kegel
  (L2 F4137: rel -599/-612).
  (c) Ein am Rand x -6982 festhaengender Gorilla wird trotz Aufmerksamkeit weder gedreht noch bewegt (L2 Gorilla 3:
  8 Treffer, r3157 -> r3163, Lage fest) und nach 29 Bildern zurueckgegeben; John ist nie im Bild.
  Nachpruefen: `J39abn1/lauf.sh l2 plan_l2.txt "5800#1141" "3400-5800/5:fd_" "RE15_GIVE=0x43:1,0x03:15,0x15:50"
  RE15_R38F_GORILLAS_TOT=2400` (und `l`/`l3` mit `plan_a.txt`/`plan_l3.txt`, EXIT 5500#1141) -> debug.log `John schiesst`,
  state.txt Slot 2/3 `r`/`@`/`gr=`, `python J39abn1/aufmerk.py l_l2/r39.txt`.
  Abhilfe-Richtung: Treffer auf einen Gorilla, der Elza nicht haelt, nicht an Elzas Opferzustand binden; Richtung
  unmittelbar vor dem Feuer nachfuehren; Zuwendung auch fuer einen klemmenden Gorilla sichtbar machen (Drehen zu John);
  am lebenden Kampf neu messen (Riegel mit lebenden Gorillas) und die PORT-WAHL dem Nutzer mit Messwerten vorlegen.

## 7. Hinweise (keine Maengel)
- H1 Klopf-Klang RE2 DOOR0F-Holzschlag (Wahl des Bau-Agenten, RE2 hat keinen Tuer-Klopfton) - Klangabnahme beim Nutzer.
- H2 Gemeinsame Dateien ausserhalb VERTRAG 1.3 (Abschnitt 5) - bei der Integration mit A/B (audio_pc.c), E
  (enemy_ai_common.c Gorilla-Hirn) und K zusammenfuehren. Spur K 18940e95 aendert `rbj_resolve_slot` (enemy_common.c
  Z. ~121), J haengt in `re15_actor_rbj_anim/_skel` (Z. ~139/~147) - verschiedene Stellen, Leihe geht am Cache vorbei.
- H3 Integration mit Spur E: E unterdrueckt das Zucken am HURT-Eintritt (`re15_r39_affen_zucken_unterdruecken`,
  enemy_ai_common.c im Integrationsbaum Z. 9428: unter 36 Energie kein Flinch). Johns `schuss()` setzt Zustand 2 mit
  +0x7 = 0 wie der Resolver -> nach der Integration zuckt ein Gorilla bei Johns Salve (2 x 12 = 24 < 36) nicht mehr; dann
  bleibt nur die Zuwendung als sichtbare Wirkung (M1) - nach der Integration mit 360 Energie und lebenden Gorillas messen.
  Ebenso die Szene einmal mit echtem Tod der Gorillas (nicht nur RE15_R38F_GORILLAS_TOT) fahren (Orchestrator-Hinweis
  "mit beiden Energien").
- H4 Runde-38-Riegel D/A und unit phase_h/abgang sind Platzhalter ("ersetzt"), wie in Abnahme 0.
- H5 Untersuchungstexte warten auf einen laufenden Hilferuf (Lauf R: msg 12 0,6 s spaeter).
- H6 Dossier 8.8 "Endstand nach Nachtrag 7 (751e3f4d, Bau 2026-10-18 ~22:45)": exe 21:47:36, Commit f744e5ad 22:13 - die
  Uhrzeit stimmt nicht (gemeint wohl 21:45).
- H7 test_r39_dining1141.cmake: Variable `_geste17` prueft jetzt mo=2 (Heranwinken) - nur der Name ist alt.
- H8 Spieler-Plc_motion(2,c) als W-Bank-Clip nur in ROOM1141 (Bau-Agent OFFEN: ROOM3060/3061 gleiche Form).

## 8. Urteil
| Punkt (Wortlaut AUFTRAG.md) | Urteil | Kurzbeleg |
|---|---|---|
| 1 keine Zombie-Geraeusche | erfuellt | A/P/L: 0x Bank 0x3C/0x3D/main42 im Abschnitt 1141; nur Klopfen/help/Glas/Rolladen/DOOR1A (+ Gorilla-Stimmen im Kampf) |
| 2 John durch die Tuer; Kamera Elza -> Tuer-zu-Ton -> Kamera Tuer mit John | erfuellt | P: Cut 7 F1708, Tuer auf ~F1781, John hinter der Kamera F1784, Tuer zu ~F1827, Cut 6 F1854 mit John (`p_tuer.png`) |
| 3 kein "shout" | erfuellt | 0x main42 / 0x3D |
| 4 help01..05 im Mix + mehrfaches Klopfen | erfuellt | A: 3,1,4,2,5,3, je 3 Schlaege Abstand 5, Abstand 220 Bilder, bis zum Entriegeln, hoerbar |
| 5 Elzas Reaktion beim ersten Hoeren (M1) | erfuellt | A/A0 msg 50 F176..F275 einmal; F/E/R: Reaktion erst nach dem Rolladen, nie in einer Szene, vor dem Entriegeln |
| 6 Glasbruch vor "What was that?!" | erfuellt | A: Glas F2538/F2543 (Takt 6206, Spitze 28672) nach Johns Satz, msg 6 F2564 |
| 7 John schaut ab msg 9 zu Elza | erfuellt | A/P: msg 9/10 |blick| <= 10 in 268 Bildern |
| 8 zwei Zeilen Johns mit passenden Animationen (M2/M3) | erfuellt | msg 51 Gesten 15/23, msg 52 Heranwinken RBJ ROOM1171 rec1 Clip 2 (96 Bilder), Blick 0, kein Sprung (0) |
| 9 keine Panik-Musik 1141 -> 1171 | erfuellt | pegel A/P: Pegel 0 / 559; Mitschnitt ohne MAIN16-Nachhall |
| N4.1 Rolltor oeffnen, danach normal laufen | erfuellt | A: frei ab F1215, Klopfen weiter; G2: Wiederbetreten |
| N4.2 Szene erst nach dem Entriegeln; rennen, andere Kamera, zur Tuer drehen, Waffe heben (Schusswaffe aus dem Inventar, sonst Messer) | erfuellt | A/P/Q/A2: msg 12, RUN, Cut 7, Drehung, Messer-DRAW / Pistole / ausgeruestete Schrotflinte |
| N4.3 John schiesst ab und an und erhaelt die Aufmerksamkeit der Gorillas | **teilweise** | L/L2/L3: 16 Salven, nur 6 mit sichtbarer Zuwendung; 6 ohne Treffer (5x Elza im Griff), 4 Treffer ohne Zuwendung (M1) |
| N4.4 / Nachtrag 7 Heranwinken wie in 1171 statt Waffe auf Elza | erfuellt | A: `John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen`, `a_wink_zoom.png` = Bewegung wie K (`hurry_up_head_vs_v0826.png`); Sub 2 Clip 6: 0 Bilder |
| N6 "trouble"/"gorilla" klein | erfuellt | msgtext + Bilder |

Gates: RE-Gate gehalten (8 Stichproben selbst disassembliert/gelesen, alle stimmen; keine Rate-Woerter; PORT-WAHL /
NUTZER-VORGABE gekennzeichnet). Fix erklaert Befund: M1 der Abnahme 0 (Pausemaske friert HAEMMERN ein -> Merker
(5,0x10)) durch Lauf F belegt (msg 50 F481 nach dem Rolladen statt F291), M2 durch Leih-Log + Bilder, M3 durch Schritt 0.
Vertrag gehalten (Pfade, Bit 94, Ids 50..58; Haken klein - H2). Tests: Unit 164/164, Integration 18/18 gruen.
**bestanden = false** (N4.3 nur teilweise: Mangel M1).
