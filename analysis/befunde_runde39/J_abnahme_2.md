# Runde 39 — Spur J "dining1141": Abnahme 2 (unabhaengig, nach Nachbesserung 2)

Baum `.claude/worktrees/r39_dining1141`, Zweig `r39/dining1141`, Stand 6c2a0547 (Merge-Basis mit master 28dad473;
master = 2963579d mit Spur K). Gepruefter Code-Diff: `git diff master...HEAD` (27 Dateien, nur Zweig-Aenderungen; die
weiteren Dateien in `git diff master` - K_*.md, heliport1171_r39.*, test_r39_heliport1171.*, UEBERGABE.md, orchestrator/*,
synchro/STAGE1/room1041/main40.wav, elza_r39.c, szenenleon_r36.c, actor_locomotion.c, player_common.c ... - sind
master-Commits nach dem Abzweig). Massstab: AUFTRAG.md (Punkte 1-9 "Mit Room 1141 ..." + "Wenn man ROOM 1141 danach
verlaesst ..."; dazu Nutzer-Nachtraege 4 (N4.1-N4.4), 6, 7, 8 und 10), VERTRAG.md, Abnahme 1 (`J_abnahme_1.md`, Mangel M1,
Hinweis H3 "die Szene einmal mit echtem Tod der Gorillas fahren").
Messordner (nicht committet): scratchpad `J39abn2/` (Laeufe `l_<name>`, eigene exe-Kopie `re15_pc_j39abn2_<name>.exe` je
Lauf neben der Baum-exe, danach geloescht; Werkzeuge `lauf.sh`, `auswert.py`, `frost.py`, `cap.py`, `stimme_xkorr.py`,
`salven.py`, `aufmerk.py`, `bogen.py`, `crop.py`).

## 0. Stand der Abnahme
- AUFTRAG/VERTRAG/Dossier (Abschnitt 9)/Abnahme 1 gelesen, Code-Diff (`git diff master...HEAD -- re15_port`) gelesen.
- Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK (configure)`, `LOCAL-BUILD-OK (build)`,
  `ninja: no work to do` (exe 00:33:45 = Stand 6161ac63, game_step_common.c 00:33:01; spaetere Commits nur Riegel/Dossier).
- Generator `python re15_port/tools/r39_dining/programme_bauen.py --pruefen` -> `dining_r39_programme.inc ok` (Listing
  `J39abn2/listing.txt`).
- `git merge-tree --write-tree master HEAD`: einziger Konflikt AUFTRAG.md (Doku); enemy_common.c / enemy_ai_common.c /
  game_step_common.c automatisch zusammenfuehrbar (Spur K 18940e95 aendert rbj_resolve_slot, J haengt davor in
  re15_actor_rbj_anim/_skel).
- Laeufe A0, A, P, L3, L, L2, M, R, R2 (Abschnitt 1); Unit gezielt 192/192, Integration gezielt 20/20 (Abschnitt 3).
- Zwischenstand committet 21dbf4a5; Endstand dieser Bericht.

## 1. Messlaeufe (echte exe, eigene Kopie je Lauf)
Gemeinsame Umgebung (`J39abn2/lauf.sh`, wie Abnahme 1): RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_PSELECT_AUTO(_SWITCH)=1
RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_EVT_TRACE=1 RE15_BGM_CTL_DEBUG=1 RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw
RE15_R39J_LOG=r39.txt RE15_STIMME_LOG=1 RE15_R39_GEGNER_LOG=1 RE15_WEGPLAN=<plan> RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78
(Nach-1021-Flags wie der Riegel) RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0 RE15_FRAMEDUMP=<bereich>. Danach nur echte
Tueren (1131 Schluessel + Tuer -> 1141 -> Dachtuer -> 1171). Bildnummern = Bilder seit Raumeintritt.

| Lauf | Wegplan / Umgebung | Ergebnis |
|---|---|---|
| A0 | `plan_a.txt` (= bytegleich der Wegplan von integration_r39_dining1141_d), RE15_GIVE=0x43:1 (Messer), EXIT 700#1141, FD 0-700/5 | rv=0, 154 s |
| A | `plan_a.txt`, RE15_GIVE=0x43:1, RE15_R38F_GORILLAS_TOT=600 + _STARR=1, EXIT 300#1171, FD 0-6200/5 | rv=0, 383 s, 1131 -> 1141 -> 1171 |
| P | wie A, RE15_GIVE=0x43:1,0x03:15,0x15:50 (Browning im Inventar, nicht ausgeruestet), FD 1500-6200/5 | rv=0, 378 s, 1131 -> 1141 -> 1171 |
| L3 | `plan_l3.txt` (Elza nach der Szene neben John), Pistole, Gorillas LEBEND, RE15_R38F_GORILLAS_TOT=2400, EXIT 5500#1141 | Elza stirbt F5421 (ohne Energie-Messhaken), exe im Todesbild, eigene Kopie per PID beendet; Daten bis F5421 |
| L | `plan_a.txt`, Pistole, LEBEND, TOT=2400, RE15_R39_MESS_SPIELER_HP=100 (wie Riegel `kampf`), EXIT 5500#1141 | rv=0, 324 s |
| L2 | `plan_l2.txt` (Elza im Gang (-7350,-9000)), sonst wie L, EXIT 5800#1141 | rv=0, 338 s |
| M | wie L, `plan_m.txt` (= plan_a + WARTE 150: msg 41 waehrend Johns Zielen ausloesen), EXIT 4300#1141 | rv=0, 274 s; msg 41 kam nicht zustande (Elza von den Gorillas aufgehalten) - nicht verwertbar |
| R | Wegplan = integration_r39_gegner_1141 bis zum Riegel (`plan_r.txt`), RE15_GIVE=0x43:1,0x04:99,0x15:60 RE15_EQUIP=4 (P228), RE15_R39_MESS_SPIELER_HP=100, RE15_INPUT_SCRIPT = derselbe Schuetze wie dort (R1 halten, 6 x Feuer, 80 Bloecke, `skript_r.txt`) ab Bild 3500, **ohne** RE15_R38F_GORILLAS_TOT (echter Tod), EXIT 11000#1141, FD 3400-11000/10 | rv=0, 506 s |
| R2 | wie R, EXIT 6700#1141, FD 6280-6340/1 (jedes Bild am Schnitt auf Cut 8) | rv=0, 355 s; bildgleich zu R (Cut_chg(8) F6296, state.txt F6292..F6310 identisch) |

Takt-Bezug Lauf A/P: Takt = F + 3666 (`F35 tick=3701`); Lauf R: F + 3668.

### Punkt 1 — keine Zombie-Geraeusche
- debug.log Abschnitt 1141, alle Skript-Toene (`SCD Se_on: bank=`), Lauf A: 18x bank 62 Satz 0 (Klopfen), Hilferufe Satz 49..53
  (help01..05, help03 zweimal), 2x Satz 33 (Glas), bank 59 Satz 10/11/12 (Rolladen), 2x2 bank 14 (DOOR1A). **0 Zeilen**
  `bank=0x3C` / `bank=0x3D` / `bank=60 ` / `bank=61 ` / `main42`. Lauf A0 (F1..F700): nur bank 62 (9 Schlaege, 3 Rufe) und MAIN16;
  alle 9 `[se] Stimme`-Zeilen = vag 1, 44100 Hz (Klopfton). 1131 davor: 0 Skript-Toene. Unit `phase_h`: "kein Se_on 0x3C/0x3D ...".
- **Urteil: erfuellt.**

### Punkt 2 — John durch die Tuer; Kamera vorne auf Elza, Tuer-zu-Ton, dann Kamera auf die Tuer mit John
- Lauf A (Messer) und P (Pistole) bildgleich: Riegel F1485 (`Dachtuer entriegelt`), msg 12 F1487..F1529, Elza rennt (Plc_dest RUN
  F1530, Kamera-Automatik Cut 6 -> 5), `[scd F1593] Cut_chg(7)` (angezeigt ab F1594), Elza dreht zur Dachtuer (rot 3791 -> 3101) und
  hebt ab F1629/F1630 die Waffe; `Se_on bank=14 id=0` (Tuer auf) zwischen fd_001665 und fd_001670; John erstmals nicht geparkt
  **F1670 bei angezeigtem Cut 7** (-7350,-1300) = hinter der Kamera; `Se_on bank=14 id=1` (Tuer zu) zwischen fd_001710 und
  fd_001715; `[scd F1739] Cut_chg(6)` + MAIN16-Stopp: erstes Bild Cut 6 F1740 mit John (-7350,-1300) vor der Tuer; F1760 Cut 5,
  John laeuft heraus. 0 Bilder mit sichtbarem John vor dem Auftritt.
- Mitschnitt A: Tuer-auf Takt 5338..5346 (Spitze 8752..9044 ueber MAIN16 ~1000 RMS), Tuer-zu Takt 5378..5388 (Spitze 8103..8890),
  MAIN16-Stopp op=2 Takt 5405 -> Takt 5420 RMS 53.
- Bild `J39abn2/a_tuer.png` (F1580 Cut 5 Elza rennt; F1595..F1730 Cut 7 Elza von vorn; F1740/F1750 Cut 6 John vor der Dachtuer;
  F1760..F1840 Cut 5 John kommt zu Elza). Integration D (Abschnitt 3) bildgleich: "John erstmals im Raum: F1670 anz=7".
- **Urteil: erfuellt.**

### Punkt 3 — kein "shout"-Voiceover fuer John
- Lauf A/P/A0: 0 x `main42.wav`, 0 x bank 0x3D; Johns Stimmen in Phase H nur help01..05 (`[voice] clip loaded: help0N.wav`).
- **Urteil: erfuellt.**

### Punkt 4 — help01..05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen
- Lauf A0/A: je Runde drei Schlaege im Abstand 5 Bilder (F35/F40/F45, F255/F260/F265, F475/F480/F485 ...), 30 Bilder danach ein
  Hilferuf, Reihenfolge help03 (F75), help01 (F295), help04 (F515), help02, help05, help03 ... (PORT-WAHL Mix), Rundenabstand 220
  Bilder (seit Nachtrag 10 ohne die Reaktion); Lauf A: 18 Schlaege, 6 Rufe, nach dem Riegel F1485 keiner mehr.
- Mitschnitt A0: Schlaege Takt 3702/3708/3712 RMS 1761/1862/1774, Spitze ~8000 ueber MAIN16 (RMS 250..300) - hoerbar.
- **Urteil: erfuellt** (Klang = RE2 DOOR0F-Holzschlag; RE2 hat keinen Tuer-Klopfton - Klangabnahme beim Nutzer, H1).

### Punkt 5 — Elzas Reaktion; massgeblich jetzt Nutzer-Nachtrag 10 ("Im ROOM 1141 soll alles so bleiben wie bisher, nur eben ohne die 'Oh, no, this is John! ...' Cutscene"; der Satz zieht nach ROOM1041 = Spur P)
- Lauf A0 (r39.txt F1..F700): **0 Bilder mit Pad-Sperre**, b94 immer 0, John die ganze Zeit geparkt (-30000,-30000); einzige Nachricht
  = msg 13 (Gemaelde) ab F676. debug.log Abschnitt 1141 (A0, A, P, R): kein `[msg] room=1141 id=50`, kein `clip loaded: main50.wav`;
  main50.wav im Mitschnitt A Takt 3600..4400: rho 0,063 (nicht gespielt). Bild `J39abn2/a0_frueh.png` (F50..F600 Elza an der
  Doppeltuer ohne Textfenster, F680 Gemaelde-Text). Alles andere unveraendert (Punkte 1-4, 6-9 hier).
- **Urteil: erfuellt** (in der Fassung von Nachtrag 10; die Reaktion selbst ist Spur P).

### Punkt 6 — Glasbruch vor "What was that?!"
- Lauf A/P: msg 5 (John, main05.wav) offen F2224..F2423; Warte-Zeile msg 58; `GLAS1090 Glasbruch` F2424 (Takt 6090) und F2429
  (Takt 6095); msg 6 `"Elza: What was that?!"` ab F2450.
- **Urteil: erfuellt.**

### Punkt 7 — John schaut ab "...some kind of gorilla..." zu Elza
- Lauf A/P/R (r39.txt `blick` = Abweichung Johns Blick zur Peilung auf Elza, 4096 = 360 Grad): msg 9 169 Bilder |blick| <= 10,
  msg 10 99 Bilder |blick| <= 10; msg 11 147 Bilder, davon die letzten 12 > 160 = John laeuft in seine Ecke (sub02 @0x00F8E).
- **Urteil: erfuellt.**

### Punkt 8 — nach dem Tod der Gorillas zwei Zeilen Johns "mit passenden Animationen" (+ Nachtraege 7, 8)
- Lauf A/P (Messhaken F3833): Cut 8 ab F3864 + Pad-Sperre; msg 51 `"John: Puuuh, that was close! I thought we end up as gorilla
  meal...."` F3886..F4060 (175 Bilder), msg 52 `"John: Elza, follow me, I think I might have found a way out of here!"` F4062..F4190
  (129 Bilder); John laeuft erst **F4192** los (0 Laufbilder mit offener msg 52; Abnahme-1-Stand 29).
- Johns Sub/Clip: msg 51 (0,15) 100 + (0,23) 75 Bilder (Gesten wie zu msg 1, sub02 @0x00CB8..@0x00CCC); msg 52 alle 129 Bilder (0,2) =
  Heranwinken, `F4061 John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)`; 0 Bilder Sub 2 Clip 6 (Waffe auf
  Elza); Blick msg 51/52 in allen 304 Bildern 0; groesster Sprung Johns ohne Schnitt/Lauf 0.
- Stimmen (Nachtrag 8, Mitschnitt A): main51.wav rho **1,000** bei Takt 7551 (= F3885, Zeile ab F3886; Haelften 1,000/1,000, Median
  0,008), main52.wav rho **1,000** bei Takt 7727 (= F4061). Gegenprobe main52 im Fenster von msg 51: 0,047. msg 12/13:
  `[voice] ROOM1141 msg 12/13: keine Aufnahme ... - Zeile bleibt stumm`; 0 x main12/13/14.wav.
- Bilder `J39abn2/a_gespraech.png` (F3865 Cut 8 Elza links / John rechts einander zugewandt, Texte, F4195 Cut 7 John laeuft los,
  F4335/F4400 Cut 0) und `a_wink_zoom.png` (F4085..F4135 John winkt mehrmals heran; ab ~F4150 haelt er das Endbild des 96-Bilder-
  Clips, waehrend main52 noch ~1 s laeuft).
- **Echter Tod (Lauf R/R2, Abnahme-1-Hinweis H3):** gegner_r39.log INIT 4 x hp=360, 60 Treffer-Zeilen, Gorilla A tot F5682, B tot
  F6195 (toedliche Treffer von Elza), (4,0xF2)=0 F6267; John 14 Schuesse / 14 Treffer (7 Salven, ~12,4 s). Szene: Cut 8 ab F6297,
  msg 51 F6329, msg 52 F6505 mit (0,2) in allen Bildern, Blick 0, John laeuft F6635 los (= Ende main52.wav), (9,67)=1, Kamera
  F6777 Cut 0. Die Szene laeuft also auch mit 360 Energie und echtem Tod.
- **Befund M1 (Lauf R, wiederholt in R2 bildgleich): Elza zielt zu Beginn des Gespraechs mit der Pistole auf John.** state.txt:
  Elza bis F6295 im Waffenzustand (pst=1, mo=215, pad 0800 = R1 gehalten, wie ein Spieler, der beim Tod des letzten Gorillas noch
  zielt); der Schnitt `[scd F6296] Cut_chg(8)` setzt sie per Pos_set (-5800,-21500) + Member_set(4,2048) mit Blick auf John, der
  Waffenzustand bleibt aber **F6296..F6306 (11 Bilder)**; erst Plc_dest TURN F6307 (pst=4) beendet ihn. Bilder `J39abn2/r2_cut8.png`
  (F6297..F6306: ausgestreckter Arm, Pistole auf Brusthoehe direkt auf John; F6307/F6308 senkt sie; F6310 Waffe unten) und
  `J39abn2/r2_f6302.png` (Vergroesserung F6302 gegen F6312). In A/P (Wegplan haelt R1 nicht) tritt es nicht auf.
- **Urteil: teilweise** - Zeilen, Gesten, Heranwinken, Blick, Stimmen und Ausreden erfuellt; der Szenenanfang zeigt aber, sobald
  Elza beim Schnitt noch zielt, eine auf John gerichtete Waffe (Mangel M1) - keine "passende Animation" (vgl. Nachtrag 4: "nicht die
  Waffe auf Elza richten"; Praezedenzfall Abnahme 0 M3 = sichtbarer Fehler im Schnittbild von Cut 8).

### Nachbesserung 2 — Kamera nach Johns Abgang
- Lauf A/P nach (9,67)=1: F4319 Cut 7, **F4334 Cut 0** (Elza (-5800,-21500)), F5924 Cut 5 (Elza (-6085,-11904)), F6087 Cut 6 (Elza an
  der Dachtuer). Lauf R: F6762 Cut 7 -> F6777 Cut 0. Vorher (Dossier 9.1, Lauf v_a) blieb Cut 8 bis zur Dachtuer stehen.
- **erfuellt** (Kamera folgt Elza wieder).

### Punkt 9 — keine Panik-Musik beim Wechsel 1141 -> 1171
- Werkzeug des Riegels: `test_r39_dining1141 pegel l_a` -> `ROOM1171 main00 Sce_bgm_control op=1 bei Takt 9950`, `Takte 9980..10005
  Pegel 0, Takte 10020..10050 Pegel 559`, `pegel: 0 Fehler` (Lauf P identisch).
- Eigene Pegel (`cap.py l_a/cap.raw 9880 10100 4`): Stille (RMS 0,7) bis Takt 9944, Tuer-Schliesston 9948..9972 (RMS 1356 -> 36),
  Stille 9976..10004, ab 10008 steigt MAIN32 (51 -> ~1000). Kein MAIN16 (in 1141 per op=2 Takt 7499 gestoppt).
- Mechanismus: audio_pc.c `if (!s_bgm_pending)` vor dem Lautstaerke-Switch in ss_seq_ctl_ex (seit Abnahme 0 unveraendert);
  Original-Reihenfolge selbst gelesen (Abschnitt 2, Zeile 5); Gegenprobe ohne den Haken: Abnahme 0 (Pegel 496 = MAIN16) gilt weiter.
- **Urteil: erfuellt.**

### Nachtraege 4, 6, 7 (Wiederholung am Endstand)
- N4.1 Lauf A: Gemaelde-Frage + Rolladen unter Pad-Sperre F867..F1100, danach frei F1101..F1484 (384 Bilder; Elza geht durch die
  Rolladen-Oeffnung zur Dachtuer, Klopfen/Rufe laufen weiter), Riegel F1485 bei Elza z -1060. ROLLTOR39 setzt (3,0x6A)=1 und schaltet
  den Gemaelde-Slot ab (`46 03 00 00 ..` = sub06 @0x00FFA) - kein zweites Hochfahren. **erfuellt.**
- N4.2 Lauf A: `Elza hebt Waffe 1 (vorher 1, Messer)`, W-Clip 13 (DRAW) ab F1630 bei Cut 7, John noch geparkt, Elza z -12652
  (`a_messer_zoom.png`); Lauf P: `Elza hebt Waffe 3 (vorher 1, Schusswaffe)`, W-Clip 6 F1630..F1805 (`p_heben_zoom.png`: Pistole
  beidhaendig auf die Tuer hinter der Kamera, gehalten bis Cut 6, gesenkt F1759). **erfuellt.**
- N4.3 (John schiesst ab und an und erhaelt die Aufmerksamkeit), Laeufe L/L2/L3 (`salven.py`, `aufmerk.py`, debug.log):

| Lauf | Lage Elza | Salven | Schuesse / Treffer | "John zugewandt" | Annaeherung je Episode (Abstand zu John, dx+dz) | John im Bild |
|---|---|---|---|---|---|---|
| L | Doppeltuer/Raummitte | 5 | 10 / 10 (2 bei gr=1) | 5 von 5 | 5878->2797, 4086->2522, 9268->2525, 8153->2529, 9289->2536 | Salven 1-2 Cut 0 |
| L2 | Gang | 6 | 12 / 12 | 6 von 6 | 6576->2793, 9542->2533, 9394->2579, 9387->2531, 9388->2529, 9393->6368 (Laufende) | nein (Cut 5) |
| L3 | neben John | 5 | 10 / 10 (6 bei gr=1) | 5 von 5 | 3729->3373, 4388->2898, 3426->3340, 3436->3462, 3402->3403 (Elza + Greifer im Weg) | Cut 0, meist verdeckt |

  Abnahme 1 (16 Salven: 6 ohne Treffer, 4 nur Zucken) -> jetzt 16 Salven, 32/32 Treffer, 16/16 mit Zuwendung; (a) Treffer bei Elza im
  Griff (8), (b) 0 Schuesse ausser Kegel (rel -7..+4), (c) Zuwendung messbar (L3 Slot 3: rel -538 F3655 -> ~0 F3680) und in L/L2 bis
  an John heran. Bilder `l_john_zoom.png` (L F4030..F4065: John neben der Pflanze zielt beidhaendig und schiesst), `l2_salve2.png`
  (L2: ein Gorilla dreht ab, laeuft an der Kamera vorbei zu John, kommt zurueck). L2 bildgleich zu f_l2 des Dossiers. **erfuellt.**
- N4.4 / Nachtrag 7: Heranwinken RBJ ROOM1171 rec1 Clip 2 (Punkt 8), 0 Bilder Waffe auf Elza. **erfuellt.**
- N6: `"... we end up as gorilla meal...."` (A/P/R), Riegel `texte` prueft "He is in trouble". **erfuellt.**

## 2. RE-Gate: Stichproben selbst disassembliert/gelesen (richtige Binaerdatei, Sprungziel)
`S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (PSX.EXE fuer 0x800x, STAGE1.BIN roh fuer 0x8010..0x8012).

| # | Zitat im Diff (Nachbesserung 2) | selbst gelesen | Befund |
|---|---|---|---|
| 1 | (b) Nachfuehren `RE15_R39J_NACHFUEHREN` 0xC8 = Gun-FSM Sub 5 `ori a1,zero,0xc8` @0x80033f90 / `jal 0x8001a8f8` @0x80033f94 (a0 = Ziel+0x34 @0x80033f98) | `dis 0x80033eec 60`: `80033f8c lw a0,0(s0)` (s0 = 0x800acbfc Ziel), `80033f90 ori a1,zero,0xc8`, `80033f94 jal 0x8001a8f8`, `80033f98 addiu a0,a0,52`; Sprungziel `dis 0x8001a8f8`: atan2_q12 (`8001a928 jal 0x8001a6d4`) zur Zielposition, dann +0x6a um hoechstens a1 drehen (`8001a958 lhu a2,106(v1)` ... `sh ..,106(v1)` @0x8001a984/@0x8001a988/@0x8001a9b0) | stimmt |
| 2 | (c) Zuwendung `RE15_R39J_ZUGEWANDT` 0x180 = Gorilla A[3] `a804(0xbb8,0x180)` @0x80117a60-74; B[3]-Lenkschritt `(rng & 0x1f) + 6` @0x80117d1c-34, `jal 0x8001a8f8` @0x80117d50 | STAGE1.BIN `dis 0x80117a50`: `80117a60 ori a0,zero,0xbb8`, `80117a64 ori a1,zero,0x180`, `80117a6c jal 0x8001a804`; Sprungziel `dis 0x8001a804`: SquareRoot0-Abstand `<= a0`, dann atan2 und Kegel `+-a1` (@0x8001a8b0..@0x8001a8d0, Bit 31 = im Kegel). `dis 0x80117d10`: `80117d1c jal 0x8001af20` (rng), `80117d24 andi v0,v0,0x1f`, `80117d30 addiu v0,v0,6`, `80117d34 sb v0,158(v1)`, a0 = playerX @0x80117d4c, `80117d50 jal 0x8001a8f8` | stimmt (0x180 ist der Biss-KEGEL des Originals; ihn als "John zugewandt" zu werten ist Teil der PORT-WAHL "Aufmerksamkeit") |
| 3 | (a) Wurf im Spieler-Takt: `jalr v0` @0x80031cb4, Ende P7 0x8011c384 (aca58 = 1, +0x93 = 0) | `dis 0x80031c80`: `80031c8c lbu v1,-13736` (aca58), `80031ca4 addiu at,at,16272` (0x80073f90), `80031cb4 jalr v0`; STAGE1.BIN `dis 0x8011c384`: `ori v0,zero,0x1` / `sw v0,-13736(at)` (aca58 = 1), `8011c3a0 sb zero,-13593(at)` (+0x93 = 0) | stimmt (Bindung 0x8011c118 = Kommando 5 steht im Bestand re15_affen.h (6d) mit GDB-Beleg; statisch ist 0x80073f90[5] = 0x80036834) |
| 4 | Kamera am Ende `RE15_R39J_CUT_ENDE` 0 = RVD @0x01A8 Satz 0 / Satz 22; Form sub02 @0x00E80 `29 00` | ROOM1141.RDT (shared_assets == info/Re1.5, `cmp` bytegleich) RDT+0x28 -> 0x1A8, 20 B je Satz: Satz 0 `01 ff 00 00 fc d6 64 9c fc d6 cc de b8 0b cc de b8 0b 64 9c` = Gruppe Cut 0, Viereck x -10500..3000, z -25500..-8500 (enthaelt (-5800,-21500)); Satz 22 `01 ff 08 00 ..` einziger Satz der Gruppe Cut 8 (Satz 23 = Gruppe 9, Satz 24 = Ende `ff ff`); @0x00E80 `29 00`, @0x00F56 `3c 01` | stimmt |
| 5 | Punkt 9 (audio_pc.c, seit Abnahme 0 unveraendert): Warten @0x8004429c-@0x800442b0, MAIN laden @0x800442b8, Raumlader @0x800399b0 vor SCD-INIT @0x80039a00 | `dis 0x80044278`: `8004428c lbu v0,21016(v0)` (0x800b5218), `8004429c jal 0x80029ac8` / `800442a0 ori a0,zero,0x1`, `800442b0 bne v0,zero,0x8004429c`, `800442b8 jal 0x80044564`; `800399b0 jal 0x80044210`, `80039a00 jal 0x8003ef6c` | stimmt |

Suche in den `+`-Zeilen von `git diff master...HEAD -- re15_port` nach `deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|
vorerst|geschaetzt|approx|ungefaehr|platzhalter|placeholder`: **0 Treffer**. Neue getenv: nur `RE15_R39J_LOG` (Messschiene) und
`RE15_SE_DEBUG` (Log); keine Env-Schalter als Abschluss. Konstanten der Nachbesserung 2: 0xC8 (@0x80033f90), 0x180 (@0x80117a64),
Cut 0 (RVD @0x01A8), msg 58 (Port-Warte-Zeile in der Form `2b 05 00 00` sub02 @0x00D9E); Ziel-Regel/Takt/Dauer/NAH = PORT-WAHL
(gekennzeichnet in re15_dining_r39.h). Fix erklaert Befund: (a)/(b)/(c) der Abnahme 1 an L/L2/L3 gemessen (Tabelle oben),
Ausreden (0 Laufbilder mit msg 52), Kamera (Cut 0 statt 8), Haken-Umzug (Treffer im selben Bild; integration_r39_gegner_1141 gruen).
**RE-Gate gehalten.**

## 3. Gezielte Tests (keine volle Suite im Spurbaum)
- Unit (Module des Diffs: dining_r38/39, Kartenbits, Gorilla-Hirn enemy_ai_common.c, RBJ-Kanal enemy_common.c, Schaden re15_damage.c,
  scd_vm.c (Plc_motion/Message_on/VM-Takt), Spieler-/NPC-/Anim-Pfad, Ton/Stimme/BGM audio_pc.c, Nachrichten-FSM, Heliport 1171,
  Gegner/Treffer/Elza/Inventar/Toene/Funk der Runde 39, Pausemasken/Einfrieren wegen des umgesetzten game_step-Hakens), Regex
  `J39abn2/regex_unit.txt` (= Regex der Abnahme 1 + `unit_text_freeze|unit_scd_zone2_pauseflags|unit_r38_zopf_freeze|unit_r39_funk10f1|
  unit_r39_toene|unit_fenster1120|unit_r35_fenster|unit_r39_inventar`):
  `ctest --test-dir re15_port/build -R "<regex>" -j 6 --timeout 600` -> **100% tests passed, 0 tests failed out of 192** (47,6 s).
- `test_r39_dining1141 {texte,programme,stamm,phase_h,fenster}` einzeln: je `0 Fehler` (`J39abn2/u_*.txt`).
- Integration (Spur-Riegel, alle R38-dining-Teile, BGM-Wechsel 10F0, Weg nach 1171, Stimmen-Pfad, Elza in 1141, Elza-Schuss,
  Gorilla-Riegel R38 und E-1141), Regex `J39abn2/regex_integ.txt`:
  `integration_r39_dining1141|integration_r38_dining1141|integration_r35_cut10f0|integration_r38_heliport1171_weg_re15|integration_r39_stimmen_1171|integration_r38_rekorder|integration_r38_elzaintro_stimme|integration_r37_elzaems_elza1141|integration_r36_elza_schuss|integration_r38_affen3|integration_r39_gegner_1141`
  `-j 4 --timeout 3600` -> **100% tests passed, 0 tests failed out of 20** (1049 s; darunter integration_r39_dining1141_d 402,8 s,
  _kampf 331,5 s, r39_gegner_1141 361,0 s, r38_dining1141_s 734,7 s). Auswertung per -DAUSWERTEN: kampf "11 Schuesse, 11 Treffer,
  0 ausser Kegel", "6 von 6 Treffer-Salven mit sichtbarer Zuwendung"; D "John erstmals im Raum: F1670 anz=7", "0 von 268 Bildern",
  "0 Laufbilder von 129 mit msg 52", pegel "Takte 9814..9839 Pegel 0, 9854..9884 Pegel 559".
- Kein Riegel deckt den Schnitt auf Cut 8 mit zielender Elza ab (M1).

## 4. Vertrag (VERTRAG.md Runde 39)
- Pfade (`git diff master...HEAD --name-only`, 27 Dateien): `analysis/befunde_runde39/{AUFTRAG.md (nur Orchestrator-Commits c4e692ac,
  4d7c7b34, 2f889c9f, 18f4306a, f60715a8, b6f25fdf, 363626f4, 3f0ef678 + Merge 25611933), J_abnahme_0/1.md, J_dining1141.md}` und
  `re15_port/{engine/src, engine/src/gen, include, platform/pc, platform/pc/src, tests/unit, tests/unit/probes, tests/integration,
  tools/r39_dining}`. Kein `release/`, `platform/android/`, `shared_assets/PSX/`, `synchro/`, keine Test-CMakeLists (Riegel ueber
  `tests/unit/probes/r39_dining1141.cmake`).
- Bank 9: Bit 94 (k_port[] +1 Zeile; seit Nachtrag 10 nicht mehr gesetzt, reserviert), Bit 95 ungenutzt. Bank 5 (Raum-Scratch):
  0x10/0x11 (Original-1141 nutzt Bit 0/1). Nachrichten-Ids 50 (Text bleibt, in 1141 nicht installiert), 51, 52, 53..57 (Sprach-Ids
  help01..05), 58 (Warte-Zeile) - alle in 50..59. Port-Bank 0x3E: `git grep` auf master ohne weiteren Nutzer.
- Gemeinsame Dateien, je Haken mit "Runde 39 Spur J" und auf ROOM1141 begrenzt: scd_vm.c (+7), platform/pc/main.c (+2), dining_r38.c,
  dining_r38_pc.c (+1), test_r36_kartenbits.c (+2) - zugeteilt; ausserhalb der Zuteilung 1.3: audio_pc.c +5 (A/B; Punkt 9 wirkt in
  allen Raeumen, nur waehrend `s_bgm_pending`), enemy_ai_common.c +7/-2 (D/E), enemy_common.c +4, game_step_common.c +2,
  re15_damage.c +8 - klein und offengelegt (Dossier 9.7), H2.
- Dateien der Spur E im Zweig nachgezogen (test_r39_gegner_1141.c/.cmake, Kommentar "Runde 39 Spur J"): Merge-Folge, offengelegt - H3.
- Riegel parallel-/Linux-fest: eigene exe-Kopie `re15_pc_r39j_d_<id>` / `re15_pc_r39j_k_<id>` + eigener Laufordner,
  `cmake_minimum_required` in beiden -P-Skripten, `m` fuer Nicht-Windows, keine `.exe`-/Backslash-Pfade; fremde RE15_*-Variablen des
  Aufrufers werden nicht geloescht - H4.
- **Vertrag gehalten** (mit den Hinweisen H2-H4).

## 5. Maengel (nummeriert, nachpruefbar)
- **M1 (Punkt 8, niedrig - tritt nur auf, wenn Elza beim Schnitt noch zielt): Elza richtet zu Beginn des Gespraechs nach dem Kampf
  ihre Waffe auf John.** ENTRIEGELN39 (`nach_dem_kampf`, gen/dining_r39_programme.inc +0x0400..+0x0438) schneidet auf Cut 8 und setzt
  Elza per Pos_set + Member_set(4,2048) mit Blick zu John, waehrend der Spieler noch im Waffenzustand sein kann; erst Plc_dest TURN
  (+0x0438, nach Sleep 1 + Plc_neck + Sleep 10) beendet ihn. Gemessen Lauf R und R2 (bildgleich, echter Tod beider Gorillas mit
  360 Energie, Elza zielt mit gehaltenem R1 wie der Schuetze des Riegels integration_r39_gegner_1141): state.txt F6296..F6306 pst=1
  mo=215 bei Lage (-5800,-21500) rot 2048, F6307 pst=4; Bilder `J39abn2/r2_cut8.png` / `r2_f6302.png`: 11 Bilder lang zeigt die
  Pistole auf Brusthoehe direkt auf John. Der Schnitt kommt ~3,4 s nach dem toedlichen Treffer (Tod F6195, (4,0xF2)=0 F6267,
  Sleep 30) - wer bis dahin noch zielt (man wartet, bis der Gorilla liegt), sieht es. Nicht gemessen: ob andere Spielerzustaende
  beim Schnitt (z. B. am Boden nach einem Wurf) dasselbe zeigen - der neue Riegel sollte sie mit abdecken.
  Nachpruefen (scratchpad J39abn2): `bash lauf.sh r2 plan_r.txt "6700#1141" "6280-6340/1:fd_" RE15_GIVE=0x43:1,0x04:99,0x15:60
  RE15_EQUIP=4 RE15_R39_MESS_SPIELER_HP=100 "RE15_INPUT_SCRIPT=$(cat skript_r.txt)" RE15_INPUT_SCRIPT_BASIS=spiel
  RE15_INPUT_SCRIPT_START=3500` -> l_r2/state.txt F6296..F6306 `pst=1`, Bilder fd_006297..fd_006306.
  Abhilfe-Richtung: Elza vor dem ersten Bild von Cut 8 in einen neutralen Zustand bringen (Waffenzustand beenden, bevor Pos_set/
  Member_set greifen - mit belegter Original-Form) und einen Riegel mit zielender Elza beim Schnitt anlegen (kein Bild in Cut 8 mit
  pst=1 bzw. Waffen-Clip).

## 6. Hinweise (keine Maengel)
- H1 Klopf-Klang = RE2 DOOR0F-Holzschlag (RE2 hat keinen Tuer-Klopfton) - Klangabnahme beim Nutzer.
- H2 Gemeinsame Dateien ausserhalb VERTRAG 1.3 (Abschnitt 4) - bei der Integration mit A/B (audio_pc.c), D/E (enemy_ai_common.c) und
  K (enemy_common.c: K 18940e95 in rbj_resolve_slot, J davor in re15_actor_rbj_anim/_skel; `git merge-tree` ohne Code-Konflikt).
- H3 Die Riegel der Spur E (unit_r39_gegner_1141_ablauf1141, integration_r39_gegner_1141) tragen im Zweig den J-Ablauf (START 3500,
  EXIT 5240) - bei der Integration die Fassung des Zweigs nehmen, sonst rot (Bau-Agent "Offen").
- H4 integration_r39_dining1141_d/_kampf erben RE15_*-Variablen des Aufrufers (nur re15_start_spiel setzt die eigenen; kein
  `unset(ENV{..})` wie in integration_r39_gegner_1141) - VERTRAG 2 "keine Umgebungsvariablen des Aufrufers erben".
- H5 Code gelesen, nicht gemessen (Lauf M brachte msg 41 nicht waehrend eines Zielens zustande): der Haken `re15_dining_r39_tick()`
  steht in game_step_common.c VOR, aber ausserhalb des Tores `!(g_re15_pauseflags & RE15_PAUSE_AI)` (Dossier 9.12 / Commit
  6161ac63: "neben re15_fenster1120_tick" - der steht innerhalb des Tores). Bei einer die Welt einfrierenden Nachricht laufen Johns Nachfuehren (NB2 b),
  ein schon angestossener Schuss und die Aufmerksamkeits-Zaehler (NB2 c: Takt/"festgeklemmt") weiter. Auch die alte Stelle (Ende des
  Schritts) war ungetort.
- H6 Mit gedrueckter Aktionstaste verschwinden die Texte msg 51/52 vor dem Ende der Aufnahme (Lauf R: 110/108 statt 175/129
  Bilder), die Stimme laeuft weiter und msg 52 wartet auf sie - allgemeines Verhalten des Stimmen-Riegels, kein J-Code.
- H7 dining_r39.c re15_dining_r39_install: Kommentar "s_leihe_ok bleibt" gegen `s_leihe_ok = 0` in der Folgezeile (nur Kommentar).

## 7. Urteil

| Punkt (Wortlaut AUFTRAG.md) | Urteil | Kurzbeleg |
|---|---|---|
| 1 keine Zombie-Geraeusche | erfuellt | A/A0/P/R: 0x bank 0x3C/0x3D/main42; nur Klopfen/help/Glas/Rolladen/DOOR1A (+ Gorilla-Stimmen im Kampf) |
| 2 John durch die Tuer; Kamera Elza -> Tuer-zu-Ton -> Kamera Tuer mit John | erfuellt | A/P: Cut 7 F1594, Tuer auf ~F1667, John F1670 hinter der Kamera, Tuer zu ~F1712, Cut 6 F1740 mit John (`a_tuer.png`) |
| 3 kein "shout" | erfuellt | 0x main42 / 0x3D |
| 4 help01..05 im Mix + mehrfaches Klopfen | erfuellt | 3 Schlaege je Runde (Abstand 5), Rufe 3,1,4,2,5, hoerbar (RMS ~1800 ueber Musik ~280) |
| 5 Reaktion (Nachtrag 10: in 1141 entfaellt sie) | erfuellt | A0: 0 Bilder Pad-Sperre, erste Nachricht msg 13, kein main50 |
| 6 Glasbruch vor "What was that?!" | erfuellt | F2424/F2429 direkt nach msg 5, msg 6 F2450 |
| 7 John schaut ab msg 9 zu Elza | erfuellt | msg 9/10 Blickfehler <= 10 in 268 Bildern |
| 8 zwei Zeilen Johns mit passenden Animationen | **teilweise** | Zeilen/Gesten/Heranwinken/Stimmen/Ausreden ok (A/P/R); Szenenanfang mit auf John gerichteter Waffe Elzas, wenn sie beim Schnitt zielt (R/R2 F6297..F6306, M1) |
| 9 keine Panik-Musik 1141 -> 1171 | erfuellt | pegel 0 / MAIN32 559; eigene Pegel ohne MAIN16 |
| N4.1 Rolltor oeffnen, danach normal laufen | erfuellt | 384 freie Bilder, Klopfen weiter, kein zweites Hochfahren |
| N4.2 Szene am Riegel; rennen, andere Kamera, zur Tuer drehen, Waffe heben | erfuellt | A Messer-DRAW, P Pistole aus dem Inventar |
| N4.3 John schiesst ab und an, erhaelt die Aufmerksamkeit | erfuellt | L/L2/L3: 16 Salven, 32/32 Treffer, 16/16 Zuwendung, in L/L2 Annaeherung bis an John |
| N4.4 / Nachtrag 7 Heranwinken wie 1171 | erfuellt | (0,2) RBJ ROOM1171 rec1 Clip 2, 0 Bilder Waffe auf Elza |
| N6 "trouble"/"gorilla" klein | erfuellt | msgtext |
| Nachtrag 8 Stimmen main50/51/52 | erfuellt | main51/52 rho 1,000 an ihren Zeilen; 12/13 stumm; main50 in 1141 nicht (Nachtrag 10) |
| NB2 Kamera nach Johns Abgang | erfuellt | Cut 0 -> 5 -> 6 statt stehender Cut 8 |

Gates: RE-Gate gehalten (5 Stichproben selbst disassembliert/gelesen, alle stimmen; keine Rate-Woerter; PORT-WAHL/NUTZER-VORGABE
gekennzeichnet). Vertrag gehalten (Hinweise H2-H4). Tests: Unit 192/192, Integration 20/20 gruen.
**bestanden = false** (Punkt 8 teilweise: Mangel M1, niedrig, aber sichtbar und auf dem echten Weg mit echtem Kampf reproduzierbar).
