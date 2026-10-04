# Runde 35 — Spur A "granate": UNABHAENGIGE ABNAHME 0

Stand: 2026-10-03, Baum `.claude/worktrees/r35_granate`, Zweig `r35/granate`, HEAD 46976419 (6 Commits auf master 154a73c1).
Abgenommen wird gegen den WORTLAUT in `AUFTRAG.md`. Alles unten ist an der gebauten exe dieses Baums selbst gemessen
(`re15_port/build/platform/pc/re15_pc.exe`, 6750915 B; `local_build.sh configure` + `build` -> "ninja: no work to do" =
exe entspricht HEAD). Messlaeufe (unversioniert): `re15_port/build/abn0_mess/<lauf>/` (gr.log = RE15_GRANATE_LOG,
state.log = RE15_STATE_LOG, wf.log = RE15_WAFFEN_LOG, debug.log, fd_*.ppm = RE15_FRAMEDUMP), Startskript
`abn0_mess/lauf2.sh <name> <timeout> VAR=...` (Kopie der exe unter eigenem Namen, Basis-Umgebung wie
`tests/integration/test_r35_granate.cmake`).

## Urteil

**NICHT BESTANDEN.** Punkt 1 (Wand) ist nur teilweise erfuellt: zwei an der exe gemessene Defekte (Durchflug durch
duenne Wandzellen; Sofort-Explosion in freiem Gelaende im Begrenzungsrechteck nicht-rechteckiger Zellen — im
Affen-Raum ROOM11C0 in Leons Hand) und eine nicht gekennzeichnete Abweichung vom zitierten RE2-Mechanismus. Punkte
2-5 halten der Messung stand. Suite gruen, Pfad-Gate haelt, zitierte Adressen stimmen.

| # | Punkt (Wortlaut) | Urteil |
|---|---|---|
| 1 | Granaten sollen nicht durch die Wand fliegen | **teilweise** |
| 2 | Range der Explosion viel zu niedrig, fast unmoeglich Gegner zu treffen | erfuellt (mit Hinweisen) |
| 3 | Die Granate hat den falschen Explosionssound | erfuellt |
| 4 | Mehr Brutalitaet am Zombie (Zerplatzen, abplatzende Beine, Arme) | erfuellt |
| 5 | Tests bei Birkin und Alligator hinzufuegen | erfuellt (Pin-Schwaeche, Mangel 5) |

## 0. Bau und Suite (selbst gefahren)

```
bash re15_port/tools/local_build.sh configure   -> === LOCAL-BUILD-OK (configure)
bash re15_port/tools/local_build.sh build       -> ninja: no work to do. / === LOCAL-BUILD-OK (build)
bash re15_port/tools/local_build.sh test        -> 100% tests passed, 0 tests failed out of 480
                                                   === LOCAL-BUILD-OK (test) — Tests 480/480   (1309,7 s, parallel zu meinen Messlaeufen)
```
Darin: `unit_r35_granate` Passed 0,07 s, `integration_r35_granate` Passed 61,4 s (`[wand] ok — Wand x=8385 -> 8332,
Explosion Tick 349`, `[zombie] ok — X=119, 2 Zombies zerrissen, 3 Eingriffe`, `[gator] ok — 23 Slot 15: HP 3000 -> 2000`,
`[birkin] ok — 36 Slot 2: HP 600 -> 520`), `integration_r34_granaten` Passed 204,8 s, `unit_r34_wurf/_schaden/_reaktion` Passed.
Kein Fenster-Haken rot. Log: `re15_port/build/abn0_mess/suite.log`, `re15_port/build/local_build_ctest.log`.

## 1. Punkt 1 — "Granaten sollen nicht durch die Wand fliegen": TEILWEISE

### 1.1 Was haelt (gemessen)
* Ausgangsbefund unabhaengig reproduziert mit der exe des Hauptbaums (v0.8.21, 2026-09-30) als Kopie, Lauf `vor_wand`
  (ROOM1140, Leon (-4311,-19289) Blick 0, Skript `M0.6,MA0.2,M2.5,W5`): `sca_pfad.py` meldet 7 Bilder F103..F109
  `IN-WAND [x8350 z-20750 w4100 d1150 typ01 u0ff]`, Liegen x 8612, `EV resolver ... r=500`, `SE esp code=0x04080001 -> CORE`.
  Die Vorbedingung des Fixes steht also im Protokoll.
* Nachher, gleiche Aufstellung (Lauf `wand`): `T=349 EV wand wpos=(8385,-12,-19739) -> rueckzug (8332,-8,-19764) ->
  explosion sofort`, `T=349 EV resolver art=2 P=(8332,-508,-19764) r=2000`, Platz frei T=356; kein Bild mehr jenseits 8350.
* Weitere dicke Rechteckzellen stoppen ebenfalls: ROOM1140 Westwand (Lauf `hoehe_m`: `EV wand wpos=(-8618,-15,-13048)`),
  ROOM1220 Zellenfront in Phase A (Lauf `t1220h1`), ROOM2090 (`boss_2090b`: `EV wand wpos=(7250,-390,-19117)`),
  ROOM5090 (`boss_5090d`).

### 1.2 Defekt A — Durchflug durch duenne Wandzellen (exe, ROOM1220 = der Zellentrakt aus dem Auftrag)
Der Wandtest ist EIN Punkttest je Bild (`re15_granate_r35_wand`, granate_r35.c:21-27, gerufen am Kopf von
`esp_fx_dispatch_b_29`, re15_esp.c:1150). Die Granate legt je Bild 380 (HOCH, `drift_x = 0x17c` @0x80018494) bzw. 280 (MITTE, @0x800184bc)
zurueck. Eine Zelle, die duenner ist als der Schritt, wird je nach Phase uebersprungen.
```
Lauf t1220h1: RE15_DEBUG_JUMP=1220@250 RE15_PLAYER_POS=-19533,-9700,2048 Skript MU0.6,MUA0.2,MU2.5,W5 (HOCH)
  wpos.x: -19723 -20103 -20481 -20857 -21231 | T=289 EV wand wpos=(-21603,-3621,-10677) -> rueckzug (-21107,...)   [gestoppt]
Lauf t1220h2: wie oben, nur RE15_PLAYER_POS=-19433,-9700,2048 (100 Einheiten weiter)
  T=289 F=43 wpos=(-21503,-3621,-10677)      <- vor der Zelle (frei)
  T=290 F=44 wpos=(-21873,-3681,-10698)      <- HINTER der Zelle (frei), kein "EV wand"
  ... 11 weitere Flugbilder IN der Gefaengniszelle ...
  T=301 EV wand wpos=(-25811,-3681,-10929) -> explosion sofort     (Einbau in der Zelle, 4000 hinter der Front)
```
Uebersprungene Zelle: ROOM1220.RDT SCA `typ1 x[-21825..-21550] z[-19875..-4300] w275 d15575 u0=ff floor=03` (Zellenfront
zum Flur). Punkt (-21603,-10677) liegt in genau dieser Zelle, (-21503,..) und (-21873,..) in keiner (eigener Abgleich
aller 35 Zellen). Bild: `abn0_mess/t1220h2/kontakt_1220_durchflug.png` (Bild 56: Feuerball IM Zelleninneren).
Fenster des Durchflugs bei senkrechtem Auftreffen: (380-275)/380 = 28 % der Wurfpositionen (HOCH), bei den 200 dicken
Zellen 47 % (HOCH) bzw. 29 % (MITTE). Freistehende Zellen < 380 (eigener Zensus aller RDTs, beide Seiten frei): ROOM1220
(275 / 225 / 200, u.a. `x[-17275..-17075] z[-28650..-4300]`, Nordwand `z[-400..-200]` = Raumgrenze), ROOM11D0 (5 x 200),
ROOM30D0 (175). Die Sonde `unit_r35_granate` 102-108 und der exe-Pin `wand` messen nur EINE 4100x1150-Zelle in EINER
Phase und sehen das nicht.
Hinweis: RE2 tastet ebenfalls je Bild (FUN_8004fba0 @0x8001eea0) — das aendert nichts am Wortlaut des Nutzers.

### 1.3 Defekt B — Sofort-Explosion in freiem Gelaende: Begrenzungsrechteck statt Zellform (exe, ROOM11C0)
`re15_granate_r35_wand` ruft `re15_collision_box_blocked` (re15_collision.c:1069-1088). Diese Funktion prueft fuer JEDEN
Zelltyp nur das achsparallele Rechteck `x..x+width / z..z+density`. Die Spielerkollision desselben Moduls unterscheidet
Typ 1 Rechteck, 2 Raute, 3 Kreis, 4/5/6/7 Dreiecke, 8/9 Kapseln (re15_collision.c:735-745). Eigener Zensus (206 Raeume,
eindeutige Zellen mit u0&1): 4666 Zellen, davon **642 (13,8 %) nicht rechteckig, in 134 Raeumen**.
```
Lauf t11c0c: RE15_DEBUG_JUMP=11C0@250, Ada-Szene laeuft bis Bild 1070 (pst 4 -> 1), danach Leon frei bei (-7157,-12355),
             RE15_INPUT_SCRIPT_START=1075 Skript M0.6,MA0.2,M2.5,W5 (MITTE auf den Affen Slot 2)
  F=1115 SPAWN granate art=2 slot=0 anker=(-6720,-2474,-13266) gier=467
  T=1361 EV wand wpos=(-6720,-2474,-13266) -> rueckzug (-7024,-2391,-13044) -> explosion sofort
  T=1361 EV resolver art=2 P=(-7024,-2891,-13044) r=2000 eingriffe=1
```
= 0 Bilder Flug, Explosion im Wurfbild 2474 ueber dem Boden neben Leon. Der Punkt liegt in genau EINEM Zellrechteck:
`typ2 x[-15400..4799] z[-13266..4921] w20199 d18187 u0=ff floor=02` (Raute, Mitte (-5300,-4172), Halbachsen 10100/9093),
und dort AUSSERHALB der Raute: |dx|/hw + |dz|/hd = 1420/10100 + 9094/9093 = 1,14 > 1 (push_diag2, re15_collision.c:412-471:
solide ist das Innere der Raute). Leon selbst steht frei auf (-7157,-12355) — ebenfalls nur in diesem Rechteck (1,08).
Bild: `abn0_mess/t11c0c/kontakt_11c0.png` (Feuerball ueber Leon auf dem offenen Parkplatz). Dass der Affe dabei stirbt
(HP 180 -> -820), liegt an der neuen Reichweite und am entfallenen Eigenschaden, nicht am Wurf.
ROOM11C0 hat fuenf solche Rauten und drei Dreiecke (2x Typ 4, 1x Typ 6) mit Kanten bis 25600; weitere grosse Nicht-Rechtecke u.a. ROOM1090
(Kreise 7320/5700), ROOM11B0 (Kreise 4800/4500), ROOM1190 (Dreieck 4914x12096 — Lauf `t1190b`: Leon laeuft frei bis
x -21260, 360 Einheiten IM Rechteck der Zelle `typ7 x[-25814..-20900]`).
Die Referenz selbst prueft die Form: RE2 FUN_8004fba0 `andi v1,v1,0xf / sltiu v0,v1,0xe / ... lw v0,4356(at) / jr v0`
@0x8004fe34-54, Sprungtabelle @0x80011104 mit 14 Eintraegen (0x8004fe5c, 0x8004fe7c, ... je `jal` Formtest,
`bne v0,zero,0x8004ffb0`).

### 1.4 Abweichung C — Zellhoehe: niedrige Hindernisse sind fuer die Granate Waende (exe, ROOM1140)
RE2 setzt das Wand-Flag nur, wenn die Geschosshoehe im Hoehenfenster der Zelle liegt: FUN_8004fba0 @0x8004ffd4-0x8005000c
(`lw v1,4(s3)` / `slt v0,s1,v1` / `bne ...0x8005005c` / `slt v0,s0,v1` / `beq ...0x80050028` / `ori v0,v0,0x1` /
`sw v0,-13368(at)` = 0x800dcbc8; s1/s0 = Unter-/Oberkante aus Zelle +12/+10 @0x8004fde4-0x8004fe30, 0x8004ffb0-d0).
Ausserdem setzt RE2 das Flag bei y > 0 (`blez v1,0x8004fc5c` / `sw v0,-13368(at)` @0x8004fc48-58) und an Objektkaesten
(@0x8004fcfc-0x8004fd18). Der Port stoppt an jeder u0&1-Zelle in jeder Hoehe:
```
Lauf tisch: RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=0,-10300,1024 Skript M0.6,MA0.2,M2.5,W5
  F=41 SPAWN anker=(-379,-2474,-11239)
  T=288 EV wand wpos=(-349,-2524,-11519) -> rueckzug (-389,-2454,-11147) -> explosion sofort     eingriffe=0
```
Zelle `typ1 x[-4650..7450] z[-17450..-11350]` = der Konferenztisch; Bild `abn0_mess/tisch/kontakt_tisch.png` (Feuerball
in der Luft ueber der Tischkante, ein Bild nach dem Wurf). Ueber Tische/Autos (ROOM11C0-Rauten `floor=02`) laesst sich
nicht mehr werfen. Das Dossier kennzeichnet nur die MASKE u0&1 als PORT-WAHL (OFFEN 1); Zellform (1.3) und Zellhoehe
stehen nirgends als Abweichung — der Text "RE2 FUN_8001ED9C Wandregel" deckt die Umsetzung an dieser Stelle nicht.

## 2. Punkt 2 — Reichweite: ERFUELLT (mit Hinweisen)
```
Lauf zomb2: RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=-1676,-18070,1076 RE15_AI_FLAVOR=re2 Skript MD0.6,MDA0.2,MD2.5,W5 (TIEF)
  T=329 EV se ... liegen (-2199,20,-20548)   T=365 EV resolver art=2 P=(-2199,-480,-20548) r=2000 eingriffe=3   (X = F119 = L+36)
  F118 -> F119: Slot 2 t=10 @(-1334,-19530) hp 50 -> -150  (Abstand zu P 1336)
                Slot 3 t=10 @(-1529,-20930) hp 80 -> -120  (771)
                Slot 4 t=11 @(2,-20640)     hp 250 -> 50   (2203)
                Slot 1 t=16 @(-800,-20600)  hp 79 unveraendert (1400; Typ 0x16 mit ss1=7 — Ursache nicht untersucht)
```
Vorher (Lauf `vor_wand`, exe v0.8.21): `r=500`, Zylinder 900 -> von diesen dreien waere nur Slot 3 getroffen. Sonde
`test_r35_granate reichweite` selbst gefahren: 120-134 gruen (vorn 2300 ja / 2600 nein, hinten 1900 ja / 2100 nein,
seitlich +-2300 ja / 2600 nein). Beleg selbst disassembliert: RE2 Op 47 @0x80020c3c (Optabelle 0x8009d868[47]),
`lui a2,0x8001 / addiu a2,a2,2328` @0x80020c58-5c, Bytes @0x80010918 `30 f8 00 00 e8 03 f4 01` = {-2000,0,1000,500},
`lui a3,0x1002 / ori a3,a3,0x9` @0x80020d54-58, `addiu v0,v0,900` @0x80020d98, zwei `jal 0x800470c0`.
Hinweise (kein Abzug am Wortlaut, aber fuer den Nutzer):
* MITTE/HOCH ueberfliegen Gegner weiterhin (Laeufe `hoehe_m`/`hoehe_h`: Zombies 3000 vor Leon, Explosion an der Wand
  6700-8600 dahinter, eingriffe=0). Treffen kann praktisch nur TIEF. Der RE2-Flugkontakt ist bewusst nicht verdrahtet
  (Dossier 4.2).
* Hoehenband: getroffen wird nur, wenn P.y im Band des Gegners liegt (Sonde 129). Aus dem Band (b98 -1500, h9e 1500,
  zwei Pruefhoehen P und P+900) folgt fuer stehende Zombies: Zuendhoehe ueber 3500 trifft niemanden am Boden. Der
  HOCH-Wurf liegt in den Flugbildern 3-20 darueber (gemessen `t1220h2`: wpos.y -3551..-3831), eine Wandzuendung in
  diesem Fenster (`t1220h3`: P=(-21324,-4025,-10649)) ist am Boden wirkungslos. ABGELEITET aus Code + gemessener Bahn;
  ein exe-Lauf mit einem treffbaren Gegner im Rechteck fehlt mir (die beiden Zombies in ROOM1220 liegen am Boden
  und wurden in keinem meiner Laeufe getroffen, auch nicht bei der Explosion IN ihrer Zelle, `t1220h2`).
* Leon wird von der eigenen Explosion nicht mehr verletzt (Sonde 133; RE1.5 hatte 1000 Schaden unter 950). Nicht
  beauftragt, im Dossier offengelegt — der Nutzer muss das wissen.

## 3. Punkt 3 — Explosionssound: ERFUELLT
Gemessen am gemischten Audio-Ausgang, nicht nur am Log:
```
Laeufe ton_wurf / ton_leer: Audio AN, RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1, Aufstellung "wand";
  ton_leer = identisch ohne Wurf (Skript M0.6,M0.2,M2.5,W5). Differenz der beiden Aufnahmen (1470 Stereo-Frames je Bild):
  Segment Ticks 356..383 (0,93 s), Spitzen-RMS 10398, Spitze 15453
  Korrelation des Segments mit info/re2leon/COMMON/SOUND/ARMS09_00001.wav (22344 Samples, bei 23352 Hz 0,96 s): 0,996
  (ARMS09_00000/2/3: -0,16 / -0,04 / -0,03)
  debug.log: [se] Stimme: se=10 layer=0 vag=2 ... pitch=0x879 (23352 Hz)  (+ layer=1)
  wf.log:    SE  re2fx code=0x01110001 -> ARMS0F Satz 10 / SE  zusatz ARMS0F satz=10 ; kein 0x04080001
```
Dateien selbst geprueft: md5 `786ad6910be7a9ea8bf1df0b145ad55b` fuer shared_assets/PSX/SOUND/ARMS0F.VB, info/Re1.5/PSX/SOUND/
ARMS0F.VB und info/re2leon/COMMON/SOUND/ARMS09.VB; RE2 ARMS09.EDH @0x44 `00 00 33 20`, RE1.5 ARMS0F.EDH @0x28 `00 00 33 20`.
RE2 `lui a0,0x111 / ori a0,a0,0x1 / jal 0x8005ba28` @0x80020d40-48 und RE1.5 `lui a0,0x408 / ori a0,a0,0x1 / jal 0x80045024`
@0x800185e4-ec selbst disassembliert. Es spielt hoerbar das RE2-Granatwerfer-Explosionssample (0,96 s; der alte
CORE-Satz 8 laut Dossier 0,47 s). Ob es der vom Nutzer gemeinte Ton ist, kann nur sein Ohr sagen.

## 4. Punkt 4 — Brutalitaet: ERFUELLT
Lauf `zomb2` (s. 2): Slots 2 und 3 (Typ 0x10) im Explosionsbild F119 `st=3 ss1=9`, F120 `mo=2` (Zerreissen, vorher
laut Dossier Clip 1), Lage Slot 2 (-1334,-19530) -> F126 (-1803,-21213), Slot 3 (-1529,-20930) -> (-1441,-22673),
F170 `st=7 hp=-1` (Leiche), keine Wiederbelebung bis F250. Bilder `abn0_mess/zomb2/kontakt_gore.png`, `gross_gore.png`
(Bild 122/125: Blutwolke am Zombie, abgetrenntes Glied fliegt durch den Feuerball). Sonde `gore` selbst gefahren: 6/6
Laeufe Spalte +0x1D2 = 1, Handler 5 (0x80108BEC), 3 fliegende Teile, Leiche. Tabelle selbst gelesen: EMOVL10_S0.BIN
@0x8010CD68 = {0x80107438, 0x80108BEC, 0x80108BEC, 0x80108530 x6}. Spalte 1 ist als PORT-WAHL / NUTZER-VORGABE gekennzeichnet.
Beobachtung (nicht beauftragt): Slot 2 stand nordoestlich der Explosion und fliegt nach Suedwesten — durch den
Explosionspunkt hindurch; beide Zombies fliegen in Wurfrichtung, nicht vom Explosionspunkt weg.

## 5. Punkt 5 — Tests Birkin und Alligator: ERFUELLT (Tests vorhanden und gruen), Pin-Schwaeche
* `unit_r35_granate` Abschnitt [5] (180-189): Birkin 0x30, 0x36@3080, G5 0x36@5090 (armiert, grid 0x13), Alligator 0x23,
  Gator-Boss@2090, je 300 und 1500 Abstand, beide KI-Flavors; misst Treffer, HP, Verlassen des Trefferzustands bzw.
  Modulreaktion. Selbst gefahren: gruen.
* exe-Pins `gator` / `birkin` (RE15_FORCE_EXPLOSION, kein Wurf). Aus dem Suite-Lauf (lauf_f8c67e26):
  ```
  gator  F59 [15 t=23 st=1 ...] hp=3000 | F60 st=2 ss1=9 hp=2000 | F61 st=1 mo=10, laeuft weiter
  birkin F59 [2 t=36 st=0 g=33 mo=0 af=0 @(-14700,-23350)] hp=600 | F60..F99 st=2 ss1=9 g=33 mo=0 af=0 hp=520 (unveraendert)
  ```
  Der Birkin-Pin trifft den UNARMIERTEN, geparkten G5 (grid 0x33, RDT-Spawn ausserhalb jeder Cut-Zone, vom Modul nicht
  getickt: enemy_ai_boss_g5.c:1454-1507) und prueft nur HP-Abfall + "exe bis EXIT_AT". Eine Reaktion des Bosses prueft
  er nicht; er bliebe gruen, wenn der armierte Boss haengen wuerde.
* Eigene Gegenmessung am ARMIERTEN Birkin mit echtem Wurf (Lauf `boss_5090d`: RE15_DEBUG_SUB=4@255,
  RE15_PLAYER_POS=-5000,-23400,2048, TIEF ab Bild 420): `[debug-sub] Frame 255: sub04 ... rc=0`, G5 grid 0x13 bei
  (-9000,-23400); `T=731 EV resolver ... eingriffe=1`; F485 hp 600 -> 520; danach kriecht er weiter (x -4085 -> -1986 ->
  -1302 -> 1960, mo 1/3/4/2) — kein Haenger. Ein echter Wurf am Alligator in ROOM2090 ist mir nicht gelungen
  (`boss_2090b`: Auto-Ziel nimmt eine Spinne, Gator 16000 entfernt) -> dort nur Sonde + FORCE_EXPLOSION-Pin.

## 6. RE-Gate
* Jede Konstante im Diff traegt `@0x...` oder ist PORT-WAHL / NUTZER-VORGABE (Maske u0&1, K0 + Zone 1, Flugkontakt aus).
  Suche im Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO/getenv: 0 Treffer. Kein Env-Schalter.
* Selbst disassembliert und mit dem Zitat verglichen (alle stimmen):
  1. RE2 FUN_8001ED9C @0x8001ef80-0x8001f108: `lw v0,-13368(v0)` / `beq v0,zero,0x8001f10c`; Rueckzug `subu v0,v0,v1 /
     sh v0,12(t2)` @0x8001efd4-d8, zweiter Abzug @0x8001f000-04, `lui t1,0x5555 / ori t1,t1,0x5556` @0x8001ef90-94, `mult / mfhi /
     sra 31 / subu` -> pos -= vel' + vel''/3; `jal 0x8001d894` @0x8001f0cc; `lbu v0,3(v1) / lb v1,27(v1) / ... lw v0,-10136(at) /
     jalr v0` @0x8001f0e0-104. `rueckzug_achse` (granate_r35.c:80-87) bildet das ab.
  2. RE2 Op 47 @0x80020c3c (s. 2) samt Datenbytes @0x80010900/@0x80010918.
  3. RE1.5 R29 `lh t1,42(t0) / blez t1,0x8001842c` @0x80018330-38; R31 `ori a0,zero,0x1f4` @0x80018598, `addiu v0,v0,-500`
     @0x800185a8, `ori a2,zero,0x2` @0x800185b4, `jal 0x80012d60` @0x800185b8.
  4. RE2 DEATH-Zeile @0x8010CD68 (s. 4); Sounddateien (s. 3).
* Ungenau im Dossier: Kontakt und Lebensdauer-Ende springen ueber `lbu v0,2(v1)` (step[2], @0x8001ef74 -> `j 0x8001f0e4`),
  nur die Wand ueber step[3]; das Dossier schreibt fuer den Kontakt "Op 47". Ohne Wirkung (Kontakt nicht verdrahtet).
* **Gate-Befund:** FUN_8004fba0 ist KEIN reiner "Zellkontakt"-Setzer, sondern prueft Form (Tabelle 0x80011104) und
  Hoehenfenster und setzt das Flag auch bei y > 0. Die Umsetzung (Rechteck, jede Hoehe) weicht davon ab und ist nicht
  gekennzeichnet (Maengel 2, 3).
* Erklaert der Fix den Befund? Ja fuer die gemessene Zelle (Vorbedingung selbst reproduziert, 1.1) — nicht fuer die
  Fehlerklasse "Wand" insgesamt (1.2, 1.3).

## 7. Vertrags-Gate
* Pfade (`git diff master --name-only`, 18 Dateien): kein `release/`, kein `platform/android/`, kein `shared_assets/PSX/`,
  keine Aenderung an `tests/unit/CMakeLists.txt` / `tests/integration/CMakeLists.txt`; Tests in `probes/r35_granate.cmake`,
  `test_r35_granate.c`, `test_r35_granate.cmake`. Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse belegt. Keine neuen Assets.
* Gemeinsame Dateien: `platform/pc/main.c` +2/-1 (ok). `engine/src/re15_esp.c` +37/-8 — VERTRAG 1.4 erlaubt dort "NUR kleine
  Haken (1-5 Zeilen)"; die neue Funktion `esp_granate_sofort` und der Hakenblock liegen darueber (Mangel 7).
* Tests messend? Ja fuer Reichweite, Ton, Gore, Bosse (Sonde). Luecken: Wand nur eine dicke Rechteckzelle (Maengel 1-3);
  Birkin-Pin (Mangel 5); `PRUEF(154, pos_se[1] == -491 || pos_se[1] == pos_se[1], ...)` (test_r35_granate.c:352) ist immer wahr (Mangel 6).

## 8. Maengel (nachzubessern, in dieser Reihenfolge)
1. **Durchflug durch duenne Wandzellen.** Punkttest je Bild bei 380 (HOCH) / 280 (MITTE) Schritt. Gemessen `t1220h2`:
   ROOM1220, Leon (-19433,-9700), HOCH: wpos.x -21503 -> -21873 ueberspringt `typ1 x[-21825..-21550] z[-19875..-4300]`,
   Explosion erst bei x -25811 in der Zelle; Kontrolle `t1220h1` (Leon x -19533) stoppt bei -21603. Gefordert: Test der
   STRECKE zwischen vorheriger und neuer Weltlage gegen die Zellen; Pin, der beide Phasen (-19533 / -19433) faehrt.
2. **Sofort-Explosion in freiem Gelaende (Rechteck statt Zellform).** `re15_collision_box_blocked` ignoriert den Zelltyp;
   642 von 4666 Zellen (13,8 %, 134 Raeume) sind nicht rechteckig. Gemessen `t11c0c`: ROOM11C0, Leon frei bei
   (-7157,-12355), `F=1115 SPAWN anker=(-6720,-2474,-13266)` und im selben Tick `EV wand ... explosion sofort`; der Punkt
   liegt nur im Rechteck der Raute `typ2 x[-15400..4799] z[-13266..4921]`, ausserhalb der Raute (1,14 > 1). Gefordert:
   formgenauer Test fuer Typ 2-9 (RE2 @0x8004fe34-54, Tabelle 0x80011104; im Port vorhanden: push_diag2/4/5/6/7,
   push_circle, push_caps8/9), exe-Pin in ROOM11C0 (Skriptstart 1075).
3. **Zellhoehe / niedrige Hindernisse — nicht gekennzeichnete Abweichung.** RE2 @0x8004ffd4-0x8005000c setzt das Flag nur
   im Hoehenfenster der Zelle. Gemessen `tisch`: ROOM1140, Wurf ueber den Konferenztisch `typ1 x[-4650..7450]
   z[-17450..-11350]` -> `EV wand wpos=(-349,-2524,-11519)` ein Bild nach dem Wurf, 2524 ueber dem Boden. Gefordert:
   entweder belegte Hoehenregel (RE1.5-Zellfeld `floor` Low-Nibble 2/3 und u0 untersuchen) oder im Dossier unter OFFEN als
   Abweichung mit diesen Adressen und fuer den Nutzer ausgeschrieben ("ueber Tische und Autos laesst sich nicht werfen").
4. **Dossier 3.1/4.1 korrigieren:** FUN_8004fba0 = Form + Hoehe + y > 0 + Objektkaesten, nicht nur "Zellkontakt";
   Kontakt/Lebensdauer springen ueber step[2], die Wand ueber step[3].
5. **Birkin-exe-Pin misst den geparkten Boss.** `integration_r35_granate [birkin]`: Slot 2 grid 0x33, st 0 -> 2, danach 39
   Bilder unveraendert, geprueft wird nur HP 600 -> 520. Gefordert: Boss armieren (RE15_DEBUG_SUB=4@255, gemessen
   `boss_5090d`) und Reaktion pruefen (Lage/Motion aendert sich nach dem Treffer).
6. **Tautologie** in `tests/unit/test_r35_granate.c:352` (PRUEF 154) — die SE-Lage wird nicht geprueft.
7. **Vertrag 1.4:** `engine/src/re15_esp.c` +37/-8 statt 1-5 Zeilen Haken; `esp_granate_sofort` und der Flugtest-Block
   gehoeren nach granate_r35.c (ein Aufruf bleibt in re15_esp.c).
8. Klein: audio_pc.c:1103 Kommentar "nur die zwei Aufschlag-Baenke vorgesehen" ist mit `ARMS_ZUSATZ_N 3` ueberholt.

## 9. Fuer den Nutzer offen zu entscheiden (keine Maengel der Spur, aber Verhaltensaenderungen)
* Kein Eigenschaden mehr durch die eigene Granate.
* Zombies fliegen in Wurfrichtung, auch durch den Explosionspunkt hindurch.
* MITTE/HOCH-Wuerfe ueberfliegen Gegner; hohe Wandzuendungen (HOCH, Flugbilder 3-20) treffen am Boden nichts (abgeleitet).
