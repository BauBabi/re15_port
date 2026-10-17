# Runde 38 — Spur G "tagebuch": UNABHAENGIGE ABNAHME 1

Pruefer: Abnahme 1 (2026-10-16/17). Baum `.claude/worktrees/r38_tagebuch`, Zweig `r38/tagebuch`, HEAD 70143c31,
Basis e2cc7673. Massstab: AUFTRAG.md (Wortlaut), VERTRAG.md §1/§2, Maengel 1-3 der Abnahme 0 (`G_abnahme_0.md`).
Nichts aus der Behauptung des Bau-Agenten wird ungeprueft uebernommen. Dieser Bericht wird FORTLAUFEND geschrieben.

## Stand
- [x] Auftrag/Vertrag/Dossier/Abnahme 0/Diff gelesen (`git diff master...HEAD`, 37 Dateien; `git diff master --stat`
      zeigt zusaetzlich geloeschte Orchestrator-Dateien = master-Commits NACH der Basis e2cc7673, nicht Teil der Spur)
- [x] Bau: `local_build.sh configure` + `build` = LOCAL-BUILD-OK, "ninja: no work to do"; Arbeitsbaum sauber
      (`git status --short` leer); exe md5 e81c5047f3c0da6642f2338f75c1a5aa (17:21, letzte Engine-Aenderung fb151062
      17:08); Test-exes enthalten die Nachbesserungs-Texte (`grep -c` je 1)
- [x] Maengel 1/2 der Abnahme 0 (Alt-Riegel) gemessen + eigene Mutationsproben (MA1, MD2, MD3) + RE-Gate der neuen Belege
- [x] 2. Sitzung (2026-10-17 ab 04:55, nach Sitzungslimit der 1.; deren 151er-Lauf brach bei 72/151 ab): Bau erneut
      `configure` + `build` = LOCAL-BUILD-OK, "ninja: no work to do", exe md5 weiterhin e81c5047...; Maschine ruhig
      (CPU 16 %, 0 fremde re15_pc, 0 ctest)
- [ ] Gezielte Tests (151er-Regex des Bau-Agenten, seriell, laeuft)
- [x] Punkt 1 an der exe (echter Weg: Tuer ROOM1021 -> ROOM1011, Roys Szene, Roy erneut untersuchen) — Laeufe a1/a2
- [x] Punkt 2 an der exe (Hinweis-Schirm 2F -> B2 + normale Karte) — Laeufe a1/a2
- [ ] RE-Gate (Stichproben selbst disassembliert)
- [ ] Vertrag / Pfade / Nebenwirkungen
- [ ] Gesamturteil

Messwerkzeuge 2. Sitzung (Scratchpad `scratchpad/G1B/`, nicht im Baum): `lauf.sh` (Karte ueber
`probe_r36_elza_karte`, Messkopie `re15_pc_abn1g2_<name>.exe` neben der Bau-exe, eigener Arbeitsordner, Ende ueber
RE15_EXIT_AT), `montage.py` (PPM -> PNG-Kacheln), `gruen.py` (gruene Pixel eines Framedumps + CLUT-Eintraege 9..14
der Seiten-TIMs).

Messwerkzeuge 1. Sitzung (Scratchpad `scratchpad/G1A/`, nicht im Baum): `dis.py` (eigener Aufruf von `dis_one` aus
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` fuer PSX.EXE, RE2-PSX.EXE und DEBUG.BIN mit Basis 0x800c0000),
`mut/` (eigene Mutanten: mutierte Kopie EINER Engine-Datei, mit den Bau-Flags kompiliert und VOR `libre15_engine.a`
an das vorhandene Test-Objekt gelinkt; das Bauverzeichnis bleibt unberuehrt).

## Nachbesserung 1 — Maengel 1/2 der Abnahme 0, selbst gemessen

### Mangel 1 (unit_re2doc_bildebene) — BEHOBEN
`ctest --test-dir re15_port/build -R "^(unit_re2doc_bildebene|unit_r26_inventar)$" --timeout 600 -V`:
    252:   Gegenprobe: erster Satz ohne Titelseite im Asset-Baum = 31 (hoechster Satz RE2/Port-Tabelle = 30)
    252:   PASS: der Probe-Satz 31 liegt hinter allen RE2-Saetzen und allen Saetzen der Port-Tabelle (hoechster 30) - davor fehlt keiner
    252:   PASS: der Nachbar Satz 30 meldet sich im Lader (1) unmittelbar vor der Probe
    252:   PASS: ein nicht vorhandenes Dokument (Satz 31) meldet 0 statt Muell - sonst waere jede Zusage oben wertlos
    1/2 Test #252: unit_re2doc_bildebene ............   Passed    0.12 sec
Code gelesen (`test_re2doc_bildebene.c:147-184`): Probe-Satz = erster nn ohne `FILEnn_title_page.TIM` (fopen); drei
Bedingungen (nn hinter RE2s 25 und jedem `bildsatz` der Port-Tabelle; Nachbar nn-1 laedt mit 1; Satz nn meldet 0).
Ein fehlendes Asset-Verzeichnis macht nn = 0 -> `nn > max_satz` rot (kein stilles Gruen).
EIGENE Mutationsprobe (verschieden von M1 des Bau-Agenten): MA1 = der Lader liefert fuer eine fehlende Datei die
zuletzt geladene Kachel (`if (!data || sz <= 0) { ... return &s_cache[(s_next+N-1)%N] ... }`, "veralteter Puffer"):
    MA1: FAIL: Satz 25: p15 vorhanden, p16 nicht ...
         FAIL: ein nicht vorhandenes Dokument (Satz 31) meldet 0 statt Muell - sonst waere jede Zusage oben wertlos
         RE2DOC: FAIL (2)  exit=1
    MA0 (unveraenderte Kopie, gleicher Link-Weg): 12 PASS, exit=0
-> die neue Gegenprobe hat Zaehne; keine feste Satznummer mehr (eine kuenftige FILE31 verschiebt die Probe auf 32).

### Mangel 2 (unit_r26_inventar B4) — BEHOBEN
    324:   (B4 Vergleiche gefahren: 846, Dokument-Ids ausgenommen: 54)
    324:   (B4 Dokument-Basis aus DEBUG.BIN @0x800c7370: 0x48; Nicht-Dokument-Ids darunter: 630 gefahren, 0 als Dokument geschluckt)
    324:   [OK  ] B4 kein Kollateral in den uebrigen Skripten
    324:   [OK  ] B4b genau die Dokument-Ids der Tabelle ausgenommen (0x48..), je Schluessel einmal
    324: === probe_r26_inventar: GRUEN (0 Fehler) ===      2/2 Test #324 Passed 0.02 sec
Code gelesen (`probe_r26_inventar.c:257-335`): Basis per fopen/fseek aus `shared_assets/PSX/BIN/DEBUG.BIN` Datei 0x7370
(fehlt die Datei: basis = -1 -> `basis == 0x48` rot, kein stilles Gruen); B4 = Gleichung (unveraendert) UND
`gefahren_unter_basis == 9 x (basis - 2) = 630` UND `dokumente_unter_basis == 0`. Die Nicht-Leere bleibt (>= 630
Vergleiche immer). Feste 850 entfernt.
EIGENE Mutationsproben (`re15_files_doc_from_item` = `item_id - RE15_FILES_FIRST_ITEM_ID`, re15_files.c:110-118 —
geschluckt werden kann nur ueber diese Abbildung):
    MD2 Off-by-one der Untergrenze (0x47 Blue Master Keycard -> Dokument 0):
         (B4 ... 837, ... 63) / (... 621 gefahren, 9 als Dokument geschluckt) [FAIL] B4, [FAIL] B4b -> ROT (2 Fehler)
    MD3 Engine-Basis 0x47 bei weiter 6 Dokumenten:
         (B4 Vergleiche gefahren: 846, Dokument-Ids ausgenommen: 54)          <- Gleichung haelt (blind)
         (... 621 gefahren, 9 als Dokument geschluckt) [FAIL] B4               <- NUR die neue Pruefung faengt es
         [OK  ] B4b ...                                                        <- blind
         [FAIL] B5b (Folgefehler) -> ROT (2 Fehler)
    MD0 (unveraenderte Kopie): GRUEN (0 Fehler), exit 0
    MD1 (nur das Tabellenfeld item_id 0x4D -> 0x30): B4 GRUEN — erwartungsgemaess, denn doc_from_item liest das Feld
         nicht; das Feld bewacht unit_r38_tagebuch_dokument: `FEHLER: Dokument 5 = Item-Id 0x30` (exit 1), Kontrolle
         21/0.
Wertung: die alte Grenze (Gleichung + >= 850) war fuer eine Basisverschiebung bei 5 Dokumenten selbst blind
(9 x 95 = 855 >= 850); die neue Pruefung faengt sie unabhaengig von der Tabellengroesse. Zweck erfuellt, strenger als vorher.

### RE-Gate der neuen Belege (selbst disassembliert, Rohbytes aus info/Re1.5)
* Ladeadresse DEBUG.BIN = 0x800c0000 (Voraussetzung fuer "Datei 0x7370 = @0x800c7370"): PSX.EXE
  `80013124 ori a0,zero,0x7` / `80013128 lui a1,0x800c` / `8001312c jal 0x80013b60` (CD_read(Datei 7, 0x800c0000)) /
  `80013130 ori a2,zero,0x0`; Dateitabelle @0x8006F43C Eintrag 7 @0x8006f474 = `00 00 04 00 ...` = 262144 B =
  Groesse von DEBUG.BIN (md5 c2c11aab... in info/ == shared_assets/). -> roh, ohne Kopf: Datei-Offset = Adresse - 0x800c0000.
* DEBUG.BIN: `800c72d0 lui v0,0x800c` / `800c72d4 lbu v0,0x6c95(v0)` (Seite) / `800c72dc addiu v1,v1,0x7370` /
  `800c72f4 addu v1,v1,v0` / **`800c72f8 lbu s1,0(v1)`** / `800c7304 andi t0,s3,0x1` / `800c7308 bne t0,zero,0x800c7320`
  / **`800c7320 jal 0x80028840`** + `800c7324 addu a0,s1,zero` / `800c7344 ori v0,zero,0xa` / `800c734c bne s0,v0,
  0x800c7304` / **`800c7350 addiu s1,s1,1`** (Delay-Slot); Datei 0x7370 = **`48 52 5c 00`**, dahinter `f0ff bd27`
  (Prolog der naechsten Funktion). -> 3 FILE-Seiten x 10 Zeilen, Zeile k = Name der Id basis+k (Bit k der Maske
  u16 @0x800c6c98+2*Seite, sonst Platzhalter @0x800c7916).
* PSX.EXE `80028840 andi a0,a0,0xff` / `8002884c addiu at,at,0x495c` / `80028854 lhu v1,0(at)` / `8002885c addiu
  v0,v0,0x4a28` / `80028864 addu v0,v1,v0` = Name(Id) aus u16 @0x800c495c + Blob @0x800c4a28. Selbst dekodiert:
  0x46 "Red Master Keycard", 0x47 "Blue Master Keycard", 0x48 "Chris' Diary", 0x49 "Operation Report", 0x4a..0x51
  "File 3".."File 10", 102 Namen.
* PSX.EXE Klemme: `8004a348 lbu v0,0(v1)` (v1 = 0x800b2669) / **`8004a350 sltiu v0,v0,0x48`** / `8004a354 bne v0,zero,
  0x8004a360` / **`8004a358 ori v0,zero,0x47`** / `8004a35c sb v0,0(v1)`. Kontext (ghidra1_V2.txt Z. 163380-163540):
  Debug-Itemwaehler — Zustand 1 laedt Datei 0xb (ITEMALL.PIX) nach 0x801a0000 (@0x8004a194-9c), Zustand 2 zeichnet Bild
  Id x 1200 (@0x8004a1bc-d8), Zustand 3 +-1/+-10 und Klemme auf 0x47. ITEMALL.PIX 86400 B = 72 x 1200, ITPS.ITP 884736
  B = 72 x 12288 -> 72 = 0x48 Items. BESTAETIGT.

## Punkt 1 — Roys Tagebuch statt der Code-Nachricht (Elza, ROOM1011), 2. Sitzung, selbst gemessen

Gemeinsame Umgebung (beide Laeufe): `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_MSG_LOG=1 RE15_STATE_LOG=state.log RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=60`, Spielstand ueber `probe_r36_elza_karte` (Elza = Figur 4 aus der Raumvariante xxx1),
Bau-exe md5 e81c5047... als Messkopie.

### Lauf a2 — der GANZE Nutzerweg: Tuer, Roys Szene, Roy "noch einmal anklicken"
Karte `1021 -4000 -18000 0 3:0xC9` (Elza im Main Office vor der Tuer zu ROOM1011, Roys Dialog NOCH NICHT gesehen),
`RE15_INPUT_SCRIPT=A0.1,W78,U0.45,W0.3,A0.1,W3,(R0.1,W1.2)x6,W0.5,A0.1,W2.5,A0.1,W12`, `RE15_FRAMEDUMP=100-3300/10:f_`,
`RE15_EXIT_AT=3300`. debug.log:

    [save] CONTINUE: resumed in room 1021 (hp=100)
    [input-script] Tick 0 -> F60 Tasten 0x8000                          <- Viereck an der Tuer
    [room] PC loaded room1011.rdt (210440 bytes)                         <- ueber die TUER
    [msgtext] room=1011 id=1 "Elza: Hang in there! ..." ... id=16 "Roy: Please, you have to help him..." /
      id=24 "Roy: groans" / id=17 "Elza: Roy, wake up! Please wake up!"  <- Roys Szene sub02 laeuft echt
    [scd F2221] letterbox closed -> gameplay (player_mode=0)
    [input-script] Tick 2366 -> F2426 Tasten 0x8000                      <- Roy noch einmal anklicken
    [schluessel] ROOM1011: Roy -> sub07 (Zettel 0513) statt sub06 (Prison key)
    [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, erstes Mal: ablegen)
    ... 6x RECHTS F2519..F2714, Viereck F2768 (EXIT), Viereck F2846 (Meldung) ...
    [tagebuch] Leser zu: Bit (9,68)=1, Kartenkette nach dem Evt_end
    [tagebuch] Evt_end erreicht: Kartenkette Eintrag 3 (ROOM10F1 Blatt 3 Rechteck 9) -> Folge 4 (ROOM1211 Blatt 1 Rechteck 3)
    [hint] F2901 begin / F3019 Folge-Hinweis 3 -> 4 (Zeit) / F3138 Zeit um / F3138 schliessen (Abbruch)

`grep -c "msgtext] room=1011 id=18"` = **0** (die Code-Nachricht erscheint nicht). Bilder (Montage
`scratchpad/G1B/a2_e2e.png`): F2200 Szenenende, F2380/F2430 Elza vor dem liegenden Roy, F2480 Titelseite "ROYS DIARY"
1/6, F2740 Seite 6/6 mit gruenem "0513" + EXIT, F2810 "The Roys Diary has been filed.", F2960 Hinweis 2F,
F3040/F3100 Hinweis B2, F3160/F3290 zurueck im Raum.

### Lauf a1 — Tuer mit Stand NACH dem Dialog, dann Lesen, zweites Untersuchen, Akten, Karte
Karte `1021 -4000 -18000 0 4:0xF1 4:0xFA 3:0x74` (sub02 setzt diese drei Flags am Ende des Dialogs),
`RE15_INPUT_SCRIPT=A0.1,W1,U2.5,R0.35,W0.5,A0.1,W5,(R0.1,W1.2)x6,W0.5,A0.1,W2.5,A0.1,W12,A0.1,W3,R0.1,W1.2,X0.1,W3,
S0.1,W2.5,M0.1,W3,X0.1,W2,Q0.1,W3,U0.1,W2,D0.1,W1.5,D0.1,W3`, `RE15_FRAMEDUMP=150-2250/5:f_`, `RE15_EXIT_AT=2250`.
debug.log (Auszug):

    [room] PC loaded room1011.rdt (210440 bytes)                     <- Tuer (Viereck F60)
    [input-script] Tick 134 -> F194 Tasten 0x8000                     <- Roy untersuchen
    [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, erstes Mal: ablegen)
    [tagebuch] Leser zu: Bit (9,68)=1, Kartenkette nach dem Evt_end
    [tagebuch] Evt_end erreicht: Kartenkette Eintrag 3 (...) -> Folge 4 (...)
    [hint] F729 begin / F847 Folge-Hinweis 3 -> 4 (Zeit) / F965 Zeit um / F965 schliessen (Abbruch)
    [input-script] Tick 977 -> F1037 Tasten 0x8000                    <- ZWEITES Untersuchen
    [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, schon abgelegt: nur lesen)
    [tagebuch] Leser zu: Bit (9,68)=1, kein Hinweis (schon abgelegt)   <- KREUZ F1169 schliesst
    START F1262 / R1 F1340 (FILE) / KREUZ F1433 / L1 F1496 (MAP) / HOCH F1589 / RUNTER F1652 / RUNTER F1700

`grep -c msgtext debug.log` = **0**. Bilder (Montagen `scratchpad/G1B/a1_m1_anlauf.png`, `a1_p2p3.png`,
`a1_p4p5.png`, `a1_p6end.png`, `a1_m3_filed_hint.png`, `a1_m4_zweit_files_karte.png`):
- F150..F210 Elza vor dem liegenden Roy; F250..F300 Titelseite "ROYS DIARY" auf der Organizer-Illustration, 1/6,
  gruener Weiter-Pfeil.
- Seiten (je eingeschwungen, 2x vergroessert gelesen) gegen AUFTRAG.md Z. 39-59 Zeile fuer Zeile:
  F380 2/6 "September 22nd / - / Umbrella... those bastards... / It was Umbrella who created / these freaks now
  crawling all / over the station. / - / The S.T.A.R.S. members told us / what was going on in the / Arklay Mountains,
  and what did / we do? ...We laughed them off." | F420 3/6 "Worse, we practically held the / door open for those
  things by / letting every bitten survivor / inside... / How could we have been so / stupid..." | F460 4/6
  "September 28th / - / Damn it, we're screwed. / Now someone has sabotaged / the Communications Room and / wrecked the
  radio equipment. / And of course, the others / didn't have to look far for / someone to blame. They went / straight
  for my friend John. / He's the only outsider here," | F500 5/6 "and he only got in because I / signed him in on a
  visitor's / pass... / They didn't even hesitate. / They threw him into a holding / cell downstairs. / - / There's no
  way in hell he did / this... / - / Anyway, they took the cell" | F535/F575 6/6 "key from me and locked it up / in the
  Communications Room so / I can't let him out. All I've / got left is my keycard for the / outer door... and its code:
  / 0513. / - / I just hope he's okay..." + EXIT.
  Jeder Satz und jede Leerzeile des Nutzertexts vorhanden, Reihenfolge gleich, kein Wort zusaetzlich; Abweichungen nur
  Form: Zeilenumbruch auf die Seitenbreite, Seitenwechsel (jedes Datum oben auf einer Seite), typografischer Apostroph
  -> Apostroph der Schrift, Titel auf der Titelseite in Grossbuchstaben ("ROYS DIARY"; in der FILE-Liste und der
  Meldung "Roys Diary" wie im Auftrag).
- Gruen (`gruen.py`, eingeschwungene Ende-Stellung F560/F575/F590 und a2 F2740/F2750): je **87** gruene Pixel, Huelle
  (36,114)-(67,121) = genau das "0513", sonst KEIN gruenes Pixel auf der Seite; Farben (0,184,40) x19, (0,160,32) x17,
  (0,136,24) x26, (0,96,16) x12, (0,80,8) x13 = CLUT-Eintraege 9..14 `16e0 1280 0e20 09e0 0980 0540` von
  FILE30_p05_page.TIM — **identisch** mit FILE28_p01 ("4312") und FILE29_p01 ("5632"), den vorhandenen
  Code-Dokumenten ("wie alle Codes in den Dokumenten"). Der Punkt hinter 0513 und der uebrige Text sind weiss.
- F610/F650 "The Roys Diary has been filed." (Name gruen wie bei jedem Dokument), F690 zurueck im Raum.
- Zweites Untersuchen: F1050 Raum, F1100 Titelseite 1/6, F1160 Seite 2/6, KREUZ -> F1200 direkt zurueck im Raum:
  keine "filed"-Meldung, kein Hinweis-Schirm.
- Akten: F1300 Statusschirm, F1380/F1420 "Files" = genau EIN Eintrag "Roys Diary" (kein Doppel nach zwei
  Untersuchungen).

### Urteil Punkt 1: ERFUELLT
Wortlaut je Teil: "in Elzas Szenario fuer Roy ein Tagebuch" (Elza-Lauf, Figur 4, Roys Diary) | "Hintergrund
FILE16_title_paper" (Organizer-Illustration auf Titel- und Textseiten; Byte-Gleichheit FILE30_title_paper == FILE16
prueft unit_r38_tagebuch_dokument, Abnahme 0 per md5) | "muss nicht unbedingt ein Welt Model haben, das taucht
einfach auf, wenn man Roy, der am Boden liegt noch einmal anklickt im Room 1011" (a2: nach der Szene Roy anklicken ->
Leser direkt, kein Weltmodell) | "statt der Code Message" (msg 18 0x im Log, bei jedem Untersuchen) | Header "Roys
Diary" + Text woertlich (s. o.) | "Code ... gruen wie alle Codes in den Dokumenten" (87 Pixel, CLUT identisch mit
FILE28/29).
