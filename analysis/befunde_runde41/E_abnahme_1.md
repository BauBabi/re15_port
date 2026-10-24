# Runde 41 — Spur E "kleinkram_b" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Pruefer: Abnahme-Agent 1, 2026-10-24 (Beginn 04:53 laut `date`). Baum `.claude/worktrees/r41_kleinkram_b`, Zweig
`r41/kleinkram_b`, HEAD 753652fc (Code-Stand: 6 Commits der Nachbesserung 1 auf ed7bd4ff; Basis master 116e8b0f).
`<sp>` = Scratchpad `C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`
(derselbe Ordner, in dem Bau-Agent und Abnahme 0 ihre Laeufe abgelegt haben); eigene Laeufe unter `<sp>/E41a1/`.

Status dieses Berichts: ABGESCHLOSSEN (fortlaufend gespeichert; §0-§8).

## 0. Gelesen
- AUFTRAG.md (Baum, Stand 88bfae38) und master-AUFTRAG.md (c94d7c48, 04:52:58 — NACH dem letzten Bau-Commit 753652fc
  04:51:33): neu dort die Orchestrator-Auftraege **E-N1** (Kamera-Flackern ROOM2020 bei (9000,-3200)) und **E-N2** (Geh-Stopp
  waehrend der Frage, "Mechanismus nicht gefunden = NICHT fertig"); kein Nutzer-Wortlaut, Spur E konnte sie noch nicht kennen.
- VERTRAG.md, UEBERGABE.md, Dossier E_kleinkram_b.md (ganz, inkl. "Nachbesserung 1"), E_abnahme_0.md.
- `git log --oneline master..HEAD` (25 Commits, davon 6 der Nachbesserung 1: 58992723 d8492f7b b2f5bb97 76a81373 b1d551d2 753652fc), `git diff master --stat`, Code-Diff `re15_port/engine`, `re15_port/include`,
  `re15_port/tests` vollstaendig gelesen; dazu `git diff ed7bd4ff HEAD -- re15_port` (Aenderung der Nachbesserung 1: scd_vm.c
  -2 Zeilen = Haken entfernt, werkstatt11b1_r41.c Slot-Suche statt scd_event_fire, neue Integrations-Riegel).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; getrennt `build` -> "ninja: no work
  to do", `=== LOCAL-BUILD-OK (build)` (`<sp>/E41a1/configure.log`, `build.log`). re15_pc.exe md5 beeca32b2e23c31edf7ad06a2073c5db
  (04:37) = Messkopie `re15_port/build/platform/pc/re15_pc_E41a1.exe`. Arbeitsbaum sauber (`git status --short` leer).

## 1. Tests (gezielt, im Spurbaum, kein test/all)
- `ctest --test-dir re15_port/build -R "r41_kleinkram_b" -j 2 --timeout 900 --output-on-failure`: **100% tests passed, 0 tests
  failed out of 10**, 74,8 s, CTEST_EXIT=0 (`<sp>/E41a1/ctest_spur.log`): 6 unit + integration_r41_kleinkram_b_kurbel 38,7 s,
  _werkstatt_szene 53,3 s, _karte 27,6 s, _werkstatt_tuer 21,5 s. Drei Riegel mit `-V` nachgesehen: 11b1_zu 11/11 PASS, karte_pin
  17/17 PASS, kurbel_schleife (Trace Plc_motion/Plc_flg(0,4) -> anim_flags 0x0004, Cut 3/4/3) — die Riegel pruefen wirklich.
- Unit-Gruppe aller beruehrten Module/Raeume (`<sp>/E41a1/ctest_unit.log`, -j 4): Karte/Marken (re15_inv_screen.c:
  map_/karte_/r30..r39_karte*/kartenbits/kartenitems/tagebuch_karte), Spieler/Posen (player_common.c: plc_*/anim_*/npc_scd_pose/
  eintritt_pose_einmal/r30_android_n3_plc_aim/1090_push/probe_1090_flinch_anim), Ereignis-Kette + Raumaufbau (scd_room_setup.c,
  frueherer scd_vm.c-Haken: scd_*/r40_nahaufnahmen/r40_untersuchen/r39_hund1121/r39_elzazombies/r39_funk10f1/r38_dining/
  r36_schluessel/r35_cut10f0/r39_tuersperre), 11B0/11B1 (r36_szenenleon/r38_werkstatt11b0/r39_johnruf1041/r37_elzaems), Szenen mit
  Plc_flg(0,4) (r35_cut1150/r36_szene1150/r36_elza/r37_roy/r38_elzatoene/r27_ada_cutscene_1050/r37_szene11c0/r38_szenenschutz/
  r38_npckante/r3x_heliport1171/r35_affen_szene/r36_zombieki_szenen/r39_elza*): **100% tests passed, 0 tests failed out of 217**.
- Gegenprobe der 4 neuen Integrations-Riegel mit der VORHER-exe (Basis 116e8b0f, `<sp>/abn0_base/.../re15_pc_E41a1_basis.exe`,
  md5 2310696431b26d115bc00f3796663c01 = Abnahme 0), `cmake -DRE15_PC_EXE=<vorher> ... -P tests/integration/test_r41_kleinkram_b.cmake`
  (`<sp>/E41a1/vorher_riegel/*.log`): **alle 4 DURCHGEFALLEN** aus dem Mechanismus — kurbel "Umlaeufe 39->0: 0, laengster Halt auf
  Slot 39: 139 Bilder"; karte "MARKER 130 112"; werkstatt_szene "kameras=3,4,5,6,8 letzte_kamera=8"; werkstatt_tuer
  "min_x=-23575" (durchgelaufen), kein msg 12. NACHHER gruen (oben). Die Riegel messen also die Mechanik, nicht nur Logzeilen.

## 2. RE-Gate — Stichproben SELBST disassembliert (re15_disasm.py, info/Re1.5/PSX.EXE = Auslieferungsstand)
1. **SCD-Sprungtabelle @0x800744a8** (`table 0x800744a8 0x60`): [0x29] 0x800402a0 (Cut_chg), [0x2E] 0x80040d2c (Work_set),
   [0x2F] 0x80040f14 (Speed_set), [0x30] **0x80040f40 (Add_speed)**, [0x3F] 0x80041b90 (Plc_motion), [0x4B] 0x80040414
   (Cut_replace) — die im Dossier zitierten Handler sind die richtigen Sprungziele.
2. **Work_set LAB_80040d2c** (`dis 0x80040d2c 60`): @0x80040d34..3c loescht 6 Worte ab a0+344 (+0x158); Art pc[1]: 1 ->
   @0x80040da4 `lui v0,0x800b` / `addiu v0,v0,-13740` (= 0x800aca54 Spieler) / `sw v0,340(a0)` (+0x154); 3 -> @0x80040dd4..
   @0x80040df0 (a1*148) + 0x800b3f98; 2 -> Gegner 0x800acc2c + a1*500; 5 -> Tabelle 0x800b23f4. **Bestaetigt.**
3. **Add_speed LAB_80040f40** (`dis 0x80040f40 40`): `lw a1,340(a0)` (Ziel +0x154), +0x158/+0x15a/+0x15c auf Ziel
   +0x34/+0x38/+0x3c, +0x15e/+0x160/+0x162 auf +0x68/+0x6a/+0x6c. **Bestaetigt.** RDT ROOM2020 (md5 53f696564c431158ef9efc3c4bd9c5c3,
   shared_assets == info) selbst gedumpt: @0x0A88 `2e 01 00` (Work_set Spieler) -> @0x0A8C `32 00 42 0e f8 f8 93 f4` Pos_set
   (3650,-1800,-2925) -> @0x0A94 `34 04 00 04` -> @0x0A98 `09 0a 14 00` Sleep 20 -> @0x0A9C `3f 00 00 00` -> @0x0AA0 `43 00 04 00`
   -> @0x0AA8 `2e 03 01` (Work_set Objekt 1) `2f 01 33 00` Speed_set + For/`30` Add_speed, @0x0AD2 `2e 03 02`, erst @0x0B12 wieder
   `2e 01 00` vor @0x0B16 `42 00` Plc_ret. Add_speed trifft also nie den Spieler — wie behauptet.
4. **Cut_chg LAB_800402a0** (`dis 0x800402a0 36`): alter Cut 0x800b0fe4 -> 0x800b3f7b/0x800b0fe8, `@0x800402f4 sb v0(=1),0x800b5457`
   OHNE Vergleich, `@0x800402fc sh a0,0x800b0fe4`, jal 0x800142f4. Zeichner (`dis 0x80021528 50`): `@0x80021530 lbu 0x800b5457`,
   != 0 und (0x800aca38 & 0x200000) == 0 -> `@0x80021558 jal 0x80021bbc` + `@0x80021560 j 0x800215fc` (ueberspringt die
   jal 0x80068fc4 @0x800215bc/@0x800215d0/@0x800215e4); FUN_80021bbc `@0x80021d2c jal 0x80013c50`. **Bestaetigt** — das
   Nachlade-Standbild des Originals bei Cut_chg auf denselben Cut ist Code, seine Dauer haengt am Laden.
5. Code-Diff der Nachbesserung 1 (werkstatt11b1_r41.c): Slot-Suche `SCD_EVENT_SLOT_FIRST..LAST`, `!active && scd_thread_start(s,
   prog) == 0` — Zeile fuer Zeile dieselbe wie scd_event_fire (scd_vm.c:684-688); keine neue Konstante. scd_vm.c ist bytegleich
   zu master (`git diff master -- re15_port/engine/src/scd_vm.c` leer).
- Wortsuche im Branch-Diff (deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/hack/vermutlich/ungefaehr/approx/
  geschaetzt): Ergebnis in §6.

## 3. Punkt 1 — Kurbel ROOM2020 ("komische herumfliegende Animation ... am Anfang wenn er die Kurbel nutzt")
Laeufe `<sp>/E41a1/runs/<lauf>/` (cmd.txt, debug.log, state.txt, anim.txt, fd_*.ppm), Helfer `<sp>/E41a1/run.sh` (exe-Kopie,
eigener Ordner, geerbte RE15_* geloescht), Bildboegen `<sp>/E41a1/sheet.py`, Paarboegen `<sp>/E41a1/pair.py`.
Gemeinsam `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1`.

### 3.1 Der Weg des Nutzers (befund.log selbst gelesen)
`C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund.log` (nur gelesen): drei Kurbel-Szenen in Sitzungen der exe
"Oct 19 2026 15:24:05" (Kopfzeilen 64588/64854/...): (1) F235 `R2020 C3 pos=(2166,-1800,-3382) rot=864` -> F295 `(3650,-1800,-3018)
rot=1024`; (2) F1031 `(3178,-1800,-3382) rot=5496` -> F1046 `(3650,-1800,-3018) rot=1024`; (3) F136..F166 `(2362,-1800,-3382)
rot=-3672` -> F181 `(3650,-1800,-3018) rot=1024`. Alle drei: Kamera C3, Spieler an der Suedkante z -3382 (Lauf in -z endet dort),
`masken=0`. Szene (3) kam im GEHEN an der Kante entlang (F91 x 462 -> F106 1782 -> F121 1822 -> F136 2362, danach steht er);
ob er im Gehen oder im Stand drueckte, gibt das 15-Bild-Raster des befund.log nicht her. Build-Stand: exe-Zeit
15:24 liegt zwischen 86394fc2 (14:51) und f8ff7e15 (15:41); `git diff --stat 86394fc2 116e8b0f -- re15_port/engine re15_port/platform
re15_port/include` = nur dining_r39, pri_masken_zwilling_r37, nahaufnahmen_r40, untersuchen_r40 (+ deren 2+4 Haken-Zeilen) —
kein Spieler-/Anim-/Render-Code. Der Nutzer sah also das Verhalten der VORHER-exe (Basis 116e8b0f).

### 3.2 R1 (NACHHER) / R3 (VORHER) — echter Weg ROOM2050 -> Tuer Slot 1 -> ROOM2020 -> Cut 3 -> Kurbel (Weg des Integrations-Riegels)
`RE15_DEBUG_JUMP=2050@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_FIRE_AOT=1@30#2050 RE15_INPUT_SCRIPT=L0.36,U3.8,(LA0.034,L0.034)x8,W2.0
RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT_BASIS=spiel RE15_PRESS=square@260,square@300,square@340,square@380 RE15_SCD_TRACE=1
RE15_ANIM_TRACE=anim.txt RE15_MOTRACE=1 RE15_STATE_LOG=state.txt RE15_MSG_LOG=1 RE15_CAM_TRACE=1 RE15_FRAMEDUMP=150-460/1:fd_
RE15_EXIT_AT=600#2020` (Laeufe `R1_weg2050`, `R3_weg2050_vorher`).
- debug.log (beide): `[fire-aot] slot=1 at F30 (Raum 2050)`, `loaded room2020.rdt`, `[scd] F186 @0x0A5C op=0x2B` (msg 0),
  `F262 @0x0A62` (Frage), `F342 @0x0A62` (Ja), `F343 @0x0A7E op=0x29 / @0x0A8C op=0x32 / @0x0A94 / @0x0A98`, `F363 @0x0A9C op=0x3F /
  @0x0AA0 op=0x43`, `[scd F471] Cut_chg(4)`, `[prop-member F539] prop[1] ... val=14316`, `F541 @0x0B16 op=0x42`.
- state.txt R1: F336..F342 `PL(2914,-3388,rot=1920) mo=200`, **F343 `PL(3650,-3018,rot=1024)`** (Sprung im Bild des Pos_set),
  bis Plc_ret 197/197 Bilder (3650,-3018) (Riegel-Auswerter). anim.txt Aktor 0: F338..F362 `200 3 1` (Clip 3, 1 Bild = Stehen
  ohne Wiegen), ab F363 `0 0 40` Clip 0.
- **VORHER (R3):** anim.txt F363..F402 Slot 0..39, dann **F403 .. Slot 39 eingefroren** (`cur` 40, 41, ... bei Slot 39) bis zum
  Schnitt auf Cut 4 F471 = 68 sichtbare Standbilder in Cut 3 (danach Buehne in Cut 4, Leon nicht im Bild). Reproduziert den
  Befund der Abnahme 0. **NACHHER (R1):** Clip 0 wrappt 39 -> 0 durchgehend (Riegel: 4 Umlaeufe, max 1 Bild Slot 39).
- Bilder: `<sp>/E41a1/R1c_338_377.png` (Leon-Ausschnitt x2, F338..F377): F338..F342 Frage, Leon mit dem Ruecken zur Kamera;
  F343 steht er an der Kurbel (naeher, seitlich); F344..F362 regungslos; F363..F365 Uebergang (Beine auseinander, Arm aus);
  ab F366 vorgebeugt, Kurbeln.

### 3.3 Original gegen Port, Bild fuer Bild — Cut-3-Anlauf (Video des Bau-Agenten `<sp>/E41n1/orig/o_hg_cut3.mp4`, selbst ausgewertet)
`ffprobe`: 640x480, 59,82 fps, 988 Bilder; `ffmpeg -vsync 0 -vf scale=320:240:flags=area` -> `<sp>/E41a1/ocut3/o_NNNN.png`.
Bitgleiche Folgebilder (eigenes Skript, mittlere Abweichung): Laeufe gleicher Bilder u.a. **v421..v436** (17 gleiche Videobilder
nach dem Schliessen der Frage), Schnitte v703 (-> Cut 4) und v856 (-> Cut 3).
- `<sp>/E41a1/O_cut3_410_441.png`: v410..v419 Frage, Leon an der Anlaufstelle; v420..v436 Frage weg, Bild steht; **v437 steht Leon
  an der Kurbel** — der Sprung ist Original (bestaetigt die Behauptung), das Original zeigt vorher ~8 Spielbilder Standbild
  (Code: §2 Nr. 4), der Port 2 Bilder (F341/F342) und dann sofort F343.
- Paarboegen `<sp>/E41a1/P_cut3_k14_25.png`, `P_cut3_k26_37.png` (oben Original v(438+2k), unten Port R1 F(343+k), Original +5 px
  wie Abnahme 0): Stehen, Uebergang und Kurbelposen gleich. Bewegte Pixel im Leon-Fenster x50-110/y60-180 (|diff|>24) je Bild:
  Original k1..k18 **0**, k19 147, k20 250, k21 134, k22 153 ...; Port k1..k19 **0**, k20 165, k21 260, k22 176 ... — Stehen ohne
  Wiegen in beiden, der Uebergang beginnt im Port 1 Spielbild spaeter (Ausrichtung am Sprungbild; das Original verschiebt den Sprung
  durch das Nachladen). Die "fliegende" Zwischenpose (Beine gespreizt, Arm aus, k20/k21 bzw. F364/F365) zeigt das Original genauso.
- Behauptung des Bau-Agenten "Original Cut-3-Anlauf Bild fuer Bild gleich": **bestaetigt** (bis auf die Nachlade-Dauer).

### 3.4 R2 (NACHHER) — Anlauf im GEHEN mit Aktionstaste (Weg a1 des Bau-Agenten, jedes Bild)
`RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=2600,-1500,1024,1 RE15_INPUT_SCRIPT=U0.3,(UA0.1,U0.1)x4,UA0.1,U1.0
RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_PRESS=square@380,square@430,square@470,square@510 RE15_FRAMEDUMP=296-480/1:fd_
RE15_EXIT_AT=500` (+Traces wie R1), Lauf `R2_geh`.
- state.txt: F300..F304 Gehen (mo=105 Clip 5) bis z -3393 -> F305 -3382 (Kante), F309 `pad=8010` Ausloesung, F310 msg 0; anim.txt
  waehrend msg 0 **Geh-Bild 9 eingefroren**; **F328 `msg(a=0)`: ein Spielbild ohne Meldung, Geh-Bild 9 -> 10** (der Spieler tickt
  ein Bild lang); F329..F380 Frage (Geh-Bild 10 eingefroren); Ja F380; F381 `mo=200` (Clip 3, 1 Bild); F383 Pos_set (3650,-3018)
  rot 1024; F383..F402 Stehen ohne Bewegung; F403 Plc_motion Clip 0.
- Bilder `<sp>/E41a1/R2_302_331.png` (Cut 0: Leon ausserhalb des Bildes) und `R2c_378_417.png` (F384 erstes Cut-3-Bild, Leon an der
  Kurbel, F403..F406 Uebergang, Kurbeln). Kein Abheben, kein Gleiten.
- R4 (NACHHER) Anlauf im RENNEN (`RE15_PLAYER_POS=2600,-800,1024,1 RE15_INPUT_SCRIPT=XU0.2,(XUA0.1,XU0.1)x4,XUA0.1,XU1.0`, sonst wie
  R2, Lauf `R4_renn`): `F307 @0x0A5C`, `F326 @0x0A62`, `F383 @0x0A8C`, `F403 @0x0A9C`; anim.txt Renn-Clip (mo=100 Clip 0, 22 Bilder)
  waehrend msg 0 auf Bild 6, nach dem Luecken-Bild auf Bild 7 eingefroren, F381 `mo=200 Clip 3`; Bilder `<sp>/E41a1/R4_frames.png`
  (Cut 0 ohne Leon, F384 an der Kurbel). Gleiches Muster wie R2.
- Vergleich mit dem Original im Geh-Fall: **nicht moeglich** — im Original-RAM waehrend der Frage steht der Spieler im Gehen
  (Dossier: `o_hg_geh3_slot1.sav` `+4..+7 = [1,1,1,0]`, `+0x94 = 5`, `+0x95 = 2`), das Ja hat die Automatisierung im Geh-Fall
  zweimal nicht angenommen; was das Original danach zeigt, ist ungemessen. Der Port tickt dagegen zwischen msg 0 und msg 1 ein
  Bild Spielersteuerung (F328 hier, F261 im Lauf t2 des Dossiers) und schaltet nach der Frage auf Stehen 200 — der Bau-Agent
  nennt das selbst OFFEN ("Mechanismus nicht belegt"), master-AUFTRAG E-N2 (04:52) fordert dafuer Disasm (0x80031de8) + Original-
  Messung. Genau diese Stelle ist "am Anfang, wenn er die Kurbel nutzt" — und der Nutzer kam in Szene (3) im Gehen an (ob die
  Taste im Gehen fiel, gibt das Raster nicht her).

### 3.5 Gegenprobe der Behauptungen zum Original (Stichproben)
- "Add_speed bewegt den Spieler nie": bestaetigt (§2 Nr. 2/3 + RDT-Bytes; Port state.txt R1 197/197 Bilder (3650,-3018)).
- "Sprung zur Kurbel ist Original": bestaetigt (§3.3, v436 -> v437).
- "Standbild vor dem Sprung = Cut_chg laedt neu": Code bestaetigt (§2 Nr. 4); Dauer im Video 17 Videobilder (~8 Spielbilder).
- "Nutzer sah den VORHER-Stand": bestaetigt (§3.1, Diff 86394fc2..116e8b0f ohne Spieler-/Anim-/Render-Code); dass sein
  "herumfliegend" das VORHER-Standbild war, bleibt UNBELEGT — das Standbild (68 sichtbare Bilder vorgebeugt, R3) ist ein
  Einfrieren, kein Fliegen; der Sprung und die Zwischenpose sind Original.

### 3.6 Urteil Punkt 1: **teilweise**
Behoben und am echten Weg gemessen: das Standbild nach einem Umlauf (VORHER R3 Slot 39 ab F403, NACHHER Schleife), Stand- und
Cut-3-Anlauf Bild fuer Bild wie das Original (§3.3). Nicht belegt: (a) welches Bild der Nutzer mit "herumfliegend" meint —
kein Port-Lauf (Gehen/Rennen/Stand/Waffen, VORHER/NACHHER) und kein Original-Video zeigt Leon fliegen; (b) der Geh-Anlauf: der
Port weicht waehrend der Fragen messbar vom Original-RAM ab (Port: 1 Bild Steuerung zwischen msg 0 und msg 1, danach Stehen 200;
Original: Gehen [1,1,1,0]/+0x94=5), das Original nach dem Ja ist ungemessen, der Mechanismus (Kommando 1 @0x80031de8 bzw. die
Meldungs-Folge) nicht disassembliert — "Mechanismus nicht gefunden = NICHT fertig" (CLAUDE.md), master-AUFTRAG E-N2.

## 4. Punkt 2 — Karte B1, vordere Tuer der "Waffen Armory" (ROOM1230 -> ROOM1190 Ostkammer)
Gemeinsam `RE15_DEBUG_JUMP=1230@240 RE15_PLAYER_POS=96,14500,1024,0 RE15_SET_FLAG=4:0xf3,3:0x89,7:0x88,7:0x89
RE15_INPUT_SCRIPT=U<t>,W0.2,S0.1,W1.0,Q0.1 RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.txt
RE15_INV_FB_SHOT=map.bmp RE15_INV_FB_SHOT_AT=120 RE15_MAP_SHOT_PAGE=0 RE15_EXIT_AT=560`; Marker per
`test_r41_kleinkram_b_exe marker map.bmp` (Ring-Texel B<=8, |R-G|<=8, R>=0x18, 13 Texel).

| Lauf | Spieler (state.txt) | VORHER (Basis 116e8b0f) | NACHHER (HEAD) |
|---|---|---|---|
| Leon U0.6 | (96,13150) | **MARKER 137 109** | MARKER 145 106 |
| Leon U1.0 (Lauf endet an der Tuer) | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Elza `RE15_TITLE_SHOT_CHAR=1`, 1231, U1.0 (room1231.rdt, PL04) | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Leon ECHTE Tuer: `U1.0,W0.3,A0.1`, `RE15_INV_OPEN_AT=60#1190` -> `[tuer] Sequenz ... DOOR19 ... 301 Bilder`, room1190.rdt | (19515,-12109) | — | MARKER 145 108 |

Bilder `<sp>/E41a1/K2_full.png` (links VORHER U1.0: Marker am Westende des Gang-Arms an der Hallen-Ecke = "hinterer Teil";
rechts NACHHER: am Ostkammer-Kasten) und `<sp>/E41a1/K2_vor_nach.png` (Ausschnitt x3, VORHER U0.6/U1.0, NACHHER U0.6/U1.0).
Die VORHER-exe zeigt den Nutzer-Befund an genau dieser Stelle (der Marker wandert beim Herantreten an die vordere Tuer bis 15 px
zur Hallen-Marke), NACHHER bleibt er an der Ostkammer-Marke (146,109), nach dem Durchgang (145,108). Leon und Elza gleich.
Nebenwirkung alle Tueren: integration_map_uebergang (-V: 214 Tueren, 151 Uebergaenge auf 12 Blaettern, 4x PASS) gruen; die
Tuer-Listen VORHER/NACHHER hat Abnahme 0 §4.3 verglichen (diff leer), der Code ist seitdem unveraendert
(`git diff ed7bd4ff HEAD -- re15_port/engine/src/karte_r41.c re15_port/engine/src/re15_inv_screen.c` leer).
**Urteil Punkt 2: erfuellt.**

## 5. Punkt 3 — Werkstatt-Tuer ROOM11B1 bei Elza dauerhaft zu (Mechanik seit Nachbesserung 1 neu gestartet -> neu gemessen)
### 5.1 P3a — ECHTER Weg: Elza ROOM1261 -> Tuer Slot 0 -> ROOM11B1, Szene "Sherry!? / Where has she gone!?"
`RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=1261@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3 RE15_PLAYER_POS=27650,9450,1024,1
RE15_PRESS=square@300,square@310 RE15_SCD_TRACE=1 RE15_MSG_LOG=1 RE15_CAM_TRACE=1 RE15_STATE_LOG=state.txt RE15_FRAMEDUMP=0-700/10:fd_
RE15_EXIT_AT=700#11b1` (Flags = Zustand nach 10B1 sub04 @0x0195E und 2001 sub04 @0x01958 `22 03 46 01`, selbst im Dump
`<sp>/E41a1/room2001_scd.txt` gesehen), Laeufe `P3a_szene_nach` / `P3a_szene_basis`.
- debug.log NACHHER: `loaded room1261.rdt` -> `[tuer] Sequenz fertig: 331 Bilder` -> `loaded room11b1.rdt` ->
  `[scd] thread start slot=10 first_op=0x4B` -> `[wk11b1-r41] ROOM11B1 (3,0x83)=1: Ereignis 24 im Init-Lauf hinter sub00` ->
  Init-Lauf F300: slot 1 sub00 `@0x10F6 06, @0x10FA 21, @0x1102 23, @0x110E 4B, @0x1112 4B, @0x1116 37, @0x111A 39, @0x111E 2E,
  @0x1122 34, @0x1126 07, @0x115A 01`, danach **im selben Lauf** slot 10 `4B 00 4B 00 37 39 2C 06 23 06 23 01` (= k_wk11b1_prog);
  `[tuersperre] F300 ROOM11B1: Slot 1 -> Text msg 12 (immer zu)` (Runde 39 P); Szene slot 10 `first_op=0x22` (sub03 @0x1438),
  `[msgtext] id=10 "Elza: Sherry!?"`, `id=11 "Elza: Where has she gone!?"`. VORHER: derselbe sub00-Zweig, kein Ereignis 24.
  (Kosmetik: der Trace druckt die Programm-Adresse relativ zur RDT als `@0x6E06B280` — das Programm liegt ausserhalb der RDT.)
- Kameras (state.txt, letzter Raum-Abschnitt): NACHHER `F1:C6 F187:C5 F235:C4 F278:C3 F301:C2`, Ende F700 Cut 2, Elza
  (-18192,-20453); VORHER `... F278:C3 F301:C8`, Ende Cut 8.
- Bild `<sp>/E41a1/P3a_tuer_zoom.png` (F690, x3): VORHER Tuerblatt offen aufgeschwungen, NACHHER geschlossenes Tuerblatt;
  Uebersicht `<sp>/E41a1/P3a_ende_vor_nach.png`.
### 5.2 P3b — Elza (Garagenseite) gegen die Tuer, Aktionstaste
`RE15_DEBUG_JUMP=11b1@240 RE15_SET_FLAG=3:0x83,4:0xf3 RE15_PLAYER_POS=-20200,-16230,2048,0 RE15_INPUT_SCRIPT=U1.5,W0.3,A0.1
(Start 262, Basis spiel) RE15_FRAMEDUMP=258-380/2:fd_ RE15_EXIT_AT=380`: state.txt F260 (-20200) -> F263 -20350 -> F266 -20575 ->
F269 -20800 -> **F272..F380 (-20942,-16230) blockiert** (Zellkante -21410 + Radius), cam 2; `[msgtext] room=11b1 id=12 "The door
is locked."` (F320.. `msg(a=1 id=12)`), `EXIT_AT: Bild 380 in Raum 11b1` (kein Raumwechsel). Bild `<sp>/E41a1/P3b_tuer.png`.
### 5.3 P3c — "dauerhaft": Elza von der WERKSTATT-Seite (Westen) gegen die Tuer
`RE15_PLAYER_POS=-24000,-16230,0,0` (Blick +x), `U2.0,W0.3,A0.1`, sonst wie P3b: NACHHER F280..F399 **(-22618,-16230) blockiert**
(Zellkante -22150 - Radius), cam 0, `[msgtext] id=12 "The door is locked."`; VORHER laeuft Elza durch (F290 -21825, F300 -21075,
F320 -19768, cam 7 -> 8). Die Tuer ist also von beiden Seiten zu; bei (3,0x83)=0 bleibt der Else-Zweig des Originals (Riegel
unit_r41_kleinkram_b_11b1_original gruen), Leon 11B0 unberuehrt (integration_r36_szenenleon/r38_werkstatt11b0/r39_stimmen_11b0 gruen).
### 5.4 Grenze der Messung
Der Abschnitt 2001 -> 1261 wurde nicht gefahren (der Zustand nach der Sherry-Szene 2001 sub04 kommt per RE15_SET_FLAG); der
Uebergang 1261 -> 11B1 ist die echte Tuer. Die Szene endet wie vorher mit Elza bei (-18192,-20453) (Abnahme 0 §5.4, nicht Punkt 3).
**Urteil Punkt 3: erfuellt.**

## 6. Gates / Vertrag
| Gate | Urteil | Beleg |
|---|---|---|
| Bau | haelt | `local_build.sh configure` + getrennt `build` -> LOCAL-BUILD-OK, "ninja: no work to do" (§0); exe md5 beeca32b2e23c31edf7ad06a2073c5db |
| Tests | haelt | 10/10 Spur, 217/217 Unit-Gruppe, 27/27 Integration (§1, unten); die 4 neuen Integrations-Riegel fallen mit der VORHER-exe aus dem Mechanismus durch |
| RE-Gate | haelt | 5 Stichproben selbst disassembliert/gelesen (§2): Sprungtabelle @0x800744a8, Work_set @0x80040d2c, Add_speed @0x80040f40, Cut_chg @0x800402a0 + Zeichner @0x80021530, RDT-Bytes ROOM2020 sub02 / ROOM11B1 @0x0EC8/@0x10F6/@0x112A / ROOM10B1 @0x0195E / ROOM2001 @0x01958. Neue Konstanten der Nachbesserung 1 nur in Riegeln (Lage (3650,-3018) = Pos_set @0x0A8C + Kollision, Marker 143..149 = Marke re15_map_zones.h:987, x >= -20950 = Zelle @0x00EC8 + Radius), jeweils mit Beleg im Kommentar |
| Kein Raten | haelt | Branch-Diff `re15_port` auf deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/hack/vermutlich/ungefaehr/approx/geschaetzt/"sieht .. aus": 0 Treffer |
| Fix erklaert Befund | Punkt 2/3 ja, Punkt 1 teilweise | Punkt 2: VORHER 137/130 px an derselben Stelle reproduziert, NACHHER 145 (§4); Punkt 3: VORHER Cut 8 + Durchlauf, NACHHER Cut 2 + blockiert von beiden Seiten (§5); Punkt 1: erklaert das Standbild, nicht das "Fliegen" (§3.6) |
| Pfade | haelt | `git diff --name-only $(git merge-base HEAD master) HEAD`: engine/src karte_r41.c, plcloop_r41.c, werkstatt11b1_r41.c (neu), player_common.c, re15_inv_screen.c, scd_room_setup.c (Haken); include 3 neue Header; tests/integration/test_r41_kleinkram_b.cmake (-P-Skript, cmake_minimum_required vorhanden), tests/unit/probes/r41_kleinkram_b.cmake (GLOB), 4 Test-Quellen; analysis/befunde_runde41. Nichts unter release/, platform/android/, shared_assets/, pri/, synchro/, keine Test-CMakeLists |
| Ressourcen | haelt | kein Bank-9-Bit (E = 120 ungenutzt, test_r36_kartenbits.c unberuehrt), keine neue Nachricht (msg 12 Original), "Ereignis 24" ist seit Nachbesserung 1 nur noch ein Name — das Programm startet direkt im ersten freien Ereignis-Slot (Trace: slot 10 im Init-Lauf, Evt_end im selben Lauf, die Szene sub03 belegt slot 10 danach) |
| Gemeinsame Dateien | haelt, mit Hinweis | scd_vm.c jetzt bytegleich zu master (Hinweis H1 der Abnahme 0 erledigt); scd_room_setup.c +2 (E zugeteilt; Zeile hinter re15_szl_init_lauf statt am Blockende — begruendet: muss vor scd_vm_tick im Init-Lauf stehen, Muster Runde 36 J); player_common.c +2 und re15_inv_screen.c +3/-1 stehen nicht in VERTRAG §1.3 — kleine, markierte Haken ("Runde 41 Spur E") an der Stelle des Defekts; bei der Integration beachten |
| Riegel je Punkt | haelt | Punkt 1 unit kurbel_{laenge,schleife} + integration_kurbel; Punkt 2 unit karte_pin + integration_karte; Punkt 3 unit 11b1_{programm,zu,original} + integration werkstatt_{szene,tuer}; parallel-fest (Zufalls-Laufordner + exe-Kopie je Lauf), geerbte RE15_* geloescht, keine .exe-/Backslash-Pfade, `_DEFAULT_SOURCE` im Auswerter |
| Nebenwirkungen beide Szenarien | haelt | Plc_flg(0,4)-Szenen Leon/Elza (1011/1021/1150/11B0/3091 u.a.) in Unit 217/217 und Integration 27/27 gruen; Karte Leon + Elza gemessen; 11B1 nur bei (3,0x83)=1, Leon 11B0 unberuehrt |

Integration 27/27 (`<sp>/E41a1/ctest_integ.log`, Regex `<sp>/E41a1/integ_regex.txt`, -j 2, 1460 s, CTEST_EXIT=0): r36_szenenleon,
r38_werkstatt11b0, r39_karten, r36_kartenitems_bild, r39_stimmen_11b0, r36_szene1150, r38_elzatoene_szene1021, r37_roy_szene1011,
r39_stimmen_1150, r37_roy_szene1021, r39_elza_karte, r39_johnruf1041_tuer_t11b1, _t11b1d, r38_tagebuch_karte,
r37_elzaems_elza3091, r37_elzaems_elza11b1, map_uebergang (-V: 214 Tueren, 4x PASS), map_raum_live, r35_karte,
r35_karte_fahrstuhl, r36_elza_fenster/irons/unlock/schuss/stoehnen, r35_cut1150, r36_karte. Kein roter Fenster-Haken.

### 6.1 Neu im master-AUFTRAG (04:52, nach dem Bau) — gemessen, damit die naechste Nachbesserung einen Ausgangswert hat
- **E-N1 Kamera-Flackern ROOM2020** (Laeufe `<sp>/E41a1/runs/EN1_kamera_{nach,basis}`, `RE15_DEBUG_JUMP=2020@240
  RE15_SET_FLAG=3:0x72 RE15_PLAYER_POS=9000,-3200,2048,1 RE15_STATE_LOG RE15_CAM_TRACE RE15_EXIT_AT=300`): im 2020-Abschnitt
  F1..F300 wechselt `cam=` **299-mal** (2,0,2,0,...), Lage (9000,-3200) unveraendert — NACHHER und VORHER gleich, also nicht von
  dieser Spur verursacht, aber offen.
- **E-N2 Geh-Stopp waehrend der Frage**: §3.4 (Port gemessen; Original nach dem Ja und Mechanismus offen). Stichprobe dazu:
  Message_on = Sprungtabelle [0x2B] = 0x800404f4 (`lbu a2,1(v0)` Id, `addiu v0,v0,4`, `jal 0x80027e68`, Rueckgabe 1 = weiter);
  das Warten auf das Schliessen haengt also nicht im Opcode selbst — wer zwischen msg 0 und msg 1 den Spieler tickt, ist zu belegen.

## 7. Maengel
1. **Punkt 1 — das Nutzer-Symptom "herumfliegend" ist weiterhin nicht nachgewiesen.** Behoben ist das gemessene Standbild
   (VORHER R3: Slot 39 ab F403, 68 sichtbare Bilder in Cut 3; NACHHER R1: Schleife), Stand- und Cut-3-Anlauf sind original-gleich
   (§3.3). Weder Port (R1-R4, VORHER/NACHHER) noch Original-Video zeigen ein Fliegen. Die Rueckfrage an den Nutzer ist nach dem
   Orchestrator-Hinweis 2026-10-24 03:21 erst zulaessig, wenn ALLE Anlauf-Varianten original-gleich belegt sind — das ist fuer den
   Geh-/Renn-Anlauf nicht der Fall (Mangel 2). Nachpruefbar: §3.1-§3.6.
2. **Punkt 1 / master-AUFTRAG E-N2 — Geh-/Renn-Anlauf nicht original-gleich belegt, Mechanismus offen.** Port: zwischen
   Message_on 0 @0x0A5C und Message_on 1 @0x0A62 ein Spielbild Spielersteuerung (R2 F328 `msg(a=0)`, Geh-Clip 5 Bild 9 -> 10;
   R4 Renn-Clip 0 Bild 6 -> 7), nach dem Ja Wechsel auf `mo=200` Clip 3 (1 Bild) F381 und 20 Bilder regungslos an der Kurbel;
   Original-RAM waehrend der Frage: Gehen `[1,1,1,0]`, +0x94 = 5, +0x95 = 2 (Bau-Agent, `o_hg_geh3_slot1.sav`); das Original nach
   dem Ja ist ungemessen, der Geh-Stopp (Kommando 1, Tabelle 0x80073f90[1] = 0x80031de8) bzw. wer zwischen zwei Message_on den
   Spieler tickt (Message_on @0x800404f4 kehrt sofort mit 1 zurueck), ist nicht disassembliert. Der Nutzer kam in Szene (3) seines
   befund.log im Gehen an die Kurbel (§3.1; ob die Taste im Gehen fiel, ist offen). Nachpruefbar: `<sp>/E41a1/runs/R2_geh`, `R4_renn` (anim.txt/state.txt).
3. **master-AUFTRAG E-N1 (Orchestrator-Abnahmepunkt, nach dem Bau erteilt) — Kamera-Flackern ROOM2020 offen.** Bei (9000,-3200)
   wechselt der Port 299-mal in 300 Bildern zwischen Cut 2 und 0 (`<sp>/E41a1/runs/EN1_kamera_nach/state.txt`), VORHER genauso
   (`EN1_kamera_basis`) — vorbestehend, nicht durch diese Spur verursacht, aber als Punkt der Spur E offen.

Hinweise (keine Maengel des Codes dieser Spur):
- H1 Vertrag §1.3: player_common.c (+2) und re15_inv_screen.c (+3/-1) sind fuer keine Spur gelistet; kleine, markierte Haken an der
  Stelle des Defekts — bei der Integration beachten. scd_vm.c ist erledigt (bytegleich zu master).
- H2 (Spur M, unveraendert): in allen Port-Bildern der Cut-3-Kurbel liegt die Reling ueber Leons Beinen (`<sp>/E41a1/P_cut3_k14_25.png`,
  unten), im Original steht er davor — MASKS/ROOM2020.MSK, nicht Code dieser Spur; den Befund des Nutzers (befund.log `masken=0`)
  erklaert es nicht.
- H3 Nachlade-Standbild: das Original haelt vor dem Sprung ~8 Spielbilder das alte Bild (Cut_chg @0x800402f4 -> Zeichner
  @0x80021530/@0x80021558), der Port 2 Bilder; CD-Ladezeit, betrifft jeden Cut_chg (vom Bau-Agenten als OFFEN gefuehrt).
- H4 Kosmetik: der SCD-Trace druckt das Port-Programm des Ereignisses mit RDT-relativen Unsinnsadressen (`@0x6E06B280`).

## 8. Urteil
| Punkt | Urteil | Kern |
|---|---|---|
| 1 Kurbel ROOM2020 | **teilweise** | Standbild behoben (VORHER/NACHHER am echten Weg 2050 -> 2020 gemessen), Stand-/Cut-3-Anlauf Bild fuer Bild wie das Original-Video; Geh-/Renn-Anlauf weicht vom Original-RAM ab und ist nach dem Ja ungemessen (E-N2); "herumfliegend" nirgends reproduziert |
| 2 Karte B1, vordere Tuer 1190 | **erfuellt** | VORHER 137/130 px zur Hallen-Seite reproduziert, NACHHER 145 an der Ostkammer, Leon + Elza + echte Tuer |
| 3 Werkstatt-Tuer 11B1 (Elza) | **erfuellt** | echter Weg 1261 -> 11B1 mit Szene: Tuerblatt zu (Cut 2 statt 8, Bild), blockiert von Garage UND Werkstatt, "The door is locked.", kein Raumwechsel |

Gates: halten (Hinweis H1). **Gesamt: NICHT bestanden** — Punkt 1 ist nicht "erfuellt" (Maengel 1 und 2); dazu der nach dem Bau
erteilte Spur-E-Punkt E-N1 (Mangel 3).
