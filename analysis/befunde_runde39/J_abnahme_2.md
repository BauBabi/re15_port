# Runde 39 — Spur J "dining1141": Abnahme 2 (unabhaengig, nach Nachbesserung 2)

Baum `.claude/worktrees/r39_dining1141`, Zweig `r39/dining1141`, Stand 6c2a0547 (Merge-Basis mit master 28dad473;
master = 2963579d mit Spur K). Gepruefter Code-Diff: `git diff master...HEAD` (27 Dateien, nur Zweig-Aenderungen; die
weiteren Dateien in `git diff master` - K_*.md, heliport1171_r39.*, test_r39_heliport1171.*, UEBERGABE.md, orchestrator/*,
synchro/STAGE1/room1041/main40.wav, elza_r39.c, szenenleon_r36.c, actor_locomotion.c, player_common.c ... - sind
master-Commits nach dem Abzweig). Massstab: AUFTRAG.md (Punkte 1-9 "Mit Room 1141 ..." + "Wenn man ROOM 1141 danach
verlaesst ..."; dazu Nutzer-Nachtraege 4 (N4.1-N4.4), 6, 7, 8 und 10), VERTRAG.md, Abnahme 1 (`J_abnahme_1.md`, Mangel M1).
Messordner (nicht committet): scratchpad `J39abn2/` (Laeufe `l_<name>`, eigene exe-Kopie `re15_pc_j39abn2_<name>.exe` je
Lauf neben der Baum-exe, danach geloescht).

## 0. Stand der Abnahme (fortlaufend)
- Abnahme begonnen; AUFTRAG/VERTRAG/Dossier Abschnitt 9/Abnahme 1 gelesen, Code-Diff (`git diff master...HEAD -- re15_port`) gelesen.
- Bau: `local_build.sh configure` + `build` -> `LOCAL-BUILD-OK (configure)`, `LOCAL-BUILD-OK (build)`, `ninja: no work to do`
  (exe 00:33:45 = Stand 6161ac63, game_step_common.c 00:33:01; spaetere Commits nur Riegel/Dossier).
- Generator `python re15_port/tools/r39_dining/programme_bauen.py --pruefen` -> `dining_r39_programme.inc ok` (Listing
  scratchpad `J39abn2/listing.txt`).
- `git merge-tree --write-tree master HEAD`: einziger Konflikt AUFTRAG.md (Doku); enemy_common.c / enemy_ai_common.c /
  game_step_common.c automatisch zusammenfuehrbar (Spur K 18940e95 aendert rbj_resolve_slot, J haengt davor in
  re15_actor_rbj_anim/_skel).

## 1. Messlaeufe (echte exe, eigene Kopie je Lauf)
Gemeinsame Umgebung (`J39abn2/lauf.sh`, wie Abnahme 1): RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_PSELECT_AUTO(_SWITCH)=1
RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_EVT_TRACE=1 RE15_BGM_CTL_DEBUG=1 RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw
RE15_R39J_LOG=r39.txt RE15_STIMME_LOG=1 RE15_R39_GEGNER_LOG=1 RE15_WEGPLAN=<plan> RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78
(Nach-1021-Flags wie der Riegel) RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0 RE15_FRAMEDUMP=<bereich>.
Bildnummern = Bilder seit Raumeintritt. Wegplan `plan_a.txt` = bytegleich der Wegplan von integration_r39_dining1141_d
(1131 Schluessel + Tuer -> 1141 WARTE 420, Gemaelde, 200 freie Bilder, Riegel, Szene, Doppeltuer msg 41, Kampf, Dachtuer -> 1171).
| Lauf | Wegplan / Umgebung | Ergebnis |
|---|---|---|
| A0 | `plan_a.txt`, RE15_GIVE=0x43:1 (nur Schluessel), EXIT 700#1141, FD 0-700/5 | rv=0, 154 s |

### Punkt 5 (mit Nutzer-Nachtrag 10: "Im ROOM 1141 soll alles so bleiben wie bisher, nur eben ohne die 'Oh, no, this is John! ...' Cutscene")
- Lauf A0 (r39.txt F1..F700): **0 Bilder mit Pad-Sperre** (pm != 0), b94 immer 0, John die ganze Zeit geparkt (-30000,-30000);
  einzige Nachricht in 1141 = msg 13 (Gemaelde) ab F676. debug.log Abschnitt 1141: kein `[msg] room=1141 id=50`, kein
  `clip loaded: main50.wav`; Klopfen F35/F40/F45 -> help03 F75 -> F255/F260/F265 -> help01 F295 -> F475/F480/F485 -> help04 F515.
- Bild `J39abn2/a0_frueh.png` (F50..F600 Elza an der Doppeltuer ohne Textfenster, F680 Gemaelde-Text "A beaut...").

## 2. RE-Gate: Stichproben selbst disassembliert/gelesen (richtige Binaerdatei, Sprungziel)
`S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (PSX.EXE fuer 0x800x, STAGE1.BIN roh fuer 0x8010..0x8012).
| # | Zitat im Diff (Nachbesserung 2) | selbst gelesen | Befund |
|---|---|---|---|
| 1 | (b) Nachfuehren `RE15_R39J_NACHFUEHREN` 0xC8 = Gun-FSM Sub 5 `ori a1,zero,0xc8` @0x80033f90 / `jal 0x8001a8f8` @0x80033f94 (a0 = Ziel+0x34 @0x80033f98) | `dis 0x80033eec 60`: `80033f8c lw a0,0(s0)` (s0 = 0x800acbfc Ziel), `80033f90 ori a1,zero,0xc8`, `80033f94 jal 0x8001a8f8`, `80033f98 addiu a0,a0,52`; Sprungziel `dis 0x8001a8f8`: atan2_q12 (`8001a928 jal 0x8001a6d4`) zur Zielposition, dann +0x6a um hoechstens a1 drehen (`8001a958 lhu a2,106(v1)` ... `8001a984/8001a988/8001a9b0 sh ..,106(v1)`) | stimmt |
| 2 | (c) Zuwendung `RE15_R39J_ZUGEWANDT` 0x180 = Gorilla A[3] `a804(0xbb8,0x180)` @0x80117a60-74; B[3]-Lenkschritt `(rng & 0x1f) + 6` @0x80117d1c-34, `jal 0x8001a8f8` @0x80117d50 | STAGE1.BIN `dis 0x80117a50`: `80117a60 ori a0,zero,0xbb8`, `80117a64 ori a1,zero,0x180`, `80117a6c jal 0x8001a804`; Sprungziel `dis 0x8001a804`: SquareRoot0-Abstand `<= a0`, dann atan2 und Kegel `+-a1` (`8001a8b0..8001a8d0`, Bit 31 = im Kegel). `dis 0x80117d10`: `80117d1c jal 0x8001af20` (rng), `80117d24 andi v0,v0,0x1f`, `80117d30 addiu v0,v0,6`, `80117d34 sb v0,158(v1)`, `80117d4c` a0 = playerX, `80117d50 jal 0x8001a8f8` | stimmt (0x180 ist der Biss-KEGEL des Originals; ihn als "John zugewandt" zu werten ist Teil der PORT-WAHL "Aufmerksamkeit") |
| 3 | (a) Wurf im Spieler-Takt: `jalr v0` @0x80031cb4, Ende P7 0x8011c384 (aca58 = 1, +0x93 = 0) | `dis 0x80031c80`: `80031c8c lbu v1,-13736` (aca58), `80031ca4 addiu at,at,16272` (0x80073f90), `80031cb4 jalr v0`; STAGE1.BIN `dis 0x8011c384`: `ori v0,zero,0x1` / `sw v0,-13736(at)` (aca58 = 1), `8011c3a0 sb zero,-13593(at)` (+0x93 = 0) | stimmt (die Bindung 0x8011c118 = Kommando 5 steht im Bestand re15_affen.h (6d) mit GDB-Beleg; statische Tabelle 0x80073f90[5] = 0x80036834) |
| 4 | Kamera am Ende `RE15_R39J_CUT_ENDE` 0 = RVD @0x01A8 Satz 0 / Satz 22; Form sub02 @0x00E80 `29 00` | ROOM1141.RDT (shared_assets == info/Re1.5, `cmp` bytegleich) RDT+0x28 -> 0x1A8, 20 B je Satz: Satz 0 `01 ff 00 00 fc d6 64 9c fc d6 cc de b8 0b cc de b8 0b 64 9c` = Gruppe Cut 0, Viereck x -10500..3000, z -25500..-8500 (enthaelt (-5800,-21500)); Satz 22 `01 ff 08 00 ..` ist der einzige Satz der Gruppe Cut 8 (Saetze 23 = Gruppe 9, 24 = Ende `ff ff`); @0x00E80 `29 00`, @0x00F56 `3c 01` | stimmt |
| 5 | Punkt 9 (audio_pc.c, seit Abnahme 0 unveraendert): Warten @0x8004429c-@0x800442b0, MAIN laden @0x800442b8, Raumlader @0x800399b0 vor SCD-INIT @0x80039a00 | `dis 0x80044278`: `8004428c lbu v0,21016(v0)` (0x800b5218), `8004429c jal 0x80029ac8` / `800442a0 ori a0,zero,0x1`, `800442b0 bne v0,zero,0x8004429c`, `800442b8 jal 0x80044564`; `800399b0 jal 0x80044210`, `80039a00 jal 0x8003ef6c` | stimmt |
Suche in den `+`-Zeilen von `git diff master...HEAD -- re15_port` nach `deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|
vorerst|geschaetzt|approx|ungefaehr|platzhalter|placeholder`: **0 Treffer**. Neue getenv: nur `RE15_R39J_LOG` (Messschiene) und
`RE15_SE_DEBUG` (Log). Konstanten der Nachbesserung 2: 0xC8 (@0x80033f90), 0x180 (@0x80117a64), Cut 0 (RVD @0x01A8), msg 58
(Port-Warte-Zeile, Form `2b 05 00 00` sub02 @0x00D9E); Ziel-Regel/Takt/Dauer/NAH = PORT-WAHL (gekennzeichnet im Kopf
re15_dining_r39.h). Generator `--pruefen` ok (gen/*.inc = Werkzeugausgabe).

### Weitere Laeufe
| Lauf | Wegplan / Umgebung | Ergebnis |
|---|---|---|
| A | `plan_a.txt`, RE15_GIVE=0x43:1 (Messer), RE15_R38F_GORILLAS_TOT=600 + _STARR=1, EXIT 300#1171, FD 0-6200/5 | rv=0, 383 s, 1131 -> 1141 -> 1171 |
| P | wie A, RE15_GIVE=0x43:1,0x03:15,0x15:50 (Browning im Inventar, nicht ausgeruestet), FD 1500-6200/5 | rv=0, 378 s, 1131 -> 1141 -> 1171 |
| L3 | `plan_l3.txt` (Elza nach der Szene neben John (-5800,-20500)), Pistole, Gorillas LEBEND, RE15_R38F_GORILLAS_TOT=2400, EXIT 5500#1141 | Elza stirbt F5421 (ohne Messhaken der Energie); exe stand im Todesbild, eigene Kopie per PID beendet (rv 127, Daten bis F5421) |
Auswertung: `J39abn2/auswert.py <lauf>` (r39.txt: Kamerafolge, Nachrichtenfenster, Johns erster Auftritt, W-Clip, Pad-Sperre, Blick
je Zeile, Johns Sub/Clip bei msg 51/52, Laufbilder mit offener msg 52, Kamera nach (9,67), groesster Sprung), `cap.py`
(Pegel je Takt), `stimme_xkorr.py` (Kreuzkorrelation Nutzer-WAV gegen Mitschnitt), `salven.py`/`aufmerk.py` (Schuesse/Zuwendung),
`bogen.py`/`crop.py` (Bilder). Takt-Bezug Lauf A/P: Takt = F + 3666 (`F35 tick=3701`).

### Punkt 1 — keine Zombie-Geraeusche
- debug.log Abschnitt 1141, alle Skript-Toene (`SCD Se_on: bank=`), Lauf A: 18x bank 62 Satz 0 (Klopfen), Hilferufe Satz 49..53
  (help01..05, help03 zweimal), 2x Satz 33 (Glas), bank 59 Satz 10/11/12 (Rolladen), 2x2 bank 14 (DOOR1A). **0 Zeilen**
  `bank=0x3C` / `bank=0x3D` / `bank=60 ` / `bank=61 ` / `main42`. Lauf A0 (F1..F700): nur bank 62 (9 Schlaege, 3 Rufe), dazu MAIN16;
  alle 9 `[se] Stimme`-Zeilen = vag 1, 44100 Hz (Klopfton). Unit `phase_h`: "kein Se_on 0x3C/0x3D ... in Phase H".
- **Urteil: erfuellt.**

### Punkt 3 — kein "shout"-Voiceover fuer John
- Lauf A/P/A0: 0 x `main42.wav`, 0 x bank 0x3D; Johns Stimmen in Phase H nur help01..05 (Nutzer-Dateien, `[voice] clip loaded: help0N.wav`).
- **Urteil: erfuellt.**

### Punkt 4 — help01..05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen
- Lauf A0/A: je Runde drei Schlaege im Abstand 5 Bilder (F35/F40/F45, F255/F260/F265, F475/F480/F485 ...), 30 Bilder danach ein
  Hilferuf, Reihenfolge help03 (F75), help01 (F295), help04 (F515), help02, help05, help03 ... (PORT-WAHL Mix), Abstand der Runden
  220 Bilder (seit Nachtrag 10 ohne die Reaktion); Lauf A: 18 Schlaege, 6 Rufe, nach dem Riegel F1485 keiner mehr.
- Mitschnitt A0: Schlaege Takt 3702/3708/3712 RMS 1761/1862/1774, Spitze ~8000 ueber MAIN16 (RMS 250..300) - hoerbar.
- **Urteil: erfuellt** (Klang = RE2 DOOR0F-Holzschlag, RE2 hat keinen Tuer-Klopfton - Klangabnahme beim Nutzer, H1).

### Punkt 2 — John durch die Tuer; Kamera vorne auf Elza, Tuer-zu-Ton, dann Kamera auf die Tuer mit John
- Lauf A (Messer) und P (Pistole) bildgleich: Riegel F1485 (`Dachtuer entriegelt`), msg 12 F1487..F1529, Elza rennt (Plc_dest RUN
  F1530, Kamera-Automatik Cut 6 -> 5), `[scd F1593] Cut_chg(7)` (angezeigt ab F1594), Elza dreht zur Dachtuer (rot 3791 -> 3101) und hebt
  ab F1629/F1630 die Waffe; `Se_on bank=14 id=0` (Tuer auf) zwischen fd_001665 und fd_001670; John erstmals nicht geparkt **F1670
  bei angezeigtem Cut 7** (-7350,-1300) = hinter der Kamera; `Se_on bank=14 id=1` (Tuer zu) zwischen fd_001710 und fd_001715;
  `[scd F1739] Cut_chg(6)` + MAIN16-Stopp: erstes Bild Cut 6 F1740 mit John (-7350,-1300) vor der Tuer; F1760 Cut 5, John laeuft heraus.
  0 Bilder mit sichtbarem John vor dem Auftritt (Cut 6, nicht geparkt).
- Mitschnitt A: Tuer-auf Takt 5338..5346 (Spitze 8752..9044 ueber MAIN16 ~1000 RMS), Tuer-zu Takt 5378..5388 (Spitze 8103..8890),
  MAIN16-Stopp op=2 Takt 5405 -> Takt 5420 RMS 53.
- Bild `J39abn2/a_tuer.png` (F1580 Cut 5 Elza rennt; F1595..F1730 Cut 7 Elza von vorn; F1740/F1750 Cut 6 John vor der Dachtuer;
  F1760..F1840 Cut 5 John kommt zu Elza).
- **Urteil: erfuellt.**

### Punkt 6 — Glasbruch vor "What was that?!"
- Lauf A/P: msg 5 (John, main05.wav) offen F2224..F2423; Warte-Zeile msg 58; `GLAS1090 Glasbruch` F2424 (Takt 6090) und F2429
  (Takt 6095); msg 6 `"Elza: What was that?!"` offen ab F2450.
- **Urteil: erfuellt.**

### Punkt 7 — John schaut ab "...some kind of gorilla..." zu Elza
- Lauf A/P (r39.txt `blick` = Abweichung Johns Blick zur Peilung auf Elza, 4096 = 360 Grad): msg 9 169 Bilder |blick| <= 10,
  msg 10 99 Bilder |blick| <= 10; msg 11 147 Bilder, davon die letzten 12 > 160 = John laeuft in seine Ecke (Original sub02 @0x00F8E).
- **Urteil: erfuellt.**

### Punkt 8 — nach dem Tod der Gorillas zwei Zeilen Johns mit passenden Animationen (+ Nachtrag 7 Heranwinken, Nachtrag 8 Stimmen)
- Lauf A/P: Messhaken F3833 (Kill-Flags), Cut 8 ab F3864 + Pad-Sperre; msg 51 `"John: Puuuh, that was close! I thought we end up as
  gorilla meal...."` offen F3886..F4060 (175 Bilder), msg 52 `"John: Elza, follow me, I think I might have found a way out of here!"`
  F4062..F4190 (129 Bilder); John laeuft erst **F4192** los (0 Laufbilder mit offener msg 52; Abnahme-1-Stand: 29).
- Johns Sub/Clip (r39.txt): msg 51 (0,15) 100 Bilder + (0,23) 75 Bilder (Gesten wie zu msg 1, sub02 @0x00CB8..@0x00CCC); msg 52 alle
  129 Bilder (0,2) = Heranwinken, `F4061 John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)`; 0 Bilder
  Sub 2 Clip 6 (Waffe auf Elza). Blick msg 51/52: 0 in allen 304 Bildern. Groesster Sprung Johns ohne Schnitt/Lauf: 0.
- Stimmen (Nachtrag 8, Mitschnitt A, `stimme_xkorr.py`): main51.wav rho **1,000** bei Takt 7551 (= F3885, Zeile offen ab F3886;
  Haelften 1,000/1,000, Median 0,008), main52.wav rho **1,000** bei Takt 7727 (= F4061; msg 52 ab F4062). Gegenproben: main52 im
  Fenster von msg 51 0,047; main50 im Fenster Takt 3600..4400 (Fruehphase) 0,063 (nicht gespielt). msg 12/13: `[voice] ROOM1141
  msg 12/13: keine Aufnahme ... - Zeile bleibt stumm`; 0 x `main12/13/14.wav`.
- Bilder `J39abn2/a_gespraech.png` (F3865 Cut 8 Elza links / John rechts einander zugewandt, Texte, F4195 Cut 7 John laeuft los,
  F4335/F4400 Cut 0) und `a_wink_zoom.png` (F4085..F4135 John streckt den Arm zu Elza und winkt mehrmals heran; ab ~F4150 haelt er
  das Endbild des 96-Bilder-Clips, waehrend main52 noch 1 s weiterlaeuft).
- **Urteil: erfuellt.**

### Nachbesserung 2 — Kamera nach Johns Abgang
- Lauf A/P nach (9,67)=1: F4319 Cut 7, **F4334 Cut 0** (Elza (-5800,-21500)), F5924 Cut 5 (Elza (-6085,-11904)), F6087 Cut 6 (Elza an
  der Dachtuer (-7137,-1414)). Vorher (Dossier 9.1, Lauf v_a) blieb Cut 8 bis zur Dachtuer stehen.
- **erfuellt** (Kamera folgt Elza wieder).

### Punkt 9 — keine Panik-Musik beim Wechsel 1141 -> 1171
- Werkzeug des Riegels: `test_r39_dining1141 pegel l_a` -> `ROOM1171 main00 Sce_bgm_control op=1 bei Takt 9950`, `Takte 9980..10005
  Pegel 0, Takte 10020..10050 Pegel 559`, `pegel: 0 Fehler` (Lauf P identisch).
- Eigene Pegel (`cap.py l_a/cap.raw 9880 10100 4`): Stille (RMS 0,7) bis Takt 9944, Tuer-Schliesston 9948..9972 (RMS 1356 -> 36),
  Stille 9976..10004, ab 10008 steigt MAIN32 (51 -> ~1000). Kein MAIN16 (in 1141 per op=2 Takt 7499 gestoppt).
- Mechanismus: audio_pc.c `if (!s_bgm_pending)` vor dem Lautstaerke-Switch in ss_seq_ctl_ex (seit Abnahme 0 unveraendert);
  Original-Reihenfolge selbst gelesen (Abschnitt 2, Zeile 5); Gegenprobe ohne den Haken: Abnahme 0 (Pegel 496 = MAIN16) - gilt weiter.
- **Urteil: erfuellt.**

### N4.1 / N4.2 / N4.4 / N6 (Nutzer-Nachtraege 4, 6, 7) - Wiederholung am Endstand
- N4.1 Lauf A: Gemaelde-Frage + Rolladen unter Pad-Sperre F867..F1100, danach frei F1101..F1484 (384 Bilder; Elza geht durch die
  Rolladen-Oeffnung zur Dachtuer, Klopfen/Rufe laufen weiter), Riegel F1485 bei Elza z -1060. ROLLTOR39 setzt (3,0x6A)=1 und schaltet
  den Gemaelde-Slot ab (`46 03 00 00 ..` = sub06 @0x00FFA) - kein zweites Hochfahren.
- N4.2 Lauf A: `Elza hebt Waffe 1 (vorher 1, Messer)`, W-Clip 13 (DRAW) ab F1630 bei Cut 7, John noch geparkt, Elza z -12652
  (`a_messer_zoom.png`: Messer ueber den Koerper gezogen, Bereitschaft); Lauf P: `Elza hebt Waffe 3 (vorher 1, Schusswaffe)`,
  W-Clip 6 F1630..F1805 (`p_heben_zoom.png`: Pistole beidhaendig auf die Tuer hinter der Kamera, gehalten bis Cut 6, gesenkt F1759).
- N4.4 / Nachtrag 7: Punkt 8 oben (Heranwinken RBJ ROOM1171 rec1 Clip 2, 0 Bilder Waffe auf Elza).
- N6: `"... we end up as gorilla meal...."` (A/P), Riegel `texte` prueft "He is in trouble" (msg 50, Text weiter vorhanden).
- **Urteil N4.1/N4.2/N4.4/N6: erfuellt.**

### N4.3 (Nachtrag 4: "John sollte auch ab und an mal auf die Gorillas schiessen ... und dann ihre Aufmerksamkeit erhalten") - Lauf L3
`salven.py l_l3` / `aufmerk.py` / debug.log (Elza neben John, wehrt sich nicht, ohne Energie-Messhaken):
- 5 Salven F3657..F5185, **10 Schuesse, 10 Treffer**, alle auf Slot 3 (den Gorilla, der Elza NICHT haelt); 6 Treffer bei gr=1
  (Elza im Griff des anderen Gorillas) -> Mangel M1 (a) der Abnahme 1 behoben. rel -6..+2 (Kegel +-0xC8) -> (b) behoben.
- Zuwendung: nach jedem Treffer-Paar `Gorilla Slot 3 John zugewandt` (9 bzw. 0..24 Bilder nach Schuss 1); danach endet jede Episode
  nach 59..84 Bildern mit `festgeklemmt`: Abstand zu John 3729 -> 3373, 4388 -> 2898, 3426 -> 3340, 3436 -> 3462, 3402 -> 3403 - nur
  in 1 von 5 Episoden ein Heranruecken (~1500); der Gorilla steht oestlich von John (x ~ -4200 gegen -7535), Elza und der Greifer
  stehen dazwischen.
- John im Bild: alle 10 Schuesse bei Cut 0 (Projektion (122,103)); Bild `J39abn2/l3_john_zoom.png`: John steht zwischen/hinter den
  beiden Gorillas und ist weitgehend verdeckt (F3655/F3660, F4435, F5155 Kopf/Jeans sichtbar).

## 3. Gezielte Tests (keine volle Suite im Spurbaum)
- Unit (Module des Diffs: dining_r38/39, Kartenbits, Gorilla-Hirn enemy_ai_common.c, RBJ-Kanal enemy_common.c, Schaden re15_damage.c,
  scd_vm.c (Plc_motion/Message_on/VM-Takt), Spieler-/NPC-/Anim-Pfad, Ton/Stimme/BGM audio_pc.c, Nachrichten-FSM, Heliport 1171,
  Gegner/Treffer/Elza/Inventar/Toene/Funk der Runde 39, Pausemasken/Einfrieren wegen des umgesetzten game_step-Hakens), Regex
  `J39abn2/regex_unit.txt` (= Regex der Abnahme 1 + `unit_text_freeze|unit_scd_zone2_pauseflags|unit_r38_zopf_freeze|unit_r39_funk10f1|
  unit_r39_toene|unit_fenster1120|unit_r35_fenster|unit_r39_inventar`):
  `ctest -R "<regex>" -j 6 --timeout 600` -> **100% tests passed, 0 tests failed out of 192** (47,6 s).
- `test_r39_dining1141 {texte,programme,stamm,phase_h,fenster}` einzeln: je `0 Fehler` (Ausgaben `J39abn2/u_*.txt`; programme
  prueft u. a. "ENTRIEGELN39 NB2: Heranwinken -> Sleep 60 + 40 -> Message_on 58", "Ende Cut_chg 0 + Cut_auto 1", "HAEMMERN39 Nachtrag 10:
  keine Reaktion in 1141"; phase_h: "900 Bilder Phase H: keine Nachricht, keine Pad-Sperre", Trefferbedingung (a), Nachfuehren (b)).

### N4.3 - Laeufe L und L2 (lebende Gorillas, Messhaken RE15_R39_MESS_SPIELER_HP=100 wie der Riegel `kampf`, Elza wehrt sich nicht)
| Lauf | Lage Elza | Salven | Schuesse / Treffer | "John zugewandt" | Annaeherung je Episode (Abstand zu John, \|dx\|+\|dz\|) | John im Bild |
|---|---|---|---|---|---|---|
| L (`plan_a.txt`) | Doppeltuer/Raummitte | 5 | 10 / 10 (2 bei gr=1) | 5 von 5 | 5878->2797, 4086->2522, 9268->2525, 8153->2529, 9289->2536 | Salven 1-2 Cut 0 |
| L2 (`plan_l2.txt`) | Gang (-7350,-9000) | 6 | 12 / 12 | 6 von 6 | 6576->2793, 9542->2533, 9394->2579, 9387->2531, 9388->2529, 9393->6368 (Laufende) | nein (Cut 5) |
| L3 (`plan_l3.txt`) | neben John | 5 | 10 / 10 (6 bei gr=1) | 5 von 5 | 3729->3373, 4388->2898, 3426->3340, 3436->3462, 3402->3403 (blockiert) | alle Cut 0, meist verdeckt |
- Zustand des Ziels (state.txt L3 Slot 3): nach dem Treffer F3657 dreht der Gorilla von rel -538 (F3655) auf rel ~0 (F3680) zu John;
  er bleibt zugewandt bis zum Prueftakt (F3716 `festgeklemmt`), danach wieder das Original-Hirn.
- Bilder: `J39abn2/l_john_zoom.png` (Lauf L F4030..F4065: John links neben der Pflanze zielt beidhaendig auf die heranrueckenden
  Gorillas und schiesst); `J39abn2/l2_salve2.png` (Lauf L2 F4050..F4260: ein Gorilla dreht ab, laeuft an der Kamera vorbei aus dem
  Bild nach Sueden zu John und kommt spaeter zurueck).
- Gegen den Befund der Abnahme 1 (16 Salven: 6 ohne Treffer, 4 nur Zucken): jetzt 16 Salven, 32/32 Treffer, 16/16 mit Zuwendung;
  in L/L2 laeuft der getroffene Gorilla jedes Mal bis an John heran (Endabstand ~2520 = Koerperkontakt, Dossier 9.7), in L3 versperren
  Elza und der Greifer den Weg - dort dreht er sich zu John und gibt nach 2-3 s auf. Die Zahlen von L2 sind bildgleich zu f_l2 des
  Dossiers (9.13), L weicht nur wegen des Energie-Messhakens ab.
- **Urteil N4.3: erfuellt** (Takt, Treffer, Zuwendung messbar und im Bild; Gorillas greifen John nicht an = PORT-WAHL, dem Nutzer
  im Dossier 9.8 in einfachen Worten beschrieben).
