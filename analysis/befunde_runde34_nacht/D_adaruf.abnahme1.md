# Spur D — Abnahme 1 (unabhaengig, nach dem Bau)

Abnehmer: hat den Code nicht geschrieben; Auftrag: widerlegen, dass Spur D fertig ist.
Gepruefter Stand: Zweig `r34n/adaruf`, Commit `56e18333` (Code-Stand f35f2744, spaetere Commits nur Doku/Belege).
Belege dieser Abnahme: `D_belege/abnahme1_*` (Bilder angesehen), Laufprotokolle `D_belege/abnahme1_laeufe.txt`,
Byte-/Disasm-Nachpruefung `D_belege/abnahme1_bytes.txt`.

## 0. Urteil

**ABGENOMMEN — mit vier kosmetischen Maengeln, kein Blocker, kein wesentlicher Mangel.**

Jeder Nutzerpunkt aus AUFTRAG.md (Z. 14-19 + Nachtrag "Woman:") laeuft an der echten exe so, wie er
beschrieben ist; die Gegenproben (Eingaben waehrend der Szene, Folgedruecke, Laden von Karte, ECHTE Rettung
in ROOM1090 mit Rueckweg und Tuer, Nachbarverhalten, Kamera danach) haben keinen Fehler gezeigt. Die
tragenden Konstanten sind an den ausgelieferten Bytes/Instruktionen nachgeprueft (38/38 Vorbild-Bytes, 4
Handler/Tabellen). Widerlegen liess sich nur Randstaendiges (Abschnitt 2).

## 1. Gepruefter Stand, Suite

- `bash re15_port/tools/local_build.sh` im Baum (configure + build + test), selbst gefahren, andere Spuren
  bauten parallel: **`100% tests passed, 0 tests failed out of 436` / `=== LOCAL-BUILD-OK (all) — Tests 436/436`**
  (698 s). Die 8 Spur-D-Riegel `unit_r34n_d_adaruf_szene/_doppel/_sperre/_frei/_rettung/_speicher/_elza/_raster`
  gruen; `szene` und `raster` zusaetzlich einzeln gefahren und die Ausgaben gelesen (szene: 0 Fehler, Zeitlinie
  Ruf B21, Schritt B123..B133 Weg 700 dz 0, zur Kamera B145, Clip19 B165/rueckw. B190, Clip17 B216, Ende B316;
  raster: 735/735 Druckstellen, haengt 0, Weg 632..700).
- Baum sauber (`git status` nur diese Datei + Belege), keine fremden Pfade veraendert; `local_build.sh`-Aenderung
  im Zweig = Infrastruktur-Commit f2efa848 (identisch mit master 7d4d11dd), nicht Spur D.

## 2. Maengelliste

| # | Schwere | Titel | Beleg |
|---|---------|-------|-------|
| M1 | kosmetisch | Debug-Menue-Sprung als Elza landet in ROOM1050 (Leon-Datei) und spielt dort die Ada-Ruf-Szene mit "Leon:"-Zeilen | `abnahme1_elza_debugsprung_1050.png`, `abnahme1_laeufe.txt` Lauf t5 |
| M2 | kosmetisch (latent) | Rufstimme `main22.wav` > 100 Bilder ueberlappt den Rueckschritt; die Aufnahmegrenze 3,3 s ist knapp | Nutzer-Aufnahme ROOM1090 `main00.wav` 3,38 s = 101 Bilder; Dossier §9.9 |
| M3 | kosmetisch | Belegwerkzeug `tools/r34n_d/texte_bauen.py` baut msg 25 ZWEIzeilig, ausgeliefert ist EINzeilig | texte_bauen.py Z. 98 vs. adaruf_1050.c `k_msg25` |
| M4 | kosmetisch (Form) | Leons Satz als zwei Kaesten statt einer Zeile — weicht von Nutzerwortlaut und RE1.5-Form ab (als PORT-WAHL begruendet) | Dossier §1 L2b; ROOM1090 msg 5 @0x2861, sub03 @0x02640..@0x0266C |

**M1 — Elza ueber das Debug-Menue.** Lauf t5 (Elza-Start wie `integration_elza_vollstart`, dann
`RE15_DEBUG_JUMP=1050@gp`): `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)` ->
`JUMP -> 105 EAST CORRIDOR (ROOM1050)` -> `PC loaded room1050.rdt` -> `[adaruf] ... Ereignis-Platz` -> Quadrat ->
"Woman: Hello? ..." und "Leon: Another civilian survivor." (Bild). Ursache liegt NICHT in Spur D: der Debug-Sprung
des Ports ignoriert das Elza-Bit. Im Original laeuft auch der Debug-Zweig des Warps in den Raumlader:
FUN_8001d600 `bne v0,zero,0x8001d82c` @0x8001d618 (Debug = 0x800ac9a8 == 0) ... `j 0x8001d988` @0x8001d824 ->
`jal 0x800396fc` @0x8001d988; dort Dateiindex = Basis + Elza-Bit: `lw a0,-13764(a0)` (0x800aca3c) @0x800397b8,
`srl a0,a0,31` @0x800397e4, `addu a0,a0,v0` @0x800397ec (selbst disassembliert) -> ROOM1051. Im Spiel (Tueren)
traegt der Port die Variante (aot_common.c `dest_id | (g_current_room_id & 0xF)`), Elza kommt also nach
ROOM1051 und die Installation kehrt dort zurueck (Riegel `elza`). Wirkung nur im Debug-Pfad; Abhilfe: Debug-Sprung
variantentreu machen (eigenes Thema) oder die Installation zusaetzlich an Spielerfamilie PL00 binden.
Derselbe Debug-Pfad setzt Cut 0 (Log `cut=0`) — Leon dreht sich dann zur Kamera Cut 4, die nicht aktiv ist; im
Spiel ist an der Tuer immer Cut 4 aktiv (RVD selbst dekodiert, Abschnitt 4 letzter Punkt).

**M2 — Stimme gegen Rueckschritt.** Der Rueckschritt haengt an `Sleep 100` nach Message_on 22 (+0x20), nicht am
Stimmende (voice_wait parkt nur das naechste Message_on). Die Aufnahmeliste verlangt <= 3,3 s. Die vorhandene
Aufnahme derselben Sprecherin fuer den gleich langen Satz "Anyone! Can someone please / get me out of here!?"
(`synchro/STAGE1/room1090/main00.wav`) misst 3,38 s = 101 Bilder, `main02.wav` 4,38 s, `main04.wav` 4,67 s — die
Grenze wird mit der ueblichen Sprechweise also eher gerissen; dann beginnt der Schritt, waehrend "Woman" noch
spricht (Nutzerwortlaut: "nach Adas Dialog"). Ohne Datei (heute) korrekt. Empfehlung: Schritt auf das
Stimmende warten lassen (Gegenpruefung Auflage 9 liess das ausdruecklich zu) oder die Grenze in der Uebergabe an
den Nutzer hervorheben.

**M3 — Werkzeug-Drift.** `texte_bauen.py` Z. 98: `(25, "tuer", None, ["I have to help", "the Survivor first!"], ...)`
erzeugt `... 44 41 48 4c 08 50 44 41 ...` (Umbruch 0x08); `adaruf_1050.c k_msg25` und Dossier §5.5 haben
`... 44 41 48 4c 00 50 44 41 ...` (eine Zeile, so auch an der exe gesehen). Das Werkzeug belegt also nicht die
ausgelieferten Bytes; im Spiel ohne Wirkung.

**M4 — Form der Leon-Zeile.** Nutzer: "- Leon: Another civilian survivor. I have to help her!" (eine Zeile);
RE1.5 setzt zwei Saetze eines Sprechers mit zwei Gesten in EINEN Kasten (ROOM1090 msg 5, sub03 @0x02640..@0x0266C).
Gebaut sind zwei Kaesten, beide mit "Leon:". Im Dossier als PORT-WAHL mit Grund (Gesten am Satzteil, auch bei
spaeter aufgenommener Stimme) gekennzeichnet und von der Gegenpruefung angenommen — hier nur als sichtbare
Formabweichung vermerkt.

## 3. Nutzerpunkte aus AUFTRAG.md an der echten exe

Alle Laeufe: eigene Kopie `re15_pc_abn1d.exe` aus `re15_port/build/platform/pc`, `RE15_WINDOW_SCALE=3`,
`RE15_FRAMEDUMP` (komponiert vor dem Present), `RE15_STATE_LOG`; Weg in den Suedteil wie im Spiel ueber die Tuer
ROOM1000 Slot 0 (Tuersequenz P07G). Befehle und Logzeilen: `abnahme1_laeufe.txt`.

| Nutzerpunkt | Gemessen (Bild / Log) | Ergebnis |
|---|---|---|
| Szene, bevor Leon 1050 -> 10A0 wechseln kann | Lauf r1: Quadrat F150 -> `[adaruf] Ereignis 13: Szene startet`, kein `DOOR FIRE slot=4`, kein `room10a0` | erfuellt |
| Dialog "Ada"/Woman + Leon | `abnahme1_szene_ueberblick.png`: F172..F270 "Woman: Hello? Anyone? Please, / get me out of here!" (Sprecher rot = Farbe 02 wie ROOM1090 msg 0), F316 "Leon: Another civilian survivor.", F366 "Leon: I have to help her!" (Sprecher gruen) | erfuellt (Sprecher "Woman:" = Nutzer-Nachtrag) |
| Schritt zurueck von der Tuer, wie ROOM1090 nach dem Lauf zum Feuer | `abnahme1_rueckschritt_drehung.png` + State-Log: F271 Drehung (1 Bild, Blick schon zur Tuer), F273..F282 je 70 Einheiten rueckwaerts (16500 -> 15800, z fest), Beine im Schritt, Blick bleibt zur Tuer — gleicher Modus 8 wie ROOM1090 sub02 @0x0247C | erfuellt |
| "another civilian survivor" = rechter Arm 180 Grad, nach rechts, denselben Weg zurueck | `abnahme1_gesten_clip19_clip17.png` F316..F364: nach Drehung zur Kamera (F283..F294, Gierung 4095 -> 2973) geht der Arm rechts im Bild seitlich hinaus, Hand dreht auf, derselbe Weg zurueck (Clip 19 vor, ab F340 rueckwaerts) | erfuellt (Lesart L4 Zuschauersicht; Gestenzensus nachgesehen, Abschnitt 5) |
| "I have to help her" = Arm-Schwung | dasselbe Bild F368..F400: Arm quer vor die Brust, dann waagerecht ganz nach rechts hinaus (F384..F390), zurueck in die Ruhe (Clip 17) | erfuellt |
| Balken wie Original-Szenen | Balken ab F151 (Rampe 15 Bilder), weg F480; `[scd F480] letterbox closed -> gameplay` | erfuellt |
| Erneuter Druck -> "I have to help the Survivor first!" | Lauf t1 (`abnahme1_folgedruecke_sperrtext.png`): nach der Szene zurueck zur Tuer, Quadrat F520 -> msg 25, Freeze `pf=FFFF0007`, Schreibmaschinentext einzeilig; F560 (noch im Tippen) schliesst nicht, F600 schliesst (F601 msg aus), F640 -> wieder msg 25 — der Schliessdruck oeffnet nicht erneut, keine zweite Szene, kein Raumwechsel | erfuellt |
| Auch nach Laden | Lauf t6 (`abnahme1_karte_continue_sperre.png`, `kartenlauf.sh gesehen`): CONTINUE ROOM1150 -> 1000 -> 1050: `[adaruf] ... Text-Platz sce 1 / msg 25 (Sperre)`, Druck -> Sperrtext | erfuellt |
| Erst nach Ada-Rettung in ROOM1090 durch die Tuer | Lauf t4b, ECHTE Kette ohne gesetztes (3,187): ROOM1090 sub03 laeuft (Rettung, `letterbox closed` F725), Tuer Slot 0 -> ROOM1050 "Leon: Hey, wait!" (sub03, (3,110) wieder 0), zu Fuss auf Adas Weg zur Tuer, Quadrat F360 -> `DOOR FIRE slot=4`, `Sequenz ... P07G ... S021 T013`, `PC loaded room10a0.rdt`; KEINE `[adaruf]`-Zeile (Installation kehrt bei (3,0xBB)=1 zurueck) — `abnahme1_echte_rettung_1090_1050.png` F72..F354 | erfuellt |

## 4. Gegenproben

- **Eingaben waehrend der Szene** (Lauf t2, `abnahme1_eingaben_waehrend_szene.png`, State-Log in
  `abnahme1_laeufe.txt`): START (F199-201, F230, F300, F420), R1 halten (F259-279, mitten im Rueckschritt),
  Links+Vor (F310-339), Kreuz+Zurueck (F340-369), Dreieck (F370-399), Quadrat/Kreuz einzeln — kein Inventar,
  kein Zielen, keine Bewegung, Zeitlinie bildgenau wie ohne Eingaben (msg 22 F171, Schritt F273..F282, msg 23 +
  Clip 19 F315, msg 24 + Clip 17 F366, pm 2 -> 0 bei F480). Danach sofort steuerbar (F481 Vorwaerts bewegt), und
  die Kamera folgt wieder (Cut 4 -> 3 bei z -11280, RVD-Satz 4->3 z -11500..-10500 @0x02A0).
- **Gegen die Wand gerannt, Quadrat bei gehaltenem Rennen** (Lauf t7): Leon steht nach dem Rennen bei x 16732 an
  der Wand, Druck F150 bei gehaltenem Kreuz+Vor -> Szene startet (pm 2), Rennen/Vor waehrend der Szene ohne
  Wirkung, Rueckschritt 16732 -> 16032 (700), Drehung zur Kamera — gleiche Szene wie vom Standplatz.
- **Quadrat-Spam / zweiter Ausloeser**: Riegel `doppel` + Builder-Lauf 8c (19 Druecke) — nicht wiederholt.
- **Folgedruecke** im selben Raum und nach Laden: Abschnitt 3.
- **Echte Rettung statt RE15_SET_FLAG**: Abschnitt 3, t4b.
- **Raumvariante Elza**: Riegel `elza` (ROOM1051 Slot 4 bleibt Tuer -> ROOM10A1); an der exe nur ueber den
  Debug-Sprung erreichbar, der die Variante verliert -> M1.
- **Nachbarverhalten**: Tuer ROOM1000 Slot 0 -> 1050 unveraendert (jeder Lauf), ROOM1090 Slot 0 -> 1050 und die
  Original-Szene "Hey, wait!" (sub03 @0x00D88..) unveraendert (t4b), Tuersequenz Slot 4 nach der Rettung
  unveraendert P07G S021 (t4b), Rolltor-Schalter/-Szene nicht beruehrt (Slot 7, sub02 unveraendert im Code).
- **Kamera an der Tuer aus jeder Richtung Cut 4** — RVD ROOM1050 @0x001B0 selbst dekodiert: 3->4 z
  -12500..-11500 (@0x0278), 5->4 z -15700..-14700 (@0x02DC); alle begehbaren Druckstellen (z -15300..-12100)
  liegen damit in Cut 4. Der fest eingetragene Blickpunkt Cut 4 (RDT @0x000E4/@0x000EC) passt also im Spiel.

## 5. RE-Gate-Pruefung des Codes

- `re15_adaruf.h` / `adaruf_1050.c`: jede Konstante traegt Datei-Offset oder Adresse bzw. ist als NUTZER-VORGABE /
  PORT-WAHL mit Grund markiert (Ereignis 13, Blick zur Kamera, Rueckschritt-Richtung). Kein "interim",
  "plausibel", "tunable", kein Platzhalter (grep). Einzige nackte Zahl `d < 65535` = Wertebereich von
  `re15_msg_install_durations`, kein Verhalten.
- Vorbild-Bytes selbst gegen die ausgelieferten RDTs gelesen: **38 von 38 identisch** (`abnahme1_bytes.txt`:
  ROOM1050 @0x00B5A/@0x000E0/@0x00C22/@0x00D88/@0x00DC2/@0x00DCA, ROOM1090 @0x02414..@0x026E6 und msg 0/1,
  ROOM1170 @0x015F0/@0x015F4, ROOM1130 @0x00A1C, ROOM11B0 @0x01478). Kamera Cut 4 = (14999,-3549,-8140).
- Selbst disassembliert (`re15_disasm.py`, `info/Re1.5/PSX.EXE`): AOT-Typtabelle @0x8007469c ([1]=0x80043084,
  [3]=0x800430f0), Plc_dest-Tabelle @0x80073e30 ([6]=0x800517f0, [8]=0x800311f0, [9]=0x80031360), sce-3-Handler
  `lhu a0,0(v0)` @0x800430fc / `lbu a1,3(v0)` @0x80043100 / `jal 0x8003ee3c` @0x80043104, sce-1-Handler
  `lhu a3,2(v0)` @0x80043098 / `lhu a2,0(v0)` @0x8004309c / `jal 0x80027e68` @0x800430a0, Modus 8 `ori v0,zero,0x46`
  @0x80031210 / `addiu a2,zero,-48` @0x80031254 / `ori a0,zero,0x800` @0x8003125c / `slti v0,v0,100` @0x800312fc,
  Modus 9 `ori v0,zero,0x5` @0x8003139c / `ori a2,zero,0x60` @0x800313d4 und @0x80031440 — alles wie zitiert.
- Punkt-Glyphe: 0x57 ist in allen RDT-Nachrichten das Satzende (890x vor Ende-/Umbruch-Code, 0x3C nie) — die
  Wahl in msg 23 ist richtig.
- Gestenwahl gegen den Zensus (`D_belege/plc_motion_zensus.tsv`, nach Inhalts-Hash gruppiert): in Leon-Raeumen
  gibt es ausser den Bibliotheks-Clips 15..21/23 nur Inhalte, die der Bau gerendert und verworfen hat (1150
  Clip 10/11/12 kniend, 1170 Clip 25, 50B0 13/14, 2020 0, 3070 Clip 24), dazu 036945c77ae4 (11B0/4010/5021 Clip 11)
  = 1-Bild-Pose und 12f8c236fb59 (11C0 Clip 21 / 6030 Clip 6) = Bibliotheks-Clip 21 "Hand hoch". Keine uebersehene
  Arm-Geste; ROOM1050-RBJ hat genau einen Spieler-Record (rec0 Marker 1, rec1 Marker 2), die Bindung ist eindeutig.
  An der exe stimmen Clip 19 (Arm seitlich hinaus, Hand dreht auf, zurueck) und Clip 17 (Schwung) mit dem
  Nutzerwortlaut ueberein (Bild).
- Commit-Messages e6f46443/5e9d089a tragen die Konstanten mit @0x (Backticks lesbar, Co-Authored-By vorhanden).
- Vertrag: nur Bank-9-Bit 65 (66 frei), Slot 4 umgewidmet (erlaubt), Nachrichten 22..25, eigene Dateien + je
  eine Haken-Zeile in `scd_room_setup.c`/`scd_vm.c`; kein Port-Code liest/schreibt sonst (9,65) (grep Bank 9).
- PSX: kein Gesamtbau moeglich (bekannte Luecke), aber das Modul uebersetzt mit `mipsel-none-elf-gcc` 12.3
  (`-DRE15_PLATFORM_PSX`, exit 0, nur eine vorbestehende Kommentar-Warnung in re15_emd.h).

## 6. Softlock-/Raumgraph-Pruefung

Nachvollzogen und an der exe gefahren: die Rettungskette braucht nichts hinter ROOM10A0 — Feuerloescher ROOM1000
sub00 @0x00C24 `Item_aot_set` Slot 3 (Item 0x31, Nimm-Bit 0x86) UNBEDINGT platziert, sub01 @0x00D00/@0x00D08/@0x00D0C
setzt (3,133); ROOM1090 sub00 @0x0230A/@0x02332 -> sub06 -> Selbsttuer -> sub03 @0x024D2 (3,187). Die echte Kette
Rettung -> ROOM1050 -> Tuer frei ist in t4b gefahren. (3,0x6E) als Freigabe waere falsch (ROOM1050 sub03 @0x00D88
loescht es) — die Wahl (3,0xBB) ist richtig. Tuergraph mit Stage-Byte (Auflage 1) nicht erneut erhoben.

## 7. Sprachdateien / Nachrichten

Dossier §9.9 nennt `synchro/STAGE1/room1050/main22.wav` (Woman), `main23.wav`, `main24.wav` (Leon) mit Wortlaut;
msg 25 ist ein Text-Platz ohne Stimme (wie alle Untersuchungstexte, scd_vm.c "No voiceover"). Heute liegt nur
`main08.wav` im Ordner — nichts Fremdes auf 22..25. Zur Laengengrenze von main22 siehe M2.

## 8. Messprotokoll

`D_belege/abnahme1_laeufe.txt` (Befehle, Logzeilen, State-Log-Auszuege der Laeufe r1, t1, t2, t4b, t5, t6, t7),
`D_belege/abnahme1_bytes.txt` (Bytes, Punkt-Zensus, Disasm). Bilder: `abnahme1_szene_ueberblick.png`,
`abnahme1_rueckschritt_drehung.png`, `abnahme1_gesten_clip19_clip17.png`, `abnahme1_folgedruecke_sperrtext.png`,
`abnahme1_eingaben_waehrend_szene.png`, `abnahme1_echte_rettung_1090_1050.png` (Bilder F360..F420 dort stammen noch
aus ROOM1090 — waehrend der Tuersequenz schreibt der Framedump nicht), `abnahme1_elza_debugsprung_1050.png`,
`abnahme1_karte_continue_sperre.png`. Nebenbeobachtung ohne Spur-D-Bezug: im Debug-Sprung-Zustand ROOM1090 mit
(3,129)=1 feuert der Auto-Platz Slot 3 (main00 @0x0215A, Band 5) Ereignis 7 in jedem Bild, solange die VM an msg 9
(`2b 09 ff ff` @0x02502) steht (13 Faeden + 64x "event 7 DROPPED") — Zustand aus dem Sprung, im Spielablauf nicht
gemessen.
