# Runde 34 Nacht — Ressourcen-Vertrag und Arbeitsregeln (verbindlich fuer alle Spuren A..G)

Zweck: drei Spuren (A, D, E) arbeiten in ROOM1050, sieben Spuren bauen parallel in eigenen Baeumen.
Jede Spur nimmt NUR die hier zugeteilten Plaetze. Wer mehr braucht: im eigenen Dossier begruenden,
aus dem eigenen Reservebereich nehmen, NIE aus dem einer anderen Spur.

## 1. Zuteilung

### 1.1 Persistente Zustandsbits — Bank 9 ("Zone-9"-Bits, gespeichert)
Grundlage: Zensus ueber alle 240 RDTs (include/re15_irons_tisch.h: 85 von 256 Bits belegt, freier Block
53..84). Portseitig vergeben: 53 Sicherung, 54 Irons Diary, 55 Memory Card, 56 Granate (Hebetisch).
Gegenprobe 2026-09-30: kein Port-Code und keiner der r34g_*-Baeume nutzt Bank-9-Bits >= 57.

| Bits | Spur | Verwendung (Vorschlag, Spur legt fest) |
|---|---|---|
| 57, 58, 59, 60 | E | "genommen" Dokument 1050 / 1000 / 1020 / 1010 |
| 61, 62 | F | "Munition genommen" Leiche 1110 / Leiche 1230 |
| 63, 64 | A | "Sicherung eingesetzt" (+1 Reserve) |
| 65, 66 | D | "Ada-Ruf-Szene gesehen" (+1 Reserve) |
| 67, 68 | B | Reserve Hebetisch-Cursor |
| 69, 70 | C | Reserve Generator |
| 71..84 | — | frei, nicht vergeben |

Vorhandene Original-Flags duerfen GELESEN werden (z.B. (3,0x6E) Ada gerettet, (3,121) Rolltor offen,
(3,0x85) Feuerloescher, (5,13..22) Generator-Schalter, (4,238) Generator geloest). Neue Bits in anderen
Baenken nur mit vollstaendigem Zensus (alle 240 RDTs + Port-Code) im Dossier — und dann im eigenen
Dossier als Abweichung vom Vertrag markieren.

### 1.2 AOT-Slots (je Raum; 48..63 sind Kamerazonen)
ROOM1050 belegt 0..10 (Tueren 0..5, Slot 4 = Tuer nach ROOM10A0, Slot 5 = Tuer nach ROOM1090,
Slot 6 Item_aot_set, Slot 7 = Rolltor-Schalter sce 3 @0x00C22, 8..10 Texte).

| Raum | Spur | Slots |
|---|---|---|
| ROOM1050/1051 | A | 11, 12 |
| ROOM1050/1051 | D | 13, 14 (oder Umwidmen von Slot 4 wie tuer1120_1130.c) |
| ROOM1050/1051 | E | 15, 16 |
| ROOM1150/1151 | B | 11, 12 (7/8 = Irons-Tisch, vorher Zensus pruefen) |
| ROOM11F0/11F1 | C | 40, 41 (nur falls noetig) |
| ROOM1000, 1010, 1020 | E | frei waehlbar nach Zensus (je Raum dokumentieren) |
| ROOM1110, 1230 | F | frei waehlbar nach Zensus (je Raum dokumentieren) |

### 1.3 Portseitige Nachrichten-IDs (re15_msg_install_text)
PSX-Tabelle hat nur 32 Eintraege (msg_common.c MSG_TABLE_N), Sprachausgabe nur 0..63
(synchro/README.md) -> neue IDs < 32. ROOM1050 hat im RDT msg 0..8.

| Raum | Spur | IDs |
|---|---|---|
| ROOM1050/1051 | A | 20, 21 (msg 2 "I need a fuse to run the shutter." ist ORIGINAL und wird benutzt) |
| ROOM1050/1051 | D | 22, 23, 24, 25 |
| ROOM1050/1051 | E | 26, 27 |
| ROOM1150/1151 | B | 20, 21 (vorher gegen die RDT-Nachrichtenzahl pruefen) |
| ROOM11F0/11F1 | C | 28, 29 (nur falls noetig, vorher pruefen) |
| ROOM1110, ROOM1230 | F | 20..23 (vorher pruefen) |
| ROOM1000, 1010, 1020 | E | 20..23 (vorher pruefen) |

Sprachdateien fuer neue Zeilen gehoeren nach `synchro/STAGE1/room1050/main<ID>.wav` — der Port bindet
sie automatisch ein. Der Nutzer produziert sie selbst (MiniMax). Neue Zeilen laufen ohne Datei stumm mit
Untertitel; die Spur nennt im Dossier die Dateinamen, die der Nutzer aufnehmen muss.

### 1.4 Dokumente (Spur E)
Irons Diary = Dokument 0 = Item 0x48 = Bildsatz FILE25 (re15_files.c, tools/re2_doc_satz.py).

| Dok | Item | Bildsatz | Titel | Papier-Vorlage (Nutzer) | Weltmodell (Nutzer) | Raum |
|---|---|---|---|---|---|---|
| 1 | 0x49 | FILE26 | Police Officer's Final Diary Entry | FILE00_title_paper | mesh00_0541704e | ROOM1050 sitzende Leiche vor dem Rolltor |
| 2 | 0x4A | FILE27 | Elliot's Diary | wie Irons Diary | "das gleiche wie bei Irons Diary" | ROOM1000 Bank (elliot.bmp) |
| 3 | 0x4B | FILE28 | Marvin's Notes | FILE02_title_paper | mesh01_ae2d0a30 | ROOM1020 Marvins Tisch (marvin.bmp) |
| 4 | 0x4C | FILE29 | Armory Notice | FILE06_title_paper | mesh04_cab7b32d | ROOM1010 Verhoertisch (interrogation.bmp) |

Speicherstand hat files[24] (v9) — kein neues Format noetig. Spur E prueft das.

### 1.5 obj_ids (Props) — je Raum nach Zensus (RDT-Byte 2 nOmodel + Port-Props), RE15_SCD_MAX_PROPS = 17.
ROOM1150/1151: 0..3 RDT, 4 Sicherung, 5 Diary, 6 Memory Card, 7 Granate -> B nimmt 8 (falls noetig).
ROOM1050: E waehlt nach Zensus (erste freie Nummer), A und D legen KEINE Props an ohne Absprache im Dossier.

## 2. Datei-Hoheit

Jede Spur legt ihre Logik in EIGENE neue Dateien: `re15_port/engine/src/<thema>_<raum>.c` +
`re15_port/include/re15_<thema>.h`, Werkzeuge unter `re15_port/tools/<thema>/`, Sonden unter
`re15_port/tests/unit/probe_r34n_<code>_*.c` registriert in `re15_port/tests/unit/probes/r34n_<code>_<thema>.cmake`,
Integrations-Haken `re15_port/tests/integration/test_r34n_<code>_*.cmake`.
Gemeinsame Dateien (scd_room_setup.c, aot_common.c, scd_vm.c, msg_common.c, game_step_common.c, main.c,
menu_common.c, re15_files.c, audio_pc.c, render_pc.c, bg_pc.c) nur mit MINIMALEN Haken-Zeilen (Aufruf in
das eigene Modul) — so bleiben die Zusammenfuehrungen konfliktarm. Keine Umformatierung fremder Zeilen.
Spur C besitzt panel_zeiger_common.c / re15_panel_zeiger.h; Spur B benutzt sie nur lesend (Klickton,
Cursor-Vorbild) — braucht B dort eine Aenderung, als eigene Funktion in einer B-eigenen Datei.

## 3. Arbeitsregeln (aus Memory, hart)

1. RE-GATE (CLAUDE.md): jede Verhaltenskonstante mit `@0x…`-Adresse oder Datei-Byte-Offset — im Code-
   Kommentar UND in der Commit-Message. Wo der Nutzer etwas NEU wuenscht (kein Original-Gegenstueck):
   als "NUTZER-VORGABE (Runde 34 Nacht, woertlich: …)" bzw. "PORT-WAHL, keine Original-Adresse —
   Grund: …" kennzeichnen, und die Ausfuehrung (Laufwege, Ton, Text-Form, Glyphen, Kamera, Modell) so weit
   wie moeglich aus RE1.5 bzw. RE2 belegen (Beta -> Retail, memory reai-v2-beta-zu-retail).
2. Nicht raten, nicht aufgeben; den Nutzer nicht fragen (er schlaeft). Unklare Nutzerworte: die
   Lesart waehlen, die zu seinem Wortlaut UND zu den Bildern passt, im Dossier begruenden.
3. Dossier `analysis/befunde_runde34_nacht/<Code>_<thema>.md` im EIGENEN Baum: der ERSTE Werkzeugaufruf
   legt es an, danach nach jedem Abschnitt speichern. Frueh und oft committen (`wip(r34n-<code>): …`
   ist erlaubt) — ein Sitzungslimit bricht alle Agenten gleichzeitig ab.
4. Commit-Messages mit Backticks: `git commit -F <datei>` (memory reai-v2-commit-backticks).
5. Bauen nur ueber `bash re15_port/tools/local_build.sh` IM EIGENEN BAUM (nie `re15_port/build` des
   Hauptbaums, nie in einem fremden Baum). Nie `( … ) &` in Hintergrund-Bash. Integrationstests mit
   echter exe flattern unter Last: einzeln nachfahren, bevor etwas als Regression gilt.
6. Abnahme am ARTEFAKT: echte exe, echter Eingabepfad (`RE15_INPUT_SCRIPT`, `RE15_DEBUG_JUMP`,
   `RE15_FIRE_AOT`, `RE15_FRAMEDUMP`, `RE15_BEFUND_MARKE`), Bilder ansehen — "Suite gruen" ist keine
   Abnahme (memory reai-v2-absicht-statt-ergebnis, reai-v2-visual-verify-gdigrab).
7. Nichts im Hauptbaum `C:/workspace/git/reAi_v2` schreiben. Lesen dort ist erlaubt (z.B.
   `build/extracted`, `stage_saves`). Nicht in r34g_*-Baeume oder deren Zweige schreiben.
8. DuckStation nur, wenn statisch nicht belegbar; vorher `tasklist | grep -i duckstation` — laeuft eine
   Instanz, warten statt parallel starten.
