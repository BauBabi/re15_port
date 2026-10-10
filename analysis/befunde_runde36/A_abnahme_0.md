# Runde 36 — Spur A "klettern": Abnahme 0 (unabhaengig)

Datum 2026-10-10. Baum `.claude/worktrees/r36_klettern`, Zweig `r36/klettern`, HEAD d6cc4fd0 (Basis master 78a4a748).
Geprueft wurde der COMMITTETE Endstand; nichts am Code geaendert. Dieser Bericht wird fortlaufend geschrieben.

## 0. Gegenstand

Wortlaut (AUFTRAG.md Z. 31):
> Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B.
> ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich

Behauptung des Bau-Agenten: Kletter-Sonde des Ports verlangte `(flags & 0x101) == 0x101`, das Original
FUN_8002d2c0 ueberspringt nur bei `(flags & 0x101) == 0`; Fix 1c926683 (climb_common.c:99).

Code-Diff gelesen (`git diff master -- re15_port`): Engine-Verhalten aendert sich in genau EINER Zeile
(climb_common.c:99 `== 0u` statt `!= 0x101u`); sonst Kommentare (scd_vm.c, re15_scd.h), ein env-gegateter
Trace-Zusatz (climb_common.c climb_trace, `t4=`), ein Messhaken `RE15_TITLE_SHOT_CHAR` (main.c, 3 Codezeilen),
Tests (2 neue Sonden, 1 Integrationstest, test_1090_climb_pin N4/N4b).

## 1. RE-Gate — Stichproben SELBST disassembliert (re15_disasm.py, info/Re1.5/PSX.EXE)

| # | Adresse | zitiert | selbst gelesen | Urteil |
|---|---|---|---|---|
| 1 | @0x8002d358-35c (FUN_8002d2c0) | `andi v0,v0,0x101` / `beq v0,zero` | Bytes `01 01 42 30 11 00 40 10`; `8002d358: andi v0,v0,0x101`, `8002d35c: beq v0,zero,0x8002d3a4` (Sprungziel 0x8002d3a4 = `addiu s1,s1,-1` / `bgez s1,0x8002d344` = naechstes Objekt). Delay-Slot `addiu s2,s2,-148`. | stimmt — uebersprungen NUR bei 0 |
| 2 | @0x80040990-a4 (LAB_80040914) | `lhu v0,6(a2); ori v0,v0,0x1; sw v0,0(a1)` | `80040990: lhu v0,6(a2)`, `80040998: ori v0,v0,0x1`, `8004099c/a0: sll/sra 16`, `800409a4: sw v0,0(a1)`; davor `80040978: lw v0,0(a1)` / `80040980: beq v0,zero,0x80040990` / `8004098c: sw zero,0(a1)` (Zweitinstallation = 0) | stimmt |
| 3 | @0x8002dd94-98 (FUN_8002dc48) | `andi v0,v1,0x100` / `beq v0,zero,LAB_8002ddc0` | Bytes `00 01 62 30 09 00 40 10`; Treffer @0x8002ddb8 `bne v0,zero,0x8002dd28` (Delay `ori v0,zero,0xd`) -> @0x8002dd2c `sw s0 -> 0x800ac78c`, @0x8002dd30 `sb v0,5(s2)` = Substate 0xd, @0x8002dd3c `sb 1,6(s2)` | stimmt — Bit 0x100 gehoert zur Herunterfall-Pruefung |

Zusaetzlich gelesen: FUN_8002d1e8 @0x8002d1e8 (Punkt 0x233 voraus @0x8002d204, Radius 0x384 @0x8002d224 /
0x2bc @0x8002d280, KEIN Typfilter), FUN_8002da4c @0x8002da4c (Band-Gate @0x8002da58, XZ-Box per
vorzeichenlosem Vergleich @0x8002da8c-c0, KEIN Typfilter), FUN_8002d474 Aufwaertszweig @0x8002d550-61c
(nur Band+1/Band-2-Boxsperren mit Rand 0xc8 @0x8002d5b4/@0x8002d5d4, Substate 9 @0x8002d604, KIND 0x81
@0x8002d614-18 nur bei s5 != 0 @0x8002d610). Deckungsgleich mit climb_common.c:241-275.

## 2. Bau und Tests (Baum r36_klettern, HEAD d6cc4fd0)

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` `=== LOCAL-BUILD-OK (build)`.
  Konsistenz geprueft: `git status` leer; climb_common.c mtime 14:25:29 < climb_common.c.obj 14:25:50 <
  re15_pc.exe 14:25:51; Commits nach 14:25 (fd9c0f48 .. d6cc4fd0) beruehren nur Tests/Dossier.
- Gezielt (`ctest -R "unit_r36_klettern|unit_1090_climb_pin|unit_1090_stay_on_crate|unit_rvd_scan_ungated_by_climb|
  unit_1090_push_pin|unit_1090_push_anim|unit_1090_room_pin|unit_r22_10f0_figur|unit_r35_cut10f0_|
  unit_r35_inventar1050_kiste|unit_1090_npc_prop_push|unit_r20_birkin_push_pin|unit_re2_room1090_ab|
  unit_1090_nodraw_pin|unit_1090_flame_out_pin|unit_r35_raeume"`): **27/27 bestanden** (10,05 s).
- `ctest -R "^integration_r36_klettern$" --timeout 1500`: **Passed 528,92 s**. Nicht nur dem "Passed"
  geglaubt — die Arbeitsverzeichnisse meines Aufrufs (`build/tests/integration/r36_klettern_wd/lauf_c0f8b98f_*`)
  selbst gelesen:
  ```
  A_leon_10f0 : T572 push=5 | T1121 sub=9 kind=0x81 t4fl=0x0001 t4pos=(-4785,-1108) | T1226 pos=(-4899,-1800,-1332) band=1 sub=0 | PL00 | EXIT_AT: Bild 1400 in Raum 1090
  B_elza_10f1 : T590 push=5 | T1137 sub=9 kind=0x81 t4pos=(-4892,-1076) | T1242 pos=(-5015,-1800,-1332) band=1 sub=0 | PL04 (character=4, Elza-Bit=1) | EXIT_AT: Bild 1320 in Raum 1091
  C_leon_szene: T3743 push=5 | T4288 sub=9 kind=0x81 t4pos=(-4970,-1108) | T4393 pos=(-5083,-1800,-1332) band=1 sub=0 | Szene gestartet + zu Ende | EXIT_AT: Bild 4550 in Raum 1090
  ```
- Zensus-Sonde selbst gefahren (`build/tests/unit/probe_r36_klettern_zensus.exe pin`, exit 0): `Zensus: 24/24 Objekte
  (Raum x Lage) kletterbar`, `PASS`; alle 96 Riegel-Seiten `try=1 sub=9 kind=0x81 ... ende=51` und oben (5 Zeilen
  nur durch verschraenkte stdout/stderr-Ausgabe zerschnitten), 3091 obj 1 vier Seiten `reach=1 try=0`.
- Weil der Fix die Aktionstaste in ROOM10F0 veraendert: `ctest -R "^integration_r35_cut10f0$"` **Passed 243,65 s**
  (Runde-35-Szene in 10F0 unberuehrt).
- Module mit portseitigen Null-Box-Objekten (Abschnitt 5): 15/15 bestanden.
- Keine volle Suite im Spurbaum (Auftrag). Kein roter Test in meinen Laeufen, kein Nachfahren noetig.

## 3. Messung des Nutzer-Punkts an der exe (eigene Laeufe, eigene Eingaben)

Messkopie `re15_port/build/platform/pc/re15_pc_abn0.exe` (Kopie der gebauten re15_pc.exe), Arbeitsverzeichnisse
unter dem Scratchpad (`abn0_A_klettern/laeufe/<lauf>`), Basis-Umgebung je Lauf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=1 RE15_CLIMB_TRACE=climb.txt`. (Hinweis zur Zeitachse: mit BASIS=spiel beginnt das Skript
nach JEDEM Raumwechsel neu, weil g_engine.frame_count je Raum neu zaehlt — `[input-script] Tick 330 -> F331`
steht im Log von M0b zweimal.)

**M2b — Tritt in der STARTLAGE, ohne Schieben, von Osten, hoch UND wieder herunter (eigene Eingabe):**
`RE15_DEBUG_JUMP=10f0@250 RE15_SET_FLAG=9:71 RE15_PLAYER_POS=-4875,4950,2048 RE15_INPUT_SCRIPT=W8,A0.1,W4,U0.6,W1,A0.1,W5 RE15_EXIT_AT=700#10f0`
```
T492 room=10F0 pad=8000 pos=(-4875,0,4950) rot=2048 band=0 sub=9 kind=0x81 ph=0 obj=3        <- Aktionstaste: Klettern
T523 room=10F0 pos=(-6023,-1800,4950) band=1 sub=9 ph=3 stand=3                               <- Ausloesebild: Y -1800, Band 1
T544 room=10F0 pos=(-6023,-1800,4950) band=1 sub=0 stand=3                                    <- oben, Standobjekt = Tritt
T615..T621 pos=(-6098..-6500,-1800,4950) band=1 stand=3 mo=105                                <- Gang zur Westkante (Eingrenzung bei -6500)
T663 sub=9 kind=0x02 -> T664 sub=10 -> T678 pos=(-7300,-1210,4950) band=0 -> T682 pos=(-7300,0,4950) -> T702 sub=0   <- Absprung + Landung
```
(Ein erster Versuch M2 ohne den Gang zur Kante sprang NICHT ab: Sonde 800 voraus lag noch in Box+450 des Tritts —
@0x8002d6a0-a8 sperrt; das ist Original-Verhalten, kein Mangel.)

**Gegenprobe V2b — dieselbe Eingabe mit einer exe OHNE Fix** (Hauptbaum `re15_port/build/platform/pc/re15_pc.exe`,
2026-10-04 18:37 = Stand zwischen v0.8.22 und v0.8.23; Kopie `re15_pc_abn0_vorher.exe`, gleiche Assets):
```
T492 room=10F0 pad=8000 pos=(-4875,0,4950) rot=2048 band=0 sub=0 kind=0x00 obj=-1    (grep -c "room=10F0.*sub=9" = 0 von 700 Zeilen)
```
-> Der Befund ist reproduziert, und der Fix (eine Gate-Zeile) erklaert ihn: gleiche Eingabe, vorher kein Start,
nachher Substate 9 / KIND 0x81.

**M0b + M3 — ECHTER WEG mit echter Tuer** (keiner der drei Integrationslaeufe betritt 10F0 durch eine Tuer:
A/C per Debug-Sprung, B per RE15_GOTO_ROOM = statische Spawn-Tabelle). Die Tuer ROOM10D0 -> ROOM10F0
(Door_aot_set @0x01174, Rechteck (300,-8050,1000,2000), Ziel (8400,0,-350) Gierung 2048) ist im Original an
(3,50) gebunden (main00 @0x01022-26 `Ifel_ck`/`Ck bank=3 bit=50`; bei 0 nur Text-AOT + Door sce 0), gesetzt
von sub01 @0x0152A nach dem Kisten-Raetsel (5,13..16). M0 ohne (3,50): Tuer nicht genommen (wie das Original).
M3: `RE15_DEBUG_JUMP=10d0@250 RE15_SET_FLAG=3:50,9:71 RE15_PLAYER_POS=1700,-7000,2048 RE15_INPUT_SCRIPT=W11,A0.1,W1,<Weg Tuer->Tritt>,<Schieben/Klettern/Tuer> RE15_EXIT_AT=60#1090`:
```
[tuer] Sequenz Archiv 2 DOOR13 ...            -> [room] PC loaded room10f0.rdt
T583  room=10F0 pos=(8400,0,-350) rot=2048 ... t4pos=(-6050,4950) t4fl=0x0001     <- durch die Tuer angekommen, Tritt in der Startlage
T1414 room=10F0 pos=(-7125,0,4960) ... mo=18 push=5                                <- Tritt schieben
T1961 room=10F0 pad=8000 pos=(-5013,0,-58) sub=9 kind=0x81 obj=3 t4pos=(-4900,-1108)   <- Klettern (Tritt in der Schiebezone)
T2013 room=10F0 pos=(-5013,-1800,-1206) rot=1024 band=1 sub=0 stand=3               <- oben
T2066 room=10F0 pad=8000 pos=(-5013,-1800,-1332) band=1 stand=3                    <- Aktionstaste an der Tuer
[tuer] Sequenz Archiv 2 DOOR1E ...            -> T2067 room=1090 pos=(-13600,-9000,1300) band=5   <- ROOM1090 erreicht
```
**Urteil Punkt 1: ERFUELLT** — man kann auf den verschiebbaren Tritt in ROOM10F0 klettern (Startlage ohne
Schieben und nach dem Schieben), oben stehen, wieder herunterspringen und von oben die Tuer nach ROOM1090 nehmen;
ueber den echten Tuerweg 10D0 -> 10F0 gemessen, Elza (10F1 -> 1091) im Integrationslauf B. Die uebrigen Tritte/
Kisten derselben Klasse (11A0, 2040, 2060, 20A0, 5020 + Elza-Spiegel) misst der Zensus-Pin in der VM (24/24,
`unit_r36_klettern_zensus` bestanden) — dort kein exe-Lauf (Raeume in Stage 1-5; Kletterstart haengt an derselben
Gate-Zeile).

## 4. Bisect-Aussage geprueft

Gate-Zeile `if ((g_scd.props[p].flags & 0x101u) != 0x101u) continue;` steht WORTGLEICH in v0.8.21 (9362dd7e),
v0.8.22 (87cc8575), v0.8.23 (3ec5b1f8) und master (`git show <tag>:re15_port/engine/src/climb_common.c`, Z. 86);
`git log -S` findet sie nur in b86957e3 (2026-08-21, Anlage von climb_common.c) und im Fix 1c926683;
`git log 9362dd7e..master -- climb_common.c re15_collision.c re15_climb.h re15_collision.h` ist leer. Die
Aussage "keine Regression der 0.8.2x, seit dem Kletter-Port nie kletterbar" ist damit statisch belegt; dynamisch
nur fuer den Stand ~v0.8.23 nachgefahren (V2b), eine v0.8.21-Windows-exe liegt lokal nicht vor (Dossier: K1
auf build_bisect 9362dd7e).

## 5. Nebenwirkungen des breiteren Gates (vom Dossier nur fuer Typ 4 betrachtet)

Das neue Gate laesst JEDES installierte Objekt durch (Bit 0 setzt @0x80040998 immer). Das ist byte-true:
FUN_8002d2c0 prueft nur `andi 0x101` (@0x8002d358), FUN_8002d1e8 und FUN_8002da4c haben keinen Typfilter,
und der Box-Zeiger obj+0x78 zeigt fuer JEDEN Typ auf die Record-Box (@0x800409f4-f8 `addiu v0,a2,22;
sw v0,120(a1)`). Eigener Zensus ueber alle 240 RDTs (scd_dump_room.py, Byte-Auswertung pc[1]/pc[2]/pc[4]/
pc[6..7]/pc[28..33]): 672 Obj_model_set-Records, **619 ohne Bit 0x100** (vorher am Port-Gate blockiert): Typ 0
Null-Box 479, Typ 0 mit Box 39 (alle Band >= 1), Typ 3 Null-Box 76, Typ 4 25 (24 mit Box + 3091 obj 1).

- **Null-Box-Objekte:** der vorzeichenlose Vergleich @0x8002da8c-c0 (Port re15_collision.c:308-311 bildet ihn
  nach) ist bei h=0 und Rand -112 INVERTIERT: Treffer genau dann, wenn |dx| >= 112 UND |dz| >= 112 (nachgerechnet:
  dx/dz 0/0, 0/300, 300/0, 111/111 -> 0; 112/112, 300/300, -500/800, 900/-900 -> 1). Solche Objekte koennen also
  im Original wie im Port einen Kletterstart ausloesen, wenn der Spieler auf IHREM Band steht und schraeg davor.
- **Praktische Reichweite (gemessen/statisch):** Standen auf einem Objekt sperrt die Sonde ganz (@0x8002d2f0
  `DAT_800aca3c & 0x4000`), es zaehlt also nur ein Spieler auf einer Bodenflaeche desselben Bands.
  Band-0-Null-Box-Objekte gibt es nur in ROOM2030/2031 (je 6 Records, Hebebuehnen) — dort steht der Spieler auf
  Band 3 (Lauf M6: `JUMP -> 203 TREATMENT POOL spawn=(6550,-5400,-100)`, 150/150 Zeilen `band=3`; Tueren floor
  3/9). In Leons Stage-1-Raeumen liegen die Nicht-0x100-Objekte fast nur auf Band 1 (Dekor ueber dem Boden);
  Ausnahmen 1090 (Band 2), 1170 (3/5), 11A0 (0/3/5), 11F0 (1/2), 1260 (2). Kein Null-Box-Objekt auf Band b+1
  liegt in Reichweite (Boxhalbe + 563 + 900) einer Band-b-Standflaeche (`zensus_nachbarn.py`: 0 Paare).
- **Portseitig angelegte Objekte** mit Flags 0x0001/0x000B (granate_1150.c:96, sicherung_1150.c:132,
  dokumente_r34.c:91, irons_tisch_1150.c:73, inhalt_r35.c:74): alle Band 1, Null-Box, in 1010/1011/1020/1021/
  1051/1150/1151. Spieler in ROOM1150 gemessen auf Band 0 (Lauf M5: 200/200 Zeilen `band=0`) -> fuer die Sonde
  unerreichbar. Dynamisch nachgefahren: 15/15 Tests dieser Module bestanden (unit_sicherung_1150, unit_r30_granate,
  unit_r30_irons_tisch, unit_r30_irons_diary_dokument, unit_r30_sicherung_nein/_modell/_sitz, unit_r31_hebetisch,
  unit_r32_hebetisch_faecher, unit_r34_plattform, unit_r34n_e_dokumente, unit_r35_granate, unit_r35_inhalt_doku,
  integration_r30_irons_tisch_laden 29,3 s, integration_r34n_e_dokumente_bild 104,8 s).
- **Nicht dynamisch gegen das Original geprueft:** ob das Original an Null-Box-Objekten auf Spieler-Band
  tatsaechlich klettert (DuckStation). Die Gleichheit folgt aus den gelesenen Instruktionen; ein Spiel-Beleg fehlt.

## 6. Vertrag und Dateien

- Geaenderte Pfade (`git diff master --name-only`, ohne Dossier/Belege): climb_common.c, scd_vm.c (nur Kommentar),
  re15_scd.h (nur Kommentar), platform/pc/main.c (Messhaken, 4 Nicht-Kommentar-Zeilen, nur mit RE15_TITLE_SHOT
  aktiv), tests/integration/test_r36_klettern.cmake, tests/unit/probe_r36_klettern_10f0.c, _zensus.c,
  tests/unit/probes/r36_klettern.cmake, tests/unit/test_1090_climb_pin.c (N4 berichtigt, N4b neu). Keine
  release/, platform/android/, shared_assets/PSX/, tests/*/CMakeLists.txt, RELEASE_NOTES.
- Keine Bank-9-Bits, Nachrichten-IDs, Ereignisse belegt (Diff-Suche `flag_set|game_flag|msg_install|
  scd_event_fire|RE15_*_BIT`: keine).
- climb_common.c (gemeinsame Datei): 11 geaenderte Nicht-Kommentar-Zeilen = 1 Logikzeile (Gate) + env-gegateter
  Trace-Zusatz (`t4=`, nur mit RE15_CLIMB_TRACE). Ueber der 1-5-Zeilen-Regel, aber reine Diagnose im bestehenden
  Trace-Block; kein anderer r36-Zweig beruehrt climb_common.c. main.c beruehren auch r36/kleidung (@9289) und
  r36/zombieki (@114, @9684) — Hunks weit auseinander, `git merge-tree` je ohne Konflikt.
- Rate-Signale im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx`): keine.
- Jede verhaltensrelevante Konstante im Code-Diff traegt @0x (Gate @0x8002d358-35c); Test-Konstanten stammen
  aus RDT-Records (@0x010F4/@0x0111A/@0x010E0/@0x01174) oder sind Testplatzierungen.

## 7. Zweiter Tritt-Raum an der exe: ROOM2060 obj 2 (Stage 2) — Nebenbefund

Der Zensus-Pin misst die uebrigen Tritte nur in der VM und endet mit dem Aufstiegsende (kein Normal-/Gegner-
Tick danach). Deshalb ROOM2060 an der exe (Leon, Debug-Sprung, Zustandslog `RE15_STATE_LOG=state.log`):

- **M7/M7b Startlage** (3,172)=0, Tritt (-3125,0,2075): `RE15_DEBUG_JUMP=2060@250 RE15_PLAYER_POS=-3075,3125,1024
  RE15_INPUT_SCRIPT=W8,A0.1,W5 RE15_EXIT_AT=420#2060`
  ```
  T492 pos=(-3075,0,3125) band=0 sub=9 kind=0x81 obj=2           <- Klettern startet (Fix wirkt auch hier)
  T523 pos=(-3075,-1800,1977) band=1 sub=9 stand=2               <- oben
  T544 pos=(-3075,-1800,1977) band=1 sub=0 stand=2
  T545 pos=(-3687,-1800,3125) band=1 sub=0 stand=-1 rc_b8=0      <- im ersten Normalbild von der Trittflaeche geschoben
  T546 pos=(-4200,-1800,2642) ... T749 pos=(-5139,-1800,-2561) band=1 stand=-1   <- bleibt auf y=-1800 ohne Standobjekt ("schwebt")
  state.log F293: [2 t=25 st=1 ss1=0 ... d=295 @(-2918,1726,r3000)] = adulte Spinne (Typ 0x25, Slot 2) direkt neben
  dem Tritt (Slot 1, ebenfalls t=25, @(-3150,3714)); PL hp=60
  ```
  Schieber = Koerper-Push der Spinne: Box @0x80118dd4 off(0,-900,0) half(1000,900,1000) (re15_damage.c:3787),
  R = 450 + 1000, Abstand 296 -> Pen 1154, Versatz x = -157*1154/297 = -610 (gemessen -612).
- **M8 Endlage** (3,172)=1, Tritt (475,0,-7375), Anlauf von Sueden: Aufstieg T492-T544, dann schiebt dieselbe Spinne
  (`@(718,-6717)`) Leon von (525,-7473) auf (25,-7750) — die Eingrenzung haelt ihn auf dem Tritt (`stand=2` bis zum Ende).
- **Einordnung (selbst disassembliert):** FUN_80031c44 ruft den Koerper-Push @0x80031cbc `jal 0x8002b544` VOR dem
  Standobjekt-Test @0x80031ce8 `jal 0x8002d100` (a1=0x12); danach nur FUN_8002dc48 @0x80031d38 (betrachtet
  ausschliesslich 0x100-Objekte, @0x8002dd94), Wandklemme @0x80031d70 und FUN_80037358 @0x80031d78 (Kopf-/
  Nacken-FSM, `jal 0x8003790c`). Eine Bodenhoehen-Nachfuehrung fuer einen von einem Nicht-0x100-Objekt
  geschobenen Spieler gibt es dort NICHT. Der Port bildet diese Reihenfolge nach; die geaenderte Gate-Zeile ist
  nicht beteiligt (das Schweben wuerde an einem 0x0101-Objekt mit Spinne daneben genauso entstehen). Offen und
  nur dynamisch klaerbar: ob die Spinne im Original Leon auf dem Tritt ueberhaupt erreicht — das Y-Band-Gate
  @0x8002b1f0-b228 (Port enemy_ai_common.c:4435-4439) verlangt |dy| < 1530 + 900 = 2430, bei Leon y=-1800 also
  eine Spinne oberhalb y=0; die Spinnen-Hoehe im Original ist nicht gemessen.
- -> Kein Mangel der Spur (Original-Reihenfolge, keine Gate-Abhaengigkeit), aber ein durch den Fix NEU
  erreichbarer, sichtbarer Zustand. Als OFFEN-Hinweis gefuehrt (H1).

## 8. Urteil je Punkt

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1. "kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmoeglich" | **erfuellt** | M2b (Startlage hoch + Kante + Absprung), M3 (echte Tuer 10D0 -> 10F0, Schieben, Klettern, Tuer -> ROOM1090), integration_r36_klettern A/B/C (Leon + Elza 10F1 -> 1091, mit Szene), Gegenprobe V2b ohne Fix = kein Start; weitere Tritte: Zensus-Pin 24/24 (VM) + ROOM2060 an der exe (M7b/M8: Aufstieg gelingt in beiden Lagen). Fix = byte-true @0x8002d358-35c (selbst disassembliert). |

## 9. Maengel

1. **RE-Aussage widerlegt (Dossier + Sondenkommentar):** `A_klettern.md` (Zensus-Pin-Tabelle Zeile 3091 "kein Tritt:
   ohne Box trifft FUN_8002da4c mit Rand -112 nie"; Text "3091 obj 1 (Null-Box @0x01508) nur Diagnose: kein Start,
   wie FUN_8002da4c mit Rand -112") und `re15_port/tests/unit/probe_r36_klettern_zensus.c:16-17` behaupten, ein
   Null-Box-Objekt treffe FUN_8002da4c mit Rand -112 nie. Die Instruktionen @0x8002da8c-c0 (`lhu v0,6(a2)` +
   Rand, `sll v0,v0,1`, `sltu v0,v0,a1` vorzeichenlos; Port re15_collision.c:308-311 identisch) treffen bei h=0
   und Rand -112 genau dann, wenn |dx| >= 112 UND |dz| >= 112 (nachgerechnet: 0/0, 0/300, 300/0, 111/111 -> 0;
   112/112, 300/300, -500/800, 900/-900 -> 1). Die Sonde fuhr nur achsgerade Anlaeufe (Querabstand 0) und hat
   deshalb "kein Start" gesehen. Folge: 3091 obj 1 (Flags 0x0081, vorher am Port-Gate blockiert) passiert jetzt
   das Gate und ist von SCHRAEG vorne startbar, sobald ein Spieler auf Band 1 in Reichweite steht — genau das,
   was die Aussage ausschliesst. Nachpruefbar: Zahlen oben bzw. eine Seite mit Querversatz >= 112 in
   probe_r36_klettern_zensus (z.B. Spieler 300 seitlich versetzt). Abhilfe: Aussage im Dossier und im
   Sondenkommentar berichtigen (und 3091 obj 1 mit Schraeganlauf diagnostisch fahren); kein Code am Fix noetig.

## 10. Hinweise (kein Mangel der Spur)

- **H1 (OFFEN, Abschnitt 7):** ROOM2060 Startlage — die adulte Spinne (Typ 0x25, Slot 2) schiebt Leon im ersten
  Bild nach dem Aufstieg von der Trittflaeche; Leon bleibt auf y=-1800/Band 1 ohne Standobjekt. Original-
  Reihenfolge belegt (@0x80031cbc vor @0x80031ce8, keine Bodenhoehen-Nachfuehrung), Erreichbarkeit im Original
  (Spinnen-Y im Y-Band-Gate @0x8002b1f0-b228) unbelegt. Messweg: DuckStation ROOM2060 per Debug-Menue, Tritt in der
  Startlage von Sueden besteigen, Spieler-Y (0x800aca54+0x38) und Spinnen-Y nach dem Aufstieg lesen.
- **H2:** Kommentare in fremden Modulen beschreiben jetzt die alte, falsche Gate-Semantik: granate_1150.c:96
  `0x0001 /* aktiv, nicht kletterbar */`, sicherung_1150.c:132 `/* aktiv, NICHT kletterbar (kein 0x100) */`.
  Diese Objekte passieren das Gate jetzt; unerreichbar sind sie nur, weil sie auf Band 1 liegen und der Spieler in
  1150/1151 auf Band 0 steht (gemessen M5). Kein Verhaltensfehler.
- **H3:** Der Dossier-Zensus betrachtet nur Typ-4-Objekte; das neue Gate oeffnet 619 Records (Abschnitt 5). Meine
  Nachpruefung fand keine praktische Reichweite ausser H1-artigen Faellen; ein Original-Abgleich fuer Null-Box-
  Objekte auf Spieler-Band fehlt.
- **H4:** climb_common.c traegt 11 geaenderte Nicht-Kommentar-Zeilen (VERTRAG 1.4: 1-5) — 1 Logikzeile + Trace-
  Diagnose hinter RE15_CLIMB_TRACE; kein anderer r36-Zweig beruehrt die Datei.
- **H5 (Nebenbeobachtung, Spur I):** `RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=3091@250` landet als Elza in ROOM3090
  (`JUMP -> 309 T CORRIDOR (ROOM3090)`), das Debug-Menue-Thema der Spur I.
- **Ablauf:** Das gemeinsame Scratchpad dieser Sitzung enthielt eine fremde `lauf.sh`, die ich versehentlich
  ueberschrieben habe; meine Fassung liegt jetzt unter `abn0_A_klettern/abn0a_lauf.sh`, an der alten Stelle liegt
  keine Datei mehr (ein Aufruf scheitert sichtbar statt still gegen den falschen Baum zu messen).

## 11. Ergebnis

**bestanden = NEIN** — wegen Mangel 1 (eine widerlegte RE-Aussage in Dossier und Sondenkommentar). Der Fix
selbst ist byte-true belegt (@0x8002d358-35c, selbst disassembliert), erklaert den Befund (gleiche Eingabe: ohne
Fix kein Start, mit Fix Substate 9 / KIND 0x81) und erfuellt den Nutzer-Punkt ueber den echten Weg (Tuer
10D0 -> 10F0, Schieben, Klettern, Tuer nach ROOM1090; Elza 10F1 -> 1091). Fuer eine Abnahme 1 genuegt die
Berichtigung der 3091-Aussage (Dossier + probe_r36_klettern_zensus.c:16-17), am Engine-Code ist nichts zu tun;
H1 (ROOM2060 Spinne) gehoert als OFFEN-Punkt mit Messweg ins Dossier.

## 12. Messprotokoll (Laeufe dieser Abnahme)

| Lauf | Zweck | Kern-Umgebung | Ergebnis |
|---|---|---|---|
| M0 | Tuer 10D0 -> 10F0 ohne (3,50) | DEBUG_JUMP=10d0@250, PLAYER_POS=1700,-7000,2048, A bei F331 | Tuer nicht genommen (Original-Sperre @0x01022-26) |
| M0b | dasselbe mit (3,50) | + SET_FLAG=3:50,9:71 | DOOR13, T583 room=10F0 (8400,0,-350), Tritt (-6050,4950) Flags 0x0001 |
| M2 | Startlage hoch, Absprung ohne Kantengang | DEBUG_JUMP=10f0@250, PLAYER_POS=-4875,4950,2048 | hoch ja (T492 sub=9), Absprung von der Mitte gesperrt (@0x8002d6a0-a8, Original) |
| M2b | Startlage hoch + Kante + Absprung | + Skript W8,A0.1,W4,U0.6,W1,A0.1,W5 | T492 sub=9/0x81, T544 oben, T663-T702 sub=10, Landung (-7300,0,4950) |
| V2b | Gegenprobe ohne Fix (Hauptbaum-exe 2026-10-04 18:37) | wie M2b | T492 sub=0, 0 Zeilen sub=9 |
| M3 | echter Weg | DEBUG_JUMP=10d0, SET_FLAG=3:50,9:71, Tuer, Weg, Schieben, A, U0.6, A | T1414 push=5, T1961 sub=9/0x81, T2013 oben, T2067 room=1090 |
| M5 | Spieler-Band ROOM1150 | DEBUG_JUMP=1150@250 | 200/200 band=0 |
| M6 | Spieler-Band ROOM2030 | DEBUG_JUMP=2030@250 | spawn (6550,-5400,-100), 150/150 band=3 |
| M7/M7b | ROOM2060 Startlage | DEBUG_JUMP=2060@250, PLAYER_POS=-3075,3125,1024, STATE_LOG | Aufstieg ja, T545 von Spinne geschoben, schwebt (H1) |
| M8 | ROOM2060 Endlage | + SET_FLAG=3:172, PLAYER_POS=525,-6325,1024 | Aufstieg ja, geschoben, bleibt stand=2 |
| M9 | 3091-Erreichbarkeit (Elza) | TITLE_SHOT_CHAR=1, DEBUG_JUMP=3091@250 | Sprung landet in ROOM3090 (H5), nicht wertbar |

Alle Laeufe: Messkopie `re15_pc_abn0.exe` (bzw. `re15_pc_abn0_vorher.exe` fuer V2b), Logs `climb.txt`/`debug.log`/
`state.log` im Scratchpad `abn0_A_klettern/laeufe/<lauf>`; Zensus-Skripte `abn0_A_klettern/zensus_*.py`.
