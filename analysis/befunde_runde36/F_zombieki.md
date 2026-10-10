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
- [x] Punkt 1 Zombies: Bisect (keine Regression) / RE-Beleg (RE2 Sichtstrahl, Navigator FUN_8004A808)
- [ ] Punkt 1 Zombies: Umsetzung + Messung nachher + Pin
- [ ] Punkt 2 Monkeys blockiert: Messung vorher
- [ ] Punkt 2 Monkeys blockiert: Beleg / Umsetzung / Messung nachher / Pin
- [ ] Punkt 3 Monkey-Blutlache: Messung vorher
- [ ] Punkt 3 Monkey-Blutlache: Beleg / Umsetzung / Messung nachher / Pin
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

### RE-Belege

(siehe oben; Fortsetzung folgt)

### Umsetzung

(folgt)

### Messung nachher

(folgt)

---

## Punkt 2 — Monkeys ROOM11C0 blockiert

### Messung vorher

(folgt)

### RE-Belege

(folgt)

### Umsetzung

(folgt)

### Messung nachher

(folgt)

---

## Punkt 3 — Monkey-Blutlache laeuft nicht aus

### Messung vorher

(folgt)

### RE-Belege

(folgt)

### Umsetzung

(folgt)

### Messung nachher

(folgt)

---

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
