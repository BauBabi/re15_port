# Runde 38 — Spur T "spiderman" — Abnahme 0 (unabhaengig)

Pruefer: Abnahme-Agent 0, 2026-10-16 (zweite Sitzung; die erste brach am Sitzungslimit nach dem Bau ab, Teilbericht
388f6578). Baum `.claude/worktrees/r38_spiderman`, Zweig `r38/spiderman`, gepruefter Code-Stand 5db78deb (HEAD 388f6578 =
nur der Teilbericht), Basis (merge-base mit master) e2cc7673. Massstab: AUFTRAG.md (Wortlaut), VERTRAG.md §1.4/§2,
CLAUDE.md STOP-GATE. Dieser Bericht wird fortlaufend geschrieben (Sitzungslimit).

Messwerkzeuge (nicht im Repo, Scratchpad dieser Sitzung): `lauf.sh` (Messkopie `re15_pc_abn0t_<name>.exe` im Bau-Ordner
des Spurbaums, Standard RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1), `sprung.py` (groesster Bild-zu-Bild-Sprung des Spielers aus
RE15_STATE_LOG), `sca_map.py` (Kollisionsplan). Vergleichs-exe "vorher" = v0.8.25-Paket
`release/pkg-win/re15_port_v0.8.25/re15_pc.exe` (nur gelesen, als Kopie in den Spurbaum gelegt); zwischen v0.8.25
(2077bb8e) und der Basis e2cc7673 gibt es keinen Commit unter re15_port (`git log 2077bb8e..e2cc7673 -- re15_port` leer).

## 0. Stand der Pruefung
- [x] Pflichtlektuere: AUFTRAG.md, VERTRAG.md, Dossier T_spiderman.md, `git log master..HEAD` (11 Spur-Commits + Teilbericht),
      `git diff master...HEAD` (Spur-Diff: 22 Dateien + Teilbericht). Die in `git diff master` geloeschten
      Orchestrator-Dateien (r38.js, r38_w2*.js, r37_*_forts.js, Release-Notes-Entwurf) und die AUFTRAG/VERTRAG/UEBERGABE-
      Abweichungen sind master-Fortschritt nach e2cc7673, NICHT die Spur.
- [x] Code-Diff gelesen: spiderman_r38.c (363 Z.), re15_spiderman_r38.h, Haken in enemy_ai_common.c (victim_clip_map,
      devour, victim_place, victim_tick, Zugaenge, tyrant case 4/5/8/9/10/15), re15_damage.c (2 Ziel-Sperren),
      platform/pc/main.c (Schatten-Sperre), Tests test_r38_spiderman.c / test_r38_griff_alle.c / probes/r38_spiderman.cmake,
      Alt-Riegel test_r37_beretta.cmake.
- [x] Bau (diese Sitzung): `bash re15_port/tools/local_build.sh configure` -> LOCAL-BUILD-OK (configure);
      `... build` -> "ninja: no work to do", LOCAL-BUILD-OK (build). re15_pc.exe 2026-10-16 17:57:37, juenger als jede
      Quelle (`find -newer` leer), Arbeitsbaum sauber (= 5db78deb).
- [x] gezielte Tests (188, alle gruen)
- [x] Messung Punkt 1 (ROOM4001 Elza Tuer + Ausloeser, ROOM5040 Leon, Gegenprobe v0.8.25)
- [x] Messung Punkt 2 (ROOM4001 Elza Griff hinten + Kollaps vorher/nachher, ROOM5040 Leon)
- [x] RE-Gate (Stichproben, Abschnitt 4)
- [x] Vertrag (Abschnitt 5)
- [x] Gegenlauf des Riegels test_r38_griff_alle gegen den ALTEN Engine-Stand e2cc7673 (Abschnitt 3.4)
- [x] Urteil (Abschnitt 6)

## 1. Gezielte Tests (diese Sitzung)
`ctest --test-dir re15_port/build -R "<R>" --timeout 600 -j 5 --output-on-failure` mit
R = `r38_|tyrant|_2b|t2b|5040|5070|4001|4000|beretta|victim|grab|griff|bissarm|1210|dog|hund|re2z|crawl|kriech|affen|gorilla|
maggot|devour|finisher|pushoff|1220|11c0|aim|ivy|enemy_ai|roster|stage4|stage5|spider|spinne|zombie|schatten|shadow|damage|
schuss|treffer|hit|crow|kraehe|gator|g5|tentakel|zellenarm|gitter|birkin|opfer|anker|anchor`
(Auswahl ueber die vom Diff beruehrten Module: Opfer-Maschine aller Greifer, Ziel-/Trefferauswahl, Schatten, Typ 0x2B, Raeume
4001/5040) -> **`100% tests passed, 0 tests failed out of 188`, Total Test time (real) = 360.54 sec.**
Darunter unit_r38_spiderman und alle zehn unit_r38_griff_*; integration_r37_beretta (233 s), integration_r36_schuss,
integration_r36_elza_schuss gruen. Einzelausgaben (direkt gestartet):
- test_r38_spiderman: 0 FAIL; austritt: grid 0x42 in Bild 64, sichtbar +6, greifbar +33 (0x21), ACTIVE +34, zweiter
  Spiderman bleibt versteckt; griff1 Lauf A/B: Latch-Anker Spieler == Spiderman ((-14022,-8593) bzw. (-13976,-7834)),
  402 Griffbilder, 0 ausserhalb, 0 hinter einer Wand, groesster Sprung 292 / 460; griff2: einmalige Lage, 0 Bilder bewegt.
- test_r38_griff_alle (10 Faelle): alle OK; groesste Bild-zu-Bild-Spruenge: zombie15 192/278, zombie2 307/307,
  kriecher2_1220 786, hund15 399, **hund2 3255**, gorilla 1743, spiderman 477/778.

## 2. Punkt 1 — "In Stage 4 die Spiderman kommen nicht richtig aus den Schächten raus wie im Original, obwohl die Schächte korrekt hoch gehen"

### 2.1 Lesart "Stage 4"
Typ 0x2B steht laut eigenem Zensus (`T_belege/t_emset_zensus.py 4 2b` / `5 2b`, selbst ausgefuehrt) in Spiel-STAGE4 NUR in
ROOM4001 (Elza; Schaechte grid 0x41 @0x0122C / @0x012BE) und in Spiel-STAGE5 in 5040/5041 + 5070/5071 (grid 0x41, beide
Figuren) sowie 5110/5130 (grid 0, ohne Schacht). Das Debug-Menue nennt Spiel-STAGE5 "Stage 4" (debug_jump_table.h). Da der
Nutzer in Punkt 2 "Leon" schreibt und Leon in Spiel-STAGE4 keinem Spiderman begegnet, sind BEIDE Lesarten gemessen:
ROOM4001 (Elza) und ROOM5040 (Leon). Die Schacht-Skripte setzen in allen drei Raeumen `Member_set 0C=0x42` (eigener
SCD-Dump `tools/scd_dump_room.py`: 4001 sub05 @0x015BA / sub07 @0x016F0, 5040 sub02 @0x018DA / sub05 @0x019AA,
5070 @0x00C12 / @0x00CD4). STAGE5.BIN traegt dieselbe Maschine um +0x180 verschoben (selbst disassembliert:
`ori v0,zero,0x42 / bne` @0x801155DC-E0, `jal 0x8001ac38` @0x80112C88).

### 2.2 Messung vorher (v0.8.25-exe, Elza, Sprung ROOM4001, RE15_SET_FLAG=4:20,3:33, Spieler steht)
`lauf.sh <v0.8.25> v0825a p1alt RE15_NO_INTRO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
"RE15_SET_FLAG=4:20,3:33" "RE15_DEBUG_JUMP=4000@250" "RE15_PLAYER_POS=-15608,-3524,1024" RE15_STATE_LOG=state.log
"RE15_EXIT_AT=400#4001"`:
- F30: beide 0x2B `st=4 ss1=1 ss2=1 g=41 mo=29 af=24` (Clip 0x1D laeuft sofort), F61: beide `st=1` (ACTIVE) mit g=41,
  F399: Slot 4 bei (-19875,-10234) — ausgestiegen, ohne dass der Spieler je den Ausloeser betrat. Bestaetigt den Befund
  des Bau-Agenten.

### 2.3 Messung nachher (5db78deb)
a) **Echter Weg (Tuer):** Sprung nach ROOM4011, Elza an der Tuer, Aktionstaste:
`"RE15_DEBUG_JUMP=4010@250" "RE15_PLAYER_POS=-20850,-23700,1024" "RE15_AUTOPILOT=xza:-20850,-24300" RE15_AOT_DUMP=1
"RE15_EXIT_AT=1300"` (+ Elza/Flags wie oben). debug.log: `[room] PC loaded room4011.rdt` -> `[auto] Ziel erreicht
(-20850,-23700), halte Aktion` -> `[room] PC loaded room4001.rdt` -> `[auto] Aktion hat versetzt -> (-15608,-3524)`.
AOT-Dump 4001: `4 AUTO_EVENT Mitte(-8727,-13740) halb(3000,2700) ev=5`, `5 AUTO_EVENT Mitte(-20057,-8740) halb(3050,2500) ev=7`.
1300 Bilder in 4001, Spieler steht: F1300 beide `[3/4 t=2b st=4 ss1=1 ss2=1 g=41 mo=29 af=1]` an (-8280,-16110) /
(-20160,-11070) — sie bleiben im Schacht.
b) **Hineinlaufen in den Ausloeser Aot 5** (Sprung 4001 an den Tuer-Spawn, Gehen per Skript):
`"RE15_DEBUG_JUMP=4000@250" "RE15_PLAYER_POS=-15608,-3524,1024" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
"RE15_INPUT_SCRIPT=U3,L1,U3,W20"`:
  - F218 Spieler (-17062,-9455), Slot 4 `g=41 ss2=1`; F219 Spieler (-17133,-9434) = im Aot 5: Slot 4 `g=42 ss2=2 af=1`.
  - F251 `af=33`, F252 `g=00 af=34` (Freigabe bei Bild 0x21 = 34. Austrittsbild), F253 `st=1 ss1=1` (0x101), F254 `ss1=2`.
    Das ist exakt die Zaehlung der Disassembly (P1 @0x8011544C faellt in P2, Pruefung +0x95 vor anim_set).
  - Slot 3 (Aot 4) bleibt versteckt.
  - Framedump `RE15_FRAMEDUMP=196-262/3:bilder/f` und `248-258/1:bilder/f`: F217 kein Spiderman am Schacht, F223 Deckel
    liegt am Boden, F226 Spiderman sichtbar am Schacht, F229-F252 kriecht/steht er heraus.
  - **BEFUND M1 (Darstellung):** F252 zeigt das Ende des Austritts (Clip 0x1D Bild 34, Wurzel (0,-940,0)), **F253 zeigt das
    Modell fuer EIN Bild in der Startpose von Clip 0x1D** (Wurzel (-1683,+1278,0) = 1278 Einheiten unter dem Boden und
    1683 seitlich; im Bild nur ein Rest unten links in der Wand), F254 steht er mit Clip 2 an der Ursprungslage. Ursache:
    re15_birkin_anim setzt am Clip-Ende `anim_frame = (af+1) % fc = 0`, der Zustand wechselt auf ACTIVE, der neue Clip
    kommt erst im naechsten Bild; die exe zeichnet dazwischen (0x1D, 0). Im Original berechnet FUN_8001f3bc die Pose fuer das
    aktuelle +0x95 VOR dem Hochzaehlen (RE_15_Quellcode_V2/FUN_8001f3bc.c: Pose aus `param_3`/+0x95, danach `+0x95 += 1`,
    bei `>= fc` auf 0 und Rueckgabe 1) — das letzte gezeigte Austrittsbild ist Bild 34, ein Zurueckspringen gibt es nicht.
    Wurzelwerte selbst aus der EM2B-Bank gelesen (tools/emd_ansichtsblatt.py-Leser): Clip 0x1D Bild 0 (-1683,1278,0),
    Bild 34 (0,-940,0); Clip 2 Bild 0 (0,-940,0). Der Spiderman_r38-Kollaps haelt das letzte Bild bereits (sm_anim_halten),
    der Austritt nicht. Was das Original im ersten ACTIVE-Bild tut (selbst disassembliert): Root 0x801118D0 ->
    Zustand [1] 0x80111C98 -> Entscheid-Tabelle @0x8011A0D4 / Animations-Tabelle @0x8011A114, [1] = 0x80112124: P0
    `+0x6=1`, `+0x94=1` (Clip 1), `+0x95=0`, `+0x96=0`, `+0x8F=7` (@0x80112144-84), danach `jal 0x8001f314` (@0x801121F0)
    — also Clip 1 mit 7 Bildern Ueberblendung aus der Austritts-Endpose, kein Bild (0x1D, 0). Der alte Port-Pfad (case 4,
    `if (re15_birkin_anim(e)) { e->state = 1; ...}`) hatte dasselbe Muster; der neue Austritt (re15_r38_sm_austritt,
    spiderman_r38.c:62-65) uebernimmt es.
c) **Leon ROOM5040** (`"RE15_DEBUG_JUMP=5040@250"`, Spawn (1500,-15000)): F1-F599 beide 0x2B `st=4 ss1=1 ss2=1 g=41`
   (versteckt, Spieler steht). Mit `"RE15_PLAYER_POS=1500,-15000,2048" "RE15_INPUT_SCRIPT=U9,W60"` laeuft Leon nach Westen,
   ueber den versteckten Schacht (-14493,-14685) hinweg ohne Schub (F253-F313 z konstant -15000), in Aot 4 (Mitte
   (-16489,-14869)): F285 `g=41 ss2=1`, F286 (Leon x -15525) `g=42 ss2=2 af=1`, F319 `g=00 af=34`, **F320 `st=1 ss1=1 mo=29
   af=0`** (derselbe Ein-Bild-Ruecksprung), F321 Clip 2. Slot 1 greift danach an (s. Punkt 2).
d) **Leon ROOM5070** (`"RE15_DEBUG_JUMP=5070@250" "RE15_PLAYER_POS=1700,-5700,2048" "RE15_INPUT_SCRIPT=U9.5,W30"
   "RE15_FRAMEDUMP=330-400/1:bilder/f"`): bis F341 beide versteckt (`g=41 ss2=1`), F342 (Leon x -19525, Aot 3) `g=42 ss2=2`,
   F375 `g=00 af=34`, F376 `st=1 ss1=1 mo=29 af=0`, F377 Clip 2. Framedump: F374/F375 kauert der Spiderman ueber dem
   Bodengitter hinter Leon, **F376 ist er weg (Startpose 1278 unter dem Boden)**, F377 steht er wieder am Ursprung —
   im Bodenschacht wirkt M1 als Ein-Bild-Verschwinden.

### 2.4 Urteil Punkt 1: **teilweise**
Der Mechanismus ist byte-true und an der exe gemessen: kein Austritt ohne Ausloeser (vorher F61 ACTIVE ohne Ausloeser),
Austritt erst mit grid 0x42 aus dem Raumskript, sichtbar ab Bild 6, Freigabe Bild 0x21, ACTIVE nach 35 Bildern, im
Schacht kein Ziel, kein Schatten, kein Koerperschub (Leon laeuft in 5040 auf z -15000 geradeaus ueber den Schacht, F253-F313
keine Querabweichung), in 4001/5040/5070 gleich. Es bleibt der sichtbare Ein-Bild-Ruecksprung am Ende des Austritts (M1:
F253 in 4001, F320 in 5040, F376 in 5070 — gleicher Codepfad): genau der Vorgang "aus dem Schacht kommen", den der Nutzer
bemaengelt, zeigt fuer ein Bild ein Modell, das 1278 Einheiten im Boden und 1683 seitlich versetzt steckt (Wandschacht:
Rest in der Wand; Bodenschacht: verschwunden). Das Original zeigt dort Bild 34 und blendet dann in Clip 1 ueber.

## 3. Punkt 2 — "die Spiderman führen - mal wieder - dazu das Leon teilweise komplett von der Karte verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner."

### 3.1 Vorher/Nachher an der exe, identischer Ablauf (Elza ROOM4001, in Aot 5 laufen, danach stehen bleiben)
Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
"RE15_SET_FLAG=4:20,3:33" "RE15_DEBUG_JUMP=4000@250" "RE15_PLAYER_POS=-15608,-3524,1024" RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=U3,L1,U3,W60" RE15_STATE_LOG=state.log`.
- **v0.8.25 (vorher):** `F675 PL(-26516,-7418) hp=40 gr=0 [4 t=2b ss1=9 ss2=0]` -> **`F676 PL(-724,761) hp=40 gr=1 [4 t=2b
  ss1=9 ss2=1 mo=18]`** — Griff hinten B[9], Spieler in EINEM Bild 27 058 Einheiten weit an (-724,761) versetzt, ausserhalb
  der Raumhuelle (SCA-Huelle z <= -380), stirbt dort (`F1024 PL(-384,497) hp=-1 pst=7`). Das ist "komplett von der Karte".
- **5db78deb (nachher):** `F677 PL(-26516,-7418) hp=40 [4 ss1=9 ss2=0]` -> `F678 PL(-26516,-7418) gr=1 mo=4 [4 ss1=9 ss2=1
  mo=18]`; Griff hinten F678-F766, Kollaps hinten B[10] (`ss1=10 mo=21`) ab F767, Tod F798 bei (-25936,-7875),
  danach (-26310,-7501). `sprung.py`: groesster Bild-zu-Bild-Sprung des ganzen Laufs **607 (F798, Uebergang in den
  Tod)**, alle Lagen im Westgang von 4001 (x > -26984 = Wandzelle). Opfer-Clip hinten = 4 (aca59*4 @0x80115D7C).

### 3.2 Leon ROOM5040 (Lesart Debug-Menue "Stage 4")
`"RE15_DEBUG_JUMP=5040@250" "RE15_PLAYER_POS=1500,-15000,2048" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
"RE15_INPUT_SCRIPT=U9,W60"`: Ausloeser F286, Austritt bis F320, Angriffe hp 100 -> 40, Griff hinten ab `F753
PL(-25886,-25198) gr=1 [1 t=2b ss1=9 mo=18]`, Kollaps B[10], Tod F873. Groesster Sprung 607 (F873, Uebergang in den Tod);
alle Lagen im Westgang von 5040 (x in (-26354,-21728), z > -25666 laut SCA). Kein Verschwinden.

### 3.3 Generell ("bei keinen Gegner")
- Zensus des Bau-Agenten (17 Schreibstellen) stichprobenhaft am Code nachgeprueft: alle Latch-Stellen
  (`grep re15_player_victim_latch|_ex|devour`) setzen vor/nach dem Latch einen Anker und kopieren ihn (Zombie RE1.5
  re15_enemy_ai_live_grab, Kriecher re15_zcrawl_grab_animate `re15_clip_anchor_set` + Kopie, Hund-A `pl->anchor = pl`
  @0x8010f8ec/f4, Gorilla ueber re15_affen_pin_anker, Arme 0x1A, RE2-Zombie re15_re2z_grab_anchor); die Kraehe platziert
  nicht (0x21 in re15_player_victim_tick ausgenommen); Hund-Maschine A/B (Zustand 4, +0x5 4/5/10/11) ist unerreichbar —
  selbst disassembliert: INIT schreibt nur 0x004/0x104 (`sw` @0x8010DB98/@0x8010DBB8), und der Port-Zustand 4 verlaesst
  +0x5 0/1 nur nach Zustand 1.
- Jeder Weg in re15_victim_place setzt den Bezugspunkt der neuen Wandregel neu (Latch aus 0/3 @enemy_ai_common.c:1009,
  RE2-Zombie :2629, Arme :12923, Gorilla affen_11c0.c:86, Kollaps ohne Griff :1377 neu) — ein veralteter Bezugspunkt aus
  einem anderen Raum kann die Regel also nicht zum Teleport machen (geprueft, weil die Regel bei einem Treffer die Lage
  AUF den Bezugspunkt setzt).
- Riegel unit_r38_griff_* (10 Faelle, vergifteter Anker + vergifteter Standpunkt) gruen; eigener Gegenlauf des Riegels
  gegen den alten Engine-Stand: s. 3.4.
- Nicht abgedeckt: Gator/G5/Tentakel/RE2-Gitterarm (eigene Platzierung, Dossier OFFEN 8), Wandzellen ausser Typ-1-Rechtecken
  (die Regel prueft nur `type == 1`), und die zwei Spieler-Versetzungen des RE2-Hund-Kehlbisses (Riegelfall hund2: 2554 am
  Kontakt-Latch, 3255 beim Wechsel Sub 3 -> 7; Dossier OFFEN 9, vorbestehende Port-Abweichung aus Runde 30). Keiner dieser
  Faelle ist ein "Verschwinden von der Karte"; der 3255er-Sprung ist aber ein sichtbarer Versatz um ~3 m in einem Bild.

### 3.4 Misst der Riegel? — Gegenlauf gegen den alten Engine-Stand
Scratch-Bau ausserhalb des Repos: `git archive e2cc7673 re15_port/{CMakeLists.txt,cmake,engine,include,platform,tests,
tools,targets}` + die NEUE Datei tests/unit/test_r38_griff_alle.c + eine Sonde nur fuer dieses Ziel; Bau mit dem PATH
von local_build.sh (`cmake --build ... --target test_r38_griff_alle`, 154/154). Ergebnis je Fall (alter Stand):
- spiderman_vorn: **FAIL** — `Latch Bild 0: ... Spieler-Anker (21492,19620), Greifer-Anker (0,0)`, `info: ... groesster
  Bild-zu-Bild-Sprung 45460, Anker im Latch UNVERAENDERT (Gift)`, `302 ausserhalb`, `kein Griffbild hinter einer Wand: 302`.
- spiderman_hinten: **FAIL** — Sprung 43357, 302 ausserhalb, 302 hinter einer Wand.
- kriecher2_1220: **FAIL** — `kein Griffbild hinter einer Wand: 132`, groesster Sprung 1237.
- zombie15_vorn/_hinten, zombie2_vorn/_hinten, hund15, hund2, gorilla: OK (diese Greifer kopierten ihren Anker schon).
  hund2 ist auch im alten Stand gruen und zeigt dort denselben Sprung 3255 — der Fall prueft die Kalibrierung (III),
  die erst durch die neue Wandregel noetig wurde.
Neuer Stand (5db78deb): alle zehn OK. Der Riegel faengt also genau die gemeldete Fehlerklasse am alten Code.

## 4. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py`)
Stichproben weit ueber die geforderten drei hinaus; jede zitierte Stelle enthaelt das Behauptete:
| Adresse (Binaerdatei) | Bytes/Instruktion | Behauptung im Diff |
|---|---|---|
| STAGE4 0x8011A270 (table) | [0] 0x8011502C [1] 0x801152EC [2] 0x801155AC [3] 0x80115AE8 | Austritts-Untertabelle |
| STAGE4 0x80115340 / 0x8011535C | `ori v0,v0,0x2` / `ori v0,v0,0x40` auf word0 | versteckt |
| STAGE4 0x80115370-74 | `sh zero,188(v0)` / `sh zero,190(v0)` | Schatten 0 |
| STAGE4 0x80115380-A8 | Clip 0x1C, bei `lbu v1,9(a0)`==0x41 Clip 0x1D | Clip-Wahl |
| STAGE4 0x801153F8-48 | Schleife a1=0x19..0, Stride 0xAC (`*3*4-1*4-1*4`), `and v0,v0,-2` | 26 Teile unsichtbar |
| STAGE4 0x8011545C-60 | `ori v0,zero,0x42` / `bne v1,v0,0x8011559c` (Rueckkehr ohne anim_set) | P1 wartet |
| STAGE4 0x80115468/78/88 | +0x95=0, +0x8F=7, +0x6=2, faellt in P2 | Start P2 |
| STAGE4 0x8011549C / 0x80115508 / 0x80115248 | `ori v0,zero,0x6` / `0x21` / `0x32` | Sichtbar-/Freigabebild |
| STAGE4 0x80115548-64 | `0x3e8`->+0xBC, `0x44c`->+0xBE, `sb zero,9(v0)` | Schatten an, grid 0 |
| STAGE4 0x80115588-98 | `ori v0,zero,0x101` / `sw v0,4(v1)` nach `beq v0,zero` (anim_set fertig) | ACTIVE +0x5=1 |
| STAGE4 0x8010026C/0x8010028C/0x80100324 | Phasentabellen B[5]/B[9]/B[15] (P1 = 0x80112D44 / 0x80113220 / 0x801143D8) | Phasen |
| STAGE4 0x80112B08 | `jal 0x8001ac38`, Verzoegerungsslot; a0 = s0-424 = 0x800ACA54 (Spielerblock) | Anker setzen + kopieren |
| EXE 0x8001acfc / 0x8001ad18 / 0x8001ad28-48 | `sh v0,160(a0)` / `sh v0,162(a0)` (Lage - RotY*Wurzel), dann `lhu 160/162` -> `sh` auf s2 = a0 | Anker -> Spieler |
| STAGE4 0x80112FB8/0x80113000/0x8011309C/0x801131B4 | Clip 0x12 / `jal 0x8001ac38` / 0x13 / 0x14 | Griff hinten |
| STAGE4 0x801130D4-0x801131A8 | ad68, anim_set, Tasten*100, dann +0x9E-- -> `sb 0xa,5` (B[10]); keine hp-Pruefung | Reihenfolge hinten |
| STAGE4 0x80112BDC-0x80112C68 | ad68, anim_set, +0x9E-- (==0 -> B[8]), `lh hp` `bgez` -> B[8], dann Tasten | Reihenfolge vorn |
| STAGE4 0x80114160-0x801142C4 | +0xA0/+0xA2 = Lage, +0x8C=0x66C, `jal 0x800245d8(0x800)`, `sw playerX/Z` @0x80114204/14, Lage zurueck, aca59 = a780+2 | Griff 2 einmalig |
| STAGE4 0x8011411C-0x8011448C | Aufrufe nur 0x800245d8, 0x8001a780, 0x8001f314 (2x), 0x80037024 — kein 0x8001ad68 | keine Platzierung |
| STAGE4 0x80115D7C/E5C/ED8, 0x8011628C/80116664, 0x80116044/C8/0x80116108 | `sll 2`, `+1`, `+2`, `+3`, `+3`, `0x8`/`0x9`/`0xa` | Opfer-Clips |
| EXE 0x80037104-10 | `lbu a0,-145(s0)` / `andi v0,a0,0x60` / `bne` | Aim-Latch-Sperre |
| STAGE4 0x80111C14-20, 0x80111DCC-E24 | word0 &= 0x1FFFFFFF; ACTIVE-Schwanz stempelt 0x40000000 nur bei grid&0x18==0, Clip>=2 | Hoehenband |
| STAGE4 0x801118D0 (Root) | Zustandstabelle @0x8011A0B4, aec4/b544, `jal 0x8003b0a4` UNBEDINGT mit Maske 4 (8 bei grid 0x10) | Root |
| STAGE5 0x801155DC-E0 / 0x80112C88 | `ori v0,zero,0x42 / bne` / `jal 0x8001ac38` | STAGE5 = STAGE4 +0x180 |
| RE2 PSX.EXE 0x800356D0 / 0x800356EC-F8 | `jal 0x8004c1bc`; `lw v0,132(s0)` `lw v1,140(s0)` `sh v0,280(s0)` `sh v1,282(s0)` | Bezugspunkt je Bild |
| RE2 PSX.EXE 0x8004C370-A0 | `lh/lhu/subu/addu/sltu/beq` je Achse | Punkttest |
Zusatz: ROOM4001-Zelle (-27830,-11990,11628,1638) traegt u0 = 0xFB (Bit 0x4 fehlt) — die Gegnermaske 4 des Roots
laesst den Spiderman also im Schacht IN der Wand stehen; die Spielermaske (Bit 1) nicht. Passt zum Port (Wandregel
prueft u0 & 1).

Rate-Woerter im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|geraten|vermutlich`): keine.
Einziger getenv im Diff: `R38_GRIFF_LOG` im Test (Logausgabe). Die einzige Regel ohne Original-Adresse
(re15_r38_griff_durch_wand) ist als NUTZER-VORGABE gekennzeichnet und fuehrt keine neue Zahl ein (Form: Strecke gegen
Typ-1-Zellen des Spielerbandes). Commit-Message 5db78deb traegt die Adressen.
Erklaert der Fix den Befund? Ja fuer Punkt 2: vorher F676 Versatz auf (-724,761) = Wurzelversatz um einen fremden Anker,
nachher Anker == Greiferanker im Latch (Riegel) und 0 Bilder ausserhalb (exe). Fuer Punkt 1: vorher fehlte das Warten
auf grid 0x42 (F30 Clip 0x1D ohne Ausloeser), jetzt P1 @0x8011545C.

## 5. Vertrag
- Pfade (`git diff master...HEAD --name-only`): engine/src/enemy_ai_common.c (+27/-70), engine/src/re15_damage.c (+3),
  engine/src/spiderman_r38.c (neu), include/re15_spiderman_r38.h (neu), platform/pc/main.c (+2),
  tests/integration/test_r37_beretta.cmake (+10/-3), tests/unit/probes/r38_spiderman.cmake (neu), tests/unit/test_r38_*.c
  (neu), analysis/befunde_runde38/T_* (eigen). Kein release/, platform/android/, shared_assets/, keine Test-CMakeLists.
- Gemeinsame Dateien: enemy_ai_common.c (T zugeteilt) — 10 Haken-Stellen zu je 1-9 Zeilen (Include, Clip-Map, Kollaps-Bezugspunkt, Wandregel, 3x Opfer-Tick, Zugangsblock 7 Z., Tyrant-Faelle 5 Z., Austritt 1 Z.) mit "Runde 38 Spur T", die alte
  Tyrant-Griff-Logik (B[5]/B[8]/B[9]/B[15], ~66 Zeilen) ist in spiderman_r38.c verlegt; main.c 2 Zeilen;
  re15_damage.c 3 Zeilen (steht nicht in §1.4, ist aber keiner Spur zugeteilt — Hinweis fuer die Integration mit R/S).
- Bank-9-Bits, Nachrichten-Ids, Items: keine belegt.
- Alt-Riegel integration_r37_beretta t2b: Szenario auf den draussen stehenden Spiderman umgestellt (Ck(4,0xD6)=1,
  Sce_em_set @0x017D4); Absicht (drei Abzuege je -10) unveraendert, gruen. Begruendet durch das byte-true Verstecken.
- Riegel je Punkt: Punkt 1 unit_r38_spiderman (austritt), Punkt 2 unit_r38_spiderman (griff1/griff2) + 10x
  unit_r38_griff_*. Beide Szenarien: Elza 4001 und Leon 5040 an der exe gemessen.
- Hinweis: RE15_MIN_TESTS in tools/local_build.sh (788) wurde nicht angehoben (Suite im Spurbaum 799) — Konvention des
  Skripts, kein Fehler der Spur.

## 6. Urteil

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| 1 Spiderman kommen richtig aus den Schaechten | **teilweise** | Warten auf grid 0x42 / sichtbar Bild 6 / frei 0x21 / ACTIVE an der exe in 4001 (Tuer + Ausloeser), 5040, 5070 gemessen, vorher (v0.8.25) F61 ACTIVE ohne Ausloeser; RE-Belege selbst disassembliert. Rest: M1. |
| 2 Leon verschwindet nicht von der Karte, bei keinem Gegner | **erfuellt** | v0.8.25 F676: Sprung 27 058 auf (-724,761) ausserhalb; 5db78deb gleicher Ablauf F678: Spieler bleibt (-26516,-7418), groesster Sprung 607 (Tod); 5040 Leon ebenso. Riegel faengt den alten Code (spiderman 302/302 ausserhalb, kriecher2 132 hinter der Wand), neuer Code 10/10. |

Gates: Bau gruen; gezielte Tests 188/188; RE-Gate gehalten (Abschnitt 4, keine Rate-Woerter, Wandregel als NUTZER-VORGABE
gekennzeichnet); Vertrag gehalten (Abschnitt 5).

**bestanden: NEIN** (Punkt 1 nur teilweise).

### Maengel
- **M1 (Punkt 1, Darstellung, urteilsrelevant): Ein-Bild-Ruecksprung am Ende des Austritts.** spiderman_r38.c:62-65
  (`if (re15_r38_birkin_anim(e)) { e->state = 1; e->sub_state_1 = 1; ... }`) laesst `anim_frame` am Clip-Ende auf 0
  springen; die exe zeichnet im Bild des Zustandswechsels Clip 0x1D Bild 0 (Wurzel (-1683,+1278,0)) statt Bild 34.
  Nachpruefbar: ROOM4001 Lauf 2.3 b) `F253 [4 t=2b st=1 ss1=1 ss2=0 mo=29 af=0]` + Framedump F252/F253/F254;
  ROOM5040 `F320 ... mo=29 af=0`; ROOM5070 `F376 ... mo=29 af=0`, Framedump F374-F377 (Spiderman fuer ein Bild weg).
  Original: FUN_8001f3bc berechnet die Pose fuer das aktuelle +0x95 vor dem Hochzaehlen; im naechsten Bild setzt ACTIVE
  (Entscheid @0x8011A0D4, Animation @0x8011A114) den neuen Clip mit +0x8F = 7 — [1] 0x80112124: Clip 1 @0x80112154,
  +0x8F @0x80112184; [2] 0x80112294: Clip 2 @0x801122F0, +0x95 = 0 @0x80112300, +0x8F = 7 @0x80112320 — es gibt kein
  Bild (0x1D, 0). Ein Riegel, der das gezeichnete Bild im
  Wechselbild prueft (motion/anim_frame im Bild des Zustandswechsels == (0x1D, letztes Bild)), fehlt in unit_r38_spiderman.
- **M2 (gering, nicht urteilsrelevant, neuer Code): derselbe Ruecksprung an den Phasenwechseln der neuen Griffe.**
  spiderman_r38.c P1 -> P2 von Griff 1 (`if (re15_r38_birkin_anim(e)) e->sub_state_2++;` Z. 147) zeigt ein Bild
  Zugriffs-Clip Bild 0: ROOM4001 Lauf 3.1 `F711 [4 t=2b ss1=9 ss2=2 mo=18 af=0]` zwischen `F710 mo=18 af=33` und
  `F712 mo=19 af=1`. Original: P1 0x80112D44/0x80113220 rechnet das letzte Bild, P2 0x80112B9C/0x80113094 setzt den
  Halte-Clip und faellt in P3 (ad68 + anim_set mit Bild 0 des Halte-Clips). Gleiches Muster bei Griff 2 P1->P2 und den
  Loese-Clips (P5 -> P6) — dort nicht an der exe gemessen (Griff 2 unerreichbar, kein Losreissen im Lauf).

### Beobachtungen (keine Maengel dieser Spur, fuer NACHLAUF/Integration)
- **B1 Koerperschub ueber den ganzen Gang.** Nach dem ersten Treffer laeuft der Spiderman mit B[3] (Clip 3) weiter und
  schiebt die stehende Figur vor sich her: 4001 F348-F382 von x -21257 nach -26507 (5250 Einheiten, 128-288 je Bild),
  5040 F426-F581 von (-19498,-15015) um die Ecke bis (-25886,-24989) (~15 800 Einheiten Weg, unterbrochen von einem Treffer F475), erst an der Endwand folgt der
  naechste Angriff. Identisch in v0.8.25 (4001 F350-F382, gleiche Endlage (-26516,-7418)) — vorbestehend, kein
  "Verschwinden". Ungepruefte Hypothese: die Angriffs-Schwelle von B[3] liegt unter dem Schub-Abstand (Muster wie
  re15_body_contact_reach); gegen das Original (B[3] Entscheid 0x801123E8 / Animation 0x801125DC) NICHT geprueft.
- **B2 RE2-Hund-Kehlbiss:** zwei Spieler-Versetzungen 2554 / 3255 in je einem Bild (Riegelfall hund2, im alten und neuen
  Stand gleich) — im Raum, nicht hinter einer Wand; Dossier OFFEN 9. Sichtbarer 3-m-Sprung, gegen das Original offen.
- **B3** Riegel und Wandregel decken Gator/G5/Tentakel/RE2-Gitterarm (eigene Platzierung) und Nicht-Rechteck-Zellen nicht
  ab (Dossier OFFEN 8; Regel prueft nur `type == 1`).
- **B4** Schattengroesse draussen 500x600 statt 0x3E8/0x44C (@0x80115548-54, Root-Schwanz @0x801119D4-F4) — Dossier OFFEN 5.
- **B5** Griff 2 schreibt den eigenen Anker +0xA0/+0xA2 = Lage (@0x801141B4-D4) nicht; ohne Wirkung, solange B[15] nicht
  platziert und der Deckenlauf (OFFEN 1) fehlt.
- **B6** Der Port zeigt Austritt/Griff-Clips allgemein ein Bild "voraus" (re15_birkin_anim zaehlt vor dem Zeichnen; erstes
  sichtbares Bild ist Pose 7 statt 6) — portweite Konvention dieses Anim-Treibers, keine Spur-Aenderung.
- **B7** re15_damage.c steht nicht in VERTRAG §1.4 (3 Haken-Zeilen) — bei der Integration mit R/S auf Konflikte achten;
  enemy_ai_common.c-Haken mit Spur B zusammenfuehren (Dossier). RE15_MIN_TESTS (local_build.sh) nicht angehoben.
- **B8** Zum relayten Nutzer-Nachtrag (ROOM20A0 Rolladen vor der Kamera): gehoert laut Orchestrator (df08d83a) zu Spur W,
  nicht zu T; hier nicht geprueft.
