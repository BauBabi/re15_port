# Spur M masken36 — Abnahme 0 (unabhaengig, 2026-10-24)

Baum `.claude/worktrees/r41_masken36` (Zweig r41/masken36, merge-base master 4c667c97, Kopf 95b9f38d, 16 Commits
6962dd32..95b9f38d + Orchestrator-Doku b28c7236). Pruefer: Abnahme 0. Massstab: Wortlaut AUFTRAG.md, VERTRAG.md §2,
CLAUDE.md RE-Gate. Bericht wird fortlaufend geschrieben (Sitzungslimit).
Messordner (Scratchpad): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/m41abn0/`
(`run.sh` Szenen-Laeufer, `cmp.py` Bildvergleich, `runs/`, `bilder/`). Messkopien der exe:
`exe/re15_pc_m41abn0_neu.exe` (Spurbaum, md5 71643eff...) und `exe/re15_pc_m41abn0_vor.exe` (Hauptbaum-exe = master
7b59e297-Stand mit Spur A, 09:57; MASKS dort = merge-base, `git diff 4c667c97 master -- MASKS masken36_r37.c
masken36_ausschluss_r37.inc bg_pc.c pri_common.c render_pc.c` leer, `git status` MASKS sauber).

## Stand
- [x] Lektuere AUFTRAG/VERTRAG/Dossier, Diff gelesen (13 Nicht-Bild-Dateien, 250 MASKS-Dateien, 275 M_bilder)
- [x] Bau: `local_build.sh configure` OK, `build` "ninja: no work to do" (exe 10:46:29 juenger als die letzte
      erzeugte Quelle masken36_ausschluss_r37.inc 10:46:19; git status sauber)
- [x] Gezielte Tests 63/63 gruen
- [x] Punkt 1 an der exe gemessen (vorher/nachher, 9 Szenen) + Bau reproduziert (246/246 bytegleich)
- [x] Punkt 2 an der exe gemessen (Kurbel Sprung + echter Weg, 20A0 C12, 13 VOR-Stellen, 3 Elza-Szenen)
- [x] RE-Gate (3 Adressen selbst disassembliert, s.u.)
- [x] Vertrag (Pfade, Ressourcen)
- [x] Urteil: **nicht bestanden** (Punkt 1 teilweise, Punkt 2 teilweise; Gates halten)

## RE-Gate (selbst disassembliert / gelesen)
1. **SLD-Entpacker FUN_800c47e8** (DEBUG.BIN, geladen @0x800c0000; Aufrufer `jal FUN_800c47e8` @0x80021db8 in
   FUN_80021bbc, ghidra1_V2.txt Z. 101831; DEBUG.BIN shared_assets == info/Re1.5 bytegleich). Eigene Disasm
   (dis_one aus re15_disasm.py ueber DEBUG.BIN):
   `800c4868 03 55 02 00 sra t2,v0,20` (12-Bit-Versatz, vorzeichenbehaftet), langer Fall:
   `800c4890 80 18 0a 00 sll v1,t2,2` / `800c4894 25 10 43 00 or v0,v0,v1` / `800c4898 00 14 02 00 sll v0,v0,16` /
   `800c489c 2a 12 03 08 j 0x800c48a8` / `800c48a0 03 54 02 00 sra t2,v0,16` = signext16((t2<<2)|(b3>>6)).
   Zitat im Diff (original.py) stimmt Byte fuer Byte. Der alte Python-Code verschob einen schon negativen Wert
   (-8192..-1, Bit 15 in Python gesetzt) nochmals um -0x10000 -> JEDER lange Rueckverweis falsch. Fix
   `& 0xFFFF` vor dem Vorzeichen = Original. **erfuellt.**
2. **NULL-Sektion ROOM2020 Cut 3**: Kamerasatz RDT+0x24 -> 0x60, Cut 3 = P(10456,-6184,956) T(-15424,3868,-3056),
   +28 = 0x3FC, Bytes `ff ff ff ff` (shared_assets == info/Re1.5 bytegleich). FUN_800392d4: `80039324 lw v1,0(t5)` /
   `80039328 addiu v0,zero,-1` / `8003932c bne v1,v0,...` / `80039334 j 0x8003955c` / `80039338 sb zero,0(a0)`
   = 0 Masken. Zitat stimmt. **erfuellt.**
3. **Pos_set ROOM2020 @0x0A8C**: RDT-Bytes `32 00 42 0e f8 f8 93 f4` = Opcode 0x32, (3650,-1800,-2925), danach
   `34 04 00 04` @0x0A94. Zitat stimmt. **erfuellt.**
   (Nebenbei gelesen: @0x8002565c `sra v1,v1,4`, @0x80066c70 `addiu t0,zero,341` + ctc2 ZSF3, @0x80039650 `lh a0,2(s3)`
   - Altbestand R37 fuer re15_pri_mask_occludes, nur im verworfenen Weg 1.5 zitiert, nicht im Endstand-Code.)
- Diff auf "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|geschaetzt" durchsucht: 0 Treffer.
  Kein neuer getenv in engine/ oder platform/.
- Konstanten des Werkzeugs schliessen_r41.py (NCC 0.97, Block 16/Stride 4, DEPTH_SAFETY 0.90, Stufen) sind die des
  Generators vom 2026-09-03 (Port-Masken, Original hat fuer diese Cuts NULL-Sektionen), im Kopf als "NICHT neu gewaehlt,
  nur uebernommen" gekennzeichnet; MAX_MASKEN 105 = RE15_PRI_MAX_MASKS_PER_CUT. Akzeptiert (Port-Wahl-Bestand).

## Statische Gegenproben (eigene Skripte, `inc_check.py`)
- Ausschluss-Tabelle merge-base (126 Zeilen) gegen neu (148 Zeilen) ueber die alten MSK-Zahlen: 216 Sektionen,
  alte Rechtecke 6478 (= 3239 x 2), neu 15078, Saum-Rechtecke 8600, davon ausgeschlossen 1662, **Abweichungen der
  alten Bits: 0** (Behauptung "alte Ausschluesse bitgleich" bestaetigt).
- ROOM20A0.MSK: nur Cut 12 geaendert (101 -> 88 Rechtecke), Cuts 2/4/5/8/9/10 bytegleich; 20A1 nutzt 20A0
  (gen/pri_masken_zwilling_r37.inc `{ 0x20A1, 0x20A0 }`).

## Gezielte Tests (eigener Lauf, Spurbaum)
`ctest --test-dir re15_port/build -R "r41_masken36|masken36|_pri|pri_|sld_atlas|bss|itemorte|optik|dokumente|r34n_g|r34n_b_cursor|r19|r22_10f0|stage6_hintergrund|rolltor20a0|zgirl_5090|elza5091|elza50b1|boot_bg_pin|r30_cut_blitz" -j 4 --timeout 900 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 63`** (Protokoll Scratchpad `m41abn0/ctest1.log`; darin die 3 Spur-Riegel,
alle r37_masken36 (3 unit + 5 integration), alle pri-/sld-/bss-Riegel, alle Riegel der beruehrten Raeume 20A0
(rolltor20a0 x5), 5090/5091, 50B1, Stage 6). Spur-Riegel einzeln mit -V: `unit_r41_masken36_loecher` Sektionen 216,
Rechtecke 15078, eigener Atlas 216, Kerben VORHER 61672 / NACHHER 15498 (25,1 %), Blit max Mittel 10,60;
`unit_r41_masken36_kurbel` Sichtlinie kreuzt SCA-Zelle 11 x -400..13100 z -2550..-2050, +0x3FC = FF FF FF FF,
Beinachse 31/101 verdeckt; `integration_r41_masken36_stage2` Passed (421 s).
Pfade: probes/*.cmake per GLOB (tests/unit/CMakeLists.txt:4060) - keine CMakeLists geaendert. Leon/Elza-Paare
Stage 3..6: 15 MSK-Paare und 108 TIM-Paare bytegleich (cmp).

## Punkt 1 — STAGE3-6-Masken "ueberall Loecher" -> "schliessen"

### 1a Herkunft der Loecher (unabhaengig nachgerechnet, `idx0_check.py`)
Index-0-Texel in den Quellflaechen ALLER alten Rechtecke (Leon-Dateien Stage 3..6, MSK der merge-base):
- alter Python-Entpacker (original.py der merge-base): **140 174 von 812 026 = 17,26 %** (+1 Cut "Entpacken
  fehlgeschlagen"), schlimmste 4080 C8 59,8 %, 5050 C2 55,7 %, 3050 C1 50,5 % (= R40-Messung "17,3 % / 3050 C1 51 %");
- berichtigter Entpacker: **0 von 1 125 178 = 0,00 %**.
Die Loecher der Uebersichtsbilder `pri/STAGE3-6/*_port.png` (a8be31a7) sind damit ein Werkzeugfehler; im Spiel waren
die Rechtecke voll deckend. Bildbeleg `bilder/uebersicht_alt_neu.png` (oben pri/ alt: gesprenkelt, unten
M_bilder/uebersicht: geschlossen). **Behauptung bestaetigt.** ABER: die vom Nutzer angesehenen Bilder unter
`pri/STAGE3-6/` sind unveraendert falsch (Spur darf pri/ nicht schreiben) - ohne Ersatz sieht der Nutzer weiter Loecher.

### 1b Im Spiel (exe, Framedump Bild 120, RE15_WINDOW_SCALE=1, A = mit / B = RE15_NO_PRI, C = probe_bg_dump)
VORHER = Hauptbaum-exe (MASKS der merge-base), NACHHER = Spurbaum-exe; "ohne"-Bilder vorher/nachher je Szene
pixelgleich (0 px), d.h. nur die Masken unterscheiden sich. Figurpunkte = |B-C|>24, verdeckt = Figur & |A-B|>24:
| Szene (Leon, Band 0) | verdeckt VORHER | NACHHER | Bild |
|---|---|---|---|
| h3050_02 ROOM3050 C2 (-22000,-20668) | 1158 | 1268 (+110) | `bilder/p1_h3050_02.png` |
| h3040_02 ROOM3040 C2 (-22900,-10500) | 1109 | 1218 (+109) | `bilder/p1_h3040_02.png` |
| h3040_13 ROOM3040 C13 (-5700,-13500) | 622 | 695 (+73) | `bilder/p1_h3040_13.png` |
| h6000_07 ROOM6000 C7 (19833,-18384) | 1190 | 1487 (+297) | `bilder/p1_h6000_07.png` |
| h6010_12 ROOM6010 C12 (-18512,-7920) | 775 | 801 (+26) | `bilder/p1_h6010_12.png` |
| h5090_13 ROOM5090 C13 (24417,-26365) | 0 | 0 | - |
Sichtbar: die schmalen Spalten/Kerben an den Objektkanten, durch die Leon vorher schien (3050 C2 rechte Kante,
6010 C12 zwei waagerechte Striche im Pfeiler, 3040 C2 Kante), sind NACHHER zu. Keine Stelle, an der vorher verdeckt
und nachher sichtbar ("rot") waere.
DAVOR (Leon vor dem Objekt): v4080_07 445 -> 454 (Tuermodell-Oberkante unter dem Tuerrahmen, nicht Leon), v4040_11
0 -> 1, **v5100_07 25 -> 27 (Leons Hand am Bildrand, alt + 2 neue Punkte)** (`bilder/p1_v5100_07.png`).
Offen bleibt (auch im Spiel sichtbar): Objekte, deren Rechtecke der R37-Ausschluss nimmt, bleiben ganz oder in Stuecken
ungezeichnet (z.B. 3050 C5: die braune Maschine links ohne Maske, 3050 C7: Gelaender ohne Maske; Bilder
M_bilder/ROOM3050_C05/C07), und 1662 der 8600 Saum-Rechtecke (Kerben 25,1 % von vorher).

### 1c Reproduktion des Baus (unabhaengig, `repro_wrap.py`)
Das committete Werkzeug `tools/maske/schliessen_r41.py` auf die MSK der merge-base (Stage 3..6) angewandt, mit EIGENEN
probe_bg_dump-Hintergruenden (`m41abn0/bg`) und frisch berechneter Region (eigener Cache), Laufzeit 6 min:
alle internen Proben bestanden (Rechteck in der Region, Tiefe = Generator-Regel, Deckung == alt + Region: fehlt 0 /
zuviel 0, alter Sektionsteil bytegleich); 15 Raeume, 108 Cuts, alt 3239 + neu 4300 Rechtecke, Saum 107 118 px,
Stufe 0 in 87 Cuts. **Ergebnis 246 Dateien (30 MSK + 216 TIM), alle bytegleich zu `shared_assets/PSX/MASKS`.**
Die Masken-Dateien sind damit genau das, was das Werkzeug aus dem Altbestand erzeugt (deterministisch).

## Punkt 2 — 53 Nutzer-Masken 11A0 + STAGE2 "alle sauber eingebaut" (Sichtpruefung im Spiel)

### 2a ROOM2020 Cut 3 an der Kurbel (Orchestrator-Hinweis H2)
- Sprung: `run.sh neu k2020_03 2020 3 3650 -2925 1 mit|ohne` -> Log `[pri] cut=3 pri_offset=0x3FC masks=104 fg_atlas=1`,
  `Player=(3650,-1800,-3018)`; Vollbild `bilder/k2020_03_voll.png`: Kamera haengt ueber dem Kanal, Leon steht auf dem
  LINKEN Laufsteg, die linke Reling (03_01) liegt zwischen Kamera und Leon. Eigene Projektion (VR/VT aus dem Log,
  H 207): Reling-Masken an Leons Beinen Tiefe 88..104 (Schluessel vz 5701..6726), Leon vz ~7800 -> Reling davor.
  **Leon steht HINTER der Reling, die Maske ist richtig** (Behauptung des Bau-Agenten bestaetigt, Orchestrator-
  Annahme "VOR der Reling" widerlegt). Vorher (Hauptbaum-exe) == nachher (ohne-Bilder 0 px verschieden, gleiche 1322 px).
- **Echter Weg** (Aktionstaste wie Abnahme E: `RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72
  RE15_PLAYER_POS=2600,-2800,1024,1 RE15_EQUIP=1 RE15_PRESS=square@330,380,420,460`, Bilder 440..700/20):
  F480..F580 Cut 3 (Kurbel), F600..F660 Cut 4 (keine Masken), F680 Cut 3; in Cut 3 je ~800 px Maskenwirkung = Reling
  ueber Leons Beinen + rechte Reling ueber der hellen Wolke (`bilder/kurbelweg_bogen.png`). Gleiches Bild wie im Sprung.
- Elza (2021 C3, RE15_TITLE_SHOT_CHAR=1): Pfosten + Handlauf ueber den Beinen, Zwischenraeume frei (Freistellung 03_01
  hat 15159 durchsichtige Punkte im Kasten), 519 der 826 Punkte innerhalb 03_01, Rest = 03_02 (`bilder/e_k2021_03_beine.png`).
- Rechte Reling 03_02 ueber der hellen Wolke (OFFEN 2 des Bau-Agenten): Maskentiefe dort 146 (Schluessel vz 9417), die
  Reling liegt an der Stelle bei vz ~8600..9100 (eigene Projektion z 1750); die Wolke wird nur gedeckt, weil ihre
  Dreieckstiefe > 9417 ist (Engine-Vergleich) -> sie liegt hinter der Reling. Kein Fehlerbild.

### 2b ROOM20A0 Cut 12, Freistellung 12_02 (Fix des Bau-Agenten)
- VORHER (Hauptbaum-exe, 101 Masken): Leon an der Wand (14000,-18900) 626 px Maskenwirkung, im Leon-Fenster 129; AUF
  dem Streifen (14000,-19300) 722 / 238 - **Fuss und Schatten weg** (`bilder/vor_s20A0_12.png`). NACHHER (88 Masken):
  1 px / 1 px; Elza 20A1 C12 auf dem Streifen: 1 px. Der gemeldete Fehler ist behoben.
- **Aber die Begruendung stimmt nicht:** die "Ziegelwand" hinter 12_02 ist der ROLLADEN = 3D-Objekt obj 1
  (R38 `W_rolltor20a0.md`: Typ 0, x 7942..14422, z -20510..-20420, faehrt per Schalter auf -10800 hoch;
  Hintergrund `bg/ROOM20A12.ppm` zeigt dahinter eine gruen gekachelte Oeffnung, `bilder/20A0_C12_bg_vs_spiel.png`).
  Hinter der Schwelle (Zelle 32 x 8000..14700 z -21000..-20500) liegt ein Schacht: die Kiste faellt dort auf Band 2
  (y -3600) und Band 1 (R38). Die Freistellung 12_02 (Bild `bilder/20A0_C12_freistellungen.png`) ist der Boden VOR
  dieser Schwelle - also eine Kante zu einer Tiefe, nicht "eine Maske auf der Bodenebene, die nichts verdecken kann".
  Eigene Projektion Cut 12: Schwelle vorn vz 9633 (x 8000) .. 11092 (x 14700), hinten 10068..11527; Leon auf dem
  Streifen vz 9198..9895; ein Objekt im Schacht (12250,-3600,-21473) bei (157,178) vz 13028. Mit der Zellen-Tiefe der
  Schwelle (wie 12_01 "Tiefe aus der Zelle") waere 12_02 baubar, ohne eine Figur auf dem Streifen zu uebermalen,
  und wuerde Inhalte des Schachts hinter der Kante verdecken. Statt dessen wurde die Nutzer-Freistellung aus dem Bau
  genommen (`auswahl.json` "_r41_nicht_gebaut") und der Nutzer gefragt - ohne die Rolladen-/Schacht-Lage zu nennen.

### 2c Stichprobe VOR-Stellen (Leon/Elza vor der Freistellung, `run.sh` + `ana.py`, Bilder `bilder/neu_v*.png`)
| Stelle | Maskenwirkung | Befund |
|---|---|---|
| 2030 C0 (400,1450) B3 | 94 | Spinne hinter der Reling; Leon nur am Bildrand |
| 2030 C7 (0,-1950) B3 | 475 | nur Spinnen hinter der Reling; **Leon gar nicht im Bild** (Projektion sy 928) - VOR-Pruefung leer |
| **2030 C15 (4400,3850) B3** | 327 | Leon VOR der Reling: **12 Leon-Punkte** an Schulter/Arm-Kante uebermalt (Handlauf), Rest Spinne (`bilder/v2030_15_zoom.png`) |
| **2031 C15 Elza** | 380 | **24 Elza-Punkte** an Haar/Schulter-Kante uebermalt (`bilder/e_v2031_15.png`) |
| 2050 C1 (8900,2400) B3 | 1326 | Spinne + Kisten hinter/unter der Reling; Leons Kopf (vorn) frei |
| 2050 C2 / C3 | 315 / 406 | Spinne hinter der Reling; Leon frei |
| 2050 C12 (-1500,-4200) B0 | 1321 | Kiste hinter dem Pfeiler; Leon (Bildrand) frei |
| 2080 C4 (-8700,-18100) B8 | 512 | Rohr/Objekt links; Leon frei |
| 20A0 C5 (23600,-9700) B0 | 551 | Gitter/Wandobjekte hinten; Leons Kopf (vorn) frei |
| 2040 C7 (-17300,-8800) B3 | 113 | Objekt hinter dem Gelaender; Leon nicht im Bild |
Ergebnis: 2030/2031 C15 ist ein sichtbarer Rest-Fehler einer Nutzer-Maske (Bau-Agent: "bis 8 Punkte, Bucket-Rundung",
OFFEN 3, gemessen Leon 12 / Elza 24), nicht behoben. Alle anderen Stichproben richtig.

## Vertrag (VERTRAG.md §1.3/§2)
- `git diff master...HEAD --name-only` (538 Dateien): 13 Nicht-Bild-Dateien (AUFTRAG.md = Orchestrator-Commit b28c7236,
  M_masken36.md, analysis/esp_masken_2026-09-03/auswahl.json, engine/src/gen/masken36_ausschluss_r37.inc (erzeugte
  Tabelle der Masken-Domaene), tests: test_r37_masken36.c, test_r37_masken36_verdeckung.cmake, test_r41_masken36.c,
  test_r41_masken36_stage2.cmake, test_r41_masken36_szenen.cmake, probes/r41_masken36.cmake; tools: original.py,
  schliessen_r41.py, bilder_r41.py), 250 Dateien unter `shared_assets/PSX/MASKS/` (fuer M erlaubt), 275 Bilder M_bilder.
  **Keine** Aenderung unter release/, platform/android/, pri/, synchro/, sonstigem shared_assets/PSX/, keine
  CMakeLists. Kein Engine-/Plattform-Code ausser der erzeugten Tabelle, kein neuer getenv.
- Ressourcen: keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignisse belegt (M hat keine Zuteilung) - eingehalten.
- Nutzer-Dateien unter pri/ unveraendert; 12_02 nur in auswahl.json als "_r41_nicht_gebaut" gefuehrt.
- Riegel je Punkt vorhanden und messend (Punkt 1: unit_r41_masken36_loecher mit Gegenprobe VORHER > 10000; Punkt 2:
  unit_r41_masken36_kurbel + integration_r41_masken36_stage2 mit Gegenprobe Basisstand 50 an 20A0 C12).
  Parallel-/Linux-Fest: eigene Laufordner + exe-Kopie `re15_pc_r41m2_<id>`, `m` gelinkt, cmake_minimum_required, keine
  .exe-Pfade (LAST_EXT). Schwach: die Szenen erben Umgebungsvariablen des Aufrufers (wie die R37-Riegel), Elza ueber
  RE15_TITLE_SHOT_CHAR=1 statt RE15_PSELECT_AUTO (Raum xxx1 im Log geprueft) - kein Mangel.
- Geaenderte Alt-Riegel (test_r37_masken36.c / _verdeckung.cmake): Erwartung "Stages 3..6 mit BSS-Atlas" -> "mit eigenem
  Atlas", Schranke NB2 M2 jetzt am Endzustand mit Tabelle gezaehlt (Assertion `gekappt_offen == 0` bleibt), Gegenprobe
  Trennschaerfe nur noch fuer Freistellungen. Begruendet im Code (Runde-41-Kommentare); fachlich nachvollziehbar.
- PSX: platform/psx liest MASKS/*.MSK gar nicht (grep: nur entladen_pc.c), also keine PSX-Regression durch die
  Saum-Rechtecke (Dossier-OFFEN 4 ist gegenstandslos, nicht falsch).

## Urteile je Punkt (gegen den Wortlaut)

| Punkt | Urteil | Kern |
|---|---|---|
| 1 "Masken STAGE3-6 ... ueberall Loecher" -> "Masken schliessen" | **teilweise** | Werkzeugfehler als Ursache der Bild-Loecher belegt und behoben (17,26 % -> 0,00 %), im Spiel Kerben 61672 -> 15498 und sichtbar geschlossene Kantenspalten; aber 25,1 % der Kerben, 1662/8600 Saum-Rechtecke und die R37-ausgeschlossenen Objektteile bleiben offen; die Nutzer-Bilder pri/STAGE3-6/*_port.png zeigen weiter die falschen Loecher |
| 2 "neue PRIs fuer STAGE2 ... alle sauber eingebaut" (Sichtpruefung) | **teilweise** | Kurbel 2020 C3 richtig (Leon/Elza hinter der Reling, auch auf dem echten Weg); 20A0 C12 Fussverlust behoben, aber durch Herausnehmen der Nutzer-Freistellung 12_02 mit falscher Begruendung; 2030/2031 C15 uebermalt Leon (12) und Elza (24) vor der Reling, offen |

## Maengel (nummeriert, nachpruefbar)
1. **P1 nicht vollstaendig geschlossen.** `unit_r41_masken36_loecher -V`: NACHHER 15 498 offene Kerben (25,1 % von
   61 672); 1 662 der 8 600 Saum-Rechtecke ausgeschlossen (eigene Zaehlung `inc_check.py`); Objekte mit
   R37-Ausschluss bleiben ganz/teilweise ohne Maske (Dossier OFFEN 1: 94 Objekte / 358 358 px; z.B. M_bilder/
   ROOM3050_C05.png braune Maschine, ROOM3050_C07.png Gelaender). Der Commit-Titel "STAGE3-6-Masken geschlossen"
   ueberzeichnet. Weg: Tiefe je Objekt aus der Kollisionszelle/Geometrie (wie raum.py "zelle" fuer 12_01), nicht aus
   der Erreichbarkeit; Restliste mit Bildern dem Nutzer vorlegen, falls Freistellungen gebraucht werden.
2. **P2 ROOM20A0 Cut 12, 12_02 aus dem Bau genommen statt sauber eingebaut.** Die Begruendung ("Ziegelwand",
   "Maske auf der Bodenebene verdeckt nichts") trifft nicht zu: dahinter steht der Rolladen obj 1 (R38
   W_rolltor20a0.md), hinter der Schwelle Zelle 32 (x 8000..14700 z -21000..-20500, Band 4) liegt ein Schacht (Kiste
   faellt auf Band 2/1). Eigene Projektion Cut 12: Leon auf dem Streifen vz 9198..9895 < Schwelle 9633..11527 -> mit
   der Zellen-Tiefe der Schwelle wuerde 12_02 keine Figur auf dem Streifen uebermalen. Auflage: 12_02 mit belegter
   Schwellen-Tiefe bauen und an der exe messen (Leon/Elza auf dem Streifen, auch dicht vor der Schwelle z.B.
   (14000,-20400), 0 uebermalt) - oder dem Nutzer die richtige Lage (Bodenkante vor Rolladen/Schacht) mit Bild vorlegen.
3. **P2 ROOM2030/2031 Cut 15 (4400,3850) Band 3: Nutzer-Maske uebermalt die Spielfigur VOR der Reling** - Leon 12
   Punkte (Schulter/Arm-Kante, `bilder/v2030_15_zoom.png`), Elza 24 Punkte (Haar/Schulter, `bilder/e_v2031_15.png`).
   Nicht behoben (Dossier OFFEN 3, dort "bis 8 Punkte"). Auflage: Tiefe des Handlaufs in C15 korrigieren und die Stelle
   (Leon + Elza) als VOR-Szene mit H == 0 in test_r41_masken36_szenen.cmake aufnehmen.
4. **P2 Sichtpruefung teils ohne Spielfigur:** VOR-Stellen 2030 C7 (0,-1950) und 2040 C7 (-17300,-8800) zeigen Leon
   gar nicht (Log-Projektion sy 928 bzw. kein Leon im Bild, `bilder/neu_v2030_07.png`, `neu_v2040_07.png`); die
   "Figurpunkte" dort sind Spinnen/Objekte. Die Aussage "42 von 44 Stellen mit sichtbarer Figur" zaehlt Gegner mit -
   fuer diese Cuts fehlt der VOR-Nachweis mit Leon/Elza (Stelle mit Spielfigur im Bild waehlen oder "keine begehbare
   VOR-Stelle mit Figur im Bild" belegen).
5. **Doku/Nebenbefunde:** (a) original.py-Kommentar "116 von 116 Stage-3..6-Atlanten falsch" widerspricht Dossier 1.1
   ("0 von 359", Stage 3..6 = 77+33+86+26 = 222); (b) Kopf von schliessen_r41.py beschreibt noch den verworfenen Weg
   ("Laufzeit-Tabelle engine/src/masken36_r41.c, erzeugt von test_r37_masken36 erzeuge_tiefe" - beides existiert nicht);
   (c) M_bilder/P1_exe_*.png zeigen den verworfenen Zwischenstand (nur im Dossier vermerkt) - entfernen oder umbenennen;
   (d) Dossier 1.4 "DAVOR keine neue Uebermalung von Figuren": gemessen ROOM5100 C7 (-12030,-5300) Leons Hand am Bildrand
   25 -> 27 Punkte (+2 durch Saum-Rechtecke, Bildpunkte (38,236),(39,237)), 4040 C11 +1.

## Auflagen fuer die Integration (kein Mangel der Spur, Vertrag §1.3)
- `pri/STAGE3-6/<raum>/<cc>_port.png` (a8be31a7, Grundlage der Nutzer-Aussage) durch
  `analysis/befunde_runde41/M_bilder/uebersicht/STAGEn/<raum>/<cc>_port.png` ersetzen (Elza-Ordner bytegleich), sonst
  sieht der Nutzer weiter Werkzeug-Loecher.
- Paket: 216 neue `MASKS/ROOMxxxx_PRIcc.TIM` (je 66 080 Byte, ~14 MB) muessen mit ins Paket.

