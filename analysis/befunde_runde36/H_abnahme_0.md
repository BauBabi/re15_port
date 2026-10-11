# Runde 36 — Spur H "ziel1190" — Abnahme 0 (unabhaengig, 2026-10-11)

Baum `.claude/worktrees/r36_ziel1190`, Zweig `r36/ziel1190` @ e3d68933 (Basis master 5aa6ddc2 = 78a4a748 + Doku).
Gebaut mit `local_build.sh configure` + `build` (ninja: no work to do — die exe 03:19 ist juenger als alle
Quell-mtimes des Diffs 03:16-03:17; die Merges 2da98280/ae704891 brachten unter re15_port NICHTS, Commits nach
f833268f aendern nur Tests/Doku). Messkopie `re15_pc_h36abn.exe` neben der exe, eigene Arbeitsordner (Scratchpad `abn/`).

Wortlaut (AUFTRAG.md): (1) Scheiben-Texte HINTEN an den Zielscheiben beim Anklicken, nicht vorne an den
Knoepfen; bei Elza die anderen zwei Scheiben = Texte vertauscht. (2) Frage: Kiste in ROOM1090 — Verwendungszweck?

## Gates (Stand beim Schreiben, wird fortlaufend ergaenzt)

### Suite (gezielt, kein voller Lauf im Spurbaum)
`ctest --test-dir re15_port/build -R "r36_ziel1190" --timeout 1500`:
```
1/2 Test #571: unit_r36_ziel1190 ...................   Passed    0.73 sec
2/2 Test #572: integration_r36_ziel1190_scheiben ...   Passed  235.47 sec
100% tests passed, 0 tests failed out of 2
```
(Zweite Gruppe — beruehrte Module — folgt unten.)

### RE-Gate (selbst nachgelesen)
RDT-Bytes (ROOM1190.RDT, eigener Dump):
* main00 @0x0227A `2c 00 01 31 00 00 d0 ee 9c 9b 58 02 60 09 02 00 ff ff 00 00` = sce 1, sat 0x31, floor 0,
  x -4400, z -25700, w 600, d 2400, msg 2, Maske 0xffff — Vorbild der vier Port-Plaetze (HALB_X 300 / HALB_Z 1200,
  SAT 0x31, BAND 0, PAUSE 0xffff): stimmt.
* sub13 Obj_model_set @0x0297E `2d 00 00 00 01 00 88 00 00 00 a4 ac a2 ee 3c a0 …` = Obj 0, x -21340, y -4446,
  z -24516; @0x029A0 Obj 1 z -20916 (`4c ae`): stimmt (Scheiben hinten bei x -21340).
* sub01 @0x02438 `06 00 3c 00 | 21 04 f3 01 | 21 03 6f 00 | 21 05 04 01 | 21 05 05 01 | 21 05 06 00 | 21 05 07 00 | 04 ff 18 0a`
  = Strom an, (3,111)=0, (5,4)=1 (5,5)=1 (5,6)=0 (5,7)=0 -> sub10 = Loesung A (Scheibe 0+2 vorn);
  @0x02478 `… 21 03 70 01 | 21 05 04 00 | 21 05 05 00 | 21 05 06 01 | 21 05 07 01 …` = (3,112)=1 -> Loesung B
  (Scheibe 1+3 vorn): stimmt.
* ROOM1241.RDT sub00 @0x00544 `04 ff 18 02` (sub02 sofort), sub02 @0x0055A `22 03 6f 01`, @0x0055E `22 03 70 01`
  = Elza setzt (3,111)=1 und (3,112)=1 beim Spielstart: stimmt -> Elza = Variante B = Obj 1+3 "viele".
* msg 2 @0x2E5E `04 02 25 00 44 3d 52 41 … 40 4b 08 44 41 4e 41 57 01 00` = Kopf 04 02, Umbruch 0x08, Ende 01 00:
  Form der Port-Texte stimmt.
* Aot_set Knopf 0 @0x02226 `2c 00 05 31 … 00 00 80 ff 00 00` (sce 5, Nutzlast 0/0x80ff), sub00 Member_set
  @0x02336 `34 00 9c eb` = -5220, sub14 @0x02D90 Aot_set slot 0x10 (= letzter Raumplatz 16 -> PORT-WAHL 17..20): stimmt.

PSX.EXE (ghidra1_V2.txt, Zeilen selbst gelesen):
* @0x80042bd0 `ori v0,zero,0x26c` (Vorwaertspunkt 620): stimmt.
* @0x80042cac `lbu v0,2(s0)` / `andi 0x80` / `lbu v1,0x82(s1)` / `bne v1,v0 -> LAB_8004301c` = Band-Gate des Scans
  (Platz-Band == Spielerband, Wildcard 0x80): stimmt.
* @0x80042cd4 `lbu v1,3(s0)`, `andi 0x80` -> @0x80042dc0 `sw zero,local_50` (kein super: Versatz 0);
  `andi 0x40` == 0 -> LAB_80042d58 `andi v1,v1,0x3f`, Stride (((v1<<3)+v1)<<2+v1)<<2 = 0x94,
  @0x80042d7c `lhu v0,0(at)=>DAT_800b3fcc` (Objekt-x), @0x80042db4 `lhu … DAT_800b3fd4` (Objekt-z);
  0x40 gesetzt -> DAT_800acc60/68 Stride 0x1f4 (Gegner). Danach @0x80042e60-84 Ecke = Versatz + rec[4]/rec[6]
  (`lhu v0,local_50` + `lhu v1,4(s0)` / `addu` / `sh`), Breite/Tiefe @0x80042e88-9c unveraendert: die
  "super-Anbindung" ist genau so im Original — der Port rechnet in re15_ziel1190_folgen() Mitte = Objekt-x/z
  (Rechteck relativ = Mitte auf der Scheibe, +-300/+-1200 — PORT-WAHL fuer die relative Lage, gekennzeichnet).
* @0x80042f3c `sh v0,DAT_800b0fd0` (work_vars[0] = getroffener Platz): stimmt. @0x80043098 `lhu a3,2(v0)`,
  @0x800430a4 `sll a3,a3,0x10` (Pausenmaske aus der Nutzlast): stimmt.
* @0x800397ec `addu a0,a0,v0` (Elza-RDT = Basisindex + Elza-Bit): stimmt.
* @0x800381e4 `addiu v0,v0,-0x708` (Klettern Y -= 1800), @0x80038200 `addiu v1,v1,0x1` (Band+1): stimmt.

Guess-Tells im Code-Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO/getenv): KEINE Treffer.
Konstanten ohne Beleg: keine — SLOT0 17 ist als PORT-WAHL gekennzeichnet (Herleitung sub14 @0x02D90).

### Vertrag
* Nachrichten-IDs 6/7 (Zuteilung 1190/1191 = 6..11), Ereignis 27 unbenutzt, keine Bank-9-Bits: eingehalten.
* Pfade (`git diff master --name-only`): analysis/befunde_runde36/H_*, engine/src/{aot_common,scd_room_setup,
  scd_vm,ziel_1190}.c, include/re15_ziel1190.h, platform/pc/main.c, tests/integration/test_r36_ziel1190_scheiben.cmake,
  tests/unit/{probe_r36_ziel1190_karte,test_r36_ziel1190,test_r35_raeume_ziel(geloescht)}.c,
  tests/unit/probes/{r35_raeume,r36_ziel1190}.cmake, UEBERGABE.md (eine Zeile aus dem Merge). Keine release/,
  android/, shared_assets/PSX/, keine Test-CMakeLists: eingehalten.
* Gemeinsame Dateien nur Haken: aot_common.c +1 Zeile (+Include), scd_room_setup.c +1 (+Include), scd_vm.c
  (Haken op_message_on ENTFERNT, Kommentar), main.c +2: eingehalten.
* Tests je Punkt: unit_r36_ziel1190 (Mechanik, 2 Raeume x 3 Zustaende x 4 Scheiben + Knoepfe + Mitfahren) und
  integration_r36_ziel1190_scheiben (exe, 9 Laeufe): vorhanden und messend (laufen gruen, s.o.). Punkt 2 ist
  eine Frage (nur Dossier): kein Test noetig.

### Suite, zweite Gruppe (beruehrte Module aot_common/scd_vm/scd_room_setup/main.c-Ladeweg + Raeume 1190/1090)
`ctest -R "^(unit_1190_weste|unit_1190_ruestung|unit_re2_room1190_ab|unit_r35_raeume_.*|unit_aot_.*|unit_scd_.*|prop_scd_pc_advance|unit_npc_scd_pose|unit_action_msg_gate|unit_msg_dialog|unit_1090_.*|unit_re2_room1090_ab|unit_map_1090|unit_plc_back_yaw_1090|unit_rvd_scan_ungated_by_climb)$"`
-> `100% tests passed, 0 tests failed out of 32` (0.97 s). Fenster-Haken (integration_save_counter_pin) s. unten.

## Punkt 1 — Scheiben-Texte hinten, Knoepfe Original, Elza vertauscht

### 1.0 Messaufbau (eigene Laeufe, ECHTER Weg = Tuer betreten, nicht nur Sprung)
Messkopie `re15_pc_h36abn.exe`; `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1`, Eingabeskript auf der
Spielbild-Achse (`RE15_INPUT_SCRIPT_BASIS=spiel`; der Bildzaehler beginnt bei jedem Raumladen bei 0, main.c:4963/8116 —
das Skript laeuft also in jedem Raum von vorn, im Vorraum reicht es bis zur Tuer-Aktion, im Zielraum bis zur Scheibe).
* Leon, Weg A (Erkundung): `RE15_TITLE_SHOT=title.bmp RE15_DEBUG_JUMP=1180@gp RE15_PLAYER_POS=-8133,10226,1024` (= der
  Punkt, an dem man aus 1190 in ROOM1180 ankommt, Tuer-Record 1180 slot 2 @0x00A54 Rechteck (-9266..-6969, 8888..9822) ->
  1190 Spawn (3174,-11182) dir 1024), Skript `W3,A0.1,…`. debug.log `wd_e1_leon_lauf`:
  `[debug-menu] AUTO-JUMP -> ROOM1180 (Frame 1768)` -> `[aot] DOOR FIRE slot=2 rect=(-8118,9355,hw=1148,hh=467) target_cut=0
  spawn=(3174,0,-11182)` -> `[tuer] Sequenz fertig: 286 Bilder …` -> `[room] PC loaded room1190.rdt` -> VIER Installer-Zeilen
  `[ziel1190] ROOM1190 Scheibe 0 -> Text-Platz 17 (Nachricht 6, viele Einschuesse) bei (-21340,-24516)`, `Scheibe 1 -> 18
  (Nachricht 7, wenige) bei (-21340,-20916)`, `Scheibe 2 -> 19 (6, viele) bei (-21340,-17316)`, `Scheibe 3 -> 20 (7, wenige)
  bei (-21340,-13716)` — der Tuerweg (scd_room_reenter-Installer) legt die Plaetze an den Scheiben HINTEN an.
  Laufmessung (state.log): Laufen 199 Einheiten/Bild (F170 x=1781 -> F180 x=-209), Rueckwand bei x=-20792 ab ~F290,
  Drehung R0.36667 = 11 Bilder = +1056 (1024 -> 2080).
* Elza, ECHTE Tuer: Spielstand ROOM1181 (Elza, Charakter 4) vor der Tuer, aus einer Scratch-Kopie des Kartenwerkzeugs
  (probe_r36_ziel1190_karte.c mit aufgehobener Raum-Sperre, nur im Scratchpad gebaut — kein Code im Baum geaendert),
  `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`. debug.log `wd_e2_elza_tuer`: `[pl] Spieler-Familie PL04
  (character=4, Elza-Bit=1)`, `[save] CONTINUE: resumed in room 1181`, `[aot] DOOR FIRE slot=2 …` -> `[room] PC loaded
  room1191.rdt` -> `[ziel1190] ROOM1191 Scheibe 0 -> Text-Platz 17 (Nachricht 7, wenige …)`, `1 -> 18 (6, viele)`,
  `2 -> 19 (7, wenige)`, `3 -> 20 (6, viele)` — bei Elza ueber die Tuer in die Elza-RDT, Zuordnung vertauscht.

## Punkt 2 — Kiste ROOM1090 (Frage; Dossier §2) — eigene Pruefung

RDT-Bytes (ROOM1090.RDT, selbst gelesen): Obj 1 @0x02190 `2d 01 04 00 05 00 00 01 00 00 b8 e8 d8 dc 86 c3 …` = Typ 4,
Band 5, Flags 0x0100 (Kletterbit), (-5960,-9000,-15482) = DACH; Obj 0 @0x0216E Typ 4 Band 1 (-8136,-1800,4753) = HOF.
Tuer slot 1 @0x0213A `3b 01 02 31 06 00 10 e1 40 ac 68 10 7c 15 | 5c ae 00 00 a0 d7 00 04 00 10 05 00` = **floor 6**,
Rechteck x -7920..-3720, z -21440..-15940 -> Raum 0x10 (ROOM1100) Spawn (-20900,0,-10336) dir 1024 Cut 5: stimmt.
AOT slot 3 @0x0215A `2c 03 03 41 05 00 …  ff 00 18 07` = sce 3, sat 0x41, floor 5, ganze Dachflaeche -> sub07;
sub07 @0x0272C `46 03 … | 2e 03 00 | 34 00 94 d9` = Aot_reset 3, Work_set Obj 0, Member_set x := -9836: stimmt.
SCA (RDT+0x20 -> 0x1DD0, Eintrag = w,d,x,z,4 Bytes): r2[9] @0x01F8C (-10000..4500, -17500..-3600) `01 ff 00 43` (Band 4
-> Oberkante Band 5), r2[0] @0x01F20 Byte 0x53, r2[10] @0x01F98 Byte 0x63, r0[3] @0x01E0C Byte 0x03, r1[18] @0x01F08 Byte
0x23: stimmt. Band-Gate des Scans @0x80042cac-cc (s. RE-Gate).

exe, Lauf `k1090_tor` (`RE15_TITLE_SHOT=title.bmp RE15_DEBUG_JUMP=1090@gp RE15_PLAYER_POS=-5960,-12000,1024,5`
(Band 5 = Dach, y -9000), Skript `U1.5,A0.1,W4,U1.5,A0.1,W4` ab Spielbild 100, `RE15_CLIMB_TRACE`):
```
[input-script] Tick 45 -> F145 Tasten 0x8000      (Aktion vor der Kiste)
climb.txt T3335 pos=(-5960,-9000,-14132) band=5 sub=9 kind=0x81 obj=1   (Klettern auf Obj 1)
climb.txt T3503 pos=(-5960,-10800,-15932) band=6 stand=1               (oben: y -10800 = Band 6, ein Schritt nach vorn)
[input-script] Tick 213 -> F313 Tasten 0x8000
[aot] DOOR FIRE slot=1 rect=(-5820,-18690,hw=2100,hh=2750) target_cut=5 spawn=(-20900,0,-10336)
[tuer] Sequenz Archiv 2 DOOR36 … / Sequenz fertig: 281 Bilder
[room] PC loaded room1100.rdt (181704 bytes)   -> climb.txt T3504 room=1100 pos=(-20900,0,-10336) band=0
```
Gegenprobe `k1090_band5` (Dach, Band 5, Lage (-4000,-15500) Blick -z: Vorwaertspunkt (-4000,-16120) liegt IM
Tuer-Rechteck, aber Spielerband 5 != 6): zwei Aktionen (F100, F163) -> KEIN `DOOR FIRE`, Spieler bleibt
(-4000,-9000,-15500) band=5; AOT-Dump zeigt `[aot] 1 DOOR Mitte(-5820,-18690) halb(2100,2750) … -> ROOM1100 cut=5`.
=> Die Antwort des Dossiers (2.4) ist am Stand belegt: Kiste 1 ist die Stufe auf Band 6, ohne sie ist das Tor nach
ROOM1100 nicht erreichbar. Kiste 0 (Hof): `[prop-member F6] prop[0] obj=0x00 m=0 -> val=-9836` beim Betreten des Dachs
(sub07) im Log von `k1090_tor` gesehen; kein weiterer Record nennt sie (Dossier-Zensus 2.1 gegen den SCD-Dump geprueft:
nur Aot_set slot 3 -> sub07). Urteil: erfuellt (Frage belegt und gemessen beantwortet).

### 1.1 Acht Scheiben-Laeufe ueber die ECHTE Tuer (Spielstand VOR der Tuer in ROOM1180/1181, Aktion, Tuer, Laufweg)
Runner `abn/scheiben_laeufe.sh`: je Lauf Spielstand (Leon `leon1180.mcr` Charakter 0 / Elza `elza1181.mcr` Charakter 4,
Lage (-8133,10226) rot 1024 = 620 vor der Tuer 1180 slot 2), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
RE15_MSG_LOG=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100`, Skript
`W3,A0.1,W2,R0.36667,XU4.4,L0.36667,U<t>,R0.36667,U1,A0.1,W6` (t = 6.05 / 4.45 / 2.85 / 1.25 fuer Scheibe 0..3: im
Vorraum feuert A die Tuer; im Zielraum laeuft das Skript von vorn: Drehung auf -x, Lauf bis zur Rueckwand x=-20792, Drehung
auf -z, Gehen bis zur Scheiben-z, Drehung auf -x, ein Schritt an die Scheibe (Halt x=-20845 = Kollisionsbox), Aktion),
`RE15_FRAMEDUMP=<A+20>-<A+140>/30:fd_`, `RE15_EXIT_AT=<A+170>#<Raum>`, `RE15_STATE_LOG=state.log`.
debug.log je Lauf (alle: `CONTINUE: resumed in room 118x`, `DOOR FIRE slot=2`, `PC loaded room119x.rdt`, Endbild erreicht):
```
L0  [ziel1190] ROOM1190 Scheibe 0 angeklickt (Platz 17) -> Nachricht 6 (viele Einschuesse)
L1  [ziel1190] ROOM1190 Scheibe 1 angeklickt (Platz 18) -> Nachricht 7 (wenige Einschuesse)
L2  [ziel1190] ROOM1190 Scheibe 2 angeklickt (Platz 19) -> Nachricht 6 (viele Einschuesse)
L3  [ziel1190] ROOM1190 Scheibe 3 angeklickt (Platz 20) -> Nachricht 7 (wenige Einschuesse)
E0  [pl] Spieler-Familie PL04 (character=4, Elza-Bit=1) … [ziel1190] ROOM1191 Scheibe 0 angeklickt (Platz 17) -> Nachricht 7 (wenige Einschuesse)
E1  … ROOM1191 Scheibe 1 angeklickt (Platz 18) -> Nachricht 6 (viele Einschuesse)
E2  … ROOM1191 Scheibe 2 angeklickt (Platz 19) -> Nachricht 7 (wenige Einschuesse)
E3  … ROOM1191 Scheibe 3 angeklickt (Platz 20) -> Nachricht 6 (viele Einschuesse)
```
In KEINEM der acht Laeufe eine `[msg] room=119x id=…`-Zeile (kein Knopf-/Original-Text), in jedem genau eine
`angeklickt`-Zeile. state.log L0 bei der Aktion: `F630 pad=8000 PL(-20845,-24299,rot=2080) cam=4 msg(a=1 fsm=1 id=6 q=0)`
(q=0 = keine Ja/Nein-Frage, Nachricht 6 offen; Lage = gemessener Halt vor Scheibe 0).
Bilder (Scratchpad `abn/`, nicht versioniert; selbst angesehen): `wd_s_L0/kontakt_L0.png` (Cut 4, Leon vor der linken Scheibe,
Untertitel tippt "This target has a surprisingly / large number of bullet holes."), `wd_s_L1/kontakt_L1.png` (Cut 5, vier
Scheiben, Leon vor der 2. von links, "This target does not have / many bullet holes."), `wd_s_L2/kontakt_L2.png` (Cut 9,
"surprisingly large number"), `leon_L0-L3_letztes_bild.png` = viele/wenige/viele/wenige; `elza_E0-E3_letztes_bild.png`
= Elza (PL04, roter Anzug) vor Scheibe 0..3 mit wenige/viele/wenige/viele — genau vertauscht.

### 1.2 Knoepfe vorn = Original (Gegenprobe ueber die Tuer, echter Laufweg zum Knopf von Kabine 1)
Lauf `s_K3`: Spielstand Leon ROOM1180 mit Strom ((4,243) in der Karte), Tuer -> 1190, Skript
`W3,A0.1,W2,R0.36667,XU0.7,L0.36667,U6.1,R0.36667,U1.2,A0.1,W6` (oestlich der Kabinen-Trennwaende r0[3]/[4]/[5]/[6]/[9]
x -5060..-2306 entlang x~-1000 nach -z, bei z~-24500 nach -x in Kabine 1 bis an den Tresen). debug.log:
`[room] PC loaded room1190.rdt` -> `[msg] room=1190 id=0 savepoint=0` und KEINE `angeklickt`-Zeile; state.log
`F527 PL(-3602,-24610,rot=2080) cam=7 msg(a=1 fsm=1 id=0 q=1)` (Original-Nachricht 0 mit Ja/Nein). Bild
`wd_s_K3/kontakt_K3.png`: "There's a switch here. Push it?" + Yes/No (Schreibmaschine ab F546), kein Scheiben-Satz.
(Lauf `s_K` mit erstem Skript scheiterte am Laufweg — Leon stand bei x=-2199 in der Trennwand r0[6]; Messfehler, kein Befund.)
AOT-Dump desselben Laufs (`RE15_AOT_DUMP=1`): Raum-Plaetze 0..3 EXAMINE (Knoepfe, Strom an) / 4 DOOR / 5; Port-Plaetze
`17..20 MESSAGE Mitte(-21340,-24516/-20916/-17316/-13716) halb(300,1200) ev=6/7/6/7 sce=31 band=0`; RVD-Zonen 36..63.
Keine Ueberlappung mit Raum-Plaetzen (max. Raumplatz 16, sub14 @0x02D90) oder Kamerazonen.

### 1.3 Urteil Punkt 1: ERFUELLT
Die beiden Saetze kommen beim Anklicken HINTEN an den Scheiben (Leon Obj 0+2 "surprisingly large number", Obj 1+3
"does not have many"; Elza (ROOM1191, PL04) genau vertauscht), die Knoepfe vorn zeigen wieder die Original-Nachricht
(msg 0 bzw. nach den Hunden msg 2 laut unit_r36_ziel1190 (C)). Gemessen ueber die echte Tuer fuer beide Figuren (9 eigene
Laeufe) plus die Riegel des Bau-Agenten (unit + 9 exe-Laeufe ueber den LADE-Weg, gruen).

## Gesamturteil

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Scheiben-Texte hinten, Knoepfe Original, Elza vertauscht | erfuellt | 1.1 (L0..L3 = 6/7/6/7, E0..E3 = 7/6/7/6 ueber die Tuer, Bilder), 1.2 (K3 msg 0), Tests gruen |
| 2 Kiste ROOM1090 — Verwendungszweck | erfuellt | Punkt-2-Abschnitt: RDT-Bytes + exe k1090_tor (Klettern Band 6 -> DOOR FIRE slot 1 -> ROOM1100) + Gegenprobe Band 5 |

Gates: RE-Gate (alle zitierten Stellen selbst nachgelesen, 3+ Adressen disassembliert: @0x80042cac-cc, @0x80042cd4-0x80042e9c,
@0x80042bd0/@0x800397ec/@0x800381e4; RDT-Bytes an 10 Offsets), Vertrag (IDs 6/7, Plaetze 17..20 frei, Pfade, Haken 1-2 Zeilen),
Tests (2 + 32 + 1 Fenster-Haken gruen) — alle gehalten. Keine Maengel. **bestanden.**

## Hinweise (keine Maengel, fuer Orchestrator/Release Notes)
* H1 Stimmen fehlen: `synchro/STAGE1/room1190/main06.wav`, `main07.wav` (Leon), `room1191/main06.wav`, `main07.wav`
  (Elza) — ohne Datei stumm mit Untertitel (ziel_1190.c meldet VOICE_ON 6/7 an). Nutzer-Aufnahme.
* H2 `re15_ziel1190_viele` schluesselt nach der RDT-Variante (Raum-Id & 0xF), nicht nach dem Charakter-Byte; im Spiel
  identisch (Elza immer in "1"-Raeumen, Tuerweg traegt die Variante). In DIESEM Baum fehlt Spur I (Debug-Menue schickt
  Elza noch in Leons Raeume): ein Elza-Sprung nach ROOM1190 zeigte Leons Zuordnung — nach der Integration von Spur I
  (Elza -> 1191) richtig. Kein H-Mangel.
* H3 PORT-WAHL relative Lage: das Original addiert Objekt-x/z zur Rechteck-ECKE (rec x/z, @0x80042e60-84); der Port setzt die
  Rechteck-MITTE auf die Scheibe (+-300/+-1200). Gleiche Mechanik, gekennzeichnet; der Nutzer hat die Lage nicht vorgegeben.
* H4 Suite-Zaehler: 592 - 1 (unit_r35_raeume_ziel entfernt) + 2 = 593 (ctest -N); `RE15_MIN_TESTS` (local_build.sh) steht
  weiter auf 592 — bei der Integration mitheben.
* H5 Beobachtung aus dem AOT-Dump (Original-Daten, nicht H): mit Strom ((4,243)=1) zeigt Tuer slot 4 von ROOM1190 auf Raum
  0x23 = ROOM1230 Cut 7 (sub13 Else-Zweig @0x028D6), ohne Strom auf ROOM1180 (@0x028B2).
* H6 Eingabeskript-Lehre fuer weitere Abnahmen: mit `RE15_INPUT_SCRIPT_BASIS=spiel` beginnt das Skript in JEDEM Raum von vorn
  (Bildzaehler-Reset main.c:4963/8116) — ein Skript "Tuer-Aktion + Laufweg" deckt Vorraum und Zielraum zugleich ab.
