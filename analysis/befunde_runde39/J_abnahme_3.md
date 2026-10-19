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

### 1.8 Punkt 8 — nach dem Tod der Gorillas zwei Zeilen Johns "mit passenden Animationen" (+ Nachtraege 7, 8; Mangel M1 der Abnahme 2)
- **Ablauf (Lauf A/P, Messhaken F3833):** Balken + Pad-Sperre ab **F3834** (vorher erst mit dem Schnitt), `[scd F3863] Cut_chg(8)`
  (angezeigt F3864, gleiches Bild wie in Abnahme 2 = Sleep 30 jetzt VOR dem Schnitt). msg 51 `"John: Puuuh, that was close! I thought
  we end up as gorilla meal...."` F3886..F4060, msg 52 `"John: Elza, follow me, I think I might have found a way out of here!"`
  F4062..F4190; John laeuft F4192 los (0 Laufbilder mit offener msg 52); Kamera nach (9,67)=1: F4319 Cut 7, F4334 Cut 0, F5954 Cut 5,
  F6117 Cut 6 (folgt Elza).
- Johns Sub/Clip: msg 51 (0,15) 100 + (0,23) 75 Bilder; msg 52 alle 129 Bilder (0,2) = Heranwinken, `F4061 John winkt heran: RBJ ROOM1171
  Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)` (nach dem Merge mit Ks rbj_resolve_slot unveraendert); Blick msg 51/52 0; groesster
  Sprung Johns ohne Schnitt/Lauf 0. Bild `J39abn3/a_gespraech.png` (F3830 Cut 0 ohne Balken, F3835/F3860 mit Balken, F3865 Cut 8 Elza
  links / John rechts, F3900..F4150 Texte, F4195 Cut 7 John laeuft, F4335 Cut 0) und `a_wink_zoom.png` (F4085..F4140 John winkt mit
  erhobenem Arm, keine Waffe auf Elza).
- Stimmen (Nachtrag 8, Mitschnitt A): main51.wav rho **1,000** bei Takt 7552 (= F3885), main52.wav rho **1,000** bei Takt 7728 (= F4061);
  Gegenprobe main52 im Fenster von msg 51: rho 0,102. msg 12/13: `[voice] ROOM1141 msg 12/13: keine Aufnahme ... - Zeile bleibt stumm`.
- **M1 (Elza zielt beim Schnitt) - Lauf R2 = Abnahme-2-Nachstellung, echter Tod (60 Treffer-Zeilen, INIT 4 x hp=360, Gorilla Slot 2
  hp 0 F5624, Slot 3 hp 0 F6170; John 14 Schuesse / 14 Treffer).** state.txt: Schuss F6263 (mg 43 -> 42, ac 7 Rueckstoss), **Pad-Sperre
  ab F6266** (pm 2), Rueckstoss bis zum Bruch, **F6271..F6279 SENKEN** (mo 215, ac 6), **ab F6280 neutral** (mo 200); die Abzuege
  F6275..F6277 / F6287..F6289 (pad 8800) feuern nicht mehr (mg bleibt 42); `[scd F6296] Cut_chg(8)`, **F6296..F6306 pst 1 mo 200**
  (Abnahme 2: mo 215 = Waffe auf John), F6307 TURN (pst 4). Bilder `J39abn3/r2_schnitt.png` (F6268 Balken, F6271..F6280 Senken im
  Hintergrund, F6297 Cut 8) und `r2_cut8_zoom.png` (F6297..F6318: Elza steht, die Pistole haengt rechts nach unten, John ebenso -
  keine Waffe auf John).
- **Messer (Lauf K, R1 ueber Kampfende und Schnitt gehalten):** Messhaken F4056 = Sperre ab F4056 (F4056 noch Halten ac 8), F4057..F4065
  SENKEN (ac 6, 9 Bilder), ab F4066 neutral, `[scd F4086] Cut_chg(8)`, F4086..F4096 mo 200, F4097 TURN. Werkzeug des Riegels auf den
  Lauf: `test_r39_dining1141 zielen l_k` -> "Sperre ab 4056, letztes Zielbild 4065, Schnitt 4086; SENKEN-Bilder 9; Cut 8: 40 Bilder,
  davon 0 mit Waffe; Schnittbild pst=1 mo=200", `zielen: 0 Fehler`. Bild `J39abn3/k_schnitt.png`.
- **Randfall des Bau-Agenten (Nachladen kurz vor der Sperre), Lauf RL:** Schuss F4000 (mg 1 -> 0), Abzug F4048 mit leerem Magazin ->
  Nachladen (ac 13) F4048..F4078, Sperre F4056, F4079 Halten (mg 15), F4080..F4088 SENKEN, Schnitt F4086 -> **F4086..F4088 = 3 Bilder in
  Cut 8 mit mo 215** (davon 2 sichtbar, das Framedump-Bild F4086 zeigt noch Cut 5), ab F4089 neutral. Bild `J39abn3/rl_cut8_zoom.png`:
  F4087/F4088 haelt Elza die Pistole schon fast unten an der Huefte (Ende des Senkens), nicht auf John gerichtet. Die Original-Form
  ROOM11C0 sub03 ersetzt das Kommandowort des Spielers im Schnittbild (@0x1B26 Plc_dest), ENTRIEGELN39 erst 11 Bilder spaeter
  (Form 1141 sub02) - ein solcher Rest reicht deshalb hoechstens bis Plc_dest TURN (+11).

### 1.9 Punkt 9 — keine Panik-Musik beim Wechsel 1141 -> 1171
- Werkzeug des Riegels: `test_r39_dining1141 pegel l_a` -> `ROOM1171 main00 Sce_bgm_control op=1 bei Takt 9981`, `Takte 10011..10036
  Pegel 0, Takte 10051..10081 Pegel 559`, `pegel: 0 Fehler`.
- Eigene Pegel (`cap.py l_a/cap.raw 9930 10110 6`): Stille (RMS 0,7) bis Takt 9972, Tuer-Schliesston 9978..10002 (RMS 1354 -> 47),
  Stille 10008..10032, ab 10038 steigt MAIN32 (74 -> ~1100). Kein MAIN16 (in 1141 per op=2 gestoppt); debug.log 1171:
  `[bgm] stage=0 room=17 entry=5572 -> MAIN32(flag 1)`, `Sce_bgm_control slot=0 op=1 ... capTick=9981`.
- Mechanismus (audio_pc.c `if (!s_bgm_pending)` vor dem Lautstaerke-Switch, seit Abnahme 0 unveraendert): Original-Reihenfolge
  FUN_80044210 selbst nachdisassembliert: `8004428c lbu v0,21016(v0)` (0x800b5218), `8004429c jal 0x80029ac8` / `800442a0 ori a0,zero,0x1`,
  `800442b0 bne v0,zero,0x8004429c` (warten bis die Ausblendung fertig ist), `800442b8 jal 0x80044564` (MAIN laden); Raumlader
  `800399b0 jal 0x80044210` VOR `80039a00 jal 0x8003ef6c` (SCD-INIT).
- **Urteil: erfuellt.**

### 1.10 Nachtraege 4, 6, 7 (Wiederholung am Endstand)
- N4.1 Lauf A: `[dining] F674 ... Gemaelde (Ereignis 6) in Phase H -> Programm ROLLTOR39`, Rolladen-Ton 0x3B F866/F881/F1076,
  Pad-Sperre F867..F1100, danach frei F1101..F1484 (Elza geht von (-8134,-10294) durch die Rolladen-Oeffnung bis (-7266,-1060), Klopfen
  und Rufe laufen weiter), Riegel F1485. **erfuellt.**
- N4.2 Lauf A: `F1485 Dachtuer entriegelt: Elza hebt Waffe 1 (vorher 1, Messer)`, W-Clip 13 (Messer ziehen) ab F1630 bei Cut 7, John
  noch geparkt; Lauf P: `Elza hebt Waffe 3 (vorher 1, Schusswaffe)`, W-Clip 6 F1630..F1805; Bild `J39abn3/p_heben_zoom.png` (F1645..
  F1735 Browning beidhaendig auf die Tuer hinter der Kamera). Der neue Haken greift dabei nicht (Elza steht im Kommando 4, Plc-Executor;
  der Haken verlangt Kommando 1). **erfuellt.**
- N4.3 (John schiesst ab und an, Aufmerksamkeit): Abschnitt 3 (Riegel `kampf`, lebende Gorillas) und Lauf R2 (14 Schuesse / 14 Treffer).
- N4.4 / Nachtrag 7: Heranwinken RBJ ROOM1171 rec1 Clip 2 (96 Bilder), 0 Bilder Waffe auf Elza (1.8). **erfuellt.**
- N6: `"... we end up as gorilla meal...."` (Lauf A/P msgtext); msg 50 ("He is in trouble") kommt in 1141 nicht mehr vor, der Text
  bleibt im Riegel `texte` gepinnt. **erfuellt.**

## 2. RE-Gate (Nachbesserung 3): Stichproben selbst disassembliert/gelesen (richtige Binaerdatei, Sprungziele)
`S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (info/Re1.5/PSX.EXE); ROOM11C0.RDT = shared_assets == info/Re1.5
(`cmp` bytegleich).

| # | Zitat im Diff | selbst gelesen | Befund |
|---|---|---|---|
| 1 | FUN_80030444 kappt das Held-Wort: `lw g_pauseflags` @0x800304f4, `lui v1,0x100` @0x800304f8, `andi v0,v0,0xf000` @0x80030514, `sw 0x800ac768` @0x8003051c | `dis 0x80030444 90`: `800304f4 lw v0,-13760(v0)` (0x800aca40), `800304f8 lui v1,0x100`, `800304fc and`, `80030500 beq v0,zero,0x80030520`, `8003050c lw v0,-14488(v0)` (0x800ac768), `80030514 andi v0,v0,0xf000`, `8003051c sw v0,-14488(at)`. Die 16 virtuellen Bits baut die Schleife @0x800304b4..@0x800304ec aus der Tastentabelle 0x80073e1c[0x800b4870] -> 0x80073dbc (Belegung 0): Bit 0..3 = 0x1000/0x2000/0x4000/0x8000 (Kreuz oben/rechts/unten/links), Bit 8 = 0x0008 (R1), Bit 12..15 = 0x8000/0x2000/0x0080/0x0040 | stimmt; die Kappung laesst nur Bit 12..15 (Menue-Links/Rechts, Quadrat, Kreuz) stehen - kein Lauf-Kreuz, kein Zielbit 0x100. "Pad 0" im Spielerzweig des Ports ist dafuer gleichwertig (der Zweig liest weder Bit 12..15 noch das Menue) |
| 2 | Umsetzer im Task-Takt @0x80029dd8/@0x80029e14; Dispatcher haelt nur bei Bit 31 @0x80031c78; Kommando 1 = 0x80073f90[1] = 0x80031de8, [4] = 0x80030660 | `dis 0x80029dc8`: `80029dd8 jal 0x80030444`, `80029e14 jal 0x80030444`; `dis 0x80031c44`: `80031c54 lw a0,-13760(a0)` (g_pauseflags), `80031c78 bltz a0,0x80031da8`, dann `lbu 0x800aca58` / Tabelle 0x80073f90 / `80031cb4 jalr v0`; `table 0x80073f90`: [1] 0x80031de8, [4] 0x80030660; Sprungziel `dis 0x80031de8`: kein Zugriff auf g_pauseflags, Sub-Verteiler 0x80073fb0/0x80073ff0 nach 0x800aca59 (`jalr` @0x80031ef4/@0x80031f1c) | stimmt (unter (2,7) laeuft die Ziel-FSM im Original weiter) |
| 3 | Ziel-FSM senkt ohne Bit 0x100: HOLD @0x800331e4/@0x800331ec/@0x800331f8/@0x80033200, Messer @0x80035128..@0x80035144, Rueckstoss @0x80033560/@0x80033600, Schwelle @0x80074092+(w-1)*5 (@0x8003362c), @0x8003364c | `dis 0x800331d0`: `800331e4 lw v1,-14488(v1)`, `800331ec andi v0,v1,0x100`, `800331f0 bne ..,0x8003320c`, `800331f8 ori v0,zero,0x3`, `80033200 sh v0,-13734(at)` (aca5a = 3); `dis 0x80035120`: `80035128 lw`, `80035130 andi 0x100`, `8003513c ori 3`, `80035144 sh aca5a`; `dis 0x800335f8`: `80033600 andi v0,a0,0x100`, `8003362c addiu at,at,16530` (0x80074092), `8003363c sltu`, `8003364c sh v0,-13734(at)`; `read 0x80074092 8 --w 1 --stride 5` = [0,0,7,7,10,10,10,10] (Waffe 3/4 = 7, wie player_common.c recoil_break) | stimmt |
| 4 | Set(2,7) = Bit 0x01000000 von 0x800aca40: Set-Tabelle @0x80074664[2] | `table 0x80074664 4`: [0] 0x800aca38, [1] 0x800aca3c, **[2] 0x800aca40**, [3] 0x800b0ff8 | stimmt |
| 5 | Original-Form ROOM11C0 sub03 @0x01B06/@0x01B0A/@0x01B14/@0x01B18, aufgerufen nach dem Tod der Parkplatz-Gorillas (sub01 @0x01836/@0x0183E -> @0x01842) | `xxd -s 0x1AF0`: @0x1B02 `22 03 43 01`, **@0x1B06 `22 01 1b 01`**, **@0x1B0A `22 02 07 01`**, @0x1B0E `54 00 02 00 00 00`, **@0x1B14 `09 0a 1e 00`**, **@0x1B18 `29 04`**; `scd_dump_room.py`: "=== sub03 [file 0x1B02..0x1C0C]"; `xxd -s 0x1820`: @0x1836 `21 07 60 01`, @0x183E `21 07 61 01`, @0x1842 `04 ff 18 03` | stimmt (Balken + Sperre + Sleep 30 VOR dem Schnitt). Unterschied zur Port-Szene: in 11C0 folgt dem Schnitt sofort `40 00 04 00 ..` Plc_dest (@0x1B26) - das Kommandowort des Spielers wechselt im Schnittbild; ENTRIEGELN39 setzt Elzas Plc_dest TURN erst 11 Bilder nach dem Schnitt (Form 1141 sub02 @0x00C24..@0x00C3E, Sleep 1 + Sleep 10). Wirkung nur im Randfall 1.8 |
| 6 | Programm-Bytes ENTRIEGELN39 +0x03FC..+0x0409 | gen/dining_r39_programme.inc: vorher `09 0a 1e 00 29 08 22 02 07 01 22 01 1b 01`, jetzt `22 01 1b 01 22 02 07 01 09 0a 1e 00 29 08` (gleiche Laenge 1376 B, Anker unveraendert); Generator `--pruefen` ok | stimmt |

Suche in allen `+`-Zeilen von `git diff master...HEAD -- re15_port` nach `deferred|tunable|interim|for now|faithful|plausib|TODO|
FIXME|vorerst|geschaetzt|approx|ungefaehr|platzhalter|placeholder|sieht richtig|nach gefuehl`: **0 Treffer**. Neue getenv: nur
`RE15_R39J_LOG` (Messschiene) und `RE15_SE_DEBUG` (Log) - kein Env-Schalter als Abschluss. Neue Konstanten der Nachbesserung 3:
keine Zahl ausser der Programm-Umstellung (Sleep 30 = @0x01B14); der Haken arbeitet nur mit Bedingungen (Raum, Phase H, Sperr-Bit,
Kommando 1, Zielphase). Fix erklaert den Befund: Ursache "player_mode 2 haelt die Ziel-FSM an" im Code nachgelesen
(player_common.c `if (g_scd.player_mode != 2 && !msg_block && !pushing)` vor dem Haken; main.c re15_cine_active = (1,27)||(2,7));
im Port wird RE15_PAUSE_PAD nur vom Skript gesetzt (`grep RE15_PAUSE_PAD`: game_state.c Set-Handler, Leser game_step_common.c /
pad_phase_common.c / panel_zeiger_common.c / dining_r39.c) - der Haken greift also nur unter Set(2,7) in ROOM1141 Phase H.
