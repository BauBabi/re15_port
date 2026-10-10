# Runde 36 — Spur F "zombieki": Abnahme 1

Datum: 2026-10-10. Pruefer: unabhaengige Abnahme 1 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **7a9a0e7f** (Basis master 78a4a748). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md,
Vorgaenger-Abnahme `F_abnahme_0.md` (Stand 853cbfb3, Maengel 1-8).

> **ERGEBNIS: NICHT BESTANDEN.** P1 teilweise, P2 teilweise, P3 erfuellt. Bau, gezielte Regression (162/162), Spur-Riegel (6/6),
> RE-Zitate (8/8 selbst disassembliert) und Vertrag halten. Maengel 1-6 in Abschnitt 8: Innenraum-Taschenfall ROOM1010 weiter in
> der exe (Wortlaut-Lage), NEUE Regression "Zombies sehen durch Waende" (eab5925a, ROOM1040 exe los 895/900 statt 0/900),
> Gorilla-Befundstelle (-24000,10000) weiter blockiert (Endlos-Sprungschleife), Riegel misst Spruenge auf der Stelle nicht.

## 0. Stand, Bau, Messmittel

- `git log master..HEAD`: Abnahme-0-Stand + 2276931a, 376da57a, 4c3e6393, 922a31c8, eab5925a, a0c36009, 7a9a0e7f.
  Arbeitsbaum sauber (git status leer).
- Code-Diff gegen master gelesen: enemy_ai_common.c (+30/-11: Include, 2 Exporte `re15_r36_los_zelle_kreuzt` /
  `re15_r36_los_ray_nib3`, Navigator-Haken `re15_r36_nav_zonenlos`, Gorilla-INIT/Leiche/A[3]/B[3]/B[4]-Haken je 1 Zeile),
  enemy_ai_re2_zombie.c (1 Zeile Sicht-Bit + Include), main.c (1 Zeile Renderer-Haken + Include), neu zombieki_r36.c
  (418 Z.) + re15_zombieki_r36.h (176 Z.), Tests probes/r36_zombieki.cmake + 6 test_r36_zombieki_*.c.

- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> "ninja: no work to do",
  `LOCAL-BUILD-OK (build)`. exe 20:38:20 nach der letzten Quellaenderung (zombieki_r36.c 20:32:10), Baum sauber => exe = HEAD.
- Messkopien neben der exe (eigener Name): `re15_pc_abn1f_neu.exe` (= re15_pc.exe, md5 ce8e2ccf...), `re15_pc_abn0f_neu.exe`
  (Stand Abnahme 0 = 853cbfb3, md5 5b13884a...), `re15_pc_abn0f_basis.exe` (= v0.8.23-Spielcode, md5 2fd81d20..., Hauptbaum-exe
  2026-10-04 18:37 nach dem letzten Engine-Commit 45f03f02 14:12; `git diff --stat 3ec5b1f8 78a4a748 -- engine platform include` leer).
- Eigene Pruefer-Sonden (Scratch, nicht im Baum, gegen die gebauten Bibliotheken gelinkt): `probe_oberkante.c` (welche Nibble-3-Zellen
  bekommen ueber `re15_r36_zelle_oberkante` eine "belegte Oberkante", alle RDTs aller Stages), `probe_durchwand.c` (wo oeffnet die
  NB1-Hoehenregel dem RE2-Zombie die Sicht, die vor NB1 zu war; Raster um jeden Zombie, nur begehbare Plaetze in BLK-Zonen).

## 1. RE-Gate (neue Zitate der Nachbesserung 1, selbst disassembliert, richtige Binaerdatei)

| Zitat im Diff | Datei | gelesen | Urteil |
|---|---|---|---|
| RE2 Zombie +0x1c1 = 8 `addiu v0,zero,8` / `sb v0,449(s2)` @0x8010082c-30 | info/re2leon/COMMON/BIN/EMZ0.BIN | genau so (449 = 0x1c1) | stimmt |
| RE2 FUN_8004A808 Teil-Lage: `lbu v1,449(s0)`, `lw a0,408(s0)`, Kette = v1*0xac, `addiu a0,a0,92`, Spieler 0x800cfd90 + 0x800cfdb9*0xac, `addiu a3,zero,1` @0x8004a86c, `addiu a2,zero,8192` @0x8004a868 | info/re2leon/PSX.EXE | genau so (a2 im Delay-Slot von `bne` @0x8004a864) | stimmt |
| RE2 FUN_8004A808 Direktform `lw v0,0(s3)` / `sh v0,452(s0)` / `lw v1,8(s3)` / `sh v1,454(s0)` @0x8004a90c-924, Zweig `lhu v1,270(s0)` / `andi 0x1000` / `beq` @0x8004a8f8-904 | info/re2leon/PSX.EXE | genau so; s3 = a1 (Zielzeiger, `addu s3,a1,zero` @0x8004a818) | stimmt |
| A[3] Wandklemme `lhu v0,470(v1)` / `bne v0,zero,0x80117c78` @0x80117b18-20 | STAGE1.BIN | genau so (+0x1d6 != 0 -> Return) | stimmt |
| A[4] Wandklemme @0x80118054-5c, Nah-Tor `lhu v0,468(v1)` / `sltiu v0,v0,0x1771` / `bne` @0x80118064-70 | STAGE1.BIN | genau so | stimmt |
| coll_contact_dir `sra 4` / `addiu 8` / `andi v1,v1,0x3` / `sb v0,144(a0)` @0x8003ba70-8c | PSX.EXE | genau so ((Code>>4)+8+(b0&3) -> +0x90) | stimmt |
| AOT-Pool-Maske `lbu v0,1(s0)` / `and v0,v0,a3` / `beq` @0x80042c84-90 (a3 = `sw a1,64(sp)` @0x80042bc0), Gegner `ori a1,zero,0x2` / `jal 0x80042bac` @0x80043720-24 | PSX.EXE | genau so | stimmt |
| RE2 Kriecher-INIT `addiu v0,zero,513` / `sw v0,4(s2)` @0x80100b70-74; DECIDE[2] @0x80103A70: Kegel `addiu a3,zero,512`, `sltiu v0,s1,0x514`, Sicht `andi 0x800` @0x80103ae8-f4, Etage +0x106 | EMZ0.BIN | genau so | stimmt |

- Suche im Code-Diff nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/approx/getenv: **0 Treffer**. Neue
  `*_enable`-Schalter nur fuer Sonden (vom Spiel nie gerufen).
- Konstanten OHNE @0x-Adresse im Diff: `RE15_R36_LEON_KOPF_Y = -2514` ("GEMESSEN" an der Port-exe, Rueckfall ohne head_world),
  Messtabelle `k_r36_moebel` Oberkante -2000 (ROOM1010 Stuhlkreise, "GEMESSEN" per Projektion, als Obergrenze), Abtastschritt 50
  in `r36_strahl_darueber`, Umweg-Parameter (Ring 3/4, 8 Knoten Vorschau, 2 Schritte Anlauf) — letztere unter der
  gekennzeichneten PORT-WAHL "Umweg". -2514/-2000 sind weder @0x noch Byte-Offset noch NUTZER-VORGABE/PORT-WAHL, sondern
  Port-Messwerte (Hinweis, s. Maengel).

## 2. Punkt 1 — "Zombies bleiben ewig beteiligungslos stehen ... fast vor ihrer Nase. Auch wenn sie innerhalb eines Raumes sind."

Alle exe-Laeufe: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_AI_FLAVOR=re2
RE15_GEGNER_Y_LOG=gy.log` + die genannten Haken; Auswertung je Slot mit eigenem Skript `gy_slot.py` (Scratch).

### 2.1 Abnahme-0-Fall ROOM1010 (exe, Leon 800 vor Slot 1 = "fast vor ihrer Nase")
`RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3750,4300,1024,0 RE15_EXIT_AT=1200#1010`
- Stand Abnahme 0 (`re15_pc_abn0f_neu.exe`): Slot 2 (205,7248) F1..F1200 `st1/0/1 los0`, reagiert nie (= Befund Abnahme 0).
- NB1 (`re15_pc_abn1f_neu.exe`): Slot 2 `los1` ab Bild 1, geht los (F60 Sub 1 (1370,6773)), Griff-Ansatz Sub 3 ab F240 (d 537 bei F300), spaeter Sub 6/8 an Leon (F840..F1020). **Mangel 1 der
  Abnahme 0 (Tisch/Stuhl nehmen die Sicht) ist in der exe behoben.**
- **ABER Slot 1 (der Zombie direkt vor Leon) in BEIDEN Staenden:** Griff, Rueckstoss nach (2934,1917), danach
  `st1/1/1 mo0` (Gang-Clip) auf der Stelle: abn0f F360..F1200 bei d 2172 (840 Bilder), NB1 F480..F1020 bei d 2518..2815 (bis Leon
  von Slot 2 getoetet wird, F~1050, Raumwechsel 1240). Das ist der im Dossier als OFFEN gefuehrte **Taschenfall** — er tritt
  also nicht nur im Sondenlauf, sondern in der exe genau in der Wortlaut-Lage "fast vor ihrer Nase, innerhalb eines Raumes" auf.
- Sonde (echte Spielschleife, Leon geht von der Tuer 1020 auf Slot 1 zu): `test_r36_zombieki_eintritt spur 1010 1020 0 1 1 1500 60`
  -> Slot 1 ab F300 bis F1500 bei (2934,1917) `st1/1/1 clip0 los0`, Leon d 1220..1472, steer = Leon. Reproduziert.
- Gegenlauf exe von der Tuer (Leon (3650,6900) rot 1024, `RE15_INPUT_SCRIPT=U0.7`, `_BASIS=spiel`, `_START=5`): Rueckstoss nach
  (3434,2983), Slot 1 kommt zurueck und greift erneut — der Taschenfall haengt am Rueckstosswinkel, ist aber kein Einzelfall
  der Sonde.

### 2.2 ROOM10B0 Terrasse (exe, Abnahme-0-Kommandozeile)
`RE15_DEBUG_JUMP=10B0@250 RE15_PLAYER_POS=-17000,-8500,2048,0 RE15_EXIT_AT=2400#10B0`
- abn0f: die vier Terrassen-Zombies laufen nach Osten weg (F2400 d 10713 / 11477 / 12251 / 13012).
- NB1: alle vier stehen ab ~F300 an der Kante vor Leon (F2400 x -18450 / -17651 / -16850 / -16047, z -11832, d 3319..3633),
  `st1/1/1` (Gang gegen die Kante), los 1. **Dossier-Angabe bestaetigt.** Herunter kommen sie nicht (SCA #9 u0=0xff, Treppen-AOT
  Pool-Maske 1 = nur Spieler, selbst disassembliert @0x80042c84/@0x80043724, Abschnitt 1) — RE2-typisch (Zombies nehmen keine
  Treppen), fuer den Nutzer sichtbar als "Zombies treten an der Kante auf der Stelle".

### 2.3 Spur-Riegel/Zensus (selbst gefahren)
- `unit_r36_zombieki_eintritt` 5/5; Ausgabe **KLEMME 3** (nicht nur der eine im Dossier genannte Fall): 1010<-1020 Slot 1 "hin"
  (Taschenfall, still 300 ab F780 d 1894), 1220<-1210 cut2 Slot 3 "steht" (Griff F133, danach Sub 5, still ab F690 d 3436),
  1220<-1210 cut4 Slot 5 (Griff F708, Sub 5 ab F840 d 3407). Die beiden 1220-Faelle liegen (Sub 5 = RE2-Knockdown) — kein Stehen.
  Der Riegel prueft nur PASSIV == 0, KLEMME wird nur gedruckt (der Taschenfall ist damit nicht gepinnt).
- `unit_r36_zombieki_sicht` 10/10, `_wecker` 2/2, `_szenen` 5/5 (Griff/Abwehr/Schuss: erneuter Angriff Bild 419/366/84/153/282).

### 2.4 NEBENWIRKUNG der NB1-Hoehenregel: Waende werden zu "Moebeln" (eigene Sonden)
`re15_r36_zelle_oberkante` nimmt das tiefste Obj_model_set-Objekt (Weltraum, y < 0), dessen (x,z) im Zell-Rechteck liegt, als
Oberkante der Zelle — ohne zu pruefen, ob die Zelle eine Wand ist und ob das Objekt aktiv/auf dem Band ist.
- `probe_oberkante` (alle RDTs, Stages 1-7): **965 von 17700 Nibble-3-Zellen** bekommen so eine Oberkante, darunter eindeutige
  Waende: ROOM1040/1041 #4/#15 **2052 x 37566** (Oberkante -1900 aus drei Objekten bei x -27038, y -1900, z 4196/8914/15468),
  ROOM1070/1071 22212 x 1944 (-1500), ROOM1110/1111 18200 x 2000 (-1800) und 12400 x 2500 (-1600), ROOM1200/1201 12300 x 2000
  (-1818), ROOM1190/1191 34758 x 2376 (-1340), ROOM2000 2000 x 32600 (-900), ROOM2090 20100 x 2000 / 2000 x 25600 (-1620),
  ROOM2010 14700 x 2000 (-1400), ROOM10F0 1800 x 13600 (-1248). Alle unter den gemessenen Kopfhoehen (~-2500) -> der Kopf-zu-Kopf-
  Strahl geht "darueber".
- `probe_durchwand` (Raster um jeden Zombie-Spawn aller Stages, nur begehbare Plaetze in BLK-Zonen, < 0x1388): NB1 oeffnet 763
  Plaetze, die vor NB1 verdeckt waren; **710 davon (462 im Kegel) liegen hinter der Wand ROOM1040/1041 #4**. Begehbarkeitsraster
  (`test_r36_zombieki_eintritt raster 1040 1 700`): West- und Ostgang liegen beidseits dieser 2-m-Wand und sind nur im Norden
  (z ~ 21000..22700) verbunden.
- exe-Gegenprobe: `RE15_DEBUG_JUMP=1040@250 RE15_PLAYER_POS=-29900,6000,0,0 RE15_EXIT_AT=900#1040` (Leon im Westgang):
  Sicht-Bit der Ostgang-Zombies Slot 1/2: v0.8.23 0/900 und 0/900, Abnahme 0 0/900 und 0/900, **NB1 895/900 und 396/900** — sie
  "sehen" Leon durch die Wand. Wege/Endlagen in allen drei Staenden bitgleich, weil die 1040-Zombies (Deskriptor 0x0D) schon im
  Gang (Sub 1) starten und das Bit dort nicht lesen; wirksam wird der Fehler bei stehenden/wandernden Zombies (Sub 0: DECISION[0]
  @0x80101308-1C, Wecker @0x80101544-7C/@0x80101674-A8) und beim Kriecher (DECIDE[2] @0x80103ae8).
- Bild (exe NB1, `RE15_FORCE_CUT=3/4/5`, Leon im Ostgang (-25500,8000)): Scratch `r1040_cuts345.png` — schmale Betongaenge mit
  massiven Waenden (Cut 3 links eine grosse dunkelgruene Tafel an der Wand); die drei Objekte bei y -1900 sind Wandobjekte
  (rot 3072, 449 Einheiten in der 2-m-Wand), keine Moebel.

## 3. Punkt 3 — "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot sind."

- Code-Stand: `git diff 853cbfb3 HEAD` beruehrt weder main.c noch `re15_r36_affen_lache_{init,tick,render}` (NB1 haengt nur
  `r36_padsuche_reset` in den INIT) — der in Abnahme 0 gemessene und disassemblierte Stand (Record 1700/1800 @0x801171e8/f0,
  Tinte 0x00ffff38 + 8/Takt @0x8011bed0-bf14, 0x5a Takte @0x8011be80-84) gilt unveraendert.
- Riegel `unit_r36_zombieki_affen` 9/9 auf HEAD (echter Schusspfad, Gegenprobe ohne Haken = Befund).
- exe NB1 (eigener Lauf, die Abnahme-0-Kommandozeile toetet den Gorilla mit NB1 nicht mehr, weil er jetzt VOR dem ersten Schuss
  beisst): `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-9013,-11800,1024,0 RE15_GIVE=7:200 RE15_EQUIP=7
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=W0.1,(MA0.1,M0.6)x45 (ausgeschrieben)
  RE15_FORCE_CUT=5 RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=200-400/10:fd_ RE15_EXIT_AT=400#11C0` (beschleunigter Renderer):
  Slot 2 Zustand 3 ab F254, Leiche (Zustand 7) ab F294 bei (-7478,-12875). Rot-dunkle Pixel (G,B <= 24, R >= G+14) im
  Leichenfenster (120..260, 90..160): F290 292, F300 294, F330 340, F360 422, F380 465, **F390 494, F400 494** — monotones
  Wachstum ueber ~91 Bilder ab dem ersten Leichentakt, dann konstant (Scratch `p3e_c5/lache_serie.png`: Lache unter der Leiche
  waechst sichtbar). Cut 1/2/3 zeigen die Stelle nicht (Uebersicht `p3e_uebersicht.png`).
- **Urteil P3: erfuellt.**

## 4. Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir irgendwie verbessern."

### 4.1 Riegel und Zufallsgang-Statistik (selbst gefahren)
- `unit_r36_zombieki_affen_block` 11/11 (16 Seeds je eigener Prozess: AUS 28653 -> AN 721, schlechtester +194, Griffe 73 -> 80;
  drei Befund-Szenen Angriff Bild 140/628/533, Gegenprobe ohne Angriff).
- 52 Seeds x 6000 Bilder, je Lauf eigener Prozess (`eins`, Pruefer-Kopie der Sonde `probe_affen_anker.c`, Spielcode = Bibliothek
  dieses Baums): mit der Blockade-Definition des Bau-Agenten (Anker-Radius 40, Manhattan) **bitgleich zum Dossier**: AUS 75408 ->
  AN 2380, Griffe 226 -> 254, besser 39 / schlechter 3, schlechtester +194.
- **Mess-Luecke:** die Definition "bewegt sich < 40 Einheiten ab Streckenbeginn" zaehlt einen Gorilla, der auf der Stelle springt
  oder hin- und herpendelt (Sprung auf der Stelle versetzt ihn um ~100), NICHT als blockiert. Mit Anker-Radius 400 (sonst gleich):
  **AUS 114029 -> AN 27597 (-76 %)**, besser 45 / schlechter 7, schlechtester Seed 0x5979fb85 +509; Sub-7-Bilder (Sprung) in den
  blockierten Strecken AN 4392. Die Verbesserung ist echt, die Rest-Blockade aber rund 11-mal groesser als im Dossier (2380).

### 4.2 exe Kampf-Layout, Leon steht an sechs Stellen (v0.8.23 gegen NB1)
`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_GEGNER_Y_LOG=gy.log RE15_EXIT_AT=2400#11C0`
(keine Eingabe, Leon kann sterben -> Lauf endet mit dem Raumwechsel). Auswertung `affen_ausw2.py`: erster Angriff = Sub 5/6/8/15;
FESTGEFAHREN = aktiv, kein Angriff, >= 90 Bilder innerhalb 400 Einheiten um den Anker, Leon > 3000 entfernt (zaehlt auch Sprung
auf der Stelle).

| Leon | v0.8.23 Slot 2 / Slot 3 | NB1 Slot 2 / Slot 3 |
|---|---|---|
| (-7000,12500) | nie (fest 1882) / F1012 | **F896 / F606**, fest 0 / 0 |
| (-14000,2000) | F680 / nie (fest 930 bei (6418,6586)) | **F419** / nie (fest 0, kriecht nach (-13522,12254)) |
| (6000,-9000) | nie (fest 1272) / F324 (fest 1574) | **F1134 / F339**, fest 0 / 0 |
| (-24000,10000) | nie (fest 1518, Sub 3 bei (-16181,-4921)) / nie (Kriechen an x 6418, fest 1376) | **nie: 1519 Bilder Sprungschleife** (Sub 7 +0x7=3, 39 Spruenge F879..F2400 in x -13518..-13418, z -263..-58) / nie (bitgleich v0.8.23) |
| (7000,4000) | nie (fest 1011) / F18 | **F1123** / F18 |
| (9000,10000) | nie (fest 1102) / F309 | **F907** / F309 |

(Laeufe, die das Leon-Sterben beendet, enden mit Raumwechsel 1240; Rohdaten Scratch `affen_*`/`affen2_*`/`affen_rerun_*`. Zwei
Verzeichnisse waren durch einen nachlaufenden Messprozess doppelt beschrieben und wurden sauber nachgefahren.)

- **5 von 6 Stellen deutlich besser** (der vorher nie angreifende Gorilla greift an; Dossier-Angabe "Slot 2 F896" bestaetigt).
- **(-24000,10000) — eine der vier eigenen Befund-Stellen des Bau-Agenten ("Messung vorher": (-7000,12500), (7000,4000),
  (-24000,10000), (9000,10000)) — ist NICHT behoben:** in der exe greift keiner der beiden Gorillas in 2400 Bildern an; Slot 2 haengt
  statt im Stand jetzt in einer **Endlos-Sprungschleife auf der Stelle** (Original-Blindsprung A[4] `ori v0,zero,0x7` /
  `sb v0,5` / `ori v0,zero,0x3` / `sb v0,7` @0x80117fe4-80118024, selbst gelesen) bei (-13518,-58), in die ihn zwei Umweg-Pad-
  spruenge (F799 Sub 3 -> 7, F839 Sub 4 -> 7) gebracht haben. Dieselbe Lage in der eigenen Sonde der Spur:
  `test_r36_zombieki_affen_block fest -24000 10000 2400` -> Gorilla 1: 53 Spruenge (48 blind), Sub 7 2032/2400 Bilder, Abstand
  16125, kein Angriff; Gorilla 2: Sub 4 1850/2400 Bilder MIT Sicht an der Klemme bei (-321,12280), Abstand 23681, kein Angriff.
  Die drei Riegel-Szenen pinnen nur einen Gorilla (der zweite wird nach (20000,-20000) geparkt) und diese Stelle nicht.
- Dieselbe Sonde an den uebrigen fuenf Stellen (beide Gorillas, 2400 Bilder): erster Angriff Bild 185..705 — bestaetigt die exe.

### 4.3 Urteil Punkt 2: **teilweise**
Verbessert ist es messbar (exe 5/6 Stellen, Zufallsgang -76 % mit strenger Definition). Der Wortlaut "noch oft blockiert" bleibt an
einer der eigenen Befund-Stellen reproduzierbar bestehen (beide Gorillas, ganze Laufzeit, einer davon in einer sichtbaren
Sprungschleife), und der Riegel misst genau diese Form nicht (Sprung auf der Stelle zaehlt nicht als Blockade; Rest-Blockade
dadurch 2380 statt 27597 ausgewiesen).

## 5. Gates

- **Bau:** OK (configure + build via local_build.sh, Abschnitt 0).
- **Spur-Tests** `ctest -R r36_zombieki` (im gezielten Lauf enthalten): 6/6 gruen — `_affen_block` 11/11, `_sicht` 10/10,
  `_eintritt` 5/5, `_wecker` 2/2, `_szenen` 5/5, `_affen` 9/9 (Ausgaben direkt nachgefahren).
- **Gezielte Regression** (Module/Raeume des Diffs: enemy_ai_common.c = alle Gegner, enemy_ai_re2_zombie.c, main.c Schatten;
  `ctest -R "affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|1140|1210|1100|10d0|10b0|r35_raeume|
  r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|r34n_f|r36_zombieki|nav|unit_enemy_|room1070_wake|
  ai_flavor|re2_room1090|ada_eintritt|r17_waffen_loop|schatten|shadow" --timeout 600 -j 6 --output-on-failure`):
  **100% tests passed, 0 tests failed out of 162** (507,7 s; darin integration_r35_cut1150 507,6 s, integration_r35_zgirl /
  _5090, unit_re2_crow_shadow_release, unit_r35_raeume_hundeschatten, unit_enemy_ai/_shootable/_body_push/_attack_reach).
- **Volle Suite:** im Spurbaum nicht gefahren (Auftrag); Sache des Orchestrators im Integrationsbaum.
- **RE-Gate:** 8 neue Zitate selbst disassembliert, alle stimmen (Abschnitt 1); Adressen auch in den Commit-Messages (eab5925a,
  376da57a, 4c3e6393, 7a9a0e7f gelesen). Keine Guess-Tells, keine Env-Schalter als Abschluss. PORT-WAHL gekennzeichnet (Umweg,
  zonenloses Ziel). Nicht belegt bzw. nur Port-Messwert: -2514 (Leons Kopf, Rueckfall), -2000 (Stuhlkreise), und die
  ABBILDUNG "tiefstes Obj_model_set-Objekt im Zell-Rechteck = Oberkante der Zelle" (ohne Original-Gegenstueck; RE2 liest die
  Hoehe aus dem Satz selbst @0x80050b34-74) — sie macht Waende mit Wandobjekten zu Moebeln (Abschnitt 2.4).
- **Fix erklaert den Befund?** ROOM1010 Slot 2 (Abnahme 0): ja, gemessen (Tisch/Stuhl unter dem Kopfstrahl). ROOM10B0: ja
  (Steuerziel (0,0) -> Spieler, Endabstand 10713..13012 -> 3319..3633). Der in der exe weiter auftretende Innenraum-Stillstand
  (Taschenfall ROOM1010 Slot 1) ist NICHT erklaert/behoben, sondern als OFFEN gefuehrt.
- **Vertrag:** Pfade OK (`git diff master --name-only`: analysis/befunde_runde36/F_*, engine/src/{enemy_ai_common.c +30/-11,
  enemy_ai_re2_zombie.c +2/-1, zombieki_r36.c}, include/re15_zombieki_r36.h, platform/pc/main.c +4, tests/unit/probes/
  r36_zombieki.cmake, tests/unit/test_r36_zombieki_*.c; kein release/, platform/android/, shared_assets/PSX/, keine Test-
  CMakeLists). enemy_ai_common.c-Haken je 1 Zeile + zwei Exporte (5 bzw. 6 Zeilen) im Sichtfilter-Bereich der Spur F
  (VERTRAG 1.4: F ist Eigentuemer der enemy_ai_common.c-Sichtfilter-Haken). Keine Bank-9-Bits, Nachrichten-IDs, Ereignisse.
  Tests ueber die eigene probes-Datei registriert.

## 6. Erweiterter Eintritts-Zensus (Pruefer, alle Stages + Elza-Raeume)

Die drei Zensus-Sonden der Spur decken nur Leons Stage-1-Raeume ab (`raum = 0x1000 | r<<4`, Ziel-Stage 1). Pruefer-Kopie der
Sonde `test_r36_zombieki_eintritt` (`probe_eintritt_alle.c`: Stages 1-6, Varianten xxx0/xxx1, Eintrittsgrenze 3000) gegen die
Bibliothek dieses Baums: 410 Eintritte, **638 Faelle, PASSIV 1, KLEMME 6**.
- PASSIV ROOM1091<-10F1 Slot 3 und KLEMME 1091 Slot 2: Elza steht auf y -9000 (Band 5), die Zombies auf y -1800 (Band 1) —
  andere Ebene (Sonde zaehlt 2D-Abstand), **kein Defekt**.
- KLEMME 1220/1221 (je 2): nach dem Griff Sub 5 (RE2-Knockdown, Liegezeit) — kein Stehen.
- KLEMME 1010<-1020 Slot 1: der Taschenfall (Abschnitt 2.1).
=> Ausser dem Taschenfall kein weiterer Stillstand-Fall in Stage 2-6 oder in Elzas Raeumen.

## 7. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Zombies bleiben ewig beteiligungslos stehen, fast vor der Nase, auch innerhalb eines Raumes | **teilweise** | Behoben: ROOM1010 Slot 2 (exe: Sicht ab Bild 1, Griff-Ansatz Sub 3 ab F240; vorher nie), ROOM10B0 (exe: Ende an der Kante vor Leon d 3319..3633 statt 10713..13012 im Osten), Zensus nach Lage ohne passiven stehenden Zombie (auch alle Stages/Elza, Abschnitt 6). **Nicht behoben:** der Innenraum-Taschenfall ROOM1010 Slot 1 — exe in der Wortlaut-Lage (Leon 800 vor dem Zombie): nach dem ersten Griff Gang auf der Stelle bei (2934,1917), 840 bzw. >= 540 Bilder, 2172..2815 vor Leon (abn0f und NB1 gleich); vom Bau-Agenten als OFFEN gefuehrt. **Neu eingefuehrt:** Waende mit Wandobjekten werden zu Moebeln — RE2-Zombies sehen Leon durch die 37,5-m-Wand ROOM1040 (exe los 895/900 statt 0/900). |
| 2 Monkeys ROOM11C0 oft blockiert — verbessern | **teilweise** | exe 5/6 Stellen besser, Zufallsgang -76 % (strenge Definition) bzw. -97 % (Definition der Spur). Eigene Befund-Stelle (-24000,10000) weiter blockiert (exe + Sonde der Spur: beide Gorillas 2400 Bilder ohne Angriff, Gorilla 1 in Endlos-Sprungschleife 53 Spruenge, Gorilla 2 1850 Bilder Sub 4). Riegel misst Spruenge auf der Stelle nicht. |
| 3 Blut der toten Monkeys laeuft nicht aus | **erfuellt** | exe NB1 Cut 5: Lache waechst ab dem ersten Leichenbild F294 ueber ~91 Bilder (292 -> 494 rot-dunkle Pixel), dann konstant; Code seit Abnahme 0 unveraendert, Riegel 9/9. |

## 8. Maengel (nummeriert, nachpruefbar)

1. **P1 Taschenfall ROOM1010 Slot 1 nicht behoben (Wortlaut-Fall).** exe: `RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3750,4300,1024,0
   RE15_AI_FLAVOR=re2 RE15_GEGNER_Y_LOG=gy.log RE15_EXIT_AT=1200#1010` -> Slot 1 greift, Rueckstoss nach (2934,1917), danach
   `st1/1/1 mo0` auf der Stelle (abn0f F360..F1200, NB1 F480..F1020), Leon 2172..2815 entfernt. Sonde:
   `test_r36_zombieki_eintritt spur 1010 1020 0 1 1 1500 60` -> F300..F1500 dieselbe Lage. Der Riegel `unit_r36_zombieki_eintritt`
   druckt KLEMME nur und pinnt den Fall nicht. Abnahmekriterium: in beiden Laeufen erreicht Slot 1 Leon nach dem Rueckstoss
   wieder (kein >= 300-Bilder-Stillstand mit Leon < 5000), Riegel prueft KLEMME == 0 fuer diesen Fall.
2. **Regression durch eab5925a (NB1 Maengel 1/3): Waende werden zu "Moebeln", RE2-Zombies sehen durch Waende.**
   `re15_r36_zelle_oberkante` nimmt jedes Obj_model_set-Objekt im Zell-Rechteck als Oberkante — 965 von 17700 Nibble-3-Zellen,
   darunter ROOM1040/1041 #4 (2052 x 37566, -1900), ROOM1070 (22212 x 1944, -1500), ROOM1110 (18200 x 2000, -1800), ROOM1200
   (12300 x 2000, -1818), ROOM2000 (2000 x 32600, -900), ROOM2090 (20100 x 2000 / 2000 x 25600, -1620) (Pruefer-Sonde
   `probe_oberkante`). Begehbare Leon-Plaetze, an denen NB1 die Sicht neu oeffnet: 763, davon 710 hinter der Wand ROOM1040/1041 #4
   (`probe_durchwand`). exe: `RE15_DEBUG_JUMP=1040@250 RE15_PLAYER_POS=-29900,6000,0,0 RE15_EXIT_AT=900#1040` -> Sicht-Bit
   Slot 1/2 (Ostgang) NB1 895/900 und 396/900, v0.8.23 und Abnahme 0 je 0/900. Ohne Wegfolge nur, weil die 1040-Zombies schon im
   Gang starten; DECISION[0]/Wecker/DECIDE[2] lesen das Bit bei stehenden/wandernden/lauernden Zombies. Kein Riegel deckt das ab.
   Abnahmekriterium: die Oberkante gilt nur fuer Moebel-Zellen (Beleg je Zelle), `probe_durchwand`-Gegenstueck im Riegel: 0 Plaetze
   hinter Waenden geoeffnet, ROOM1040-Westgang los 0/900.
3. **P2 Befund-Stelle (-24000,10000) weiter blockiert.** exe (Kommandozeile Abschnitt 4.2) und `test_r36_zombieki_affen_block fest
   -24000 10000 2400`: kein Angriff beider Gorillas; Gorilla 1 Endlos-Blindsprung (A[4] +0x7=3 @0x80117fe4-80118024) bei
   (-13518,-316)/(-13518,-58), in den ihn die Umweg-Padspruenge fuehren; Gorilla 2 Sub 4 mit Sicht an der Klemme (-321,12280).
4. **P2 Riegel misst die Nutzer-Form "blockiert" nicht vollstaendig.** Blockade-Definition "< 40 Einheiten ab Streckenbeginn"
   zaehlt Spruenge/Pendeln auf der Stelle nicht; mit Anker 400: AUS 114029 -> AN 27597 (Dossier: 2380), schlechter 7 statt 3
   Seeds, schlechtester +509 (0x5979fb85). Die Stelle (-24000,10000) ist nicht gepinnt; die drei Szenen parken den zweiten Gorilla.
5. **RE-Gate, Belegform:** `RE15_R36_LEON_KOPF_Y = -2514` und die Messtabelle -2000 (Stuhlkreise ROOM1010) sind Port-Messwerte
   ohne @0x/Datei-Byte-Offset und ohne PORT-WAHL-Kennzeichnung (Herleitung aus PLD-/Kamera-Daten fehlt im Code).
6. **Voll-Suite** fuer den Endstand: vom Orchestrator im Integrationsbaum zu fahren (gezielte 162 Tests hier gruen).

Hinweise (kein Mangel): (a) Dossier "einziger Fall im Eintritts-Zensus" — die Sonde meldet KLEMME 3 (zwei 1220-Faelle = Sub 5
Liegezeit, kein Stehen). (b) Die drei Zensus-Sonden der Spur decken nur Leons Stage 1; die Pruefer-Erweiterung (Abschnitt 6) fand
dort keinen weiteren Fall. (c) Die Lage-Sonde `probe_cut` (Kamera-Automatik ohne Renderer) lieferte immer Cut 0 — fuer P3 wurde
der Cut per `RE15_FORCE_CUT` gesucht (Cut 5).
