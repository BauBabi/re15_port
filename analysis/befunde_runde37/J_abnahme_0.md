# Runde 37 — Spur J "bissarm" — Abnahme 0 (2026-10-12)

Unabhaengige Abnahme des Baus `r37/bissarm` (Basis master 419380dc, Kopf 5d96f36d). Fortlaufend geschrieben.
Messordner (unversioniert): `%TEMP%/claude/.../scratchpad/J37A/` (Zahlen und Bildpfade hier zitiert).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md Nachtrag 6)

> Ja, wie ich gerade gesehen habe fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird.

## 0. Stand der Abnahme

- [x] Diff gelesen (`git diff master...HEAD`, Code komplett)
- [x] Bau (local_build.sh configure + build: `ninja: no work to do`, LOCAL-BUILD-OK)
- [x] Riegel der Spur 9/9 gruen (selbst gefahren)
- [x] Gegenprobe-Bau 419380dc selbst gebaut (git archive -> scratchpad, nur re15_pc + Sonde)
- [x] Messung an der exe (vorher/nachher, 5 Szenarien, FRAMEDUMP, WINDOW_SCALE=1, beschleunigter Renderer)
- [x] RE-Gate (8 Stellen selbst disassembliert)
- [x] Wandnahe Griff-Szenarien STAGE1-6 (Sweep alt gegen neu, 832 Szenarien)
- [x] Kriecher-Begruendung nachgemessen
- [x] Nachbar-Riegel 142/142
- [x] Vertrag / Urteil

## 1. Diff (gelesen)

`git diff master...HEAD --name-only` (Zweig allein, master ist seit der Basis weitergelaufen):
neu `engine/src/bissarm_r37.c`, `include/re15_bissarm_r37.h`, Riegel `tests/unit/test_r37_bissarm.c`,
`tests/unit/probes/r37_bissarm.cmake`, `tests/integration/test_r37_bissarm_echtlauf.cmake`; Haken
`enemy_ai_common.c` (+5/-1: Include + Vorwaerts-Deklaration, Startwert in beiden Latches, `|| ein_durchgang` in
re15_victim_place), `re2_ems.c` (+2: Include + RE2-Bind merken), `platform/pc/main.c` (+3: Include, Opfer-Haken im
Spieler-Zeichner, Handkontakt im NPC-Zeichner); Dossier + Belege unter `analysis/befunde_runde37/J_*`.

## 2. RE-Gate — selbst disassembliert

| Stelle | Binaerdatei | Gelesen | Behauptung |
|---|---|---|---|
| RE2 Hauptschleife @0x80026620/28/34 | info/re2leon/PSX.EXE | `jal 0x8003bfac` / `jal 0x800355c4` / `jal 0x8003567c` + Delay `ori a1,zero,0x8000`; davor `lhu v0,15134(s3)` (+0x10E, s0=s3+0x3a10) `andi 0x8000` `bne` | stimmt |
| RE2 FUN_8003567c @0x800356d0 / @0x800356f4-f8 | info/re2leon/PSX.EXE | `jal 0x8004c1bc` (a0=+0x84, a1=`lhu 144`, a2=`1<<lbu 262`); `sh v0,280(s0)` / `sh v1,282(s0)`; Gates `andi v0,v1,0x8` @0x80035694, `lw v0,484(s0)` @0x800356a0 | stimmt |
| RE2 FUN_8004c1bc -> Formhandler @0x800a737c | info/re2leon/PSX.EXE | die Formhandler 0x8004c900/cbd0/d048/d508/d9b4/f2ac lesen `lh/lhu 280(..)`/`282(..)` = alter Standpunkt +0x118/+0x11A | bestaetigt (selbst nachgelegt: der "alte Standpunkt" wird tatsaechlich im Klemm-Durchgang benutzt) |
| RE2-RAM-Fixture (re2_vorn/hinten_frames.bin, 30+30 Halte) | J_belege | Spieler word0 = 0x40001001 (Bit 3 = 0), +0x1E4 = 0, +0x10E = 0x000F, +0x90 = 450, +0x118/+0x11A = Stand | stimmt (beide Gates fallen im Griff durch) |
| RE1.5 @0x80031d44-74 | info/Re1.5/PSX.EXE | `lw v0,-13764(v0)` (0x800aca3c) `andi 0x4000` `bne`, `jal 0x8002b498` @0x80031d58, `jal 0x8003b0a4` @0x80031d70 + `ori a2,zero,0x1` @0x80031d74 | stimmt |
| RE2 EMOVL10_S0 @0x80102830-3c, @0x8010266c-90 | info/re2leon/COMMON/BIN/EMOVL10_S0.BIN | `addiu v1,v1,1`; P3 Bytes `21 20 20 02 21 28 40 02`; `lhu v0,270(s1)`, `andi s5,v0,0x1`, Typ 23/17 -> `addiu s5,s5,2` | stimmt |
| Tabellen @0x8010C8CC / @0x8010C918 / @0x8010C854 | EMOVL10_S0.BIN | [3] = 0x801025EC; Kriecher [1] = 0x801025EC, [3] = 0x803200be (kein Code -> Kriecher hat kein +0x5==3); Wurzel gerade/ungerade = 0x8010118C/0x80101210 | stimmt |
| RE2 FUN_80028DAC @0x80028dcc / @0x80028ddc | info/re2leon/PSX.EXE | `sltiu v0,v0,0x1d`; `j 0x80028de4` + Delay `sw zero,0(s0)`; +0x9A += +0x9E, +0x60 += +0x9A | stimmt |
| RE1.5 PLD-Tabelle @0x80073f70 | info/Re1.5/PSX.EXE | u16 [0x3C, 0x3D, 0x3E, 0x3F] -> Eintrag 0 = 0x3C (PL00.PLD) | stimmt |

Konstanten im Diff ohne Adresse: Knochen 9/12 (Schulter), 10/13, 11/14 (RE2-Reihenfolge, aus der Fixture belegt),
`OPFER_ANKER 2`, 1/d^2-Gewichtung, `L1+L2-1` / `|L1-L2|+1` (Reichweiten-Schutz), `+1.0` im Nenner — alle im Kopf
von `re15_bissarm_r37.h` als PORT-KONSTRUKT benannt (Hybrid hat kein Original). Kein "deferred/tunable/interim/
for now/plausibel/TODO" im Code-Diff (grep), kein Env-Schalter als Abschluss (RE15_BISSARM_LOG ist nur Mess-Schiene).

## 3. Riegel der Spur (selbst gefahren)

`ctest -R "r37_bissarm" --timeout 900`: **100% tests passed, 0 tests failed out of 9** (40,6 s).
Einzeln nachgesehen: pruef_re2 vorn "ohne 222/223 -> mit 9/3", hinten "ohne 222/223 -> mit 8/2"; Echtlauf A/B/C:
103 Griffbilder je Lauf, 50/54/58 Biss-Bilder mit beiden Haenden auf dem Ziel, Opfer-Bezug Leon 0, Elza |Y| 116,
Halten max 1128/917/1128, 0 Spruenge, 0 auf (0,0).
Hinweis: `pruef_armlog` (Hand liegt auf dem Ziel) prueft den IK-Ausgang gegen sein eigenes Ziel — das belegt nur,
dass der Haken in der exe laeuft; der Beleg fuer das ZIEL ist `pruef_re2` (Port-Bild gegen RE2-RAM).

## 4. Messung an der exe — vorher (419380dc, selbst gebaut) gegen nachher (5d96f36d)

Gegenprobe-Bau: `git archive 419380dc re15_port` (ohne shared_assets) -> scratchpad `J37A/base_src`, cmake mit
derselben Toolchain (msys64 zuerst im PATH, SDL2 aus dem vorhandenen `_deps/sdl2-src`), Ziel `re15_pc`. Beide exe als
Messkopien unter eigenem Namen neben der Bau-exe (`re15_pc_j37abn_alt.exe`, `re15_pc_j37abn_neu.exe`, md5 der neuen
== re15_pc.exe), Assets ueber die exe-Wurzel. Spielstand je Lauf mit `test_r37_bissarm karte` (Slot 0) -> CONTINUE;
`RE15_WINDOW_SCALE=1`, beschleunigter Renderer (kein SOFTWARE_RENDER), `RE15_FRAMEDUMP=56-168/4:<praefix>`,
`RE15_STATE_LOG`, `RE15_BISSARM_LOG`, `RE15_EXIT_AT=170#<raum>`. Werkzeug `J37A/tools/lauf.sh`, Bilder `J37A/tools/crop.py`.

| Lauf | Karte | vorher (alt) | nachher (neu) | Bild |
|---|---|---|---|---|
| M1 ROOM1140 Leon, Griff von HINTEN (Kamera vor Leon) | (-1000,-19600) rot 2048, Kill-Flags 7:211-214 | Zombie-Unterarm liegt TIEF quer ueber der Brust bzw. ist hinter Leon verschwunden (F100: kein Unterarm auf Leon sichtbar) | Unterarm liegt UEBER Leons rechter Schulter auf der oberen Brust (F88-F120) — wie `re2_original_hinten_biss.png` | `J37A/cmp_M1_zoom.png`, `cmp_M1.png` |
| M2 ROOM1140 Leon, Griff von VORN (Kamera hinter Leon) | rot 0 | Unterarm tiefer am Ruecken | Unterarm ueber der Schulter auf dem oberen Ruecken | `J37A/cmp_M2_zoom.png` |
| M3/M3v ROOM1140 Elza hinten/vorn | char 4 | Hand tiefer | Hand auf Elzas Schulterhoehe (Unterschied klein, Bild-Aufloesung 320x240) | `J37A/cmp_M3_zoom.png`, `cmp_M3v_zoom.png` |
| M4 ROOM1010 Cut 6, Weg des Integrationsriegels (Leon (3700,2300), D0.3) | | **Zombie beisst allein ins Leere** (Leon fehlt im Bild, er steht auf (0,0)) | Leon und Greifer zusammen, Arm an Leon | `J37A/cmp_M4_zoom.png`, `cmp_M4_over.png` |

bissarm.log (nur neu): je Lauf 103 Zeilen (Haken laeuft im Griff); alt: Datei fehlt (Haken fehlt) — erwartet.

Wertung: Der Fall, der dem Wortlaut am naechsten kommt ("teilweise fehlt der dranhaengende Arm"), ist im Bau
deutlich zu sehen und behoben: (B) Leon stand im Griff auf einem falschen Platz (Startwert der Port-Wandklemme nie
gesetzt) und rang allein bzw. der Zombie biss ins Leere. (A) ist eine kleinere, aber sichtbare Verbesserung: die
Hand des Hybrid-Zombies liegt jetzt ueber der Schulter statt darunter/in Leon.

## 5. Wandklemme — wandnahe Griffe in ALLEN Raeumen (selbst gemessen, alt gegen neu)

Werkzeug `J37A/tools/sweep.py`: je Raum (alle Leon-Raeume STAGE1-6 mit RDT) bis 16 Punkte 500 vor einer Sperrzellen-Kante
(geom.sca_sperrzellen Band 0), Leon zwischen Wand und Greifer (Blick weg + "vorn", Blick zur Wand + "hinten"),
`test_r37_bissarm pruef_klemme` einmal gegen die Engine 419380dc (Sonde gegen die Basis-Engine gelinkt, nur der
Modul-Quelltext bissarm_r37.c/.h dazu, enemy_ai_common.c = Basis, `diff --strip-trailing-cr` leer) und einmal gegen
den Bau der Spur. Ergebnis `J37A/sweep_stage1.tsv` (544 Szenarien, 0 Fehler):

| Raum | Szen. | Szen. mit Fehlbildern alt / neu | Fehlbilder "Sprung>1200/weiter als 3000" alt / neu | Bilder "in Wandzelle" alt / neu |
|---|---|---|---|---|
| 1010 | 32 | 14 / 0 | 1091 / 0 | 118 / 0 |
| 1030 | 32 | 10 / 0 | 810 / 0 | 0 / 0 |
| 1040 | 32 | 6 / 0 | 486 / 0 | 0 / 0 |
| 10D0 | 32 | 6 / 2 | 486 / 0 | 486 / 16 |
| 10E0 | 32 | 10 / 0 | 810 / 0 | 0 / 0 |
| 1140 | 32 | 14 / 1 | 1134 / 0 | 486 / 16 |
| 1180 | 32 | 10 / 0 | 810 / 0 | 0 / 0 |
| 11E0 | 32 | 8 / 0 | 648 / 0 | 648 / 0 |
| 1220 | 32 | 28 / 19 | 783 / 53 | 0 / 172 |
| 1250 | 32 | 10 / 0 | 810 / 0 | 0 / 0 |
| 1020/1070/10B0/1100/1110/11F0/1200 | je 32 | kein Griff in der Sonde | | |

Gesamt STAGE1: neu besser in 104, gleich in 431, schlechter in 9 Szenarien. STAGE2-6 (`J37A/sweep_stage2_6.tsv`,
288 Szenarien in 2000/2020/30D0/40A0/40B0/5020/5060/5120/5140): neu besser in 80, gleich in 208, schlechter in 0.
Der Bau 419380dc liess Leon in wandnahen Griffen auf dem nie gesetzten Startwert stehen (max Abstand bis 35520) —
die Fehlerklasse "Zombie beisst ins Leere / Leon ringt allein" trat also nicht nur in ROOM1010 auf, sondern in
JEDEM Raum, in dem die Sonde einen Griff herstellt (STAGE1 10/10, STAGE2-6 7/8); der Bau der Spur behebt sie ueberall.
Die 9 schlechteren Szenarien (Detail `J37A/tools/klemm_detail.py`, Sonde `griff` + R37J_KLEMME):
- 10D0 (-6150,24850) und 1140 (3600,-21950): neu bleibt Leon nach dem einen Durchgang 31..77 Einheiten in einer Zelle
  (zweiter Durchgang wuerde ihn verschieben), alt hielt ihn auf dem letzten begehbaren Stand. Klein, Original-Ablauf
  (ein Durchgang ohne Rueckfall @0x800356d0 / @0x80031d70).
- 1220 (Kriecher-Formation, Greifer per Sonde versetzt, Griff ab Bild 1): neu Eindringtiefe bis 614/705 Einheiten,
  alt Leon 1400..35520 neben der Formation. Beides kaputt; derselbe Raum wie OFFEN 1 des Dossiers (duenne Wand).
  Im Echtlauf ROOM1220 (Karte (-22300,-5200) und (-22400,-9700), 1500/700 Bilder) griff kein Zombie
  (`ss1=2 mo=23 af=9` unbewegt) — der natuerliche Fall liess sich nicht herstellen.

Weitere Gegenproben an der exe:
- ROOM1140 mit allen fuenf Zombies (Karte (-1400,-19600) rot 0, ohne Kill-Flags, 600 Bilder, `J37A/*_R1140voll`,
  Auswerter `J37A/tools/griffweg.py`): alt und neu bitgleich im Griffweg — Slot 5 F58..F160, Slot 2 F166..F268,
  Slot 5 F384..F404 (Folgegriff: der Startwert wird beim erneuten Zupacken neu gesetzt). Im Halten (ss2=3, Clip 12)
  Abstand Leon-Greifer 338..1030 (RE2-Fixture: 500..980). Die grossen Abstaende (bis 2981) liegen nur im Abwurf
  (ss2=8, Clip 13), wo der Zombie zurueckweicht — in beiden Bauten gleich, nicht Teil dieser Spur.
- Elza-Archive selbst verglichen (EMS-Blobs je Typ, 9 Abschnitte): CDEMD0/CDEMD1 unterscheiden sich fuer 0x10, 0x11,
  0x12, 0x13, 0x16 genau in dir[3..6], fuer 0x18 nur in dir[5..6] — wie im Dossier 2.3 behauptet.

ROOM1010 (M4) Griffweg aus state.log (`griffweg.py`): alt Slot 1 F58..F160, **89 Bilder auf (0,0)**, max 5571;
neu 0 Bilder auf (0,0), im Halten (Clip 11/12, 81 Bilder) max 917 — Behauptung des Bau-Agenten bestaetigt.

## 6. Kriecher-Biss (+0x5==1) — Nachmessung der Begruendung "kein Kontaktpunkt"

Der Bau laesst den Kriecher-Griff unveraendert, Begruendung: RE2-Rig- und Hybrid-Hand liegen 531/542 vom naechsten
Bein-GELENK (Sonde `kriecher`, ROOM1030). Gelenkabstand sagt ueber einen Griff um den Unterschenkel wenig (ein
Griff in der Schienbeinmitte liegt ~330 von beiden Gelenken). Selbst nachgemessen (Kopie der Sonde in
`J37A/head_src`, Stand 5d96f36d, nur Ausgabe ergaenzt; `J37A/kriecher_abn.txt`): Abstand RE2-Rig-Hand bzw.
Kriecher-Kopf (Gelenk 8) zur STRECKE Knie-Knoechel (L3-L4 / L6-L7), 22 Stichbilder aus 172 Griffbildern:
Hand 11 mittel 482, Hand 14 mittel 491, naehere Hand mittel 415 (min 89), **Kopf mittel 449 (min 206)**.
Bestaetigt: keine Hand liegt am Bein — aber auch der Kopf nicht; der Satz "der Kriecher beisst ins Bein" (Dossier 2.4)
ist geometrisch nicht belegt. Ob das RE2 so zeigt, ist NICHT am RE2-Original gemessen (der RAM-Beleg liegt nur fuer
den Steh-Griff vor; die Kriecher-Formation ueber die Kriech-Opfer-Maschine 0x8010AF58 ist im Port nie gegen RAM
gestellt worden). Nicht Teil der AUFTRAG-Lesart (1) "Griff von vorn/hinten" — deshalb Hinweis H1, kein Mangel.

## 7. Nachbar-Riegel

Regex (`J37A/nachbar_regex.txt`): `r37_bissarm|r36_kleidung|r36_schuss|re2_hybrid_rig|rig_root_fix|re2_gore|re2z_|
re2_victim_collapse|dog|hund|crow|kraehe|re2_crawler|crawl_toggle|1030_|1210_|r35_raeume|r35_affen|r36_zombieki|
re2_zombie|zombie_pose|zgirl|room1140|lyer_1140|1140_straight|r36_blut_pin_kriecher|r36_szenenleon` — deckt alle
Greifer ab, die durch den geaenderten Latch/die Wandklemme laufen (Zombie, Hund, Kraehe, Gitterarme 1210, Gorilla,
Zombie-Girl, Kriecher) und die Hybrid-/Gore-Pfade.
`ctest -R <regex> -j 2 --timeout 600`: **100% tests passed, 0 tests failed out of 142** (619 s, unter Last anderer
Spuren; kein Fenster-Haken rot). Log `J37A/ctest_nachbarn.log`.

## 8. Vertrag

- Pfade (`git diff master...HEAD --name-only`): nur `engine/src/bissarm_r37.c`, `include/re15_bissarm_r37.h` (neu),
  Haken in `enemy_ai_common.c` (+5/-1, je Zeile "Runde 37 Spur J"), `re2_ems.c` (+2), `platform/pc/main.c` (+3),
  Riegel ueber `tests/unit/probes/r37_bissarm.cmake` + `tests/integration/test_r37_bissarm_echtlauf.cmake`
  (`cmake_minimum_required(VERSION 3.16)` vorhanden), Dossier/Belege. Kein release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists. `re2_ems.c` steht nicht in der Liste 1.4, ist aber ein 2-Zeilen-Haken
  mit Kommentar — zulaessig als kleiner Haken.
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignisse/Slots (das einzige `re15_game_flag_set` steht im
  Kartenwerkzeug der Sonde: Kill-Flags fuer den Spielstand).
- Env-Schalter im Engine-Code nur `RE15_BISSARM_LOG` (Mess-Schiene, kein Verhalten); `R37J_*` nur in der Sonde.
- Linux-Portabilitaet: keine POSIX-Aufrufe in der Sonde; `file(COPY_FILE)` wie 28 andere Integrationsriegel.

## 9. Urteil

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| "fehlt in unseren Port teilweise noch der dranhaengende Arm wenn man vom Zombie gebissen wird" | **erfuellt** | Beide gemessenen Ursachen sind im Bau behoben und am Original belegt: (B) der Startwert der Opfer-Wandklemme wurde beim Zombie-Griff nie gesetzt — im Bau 419380dc steht Leon in ROOM1010 89 von 103 Griffbildern auf (0,0), der Zombie beisst ins Leere (`cmp_M4_zoom.png`), und dieselbe Fehlerklasse tritt in jedem Raum mit Griff auf (Sweep 832 Szenarien, 552 mit Griff: alt 198 Szenarien mit Fehlbildern in 17 Raeumen, neu 24 — 21 davon Kriecherformationen in 1220/5060 (5060 unveraendert gleich), 3 Randfaelle mit <= 77 Wandueberlappung, H2); jetzt ein Durchgang je Bild vom alten Standpunkt wie RE2 FUN_8003567c (`jal 0x8004c1bc` @0x800356d0, `sh` @0x800356f4-f8, Formhandler lesen +0x118/+0x11A) und RE1.5 @0x80031d70. (A) die Hand des Hybrid-Zombies lag ~200 unter der RE2-Kontaktstelle; jetzt gegen die RE2-RAM-Fixture (P3 @0x8010F838) ab Biss-Bild 4 <= 6 Abweichung (selbst gefahren: vorn 9/3, hinten 8/2 im Mittel), im Bild liegt der Unterarm ueber der Schulter statt darunter/in Leon (`cmp_M1_zoom.png`: alt F100 kein Unterarm sichtbar, neu sichtbar). Lesart (2) "baumelnder Arm": RE2 hat keinen (FUN_80028DAC @0x80028dcc/@0x80028ddc, Wurf @0x8010755c/60 selbst gelesen), der Port wirft schon so. |

Gates: RE-Gate haelt (8 Stellen selbst disassembliert, alle wie zitiert; jede Konstante mit @0x bzw. als
PORT-KONSTRUKT benannt; keine Rate-Woerter; der Fix erklaert beide Befunde). Vertrag haelt. Riegel 9/9 + 142/142.
Gegenprobe: die Riegel der Spur sind gegen die Basis-Engine rot (klemme_1010a: 16 Fehlbilder, klemme_1010b: Leon 80
Bilder auf (0,0), 81 in einer Wandzelle; selbst gefahren).

**bestanden = true** (keine Maengel). Hinweise (nicht blockierend, fuer Integration/Folgearbeit):

- **H1 Kriecher-Biss nicht am Original belegt.** Weder Hand (naehere Hand mittel 415 zur Unterschenkel-Strecke) noch
  Kopf (mittel 449, min 206) beruehren Leon im Kriecher-Biss — vor und nach dem Fix. Die Aussage "RE2 hat dort
  keinen Kontaktpunkt / der Kriecher beisst ins Bein" stuetzt sich nur auf das RE2-Rig in der PORT-Formation. Weg:
  RE2-RAM-Messung wie `re2_biss_gdb.py`, Haltepunkt im Kriecher-Zweig von 0x801025EC (+0x10E Bit 0 = 1) bzw. die
  Kriech-Opfer-Maschine 0x8010AF58; oder die Kriecher-Formation (Anker/Yaw/Opfer-Clip) Byte fuer Byte pruefen. Liegt
  ausserhalb der AUFTRAG-Lesart (1) "Griff von vorn/hinten".
- **H2 Wandklemme, Randfaelle.** Ohne Rueckfall bleibt Leon in 3 von 544 STAGE1-Szenarien nach dem einen Durchgang
  31..77 Einheiten in einer Zelle (10D0 (-6150,24850) beide Seiten, 1140 (3600,-21950) hinten; 6..16 Bilder), wo der
  alte Rueckfall ihn zurueckhielt; in den 1220-Kriecherformationen bis 614/705 (alt dort Leon 1400..35520 neben der
  Formation). Das ist der Original-Ablauf (ein Durchgang); zusammen mit OFFEN 1 des Dossiers (duenne Wand 1220)
  ansehen, falls der Nutzer 1220 wieder meldet.
- **H3 Reichweite.** In den Biss-Bildern 0..5 und 31..37 bleibt eine Hand bis 133 vor dem RE2-Kontakt (RE1.5-Arm zu
  kurz, Arm gestreckt) — Dossier OFFEN 4, sichtbar nur als kleiner Abstand am Schleifenanfang/-ende.
- **H4 Riegel-Guete.** `pruef_armlog` prueft die IK-Hand gegen ihr eigenes Ziel (belegt nur, dass der Haken laeuft);
  der Zielbeleg ist `pruef_re2`. `pruef_griffweg` misst echt (gegen 419380dc rot).
- **H5 Speicher/PSX.** `s_re2_bind[256][32][3]` = 48 KiB statisch fuer 6 Zombie-Typen, dazu double-Mathe in
  engine/src (wird fuer jedes Ziel mitgebaut). Fuer PC belanglos; beim PSX-Ziel (Laufzeitbudget) auf die
  Zombie-Familie begrenzen.
- **H6 Integration.** Neue Datei `engine/src/bissarm_r37.c` -> Android-Bau braucht einen frischen Configure
  (GLOB-Cache app/.cxx); die Unit-Riegel brauchen `analysis/befunde_runde37/J_belege/re2_*_frames.bin` (sonst SKIP 77).
- **Echter Weg.** Gemessen per Spielstand+CONTINUE in den Raeumen. Der Defekt (statischer Startwert) und der Fix
  (Startwert bei jedem Zupacken aus Leons Stand) haengen nicht vom Eintrittsweg ab; der Folgegriff im selben Raum
  (ROOM1140, 3 Griffe hintereinander) ist gemessen.

Messkopien der exe (`re15_pc_j37abn_alt/neu.exe`) nach der Messung entfernt; kein Code geaendert.
