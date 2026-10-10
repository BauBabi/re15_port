# Runde 36 — Spur D "granate": UNABHAENGIGE ABNAHME 0

Stand: 2026-10-10 (Beginn 17:29), Baum `.claude/worktrees/r36_granate`, Zweig `r36/granate`, HEAD 44a3eaf6
(Basis master 78a4a748). Abnehmer: unabhaengige Abnahme-Instanz (glaubt nur, was hier selbst gemessen ist).
Bericht wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz).

## 0. Grundlage
* Wortlaut (AUFTRAG.md Z. 25 / Z. 51):
  1. "Bei normalen Granaten brauche ich keinen Feuer Effekt bei den Zombies, sondern nur das Zerplatzen der
     Körperteile. [Feuer] würde ich nur bei den incendiary Granaten erwarten."
  2. "Manchmal selten löst die Granate bei der Verwendung nicht aus/wird nicht geworfen."
* Commits master..HEAD (13 + 2 Merges): 384116f7 c0abd7e7 e289dd0e 62ea41a7 2cb21596 dd88956b 10b417f1 cc628051
  815bb243 41d1b0f7 fa669d3c(M) d9627833(M) e93f75b7 90d559cc 44a3eaf6. Arbeitsbaum sauber (`git status --short` leer).
* Diff (git diff master --stat): 11 Dateien, +1292/-3 — engine: granate_r36.c (neu), enemy_ai_re2_zombie.c (+5),
  player_common.c (+4), re15_esp.c (+11), include: re15_granate_r36.h (neu), re15_esp.h (+2); Tests:
  probes/r36_granate.cmake, test_r36_granate.c, integration/test_r36_granate.cmake (neu), probe_r34_wurf.c (PRUEF 109
  umgestellt); Dossier D_granate.md.

## 1. Bau
* `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`.
* `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `LOCAL-BUILD-OK (build)`.
  re15_pc.exe 2026-10-10 09:56:54 (libre15_engine.a 09:56:53) ist juenger als jede Engine-Quelle des Diffs
  (letzte: include/re15_granate_r36.h 09:56:18); die Merges/Commits danach beruehren nur Doku/Tests
  (`git log --stat`) -> die exe ist der Engine-Stand von HEAD.

(Fortsetzung folgt — Abschnitte 2 ff. werden je gemessenem Punkt angehaengt.)

## 2. Gezielte Tests (selbst gefahren, 17:30-17:32)
* `ctest -R "^(unit_r36_granate|unit_r34_wurf)$" --timeout 600 -V` -> 2/2 Passed. test_r36_granate: [1] 103-108 ok
  (HE: "Flammen max 0, Gate 1, Handler 5, Teile 3, Tinte[0]=0x404040" in 4/4; Brand: "Flammen max 3" in 4/4),
  [2] 201-208 ok (96 Wuerfe bei 96 Abzuegen; 208: "READY Bild 11, 3 Abzuege (11,45,79) / 3 Spawns (33,67,101), Menge 2"),
  [3] 300-304 ok. probe_r34_wurf: "ALLE PRUEFUNGEN GRUEN" (PRUEF 109 neu: Spawn 22 statt keiner).
* `ctest -R "^integration_r36_granate$" --timeout 1200 -V` -> Passed 76.7 s:
  `[he]: ok — X=119, 2 Zombies zerrissen, Flammen nach X: 0 (vor X: 0)`, `[brand]: ok — X=119, Eingriffe 3, Flammen
  nach X: 1899`, `[serie]: ok — 8 Abzuege = 8 Wuerfe = 8 Explosionen, Granaten 30 -> 22`.

## 3. RE-Gate — selbst disassembliert (Stichproben, richtige Binaerdatei)
* **RE2 EMOVL10_S0.BIN @0x8010640c** (`re2_disasm.py dis 0x8010640c 70 --bin EMOVL10_S0.BIN`, sha1 2d8885fc gleich
  Hauptbaum): `80106418 lui a0,0x503 / 8010641c ori a0,a0,0x2710 / 80106424 addiu v0,zero,200 / 8010642c-34 sh zero,16 /
  sh v0,18 / sh zero,20 / 80106440 jal 0x8001bf10 / 80106444 addiu a2,v0,72` (Flamme 0x05032710 Part 0, v=(0,200,0));
  `80106450 lw v0,516(a2) / 80106458 andi v0,v0,0x1 / 8010645c beq v0,zero,0x80106478 / 80106460-64 lui/ori a0=0x050313e8 /
  8010646c addiu a2,a2,588 / 80106470 jal 0x8001bf10` (Flamme Part 3); `80106478..801064f8` = die Russ-Tinten
  0x404040/0x484848/0x707070/0x505050 (`sw a0,112(v0)` @0x8010648c ...). **Stimmt mit dem Zitat ueberein.**
* **Aufrufer** (dieselbe Datei): `80108cc0 lbu v1,5(s3) / 80108cc8 beq v1,9 / 80108cd0 bne v1,17 / 80108cd8 lhu v0,270(s3) /
  80108ce0 andi 0x80 / 80108ce4 bne / 80108cec jal 0x8010640c`; `80108490 beq v1,9 / 80108498 bne v1,17 / 801084a0-ac lhu 270 /
  andi 0x80 / bne / 801084b4 jal 0x8010640c`; `801051e8 bne v1,9 / 801051f0-fc lhu 270 / andi 0x80 / bne / 80105204 jal
  0x8010640c`. **Stimmt.** Im Port laufen alle fuenf Aufrufstellen von `re2z_gore_soot` (enemy_ai_re2_zombie.c:4883,
  :5824, :7513, :7589, :7640, selbst gegrept) durch das Gate IN der Funktion; `re2z_gore_burn` (FUN_80106128, Zeile 10/16)
  ist unberuehrt.
* **RE1.5 PSX.EXE @0x80033600** (`re15_disasm.py dis 0x800335f0 70`, sha1 2637b76d gleich Hauptbaum): `80033600 andi
  v0,a0,0x100 / 80033604 bne v0,zero,0x80033658 / 80033610 lbu 0x800aca5d / 80033618 lbu 0x800acae9 / 8003362c addiu
  at,at,16530 (0x80074092) / 80033634 lbu v0,0(at) / 8003363c sltu v0,v0,a0 / 80033640 beq / 80033644 ori v0,zero,0x3 /
  8003364c sh v0,0x800aca5a / 80033650 j 0x800337ac` (= RETURN vor dem Spawn); `80033688 ori v0,zero,0x9 / 8003368c bne
  v1,v0,0x800337ac / 80033690 ori v0,zero,0x13 / 800336a0 bne v1,v0 / 800336a4 ori v0,zero,0x16 / 800336b4 andi 0x8000 /
  800336ec jal 0x80019700` (HOCH 19), `80033714 andi 0x4000 / 80033748 jal 0x80019700` (MITTE 22), `80033758 ori
  v0,zero,0x18 / 80033770 andi 0x2000 / 800337a4 jal 0x80019700` (TIEF 24). **Stimmt.**
* **Byte 2 @0x800740ba** (`re15_disasm.py bytes 0x80074092 60`): Satz Waffe 9 @0x800740b8 = `18 30 0a 01 00` -> 0x0a = 10;
  Waffe 3 @0x8007409c = 0x07. **Stimmt.**
* **Abzug im Rueckstossbild 0** (zentral fuer "Granate weg"): `800334cc-e0 lui at,0x8007 / addiu at,at,16640 (0x80074100) /
  addu at,at,v1 / lw v0,0(at) / jalr v0` nur im ersten Bild (`lbu 0x800aca5b / bne` @0x80033470-78); Tabelle @0x80074100
  [9] = 0x80033b38 (`re15_disasm.py table 0x80074100 16`) -> `80033b40 jal 0x8004eae4`; 0x8004eae4 = Mengen-Dekrement
  (`addiu v0,v0,-1` @0x8004eb1c/@0x8004eb48, Platz 0x800b10ad+4*i). **Stimmt** — das Original verliert die Granate, wenn
  R1 zwischen Cursor 11 und dem Spawnbild losgelassen wird.
* Konstanten im Diff: 19/22/24 (@0x80033690/@0x800336a4/@0x80033758), Ids 9/10/11 (`id == 9` @0x8003368c + Port-Zuordnung
  E1), 0x05032710/0x050313E8 (@0x80106418-1c/@0x80106460-64), Blut-Id 0 + kleinster Index = **PORT-WAHL** (gekennzeichnet),
  Schwelle = Spawnbild = **NUTZER-VORGABE/PORT-WAHL** (gekennzeichnet). Wortsuche im Diff (deferred|tunable|interim|for
  now|faithful|plausibel|TODO): kein Treffer im Code-Diff (s. Abschnitt 6). Kein Env-Schalter.

## 4. Punkt 1 — "keinen Feuer-Effekt bei den Zombies, nur das Zerplatzen; Feuer nur bei incendiary"
Messkopie `re15_port/build/platform/pc/re15_pc_abn0d.exe` (sha1 e18c61ef = re15_pc.exe HEAD). Gegenprobe = die ausgelieferte
**v0.8.23** (`release/re15_port_v0.8.23_win64.zip`, in den Scratchpad entpackt, eigene shared_assets). Treiber: Env +
exe im eigenen Arbeitsordner (RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT, Default-KI = RE2, KEIN
RE15_AI_FLAVOR). Eigene Aufstellungen, NICHT die des Bau-Agenten:
`RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=<P> RE15_GIVE=9:5 RE15_EQUIP=9 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=MD0.6,MDA0.2,MD2.5,W5 RE15_EXIT_AT=300#1140 RE15_GRANATE_LOG RE15_FX_LOG
RE15_STATE_LOG`, P1 = (-1000,-17000,1024), P2 = (-2400,-17200,900).

| Lauf | exe | Explosion (gr.log) | gezeichnete Flammen `id=8` (fx.log) | Blut 0/0 | Brocken 7/2 |
|---|---|---|---|---|---|
| P1 HE | HEAD | `T=365 EV resolver art=2 P=(-2365,-480,-20050) eingriffe=3` | **0** | 1051 | 120 |
| P1 HE | v0.8.23 | identisch (gleicher Anker/P/eingriffe) | **724** | 1007 | 120 |
| P2 HE | HEAD | `T=365 EV resolver art=2 P=(-1847,-480,-20387) eingriffe=3` | **0** | 973 | 192 |
| P2 HE | v0.8.23 | identisch | **724** | 973 | 192 |
| P2 Brand (11) | HEAD | `art=4 ... eingriffe=3`, `EV aufschlag re2_art=1` | **729** | | |
| P2 Saeure (10) | HEAD | `art=3 ... eingriffe=3`, `EV aufschlag re2_art=2` | 0 | | |

* Zustand (state.log P1/P2 HEAD, Bild 119 = Explosionsbild): Zombies 2/3 (t=10) `st=3 ss1=9 hp=-150/-120` (Zerreiss-Tod
  Zeile 9), in 11 Bildern um > 1000 weggeschleudert ((-1322,-19509) -> (-1847,-20988)); Typ 0x11 `st=2 ss1=9` (HURT-Leiter,
  ebenfalls ohne Flammen). Brocken (7/2) und Blut bitgleich zur v0.8.23 = das Zerplatzen ist unveraendert, NUR die 724
  Flammen-Zeilen fehlen.
* Bild (Framedump, RE15_FRAMEDUMP=113-185/6, RE15_WINDOW_SCALE=1): `scratchpad/abn/runs/ab_he_1140_b2.png` (oben HEAD,
  unten v0.8.23, Bilder 119/125/137/155/185): beide zeigen Explosion + Blut-/Teile-Schwall (125); ab 137 brennen in der
  v0.8.23 grosse Flammen auf den Leichen, in HEAD keine (Leichen russig-dunkel). `brand_saeure_head_b2.png`: Brandgranate
  HEAD brennt (oben), Saeure ohne Feuer (unten). `ab_he_1140_crop.png` (P1, Kamera 0): dasselbe.
* **Urteil Punkt 1: ERFUELLT.** Hinweise (keine Maengel): (a) die Russ-Tinten (@0x80106478-f8) bleiben — die Leichen sind
  dunkel getoent; das ist kein Feuer, im Dossier offen benannt (OFFEN 2). (b) Das Gate wirkt fuer JEDEN Aufrufer des
  Spreng-Russ, also auch GL-Explosivrunde und Rakete (Zeile 9/17) — ueber den Wortlaut ("normale Granaten") hinaus, aber
  vom Satz "Feuer nur bei incendiary" gedeckt und im Dossier ("Fuer den Nutzer") genannt.

## 5. Punkt 2 — "Manchmal selten loest die Granate bei der Verwendung nicht aus / wird nicht geworfen"
Eigene Aufstellung (NICHT ROOM1220 des Bau-Agenten): **ROOM1100**, Leon (-20975,-25000) Gier 3917, freier Flug ~7000
Einheiten bis zur Stirnwand. Je Lauf 9 Wuerfe `<H>0.5,<H>A0.1,<H><h>,W1.8` mit h = 0.0..0.8 s (R1 h Sekunden nach dem
Abzug los), H = M (MITTE) / MU (HOCH) / MD (TIEF); RE15_GIVE=9:30, RE15_EXIT_AT=900#1100. Abzug-Bilder aus debug.log
(`[input-script] Tick .. Tasten 0x8800`), Menge aus state.log `mg=`, Wuerfe/Explosionen aus gr.log.

| Lauf | Abzuege | Granaten | SPAWN (Bild) | Explosionen |
|---|---|---|---|---|
| MITTE HEAD | 9 (F16 88 163 241 322 406 493 583 676) | 30 -> 21 | **9** (je Abzug + 22: F38 110 185 263 344 428 515 605 698) | 9 |
| MITTE v0.8.23 | 9 | 30 -> 21 | **2** (F605, F698 = h 0.7/0.8) | 2 |
| HOCH HEAD | 9 | 30 -> 21 | **9** (je + 19: F35 107 ...) | 9 |
| HOCH v0.8.23 | 9 | 30 -> 21 | **3** (h 0.6-0.8) | 3 |
| TIEF HEAD | 9 | 30 -> 21 | **9** (je + 24: F40 112 ...) | 9 |
| TIEF v0.8.23 | 9 | 30 -> 21 | **1** (h 0.8) | 1 |
| Saeure 10 / Brand 11 HEAD (h 0.0/0.3/0.5) | 3 / 3 | 30 -> 27 | **3 / 3** | |
| Saeure 10 / Brand 11 v0.8.23 | 3 / 3 | 30 -> 27 | **0 / 0** | |

-> Der gemessene Aussetzer (Abzug verbraucht die Granate, R1 frueh los -> kein Wurf) ist in der ausgelieferten v0.8.23
reproduziert (21 von 27 HE-Abzuegen ohne Wurf, alle 6 Saeure/Brand) und in HEAD weg (27/27 + 6/6). Der Mechanismus ist
der selbst disassemblierte (Abschnitt 3: Abzug @0x80033b40 im Bild 0, Abbruch `sltu` @0x8003363c ab Cursor 11, Spawn
erst 19/22/24) — der Fix erklaert den Befund.
* Gehaltenes Quadrat (`MA1.6` ab dem Heben, dann R1 los, Lauf c3): HEAD 2 Abzuege / 2 Wuerfe (F143, F177), v0.8.23
  2 Abzuege / **1** Wurf (der automatische zweite Abzug des Held-Bits @0x80033300-0c verfiel beim R1-Loslassen) — ebenfalls
  behoben. Quadrat-Tipp WAEHREND des Hebens (F4-6, vor READY): in beiden Staenden kein Wurf und KEIN Verbrauch (Held-Bit;
  Original-Verhalten aller Waffen, kein Granatenverlust).
* **Rest-Fall, an der HEAD-exe gemessen (Lauf c2, ROOM1140 P1, Skript `MD0.6,MDA0.2,MD2.667,MDA0.2,MD2.0,W3`):** zweiter
  Abzug F105 (mg 4 -> 3), Zombie (Typ 0x11) packt Leon im Bild F113 (`gr=1`, Lage springt (-1000,-17918) -> (-1043,-18081),
  danach hp 100 -> 70 -> 40) = 8 Bilder nach dem Abzug, Spawnbild waere F129 -> **2 Abzuege, 1 Wurf, Menge 5 -> 3** —
  in HEAD wie in v0.8.23. Selbst disassembliert: STAGE1.BIN `80113b00 sb v1(=2),0x800aca58` + `80113b30 sh zero,0x800aca5a`
  (Griff ersetzt das Kommando ohne Ziel-Gate) = Original-Verhalten. Das Dossier fuehrt diesen Fall als OFFEN 1 und legt ihn
  dem Nutzer als **Wahlfrage** vor ("Soll sie im Inventar bleiben?"), obwohl seine eigene Beta->Retail-Begruendung fuer
  Punkt 2 (Verbrauch im Bild 0 + Wurf im Bild 19-24 = unfertige Beta-Kombination; RE2-Retail-Werfer: Abgang im
  Rueckstossbild 1, selbst geprueft `80044b98 lbu v1,333(s0) / 80044b9c addiu v0,zero,1 / 80044ba0 bne` in
  info/re2leon/PSX.EXE) genau diesen Fall mit abdeckt.
* Zweite Ursache "voller Pool im Spawnbild": das Fenster existiert im Spiel (P1-Lauf: state.log `fx=96` in F128-F130 nach
  der HE-Explosion an 3 Zombies), an der exe wegen des Griffs (s.o.) nicht nachstellbar — wie beim Bau-Agenten; belegt nur
  durch Sonde 300-304 (gruen) und Code-Lesung (1 Platz fuer 4/0x0D, `re15_esp_fx_free_slot` setzt flags/active wie
  `esp_fx_kill`, keine fx->fx-Verweise im Slot; `follow_slot` ist ein Aktor-Index).
* **Urteil Punkt 2: TEILWEISE.** Der haeufige, stille Aussetzer (R1-Loslassen) ist behoben und an eigener Aufstellung A/B
  gegen v0.8.23 gemessen; "Granate verbraucht, aber nicht geworfen" bleibt an der HEAD-exe reproduzierbar, sobald Leon in
  den 19-24 Bildern der Wurfbewegung gepackt/getroffen wird (Mangel 1), und der Rest wird als Wahlfrage an den Nutzer
  weitergereicht (Mangel 2).
* **Elza-Szenario** (Start-Rezept test_r35_werfer_elza: RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
  RE15_PSELECT_AUTO_SWITCH=1, ROOM1000 (21850,-13400) Blick 2048, RE15_GIVE=9:30, 3 Wuerfe mit R1 los nach 0.0/0.3/0.5 s):
  debug.log `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[equip] W-bank -> W09 (Clips 14, Rueckstoss-Clip7
  fc=35)`; HEAD 3 Abzuege / **3 Wuerfe** (F38, F110, F191) / 3 Explosionen, v0.8.23 3 / **0** / 0. Elzas Bank PL04W09.PLW
  hat dieselben Cliplaengen (Datei-Bytes @0x20 `0a00`, @0x24 `2300`, @0x2c `2800`, @0x34 `2800` = 10/35/40/40) wie
  PL00W09.PLW (@0x20 `0a00`, @0x24 `2300`, @0x2c `2800`, @0x34 `2800`, selbst gelesen) -> die Spawnbilder 19/22/24 liegen
  in beiden Clips; die Sonden-Konstanten 35/40/40 (test_r36_granate.c:277) stimmen mit den Datei-Bytes.
