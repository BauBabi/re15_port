# Runde 36 — Spur F "zombieki": Abnahme 0

Datum: 2026-10-10. Pruefer: unabhaengige Abnahme 0 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **853cbfb3** (Basis master 78a4a748). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md.

> **ERGEBNIS: NICHT BESTANDEN.** P1 teilweise, P2 teilweise, P3 erfuellt. Gates Bau/Tests/RE-Zitate/Vertrag halten;
> Maengel 1-8 in Abschnitt 5. Alle Messungen an der gebauten exe dieses Baums gegen die v0.8.23-exe (Spielcode = master).

## 0. Stand, Bau, Messmittel

- `git log master..HEAD`: fb07381d, 490f1be5, 5e223660, a0e481eb, (Merges 1988ad74, 8c223733), 3df50132, fee4e956,
  72693646, 853cbfb3. Arbeitsbaum sauber.
- Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK`, "ninja: no work to do" (exe
  15:59:32 nach der letzten Quellaenderung 15:59:28, Baum sauber = exe entspricht HEAD).
- Messkopien (eigener Name, neben der exe): `re15_pc_abn0f_neu.exe` (= re15_pc.exe dieses Baums, md5 5b13884a...),
  `re15_pc_abn0f_basis.exe` (= Hauptbaum-exe 2026-10-04 18:37, md5 2fd81d20...; Spielcode = v0.8.23 = master
  78a4a748, `git diff --stat 3ec5b1f8 78a4a748 -- re15_port/engine re15_port/platform re15_port/include` leer).
- Logs: `RE15_GEGNER_Y_LOG` (trage_1200.c: je Bild Spieler + je Gegner st/Lage/los), ausgewertet mit eigenem Skript
  (Scratch `gy_sum.py`: erste Reaktion, los-Anteil, Weg, Abstand, Stillstand-Strecken >= 90 Bilder < 40 Einheiten).

## 1. Zwischenstand Messungen (fortlaufend)

### P1 exe ROOM10B0 (Basis gegen neu), Leon (-17000,-8500) unterhalb der Terrasse, 2400 Bilder
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10B0@250
RE15_PLAYER_POS=-17000,-8500,2048,0 RE15_AI_FLAVOR=re2 RE15_GEGNER_Y_LOG=gy.log RE15_EXIT_AT=2400#10B0`
- Basis: alle vier Terrassen-Zombies (Slot 1-4, Typ 0x10, Band 1) reagieren NIE (los=1 0/2400), Stillstaende bis 1279 Bilder.
  Befund reproduziert.
- Neu: alle vier reagieren in Bild 7 (Sub 1), los=1 2395/1253/1009/582 von 2400 — laufen aber nach OSTEN von Leon WEG
  (Abstand 3274..4637 -> Ende 10713/11477/12252/13013) und treten ab F777..F1441 bei x -6818/-6017/-5210/-4418, z -11832
  (Terrassenkante) bis Bildende auf der Stelle (Stillstand bis 1265 Bilder). Leon wird nie erreicht.

### P1 exe ROOM1030 (Innenraum, Debug-Spawn (-8200,-22500) = Nordseite des Tresens), 1500 Bilder
- Basis und neu Bild fuer Bild GLEICH (Lage, Weg, Stillstaende): sechs Zombies Typ 0x16 laufen ab Bild 6 los, bleiben am
  Tresen (z -24838/-25532) 2526..4041 vor Leon stehen bzw. treten auf der Stelle (Stillstand bis 395 Bilder); einziger
  Unterschied los 0 -> 1. Alle drei Tueren des Raums (AOT-Dump: ->1050 (2250,-12700), ->1070 (-16150,-3800), ->1040
  (-26900,-3650)) liegen auf Leons Seite; Nav-Zonen 1/8/9 (ROOM1030.RDT blk @0x1bb8) verbinden Sued-Zone 0 und Nord-Zone 2.

### P2 Zufallsgang-Statistik (Scratch-Kopie der Sonde mit Schalter ABN_PADSUCHE, gegen die gebauten Bibliotheken)
52 Seeds x 6000 Bilder x 2 Gorillas: blockierte Gorilla-Bilder AN 38592 / AUS 47234 (-18 %), Griffe 234 / 226; Seeds
besser 19, schlechter 17, gleich 16; Riegel-Seed 0x1234567: AUS 0 blockiert, 9 Griffe -> AN 2067 blockiert, 3 Griffe.
Rest-Blockade AN: Sub 4 36094, Sub 3 2314, Sub 7 184 Bilder (93,5 % Sub 4).

### P2 exe ROOM11C0 Kampf-Layout (Basis gegen neu), Leon still nordlich der Wagen (-7000,12500)
`RE15_SET_FLAG=4:64` (sub00 @0x1768 / sub01 @0x181C: Ck(4,0x40) -> keine Intro-Szene, Kampf-Layout) `RE15_DEBUG_JUMP=11C0@240
RE15_PLAYER_POS=-7000,12500,1024,0 RE15_GEGNER_Y_LOG=gy.log`
- Basis: Slot 2 (Start (-9013,-15461)) nie Angriff, 1634 Bilder fest (Sub 3, z -12016); Slot 3 Angriff F1012.
- Neu: Slot 2 Angriff F1000 (Pad + Sprung); Slot 3 (Start (9434,2189)) von der Pad-Suche nach Sueden zum Ost-Pad geschickt,
  danach nie Angriff, 1568 Bilder fest in Sub 4 bei (4786,-8892), 25000 von Leon.
  => Die Blockade wandert vom einen zum anderen Gorilla; fuer den Nutzer bleibt "ein Affe haengt".

### P3 exe ROOM11C0 Kampf-Layout, Gorilla Slot 2 mit der Schrotflinte erschossen (Basis gegen neu)
`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-9013,-11800,1024,0 RE15_GIVE=7:200 RE15_EQUIP=7
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=250 RE15_INPUT_SCRIPT=W0.1,(MA0.1,M0.6 x40) RE15_FORCE_CUT=4
RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=600-1000/10:fd_ RE15_GEGNER_Y_LOG=gy.log RE15_EXIT_AT=1000#11C0` (beschleunigter Renderer;
ohne FORCE_CUT bleibt die Kamera nach dem Sprung auf Cut 0 = leere Ecke, Leiche (-10994,-10275) liegt in den Zonen #51/#55 = Cut 4)
- Beide: Slot 2 Zustand 3 ab F631, Leiche (Zustand 7) ab F671 bei (-10994,-10275).
- Pixelvergleich Basis gegen neu (Vollbild 320x240, "neu rot-dunkler" = G/B stark abgezogen, R weniger = subtraktive Lache
  0x00ffff38): F660 0, F670 2, **F680 490, F690 619, F700 733, F710 810, F720 864, F740 936, F760 1042, F780..F1000 1046**
  (konstant). Wachstum ab dem ersten Leichentakt, Ende nach ~91 Bildern (F671 + 91 = F762) — passt zu FUN_8011be54.
  Sichtbild: Basis F800 tote Gorilla ohne Lache, neu F800 dunkelrote Lache links unter der Leiche (Scratch-Bilder
  `p3_png/cut4_F800.png`, `neu_cut4_F690_F780.png`; nicht eingecheckt, Regel "nur den Bericht committen").
- Nebenwirkung: der LEBENDE Gorilla wirft jetzt den Record-Schatten 1700/1800 statt 500x600 (F640: 2937 Pixel gleichmaessig
  dunkler, grosser schwarzer Fleck unter dem Gorilla). Folgt aus den disassemblierten INIT-Werten (unten), Konvention wie
  Zombie 600/700 @0x80100c2c/c38.

## 2. RE-Gate — selbst disassembliert (re15_disasm.py / re2_disasm.py, richtige Binaerdatei)
| Zitat im Diff | Datei | gelesen | Urteil |
|---|---|---|---|
| `ori a2,zero,0xf00` @0x8001b8d4, `jal 0x8003dcc4` @0x8001b8e8, `ori a3,zero,0x300` @0x8001b8ec | PSX.EXE | genau so (a3 im Delay-Slot) | stimmt |
| Band `lbu v1,130(v1)`/`sra v0,v0,28`/`bne` @0x8003de60-6c, Nibble `and v1,t5,a0`/`bne` @0x8003de74-94 | PSX.EXE | genau so | stimmt |
| zweiter Aufrufer `ori a2,zero,0x400`/`ori a3,zero,0x400` @0x8001bbcc-d4 | PSX.EXE | genau so | stimmt |
| RE2 `addiu a2,zero,8192` @0x8004a868, `addiu a3,zero,1` @0x8004a86c, `andi 0xf7ff`/`sh` @0x8004a87c-80, `jal 0x80050858` @0x8004a8d8, `ori 0x800`/`sh` @0x8004a8f0-f4 | re2leon PSX.EXE | genau so (a2 im Delay-Slot von bne @0x8004a864) | stimmt |
| Skip-Tabelle @0x800A73B4 = [0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0], `and v0,v1,fp`/`beq` @0x800508bc-c8, `lbu ...29620(at)` @0x800508dc | re2leon PSX.EXE | genau so | stimmt |
| Hoehe `-(w>>11)*100 - ((w>>6)&0x1f)*1800` @0x80050b34-74 | re2leon PSX.EXE | Schiebe-Ketten = x*1800 bzw. x*100, `subu t4,v0,a0` | stimmt |
| `+0x1e2 = 4` @0x80117098-9c; Tabelle 0x80121368[4] = 0x80121350; u16 @0x80121356 = 1600; `addiu a2,a3,100` @0x801171e8; `jal 0x8001af5c` @0x801171ec; `addiu a3,a3,200` @0x801171f0 | STAGE1.BIN | genau so; Record 0x80121350 = [0,-1440,0,1600,1440,1600] | stimmt |
| FUN_8001af5c `sh s3,12(s0)` @0x8001b030, `sh s4,14(s0)` @0x8001b038 (s0 = sp+64 = Aufrufer sp+16 = entity+0xB0) | PSX.EXE | genau so | stimmt |
| FUN_8011be54: `lui a2,0xff` @0x8011be6c (Delay-Slot), `ori a2,a2,0xff38` @0x8011bed0, +0xC4/+0xEC @0x8011bee0-f00, `addiu 8` + `sh 188/190` @0x8011bf08-14, `ori v0,zero,0x5a`/`sb 158` @0x8011be80-84, Dekrement @0x8011bf24-34, `sb 2,7` @0x8011bf40-44 | STAGE1.BIN | genau so (91 Wachstumstakte inkl. Durchfall) | stimmt |
| A[3] `sltiu v0,v0,0x1771` @0x80117bc8 und @0x80117c34, fern+Sicht -> `ori v0,zero,0x4`/`sb v0,5` @0x80117c50-54, sonst Return @0x80117c78 | STAGE1.BIN | genau so; kein Ausgang "ohne Sicht" | stimmt |
| B[3] rng `andi 0x1f`/`addiu 6`/`sb 158` @0x80117d1c-34, `addiu a0,a0,-13688` (0x800aca88 playerX) @0x80117d4c, `jal 0x8001a8f8` @0x80117d50 | STAGE1.BIN | genau so | stimmt |
| A[4] FUN_8003b93c(+0x34, rec[6]+100, +0x82, 0x10) @0x80117ee0-f00, Fenster @0x80117f18-38, `sb a1,159` @0x80117f40, +5/6/7 = 7/0/1 @0x80117f50-74 | STAGE1.BIN | genau so | stimmt |
- PORT-WAHL-Konstante 3000 (Laenge des Richtungs-Zielpunkts): `re15_enemy_steer_point` (enemy_ai_common.c:3526) nutzt nur
  den Winkel atan2 -> wirkungsneutral, als PORT-WAHL gekennzeichnet.
- Suche im Diff nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/getenv: **0 Treffer**. Neue Env-Schalter:
  keine (die `*_enable`-Schalter sind nur fuer Sonden, vom Spiel nie gerufen).

### P1 exe ROOM1010 Verhoerraum (Basis gegen neu), Leon (3750,4300) 800 vor Slot 1, 1200 Bilder
`RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3750,4300,1024,0 RE15_AI_FLAVOR=re2 RE15_GEGNER_Y_LOG=gy.log RE15_EXIT_AT=1200#1010`
(`@gp` aus dem Dossier feuert im Intro von ROOM1240 nicht — Lauf damit endete ohne Sprung in ROOM1170; `@250` geht.)
- Basis und neu Bild fuer Bild GLEICH:
  - Slot 1 (3750,3500): Reaktion F7, Griff, Rueckstoss in die Tasche, danach Stillstand F107-F1200 (1094 Bilder) bei
    (2933,1918), 2172 vor Leon (der im Dossier selbst gemessene "Taschen-Fall" — unveraendert).
  - **Slot 2 (205,7248) rot 0: reagiert NIE.** F100..F1200: st 1/0/1/0, Leon (3669,3962) 4775 entfernt, Winkel +494 im
    Kegel +-1024, Abstand < 0x1388; Selbstwecker t158 308 -> 0 (F600) ohne Wirkung; los=0 durchgehend.
- Wer die Sicht nimmt (RE1.5-Strahl wie `re15_los_ray_blocked` nachgebildet: Band, Nibble 3, u0&4, Diagonal-Straddle /0x12,
  ROOM1010.RDT SCA): **#5 typ 1 x -50..2450 z 1550..5900 (2500 x 4350) und #12 typ 3 Kreis 1200 bei (2450,4700)** — der
  niedrige Verhoertisch und ein Rundkoerper an seiner Ecke (Framedump F60 zeigt den Tisch, Scratch `p3_png/r1010_F60.png`;
  das Dossier nennt Zelle 5 selbst "hinter Moebeln"). Beide tragen Nibble 3. Leon an (3200,5000), 3745 vor Slot 2, ist
  ebenso blockiert; erst ab (3300,6400) frei.
- Der Zensus fand das nicht: Raster nur bis 4000 und Bodentest r 450 gegen die Zell-Rechtecke -> oestlich des Tischs liegt
  fuer Slot 2 kein gueltiger Rasterplatz <= 4000; fuer Slot 1 stehen 3 Plaetze (1350..2150, 6700) hinter #5/#12 — im Dossier
  als "hinter Nibble-3-Zellen (Waende)" gezaehlt.
- Das Dossier selbst sagt zu RE2 (Ziel): "niedrige Saetze (Tische, Tresen, Kanten) nehmen dem RE2-Zombie keine Sicht"
  (Hoehenpfad @0x80050b34-74, selbst bestaetigt, Tabelle oben). Die gewaehlte Abbildung "nur Nibble 3 blockt" laesst den
  Nibble-3-Tisch aber blocken.
- EINSCHRAENKUNG (selbst geprueft): die Lage ist per Sprung KONSTRUIERT. Das Sicht-Bit wird im RE2-Hirn nur zum WECKEN
  gelesen (enemy_ai_re2_zombie.c: DECISION @0x80101318-1C, Wecker @0x80101544-7C und @0x80101674-A8, @0x80103AE8-F4) —
  kein Leser haelt einen laufenden Zombie an. Beim normalen Tuereintritt aus ROOM1020 (Spawn (3650,6900) yaw 2048, Runde 35
  F_inhalt) sieht Slot 2 (Blick +X) Leon zuerst frei und laeuft los. Der Fall belegt also die Luecke der Abbildung
  (Nibble-3-Moebel nehmen die Sicht), nicht eine Normalfluss-Reproduktion des Nutzer-Innenraumfalls. Eine solche hat weder
  der Bau-Agent noch diese Abnahme gefunden.
- Wirkung des Fixes innen laut eigenem Vorher-Zensus (`F_belege/p1_sicht_vorher.txt`): genau EIN Platz (ROOM11E0 Slot 1,
  Reaktion 368 Bilder nur wegen des Slab-Tests); kein innen stehender Zombie wurde durch den Fix geweckt.
- ROOM1030 (Tresen/Tor) dagegen ist KEIN Gegenbeleg: main00 setzt die Zombies bedingungslos (Sce_em_set @0x1DE6..0x1F62),
  das Haengen am Tor ist die geskriptete Kriech-Szene mit Scharfschalt-Budget (probe_1030_stuck_who, im Original genauso).
  Die Dossier-Aussage "kein Spielweg" ist aber falsch (alle drei Tueren liegen auf Leons Seite).

## 3. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Zombies bleiben ewig beteiligungslos stehen, fast vor der Nase, auch innerhalb eines Raumes | **teilweise** | ROOM10B0 (einziger reproduzierter Fall): Terrassen-Zombies reagieren jetzt (F7 statt nie), laufen aber von Leon weg und treten 10700..13000 entfernt an der Kante auf der Stelle — Leon wird nie erreicht. Innenraum-Teil des Wortlauts: nicht reproduziert, Fix wirkt innen an 1 Zensus-Platz; per Sprung (ROOM1010) steht ein Zombie in BEIDEN Staenden 1200 Bilder 4775 vor Leon, Sicht vom Nibble-3-Verhoertisch genommen (Abbildungsluecke gegen die eigene RE2-Lesart). |
| 2 Monkeys ROOM11C0 oft blockiert — verbessern | **teilweise** | Riegel-Szenen (blinde Jagd fern an der Klemme) jetzt mit Angriff. Aber exe Kampf-Layout: die Blockade wandert nur (Basis Slot 2 1634 Bilder fest, neu Slot 3 1568 Bilder fest in Sub 4 — von der Pad-Suche weg von Leon nach Sueden gesprungen). Zufallsgang 52 Seeds: -18 % blockierte Bilder, aber 17 Seeds schlechter, Riegel-Seed 0 -> 2067; 93,5 % der Rest-Blockade in Sub 4 unbehandelt. |
| 3 Blut der toten Monkeys laeuft nicht aus | **erfuellt** | exe Cut 4: Lache ab F680 (490 Px) monoton bis F760 (1042 Px), dann konstant; Basis ohne Lache. Konstanten selbst disassembliert (Gorilla-INIT 1600+100/+200, FUN_8011be54 Tinte/+8/0x5a). |

## 4. Gates

- **Bau:** OK (local_build.sh configure + build).
- **Spur-Tests** `ctest -R r36_zombieki`: 5/5 gruen (affen 9/9, szenen 5/5, affen_block 7/7, wecker, sicht 6/6; Zahlen
  des Dossiers reproduziert: Sicht-Zensus 45 Zombies / 2657 Plaetze / STEHT 250, Gegenprobe 10B0 68).
- **Gezielte Regression** (153 Tests: affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|1140|1210|1100|10d0|
  r35_raeume|r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|r34n_f, `-j 6 --timeout 600`):
  **153/153 bestanden** (516 s), darin alle 20 unit_r35_affen_*, unit_maggot_*, alle re2z/re2_zombie/zombie, Hund/Kraehe,
  1030/1040/1140/1210/1100/10d0, r35_raeume_*, r35_inhalt_zombies, unit_r35_cut1150_* + integration_r35_cut1150 (508 s),
  integration_r35_zgirl/_5090.
- **Volle Suite:** vom Bau-Agenten fuer den Endstand nicht gruen gefahren (eigene Angabe); laut Auftrag Sache des Orchestrators.
- **RE-Gate:** alle 12 geprueften Zitate stimmen (Abschnitt 2); keine Guess-Tells, keine Env-Schalter als Abschluss;
  Nutzer-Vorgabe/PORT-WAHL gekennzeichnet (Pad-Suche, 3000). Inhaltlicher Bruch siehe Mangel 1: die Abbildung "nur Nibble 3
  blockt" widerspricht beim Nibble-3-Tisch der eigenen RE2-Lesart (Hoehenpfad).
- **Vertrag:** Pfade OK (`git diff master --name-only`: nur analysis/befunde_runde36/F_*, engine/src/{enemy_ai_common.c,
  enemy_ai_re2_zombie.c, zombieki_r36.c}, include/re15_zombieki_r36.h, platform/pc/main.c, tests/unit/probes/r36_zombieki.cmake,
  tests/unit/test_r36_zombieki_*.c; kein release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists). Haken in
  gemeinsamen Dateien klein (main.c 1 Code-Zeile, enemy_ai_re2_zombie.c 1 Zeile, enemy_ai_common.c Haken je 1-2 Zeilen +
  6-Zeilen-Export re15_r36_los_ray_nib3 im eigenen Sichtfilter-Bereich der Spur F). Keine Bank-9-Bits, Nachrichten-IDs,
  Ereignisse belegt. Tests ueber die eigene probes-Datei registriert.
- **Nebenwirkungen:** Gorilla-Haken greifen nur Typ 0x27. Sce_em_set-Zensus aller RDTs (scd_dump_room.py): Typ 0x27 in
  ROOM11C0 (Leon, 4 Records) und **ROOM1141 (Elza, 4 Records, Sce_em_set @0xB1A/@0xB2E/@0xB54/@0xB68)**; ROOM11C1 (Elza)
  setzt 5 Hunde, keine Gorillas. ROOM1141 hat KEINE Pad-Zelle (u1 0x10/0x20) -> Pad-Suche dort wirkungslos (Original);
  Lache und grosser Record-Schatten wirken dort ebenfalls (an der exe nicht gefahren). Der Sicht-Fix wirkt auf alle
  RE2-Zombies beider Szenarien (Zellen mit Nibble != 3 nehmen keine Sicht mehr).

## 5. Maengel (nummeriert, nachpruefbar)

1. **P1 Innenraum-Teil des Wortlauts nicht erklaert, Abbildungsluecke bei Nibble-3-Moebeln.** Der Wortlaut nennt ausdruecklich
   "auch wenn sie innerhalb eines Raumes sind"; der Bau-Agent hat keinen Innenraumfall gefunden, der Fix wirkt innen laut
   eigenem Vorher-Zensus an genau EINEM Platz (ROOM11E0 Slot 1). Die Abbildung "nur Nibble 3 blockt" laesst niedrige
   Nibble-3-Moebel die Sicht nehmen, entgegen der eigenen RE2-Lesart (niedrige Saetze blocken nicht, @0x80050b34-74).
   Per Sprung belegt: `RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3750,4300,1024,0 RE15_GEGNER_Y_LOG=gy.log
   RE15_EXIT_AT=1200#1010`, neu und v0.8.23 bitgleich: Zombie Slot 2 (205,7248) rot 0 bleibt F1..F1200 in st 1/0/1/0,
   Leon (3669,3962) 4775 entfernt, Kegelwinkel +494, los=0; Sicht nehmen SCA #5 (Verhoertisch x -50..2450 z 1550..5900,
   Nibble 3) und #12 (Kreis 1200 bei (2450,4700), Nibble 3). Im normalen Tuereintritt sieht dieser Zombie Leon zuerst frei
   (Sicht-Bit nur Weck-Bedingung) — die Luecke wirkt also dort, wo ein schlafender Zombie Leon nur ueber ein Nibble-3-Moebel
   sehen koennte; eine Normalfluss-Reproduktion fehlt (vom Bau-Agenten nicht gesucht, hier nicht gefunden).
2. **P1 ROOM10B0 nur halb.** Neu reagieren die vier Terrassen-Zombies (F7), laufen aber nach Osten von Leon weg und stehen
   ab F777..F1441 bis Bildende bei x -6818/-6017/-5210/-4418, z -11832 auf der Stelle (Abstand 10713..13013). Leon wird nie
   erreicht. Als OFFEN ohne DuckStation-Gegenmessung des Originals abgelegt — fuer den Wortlaut "Das darf so nicht sein"
   ist das kein Abschluss.
3. **P1 Zensus/Riegel klassifiziert falsch.** "250 hinter Nibble-3-Zellen (Waende)" stimmt nicht: ROOM1010 Slot 1 Plaetze
   (1350..2150, 6700) stehen hinter Tisch #5 / Kreis #12. Raster <= 4000 + Bodentest r 450 lassen die Tisch-Ostseite fuer
   Slot 2 ganz aus. `unit_r36_zombieki_sicht` pinnt damit die Abbildung, die Mangel 1 verursacht.
4. **P2 Pad-Suche schickt von Leon weg (exe ROOM11C0 Kampf-Layout).** `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240
   RE15_PLAYER_POS=-7000,12500,1024,0 RE15_GEGNER_Y_LOG=gy.log`: Slot 3 (blind, fern) wird zum naechsten 0x10-Pad gesteuert
   (8614,2026) -> (7967,-775), springt F411 nach SUEDEN (z -775 -> -9052, Leon bei z 12500), steht ab F450 in Sub 4 bei
   (4786,-8892) bis Laufende (1568 Bilder fest, nie Angriff); in v0.8.23 kroch derselbe Gorilla die Ostgasse nach Norden und
   griff F1012 an. Gesamtblockade unveraendert (Basis 1634 / neu 1568 Bilder). Die PORT-WAHL waehlt das Pad ohne Bezug zur
   Richtung auf Leon.
5. **P2 Wirkung statistisch schwach, Sub 4 unbehandelt.** 52 Zufallsgang-Seeds (Sonde mit Schalter, gleiche Spielschleife):
   AN 38592 / AUS 47234 blockierte Gorilla-Bilder, 19 Seeds besser, 17 schlechter, 16 gleich; Rest-Blockade zu 93,5 % Sub 4
   (Auswahl ohne Sicht an der Klemme, OFFEN). "noch oft blockiert" bleibt messbar bestehen.
6. **P2 Riegel verdeckt die eigene Regression.** `unit_r36_zombieki_affen_block` (riegel) dokumentiert "Zufallsgang ... keine
   Strecke >= 90 Bilder ohne Bewegung (wie vor der Aenderung)", prueft aber nur `griffe > 0`. Auf genau diesem Seed
   0x1234567: ohne Pad-Suche 0 blockierte Bilder / 9 Griffe, mit 2067 / 3. Dossier-Angabe "Seed 0xbeef vorher 3 / 219" ist
   am Stand HEAD nicht reproduzierbar (ohne Pad-Suche: 3 Griffe, 0 blockiert).
7. **Dossier-Fakt ROOM1030 falsch** ("Debug-Spawn ... kein Spielweg"): Tueren ->1050/1070/1040 liegen auf Leons Seite, main00
   setzt die Zombies bedingungslos. Ohne Verhaltensfolge (geskriptete Tor-Szene, frueher als Original belegt).
8. **Keine gruene Voll-Suite fuer den Endstand** (Angabe des Bau-Agenten); vom Orchestrator nachzufahren.

Hinweis fuer den Nutzer (kein Mangel): der lebende Gorilla hat jetzt einen grossen dunklen Schatten (Record 1700/1800 aus
dem disassemblierten INIT statt 500x600-Standard) — bitte beim Spieltest ansehen.
