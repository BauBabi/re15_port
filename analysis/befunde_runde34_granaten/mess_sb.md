# Runde 34 (Granaten) — MESSUNG Saeure- (0x0A) und Brandgranate (0x0B) an der echten exe

Stand: 2026-09-30, Zweig `r34g/integration` HEAD ef1c6f94 (Code-Stand c8cb4040), exe
`re15_port/build_r34_int/platform/pc/re15_pc.exe` (md5 f99f6f2c7f5e258ab9794d30b59e96b2), gemessen an der byte-gleichen
Kopie `re15_pc_sb.exe` im selben Verzeichnis (Schutz vor fremdem `taskkill /IM re15_pc.exe`; nach den Laeufen geloescht,
`lauf_sb.sh` legt sie bei Bedarf per `cmp`/`cp` neu an).
Rolle: MESSER — nur messen, KEIN Code geaendert, KEIN Commit (der Orchestrator committet dieses Dossier).
Soll: `BAUPLAN.md` §1.1/§1.3/§1.4/§1.5/§1.6 (+ Nachbesserungen bau_b N2/N3 fuer Art 5 am RE1.5-KI-Zombie).
Rohdaten: `build/r34g_mess_sb/` (unversioniert; je Lauf `laeufe/<marke>/` mit env.txt, debug.log, state.log, gr.log, wf.log,
fx.log, gdb_fx.log; Framedumps nachtraeglich verlustfrei nach `f_NNNNNN.png` gewandelt, 2.2 GB -> 216 MB, `ppm2png.py`).
Belegbilder: `analysis/befunde_runde34_granaten/mess_sb_belege/` (5 PNG, je 100-180 KB).

Dieses Dossier wird fortlaufend geschrieben (Sitzungslimit-Schutz).

## Fazit (Kurzfassung)

Saeure- und Brandgranate laufen an der echten exe in zwei Raeumen (ROOM1140, ROOM1030), mit RE2- und RE1.5-KI, mit und ohne
Import-Option, ueber `RE15_GIVE` und ueber das Item-Debug, ohne Haenger. Gegen den BAUPLAN gemessen und bestaetigt: gleicher Wurf
wie 0x09 (Platzzeilen byte-gleich), X = L+36, im Explosionsbild nur Resolver Art 3/4 + RE2-Aufschlag (kein HE-Kind, kein SE
0x04080001, kein Licht-Latch — Positivkontrolle 0x09 zeigt alle drei), Saeure-Kind-Folge X..X+4 exakt nach RE2 Op 49, Brand
Op 48 mit Runde + 2 Kindern + 3 Bodenflammen + 3 Folgeflammen, Flammen gleiten 850..1570 und brennen 135..150 Bilder,
Nachbrenner ab X+19, Toene ARMS10/ARMS11 Satz 10 (VAG 3) genau einmal, Zombie-Zeile 11/10 (RE2) bzw. 10/11 (RE1.5), Schaden
200/80/1000 je Modell, DoT 1 HP je 8 Bilder mit Tod 0x0B03 (Saeure) und 0x0A03 (Brand), Nachbrenner 5 (RE2-Modell) bzw. 50
(Import AUS) mit Zeile 14 an RE1.5-KI, Spieler vom Bodenfeuer nie getroffen, Eigentreffer 1000 mit Clip 7 und CORE-3-Ton.
Zwei Maengel [mittel], beide im Port-Zuordnungs-Pfad O-VB4 (Bodenfeuer an RE1.5-KI-Gegnern): 3.1 kein Treffer-Takt (alle 6
statt 15 Bilder) und 3.2 Riegelzweig je Bild (Blutfontaene am Gegner, Flamme "verbraucht" ihren Treffer an einem gesperrten
Gegner).

## 0. Fortschritt

| Schritt | Stand |
|---|---|
| Werkzeug (Lauf-Skript, gdb-Protokoll der RE2-FX-Maschine, Auswerter) | erledigt (`build/r34g_mess_sb/lauf_sb.sh`, `gdb_fx.py`, `auswert_sb.py`) |
| M1-M4 echte Wuerfe ROOM1140 (0x0A/0x0B x RE2-/RE1.5-KI, Aufstellungen des Integrationstests) | gemessen (Abschnitt 2.1-2.4) |
| K0 Positivkontrolle 0x09, N1 Nachbrenner RE2-KI (FORCE_AUFSCHLAG) | gemessen (2.5, 2.6) |
| D1-D4 DoT/Tod 0x0A03/0x0B03 (FORCE_EXPLOSION an Brad 0x11 / 0x16 HP 80) | gemessen (2.7) |
| R1-R5 + M5 zweiter Raum ROOM1030 (0x16-Zombies) + Import AUS | gemessen (2.8, 2.9) |
| F1/F2 FORCE_AUFSCHLAG Sichtabnahme ROOM1140 Tuer / ROOM1030 Eingang | gemessen (2.10) |
| Einzelmessungen (gleicher Wurf, Ton hoerbar, Eigentreffer, Spieler im Feuer, Item-Debug 0x0A/0x0B) | gemessen (2.11) |
| Maengel 3.1/3.2 mit gdb-Watchpoint und Bildbeleg | erledigt |

## 1. Messaufbau

* **Lauf** (`build/r34g_mess_sb/lauf_sb.sh <marke>`): Kopie `re15_pc_sb.exe` (vor jedem Lauf per `cmp` gegen `re15_pc.exe`
  geprueft), Titel-Autostart, `RE15_DEBUG_JUMP=<raum>@250` (Bildzaehler faengt nach dem Sprung neu an), `RE15_NOAUDIO=1`,
  Logs `RE15_STATE_LOG`/`RE15_GRANATE_LOG`/`RE15_WAFFEN_LOG`/`RE15_FX_LOG`, Framedumps `RE15_FRAMEDUMP` mit
  `RE15_WINDOW_SCALE=3` (echte exe, beschleunigter Renderer; kein AUTOSHOT/SOFTWARE_RENDER). Wurf-Skript wie
  `integration_r34_granaten`: `MD0.6,MDA0.2,MD2.5,W5` ab Spielbild 1 (TIEF, Abzug Bild 19).
* **gdb-Protokoll der RE2-FX-Maschine** (`gdb_fx.py`, GDB=1): die exe laeuft unveraendert unter
  `C:/msys64/mingw64/bin/gdb.exe`; Haltepunkte kehren sofort zurueck und LESEN nur Speicher (ASLR-fest ueber
  `re2fx_tick`): Pool-Abbild `s_pool` (96 x 136 Byte, Abbild +0x00..+0x7B) am `ret` von `re2fx_tick` (= Zustand NACH der
  Pumpe des Bilds), jeder `spawn_kern`-Aufruf (a0/a1/st0/Matrix-Translation/ofs), jeder Op-40-Applier-Ruf mit Rueckgabe
  (`treffer` = Gegner-Slot + 1, 0 = kein Treffer; `re15_re2_gl_apply` gibt `s + 1` zurueck) und `re2fx_aufschlag`.
  Adressen aus `nm -n`/`objdump -d` der Kopie (md5 f99f6f2c...): re2fx_tick 0x1400f4b10 (`ret` @0x1400f4d09),
  spawn_kern 0x1400f1618, op_40 0x1400f3761 (nach `call *%rbx` @0x1400f3813), g_engine 0x1404b5020 (+0 frame_count),
  s_pool 0x140bbe080, s_cur 0x140bc1380. Laufzeit unter gdb ~20 s wie ohne (kein Bild-Takt-Unterschied: alle Zeitangaben
  sind Spielbilder).
* **Auswertung** `auswert_sb.py <lauf>`: A (erstes Bild mit fallender Munition), S/L/X/P aus gr.log, Ereignisse des
  Explosions-Ticks, Toene aus wf.log, Spawns/Belegungen/Op40 aus gdb_fx.log, Treffer (HP-Abfall in Zeile X), Reaktion in
  X+1, HP-/Zustandsverlauf.

## 2. Ergebnisse je Szenario

### 2.1 M1 — Saeure 0x0A, echter Wurf, RE2-KI, ROOM1140 (`laeufe/m1_saeure_re2`)
Aufstellung wie `integration_r34_granaten` g10_re2: `POS=-1676,-18070,1076`, `AI=re2`, `GIVE=10:5 EQUIP=10`, Skript TIEF.
| Pruefung (Soll BAUPLAN) | gemessen | Ergebnis |
|---|---|---|
| A / S = A+24 (TIEF, §1.1) | A 19, S 43 (`gr.log: F=43 SPAWN granate art=3 slot=0 anker=(-2108,-772,-19007) gier=1079`) | ok |
| Wurf-Toene 6x (5 Abpraller + Liegen, ARMS Satz 10) | `010a0401, 0301, 0201, 0101, 0001` + Liegen `010a0001`, wf.log je `bank=1 satz=10 -> ARMS` | ok |
| X = L+36 | L 83, X 119 | ok |
| Explosionsbild: Resolver Art 3, P = (x, y-500, z), KEIN HE-Kind/SE/Latch/Licht | `T=365 EV resolver art=3 P=(-2199,-480,-20548) r=500 eingriffe=1`, `EV aufschlag re2_art=2 q=(-2199,20,-20548) gier=1079`; keine `EV latch`/`EV kind`, kein `04080001`, kein `[licht]` im debug.log (Positivkontrolle K0 mit 0x09: alle vier vorhanden) | ok |
| Ton ARMS10 Satz 10 genau einmal | `SE  re2fx code=0x01130001 -> ARMS10 Satz 10` (1x) | ok |
| Kind-Folge X..X+4 (§1.3) | gdb: F119 `020c1000` (Runde) + `030f2000 040c2000 041d1800`; F120 `031f2000`; F121 `03142000`; F122 `040d2800`; F123 `030f2000`; Runde (P95, Op B 49) belegt F119..F122, frei in F123 | ok |
| Lage der Kinder | Bild 0 an Q = (-2199,20,-20548) (ofs = Q); Phase 1..4 an der mitlaufenden Rundenlage (-2220,20,-20787) / (-2240,43,-21025) / (-2260,89,-21263) / (-2280,158,-21501): +~240 je Bild in Wurfrichtung (Gier 1079), y sinkt unter den Boden (vel.y 240 / acc.x -23 im 0x400-Rahmen, O-VB1) | siehe 4.1 |
| Treffer + Reaktion (§1.6, E6) | Slot 3 t=10 HP 80 -> -120 (-200 = Z11 K0), st 3, +0x5 = 11 in X; X+1 mo 0->1, ss2 0->1; Leiche (st 7) ab F180 | ok |
| Spieler in X | HP 100 -> 100 | ok |
| kein Haenger | EXIT_AT 240 erreicht, rc 0 | ok |
| Bild | F120 grauer Puff an der Liegestelle, F121-F124 weiss-gelb, F126 orange, dunkel; Effekt am unteren Bildrand der Kamera 1 teilweise abgeschnitten (Kameralage, nicht Effektlage); getroffener Zombie dunkel (Aetzung) | ok |

### 2.2 M2 — Brand 0x0B, echter Wurf, RE2-KI, ROOM1140 (`laeufe/m2_brand_re2`)
| Pruefung | gemessen | Ergebnis |
|---|---|---|
| A/S/L/X, Toene des Wurfs | A 19, S 43, L 83, X 119; 6 Wurf-Toene wie M1 | ok |
| Explosionsbild | `EV resolver art=4 P=(-2199,-480,-20548) ... eingriffe=1`, `EV aufschlag re2_art=1`; kein HE-Inhalt, kein Licht | ok |
| Ton ARMS11 Satz 10 genau einmal | `SE  re2fx code=0x01120001 -> ARMS11 Satz 10` (1x) | ok |
| Op 48 Bild 0 (§1.4) | F119: Runde `020c1000`, Kinder `040c2800` + `041d2700`, drei Bodenflammen `05052e00 05053100 05052200` (Skala 11776/12544/8704 = 7168 + 6/7/2 x 768); Runde P95 nur F119 belegt (Phase 1 gibt frei) | ok |
| Folgeflammen (Op 29) | F122 (X+3): `05041b33 05042733 050424cc` = 0.8 x 8704/12544/11776, je eine | ok |
| Flammen-Zeitlinie (flammen.py) | Spawn X (0x4000), X+1 Op 27 (b003, Op A 58 / B 28, Zaehler 10/8/9 = 8 + rng%3, Lage Q), X+2 Landung Op 46 (A 19, B 29, Zaehler 45/40/42 = 38 + rng%8), Gleiten bis vel.x 0 in F132..F134, Weg 1218/1362/1438, Zustand 1 in F162..F167 mit Zaehler 91/92/95 (= 90 + rng%11), tot F256/F258/F260 -> Belegung 138/140/142 Bilder | ok (Soll ~130-145) |
| Op-19-Tor | step[0x16] >= 16 ab F137, erster Op-40-Ruf F138 = X+19 | ok |
| Nachbrenner-Treffer | 197 Op-40-Rufe, alle 0: kein Gegner lief in die Flammen (Zombie 3 tot, Brads bei Leon) | nicht gemessen hier -> 2.6 |
| Treffer + Reaktion | Slot 3 t=10 HP 80 -> -120 (-200 = Z10 K0), st 3, +0x5 = 10; X+1 mo 0->2 (Sturz-Tod Clip 2), ss2 1; Leiche ab F170 | ok |
| kein Haenger | EXIT_AT 300 erreicht | ok |

### 2.3 M3 — Saeure 0x0A, echter Wurf, RE1.5-KI (`laeufe/m3_saeure_re15`)
Aufstellung g10_re15: `POS=-4311,-19289,0`, `AI=re15` (Import-Option Standard AN).
| Pruefung | gemessen | Ergebnis |
|---|---|---|
| A/S/L/X, Toene, Explosionsbild, ARMS10 | wie M1 (A 19, S 43, L 83, X 119; `resolver art=3 P=(-1856,-480,-19902)`, `aufschlag re2_art=2 q=(-1856,20,-19902) gier=79`, 1x ARMS10 Satz 10, kein HE-Inhalt/Licht) | ok |
| Kind-Folge X..X+4 | identisch M1 (F119 4 Spawns, F120 031f2000, F121 03142000, F122 040d2800, F123 030f2000) | ok |
| Treffer + Reaktion | Slot 2 t=10 HP 75 -> -125 (-200: Import-Option = RE2-Modell, E4), st 3, +0x5 = 10 (RE1.5 DAT_8006f430[3] = 10); X+1 mo 1->11, ss3 0->1; Leiche ab F173 | ok |
| Bild (`mess_sb_belege/m3_saeure_re15_crop.png`) | F116/F118 Granate liegt (oliv) am Fuss des Zombies; F119 weg (Flags 0x61); F120 grauer Puff GENAU dort; F121-F124 weiss-gelb, F126 orange, F128-F130 rot/dunkel, F134 leer | ok |

### 2.4 M4 — Brand 0x0B, echter Wurf, RE1.5-KI (`laeufe/m4_brand_re15`)
| Pruefung | gemessen | Ergebnis |
|---|---|---|
| A/S/L/X, Toene, Explosionsbild, ARMS11 | wie M2 (`resolver art=4`, `aufschlag re2_art=1`, 1x ARMS11 Satz 10, kein HE-Inhalt/Licht) | ok |
| Flammen | Belegung 135/145/150 Bilder; Landung X+2 mit Zaehler 38/41/45, Zustand 1 mit 92/99/100, Gleitweg 1057..1272; Folgeflammen X+3, 100..108 Bilder, liegen still | ok (150 = Obergrenze: 2 + 45 + 1 + 100 + 2) |
| Treffer + Reaktion | Slot 2 t=10 HP 75 -> -125, st 3, +0x5 = 11 (DAT_8006f430[4]); X+1 mo 1->11; Leiche ab F173 | ok |
| Nachbrenner an RE1.5-KI (Import AN) | Brad Slot 5 (t=11, HP 250) laeuft ab F138 = X+19 durch Flamme P90/P92: HP -5 in F138, 144, 150, ..., 198 (11 Treffer, JE 6 BILDER), st 2, +0x5 = 14; F204 zurueck in st 1 mit HP 195 | Schaden 5 ok (RE2-Wert Z10 K2, bau_b N2 M1); Takt 6 statt 15 -> 3.1 |
| Op40 an liegendem Fresser | Flamme P91 (bei (-1126,-20666)) meldet JEDES Bild `treffer=2` (Slot 1 t=16, liegender Fresser, HP unveraendert 65) | -> 3.2 |

### 2.5 K0 — Positivkontrolle Handgranate 0x09 (gleiche Aufstellung wie M1, `laeufe/k0_he_re2_kontrolle`)
`EV latch`, `[licht] F119 Latch -> Cut 1 Licht2 Typ 0 Farbe (210,140,80) Lage (-746,-800,-19106) Hell 6000`, `EV kind
code=03195000`, `SE 04080001`, kein `AUFSCHLAG` — die Negativpruefungen in 2.1-2.4 haben also Zaehne.

### 2.6 N1 — Nachbrenner an RE2-KI-Zombies, `RE15_FORCE_AUFSCHLAG=1@70` (Brand ohne Resolver), ROOM1140 (`laeufe/n1_nach_re2`)
Leon `POS=-1676,-18070,1076`, kein Wurf; Q = (-1796,0,-19565) (1500 vor Leon). (Hinweis: der Haken feuert auch im
Startraum vor dem Sprung, weil der Bildzaehler dort ebenfalls 70 erreicht — der Pool wird beim Sprung geleert
(`re2fx_reset`), die Auswertung nimmt nur das Segment nach dem Sprung.)
| Pruefung | gemessen | Ergebnis |
|---|---|---|
| erster Op-40-Ruf | F90 = Aufschlag + 20 (Q.y = 0 statt 20: Landung ein Bild spaeter, Op 28 `f < y` erst bei y > 0) | ok |
| Schaden je Treffer | Slot 3 t=10: 80 -> 75 (F90) -> 70 (F105) -> 65 (F120) -> 60 (F135) -> 55 (F150); Brad Slot 4: 250 -> 245 (F91) -> 240 (F106) | ok: 5 = Z10 K2 (@0x800A41E0), Takt 15 = Sperre +0x1D3 |
| Reaktionszeile | st 2, +0x5 = 10 | ok |
| DoT nach Nachbrenner | keiner (HP bleibt 55 bis F260) | RE2-konform: K2 -> Spalte 6/7 -> HURT 0x80105438 ohne Element-Leiter (bau_b N7) |
| Spieler | HP-Verluste F140/F178/F248 = Bisse von Brad Slot 5 (ss1 3/5), gleiche Bilder wie in M1/M2 ohne Flammen | ok |

### 2.7 D1-D4 — Reaktion ueberlebender RE2-KI-Zombies: Aetzung/Verkohlung, DoT, Tod 0x0B03/0x0A03
Weg: ein echter Wurf trifft den gehenden Brad (0x11, HP 250) nicht verlaesslich (Proben p1-p4: die RE2-Brads laufen nach dem
Aufstehen um den Tisch, Auto-Nachfuehrung zielt auf den naechsten Zombie; p2 traf stattdessen Zombie 2 und 3). Deshalb der
Mess-Haken der Integration `RE15_FORCE_EXPLOSION="<art>@<bild>:<slot>"` (W6: Routine 31 am Gegner, P.y = y - 500,
FUN_80012d60(500, P, Art) + RE2-Aufschlag), Leon `POS=2745,-17055,1536` (nur Brad Slot 5 wacht auf).
| Lauf | gemessen (state.log) | Ergebnis |
|---|---|---|
| d1 Saeure an Brad F100 | 250 -> 50 (-200), st 2, +0x5 11 (HURT Taumeln bis F141), dann DoT **-1 je 8 Bilder** F142, 150, ..., 230 (ss1 2/mo 6); ab F235 Griff an Leon (ss1 3/5) -> kein DoT (nur EXEC[1]/[2]) | ok (Z_DOT_TAKT) |
| d2 Brand an Brad F100 | 250 -> 50, st 2, +0x5 10; DoT -1 je 8 Bilder F142..F166; F167 Nachbrenner-Treffer 46 -> 41 (-5, st 2, +0x5 10); DoT weiter F198..F238 und nach dem Griff wieder ab F518 (ss1 1) | ok |
| d4 Saeure an Brad F100, Leon laeuft weg (`SKRIPT=W3.4,L2.2,XU7,W20`) | DoT lueckenlos F142 .. F534 (HP 1 -> 0 lebt), **F542: HP 0 -> -1, st 3, +0x5 11 = Wort 0x0B03**; Leiche st 7 ab F603 | ok (Z_DOT_TOD Saeure) |
| d3 ROOM1030 Brand an 0x16 Slot 2 (HP 80) F60 | 80 -> 0 (-80 = Z10 K0 0x16; HP 0 lebt: st 2), HURT bis F105, **F110 DoT: 0 -> -1, st 3, +0x5 10 = Wort 0x0A03**; Leiche ab F171 | ok (Z_DOT_TOD Brand) |
| Sicht (`v_brad_art3/4`, `RE15_FORCE_CUT=1`) | Saeure: F102 Brad verdunkelt (Aetzung), weisser Blitz, F104-F108 orange Wolke am Brad, Brad geht zu Boden, steht ab ~F130 dunkel wieder auf; Brand: F102-F150 Flammen um Brad, Koerper dunkel | ok (Beleg `mess_sb_belege/brad_saeure_brand.png`) |
Bein-Wegaetzen trat in d1/d4 nicht auf (Brad steht ab ~F236 wieder als Geher ss1 1/mo 2) — BAUPLAN §1.6 "ggf. Bein"; Tor nicht
weiter vermessen.

### 2.8 R1-R5 — zweiter Raum ROOM1030 (sechs 0x16-Zombies hinter dem Gitter, Leon am Eingang, Wurf-Skript wie oben)
| Lauf | gemessen | Ergebnis |
|---|---|---|
| r1 Brand RE2-KI | A 19, S 43, L 83, X 119, 1x ARMS11 Satz 10; Explosion trifft Slot 5 (0x16, HP 75 -> -5 = -80 Z10 K0), st 3, +0x5 10, X+1 mo/ss2, Leiche F170; Nachbrenner ab **F138 = X+19** an Slot 1/4/6 gleichzeitig (-5 je Flamme = Z10 K2 0x16), Slot 3 ab F144; je Gegner **alle 15 Bilder** (F138/153/168/183 bzw. F144/159/174/189); Flammen hinter den Gitterstaeben korrekt verdeckt (`mess_sb_belege/r1_1030_brand_gitter.png`) | ok |
| r2 Saeure RE2-KI | X 119, 1x ARMS10 Satz 10, Kind-Folge X..X+4 identisch M1; Slot 5 75 -> -125 (-200 Z11), +0x5 11, Leiche F170 | ok |
| r3 Brand RE1.5-KI (Import AN) | X 119, kein Gegner im Explosionsradius (`eingriffe=0`); Nachbrenner an Slot 1 (0x16, RE1.5-KI): F201 65 -> 60, F207 60 -> 55 (**-5, Takt 6 Bilder**), +0x5 14 | Wert ok; Takt -> 3.1 |
| r4 Brand RE1.5-KI Import AUS | HP-Tabelle RE1.5 (83/97/87/91/77/91); keine Gegner in Flammen/Radius | kein Treffer (Messung M5) |
| r5 Saeure RE1.5-KI | X 119, Kind-Folge wie M1, 1x ARMS10; `eingriffe=0` | ok (kein Ziel) |

### 2.9 M5 — Brand, RE1.5-KI, **Import AUS** (`RE15_RE15_RE2Z_IMPORT=0`), ROOM1140, Aufstellung M4
| Pruefung | gemessen | Ergebnis |
|---|---|---|
| Explosion | Slot 2 t=10 HP 81 -> -919 (**-1000** = DAT_8006f418[4] @0x8006f420), st 3, +0x5 11 (@0x8006f434), Leiche F173 | ok (RE1.5 byte-true) |
| Nachbrenner an RE1.5-KI | Brad Slot 5 (RE1.5-HP 77): F138 77 -> 27 (**-50** = DAT_8006f418[5] @0x8006f422), st 2, +0x5 14 (@0x8006f435); F144 27 -> -23 (-50, st 3); Leiche F197 | Wert ok (O-VB4); Takt 6 -> 3.1 |
| Op40-Rueckgaben | 75 von 190 Rufen != 0; Flamme P91 meldet jedes Bild `treffer=2` (Slot 1 liegender Fresser, HP 83 unveraendert) | -> 3.2 |

### 2.10 F1/F2 — Sichtabnahme mit `RE15_FORCE_AUFSCHLAG="2@300,1@380"` (Saeure, dann Brand; Q 1500 vor Leon, Gier 0)
| Lauf | gemessen | Ergebnis |
|---|---|---|
| f1 ROOM1140 Tuer (-7600,-17600) | Kind-Folge F300..F304 wie M1; Flammen: Landung F383 (Q.y 0 -> ein Bild spaeter als bei y 20), Folgeflammen F384, Belegung 143/145/147, Faecher: Flamme 0 Richtung (0.998,-0.06), Flamme 1 (0.80,-0.60), Flamme 2 (0.83,0.56) = Gier +rng%40 / +400.. / -400..; erster Op40 F400 (X+20). Bild: Saeure klein hinter der Tischkante, Flammen an der Tischkante (PRI-Maske des Tischs verdeckt den Fuss, wie W3) | ok |
| f2 ROOM1030 Eingang (-8200,-22303) | Saeure an Q: F302 weisser Blitz, F304 gelb, F306 orange Kugel, F308 orange-rot, F312 dunkelrot, F316 leer; Brand: F382 Flamme + Dampf, F384-F392 Flammen gleiten nach links (Welt +x), F400-F430 Flammenfeld, F470-F524 kleiner werdend, F528 leer = Belegungsende F515/F524/F525 (`mess_sb_belege/f2_1030_saeure_brand.png`) | ok |
Farben: die Saeure-Kinder benutzen RE2-TEX.TIM-Paletten (272, 481..483) = dunkelgrau / braun / weiss-gelb-orange-rot (gelesen aus
`shared_assets/RE2/TEX.TIM`: Zeile 483 `f8f8f8 f0f0d0 f0f0a8 e0c078 d8a058 d08038 c86020 ...`); der gelb-orange "Feuerball" der
Saeure ist also RE2-Kind 0x041D1800/0x031F2000 (Palette 483), NICHT das HE-Kind 0x03195000 (RE1.5-CLUT 0x78D1) — belegt durch
das gdb-Spawn-Protokoll.

### 2.11 Weitere Einzelmessungen
| Szenario | gemessen | Ergebnis |
|---|---|---|
| gleicher Wurf wie 0x09 | gr.log-Platzzeilen (A/B/Flags/Zuender/Zaehler/wpos) F43..F125 von K0 (0x09), M1 (0x0A), M2 (0x0B) in derselben Aufstellung **byte-gleich** (83 Zeilen, `cmp` gleich) | ok |
| Wurf-Toene der Bank | Abpraller/Liegen `arms_rec=10 bank=9/10/11(geladen=1) count=11` fuer 0x09/0x0A/0x0B; Waffenbank `W0A`/`W0B` (`[equip] W-bank -> W0A (Clips 13 ...)`) | ok |
| Aufschlagton hoerbar (`t1_ton_dummy`: `SDL_AUDIODRIVER=dummy`, `RE15_SE_DEBUG=1`, ohne NOAUDIO) | F300 Saeure: `[se] Stimme: se=10 layer=0/1 vag=2 ... center=72 ... SE-Stimme -16`; F380 Brand: dasselbe mit `center=73`. Dateien selbst dekodiert: ARMS10/ARMS11.EDH Satz 10 = `00 00 33 20` -> Prog 0, Basis-Ton 3, Extra 1 (Toene 3+4), Stimme -16 (Direkt-Zweig); Ton 3/4 -> VAG-Index 3 (1-basiert; Port-Array 0-basiert 2); VAG-3-Groesse ARMS10 10992 B, ARMS11 11664 B; Center 72 (ARMS10) / 73 (ARMS11) | ok (E9) |
| Spieler-Eigentreffer 0x0A (`s1_selbst_saeure`: Tuer, Wurf TIEF, dann `U1.4` zur Granate) | Leon in 343 waagrecht von P: F119 HP 100 -> -900 (-1000 = DAT_8006f418[3]), pst 3; F120 Clip 7; pst 7 ab ~F259; exe laeuft bis EXIT_AT | ok (BAUPLAN 1.5 / K4 SPIELER_TOD); Ton per gdb (`gdb_coreSE.py`, Lauf `s3_selbst_se`): `F=120 re15_audio_core_se(3) <- re15_player_death_cmd3_tick` = Se_on(0x04030001) CORE 3 (@0x800367a8), genau einmal |
| M6 Saeure RE1.5-KI Import AUS | Slot 2 81 -> -919 (-1000 = DAT_8006f418[3] @0x8006f41e), +0x5 10 (@0x8006f433), Leiche F173 | ok |
| Spieler im Bodenfeuer (`s2_leon_in_flammen`, ROOM1030, `FORCE_AUFSCHLAG=1@300`, Leon geht bis 75 an Q) | Folgeflammen P88/P95 liegen 318..341 neben Leon und rufen Op 40 (je 2x, `treffer=0`), Hauptflammen 193 Rufe `treffer=0`; Leon HP 100 bis F460 | ok (Applier-Liste = Gegner) |
| Item-Debug 0x0A/0x0B (`i10_itemdebug`/`i11_itemdebug`: Inventar, ITEM-Raster, SELECT, 10x bzw. 11x R1, Kreis, Wurf) | `[equip] W-bank -> W0A` bzw. `W0B`, Menge 255 -> 254 nach dem Abzug, kein `[debug-menu] OPEN`; Wurf A 602/611, S = A+24, X = L+36, Resolver Art 3/4, ARMS10/ARMS11 Satz 10 | ok (BAUPLAN §2 letzte Zeile, K9) |
| Kein Haenger | 65 von 66 exe-Laeufen (inkl. 25 Raum-Scans, 5 Kamera-Proben, alle gdb-Laeufe) enden mit `[flow] EXIT_AT`, rc 0; jeder getoetete Gegner erreicht st 7. Ausnahme `scan_1091`: der Debug-Sprung "1091" laedt ROOM1090 (`[debug-menu] JUMP -> 109 REAR EXTERIOR (ROOM1090)`), `EXIT_AT=40#1091` greift deshalb nie -> eigener Timeout (Werkzeug, keine Granate) | ok |

## 3. Maengel / Abweichungen

### 3.1 [mittel] Nachbrenner an RE1.5-KI-Kandidaten: keine Sperre — Treffer alle 6 Bilder statt 15
* **Beleg**: M4 (`m4_brand_re15`, Import AN): Brad Slot 5 HP 250 -> 245 -> ... -> 195 in F138, 144, 150, ..., 198 (11 Treffer a
  -5 in 60 Bildern); R3 (`r3_1030_brand_re15`): Slot 1 F201 -> F207 (-5, -5); M5 (`m5_brand_re15_imp0`, Import AUS): Brad 77 -> 27
  (F138) -> -23 (F144) = tot nach 2 Treffern in 6 Bildern. Gegenprobe RE2-KI (N1 `n1_nach_re2`, R1 `r1_1030_brand_re2`): Treffer
  genau alle 15 Bilder (F90/105/120/135/150; F138/153/168/183) = Sperre +0x1D3 := (w1 >> 9) & 0x7F = 15 (@0x8004731c-4c).
* **Ursache (gemessen, gdb-Hardware-Watchpoint auf `hit_react` des Brad-Slots, `gdb_riegel.py`, Lauf `w1_riegel_brad` = M4;
  Struktur-Offsets per Hilfsprogramm mit `re15_actor.h`: sizeof 1880, hit_react +42, hp +40)**: je Bild schreibt zuerst
  `re15_enemy_gore_tick+0x9f` (Bit 1 loeschen, +0x93 &= 0xfd @0x80106abc) und dann der Applier-Pfad
  `op_40 -> re15_re2_gl_apply -> re15_resolver_gegnerzweig -> re15_enemy_take_damage_at` (+0x9a: |= 2 ohne Schaden bzw. +0x262:
  := 1 mit Schaden); nur in F144/150/156/162 loescht `re15_enemy_ai_live_hurt+0x4b3` Bit 0 (Ende der Port-HURT-Reaktion nach
  6 Bildern) -> genau dann geht der naechste Treffer mit Schaden durch. `re15_re2_gl_apply` (re15_damage.c:4218ff) prueft die
  RE2-Sperre nur fuer RE2-Typen (`if (re2 && e->re2z_self1d3 != 0u) continue;`, Gate 2 @0x80047138-40 — RE2 selbst
  nachgelesen: `lbu v0,467(s0)` / `bne v0,zero,0x8004740c`, 0x8004740c = naechster Listeneintrag `addiu s2,s2,4`).
* **Einordnung**: O-VB4 / bau_b N2-M1 legen Schaden (50 @0x8006f422 bzw. RE2-Wert 5 @0x800A41E0) und Reaktion (14 @0x8006f435)
  fest, aber KEINEN Takt; es gibt keine Original-Adresse fuer diesen Takt (Art 5 hat im Original keinen Aufrufer). Folge: ein
  RE1.5-KI-Zombie im Bodenfeuer nimmt 2,5-fachen RE2-Schaden (Import AN) bzw. 250 HP/s (Import AUS) und steht dauerhaft im
  Treffer-Zustand (Brad M4: st 2 von F138 bis F204).
* **Vorschlag**: Entscheidung Orchestrator/Nutzer: entweder die RE2-Sperre 15 (Z10-Record w1 `0a 1e 8f 07`, @0x800A41E4) auch fuer
  RE1.5-KI-Kandidaten im Applier fuehren (Port-Zuordnung mit Beleg) oder den 6-Bild-Takt ausdruecklich als Port-Zuordnung O-VB4
  dokumentieren und pinnen.

### 3.2 [mittel] Gesperrter RE1.5-KI-Kandidat: Flammen-Treffer JEDES Bild -> Blutfontaene + "verbrauchter" Treffer
* **Beleg (Zahl)**: M4/M5: Flamme P91 bei (-1126,-20666) meldet in JEDEM Bild ab F138 `OP40 ... treffer=2` (Slot 1 = liegender
  Fresser 0x16, RE1.5-KI, HP bleibt 65 bzw. 83 — BAUPLAN §1.6 "+0x93 = 1 in Ruhe -> nur |= 2, KEIN Schaden"); in M5 sind 75 von
  190 Op-40-Rufen solche Rueckgaben != 0. Watchpoint (3.1): der Brad wird in JEDEM Bild F139..F143, F145..F149 usw. ueber den
  Riegelzweig (+0x93 |= 2, `re15_enemy_take_damage_at+0x9a`) "getroffen", und `re15_enemy_gore_tick` (RE1.5 STAGE1
  +0x93 & 2 @0x80106a98 -> Blut-Effekt 0 mit a0 0x2000, dann &= 0xfd @0x80106abc) spawnt dafuer im Folgebild einen Blut-Effekt.
  fx.log (RE15_FX_LOG, gezeichnete Effekt-0-Teilchen je Bild): F139 27, F140 29, F141 31, F142 33, F143 35, F144 37 (+2 je Bild
  ohne Schaden), Sprung auf 64 im Schadensbild F145, dann bis F203 ~70 gleichzeitig.
* **Beleg (Bild)**: `mess_sb_belege/m4_re15ki_brad_blut_je_bild.png` (Lauf `v_m4_cut1` = M4 mit `RE15_FORCE_CUT=1`): der Brad
  steht F140..F200 hinter dem Bodenfeuer und blutet ununterbrochen am Kopf/Schulter.
* **Mechanismus**: RE1.5-Riegelzweig (Bit 0 gesetzt -> `ori v0,v1,0x2` / `j 0x80013024`, gezaehlt `addiu s4,s4,1` @0x80013024 —
  selbst disassembliert `re15_disasm.py dis 0x80012f30 70`) ist fuer EINEN Schuss gebaut (ein Blutspritzer je Treffer auf
  einen schon getroffenen Gegner). Der Op-40-Pfad ruft ihn aber je Flamme und Bild; Gate B ((+0x93 & 3) == 3 @0x80012f54-60)
  greift nie, weil der Gore-Tick Bit 1 vor dem naechsten Flammenruf wieder loescht. Zusaetzlich liefert der Applier dafuer
  `getroffen = s + 1` und bricht im Modus "erster" ab (`if (!alle) break;`): die Flamme prueft keinen weiteren Gegner (RE2
  ueberspringt einen gesperrten Gegner ungezaehlt, s. 3.1) und ruft Op 50.
* **Einordnung**: Folge der Port-Zuordnung O-VB4 (RE1.5-Gegnerzweig fuer den RE2-Applier); kein Original-Beleg fuer
  "Blut je Bild" (Art 5 hat im Original keinen Aufrufer). RE2-Flavor nicht betroffen.
* **Vorschlag**: im RE1.5-Zweig des Appliers den Riegelzweig nicht ausfuehren/zaehlen (gesperrt -> `continue` wie RE2 Gate 2)
  oder als Port-Zuordnung bewusst festschreiben; Entscheidung Orchestrator/Nutzer.

## 4. Einordnung (Port-/Original-Eigenheiten, belegt)

* **4.1 Saeure-Spritzer sinkt unter die Bodenebene** (O-VB1-Port-Zuordnung `M.rot = RotY(Gier)*B`): die Runde laeuft mit
  vel.y 240 (lokal = Laufachse = Wurfrichtung) und acc.x -23 (lokal +x = Welt-oben, B-Spalte (12,-4076,-23)) — gemessen P95:
  lok (0,240,0) -> (-23,480,0) -> (-69,720,0) -> (-138,960,0); Kinder Phase 1..4 je ~239 weiter in Wurfrichtung und 0/23/69/138
  unter Q.y (M1: y 20/43/89/158). Ein Aufschlag in Bodenhoehe (Granate liegt) schickt die Phasen-Kinder bis 138 unter den
  Boden; sichtbar nur als etwas tiefer sitzende Sprites. Kein Mangel gegen den BAUPLAN (O-VB1 ausdruecklich Port-Zuordnung).
* **4.2 Saeure sieht wie ein Feuerball aus**: RE2-Kinder mit Palette (272,483) der RE2-TEX.TIM (weiss-gelb-orange-rot) —
  gdb-Spawns belegen, dass KEIN HE-Kind 0x03195000 laeuft; CLUT-Worte der Kinder 0x78D1 (Zeile 483) / 0x7851 (481).
* **4.3 Nachbrenner-Beginn X+19 bzw. X+20**: X+19 bei der echten Granate (Q.y = 20, Op 28 landet sofort), X+20 beim Haken
  mit Q.y = Spieler-y = 0 (`f < y` erst im Folgebild).
* **4.4 Flammen-Belegung bis 150 Bilder** (Soll "~130-145"): Obergrenze aus den RE2-Konstanten = 2 (Spawn/Luft) + 45 (38 + rng%8)
  + 1 + 100 (90 + rng%11) + 2; gemessen 135..150, Folgeflammen 99..109.
* **4.5 Nachbrenner verkohlt nicht**: Treffer mit Klammer 2 -> Spalte 6/7 -> HURT 0x80105438 ohne Element-Leiter (bau_b N7);
  DoT/Verkohlung nur nach dem Explosionstreffer (Klammer 0). Gemessen: N1 Slot 3 nach 5 Nachbrenner-Treffern ohne DoT.
* **4.6 Bein-Wegaetzen** trat in d1/d4 nicht auf (BAUPLAN "ggf."); nicht weiter gesucht.
* **4.7 Effekte am Bildrand/hinter Masken**: M1/M2 (Kamera 1: unterer Rand schneidet den Effekt), F1 (Tischkante verdeckt den
  Fuss) — Kamera-/PRI-Lage, die Weltlagen (gdb `w=`) liegen an Q bzw. an der Gleitbahn.
* **4.8 Mess-Haken**: `RE15_FORCE_AUFSCHLAG` feuert auch im Startraum vor dem Debug-Sprung, wenn das Bild <= 250 ist (N1:
  zwei `AUFSCHLAG-HARNESS`-Zeilen F70) — nur Werkzeug, Pool wird beim Sprung geleert (`RESET` im gdb-Log).
* **4.9 Blockartige helle Wolke im Brand-Aufschlag** (M2 F120, F2 F382 = X+1/X+2): RE2-Kind 0x041D2700 (Bank 4 Skr. 5, Anim 32
  = 9 Zellen 16x16, Palette 483, Step-TPage |= 0x60 -> ABR 3 = B + F/4, im Port Alpha 64). Aus `shared_assets/RE2/CORE00.ESP`
  (Bank 4 @0x1BCC, Anim-Tabelle @0x1BD4, UV @0x1CFC) und `TEX.TIM` selbst gerendert: die Mittelzelle ist bis an den Rand deckend
  (259 von 540 Randtexeln der Anim 32 != 0) — die Kastenkanten stammen aus den RE2-Daten. Kein Vergleich mit laufendem RE2
  gemacht; kein Beleg fuer einen Port-Fehler.

## 5. Offen

* (erledigt, 3.1) Takt-Ursache: `re15_enemy_ai_live_hurt+0x4b3` gibt Bit 0 nach 6 Bildern frei. Offen bleibt, ob die
  Port-HURT-Dauer von 6 Bildern fuer +0x5 = 14 selbst belegt ist (E7-Rueckfall; nicht Teil dieser Messung).
* Nebenbefund ohne Bezug zur Granate, nicht geprueft: Leons Gier im state.log wird beim Drehen im Stand nicht maskiert
  (`rot=-3168`, `rot=-4800` in d4 nach `L2.2`).
