# Runde 37 — Spur F "sperre1050": UNABHAENGIGE ABNAHME 0 (2026-10-12)

Baum `.claude/worktrees/r37_sperre1050`, Zweig `r37/sperre1050`, gepruefter Stand HEAD 3612ffcd (Merge-Basis master f1e019ca).
Die Abnahme aendert keinen Code; nur diese Datei wird committet.

## Stand der Abnahme (fortlaufend)

- [x] Lektuere AUFTRAG/VERTRAG/Dossier + Code-Diff (git diff master...HEAD, 24 Dateien; Code: 2 neue Dateien, 2 Haken, Text-Weg)
- [x] Bau (local_build.sh configure + build: LOCAL-BUILD-OK, "ninja: no work to do" — exe 14:22 juenger als jede Quelle; letzter Commit 3612ffcd aendert nur das Dossier)
- [x] Gezielte ctest-Laeufe (unit 106/106, integration_r37_sperre1050 1/1)
- [x] #10 an der exe gemessen (R1 Luke-Weg, R2/R2b Ada-Logik, R3-R6 Speichern/CONTINUE, Integrationslaeufe A-E)
- [x] #5 an der exe gemessen (R7 Elza ueber die echte Tuer 5141 -> 50B1, Bilder beider Seiten, Sweep)
- [x] RE-Gate (4 Code-Stellen + 7 Datei-Offsets selbst gelesen, Zensus nachgerechnet)
- [ ] Vertrag / Pfade / Nebenwirkungen


## 1. Bau und gezielte Tests (selbst gefahren)

* `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`; der Bau-Agent hatte bereits
  gebaut (ninja: no work to do). Die exe enthaelt die Logzeile `[sperre1050] ROOM%04X Slot ...` (grep -c = 1).
  local_build.sh beendet nur re15_pc-Prozesse unter dem eigenen Bau-Pfad (Z. 311-324) — keine fremden Laeufe.
* Eigene Riegel: `ctest -R "^unit_r37_sperre1050_" -V` -> **5/5 gruen** (bedingung: 16 Stellungen, gesperrt genau
  (1,1,x,0); raum: Fruehspiel Ereignis 13 / msg 25, Mittelspiel (auch ohne (3,94)) Tuer -> 0x0A, Endspiel Typ 5 Id 26,
  Rechteck (17200,-13700) halb (500,1000) Band 0, Maske 0xFFFF, flags 0x31, msg 26 = 36 B der 1060-Sperre, Slots
  0/1/2/3/5 Tueren, nach Irons Tuer, ROOM1051 Tuer; druck: Text + Freeze, kein Raumwechsel, keine Sequenz, zweiter
  Druck ebenso, Mittelspiel/nach Irons -> ROOM10A0; luke: Bytes @0x00F52/@0x01E3C/@0x0211A, Typwort 0x530E;
  elza50b1: Original @0x0055A, neuer Text, Form, Dauer 120, Sweep 103 Elza-RDTs "Ada" nur 30C1 msg 4/5).
* Gruppen der beruehrten Module/Raeume (breiter als der Bau-Agent: zusaetzlich kartenbits, room_load,
  load_after_death, alle unit_1090_*/probe_1090_*, szenenleon, r36_elza_*, affen_ada, elza_zweig, karte_speicher,
  r30_irons_*, irons_neck_1150, re2_room1090_ab, plc_back_yaw_1090, map_1090, r33_tuer1120_elza,
  r30_elza_selbsttuer):
  `ctest -R "^(unit_r37_sperre1050_|unit_r34n_d_adaruf_|unit_r34n_a_rolltor|unit_r34n_e_dokumente|unit_ada_eintritt_1050|unit_r19_marke3_1050_c1_tiefe|unit_r27_ada_cutscene_1050|unit_room1050_sicherung|unit_r35_inventar1050_|unit_r35_cut1150_|unit_r36_story_|unit_msg_dialog|unit_action_msg_gate|unit_text_freeze|unit_savepoint|unit_r36_klettern_|unit_r35_cut10f0_|unit_r33_speichern|unit_map_speichern_laden|unit_stair_hwtruth_1060|unit_r34n_f_leiche_texte|unit_r34n_b_kreuztext|unit_r36_kartenbits|unit_room_load|unit_load_after_death|unit_1090_|unit_r36_szenenleon_|unit_r36_elza_|unit_r35_affen_ada|unit_elza_zweig|unit_r33_karte_speicher|unit_r30_irons_|unit_irons_neck_1150|unit_re2_room1090_ab|unit_plc_back_yaw_1090|unit_map_1090|probe_1090_|unit_r33_tuer1120_elza|unit_r30_elza_selbsttuer)" -j 4`
  -> **`100% tests passed, 0 tests failed out of 106`**.
* Generator: `texte_bauen.py` (bauen() + schreiben() mit umgelenktem OUT in den Scratchpad, Baum unberuehrt)
  reproduziert `engine/src/gen/story_r36_texte.inc` **bytegleich**; 50B1 msg 0 = 78 B, Zeilen 162/170/126 px.
* Integration: `ctest -R "^integration_r37_sperre1050$" --timeout 1200 -V` -> **Passed 220.85 s**, Laufordner
  `re15_port/build/tests/integration/r37_sperre1050_wd/lauf_fc302e5d_*`:
  `[A_luke] Gitter=1 Absprung=1 Hof-Tuer=1 Sperre=1 Text=1 10A0=0 Ende1050=1`, `[E_endspiel] Sperre=1 Text=1 Tuer4=0
  10A0=0`, `[B_mittelspiel] Sperre=0 Text=0 Tuer4=1 10A0=1`, `[C_nach_irons] Sperre=0 Text=0 Tuer4=1 10A0=1`,
  `[D_elza_50b1] Elza=1 neuerText=1 Ada=0`. Selbst gelesen in `lauf_fc302e5d_A_luke/debug.log`: Z.127 `[aot] DOOR FIRE
  slot=1 rect=(-4750,-1750,hw=600,hh=500) ... spawn=(-13600,-9000,1300)` (Gitter 10F0), climb `T1488 room=1090 ...
  band=5 sub=9 kind=0x0E` (Absprung), `T1619 ... band=2 sub=9 kind=0x02` (von der Kiste), Z.214 `DOOR FIRE slot=0
  rect=(-10080,3060,...) spawn=(19850,0,-22300)` (Hof-Tuer), Z.225 `[sperre1050] ROOM1050 Slot 4 -> Text-Platz msg 26
  ((3,0xBB)=1 (9,73)=0 (9,71)=1 (3,94)=1)`, nach `Tick 2076 -> F372 Tasten 0x8000` Z.278 `[msgtext] room=1050 id=26
  "I have to get the Chief first..."`, Z.290 `EXIT_AT: Bild 700 in Raum 1050`; **kein** `PC loaded room10a0`.

## 2. RE-Gate (selbst disassembliert / selbst gelesen)

PSX.EXE = `info/Re1.5/PSX.EXE` (Auslieferungsstand), Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`.

| Behauptung (Code/Dossier) | selbst gelesen | Urteil |
|---|---|---|
| Typtabelle @0x8007469c[1] = sce-1-Handler 0x80043084 | `table 0x8007469c`: `[1] 0x800746a0 -> 0x80043084` | stimmt |
| Handler: `lhu a3,2(v0)` @0x80043098 (Maske), `lhu a2,0(v0)` @0x8004309c (msg), `jal 0x80027e68` @0x800430a0 | `dis 0x80043084 14`: genau diese drei Instruktionen, danach `sll a3,a3,16` (Delay-Slot), `addu v0,zero,zero`, `jr ra` | stimmt |
| Sprungziel 0x80027e68 = Nachricht oeffnen, kein Raumwechsel | `dis 0x80027e68 40`: `sb t1(=0x80),0(v1=0x800b8520)` Nachrichten-Zustand, `sw a3,-31424(at)` (0x800b8540), `or v0,a3,v0` / `sw v0,0(a0=g_pauseflags 0x800aca40)` = Freeze Maske<<16, Verzweigung nach a1&0x300 — kein Raumlader | stimmt (erklaert Freeze 0xffff0000 im Riegel `druck`) |
| Aot_reset LAB_80040738: rec[0]=pc[2] @0x80040764, rec[1]=(rec[1]&0x80)+pc[3] @0x8004076c-78, Nutzlast @0x80040788-a8, rec[2] unberuehrt | `dis 0x80040738 32`: `sb v0,0(a1)` @0x80040764; `andi v1,v1,0x80` @0x8004076c, `addu v0,v1,v0` @0x80040770, `sb v0,1(a1)` @0x80040778 (Delay-Slot); `sh` nach rec+12/+20 @0x80040790/9c/a8; kein Schreiben auf 2(a1) | stimmt |
| Kletter-Sonde FUN_8002d474 Zweig ABWAERTS: `andi v0,s6,0x2` @0x8002d74c, `lbu a2,-13610(a2)` @0x8002d760, `jal 0x8001c6e8` @0x8002d764, `andi v0,s6,0xe` @0x8002d780, `sb v0,-13734(at)` @0x8002d788, `sb v1(=9),-13735(at)` @0x8002d794 | `dis 0x8002d734 28`: Instruktion fuer Instruktion so (0x800aca5a = kind, 0x800aca59 = Substate 9) | stimmt |

Datei-Offsets (shared_assets/PSX == info/Re1.5/PSX, md5 je Datei gleich: 1050, 1090, 10F0, 50B1, 1130, 1060):

| Zitat | Bytes selbst gelesen |
|---|---|
| ROOM1050 @0x00B5A Slot 4 -> 10A0 | `3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00 08` (Slot 4, sce 2, flags 0x31, Band 0, Rechteck 16700/-14700/1000/2000, Raum 0x0A) |
| ROOM1090 @0x024D2 Set(3,0xBB) | `22 03 bb 01` |
| ROOM1130 sub01 @0x00A1C (Vorbild Text-Platz) | `46 03 01 31 01 00 ff ff 00 00` (Aot_reset Slot 3, sce 1, flags 0x31, msg 1, Maske 0xffff) |
| ROOM1060 @0x00D52 (1060-Sperre) | `3b 02 02 31 00 00 e8 67 ec 5e e8 03 98 08 f0 ad 00 00 b2 cc 40 08 00 04 05 00` |
| ROOM50B1 @0x0055A msg 0 (74 B) | `04 02 25 50 3a 4f 00 1d 40 3d 57 ...` = "It's Ada. She's dead." `02 00` "It looks like one of those" `08` "gorillas killed her." `01 00`; Block @0x558 `02 00` = 1 Nachricht |
| ROOM10F0 @0x00F52 / ROOM1090 @0x01E3C / @0x0211A | vom Riegel `unit_r37_sperre1050_luke` bytegenau gegen die Datei geprueft (gruen) |

Neuer Text (story_r36_texte.inc `k_s37_50b1_m0`, 78 B) selbst dekodiert: Kopf `04 02` + `25 50 3a 4f 00 3d 00 53 4b 49 3d 4a
57 00 2f 44 41 3a 4f 00 40 41 3d 40 57` = "It's a woman. She's dead." + `02 00` + Seite 2 bytegleich zum Original + `01 00`.

**Konstanten im Diff** (re15_sperre1050_r37.h): RAUM 0x1050, SLOT 4 (@0x00B5A), ADA (3,0xBB) (@0x024D2), SCE 1 / FLAGS 0x31 /
MASKE 0xffff (@0x80043098/9c/a0, Tuer-Satz pc[3], Vorbild @0x00A1C), PLAN/TOD (9,71)/(9,73) (Port-Bits aus re15_tuer1060.h,
dort belegt), MSG_ID 26 (VERTRAG §1.2). Die Bedingung selbst ist als **PORT-WAHL AUF NUTZERWUNSCH** gekennzeichnet und
traegt die woertliche Nutzer-Bestaetigung (master AUFTRAG.md "Bestaetigung 2026-10-12": "ja, perfekt." auf "Sperre
ROOM1050: vor Adas Rettung bleibt die Ada-Szene, danach der Chief-Text nur in der Endphase — nach der 10F0-Szene und
bevor Irons geholt ist; Fruehspiel ungesperrt"). Der neue Wortlaut 50B1 ist als NUTZER-VORGABE (Nachtrag 8) gekennzeichnet.
Testparameter (STAND_X/Z, Wege-Skripte) sind Messgroessen, keine Spielkonstanten. Suche im Diff nach "deferred",
"tunable", "interim", "for now", "faithful", "plausibel", "TODO": **keine Treffer** (s. Abschnitt 4).

**Wege-Zensus nachgerechnet:** alle 40 `ROOM1??0.RDT` frisch mit `re15_port/tools/scd_dump_room.py` gedumpt, die
Skripte `F_belege/tuer_graph.py` und `F_belege/wege_zensus.py` darauf gefahren: Ausgabe **identisch** zu
`tuer_zensus_stage1_leon.txt` (134 Saetze) und `wege_zensus.txt` (einziger Schnitt `1050 -> 10A0 0x00B5A Slot 4 Band 0`).
Gegenprobe der als inert gezaehlten Saetze: ROOM1170 Slot 1 -> 10B0 @0x01226 ist `sce=0`; ROOM1170 schaltet per
`Aot_on 47 02` (@0x01614) und `Aot_on 47 03` (@0x016D0) nur Slots 2/3, kein Aot_reset; kein Port-Code nennt ROOM10B0 als
Ziel ausser den Tuerbildern (gen/re15_tuer_eigen.inc 10B0<->1260, gen/tuer_zuordnung.inc 10B0->1170) — der Weg
1170 -> 10B0 -> 1260 -> 2000 ist also auch im Port zu.

## 3. Punkt #10 — "Chief first" zusaetzlich an der ROOM1050-Tuer (eigene Messungen an der exe)

Messkopie `re15_port/build/platform/pc/re15_pc_r37f_abn_mess.exe` (= re15_pc.exe des Baums), eigene Arbeitsordner im
Scratchpad `.../scratchpad/abn/w_<lauf>/` (Laeufer `abn/lauf.sh`, PPM->PNG `abn/ppm2png.py`).

### R1 — der Luke-Weg des Nutzers, Ende-zu-Ende in EINEM Lauf (Endspiel-Zustand)

Befehl (Wege-Skripte = die des Integrationstests, Zustand wie dort):
`RE15_DEBUG_JUMP=10f0@250 RE15_SET_FLAG=9:71,3:94,3:187,3:121,3:129 RE15_PLAYER_POS=-7125,4950,0
RE15_INPUT_SCRIPT_START=0 RE15_INPUT_SCRIPT=<10F0>,<1090>,<1050> RE15_FRAMEDUMP=330-480/30:f1050_ RE15_EXIT_AT=520#1050`
(+ NO_INTRO, NOAUDIO, TITLE_SHOT, WINDOW_SCALE=1, MSG_LOG, CLIMB_TRACE) -> exit=0. debug.log:

```
[room] PC loaded room10f0.rdt
[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500) target_cut=0 spawn=(-13600,-9000,1300)   <- Gitter 10F0 @0x00F52
[room] PC loaded room1090.rdt
   climb: T1488 room=1090 pos=(-9900,-9000,3358) band=5 sub=9 kind=0x0E                          <- Absprung vom Steg
[aot] DOOR FIRE slot=0 rect=(-10080,3060,hw=2350,hh=800) target_cut=6 spawn=(19850,0,-22300)    <- Hof-Tuer @0x0211A
[room] PC loaded room1050.rdt
[sperre1050] ROOM1050 Slot 4 -> Text-Platz msg 26 ((3,0xBB)=1 (9,73)=0 (9,71)=1 (3,94)=1)
[input-script] Tick 2076 -> F372 Tasten 0x8000
[msgtext] room=1050 id=26 "I have to get the Chief first..."
[flow] EXIT_AT: Bild 520 in Raum 1050 erreicht -> exit
```

`grep -c room10a0` = **0**. Bilder (RE15_FRAMEDUMP, WINDOW_SCALE=1, 2x vergroessert):
`abn/w_r1_luke/f1050_000360.png` (vor dem Druck: Leon im Flur vor der Tuer links, kein Text) und
`abn/w_r1_luke/f1050_000450.png` (nach dem Druck: **"I have to get the Chief first..."** im Bild, Leon steht vor der Tuer,
kein Raumwechsel). -> Die Luecke des Nutzers ist an der exe geschlossen; der Text steht an der 1050-Tuer.

### R2 / R2b — vor Adas Rettung bleibt die Ada-Logik (derselbe Weg, dieselbe Stelle)

`RE15_DEBUG_JUMP=1050@250 RE15_PLAYER_POS=19850,-22300,3072,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
RE15_INPUT_SCRIPT=W3,<1050-Weg>` ohne Endspiel-Bits (R2) bzw. mit `RE15_SET_FLAG=9:65` (R2b), exit=0:

```
R2 : [adaruf] ROOM1050 Slot 4 -> Ereignis-Platz sce 3 / Ereignis 13 (Szene) (Flags (3,187)=0, (9,65)=0)
     [adaruf] Ereignis 13: Szene startet, Spieler (16732,-13750) Gierung 4128 -> Blickpunkt (17487,-13750) ...
     [msgtext] room=1050 id=22 "Woman: Hello? Anyone? Please, get me out of here!"
     [msgtext] room=1050 id=23 "Leon: Another civilian survivor."
R2b: [adaruf] ROOM1050 Slot 4 -> Text-Platz sce 1 / msg 25 (Sperre) (Flags (3,187)=0, (9,65)=1)
     [msgtext] room=1050 id=25 "I have to help the Survivor first!"
```

Keine `[sperre1050]`-Zeile in beiden. Die Ada-Szene startet an GENAU der Stelle (16732,-13750), an der im Endspiel jetzt
"I have to get the Chief first..." kommt -> "die Tuer, wo wir auch die Ada Cutscene triggern" ist getroffen.

### R3-R6 — Boot-/CONTINUE-Weg (Haken platform/pc/main.c)

* R3 (Speichern nach Debug-Sprung, `RE15_SAVE_TEST_AGAIN=300 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`):
  `[save] saved (room 1050) slot n=0`; R4 (`RE15_CONTINUE_TEST=1`): `[save] CONTINUE: resumed in room 1050 (hp=100)`,
  `[sperre1050] ROOM1050 Slot 4 -> Text-Platz msg 26 (...)`, Druck F370 -> `[msgtext] room=1050 id=26 "I have to get
  the Chief first..."`, kein 10A0. (Die Kamera blieb hier auf Cut 0 — Artefakt des Debug-Sprungs: die Sprungtabelle
  setzt `cut=0`, Lauf E des Integrationstests bleibt ebenso auf Cut 0; der Spielstand trug diesen Cut. Nicht der Spur
  zuzurechnen, siehe R5/R6.)
* R5 (echter Tuereintritt: Sprung in den Hof 1090 `RE15_PLAYER_POS=-9000,4300,1024,1`, Aktionstaste an der Hof-Tuer):
  `[aot] DOOR FIRE slot=0 rect=(-10080,3060,hw=2350,hh=800) target_cut=6`, `PC loaded room1050`, `[sperre1050] ...`,
  `[pri] cut=6`, `[save] saved (room 1050) slot n=0`.
* R6 (CONTINUE aus diesem Stand, derselbe Weg zu Slot 4): `[save] CONTINUE: resumed in room 1050`, `[sperre1050]
  ROOM1050 Slot 4 -> Text-Platz msg 26 ((3,0xBB)=1 (9,73)=0 (9,71)=1 (3,94)=1)`, Kamera 6 -> 5 -> 4, `[msgtext]
  room=1050 id=26 "I have to get the Chief first..."`, Leon bleibt bei (16732,-13750), kein 10A0. Bild
  `abn/w_r6_continue_tuer/cont_000450.png`: Leon vor der Tuer, Text im Bild (identisch zum Tuer-Weg R1).
  -> Der main.c-Haken wirkt; nach einem Laden ist die Sperre ebenso da.

### Urteil #10

Wortlaut: "...man kann über die Luke bis nach F1 und kann den 'Ich muss zuerst irons holen' Blocker bei der Tür
überspringen. Ich schlage vor - du machst den Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada Cutscene
triggern, wenn wir Ada noch nicht gerettet haben." + bestaetigte Lesart ("ja, perfekt.": vor Adas Rettung bleibt die
Ada-Szene, danach der Chief-Text nur in der Endphase — nach der 10F0-Szene und bevor Irons geholt ist; Fruehspiel
ungesperrt).

| Teil | Messung | Ergebnis |
|---|---|---|
| Luecke "ueber die Luke bis nach F1" reproduziert/benannt | Dossier r7 + eigener R1: Gitter 10F0 -> Steg 1090 -> Absprung -> Hof -> 1050 | Weg existiert, ist der des Nutzers ("bis nach F1" = 1050) |
| Text an der 1050-Tuer der Ada-Szene | R1 (Tuer-Weg), R6 (CONTINUE), Integration A/E: `[msgtext] room=1050 id=26 "I have to get the Chief first..."` an (16732,-13750); R2 zeigt an derselben Stelle vor der Rettung die Ada-Szene | erfuellt |
| derselbe Text wie der 1060-Blocker | Riegel `raum`: msg 26 = 36 B von `re15_tuer1060_meldung` | erfuellt |
| Blocker nicht mehr ueberspringbar | R1: kein `room10a0`; Wege-Zensus nachgerechnet: 1050 -> 10A0 ist der einzige Schnitt | erfuellt |
| vor Adas Rettung bleibt die Ada-Logik | R2 (Szene, Ereignis 13), R2b (msg 25), keine Sperr-Zeile | erfuellt |
| Fruehspiel/Mittelspiel ungesperrt, nach Irons frei | Integration B/C: `DOOR FIRE slot=4` -> `PC loaded room10a0`; Riegel `raum`/`druck` (auch ohne (3,94)) | erfuellt |
| Elza (1051) unberuehrt | Riegel `raum` ROOM1051 mit Endspiel-Bits -> Tuer | erfuellt |
| nach Laden (CONTINUE) ebenso | R6 | erfuellt |

**#10: erfuellt.**

## 4. Punkt #5 (+ Nachtrag 8) — ROOM50B1 msg 0 (Elza) ohne Ada-Bezug

### R7 — Elza ueber die ECHTE Tuer 5141 -> 50B1 zur Leiche

Befehl: `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_TITLE_SHOT_CHAR=1` (Neues Spiel als Elza ueber
re15_gameflow_new_game(1), derselbe Aufruf wie das Player-Select-Ende), `RE15_DEBUG_JUMP=5140@250
RE15_PLAYER_POS=12515,-10030,3072,0` (Mitte des Tuer-Rechtecks ROOM5141 main00 @0x01A5E Slot 0 -> Raum 0x0B),
`RE15_INPUT_SCRIPT_START=0 RE15_INPUT_SCRIPT=W14,A0.1,W14,U6.6,W0.3,R0.37,W0.3,U0.7,W0.5,A0.1,W4,A0.1,W3,A0.1,W3`
(Weg nach SCA-Zellen von ROOM50B1: Tuer-Spawn (-14100,-26150) nach +Z, dann +X vor die Leiche), `RE15_FRAMEDUMP=0-1200/30:e50b1_
RE15_EXIT_AT=900#50b1` -> exit=0:

```
[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)
[debug-menu] JUMP -> 514 F C-2 CORRIDOR (ROOM5141) spawn=(12515,0,-10030) cut=0
[input-script] Tick 420 -> F167 Tasten 0x8000
[aot] DOOR FIRE slot=0 rect=(12515,-10030,hw=2525,hh=820) target_cut=0 spawn=(-14100,0,-26150)
[room] PC loaded room50b1.rdt
[walk] F600 cut=2 ... pos=(-14100,0,-12800) ... F690 pos=(-12546,0,-11363) rot=4128
[input-script] Tick 1106 -> F686 Tasten 0x8000
[scd] Plc_motion(entity=1, motion=11, flags=0x00)
[msgtext] room=50b1 id=0 "It's a woman. She's dead.It looks like one of those gorillas killed her."
[input-script] Tick 1229 -> F809 Tasten 0x8000            <- Seite 2
```

Bilder (Kamera Cut 2 des Raums, Elza und die Leiche im Bild): `abn/w_r7_elza_tuer/e50b1_000780.png` Seite 1
**"It's a woman. She's dead."** mit Weiter-Pfeil; `abn/w_r7_elza_tuer/e50b1_000870.png` Seite 2 im Aufbau
**"It looks like one of those / gorilla..."** (zweizeilig wie das Original). Kein "Ada" im Protokoll dieses Laufs.
(Das Belegbild des Bau-Agenten `F_belege/bild_50b1_elza_f200.png` ist unter der Sprung-Kamera Cut 0 entstanden — weder
Elza noch Leiche im Bild; R7 schliesst diese Luecke.)

### Unabhaengiger Sweep (Original-Daten, eigenes Dekodierskript)

Alle 103 Elza-RDTs `ROOM???1.RDT` (17 weitere sind 4-B-Platzhalter) roh dekodiert (Block RDT+0x3C, Glyphen wie
texte_bauen.py): "Ada" steht im Original nur in **ROOM30C1 msg 4** @0x00F7A (Marvin: "...me and Ada escape the city."),
**ROOM30C1 msg 5** @0x00FC7 ("Elza: Ada?") und **ROOM50B1 msg 0** @0x0055A ("It's Ada. She's dead..."). 30C1 msg 4/5:
Elza hoert den Namen bzw. fragt nach — bleibt nach Nachtrag 8 ("dass Elza den NAMEN Ada kennt ..., bleibt"); 50B1 msg 0
ist die einzige Stelle, an der Elza Ada am Aussehen erkennen muesste — sie ist ueberschrieben. Port-eigene Elza-Texte
(`*elza*`, gen/*.inc) tragen "Ada" nur in der alt-Spalte/Kommentaren bzw. in Leon-/Marvin-Saetzen. Der Riegel
`unit_r37_sperre1050_elza50b1` (Port-Lader + Overrides) kommt zum selben Ergebnis.

### Form/Begruendung

* Wortlaut = Nachtrag 8 woertlich in der Lesart des Orchestrators ("It's a woman. She's dead." / "It looks like one of
  those" / "gorillas killed her."); kein Text behauptet, Elza und Ada koennten sich nicht begegnen.
* Kopf `04 02`, Seitentrenner `02 00`, Schwanz `01 00` = Original-Bytes; Seite 2 bytegleich; Dauer 120 = Original;
  Zeilen 162/170/126 px (< 271). Generator reproduzierbar (Abschnitt 1).
* Begruendung in Code (`texte_bauen.py` UNTERSUCHEN-Grund, `story_r36_texte.inc`-Kopf) und Dossier traegt den Grund des
  Nutzers ("Dann ist es halt nur - a woman."). Der Code-Kommentar "Ada kennt sie nur dem Namen nach" beschreibt den Stand
  beim Fund der Leiche (Marvin 30C1), nicht eine Regel "nie begegnet" — kein Widerspruch zu Nachtrag 8.

**#5: erfuellt.**
