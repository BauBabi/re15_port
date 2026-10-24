# Runde 41 — Spur E "kleinkram_b" — Abnahme 2 (unabhaengig, nach Nachbesserung 2)

Pruefer: Abnahme-Agent 2, 2026-10-24 (Beginn 09:55 laut `date`). Baum `.claude/worktrees/r41_kleinkram_b`, Zweig
`r41/kleinkram_b`, HEAD 82cd04ec (Code-Stand 11b8f867; Basis master 116e8b0f). `<sp>` = Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`
(derselbe Ordner wie Bau-Agent / Abnahme 0 / Abnahme 1); eigene Laeufe unter `<sp>/E41a2/`.

Status dieses Berichts: IN ARBEIT (fortlaufend gespeichert).

## 0. Gelesen
- AUFTRAG.md (Baum und master af7e0e8a; fuer Spur E gelten die Orchestrator-Auftraege E-N1/E-N2 vom 2026-10-24 04:52,
  danach kein weiterer Spur-E-Eintrag — insbesondere KEIN Orchestrator-Entscheid zum Renn-Stopp), VERTRAG.md, UEBERGABE.md,
  Dossier E_kleinkram_b.md (ganz, inkl. "Nachbesserung 2"), E_abnahme_0.md, E_abnahme_1.md.
- `git log --oneline master..HEAD` (15 Commits der Nachbesserung 2: d7c57d7d .. 82cd04ec), `git diff 116e8b0f HEAD --stat`
  (29 Dateien), Code-Diff `re15_port` vollstaendig gelesen (engine/src kamerahalt_r41.c, msgtakt_r41.c, Haken msg_common.c,
  scd_vm.c; platform/pc bildhalt_r41_pc.c, Haken main.c, render_pc.c; Tests). Seit Abnahme 1 (753652fc) aendert sich am Code nur
  Nachbesserung 2; karte_r41.c, werkstatt11b1_r41.c, plcloop_r41.c sind unveraendert.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; getrennt `build` -> "ninja: no work
  to do", `=== LOCAL-BUILD-OK (build)` (`<sp>/E41a2/configure.log`, `build.log`). re15_pc.exe md5 486f77a7d870cf73c1bdf542bf40cbfb
  (= Endstand des Bau-Agenten 11b8f867) -> Messkopie `re15_pc_E41a2.exe`. VORHER-exe = `re15_pc_E41a1.exe` (753652fc, md5
  beeca32b2e23c31edf7ad06a2073c5db, Abnahme 1).

## 1. Tests (gezielt, im Spurbaum, kein test/all)
- `ctest --test-dir re15_port/build -R "r41_kleinkram_b" -j 2 --timeout 900 --output-on-failure` (09:56-09:58):
  **100% tests passed, 0 tests failed out of 16**, CTEST_EXIT=0 (`<sp>/E41a2/ctest_spur.log`): 9 unit (11b1 x3, karte_pin,
  kurbel x2, takt_{msgtakt,kamerahalt,frage}) + 7 integration (kurbel 36 s, werkstatt_szene 52 s, renn 41 s, karte 58 s,
  werkstatt_tuer 24 s, geh 26 s, kante 12 s). `-V` fuer takt_*: msgtakt 11/11, kamerahalt 6/6, frage 5/5 PASS (inkl. Gegenprobe
  "alte Reihenfolge: Luecken-Bild 1, Pos_set +2").
- Weitere Gruppen: §6.

## 2. RE-Gate — Stichproben SELBST disassembliert (re15_disasm.py, info/Re1.5/PSX.EXE = Auslieferungsstand)
1. **Present FUN_8002137c** (`dis 0x800214f0 76`): @0x800214f8 `lh 0x800b0fe4` (angezeigt) / @0x80021500 `lbu 0x800afbb5`
   (angefordert) / @0x80021508 `beq` / @0x80021514 `sb 1,0x800b5457`; @0x80021530 `lbu 0x800b5457`, @0x80021538 `beq` ->
   @0x80021544 `lw 0x800aca38 & 0x200000` @0x80021550 `bne` -> @0x80021558 `jal 0x80021bbc` + @0x80021560 `j 0x800215fc`
   ueberspringt die drei `jal 0x80068fc4` (@0x800215bc/@0x800215d0/@0x800215e4) und den Pufferwechsel @0x800215f4 `xori` /
   @0x800215f8 `sb 0x800aca34`; @0x80021618 `sb zero,0x800b5457`. **Bestaetigt.** Nicht im Dossier/Modul erwaehnt: zwei
   weitere Tore vor dem Apply — @0x8002151c `lbu 0x800b536c` (Bildschirm-Modus, Schreiber nur FUN_80021634 @0x80021638;
   != 0 -> @0x80021584 direkt zeichnen) und @0x80021544-@0x80021550 (`aca38 & 0x200000` -> zeichnen statt Apply). Im
   Spielbetrieb sind beide 0 (FUN_80021634(0,0) bei der Raum-Freigabe @0x8001dadc; ein Schreiber von aca38 Bit 0x200000 ist im
   EXE-Dump nicht zu finden) -> kein Mangel, Hinweis H2.
2. **Message_on** = SCD-Tabelle @0x800744a8 [0x2B] = 0x800404f4 (`table`): @0x8004050c/@0x80040510 pc += 4, @0x80040518
   `jal 0x80027e68` mit a3 = pc[2..3] << 16 (@0x8004051c), @0x80040520 `ori v0,zero,1` = Rueckgabe 1 (kein Parken).
   **Bestaetigt.** Skript-Lauf FUN_8003f038: @0x8003f040 `lw g_pauseflags`, @0x8003f044 `lui v1,0x200`, @0x8003f048 `and`,
   @0x8003f04c `bne` -> @0x8003f090 (Ende). **Bestaetigt.**
3. **Bildreihenfolge**: Hauptschleife @0x80020de4 `jal 0x800298b0` (Task-Plan), @0x80020df8 `beq` (aca38 & 0x10000) ->
   @0x80020f34 `jal 0x80021a0c` (Letterbox), @0x80020f3c `jal 0x80010000`, @0x80020f44 `jal 0x80021880` (Blenden-Takt),
   @0x80020f4c `jal 0x8002137c` (Present). Spiel-Task: @0x8001cdec `jal 0x8003f038` (Skript), @0x8001ce0c `jal 0x80031c44`
   (Spieler), @0x8001ce1c `jal 0x800436a8`, @0x8001ce2c `jal 0x80019e20`, @0x8001ce34 `jal 0x8001db28` (Modal). FUN_80010000:
   @0x80010044 `jal 0x800280b4` (Nachrichten-Takt, @0x800280dc `jal 0x80028134`). **Bestaetigt** — mit einer Ergaenzung, die
   Dossier und Modul nicht nennen: FUN_80010000 laeuft nur bei einem Pufferwechsel seit dem letzten Aufruf (@0x8001001c `lbu
   0x800a73a4` / @0x80010020 `lbu 0x800aca34` / @0x80010028 `beq` -> @0x8001012c Ruecksprung, @0x80010040 `sb` merkt den Index).
   Im Bild NACH einem Apply-Bild (kein Pufferwechsel @0x800215f8) faellt der Nachrichten-Takt im Original also aus. Fuer die
   gemessenen Kurbel-Faelle ohne Wirkung (keine Nachricht offen); Hinweis H3.
4. **Renn-Stopp** (OFFEN des Bau-Agenten, Mechanismus geprueft): Tabellen @0x80073f90 [1] = 0x80031de8, DECIDE @0x80073fb0
   [1] = 0x800322e8 / [2] = 0x80032604, ANIMATE @0x80073ff0 [0] = 0x80032038 / [2] = 0x80032778. DECIDE Rennen @0x800326e8
   `sltiu +6,2`, @0x800326fc `andi 0x201` / @0x80032700 `bne` -> @0x8003272c `sb 2,754(s0)` (+6 = 2); @0x80032738/@0x80032744/
   @0x80032750 Richtungen -> 0x101/0x401/0x301 @0x80032760 `sw 0x800aca58`. ANIMATE Fall 2 @0x800328f8 +6 = 3, @0x80032904
   `sb 3,0x800acae8` (Clip 3), @0x80032910 `sb 7,0x800acae3`; Fall 3 @0x80032928 `jal 0x8001f314`, @0x8003293c Tempo
   (0x800acae0) -= 40, @0x8003294c `bgez`, @0x80032958 `sw 1,0x800aca58` (Stehen), @0x8003295c `jal 0x800245d8` (Vorschub).
   Renn-Tempo-Byte @0x80073f24 = 0xc8 (200), Drehung @0x80073f25 = 0x48. **Bestaetigt: das Original hat einen Renn-Stopp mit
   Auslauf, der Port nicht.**
5. **Blenden-Takt FUN_80021880** (`dis 0x80021880 100`): @0x8002191c `lhu` Pegel / @0x80021920 `lhu` Schritt / @0x80021928
   `addu` / @0x8002192c `sh` — der Pegel jedes aktiven Kanals laeuft JEDEN Durchlauf weiter, auch im Apply-Bild (Aufruf
   @0x80020f44 liegt VOR dem Present @0x80020f4c). Siehe Mangel 3.
- Wortsuche im Diff: §6.

## 4. Punkt 2 — Karte B1, vordere Tuer der "Waffen Armory" (ROOM1230 -> ROOM1190 Ostkammer) — neu gemessen
Gemeinsam `RE15_DEBUG_JUMP=1230@240 RE15_PLAYER_POS=96,14500,1024,0 RE15_SET_FLAG=4:0xf3,3:0x89,7:0x88,7:0x89
RE15_INPUT_SCRIPT=U<t>,W0.2,S0.1,W1.0,Q0.1 RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.txt
RE15_INV_FB_SHOT=map.bmp RE15_INV_FB_SHOT_AT=120 RE15_MAP_SHOT_PAGE=0 RE15_EXIT_AT=560`, Marker per
`test_r41_kleinkram_b_exe marker map.bmp`; Laeufe `<sp>/E41a2/runs/P2_*`.

| Lauf | Spieler (state.txt) | BASIS 116e8b0f | NACHHER (HEAD) |
|---|---|---|---|
| Leon U0.6 | (96,13150) | **MARKER 137 109** | MARKER 145 106 |
| Leon U1.0 (Lauf endet an der Tuer) | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Elza `RE15_TITLE_SHOT_CHAR=1`, 1231, U1.0 (loaded room1231.rdt) | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Leon ECHTE Tuer `U1.0,W0.3,A0.1`, `RE15_INV_OPEN_AT=60#1190`: `[tuer] Sequenz ... DOOR19 ... 301 Bilder`, room1190.rdt | (19515,-12109) | — | MARKER 145 108 |

Ostkammer-Marke (146,109), Hallen-Marke (127,115) (re15_map_zones.h:986/987). BASIS wandert beim Herantreten an die vordere
Tuer bis 16 px zur Hallen-Seite (= "Sprung zum hinteren Teil, obwohl man gar nicht da ist"), NACHHER bleibt der Marker an der
Ostkammer, Leon und Elza gleich, nach dem Durchgang (145,108). Bild `<sp>/E41a2/P2_karte_vor_nach.png` (Ausschnitt x3; links
BASIS U0.6/U1.0, rechts NACHHER U0.6/U1.0). Die Nachbesserung 2 (Bildhalt/Nachrichten-Takt) aendert daran nichts.
**Urteil Punkt 2: erfuellt.**

## 5. Punkt 3 — Werkstatt-Tuer ROOM11B1 bei Elza dauerhaft zu — neu gemessen
### 5.1 P3a — ECHTER Tuerweg: Elza ROOM1261 -> Tuer Slot 0 -> ROOM11B1, Szene "Sherry!? / Where has she gone!?"
`RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=1261@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3 RE15_PLAYER_POS=27650,9450,1024,1
RE15_PRESS=square@300,square@310 RE15_SCD_TRACE=1 RE15_MSG_LOG=1 RE15_CAM_TRACE=1 RE15_STATE_LOG=state.txt RE15_KAMERAHALT_LOG=1
RE15_FRAMEDUMP=0-700/10:fd_ RE15_EXIT_AT=700#11b1` (Flags = Zustand nach 10B1 sub04 @0x0195E und 2001 @0x01958), Laeufe
`<sp>/E41a2/runs/P3a_szene_{nach,basis}`.
- NACHHER debug.log: `loaded room1261.rdt` -> `[tuer] Sequenz fertig: 331 Bilder` -> `loaded room11b1.rdt` -> `[wk11b1-r41] ROOM11B1
  (3,0x83)=1: Ereignis 24 im Init-Lauf hinter sub00 — Werkstatt-Tuer bleibt zu` -> `[msgtext] room=11b1 id=10 "Elza: Sherry!?"`,
  `id=11 "Elza: Where has she gone!?"`. Kameras (`segment`): **2,3,4,5,6, letzte 2** (nie 7/8), Ende F700 Elza (-18192,-20453).
  BASIS: kein Ereignis 24, Kameras 3,4,5,6,8, **letzte 8** (F301 Cut 8).
- Bild F690 `<sp>/E41a2/P3a_tuer_zoom.png` (x3; links BASIS: Tuerblatt aufgeschwungen, rechts NACHHER: geschlossene Tuer mit
  Klinke), Uebersicht `P3a_ende_basis_nach.png`. 5 gehaltene Bilder in der Szene (Kamerawechsel), Szenenfolge unveraendert.
### 5.2 P3b — Elza von der Garagenseite gegen die Tuer, Aktionstaste
`RE15_DEBUG_JUMP=11b1@240 RE15_SET_FLAG=3:0x83,4:0xf3 RE15_PLAYER_POS=-20200,-16230,2048,0 RE15_INPUT_SCRIPT=U1.5,W0.3,A0.1`
(Start 262): F262 -20275 -> F270 -20875 -> **F272..F380 (-20942,-16230) blockiert**, cam 2, ab F320 `msg(a=1 id=12)`,
`[msgtext] room=11b1 id=12 "The door is locked."`, `EXIT_AT: Bild 380 in Raum 11b1` (kein Raumwechsel).
### 5.3 P3c — "dauerhaft": Elza von der WERKSTATT-Seite gegen die Tuer
`RE15_PLAYER_POS=-24000,-16230,0,0`, `U2.0,W0.3,A0.1`: NACHHER F280..F399 **(-22618,-16230) blockiert**, cam 0, msg 12;
BASIS laeuft durch (F290 -21825, F300 -21075, F320 -19768, Kameras 7 -> 8).
### 5.4 Grenze
Der Abschnitt 2001 -> 1261 wurde nicht gefahren (RE15_FIRE_AOT feuert nur einmal; der Zustand nach 2001 sub04 kommt per
RE15_SET_FLAG), der Uebergang 1261 -> 11B1 ist die echte Tuer. Code von Punkt 3 seit Abnahme 1 unveraendert.
**Urteil Punkt 3: erfuellt.**

## 3. Punkt 1 — Kurbel ROOM2020 ("komische herumfliegende Animation ... am Anfang wenn er die Kurbel nutzt") mit E-N1/E-N2
Laeufe `<sp>/E41a2/runs/<lauf>/` (cmd.txt, debug.log, state.txt, anim.txt, fd_*.ppm), Helfer `<sp>/E41a2/run.sh` (exe-Kopie,
eigener Ordner, geerbte RE15_* geloescht), Pixelvergleich `<sp>/E41a2/fdiff.py`. Gemeinsam `RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1` + `RE15_SCD_TRACE=1 RE15_ANIM_TRACE=anim.txt RE15_MOTRACE=1
RE15_STATE_LOG=state.txt RE15_MSG_LOG=1 RE15_CAM_TRACE=1 RE15_KAMERAHALT_LOG=1`. NACHHER = re15_pc_E41a2.exe, VORHER =
re15_pc_E41a1.exe (753652fc).

### 3.1 Geh-Anlauf (E-N2), Lauf `P1_geh_{nach,vorher}`
`RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=2600,-1500,1024,1
RE15_INPUT_SCRIPT=U0.3,(UA0.1,U0.1)x4,UA0.1,U1.0 (Start 300, Basis spiel) RE15_PRESS=square@380,square@430,square@470,square@510
RE15_FRAMEDUMP=296-430/1:fd_ RE15_EXIT_AT=480#2020`.

| | NACHHER | VORHER | Original (GDB `<sp>/E41n2/o_geh1.txt`, selbst gelesen) |
|---|---|---|---|
| Message_on 0 @0x0A5C + Evt_next | F310 | F310 | n9 (`t2@0A61`) |
| Spieler zwischen msg 0 und msg 1 | `[mot]` F309..F381 mo 105 **af 9 durchgehend** | F328 af 9 -> **10** (Luecken-Bild) | n9..n94 `mo=5/9` durchgehend |
| Message_on 1 @0x0A62 | F329 **+ Evt_next @0x0A66 im selben Takt** | F329 geparkt, F382 erneut + Evt_next | n47 `t2@0A67` |
| Frage zu | F381 (state `msg(a=0)`) | F380/F381 | n94 |
| Ifel_ck @0x0A68 .. Pos_set @0x0A8C .. Sleep @0x0A98 | **F382 in einem Takt** | F383 | n95 (`t2@0A99`, pl 3650,-2925) |
| Spieler im Pos_set-Bild | F382 mo 200 af 0 frac 7 (3650,-3018) | F381 mo 200 an der ALTEN Stelle, Sprung F383 | n96 `cmd 1/0/1/0 mo=3/0 pl=3650,-3018` (Takt von n95) |
| gehaltenes Bild | `[kamerahalt] F382 ... pend=1 wv0A=3 cam=3 -> HALTEN`, `fd_000382.ppm (gehalten)` = F381 (0 px) | keins | n95 -> n96 34 VSyncs (Apply + CD-Laden) |
| Plc_motion @0x0A9C | F402 = Pos_set + 20 | F403 | n115 = n95 + 20 |

=> Der Geh-Anlauf ist in Bildfolge und Zustand original-gleich (bis auf die Dauer des Apply-Standbilds: Port 1 Spielbild,
Original CD-Ladezeit). Pixel: F381/F380 1411 px (Text weg), F382 = F381, F383 Cut 3.

### 3.2 Stand-Anlauf, Lauf `P1_stand_{nach,vorher}` (`RE15_PLAYER_POS=2600,-2800,1024,1 RE15_EQUIP=3 RE15_PRESS=square@330,..@460`)
NACHHER: Idle mo 212 **af 49 durch beide Nachrichten**, Frage zu F461, Pos_set F462 (= +1, gehalten), Idle laeuft weiter (af 50..69),
Plc_motion F482 (= +20). VORHER: F458..F462 mo 211/200/211 waehrend der offenen Frage, Pos_set F463 (+2), Plc_motion F483.
Original (Dossier o_stand1, Bau-Agent): Sway laeuft weiter, Pos_set im Bild nach der Bestaetigung — gleich.

### 3.3 Weg des Nutzers 2050 -> Tuer Slot 1 -> 2020 (Cut 3), Lauf `P1_weg2050_{nach,vorher}`
`RE15_DEBUG_JUMP=2050@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_FIRE_AOT=1@30#2050 RE15_INPUT_SCRIPT=L0.36,U3.8,(LA0.034,L0.034)x8,W2.0
(Start 60) RE15_PRESS=square@260,..@380 RE15_FRAMEDUMP=150-420/1:fd_ RE15_EXIT_AT=600#2020`.
NACHHER: `[fire-aot] slot=1 at F30`, msg 0 F186, msg 1 F262 (+Evt_next), F342 Ifel_ck..Sleep, F362 Plc_motion, F540 Plc_ret;
gehaltene Bilder F102 (Zone Cut 0 -> 2), F150 (2 -> 3), F342 (Cut_chg 3 auf denselben Cut, @0x800402f4), F470 (Cut_chg 4),
F540 (Cut_chg 3). VORHER: msg 1 geparkt F262 -> F342, Pos_set F343. Bildbogen `<sp>/E41a2/P1_weg_nach_338_349.png`:
F338..F340 Frage, F341 Frage zu (Leon an der Anlaufstelle), **F342 = F341** (gehalten, 0 px), F343 Leon an der Kurbel, Balken
setzen ein (VORHER `P1_weg_vorher_338_349.png`: F342 192 px Posenwechsel an der alten Stelle, Sprung F343). Der Sprung selbst ist
Original (Abnahme 1 §3.3: Video v436 -> v437).

### 3.4 Renn-Anlauf, Lauf `P1_renn_nach` (`RE15_PLAYER_POS=2600,-800,1024,1 RE15_INPUT_SCRIPT=XU0.2,(XUA0.1,XU0.1)x4,XUA0.1,XU1.0`)
NACHHER: Renn-Clip mo 100 af 6 durch beide Nachrichten (kein Luecken-Bild), F382 Pos_set, **im selben Bild (3650,-3018) mo 200**
(Stehen), danach unbewegt bis Plc_motion F402. Original (GDB `<sp>/E41n2/o_renn2.txt`, selbst gelesen): n93 Pos_set
(3650,-2925), dann Renn-STOPP `cmd 1/2/3/0 mo=3/0` mit z -3099, -3233, -3327, -3381, -3395, n99 -3369 (`spd` 174 .. -26), Leon
steht und kurbelt bei **(3650,-3369)** (Plc_motion n113). Mechanismus selbst disassembliert (§2 Nr. 4). **Abweichung**: im
Renn-Anlauf fehlt dem Port der 6-Bild-Auslauf und Leon kurbelt 351 Einheiten neben der Original-Stelle. Der Bau-Agent fuehrt das
als OFFEN (Renn-Stopp gebaut 635be823, wieder heraus d4fa9074 wegen unit_r36_zombieki_affen_block); ein Orchestrator-Entscheid
dazu steht NICHT in AUFTRAG.md (master af7e0e8a).

### 3.5 E-N1 Kamera-Kante x 9000, Lauf `EN1_kante_{nach,vorher}`
`RE15_PLAYER_POS=9375,-3200,2048,1 RE15_INPUT_SCRIPT=U0.1667 (Start 60) RE15_FRAMEDUMP=50-200/1:fd_ RE15_EXIT_AT=200#2020`:
state.txt F60..F63 x 9300..9075, **F64..F200 x 9000**, `cam=` 2,0,2,0,... (beide exe, wie das Original o_cam9000: angeforderter
Cut wechselt je Durchlauf). NACHHER: alle Folgebilder F64..F200 pixelgleich (0 px; 137 `HALTEN`, 137 `(gehalten)`); VORHER: ab F65
je Bild ~68 050 px (Hintergrund Cut 0/2 im Wechsel). Original (GDB `<sp>/E41n2/o_cam9000_draw2.txt`, selbst gelesen): auf 9000
39x RELOAD ohne DrawOTag (VSync-Abstand 12/13), nach dem Patch auf 8990 DRAWOTAG im 2-VSync-Takt. => **E-N1 behoben** (Bild steht
wie im Original). Nebenprobe Inventar auf der Kante (`EN1_inv_nach`, START): F64..F94 gehalten, F95..F201 gezeigt (Statusschirm
sichtbar, `<sp>/E41a2/EN1_inv_sheet.png`), ab F202 wieder gehalten — kein haengender Schirm.

### 3.6 Urteil Punkt 1: **teilweise**
Erreicht und selbst gemessen: Schleife statt Standbild (Nachbesserung 1, integration_r41_kleinkram_b_kurbel gruen), Geh-, Stand-
und Cut-3-Anlauf in Bildfolge/Zustand wie das Original (§3.1-§3.3), Kamera-Flackern E-N1 weg (§3.5). Nicht erreicht: (a) der
Renn-Anlauf weicht weiter vom Original ab (§3.4, kein Renn-Stopp) — nach dem Orchestrator-Hinweis 2026-10-24 03:21 bleibt die
Rueckfrage an den Nutzer erst zulaessig, wenn ALLE Anlauf-Varianten original-gleich sind; (b) das Nutzer-Symptom "herumfliegend"
ist in keinem Lauf (Port VORHER/NACHHER, Original) gefunden, der Fix erklaert es nicht nachweislich.

## 6. Gates / Vertrag / Nebenwirkungen
### 6.1 Tests (gezielt)
- Spur: 16/16 (§1). Gegenprobe der neuen Riegel mit der VORHER-exe (753652fc), `cmake -DRE15_PC_EXE=<sp>/E41a2/vorher_riegel/
  re15_pc_vorher.exe ... -DTEIL=<kante|geh|renn> -P tests/integration/test_r41_kleinkram_b.cmake` (`<sp>/E41a2/vorher_riegel/*.log`):
  **alle 3 DURCHGEFALLEN** aus dem Mechanismus — kante "GLEICH 0 von 24", kein "(gehalten)"; geh/renn "Spieler-Pose zwischen
  Message_on 0 und 1: 1 Wechsel", "Pos_set F383, Frage zu F381", "nicht gehalten". NACHHER gruen.
- Unit/Prop/Probe der beruehrten Module (Regex `<sp>/E41a2/unit_regex.txt`: msg/scd/dialog/frage/kamera/cam/cut/pause/freeze/
  render/fade/letterbox/plc/anim/karte/map_/szenenleon/werkstatt/johnruf/elzaems/kartenbits/r41_): **100% tests passed, 0 tests
  failed out of 135** (`<sp>/E41a2/ctest_unit.log`). Hinweis: msgtakt ist ohne `re15_msgtakt_r41_takt_beginn` (PC-main) inaktiv,
  diese Unit-Sonden sehen die neue Bildreihenfolge also nicht — aussagekraeftig sind die Integrations-Riegel mit der echten exe.
- Integration (echte exe), Auswahl nach Beruehrung (Auswahl-Nachrichten/Ja-Nein, Item-Aufnahme, Ruecklese-/Kartenabzug,
  11B0/11B1, Szenen mit Nachrichten + Kamerawechsel, Leon UND Elza; Liste `<sp>/E41a2/integ_liste.txt`, 35 Riegel):
  ERGEBNIS_INTEGRATION
- Logs des Bau-Agenten geprueft (nicht wiederholt): `<sp>/E41n2/ctest_unit_alle2.log` "100% ... out of 925" (07:41, Code d4fa9074;
  11b8f867 aendert nur platform/pc), `ctest_integ_auswahl2.log` "99% ... 1 tests failed out of 127" (integration_r38_dining1141_s),
  `ctest_nach_fix.log` 17/17, `ctest_integ_rest.log` 95/95 — Zahlen wie behauptet.

### 6.2 RE-Gate
| Pruefung | Urteil | Beleg |
|---|---|---|
| Adressen der neuen Konstanten/Mechanik stimmen | haelt | 5 Stichproben selbst disassembliert (§2): Present @0x80021558/@0x80021560 -> @0x800215fc, DrawOTag @0x800215bc/d0/e4, Pufferwechsel @0x800215f4/f8; Message_on @0x800404f4 Rueckgabe 1 @0x80040520, Skript-Freeze @0x8003f040-@0x8003f04c; Bildreihenfolge @0x80020de4/@0x80020f3c/@0x80010044, Spiel-Task @0x8001cdec/@0x8001ce0c/@0x8001ce34; Renn-Stopp 0x80032604/0x80032778; Census 102/102 Auswahl-Message_on mit Skript-Bit und Evt_next (Skript des Bau-Agenten `<sp>/E41n2/choice_census2.py` selbst gelaufen: "Masken {0xffff0000: 69, 0xff800000: 33}", "Folge-Opcode 0x2: 102"); Port-Programme mit Auswahl-Nachricht (nahaufnahmen_r40.c msg 8/1, rolltor_1050.c msg 0/20, codepanel_r37.c) folgen ebenfalls mit Evt_next |
| Neue Konstanten | haelt | in kamerahalt_r41.c / msgtakt_r41.c / bildhalt_r41_pc.c keine Zahl ohne Beleg; Haken `t->pc += 4` = @0x8004050c `addiu v0,v0,4` |
| Kein Raten | haelt | `git diff 116e8b0f HEAD -- re15_port` (+Zeilen) auf deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/hack/vermutlich/ungefaehr/approx/geschaetzt/"sieht .. aus"/Platzhalter: **0 Treffer**; neue getenv nur RE15_FORCE_CUT (Messhaken-Ausnahme), RE15_FRAMEDUMP (Abzug), RE15_KAMERAHALT_LOG (Log) |
| Fix erklaert Befund | E-N1/E-N2 ja, Punkt 1 nein | E-N1: Flackern = Apply-Bild gezeigt, jetzt gehalten (§3.5); E-N2: Luecken-Bild/Parken = Reihenfolge, jetzt original (§3.1); Punkt 1 "herumfliegend": nicht reproduziert (§3.6) |
| Mechanismus vollstaendig | teilweise | Hinweise H1-H3 (§7): Blenden-Takt im gehaltenen Bild, zwei weitere Present-Tore, Nachrichten-Takt-Tor @0x80010028 |

### 6.3 Vertrag
| Pruefung | Urteil | Beleg |
|---|---|---|
| Pfade | haelt | `git diff --name-only 116e8b0f HEAD`: engine/src (kamerahalt_r41, karte_r41, msgtakt_r41, plcloop_r41, werkstatt11b1_r41 neu; Haken msg_common, player_common, re15_inv_screen, scd_room_setup, scd_vm), include 5 neue Header, platform/pc (bildhalt_r41_pc.c neu; Haken main.c, render_pc.c), tests/integration/test_r41_kleinkram_b.cmake, tests/unit/probes/r41_kleinkram_b.cmake (GLOB), 5 Test-Quellen, analysis/. Nichts unter release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine Test-CMakeLists |
| Ressourcen | haelt | kein Bank-9-Bit (120 ungenutzt, test_r36_kartenbits.c unberuehrt), keine neue Nachricht, kein Asset; Ereignis-Faden im ersten freien Slot (seit Nachbesserung 1) |
| Gemeinsame Dateien | **haelt nicht ohne Entscheid** | VERTRAG §1.3 teilt E nur scd_room_setup.c, karte_r39.c/re15_map_zones.c, elzazombies_r39.c, test_r36_kartenbits.c zu. Nachbesserung 2 aendert zusaetzlich scd_vm.c (+2, §1.3: C/F/G), platform/pc/main.c (+9/-1 an 3 Stellen, §1.3: D/F/G), render_pc.c (+9, §1.3: D), msg_common.c (+4/-1, nicht gelistet); dazu aus Nachbesserung 0/1 player_common.c (+2) und re15_inv_screen.c (+3/-1). Die Zeilen sind markiert ("Runde 41 Spur E"), aber die WIRKUNG ist spielweit: jedes Apply-Bild im ganzen Spiel wird gehalten, jede Nachricht gibt den Spieler einen Takt spaeter frei, alle Auswahl-Nachrichten laufen ohne Parken. Fuer B/C/A gab es dafuer Orchestrator-Entscheide (B5, C-E3, A-E3), fuer E nicht -> Mangel 3 |
| Riegel je Punkt | haelt, mit Mangel 2 | Punkt 1: unit kurbel_{laenge,schleife}, takt_{msgtakt,kamerahalt,frage}, integration kurbel/geh/renn/kante; Punkt 2: unit karte_pin + integration karte; Punkt 3: unit 11b1_* + integration werkstatt_{szene,tuer}. Parallel-fest (Zufalls-Laufordner, exe-Kopie je Lauf, geerbte RE15_* geloescht). Aber: der Riegel `renn` prueft mit dem Auswerter `takt` im Pos_set-Bild "(3650,-3018) mo 200" und meldet "Bildfolge wie das Original" — das ist genau der bekannte Nicht-Original-Zustand (Original n94: (3650,-3099) `cmd 1/2/3/0` Clip 3, Renn-Stopp) -> Mangel 2 |
| Nebenwirkungen beide Szenarien | haelt (gemessen) | Leon + Elza in Punkt 2/3 neu gemessen; Inventar auf der Kamera-Kante (§3.5); Tuer-Einblendungen 2050 -> 2020 und 1230 -> 1190 mit `RE15_FADE_LOG` + `RE15_KAMERAHALT_LOG`: kein gehaltenes Bild waehrend der 6-Bild-Einblendung (`<sp>/E41a2/runs/FADE_tuer*`); Integrations-Auswahl §6.1 |
| PSX | Hinweis | platform/psx/main.c ruft weder msgtakt noch kamerahalt (Bau-Agent: PSn00bSDK fehlt); msgtakt faellt ohne `takt_beginn` auf das alte Verhalten zurueck (Riegel takt_msgtakt "ohne takt_beginn ... Restore sofort", "inaktiv: Auswahl parkt wie vorher") -> PC und PSX verhalten sich verschieden. Andere Spuren haben Haken in beiden main.c gesetzt (z.B. 370793fe "Haken main.c PC/PSX") |

