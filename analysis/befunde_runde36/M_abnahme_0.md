# Runde 36 — Spur M "karte": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-11). Baum `.claude/worktrees/r36_karte`, Zweig `r36/karte`,
HEAD 98c2e109, Basis 78a4a748 (merge-base mit master; master steht inzwischen auf 565da62b).
Massstab: Wortlaut AUFTRAG.md Z. 61-66. Gemessen wird am selbst gebauten Stand, nichts aus dem
Dossier wird ungeprueft uebernommen.

## Stand der Pruefung (fortlaufend)

- [x] Lektuere AUFTRAG/VERTRAG/Dossier/Code-Diff (616 Zeilen Engine-Diff gelesen)
- [x] Bau (local_build.sh configure + build)
- [x] Gezielte Tests (66/66)
- [x] Punkt 1..6 an der exe gemessen (20 eigene Laeufe M1..M8, Ordner Scratchpad `M36_abn0/laeufe/`)
- [x] RE-Gate (11 Zitate selbst disassembliert/gelesen)
- [x] Vertrag / Pfade
- [x] Urteil: NICHT bestanden (M1, Punkt 6 teilweise)

## Bau

`bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
`bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
(der Stand im Baum war bereits gebaut; ninja vergleicht alle Quellen gegen die Ziele - die exe
entspricht HEAD 98c2e109, letzte Engine-Aenderung da5ae967).

## Gezielte Tests (keine volle Suite, VERTRAG)

```
ctest --test-dir re15_port/build -R "map|karte|fahrstuhl|elevator|1190|11a0|11d0|1110|10a0|weste|
      teilbereich|save|speicher|memcard|continue|hint|zone|inv_screen|marke|mark" --timeout 600 --output-on-failure
100% tests passed, 0 tests failed out of 66      Total Test time (real) = 525.17 sec
```
Darin die sechs `unit_r36_karte_*` (je 0,01-0,03 s), `integration_r36_karte` (152,94 s), alle
`unit_map_*`, `unit_r30/r33/r35_karte*`, `integration_r35_karte` (74 s) / `_fahrstuhl`,
`integration_map_raum_live` / `_uebergang`, `unit_1190_weste` / `_ruestung`, `unit_re2_room1190_ab`,
`unit_fahrstuhl_1080/4020_etagen`, `probe_elevator_1080`, Speichern/Laden-Pins,
`integration_r33_speichern`, `integration_weste_load_pin`, `integration_save_counter_pin`.
Kein Fenster-Haken musste nachgefahren werden. Log: Scratchpad `M36_abn0/ctest1.log`.

Angepasste fremde Pins (selbst gelesen): `test_map_teilbereich.c` verlangte im Evidence Room "die
anderen beiden sind GRUEN" = woertlich der gemeldete Fehler -> jetzt besucht nur, wo man schon war
(Reihenfolge 0,1,2); `test_r30_karte_nutzerstand.c` B4 begeht zusaetzlich die Mitte jedes Teils.
Beide Aenderungen sind begruendet und schwaechen nichts ab.

## RE-Gate (selbst disassembliert / selbst gelesen)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (RE1.5 `info/Re1.5/PSX.EXE`) und
`re2_disasm.py` (RE2 `info/re2leon/PSX.EXE`), RDT/PIX-Bytes mit eigenem Python aus
`re15_port/shared_assets/PSX/`.

| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| RE1.5 Massstabszeilen @0x80076978 (1190) / 980 (11A0) / 988 (11B0) / 990 (11C0) / 998 (11D0) | `4b 00 7f 00 80 07 fd 08` = (75,127,1920,2301); (138,113,1776,2048); (159,110,2272,1954); (174,196,1,1); (0,0,1,1) | stimmt (Index (Adr-0x800768b0)/8 = 25..29 = ROOM1190..11D0) |
| Seiten-Setzer @0x8004bdb4 / @0x8004bf50 / @0x8004bee4 | `ori v1,zero,0x8` / `ori v1,zero,0x9` / `ori v1,zero,0xa` (Delay-Slot von `j 0x8004bf54`) | stimmt |
| RE2 FUN_80049e48 `jal 0x8006931c` @0x8004a39c | `8004a39c: jal 0x8006931c` | stimmt |
| RE2 FUN_8006931c `sltiu v0,a0,0x7` @0x80069330, Basen 30/58/72/89/99/123, `addiu a0,s0,238` @0x80069390, `jal 0x8007730c` @0x80069394 | genau so; s0 = 0x800d481e -> a0 = 0x800d490c; 0x8007730c = Bit SETZEN (`or` + `sw`), 0x80077360 = Bit TESTEN (`and`) | stimmt |
| RE2 Zeichner `lbu a1,12(s1)` @0x8006e67c/@0x8006e730, Test @0x8006e688/@0x8006e73c, `beq v0,zero,0x8006e768` @0x8006e744, `bne s2,a3` @0x8006e640 -> `addiu s5,s5,1` @0x8006e648 | genau so (a0 = 0x800d4924-24 = 0x800d490c = Bank 9) | stimmt |
| ROOM10A0.RDT @0x00E7C / @0x00E90 (Treppen-AOT Band 4) | `2c 06 0d 31 04 00 ...` slot 6 sce 13 Band 4 (24000,20100,2700,4500); `2c 07 0d 31 04 00 ...` slot 7 sce 13 Band 4 (20100,19500,2700,2300) | stimmt |
| ROOM1190.RDT Hunde @0x02900, Luke @0x02A06, Weste-AOT @0x02D90, `22 03 75 01` @0x02DD0, Tueren @0x028B2 / @0x02B1C | 0x44 Typ 0x20 bei (690,-3600,-28500) = hinter der Suedwand; 0x2D slot 4 (310,-4500,-26946); 0x2C slot 16 sce 3 (19700,-22000,1000,1500); Set(3,0x75); 0x3B slot 4 (1900,-10400,2200,1500) -> ROOM1180 cut 7 / 0x3B slot 6 (18200,-12300,2600,1500) -> ROOM1180 cut 6 | stimmt |
| ROOM4000 @0x013CC / ROOM5000 @0x008FC / ROOM50C0 @0x00A34 | `22 03 93 01` / `22 03 94 01` / `22 03 95 01` | stimmt |
| MAP01.PIX Datei-Byte 0x8DD oberes Nibble = 4 | 0x41 (Zeile 17, Byte 93 -> u186 = 1, u187 = 4) | stimmt |
| SCA ROOM11D0 @0x08CC / @0x08A8 / @0x089C / @0x08D8 | Eintrag 10 x -2200..7750 z -16500..-15500; 7 x -9000..7750 z -24050..-23050; 6 x 6750..7750 z -24050..-15500; 11 x -11950..-1700 z -11450..-10950 (alle typ 1, u0 ff, Band 0) | stimmt |
| SCA ROOM1190 Halle @0x1C70/@0x1C34/@0x1C7C/@0x1C28, Kammer @0x1D00/@0x1CE8/@0x1D24/@0x1CDC | Innenkanten x -24536 / 4624, z -26498 / -10280; x 13998 / 21360, z -25472 / -11432 | stimmt |

Begehbarkeit selbst nachgerechnet (Flutfuellung `M_karte_werkzeug/flut.py`, Port-Kollisionsmodell):
ROOM1190 Spawn Gang-Tuer (3174,-11182) und Spawn Kammer-Tuer (19515,-12109) `verbunden: False`;
ROOM1110 alle drei Teile paarweise `verbunden: False`; ROOM11D0 vom Tuer-Spawn (-300,-17400) aus
begehbar x -7950..3850, z -22550..**-12050** (also auch NOERDLICH der Tuerwand z -16500, s. Punkt 2).

## Messung an der exe (je Punkt, eigener Lauf)

Werkzeug (Scratchpad `M36_abn0/`): `lauf.sh` = Spielstand Slot 0 mit `probe_r36_karte karte`
(nur Raum/Position/Blick, Flags und ein Startbesuch), eigene exe-Kopie `re15_pc_m36abn0_<lauf>.exe`,
`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_WINDOW_SCALE=1 RE15_SOFTWARE_RENDER=1`, Tuer per
`RE15_INPUT_SCRIPT` (Aktionstaste), Karte per `RE15_INV_OPEN_AT=<n>#<raum>`, Abzug
`RE15_INV_FB_SHOT`; Auswertung mit EIGENEM `bild.py` (Farbhistogramm/Pixelsuche je Kasten,
Ausschnitte vergroessert), nicht mit der Sonde des Bau-Agenten. Farben im Abzug: aktuell
(48,8,48)/(48,8,64), besucht (16,56,40)/(16,56,56), Wand (176,176,176), Tuerbalken (224,168,40),
Treppensprosse (240,240,216), Spieler-Ring (16,16,0), Grund (0,16,88)/(0,16,120).
Wichtig fuer die Schienen: `g_engine.frame_count` wird bei JEDEM Raumwechsel auf 0 gesetzt
(main.c:8114), Skript (`BASIS=spiel`) und `RE15_PAD_AT` laufen also je Raum neu an.

### Punkt 1 — Treppen 10A0 auf B1
Lauf `M1_10A0_B1`: Stand ROOM1230 (-358,-6536) Blick -z, KEIN weiterer Besuch; Aktionstaste ->
`[tuer] Sequenz ... DOOR07` -> `[room] PC loaded room10a0.rdt` -> Karte B1.
Abzug: Blatt "B1", 10A0-Absatz aktuell; im Kasten (153,114)-(168,134): 30 Sprossen-Pixel
(240,240,216) = zwei 3-Sprossen-Glyphen bei x161..165 y123/125/127 und x156..160 y126/128/130;
Tuerbalken der 1230-Tuer oben, Spieler-Ring (154..158,116..120). Bild
`laeufe/M1_10A0_B1/crop_landing_x10.png`. -> **erfuellt** (Glyphen gleicher Art kind 4 wie auf
1F/B2, Lage = Projektion der beiden Band-4-Treppen-AOT @0x00E7C/@0x00E90).

### Punkt 2 — Marker ROOM11D0 an der Tuer
Lauf `M2_11D0_ankunft`: ROOM1230 (-4729,-16018) + Flag (4,243), Aktionstaste -> 11D0 bei
(-300,-17400) (`[walk]`-Zeile). Tuerbalken (127..131,124) = Oberkante; Spieler-Ring
(126..130,127..131), Mitte (128,129) = 5 px unter der Tuer, 1 px links. **erfuellt.**
Gegenprobe Bewegung `M2b_11D0_gehen` (2 s vorwaerts nach Sueden, Spieler (-300,-21900)): Ring
(126..130,161..165) - wandert richtig nach unten (Unterwand y172).
Nebenwirkung gemessen `M2c_11D0_zwinger`: Stand im Zwinger-Gang NOERDLICH der Tuerwand (-6700,-13000)
(begehbar x -7950..-5450, z -16500..-12000, Flutfuellung): Ring (107..111,125..129) - klemmt an der
Oberkante. Auf 4500 Einheiten Gang bewegt sich der Marker dort nicht mehr senkrecht (mit der
alten Weltbox z -24050..-10950 GERECHNET, nicht gemessen: y128..144). Die Kachel MAP01.PIX
uv(128,56) ist ein leerer Kasten ohne diesen Gang; mit der
Tuer an der Oberkante gibt es dafuer keinen Platz - im Dossier unter OFFEN benannt. Hinweis H1.

### Punkt 3 — "ROOM 1090" = ROOM1190, zwei Bereiche
Raumzuordnung selbst geprueft (RE-Gate oben): Hunde (Typ 0x20 hinter der Suedwand) + Luke (Obj 4)
in der Haupthalle, Weste (AOT slot 16, Set(3,0x75)) in der Ostkammer, zwei Tueren zum Gang,
Bereiche nicht verbunden. ROOM1090 hat weder Hunde noch Weste. Die Deutung ist belegt.
* `M3a_1190_halle` (Tuer slot 2 aus 1230): rect 1 (76..148,76..124) aktuell, Ring direkt unter
  ihrer Tuer (136..140,78..82); rect 2 im Gang-Ring NICHT gezeichnet (Grund sichtbar).
* `M3b_1190_kammer` (Tuer slot 4, Hunde-Kill-Flags): rect 2 aktuell, Ring an ihrer Nische
  (152..156,70..74); rect 1 nicht gezeichnet.
* `M3c_1190_beide` (Halle vorher besucht, dann Kammer per Tuer): Kasten (77,77)-(142,113) nur
  besucht-gruen 2170+272 px, KEIN Rot; Kammer (145,70)-(158,103) nur aktuell 406+57 px.
-> **erfuellt** (zwei getrennte Bereiche, jeder leuchtet nur fuer sich). Rueckfrage
"1090 oder 1190" steht zu Recht im Dossier.

### Punkt 4 — Tuer 11C0 um 90 Grad
`M4_11C0_tuer` (11B0 -> 11C0 per Tuer): Tuer loeste nicht aus - in ROOM11B0 laeuft beim Laden die
Leon/Ada/Marvin-Szene (EM40/42/4B geladen, Cut 13), die Tuer reagiert nicht (kein Befund der Karte).
`M4a_11C0_laden`: Laden in 11C0 startet den Eintritts-Skriptlauf (Plc_dest), Karte gesperrt.
`M4b_11C0_laden` (11C0 + Flag (4,64), 11B0 besucht): Tuerbalken = genau 5 Pixel (171..175,100)
auf der gemeinsamen Wandzeile, KEIN gelber Pixel bei x171..175 in y90..99 / y101..112.
Zeichner: `laengs_x = (kind == 0 || kind == 2)` (re15_inv_screen.c:2281) - alt kind 1 = senkrecht,
neu kind 2 = waagerecht; 11C0 hat genau EINE Tuer (slot 0 -> 11B0). -> **erfuellt.**

### Punkt 5 — Teilbereiche ROOM1110
* `M5a_1110_ankunft` (1100 -> Tuer -> 1110): Evidence Room aktuell (1010+134 px); Kaesten der
  kleinen Raeume (190,123)-(211,132) und (190,135)-(211,144): nur Grund/Fremdwand, kein besucht.
* `M5b_1110_selbsttuer` (Evidence Room (-2500,-650), Aktionstaste an Selbst-Tuer slot 1 ->
  `[tuer] Sequenz ... DOOR19` -> (-4550,-650)): oberer kleiner Raum aktuell (196+16 px), Evidence
  Room besucht (1017+136 px), unterer kleiner Raum NICHT gezeichnet.
-> **erfuellt.**

### Punkt 6 — Fahrstuhl-Etagen
ECHTE Fahrt (nicht nachgebildet): Stand ROOM1040 an der Fahrstuhltuer + Flag (4,243), 10C0 vorher
zu Fuss besucht; Aktionstaste -> 1080; `RE15_FIRE_AOT=3@120#1080` (Panel-AOT slot 3, ev 6 =
sub06, derselbe Weg wie die Aktionstaste im AOT, aot_common.c:878-884); HOCH 22 Bilder (Cursor
in die 3F-Zelle), VIERECK -> Fahrt -> `[room] PC loaded room1120.rdt`.
* `M6a_fahrt_1F_3F` Karte in 1120 (3F): Kabine (127,137)-(136,146) besucht (52+8 px).
* `M6b_fahrt_blatt2F` (+ RUNTER im Kartenschirm): 2F-Kabine (109,134)-(118,143) NICHT gezeichnet
  (nur Grund, Wand von 10C0 und deren Tuerbalken).
* `M6c_fahrt_blatt1F` (+ 2x RUNTER): 1F-Kabine besucht (55+9 px).
-> Der normale Spielweg ist **behoben**.
* RESTBEFUND (Debug-Menue): `M6d/e/f_debugsprung_1080` - Stand ROOM1100, `RE15_DEBUG_JUMP=1080@gp`
  (= Quadrat im Debug-Menue "108 ELEVATOR"), Karte in der Kabine: 1F-Kabine aktuell, **2F-Kabine
  besucht (55+9 px) und 3F-Kabine besucht (56+8 px)** - auf 3F steht die Kabine als einziger
  gruener Kasten auf einem Blatt, auf dem der Spieler nie war. Ursache im Code: bei unbekannter
  Etage (`re15_karte_fahrstuhl_blatt() < 0`: Vorraum kein Etagenraum, Bits (3,54/55/56) leer)
  greift weiter die alte "gleiche Baender"-Regel (re15_map_zones.c:436-441) und setzt ALLE drei
  Kabinen-Bits 112..114. Genau das gemeldete Bild. -> Punkt 6 **teilweise** (Mangel M1).

### Nebenwirkungen (selbst gemessen)
* `M7_sweep` (`RE15_MAP_SHOT_SWEEP`, alle Blaetter aufgedeckt): B1 traegt die zwei neuen
  10A0-Glyphen (30 Sprossen-Pixel), B2 (30 px, (175..188,110..117)) und 1F (60 px) unveraendert je
  zwei; Blatt 0 rect 1 = 1190-Halle, rect 2 = Kammer im Gang-Ring; ROOM11A0 steht auf "DRAINS B2"
  rect 2.
* `M8_11A0_sprung` (Debug-Menue "11a SEWER EXIT", Spawn (-50,-1800,24200)): Karte zeigt
  "DRAINS B2", 11A0 aktuell. Der Ring liegt (106..110,59..63) in der Kerbe der L-Kachel, also
  neben der gemalten Flaeche - dieselbe Projektion (Zonenkasten + Blatt 6 rect 2) galt schon vor
  dem Zweig fuer Band 0/1 (Gast-Zeile), also NICHT von Spur M verursacht (Hinweis H3).
* Alte Spielstaende: Ostkammer 1190, kleine Raeume 1110, 11A0-Band-3 und die Kabinen erscheinen
  erst nach erneutem Betreten (im Dossier unter "Fuer den Nutzer" benannt; Speicherformat v9
  unveraendert, nur neue Bits im vorhandenen Feld visited_floor[16]).
* RE2-Beleg der NUTZER-VORGABE selbst nachgefahren (`re2_areas.py` auf `info/re2leon/PSX.EXE`):
  vis 100 (ROOM601) in Area 13/16/18, vis 94 (ROOM505) in 10/12/13/16 - RE2 fuehrt EIN Bit je Raum
  ueber alle Blaetter; das Etagen-Besucht der Kabine ist also richtig als NUTZER-VORGABE markiert.

## Vertrag und Pfade

* Dateien des Zweigs (`git diff master...HEAD --name-only`, 37): Dossier/Bilder/Werkzeuge unter
  `analysis/befunde_runde36/M_karte*`; Engine `karte_fahrstuhl_1080.c`, NEU `karte_r36.c`,
  `re15_map_zones.c/.h` (Kartenmodul, Eigentum M/N laut VERTRAG §1.4), Header
  `re15_karte_fahrstuhl.h`, NEU `re15_karte_r36.h`; Tests NEU `test_r36_karte.c`,
  `tests/unit/probes/r36_karte.cmake`, `tests/integration/test_r36_karte.cmake`, angepasst
  `test_map_teilbereich.c`, `test_r30_karte_nutzerstand.c`. KEIN release/, platform/android/,
  shared_assets/PSX/, tests/*/CMakeLists.txt, scd_room_setup.c, main.c. -> haelt.
* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine belegt (die neuen Besucht-Bits 96..117 liegen im
  kartenseitigen Feld visited_floor, nicht in g_game.flags). -> haelt.
* Leitwoerter im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|getenv`): kein
  Treffer in den 1608 hinzugefuegten Zeilen. -> haelt.
* Konstanten: jede Lage/Box/Blattnummer im Code mit @0x.../SCA-/RDT-Offset oder als Projektion
  hergeleitet; Fahrstuhl-je-Etage als NUTZER-VORGABE gegen RE2 gekennzeichnet; Bit-Basen 96/112
  sind Speicherlayout (im Header begruendet). Commit-Message 98c2e109 traegt die Adressen. -> haelt.
* Herkunft Punkt 1 selbst nachgesehen: 0549cfae (nur in v0.8.23) legte die B1-Zeile fuer 10A0
  (Blatt 0 rect 6) OHNE Treppenmarken an - der Fix (Marken) erklaert den Befund.

## Urteil je Punkt

| # | Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|---|
| 1 | Treppen 10A0 auf B1 fehlen | erfuellt | M1: zwei 3-Sprossen-Glyphen (30 px) im B1-Absatz nach echter Tuer 1230->10A0 |
| 2 | 11D0: Marker in der Mitte statt am Tuereingang | erfuellt | M2: Ring-Mitte (128,129), Tuerbalken (127..131,124); M2b wandert nach unten; Zwinger-Gang klemmt (H1) |
| 3 | "1090": zwei Tueren / zwei getrennte Bereiche | erfuellt | M3a/b/c: Halle nur rect 1, Kammer nur rect 2, beide getrennt (gruen/rot); Raum = ROOM1190 belegt |
| 4 | Tuer 11C0 um 90 Grad verdreht | erfuellt | M4b: Balken genau (171..175,100) waagerecht, keine senkrechten Pixel |
| 5 | kleine Tuerbereiche 1110 vorab besucht | erfuellt | M5a: kleine Raeume ungezeichnet; M5b Selbst-Tuer: nur der betretene wird sichtbar |
| 6 | Fahrstuhl-Etagen vorab besucht | teilweise | M6a-c echte Fahrt 1F->3F: 1F+3F besucht, 2F nicht; ABER M6d-f Debug-Sprung "ELEVATOR": 1F aktuell, 2F UND 3F besucht (M1) |

## Maengel

**M1 (Punkt 6, Restpfad Debug-Menue).** Springt man ueber das Debug-Menue in den Fahrstuhl
(JUMP "108 ELEVATOR" = `RE15_DEBUG_JUMP=1080@gp`, Stand in ROOM1100), zeigt die Karte die Kabine
auf 2F (55+9 besucht-Pixel im Kasten (109,134)-(118,143)) und auf 3F (56+8 px im Kasten
(127,137)-(136,146)) als besucht, obwohl der Spieler dort nie im Fahrstuhl war; 3F wird dadurch sogar
als Blatt anwaehlbar (einziger Kasten). Ursache: unbekannte Etage (`re15_karte_fahrstuhl_blatt() < 0`,
Vorraum kein Etagenraum, Bank 3 Bit 54/55/56 leer) -> die alte "gleiche Baender"-Regel
(re15_map_zones.c:436-441) setzt alle Kabinen-Bits 112..114 (bei ROOM4020 ebenso 115..117).
Der Nutzer benutzt das Debug-Menue (Auftrag Z. 28) - das ist woertlich das gemeldete Bild.
Nachpruefen (Arbeitsordner beliebig): `probe_r36_karte karte re15_card.mcr 1100 -26130 0 -11036 2048
besucht:1100:-26130:-11036`, dann die exe (eigene Kopie) mit `RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
RE15_DEBUG_JUMP=1080@gp RE15_PAD_AT=100:U RE15_INV_OPEN_AT=40#1080 RE15_INV_FB_SHOT=karte.bmp
RE15_INV_FB_SHOT_AT=85 RE15_EXIT_AT=170#1080` -> im Abzug (Blatt 2F) ist der Kasten
(109,134)-(118,143) besucht-gruen; mit `RE15_PAD_AT=100:U,108:U` dasselbe auf 3F (127,137)-(136,146).
Zum Vergleich der normale Weg (M6b): dort ist derselbe 2F-Kasten nur Grund.
Belegter Ersatz fuer "Etage unbekannt": das Original zeigt die Kabine IMMER auf Blatt 2
(Seiten-Setzer, Raumindex 8: `ori v0,zero,0x2` @0x8004b684 im Delay-Slot von `j 0x8004b888`,
`sb v0,0x260e(at)` @0x8004b88c - selbst disassembliert), fuer ROOM4020 Blatt 8 (`ori v1,zero,0x8`
@0x8004bdb4). Also bei unbekannter Etage nur DIESE eine Kabinen-Zeile setzen statt aller drei, und
einen Riegel (Debug-Sprung bzw. Kabine ohne Etagenraum davor) dazu. Hinweis zur Umsetzung:
`floor_row()` waehlt bei fb < 0 schon die erste Zeile (Band 0 -> Blatt 2 bzw. 8); es genuegt, die
"gleiche Baender"-Schleife fuer die beiden Kabinen (re15_karte_r36_fahrstuhl_bit() >= 0) ganz
auszulassen.

## Hinweise (kein Mangel)

* **H1 (Punkt 2, Zwinger-Gang 11D0).** Gemessen M2c: im begehbaren Gang noerdlich der Tuerwand
  (x -7950..-5450, z -16500..-12000) klemmt der Ring an der Oberkante (107..111,125..129) und bewegt
  sich auf 4500 Einheiten nicht mehr senkrecht. Unvermeidlich, solange die Tuer an der Oberkante der
  leeren Kachel MAP01.PIX uv(128,56) stehen soll; im Dossier unter OFFEN benannt. Wenn der Nutzer
  das bemaengelt, braucht es eine Kachel mit Gang.
* **H2 (Testabdeckung Punkt 6).** `integration_r36_karte` F/H bilden die Fahrt nur nach
  (Spielstand mit der Raumfolge 1040->1080->1120 aus der Sonde); die echte Fahrt (Panel-AOT slot 3,
  Cursor, VIERECK, Skript-Tuer) faehrt kein Test. Meine Laeufe M6a-c zeigen, dass sie richtig
  ankommt (`RE15_FIRE_AOT=3@120#1080`, HOCH 22 Bilder, VIERECK) - als Riegel uebernehmbar.
* **H3 (ROOM11A0, nicht Spur M).** Debug-Spawn (-50,-1800,24200) projiziert in die Kerbe der
  L-Kachel (Blatt 6 rect 2), Ring neben der gemalten Flaeche; gleiche Projektion wie vor dem Zweig.
* **H4 (Punkt 3).** Rueckfrage an den Nutzer ("ROOM 1090" = ROOM1190?) ist berechtigt; der
  Debug-Menue-Name von ROOM1190 lautet "FIRING-ARMORY" (Schiessstand + Waffenkammer) und passt zu
  den zwei Bereichen.

## Ergebnis

Fuenf Punkte erfuellt, Punkt 6 teilweise (M1). Gates (RE-Gate Stichprobe 11 Zitate, Vertrag, Pfade,
Leitwoerter, gezielte Tests 66/66) halten. **Nicht bestanden** wegen M1.

## Vorbemerkung: `git diff master` zeigt fremde Dateien

`git diff master --name-only` listet 63 geloeschte `synchro/STAGE1/room1xx1/*.wav` und eine
geaenderte `AUFTRAG.md`. Das sind KEINE Aenderungen des Zweigs: master ist nach dem Abzweig um
836023f5 (AUFTRAG-Nachtrag Spur J) und 565da62b (Elza-Sprachdateien) weitergelaufen. Der Zweig
selbst (`git diff master...HEAD`, gegen die merge-base 78a4a748) beruehrt 37 Dateien, keine davon
unter synchro/, release/, platform/android/, shared_assets/PSX/ oder tests/*/CMakeLists.txt.
