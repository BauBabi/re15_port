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
- [x] Nachbesserung 3 (Abnahme 2): M1 Umweg nach Weglaenge (A*), M2 Tasche = Zurueckschnapp-Artefakt + nicht begehbar, Beginn erst beim Haengen (+0x1de), Riegel 46/46, exe zehn Stellen nicht spaeter als v0.8.23, gezielt 163/163
- [x] Nachbesserung 4 (Abnahme 3): M1 FERN-Stau am Nordwest-Rand (Original per GDB = Stau; PORT-WAHL Vortritt, exe FERN 0), M2 Nah-Wartezeit am Original gemessen (Nordwest-Ecke, Ostgasse: Original wartet ebenso, Riegel pinnt es), Angabe "fern 0" berichtigt
- [x] Nachbesserung 5 (Abnahme 4): M1 PORT-WAHL Nah-Umweg zum freien Bissplatz ab 354 (= J-Fenster 353 + 1, kanonischer Kampf bitgleich), exe Stelle 3 1653/1875 -> 429/368, 14 Stellen 10466 -> 7500; Nordwest-Ecke = Anstehen (nur EIN Bissplatz, gemessen) mit Nachruecken; M2 Begruendung berichtigt; M3 Original-Gleichheit im Modus Original-Regel, Riegel 66/66, gezielt 232/232
- [ ] Voll-Suite (Orchestrator im Integrationsbaum)

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

### NB1 Zwischenstand 1 — Eintritts-Zensus nach LAGE (neue Sonde `test_r36_zombieki_eintritt`)
- Sonde: jede Door_aot_set-Tuer eines Stage-1-Raums in einen Raum mit Zombies (78 Eintritte, Ziel-Id
  ((pc22+1)<<12)|(pc23<<4) wie aot_common.c), Leon am Spawnpunkt; Szenario "steht" und "hin" (Leon geht per Pad UP
  mit Blick auf JEDEN Zombie bis 900 vor ihn); je Zombie Reaktion ab der Basis NACH dem INIT (Bild 1), kleinster
  Abstand, laengste Strecke ohne Bewegung (< 40 je 30 Bilder, Leon < 5000, kein Griff). 253 Faelle, Rohdaten
  `F_belege/nb1_eintritt_vorher.txt` (Stand 72050beb + Sonde).
- Ergebnis: KEIN stehender Zombie (Basis Sub 0) bleibt im Normalfluss passiv. Lange Stillstaende mit Leon nah:
  1. **ROOM1010 Slot 1 (Tuer aus 1020, Cut 0, "hin")**: Griff F26, Rueckstoss in die Tasche (2933,1918), danach
     **auf der Stelle ab F780 bis Laufende, 1894 vor Leon, los 0** — der Innenraumfall im NORMALFLUSS.
  2. Kriecher Deskriptor 0x81 (ROOM1010 Slot 3/4, ROOM1220 Slot 1/2): Sub 2 = RE2-Lauern (INIT Zeile 1 `0x201`
     @0x80100b6c-74; DECIDE[2] @0x80103A70: Kegel 512, dist < 0x514, Sicht, Riegel, gleiche Etage -> 0x101), fruehere
     NUTZER-VORGABE 2026-09-19 ("stellen sich erst einmal tot ... in RE2 auf jeden Fall auch") — kein Defekt.
  3. Nach Griffen Sub 5 = RE2-Knockdown EXEC[5] @0x80103188 (Liegezeit +0x15A, @0x80103548-64) — Liegezeit, kein Stehen.
     Der Rueckstoss davor (EXEC[3] P8: Clip-Delta + `-30` @0x80102CA0-AC) traegt den Zombie ~3000 Einheiten
     (ROOM1220 Slot 4 F186-F198 je Bild 190-330) — Keyframe-Wurzeln der RE2-Bank, nicht Spur F.
  4. ROOM1040 (Tuer aus 1030/1080): Deskriptor 0x0D (Sub 1 ab Start) drueckt gegen Zelle #14/#17 (x -27866..-21782,
     z -722..782, Nibble 3), Leon kommt selbst nicht naeher (dmin 2372) — Raumgeometrie.
  5. ROOM1030 aus 1050: die geskriptete Tor-Szene (Abnahme 0, Original).
- **Mangel 7 (Dossier-Fakt ROOM1030):** die Aussage "Debug-Spawn ... kein Spielweg" ist FALSCH und hiermit
  zurueckgenommen: die Tueren ->1050 (2250,-12700), ->1070 (-16150,-3800), ->1040 (-26900,-3650) liegen auf Leons
  Seite (Abnahme-AOT-Dump), der Zensus faehrt den Eintritt aus 1050 (Leon (1172,-12558), Cut 2). Verhaltensfolge
  keine: main00 setzt die Zombies bedingungslos (Sce_em_set @0x1DE6..0x1F62), das Haengen am Tor ist die geskriptete
  Kriech-Szene (Original, Abnahme 0).

### NB1 Mangel 2 — ROOM10B0 Terrasse: Ursache gemessen, behoben
- **Messung:** `test_r36_zombieki_eintritt platz 10B0 0 -17000 -8500 1 1500 300 0` (Haken aus): die vier Zombies
  reagieren (Bild 2), ihr Steuerziel ist **steer = (0,0)**, sie laufen nach Osten zum Weltursprung (Ende x -6825 /
  -6024 / -5223 / -4416, Abstand bis 10706). Ursache: die Terrasse liegt in keiner BLK-Zone (ROOM10B0.RDT @0x1680),
  re15_nav_update_steer (FUN_80039e7c-Zwilling) ruft fuer Zone 0xff den Pfadsucher ohne Ergebnis und laesst das Ziel
  stehen (Startwert 0).
- **RE2 (Ziel):** +0x1c4/+0x1c6 schreibt NUR der Navigator FUN_8004A808 (RE2_Quellcode_V2 Z. 46/60) bzw. FUN_8004ab60
  (Z. 347); das Zombie-Overlay EMZ0.BIN liest sie nur (13 `lh ...,452/454`-Stellen, kein Store — eigener Voll-Dump).
  Bei Zone 0xff (FUN_8004aab8) und a3 = 0 schreibt er nichts (`return 2`). Die DIREKTE Form (Ziel = Zielposition)
  steht im Zweig +0x10E & 0x1000: `lw v0,0(s3)` / `sh v0,452(s0)` / `lw v1,8(s3)` / `sh v1,454(s0)` @0x8004a90c-924.
- **Umsetzung (PORT-WAHL, Nutzer "Das darf so nicht sein"):** `re15_r36_nav_zonenlos` (zombieki_r36.c): RE2-Zombie in
  KEINER Zone eines Raums MIT Graph -> Ziel = Spielerlage (die direkte Form; dieselbe, die der Port-Navigator ohne Graph
  nimmt). Haken: enemy_ai_common.c 1 Zeile direkt nach dem Navigator-Aufruf (FUN_80039e7c @0x8010a9c0-e0-Zwilling).
- **Abstieg gibt es nicht (Daten):** SCA #9 (Band 1, u0 = 0xff, x -24392..-3980, z -11414..-9380) klemmt jedes
  Band-1-Wesen an der Kante, auch ueber der Treppe #18 (Band 1, u0 = 0x01, u1 = 0x02, x -9070..-7162); der einzige
  AOT am Treppenbereich (aot2, Typ 8, Aot_set pc[3] = 0x41 -> Pool-Maske 1 = NUR Spieler, Test `&kind` @0x80042c84;
  Gegner laufen mit kind 2 @0x80043724, FUN_800436a8) — Zombies verlassen die Terrasse weder in RE1.5 noch in RE2.
- **Nachher** (Haken an): die vier Zombies stehen VOR Leon an der Kante (x -18474 / -17671 / -16870 / -16061,
  z -11832, Abstand 3317..3643, Ziel = Leon) statt 10700..13000 entfernt im Osten.

### NB1 Mangel 1 — Innenraumfall im Normalfluss + niedrige Nibble-3-Moebel
- **Ursache 1 (Innenraum, Normalfluss):** Eintritts-Zensus (`test_r36_zombieki_eintritt`, oben): kein STEHENDER Zombie bleibt
  im Normalfluss passiv; der einzige Innenraum-Dauerstillstand ist ROOM1010 Slot 1 nach dem Griff (Tuer aus 1020,
  Leon geht auf ihn zu): Rueckstoss EXEC[3] P8 (Clip-Delta + `-30` @0x80102CA0-AC) traegt ihn in die Ueberlappung
  der Zellen #13/#15/#3/#5; die echte Klemme (FUN_8003b0a4-Zwilling) findet dort keine freie Lage — Raster
  `test_r36_zombieki_eintritt raster 1010 1 250`: (2933,1918) gesperrt, naechster freier Knoten (3600,3350), dazwischen
  alles gesperrt. Der Zombie tritt ab F780 auf der Stelle, 1894 vor Leon. -> OFFEN (Wurf/Kollision, s. unten).
- **Ursache 2 (Abbildungsluecke Moebel, Abnahme-Fall):** ROOM1010 traegt in ALLEN 16 Zellen (x5 Partitionen) fl = 0x03
  (`sca.py 1010`): Tisch #5, Stuhlkreise #12/#13/#14, Waende — das Nibble trennt dort nicht. Messung exe (Abnahme-Lage,
  Leon (3750,4300) r1024): RE2-KI Slot 2 F1..F1000 in 1/0/1/0 bei (205,7248); **RE1.5-KI (RE15_AI_FLAVOR=re15) laeuft
  derselbe Zombie um den Tisch auf Leon zu (F300 bei (2853,6614), Sub 12)** — der Port war dort passiver als BEIDE Originale.
- **Belege:**
  - RE2-Strahl Kopf zu Kopf: Zombie +0x1c1 = 8 (`addiu v0,zero,8` / `sb v0,449(s2)` @0x8010082c-30, EMZ0.BIN), Teil-Lage
    `lw a0,408(s0)` + +0x1c1*0xac + 0x5c @0x8004a874-a4, Spieler DAT_800cfd90/DAT_800cfdb9 @0x8004a8a8-dc,
    `jal 0x80050858` mit `addiu a3,zero,1` @0x8004a86c; +0x1c1 ist der Blickteil (FUN_800177c0 Neck-FSM liest ihn).
    FUN_80050858 a3=1: Oberkante `-(h>>11)*100 - ((h>>6)&0x1f)*0x708` (Z. 71), blockt nur bei Schnitt in XZ, YZ und YX.
  - Kopfhoehen GEMESSEN (exe ROOM1010, temporaere Ausgabe, nicht eingecheckt): Zombie re15_enemy_bone_world_pos(e,8)
    y -2484..-2517, Leon head_world y -2319..-2514 (Stand -2514, Motion 200).
  - Tischplatte #5 = y -1600: Obj_model_set (200,-1600,5500) / (1800,-1600,5750) liegen im Grundriss von #5 (Sonden-Log
    `[scd] Obj_model_set[0/1]`); #6 im Suedteil ebenso ((-1800,-1600,-1900)).
  - Stuhlkreise: Projektion mit den Engine-Matrizen (`probe_r34n_e_dokumente kamera 1010`, geom.py): die Tisch-Ecken bei
    y -1600 liegen exakt auf den Tischkanten (Eichung), die Marke y -2000 der Stuhlsaeulen #12/#13 liegt auf der Wand
    ueber der Lehne -> Oberkante unter -2000 (`F_belege/nb1_r1010_c0_hoehen.png`, Skript `F_belege/nb1_hoehe_1010.py`).
- **Aenderung:** `re15_r36_zombie_sicht_frei` (zombieki_r36.c): Kreuzungstest je Zelle wie FUN_8003dcc4 (Export
  `re15_r36_los_zelle_kreuzt`, enemy_ai_common.c, 4 Zeilen); gekreuzte Nibble-3-Zelle MIT belegter Oberkante blockt nur,
  wenn der Strahl Kopfteil 8 Zombie -> Kopfteil 8 Leon im Grundriss tiefer liegt (Abtastung 50er-Schritt). Belegte
  Oberkante: (a) Obj_model_set-Objekt (Weltraum) auf der Zelle -> dessen y; (b) Messtabelle `k_r36_moebel` (die drei
  Stuhlkreise ROOM1010, -2000). Ohne Beleg = volle Hoehe (Wand) wie bisher. Leons Kopf: head_world, sonst y - 2514 (gemessen).
- **Nachher:** exe Abnahme-Lage: Slot 2 reagiert F10 (Sub 1, Sicht 1), laeuft um den Tisch ((642,6969) -> (1921,6326)
  -> (2884,5712)), Griff F300 (`F_belege/nb1_exe_1010_abnahmefall.txt`). Riegel `unit_r36_zombieki_sicht`: Abnahme-Fall
  vor NB1 verdeckt / mit NB1 frei.

### NB1 Mangel 3 — Zensus klassifiziert richtig
- **Ursache:** der alte Zensus zaehlte Plaetze HINTER Aussenwaenden (z.B. ROOM1010 (7350,3500) jenseits Wand #0) und
  nannte jeden Nibble-3-Blocker "Wand"; Raster nur bis 4000.
- **Aenderung (`test_r36_zombieki_sicht`):** Raster bis < 0x1388 (DECISION[0] @0x80101308), nur Plaetze IN einer
  BLK-Zone (re15_nav_zone_from_pos != 0xff = Capcoms begehbare Flaeche), Klassen je stehendem Platz: hinter Zelle OHNE
  belegte Oberkante (Wand) / NUR hinter belegten Moebeln.
- **Nachher (`F_belege/nb1_sicht_nachher.txt`):** 45 Zombies, 3360 Plaetze (2928 ausserhalb verworfen), STEHT 49 = alle
  hinter Waenden (ROOM1220 Zellwaende #24/#28 200 dick, ROOM1250 #14 19100x2000, ROOM1200 #8/#9 Band-1-Reihe 2000 dick,
  ROOM1180), 0 nur hinter Moebeln, 0 bei freier Sicht. Gegenproben: vor NB1 ROOM1010 24 Moebel-Plaetze; vor Runde 36
  ROOM10B0 267 Slab-Plaetze.

### NB1 Maengel 4/5/6 — Monkeys ROOM11C0
- **Messung vorher (Stand 72050beb, 52 Seeds x 6000 Bilder):** im selben Prozess AUS 60561 / AN 76667 blockierte Bilder
  (besser 22, schlechter 28) — die "naechstes Pad"-Suche war netto schlechter. Mess-Falle gefunden: Laeufe im selben
  Prozess sind NICHT unabhaengig (nur-AUS 73006 gegen 61502 im Wechsel mit AN, trotz festem Engine-Zufall) -> seitdem je
  Lauf ein eigener Prozess (Modus `eins`): AUS 75408 blockiert / 226 Griffe. Aufschluesselung: 95 % fern (>= 6001),
  22 % mit Sicht, 100 % an der Klemme; 90 % Sub 4.
- **Ursache (gemessen):** Lagekarte der Blockaden (`stat ... 0 1`): 23030 von 60561 blockierten Bildern auf EINER Stelle
  (6418,-3617) = Mitte im 0x10-Pad #36 der Ost-Engstelle; zweiter Brennpunkt das West-Pad #47. Spur Seed 0x708f5bfa:
  Gorilla F426-F2744 dort, `zc = 0xf8` (Platzhalter 0xf00, Mitte im Pad, coll_contact_dir @0x8003ba8c), Fenster
  1800-2700 statt < 1024 (@0x80117f18-38), B[4] dreht ihn zu Leon. Begehbarkeitskarte (`raster 800 1600`, echte
  Klemme): Nord-/Suedteil fuer den Gorilla zu Fuss getrennt, nur die Pads verbinden.
- **Aenderung (PORT-WAHL, Nutzer "irgendwie verbessern"; Header re15_zombieki_r36.h Punkt 2):** Umweg statt
  Naechstes-Pad: Raster aus der Original-Klemme, Komponenten + Breitensuche; gleiche Komponente -> zu Fuss; andere ->
  Anlauf am verbindenden Pad von der eigenen Seite (Sprung = Original A[4] Path A / Sub 3 @0x80117f40-74); Mitte im Pad
  -> in Code-Richtung drehen. Sub 3 und Sub 4 (Haken: enemy_ai_common.c vor B[4] und B[4]-Ziel, je 1 Zeile). Gemessen
  und verworfen auf dem Weg: (i) "Pad zwischen G und L" allein (wirkungslos: im Keil liegt kein Pad zwischen G und L);
  (ii) Nah-Tor weg (23114 statt 12288 — stoert die Nahkampf-Logik); (iii) Gerade-Strecke-Test (12560/228 statt
  12288/285 — Raster "frei", Klemme haelt trotzdem).
- **Nachher:** 52 Seeds: AUS 75408 -> AN 2380 blockiert (-96,8 %), Griffe 226 -> 254, besser 39 / schlechter 3 (+194,
  +34, +143) / gleich 10 (`F_belege/nb1_affen_52seeds_nachher.txt`); exe Kampf-Layout (`RE15_SET_FLAG=4:64
  RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-7000,12500,1024,0`): Slot 2 Angriff F896 (v0.8.23 nie, Ende 22376 entfernt),
  Slot 3 F606 (v0.8.23 F1012) (`F_belege/nb1_affen_exe_kampflayout.txt`).
- **Mangel 6 (Riegel):** `unit_r36_zombieki_affen_block` misst jetzt die Blockade: 16 Seeds AUS/AN je eigener Prozess
  (AUS 28653 / AN 721, schlechtester +194, Griffe 73 -> 80) + drei Befund-Szenen (Angriff Bild 140/628/533, Gegenprobe
  ohne Angriff) — 11/11 (`F_belege/nb1_affen_riegel.txt`). Die frueheren Dossier-Zahlen "Seed 0xbeef vorher 3 / 219"
  und "Zufallsgang 0 Stillstand, 12 Griffe" sind damit ueberholt (sie stammten aus Laeufen im selben Prozess).

### NB1 Mangel 8 — Voll-Suite
Im Spurbaum verboten (Auftrag); der Orchestrator faehrt sie im Integrationsbaum. Hier gezielte Laeufe (unten).

### NB1 Tests (probes/r36_zombieki.cmake; ersetzt die Tabelle "Tests" oben)
| Test | Punkt/Mangel | Misst | Ergebnis |
|---|---|---|---|
| `unit_r36_zombieki_wecker` | 1 | Weck-Zensus 45 stehende Zombies, Leon 800 vor der Nase | gruen (unveraendert) |
| `unit_r36_zombieki_szenen` | 1 | Griff/Abwehr/Schuss ROOM1010, erneuter Angriff | gruen |
| `unit_r36_zombieki_sicht` | 1, M1, M3 | Abnahme-Fall ROOM1010 (vor NB1 verdeckt / NB1 frei); Gegenproben vor NB1 (24 Moebel-Plaetze) und vor R36 (267 Slab-Plaetze); Zensus 45 Zombies / 3360 Plaetze in BLK-Zonen < 0x1388: 0 frei, 0 nur Moebel, 49 = 49 Wand | 10/10 |
| `unit_r36_zombieki_eintritt` (NEU) | 1, M1, M2 | 78 Tueren x Zombie x steht/hin (266 Faelle): kein stehender Zombie passiv; Kriecher lauert und greift (Bild 2); ROOM10B0 Terrasse Gegenprobe 13025 -> mit Haken 3643 | 5/5 |
| `unit_r36_zombieki_affen_block` | 2, M4-M6 | 16 Seeds AUS/AN je eigener Prozess (28653 -> 721, schlechtester +194, Griffe 73 -> 80) + 3 Befund-Szenen | 11/11 |
| `unit_r36_zombieki_affen` | 3 | Gorilla-Leiche: Record 1700/1800, Lache 0x00ffff38 +8 x 91 | 9/9 (unveraendert) |

### NB1 OFFEN (ersetzt "OFFEN" oben)
- **Taschenfall ROOM1010 Slot 1 (Innenraum, Normalfluss):** nach dem Griff traegt der RE2-Rueckstoss (EXEC[3] P8:
  Keyframe-Wurzeldelta + `-30` @0x80102CA0-AC, ~300 Einheiten/Bild) den Zombie in die Ueberlappung #13/#15/#3/#5, wo die
  Klemme (FUN_8003b0a4-Zwilling, einmal je Bild nach dem KI-Takt) keine freie Lage findet; er tritt ab F780 auf der
  Stelle. Messweg: `test_r36_zombieki_eintritt spur 1010 1020 0 1 1 900 30` + `raster 1010 1 250`. Weder RE2 noch RE1.5
  haben einen Ausweg aus einer solchen Ueberlappung; naechster Weg: den P8-Wurf in Teilschritten <= Radius klemmen
  (verhindert das Eintunneln) und gegen die Spur-Riegel der RE2-Zombies (Griff/Sturz bildgenau) messen. Einziger Fall
  im Eintritts-Zensus (266 Faelle).
- **ROOM10B0:** die Terrassen-Zombies kommen nicht herunter — per Daten ausgeschlossen (SCA #9 Band 1 u0=0xff ueber der
  Treppe #18, Treppen-AOT Pool-Maske 1 = nur Spieler @0x80042c84); sie stehen jetzt vor Leon an der Kante (M2).
- **Moebelhoehen anderer Raeume:** die Messtabelle traegt nur ROOM1010 (der einzige Stage-1-Raum, in dem der Sicht-Zensus
  Moebel-Blocker fand); Obj_model_set-Belege wirken in allen Raeumen. Messweg fuer weitere: `nb1_hoehe_1010.py` mit
  `probe_r34n_e_dokumente kamera <raum>`.
- Gorillas: 3 von 52 Seeds leicht schlechter (+34..+194 Bilder); Rest-Blockade 2380 Bilder (Sub 4 1311, Sub 3 1033).

### NB1 Fuer den Nutzer (ersetzt "Fuer den Nutzer" oben)
- Neue Assets: keine. Sprachdateien: keine. Bedienhinweise: keine. Entscheidungsfragen: keine.
- Spuerbar neu: (1) **Zombies** sehen Leon ueber niedrige Moebel (Verhoertisch, Stuehle, Tische mit Gegenstaenden darauf)
  wie in RE2 und kommen um den Tisch herum; Waende nehmen weiter die Sicht. (2) Die vier **Zombies auf der Terrasse
  (Vorhof 10B0)** laufen nicht mehr ans andere Ende, sondern stellen sich an der Kante vor Leon (herunter koennen sie
  weder in RE1.5 noch in RE2). (3) **Monkeys 11C0**: ein Gorilla, der an Wagen oder in der engen Gasse festhaengt,
  sucht jetzt den Weg um die Wagen bzw. springt ueber das Pad auf Leons Seite (der Sprung des Originals); im
  Zufallsgang-Test sinkt die Zeit, die Gorillas feststecken, um 97 %. (4) Gorilla-Leiche: Blutlache (unveraendert erfuellt).
- Hinweis aus Abnahme 0: der lebende Gorilla wirft den Record-Schatten 1700/1800 (disassemblierter INIT) — beim Spieltest
  ansehen.

### NB1 Messung nachher in der exe (Zusammenfassung)
- ROOM10B0 (Abnahme-Kommandozeile, 1500 Bilder, `F_belege/nb1_exe_10b0_terrasse.txt`): neu alle vier Terrassen-Zombies
  Reaktion F7, Ende vor Leon an der Kante (x -18468 / -17669 / -16868 / -16031, z -11832, Abstand 3334..3641);
  v0.8.23 keine Reaktion (Sub 0, Abstand 3463..6865); Stand Abnahme 0: Reaktion F7, Ende 10713..13013 im Osten.
- ROOM1010 Abnahme-Fall (`F_belege/nb1_exe_1010_abnahmefall.txt`): Slot 2 Reaktion F10, Griff F300.
- ROOM11C0 Kampf-Layout (`F_belege/nb1_affen_exe_kampflayout.txt`): Slot 2 Angriff F896 (v0.8.23 nie), Slot 3 F606.

### NB1 Gezielte Tests (keine Voll-Suite im Spurbaum)
- `ctest -R "affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|1140|1210|1100|10d0|10b0|r35_raeume|r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|r34n_f|r36_zombieki|nav" --timeout 600 -j 6`
  -> **100% tests passed, 0 tests failed out of 154**
- nach dem letzten Neubau `ctest -R "r36_zombieki" --timeout 600 -j 6` -> **100% tests passed, 0 tests failed out of 6**
- Gorilla-Gruppe `ctest -R "unit_r35_affen|unit_maggot|unit_r36_zombieki_affen$|r17_waffen_loop"` -> 100% tests passed, 0 tests failed out of 25

---

## Nachbesserung 2 (nach Abnahme 1, 2026-10-10) — fortlaufend

Grundlage: `F_abnahme_1.md` (Stand 7a9a0e7f, NICHT BESTANDEN, Maengel 1-6). Plan je Mangel:

| Mangel | Kern | Weg |
|---|---|---|
| 1 | Taschenfall ROOM1010 Slot 1: nach Griff + Rueckstoss Gang auf der Stelle (2934,1917), Riegel druckt KLEMME nur | Rueckstoss Bild fuer Bild messen (Lage, Clip, Wurzeldelta), RE2 EXEC[3] P8 @0x80102CA0-AC + Klemme des Originals lesen; Ursache des Eintunnelns belegen, beheben; Riegel KLEMME == 0 fuer den Fall |
| 2 | Regression eab5925a: jede Obj_model_set-Lage im Zell-Rechteck = Oberkante -> Waende werden Moebel (ROOM1040 #4) | Oberkante nur fuer Moebel-Zellen mit Beleg je Zelle; Riegel: 0 Plaetze hinter Waenden geoeffnet, ROOM1040 Westgang Sicht 0 |
| 3 | Befundstelle (-24000,10000): Gorilla 1 Endlos-Blindsprung, Gorilla 2 Sub 4 mit Sicht an der Klemme | messen, Ursache je Gorilla, beheben |
| 4 | Riegel-Definition zaehlt Spruenge/Pendeln nicht; Stelle nicht gepinnt; Szenen parken Gorilla 2 | Anker 400 (wie Abnahme), Stelle (-24000,10000) mit beiden Gorillas pinnen |
| 5 | -2514 / -2000 ohne Beleg | aus Daten herleiten (PLD-Knochen / Kamera+Bild) mit Byte-Offset, sonst als PORT-WAHL kennzeichnen |
| 6 | Voll-Suite | Orchestrator; hier gezielte Laeufe |

### NB2 Mangel 1 — Taschenfall ROOM1010 Slot 1
- **Messung vorher** (Stand 7a9a0e7f, `test_r36_zombieki_eintritt spur 1010 1020 0 1 1 140 1` + `RE15_COLL_TRACE=1`,
  `F_belege/nb2_m1_spur1010_vorher.txt`): Griff F26, P5 ab F107, P7 F114, **P8 ab F115**; Lage je Bild (3541,3990) ->
  (3539,3803) -> (3537,3563) -> (3530,3276) -> (3463,3012) -> (3292,2842) -> (3167,2716) -> (3065,2615) -> (2969,2519) ->
  (3023,2573) -> **(2955,1895)** (F125) -> ab F129 Sub 5, danach Sub 1 Gang auf der Stelle bei (2934,1917) bis Laufende.
  Klemmen-Spur F124->F125: Lage nach dem Wurfschritt (3031,2336) -> Kreis #13 (t3, Mitte (2450,2800) r 600) radial auf
  (3234,2174), also UNTER die Mitte z 2200 der Raute #15 (t2 = Raute 3050..4650 / 1400..3000, Mitte (3850,2200)) -> #15
  schiebt im unteren linken Quadranten nach SUEDWEST auf (2955,1895). Die Tasche zwischen #3 (z 1450..1550), #5
  (x -50..2450), #13 und #15 hat fuer Radius 400 KEINE stabile Lage (aufgeblasener Kreis und aufgeblasene Raute
  ueberlappen an der Rautenspitze (3050,2200)); jedes Bild kaempfen #3/#13/#15 (Spur (2954,1866) -> #3 (2954,1968) ->
  #13 (2968,1945) -> #15 (2937,1914)).
- **Beleg RE2 (Ziel)**: P8 @0x80102C60: `jal 0x80015e7c` (Wurzeldelta), `jal 0x8002959c` (a3 = 512), `lhu a2,324(s1)` /
  `addiu a2,a2,-30` / `jal 0x800152c8` / `sh a2,324(s1)` @0x80102C98-AC (EMZ0.BIN, selbst gelesen). Die Klemme ruft die
  Zombie-Wurzel EINMAL je Bild nach dem Zustand: `jal 0x8003567c` @0x80100638 -> FUN_8003567c -> FUN_8004c1bc
  (RE2_Quellcode_V2: Satz fuer Satz, je Form ein Schieber, kein Rueckfall auf die Vorlage). Die Keil-Lage folgt aus den
  RE1.5-Raumdaten (Kreis und Raute 600 auseinander) unter dem RE2-Wurf (140..290 Einheiten je Bild).
- **Beleg RE1.5 (Form des Rueckfalls)**: die Original-Klemme faellt im Kontaktcode-0-Zweig des Diagonal-Schiebers
  LAB_8003cb9c auf die Vorlage zurueck: `lh v0,0(s4)` / `sw v0,0(t2)` / `lh v1,4(s4)` / `clear v0` / `sw v1,8(t2)`
  @0x8003ced4-ceec (ghidra1_V2.txt, selbst gelesen; s4 = Vorlage, t2 = Lage; Port push_diag6/push_diag7).
- **Aenderung (PORT-WAHL, Nutzer "Das darf so nicht sein")**: `re15_r36_wurf_klemme` (zombieki_r36.c); Haken
  enemy_ai_common.c 1 Zeile direkt nach der Zombie-Klemme `re15_enemy_sca_clamp` (Original-Reihenfolge @0x8010062c):
  NUR waehrend EXEC[3] P7/P8 (+0x5 = 3, +0x6 = 7/8): verschiebt die Klemme die eben geklemmte Lage erneut (keine stabile
  Lage), bleibt der Zombie auf der Lage vor dem Bild. Keine neue Zahl.
- **Nachher** (`F_belege/nb2_m1_spur1010_nachher.txt`): der Wurf endet bei (3463,3012) -> (3434,2983) VOR dem Keil, Sturz,
  F300 Gang, **F360 erneuter Griff**, F450 zweiter Wurf (wieder vor dem Keil), F1320 dritter Griff — 0 Bilder Gang auf
  der Stelle.
- **Riegel** `unit_r36_zombieki_eintritt`: Gegenprobe Wurf-Klemme aus = Befund (Griff Bild 26, 450 Bilder auf der Stelle
  bei (2934,1916)); mit Wurf-Klemme laengster Stillstand aufrecht 0, kleinster Abstand ab Griff+300 = 85. KLEMME zaehlt
  nur noch AUFRECHT (Sub 0/1) NACH der Reaktion (Sub 5 = RE2-Knockdown liegt, Abnahme 1 Abschnitt 2.3) und ist jetzt
  **gepinnt: KLEMME 0** (266 Faelle, PASSIV 0).

### NB2 Mangel 2 — Waende wurden zu Moebeln (Regression eab5925a)
- **Messung vorher** (neue Sonden-Modi von `test_r36_zombieki_sicht`, `F_belege/nb2_m2_*`): `oberkante` — die
  NB1-Abbildung gab 961 von 17700 Nibble-3-Saetzen (99 verschiedene Zellen) eine Oberkante. Die Modelle zeigen, was dort
  "ruht": ROOM1040 #4 drei SENKRECHTE Tafeln (MD1 nv4, x -1800..1800, y -2232..0, **z 0..0**), ROOM2000/2090 waagrechte
  Flaechen (y 0..0, 7200 x 32400 bzw. 21600 x 16200), ROOM1200 #0 Kuehlfach-Tueren in zwei Hoehen (-2858 / -1818).
  `durchwand` (alle 240 RDTs, 182 Zombies, 72900 begehbare Plaetze in BLK-Zonen < 0x1388, Raster 250, je Platz Sicht
  ohne/mit Hoehenregel): die NB1-Abbildung oeffnet 1829 Plaetze, **1814 davon ueber die Wand ROOM1040 #4**, der Rest ueber
  ROOM1010 #5 (Tisch), #12 (Stuhl) und ROOM1220 #18 (Bett).
- **Ursache**: `re15_r36_zelle_oberkante` nahm jedes Weltraum-Obj_model_set im Zell-Rechteck als Standflaeche — ohne
  Original-Gegenstueck (RE2 liest die Hoehe aus dem Satz selbst @0x80050b34-74; RE1.5-Saetze haben keine).
- **Aenderung**: Oberkante NUR fuer die Zellen der Beleg-Tabelle `k_r36_moebel` (Zell-Rechteck, Raum ohne
  Varianten-Nibble), je Zelle mit Beleg (Header + Code): ROOM1010/1011 Tisch #5 = -1600 (Obj_model_set id0 @ROOM1010.RDT
  0x00930, y-Bytes @0x0093c = c0 f9; id1 @0x00952 / @0x0095e; ROOM1011 @0x008ee/@0x008fa, @0x00910/@0x0091c; MD1 y -489..0
  / -72..0 = ruht auf der Platte); Stuhlkreise #12/#13/#14 = -2000 (PORT-WAHL Messwert, Mangel 5); ROOM1220/1221 Betten
  #18/#21/#34 = -1000 (Obj_model_set @0x00e72/@0x00eb6/@0x00e94, y-Bytes @0x00e7e/@0x00ec2/@0x00ea0 = 18 fc; MD1 nv40
  y -176..-1 = ruht darauf; Zellentrakt je Zelle Bett 1950/2125 x 3650..4150 + WC-Kreis 1200 + Becken 1375 x 1000,
  Zellwaende 200). Jede andere Nibble-3-Zelle hat volle Hoehe (RE1.5-Datenlage). Die NB1-Abbildung bleibt nur als
  Sonden-Gegenprobe (`re15_r36_zombie_hoehe_enable(2)`).
- **Nachher**: `durchwand` NB2: 74 geoeffnete Plaetze, **0 ueber ROOM1040 #4**, alle ueber Beleg-Zellen (1010 #5 52,
  #12 59, 1220 #18 8 — je Platz jede Zelle einmal gezaehlt). ROOM1040 Leon im Westgang (-29900,6000): Ostgang-Zombies
  NB1 2/2 "sehen", NB2 0/2.
- **Riegel** `unit_r36_zombieki_sicht` 15/15: zusaetzlich Gegenprobe NB1 (1829 / 1814 ueber #4), NB2 0 ueber #4 + nur
  Beleg-Zellen, ROOM1040-Westgang 0/2 (Gegenprobe 2/2); die NB1-Pruefungen (Abnahme-0-Fall, 49 = 49 hinter Waenden, 0
  hinter Moebeln, 0 bei freier Sicht) bleiben gruen.

### NB2 Mangel 5 — Belegform -2514 / -2000
- **-2514 (Leons Kopf)**: die Konstante RE15_R36_LEON_KOPF_Y ist entfernt. Das Spiel liest Leons Kopf nur aus head_world
  (Plc_neck-Ursprung, skeleton_common.c: hm = Wurzel-Rot * relpos[8] + Wurzel-Trans, je gezeichnetem Bild; neck_bone = 8
  @0x80031938). Ohne head_world gibt es keinen Hoehenvergleich: die Zelle blockt (RE1.5-Datenlage). Die Sonden setzen
  head_world mit DERSELBEN Formel aus PLD/PL00.EMR + PL00.EDD (neu `tests/unit/test_r36_zombieki_kopf.h`):
  `test_r36_zombieki_sicht kopf` -> Clip 0 Bild 0 Kopf (-51,**-2513**,0); Clips 11/17/19/20/21 -2514 — der NB1-exe-Wert
  (-2514) war derselbe Datenwert.
- **-2000 (Stuhlkreise)**: als **PORT-WAHL (Messwert)** gekennzeichnet, mit Herleitung im Code: Projektion mit den
  Engine-Matrizen (`probe_r34n_e_dokumente kamera 1010`, Kamera-Record Cut 0 @ROOM1010.RDT 0x60) auf das Hintergrundbild
  Cut 0, geeicht an der Tischplatte (Ecken y -1600 liegen auf den Tischkanten); die Marke y -2000 der Stuhlsaeule liegt
  auf der Wand ueber der Lehne -> Oberkante tiefer, eingetragen als OBERGRENZE (`F_belege/nb1_r1010_c0_hoehen.png`,
  `F_belege/nb1_hoehe_1010.py`). Die RE1.5-Daten tragen keine Stuhlhoehe; ohne die Zahl blockten die Stuehle wie Waende
  (Abnahme-0-Fall Slot 2 stand dann wieder).

### NB2 Mangel 3 — Befundstelle (-24000,10000) weiter blockiert
- **Messung vorher** (Stand 7a9a0e7f, `test_r36_zombieki_affen_block fest -24000 10000 2400`, `F_belege/nb2_m3_fest_24000_vorher.txt`):
  bitgleich zur Abnahme — Gorilla 1: 53 Spruenge (48 blind), Sub 7 2032/2400, Abstand min 16125, kein Angriff; Gorilla 2:
  Sub 4 1850/2400 mit Sicht an der Klemme bei (-321,12280), kein Angriff.
- **Ursache (gemessen)**: Raster der Original-Klemme (`raster 800/400/100 1600`): Leon steht in der NORDWEST-TASCHE
  (x ~ -29800..-23400, z ~ 4800..15600), die fuer Radius 1600 eine EIGENE Komponente ist — sie beruehrt den Nordstreifen
  nur an einer Spitze bei (-25850,15750), kein 0x10-Pad fuehrt hinein. Der NB1-Umweg suchte nur ein Pad direkt zu Leons
  Komponente, fand keins und schaltete ab: Gorilla 2 drueckte (Original B[4] auf playerX @0x801181e0) in die Wand bei
  (-321,12280) — Path B (Weitsprung) verlangt `+0x1d6 == 0` @0x80118054-5c und bleibt an der Klemme zu; Gorilla 1 landete
  nach dem Pad-Mitte-Sprung (NB1-Regel, Code 0xf00 -> Gier 3840) bei (-13418,-316) mit Blick nach Osten auf das
  0x20-Pad #19/#55 (x -11900..-9300) -> Original-Blind-Sprung `ori v0,zero,0x3` / `sb v0,7` @0x80117fe4-80118024 in die
  Wagen und zurueck, endlos (dieselbe Schleife, die `unit_r35_affen_zonensprung` als Original pinnt).
- **Beleg, dass die Tasche angreifbar ist**: neue Sonde `weit <gx> <gz> <lx> <lz>` (Path-B-Weitsprung +0x5 = 7, +0x7 = 0
  @0x801180cc-f8 erzwungen): von (-20000,14500) / (-15000,12000) / (-26000,16000) landet der Gorilla 2050 vor Leon und
  greift an (Sub 6 -> Sub 15); zu Fuss erreicht er den Taschenrand bei (-22600,13200) (Raster), 3493 vor Leon.
- **Aenderung (PORT-WAHL, Erweiterung des NB1-Umwegs)**: `r36_uw_route` (zombieki_r36.c): Breitensuche ueber die
  Komponenten (0x10-Pads als Kanten, auch mehrere Spruenge). Ziel = Leons Knoten, wenn erreichbar, SONST der erreichbare
  Knoten, der Leon am naechsten liegt; der erste Sprung auf dem Weg ist der Original-Sprung (wie NB1). Unter 0x1771 schaltet
  decide wie bisher ab (Original-Nahlogik, Spur-J-Riegel bildgenau). Dazu (gemessen beim Pinnen mit BEIDEN Gorillas,
  Mangel 4): zwei Gorillas im Koerperkontakt (Radien 2 x 1600 @0x80121356 + ein Rasterschritt) auf demselben Pad schoben
  sich 1200 Bilder fest (Szene 2: beide auf Pad #36 bei (4806,-8856)/(6749,-11275)) -> der dem Pad naehere behaelt es, der
  andere nimmt ein anderes (`r36_pad_belegt`). Keine neue Zahl. Verworfen (gemessen): (i) Umweg auch im Nahbereich an der
  Klemme mit versperrtem Koerperweg -> `unit_r35_affen_griff` + `_szene` rot (Spur-J-Original-Spuren) — zurueckgenommen;
  (ii) Pad-Wahl nach naechstem Anlauf + Belegung ohne Kontakt-Bedingung -> 52 Seeds 22817..23186 statt 15857.
- **Nachher**: Sonde (beide Gorillas Original-Lage F195, je Lauf eigener Prozess): Angriff Bild **597 / 929**, Abstand
  min 955 / 954, **0 Blind-Spruenge**; Gegenprobe (Umweg aus) kein Angriff. exe (`re15_pc_nb2f.exe`, Abnahme-Kommandozeile
  `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-24000,10000,1024,0`, `F_belege/nb2_exe_messungen.txt`):
  **Slot 3 Angriff F850, Slot 2 F1912**, Abstand min 761 / 954, 0 Blind-Spruenge (NB1: nie, 1519 Bilder Sprungschleife).
- **Rest (OFFEN, Original-Nahlogik)**: nach den ersten Angriffen wird Leon tiefer in die Tasche gestossen; der Gorilla
  wartet dann in Sub 3 am Taschenrand (-23302,13100) an der Klemme, 3900..4500 vor Leon (A[3] unter 0x1771 ohne Ausgang,
  Biss nur unter 0xbb8 @0x80117b60-68).

### NB2 Mangel 4 — Riegel misst die Nutzer-Form "blockiert"
- **Aenderung `unit_r36_zombieki_affen_block`**: Blockade mit **Anker 400** (Manhattan; Spruenge/Pendeln auf der Stelle
  zaehlen, `eins ... [anker]` fuer Vergleiche); Szenen mit **BEIDEN** Gorillas (`szene2`, keiner geparkt; je Szene und
  Modus ein eigener Prozess — im selben Prozess lieferte die Gegenprobe der Szene 4 nach Szene 3 Angriffe, Modulzustand);
  **Szene 4 = Befundstelle (-24000,10000)** gepinnt: Gegenprobe kein Angriff, Umweg BEIDE greifen an, keine Blind-Schleife.
- **Zufallsgang** (`F_belege/nb2_m4_52seeds_final.txt`, je Lauf eigener Prozess, AUS = Original-Regel):

| Messung | AUS | NB1 (Abnahme 1) | **NB2** |
|---|---|---|---|
| 52 Seeds, Anker 400 | 114029 | 27597 (7 schlechter, +509) | **15857** (-86 %; 3 schlechter, schlechtester 0x179803a8 +825, Nahjagd) |
| 52 Seeds, Anker 40 | 75408 | 2380 (3 schlechter, +194) | **1329** (2 schlechter, +196) |
| 16 Seeds (Riegel), Anker 400 | 38439 / 73 Griffe | — | **4568 / 76** (0 schlechter, -100) |

  Rest (Anker 400, 52 Seeds): 13794 von 15857 Bildern NAH (< 6001) in Sub 3 an der Klemme, 11729 mit Sicht — Gorilla sieht
  Leon ueber eine Wagen-Diagonale (Nibble 2, blockt keine Sicht), drueckt in der Original-Nahjagd dagegen (B[3] playerX
  @0x80117d4c) bis Leon weitergeht. Jede gemessene Ausdehnung des Umwegs in den Nahbereich brach Spur-J-Riegel (s. oben).
- **Riegel 17/17** (`F_belege/nb2_m4_affen_riegel.txt`): 16 Seeds AUS 38439 -> AN 4568 (<= 1/6), schlechtester -100
  (<= 600), Griffe 76 >= 80 % von 73; Szene 1 Angriff 140/425, Szene 2 898/1275, Szene 3 929/218, Szene 4 (Befundstelle)
  597/929, je Gegenprobe kein Angriff, keine Blind-Schleife.

### NB2 Mangel 6 — Voll-Suite
Im Spurbaum verboten (Auftrag); der Orchestrator faehrt sie EINMAL im Integrationsbaum. Hier gezielt (unten).

### NB2 Messung nachher in der exe (Zusammenfassung, `F_belege/nb2_exe_messungen.txt`, Kopie `re15_pc_nb2f.exe`)
- **M1 ROOM1010 Abnahme-Lage** (`RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3750,4300,1024,0 RE15_AI_FLAVOR=re2`): Slot 1 Griff F30,
  Wurf endet bei (3433,2983) VOR dem Keil, Sub 5 (liegt) F120-F420, F450 Gang, **F480 erneuter Griff**, F540 Sub 6 (frisst Leon).
  Vorher (Abnahme 1): nach dem Griff Gang auf der Stelle bei (2934,1917), 540-840 Bilder, 2172..2815 vor Leon.
- **M2 ROOM1040, Leon im Westgang** (`RE15_DEBUG_JUMP=1040@250 RE15_PLAYER_POS=-29900,6000,0,0`): Sicht-Bit Ostgang Slot 1 / 2
  **0/900 und 0/900** (NB1: 895/900 und 396/900; v0.8.23: 0/900); Westgang Slot 3/4/5 895/894/895 (sehen Leon zu Recht).
- **M3 ROOM11C0 Befundstelle** (`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-24000,10000,1024,0`): **Slot 3
  Angriff F850, Slot 2 F1912**, 0 Blind-Spruenge (NB1: keiner greift an, Slot 2 1519 Bilder Sprungschleife).

### NB2 Tests (probes/r36_zombieki.cmake)
| Test | Mangel | Misst | Ergebnis |
|---|---|---|---|
| `unit_r36_zombieki_eintritt` | M1 | 266 Faelle: PASSIV 0, **KLEMME 0** (aufrecht nach der Reaktion); Taschenfall 1010<-1020 Slot 1: Gegenprobe 450 Bilder auf der Stelle, mit Wurf-Klemme 0 und Abstand ab Griff+300 = 85; Kriecher; ROOM10B0 | 9/9 |
| `unit_r36_zombieki_sicht` | M2, M5 | Durchwand-Zensus aller Stages: Gegenprobe NB1 1814 Plaetze ueber ROOM1040 #4, NB2 0, gesamt 74 nur ueber Beleg-Zellen; ROOM1040-Westgang 0/2 (Gegenprobe 2/2); Leons Kopf aus PL00; NB1-Pruefungen unveraendert | 15/15 |
| `unit_r36_zombieki_affen_block` | M3, M4 | 16 Seeds Anker 400 (38439 -> 4568, 0 schlechter); 4 Szenen mit beiden Gorillas je eigener Prozess, Szene 4 = Befundstelle (-24000,10000): beide greifen an, keine Blind-Schleife, Gegenprobe kein Angriff | 17/17 |
| `unit_r36_zombieki_wecker`, `_szenen` | M5 | Kopf aus PL00 im Bildtakt (wie der Renderer); Weck-Zensus / Szenen unveraendert | 2/2, 5/5 |
| `unit_r36_zombieki_affen` | P3 (erfuellt) | Gorilla-Lache, unveraendert | 9/9 |

**Gezielte Laeufe (keine Voll-Suite im Spurbaum)** — Module/Raeume des Diffs (enemy_ai_common.c Zombie-Klemme + Gorilla-Haken,
zombieki_r36.c, Zombie-/Gorilla-/Raum-Riegel):
- `ctest --test-dir re15_port/build -R "affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|1140|1210|1100|10d0|10b0|1010|1220|r35_raeume|r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|r34n_f|r36_zombieki|nav|unit_enemy_|room1070_wake|ai_flavor|re2_room1090|ada_eintritt|r17_waffen_loop|schatten|shadow|grab|griff|victim" --timeout 600 -j 6 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 163** (510,6 s)
- nach dem letzten Neubau `ctest --test-dir re15_port/build -R "r36_zombieki" --timeout 600 -j 6` -> **100% tests passed, 0 tests failed out of 6**
- Gorilla-Gruppe `ctest -R "unit_r35_affen|unit_maggot|r17_waffen_loop|unit_r36_zombieki_affen$"` -> 100% tests passed, 0 tests failed out of 25

### NB2 OFFEN
- **Gorillas, Nahjagd an Wagen-Diagonalen**: der Rest der Blockade (52 Seeds, Anker 400: 13794 von 15857 Bildern) ist die
  Original-Nahjagd (Sub 3 unter 0x1771, Gorilla sieht Leon ueber eine Wagen-Diagonale mit Nibble 2, drueckt mit B[3] auf
  playerX @0x80117d4c dagegen, bis Leon weitergeht). Jede gemessene Ausdehnung des Umwegs in den Nahbereich brach Spur-J-
  Riegel (unit_r35_affen_griff/_szene, Original-GDB-Spuren). Naechster Weg: GDB-Messung des Originals an einer
  Wagen-Diagonale (Muster J_affen.md), ob das Original dort ebenso steht — dann ist es RE1.5-Verhalten.
- **Befundstelle nach den Angriffen**: wird Leon tiefer in die Nordwest-Tasche gestossen (unerreichbar fuer Radius 1600),
  wartet der Gorilla am Rand (Sub 3, 3900..4500 vor Leon) — dieselbe Original-Nahlogik.
- **Zufallsgang 52 Seeds**: 3 Seeds schlechter als die Original-Regel (schlechtester 0x179803a8 +825, Nahjagd); Griffe 226 -> 217.
- **Moebel-Tabelle**: deckt die Zellen, ueber die der Durchwand-Zensus (alle 240 RDTs, jede Zombie-Startlage, Raster bis
  0x1388) ueberhaupt Sicht oeffnen kann, mit Beleg (ROOM1010 Tisch/Stuehle, ROOM1220 Betten). Weitere Moebel (99 Zellen
  mit Obj_model_set darauf, `F_belege/nb2_m2_oberkante_nb1_alle_rdt.txt`) bleiben volle Hoehe, bis je Zelle ein Beleg
  vorliegt (Messweg: `test_r36_zombieki_sicht oberkante <raum>` + Projektion `F_belege/nb1_hoehe_1010.py`).
- Vorgaenger-OFFEN ROOM10B0 (Terrasse ohne Abstieg, per Daten) unveraendert.

### NB2 Fuer den Nutzer
- Neue Assets: keine. Sprachdateien: keine. Bedienhinweise: keine. Entscheidungsfragen: keine.
- Spuerbar neu: (1) Ein Zombie, den Leon nach dem Griff wegstoesst, faellt nicht mehr in eine Ecke zwischen Stuhl und
  Schrank, aus der er nicht mehr herauskommt (Verhoerraum 1010) — er steht wieder auf und greift erneut an. (2) Zombies
  sehen nicht mehr durch Waende, an denen Gegenstaende haengen (Gaenge 1040); ueber den Verhoertisch und die Betten im
  Zellentrakt sehen sie weiter. (3) Monkeys 11C0: auch wenn Leon in einer Ecke steht, in die ein Gorilla nicht passt,
  laufen beide bis an den Rand und greifen an; sie springen nicht mehr endlos gegen einen Wagen, und zwei Gorillas
  blockieren sich nicht mehr gegenseitig am selben Sprungfeld.

### NB2 Ergebnis je Mangel (Kurzfassung)
| Mangel | Ursache | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| 1 Taschenfall 1010 | RE2-Wurf P8 (@0x80102CA0-AC) + Klemme Satz fuer Satz schiebt den Zombie ueber die Rautenmitte in eine Tasche ohne stabile Lage | Klemmen-Spur F124->F125 (#13 -> #15); Rueckfall-Form @0x8003ced4-ceec | PORT-WAHL Wurf-Klemme P7/P8 | exe erneuter Griff F480; Sonde KLEMME 0, Taschenfall gepinnt |
| 2 Waende als Moebel | NB1-Abbildung "jedes Obj_model_set im Rechteck" (eab5925a) | Durchwand 1814 Plaetze ueber ROOM1040 #4 (senkrechte Tafeln MD1 z 0..0) | Beleg-Tabelle je Zelle (Byte-Offsets) | exe Ostgang 0/900; Sonde 0 ueber #4, 74 nur ueber Beleg-Zellen |
| 3 Befundstelle 11C0 | Leon in Tasche ohne Pad -> NB1-Umweg aus -> Wanddruck / Blind-Schleife | Raster (Komponenten), Weitsprung-Sonde 2050 vor Leon | Ziel = erreichbarer Knoten naechst Leon; Kontakt-Pad-Regel | exe Angriff F850/F1912, Sonde 597/929, 0 Blind-Spruenge |
| 4 Riegel | Anker 40, geparkter Gorilla 2, Stelle ungepinnt | Abnahme-Messung | Anker 400, beide Gorillas, Szene 4, eigene Prozesse | 17/17; 52 Seeds 114029 -> 15857 |
| 5 Belegform | -2514 / -2000 ohne Beleg | PL00 Clip 0 Bild 0 = -2513; Kamera Cut 0 @ROOM1010.RDT 0x60 | Konstante entfernt (head_world); -2000 PORT-WAHL Messwert | Sonden-Kopf aus PL00 |
| 6 Voll-Suite | — | — | Orchestrator | gezielt 163/163 |

---

## Nachbesserung 3 (nach Abnahme 2, 2026-10-11) — fortlaufend

Grundlage: `F_abnahme_2.md` (Stand 4285214f, P1/P3 erfuellt, P2 teilweise, Maengel 1-3). P1/P3 nicht anfassen
(ihre Riegel `unit_r36_zombieki_{sicht,eintritt,wecker,szenen,affen}` gruen halten). Ziel-Kriterium (Orchestrator):
an KEINER gemessenen Stelle (drei Nordwest-Gang-Plaetze, drei Nordwest-Taschen-Plaetze, gepinnter Punkt, zehn
exe-Stellen der Abnahme 2) greift ein Gorilla spaeter an als in v0.8.23, und die Blockade sinkt insgesamt; wo eine
Routen-Erweiterung schadet, enger gaten statt global umsteuern.

| Mangel | Kern | Weg |
|---|---|---|
| 1 | `r36_uw_komp_bfs` ungewichtet: Pad #36 (Ost) vor #47 (West) nach SCA-Reihenfolge -> Nordwest-Gang-Umleitung | Routenwahl nach Weglaenge (Raster-Abstand + Sprung + Abstand ab Landung); Riegel-Szene Nordwest-Gang mit Gorilla-Start (-9013,-15461) |
| 2 | Nordwest-Tasche nur am gepinnten Punkt; Startknoten ohne Erreichbarkeit; Path-B-Weitsprung ungenutzt | Startknoten nur aus der eigenen Komponente; am Rand der unerreichbaren Tasche der Original-Weitsprung (A[4] Path B @0x801180cc-f8); Riegel prueft Blockade auch NACH dem ersten Angriff, drei Taschen-Plaetze |
| 3 | Voll-Suite | Orchestrator; hier gezielt |


### NB3 Mangel 1 — Umweg nach SCA-Reihenfolge statt Weglaenge (Regression a76cd217)
- **Ursache (Code gelesen + gemessen):** `r36_uw_komp_bfs` war eine ungewichtete Breitensuche ueber Komponenten; die erste
  Pad-Kante in SCA-Reihenfolge setzt den Vorgaenger — #36 (Ost, @ROOM11C0.RDT 0x015d8) vor #47 (West, @0x0165c), beide u1 = 0x10.
  Messung vorher = Abnahme 2 (exe Slot 2 F2438/F1939/F1806 statt v0.8.23 F680/F704/F694; Sonde (-14000,2000) Umweg G1 nie, 919
  blockiert). Eigene Gegenmessung v0.8.23-exe (`re15_pc_abn0f_basis.exe`, (-14000,2000)): Slot 2 F680, bitgleich zur Abnahme.
- **Beleg (Sprung = Original):** A[4] Path A FUN_8003b93c(+0x34, box[6]+100, +0x82, attr 0x10) @0x80117ee0-f00, Yaw-Fenster
  @0x80117f18-38, +0x9f / +0x5/6/7 = 7/0/1 @0x80117f40-74 (unveraendert aus NB1/NB2, Abnahme 2 Abschnitt 1 selbst gelesen).
- **Aenderung (PORT-WAHL):** `r36_uw_astern` — A* ueber das Raster (Achtel-Metrik gerade 2 / diagonal 3), die 0x10-Pads als
  Kanten mit ihrer Weglaenge Start -> Landung; der erste Sprung auf dem KUERZESTEN Weg ist der Original-Sprung.
- **Nachher:** Sonde Szene 5 (Leon (-14000,2000), Gorilla 1 ab (-9013,-15461)): kein Gorilla ueber #36, erster Angriff Bild
  352 (Original-Regel 430); (-14000,4000) 350 (458), (-14500,6000) 358 (433). exe: Tabelle unten.

### NB3 Mangel 2 — Nordwest-Tasche
- **Messung 1 — die Plaetze sind fuer Leon nicht begehbar:** Sonde `leonlauf` (echte Spieler-Klemme, Leon rennt 600 Bilder
  auf den Platz zu): von (-23000,14400) Richtung (-26000,7000) / (-27000,12000) / (-24000,10000) endet Leon bei z 11938
  (= #20-Oberkante 11470 + 450 + 0x12), Abstand 5174 / 2548 / 1938; von (-14600,6000) Richtung (-22000,7000) bei x -17322
  (#13/#20-Ostkante -17790 + 468), Abstand 4678. Leon-Raster (Radius 450, Maske 1): x < -17322 / z < 11938 alles Zelle
  (#13 @0x014c4 x -28290..-17790 z ..8112, #20 @0x01518 Typ 8 z 6800..11470, #21 @0x01524 x ..-24920 z 10000..14110).
  `RE15_PLAYER_POS` setzt Leon hinein; exe NB2: (-26000,7000) -> (-26000,6332), (-27000,12000) -> (-26932,9532),
  (-22000,7000) -> (-22000,6332) — alle IN #13/#20. Kein Gorilla kann dort angreifen (Angriffsweite 0xbb8/0xfa0, s. unten).
- **Messung 2 — die "Tasche" im Gorilla-Raster ist ein Artefakt:** Sonde `klemme`: (-25800,6800) -> #13 schiebt auf 9730,
  #20 auf 13088, #21 zurueck auf 6800 = die Vorlage. Beleg FUN_8003bca8 Code-0-Zweig: Vergleich Lage/Vorlage `lw a3,0(a1)` /
  `lh v0,0(a2)` / `bne` @0x8003be38-54, Rueckfall `lh v0,0(a2)` / `sw v0,0(a1)` / `lh v1,4(a2)` / `addu v0,zero,zero` /
  `sw v1,8(a1)` @0x8003be8c-a0 (PSX.EXE, re15_disasm.py). Mit Vorlage = Knoten (NB2) zaehlt jede solche Lage als frei:
  286 von 708 Knoten (Schritt 800), darunter die ganze Tasche (`F_belege/nb3_raster800_vorlage_knoten.txt` gegen
  `..._plus1.txt`); ein in der Tasche abgesetzter Gorilla bewegt sich 300 Bilder nicht (Sonde `fest -26000 7000 300 -26000 11000`).
- **Messung 3 — Startknoten (Abnahme-Teilursache):** bestaetigt; am Rand (-23302,13088) lag der naechste "freie" Knoten
  (-23400,12400) in der Tasche (Artefakt) — mit dem neuen Frei-Test existiert er nicht mehr, zusaetzlich nimmt
  `r36_uw_knoten` nur Knoten, die der Gorilla GERADE erreicht (Strecke frei fuer seinen Radius).
- **Path-B-Weitsprung:** traegt nicht in die Tasche — +0x7 = 0 laesst +0x82 = 0 (`floor = 1` nur bei +0x7 != 0
  @0x80118ab4-af0), die Flugbahn bleibt an Band 0 geklemmt; Sonde `weit -20000 14500 -24000 10000`: Landung am Rand
  (-23302,13088), nicht hinein.
- **Aenderung:** Frei-Test mit Vorlage = Knoten + 1 (`r36_frei_bei`), Kanten = Schritt, den die Klemme in BEIDE Richtungen
  nicht aufhebt (Schraegschieber haengen an der Bewegungsrichtung, Rueckfall @0x8003ced4-ceec), Startknoten gerade
  erreichbar, Leons Knoten auf Leons Seite (Strecke frei fuer die Spieler-Klemme: Radius 450 = PR, Maske 1).
- **Nachher (Sonde, Original-Regel -> Umweg):** Befundstelle (Leon steht auf (-24000,11938)): nie/nie -> 572/360;
  Taschenrand (-24452,11938): nie/nie -> 601/361; (-22288,11938): nie/nie -> 480/340. An den Abnahme-Plaetzen selbst (Leon
  IN den Zellen) greift keiner an — dort ist Leon im Spiel nicht.
- **Blockade NACH dem ersten Angriff:** der Riegel misst sie jetzt je Gorilla (`nach`) und davon FERN (`fern`, Leon >= 0x1771).
  In allen sieben Riegel-Szenen ist die FERN-Blockade nach dem ersten Angriff 0 (Szene 1: 4 Bilder). Was bleibt, ist NAH
  (3900..4600 vor Leon): Leon wird in die Nordwest-Ecke (x -26932..-24452, z 14578..17268) gestossen, die ein Gorilla nur
  ueber den 390 breiten Streifen z 15728..16118 erreicht; einer greift dort an, der zweite wartet. Eine Umlenkung auch NAH
  wurde gemessen (Haenge-Beginn bei Leon > 3000): `unit_r35_affen_griff` rot ("Weg 2: erster Biss T474", Original-GDB kein
  Biss bis T495, e1/e2 ~3925/~4990 an der Klemme) und `unit_r35_affen_szene` rot (5 Fehler, Biss-Takt/Tod gegen das Original)
  — die Spur-J-Riegel pinnen genau diese Nahjagd als Original. Deshalb nur FERN.

### NB3 Umweg-Beginn (beide Maengel): erst wenn der Gorilla haengt
- NB2 startete bei JEDEM Klemmen-Kontakt und lenkte auch Gorillas um, die an der Wand entlang zu Leon rutschten: Sonde Leon
  (6000,-9000) Gorilla 1 Angriff Bild 196 (Original-Regel) -> 284 (sofortiger Beginn mit A*).
- Beleg: der Gorilla zaehlt das Klemmen-Haengen selbst — +0x1de im Gehirn-Schwanz `lhu v0,470(v1)` / `beq` @0x80117448-50,
  +1 @0x80117458-68, sonst 0 @0x8011746c, INIT @0x8011706c; Voll-Scan 0x80116db8..0x8011c600 (STAGE1.BIN ohne Kopf):
  sonst kein Zugriff auf Offset 478 — kein Verbraucher in der Beta.
- PORT-WAHL: der Umweg ist dieser Verbraucher — Beginn, wenn der Gorilla FERN (>= 0x1771 @0x80117bc8) in Sub 3/4 seit 45
  Bildern weniger als 400 vorankommt und an der Klemme steht. 45 = halbe Mess-Definition der Abnahme (90 Bilder im Anker
  400); gemessen 1 / 15 / 30 / 45 (Raster Radius/4): 24353 / 22328 / 25112 / 20084 blockierte Bilder (52 Seeds).
  Ausserdem sofort: in der JAGD (Sub 3) an der Kante eines 0x10-Pads, wenn der kuerzeste Weg ueber dieses Pad fuehrt (A[3]
  hat keinen Sprung @0x80117b40-c74; ohne das lief der Gorilla in die Pad-Mitte, sprang in Richtung 0xf00 und landete vor dem
  0x20-Pad #19 in der Original-Blindsprung-Schleife @0x80117fe4-80118024); Pad-Mitte-Drehen (NB1) nur bei Leon >= 0xbb8
  (@0x80117a60-74; sonst sprang Gorilla 2 2053 vor Leon davon). Gerader Weg zu Leon frei -> Original (sonst verfehlten zwei
  Gorillas 2100 vor Leon auf seinen KNOTEN gelenkt das Biss-Fenster).
- Hook: enemy_ai_common.c Gehirn-Schwanz (1 Zeile, nach den Timern +0x1dc/+0x1e1).

### NB3 Zufallsgang (52 Seeds x 6000 Bilder, je Lauf eigener Prozess; `F_belege/nb3_52seeds_*.txt`)
| Anker | AUS (Original-Regel) | NB2 | **NB3** |
|---|---|---|---|
| 400 | 114029 / 226 Griffe | 15857 / 217 | **17975 / 245** (besser 47, schlechter 3, schlechtester +135) |
| 40 | 75408 | 1329 | **221** (schlechter 0) |
| 16 Seeds (Riegel), 400 | 38439 / 73 | 4568 / 76 | **5725 / 74** (schlechtester +59) |

Anker 40 (Stillstand) faellt um 83 % gegenueber NB2; Anker 400 (Pendeln/Springen im 400er-Fenster) steigt um 2118 Bilder:
NB2s sofortiger Umweg-Beginn holte mehr Gorillas aus der Nahjagd an Wagen-Diagonalen (Rest NB3: nah 14662, Sub 7 2352), lenkte
aber dafuer auch rutschende Gorillas um (oben). Abwaegung im Sinne des Orchestrators ("lieber enger gaten").

### NB3 Messung nachher in der exe (`F_belege/nb3_exe_zehn_stellen.txt`, Kopie `re15_pc_nb3f.exe`)
Kommandozeile der Abnahme 2 (`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700`), keine
Eingabe. Auswertung eigenes Skript (erster Angriff = Sub 5/6/8/15; fest = aktiv, Sub != 0, kein Angriff, >= 90 Bilder im Anker
400, Leon > 3000); Eichung an v0.8.23 (-14000,2000): Slot 2 F680, Slot 3 fest 991 (Abnahme 2: F680 / 930). v0.8.23 und NB2 aus
der Abnahme-2-Tabelle.

| Leon | v0.8.23 Slot 2 / Slot 3 | NB2 Slot 2 / Slot 3 | **NB3 Slot 2 / Slot 3** (fest) |
|---|---|---|---|
| (-7000,12500) | nie / F1012 | F1514 / F606 | **nie / F730** (863 / 0) |
| (-14000,2000) NW-Gang | F680 / nie | F2438 / F1600 | **F680 / nie** (0 / 0) |
| (6000,-9000) | nie / F324 | F1246 / F339 | **nie / F324** (1654 / 1874) |
| (-24000,10000) Befundstelle | nie / nie | F1913 / F851 | **F853 / F1570** (0 / 1086) |
| (7000,4000) | nie / F18 | nie / F18 | **nie / F18** (97 / 0) |
| (9000,10000) | nie / F309 | F1045 / F309 | **F1175 / F309** (0 / 0) |
| (-14746,-4140) West-Pad | F364 / nie | F364 / F1186 | **F364 / F1021** (174 / 0) |
| (7200,-4130) Ost-Pad | F1246 / F274 | F981 / F274 | **F1050 / F274** (0 / 468) |
| (-14000,4000) NW-Gang | F704 / nie | F1939 / F1577 | **F704 / F1496** (244 / 0) |
| (-14500,6000) NW-Gang | F694 / F1256 | F1806 / F1534 | **F694 / F892** (0 / 0) |
| (-26000,7000) Tasche* | nie / nie | nie / nie | nie / nie (1754 / 584) |
| (-27000,12000) Tasche* | nie / nie | nie / nie | nie / nie (1765 / 567) |
| (-22000,7000) Tasche* | nie / nie | nie / nie | nie / nie (1927 / 726) |

\* Leon steht dort IN den Zellen (exe F300: (-26000,6332) / (-26932,9532) / (-22000,6332)), im Spiel nicht erreichbar (oben).

- **Kein Gorilla greift an einer der zehn Stellen spaeter an als in v0.8.23** (NB2: fuenfmal an vier Stellen spaeter, u. a. Nordwest-Gang
  F2438/F1939/F1806 und Slot 3 (6000,-9000) F339 > F324). Nordwest-Gang Slot 2 = v0.8.23 (F680/F704/F694), Slot 3 greift dort neu
  an (F1496/F892). Befundstelle beide.
- fest an den zehn Stellen: v0.8.23 14726 (Abnahme-Skript) -> NB2 2662 -> **NB3 6460** (eigenes Skript). Der Rest ist NAH:
  (6000,-9000) Slot 2 d ~5400 / Slot 3 d ~3100 nach seinem Angriff, Befundstelle Slot 3 in der Nordwest-Ecke nach dem Angriff,
  (-7000,12500) Slot 2 am Nordende des Nordwest-Gangs 3560 vor Leon (Wagenblock #2 dazwischen) — Original-Nahjagd, die die
  Spur-J-Riegel pinnen (oben). NB2 hatte dort weniger, weil sein sofortiger Umweg-Beginn die Gorillas auf anderen Wegen
  heranfuehrte — um den Preis der Verspaetungen gegenueber v0.8.23.

### NB3 Tests (probes/r36_zombieki.cmake)
| Test | Mangel | Misst | Ergebnis |
|---|---|---|---|
| `unit_r36_zombieki_affen_block` | M1, M2 | 16 Seeds Anker 400 (38439 -> 5725, schlechtester +59, Griffe 73 -> 74); 7 Szenen mit beiden Gorillas je eigener Prozess: Gegenprobe Original-Regel, erster Angriff nicht spaeter, angreifende Gorillas, KEINE FERN-Blockade nach dem ersten Angriff, keine Blind-Schleife; Szene 5 Nordwest-Gang (kein Pad #36, 352 statt 430); Szenen 6/7 Taschenrand; 4b Tasche nicht begehbar (leonlauf 4 Plaetze) + Raster ohne Zurueckschnapp-Knoten (87 -> 0) | 46/46 |
| `unit_r36_zombieki_sicht`, `_eintritt`, `_wecker`, `_szenen`, `_affen` | P1/P3 (erfuellt) | unveraendert | gruen |
| Spur-J `unit_r35_affen_*` (20), `unit_maggot_*` (3), `unit_r17_waffen_loop_pin` | Nahjagd/Original | bildgenau gegen GDB | gruen |

**Gezielte Laeufe (keine Voll-Suite im Spurbaum):**
- `ctest --test-dir re15_port/build -R "affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|1140|1210|1100|10d0|10b0|1010|1220|11c0|r35_raeume|r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|r34n_f|r36_zombieki|nav|unit_enemy_|room1070_wake|ai_flavor|re2_room1090|ada_eintritt|r17_waffen_loop|schatten|shadow|grab|griff|victim" --timeout 600 -j 6 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 163** (508,3 s; `F_belege/nb3_ctest_gezielt.txt`)
- nach dem letzten Neubau `ctest --test-dir re15_port/build -R "r36_zombieki|unit_r35_affen|unit_maggot|r17_waffen_loop" --timeout 600 -j 6`
  -> **100% tests passed, 0 tests failed out of 30**

### NB3 OFFEN
- **Nahjagd (Leon < 0x1771):** der Rest der Blockade (exe zehn Stellen 6460, Zufallsgang 52 Seeds nah 14662 von 17975) ist die
  Original-Nahjagd an Klemmen (Wagenblock/Ecke, Sub 3 auf playerX @0x80117d4c). Jede gemessene Umlenkung NAH brach die
  Spur-J-Riegel griff/szene (GDB-Spuren des Originals: e1/e2 warten ~3925/~4990 vor Leon an der Klemme, kein Biss bis T495).
  Naechster Weg: GDB-Messung des Originals an der Nordwest-Ecke / am Nordende des Nordwest-Gangs (Muster J_affen.md) — wartet
  das Original dort ebenso, ist es RE1.5-Verhalten; sonst den Unterschied zu griff/szene benennen und nur dort lenken.
- Zufallsgang Anker 400: NB3 17975 gegen NB2 15857 (Anker 40: 221 gegen 1329), Abwaegung s. oben.
- PSX-Laufzeit: Raster (~5000 Knoten x 9 Klemmen-Aufrufe je Raum) + A* je Gorilla und Bild im Umweg — vor einem PSX-Port fuer
  ROOM11C0 als Tabelle vorzuberechnen (Nutzer-Regel 2026-09-03). Statische Puffer ~144 KB (12288 x (1+1+2) + 4 x 24 KB).
- Vorgaenger-OFFEN (ROOM10B0-Terrasse, Moebel-Tabelle) unveraendert.

### NB3 Fuer den Nutzer
- Neue Assets: keine. Sprachdateien: keine. Bedienhinweise: keine. Entscheidungsfragen: keine.
- Spuerbar: (1) Steht Leon im Gang an der Westseite des Parkplatzes, kommt der suedliche Gorilla wieder direkt ueber den
  westlichen Sprungplatz (wie vorher im Original) statt den Umweg ueber die Ostseite zu nehmen; der zweite Gorilla findet jetzt
  ebenfalls hin. (2) Gorillas, die an der Wand entlang zu Leon rutschen, werden nicht mehr umgelenkt — nur wer wirklich
  feststeckt. (3) Die drei "Nordwest-Taschen"-Plaetze der Abnahme liegen in den Kollisionszellen; Leon kann sie im Spiel nicht betreten.

### NB3 Ergebnis je Mangel (Kurzfassung)
| Mangel | Ursache | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| 1 Umleitung Ost-Pad | ungewichtete Komponenten-Suche, erste Pad-Kante in SCA-Reihenfolge (#36 @0x015d8 vor #47 @0x0165c) | Code + Sonde (Pad 36 ab F60) | A* nach Weglaenge; Beginn erst beim Haengen (+0x1de @0x80117448-6c) | exe NW-Gang Slot 2 F680/F704/F694 = v0.8.23, Slot 3 nie->nie / nie->F1496 / F1256->F892; Riegel Szene 5 |
| 2 Nordwest-Tasche | Plaetze IN den Zellen #13/#20/#21 (nur per RE15_PLAYER_POS); "Tasche" im Raster = Zurueckschnapp-Artefakt @0x8003be8c-a0; Startknoten ohne Erreichbarkeit | leonlauf (1938..5174 entfernt), klemme-Sonde, Raster 286/708 | Frei-Test Vorlage+1, Kanten, Startknoten gerade erreichbar, Leon-Seite | Riegel 4b; Taschenrand beide Angriff; Blockade NACH dem ersten Angriff: FERN 0, Rest NAH = Original (Spur J) |
| 3 Voll-Suite | — | — | Orchestrator | gezielt 163/163 |

---

## Nachbesserung 4 (nach Abnahme 3, 2026-10-11) — fortlaufend

Grundlage: `F_abnahme_3.md` (Stand 43220d8d/5a724df3: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3). P1/P3 nicht
anfassen (Riegel `unit_r36_zombieki_{sicht,eintritt,wecker,szenen,affen}` gruen halten), Spur-J-Riegel `unit_r35_affen_*`
(bildgenau gegen GDB) gruen halten.

| Mangel | Kern | Weg |
|---|---|---|
| 1 | FERN-Stau am Nordwest-Rand nach dem ersten Angriff: Slot 3 (Sub 4, d ~6200) drueckt in Slot 2 (Sub 3 an der #21-Ecke, d ~3300); `r36_uw_route` gibt 0 zurueck ("gerader Weg zu Leon frei"), der Streckentest kennt nur SCA-Zellen, nicht den zweiten Gorilla | Stau reproduzieren (Sonde probe_stau, exe 11/12); Original an derselben Lage messen (GDB); Streckentest/Route mit Koerperbelegung des anderen Gorillas; Riegel mit Gegenprobe; Angabe "fern 0" berichtigen |
| 2 | Nahe Rest-Blockade (Ostgassen-Eingang (6000,-9000), Nordwest-Ecke/-Rand) am Original nicht gemessen | GDB-Messung des Originals an beiden Stellen (Muster J_affen.md: r3 s033, Lagen per GDB setzen, je Bild beide Gorillas + Leon); Port-Sonde ab derselben Lage dagegen; danach Gate fuer die Nah-Blockade, das `unit_r35_affen_griff/_szene` gruen laesst |
| 3 | Voll-Suite | Orchestrator; hier gezielt |

### NB4 Messweg Original (DuckStation-GDB, Muster Spur J jnb6/gdbm2.py)
- Werkzeug `F_belege/nb4/gdb_affen.py`: DuckStation-GDB-Server (Port 2345; `%LOCALAPPDATA%/DuckStation/settings.ini` `[Debug]
  EnableGDBServer` nur fuer den Lauf auf true, nach dem Prozessende zurueckgeschrieben — nach jedem Lauf geprueft: "settings.ini
  zurueck: True", Zeile 507 `EnableGDBServer = false`), r3-Savestate `orig_scene/r3/s033_t054.55.sav` direkt geladen (MZD-Disc wie
  Spur J), Haltepunkt Gorilla-Wurzel 0x80116db8, Patch beim ersten Halt mit cur (0x800ac784) == e2 (0x800ad014) ab VSync 10029
  (0x800787dc; = Original-Bild F194/195 der Spur J): Lage +0x34/+0x38/+0x3c + Vorlage +0x40/+0x42/+0x44, Yaw +0x6a; Gorilla
  +0x4..+0x7 := 1/sub/0/sub3, +0x82 := 0, +0x1dc := 0; Leon (0x800aca54) HP +0x9a je Bild auf 100 gehalten (wie der Port-Riegel).
  Je e2-Halt eine Zeile: e1 (0x800ace20) / e2 Lage, Yaw, Sub, Clip, +0x1d4 (d), +0x1d6 (k), +0x1de (h), +0x1d0 (L), +0x1dc (B).
- Eichung (`g_kalib`): ohne Patch e1 (-5525,-14883) r2731 Clip 5/10 +0x1dc 9, e2 (-8915,-12487) r93 Clip 5/16 +0x1dc 15 = die
  Original-Lage F195 des Riegels (`aufbau`, test_r35_affen takt). e1 = Port-Slot 2, e2 = Port-Slot 3.
- Grenze des Messwegs (gemessen, O4): ein in der JAGD (Sub 3, Clip 5 mit Fusssperre bf50) abgesetzter Gorilla, der im SELBEN Bild
  noch tickt (e2), springt mit der alten Fussstellung zurueck (e2 10029 (5876,-8818) -> 10031 (-9124,-12478)); e1 (nach seinem Tick
  gesetzt) und Gorillas in der Auswahl (Sub 4, Clip 6 ohne Fusssperre) halten die Lage. O3/O3b setzen e1 in Sub 3 und e2 in Sub 4
  bzw. Sub 0 — gueltig. In O4 kam e2 aus der Szene selbst an die Stelle (s. unten).

### NB4 Mangel 1 — FERN-Stau am Nordwest-Rand (Gegenbeleg zu "fern 0" — BERICHTIGT)
- **Messung vorher** (exe 43220d8d = Abnahme 3, Auswertung `F_belege/nb4/fest_nach.py` = Abnahme-Skript fest_split.py + Teilung
  "nach dem ersten Angriff"): (-24452,11938) Slot 2 Angriff F828, danach fest 1529 (nah), **Slot 3 nie, fest 1465, davon FERN 1465**;
  (-22288,11938) Slot 2 F868 fest 760, **Slot 3 F1025 fest 894, FERN 691** — bitgleich zur Abnahme. Sonde `lage` (neu im Riegel,
  dieselbe Startlage wie die GDB-Messung) Leon (-26606,14578), G1 (-23302,14489) Sub 3, G2 (-20579,16118) Sub 4: Original-Regel und
  Stand NB3 (Umweg ohne Vortritt) **bitgleich**: 900/900 beide ohne Bewegung, G2 900 Bilder FERN (`p_o3_stau_originalregel.txt` /
  `p_o3_stau_nb3.txt`, "uw0" je Bild).
- **Berichtigung:** Die NB3-Angabe "Blockade NACH dem ersten Angriff: FERN 0" galt nur fuer die sieben Riegel-Szenen (Sonden-Start
  F195, Leon fest); in der exe stand Slot 3 an beiden Nordwest-Rand-Stellen 1465 / 691 Bilder FERN nach dem ersten Angriff (Abnahme
  3 hatte recht). Die Angabe in Commit 43220d8d ist damit falsch; diese Nachbesserung ersetzt sie.
- **Original (GDB, `F_belege/nb4/g_o3_stau_je30.txt`, 900 Bilder ab derselben Lage):** e1 (-23302, 14388..14507) Sub 3, d 3298,
  Sicht-Latch 1, Klemme 1; e2 (-20570..-20659, 16118) Sub 4, d 6122..6228, Klemme 1, **+0x1de 179 -> 1079 ohne Unterbrechung**; kein
  Angriff, Leon HP 100 unveraendert. **Das Original staut sich hier genauso** — der Stau ist die Original-Regel, sobald BEIDE
  Gorillas am Nordwest-Rand ankommen (im Original kommen sie dort gar nicht an: v0.8.23 an beiden Stellen nie, Abnahme 3).
- **Ursache (Code + Messung):** der einzige Weg in Leons Ecke ist der Streifen z 15728..16118 ueber #21 (@ROOM11C0.RDT 0x01524);
  Gorilla 1 steht NAH (d 3298 < 0x1771) in der Original-Jagd (B[3] auf playerX @0x80117d4c) an der #21-Ecke und versperrt den
  Streifen mit seinem Koerper (Gorilla 2 muesste mit dz >= 3200 an ihm vorbei, der Streifen erlaubt hoechstens 1629).
  `r36_uw_route` fuer Gorilla 2 gibt 0 zurueck ("gerader Weg frei", zombieki_r36.c Z. 519): die Knotenstrecke auf dem Streifen ist
  SCA-frei — der Blocker ist kein SCA, sondern Gorilla 1. Ein Umweg FUER GORILLA 2 kann den Stau nicht loesen (kein anderer Weg in
  die Ecke); loesen kann ihn nur Gorilla 1.
- **Aenderung (PORT-WAHL, zombieki_r36.c `re15_r36_affen_fest_takt` + `re15_r36_affen_padsuche_decide`, keine neue Zahl):
  VORTRITT** — steht ein Gorilla FERN (Sub 3/4, +0x1d4 >= 0x1771 @0x80117bc8) im Koerperkontakt (Radien @0x80121356 + ein
  Rasterschritt, wie `r36_pad_belegt`) mit einem Gorilla, der NAH und naeher an Leon in Sub 3/4 steht, nimmt der NAHE den Umweg
  (`r36_uw_route`, kuerzester Weg), bis sein gerader Weg zu Leon frei ist (dann Original) oder er Sub 3/4 verlaesst. Erste Fassung
  mit der Umweg-Startbedingung (R36_FEST_START 45 Bilder im Anker) liess Slot 3 an (-24452,11938) noch 103 Bilder FERN stehen
  (F1245-F1325, `re15_pc_nb4f.exe`) — deshalb ohne Wartezeit (der Vortritt wirkt ohnehin nur, wenn der NAHE keinen geraden Weg hat).
  Zwei NAHE Gorillas (Spur-J-Original griff: e1/e2 ~3925/~4990, Koerperkontakt 3209, +0x1de bis 497, `jnb1/g_griff.txt` selbst
  dekodiert) erfuellen die Bedingung nie und bleiben Original.
- **Messung nachher (ZWISCHENSTAND, Vortritt sofort bei Kontakt — endgueltige Fassung und Zahlen im Abschnitt "endgueltige Fassung"
  unten):** Sonde `lage` (Stau-Lage): G1 Angriff Bild 58, G2 FERN 0 (danach NAH an der #21-Ecke, s. Mangel 2)
  (`p_o3_stau_nb4.txt` ist inzwischen mit dem Endstand neu erzeugt: Angriff 91/101). exe `re15_pc_nb4g.exe`: (-24452,11938) Slot 2 F828, fest 0; **Slot 3 Angriff F1210
  (vorher nie), FERN 0**, nah 1377 (#21-Ecke); (-22288,11938) Slot 2 F868 fest 95 (nah); Slot 3 F1025, **FERN 4** (vorher 691),
  nah 797.

### NB4 Mangel 2 — Nahe Rest-Blockade: Messung des ORIGINALS (GDB)
| Lauf | Lage (ab VSync 10029) | Original (GDB) | Port (gleiche Lage, Original-Regel) |
|---|---|---|---|
| O3 Nordwest-Ecke, zwei Gorillas | Leon (-26606,14578); e1 (-23302,14489) Sub 3; e2 (-20579,16118) Sub 4 | 900/900 beide ohne Bewegung; e1 d 3298 Klemme 1 Sicht 1; e2 d 6122..6228 | 900/900 (`lage` um 0): G1 d 3304..3307, G2 d 6180..6225 |
| O3b Nordwest-Ecke, EIN Gorilla | Leon wie O3; e1 wie O3; e2 weit weg (9434,2189) Sub 0 | **900 Bilder e1 an der #21-Ecke (-23302, 14575..14599), d 3298, Klemme 1, Sicht 1, kein Angriff**; e2 kommt in 900 Bildern nicht naeher als 34000 | exe-Stelle 13: Slot 3 1649 Bilder an derselben Ecke (Abnahme 3) |
| O4 Ostgasse | Leon (5858,-5716) in der Gasse; e1 (3040,-10330) blind; e2 aus der Szene | **e1 an den Gasseneingang (5793..5878,-8818) d 3093 Sub 3 Sicht 1 Klemme 1, +0x1de bis 749; e2 in der Auswahl (3045,-10323) d 5394 Klemme 1, +0x1de bis 929; 900 Bilder kein Angriff** | exe (6000,-9000): Slot 3 (5876,-8818) d 3102, Slot 2 (3040,-10330) d 5400, 1652/1874 Bilder (Abnahme 3) |
| O1 Leon (6000,-9000), Gorillas aus der Szene | nur Leon gesetzt | e1 Heavy/Bisse ab ~F100, schiebt Leon DURCH die Ostgasse bis (7880,-1379); ab ~F1600 e1 in der Gassenmitte (6418,-4330) d 3298 Sicht 1 Klemme 1, e2 am Suedeingang (8061,-7080) d 5701; **beide bis F2400 ohne Bewegung (+0x1de bis 2299/2479)** | `szene2 6000 -9000` Original-Regel: G1 Angriff 196, schiebt Leon durch die Gasse bis (8837,-2141); G1 in der Gassenmitte (6418,-4413), G2 (8098,-7138), fest 1473 (`p_o1_6000_originalregel.txt`) |
| O5 Nordende Nordwest-Gang (exe-Stelle 1) | Leon (-9960,11409); e1 (-13533,11917) Sub 3 (Westkante Wagenblock #2 @ROOM11C0.RDT, z ueber der #2-Oberkante 10849); e2 (-8477,12822) Sub 4 | **e1 900 Bilder (-13527, 11912..11925) d 3602, Sub 3, Sicht 1, Klemme 1, +0x1de 149 -> 749+, kein Angriff**; e2 hielt die Lage nicht (Fusssperre) und stand FERN in der Auswahl bei (-12316,-8452) (+0x1de bis 929) | `lage` mit e2 dort: G1 (-13527, 11912..11920) d 3602..3633 900/900, G2 900 Bilder FERN = **Original Lage fuer Lage** (F600 (-13527,11912) r91 beide); mit Umweg verlaesst G2 seine Stelle, G1 wartet gleich |

- **Ergebnis: das Original wartet an beiden Stoss-Zielen ebenso.** An der Nordwest-Ecke bleibt ein naher Gorilla in der Jagd
  (B[3] steuert gerade auf playerX @0x80117d4c, A[3] hat unter 0x1771 keinen anderen Ausgang als den Biss unter 0xbb8
  @0x80117a60-74) an der #21-Ecke haengen — allein (O3b) wie zu zweit (O3); am Nordende des Nordwest-Gangs an der Westkante
  von #2 ebenso (O5, Lage fuer Lage wie der Port). ~~In die Ostgasse (2711 breit < 2 x 1600, Pad #36 @ROOM11C0.RDT 0x015d8) kommt
  kein Gorilla zu Fuss~~ — **BERICHTIGT in NB5 (Abnahme 4 Mangel 2): falsch.** Gemessen: ein naher Gorilla erreicht Leon in der
  Ostgasse zu Fuss (Abnahme 4 Sonde V3: 510 Seitenschritt + 166 nach Norden, Biss in Bild 54; NB5 exe-Stelle 3: Slot 3 geht nach
  dem Nah-Umweg den Kanal x 6800..7600 hinauf und beisst F1220; Bissplaetze fuer Leon (5858,-5716) x 6408..8258 z -8616..-4466,
  `F_belege/nb5/nb5_bissplaetze.txt`); im GDB-Original O1 steht e1 sogar in der Gassenmitte (6418,-4282..-4421). Der Gorilla
  bleibt stehen, weil B[3] GERADE auf playerX steuert (`addiu a0,a0,-13688` @0x80117d4c, `jal 0x8001a8f8` @0x80117d50) und A[3]
  NAH nur den Biss kennt (`ori a0,zero,0xbb8` / `ori a1,zero,0x180` / `jal 0x8001a804` / `bgez v0,0x80117b40` @0x80117a60-74);
  er haengt an der Klemme knapp ausserhalb des Bissfensters. Das Original schiebt Leon selbst hinein (O1) und wartet dann davor
  bzw. darin. Dazu Spur J (`jnb1/g_griff.txt`, selbst dekodiert, +0x1d0/+0x1de): im kanonischen Kampf warten e1/e2 mit Sicht-Latch 1
  an der Klemme ~3925/~4990 vor Leon, e2 mit +0x1de bis 497 — dieselbe Original-Nahjagd, die `unit_r35_affen_griff/_szene`
  bildgenau pinnen.
- Ein Gate, das die Nah-Wartezeit senkt, muesste genau diese Original-Lage aendern (NB3 gemessen: Haenge-Beginn auch NAH ->
  griff/szene rot; die Wartenden im kanonischen Kampf haben wie an der Nordwest-Ecke Sicht-Latch 1 und Klemme 1). Die
  Nah-Wartezeit bleibt deshalb Original und wird unter "Fuer den Nutzer" benannt. Was die PORT-WAHL aendert, ist nur, dass beide
  Gorillas ueberhaupt an den Nordwest-Rand kommen (Umweg) und sich dort nicht mehr gegenseitig festhalten (Vortritt, Mangel 1).

### NB4 Mangel 1 — endgueltige Fassung des Vortritts (Messreihe, Zufallsgang 52 Seeds x 6000 Bilder, je Lauf eigener Prozess)
- **Sofort bei Koerperkontakt** (Stand `re15_pc_nb4g.exe`): exe Nordwest-Rand FERN 0 / 4, aber Zufallsgang Anker 400 17975 -> 25676
  (FERN 3313 -> 9209). Ursache gemessen (Riegel-Modus `eins ... 1` mit LAGE-Zeilen): der Vortritt fuehrte den NAHEN ueber ein
  0x10-Pad (Umweg-Modus 2) — Seed 0x9075ea6c: Landung vor dem 0x20-Pad #19 bei (-13518,-560), dort 1789 Bilder die
  Original-Blindsprung-Schleife (+0x7 = 3 @0x80117fe4-80118024); Seed 0x95e8eee7: Gorilla 2346 Bilder in Sub 4 vor dem
  Ost-Pad-Anlauf (8215,-7955) (Pad-Kante nicht erreicht, FUN_8003b93c-Radius box[6]+100 @0x80117ee0-f00). -> Vortritt nur
  **zu Fuss (Modus 1) und nur solange NAH**: 22604 (FERN 5534) — immer noch schlechter als NB3.
- **Schwelle** (PORT-WAHL, Stellgroesse per Messreihe; Anker = R36_FEST_ANKER 400 Manhattan, derselbe Zaehler wie der Umweg-Beginn):
  Bilder im Anker, die der FERNE vor dem Vortritt steht — `eins <i> 1 6000 <anker> 0 <10+K>` (`F_belege/nb4/seeds/`):

| K | 1 | 8 | 15 | **22** | 30 | 37 | 45 | 60 (= NB3) |
|---|---|---|---|---|---|---|---|---|
| Anker 400 | 22057 | 21150 | 17356 | **16352** | 16964 | 18881 | 17806 | 17975 |
| Anker 40 | 1012 | 257 | 228 | **125** | 221 | 327 | 221 | 221 |
| Griffe | 254 | 251 | 237 | **242** | 241 | 245 | 240 | 245 |

  Gewaehlt **R36_VORTRITT_START = R36_FEST_START / 2 = 22** (bester Wert beider Anker, INNERHALB der Reihe; die Reihe ist
  chaotisch — 37 schlechter als 30 und 45 —, deshalb an die bestehende Umweg-Startzeit gebunden statt frei gewaehlt). Mit 45
  stand Slot 3 an (-24452,11938) 103 Bilder FERN (F1245-F1325) — das Abnahmekriterium (< 90) haette es verfehlt.
- **Riegel** `unit_r36_zombieki_affen_block` 54/54 (`F_belege/nb4/nb4_affen_riegel.txt`): Block "Stau Nordwest-Rand" — Sonde `lage`
  (dieselbe Startlage wie GDB O3) je Modus ein eigener Prozess: Original-Regel 900/900 fest, G2 900 FERN (= Original); Stand NB3
  (Umweg ohne Vortritt) 900/900, G2 900 FERN (= Abnahme 3 "Umweg 0"); mit Vortritt **G1 Angriff Bild 91, G2 Bild 101, FERN 0/0**.
  16 Seeds AUS 38439 -> AN 5174 (NB3 5725), schlechtester +40 (NB3 +59), Griffe 73 -> 71; sieben Szenen unveraendert gruen.

### NB4 Messung nachher in der exe — alle 14 Stellen der Abnahme 3 (`F_belege/nb4/nb4_exe_14_stellen.txt`)
Kommandozeile der Abnahme (`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700`, Basis-Haken wie
`F_abn3/run.sh`), keine Eingabe, je Lauf eigener Prozess, Kopie `re15_pc_nb4h.exe` (= Endstand; `re15_pc_nb4i.exe` aus dem letzten Bau
liefert an (-24452,11938) ein bitgleiches gy.log). Auswertung `fest_nach.py` (= Abnahme-Skript: fest = aktiv, kein Angriff, >= 90
Bilder im Anker 400, Leon > 3000; fern = d >= 6001; "nach" = nach dem ersten Angriff irgendeines Gorillas). v0.8.23 und NB3 = die
gy.log-Dateien der Abnahme 3 (`F_abn3/affen_*_{abn0f_basis,abn3f_neu}`), mit demselben Skript ausgewertet. Eintrag: erster Angriff
Slot 2 / Slot 3 (fest S2/S3; FERN S2/S3; FERN nach dem ersten Angriff S2/S3).

| # | Leon | v0.8.23 | NB3 | **NB4** |
|---|---|---|---|---|
| 1 | (-7000,12500) | nie / F1012 (1882/0; 1882/0; 1283/0) | nie / F730 (826/0; 0/0; 0/0) | nie / F730 (826/0; 0/0; 0/0) |
| 2 | (-14000,2000) | F680 / nie (0/930; 0/930; 0/629) | F680 / nie (0/0; 0/0; 0/0) | F680 / nie (0/0; 0/0; 0/0) |
| 3 | (6000,-9000) | nie / F324 (1573/1875; 0/0; 0/0) | nie / F324 (1653/1875; 0/0; 0/0) | nie / F324 (1653/1875; 0/0; 0/0) |
| 4 | (-24000,10000) | nie / nie (1819/1418; 1819/1418; 0/0) | F853 / F1570 (0/1193; 0/1; 0/1) | F853 / F1570 (0/1193; 0/1; 0/1) |
| 5 | (7000,4000) | nie / F18 (1012/0; 1012/0; 1012/0) | nie / F18 (96/0; 96/0; 96/0) | nie / F18 (96/0; 96/0; 96/0) |
| 6 | (9000,10000) | nie / F309 (1102/0; 1102/0; 1102/0) | F1175 / F309 (0/0) | F1175 / F309 (0/0) |
| 7 | (-14746,-4140) | F364 / nie (1835/0; 0/0; 0/0) | F364 / F1021 (179/108; 0/108; 0/108) | F364 / F1021 (179/108; 0/108; 0/108) |
| 8 | (7200,-4130) | F1246 / F274 (0/670; 0/0) | F1050 / F274 (0/468; 0/0) | F1050 / F274 (0/468; 0/0) |
| 9 | (-14000,4000) | F704 / nie (0/1088; 0/1088; 0/761) | F704 / F1496 (250/0; 0/0) | F704 / F1496 (250/0; 0/0) |
| 10 | (-14500,6000) | F694 / F1256 (171/298; 0/298; 0/25) | F694 / F892 (0/0) | F694 / F892 (0/0) |
| **11** | (-24452,11938) | nie / nie (1821/1588; 1821/1588; 0/0) | F828 / nie (1529/1465; 0/1465; **0/1465**) | **F828 / F1253 (0/1335; 0/0; 0/0)** |
| **12** | (-22288,11938) | nie / nie (1769/1672; 1769/1672; 0/0) | F868 / F1025 (760/894; 0/691; **0/691**) | **F868 / F1025 (0/833; 0/50; 0/50)** |
| 13 | (-26500,15500) | nie / F1620 (1841/90; 1841/90; 1080/0) | F842 / nie (0/1650; 0/0) | F842 / nie (0/1650; 0/0) |
| 14 | (-20000,14500) | nie / F1541 (1482/0; 1482/0; 976/0) | F883 / F896 (0/0) | F883 / F896 (0/0) |

- **Abnahmekriterium Mangel 1 erfuellt:** an beiden Nordwest-Rand-Stellen kein Gorilla >= 90 Bilder FERN nach dem ersten Angriff
  (11: 0, vorher 1465; 12: 50, vorher 691). Slot 3 greift an Stelle 11 jetzt an (F1253, vorher nie); Slot 2 steht an beiden
  Stellen nicht mehr (fest 1529 -> 0 bzw. 760 -> 0). Die uebrigen zwoelf Stellen sind gegenueber NB3 unveraendert (erster Angriff und fest-Bilder gleich) —
  kein Gorilla greift spaeter an als in NB3, keine Stelle spaeter als v0.8.23.
- Summen 14 Stellen: fest v0.8.23 25936 (FERN 19812) -> NB3 12946 (FERN 2361) -> **NB4 10466 (FERN 255)**. Die 255 FERN-Bilder:
  Stelle 7 Slot 3 108 (F420-F510 blind an der Ostkante des Wagenblocks #7 (6418,~1500), d ~22000, BEVOR er selbst angreift; der
  Umweg loest es nach R36_FEST_START + Anlauf, unveraendert seit NB3), Stelle 5 Slot 2 96 (F390-F480 blind bei (-7800,-12700), d
  ~25000, nach Slot 3s Angriff F18; ebenfalls vom Umweg aufgeloest, seit NB3 unveraendert, v0.8.23 1012), Stelle 12 Slot 3 50,
  Stelle 4 Slot 3 1. Die beiden Strecken ueber 90 (Stellen 5/7) sind Haengen, die der Umweg nach seiner Startzeit selbst aufloest
  (Abnahme 3 hat sie gemessen und nicht beanstandet); sie liegen nicht am Nordwest-Rand.
- Die NAH-Bilder (10211) sind die Original-Nahjagd (Mangel 2), an jeder dieser Lagen im Original gemessen: Ostgassen-Eingang
  (Stelle 3: O1/O4), #21-Ecke der Nordwest-Ecke (Stellen 4, 11, 12, 13: O3/O3b), Westkante #2 am Nordende des Nordwest-Gangs
  (Stelle 1: O5).

### NB4 Zufallsgang (52 Seeds x 6000 Bilder, je Lauf eigener Prozess; `F_belege/nb4/seeds/`)
| Anker | AUS (Original-Regel) | NB3 | **NB4** |
|---|---|---|---|
| 400 | 114029 / 226 Griffe | 17975 (nah 14662 / fern 3313) / 245 | **16352 (nah 13583 / fern 2769) / 242** |
| 40 | 75408 | 221 | **125** |
| 16 Seeds (Riegel), 400 | 38439 / 73 | 5725 / 74 | **5174 / 71** (schlechtester +40) |

### NB4 Mangel 3 — Voll-Suite
Im Spurbaum verboten (Auftrag); der Orchestrator faehrt sie EINMAL im Integrationsbaum. Hier gezielt (unten, NB4 Tests).

### NB4 Tests (probes/r36_zombieki.cmake, unveraendert registriert)
| Test | Mangel | Misst | Ergebnis |
|---|---|---|---|
| `unit_r36_zombieki_affen_block` | M1, M2 | wie NB3 (16 Seeds, sieben Szenen, 4b) + NEU: Stau Nordwest-Rand je Modus eigener Prozess (Gegenprobe Original-Regel = GDB-Original 900/900, Gegenprobe Stand NB3 = "Umweg 0", Vortritt: Angriff 91/101, FERN 0/0); Original-Nahwarten gegen die GDB-Lage (Nordwest-Ecke O3b Ende (-23272,14587) / Original (-23302,14597), kein Angriff; Ostgasse O1 Ende (6418,-4399) / Original (6418,-4409); Nordende Nordwest-Gang O5 Ende (-13557,11920) / Original (-13527,11914), kein Angriff) | **54/54** |
| `unit_r36_zombieki_sicht`, `_eintritt`, `_wecker`, `_szenen`, `_affen` | P1/P3 (erfuellt) | unveraendert (kein P1/P3-Code geaendert) | gruen |
| Spur-J `unit_r35_affen_*` (20), `unit_maggot_*` (3), `unit_r17_waffen_loop_pin` | Original-Nahjagd | bildgenau gegen GDB | gruen |

**Gezielte Laeufe (keine Voll-Suite im Spurbaum):**
- Regex der Abnahme 3 (Module/Raeume des Zweig-Diffs), Stand Code = Endstand (nur der Riegel bekam danach den O5-Pin):
  `ctest --test-dir re15_port/build -R "affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|1140|1210|1100|10d0|10b0|1010|1220|11c0|r35_raeume|r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|r34n_f|r36_zombieki|nav|unit_enemy_|room1070_wake|ai_flavor|re2_room1090|ada_eintritt|r17_waffen_loop|schatten|shadow|grab|griff|victim|re2_crawler|knockdown|stagger|hit_reaction|re2_livepath|re2_hit_repeat|sca_contact90|sca_wall_hit|npc_wall_clamp|r16_aufstehen|r34_reaktion|r34_schaden|unit_damage|re2_gore|r26_mg_blut|re2_stepsize|re2_kfjump|poise|re2_hp_model|room_probe_sweep|r35_granate|r35_redhawk|r35_werfer|r34_granaten" --timeout 600 -j 6 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 200** (8 min 47 s; `F_belege/nb4/nb4_ctest_gezielt.txt`)
- nach dem letzten Bau (Riegel mit O5-Pin) `ctest --test-dir re15_port/build -R "r36_zombieki|unit_r35_affen|unit_maggot|r17_waffen_loop" --timeout 600 -j 6 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 30**; Riegel direkt `test_r36_zombieki_affen_block riegel` **54/54** (`F_belege/nb4/nb4_affen_riegel.txt`)

### NB4 OFFEN
- **Nah-Wartezeit an den Stoss-Zielen (Original, gemessen):** Nordwest-Ecke (#21-Ecke) und Ostgasse — das Original wartet dort ebenso
  (GDB O1/O3/O3b/O4, Tabelle oben; Riegel pinnt Port = Original). Eine Aenderung waere eine bewusste Abweichung vom Original, die die
  Spur-J-Riegel `griff/szene` (kanonischer Kampf: e1/e2 warten mit Sicht-Latch 1 an der Klemme, +0x1de bis 497) brechen wuerde —
  nicht gebaut.
- Stelle 7 (-14746,-4140): Slot 3 108 Bilder FERN blind an der Ostkante von #7, bevor der Umweg nach R36_FEST_START greift (seit NB3
  unveraendert; Messweg: `fest_nach.py` auf `affen_-14746,-4140_nb4h`).
- PSX-Laufzeit (aus NB3): Raster + A* je Gorilla und Bild; der Vortritt fuegt je fernem Gorilla nur eine Schleife ueber die Aktoren
  hinzu. Vor einem PSX-Port fuer ROOM11C0 als Tabelle vorzuberechnen.
- Vorgaenger-OFFEN (ROOM10B0-Terrasse, Moebel-Tabelle) unveraendert.

### NB4 Fuer den Nutzer
- Neue Assets: keine. Sprachdateien: keine. Bedienhinweise: keine. Entscheidungsfragen: keine.
- Spuerbar neu: Zwei Gorillas halten sich am Nordwest-Rand des Parkplatzes nicht mehr gegenseitig fest — wird Leon dort in die Ecke
  gestossen, geht der vordere Gorilla oben um den Block herum und greift an, der hintere rueckt nach (vorher standen beide bis zum
  Ende still).
- ~~Ausdruecklich, weil es so bleibt: das Original macht es genauso. Steht Leon in einer Ecke oder Gasse, in die ein Gorilla nicht
  hineinpasst — die Nordwest-Ecke hinter dem Block, die schmale Ostgasse zwischen den Wagen —, wartet der Gorilla davor, bis Leon
  wieder herauskommt.~~ **BERICHTIGT in NB5 (Abnahme 4 Mangel 2):** Der Grund war nicht der Platz — ein Gorilla kommt zu Fuss in
  Bissweite (Ostgasse: Kanal neben dem Eingang; Nordwest-Ecke: ueber den Nordstreifen). Er blieb stehen, weil er im Original
  stur geradeaus auf Leon zulaeuft und an der Wagenkante haengen bleibt, knapp ausserhalb seiner Bissweite. Seit NB5 sucht er
  sich nach rund 12 s den Weg zum freien Bissplatz (s. "NB5 Fuer den Nutzer"). Im Original gemessen (Debugger, gleiche Lage): an der Ecke 900 Bilder (30 s) ohne Angriff, vor der Ostgasse
  900 Bilder ohne Angriff; aus dem normalen Kampf heraus schiebt der Gorilla Leon beim Angriff sogar selbst durch die Ostgasse und
  wartet dann bis zum Ende der Messung (ueber 800 Bilder, ~27 s). Auch im normalen Kampf nach der
  Ada/Marvin-Szene wartet im Original ein Gorilla eine Weile an einer Wand, bis Leon sich bewegt.

### NB4 Ergebnis je Mangel (Kurzfassung)
| Mangel | Ursache | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| 1 FERN-Stau Nordwest-Rand | Gorilla 1 NAH in der Original-Jagd an der #21-Ecke versperrt den einzigen Streifen; Umweg von Gorilla 2 sah nur SCA ("gerader Weg frei") | Original per GDB ab derselben Lage: 900/900 Stau (e2 +0x1de 179 -> 1079); Port-Sonde Original-Regel = NB3 = Befund | PORT-WAHL Vortritt: FERN seit R36_FEST_START/2 Bildern im Anker + Koerperkontakt mit einem NAHEN vor ihm -> der NAHE geht den Umweg zu Fuss (Messreihe 1..60) | exe 11/12 FERN 1465/691 -> 0/50, Slot 3 an 11 greift an (F1253); Riegel Stau mit zwei Gegenproben; "fern 0" berichtigt |
| 2 Nahe Rest-Blockade | Original-Nahjagd (A[3] unter 0x1771 nur Biss < 0xbb8 @0x80117a60-74, B[3] auf playerX @0x80117d4c) an Ecke/Gasse | GDB O1/O3/O3b/O4: das Original wartet ebenso (900..2400 Bilder); J-Spur griff: Wartende mit Sicht-Latch 1 | keine (Original); benannt unter "Fuer den Nutzer"; Riegel pinnt Port = Original | Riegel O3b/O1 gegen GDB-Lage (Abweichung 32 / 10) |
| 3 Voll-Suite | — | — | Orchestrator | gezielt (oben) |

---

## Nachbesserung 5 (nach Abnahme 4, 2026-10-11) — fortlaufend

Grundlage: `F_abnahme_4.md` (Stand c541415c/0b12bbf6: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3). P1/P3 nicht
anfassen (Riegel `unit_r36_zombieki_{sicht,eintritt,wecker,szenen,affen}` gruen), Spur-J-Riegel `unit_r35_affen_*` (bildgenau
gegen GDB, laufen mit aktiver PORT-WAHL) gruen. Ziel-Kriterium (Abnahme 4 Mangel 1 + Orchestrator): an den exe-Stellen 3, 4, 11,
12, 13 und im Tuerweg `t11b0kampf` steht kein Gorilla in SUMME mehr als 450 Bilder NAH fest (Definition Abnahme 4.1: aktiv, kein
Angriff Sub 5/6/8/15, Leon > 3000, >= 90 Bilder im 400er-Anker); Gesamt (HEAD 10466) und FERN (255) an den 14 Stellen steigen
nicht. Aufhebung des am GDB-Original gemessenen NAH-Wartens = PORT-WAHL (Nutzer "irgendwie verbessern"); die Original-Gleichheit
bleibt im Modus Original-Regel gepinnt.

| Mangel | Kern | Weg |
|---|---|---|
| 1 | NAH-Stillstand an den Stoss-Zielen (Stellen 3/4/11/12/13, Tuerweg t11b0kampf 1309) | Messen, WAS den nahen Gorilla haelt (Klemme/Koerper/Biss-Sperre, Route vorhanden?) und was den kanonischen Kampf (J-Riegel szene: S2 wartet ~400 Bilder bei d 3004..3149 vor dem ersten Biss +521) davon unterscheidet; Nah-Umweg mit Gate, das szene/griff nicht beruehrt; Messreihe Gate-Beginn; exe 14 Stellen + Tuerweg |
| 2 | Begruendung "passt nicht hinein" / "kommt nicht zu Fuss" gemessen falsch | Text an Z. 960-961 und 1073-1077 berichtigen (B[3] gerade auf playerX @0x80117d4c, A[3] nah nur Biss @0x80117a60-74) |
| 3 | Riegel pinnt Stillstand im PORT-Modus (`lage <b> 1`) | O1/O3b/O5-Gleichheit in den Modus Original-Regel (`lage <b> 0`); im PORT-Modus Pins auf das Kriterium (Angriff, NAH-fest <= Schranke) |

### NB5 Mangel 1 — Messung vorher: WAS den nahen Gorilla haelt (HEAD c541415c, Rohdaten der Abnahme 4 `F_abn4/`, Auswerter `ausw4.py`)
Spuren je Stelle mit `F_belege/nb5/spur.py` (gy.log -> Lage/Sub/Abstand je Gorilla), NAH-Anker offline mit `F_belege/nb5/nahz.py`
(dieselbe Zaehlung wie der Port: Sub 3, 0xbb8 <= d < 0x1771, Anker 400 Manhattan):

| Lage | Wer steht | Was ihn haelt | Freier Bissplatz? |
|---|---|---|---|
| Stelle 3 (6000,-9000) Slot 3 | F826+1875 am Ostgassen-Eingang (5771..5881,-8818), d 3087..3158 | Wagenkante; B[3] steuert gerade auf playerX, A[3] nah nur Biss | JA: Kanal x 6800..7600 bis z -6400 neben dem Eingang (Raster 400, Bissplaetze x 6408..8258) |
| Stelle 3 Slot 2 | F1048+1653 bei (3030,-10336), d ~5400, blind | Koerperkontakt zu Slot 3 (dx 2820 / dz 1510 = 3199) | erst, wenn Slot 3 weg ist |
| Stellen 4/11/12/13 Slot 3 | an der Ostseite von #21 (-23302, 13088..13333), d 3211..5514 | #21 (@ROOM11C0.RDT 0x01524) — Slot 2 greift derweil in der Ecke an (Angriffsbilder 785/810/652/731) | NEIN: einziger Bissplatz von Slot 2 belegt (s. Bissplaetze) |
| t11b0kampf Slot 2 | auf dem Nordstreifen (-22290..-22473, 16118), d 4604..4725 | Koerperkontakt zu Slot 3 (dx ~3150), Slot 3 beisst Leon aus (-25600,15728) d ~2000 | NEIN: wie oben |
| kanonischer Kampf Slot 2 (J `szene`) | V-Spitze (-4940..-5052, -14535..-14658), d 3041..3099 vor dem ersten Biss (Original +521) | Wagenkanten der V-Nische; Leon steht IN der Nische | JA: linker V-Grund (-6800,-14400) d ~2070 |

Der kanonische Kampf ist DIESELBE Lage wie Stelle 3 (ein Gorilla 41..158 Einheiten ausserhalb des Bissfensters an der Klemme) —
und er ist durch die Spur-J-Riegel als Original gepinnt (Biss +521, Takt, Tod +1194; GDB jnb1/g_orig.txt). Ein Nah-Umweg darf
dort also erst NACH der kanonischen Wartezeit beginnen.

### NB5 RE-Belege (STAGE1.BIN, re15_disasm.py, selbst gelesen)
- A[3] Biss-Test: `lbu v0,0(a2)` / `bne v0,zero,0x80117b40` @0x80117a54-5c (Spieler in Trefferreaktion -> kein Biss), `ori a0,zero,0xbb8`
  @0x80117a60, `ori a1,zero,0x180` @0x80117a64, `addiu s0,a2,-95` @0x80117a68 (a2 = Leon +0x93 -> +0x34), `jal 0x8001a804` @0x80117a6c,
  `bgez v0,0x80117b40` @0x80117a74. Rest von A[3] ohne Zustandswechsel NAH: `lhu v0,468(v1)` / `sltiu v0,v0,0x1771` @0x80117bc0-c8;
  nur FERN + Sicht: `sltiu v0,v0,0x1771` / `bne v0,zero,0x80117c78` @0x80117c34-38, `lhu v0,464(v1)` / `andi v0,v0,0x1` / `beq` @0x80117c40-4c,
  `ori v0,zero,0x4` / `sb v0,5(v1)` @0x80117c50-54.
- B[3]: `lb a1,158(v0)` @0x80117d44, `lui a0,0x800b` / `addiu a0,a0,-13688` (= 0x800aca88 playerX) @0x80117d48-4c, `jal 0x8001a8f8`
  @0x80117d50 — gerade auf Leon, kein Navigator.
- Radius box[6]: Bytes ab 0x80121350 `00 00 60 fa 00 00 40 06` -> u16 @0x80121356 = 0x0640 = 1600.
- Koerperabstand zweier Gorillas: Spur J `griff` (GDB) e1-e2 3168..3209 (= 2 x 1600).

### NB5 Bissplaetze (neue Sonde `platz`, `F_belege/nb5/nb5_bissplaetze.txt`)
Alle Lagen im Abstand < 0xbb8 um Leon, die die Original-Klemme (Radius 1600, Band 0, Maske 4, Vorlage + 1 wie `r36_frei_bei`) NICHT
verschiebt (Schritt 50); Durchmesser = groesster Abstand zweier Bissplaetze. Unter 3168 kann nur EIN Gorilla zugleich beissen.

| Leon | Bissplaetze | Bereich | Durchmesser | |
|---|---|---|---|---|
| (-26932,17268) Ecke Nord (t11b0kampf) | 279 | x -25782..-24182, z 15718..16118 | **1649** | nur EIN Gorilla |
| (-26932,14578) Ecke Sued | 279 | x -25782..-24182, z 15728..16128 | **1649** | nur EIN Gorilla |
| (-26932,15700) Ecke Mitte | 296 | x -25782..-23982 | **1833** | nur EIN Gorilla |
| (-26600,14578) | 342 | x -25800..-23850 | 1990 | nur EIN Gorilla |
| (-7107,-12395) kanonisch (exe) | 4030 | x -10057..-5407 | 5809 | zwei passen |
| (-7138,-12372) kanonisch (J) | 4035 | | 5809 | zwei passen |
| (5858,-5716) Ostgasse | 1769 | x 6408..8258, z -8616..-4466 | 4154 | Durchmesser reicht; der Kanal ist aber einspurig (s. unten) |

**Folge (Messung, nicht Annahme):** In der Nordwest-Ecke (Stellen 4, 11, 12, 13 und der Tuerweg t11b0kampf: Leon an der Westwand
x -26932, z 14562..17268) liegt JEDER Bissplatz auf dem 390 breiten Nordstreifen westlich x -23982; der zweite Gorilla muss
3168 Abstand zum ersten halten und liegt damit immer > 3000 vor Leon. Er KANN dort nicht beissen, solange der erste angreift —
weder im Original noch im Port, und kein Umweg aendert das. Gegenprobe gemessen (`lage` Stau-Lage O3, 1800 Bilder,
`F_belege/nb5/anstehen_o3_um415.txt`): Stand NB4 Gorilla 2 fest 1613 an der #21-Ecke; Nah-Umweg MIT Belegt-Test (Endstand) 1613
(kein freier Bissplatz -> kein Umweg); Nah-Umweg OHNE Belegt-Test (Sonde, Modus 5) zieht Gorilla 2 nur auf den Nordstreifen HINTER
Gorilla 1 (-22357,15752) und er steht dort 1466 Bilder — genau das t11b0kampf-Bild. Das Kriterium "kein Gorilla > 450 NAH fest"
ist an diesen fuenf Stellen fuer den ZWEITEN Gorilla nicht durch Angreifen erreichbar (WIDERLEGT fuer diesen Teil); erreichbar waere
es nur, wenn der Wartende sich ohne Ziel bewegt (Hin-und-her-Laufen) — das waere ein erfundenes Verhalten allein fuer das Messmass
und ist nicht gebaut.

### NB5 J-Fenster: wann darf ein Nah-Umweg beginnen? (neue Sonde `kanon` = Harness des J-Riegels `szene`, `F_belege/nb5/nb5_kanon_messreihe.txt`)
- `kanon` reproduziert den J-Riegel bitgleich (Freigabe F1066, Heavy +363, Bisse 468 521 571 ... 1139, Tod +1194 = `test_r35_affen szene`).
- NAH-Anker von Slot 2 vor seinem ersten Biss: **353** (J-Harness); exe-Tuerweg 11B0->11C0 (t11b0szene) **353**, exe-Sprung (szene11c0)
  346 (offline, nahz.py); +0x1de-Zwilling (Klemme ohne Unterbrechung) 444.
- Messreihe Beginn N (Nah-Umweg nach N Bildern im NAH-Anker; Bisse relativ zur Freigabe; Original: 363 468 521 571 624 ... Tod +1194):

| N | Bisse (erste 5) | Tod | kanonischer Kampf |
|---|---|---|---|
| 1 | 89 147 205 263 322 | +935 | veraendert (Slot 2 beisst +34) |
| 45 | 161 219 277 336 373 | +1031 | veraendert |
| 90 | 279 338 375 412 470 | +1023 | veraendert |
| 150 | 351 586 628 664 700 | +1092 | veraendert |
| 200 | 363 469 515 572 619 | +1188 | veraendert |
| 250 | 363 468 514 570 618 | +1186 | veraendert |
| 300 | 363 468 513 549 585 | +980 | veraendert |
| 330 | 363 469 526 700 757 | +1390 | veraendert |
| 340 | 363 468 527 888 947 | +1421 | veraendert |
| 343 | 363 468 527 733 770 | +1382 | veraendert |
| 344 | 363 468 521 571 624 | +1194 | gleich (Umweg-Ziel in dem Bild belegt) |
| 345 | 363 468 527 695 896 | +1297 | veraendert |
| 350 / 353 | 363 468 521 571 624 | +1194 | gleich |
| **354** | 363 468 521 571 624 | +1194 | **gleich (Zaehler erreicht 354 nie vor dem Biss)** |

  Unterhalb 354 haengt das Ergebnis vom Zufall des Bildes ab (344/350/353 gleich, 345 nicht); erst ab 354 ist der kanonische Kampf
  SICHER unberuehrt. Damit ist die Frage der Abnahme ("Gate-Beginn oberhalb des J-Fensters") gemessen: jeder Beginn unter 354 bricht
  `unit_r35_affen_szene` (Slot-2-Bisse / Tod), ab 354 bleibt er bitgleich; `unit_r35_affen_griff` (Fenster T378..T495 = 117 Bilder)
  liegt weit darunter.

### NB5 Umsetzung — PORT-WAHL Nah-Umweg zum freien Bissplatz (zombieki_r36.c, keine neue gemeinsame Datei)
- NAH-Anker `s_r36_nah`: Gorilla in der Jagd (Sub 3, Zustand 1), Abstand 0xbb8 <= +0x1d4 < 0x1771 (Bissweite a804 @0x80117a60 /
  Nah-Grenze `sltiu 0x1771` @0x80117bc8), Bilder im Anker R36_FEST_ANKER (400 Manhattan, derselbe wie der Umweg-Beginn seit NB3).
- `r36_nah_route`: Ziel = Rasterknoten der EIGENEN Komponente (zu Fuss), Leon am naechsten, in Bissweite (< 0xbb8) und von keinem
  anderen lebenden Gorilla belegt (Abstand >= Summe der Radien box[6] @0x80121356 = Koerperkontakt). Weg = A* zu Fuss (keine Pads —
  NB4 gemessen: Pads im Vortritt fuehrten in die Original-Blindsprung-Schleife), Zielpunkt wie `r36_uw_route` Modus 1. Kein freier
  Bissplatz -> 0 = Original (der Gorilla wartet; Anstehen, s. Bissplaetze).
- Haken in `re15_r36_affen_padsuche_decide` (A[3]-Haken, nach dem Biss-Test): ab `s_r36_nah >= R36_NAH_START` (oder solange aktiv)
  NAH und ausserhalb der Bissweite -> Steuerziel = Umweg-Punkt (B[3]-Haken `re15_r36_affen_padsuche_ziel`, unveraendert); endet mit
  Sub != 3 (Biss/Griff/Treffer), mit Bissweite oder wenn kein freier Bissplatz mehr existiert.
- **R36_NAH_START = 354** (PORT-WAHL, aus der Messung): die laengste gemessene Nah-Wartezeit des kanonischen Ankunftskampfes (353
  Bilder: J-Harness `szene`, exe-Tuerweg 11B0->11C0; exe-Sprung 346) + 1. Kein Original-Wert — das Original hat NAH keinen Ausgang;
  der Wert legt nur fest, dass der gepinnte Original-Kampf unberuehrt bleibt (Messreihe oben).
- Keine Aenderung an enemy_ai_common.c / main.c (die Haken aus NB1-NB4 reichen). Nur-Sonden-Schalter `re15_r36_affen_nah_enable`
  (0 = Stand NB4, 2 = ohne Belegt-Test, >= 10 = Schwelle - 10) ohne Aufrufer in engine/platform; Leser `re15_r36_affen_nah_lesen`.

### NB5 Messung nachher in der exe (`F_belege/nb5/nb5_exe_14_stellen_ausw.txt`, `nb5_exe_kommandos_kanon.txt`, Kopie `re15_pc_nb5a.exe`)
Kommandozeilen der Abnahme 4 (Abschnitte 4.1/4.4), je Lauf eigener Prozess, Auswertung mit dem Skript der Abnahme (`F_abn4/ausw4.py`).
**gy.log bitgleich zu HEAD (abn4f_neu): 12 der 14 Stellen, Flucht West, t11b0kampf, kanonischer Kampf per Sprung (szene11c0) und per
Tuerweg (t11b0szene).** Veraendert nur Stellen 1 und 3:

| # | Leon | HEAD erster Angriff S2/S3 (fest S2/S3) | **NB5** |
|---|---|---|---|
| 1 | (-7000,12500) | nie / F730 (826/0) | **F1627** / F730 (341/250; S3 250 bis zum Laufende F2160) |
| 3 | (6000,-9000) | nie / F324 (1653/1875) | **F1596** / F324 (**429** davon FERN 45 / **368**) |
| 4, 11, 12, 13 | Nordwest-Ecke | S3 1193 / 1335 / 833 / 1650 | unveraendert (Anstehen, s. Bissplaetze) |
| t11b0kampf | Tuerweg, Kampf-Layout | S2 1309 | unveraendert (Anstehen) |
| uebrige 8 | | | bitgleich |

- Summe 14 Stellen fest (FERN / NAH): v0.8.23 25936 (19812 / 6124) -> NB4 10466 (255 / 10211) -> **NB5 7500 (300 / 7200)**.
- **Stelle 3 erfuellt das Kriterium** (S2 429, S3 368 <= 450): Slot 3 beginnt F826+354 = F1180 den Nah-Umweg, geht den Kanal
  hinauf und beisst F1220, schiebt Leon durch die Gasse nach Norden; Slot 2 rueckt bis an den Eingang nach (5204,-8818), wird FERN,
  als Leon die Gasse verlaesst, und springt mit dem ORIGINAL-Padsprung (Sub 4 @0x80117c50-54, A[4] Path A @0x80117ecc-8011802c)
  F1580 ueber die Gasse, Angriff F1596/F1600.
- FERN +45 (255 -> 300): genau diese 45 Bilder von Slot 2 an Stelle 3 (Ende seiner Strecke F1397+158) zwischen "Leon verlaesst die Gasse" und dem
  Original-Padsprung — kein Stau (vorher stand Slot 2 dort 1653 Bilder NAH ohne Angriff).
- Stellen 4/11/12/13 und t11b0kampf: der Wartende ist der ZWEITE Gorilla, der einzige Bissplatz ist belegt (Bissplaetze oben) —
  Kriterium dort fuer ihn gemessen nicht erreichbar.
- Zufallsgang 52 Seeds x 6000 Bilder (`F_belege/nb5/nb5_zufallsgang.txt`): Anker 400 16352 / 242 Griffe, Anker 40 125 — bitgleich
  zu NB4 (der Nah-Umweg greift dort nie: keine Nah-Wartezeit erreicht 354 Bilder, weil Leon laeuft).

### NB5 Mangel 2 — Begruendung berichtigt
An beiden Stellen im Dossier (NB4-Ergebnis Z. 960ff., NB4 "Fuer den Nutzer" Z. 1073ff.) durchgestrichen und berichtigt: nicht
der Platz haelt den Gorilla, sondern B[3] steuert gerade auf playerX (@0x80117d4c) und A[3] hat NAH nur den Biss (@0x80117a60-74);
er haengt an der Klemme knapp ausserhalb des Bissfensters. Gemessen: Ostgasse erreichbar (V3 Biss Bild 54; NB5 exe-Stelle 3 Biss
F1220; Bissplaetze), Nordwest-Ecke ueber den Nordstreifen (Vortritt/Nah-Umweg, O3b Biss Bild 399). Die NB4-Commit-Message
c541415c ("In die Ostgasse kommt kein Gorilla zu Fuss") ist damit ebenfalls berichtigt (diese Nachbesserung).

### NB5 Mangel 3 — Riegel
`unit_r36_zombieki_affen_block` (66 Pruefungen, `F_belege/nb5/nb5_affen_riegel.txt`):
- Original-Gleichheit O1/O3b/O5 jetzt im Modus ORIGINAL-REGEL (`lage <b> 0`): O3b Ende (-23272,14587) / GDB (-23302,14597), kein
  Angriff; O1 (6418,-4399) / (6418,-4409); O5 (-13557,11920) / (-13527,11914), kein Angriff.
- PORT-Modus (1) misst die Verbesserung: O3b Angriff Bild 399, NAH fest 371; O5 Angriff 410, fest 377 (<= 450); Gegenprobe Stand NB4
  (Modus 4) je 900 fest ohne Angriff (= Befund).
- Bissplaetze: Ecke 1649/1649/1833 < 3168 (EIN Gorilla), V-Nische 5809 (zwei) — der Grund des Anstehens als Pin, nicht der Stillstand.
- Nachruecken: Gorilla 1 wird in der Stau-Lage in Bild 400 per Treffer getoetet (HP 1 + `re15_enemy_take_damage`, Leiche bleibt
  liegen) -> Gorilla 2 nimmt den freien Bissplatz und greift an (Bild 988); Stand NB4 nie (1231 fest an der #21-Ecke).
- J-Fenster: kanonischer Kampf mit Nah-Umweg bitgleich (Heavy + 14 Bisse 15/15, Tod +1194), Slot 2 NAH-Anker 353 < 354.
- Unveraendert gruen: 16 Seeds AUS 38439 -> AN 5174, sieben Szenen, 4b, Stau-Block (Modus 0/3/1).

### NB5 Tests (probes/r36_zombieki.cmake, unveraendert registriert — keine neue Testdatei)
| Test | Mangel | Misst | Ergebnis |
|---|---|---|---|
| `unit_r36_zombieki_affen_block` | M1, M3 | wie NB4 (16 Seeds, sieben Szenen, 4b, Stau-Block) + NB5: Original-Gleichheit O1/O3b/O5 im Modus Original-Regel; Nah-Umweg O3b/O5 (Angriff, NAH fest <= 450) mit Gegenprobe Stand NB4 (900 fest); Bissplaetze Ecke < 3168 / V-Nische >= 3168; Nachruecken nach Ausfall; J-Fenster (kanonischer Kampf bitgleich, Slot 2 NAH-Anker 353) | **66/66** |
| `unit_r36_zombieki_sicht`, `_eintritt`, `_wecker`, `_szenen`, `_affen` | P1/P3 (erfuellt) | unveraendert (kein P1/P3-Code geaendert) | gruen |
| Spur-J `unit_r35_affen_*` (20), `unit_maggot_*` (3), `unit_r17_waffen_loop_pin` | Original-Nahjagd, kanonischer Kampf | bildgenau gegen GDB | gruen |

**Gezielte Laeufe (keine Voll-Suite im Spurbaum):**
- Regex der Abnahme 4 (Module/Raeume des Zweig-Diffs + alle Gegner-KI-Tests):
  `ctest --test-dir re15_port/build -R "affen|maggot|re2z|re2_zombie|zombie|dog|hund|crow|kraehe|1030|1040|1140|1210|1100|10d0|10b0|1010|1220|11c0|r35_raeume|r35_inhalt_zombies|r35_cut1150|r35_zgirl|r35_fenster_kraehe|leiche|corpse|r34n_f|r36_zombieki|nav|unit_enemy_|room1070_wake|ai_flavor|re2_room1090|ada_eintritt|r17_waffen_loop|schatten|shadow|grab|griff|victim|re2_crawler|knockdown|stagger|hit_reaction|re2_livepath|re2_hit_repeat|sca_contact90|sca_wall_hit|npc_wall_clamp|r16_aufstehen|r34_reaktion|r34_schaden|unit_damage|re2_gore|r26_mg_blut|re2_stepsize|re2_kfjump|poise|re2_hp_model|room_probe_sweep|r35_granate|r35_redhawk|r35_werfer|r34_granaten|spider_ai|adult_spider|cockroach|unit_npc_ai|zgirl_ai|birkin|writher|alligator|fx_emitter|tyrant|ivy_ai|re2_spider|baby_spider|gator|g5_|spinne|autofire|r35_entladen_gegner" --timeout 600 -j 6 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 232** (511 s; `F_belege/nb5/nb5_ctest_gezielt.txt`; darin alle 6 Spur-Tests, alle 20
  `unit_r35_affen_*`, `integration_r35_cut1150`, `integration_r35_zgirl`)
- nach dem letzten Bau (nur der Riegel geaendert: Nachruecken per Treffer) `ctest --test-dir re15_port/build -R "r36_zombieki|unit_r35_affen|unit_maggot|r17_waffen_loop" --timeout 600 -j 6 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 30** (`F_belege/nb5/nb5_ctest_spur.txt`); Riegel direkt `test_r36_zombieki_affen_block riegel` **66 Pruefungen, 0 Fehler** (`F_belege/nb5/nb5_affen_riegel.txt`)

### NB5 OFFEN
- **Anstehen** (Nordwest-Ecke: Stellen 4/11/12/13, Tuerweg t11b0kampf): der zweite Gorilla kann nicht angreifen, solange der erste den
  einzigen Bissplatz haelt (Bissplaetze: Durchmesser 1649..1833 < 3168) — im Original ebenso. Das Abnahme-4-Kriterium ist dort fuer den
  zweiten Gorilla nicht durch Angreifen erreichbar (gemessen, Gegenprobe ohne Belegt-Test: nur Umzug hinter den ersten). Nicht gebaut:
  zielloses Hin-und-her-Laufen allein fuer das Messmass.
- ~~**Gleiten an Schraegkanten** setzt den NAH-Anker (400) zurueck: Sonde `lage 900 1 1 5858 -5716 0 5876 -8818 3077 3 0 9434 2189 2656 0 0`
  (EIN Gorilla am Ostgassen-Eingang, GDB-Lage O4) gleitet F301-F330 um ~1000 nach Westen und pendelt dort um 300..400 — der Zaehler
  kommt nicht auf 354, fest 892 (lage-Mass). In der exe (Stelle 3) gleitet er nicht und der Nah-Umweg loest. Naechster Weg: Anker an
  den Abstand zu Leon koppeln (seitliches Gleiten ohne Annaeherung = kein Fortschritt) — dann aber das J-Fenster mit derselben
  Zaehlung neu messen (der +0x1de-Zwilling ohne Anker erreicht im kanonischen Kampf 444).~~
  **BERICHTIGT (NB6, Abnahme 5 Mangel 3; gemessen):** dieselbe Sonde im Modus Stand NB5 (`lage 900 6 1 ...`, Spur je Bild,
  `F_belege/nb6/nb6_gleiten_stand_nb5.txt`): G1 gleitet F301..F307 von (5869,-8818) nach (4992,-8945) (Resets F303/F307, ein weiterer
  F422), danach steigt der NAH-Zaehler ungestoert — **354 in F776, 478 in F900**; ab F777 antwortet `r36_nah_route` in JEDEM Bild mit
  Grund 2 = "kein freier Bissplatz" (124 von 124 Bildern). Die Sonde hat nicht "EIN Gorilla": G2 (Start (9434,2189)) springt F300 in die
  Gasse und beisst Leon (5268,-5719) ab F330 fortlaufend (265 Angriffsbilder) aus (7132..7767, -6216..-6821). Bissplaetze um Leon
  (`platz 5268 -5719 50`): 1431, x 6418..7968, z -8469..-4469, Durchmesser 4000 — ohne Koerperkontakt zu G2 (`platz 5268 -5719 50 7139
  -6520`): **0**. Der Fall ist ANSTEHEN (G2 belegt den einspurigen Bissstreifen), kein Gleit-Problem; der damals genannte "naechste Weg"
  (Anker an den Abstand koppeln) haette ihn nicht geaendert. NB6 laesst ihn bewusst so (G1 892, frei 0).
- ~~**Gassenmitte** (GDB O1, exe-Stelle 8 Slot 3 468 Bilder bei (6418,-3617)): Gorilla mit Sicht im 0x10-Pad verkeilt; sein Rasterknoten
  liegt in der Suedkomponente, kein Bissplatz erreichbar -> kein Umweg (Original O1 ebenso; seit NB3 unveraendert).~~
  **BERICHTIGT (NB6):** gemessen betraf das im PORT-Modus BEIDE Gorillas (O1: G1 1368 / G2 1615 NAH fest ohne Angriff, Abnahme 5 2.4) und
  einen einzelnen Gorilla bis ~1530 Bilder (2.5); "wie im Original" war kein Grund, es so zu lassen (der Auftrag verlangt die
  Verbesserung). Behoben in NB6 (Mangel 1, Abschnitt "Nachbesserung 6").
- **Kanonischer Ankunftskampf**: Slot 2 wartet weiter ~12 s an der V-Spitze (Original, Spur-J-Riegel `szene`); der Nah-Umweg beginnt
  bewusst erst danach (354).
- PSX-Laufzeit: der Nah-Umweg prueft je Bild alle Rasterknoten (~5000) auf Bissweite + Belegung und rechnet A* — vor einem PSX-Port
  fuer ROOM11C0 als Tabelle vorzuberechnen (wie NB3/NB4).
- Vorgaenger-OFFEN (ROOM10B0-Terrasse, Moebel-Tabelle, Stelle 7 FERN 108) unveraendert.

### NB5 Fuer den Nutzer
- Neue Assets: keine. Sprachdateien: keine. Bedienhinweise: keine. Entscheidungsfragen: keine.
- Spuerbar neu: Bleibt ein Gorilla knapp vor Leon an einer Wagenkante haengen — am Eingang der schmalen Gasse zwischen den Wagen
  (Ostseite), an der Ecke vor der Nordwest-Ecke hinter dem Block, am Nordende des Westgangs —, sucht er sich nach rund 12 Sekunden
  zu Fuss den Weg zu einem freien Platz neben Leon und greift an. Im Original (und bis v0.8.23) wartete er dort, bis Leon herauskam.
- Bleibt wie im Original: der erste Kampf direkt nach der Ada/Marvin-Szene (der erste Gorilla wartet dort etwa 12 s an der
  Wagenkante, bis der zweite Leon anspringt — diese Szene ist Bild fuer Bild am Original gemessen und festgeschrieben).
- Anstehen: In der Nordwest-Ecke hinter dem Block passt nur EIN Gorilla in Bissweite. Greift dort einer an, wartet der zweite, bis
  Platz ist; ~~faellt der erste aus oder laeuft Leon aus der Ecke, kommt der zweite nach.~~
  **BERICHTIGT (NB6, Abnahme 5 Mangel 3):** faellt der erste aus, kam der zweite im Stand NB5 erst nach einer zweiten Wartezeit nach —
  gemessen 588 Bilder (~20 s) nach dem Tod des ersten (Sonde Nachruecken: Tod F400, Biss F988), in der exe (`nw12_schuss`) ~375 Bilder
  Stillstand nach dem Tod, 532 Bilder NAH fest. Ausserdem blieb ein Gorilla, der in der schmalen Gasse zwischen den Wagen (Ostseite)
  feststeckte, dort unbegrenzt stehen (~1530 Bilder = 51 s; zwei Gorillas 1368/1615 Bilder). Beides ist in NB6 behoben ("Nachbesserung 6,
  Fuer den Nutzer").
- Berichtigung zu NB4: dort stand, ein Gorilla "passe nicht hinein". Das war falsch — er blieb an der Wagenkante haengen, weil er im
  Original stur geradeaus auf Leon zulaeuft.

### NB5 Ergebnis je Mangel (Kurzfassung)
| Mangel | Ursache | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| 1 NAH-Stillstand an den Stoss-Zielen | (a) EIN naher Gorilla an der Klemme knapp ausserhalb der Bissweite (B[3] gerade auf playerX @0x80117d4c, A[3] nah nur Biss @0x80117a60-74); (b) Stellen 4/11/12/13 + t11b0kampf: der ZWEITE Gorilla steht an, der einzige Bissplatz ist belegt | Spuren je Stelle; Bissplaetze Ecke 1649..1833 < 3168; J-Fenster 353 (Messreihe N) | PORT-WAHL Nah-Umweg zum freien Bissplatz ab R36_NAH_START 354 (= J-Fenster + 1), Belegt-Test | exe Stelle 3 1653/1875 -> 429/368 (Kriterium erfuellt), Stelle 1 826 -> 591, 14 Stellen 10466 -> 7500; kanonischer Kampf + 12 Stellen bitgleich; (b) gemessen nicht erreichbar (WIDERLEGT fuer den zweiten Gorilla), Nachruecken gebaut |
| 2 falsche Begruendung | "passt nicht hinein" / "kommt nicht zu Fuss" | V3 Biss 54, exe Stelle 3 Biss F1220, Bissplaetze, GDB O1 Gassenmitte | an beiden Stellen durchgestrichen + berichtigt | — |
| 3 Riegel pinnt Stillstand | O1/O3b/O5 im PORT-Modus | Abnahme 4 | Gleichheit im Modus Original-Regel, PORT-Modus misst Angriff/<= 450 mit Gegenprobe NB4 | Riegel 66/66 |

---

## Nachbesserung 6 (nach Abnahme 5, 2026-10-11) — fortlaufend

Grundlage: `F_abnahme_5.md` (Stand e856447f/8030e41f: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3). P1/P3 nicht
anfassen (Riegel `unit_r36_zombieki_{sicht,eintritt,wecker,szenen,affen}` gruen), Spur-J-Riegel `unit_r35_affen_*` und das
J-Fenster (`kanon`: Heavy +363, Bisse 468 521 571 ..., Tod +1194, Slot-2-NAH-Anker 353) unveraendert.

| Mangel | Kern | Kriterium der Abnahme | Weg |
|---|---|---|---|
| 1 | NAHER Gorilla, den nur das 0x10-Pad #36 (ROOM11C0.RDT 0x015d8) von Leon trennt, steht unbegrenzt (Gasse einzeln ~1530, O1 PORT G1 1368 / G2 1615, exe-Stelle 8 S3 468) | beide Lagen: jeder nahe Gorilla greift nach <= 450 NAH-fest an oder steht nachweislich an (kein freier Bissplatz); exe-Stelle 8 S3 <= 450; J-Pins unveraendert; Riegel mit Gegenprobe NB5 | Messen, WO er steht (Pad-Mitte/Pad-Kante, Kontaktcode, Sicht), Original-Sprungpfad A[4] Path A @0x80117ecc-8011802c lesen (Bedingungen, Richtung, Landung); Nah-Umweg auch ueber 0x10-Pads (A* mit Pad-Kanten wie `r36_uw_route`), Sprung = Original |
| 2 | 354-Bilder-Wartezeit beginnt nach Anker-Reset (Rutschen an der Kante) und nach dem Tod des Partners neu (Nachruecken 588, exe nw12_schuss S2 532) | Nachruecken <= 450 nach dem Tod; exe nw12_schuss kein Gorilla > 450 NAH fest; J-Fenster bitgleich; Riegel prueft die Schranke | Messen, WAS den Zaehler zuruecksetzt (Spur je Bild); Zaehlung an den Fortschritt zu Leon koppeln bzw. Anstehen-Zeit anrechnen; J-Fenster mit derselben Zaehlung neu messen |
| 3 | Dossier: NB5-OFFEN "Gleiten" gemessen falsch (Anstehen hinter G2), NB5 "Fuer den Nutzer" ohne Verzoegerung/Gassenmitte | beide Stellen nach Messung berichtigt | durchstreichen + berichtigen (Messwerte der Abnahme 5 2.2 und eigene) |

### NB6 Messung vorher (Stand NB5 = HEAD 8030e41f; Sonden je eigener Prozess, `F_belege/nb6/`)
Die Sonde `lage` hat seit NB6 den Modus 6 = Stand NB5 (alle NB6-Erweiterungen aus, `re15_r36_affen_nb6_enable(0)`); die Spur je Bild
zeigt zusaetzlich `ng` (Antwort von `r36_nah_route`: 1 kein Startknoten, 2 kein freier Bissplatz, 3 schon am Platz, 4 kein Weg, 5 Pad vom
anderen belegt, 10 zu Fuss, 11/12 ueber ein Pad, 13 verkeilt-Sprung), `up` (Pad) und `zc` (Kontaktcode der 0x10-Zone).

| Lage (Abnahme 5) | Stand NB5 (Modus 6) | Ursache (gemessen) |
|---|---|---|
| Gasse einzeln `lage 2400 6 .. 7873 -1412 0 6418 -4333 3373 3 0 -9013 -15461 2656 0 0` | G1 kein Angriff, NAH fest 2258 (lage-Mass 2291) — bitgleich zur Abnahme 5 | G1 mitten im Pad #36, eigene Fuss-Komponente ohne Bissplatz -> `r36_nah_route` 0 (Grund 2) |
| O1 `lage 2400 6 .. 6000 -9000 1332 -5525 -14883 2731 3 0 -8915 -12487 93 3 0` | G1 1368 / G2 1615 NAH fest, G2 nie, G1 letzter Angriff F1032 — bitgleich zur Abnahme 5 | beide Gorillas suedlich/in Pad #36, Leon (7873,-1412) noerdlich; Bissplaetze um Leon 4557 (zwei passen), aber nur ueber das Pad |
| exe-Stelle 8, nachgestellt `lage 900 1 30 7320 -7071 0 6418 -3617 857 3 0 -9013 -15461 2656 0 0` (Endlage der exe F648) | G1 900/900 Bilder bei (6418,-3617), `ng 1` = KEIN Startknoten | G1 mit der Mitte im Pad #36 VERKEILT: `r36_uw_knoten` findet keinen freien Knoten, den er gerade erreicht; auch nach dem Umdrehen nach Norden (Yaw 857 -> 3175) bewegt er sich 900 Bilder nicht (die Klemme setzt jeden Schritt auf die Vorlage zurueck) |
| Nachruecken `lage 1200 6 10 -26606 14578 18 -23302 14489 2067 3 0 -20579 16118 1885 4 0 400 2` | G2 Biss F988 = 588 Bilder nach dem Tod von G1 (F400) | NAH-Anker-Resets durch Gleiten an der #21-Kante nach dem Tod (F480/F502/F527/F579), Zaehlung von vorn |
| exe `nw12_schuss` (Abnahme 5 jobsC) | S2 532 NAH fest (F1464+141, F1605+391) | S3 stirbt F1620, S2 gleitet an der #21-Nordkante, Zaehlung von vorn |

Warum nicht einfach "Gleiten zaehlt nicht als Fortschritt"? Gemessen am kanonischen Kampf (Sonde `kanon 1 1 1`, Spur je Bild,
`F_belege/nb6/nb6_kanon_zaehlregeln.txt`, Auswertung `zaehl.py`/`zaehl2.py`): Slot 2 steht dort ab +30 an der V-Spitze (d 3093 -> 3080)
und KRIECHT bis +108 an der Kante entlang (Abstand aendert sich um 13); der Lage-Anker (400) setzt dabei einmal zurueck (+108), daher
353. Jede Fortschritts-Zaehlung, die Gleiten nicht als Fortschritt wertet, kommt dort VOR dem ersten Biss auf 434 (reiner Abstand)
bzw. 434..438 (Lage-Anker mit Abstands-Fortschritt >= 100/200/300) — mit Schwelle 354 aenderte sie den gepinnten Original-Kampf,
mit Schwelle >= 435 waere der Nah-Umweg an jeder anderen Stelle 81 Bilder spaeter (Stelle 3: 368 -> ~449). Deshalb bleibt die
Lage-Zaehlung; NB6 behebt die gemessene Ursache selbst: den Tod des Partners (Mangel 2) und das Pad (Mangel 1).

### NB6 RE-Belege (selbst gelesen, re15_disasm.py; STAGE1.BIN roh @0x80100000, PSX.EXE)
- **Gorilla-Zustaende**: Wurzel `lbu v0,4(v1)` / `sll v0,v0,2` / `lui at,0x8012` / `addiu at,at,5064` / `lw v0,0(at)` / `jalr v0`
  @0x80116e00-20 -> Tabelle @0x801213c8: [0] 0x80116f50 INIT, [1] 0x80117254 ACTIVE, [2] 0x8011af5c HURT, **[3] 0x8011b6fc DEATH**,
  [4]/[5] 0x8011bdec, [6] 0x8011c598, **[7] 0x8011be54 CORPSE**.
- **Abstand +0x1d4**: `jal 0x80065f60` (SquareRoot0) / `sh v0,468(v1)` @0x80117300-14 (nur im ACTIVE-Gehirn; im Tod bleibt der letzte
  Wert). Bissweite a804(0xbb8, 0x180): `ori a0,zero,0xbb8` @0x80117a60 (Abnahme 5 Abschnitt 1, Sprungziel FUN_8001a804 gelesen).
- **Pad-Sprung A[4] Path A** (0x80117e9c..): `lbu v0,482(v1)` (+0x1e2) / `beq v0,zero,0x80118028` @0x80117ed4-dc; `ori a3,zero,0x10`
  @0x80117ee0, `lbu a2,130(v1)` (+0x82) @0x80117ee4, `lhu a1,6(v0)` (box[6]) @0x80117ef4, `addiu a0,a0,52` (+0x34) @0x80117ef8,
  `jal 0x8003b93c` @0x80117efc, `addiu a1,a1,100` @0x80117f00; Yaw-Fenster `lbu a1,144(a0)` (+0x90) / `lh v1,106(a0)` / `andi v0,a1,0xf0` /
  `sll v0,v0,4` / `subu` / `addiu v0,v0,512` / `andi v0,v0,0xfff` / `slti v0,v0,1024` @0x80117f18-34; Sprung `sb a1,159(a0)` (+0x9f)
  @0x80117f40, `ori v0,zero,0x7` / `sb v0,5(v1)` @0x80117f3c/50, `sb zero,6(v0)` @0x80117f60, `ori v0,zero,0x1` / `sb v0,7(v1)`
  @0x80117f6c-74. Ausfuehrung B[7]: +0x7 != 0 -> `lbu v0,159(v1)` / `andi 0xf0` / `sll 4` / `sh v0,106(v1)` (Yaw = Code) /
  `sb a0,130(v0)` (+0x82 Band) @0x80118ac0-af0 — im Flug klemmen die Band-0-Wagenzellen nicht (Port-Doku re15_collision.c).
- **Kontaktcode** FUN_8001bf04 (PSX.EXE): Vorgabe `ori t2,zero,0xf00` @0x8001bf74 (Mitte/Ecke = Platzhalter), links `addu t2,zero,zero`
  @0x8001bfa4 (0x000), rechts `ori t2,zero,0x800` @0x8001bfac, davor `ori t2,zero,0xc00` @0x8001bfe8, dahinter `ori t2,zero,0x400`
  @0x8001bfec, `jr ra` / `addu v0,t2,zero` @0x8001bff0-f4; FUN_8003b93c legt (Code >> 4) + 8 + (u1 & 3) nach +0x90 (`sb v0,144(a0)`
  @0x8003ba8c, Port re15_collision_zone_query). Fuer Pad #36 (u1 0x10): Sued 0xc8 -> Sprung nach Norden, Nord 0x48 -> nach Sueden.
- **Pad #36** (ROOM11C0.RDT 0x015d8, SCA-Eintrag 12 Byte, selbst geparst): x 4900..9500, z -6080..-2180, u1 0x10, Band 0 (floor 0x02).

### NB6 Umsetzung (zombieki_r36.c, keine neue gemeinsame Datei; Header-Doku NB6)
- **Mangel 1 — Nah-Umweg ueber 0x10-Pads (PORT-WAHL)**: `r36_nah_route` sucht wie bisher zuerst den naechsten freien Bissplatz in der
  EIGENEN Fuss-Komponente (Grund 10). Gibt es dort keinen, nimmt er (neu) den naechsten freien Bissplatz in LEONS Komponente
  (Leon-Knoten wie `r36_uw_route`: Strecke frei fuer die Spieler-Klemme, Radius 450, Maske 1 — ein Knoten < 0xbb8 hinter einer Wand waere
  ein Biss durch die Wand), erreichbar ueber die Pad-Kanten des FERN-Umwegs (`r36_uw_kanten`, A* `r36_uw_astern`), und folgt dem Weg
  mit derselben, aus `r36_uw_route` unveraendert herausgeloesten Funktion `r36_weg_folgen` (Weg zum Anlauf = Grund 11, Anlauf auf die
  Pad-Mitte = Grund 12). Der Sprung ist der Original-Pad-Sprung (`r36_pad_sprung_code`: Yaw-Fenster @0x80117f18-38, +0x9f/+0x5..7
  @0x80117f40-74), im Nah-Umweg nur mit einem SEITENcode (0xf8 = Mitte ohne Richtung springt nicht). Nimmt der andere Gorilla dasselbe
  Pad und steht naeher an der Mitte (NB2-Regel `r36_pad_belegt`), steht er an (Grund 5) statt ueber ein fernes Pad zu laufen
  (gemessen ohne diese Regel: O1-G2 nahm Pad #47 im Westen, wurde FERN und irrte 500 Bilder).
- **Beginn**: zu Fuss weiter erst ab R36_NAH_START = 354 (J-Fenster: der kanonische Kampf hat einen Fuss-Weg, Slot 2 wartet 353), ueber
  ein Pad ab R36_FEST_START = 45 — derselbe Beginn wie der FERN-Umweg (NB3): ohne Fuss-Weg haengt der nahe Gorilla genauso wie ein ferner.
  Erste Fassung mit 354 auch fuer das Pad (gemessen, `F_belege/nb6/nb6_lage_vorher_nachher.txt`): O1 G1 549 / G2 888 NAH fest (Leon
  wird durch die Gasse hin- und hergeschoben, jede Runde kostete erneut 354 Bilder) -> mit 45: 0 / 248.
- **Verkeilt** (Mitte im Pad und KEIN Knoten gerade erreichbar, exe-Stelle 8): Startknoten ohne Streckenpruefung, und fuehrt der Weg
  ueber genau dieses Pad, dreht er in die Sprungrichtung und springt mit dem Code der Pad-Seite, von der der Weg das Pad ueberquert
  (`r36_seiten_code` = FUN_8001bf04 + @0x8003ba8c am Startknoten der Pad-Kante; Grund 13). PORT-WAHL: die Mitte hat keinen Code
  (0xf00), zu Fuss kommt er nicht frei (Sonde: 900/900 Bilder ohne Bewegung, auch nach dem Umdrehen zum Ausgang), nur der Sprung
  wechselt das Band (+0x82 @0x80118af0).
- **Mangel 2 — Nachruecken**: `re15_r36_affen_fest_takt` erkennt den Tod eines Gorillas (Zustand 3/7, einmal je Tod, `s_r36_tot`).
  Stand er in Bissweite (+0x1d4 < 0xbb8), wird ein Bissplatz frei: jeder Gorilla, der gerade NAH in der Jagd wartet (Sub 3,
  0xbb8 <= +0x1d4 < 0x1771), bekommt die Wartezeit als abgelaufen (NAH-Zaehler := R36_NAH_START, Anker := jetzige Lage). Kein Eingriff in
  den kanonischen Kampf (dort stirbt kein Gorilla vor Leon).
- Nur-Sonden: `re15_r36_affen_nb6_enable(bits)` (0 = Stand NB5; kein Aufrufer in engine/platform), `re15_r36_affen_nah_grund`,
  `re15_r36_affen_bissplaetze_frei` (freie Bissplatz-Knoten ausser Koerperkontakt = "steht nachweislich an" bei 0). Sonde `lage`:
  Modus 6 = Stand NB5, Spur mit `ng`/`up`/`zc`, LAGE-Zeile mit letztem Angriff/Angriffsbildern/Leon-Endlage/`frei`; `platz <lx> <lz>
  [schritt] [ox oz]` schliesst Koerperkontakt zu (ox,oz) aus.

### NB6 Bau (Fortsetzung nach dem Wochenlimit, 2026-10-11 11:1x, anderer Account)
`local_build.sh configure` + `build` am Stand 94d762de: OK (`F_belege/nb6/nb6_build_configure.log`, `nb6_build_build.log`; die drei
Warnungen liegen in main.c 8668/9964/10343 ausserhalb des Spur-F-Diffs). Riegel `unit_r36_zombieki_affen_block` direkt 74/74
(`nb6_riegel_direkt.txt`), ctest `r36_zombieki_affen|r35_affen|maggot` 25/25 (`nb6_ctest_riegel_lauf1.txt`). Die exe-Laeufe des
Vorgaengers (`re15_pc_nb6b.exe`, Scratch F_nb6) sind gy.log-bitgleich zu den eigenen Laeufen der frischen Kopie `re15_pc_nb6f.exe`
(Stelle 8, Stelle 3, nw12_schuss, szene11c0, t11b0szene, t11b0kampf) — derselbe Code.

### NB6 Messung nachher — Sonden am Stand NB6a (= 5a02b824/94d762de; `F_belege/nb6/nb6_lage_*.txt`, je Lauf eigener Prozess)
| Lage | Stand NB5 (Modus 6) | NB6a (Modus 1) |
|---|---|---|
| Gasse einzeln | G1 kein Angriff, NAH fest 2258 | G1 Angriff **313**, fest 0 — Grund 11: er laeuft erst nach Sueden zum Anlaufpunkt (7000,-8700) hinaus und springt F210 nach Norden |
| O1 | G1 1368 / G2 1615, G2 nie | G1 195 / fest 0; G2 Angriff **937**, fest 248 (Nah-Umweg ueber #36 ab nah 45) |
| exe-Stelle 8 nachgestellt | G1 900/900, kein Angriff (`ng 1`: kein Startknoten) | G1 Angriff **112**, fest 0 — Grund 13: verkeilt, Sprung mit dem Code der Nordseite 0x48 nach Sueden |
| Nachruecken (G1 stirbt F400) | G2 Biss F988 = 588 nach dem Tod | G2 Biss **F505 = 105** nach dem Tod |
| kanon (J-Fenster) | 363 468 521 ..., Tod +1194, Anker 353 | **bitgleich** |

### NB6 Zufallsgang 52 Seeds x 6000 Bilder (`F_belege/nb6/seeds/`, Sonde `eins <i> 1 6000 <anker> 0 1 1 <nb6-bits>`, je Lauf eigener Prozess)
| Stand | Anker 400 blockiert | Griffe | Anker 40 (Stillstand) |
|---|---|---|---|
| NB5 (bits 0; bitgleich zu NB4 `s400d_k22.txt`) | 16352 | 242 | 125 |
| NB6a (bits 3, Stand 5a02b824) | 17242 | 252 | **0** |
| NB6b (bits 3, Stand c9f3a8eb, s. u.) | 17638 | 253 | **0** |

Kein Gorilla steht mit NB6 noch >= 90 Bilder STILL (Anker 40: 125 -> 0). Das Anker-400-Mass steigt um 890/1286 Bilder: je Seed
(`zg_diff_nb5_nb6a_nb6b.txt`) 4 besser / 4 schlechter / 44 bitgleich. Die zwei groessten Zuwaechse gemessen mit der neuen Spur je Bild
(`eins ... <von> <bis>`, `zg_seed18_nb6_spur_F3000_3740.txt`, `zg_seed27_nb6_spur_F2100_2980.txt`): Seed 0x7ed38e72 (261 -> 597/633) und
0x0ec6d5ab (0 -> 370) landen in der **vorbestehenden Sprungschleifen-Klasse** — Pad #47 mit der Mitte (Kontaktcode 0xf8), Original-
Sprung in Richtung 0xf00 (A[4] Path A, bzw. die NB1-Regel "verkeilt -> in Code-Richtung springen"), Landung wieder in einer Pad-Mitte
oder vor dem 0x20-Pad #19 bei (-13518,..), dort der Original-Blindsprung (+0x7 = 3 @0x80117fe4-80118024, NB4 dokumentiert) bzw. der
an der Klemme x -15173 zurueckgeworfene Sprung (Sub 7/2/1 ohne Ortswechsel, Seed 27 ab F2409 zehnmal). Dieselbe Klasse zeigt der
Stand NB5 in 7 Seeds bitgleich (0x76026075 sub7 504, 0xa42d215f 494, 0xacfe4f5c 263, 0x4b35c90d 234, 0x05f5a7ae 228, 0xe3fa3e43 186,
0x3cf19695 83) und der Original-Modus (modus 0) mit 896/702. NB6 erzeugt keinen neuen Mechanismus, sondern fuehrt ueber die
veraenderte Bahn (erster Angriff per Pad-Sprung, Leon laeuft danach anders) zwei weitere Seeds hinein. Seed 0xbeef (126 -> 769): sechs
Strecken je 99..152 Bilder, fuenf davon das bekannte Anstehen in der Nordwest-Tasche (Griffe 2 -> 10). Seed 0xc413afd1 (409 -> 838):
fuenf Strecken 141..205, Jagd mit Sicht an der Klemme (Original), kein Haenger. Sub-4-Erweiterungen gegen die Sprungschleifen hat NB1
gemessen und verworfen (Sprungketten / Hin-und-her); sie bleiben OFFEN (Original-Klasse).

### NB6b — Befund Stelle 3 in der exe und Korrektur (Stand c9f3a8eb)
**Messung vorher** (exe Kopie `re15_pc_nb6f.exe`, 14 Stellen wie Abnahme 5, `F_belege/nb6/nb6a_exe_14_stellen_ausw5.txt`): Stelle 8
S3 468 -> **0** (Angriff F274), nw12_schuss S2 532 -> **141** NAH fest (nur noch die Strecke F1464+141 VOR dem Tod von S3), kanon
szene11c0/t11b0szene, t11b0kampf und die vier Nordwest-Stellen gy.log-bitgleich zu Abnahme 5 — ABER **Stelle 3 (6000,-9000): S2 greift
NIE an** (Abnahme 5: F1596; fest 367 statt 429), Leon stirbt F1947 durch S3 allein.

**Ursache (gemessen, neue Messschiene `RE15_AFFEN_LOG=<datei>`: je Bild je Gorilla ua/up/nu/nah/ng, gy.log dabei bitgleich;
`nb6_stelle3_affenlog_vorher.txt`)**: S3 steht ab F1580 mit der Mitte in Pad #36 (6418,-3617) und beisst Leon von dort (ua=0, up=-1 —
er belegt das Pad NICHT; die erste Hypothese `r36_pad_belegt` ist damit widerlegt). S2 steht NAH (+0x1d4 5655) in der Gassenmuendung an
der Suedkante von #36 bei (6418,-6820), Kontaktcode 0xc8. Mit nah 45 antwortet `r36_nah_route` Grund 11 = "Weg zum Anlauf" mit Ziel
(7000,-8700): der Raster-Startknoten der Pad-Kante liegt 1970 entfernt AUSSERHALB der Gasse, weil die Gasse (x 6418..7968, 1550 breit)
schmaler ist als zwei Gorilla-Radien (2 x 1600 @0x80121356) und das Raster (Vorlage + Klemme) dort keinen freien Knoten hat. S2 laeuft
also von Leon WEG nach Suedosten, +0x1d4 kippt ueber 0x1771 (@0x80117bc8) -> FERN -> Sub 4, die NAH-Regel loescht den Umweg; zurueck
an der Kante (Original B[3] gerade auf playerX @0x80117d4c) zaehlt der NAH-Anker von 0: Zyklus F1425/F1633/F1779/F1920, kein Sprung bis
zum Laufende. Im frischen Sondenzustand (`lage 600 1 30 5615 -1214 2048 6418 -3616 2861 3 0 6429 -6881 2969 3 0`) springt G2 dagegen
F120 und greift F215 an — die exe-Bahn bringt S2 naeher an die Kante (dd 5655 statt 5724) und Leon weiter weg, so dass der Umweg zum
Anlaufpunkt ueber 0x1771 fuehrt.

**RE-Beleg der Korrektur**: der Original-Pad-Sprung findet das Pad per Box-Abfrage FUN_8003b93c(+0x34, box[6]+100, +0x82, 0x10)
(`ori a3,zero,0x10` @0x80117ee0, `lhu a1,6(v0)` @0x80117ef4, `jal 0x8003b93c` @0x80117efc, `addiu a1,a1,100` @0x80117f00) — "am Pad" ist
im Original ein Rechteck um das Pad, erweitert um box[6]+100, nicht ein Rasterknoten.

**Umsetzung (PORT-WAHL, zombieki_r36.c)**: `r36_weg_folgen(..., nah)` — im Nah-Umweg gilt "am Anlaufpunkt" auch, wenn der Gorilla das
Pad im Sinne genau dieser Box-Abfrage erreicht hat (`r36_am_pad`, Radius hit_radius_min + 100): gerader Anlauf auf die Pad-Mitte,
Sprung = Original (Yaw-Fenster @0x80117f18-38, +0x9f/+0x5..7 @0x80117f40-74). Steht er dabei mit der MITTE in diesem Pad (Code 0xf8 =
Platzhalter ohne Seite @0x8001bf74; im Anlauf-Modus gaebe es keinen Sprung), springt er wie der Verkeilte mit dem Seitencode der
Startseite (Grund 14; vorher nur verkeilt = Grund 13). Der FERN-Umweg (`r36_uw_route`, nah = 0) ist unveraendert.

**Messung nachher (Sonden `nb6b_lage_*_m1_spur30.txt`, Riegel `nb6_riegel_direkt4.txt`)**: exe Stelle 3 (Kopie `re15_pc_nb6g.exe`,
`nb6_stelle3_affenlog_nachher.txt`): **S2 Angriff F1651**, fest 271 (NB5 429, NB6a 367/nie), S3 F324 / 368 unveraendert; Gasse einzeln
G1 Angriff **157** (NB6a 313; Grund 14: Sprung aus der Pad-Mitte nach Norden); Stelle-3-Endlage G2 **175** (215); Stelle 8 112 (gleich,
Grund 13); Nachruecken 505 (gleich); O1 G1 195 / G2 **937** nach **248** NAH fest, danach 323 Bilder Anstehen an der Wand x 8382
(Leon in der Ecke (11159,-1181) hinter G1, `frei 0`; die Spur `nb6b_lage_o1_m1_spur30.txt` zeigt ab F1650 Grund 2 = kein freier
Bissplatz); kanon bitgleich (363 468 521 ..., Tod +1194, Anker 353); unit_r35_affen_* 20/20.

**Riegel (NB6b)**: die Ostgasse-Pins messen jetzt die NAH-fest-Zeit VOR dem ersten Angriff (<= 450; das Mangel-1-Mass der Abnahme 5
"greift nach hoechstens 450 Bildern NAH fest an") und lassen eine spaetere Wartezeit nur als Anstehen zu (`frei 0`) oder <= 450;
neuer Pin "Stelle 8 verkeilt" (Gegenprobe Stand NB5 900/900 ohne Angriff; NB6 Angriff 112, Grund 13). 77 Pruefungen, 0 Fehler.
