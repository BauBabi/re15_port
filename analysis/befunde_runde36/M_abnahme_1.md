# Runde 36 — Spur M "karte": UNABHAENGIGE ABNAHME 1

Pruefer: Abnahme-Agent 1 (2026-10-11). Baum `.claude/worktrees/r36_karte`, Zweig `r36/karte`,
HEAD 2c5433a3 (Nachbesserung 1 nach Abnahme 0), merge-base mit master 78a4a748 (master steht auf
565da62b; `git diff master` zeigt deshalb fremde synchro/-Loeschungen und AUFTRAG.md — der Zweig selbst
wird mit `git diff master...HEAD` gelesen). Massstab: Wortlaut AUFTRAG.md Z. 61-66.
Gemessen wird am selbst gebauten Stand; nichts aus Dossier/Abnahme 0 wird ungeprueft uebernommen.
Arbeitsordner (Scratchpad): `M36_abn1/` (eigene Laeufe `laeufe/<lauf>/`, eigene Auswertung `bild.py`,
eigenes Lauf-Skript `lauf.sh` mit exe-Kopie `re15_pc_m36abn1_<lauf>.exe`).

## Stand der Pruefung (fortlaufend)

- [x] Lektuere AUFTRAG/VERTRAG/Dossier (inkl. Nachbesserung 1)/Abnahme 0/Code-Diff
      (Engine-Diff gegen die merge-base komplett gelesen; Nachbesserung `git diff 5064c3e8 HEAD`)
- [x] Bau (local_build.sh configure + build)
- [x] Gezielte Tests (76/76)
- [x] Punkt 1..6 an der exe gemessen (20 eigene Laeufe P1..P6j, Ordner `M36_abn1/laeufe/`)
- [x] RE-Gate (Stichprobe selbst disassembliert)
- [x] Vertrag / Pfade / Leitwoerter / Nebenwirkungen
- [x] Urteil: BESTANDEN (alle sechs Punkte erfuellt, Gates halten; Hinweise H1..H7)

## Bau

`bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
`bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
Arbeitsbaum = HEAD (git status leer bis auf diesen Bericht). Zeitstempel: re15_map_zones.c 05:15:10,
karte_fahrstuhl_1080.c 05:10:14, re15_karte_fahrstuhl.h 05:13:29 -> re15_pc.exe 05:15:15,
probe_r36_karte.exe 05:27:33 (test_r36_karte.c 05:21:30). Dass die exe die Nachbesserung wirklich
enthaelt, belegen zusaetzlich die Laeufe P6d..P6i unten (Verhalten = neuer Stand, Abnahme 0 mass am
alten Stand 2F/3F besucht).

## Gezielte Tests (keine volle Suite, VERTRAG)

```
ctest --test-dir re15_port/build -R "map|karte|fahrstuhl|elevator|1190|11a0|11d0|1110|10a0|weste|
      teilbereich|save|speicher|memcard|continue|hint|hinweis|zone|inv_screen|marke|mark|debug_menu|
      ebene|blatt|1080|4020|etage|visited|besucht|raeume_ziel|inv_fsm" --timeout 600 --output-on-failure
100% tests passed, 0 tests failed out of 76      Total Test time (real) = 571.96 sec
```
Darin die sieben `unit_r36_karte_*` (treppe, marker11d0, r1190, tuer11c0, teile1110, fahrstuhl,
kabine), `integration_r36_karte` (210,54 s, Laeufe A..I), `integration_r35_karte` (76,26 s) /
`_fahrstuhl` (179,96 s), `unit_map_speichern_laden` (angepasster Pin), `unit_map_teilbereich`,
`unit_r30_karte_nutzerstand`, `unit_debug_menu`, `unit_fahrstuhl_1080/4020_etagen`,
`probe_elevator_1080`, `unit_r30_hinweis_*` (5), `unit_r33_karte_*`, `integration_r33_speichern`,
`integration_weste_load_pin`, `integration_save_counter_pin`, `integration_map_raum_live` /
`_uebergang`, `unit_r35_raeume_ziel`. Kein Fenster-Haken musste nachgefahren werden (parallel liefen
fremde exe-Kopien re15_pc_nb6a / re15_pc_abn0n_kartenitems - trotzdem alles gruen).
Log: Scratchpad `M36_abn1/ctest1.log`.

## RE-Gate (selbst disassembliert; Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`
auf `info/Re1.5/PSX.EXE`, RDT-Bytes mit eigenem Python aus `re15_port/shared_assets/PSX/`)

Stichprobe der NEUEN Zitate der Nachbesserung (die 11 Zitate der ersten Runde hat Abnahme 0 geprueft):

| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| Kartenschirm waehlt das Blatt ueber Stage-Tabelle @0x80074c0c | `80049968 lui v0,0x800b; 8004996c lh v0,4064(v0)` (=0x800b0fe0 Stage), `8004997c sll v0,v0,2`, `80049984 addiu at,at,0x4c0c`, `8004998c lw v0,0(at)`, `80049994 jalr v0`; Tabelle [0] 0x8004b568 [1] 0x8004b8a0 [2] 0x8004b9d4 [3] 0x8004bc9c [4] 0x8004bdd4 [5] 0x8004bf70 | stimmt |
| Stage 0: `@0x8001103c[8] = 0x8004b63c` -> `ori v0,zero,0x2` @0x8004b684 (Delay-Slot von `j 0x8004b888`) -> `sb v0,0x260e(at)` @0x8004b88c | 0x8004b568: `lh v1,0x0fe2` (Raumindex), `sltiu v0,v1,0x26`, Tabelle 0x8001103c; [8] = 0x8004b63c; 0x8004b63c..0x8004b674 = Faelle 8,9,10 je nur `lbu v0,0x0fe2` + `sb v0,0x260d`; 0x8004b678 `lbu v1,0x0fe2`, 0x8004b680 `j 0x8004b888`, 0x8004b684 `ori v0,zero,0x2`; 0x8004b88c `sb v0,9742(at)` (=0x800b260e), 0x8004b894 `sb v1,0x260d`, `jr ra` | stimmt (ROOM1080 = Raumindex 8 -> Blatt 2; kein Lesen von Bank 3 / 0x800b0fe6) |
| Stage 3: `@0x80011184[2] = 0x8004bcf8` -> `ori v1,zero,0x8` @0x8004bdb4 -> `sb v1,0x260e(at)` @0x8004bdbc | 0x8004bc9c: `lh v1,0x0fe2`, `sltiu v0,v1,0xc`, Tabelle 0x80011184; [2] = 0x8004bcf8; Fall-Kette 2..11 (je `sb v0,0x260d`) -> 0x8004bdb4 `ori v1,zero,0x8`, 0x8004bdbc `sb v1,9742(at)`, `addiu v0,v0,65`, `sb v0,0x260d`, `jr ra` | stimmt (alle 12 Raeume der Stage -> Blatt 8) |
| ROOM1080 sub10 "Tuer auf" nur unter Ck(3,54/55/56,1) @0x08AC/0x08BC/0x08CC | ROOM1080.RDT @0x08AC `21 03 36 01 22 03 36 00 47 00 08 00`, @0x08BC `21 03 37 01 ...47 01..`, @0x08CC `21 03 38 01 ...47 02..`; einzige Treffer von Ck(3,54..56,1) in der Datei | stimmt |

## Messung an der exe (je Punkt, eigener Lauf)

Werkzeug: Spielstand Slot 0 mit `probe_r36_karte karte` (nur Raum/Position/Blick, Flags und
Startbesuche - Tueren/Fahrten danach ECHT in der exe), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_WINDOW_SCALE=1 RE15_SOFTWARE_RENDER=1 RE15_NO_INTRO=1 RE15_NOAUDIO=1`, Karte per
`RE15_INV_OPEN_AT=<n>#<raum>`, Abzug `RE15_INV_FB_SHOT`; Auswertung mit eigenem `bild.py`
(Farbhistogramm je Kasten, Pixelsuche, Vergroesserung). Farbklassen: aktuell (48,8,48)/(48,8,64),
besucht (16,56,40)/(16,56,56), Wand (176,176,176), Tuerbalken (224,168,40), Sprosse (240,240,216),
Grund (0,16,88)/(0,16,120). Blattwechsel zusaetzlich unabhaengig vom Bild ueber die Log-Zeile
`[inv] Blatt N: ...` (wird je GELADENEM Blatt einmal geschrieben).

### Punkt 1 — Treppen 10A0 auf B1
`P1_10A0_B1`: Stand ROOM1230 (-358,-6536) Blick -z, nur 1230 besucht; Aktionstaste -> Log `[tuer]
Sequenz ... DOOR07` -> `[room] PC loaded room10a0.rdt` -> Spieler (21200,-7200,25500) -> Karte
`[inv] Blatt 0` = "POLICE STATION B1". Im 10A0-Absatz (153,114)-(168,134) (aktuell 206+17 px):
30 Sprossen-Pixel (240,240,216), Bbox (156,123)-(165,130) = zwei 3-Sprossen-Glyphen
(x161..165 y123/125/127 und x156..160 y126/128/130). Bilder `laeufe/P1_10A0_B1/voll_x2.png`,
`crop_x10.png`. -> **erfuellt**.

### Punkt 2 — Marker ROOM11D0 an der Tuer
`P2_11D0_ankunft`: Stand ROOM1230 (-4729,-16018) + Flag (4,243); Aktionstaste -> `DOOR1A` ->
`room11d0.rdt`, Spieler (-300,-17400). Tuerbalken (127..131,124) = Oberkante des 11D0-Kastens,
Spieler-Ring (126..130,127..131), Mitte (128,129) = 5 px unter der Tuer (vorher laut Dossier
(129,151) = Mitte). Kasten (96,124)-(149,172) aktuell 2141+290 px. Bild `crop_x6.png`.
Gegenprobe Bewegung `P2b_11D0_gehen` (+ 2 s vorwaerts nach Sueden, Log bis (-300,-21900)): Ring
(126..130,161..165) - wandert nach unten bis vor die Suedwand (y172). -> **erfuellt**.
Nebenwirkung selbst nachgemessen `P2c_11D0_zwinger` (Stand im Zwinger-Gang NOERDLICH der Tuerwand
(-6700,-13000)): Ring (107..111,125..129) klemmt an der Oberkante - Hinweis H1 der Abnahme 0 besteht
unveraendert (Kachel MAP01.PIX uv(128,56) hat fuer diesen Gang keinen Platz; im Dossier unter OFFEN).

### Punkt 3 — "ROOM 1090" = ROOM1190, zwei Bereiche
Raumzuordnung selbst nachgelesen: ROOM1090.RDT Sce_em_set @0x02214..0x0228C nur Typ 0x26 (Feuer,
7 Saetze) + @0x022CC Typ 0x42, KEIN `22 03 75 01`; ROOM1190.RDT @0x02900/0x02914/0x02928 Typ 0x20
(Hunde), `22 03 75 01` (Weste) @0x02DD0. Die Deutung "ROOM1190" ist belegt; die Rueckfrage an den
Nutzer steht im Dossier.
* `P3a_1190_halle` (1230 Tuer -> `DOOR13` -> Spieler (3174,-11182)): rect 1 (77,77)-(142,113)
  aktuell 2158+271 px, Ring (136..140,78..82) direkt unter ihrer Tuer; rect 2 (145,70)-(158,103)
  NUR Grund (416+60).
* `P3b_1190_kammer` (1230 Ostkammer-Tuer, Hunde-Kill-Flags, `DOOR19` -> (19515,-12109)): rect 2
  aktuell 406+57 px, Ring (152..156,70..74) an der Nische; rect 1 NUR Grund (2170+272).
* `P3c_1190_beide` (Halle vorher besucht, dann Kammer per Tuer): rect 1 nur besucht-gruen
  2170+272 px, rect 2 nur aktuell 406+57 px - zwei Kaesten mit je eigener Wand, sauber getrennt.
Bilder `laeufe/P3*/crop_x4.png`. -> **erfuellt**.

### Punkt 4 — Tuer 11C0 um 90 Grad
`P4_11C0_laden` (Stand ROOM11C0 (-25279,17268) + Flag (4,64), 11B0 besucht; Karte Blatt 0):
gelbe Tuerpixel im Umkreis (160..190,85..115) = genau 5, (171..175,100) = waagerecht auf der
gemeinsamen Wandzeile y100; (171..175,90..99) nur besucht-gruen (11B0), (171..175,101..112) nur
aktuell/Ring (11C0) - kein senkrechter Balken mehr. ROOM11C0.RDT hat genau EINEN Tuer-Datensatz
(@0x01712 `3b 00 02 31 ...` Trigger (-27100,15900) 4000x2700, Ziel Raum 0x1B = ROOM11B0) - es ist
"die" Tuer von 11C0 (eigene Rohsuche nach 0x3B-Saetzen mit sce 2 im SCD-Bereich: nur dieser).
Bild `crop_x5.png`. (Den Weg durch die Tuer 11B0->11C0 habe ich nicht gefahren - laut Abnahme 0
blockiert ihn die Leon/Ada/Marvin-Szene beim Laden von 11B0; die Marke ist Kartendaten und haengt
nicht am Weg, der Abzug zeigt sie im echten Kartenschirm.) -> **erfuellt**.

### Punkt 5 — Teilbereiche ROOM1110
* `P5a_1110_ankunft` (1100 + Tuer-Flags (3,103)/(4,232) + Zombie-Kill-Flags; Aktionstaste ->
  `DOOR19` -> (-2400,-5000)): Evidence Room (147,115)-(186,143) aktuell 1010+134 px; kleine Raeume
  (190,123)-(211,132) und (190,135)-(211,144) nur Grund (203+17 / 182+25, dazu 13 px Gangwand).
* `P5b_1110_selbsttuer` (Stand Evidence Room (-2500,-650), Aktionstaste an der Selbst-Tuer ->
  `DOOR19` -> (-4550,-650), Cut 7): oberer kleiner Raum aktuell 196+16 px, Evidence Room besucht
  1016+136 px, unterer kleiner Raum NUR Grund (182+25).
Bilder `laeufe/P5*/crop_x5.png`. -> **erfuellt**.

### Punkt 6 — Fahrstuhl-Etagen (Schwerpunkt der Nachbesserung)

**Echte Fahrt** (Stand ROOM1040 an der Fahrstuhltuer (-21786,-10500) Blick +z, Flag (4,243), vorher
10C0 zu Fuss besucht; `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
RE15_INPUT_SCRIPT=W1,A0.3,W4,U0.73,W0.5,A0.3[,W1,D0.1[,W0.5,D0.1]]`, `RE15_FIRE_AOT=3@120#1080`
= Panel-AOT der Kabine). Log P6a: `[tuer] Sequenz ... DOOR25` -> `[room] PC loaded room1080.rdt` ->
`[fire-aot] slot=3 at F120 (Raum 1080)` -> HOCH F219 -> VIERECK F256 -> `[room] PC loaded
room1120.rdt` (Skript laeuft je Raum neu an, `[input-script] Tick 30 -> F90` in jedem Raum).
* `P6a_fahrt_3F` Karte in 1120, Blatt 4 (3F): 3F-Kabine (127,137)-(136,146) besucht 52+8 px,
  1120 aktuell (675 px). -> Ankunft zaehlt.
* `P6b_fahrt_blatt2F` (+RUNTER, Log `[inv] Blatt 3`): 2F-Kabine (109,134)-(118,143) = nur Grund 63+9
  px, 10C0-Wand 23 und dessen Tuerbalken 5; Innenraum (110..117,135..142) nur Grund (55+9). -> 2F
  NICHT besucht.
* `P6c_fahrt_blatt1F` (+2x RUNTER, `[inv] Blatt 2`): 1F-Kabine besucht 55+9 px. -> Einstieg zaehlt.

**Debug-Menue-Sprung (Mangel M1 der Abnahme 0)** — Stand ROOM1100 (-26130,-11036), nur 1100 besucht,
keine Bank-3-Bits, `RE15_DEBUG_JUMP=1080@gp` (Log: `[debug-menu] JUMP -> 108 ELEVATOR (ROOM1080)
spawn=(-13650,0,-900) cut=0`), Karte nach 40 Bildern in der Kabine:
* `P6d_debug_1F`: Blatt "POLICE STATION 1F", Kasten (109,134)-(118,143) aktuell 48+6 px (Bbox des
  Aktuell-Rots (110,135)-(117,142) = genau die Kabine), KEIN besucht-Pixel auf dem ganzen Blatt.
* `P6e_debug_1xHOCH` (`RE15_PAD_AT=100:U`, Log `[inv] Blatt 3`): "POLICE STATION 2F", nur der
  1100-Gang besucht (559 px, (187,115)-(220,154)); 2F-Kabinenkasten nur Grund (85+15). Vorher
  (Abnahme 0): 55+9 besucht.
* `P6f_debug_2xHOCH` (`100:U,108:U`): Log zeigt NUR `Blatt 2` und `Blatt 3` - Blatt 4 wird nie
  geladen; Bild = P6e (0 Pixel anders, eigener Vergleich); 3F-Kasten (127,137)-(136,146) nur Grund.
  -> 3F nicht anwaehlbar. Vorher: 3F-Blatt mit der Kabine als einzigem Kasten.
* Zweiter Fahrstuhl `P6g/h/i_debug4020` (Stand ROOM4010 (-22574,-21278), `RE15_DEBUG_JUMP=4020@gp`,
  Log `JUMP -> 402 A-2 ELEVATOR (ROOM4020) spawn=(-21400,0,-23400)`): Blatt 8 mit der Kabine aktuell
  ((128,69)-(137,78), 86 px) und 4010 besucht; HOCH bzw. RUNTER: Log nur `Blatt 8`, Bild 0 Pixel
  anders -> Blatt 9/10 nicht anwaehlbar.

-> Punkt 6 **erfuellt** (echte Fahrt: nur Einstiegs- und Ankunftsetage; Debug-Sprung: Kabine nur auf
dem Blatt des Original-Seiten-Setzers, andere Etagen weder besucht noch anwaehlbar; beide Fahrstuehle).

## Nebenwirkungen (selbst gemessen bzw. am Code/an den Bytes nachgelesen)

* **Debug-Sprung AUS der Kabine in einen Etagenraum** (`P6j_kabine_sprung_1120`): Stand in der Kabine
  ROOM1080 (-13650,-900) (= Zustand nach "108 ELEVATOR"; Laden loescht den Vorraum, Etage unbekannt),
  dann `RE15_DEBUG_JUMP=1120@gp` (`JUMP -> 112 3F WEST SIDE (ROOM1120) spawn=(150,0,9550) cut=0`),
  Karte in 1120: 3F-Kabine (127,137)-(136,146) **besucht 56+8 px**, obwohl keine Fahrt stattfand.
  Ursache am Code: `re15_karte_fahrstuhl_ankunft` erkennt die Ankunft an der Raumfolge Kabine ->
  Etagenraum und kann Debug-Sprung und Skript-Tuer der Fahrt nicht unterscheiden. Einordnung: nur mit
  zwei Debug-Spruengen hintereinander erreichbar, im normalen Spiel nicht; das ORIGINAL setzt die Kabine
  beim Betreten des Etagenraums ebenso ohne Fahrt auf diese Etage (ROOM1120.RDT @0x0D56: `21 04 f3 01`
  Ck(4,243,1) -> `21 03 38 00` Ck(3,56,0) -> `23 00 0a 00 00 00` -> `22 03 38 01` Set(3,56,1) @0x0D6C;
  ROOM1040 @0x15C0..: dasselbe mit `23 00 0a 00 05 00` und Set(3,54,1) @0x15D6) - kein Mangel,
  Hinweis H5.
* **ROOM11A0 zeigt jetzt Blatt 6 ("DRAINS B2")**, beide Baender (Folge von Punkt 3: rect 1 von B1 ist die
  1190-Halle). Das ORIGINAL oeffnet die Karte in 11A0 auf Blatt 0 (B1): Seiten-Setzer Stage 0
  Raumindex 26 -> @0x8001103c[26] = 0x8004b794 -> Fall-Kette 26..29 -> `sb zero,0x260e(at)` @0x8004b7dc
  (selbst disassembliert; gilt fuer 1190/11A0/11B0/11C0/11D0). Die Wahl ist im Code begruendet
  (Zeile @0x80076980 projiziert 11A0 UEBER alle B1-Rechtecke, keine Tuer zu einem B1-Raum, Kachel
  Blatt 6 rect 2), Band 0 lag schon vor dem Zweig auf Blatt 6; der Seiten-Setzer @0x8004b7dc wird aber
  nicht genannt und die Wahl nicht als PORT-WAHL markiert. Kein Nutzerbefund, kein Mangel - Hinweis H6.
* **Nebenbefund des Bau-Agenten** (Treppenhaus-Gast-Zeilen machen fremde Blaetter anwaehlbar) selbst
  gesehen: `probe_r36_karte messung` -> `1060 Band 0: gezeigt Blatt 2 | Blatt 3 (2F) bekannt 1,
  1060-Zeichnung 3/1 unbesucht | Blatt 4 (3F) bekannt 1, 1060-Zeichnung 4/1 unbesucht`. Bestand vor dem
  Zweig, kein Nutzerbefund, als Frage im Dossier - Hinweis H7.
* **Elza-Varianten**: jede geaenderte Zonen-/Etagenzeile hat ihre Zwillingszeile mit identischen Werten
  (0x1191, 0x11A1, 0x11D1); Marken haengen an der zid, Teil-/Kabinen-Bits und Fahrstuhl-Etage maskieren
  die Variante (`room & ~1u`). Kein exe-Lauf als Elza (das Spielstand-Werkzeug schreibt nur Leon).
* **Andere Spuren**: kein anderer r36-Zweig aendert re15_map_zones.c/.h, karte_fahrstuhl_1080.c,
  re15_karte_fahrstuhl.h, karte_r36.c oder die drei angepassten Pins (`git diff <merge-base> r36/*`).
  Spur N (r36/kartenitems) aendert map_hint_common.c / re15_map_owned.c / scd_room_setup.c / scd_vm.c /
  main.c - keine gemeinsame Datei mit M.

## Vertrag und Pfade

* Dateien des Zweigs (`git diff master...HEAD --name-only`): Dossier/Abnahmen/Bilder/Werkzeuge unter
  `analysis/befunde_runde36/M_*`; Engine `karte_fahrstuhl_1080.c`, NEU `karte_r36.c`,
  `re15_map_zones.c/.h`, Header `re15_karte_fahrstuhl.h`, NEU `re15_karte_r36.h`; Tests NEU
  `test_r36_karte.c`, `tests/unit/probes/r36_karte.cmake`, `tests/integration/test_r36_karte.cmake`,
  angepasst `test_map_speichern_laden.c`, `test_map_teilbereich.c`, `test_r30_karte_nutzerstand.c`.
  KEIN release/, platform/android/, shared_assets/PSX/, tests/*/CMakeLists.txt, scd_room_setup.c,
  main.c, RELEASE_NOTES.md. -> haelt.
* re15_map_zones.h steht in VERTRAG §1.4 unter den gemeinsamen Dateien; die Aenderungen dort sind
  Datenzeilen (Zonen/Marken/Etagen) mit Kommentar "Runde 36 Spur M" - genau das Zonen-/Tuermarken-
  Eigentum von M (§1.4 "M nur Zonen/Tuermarken/Besucht-Latches"), keine andere Spur fasst die Datei an.
  -> haelt.
* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine (`re15_game_flag_set|re15_msg_install|
  scd_event_fire` in den hinzugefuegten Engine-/Header-Zeilen: kein Treffer). Die Besucht-Bits 96..117
  liegen im kartenseitigen Feld visited_floor (v9), nicht in g_game.flags. -> haelt.
* Leitwoerter in den 1920 hinzugefuegten Zeilen unter re15_port/ (`deferred|tunable|interim|for now|
  faithful|plausib|TODO|FIXME|getenv|XXX|HACK|geraten|Naeherung|...`): nur zwei Verneinungen ("nichts
  ist geraten", "nichts wird geraten"). -> haelt.
* Konstanten der Nachbesserung: `return 2` / `return 8` mit @0x8004b684/@0x8004b88c bzw.
  @0x8004bdb4/@0x8004bdbc (selbst geprueft, s. RE-Gate), im Code UND in den Commit-Messages 8bc13ebb /
  2c5433a3. Pin-Werte (D2 >= 24, e1080 == 0, Summe 1) sind gemessene Folgen der Regel und im Pin
  begruendet. Etage je Kabine als NUTZER-VORGABE gegen RE2 gekennzeichnet. -> haelt.
* Erklaert der Fix den Befund? Abnahme 0 mass am alten Stand (Debug-Sprung) 2F-Kabine 55+9 und
  3F-Kabine 56+8 besucht + Blatt 3F anwaehlbar; Ursache am Code: `re15_karte_fahrstuhl_blatt` -1 ->
  "gleiche Baender"-Schleife setzt 112..114, und `re15_map_page_known` zaehlt die Gast-Zeilen der Kabine
  ueber das raumweite Zonen-Bit. Beide Pfade sind geschlossen (Kabinen-Zeilen aus der Schleife,
  page_known ueberspringt Kabinen-Zonenzeilen, Etage nie mehr -1); die Laeufe P6d..P6i zeigen genau das
  Gegenbild. -> ja.
* Riegel je Punkt vorhanden und messend; selbst ausgefuehrt `probe_r36_karte kabine|fahrstuhl|
  teile1110`: alle PASS-Zeilen mit Messwerten (z.B. `Kabinen-Etagen-Bits nach dem Sprung: 1F(112)=1
  2F(113)=0 3F(114)=0`, `3F-Blatt (4) ... bekannt 0`, `v8-Hebung: ... gesetzt 0 (vorher alle 6)`).
* Angepasster fremder Pin `test_map_speichern_laden.c`: v8 "Etagen-Bit := Zonen-Bit, alle 3 Blaetter"
  (e1080 == 3) verlangte woertlich den gemeldeten Befund fuer gehobene v8-Staende; jetzt e1080 == 0,
  Summe 1; ROOM1060-Zeile unveraendert; D2-Schranke 25 -> 24 = gemessene Zahl (2 statt 6 Kabinen-Bits).
  Begruendet, der Pruefzweck (D2: Laden loescht die Etagen-Bits) bleibt. -> haelt.

## Urteil je Punkt

| # | Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|---|
| 1 | Treppen 10A0 auf B1 fehlen | erfuellt | P1: echte Tuer 1230 -> 10A0, Blatt B1: zwei 3-Sprossen-Glyphen (30 px) im 10A0-Absatz |
| 2 | 11D0: Marker in der Mitte statt am Tuereingang | erfuellt | P2: echte Tuer, Ring-Mitte (128,129) 5 px unter dem Tuerbalken (127..131,124); P2b wandert nach Sueden mit; P2c Zwinger-Gang klemmt (H1) |
| 3 | "1090": zwei Tueren / zwei getrennte Bereiche | erfuellt | P3a/b/c: Halle nur rect 1, Kammer nur rect 2, beide zusammen getrennt (gruen/rot); Raum = ROOM1190 an den RDT-Bytes belegt (H4 Rueckfrage) |
| 4 | Tuer 11C0 um 90 Grad verdreht | erfuellt | P4: genau 5 gelbe Pixel (171..175,100) waagerecht auf der Wandzeile, keine senkrechten; 11C0 hat genau eine Tuer |
| 5 | kleine Tuerbereiche 1110 vorab besucht | erfuellt | P5a: kleine Raeume nach echter Tuer ungezeichnet; P5b: Selbst-Tuer -> nur der betretene wird sichtbar |
| 6 | Fahrstuhl-Etagen vorab besucht | erfuellt | P6a-c echte Fahrt 1F->3F: 1F+3F besucht, 2F ungezeichnet; P6d-f Debug-Sprung 1080: nur 1F (aktuell), 2F leer, 3F nicht anwaehlbar (Log nur Blatt 2/3); P6g-i 4020: Blatt 9/10 nicht anwaehlbar; H5 nur bei Debug-Sprung aus der Kabine |

## Maengel

Keine.

## Hinweise (kein Mangel)

* **H1 (Punkt 2, 11D0-Zwinger-Gang)** unveraendert, selbst nachgemessen (P2c): Ring klemmt an der
  Oberkante (107..111,125..129). Die Kachel hat keinen Platz fuer den Gang; im Dossier unter OFFEN.
* **H2 (Testabdeckung Punkt 6)** weiterhin kein Riegel fuer die ECHTE Fahrt (Panel-AOT, Cursor,
  VIERECK, Skript-Tuer); `integration_r35_karte_fahrstuhl` faehrt nur den Einstieg. Uebernehmbar aus
  P6a-c: Stand 1040 an der Fahrstuhltuer + Flag (4,243) + 10C0 besucht, `RE15_INPUT_SCRIPT_BASIS=spiel
  RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT=W1,A0.3,W4,U0.73,W0.5,A0.3,W1,D0.1,W0.5,D0.1
  RE15_FIRE_AOT=3@120#1080 RE15_INV_OPEN_AT=40#1120` -> Log `[room] PC loaded room1120.rdt`, Abzug je
  Blatt (FB_SHOT_AT 60 / 290 / 310).
* **H3 (ROOM11A0-Marker in der Kerbe der L-Kachel)** - nicht Spur M, unveraendert.
* **H4 (Punkt 3)** Rueckfrage "ROOM 1090 = ROOM1190?" bleibt berechtigt (ROOM1090 ohne Hunde/Weste,
  selbst an den Bytes nachgesehen).
* **H5 (Punkt 6, nur Debug-Menue)** Sprung aus der Kabine direkt in einen Etagenraum zaehlt als Ankunft
  (P6j: 3F-Kabine besucht ohne Fahrt). Nachpruefen: `probe_r36_karte karte re15_card.mcr 1080 -13650 0
  -900 2048 besucht:1100:-26130:-11036`, exe-Kopie mit `RE15_DEBUG_JUMP=1120@gp
  RE15_INV_OPEN_AT=40#1120 RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60 RE15_EXIT_AT=200#1120`
  (+ Standard-Haken) -> Kasten (127,137)-(136,146) besucht. Das Original setzt die Kabine beim Betreten
  des Etagenraums unter Ck(4,243)/Ck(3,56,0)/`23 00 0a 00 00 00` ebenso ohne Fahrt auf 3F
  (ROOM1120 @0x0D6C). Falls der Nutzer das bemaengelt: Ankunft nur ueber die Skript-Tuer der Fahrt
  (ROOM1080 main00 @0x0482/@0x04A2/@0x04C2) zaehlen statt ueber die Raumfolge.
* **H6 (Nebenwirkung Punkt 3, ROOM11A0 auf Blatt 6)** Dossier/Kommentar sollten den Seiten-Setzer
  @0x8004b7dc (`sb zero,0x260e`, 11A0 -> Blatt 0 im Original) nennen und die Blatt-6-Wahl als PORT-WAHL
  markieren.
* **H7 (Nebenbefund des Bau-Agenten)** Treppenhaus-Gast-Zeilen machen leere Etagenblaetter anwaehlbar
  (ROOM1060 nur 1F -> Blatt 3/4 bekannt 1) - Entscheidung beim Nutzer/Orchestrator, wie im Dossier.

## Ergebnis

Alle sechs Punkte am gebauten Stand ueber den echten Weg gemessen und **erfuellt**; der Mangel M1 der
Abnahme 0 (Debug-Sprung "108 ELEVATOR": 2F/3F besucht, 3F anwaehlbar) ist behoben und erklaert. Gates
(Bau, gezielte Tests 76/76, RE-Gate-Stichprobe: 4 Zitate der Nachbesserung + 11A0-Seiten-Setzer selbst
disassembliert, Vertrag, Pfade, Leitwoerter, Konflikte mit anderen Spuren) halten. **BESTANDEN**, mit
den Hinweisen H1..H7.
