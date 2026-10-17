# Abnahme 1 — Spur P masken36 (Runde 37, 2026-10-14)

Unabhaengige Abnahme nach Nachbesserung 1. Baum `.claude/worktrees/r37_masken36` (Zweig r37/masken36, Basis bddd6b31,
HEAD fb4be428). Gemessen nur am selbst gebauten Stand (`local_build.sh configure` + `build` -> "ninja: no work to do",
`=== LOCAL-BUILD-OK (build)`) mit eigener exe-Kopie `re15_pc_abnP1_*.exe` und eigenem Scratchpad `abnP1/`.

## Stand der Abnahme (fortlaufend)

- [x] Pflichtlektuere: AUFTRAG.md (Orchestrator-Auftrag P), VERTRAG.md, Dossier P_masken36.md inkl. "Nachbesserung 1",
      P_abnahme_0.md, Code-Diff bddd6b31..HEAD und 25a79ae3..HEAD
- [x] Bau
- [x] gezielte Tests: 44/44 gruen (§2)
- [x] RE-Gate (§3)
- [x] Messung an der exe: M1 auf dem echten Tuerweg (§4.1), Stage 4 (§4.2), Stage 1/2 (§4.3), Cut 15 (§4.4),
      Zufallswege 30 Raeume / 570 Bilder (§4.5), Je-Cut-Stichprobe (§4.6)
- [x] Vertrag (§5)
- [x] Urteil (§6), Maengel (§7): **NICHT bestanden** (M1 Cut-15-Cursor, M2 Saumpunkte Kopfoberkante)

Scratchpad dieser Abnahme (nicht im Baum): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/
c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abnP1/` (lauf.sh, sh.py, rw_*.py, runs/, rw/, Bilder).

## 1. Code-Diff (gelesen)

Seit Abnahme 0 (25a79ae3) aendert sich am LAUFZEIT-Code nur die Tabelle `engine/src/gen/masken36_ausschluss_r37.inc`
(85 -> 124 Zeilen); `bg_pc.c`, `masken36_r37.c`, `re15_masken36_r37.h`, `platform/pc/main.c`, `platform/psx/main.c` sind
bitgleich zu 7495dc73 (`git diff 25a79ae3 -- re15_port/engine re15_port/platform re15_port/include` zeigt nur die .inc).
Der Rest ist der Generator/Riegel `tests/unit/test_r37_masken36.c` (+517 Zeilen):
- `m36_tueren_statisch`: jeder 0x3B-Satz jeder RDT (main-/sub-Sektionen linear mit `scd_opcode_size_at`), Laenge 32/40 nach
  pc[3]&0x80, Nutzlast pc+14/pc+22, Ziel = Stage nl[8]+1, Raum nl[9], Variante des Quellraums; Ankunft (0,0,0) zaehlt nicht.
- `raum_laden`: Flutfuellung startet an den Saaten von `raum` UND `raum ^ 1` (Raumpaar), laeuft aber in der Kollision der
  eigenen RDT.
- `form_drin`/`in_zelle`/`form0`: Nicht-Rechteck-Zellen (type != 1) werden ueber `re15_collision_constrain_contact_band`
  (Port von FUN_8003b0a4, Radius 450, Maske 1) gefragt statt als Kasten; Sichtstrecke gegen die Form im Raster 25.
- Riegel `ausschluss`: neue CHECKs "ohne Stehplatz == belegte Liste {3050/15, 3051/15}", "bytegleiche Paare gleiche Zeilen",
  "jedes Paar gleiche Zeilen"; `verdeckung`: FREI verlangt "ungefiltert verdeckt > 0"; Szenen h4040c4/h4080c1/v4040c7
  ersetzt durch h4080c5/v4080c5, neu f30A1c0. Integration Stage 3 faehrt den M1-Weg (Leon + Elza), Stage 4 = ROOM4080 Cut 5.
- Werkzeug-Modi saaten/soll/proj/fehl/cut (kein Riegel). Timeout der drei Unit-Riegel 300 -> 900 s.

Tabelle unabhaengig nachgezaehlt (Python ueber die .inc): **124 Zeilen, 1224 ausgeschlossene Rechtecke**; alle 15 Raumpaare
xxx0/xxx1 der Stages 3..6 mit MSK haben identische Zeilen (30A0/30A1 je 4 Zeilen, 5020/5021 je 5, 4040/4041 je 7,
4080/4081 je 7; 40A0/40A1 keine). RDT-bytegleich sind 3040, 3050, 30A0, 4040, 4080, 5050, 5100, 6010 (8 Paare).

## 2. Gezielte Tests

`ctest --test-dir re15_port/build -R "r37_masken36|r37_pri1100|_pri_|pri_hashes|pri_masken|r35_entladen|sld_atlas|maskgrp|boot_bg_pin|bss_chunk|bss_room_source" --timeout 900 --output-on-failure -j3`
-> **100% tests passed, 0 tests failed out of 44** (Log Scratchpad `abnP1/ctest1.log`): integration_r37_masken36_stage3
305.8 s, stage4/5/6 je ~128.7 s, unit_r37_masken36_ausschluss 87.2 s, zensus 1.2 s, verdeckung 0.02 s, alle
unit/integration_r37_pri1100_*, integration_pri_masken, integration_boot_bg_pin, unit/integration_r36_pri_*, unit_pri_*,
unit_sld_atlas, unit_r34n_g_maskgrp, unit/integration_r35_entladen_* (a..l, inkl. der Integrationslaeufe, die der
Bau-Agent nicht gefahren hat), prop_bss_chunk, unit_bss_room_source.

## 3. RE-Gate (selbst disassembliert, `re15_disasm.py`, info/Re1.5/PSX.EXE)

| zitiert | gefunden | haelt |
|---|---|---|
| Satzlaenge 0x3B @0x80040618/@0x80040630 | Sprungtabelle @0x800744a8 Eintrag [59]=0x3B -> 0x800405bc (Handler). Darin `80040600 lw v0,28(a0)` / `80040608 addiu v0,v0,2` / `8004060c sw v0,0(v1)` (Slot-Zeiger = pc+2), `80040618 lbu v0,3(v1)`, `80040620 andi v0,v0,0x80`, `80040624 beq`, `80040630 addiu v0,v1,40` (Delay-Slot des `j`), `80040634 addiu v0,v1,32`, `80040638 sw v0,28(a0)` | ja |
| Nutzlast @0x80042f90 / @0x80042fb8 | `80042f60 lbu v0,1(s0)` (s0 = Slot-Zeiger = pc+2 -> pc[3]), `80042f68 andi v0,v0,0x80`, `80042f8c jalr v0` + `80042f90 addiu a0,s0,20` (= pc+22), `80042fb4 jalr v0` + `80042fb8 addiu a0,s0,12` (= pc+14) | ja |
| Warp liest Stage/Raum @0x8001d94c/@0x8001d960 (Zeiger @0x8001d874, gesetzt @0x800430c4) | `8001d874 lw a0,-13912(a0)` (0x800ac9a8), `800430c4 sw a0,-13912(at)`, `8001d930 lbu v1,10(a0)` -> `sh v1,4068(at)` (Cut 0x800b0fe4), `8001d94c lbu v0,9(a0)` -> `8001d95c sh v0,4066(at)` (Raum 0x800b0fe2), `8001d960 lbu v0,8(a0)` (Stage, Vergleich mit 0x800b0fe0) | ja |
| Hintergrund ohne Variante @0x80021c90/@0x80021ca8 | `80021c88 lh a3,4068(a3)` (Cut), `80021c90 lh a1,4066(a1)` (Raum 0x800b0fe2), `80021ca8 lh v0,4064(v0)` (Stage), `80021cbc..cc` Tabelle 0x8007438c[Stage], `80021cd4 lhu v0,0(a1)` | ja |
| Spielerradius 450 = DAT_80073e94[6] | `read 0x80073e94` s16: [0, -1530, 0, **450**, 1530, 450, ...] -> Byte-Offset +6 = 450 (vorbestehend `re15_collision.c #define PR 450`) | ja (Byte-Offset) |

RDT-Bytes (shared_assets/PSX/STAGE3, Python): ROOM3091 +0x1398 `3b 02 02 31 00 00 be c9 7a db e6 05 3a 02 | 64 9c 00 00 b4 2d
00 00 02 0a 00` (Slot 2, Rechteck (-13890,-9350) 1510x570, Ankunft (-25500,0,11700), Stage-Index 2, Raum 0x0A) = ROOM3090
+0x1390 bytegleich; +0x1492 `21 04 10 00` Ck(4,16)=0; +0x149A `21 04 08 00`; +0x14B2 `46 02 01 31 08 00 ff ff` Aot_reset
Slot 2 sce 1; +0x16D0 `22 04 10 01` Set(4,16); +0x1D7C `46 02 02 31 64 9c 00 00 b4 2d` Aot_reset Slot 2 sce 2. ROOM3050
+0x2596 `29 0f` Cut_chg 15, +0x1FB4 `2c 0a 03 31 .. d4 9a fc bd e8 03 d0 07 ff 00 18 0a` (Slot 10, (-25900,-16900)
1000x2000, Ereignis 0x0a). ROOM3050/3051 und 30A0/30A1 RDT bytegleich, 3090/3091 nicht. Alle zitierten Stellen stimmen.

Rate-Woerter im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO): siehe §5.

## 4. Messung an der exe

Werkzeug: `abnP1/lauf.sh <exe> <name> <raum> <bild> ENV...` (exe-Kopie `re15_pc_abnP1_<quelle>.exe` neben re15_pc.exe,
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_FRAMEDUMP=<bild>:f.ppm RE15_EXIT_AT=<bild>#<raum>`), Zaehlung mit EIGENEM Skript `abnP1/sh.py` (gleiche Definition wie
probe_r37_masken36_ppm: S = |B-C| > 24, H = davon |A-B| > 24; ohne Fenster = ganzes Bild). A = mit Masken, B = `RE15_NO_PRI=1`,
C = Figur ausserhalb des Regionsvierecks (Zwangs-Cut). exe "neu" = re15_pc.exe (HEAD fb4be428, gebaut), exe "alt" =
re15_pc_p37final2.exe (Stand P vor NB1; erkennbar an `masks=46` fuer ROOM30A1 Cut 0).

### 4.1 M1 Elza ROOM30A1 — ECHTER Weg durch die Tuer ROOM3091 -> ROOM30A1 (neu in dieser Abnahme)

Die Tuer der Abnahme 0 war nur "ab der Tuer-Ankunft" gelaufen. Jetzt durch die Tuer selbst: Elza
(`RE15_TITLE_SHOT_CHAR=1`), `RE15_SET_FLAG=4:16` (der Zustand nach sub07, in dem Slot 2 von ROOM3091 wieder Tuer ist,
+0x16D0/+0x1D7C), `RE15_DEBUG_JUMP=3090@250` (-> ROOM3091), `RE15_PLAYER_POS=-13135,-9065,3072` (Mitte des Tuer-AOT Slot 2),
`RE15_PRESS=square@60,square@62`, danach `RE15_INPUT_SCRIPT=L0.233,U1.07` ab Tick 600, Bild 420 in ROOM30A1.
Facings 0/1024/2048 loesen nicht aus, 3072 schon.

Log (neu): `[room] PC loaded room3091.rdt` -> `[tuer] Sequenz Archiv 2 DOOR06 ... 311 Bilder` -> `[room] PC loaded
room30a1.rdt` -> `[pri] ROOM30A1 Cut 0: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)` ->
`[pri] cut=0 pri_offset=0x3C0 masks=21 fg_atlas=1` -> `[walk] F420 cut=0 ... pl pos=(-24284,0,13657) rot=-672` (Kamera
selbst auf Cut 0, Lage = die der Abnahme 0). C = Zwangs-Cut 0 bei (-19800,19763).

| exe | Log | S | H (uebermalt) |
|---|---|---|---|
| neu (Tuerweg) | `masks=21 fg_atlas=1` | 464 | **0** |
| alt (Tuerweg) | `masks=46 fg_atlas=1` | 464 | 265 |
| neu (Weg der Abnahme 0: Ankunft + U1.07) | `masks=21`, `[walk] F240 cut=0 pl pos=(-24284,0,13657)` | 464 | **0** (ganzes Bild ebenso) |
| alt (Weg der Abnahme 0) | `masks=46` | 464 | 265 |

Bild `abnP1/dw_montage.png` (alt | neu | ohne Masken) und `abnP1/dw_zoom.png` (Ausschnitt 4-fach, + C): alt bleibt von
Elza nur der Unterschenkel unter der Trennwand, neu steht sie ganz sichtbar VOR der linken Kante der Trennwand (Schatten vor
dem Sockel), bitgleich zu "ohne Masken" im Figurfenster. **M1 behoben.**

### 4.2 Stage 4 (neue Zellform): ROOM4040 Cut 4, alt gegen neu (Leon, Zwangs-Cut 4, Bild 120, C = (-26400,17150))

| Stelle | alt: Log / S / H | neu: Log / S / H |
|---|---|---|
| (-22000,-3722) neben dem Wandblock (Stelle des Bau-Agenten) | `masks=7` / 397 / 57 | `masks=2` / 397 / **0** |
| (-23200,-922) HINTER dem Wandblock (alte Riegel-Szene h4040c4) | `masks=7` / 208 / 146 | `masks=2` / 208 / **19** |

Bilder `abnP1/s4_montage.png`, `abnP1/s4a_zoom.png`, `abnP1/s4h_zoom.png` (je alt | neu | ohne Masken | ohne Figur):
- (-22000,-3722): alt schneidet eine senkrechte Kante durch Leons Ruecken, waehrend Fuesse und Schatten vor dem Sockel des
  Wandblocks stehen -> Fehlverdeckung, neu = ohne Masken. Die Aussage des Bau-Agenten stimmt.
- (-23200,-922): alt ist Leon hinter dem Wandblock bis auf einen schmalen Streifen verdeckt (richtig), neu steht er fast ganz
  sichtbar VOR dem Wandblock gezeichnet, obwohl er dahinter steht (19 von 208 Punkten verdeckt). Das ist die vom Bau-Agenten
  offengelegte Folge (NB1.6 OFFEN 2: "ROOM4040 Cut 4: 5 von 7 Rechtecken aus ... weniger Wandblock-Verdeckung im EAST
  PASSAGE"): dasselbe Rechteck verdeckt hinten richtig und vorne falsch und faellt ganz weg. Zustand dort = vor P (keine
  Verdeckung), keine Fehlverdeckung. Im Nutzertext §7 steht es ("verdecken die Wandbloecke weniger als vorher").

Riegel-Werte (LastTest.log meines Laufs): Stage 4 ROOM4080 Cut 5 HINTER Leon 879/883, Elza 621/627, DAVOR 0/8374 bzw.
0/6165; Stage 3 HINTER 1123/1234 (Elza 1158/1260), DAVOR 0/684 (0/1067), FREI 0/883 (0/997), WEG M1 Elza 0/464, Leon 0/602;
Stage 5 ROOM5020 Cut 9 HINTER 199/322 (Elza 224/309), DAVOR 0/5007 (0/5050); Stage 6 ROOM6010 Cut 12 HINTER 820/1440 (Elza
639/1105), DAVOR 0/9489 (0/7931). Alle `fg_atlas=1`.

### 4.3 Stage 1/2 (eigener Atlas, Spur A) unveraendert

exe neu gegen vorher-exe `re15_pc_p37vorher.exe` (Basis bddd6b31; enthaelt `k_r37a_zwilling`, nicht die Spur-P-Logzeile),
gleicher Lauf, Framedump Bild 120, `cmp`:

| Lauf | Framedump | Log neu = vorher | Spur-P-Zeile neu |
|---|---|---|---|
| Leon ROOM1100 C1 (-14914,-25645) rot 4488 (F9-Marke Spur A) | **bitgleich** | `cut=1 pri_offset=0x60C masks=52 fg_atlas=1` | 0 |
| Leon ROOM1220 C0 | **bitgleich** | `cut=0 pri_offset=0x4DC masks=20 fg_atlas=1` | 0 |
| Leon ROOM2090 Sprung-Spawn C0 | **bitgleich** | `cut=0 pri_offset=0x670 masks=56 fg_atlas=1` | 0 |
| Elza ROOM1101 C1 (Zwilling -> 1100) | **bitgleich** | `cut=1 ... masks=52 fg_atlas=1` | 0 |
| Elza ROOM2091 C0 | verschieden | neu `masks=56 fg_atlas=1` (Zwilling 2091 -> 2090), vorher `masks=0 fg_atlas=0` | 0 |

ROOM2091: Unterschied kommt aus eab5d03c (= be2dbd26 der Integration, `git patch-id --stable` beider Commits identisch
ec4073ed...: ROOM2091.MSK/_PRI*.TIM geloescht, Zwilling 0x2091 -> 0x2090) — nicht Arbeit der Spur P; die vorher-exe liest im
Spurbaum die geloeschte Datei nicht mehr (Asset-Stand gemischt). Kein P-Befund.

## 5. Vertrag und Rate-Woerter (statisch)

- Pfade (`git diff bddd6b31 --name-only`): analysis/ (Dossier, Abnahme 0, alle_cuts.txt), engine/src/masken36_r37.c +
  gen/masken36_ausschluss_r37.inc (neu), include/re15_masken36_r37.h (neu), platform/pc/src/bg_pc.c (Lader, VERTRAG §1.4
  erlaubt P genau das), platform/pc/main.c + platform/psx/main.c (je 2 Zeilen, Kommentar "Runde 37 Spur P"),
  tests/unit/probes/r37_masken36.cmake, tests/integration/test_r37_masken36_verdeckung.cmake (`cmake_minimum_required(VERSION
  3.16)` vorhanden), tests/unit/*r37_masken36*; dazu die Uebernahme eab5d03c = be2dbd26 der Integration (`git patch-id
  --stable` gleich: MASKS-Loeschungen 1221/2091, pri_masken_zwilling_r37.inc, test_r37_pri1100.c, zwilling_zensus.py).
  Keine release/, platform/android/, keine Test-CMakeLists, keine neuen/umgeschriebenen MASKS-Dateien durch P.
- Keine Bank-9-Bits, Nachrichten-Ids, Ereignisse, Slots. Keine neuen Env-Schalter im Diff (grep `getenv("RE15_` in den
  +Zeilen: 0). Kein popen in den neuen Tests.
- Rate-Woerter in den +Zeilen von re15_port (deferred|tunable|interim|for now|faithful|plausib|TODO|vorlaeufig): **0**.
- Konstanten: die neuen Generator-Werte (Raster 25, Satzlaengen, Nutzlast-Offsets) sind Messaufbau bzw. tragen @0x (s. §3);
  die Tabelle selbst ist Port-Massnahme auf NUTZER-VORGABE-Masken (R15M) mit Begruendung im Kopf von masken36_r37.c, nicht
  als Original verkauft.
- Zusammenfuehrung: `git merge-tree --write-tree r37/integration HEAD` -> Konflikte nur in AUFTRAG.md/VERTRAG.md
  (Orchestrator-Dateien), Code (main.c) automatisch.
- M3: Dossier §7 sagt jetzt "Seit v0.7.19 (2026-09-08, Commit 53741fe5) ... die Masken selbst gibt es seit v0.3.96";
  nachgeprueft `git tag --contains 53741fe5 | sort -V | head -1` = v0.7.19, `... b8db7b2b ...` = v0.3.96. **M3 behoben.**

### 4.4 ROOM3050 Cut 15 ("ohne Stehplatz, belegt") an der exe: die Masken uebermalen den Cursor der Kran-Konsole

Der Riegel fuehrt ROOM3050/3051 Cut 15 als einzige Sektionen ohne Stehplatz mit der Begruendung "es gibt keine Figur, die
eine Maske dieses Cuts verdecken koennte" (test_r37_masken36.c OHNE_STEHPLATZ_BELEGT, Dossier NB1.2). Nachgemessen:
Cut 15 ist die Nahaufnahme des Konsolen-Monitors mit dem beweglichen Ziel-Cursor (sub10 +0x2596 `Cut_chg 15`, danach
sub11..14 `Work_set 2e 03 03` + `Speed_set` = Objekt 3 wird gesteuert). Aufbau: `RE15_SET_FLAG=3:178` (main00 +0x1F90
`Ck(3,0xb2)=0` -> sonst +0x1FB4 Aot_set Slot 10 sce 3 Ereignis 0x0a), `RE15_DEBUG_JUMP=3050@250
RE15_PLAYER_POS=-24757,-15900,3072 RE15_FORCE_EVENT=10@300 RE15_PRESS=square@420..962 (alle 60 Bilder)`, A mit Masken, B
`RE15_NO_PRI=1`, Bild 900.

Log A: `[scd F783] Cut_chg(15)` -> `[pri] ROOM3050 Cut 15: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37
Spur P)` -> `[pri] cut=15 pri_offset=0x84C masks=36 fg_atlas=1`. A gegen B: 138 Punkte > 24, davon **60 Punkte = der
dunkelgruene Innenrand des Cursors** (B (0,21,0)/(2,60,0), A Monitorbild), der Rest Paletten-Rundung des Blits (<= ~30 je
Kanal, ohne Gegenstand). Bild `abnP1/k15_cursor.png` (A | B, 8-fach): mit den Masken fehlt den gruenen Winkeln des
Cursors die dunkle Innenkante, der Cursor wird teilweise vom Monitor-"Vordergrund" ueberdeckt, obwohl er VOR dem
Bildschirm liegt; vor P (keine Masken) und im Original (keine Masken-Records fuer diesen Cut) ist er vollstaendig.
`abnP1/k15_m3.png` zeigt den ganzen Cut. -> Mangel M1 (s. §7).

Gegenprobe Elza und vorher-exe (gleicher Aufbau, Bild 900): Elza ROOM3051 (`RE15_TITLE_SHOT_CHAR=1`) A/B ebenso 138 Punkte,
davon 60 Cursor-Rand; vorher-exe (bddd6b31, `cut=15 ... masks=36 fg_atlas=0`) gegen neu ohne Masken: **0** Punkte
verschieden -> der Schaden am Cursor kommt mit Spur P.

### 4.5 Stichprobe ohne Modell: Zufallswege durch alle 30 Generator-Raeume (Leon xxx0 und Elza xxx1)

Um die Fehlverdeckung unabhaengig vom Erreichbarkeitsmodell des Bau-Agenten zu pruefen: je Raum ein Zufallsweg (Seed 37014,
`abnP1/rw_gen.py`, Folge aus Drehen L/R 0.1-0.7 s + Gehen 0.6-2.2 s, ~10.5 s) ab dem Sprung-Spawn (`RE15_DEBUG_JUMP=<xxx0>@250`,
Elza mit `RE15_TITLE_SHOT_CHAR=1`, `RE15_INPUT_SCRIPT_START=520`), natuerliche Kamera, Bildserie
`RE15_FRAMEDUMP=260-620/20:f`, einmal mit Masken (A), einmal `RE15_NO_PRI=1` (B); Laeufe deterministisch (alle
`[walk]`-Lagen A == B in allen 30 Raeumen). 570 Bildpaare, 60 Laeufe rc=0 (`abnP1/rw_batch.log`). Ausgewertet je Raum und
Cut: Punkte mit |A-B| > 24, die NICHT in mindestens der Haelfte der Bilder dieses Cuts stehen (= nicht der statische
Blit-Rest, sondern etwas, das sich mit der Figur bewegt; `abnP1/rw_eval2.py`), danach jeder Fund angesehen:

| Raum Cut | Bilder | Fund | Urteil |
|---|---|---|---|
| 3040/41, 30E0/E1, 4040/41, 4080/81, 40A0, 5090/91, 6000, 6030/31, 3051, 30A1, 5021 | alle | keine bewegten Punkte | - |
| 30A0 C0, Leon ~(-25400,13550) | F380 / F400 | **4 / 7 Punkte** an der Kopfoberkante (y 77-78), Leon S = 328 / 355 (C = m1_e_C) | Leon steht mit Koerper und Fuessen VOR dem linken Pfosten der Trennwand (Koerper unverdeckt), nur das Haar oben wird vom Rahmen-Balken abgeschnitten -> Saumpunkte, Bild `abnP1/rwz_30A0_400_kopf.png` |
| 3050 C0, Leon (-22593,1555) | F380-F460 | 6-12 Punkte an Leons Hand an der Oberkante des roten Kanisters | Tiefe uneindeutig (Beine ueber dem Kanister, Fuesse knapp dahinter), `abnP1/rwz_3050_380.png` |
| 5051 C0, Elza | F320 / F560 | 8-9 Punkte Stiefelspitze/Schatten am blauen Pfosten | uneindeutig, `abnP1/rwz_5051_320.png` |
| 6010/6011 C0/C1, 40A1, 5050 C1 | einzelne | Blit-Rest am Gitter bzw. ohne Figur | keine Figur betroffen (`rwz_6010_320.png`, `rwz_40A1_500.png`) |
| 5020/21 C0, 50B0/B1, 5100, 6001 | Cut-Wechsel | statischer Blit-Rest (falscher Cut zugeordnet), z.B. 5020 C0 linker Pfeiler 608 Punkte | Palette gegen MDEC, kein P-Befund (Vergleich Original-RDT-Masken: ROOM3000 C0 1472, 3010 C0 3561, 3080 C0 181, 3070 C0 22 Punkte A/B) |

Folgerung: auf 570 natuerlichen Bildern keine grossflaechige Fehlverdeckung (wie M1: 265 Punkte); als Rest Saumpunkte.
Der Fall 30A0 C0 erklaert sich aus dem Modell: die Figur wird in 9 Zeilen abgetastet, oberste Zeile bei
FIGUR_HOCH/9*(8+0.5) = 2833 von 3000; `test_r37_masken36 proj 30A0 0 -25400 13550` -> FENSTER 161 73 186 145, Kopfoberkante
y = 77 = genau die uebermalten Zeilen 77-78 -> das Band 2833..3000 (Haar) wird nie gefragt. Der Dossier-/Nutzertext nennt
als Reste nur ROOM3040 Cut 1 und ROOM5090 Cut 14 (-> Mangel M2).

### 4.6 Je-Cut-Liste an der exe (Stichprobe)

`RE15_FORCE_CUT`, Bild 40, je Logzeile + Spur-P-Zeile; Soll unabhaengig aus der .inc (Maskenzahl - Bits) nachgerechnet:
ROOM5021 Cut 6 `masks=22` (55-33), ROOM4081 Cut 2 `masks=5` (15-10), ROOM6001 Cut 9 `masks=2` (18-16), ROOM30A1 Cut 3
`masks=42` (56-14), alle `fg_atlas=1` — gleich der Liste `P_masken36_alle_cuts.txt`.

## 6. Urteil je Punkt (gegen den Wortlaut)

| # | Punkt | Urteil | Beleg |
|---|---|---|---|
| 1a | Masken der Stages 3..6 verdecken die Figuren wieder (BSS-Atlas) | erfuellt | Lader unveraendert seit Abnahme 0 (dort an der exe belegt); Riegel-Laeufe meines ctest: HINTER je Stage 879/883, 1123/1234, 199/322, 820/1440 (Elza 621/627, 1158/1260, 224/309, 639/1105), alle `fg_atlas=1`; Stichprobe 4 Cuts = Liste. Verdeckung in Stage 4 (ROOM4040 Cut 4..7) gegenueber Abnahme 0 bewusst verringert (h4040c4 146 -> 19 von 208, §4.2) — offengelegt, Zustand dort = vor P, keine Fehlverdeckung |
| 1b | Raeume mit eigenem Atlas (Stage 1/2, Spur A) unveraendert | erfuellt | §4.3: 1100/1220/2090/1101 bitgleich zur vorher-exe, 0 Spur-P-Zeilen; 2091 = Integrations-Commit |
| 1c | Figuren VOR einem Objekt werden nicht verdeckt | erfuellt fuer Figuren bis auf Saumpunkte | M1 der Abnahme 0 behoben, jetzt auch auf dem echten Tuerweg ROOM3091 -> ROOM30A1 (Elza 265 -> 0 von 464, §4.1); ROOM4040 C4 (-22000,-3722) 57 -> 0; Zufallswege 570 Bilder ohne grossflaechige Fehlverdeckung (§4.5); Rest: Saumpunkte (30A0 C0 Kopfoberkante 4-7 Punkte, M2) |
| 1 | Punkt 1 gesamt | **teilweise** | 1a/1b/1c fuer Figuren gehalten, aber die wieder gezeichneten Generator-Masken uebermalen in ROOM3050/3051 Cut 15 den Cursor der Kran-Konsole (60 Punkte, neu durch P, §4.4) — dieselbe Fehlerklasse "falsche Maske uebermalt, was davor liegt" in genau dem Cut, den der Riegel als "nichts kann verdeckt werden" fuehrt (M1) |
| 2 | Bezug: Verdeckung fehlte in Stages 3..6 unbemerkt seit ... | erfuellt | M3 der Abnahme 0 behoben: Dossier §7 "seit v0.7.19 (2026-09-08, 53741fe5) ... Masken seit v0.3.96", per `git tag --contains` nachgeprueft |

Gates: RE-Gate haelt (§3: fuenf zitierte Stellen selbst disassembliert, Sprungtabelle 0x3B -> 0x800405bc, RDT-Bytes
stimmen; 0 Rate-Woerter). Vertrag haelt (§5). Tests 44/44 gruen (§2). Nebenwirkung: Cut 15 (M1).

## 7. Maengel

**M1 (Nebenwirkung, Punkt 1): ROOM3050/3051 Cut 15 — die 36 Generator-Masken uebermalen den Konsolen-Cursor; die "belegte
Liste ohne Stehplatz" ruht auf einer falschen Begruendung.** test_r37_masken36.c `OHNE_STEHPLATZ_BELEGT` und Dossier NB1.2
sagen fuer Cut 15 "es gibt keine Figur, die eine Maske dieses Cuts verdecken koennte" und lassen alle 36 Rechtecke an.
Cut 15 ist aber die Monitor-Nahaufnahme, in der der Spieler den Ziel-Cursor steuert (sub10 +0x2596 Cut_chg 15, sub11..14
Work_set Objekt 3 + Speed_set); die Masken legen das Monitorbild ueber den dunkelgruenen Innenrand des Cursors (60 Punkte,
Leon und Elza gleich), vorher-exe 0 Punkte. Nachpruefbar: `RE15_SET_FLAG=3:178 RE15_DEBUG_JUMP=3050@250
RE15_PLAYER_POS=-24757,-15900,3072 RE15_FORCE_EVENT=10@300 RE15_PRESS=square@420,square@422,...(alle 60 Bilder bis 962)`,
Bild 900, mit/ohne `RE15_NO_PRI=1` -> Log `[scd F783] Cut_chg(15)`, `[pri] cut=15 pri_offset=0x84C masks=36 fg_atlas=1`;
A/B 138 Punkte > 24, davon 60 mit B = (0,21,0)/(2,60,0); Bild `abnP1/k15_cursor.png`. Weg (Vorschlag): Cut 15 wie Cut 6
ganz ausschliessen (im Original hat der Cut keine Masken-Records; was er zeigt, liegt VOR dem Monitor) bzw. die Liste
nur mit einem exe-Beleg "nichts Gezeichnetes im Regionsviereck/Cut" fuehren; Riegel um diesen Cut erweitern.

**M2 (Punkt 1c / Dossier): Saumpunkte nicht nur an den zwei genannten Stellen — Kopfoberkante wird vom Modell nie
abgetastet.** Auf einem Zufallsweg in ROOM30A0 Cut 0 (Leon ~(-25400,13550), Bild 380/400 des Laufs `rw/30A0_A`) schneidet der
obere Rahmen-Balken der Buero-Trennwand Leon 4 bzw. 7 Punkte vom Haar ab, obwohl Koerper und Fuesse VOR dem Trennwand-Pfosten
stehen und unverdeckt sind (`abnP1/rwz_30A0_400_kopf.png`); ROOM30A1 hat dieselben Zeilen. Ursache im Generator/Riegel: 9 x 9
Figurpunkte, oberste Zeile bei 2833 von FIGUR_HOCH 3000 — das Band 2833..3000 wird nie geprueft (Projektionsfenster
`proj 30A0 0 -25400 13550` oben y = 77 = die uebermalten Zeilen). Dossier §7/NB1.6 nennen als Reste nur ROOM3040 Cut 1 und
ROOM5090 Cut 14. Weg (Vorschlag): Abtastung bis zur Kopfoberkante (Zeile bei 3000 bzw. Rand der Figur) und Tabelle neu,
oder den Rest im Dossier/Nutzertext allgemein benennen ("an Maskenkanten in Kopfhoehe moeglich", mit 30A0 C0).

Hinweise (keine Maengel): Stage-4-Verdeckung verringert (h4040c4 146 -> 19, offengelegt in NB1.6/§7); Dossier §4.2 fuehrt
h4040c4/v4040c7 noch als Riegel-Zeilen ("Die Zeilen 1..8 laufen als Riegel"), seit NB1 ist Stage 4 = ROOM4080 Cut 5;
uneindeutige Kleinstfunde 3050 C0 (Kanister, 6-12 Punkte) und 5051 C0 (Stiefelspitze, 8-9 Punkte); statischer Blit-Rest
5020 C0 (608 Punkte, Palette gegen MDEC) liegt im Bereich der Original-RDT-Masken (22..3561 Punkte).

**Ergebnis: NICHT bestanden** (Punkt 1 teilweise: M1; dazu M2). M1, M2, M3 der Abnahme 0 sind behoben.
