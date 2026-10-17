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
- [x] Messung Punkt 2 an der exe (Abschnitt 3)
- [x] gezielte Tests zweite Sitzung: `ctest -R "r38_|tyrant|beretta"` -> `100% tests passed, 0 tests failed out of 14`
      (`Total Test time (real) = 181.98 sec`)
- [x] RE-Gate (Abschnitt 4)
- [x] Vertrag (Abschnitt 5)
- [x] Urteil (Abschnitt 6)

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

## 3. Punkt 2 — "In Stage 4 die Spiderman führen - mal wieder - dazu das Leon teilweise komplett von der Karte verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner."

Massstab: Abnahme 0 hat Punkt 2 als erfuellt gemessen (v0.8.25: Sprung 27 058 auf (-724,761) ausserhalb; neu: im Gang).
Nachbesserung 1 aendert die Opfer-Maschine nicht (`git diff 1488abd8..HEAD -- re15_port/engine/src/enemy_ai_common.c` =
nur Fall 2 Bruellen). Hier: nicht verschlechtert, M2 an der exe, dazu der Griff ueber den Tuer-Weg und das Losreissen.
Werkzeuge: `sprung.py` (groesster Bild-zu-Bild-Sprung), `wandcheck.py` (je Bild Spielerlage gegen die SCA-Rechteckzellen
Typ 1 mit u0&1 der RDT: Lage im Inneren einer Zelle? Strecke Vorbild->Bild durch das Innere einer Zelle?).

### 3.1 ROOM4001 Elza — Tuer-Weg (Lauf n4001t aus 2.1, bis F1250)
- Griff hinten B[9]: `F916 [4 ss1=9 ss2=0 mo=12 af=10]` -> `F917 PL(-26516,-7418) gr=1 [4 ss1=9 ss2=1 mo=18 af=1]`; Kollaps
  B[10] ab F1001 (`ss1=10`), Tod `F1037 PL(-25936,-7875) hp=-1 pst=7`.
- `sprung.py`: 1250 Bilder, x -26526..-15500, z -9888..-3524; **groesster Sprung 607 bei F1037** (Uebergang in den Tod),
  danach 215 (F591/F592, Koerperschub vor dem Griff, hp 90). `wandcheck.py`: **0 Bilder in einer Wandzelle, 0 Wanddurchgaenge**.
- M2 an der exe (anim.log Slot 4): `949 ... 18 18 34 33 33`, **`950 ... 18 18 34 33 33`** (P1 -> P2), `951 ... 19 19 21 1 1`
  (Halte-Clip); Kollaps-Start `1001 ... 19 19 21 9 9` -> `1002 ... 21 21 85 1 1`.
- Framedump F912-F1010 (`abn1t/n4001t_griff_F912-1008.png`, Ausschnitt `abn1t/n4001t_griff_crop.png`): Elza bleibt im Griff
  am Spiderman, am Ende des Gangs, in jedem Bild sichtbar.

### 3.2 ROOM4001 Elza — Abnahme-0-Lauf 3.1 unveraendert (Vergleichszahlen)
`lauf1.sh <exe> n4001a <wd> RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 "RE15_SET_FLAG=4:20,3:33"
"RE15_DEBUG_JUMP=4000@250" "RE15_PLAYER_POS=-15608,-3524,1024" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
"RE15_INPUT_SCRIPT=U3,L1,U3,W60" RE15_STATE_LOG=state.log RE15_ANIM_TRACE=anim.log "RE15_EXIT_AT=1000#4001"`:
Ausloeser F219, Wechselbild F253, Griff hinten `F679 PL(-26516,-7418) gr=1`, Kollaps F763, Tod F799. `sprung.py`: groesster
Sprung **607 bei F799** (Tod), sonst <= 288 (F357/F358 Koerperschub, hp 90). `wandcheck.py`: 0 / 0. Gleiche Werte wie der
Bau-Agent (F679, 607 bei F799) und wie Abnahme 0 (dort F678/F798, ein Bild frueher, weil das Bruellen jetzt sein P2-Bild hat).

### 3.3 ROOM4001 Elza — Losreissen (Tasten im Griff; Abnahme 0 konnte P5 -> P6 nicht an der exe messen)
Wie 3.2, aber `"RE15_INPUT_SCRIPT=U3,L1,U3,W13.3,B15"` (ab F669 Tastenhaemmern, input_pc.c 'B') und `"RE15_EXIT_AT=1300#4001"`:
- Drei Griffe losgerissen (Latch F679 / F826 / F974, Loesen bis F755 / F903 / F1051), der vierte (F1122, nach Ende des
  Haemmerns) fuehrt in Kollaps (F1206) und Tod (F1242).
- anim.log Slot 4, erster Griff: `711 ... 18 34 33 33`, `712 ... 18 34 33 33` (P1 -> P2), `713 ... 19 21 1 1` (erstes
  Halte-Bild, im selben Bild losgerissen: ss2=4), `714 ... 20 41 1 1` (Loese-Clip 0x14) ... `753 ... 20 41 40 40`,
  **`754 ... 20 41 40 40`** (P5 -> P6), **`755 ... 20 41 40 40`** (P6 ohne anim_set), `756 ... 3 15 1 1` (B[3]).
- Spieler: Clip 4 (Zugriff hinten) Bild 0..33, ab F713 Clip 6 (Loesen) Bild 0..47; Lage bleibt (-26516,-7418) bis F761.
- `sprung.py`: groesster Sprung 607 bei F1242 (Tod), sonst <= 288; `wandcheck.py`: 0 / 0 in 1300 Bildern.

### 3.4 ROOM5040 Leon (Lauf n5040 aus 2.3)
Griff hinten `F754 PL(-25886,-25198) gr=1 [1 ss1=9 ss2=1 mo=18 af=1]`, Kollaps F838, Tod F874. `sprung.py`: groesster Sprung
**607 bei F874** (Tod); danach 494 / 421 bei F499 / F500 (hp 80, kein Griff: der Spiderman schiebt Leon mit B[3] um die Ecke
nach Sueden = Abnahme 0 B1, in v0.8.25 gleich). `wandcheck.py` (ROOM5040.RDT): 0 / 0 in 1000 Bildern.

### 3.5 Zwischenurteil Punkt 2
In keinem der vier Laeufe verlaesst Elza/Leon den Raum, steht in einer Wand oder springt durch eine (0 / 0 je Lauf, 4550
Bilder), vorher (v0.8.25, Abnahme 0) 27 058 Einheiten auf (-724,761) ausserhalb. Unveraendert gegenueber Abnahme 0: ein
Sprung von 607 Einheiten im Bild des Todes (Lage von (-26336,-7418) nach (-25936,-7875), beide im Gang; s. Beobachtung B2).
Die generelle Regel (alle Greifer) ist von der Nachbesserung unberuehrt; ihre zehn Riegel-Faelle unit_r38_griff_* laufen
gruen (beide Sitzungen).

## 4. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, Binaerdatei je Zeile)

| Adresse (Binaerdatei) | Instruktionen (selbst gelesen) | Behauptung NB1 | Befund |
|---|---|---|---|
| EXE 0x8001f314-0x8001f390 | `lbu v0,148(t0)` Clip; a2 == 0: `lbu v0,149(t0)` @0x8001f35c, sonst fc - (+0x95) - 1 (@0x8001f340-54); `sw a2,360(t0)` @0x8001f36c; `jal 0x8001f3bc` @0x8001f38c, `addu a3,t1,zero` (fc) @0x8001f390 | anim_set waehlt Bild +0x95 | stimmt; zusaetzlich belegt: a2 = Rueckwaerts-Argument |
| EXE 0x8001f534 / 0x8001f574 / 0x8001f5a8-b4 / 0x8001f5d0 / 0x8001f5f0 | `jal 0x8001f664` (Keyframe); `jal 0x80068098` (RotMatrix, ohne/mit Ueberblendung); `lbu/addiu -1/sb 143` (+0x8F); `jal 0x80020510` | Pose VOR dem Hochzaehlen | stimmt |
| EXE 0x8001f610-0x8001f63c | `lbu v0,149` / `addiu v0,v0,1` / `sb v0,149`; `sltu v0,v0,s4`; `ori v0,zero,0x1` @0x8001f638; `sb zero,149(v1)` @0x8001f63c | Hochzaehlen danach, Wrap 0, Rueckgabe 1 | stimmt |
| STAGE4 0x80112B9C-0x80112BD8 | +0x94 = 0xF @0x80112BA8, +0x6 = 3 @0x80112BB8, +0x9C = 0x64 @0x80112BC8, +0x9E = 0x32 @0x80112BD8, kein +0x95 | Griff 1 P2 ohne +0x95 | stimmt |
| STAGE4 0x80112D20-0x80112E14 | P5: SE 0 bei +0x95 == 0x18; P1: `jal 0x8001ad68` @0x80112D5C, anim_set (a2 = 0) @0x80112D78, `lbu v1,6 / addu / sb v1,6` @0x80112D8C-9C; P6 @0x80112DA0: +0x1DE = 0x3C, +0x5 = 3, +0x6 = 0, word0 &= ~0x1000, +0x93 &= 0xFE, `jr ra` @0x80112E10 ohne `jal` | Griff 1 P1/P5/P6 | stimmt |
| STAGE4 0x801142C8-0x801142F4, 0x801143D8-0x80114488 | P2: +0x94 = 0x19, +0x6 = 3, +0x9C = 0x64, kein +0x95; P1 = P5: anim_set @0x801143EC, `+0x6 += fertig` @0x80114400-10; P6 @0x80114414: +0x1DE = 0x1E, +0x5 = 0xD, ..., `jr ra` @0x80114484 ohne `jal` | Griff 2 | stimmt |
| STAGE5 0x801156FC-0x80115718 | `jal 0x8001f314`, `beq v0,zero`, `ori v0,zero,0x101` @0x80115708, `sw v0,4(v1)` @0x80115718 | STAGE5 = STAGE4 +0x180 | stimmt |
| STAGE4 0x80112294-0x801123D8 (B[2]) | P0 @0x801122E0: +0x6 = 1, +0x94 = 2 @0x801122F0, +0x95 = 0 @0x80112300, +0x96 = 0 @0x80112310, +0x8F = 7 @0x80112320; P1 @0x80112324: SE 1 bei +0x95 == 0x14 (`jal 0x800453d0` @0x80112340); **`lbu a2,7(v0)` @0x8011235C** -> `jal 0x8001f314` @0x80112360 (a3 = 0x200); `lbu v1,6(a2)` / `addu` @0x80112374-7C, **`jal 0x80115bec` (a0 = a1 = 0) @0x80112380**, `sb v1,6(a2)` im Verzoegerungsslot @0x80112384; P2 @0x80112390: `lbu v0,7(a0)`, == 0 -> +0x5 = 3 @0x801123C4, +0x6 = 0 @0x801123D4, sonst +0x5/+0x6/+0x7 = 0 @0x801123A0-C0 | re15_r38_sm_bruellen | Phasen/Adressen stimmen; **zwei Elemente fehlen im Port und im Dossier: Rueckwaerts-Argument a2 = +0x7 und der Aufruf 0x80115bec** (Mangel M3) |
| STAGE4 0x801123E8-0x80112540 (A[3], nicht Teil des Diffs) | Bruellen nur wenn `lhu v0,464(v1)` (+0x1D0) `andi 0x1` == 0 (`bne` @0x80112500) und +0x1D4 >= 0x1B59 (`sltiu`/`bne` @0x80112510-14); dann +0x5 = 2 @0x8011251C, +0x6 = 0 @0x8011252C, **+0x7 = 1** (`ori v0,zero,0x1` @0x80112538, `sb v0,7(v1)` @0x80112540) | — | Port-A[3] (enemy_ai_common.c re15_tyrant_ai_tick Fall 3: `else if (dist >= 7001) { sub_state_1 = 2; ... }`) hat weder die +0x1D0-Sperre noch +0x7 = 1 -> Beobachtung B1 |

Weitere Pruefungen:
- Rate-Woerter in den hinzugefuegten Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|geraten|vermutlich`):
  keine. Einziger getenv im Spur-Diff: `R38_GRIFF_LOG` im Test (Logausgabe). NB1 fuehrt keine neue Zahl ein: fc-1 aus der
  Bank (re15_birkin_fc), 0 = Wrap-Wert @0x8001f63c.
- Erklaert der Fix den Befund? Ja: M1 = Port zeichnet nach dem Vorschub, am Clip-Ende stand Bild 0 (exe Abnahme 0: `af=0`
  in F253/F320/F376); Original rechnet die Pose vor dem Hochzaehlen (oben) -> Wechselbild = fc-1; nachher an der exe
  `af=34` und gezeichnet (0x1D, 34) in allen drei Raeumen (Abschnitt 2), die Pose des Riegels und der exe stimmen ueberein.
- **Misst der Riegel? Gegenlauf mit Mutation (Scratch-Bau ausserhalb des Repos):** `git archive HEAD re15_port/{CMakeLists.txt,
  cmake,engine,include,platform,tests,targets,tools}` nach `abn1t_mut/`, Asset-Pfad der Sonde auf den Spurbaum gestellt,
  Configure mit den Optionen von local_build.sh und msys64-PATH vorn, `cmake --build build_mut --target test_r38_spiderman`
  (155/155).
  - Mutation 1 (`sm_anim_halten` ohne Halten): `test_r38_spiderman: 16 FAIL`, u. a. `FAIL: M1: Wechselbild zeigt Clip 0x1D
    Bild 34 (...), gezeichnet Clip 0x1D Bild 0`, `FAIL: M1: Ueberblendung startet an der Austritts-Endpose (Abstand 2084 <= 21)`,
    `FAIL: Bruellen B[2]: Fertig-Bild und P2-Bild zeigen Clip 2 Bild fc-1 ...`, `FAIL: M2 Griff 2: kein Ruecksprung auf Bild 0: 2`.
  - Mutation 2 (Griff-P2 ohne `anim_frame = 0`): `test_r38_spiderman: 4 FAIL` (`M2 FEHLER: erstes Halte-Bild Clip 0xF Bild 15
    (soll 1)`, `M2 FEHLER (Griff 2): erstes Halte-Bild Clip 0x19 Bild 5`).
  Beide Zahlen = Angabe des Bau-Agenten. Der Riegel faengt M1 und M2.

## 5. Vertrag
- Pfade (`git diff master...HEAD --name-only`): engine/src/enemy_ai_common.c, engine/src/re15_damage.c, engine/src/spiderman_r38.c
  (neu), include/re15_spiderman_r38.h (neu), platform/pc/main.c, tests/integration/test_r37_beretta.cmake,
  tests/unit/probes/r38_spiderman.cmake (neu), tests/unit/test_r38_griff_alle.c / test_r38_spiderman.c (neu), analysis/befunde_runde38/T_*.
  Kein release/, platform/android/, shared_assets/, keine Test-CMakeLists.txt.
- Nachbesserung 1 (`git diff 1488abd8..HEAD --name-only`): nur spiderman_r38.c, re15_spiderman_r38.h, test_r38_spiderman.c,
  Dossier/Bilder und in enemy_ai_common.c **ein** Haken (Fall 2 Bruellen, +3/-5 Zeilen, "Runde 38 Spur T"; enemy_ai_common.c ist
  T zugeteilt, VERTRAG §1.4). re15_tyrant_ai_tick wird nur fuer Typ 0x2B gerufen (enemy_ai_common.c:14798-14801) — keine
  Nebenwirkung auf andere Gegner.
- Bank-9-Bits, Nachrichten-Ids, Items: keine belegt. re15_damage.c (Bauphase, 3 Zeilen) steht nicht in §1.4 — Hinweis fuer die
  Integration mit R/S (wie Abnahme 0 B7); der Fall-2-Haken ist bei der Integration mit Spur B (gleiche Datei) zusammenzufuehren.
- Riegel je Punkt: Punkt 1 unit_r38_spiderman (austritt + Wechselbild, Mutationsproben oben), Punkt 2 unit_r38_spiderman
  (griff1/griff2) + zehn unit_r38_griff_*; beide Szenarien an der exe gemessen (Elza 4001, Leon 5040/5070).
- Nebenwirkung geprueft: ROOM5110 (Typ 0x2B ohne Schacht, grid 0, Leon) — Bruellen wird mit P2-Bild beendet (s. B1); in
  v0.8.25 (Gegenprobe, Messkopie `re15_pc_abn1t_v0825.exe` im Bau-Ordner des Spurbaums, nach dem Lauf entfernt) derselbe Ablauf.

## 6. Urteil

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| 1 Spiderman kommen richtig aus den Schaechten | **erfuellt** | M1 behoben an der exe: Wechselbild zeichnet Clip 0x1D Bild 34 in 4001 (Tuer-Weg, F485), 5040 (F320), 5070 (F376) statt Bild 0 (Abnahme 0); Framedumps 4001 F481-F488 und 5070 F372-F379 durchgehend sichtbar, kein Bild in Wand/Boden. Austritts-Mechanismus wie in Abnahme 0 (Warten auf grid 0x42 ueber 445 Bilder im Tuer-Lauf, Freigabe 0x21, ACTIVE nach 35 Bildern, zweiter Schacht bleibt zu). Original-Beleg FUN_8001f3bc @0x8001f534..@0x8001f63c selbst disassembliert; Riegel faengt die Mutation (16 FAIL). |
| 2 Spieler verschwindet bei keinem Gegnerangriff von der Karte | **erfuellt** | 4 exe-Laeufe (4001 Tuer-Weg, 4001 Abnahme-0-Lauf, 4001 Losreissen, 5040 Leon), 4550 Bilder: 0 Bilder in einer Wandzelle, 0 Wanddurchgaenge, groesster Sprung 607 im Todesbild (im Gang, s. B2) — vorher (v0.8.25) 27 058 auf (-724,761) ausserhalb. M2 an der exe: Griff P1 -> P2 (0x12, 33) und Loese-Ende P5 -> P6 (0x14, 40) gehalten. Generelle Regel und ihre zehn Riegel unveraendert gruen. |

Gates: Bau gruen (beide Sitzungen); gezielte Tests 204/204 (erste Sitzung) und 14/14 (zweite Sitzung); RE-Gate gehalten
(Abschnitt 4: alle Stichproben stimmen, keine Rate-Woerter, keine neue Zahl, Mutationsproben 16 / 4 FAIL); Vertrag gehalten
(Abschnitt 5).

**bestanden: JA.** M1 und M2 aus Abnahme 0 sind behoben und an der exe gemessen; beide Punkte erfuellt. Die folgenden
Punkte sind NICHT urteilsrelevant (nicht im Wortlaut, vorbestehend, in v0.8.25 gleich) und gehoeren in den NACHLAUF.

### Maengel (nicht urteilsrelevant)
- **M3 (Dokumentation/RE-Vollstaendigkeit, neu geschriebener Code B[2]):** re15_r38_sm_bruellen (spiderman_r38.c, Kommentar
  "(1b) BRUELLEN") nennt fuer P1 "anim_set @0x80112360 mit a2 = +0x7" und "`+0x6 += fertig` @0x80112374-84", setzt aber
  (a) das Rueckwaerts-Argument nicht um (sm_anim_halten spielt immer vorwaerts; Original `lbu a2,7(v0)` @0x8011235C, in
  FUN_8001f314 a2 != 0 -> Bild = fc - (+0x95) - 1 @0x8001f340-54) und laesst (b) den Aufruf `jal 0x80115bec` (a0 = a1 = 0)
  @0x80112380 weg, der mitten im zitierten Bereich liegt (Matrizenkette Teile 0/19/20/21 = Bein 0 ueber FUN_80022da0
  @0x80115c1c-60, danach Modell-Translation +0x34/+0x3C -= Differenz @0x80115c78-a4; B[7] ruft dasselbe mit a1 = 1
  @0x8011473C). Beides steht nicht im Dossier-OFFEN (dort 0x80115bec nur fuer B[7], OFFEN 10). (a) ist im Port derzeit
  unerreichbar, weil +0x7 beim Bruellen immer 0 ist (s. B1); die Wirkung von (b) im Bild ist nicht ermittelt. Nachpruefbar:
  `re15_disasm.py dis 0x80112294 82 --bin STAGE4.BIN`. Erwartet: OFFEN-Eintrag im Dossier (oder Umsetzung) im NACHLAUF.

### Beobachtungen (vorbestehend, fuer NACHLAUF/Integration)
- **B1 Dauer-Bruellen auf Abstand (A[3] unvollstaendig).** Original A[3] 0x801123E8 bruellt nur bei GELOESCHTEM Sichtbit
  (+0x1D0 & 1, `andi`/`bne` @0x801124FC-500) und Abstand >= 7001 (@0x80112510) und setzt dann **+0x7 = 1** (`sb v0,7(v1)`
  @0x80112540) -> B[2] spielt rueckwaerts und endet in P2 im Leerlauf (+0x5 = +0x6 = +0x7 = 0 @0x801123A0-C0). Port-A[3]
  (re15_tyrant_ai_tick Fall 3): `else if (dist >= 7001) { sub_state_1 = 2; sub_state_2 = 0; }` — ohne Sichtbit, ohne +0x7.
  Folge an der exe: ROOM5110 Leon steht am Sprungziel (-25000,-13000), Spiderman Slot 2 bei (-19484,-7209) (Abstand ~7998)
  bruellt endlos: `F413 ss1=2` ... `F463 ss1=3 ss2=0` -> `F464 ss1=2 ss2=1 mo=2 af=1` -> ... (9 Bruellen in 450 Bildern, ohne
  sich zu bewegen). v0.8.25 (Gegenprobe, gleicher Lauf) identisch (F413/F463/F513/..., Periode 50 statt 51). In den
  Schacht-Raeumen tritt das ein, sobald der Spieler am Ende eines Bruellens >= 7001 entfernt ist. Lauf: `lauf1.sh <exe> n5110
  <wd> RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 "RE15_DEBUG_JUMP=5110@250" RE15_STATE_LOG=state.log
  RE15_ANIM_TRACE=anim.log "RE15_EXIT_AT=900#5110"`.
- **B2 Sprung 607 im Todesbild.** In allen vier Laeufen springt der Spieler im Bild des Todes (Kollaps-Bild 0x23, hp -1,
  pst 7) um 607 Einheiten, z. B. 4001 `F1036 PL(-26336,-7418)` -> `F1037 PL(-25936,-7875)`, 5040 `F873` -> `F874
  (-25428,-24618)`. Vorher steht der Spieler waehrend Griff und Kollaps mit z = -7418 an der Nordwand des Gangs (SCA-Zelle #5
  ROOM4001 x -27776..-15968, z -6950..-5222; -6950 - Radius 468 = -7418), danach liegt er ungeklemmt im Gang. Beide Lagen im
  Raum (wandcheck 0/0); gleiche Werte wie Abnahme 0 (dort F798). Ursache (Wechsel der Platzierung zwischen Opfer-Kollaps und
  Todes-Zweig) nicht ermittelt — kein "Verschwinden", aber ein sichtbarer Ruck von ~0,6 m.
- **B3 Charge B[7] direkt nach dem Bruellen** (Dossier OFFEN 10, selbst bestaetigt: +0x9E = 1 @0x80114634, `lb a2,158(v0)`
  @0x80114718 -> anim_set rueckwaerts, im Handler nur arc_test/FX/anim_set/0x80115bec, kein Walker): der Port laeuft
  stattdessen vorwaerts (4001 Lauf n4001r `F348 ss1=7 mo=4 @(-23894,-8587)` -> `F357 @(-24389,-8362)` = 540 Einheiten in 9
  Bildern). B[7] folgt in den Schacht-Raeumen regelmaessig auf das erste Bruellen (Spieler-hp >= 81): n4001t F543, n4001r F348.
- **B4 Koerperschub** (Abnahme 0 B1, unveraendert): 5040 schiebt der Spiderman Leon mit B[3] bis 494 Einheiten je Bild um die
  Ecke (F498-F503, hp 80, kein Griff), im Raum.
- **B5 Zeichen-Konvention** (Dossier OFFEN 12 / Abnahme 0 B6): innerhalb eines Clips zeigt der Port das Bild, das das Original
  ein Bild spaeter zeigt (erstes sichtbares Austrittsbild 7 statt 6, erstes Halte-Bild 1 statt 0); am Clip-Ende stimmen beide
  jetzt ueberein (fc-1).
- **B6** Integration: Fall-2-Haken in enemy_ai_common.c mit Spur B zusammenfuehren; re15_damage.c-Haken (Bauphase) mit R/S.
  Der relayte Nutzer-Nachtrag ROOM20A0 (Rolltor vor der Kamera) gehoert zu Spur W, hier nicht geprueft.

### Messdaten (Scratchpad dieser Sitzung, nicht im Repo)
`abn1t/n4001t`, `n4001u` (Tuer-Weg), `n4001a` (Abnahme-0-Lauf), `n4001m` (Losreissen), `n4001r` (Weglaufen), `n5040`, `n5070`,
`n5070b`, `n5110`, `a5110` (v0.8.25), `mut_run/mut1_out.txt` / `mut2_out.txt` (Mutationsproben); Tafeln `n4001t_F481-488.png`,
`n4001_austritt_F448-484.png`, `n4001_austritt_crop.png`, `n4001t_griff_F912-1008.png`, `n4001t_griff_crop.png`,
`n5070_F372-379.png`, `n5070_crop_F374-377.png`.
