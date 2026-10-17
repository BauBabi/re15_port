# Runde 38 — Spur T "spiderman" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Pruefer: Abnahme-Agent 1, 2026-10-16. Baum `.claude/worktrees/r38_spiderman`, Zweig `r38/spiderman`, gepruefter Stand
0f6e41d7 (Nachbesserung 1 abgeschlossen), Basis (merge-base mit master) e2cc7673. Massstab: AUFTRAG.md (Wortlaut),
VERTRAG.md §1.4/§2, CLAUDE.md STOP-GATE, Maengel M1/M2 aus `T_abnahme_0.md`. Bericht wird fortlaufend geschrieben
(Sitzungslimit). Zweite Sitzung (2026-10-17): setzt den Teilbericht 1f1f030b fort (Abschnitte 0/1 unveraendert uebernommen,
ab Abschnitt 2 neu gemessen).

Messwerkzeuge (Scratchpad, nicht im Repo): `abn1t/lauf1.sh` (Messkopie `re15_pc_abn1t_<name>.exe` neben der Bau-exe,
RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1, nach dem Lauf geloescht), `abn1t_tools/sprung.py` (groesster
Bild-zu-Bild-Sprung des Spielers je Raum aus RE15_STATE_LOG), `abn1t_tools/sca.py` (SCA-Zellen einer RDT, Format
rdt_common.c:250-272), `abn1t_tools/montage.py`/`crop.py` (Framedump-Tafeln). Gezeichnetes Bild = RE15_ANIM_TRACE
(anim_select_common.c:135, Spalten Bild/Slot/Typ/motion/Clip/fc/anim_frame/aufgeloestes Bild/...).

## 0. Stand der Pruefung
- [x] Pflichtlektuere: AUFTRAG.md, VERTRAG.md, Dossier T_spiderman.md (inkl. Abschnitt "Nachbesserung 1"),
      T_abnahme_0.md, `git log master..HEAD` (8 NB1-Commits 4e3b44b5..0f6e41d7 auf Abnahme 0 1488abd8),
      `git diff master...HEAD` (Spur-Diff 25 Dateien). Die in `git diff master` (zwei Punkte) zusaetzlich geloeschten
      Orchestrator-Dateien und AUFTRAG/VERTRAG-Abweichungen (Nachtraege 1/2, Spur W) sind master-Fortschritt nach e2cc7673.
- [x] Code-Diff gelesen (NB1: `git diff 1488abd8..HEAD -- re15_port`): sm_anim_halten an den Dateianfang verlegt und an
      Austritt-Ende, Griff 1 P0/P1/P5, Griff 2 P0/P1/P5 eingesetzt; P2 beider Griffe `anim_frame = 0`; neues
      re15_r38_sm_bruellen (B[2]) mit Haken in enemy_ai_common.c case 2; Test test_r38_spiderman.c um gezeichnete Pose
      erweitert.
- [x] Bau (diese Sitzung): `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`;
      `... build` -> `ninja: no work to do.`, `LOCAL-BUILD-OK (build)`. re15_pc.exe 2026-10-16 23:06:09, spiderman_r38.c.obj
      23:06:03 (Quelle 23:05:49), enemy_ai_common.c.obj 23:02:13 (Quelle 23:01:17); `find -newer re15_pc.exe` ueber
      engine/include/platform leer; Arbeitsbaum sauber (= 0f6e41d7; letzter Code-Commit 2029729b 23:07:52).
- [x] gezielte Tests (204/204 gruen, Abschnitt 1)
- [x] Bau zweite Sitzung: `local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`, `... build` -> `ninja: no work to do.`,
      `LOCAL-BUILD-OK (build)`; re15_pc.exe unveraendert 2026-10-16 23:06:09, Arbeitsbaum sauber,
      `git diff 2029729b..HEAD -- re15_port` leer.
- [x] Messung Punkt 1 (M1) an der exe (Abschnitt 2)
- [ ] Messung Punkt 2 an der exe (nicht verschlechtert)
- [ ] RE-Gate (Stichproben NB1)
- [ ] Vertrag
- [ ] Urteil

## 1. Gezielte Tests (diese Sitzung)
`ctest --test-dir re15_port/build -R "<R>" --timeout 600 -j 5 --output-on-failure` mit R = Auswahl des Bau-Agenten
(`r38_|tyrant|_2b|t2b|5040|5070|4001|4000|beretta|victim|grab|griff|bissarm|1210|dog|hund|re2z|crawl|kriech|affen|gorilla|
maggot|devour|finisher|pushoff|1220|11c0|aim|ivy|enemy_ai|roster|stage4|stage5|spider|spinne|zombie|schatten|shadow|damage|
schuss|treffer|hit|crow|kraehe|gator|g5|tentakel|zellenarm|gitter|birkin|opfer|anker|anchor`) **erweitert um
`shootable|elzaems|anim_`** (unit_enemy_shootable schiesst auf 0x2B, unit_r37_elzaems_* laden die EM2B-Baenke, unit_anim_*
pruefen den Anim-Treiber, den die Nachbesserung am Clip-Ende umgeht) -> **`100% tests passed, 0 tests failed out of 204`,
`Total Test time (real) = 204.49 sec`** (Maschine parallel von anderen Agenten belastet: r37g/r37n/r38c/r38e-Messkopien liefen).
Darin unit_r38_spiderman, alle zehn unit_r38_griff_*, unit_tyrant_ai, integration_r37_beretta, integration_r36_schuss,
integration_r36_elza_schuss, integration_r37_szene11c0.

test_r38_spiderman direkt gestartet (Ausgabe `abn1t/t_spiderman_out.txt` im Scratchpad):
```
== austritt (ROOM4001, Ck(4,20)=1, Ck(3,0x21)=1) ==
  Wechselbild: [1/1/0] Clip 0x1D Bild 34, gezeichnete Wurzel (0,-940,0); Clip 0x1D Bild 34 (0,-940,0), Bild 0 (-1683,1278,0)
  erstes ACTIVE-Bild: [1/2/1] Clip 0x2 Bild 1 frac 6, gezeichnete Wurzel (0,-946,0), Abstand zur Austritts-Endpose 6 (Clip-Ziel 21)
  Bruellen: Fertig-Bild 1, P2-Bild 1, Fehler 0 (Clip 2 fc 50)
== griff1 vorn/hinten Lauf A ...  Latch Bild 108: Spieler-Anker (-14022,-8593), Greifer-Anker (-14022,-8593), aca59 0
  Wechselbilder: Zugriff-Ende 1, Halte-Start 1, Loese-Ende 0, P6 0, Kollaps-Ende 1, Fehler 0; Ruecksprung Modul 0
== griff1 Lauf B (Ecke) ... Latch Bild 95: Spieler-Anker (-13977,-7782) == Greifer-Anker; Fehler 0
== griff1 Losreissen ... Zugriff-Ende 3, Halte-Start 3, Loese-Ende 2, P6 2, Kollaps-Ende 0, Fehler 0
== griff2 ... Griff-2-Wechselbilder: Zugriff-Ende 1, Halte-Start 1, Loese-Ende 1, P6 1, Fehler 0; Ruecksprung 0
test_r38_spiderman: 0 FAIL
```

## 2. Punkt 1 — "In Stage 4 die Spiderman kommen nicht richtig aus den Schächten raus wie im Original, obwohl die Schächte korrekt hoch gehen"

Massstab: Abnahme 0 hat den Mechanismus (Warten auf grid 0x42, sichtbar Bild 6, Freigabe 0x21, ACTIVE nach 35 Bildern, im
Schacht kein Ziel/Schatten/Schub) als byte-true gemessen und nur M1 (Ein-Bild-Ruecksprung am Austritts-Ende) offen gelassen.
Hier: M1 an der exe in allen drei Schacht-Raeumen, 4001 ueber den ECHTEN Weg (Tuer), dazu der Rest des Ablaufs erneut.

### 2.1 ROOM4001 Elza, echter Weg: Tuer 4011 -> 4001, stehen, dann in den Ausloeser Aot 5 gehen
`lauf1.sh <exe> n4001t <wd> RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 "RE15_SET_FLAG=4:20,3:33"
"RE15_DEBUG_JUMP=4010@250" "RE15_PLAYER_POS=-20850,-23700,1024" "RE15_AUTOPILOT=xza:-20850,-24300" RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=300 "RE15_INPUT_SCRIPT=R0.35,U3,L1,U3,W60" RE15_STATE_LOG=state.log RE15_ANIM_TRACE=anim.log
"RE15_FRAMEDUMP=470-1010/1:bilder/f" "RE15_EXIT_AT=1250#4001"` (Bildnummern = Bilder in 4001; die Bildzaehlung beginnt je
Raum neu, main.c:8183). debug.log: `[room] PC loaded room4011.rdt` -> `[auto] Ziel erreicht (-20850,-23700), halte Aktion`
-> `[room] PC loaded room4001.rdt` -> `[auto] Aktion hat versetzt -> (-15608,-3524), Ziel 0 fertig`. Lauf zweimal gefahren
(n4001t, n4001u mit `RE15_FRAMEDUMP=440-470/1`), state.log bitgleich in den Zustandswechseln.
- F6-F450: beide 0x2B `st=4 ss1=1 ss2=1 g=41` (versteckt, P1) — 445 Bilder, waehrend Elza an der Tuer steht und losgeht.
- F451: Elza (-17084,-9359) = im Aot 5 (Mitte (-20057,-8740) halb (3050,2500)): Slot 4 `g=42 ss2=2 af=1`; Slot 3 (Aot 4)
  bleibt bis zum Ende `g=41 ss2=1`.
- F484 `g=00 mo=29 af=34`; **F485 `st=1 ss1=1 ss2=0 mo=29 af=34`** (Wechselbild); F486 `ss1=2 mo=2 af=1` (Bruellen).
- anim.log Slot 4: `484 4 43 29 29 35 34 34`, **`485 4 43 29 29 35 34 34`**, `486 4 43 2 2 50 1 1` — im Wechselbild wird
  Clip 0x1D Bild 34 gezeichnet (Abnahme 0, gleicher Codepfad: F253 Bild 0).
- Bruellen-Ende: `534 ... 2 2 50 49 49`, `535 ... 49 49` (ss2=2), `536 ... 49 49` (P2: ss1=3), `537 ... 3 3 15 1 1`.
- Framedump (Scratchpad `abn1t/n4001t_F481-488.png`, `abn1t/n4001_austritt_F448-484.png`, `abn1t/n4001_austritt_crop.png`):
  F452 Deckel fliegt, F456 liegt er am Boden, F457 erstes sichtbares Spiderman-Bild am Wandschacht (Port-Bild 7, Konvention
  B6/OFFEN 12), bis F484 kriecht er heraus; **F484 und F485 zeigen dieselbe Pose, F486 beginnt das Bruellen — kein Bild in der
  Wand, kein Verschwinden.**

### 2.2 ROOM5070 Leon (Bodenschacht; Abnahme-0-Lauf 2.3 d unveraendert)
`lauf1.sh <exe> n5070b <wd> RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 "RE15_DEBUG_JUMP=5070@250"
"RE15_PLAYER_POS=1700,-5700,2048" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=U9.5,W30"
RE15_STATE_LOG=state.log RE15_ANIM_TRACE=anim.log "RE15_FRAMEDUMP=366-386/1:bilder/f" "RE15_EXIT_AT=400#5070"` (zweimal
gefahren, n5070/n5070b identisch).
- state.log: F375 `[1 t=2b st=4 ss1=1 ss2=2 g=00 mo=29 af=34]`, **F376 `[1 t=2b st=1 ss1=1 ss2=0 g=00 mo=29 af=34]`**,
  F377 `ss1=2 ss2=1 mo=2 af=1`.
- anim.log Slot 1: `375 1 43 29 29 35 34 34`, **`376 1 43 29 29 35 34 34`** (Abnahme 0: `af=0`, Startpose 1278 unter dem
  Boden), `377 1 43 2 2 50 1 1`.
- Framedump (`abn1t/n5070_F372-379.png`, Ausschnitt `abn1t/n5070_crop_F374-377.png`): der Spiderman ist in F372-F379
  durchgehend ueber dem Gitter hinter Leon sichtbar; F375/F376 gleiche Pose, F377 weicher Uebergang. Gegenbild vorher
  (Bau-Agent, `T_bilder/nb1_vor_5070_F374-377.png`): F376 leer.

### 2.3 ROOM5040 Leon (Abnahme-0-Lauf 2.3 c)
`lauf1.sh <exe> n5040 <wd> RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 "RE15_DEBUG_JUMP=5040@250"
"RE15_PLAYER_POS=1500,-15000,2048" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=U9,W60"
RE15_STATE_LOG=state.log RE15_ANIM_TRACE=anim.log "RE15_EXIT_AT=1000#5040"`: anim.log Slot 1 `319 ... 29 35 34 34`,
**`320 ... 29 35 34 34`** (Abnahme 0: `mo=29 af=0`), `321 ... 2 50 1 1`; Bruellen-Ende `369/370/371 ... 2 50 49 49`,
`372 ... 3 15 1 1`. Slot 2 (zweiter Schacht) bleibt `g=41` versteckt.

### 2.4 Zwischenurteil Punkt 1
M1 ist an der exe in 4001 (Tuer-Weg), 5040 und 5070 behoben: das Wechselbild zeichnet Clip 0x1D Bild 34, der Uebergang in
das Bruellen ist weich, kein Bild zeigt den Spiderman in der Wand oder unter dem Boden. Der uebrige Austritts-Mechanismus
ist unveraendert wie in Abnahme 0 gemessen (Warten auf grid 0x42 ueber 445 Bilder im Tuer-Lauf, Freigabe bei Bild 0x21,
ACTIVE nach 35 Bildern, zweiter Schacht bleibt zu). Urteil s. Abschnitt 6.
