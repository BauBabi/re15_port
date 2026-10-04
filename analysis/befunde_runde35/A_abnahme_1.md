# Runde 35 — Spur A "granate": UNABHAENGIGE ABNAHME 1

Stand: 2026-10-03, Baum `.claude/worktrees/r35_granate`, Zweig `r35/granate`, HEAD **a30b66c9** (Nachbesserung 1 auf
Abnahme 0). Abgenommen wird gegen den WORTLAUT in `AUFTRAG.md`. Alles unten ist an der gebauten exe dieses Baums selbst
gemessen (`re15_port/build/platform/pc/re15_pc.exe`, 6756748 B). Messlaeufe (unversioniert): `re15_port/build/abn1_mess/<lauf>/`
(gr.log = RE15_GRANATE_LOG, state.log, wf.log, debug.log, fd_*.ppm/png = RE15_FRAMEDUMP, cap.raw = RE15_AUDIO_CAP_SYNC),
Startskript `abn1_mess/lauf.sh <name> <timeout> VAR=...` (Kopie der exe unter eigenem Namen; `EXE=<pfad>` fuer die Vorher-exe),
Treiber `sweep.sh`, `reich.sh`, `ton.sh`. Eigene Pruefwerkzeuge (ebenfalls dort abgelegt): `abn1_pfad.py` (unabhaengiger
Strecken-Abtaster), `abn1_diff.py` + `abn1_harness.c` (Differenzlauf gegen die Port-Funktion), `abn1_strahl2.py`
(Strahl-Zensus), `abn1_sweep_auswertung.py`.
"Vorher" = exe des Hauptbaums `C:/workspace/git/reAi_v2/re15_port/build/platform/pc/re15_pc.exe` (2026-09-30, v0.8.21), nur
kopiert und gestartet, gleiche Assets.

Hinweis zur Herkunft: Die erste Abnahme 1 wurde vom Guthaben-Limit abgebrochen. Ihre Reste liegen in `abn1_mess/`
(`diff_harness.c`, `zensus_strahl.py`, `d_b/`, `suite.log`). Ich habe davon nur die Idee des Differenzlaufs uebernommen,
`diff_harness.c` gelesen und SELBST gegen die aktuelle `libre15_engine.a` neu gebaut (`abn1_harness.exe`); die Vergleichsgeometrie
ist meine eigene (`abn1_pfad.py`). Ihr ctest-Lauf laeuft noch (s. 0.).

## Urteil

**BESTANDEN.** Alle fuenf Punkte sind am gebauten Stand gemessen erfuellt, Suite 480/480 gruen (eigener Lauf), RE-Gate
(10 Adressen selbst disassembliert, alle stimmen), Pfad-Gate und Vertrag halten, Tests je Punkt vorhanden und messend.
Keine blockierenden Maengel. Sieben Hinweise (Abschnitt 9) fuer Nutzer/Orchestrator, darunter eine nicht gekennzeichnete
Teil-Auslassung des zitierten RE2-Mechanismus (Objektkaesten, H1) und ein Zusammenfuehrungskonflikt mit Spur B (H5).

| # | Punkt (Wortlaut) | Urteil | Kernbeleg |
|---|---|---|---|
| 1 | Granaten sollen nicht durch die Wand fliegen | **erfuellt** | vorher 4/11 Wuerfe durch Zellen (bis z -30996 ausserhalb des Raums), nachher 0/14; Sweep 72 Wuerfe / 9 Raeume: 0 Strecken durch eine solide Zelle |
| 2 | Range der Explosion viel zu niedrig, fast unmoeglich Gegner zu treffen | **erfuellt** (Hinweis H2) | 11 gleiche Wuerfe auf die ROOM1140-Gruppe: Treffer gesamt vorher 1, nachher 9 |
| 3 | Die Granate hat den falschen Explosionssound | **erfuellt** | gemischter Ausgang: Explosions-Segment korreliert 0,961 mit RE2 `ARMS09_00001.wav` |
| 4 | Mehr Brutalitaet beim Zombie (Zerplatzen, abplatzende Beine, Arme) | **erfuellt** (Hinweis H3) | 2 Zombies 0x10 zerrissen (Clip 2), ~1800 weggeschleudert, Glied im Bild F122, Leiche F180 |
| 5 | Tests bei Birkin und Alligator hinzufuegen | **erfuellt** | unit [5] 5 Bosse x 2 Abstaende x 2 KI; exe-Pins armierter G5 + Gator gruen; eigene Mehrfachtreffer 600->520->440->360 / 3000->2000->1000 |

## 0. Bau und Suite (selbst gefahren)

```
git status --short                               -> leer (Baum = HEAD a30b66c9)
bash re15_port/tools/local_build.sh configure    -> === LOCAL-BUILD-OK (configure)
bash re15_port/tools/local_build.sh build        -> ninja: no work to do. / === LOCAL-BUILD-OK (build)
bash re15_port/tools/local_build.sh test         -> 100% tests passed, 0 tests failed out of 480
                                                    Total Test time (real) = 1319.83 sec
                                                    === LOCAL-BUILD-OK (test) — Tests 480/480
ctest --test-dir re15_port/build -R r35_granate -V   -> 100% tests passed, 0 tests failed out of 2
   test_r35_granate: ALLE PRUEFUNGEN GRUEN
   -- r35_granate: Negativ-Kontrolle der Auswerter ok
   -- r35_granate [wand]: ok — Wand x=8385 -> 8332, Explosion Tick 349
   -- r35_granate [zombie]: ok — X=119, 2 Zombies zerrissen, 3 Eingriffe
   -- r35_granate [gator]: ok — 23 Slot 15: HP 3000 -> 2000, Zustand 2, exe bis EXIT_AT
   -- r35_granate [birkin]: ok — 36 Slot 2 armiert (g=13): HP 600 -> 520, danach Weg 3421, af 53 -> 93, exe bis EXIT_AT
   -- r35_granate [duenn_a]: ok — Wandpunkt x=-21603 -> Rueckzug -21107, kleinstes Granaten-x -21231
   -- r35_granate [duenn_b]: ok — Wandpunkt x=-21873 -> Rueckzug -21379, kleinstes Granaten-x -21503
   -- r35_granate [raute]: ok — Hand (-6720,-13266), 25 Flugbilder, Wandzeilen 1, Explosion Tick 1386
```
Im Suite-Lauf: `unit_r35_granate` Passed 0,09 s, `integration_r35_granate` Passed 160,8 s, `integration_r34_granaten` Passed
208,9 s, `unit_r34_wurf/_schaden/_reaktion` Passed. Kein Fenster-Haken rot. Protokoll:
Scratchpad `abn1_suite.log` (Summenzeile meines Prozesses), `abn1_mess/` fuer die Messlaeufe.

**Umgebung (kein Befund der Spur, fuer den Orchestrator):** Ein ctest-Prozess (PID 67360, gestartet 17:37:14, `--test-dir
re15_port/build`) des abgebrochenen ersten Abnahme-1-Laufs laeuft noch im SELBEN Bauverzeichnis und schreibt verschachtelt in
dasselbe `re15_port/build/local_build_ctest.log`. Die dort sichtbaren `***Timeout`-Zeilen (#370, #371, #374-#379) gehoeren zu
ihm (dieselben Tests stehen im selben Log mit `Passed`); mein Lauf (gestartet ~17:52) endete mit eigener Summenzeile 0 failed.
Ich habe ihn nicht beendet (nicht meine PID). Er belastet die Maschine und kann fremde Fenster-Haken flattern lassen.

## 1. Punkt 1 — "Granaten sollen nicht durch die Wand fliegen": ERFUELLT

### 1.1 Vorher/Nachher an derselben Aufstellung (ROOM1140, RE2-KI, 11 Wuerfe je exe, `reich.sh`)
Unabhaengiger Abtaster `abn1_pfad.py` (jede Strecke zwischen zwei Granatenbildern in 2-4-Einheiten-Schritten gegen die
soliden Flaechen der Band-0-Zellen, Formtabelle Typ 1-9):

| Lauf (Leon-Lage, Zielhoehe) | vorher v0.8.21: Strecken durch Zelle / tiefstes z | nachher a30b66c9 |
|---|---|---|
| r2mitte3k_M (-4311,-19289), MITTE | 8 (Zelle x[8350..12450] z[-20750..-19600], Liegen x 8612) | 0, `EV wand wpos=(8385,..) -> rueckzug (8332,..)` T=349 |
| r2mitte3k_U, HOCH | 77 | 0, Wand T=338 bei x 12252 |
| r3nah_M (-1676,-18070), MITTE | 9, Liegen bei **z -30996** (Raum-Zellen enden bei z -25600) | 0, Wand T=304 bei z -23605 |
| r5ost2_M (3500,-20300), MITTE | 62 | 0, Wand T=341 bei x -8614 |
| 7 weitere (TIEF/MITTE ohne Wand im Weg) | 0 | 0 |
Zusaetzlich nachher 0 in `gore_re2`, `gore_re15`, `ton_wurf`. Die Vorbedingung des Fixes (Durchflug) steht damit in meinem
eigenen Vorher-Protokoll; der Fix erklaert den Befund (Wandzeile genau dort, wo vorher die erste Strecke in die Zelle lief).

### 1.2 Wand-Sweep (unabhaengige Aufstellungen, `sweep.sh`): 9 Raeume x 4 Blickrichtungen x (MITTE, HOCH) = 72 Wuerfe
Tuer-Spawn je Raum aus `include/re15_room_spawns.h`, `RE15_PLAYER_POS=x,z,rot[,band]`, Default-KI (Auto-Ziel aktiv).
Auswertung `abn1_mess/sweep_auswertung.txt` (Bandfilter = Band des Werfers; ROOM1090 Band 1):
```
ROOM1010 8 Wuerfe, 8 Wand          ROOM1110 8 / 8 Wand
ROOM1030 8 / 8 Wand (5 Wurfbild)   ROOM1140 8 / 5 Wand, 3 liegen im Raum
ROOM1050 8 / 8 Wand                ROOM1190 8 / 4 Wand, 4 liegen im Raum
ROOM1070 8 / 8 Wand (4 Wurfbild)   ROOM1220 8 / 8 Wand
ROOM1090 8 / 8 Wand (4 Wurfbild)
{'laeufe': 72, 'wuerfe': 72, 'wand': 65, 'durch': 0, 'hand_durch': 13}
```
**0 von 72 Wuerfen** haben eine Bildstrecke durch eine solide Zelle. Die 13 "Wurfbild"-Faelle: Leon steht 468 vor einer
Wand (Tuer-Spawn, von der Kollision an die Wand geschoben) und die Hand liegt jenseits davon (z.B. ROOM1030 Leon
(-8200,-22303), Hand (-8346,-23304) in `x[-20144..4120] z[-24420..-22771]`) -> `EV wand ... wurfbild`, Explosion ueber Leon.
Das ist die dokumentierte PORT-WAHL (Dossier N1.5 Punkt 3, N1.6); kein Durchflug.

### 1.3 Die abgenommenen Pins selbst nachgerechnet (Laeufe meines Suite-Laufs, `tests/integration/r35_granate_wd/lauf_48d043de/`)
```
wand    WAND T=349 von=(8345,-19758) wpos=(8385,-19739): Strecke trifft typ1 x[8350..12450] bei (8353,-19754); Rueckzug frei; 68 Strecken, 0 durch
duenn_a WAND T=289 von=(-21231,-10656) wpos=(-21603,-10677): Strecke trifft typ1 x[-21825..-21550]; 11 Strecken, 0 durch
duenn_b WAND T=290 von=(-21503,-10677) wpos=(-21873,-10698): beide Bildpunkte AUSSERHALB, Strecke trifft die 275 dicke Zelle bei (-21551,-10679); 0 durch
raute   Leon y 0 (Band 0), 25 Flugbilder, Wand an typ1 band0 z <= -17095; die einzigen "Treffer" meines bandlosen Abtasters
        sind die Band-1-Zelle x[-24600..16400] z[-25000..-14100] flr=13 (andere Etage, fuer y 0 nicht zustaendig)
```

### 1.4 Geometrie des Wandtests: Differenzlauf gegen meine eigene Abtastung (`abn1_diff.py`)
`re15_granate_r35_strecke` (gebaut gegen `libre15_engine.a` vom 17:10 = HEAD) gegen meine abtastende Geometrie (Quadrant nach
FUN_8003b068 selbst disassembliert, s. 6., Band strikt, u0&1, Formen Typ 1-9) an Zufallsstrecken (Laengen 0..1300, je ein
Drittel an Zellkanten, an der Quadrantengrenze, frei) in **allen 206 Raeumen mit SCA**:
```
Seed 35001: Strecken 30900, Port blockiert 14804, gleich 30900, abweichend 0
Seed 777:   Strecken 82400, Port blockiert 39148, gleich 82399, abweichend 1 (Randfall, +-3 Einheiten kippen es), HART 0
```
Form-Stichprobe gegen den Port-Zwilling des RE1.5-Handlers selbst gelesen: Typ 4 `Q = -(r+D)(px-ex+r)/(W+r) < pz-zmax`
(re15_collision.c:481-483) = Dreieck mit rechtem Winkel bei (x+w, z+d); Typ 5 `D(px-X)/W < PZ-Z` (:530-532) = rechter Winkel
bei (x, z+d) — wie Dossier N1.1.

### 1.5 Strahl-Zensus: kann eine Granate einen Raum ohne Wandereignis verlassen? (`abn1_strahl2.py`)
Von der Tuer-Spawnstelle jedes Raums (`re15_room_spawns.h`, Band = -(y/0x708) wie `re15_collision_band_from_y`) 64
Richtungen bis zum Austritt aus dem Zellrechteck des Bandes (hoechstens MITTE-Weite 11929), Urteil = Port-Funktion:
```
Spawnstellen 204, Strahlen 13056, FREI ueber den Rand: 110 an 4 Spawnstellen
  ROOM3080/3081 (-25850,0,-25900) band 0: 6 Richtungen    ROOM6000/6001 (7186,0,-9289) band 0 [FLR-floor]: 49 Richtungen
```
* ROOM3080 "WAREHOUSE LIFT" hat nur 3 Zellen. Exe-Lauf `r3080_wurf`/`r3080_lauf`: Szene laeuft (`pst=1`, NPC t=42/t=40
  `Plc_dest` Sub-VM-WALK), Leon steht 260 Bilder fest, kein Wurf moeglich -> **nicht pruefbar** am Sprung-Stand.
* ROOM6000: der FLR-Ersatzspawn y 0 ist synthetisch; die Raumzellen liegen in Band 2 (x[-7867..22283]). Kein echter Fall.
* Erster Zensus mit der Band-Spalte von `tools/engine_tueren.txt` meldete 8 Raeume — Fehlalarm: diese Spalte ist fuer
  Hoehenraeume 0 (ROOM1170 Spawn y -7200 = Band 4 laut `re15_room_spawns.h`).

### 1.6 Gekennzeichnete Abweichungen (geprueft, im Dossier N1.5/N1.6 und im Code ausgeschrieben)
Zellhoehe (Tisch ROOM1140 = Wand), Wand-Maske u0&1 (PORT-WAHL), Wurfbild-Regel (PORT-WAHL), Strecke statt Punkt
(PORT-WAHL zur NUTZER-VORGABE). Nicht gekennzeichnet: Objektkaesten -> Hinweis H1.

## 2. Punkt 2 — Reichweite: ERFUELLT (Hinweis H2)

Realistische Wuerfe auf die Zombiegruppe ROOM1140 (Default RE2-KI, Auto-Ziel), je Aufstellung dieselben Eingaben fuer
vorher (r=500) und nachher (r=2000), `reich.sh`, Spalte = `eingriffe` der Resolver-Zeile:

| Aufstellung | TIEF vorher | TIEF nachher | MITTE vorher | MITTE nachher | HOCH v/n |
|---|---|---|---|---|---|
| r1 Briefing-Spawn (-7600,-17600), Gruppe ~6500 entfernt | 0 | 0 | 0 | 0 | — |
| r2 (-4311,-19289), ~3000 | 0 | **2** | 0 | 0 (Wand 8385) | 0 / 0 |
| r3 (-1676,-18070), ~1500 | 1 | **3** | 0 (durch die Wand, z -30996) | **2** (Wand z -23605 hinter der Gruppe) | — |
| r4 (6000,-20300) Blick -x, ~6000 | 0 | 0 | 0 | 0 | — |
| r5 (3500,-20300) Blick -x, ~3500 | 0 | **2** | 0 | 0 (Wand -8614) | — |
| **Summe** | 1 | 7 | 0 | 2 | 0 / 0 |
(Gesamt vorher 1 Treffer in 1 von 11 Wuerfen, nachher 9 Treffer in 4 von 11 Wuerfen.)
Trefferzylinder vorher 900 (500 + Kasten 400), nachher Quadrat +-2000 um P und P+900 (RE2 Op 47). Sonde
`unit_r35_granate` 120-136 im Suite-Lauf gruen (vorn 2300 ja / 2600 nein, hinten 1900/2100, seitlich +-2300/2600, Puffer-
Wachstum, kein Spielerzweig). Wandzuendung: P = Rueckzugspunkt — in allen Wand-Laeufen `rueckzug (x,y,z)` == `P=(x,y,z)`
(z.B. `rueckzug (8332,-8,-19764)` / `P=(8332,-8,-19764)`), RE2 `lh v0,52/54/56(v1)` -> sp+16/20/24 @0x80020cdc-fc selbst gelesen.
Am Wortlaut: die Explosions-Reichweite ist belegt vergroessert, und Gegner sind damit jetzt trefferbar (9 statt 1 Treffer).
Was bleibt, ist der Wurf selbst (H2).

## 3. Punkt 3 — Explosionssound: ERFUELLT

Gemischter Ausgang gemessen (`ton.sh`: Audio AN, `RE15_AUDIO_CAP_SYNC=cap.raw`, Aufstellung "wand", Lauf `ton_wurf` mit Wurf,
`ton_leer` ohne Feuerknopf, sonst gleich; 1470 Stereo-Frames je Bild):
```
Differenz-RMS je Bild:  356:486  357:9334 358:9673 ... 363:10397 (Max) ... 380:506 381:332 382:120 384:5 386:0   (~0,95 s)
Korrelation des Differenzsignals (Fenster Bild 350-400, auf 44100 Hz bei Abspielrate 23352 Hz = pitch 0x879 umgerechnet):
  ARMS09_00001.wav 0,961 (Versatz -> Bild 357,04)   ARMS09_00000 0,230   _00002 0,033   _00003 0,036   _00004 0,113
gr.log  T=349 EV se code=01110001 pos=(8332,-8,-19764)
wf.log  SE  re2fx code=0x01110001 -> ARMS0F Satz 10 / SE  zusatz ARMS0F satz=10   (kein "SE  esp code=0x04080001")
debug.log [se] Stimme: se=10 layer=0 vag=2 ... pitch=0x879 (23352 Hz) (+ layer=1)
```
Referenzdatei `info/re2leon/COMMON/SOUND/ARMS09_00001.wav` (RE2). RE2-Aufruf `lui a0,0x111 / ori a0,a0,0x1 / jal 0x8005ba28`
@0x80020d40-48 selbst disassembliert. In allen 7 Integrationslaeufen genau eine RE2-Explosionszeile, keine RE1.5-Zeile.

## 4. Punkt 4 — Brutalitaet: ERFUELLT (Hinweis H3)

Lauf `gore_re2` (Aufstellung `zombie`, RE2-KI, TIEF, `RE15_FRAMEDUMP=116-160/3`):
```
F118 [2 t=10 st=1 ... @(-1341,-19524)] hp=50   [3 t=10 st=1 ... @(-1521,-20948)] hp=80   [4 t=11 ...] hp=250
F119 [2 st=3 ss1=9 ...] hp=-150   [3 st=3 ss1=9 ...] hp=-120   [4 t=11 st=2 ss1=9] hp=50          (Explosion eingriffe=3)
F120 [2 st=3 ss2=1 mo=2 @(-1441,-19915)]  [3 st=3 ss2=1 mo=2 @(-1509,-21329)]                       (Clip 2 = re2z_death_rip)
F126 [2 @(-1803,-21213)]  [3 @(-1441,-22673)]                                                      (weggeschleudert ~1400-1800)
F180 [2 st=7 mo=22] hp=-1  [3 st=7 mo=22] hp=-1  [4 st=1] hp=50                                     (Leichen; 0x11 ueberlebt)
```
Bilder: `abn1_mess/gore_re2/fd_000122.png` (Blutwolke am Zombie, abgerissenes Glied fliegt durch den Feuerball),
`fd_000128.png` (Blutspritzer, zerrissener Koerper), `fd_000158.png` (zwei brennende Leichen am Boden). Sonde `gore`
(170-175) im Suite-Lauf gruen (6/6 Laeufe Spalte 1, Handler 0x80108BEC/0x80109610, Teile-Flags, keine Wiederbelebung).
Handler-Port `re2z_death_rip` (enemy_ai_re2_zombie.c:5757) mit Zeile-9-Ausstieg 2 @0x80108CF4-D40 gelesen.

## 5. Punkt 5 — Tests Birkin und Alligator: ERFUELLT

* `unit_r35_granate` [5] (180-189): Birkin 0x30, 0x36@3080, G5 0x36@5090 (armiert), Alligator 0x23, Gator-Boss@2090, je 300
  und 1500 vor dem Boss, beide KI-Flavors; misst Treffer, HP, Reaktion/Verlassen des Trefferzustands. Gruen.
* exe-Pins aus meinem Suite-Lauf (`lauf_48d043de/gator`, `/birkin`):
  ```
  gator  F59 [15 t=23 st=1] hp=3000 | F60 st=2 ss1=9 hp=2000 | F61 st=1 mo=10 | F80 @(-7112,-8313) | F99 @(-7210,-9703) mo=0
  birkin F399 [2 t=36 st=0 g=13 mo=1 af=52 @(-7523,-23400)] hp=600 | F400 st=2 hp=520 | F420 @(-4471) af=73 | F440 @(-3952) af=93
  ```
* Eigene Zusatzpruefung "kein Haenger, wiederholt treffbar" (Laeufe `boss_birkin3`, `boss_gator2`, mehrere `RE15_FORCE_EXPLOSION`):
  ```
  Birkin (armiert, sub04): F399 hp 600 | F400 520 | F440 440 | F480 360   (mo 1 -> 8 -> 5, Lage aendert sich)
  Gator  (ROOM2090 Slot 15): F59 3000 | F60 2000 (st 2) | F119 st 1 | F120 1000 (st 2) | F179 st 1, Lage z -9200 -> -12503
  ```
Ein echter Wurf an den Gator-Boss ist am Sprung-Stand nicht aufstellbar (Lauerstellung (-7200,-9200), Leon (-8432,-26532));
die Tests decken den Wortlaut ("Tests hinzufuegen") vollstaendig.

## 6. RE-Gate

**Selbst disassembliert und mit dem Zitat verglichen (alle stimmen):**
1. RE2 FUN_8004fba0 Formtest @0x8004fe34-54: `andi v1,v1,0xf / sltiu v0,v1,0xe / beq ..0x8004ffb0 / sll / lui at,0x8001 /
   addu / lw v0,4356(at) / jr v0`; Tabelle `read 0x80011104 14` = 0x8004FFB0, 0x8004FE5C, 0x8004FE7C, ... [10] 0x800500C8 ... wie N1.1.
2. RE2 Hoehenfenster @0x8004ffb0-0x8005000c: `srl v1,v1,11`, v1*25*4 = *100, `subu s0,s0,v0`, `lw v1,4(s3) / slt v0,s1,v1 / bne
   ..0x8005005c / slt v0,s0,v1 / beq ..0x80050028 / ori v0,v0,0x1 / sw v0,-13368(at)`.
3. RE2 y > 0 @0x8004fc48-58 (`blez v1 / addiu v0,zero,1 / sw v0,-13368(at)`), Objektkaesten @0x8004fcfc-18 (`ori v0,v0,0x1 /
   sw v0,-13368(at)`), Klassenmaske @0x8004fdc0-d0 (`lhu v1,8(s2) / lw t0,16(sp) / and v0,v1,t0 / beq`).
4. RE2 Aufruf @0x8001ee60-a0: `addiu a1,zero,2`, `addiu a2,zero,8192`, `addu a3,zero,zero`, `jal 0x8004fba0`.
5. RE2 step[2] @0x8001ef74 `lbu v0,2(v1) / j 0x8001f0e4`; Wand @0x8001ef84-8c `lw v0,-13368(v0) / beq v0,zero,0x8001f10c`;
   step[3] @0x8001f0e0 `lbu v0,3(v1) / lb v1,27(v1) / ... lw v0,-10136(at) / jalr v0`.
6. RE2 Op 47 @0x80020cdc-fc `lh v0,52/54/56(v1)` -> sp+16/20/24 (kein Versatz bis `addiu a0,sp,16` @0x80020d50); @0x80020d40-d98
   SE 0x01110001, Hitcode `lui a3,0x1002 / ori a3,a3,0x9`, `addiu v0,v0,900`.
7. RE2 Boxbytes `bytes 0x80010900 32`: `88 fa 00 00 5e 01 fa 00 ... 30 f8 00 00 e8 03 f4 01` = {-1400,0,350,250} / {-2000,0,1000,500}.
8. RE1.5 FUN_8003aea0 @0x8003af04-84: neun `sw` nach 0x800b285c..0x800b287c mit 0x8003bca8, 0x8003d00c, 0x8003d6a8, 0x8003beb0,
   0x8003c734, 0x8003cb9c, 0x8003c2cc, 0x8003d7e8, 0x8003d930 (= Typ 1-9 wie im Code-Kommentar).
9. RE1.5 FUN_8003b068 @0x8003b068-a0: z-Vorzeichen (Lage.z+Off - a3) Bit 31, x-Vorzeichen >> 1, `srl v0,v0,30` = Quadrant wie
   `quadrant()` in granate_r35.c:138-143.
10. RE1.5 Routine 31 @0x80018598-b8 (`ori a0,zero,0x1f4`, `addiu v0,v0,-500` @0x800185a8, `jal 0x80012d60`), Routine 30
    `ori v0,zero,0x17c` @0x80018494 / `ori v0,zero,0x118` @0x800184bc, Routine 29 `lh t1,42(t0) / blez` @0x80018330-38.

**Wortsuche im Diff** (`git diff master -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/
TODO/FIXME/getenv/approx: **0 Treffer**. Kein Env-Schalter als Abschluss (SDL_ASSERT=always_ignore nur im Test-Harness).
**Konstanten:** jede verhaltensrelevante Konstante in granate_r35.c / re15_granate_r35.h / fx_plattform_pc.h traegt @0x...
(1400/350/250 @0x80010900, 1000 @0x8001eec8, -2000/1000/500 @0x80010918, 900 @0x80020d98, 0x01110001 @0x80020d40, 500
@0x800185a8, Liegezustand 0x63/31/0/7 @0x80018368-84/@0x8001856c); PORT-WAHL/NUTZER-VORGABE gekennzeichnet: Maske 1u,
Strecke, Wurfbild-Regel, K0 + Zone 1 (re15_damage.c:4235-4248). `4096` in `strecke_kreis` und Q16 `65536` sind
Rechengenauigkeit, kein Verhalten.
**Erklaert der Fix den Befund?** Ja: vorher an derselben Aufstellung Durchflug (1.1, eigene Vorher-Messung), nachher die
Wandzeile an genau der Zelle, in die vorher die erste Strecke lief.

## 7. Vertrags-Gate

* Pfade (`git diff master --name-only`, 22 Dateien): kein `release/`, kein `platform/android/`, kein `shared_assets/PSX/`,
  keine Aenderung an `tests/unit/CMakeLists.txt` / `tests/integration/CMakeLists.txt`. Neue Dateien: `engine/src/granate_r35.c`,
  `include/re15_granate_r35.h`, `tests/unit/probes/r35_granate.cmake`, `tests/unit/test_r35_granate.c`,
  `tests/integration/test_r35_granate.cmake`.
* Gemeinsame Dateien: `engine/src/re15_esp.c` **+10/-7** (include, 1-Zeilen-Haken Routine 29, Zustellung Routine 31, SE,
  drei Dienst-Einzeiler) — Vertrag 1.4 eingehalten (Abnahme-0-Mangel 7 behoben). `platform/pc/main.c` +2/-1.
  `engine/src/re15_damage.c` +67/-5 (nicht in der Liste 1.4; `re15_re2_gl_kandidat` angehaengt, Stempel-Spalte).
* Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse belegt (Diff-Suche); keine neuen Assets (ARMS0F liegt unter
  shared_assets/PSX/SOUND).
* Tests je Punkt vorhanden und messend: Wand (unit [1]+[6], exe wand/duenn_a/duenn_b/raute, mit Negativkontrolle 109 "ohne
  Zellen fliegt sie weiter" und duenn_b "Bildpunkte beidseits ausserhalb"), Reichweite ([2]), Ton ([3] + exe wf.log), Gore
  ([4] + exe zombie), Bosse ([5] + exe gator/birkin armiert mit Reaktion). Auswerter mit Negativkontrolle
  (`r35_gegner` an erfundenen Zeilen).
* Zusammenfuehrung (Probe `git merge-tree --write-tree HEAD r35/werfer`): **Konflikte** in `platform/pc/src/audio_pc.c`
  (`ARMS_ZUSATZ_N 3` beide Spuren) und `platform/pc/src/fx_plattform_pc.c` (Log-Zeile der RE2-SE-Weiche) -> H5.

## 8. Maengel

Keine blockierenden Maengel.

## 9. Hinweise (nicht blockierend; fuer Nutzer und Orchestrator)

**H1 — Objektkaesten nicht umgesetzt und nicht als Abweichung gekennzeichnet.** RE2 FUN_8004fba0 setzt das Wand-Flag auch an
Objektkaesten (a3 = 0 @0x8001ee84; Schleife @0x8004fc5c-0x8004fd6c, Setzer @0x8004fd0c-18 — Dossier N1.1 liest das selbst).
`re15_granate_r35_wand` prueft nur SCA-Zellen (granate_r35.c:145-158, `strecke_liste` ueber `rdt->sca`); der Code-Kommentar
granate_r35.c:16-20 und OFFEN N1.5 nennen die Objektkaesten nicht. Der Spieler wird dagegen nach den Zellen auch aus festen
Obj_model_set-Requisiten geschoben (game_step_common.c:1636-1641, `re15_collision_objects`). Zensus (`scd_walk_lib`, alle RDTs):
672 Obj_model_set, **114 mit Kollisionskasten in 67 Raeumen**, davon 14 STAGE1-Raeume (1080, 1090, 10D0, 10F0, 1100, 1150,
1170, 1190 [4 Zielscheiben 90x1400, Band 1], 11A0, 11B0 [Kasten 3600x2160, Band 1], 11C0, 11E0, 11F0, 1230 [Kisten 1800^3]).
Gemessen am einzigen geprueften Fall ROOM1230 (Kiste @0x0cc0 bei (-19554,-23216)): Leon laeuft selbst bis x -19773 in den
Kastenbereich (`kiste_lauf`), die Granate fliegt dort ebenfalls hindurch (`kiste_tief`, Liegen (-19812,-22906)) — kein
Widerspruch Spieler/Granate am Sprung-Stand. Naechster Messweg: ein Raum, in dem die Requisite Leon nachweislich sperrt
(ROOM11B0 obj0 Band 1, ROOM11E0 obj2), Wurf hindurch; mindestens unter OFFEN als Abweichung von @0x8004fc5c-0x8004fd6c eintragen.

**H2 — MITTE/HOCH ueberfliegen die Gegner.** Die Wurfweite ist RE1.5 byte-true (MITTE ~12000 @0x800184bc, HOCH ~18760
@0x80018494). Mit Auto-Ziel auf einen Zombie in 1500-6500 traf MITTE nur, wenn eine Wand dicht hinter der Gruppe lag (r3: 2),
sonst 0 (r1, r2, r4, r5). Trefferfaehig ist TIEF (7 Treffer in 5 Wuerfen). Der RE2-Kontaktzuender ist bewusst nicht
verdrahtet (Dossier §4.2) und steht dem Nutzer als Frage vor (N1.6 Punkt 3) — dort sollte der Nutzer entscheiden.

**H3 — Unter der OPTIONS-KI "RE1.5" kein Zerreissen.** Lauf `gore_re15` (gleiche Aufstellung, `RE15_AI_FLAVOR=re15`): der
getroffene Zombie 0x10 nimmt Clip 13, bleibt liegen (Lage (-1800,-21600) unveraendert), F180 `st=7 mo=21`; Blutwolken sichtbar
(`gore_re15/fd_000125.png`), aber kein Wegschleudern/Zerreissen. Grund: `re2_gl_typ` gilt nur fuer RE2-KI-Typen
(re15_damage.c:4045-4046). Default ist RE2-KI, der Wortlaut ist damit im Standard erfuellt; fuer den RE1.5-Modus nicht.

**H4 — Wurfbild-Zuendung ist an Waenden haeufig.** 13 von 72 Sweep-Wuerfen von Tuer-Spawnstellen zuendeten im Wurfbild ueber
Leon (Leon 468 vor der Wand, Auto-Ziel zeigt durch die Wand, z.B. ROOM1030). Folge ohne Schaden fuer Leon (kein Spielerzweig),
in ROOM1030 trafen zwei dieser Zuendungen je einen Gegner (`sw_1030_3072_M`, `sw_1030_3072_MU`). Dokumentierte PORT-WAHL (N1.5 Punkt 3); der Nutzer sollte es wissen.

**H5 — Zusammenfuehrung mit Spur B (r35/werfer).** Beide Spuren bilden 0x01110001 -> ARMS0F Satz 10 ab, aber verschieden:
A in der Weiche (`re15_pc_re2fx_se_weiche` ok=1, `arms == RE15_PC_ARMS_EXPLOSIV`), B NACH der Weiche (ok=0-Zweig) mit
eigener `was`-Kette, deren Ok-Zweig nur ARMS10/ARMS11 kennt. Wird beim Aufloesen A's Weiche mit B's Log-Zeile kombiniert,
schreibt wf.log fuer die Granate "ARMS11 Satz 10" -> `integration_r35_granate` (Regex `SE  re2fx code=0x01110001 -> ARMS0F
Satz 10`) faellt. Beim Merge die Dreifach-Unterscheidung behalten und beide r35-Granaten-/Werfer-Tests danach fahren.

**H6 — Verwaister ctest** im Bauverzeichnis dieses Baums (s. 0.), PID 67360, Eltern-PID 47444.

**H7 — Explosion hinter Waenden nicht geprueft.** Die Box +-2000 (FUN_800470C0) kennt keine Sichtlinie; eine Wandzuendung
trifft auch Gegner jenseits einer duennen Wand innerhalb des Quadrats (Abnahme 0 / d_a-d_b: liegende Zombies in den
ROOM1220-Zellen lagen unter dem Band, `eingriffe=0`). Nicht Gegenstand des Wortlauts; nicht gemessen.

## 10. Messlaeufe (unversioniert, `re15_port/build/abn1_mess/`)
`sw_<raum>_<rot>_<M|MU>` (72, Sweep) · `rw_{neu,alt}_<aufstellung>_<D|M|U>` (22, Reichweite/Vorher-Nachher) ·
`gore_re2`, `gore_re15` (+ fd_*.png) · `ton_wurf`, `ton_leer` (cap.raw) · `boss_birkin3`, `boss_gator2` · `r3080_wurf`,
`r3080_lauf` · `kiste_lauf`, `kiste_tief`, `kiste_mitte` · `sweep_auswertung.txt` · Werkzeuge `abn1_*.py`, `abn1_harness.c`.
