# Runde 37 Spur H "zombieaktiv" — Dossier

Baum `.claude/worktrees/r37_zombieaktiv`, Zweig `r37/zombieaktiv`, Basis master 44881ac2.

## Wortlaut des Nutzers (AUFTRAG.md, Nachtrag 2 + Ergaenzung)

> die Zombies sind immer noch zu passiv. Beispielsweise, am Ende von STAGE1 Leon, wenn wir die ganzen Zombies spawnen nach
> Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2
> bewegen sie sich doch auch teilweise aktiv im Gang.

> das soll nicht nur da gelten, das darf generell für die Zombies gelten

> schaue einfach nach wie Resident Evil 2 das macht

Lesart: GENERELL alle Zombies (jeder Raum, jede Spawn-Art) bekommen das RE2-Retail-Verhalten (teilweise aktiv im Gang, auf
Leon zu); die Montage nach Irons' Tod ist das Beispiel.

## Stand

- [x] Schritt 0: Lektuere (Vorrunde F/K/E, Port-KI enemy_ai_re2_zombie.c)
- [x] Schritt 1: Messung vorher (exe: 1130/1030/1040 nach der Montage, M1/M2)
- [x] Schritt 2: RE-Belege RE2 (INIT-Deskriptoren, Steh-/Weck-/Wander-Zweig, Zensus 495 RDTs) + RE1.5 (grid-Klassen) +
      RE2-Original gemessen (DuckStation-GDB ROOM20A0, Deskriptor 6 vs. 0)
- [x] Schritt 3: Umsetzung (zombieaktiv_r37.c, Haken scd_vm.c/enemy_ai_re2_zombie.c, Nachspawn 1130 grid 0x0d)
- [x] Schritt 4: Messung nachher (exe N1/N2, Harness, Framedump)
- [x] Schritt 5: Tests (3 Unit-Riegel + 1 Integration, gezielte Laeufe gruen)
- [x] Nachbesserung 1 (Abnahme 0): M1 Zombie-Girl-Zitat auf STAGE4.BIN, M2 zwei Nutzer-Saetze nach Messung, H6 Stil;
      kein Verhalten geaendert (gemessen), gezielte Tests gruen

## Messung vorher

### M1 — echte exe, ROOM1130 nach der Montage (Stand master 44881ac2, Messkopie `re15_pc_r37h_mess.exe`)

Weg: Spielstand + CONTINUE + echte Tuer. Karte `probe_r36_gegnerzahl_karte re15_card.mcr 1150 -17150 -11960 0 9:71 3:94
9:73-76` (ROOM1150 vor der Tuer Slot 0 @0x00D5E nach 1130, Montage gesehen: (9,73)-(9,76)), Aktionstaste -> Tuer ->
ROOM1130 (debug.log `Nachspawn 1130 (Programm 10)` = k_p_1130_nach = EM_1130_LISTE, grid 0x02), RE2-KI (Default),
`RE15_STATE_LOG`. Auswerter `H_belege/ausw.py` (je Zombie Zustandsfolge st/ss1/ss2, Weg, Abstand).

**Lauf a (Leon bleibt an der Tuer (-5900,16350), 1800 Bilder = 60 s):** alle fuenf Zombies (Typ 0x16/0x10/0x10/0x11/0x11,
grid 0x02) bleiben in Zustand 1 / Unterzustand 0 (RE2 EXEC[0] Stehen): Folge `0/0 -> 1/0/0 @6 -> 1/0/1 @7` (Stehen P1),
dazwischen RE2-Wander (P3 @544..812, @876..1232, @1412..1592, @1713..) — Wege 293..9044 Einheiten zufaellig im Gang,
Abstand zu Leon am Ende 20680..28524. **Kein einziger geht auf Leon zu (Unterzustand 1 nie erreicht).**

**Lauf w (Leon geht den Gang hinunter, `R0.33,U25`):** Leon passiert die 1120-Tuer (z -3150..-1150) und laeuft weiter;
die Zombies wechseln erst in Bild 672..702 in den Gang (Unterzustand 1), als Leon bei z ~ -4800 ist (Abstand ~6000-7000,
RE2-Selbstwecker dist < 0x1D4C mit Sicht @0x80101544-7C bzw. DECISION[0] dist < 0x1388 @0x80101308-1C). Leon stirbt
danach im Griff (hp 100 -> -20).

**Ursache (Mechanismus, gemessen):** die Montage-Zombies in 1130 tragen grid 0x02 (re15_irons_tod.h EM_1130_LISTE =
"STEHENDE Form" ROOM1140 sub00 @0x00C12..). Der RE2-INIT bildet jeden RE1.5-Deskriptor ausser 0x0d auf Zustandswort
0x1 = Stehen ab (enemy_ai_re2_zombie.c re2z_init, `else re15_ai_set_state_word(e, 0x1)`), und ein stehender RE2-Zombie
wird nur wach bei dist < 5000 im Vorderkegel +-1024 mit Sicht (DECISION[0] @0x80101308-1C) oder dist < 7500 mit Sicht
nach 150..405 Bildern (@0x80101544-7C), sonst nur Zufalls-Wander (@0x8010151C-40). Der Weg 1150 -> 1120 fuehrt in
>= 8800 Abstand an ihnen vorbei (1120-Tuer z -2150, naechster Zombie z -10800) -> sie bleiben stehen. Das ist genau der
Nutzer-Befund "bleiben einfach nur stumpf alle im Gang stehen".

### M2 — echte exe, ROOM1030 / ROOM1040 nach der Montage (Karte mit allen Montage-Bits: 9:71 3:94 9:73-76 4:4-5 4:15
7:0xd3-0xd7 7:0xc6-0xca, Leon am Eintritt aus 1040 bzw. 1060)

* 1030 (Leon (-26214,-3861), Eintritt aus 1040): die 1070-Kopien (grid 0x00 -> RE2 Stehen) liegen 3800..7600 vor Leon und
  wachen in Bild 1..328 (DECISION[0] bzw. Selbstwecker); Raum-Records (0x0d) und Kriecher (0x0d) gehen ab Bild 0. Leon
  stirbt ohne Gegenwehr nach ~600 Bildern. -> 1030 ist bereits bedrohlich (die Kopien stehen NAH an der Tuer).
* 1040 (Leon (-21008,-13134), Eintritt aus 1060): alle fuenf (0x0d) gehen ab Bild 0 (Folge `1/1@0`), Leon stirbt nach ~900.
* Fazit M1+M2: "stumpf im Gang stehen" ist ROOM1130 (grid 0x02, weit weg vom Weg). 1040/1030 sind aktiv, weil ihre
  Records 0x0d tragen bzw. nah an der Tuer stehen.

## RE-Belege

### R1 — RE2 Retail: der Spawn-Deskriptor entscheidet, ob ein Zombie sofort losgeht (EMZ0.BIN, re2_disasm.py)

Sce_em_set (RE2 Opcode 0x44, 22 Byte, Handler 0x8005714C = Tabelle 0x800A74C8[0x44]): `lhu v0,4(v1)` @0x8005734c /
`sh v0,270(s0)` @0x80057354 -> **+0x10E = pc[4..5]** (Deskriptor), Typ `lbu v0,3(v1)` / `sb v0,8(s0)` @0x80057334-3c.
RE2-INIT 0x8010065C (Tabelle 0x8010C830[0]) wertet `+0x10E & 0x3F` aus:
* Grundwort `addiu v0,zero,1` / `sw v0,4(s2)` @0x801006ac-b8 = 0x0001 = Zustand 1 / Unterzustand 0 = **Stehen** (EXEC[0]).
* Deskriptor 2 -> 0x701 Liegen (Clip 22) @0x801009c4-f0; 4 -> 0x701 (Clip 23) @0x801009f4-a10; 5/7 -> Leiche (HP -1,
  +0x10E 0x4002/0x4004) @0x80100a14-aa4; 8 -> 0x801 Fressen (Clip 18) @0x80100ac0-adc; ungerade -> Kriecher @0x80100ae0ff;
  10 -> 0xF01 @0x80100ba0-c38.
* **Deskriptor 6 -> `addiu v0,zero,257` / `sw v0,4(s2)` @0x80100ab8-abc = 0x0101 = Zustand 1 / Unterzustand 1 = GANG**:
  der Zombie geht ab dem ersten Takt auf Leon zu (EXEC[1] 0x80101A40, Ziel = Navigator FUN_8004A808 auf die
  Spielerlage, Wurzel @0x801002d0-358), aus JEDER Entfernung; die Gang-Leiter DECISION[1] 0x80101714 kennt keinen
  Rueckweg ins Stehen (9 `sw ...,4(s0)`, keiner schreibt 0x0001).
* Ein stehender Zombie (Deskriptor 0) wird nur wach bei dist < 0x1388 (5000) + Vorderkegel 1024 + Sicht
  (@0x801012f0-31c), oder nach +0x158 = rand+150 (150..405 Bilder, rand 0..255 @0x80101478-88) bei dist < 0x1D4C (7500)
  + Sicht (@0x80101544-7c); sonst nur Zufalls-Wander (@0x8010151c-40 / P3 @0x801015e0-16a8, Ausgaenge dist < 0xBB8
  @0x80101650-70 bzw. < 0x1D4C mit Sicht @0x80101674-a8). dist = +0x1F0 = SquareRoot0(dx^2+dz^2) (EXE @0x800265a4-e0).

**RE2-Zensus der Zombie-Sce_em_set (Typ 0x10..0x1F, alle 495 RE2-RDTs, Walker r30_re2_scd.py, Skript
H_belege/re2_emset_zensus.py):** 433 Saetze; Deskriptor 0 (Stehen) 214, **6 (Gang) 140 = 32 %**, 8 (Fressen) 13, 2/4
(Liegen) 36, 1/3 (Kriecher) 19, 5/7 (Leiche) 4, 10: 7. Fast jeder RE2-Raum MISCHT 0 und 6 ("teilweise aktiv"), z.B.
ROOM1100 7 von 15 Gang, ROOM1140 (sub00) 7 von 15, ROOM20A0 16 von 18, ROOM20F0 9 von 10, ROOM2070 9 von 12,
ROOM5080 6 von 8; reine Steh-Raeume z.B. ROOM10F0 (7), ROOM2040 (14), F080 (6).

### R2 — RE1.5: welche Deskriptoren "aktiv" sind (STAGE1.BIN, re15_disasm.py)

RE1.5-Zensus Zombie-Sce_em_set (Typ 0x10/11/12/13/16/18, alle 240 RDTs, 419 Saetze): grid 0x0d 142, 0x00 72, 0x02 91,
Liegen/Fressen/Kriecher/Sitzer den Rest.
* **0x0d** (INIT @0x80100f20-54: `ori v0,zero,0x201` / `sw v0,4(v1)` @0x80100f40-44, +0x9 := 0) = sofort ENGAGE -> der Port
  bildet es schon auf RE2 0x0101 = Deskriptor 6 ab (re2z_init `sel == 0x0d`). Raeume: 1030/1040 (die durchs Tor
  kommen), 1200.
* **0x00** = Kampf-Untermodus 0, DECIDE FUN_80101b64 (@0x8011f840[0]): `jal arc_test(0x5f4)` @0x80101b8c, dann
  `sll v0,a0,16 / bne -> Ende` @0x80101bf4-f8 (Spieler im Weitkegel +-0x5f4) und `lhu v0,472(v1) / andi 0x10 / beq`
  @0x80101c0c-18 (Sicht-Bit) -> `ori v0,zero,0x201 / sw v0,4(v1)` @0x80101c1c-20 = ENGAGE — **OHNE Abstandsgrenze**
  (der einzige Abstandstest des Engage-Zweigs: bei dist >= 0x2711 (`sltiu` @0x80101c38) ersetzt ein 1/8-Wurf (`jal rng`
  @0x80101c44 / `andi 0x7` @0x80101c4c) das 0x201 durch 0x801 (@0x80101c54/64); davor Zaehler +0x9C -> 0x101
  (@0x80101ba0-c4) und 0x701 bei dist < 0x7D0 ausserhalb 0x2c8 (@0x80101bd4-f0) — Nachbesserung 1, ganze Funktion
  selbst gelesen). Stage 4 (ROOM4050) laedt STAGE4.BIN, dort liegt dieselbe Funktion bei 0x80101b18 (Nachbesserung 1,
  M1). Der Sichtsensor
  FUN_8001bc08 -> FUN_8001b84c(0x5e8, Spieler) prueft nur Kegel +-0x5e8 + Nibble-3-Strahl (@0x8001b87c-8e8), keine
  Entfernung. => RE1.5-Zombies mit grid 0x00 greifen aus JEDER Entfernung an, sobald sie Leon sehen = die RE1.5-Form
  eines AKTIVEN Zombies.
* **0x02** = DECIDE FUN_80101c7c (@0x8011f960[0]): ENGAGE nur bei dist < 0xFA0 (4000) = schlafender Steher.
* Der RE2-Port (re2z_init) bildet 0x00 UND 0x02 beide auf RE2-Stehen (0x0001) ab: grid-0x00-Zombies sind unter der
  RE2-KI damit PASSIVER als im RE1.5-Original (wach nur < 5000/7500 statt aus jeder Entfernung).

### R3 — RE2 im Original (DuckStation-Spielstaende SLUS-00748, RAM per re2_ss.py)

`SLUS-00748_4.sav` (Stufe 0 Raum 20 = ROOM1140): Zombie Typ 0x10, **+0x10E = 0x0006, Zustandswort +0x4 = 0x00010101
(Zustand 1, Unterzustand 1 = GANG)**, Abstand +0x1F0 = 7489, Sicht +0x154 = 0x0800. `SLUS-00748_6.sav` (Stufe 1 Raum 10
= ROOM20A0, Raumeintritt): Zombie Typ 0x1F, +0x10E = 0x0006 an (-15699,3739) = ROOM20A0 sub00 @0x00A3E (Satzgruppe 2:
sechs Saetze, alle Deskriptor 6), Leon (-15554,-15268) = 19000 entfernt, Zustandswort noch 0 (INIT im naechsten Takt).

### R4 — RE2 im Original GEMESSEN (DuckStation-GDB, `H_belege/re2_gdb_zombie.py`, Spielstand SLUS-00748_6 = ROOM20A0)

Haltepunkt auf der EXE-Gegnerschleife @0x800265E0 (`sw v0,496(s0)` = Abstand +0x1F0, s0 = Entity, einmal je Gegner und
Bild), je Takt Zustandswort/Deskriptor/Lage/Abstand/Sicht; 600 Takte je Zombie. settings.ini (EnableGDBServer,
SaveStateOnExit) und SLUS-00748_resume.sav gesichert und zurueckgeschrieben.

**Lauf 1 (Original unveraendert, alle sechs Deskriptor 6), `H_belege/re2_original_room20a0_takte.txt`:** alle sechs
stehen ab Takt 0 auf Zustandswort 0x0101 (INIT @0x80100ab8-abc), ab **Takt 2 auf 0x00010101 (Gang, Phase 1)**, und gehen
aus 13660..18992 Abstand auf Leon zu: Abstand nach 100 Takten -1536..-3218 (18992->16636, 13660->10442, 16124->14588,
16384->14002, 14372->12072, 18544->16508), d.h. **15..32 Einheiten je Takt**; der erste greift nach ~500 Takten
(0x00030301 bei d=446), alle sechs sind nach 600 Takten 2600..6200 vor Leon.
**Lauf 2 (Gegenprobe Deskriptor 0: beim ersten Takt +0x4 := 0x0001, +0x10E := 0 — der Zustand, den der INIT
@0x801006ac-b8 fuer Deskriptor 0 hinterlaesst), `H_belege/re2_original_room20a0_deskriptor0_takte.txt`:** dieselben
sechs Zombies an denselben Plaetzen bleiben 600 Takte auf 0x00010001 (Stehen P1), Abstand unveraendert 13584..18992;
zwei wuerfeln den Wander (0x00030001, @0x8010151c-40) und schlurfen 600..3400 Einheiten zufaellig.
=> **Im RE2-Original ist "stumpf stehen" bei Deskriptor 0 genau das Verhalten des Ports, und "aktiv im Gang" ist
Deskriptor 6.** RE2 macht seine Zombies dadurch teilweise aktiv, dass rund ein Drittel der Spawn-Saetze Deskriptor 6
tragen (R1-Zensus).

## Umsetzung

**Entscheidung (Mechanismus, nicht Zahl):** RE2 macht Zombies "teilweise aktiv", indem rund ein Drittel der Spawn-Saetze
Deskriptor 6 tragen (Gang ab Spawn, R1/R4). RE1.5-Raeume tragen keine RE2-Deskriptoren, aber ihre eigene Klasse "aktiv":
grid 0x00 greift im Original aus JEDER Entfernung an, sobald er Leon sieht (R2), grid 0x02 nur unter 4000. Die RE2-KI des
Ports kannte fuer beide nur RE2-Stehen. Die Abbildung legt die RE1.5-Klasse "aktiv" auf die RE2-Klasse "aktiv":

| RE1.5 grid (Sce_em_set pc[3]) | RE1.5-Original | RE2-KI v0.8.24 | RE2-KI jetzt |
|---|---|---|---|
| 0x0d (142 Saetze; 1030/1040/1200) | ENGAGE ab INIT @0x80100f40-44 | Gang 0x0101 | Gang 0x0101 (unveraendert) |
| **0x00 (72; 1010/1020/1070/1100/1180/11E0/11F0/1250 + Elza, 4050)** | ENGAGE auf Sicht ohne Abstandsgrenze @0x80101bf4-c20 | Stehen 0x0001 | **Gang 0x0101 = RE2-Deskriptor 6** |
| 0x02 (91; 1090/10B0/10D0/10E0/1140-Wiederkehr/1220 ...) | ENGAGE nur < 0xFA0 (FUN_80101c7c) | Stehen | Stehen (unveraendert) |
| Liegen/Fressen/Kriecher/Sitzer | Skript-/Naehe-Wecker | unveraendert | unveraendert |

Ergebnis-Anteil: von den aufrechten RE1.5-Saetzen gehen jetzt 0x0d + 0x00 = 214 von 305 ab Spawn (RE2: 140 von 354); je
Raum mischen Liegende/Fresser/Kriecher und Schlaefer weiter mit (z.B. 1020: 4 Gang + 4 Liegende; 10E0: 4 Steher + 4
Fresser + 2 Liegende).

**Dateien:**
* NEU `re15_port/include/re15_zombieaktiv_r37.h` + `re15_port/engine/src/zombieaktiv_r37.c`: Marke je Aktor-Platz beim
  Sce_em_set (nur grid 0x00, nur Zombie-Familie re15_re2z_owns_type, nicht waehrend eines Montage-Schritts =
  re15_irons_tod_zustand() != RE15_IT_AUS), verbraucht im RE2-INIT mit Pruefung Typ + Spawn-Lage (der INIT laeuft im
  ersten KI-Takt vor jeder Bewegung). Konstanten: RE15_R37_RE2_WORT_GANG 0x0101 @0x80100ab8-abc, RE15_R37_RE2_WORT_STEHEN
  0x0001 @0x801006ac-b8, RE15_R37_RE15_AKTIV 0x00 (@0x8011f80c[0] -> DECIDE @0x8011f840[0] = FUN_80101b64).
* Haken `scd_vm.c` op_sce_em_set (1 Zeile + Include): `re15_r37_em_set(actor_slot, a, behavior)`.
* Haken `enemy_ai_re2_zombie.c` re2z_init (2 Zeilen + Include): vor dem Grundwort 0x0001 `else if (re15_r37_re2_gang(slot,
  e)) re15_ai_set_state_word(e, RE15_R37_RE2_WORT_GANG)`.
* `irons_tod_1150.c` (Montage-Modul): Nachspawn 1130 (Programm 10, spaeteres Betreten) in der RE1.5-Durchbruch-Form grid
  0x0d (`EM_1130_NACH`, Form der Raum-Records ROOM1040 main00 @0x011FC.. / ROOM1030 main00 @0x01DE6.., die durchs
  Rolltor/Tor kommen; INIT @0x80100f40-44 = ENGAGE; unter der RE2-KI re2z_init sel 0x0d -> 0x0101); Typen/Tot-Bits/Lagen
  unveraendert. Der Montage-SCHRITT (k_p_1130, Spieler geparkt) behaelt die Stehform 0x02. Gegenprobe-Liste
  `k_p_1130_nach_v0824` nur bei `re15_r37_zombieaktiv_enable(0)` (Sonden).
* Riegel-Anpassung `tests/unit/test_r36_zombieki_sicht.c` (Runde 36 Spur F): der Zensus misst das Sicht-Bit STEHENDER
  Zombies; seit der Abbildung reagieren grid-0x00-Zombies in Bild 1 unabhaengig von der Sicht (gemessen: Gegenprobe
  ROOM1010 vor NB1 0 statt > 0 Plaetze -> rot). Der Zensus laeuft deshalb mit `re15_r37_zombieaktiv_enable(0)` und prueft
  dieselben Zombies wie bisher als Steher (danach 15/15 gruen).

## Messung nachher

### N1 — echte exe, ROOM1130 nach der Montage (gleicher Weg wie M1 Lauf a, Bau f3fd3855)
Alle fuenf Montage-Zombies (jetzt grid 0x0d) gehen im ersten KI-Takt nach der Blende (Bild 6) in den Gang (Folge
`0/0/0@1 1/1@6`) und laufen aus 25000..31000 Abstand auf Leon zu (Wege 25000..49000 Einheiten in ~1450 Bildern); der
erste greift in Bild 896, Leon (ohne Eingabe) stirbt nach ~1200 Bildern. Vorher (M1): keiner im Gang in 1800 Bildern.

### N2 — echte exe, ROOM1030 nach der Montage (wie M2)
Die 1070-Kopien (grid 0x00) gehen ab Bild 0 in den Gang (`1/1@0`) statt erst bei < 5000/7500 zu wachen (vorher Bild
1..328); Raum-Records/Kriecher unveraendert.

### N3 — Harness (echte Spielschleife, test_r37_zombieaktiv `mess`)
* ROOM1130 nach der Montage, Leon an der Tuer aus 1150: alle fuenf ab Bild 1 im Gang; Abstand nach 300 Bildern
  30879->25907, 29674->23051, 28616->23743, 27536->20166, 28426->19461 (4873..8965 naeher); Weg in 300 Bildern 5098..10156.
  Gegenprobe v0.8.24: alle fuenf 600 Bilder stehend, Weg 0, Abstand unveraendert.
* ROOM1180 (Normalspiel, 3x grid 0x00), Leon am Eintritt aus 1160: alle drei ab Bild 1 im Gang, Weg in 300 Bildern
  5194/7823/5462; die zwei fernen folgen den Navigationspunkten des Hofs (Steuerziel (1900,13800), dann (1900,-5750)) und
  sind nach 900 Bildern 3579 bzw. 18437 naeher; Gegenprobe: zwei stehen 900 Bilder (Wander), der nahe wacht erst in
  Bild 192 (RE2-Selbstwecker, < 0x1D4C).
* Framedump (exe, Kamera Cut 6 an der Chief-Office-Tuer): `H_belege/nachher_1130_bilder_150_450_750_900.png` — in Bild
  900 hat der erste Zombie die 28000 Einheiten des Gangs zurueckgelegt und greift Leon an der Tuer.

## Tests

Neu (re15_port/tests/unit/probes/r37_zombieaktiv.cmake, Quelle tests/unit/test_r37_zombieaktiv.c,
tests/integration/test_r37_zombieaktiv.cmake):

| Test | misst | Ergebnis |
|---|---|---|
| unit_r37_zombieaktiv_abbildung | Spawn ueber den echten Sce_em_set (VM), alle 14 RE1.5-Formen mit/ohne Abbildung: NUR grid 0x00 1/0 -> 1/1, Gang Bild 1 (<= 2 = RE2 Takt 2); ohne Sce_em_set Steher; liegengebliebene Marke an anderer Lage verworfen; Montage-Schritt Steher | 6/6 |
| unit_r37_zombieaktiv_nachspawn1130 | Nutzer-Beispiel: 5 Montage-Zombies Gang <= Bild 2, >= 4500 naeher nach 300 Bildern; Gegenprobe v0.8.24 keiner im Gang in 600 Bildern | 4/4 |
| unit_r37_zombieaktiv_raum1180 | allgemein, Normalspiel: 3x grid 0x00 Gang <= Bild 2, Weg >= 4500 in 300, naeher nach 900; Gegenprobe keiner <= Bild 2 | 5/5 |
| integration_r37_zombieaktiv | echte exe, Spielstand + CONTINUE + Tuer 1150 -> 1130: Bild 7 5/5 im Gang, Bild 306 5/5 >= 4500 naeher (4892..8948) | gruen (25 s) |

Angepasst: unit_r36_zombieki_sicht (Runde 36 Spur F) — Zensus mit `re15_r37_zombieaktiv_enable(0)`, s. Umsetzung.

### Gezielte Laeufe (KEINE volle Suite im Spurbaum), Endstand Bau nach 38681a0c (Code unveraendert bis zum Abschluss)

Unit/Sonden aller beruehrten Module (RE2-Zombie, Montage 1150, Gegnerzahl, Zombie-KI Runde 36, Raeume mit grid-0x00-Zombies,
neue Riegel), Liste `H_belege/ctest_gezielt_unit.txt`:
```
ctest --test-dir re15_port/build -R "re2|zombie|r35_cut1150|r36_szene1150|r36_gegnerzahl|r36_zombieki|r35_inhalt|lane_b1|kriecher|1010|1020|1070|1100|1180|11e0|11f0|1250|4050|room1140|1140|irons|montage|szenen|gegner|enemy|ai_|r37_zombieaktiv" -E "^integration_" --timeout 900 -j 6
100% tests passed, 0 tests failed out of 155
```
Integration (echte exe) der Montage, der Raeume mit grid-0x00-Zombies (Story Leon/Elza 1010/1020/1100, Elza-Laeufe 1011/1021,
Entladen 1020/11E0/11F0, Dokumente 1010/1020/1180, 10F0-Szene 1180, Zombie-Girls 4050, PRI 1100, Inventar 1071) und der
neue Riegel, Liste `H_belege/ctest_gezielt_integration.txt`:
```
ctest --test-dir re15_port/build -R "^integration_(r35_entladen|r34n_e_dokumente|r35_cut10f0|r35_zgirl$|r36_elza|r36_inventar|r36_pri_schatten|r36_story|r35_cut1150|r36_szene1150|r36_gegnerzahl|r30_cut_blitz|r37_zombieaktiv)" --timeout 1500 -j 3
100% tests passed, 0 tests failed out of 29
```
(Darin die Montage-Riegel integration_r35_cut1150 (Spieler-HP 100 -> min 100 ueber die ganze Kette) und
integration_r36_szene1150, Spur-E-Riegel integration_r36_gegnerzahl; die Runde-36-Spur-F-Riegel unit_r36_zombieki_* im
Unit-Lauf.)

## OFFEN

* Nichts aus dem Auftrag dieser Spur offen. Nebenbefunde (beim Disassemblieren gefunden, keine Passivitaets-Ursache):
* **Zwei Abweichungen im RE2-Steh-Zweig gefunden (nicht Teil der Passivitaet, NICHT geaendert — jede verschiebt den
  gemeinsamen RE2-Zufallsstrom und damit deterministische Riegel anderer Spuren; eigener Schritt mit Strom-Messung):**
  1. DECISION[0] Block 3 (Ausfall-Biss 0xC01 bei dist < 0x7D0, ausserhalb Kegel 1024, Spieler bewegt): Original
     `jal rand` @0x801013bc / `andi v0,v0,0x1` @0x801013c4 / `beq v0,zero,0x801013d4` @0x801013c8 = Commit bei
     (rand & 1) == 1; Port re2z_decide_stand `(re2z_rand() & 1u) == 0u` (umgekehrt). Gleiche Wahrscheinlichkeit
     (1/2), gleiche Wurfzahl, aber die Entscheidung je Wurf ist invertiert.
  2. EXEC[0] P1 Wander-Wurf: Original setzt `sb v0,6(s0)` = 2 @0x80101540 und laeuft im SELBEN Takt weiter in den
     Selbstwecker (@0x80101544-a0, inkl. +0x158-Dekrement), P2 (@0x801015a4) erst im NAECHSTEN Takt (Dispatch @0x80101410-
     50); der Port ueberspringt nach dem Wurf den Wecker (`if (e->sub_state_2 == 1)`) und faellt im selben Takt in P2/P3
     (ein Takt frueher, die P2-Wuerfe rand+150 / Zonen-Wurf 0x8004AA50 einen Takt frueher im Strom).
* Die drei Produzenten des Arme-hoch-Gangs 0x201 in EXEC[1] (@0x80101D68-DBC, @0x80101EC4-F10, @0x80101F14-F60) bleiben
  wie vor dieser Spur gedrosselt (enemy_ai_re2_zombie.c OFFEN-Block, Nahkampf-Zusammenbruch unaufgeklaert) — betrifft den
  Angriffsgang, nicht das Stehen.

## Fuer den Nutzer

* **Was jetzt anders ist (RE2-KI, Standard):** In RE2 entscheidet der Spawn-Satz, ob ein Zombie schon beim Erscheinen
  auf Leon zugeht (Spawn-Typ 6, im RE2-Original nachgemessen: sie laufen aus 19000 Einheiten los und sind nach ~20 s bei
  Leon) oder steht, bis Leon nahe kommt, und sonst zufaellig herumschlurft (Spawn-Typ 0). Im Port gehen jetzt die
  Zombies ab dem Betreten auf Leon zu, die RE1.5 selbst als "kampfbereit" ablegt (sie greifen im Original an, sobald sie
  Leon sehen, egal wie weit). Schlafende Steher, Liegende, Fresser und Kriecher bleiben wie bisher. **Die Aufteilung
  folgt damit den RE1.5-Klassen, nicht der RE2-Quote, und der Port ist jetzt aggressiver als RE2** (gezaehlt ueber alle
  Zombie-Spawn-Saetze beider Spiele, Nachbesserung 1): RE2 laesst 140 von 433 Saetzen ab dem Spawn gehen (32 %; von den
  aufrechten 140 von 354 = 40 %), der Port jetzt 214 von 419 (51 %; von den aufrechten 214 von 305 = 70 %; vor dieser
  Spur 142 von 419 = 34 % bzw. 142 von 305 = 47 %). Betroffen (Leon und Elza): ROOM1010, 1020, 1070, 1100, 1180, 11E0,
  11F0, 1250, dazu 4050 (Zombie-Girls); ROOM1030/1040/1200 waren schon aktiv.
* **Nach Irons' Tod:** im Gang 1130 kommen die fuenf aus dem Besprechungsraum ausgebrochenen Zombies jetzt auf Leon zu
  (wie die Zombies, die in 1040 durchs Rolltor kommen); in 1030 die Zombies aus 1070 ebenso. Die Montage selbst ist
  unveraendert (Leon ist dort nicht im Bild).
* **RE1.5-KI im Menue:** Die neue Spawn-Regel wirkt dort nicht — gemessen in ROOM1180 (`RE15_AI_FLAVOR=re15`): dieser
  Stand und v0.8.24 laufen Bild fuer Bild gleich. Der Nachspawn nach Irons' Tod in 1130 gilt aber auch dort: die fuenf
  greifen ab Bild 6 an (RE1.5-Angriff) und kommen in 400 Bildern 2511..8610 Einheiten naeher; in v0.8.24 standen sie
  (Weg 0..18). Die Montage nach Irons' Tod gibt es im Original nicht, der Nachspawn ist eine Port-Szene.
* Neue Assets fuer das Paket-Gate: keine. Neue Sprachdateien: keine. Keine Bank-9-Bits, keine Text-Ids.
* Fuer den Release-Bau: neue Quelldatei `re15_port/engine/src/zombieaktiv_r37.c` (Android: GLOB-Cache neu konfigurieren);
  +4 Tests (3 unit, 1 integration).

## Nachbesserung 1 (nach Abnahme 0, `H_abnahme_0.md`)

Zwei Maengel, beide Text/Zitat. Am Verhalten ist nichts geaendert: die neu gebaute exe faehrt den Messweg 1130 unter der
RE1.5-KI Zeile fuer Zeile wie die exe der Abnahme (M2, Messung nachher). Protokolle: `H_belege/nb1_stage4_kette.txt`
(M1), `H_belege/nb1_re15ki_messung.txt` (M2b), Helfer `H_belege/nb1_werkzeug/`.

### M1 — Zombie-Girl-Zitat in der falschen Binaerdatei

**Ursache.** Die Girl-Kette im Kopf von `re15_zombieaktiv_r37.h` war mit `re15_disasm.py` ohne `--bin` gelesen, also in
STAGE1.BIN (Voreinstellung des Werkzeugs). STAGE1.BIN traegt eine eigene Kopie des Girl-Codes (`lui v1,0x8011 / addiu
v1,v1,-22328` @0x8011e870-74, `sw v1,0x2bf8(at)` @0x8011e87c: 0x80072bac[0x13] = 0x8010a8c8; Modus-Tabelle
@0x80120230[0] = 0x8010b6d4, DECIDE @0x80120264[0] = 0x80101b64). Dort stimmte jede Zeile, aber ROOM4050 laedt diese
Datei nicht.

**Messung vorher.** Kopf Z. 37-39 (Stand 6de90398): "Typ 0x13, Stage 4 ROOM4050: Modus-Tabelle @0x80120230[0] =
0x8010b6d4 laedt DECIDE aus @0x80120264[+0x5] (`addiu at,at,612` @0x8010b728, `jalr` @0x8010b738), Eintrag [0] =
0x80101b64". STAGE4.BIN ist 108064 = 0x1A620 Byte lang (endet @0x8011A620), @0x80120230/@0x80120264 liegen ausserhalb.

**Beleg (selbst disassembliert, `re15_disasm.py` gegen PSX.EXE/STAGE4.BIN, Protokoll `nb1_stage4_kette.txt`):**
1. *ROOM4050 ist Stage-Byte 3.* Raumlader @0x800397a4-ec: `lh v0,4064` (0x800b0fe0 Stage) / `lh v1,4066` (0x800b0fe2
   Raum), `addiu at,at,17292` (= 0x8007438c) + Stage*4, `lhu v0,0(v1)` (+ Raum*2), `srl a0,a0,31` (Figur-Bit aus
   0x800aca3c), `jal CD_read` @0x800397e8. Stage 3 / Raum 5: @0x8007438c[3] = 0x8007432c -> Datei 912/913, Groesse laut
   Dateitabelle @0x8006f43c je 221128 = ROOM4050.RDT/ROOM4051.RDT. Gegenprobe Stage 0 / Raum 5: 297324 / 243976 =
   ROOM1050.RDT / ROOM1051.RDT.
2. *Stage-Byte 3 laedt STAGE4.BIN.* Overlay-Wechsel @0x80039a30 (Aufruf @0x8001d980, nur bei Stage-Wechsel, Vergleich
   @0x8001d968): `lh a1,4064`, `ori a0,zero,0x2`, `jal 0x800299a4`, Delay-Slot `addiu a1,a1,1` @0x80039a48. 0x800299a4:
   Satz = 0x80073bdc + 12*a1 (@0x800299b4-bc), Datei `lw a0` @0x800299e0, Ziel `lw a1,0(v0)` @0x800299e4, `jal CD_read`
   @0x800299e8, Installationsroutine an 0x80029a98 (@0x80029a04). Satz 4 @0x80073c0c = {Datei 3, 0x80118380,
   0x80011370}, *(0x80011370) = 0x80100000; Dateitabelle @0x8006f454 (Eintrag 3) = 108064 = STAGE4.BIN. (Satz 1 =
   Datei 0 = 137648 = STAGE1.BIN mit Installation 0x8011e864.)
3. *Typ 0x13 in STAGE4.BIN.* Installation 0x80118380: `lui v1,0x8011 / addiu v1,v1,-22404` @0x8011838c-90 =
   0x8010a87c, `sw v1,0x2bf8(at)` @0x80118478 -> 0x80072bac[0x13] = 0x8010a87c; Typ 0x10/0x11/0x12/0x16/0x18/0x1c..0x1f
   -> 0x801003d8 (`addiu v0,v0,984` @0x80118388, Stores @0x80118398-d8).
4. *Girl-Kette in STAGE4.BIN.* 0x8010a87c: `lbu v0,4(v0)` @0x8010a9c0, `addiu at,at,-26832` @0x8010a9d0 (= 0x80119730),
   `jalr v0` @0x8010a9e0; @0x80119730[1] = 0x8010b228 (eine Funktion: Prolog @0x8010b230, einziges `jr ra` @0x8010b680):
   `lbu v0,9(v0)` @0x8010b648, `andi v0,v0,0xf` @0x8010b650, `addiu at,at,-26792` @0x8010b65c (= 0x80119758), `jalr v0`
   @0x8010b66c; @0x80119758[0] = 0x8010b688: `lbu v0,5(v0)` @0x8010b6cc, `addiu at,at,-26740` @0x8010b6dc
   (= 0x8011978c), `jalr v0` @0x8010b6ec; **@0x8011978c[0] = 0x80101b18**.
5. *Dieselbe Funktion.* 0x80101b18 (STAGE4.BIN) gegen FUN_80101b64 (STAGE1.BIN): 70 von 70 Worten bytegleich, Ende
   `jr ra` @0x80101c28 bzw. @0x80101c74, naechste Funktion 0x80101c30 bzw. 0x80101c7c.
6. *Zombies in STAGE4.BIN.* 0x801003d8 -> Modus `lbu v0,9(v0)` @0x80101600, `andi 0xf` @0x80101608, `addiu
   at,at,-29388` @0x80101614 (= 0x80118d34); @0x80118d34[0] = 0x80101640: `lbu v0,5(v0)` @0x80101650, `addiu
   at,at,-29336` @0x80101660 (= 0x80118d68); @0x80118d68[0] = 0x80101b18. (In ROOM4050/4051 tragen nur die zwei Girls
   grid 0x00, main00 @0x01EB4/@0x01F5C; die zwei Typ-0x18-Saetze dort sind 0x81 und 0x0d.)
7. *Herkunft des alten Zitats.* Girl-Modus 0 STAGE1.BIN 0x8010b6d4 gegen STAGE4.BIN 0x8010b688: 42 von 44 Worten
   gleich; die zwei Abweichungen sind genau die Tabellenbasen (`addiu at,at,612` -> 0x80120264 gegen `addiu
   at,at,-26740` -> 0x8011978c, Animate 0x801202a8 gegen 0x801197d0).
8. *Zusatz der Abnahme zu FUN_80101b64, ganze Funktion selbst gelesen* (@0x80101b64-78): Zaehler +0x9C (`lhu a1,156(a0)`
   @0x80101ba0, Dekrement @0x80101ba8-ac, `bne a1,zero` @0x80101bb0, 0x101 @0x80101bc0-c4); 0x701 bei `sltiu 0x7d0`
   @0x80101bdc und arc_test(0x2c8) != 0 (@0x80101be4-f0); ENGAGE 0x201 @0x80101c1c-20, danach `sltiu 0x2711` @0x80101c38
   -> `jal rng` @0x80101c44 / `andi 0x7` @0x80101c4c / 0x801 @0x80101c54-64. Die Kernaussage (ENGAGE auf Sicht ohne
   Abstandsgrenze) bleibt; bei dist >= 10001 wird ENGAGE je Aufruf mit 1/8 durch 0x801 ersetzt.

**Aenderung.** `re15_zombieaktiv_r37.h` Kopf: Girl-Absatz auf STAGE4.BIN (Punkte 1-6 in Kurzform, die STAGE1-Adressen
nur noch als ausgewiesene STAGE1.BIN-Kopie), die Adressen der Klassen 0x00/0x02 tragen "STAGE1.BIN", der Abstands-Satz
nach Punkt 8 praezisiert. Dossier R2 ebenso. Kein Code.

**Messung nachher.** Jede Adresse des neuen Kopf-Absatzes steht im Protokoll `nb1_stage4_kette.txt` (Werkzeugausgabe
gegen PSX.EXE/STAGE4.BIN). Die Abschlussmeldung dieser Nachbesserung zitiert STAGE4.BIN.

**Hinweis (nicht geaendert, Vorrunde):** der Girl-Block in `enemy_ai_common.c` (um Z. 10650-10690) beschreibt dieselbe
KI an den Adressen der STAGE1.BIN-Kopie (Wurzel FUN_8010a8c8, Tabellen @0x80120208/30/64/a8), ohne die Binaerdatei zu
nennen; fuer ROOM4050 gelten 0x8010a87c / @0x80119730 / @0x80119758 / @0x8011978c / @0x801197d0. Inhaltlich gleich
(Code bis auf die Tabellenbasen bytegleich, Punkt 7), nur Kommentar, kein Pfad dieser Spur.

### M2 — zwei Saetze in "Fuer den Nutzer" widersprechen der Messung

**Ursache.** (a) "Genau diese Aufteilung gilt jetzt auch fuer RE1.5" war aus dem Mechanismus abgeleitet (Klasse "aktiv"
-> Klasse "aktiv"), nicht nachgezaehlt. (b) "RE1.5-KI unveraendert" galt fuer die Abbildung (Haken nur in re2z_init),
uebersah aber, dass der Nachspawn 1130 (irons_tod_1150.c) eine Spawnliste ist und unter jeder KI wirkt.

**Messung / Beleg (a), Zensus beider Spiele, beide Skripte selbst gefahren:**
`python analysis/befunde_runde37/H_belege/re15_emset_zensus.py sum` -> 419 Zombie-Saetze (Typ 0x10/11/12/13/16/18, alle
240 RDTs): grid 0x00 72, 0x02 91, 0x0d 142, Rest liegend/fressend/kriechend/sitzend (0x06 8, 0x0e 2, 0x81 12, 0x83 2,
0x84 28, 0x85 2, 0x86 4, 0x87 20, 0x88 33, 0xa1 2, 0xae 1).
`python analysis/befunde_runde37/H_belege/re2_emset_zensus.py sum` -> 433 Saetze in 55 RE2-Raeumen: Deskriptor 0 214,
6 140, 1 13, 2 18, 3 6, 4 18, 5 3, 7 1, 8 13, 10 7.
"Aufrecht" = was der RE2-INIT auf Stehen oder Gang legt: RE2 Deskriptor 0 + 6 = 354; RE1.5 grid 0x00 + 0x02 + 0x0d = 305
(re2z_init: 0x0d -> 0x101, 0x00 -> Gang mit Marke, sonst 0x0001; alle anderen Zweige liegen/fressen/kriechen).

| | ab Spawn im Gang | Anteil aller Zombie-Saetze | Anteil der aufrechten |
|---|---|---|---|
| RE2 Retail | 140 (Deskriptor 6) | 140/433 = 32 % | 140/354 = 40 % |
| Port v0.8.24 | 142 (grid 0x0d) | 142/419 = 34 % | 142/305 = 47 % |
| Port jetzt | 214 (grid 0x0d + 0x00) | 214/419 = 51 % | 214/305 = 70 % |

Die Aufteilung folgt den RE1.5-Klassen, nicht der RE2-Quote; der Port ist jetzt aggressiver als RE2.

**Messung / Beleg (b), echte exe, RE1.5-KI (`RE15_AI_FLAVOR=re15`), `nb1_re15ki_messung.txt`:**
* Nachspawn 1130 (Weg wie integration_r37_zombieaktiv: Karte 1150 + CONTINUE, Tuer -> 1130, Leon steht), 400 Bilder,
  Abstand aus den Lagen: Spur-exe (Bau 12:22 = 6de90398) alle fuenf g=0d `0/0@1 1/2@6` = RE1.5-Angriff 0x201 (INIT
  @0x80100f40-44), Abstand F1 -> F400 30880->23389, 29674->27163, 28617->20007, 27537->21469, 28430->22590 = 2511..8610
  naeher. v0.8.24-exe (Hauptbaum, Bau 09:02; Engine-Quelle 7f0fa8bb gleich Basis 44881ac2, `git diff --stat` leer):
  alle fuenf g=02 `0/0@1 1/0@6` (Stehen), Abstand unveraendert, Weg 0..18. Wie Abnahme N5.
* Abbildung unter der RE1.5-KI: ROOM1180 (3x grid 0x00, Debug-Sprung `1180@250`, 300 Bilder): Spur-exe und v0.8.24-exe
  schreiben ein Zeile fuer Zeile gleiches state.log (Besuch 0 251/251, Besuch 1 301/301 Zeilen); der nahe Zombie greift
  in beiden ab Bild 10 an (1/2), die zwei fernen stehen.

**Aenderung.** "Fuer den Nutzer": (a) durch die gezaehlte Aufteilung ersetzt (folgt den RE1.5-Klassen, Port aggressiver
als RE2, Zahlen der Tabelle); (b) durch die gemessene Aussage (Spawn-Regel wirkt unter der RE1.5-KI nicht, der Nachspawn
1130 schon). Derselbe Satz stand verkuerzt im Kopf von `re15_zombieaktiv_r37.h` ("RE1.5-KI und MIXED unberuehrt"): dort
ebenso korrigiert (MIXED: Zombies laufen dort auf dem RE1.5-Gehirn, `re15_ai_re2_for_type` gibt unter MIXED nur Typ 0x20
frei); Kommentar der Liste `EM_1130_NACH` (irons_tod_1150.c) "gilt fuer jede KI-Wahl". Verhalten unveraendert (Abnahme:
"am Verhalten ist nichts zu aendern").

**Messung nachher** (neu gebaute exe, Bau 14:19 nach 83eeff99, gleicher Weg): RE1.5-KI state.log Zeile fuer Zeile gleich
der Spur-exe (91/91 + 401/401 Zeilen); RE2-KI (Standard) alle fuenf g=0d `1/1@6` (Gang ab Bild 6), Abstand F1 -> F400
30880->24420, 29674->21031, 28617->22162, 27537->18040, 28430->16417 (6455..12013 naeher), wie Abnahme N1.

### Hinweise der Abnahme
* H6 (Stil): `EM_1130_NACH` mit Zeilenfortsetzungen wie `EM_1130_LISTE`; Inhalt gleich (das gleiche state.log oben).
* H4: dem Nutzer gesagt (M2a, "Fuer den Nutzer"). H1/H2/H3/H5: keine Maengel, unveraendert (H1 = die zwei vorbestehenden
  RE2-Abweichungen im Steh-Zweig, OFFEN oben; H3 = die Montage-Einstellung 1130 zeigt die fuenf absichtlich stehend).

### Tests Nachbesserung 1 (gezielt, KEINE volle Suite)
Geaendert sind nur Kommentare (Kopf `re15_zombieaktiv_r37.h`, Kommentar in irons_tod_1150.c) und die Schreibweise von
`EM_1130_NACH`; neu uebersetzt wurden zombieaktiv_r37.c, irons_tod_1150.c, scd_vm.c, enemy_ai_re2_zombie.c und zwei
Test-Quellen. Gezielt die Gruppen aller Module der Spur (RE2-Zombie, Montage/Irons 1150, Gegnerzahl, Zombie-KI Runde 36,
eigene Riegel):

Bau 14:19 (nach 83eeff99):
```
ctest --test-dir re15_port/build -R "r37_zombieaktiv|r36_zombieki|r35_cut1150|r36_szene1150|r36_gegnerzahl|re2_zombie|re2z|irons|montage" -E "^integration_" --timeout 600 --output-on-failure -j 4
100% tests passed, 0 tests failed out of 62
ctest --test-dir re15_port/build -R "^integration_(r37_zombieaktiv|r35_cut1150|r36_szene1150|r36_gegnerzahl)$" --timeout 1500 --output-on-failure -j 2
100% tests passed, 0 tests failed out of 4
```
(Integration 515,5 s; darin die Montage-Kette integration_r35_cut1150, integration_r36_szene1150, Spur-E-Riegel
integration_r36_gegnerzahl und integration_r37_zombieaktiv; im Unit-Lauf die Runde-36-F-Riegel unit_r36_zombieki_*.)

Endstand-Bau 14:31 (nach d3d92ecd, gegenueber 14:19 nur Kopf-Kommentare):
```
ctest --test-dir re15_port/build -R "r37_zombieaktiv" --timeout 600 --output-on-failure -j 2
100% tests passed, 0 tests failed out of 4
```
dazu der RE1.5-KI-Messweg 1130 mit der Endstand-exe: state.log Zeile fuer Zeile gleich der Spur-exe 12:22 (91/91 +
401/401, `nb1_re15ki_messung.txt` Abschnitt D).
