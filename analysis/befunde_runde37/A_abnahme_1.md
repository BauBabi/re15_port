# Runde 37 — Spur A "pri1100": Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum: 2026-10-12. Baum `.claude/worktrees/r37_pri1100`, Zweig `r37/pri1100`, Stand 75a2b03d (Basis f1e019ca).
Pruefer: Abnahme 1 (kein Code geaendert; nur diese Datei committet).
Gegenstand: Spur A (AUFTRAG.md #1) nach Nachbesserung 1 (Abnahme 0: M1 Elza ROOM1101 ohne Masken, M2 Nutzertext).
Der im Workflow mitgeschickte Nutzer-Nachtrag (Beretta M93R) gehoert zu Spur G (r37_beretta) und ist hier nicht bewertet.
Messordner (ausserhalb des Baums): Scratchpad `ab1/` (Laeufer `run.sh`, Auswertung `py/ppm.py` (Punkte mit
Kanaldifferenz > 24), `py/marke.py` (Nutzerbild 960x720 an den Pixelmitten auf 320x240), Laeufe `runs/<name>/`).

## 0. Stand / Bau
- `git log master..HEAD`: 15 Commits (a0d82ed0 .. 75a2b03d), davon 7 der Nachbesserung 1 (553dbacf .. 75a2b03d) +
  0ae925a6 (VERTRAG 1.7 im Zweig) + 0093d0dc (Abnahme 0). master ist seit f1e019ca nur um Doku-/Orchestrator-
  Commits gewachsen (kein `re15_port/`-Pfad, `git diff f1e019ca master -- re15_port` leer); die Spur-Aenderungen =
  `git diff master...HEAD` (41 Dateien). Die `.gitattributes`-Zeile und AUFTRAG/UEBERGABE/orchestrator in
  `git diff master` stammen von master, nicht vom Zweig.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do`, `LOCAL-BUILD-OK (build)` (exe 14:11:50; Arbeitsbaum sauber = HEAD, nur `?? build/`).
- Messkopie `re15_port/build/platform/pc/re15_pc_ab1m.exe` (eigener Name, nach der Abnahme geloescht).
- Masken ROOM1100.* seit Abnahme 0 (9e71f0a1) unveraendert (`git diff 9e71f0a1 HEAD -- re15_port/shared_assets
  analysis/esp_masken_2026-09-03 re15_port/tools/maske/raum.py` leer); sha256 MSK 57fed6e2.., PRI01 8fe6fc58..,
  PRI02 67e5d456... Vergleichssaetze im Scratchpad: `m_alt` = f1e019ca (v0.8.24, MSK 1b7a671c..), `m_v0823` = Tag
  v0.8.23 (MSK c5f021d8..), eingeschoben ueber `RE15_CD_ROOT`.

## 1. Punkt 1 (Wortlaut AUFTRAG.md #1)
> Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker. Die Marker Datei liegt da wo immer die
> DAteien liegen mit F9.

Gemeinsame Umgebung aller Laeufe: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`. Wandfenster = x168..191 y126..149 (wie die Riegel). "ohne Zombie" =
`RE15_GEGNER_POS=1,-14000,-11000,1024@5#<raum>` (Zombie 1 hinter die Kamera).

### 1.1 Leon, ROOM1100 — die beiden Marken (Nachbau gegen die Nutzerbilder im Hauptbaum)
Szene F892: `RE15_DEBUG_JUMP=1100@250 RE15_PLAYER_POS=-14914,-25645,392 RE15_FORCE_CUT=1
RE15_GEGNER_POS=1,-16157,-23876,1024@5#1100 RE15_FRAMEDUMP=150:f.ppm RE15_EXIT_AT=150#1100` (Laeufe `l892_*`).
Szene F240: `... RE15_PLAYER_POS=-15607,-23728,-1104 ...`, Zombie 1 an der Spawnlage (Laeufe `l240_*`).

| Szene | Masken (Logzeile) | Wandfenster gegen "ohne Zombie" | Nutzerbild gegen Nachbau (Fenster) |
|---|---|---|---|
| F892 | HEAD (`[pri] cut=1 pri_offset=0x60C masks=52 fg_atlas=1`) | **0** (max 0) | 28 (= Kopf + Arm jetzt verdeckt) |
| F892 | v0.8.24 (`masks=105`) | 28 (max 176) | **0 (max 0)** — Nachbau = Marke F892 |
| F892 | keine (`RE15_NO_PRI=1`) | 101 | |
| F240 | HEAD | **0** (max 0) | 16 |
| F240 | v0.8.23 (`masks=105`) | 16 (max 127, x179..183 = schwebender Kopf) | **0 (max 0)** — Nachbau = Marke F240 |
| F240 | keine | 164 | |

Ganzes Bild F892 HEAD gegen "ohne Zombie": 49 Punkte, alle x155..165 y129..145 = Stiefel/Hand in der Tueroeffnung
(wie in der Marke des Nutzers, nicht hinter der Wand).

### 1.2 Leon, ROOM1100 — echter Tuerweg (kein FORCE_CUT, kein Sprung in den Raum)
`RE15_DEBUG_JUMP=10D0@250 RE15_PLAYER_POS=-19300,24000,2048 RE15_PRESS=square@60 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=200 RE15_INPUT_SCRIPT=U2.5,W10 RE15_FRAMEDUMP=300-420/40:f RE15_EXIT_AT=420#1100`
(Laeufe `ltw_*`): Log `[room] PC loaded room10d0.rdt` -> `[room] PC loaded room1100.rdt` -> `[pri] cut=1
pri_offset=0x60C masks=52 fg_atlas=1` (keine Zwilling-Zeile: 1100 bleibt 1100). Bilder 300/340/380/420 je:
HEAD **0**, ohne Masken 163, v0.8.24 25 (x171..183 y140..145).

### 1.3 Elza, ROOM1101 — echter Tuerweg 10D1 -> 1101 (der Mangel M1 der Abnahme 0)
Wie 1.2 plus `RE15_TITLE_SHOT_CHAR=1`, `RE15_EXIT_AT=420#1101` (Laeufe `etw_*`). Log: `[room] PC loaded room10d1.rdt`,
`[pri] ROOM10D1: Masken der Leon-Variante MASKS/ROOM10D0.MSK (Runde 37 Spur A)`, `[room] PC loaded room1101.rdt`,
`[pri] ROOM1101: Masken der Leon-Variante MASKS/ROOM1100.MSK (Runde 37 Spur A)`, `[pri] cut=1 pri_offset=0x60C
masks=52 fg_atlas=1` (Abnahme 0: `masks=0 fg_atlas=0`). Bilder 300/340/380/420 je: Wandfenster mit Masken **0**
(max 0), ohne Masken **163** (x168..189 y128..145); ganzes Bild mit Masken gegen "ohne Zombie" 20 Punkte, alle
x160..165 y129..142 (Stiefel/Hand in der Tueroeffnung, wie bei Leon). Bild
`ab1/elza_tuerweg_F420_mit_ohne_ohneZombie.png` (x140..219 y100..169, 4-fach: mit | ohne Masken | ohne Zombie).
Gegenprobe ueber die ECHTE Spielerwahl statt `RE15_TITLE_SHOT_CHAR` (Laeufe `ps_*`: `RE15_TITLE_CONFIRM_MS=1500
RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, ohne TITLE_SHOT, sonst gleich): Log `[pl] Spieler-Familie PL04
(character=4, Elza-Bit=1)` -> `room10d1` -> `room1101` -> Leon-Masken, `masks=52 fg_atlas=1`; Bilder 300/420:
Wandfenster 0 / ohne Masken 163, und ganzes Bild **bitgleich** zu den `etw_a`-Bildern (0 Punkte, max 0).

### 1.4 Elza, ROOM1101 — Szene der Marke F892 und Cut 2 (Nordecke)
- F892 als Elza (`RE15_TITLE_SHOT_CHAR=1`, sonst wie 1.1, `RE15_GEGNER_POS=...#1101`, `RE15_EXIT_AT=150#1101`;
  Laeufe `e892_*`): Log `[pri] ROOM1101: Masken der Leon-Variante MASKS/ROOM1100.MSK`, `cut=1 ... masks=52 fg_atlas=1`.
  Wandfenster mit Masken **0**, ohne Masken 101; ganzes Bild 49 Punkte x155..165 (Tueroeffnung) — Zahl fuer Zahl wie Leon.
- Cut 2 (`RE15_PLAYER_POS=-15100,-18500,-1024 RE15_FORCE_CUT=2`, Laeufe `ec2_*`/`lc2_*`): `cut=2 pri_offset=0x610
  masks=53 fg_atlas=1` fuer beide. Was die Masken verdecken (mit gegen ohne Masken, ganzes Bild): Elza 56 Punkte
  x127..139 y127..139 (Kopf des Liegenden im Querflur hinter dem Wandpfeiler), Leon 56 Punkte — **dieselbe Punktmenge**
  (Mengenvergleich identisch). Bild `ab1/elza_c2_mit_ohne.png`.

### 1.5 Echtes Fenster (Elza, Tuerweg) — Gegenprobe zur Framedump-Messung
Auf der Maschine liefen gleichzeitig fuenf fremde Fenster mit demselben Titel "RE1.5 Rebuilt — PC" (andere Spuren) —
gdigrab per Titel haette ein fremdes Fenster greifen koennen. Deshalb das EIGENE Fenster ueber den Prozess-Handle
(`ab1/fenster.ps1`: Start-Process, Warten auf `cut=1 pri_offset=0x60C` im debug.log + 20 s, `PrintWindow(hwnd,
PW_CLIENTONLY|PW_RENDERFULLCONTENT)` = das praesentierte Bild, kein Readback). `RE15_WINDOW_SCALE=3`, Elza-Tuerweg wie
1.3 (`W60` statt `W10`): Log `room1101` / `Leon-Variante MASKS/ROOM1100.MSK` / `masks=52 fg_atlas=1`; Fenster 960x720,
an den Pixelmitten auf 320x240: Wandfenster gegen Framedump "ohne Zombie" **0 (max 0)**, gegen "ohne Masken" 163;
ganzes Bild gegen den Framedump F420 mit Masken 35 Punkte, alle x142..154 y89..111 = auf Elza (Ruheanimation, spaeterer
Zeitpunkt). Ausschnitt `ab1/elza_fenster_ausschnitt.png`: Wandflaeche sauber, nur Stiefel/Hand in der Tueroeffnung.

**Urteil Punkt 1: erfuellt** — fuer Leon (beide Marken bildpunktgenau nachgebaut, mit HEAD verdeckt; echter Tuerweg 0)
und fuer Elza im selben Raum auf ihrem Pflichtweg (Abnahme-0-Mangel M1 behoben: vorher `masks=0`, 163..168 Punkte
ueber der Wand; jetzt Leon-Masken, 0 Punkte).

## 2. Nebenwirkungen des Masken-Zwillings (20 Elza-Raeume)
- **Props Leon gegen Elza** (eigene Sonde `ab1/probe/props_paare.c`, gegen die gebauten libre15_engine.a /
  libre15_test_support.a gelinkt; SCD-Init wie probe_1190_props: main00 + sub00, 4 Init-Takte + 300 Takte, frische
  Flags): in 19 der 20 Paare sind die Obj_model_set-Props nach dem Init identisch (obj_id, Typ, aktiv, Lage).
  Einzige Abweichung ROOM1130/1131 obj0: Leon (-1240,0,16300), Elza (-1240,0,16228). Gemessen (Cut 3 = einziger
  Masken-Cut, `RE15_FORCE_CUT=3`, Laeufe `l1130_*`/`e1131_*`): was die Masken verdecken, Leon 73 Punkte, Elza 73
  Punkte, **dieselbe Punktmenge** (nur Leon 0 / nur Elza 0). Grenze: Props, die erst spaeter im Spiel (andere Flags)
  angelegt werden, deckt die Sonde nicht ab.
- **Port-Props in Elza-Raeumen:** der Prison Key (Runde 36 Spur Q, ROOM10F1) liegt an derselben Stelle (8050,-1460,2700)
  wie der Parking Lot Key in 10F0; seine Lage wurde damals gegen die 10F0-Port-Masken gewaehlt ("Cut 0 (97;170) und Cut 1
  (123;113) ohne Maske", Q_schluessel.md §3.2) — gilt jetzt unveraendert fuer Elza. Die verdeckte Schreibmaschine auf dem
  Holzschreibtisch 10F0/10F1 Cut 5/6 steht in den v0.8.24-Release-Notes bereits als offene Nutzerfrage fuer BEIDE
  Varianten ("ROOM10F0/10F1 Cut 5/6 ...") — mit dem Zwilling stimmt das jetzt auch fuer Elza; kein neuer Befund.
- **Dokument 4 in ROOM1011** (Folgeanpassung `dokumente_r34.c`, Klemme ueber `re15_pri_masken_raum`): Elza,
  `RE15_DEBUG_JUMP=1010@250 RE15_PLAYER_POS=3400,7200,1024 RE15_FORCE_CUT=<c> RE15_FRAMEDUMP=100`, Blatt da gegen
  genommen (`RE15_SET_FLAG=9:60`), mit gegen ohne Masken (Laeufe `d<c>_a..d`). Sichtbare Blattpunkte mit Masken /
  ohne Masken: Cut 0 **202/202** (masks=100), Cut 1 **171/163** (masks=88), Cut 6 **130/129** (masks=86), Cut 8
  **176/176** (masks=88) — das Blatt ist ganz zu sehen (Ueberschuss = 8-bpp-Atlaspalette an der Blattkante); exakt die
  Zahlen des Dossiers §8.1a.
- **Zensus selbst nachgerechnet** (eigenes Skript `ab1/py/zensus_eigen.py`, unabhaengig von zwilling_zensus.py; verlangt
  zusaetzlich, dass der Hintergrund JE Cut wirklich gefunden wird, nicht nur "beide fehlen"): 103 Paare, **20 Rueckfaelle
  == Tabelle** `gen/pri_masken_zwilling_r37.inc`; Kamera verschieden nur 1030/1031 und 11C0/11C1 (beide ohne Leon-MSK);
  kein Kandidat mit Leon-MSK scheitert an Kamera/Hintergrund; eigene Elza-MSK: 1221, 2091 und 15 Paare der Stages 3..6.
  `python re15_port/tools/maske/zwilling_zensus.py --nur-pruefen` -> "Tabelle ... aktuell".
- Lader-Code gelesen: R15M nur bei Original-NULL-Sektion (main.c `if (pri_n == 0 ...)` -> `re15_entladen_msk`), MSK
  (entladen_pc.c) und Atlas (bg_pc.c, BEIDE `MASKS/..._PRI%02d.TIM`-Pfade) kommen aus derselben Zwilling-Id; der Cache-
  Schluessel bleibt die echte Raum-Id (1100 <-> 1101 laedt neu). Original-Atlas-Pfade (`BSS/ROOM%04X/PRI%02d.TIM`, SLD-
  Auszug) bleiben bei der echten Id (nur fuer Original-Masken). Jeder `MASKS/`-Leser in engine/platform geht ueber
  `re15_pri_masken_raum` (grep). Klemmen je Raum: nur dokumente_r34.c (angepasst) und irons_tisch_1150.c (1150/1151 ohne
  Port-Masken) — keine weitere raumgebundene Masken-Sonderbehandlung gefunden.

## 3. RE-Gate (Stichprobe selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, PSX.EXE)
1. **FUN_800392d4 (Masken je Kamerasatz)**: `800392d8 lw a0,-14472(a0)` (RDT-Zeiger 0x800ac778), `800392e0 lh v0,4068(v0)`
   (Cut 0x800b0fe4), `80039310 lw v1,36(a0)` (RDT+0x24), `80039314 sll v0,v0,5` (Cut*0x20), `8003931c lw t5,28(v0)`
   (Satz+0x1C), `80039324 lw v1,0(t5)` / `80039328 addiu v0,zero,-1` / `8003932c bne v1,v0,0x8003933c`, sonst
   `80039334 j 0x8003955c`. ✓ wie zitiert.
2. **Cut-Wechsel / Projektion**: `80021e5c lw v1,36(v1)` / `80021e60 sll v0,v0,5` / `80021e68 lhu a0,2(v0)` (fov) /
   `80021e6c jal 0x80066c30` / Delay `srl a0,a0,7`; Sprungziel `80066c30 .word 0x48c4d000` = ctc2 a0,$26 (H) / `jr ra`.
   `80021e84 lw v0,36(v0)` / `80021e88 sll a0,a0,5` / `80021e8c jal 0x80053ca4` / Delay `addu a0,a0,v0` (a0 = Satz).
   **FUN_80053ca4**: `80053cac addu s3,a0,zero`; `80053d1c lw v0,16(s3)` / `80053d20 lw v1,4(s3)` / `80053d30 lw v0,20(s3)` /
   `80053d34 lw v1,8(s3)` / `80053d44 lw v1,12(s3)` / `80053d48 lw v0,24(s3)` -> `80053d60 jal 0x80065f60` (SquareRoot0).
   Die Berichtigung stimmt: `80053cc0 lui s5,0x8007` / `80053cc4 addiu s5,s5,11596` (0x80072d4c), `80053cdc..d18` kopiert
   32 Byte von s5 nach s6 = 0x800b5288 — NICHT den Satz. Der falsche Satz steht nur noch im wip-Commit 4a07f568 (Historie);
   Code, Werkzeug, Tabelle, Test und der Fix-Commit 75a2b03d zitieren @0x80053cac/@0x80053d1c..d48 (grep `80053cdc` nur im
   Dossier als Berichtigung). Der Port (`camera_common.c re15_camera_build_view`) liest genau fov/pos/target = Satz+0x02..+0x1B
   -> das Vergleichsfenster +0x00..+0x1B deckt alles ab, was die Projektion liest.
3. **Hintergrund**: `80021c88 lh a3,4068(a3)` (Cut), `80021c90 lh a1,4066(a1)` (Raum 0x800b0fe2), `80021ca8 lh v0,4064(v0)`
   (Stage), `80021cc0 addiu at,at,17292` (0x8007438c) + Stage*4 -> `80021cc8 lw v0,0(at)`, `80021ccc sll a1,a1,1` /
   `80021cd4 lhu v0,0(a1)` / `80021cdc addiu v0,v0,-1` (Dateinummer), `80021cd8 sll a3,a3,5` + `80021d20 addu v0,v0,a3`
   (+Cut*32 Sektoren). Tabelle gelesen: 0x8007438c[0] = 0x8007429c; STAGE1 u16[0x10] = 729 (Folge 681, 684, ... Schritt 3).
   Gegenprobe Raum-Lader (RE_15_Quellcode_V2/FUN_800396fc.c): RDT-Datei = Tabelle[Raum] - (DAT_800aca3c >> 31), BSS =
   Tabelle[Raum] - 1 -> je Raumpaar EIN Hintergrund, die Variante waehlt nur die RDT. ✓
- Konstanten im Diff: keine neue Verhaltenskonstante. Die Tabelle ist das Ergebnis des Zensus (Kriterium mit den Adressen
  oben belegt, Freigabe VERTRAG 1.7); die Masken selbst bleiben NUTZER-VORGABE (Original-Sektionen NULL, Dossier §2).
  Test-Konstanten (Wandfenster, Schwelle 24, Lagen der Marken) sind Messaufbau aus den Nutzer-Marken / Abnahme 0.
- Rate-Woerter (deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/vorlaeufig/geschaetzt/ungefaehr/approx) im
  Diff `re15_port` und in den Commit-Texten master..HEAD: **0 Treffer**. Kein Env-Schalter als Abschluss.
- Erklaert der Fix den Befund? Ja: Elza 1101 hatte `masks=0` (Abnahme 0, Dossier "vorher"), weil der Lader die volle Id
  suchte; mit dem Zwilling laedt sie dieselben 52/53 Masken wie Leon, und das Wandfenster ist gleich der Szene ohne Zombie
  (1.3/1.4). Fuer Leon unveraendert (1.1/1.2).

## 4. Gegenproben (Trennschaerfe der Riegel, Binaerdatei, Abnahme-0-Mangel M2)
- Trennschaerfe selbst gegengebaut (Scratchpad, gcc gegen libre15_engine.a/libre15_test_support.a des Baums):
  - `test_r37_pri1100.c` + eigenes `re15_pri_masken_raum` = Identitaet (vor der Bibliothek gelinkt): `elza` -> **FAIL**
    ("ROOM1101 Cut 1: Sektion (0) oder Atlas fehlt"), `zwilling` -> **FAIL** ("103 Paare, 20 Rueckfaelle, 20 Abweichungen");
    `leck`/`marke` (Leon) bleiben gruen — wie erwartet.
  - `test_r34n_e_dokumente.c` + `dokumente_r34.c` aus f1e019ca (Klemme nur 1010): **FAIL** "K ROOM1011 Cut 0 obj 3: Klemme
    -1" (und Cut 1) -> der angepasste Riegel verlangt die Engine-Aenderung.
- Disassembliert wurde `info/Re1.5/PSX.EXE` (sha256 e6b340fd.., im Baum und im Hauptbaum gleich), Auslieferungsstand:
  `@0x80026e4c = 08 00 e0 03` (jr ra), nicht die gepatchte EXE.
- Messung M2 nachgefahren (Westbahn Leon, `RE15_DEBUG_JUMP=1100@250 RE15_PLAYER_POS=-15250,-13500,1024
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=U4,R0.5,U3,W1`, Zombie 1 weg,
  `RE15_FRAMEDUMP=60-330/6`, Laeufe `wb_neu/wb_alt/wb_nopri`): neu gegen v0.8.24 ausserhalb des Palettenbands der
  Tuerrahmenkante (x170..176 y55..100) nur **F162 3** (176..177,151..152), **F192 2** (187..188,162..163), **F222 4**
  (201..203,177..179), jeweils "neu deckt, alt zeigt" (Punkte neu != ohne Masken, alt == ohne Masken); state.txt F162
  `PL(-15132,-22148,rot=2464) mo=105` (an der Wand). Exakt die Zahlen des Dossiers §8.2; der berichtigte Nutzertext §7
  ("2..5 Bildpunkte der Stiefelspitze") stimmt. Bild `ab1/leon_westbahn_F162_neu_alt_ohne.png`.

## 5. Vertrag (VERTRAG.md)
- Pfade (`git diff master...HEAD --name-only`, 41 Dateien): Dossier/Bilder/Abnahme 0/VERTRAG (1.7 im Zweig), auswahl.json,
  neu `engine/src/pri1100_r37.c`, `engine/src/gen/pri_masken_zwilling_r37.inc`, `include/re15_pri1100_r37.h` (§1.4 "neue
  Logik in neuen Dateien"), Lader `platform/pc/src/entladen_pc.c` (3 Zeilen + Include), `platform/pc/src/bg_pc.c` (2 Zeilen
  + Include), `platform/psx/main.c` (1 Zeile + Include) — alle drei von §1.7 gedeckt ("den Masken-LADER ... und was die
  PRI-Masken je Raum laedt"), `platform/pc/main.c` (1 Zeile Messhaken, aus dem Bau), `shared_assets/PSX/MASKS/ROOM1100.*`
  (§1.6), Werkzeuge `tools/maske/raum.py`, `tools/maske/zwilling_zensus.py`, Tests `tests/unit/probes/r37_pri1100.cmake`
  (§1.5), `tests/unit/test_r37_pri1100.c`, `tests/integration/test_r37_pri1100_{marke,elza}.cmake` (beide
  `cmake_minimum_required(VERSION 3.16)`; `file(COPY_FILE)` wie bestehende Integrationen, Wurzel verlangt 3.21; Endung
  ueber `LAST_EXT` -> Linux ohne .exe). KEINE Aenderung an release/, platform/android/, Test-CMakeLists, anderen
  shared_assets.
- **Ausserhalb der Spur-Liste** (vom Bau-Agenten offen gemeldet): `engine/src/dokumente_r34.c` (1 Zeile Schluessel ueber
  `re15_pri_masken_raum` + Include + Kommentar "Runde 37 Spur A (VERTRAG 1.7)"), `include/re15_dokumente.h` (nur Kommentar),
  `tests/unit/test_r34n_e_dokumente.c` (Riegel: Masken wie der Lader, 1011 Cut 0/1/6/8 mit Klemme statt "frei"),
  `tests/unit/probe_r34n_e_dokumente.c` (Messsonde). Form eines kleinen Hakens (§1.4), zwingende Folge von §1.7 (ohne sie
  verdeckten die Tischmasken in Elzas 1011 das Blatt: Dossier 73/163, der Gegenbau oben faellt mit "Klemme -1"). Haelt
  nach meinem Urteil; bei der Integration bestaetigen (Datei der Runde 34, in §1.4 keiner Spur zugeteilt).
- Bank-9-Bits / Nachrichten-Ids / AOT-Slots / Ereignisse: keine (grep im Diff nach flag_set/game_flag/scd_event_fire/
  aot.slots/MSG_ID leer). ✓
- Paket: keine neuen Asset-Dateien; die Leon-Dateien der 20 Zwilling-Raeume werden schon ausgeliefert.
  `release/make_package.sh` prueft MASKS/ROOM1100.MSK/_PRI01.TIM/_PRI02.TIM (geaendert, vorhanden). APK neu bauen
  (geaenderte ROOM1100-Masken) und Android-.cxx-Cache neu konfigurieren (neue Quelle pri1100_r37.c) — Sache der Integration,
  im Dossier §7 genannt.

## 6. Tests (gezielt, kein Volllauf; PATH msys64 zuerst)
- Unit (`ctest --test-dir re15_port/build -R "unit_r37_pri1100|unit_r34n_e_dokumente|unit_pri_|unit_sld_atlas|unit_r36_pri|
  unit_r34n_g_maskgrp|room1100|unit_r22_10f0_figur|unit_bss_room_source|unit_map_innenwand|unit_r35_entladen|unit_r36_elza|
  unit_elza_zweig|unit_r35_inventar1050_elza|unit_r30_elza|unit_r33_tuer1120_elza|unit_r34n_d_adaruf_elza|unit_r35_cut10f0|
  unit_r36_klettern_10f0|unit_r36_szenenleon_abgang10f0|unit_r30_irons_diary_dokument|unit_r30_hund_tod_elza" --timeout 600
  -j4 --output-on-failure`) -> **100% tests passed, 0 tests failed out of 48**. Direkt nachgesehen: `elza` = 8280/0 und
  1300/0 (Cut 1), 7552/0 und 1345/0 (Cut 2), Marke 14/14 mit ROOM1101-Kamera; `zwilling` = 103 Paare, 20 Rueckfaelle,
  0 Abweichungen (kein leeres Gruen).
- Integration, sequentiell (-j1, Maschine auf 100 % CPU durch andere Spuren; `-R "integration_r37_pri1100|
  integration_r34n_e_dokumente_bild|integration_r36_pri_schatten|integration_pri_masken|integration_headless_run|
  integration_boot_bg_pin|integration_r35_entladen|integration_elza_vollstart|integration_r35_werfer_elza|
  integration_r36_elza|integration_r36_story_elza|integration_r35_cut10f0" --timeout 900`) -> **100% tests passed, 0 tests
  failed out of 28** (u.a. integration_r37_pri1100_marke 93.6 s: "F892 mit Masken=0 ohne Masken=101 | Spawnlage (F240) mit
  Masken=0 ohne Masken=163"; integration_r37_pri1100_elza 87.2 s: "ROOM1101 Cut 1 masks=52; ... mit Masken=0 ohne
  Masken=163"; integration_r34n_e_dokumente_bild 133 s; integration_r36_story_elza 269 s; alle 12 r35_entladen; alle 5
  r36_elza-Integrationen). Kein roter Fenster-Haken -> keine Nachlaeufe noetig.
- Beruehrte Module laut Diff abgedeckt: Masken/PRI/SLD (pri_*, sld, r36_pri, maskgrp, room1100), Lader entladen_pc.c
  (r35_entladen unit + integration a..l), bg_pc.c (bss_room_source, boot_bg_pin, headless_run), Dokumente (r34n_e unit +
  Bild-Integration), main.c-Haken (jede exe-Integration), Elza in Zwilling-Raeumen (r36_elza*, r35_werfer_elza (1001),
  r35_cut10f0 (10F0/10F1), elza_vollstart, story_elza, r33_tuer1120_elza, r34n_d_adaruf_elza).

## 7. Alle 20 Zwilling-Raeume an der echten exe (eigene Laeufe `r20_*`, je Leon und Elza, `RE15_DEBUG_JUMP`, FORCE_CUT
auf den ersten Port-Masken-Cut, `RE15_EXIT_AT=60#<raum>`)
Elza laedt in JEDEM der 20 Raeume dieselbe Maskenzahl mit Atlas wie Leon und schreibt die Zwilling-Zeile; Leon nie:
1001/1000 c0 105, 1011/1010 c0 100, 1051/1050 c1 92, 1061/1060 c0 105, 10A1/10A0 c0 91, 10C1/10C0 c2 105, 10D1/10D0 c1 93,
10E1/10E0 c0 85, 10F1/10F0 c0 86, 1101/1100 c1 52, 1111/1110 c0 102, 1121/1120 c2 79, 1131/1130 c3 71, 1141/1140 c0 104,
1161/1160 c3 103, 1181/1180 c2 104, 11B1/11B0 c3 94, 11D1/11D0 c3 103, 11F1/11F0 c0 105, 1231/1230 c2 49 (alle
`fg_atlas=1`) = exakt die Tabelle des Dossiers §8.1. Gegenprobe ROOM1221 (eigene Datei): keine Zwilling-Zeile.

## 8. Neu gefunden (vorbestehend, nicht Teil von Punkt 1; betrifft Dossier-Text und den Zensus-Rahmen)
- **ROOM1221 (Elza) hat KEINE wirksamen Masken.** `MASKS/ROOM1221.MSK` (02c62294, 2026-09-03) hat keinen einzigen
  `ROOM1221_PRI%02d.TIM`-Atlas. Der Lader verlangt fuer nachgezeichnete Masken den eigenen Atlas (bg_pc.c
  `re15_pri_load_cut_atlas_ex`: `if (nachgezeichnet) { ... MASKS/ROOM%04X_PRI%02d.TIM ...; if (!buf) { set_pri_atlas(NULL);
  return 0; } }`, main.c zeichnet nur bei `has_fg && pri_n > 0`). Gemessen Elza 1221 Cut 0..9 (Laeufe `q_1221_c*`):
  `masks=21/16/16/13/25/14/22/0/4/0`, **ueberall `fg_atlas=0`**; Leon 1220 (gleiche RDT, gleiche Kamera, gleicher
  Hintergrund — selbst nachgerechnet) zeichnet Cut 0/2/4/6/8 mit 20/7/10/86/87 Masken und Atlas. Bild: Elza 1221 Cut 6 mit
  gegen ohne Masken **0 Punkte** (`y1221_*`). Elza hat in 1221 also genau den Zustand des Abnahme-0-Mangels M1 (keine
  Masken), und die Regel "eine eigene Elza-Datei gewinnt" (zwilling_zensus.py `rueckfall = msk_g and not msk_u ...`)
  verhindert den Rueckfall auf die funktionierenden Leon-Masken. Das Dossier beschreibt 1221 dagegen als "EIGENE
  Elza-Masken, die von der Leon-Fassung abweichen" (§7, §8.4) und legt dem Nutzer eine Wahlfrage vor.
- **Stages 3..6: alle 30 MSK-Dateien (Leon UND Elza) sind ohne eigenen Atlas** (`ls MASKS/ROOM3040_PRI*.TIM` usw. leer)
  und werden nie gezeichnet: ROOM3040 Leon Cut 0..7 `masks=19/5/62/36/59/60/48/41`, alle `fg_atlas=0`; Cut 2 mit gegen ohne
  Masken 0 Punkte (`x3040_*`). Das Dossier §8.4 sagt "Stages 3..6: die Elza-MSK sind bitgleich zu den Leon-MSK (nur MSK,
  Atlas aus dem BSS) — kein Befund" — "Atlas aus dem BSS" stimmt nicht (der Lader nimmt fuer R15M-Masken nie den
  BSS-Atlas). Fuer den Zwilling ohne Folge (beide Varianten gleich tot), fuer den Nutzer aber eine falsche "kein
  Befund"-Aussage. Vorbestehend seit der Atlas-Regel 2026-09-08 (bg_pc.c-Kommentar), nicht von Spur A.

## 9. Urteil
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1 — ROOM1100 (Leon), Zombie an der F9-Marke | **erfuellt** | Nachbau mit v0.8.24-/v0.8.23-Masken = Nutzer-Marke F892/F240 bildpunktgenau (0 Punkte, max 0); mit HEAD Wandfenster 0 (ohne Masken 101/164); echter Tuerweg 10D0 -> 1100: 0 (alt 25, ohne 163) |
| 1 — derselbe Raum fuer Elza (ROOM1101, Pflichtweg; Abnahme-0-Mangel M1) | **erfuellt** | Log `ROOM1101: Masken der Leon-Variante MASKS/ROOM1100.MSK`, `masks=52 fg_atlas=1` (vorher 0/0); echter Tuerweg 10D1 -> 1101 (auch ueber die echte Spielerwahl PL04): 0 Punkte (ohne Masken 163); F892-Szene 0; Cut 2 gleiche verdeckte Punktmenge wie Leon; echtes Fenster (PrintWindow) 0 |
| Abnahme-0-Mangel M2 (Nutzertext Leon an der Wand) | behoben | Dossier §4.3/§7/§8.2 berichtigt; F162 3 / F192 2 / F222 4 Punkte selbst nachgemessen |
| RE-Gate | haelt | 3 Stellen + Sprungziele selbst disassembliert (FUN_800392d4, Cut-Wechsel/0x80066c30/FUN_80053ca4, Hintergrund @0x80021c88..d20 + Tabelle 0x8007438c), Berichtigung @0x80053cdc bestaetigt; keine Rate-Woerter; Fix erklaert den Befund |
| Vertrag | haelt (Hinweis) | erlaubte Pfade, keine Bits/Ids/Slots/Ereignisse; dokumente_r34.c-Haken (1 Zeile) ausserhalb der Spur-Liste, zwingende Folge von §1.7 — bei der Integration bestaetigen |
| Tests | gruen + messend | 48/48 Unit, 28/28 Integration (gezielt); Identitaets-Lader -> `elza`/`zwilling` rot; alte Dokument-Klemme -> `unit_r34n_e_dokumente` rot |

**bestanden = true** — der Nutzerpunkt ist fuer beide Szenarien im Raum der Marke erfuellt, Gates halten. Die Maengel
unten sind nicht blockierend fuer Punkt 1 (Dossier-/Nutzertext und vorbestehende Zustaende anderer Raeume), muessen aber
vor dem Release-Text korrigiert bzw. vom Orchestrator entschieden werden.

## 10. Maengel (nicht blockierend fuer Punkt 1)
- **M1 — ROOM1221 falsch beschrieben, Elza dort ohne wirksame Masken.** Dossier §7 ("ROOM1221 und ROOM2091 haben EIGENE
  Elza-Masken, die von der Leon-Fassung abweichen ... Sollen auch dort die Leon-Masken gelten?") und §8.4: fuer 1221
  unzutreffend — `ROOM1221.MSK` hat keinen Atlas, in keinem Cut wird eine Maske gezeichnet (`fg_atlas=0` Cut 0..9, Bild 0
  Punkte, §8). Elza hat in 1221 denselben Zustand wie in 1101 vor der Nachbesserung; Kamera und Hintergrund sind gleich
  (selbst nachgerechnet), die funktionierenden Leon-Masken (1220: Cut 0/2/4/6/8 mit Atlas) blockiert nur die Regel "eigene
  Datei gewinnt". Die Wahlfrage an den Nutzer hat der Orchestrator bereits durch eine Integrations-Aufgabe ersetzt
  (master UEBERGABE.md, Eintrag 14:28: "Elza 1221/2091 bekommen die Zwillings-Masken ... veraltete Elza-Dateien entfernen
  bzw. Zwilling bevorzugen ... Framedump-Vergleich Elza gegen Leon je Cut"). Fuer diese Aufgabe wichtig: 1221 ist nicht
  nur veraltet, sondern wirkungslos — Elza bekommt dort mit dem Zwilling ERSTMALS Masken (Vergleich gegen "vorher keine
  Masken", nicht gegen "andere Masken"); 2091 ist richtig beschrieben (Cuts 0..7 mit Atlas, 8/9 fehlen). Abhilfe: Satz in
  §7/§8.4 berichtigen (bei der Integration oder im Dossier).
- **M2 — §8.4 "Stages 3..6: die Elza-MSK sind bitgleich zu den Leon-MSK (nur MSK, Atlas aus dem BSS) — kein Befund" ist
  falsch.** Alle 30 MSK der Stages 3..6 (Leon UND Elza) haben keinen eigenen Atlas und werden nie gezeichnet (ROOM3040
  Cut 0..7 `fg_atlas=0`, Cut 2 Bild 0 Punkte). Die Dateien stammen aus 02c62294 (2026-09-03), die Lader-Regel "nachgezeichnete
  Masken nur mit eigenem Atlas" (`re15_pri_load_cut_atlas_ex`) aus 53741fe5 (2026-09-08). Vorbestehend, fuer den Zwilling
  ohne Wirkung (beide Varianten gleich), nicht Teil von Punkt 1 — aber kein "kein Befund": als vorbestehender Befund
  fuehren (Folge-Kandidat fuer den Orchestrator: die Vordergrund-Masken der Stages 3..6 wirken fuer niemanden).

## 11. Beobachtungen (kein Mangel)
- B1 Vertrag: dokumente_r34.c / re15_dokumente.h (Kommentar) / test_r34n_e_dokumente.c / probe_r34n_e_dokumente.c
  liegen ausserhalb der Spur-Liste (§1.4); kleiner Haken, gemessen notwendig (Dossier 73/163, Gegenbau rot) —
  Integration bestaetigen.
- B2 Der wip-Commit 4a07f568 traegt noch das falsche Zitat "@0x80053cdc..d18 kopiert den Satz" (Historie); der Fix-Commit
  75a2b03d berichtigt es ausdruecklich, Code/Werkzeug/Tabelle/Test sind sauber.
- B3 Restband der Rechteckform (Abnahme 0 B3, Dossier §6) unveraendert: ein Figurteil weniger als 256 (C1) / 192 (C2)
  Kamera-z hinter der Wandflaeche kann noch an der Wandkante erscheinen; die Marken-Lagen (>= 293 dahinter) sind frei.
- B4 Props-Vergleich (eigene Sonde) nur fuer den Init-Zustand mit frischen Flags; spaeter angelegte Props sind nicht erfasst.
- B5 `platform/psx/main.c` nutzt den Zwilling, ist aber ungebaut (PSX-Ziel baut nicht). APK neu bauen + Android-.cxx neu
  konfigurieren (neue Quelle `pri1100_r37.c`) — Integration.

## 12. Messlaeufe (Scratchpad `ab1/`, Laeufer `run.sh`/`run2.sh`, Auswertung `py/ppm.py`, `py/marke.py`, `py/streifen.py`,
`py/zensus_eigen.py`; Sonden `probe/props_paare.c`, `probe/identitaet.c`, `probe/dokumente_alt.c`; Fenster `fenster.ps1`)
- Leon 1100: `l892_{a,b,c,alt}`, `l240_{a,b,c,v0823}`, `ltw_{a,b,c,alt}` (Tuerweg 10D0), `lc2_{a,b}`, `wb_{neu,alt,nopri}`.
- Elza 1101: `etw_{a,b,c}` (Tuerweg 10D1), `ps_{a,b,c}` (echte Spielerwahl), `e892_{a,b,c}`, `ec2_{a,b}`, `gdi_elza`
  (PrintWindow).
- Nebenwirkungen: `l1130_*`/`e1131_*`, `d{0,1,6,8}_{a,b,c,d}` (Dokument 1011), `r20_*` (20 Raeume je Leon/Elza),
  `q_122{0,1}_c*`, `x3040_*`, `y1221_*`, `x_3040_c*`.
- Messkopie `re15_port/build/platform/pc/re15_pc_ab1m.exe` nach der Abnahme geloescht.
