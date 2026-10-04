# Runde 35 — Ressourcen-Vertrag und Arbeitsregeln (verbindlich fuer alle 14 Spuren)

Zweck: 14 Spuren bauen parallel in eigenen Arbeitsbaeumen (`.claude/worktrees/r35_<spur>`, Zweig
`r35/<spur>`, Basis master e6783b27 = 19f32749 + dieser Auftrag). Jede Spur nimmt NUR die hier zugeteilten
Plaetze. Wer mehr braucht: im eigenen Dossier begruenden, aus dem eigenen Reservebereich nehmen,
NIE aus dem einer anderen Spur. Dossier je Spur: `analysis/befunde_runde35/<SPUR>_<thema>.md`
(ZUERST anlegen, fortlaufend schreiben — [[Sitzungslimit bricht Agenten ab]]).

## 0. Die Spuren (Auftrag = AUFTRAG.md, woertlich)

| Spur | Thema | Auftrags-Punkte |
|---|---|---|
| A | granate | Granate nicht durch Waende; Explosions-Reichweite; Explosionssound; mehr Brutalitaet der Handgranate (Zerplatzen, abplatzende Beine/Arme); Tests Birkin + Alligator |
| B | werfer | Granatwerfer + Raketenwerfer gehen nicht; Flammenwerfer + Colt Python gehen nicht richtig |
| C | zgirl | "Was sollen Zombie Maedchen sein?" — Typ 0x13 (EM013) erklaeren, Vorkommen zensieren, vollstaendig portieren (v0.8.21-Grenze: "erscheint im Port in ROOM4050 noch nicht") |
| D | redhawk | Super Redhawk auf Hunde: Fleisch-Effekte bleiben permanent im Loop |
| E | inventar1050 | Inventar oeffnet nach der Ada-Szene in ROOM1050 nicht mehr; Kampfmesser raus aus dem Inventar (immer Rueckfall ohne Waffe); ROOM1050-Szene: Leon redet mit sich selbst wie ROOM1170, nicht zum Spieler |
| F | inhalt | Doppeltueren der Tuersequenzen unsymmetrisch (Griffe); Codes in den selbsterstellten Dokumenten GRUEN; ROOM1010/ROOM1220 stehende Zombies weiter von der Tuer; Memory Card im Regal ROOM1010 (add_card); Schrot-Munition auf dem Aussenluefter ROOM1090 (Shotgun.bmp); RE2-Karten-Weltmodelle (World Items) extrahieren und ablegen |
| G | karte | ROOM1080 Fahrstuhl: Spieler-Cursor bewegt sich nicht; ROOM11F0 und ROOM1200 erscheinen nicht, wenn man drin ist; ROOM1230 zeigt die Karte von ROOM11E0; ROOM1210 Korridor falsch + Tueren fehlen |
| H | raeume | ROOM1190 Hunde-Schatten waehrend des Sprungs durch die Luke; ROOM1190 Zielscheiben-Texte (4 Scheiben); ROOM1200 Trage-Zombie laeuft nach dem Minidisc-Player durch die Luft; ROOM1210 Gitterarme beim Griff nicht synchron zu Leon (Clipping) |
| I | entladen | Nach Tod + New Game bleiben PRIs des alten Spielstands; beim Tod UND beim Raumwechsel saemtliche Assets der vorherigen Raeume entladen |
| J | affen | ROOM11C0: Ada verschwindet nach ihrer Szene nicht (verstecken) und kommt nach dem Sieg nicht zurueck; Monkey kommt an falscher Position aus dem Auto; komisch beweglicher Oberkoerper-Teil; KI nicht zielstrebig/aggressiv wie Original; Brust-Schlag-Animation fehlt; Sprung erst nach 3 Treffern |
| K | cut10f0 | Neue Szene ROOM10F0 (Ada/Leon/Marvin, kompletter Dialog); danach Karte: ROOM11C0 markiert, dann ROOM1150 blinkend, beide bis besucht; MAIN01 als Hintergrundmusik bis zum Parkplatz |
| L | cut1150 | Dialog ROOM1060->ROOM1040 "I have to get the Chief first..." solange ROOM1150 nicht besucht; Irons-Todesszene ROOM1150 (Dialog, Arm, Knien); danach Knaelle + Zombie-Spawns ROOM1130 (Rest aus 1140), ROOM1040 (Rolltor + 5), ROOM1030 (Rest aus 1070, Cut 7; Kriechen durchs Tor Cut 6) |
| M | cut11c0_fenster | ROOM1120 Cut 1: Scheiben zerbrechen wie RE2 (Glas-Effekt + Knall aus RE2 extrahieren), Kraehe fliegt herein, hinteres Fenster im Hintergrund beschaedigt. (Die Szene ROOM11C0 Cut 13 Ada+Marvin ist der letzte Schnitt der 1150-Montage und gehoert zu Spur L; der Baum heisst aus historischen Gruenden r35_cut11c0_fenster.) |
| N | android | Fortschrittsanzeige auf sehr breiten Displays abgeschnitten; Datei<->Ordner-Konflikt beim Update bricht sauber ab, erst 2. Start heilt; Pruefskript-Urteilslogik selbst mittesten |

Reihenfolge der Zustaende K -> L ist eine Geschichte: K setzt das Flag "10F0-Szene gesehen" (9,71).
Der Nutzer sagt "Betritt man ROOM 1150 kommt eine weitere Cutscene" im Anschluss an K; L gatet
seine Szene auf (9,71)=1 UND (3,94)=1 (erste Irons-Szene gelaufen) und setzt (9,73). Die Montage
von L endet mit dem Schnitt nach ROOM11C0 Cut 13 (Ada/Marvin: "What was this noise?" ... "Marvin!")
— das ist EIN Ereignisprogramm, deshalb liegt dieser Schnitt bei L und nicht bei M. Die
Ada-Marvin-Vorszene von 11B0 sub06 (Spur J) bleibt unberuehrt. Jede Spur liest die Flags der
anderen nur ueber die hier vereinbarten Nummern.

## 1. Zuteilung

### 1.1 Persistente Zustandsbits — Bank 9 (gespeichert mit g_game.flags)
Belegt (Zensus 2026-10-03, re15_port/include/*.h): 53 Sicherung, 54 Irons Diary, 55 Memory Card
(Hebetisch), 56 Granate, 57-60 Dokumente, 61/62 Leichen-Munition, 63 Sicherung eingesetzt,
65 Ada-Ruf gesehen. Frei: 64, 66-84.

| Bits | Spur | Verwendung |
|---|---|---|
| 71, 72 | K | 71 = "10F0-Szene gesehen" (Kartenmarken blinken ab hier), 72 Reserve |
| 73, 74, 75, 76, 77, 78 | L | 73 = "Irons-Todesszene gesehen" (= Spawns ausgeloest), 74 = Spawn 1130 verbraucht, 75 = Spawn 1040 verbraucht, 76 = Spawn 1030 verbraucht, 77 = "1060->1040-Dialog" nur falls noetig, 78 Reserve (frueher M: 11C0-Schnitt, jetzt Teil von 73) |
| 79 | M | 79 = "1120-Fenster zerbrochen" |
| 80, 81 | F | 80 = Memory Card ROOM1010 genommen, 81 = Schrot-Munition ROOM1090 genommen |
| 82 | J | Reserve (Ada versteckt/zurueck — bevorzugt aus dem RAUMSKRIPT ableiten) |
| 83 | E | Reserve |
| 84, 64, 66-70 | — | frei, nicht vergeben |

Vorhandene Original-Flags duerfen GELESEN werden ((3,94) erste Irons-Szene, (3,0xBB) Ada gerettet,
(3,121) Rolltor 1050 offen, (3,0x43) beide 11C0-Bosse tot, Zone 4/7/8 Gegner-Tot-Bits, Zone 1/27+2/7
Besucht-Latches). Neue Bits in anderen Baenken nur mit vollstaendigem Zensus (alle 240 RDTs +
Port-Code) im Dossier — und dann als Abweichung vom Vertrag markieren.

### 1.2 Portseitige Nachrichten-IDs (re15_msg_install_text; PSX-Tabelle MSG_TABLE_N = 32 -> IDs < 32)
RDT-Nachrichtenzahl je Raum (rdt_msgdump.py, 2026-10-03): 10F0=6, 1150=15 (20/21 vergeben an
Hebetisch-Cursor), 11C0=10, 1120=9, 1060=1, 1040=2, 1130=6, 1030=18, 1010=3, 1090=10, 1190=6,
1050=9 (20..27 vergeben: Rolltor 20/21, Ada-Ruf 22..25, Dokumente 26/27), 1110 (20/21), 1230 (22/23).

| Raum | Spur | IDs |
|---|---|---|
| ROOM10F0/10F1 | K | 6..30 (Ada/Leon/Marvin-Zeilen) |
| ROOM1150/1151 | L | 22..31 (Irons-Szene; 20/21 sind der Hebetisch) |
| ROOM1060/1061 | L | 1..5 ("I have to get the Chief first...") |
| ROOM1130, 1040, 1030 | L | nur falls noetig, ab RDT-Zahl, max +4 je Raum |
| ROOM11C0/11C1 | L (Schluss-Schnitt Ada/Marvin der 1150-Montage) 10..19; J (Ada verstecken/zurueck) 20..23 |
| ROOM1120/1121 | M | 9..15 |
| ROOM1190/1191 | H | 6..11 (Zielscheiben) |
| ROOM1010/1011, ROOM1090/1091 | F | ab RDT-Zahl, max +3 je Raum (Memory Card / Munition) |
| ROOM1050/1051 | E | 28..31 (nur falls die Szene neue Zeilen braucht) |

Sprachdateien fuer neue Zeilen gehoeren nach `synchro/STAGE1/room<RAUM>/main<ID>.wav` — der Port
bindet sie automatisch ein, der Nutzer produziert sie selbst (MiniMax). Neue Zeilen laufen ohne
Datei stumm mit Untertitel; die Spur nennt im Dossier die Dateinamen, die der Nutzer aufnehmen muss.
Untertitel-Sprecher: Ada vor der Vorstellung = "Woman:" (Nutzer-Konvention ROOM1090), danach "Ada:".

### 1.3 AOT-Slots (je Raum; 48..63 sind Kamerazonen) und Ereignis-Nummern
Jede Spur macht VOR dem Belegen einen Zensus des Raums (main00 + alle subs, Port-Installer in
scd_room_setup.c) und schreibt ihn ins Dossier. Vergeben (Port): ROOM1050 Slots 11/12 Rolltor,
13/14 Ada-Ruf, 15/16 Dokumente; ROOM1150 11/12 Hebetisch-Cursor. Ereignisse (Port-Programme ueber
scd_event_fire, < 32, nicht die RDT-Subs des Raums): ROOM1050 2 (Rolltor), 13 (Ada-Ruf); ROOM1150 4
(Hebetisch-Cursor). Vorschlag: K = Ereignis 20 in 10F0, L = 21 (1150), 22 (1060), 23 (11C0-Schnitt,
falls als eigenes Programm im Raum 11C0 noetig), M = 24 (1120), J = 25 (11C0), E = 26 (1050),
H = 27 (1190), F = 28 (1010), 29 (1090).

### 1.4 Dateien — Konfliktvermeidung beim Zusammenfuehren
* Neue Logik in NEUEN Dateien `engine/src/<thema>_<raum>.c` + `include/re15_<thema>.h` (Vorbild:
  adaruf_1050.c / tuer1120_1130.c / dokumente_r34.c / leiche_1110_1230.c). CMake sammelt
  `engine/src/*.c` per GLOB — Configure neu laufen lassen.
* Gemeinsame Dateien (scd_room_setup.c, scd_vm.c, game_step_common.c, enemy_ai_common.c,
  re15_esp.c, main.c, re15_map_zones.h, msg_common.c, menu_common.c): NUR kleine Haken (1-5 Zeilen)
  an klar benannten Stellen; Reihenfolge der Installer in scd_room_setup.c: am Ende des Blocks
  anhaengen, jede Spur eine eigene Zeile mit Kommentar `Runde 35 Spur X`.
* Tests: NEUE Sonden/Pins in `tests/unit/probes/r35_<spur>.cmake` (GLOB-Include) + eigene
  `tests/unit/test_r35_<spur>*.c` / `tests/integration/test_r35_<spur>.cmake`. NICHT
  tests/unit/CMakeLists.txt editieren. Pins, die die echte exe starten: Muster
  tests/integration/test_r34n_b_cursor.cmake + spiel_lauf.cmake.
* `release/RELEASE_NOTES.md`, `release/*`, `platform/android/*`: NUR Spur N (Android) und der
  Orchestrator. Alle anderen Spuren fassen `release/` NICHT an.
* Assets: KEIN Patch an `shared_assets/PSX/*` (RDT/BSS bleiben byte-true). Neue Daten als
  eingebackene `gen/*.inc` oder als neue Dateien unter `shared_assets/RE2/` / `shared_assets/RE15DOOR/`
  (dann im Dossier nennen — der Orchestrator traegt sie ins Paket-/Android-Gate ein).

### 1.5 Bauen und Testen im eigenen Baum
```
cd <baum>
bash re15_port/tools/local_build.sh configure     # einmal (GLOB), danach build/test/all
bash re15_port/tools/local_build.sh all           # Suite; Schranke RE15_MIN_TESTS=478 gilt
```
local_build.sh beendet NUR exe-Prozesse aus dem eigenen Bauverzeichnis (Pfadfilter) — fremde Laeufe
bleiben am Leben. Der Bau braucht ~2-4 min, die Suite ~4-8 min; unter Last parallel laufender Baeume
flattern die fuenf GUI-Haken (integration_boot_bg_pin, dark_start_pin, relatch_pin,
save_counter_pin, weste_load_pin) — faellt einer davon, NICHT als Regression werten, sondern
einzeln nachfahren (`ctest --test-dir re15_port/build -R "^<name>$"`), 2-3x. Zwei Builds im selben
build/ gleichzeitig: NIE.
Der schlanke Baum (sparse-checkout cone) traegt re15_port, analysis, alle Quellcode-/Overlay-
Verzeichnisse, info/Re1.5, info/re2leon, voice, synchro, stage_saves, release, tools, psx_dev u.a.
NICHT dabei: `info/Resident_Evil_und_Playstation_Information/` und `.agent_refs/` — die liest man
direkt aus dem Hauptbaum `C:/workspace/git/reAi_v2/...` (nur lesen, nie schreiben; der Hauptbaum
gehoert dem Orchestrator). Keine Junctions/Links in den Baum legen (zerstoert den sparse index).
Die Ghidra-Dumps `ghidra1_V2.txt` / `ghidra_re2_Leon.txt`, die Wissensdateien `RE15_*.md` und
`RE_15_Quellcode_*`/`RE2_Quellcode_*` liegen im Baum (Wurzel-Dateien des cone);
`re15_disasm.py`/`re2_disasm.py` in `.claude/skills/re15-psx-disasm/scripts/` funktionieren dort.

### 1.6 Commits
Frueh und in Schritten committen (`wip(r35-<spur>): ...` ist erlaubt, am Ende `fix/feat(r35-<spur>): ...`).
Jede Konstante traegt im Code UND in der Commit-Message ihre `@0x...`-Adresse bzw. den Datei-Byte-
Offset; Nutzer-Vorgaben (Positionen, Texte, Szenen-Choreografie) sind als **NUTZER-VORGABE /
PORT-WAHL** gekennzeichnet, aber ihre FORM (Opcodes, Clips, Handler) ist belegt. Commit-Messages mit
Rohbytes/Backticks per `git commit -F <datei>` ([[commit-backticks]]). Nur die eigenen Pfade adden
(`git add <pfade>`), nie `git add -A`.

## 2. Regeln, die fuer jede Spur gelten (Kurzfassung, Details CLAUDE.md)
1. STOP-GATE: MESSEN/REPRODUZIEREN -> ORIGINAL disassemblieren (RE1.5, oder RE2 wo RE1.5 unfertig
   ist) + Adresse ins Dossier -> DANN Code -> VERIFIZIEREN gegen die Disasm-Werte.
2. Beta -> Retail: wo RE1.5 nachweislich unfertig ist (Stub, jalr-0, fehlendes System), ist RE2
   Retail das Ziel; der Beleg kommt dann aus RE2 (`info/re2leon/`, `ghidra_re2_Leon.txt`,
   `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`, `RE2_Quellcode_*`). Sound ist RE2.
3. Nutzer-Beobachtung schlaegt Code-Hypothese; "sieht richtig aus" ist kein Beleg; die Abnahme misst
   am gebauten Stand gegen den WORTLAUT des Nutzers (AUFTRAG.md).
4. Kein "deferred/faithful-line/tunable/Env-Schalter" als Abschluss. Was offen bleibt, steht mit
   Adresse und naechstem Messweg im Dossier unter OFFEN — nicht still im Code.
5. Das Dossier ist das Produkt der Ermittlung; die Abnahme eines anderen Agenten liest es und misst
   nach. Abschlussantwort < 15 Zeilen, alles Wesentliche steht in der Datei.

## 3. Hinweis des Orchestrators fuer Spur C (2026-10-04, aus Spur H gemessen)
Spur H (ROOM1200 Trage-Zombie) fand: der Port hatte die Engine-Schwerkraft FUN_8001bd60(-10, 0x14)
nie umgesetzt (Absturzkante @0x8001bdb0-be54, Fall @0x8001be78-e8; Aufruf z.B. `jal` @0x80100514).
Spur H portiert sie fuer den Trage-Zombie (Zweig r35/raeume, lesbar ueber
`git -C <baum> show r35/raeume:<pfad>`; Dossier analysis/befunde_runde35/H_raeume.md). Weitere
Aufrufer laut Spur H: Zombie-Maedchen @0x8010a9b8 und @0x8010c288. Wenn die Zombie-Maedchen-KI
diese Routine braucht: dieselbe Port-Funktion von Spur H verwenden (nicht doppelt bauen) und die
Abhaengigkeit im Dossier fuer die Zusammenfuehrung vermerken.
