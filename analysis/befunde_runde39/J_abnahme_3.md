# Runde 39 — Spur J "dining1141": Abnahme 3 (unabhaengig, nach Nachbesserung 3)

Baum `.claude/worktrees/r39_dining1141`, Zweig `r39/dining1141`, Stand e9f23f50 (Merge-Basis mit master 2963579d; master =
0a5ca4f2, danach nur Doku UEBERGABE.md / orchestrator/args_j_nach3.json). Gepruefter Code: `git diff master...HEAD -- re15_port`
(35 Dateien gesamt) und gesondert die Nachbesserung 3 `git diff 0e486aa6..HEAD` (dining_r39.c +37, gen/dining_r39_programme.inc
+-12, player_common.c +10/-1, re15_dining_r39.h +6, programme_bauen.py, Riegel). Massstab: AUFTRAG.md (Punkte 1-9 "Mit Room 1141
..." + "Wenn man ROOM 1141 danach verlaesst ..."; Nutzer-Nachtraege 4, 6, 7, 8, 10), VERTRAG.md, Abnahme 2 (`J_abnahme_2.md`,
Mangel M1, Hinweise H1-H7).
Messordner (nicht committet): scratchpad `J39abn3/` (Laeufe `l_<name>`, eigene exe-Kopie `re15_pc_j39abn3_<name>.exe` je Lauf
neben der Baum-exe, danach geloescht; Werkzeuge aus Abnahme 2 uebernommen und vorher gelesen: `lauf.sh`, `auswert.py`, `cap.py`,
`stimme_xkorr.py`, `salven.py`, `aufmerk.py`, `bogen.py`, `crop.py`).

## 0. Stand der Abnahme
- AUFTRAG/VERTRAG/Dossier (Abschnitte 0-10)/Abnahme 2 gelesen; Code-Diff gelesen (Nachbesserung 3 vollstaendig, die gemeinsamen
  Dateien des Zweigs erneut: enemy_ai_common.c, enemy_common.c, game_step_common.c, re15_damage.c, scd_vm.c, main.c, audio_pc.c,
  dining_r38_pc.c, test_r36_kartenbits.c, player_common.c).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (exe 05:15, Arbeitsbaum sauber = e9f23f50).
- Generator `python re15_port/tools/r39_dining/programme_bauen.py --pruefen` -> `dining_r39_programme.inc ok`.
- Unit der Spur `ctest -R "^unit_r39_dining1141"` -> `100% tests passed, 0 tests failed out of 6`; `test_r39_dining1141 senken`
  direkt: Pistole im Halten "SENKEN ab Bild 1, neutral ab Bild 10 (motion 200)", Messer im Halten dito, Pistole im Rueckstoss
  "SENKEN ab Bild 7, neutral ab Bild 16", Gegenprobe ohne Phase H "Waffe bleibt oben", `senken: 0 Fehler`.
- (fortlaufend: Laeufe A, P, R2, K ausgewertet; RL, Unit- und Integrationslauf folgen)

## 1. Messlaeufe (echte exe, eigene Kopie je Lauf)
Gemeinsame Umgebung (`J39abn3/lauf.sh` = Abnahme 2): RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_PSELECT_AUTO(_SWITCH)=1 RE15_MSG_LOG=1
RE15_SE_DEBUG=1 RE15_EVT_TRACE=1 RE15_BGM_CTL_DEBUG=1 RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw RE15_R39J_LOG=r39.txt
RE15_STIMME_LOG=1 RE15_R39_GEGNER_LOG=1 RE15_WEGPLAN=<plan> RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78 (Nach-1021-Flags wie
der Riegel) RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0 RE15_FRAMEDUMP=<bereich>. Danach nur echte Tueren (1131 Schluessel
+ Tuer -> 1141 -> Dachtuer -> 1171). Bildnummern = Bilder seit Raumeintritt. Takt-Bezug 1141 Lauf A: Takt = F + 3667 (`F35 tick=3702`).

| Lauf | Wegplan / Umgebung | Ergebnis |
|---|---|---|
| A | `plan_a.txt` (= Wegplan integration_r39_dining1141_d), RE15_GIVE=0x43:1 (Messer), RE15_R38F_GORILLAS_TOT=600 + _STARR=1, EXIT 300#1171, FD 0-6200/5 | rv=0, 335 s, 1131 -> 1141 -> 1171 |
| P | wie A, RE15_GIVE=0x43:1,0x03:15,0x15:50 (Browning im Inventar, nicht ausgeruestet), FD 1500-6200/5 | rv=0, 335 s, 1131 -> 1141 -> 1171 |
| R2 | = Abnahme-2-Lauf R2 (M1-Nachstellung): `plan_r.txt`, RE15_GIVE=0x43:1,0x04:99,0x15:60 RE15_EQUIP=4 (P228), RE15_R39_MESS_SPIELER_HP=100, RE15_INPUT_SCRIPT = Schuetze `skript_r.txt` (R1 halten + Feuer) ab Bild 3500, **echter Tod** beider Gorillas (ohne RE15_R38F_GORILLAS_TOT), EXIT 6700#1141, FD 6240-6340/1 | rv=0, 338 s |
| K | `plan_r.txt`, nur Messer (`[messer] Startinventar Charakter 4: ... Waffe 1`), RE15_R38F_GORILLAS_TOT=600 + _STARR=1, RE15_INPUT_SCRIPT=M45 ab Bild 3000 (R1 ueber Kampfende und Schnitt gehalten), EXIT 4400#1141, FD 3900-4300/2 | rv=0, 261 s |
| RL | wie K, aber Browning mit 1 Patrone (RE15_GIVE=0x43:1,0x03:1,0x15:50 RE15_EQUIP=3), Skript `M33.3333,MA0.1,M1.5,MA0.1,M10` ab 3000 (Schuss ~F4000, Nachladen ab ~F4048, kurz vor der Sperre = Randfall des Bau-Agenten), FD 4030-4120/1 | siehe 1.8 |

### 1.1 Punkt 1 — keine Zombie-Geraeusche
- Lauf A, debug.log Abschnitt 1141 (2112 Zeilen), alle Skript-Toene (`SCD Se_on: bank=`): 18x bank 62 (=0x3E) id 0 (Klopfen),
  id 49..53 (help01..05, help03 zweimal), 2x id 33 (Glas), bank 59 id 10/11/12 (Rolladen), 2x2 bank 14 id 0/1 (DOOR1A).
  **0 Zeilen** `bank=0x3C` / `bank=0x3D` / `bank=60 ` / `bank=61 ` / `main42` / `main50`; keine Ton-Zeile ausserhalb des
  Skript-Pfads (Census aller `[se]`-Zeilen). 1131 davor: 0 Skript-Toene.
- Nebenbefund (kein Mangel): 3 der 18 Schlaege (F884/F889/F894, waehrend die Rolladen-Szene laeuft) verwirft das SE-Tor
  (`[se] GATE: se=0 Stimme=6 prio=4(nib 4) VERWORFEN (laufend prio=6) [FUN_80045a18]`) - der Rolladen-Motor belegt die Stimme
  mit hoeherer Prioritaet; danach klopft es wieder hoerbar (F1104..).
- **Urteil: erfuellt.**

### 1.2 Punkt 2 — John durch die Tuer; Kamera vorne auf Elza, Tuer-zu-Ton, dann Kamera auf die Tuer mit John
- Lauf A (Messer) und P (Browning) bildgleich: `[dining39] F1485 Dachtuer entriegelt`, msg 12 F1487..F1529, Elza rennt (Cut 6 -> 5),
  `[scd F1593] Cut_chg(7)` (angezeigt ab F1594), Elza dreht zur Dachtuer, W-Clip ab F1629/F1630; `Se_on bank=14 id=0` (Tuer auf)
  zwischen fd_001665 und fd_001670; John erstmals nicht geparkt **F1670 bei angezeigtem Cut 7** (-7350,-1300) = hinter der Kamera;
  `Se_on bank=14 id=1` (Tuer zu) zwischen fd_001710 und fd_001715; `[scd F1739] Cut_chg(6)`: erstes Bild Cut 6 F1740 mit John
  (-7350,-1300) rot 1024 vor der Tuer; F1760 Cut 5, John laeuft heraus. "Bilder mit John sichtbar vor dem Auftritt: 0".
- Bild `J39abn3/a_tuer.png` (F1580 Cut 5 Elza rennt; F1595..F1735 Cut 7 Elza von vorn, Blick zur Tuer hinter der Kamera; F1740/F1750
  Cut 6 John vor der Dachtuer; F1760/F1800 Cut 5 John kommt zu Elza).
- **Urteil: erfuellt.**

### 1.3 Punkt 3 — kein "shout"-Voiceover fuer John
- Lauf A/P: 0x `main42.wav`, 0x bank 0x3D; Johns Stimmen in Phase H nur help01..05 (`[voice] clip loaded: help0N.wav`).
- **Urteil: erfuellt.**

### 1.4 Punkt 4 — help01..05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen
- Lauf A: je Runde drei Schlaege im Abstand 5 Bilder (F35/F40/F45, F255/F260/F265, F475/F480/F485, F884.., F1104.., F1324..),
  30 Bilder danach ein Hilferuf: help03 F75, help01 F295, help04 F515, help02 F924, help05 F1144, help03 F1364 (PORT-WAHL Mix);
  nach dem Riegel F1485 keiner mehr.
- Mitschnitt A: Schlaege Takt 3703/3708/3713 RMS 2481/2484/2502 (Spitze ~8000) gegen Musik RMS ~250; Kreuzkorrelation help03.wav
  rho 0,908 bei Takt 3742 (= F75), help01.wav rho 0,895 bei Takt 3962 (= F295).
- **Urteil: erfuellt** (Klang = RE2 DOOR0F-Holzschlag; Klangabnahme beim Nutzer, H1 der Abnahme 2).

### 1.5 Punkt 5 — massgeblich Nutzer-Nachtrag 10 (in 1141 alles wie bisher, nur ohne Elzas Reaktion; der Satz zieht nach ROOM1041 = Spur P)
- Lauf A: erste Nachricht in 1141 = msg 13 (Gemaelde) F676; erste Pad-Sperre F867 (Rolladen); 0x `[msg] room=1141 id=50`, 0x
  `main50.wav`; Kreuzkorrelation main50.wav im Fenster Takt 3600..4400: rho 0,063 (nicht gespielt). Bild `J39abn3/a0_frueh.png`
  (F400..F600 Elza an der Doppeltuer ohne Textfenster, F680 Gemaelde-Text; F50..F300 des Bogens sind von 1171 ueberschrieben).
- **Urteil: erfuellt** (in der Fassung von Nachtrag 10).

### 1.6 Punkt 6 — Glasbruch vor "What was that?!"
- Lauf A: msg 5 (John, main05.wav) offen F2224..F2423; `GLAS1090 Glasbruch` F2424 (Takt 6091) und F2429 (Takt 6096); msg 6
  `"Elza: What was that?!"` ab F2450. Mitschnitt: Takt 6088 RMS 1 -> Takt 6090..6102 RMS 5676..10144 (Spitze 28673).
- **Urteil: erfuellt.**

### 1.7 Punkt 7 — John schaut ab "...some kind of gorilla..." zu Elza
- Lauf A/P (r39.txt `blick`): msg 9 169 Bilder |blick| <= 10, msg 10 99 Bilder |blick| <= 10; msg 11 147 Bilder, davon die letzten 12
  > 160 = John laeuft in seine Ecke (Original sub02 @0x00F8E).
- **Urteil: erfuellt.**
