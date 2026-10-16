# Runde 38 — Spur G "tagebuch": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme 0 (2026-10-16). Baum `.claude/worktrees/r38_tagebuch`, Zweig `r38/tagebuch`, HEAD cd21bb26,
Basis e2cc7673. Massstab: AUFTRAG.md (Wortlaut), VERTRAG.md §1/§2. Nichts aus der Behauptung des Bau-Agenten wurde
ungeprueft uebernommen. Dieser Bericht wird FORTLAUFEND geschrieben.

## Stand
- [x] Auftrag/Vertrag/Dossier/Diff gelesen (`git diff master...HEAD`, 34 Dateien; die 4 geloeschten Orchestrator-
      Skripte in `git diff master --stat` sind master-Commits NACH der Basis, nicht Teil der Spur)
- [x] Bau: `local_build.sh configure` + `build` = LOCAL-BUILD-OK, "ninja: no work to do" (exe 17:21 = Quellstand
      HEAD; cd21bb26 aendert nur das Dossier)
- [x] Gezielte Tests (ctest -R, 85 + 59 Tests): **2 ROT durch die Spur** (unit_re2doc_bildebene, unit_r26_inventar)
- [x] Sweep aller unit_/prop_-Tests (645 Tests, Lauf T3): 3 rot = die 2 bekannten + unit_r35_android_bash_mutanten
      (Last, Einzel-Nachlauf s. T3)
- [x] Fortsetzung nach dem Sitzungslimit (2. Sitzung, 2026-10-16 abends): Bau erneut geprueft ("no work to do"),
      beide roten Tests einzeln nachgefahren + master-Gegenprobe wiederholt, 3 RE-Stichproben wiederholt
- [x] Punkt 1 an der exe gemessen (echter Weg: Tuer ROOM1021 -> ROOM1011, zu Roy gehen, Viereck)
- [x] Punkt 2 an der exe gemessen (Hinweis-Schirm + normale Karte)
- [x] RE-Gate (11 Stellen selbst disassembliert/gelesen, s. u.)
- [x] Vertrag / Pfade / Nebenwirkungen (Abschnitt "Vertrag")

Messwerkzeuge (Scratchpad, nicht im Baum): `lauf.sh` (Messkopie der exe als `re15_pc_abn0g_<name>.exe` neben der
Bau-exe bzw. neben der v0.8.25-exe, eigener Arbeitsordner, Ende ueber RE15_EXIT_AT; Spielstand ueber
`probe_r36_elza_karte`), `montage.py` (PPM-Kacheln), `tim2png.py` (TIM -> PNG). Referenz v0.8.25 =
`release/re15_port_v0.8.25_win64.zip` (entpackt, exe md5 7aa892fc...).

## Punkt 1 — Roys Tagebuch statt der Code-Nachricht (Elza, ROOM1011)

### Messprotokoll (echter Weg, NICHT per Sprung in den Raum)
Spielstand: `probe_r36_elza_karte re15_card.mcr 1021 -4000 -18000 0 4:0xF1 4:0xFA 3:0x74` (Elza im Main Office
ROOM1021 vor der Tuer zu ROOM1011, Stand NACH Roys Dialog: sub02 setzt (4,0xF1)/(4,0xFA)/(3,0x74)). Lauf:
`RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
RE15_MSG_LOG=1 RE15_STATE_LOG=state.log RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
RE15_INPUT_SCRIPT=A0.1,W1,U2.5,R0.35,W0.5,A0.1,W5,R0.1,W1,(R0.1,W1)x5,A0.1,W3,A0.1 RE15_FRAMEDUMP=190-1400/5:f_`
= Viereck an der Tuer (ROOM1021 Slot 1 -> ROOM1011, Ankunft (3650,6900) Gier 2048), 2,5 s nach Westen gehen,
rechts drehen, Viereck vor Roy (Elza (-1740,6570) Gier 3104, Roy-Entitaet t=47 bei (-1932,7450)), im Leser
6x RECHTS, Viereck schliesst, Viereck bestaetigt.

Lauf b1 (HEAD-exe), debug.log:
    [room] PC loaded room1011.rdt (210440 bytes)                <- ueber die TUER betreten
    [input-script] Tick 134 -> F194 Tasten 0x8000               <- Viereck vor Roy
    [scd] Plc_motion(entity=1, motion=11, flags=0x00) -> slot=0 <- sub07 @0x00E5C
    [msg] room=1011 id=18 savepoint=0
    [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, erstes Mal: ablegen)
    [tagebuch] Leser zu: Bit (9,68)=1, Kartenkette nach dem Evt_end
    [scd] Plc_motion(entity=1, motion=11 ...) / [scd F653] Plc_flg(mask=0x0080)   <- sub07 @0x00E6A/@0x00E6E
    [tagebuch] Evt_end erreicht: Kartenkette Eintrag 3 (ROOM10F1 Blatt 3 Rechteck 9) -> Folge 4 (ROOM1211 Blatt 1 Rechteck 3)
  `grep -c msgtext debug.log` = **0** — die Code-Nachricht wird NICHT mehr gezeigt.
Gegenprobe v0.8.25 (Lauf v1, identische Karte + identisches Skript bis zum Viereck):
    [scd] Plc_motion(entity=1, motion=11, flags=0x00) -> slot=0
    [msg] room=1011 id=18 savepoint=0
    [msgtext] room=1011 id=18 "He has passed out from his injuries.He is holding a slip.The numbers 0513" are printed on the slip."
  Bild v1 F230..F300: "He has passed out from his injuries." im Raum = der Vorher-Zustand.

Bilder (Lauf b1, RE15_WINDOW_SCALE=1, 320x240):
- F190/F205: Elza vor dem liegenden Roy (Kamera, in der beide sichtbar sind). F230 Abblende.
- F260..F340: Titelseite "ROYS DIARY" auf der FILE16-Illustration (Organizer mit Stift), Seitenzaehler 1/6, gruener
  Weiter-Pfeil.
- F375 2/6 "September 22nd / Umbrella... those bastards... / ..." | F410 3/6 "Worse, we practically held the ..." |
  F440 4/6 "September 28th / Damn it, we're screwed. ..." | F475 5/6 "and he only got in because I ..." |
  F505/F540 6/6 "... outer door... and its code: / **0513** (gruen). / I just hope he's okay..." + EXIT.
- F550..F630: "The Roys Diary / has been filed." (Name gruen), danach zurueck im Raum (F660/F680).
Text woertlich: alle sechs Seiten gegen AUFTRAG.md Z. 39-59 Zeile fuer Zeile gelesen (Bilder + eigene TIM->PNG-
Wandlung von FILE30_p01..p05): jeder Satz, jede Leerzeile zwischen den Absaetzen vorhanden; einzige Abbildung:
typografischer Apostroph U+2019 -> Apostroph der RE2-Schrift (9x), "..." als Schrift-Ellipse. Kein Wort fehlt.
Hintergrund: `FILE30_title_paper.TIM` md5 a2510fd6... == `FILE16_title_paper.TIM` (byte-gleich, unit-Pruefung
"Hintergrund ... == FILE16" gruen; im Bild sichtbar).
Gruen "wie alle Codes in den Dokumenten": CLUT der Seite FILE30_p05 Eintraege 9..14 = `16e0 1280 0e20 09e0 0980 0540`
— IDENTISCH mit FILE28_p01 ("4312") und FILE29_p01 ("5632"), den vorhandenen Code-Dokumenten; = TEX.TIM-Zeile 482
(Steuerbyte 0x05 Arg 1, RE-Gate #3). Im Leser-Bild ist "0513" gruen, der uebrige Text weiss.

### Zweites Untersuchen + FILE-Liste (Laeufe b4, b6)
- b4: nach Kette und Rueckkehr erneut Viereck vor Roy (F1061):
      [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, schon abgelegt: nur lesen)
      [tagebuch] Leser zu: Bit (9,68)=1, kein Hinweis (schon abgelegt)
  Bild F1200 Titelseite 1/6, F1230/F1250 Seite 2/6, KREUZ -> F1265 direkt zurueck im Raum, KEINE "filed"-Meldung,
  kein Hinweis-Schirm. Statusschirm -> R1 (FILE): Liste "Files" = genau EIN Eintrag "Roys Diary" (F1440..F1555).
- b6: aus der FILE-Liste geoeffnet (Viereck, Viereck): Titelseite 1/6 ... 6/6 mit gruenem "0513" und EXIT
  (F1260..F1700) — das Dokument ist ueber die Akten wieder lesbar (RE2-Form).

### Nebenbeobachtung (KEIN Mangel der Spur)
- Die Dossier-/Behauptungszeile "Elza beugt sich vor und richtet sich wieder auf wie im Original" ist im Bild NICHT
  zu sehen: Ausschnitt-Vergroesserung (Bilder F190..F225 und F650..F660) zeigt Elza aufrecht. Dasselbe in v0.8.25
  (Lauf v1, F190..F300): Plc_motion(1,11) laeuft, eine Beuge-Pose ist nicht sichtbar. Vorbestand, von der Spur nicht
  veraendert und vom Nutzer nicht verlangt; die Behauptung ist aber unbelegt (Opcodes laufen, Pose nicht gemessen).
- Statusschirm zeigt in Elzas Spiel die Karte "Leon S. Kennedy" — ebenso in v0.8.25 (Lauf v2, F120). Ausserhalb
  der Spur.

### End-to-End mit dem echten Dialog davor (Lauf e1/e2)
Spielstand OHNE die Dialog-Flags: `probe_r36_elza_karte re15_card.mcr 1021 -4000 -18000 2695 3:0xC9`;
`RE15_FIRE_AOT=1@40#1021` (Tuer), danach laeuft Roys Szene in ROOM1011 von selbst (sub00 @0x009E8 Ck(4,0xF1)==0 ->
Evt_exec sub02), `[msgtext] room=1011 id=17 "Elza: Roy, wake up! Please wake up!"`, Spieler frei ab F2221 (pm 0).
Danach (Skript ab F2260) 0,43 s vor, Viereck F2288:
    [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, erstes Mal: ablegen)
    [tagebuch] Leser zu: Bit (9,68)=1, Kartenkette nach dem Evt_end
    [tagebuch] Evt_end erreicht: Kartenkette Eintrag 3 (...) -> Folge 4 (...)
    [hint] F2787 begin / F2905 Folge-Hinweis 3 -> 4 (Zeit) / F3023 schliessen
Bilder e2 F2240..F2950: Elza vor Roy -> Titelseite -> 6/6 mit gruenem 0513 -> "The Roys Diary has been filed." ->
2F-Kachel -> B2-T-Kachel. Der GANZE Nutzerweg (Szene, "noch einmal anklicken", Tagebuch, Karte) laeuft.

### Urteil Punkt 1: ERFUELLT
Beleg: Lauf b1 (Tagebuch statt msg 18 ueber den echten Weg, `msgtext`-Zaehler 0), v1 (Vorher msg 18), Bilder b1
F260..F630 (Titel, 6 Seiten, gruenes 0513, "filed"), b4 (nur lesen, Liste 1 Eintrag), b6 (aus den Akten lesbar).

## Punkt 2 — Karte: Communications Room und Prison Room markieren (Lauf b1, b3)

### Hinweis-Schirm direkt nach dem Tagebuch (b1)
    [hint] F693 begin ... (Zaehler 10, Richtung 1)
    [hint] F787 Folge-Hinweis 3 -> 4 (Zeit)
    [hint] F787 begin ...
    [hint] F900 Zeit um (Hinweis 4, 3 Blinkperioden)
    [hint] F900 schliessen (Abbruch)
Bilder F700..F760: "POLICE STATION 2F", die Kachel Blatt 3 / Rechteck 9 blinkt (gefuellt/Umriss) — dieselbe Kachel,
die Leons Comms-Hinweis (ROOM10F0, Runde 30/33) zeigt (re15_map_zones.h:161/162 identische Zeilen fuer 10F0/10F1).
Bilder F795..F870: "POLICE STATION B2", die T-Kachel Blatt 1 / Rechteck 3 = ROOM1210/1211 "DETENTI. AREA"
(re15_map_zones.h:302/303; Johns Zelle: ROOM1211 sub00 @0x01E36 Typ 0x49, msg 10 "It's locked. It seems that there
is someone inside...") blinkt. Danach zu, Spiel laeuft weiter (b2: Elza bewegt sich auf HOCH/RUNTER).
Zeitsteuerung haengt an der Wanduhr (b1 Folge nach 94 Bildern, b2/b4 nach 118 Bildern) — die Kettenform der
Runde 35 K (3 Blinkperioden je Ziel); START schaltet weiter.

### Normale Karte danach (b3)
Statusschirm (START F1061) -> L1 = MAP (F1124): "POLICE STATION 1F" (besuchte Raeume), HOCH -> "2F": Comms-Kachel
blinkt (F1240..F1290); RUNTER -> 1F; RUNTER -> "B2" (B1 uebersprungen): T-Kachel blinkt (F1365..F1410).

### Urteil Punkt 2: ERFUELLT
"einmal den Communications Room ... und einmal den Prison Room" = zwei Hinweise nacheinander, beide gemessen;
"so wie auch schon bei Leon im Communications Room gemacht" = dieselbe Hinweis-Mechanik (RE2 0x84-Form, Runde 30) und
dieselbe Kettenform wie Leons Szene im Communications Room (Runde 35 K). "Und danach" = nach dem Tagebuch (Kette erst
am Evt_end von sub07, unit_r38_tagebuch_ablauf "in keinem Bild davor").

## RE-Gate — selbst nachgeprueft (Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` /
`re2_disasm.py` gegen `info/Re1.5/PSX.EXE` bzw. `info/re2leon/PSX.EXE`, `xxd`, `re15_port/tools/rdt_msgdump.py`)

| # | Behauptung | Selbst gelesen | Ergebnis |
|---|---|---|---|
| 1 | ROOM1011 sub07 @0x00E64 `2b 12 ff ff` Message_on 18, @0x00E82 Evt_end, @0x00E78 Aot_reset Slot 5 -> sub07 | `xxd -s 0xE40 ROOM1011.RDT`: `0e4e: 46 05 00..` / `0e58: 2e 01 00 00` / `0e5c: 3f 01 0b 00` / `0e60: 09 0a 1e 00` / **`0e64: 2b 12 ff ff`** / `0e68: 02 00` / `0e6a: 3f 01 0b 00` / `0e6e: 43 00 80 00` / `0e72: 09 0a 1e 00` / `0e76: 42 00` / `0e78: 46 05 03 31 ff 00 18 07 00 00` / **`0e82: 01 00`**; RDT md5 dfcbc099... == info/Re1.5; EINZIGES `2b 12` in ROOM1011 (scd_dump_room: 21 Message_on, nur @0x00E64 hat Id 18) | BESTAETIGT |
| 2 | msg 18 @0x1231 = Code-Nachricht, `05 01` faerbt 0513 gruen | rdt_msgdump: `id 18 @0x1231: 'He has passed out from his injuries.He is holding a slip.The numbers 0513" are printed on the slip.'`; Bytes `0x127c: 61 05 01 0c 11 0d 0f 05 00` | BESTAETIGT |
| 3 | Gruen = Textmaler-CLUT-Zeile 482 @0x80028974-94 | RE1.5 Sprungtabelle @0x8001099c (Index = Byte-1): `[4] -> 0x80028968` (Steuerbyte 0x05); dort `lbu v0,0(a2)` (Arg) / `80028974 andi v1,v0,0x4` / `sltu v1,zero,v1` / `andi v0,v0,0x3` / `sll v0,v0,1` / `80028984 addiu v1,v1,480` / `addu v0,v0,v1` / `sll v0,v0,6` / `80028994 ori s5,v0,0x10` -> Arg 1: y = 480 + 2 = 482 | BESTAETIGT |
| 4 | RE2 FUN_80071ba0: Dokument an die FILE-Liste VOR dem Lesen, Modus 4 | `80071bbc sltiu v0,a3,0x68` / `80071bc0 beq v0,zero,0x80071d00` / `80071d00 jal 0x800692dc` / `80071d04 addiu a0,a3,-104` / `80071dc0 addiu v0,zero,4` / `80071dc4 sb v0,1(s1)` | BESTAETIGT |
| 5 | RE2 FUN_800692dc: erster freier Platz, keine Doppelpruefung | `800692e0 addiu a2,zero,255` / `800692ec lbu v0,0(v1)` / `800692f4 bne v0,a2,..` / `80069300 sb a0,0(v1)` / `80069308 sltiu v0,a1,0x18` / `jr ra` — kein Vergleich mit a0 | BESTAETIGT (die Doppel-Sperre des Ports ist damit eine PORT-WAHL, als solche gekennzeichnet) |
| 6 | RE2 @0x80072b40 Zone aus, @0x80072b8c Aufnahme-Flag | `80072b3c beq ...` / **`80072b40 sb zero,0(v1)`** (Delay-Slot, v1 = [0x800ce5d8]) / **`80072b8c jal 0x8007730c`** | BESTAETIGT |
| 7 | RE2 Opcode 0x84 @0x800591C4: Modus 4, Anforderung, EIN Ziel je Opcode | `800591d4 addiu v0,zero,4` / `800591dc sb v0,[0x800d5c00]` / `800591e8 sb 1,[0x800df348]` / `800591ec ori v1,v1,0x8000` / `80059208 lbu v0,1(v0)` / `80059210 sb v0,[0x800d69f2]` / `8005921c addiu v0,v0,2` (PC += 2) | BESTAETIGT |
| 8 | RE2-Record @0x800AA184 = `06 00 50 00` (H 176) | `re2_disasm.py bytes 0x800AA144`: `800aa184: 06 00 50 00` (Index 16 der Tabelle @0x800AA144) | BESTAETIGT |
| 9 | ROOM1211: John sub00 @0x01E36 Typ 0x49; Befreiung (4,0xFC) @0x01F76 | `xxd`: `1e36: 44 00 49 40 ...`; `1f76: 22 04 fc 01` (danach `22 04 fa 00`); msg 10 "It's locked.It seems that there is someone inside..." | BESTAETIGT |
| 10 | Kartenzonen: ROOM10F1 Zone 0 -> Blatt 3 Rechteck 9; ROOM1211 -> Blatt 1 Rechteck 3 | `engine/src/re15_map_zones.h:162` `{ 0x10F1, ..., 3, 9, 0, 15, ... 0 }` (Hauptzeile; :475 Gastzeile Blatt 2/8 mit Kennung 1); `:303` `{ 0x1211, ..., 1, 3, 0, 34, ...}` | BESTAETIGT |
| 11 | Prison Key liegt in ROOM10F1, Aufnahme-Bit (9,83) | `include/re15_schluessel_r36.h:2-3, :60, :67` (`RE15_R36Q_PRISON_ITEM 0x42`, `RE15_R36Q_PRISON_BIT 83`, "im Communications Room ROOM10F0/10F1") | BESTAETIGT (Tagebuchtext "locked it up in the Communications Room" passt zum Port-Stand) |

Nicht selbst nachpruefbar aus der Datei: `RE15_FILES_FIRST_ITEM_ID 0x48` ("u8 @0x800c7370") — die Adresse liegt
hinter t_size 0xaf000 (BSS), also Laufzeitwert; die Konstante ist VORBESTAND (re15_files.h, Runde 30), nicht von
dieser Spur eingefuehrt. Kein Mangel der Spur.

Konstanten im Code-Diff (include/re15_tagebuch_r38.h, re15_files.c, map_hint_common.c): RAUM 0x1011 (@0x009F6/
@0x00A3E), MSG 18 (@0x00E64), EVT_END 0x1E (= 0xE82 - 0xE64, nachgerechnet), DOK 5 / Item 0x4D (Zensus + @0x80071d04),
BILDSATZ 30 (PORT-WAHL, gekennzeichnet), MAX_PAGE 5 (Satzbestand p00..p05, unit-gemessen), SEITE_H 176 (@0x800AA184),
BIT (9,68) (VERTRAG §1.1), ZIEL1 0x10F1/(9,83), ZIEL2 0x1211/(4,0xFC) (@0x01F76); Kettenform (Folge, zeitgesteuert)
= Form der Runde 35 K (im Dossier begruendet, keine neue Zeitkonstante); Listenname "Roys Diary" = NUTZER-VORGABE,
Kodierung ASCII-0x24 nachgerechnet (R 0x2e, o 0x4b, y 0x55, s 0x4f, D 0x20, i 0x45, a 0x3d, r 0x4e, Ende 0x07).
Jede Konstante traegt Beleg oder Kennzeichnung.

Suche im Diff (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen) nach
"deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|XXX|approx|geschaetzt|vermutlich": **0 Treffer**;
`getenv` im Engine-/Include-Diff: **0** (kein Env-Schalter als Abschluss).
Erklaert der Fix den Befund? Vorher (v1): sub07 @0x00E64 oeffnet msg 18. Fix: Haken in op_message_on genau fuer
Raum 0x1011 + Id 18 -> Dokument-Leser, Faden parkt, laeuft danach hinter das Message_on weiter. Nachher (b1):
kein msgtext, Leser, Evt_end, Kette. Ja.

## Tests (gezielt, im Spurbaum; KEINE volle Suite)

### Lauf T1 — Regex des Bau-Agenten, ERWEITERT um re2doc / r30_karte / r34n_f_leiche / r35_karte / r36_karte /
r36_story_roy / karte_besitz (85 Tests, seriell)
`ctest --test-dir re15_port/build -R "r38_tagebuch|r30_irons|r30_hinweis|r30_karte|r33_karte|r34n_e_dokumente|r34n_f_leiche|r35_inhalt_doku|r35_cut10f0|r35_karte|r36_kartenbits|r36_karte|r36_schluessel|r36_elza_roy|r36_story_roy|r37_roy|unit_savedata|unit_msg_dialog|unit_action_msg_gate|re2doc|karte_besitz" --timeout 600 --output-on-failure`
-> **`99% tests passed, 1 tests failed out of 85`** (Total Test time (real) = 1964.23 sec). Rot:
    252 - unit_re2doc_bildebene (Failed)
      FAIL: ein nicht vorhandenes Dokument meldet 0 statt Muell - sonst waere jede Zusage oben wertlos
Alle 7 Spur-Tests gruen (unit_r38_tagebuch_dokument/ablauf/karte/leon 21/18/14/2 Pruefungen, je direkt ausgefuehrt
und gelesen; integration_r38_tagebuch_lesen 44 s / _karte 21 s / _leon 28 s), ebenso der nachgezogene Alt-Riegel
unit_r36_schluessel_zelle und alle Roy-/Karten-/Dokument-/Hinweis-Tests der Runden 30-37.

### Lauf T2 — Module, die der Diff beruehrt, ueber die Datei-Zensur der Tests (`grep -l re15_files_|
re15_menu_request_doc|re15_map_hint_|map_ziel_aktiv` in tests/unit) + Karten-/Inventar-Gruppe (59 Unit-Tests)
`ctest ... -R "^unit_(inv_fsm|r26_inventar|r30_pfeil|r34n_g_maskgrp|r34n_e|r35_inhalt_items|r37_feuerloescher|r37_kistenregel|map_|savepoint|inv_grant|r36_inventar|r35_inventar1050|debug_menu|pri_inventar_riegel|em_status_persist|r36_story|aot_sce_census)"`
-> **`98% tests passed, 1 tests failed out of 59`**. Rot:
    324 - unit_r26_inventar
      (B4 Vergleiche gefahren: 846, Dokument-Ids ausgenommen: 54)
      [FAIL] B4 kein Kollateral in den uebrigen Skripten

### Ursache beider roten Tests = diese Spur (Gegenprobe gegen den master-Bau, deterministisch)
* unit_re2doc_bildebene: `tests/unit/test_re2doc_bildebene.c:150` nimmt als "nicht vorhandenes Dokument"
  `re15_re2doc_size(RE2_FILES_DOC_COUNT + 5, -1, ...)` = Satz **30** (RE2_FILES_DOC_COUNT 25, gen/re2_files_toc.inc:5) —
  genau den Satz, den die Spur als FILE30_* anlegt. master-Bau (`C:/workspace/git/reAi_v2/re15_port/build/tests/
  unit/test_re2doc_bildebene.exe`, Assets ohne FILE30): `PASS: ein nicht vorhandenes Dokument meldet 0`; HEAD: FAIL
  (zweimal einzeln nachgefahren, 0,2 s, keine Last-Frage).
* unit_r26_inventar B4: `tests/unit/probe_r26_inventar.c:296-298`
  `gefahren == 9 * (re15_item_prompt_name_count() - n_dok - 2) && gefahren >= 850`. Mit Dokument 5 (Item 0x4D) steigt
  n_dok 5 -> 6, gefahren sinkt 855 -> 846: die Gleichung haelt (9 x 94), die Untergrenze 850 nicht. master-Bau
  (`.../build/tests/unit/probe_r26_inventar.exe`): `B4 Vergleiche gefahren: 855 ... [OK  ] B4`; HEAD: 846, FAIL.
Beide sind ALT-RIEGEL, deren Annahme die Spur mit ihren Ressourcen (Satz 30, Dokument 5) beruehrt — genau wie
unit_r36_schluessel_zelle, das der Bau-Agent nachgezogen hat. Sie fehlen in seiner Regex (re15_files.c und die
FILES-Assets sind beruehrt, ihre Verbraucher-Tests wurden nicht gefahren); seine Aussage "100% ... out of 60" ist
deshalb fuer die beruehrten Module nicht vollstaendig.

Nachlauf 2. Sitzung (Bau unveraendert, "ninja: no work to do"):
    ctest -R "^(unit_re2doc_bildebene|unit_r26_inventar)$"  ->  0% tests passed, 2 tests failed out of 2
      FAIL: ein nicht vorhandenes Dokument meldet 0 statt Muell ...      (0,29 s)
      (B4 Vergleiche gefahren: 846, Dokument-Ids ausgenommen: 54) [FAIL] B4   (0,04 s)
    master-Bau (C:/workspace/git/reAi_v2/re15_port/build, master hat seit der Basis e2cc7673 KEINE re15_port-Aenderung,
    Assets ohne FILE30), eigener Arbeitsordner: "PASS: ein nicht vorhandenes Dokument meldet 0" /
    "(B4 Vergleiche gefahren: 855, Dokument-Ids ausgenommen: 45) [OK  ] B4".
    Code: test_re2doc_bildebene.c:150 `re15_re2doc_size(RE2_FILES_DOC_COUNT + 5, -1, ...)` mit
    engine/src/gen/re2_files_toc.inc:5 `#define RE2_FILES_DOC_COUNT  25` -> Satz 30, Seite -1 = FILE30_title_page.TIM
    (von der Spur angelegt). probe_r26_inventar.c:296-298 `gefahren == 9 * (name_count - n_dok - 2) && gefahren >= 850`,
    n_dok = re15_files_doc_count() = 6 -> 846.

### Lauf T3 — Sweep aller unit_- und prop_-Tests (645 = 629 unit_ + 16 prop_, keine Integration)
Gestartet in der 1. Sitzung, lief nach deren Ende zu Ende (Log `scratchpad/g_ctest_unit.log`, 18:29):
    99% tests passed, 3 tests failed out of 645   (Total Test time (real) = 741.60 sec)
      1/645   Test #324: unit_r26_inventar ..............***Failed    0.03 sec    <- Spur (s. o.)
      347/645 Test #252: unit_re2doc_bildebene ..........***Failed    0.07 sec    <- Spur (s. o.)
      645/645 Test #480: unit_r35_android_bash_mutanten ..***Failed  662.03 sec   <- Last? (s. u.)
`unit_r35_android_bash_mutanten` mutiert `release/apk_pruefen.sh` und laesst je Mutant
`tests/unit/r35_android/urteil_kontrollen.sh` laufen (10 parallel, tests/unit/probes/r35_android.cmake:77-80); weder
release/ noch tests/unit/r35_android/ noch platform/android/ sind von der Spur beruehrt. Einzel-Nachlauf: s. T3b.
Mechanismus des Rot (aus dem Sweep-Log + bash_urteil_mutanten.py:316-361): jeder Mutant laeuft mit
`timeout=300`; ein Zeitlimit liefert rc 124 und zaehlt als "erkannt". Der EINZIGE Zeitlimit-Fall im Sweep:
    [erkannt] Zeile 317: A (( rc == 0 )): == -> <=  <- Zeitlimit            (g_ctest_unit.log:1529)
Genau dieser Mutant steht in AEQUIVALENT (bash_urteil_mutanten.py:59-60, "rc <= 0 und rc == 0 sind gleich"). Als
"erkannt" gezaehlt wird er nicht "gesehen" -> sein AEQUIVALENT-Eintrag gilt als veraltet -> `return 1`
(:358-361). Ein Last-Artefakt (Sweep mit parallelen Tests + parallele Agenten), unabhaengig vom Diff der Spur.

## Vertrag (VERTRAG.md §1/§2) — selbst geprueft
* Pfade (`git diff master...HEAD --name-only`, 35 Dateien): KEINE unter release/, re15_port/platform/android/,
  re15_port/shared_assets/PSX/, KEINE CMakeLists-Aenderung (Test-Einbindung ueber
  tests/unit/probes/r38_tagebuch.cmake, tests/unit/CMakeLists.txt:4060 `file(GLOB ... probes/*.cmake)`).
  Neue Assets nur unter re15_port/shared_assets/RE2/FILES/ (FILE30_*, 8 Dateien) — nicht TABU.
* Gemeinsame Dateien, nur Haken mit Kommentar "Runde 38 Spur G": scd_vm.c +4 (Include + 1 Code-Zeile),
  game_step_common.c +2, map_hint_common.c +9 (Include + 2 Tabellenzeilen + Kommentar), re15_files.c +9 (Name +
  1 Tabellenzeile), menu_common.c +10/-1 (Include, Merker, 1 Zeile im Leser-Schliessen, 3 Zeilen in der Aufnahme,
  1 Zeile in request_doc). menu_common.c / map_hint_common.c / re15_files.c stehen nicht in der Liste §1.4, die
  Aenderungen sind aber Haken-gross; die Logik liegt in tagebuch_r38.c. menu_common.c aendert auch Spur Q
  (r38_kleinkram: Inventar-Ton in re15_menu_start_poll / tab_select) — andere Stellen, Integrationsfrage, kein Mangel.
* Ressourcen: Bank-9-Bit 68 (zugeteilt G) — als einziges neu belegt, in k_port[] eingetragen
  (test_r36_kartenbits.c +2); keine Nachrichten-Id belegt (1011 40-44 nicht gebraucht); Dokument 5 / Item 0x4D /
  Bildsatz 30 per Zensus (G_belege/zensus.txt). Kein anderer r38-Baum aendert re15_files.c, map_hint_common.c oder
  legt FILE3x an (gegrept ueber alle `.claude/worktrees/r38_*`, Diff gegen e2cc7673).
* Nebenwirkung "nur lesen" (menu_common.c): `file_reader_close` setzt bei s_doc_target && s_doc_nur_lesen
  s_phase = 2 -> der gemeinsame Abbau (menu_common.c:1532 `s_doc_target = 0; s_doc_msg = 0; ...`) laeuft wie beim
  normalen Schliessen, kein Zustand bleibt stehen (b4: danach Statusschirm normal, FILE-Liste 1 Eintrag).
  `re15_tagebuch_r38_listenplatz` wird fuer JEDES Aufnahme-Dokument gefragt; fuer die Welt-Dokumente der Runden
  30/34 liefert es -1 (ihre Zone ist nach dem Aufheben weg) -> unveraendert (unit_r34n_e_dokumente,
  integration_r34n_e_dokumente_bild, unit_r30_irons_* gruen in T1).
* Karte: Eintraege 3/4 haengen an (9,68) "gezeigt"; das Bit setzt nur das Ablegen von Roys Diary (Elza) -> in Leons
  Spiel still (unit_r38_tagebuch_karte "Leon unveraendert", integration_r38_tagebuch_leon gruen). "erreicht" wirkt
  nur auf das Blinken in der normalen Karte (map_hint_common.c:292-294), nicht auf den Hinweis-Schirm.
* Tests je Punkt: Punkt 1 unit_r38_tagebuch_dokument/_ablauf + integration_r38_tagebuch_lesen; Punkt 2
  unit_r38_tagebuch_karte + integration_r38_tagebuch_lesen/_karte; Gegenprobe Leon unit/integration_r38_tagebuch_leon.
  Messend (echte VM + Statusschirm, echter Seitenlader, Framedump-Pixel), alle gruen (T1).
* Alt-Riegel: unit_r36_schluessel_zelle R nachgezogen (Nutzer-Vorgabe ersetzt msg 18 — begruendet, Kern der Runde
  36 bleibt geprueft). Die ZWEI weiteren Alt-Riegel, deren Annahme die Spur bricht (unit_re2doc_bildebene,
  unit_r26_inventar B4), sind NICHT nachgezogen -> Mangel 1/2.

### Lauf T3b — Einzel-Nachlauf unit_r35_android_bash_mutanten (2. Sitzung)
`ctest --test-dir re15_port/build -V -R "^unit_r35_android_bash_mutanten$" --timeout 1500` (Log
`scratchpad/G0/abn0_android_mut_einzeln.log`), gefahren bei 100 % CPU-Last durch PARALLELE AGENTEN (gemessen:
Win32_Processor LoadPercentage 100, 8 ctest-, 118 bash-Prozesse, 12 fremde re15_pc-Messkopien):
    480:    Kontrolle (unveraendert): Rueckgabe 0 KONTROLLEN: 102 ok, 0 FALSCH
    480:    [gleichwertig] Zeile 317: A (( rc == 0 )): == -> <=          <- DIESMAL korrekt eingestuft
    1/1 Test #480: unit_r35_android_bash_mutanten ...***Timeout 1503.36 sec   (212 von 328 Mutanten durch)
Unter dieser Last nicht zu Ende zu fahren. Wertung: KEIN Befund gegen die Spur — (a) der Test liest nur
release/apk_pruefen.sh, release/python_finden.sh und tests/unit/r35_android/* mit eigenen Attrappen (gegrept: keine
shared_assets-/FILES-Pfade), die Spur aendert keine dieser Dateien (`git diff master...HEAD --name-only | grep -E
"release/|r35_android|apk|gate"` leer), die Eingaben sind also bit-gleich mit master; (b) das Sweep-Rot ist
mechanisch erklaert (Zeitlimit eines AEQUIVALENT-Mutanten), und eben dieser Mutant laeuft im Nachlauf als
"gleichwertig" durch. Der Orchestrator faehrt ihn in der seriellen Integrationssuite.

### Lauf T4 — Nebenwirkungen Inventar / Karte / Elza-Story / ROOM1011 + Spur-Tests frisch (2. Sitzung, seriell)
(folgt nach T3b, damit die Fenster-Haken nicht unter der Last der 10 Mutanten-Prozesse laufen)

## Nachkontrolle Wortlaut (2. Sitzung, eigene Bilder der 1. Sitzung, Lauf b1)
Montagen `scratchpad/G0/abn0s2_seiten_a.png` (F260/F375/F410/F440) und `abn0s2_seiten_b.png` (F475/F540/F600/F680)
noch einmal Zeile fuer Zeile gegen AUFTRAG.md gelesen: Titel "ROYS DIARY" 1/6; 2/6 "September 22nd / Umbrella...
those bastards... / It was Umbrella who created these freaks now crawling all over the station. / The S.T.A.R.S.
members told us what was going on in the Arklay Mountains, and what did we do? ...We laughed them off."; 3/6 "Worse,
... survivor inside... / How could we have been so stupid..."; 4/6 "September 28th / Damn it, we're screwed. / Now
someone has sabotaged the Communications Room and wrecked the radio equipment. / And of course, the others didn't
have to look far for someone to blame. They went straight for my friend John. He's the only outsider here,"; 5/6
"and he only got in because I signed him in on a visitor's pass... / They didn't even hesitate. They threw him into a
holding cell downstairs. / There's no way in hell he did this... / Anyway, they took the cell"; 6/6 "key from me and
locked it up in the Communications Room so I can't let him out. All I've got left is my keycard for the outer
door... and its code: / 0513. / I just hope he's okay..." (+ EXIT); F600 "The Roys Diary / has been filed."; F680
zurueck im Raum (Elza neben dem liegenden Roy). Die Stelle "outer door..." in 3-facher Vergroesserung
(`abn0s2_p6_tuer.png`): drei Punkte, kein vierter (die Illustration dahinter taeuscht im kleinen Bild). "0513" gruen,
der Punkt dahinter weiss.

## Gates
| Gate | Ergebnis | Beleg |
|---|---|---|
| Punkt 1 (Tagebuch statt Code-Nachricht) | ERFUELLT | Laeufe b1/b4/b6/e1/e2 + v1 (Abschnitt Punkt 1), Wortlaut-Nachkontrolle |
| Punkt 2 (Karte Comms + Prison) | ERFUELLT | Laeufe b1/b3 (Abschnitt Punkt 2), `abn0s2_hinweis.png` |
| RE-Gate | HAELT | 11 Stellen (1. Sitzung) + 3 Stichproben (2. Sitzung: ROOM1011 @0xE58-0xE83, RE2 @0x80071cf8-d0c, RE1.5 @0x80028968-94) selbst gelesen; 0 Rate-Woerter, 0 getenv im Diff |
| Vertrag / Pfade / Ressourcen | HAELT | Abschnitt Vertrag |
| Tests der Spur (r38_tagebuch, 7) | GRUEN | T1 (+ T4) |
| Nebenwirkung auf vorhandene Riegel | BRICHT | unit_re2doc_bildebene + unit_r26_inventar deterministisch rot durch die Spur (T1/T2/T3, master gruen) |

## Maengel (nummeriert, nachpruefbar)
1. **unit_re2doc_bildebene rot durch FILE30.** `tests/unit/test_re2doc_bildebene.c:150` prueft als "nicht vorhandenes
   Dokument" `re15_re2doc_size(RE2_FILES_DOC_COUNT + 5, -1, ...)` = Satz 30, Titelseite — die Spur legt genau
   `shared_assets/RE2/FILES/FILE30_title_page.TIM` an. HEAD: `FAIL: ein nicht vorhandenes Dokument meldet 0 statt
   Muell`; master: PASS. Nachpruefen: `ctest --test-dir re15_port/build -R "^unit_re2doc_bildebene$"`.
   Forderung: Alt-Riegel begruendet nachziehen (Gegenprobe auf einen Satz, den es nachweislich nicht gibt, z. B. den
   ersten Satz ohne FILEnn_title_page.TIM im Asset-Baum statt der festen +5), Kommentar "Runde 38 Spur G".
2. **unit_r26_inventar B4 rot durch Dokument 5.** `tests/unit/probe_r26_inventar.c:296-298`
   `gefahren == 9 * (re15_item_prompt_name_count() - n_dok - 2) && gefahren >= 850`: mit n_dok = 6 (Item 0x4D
   ausgenommen) ist gefahren = 846 — die Gleichung haelt, die feste Untergrenze 850 nicht. HEAD:
   `(B4 Vergleiche gefahren: 846, Dokument-Ids ausgenommen: 54) [FAIL] B4`; master: 855/45 OK. Nachpruefen:
   `ctest --test-dir re15_port/build -R "^unit_r26_inventar$"`. Forderung: Untergrenze begruendet an die
   Dokument-Tabelle anpassen (sie soll weiterhin verhindern, dass die Tabelle Nicht-Dokument-Ids schluckt), Kommentar
   "Runde 38 Spur G".
3. **Testaussage "100% ... out of 60" deckt die beruehrten Module nicht.** Die Regex des Bau-Agenten enthaelt keine
   Verbraucher von `re15_files.c` / `shared_assets/RE2/FILES` (re2doc, r26_inventar). Forderung: nach 1./2. die
   Regex um `re2doc|r26_inventar` (und die Inventar-/Kartengruppe aus T2/T4) erweitern und das Ergebnis woertlich
   ins Dossier.

## Hinweise (nicht blockierend)
- H1: Behauptung/Dossier "Elza beugt sich vor und richtet sich wieder auf wie im Original" ist im Bild NICHT zu sehen
  (Ausschnitte F190..F225, F650..F660; ebenso v0.8.25 Lauf v1). Plc_motion(1,11) laeuft (Log), die Pose ist nicht
  gemessen. Vorbestand, nicht verlangt — den Satz im Dossier auf "die Opcodes des Originals laufen weiter"
  korrigieren oder die Pose messen.
- H2: Integration: menu_common.c aendert auch Spur Q (Inventar-Ton in re15_menu_start_poll/tab_select) — andere
  Stellen; der Aufnahme-Leser des Tagebuchs geht NICHT durch re15_menu_start_poll (re15_menu_request_doc setzt
  s_latch/s_stage selbst, menu_common.c:2580-2593), Qs Oeffnen-Ton trifft ihn also nicht; map_hint_common.c Folge-Index 4 ist
  positionsabhaengig (unit_r38_tagebuch_karte faengt es); neue Assets FILE30_* (8 Dateien) muessen ins Paket
  (Asset-Gate beim Release, Spur V/Orchestrator).
- H3: Unversionierter Ordner `build/r38_g/` in der Baumwurzel (Ausgabe von `tagebuch_satz.py --out`) — nicht
  committet, beim Aufraeumen entfernen.
- H4: unit_r35_android_bash_mutanten ist lastempfindlich (AEQUIVALENT-Mutant + Zeitlimit 300 s) — kein Bezug zur
  Spur, Hinweis fuer den Orchestrator (volle Suite seriell fahren).

(Gesamturteil folgt nach T3b/T4)
