# Runde 39 Spur Q "elzazombies" — Abnahme 2 (unabhaengig, nach Nachbesserung 2)

Stand: ABSCHLUSS. ERGEBNIS: NICHT BESTANDEN - 2 Maengel (Folgezustand des neuen Leon-Treppenhaus-Schritts; Haltepose bei Ankunft von oben), alle uebrigen Nutzer-Punkte selbst gemessen erfuellt.
Baum `.claude/worktrees/r39_elzazombies`, Zweig `r39/elzazombies`, HEAD e5500cea, Merge-Basis c4675474 (master ist seither
nur um Doku-Commits weiter: 089b6817/ca49f1e2 Nutzer-Nachtrag 15 fuer Q, 017f5094 J). Gebaut 2026-10-19 mit `bash re15_port/tools/local_build.sh configure`
(exit 0, "LOCAL-BUILD-OK (configure)") + `build` (exit 0, "ninja: no work to do") — re15_pc.exe 09:08:24 (7697079 B).
Diff gegen die Merge-Basis c4675474 (`git diff --numstat`): elzazombies_r39.c +256/-53, irons_tod_1150.c +15/-2,
re15_elzazombies_r39.h +59/-11, re15_irons_tod.h +4/-1, Tests (test_r39_elzazombies.c/.cmake, probes/r39_elzazombies.cmake, test_r35_cut1150.c, test_r36_szene1150.c/.cmake,
probe_r35_cut1150_karte.c), Dossier + 2 Belegbilder.

## Nutzer-Punkte (AUFTRAG.md, woertlich)
- P1 (Nachtrag 10): "Und auch hier sollen jetzt Zombies durchbrechen - einmal in den Flur hinein von 10C1 aus - so viele wie im
  medical room - also 10E1 am Leben gelassen wurden., die sollen dann, wenn man in ROOM 1061 die Etage von unten über die Treppe
  erreicht aber möglichst weit weg an der Seite stehen. Dann das gleiche nochmal wie bei Leon in den ROOM 1071, falls die Zombies
  nicht besiegt wurden und im ROOM 1031 über die Absperrung/Das Rollo, so wie bei der cutscene."
- P2 (Nachtrag 11): "Ich korrigiere, die Zombies sollen nicht in ROM 1071 noch durchbrechen, sondern dann noch wie bei leon im ROOM 1041."
- P3 (Nachtrag 12, Teil Q — im Relais des Nutzers dieser Runde enthalten): "Außerdem habe ich mich entschlossen eine Angleichung
  zu machen der hereinbrechenden Zombies. Bei Irons Tod und Johns Hilferuf sollen die Zombies in beiden Szenarien aus den gleichen
  Türen brechen. Also bei Leon zusätzlich das Treppenhaus und bei Elza zusätzlich ROOM 1071."
- Orchestrator zu H1 (Abnahme 1): Rolltor-Durchbruch 1041 nur in der Panikphase ((9,97)=1).
- P4 (Nachtrag 15, AUFTRAG.md in master seit 089b6817/ca49f1e2, 09:51): "In ROOM 1131 dürfen bei Elza natürlich keine Zombies
  durchbrechen, der Raum 1141 ist bei ihr ja abgeschlossen." Nachschub: "Nur die Zombies von ROOM 1071 kommen bei Elza noch hinzu."
  (Orchestrator: "Abnahme: in Elzas Panikphase KEIN Durchbruch und kein Zombie-Spawn in ROOM1131.")

## Messprotokoll

Werkzeug: eigenes Lauf-Skript `qa2/lauf.sh` (Karte mit `probe_r39_elzazombies_karte` schreiben, Messkopie
`re15_pc_qa2_<lauf>.exe` in build/platform/pc, CONTINUE ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0,
RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1, Kopie danach geloescht), Laufordner `qa2/runs/<lauf>`, Bildtafeln `qa2/*.png`
(`qa2` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/qa2`).
Shell ohne RE15_*-Variable (`env | grep -i ^re15_` leer). Spur P ist in diesem Baum nicht enthalten: (9,97) setzt
`RE15_SET_FLAG_AT=9:97@200` (= P's Setzpunkt in 1041). EIGENE Ueberlebenden-Zahlen, verschieden von denen der Riegel
(Riegel: 10E1 4/0/2/0, 1071 5/5/3/0; Leon 10E0 4/2/0).

### M1 Elza-Kette, 10E1: 3 leben (0xe0 tot), 1071: 2 leben (0xc6..0xc8 tot) — P1, P3
Laeufe e1_voll (Ende 400#1041), e1_1061 (FRAMEDUMP 10-170/40, Ende 172#1061), e1_1031 (FRAMEDUMP 10-530/40, Ende 532#1031);
Karte `1041;-21008;0;-13134;2112;0;3:125;3:193;3:116;7:224;7:198;7:199;7:200`, `RE15_SET_FLAG_AT=9:97@200`. debug.log (e1_voll):
```
[setflag-at] Frame 200: flag(9,97/0x61) = 1
[elzazombies-r39] Flanke (9,97) 0->1 in ROOM1041 Takt 203 -> Montage ausstehend
[elzazombies-r39] Umzug: 10E1 lebend 3 -> 1061; 1071 lebend 2 -> 1031 Cut 7; (9,95)=1
Folge #1 K ROOM1041 T61 0                     (Tuerknall Satz 0)
[aot] DOOR FIRE slot=20 ... target_cut=2 spawn=(20200,-7200,20400) ; [room] PC loaded room1061.rdt
Folge #3 S ROOM1061 T105 3 ; Folge #4 C ROOM1061 T105 2           <- DREI Kopien = drei Ueberlebende in 10E1
Folge #5 K ROOM1061 T237 0                    (Tuerknall Satz 0 = die 1071-Tuer bricht)
[aot] DOOR FIRE slot=20 ... target_cut=7 spawn=(-28558,0,-2183) ; [room] PC loaded room1031.rdt
ROOM1061 Cut 2 Bild 176: Bit 125 6421 -> 4846, Bit 126 6638 -> 5467, Bit 127 7547 -> 5585 ; Spieler-HP min 100
Folge #7 S ROOM1031 T281 2 ; Folge #8 C ROOM1031 T281 7            <- ZWEI Kopien = zwei Ueberlebende in 1071
Folge #9 K ROOM1031 T457 2 (Original-Se_on @0x02862) ; #10 S ROOM1031 T467 3 (Kriecher) ; #11 C ROOM1031 T489 12
ROOM1031 Cut 7 Bild 207: Bit 75 7222 -> 2005, Bit 76 5599 -> 519 (Kopien von 0xc9/0xca) ; Raum-Zombies Bit 0..5 hinter dem
                         Rollo fast unbewegt (Baustein 5) ; Spieler-HP min 100
ROOM1031 Cut 12 Bild 335: Kriecher 133/144/145 9827 -> 5905, 10558 -> 5675, 9560 -> 5164 ; Raum-Zombies 12906 -> 9124 ..;
                          1071-Kopien hinter der Kamera angehalten (angehalten 1) ; Spieler-HP min 100
ROOM1041: Rueckkehr (Programm D) ; Rueckkehr beendet, Steuerung frei
```
Bilder `qa2/e1_1061.png` (Bild 10-170: am 2F-Absatz, Schild "F2", die Zombies an der 10C1-Tuer, sie gehen zum Gelaender auf die
Kamera zu) und `qa2/e1_1031.png` (Bild 10-210 Cut 7: zwei Polizei-Zombies im Gang vor der Lobby-Office-Tuer kommen bis dicht vor
die Kamera; ab Bild 250 Cut 12: das aufgebrochene Rollo, Kriecher kommen darunter durch, Zombies dahinter).

### M2 Elza-Kette, 10E1: 0 leben, 1071: 1 lebt (nur 0xca) — 1061-Schritt entfaellt, 1071-Schritt allein
Lauf e2_voll: `Umzug: 10E1 lebend 0 -> 1061; 1071 lebend 1 -> 1031 Cut 7` -> `K ROOM1041 T61 0` -> `DOOR FIRE ... target_cut=7` ->
ROOM1031 `S T105 1`, `C T105 7` -> `K ROOM1031 T281 2` -> `S T291 3` -> `C T313 12` -> ROOM1041 `Rueckkehr beendet, Steuerung frei`.
Cut 7: Bit 76 5599 -> 708, HP min 100; Cut 12 HP min 100.

### M3 Elza-Kette, 10E1: 2 leben (0xe0, 0xe2), 1071: 0 — wie vor Nachtrag 12 (kein Cut 7)
Lauf e3_voll: `Umzug: 10E1 lebend 2 -> 1061; 1071 lebend 0` -> 1061 `S 2`, `C 2` (Bit 124 7062 -> 5485, Bit 126 6638 -> 4861,
HP 100) -> `K ROOM1061 T237 1` (Knall Satz 1 wie ROOM1030) -> `DOOR FIRE ... target_cut=12` -> 1031 `S 3`, `C 12` -> 1041
`Rueckkehr beendet, Steuerung frei`.

### M4 Leon-Kette (echter Weg: Spielstand ROOM1130 vor Irons' Tuer, (9,71),(3,94); `W1,A0.2,W200`) — P3 Leon
* l1_voll (10E0: nur 0xe3 lebt; 1140/1070 alle leben; Ende 1500#1150):
```
[elzazombies-r39] Leon: Umzug 10E0 lebend 1 -> 1060 (Treppenhaus-Schritt nach 1130 bzw. nach der Szene)
[irons-tod] Umzug: 1140 lebend 5 -> 1130 (9,74)=1; 1070 lebend 5 -> 1030 (9,76)=1
Folge #1 K ROOM1150 T1284 Knall Satz 0 ; room1130: #2 S +5, #3 C 0 ; #4 K ROOM1130 T1460 Knall Satz 0   (Tuerknall der 10C0-Tuer)
[aot] DOOR FIRE slot=20 ... target_cut=2 spawn=(20200,-7200,20400) ; [room] PC loaded room1060.rdt
#5 S ROOM1060 T1504 Spawn + 1 ; #6 C ROOM1060 T1504 Kamera Cut 2 ; ROOM1060 Zustand 7: Montage 1060 (Runde 39 Spur Q, Programm L)
#7 K ROOM1060 T1636 Knall Satz 1 ; room1040: #8 S +5, #9 C 1 ; ROOM1060 Cut 2 Bild 176: Bit 127 7547 -> 4861, HP min 100
Bilanz 1040: 5 Zombies, durchs Tor 5, letzter bei Bild 270 von 405 ; #10 K ROOM1040 T2041 Satz 0 ; room1030 #11 S +11, #12 C 7,
#13 K 2, #14 S +3, #15 C 12 ; room11c0 #16 S +4, #17 C 13 ; room1150: Rueckkehr: Programm 5 ; Rueckkehr beendet, Steuerung frei ;
Bilanz Kette: Spieler-HP 100 -> min 100
```
  1040-Schritt T1680 -> T2041 = 361 Bilder (wie die Messung vorher des Dossiers 9.2).
* l2_1060 (1140 alle tot, 10E0: 3 leben (0xe1 tot); FRAMEDUMP 10-170/40 in 1060): `Umzug 10E0 lebend 3`, `1140 lebend 0 -> 1130
  (9,74)=0` -> Knall Satz 0 am Ende der Szene -> DIREKT `PC loaded room1060` (S +3, C 2) -> `K ROOM1060 T1460 Knall Satz 1`.
  Bild `qa2/l2_1060.png`: dieselbe Kamera wie Elzas 1061 (Schild F2), drei Polizei-Zombies an der 10C0-Tuer gehen zum Gelaender.
* l3_tot10e0 (10E0 alle tot): `Umzug 10E0 lebend 0 -> 1060 (Treppenhaus-Schritt entfaellt)` -> 1130 -> `K ROOM1130 T1460 Knall
  Satz 1` -> `DOOR FIRE ... target_cut=1` -> room1040 (S +5, C 1) = die alte Kette unveraendert.

### M5 Elza 1061 von unten (echter Weg: Tuer 1041 -> 1061, Treppe A + B), DREI Kopien (124, 126, 127 leben) — P1 Aufstellung
Lauf t1_elza_unten (Karte `1041;-20700;0;-13000;0;0;3:125;3:193;3:116;9:95;7:224..227;7:125;7:128..131`, Treppen-Skript des Riegels
treppe, FRAMEDUMP 460-620/40) und t1b_elza_unten (FRAMEDUMP 489-494/1):
```
[aot] DOOR FIRE slot=2 rect=(-19650,-13000,hw=750,hh=1100) target_cut=7 spawn=(26000,0,25300) ; PC loaded room1061.rdt
ROOM1061: Nachspawn 1061 (Programm E): Kopien an der Westseite, gehalten bis Etage 4
ROOM1061: Kopie Aktor 1 Bit 124 bei (20218,26532), Abstand zur Spielerin 5564
ROOM1061: Kopie Aktor 3 Bit 126 bei (21019,26532), Abstand zur Spielerin 4823
ROOM1061: Kopie Aktor 4 Bit 127 bei (20900,25350), Abstand zur Spielerin 4591
ROOM1061: Spielerin auf Etage 4 bei (25424,24568) -> Kopien frei, kleinster Abstand 4591
```
state.log: Kamera 3 -> 2 in F491 (Elza bei (25424,24568) = Treppenende), die Kopien sind in F491 noch ungestartet (st=0, hp=0),
ab F492 initialisiert (st=1, hp 80/94/95). Bild `qa2/t1_elza_unten.png`: Bild 500 Elza rechts an der 10C1-Tuer, die drei links
unter dem Fenster; danach kommen sie heran. `qa2/t1b_elza_491.png`: F490/F491 noch im Treppenschnitt gezeichnet, F492 Cut 2 mit
stehenden Zombies — von unten ist die Haltepose NIE zu sehen.

### M6 Elza Rolltor-Durchbruch 1041 (echter Weg: Tuer 1061 -> 1041), ZWEI Kopien (124, 125 leben) — P2 + H1-Entscheid
* d1_panik (mit (9,97)): `DOOR FIRE slot=2 ... spawn=(-21008,0,-13134)` -> `Durchbruch 1041: 2 Zombies aus 1061 (nicht besiegt) ->
  hinter das Rolltor` -> `S ROOM1041 T0 2`, `C ROOM1041 T0 1` -> `Durchbruch 1041 startet (Programm G) ... Rolltor faehrt hoch` ->
  `Signal (5,29): Knall wie ROOM1030` -> `Durchbruch Bild 256: alle durch das Rolltor` -> Bit 128 12391 -> 6570, Bit 129 13004 ->
  9012, Spieler-HP min 100 -> `Durchbruch beendet, Steuerung frei (Haltebits geloest)`.
* d2_gerettet (gleiche Karte OHNE (9,97) = John gerettet): `ROOM1041: Durchbruch 1041 entfaellt: Panikphase vorbei ((9,97)=0)`,
  keine Szene; F300 `PL(-21008,-13134,rot=2112,hp=100) ... cam=5 pf=00000007` = Elza mit Steuerung am Eintrittspunkt (13 [walk]-Zeilen).

### M7 Elza Nachspawn 1031 (Tuer 1041 -> 1031 nach der Montage; 1071-Kopien 75/76 leben, 47..49 tot) — P3 Folgezustand
Lauf n1031: `DOOR FIRE slot=1 ... target_cut=8 spawn=(-26214,0,-3861)` -> `Nachspawn 1031 (Programm F)`; state.log F40: Aktoren 10/11
Typ 0x11 bei (-20453,-2463)/(-22674,-4550) (= Leons Nachspawn-Lagen k_p_1030_nach im Gang 1041/1071, hp 250), Aktoren 12..14 die
Kriecher, 1..6 die Raum-Zombies des aufgebrochenen Rollos. Die getoeteten 47..49 fehlen.

### M8 Leon Nachspawn 1060 von UNTEN (echter Weg: Tuer 1040 -> 1060, Treppe A + B), Kopien 125/127 leben
Lauf l4_leon_unten (Karte Leon 1040 mit (9,71),(3,94),(9,73..76), 10E0-Quellen tot, 124/126 tot): `Leon: Nachspawn 1060 (Programm
E)` -> `Kopie Aktor 2 Bit 125 bei (20200,25750), Abstand 5356`, `Aktor 4 Bit 127 bei (20900,25350), Abstand 4591` -> `Spielerin auf
Etage 4 bei (25424,24568) -> Kopien frei, kleinster Abstand 4591` = Elzas Bild.

### M9 Leon Nachspawn 1060 von OBEN (Weg 3F -> Treppenhaus -> abwaerts, wie nach Irons' Tod ueber 1120) — NEBENWIRKUNG
1120 Tuer Slot 0 (ROOM1120 @0x00C76 `.. 90 65 c0 c7 d4 62 00 08 00 06 00 08`) setzt Leon in 1060 auf (26000,-14400,25300) Etage 8,
Cut 0. Lauf l7_leon_e8 / l7b_leon_e8: Karte genau dort (Leon, (9,71),(3,94),(9,73..76), 10E0-Quellen tot, Kopien 124..127 leben =
Zustand nach einer Montage mit vier Ueberlebenden), Skript `W0.5,U0.67,W0.3,L0.35,W0.3,U0.45,W0.3,A0.2,W5,R0.35,W0.3,U3.0,W0.3,
R0.35,W0.3,U0.5,W0.3,A0.2,W8` = Treppe B 8->6, quer zur Westtreppe, Treppe A 6->4. state.log/debug.log:
```
F119 cam=0 PL(24538,24448) ; F148 cam=1 (Treppe B) ; F239 PL(24551,19532) Etage 6 ; F419 PL(20324,20322) ; F458 cam=2 PL(20386,22057)
F458..F490 cam=2: die vier Kopien st=0 hp=0 (gehalten, nie gestartet) bei (20218,26533) (20218,25732) (21019,26532) (20916,25341)
ROOM1060: Kopie Aktor 1 Bit 124 ... Abstand zur Spielerin 1174 ; Aktor 2 Bit 125 ... 460 ; Aktor 3 Bit 126 ... 1336 ; Aktor 4 Bit 127 ... 853
ROOM1060: Spielerin auf Etage 4 bei (20364,25368) -> Kopien frei, kleinster Abstand 460
F539 PL(20860,25368) hp=80 gr=1 ; F599 hp=60 ; F659 hp=40 ; F719 hp=20 ; F779 hp=-20   (ohne Eingabe nach der Ankunft)
```
Bild `qa2/l7b_leon_e8.png` + `qa2/l7b_gross.png`: Bild 460-490 (Schnitt 2, Leon noch auf der Treppe) liegen die vier Kopien STEIF und
FLACH am Boden links (Pose ohne INIT, Haltebit 0x20 vor dem ersten KI-Takt), ab Bild 505 stehen sie auf und umringen Leon am Fuss
der Westtreppe. Gegenprobe ohne Kamera-Wechsel: l5_leon_oben / l5b_leon_oben (Karte Etage 6 am Kopf der Westtreppe, Hardware-Strecke
6->4 = z 20332 -> 25368 laut probe_stair_hwtruth_1060.c): Ankunft (21576,25368), Abstaende 1789/1406/1290/731, Griff ab F125, HP 0
bei F380. Zum Vergleich von UNTEN (M8/M5): kleinster Abstand 4591. Die Westseite ist "moeglichst weit weg" nur fuer die Ankunft von
unten — von oben endet die Westtreppe genau dort.

### M10 ALTER Leon-Spielstand nach Irons' Tod (v0.8.22..v0.8.26 und master: (9,73)=1, Q-Kopie-Bits nie gesetzt) — NEBENWIRKUNG
Lauf l6_altstand: Karte Leon 1040 an der Tuer nach 1060, (9,71),(3,94),(9,73), ALLE vier 10E0-Zombies getoetet (7:224..227), Kopie-
Bits 124..127 nicht gesetzt (= jeder Spielstand, der vor dieser Spur nach der Irons-Szene gespeichert wurde). `A0.2`:
```
[aot] DOOR FIRE slot=2 rect=(-19650,-13000,hw=750,hh=1100) target_cut=7 spawn=(26000,0,25300) ; PC loaded room1060.rdt
[elzazombies-r39] ROOM1060: Leon: Nachspawn 1060 (Programm E): Kopien an der Westseite, gehalten bis Etage 4
F90 ... [1 t=10 ... g=2d @(20218,26533)] [2 t=10 ... g=2d @(20218,25732)] [3 t=10 ... g=2d @(21019,26532)] [4 t=10 ... g=2d @(20916,25341)]
```
VIER Zombies im Treppenhaus, obwohl der Spieler alle vier in 10E0 getoetet hat (lebten sie noch, staenden sie zusaetzlich weiter in
10E0 = doppelt). Derselbe Zustand im vorhandenen Riegel integration_r35_cut1150 Teil D (Karte `1060 nach10f0;ersteszene;gesehen`,
10E0 nie betreten): build/r35_cut1150/lauf_0e944695_d/debug.log Zeile 45 `Leon: Nachspawn 1060 (Programm E)`, state.log F5 vier
Aktoren Typ 0x10 an der Westseite — der Riegel prueft es nicht und bleibt gruen. Ursache (Code gelesen): leon_nachspawn
(elzazombies_r39.c) gated nur ueber (9,73) und `re15_elzazombies_r39_lebend_1061()` = Kopie-Bits 124..127 == 0; ein Spielstand
ohne den neuen Umzug hat dort 0 = "lebt". Leons eigene Nachspawns 1130/1040 haben dafuer eigene "Kopien vorhanden"-Bits ((9,74)/(9,75),
irons_tod_1150.c umzug_vorbereiten), die mit dem Umzug gesetzt werden.
* Gegenprobe mit der AUSGELIEFERTEN v0.8.26-exe (release/pkg-win/re15_port_v0.8.26/re15_pc.exe, als Messkopie
  re15_pc_qa2_g_v0826_altstand.exe im Spurbaum gestartet; Spielstandformat seit v0.8.26 unveraendert: `git diff --stat v0.8.26 HEAD
  -- re15_savedata.c re15_memcard.c re15_savedata.h re15_memcard.h` leer), dieselbe Karte: `PC loaded room1060.rdt`, F90 OHNE einen
  Gegner. Der Unterschied kommt also aus dieser Nachbesserung (leon_nachspawn).

### M11 Elza von OBEN (1121 -> 1061 Etage 8 -> abwaerts; Abnahme 1 H3) — dieselbe Haltepose
Lauf e8_elza_oben (Elza, (9,95), Kopien 124..127 leben, Skript wie M9): Kamera 2 ab F463 waehrend der Westtreppe; Bild `qa2/
e8_elza_oben.png` Bild 465/495: die vier Kopien liegen flach am Boden links, Bild 525 umringen sie Elza; `Spielerin auf Etage 4 bei
(20432,25368) -> Kopien frei, kleinster Abstand 497`. (Abnahme 1 hat fuer diesen Fall nur die Lagen gemessen, nicht das Bild.)

### M12 Nachtrag 15: ROOM1131 in Elzas Panikphase (echter Weg: Tuer ROOM1121 Slot 2 @0x00CD8 -> 1131)
Lauf n15_tuer_a: Karte `1121;-7650;0;-2900;2048;0;` + (3,125),(3,193),(3,116),(9,95),(9,97) (Montage gelaufen, Panikphase),
10E1-/1071-Quellen umgezogen (7:224..227, 7:198..202), 1041-Kopien still (7:128..131), 1061-Kopien leben, (7,174) = Spur R's Hund
schon gesprungen (hund1121_r39.c
`schon_gesprungen`, sonst startet R's Fensterszene); `A0.2,W1,U3,W2`: `DOOR FIRE slot=2 rect=(-8450,-2900,hw=500,hh=1000)
target_cut=2 spawn=(-2200,0,-2150)` -> `PC loaded room1131.rdt` -> bis Bild 300 in 1131 KEIN Gegner (state.log, alle Bilder),
keine `[elzazombies-r39]`- und keine `[irons-tod]`-Zeile. Dasselbe beim Laden in 1131 (Lauf n15_1131, CONTINUE, 300 Bilder, 0
Gegner). ROOM1131.RDT hat keinen Sce_em_set (scd_dump_room.py), der Code der Spur kennt 0x1131 nicht (grep), Leons Kette greift
nur in xxx0-Raeumen; die Elza-Kette laeuft 1041 -> [1061] -> 1031 -> 1041 (M1-M3). "Nur 1071 kommt hinzu": gegenueber dem Stand
nach Nachtrag 11 ist bei Elza einzig der 1031-Cut-7-Teil neu (M1/M2; Dossier 9.7).

## Gates

### RE-Gate (Stichproben selbst disassembliert/gelesen; PSX.EXE sha256 e6b340fd..., STAGE1.BIN sha256 50dffb0e... =
### info/Re1.5/PSX/BIN/STAGE1.BIN; RDTs re15_port/shared_assets/PSX = info/Re1.5/PSX)
1. PSX.EXE @0x800397a8 `lh v0,4064(v0)` (0x800b0fe0 Stage), @0x800397b0 `lh v1,4066(v1)` (0x800b0fe2 Raum), @0x800397b8 `lw a0,
   -13764(a0)` (0x800aca3c), @0x800397e4 `srl a0,a0,31` (Bytes `c2 27 04 00`), @0x800397e8 `jal 0x80013b60`, Delay-Slot
   @0x800397ec `addu a0,a0,v0` (`21 20 82 00`) = Datei-Index = Tabellenwert + Elza-Bit. Traegt die Behauptung, dass dasselbe
   Raum-Byte 0x06 der Tuer ROOM10C0/10C1 @0x00E42 fuer Leon ROOM1060 und fuer Elza ROOM1061 laedt (RB_1061, Leons Tuer-Weiche).
2. STAGE1.BIN @0x80100450 `lbu v0,9(a0)` / @0x80100458 `andi v0,v0,0x20` / @0x8010045c `bne v0,zero,0x80100658` (Bytes `09 00 82 90
   00 00 00 00 20 00 42 30 7e 00 40 14`) = Zombie-Takt uebersprungen bei entity+0x9 Bit 0x20 — die Form, mit der Baustein 3/5
   (rollo_warten) und das Halten der 1061/1060-Kopien arbeiten. Stimmt (die Haltepose aus M9/M11 ist die Folge davon, dass der
   Port das Bit VOR dem ersten Takt setzt: der INIT-Zweig laeuft dann nie).
3. PSX.EXE FUN_8003f038: @0x8003f040 `lw v0,-13760(v0)` / @0x8003f048 `and v0,v0,v1` (0x02000000) / @0x8003f04c `bne`; @0x8003f064
   `ori a0,zero,0x1`, @0x8003f070 `lw v0,68(v0)` (Raumkopf+0x44 = sub-Zeiger), @0x8003f07c `sw v0,16240(at)` (0x800b3f70),
   @0x8003f080 `jal 0x8003ee3c` (a1 = 1) = der sub01-Reseed je Bild, den re15_irons_tod_sub01_gesperrt jetzt auch in RE15_IT_S1060
   unterdrueckt (wie in Leons uebrigen Schritten). Stimmt.
4. RDT-Bytes (python, selbst gelesen): `cmp -l` ROOM1060/1061, 10E0/10E1, 10C0/10C1, 1070/1071 = je 0 Bytes (1030/1031 und 1040/1041
   verschieden); ROOM1071 @0x015CA/DE/F2/@0x01606/@0x0161A `44 0i tt gg 00 00 p6 c6..ca` Typen 10/10/10/11/11; ROOM10E0
   @0x00D56/6A/7E/92 Typ 0x10 grid 0x06 Bits e0..e3; ROOM1031 RID @0x00140 = ROOM1030 @0x00140 bis auf die letzten 4 Byte
   (`98 12` / `3c 12`), pos/target (-26820,-3114,-2574)/(-15858,-936,-5040); ROOM1060 RID @0x000A0 (20199,-12628,21273)/(24670,
   -7088,27767); Se_on ROOM1031 @0x02862 = ROOM1030 @0x02776 = `36 02 0c 00 00 00 cc dd f8 f8 f0 a7`; ROOM10C0 @0x00E42 -> Raum 0x06
   Cut 2 Etage 4 (26000,-7200,25300) Gier 2048; ROOM1040 @0x010D8 -> 0x06 Cut 7 (26000,0,25300); ROOM1031 BLK @0x01C20 (-13470,
   -32760)-(-3660,-23860) und @0x01C98 (-21290,-31360)-(-13470,-26960) = ROOM1030 @0x01BBC/@0x01C34; ROOM1141 @0x00E70/@0x00E78
   `34 0c 30 00`, @0x00E80 `29 00`, @0x00F64/@0x00F74 `34 0c 10 00`; ROOM1141 sub00 nur Typ 0x49 @0x00B06 + Typ 0x27 @0x00B1A/2E/54/68
   (kein Zombie -> kein Briefing-Room-Schritt fuer Elza, wie im Dossier 9.7). ROOM1060: Slots 0..10 + 16, sub00/sub01 = Evt_end
   (Ereignis 21/30 und Tuer-Slot 20 frei).
5. Leons Programm-Vorlagen, die Elzas Programm C uebernimmt (irons_tod_1150.c re15_irons_tod_programm): 6 = k_p_1030_tuer
   (EM_1070_DURCH Bits 47,48,49,75,76 + Sleep 174), 7 = k_p_1030_tor (Se_on @0x02776), 12 = k_p_1030_nach_tuer (EM_1070_LISTE),
   19 = k_p_1030_luecke (Sleep 10) — Indizes stimmen.
6. Guess-Tells im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|geschaetzt|vorerst|platzhalter`): keine;
   kein neuer getenv. Neue Konstanten tragen Adresse/Byte-Offset (0xc6/5 @0x015CA.., Cut 7 @0x00140, Zonen 0/10 @0x01C20/@0x01C98,
   Kamera 1060 @0x000A0) oder sind als PORT-WAHL benannt (geteilte Kopie-Bits nach der Raum-Paar-Regel, Reihenfolge 1130 -> 1060
   -> 1040, Zeiten = Leons k_p_1130).
7. Erklaert der Fix den Befund (Dossier 9.2 "vorher")? Ja: Leons Kette bekommt den Schritt ueber re15_r39q_leon_tuer + Zustand S1060
   (M4: 1130 -> 1060 -> 1040; ohne 10E0-Ueberlebende alte Kette), Elzas Programm C haengt Leons Cut-7-Vorlagen an, wenn s_n1071 > 0
   (M1/M2; ohne: M3), durchbruch_1041 fragt (9,97) (M6).

### Zensus (selbst, unabhaengig vom Riegel)
`qa2/tools/zensus.py` (scd_walk_lib) ueber STAGE1-3: 132 RDTs mit SCD, 706 Sce_em_set, 177 verschiedene pc[7]; KEIN Sce_em_set-
Tot-Bit und kein Ck/Set(7,..) auf 47, 48, 49, 75, 76, 124..131, 133, 144, 145. Port-Code mit Zone-7-Zugriff ausser dieser Spur: nur
irons_tod_1150.c (IT_ZONE, Leon-Raeume). Die Mitbenutzung von Leons 47..76 fuer Elzas 1031-Cut-7-Kopien ist damit konfliktfrei.

### Vertrag (VERTRAG.md 1.1-1.3, 2)
* Pfade (`git diff master --name-only`): Dossier + 2 Belegbilder, elzazombies_r39.c/.h (eigen), irons_tod_1150.c + re15_irons_tod.h
  (Leons Modul der Runde 35; fuer "bei Leon zusaetzlich das Treppenhaus" noetig; 15 Zeilen an 7 Stellen + 4 im Header, jede Stelle
  "Runde 39 Spur Q", die Logik liegt in elzazombies_r39.c), Tests (eigene + vier Riegel der Runden 35/36 auf "10E0 leer" gesetzt,
  begruendet, die Kette ohne 1060 bleibt so gemessen; der neue Schritt in unit/integration_r39_elzazombies_leon*). Kein release/,
  platform/android/, shared_assets/PSX/, keine CMakeLists.txt. OK (Hinweis H3 zur Hakengroesse).
* Bits/Ids: kein neues Bank-9-Bit, keine Nachrichten-Ids, keine Sprachdateien, keine Assets; Ereignis 30 + Slot 20 in 1060 frei (s.o.).
* Riegel je Punkt vorhanden und messend (19: unit zensus, kette_4/_0/_2/_00, nach1061, nach1031, durch1041, ohne, leon_4/_2/_0,
  leon_nach1060; integration kette, knall, treppe, durch, ohne, leon). Die Faelle "Leon von oben" und "alter Leon-Spielstand" deckt
  kein Riegel (s. Maengel).

### Gezielte Tests (keine volle Suite; Shell ohne RE15_*)
* `ctest --tests-from-file qa2/unit_liste.txt -j 3 --timeout 600` (93 Tests: alle unit_r39_elzazombies_*, unit_r35_cut1150_*,
  unit_r36_szene1150_*, unit_r38_montage_*, unit_r37_zombieaktiv_*, r38_kleinkram_irons, r37_szene11c0, r35_fenster_knall,
  montage_fx, r36_gegnerzahl_raum1030/1040, r34n_a_rolltor, stair_hwtruth_1060, r36_kartenbits, r33_speichern, map_speichern_laden,
  r37_re2zombie_raum1130, r36_szenenleon_irons, r36_elza_irons, r39_parkplatz11c0_*, room1140/1070/1040/1030-Gruppen, irons_*,
  probe_r16_tuer1040_*): "100% tests passed, 0 tests failed out of 93" (09:51:41-09:52:00).
* `ctest --tests-from-file qa2/integ_liste.txt -j 2 --timeout 1500` (27 Tests: alle 6 integration_r39_elzazombies_*,
  integration_r35_cut1150, r36_szene1150, r38_montage, r37_zombieaktiv, r38_kleinkram_irons, r37_szene11c0, r37_re2zombie,
  r37_elzaems_leon10c0kraehe, r37_elzaems_elza10c1kraehe, r33_speichern, r36_elza_irons, r39_hund1121, r35_cut10f0, r37_sperre1050,
  r39_parkplatz11c0_k/w/n/e/i, elza_vollstart, r36_story_elza): "100% tests passed, 0 tests failed out of 27" (Total 1706,72 s,
  unter Fremdlast: Spuren J/P/T liefen parallel). Danach liegt in build/platform/pc nur re15_pc.exe.

## Urteile je Punkt (gegen den Wortlaut; Lesart der AUFTRAG.md eingerechnet)
* P1a "einmal in den Flur hinein von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden": ERFUELLT.
  Montage-Schnitt ROOM1061 Cut 2 (Schild F2, 10C1-Tuer); Zahl mit eigenen Faellen: 3 -> `S ROOM1061 T105 3` (M1), 2 -> 2 (M3),
  0 -> kein 1061-Schritt (M2); Kopien kommen 1171..1962 naeher an die Kamera, HP 100; Bild qa2/e1_1061.png.
* P1b "wenn man in ROOM 1061 die Etage von unten über die Treppe erreicht aber möglichst weit weg an der Seite stehen": ERFUELLT
  fuer den genannten Fall (M5, echter Weg von unten: Westseite unter dem Fenster, Abstaende 5564/4823/4591, bis zur Ankunft
  gehalten, die Haltepose ist von unten nie im Bild). Der Fall "von oben" ist nicht verlangt, hat aber einen Mangel (Mangel 2).
* P1c "im ROOM 1031 über die Absperrung/Das Rollo, so wie bei der cutscene": ERFUELLT (M1-M3: Original-Zeilen ROOM1031 sub08,
  Cut 12, Kriecher unter dem Rollo durch, HP 100; Bild qa2/e1_1031.png ab Bild 250).
* P2 Nachtrag 11 "wie bei leon im ROOM 1041" (falls nicht besiegt): ERFUELLT (M6 d1: echter Weg 1061 -> 1041 in der Panikphase,
  zwei nicht Besiegte -> zwei durchs Rolltor, Knall, HP 100, Steuerung zurueck). Der Teil "nicht in ROM 1071" ist durch Nachtrag 12
  ("bei Elza zusätzlich ROOM 1071") ersetzt.
* H1-Entscheid (nur Panikphase): ERFUELLT (M6 d2: ohne (9,97) "Durchbruch 1041 entfaellt", Elza mit Steuerung an der Tuer).
* P3a Nachtrag 12 "bei Elza zusätzlich ROOM 1071": ERFUELLT (M1: zwei Ueberlebende -> zwei Kopien vor der Lobby-Office-Tuer in 1031
  Cut 7, Original-Knall @0x02862, dann das Rollo; M2: einer -> einer; M3: keiner -> kein Cut 7; Nachspawn in 1031 M7; Bild
  qa2/e1_1031.png Bild 10-210).
* P3b Nachtrag 12 "bei Leon zusätzlich das Treppenhaus": TEILWEISE. Der Montage-Schritt ist erfuellt (M4: 1130 -> Tuerknall ->
  ROOM1060 Cut 2 mit genau den 10E0-Ueberlebenden 1/3 -> Knall -> 1040 (unveraendert 361 Bilder) -> 1030 -> 11C0 -> Rueckkehr,
  HP 100; 10E0 leer -> alte Kette; Bild qa2/l2_1060.png). Der Folgezustand derselben Zombies im Treppenhaus hat zwei Maengel
  (alte Leon-Spielstaende: vier Zombies aus dem Nichts; Ankunft von oben: liegende Haltepose + Hinterhalt am Treppenfuss).
* P4 Nachtrag 15 (1131 bei Elza ohne Durchbruch; nur 1071 kommt hinzu): ERFUELLT (M12: echter Weg 1121 -> 1131 in der Panikphase,
  0 Gegner, keine Zeile der Spur; Laden in 1131 ebenso; bei Elza neu nur der 1071-Teil).
* Gesamtbild "aus den gleichen Tueren" (Dossier 9.7 gegengeprueft): Treppenhaus 10C0/10C1 beide; Lobby Office 1070/1071 beide;
  Rollo 1030/1031 beide; Rolltor 1040/1041 beide (Leon in der Montage, Elza beim naechsten Betreten in der Panikphase = Lesart zu
  Nachtrag 11); Briefing Room nur Leon — ROOM1141 sub00 hat keinen Zombie-Record (selbst gelesen), vom Nutzer nicht verlangt.
* Gates: RE-Gate ERFUELLT; Zensus ERFUELLT; Vertrag ERFUELLT (Hinweis H3); gezielte Tests ERFUELLT (93 + 27 gruen);
  Nebenwirkungen (beide Szenarien, Speichern/Laden) NICHT ERFUELLT (Maengel 1 und 2).

## Maengel
1. **Alte Leon-Spielstaende nach Irons' Tod bekommen vier Zombies im Treppenhaus.** Jeder Leon-Spielstand mit (9,73)=1, der vor
   dieser Nachbesserung gespeichert wurde (ausgeliefert v0.8.22-v0.8.26, auch master/Hauptbaum-exe), hat die Kopie-Bits 124..127 = 0.
   Beim Betreten von ROOM1060 meldet die exe `Leon: Nachspawn 1060 (Programm E)` und setzt vier Polizei-Zombies an die Westseite —
   auch wenn der Spieler alle vier in 10E0 getoetet hat (M10, Lauf l6_altstand: 7:224..227 gesetzt, trotzdem Aktoren 1..4 Typ 0x10);
   leben sie noch, stehen sie doppelt (10E0 und 1060). Gegenprobe v0.8.26-exe mit derselben Karte: kein Gegner in 1060. Derselbe
   Zustand steckt ungeprueft im vorhandenen Riegel integration_r35_cut1150 Teil D (build/r35_cut1150/lauf_*_d/debug.log Z. 45,
   state.log F5 vier Aktoren). Ursache: `leon_nachspawn`/`programm_bauen` (Zustand AUS, Leon) pruefen nur (9,73) und Bits 124..127
   == 0; es fehlt ein Merker "Umzug 10E0 -> 1060 ist gelaufen", wie ihn Leons Montage fuer 1130/1040 hat ((9,74)/(9,75), gesetzt in
   umzug_vorbereiten). Nachpruefbar: Karte `1040;-20700;0;-13000;0;0;9:71;3:94;9:73;7:224;7:225;7:226;7:227`, Skript `A0.2,W5`
   (Start 60), `RE15_STATE_LOG`, Ende 100#1060 -> F90 darf keinen Gegner zeigen. Ein Riegel dafuer fehlt.
2. **Ankunft im Treppenhaus von OBEN: Haltepose sichtbar, dann Hinterhalt am Fuss der Westtreppe (beide Szenarien; fuer Leon der Weg
   3F -> 1120 -> 1060 nach Irons' Tod).** Die Kopien bekommen das Haltebit 0x20 vor ihrem ersten KI-Takt (halten_1061, Kommentar
   "sofort: vor dem ersten KI-Takt"), der INIT-Zweig laeuft nie (state.log st=0, hp=0). Von unten ist das unsichtbar
   (Kamerawechsel und Freigabe im selben Bild, M5). Von oben zeigt Schnitt 2 den Absatz schon waehrend der Westtreppe 6->4: Leon
   (M9, l7b_leon_e8 Bilder 460-490) und Elza (M11, e8_elza_oben Bilder 465-495) sehen vier STEIF FLACH LIEGENDE Koerper, die bei der
   Ankunft aufspringen und sie am Treppenfuss umringen (kleinster Abstand 460 bzw. 497; von unten 4591). Das Dossier 9.8 sagt dem
   Nutzer fuer Leon nur "stehen sie auf dem Absatz unter dem Fenster und laufen erst los, wenn du oben ankommst (wie bei Elza)" —
   der Fall von oben (fuer Leon nach Irons' Tod ein naheliegender Weg nach unten, neben dem Aufzug 1080) fehlt. Nachpruefbar:
   Karte `1060;26000;-14400;25300;2048;0;9:71;3:94;9:73;9:74;9:75;9:76;7:224;7:225;7:226;7:227` (= Ankunft aus 1120, ROOM1120
   Tuer Slot 0 @0x00C76), Skript `W0.5,U0.67,W0.3,L0.35,W0.3,U0.45,W0.3,A0.2,W5,R0.35,W0.3,U3.0,W0.3,R0.35,W0.3,U0.5,W0.3,A0.2,W8`
   (Start 30), FRAMEDUMP 460-580/15. Ob die Kopien bei Ankunft von oben anders stehen sollen, ist eine Entscheidungsfrage an den
   Nutzer (E1); die liegende Haltepose ist in keinem Fall gewollt.

## Hinweise (keine Maengel)
* H1 (wie Dossier 9.9) Integration mit Spur P im Spurbaum nicht pruefbar: P's Tuersperre 1041 -> 1031 in der Panikphase, Panikmusik
  ueber der jetzt rund 830 Bilder langen Elza-Montage, P's 1041-Szene bei der Rueckkehr der Montage.
* H2 Elzas Rolltor kommt weiter erst beim naechsten Betreten von 1041 (Leon: in der Montage) — Orchestrator-Lesart zu Nachtrag 11,
  unveraendert seit Abnahme 1 (H2 dort).
* H3 Hakengroesse: irons_tod_1150.c +15/-2 Zeilen an 7 Stellen, re15_irons_tod.h +4/-1 — mehr als die "1-5 Zeilen" der gemeinsamen
  Dateien (VERTRAG 1.3); die Datei steht nicht in der Liste, der Eingriff ist durch Nachtrag 12 (Leons Montage) zwingend, jede Stelle
  ist markiert, die Logik liegt in elzazombies_r39.c. Zur Kenntnis fuer den Orchestrator.
* H4 Dieselbe Klasse wie Mangel 1 fuer Elza waere ein Spielstand mit (9,95)=1 aus dem alten Q-Stand (master/Hauptbaum-exe 08:30):
  1031 bekaeme die 1071-Kopien (Bits 47..76 = 0), 1071 behielte seine. Praktisch nicht erreichbar, weil ohne Spur P niemand (9,97)
  setzt; beim Beheben von Mangel 1 mitbedenken.
* E1 Entscheidungsfrage an den Nutzer (zu Mangel 2): Sollen die Treppenhaus-Zombies bei Ankunft von OBEN ebenfalls "moeglichst weit
  weg" stehen (dann an der Tuer-/Ostseite)? Fuer Elza bisher "nicht verlangt" (Abnahme 1 H3), fuer Leon ist es jetzt sein Weg nach
  unten durchs Treppenhaus.

## Gesamturteil
NICHT BESTANDEN. Erfuellt und selbst gemessen: P1 (10C1 -> Flur mit der Zahl der 10E1-Ueberlebenden, 1061 von unten weit weg, Rollo
1031), P2 (1041 wie Leon), H1 (nur Panikphase), P3 Elza (+1071), P4 (1131 ohne Durchbruch) und der Leon-Montage-Schritt
(+Treppenhaus); RE-Gate (3 Code-Stellen + 12 RDT-Stellen selbst gelesen), Zensus, Vertrag, 120 gezielte Tests gruen. Offen: Mangel 1 (alte Leon-Spielstaende ->
vier Zombies aus dem Nichts in 1060, Gegenprobe v0.8.26) und Mangel 2 (Ankunft von oben: liegende Haltepose + Hinterhalt;
Dossier-Text fuer Leon unvollstaendig).
