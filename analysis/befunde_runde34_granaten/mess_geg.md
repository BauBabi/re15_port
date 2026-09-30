# Runde 34 (Granaten) — MESSUNG "alle weiteren Gegnertypen"

Stand: 2026-09-30, Integrationsbaum `.claude/worktrees/r34g_int`, Zweig `r34g/integration` HEAD ef1c6f94.
exe: `re15_port/build_r34_int/platform/pc/re15_pc.exe`, gemessen ueber die byte-gleiche Kopie `re15_pc_geg.exe`
(gleiches Verzeichnis, Schutz vor fremden `taskkill /IM re15_pc.exe`). Nur messen — kein Code, kein Commit.
Soll: BAUPLAN.md §1.5/§1.6 (+ integration.md I-5: HE an der RE2-Zombie-Familie = DEATH[9][3]).
Ausgaben: `build/r34g_mess_geg/<lauf>/` (unversioniert), Belege: `mess_geg_belege/`.

Dieses Dossier wird fortlaufend geschrieben (Sitzungslimit-Schutz).

## 0. Fortschritt

| Typ | Raum | Weg | echter Wurf getroffen | Schaden / Reaktion / Leiche | Haenger | Urteil |
|---|---|---|---|---|---|---|
| Hund 0x20 RE2 | 3060 (+11D0) | Wurf TIEF, 3 Granaten | ja (310/387 neben) | 300 -> Tod, Zeile 9/11/10 (zerplatzt/geaetzt/verkohlt), Leiche X+22 | nein | ok (Hinweis M-H1) |
| Hund 0x20 RE1.5 | 3060 (+11D0) | Wurf TIEF, 3 Granaten | ja | 1000 -> Tod, Clip 7 (0x80110eb0), Leiche X+16 | nein | ok |
| Hund ROOM1190 (I-2) | 1190 | Haken | — | ruhende Hunde auf y -3600/-10000/-20000 -> Band | — | I-2 geklaert |
| Kraehe 0x21 RE2 | 10C0 | Wurf TIEF, 3 Granaten | ja (391) | 60 -> Tod, GIB, Leiche X+1 | nein | ok (Hinweis Teile-Wurf OFFEN) |
| Kraehe 0x21 RE1.5 | 10C0 | Haken (Wurf physikalisch unmoeglich) | nein (fliegt < 5000 auf) | 1000, GIB-Spur, Leiche X+52 | nein | ok |
| Spinne 0x25 RE2 | 2070 | Wurf TIEF, 3 Granaten + Doppelwurf | ja (310) | 60/90 HURT, 130 Tod (Brand); 2. HE -> Tod + 6 Babys | nein | ok |
| Spinne 0x25 RE1.5 | 2070 | Wurf TIEF, 3 Granaten | ja (347) | 1000 -> Tod, Leiche X+9 (Clip 5) | nein | ok |
| Baby 0x26 (RE2) | 2070 | Haken (Wurf trifft nur in 501) | nein (968 entfernt) | 60/90/130 -> Tod, entfernt X+2 | nein | ok |
| Feuer 0x26 | 1090 | Wurf + Haken | nein (Granate faellt auf y 0, Emitter y -2680) | Haken: 1000, Flammen-Minderung, zurueck in st 1 | nein | ok (Hinweis Boden y 0) |
| Made 0x27 | 11C0 (Flag 4:64) | Wurf TIEF, 3 Granaten | ja (505/560) | 1000 -> Tod, Clip 10, Rueckstoss, Leiche X+41 | nein | ok |
| Kakerlake 0x29 | 5000 | Wurf TIEF, 3 Granaten | ja (533/656) | 1000 -> Tod, Clip 10/11 (vorn/hinten), Leiche X+41 | nein | ok |
| Tyrant 0x2b | 5110 | Wurf TIEF, 3 Granaten | ja (313) | 1000 -> Tod, Clip 9 -> Ph.2 Clip 11 (Bit 0x80), Leiche X+65 | nein | ok |
| Ivy 0x2d | 4030 | Wurf + Haken | nein (Gate B) | kein Schaden, keine Reaktion | nein | ok (immun) |
| Birkin 0x30 | 3070 (Flag 4:215) | Wurf TIEF, 3 Granaten, 3 Stellungen | ja (746) | 1000, Clip 9, Flag 5:0x1c, Phase 3 halten | nein | **Mangel M-B1** (steht wieder auf) |
| Birkin 0x36 | 3080 | Haken (Raum = Zwischensequenz) | nicht messbar | 1000, eingefroren bis die Sequenz ihn neu setzt | nein | nicht_messbar (Wurf) |
| G5 (0x36 RE2-Modul) | 5090 | Wurf TIEF im Kampf, 3 Granaten + Doppelwurf | ja (1383) | 80/70/70, Akku 14, 2. Treffer STAGGER Clip 8 | nein | ok |
| Alligator 0x23 (nicht 2090) | — | — | nicht messbar (kein Spawn) | — | — | nicht_messbar |
| Gator-Boss | 2090 | Wurf MITTE (kalibriert), 3 Granaten | ja (1006) | 1000, Modul-Absorb, Flinch Clip 10, Aggro | nein | ok |
| Zombie-Maedchen 0x13 | 4050 | — | nicht messbar (Port-Luecke Selbst-Tuer) | — | — | nicht_messbar (M-H3) |
| Gitterarm 0x1a RE1.5 | 1210 | Wurf TIEF, 3 Granaten | ja (391) | 1000 -> Tod, Clip 3, Leiche X+55 | nein | ok |
| Zellenarm 0x1a RE2 | 1210 | Wurf + Haken | nein (Arm auf y -2500) | Haken: Rueckzug, HP 250, unsterblich | nein | ok (Hinweis H-Z1) |
| NPC 0x45 | 1150 | Wurf TIEF | nein (kein Kandidat) | keine | nein | ok |
| Fresser 0x16 liegend | 1140 | Haken | — | beide KI: kein Schaden | nein | Port = beide Originale; BAUPLAN-Text veraltet (M-H4) |
| Brad 0x11 | 1140 | Haken | — | 200, HURT, lebt; Saeure DoT | nein | ok |

## 1. Werkzeug und Vorgehen

* `mess_geg_werkzeug/lauf.sh <marke>` — ein exe-Lauf (Env wie `test_r34_granaten.cmake`: Titel-Autostart,
  `RE15_DEBUG_JUMP=<raum>@250`, Skript auf der Spielbild-Achse ab Bild 1, Logs `state.log` / `gr.log` / `wf.log`,
  `RE15_EXIT_AT`), Kopie `re15_pc_geg.exe`. `zensus.sh` = Sprung + 60 Bilder je Raum; `zensus_xyz.py` = alle
  `Sce_em_set`-Saetze (Opcode 0x44, Walker `scd_walk_lib.py`) mit Lage (x,y,z) aus pc[8..13].
* Wurf wie W9: TIEF (`MD0.6,MDA0.2,MD2.5,W5` ab Bild 1 -> Abzug A = 19, Spawn S = A + 24 = 43, Liegen L,
  Explosion X = L + 36). Gemessene Wurfgeometrie (ROOM1140, integration W9 r2_10): Landung = Leon +
  2513 * (cos r, -sin r) - 313 * (sin r, cos r), r = Gier im Spawnbild (die Auto-Nachfuehrung dreht Leon beim
  Heben auf den naechsten Gegner). Boden der Granate = Welt-y 0 (BAUPLAN 1.1, Routine 29 `blez` @0x80018330-38).
* Treffer-Regel (BAUPLAN 1.5): R = r + 500 waagrecht, Band |P.y - (Y + O.y)| < 500 + h, P.y = Liegestelle-y - 500.
* Weitere Werkzeuge (`mess_geg_werkzeug/`): `plan.py` (Leon-Stellung fuer einen TIEF-Wurf auf ein Ziel), `treffer.py`
  (Auswertung X-1/X/X+1, Folge, Leiche, EXIT_AT), `spur.py` (Gegnerspur), `fxspawn.py` (ESP-Spawns/SEs je Bild aus wf.log),
  `kamsicht.py` (welcher Cut zeigt einen Weltpunkt), `tuer_nach.py` (Tueren mit Ziel Stage/Raum), `cut_regionen.c` (Region-Quad
  je Cut; Bau: `gcc -O1 -I re15_port/include -I re15_port/engine/include cut_regionen.c build_r34_int/engine/libre15_engine.a
  build_r34_int/tests/libre15_test_support.a build_r34_int/engine/libre15_engine.a -lm`), `sheet.py`/`crop.py` (Bildboegen).
* Wo ein echter Wurf den Typ nicht verlaesslich erreicht: Mess-Haken `RE15_FORCE_EXPLOSION=<art>@<bild>:<slot>`
  (Integration W6: P = Gegnerlage mit y - 500, FUN_80012d60(500, P, Art), Art 3/4 + RE2-Aufschlag) — so gekennzeichnet.

## 2. Raum-Zensus (Sprung + 60 Bilder, RE2-KI = Default)

Spawn-Saetze (RDT, `zensus_xyz.py`) und was nach dem Sprung wirklich steht (Zustandslog Bild 60):

| Typ | Raum | nach dem Sprung (Slot: Lage, Zustand, HP) | RDT-Lage y |
|---|---|---|---|
| 0x20 Hund | 1190 | 1..3 bei (690,-28500), st 4 grid 0x40 (Skript-Pounce), HP 129/80/81 | **-3600 / -10000 / -20000** (sub13 @0x2900/14/28) |
| 0x20 Hund | 11D0 | 5 Hunde st 4 ss1 1 grid 0x41, HP 83/98/69/96/95 | 0 |
| 0x20 Hund | 1230 | 2 Hunde st 1 (aktiv), weit weg (d 28888/34016) | 0 |
| 0x20 Hund | 3060 | 3 Hunde st 4 grid 0x41 | 0 |
| 0x21 Kraehe | 10C0 / 1120 | je 3 Kraehen st 1, HP 10 | 0 |
| 0x21 Kraehe | 1170 | keine nach dem Sprung (sub15, Ereignis) | -7200 |
| 0x25 Spinne | 2030/2050/2060/2070/20A0/2090 | 2..3 Spinnen st 1, HP 109..111 (2060/20A0 grid 0x41 Decke) | 2030/2050 -5400, 2060/20A0 grid 0x41: -3600/-5400, 2070/2090 0 |
| 0x26 Feuer | 1090 | 7 Emitter st 1, HP 100 | **-1800** |
| 0x27 Made | 11C0 | 2 Maden st 0, HP 0, grid 0x30 (geparkt) | **-20000** (grid 0x30); Variante grid 0x10 y 0 |
| 0x29 Kakerlake | 3040 / 4040 / 4080 / 5000 | 3040: 2x st 1 mo 22 (y -1800); 5000: 1x st 1 mo 22; 4040/4080: grid 0x61 | 3040 -1800, sonst 0 |
| 0x2b Tyrant | 5040/5070 (grid 0x41), 5110/5130 (grid 0) | je 2, HP 121/91 | 0 |
| 0x2d Ivy | 4030 | 1 Ivy st 1, HP 100 | -450 |
| 0x30 Birkin | 3070 / 5080 | 3070: geparkt (30000,30000) grid 0x33; 5080: st 1 ss1 9 grid 0x33, HP 300 | 5080 -5600 |
| 0x36 Birkin | 3080 | 1 x st 1 grid 0x24 bei (-20000,-26000), HP 300 (+ NPC 0x40/0x42) | 0 |
| G5 (0x30 -> 0x36) | 5090 | st 0 grid 0x33 bei (-14700,-23350), HP 600 (+ NPC 0x4d) | 0 |
| 0x13 Zombie-Maedchen | 4050 | **keines** nach dem Sprung (bedingter Spawn) | 0 |
| 0x1a Gitterarm | 1210 | 10 Arme st 1, HP 250 | 0 |
| 0x23 Alligator | — | **kein einziger Sce_em_set-Satz in 240 RDTs**; der Port setzt 0x23 nur in 2090/2091 (Port-Ergaenzung, main.c ~7425) | — |
| Gator-Boss | 2090 | Slot 15, HP 3000 (+ 2 Spinnen) | — |
| NPC 0x45 | 1150 | 1 x st 4, HP -1 | -720 |
| NPC 0x40/0x42/0x4b | 11B0 / 3080 / 4000 | geparkt (11B0) bzw. neben Leon (3080/4000), HP -1 | 0 |

## 3. Ergebnisse je Typ

Spalten: X = Explosionsbild (L + 36, gr.log), "Zeile X" = Zustandslog-Zeile des Explosionsbilds (hinter dem ESP-Takt,
integration.md W9 Harness-Befund 3). Alle Laeufe: exe bis `RE15_EXIT_AT` (Zeile `[flow] EXIT_AT` im debug.log, rc 0) =
**kein Haenger**. Leon-HP im Explosionsbild unveraendert (Abstand > 950), sofern nicht anders vermerkt.

### 3.1 Hund 0x20 — echter Wurf, beide KI, alle drei Granaten: OK

**I-2 aufgeklaert (Hoehe, nicht Kasten):** die ruhenden Skript-Hunde in ROOM1190 liegen NICHT am Boden. RDT sub13
@0x2900/0x2914/0x2928 setzt sie auf y = -3600 / -10000 / -20000 (`zensus_xyz.py`), gemessen mit dem Mess-Haken
(`h1190_fe`, debug.log `RE15_FORCE_EXPLOSION F40 ... P=(690,-4100,-28500)`, `P=(690,-10500,...)`, `P=(690,-20500,...)`
= y - 500). Die liegende Granate hat Welt-y ~3 (Boden y 0), P.y = -497; Hoehenband (BAUPLAN 1.5, @0x8002b6fc-7ac)
|P.y - (Y + O.y)| = |-497 - (-3600 - 600)| = 3703 >= 500 + 600 -> kein Treffer. Der Integrations-Lauf `dog3`
(`eingriffe=0` bei 577 waagrecht) ist damit Resolver-korrekt, kein Mangel.
(Nebenbefund in `dog3`: Anker (1037,-1693,-25992) nur 23 vor Leon, y = Bindpose-Hoehe — Leon stand ausserhalb des Cut-0-
Bereichs (`cut_regionen.exe`: (1190,-26000) "gecullt" in Cut 0), der Renderer posierte Knochen 11 nicht fuer den Wurf.
Harness-Artefakt des Teleports ohne Kamerawechsel; alle Laeufe unten pruefen den Anker: TIEF = ~970 vor Leon, y -772.)

Aufstellung: ROOM3060 (Hof, Hund Slot 1 ruht st 4 / grid 0x41 / Clip 8 bei (-14000,-24000), y 0), Leon
`RE15_PLAYER_POS=-16452,-23452,143` (2513 vor dem Hund, `plan.py`), `RE15_FORCE_CUT=1` (Cut 1 zeigt Leon UND Hund,
`kamsicht.py`; Cut 0 schaut weg). Anker (-15584,-772,-24005) = 967 vor Leon, y -772 (gueltig). Landung/P
(-14070,-480,-24302), 310 neben dem Hund. Gegenprobe ROOM11D0 (Zwinger-Hund Slot 5, Leon (-1480,-17762,1854), Cut 0):
dieselben Zahlen (Treffer 387 neben dem Hund), Laeufe `hund11d0_g{9,10,11}_{re2,re15}`.

| Lauf | Art | KI | HP X-1 -> X | +0x5 (Zeile X) | X+1 | Folge | Leiche | Soll (BAUPLAN 1.6) |
|---|---|---|---|---|---|---|---|---|
| hund3060c1_g9_re2 | 2 | RE2 | 129 -> -171 (300) | 9, +0x6 0 | st 3/9/2, Clip 17 | Clip 18 ab F132 | F141 | 300 -> Tod, Zeile 9 zerplatzt: ok (Bild: Koerper fliegt, Teile + Blut F121-F185) |
| hund3060c1_g10_re2 | 3 | RE2 | 129 -> -171 (300) | 10 (Waffen-Id; Hund-Gehirn -> RE2-Zeile 11, bau_b Sonde 140) | Clip 17 | Clip 18 F132 | F141 | Zeile 11 geaetzt: Koerper dunkel-oliv F125/F129 — ok |
| hund3060c1_g11_re2 | 4 | RE2 | 129 -> -171 (300) | 11 (-> RE2-Zeile 10) | Clip 17 | Clip 18 F132 | F141 | Zeile 10 verkohlt: Koerper schwarz, Bodenflammen bis F185 — ok |
| hund3060c1_g9_re15 | 2 | RE1.5 | 73 -> -927 (1000) | 9, +0x6 1 | Clip 7 | Rueckstoss 2000 Einheiten | F135 | 1000, 1D-DEATH [9] 0x80110eb0: ok |
| hund3060c1_g10_re15 | 3 | RE1.5 | 73 -> -927 | 10 | Clip 7 | — | F135 | [10] = 0x80110eb0 (Tabelle @0x80121070 [7..11]) ok |
| hund3060c1_g11_re15 | 4 | RE1.5 | 73 -> -927 | 11 | Clip 7 | — | F135 | [11] = 0x80110eb0 ok |

Beleg RE1.5-Clip (selbst disassembliert, STAGE1): `table 0x80121070` [7..11] = 0x80110eb0; dort `ori v0,zero,0x7` /
`sb v0,148(v1)` @0x80110f24-28 (+0x94 = Clip 7), SE 4 `jal 0x800453d0` @0x80110f78, Blut `jal 0x80019700` a0 = 0x2000
@0x80110f80-9c, Rueckstoss `lbu v0,147` / `andi 0x80` / `subu` / `sll 11` / `jal 0x800245d8` @0x80111024-38 (Richtung
Gier + 0x800, "von hinten" dreht) — Port `enemy_ai_common.c` ~8341-8370 deckungsgleich; gemessen gleitet der Hund
(-14000,-24000) -> (-15455,-22599) = 180 Grad zu seiner Gier 499.
Effekt-Zaehlung RE2 (wf.log `SPAWN`, `fxspawn.py`, 11D0): HE F120 3x id0/1 + id7/1, F121-125 je 2x id0/1 + id7/0
(Summe 13 + 6 = bau_b Sonde 123/124 "FX0 13, FX1/2 6"); Brand F120 6x id8/3 (= "FX7 6", Sonde 130-133); Saeure nur 1x
id0/1 — FX 9/10 der Saeure-Zeile fehlen (s. Mangel M-H1).
Bilder: `mess_geg_belege/hund_3060_re2_he.png`, `hund_3060_re15_he.png`, `hund_3060_re2_saeure.png`, `hund_3060_re2_brand.png`
(Ausschnitte F117/F121/F129/F141 bzw. F133/F157).

### 3.2 Kraehe 0x21 — RE2: echter Wurf OK (alle drei); RE1.5: echter Wurf physikalisch nicht moeglich, Reaktion per Mess-Haken OK

Aufstellung ROOM10C0 (Kraehe Slot 3 sitzt bei (5100,3600), y 0), Leon `3522,5556,581` (die Kollision schiebt ihn im
Bild 6 auf (3918,5556), 2285 vor der Kraehe), `RE15_FORCE_CUT=1`. Landung P (4942,-480,3242), 391 neben der Kraehe.

| Lauf | Art | KI | HP X-1 -> X | +0x5 | X+1 | Soll |
|---|---|---|---|---|---|---|
| kraehe10c0_g9_re2 | 2 | RE2 | 10 -> -50 (60) | 9 | st 7 (Leiche) sofort, Modell weg | 60 -> Tod, GIB 0x80102CA0: ok |
| kraehe10c0_g10_re2 | 3 | RE2 | 10 -> -50 | 10 | st 7 | GIB (alle drei Zeilen): ok |
| kraehe10c0_g11_re2 | 4 | RE2 | 10 -> -50 | 11 | st 7 | GIB: ok |
| kraehe10c0_g9_re15 / 10c0b_g9_re15 | 2 | RE1.5 | kein Treffer (eingriffe 0) | — | — | s. unten |
| kraehe10c0_fe{2,3,4}_re15 (Haken) | 2/3/4 | RE1.5 | 0 -> -1000 | 9/10/11 (Port setzt in X+1 intern 7) | GIB-Spur, Leiche F82 = X + 52 | Tabelle @0x801211cc [9..11] = 0x801149c4 (GIB): ok |

* **RE1.5-Kraehe und echter Wurf:** die Kraehe fliegt auf, sobald Leon naeher als 5000 steht — DIVE-DECIDE
  `ring = (grid < 0x80) ? 5000 : 10000`, `crow_dist < ring && vert_err < 5400 -> Unterzustand 4` (@0x801126b4-f8,
  Port `enemy_ai_common.c` ~6047-6049). Ein TIEF-Wurf landet ~2500 vor Leon -> die Kraehe ist im Bild X laengst in der
  Luft (gemessen: Bild 10 Unterzustand 4, Bild 70 Flug, 8318/9789 von P entfernt; auch von hinten, `kraehe10c0b`). Im Flug
  liegt sie ausserhalb des Hoehenbands (|P.y - Y| < 500 + 180). Das ist Original-Verhalten, kein Mangel. Reaktion daher
  ueber `RE15_FORCE_EXPLOSION` (Leon am Sprungpunkt, 7644 entfernt): GIB-Spur mit Zeitgeber 0x32 (@0x80114ab4), Leiche
  nach 52 Bildern, Modell verborgen, roter Feder-Stand-in (`kraehe_10c0_re15_force.png`).
* RE1.5-Kraehen-HP: der ACTIVE-Tail schreibt jedes Bild `hp = (vert_err >= 5200) ? -1 : 0` (@0x80115f88-9c) -> HP 0,
  1000 Schaden -> -1000 (Zustandslog). Ein Kraehen-Treffer kostet Leon nichts (Abstand).
* RE2-GIB: Port verbirgt das Modell (`crow_hide`) statt der 13 Teile-Wuerfe (0x80102EF4/0x80102F34) — im Code als OFFEN
  benannt (`enemy_ai_re2_crow.c:56-57`, Part-Scatter nicht modelliert). Bild `kraehe_10c0_re2_he.png`: Kraehe F117 am
  Boden, ab F141 verschwunden.

### 3.3 Spinne 0x25 und Baby 0x26 (RE2-Herkunft) — echter Wurf OK, beide KI

Aufstellung ROOM2070 (Boden y 0; Spinne Slot 1 bei (-12000,-6400)), Leon `-9487,-6400,2048`, `RE15_FORCE_CUT=2`
(Cut 2 zeigt beide). P (-11999,-480,-6090), 310 (RE2) bzw. 347 (RE1.5) neben der Spinne. RE1.5-Spinne steht still
(st 1/0/0/1), RE2-Spinne wandert, ist im Bild X aber noch an der Stelle.

| Lauf | Art | KI | HP X-1 -> X | Zustand X / X+1 | Ergebnis | Soll |
|---|---|---|---|---|---|---|
| spinne2070_g9_re2 | 2 | RE2 | 111 -> 51 (60) | 2 (HURT) +0x5 9 / st 2 Clip 1 | ab F128 wieder aktiv, Bild: hochgeworfen + Blut | "HE meist HURT, Zeile 9 Knock + Blut": ok |
| spinne2070_g10_re2 | 3 | RE2 | 111 -> 21 (90) | 2, +0x5 10 / Clip 1 | ab F128 aktiv; Saeure-Blitz F121-129 | "Saeure meist HURT (Zeile 11)": ok; "Bein ab" im Bild nicht beurteilbar |
| spinne2070_g11_re2(_lang) | 4 | RE2 | 111 -> -19 (130) | 3, +0x5 11 / Clip 12 | brennt 180 Bilder (Clip 12 bis af 181), Leiche F299 | "Brand meist Tod (Parts 0x00202F2F)": ok, Koerper dunkel |
| spinne2070_g9x2_re2 (zwei HE) | 2 | RE2 | 2. Explosion X2 = 170: 51 -> -9 | 3, +0x5 9 / Clip 12 | **6 Babys 0x26 (HP 1) ab F175**, Leiche F182 | "Tod Zeile 9 = Zerplatzen + 6..9 Babys": ok |
| spinne2070_g{9,10,11}_re15 | 2/3/4 | RE1.5 | 129 -> -871 (1000) | 3, +0x5 9/10/11, +0x6 1 / Clip 1 | Leiche F128 (Clip 5) | RE1.5 byte-true (DEATH 0x80113f40, CORPSE Clip 5): ok |

* **Baby 0x26 (RE2-Herkunft):** ein dritter echter Wurf (`spinne2070_g9x3_re2`, X3 = 227) trifft KEIN Baby: das
  naechste stand 968 von P. Die RE2-Babys entstehen ueber `re15_actor_alloc` (re2s_spawn_babies, `enemy_ai_re2_spider.c`
  ~1795) ohne `re15_enemy_apply_hitbox`; der Resolver-Zwilling nimmt dann die Sce_em_set-Voreinstellung {0,0,0,1,1,1}
  (@0x800422c8-d0, `re15_damage.c` ~3425) -> R = 1 + 500 = 501. Ein Wurf trifft ein Baby also nur, wenn es in 501 um P
  steht. (`eingriffe=1` dieses Laufs = die Spinnen-Leiche mit gesetztem Bit 0 -> nur `|= 2`, kein Schaden, BAUPLAN 1.5.)
  Reaktion ueber den Mess-Haken (Slot 3, Bild 200, `baby2070_fe{2,3,4}_re2`): HP 1 -> -59 / -89 / -129 (60/90/130),
  Zustand 3, +0x5 9/10/11, Bild X+2 entfernt ("weg" im Zustandslog) — "HP 1 -> Tod, RE2-Tod": ok. `Treffer=2`: ein
  zweites Baby stand im Radius.
* Bilder: `spinne_2070_re2_he_hurt.png`, `spinne_2070_re15_he_tod.png`, `spinne_2070_re2_brand_tod.png`,
  `spinne_2070_re2_he2_babys.png`.

### 3.4 Feuer 0x26 (ROOM1090, RE1.5-Emitter) — echter Wurf trifft physikalisch nie; Reaktion per Mess-Haken OK

* Mess-Haken (`feuer1090_fe2`, Slot 7 bei (1374,1340), Bild 30): debug.log `P=(1374,-3180,1340)` -> Emitter-y zur Laufzeit
  **-2680** (RDT-Satz y -1800 @0x228c, der INIT hebt ihn). HP 100 -> -900 (1000), Zustand 3, +0x5 9, +0x6 1; Folge
  st 3/9/1/0 -> /1 -> /2 -> **st 1, +0x5 = 4 (= grid & 0x7f)** im Bild X+3, kein Tod, HP bleibt -900. Soll "Flammen-
  Minderung 0x80116a04 wie jeder Treffer, HP ungelesen, kein Tod" (Phase 2: `+0x4`-Wort 0x10001, +0x5 = grid & 0x7f,
  `sb zero,147` @0x80116b2c-5c): ok.
* Echter Wurf (`feuer1090_g9`, Leon `-1139,1340,0,1` = Band 1, y -1800, Cut 5): Anker (-171,-2572,989) = Leon-y - 772
  (gueltig). Die Granate faellt durch den Boden bis Welt-y 0 (Routine 29 `blez` @0x80018330-38, BAUPLAN 1.1 "Boden = y 0"),
  liegt bei (2073,3,1051), X = 141. P.y = -497, Emitter Y = -2680, O.y = 0 (`re15_damage.c` 0x26-Kasten {0,0,0,600,720,600}):
  |P.y - Y| = 2183 >= 500 + 720 -> `eingriffe=0`, obwohl waagrecht 756 < 1100. **Kein Port-Mangel** (Original-Physik),
  aber sichtbar: die Explosion leuchtet UNTER dem Truemmerboden (Bild F145/F149 roter Schein im Schutt,
  `feuer_1090_wurf_durch_boden.png`). Nutzer-Hinweis: in Raeumen mit Boden != y 0 verschwindet jede Granate im Boden.

### 3.5 Made/Gorilla 0x27 (RE1.5-KI) — echter Wurf OK, alle drei Granaten

* ROOM11C0 zeigt nach dem Sprung nur die geparkten Maden (grid 0x30, y -20000, Zustand 0). Die aktiven Maden (grid 0x10,
  y 0) setzt sub00 erst hinter `Ck bank 4 bit 64 == 0` -> sonst-Zweig `Ck bank 3 bit 67` (@0x176C/@0x17D0,
  `scd_dump_room.py`) -> `RE15_SET_FLAG=4:64`: zwei Maden HP 180, st 1 Clip 22.
* Erster Versuch (Leon 2513 vor der Made, `made11c0_g9_*`): **kein Wurf** — die Made springt Leon aus 2513 an (Bild 20
  Clip 19, Bild 45 Leon HP 100 -> 88, 2550 zurueckgestossen), der Treffer bricht den Rueckstoss-Clip vor Clipbild 24 ab;
  Munition 5 -> 4, keine Granate (BAUPLAN 1.1: Munition beim Abzug weg). Original-Verhalten des Wurfs, kein Mangel.
* Mit Leon 4400 bzw. 5961 entfernt (`made11c0b_*`, `made11c0c_*`, Cut 5): Made kommt naeher, P liegt 505/560 neben ihr.

| Lauf | Art | KI | HP X-1 -> X | +0x5/+0x6 | X+1 | Leiche | Soll |
|---|---|---|---|---|---|---|---|
| made11c0b_g9_re2 / c_g9_re2 | 2 | (RE1.5) | 180 -> -820 | 9 / 1 | Clip 10 | F160 | 0x8011bb9c Clip 10 (vorn; `ori v0,zero,0xa` / `sb v0,148` @0x8011bc10-14, von hinten 0xb @0x8011bc2c-38), Rueckstoss, Leiche: ok |
| made11c0b_g10_re2 | 3 | (RE1.5) | 180 -> -820 | 10 / 1 | Clip 10 | F160 | 1000, fertige Zeile: ok |
| made11c0b_g11_re2 | 4 | (RE1.5) | 180 -> -820 | 11 / 1 | Clip 10 | F160 | ok |
| made11c0b_g9_re15 | 2 | RE1.5 | 180 -> -820 | 9 / 1 | Clip 10 | F160 | ok (Flavor ohne Einfluss, 0x27 hat keine RE2-KI) |

Rueckstoss gemessen: (-9155,-13997) -> (-9175,-14797) in 10 Bildern (von Leon weg). Leon verliert im Wurf-Lauf 12 HP
durch den Madenangriff (Bild 100), nicht durch die Explosion (Abstand 3337). Bild: `made_11c0_he_tod.png`.

### 3.6 Kakerlake 0x29 (RE1.5-KI) — echter Wurf OK, alle drei Granaten

ROOM5000 (Boden y 0, Kakerlake Slot 1 bei (-14150,-3700), HP 87), `RE15_FORCE_CUT=2`. Zwei Stellungen: a = Leon 2513
vor ihr (`-11656,-3394,1969`), b = 4399 (`-9783,-3165,1969`); sie kriecht auf Leon zu.

| Lauf | Art | HP X-1 -> X | +0x5/+0x6 | X+1 | Leiche | Bemerkung |
|---|---|---|---|---|---|---|
| kaefer5000a_g9 | 2 | 87 -> -913 | 9 / 1 | **Clip 11** | F160 | P liegt HINTER ihr (sie lief an P vorbei auf Leon zu) -> Rueckstoss netto ~0 ((-13753) -> (-13773)) |
| kaefer5000b_g9 | 2 | 87 -> -913 | 9 / 1 | **Clip 10** | F160 | P vor ihr -> Rueckstoss 1600 von P weg ((-12919) -> (-14519) in 20 Bildern) |
| kaefer5000b_g10 / g11 | 3 / 4 | 87 -> -913 | 10 / 11 | Clip 10 | F160 | fertige Zeilen |
| kaefer5000b_g9_re15 | 2 | 87 -> -913 | 9 | Clip 10 | F160 | Flavor ohne Einfluss |

Soll "0x801154b4: Clip 10/11, SE 7, Rueckstoss wie 0x27, Blut, Leiche": ok (vorn/hinten beide Zweige gemessen).
Bild F117-F213 (`kaefer5000b_g9`): getroffen, hochgeworfen, Blut, Leiche am Boden (`kaefer_5000_he_tod.png`).

### 3.7 Tyrant 0x2b (RE1.5-KI) — echter Wurf OK, alle drei Granaten

ROOM5110 (Boden y 0), Tyrant Slot 2 (HP 91) steht still bei (-15946,-6644) (Gier 3072, Blick +z), Leon
`-18007,-8083,3699` (2513), `RE15_FORCE_CUT=1`. P (-15772,-480,-6905), 313 neben ihm — **hinter** ihm (P - Tyrant =
(174,-261), Blick (0,1)). Gegenprobe Leon 4000 entfernt (`tyrant5110b_g9`): Landung 1528 vor ihm > R = 800 + 500 -> kein
Treffer (Geometrie wie berechnet).

| Lauf | Art | HP X-1 -> X | +0x5/+0x6 | X+1 | Phase 2 | Leiche | Soll |
|---|---|---|---|---|---|---|---|
| tyrant5110a_g9 | 2 | 91 -> -909 | 9 / 1 | Clip 9 | F144 Clip 11 | F184 | 0x80114cb0 (STAGE4): `ori v0,zero,0x8` @0x80114d24, bei +0x93 & 0x80 `ori v0,zero,0x9` @0x80114d40-50; P hinten -> Bit 0x80 -> Clip 9, Ph. 2 Clip 0xb: ok |
| tyrant5110a_g10 / g11 | 3 / 4 | 91 -> -909 | 10 / 11 | Clip 9 | Clip 11 | F184 | ok |
| tyrant5110a_g9_re15 | 2 | 91 -> -909 | 9 | Clip 9 | Clip 11 | F184 | Flavor ohne Einfluss |

Phase 2 beginnt in X + 25 (Soll "Bild 24 SE 7, dann Ph. 2"). Bild: `tyrant_5110_he_tod.png` (getroffen, geschleudert,
Blut, Leiche).

### 3.8 Ivy 0x2d — immun: OK (echter Wurf und Mess-Haken)

ROOM4030, Ivy Slot 1 bei (-23040,-23040), Laufzeit-y -450 (Haken-Log `P=(-23040,-950,-23040)`), bleibt in st 1.
* Echter Wurf (`ivy4030_g9`, Leon `-22898,-20531,1061`): P (-23352,-480,-23022) 312 neben Ivy — innerhalb von
  R = 1 + 500 (kastenlos -> Voreinstellung {0,0,0,1,1,1}) und im Band (|dy| 30) — `eingriffe=0`, HP 100 bleibt,
  keine Zustandsaenderung.
* Mess-Haken Art 2/3/4 genau auf Ivy (`ivy4030_fe`): `Treffer=0`, HP 100, Zustand unveraendert.
* Grund: INIT setzt +0x93 = 3 (`enemy_ai_common.c:13870` mit Beleg @0x8011693c-44) -> Gate B `(+0x93 & 3) == 3`
  ueberspringt und zaehlt nicht (@0x80012f54-60). Soll "immun (+0x93 = 3 -> Gate B)": ok. Kein Haenger.

### 3.9 Birkin 0x30 (ROOM3070, RE1.5-KI) — echter Wurf OK; MANGEL: steht in Todesphase 3 wieder auf

* ROOM3070 setzt den kaempfenden Birkin (grid 0x10, (-23860,0,-12720), HP 300) nur hinter `Ck bank 4 bit 215` (sub00
  @0x3408, sonst geparkt bei (30000,30000)) -> `RE15_SET_FLAG=4:215`. Leon `-18605,-14500,2048` (die Kollision schiebt ihn
  auf x -20568), `RE15_FORCE_CUT=5`; der Birkin laeuft heran, P 746 neben ihm (R = 1000 + 500).
* Drei Stellungen (`birkin3070{a,b,c}_g9`) treffen alle: HP 300 -> -700, Zustand 3, +0x5 9, +0x6 1; X+1 Clip 9 (Phase 0
  @0x8011a618: `ori v0,zero,0x9` / `sb v0,148` @0x8011a630-34), Phase 1 spielt Clip 9 (120 Bilder), F239 Phase 2
  (Flag Zone 5 Bit 0x1c `jal 0x8004ef90` @0x8011a6b8 -> Raum-sub01 setzt 4:194/4:236 und stoppt die Boss-Musik),
  ab F240 Phase 3 = Warten auf grid & 0xf == 2 (@0x8011a6c0-e4) — im Original wie im Port dauerhaft (kein SCD setzt das
  grid, sub01 @0x348A nur Flags/BGM). Art 3/4 und RE1.5-Flavor identisch (`birkin3070a_g10/_g11/_g9_re15`). Kein Haenger,
  keine Leiche (Soll E7: Port-Todeshandler 0x8011a5d8, waffenunabhaengig): Ablauf ok.
* **MANGEL M-B1 (Darstellung):** ab F240 steht der tote Birkin wieder aufrecht (Bild F235 liegt, F240/F260 steht,
  `birkin_3070_he_steht_wieder_auf.png`; Zustandslog F240: st 3/9/1/3, Clip 9, **af 0**). Original-Phase 3 @0x8011a6c0-e4
  ruft KEIN `jal 0x8001f314` (nur Phase 1 @0x8011a760) -> der Posen-Puffer behaelt den letzten Keyframe von Clip 9 (liegt).
  Der Port rendert die Pose aus anim_frame, der beim Clip-Ende auf 0 umlaeuft -> Bild 0 = stehend. Dieselbe Fehlerklasse ist
  beim Hund schon behoben (`re15_dog_anim_hold_last`, `enemy_ai_common.c` ~7100: "Pin fc-1 at the wrap"). Nicht
  granatenspezifisch (jeder toedliche Treffer), mit der Granate aber jetzt erreichbar.
* ROOM3080 (0x36): reine Zwischensequenz — player_mode 2 in allen 912 Bildern des Raums (`probe_3080_msg`, Aktionstaste
  alle 1,5 s), danach automatisch ROOM4000/4010. Kein Wurf moeglich (nicht_messbar). Mess-Haken (Slot 3, Bild 100,
  `birkin3080_fe{2,3,4}_re2`, `_fe2_re15`): HP 300 -> -700, Zustand 3, danach **eingefroren** (st 3/9/1/0, Clip 0, af 0
  unveraendert 272 Bilder — die KI tickt in der Sequenz nicht: schon ohne Treffer steht 0x36 dort mit af 0). Im Bild 372
  setzt die Sequenz ihn auf st 1/6 und HP 150. Einordnung: BAUPLAN 1.5 "geparkte Gegner sind Kandidaten, reagieren erst,
  wenn ihre Wurzel wieder tickt" — deckungsgleich; im Spiel unerreichbar.

### 3.10 Zombie-Maedchen 0x13 — im Port NICHT erreichbar (nicht_messbar)

* Einziger Spawn-Ort ROOM4050/4051 (Zensus: 4 Saetze). main00 waehlt die Gegner per `Switch work_vars[0x0A]`
  (@0x1E56, `13 0a d2 01`; work_vars[0x0A] = Eintritts-Cut): Fall 9 -> 0x13 bei (-9900,0,1150) (@0x1EB4), Fall 14 ->
  0x13 bei (1600,0,4700) (@0x1F5C), Faelle 6/11 -> 0x18. Cut 9/14 liefern NUR die Selbst-Tueren des Raums (Slot 6
  @0x1C16 -> Cut 9, Slot 7 @0x1C36 -> Cut 14; `tuer_nach.py 3 05`). Der Debug-Sprung kommt mit Cut 0 (kein Fall).
* Gemessen: `RE15_FIRE_AOT=6@30#4050` (`probe_4050_tuer6`) teleportiert Leon nach (-9350,-2600), aber main00 laeuft
  nicht neu — kein Gegner in 600 Bildern. Ursache im Port benannt: `aot_common.c` ~790-801 ("BEWUSST NICHT
  verallgemeinert: Stage >= 2 ... 46 weitere Selbst-Tueren") — der Wiedereintritt gilt nur fuer Stage-1-Raeume; das
  Original laedt jede Tuer neu (`jal 0x800396fc` @0x8001d988 unbedingt, darin `jal 0x8003ef6c` @0x80039a00).
  `RE15_FORCE_CUT=9/14` beim Sprung wirkt nicht auf den Switch (`probe_4050_c9/_c14`: kein Gegner).
* Folge: 0x13 (und die 0x18-Faelle 6/11 dieses Raums) erscheinen im Port nie — kein Granaten-Mangel, aber eine
  bestehende Luecke (siehe Mangel M-H3). Die Reaktion laeuft ueber dieselbe RE2-Zombie-Familie
  (`re15_re2z_owns_type` 0x13) wie der in W9 gemessene 0x10.

### 3.11 Gitterhaende / Zellenarm 0x1a (ROOM1210) — RE1.5: echter Wurf OK; RE2: echter Wurf erreicht den Arm nie (Hoehe), Reaktion per Haken OK

* **RE1.5-KI** (`arm1210b_g9_re15`, Arm Slot 3 auf RDT-Lage (-14000,0,-5897), Leon `-16513,-5897,0`, Cut 3): P 391 neben
  dem Arm, HP 74 -> -926 (1000), Zustand 3, +0x5 9, X+1 Clip 3, Leiche F174. Soll "Port-Tod Clip 3 -> Leiche (Original
  NULL, E7)": ok. (Mess-Haken `arm1210_fe2_re15`: dasselbe, Leiche F95.)
* **RE2-KI (Zellenarm)**: der Port setzt die zehn Arme ins Fensterloch — x = Wandflaeche -/+ 400, **y = -2500**
  (`enemy_ai_re2_zellenarm.c:177-200`, RE2ARM_1210_Y, am Originalhintergrund gemessen). Echter Wurf (`arm1210_g9_re2`,
  Leon `-18789,-5897,0`): P (-16278,-480,-6208) 311 waagrecht neben Arm 3, aber Band |P.y - (Y + O.y)| =
  |-480 - (-2500 - 1440)| = 3460 >= 500 + 1440 -> `eingriffe=0`. Folge der Portgeometrie (Arm im Fenster, Granate am
  Boden) — kein Resolver-Mangel.
  Mess-Haken nach dem Wecken (`arm1210_fe2wach2_re2`, `RE15_FIRE_AOT=6@20#1210` = Weck-Zone, sub02 Member_set(12,1)):
  HP -1 -> -1001 (Arm war im Versteck-Unterzustand 7 mit HP -1), Zustand 3, **X+1: HP 250, st 1 / Unterzustand 5
  (Rueckzug)**, F71 wieder versteckt (sub 7). Soll "RE2: Rueckzug, unsterblich (zeilenunabhaengig)" = `arm_death`
  (HP 250 `sh 250,342` @0x80101090-94, Zustandswort 0x501 @0x8010109C-A0): ok.
  Schlafend (ohne Weckzone, `arm1210_fe2_re2`/`_fe2wach_re2`): Zustand 3 bleibt eingefroren (st 3/9/1/0, kein Tick
  solange grid & 0x1F != 1, @0x8002659C-Analog) — BAUPLAN 1.5 "reagieren erst, wenn die Wurzel tickt" (bau_b OFFEN
  "schlafender RE2-Arm").
* Hinweis H-Z1: der RE1.5-Resolver nimmt den versteckten RE2-Arm (HP -1) als Kandidaten; RE2s Applier schloesse HP < 0 aus
  (Gate 3 `lh v0,342(s0)` / `bltz` @0x80047148-50, im Port nur fuer NPC 0x40..0x4D und 0x37 nachgebaut,
  `re15_damage.c` ~3495-3510). Wirkung: ein versteckter Arm spielt den Rueckzug erneut. Mit echten Granaten nicht
  erreichbar (Hoehe), daher nur Hinweis.

### 3.12 G5 (ROOM5090, RE2-Modul 0x36) — echter Wurf OK: Schaden 80/70/70, Flinch 14 je Zeile, STAGGER beim zweiten Treffer

Harness wie `analysis/befunde_2026-09-19/birkin-g5.md` §6.4: `RE15_DEBUG_JUMP=5090@30` (Sprung VOR dem Ereignis),
`RE15_FORCE_EVENT=4@120` (sub04 = echter Kampfstart), Leon `-2000,-23350,2048`, Skript ab Bild 900
(`SKRIPT_AB=900`), `RE15_BIRKIN_DBG=1` (g5-Zeile alle 15 Ticks: sub/ph/clip/hp/akku). Die Masse schiebt Leon den Wagen
entlang; im Bild 900 steht sie bei (1960,-23400), Leon bei (4593,-26223). (Erster Versuch mit Sprung @250 feuerte das
Ereignis noch im Startraum — `probe_5090_g5`, G5 blieb in st 0; Harness-Falle, nicht Spiel.)

| Lauf | Art | HP X-1 -> X | Akku | Bemerkung | Soll (E16 / 1.6) |
|---|---|---|---|---|---|
| g5_a900 / _bild | 2 | 600 -> 520 (80) | 14, dann -1 je ~16 Bilder (14,13,12,11,11,10,...) | P 1383 neben der Masse, kein STAGGER | Zeile 9 Kl. 0 = 80, Zuschlag 14 < 15, Zerfall 1/16 im Fenster 15: ok |
| g5_a900_g10 | 3 | 600 -> 530 (70) | 14 | +0x5 10 -> Zeile 11 -> Byte 14 | 70, 14: ok |
| g5_a900_g11 | 4 | 600 -> 530 (70) | 14 | +0x5 11 -> Zeile 10 -> Byte 14 | 70, 14: ok |
| g5_a900_x2 (zwei HE, X 1018 / 1069) | 2+2 | 600 -> 520 -> 440 | 11 + 14 = 25 >= 15 -> **STAGGER**: X2+1 Clip 8 (Flinch, 50 Bilder, F1070-F1119), danach Clip 5; Akku 0 | Schwelle 15 -> STAGGER, Akku/Zaehler/Fenster zurueck (@0x80102a58/@0x80102a84): ok |

Kein Haenger (exe bis EXIT_AT, der Kampf laeuft weiter). Beobachtung: das Zustandswort bleibt nach dem ersten Treffer auf
st 2 / +0x5 (das RE2-Modul fuehrt eigene sub/ph und liest +0x4 nicht zurueck) — ohne sichtbare Folge im Kampf.
Bild `g5_5090_he2_stagger.png` (F1066 vor, F1070-F1110 im Flinch). Die Explosion selbst liegt ausserhalb des Kampf-Cuts.

### 3.13 Gator-Boss ROOM2090 (Port-Modul) — echter Wurf (MITTE) OK: 1000, Modul-Absorb

* Der Mess-Haken passt hier NICHT: das Modul hebt das Kollisionszentrum auf Bodenhoehe (`hit_offset_y = -GB_WATER_Y`,
  `enemy_ai_boss_gator.c` ~1048; Gator-y -1200), der Haken setzt P aber auf Gator-y - 500 = -1700 -> Band
  |P.y - 0| = 1700 >= 500 + 480 (Hoehe 720 x 2/3) -> `Treffer=0` (`probe_2090`). Harness-Grenze, kein Mangel.
* TIEF aus 2513/4000/6000 (`gator2090_d*`): der Gator (Aggro < 9000, GB_AGGRO) beisst Leon im Bild 14 aus 3444 Abstand
  (gator_boss.log `BISS ... maul=864`), Leon HP 100 -> 50, der Wurf bricht ab (keine Granate). Boss-Reichweite (Nutzer-
  Design), kein Granatenfehler.
* Loesung: Leon ausserhalb der Aggro (12900 vor dem Gator), MITTE-Wurf (`M`, ohne Hoch/Tief). Die Auto-Nachfuehrung
  dreht Leon auf die naechste Spinne (Radius 30000, rundum, `re15_player_aim_target` / FUN_8003703c), danach LINKS im
  Halten 48 je Bild (gemessen 227 -> -1069 in 27 Bildern) auf die errechnete Gier. Kalibrierlauf `gator2090_mitte1`:
  MITTE landet 12856 vor und 1345 seitlich (Gier im Spawnbild). Messlauf `gator2090_mitte2` (RE1.5-Flavor, Spinnen
  stehen still; Leon `-7300,-23121`, Skript `M0.6,ML0.73,MA0.2,M3.5,W6`): Spawn F63 Gier -957, Liegen (-7347,9,-10196),
  X = 172, P 1006 vom Gator.
* Ergebnis: HP 3000 -> **2000 (1000)**, Zustand 2, +0x5 9; X+1 fängt das Modul den Treffer ab (st 1, **Clip 10** =
  Flinch an der 10-%-Schwelle), danach Aggro und Angriff auf Leon (gator_boss.log ph 3). Soll "1000 (Bestand), Modul-
  Absorb (Nutzer-Design) — unveraendert": ok. Kein Haenger.

### 3.14 Alligator 0x23 ausserhalb 2090 — nicht messbar

`spawn_zensus.py`: 0 Sce_em_set-Saetze fuer 0x23 in allen 240 RDTs. Der Port setzt 0x23 nur in ROOM2090/2091
(`main.c` ~7425, Port-Ergaenzung) und dort uebernimmt das Boss-Modul (`re15_gator_boss_active`: Typ 0x23 &&
(Raum & 0xFFFE) == 0x2090, `enemy_ai_boss_gator.c:304-307`). Der RE1.5-Pfad `re15_alligator_ai_tick` (Soll 0x8010ea30
Clip 13) ist im Spiel unerreichbar; abgedeckt nur durch die Sonde `unit_r34_reaktion` (Spur B).

### 3.15 NPC (0x45, ROOM1150) — kein Kandidat: OK

Das Buero-Ereignis (Aot_set Slot 6, sce 3, Rechteck x -27300..-15800 / z -23800..-21800, main00 @0x0DEA) sperrt den
Spieler (pm 2), sobald Leon das Band betritt (`npc1150_g9`: kein Wurf). Leon deshalb SUEDLICH des Bandes
(`npc1150b_g9`, `-18941,-24931,1724`, Cut 5): TIEF-Wurf, P (-21298,-480,-25854) **314 neben dem NPC** (Slot 1, Typ 0x45,
HP -1, Zustand 4 = Skript). Waagrecht in R = 500 + 500, Band in jedem Fall (Kasten {500,1440}) — trotzdem `eingriffe=0`,
HP bleibt -1, Zustand/Clip laufen unveraendert weiter. Grund: `re15_resolve_attack` ueberspringt 0x40..0x4D mit HP < 0
(`re15_damage.c` ~3495-3505, Beleg RE2 Gate 3 @0x80047148-50 / RE1.5-INIT HP -1 @0x8011d320-24). Soll "KEIN Kandidat
(HP -1)": ok. Bild `npc_1150_kein_kandidat.png` (Explosion auf dem Sofa, NPC unberuehrt). Leon unverletzt.

### 3.16 Zusatz Zombie-Sonderzeilen ROOM1140 (liegender Fresser 0x16, Brad 0x11) — Mess-Haken

Die echte Wurfkette an der Zombie-Familie misst `integration_r34_granaten` (W9). Die beiden Sonderzeilen aus §1.6 hier
per Mess-Haken (`fresser1140_fe_{re2,re15}`: F30 Art 2 auf Slot 1, F32 Art 2 auf Slot 5, F90 Art 3 auf Slot 4; Leon am
Sprungpunkt, 7400 entfernt, alle schlafen/fressen):

| Ziel | KI | Ergebnis | Soll §1.6 | Einordnung |
|---|---|---|---|---|
| 0x16 liegend (grid 0x88) | RE1.5 | `Treffer=1`, HP 65 bleibt, Zustand bleibt | "+0x93 = 1 in Ruhe -> nur \|= 2, KEIN Schaden" | ok |
| 0x16 liegend (grid 0x88) | RE2 | `Treffer=1`, **HP 79 bleibt**, Zustand bleibt | "Port-RE2-Bruecke macht die Spawn-Pose treffbar -> stirbt" | **Abweichung vom BAUPLAN-Text**, aber Port-Stand seit Runde 16 bewusst: der passive Liegende (grid & 0x80, Nibble 7..10) ist von der Spawn-Pose-Ausnahme ausgenommen (`enemy_ai_re2_zombie.c` ~8899-8935, "in BEIDEN Originalen unschiessbar": RE1.5 `ori v0,v0,0x1` / `sb v0,147` jeden Tick @0x80103AAC-AB8, RE2 Gates (2)/(4) @0x80047138-40 / @0x80047158-64). Die BAUPLAN-Zeile zitiert die Fresser-Ausnahme (8809-8815) — veraltet (Mangel M-H4, Doku) |
| 0x11 Brad (HP 250), HE | RE2 | 250 -> 50 (200), st 2, +0x5 9, X+1 Clip 2, danach Gang (st 1/1) | "lebt: HE HURT" | ok |
| 0x11 Brad, Saeure | RE2 | 250 -> 50, st 2, +0x5 11, X+1 Clip 4; danach **HP 50 -> 46 -> 45 -> 43** im Gang | "HURT[11][0] 0x80105BC0 Taumeln + Aetzung ... DoT 1 HP je 8 Bilder im Gang" | ok (DoT gemessen) |
| 0x11 Brad, HE / Saeure | RE1.5 | 250 -> 50 (Import-Option AN = RE2-Modellschaden 200, E4), st 2, +0x5 9/10, +0x6 1, X+1 Clip 4 bzw. 3, danach Gang | "Import AN: wie RE2-Modell ... Port-HURT" | ok |

## 4. Maengel und Hinweise (mit Beleg)

**M-B1 (mittel, Darstellung) — Birkin 0x30 steht nach dem toedlichen Treffer in Todesphase 3 wieder auf.**
* Lauf `birkin3070a_g9_bild` (ROOM3070, `RE15_SET_FLAG=4:215`, Cut 5, Framedump x3): F235 liegt der Birkin am Ende von
  Clip 9, **F240/F260 steht er aufrecht** (Bild `mess_geg_belege/birkin_3070_he_steht_wieder_auf.png`). Zustandslog F240:
  `st=3 ss1=9 ss2=1 ss3=3 mo=9 af=0` und so bis Laufende (F300/F330, alle Arten, beide Flavors).
* Original: Phase 1 ruft `jal 0x8001f314` @0x8011a760 (Anim), Phase 3 @0x8011a6c0-e4 prueft nur `lbu v0,9(a0)` /
  `andi v0,v0,0xf` / `bne v0,v1` und ruft KEIN Anim -> der Posen-Puffer behaelt den letzten Keyframe von Clip 9 (liegt).
* Port: `enemy_ai_common.c:12009-12013` `if (re15_birkin_anim(e)) e->sub_state_3 = 2;` — `re15_birkin_anim` laesst
  anim_frame beim Clip-Ende auf 0 umlaufen (`% fc`, :11595), der Renderer posiert daraus Bild 0.
* Vorschlag: am Ende von Phase 1 (und Phase 5, :12038) das letzte Bild halten wie beim Tyrant in Runde 34 B8
  (`e->anim_frame = re15_birkin_fc(e) - 1;` :13778/:13792) bzw. wie beim Hund (`re15_dog_anim_hold_last`). Nicht
  granatenspezifisch (jeder toedliche Treffer), mit der Granate aber jetzt direkt sichtbar.

**M-H1 (Hinweis, bekannt OFFEN) — Hund RE2, Saeure-Tod ohne FX 9/10.** `hund11d0_g10_re2`/`hund3060c1_g10_re2`: im
Bild X+1 nur 1x `SPAWN id=0 sub=1` (wf.log), waehrend HE 13 + 6 und Brand 6x id8/3 spawnen. Ursache benannt in
`enemy_ai_re2_dog.c:347` (`if (fx == 6 || fx >= 9) return;` — Arten 0x84/0x86 der Tabelle @0x801056AC [9]/[10] "kein
RE1.5-Pendant (OPEN, stumm)"). Die Part-Farbe 0x00003F2F wirkt (Bild `hund_3060_re2_saeure.png`).

**M-H2 (Hinweis, Original-Physik laut BAUPLAN 1.1) — Granate faellt durch erhoehte Boeden bis Welt-y 0.** ROOM1090
(`feuer1090_g9`): Leon auf y -1800, Liegestelle y 3, Explosion unter dem Truemmerboden (Bild `feuer_1090_wurf_durch_boden.png`),
kein Emitter erreichbar. Gleiches gilt fuer alle Raeume mit Boden != 0 (Zensus-Lagen: 1170 Heliport -7200, 2030/2050
-5400, 20A0 -3600, 3040 -1800, 5080 -5600) — dort trifft KEINE Granate etwas. Beleg der Regel: Routine 29 `lh t1,42(t0)`
/ `blez t1` @0x80018330-38 (BAUPLAN E12). Kein Port-Mangel, aber fuer den Nutzer sichtbar — melden.

**M-H3 (Hinweis, bestehende Port-Luecke) — Zombie-Maedchen 0x13 erscheint im Port nie.** ROOM4050 waehlt 0x13 ueber den
Eintritts-Cut 9/14 (Switch work_vars[0x0A] @0x1E56), den nur die Selbst-Tueren liefern; der Port laedt Selbst-Tueren
in Stage >= 2 nicht neu (`aot_common.c` ~790-801 "BEWUSST NICHT verallgemeinert"), das Original schon (`jal 0x800396fc`
@0x8001d988). Gemessen `probe_4050_tuer6`: Teleport ohne Neuaufbau, kein Gegner. Blockiert die 0x13-Messung.

**M-H4 (Hinweis, Doku) — BAUPLAN §1.6 "Liegender Fresser 0x16, RE2-KI -> stirbt" ist ueberholt.** Gemessen
(`fresser1140_fe_re2`): kein Schaden (Treffer gezaehlt, Bit 0 gesetzt -> nur `|= 2`). Port-Stand seit Runde 16: der
passive Liegende (grid 0x88) ist in beiden Originalen unschiessbar (`enemy_ai_re2_zombie.c` ~8899-8935 mit Belegen
@0x80103AAC-AB8 / @0x80047138-40 / @0x80047158-64). Die BAUPLAN-Zeile zitiert die Fresser-Ausnahme (8809-8815).

**H-Z1 (Hinweis)** RE1.5-Resolver nimmt den versteckten RE2-Zellenarm (HP -1) als Kandidaten (RE2 Gate 3
@0x80047148-50 nur fuer NPC/0x37 nachgebaut, `re15_damage.c` ~3495-3510) — mit echten Granaten unerreichbar (Arm y -2500).
**H-K1 (Hinweis, bekannt OFFEN)** RE2-Kraehe GIB: Teile-Wurf nicht modelliert, Modell wird verborgen
(`enemy_ai_re2_crow.c:56-57`). **H-G5 (Beobachtung)** G5: +0x4 bleibt nach dem ersten Treffer auf 2 (Modul fuehrt eigene
sub/ph), ohne sichtbare Folge.

## 5. Harness-Befunde (fuer kuenftige Messlaeufe, kein Spielverhalten)

1. `RE15_PLAYER_POS` ohne Kamerawechsel: steht Leon ausserhalb des Cut-Bereichs (`cut_regionen.exe` "gecullt"), posiert
   der Renderer Knochen 11 nicht fuer den Wurf -> Anker in Bindpose-Hoehe (Integrationslauf `dog3`: -1693 statt -772).
   Abhilfe: `RE15_FORCE_CUT=<Cut, dessen Bereich Leon enthaelt>`; Pruefung: Anker TIEF ~970 vor Leon, y -772.
2. Die Region-Quads (`cut_regionen.exe`) sagen nichts ueber das Bild; fuer Belegbilder zusaetzlich `kamsicht.py`
   (Projektion pos->Ziel, H = fov>>7).
3. `RE15_FORCE_EXPLOSION` setzt P.y = Aktor-y - 500. Beim Gator-Boss (Kollisionszentrum auf Bodenhoehe gehoben) liegt P
   damit ausserhalb des Bands -> `Treffer=0`; dort nur echte Wuerfe.
4. `RE15_FORCE_EVENT=4@120` mit `RE15_DEBUG_JUMP=5090@250` feuert das Ereignis noch im Startraum; Sprung `@30` nehmen
   (wie `analysis/befunde_2026-09-19/birkin-g5.md` §6.4).
5. Die RE1.5-Kraehe fliegt unter 5000 Abstand auf (@0x801126b4-f8) — ein TIEF-Wurf (Reichweite ~2500) erreicht sie nie
   am Boden.

## 6. Fazit

Alle erreichbaren Gegnertypen wurden mit ECHTEN Wuerfen (TIEF, fuer den Gator-Boss MITTE) und allen drei Granaten
gemessen, die RE2-KI-Typen in beiden Flavors: Treffer, Schaden (1000 bzw. RE2-Modellwerte 300/60/60-90-130/80-70-70/200),
Zeile, Reaktionsclip und Leiche entsprechen BAUPLAN §1.6 bzw. den selbst gelesenen Handlern. In 160 exe-Laeufen (inkl. Zensus/Sonden) gab es
keinen einzigen Haenger: 159 endeten per `RE15_EXIT_AT` (rc 0); `probe_3080_msg` lief in den Zeitdeckel (rc 124),
weil sein `EXIT_AT` an ROOM3080 gebunden war und das Spiel planmaessig nach ROOM4000/4010 weiterlief (Zustandslog bis
zum Schluss fortlaufend, 5601 Bilder in 4010). Mess-Haken nur dort, wo ein Wurf physikalisch nicht
trifft (RE1.5-Kraehe, Baby, Feuer 1090, RE2-Zellenarm) bzw. der Raum keine Steuerung erlaubt (3080). Nicht messbar:
Alligator 0x23 ausserhalb 2090 (kein Spawn), Zombie-Maedchen 0x13 (Port-Luecke M-H3), Birkin 0x36 in 3080 per Wurf.
Ein echter Mangel: M-B1 (Birkin steht nach dem Tod wieder auf).
