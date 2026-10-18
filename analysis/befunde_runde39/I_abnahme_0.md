# Runde 39 — Spur I "funk10f1": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-18), Baum `.claude/worktrees/r39_funk10f1`, Zweig `r39/funk10f1`, Kopf c95c4976
(Merge-Basis mit master 22abaed9; master ist nur um den Doku-Commit 37011982 voraus — die Zeilen UEBERGABE.md /
args_welle3_forts.json in `git diff master --stat` stammen von master, nicht von der Spur; Spur-Diff = `master...HEAD`,
17 Dateien, +1861/-5).

Status: **FERTIG — BESTANDEN** (alle 4 Nutzer-Punkte erfuellt, alle Gates halten, 0 Maengel, 5 nicht blockierende Hinweise §12).

## 0. Gelesen
- AUFTRAG.md (Punkte 10F1 woertlich), VERTRAG.md (I = Bit (9,70), Ids 10F1 40..44), Dossier I_funk10f1.md.
- Code-Diff vollstaendig: funk10f1_r39.c (neu), gen/funk10f1_r39_szene.inc (neu, erzeugt), re15_funk10f1_r39.h (neu),
  Haken scd_room_setup.c (+2), scd_vm.c (+2), platform/pc/src/cut10f0_pc.c (+5/-2), test_r36_kartenbits.c k_port (+2),
  3 fremde Integrations-Riegel (+RE15_SET_FLAG=9:70), eigene Riegel test_r39_funk10f1.c / .cmake / probes/r39_funk10f1.cmake,
  Werkzeuge tools/r39_funk10f1/*.py.

## 1. Bau und eigene Riegel (gemessen)
- `bash re15_port/tools/local_build.sh configure` + `build` im Spurbaum: `=== LOCAL-BUILD-OK (configure)` /
  `=== LOCAL-BUILD-OK (build)` (762 Schritte, exe 18:40, enthaelt "Funkszene").
- `ctest --test-dir re15_port/build -R "r39_funk10f1|unit_r36_kartenbits" --timeout 900 -j2`:
  **100% tests passed, 0 tests failed out of 8** (integration echt 69,2 s, zweit 19,9 s, leon 8,3 s; unit form/texte/
  szene/einmal, kartenbits). Unit-Ausgabe selbst gelesen (szene: Motions 105 2 ... 11 0 9 12 200, Stimme 40 41 42,
  Druck bei (6862,6368) Gierung 4057 Cut 1, 99 Bilder, Ende Bild 476, frei Bild 490).

## 2. Messlauf w1 = ECHTER WEG (Elza laeuft in ROOM10D1 selbst in die Kartenleser-Tuer)
Befehl (eigene exe-Kopie `re15_pc_abn0I_w1.exe`, Laufordner Scratch `abn0/runs/w1`, Stellvertreter-Stimmen NUR im Laufordner:
`synchro/STAGE1/room10F1/main40.wav` = Kopie room1031/main19.wav, `main42.wav` = Kopie room1031/main18.wav):
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
RE15_SET_FLAG=3:50 RE15_DEBUG_JUMP=10D1@250 RE15_PLAYER_POS=2000,-7050,2048 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=320 RE15_INPUT_SCRIPT=U3,A0.1,W0.3,A0.1,W0.3,A0.1 RE15_STATE_LOG RE15_ANIM_TRACE RE15_MSG_LOG=1
RE15_FRAMEDUMP=0-720/10:fd/f RE15_EXIT_AT=720#10f1 RE15_AUDIO_CAP_SYNC=cap.raw` -> exit 0, 70 s.
(Kein RE15_FIRE_AOT: Elza geht 3 s gegen die Westwand von 10D1 und drueckt VIERECK; (3,50) = Kartenleser benutzt, ROOM10D1
main00 @0x01026 `21 03 32 00` Ck(3,0x32) -> Else-Zweig @0x01174 Door_aot_set Slot 0, selbst disassembliert.)
debug.log: `[pl] Spieler-Familie PL04` -> `[debug-menu] JUMP -> 10d 2F CORRIDOR (ROOM10D1)` -> `[input-script] Tick 90 -> F410
Tasten 0x8000` -> `[aot] DOOR FIRE slot=0 rect=(800,-7050,hw=500,hh=1000) target_cut=0 spawn=(8400,0,-350)` -> `[tuer] Sequenz
Archiv 2 DOOR13 ... 286 Bilder` -> `[room] PC loaded room10f1.rdt` -> `[funk10f1] F410 ROOM10F1: Funkszene gestartet (Ereignis 27,
Faden 10, Flag (9,70)=0)` -> `[rbj] Animationsblock von ROOM1031 geliehen (26760 B, Runde 39 Spur I)` -> Plc_dest 9 (6250,2900)
/ 4 (6250,2900) / 4 (6250,5900) / 4 (6900,6400) / 9 (7400,6400) -> `[scd F127] Cut_chg(1)` -> `[msgtext] room=10f1 id=40 "Elza:
Hello, anyone, can you hear me? We Need help!"` + `[voice] clip loaded: main40.wav` -> id=41 "Elza: ..." + `keine Aufnahme
(synchro/STAGE1/room10f1/main41.wav)` -> id=42 "Elza: Uhh, this Thing is broken!" + `[voice] clip loaded: main42.wav`.
state.txt (10F1-Abschnitt ab Zeile 662): F1..F5 Tuer (8400,-350) rot 2048 pm=0 bei pf=FF000007 (Einblendung nach der
Tuersequenz, Spiel pausiert — kein Steuerfenster); F6 pm=2 rot 2144 -> F11 2624 -> F20 2691
(8106,84) Gang; F70 cam=1; F120 (6805,6321); F127 mo=11 bei (6862,6368) rot 4057; msg 40 F147..F245; F247 mo=0 (Senken);
msg 41 F277..F375 mo=9; msg 42 F382..F508 mo=12; F496 pm=0 (frei), Elza bleibt bei (6862,6368) Cut 1. Das waehrend der
Szene ab F320 erneut ablaufende Eingabeskript (U 3 s, VIERECK bei F410/F422/F434) aenderte NICHTS (pm=2, Position fest) —
die Szene ist gegen Tastendruecke fest.

### 2.1 Bilder w1 (RE15_WINDOW_SCALE=1, 320x240, angesehen; Blaetter Scratch `abn0/runs/w1/blatt_a|b|c.png`, Ausschnitte
`crop_druck.png`, `crop_punkte.png`, `crop_kaputt.png`)
- F10..F60 Cut 0: Elza an der Tuer (hinten links), dreht sich zur Kamera und geht auf die Kamera / die Konsole im linken
  Vordergrund zu; F70..F110 Cut 1: sie geht den Gang entlang, Konsole links mit Monitoren/Tastatur/Telefonen; F120 sie dreht
  sich zur Konsole.
- F130/F140/F200/F240: linker Arm nach vorn AUF dem Bedienfeld der Konsole (Hand an der dunklen Knopfreihe) — sichtbares
  "Druecken"; F160..F240 Untertitel zweizeilig "Elza: Hello, anyone, can you hear me? / We Need help!" bei gehaltener Hand.
- F250/F260 Arm sinkt; F280..F370 "Elza: ..." Hand an der Huefte, Kopf dreht (Umsehen); F390..F420 Hand an der Stirn bei
  "Elza: Uhh, this Thing is broken!", F430..F480 Arm unten; F490/F500 Balken fahren weg, Elza steht an der Konsole.
- ANIM_TRACE Spieler 10F1: F127 Motion 11 Clip 11 (2 Bilder) gehalten bis F247 OHNE Zwischenwechsel, F247 Motion 0 Clip 0
  (20 Bilder) rueckwaerts=1, F277 9/9 (24), F382 12/12 (50), F482 200 — der Arm hebt sich genau EINMAL und senkt sich EINMAL
  (kein "zweimal heben" wie im Nutzerbefund ROOM1031, Spur H).
- Zusatzlauf w3 (gleicher Weg, RE15_WINDOW_SCALE=3, Bilder F120..F480/30, `abn0/runs/w3/crop3_druck.png`): F150/F210
  Hand auf dem Bedienfeld bei "Hello, ...", F270 Arm unten, F330 Hand an der Huefte bei "...", F390/F420 Hand an
  der Stirn bei "Uhh, this Thing ..." — gleiche Gesten in hoeherer Modell-Aufloesung.

### 2.2 "Rechts" — drei unabhaengige Belege
1. Original-Drehrichtung selbst disassembliert (PSX.EXE): @0x80033000 `andi v0,a0,0x8` (virtuell LINKS) -> @0x80033040
   `subu v0,v0,v1` -> `sh v0,-13634(at)` (0x800acabe = Spieler-Gierung); @0x8003304c `andi v0,a0,0x2` (RECHTS) -> @0x8003308c
   `addu v0,v0,v1` -> `sh` 0x800acabe. RECHTS = Gierung PLUS. Port-Gang: (8106,84)->(7686,704) bei rot 2691/2693 =
   (cos, -sin) — eine Rechtsdrehung aus 2048 (Blick -X) fuehrt nach 3072 = +Z.
2. Gemessen: rot 2048 (F1..F5) -> 2144, 2240 ... 2624 (F11) -> 2691 (F20): Gierung steigt = Rechtsdrehung; erster
   Wegpunkt z 2900 > -350.
3. Bilder Lauf k1 F5..F21 (`abn0/runs/k1/crop_dreh2.png`): F5 Elza im Profil mit Blick nach Bild-rechts (in den Raum, -X;
   die Konsole x 7000..7800 > Kamera-x 6535 steht links = +X ist Bild-links), F7..F11 sie dreht sich zur Kamera (= ihre rechte
   Seite), F13..F21 sie geht auf die Kamera zu.
- Das Kommunikationssystem ist eindeutig: ROOM10F1 main00 hat genau EINE Zone mit msg 0 "A large communications device. It's
  out of commission. ..." = Aot_set Slot 2 @0x00F72 `2c 02 01 31 00 00 58 1b 28 0a 20 03 e4 25 00 00 ff ff 00 00` (x 7000..7800,
  z 2600..12300, selbst dekodiert); die grauen Schraenke rechts im Bild sind Slot 5 msg 1 "Nothing suspicious" (x 1950..2750).
  Vom Eintritt (8400,-350) Blick -X liegt die Zone vorn-rechts (+Z). Aus Sicht des Bildes (Cut 0) steht die Konsole LINKS im
  Vordergrund — "Rechts" ist also aus Elzas Sicht erfuellt; eine Bild-rechts-Lesart haette kein Kommunikationssystem als Ziel.

### 2.3 Stimme (Ton-Mitschnitt w1, RE15_AUDIO_CAP_SYNC, 2349 Ticks = 1470 Stereo-Frames je Spielbild, letzter Tick = 10F1 F720)
Normierte Kreuzkorrelation der Stellvertreter-Dateien (32 kHz -> 44,1 kHz) gegen den Mitschnitt (Scratch `abn0/py/korr.py`):
- main40.wav: Maximum r = 0,971 bei Raumbild 148,00 (msg 40 erstes Bild F147), naechstbester Wert 0,074, Verstaerkung 0,996.
- main42.wav: Maximum r = 0,874 bei Raumbild 383,00 (msg 42 erstes Bild F382), naechstbester 0,055, Verstaerkung 1,096.
- msg 41: `[voice] ROOM10F1 msg 41: keine Aufnahme (...main41.wav) - Zeile bleibt stumm` (angefordert).
=> Liegt eine Datei unter synchro/STAGE1/room10F1/, spielt sie genau mit ihrer Zeile (Lader probiert room10F1 und room10f1,
audio_pc.c:2243/2260 — Gross-/Kleinschreibung auch unter Linux abgedeckt). Die drei echten Aufnahmen fehlen (Nutzer).

## 3. Nebenwirkung: Fortschritt NACH der Szene (Lauf k2, in keinem Riegel der Spur gemessen)
Die drei angepassten Fremd-Riegel (klettern B, dachkiste elza, optik A2) setzen jetzt (9,70)=1 und messen damit NICHT mehr
"Elza nach der Funkszene". ROOM10F1 -> ROOM1091 (Tritt schieben, hochklettern, Tuer oben) ist Elzas EINZIGER Weg weiter
(ROOM1091 Door_aot_set nur Slot 0 -> 0x05, Slot 1 -> 0x10, selbst gelesen @0x0211A/@0x0213A). Deshalb selbst gemessen:
Befehl wie w1 (echter Tuerweg, ohne (9,70)), dazu `RE15_CLIMB_TRACE=climb.txt RE15_EXIT_AT=60#1091` und Eingabeskript ab
Bild 320 je Raum: `U3,A0.1,W0.3,A0.1,W0.3,A0.1,W2.8` (10D1: Tuer) + ab 10F1-Bild 521 (nach der Szene)
`L0.7,W0.3,U0.47,W0.3,R0.37,W0.3,XU0.47,W0.4,L0.37,W0.4,XU0.9,W0.4,L0.37,W0.4,XU1.3,W0.4,R0.37,W0.4,XU1.45,W0.4,R0.37,W0.4,
XU0.63,W0.4,R0.37,W0.5` + der Schluss von test_r36_klettern.cmake (`U2.6,...,A0.1,W3`). -> exit 0, 105 s.
- debug.log: `[funk10f1] ... gestartet` -> `[rbj] Animationsblock von ROOM1031 geliehen` -> ... `[aot] DOOR FIRE slot=1
  rect=(-4750,-1750,hw=600,hh=500)` -> `[room] PC loaded room1091.rdt` -> `[flow] EXIT_AT: Bild 60 in Raum 1091 erreicht`.
- climb.txt: Szene T788..T1143 (mo 11/0/9/12/200), dann Gang; T1598 `push=5` mo=18 (Tritt wird geschoben), T2145 `sub=9
  kind=0x81` (Klettern), T2197 `pos=(-5015,-1800,-1244) band=1 ... stand=3` (oben), danach Tuer -> ROOM1091.
- ANIM_TRACE: Schiebe-Start Motion 0 = Clip 0 mit 34 Bildern (Basisbank, NICHT der geliehene 20-Bilder-Clip 0 von ROOM1031),
  17 (10), 18 (25), Klettern 230/231 — der geliehene Block greift nur in die Skript-Motions der Szene.
=> Mit dem geliehenen Gestenblock bleibt der Weg nach ROOM1091 offen. (Erster Versuch k1 mit falsch geschaetzter Laufdauer
endete an der Nordwand — Wegfehler des Pruefers, kein Spielbefund; k2 mit korrigierten Zeiten durchgelaufen.)

## 4. RE-Gate (selbst nachgeprueft)
### 4.1 Stichproben — Adressen SELBST disassembliert/gelesen (richtige Binaerdatei, Sprungziel bzw. Satzanfang)
| Zitat im Diff | Datei | selbst gelesen | Ergebnis |
|---|---|---|---|
| ROOM1031 sub13 @0x02982..@0x02A58, sub14 @0x02A5A | shared_assets/PSX/STAGE1/ROOM1031.RDT (== info/Re1.5, `cmp` 0 Diff) | Sub-Tabelle @RDT+0x44=0x020BC: 16 Subs, sub13 = 0x02982, sub14 = 0x02A5A; lineare Disassembly (eigenes Skript, Laengen = scd_vm.c s_opcode_sizes): @0x029A4 `3f 00 0b 00`, @0x029A8 `43 00 04 00`, @0x029AC `09 0a 14 00`, @0x029BE `3f 00 00 00`, @0x029C2 `43 00 80 00`, @0x029C6 `09 0a 1e 00`, @0x029CC `40 00 09 20 77 dc a1 aa`, @0x029D6 `40 00 04 20 ..`, @0x029E0 `40 00 09 20 ..`, @0x02A00 `3f 00 0c 00`, @0x02A04/@0x02A14/@0x02A44 `09 0a 64 00`, @0x02A08 `2b 12 00 00`, @0x02A32 `3f 00 09 00`, @0x02A36 `09 0a 05 00`, @0x02A3A `41 04 03 00 00 00 00 00 3c 00`, Schwanz @0x02A48..@0x02A59, sub14 `11 00 08 00 02 00 12 04 21 05 20 00 01 00` | stimmt Byte fuer Byte; der Generator prueft dieselben Bytes beim Erzeugen, unit_r39_funk10f1_form vergleicht das Programm gegen die Datei |
| Druck = Rolltor-Notschalter | ROOM1031.RDT | Member_set @0x02994/@0x02998/@0x0299C = (-13097,-21855) Gierung 1024 (-Z); Aot_set Slot 7 @0x01D88 `2c 07 01 31 00 00 4a c8 6c 9d 72 05 f2 0c 02 00 ff ff 00 00` = x -14262..-12868, z -25236..-21922, msg 2 @0x2B3B "The emergency shutters have / already been activated." | Elza steht 67 Einheiten vor der Schalter-Zone und blickt hinein -> Clip 11 + Halten = ihre Druck-Geste; Lesart belegt |
| ROOM10F1 Aot_set Slot 2 @0x00F72, msg 0 @0x1174 | ROOM10F1.RDT (== ROOM10F0.RDT == info/Re1.5) | s.o. §2.2; Sub-Tabelle @0x10D4 = 2 Subs, RDT+0x5C = 0 | stimmt |
| ROOM10D1 @0x01174 Door_aot_set Slot 0 | ROOM10D1.RDT | `3b 00 02 31 00 00 2c 01 8e e0 e8 03 d0 07 d0 20 00 00 a2 fe 00 08 00 0f 00 ...` = Rechteck x 300..1300 z -8050..-6050, Ziel (8400,0,-350) Gierung 2048, Raum 0x0F, Cut 0; im Else-Zweig von @0x01026 Ck(3,0x32) | stimmt |
| Einmal-Riegel-Form ROOM11B0 sub06 @0x01478 | ROOM11B0.RDT | sub06 beginnt @0x01478 `22 03 83 01` | stimmt |
| Cut_auto-Form ROOM1050 sub03 @0x00DF4 | ROOM1050.RDT | @0x00DF4 `3c 01` | stimmt |
| Elza-Kopf/Ende ROOM1031 msg 19 @0x2FA7 | ROOM1031.RDT | `04 00 05 01 21 48 56 3d 16 05 00 00` ... @0x2FC8 `04 01 01 63` | stimmt |
| Raumvariante @0x800397e4/@0x800397ec (Kopf re15_funk10f1_r39.h) | info/Re1.5/PSX.EXE | `srl a0,a0,31` / `jal 0x80013b60` / Delay-Slot `addu a0,a0,v0` | stimmt |
| Bank 9 = 0x800b1078 = 0x80074664[9] | PSX.EXE | `read 0x80074664`: Eintrag 9 = 2148208760 = 0x800B1078 | stimmt |
| (zusaetzlich, Beleg fuer "rechts") Drehrichtung | PSX.EXE | @0x80033000..@0x80033094, s. §2.2 | RECHTS = Gierung plus |

### 4.2 Konstanten des Diffs
Jede verhaltensrelevante Konstante traegt @0x/Byte-Offset oder ist gekennzeichnet: Bit (9,70) = VERTRAG 1.1; Ids 40/41/42 =
VERTRAG 1.2; Ereignis 27 = Port-Nummer, frei (eigener grep: belegt 2..6, 13, 20..26, 28, 29; nur 27 = funk10f1); Ankunftsbit 0x20,
Clips 11/0/9/12, Plc_flg 4/0x80, Sleeps 20/100/30/5/100/100, Plc_neck-Parameter, Rahmen/Schwanz = ROOM1031 @0x...; Set zuerst =
ROOM11B0 @0x01478; Cut_auto 1 = ROOM1050 @0x00DF4; Kopf/Ende der Texte = ROOM1031 @0x2FA7; Gestenquelle 0x1031 = RDT+0x5C
0x307C (26760 B, im Log bestaetigt). PORT-WAHL (gekennzeichnet, Form belegt): Weg (6250,2900)/(6250,5900), Platz (6900,6400),
Blick (7400,6400), Kamera Cut 1 — aus Kollisionszellen (Radius 468/457 gemessen), Text-Zone Slot 2 und Kamerabild. Texte =
NUTZER-VORGABE (woertlich), Zeilenumbruch = Port-Satz unter dem gemessenen Korpus-Budget 285 px.
`git diff master...HEAD -- re15_port | grep -i "deferred|tunable|interim|for now|faithful|plausib|TODO|approx|getenv"`: 0 Treffer.
Kein Env-Schalter als Abschluss. Kein Asset-Patch (shared_assets unberuehrt).

### 4.3 Erklaert der Fix den Befund?
Befund vorher (Dossier §1, nachvollziehbar am Code): ROOM10F1 hat im Auslieferungsstand keine Szene (2 Subs: Tritt/Flag,
Member_cmp; main00 nur Tueren/Texte/Props) — es gab nichts, was beim Betreten lief. Die Umsetzung fuegt die Szene ueber den
Installer am Ende von scd_room_reenter + Ereignis 27 hinzu; gemessen startet sie genau beim ersten Betreten (w1/k2) und nicht
beim zweiten (integration echt Teil 2 / zweit, unit einmal). Ursache/Wirkung passen.

## 5. Vertrag (VERTRAG.md) und Pfade
- Bank-9-Bit: nur **70** (VERTRAG 1.1: I = 70); k_port[]-Eintrag (+1 Zeile + Include) in test_r36_kartenbits.c; Lauf
  test_r36_kartenbits: `ok: Bit 70 RE15_FUNK10F1_GESEHEN_BIT ... — im Original frei`, `41 Port-Bits gegen 85 Original-Bits:
  0 Kollision(en)`.
- Nachrichten-Ids: nur 40/41/42 in ROOM10F1 (VERTRAG 1.2: I = 10F1 Ids 40..44); kein anderes Port-Modul setzt in 10F1 Texte
  mit diesen Ids (grep re15_msg_install_text: schluessel_r36 = 28/29 im Tuerraum, kartenitems_r36 = andere Raeume);
  synchro/STAGE1/room10F1 existiert nicht -> keine Nutzer-WAV umgewidmet.
- Ereignis 27: einziges Vorkommen unter allen RE15_*EREIGNIS (eigener grep); Haken liefern nur fuer ihren Raum.
- Gemeinsame Dateien nur kleine Haken: scd_room_setup.c +2 (Include + Installer-Zeile am ENDE des Blocks, nach Spur C),
  scd_vm.c +2 (Include + 1 Zeile in der scd_event_fire-Kette), platform/pc/src/cut10f0_pc.c +5/-2 (Leih-Quelle-Rueckfall +
  Logtext; fuer 10F0 unveraendert, da re15_cut10f0_rbj_quelle zuerst gefragt wird), test_r36_kartenbits.c +2.
- `git diff master...HEAD --name-only`: keine Datei unter release/, platform/android/, shared_assets/PSX/, keine
  Test-CMakeLists (Registrierung nur in tests/unit/probes/r39_funk10f1.cmake, GLOB). Drei FREMDE Integrations-Riegel um je
  eine Umgebungszeile ergaenzt (`RE15_SET_FLAG=9:70` in test_r36_dachkiste / test_r36_klettern / test_r39_optik_schluessel) —
  Muster Runde 35 K (9:71); die dadurch entfallene Abdeckung "Elza NACH der Szene" habe ich mit k2 (§3) und k3 (§8) selbst gemessen.
- Neue Riegel parallel-/Linux-fest: eigener Laufordner `${WORKDIR}_${TEIL}/${name}_${lauf_id}`, exe-Kopie
  `re15_pc_r39funk_${TEIL}_${lauf_id}${_exe_ext}` (keine .exe-Endung hart), `cmake_minimum_required` im -P-Skript, `m` gelinkt
  (sqrt) bei NOT WIN32; Stellvertreter-WAV nur im Laufordner (Nutzer-Ordner synchro/ unberuehrt).
- Eigene Commits beruehren nur die 17 Spur-Dateien (Dossier + Code + Riegel + Werkzeuge).

## 6. Gezielte Testlaeufe (KEINE volle Suite)
- Unit-Gruppen der beruehrten Module/Raeume (Regex des Dossiers §5.1 ohne die langen exe-Riegel, plus
  unit_r38_itemorte_raum), `-j2`: **92 von 93 gruen**, rot nur **unit_r38_itemorte_raum** — Pruefung "R 1030: Loescher-Prop
  obj 5 an der gemessenen Lage" (test_r38_itemorte.c:384-386, `band == 1`). Ursache selbst nachgesehen:
  feuerloescher_r37.c:185 `band = re15_elza_r39_prop_band_ueber_boden(g->y)`, letzter Commit an der Datei 08f3094d
  "wip(r39-elza)" (Spur H, liegt in master und allen Welle-3-Zweigen). Die 10F1-Pruefungen desselben Tests sind gruen; Spur I
  beruehrt ROOM1030/den Loescher nicht. -> NICHT Spur I, bei der Integration abzugleichen (wie im Dossier gemeldet).
- Lange exe-Riegel der beruehrten Raeume/Haken, `-j4`: integration_r39_optik_schluessel **Passed 225,1 s**,
  integration_r35_cut10f0 (Leons 10F0-Szene, cut10f0_pc.c-Haken) **Passed 242,8 s**, integration_r36_klettern
  **Passed 516,7 s**, integration_r36_dachkiste **Passed 1378,1 s** -> `100% tests passed, 0 tests failed out of 4`
  (die drei von der Spur angepassten Fremd-Riegel + Leons 10F0-Szene).

- Weitere exe-Riegel, die ROOM10F1 mit Elza betreten (Ladeweg/Sprung, ohne (9,70)), `-j3`: integration_r36_schluessel
  Passed 85,4 s, integration_r38_tagebuch_lesen 69,1 s, _karte 21,5 s, _leon 23,5 s, integration_r38_cursor 192,4 s,
  integration_r39_elza_karte 43,2 s, _hand 86,8 s, _loescher 41,8 s, integration_r38_itemorte 339,7 s ->
  **100% tests passed, 0 tests failed out of 9**.
- Summe der gezielten Laeufe: eigene 8/8, Unit-Gruppen 92/93 (rot = Spur H, s.o.), lange exe-Riegel 4/4 + 9/9.

## 7. Zweites (und n-tes) Betreten ueber die ECHTEN Tueren (Lauf z1)
Befehl wie w1, aber `RE15_PLAYER_POS=1700,-7000,0` (= Ankunftspunkt der 10F1->10D1-Tuer) und ein Skript, das in JEDEM Raum ab
Bild 320 laeuft: `R0.74,U1.5,A0.1,W0.3,A0.1` (10D1: umdrehen, zur Westwand, VIERECK = Tuer nach 10F1) + `W3.95` + ab Bild 521
`R0.37,W0.3,U3.0,W0.3,L0.37,W0.3,U1.2,A0.1,W0.3,A0.1` (10F1 nach der Szene: nach Sueden, nach Osten in die Tuer, VIERECK).
Der Lauf pendelt dadurch 10D1 <-> 10F1 bis zur Zeitgrenze (600 s, exit 124 = gewollt).
- debug.log: `loaded room10f1` **14x**, `[funk10f1] ... gestartet` **1x**, `von ROOM1031 geliehen` **1x**, `[msgtext] room=10f1
  id=40/41/42` je **1x** (nur im ersten Besuch), `room 10F1 has no RBJ` **13x**; Tuer 10F1->10D1 `[aot] DOOR FIRE slot=0
  rect=(9300,-300,hw=500,hh=1000) ... spawn=(1700,0,-7000)` nach der Szene (Pad wirkt, Elza verlaesst den Raum).
- state.txt Abschnitt 3 (erster Besuch): 476 Bilder pm=2 (Szene). Abschnitt 5 (zweiter Besuch): F1 (8400,-350) pm=0, F100
  unveraendert (keine Szene, kein Gang), F330 Drehung per Pad (rot 3104), F360 Gang (8832,-483) — Spieler frei.

## 8. Nebenwirkung: Prison Key in ROOM10F1 NACH der Szene (Lauf k3)
Der Riegel optik A2 setzt jetzt (9,70)=1, integration_r36_schluessel nimmt den Ladeweg (CONTINUE, ohne Szene) — "Elza nimmt den
Schluessel im SELBEN Besuch nach der Funkszene" misst keiner davon. Selbst gemessen: echter Tuerweg wie w1, nach der Szene
`L0.7,W0.3,U0.47,W0.3,R0.37,W0.3,XU0.47,W0.4,L0.37,W0.4,XU0.9,W0.4,L0.37,W0.4,XU2.0,W1,A0.1,W3.5,A0.1,W4` (an die Pinnwand,
Blick -Z, VIERECK, "Ja"), `RE15_EXIT_AT=1150#10f1` -> exit 0, 85 s. debug.log: `[funk10f1] ... gestartet`, msg 40/41/42, dann
`[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)` + `Prison Key genommen -> Flag (4,0xEC) = 1 (Zelle ROOM1211)`;
state.txt danach (1136,-2813) pm=0. => Aufnahme nach der Szene unberuehrt.
## 9. Urteile je Nutzer-Punkt (gegen den WORTLAUT)
| # | Wortlaut | Urteil | Beleg (gemessen) |
|---|---|---|---|
| 1 | "In Room 10F1 möchte ich, das wenn Elza ihn betritt eine Cutscene läuft wo sie Rechts zum Kommunikationssystem geht und von der Animation her drauf drückt und sagt "Hello, anyone, can you hear me? We Need help!"" | **erfuellt** | w1/k2/k3/z1: echter Tuerweg (Elza laeuft in 10D1 selbst in die Kartenleser-Tuer, VIERECK) -> Szene startet beim ersten Betreten (Balken + Pad-Sperre 476 Bilder); Rechtsdrehung 2048 -> 2691 und Gang nach +Z (Original: RECHTS = Gierung plus @0x8003304c/@0x8003308c; Bild k1 F5..F21); Ziel = die einzige "communications device"-Zone (Slot 2 @0x00F72), Druckpunkt (6862,6368) Blick Osten, Pruefpunkt in der Zone; Bild F130..F240: Arm auf dem Bedienfeld gehalten, Untertitel woertlich; Stimme: Zeile 40 fordert main40.wav an, Stellvertreter spielt ab Bild 148 (r 0,971) — die echte Aufnahme liefert der Nutzer |
| 2 | "Dann kommt "..."" | **erfuellt** | msg 41 "Elza: ..." F277..F375 direkt nach dem Senken (F247), Hand an der Huefte + Umsehen (Clip 9 + Plc_neck 4 = ROOM1031 @0x02A32..@0x02A44); Bild F280..F370 |
| 3 | "Dann "Uhh, this Thing is broken!"." | **erfuellt** | msg 42 "Elza: Uhh, this Thing is broken!" F382..F508 mit Hand an der Stirn (Clip 12 = @0x02A00); Bild F390..F420; Stellvertreter main42.wav ab Bild 383 (r 0,874); danach frei (F496 pm=0) |
| 4 | "Suche die dir passenden Animationen dafür heraus." | **erfuellt** | Gesten aus Elzas eigener Original-Bibliothek ROOM1031 rec 0, bytegleich mit ihrer ausgelieferten Szene sub13 (selbst disassembliert): Druck = ihr Druck auf den Rolltor-Notschalter (Slot 7 @0x01D88), "keine Antwort" = ihre Form nach "Hey, is anyone here?!", Stirn = ihre "Uff"-Geste; ANIM_TRACE 11 (gehalten, EINMAL gehoben) -> 0 rueckwaerts -> 9 -> 12; Bilder passen zur Handlung |

## 10. Gates
- Bau: configure + build `LOCAL-BUILD-OK`, keine Compiler-Warnung (nur CMake-Hinweise aus FetchContent/SDL2).
- RE-Gate: haelt (§4) — 9 zitierte Stellen selbst gelesen/disassembliert, alle stimmen; keine Rate-Woerter, kein Env-Schalter;
  PORT-WAHL/NUTZER-VORGABE gekennzeichnet und mit belegter Form.
- Vertrag: haelt (§5) — nur Bit 70, Ids 40..42, Ereignis 27 frei, kleine Haken, keine Tabu-Pfade.
- Tests: je Punkt messende Riegel vorhanden (unit form/texte/szene/einmal, integration echt/zweit/leon) und gruen.
- Nebenwirkungen: Leon 10F0 (integration_r35_cut10f0, _leon, unit einmal) unberuehrt; Elza nach der Szene: Tritt/Klettern/
  ROOM1091 (k2), Prison Key (k3), Tuer hinaus + 13 weitere Eintritte ohne Szene (z1); alle 10F1-Riegel anderer Spuren gruen.

## 11. Maengel
Keine.

## 12. Hinweise (nicht blockierend)
1. **Sprachdateien (Nutzer)**: synchro/STAGE1/room10F1/main40.wav ("Hello, anyone, can you hear me? We Need help!"),
   main42.wav ("Uhh, this Thing is broken!"), optional main41.wav ("..."). Bis dahin laufen die Zeilen stumm mit Untertitel
   (gemessen: mit Datei spielt sie bildgenau mit der Zeile). Zeile 40 steht 99 Bilder; die naechste Zeile kommt 130 Bilder
   nach ihrem Beginn — eine Aufnahme laenger als ~4,3 s wuerde von Zeile 41 abgeloest; bei der Abgabe der Aufnahme pruefen.
2. **Schreibweise** woertlich wie im Auftrag ("We Need help!", "this Thing") — Praezedenz Runde 35 ("another Survivor!"),
   Rueckfrage des Bau-Agenten steht im Dossier §8; Aenderung = eine Zeile in tools/r39_funk10f1/szene_bauen.py.
3. **Lesart "beim Betreten"** = nur beim ERSTEN Betreten (Story-Szene, Bit (9,70)); "Rechts" = aus Elzas Sicht (im Bild Cut 0
   geht sie auf die Kamera / nach links vorn zu, weil die Konsole dort steht — rechts im Bild stehen nur Schraenke "Nothing
   suspicious"). Beides ist im Dossier begruendet.
4. **Abdeckung**: Die drei angepassten Fremd-Riegel ueberspringen die Szene jetzt per (9,70). "Elza NACH der Funkszene -> Tritt,
   Klettern, ROOM1091" und "Prison Key nach der Szene" misst kein Riegel; beides habe ich gemessen (k2, k3: funktioniert).
   Empfehlung fuer die Integration: einen Lauf nach dem Muster klettern C (Szene, dann Weg) fuer Elza ergaenzen.
5. **unit_r38_itemorte_raum** rot durch Spur H (08f3094d, feuerloescher_r37.c:185 Band ueber Boden statt 1), nicht durch Spur I —
   bei der Integration Riegel oder Band-Regel abgleichen.
