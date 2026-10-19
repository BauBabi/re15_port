# Runde 39 — Spur J "dining1141": Abnahme 4 (unabhaengig, nach Nachbesserung 4)

Baum `.claude/worktrees/r39_dining1141`, Zweig `r39/dining1141`, Stand 4ed68276 (Merge-Basis mit master 2963579d; master =
c4675474). Geprueft: `git diff master...HEAD -- re15_port` (Zweig-eigene Aenderungen, 29 Dateien unter re15_port) und gesondert die
Nachbesserung 4 `git diff ece3a8f9..HEAD` (nur gen/dining_r39_programme.inc +-12, tools/r39_dining/programme_bauen.py +-22,
Riegel test_r39_dining1141.c / probes / test_r39_dining1141_zielen.cmake, Dossier, Belegbilder). Massstab: AUFTRAG.md (Punkte 1-9
"Mit Room 1141 ..." + "Wenn man ROOM 1141 danach verlaesst ..."; Nutzer-Nachtraege 4, 6, 7, 8, 10, 13), VERTRAG.md, Abnahme 3
(`J_abnahme_3.md`, Mangel M1-Rest, Hinweise H1-H8).
Messordner (nicht committet): scratchpad `J39abn4/` (Laeufe `l_<name>`, eigene exe-Kopie `re15_pc_j39abn4_<name>.exe` je Lauf
neben der Baum-exe, danach geloescht; Werkzeuge aus Abnahme 3 uebernommen und vorher gelesen: `lauf.sh` (Praefix j39abn4),
`auswert.py`, `cap.py`, `stimme_xkorr.py`, `salven.py`, `aufmerk.py`, `bogen.py`, `crop.py`).

## 0. Stand der Abnahme (fortlaufend)
- AUFTRAG (Baum + master-Fassung: dort zusaetzlich Nachtraege 12-14, fuer J nur 13 "1141 msg 41 bleibt einfacher Raumtext"),
  VERTRAG, Dossier Abschnitt 11 (Nachbesserung 4), Abnahme 3 gelesen; Code-Diff gelesen (NB4 vollstaendig; die gemeinsamen
  Dateien des Zweigs erneut: dining_r38.c, enemy_ai_common.c, enemy_common.c, game_step_common.c, player_common.c, re15_damage.c,
  scd_vm.c, main.c, audio_pc.c, dining_r38_pc.c, test_r36_kartenbits.c - unveraendert seit Abnahme 3).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (exe 08:24:04, gen/dining_r39_programme.inc 08:19:55 = aelter als die exe; Arbeitsbaum sauber = 4ed68276).
- Generator `python re15_port/tools/r39_dining/programme_bauen.py --pruefen` -> `dining_r39_programme.inc ok`; Listing ENTRIEGELN39
  +0x0408 `29 08` Cut_chg 8, +0x040A Work_set(2,0), +0x040E Pos_set John, +0x0416 `2e 01 00 00` Work_set(1,0), +0x041A Pos_set Elza,
  +0x0422 `34 04 00 08` Member_set(4,2048), **+0x0426 `40 00 09 00 82 e2 04 ac` Plc_dest TURN Elza -> John**, +0x042E Sleep 1,
  +0x0432 Plc_neck, +0x043C Sleep 10, +0x0440 Sleep 5 (1376 B).

## 1. Messlaeufe (echte exe, eigene Kopie je Lauf)
Gemeinsame Umgebung (`J39abn4/lauf.sh` = Abnahme 3, nur Praefix): RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_PSELECT_AUTO(_SWITCH)=1
RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_EVT_TRACE=1 RE15_BGM_CTL_DEBUG=1 RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw
RE15_R39J_LOG=r39.txt RE15_STIMME_LOG=1 RE15_R39_GEGNER_LOG=1 RE15_WEGPLAN=<plan> RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78
(Nach-1021-Flags wie der Riegel) RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0 RE15_FRAMEDUMP=<bereich>. Danach nur echte
Tueren (1131 Schluessel + Tuer -> 1141 -> Dachtuer -> 1171). Bildnummern = Bilder seit Raumeintritt (g_engine.frame_count).
Auswertung Schnitt: neues Werkzeug `J39abn4/schnitt.py` (state.txt pst/mo/ac/mg/bx/cam/pm je Bild + debug.log Messhaken/Cut_chg/
Plc_dest), gegen den Vorher-Lauf der Abnahme 3 (`J39abn3/l_rl2`, exe 05:15) geprueft: dort "Cut 8 ab F4086 ... davon mo 215: 8
[4086..4093]" = Befund M1-Rest.

| Lauf | Wegplan / Umgebung | Ergebnis |
|---|---|---|
| A | `plan_a.txt` (= Wegplan integration_r39_dining1141_d), RE15_GIVE=0x43:1 (nur Messer: `[messer] Startinventar Charakter 4: ... Waffe 1`), RE15_R38F_GORILLAS_TOT=600 + _STARR=1, EXIT 300#1171, FD 0-6200/5 | rv=0, 373 s, 1131 -> 1141 -> 1171 |
| P | wie A, RE15_GIVE=0x43:1,0x03:15,0x15:50 (Browning im Inventar, nicht ausgeruestet), FD 1500-6200/5 | rv=0, 373 s, 1131 -> 1141 -> 1171 |
| R2 | = Abnahme-3-Lauf R2: `plan_r.txt`, RE15_GIVE=0x43:1,0x04:99,0x15:60 RE15_EQUIP=4, RE15_R39_MESS_SPIELER_HP=100, RE15_INPUT_SCRIPT = `skript_r.txt` (R1 + Feuer) BASIS=spiel ab 3500, **echter Tod** beider Gorillas (ohne Messhaken), EXIT 6700#1141, FD 6240-6340/1 | rv=0, 351 s |
| RL2 | = Abnahme-3-Lauf RL2 (M1-Rest): `plan_r.txt`, RE15_GIVE=0x43:1,0x03:1,0x15:50 RE15_EQUIP=3 (Browning, 1 Patrone), RE15_R38F_GORILLAS_TOT=600 + _STARR=1, RE15_INPUT_SCRIPT=`M33.3333,MA0.1,M1.6667,MA0.1,M10` BASIS=spiel ab 3000 (Abzug mit leerem Magazin F4053 = 3 Bilder vor der Sperre), EXIT 4250#1141, FD 4040-4110/1 | rv=0, 291 s |
| RL3 | NEU: wie RL2, Skript `M33.3333,MA0.1,M1.7333,MA0.1,M10` = Abzug mit leerem Magazin F4055 (1 Bild vor der Sperre, Nachladen reicht bis in den Schnitt-Takt) | rv=0, 277 s |
| KH | NEU: nur Messer, Skript `M35.1,MA0.1,M10` ab 3000 = Hieb F4053 (3 Bilder vor der Sperre) | rv=0, 280 s |

### 1.1 Punkt 1 — keine Zombie-Geraeusche
- Lauf A, debug.log Abschnitt 1141 (`J39abn4/a_1141.log`, 2112 Zeilen), alle Skript-Toene (`SCD Se_on: bank=`): 18x bank 62 (=0x3E) id 0
  (Klopfen), id 49..53 (help01..05, id 51 = help03 zweimal), 2x id 33 (Glas), bank 59 id 10/11/12 (Rolladen), 2x2 bank 14 id 0/1
  (DOOR1A Auftritt/Abgang). **0 Zeilen** `bank=0x3C` / `bank=0x3D` / `bank=60 ` / `bank=61 `, 0x `main42` / `main50`. Census aller
  `[se]`-Zeilen: ausser diesen nur Stimmen ohne Skript-Ton, je am Ausloeser zugeordnet: se=6 F~864 (Ja/Nein-Wahl msg 13), se=3 F1485
  (Riegel), se=8 F1629 (Messer ziehen), se=1 ~F3740 (Johns Schuss, `Plc_motion(entity=2, motion=7)`), se=0/1 F4311.. (Tuerbank Abgang).
  Kein Zombie-Ton.
- **Urteil: erfuellt.**

### 1.2 Punkt 2 — John durch die Tuer; Kamera vorne auf Elza, Tuer-zu-Ton, dann Kamera auf die Tuer mit John
- Lauf A (Messer) und P (Browning) bildgleich (`auswert.py`): `[dining39] F1485 Dachtuer entriegelt`, msg 12 F1487..F1529, Elza rennt
  (`Plc_dest(slot=0 mode=0x05 dest=(-7800,-12800))`, Cut 5), `[scd F1593] Cut_chg(7)` (angezeigt ab F1594), `Plc_dest(slot=0 mode=0x09
  dest=(-7350,-1300))` (Drehung zur Dachtuer), W-Clip ab F1629/F1630; `Se_on bank=14 id=0` (Tuer auf) zwischen fd_001665 und
  fd_001670; John erstmals nicht geparkt **F1670 bei angezeigtem Cut 7** (-7350,-1300) = hinter der Kamera; `Se_on bank=14 id=1`
  (Tuer zu) zwischen fd_001710 und fd_001715; `[scd F1739] Cut_chg(6)`: erstes Bild Cut 6 F1740 mit John (-7350,-1300) rot 1024 vor der
  Tuer; F1760 Cut 5, John laeuft heraus. "Bilder mit John sichtbar vor dem Auftritt: 0".
- Bild `J39abn4/a_tuer.png` (F1490 Riegel "You've ..."; F1580 Cut 5 Elza rennt; F1595..F1735 Cut 7 Elza von vorn, Blick zur Tuer hinter der
  Kamera, Messer in der Hand; F1740/F1750 Cut 6 John vor der Dachtuer; F1760..F1865 Cut 5 John kommt zu Elza, msg 40).
- **Urteil: erfuellt.**

### 1.3 Punkt 3 — kein "shout"-Voiceover fuer John
- Lauf A/P: 0x `main42.wav` geladen, 0x bank 0x3D; Kreuzkorrelation main42.wav (liegt weiter in synchro/STAGE1/room1141) im Fenster Takt
  3600..5200: rho 0,159 (Haelften -0,167 / -0,020 = kein Treffer). Johns Stimmen in Phase H nur help01..05 (`[voice] clip loaded: help0N.wav`).
- **Urteil: erfuellt.**

### 1.4 Punkt 4 — help01..05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen
- Lauf A: je Runde drei Schlaege im Abstand 5 Bilder (F35/F40/F45, F255/F260/F265, F475/F480/F485, F884.., F1104.., F1324..), 30 Bilder
  danach ein Hilferuf: help03 F75, help01 F295, help04 F515, help02 F924, help05 F1144, help03 F1364 (PORT-WAHL Mix); nach dem Riegel F1485
  keiner mehr. (Wie Abnahme 3: waehrend der Rolladen-Szene verwirft das SE-Tor FUN_80045a18 die drei Schlaege F884..F894 -
  `GATE: ... VERWORFEN (laufend prio=6)`, H7.)
- Mitschnitt A (`cap.py l_a/cap.raw 3698 3720 1`): Schlaege Takt 3705/3710/3715 RMS 2480/2484/2502 (Spitze ~8000) gegen Musik RMS ~250;
  `stimme_xkorr.py`: help03.wav rho 0,908 bei Takt 3744 (= F75), help01.wav rho 0,895 bei Takt 3964 (= F295), Median |rho| 0,007/0,008.
- **Urteil: erfuellt** (Klang = RE2 DOOR0F-Holzschlag; Klangabnahme beim Nutzer, H1).

### 1.5 Punkt 5 — massgeblich Nutzer-Nachtrag 10 (in 1141 alles wie bisher, nur ohne Elzas Reaktion; der Satz zieht nach ROOM1041 = Spur P)
- Lauf A: erste Nachricht in 1141 = msg 13 (Gemaelde) F676; erste Pad-Sperre F867 (Rolladen); 0x `[msg] room=1141 id=50`, 0x `main50.wav`;
  Kreuzkorrelation main50.wav im Fenster Takt 3600..4400: rho 0,063 (nicht gespielt).
- **Urteil: erfuellt** (in der Fassung von Nachtrag 10).

### 1.6 Punkt 6 — Glasbruch vor "What was that?!"
- Lauf A: msg 5 (John, main05.wav) offen F2224..F2423; `Se_on bank=0x3E id=0x21 -> ... GLAS1090 Glasbruch` F2424 (Takt 6093) und F2429
  (Takt 6098); msg 6 `"Elza: What was that?!"` ab F2450. Mitschnitt: Takt 6088..6090 RMS 8/1 -> Takt 6092..6106 RMS 5676..10144 (Spitze 28673).
- **Urteil: erfuellt.**

### 1.7 Punkt 7 — John schaut ab "...some kind of gorilla..." zu Elza
- Lauf A/P (r39.txt `blick`): msg 9 169 Bilder |blick| max 10, msg 10 99 Bilder max 10; msg 11 147 Bilder, davon die letzten 12 > 160 = John
  laeuft in seine Ecke (Original sub02 @0x00F8E). Bild `J39abn4/a_blick.png` (F2820..F3235).
- **Urteil: erfuellt.**

### 1.8 Punkt 8 — nach dem Tod der Gorillas zwei Zeilen Johns "mit passenden Animationen" (+ Nachtraege 4/7/8; Mangel M1-Rest der Abnahme 3)
- **Ablauf (Lauf A/P, Messhaken F3833):** Balken + Pad-Sperre ab **F3834** (pm 2 bis F4347), `[scd F3863] Cut_chg(8)` und **im selben
  Takt** `[scd] Plc_dest(slot=0 mode=0x09 dest=(-7550,-21500) flag=0)` (debug.log Zeilen 1449/1450; Abnahme 3: 11 Bilder spaeter),
  angezeigt F3864. msg 51 `"John: Puuuh, that was close! I thought we end up as gorilla meal...."` F3886..F4060, msg 52 `"John: Elza,
  follow me, I think I might have found a way out of here!"` F4062..F4190; John laeuft F4192 los (0 Laufbilder mit offener msg 52);
  Kamera nach (9,67)=1: F4319 Cut 7, F4334 Cut 0, F5954 Cut 5, F6117 Cut 6 (folgt Elza). Alle Bildnummern ab msg 51 = Abnahme 3.
- Johns Sub/Clip (`auswert.py`): msg 51 (0,15) 100 + (0,23) 75 Bilder; msg 52 alle 129 Bilder (0,2) = Heranwinken, `[dining39] F4061 John
  winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)`; Blick msg 51/52 0; groesster Sprung Johns ohne Schnitt/Lauf 0.
  Bilder `J39abn4/a_gespraech.png` (F3830 Cut 0, F3865 Cut 8 Elza links / John rechts, F3900..F4150 Texte, F4195 Cut 7 John laeuft) und
  `a_wink_zoom.png` (F4065..F4200: F4095..F4140 John winkt mit erhobenem Arm, dreht dann zur Tuer; keine Waffe auf Elza).
- Neu sichtbar durch NB4 (Normalfall, kein Mangel): Elzas TURN beginnt jetzt im Schnittbild - `a_schnitt_zoom.png` F3865/F3870 macht sie
  den Dreh-Schritt (Clip 5, Bein angewinkelt), in Abnahme 3 (`J39abn3/l_a`, gleiche Bilder `a3_schnitt_zoom.png`) stand sie dort noch und
  machte ihn bei F3875/F3880. Form 11C0 sub03 @0x01B26 (RE-Gate Nr. 1); John und alle Zeilen unveraendert.
- Stimmen (Nachtrag 8, Mitschnitt A): main51.wav rho **1,000** bei Takt 7554 (= F3885), main52.wav rho **1,000** bei Takt 7730 (= F4061);
  Gegenprobe main52 im Fenster von msg 51: rho 0,047. msg 12/13: `[voice] ROOM1141 msg 12/13: keine Aufnahme ... - Zeile bleibt stumm`.
- **M1 / M1-Rest:** Normalfall R2 (echter Kampf, Rueckstoss + Senken unter der Sperre) 0 Bilder Cut 8 mit mo 215; Randfaelle RL2 (Nachladen 3
  Bilder vor der Sperre: vorher 8, jetzt 0), RL3 (Nachladen 1 Bild vorher, bis in den Schnitt-Takt: 0, Magazin leer, Vorrat 50 -> 50) und
  KH (Messer-Hieb 3 Bilder vorher: 0). Sichtbar bleibt in den Randfaellen nur die Ueberblendung der ersten 2-3 Bilder (Abschnitt 1.8a):
  F4087/F4088 Arm schraeg nach unten, Pistole etwa auf Johns Guertelhoehe, ab F4090 unten - nach dem Original-Mechanismus (TURN-Init +0x8f =
  7 @0x800313b8, FUN_8001f3bc @0x8001f408..@0x8001f4b0; dieselbe Form 11C0 sub03).
- **Urteil Punkt 8: erfuellt** (Zeilen, Gesten, Heranwinken wie 1171, Stimmen, Kamera; M1-Rest behoben: im Schnittbild Kommando 4, kein
  Waffen-Clip in Cut 8 in allen vier gemessenen Faellen; Rest = Original-Ueberblendung, Hinweis H9).

### 1.8a M1-Rest (Punkt 8) - Lauf RL2 nach der Nachbesserung 4
- debug.log: `[dining] F4056 MESSHAKEN ...`, `[scd F4086] Cut_chg(8)` und im selben Takt `[scd] Plc_dest(slot=0 mode=0x09
  dest=(-7550,-21500) flag=0) walk_active=1` (vorher stand diese Zeile erst bei F4097).
- state.txt (`schnitt.py l_rl2 4050 4130`): F4053..F4083 `mo=215 ac=13 mg=0 bx=50` (Nachladen, pad 8800 F4053..F4055), F4056 `pm=2`
  (Sperre, Nachladen laeuft weiter), F4084 `ac=8 mg=15 bx=35` (Auffuellen am Clip-Ende), F4085 `ac=6` (SENKEN), **F4086 `pst=4 mo=105
  cam=8`** (Schnittbild = Kommando 4, TURN-Clip), F4087 `pst=4 mo=105`, F4088..F4103 `mo=1` (Sub 6), ab F4104 `mo=2`.
  **"Cut 8 ab F4086: 40 Bilder, davon mo 215: 0"** (vorher 8: F4086..F4093). Elza steht (-5800,-21500) rot 2047 zu John.
- Bilder `J39abn4/rl2_bogen.png` (F4084..F4095 ganz) und `rl2_cut8_zoom.png` (F4086..F4093, x110..250 y100..225, 3x): F4086 zeigt
  noch Cut 5 (leerer Gang - Elza ist im Schnitt-Takt schon versetzt; die Kamera wird ein Bild spaeter sichtbar, Port-weit wie Cut 7
  in 1.2). **F4087/F4088: Elzas rechter Arm schraeg nach unten, die Pistole etwa auf Hoehe von Johns Guertel** (vorher, Abnahme 3
  `rl2_cut8_zoom.png`: beide Arme waagerecht, Pistole auf Johns Oberkoerper), F4089 Hand an der Huefte, ab F4090 haengt der Arm.
  Das ist die Ueberblendung FUN_8001f3bc aus der zuletzt gezeigten Lage (TURN-Init +0x8f = 7 @0x800313b8, RE-Gate Nr. 5/7); die
  Original-Form 11C0 sub03 versetzt Leon im Schnitt-Takt ebenso (Member_set 0/2 @0x01B1E/@0x01B22) und gibt ihm im selben Takt den
  Plc-Befehl - dieselbe Ueberblendung. Kein Waffen-Clip mehr im Schnittbild.
- **Lauf RL3 (NEU, Nachladen bis in den Schnitt-Takt):** F4055 `pad=8800 ac=13 mg=0 bx=50` (Nachladen beginnt 1 Bild vor der Sperre),
  F4056 `pm=2`, Nachladen F4055..F4085, `[scd F4086] Cut_chg(8)` + `Plc_dest(slot=0 mode=0x09 dest=(-7550,-21500))` im selben Takt,
  **F4086 `pst=4 mo=105 mg=0 bx=50`** (das Auffuellen haette im Bild F4086 stattgefunden - abgebrochen, Magazin leer, **Vorrat 0x15
  unveraendert 50**, keine Patrone verloren), F4088.. `mo=1`, F4104.. `mo=2`; **Cut 8: 40 Bilder, davon mo 215: 0**. Bild
  `J39abn4/rl3_cut8_zoom.png` (F4084..F4100): F4087/F4088 Arm aus der Nachlade-Lage schraeg vor Johns Bauch, F4089 an der Huefte, ab
  F4090 unten (Ueberblendung wie RL2).
- **Lauf KH (NEU, Messer-Hieb 3 Bilder vor der Sperre):** F4053..F4077 `ac=7` (Hieb, 25 Bilder), F4056 `pm=2`, F4078..F4085 `ac=6`
  (SENKEN, 10 Bilder haetten bis F4087 gereicht), **F4086 `pst=4 mo=105`**, **Cut 8: 40 Bilder, davon mo 215: 0** (Unit-Gegenprobe
  `schnitt` mit der Programm-Fassung NB3: 1 Bild mit erhobener Waffe).
- **Normalfall R2 (echter Kampf, Abnahme-3-Nachstellung):** `affen init ... hp=360` (Spur-E-Energie), `affen treffer F5624 slot=2 hp=0`,
  `F6170 slot=3 hp=0`; John 14 Schuesse / 14 Treffer (`[dining39] F3874 ... F6132 John schiesst ... Treffer, Aufmerksamkeit`). Schuss
  F6263 (mg 43 -> 42), Sperre F6266, Rueckstoss bis F6270, SENKEN F6271..F6279, neutral F6280..F6295 (`mo=200`), **F6296 Cut_chg(8) +
  Plc_dest TURN im selben Takt, `pst=4 mo=105`**, ab F6298 `mo=1`; Cut 8 mit mo 215: 0. Bild `J39abn4/r2_bogen.png` (F6266..F6320):
  ab F6297 stehen Elza und John nebeneinander, beide Arme unten - wie in Abnahme 3, nur steht Elza jetzt ab dem Schnittbild im TURN/Sub 6
  statt 11 Bilder neutral im Kommando 1.

### 1.9 Punkt 9 — keine Panik-Musik beim Wechsel 1141 -> 1171
- Werkzeug des Riegels: `test_r39_dining1141 pegel l_a` -> `ok: ROOM1171 main00 Sce_bgm_control op=1 bei Takt 9983`, `Takte 10013..10038
  Pegel 0, Takte 10053..10083 Pegel 559`, `pegel: 0 Fehler`.
- Eigene Pegel (`cap.py l_a/cap.raw 9930 10110 6`): Stille (RMS 0,7) bis Takt 9972, Tuer-Schliesston 9978..10002 (RMS 1689 -> 86), Stille
  10008..10032, ab 10038 steigt MAIN32 (16 -> ~1200). 1141: MAIN16 per `Sce_bgm_control op=2` bei capTick 7502 gestoppt (nach dem Kampf);
  1171: `[bgm] stage=0 room=17 entry=5572 -> MAIN32(flag 1)`, `Sce_bgm_control slot=0 op=1 ... capTick=9983`. Kein MAIN16 im Uebergang.
- Mechanismus unveraendert seit Abnahme 0 (audio_pc.c `if (!s_bgm_pending)` vor dem Lautstaerke-Switch; FUN_80044210 @0x8004429c-@0x800442b0
  wartet auf das Ende der Ausblendung, Raumlader @0x800399b0 vor @0x80039a00 - in Abnahme 3 nachdisassembliert).
- **Urteil: erfuellt.**

### 1.10 Nachtraege 4, 6, 7, 8, 10 (Wiederholung am Endstand)
- N4.1 Lauf A: `[dining] F674 ... Gemaelde (Ereignis 6) in Phase H -> Programm ROLLTOR39`, Rolladen-Ton 0x3B F866/F881/F1076, Pad-Sperre
  F867..F1100, danach frei F1101..F1484 (Elza (-8134,-10294) rot 2080 -> F1400 (-7427,-5555) -> F1484 (-7266,-1060), Klopfen und Rufe
  laufen weiter F1104.., F1324..), Riegel F1485. **erfuellt.**
- N4.2 Lauf A: `F1485 Dachtuer entriegelt: Elza hebt Waffe 1 (vorher 1, Messer)`, W-Clip 13/6 ab F1630 bei Cut 7, John noch geparkt
  (`a_heben_zoom.png`: Messer in der rechten Hand, Abwehrhaltung zur Tuer); Lauf P: `Elza hebt Waffe 3 (vorher 1, Schusswaffe)`, W-Clip 6
  F1630..F1805 (`p_heben_zoom.png`: F1645..F1735 Browning beidhaendig auf die Tuer hinter der Kamera). **erfuellt.**
- N4.3 (John schiesst ab und an, Aufmerksamkeit), Lauf R2 mit lebenden Gorillas: 14 Schuesse / 14 Treffer im Abstand ~370 Bilder (zwei je
  Salve), Peilung rel -12..0 (im Kegel), 4 davon bei gr=1 (Elza im Griff, Ziel = der andere Gorilla) (`salven.py`); 7 Aufmerksamkeits-
  Episoden, z. B. Gorilla B F4989..F5197 Abstand zu John 12188 -> 2929 (`aufmerk.py`). Dazu Riegel `integration_r39_dining1141_kampf`
  (Abschnitt 3). **erfuellt.**
- N4.4 / Nachtrag 7: Heranwinken RBJ ROOM1171 rec1 Clip 2 (96 Bilder), 0 Bilder Waffe auf Elza (1.8). **erfuellt.**
- N6: `"... we end up as gorilla meal...."`, `"He is in trouble"` nur im Riegel `texte` (msg 50 kommt in 1141 nicht vor). **erfuellt.**
- N8: main51/main52 an msg 51/52 (rho 1,000). **erfuellt.** N10: keine Reaktion in 1141 (1.5). **erfuellt.**

## 2. RE-Gate (Nachbesserung 4): Stichproben selbst disassembliert/gelesen (richtige Binaerdatei, Sprungziele)
`S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (info/Re1.5/PSX.EXE); ROOM11C0.RDT / ROOM1141.RDT: shared_assets ==
info/Re1.5 (`cmp` bytegleich), SCD per `re15_port/tools/scd_dump_room.py`.

| # | Zitat (Generator/Dossier 11.2) | selbst gelesen | Befund |
|---|---|---|---|
| 1 | Form ROOM11C0 sub03: Cut_chg @0x01B18, Work_set(1,0) @0x01B1A, Member_set @0x01B1E/@0x01B22, Plc_dest @0x01B26 im selben Takt | `xxd -s 0x1B02`: `.. 09 0a 1e 00 29 04 2e 01 00 00 34 00 9d e0 34 02 ff c1 40 00 04 00 38 c8 a1 de 18 05 40 00 09 00 ..`; `scd_dump_room.py`: "=== sub03 [file 0x1B02..0x1C0C]", 0x01B14 Sleep, 0x01B18 Cut_chg, 0x01B1A Work_set kind=1 idx=0, 0x01B1D Nop, 0x01B1E/0x01B22 Member_set, **0x01B26 Plc_dest `40 00 04 00 38 c8 a1 de`**, 0x01B2E Gosub | stimmt: kein Sleep zwischen Schnitt und dem Plc-Befehl des Spielers |
| 2 | Opcode-Tabelle 0x800744a8[0x40] = 0x80041be4; Work_set Art 1 = 0x800aca54 @0x80040da4..@0x80040db0 | `table 0x800744a8 0x48`: [46] 0x80040d2c, [63] 0x80041b90, **[64] 0x800745a8 -> 0x80041be4**; `dis 0x80040d2c`: `80040d4c lbu v1,1(v0)` (Art), `80040d70 ori v0,zero,0x1` / `80040d74 beq v1,v0,0x80040da4`, `80040da4 lui v0,0x800b`, `80040da8 addiu v0,v0,-13740` (= 0x800aca54), `80040db0 sw v0,340(a0)` (Faden+0x154) | stimmt |
| 3 | Plc_dest schreibt das Kommandowort sofort: +4 = 4 @0x80041c14, +5 @0x80041c18, +6/+7 = 0 @0x80041c1c/@0x80041c20 | `dis 0x80041be4`: `80041be4 lw a1,340(a0)`, `80041bf0 lbu a2,2(v1)` (Modus), `80041bf8 andi v0,v0,0x4` / `80041bfc beq v0,zero,0x80041c14` / `80041c00 ori v0,zero,0x4` (Verzoegerungsplatz), Waechter `80041c04 lbu v0,5(a1)` / `80041c0c beq v0,a2,0x80041c24`; **`80041c14 sb v0,4(a1)`, `80041c18 sb a2,5(a1)`, `80041c1c sb zero,6(a1)`, `80041c20 sb zero,7(a1)`** | stimmt (Waechter greift nur bei +0x1c4 Bit 2 UND gleichem Modus) |
| 4 | SCD-Laeufer vor dem Spieler: @0x8001cdec jal 0x8003f038 vor @0x8001ce0c jal 0x80031c44 | `dis 0x8001cde0`: `8001cde8 sw v0,-13760(at)` (g_pauseflags), **`8001cdec jal 0x8003f038`**, `8001cdf4 jal 0x8004f090`, `8001cdfc jal 0x8001500c`, `8001ce04 jal 0x8001a50c`, **`8001ce0c jal 0x80031c44`** | stimmt |
| 5 | Kommando 4 = 0x80073f90[4] = 0x80030660 -> 0x80073e30[9] = TURN 0x80031360; Init @0x80031398/@0x800313a4/@0x800313b8 (+0x8f = 7) | `dis 0x80031c44`: `80031c78 bltz a0` (nur Bit 31), `80031c8c lbu v1,-13736(v1)` (0x800aca58), `80031ca4 addiu at,at,16272` (0x80073f90), `80031cb4 jalr v0`; `table 0x80073f90`: **[4] 0x80030660**; `dis 0x80030660`: `8003068c lbu v0,-13735(v0)` (0x800aca59), `8003069c addiu at,at,15920` (0x80073e30), `800306ac jalr v0`; `table 0x80073e30`: **[9] 0x80031360**; `dis 0x80031360`: `80031374 lbu v1,0(a0)` (0x800aca5a), `80031398 sb v0(=1),0(a0)`, `800313a4 sb v0(=5),-13592(at)` (0x800acae8), `800313b0 sb zero,-13591(at)`, **`800313b8 sb v0(=7),-13597(at)` (0x800acae3 = 0x800aca54 + 0x8f)**, `800313d0 jal 0x8001ab9c` / `800313d4 ori a2,zero,0x60` | stimmt |
| 6 | Ziel-FSM 0x80032e9c liest 0x800aca5a, Tabelle 0x800740f4 [3] 0x80033c74 / [4] 0x80033d7c; Auffuellen nur am Clip-Ende @0x80033ebc/@0x80033ec0 | `dis 0x80032e9c`: `80032ea0 lbu v0,-13734(v0)` (0x800aca5a), `80032eb4 addiu at,at,16628` (0x800740f4); `table 0x800740f4`: [3] 0x80033c74, **[4] 0x80033d7c**; `dis 0x80033e9c`: `80033ea8 jal 0x8001f314` (anim_set) / `80033eb0 beq v0,zero,0x80033ed8` (Clip nicht zu Ende -> nichts), `80033ebc sh v0(=1),-13734(at)`, **`80033ec0 jal 0x8004ebdc`**, `80033ed0 jal 0x80045024` | stimmt: ein bei +6 = 0 abgebrochenes Nachladen fuellt nicht auf |
| 7 | Ueberblendung FUN_8001f3bc (+0x8f) @0x8001f408 / @0x8001f474 / @0x8001f4a8 / @0x8001f4b0 | `dis 0x8001f3bc`: `8001f408 lbu s0,143(v1)` (+0x8f der laufenden Figur), `8001f41c bne a3,zero,0x8001f45c` (Ueberblend-Zweig), alte Lage aus `+56/+58/+60` des Zeigers +0x188 (`8001f484`/`8001f490`/`8001f49c`), **`8001f474 mult v0,a3`**, **`8001f4a8 ori v0,zero,0x1000`**, **`8001f4b0 subu v0,v0,v1`** | stimmt: Gewicht des neuen Clips = 0x1000 - Schrittweite x Restzahl |
| 8 | Vorbild der Port-Zeilen ROOM1141 sub02 @0x00C24..@0x00C76 | `scd_dump_room.py ROOM1141.RDT`: 0x00C24 Cut_chg 7, 0x00C26 Set(2,7), 0x00C2A Set(1,0x1B), 0x00C2E Work_set 1/0, 0x00C32 Pos_set, 0x00C3A Member_set(4,0), 0x00C3E Sleep 1, 0x00C42 Plc_neck, 0x00C4C Sleep 10, **0x00C50 Plc_dest `40 00 09 00 a2 e5 da cb`**, 0x00C58 Sleep 5, 0x00C5C Work_set 2/0, 0x00C6E Plc_dest `40 00 09 01 ..`, 0x00C76 Gosub | stimmt (der TURN ist derselbe Befehl, nur im Schnitt-Takt) |
| 9 | Cut_chg 0x800402a0 und Pos_set 0x80041048 setzen +0x8f nicht zurueck (Ueberblendung ueber den Schnitt) | `table 0x800744a8`: [41] 0x800402a0 (0x29), [50] 0x80041048 (0x32); `dis 0x800402a0`: `800402d4 ori v0,v0,0x100` / `800402d8 sw v0,0(v1)` (0x800aca3c), Kamera-Bytes 0x800b0fe4/0x800b3f7b/0x800b5457, `80040300 jal 0x800142f4`, `8004030c ori v0,zero,0x1` (weiter im selben Takt); `dis 0x80041048`: nur `sw v0,52/56/60(a1)` (+0x34/+0x38/+0x3C), `80041074 ori v0,zero,0x1` | stimmt (kein Schreiber auf +0x8f; beide Handler geben 1 zurueck = kein Takt-Ende zwischen Cut_chg und Plc_dest) |

Suche in allen `+`-Zeilen von `git diff master...HEAD -- re15_port` nach `deferred|tunable|interim|for now|faithful|plausib|TODO|
FIXME|vorerst|geschaetzt|approx|ungefaehr|platzhalter|placeholder|sieht richtig|nach gefuehl|sieht besser`: **0 Treffer**. Neue getenv
im ganzen Zweig: nur `RE15_R39J_LOG` (Messschiene) und `RE15_SE_DEBUG` (Log) - kein Env-Schalter als Abschluss. Neue Konstanten der
Nachbesserung 4: keine (nur die Lage einer vorhandenen Original-Zeile im Programm; Pruefzahlen im Riegel = Messfaelle).

## 4. Vertrag (VERTRAG.md Runde 39)
- Pfade (`git diff master...HEAD --name-only`): `analysis/befunde_runde39/{AUFTRAG.md (nur Orchestrator-Commits + Merges), J_abnahme_0..4.md,
  J_belege/*.png, J_dining1141.md}` und `re15_port/{engine/src, engine/src/gen, include, platform/pc, platform/pc/src, tests/integration,
  tests/unit, tests/unit/probes, tools/r39_dining}`. `grep -E "release/|platform/android|shared_assets/PSX|CMakeLists|synchro/"` auf die
  Liste: **leer** (0). Nachbesserung 4 (`git diff ece3a8f9..HEAD --name-only`): nur gen/dining_r39_programme.inc, tools/r39_dining/
  programme_bauen.py, tests/unit/test_r39_dining1141.c, tests/unit/probes/r39_dining1141.cmake (Sonden-Datei je Thema, keine Test-
  CMakeLists), tests/integration/test_r39_dining1141_zielen.cmake, Dossier, J_belege/nb4_* - **keine gemeinsame Datei**.
- Ressourcen unveraendert seit Abnahme 3: Bank 9 Bit 94 (k_port[] +1 Zeile, seit Nachtrag 10 nur reserviert), Bit 95 ungenutzt;
  Nachrichten-Ids 50..58 (in 50..59); Port-Bank 0x3E (in master sonst nicht belegt: `git grep` auf master nur 0x3A/0x3B/0x3C/0x3D);
  Evt_exec sub 30 (ROOM1141 hat sub00..sub06). NB4: keine neue Ressource.
- Gemeinsame Dateien des Zweigs (unveraendert seit Abnahme 3, je Haken "Runde 39 Spur J", auf ROOM1141 begrenzt): scd_vm.c,
  platform/pc/main.c, dining_r38.c, dining_r38_pc.c, test_r36_kartenbits.c (zugeteilt); ausserhalb der Zuteilung 1.3 (offengelegt,
  Dossier 10.7/11.7): audio_pc.c (+5), enemy_ai_common.c (+7/-2), enemy_common.c (+4), game_step_common.c (+2), re15_damage.c (+8),
  player_common.c (+10/-1) - klein, gekennzeichnet (H2).
- Riegel je Punkt vorhanden und messend (Abschnitt 3); der neue Riegel `integration_r39_dining1141_zielen_nachladen` ist parallel-/
  Linux-fest: eigene exe-Kopie `re15_pc_r39j_zn_<id>` (Glob des Geschwister-Riegels `re15_pc_r39j_z_*` trifft sie nicht - "zn_" beginnt
  nicht mit "z_"), eigener Laufordner `r39_dining1141_zielen_nachladen_wd`, Umgebung des Aufrufers geleert (r39j_umgebung_leeren),
  `cmake_minimum_required(VERSION 3.16)`, Endung per `get_filename_component(... EXT)`, cap.raw danach geloescht.
- **Vertrag gehalten.**

## 6. Hinweise (keine Maengel)
- H1 Klopf-Klang = RE2 DOOR0F-Holzschlag (RE2 hat keinen Tuer-Klopfton) - Klangabnahme beim Nutzer (wie Abnahmen 2/3).
- H2 Gemeinsame Dateien ausserhalb VERTRAG 1.3 (Abschnitt 4: audio_pc.c, enemy_ai_common.c, enemy_common.c, game_step_common.c,
  re15_damage.c, player_common.c) - bei der Integration mit A/B (audio_pc.c), D/E (enemy_ai_common.c), K (enemy_common.c,
  player_common.c) zusammen ansehen. Nachbesserung 4 aendert keine davon.
- H3 Die E-Riegel `unit_r39_gegner_1141_ablauf1141` / `integration_r39_gegner_1141` tragen im Zweig den J-Ablauf - bei der Integration
  die Fassung dieses Zweigs nehmen (unveraendert).
- H4 Geltungsbereich des Senkens unter der Pad-Sperre: nur ROOM1141 Phase H; das Original senkt ueberall (z. B. 11C0 sub03). Offengelegt
  (Dossier 10.7/11.7), eigener Punkt fuer eine spaetere Runde.
- H5 re15_dining_r39_tick laeuft vor, aber ausserhalb des KI-Tors (Abnahme 2 H5, unveraendert).
- H6 SE-Tor: waehrend der Rolladen-Szene verwirft FUN_80045a18 drei Schlaege (F884..F894) - Prioritaetsregel, danach klopft es weiter.
- H7 Riegel `integration_r39_dining1141_zielen` laeuft ohne RE15_AUDIO_CAP_SYNC (Szenenende nach der Wanduhr); der neue Riegel
  `..._zielen_nachladen` braucht das bildgenaue Szenenende (CAP_SYNC, Messhaken F4056) und meldet bei einer Verschiebung um mehr als
  etwa -2/+8 Bilder "Vorbedingung" statt still zu bestehen (Dossier 11.7) - gewollt, nicht still.
- H8 (neu, Port-weit, nicht Spur J) Der Kamerawechsel wird ein Bild nach Cut_chg sichtbar, ein Pos_set im Schnitt-Takt aber sofort: im
  Schnittbild (RL2 F4086, R2 F6296) zeigt die alte Kamera den Raum schon ohne Elza. Bestand schon vor NB4 (Abnahme 3: "F4086 zeigt noch
  Cut 5"), gleiche Verzoegerung bei Cut 7 in Punkt 2. Wenn, dann ein eigener Port-Punkt (Kamera-Takt gegen das Original pruefen).
- H9 (neu) Randfaelle Nachladen/Hieb kurz vor der Sperre: in den ersten 2 sichtbaren Bildern von Cut 8 gleitet Elzas Waffenarm aus der
  letzten Lage nach unten (RL2/RL3 F4087/F4088: Pistole etwa auf Johns Guertelhoehe, ab F4090 unten). Kein Waffen-Clip, sondern die
  Ueberblendung des Originals (TURN-Init +0x8f = 7 @0x800313b8, FUN_8001f3bc @0x8001f408..@0x8001f4b0; Cut_chg/Pos_set setzen +0x8f
  nicht zurueck, RE-Gate Nr. 9). Sollte der Nutzer auch diese 2 Bilder nicht sehen wollen, waere das eine NUTZER-VORGABE ueber das
  Original hinaus.
- H10 (neu) Normalfall: Elzas Dreh-Schritt (TURN Clip 5 -> Sub 6) kommt jetzt im Schnittbild statt 11 Bilder spaeter (Lauf A F3865/F3870
  gegen Abnahme 3 F3875/F3880) - Form 11C0 sub03 @0x01B26; Johns Ablauf und alle Zeilen bildgleich (msg 51 F3886, msg 52 F4062).
- H11 Zusammenspiel mit Spur P (Nachtraege 10/12: Johns Ruf ab 1041, Panikmusik bis zur Rettung, Tuersaetze): nicht in diesem Zweig;
  Punkt 9 (keine MAIN16-Reste beim Wechsel 1141 -> 1171) haengt am audio_pc.c-Haken und gilt unabhaengig davon, welche Musik vorher lief -
  bei der Integration von P den Wechsel 1141 -> 1171 mit P's Panikmusik einmal nachmessen (Werkzeug `test_r39_dining1141 pegel`).
