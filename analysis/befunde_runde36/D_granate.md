# Runde 36 — Spur D: Granate (HE ohne Feuer an Zombies; Wurf-Aussetzer)

Stand: 2026-10-10, Zweig `r36/granate`, Baum `.claude/worktrees/r36_granate`, Basis master 5a9dbb7b (v0.8.23 + Auftrag),
zuletzt master 78a4a748 eingemischt (d9627833). Fortsetzung nach dem Sitzungslimit ab Commit e93f75b7 (§3.7, §5).
**Nachbesserung 1 (nach Abnahme 0): §6** — Granate wird nur noch im Spawnbild verbraucht (M1/M2).
Dossier wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz). Auftrag: `AUFTRAG.md` (woertlich), Vertrag: `VERTRAG.md`.
Vorrunde: `analysis/befunde_runde35/A_granate.md` (Wand/Reichweite/Sound/Brutalitaet), `B_werfer.md` (Granatwerfer).

## 0. Auftrag (Wortlaut des Nutzers)
1. "Bei normalen Granaten brauche ich keinen Feuer Effekt bei den Zombies, sondern nur das Zerplatzen der Körperteile.
   [Feuer] würde ich nur bei den incendiary Granaten erwarten."
2. "Manchmal selten löst die Granate bei der Verwendung nicht aus/wird nicht geworfen."

## 1. Ausgangslage (Stand master 5a9dbb7b, selbst nachgelesen)
* Handgranate Art 2 (Item 9), Saeure Art 3 (0x0A), Brand Art 4 (0x0B). Routinen 30/29/31 in re15_esp.c; Wand/Reichweite/
  Explosion in granate_r35.c (Runde 35 Spur A); Explosion stempelt RE2-Zeile 9, Klammer 0, Zone 1 -> DEATH[9][1] =
  0x80108BEC `re2z_death_rip` (Zerreissen) bzw. 0x80109610 (Wegschleudern).
* `re2z_death_rip` Phase 0 ruft fuer Zeile 9/17 ohne +0x10E&0x80 `re2z_gore_soot` = FUN_8010640C ("Spreng-Russ",
  `jal 0x8010640C` @0x80108CEC). Diese spawnt RE2-Effekt-Ids 0x05032710 (Part 0, v={0,200,0}, @0x80106418-44) und
  0x050313E8 (Part 3 R-Unterarm, @0x80106450-70) und setzt die Russ-Tinten (@0x8010648C-F8).
* Port-Dekoder `re2z_gore_fx_ex` (enemy_ai_re2_zombie.c:1107): RE2-Klasse 5 -> RE1.5-CORE00-Id 8 (= FEUER), "Klassen-
  Zuordnung, Sichtpruefung offen" (Runde 4, gore-vollausbau.md §2.1). Dieselbe Klasse 5 nutzt die Brand-Verkohlung
  FUN_80106128 (Zeile 10/16).

## 2. Punkt 1 — HE-Granate: Feuer an den Zombies

### 2.1 Messung vorher (exe 5a9dbb7b, Messkopie `re15_pc_r36d.exe`, Lauf `build/r36d_mess/he_zombie`)
Aufstellung = integration_r35_granate "zombie": ROOM1140 per RE15_DEBUG_JUMP=1140@250, Leon (-1676,-18070) Blick 1076,
RE2-KI, RE15_GIVE=9:5 RE15_EQUIP=9, Skript `MD0.6,MDA0.2,MD2.5,W5` (TIEF-Wurf), RE15_FX_LOG=fx.log, RE15_EXIT_AT=330.
```
gr.log:  F=43 SPAWN granate art=2 ... T=329 liegen ... T=365 EV resolver art=2 P=(-2199,-480,-20548) r=2000 eingriffe=3
state.log F119: [2 t=10 st=3 ss1=9 ss2=0 mo=2 @(-1334,-19530)] hp=-150  [3 t=10 st=3 ss1=9 ss2=0 ...] = Explosionsbild X=119
fx.log (gezeichnete Teilchen je Bild): ab F=120 bis F=330 (Ende) in JEDEM Bild 4 Zeilen `id=8 sub=3 ... A=18` =
  F120: w(-1277,-522,-19546) w(-848,-741,-18907)      (Zombie 2: Huefte y+200 / rechter Unterarm)
        w(-1452,-1640,-20946) w(-1323,-975,-20727)   (Zombie 3)
  Summe 844 id=8-Zeilen = 211 Bilder x 4 Flammen; sie erloeschen bis zum Ende nicht.
```
= je zerrissenem Zombie ZWEI Flammen (RE1.5-CORE00-Id 8 = FEUER, Sub 3, Routine A 18), ab dem Bild nach dem
Explosionsbild (= `re2z_death_rip` Phase 0, ss2 0 -> 1). Keine andere Feuerquelle im Log: die HE-Explosion selbst
spawnt nur die RE1.5-Kinder 0x03195000/0x030B5400/0x030B5800 (Id 3, re15_esp.c case 31) — im fx.log `3/25`, `3/11`.

### 2.2 RE-Beleg (RE2 EMOVL10_S0.BIN, selbst disassembliert `re2_disasm.py dis 0x8010640c 66 --bin EMOVL10_S0.BIN`)
FUN_8010640C ("Spreng-Russ", Zeile 9 HE / 17 Rakete):
```
80106418 lui a0,0x503 / 8010641c ori a0,a0,0x2710      a0 = 0x05032710 (Klasse 5 = RE2-CORE00-Id 5 FEUER, Sub 3, Skala 10000)
80106424 addiu v0,zero,200 / 8010642c-34 sh zero,16 / sh v0,18 / sh zero,20   a3 = (0,200,0)
80106438 lw v0,408(s0) / 8010643c lh a1,118(s0) / 80106440 jal 0x8001bf10 / 80106444 addiu a2,v0,72   Anker Part 0
80106450 lw v0,516(a2) / 80106458 andi v0,v0,0x1 / 8010645c beq v0,zero,0x80106478                 Part 3 vorhanden?
80106460 lui a0,0x503 / 80106464 ori a0,a0,0x13e8 / 8010646c addiu a2,a2,588 / 80106470 jal 0x8001bf10   0x050313E8 Part 3
80106478-801064f8  Russ-Tinten 0x404040/0x484848/0x707070/0x505050 nach +0x70 der Parts 0,1,2,5,9,10,11,12,13,14
```
Aufruf nur im Zerreiss-Tod: FUN_80108BEC Phase 0 `lbu v1,5(s3) / addiu v0,zero,9 / beq v1,v0,0x80108cd8` @0x80108cc0-c8,
`addiu v0,zero,17 / bne v1,v0,0x80108cf4` @0x80108ccc-d0, `lhu v0,270(s3) / andi 0x80 / bne` @0x80108cd8-e4,
`jal 0x8010640c` @0x80108cec. Ebenso die Leitern @0x801051E8-208 (HURT) / @0x801084A0-B8 (DEATH-Zwilling).
RE2-CORE00.ESP Id 5 (Dump `re2esp_dump.py`): Sub 3 = Skript @0x0788, Step 0 Op A 2 (Anim-Start 0 + rand%5), Status 0xB803
(Bit 0x800 = folgt der Anker-Matrix), Step 1 Op B 25; Anim 0..9 = die Flammenbilder (UV 0x00A8.. Seite 0x1E, CLUT 0x7911),
dieselben wie Sub 0/1 der Verkohlung FUN_80106128 (Brand, Zeile 10/16). Also: RE2 Retail laesst die HE-Leiche brennen.
Port-Dekoder `re2z_gore_fx_ex` (enemy_ai_re2_zombie.c:1118): Klasse 5 -> RE1.5-CORE00-Id 8 (FEUER) Sub 3 = die gemessenen
Teilchen. Der Dekoder-Pfad `re2z_gore_soot` (enemy_ai_re2_zombie.c:4820-4835) ist 1:1 FUN_8010640C.

### 2.3 NUTZER-VORGABE (Abweichung von RE2 Retail)
"Bei normalen Granaten KEIN Feuer-Effekt an den Zombies, nur das Zerplatzen; Feuer nur bei incendiary." Die FORM der
Abweichung: die beiden Flammen-Spawns von FUN_8010640C (@0x80106418-44 und @0x80106450-74) entfallen fuer die Zeilen 9/17
(HE-Handgranate 0x09 = Art 2, GL-Explosivrunde w15, Rakete w18 — alle "explosiv, nicht incendiary"); die Russ-Tinten
(@0x80106478-F8, die Schwaerzung der Teile) bleiben, ebenso das Zerreissen/Wegschleudern (0x80108BEC/0x80109610). Die
Brand-Verkohlung FUN_80106128 (Zeile 10 = Brandgranate 0x0B Art 4, Zeile 16 Flammenwerfer) bleibt unveraendert (Feuer).
Saeure (Zeile 11, FUN_80106310) unveraendert.

### 2.4 Umsetzung
* `engine/src/granate_r36.c` / `include/re15_granate_r36.h`: `re15_granate_r36_russ_flammen(zeile)` = 0 fuer jede Zeile,
  die den Spreng-Russ erreicht (9/17; `beq v1,9` @0x80108cc8 / `bne v1,17` @0x80108cd0), Zaehler fuer die Sonde.
* `engine/src/enemy_ai_re2_zombie.c` `re2z_gore_soot` (= FUN_8010640C): die beiden Spawns (@0x80106418-44 / @0x80106450-74)
  stehen hinter dem Gate; Tinten @0x80106478-f8 unveraendert. Kein Env-Schalter, keine Konstante nach Gefuehl.

### 2.5 Messung nachher (exe e289dd0e+, Lauf `he_zombie_n`, dieselbe Aufstellung wie 2.1)
`eingriffe=3`, Explosion F119, **0 Zeilen `id=8`** im fx.log (vorher 844), Zombies 2/3 zerrissen (F120 ss1=9 ss2=1 mo=2,
Lage F120 (-1441,-19915) -> F121 (-1535,-20252)), Blut/Brocken wie vorher (`0/0` 913 Zeilen, `7/2` 153). Gegenprobe
Brandgranate (`brand_zombie_n`, Item 0x0B): Explosion F119 + `aufschlag re2_art=1`, **`id=8` 9 je Bild ab F120** bis zum
Ende (3 Zombies x Verkohlung FUN_80106128 Part 0/Kopf/Unterarm) — Feuer bei incendiary unveraendert.

## 3. Punkt 2 — "Manchmal selten loest die Granate nicht aus / wird nicht geworfen"

### 3.1 Messung vorher (exe 5a9dbb7b, Messkopie, Laeufe `build/r36d_mess/serie*`, `r1_sweep`)
* **Wurfserien, R1 durchgehend gehalten** (Skript `M0.6` + 12x `MA0.2,M1.4`, RE15_GIVE=9:30):
  ROOM1220 (-19533,-9700) Blick 2048, MITTE/HOCH/TIEF: je **12 SPAWN / 12 Explosionen** = 12 von 12 Abzuegen.
  ROOM1140 (-4311,-19289) RE2-KI MITTE/HOCH: nur 6 von 12 — Ursache im state.log: F179..F283 Griff/Biss der
  RE2-Zombies (mo 0/1, hp 100 -> 80 -> 60, Lage springt): die Ziel-FSM wird unterbrochen (re15_player_aim_interrupt),
  Abzuege waehrend des Griffs verfallen — Aufstellungs-Artefakt, kein Wurf-Defekt. TIEF 12/12 (die Zombies wurden
  vorher zerrissen).
* **R1-Freigabe nach dem Abzug** (`r1_sweep`, ROOM1220, je Block `M0.5,MA0.2,M<h>,W1.5`, h = 0.0 .. 0.7 s):
  ```
  Abzuege (pad 8800-Flanke, Zustand HOLD clip 8): F16, F82, F151, F223, F298, F376, F457, F541   (8 Abzuege)
  SPAWN granate:                                   F479 (= 457+22, h=0.6), F563 (= 541+22, h=0.7)  (2 Wuerfe)
  Granaten im Inventar (state.log mg):             30 -> 22  (8 abgezogen)
  ```
  **Befund:** wird R1 innerhalb von 21 Bildern (0,7 s) nach dem Abzug losgelassen (MITTE; HOCH 18, TIEF 23 Bilder),
  bricht der Rueckstoss ab Bild 11 in LOWER ab, die Granate wird NICHT geworfen — aber sie ist WEG (Abzug im
  Rueckstossbild 0). 6 von 8 Abzuegen ohne Wurf, 6 Granaten verloren. Das ist der Aussetzer: "loest nicht aus"
  (Leon setzt zum Wurf an und senkt den Arm) und "wird nicht geworfen".
* Weitere Kandidaten gemessen/gelesen: Clip-Laengen W09 7/9/11 = 35/40/40 Bilder (> 24, kein Abschneiden);
  Pool voll: nach der HE-Explosion an 2 Zombies steigt die Belegung F120..F130 auf **95 von 96** Plaetzen
  (state.log fx=, Lauf he_zombie) und faellt bis F141 auf 8 — ein Wurf, dessen Spawnbild (Abzug + 22) in dieses
  Fenster faellt, ginge ebenfalls verloren (FUN_80019700 `ori v0,zero,0xff` @0x800197c8-cc; s. 3.4).

### 3.2 RE-Beleg (RE1.5 PSX.EXE, selbst disassembliert `re15_disasm.py dis 0x80033600 42`)
Standard-Feuer-Handler Substate 2 (LAB_80033460), jedes Bild:
```
80033600 andi v0,a0,0x100 / 80033604 bne v0,zero,0x80033658     R1 (0x100) gehalten -> weiter zum Anim-Tick
8003360c-18 lbu v0,0x800aca5d (Waffe) / lbu a0,0x800acae9 (Cursor)
8003361c-30 addiu v0,v0,-1 / sll v1,v0,2 / addu v1,v1,v0 / addiu at,at,16530 (0x80074092) / addu at,at,v1
80033634 lbu v0,0(at)        Byte 2 des Waffensatzes: Waffe 9 @0x800740b8 = `18 30 0a 01 00` -> 0x0a = 10
8003363c sltu v0,v0,a0 / 80033640 beq v0,zero,0x80033658         Cursor <= 10 -> weiter
80033644 ori v0,zero,0x3 / 8003364c sh v0,0x800aca5a             Cursor > 10 -> Substate 3 (LOWER)
80033650 j 0x800337ac                                             RETURN — der Granaten-Spawn @0x80033680-7a8 wird uebersprungen
80033668 jal 0x8001f314 (Anim +1) ... 80033684-8c Waffe == 9? / 80033690 ori v0,zero,0x13 (19) / 800336a4 0x16 (22) /
80033758 0x18 (24): Spawn 0x040D1000 @0x800336bc-ec / @0x8003371c-48 / @0x80033778-a4
```
Der Munitionsabzug liegt im ERSTEN Rueckstossbild: `jalr v0` @0x800334e0 -> Entlade-Handler 0x80033B38 = `jal 0x8004eae4`
@0x80033b40 (nur Abzug, Dossier analysis/waffen_fsm_2026-09-12/entlade-granaten-burst.md §1). Das Original verliert
also genau wie der Port die Granate, wenn R1 zwischen Cursor 11 und 18/21/23 losgelassen wird — der Port ist hier
byte-true (player_common.c:996-1016 `recoil_break[9-1] = 10`, game_step_common.c:2007-2021 Spawn bei 19/22/24).
Einordnung Beta -> Retail: die Rueckstoss-Abbruch-Regel (Byte 2 je Waffe, 7/10) ist fuer SCHUSSWAFFEN gebaut — deren
Schuss faellt im Bild 0 (Entlade: Resolve + Munition), der Abbruch ab Bild 11 kostet nichts. Die Handgranate haengt
ihren Wurfkoerper an das Loslass-Bild der Wurfanimation (19/22/24), behaelt aber Abzug im Bild 0 und Abbruch ab 11:
eine unfertige Kombination der Beta (die Granate ist eine RE1.5-eigene Waffe ohne RE2-Gegenstueck). Das Retail-
Vorbild fuer "Geschoss aus einer Waffe mit Rueckstoss" ist der RE2-Granatwerfer: die Runde verlaesst den Lauf im
Rueckstossbild 1 (werfer_r35.c, RE2 Tabelle @0x800A6FDC, Aufruf je Bild @0x800431ac-cc) — ein Abzug ist dort IMMER
ein Schuss, R1 kann den Abgang nicht mehr verhindern.

### 3.3 NUTZER-VORGABE / PORT-WAHL
Jeder Abzug der Handgranate (Ids 9/10/11) ist ein Wurf: der Rueckstoss-Abbruch (`Cursor > Byte 2` @0x8003363c)
greift fuer die Granate erst ab dem Spawnbild der aktuellen Zielhoehe (19 HOCH @0x80033690 / 22 MITTE @0x800336a4 /
24 TIEF @0x80033758) statt ab 10 (@0x800740ba) — die Form der Regel (Schwelle je Waffe, LOWER sobald Cursor > Schwelle)
bleibt die des Originals, nur die Schwelle der Granate wird an ihr "Schuss-Bild" gebunden, wie bei den Schusswaffen
(Schuss Bild 0 < Schwelle 10). Nach dem Spawnbild senkt R1-Loslassen den Arm wie bisher.

### 3.4 Zweite Ursache: voller Effektpool im Spawnbild (PORT-WAHL)
FUN_80019700 sucht die Plaetze 0..95 (`sltiu` @0x8001978c) und liefert bei vollem Pool 0xFF (`ori v0,zero,0xff`
@0x800197c8-cc) — Routine 30 laeuft dann nie, die Granate ist weg (Abzug im Bild 0), kein Wurf. Gemessen he_zombie
(state.log `fx=`): F119 Explosion an 2 Zombies, F120 54, F124 72, F128 88, **F130 95 von 96**, F141 8. Drei zerrissene
Zombies fuellen den Pool; ein Spawnbild (Abzug + 19/22/24) in diesem Fenster verliert die Granate — der zweite Wurf
unmittelbar nach dem ersten ("selten"). Der RE2-Gore-Schub (Blut 0/0, Brocken 5/7) auf dem RE1.5-Pool ist eine
Port-Konstruktion, das Original kennt diese Last nicht. PORT-WAHL: der Wurfkoerper (Spielobjekt) verdraengt das
Blut-Teilchen mit dem kleinsten Index (Effekt-Id 0, die Mehrzahl des Schubs; der Spawner fuellt ab Index 0, also das
aelteste); keine Granate, kein Nicht-Blut-Effekt wird verdraengt, ohne Blut bleibt das Original-Verhalten.
(Versuch `pool_voll`: zweiter Abzug im Bild 99 mit Spawnbild 123 — dort fiel der Wurf stattdessen dem GRIFF des
Zombies zum Opfer (F119 Leon rot 1079 -> 507, mo 0/1, Ziel-FSM unterbrochen): Abzug waehrend eines Griffs verfaellt,
byte-true. Der Pool-Fall ist deshalb in der Sonde [3] nachgestellt, nicht an der exe.)

### 3.5 Umsetzung (Dateien, Konstanten)
| Datei | Aenderung | Beleg |
|---|---|---|
| `engine/src/granate_r36.c` + `include/re15_granate_r36.h` | `re15_granate_r36_russ_flammen` (Punkt 1, Gate = 0), `re15_granate_r36_spawnbild` (19/22/24), `re15_granate_r36_recoil_break(id, elev)` (9/10/11 -> Spawnbild, sonst -1), `re15_granate_r36_platz_freimachen` (Blut Id 0, kleinster Index), Zaehler fuer Sonden | Spawnbilder @0x80033690/@0x800336a4/@0x80033758; Abbruch `sltu` @0x8003363c, Byte 2 @0x800740ba; Flammen @0x80106418-44/@0x80106450-74; Pool 0xFF @0x800197c8-cc |
| `engine/src/enemy_ai_re2_zombie.c` | include + 2 Zeilen in `re2z_gore_soot` (Gate um die beiden Flammen-Spawns) | FUN_8010640C |
| `engine/src/player_common.c` | 4 Zeilen hinter dem Werfer-Override: `rb_thr` der Granate = `re15_granate_r36_recoil_break(eq_w, s_aim_elev)` | @0x8003360c-50 |
| `engine/src/re15_esp.c` + `include/re15_esp.h` | `re15_esp_fx_free_slot(i)` (3 Zeilen) + 1 Zeile in `re15_esp_granate_spawn` (Pool voll -> raeumen) | @0x800197c8-cc |
| `tests/unit/probe_r34_wurf.c` | PRUEF 109: R1 los im Bild 15 -> jetzt Spawn 22 (vorher keiner), Menge 4 | verhaltensbedingt (Punkt 2) |
Keine neuen Assets, kein Patch an shared_assets, keine Bits/IDs/Ereignisse, kein Env-Schalter.

### 3.6 Messung nachher (exe ab dd88956b bzw. 2cb21596, Messkopie, gleiche Aufstellungen)
* `r1_sweep_n` (ROOM1220, 8 Abzuege, R1 nach 0,0..0,7 s los): **8 SPAWN (F38, 104, 173, 245, 320, 398, 479, 563) =
  Abzug + 22, 8 Explosionen, Granaten 30 -> 22** (vorher 2 Wuerfe, 30 -> 22).
* `he_zombie_n` (ROOM1140 TIEF an die Zombies, RE15_FX_LOG): Explosion F119 `eingriffe=3`, **0 Zeilen `id=8`** (vorher
  844 = 4 je Bild), Zerreissen unveraendert (ss1=9, mo=2, Lage F120 -> F121 um > 300), Blut/Brocken wie vorher (0/0 913,
  7/2 153 Zeilen).
* `brand_zombie_n` (Brandgranate 0x0B, gleiche Aufstellung): Explosion F119, `aufschlag re2_art=1`, **id=8 ab F120 9 je
  Bild** (3 Zombies x Verkohlung Part 0/Kopf/Unterarm) bis zum Ende — Feuer bei incendiary unveraendert.

(Nachtrag 2.2: die drei Aufrufer von FUN_8010640C selbst disassembliert — Zerreiss-Tod `jal 0x8010640c` @0x80108cec,
DEATH-Leiter `beq v1,9` @0x80108490 / `bne v1,17` @0x80108498 / `lhu 270 / andi 0x80 / bne` @0x801084a0-ac /
`jal 0x8010640c` @0x801084b4, HURT-Leiter `lhu 270 / andi 0x80 / bne` @0x801051f0-fc / `jal 0x8010640c` @0x80105204.
Alle drei laufen ueber dasselbe Gate im Port (re2z_gore_soot).)

### 3.7 Fortsetzung nach dem Sitzungslimit (2026-10-10 nachmittags) — Sonden 207/208 geprueft
Der ungeprueft committete Zwischenstand 41d1b0f7 (+33 Zeilen test_r36_granate.c) kompiliert; Pruefung 207 (letzte
Granate im Stapel, Menge 1: Spawn 22, Menge 0; zweiter Abzug ohne Granate = kein Wurf, Menge bleibt 0) ist gruen.
Pruefung 208 (Quadrat schon waehrend des Hebens gedrueckt und gehalten) war FALSCH GEMESSEN, nicht der Code:
`re15_player_aim_ready()` wird im selben Spielschritt vom Abzug ueberdeckt (READY -> Rueckstoss im selben Bild),
und das gehaltene Quadrat zieht nach dem Ende des Rueckstoss-Clips byte-true ERNEUT ab (Menge 3 statt der
erwarteten 4 nach 60 Bildern). Beleg, selbst disassembliert (`re15_disasm.py dis 0x800332e0 24`):
```
80033300 lw v0,-14488(v0)      0x800ac768 = das GEHALTENE Pad-Wort (DAT_800ac768; das Flanken-Wort 0x800ac76c
80033308 andi v0,v0,0x40        liest erst der Leer-Magazin-Zweig @0x80033338)
8003330c beq v0,zero,0x80033394  kein Abzug -> weiter im HOLD
80033314 jal 0x8004ea6c         Magazin > 0 ?
80033320-28 ori v0,zero,0x2 / sh v0,0x800aca5a   Substate 2 (Rueckstoss)
```
Rueckstoss-Clips der Granate: PL00W09.PLW EDD-Tabelle (Datei-Bytes) Clip 7 @0x24 = 0x23 (35 Bilder, MITTE),
Clip 9 @0x2c = 0x28 (40, HOCH), Clip 11 @0x34 = 0x28 (40, TIEF); Clip 6 @0x20 = 0x0a (Heben, 10). Der Rueckstoss
endet bei Bild fc-1 (player_common.c:1200), HOLD und Held-Abzug fallen ins selbe Bild -> Abzug k+1 = Abzug k + 34.
Neue Messung der Sonde (Abzug-Bilder ueber die Menge, Spawn-Bilder ueber die Granaten-Plaetze):
`READY Bild 11, 3 Abzuege (11,45,79) / 3 Spawns (33,67,101), Menge 2` = Abzug 1 im ersten HOLD-Bild, jeder Spawn
= Abzug + 22 (@0x800336a4), Abzug k+1 = Abzug k + 34, Menge 5 - 3. N Abzuege = N Granaten gilt also auch fuer den
gehaltenen Abzug (Dauer-Wurf).
Die Port-Aufrufer von `re2z_gore_soot` (alle laufen durch das Gate IN der Funktion): enemy_ai_re2_zombie.c:4883
(HURT/DEATH-Leitern @0x801051E8-208 / @0x801084A0-B8), :5824 (Zerreissen `jal 0x8010640C` @0x80108CEC), :7513
(@0x80108B58-88), :7589 (@0x80109c2c-50, Wegschleudern), :7640 (@0x80108790-C0) — fuenf Stellen, kein Aufruf
ausserhalb dieser Datei (`grep 0x0503\|gore_soot` ueber engine/src). Die Brand-Verkohlung :4771-4786
(FUN_80106128, Ids 0x05032710/0x05031388/0x050313E8 an Part 0/Kopf/Unterarm) ist davon unberuehrt.

## 4. Tests
* `unit_r36_granate` (tests/unit/test_r36_granate.c, probes/r36_granate.cmake):
  [1] feuer 103-108 — 4 Laeufe HE: Flammen-Slots (Id 8) nach dem Tod = 0 (vorher 2 je Zombie), Spreng-Russ erreicht
  und unterdrueckt (Zaehler), Zerreissen (Handler 5/6, Teile 0x4A, FX im Pool) und Russ-Tinte 0x404040 bleiben; 4 Laeufe
  Brand: Flammen-Slots > 0 (min 3); Gate Zeile 9/17 = 0.
  [2] wurf 201-206 — 96 Wuerfe bei 96 Abzuegen (R1 los in Bild 0..30 nach dem Abzug, HOCH/MITTE/TIEF), Spawnbild
  19/22/24, je Abzug genau eine Granate, LOWER im Bild nach dem Abgang bzw. nach dem Loslassen, Schwellen-Tabelle,
  Negativ-Kontrolle Browning (Schwelle 7, LOWER Bild 9 unveraendert). (Nachbesserung 1: 209 Verbrauch im Spawnbild,
  208 umgestellt, [4] griff 400-404 — §6.6.)
  [2c] 207 — letzte Granate (Menge 1): Spawn 22, Menge 0, danach kein Wurf ohne Granate (Menge bleibt 0).
  [2d] 208 — Quadrat waehrend des Hebens gehalten (Held-Bit @0x80033300-0c): Abzug 1 = erstes HOLD-Bild, Spawn =
  Abzug + 22, Abzug k+1 = Abzug k + 34 (Clip 7 = 35 Bilder, PLW @0x24), Menge = 5 - Abzuege (3 Abzuege / 3 Spawns).
  [3] pool 300-304 — Pool 96/96 im Spawnbild: Granate spawnt im Bild 22, ein Blut-Teilchen geraeumt, Menge -1, Pool nie
  > 96; Negativ-Kontrolle ohne Blut = kein Platz (Original 0xFF).
  Ergebnis: `test_r36_granate: ALLE PRUEFUNGEN GRUEN` (103-108, 201-208, 300-304).
* `integration_r36_granate` (tests/integration/test_r36_granate.cmake, echte exe): he (X=119, 2 Zombies zerrissen,
  Flammen nach X: 0), brand (X=119, Eingriffe 3, Flammen nach X: 1899), serie (8 Abzuege = 8 Wuerfe = 8 Explosionen,
  Granaten 30 -> 22). Eigener Lauf `build/r36d_mess/int_run1.log`: alle drei ok.
* Angepasst (verhaltensbedingt): `probe_r34_wurf.c` PRUEF 109 (R1 los im Bild 15 -> Spawn 22 statt keiner, Menge 4);
  `probe_r34_wurf: ALLE PRUEFUNGEN GRUEN`.

## OFFEN
1. ~~Griff/Treffer waehrend der Wurfbewegung~~ — **ERLEDIGT in Nachbesserung 1 (§6)**: die Granate wird erst im
   Spawnbild verbraucht; ein Griff/Treffer vor dem Abgang bricht den Wurf ab (Original @0x80113b00/@0x80113b30), die
   Granate bleibt im Inventar (Lauf c2: vorher Menge 5 -> 3, nachher 5 -> 4). Keine Wahlfrage mehr.
2. **Russ-Tinten bleiben** (@0x80106478-f8: Teile dunkelgrau nach der HE-Explosion) — nach dem Wortlaut "nur das
   Zerplatzen" koennte der Nutzer auch die Schwaerzung nicht wollen; sie ist kein Feuer-Effekt, deshalb belassen.
3. **Flammen-Lebensdauer der Brandgranate**: RE1.5-CORE00-Id 8 Sub 3 (Routine 17/18) brennt bis zum Raumwechsel; RE2
   Id 5 Sub 3 fuehrt Op B 25 (Lebensdauer/Verhalten nicht gelesen). "Feuer wie bisher" laut Auftrag — nicht angefasst.
   Naechster Messweg: re2_fx.c Op 25 lesen, DuckStation-Brandrunde an einem Zombie zaehlen.
4. **Wurfbild-Regel** (Runde 35, PORT-WAHL): Hand in/hinter einer Zelle -> Explosion ueber dem Werfer im Wurfbild. Ist ein
   Wurf mit Explosion, kein Aussetzer; unveraendert.
5. ~~Statusschirm waehrend der Wurfbewegung~~ — **durch Nachbesserung 1 mit erledigt**, und KORREKTUR der alten
   Annahme ("ohne Waffenwechsel laeuft der Rueckstoss nach dem Schliessen weiter"): das Inventar-Ende setzt das
   Kommandowort auf 0 (Task 0 -> Zustand 3, `sb zero,0x800aca58` @0x8001cbdc; Port `menu_stage3_cmd_zero` ->
   `re15_player_cmd_zero`, menu_common.c:1470-1480, seit Runde 30 N1) und beendet damit JEDE laufende Zielaktion —
   Statusschirm auf/zu zwischen Abzug und Spawnbild bricht den Wurf ab, auch ohne Waffenwechsel (im Original und bis
   HEAD: Granate weg). Jetzt: kein Abgang, kein Verbrauch (Sonde 405: Inventar-Ende im Bild 10 -> kein Spawn,
   Granaten 5; Sonde 403: Waffenwechsel im Bild 10 -> Spawn-Gate `id == 9` @0x8003368c scheitert, Granaten 5). Am
   Statusschirm der exe selbst nicht gefahren (START innerhalb 0,4 s nach dem Abzug); die Sonden rufen dieselben
   Port-Funktionen (Inventar-Ende bzw. Waffen-Setter mit Platz-Sync).

## Fuer den Nutzer
* Keine neuen Assets (Paket-Gate), keine Sprachdateien, keine Aenderung an shared_assets.
* **Handgranate (und Granatwerfer-Explosivrunde/Rakete):** Zombies zerplatzen wie bisher (Kopf/Beine ab, Wegschleudern),
  brennen aber nicht mehr; die Teile sind weiterhin russ-geschwaerzt. **Brandgranate:** Feuer wie bisher. Saeure unveraendert.
* **Wurf:** jeder Druck auf den Abzug wirft jetzt eine Granate — auch wenn R1 (Zielen) sofort nach dem Abzug losgelassen
  wird. Vorher ging die Granate verloren, wenn R1 innerhalb von ~0,6-0,8 s nach dem Abzug losgelassen wurde (Leon setzte an
  und senkte den Arm; die Granate fehlte im Inventar). Nach dem Abgang senkt R1-Loslassen den Arm wie bisher.
  Zusaetzlich geht eine Granate nicht mehr verloren, wenn der Effektpool im Wurfbild voll ist (viel Gore nach der
  vorigen Explosion).
* **Wurf abgebrochen = Granate bleibt (Nachbesserung 1):** eine Granate wird erst verbraucht, wenn sie die Hand
  verlaesst (Leon wie Elza). Wird die Figur waehrend der Wurfbewegung (ca. 0,7 s) gepackt oder getroffen, oder wird in
  dieser Zeit der Statusschirm geoeffnet, bricht der Wurf ab und die Granate bleibt im Inventar (vorher war sie in diesem
  Fall weg). Die angezeigte Menge sinkt deshalb erst im Moment des Abwurfs, nicht schon beim Druecken des Abzugs.
* Entscheidungsfragen: keine.
* Hinweis fuer das Zusammenfuehren mit Spur I ("Handgranaten bei 0 aus dem Inventar entfernen"): die Menge erreicht 0
  jetzt erst im Spawnbild, also NACH dem Abgang — das Entfernen kann den letzten Wurf nicht mehr verhindern (vorher fiel
  die Menge im Abzugsbild; legte das Entfernen dabei auch die Waffe ab, koennte das Spawn-Gate `id == 9` @0x8003368c
  im Spawnbild scheitern — Spur I ist hier nicht gelesen, nur die Reihenfolge ist belegt).
* Bedienung unveraendert: Granate ueber Item-Debug (SELECT + R1 im Statusschirm) oder Fund; Zielhoehe hoch/mitte/tief.

## 6. Nachbesserung 1 (nach Abnahme 0, `D_abnahme_0.md`; Beginn 2026-10-10 abends, HEAD 95d15ec7)

Maengel: **M1** "Granate verbraucht, aber nicht geworfen" bleibt, sobald Leon in den 19-24 Bildern der
Wurfbewegung gepackt/getroffen wird (Abnahme-Lauf c2 ROOM1140 P1: 2 Abzuege, 1 Wurf, Menge 5 -> 3).
**M2** keine Wahlfrage an den Nutzer — ORCHESTRATOR-ENTSCHEIDUNG gemaess Nutzer-Wortlaut ("wird nicht geworfen"):
eine Granate wird NUR verbraucht, wenn sie wirklich geworfen wird (Spawn); bricht ein Griff/Treffer die
Wurfbewegung vor dem Spawnbild ab, bleibt sie im Inventar. **M3** Hinweis (Abnahme 0 unvollstaendig).

| Mangel | Ursache | Messung vorher | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|---|
| M1 | Verbrauch im Rueckstossbild 0, Abgang erst 19/22/24; Griff/Treffer dazwischen beendet die Ziel-FSM | Lauf c2 (6.3): Abzuege F19/F105, Griff F113, 1 Wurf, Menge 5 -> 3 | `jal 0x8004eae4` @0x80033b40 (Bild 0, `jalr` @0x800334e0), `jal 0x80019700` @0x800336ec/@0x80033748/@0x800337a4, Griff @0x80113b00/@0x80113b30 | Verbrauch ins Spawnbild hinter die Platzvergabe (6.2/6.4) | Lauf c2 (6.5): 1 Wurf, Menge 5 -> 4, Verbrauch F43 = Spawnbild; Pin `griff` (gegen alte exe rot) |
| M2 | Dossier legte den Griff-Fall als Wahlfrage vor | OFFEN 1 + "Fuer den Nutzer" (alt) | Regel "Original oder nicht"; Retail-GL Munition @0x80042d3c -> Geschoss Bild 1 @0x80044b98-a0 | ORCHESTRATOR-ENTSCHEIDUNG umgesetzt, Wahlfrage gestrichen | Dossier ohne Wahlfrage; OFFEN 1/5 erledigt |
| M3 | Abnahme 0 beim Sitzungslimit abgebrochen | — | — | vollstaendige gezielte Testliste der beruehrten Module (6.7) | 26/26 + 2/2 gruen |

### 6.1 RE-Belege (selbst disassembliert, 2026-10-10 abends)
**RE1.5 PSX.EXE — Verbrauchsstelle = Rueckstossbild 0** (`re15_disasm.py dis 0x80033460 50`):
```
80033470 lbu v0,0(v1)  [0x800aca5b]    / 80033478 bne v0,zero,0x80033508   nur beim ERSTEN Durchlauf von Substate 2
80033480 sb v0(=1),0(v1)               aca5b := 1
8003349c sb zero,0x800acae9            Bildzaehler := 0
800334cc lui at,0x8007 / 800334d0 addiu at,at,16640 (0x80074100) / 800334d4 addu at,at,v1(=Waffe*4) / 800334d8 lw v0,0(at)
800334e0 jalr v0                       Entlade-Handler je Waffe
80033b38 addiu sp,sp,-24 / 80033b3c sw ra / 80033b40 jal 0x8004eae4 / ... / 80033b50 jr ra   Granate 9: NUR Mengen-Abzug
```
**RE1.5 PSX.EXE — Spawn (Abgang) = Rueckstossbild 19/22/24** (`re15_disasm.py dis 0x80033680 76`):
```
80033684 lbu v1,0x800aca5d / 80033688 ori v0,zero,0x9 / 8003368c bne v1,v0,0x800337ac   Spawn-Gate Waffe == 9
80033690 ori v0,zero,0x13 / 800336a0 bne v1(=acae9),v0 / 800336b4 andi v0,v0,0x8000 / 800336ec jal 0x80019700   HOCH 19
800336a4 ori v0,zero,0x16 / 80033700 bne / 80033714 andi v0,v0,0x4000 / 80033748 jal 0x80019700                 MITTE 22
80033758 ori v0,zero,0x18 / 8003375c bne / 80033770 andi v0,v0,0x2000 / 800337a4 jal 0x80019700                 TIEF 24
800336bc-c0 lui a0,0x40d / ori a0,a0,0x1000   Wurfkoerper 0x040D1000 (Rueckgabe von 0x80019700 wird NICHT geprueft)
```
**RE1.5 STAGE1.BIN — Griff ersetzt das Kommando ohne Ziel-Gate** (`re15_disasm.py dis 0x80113ad8 24 --bin STAGE1.BIN`):
`80113af8 ori v1,zero,0x2 / 80113b00 sb v1,0x800aca58` (Kommando := 2) ... `80113b30 sh zero,0x800aca5a` (Substate := 0).
Damit endet Substate 2 vor dem Spawnbild; der Abzug aus Bild 0 ist nicht rueckgaengig — im Original ist die Granate weg.
Ebenso das Inventar-Ende (`re15_disasm.py dis 0x8001cbb0 12`): `8001cbb4 sb v0,0x800b5359` (Zustand 3) ...
`8001cbdc sb zero,-13736(at)` = 0x800aca58 := 0 — Statusschirm auf/zu beendet die laufende Zielaktion.
Fenster zwischen Verbrauch und Abgang im Original: **19/22/24 Bilder**.

**RE2 Retail info/re2leon/PSX.EXE — Granatwerfer (das Retail-Vorbild "Geschoss aus einer Waffe mit Rueckstoss")**
(`re2_disasm.py dis 0x80042c90 56`, `dis 0x80043fcc 10`, `dis 0x80043190 20`, `dis 0x80044b44 24`, `table 0x800a6f90 18`):
```
80042ce8 lbu v0,7(s1) / 80042cf0 bne v0,zero,0x80042d78   Feuer-Zustand: nur beim Eintritt (+7 == 0)
80042d00 sb v1(=1),7(s1)                                  +7 := 1
80042d28 sw v0,332(s1)                                    Rueckstoss-Clip setzen (Bild 0)
80042d34 lw v0,28560(at) [0x800a6f90 + 4*Id] / 80042d3c jalr v0     Entlade-Handler: Tabelle [9] = 0x80043fcc
80043fd4 lhu a0,270(a0) / 80043fdc jal 0x8006a0cc         GL: eine Patrone (Munitions-Abzug) — beim Eintritt
800431ac lhu v0,270(s1) / 800431c4 lw v0,28636(at) [0x800a6fdc + 4*Id] / 800431cc jalr v0   Effekt-Handler JEDES Bild
80044b98 lbu v1,333(s0) / 80044b9c addiu v0,zero,1 / 80044ba0 bne v1,v0,0x80044e7c          GL-Geschoss nur im Bild 1
```
Fenster zwischen Munitions-Abzug und Abgang im Retail: **1 Bild** (Eintritt -> Bild 1). Ein abgezogenes Geschoss ist
in RE2 praktisch immer abgefeuert; die RE1.5-Handgranate (Beta, ohne RE2-Gegenstueck) streckt dasselbe auf 19-24
Bilder — in dieses Fenster fallen Griff (`80113b00`), Treffer und Statusschirm-Waffenwechsel (`8003368c`).

### 6.2 Form der Aenderung (ORCHESTRATOR-ENTSCHEIDUNG / NUTZER-VORGABE "wird nicht geworfen")
Fuer die Wurf-Ids 9/10/11 wandert der Mengen-Abzug FUN_8004eae4 vom Entlade-Handler im Rueckstossbild 0
(`jal 0x8004eae4` @0x80033b40 / @0x80033b60 / @0x80033b80) in das Spawnbild, unmittelbar hinter die erfolgreiche
Platzvergabe des Wurfkoerpers (`jal 0x80019700` @0x800336ec / @0x80033748 / @0x800337a4). Verbrauch und Abgang
fallen damit in DASSELBE Bild (Retail-Eigenschaft "abgezogen = abgefeuert", RE2 1 Bild, hier 0 Bilder). Abzug-Gate
(Magazin > 0 `jal 0x8004ea6c` @0x80033314), Rueckstoss, Spawnbilder und Spawn-Gate bleiben unveraendert. Folgen:
Griff/Treffer/Statusschirm-Waffenwechsel vor dem Spawnbild -> kein Wurf UND kein Verbrauch; kein Platz im Pool
(0xFF @0x800197c8-cc, trotz Raeumung) -> kein Wurf und kein Verbrauch.

### 6.3 Messung vorher (M1 reproduziert; exe HEAD 95d15ec7 = re15_pc.exe sha1 e18c61ef, Messkopie `re15_pc_r36dn1_vor.exe`)
Aufstellung der Abnahme (Lauf c2): `RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=-1000,-17000,1024 RE15_GIVE=9:5 RE15_EQUIP=9
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=MD0.6,MDA0.2,MD2.667,MDA0.2,MD2.0,W3
RE15_EXIT_AT=300#1140` (+ NO_INTRO/NOAUDIO/WINDOW_SCALE=1/TITLE_SHOT, Default-KI RE2), Ordner `build/r36d_mess/n1/c2_vor`:
```
state.log (nach dem Sprung)  F19  pad=8840 mg=4      Abzug 1 (Menge 5 -> 4 im Abzugsbild)
gr.log                       F=43 SPAWN granate art=2 anker=(-1735,-772,-18641)   (= 19 + 24, TIEF @0x80033758)
state.log                    F105 pad=8840 mg=3      Abzug 2 (Menge 4 -> 3 im Abzugsbild)
state.log                    F113 gr=1 PL(-1000,-17918) -> (-1043,-18081)        Zombie packt Leon (8 Bilder nach Abzug 2)
wf.log                       F113 rec=1 frame=8 -> F114 ph=0 rec=0               Ziel-FSM abgebrochen (aim_interrupt)
gr.log                       kein zweites SPAWN (Spawnbild waere F129 = 105 + 24); T=365 EV resolver art=2 eingriffe=3
state.log                    F300 mg=3 hp=40
```
= **2 Abzuege, 1 Wurf, Menge 5 -> 3** — bitgleich zum Abnahme-Befund (M1).

### 6.4 Umsetzung (Dateien, Konstanten)
| Datei | Aenderung | Beleg |
|---|---|---|
| `include/re15_granate_r36.h`, `engine/src/granate_r36.c` | `re15_granate_r36_verbrauch_im_spawnbild(id)` (1 fuer 9/10/11), `re15_granate_r36_geworfen()` (= `re15_ammo_consume` = FUN_8004eae4), Zaehler `re15_granate_r36_verbraucht()` | Entlade-Tabelle @0x80074100 [9..11] = 0x80033B38/58/78, `jal 0x8004eae4` @0x80033b40/60/80 |
| `engine/src/game_step_common.c` | Include + 1 Zeile vor der Munitions-Schleife des Entlade-Handlers (`if (!re15_granate_r36_verbrauch_im_spawnbild(eq_item))`), 1 Zeile hinter `re15_esp_granate_spawn` (`if (gs) re15_granate_r36_geworfen();`), Kommentarzeile am Spawn-Block | Bild 0 `jalr` @0x800334e0; Spawn `jal 0x80019700` @0x800336ec/@0x80033748/@0x800337a4 |
Keine neue Konstante ausser den Ids 9/10/11 (Tabelle @0x80074100). Kein Env-Schalter, keine Assets, keine Bits/IDs.
Nebenwirkung (gewollt, Folge derselben Regel): Statusschirm auf/zu waehrend der Wurfbewegung (Inventar-Ende
`sb zero,0x800aca58` @0x8001cbdc beendet die Zielaktion, mit oder ohne Waffenwechsel; OFFEN 5) und "kein Platz im
Pool trotz Raeumung" kosten ebenfalls keine Granate mehr — alle erreichen den Abgang nicht.

### 6.5 Messung nachher (exe 0bb12234, Messkopie `re15_pc_r36dn1_nach.exe` sha1 0cbfb9ea, Ordner `n1/c2_nach`)
Dieselbe Aufstellung, dieselben Eingaben:
```
state.log  F19  pad=8840 mg=5                       Abzug 1 — Menge bleibt im Abzugsbild
gr.log     F=43 SPAWN granate art=2 anker=(-1735,-772,-18641)
state.log  F43  mg=4                                Menge 5 -> 4 IM Spawnbild
state.log  F105 pad=8840 mg=4                       Abzug 2
state.log  F113 gr=1                                Griff (gleiches Bild wie vorher)
gr.log     kein zweites SPAWN; T=365 EV resolver art=2 P=(-2365,-480,-20050) eingriffe=3 (gleich)
state.log  F300 PL(-902,-18596) hp=40 mg=4          Endlage/HP gleich, Granate 2 noch im Inventar
```
= **2 Abzuege, 1 Wurf, Menge 5 -> 4.** Die abgebrochene Granate bleibt im Inventar; alles andere (Anker, Explosion,
Griff, Endlage) ist bitgleich zum Vorher-Lauf.
Gegenprobe des Pins an der ALTEN exe (`cmake -DRE15_PC_EXE=.../re15_pc_r36dn1_vor.exe -DR36_NUR=griff -P
tests/integration/test_r36_granate.cmake`): `r36_granate [griff]: Granaten 5 -> 3, soll 5 -> 4 (vorher 5 -> 3:
verbraucht, aber nicht geworfen)` = rot. Mit der neuen exe: s. 6.6.

### 6.6 Tests (Nachbesserung 1)
* `unit_r36_granate`:
  - **209 neu**: in allen 96 Wuerfen von [2] faellt die Menge IM Spawnbild (19/22/24).
  - **208 umgestellt**: Abzug-Bilder ueber den Neubeginn des Rueckstoss-Clipzaehlers (`re15_player_granate_frame`;
    beim gehaltenen Quadrat faellt das Rueckstoss-Bit nie), Verbrauchsbilder = Spawnbilder: `READY Bild 11, 3 Abzuege
    (11,45,79) / 3 Spawns (33,67,101) / 3 Verbrauchsbilder (33,67,101), Menge 2`.
  - **[4] griff neu**: 400 Treffer im Bild 8 (echter HP-Drop-Detektor -> `re15_player_stagger_cmd2` ->
    `re15_player_aim_interrupt`, dieselbe Unterbrechung wie der Griff) -> `Spawn -1, Menge 5, Verbrauch 0`;
    401 danach neu zielen + werfen -> `Spawn 22, Menge 4, Verbrauchsbild 22`; 402 Treffer im Bild 23 (nach dem
    Abgang) -> `Spawn 22, Menge 4`; 403 Waffenwechsel (Browning) im Bild 10 -> `Spawn -1, Granaten 5`;
    404 Zuordnung nur 9/10/11; 405 (nachgereicht) Statusschirm-Ende (`re15_player_cmd_zero` = `sb zero,0x800aca58`
    @0x8001cbdc) im Bild 10 -> kein Spawn, Granaten 5.
  - **Mutationsprobe** (alte Verbrauchsstelle zurueck, gebaut, gefahren): 209 `0/96`, 208 `Verbrauchsbilder
    (11,45,79)`, 400 `Menge 4`, 401 `Menge 3`, 403 `Granaten 4`, 404 rot; probe_r34_wurf 92/98/104 `Menge nach dem
    Abzug 4` rot — die Pins haben Zaehne. Danach `git checkout` + Neubau.
* `integration_r36_granate`: **Lauf `griff` neu** (= Abnahme-Lauf c2, RE15_AI_FLAVOR=re2): 2 Abzuege, Griff zwischen
  Abzug 2 und dessen Spawnbild, 1 Wurf, Granaten 5 -> 4, Verbrauchsbilder = Spawnbilder; **`serie`** prueft
  zusaetzlich Verbrauchsbilder = Spawnbilder. Auswerter `r36_wurfspur` mit Negativ-Kontrolle (synthetisches Protokoll).
* Verhaltensbedingt angepasst (fremde Pins, deren Messgroesse die alte Verbrauchsstelle war):
  - `probe_r34_wurf.c` PRUEF 92/98/104: `Menge nach dem Abzug 5, im Spawnbild 4` (vorher 4 nach dem Abzug).
  - `test_r34_granaten.cmake` `r34_abzug`: Abzugsbild = erstes Bild des Rueckstoss-Clips 7/9/11 (`ac=`; Clip =
    7 + 2*HOCH + 4*TIEF @0x800334a8-c8) statt "erstes Bild mit Menge -1" (das waere jetzt das Spawnbild S).
* Gezielte Laeufe: s. 6.7.

### 6.7 Gezielte Laeufe (KEINE volle Suite; exe sha1 3f5d5a70 = Engine-Stand 0bb12234, spaetere Commits nur Tests/Doku)
Lauf 1 (alle Tests der beruehrten Module: Granate/Wurf/Werfer/Munition/Ziel-FSM aus game_step_common.c +
player_common.c, Gore/Zerreissen/ESP aus enemy_ai_re2_zombie.c + re15_esp.c), `build/r36d_mess/n1_ctest1.log`:
```
ctest --test-dir re15_port/build -R "^(unit_r36_granate|integration_r36_granate|unit_r34_wurf|integration_r34_granaten|
  unit_r35_granate|integration_r35_granate|unit_r30_granate|integration_r30_granate_laden|unit_r17_munition_pin|
  unit_r17_waffen_loop_pin|m93r_nachladen|unit_r35_werfer|unit_r35_werfer_kombi|integration_r35_werfer|
  integration_r35_werfer_ton|integration_r35_werfer_elza|integration_r35_werfer_form|unit_esp_parse|unit_re2_gore|
  unit_re2_gore_render|unit_re2_gore_fly|unit_re2_gore_decoder|unit_re2_zombie_teardeath|unit_r19_zerreiss_10e0c7|
  unit_aim_lower_exit|unit_aim_all_around)$" --timeout 600 --output-on-failure
100% tests passed, 0 tests failed out of 26
Total Test time (real) = 787.22 sec
```
(integration_r34_granaten mit der neuen Abzug-Erkennung Passed 213.09 s; integration_r35_granate 164.47 s.)
Lauf 2 nach Sonde 405 (nur test_r36_granate.exe neu gelinkt, re15_pc.exe unveraendert sha1 3f5d5a70),
`build/r36d_mess/n1_ctest2.log`:
```
ctest --test-dir re15_port/build -R "^(unit_r36_granate|integration_r36_granate)$" --timeout 600 -V
100% tests passed, 0 tests failed out of 2
  test_r36_granate: ALLE PRUEFUNGEN GRUEN   (103-108, 201-209, 300-304, 400-405)
  r36_granate [he]: ok — X=119, 2 Zombies zerrissen, Flammen nach X: 0 (vor X: 0)
  r36_granate [brand]: ok — X=119, Eingriffe 3, Flammen nach X: 1899
  r36_granate [serie]: ok — 8 Abzuege = 8 Wuerfe = 8 Explosionen, Granaten 30 -> 22, Verbrauch in den Spawnbildern
                       38;104;173;245;320;398;479;563
  r36_granate [griff]: ok — Abzuege 19;105, Griff 113 (vor dem Spawnbild 129), Wurf 43, Granaten 5 -> 4, Verbrauch in 43
```
Punkt 1 (erfuellt laut Abnahme) unveraendert gruen: `[he]` Flammen 0, `[brand]` Flammen 1899, Sonden 103-108.

### 6.8 M3 und Vertrag
* M3 (Abnahme 0 unvollstaendig): kein Code-Mangel; die vollstaendige Testliste der beruehrten Module steht in 6.7,
  Messordner `re15_port/build/r36d_mess/n1/` (c2_vor, c2_nach, int_vor = Pin gegen die alte exe).
* Vertrag: gemeinsame Datei nur game_step_common.c (Include + 2 Code-Zeilen + Kommentar), neue Logik in
  granate_r36.c/.h; keine Bits/Nachrichten-IDs/Ereignisse, keine Assets, kein Patch an shared_assets, kein
  Env-Schalter. Fremde Tests nur verhaltensbedingt angepasst (probe_r34_wurf.c 92/98/104, test_r34_granaten.cmake
  `r34_abzug`), Registrierung unveraendert (keine CMakeLists-Aenderung).
* Elza: derselbe Pfad (Waffen-Id, nicht Figur; Bank PL04W09 mit denselben Cliplaengen, Abnahme 0 §5) — in der
  Nachbesserung nicht eigens gefahren.

## 5. Suite (Fortsetzung, Stand 90d559cc + Sonde 208)
* `bash re15_port/tools/local_build.sh test` (Lauf `build/r36d_mess/suite1.log`, 14:24-15:50, parallel zu fuenf
  fremden ctest-Laeufen anderer Spuren auf derselben Maschine): **593 von 594 bestanden**, `Total Test time (real) =
  5150.95 sec`. Einziger Ausfall: `502 - integration_r35_cut1150 (Timeout)` — `***Timeout 900.17 sec` (Szene ROOM1150,
  Spur K der Runde 35; Normalkosten laut CTestCostData 509 s, Timeout 900 s; nichts mit der Granate zu tun).
  Nachgefahren allein (`ctest -R ^integration_r35_cut1150$`, `build/r36d_mess/cut1150_rerun1.log`): **Passed 518.94 sec**
  = Last-Artefakt (VERTRAG §1.5: einzeln nachfahren, nicht als Regression werten). Alle Granaten-Tests im Lauf gruen:
  `unit_r36_granate`, `integration_r36_granate`, `unit_r35_granate`, `integration_r35_granate`, `probe_r34_wurf`.
