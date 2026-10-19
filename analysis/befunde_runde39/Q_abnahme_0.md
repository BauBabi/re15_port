# Runde 39 Spur Q "elzazombies" — Abnahme 0 (unabhaengig)

Stand: Abschluss (fortlaufend geschrieben). ERGEBNIS: NICHT BESTANDEN — alle Nutzer-Punkte am Spiel erfuellt, aber ein
Vertrags-Gate (VERTRAG Abschnitt 2, Test-Riegel erbt die Umgebung des Aufrufers) haelt nicht: Mangel 1.
Baum `.claude/worktrees/r39_elzazombies`, Zweig `r39/elzazombies`, HEAD 45978a7d (Code-Stand = ea6e4cab/35e19321/b7f4cd97,
Dossier 0f730673/45978a7d). Gebaut mit `bash re15_port/tools/local_build.sh configure` + `build` (ninja: no work to do,
elzazombies_r39.c 02:17:03 -> .obj 02:17:17 = aktueller Stand).

## Nutzer-Punkt (AUFTRAG.md Nachtrag 10 Absatz 2, ersetzt durch Nachtrag 11 fuer 1071 -> 1041)
> "Und auch hier sollen jetzt Zombies durchbrechen - einmal in den Flur hinein von 10C1 aus - so viele wie im medical
> room - also 10E1 am Leben gelassen wurden., die sollen dann, wenn man in ROOM 1061 die Etage von unten über die Treppe
> erreicht aber möglichst weit weg an der Seite stehen. Dann das gleiche nochmal wie bei Leon in den ROOM 1071, falls die
> Zombies nicht besiegt wurden und im ROOM 1031 über die Absperrung/Das Rollo, so wie bei der cutscene."
> Nachtrag 11: "Ich korrigiere, die Zombies sollen nicht in ROM 1071 noch durchbrechen, sondern dann noch wie bei leon im ROOM 1041."

Teilpunkte fuer das Urteil:
- Q1 Durchbruch 10C1 -> Flur (Treppenhaus 1061), Zahl = in 10E1 am Leben gelassen
- Q2 1061: bei Ankunft von unten ueber die Treppe moeglichst weit weg an der Seite
- Q3 1041 wie bei Leon (Rolltor), falls nicht besiegt (Nachtrag 11); 1071 unberuehrt
- Q4 1031 ueber die Absperrung/das Rollo wie in der Cutscene
- Q5 Ausloeser = steigende Flanke (9,97) (Orchestrator-Lesart), Laden keine Flanke, Leon unveraendert

## Messprotokoll

Werkzeug: Messkopie der exe je Lauf `re15_pc_qa0abn_<lauf>.exe` (im build/platform/pc, nach dem Lauf geloescht), Spielstand
mit `probe_r39_elzazombies_karte` (Elza, CONTINUE: RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0, RE15_NO_INTRO=1,
RE15_WINDOW_SCALE=1), Laufordner `scratchpad/qa0/runs/<lauf>` (Skript `scratchpad/qa0/lauf.sh`). `scratchpad` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`
(Sitzungsordner der Abnahme; Bilder/Logs liegen dort, committet wird nur dieser Bericht). Spur P ist in diesem Baum
nicht enthalten: (9,97) wird wie in den Riegeln der Spur mit `RE15_SET_FLAG_AT=9:97@<bild>` gesetzt (= P's Setzpunkt).

### M1 Kette mit DREI Ueberlebenden in 10E1 (eigene Zahl, nicht die der Riegel: 7:224 = 0xe0 tot)
Lauf m1_kette3: Karte `1041;-21008;0;-13134;2112;0;3:125;3:193;3:116;7:224`, RE15_NOAUDIO=1, RE15_SET_FLAG_AT=9:97@200,
RE15_EXIT_AT=400#1041. debug.log:
```
[setflag-at] Frame 200: flag(9,97/0x61) = 1
[elzazombies-r39] Flanke (9,97) 0->1 in ROOM1041 Takt 203 -> Montage ausstehend
[elzazombies-r39] Umzug: 10E1 lebend 3 -> 1061; (9,95)=1
[elzazombies-r39] Folge #1 K ROOM1041 T61 0                       (Tuerknall)
[aot] DOOR FIRE slot=20 ... target_cut=2 spawn=(20200,-7200,20400)
[room] PC loaded room1061.rdt
[elzazombies-r39] Folge #3 S ROOM1061 T105 3                       <- 3 Kopien = 3 Ueberlebende
[elzazombies-r39] Folge #4 C ROOM1061 T105 2
[elzazombies-r39] Folge #5 K ROOM1061 T237 1                       (Knall Satz 1)
ROOM1061 Cut 2 Bild 176: Bit 125 6421 -> 4846, Bit 126 6638 -> 5467, Bit 127 7547 -> 5585 (Bit 124 = Kopie der toten 0xe0 fehlt)
[room] PC loaded room1031.rdt ; Folge S ROOM1031 T281 3 ; C ROOM1031 T281 12
ROOM1031 Cut 12 Bild 357: Kriecher 133/144/145 9790 -> 7561, 10524 -> 5579, 9522 -> 4765; Rollo-Zombies Bits 0..5 naeher; HP min 100
[room] PC loaded room1041.rdt -> Rueckkehr (Programm D) -> "Rueckkehr beendet, Steuerung frei"
```
Bilder (mit Ton, Knall-Tonbank geladen): Lauf m1b_bild1061 (`RE15_FRAMEDUMP=5-175/17`, Ende 175#1061) -> `scratchpad/qa0/m1b_1061.png`:
2F-Absatz (Fenster, Schild F2, 10C1-Tuer rechts), die drei stehen an der Tuer und gehen nach links zur Kamera; die Tuer
selbst bleibt zu (Form Leons 1130-Schritt: Knall vor dem Schnitt, dann stehen sie schon drin). Lauf m1c_bild1031
(`2-352/35`, Ende 352#1031) -> `scratchpad/qa0/m1c_1031.png`: Cut 12 zeigt das aufgebrochene Rollo (verbogene Stangen,
Truemmer), Zombies dahinter, Kriecher kommen darunter durch auf die Kamera zu.

### M2 Kette mit NULL Ueberlebenden (7:224..227 tot)
Lauf m2_kette0: `Umzug: 10E1 lebend 0 -> 1061` -> `Folge #1 K ROOM1041 T61 1` (gleich Knall Satz 1) -> ROOM1031 (S 3, C 12)
-> ROOM1041 Rueckkehr. Kein 1061-Schritt (keine Zombies im Flur, wenn in 10E1 keiner am Leben gelassen wurde).

### M3 ECHTER WEG Kette -> Tuer 1041 -> 1061 -> Treppe A + B von unten (Q2)
Lauf m3_treppe_echt: Karte `1041;-20700;0;-13000;0;0;3:125;3:193;3:116` (alle 10E1 leben), RE15_SET_FLAG_AT=9:97@100,
RE15_INPUT_SCRIPT_BASIS=spiel START=60, Skript = Treppen-Skript des Riegels mit Vorlauf `W4` (die Tasten fallen in
1041/1061/1031 der Montage in den Riegel, player_mode 2; erst nach der Rueckkehr wirkt `A` an der 1061-Tuer), FRAMEDUMP
400-900/25, Ende 900#1061. debug.log (gekuerzt):
```
Flanke (9,97) 0->1 in ROOM1041 Takt 103 ; Umzug: 10E1 lebend 4 -> 1061 ; S ROOM1061 T105 4 ; C 2 ; ... ROOM1031 C 12 ...
ROOM1041: Rueckkehr beendet, Steuerung frei
[aot] DOOR FIRE slot=2 rect=(-19650,-13000,hw=750,hh=1100) target_cut=7 spawn=(26000,0,25300)
[room] PC loaded room1061.rdt
ROOM1061: Nachspawn 1061 (Programm E): Kopien an der Westseite, gehalten bis Etage 4
ROOM1061: Kopie Aktor 1 Bit 124 bei (20218,26533), Abstand zur Spielerin 5565
ROOM1061: Kopie Aktor 2 Bit 125 bei (20218,25732), Abstand zur Spielerin 5335
ROOM1061: Kopie Aktor 3 Bit 126 bei (21019,26532), Abstand zur Spielerin 4823
ROOM1061: Kopie Aktor 4 Bit 127 bei (20916,25341), Abstand zur Spielerin 4574
ROOM1061: Spielerin auf Etage 4 bei (25424,24568) -> Kopien frei, kleinster Abstand 4574     (~Bild 610)
[walk] F720 cut=2 mo=211 ... pl pos=(25424,-7200,24568)  ; ab ~F760 Griff (pl pos/rot aendern sich)
```
Bild `scratchpad/qa0/m3_1061.png`: Bild 500-600 Aufstieg (Etage 0 -> 2 -> 4), Bild 625 = Ankunft: Elza rechts an der
10C1-Tuer, die VIER links unter dem Fenster (Westseite, am weitesten weg); 650-775 kommen sie heran, 750/775 Griff.
Waehrend Etage 0 und 2 standen sie (Lage bei der Freigabe = Spawn +-19, k_lagen_nach (20200,26550)...).

### M4 ECHTER WEG Kette -> 1061 -> zurueck durch die Tuer nach 1041 = Rolltor-Durchbruch (Q3, Nachtrag 11)
Lauf m4_durch_echt: Karte wie M3, mit Ton, Skript `W4,A0.2,W1,R0.7,U0.2,A0.2,W20` (1041-Rueckkehr: `A` -> Tuer 1061;
1061: umdrehen, `A` -> Tuer 1041), FRAMEDUMP 10-430/30, Ende 430#1041. debug.log:
```
... Kette wie M3 ... ROOM1041: Rueckkehr beendet, Steuerung frei
[aot] DOOR FIRE slot=2 ... spawn=(26000,0,25300) ; [room] PC loaded room1061.rdt ; Nachspawn 1061 (Programm E)
[aot] DOOR FIRE slot=2 rect=(27100,25400,hw=500,hh=1100) target_cut=5 spawn=(-21008,0,-13134)
[room] PC loaded room1041.rdt
Durchbruch 1041: 4 Zombies aus 1061 (nicht besiegt) -> hinter das Rolltor
Folge #2 S ROOM1041 T0 4 ; Folge #3 C ROOM1041 T0 1
Durchbruch 1041 startet (Programm G): Eintritt (-21008,0,-13134) Gier 2112 Cut 5, Rolltor faehrt hoch
Signal (5,29): Knall wie ROOM1030 (Knall-Bank Satz 1) ; Folge #4 K ROOM1041 T2 1
Durchbruch Bild 266: alle durch das Rolltor -> (5,30)=1
ROOM1041 Cut 1 Bild 356: Bit 128 12391 -> 6201, Bit 129 13004 -> 8316, Bit 130 13941 -> 6828, Bit 131 14897 -> 9031
   (Raum-Records Bit 20/21 kommen mit durch 11089 -> 8246 / 14047 -> 8758; Bit 22..24 laufen vom Tor weg)
ROOM1041 Cut 1 Ende Bild 356: Spieler-HP min 100 ; Durchbruch beendet, Steuerung frei (Haltebits geloest)
[walk] F390 cut=5 ... pl pos=(-21008,0,-13134) rot=2112
```
Bild `scratchpad/qa0/m4_1041.png`: Cut 1 Rolltor zu (10/70), faehrt hoch (130-190), die Zombies stehen dahinter und
kommen auf die Kamera zu (190-340), Bild 370/400 Cut 5: Elza an der 1061-Tuer mit Steuerung.

### M5 Kette mit schon gebrochenem Rollo ((4,15)=1 im Spielstand = Elza hat die Original-Rollo-Szene 1031 schon gesehen)
Lauf m5_rollo_schon: wie M1 + `4:15`, FRAMEDUMP 2-352/50, Ende 352#1031. Folge wie M1 (1061 S 4 C 2, 1031 S 3 C 12);
der Bruch-Teil (Aot_reset/Se_on/Cut_replace) laeuft NICHT ein zweites Mal (sub00 @0x020E0 hat ihn beim Raumaufbau schon
ausgefuehrt; Cut_replace ist ein Tausch). Bild `scratchpad/qa0/m5_1031.png`: Cut 12 zeigt das aufgebrochene Rollo, die
Kriecher kommen durch, ein Raum-Zombie steht schon in der Halle (sub00-Zweig (4,15)=1). Original ROOM1031 sub08
(@0x02850..@0x028CA, eigener Dump scd_dump_room.py) setzt ausser (4,15) (sub01 @0x02284) nur Raum-Bits (Bank 1/2/5) —
die Montage verbraucht also keinen weiteren Story-Zustand.

### M6/M7 "falls die Zombies nicht besiegt wurden" an der exe (Tuer 1061 -> 1041, Zustand nach der Montage)
* m6_durch_besiegt: Karte 1061 (26200,0,25300) Gier 0 Cut 7 + (9,95), 10E1-Quellen tot, ALLE 1061-Kopien 124..127 tot,
  1041-Kopien still; `A0.2` -> `DOOR FIRE slot=2 ... spawn=(-21008,0,-13134)` -> ROOM1041 bis Bild 300: KEINE
  Durchbruch-Zeile, keine Szene (alle besiegt = nichts).
* m7_durch_zwei: wie m6, aber 125 und 127 leben: `Durchbruch 1041: 2 Zombies aus 1061 (nicht besiegt)`, S 2, C 1, Knall,
  `Durchbruch Bild 297: alle durch`, Bit 129 13004 -> 5441, Bit 131 14897 -> 8890, HP min 100, Steuerung frei.
  Zahl der Durchbrechenden = Zahl der in 1061 nicht Besiegten.

### M8 Durchbruch bei SCHON OFFENEM Rolltor ((4,5)=(4,4)=1 — der wahrscheinliche Stand, wenn Elza aus 1021 kommt)
Lauf m8_durch_tor_offen: Karte wie m7 mit allen vier 1061-Kopien lebend + `4:5;4:4`, mit Ton, FRAMEDUMP 10-400/30.
`Durchbruch 1041 startet (Programm G): ... Rolltor schon offen` -> Knall (K T2 1) -> `Durchbruch Bild 233: alle durch`
-> Bits 128..131 12391 -> 5404, 13004 -> 5153, 13941 -> 8574, 14897 -> 7599, HP min 100 -> Steuerung frei.
Bild `scratchpad/qa0/m8_1041.png`: Cut 1, offenes Rolltor, die vier kommen aus dem dunklen Gang auf die Kamera zu;
Bild 340 Cut 5: Elza an der 1061-Tuer, links ein Raum-Zombie (Record Bit 22, im Suedteil gespawnt = Original-Raumzustand
bei (4,5)=1; waehrend der Szene hinter der Kamera angehalten, `angehalten 1`, danach frei) — kein Effekt der Spur.

### M9 Test-Riegel erbt die Umgebung des Aufrufers (VERTRAG Abschnitt 2)
`RE15_NOAUDIO=1 ctest --test-dir re15_port/build -R "^integration_r39_elzazombies_knall$"` ->
`***Failed` "FEHLER: Mitschnitt fehlt (.../laut/cap.raw / .../stumm/cap.raw)", `auswerten_ton: 1 Fehler`
(`scratchpad/qa0/knall_envleak.log`). Ohne die Variable gruen (69,6 s, `scratchpad/qa0/knall_v.log`: Knall Bilder
624..665 RMS 6567, 806..849 RMS 4657). test_r39_elzazombies.cmake loescht geerbte RE15_*-Variablen NICHT (spiel_lauf.cmake
setzt nur die uebergebenen); die Riegel der Spuren A und K tun es (test_r39_stimmen.cmake:31-37,
test_r39_heliport1171.cmake:47-53 `cmake -E environment` + `unset(ENV{RE15_...})`).

### Nachrechnung "moeglichst weit weg" (Q2) aus der SCA (selbst geparst)
ROOM1061 SCA @0x004A0 (RDT+0x20), Band 4 = floor-Byte 0x43 @0x00584..@0x005C0: Waende x 26900.. / z 26950.. / x ..19800 /
z ..18200, Treppenkern x 19600..23400 z 20800..24900, Lauf-B-Block x 22900..27100 z 20000..24100. Kern und Block reichen
von Wand zu Wand (19600 < 19800, 27100 > 26900): die freie Band-4-Flaeche zerfaellt in den Nord-Absatz (z 24100/24900 ..
26950, Tuer 10C1, Treppenende) und einen davon GETRENNTEN Suedstreifen (z 18200..20000/20800, Parkplatz der Montage).
Vom Treppenende (25023,24568) aus ist der weiteste erreichbare Punkt (Radius 400 an den Waenden) x 20200 / z 26550 =
5214 — genau die erste Lage der Spur (k_lagen_nach); die vier Lagen sind die Westseite des Nord-Absatzes. Bestaetigt.

## Gates

### RE-Gate (Stichprobe, selbst disassembliert / Bytes selbst gelesen)
1. STAGE1.BIN @0x80100450..@0x8010045c (re15_disasm.py): `lbu v0,9(a0)` / `andi v0,v0,0x20` / `bne v0,zero,0x80100658`
   in FUN_80100424 (Kopf: `addiu sp,sp,-24`, Pause-Gate @0x8010043c). Der Sprung ueberspringt den ganzen KI-Block
   0x80100464..0x80100654 (SquareRoot0 @0x801004ac, FUN_80039e7c @0x80100538, Dispatch @0x8011f7b4 `jalr` @0x80100588,
   jal 0x800437d4 @0x80100650) -> "entity+0x9 Bit 0x20 = Zombie-Takt aus" stimmt. OK.
2. STAGE1.BIN @0x80100f40/@0x80100f44: `ori v0,zero,0x201` / `sw v0,4(v1)` (v1 = g_entity cur) = Durchbruch-Form. OK.
3. PSX.EXE @0x80042120..@0x80042138: `lbu a1,7(a1)` (pc[7]), `ori v0,zero,0xff` / `beq a1,v0` (0xff = ohne Gate),
   `jal 0x8004efe4` (Bit-Test: `srl v1,a1,5`/`sll v1,v1,2`/`lui v0,0x8000`/`srlv`/`and`), `beq v0,zero` = Tot-Bit-Gate. OK.
4. RDT-Bytes (info/Re1.5/PSX/STAGE1, sha256 = shared_assets): ROOM1031 sub08 @0x02850 `22 02 07 01 22 01 1b 01`,
   @0x02858 `46 11 01 31 01 00 ff ff 00 00`, @0x02862 `36 02 0c 00 00 00 cc dd f8 f8 f0 a7`, @0x0286E `4b 00 09 4b 03 0a
   4b 04 0b 4b 06 0c`, @0x0287A `37 02 06 f7 37 03 06 f7`, @0x02896 `29 0c` = k_p_1031_bruch/kriechen. ROOM1041 sub08
   @0x019DC..@0x01A53 Byte fuer Byte = k_p_1041_tor (ohne @0x019FC..@0x01A07 = Elzas Drehung), @0x01118 Schalter-Rechteck
   (-26000,-16000,2100,2200) -> Mitte (-24950,-14900), @0x01190 Rolltor (-25200,0,-360), @0x01218 `24 12 05 00`.
   ROOM10C1 @0x00E42 Tuer -> Raum 06 Cut 02 Etage 04 Spawn (26000,-7200,25300) Gier 2048. ROOM10E1 @0x00D56/6A/7E/92
   Sce_em_set Typ 0x10 grid 0x06 pc[7] = e0/e1/e2/e3 (Else @0x00DBE grid 0x02). ROOM1061 RID Cut 2 @0xA0 =
   (20199,-12628,21273)->(24670,-7088,27767); Tueren @0x00D12/32/52 -> 0x12 / 0x0c / 0x04 (Spawn (-21008,0,-13134) Gier
   2112 Cut 5). Alles wie zitiert.
5. Guess-Tells im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME`): keine Treffer. Einziger Env-
   Schalter `RE15_R39Q_KNALL_STUMM` = Mess-Haken (Knall verbrauchen, nicht spielen), kein Abschluss-Ausweg.
6. Konstanten ohne Original-Adresse sind als PORT-WAHL/NUTZER-VORGABE mit Regel gekennzeichnet: Lagen 1061 (Tuer-Packung,
   Weit-weg-Packung vom gemessenen Treppenende (25023,24568) im 800er-Abstand, Gier = atan2 zur Ankunft: 254/157/298/122
   nachgerechnet), Parkplatz 1061 (Tiefe -718 hinter Cut 2, nachgerechnet -719), 1041-Gassen/Parkplaetze/Zeiten =
   Leons Konstanten (re15_irons_tod.h RE15_IT_PARK_1040_*, RE15_IT_TOR_Z_1040, RE15_IT_1040_KAPPE, irons_tod_1150.c
   Sleep 60/45/130/34/90/120, Kriecher (-9500/-7300/-11500, -24800 bzw. -19000/-18500/-18800)) — gegengelesen, identisch.

### Zensus (selbst, unabhaengig vom Riegel)
Alle 144 RDTs STAGE1-3 mit scd_dump_room.py: 177 verschiedene Sce_em_set-Tot-Bits; die gewaehlten Zone-7-Bits 124..131,
133, 144, 145 kommen weder als Sce_em_set pc[7] noch in Ck/Set Bank 7 vor. Port-Programme (OP_EM in engine/src) belegen
40..43, 46..49, 75..79, 83, 84, 90, 91, 95 (Leon) — keine Ueberschneidung. Bank 5 Bits 28/29/30: in ROOM1041 (0..20,
32..35), ROOM1031 (0..20, 32..34), ROOM1061 (keins) frei. Tuer-Slot 20: 1041 nutzt 0..8, 1061 0..10+16, 1031 0..19 (Leon
nutzt dieselbe 20 in 1030). Ereignis 30 > Zahl der Subs (1041: 12, 1061: 2, 1031: 16).

### Vertrag (VERTRAG.md 1.1-1.3, 2)
* Bits: geschrieben nur Bank 9 Bit 95 (VERTRAG 1.1 "95 seit Nachtrag 10 an Q", in k_port[] eingetragen,
  test_r36_kartenbits.c +2 Zeilen); (9,97) nur gelesen (kein `re15_game_flag_set(9,97)` im Diff). Zone-7-Tot-Bits
  124..131/133/144/145 frei (eigener Zensus oben). Bank 5 Bit 28/29/30 = Raum-Scratch, frei. Keine Nachrichten-Ids, keine
  Sprachdateien. OK.
* Pfade (`git diff master --name-only`): nur analysis/befunde_runde39/Q_*, engine/src/elzazombies_r39.c,
  include/re15_elzazombies_r39.h, tests/integration/test_r39_elzazombies.cmake, tests/unit/probes/r39_elzazombies.cmake,
  tests/unit/probe_r39_elzazombies_karte.c, tests/unit/test_r39_elzazombies.c + Haken. Kein release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists.txt. OK.
* Gemeinsame Dateien nur kleine Haken mit "Runde 39 Spur Q": scd_room_setup.c (+1 include, +1 Installer-Zeile am
  Blockende nach Spur K), scd_vm.c (+1 include, +1 Ereignis-Zeile, sub01-Sperre +1 Bedingung, +1 Takt-Zeile),
  platform/pc/main.c (+2 Zeilen CONTINUE-Weg), test_r36_kartenbits.c (+2). OK.
* Riegel je Punkt vorhanden und messend: unit_r39_elzazombies_{zensus,kette_4,kette_0,kette_2,nach1061,nach1031,durch1041,
  ohne} (mit Gegenprobe "Modul aus"), integration_r39_elzazombies_{kette,knall,treppe,durch,ohne}. Selbst gefahren:
  `ctest -R "r39_elzazombies" -j 2` -> 13/13 gruen (167,7 s, `scratchpad/qa0/ctest_eigen.log`).
* Linux-fest: cmake_minimum_required im -P-Skript, libm ueber re15_test_support (PUBLIC m) bzw. `m` im Test-Target, keine
  .exe-/Backslash-Pfade. OK.
* NICHT OK: "keine Umgebungsvariablen des Aufrufers erben" (VERTRAG 2) — gemessen M9, Mangel 1.

### Gezielte Gruppen der beruehrten Module/Raeume (nicht die volle Suite)
`ctest -R "unit_r36_kartenbits|cut1150|r38_montage|zombieaktiv|r36_gegnerzahl_raum10[34]1|r34n_a_rolltor|r33_speichern|
^integration_elza_vollstart$|r38_elzaintro|r39_elza_|r38_elzatoene|r38_tagebuch|r36_story_elza|^unit_r36_story|r36_elza_|
r37_elzaems_elza10c1kraehe|r38_dining1141|r39_heliport1171|unit_montage_fx|r36_szene1150_knall|r35_fenster_knall|
unit_elza_zweig|unit_map_speichern_laden" -j 2` (117 Tests: Leons Montage 1150/1130/1040/1030, Elza-Raeume, Speichern,
Karte/Tagebuch, 1141, Spur K) -> "100% tests passed, 0 tests failed out of 117" (1938 s, `scratchpad/qa0/ctest_gruppen.log`), darunter
integration_r35_cut1150 (510 s), integration_r38_montage, unit_r35_cut1150_* (13), unit_r38_montage_* (8), unit_r36_kartenbits,
integration_r33_speichern, integration_elza_vollstart, integration_r38_dining1141_* (7), integration_r39_heliport1171_*.

## Urteile je Teilpunkt (gegen den Wortlaut, Nachtrag 11 eingerechnet)
* Q1 "einmal in den Flur hinein von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden":
  ERFUELLT. Montage-Schnitt ROOM1061 Cut 2 (2F-Absatz mit der 10C1-Tuer, RID @0xA0), die Zombies stehen nach dem Knall an
  der Tuer und gehen auf die Kamera zu; Zahl gemessen 4 (M3/M4), 3 (M1: `S ROOM1061 T105 3`), 0 (M2: kein 1061-Schritt).
* Q2 "wenn man in ROOM 1061 die Etage von unten ueber die Treppe erreicht aber moeglichst weit weg an der Seite stehen":
  ERFUELLT. Echter Weg M3 (Tuer 1041 -> 1061, Treppe A + B): vier Kopien an der Westseite unter dem Fenster, gehalten
  waehrend Etage 0/2, frei bei der Ankunft (25424,24568), Abstaende 4574..5565; SCA-Nachrechnung: weitester erreichbarer
  Punkt = erste Lage (5214 vom Treppenende). Bild m3_1061.png Bild 625.
* Q3 "das gleiche nochmal wie bei Leon ... falls die Zombies nicht besiegt wurden" + Nachtrag 11 "nicht in ROM 1071 ...,
  sondern ... wie bei leon im ROOM 1041": ERFUELLT (in der Lesart der AUFTRAG.md-Festlegung "Durchbruch durchs Rolltor
  beim naechsten Betreten von 1041, solange diese Zombies leben"). Echter Weg M4: Kette -> 1061 -> zurueck nach 1041 ->
  Rolltor-Schnitt Cut 1, Rolltor faehrt mit den Original-Zeilen ROOM1041 sub08 hoch, die vier kommen durch, HP 100,
  Steuerung an der Eintrittstuer. Bedingung gemessen: alle besiegt -> nichts (M6), zwei leben -> zwei (M7), Rolltor schon
  offen -> sie kommen durch das offene Tor (M8). ROOM1071: kein Schritt, keine Kopie (M1-M5: keine Zeile, Tot-Bits
  0xc6..0xca unberuehrt laut unit kette_*).
* Q4 "im ROOM 1031 ueber die Absperrung/Das Rollo, so wie bei der cutscene": ERFUELLT. Montage-Schnitt 1031 Cut 12 mit den
  Original-Zeilen sub08 (Aot_reset/Se_on/Cut_replace/Sca_id_set @0x02858..@0x0287E, falls (4,15)=0), Kriecher kriechen
  durch das aufgebrochene Rollo (m1c_1031.png); bei schon gebrochenem Rollo kein zweiter Tausch (m5_1031.png).
* Q5 Ausloeser/Nebenwirkungen (Orchestrator-Lesart: steigende Flanke (9,97), Leon unveraendert): ERFUELLT. Flanke bei
  Bild 200/100 startet die Kette (M1-M5); Laden mit (9,97) = keine Flanke und Leon ROOM1040 + Flanke = nichts
  (integration_r39_elzazombies_ohne gruen); Leons Montage-Riegel in der Gruppe.

## Maengel
1. VERTRAG Abschnitt 2 "keine Umgebungsvariablen des Aufrufers erben" verletzt: re15_port/tests/integration/
   test_r39_elzazombies.cmake loescht geerbte RE15_*-Variablen nicht. Nachpruefbar: `RE15_NOAUDIO=1 ctest --test-dir
   re15_port/build -R "^integration_r39_elzazombies_knall$"` -> Failed ("Mitschnitt fehlt"), ohne die Variable gruen.
   Dasselbe gilt fuer alle fuenf TEILe (jede geerbte RE15_INPUT_SCRIPT/RE15_DEBUG_JUMP/RE15_FRAMEDUMP/... wirkt in die
   Laeufe). Abhilfe wie test_r39_stimmen.cmake:31-37 / test_r39_heliport1171.cmake:47-53 (`cmake -E environment`,
   jede RE15_*-Variable `unset(ENV{...})` vor dem ersten Lauf); der Kopf des Skripts ("Umgebung nur ueber
   re15_start_spiel") ist danach erst zutreffend.
   Einordnung fuer den Orchestrator: von den 15 test_r39_*.cmake in diesem Baum loeschen nur test_r39_stimmen.cmake und
   test_r39_heliport1171.cmake die geerbten Variablen; 13 (auch die schon abgenommenen der Wellen 1-3) haben dieselbe
   Luecke. Das Gate ist trotzdem nicht erfuellt; eine einheitliche Nachbesserung aller Riegel waere moeglich.

## Hinweise / Entscheidungsfragen (keine Maengel)
* H1 Der 1041-Durchbruch prueft (9,97) nicht: er kommt auch NACH Johns Rettung, solange 1061-Kopien leben und Elza 1041
  im Suedteil betritt (integration_r39_elzazombies_durch faehrt ohne (9,97)). Mit "falls die Zombies nicht besiegt
  wurden" vereinbar; ob das zu "Der Rest bleibt normal" passt, entscheidet der Orchestrator/Nutzer.
* H2 Auf dem ueblichen Panik-Weg (1041 -> 1061 hinauf -> 1121) kehrt Elza nicht nach 1041 zurueck — der Durchbruch zeigt
  sich nur, wer vor den Zombies in 1061 zurueck nach 1041 flieht (oder spaeter wiederkommt). So in der AUFTRAG-Festlegung;
  dem Nutzer im Dossier 7 erklaert.
* H3 Ankunft in 1061 von OBEN (Rueckweg aus 1121): die Kopien stehen an der Westseite nahe dem Ende von Treppe A (Frage
  der Spur, Dossier 7). Nicht verlangt.
* H4 Integration: Ereignis 30 belegt auch Spur R (re15_hund1121_r39.h:60, nur ROOM1121) — raumgetrennt, keine Wirkungs-
  ueberschneidung, aber Textkonflikt in scd_vm.c (beide Spuren haengen ihre Zeile hinter Spur I an). Spur P setzt (9,97)
  am Ende von RUF1041 nach dem Kartenhinweis (re15_johnruf_r39.h); Q wartet auf frei() (cine/player_mode/Kartenhinweis).
* H5 Robustheit: durchbruch_1041 zieht die Kopien um (umzug_1041), BEVOR starten() einen Ereignis-Faden hat; findet sich
  keiner (14 Plaetze 10..23), waeren die Kopien ohne Szene nach 1041 umgezogen (beim naechsten Betreten Nachspawn H statt
  Durchbruch; die Kette selbst wurde in 05d03c9f genau dagegen gesichert). Praktisch nicht erreichbar, nur zur Kenntnis.
