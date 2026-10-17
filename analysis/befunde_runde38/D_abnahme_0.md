# Runde 38 Spur D "zopf" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r38_zopf`, Zweig `r38/zopf`, Basis f587d34e, gepruefter Stand **0c8ae8f2**.
Abnehmer: unabhaengige Abnahme 0 (2026-10-17). Alles unten ist am gebauten Stand selbst gemessen; was ich vom Bau-Agenten
uebernehme, ohne es selbst zu messen, steht als "(Bau-Agent, nicht nachgemessen)" dabei. Mess-Artefakte (Bilder, Logs,
Skripte) liegen im Scratchpad
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\`
(unten `SP\`), Unterordner je Messung.

**Ergebnis: BESTANDEN.** Punkt 1 erfuellt, alle Gates halten, keine Maengel. Vier Nebenbefunde fuer Orchestrator/Integration (§6).

## 0. Stand / Bau

- `git log master..HEAD`: 11 Commits (1beb52e3 .. 0c8ae8f2), Baum sauber.
- `git diff master...HEAD --name-only` (nur Zweig): Code `engine/src/zopf_r38.c` (neu), `include/re15_zopf_r38.h` (neu);
  Haken `game_step_common.c` (+2), `room_common.c` (+2), `skeleton_common.c` (+4), `include/re15_skeleton.h` (+3),
  `platform/pc/main.c` (+13/-2); Tests `tests/unit/test_r38_zopf.c`, `tests/unit/r38_zopf_orig.inc`,
  `tests/unit/probes/r38_zopf.cmake`, `tests/integration/test_r38_zopf.cmake`; Doku `analysis/befunde_runde38/D_*`.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `LOCAL-BUILD-OK (build)`.
  exe 2026-10-17 09:03:30, juenger als jede Code-Quelle des Zweigs (zuletzt zopf_r38.c 06:28:59); `strings` enthaelt
  `RE15_ZOPF_*`. Messkopie `build/platform/pc/re15_pc_abn0d.exe` (md5 = re15_pc.exe).
- Vorher-exe: `re15_pc_r38d_basis.exe` (Bau-Agent, aus `SP\base`). Selbst geprueft: `git hash-object` der fuenf beruehrten
  Dateien in `SP\base\re15_port` = Blobs von f587d34e (alle GLEICH), Dateiliste engine/platform/include identisch,
  kein zopf_r38.c, `strings` ohne `RE15_ZOPF`, md5 der Kopie = `SP\base\...\re15_pc.exe`. Meine Kopie `re15_pc_abn0d_basis.exe`.

## 1. Gezielte Tests (keine volle Suite)

    ctest --test-dir re15_port/build -R "r38_zopf|anim_pose_order|npc_scd_pose|zombie_pose|marvin_render_pose|neck_headlook|room_transition|eintritt_pose_einmal|irons_neck_1150|elza_zweig|integration_elza_vollstart|r36_elza_fenster|r37_pri1100_elza|r37_bissarm|scd_zone2_pauseflags|text_freeze" --timeout 1200 --output-on-failure
    100% tests passed, 0 tests failed out of 35
    Total Test time (real) = 387.56 sec

Auswahl: Spur (7 Unit + integration_r38_zopf) + Gruppen der beruehrten Module: skeleton_common.c (Pose: anim_pose_order,
npc_scd_pose, zombie_pose, re2_zombie_pose_bank, marvin_render_pose, neck_headlook, irons_neck_1150, eintritt_pose_einmal),
room_common.c (unit_room_transition_cmd, integration_room_transition), game_step_common.c Freeze-Gate
(scd_zone2_pauseflags, text_freeze), main.c Spieler-Zeichenpfad Elza (integration_elza_vollstart, unit/integration_r36_elza_fenster,
unit/integration_r37_pri1100_elza, r37_bissarm_* inkl. integration_r37_bissarm_echtlauf, unit_elza_zweig). Kein roter Test,
kein Wiederholungslauf noetig. integration_r38_zopf 116,28 s, Laufordner `build/r38_zopf_wd/lauf_fbe491b3`; die Riegel-Werkzeuge
darauf geben dieselben Zahlen wie das Dossier (deterministisch): spur 216 Schritte / Drehung 2383 / Ausschlag 20,2 Grad /
6 Seitenwechsel / 52 Schritte Ruhe; pixel 102 Pixel, alle im Hinterkopf-Fenster; lauf Glied 17..20 Gehen exe 339/745/615/637
gegen Original 367/765/637/645, Rennen 667 > 339, Durchschwung -219, Ausschwingen 336 -> 80.

## 2. Punkt 1 — "Bei Elza fehlt der Zopf - schaue in Resident Evil 2 wie es da gemacht ist mit Clairs Zopf."

**Urteil: erfuellt.**

### 2.1 Original hat den Zopf (selbst gemessen, Original-RAM)
Die 8 Elza-Spielstaende des Nutzers (`%LOCALAPPDATA%\DuckStation\savestates\HASH-957757946319438E_{1..7,10}.sav`, Kopien
in `SP\sav`, gelesen mit `SP\abn_sav.py` ueber re15_ss.py): alle Stub @0x80026e4c = `0800e003` (Auslieferungs-EXE),
Charakter 0x800aca5c = 0x04, Teile player+0x83 = 21, Flags Teil 15..20 = `0x0,0x0,0x41,0x41,0x41,0x41`, Versaetze
Teil 17..20 = (-115,-346,0) (-45,-32,0) (-101,97,0) (-1,106,0) (= Init @0x80031b44-b80). VRAM-Bilder: `SP\sav\orig3_kopf.png`
(ROOM1011 von hinten: Zopf haengt vom Hinterkopf zum Kragen), `SP\sav\orig1_kopf.png` (ROOM10B1 seitlich: Pferdeschwanz mit
Haarband im Bogen).

### 2.2 Vorher/Nachher an der exe (selbst gefahren)
- **Szene ROOM1011 (Roy, Elza seitlich, echte Szene nach RE15_DEBUG_JUMP 1010@250, Pos (-150,7139) Blick 2157, Bilder 255..415/20):**
  `SP\e1011\vor` (Vorher-exe) / `SP\r1011` (Zweig-exe) / `SP\e1011\aus` (Zweig-exe + Mess-Sonde RE15_ZOPF_SONDE_AUS=1).
  Vorher gegen Zweig+Sonde: **9/9 Bilder BITGLEICH** (der Zopf aendert sonst nichts, auch nicht den Szenenablauf).
  Vorher gegen Zweig mit Zopf: 182..270 Pixel verschieden, Huellen nur am Hinterkopf (z.B. (184,54)-(198,94)).
  Bild `SP\e1011\vor_nach_1011.png` (links vorher ohne Zopf, rechts nachher: Pferdeschwanz mit Haarband, haengt im Bogen,
  bleibt beim Knien/Kopfdrehen am Kopf). Vergleich mit dem Original: `SP\abn0d_seite_original_vorher_nachher.png`
  (links ORIGINAL ROOM10B1, Mitte Port vorher, rechts Port nachher — gleiche Form: Haarband, Bogen nach unten).
- **Stehen von hinten (ROOM1121 (150,9550) Blick 2048, Bilder 30..150/30):** `SP\r1121s` / `SP\r1121s_aus`: 48..51 Pixel
  Unterschied, Huelle (213..220, 82..98) = Hinterkopf/Nacken. `SP\r1121s\orig_ohne_mit.png`: links ORIGINAL (Spielstand 4,
  ROOM1211 von hinten), Mitte Port ohne Zopf, rechts Port mit Zopf — der Haarstrang am Hinterkopf wie im Original.
- **Zopflage in Ruhe gegen das Original** (Port-Log `RE15_ZOPF_LOG`, elza_lauf Schritt 199/200, Blick 2048, Nacken 0, gegen
  Spielstand 3, Blick 2157): Gelenke relativ zur Figur Port (162,-2688,0)/(206,-2720,0)/(339,-2678,-2)/(340,-2572,-3),
  Original (146,-2665,40)/(190,-2696,34)/(323,-2655,19)/(327,-2549,32); Ruhewinkel zang Port 1/301/1/-7, Original (4
  Spielstaende) -1..9 / 285..311 / -21..13 / -23..27.
- **Gehen/Rennen:** integration_r38_zopf Teil elza_lauf (oben §1): exe gegen Original-GDB-Messung, groesster Ausschlag je
  Glied Abweichung 28/20/22/8 Einheiten (Schranke = Effekt der gleichmaessigen Port-Schrittweite, `test_r38_zopf konst`).
  Die Gleichheit der Eingaenge beim Gehen (Blick 0, Animation +-1) habe ich nicht selbst gegen das Original gemessen
  (Bau-Agent, D_belege/vergleich_exe_original.txt).
- **Echter Weg Tuer 1121 -> 1061** (`SP\tuer`, `SP\tuer2`, Eingabe `W1,U1.2,W0.3,A0.1,...`): Raumwechsel per Aktionstaste,
  Zopf-Log `R kommando0 modell=4` beim Wechsel (Haken room_common.c), Zopf danach gezeichnet und gerechnet (Schritte Z339..);
  er schwingt aus der alten Lage in 14 Schritten in die Ruhe (zang 2925 -> -965 -> ... -> 1), Raum ab Bild 5 hell. Das ist
  die Original-Mechanik: Kommando 0 schreibt +0x54..+0x5c nicht (@0x80031b38-bf0 selbst gelesen, nur +0x2c/+0x30/+0x34,
  +0x62, +0x94/+0x96, Flags).
- **Leon unveraendert** (`SP\leon`, ROOM1120 (2500,10000) Blick 2048, Eingabe `W1,U1.0,R1.0,W2` = gehen + drehen, andere
  Eingabe als der Bau-Agent, Bilder 20..200/20): Vorher-exe gegen Zweig-exe **10/10 BITGLEICH**, beide `17 meshes`.

### 2.3 RE2 / Claire (selbst disassembliert, info/re2leon/PSX.EXE, re2_disasm.py)
@0x8003befc `sltiu v0,v1,0xa` / @0x8003bf04 `andi v0,v1,0x1` / @0x8003bf10 `jal 0x8001844c` = nur ungerade Spieler-Ids < 10
(Claire). Parametersaetze @0x8001850c-614: (400,100,40,2,0,400,100,24,4,0), (350,0,48,4,**150**,150,0,24,6,0),
(300,0,48,4,0,150,0,24,6,0), (250,0,48,4,0,100,0,24,6,0) = RE1.5 bis auf p7 des 2. Glieds (RE1.5 0x12c). RE2 @0x800184d4-e8
nur `andi v0,v0,0xfff; sh` (ohne die +0x800-Regel von RE1.5 @0x80024cc8-d8). MVMVA `4a486012` an beiden Stellen
(RE1.5 @0x80024f58, RE2 @0x80018838, Bytes `12 60 48 4a`). Die Lesart des Bau-Agenten (RE1.5 hat dieselbe Routine, Port folgt
RE1.5 wegen Elzas Netz, RE2 = Gegenbeleg) ist damit belegt; der Nutzer-Hinweis "schaue in RE2" ist erfuellt und dokumentiert.

## 3. RE-Gate

Stichproben selbst disassembliert (info/Re1.5/PSX.EXE, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`):

| Zitat im Code | selbst gelesen |
|---|---|
| Freeze-Gate @0x80031c78 | `bltz a0,0x80031da8` (a0 = `lw g_pauseflags` @0x80031c54); alle Pfade ab 0x80031c80 laufen nach 0x80031d78 |
| Aufruf @0x80031d84-98 | `lbu v0,0x800aca5c` / `andi v0,v0,0x4` / `beq` / `jal 0x80024c30` mit a0 = s0 = 0x800aca54 |
| Init @0x80031a78-bf0 | `addiu v0,v0,-4; sltiu v0,v0,0x2` (4,5), `bne v1,6`; Teil-15-Flag `sw zero,2580(v1)`, Teil-16 `sw zero,2752(v0)`; Versaetze -115/-346, -45/-32, -101/0x61, -1/0x6a an +0xb98..+0xda0; z/v94/v96/rot.vy = 0; `ori 0x40` an +0xb6c/+0xc18/+0xcc4/+0xd70 |
| Treiber @0x80024c30-e3c | RotMatrix @0x80024c68, zwei FUN_80022da0, lhu-Summe +0x6a/+0x62/+0x5c2, `andi 0xfff`, `addiu -1501; sltiu 0x3e7` (+0x800), Bit 0 von 0x800acc0c, vier `jal 0x80024e40` mit genau den Saetzen der Port-Tabelle (alle 4 Aufrufe Wert fuer Wert verglichen) |
| Glied FUN_80024e40 | Aufruffolge RotMatrixYXZ @0x80024ee4, RotMatrixY @0x80024ef0, MVMVA x2, ApplyMatrix @0x8002504c, RotMatrixY @0x800250e0, ApplyMatrix @0x80025130, FUN_80025360 @0x80025140, RotMatrixZ @0x8002519c, FUN_800253fc @0x800251c4, rsin @0x800251ec/@0x80025278, div, `andi 0xfff`/`andi 0x800`, RotMatrixYXZ @0x80025308, RotMatrixY @0x80025324 |
| libgte-Namen | ghidra1_V2.txt-Labels: catan 0x800658fc, RotMatrixY 0x800659d0, RotMatrixZ 0x80065b70, ApplyMatrix 0x800661c0, RotMatrixYXZ 0x800666d0 (XREF FUN_80024e40:80024ee4/80025308), RotMatrix 0x80068098, rsin 0x800683e8 |
| Negativ-Zweig | RotMatrixY @0x800659d8 `bgez t7` / @0x800659e0 `subu t7,zero,t7`; Tabelle DAT_800794c4 selbst gelesen: sin[1]=6, sin[4094]=-6, **sin[4095]=0**, sin[2047]=sin[2048]=0 -> nicht antisymmetrisch, Zweig beobachtbar |
| Winkelhelfer | FUN_80025360: `sltiu`/`sll 11`/`addiu 3072` (0xc00/0x1400), `bgez s0` sonst +2048; FUN_800253fc: `ori 0x400` |
| Zeichner | FUN_8001e9ec @0x8001ea10 `andi 0x40` (Kette ueberspringen), @0x8001ecc4 `andi v0,v1,0x1` (nur Bit 0 wird gezeichnet) |

**Unabhaengige Gegenprobe der Port-libgte gegen Original-RAM** (nicht die GDB-Daten des Bau-Agenten): `SP\harness\harness.c`
gegen `libre15_engine.a` gelinkt, Daten `SP\harness\sav_glieder.py` aus 10 Original-Spielstaenden (8 Elza-Spielstaende +
2 Spielerwahl-Staende). (1) Weltmatrix +0x40 = Port-`re15_zopf_r38_rotmatrix_yxz((rot.vx,0,rot.vz))` + `_rotmatrix_y(rot.vy)`
aus rot +0x60; (2) Lage +0x54 = (ApplyMatrix(Elter+0x40, Versatz gepackt `<<4/<<20`) >> 4) + Elter.t:
`SUMME Schritt7 40/40 bitgleich, Schritt3 40/40 bitgleich` (Spielstand 5 hat rot.vy = -4199 -> Negativ-Zweig dabei).
Die GDB-Daten `r38_zopf_orig.inc` (unit_r38_zopf_original, 235/235) sind nach Augenschein echt: Kette geschlossen
(F0 "nach" = F1 "vor"), s16-Saettigung -4097, Elter-Zeiger 0x80072d4c im Log.

Raten-Woerter im Code-Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|heuristi`): **keine**.
Jede Konstante in zopf_r38.c traegt @0x... (Stichproben oben). Env-Haken: `RE15_ZOPF_LOG` (nur Log) und
`RE15_ZOPF_SONDE_AUS` (MESS-SONDE fuer den Pixel-Riegel, im Code als solche gekennzeichnet, kein Spielverhalten).
**Fix erklaert den Befund:** Vorher zeichnete main.c genau `skel.bone_count` = 15 Teile (Teile 17..20 nie), selbst gesehen
im Vorher-Bild ohne Zopf; nachher Teile 17..20 mit der Weltmatrix aus FUN_80024e40 -> Zopf sichtbar, sonst bitgleich.

## 4. Vertrag

- Bank-9-Bits / Nachrichten-Ids / Items / Dokumente: keine belegt (Diff beruehrt keine Flag-/Text-Tabellen).
- Pfade: kein `release/`, kein `platform/android/`, kein `shared_assets/PSX/`, keine Test-CMakeLists (Registrierung ueber
  `tests/unit/probes/r38_zopf.cmake`, GLOB @tests/unit/CMakeLists.txt:4060).
- Gemeinsame Dateien nur kleine Haken mit "Runde 38 Spur D": game_step_common.c 2 Zeilen, room_common.c je 1 Zeile an zwei
  Stellen, skeleton_common.c 4 Zeilen (Fangpuffer, Standard NULL — nicht in VERTRAG 1.4 gelistet, aber klein und wirkungslos
  ohne Zopf: Elza-Szene 9/9 bitgleich mit Sonde), re15_skeleton.h 3, main.c 3 Stellen (Include; 4 Zeilen nach der Pose;
  Zeichenschleife 1 Kommentar + n_zopf + Schleifengrenze + Pose-Index + 4-Zeilen-Block).
- Riegel je Punkt vorhanden und messend (8 Tests), selbst gefahren gruen (§1).
- Nebenwirkungen: Leon bitgleich (§2.2), Elza ohne Zopfglieder bitgleich (§2.2); Portabilitaet: M_PI-Ersatz im Test,
  `cmake_minimum_required` + `file(COPY_FILE)` wie test_r34n_b_cursor.cmake; fopen/getenv in Engine-Dateien ist Bestand
  (31/47 Dateien).
- Merge (selbst mit `git merge-tree`): gegen master konfliktfrei; gegen r38/integration (078b62af) genau ein Konflikt in
  `platform/pc/main.c`, nur die Include-Zeilen (E/I/T gegen zopf) -> alle behalten.

## 5. Maengel

Keine.

## 6. Nebenbefunde (kein Mangel, zur Entscheidung/Integration)

1. **Spielerwahl-Bildschirm (Original rechnet den Zopf auch dort, Port nicht).** TITLE.BIN @0x80101c54-6c: fuer Modell 1
   (Elza, 0x800ace20) `jal 0x80024c30` vor dem Zeichner `jal 0x8001e8c8` (selbst disassembliert, `--bin TITLE.BIN`; Suche
   nach `jal 0x80024c30` in allen Binaerdateien: nur TITLE.BIN 0x1c60 und PSX.EXE @0x80031d98). Original-RAM
   `stage_saves/mzd_after_flow.sav` und `sub_newgame.sav`: Elza-Modell +0x83 = 21, Flags 17..20 = 0x41, Pendelzustand
   (Harness oben: bitgleich). Port: `pselect_render_model` zeichnet nur `skel.bone_count` Teile (`SP\psel\ser`, kein Zopf).
   ABER: im Original ist der Koerperwinkel dort s0 = 7180 (selbst nachgerechnet: 1028 + 8 + 4056 + Teil-8-+0x94 = 2088, weil
   0x800acc0c Bit 0 = 0), also ~183 Grad gedreht — der Zopf liegt im Kopf. Silhouetten-Render der Original-RAM
   (`SP\harness\render_ram.c`, Kamera 0x800b5288 = diag(4104,4096,4104) t (0,0,20039), H 1000; an Leon gegen das echte
   Original-Bild deckungsgleich, `SP\harness\pselect_check.png`): nur 12 (z-Puffer) bzw. 6 (Malerreihenfolge) Zopf-Pixel
   sichtbar, als duenne Linie am Scheitel (`SP\harness\pselect_elza_orig_zb_ot_port.png`). Im Original-Auswahlbild ist also
   ebenfalls kein Pferdeschwanz zu sehen; byte-genau fehlen dem Port dort diese wenigen Pixel. Kein Mangel am Wortlaut;
   wer den Auswahlbildschirm byte-genau will, muss den Titel-Aufruf mit Teil-8-+0x94 = 2088 nachbauen.
2. **Raumwechsel:** der Zopf schwingt nach jeder Tuer ~14 Bilder aus der alten Weltlage nach (Original-Mechanik, §2.2;
   am Original selbst nicht gemessen). Sichtbar ab Bild 5 nach dem Eintritt (Einblendung), beim Blick von vorn kaum.
3. **Android-Paket:** neue Quelldatei `engine/src/zopf_r38.c` — der Android-Bau friert die GLOB-Liste ein
   (app/.cxx-Cache); beim Release-Bau den Configure erneuern, sonst fehlen die Zopf-Symbole beim Linken.
4. **Huelle der Spur P (N5 des Bau-Agenten) bestaetigt als Integrationsthema:** der Zopf steht seitlich ueber den
   Figurkasten +-350 hinaus (Bau-Agent 404/485/809; nicht nachgemessen). Gehoert in die Integration mit r37_masken36.

Nebenbefunde N1 (Leon Teil 15), N2 (Port-RotMatrix ohne Negativ-Zweig), N3 (Gehschritt 74), N4 (Nacken 1121), N6 (PL07)
des Bau-Agenten: nicht beauftragt, nicht nachgeprueft.
