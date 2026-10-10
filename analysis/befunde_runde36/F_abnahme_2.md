# Runde 36 — Spur F "zombieki": Abnahme 2

Datum: 2026-10-10/11. Pruefer: unabhaengige Abnahme 2 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **4285214f** (Basis master 78a4a748). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md,
Vorgaenger-Abnahme `F_abnahme_1.md` (Stand 7a9a0e7f, Maengel 1-6).

> **ERGEBNIS: NICHT BESTANDEN.** P1 erfuellt, P2 teilweise, P3 erfuellt. Bau, gezielte Regression (196/196), Spur-Riegel (6/6),
> RE-Zitate (9/9 selbst gelesen) und Vertrag halten. Maengel 1-3 in Abschnitt 7: der NB2-Umweg leitet den suedlichen Gorilla bei
> Leon im Nordwest-Gang ueber das Ost-Pad um (erster Angriff 850..920 Bilder spaeter als v0.8.23), und die Nordwest-Tasche ist
> nur am gepinnten Punkt behoben (drei weitere Plaetze: kein Angriff, der Umweg steuert einen Gorilla in die Wand).

## 0. Stand, Bau, Messmittel

- `git log master..HEAD`: Abnahme-1-Stand + 3cd499b7, 517a7dca, a76cd217, 4285214f. Arbeitsbaum sauber (git status leer).
- Code-Diff gegen master gelesen (engine/src/enemy_ai_common.c, enemy_ai_re2_zombie.c, zombieki_r36.c,
  include/re15_zombieki_r36.h, platform/pc/main.c). Seit Abnahme 1 (7a9a0e7f..HEAD, Code): enemy_ai_common.c +1 Zeile
  (Haken `re15_r36_wurf_klemme(e, sweep_ox, sweep_oz)` direkt nach `re15_enemy_sca_clamp` im Zombie-Zweig von
  `re15_enemy_ai_run_all`), zombieki_r36.c (Beleg-Tabelle `k_r36_moebel`, `re15_r36_wurf_klemme`, `r36_uw_route` mit
  Komponenten-Breitensuche + "naechster erreichbarer Knoten", `r36_pad_belegt`), Header-Doku. main.c / Lache unveraendert.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> "ninja: no work to do",
  `LOCAL-BUILD-OK (build)`. exe 22:36:49 nach der letzten Quellaenderung (Header 22:36:33), Baum sauber => exe = HEAD.
- Messkopien neben der exe (eigener Name): `re15_pc_abn2f_neu.exe` (= re15_pc.exe HEAD, md5 4bd3a634...),
  `re15_pc_abn1f_neu.exe` (Stand Abnahme 1 = 7a9a0e7f, md5 ce8e2ccf...), `re15_pc_abn0f_basis.exe` (v0.8.23-Spielcode,
  md5 2fd81d20...). Alle exe-Laeufe: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
  RE15_GEGNER_Y_LOG=gy.log` + die genannten Haken; Rohdaten im Pruefer-Scratch `F_abn2/<lauf>_<exe>/gy.log`,
  Auswertung `gy_slot.py` / `affen_ausw2.py` / `tl.py` (Scratch).
- Pruefer-Sonde (Scratch, gegen die Bibliotheken dieses Baums gelinkt): `probe_eintritt_alle2.c` = Kopie des NB2-Riegels
  `test_r36_zombieki_eintritt.c` mit 4 geaenderten Zeilen (Stages 1-6, Varianten xxx0/xxx1, Ziele Stage 1-6; sonst
  identisch, `diff --strip-trailing-cr` geprueft).

## 1. RE-Gate (neue Zitate der Nachbesserung 2, selbst gelesen, richtige Binaerdatei)

| Zitat im Diff / Commit | Datei | gelesen | Urteil |
|---|---|---|---|
| RE2 EXEC[3] P8: `jal 0x80015e7c`, `jal 0x8002959c` (a3 = `addiu a3,zero,512`), `lhu a2,324(s1)` @0x80102c98, `addiu a2,a2,-30` @0x80102ca0, `jal 0x800152c8` @0x80102ca8, `sh a2,324(s1)` @0x80102cac | info/re2leon/COMMON/BIN/EMZ0.BIN (`re2_disasm.py dis 0x80102C60 --bin EMZ0.BIN`) | genau so | stimmt |
| RE2 Zombie-Wurzel ruft die Klemme einmal je Bild: `jal 0x8003567c` @0x80100638 (a1 = 1024); FUN_8003567c `jal 0x8004c1bc` @0x800356d0 | EMZ0.BIN / info/re2leon/PSX.EXE | genau so | stimmt |
| RE1.5 Rueckfall auf die Vorlage `lh v0,0(s4)` / `sw v0,0(t2)` / `lh v1,4(s4)` / `addu v0,zero,zero` / `j 0x8003cfdc` / `sw v1,8(t2)` @0x8003ced4-ceec in LAB_8003cb9c (t2 = a1 Lage, s4 = a2 Vorlage @0x8003cba4/@0x8003cc34; Sprung dorthin bei Kontaktcode 0 @0x8003ce28) | info/Re1.5/PSX.EXE + ghidra1_V2.txt Z. 145139-145361 | genau so | stimmt (FORM-Zitat; die Wurf-Klemme selbst ist PORT-WAHL und so gekennzeichnet) |
| A[4] Path B: `lhu v0,470(v1)` / `bne v0,zero` @0x80118054-5c (+0x1d6 == 0), Nah-Tor `sltiu 0x1771` @0x8011806c, `+0x5 = 7` @0x801180d8, `+0x6 = 0` @0x801180e8, `+0x7 = 0` @0x801180f8 | info/Re1.5/PSX/BIN/STAGE1.BIN | genau so | stimmt |
| Gorilla-Radius u16 @0x80121356 = 1600 | STAGE1.BIN (`00 00 60 fa 00 00 40 06` ab 0x80121350) | 0x0640 | stimmt |
| Leons Halsteil 8: `ori v0,zero,0x8` / `sb v0,-13299(at)` (= 0x800acc0d) @0x80031930-38 | PSX.EXE | genau so | stimmt |
| Beleg-Tabelle ROOM1010: Obj_model_set (Op 0x2d) @0x00930 / @0x00952, y-Bytes @0x0093c / @0x0095e = `c0 f9` (-1600), Lage (200,-1600,5500) / (1800,-1600,5750); SCA #5 @0x004f8 = 2500 x 4350 ab (-50,1550), w5 0x0300; Kreise #12/#13/#14 @0x0054c/558/564 = 1200 x 1200 ab (1850,4100)/(1850,2200)/(-700,2800) | shared_assets/PSX/STAGE1/ROOM1010.RDT | Bytes genau so; beide Objekte im Rechteck #5 | stimmt |
| Beleg-Tabelle ROOM1220: Obj_model_set @0x00e72/@0x00e94/@0x00eb6, y-Bytes @0x00e7e/@0x00ea0/@0x00ec2 = `18 fc` (-1000), Lagen (-26128,-9748)/(-12693,-22725)/(-26193,-17425); SCA #18/#34/#21 @0x00620/@0x006e0/@0x00644 = (-27600,-11700) 1950x3650 / (-13475,-23600) 2125x3800 / (-27600,-19750) 1950x4150 | ROOM1220.RDT | Bytes genau so; jedes Objekt in seinem Rechteck | stimmt |
| Kamera-Record Cut 0 @ROOM1010.RDT 0x60 (Beleg-Ort der PORT-WAHL -2000) | ROOM1010.RDT Header-Offset[7] = 0x60 | ja | stimmt |

- Suche im Code-Diff (`git diff master -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/
  TODO/FIXME/approx/getenv/schaetz/ungefaehr: **0 Treffer**. Schalter `*_enable` nur fuer Sonden (vom Spiel nie gerufen).
- Konstanten: -2514 ist entfernt (Kopf nur aus head_world); -2000 ist als PORT-WAHL (Messwert) mit Herleitung im Code
  gekennzeichnet; -1600 / -1000 tragen Datei-Byte-Offsets (selbst gelesen). Ohne @0x und ohne eigene Kennzeichnung bleiben
  Implementierungsparameter der PORT-WAHL "Umweg" (Ring 3/4, 8 Knoten Vorschau, Anlauf 2 Schritte, `kt[64]`) und der
  Abtastschritt 50 in `r36_strahl_darueber` (Hinweis, wie Abnahme 1).

## 2. Punkt 1 — "Zombies bleiben ewig beteiligungslos stehen ... fast vor ihrer Nase. Auch wenn sie innerhalb eines Raumes sind."

### 2.1 Abnahme-1-Fall ROOM1010 (exe, Leon 800 vor Slot 1)
`RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3750,4300,1024,0` (Leon stirbt in beiden Staenden; Auswertung bis
zum Raumwechsel, `gy_slot.py ... 1010 1,2 30`):
- NB1 (`re15_pc_abn1f_neu.exe`): Slot 1 Griff, Wurf nach (2934,1917) (F120), `st1/5` liegt, ab F450 `st1/1/1 mo0` **auf der
  Stelle bis F1050** (d 2518..3053, los 0) = Befund Abnahme 1, reproduziert.
- **NB2 (`re15_pc_abn2f_neu.exe`): Wurf endet bei (3433,2983) (F120, d 1007, los 1), F450 Gang, F480 erneuter Griff (Sub 3),
  F540 Sub 6 (frisst Leon), F660 Sub 8.** Slot 2: Reaktion F30, Griff-Ansatz F240-F300 (wie NB1). Dossier-Angabe bestaetigt.

### 2.2 Regression Abnahme 1 Mangel 2 — ROOM1040 Westgang (exe)
`RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1040@250 RE15_PLAYER_POS=-29900,6000,0,0 RE15_EXIT_AT=900`, Sicht-Bit je Slot ueber 900 Bilder:
- NB1: Ostgang Slot 1 **895/900**, Slot 2 **396/900** (durch die Wand); Westgang Slot 3/4/5 895/895/895.
- **NB2: Ostgang Slot 1 0/900, Slot 2 0/900**; Westgang 895/894/895 (sehen Leon zu Recht). Regression behoben.

### 2.3 ROOM10B0 Terrasse (exe, NB1-Verhalten erhalten)
`RE15_DEBUG_JUMP=10B0@250 RE15_PLAYER_POS=-17000,-8500,2048,0 RE15_EXIT_AT=2400`: alle vier Terrassen-Zombies ab F300 an der
Kante vor Leon (F2400 x -18450 / -17651 / -16850 / -16047, z -11832, d 3319..3633, los 1, `st1/1/1`) — wie Abnahme 1
(herunter kommen sie per Daten nicht: SCA #9 Band 1 u0=0xff, Treppen-AOT Pool-Maske 1, Abnahme 1 Abschnitt 2.2).

### 2.4 Eintritts-Zensus alle Stages + Elza (Pruefer-Sonde `probe_eintritt_alle2`, NB2-Definition)
410 Eintritte, **638 Faelle: PASSIV 1, KLEMME 1** — beide ROOM1091<-10F1 (Elza): Elza auf y -9000 (Band 5), die Zombies
auf y -1800 (Band 1) (Spur-Sonde `probe_eintritt_y spur 1091 10F1 0 2 1 900 90`: `y-1800/b1 Ly-9000/b5` in jedem Bild) —
andere Ebene, 2D-Abstand der Sonde, kein Defekt (wie Abnahme 1). Taschenfall: Gegenprobe 450 Bilder auf der Stelle bei
(2934,1916), mit Wurf-Klemme 0, kleinster Abstand ab Griff+300 = 85. Kriecher Bild 2, ROOM10B0 3643. Abnahme 1 hatte mit
derselben Sonde (NB1) KLEMME 6 (davon 4 = Sub 5 Liegezeit, 1 = Taschenfall, 1 = 1091).

### 2.5 Echter Weg: Tuer ROOM1020 -> ROOM1010 (exe)
`RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1020@250 RE15_PLAYER_POS=-2950,-17700,1024,0 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=U0.5,W0.3,A0.1,W8,U0.7,W60 RE15_EXIT_AT=2600` (Tuer-AOT 1020 (-2950,-18450) ->
1010 (3650,6900) laut `tools/engine_tueren.txt`): Raumfolge 1020 (44 Bilder) -> **1010 per Tuer** (765 Bilder) -> Tod -> 1240.
In 1010 reagieren beide Zombies sofort (F30 `st1/1/1 los1`), Slot 2 Griff F90-F150, Slot 1 Griff F210-F300, danach erneut
Gang/Griff bis Leons Tod (F750 Slot 2 Sub 8, Slot 1 d 431). NB1 und NB2 bitgleich (kein Taschenwurf auf diesem Weg).

### 2.6 Urteil Punkt 1: **erfuellt**
Die Abnahme-0/1-Faelle sind in der exe behoben (ROOM1010 Slot 2 Sicht ueber den Tisch, Taschenfall Slot 1 -> erneuter Griff
F480, ROOM10B0 Ziel = Leon), die NB1-Regression "Sicht durch Waende" ist weg (ROOM1040 Ostgang 0/900), der echte Tuerweg zeigt
sofortige Reaktion, und der Lage-Zensus ueber alle Stages und beide Szenarien findet ausser der Ebenen-Abweichung ROOM1091
keinen passiven oder klemmenden Zombie. Rest (Hinweis, kein Mangel der Spur): die vier Terrassen-Zombies ROOM10B0 treten an der
Kante 3,3..3,6 m vor Leon auf der Stelle (herunter per Daten unmoeglich, Abnahme 1 Abschnitt 2.2).

## 3. Punkt 3 — "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot sind."

- Code: `git diff 7a9a0e7f HEAD` aendert weder main.c noch `re15_r36_affen_lache_{init,tick,render}` — der in Abnahme 0/1
  disassemblierte Stand (Record 1700/1800 @0x801171e8/f0, Tinte 0x00ffff38 + 8/Takt @0x8011bed0-bf14, 0x5a Takte @0x8011be80-84)
  gilt unveraendert. Riegel `unit_r36_zombieki_affen` 9/9 (Ausgabe nachgefahren: VORHER pool -1 / 0 Takte, NACHHER pool ab
  Leichenbild 1, 91 Takte, Ende 2428/2528).
- exe NB2 (`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-9013,-11800,1024,0 RE15_GIVE=7:200 RE15_EQUIP=7
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=W0.1,(MA0.1,M0.6)x45 ausgeschrieben
  RE15_FORCE_CUT=5 RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=200-700/10:fd_ RE15_EXIT_AT=700`, beschleunigter Renderer): Slot 2 Zustand 3
  ab F254, **Leiche (Zustand 7) ab F294** bei (-7478,-12875). Rot-dunkle Pixel (G,B <= 24, R >= G+14) im Leichenfenster
  (120..260, 90..160): F290 292, F300 294, F330 340, F360 422, F380 465, **F390 494**, F400..F480 494 — monotones Wachstum ueber
  ~91 Bilder ab dem ersten Leichentakt, dann konstant (bitgleich zu Abnahme 1); ab F490 kommt Slot 3 ins Bild. Bildstreifen
  F290/F330/F360/F400 (Scratch `F_abn2/p3_lache_serie_nb2.png`): dunkelrote Lache unter der Leiche, waechst sichtbar.
- **Urteil P3: erfuellt.**

## 4. Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir irgendwie verbessern."

### 4.1 Riegel und Zufallsgang (selbst gefahren)
- `unit_r36_zombieki_affen_block riegel` **17/17** (Ausgabe nachgefahren): 16 Seeds AUS 38439 -> AN 4568 (Anker 400),
  schlechtester -100, Griffe 73 -> 76; Szenen 1-4 mit BEIDEN Gorillas, je Gegenprobe kein Angriff, Umweg Angriff
  140/425, 898/1275, 929/218, **Szene 4 = Befundstelle (-24000,10000) 597/929**, keine Blind-Schleife.
- 52 Seeds x 6000 Bilder, je Lauf eigener Prozess (`test_r36_zombieki_affen_block eins <i> <0|1> 6000 <anker>`, Pruefer-Lauf,
  `F_abn2/seeds52_anker{400,40}.txt`): **Anker 400: AUS 114029 -> AN 15857** (besser 48, schlechter 3: 0x179803a8 +825,
  0x5979fb85 +474, 0xdb291046 +31), Griffe 226 -> 217; Rest AN: nah 13794, mit Sicht 11729, Sub 3 13953 / Sub 4 1229 / Sub 7 675.
  **Anker 40: 75408 -> 1329** (schlechter 2). Bitgleich zum Dossier. (Mess-Luecke der Abnahme 1 — Spruenge auf der Stelle —
  ist mit Anker 400 geschlossen.)

### 4.2 exe Kampf-Layout, Leon steht an zehn Stellen (v0.8.23 gegen NB2)
`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700` (keine Eingabe; Leon kann sterben ->
Raumwechsel 1240). Auswertung `affen_ausw2.py` wie Abnahme 1 (erster Angriff = Sub 5/6/8/15; FESTGEFAHREN = aktiv, kein Angriff,
>= 90 Bilder im Anker 400, Leon > 3000). v0.8.23-Werte der ersten sechs Stellen aus Abnahme 1 (`abn1F_zk`), die uebrigen neu.

| Leon | v0.8.23 Slot 2 / Slot 3 | NB1 Slot 2 / Slot 3 | **NB2 Slot 2 / Slot 3** |
|---|---|---|---|
| (-7000,12500) | nie (fest 1882) / F1012 | F896 / F606 | F1514 / F606, fest 0 / 0 |
| (-14000,2000) | **F680** / nie (fest 930) | F419 / nie | **F2438** (fest 246) / F1600 (fest 99) |
| (6000,-9000) | nie (fest 1272) / F324 (fest 1574) | F1134 / F339 | F1246 (fest 102) / F339 (fest 309) |
| (-24000,10000) | nie (fest 1518) / nie (fest 1376) | nie (Sprungschleife 1519) / nie | **F1913 (fest 719, laengste 576) / F851** |
| (7000,4000) | nie (fest 1011) / F18 | F1123 / F18 | nie bis Leons Tod F1407 (fest 98, dmin 5222) / F18 |
| (9000,10000) | nie (fest 1102) / F309 | F907 / F309 | F1045 / F309 |
| (-14746,-4140) West-Pad | F364 (fest 1834) / nie | — | F364 (fest 344) / F1186 |
| (7200,-4130) Ost-Pad | F1246 / F274 (fest 670) | — | F981 / F274 (fest 396) |
| (-14000,4000) NW-Gang | **F704** / nie (fest 1088) | — | **F1939** (fest 242) / F1577 |
| (-14500,6000) NW-Gang | **F694** (fest 171) / F1256 (fest 298) | — | **F1806** (fest 107) / F1534 |

- Summe FESTGEFAHREN ueber alle zehn Stellen (Scratch `F_abn2/v0823_zehn.txt` / `nb2_zehn.txt`): v0.8.23 **14726** Bilder -> NB2 **2662** (-82 %). Befundstelle (-24000,10000):
  beide greifen an (Dossier-Angabe F850/F1912 bestaetigt), 0 Blind-Spruenge.
- **Rest-Blockade (OFFEN des Bau-Agenten, bestaetigt):** an der Befundstelle wartet Slot 2 nach den ersten Angriffen F2160-F2700
  am Taschenrand (-23302,13088), Sub 3, 3900..4500 vor Leon, der in die Tasche gestossen wurde; an West-/Ost-Pad 344 / 396 Bilder
  fest (Nahjagd an der Klemme).
- **NEU: Umweg-Umleitung ueber das Ost-Pad.** Steht Leon im Nordwest-Gang noerdlich des West-Pads #47 ((-14000,2000) / (-14000,4000) /
  (-14500,6000)), kommt der suedliche Gorilla (Slot 2, Start (-9013,-15461)) in v0.8.23 und NB1 direkt ueber das West-Pad
  (Sub 4 -> A[4]-Sprung, Angriff F680/F704/F694 bzw. NB1 F419). In NB2 laeuft er nach OSTEN zum Pad #36 ((-7527,-14724) F360 ->
  (6435,-9161) F1080 -> Sprung -> (6919,8142) F1200), dann den Nordstreifen zurueck nach Westen — **erster Angriff F2438 / F1939 /
  F1806**; der erste Angriff auf Leon ueberhaupt kommt 850..920 Bilder spaeter als im Original (F680 -> F1600, F704 -> F1577,
  F694 -> F1534). Spur der Sonde (`test_r36_zombieki_affen_block szene2 -14000 2000 -9013 -15461 2400 1 1`): "Umweg 1 ... Pad 36"
  ab F60, nach dem Umweg 1140 Bilder Sub 3 an der Klemme MIT Sicht 5200 vor Leon (Umweg aus, weil nah). Sonden-Gegenprobe
  (Umweg aus / an, je eigener Prozess): G1 Angriff 430 / **nie (blockiert 919)**, (-14000,4000) 458 / 1440, (-14500,6000) 433 /
  **nie (blockiert 867)**. Ursache im Code: `r36_uw_komp_bfs` ist eine ungewichtete Breitensuche ueber Komponenten; die erste
  Kante (Pads in SCA-Reihenfolge, `r36_uw_kanten` Schleife `i = 0..sca_count`) setzt den Vorgaenger — #36 kommt vor #47, also
  fuehrt JEDE Route Sued -> Nord ueber das Ost-Pad, egal wie weit (Raster `raster 800 1600`: beide Pads verbinden dieselben zwei
  Komponenten, das West-Pad liegt 9000 vom Gorilla, das Ost-Pad 17000 plus 25000 Rueckweg).

### 4.3 Nordwest-Tasche: die Befundstelle ist nur ein Punkt (exe + Sonde)
Die Befundstelle (-24000,10000) liegt in der Nordwest-Tasche (Raster `raster 800 1600`: x -29800..-20200, z 5200..15600, fuer
Radius 1600 eigene Komponente ohne Pad). Andere Plaetze DERSELBEN Tasche:
- exe (Kommandozeile 4.2): Leon (-26000,7000): **kein Angriff in 2700 Bildern**, Slot 3 steht ab F2100 bis Laufende am Taschenrand
  (-23314,13088) d 7270 Sub 3 (fest 726), Slot 2 (-20917,15208) d 10228 Sub 4 (fest 608) — Slot 2 kam auch hier ueber das
  Ost-Pad (F1050 (6253,-9453), F1200 Nordseite, Mangel 1). Leon (-27000,12000): **kein Angriff**,
  Slot 3 (-23302,13088) d 5081 Sub 3 (fest 705), Slot 2 (-20920,15204) d 8265 Sub 4 (fest 668). Leon (-22000,7000): **kein
  Angriff**, Slot 3 (-16172,5572) d 5877 (fest 743), Slot 2 (-14170,8059) d 8018 Sub 4 (fest 775). (v0.8.23 dort ebenfalls kein
  Angriff, Gorillas weit weg festgefahren.)
- Sonde `test_r36_zombieki_affen_block szene2 <lx> <lz> 0 0 2400 {1,0}` (beide Gorillas Original-Lage, je eigener Prozess):

| Leon | Umweg AN: G1 / G2 erster Angriff (blockiert) | Umweg AUS |
|---|---|---|
| (-24000,10000) gepinnt | 597 (1530) / 929 (0) | nie / nie |
| (-22000,11000) | 487 (1719) / 622 (0) | nie / nie |
| (-22000,7000) | **nie (1545) / nie (1678)** | nie / nie |
| (-26000,7000) | **nie (1809) / nie (1651)** | nie / nie |
| (-27000,12000) | **nie (1800) / nie (1643)** | nie / nie |

  Der NB2-Umweg fuehrt die Gorillas an den naechsten erreichbaren Knoten; liegt Leon tiefer als die Angriffsreichweite (~2050)
  hinter dem Rand, endet dort die Original-Nahjagd an der Klemme. Spur der Sonde (`szene2 -26000 7000 0 0 2400 1 1`): G2 steht ab
  F840 bei (-20904,15219) Sub 4, "Umweg 1 (-22600,13700)" (Randknoten, erreicht nie, klemme 0, keine Sicht); **G1 steht F720-F2400
  bei (-23302,13088) Sub 4, klemme 1, mit "Umweg 1 (-25800,6500)" — einem Zielknoten IN der Tasche, 1300 vor Leon**: der Umweg
  haelt Leons Knoten fuer zu Fuss erreichbar und steuert G1 1700 Bilder gegen die Wand. Ursache im Code: `r36_uw_route` setzt den
  Startknoten mit `r36_uw_knoten(e->x, e->z, 3)` = naechster freier Knoten im Ring 3 (bis 2400 Einheiten) ohne Pruefung, ob der
  Gorilla ihn erreicht; am Taschenrand liegt der naechste freie Knoten in der Tasche (andere Komponente), danach gilt "gleiche
  Komponente -> zu Fuss".
  Auch an der gepinnten Stelle steht G1 nach seinen Angriffen (Sub 6/15/5 F600-F840) von F960 bis F2400 in Sub 3 an der Klemme
  bei (-23302,13088), 3900..4500 vor Leon (Sonde: blockiert 1530 gesamt; der Riegel prueft nur "erster Angriff"). Auch hier
  laeuft G1 vom Start (-5525,-14883) zuerst ueber das Ost-Pad ("Umweg 1 ... Pad 36" F120-F360, Mangel 1).
  Der vom Bau-Agenten selbst gemessene Weitsprung vom Rand (`weit`, Path B @0x801180cc-f8, Landung 2050 vor Leon) wird vom
  Umweg nicht benutzt. Fuer den Nutzer: steht Leon in der Nordwest-Ecke des Parkplatzes, stehen beide Gorillas am Rand und
  greifen nicht an — Abnahme-1-Mangel 3 ist nur fuer den gepinnten Punkt behoben.

### 4.4 Urteil Punkt 2: **teilweise**
Die Blockade ist messbar und stark verringert (Zufallsgang -86 %, zehn exe-Stellen -82 % Festgefahren, gepinnte Befundstelle
beide Angriff). Zwei Befunde stehen dem Wortlaut "noch oft blockiert ... verbessern" entgegen: (1) die PORT-WAHL fuehrt im
Nordwest-Gang am West-Pad eine neue, sichtbare Fehlleitung ein — der Gorilla laeuft von Leon weg ueber die ganze Ostseite, Leon
wird 850..920 Bilder spaeter angegriffen als in v0.8.23, in der Sonde endet derselbe Gorilla danach festgefahren (867..919
Bilder), wo die Original-Regel ihn bei Bild ~430 angreifen laesst (Verschlechterung gegenueber v0.8.23); (2) in der
Nordwest-Tasche greift ausser an der gepinnten Stelle kein Gorilla an — beide stehen am Rand (exe 3 von 3 weiteren Plaetzen,
Sonde 3 von 4). Die Rest-Nahjagd an Pads/Diagonalen (344/396 Bilder exe, 13794 Bilder im Zufallsgang) ist als OFFEN mit Messweg
gefuehrt (GDB-Gegenmessung des Originals steht aus) und wird hier nur als Hinweis gewertet.

## 5. Gates

- **Bau:** OK (configure + build via local_build.sh, "ninja: no work to do" = HEAD, Abschnitt 0).
- **Spur-Tests** (direkt nachgefahren): `unit_r36_zombieki_affen_block` 17/17, `_sicht` 15/15, `_eintritt` 9/9, `_wecker` 2/2,
  `_szenen` 5/5, `_affen` 9/9 — alle messend mit Gegenprobe (Haken aus = Befund). Je Nutzer-Punkt vorhanden (P1: sicht/eintritt/
  wecker/szenen, P2: affen_block, P3: affen). Luecken: der Gorilla-Riegel pinnt in der Nordwest-Tasche nur (-24000,10000) und
  prueft dort nur den ERSTEN Angriff (G1 danach F960-F2400 an der Klemme), keine Szene im Nordwest-Gang (Umleitung, Abschnitt 4.2).
- **Gezielte Regression** (Module/Raeume des Diffs: enemy_ai_common.c = alle Gegner inkl. Zombie-Klemme und Gorilla-Zweig,
  enemy_ai_re2_zombie.c, main.c Schatten; Regex des Bau-Agenten erweitert um re2_crawler|knockdown|stagger|hit_reaction|
  re2_livepath|re2_hit_repeat|sca_contact90|sca_wall_hit|npc_wall_clamp|r16_aufstehen|r34_reaktion|r34_schaden|unit_damage|
  re2_gore|r26_mg_blut|re2_stepsize|re2_kfjump|poise|re2_hp_model|room_probe_sweep|r35_granate|r35_redhawk|r35_werfer|
  r34_granaten; `ctest --test-dir re15_port/build -R "<regex>" --timeout 600 -j 6 --output-on-failure`):
  **100% tests passed, 0 tests failed out of 196** (514,4 s; integration_r35_cut1150 514,4 s, r35_zgirl 261,6 s, r34_granaten
  210,9 s). Kein Fenster-Haken rot.
- **Volle Suite:** im Spurbaum nicht gefahren (Auftrag); Sache des Orchestrators im Integrationsbaum.
- **RE-Gate:** 9 Zitate/Offsets der NB2 selbst gelesen, alle stimmen (Abschnitt 1); Adressen auch in den Commit-Messages
  517a7dca / a76cd217 / 4285214f. Keine Guess-Tells, keine Env-Schalter als Abschluss; PORT-WAHL gekennzeichnet (Wurf-Klemme,
  Umweg, Stuhlhoehe -2000).
- **Fix erklaert den Befund?** Taschenfall: ja (Klemmen-Spur F124->F125, Wurf endet jetzt vor dem Keil, exe erneuter Griff F480).
  Waende als Moebel: ja (Beleg-Tabelle, exe 0/900). Befundstelle 11C0: nur fuer den gepinnten Punkt (Abschnitt 4.3).
- **Vertrag:** Pfade OK (`git diff master --name-only` ohne release/, platform/android/, shared_assets/PSX/, Test-CMakeLists;
  einzige Fremd-Zeile AUFTRAG.md stammt aus master 836023f5 = Nachtrag Spur J nach dem Abzweig, nicht aus diesem Zweig).
  Gemeinsame Dateien: enemy_ai_common.c +31/-11 (Haken je 1-2 Zeilen + zwei Exporte 5/6 Zeilen im Sichtfilter-Bereich, Spur F =
  Eigentuemer laut VERTRAG 1.4), enemy_ai_re2_zombie.c +2/-1, main.c +4. Keine Bank-9-Bits, Nachrichten-IDs, Ereignisse (grep in
  zombieki_r36.c: kein flag_set/msg_install/scd_event_fire). Tests nur ueber `probes/r36_zombieki.cmake`.
- **Nebenwirkungen beider Szenarien:** Elzas Parkplatz ROOM11C1 hat keine Gorillas (Pruefer-Sonde `probe_affen_raum zaehl`:
  5 x Typ 0x20, 0 x Typ 0x27; ueber alle RDTs spawnt nur ROOM11C0 Gorillas) — Umweg/Lache wirken nur in 11C0. Zombie-Seite:
  Zensus beider Varianten (Abschnitt 2.4) ohne neuen Fall; Wurf-Klemme im Tuerweg 1010 und in drei Schrotflinten-Laeufen
  (`schrot_*`, Wurf endet jeweils bei (3434,2983), erneuter Griff) ohne Auffaelligkeit.

## 6. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Zombies bleiben ewig beteiligungslos stehen, fast vor der Nase, auch innerhalb eines Raumes | **erfuellt** | exe ROOM1010 Abnahme-Lage: Taschenwurf endet vor dem Keil (3433,2983), erneuter Griff F480 (NB1: 600 Bilder auf der Stelle); Slot 2 Reaktion F30. Tuerweg 1020->1010: beide reagieren F30, Griffe F90/F210. ROOM1040 Ostgang 0/900 (NB1 895/396). ROOM10B0 Kante vor Leon. Lage-Zensus alle Stages + Elza: 638 Faelle, PASSIV/KLEMME nur ROOM1091 (andere Ebene). |
| 2 Monkeys ROOM11C0 oft blockiert — verbessern | **teilweise** | Verbessert: Zufallsgang 114029 -> 15857 (-86 %), zehn exe-Stellen Festgefahren 14726 -> 2662, gepinnte Befundstelle beide Angriff. Nicht erfuellt: neue Umleitung ueber das Ost-Pad (Nordwest-Gang: erster Angriff F680/F704/F694 -> F1600/F1577/F1534, Slot 2 F2438/F1939/F1806), Nordwest-Tasche ausser dem gepinnten Punkt weiter ohne Angriff (exe 3/3, Sonde 3/4), Umweg steuert G1 1700 Bilder in die Wand. |
| 3 Blut der toten Monkeys laeuft nicht aus | **erfuellt** | exe NB2 Cut 5: Leiche ab F294, Lache 292 -> 494 rot-dunkle Pixel ueber ~91 Bilder, dann konstant; Bildstreifen; Riegel 9/9; Code seit Abnahme 1 unveraendert. |

## 7. Maengel (nummeriert, nachpruefbar)

1. **P2 Regression durch a76cd217: der Umweg nimmt das Pad nach SCA-Reihenfolge, nicht nach Weg.** `r36_uw_komp_bfs` ist eine
   ungewichtete Breitensuche ueber Komponenten; die erste Kante (Pads in SCA-Index-Reihenfolge, #36 vor #47) setzt den Vorgaenger.
   NB1 (`r36_uw_pad`, 7a9a0e7f) nahm das Pad mit dem naechsten Anlaufpunkt. Folge: jede Route Sued -> Nord (gemessen; nach dem
   Code ebenso Nord -> Sued) laeuft ueber das
   Ost-Pad #36. exe `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<-14000,2000 | -14000,4000 | -14500,6000>,1024,0
   RE15_EXIT_AT=2700`: Slot 2 laeuft von (-9013,-15461) nach Osten (F1080 (6435,-9161), F1200 Sprung), erster Angriff F2438 / F1939 /
   F1806 (v0.8.23 F680 / F704 / F694, NB1 F419 bei (-14000,2000)); erster Angriff auf Leon ueberhaupt 850..920 Bilder spaeter als
   v0.8.23. Sonde `test_r36_zombieki_affen_block szene2 -14000 2000 -9013 -15461 2400 {0|1} [1]`: Umweg aus G1 Angriff 430 / an
   **nie, blockiert 919** (Spur: "Umweg 1 ... Pad 36" ab F60); (-14500,6000): 433 / nie (867). Abnahmekriterium: Pad-/Routenwahl
   nach Weglaenge; an den drei Stellen Slot 2 nicht spaeter als v0.8.23, Sonde G1 greift an; ein Riegel pinnt eine Szene im
   Nordwest-Gang mit Gorilla-Start (-9013,-15461).
2. **P2 Nordwest-Tasche nur am gepinnten Punkt behoben (Abnahme-1-Mangel 3 nicht verallgemeinert).** exe (Kommandozeile wie 1)
   Leon (-26000,7000) / (-27000,12000) / (-22000,7000): **kein Angriff in 2700 Bildern**, Gorillas am Rand festgefahren
   (726/608, 705/668, 743/775 Bilder, d 5081..10228). Sonde `szene2 <lx> <lz> 0 0 2400 1`: (-22000,7000), (-26000,7000),
   (-27000,12000) beide nie (blockiert 1545..1809); an der gepinnten Stelle G1 nach den Angriffen F960-F2400 an der Klemme (Sonde blockiert 1530).
   Teilursache im Code (Sonde `szene2 -26000 7000 0 0 2400 1 1`): `r36_uw_route` nimmt als Startknoten `r36_uw_knoten(e->x, e->z,
   3)` ohne Erreichbarkeitspruefung — am Taschenrand (-23302,13088) liegt er IN der Tasche, der Umweg haelt Leon fuer zu Fuss
   erreichbar ("Umweg 1 (-25800,6500)") und steuert G1 F720-F2400 gegen die Wand (klemme 1). Der selbst belegte Path-B-Weitsprung
   vom Rand (@0x801180cc-f8, Sonde `weit`: Landung 2050 vor Leon) wird nicht genutzt. Abnahmekriterium: an mindestens diesen drei
   Plaetzen greift ein Gorilla an (exe + Sonde), der Riegel prueft die Blockade auch NACH dem ersten Angriff.
3. **Voll-Suite** fuer den Endstand: vom Orchestrator im Integrationsbaum zu fahren (gezielte 196 Tests hier gruen).

Hinweise (kein Mangel): (a) Rest-Nahjagd an Pads/Wagen-Diagonalen (exe West-/Ost-Pad 344/396 Bilder, Zufallsgang 13794 nah) ist
als OFFEN mit Messweg gefuehrt; "Original-Nahjagd" ist statisch belegt (B[3] playerX @0x80117d4c), die GDB-Gegenmessung steht aus.
(b) ROOM10B0: die Terrassen-Zombies treten 3,3..3,6 m vor Leon an der Kante auf der Stelle (per Daten kein Abstieg). (c)
ROOM1091<-10F1: Elza steht auf y -9000 (Band 5), die Zombies auf y -1800 — fuer Spur F ein Zensus-Artefakt; ob die Eintrittshoehe
selbst stimmt, ist nicht Gegenstand dieser Spur. (d) PSX-Laufzeitbudget (Nutzer-Regel 2026-09-03 "die Laufzeit schlaegt nach, sie
rechnet nicht"): der Umweg baut zur Laufzeit ein Raster (~5000 Klemmen-Aufrufe beim ersten Gebrauch), sucht je Bild und Gorilla
Kanten/Komponenten und bei unerreichbarem Leon ueber alle Knoten; statische Puffer ~240 KB (R36_UW_MAX 12288: frei/komp/dist
60 KB, zwei `static int q[12288]` 96 KB, s_uw_ks/kv/kp + Komponenten-Queue 84 KB). Fuer ROOM11C0 (einziger Gorilla-Raum) waere
das Raster offline als Tabelle ableitbar. (e) Abtastschritt 50 in `r36_strahl_darueber` und die Umweg-Parameter (Ring 3/4,
8 Knoten Vorschau) ohne eigene Kennzeichnung (unter der PORT-WAHL "Umweg").
