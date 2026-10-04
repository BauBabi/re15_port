# Runde 35 — Spur B "werfer": UNABHAENGIGE ABNAHME 0

Stand: Zweig `r35/werfer`, HEAD c5f1484f (Code-Stand f391f1c6, danach nur Dossier), Baum sauber.
Datum 2026-10-03. Pruefer: Abnahme-Agent 0 (kein Code geaendert, nur diese Datei).
Massstab: Wortlaut AUFTRAG.md Zeilen 13/14, VERTRAG.md, CLAUDE.md (RE-Gate).

> 1. Einige Waffen, wie die Granatwerfer oder der Raketenwerfer gehen noch nicht
> 2. Andere Waffen wie der Flammenwerfer oder die Colt Python gehen noch nicht richtig.

## 0. Ergebnis

| Punkt | Urteil |
|---|---|
| 1 Granatwerfer (15/16/17) + Raketenwerfer (18) | **teilweise** |
| 2 Flammenwerfer (14) + Colt Python (20) | **teilweise** (Python erfuellt; Flammenstrahl teilt Mangel M1) |
| Gate Suite | haelt (eigener Lauf 481/481) |
| Gate @0x (RE-Gate) | **verletzt** (M3: mindestens sieben falsch zitierte Adressen der Magazin-Konstanten) |
| Gate Pfade/Vertrag | haelt (Anmerkung A1) |
| Gate Tests | haelt (Anmerkung A2: Luecken) |

**bestanden = NEIN.** Die Waffen feuern, fliegen, explodieren, treffen, laden nach und klingen — das
ist am gebauten Stand gemessen und gilt auch fuer Elza (vom Bau-Agenten nicht gemessen, hier nachgeholt).
Es bleiben vier Maengel (M1-M4), davon zwei am Spielverhalten.

## 1. Bau und Suite

* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `... build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (die exe vom 16:35:25 ist der
  Stand der Quellen; `git diff f391f1c6 HEAD --stat` = nur B_werfer.md).
* `ctest --test-dir re15_port/build -R r35_werfer` (PowerShell): `unit_r35_werfer` Passed,
  `unit_r35_werfer_kombi` Passed, `integration_r35_werfer` Passed 136.93 s — `100% tests passed, 0 tests
  failed out of 3`.
* Volle Suite SELBST gefahren (`local_build.sh test`, parallel zu eigenen Messlaeufen):
  `100% tests passed, 0 tests failed out of 481`, `Total Test time (real) = 1393.41 sec`,
  `=== LOCAL-BUILD-OK (test) — Tests 481/481`. Kein Fenster-Haken rot.
* Nebenbefund Dossier: Protokollzeile "17:20 Suite Lauf 4 gruen" (B_werfer.md Zeile 37) passt nicht zu den
  Dateien — `re15_port/build/mess_r35b_logs/suite_lauf4.log` endet 16:59:25, der Abschluss-Commit traegt
  17:00:46. Das Ergebnis (481/481) stimmt, die Uhrzeit ist erfunden/verschrieben.

## 2. Messprotokoll (eigene Laeufe)

exe-Kopie `re15_port/build/platform/pc/re15_abnB0.exe` (nie taskkill, eigener Name). Laeufe und Skripte
unter dem Sitzungs-Scratchpad `.../scratchpad/abnB0/` (`run_real.sh`, `run_gen.sh`, `run_elza.sh`,
`ana.py`, `sca.py`, `aud.py`, Sonden `probe/*.c`); die tragenden Zeilen stehen hier woertlich.
Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=<raum>@250 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG RE15_WAFFEN_LOG RE15_WPN_DBG=1
RE15_EXIT_AT=<bild>#<raum>`, OHNE `RE15_AI_FLAVOR` (= Vorgabe des Nutzers), Bilder ueber `RE15_FRAMEDUMP`.

### 2.1 Echter Weg: Statusschirm -> Item-Debug (SELECT + N x R1) -> schliessen -> zielen -> feuern
Skript (Start 260): `S0.1,W2,A0.1,W1,E0.1,W0.3,(M0.1,W0.2) x N,X0.1,W1,S0.1,W3,M0.6,MA1.5,M1.0,W4`, ROOM1000,
Leon (21850,-13400) Blick -x, N = Waffen-Id. Alle sechs Laeufe exit 0.

| N | debug.log | wf.log |
|---|---|---|
| 15 | `[equip] W-bank -> W0F (Clips 11, Rueckstoss-Clip7 fc=36)` | `RE2SPAWN a0=01002000` + 5 x `a0=020c0a00 a1=2048 ofs=(-204,1717,3) ... lauf=(-4063,-353,9)`; `SE re2fx code=0x01000001`; 5 x `0x01110001 -> ARMS0F Satz 10`, u.a. `@(16657,-2083,-13029)` (Wand) |
| 16 | W0F | `020c1000 a1=2048`, `03081200`, `0x01130001 -> ARMS10 Satz 10 @(17071,-2094,-13030)`, `arms_rec=0 bank=16` |
| 17 | W0F | `020c1000`, `0x01120001 -> ARMS11 Satz 10 @(17071,-2094,-13030)`, `arms_rec=0 bank=17` |
| 18 | `W-bank -> W12` | `020d1000 a1=2048 ofs=(0,1100,0)`, `030a1a00`, `030a1500 ofs=(300,-900,0)`, `0x01140001 -> RE2 ARMS11 Satz 20 @(16882,-2728,-12963)` |
| 14 | `W-bank -> W0E` | 15 x `031d1200 a1=2048 ofs=(150,1200,0) ... lauf=(-4076,-4,-190)`, `re2arms ARMS10 satz=0` (3x) / `satz=11` (2x) |
| 20 | `W-bank -> W14` | `SPAWN id=2 sub=0 scale=0xe00 streams=2`, `SPAWN id=3 sub=0 scale=0x1000`, `SE arms_rec=0 bank=20`, keine Huelse |

Bilder (RE15_FRAMEDUMP, Montagen `real_w15_mont.png`, `real_w18_mont.png`, `real_w14_mont.png`,
`real_w20_mont.png`): Werfer waagerecht in Leons Haenden, Rauchspur, fuenf Feuerbaelle an Boden/Spindwand
(15); Rakete von der Schulter, Rueckstrahl- und Flugrauch, ein Feuerball an der Spindwand (18);
durchgehender waagerechter Flammenstrahl (14); Muendungsblitz + Rauch (20).
(Ein erster Versuch in ROOM1140 mit RE15_PLAYER_POS neben den Zombies war ein HARNESS-Fehler von mir:
Leon wurde vor dem Skript gepackt, der Statusschirm ging nie auf — nicht gewertet.)

### 2.2 Schaden (ROOM1140, Leon (-1676,-18070) Blick 1076, RE15_GIVE=<id>:6, Vorgabe-KI)
state.log, Platz 2 (Typ 0x10, HP 50) / Platz 3 (HP 80):
15: `F21 hp-150 ss1=9`, P3 `F21 30` -> `F57 -170`, P4 250 -> 200 -> 150, P5 250 -> 200. 16: `F21 -150 ss1=11`, P3 `F56 -120`.
17: `F21 -150 ss1=10`. 18: `F26 -850 ss1=17`, P3 `F60 -820`. 14: `F21 35, F26 20, F31 5, F36 -10 (ss1=16)`,
P3 80 -> -10 in sechs Schritten zu 15. 20: `F19 -850 ss1=5`, P3 `F44 -820`.
RE1.5-KI (`RE15_AI_FLAVOR=re15`): 18 `F26 75 -> -825 ss1=18`; 15 `F26 75 -> -125 ss1=15`, P3/P4/P5 je -50.
Mittlere Distanz (Leon (-5000,-19600) Blick +x, 3200 vor Platz 2): 18 `F27` P2 -850 und P3 -820; 15/16 `F28` -150/-120.
Gegenprobe andere Ids (gleiches Skript): 3, 5, 7, 8, 12, 13, 19 — alle verlieren Munition und der Zombie HP
(keine Regression der Nachbarwaffen).

### 2.3 Elza (PL04) — vom Bau-Agenten als "nicht gemessen" gemeldet, hier gemessen
Start: `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1` (statt RE15_TITLE_SHOT, das
`re15_gameflow_new_game(0)` = Leon ruft) + `RE15_DEBUG_JUMP=1000@250`; debug.log `[pl] Spieler-Familie PL04
(character=4, Elza-Bit=1)`. Alle sechs Waffen exit 0:
* 15: `W-bank -> W0F (Clips 14, Rueckstoss-Clip7 fc=39)`, `RE2SPAWN a0=020c0a00 a1=2048 ofs=(120,1200,0) ...
  lauf=(-4079,7,5)`; gerade Runde (20646,-1899) -> (16464,-1683), 597 je Bild; Explosion `@(16663,-1703,-13147)`.
* 16/17: `020c1000 ofs=(120,1200,0)`, Toene 0x01130001 / 0x01120001. 18: W12, vier Spawns, `0x01140001`.
* 14: W0E, 20 x `031d1200`. 20: W14, Redhawk-FX, `arms_rec=3 bank=20` (Nachladen).
* Nachladen 15 (1 + 8 Runden): `mg 1 -> 0 -> 6 -> 5 -> 4`, `SE arms_rec=3 bank=15`; Python ebenso `1 -> 0 -> 6 -> 5 -> 4`.
* Bild `elza_mont.png`: Elza haelt Werfer/Rakete/Flammenwerfer/Python, Effekte wie bei Leon.
Elza ist damit in Ordnung (OFFEN 7 des Dossiers ist durch diese Messung erledigt).

### 2.4 Inventar-Kombination AN DER EXE (Dossier: nur Unit-Test)
`RE15_GIVE=15:2,25:10,26:7,20:1,23:9` -> Plaetze 3..7; Menue per Skript (Raster, COMBN, zweiter Cursor):
* K1 EXPLOSIVE RND auf GL: state.log `mg 2 -> 6` (Bild 461), danach Schuesse 5, 4 (5 x `020c0a00` je Schuss).
* K2 ACID ROUNDS auf GL Explosiv: `mg 2 -> 7`, danach `RE2SPAWN a0=020c1000`, `arms_rec=0 bank=16`,
  `0x01130001 -> ARMS10 Satz 10`; Bild `kombi_mont.png` (kombi2 F500): Werfer 7 in GELB, Runden-Zelle
  "Explosive Rounds" 2.
* K3 MAGNUM auf Python: `mg 1 -> 6`, Schuesse 5, 4; Bild: Python 6, MAGNUM 4.

### 2.5 Ton (gemessen am Mischer, nicht an der Log-Zeile)
`RE15_AUDIO_CAP_SYNC=cap.raw` (1470 Stereo-Abtastungen je Spielbild), Spitzenpegel je Bild (Betrag / 256)
gegen einen Leerlauf ohne Schuss (gleiche BGM, dort im Schussfenster 12..46, danach 4..31):
15 Schuss 47/58/65/53, Explosionen 124..128 = Vollaussteuerung ueber ~8 Bilder (fuenf ueberlagerte
Explosions-Saetze); 16 Schuss wie 15, Aufschlag 63..89; 18 Schuss/Explosion 86..122; 14 Strahl dauerhaft
25..59, wo der Leerlauf auf 4..31 faellt; 20 Schuss 72/71/77/63.
Leerdruck (`RE15_GIVE=<id>:1`, zweiter Abzug, wf.log beide `F76 SE arms_rec=1`): GL (ARMS0F Satz 1 `00005216`)
hoerbar (Bild 78: +16 gegen den Leerlauf), **Raketenwerfer ohne jede Abweichung** -> Mangel M2.

### 2.6 Freie Bahn, andere Raeume, Elevation
* Wand ROOM1000 unabhaengig geprueft (eigener SCA-Leser `sca.py`): Strahl von (21850,-13400) nach -x trifft Zelle 4
  `{x 14350..16350, z -16850..1650, Typ 1}` bei x 16330; Raketen-Explosion nach Rueckprall bei x 16882, GL bei 16657.
* ROOM1060 (Band 8, y -14400): GL Explosiv fliegt 8 Bilder, Explosionen (20207,-16479), (22625,-14565) usw.;
  Saeure `0x01130001 @(20582,-16503,25671)`; Flammen-Plaetze laufen.
* ROOM1140 Fernwand: Rakete `@(4165,-3056,-22211)` (Zelle 5) bzw. `@(7866,-3122,-20167)` (Zelle 11).
* Rakete hoch/tief gezielt: Flugbahn identisch zur Waagerechten (wie im Dossier beschrieben).

## 3. Urteil je Punkt

### Punkt 1 — Granatwerfer / Raketenwerfer: TEILWEISE
Erfuellt (gemessen 2.1-2.6): Ausruesten ueber den echten Weg, Feuerclip, RE2-Geschosse, waagerechte Flugbahn
(G1), Wandkontakt (G2), Explosionsbild, Ton, Schaden an stehenden/aufgewachten Zombies beider KI-Flavors,
Munition, Nachladen per Abzug und per Inventar, Munitionswechsel, Elza.
Nicht erfuellt: M1 (Moebel-Zellen), M2 (Leerschuss-Ton Rakete), M4 (Wirkung auf fressende Zombies unbelegt).

### Punkt 2 — Flammenwerfer / Colt Python: TEILWEISE
Colt Python: erfuellt (Blitz, Rauch, Knall ARMS14, 900 Schaden = Magnum-Klasse, Nachladen Abzug + Inventar,
Leon und Elza). Die Zuordnung "zweiter Magnum-Revolver" ist als PORT-WAHL gekennzeichnet; ihr Beleg stimmt
(selbst gelesen: ARMS14.EDH Saetze `00001330 00003310 00004210 00005310 00006310 00006311 00007311 00009314`
= Layout ARMS07, nicht ARMS03 `00001336 00003300 00004201`).
Flammenwerfer: Strahl, Takt, Fuel 2 je 8 Bilder, Toene, Schaden 15 je 5 Bilder, Elza — erfuellt; aber der
Strahl (Op 70) laeuft durch dieselbe Moebel-Sperre wie M1 (gemessen am Tisch ROOM1140) -> deshalb nur teilweise.

## 4. Maengel (nummeriert, nachpruefbar)

**M1 — Geschosse und Flammenstrahl enden an MOEBEL-Zellen; ueber den Konferenztisch von ROOM1140 hinweg ist
kein Werfer-Schuss moeglich, Hitscan-Waffen schon.**
Messung (ROOM1140, Leon (200,-10300) Blick 1024 = -z, vor ihm der Tisch = SCA-Zelle 6
`{x -4650..7450, z -17450..-11350, Typ 1, u0 0xff, Band 0}`; Laufprobe `U3`: Leon steht ab Bild 19 bei z -10882,
die Zelle ist also Spieler-Kollision):
* 18: `SE re2fx code=0x01140001 -> RE2 ARMS11 Satz 20 @(-174,-2530,-10668)` im Startbild — 368 vor und 374
  neben Leon, 2530 ueber dem Boden; kein Flugbild (0 RE2FLUG-Zeilen); alle fuenf Zombies HP unveraendert. 600 weiter hinten
  (z -9700): ein Flugbild, Explosion `@(-185,-2563,-10835)`.
* 15: fuenf Explosionen `@(-100,-2146,-11757)`, `(-232,-2145,-11757)`, `(-32,-2003,-11570)`,
  `(-297,-2001,-11570)`, `(-163,-1838,-11317)` = an der Tischkante in 1840..2150 Hoehe; kein Schaden.
* 14: alle Strahl-Plaetze bleiben in z -11349..-11569 (220 Einheiten an der Kante).
* Gegenprobe 20 (gleicher Standort): `F25` Platz 5 (200,-19600) HP 250 -> -650 — der Schuss geht ueber
  denselben Tisch 9300 weit.
Ursache: `werfer_boden`/`wandzelle_im_band` (re2_fx.c, Runde-35-Block) sperrt jede Typ-1-Zelle des
Schuetzen-Bandes in jeder Hoehe; RE2 traegt die Formhoehe (`-1800 * ((+10 >> 6) & 0x1f)` @0x8004fe08-30).
Der Bau-Agent fuehrt das als OFFEN 9 — es ist aber am Hauptmoebel des eigenen Testraums wirksam und macht
G2 (Wand) nur um den Preis einer neuen Abweichung richtig. Erwartet: Hoehe je Zelle belegen (Quader-Verfahren
oder RE2-Formhoehen) oder ein belegtes Kriterium, das Tisch/Bank von Wand trennt; Test: Schuss ueber Zelle 6
ROOM1140 und ueber die Baenke ROOM1000 (Zellen 6/7) erreicht das Ziel.

**M2 — Leerschuss des Raketenwerfers ist STUMM; das Dossier nennt ihn "Klick".**
Dossier §4.2 ("jeder weitere Druck `SE arms_rec=1` (Klick)") und OFFEN 11 ("der Port spielt den RE1.5-Klick
0x01010001 = ARMS12 Satz 1"). Gelesen: `shared_assets/PSX/.../ARMS12.EDH` Satz 1 (Dateibytes 4..7) =
`ff ff ff ff` — die Bank hat nur Satz 0 `00001320` und Satz 10 `00003320` (so steht es auch in §2.1 des
Dossiers). Gemessen (2.5, `RE15_GIVE=18:1`, zweiter Abzug): wf.log `SE arms_rec=1 bank=18(geladen=1)`,
Audio-Mitschnitt im selben Bildfenster ohne jede Abweichung vom Leerlauf; beim GL (ARMS0F Satz 1 `00005216`)
+16. Die Log-Zeile ist ein AUFRUF, kein Ton. Erwartet: RE2-Leerzweig disassemblieren (ARMS11 Satz 1
`00005416` existiert) und den Ton liefern, oder den Befund ehrlich als "stumm" fuehren.

**M3 — RE-Gate: die Magazin-Konstanten zitieren Adressen, an denen sie nicht stehen.**
Waffen-Records `0x80074da8 + id*12` (selbst gelesen, `re15_disasm.py bytes 0x80074da8 180` und
`bytes 0x80074e50 84`): [15] `06 00 00 00 88 4c 07 80 03 00` @**0x80074e5c**, [16] @0x80074e68, [17] @0x80074e74,
[18] `04 ..` @0x80074e80, [19] `64 ..` @0x80074e8c, [20] `06 ..` @0x80074e98, [7] `06 00 00 00 9c 4c 07 80`
@0x80074dfc. Zitiert wird stattdessen:
* inventory_common.c:193-194 "[15] 6 @0x80074dd4, [16] 6 @0x80074de0, [17] 6 @0x80074dec, [18] 4 @0x80074df8,
  [19] 100 @0x80074e04, [20] 6 @0x80074e10" — an 0x80074dd4 stehen `03 01 00 00` (Byte +8 des Pistolen-Records 3);
* re15_werfer.h:7 "Munitions-Zeiger NULL (@0x80074dd4..e85)", :30 "Magazin 6 @0x80074dd4/e0/ec", "18 = 4 Schuss
  ... (@0x80074e68" (das ist Record 16);
* game_step_common.c:1714 "@0x80074dd4-e10"; werfer_r35.c:311 "Magazin 6 (@0x80074dd4)";
* test_r35_werfer.c:123, test_r35_werfer_kombi.c:9/125; Dossier §3.5 Punkt 1 ("@0x80074df8" fuer w7) und Punkt 9.
Die WERTE (6/6/6/4/100/6) sind richtig; die Belege sind es nicht (Memory "zitierte Adresse ist kein Beleg").
Erwartet: alle Stellen auf 0x80074e5c/e68/e74/e80/e8c/e98 (bzw. 0x80074dfc) berichtigen.

**M4 — Wirkung der Werfer/Flamme auf FRESSENDE Zombies ist nicht gemessen und nicht belegt; vom Tuerpunkt
ROOM1140 aus richten Granatwerfer und Rakete nichts aus.**
Messung (ROOM1140, Zombies bleiben liegen, `ss 8/1`): Leon am Sprungpunkt der Tuer (-7600,-17600), Auto-Zielen
dreht auf Blick 215: Rakete fliegt (-2086,-2772,-19970) -> (-1364,-2805,-20228) an Platz 2 (-1800,-19600)
vorbei (431 seitlich) und explodiert an der Fernwand `@(4165,-3056,-22211)`; GL Explosiv: fuenf Explosionen
`@(-922,-2032,-20288)`, `(-2414,-179,-20814)`, `(-1334,91,-18972)`, ... zwischen den Zombies — HP aller fuenf
unveraendert. Dasselbe aus 4500 und 6100 in Achsrichtung ((-6300,-19600) / (-7900,-19600) Blick +x). Die Python
toetet vom selben Tuerpunkt Platz 2 (`F25 hp -850`). Ursache gemessen: `RE15_BEFUND_MARKE` -> befund.log
`typ=0x10 st=1 ss= 8/1 ... 1D3=80` fuer alle fuenf; der Applier ueberspringt +0x1D3 != 0 (RE2 Gate 2
`lbu v0,467(s0)` @0x80047138 / `bne v0,zero,0x8004740c` @0x80047140 — selbst disassembliert, stimmt). Eigene
Sonde (`probe_band.c` gegen libre15_engine.a): ein stehender RE2-Zombie wird in jeder Raketenhoehe 0..-3002
und auf Bodenhoehe getroffen (laengs -800..1250, seitlich +-1300) — an Kasten/Band liegt es nicht.
Nicht aufgeklaert: aus 3200 Abstand werden dieselben liegenden Zombies bei Bild 27/28 getroffen (2.2), ein
zweiter Schuss bei Bild ~97 fliegt an den liegenden Plaetzen 4/5 (376 seitlich) wieder wirkungslos vorbei —
das Bit steht offenbar erst einige Bilder nach dem Raumeintritt.
Das Gate ist RE2-treu; ob der fressende RE1.5-/RE2-Zombie das Bit 0x80 in RE2 wirklich traegt, steht in
Spur B nirgends, und das Dossier misst "Treffer" nur an Zombies, die Leon vorher geweckt hat (1535 Abstand).
Fuer den Nutzer sieht es so aus: Tuer auf, Granatwerfer auf die Fresser — fuenf Feuerbaelle, kein Schaden.
Erwartet: Fall messen und ins Dossier; die 0x80-Schreiber der Fresser-Zustaende (EMZ0, im Port
enemy_ai_re2_zombie.c z.B. @0x80101e78-88) gegen RE2 belegen ODER als OFFEN mit Messweg fuehren.

## 5. Gates

### 5.1 RE-Gate (@0x)
Stichproben SELBST disassembliert/gelesen, alle zutreffend:
1. RE2 Tabelle @0x800A6FDC: [9] 0x80044b44, [10] 0x80044f44, [11] 0x80045090, [16] 0x800454a0, [17] 0x80045588.
2. RE2 Raketen-Handler @0x80045588: `lbu v1,333 / addiu v0,zero,1 / bne`, `lui 0x100 / ori 0x2000`, `addiu 1100`,
   `lui 0x20d / ori 0x1000` + `lh a1,118(s1)`, `0x30a/0x1a00`, `0x30a/0x1500` mit 300/-900.
3. RE2 Op-Tabelle @0x8009D868 [7/15/17/22/23/24/47/59/70]; Op 24 @0x8001f6e0: Box 0x80010908, `slti 32001`
   @0x8001f724/738, `lui a3,0x3 / ori 0x11` @0x8001f7b0/c0, `jal 0x8004fba0(.,2,8192,0)` @0x8001f810, 0x20011
   @0x8001f820-3c; Boxen @0x80010900 = {-1400,0,350,250},{-800,0,400,200},{-600,0,300,150},{-2000,0,1000,500}.
4. RE2 GL-Handler @0x80044b44: Tabelle @0x80011030 `00 00 88 ff 10 ff fa 00 c8 00 96 00`, Versatz 120/1200,
   `addiu -10` @0x80044c18, `addiu 600` @0x80044c40, `addiu -20` @0x80044c98.
5. RE2 Fuel @0x8006a184-21c (`slti v0,v0,8`, zwei Abzuege); RE2-Records @0x800A4258/5C/6C/70 =
   0x00F03C0F/0x02850A0A/0x384E1384/0x078F1E0A.
6. RE1.5 Dispatch @0x80074030 [15..18] 0x80032e9c, [20] 0; Entlade @0x80074100 [15..18] 0, [7] 0x800339a4;
   Redhawk-Handler @0x800339a4 (0x02000E00 {0x8c,0x25d,0}, 0x03001000 {0x91,0x1f4,-25}, jal 0x80011f50, jal
   0x8004eae4); Rueckstoss-Saetze @0x80074090 (15..18/20 = 0); Nachlade-Gate `sltiu v0,v0,0x9` @0x80033368;
   DAT_8006f418 = [10,20,1000,1000,1000,50,100,200,300,1000,0], DAT_8006f430 = 03 03 09 0a 0b 0e 0f 10 11 12 14;
   Munitions-Saetze @0x80074cb4 `19 0f 02 00 | 1a 10 04 0c | 1b 11 04 0c`.
7. PL00W0F.PLW Punkte @0x518C..0x522C wie zitiert; nachgerechnet Winkel 35,62 Grad, sin 2386 / cos 3330,
   Versatz (-203,8 / 1716,5 / 3) — der Code nimmt 1717 (Rundung um 0,5, unerheblich).
8. shared_assets/RE2/SOUND/ARMS10/ARMS11 (.EDH/.VB) bytegleich zu info/re2leon/COMMON/SOUND.
Verletzt: M3. Keine Rate-Marker im Code-Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO: 0
Treffer), kein neuer getenv. PORT-WAHLEN sind gekennzeichnet (§3.5); "vermutlich" steht nur in OFFEN 1/2.
Erklaert der Fix den Befund? G1 ja (Netzbytes -> Winkel -> waagerechter Flug gemessen). G2 ja fuer die Wand,
mit der Nebenwirkung M1.

### 5.2 Vertrag / Pfade
`git diff master --name-only`: nur engine/src, include, platform/pc, shared_assets/RE2/SOUND (neu, erlaubt),
tests/unit/probes/r35_werfer.cmake + eigene Tests, vier nachgezogene Alt-Pins, Dossier. Kein release/,
platform/android/, shared_assets/PSX/, keine CMakeLists der Tests. Keine Bank-9-Bits, Nachrichten-IDs,
AOT-Slots, Ereignisse.
A1 (Anmerkung): game_step_common.c traegt +55/-11 Zeilen an fuenf Stellen (Haken einzeln klein, mit
Kommentaren ueber der 1-5-Zeilen-Regel); re2_fx.c, re15_damage.c und enemy_ai_re2_zombie.c sind Dateien, die
auch die Spuren A/D anfassen — Konfliktrisiko beim Zusammenfuehren.

### 5.3 Tests
Vorhanden und messend fuer beide Punkte (unit 78 + 52 Pruefungen, integration 9 exe-Laeufe).
A2 (Luecken): kein Test schiesst ueber eine Moebel-Zelle (M1), keiner auf liegende Fresser (M4), keiner prueft
einen Ton am Sample (M2 — die Tests zaehlen Log-Zeilen), Elza nur per Unit-Test (2.3 zeigt: laeuft).

## 6. Sonstige Feststellungen (nicht gewertet)
* GL Explosiv: fuenf gleichzeitige `0x01110001` treiben den Mischer in die Vollaussteuerung (2.5) — ob RE2
  fuenf Stimmen oder eine neu gestartete spielt, ist nicht belegt.
* OFFEN 12 (Vorpruefung "Waffe schon voll"), 14 (Selbstschaden), 1/2 (Latch 0x800DF349, FUN_80043d30)
  bleiben wie vom Bau-Agenten genannt offen; nicht nachgemessen.
