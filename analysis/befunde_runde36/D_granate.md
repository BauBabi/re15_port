# Runde 36 — Spur D: Granate (HE ohne Feuer an Zombies; Wurf-Aussetzer)

Stand: 2026-10-10, Zweig `r36/granate`, Baum `.claude/worktrees/r36_granate`, Basis master 5a9dbb7b (v0.8.23 + Auftrag).
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

(Fortsetzung: Messung vorher / RE-Belege / Umsetzung / Messung nachher / Tests / OFFEN / Fuer den Nutzer)

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
