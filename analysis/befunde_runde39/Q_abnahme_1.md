# Runde 39 Spur Q "elzazombies" — Abnahme 1 (unabhaengig)

Stand: Abschluss (fortlaufend geschrieben). ERGEBNIS: BESTANDEN — alle Teilpunkte am gebauten Spiel selbst gemessen und
erfuellt (eigene Ueberlebenden-Zahlen 0/1/2, echte Tuer-Wege), Mangel 1 der Abnahme 0 (Riegel erben die Umgebung) behoben
und mit Gegenprobe belegt, RE-Gate (7 Stichproben), Vertrag und 93 Gruppentests gruen. Keine Maengel; Hinweise H1-H7.
Baum `.claude/worktrees/r39_elzazombies`, Zweig `r39/elzazombies`, HEAD 721d530a (Nachbesserung 1 nach Abnahme 0).
Gebaut mit `bash re15_port/tools/local_build.sh configure` (exit 0) + `build` (exit 0, "ninja: no work to do") —
re15_pc.exe 2026-10-19 02:17:18 (7670520 B), test_r39_elzazombies.exe 03:55:01 (nach der Quellaenderung 03:54:31).
Seit Abnahme 0 (1c304b80) geaendert (`git diff 1c304b80 HEAD --stat`): NUR Dossier, tests/integration/test_r39_elzazombies.cmake,
tests/unit/probes/r39_elzazombies.cmake (Kopfkommentar), tests/unit/test_r39_elzazombies.c — kein Spielcode.

## Nutzer-Punkt (AUFTRAG.md Nachtrag 10 Absatz 2, Nachtrag 11 fuer 1071 -> 1041)
> "Und auch hier sollen jetzt Zombies durchbrechen - einmal in den Flur hinein von 10C1 aus - so viele wie im medical
> room - also 10E1 am Leben gelassen wurden., die sollen dann, wenn man in ROOM 1061 die Etage von unten über die Treppe
> erreicht aber möglichst weit weg an der Seite stehen. Dann das gleiche nochmal wie bei Leon in den ROOM 1071, falls die
> Zombies nicht besiegt wurden und im ROOM 1031 über die Absperrung/Das Rollo, so wie bei der cutscene."
> Nachtrag 11: "Ich korrigiere, die Zombies sollen nicht in ROM 1071 noch durchbrechen, sondern dann noch wie bei leon im ROOM 1041."

Teilpunkte:
- Q1 Durchbruch 10C1 -> Flur (Treppenhaus 1061, 2F-Absatz), Zahl = in 10E1 am Leben gelassen
- Q2 1061: Ankunft von unten ueber die Treppe -> moeglichst weit weg an der Seite
- Q3 1041 wie bei Leon (Rolltor), falls nicht besiegt (Nachtrag 11); 1071 unberuehrt
- Q4 1031 ueber die Absperrung/das Rollo wie in der Cutscene
- Q5 Ausloeser = steigende Flanke (9,97) (Orchestrator-Lesart), Laden keine Flanke, Leon unveraendert
- N1 Nachbesserung 1: Riegel erben keine RE15_*-Umgebung (VERTRAG Abschnitt 2), Messkopien werden entfernt

## Messprotokoll

Werkzeug: eigenes Lauf-Skript `scratchpad/qa1/lauf.sh` (Karte mit `probe_r39_elzazombies_karte` schreiben, Messkopie
`re15_pc_qa1_<lauf>.exe` im build/platform/pc, CONTINUE ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0,
RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1, Kopie danach geloescht), Laufordner `scratchpad/qa1/runs/<lauf>`, Bildtafeln
`scratchpad/qa1/*.png` (`scratchpad` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`).
Spur P ist in diesem Baum nicht enthalten: (9,97) setzt `RE15_SET_FLAG_AT=9:97@<bild>` (= P's Setzpunkt). Die Shell der
Messlaeufe hatte keine RE15_*-Variable (`env | grep -i ^re15_` leer). EIGENE Ueberlebenden-Zahlen (1 und 2, dazu 0), 1 und 2
anders als die der Riegel (4/0/2 unit, 4 integration) und der Abnahme 0 (3/0/4).

### M1 Kette mit EINEM Ueberlebenden in 10E1 (7:224, 7:225, 7:226 tot; nur 0xe3 lebt) — Q1, Q4, Q5
Laeufe a_kette1_1061 (FRAMEDUMP 10-170/40, Ende 175#1061), b_kette1_1031 (FRAMEDUMP 2-352/50, Ende 352#1031),
c_kette1_voll (Ende 400#1041); Karte `1041;-21008;0;-13134;2112;0;3:125;3:193;3:116;7:224;7:225;7:226`, mit Ton,
`RE15_SET_FLAG_AT=9:97@200`. debug.log (c_kette1_voll, gekuerzt):
```
[elzazombies-r39] Flanke (9,97) 0->1 in ROOM1041 Takt 203 -> Montage ausstehend
[elzazombies-r39] Umzug: 10E1 lebend 1 -> 1061; (9,95)=1
[elzazombies-r39] Folge #1 K ROOM1041 T61 0                                  (Tuerknall, Satz 0)
[aot] DOOR FIRE slot=20 ... target_cut=2 spawn=(20200,-7200,20400) ; [room] PC loaded room1061.rdt
[elzazombies-r39] Folge #3 S ROOM1061 T105 1 ; Folge #4 C ROOM1061 T105 2       <- EINE Kopie = EIN Ueberlebender
[elzazombies-r39] Folge #5 K ROOM1061 T237 1                                  (Knall Satz 1)
[aot] DOOR FIRE slot=20 ... target_cut=12 spawn=(-28558,0,-2183) ; [room] PC loaded room1031.rdt
ROOM1061 Cut 2 Bild 176: Aktor 4 Bit 127 Abstand zur Kamera 7547 -> 4861 Lage (26000,26100) -> (23818,24518) ; HP min 100
[elzazombies-r39] Folge #7 S ROOM1031 T281 3 ; Folge #8 C ROOM1031 T281 12    (3 Kriecher, erstes Bild Cut 12)
[aot] DOOR FIRE slot=20 ... target_cut=0 spawn=(-21008,0,-13134) ; [room] PC loaded room1041.rdt
ROOM1031 Cut 12 Bild 357: Kriecher Bit 133 9790 -> 7561, Bit 144 10524 -> 5579, Bit 145 9522 -> 4765;
                          Raum-Zombies Bit 0..5 12906 -> 8997 .. 15058 -> 9823 ; HP min 100
[elzazombies-r39] ROOM1041: Rueckkehr (Programm D) ; Rueckkehr beendet, Steuerung frei
[walk] F390 cut=0 mo=211 ... pl pos=(-21008,0,-13134) rot=2112                (gemerkte Stelle)
```
Bilder: `qa1/a_1061.png` + `qa1/a_1061_gross.png` (Bild 10: ein Polizei-Zombie steht am 2F-Absatz links neben der
10C1-Tuer, Schild "F2"; Bild 170: er ist bis an das Treppengelaender auf die Kamera zu gegangen; Tuer bleibt zu = Leons
Form "Knall vor dem Schnitt, dann stehen sie schon drin"). `qa1/b_1031.png`: Bild 2 Einblendung, ab Bild 52 Cut 12 =
das aufgebrochene Rollo (verbogene Stangen, Truemmer), Zombies dahinter, Kriecher kommen darunter durch auf die Kamera zu.

### M2 ECHTER WEG mit ZWEI Ueberlebenden (7:224, 7:226 tot): Kette -> Tuer 1041 -> 1061 -> Treppe A + B von unten — Q2
Lauf d_treppe2_echt: Karte `1041;-20700;0;-13000;0;0;3:125;3:193;3:116;7:224;7:226`, RE15_NOAUDIO=1,
`RE15_SET_FLAG_AT=9:97@100`, Eingabeskript (je Raum ab Bild 60) `W4,` + Treppen-Skript des Riegels (in der Montage wirkt
es nicht: player_mode 2), FRAMEDUMP 400-900/25, Ende 900#1061. debug.log:
```
Umzug: 10E1 lebend 2 -> 1061 ; S ROOM1061 T105 2 ; C 2 ; Bit 125 6421 -> 4861, Bit 127 7547 -> 5438 ; ... 1031 C 12 ...
ROOM1041: Rueckkehr beendet, Steuerung frei
[aot] DOOR FIRE slot=2 rect=(-19650,-13000,hw=750,hh=1100) target_cut=7 spawn=(26000,0,25300)   (echte Tuer)
[room] PC loaded room1061.rdt
ROOM1061: Nachspawn 1061 (Programm E): Kopien an der Westseite, gehalten bis Etage 4
ROOM1061: Kopie Aktor 2 Bit 125 bei (20200,25750), Abstand zur Spielerin 5356
ROOM1061: Kopie Aktor 4 Bit 127 bei (20900,25350), Abstand zur Spielerin 4591
ROOM1061: Spielerin auf Etage 4 bei (25424,24568) -> Kopien frei, kleinster Abstand 4591
[walk] F330 cut=5 mo=221 pl pos=(21375,-761,24184) (Treppe A) ... F570 cut=4 mo=221 (Treppe B) ... F660 cut=2 pl pos=(25424,-7200,24568)
```
Bild `qa1/d_1061.png`: 550/600 Aufstieg (Treppe), 625 = Ankunft Cut 2: Elza rechts an der 10C1-Tuer, die ZWEI links an
der Westseite unter dem Fenster (am weitesten weg); 650-750 kommen sie heran, 800 Griff, 875 einer am Boden.
Die Lagen sind genau k_lagen_nach[1]/[3] (die Plaetze der lebenden Records 0xe1/0xe3) — waehrend Etage 0/2 unbewegt.

### M3 ECHTER WEG Kette -> 1061 -> zurueck durch die Tuer nach 1041 = Rolltor-Durchbruch (Q3, Nachtrag 11), ZWEI Kopien
Lauf e_durch2_echt: Karte wie M2, mit Ton, Skript `W4,A0.2,W1,R0.7,U0.2,A0.2,W20` (1041-Rueckkehr: `A` -> Tuer nach
1061; in 1061 umdrehen, `A` -> Tuer nach 1041), FRAMEDUMP 10-430/30, Ende 430#1041. debug.log:
```
... Kette (Umzug 2, S ROOM1061 T105 2, C 2, K 1, ROOM1031 S 3 C 12) ... ROOM1041: Rueckkehr beendet, Steuerung frei
[aot] DOOR FIRE slot=2 rect=(-19650,-13000,hw=750,hh=1100) target_cut=7 spawn=(26000,0,25300) ; PC loaded room1061.rdt
ROOM1061: Nachspawn 1061 (Programm E): Kopien an der Westseite, gehalten bis Etage 4
[aot] DOOR FIRE slot=2 rect=(27100,25400,hw=500,hh=1100) target_cut=5 spawn=(-21008,0,-13134) ; PC loaded room1041.rdt
Durchbruch 1041: 2 Zombies aus 1061 (nicht besiegt) -> hinter das Rolltor
Folge #2 S ROOM1041 T0 2 ; Folge #3 C ROOM1041 T0 1
Durchbruch 1041 startet (Programm G): Eintritt (-21008,0,-13134) Gier 2112 Cut 5, Rolltor faehrt hoch
Signal (5,29): Knall wie ROOM1030 (Knall-Bank Satz 1) ; Folge #4 K ROOM1041 T2 1
ROOM1041: Durchbruch Bild 297: alle durch das Rolltor -> (5,30)=1 ; Folge #5 C ROOM1041 T388 5
ROOM1041 Cut 1 Bild 387: Bit 129 13004 -> 5441 Lage (-25700,2300) -> (-25401,-5320); Bit 131 14897 -> 8890 (-24500,4100) -> (-24341,-2012)
   (Raum-Records Bit 20/21 kommen mit durch 11089 -> 7869 / 14047 -> 7063; Bit 22..24 laufen vom Tor weg = Original-Raum)
ROOM1041 Cut 1 Ende Bild 387: Spieler-HP min 100 ; ROOM1041: Durchbruch beendet, Steuerung frei (Haltebits geloest)
```
Bild `qa1/e_1041.png`: Bild 10 Rolltor zu (Cut 1), 100 Fuesse unter dem hochfahrenden Rolltor, 160 halb offen, 220-370
kommen die Zombies hindurch auf die Kamera zu, 430 Cut 5: Elza an der 1061-Tuer mit Steuerung.

### M4 "falls die Zombies nicht besiegt wurden" (Tuer 1061 -> 1041, Zustand nach der Montage im Spielstand)
* f_durch_besiegt: Karte `1061;26200;0;25300;0;7` + (3,125),(3,193),(3,116),(9,95), 10E1-Quellen 224..227 tot, ALLE
  1061-Kopien 124..127 tot, 1041-Kopien 128..131 still; `A0.2` -> `DOOR FIRE slot=2 ... spawn=(-21008,0,-13134)` ->
  ROOM1041 bis Bild 300: KEINE Durchbruch-Zeile, keine Szene, Elza laeuft normal (13 [walk]-Zeilen). Alle besiegt = nichts.
* g_durch_einer: wie f, aber nur 126 lebt: `Durchbruch 1041: 1 Zombies aus 1061 (nicht besiegt)`, S 1, C 1, Knall Satz 1,
  `Durchbruch Bild 197: alle durch`, Bit 130 13941 -> 7652, HP min 100, `Durchbruch beendet, Steuerung frei`.
  Zahl der Durchbrechenden = Zahl der in 1061 nicht Besiegten (1 / 2 in M3 / 0).

### M5 Nebenwirkungen: Leon, Laden (Q5)
* h_leon1060 (Leon, Karte `1060;26200;0;25300;0;7;9:95;3:94`, `RE15_SET_FLAG_AT=9:97@50`, `A0.2`): `[setflag-at] Frame 50:
  flag(9,97) = 1` -> `DOOR FIRE slot=2 ... spawn=(-21008,0,-13134)` -> `PC loaded room1040.rdt` -> Bild 300: KEINE
  `[elzazombies-r39]`-Zeile (keine Flanke, kein Nachspawn 1060, kein Durchbruch 1040), obwohl (9,95) gesetzt ist.
  (Erster Versuch ohne (3,94): Leons Tuer 1060 Slot 2 ist bis zur Irons-Szene ein Text-Platz — `[tuer1060] ... Text-Platz
  msg 1` = Runde 35 L, kein Effekt der Spur.)
* i_laden1041 (Elza, Karte 1041 Cut 5 mit (9,95), (9,97), 1061-Kopien leben): CONTINUE -> bis Bild 300 keine Zeile
  (Laden ist keine Flanke; Durchbruch nur ueber eine Tuer, wie im Dossier 4.6 beschrieben).

### N1 Nachbesserung 1 — Riegel erben keine Umgebung (VERTRAG Abschnitt 2), selbst gemessen
* FEINDLICHE Umgebung, drei Schreibweisen: `env RE15_NOAUDIO=1 re15_kill_at=150 Re15_Re2_Dmg_Model=0 ctest -R
  "r39_elzazombies" -j 2 -V` -> "100% tests passed, 0 tests failed out of 13" (04:27:13-04:29:56,
  `qa1/ctest_eigen_feindlich.log`). Jeder der 5 Integrationsteile meldet `geerbte Umgebung geloescht:
  RE15_NOAUDIO;Re15_Re2_Dmg_Model;re15_kill_at` (Reihenfolge je Teil verschieden), jeder der 8 Unit-Teile drei Zeilen
  `umgebung: geerbt <name> -> geloescht`. Knall-Werte wie ohne Umgebung: `Knall Bilder 624..665 (42), Differenz-RMS max
  6567` / `Knall Bilder 806..849 (44), Differenz-RMS max 4657`.
* GEGENPROBE mit dem ALTEN Riegel (git show 1c304b80:.../test_r39_elzazombies.cmake, nur der exe-Kopiename geaendert),
  TEIL durch, `env re15_kill_at=150` (klein geschrieben): die exe stirbt in der Szene -> `FAIL ...: Spieler-HP waehrend
  der Szene nicht 100`, `exit=Process terminated due to timeout`, "2 Fehler" (`qa1/alt_riegel/durch_killat.log`). Der
  neue Riegel loescht genau diese Variable (oben) — die Luecke ist geschlossen, auch fuer klein geschriebene Namen.
* Linux (Release-Container re15-linux-build:deb11, cmake 3.28.6) und Windows: der Block aus test_r39_elzazombies.cmake
  (per awk ausgeschnitten, `qa1/linux_env/block.cmake`) mit `env RE15_NOAUDIO=1 re15_klein=2 Re15_Mix=3 ANDERES=4` ->
  "geerbte Umgebung geloescht: RE15_NOAUDIO;re15_klein;Re15_Mix", danach alle drei leer, ANDERES=[4], Kind-Umgebung ohne
  RE15-Treffer. test_r39_elzazombies.c unter gcc 10.2.1 `-std=c11 -Wall -Wextra -Werror=implicit-function-declaration
  -fsyntax-only`: rc 0, keine Warnung aus der Datei selbst (unsetenv deklariert ueber _DEFAULT_SOURCE).
* Messkopien: nach dem Lauf liegt in build/platform/pc nur re15_pc.exe (keine re15_pc_r39elzazombies_*.exe).

### M6 Knall im Durchbruch 1041 hoerbar (Ton-Mitschnitt, eigene Auswertung)
Laeufe j_durch_laut / j_durch_stumm (Karte wie g_durch_einer, `RE15_AUDIO_CAP_SYNC=cap.raw`, einmal mit
`RE15_R39Q_KNALL_STUMM=1`, Ende 300#1041; beide `Signal (5,29): Knall wie ROOM1030 (Knall-Bank Satz 1)`, `Folge #4 K
ROOM1041 T2 1`). Differenz der Mitschnitte (je Bild 1470 Stereo-Samples, eigenes Python-Skript): GENAU EINE Gruppe,
Bilder 597..635 (39), Differenz-RMS max 4657 (= derselbe Satz-1-Knall wie im 1061-Schritt der Kette, RMS 4657 im
Riegel knall); sonst bitgleich (890 Bilder). Der Knall vor dem Rolltor ist also hoerbar, nicht nur geloggt.

### M7 Durchbruch bei SCHON OFFENEM Rolltor ((4,5)=(4,4)=1; Elza hat das Rolltor auf dem Weg zu Roy selbst geoeffnet)
Lauf k_durch_tor_offen: Karte 1061 wie g, lebend 125 + 127, dazu `4:5;4:4`, FRAMEDUMP 10-370/40, Ende 400#1041:
`Durchbruch 1041: 2 Zombies aus 1061 (nicht besiegt)` -> `Durchbruch 1041 startet (Programm G): ... Rolltor schon offen`
-> Knall (K T2 1) -> `Durchbruch Bild 201: alle durch` -> Bit 129 13004 -> 7009, Bit 131 14897 -> 8122, HP min 100 ->
`Durchbruch beendet, Steuerung frei`. Bild `qa1/k_1041_offen.png`: die Zombies kommen aus dem dunklen Gang durch die
offene Toroeffnung auf die Kamera zu; Bild 370 Cut 5: Elza an der 1061-Tuer, links ein Raum-Zombie = ROOM1041 main00
@0x0124C `44 02 16 0d 00 00 01 16 ef 9b .. 8a d4` (Bit 22 bei (-25617,-11126), Zweig Ck(4,5)=1 @0x01220) = Original-
Raumzustand bei offenem Rolltor, in der Szene hinter der Kamera angehalten (`angehalten 1`), danach frei — kein Effekt
der Spur.

### M8 Kette mit NULL Ueberlebenden (7:224..227 tot)
Lauf l_kette0: `Umzug: 10E1 lebend 0 -> 1061` -> `Folge #1 K ROOM1041 T61 1` (gleich Knall Satz 1) -> `DOOR FIRE ...
target_cut=12` -> ROOM1031 (S 3 Kriecher, C 12) -> ROOM1041 `Rueckkehr beendet, Steuerung frei`. Kein 1061-Schritt:
wer im Medical Room keinen am Leben laesst, bekommt keine Zombies im Flur.

## Gates

### RE-Gate: Stichprobe selbst disassembliert (re15_disasm.py / re2_disasm.py der Baum-Kopie, PSX.EXE sha256 e6b340fd...)
1. PSX.EXE FUN_80039e7c (Navigator): @0x80039f14/@0x80039f28 `jal 0x8003a0fc` -> `sb v0,-32116(at)` = Zone der Entitaet
   (0x800b828c, Lage +0x34/+0x3c), @0x80039f30/@0x80039f3c Zone des Zielpunkts (0x800b8290); @0x80039fbc
   `bne a0,a1,0x80039ffc`; bei GLEICHER Zone @0x80039fd0/@0x80039fe0 `sh s1,444(v0)` / `sh s0,446(v1)` = Ziel der
   Entitaet direkt der uebergebene Punkt, `ori v0,zero,0x1`. Behauptung "eine BLK-Zone -> Ziel = Spielerlage" stimmt.
2. PSX.EXE FUN_8003ecec @0x8003ed74 `sw zero,4136(at)` (at=0x800b0000 -> 0x800b1028); Flag-Bank-Tabelle @0x80074664[5] =
   0x800b1028 (selbst dekodiert: [3] 0x800b0ff8 ... [5] 0x800b1028 ... [9] 0x800b1078). Bank 5 Wort 0 (Bits 0..31) wird beim
   Raum-Init geloescht -> (5,28)/(5,29)/(5,30) sind Raum-Scratch. Stimmt.
3. PSX.EXE Sce_em_set-Handler: @0x800420f4 `addiu s2,a1,2` (s2 = pc+2), @0x800421c8 `lbu v0,2(s2)` (= pc[4]) ->
   @0x800421d0 `sb v0,130(s0)` (Entity +0x82); dazu @0x80042120 `lbu a1,7(a1)` / @0x80042124 `ori v0,zero,0xff` /
   @0x80042128 `beq` / @0x80042130 `jal 0x8004efe4` = Tot-Bit-Gate pc[7]. Etage aus pc[4] und Gate stimmen.
4. PSX.EXE LAB_80040414 (Cut_replace): Schleife @0x8004044c..@0x800404a8 ueber 20-Byte-Saetze: Byte == pc[1] -> pc[2],
   Byte == pc[2] -> pc[1] (`sb a1,0(v1)` / `sb a2,0(v1)`, dasselbe fuer +1), Ende bei 0xff = TAUSCH. Begruendung "Bruch-
   Zeilen bei (4,15)=1 nicht ein zweites Mal" stimmt.
5. RE2 EMZ0.BIN (info/re2leon/COMMON/BIN): @0x80100aa8 `lhu v0,270(s2)` / @0x80100aac `addiu v1,zero,6` / @0x80100ab0
   `andi v0,v0,0x3f` / @0x80100ab4 `bne v0,v1,0x80100ac0` / @0x80100ab8 `addiu v0,zero,257` (Delay-Slot) / @0x80100abc
   `sw v0,4(s2)` = Deskriptor 6 -> Gang 0x0101 (das zitierte "@0x80100ab8-abc" liegt in RE2, NICHT in RE1.5 STAGE1.BIN —
   dort steht `lw v0,392(v1)`; der Code nennt es korrekt "RE2-KI"). Stimmt. STAGE1.BIN @0x80100f40/@0x80100f44 `ori
   v0,zero,0x201` / `sw v0,4(v1)` (RE1.5-Durchbruchform) stimmt ebenfalls.
6. RDT-Bytes selbst gelesen (shared_assets = info/Re1.5/PSX, sha256 gleich): ROOM10C1 @0x00E42 `3b 00 02 31 ... 90 65 e0 e3
   d4 62 00 08 00 06 02 04` (Ziel 06, Cut 2, Etage 4, (26000,-7200,25300), Gier 2048); ROOM10E1 @0x00D56/6A/7E/92 Typ 0x10
   grid 0x06 Bits e0..e3, @0x00DBE.. grid 0x02; ROOM1041 sub08 @0x019DC..@0x01A53 Byte fuer Byte = k_p_1041_tor (ohne
   @0x019FC `2e 01 00 00 40 00 09 3d 90 9d 98 fe` = Elzas Drehung), @0x01118 `2c 04 04 31 00 00 70 9a 80 c1 34 08 98 08`
   (-26000,-16000,2100,2200), @0x01190 Rolltor x `90 9d` z `98 fe` (-25200,-360), @0x01218 `24 12 05 00`, sub11
   @0x0222C `22 02 07 00 22 01 1b 00 3c 01 2e 01 00 00 42 00`; ROOM1031 sub08 @0x02850..@0x028C9 = k_p_1031_bruch/
   _kriechen (Gosub-Folge mit Leons Verdichtung, s. Hinweis H5).
7. Guess-Tells im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|geschaetzt`): keine (nur
   "xxx1" als Raum-Muster). Einziger Env-Schalter `RE15_R39Q_KNALL_STUMM` = Mess-Haken, kein Abschluss-Ausweg.

### Zensus (selbst, unabhaengig vom Riegel)
`scd_dump_room.py` ueber alle 144 RDTs STAGE1-3 (706 Sce_em_set, 177 verschiedene pc[7] inkl. 0xff): Bits 124..131, 133,
144, 145 kommen weder als Sce_em_set-Tot-Bit noch in Ck/Set Bank 7 vor. Port-Programme mit Literal-Saetzen (408 Dateien
engine/src, gen, include, ohne elzazombies): keine Ueberschneidung. Leons Konsum von (5,28)/(5,29)/(5,30) laeuft nur bei
Leons Kettenzustand != AUS bzw. im 1040-Schritt (irons_tod_1150.c:1338-1357, :1252-1258) — kein Doppelverbrauch.

### Vertrag (VERTRAG.md 1.1-1.3, 2)
* Pfade: `git diff master...HEAD --name-only` (master ist seit dem Spurstart um den Doc-Commit 0a5ca4f2 weiter; der
  Zweig selbst aendert nur): analysis/befunde_runde39/Q_* + Q_belege/, engine/src/elzazombies_r39.c,
  include/re15_elzazombies_r39.h, tests/integration/test_r39_elzazombies.cmake, tests/unit/probes/r39_elzazombies.cmake,
  tests/unit/probe_r39_elzazombies_karte.c, tests/unit/test_r39_elzazombies.c + die Haken. Kein release/,
  platform/android/, shared_assets/PSX/, keine Test-CMakeLists.txt. OK.
* Gemeinsame Dateien nur kleine Haken mit "Runde 39 Spur Q": scd_room_setup.c (+1 include, +1 Installer-Zeile am Ende des
  Blocks nach Spur K), scd_vm.c (+1 include, +1 Ereignis-Zeile, sub01-Bedingung +1, +1 Takt-Zeile), platform/pc/main.c (+2
  im CONTINUE-Weg), test_r36_kartenbits.c (+1 include, +1 k_port-Zeile). OK.
* Bits: Bank 9 geschrieben nur Bit 95 (VERTRAG 1.1 "95 seit Nachtrag 10 an Q", in k_port[] eingetragen,
  unit_r36_kartenbits gruen); (9,97) im Spielcode nur gelesen (alle `re15_game_flag_set(9, 97, ..)` im Diff stehen im
  Unit-Riegel). Zone-7-Tot-Bits 124..131/133/144/145 frei (eigener Zensus oben). Bank-5-Bits 28/29/30 = Raum-Scratch
  (Init-Loeschung @0x8003ed74 selbst gelesen). Keine Nachrichten-Ids, keine Sprachdateien, keine Assets. OK.
* Riegel je Punkt vorhanden und messend: unit_r39_elzazombies_{zensus,kette_4,kette_0,kette_2,nach1061,nach1031,
  durch1041,ohne} (61 PRUEF-Aussagen ueber Lagen, Abstaende, Folgen, Bytes, Bits; kette_4 mit Gegenprobe "Modul aus"),
  integration_r39_elzazombies_{kette,knall,treppe,durch,ohne}. Selbst gefahren (feindliche Umgebung, N1): 13/13 gruen.
* VERTRAG Abschnitt 2 (neue Tests parallel-/Linux-fest, keine Umgebung erben): eigene Laufordner je Teil
  (`r39_elzazombies_wd_<teil>/<lauf>`), exe-Kopie mit Praefix je Teil und danach entfernt, Umgebung geloescht (N1,
  gemessen mit drei Schreibweisen + Gegenprobe alter Riegel), `cmake_minimum_required` im -P-Skript, `m` unter Linux,
  `_DEFAULT_SOURCE` fuer unsetenv, keine .exe-/Backslash-Pfade (Endung aus dem uebergebenen Pfad). OK — der Mangel 1 der
  Abnahme 0 ist behoben.

### Gezielte Gruppen der beruehrten Module/Raeume (keine volle Suite)
`ctest -R "kartenbits|cut1150|r38_montage|zombieaktiv|gegnerzahl_raum10[34]1|r34n_a_rolltor|r33_speichern|elza_vollstart|
r38_elzaintro|r39_elza_|r38_elzatoene|r38_tagebuch|r36_story_elza|^unit_r36_story|r36_elza_|elzaems_elza10c1|unit_montage_fx|
szene1150_knall|fenster_knall|unit_elza_zweig|map_speichern_laden|unit_scd" -j 2 --timeout 1500` (93 Tests: Leons Montage
1150/1130/1040/1030 inkl. integration_r35_cut1150 + integration_r38_montage, Rolltor 1040, Elza-Raeume 1031/1041/10C1,
Elza-Szenen, Tagebuch/Karte, Speichern, Bank-9-Riegel, SCD-VM-Opcodes) unter Fremdlast (CPU 100 %, Integrationsbaum
und Spur J liefen parallel): "100% tests passed, 0 tests failed out of 93" (04:53:46 Start, Total Test time 1097,92 s,
`qa1/ctest_gruppen.log`), darunter integration_r35_cut1150 (529 s), integration_r38_montage, integration_r37_zombieaktiv,
integration_r33_speichern, integration_elza_vollstart, integration_r36_story_elza, alle unit_r35_cut1150_* / unit_r38_montage_*,
unit_r36_kartenbits, unit_r34n_a_rolltor, unit_scd_*. Danach liegt in build/platform/pc nur re15_pc.exe.

## Urteile je Teilpunkt (gegen den Wortlaut, Nachtrag 11 und die Lesart in AUFTRAG.md eingerechnet)
* Q1 "einmal in den Flur hinein von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden":
  ERFUELLT. Montage-Schnitt ROOM1061 Cut 2 (2F-Absatz, Schild F2, 10C1-Tuer), nach dem Tuerknall stehen die Zombies an der
  10C1-Tuer und gehen in den Absatz hinein auf die Kamera zu (M1 Bild 10 -> 170). Zahl selbst gemessen mit EIGENEN Faellen:
  1 Ueberlebender -> `S ROOM1061 T105 1` (M1), 2 -> `S ... 2` (M2/M3), 0 -> kein 1061-Schritt (M8); die Riegel decken 4/2/0.
* Q2 "wenn man in ROOM 1061 die Etage von unten ueber die Treppe erreicht aber moeglichst weit weg an der Seite stehen":
  ERFUELLT. Echter Weg M2 (Kette -> Tuer 1041 -> 1061 -> Treppe A + B): die zwei Kopien stehen an ihren Westseiten-Lagen
  unter dem Fenster, unbewegt waehrend Etage 0/2, frei bei der Ankunft (25424,24568), Abstaende 5356/4591; Bild 625 zeigt
  Elza rechts an der Tuer, die Zombies links am anderen Ende des Absatzes.
* Q3 "das gleiche nochmal wie bei Leon ... falls die Zombies nicht besiegt wurden" + Nachtrag 11 "nicht in ROM 1071 ...,
  sondern ... wie bei leon im ROOM 1041": ERFUELLT in der Lesart der AUFTRAG.md (Orchestrator zu Nachtrag 11: Bedingung =
  die Zombies aus 10C1/1061, Form "Durchbruch durchs Rolltor beim naechsten Betreten von 1041, solange diese Zombies
  leben", Leons 1040 als Vorbild). Echter Weg M3: Kette -> 1061 -> zurueck nach 1041 -> Cut 1, Knall (hoerbar, M6),
  Rolltor faehrt mit den Original-Zeilen ROOM1041 sub08 hoch, beide nicht Besiegten kommen durch und naeher, HP 100,
  Steuerung an der Eintrittstuer. Bedingung gemessen: alle besiegt -> nichts (M4 f), einer lebt -> einer (M4 g), zwei ->
  zwei (M3), Rolltor schon offen -> sie kommen durch die offene Toroeffnung (M7). ROOM1071: kein Schritt (in keinem Lauf
  eine 1071-Zeile; unit kette_*: Tot-Bits 0xc6..0xca unberuehrt). Dem Nutzer im Dossier 7 in einfachen Worten erklaert.
* Q4 "im ROOM 1031 ueber die Absperrung/Das Rollo, so wie bei der cutscene": ERFUELLT. Montage-Schnitt 1031 Cut 12 mit den
  Original-Zeilen sub08 (Aot_reset/Se_on/Cut_replace/Sca_id_set @0x02858..@0x0287E bei (4,15)=0); Bild b_1031.png: das
  aufgebrochene Rollo, Kriecher kommen darunter durch auf die Kamera zu (Bits 133/144/145 9790 -> 7561, 10524 -> 5579,
  9522 -> 4765), die Raum-Zombies dahinter rucken nach, HP 100.
* Q5 Ausloeser/Nebenwirkungen (steigende Flanke (9,97), Laden keine Flanke, Leon unveraendert): ERFUELLT. Flanke startet
  die Kette (M1-M3, M8); Leon mit (9,95) + Flanke in 1060 und Tuer nach 1040: keine Zeile (M5 h); Laden in 1041 mit
  (9,95)/(9,97): nichts (M5 i); integration_r39_elzazombies_ohne gruen (Leon 1040 + Flanke; Laden mit (9,97)).
* N1 Nachbesserung 1 (Mangel 1 der Abnahme 0, VERTRAG Abschnitt 2): BEHOBEN. Riegel mit feindlicher Umgebung in drei
  Schreibweisen 13/13 gruen, jede Variable geloescht; Gegenprobe mit dem alten Riegel faellt mit derselben Variable
  (Spieler stirbt, Zeitueberschreitung); Linux-Block gemessen; keine Messkopien bleiben liegen.

## Hinweise / Entscheidungsfragen (keine Maengel)
* H1 (wie Abnahme 0 H1, offen beim Orchestrator) Der 1041-Durchbruch prueft (9,97) nicht: er kommt auch NACH Johns
  Rettung, solange 1061-Kopien leben und Elza 1041 im Suedteil betritt (M4/M7 ohne (9,97) gemessen). Mit "falls die
  Zombies nicht besiegt wurden" vereinbar; ob das zu "Der Rest bleibt normal" passt, entscheidet der Nutzer.
* H2 Sichtbarkeit: auf dem ueblichen Panik-Weg (1041 -> 1061 hinauf -> 1121 -> 1131 -> 1141) kehrt Elza nicht nach 1041
  zurueck — den Rolltor-Durchbruch sieht nur, wer vor den Zombies im Treppenhaus zurueck nach 1041 geht (oder spaeter
  wiederkommt). So in der AUFTRAG-Festlegung ("beim naechsten Betreten") und im Dossier 7 erklaert. Erwartet der Nutzer den
  1041-Schnitt IN der Montage (Leons 1040-Schritt ist Teil seiner Montage, mit den Raum-Zombies hinter dem Rolltor), waere
  das eine neue Vorgabe.
* H3 Ankunft in 1061 von OBEN (Rueckweg aus 1121): die Kopien stehen an der Westseite nahe dem Ende von Treppe A. Nicht
  verlangt (Dossier 7 Entscheidungsfrage).
* H4 Integration (in diesem Baum nicht pruefbar, Spur P/R nicht enthalten): (a) P setzt (9,97) am Ende des 1041-Rufs;
  Q startet erst bei frei() (keine Szene, player_mode != 2, kein Raumwechsel, kein angeforderter Kartenhinweis) — die
  echte Folge "Satz -> Karte -> Panikmusik -> Montage" ist im Integrationsbaum mit P zu messen; (b) P's 1041-Szene darf
  bei der Rueckkehr der Montage (Raum 1041 wird neu geladen) nicht neu starten; (c) Ereignis 30 nutzt auch Spur R (nur
  ROOM1121) — Textkonflikt in scd_vm.c (beide Zeilen hinter Spur I), keine Wirkungsueberschneidung (jede Abfrage prueft
  Raum UND Zustand).
* H5 ROOM1031-Schritt: die vier Gosub sub09 laufen wie in Leons Montage unmittelbar hintereinander (Original sub08:
  `18 09` @0x0288A, Sleep 5 @0x0288C, `18 09` @0x02890, ..., `18 09` @0x028A0, Sleep 10 @0x028A2, `18 09` @0x028A6) und die
  Original-Fortsetzung Cut 11/Sleep 200/Cut 5 @0x028B4..@0x028BA entfaellt — uebernommene PORT-WAHL aus Leons Montage
  (Runde 35 L §8, vom Nutzer abgenommen), im Code als solche benannt. Kein neuer Rateschritt.
* H6 Robustheit (Kleinigkeit): bricht ein Teil des Integrationsriegels hart ab (FATAL_ERROR in lauf(): Kartenwerkzeug
  oder fehlendes debug.log), bleibt dessen Messkopie liegen (file(REMOVE) steht am Skriptende). Im Normal- und im
  Fehlerfall ueber `fail()` wird sie entfernt (gemessen: 0 Kopien nach 13/13). `cmake_minimum_required(VERSION 3.16)` bei
  `file(COPY_FILE)` (ab CMake 3.21) — wie die uebrigen Riegel; Projekt verlangt ohnehin >= 3.21.
* H7 (wie Abnahme 0 H5) durchbruch_1041 zieht die Kopien um (umzug_1041), BEVOR starten() einen Ereignis-Faden hat; ohne
  freien Platz (14 Plaetze) waeren sie ohne Szene nach 1041 umgezogen. Praktisch nicht erreichbar.

## Maengel
Keine. (Mangel 1 der Abnahme 0 — VERTRAG Abschnitt 2, Riegel erbten die Umgebung des Aufrufers — ist behoben: N1.)

## Gesamturteil
BESTANDEN. Q1-Q5 erfuellt (Messungen M1-M8 an der exe, Bildtafeln `qa1/a_1061*.png`, `qa1/b_1031.png`,
`qa1/d_1061.png`, `qa1/e_1041.png`, `qa1/k_1041_offen.png`, Logs `qa1/runs/<lauf>/debug.log`), N1 behoben,
RE-Gate/Vertrag/Zensus halten, eigene Riegel 13/13 (feindliche Umgebung) und 93 Gruppentests gruen.
Fuer den Orchestrator offen: H1/H2 (Entscheidungsfragen an den Nutzer) und H4 (Messung mit Spur P im Integrationsbaum).
