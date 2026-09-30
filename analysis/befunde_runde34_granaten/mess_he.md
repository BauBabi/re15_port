# Runde 34 (Granaten) — Messung "Handgranate 0x09 an der echten exe" (mess_he)

Stand: 2026-09-30, Zweig r34g/integration HEAD ef1c6f94, exe re15_port/build_r34_int/platform/pc/re15_pc.exe
(Messkopie re15_pc_he.exe, byte-gleich, im selben Verzeichnis; nach den Laeufen geloescht).
Rolle: NUR messen, kein Code, kein Commit (der Orchestrator committet dieses Dossier).
Ausgaben: build/r34g_mess_he/ ; Belege (PNG): analysis/befunde_runde34_granaten/mess_he_belege/

STATUS: ABGESCHLOSSEN (26 exe-Laeufe, alle rc 0 bis RE15_EXIT_AT ausser dem Langlauf `re2_tief_nah_lang`, s.u.; kein
Fremd-Kill, kein Haenger, kein Absturz). Werkzeuge zusaetzlich unter `mess_he_belege/werkzeug/` (lauf_he.sh, auswert_he.py,
fxpos.py, taumel.py; Bildbogen crop.py/sheet.py/w8_mess.py aus `integration_werkzeug/`).

## Kurzfassung

* **BAUPLAN §1.1/§1.2 an der echten exe in allen drei Zielhoehen und beiden KI-Varianten getroffen**: Clip 7/9/11 (fc 35/40/40,
  1 Clipbild je Spielbild), Spawn A+22/A+19/A+24, Flug-Saetze 23..34 im 12er-Zyklus mit EXAKT den Farben der CLUT-Zeile 492
  (0x7B11, oliv, deckend), Kontaktbilder und SE-Folge 0x010A0x01 wie die Port-h-Tabelle (MITTE 29/47/55/60/64/67/70 + L 73,
  HOCH 40..85 + L 88, TIEF 13/23/28/32/36 + L 40), Liegen Flags 63 / A 31, **X = L+36 in jedem Lauf**, Resolver genau einmal mit
  P = (x, y−500, z), Kinder X / X+5 / X+5 / X+7 (Platz frei vor Rauch #2), Feuerball X..X+12 / X+5..X+17 halbtransparent
  weiss-gelb -> rot, Rauch X+5..X+19 / X+7..X+21 abdunkelnd und steigend (vel_y −130, +5/Bild), Licht-Latch genau im Bild X
  (Farbe 210/140/80, Hell 6000, 1200 vor Leon, y −800) — Leon (und die Figuren) genau EIN Bild orange, kein Wackeln.
* **Kein Schaden im Abzugsbild** (alle Laeufe), **kein Haenger** (jede exe bis EXIT_AT; jeder Getoetete erreicht Zustand 7).
* **Gegnerreaktion (§1.6)**: RE2-KI Zombie 0x10 stehend -> −200, Zeile 9, X+1 Sturz-Tod (Clip 1), Leiche (R1 F180, R10 F360);
  Brad 0x11 stehend -> 250 -> 50, Haupt-Treffer-Phasen P0..P3, geht weiter (R7); RE1.5-KI 0x10 -> −200 (Import), Zeile 9,
  X+1 Port-Standardtod Clip 0x0b, Leiche (R2 F173); Fresser (beide KI) getroffen ohne Haenger; liegender 0x16 in beiden KI
  ohne Schaden.
* **Eigenschaden**: 947 waagrecht -> HP 100 − 1000, Modus 3, X+1 Clip 7, 113 Bilder spaeter Modus 7, YOU DIED; 1017 -> nichts.
* **Maengel/Hinweise** (§3): M-HE-1 (mittel) der Wurf-Anker haengt davon ab, ob Leon gezeichnet wird — steht er ausserhalb
  der Region des aktiven Cuts, kommt der Anker aus einer abweichenden Ersatzrechnung bzw. einem alten Zeichenstand (Original
  rechnet die Part-Matrizen auch dann, FUN_8001ef54 @0x8001e9b4/@0x8001ef80) — 519 Einheiten / 2 Bilder Abweichung gemessen; H-HE-1 BAUPLAN-§1.6-Zeile "liegender 0x16 RE2-KI -> stirbt" ueberholt (Runde-16-Regel,
  belegt richtig); H-HE-2 Leiche brennt nach HE-Tod bis zum Raumwechsel (RE2-Spreng-Russ -> RE1.5-Feuer, RE2-Anim ebenfalls
  Schleife); H-HE-3 Brad zuckt nicht (bekannter Modell-Lean-Rest); H-HE-4 Waffen-Log ohne F-Zeilen waehrend eines Griffs.
* MITTE/HOCH gegen STEHENDE Zombies ist in ROOM1140 geometrisch nicht erreichbar (Wurfweite 12858/18944 vs. Weck-Radius
  4000/3000); gemessen wurde MITTE aus 12900 gegen die Fresser (Treffer 0x11, Haupt-Treffer bzw. Port-HURT).

## 0. Soll (BAUPLAN §1.1 / §1.2 / §1.6) — Abnahmepunkte

| Nr | Punkt | Soll (BAUPLAN, Beleg dort) |
|---|---|---|
| P1 | Wurfclip | Clip = 7 + 2·(acaec>>15) + ((acaec>>11)&4): MITTE 7 / HOCH 9 / TIEF 11 (@0x800334a8-c8) |
| P2 | Abzugsbild A | Munition −1 (nur `jal 0x8004eae4`), KEIN Schaden (Hitscan-Tester fuer 9..11 tot, Resolver-GP K23/M9) |
| P3 | Spawnbild S | HOCH A+19 / MITTE A+22 / TIEF A+24 (Clipbild 0x13/0x16/0x18, @0x80033688-0x800337a8) |
| P4 | Flug-Sprite | Saetze 23..34, Satz 35 = Schleife auf 23 (12-Bilder-Zyklus), CLUT 0x7B11 oliv, deckend (CORE00.ESP @0x17E8..0x1848) |
| P5 | Abpraller + SEs | Zaehler 7 gesund (TIEF 5): je Kontakt SE 0x010A0001\|(n<<8) mit n = neuer Zaehler, Liegen 0x010A0001 (@0x8001834c-0x80018428); Zeitlinie ab S: MITTE Kontakte 29,47,55,60,64,67,70, L 73, X 109; HOCH 1. Kontakt 40, L 88, X 124; TIEF 1. Kontakt 13, 6 Kontakte, L 40, X 76 (Port-h-Tabelle §1.1) |
| P6 | Liegen | Flags 0x63, A := 31, B := 0, keine y-Korrektur (@0x80018350-84) |
| P7 | Explosion | X = L+36 (Zuender 42 -> 7, @0x8001856c-70); Flags 0x61 (Granate unsichtbar); Resolver(500, P, 2) mit P = (x, Welt-y−500, z); Kind 0x03195000 + SE 0x04080001 im Bild X; Zuender 2 (X+5) Feuerball #2 + Rauch #1 (0x030B5400); Zuender 0 (X+7) Platz frei, dann Rauch #2 (0x030B5800) |
| P8 | Sichtbarkeit Explosion | Feuerball weiss-gelb -> rot, halbtransparent (ABR 0), #1 X..X+12, #2 X+5..X+17; Rauch dunkel (ABR 2), steigt, #1 X+5..X+19, #2 X+7..X+21; kein Wackeln |
| P9 | Licht | Licht 2 der aktiven Kamera NUR im Bild X: Typ 0, Farbe max(alt, D2/8C/50), Lage Spieler + RotY·(1200,·,0), y−800, Hell 0x1770 (@0x8001ce5c-0x8001d084, Loeschung @0x8001d1b4) -> Leon GENAU ein Bild orange |
| P10 | Gegnerreaktion X+1 | RE2-KI Zombie 0x10: RE2-Modell 200, Zeile 9, DEATH[9][3] = 0x80108530 Sturz-Tod -> Leiche (I-5/bau_b K1); Brad 0x11 (HP 250) ueberlebt -> HURT[9][3] = 0x80105438 Haupt-Treffer; RE1.5-KI: Port-Standardtod (E7) |
| P11 | Kein Haenger | exe laeuft bis RE15_EXIT_AT, Getoetete erreichen Zustand 7 |
| P12 | Eigenschaden | Spieler getroffen bei waagrecht < 950 UND −3560 < P.y − Spieler-y < +500 -> HP 100 − 1000 -> Modus 3, Clip 7 + SE 0x04030001 -> Modus 7; weiter weg nichts (@0x80012e18-efc, @0x800366bc-0x80036814) |

## 1. Aufbau der Messlaeufe

* Laeufer `build/r34g_mess_he/werkzeug/lauf_he.sh <marke>` (Vorlage `integration_werkzeug/lauf.sh`): startet NUR die
  Messkopie `re15_pc_he.exe` (vor jedem Lauf `cmp` gegen `re15_pc.exe`, md5 f99f6f2c7f5e258ab9794d30b59e96b2), Arbeits-
  verzeichnis `build/r34g_mess_he/<marke>/`. Umgebung: `RE15_NOAUDIO=1 RE15_NO_INTRO=1`, Titel-Autostart,
  `RE15_WINDOW_SCALE=3` (Framedumps 960x720), `RE15_DEBUG_JUMP=1140@250`, `RE15_GIVE=9:5 RE15_EQUIP=9`,
  `RE15_INPUT_SCRIPT_BASIS=spiel` (Skript auf der Spielbild-Achse, Start 1 = erstes Bild nach dem Sprung),
  Logs `state.log` (RE15_STATE_LOG), `wf.log` (RE15_WAFFEN_LOG), `gr.log` (RE15_GRANATE_LOG), `fx.log` (RE15_FX_LOG),
  `debug.log`; `RE15_EXIT_AT=<bild>#1140`; Bilder NUR per `RE15_FRAMEDUMP` (kein AUTOSHOT/SOFTWARE_RENDER).
* Auswerter `build/r34g_mess_he/werkzeug/auswert_he.py <lauf>` (nur Lesen): A = erstes Nachsprung-Bild mit fallender
  Munition (mg), Wurfclip aus wf.log (Zeile A+1: clip/fc/frame), S/Kontakte/L/X/frei/Kinder aus gr.log (T->F ueber die
  Platzzeilen), SEs aus wf.log, Licht-Latch aus debug.log, Gegner/Spieler je Bild aus state.log, gezeichnete FX aus fx.log.
* Zielhoehe im Skript: `M` = R1 (MITTE), `MU` = R1+OBEN (HOCH), `MD` = R1+UNTEN (TIEF); `A` = Quadrat (Abzug).
* Bildnummern-Semantik (gemessen, deckt sich mit integration.md W9): die state.log-Zeile F steht HINTER dem ESP-Takt
  (Treffer/HP schon in Zeile X, Reaktion in Zeile X+1); die wf.log-Zeile F steht VOR dem Feuerpfad des Bilds.
* Ablage: je Lauf `build/r34g_mess_he/<lauf>/` mit env.txt, debug.log, state.log, wf.log, gr.log, fx.log, lauf_rc.txt,
  auswertung.txt (unversioniert, 22 MB). Die 2562 Framedump-PPMs (5,1 GB) sind nach dem Schneiden der Belege geloescht;
  die Messkopie `re15_pc_he.exe` ist geloescht (letzter `cmp` gegen `re15_pc.exe` gleich = die exe blieb waehrend aller
  Laeufe unveraendert). Kein Code geaendert, kein Commit.

## 2. Ergebnisse je Lauf

### 2.1 R1 `re2_tief_nah` — RE2-KI, TIEF, Leon per RE15_PLAYER_POS nah (Aufstellung von integration_r34_granaten)

Umgebung: `RE15_PLAYER_POS=-1676,-18070,1076`, `RE15_AI_FLAVOR=re2`, Skript ab Bild 1 `MD0.6,MDA0.2,MD2.5,W5`,
`RE15_EXIT_AT=230#1140`, Framedump 10-200/1 (x3). `lauf_rc: rc=0 dauer=18s`, debug.log
`[flow] EXIT_AT: Bild 230 in Raum 1140 erreicht -> exit`. Auswertung `build/r34g_mess_he/re2_tief_nah/auswertung.txt`.

| Punkt | Gemessen | Soll | Urteil |
|---|---|---|---|
| P2 Abzug | A = 19 (mg 5 -> 4 in state F19); HP aller 5 Gegner in F19 = F18 (79/50/80/250/250) | kein Schaden | ok |
| P1 Wurfclip | wf F20 `clip=11 fc=40 frame=1`; Clip 11 laeuft bis wf F57 (frame 38), dann Clip 12 (Zielhaltung) | TIEF = 11 | ok |
| P3 Spawn | gr.log `F=43 SPAWN granate art=2 slot=0 anker=(-2108,-772,-19007) gier=1079`; wf F43 `frame=24` -> S = A+24 | A+24 | ok |
| P4 Flug-Sprite | Saetze S..L: 23,24,...,34,23,... (Satz 23 in F43/55/67/79, Abstand 12); Flags 03 im Flug; fx.log: id4/13 Textur-Slot 51 (Seite 0x1F) | 23..34, Zyklus 12 | ok (Farbe siehe R2; das Sprite liegt hier am unteren Bildrand, sy 232..258 bei 240 Zeilen -> kaum sichtbar) |
| P5 Abpraller | Kontakte F56/66/71/75/79 (S+13/23/28/32/36) mit SE 0x010a0401/0301/0201/0101/0001, Liegen F83 (S+40) 0x010a0001 — alle `bank=1 satz=10 -> ARMS` | TIEF: 1. Kontakt 13, 6 Kontakte, L 40 | ok |
| P6 Liegen | F83 `A=31 B=0 fl=63`, wpos (-2199,20,-20548), xlat (1543,792,40) = Tabelle (1543/40) | Flags 0x63, A 31 | ok |
| P7 Explosion | X = 119 = L+36 (S+76, A+100); Tick X: `latch`, `resolver art=2 P=(-2199,-480,-20548) r=500 eingriffe=1`, `kind code=03195000`, `se code=04080001`; X+5 (F124) Kind 0x03195000 + 0x030b5400; F126 (X+7) `frei art=2`, danach `kind code=030b5800 slot=0` (Granatenplatz neu belegt) | wie P7 | ok |
| P8 Sichtbarkeit | fx.log: Feuerball #1 F119..F131 (Satz 10..22), #2 F124..F136; Rauch #1 F124..F138 (Satz 8..22), #2 F126..F140; Rauch wpos_y -480 -> -1915 (vel_y -130 +5/Bild), sy 235 -> 169 = steigt; Granaten-Sprite zuletzt F118 (Flags 0x61 ab X). Bild: F119 weiss-grauer Ball, F124 zweiter Ball mit rotem Rand, F126-F136 dunkler Rauch (Abdunkeln) — `mess_he_belege/he_r1_re2_tief_explosion_F118-142.png`. Halbtransparenz: 5938 Pixel mit Beitrag 2*out−B ~248, deren out mit dem Hintergrund mitlaeuft (G 126 bei dunklem, 147 bei hellerem Grund), 0 Pixel exakt (248,248,248) | #1 X..X+12, #2 X+5..X+17, Rauch X+5..X+19 / X+7..X+21, halbtransparent, Rauch steigt | ok |
| P9 Licht | debug.log genau EINE Zeile `[licht] F119 Latch -> Cut 1 Licht2 Typ 0 Farbe (210,140,80) Lage (-746,-800,-19106) Hell 6000`; Lage = Leon (-1599,-18264) + RotY(507)·(1200,0,0) = (-744,-19106), y -800. Figurenband (x 280..960, y 300..480) Mittel RGB F118 (25.9,23.0,22.8) -> **F119 (41.9,35.3,30.5)** -> F120 (23.0,21.5,20.9); Hintergrund (y < 300) unveraendert 47.8/46.1/36.0 — `he_r1_re2_tief_licht_F117-121.png` | genau ein Bild, vor Leon | ok (auch die Zombies werden im Bild X angestrahlt — Original: Spieler UND Figuren-Schleife @0x8001d09c/@0x8001d0e8-164 vor dem Zurueckstellen @0x8001d1ac) |
| P10 Reaktion | Zombie 3 (0x10, HP 80, stehend: st1 ss1=1 mo=0 in F117/F118) in dP 771: F119 `st=3 ss1=9 ss2=0 mo=0 hp=-120` (−200 = RE2-Modell Zeile 9 Kl. 0); F120 (X+1) `ss2=1 mo=1 af=0` (Sturz-Tod Clip 1 ab 0); F179 ss2=2; **F180 st=7** (Leiche); F181 mo=23 hp=-1 | Sturz-Tod -> Leiche | ok |
| P11 Haenger | exe bis EXIT_AT Bild 230, rc 0 | kein Haenger | ok |
| P12 Eigenschaden | Leon dP 2361 (> 950): HP 100 im Bild X | nichts | ok |

Nebenbefunde R1 (keine Granaten-Maengel, aber beobachtet):
* Zombie 5 (0x11, HP 250) greift Leon genau im Bild X (F119 `gr=1`, Leon auf (-1599,-18264) rot 507 gerissen); Biss
  HP 100 -> 70 (F140) -> 40 (F178), Griff endet F226. Normale RE2-KI der aufgeweckten Fresser (Aufstellung = Integration).
* Die Zombie-Reaktion spawnt im Bild X+1 (wf.log hinter F119, fx.log ab F120): `SPAWN id=0 sub=0 scale=0x17d0 streams=3`
  (Blut, RE2 6096 @0x8010567c) und `SPAWN id=8 sub=3 scale=0x2710` / `0x13e8` = RE2-"Spreng-Russ" FUN_8010640C
  (0x05032710 / 0x050313E8 @0x80106418-70), vom Port auf RE1.5-CORE00-Id 8 = FEUER umgesetzt
  (`enemy_ai_re2_zombie.c` re2z_gore_fx_ex: "5 -> CORE00-Id 8 (FEUER) - Klassen-Zuordnung, Sichtpruefung offen").
  Sichtbar: Flammen auf der Leiche von F120 bis Laufende F230 (fx.log id8/3, 222 Zeilen) — siehe §3.

### 2.2 R2 `re15_tief_nah` — RE1.5-KI, TIEF, Leon per RE15_PLAYER_POS nah (RE1.5-Aufstellung der Integration)

Umgebung: `RE15_PLAYER_POS=-4311,-19289,0`, `RE15_AI_FLAVOR=re15`, Skript wie R1, `RE15_EXIT_AT=400#1140`, Framedump
10-200/1. `rc=0 dauer=22s`, `[flow] EXIT_AT: Bild 400 in Raum 1140 erreicht -> exit`. Kamera Cut 0 (Tisch vorn) — hier
ist der ganze Flug im Bild.

| Punkt | Gemessen | Urteil |
|---|---|---|
| P1-P3 | A = 19, wf F20 `clip=11 fc=40 frame=1`, S = 43 = A+24 (wf F43 `frame=24`), Anker (-3392,-772,-19755) Gier 79; keine HP-Aenderung in F19 (65/75/65/250/250) | ok |
| P4 | Saetze 23..34 im 12er-Zyklus (Satz 23 in F43/55/67/79). Bild: `he_r2_re15_tief_taumel_F44-55.png` (Ausschnitt je Bild um die fx.log-Lage, x4): dunkles oliv-gruenes Granatenkoerper-Sprite mit hellem Zuenderkopf, dreht sich durch 12 Zellen, deckend. **2259 Sprite-Pixel in F44..F54 tragen EXAKT Farben der CLUT-Zeile 492** (DATA/TEX.TIM CLUT-Block x256 y480: Zeile 492 ab x272 = (136,136,104) ... (0,8,8), STP 1 = CLUT 0x7B11), 0 Pixel der Huelsen-Zeile 491; Gegenprobe Hintergrund F30 in denselben Kaesten: 0 Treffer -> Palette 0x7B11, Modulation x1.0, deckend | ok |
| P5/P6 | Kontakte F56/66/71/75/79 + Liegen F83 mit 0x010a0401..0x010a0001 + 0x010a0001, `-> ARMS` Satz 10; L: `A=31 B=0 fl=63` bei (-1856,20,-19902) | ok |
| P7 | X = 119 = L+36; Tick X: latch, `resolver art=2 P=(-1856,-480,-19902) r=500 eingriffe=1`, Kind 0x03195000, SE 0x04080001; F124 0x03195000 + 0x030b5400; F126 frei, danach 0x030b5800 | ok |
| P8 | Feuerball F119..F136, Rauch F124..F140 (wie R1); Bild `he_r2_re15_tief_explosion_F118-142.png`: F119 weiss, F122 gelb, F124 orange 2. Ball, F128-F130 rote Wolke, F132-F136 dunkler Rauch, F138ff. Leiche | ok |
| P9 | genau eine Zeile `[licht] F119 Latch -> Cut 0 Licht2 Typ 0 Farbe (210,140,80) Lage (-3121,-800,-19435) Hell 6000` = Leon (-4311,-19289) + RotY(79)·(1200,0,0); Bild `he_r2_re15_tief_licht_F117-121.png`: nur in F119 Leons Haar/Gesicht orange und der Zombie hell | ok |
| P10 | Zombie 2 (0x10, HP 75) — wach seit F6 (ss1 12 -> 13 Aufstehen mo=41 F7-F65 -> ss1 19 Gehen mo=1 ab F67, also STEHEND/gehend im Bild X) — in dP 513: F119 `st=3 ss1=9 ss2=1 mo=1 hp=-125` (−200: Import-Bruecke = RE2-Modell, E4), F120 (X+1) `ss3=1 mo=11 af=1` = Port-Standardtod Clip 0x0b (E7), **F173 st=7** Leiche | ok |
| P11/P12 | exe bis Bild 400; Leon dP 2530 -> HP 100 bleibt | ok |

### 2.3 R3-R6 MITTE / HOCH, Leon nah (Aufstellungen von R1/R2), beide KI-Varianten

Skripte `M0.6,MA0.2,M2.5,W6` (MITTE) bzw. `MU0.6,MUA0.2,MU2.5,W6` (HOCH) ab Bild 1, `RE15_EXIT_AT=260#1140`, Framedump
15-230/1. Alle vier: `rc=0`, `EXIT_AT: Bild 260` erreicht. Auswertungen `build/r34g_mess_he/{re2,re15}_{mitte,hoch}_nah/auswertung.txt`.

| Lauf | A | Wurfclip (wf A+1) | S (Clipbild) | Kontakte ab S (gr.log) + SE-Folge | L | X | P (Resolver) | Treffer | Licht |
|---|---|---|---|---|---|---|---|---|---|
| R3 re2 MITTE | 19 | `clip=7 fc=35`, bis F52, dann Clip 8 | 41 = A+22 (frame 22) | 29,47,55,60,64,67,70 mit 0x010a0601,0501,0401,0301,0201,0101,0001 + Liegen 0x010a0001 | 114 = S+73 | 150 = L+36 = S+109 = A+131 | (-1412,-491,-30996), `eingriffe=0` | keiner | `[licht] F150 ... Cut 1` (einmal) |
| R4 re2 HOCH | 19 | `clip=9 fc=40`, bis F57, dann Clip 10 | 38 = A+19 (frame 19) | 40,61,70,75,79,82,85 + Liegen, dieselbe SE-Folge | 126 = S+88 | 162 = L+36 = S+124 = A+143 | **(-564,-491,28356)** (s16-Umlauf, s.u.), `eingriffe=0` | keiner | `[licht] F162 ... Cut 1` |
| R5 re15 MITTE | 19 | `clip=7 fc=35` | 41 = A+22 | wie R3 | 114 | 150 | (8612,-491,-19500), `eingriffe=0` | keiner | `[licht] F150 ... Cut 0` |
| R6 re15 HOCH | 19 | `clip=9 fc=40` | 38 = A+19 | wie R4 | 126 | 162 | (14822,-491,-18882), `eingriffe=0` | keiner | `[licht] F162 ... Cut 0` |

* Soll-Tabelle BAUPLAN §1.1 exakt getroffen: MITTE gesund 1. Kontakt 29, 8 Kontakte, L 73, X 109, A+131; HOCH 1. Kontakt 40,
  L 88, X 124, A+143; Weg bis L lokal xlat MITTE (11929, 2483, 1752), HOCH (18760, 3180, 1848) = Tabelle 11929/1752 und
  18760/1848. Satz 23 alle 12 Bilder (MITTE F41/53/.../113, HOCH F38/50/.../122), Flags 03 im Flug, 63 ab L, 61 ab X.
  Kinder X/X+5/X+5/X+7 und `frei` X+7 wie R1.
* Kein Gegner steht in 12000..19000 vor Leon — die Explosion liegt ausserhalb des Raums (keine Wandkollision = Original,
  BAUPLAN §1.1), der Resolver trifft niemanden, Feuerball/Rauch werden nicht gezeichnet (Region-Cull des aktiven Cuts,
  @0x80053314-3c; fx.log: Granaten-Sprite nur F41..F65 bzw. F38..F57 gezeichnet). Leon wird trotzdem im Bild X angestrahlt
  — das Licht sitzt vor Leon, nicht an der Granate (BAUPLAN §1.2).
* **s16-Umlauf der Weltlage (Original-Eigenheit, gemessen):** R4 wirft aus (-1676,-18070) Blick 1079 HOCH in −z; Anker
  (-825,-3171,-18333) + ~18700 in −z ergaeben z ≈ −37000, `wpos` ist `int16_t` (re15_esp.h:211) -> gr.log F126
  `wpos=(-564,9,28356)` bei `xlat=(18760,3180,1848)`: die Granate "liegt" auf der anderen Raumseite (z +28356), dort
  explodiert sie. BAUPLAN §1.1: "Weltlage +0x28/+0x2a/+0x2c ist s16 (`sh`/`lh`)" — das Original speichert ebenso mit `sh`,
  also derselbe Umlauf. Kein Mangel; festgehalten, weil ein HOCH-Wurf nahe der Raumgrenze z −32768 damit an einer
  unerwarteten Stelle explodiert.
* In R3/R4 (RE2-KI) greift Zombie 5 (0x11) Leon wieder ab F119 (wie R1); Bisse HP 100 -> 70 -> 40 -> 20. Waehrend des
  Griffs schreibt wf.log keine F-Zeilen (Feuerpfad uebersprungen) — die SE-Zeilen hinter "F119" gehoeren zu spaeteren
  Bildern; die Kontaktbilder oben stammen deshalb aus gr.log (T->F), nicht aus wf.log.
* Wurfbilder (Cut 0, R5/R6): `he_r5_re15_mitte_wurf.png` (MITTE: Seitwurf nach vorn-unten, Granate verlaesst die Hand
  F41/F42) und `he_r6_re15_hoch_wurf.png` (HOCH: Ueberkopfwurf, Granate ueber dem Kopf F38/F39).

### 2.4 R7 `re2_brad_tief` — RE2-KI, TIEF gegen Brad (0x11, HP 250, ueberlebt)

Aufstellung (gemessen, Erkundungslauf `erkund_ost` ohne Wurf): Leon `RE15_PLAYER_POS=3000,-20600,2048` — nur die zwei 0x11
(Slots 4/5, Abstand 2963 < 4000) wachen auf, die 0x10 (4884) fressen weiter; Skript `W1.0,MD0.6,MDA0.2,MD2.5,W5`
(Abzug spaeter, damit Slot 4 zur Explosion an der Liegestelle steht). Die Auto-Nachfuehrung beim Heben dreht Leon
2048 -> 1825. ROOM1140 ist gross (RVD Cut 0: x −10500..3000, Cut 1: x −7000..16500); der Sprung setzt Cut 0 und Leon quert
keine Wechselzone — Leon/Brad liegen ausserhalb der Cut-0-Region. Fuer die Bilder deshalb ein zweiter, sonst gleicher Lauf
`re2_brad_tief_cut1` mit dem Anzeige-Haken `RE15_FORCE_CUT=1`: **gr.log byte-gleich (`cmp`), state.log bis auf `cam=`
gleich, Auswertung identisch** — der Haken aendert nur die Kamera (und damit das Cut-Licht 2, s. Licht-Zeile).

| Punkt | Gemessen | Urteil |
|---|---|---|
| P1-P3 | A = 49, wf F50 `clip=11 fc=40`, S = 73 = A+24, Gier 1825 | ok |
| P5-P7 | Kontakte S+13/23/28/32/36 + Liegen S+40 (F113) mit 0x010a0401..0001 + 0001; X = 149 = L+36 = A+100; `resolver art=2 P=(528,-480,-21149) r=500 eingriffe=1`; Kinder X/X+5/X+5/X+7, frei F156 | ok |
| P9 | `[licht] F149 Latch -> Cut 0 ...` (bzw. `Cut 1` mit Haken), Lage (1869,-800,-21001) = Leon + RotY(1825)·1200 | ok |
| P10 Brad | Slot 4 (0x11) stehend/gehend (st1 ss1=1 mo=0) in dP 664: F149 `st=2 ss1=9 ss2=0 hp=50` (250 − 200 = RE2-Modell Zeile 9, lebt -> +0x04 := 2); F150 (X+1) ss2=1, F154 ss2=2, F171 ss2=3, F172 st1 ss1=2 -> F173 ss1=1 (geht weiter). Das ist der Phasenweg des Haupt-Treffers 0x80105438 (P0 -> P1 -> P2 -> P3 Erholung, `re2z_hit_main`); Clip bleibt der Geh-Clip (P0 zwingt `+0x14C` auf den Walk-Clip @0x801054BC-C8) | ok (HURT[9][3], I-5) |
| P11/P12 | exe bis Bild 260; Leon dP 2532, HP 100 im Bild X (Zombie 5 greift ab F153, Biss F174) | ok |

Bild `mess_he_belege/he_r7_re2_brad_F147-174.png` (Cut 1): F149 Brad und Leon (hinten) angestrahlt, Brad geht durch
den Feuerball weiter, kein sichtbares Zucken. Das fehlende Oberkoerper-Zucken ist ein BEKANNTER, benannter Port-Rest
(`enemy_ai_re2_zombie.c` Kopf von `re2z_hit_main`: "⛔ NICHT portiert (OPEN, reine Presentation): der Modell-Lean ...
@0x801057A4-E8 / @0x801058D4-960"), gilt fuer alle Waffen, kein Granaten-Mangel -> §3 Hinweis.

### 2.5 R8/R9 Eigenschaden — Leon laeuft nach dem Wurf auf die Granate zu (Tuer-Sprungpunkt, alle Zombies > 4000)

Ohne RE15_PLAYER_POS (Tuer-Sprungpunkt (-7600,-17600), die Fresser 6122..8748 entfernt, keiner wacht), RE2-KI,
`RE15_EXIT_AT=260#1140` bzw. 200. TIEF-Wurf, danach geradeaus gehen (`U`); die Granate liegt vor Leon, er geht ueber sie
hinweg und steht im Bild X knapp dahinter.

| Lauf | Skript | Leon im Bild X | dP (waagrecht) | P.y − Leon.y | Resolver | Leon |
|---|---|---|---|---|---|---|
| R8 `eigen_tuer_lauf` | `MD0.6,MDA0.2,MD1.3,U2.1,W3` | (-4380,-18704) | **947** | −480 | `eingriffe=1` | F119 `hp=-900 pst=3` (100 − 1000), F120 (X+1) `mo=7`, F233 `pst=7` (= X+1 + 113 Bilder Clip 7, PL00.EDD), danach Weissblende + YOU DIED |
| R9 `eigen_tuer_lauf_knapp` | `MD0.6,MDA0.2,MD1.25,U2.1,W3` (Gehen 1 Bild frueher) | (-4310,-18728) | **1017** | −480 | `eingriffe=0` | HP 100, pst 1 — nichts |

* Soll BAUPLAN §1.5: Spieler r 450 + 500 = 950, `sqrt(dx²+dz²) < R` streng, Band −3560 < P.y − y < +500 (@0x8002b6fc-7ac,
  @0x80012e18-efc). 947 -> Tod, 1017 -> nichts: die Grenze liegt dazwischen, wie gefordert. Beide Laeufe sonst identisch
  (A 19, S 43 Gier 215, L 83, X 119, P (-5327,-480,-18709)).
* Tod-Kette R8: Modus 3 im Bild X (state `pst=3`), Clip 7 ab X+1 (`mo=7`), Modus 7 nach 113 Bildern — BAUPLAN §1.2
  Zeile X+1 ("Modus 3 -> Clip 7 (PL00-Basisbank) + SE 0x04030001 -> spaeter Modus 7"). Bild
  `mess_he_belege/he_r8_eigenschaden_tod_F117-168.png`: F119 Leon orange (Latch), F121-F130 Todesclip-Beginn, F135-F168
  Zusammenbruch hinter dem Stuhl.
* SE 0x04030001: NICHT messbar in diesen Laeufen — `re15_audio_core_se(3)` (game_step_common.c:222, im selben Ph0-Block,
  der `mo=7` setzt) kehrt bei `RE15_NOAUDIO=1` sofort zurueck und schreibt keine Messzeile (nur Waffen-/ESP-SEs landen im
  Waffen-Log). Indirekt: der Ph0-Block lief (mo=7 in F120).
* Waehrend des Todes-Clips zeichnet der Port Feuerball/Rauch normal weiter (F120-F140), kein Haenger, exe bis EXIT_AT.

### 2.6 Laufweg-Varianten (Leon geht selbst in Stellung, Zombies wachen im Lauf auf)

| Lauf | Umgebung / Skript | Ergebnis |
|---|---|---|
| R10 `re2_tief_lauf2` (RE2) | Start `RE15_PLAYER_POS=-8500,-21600,0` (Suedgang, alle Zombies > 4000), `W0.2,U1.3,W4.5,MD0.6,MDA0.2,MD2.5,W5`: Leon GEHT 3300 nach Osten bis (-5207,-21600) — Zombie 3 (0x10) wacht bei d 3779 (F40) auf, steht auf, laeuft; Abzug so gelegt, dass er zur Explosion an der Liegestelle steht (Wegdaten aus dem Erkundungslauf `erkund_sued_re2`, gleiche Eingaben bis zum Heben) | A 199, S 223 = A+24, Kontakte S+13/23/28/32/36 + Liegen S+40 (F263), **X 299 = L+36**, `resolver ... P=(-2761,-480,-20964) eingriffe=1`; Zombie 3 **gehend** (st1 ss1=1 mo=0) in dP 422: F299 `st=3 ss1=9 hp=-120`, F300 (X+1) `ss2=1 mo=1` (Sturz-Tod), **F360 st=7** Leiche; Licht `[licht] F299 ... Cut 0` genau einmal; exe bis Bild 400. Bild `he_r10_re2_laufweg_F262-380.png` |
| `re2_tief_lauf` (RE2) | Tuer-Sprungpunkt, Auto-Nachfuehrung auf Zombie 2, 2350 gehen, sofort werfen | Explosion X 182 regelrecht (L+36), **kein Treffer**: Zombie 2 geht nach dem Aufstehen einen Umweg nach Sueden (Tisch dazwischen, (-1800,-19600) -> (-1839,-20799) im Bild X), dP 1818. KI-Weg, kein Granatenfehler |
| R11 `re15_tief_lauf` / `_lauf2` (RE1.5) | Tuer-Sprungpunkt, 3040..3470 gehen, werfen | X 190 / 191 = L+36; Zombie 2 in dP 733 / 667 getroffen, aber noch **FRESSEND** (st1 ss1=12 mo 39, das RE1.5-Weck-Tor ist d < 3000: "decide FUN_801048a8 flips +0x5=0xc -> 0xd when dist<3000", enemy_ai_common.c; Leon stand bei 3166 / 3101): HP 75 -> -125, Zeile 9, X+1 `mo=11` Port-Standardtod, Leiche F242 / F243 — auch aus der Fress-Pose kein Haenger |
| `re15_tief_lauf3` (RE1.5) | 3290 gehen (d 2952 < 3000) | Zombie 2 wacht (ss1 13 Aufstehen F74-F132), geht dann aber mit ss1=2 mo=4 nach OSTEN weg — dP 1982, kein Treffer (KI-Weg) |

Die Laufweg-Laeufe zeigen dieselbe Zeitlinie wie die Teleport-Laeufe (Clip 11, A+24, L+36, 6 Toene, Kinder X/X+5/X+7,
Licht genau im Bild X) — der Teleport verdeckt nichts. Stehende Treffer im Laufweg: R10 (RE2); die RE1.5-Zombies wandern
nach dem Aufstehen ab (ss1=2) oder wachen erst unter 3000 auf, der RE1.5-Laufweg traf deshalb nur Fresser.

### 2.7 MITTE aus der Ferne gegen die FRESSER (Raum ist 27000 breit: RVD Cut 0..9, x −10500..16500, z −26000..1500)

MITTE fliegt 12858 vor / 1351 seitlich (aus R3 zurueckgerechnet), HOCH 18944 / 2715 — ein STEHENDER Zombie ist fuer
MITTE/HOCH in ROOM1140 nicht erreichbar: die Fresser wachen erst unter 4000 (RE2) bzw. 3000 (RE1.5) auf und laufen dann auf
Leon zu (RE2 ~20..30 je Bild, Aufstehen ~70 Bilder); bei X = A+131/143 muesste der Zombie zum Abzug ~16000..22000 entfernt
UND wach sein. Gemessen wurde deshalb MITTE von Norden auf die noch fressende Gruppe (Leon `-3151,-6742,1024`, Skript
`M0.6,MA0.2,M2.5,W6`):

| Lauf | Anker (gr.log) | L / X | P | Treffer |
|---|---|---|---|---|
| `re2_mitte_fern` (Cut 0 bleibt, Leon unsichtbar) | (-3397,**-2356**,-8171) | **112 / 148** (S+71 / S+107) | (-434,-491,-19445) | Slot 5 (0x11, fressend) HP 250 -> 50 |
| `re2_mitte_fern_cut5` (gleich + `RE15_FORCE_CUT=5`, Leon sichtbar) | (-3446,**-2474**,-8301) | **114 / 150** (S+73 / S+109 = Tabelle) | (-392,-491,-19962) | Slot 5 (0x11, fressend, mo 18) dP 693: HP 250 -> 50, st2 ss1=9, X+1 ss2=1 **mo 18 -> 2** (Haupt-Treffer zwingt den Geh-Clip @0x801054BC-C8, der Fresser steht damit sofort im Gehen), Erholung F173 ss1=1 -> geht auf Leon zu; `eingriffe=2`: der liegende 0x16 (Slot 1, g=0x88, EXEC[7], dP 757) wird mitgezaehlt, **bekommt aber keinen Schaden** (HP 79 bleibt, st1 ss1=7 unveraendert bis F280) |
| `re15_mitte_fern_cut5` (RE1.5-KI, Leon sichtbar) | (-3446,-2474,-8301) | 114 / 150 | (-392,-491,-19962) | Slot 5 (0x11, fressend ss1=12) HP 250 -> 50 (Import = RE2-Modell), st2 ss1=9, X+1 mo 40 -> 4, F156 ss1=7, F167 ss1=2 (Port-HURT, E7), kein Haenger; liegender 0x16 (HP 65) mitgezaehlt, kein Schaden |

* Der liegende 0x16 ohne Schaden in BEIDEN KI-Varianten: RE1.5-KI = BAUPLAN §1.6 ("+0x93 = 1 in Ruhe -> nur |= 2, KEIN
  Schaden (16/16 Saves)"). RE2-KI: BAUPLAN §1.6 sagt "Port-RE2-Bruecke macht die Spawn-Pose treffbar
  (enemy_ai_re2_zombie.c:8809-8815) -> stirbt". Gemessen NICHT: `re15_re2z_hit_filter_apply` nimmt den PASSIVEN Liegenden
  (grid 0x87/0x88 = Nibble 7/8 + Bit 0x80) ausdruecklich von der Spawn-Pose-Ausnahme aus (Runde 16, Kommentar dort mit
  RE1.5 @0x80103AAC-AB8 `ori v0,v0,0x1 / sb 147` jeden Tick und RE2 @0x80103804-14 `+0x1D3 |= 0x80`: in beiden Originalen
  unschiessbar); die Ausnahme gilt nur fuer die Fresser 0x86/0x06. Das Verhalten ist damit belegt richtig, der
  BAUPLAN-Text ist ueberholt -> §3 (wie I-5).
* **Anker haengt am Renderer** (Befund, §3): gleiche Eingaben, einziger Unterschied die angezeigte Kamera — ohne
  `RE15_FORCE_CUT` steht Leon (nach dem Kollisions-Herausschieben im Bild 6 von (-3151,-6742) auf (-3151,-7332)) ausserhalb
  der Region von Cut 0 und wird NICHT gezeichnet (`RE15_VIS_TRACE`: `vis=0` in allen 311 Bildern); der Wurf-Anker ist dann
  118 tiefer (y −2356 statt −2474) und 139 naeher an Leon (874 statt 1013 waagrecht), Liegen und Explosion kommen 2 Bilder frueher, die Explosion liegt **519** Einheiten daneben. `re2_hoch_fern` (HOCH, Leon von (-4515,-656) auf
  (-6218,-718) geschoben, ebenfalls unsichtbar): Anker y **-2884** statt -3171, L S+89 / X S+125 statt 88 / 124.

### 2.8 Einzelpruefungen §1.1 (Abbruch, Drehen, Abprall-Arithmetik) und "kein Wackeln"

* **R1 los vor dem Spawn** (`abbruch_r1`, `MD0.6,MDA0.2,W4`): Abzug F19 (mg 5 -> 4), R1 ab F25 (Clipbild 6) los; Clip 11
  laeuft bis Clipbild 10 (wf F29), im naechsten Bild (Clipbild waere 11 > 10) Clip 6 `ph=4` (Senken); gr.log leer = KEIN
  Spawn, Munition weg — BAUPLAN §1.1 "R1 los UND Clipbild > 10 (Byte2 0x0a) -> 0x800aca5a := 3, KEIN Spawn (Munition schon
  weg)" @0x80033604-50: ok.
* **Drehen im Wurf** (`drehen_r`, RECHTS F25-F39 gehalten): rot 215 -> 239 -> ... -> 575, **+24 je Bild** (15 Bilder = 360),
  danach steht rot; Spawn `gier=575` = Spieler-Gier im Spawnbild — §1.1 "Pad links/rechts -> Gier um 24 je Bild
  (@0x800740b8 Byte1 0x30, `srl 1`)" und "Gier = Spieler +0x6a -> Platz +0x2e": ok.
* **Abprall-Arithmetik** (R1, gr.log T=301/302): vor dem Kontakt xlat (962,780,13), vel (67,130,1); Weltlage im Bild F56
  = −772 + 780 = **8 > 0** -> vx −= trunc(67/3) = 22 -> 45, vy := −trunc(130/3) = −43, xlat_y −= 8 -> 772, Zaehler 5 -> 4,
  SE 0x010a0401; dann Physik xlat += vel -> (1007,729,14), vel += acc (−1,10,0) -> (44,−33,1) = gr.log F56
  `xlat=(1007,729,14) vel=(44,-33,1)`, exakt §1.1 (@0x8001834c-0x80018428, "xlat += vel, DANACH vel += acc"). vz bleibt
  ungedaempft (TIEF 1, MITTE 24, HOCH 21 bis L).
* **Kein Bildschirmwackeln** (R2, Cut 0): Wandausschnitt (600,0)-(960,250) in F116..F141 gegen F118 pixelgleich (0 geaenderte
  Pixel, auch F119/F121/F123/F125). **Kein Rumble**: `re15_rumble_small/large/ramp` werden nur in enemy_ai_boss_g5.c
  gerufen (grep), nicht im Granatenpfad (Code-Stichprobe; eine Laufzeitmessung ohne Pad ist nicht moeglich).

## 3. Maengel / Einordnung

### 3.1 Mangel M-HE-1 (mittel): Wurf-Anker (Teil 11) haengt davon ab, ob Leon gezeichnet wird

* **Gemessen** (§2.7): `re2_mitte_fern` vs `re2_mitte_fern_cut5` — identische Eingaben, nur die angezeigte Kamera
  verschieden. Leon ausserhalb der Region des aktiven Cuts (nicht gezeichnet): Anker (-3397,**-2356**,-8171), L S+71, X S+107,
  P (-434,-19445). Leon gezeichnet: Anker (-3446,**-2474**,-8301), L S+73, X S+109 (= BAUPLAN §1.1), P (-392,-19962).
  Unterschied 519 Einheiten Liegestelle, 2 Bilder Zuendung. HOCH (`re2_hoch_fern`, unsichtbar): Anker y −2884 statt −3171,
  L S+89 / X S+125 statt 88 / 124.
* **Original** (selbst disassembliert, info/Re1.5/PSX.EXE): der Spielerzeichner FUN_8001e8c8 (`jal` @0x8001d09c) prueft die
  Region (`jal 0x80014368` @0x8001e974, `beq v0,zero,0x8001e9c0` @0x8001e97c) — AUSSERHALB laeuft die Schleife
  @0x8001e9b4-c8 `jal 0x8001ef54` je Part weiter, und FUN_8001ef54 rechnet die Welt-Matrix JEDES Parts trotzdem:
  `lw a0,108(s0)` @0x8001ef6c, `addiu a1,s0,24` @0x8001ef7c, `jal 0x80022da0` @0x8001ef80, `addu a2,s1,zero` (s1 = Part+64)
  @0x8001ef84 — derselbe Aufruf wie im Zeichenzweig FUN_8001e9ec @0x8001ea0c-28. Part+0x40 von Part 11 = Posenpuffer + 0x7a4
  = der Wurf-Anker (§1.1). Im Original ist der Anker also unabhaengig davon, ob Leon gezeichnet wird.
* **Port**: `re15_player_gunbone_world` (re15_damage.c:1148-1170) nimmt `s_hand_world`/`s_hand_rot`, die NUR der Zeichner
  fuettert (main.c:8869-8873, innerhalb `if (player_visible && skel_ok)` main.c:8569); `s_hand_valid` wird nie
  zurueckgesetzt. Solange noch nie gezeichnet wurde, springt die engine-seitige Ersatzrechnung `muzzle_bone_world`
  (re15_damage.c:1309ff.: aktueller Waffenclip + `pl->anim_frame`, OHNE Crossfade `g_anim_pose_actor = NULL`) ein; nach dem
  ersten gezeichneten Bild bleibt bei unsichtbarem Leon der letzte gezeichnete Stand stehen. Beide Wege weichen vom
  gezeichneten Fall ab. **Gemessen** (`re2_mitte_fern_vis`, gleicher Lauf + `RE15_VIS_TRACE=1`): Leon ist in ALLEN 311
  Bildern `vis=0` (ROOM1240 Cut 0/1 und ROOM1140 Cut 0, `inrgn=0`) -> der Anker (-3397,-2356,-8171) stammt hier aus der
  Ersatzrechnung (byte-gleich zu `re2_mitte_fern`). Gleicher Weg fuer Muendungsfeuer/Rauch/Huelse (game_step_common.c:
  1838/1918-1928) und den Messer-Trefferpunkt (re15_damage.c:1943).
* **Reichweite**: im normalen Spiel steht Leon praktisch immer in der Region des aktiven Cuts (die RVD-Wechselzonen
  folgen ihm); erreicht wurde der Zustand hier ueber RE15_PLAYER_POS + Kollisions-Herausschieben (Cut 0 blieb, Leon bei
  z −7332 ausserhalb der Cut-0-Region z ≤ −8500). Betroffen sind damit v.a. Mess-Aufstellungen (I-1: eine kuenftige
  Aufstellung ausserhalb der Region misst still einen falschen Anker) und Randfaelle.
* **Vorschlag**: die Spielerpose (mindestens Knochen 11) wie FUN_8001ef54 auch ohne Zeichnen rechnen und fuettern — mit
  DERSELBEN Rechnung wie im Zeichenzweig (Crossfade, Bild), damit sichtbar/unsichtbar denselben Anker ergibt; eine
  getrennte Ersatzrechnung (heute `muzzle_bone_world`) reicht nicht, sie liefert gemessen einen anderen Anker.

### 3.2 Hinweis H-HE-1: BAUPLAN §1.6 Zeile "Liegender Fresser 0x16 ROOM1140 | RE2-KI | -> stirbt" ist ueberholt

Gemessen (§2.7, RE2-KI): der liegende 0x16 (Slot 1, grid 0x88, EXEC[7]) im Explosionsradius wird gezaehlt
(`eingriffe=2`; getroffen mit Schaden ist nur Slot 5 in dP 693, der einzige weitere Kandidat in Reichweite ist Slot 1 in
dP 757, der naechste — Slot 2 — liegt bei 1453; der Resolver zaehlt auch den Riegel-Fall "Bit 0 gesetzt -> |= 2, kein
Schaden", re15_damage.c `re15_resolver_gegnerzweig` @0x80013024), bekommt aber KEINEN Schaden und keine Reaktion (HP 79, st1 ss1=7 unveraendert bis F280). Das ist die
Runde-16-Regel in `re15_re2z_hit_filter_apply` (passiver Liegender Nibble 7/8 + Bit 0x80 ausgenommen; RE1.5 @0x80103AAC-AB8,
RE2 @0x80103804-14 / Gate (2) @0x80047138-40: in beiden Originalen unschiessbar) und deckt sich mit der RE1.5-KI-Zeile
("+0x93 = 1 in Ruhe -> nur |= 2, KEIN Schaden") — Verhalten belegt richtig, nur der Plantext ist falsch (wie I-5).
Vorschlag: §1.6-Zeile auf "kein Schaden (passiver Liegender, Runde 16)" berichtigen.

### 3.3 Hinweis H-HE-2: Leiche brennt nach HE-Tod (RE2-KI) bis zum Raumwechsel

Gemessen: nach dem Sturz-Tod spawnt die RE2-Zombie-Reaktion `SPAWN id=8 sub=3 scale=0x2710` und `0x13e8` (RE2 0x05032710 /
0x050313E8, "Spreng-Russ" FUN_8010640C @0x80106418-70 laut Port-Kommentar); der Port setzt RE2-Id 5 auf RE1.5-CORE00-Id 8
= FEUER um (`re2z_gore_fx_ex`, Kommentar "Klassen-Zuordnung, Sichtpruefung offen"). Die Flammen brennen auf der Leiche von
X+1 bis zum Raumwechsel (`re2_tief_nah_lang`: F120..F728, 608 Bilder, 2 Plaetze je Bild; R10 F300..F400 = Laufende).
(`re2_tief_nah_lang` = R1 mit `RE15_EXIT_AT=1000#1140`, endete mit rc 124 = Zeitlimit des Laeufers: Leon wird von Zombie 5
totgebissen — HP 0 ab F300, Modus 7 ab ~F500 —, das Spiel geht ueber YOU DIED in ein neues Spiel (debug.log: Boot ROOM1240,
dann ROOM1170) und erreicht Raum 1140 nie wieder; debug.log laeuft bis F6510 weiter = kein Haenger. Ausgewertet ist nur der
erste 1140-Abschnitt.)
RE2-Daten dazu: CORE00.ESP Bank 5 Anim-Satz 10 = `00 01 ff 28` (Dauer 0xFF = Schleife auf Satz 0), Skript 3 = Op A 2 (Init)
-> Op A 0 / Op B 25; Op 25 = 0x8001fa08 (RE2-Optab @0x8009d8cc, selbst disassembliert) toetet den Platz nur, wenn
`jal 0x800527b4` @0x8001fa20 einen Wert liefert und `slt v0,a0,v0` @0x8001fa48 greift (`sb zero,0(v1)` @0x8001fa54 =
Wasser-/Hoehentest) — ein Zeitende hat die RE2-Flamme damit ebenfalls nicht, sie laeuft in der Anim-Schleife weiter.
Kein Granaten-Mangel; offen bleibt nur die schon benannte Sichtpruefung der Klassen-Zuordnung RE2 Bank 5 -> RE1.5 Feuer.
Bilder: `he_r1_re2_tief_explosion_F118-142.png` (F136-F142), `he_r10_re2_laufweg_F262-380.png`.

### 3.4 Hinweis H-HE-3: Brad (0x11) zuckt beim Haupt-Treffer nicht

Zustands-/Zeitweg des Haupt-Treffers 0x80105438 ist gemessen richtig (§2.4); sichtbar geht Brad ohne Oberkoerper-Zucken durch
den Feuerball. Bekannter, im Code benannter Port-Rest (`re2z_hit_main`: "Modell-Lean ... @0x801057A4-E8 / @0x801058D4-960
NICHT portiert"), gilt fuer alle Waffen.

### 3.5 Hinweis H-HE-4 (Mess-Werkzeug): Waffen-Log ohne Bildzeilen waehrend eines Griffs

Waehrend Leon gegriffen wird (state `gr=1`), schreibt der Feuerpfad keine `F<n>`-Zeilen ins Waffen-Log (R1: F119 -> F227);
dazwischen gemeldete `SE esp code=...`-Zeilen haengen dann scheinbar am letzten F-Bild (R3/R4: Abpraller "F119").
Fuer Zeitlinien gr.log (T -> F ueber die Platzzeilen) verwenden. Kein Spielfehler.

### 3.6 Nicht messbar

* SE 0x04030001 beim Eigenschaden-Tod: `re15_audio_core_se(3)` kehrt bei RE15_NOAUDIO=1 ohne Messzeile zurueck
  (game_step_common.c:222); indirekt belegt ueber `mo=7` aus demselben Ph0-Block.
* Rumble zur Laufzeit (kein Pad); Code-Stichprobe: kein Aufruf im Granatenpfad.
