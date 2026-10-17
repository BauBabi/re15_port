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

## Punkt 2 — Karte: Communications Room und Prison Room markieren, "so wie bei Leon" (2. Sitzung, selbst gemessen)

### Hinweis-Schirm direkt nach dem ersten Lesen (Laeufe a1, a2)
a1: `[hint] F729 begin` -> `F847 Folge-Hinweis 3 -> 4 (Zeit)` -> `F965 Zeit um (Hinweis 4, 3 Blinkperioden)` ->
`F965 schliessen`; a2 (ganzer Nutzerweg): F2901 / F3019 / F3138 — dieselbe Kette, je 118 Bilder.
Bilder a1 (`scratchpad/G1B/a1_m3_filed_hint.png`): F690/F710 noch im Raum, F740..F830 "POLICE STATION 2F", die
Kachel Blatt 3 / Rechteck 9 wechselt gefuellt (F740) / Umriss (F760..F830); F860..F920 "POLICE STATION B2", die
T-Kachel Blatt 1 / Rechteck 3 gefuellt (F860) / Umriss (F880, F920); F990 zurueck im Raum. Log je Periode
"rot +Ton Se(2,0x2B)" / "umriss" (F730/F750/F769/F789 ...). a2 ebenso (F2960 2F, F3040/F3100 B2). Reihenfolge
"einmal den Communications Room ... und einmal den Prison Room" = erst 2F, dann B2; "Und danach" = erst NACH dem
Tagebuch (Kette am Evt_end von sub07, im Log nach "Leser zu").
Zielraeume: ROOM10F1 = Kartenzone Blatt 3 Rechteck 9 (re15_map_zones.h Hauptzeile, "COMMUNIC. ROOM"); ROOM1211 =
Blatt 1 Rechteck 3 ("DETENTI. AREA", Johns Zelle: ROOM1211 @0x1E36 `44 00 49 40 .. 8f 99 .. f4 9d` = Sce_em_set
Typ 0x49 bei (-26225, -25100), selbst gelesen). Das Tagebuch selbst nennt "holding cell downstairs" — passt zu B2.

### Gegenprobe "so wie auch schon bei Leon im Communications Room gemacht" (Lauf l1, Leon)
Leons Szene IM Communications Room (Runde 35 K) mit derselben exe: Karte `probe_r35_cut10f0_karte` (ROOM10D0 vor der
Tuer), `RE15_INPUT_SCRIPT=A0.2,W5`, `RE15_FRAMEDUMP=100-3200/10:f_`, `RE15_EXIT_AT=3200#10f0`:

    [room] PC loaded room10f0.rdt
    [cut10f0] ROOM10F0: Szene gestartet ... / [cut10f0] Szene zu Ende: Kartenhinweis ROOM11C0 -> ROOM1150 angefordert
    [hint] F2742 begin / F2854 Folge-Hinweis 1 -> 2 (Zeit) / F2964 Zeit um (Hinweis 2, 3 Blinkperioden) / F2964 schliessen

Bilder (`scratchpad/G1B/l1_leon_kette.png`): F2750..F2800 "POLICE STATION B1" Parkplatz-Kachel gefuellt/Umriss,
F2860..F2900 "POLICE STATION 3F" ROOM1150-Kachel gefuellt/Umriss, F2990 zurueck im Raum. Elzas Kette hat GENAU diese
Form (Schirm, Kachel-Blinken gefuellt/Umriss, Ton je Periode Se(2,0x2B), 3 Perioden ~118 Bilder je Ziel, Folge-Ziel,
schliessen).

### Normale Karte danach (Lauf a1)
START F1262 -> Statusschirm, L1 F1496 -> MAP "POLICE STATION 1F" (besuchte Raeume 1021/1011, Standort), HOCH F1589 ->
"2F", RUNTER F1652 -> 1F, RUNTER F1700 -> "B2" (B1 unbekannt, uebersprungen). Pixel in der Kachelmitte (`mitte.py`):
2F-Kachel (178,95): F1600..F1610 (0,16,88) Kartenblau, F1615..F1630 (48,8,48) gefuellt, F1635..F1650 wieder blau;
B2-Kachel (205,110): F1705/F1710 (48,8,64), F1715..F1730 (0,16,120), F1735..F1750 (48,8,64) -> beide BLINKEN in der
normalen Karte weiter (Form der Runde 33/35 bei Leon). Ende des Blinkens ("erreicht"): Comms = Prison Key genommen
(9,83), Prison = John befreit (4,0xFC); als PORT-WAHL im Header begruendet (Raum kann vor dem Lesen schon besucht sein,
gleiche Lage wie ROOM1150 in Runde 35 K), geprueft von unit_r38_tagebuch_karte (Pruefungen "ROOM10F1 nur betreten:
Comms blinkt weiter", "Prison Key genommen -> nur noch Prison", "John befreit -> kein Ziel mehr").

### Urteil Punkt 2: ERFUELLT
Beide Raeume werden markiert (Hinweis-Schirm nacheinander + Blinken in der normalen Karte), nach dem Tagebuch, in
Leons Form (Gegenlauf l1 mit derselben exe).

## RE-Gate (2. Sitzung, drei weitere Stichproben selbst gelesen; Werkzeug `scratchpad/G1B/dis.py` = `dis_one` aus
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, Rohbytes aus `info/`)

| # | Behauptung im Diff | Selbst gelesen | Ergebnis |
|---|---|---|---|
| S1 | RE15_TAGEBUCH_R38_MSG 18 = sub07 @0x00E64 `2b 12 ff ff`; EVT_END 0x1E = @0x00E82 - @0x00E64 | `xxd -s 0xE4E info/Re1.5/PSX/STAGE1/ROOM1011.RDT`: `0e4e 46 05 00..` / `0e58 2e 01 00 00` / `0e5c 3f 01 0b 00` / `0e60 09 0a 1e 00` / **`0e64 2b 12 ff ff`** / `0e68 02 00` / `0e6a 3f 01 0b 00` / `0e6e 43 00 80 00` / `0e72 09 0a 1e 00` / `0e76 42 00` / `0e78 46 05 03 31 ff 00 18 07 00 00` / **`0e82 01 00`**; `2b 12 ff ff` kommt in der Datei genau einmal vor (0xE64); md5 info == shared_assets (dfcbc099...) | BESTAETIGT |
| S2 | RE2 FUN_80071ba0: Id >= 0x68 -> `jal 0x800692dc` mit `a0 = Id - 104` (an die FILE-Liste VOR dem Lesen), Modus 4; FUN_800692dc ohne Doppelpruefung | `80071bbc 2ce20068 sltiu v0,a3,0x68` / `80071bc0 1040004f beq v0,zero,0x80071d00` / `80071d00 0c01a4b7 jal 0x800692dc` / `80071d04 24e4ff98 addiu a0,a3,-104` / `80071dc0 24020004 addiu v0,zero,4` / `80071dc4 a2220001 sb v0,1(s1)`; Sprungziel `800692dc addu a1,zero,zero` / `800692e0 addiu a2,zero,255` / `800692e8 addiu v1,v1,19304` (0x800d4b68) / `800692ec lbu v0,0(v1)` / `800692f4 bne v0,a2,0x80069304` / `80069300 sb a0,0(v1)` / `80069308 sltiu v0,a1,0x18` / `80069314 jr ra` — kein Vergleich mit a0 | BESTAETIGT (Doppel-Sperre des Ports = PORT-WAHL, im Header so gekennzeichnet) |
| S3 | Gruen = Textmaler Steuerbyte 0x05, Arg 1 -> CLUT-Zeile 482 @0x80028974-94; Seiten-CLUT 9..14 = diese Zeile | Sprungtabelle `8001099c..`: Index 4 (`800109ac`) = **0x80028968**; dort `addiu a2,a2,1` / `lbu v0,0(a2)` / `80028974 andi v1,v0,0x4` / `sltu v1,zero,v1` / `andi v0,v0,0x3` / `sll v0,v0,1` / `80028984 addiu v1,v1,480` / `addu v0,v0,v1` / `sll v0,v0,6` / `j 0x80028bd0` / `80028994 ori s5,v0,0x10` -> Arg 1: y = 480 + 2 = 482. `info/Re1.5/PSX/DATA/TEX.TIM` CLUT-Block x 256 y 480 w 32 h 24, Zeile y=482: `0000 16e0 1280 0e20 09e0 0980 0540 ...` = genau die CLUT-Eintraege 9..14 von FILE30_p05 / FILE28_p01 / FILE29_p01 und die 5 Farben im Bild (s. Punkt 1) | BESTAETIGT |
| S4 | SEITE_H 176 = RE2 @0x800AA184 `06 00 50 00`; ZIEL2 erreicht (4,0xFC) = ROOM1211 @0x01F76 `22 04 fc 01` | RE2 PSX.EXE `800aa184: 06 00 50 00` (Nachbarn `800aa180: 08 00 50 00`, `800aa188: 09 00 50 00`); `xxd -s 0x1F70 ROOM1211.RDT`: `1f76 22 04 fc 01` (danach `22 04 fa 00`) | BESTAETIGT |

Rate-Woerter im Diff (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen,
"deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|XXX|approx|geschaetzt|vermutlich|ungefaehr|nach gefuehl"):
**0 Treffer**. `getenv` in den `+`-Zeilen von engine/include/platform: **0**, in tests/tools: **0** — kein
Env-Schalter als Abschluss. Jede Konstante in `re15_tagebuch_r38.h` traegt Adresse/Byte-Offset oder die Kennzeichnung
NUTZER-VORGABE / PORT-WAHL (RAUM 0x1011 @0x009F6/@0x00A3E — in meiner SCD-Ausgabe `0x009F6 Sce_em_set 44 00 47 40`,
`0x00A3E` dto.; MSG/EVT_END S1; DOK 5/Item 0x4D = 0x48 + 5, 0x48 aus DEBUG.BIN @0x800c7370 (1. Sitzung bestaetigt);
BILDSATZ 30 PORT-WAHL; MAX_PAGE 5 = Satzbestand p00..p05, im Bild 6 Seiten "1/6".."6/6"; SEITE_H S4; BIT 68 VERTRAG;
ZIEL1 0x10F1/(9,83) schluessel_r36.h:60/67; ZIEL2 S4). Listenname "Roys Diary": Kodierung ASCII - 0x24 nachgerechnet
(R 0x2e, o 0x4b, y 0x55, s 0x4f, Leerzeichen 0x00, D 0x20, i 0x45, a 0x3d, r 0x4e, y 0x55, Ende 0x07) und im Bild
"Files: Roys Diary" gelesen.
Erklaert der Fix den Befund? Vorher sub07 @0x00E64 oeffnet msg 18 (Abnahme 0 Lauf v1 mit v0.8.25); der Haken ersetzt
genau dieses Message_on (Raum 0x1011 + Id 18) durch den Dokument-Leser und laesst den Faden danach hinter das Opcode
laufen; nachher (a1/a2) 0 x msgtext id 18, Leser, Evt_end, Kette. Ja.

## Vertrag / Pfade / Nebenwirkungen (2. Sitzung, selbst geprueft)

* Pfade (`git diff master...HEAD --name-only`, ohne analysis/): engine/src game_step_common.c, map_hint_common.c,
  menu_common.c, re15_files.c, scd_vm.c, tagebuch_r38.c (neu); include/re15_tagebuch_r38.h (neu);
  shared_assets/RE2/FILES/FILE30_* (8 neu); tests (integration/test_r38_tagebuch.cmake, unit/probes/r38_tagebuch.cmake,
  unit/test_r38_tagebuch.c, unit/probe_r38_tagebuch_bild.c, Alt-Riegel probe_r26_inventar.c, test_r36_kartenbits.c,
  test_r36_schluessel.c, test_re2doc_bildebene.c); tools/r38_g/*.py. `grep -E "^release/|platform/android|
  shared_assets/PSX/|CMakeLists"` = **leer** (Test-Einbindung ueber probes/*.cmake-GLOB). master hat seit der Basis
  e2cc7673 keine re15_port-Aenderung (`git diff e2cc7673 master --stat -- re15_port` leer).
* Gemeinsame Dateien = Haken (numstat): game_step_common.c +2, scd_vm.c +4, map_hint_common.c +9 (Include + 2
  Tabellenzeilen + Kommentar), re15_files.c +9 (Name + 1 Zeile), menu_common.c +9/-1; je Kommentar "Runde 38 Spur G".
  Die Logik liegt in tagebuch_r38.c (111 Zeilen).
* Ressourcen: Bank-9-Bit 68 (Zuteilung G) als einziges neu belegtes Bit, in k_port[] eingetragen
  (test_r36_kartenbits.c +2); gelesen (nicht belegt) werden (9,83) Prison Key und (4,0xFC) John befreit. Keine
  Nachrichten-Id belegt (1011 40-44 ungenutzt). Dokument 5 / Item 0x4D / Bildsatz 30 per Zensus.
  Kollisionssuche ueber alle anderen `.claude/worktrees/r38_*` (Diff je ab merge-base mit master, `+`-Zeilen in
  re15_port nach "9,68 | 9:68 | BIT 68 | 0x4D | FILE30 | Roys Diary"): keine Treffer ausser Binaerdaten-Bytes 0x4d und
  NPC-Typbereichen 0x40..0x4d (r38_cursor/dining1141/itemorte/npckante/szenenschutz/integration) — kein Bezug.
  Keine andere Spur aendert re15_files.c, map_hint_common.c oder legt FILE3x an.
* Integrationsstellen (kein Mangel, Hinweis): menu_common.c aendert im Integrationszweig r38/integration auch Spur Q
  (Inventar-Ton in re15_menu_start_poll / tab_select) — andere Hunks, aber das Q-Include steht an DERSELBEN Stelle wie
  das G-Include (beide direkt nach `#include "re15_cut10f0.h"`) -> beim Merge beide Zeilen behalten. Der Leser des
  Tagebuchs laeuft nicht ueber re15_menu_start_poll/tab_select (Anforderung re15_menu_request_doc; "nur lesen"
  schliesst ueber file_reader_close -> s_phase 2). test_r36_kartenbits.c k_port[]: Spur F traegt 64/66/67 ein, G 68 ->
  beide Seiten uebernehmen. scd_vm.c / game_step_common.c werden von F, M, R, N, D und der Integration an anderen
  Stellen beruehrt.
* Nebenwirkungen, selbst gemessen:
  - Leon: Lauf l1 (Leons Szene im Communications Room + Kette 11C0 -> 1150) unveraendert; die Eintraege 3/4 haengen
    an (9,68), das nur das Ablegen von Dokument 5 in ROOM1011 setzt.
  - Erstes Lesen VORZEITIG abgebrochen (Lauf a3, Karte `1011 -1900 6830 3072 4:0xF1 4:0xFA 3:0x74`,
    `RE15_INPUT_SCRIPT=A0.1,W4,R0.1,W1.2,X0.1,W3,A0.1,W12`): KREUZ auf Seite 2 (F222) -> trotzdem "filed"-Meldung
    (Viereck F315) -> `Leser zu: Bit (9,68)=1, Kartenkette nach dem Evt_end` -> Kette F370 / F488 / F606 — RE2-Form
    (das Dokument kommt beim Aufheben in die Akten, nicht erst am Ende).
  - Zweites Untersuchen (a1): nur lesen, Liste bleibt 1 Eintrag, kein zweiter Hinweis.
  - Speicherstand: die FILE-Liste ist Teil des Spielstands (re15_savedata.c:233 `re15_files_export(out->files)`,
    :333 `re15_files_import`, v9), Bit 68 liegt in g_game.flags -> "nur lesen" und das Blinken ueberleben
    Speichern/Laden.
* Tests je Punkt (vorhanden und messend, Code gelesen): Punkt 1 unit_r38_tagebuch_dokument (Tabelle, FILE30-Bytes,
  Papier == FILE16, 87 Gruen-Pixel ueber den echten Seitenlader) + _ablauf (echte VM + Statusschirm: Parken an
  @0x00E64, keine Nachricht, Liste vor dem Lesen, "filed", Bit 68, Kette erst am Evt_end, zweites Untersuchen nur
  lesen) + integration_r38_tagebuch_lesen (exe: kein msgtext 18, Gruen im Bild, Kette im Bild); Punkt 2
  unit_r38_tagebuch_karte (Kette/Folge/Ziele/Erreicht-Regeln, Leon unveraendert) + integration_r38_tagebuch_karte
  (normale Karte, beide Kacheln blinken); Gegenprobe unit/integration_r38_tagebuch_leon.
