# Runde 41 — Spur E "kleinkram_b" — Abnahme 4 (unabhaengig, nach Nachbesserung 4)

Pruefer: Abnahme-Agent 4, 2026-10-24 (Beginn 20:34 laut `date`). Baum `.claude/worktrees/r41_kleinkram_b`, Zweig
`r41/kleinkram_b`, HEAD d2d4e794 (merge-base master 116e8b0f). `<sp>` = Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`;
eigene Laeufe unter `<sp>/E41a4/` (Helfer `<sp>/E41a4/run.sh`: exe-Kopie, eigener Laufordner, geerbte RE15_* geloescht).

Status dieses Berichts: IN ARBEIT (fortlaufend gespeichert).

## 0. Gelesen / Bau
- AUFTRAG.md (Wortlaut; fuer Spur E: Hinweise 03:21, E-N1/E-N2 04:52, E-E1..E-E3 10:50, **E-E4 17:32**, **Nachtrag 14 17:36**;
  master-Stand 3da82407 hat danach nur Eintraege fuer C/F/G/H), VERTRAG.md, Dossier E_kleinkram_b.md (ganz, Schwerpunkt
  "Nachbesserung 4"), E_abnahme_3.md (Maengel 1-2, H1-H6).
- `git log --oneline master..HEAD`: NB4 = 5057b8d6 .. d2d4e794 (11 Commits) auf 6e2c62b1/3c02068f (AUFTRAG vom Orchestrator).
  `git diff master...HEAD --stat` 36 Dateien; NB4-Teil `git diff 877541e2 HEAD -- re15_port` (861 Zeilen, `<sp>/E41a4/nb4.diff`)
  vollstaendig gelesen; Gesamt-Diff `<sp>/E41a4/code.diff` (4184 Zeilen).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; getrennt `build` -> "ninja: no work
  to do.", `=== LOCAL-BUILD-OK (build)` (`<sp>/E41a4/configure.log`, `build.log`). re15_pc.exe md5 732dbebc43ef246b131c27fa3e8cf5bb
  (= behaupteter Endstand) -> Messkopie `re15_pc_E41a4.exe`. VORHER-exe = `re15_pc_E41a3.exe` (c3ba4317 = Stand Abnahme 3, md5
  1f38e02535c6259e6f99619bba2917ab), BASIS = `<sp>/abn0_base/.../re15_pc_E41a1_basis.exe` (116e8b0f, md5 2310696431b26d115bc00f3796663c01).

(weitere Abschnitte folgen)

## 1. Tests der Spur (gezielt, im Spurbaum, kein test/all)
- `ctest --test-dir re15_port/build -R "r41_kleinkram_b|^unit_r36_zombieki_" -j 2 --timeout 1200 --output-on-failure` (20:37-20:41,
  CPU-Last fremder Laeufe ~93 %): **100% tests passed, 0 tests failed out of 29**, CTEST_EXIT=0 (`<sp>/E41a4/ctest_spur.log`): 12 integration
  (kurbel, karte, werkstatt_szene, werkstatt_tuer, kante, geh, renn, spawn_kante, **treppe**, **losl**), 13 unit (11b1 x3, karte_pin,
  kurbel x2, takt_{msgtakt,kamerahalt,frage,rennstopp,renntempo,rennpfad,**uebernahme**}), 6 unit_r36_zombieki_* (affen_block 59 s).

## 2. RE-Gate — Stichproben SELBST disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE bzw. STAGE1.BIN)
1. **Apply FUN_80021bbc liest blockierend** (`dis 0x80021cc8 30`, `dis 0x80013c50 40`, `dis 0x80013df0 60`): @0x80021cd8 `sll a3,a3,5`
   (Cut*0x20), @0x80021d0c/@0x80021d14 Datei-LBA aus 0x8006f440, @0x80021d28 `sw v0,0x800be57c` (LBA), **@0x80021d2c `jal 0x80013c50`,
   Verzoegerung @0x80021d30 `addu a1,zero,zero`**; FUN_80013c50 @0x80013c80 `bne s0(=a1),zero` -> a1 = 0: @0x80013c88 `jal 0x80013df0`
   mit a0 = 1; FUN_80013df0 Zustandsmaschine 0x800be570 (Sprungtabelle @0x80010514), Schleife @0x80013f60 `bne s1(=a0),zero,0x80013e04`
   bis Zustand 0 = **blockierend**. Danach @0x80021e34 `jal 0x80053a8c` (MDEC). Vorladen: @0x80021c30-@0x80021c48 `lui 0x2000` / `lw
   0x800aca38` / `beq` -> Bit loeschen @0x80021c5c, `j 0x80021d38` (kein Lesen). **Bestaetigt.**
2. **Renn-Stopp-Marke +6 = 3**: `dis 0x80032774 6` a1 = 0x800aca5a (@0x8003277c/@0x80032780); `dis 0x800328f0 10` @0x800328f8 `ori
   v0,zero,0x3`, @0x800328fc `sb v0,0(a1)`, @0x80032908/@0x80032910 +0x8f = 7. **Bestaetigt.**
3. **Ueberschreiber von +5/+6**: Treppen-Abschluss `dis 0x80038e98 8` @0x80038ea8 `sb zero,0x800aca59`, @0x80038eb0 `sb zero,0x800aca5a`;
   Plc_ret `dis 0x80041f84 8` @0x80041f90 `sb v1(=1),4(v0)`, @0x80041f94/98/9c `sb zero,5/6/7(v0)`; Plc_motion `dis 0x80041ba8 10`
   @0x80041bb0 `sb v1,4`, @0x80041bb4 `sb zero,6`, @0x80041bc4 `sb a2,5`; Treppe sce 12 @0x80043540/@0x80043544 `ori 0xb` / `sb v0,5(a1)`,
   sce 13 @0x8004360c/@0x80043610 dito, Rueckgabe @0x800435c4/@0x800435c8 `jr ra` / `ori v0,zero,0x1`; Griff STAGE1 @0x80102630-@0x80102640
   (`addiu -3`, `sll 8`, `ori 0x5`, `sw v0,0x800aca58` = Wort Kommando 5), Freigabe STAGE1 @0x8011c384/@0x8011c38c `ori v0,zero,1` /
   `sw v0,0x800aca58`; Dispatcher `dis 0x80031c44 14` @0x80031c54 `lw a0,g_pauseflags`, **@0x80031c78 `bltz a0,0x80031da8`**. **Bestaetigt.**
4. **Gemessene Dauer 14 VSyncs** (Original-Messung des Bau-Agenten selbst gelesen, `<sp>/E41n4/orig/o_kurbel_cut3.txt`): n103 vs10456 (Ja,
   `pad h4000`), n104 vs10458 `pl=3650,-2925 t2@0A99` (Pos_set im Durchlauf nach dem Ja), `# RD0 vs10460 sekt=31964 lba=39845 cut=3`,
   `# RD1 vs10468`, `# DEC vs10472`, n105 vs10472 -> Durchlauf n104 = **14 VSyncs**, Bild von n103 steht bis n105 = 16 VSyncs.
   Original-Video `<sp>/E41n1/orig/o_hg_cut3.mp4` selbst ausgewertet (ffprobe: konstant 16,72 ms je Bild = 1 Bild je VSync; Pixel-Diff
   `<sp>/E41a4/tools/vdiff.py`, Schwelle 24): v414 153 px, **v420 6045 px (Frage weg), v421..v436 0 px, v437 27104 px (Leon an der Kurbel)**,
   danach Leerlauf-Atmen alle ~4 Bilder -> Bild ohne Frage steht **17 Videobilder = 284 ms**.

## 3. Punkt 1 — Kurbel ROOM2020 ("komische herumfliegende Animation"; Nachtrag 14: "direkt am Anfang springt Leon so komisch zu der Kurbel. So halb fliegend.")
Massstab (Nachtrag 14, Folgen): Port vom "Yes" bis Plc_motion Bild fuer Bild wie das Original-Video, Leon erscheint erst an der Kurbel,
kein sichtbares Hinueberfliegen, Standbild-Dauer gemessen. Weil die Dauer eine WANDUHR-Wartezeit ist (bildhalt_r41_pc.c), die ein
RE15_FRAMEDUMP nicht zeigt, habe ich das ECHTE Fenster aufgenommen (Skill re15-port-visual-verify): `<sp>/E41a4/grab.sh` startet den
Lauf (Messkopie, eigener Ordner), setzt das Fenster per `SetWindowPos` TOPMOST an (20,20) (`tools/topwin.ps1`) und nimmt es per
`ffmpeg -f gdigrab -framerate 60 -i hwnd=<HWND>` verlustfrei auf (ffv1; Zeitstempel je Bild per ffprobe; Diff je Folgebild
`tools/gdiff.py`, Schwelle 24). Kein SOFTWARE_RENDER, kein AUTOSHOT, kein FRAMEDUMP in diesen Laeufen. CPU-Last fremder Laeufe ~90 %.

### 3.1 Weg des Nutzers gehend (Cut 0 -> 2 -> 3, links drehen, Quadrat, Ja) — `G1_weg_cut3` (NACHHER) / `G1v_weg_cut3` (VORHER)
Befehl (wie Bau-Agent `P1_weg_cut3`): `RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=9100,-3200,2048,1
RE15_INPUT_SCRIPT=U2.667,L0.25,W1.0,A0.034,W8.0 RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_PRESS=square@458,square@498,square@538,square@578 RE15_SCD_TRACE=1 RE15_MOTRACE=1 RE15_STATE_LOG=state.txt RE15_KAMERAHALT_LOG=1
RE15_EXIT_AT=650#2020`. debug.log NACHHER: `[kamerahalt] F301 ... HALTEN` / `[standbild] F301 6 Bilder 198 ms`, F347 dito, **`[kamerahalt] F540
... pend=1 wv0A=3 cam=3 -> HALTEN`, `[standbild] F540 6 Bilder 199 ms`**, `@0x0A7E op=0x29` + `@0x0A8C op=0x32` F540, `@0x0A9C op=0x3F` F560.
VORHER (c3ba4317): dieselben HALTEN-Zeilen, kein `[standbild]`.

| Fenster-Aufnahme | Frage zu (Textfeld 1411 px) | naechste Aenderung = Leon an der Kurbel (8799 px, ganzes Bild) | Bild ohne Frage steht | Kurbel-Clip beginnt (Leon-Bereich) |
|---|---|---|---|---|
| NACHHER g1525 -> g1539 | t=26,043 s | t=26,293 s | **250 ms** (14 Aufnahmebilder ohne Aenderung) | g1578 t=26,944 = **651 ms** nach dem Sprung |
| VORHER g1427 -> g1432 | t=25,525 s | t=25,600 s | **75 ms** | — |
| Original o_hg_cut3 v420 -> v437 | v420 | v437 | **284 ms** (17 VSync-Bilder) | v475 = **635 ms** (38 VSyncs) nach dem Sprung |

- Zwischen "Frage zu" und "Leon an der Kurbel" zeigt das Fenster in beiden Staenden KEIN Zwischenbild (Diff 0 je Aufnahmebild); NB4 aendert
  nur die Dauer (Folgebilder nach dem Sprung VORHER/NACHHER pixelgleich in der Diff-Folge 8799 / 18 / 10 / 13 / 19 / 30 ...).
- Bildvergleich `<sp>/E41a4/P1_G1_zoom.png` (Ausschnitt um Leon x1,5; oben Original v419/v420/v436/v437/v441/v450/v470/v490, unten
  Port g1524/g1525/g1538/g1539/g1543/g1552/g1572/g1592): gleiche Folge — Frage; Leon an der alten Stelle an der Reling; stehendes Bild;
  Leon steht an der Kurbel (Stehpose, Blick zur Wand); Atmen; Kurbelpose. Kein Gleiten, kein Abheben, keine Fremdpose.
- Letterbox nach dem Sprung (Zeilenmittel oben/unten): Port g1539 Zeilen 0-20 ~12, g1545 0; Original v437 15-17, v441 2-3 — beide
  blenden die Balken in ~4 Bildern ein (`<sp>/E41a4/runs/G1_weg_cut3/crank_seq.png`).
- Abweichung: Standbild Port 250 ms (Rechnung 2 x 33 + 198 = 264 ms) gegen Original 284 ms = 1-2 VSyncs kuerzer (Messgenauigkeit
  der Aufnahme +-17 ms); Sprung -> Kurbel-Clip 651 gegen 635 ms. VORHER war das Standbild 75 ms = gut ein Viertel des Originals.

### 3.2 Stehen (`G2_stand`) und Rennen (`G3_renn`) — Fenster-Aufnahme NACHHER
- Stehen: `RE15_PLAYER_POS=2600,-2800,1024,1 RE15_PRESS=square@330,square@380,square@420,square@460` (sonst wie 3.1, EXIT_AT 560): `[standbild]
  F462 6 Bilder 199 ms`, Pos_set F462, Plc_motion F482. Fenster: Frage zu g1344 (t=22,967) -> **Cut 3 mit Leon an der Kurbel g1360 (t=23,233)
  = 266 ms**, dazwischen 0 px. Die Frage steht wie im Original in der Tuer-Kamera (Leon nicht im Bild); Bogen `<sp>/E41a4/P1_G2_stand_sheet.png`
  (oben Port g1343/g1344/g1359/g1360/g1366/g1400, unten Original o_w03_stand v433/v434/v469/v470/v476/v510): gleiche Folge. Dauer im einzigen
  Original-Stand-Video (`o_w03_stand.mp4`, selbst ausgewertet: v434 Frage zu, v435..v469 0 px, v470 Cut 3) = **36 Bilder = 602 ms** — dieser
  Original-Lauf startete aber nach einem Waffenwechsel im Statusschirm (PLW-Laden, weiter Kopfweg; GDB des Bau-Agenten hg_ready: 34 VSyncs).
- Rennen (Riegel-Weg renn/o_renn2): `RE15_PLAYER_POS=2600,-800,1024,1 RE15_INPUT_SCRIPT=XU0.2,XUA0.1,XU0.1,...,XU1.0` (Start 300), Quadrat
  @380/430/470/510: `[standbild] F382 6 Bilder 198 ms`; state.txt F382..F387 z **-3099 -3233 -3327 -3381 -3395 -3369** (ps2 = 3 bis F386, F387 0)
  = o_renn2 n94..n99; Plc_motion F402. Fenster: Frage zu g1072 (t=20,483) -> Cut 3 g1086 (t=20,750) = **267 ms**, dazwischen 0 px; danach rutscht
  Leon in 6 Bildern in die Stehpose aus (`<sp>/E41a4/P1_G3_renn_zoom.png`) — wie die GDB-Folge des Originals (Lage, Clip 3, Mischzaehler; Abnahme 3
  §3.1). Ein Original-VIDEO des Renn-Anlaufs gibt es nicht; Original-Dauer dieses Apply auf dem Renn-Weg 15 VSyncs (`o_kurbel_cut3_renn.txt`).

- Weg des Nutzers aus seinem befund.log (Abnahme 3 §3.5: Tuer 2050 -> 2020, gerannt, gestoppt, gedreht, Aktion im Stand), Fenster-Aufnahme
  `G4_nutzerweg` (`RE15_DEBUG_JUMP=2050@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_FIRE_AOT=1@30#2050 RE15_INPUT_SCRIPT=L0.36,XU1.47,W0.3,L0.4,U0.4,
  A0.034,W5.0` Start 60, Quadrat @200/240/280/320): `[standbild] F83 / F100 / F282 / F410 6 Bilder 198 ms`; Frage zu g1209 (t=22,823) -> Leon an der
  Kurbel g1223 (t=23,090) = **267 ms**, dazwischen 0 px; Bogen `<sp>/E41a4/P1_G4_nutzerweg_zoom.png` (Frage / zu / steht / Kurbel / Blende /
  Kurbelpose) — dieselbe Folge wie das Original-Video.
- Elza (`N_P1_elza_stand`, ROOM2021, `RE15_TITLE_SHOT_CHAR=1`, Stand wie 3.2): `PL04 (character=4, Elza-Bit=1)`, `[kamerahalt] F462 raum=2021 ...
  HALTEN`, `[standbild] F462 6 Bilder 199 ms`, Pos_set F462 (2600,-3018) -> (3650,-3018), Plc_motion F482 — wie Leon.

### 3.3 Urteil Punkt 1: **erfuellt**
Gegen den Massstab des Nachtrags 14 gemessen am echten Fenster: zwischen "Frage zu" und "Leon an der Kurbel" steht das Bild jetzt
250-267 ms (Original auf dem Weg des Nutzers 284 ms; VORHER 75 ms), kein Zwischenbild, kein Hinueberfliegen, dieselbe Pose/Blende/Letterbox-
Folge wie das Original-Video, Sprung -> Kurbel-Clip 651 ms (Original 635 ms). Der Mechanismus ist belegt (FUN_80021bbc blockierendes Lesen +
MDEC, §2 Nr. 1/4) und erklaert den Befund ("Original friert ~1/4 s ein, Port 1 Bild" = die vom Nutzer gewaehlte Antwort). Rest (kein Mangel,
OFFEN des Bau-Agenten): die Original-Dauer schwankt mit dem CD-Kopfweg (12-15 VSyncs RVD, 34-36 nach PLW-Laden, z.B. o_w03_stand 602 ms);
der Port nimmt einheitlich den Wert des Nutzer-Wegs. Ein Bildvergleich gegen ein Original-VIDEO liegt nur fuer Gehen (o_hg_cut3) und Stehen
(o_w03_stand, Dauer s.o.) vor; Rennen ist ueber die GDB-Folge belegt.

## 4. Mangel 1 der Abnahme 3 — Renn-Stopp nach einer Uebernahme (Treppe, Griff, Plc_ret) — selbst nachgemessen
Laeufe `<sp>/E41a4/runs/{M,V}_<lauf>/` (M = NACHHER re15_pc_E41a4.exe, V = VORHER re15_pc_E41a3.exe = c3ba4317), Env-Dateien
`<sp>/E41a4/env/` (Befehle der Abnahme 3 / des Bau-Agenten, ohne FRAMEDUMP), Vergleich je Bild `<sp>/E41a4/tools/stcmp.py` (Lage, Gier, mo, pst, cam).
- **Treppe im Stopp** (`N2_treppe`: ROOM2040 Galerie Band 5, `XU0.4` an die Kante, dann Loslassen + Quadrat, Treppe sce 13 abwaerts):
  | Bild | NACHHER z | VORHER z |
  |---|---|---|
  | F312 (Aktionsbild, Treppe ab) | -18368 mo 220 ps2 3 | -18368 mo 220 ps2 0 |
  | F374 / F375 (Abschluss) | -14234 / -13782 | -14234 / -13782 |
  | F376 / F377 / F378 / F379 | **-13782 / -13782 / -13782 / -13782** | -13665 / -13588 / -13551 / -13554 (+117 +77 +37 -3) |
  | F380..F383 | -13782 | -13554 |
  `N2b_treppe` (2 Stopp-Bilder vor der Aktion): NACHHER F376.. -13782, VORHER F376 -13783 (-1). `N2c_treppe` (Rennen gehalten): beide -13782.
  **Behoben** — Leon steht nach dem Treppen-Abschluss, wie LAB_80038e50 @0x80038ea8/@0x80038eb0 (+5/+6 = 0) es verlangt (§2 Nr. 3).
- **Keine Nebenwirkung** (NACHHER gegen VORHER je Bild, alle Zeilen gleich): `N1_tuer` (Tuer im Stopp, 1447 Zeilen, Tuer 1230 -> 1190 laeuft),
  `N3_inventar` (Statusschirm im Stopp, 461), `P1_frei` (freie Strecke, 501: Stopp **157 117 77 37 -3** mit ps2 = 3, letzter Schritt ps2 0 =
  o_rennstopp), `TA_links` (361), `TC_stopp` (361), `E_elza_renn` (Elza ROOM2021, 351), `P1_losl` (Kurbel Loslassen + Aktion, 661: ps2 = 3
  von F163 durch die gesperrten Frage-Bilder bis F285, nach Pos_set F282..F286 **-3056 -3147 -3198 -3209 -3180** = o_renn_losl n94..n98),
  `P1_renn_a2` (721: F382..F387 -3099 .. -3369 = o_renn2 n94..n99).
- Griff und Plc_ret im Stopp sind am echten Spiel schwer herzustellen (6 Stopp-Bilder); dafuer der Riegel `unit_r41_kleinkram_b_takt_uebernahme`
  (gruen, §1) ueber den echten Spieler-Takt: T Treppe, R Plc_ret (SCD-Schnipsel `42 01`), F Flinch, P gesperrte Durchlaeufe, G Griff
  (Pin + Freigabe mit Bewegung 200 wie enemy_ai_common.c); Gegenprobe vor NB4 T/R/G rot (Code gelesen, §1).
- Code gelesen (rennstopp_r41.c): Marke +6 = 3 im Stopp (`RS_FALL3` @0x800328fc), +6 = 0 bei Stopp-Ende/Abbruch (@0x80032958 / Wort
  @0x80032760); `re15_rennstopp_r41_dispatcher` (game_step_common.c vor der Zweig-Kette) loescht den Stopp, wenn der letzte UNGESPERRTE
  Durchlauf keinen Kommando-1-Schritt hatte (Klettern, Treppe, Tod, Griff, Knockdown, Flinch, Schieben, Skript mit Plc_dest/anim_freeze);
  gesperrte Durchlaeufe (Spieler-Bit = `bltz` @0x80031c78) zaehlen nicht; msg_block-Zweig = `halten()` (keine Uebernahme). Erklaert den
  Befund (Abnahme 3: Modulzustand ueberlebte die Treppe, weil der Treppen-FSM den Spieler-Takt auslaesst).
- Offen wie vom Bau-Agenten benannt (H2 Abnahme 3): im Aktionsbild einer Treppe macht der Port einen Stopp-Schritt (F312 ps2 3), das Original
  nicht (Handler-Rueckgabe 1 @0x800435c8, selbst gelesen); an der gemessenen Treppe ohne Lagewirkung (Kollision an der Kante, F312 z -18368).
**Urteil Mangel 1: behoben.**

## 5. Mangel 2 der Abnahme 3 / E-E4 — Gorilla-Sonde misst den Haenger (E-E5 beachtet)
- Code gelesen (test_r36_zombieki_affen_block.c, NB4-Teil): je blockierte Strecke (>= 90 Bilder, Anker 400) zaehlt `akk_fk` die FERN-Bilder
  (Leon >= 0x1771 @0x80117bc8) AN der Klemme (`sca_wall_hit`); `zg_haenger` nimmt je Seed die groesste Strecke ausserhalb der benannten
  Stellen (`zg_ort`), an den Stellen wird summiert und ausgegeben. Riegel: `fk_ueber == 0` (keine Strecke >= R41_WARTE_MAX = 354), Gegenprobe
  `fk_ueber_aus > 0`; die alte Schranke `R41_BAHN_CHAOS = 5982` ist entfernt, die Seed-Differenz nur noch Ausgabe.
- Schranke 354 = `R36_NAH_START` (zombieki_r36.c:213, Herleitung :202-212 = kanonischer Original-Kampf, NAH-Anker hoechstens 353 Bilder,
  J-Harness/GDB) — eine vor dieser Sonde vorhandene, unabhaengig gemessene Zahl, nicht aus dem Bestehen gewaehlt (AN-Maximum 103 liegt weit
  darunter, die Original-Regel reisst sie in 76 Seeds).
- Selbst gemessen `ctest -R "^unit_r36_zombieki_affen_block$" -V` (`<sp>/E41a4/ctest_affen_V.log`): `ok: HAENGER ... groesste AN 103
  (0x6394780b bei (-13592,11847)), Seeds darueber 0; Original-Regel AUS: 76 Seeds darueber`, `ok: Gegenprobe Original-Regel ... (76 Seeds >= 354)`,
  benannte Stellen TASCHE AUS 6223/2 | AN 6583/2, SUB7 36710/57 | 12637/20, WESTBLOCK 5880/15 | AN 3590/6; FERN 24433 <= 1/6 von 253141; Griffe
  657 >= 80 % von 587 — Zahlen wie im Dossier.
- Einzel-Laeufe `test_r36_zombieki_affen_block eins <i> 1 6000 400 1` (`<sp>/E41a4/affen/an_<i>.txt`): i = 2 (0x9b5bf362) `LAGE ... g1 11129 9982
  ... dauer 4405 ... fklemme 4393` (TASCHE, der Haenger der Abnahme 3); i = 47 (0x6b1c577f) g1 (-13418,-140) fklemme 1994 (SUB7); i = 117
  (0xae479de5) g1 (5982,12145) fklemme 760 und g0 (4316,10018) fklemme 756, Sub 4 (WESTBLOCK) — ohne die dritte Ausnahme risse der Riegel hier.
- Die dritte Ausnahme WESTBLOCK ging ueber E-E4 (zwei Stellen) hinaus; **Orchestrator-Entscheid E-E5 (master 6fc20609, 20:34) bestaetigt sie**
  ("dort haengt auch die Original-Regel (AUS 5880 in 15 Seeds)" — von mir bestaetigt) und fuehrt die drei Haenger als Folgeauftrag (UEBERGABE §3b).
**Urteil Mangel 2: behoben im Sinne von E-E4/E-E5** (Sonde misst den Haenger wieder; drei benannte, belegte Ausnahmen; Folgeauftrag gestellt).

## 6. Punkt 2 — Karte B1, vordere Tuer der "Waffen Armory" (Gang ROOM1230 -> Ostkammer ROOM1190) — neu gemessen
Gemeinsam `RE15_DEBUG_JUMP=1230@240 RE15_PLAYER_POS=96,14500,1024,0 RE15_SET_FLAG=4:0xf3,3:0x89,7:0x88,7:0x89 RE15_INPUT_SCRIPT=<anlauf>,W0.2,S0.1,
W1.0,Q0.1 RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.txt RE15_INV_FB_SHOT=map.bmp RE15_INV_FB_SHOT_AT=120
RE15_MAP_SHOT_PAGE=0 RE15_EXIT_AT=560`, Marker per `test_r41_kleinkram_b_exe marker map.bmp`. NACHHER = HEAD, BASIS = 116e8b0f (`run2.sh`: der
Basis-Baum hat kein shared_assets mehr -> `RE15_ASSET_ROOT` = shared_assets/PSX des Spurbaums; `git diff 116e8b0f HEAD -- re15_port/shared_assets` leer).
Laeufe `<sp>/E41a4/runs/{N,B}_P2_*`.

| Lauf | Spieler F399 | BASIS | NACHHER |
|---|---|---|---|
| Leon U0.2 (weit weg) | (96,14050) | — | MARKER 145 104 |
| Leon U0.6 | (96,13150) | **MARKER 137 109** | MARKER 145 106 |
| Leon U1.0 (vor der Tuer) | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Elza `RE15_TITLE_SHOT_CHAR=1` (room1231.rdt, PL04), U1.0 | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Leon RENNEND `XU0.25,W0.5` (Stopp ps2 = 3 an der Tuer) | (96,12799) | — | MARKER 145 107 |
| Leon echte Tuer `U1.0,W0.3,A0.1` + `RE15_INV_OPEN_AT=60#1190` (`[tuer] Sequenz fertig: 301 Bilder`, room1190.rdt, (19515,-12109)) | 1190 | — | MARKER 145 108 |

Ostkammer-Marke (146,109), Hallen-Marke (127,115). BASIS wandert beim Herantreten an die vordere Tuer bis 15 px zur Hallen-Tuer (Nutzer-Befund
reproduziert), NACHHER bleibt der Marker an der Ostkammer-Tuer — Leon und Elza, gehend und rennend, nach dem Durchgang in der Ostkammer.
Ausschnitt x6 `<sp>/E41a4/P2_karte_basis_nach.png`. Code (karte_r41.c, Haken re15_inv_screen.c) seit Abnahme 1 unveraendert (NB4-Diff beruehrt
ihn nicht). **Urteil Punkt 2: erfuellt.**

## 7. Punkt 3 — Werkstatt-Tuer ROOM11B1 bei Elza dauerhaft zu — neu gemessen
- **Echter Tuerweg mit Szene** (`P3a_szene`: `RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=1261@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3
  RE15_PLAYER_POS=27650,9450,1024,1 RE15_PRESS=square@300,square@310 ... RE15_FRAMEDUMP=690:fd_690.ppm RE15_EXIT_AT=700#11b1`; Flags = Zustand nach
  10B1 sub04 @0x0195E und 2001 sub04 @0x01958 `22 03 46 01`, selbst gelesen `scd_dump_room.py ROOM2001.RDT`): NACHHER `loaded room1261.rdt` ->
  `[tuer] Sequenz fertig: 331 Bilder` -> `loaded room11b1.rdt` -> `[wk11b1-r41] ROOM11B1 (3,0x83)=1: Ereignis 24 ... Werkstatt-Tuer bleibt zu`,
  msg 10 "Elza: Sherry!?" (@0x1480 F84), msg 11 "Elza: Where has she gone!?" (@0x14CE F372), Plc_ret @0x14E6 F473; Kameras 6,5,4,3,**2**, Ende (-18192,
  -20453) Cut 2. BASIS: dieselbe Szene, Kameras 6,5,4,3,**8**, Ende Cut 8 (gemaltes offenes Tuerblatt). Bild F690 `<sp>/E41a4/P3a_ende_basis_nach.png`:
  links BASIS Tuerblatt offen (hell), rechts NACHHER geschlossene Tuer.
- **Dauerhaft**: Garagenseite (`P3b_garage`, `RE15_PLAYER_POS=-20200,-16230,2048,0`, `U1.5,W0.3,A0.1`): ab F272 **(-20942,-16230) blockiert**, `"The door is
  locked."`, Raum bleibt 11b1. Werkstattseite (`P3c_werkstatt`, (-24000,-16230) Blick 0, `U2.0`): NACHHER ab F280 **(-22618,-16230) blockiert**, Cut 0,
  msg 12; BASIS laeuft durch (F290 -21825, F300 -21075, F320 -19768, Kameras 7 -> 8). Wiederbetreten nach der Szene ueber die echte Tuer (`P3d_wieder`,
  1261 -> 11B1 mit (3,0x83) = 1, (3,0x46) = 0): `[wk11b1-r41] ... Werkstatt-Tuer bleibt zu`, Elza (-195,-21821) Cut 6, keine Szene.
- **Leon unberuehrt**: `P3e_leon11b0` (ROOM11B0, (3,0x83) = 1, Werkstattseite, `U2.0`): NACHHER gegen BASIS **661/661 Zeilen gleich** (Leon geht durch,
  Szene "Leon: H-hey!"), kein `[wk11b1-r41]`.
- Grenze wie Abnahme 3: der Abschnitt 2001 -> 1261 ist nicht gefahren (Flags gesetzt); 1261 -> 11B1 ist die echte Tuer. Code von Punkt 3 seit Abnahme 1
  unveraendert. **Urteil Punkt 3: erfuellt.**
