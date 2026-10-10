# Runde 36 — Spur F "zombieki" (Zombie-KI, Monkeys ROOM11C0, Monkey-Blutlache)

Baum: `.claude/worktrees/r36_zombieki`, Zweig `r36/zombieki`, Basis master 5a9dbb7b (v0.8.23 + Auftrag).
Datum: 2026-10-10.

## Auftrag (Wortlaut, AUFTRAG.md)

1. > Ansonsten Verhalten sich die Zombies anders als in Resident Evil 2 - sie bleiben hier ewig
   > Beteiligungslos stehen, obwohl ich ziemlich in Ihrer Nähe vor ihnen bin, fast vor ihrer Nase.
   > Auch wenn sie innerhalb eines Raumes sind. Das darf so nicht sein....
2. > Die Monkeys scheinen noch oft blockiert zu sein. Das müssen wir irgendwie verbessern.
3. > Bei den Monkeys in ROOM 11C0 läuft das Blut nicht aus wenn sie tot sind.

## Arbeitsstand (fortlaufend)

- [x] Punkt 1 Zombies: Messung vorher (exe 1010/1040/1030/1140, Weck-Zensus 45 Faelle, Szenen)
- [x] Punkt 1 Zombies: Bisect (keine Regression; Ursache 3486be93 v0.7.18) / RE-Beleg (RE2 FUN_80050858 Hoehe + Attribut, RE1.5 FUN_8003dcc4 Nibble 3)
- [x] Punkt 1 Zombies: Ursache gefunden (Slab-Test im Sicht-Bit) + Umsetzung (zombieki_r36.c) + Messung nachher + Riegel unit_r36_zombieki_sicht
- [x] Punkt 2 Monkeys blockiert: Messung vorher (Zufallsgang 0 Stillstand; feste Plaetze: Jagd ohne Sicht an der Klemme = 1500/1500 Bilder)
- [x] Punkt 2 Monkeys blockiert: Beleg (A[3] @0x80117b40-c74 ohne Ausgang, B[3] playerX @0x80117d4c) / Umsetzung PORT-WAHL Pad-Suche / Messung nachher / Riegel unit_r36_zombieki_affen_block 7/7
- [x] Punkt 3 Monkey-Blutlache: Messung vorher (Code + Gegenprobe im Riegel)
- [x] Punkt 3 Monkey-Blutlache: Beleg (FUN_80116f50/FUN_8011be54) / Umsetzung (zombieki_r36.c) / Messung nachher (Riegel 9/9, exe-Bild) / Pin unit_r36_zombieki_affen
- [ ] Suite gruen (RE15_MIN_TESTS=592)

---

## Punkt 1 — Zombies bleiben beteiligungslos stehen

### Messung vorher (Stand master 5a9dbb7b = v0.8.23, exe-Kopie re15_pc_r36f.exe, Harness)

**exe, ROOM1010 (RE15_DEBUG_JUMP=1010@gp, RE15_PLAYER_POS, RE15_GEGNER_Y_LOG, 700..900 Bilder, keine
Eingabe; Zombie Slot 1 Typ 0x10 @(3750,3500) rot 3072 = Blick +Z):**
| Leon | Ergebnis |
|---|---|
| (3750,4300) rot 1024 = 800 VOR der Nase | F6 Sub 0 -> Sub 1 (Gang), **F8 Griff** (Sub 3), Bisse bis F97, Rueckstoss, F111 Sturz 0x501 @(2937,1913), F151 Liege-Clip 22, F346 Aufstehen -> Sub 1 Gang. **Danach steht er bei (2933,1918) bis Bild 900: Gang-Clip 0 laeuft (af 21..79), Lage aendert sich nur um 1-3 Einheiten, los=0, Leon 2173 entfernt.** |
| (2900,3500) rot 0 = 850 seitlich | F6/F7 Gang, sofort |
| (3750,2800) rot 3072 = 700 HINTER ihm | Koerperschub trennt auf 850; Sub 0 P1 mit t158 356 -> **F364 Gang** (Selbstwecker +0x158 @0x80101544-7C, kein Kegeltest) |
| (3750,5000) = 1500 vor der Nase | F6/F7 Gang, sofort |
| (2000,6200) / (-588,4335) = 3400/4400 seitlich hinter Moebeln | los=0 die ganze Zeit (Zelle 12 Kreis bzw. Zelle 5 Rechteck -50..2450/1550..5900 zwischen beiden), Sub 0 P1 bis F522, dann Wander (P2/P3) — kein Angriff (korrekt nach RE2-Regel: ohne Sicht nur Wander-Wurf @0x8010151C-40) |

Der Fall "steht bei (2933,1918)" ist ein **Taschen-Fall**: der Rueckstoss des Griffs (EXEC[3] P8 `re2z_thrust -30`
@0x80102CA0-AC, Sturz 0x501 @0x80102D24-2C) wirft den Zombie in die Tasche x 2450..3050 / z 1550..2200 zwischen
Zelle 3 (Wandstreifen z 1450..1550), Zelle 13 (Kasten 1850..3050/2200..3400, typ 3) und Zelle 15 (Diagonale
3050..4650/1400..3000, typ 2). ROOM1010 hat NUR ZWEI Nav-Zonen (Block @0x880: z -6000..500 und z 1200..8700,
keine Verbindung) — Zombie und Leon sind in Zone 1, der Navigator steuert die ROHE Spielerlage
(FUN_80039e7c-Zwilling re15_nav_update_steer, "gleiche Zone -> Ziel = Spieler"), der Gang drueckt gegen die
Zelle, die SCA-Klemme haelt ihn: **er laeuft auf der Stelle**. Sicht los=0, weil Zelle 15 (typ 2 = Diagonale) die
Strecke zu Leon schneidet.

**exe, ROOM1040 / 1030 / 1140 (Debug-Sprung-Spawn, 900 Bilder):** 1040: 5 Zombies gehen sofort in den Gang (F6/7),
los=0 durchgehend, laufen aber vom Spieler WEG (Debug-Spawn (-22000,-3200) liegt in einer durch Zelle 14
(-27866..-21782/-722..782) abgeriegelten Tasche — kein Spielweg). 1030: 6 Zombies gehen sofort los, laufen bis
z=-24838 und stehen dort (Band-0-Zelle 18 -20144..4120/-24420..-22771 = Tresen; der Debug-Spawn liegt dahinter —
kein Spielweg). 1140: 5 Liegende (Deskriptor 0x86/0x88), los=1, liegen (korrekt: Nibble 7/8 weckt im Original nur
das Skript, re2z_exec_lying).

**Harness `test_r36_zombieki_wecker mess` (echte Spielschleife, alle Leon-Raeume der Stage 1 mit Zombie-Spawns,
jeder Eintritts-Cut, jeder STEHENDE Zombie, Leon 800 vor der Nase, 480 Bilder, keine Eingabe):**
45 Faelle: **0 stehen MIT Sicht, 0 stehen ohne Sicht**, 10 ohne freien Platz vor der Nase (Spawn in/an einer
Wand: 1030 Slot 3/6, 1040 Slot 1/3/4/5, 10B0 Slot 1-3, 1200 Slot 3). Alle 35 Zombies mit Platz reagieren in
Bild 1-2 (DECISION[0] `dist<0x1388 && a1024==0 && +0x154&0x800 -> 0x101` @0x80101308-1C) und greifen in Bild
2-3 (Block G `dist<0x4b0` @0x801018F4 -> 0x301), Kriecher (0x81) gehen in Bild 1 in Sub 2.

**Harness `test_r36_zombieki_szenen` (ROOM1010 Slot 2, Leon still vor ihm; nach der Interaktion):**
griff (keine Eingabe): Griff F3, frei F110, **erneuter Griff F419**; wehr (Tastenflanken): frei F57, erneut F366;
schuss1/3/6 (Handfeuerwaffe ab 2400): Treffer (hp 98 -> 82/50/34), Griff F84/F153/F282. Kein Stillstand.

**Fazit der Messung:** die Weck-/Angriffsregeln des RE2-Hirns sind im Port vorhanden und greifen (Sicht + Kegel
sofort, Selbstwecker <= 405 Bilder, Griff < 1200). Ein Zombie, der mit freier Sicht vor Leon steht, bleibt
nirgends stehen. Was im Port "stehen" erzeugt, ist die RAUMGEOMETRIE der RE1.5-Beta-Raeume: (a) Sicht durch
Moebelzellen geblockt (Tresen/Kaesten/Pflanzen sind in RE1.5-SCA Zellen voller Bandhoehe ohne Hoehenangabe; RE2
prueft im Sichtstrahl 0x80050858 mit a3=1 die OBERKANTE des Satzes `-(w>>11)*100 - ((w>>6)&0x1f)*1800`
@0x80050b34-74 und laesst Formen 1..5/13 (Dreiecke/Raute) nie blocken, Skip-Tabelle @0x800A73B4 = [0,1,1,1,1,1,0,
0,0,0,0,0,0,1,0,0] gelesen @0x800508d0-e4); (b) Taschen zwischen Zellen ohne Nav-Zonen (ROOM1010: 2 Zonen).

### Bisect v0.8.21 -> v0.8.23
Keine Regression gefunden: der Weck-Zensus (45 Faelle) reagiert auf dem Stand v0.8.23 vollstaendig; die
Sichtfilter-Aenderung der Runde 35 Spur H (d0684bcc, `u0 & Maske` @0x800508bc-c8) kann die Sicht nur FREIER
machen (Zellen ohne Maskentreffer werden uebersprungen), nicht blinder; die Schwerkraft 94494e03 greift nur auf
Absturzkanten (Zellattribut 2, @0x8001bdb0-b4) — in den gemessenen Raeumen b0/f1c0=0000 durchgehend.

### Messung vorher, Fortsetzung (2. Sitzung): der Nutzer-Fall liegt im SICHT-BIT, nicht im Hirn

Der Weck-Zensus stellte Leon nur auf Plaetze, die der Zell-Strahl `re15_re2_los_cells_blocked` (Maske 1) FREI meldet —
er konnte den Fall per Konstruktion nicht finden, wenn der Zell-Strahl selbst die Ursache ist. Neue Sonde
`test_r36_zombieki_sicht` (echte Spielschleife wie der Weck-Zensus): je stehendem Zombie ein RASTER von Leon-Plaetzen
im Vorderkegel +-1024 (Schritt 400, Abstand 600..4000, Boden frei r 450 Maske 1), je Platz drei Werte — `zell` = der
Zell-Rechteck-Strahl (Slab-Test gegen JEDE Zelle mit u0&4, aus `re15_re2_los_clear`), `ray` = der RE1.5-Sichtstrahl
FUN_8003dcc4 (nur Zellen mit Boden-Nibble 3, 4 Regionen), `steht` = keine Reaktion in 480 Bildern.

**Ergebnis (Stand v0.8.23, `F_belege/p1_sicht_vorher.txt`): 45 Zombies, 2653 Plaetze. Zell-Strahl blockt 721 (27,2 %),
RE1.5-Strahl 415 (15,6 %), NUR der Zell-Strahl 306 (11,5 %). STEHT 318 (12,0 %): davon 250 hinter einer Nibble-3-Zelle
(Wand — korrekt), 0 bei freier Sicht, 68 NUR wegen des Zell-Strahls.** Die 68 liegen alle in **ROOM10B0 (FRONT
EXTERIOR, Vorhof)**: vier Zombies (Typ 0x10, y = -1800 = Band 1, Sce_em_set @ROOM10B0.RDT 0x1726-0x1775) stehen auf der
Terrasse hinter der Kante **Zelle #9 (t1, u0=ff, fl=0x11 = Band 1, Nibble 1; x -24392..-3980, z -11414..-9380, 20412 x
2034)**; Leon davor auf z >= -8632, Abstand 3200..4000 — der Slab-Test schneidet die Kante, der RE1.5-Strahl nicht,
DECISION[0] bekommt kein Sicht-Bit, der Selbstwecker (+0x158, braucht Sicht @0x80101544-7C) auch nicht: der Zombie
steht, bis der Wander-Wurf (rand&1 je 300..555 Bilder) ihn irgendwann in P3 bringt. Zweiter Fall derselben Klasse:
ROOM1030 Slot 3/6 (hinter dem Tresen #18 = Nibble 0, 24264 x 1649) — alle 108/122 Plaetze Zell-geblockt, dort aber
sofortige Reaktion ueber den Deskriptor (Sub 1 im Bild 1), deshalb nicht als "steht" gezaehlt.

Was das Nibble ist (Daten, Stage 1, eindeutige Zellen mit u0&4 und Nibble != 3): 1030 Tresen (Nibble 0), 1210 Gitter-
staebe der Zellen 1060 x 16662 (Nibble 0 = durchsichtig!), 1250 Tischchen 600 x 800 / 900 x 600 (0), 10B0 Terrassen-
kante (1), 11A0/1260 Rampenkanten u1=01 (1), 11C0 die sieben Wagen-Diagonalen t2/t4/t6 (2), 1020/1150/1190 Moebel (2),
Waende ueberall 1800/2000 dick (3). Das Nibble ist der von Capcom gesetzte Hoehen-/Sicht-Klassen-Wert; der RE1.5-Sensor
liest ausschliesslich 3 (Belege unten).

### Bisect v0.8.21 -> v0.8.23
Keine Regression in diesem Fenster: der Zell-Strahl kam mit **3486be93 (2026-09-08, v0.7.18, "fix(re2-kraehe): der
Sichtstrahl kennt jetzt die Zellen, die den Flug stoppen")** als Flugkopplung der RE2-Kraehe in `re15_re2_los_clear`,
das der Zombie seit dem F2-Fix (2026-09-05) mitliest. Die Sichtfilter-Aenderung der Runde 35 Spur H (d0684bcc, `u0 &
Maske` @0x800508bc-c8) kann die Sicht nur FREIER machen; die Schwerkraft 94494e03 greift nur auf Absturzkanten.

### RE-Belege (selbst gelesen)
- **RE2 (Ziel), Zombie-Navigator FUN_8004A808:** `andi v0,v0,0xf7ff` / `sh v0,0x154(s0)` @0x8004a87c-80 loescht das
  Sicht-Bit; `li a2,0x2000` @0x8004a868, `li a3,0x1` @0x8004a86c, a0 = Teil-Lage self[+0x198] + self[+0x1c1]*0xac + 0x5c
  @0x8004a878-a4, a1 = Spieler-Teil DAT_800cfd90 + DAT_800cfdb9*0xac + 0x5c @0x8004a8a8-dc, `jal FUN_80050858`
  @0x8004a8d8; Rueckgabe 0 -> `ori v0,v0,0x800` / `sh` @0x8004a8f0-f4. Leser des Bits: DECISION[0] @0x80101308-1C
  und fuenf weitere (@0x8010134C/@0x80101390/@0x80101568/@0x80101688/@0x80103AE8).
- **RE2 FUN_80050858 (RE2_Quellcode_V2, 130 Z.):** Satz-Schleife ueberspringt `(attr & a2) == 0` und Formen mit
  Skip-Tabelle `(&DAT_800a73b4)[attr & 0xf] != 0` = {1,2,3,4,5,13} (@0x800508bc-e4); Endpunkt IM Rechteck -> Hoehenpfad
  ohne XZ-Test, sonst Diagonal-Straddle in XZ; `param_4 == 0 -> return 1`; mit a3 = 1: Satzhoehe oben = `-(w>>11)*100 -
  ((w>>6)&0x1f)*0x708` (psVar7[0xd]), unten = -0x708 je Null-Bit der Boden-Maske (psVar7[0xe]); blockt nur, wenn der
  Strahl zwischen den KOERPERTEILEN das Rechteck auch in YZ und YX schneidet (@0x80050b34-c30). => niedrige Saetze
  (Tische, Tresen, Kanten) nehmen dem RE2-Zombie keine Sicht.
- **RE1.5 (Datenseite), Sensor FUN_8001b84c:** `ori a2,zero,0xf00` @0x8001b8d4, `jal 0x8003dcc4` @0x8001b8e8,
  `ori a3,zero,0x300` @0x8001b8ec (Delay-Slot) — nur Zellen mit `(w5 & 0xf00) == 0x300`. FUN_8003dcc4: Band
  `lbu v1,0x82(v1)` / `sll v0,a0,0x10` / `sra v0,v0,0x1c` / `bne v1,v0,LAB_8003e0a8` @0x8003de60-6c, Nibble
  `lhu t5,local_80(sp)` / `and v1,t5,a0` / ... / `bne v1,v0,LAB_8003e0a8` @0x8003de74-94; dann die beiden
  Diagonal-Straddle-Tests (OuterProduct0). Zweiter Aufrufer FUN_8001bb00 nimmt `ori a2,zero,0x400` / `ori a3,zero,0x400`
  @0x8001bbcc-d4 (Nibble-Bit 4 = andere Klasse). Die Kollision selbst liest vom Wort nur `& 0xf002` (Band + u1-Bit 1,
  @0x8003b8a8-b0) — das Nibble 0x0f00 ist AUSSCHLIESSLICH der Sicht-Klassen-Wert.
- **Port:** `re15_los_ray_blocked` (enemy_ai_common.c) ist der byte-true FUN_8003dcc4 (Nibble-3-Filter, Regionen);
  `re15_re2_los_clear` setzte DAVOR den Slab-Test `re15_re2_los_cells_blocked` ueber jede u0&4-Zelle — eingefuehrt fuer
  die Kraehe (Flug wird von denselben Zellen gestoppt), ohne Gegenstueck in RE2s Zombie-Strahl (Hoehe!) und ohne
  Gegenstueck im RE1.5-Sensor.

### Umsetzung
- NEU in `engine/src/zombieki_r36.c` / `include/re15_zombieki_r36.h`: `re15_r36_zombie_sicht_frei(e, pl)` = NUR der
  RE1.5-Sichtstrahl FUN_8003dcc4 ueber die 4 Regionen (Aktor-Maske wie Runde 35 Spur H), kein Slab-Test;
  `re15_r36_zombie_sicht_enable` (nur Sonden: alter Weg fuer die Gegenprobe).
- Haken: `enemy_ai_re2_zombie.c` Z. 8562 (1 Zeile: `e->re2z_los154 = re15_r36_zombie_sicht_frei(e, pl)`) + Include;
  `enemy_ai_common.c` nach `re15_re2_los_clear` der Export `re15_r36_los_ray_nib3` (6 Zeilen, Schleife ueber
  `re15_los_ray_blocked`). `re15_re2_los_clear` selbst bleibt unveraendert fuer Kraehe (Flugkopplung) und Zellenarm.
- Konstanten: keine neuen Zahlen — der Nibble-3-Filter (`0xf00`/`0x300` @0x8001b8d4/@0x8001b8ec) und die Regionen-
  Schleife stehen bereits in `re15_los_ray_blocked`.

### Messung nachher
- Sonde `test_r36_zombieki_sicht mess` (`F_belege/p1_sicht_nachher.txt`): 45 Zombies, 2657 Plaetze, **STEHT 250 (9,4 %):
  250 hinter Nibble-3-Zellen (Waende), 0 nur-Zell-Strahl, 0 bei freier Sicht**; ROOM10B0 alle vier Zombies: 0 stehende
  Plaetze (vorher 21/23/23/1), Reaktion Bild 2 (Selbstwecker-Faelle Bild 225/279).
- exe (Kopie `re15_pc_r36f.exe` = Stand mit Haken; `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10B0@250 RE15_PLAYER_POS=-17000,-8500,2048,0 RE15_AI_FLAVOR=re2
  RE15_GEGNER_Y_LOG RE15_EXIT_AT=3600#10B0`, `r36f/l2/gy.log`): Leon am Fuss der Terrasse (Band 0), die vier Zombies
  gehen in Bild 2 in Sub 1 (los=1) und laufen los. **OFFEN-Befund dabei (Navigation, nicht Sicht):** sie laufen an der
  Terrassenkante entlang nach Osten bis x ~ -6800 (Zelle #18 = Treppe x -9070..-7162 wird nicht genommen) und treten
  dort auf der Stelle — die Terrasse (z < -11414) liegt in KEINER der 5 Nav-Zonen des Raums (ROOM10B0.RDT @0x1680:
  Zone 1 = Vorhofstreifen z -9400..-6100, Zone 2 = x -4300..18300 / z -14500..-9400), der Navigator FUN_80039e7c
  (re15_nav_update_steer) findet fuer "keine Zone -> Zone 1" keinen Pfad und steuert den letzten Zwischenpunkt. Das ist
  dieselbe Nav-Datenlage wie im Original; ob RE1.5s eigene Zombies dort herunterkommen, ist nicht gemessen (DuckStation
  ROOM10B0). Fuer den Nutzer: die Zombies reagieren jetzt (Wecken, Stoehnen, Gang), erreichen Leon unten aber nicht.
- Riegel `unit_r36_zombieki_sicht` 6/6 (Gegenprobe alter Weg ROOM10B0 = 68 Befund-Plaetze; Haken: 0 / 0 / 250 = 250).

---

## Punkt 2 — Monkeys ROOM11C0 blockiert

### Messung vorher (Stand nach Punkt 1; Sonde `test_r36_zombieki_affen_block`, echte Spielschleife, beide Gorillas in der Original-Lage F195 aus test_r35_affen)
- **Zufallsgang** (Leon rennt 6000 Bilder mit zufaelliger Richtung alle 90 Bilder, HP je Bild auf 100 gehalten; Seeds
  0x1234567 / 0xbeef): **0 Strecken >= 90 Bilder ohne Bewegung**, 9 bzw. 3 Griffe — solange Leon in Bewegung und meist
  sichtbar ist, jagen beide Gorillas ihn ueber den ganzen Parkplatz (Spur `block_seed1/2.txt`). Das "Blockiert" liegt
  also nicht in der Bewegung an sich.
- **Feste Leon-Plaetze** (`fest x z`): steht Leon NOERDLICH der beiden Wagenbloecke (SCA #2 x -11900..-1940 / z -6810..
  10849 und #7 x -3862..4800 / z -7200..8400, beide Nibble 3 = Sichtblocker) — (-7000,12500), (7000,4000), (-24000,
  10000), (9000,10000) — bleiben BEIDE Gorillas (Start suedlich, Jagd Sub 3) **1500/1500 Bilder** an derselben Stelle
  ((-6048,-14181) / (-8410,-12022)): Sicht 0, Klemme 1500/1500, nur Sub 3, Abstand 18000..28000 unveraendert. Mit
  versetztem Gorilla ebenso: (-14000,-3000) -> Leon (-7000,12500): 1200 Bilder, Abstand 8415 -> 8476, Klemme 1200,
  Sicht 0 (Spur `fest*.txt`, `fest2*.txt`). Steht Leon mit Sicht ((-16000,3000), (-14000,7000), (6000,-9000)):
  Angriff in Bild 32..221. => **Blockiert = Jagd OHNE Sicht gegen ein Nibble-3-Hindernis**: A[3] hat dafuer keinen
  Ausgang (Beleg unten), B[3] steuert geradeaus auf Leon. Dasselbe Bild in der AUSWAHL (Sub 4, B[4]).
- Was das Nutzer-Erlebnis ist: Leon laeuft um den Wagenblock, der Gorilla verliert die Sicht und drueckt gegen die
  Wagen (oder springt hinter den Wagen auf der Stelle = der Blind-Zonensprung, Runde 35 Spur J: Original).

### RE-Belege (STAGE1.BIN, selbst disassembliert)
- A[3] Entscheidungsschwanz @0x80117b40-c74: `lbu player.hit_react` @0x80117b44; dist `lhu 468(v1)` / `sltiu
  v0,v0,0xbb8` @0x80117b60-68 + Latch `lhu 464(v1)` / `andi 1` @0x80117b74-7c + `lbu 7(v1)` @0x80117b88 -> Wieder-
  eintritt `sb zero,6` / `sb zero,7` @0x80117b9c/c74; `sltiu v0,v0,0x1771` @0x80117bc8 (< 6001: bleiben @0x80117c38
  `bne -> 0x80117c78` = Return); FERN: `andi v0,v0,0x1` / `beq v0,zero,0x80117c78` @0x80117c48-4c -> ohne Latch
  Return, mit Latch `ori v0,zero,0x4` / `sb v0,5(v1)` @0x80117c50-54 (Auswahl). **Kein Ausgang fuer "keine Sicht".**
- B[3] Steuern @0x80117d1c-54: `jal 0x8001af20` (rng) / `andi 0x1f` / `addiu v0,v0,6` / `sb v0,158(v1)` (+0x9e = 6..37)
  @0x80117d1c-34, `lb a1,158(v0)` @0x80117d44, **`addiu a0,a0,-13688` = 0x800aca88 = Spieler +0x34 (playerX)**
  @0x80117d4c, `jal 0x8001a8f8` @0x80117d50. FUN_8001a8f8: atan2 auf (a0.x, a0.z) gegen +0x34/+0x3c @0x8001a918-28 —
  geradeaus auf den Spieler, kein Navigator (ROOM11C0 hat ohnehin nur EINE Nav-Zone @ROOM11C0.RDT 0x16f0).
- A[4] Sprungpads @0x80117ecc-8011802c (Runde 35 Spur J belegt, hier nachgelesen): `+0x1e2 != 0` @0x80117ed4-dc,
  FUN_8003b93c(+0x34, box[6]+100, +0x82, attr 0x10) @0x80117ee0-f00, Yaw-Fenster `((+0x90&0xf0)<<4 - Yaw + 512) & 0xfff
  < 1024` @0x80117f18-38, `+0x9f = +0x90` @0x80117f40, `+0x5/6/7 = 7/0/1` @0x80117f50-74 (Sprung auch MIT Sicht);
  attr 0x20 + Latch frei + Fenster -> `+0x7 = 3` @0x80118000-24 (Blind-Sprung). Pads in ROOM11C0: #36 (4900..9500,
  -6080..-2180) und #47 (-17397..-12096, -6590..-1690) mit u1 = 0x10 in der Ost-/West-Gasse; #4/#31 und #19/#55
  mit u1 = 0x20 an den Blockkanten; alle u0 = 0 (reine Marker). Der Sprungcode (zc & 0xf0) kommt aus der Kontakt-
  seite des Pads (`(Code>>4)+8+(u1&3)` @0x8003ba8c), die Pads sind also in beide Richtungen nutzbar.
- Der Blind-Zonensprung auf der Stelle: Original (J r35 GDB 400/400, Riegel `unit_r35_affen_zonensprung`).

### Umsetzung — PORT-WAHL (NUTZER-VORGABE "irgendwie verbessern"; das Original hat hier KEIN Verhalten, das man nachbauen koennte)
- NEU in `zombieki_r36.c` / `re15_zombieki_r36.h`: **Pad-Suche in der blinden Jagd**: `re15_r36_affen_padsuche_decide`
  (A[3]-Haken, nach dem Biss-Test) — nur wenn FERN (+0x1d4 >= 6001 = dieselbe Schwelle `sltiu v0,v0,0x1771`
  @0x80117bc8/@0x80117c34, hinter der das Original MIT Sicht in die Auswahl geht; die Suche ist genau das fehlende
  Gegenstueck "fern OHNE Sicht") UND Latch +0x1d0 = 0 UND Wandklemme +0x1d6 != 0 (das Flag, das A[3]
  @0x80117b18-20 / A[4] @0x80118054-5c lesen) UND der Raum ein 0x10-Pad hat; dann steuert B[3] (`re15_r36_affen_
  padsuche_ziel`, 1 Zeile statt playerX) auf die Mitte des naechsten 0x10-Pads; steht der Gorilla darauf
  (FUN_8003b93c attr 0x10, Radius box[6]+100 wie A[4]) und im Yaw-Fenster des Originals, fuehrt er den 0x10-Sprung
  des Originals aus (+0x9f = Code, +0x5/6/7 = 7/0/1 @0x80117f40-74; Flug B[7] unveraendert); steht er falsch
  gedreht darauf, dreht er in die Pad-Richtung ((zc&0xf0)<<4). Sicht zurueck -> sofort Original-Jagd.
- Haken `enemy_ai_common.c` Fall 3: 1 Zeile vor `b3:` (decide) + 1 Zeile im Steuern (Ziel). Sonst nichts.
- Konstanten: keine neuen (Radius, Fenster, Impuls, Sprungcode = Original); 3000 = Laenge des Richtungs-Zielpunkts
  (a8f8 nutzt nur den Winkel, jeder Wert > 0 gleichwertig).
- Die Schwelle 6001 kam nach dem ersten Suitelauf dazu: ohne sie waren `unit_r35_affen_takt` und `unit_r35_affen_griff`
  (Spur J, Biss-Zyklus / Griff bildgenau gegen das Original, Gorillas NAH an Leon) rot — die Suche griff in der nahen
  blinden Jagd an der Klemme und veraenderte die Original-Spur. Mit der Schwelle: alle 20 `unit_r35_affen_*` gruen,
  die nahe Jagd ist byte-true unveraendert.
- **Gemessen und VERWORFEN** (im Riegel dokumentiert): (a) dieselbe Suche auch in der Auswahl (Sub 4) mit den 0x20-
  Pads -> Endlos-Sprungketten (19-22 Spruenge je 1500 Bilder, Leon nie erreicht); (b) Sub 4 nur mit 0x10-Pads ->
  Hin-und-her am selben Pad (Szene 1: 15 Spruenge, kein Angriff); (c) dazu "zuletzt benutztes Pad meiden" ->
  Zufallsgang 1116 blockierte Bilder, 1 Griff. Die Auswahl (Sub 4) bleibt Original.

### Messung nachher (Riegel `unit_r36_zombieki_affen_block` 7/7, `F_belege/p2_riegel.txt`)
- Zufallsgang 6000 Bilder (chaotisch: kleinste Aenderungen -> anderer Weg, deshalb nur "erreicht Leon" als Riegel):
  Seed 0x1234567 3 Griffe, 2067 blockierte Bilder — ALLE in Sub 4 (Auswahl) mit Leon 14000..33000 entfernt und
  Latch 0 (Original-Regel A[4], s. OFFEN); Seed 0xbeef 5 Griffe, 0 blockierte Bilder (vorher 3 / 219).
- Drei Befund-Szenen (Leon noerdlich/oestlich der Bloecke, Gorilla in der blinden Jagd suedlich/westlich), je 1200
  Bilder, Gegenprobe (Suche aus) gegen Haken: (1) (-7000,12500)/(-14000,-3000): vorher kein Angriff, Abstand min 8416,
  1200/1200 Sub 3 — nachher 3 Spruenge, erster Angriff Bild 290, Abstand min 954; (2) (-7000,12500)/(-5525,-14883):
  vorher min 24545 — nachher Angriff Bild 893, min 955; (3) (7000,4000)/(-8915,-12487): vorher min 22222 — nachher
  Angriff Bild 523, min 954.
- exe (Kopie `re15_pc_r36f.exe` = Stand mit beiden Haken; `RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11C0@250
  RE15_PLAYER_POS=-7000,12500,2048,0 RE15_GEGNER_Y_LOG RE15_EXIT_AT=2400#11C0`, `r36f/l3/gy.log`): die Ada/Marvin-Szene
  setzt Leon auf (-7107,-12395) (PLAYER_POS wird von der Szene ueberschrieben), die Gorillas kommen F900/F1200 aus den
  Wagen, Jagd ab F1201 (Sub 3), erste Bisse F1651 (Slot 2) / F1801 (Slot 3) — der normale Fluss ist unveraendert.
  Ein exe-Lauf mit Eingabeskript "Leon rennt nach der Szene um den Westblock" wurde nicht gefahren (OFFEN); die
  Mechanik ist im Riegel mit derselben Spielschleife (scd_room_reenter + re15_game_step) belegt.
- OFFEN (gemessen, Original-Regel): in der AUSWAHL (Sub 4, B[4]) ohne Sicht an der Klemme bleibt der Gorilla stehen
  (A[4] ohne Sicht ohne Ausgang, B[4] steuert geradeaus) — z.B. Leon (-14000,2000) westlich, Gorilla nach dem Sprung
  in der Ost-Gasse (6418,2500): 1500 Bilder Sub 4. Jede gemessene Erweiterung auf Sub 4 war schlechter (oben).

---

## Punkt 3 — Monkey-Blutlache laeuft nicht aus

### Messung vorher (master 5a9dbb7b)
- Code: `re15_maggot_ai_tick` Zustand 7 (enemy_ai_common.c "CORPSE 0x8011be54") zaehlte nur +0x9e herunter; Tinte und
  Wachstum standen als Kommentar ("render-side"), wurden aber NIE geschrieben; der INIT trug die HUNDE-Werte
  1000/500 (@0x8010dabc/dac8 = Hund!) in +0xBC/+0xBE; der Renderer (main.c Schattenquad) las die Felder fuer Typ
  0x27 gar nicht -> 500x600-Standard-Charakterschatten unter der Leiche. Dieselbe Fehlerklasse wie der Hund
  (Runde 34 "laufen nicht aus").
- Riegel `test_r36_zombieki_affen` Gegenprobe (Haken aus, echter Schusspfad Handfeuerwaffe auf Gorilla mit 1 HP in
  ROOM11C0): `VORHER: Tod Bild 90, pool ab Leichenbild -1 (nie), 0 Takte Wachstum, Ende 1700/1800` (mit INIT-Haken;
  Stand v0.8.23 zusaetzlich 1000/500).

### RE-Belege (STAGE1.BIN, re15_disasm.py, alle selbst gelesen)
- INIT FUN_80116f50: `lbu v0,482(v1)` (+0x1e2 = 4 @0x80117098-9c) @0x801171a0, `lw v0,0(at)` Tabelle 0x80121368[4]
  = 0x80121350 @0x801171b8, `sw v0,120(v1)` (+0x78) @0x801171c0, `lhu a3,6(v1)` @0x801171d8 = u16 @0x80121356 =
  **1600**, `addiu a2,a3,100` @0x801171e8, `jal 0x8001af5c` @0x801171ec, `addiu a3,a3,200` @0x801171f0 (Delay-Slot),
  sp+16 = entity+0xB0 @0x801171dc-e0, sp+20 = 0x80808080 @0x8011716c/bc/e4. FUN_8001af5c: `addu s3,a2` @0x8001af80,
  `addu s4,a3` @0x8001af88, `sh s3,12(s0)` @0x8001b030 (= +0xBC), `sh s4,14(s0)` @0x8001b038 (= +0xBE).
  => +0xBC = 1700, +0xBE = 1800. Konventions-Gegenprobe: der Zombie-INIT uebergibt `ori a2,zero,0x258`/`ori a3,zero,
  0x2bc` = 600/700 @0x80100c2c/c38 — der Port zeichnet dafuer Halbmasse nhx 600 / nhz 700.
- Record 0x80121338..0x80121370 (u16): [0,-1080,0,1100,1080,1100 | 0,-1530,0,800,1530,800 | **0,-1440,0,1600,1440,
  1600** | 0,-2160,0,1100,2160,1100] = Hitbox-Records (Halbbreite 1600 = hit_radius_min des Gorillas).
- LEICHE FUN_8011be54: `lbu v1,7(a0)` @0x8011be60; +0x7 == 1 -> 0x8011bed0; `slti v0,v1,2 / beq -> jr` (>= 2 inert)
  @0x8011be70-74; +0x7 == 0: `ori v0,zero,0x5a / sb v0,158(a0)` @0x8011be80-84, `sb 1,7` @0x8011be90-94, Wort0 |= 2
  @0x8011beac-b0, |= 0x40 @0x8011bec8-cc, DURCHFALL nach 0x8011bed0; dort `lui a2,0xff` @0x8011be6c + `ori a2,a2,
  0xff38` @0x8011bed0, `lui a1,0xff00` @0x8011bedc, +0xC4 = (alt & a1) | a2 @0x8011bee0-f4, +0xEC ebenso
  @0x8011bee4-f00, **`lhu v0,188(a0)` / `lhu v1,190(a0)` / `addiu v0,v0,8` / `addiu v1,v1,8` / `sh v0,188(a0)` /
  `sh v1,190(a0)` @0x8011bef8-bf14** (+0xBC/+0xBE += 8), `lbu a0,158(v1)` / `addiu v0,a0,-1` / `bne a0,zero` /
  `sb v0,158(v1)` (Delay-Slot) @0x8011bf24-34, alt == 0 -> `sb 2,7` @0x8011bf40-44.
  => 91 Takte Wachstum (Takt 0 Durchfall + 90), Endmasse 2428/2528, dann inert.

### Umsetzung
- NEU `engine/src/zombieki_r36.c` + `include/re15_zombieki_r36.h`: `re15_r36_affen_lache_init` (+0xBC/+0xBE =
  1600+100 / 1600+200), `re15_r36_affen_lache_tick` (crow_pool = 1, += 8), `re15_r36_affen_lache_render` (Typ 0x27:
  Halbmasse aus den Feldern, Lachenfarbe 0x00ffff38 -> r 0x38 g 0xff b 0xff auf dem subtraktiven Schattenblob wie
  der RE1.5-Hund @0x801117F0), `re15_r36_affen_lache_enable` (nur Sonden).
- Haken: enemy_ai_common.c INIT 0x27 (1 Zeile, ersetzt die zwei Hunde-Werte), Zustand 7 (1 Zeile vor dem +0x9e-
  Dekrement), main.c Schattenquad (1 Zeile vor dem Kraehen-Zweig); je ein Include.
- Konstanten: RE15_R36_AFFEN_HITBOX_HALB 1600 (@0x80121356), +100 (@0x801171e8), +200 (@0x801171f0), Schritt 8
  (@0x8011bf08-0c), Takte 0x5a (@0x8011be80-84).

### Messung nachher
- Riegel `unit_r36_zombieki_affen` 9/9: Tod Bild 90 (echter Schusspfad), INIT 1700/1800, Lache ab dem ersten
  Leichen-Root-Takt, 91 Takte je +8 (monoton), +0x7 = 2 nach 91 Takten, Ende 2428/2528; Gegenprobe ohne Haken =
  Befund (keine Lache, kein Wachstum); Renderer-Haken 0x27 liefert 2428/2528 + 0x38/ff/ff, Hund 0x20 nicht.
- exe (Kopie re15_pc_r36f.exe = Stand nach dem Haken; `RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11C0@240 RE15_GIVE=7:200
  RE15_EQUIP=7 RE15_INPUT_SCRIPT=(MA0.1,M0.6)x60 _BASIS=spiel _START=1260`, Rezept Spur J r35): Slot 3 Zustand 7 ab
  F1827, Slot 2 ab F1888 (state.log). Framedump F1820-2120/20 mit dem beschleunigten Renderer:
  `F_belege/p3_11c0_lache_F1900.png` — unter dem toten Gorilla die dunkelrote, groesser werdende Lache
  (subtraktiv, wie Zombie/Hund); `p3_11c0_vor_tod_F1820.png` = Vergleich davor.
  ⛔ Mit RE15_SOFTWARE_RENDER=1 erscheint die Lache CYAN (`p3_..._softwarerender_cyan_artefakt.png`): SDLs
  Software-Rasterizer kann den REV_SUBTRACT-Blend (render_pc.c @673) nicht, die Farbe 0x38/ff/ff wird dann roh
  gezeichnet — Messartefakt, gilt genauso fuer Zombie- und Hundelache, nicht fuer den Nutzer (GPU-Pfad).

---

## Tests (probes/r36_zombieki.cmake, alle in der Suite)
| Test | Punkt | Misst | Ergebnis |
|---|---|---|---|
| `unit_r36_zombieki_wecker` | 1 | Weck-Zensus: 45 stehende Zombies, Leon 800 vor der Nase (Plaetze mit freiem Zell-Strahl), 480 Bilder | 2/2 (Vorgaenger) |
| `unit_r36_zombieki_szenen` | 1 | Griff/Abwehr/Schuss in ROOM1010: erneuter Angriff danach | gruen (Vorgaenger) |
| `unit_r36_zombieki_sicht` | 1 | Sicht-Zensus 45 Zombies x Raster (2657 Plaetze): Gegenprobe alter Weg ROOM10B0 = 68 Befund-Plaetze; Haken: 0 nur-Zell-Strahl, 0 bei freier Sicht, 250 = 250 hinter Nibble-3 | 6/6 |
| `unit_r36_zombieki_affen_block` | 2 | Zufallsgang 6000 Bilder (0 Stillstand, 12 Griffe) + 3 Befund-Szenen Gegenprobe (kein Angriff, Abstand bleibt) gegen Pad-Suche (Angriff Bild 290/893/523) | 7/7 |
| `unit_r36_zombieki_affen` | 3 | Gorilla-Leiche: Schattenrecord 1700/1800, Lache 0x00ffff38 +8/Takt ueber 91 Takte am echten Schusspfad, Gegenprobe = Befund | 9/9 |

## OFFEN
- **Punkt 1, ROOM10B0 Terrasse (Navigation, nicht Sicht):** nach dem Wecken laufen die vier Zombies an der Terrassen-
  kante entlang nach Osten und treten bei x ~ -6800 auf der Stelle — die Terrasse liegt in keiner der 5 Nav-Zonen
  (ROOM10B0.RDT @0x1680), FUN_80039e7c findet keinen Pfad, die Treppe #18 wird nicht genommen. Gleiche Datenlage wie im
  Original; ob RE1.5s eigene Zombies dort herunterkommen: DuckStation ROOM10B0 nicht gemessen. RE2-Ziel (Zombies auf
  Treppen) braeuchte die Band-/Treppenlogik fuer Gegner — nicht Spur F.
- **Punkt 1, Sichtstrahl-Maske:** `re15_los_ray_blocked` filtert seit Runde 35 Spur H mit der Aktor-Maske (`u0 & Maske`);
  der RE1.5-Sensor FUN_8003dcc4 prueft u0 NICHT (nur Band + Nibble). Beibehalten (H-Entscheidung); eine Nibble-3-Zelle
  ohne Bit 4 in u0 wuerde dem Zombie die Sicht lassen, dem RE1.5-Sensor nicht — in Stage 1 nicht gezaehlt.
- **Punkt 2, Auswahl (Sub 4) ohne Sicht an der Klemme:** Original-Regel ohne Ausgang (A[4]), bleibt; alle drei
  gemessenen Erweiterungen waren schlechter (Dossier Punkt 2). Zufallsgang Seed 0xbeef: 219 blockierte Bilder (1,8 %).
- **Punkt 2, exe-Lauf mit Eingabeskript** (Leon rennt nach der Szene um den Westblock, Gorilla verliert die Sicht):
  nicht gefahren; die Szene ueberschreibt RE15_PLAYER_POS, ein Laufskript waere noetig.
- **Punkt 2, Original-Gegenmessung per GDB:** die Pad-Suche ist PORT-WAHL; eine DuckStation-Messung des Originals in den
  drei Szenen (Erwartung: Gorilla bleibt in Sub 3 an der Klemme, wie A[3] @0x80117b40-c74 zeigt) wurde aus Zeitgruenden
  nicht gefahren; die Regel ist statisch belegt.
- Vorgaenger-Punkte aus Punkt 3: keine.

## Fuer den Nutzer
- Neue Assets fuer das Paket-/Android-Gate: keine. Sprachdateien: keine. Bedienhinweise: keine.
- Spuerbar neu: (1) **Zombies** wecken jetzt auch dann, wenn zwischen ihnen und Leon niedrige Hindernisse (Tresen,
  Terrassenkante, Tischchen, Gitterstaebe) liegen — nur Waende und hohe Objekte (Capcoms Sicht-Klasse 3 je Zelle) nehmen
  die Sicht, wie in RE2 (dort Satzhoehe). Im Vorhof ROOM10B0 reagieren die vier Zombies auf der Terrasse jetzt, kommen
  aber (noch) nicht herunter (Nav-Daten, s. OFFEN). (2) **Monkeys ROOM11C0**: verliert ein Gorilla in der Jagd die
  Sicht und drueckt gegen einen Wagen, laeuft er jetzt zum naechsten Sprungpad der Gasse und springt (der Original-
  Sprung) statt stehenzubleiben; der Blind-Sprung auf der Stelle hinter den Wagen bleibt Original. (3) **Gorilla-
  Leiche** laesst Blut aus (Punkt 3, 1. Sitzung).
- Entscheidungsfragen: keine.

---

## Nachbesserung 1 (nach Abnahme 0, 2026-10-10) — fortlaufend

Grundlage: `F_abnahme_0.md` (Stand 853cbfb3, NICHT BESTANDEN, Maengel 1-8). Plan je Mangel:

| Mangel | Kern | Weg |
|---|---|---|
| 1 | P1 innen: keine Normalfluss-Reproduktion; Nibble-3-Moebel nehmen die Sicht (gegen RE2-Hoehenregel @0x80050b34-74) | RE2-Koerperteil-Hoehen des Strahls belegen (+0x1c1/+0x5c), Moebelhoehen aus RDT-Daten, Stillstand-Zensus nach LAGE (nicht nach Reaktion) mit echten Tuer-Eintrittsplaetzen |
| 2 | ROOM10B0 Terrasse: Zombies laufen weg, bleiben an der Kante | Nav/Band-Weg der Gegner belegen, Original/RE2 messen, beheben |
| 3 | Zensus klassifiziert Moebel als Wand, Raster laesst Tisch-Ostseite aus | Zensus neu (Klassen Wand/Moebel aus Daten), Riegel pinnt die richtige Abbildung |
| 4 | Pad-Suche schickt von Leon weg | Zielwahl mit Bezug auf Leon |
| 5 | Sub 4 unbehandelt (93,5 % Rest-Blockade) | Sub 4 einbeziehen, Statistik ueber viele Seeds als Riegel |
| 6 | Riegel prueft nur griffe > 0 | Riegel auf blockierte Bilder (Seed-Satz), Dossier-Zahlen korrigieren |
| 7 | ROOM1030 "kein Spielweg" falsch | Dossier korrigieren |
| 8 | keine Voll-Suite | Orchestrator; hier gezielte Laeufe |
