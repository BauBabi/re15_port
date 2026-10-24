# Runde 41 — Spur E "kleinkram_b" — Abnahme 3 (unabhaengig, nach Nachbesserung 3)

Pruefer: Abnahme-Agent 3, 2026-10-24 (Beginn 16:21 laut `date`). Baum `.claude/worktrees/r41_kleinkram_b`, Zweig
`r41/kleinkram_b`, HEAD c3ba4317 (Basis = merge-base master 116e8b0f). `<sp>` = Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`;
eigene Laeufe unter `<sp>/E41a3/` (Helfer `<sp>/E41a3/run.sh`: exe-Kopie, eigener Laufordner, geerbte RE15_* geloescht).

Status dieses Berichts: IN ARBEIT (fortlaufend gespeichert).

## 0. Gelesen
- AUFTRAG.md (Wortlaut; fuer Spur E: Orchestrator-Hinweise 2026-10-24 03:21, Auftraege E-N1/E-N2 04:52, Entscheide E-E1..E-E3
  10:50), VERTRAG.md, UEBERGABE.md, Dossier E_kleinkram_b.md (ganz, Schwerpunkt "Nachbesserung 3" + "Fortsetzung"),
  E_abnahme_2.md (Maengel 1-4, Hinweise H1-H7).
- `git log --oneline master..HEAD` (Nachbesserung 3: 6af253d8 .. c3ba4317, 22 Commits), `git diff master...HEAD --stat`
  (= `git diff 116e8b0f HEAD`, 34 Dateien; `git diff master --stat` zeigt zusaetzlich die seither auf master gemergten
  Spuren A/D als "Loeschungen" — nicht Spur E). Code-Diff `re15_port` vollstaendig gelesen (`<sp>/E41a3/code.diff`, 3665 Zeilen),
  NB3-Teil `git diff 5c27360b HEAD -- re15_port` (11 Dateien): rennstopp_r41.c/.h (neu), player_common.c (4 Haken),
  bildhalt_r41_pc.c (+re15_fade_tick), main.c (nur Kommentar), re15_kamerahalt_r41.h (Kommentar), Tests.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; getrennt `build` -> "ninja: no work
  to do.", `=== LOCAL-BUILD-OK (build)` (`<sp>/E41a3/configure.log`, `build.log`). re15_pc.exe md5
  1f38e02535c6259e6f99619bba2917ab (= behaupteter Endstand) -> Messkopie `re15_pc_E41a3.exe`. VORHER-exe = `re15_pc_E41a2.exe`
  (11b8f867 = Stand Abnahme 2, md5 486f77a7d870cf73c1bdf542bf40cbfb), BASIS = `<sp>/abn0_base/.../re15_pc_E41a1_basis.exe`
  (116e8b0f, md5 2310696431b26d115bc00f3796663c01).
- Last: waehrend der Abnahme liefen Laeufe anderer Spuren (re15_pc_r41c_*, _h41abn0*, _r41m*, ctest/cmake) — Fenster-Haken
  werden bei Rot einzeln nachgefahren.

## 1. Tests (gezielt, im Spurbaum, kein test/all)
- `ctest --test-dir re15_port/build -R "r41_kleinkram_b|^unit_r36_zombieki_affen_block$" -j 2 --timeout 900 --output-on-failure`
  (16:34-16:37): **100% tests passed, 0 tests failed out of 21**, CTEST_EXIT=0 (`<sp>/E41a3/ctest_spur.log`): 11 unit
  (11b1 x3, karte_pin, kurbel x2, takt_{msgtakt,kamerahalt,frage,rennstopp,renntempo,rennpfad}), 8 integration (kurbel, karte,
  werkstatt_szene, werkstatt_tuer, kante, geh, renn, spawn_kante), unit_r36_zombieki_affen_block (95 s).
- Auswerter der Riegel selbst auf die frischen Laufordner angewandt (`test_r41_kleinkram_b_exe`): renn `F282 (3650,-3099) ..
  F287 (3650,-3369)`, `Stand (3650,-3369) F287..F420, Plc_motion F302`, erstes Renn-Bild F157 z -3216; spawn_kante `FREI F6
  GEHALTEN 70 (F1..F5: 5) X1 9000 X100 11100`; kurbel `4 Umlaeufe, laengster Halt auf Slot 39: 1`; geh `Pos_set F382 = Frage zu
  F381 + 1`. `-V` takt_rennstopp 17/17, takt_renntempo 7/7 (o_renn2 198 210 171 182 204 211 214, o_rennstopp ab Bild 2 11/11,
  Kurbel z -3099..-3369), takt_rennpfad 6/6 inkl. Gegenprobe "ohne Modul A-E fallen durch" (`<sp>/E41a3/takt_renn_V.log`).
- Weitere Gruppen (unit/prop/probe, Integrations-Auswahl Renn-Eingaben/Blenden/Tueren/Karte): §7.1.

## 2. RE-Gate — Stichproben SELBST disassembliert (re15_disasm.py, info/Re1.5/PSX.EXE = Auslieferungsstand)
1. **DECIDE Rennen 0x80032604** (`dis 0x80032604 95`): @0x80032618 `lw a0,0(s0)` (gehalten 0x800ac768); @0x80032620 `andi 0x2` ->
   @0x80032644 `addiu at,at,16165` (= 0x80073f25[Figur*2]) / @0x80032654 `addu` / @0x8003265c `sh` Gier +0x6a; @0x80032660 `andi 0x8` ->
   @0x80032698 `subu` / @0x800326a0 `sh`; danach erst @0x800326a4 Aktions-Pruefung (`lw 0x800ac76c & 0x80`, `jal 0x8002d474`,
   `jal 0x80042bac` a2 = 0x10, Treffer -> @0x80032764), @0x800326e8 `sltiu +6,2`, @0x800326fc `andi 0x201` / @0x80032700 `bne` ->
   @0x8003272c `sb 2,754(s0)`; Zielen @0x80032708 `andi 0x100` + Waffe @0x80032718 `lbu 0x800aca5d`. +6 >= 2: @0x80032738 `andi 0x1`
   -> 0x101, @0x80032744 `andi 0x4` -> 0x401, @0x80032750 `andi 0xa` -> 0x301, @0x80032760 `sw 0x800aca58`. **Bestaetigt** (Drehen 72 in
   JEDEM Bild des Unterzustands 2 vor allen Entscheidungen; OBEN im Stopp = 0x0101 Gehen ohne RENNEN-Pruefung).
2. **ANIMATE Rennen 0x80032778** (`dis 0x80032778 130`): Fall 0 @0x800327e0 `addiu at,at,16164` (0x80073f24) -> @0x80032830 `sh a0`
   Tempo, @0x800327f4/@0x800327fc +0x8f = 7, @0x8003280c/@0x80032814 Clip 0 / Bild 0, faellt in Fall 1; Fall 1 @0x80032884 anim_set
   (a3 = 0x200), @0x80032894 Figur 0xf -> ohne Fussrechnung, @0x800328b4 `andi 0x2000` -> a1 0/1, @0x800328cc `jal 0x800369f8`,
   @0x800328e4/@0x800328e8 `sll 16`/`sra 18`, @0x800328ec `addiu 150`; Fall 2 @0x800328f8-@0x80032910 (+6 = 3, Clip 3, +0x8f = 7 —
   Bild +0x95 wird NICHT geschrieben), Fall 3 @0x8003293c `addiu -40`, @0x8003294c `bgez`, @0x80032958 `sw 1,0x800aca58`,
   @0x8003295c `jal 0x800245d8` (auch mit negativem Tempo). **Bestaetigt.**
3. **FUN_800369f8** (`dis 0x800369f8 100`): @0x80036a30 `lw s3,392(v0)` (Teile +0x188), @0x80036a34 `jal 0x80068098` (RotMatrix
   +0x68 -> +0x20), Kette `jal 0x80022da0` mit s3+24 (Teil 0), s3+196 (Teil 1 = +0xAC), Fuss-Versatz @0x80036a60-@0x80036a70
   (a1*0x204), s3+688-320 / -148 / +24 = Teile 2+3f / 3+3f / 4+3f; Modus 1 @0x80036af0-@0x80036b34: (s16)(x - Teil+0x54)^2 +
   (s16)(z - Teil+0x5c)^2 -> `jal 0x80065f60` SquareRoot0 -> @0x80036b44 `sh v0,140(v1)` (+0x8c). **Bestaetigt.**
4. **Blenden-Takt** (`dis 0x80020f2c 12`, `dis 0x80021880 60`): @0x80020f34 `jal 0x80021a0c`, @0x80020f3c `jal 0x80010000`,
   @0x80020f44 `jal 0x80021880`, @0x80020f4c `jal 0x8002137c` (Present), @0x80020f54 `j 0x80020c10`; FUN_80021880 @0x8002191c
   `lhu` / @0x80021920 `lhu -6(s1)` / @0x80021928 `addu` / @0x8002192c `sh`. **Bestaetigt** (Takt vor dem Present, also auch im
   Apply-Durchlauf).
5. Tabellen/Dispatcher: `table 0x80073fb0 4` = 0x80031f38 / 0x800322e8 / 0x80032604 / 0x80032974, `table 0x80073ff0 4` =
   0x80032038 / 0x80032498 / 0x80032778 / 0x80032ae4; `read 0x80073f24 8 --w 2` = 18632 (0x48C8 = 200/72) x4, 18642 (0x48D2 = 210/72)
   x4; 0x80073ea4 = 0x304B (75/48) / 0x3046; 0x80073ee4 = 0x6000 (0/96); Pad-Tabelle 0x80073dbc [1] 0x2000 RECHTS, [3] 0x8000 LINKS,
   [7] 0x80 QUADRAT, [8] 0x8 R1, [9] 0x40 KREUZ. Dispatcher @0x80031ef4 `jalr` DECIDE, @0x80031efc `lbu +5` neu, @0x80031f1c `jalr`
   ANIMATE. Stehen-ANIMATE Fall 0 @0x80032074-@0x800320bc (+6 = 1, Clip 3, Bild 0, +0x8f = 7, Tempo 0, `jal 0x8001af20` & 0x1f + 90);
   Gehen-DECIDE @0x80032444 `andi 0x201` / @0x80032454 `sw` (Rennen erst hier). **Bestaetigt.**
6. Gegenprobe Mechanismus am Original (eigene Messung, §3.2): die Aktions-Pruefung @0x800326a4 haelt den Stopp an der Kurbel
   NICHT auf — die Kurbel-Zone wird nicht ueber FUN_80042bac(…,0x10) im DECIDE ausgeloest; Port und Original starten den Stopp im
   Aktionsbild gleich.

## 3. Punkt 1 — Kurbel ROOM2020 ("komische herumfliegende Animation ... am Anfang wenn er die Kurbel nutzt")
Laeufe `<sp>/E41a3/runs/<lauf>/` (cmd.txt, debug.log, state.txt, anim.txt, fd_*.ppm), gemeinsam `RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1`; NACHHER = re15_pc_E41a3.exe, VORHER = re15_pc_E41a2.exe
(11b8f867). Original = DuckStation-GDB (Treiber des Bau-Agenten `<sp>/E41n2/gdb_geh2.py`, Kopie `<sp>/E41a3/tools/gdb_geh2_kopie.py`,
Protokoll am Eingang FUN_80031c44 = vor dem Spieler-Takt; Start `<sp>/E41n1/orig/hg_ready.sav`).

### 3.1 Renn-Anlauf der Abnahme 2 (`P1_renn_a2`)
`RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=2600,-800,1024,1
RE15_INPUT_SCRIPT=XU0.2,(XUA0.1,XU0.1)x4,XUA0.1,XU1.0` (Start 300, Basis spiel), Fragen per Quadrat @380/430/470/510.

| | Port NACHHER (state.txt / [mot] / anim.txt) | Original o_renn2 (`<sp>/E41n2/o_renn2.txt`, selbst gelesen) |
|---|---|---|
| erstes Renn-Bild | F300 (2600,-3216), F301 -3382 (Stegkante) | n1 (2600,-3216) spd 198, n2 -3382 |
| Nachrichten | Aktion F306, msg 0 F307, Renn-Clip mo 100 af 6 eingefroren bis F381 | Aktion n6, msg 0 n7, mo=0/7 eingefroren |
| Pos_set @0x0A8C | F382 (Ifel_ck..Sleep im selben Takt, `[kamerahalt] F382 ... HALTEN`) | n93 (t2@0A99) |
| Auslauf | F382..F387 z **-3099 -3233 -3327 -3381 -3395 -3369**, mo 200 = Clip 3 (fc 1), frac 7..2 | n94..n99 z **-3099 -3233 -3327 -3381 -3395 -3369**, mo=3/0, e3 6..1 |
| Stehen danach | F388 frac 7 (Stehen-Fall 0), F389.. 6..0 | n100 e3 6 (cmd 1/0/1/0), n101.. 5..0 |
| Plc_motion @0x0A9C | F402 = Pos_set + 20, Kurbel bei (3650,-3369) | n113 = n93 + 20, (3650,-3369) |

Port-frac = Original-e3 + 1 durchgehend (der Port protokolliert den Zaehler vor dem Abzaehlen; dieselbe Folge). VORHER (Abnahme 2
§3.4): F382 Stehen (3650,-3018) ohne Auslauf. Bildbogen `<sp>/E41a3/P1_renn_a2_380_391.png` (F380/381 Frage, F382 gehalten = F381,
F383..F387 Leon rutscht in Cut 3 Richtung Kamera, F388/389 steht, F402/F410 Kurbel). **Original-gleich.**

### 3.2 NEU: Rennen loslassen und Aktion im SELBEN Bild (Daumenwechsel Kreuz -> Quadrat), Port UND Original gemessen
Port `P1_renn_losl_aktion`: `RE15_PLAYER_POS=2600,-3018,1024,1 RE15_INPUT_SCRIPT=XU0.2,A0.034,W1.0` (Start 157 = Stehclip 2 wie Riegel
renn), Quadrat @200/240/280/320. Original `<sp>/E41a3/o_renn_losl.txt`: `gdb_geh2_kopie.py --state <sp>/E41n1/orig/hg_ready.sav --up 6
--run --act 6` (Pad-Bild 0..5 OBEN+RENNEN, Bild 6 nur Aktion). DuckStation settings.ini danach bytegleich zurueckgestellt (der Treiber
schrieb CRLF statt LF, Inhalt gleich; Sicherung `<sp>/E41a3/tools/settings.ini.vor_E41a3`, md5 b3e220606abe9b06da203ba9d0fcf133).

| | Port | Original |
|---|---|---|
| Aktionsbild | F163 Stopp beginnt (mo 200 af 0 frac 7), z -3382 (Kante) | n6 -> n7 `cmd=1/2/3/0 mo=3/0 spd=171 e3=6` (Stopp beginnt, 211 - 40) |
| waehrend der Fragen | mo 200 frac 7 eingefroren | mo=3/0 e3=6 eingefroren |
| nach Pos_set | F282..F286 z **-3056 -3147 -3198 -3209 -3180** | n94..n98 z **-3056 -3147 -3198 -3209 -3180** (spd 131 91 51 11 -29) |
| Stehen / Kurbel | F287 frac 7, Plc_motion F302, Kurbel (3650,-3180) | n99 e3 6, Plc_motion n113, (3650,-3180) |

Bild fuer Bild gleich. Der Bau-Agent fuehrt die Aktions-Pruefung @0x800326a4-@0x800326dc als OFFEN ("das Original rennt ein Bild
weiter"); an der Kurbel trifft das NICHT zu — die Kurbel-Zone wird im Original nicht ueber FUN_80042bac(…,0x10) im DECIDE ausgeloest,
der Stopp beginnt im Aktionsbild wie im Port. Fuer Tueren/0x10-Zonen nicht gemessen (Hinweis H2).

### 3.3 Freie Strecke (Renn-Tempo + Stopp spielweit), `P1_rennstopp_frei`
(2600,-3200) Blick 0, Start 153 (Stehclip 2 Bild 39 wie o_rennstopp `mo=2/40`), 12 Bilder XU, dann loslassen.
Port dx je Bild: 199, **210 170 182 205 210 214 210 188 159 176 197**, Stopp **157 117 77 37 -3**, dann Stand.
Original `<sp>/E41n2/o_rennstopp.txt` (selbst gelesen): 269, **210 170 182 205 210 214 210 188 159 176 197**, **157 117 77 37 -3**.
Gleich ab Bild 2; Bild 1 (269) = der vom Bau-Agenten benannte Lage-Versatz des GDB-Laufs (Port und Sonde rechnen 199). VORHER: konstant
200, sofort Stehen.

### 3.4 Drehen aus dem Rennen und OBEN+RENNEN im Stopp
`TA_links_{nach,vorher}`: (4000,-3150) Blick 0, XU 8 Bilder, dann nur LINKS. NACHHER: F68 Gier -72 + Stopp-Schritt 169 (mo 200), F69 -72
ohne Schritt (mo 105 = Drehen 0x0301), ab F70 -96. VORHER: ab F68 -96 ohne Schritt. Original (Disasm §2 Nr. 1): Bild 1 dreht 72
(@0x80032660-@0x800326a0) und setzt +6 = 2 (@0x8003272c), ANIMATE Fall 2/3 mit Schritt; Bild 2 dreht 72 und schreibt 0x0301
(@0x80032750), Drehen-ANIMATE Fall 0 ohne Schritt; danach Drehen-DECIDE 96. **Gleich.**
`TC_stopp_obenrennen` (Stopp 2 Bilder, dann OBEN+RENNEN): 170, 130, dann **ein Geh-Bild 75 (mo 105)**, dann Rennen 208 231 ...;
loslassen 154 114 74 34 -6 = @0x80032738 (0x0101) / @0x80032444 (Rennen erst aus dem Gehen-DECIDE). Das Geh-Tempo 75 ist der
vorbestehende Port-Wert (Original: Fussweg @0x800325c4-@0x800325e8; OFFEN des Bau-Agenten, nicht E-E2).

### 3.5 Weg des Nutzers aus seinem befund.log, nachgefahren (`P1_nutzerweg`)
Gelesen `re15_port/build/platform/pc/befund.log` (Hauptbaum, nur Lesen), Zeilen 64768-64810: Tuer 2050 -> 2020 (11350,-3150), F25..F70
**gerannt** nach Westen (dx ~199 je Bild, Cut 0 -> 2 -> 3), F85 (2133,-3006) gestoppt, auf der Stelle gedreht (Gier 2064 -> 912), F235
(2166,-3382) Gier 864 an der Stegkante, F280 -> F295 an der Kurbel (3650,-3018), F415 Cut 4, F505 nach Plc_ret.
Nachgefahren: `RE15_DEBUG_JUMP=2050@240 RE15_FIRE_AOT=1@30#2050 RE15_EQUIP=3 RE15_INPUT_SCRIPT=L0.36,XU1.47,W0.3,L0.4,U0.4,A0.034,W5.0`
(Start 60), Quadrat @200/240/280/320: Rennen F71..F114 nach Westen (Cut 0 -> 2 -> 3), Auslauf F115..F120 (2562 -> 2480 -> 2497), Drehen
auf der Stelle, Gehen an die Kante (2713,-3382) Gier 864, Aktion F148, msg 0 F149, Frage F202, Ja -> F282 Pos_set (gehalten), F302
Plc_motion, F480 Plc_ret. Bildbogen `<sp>/E41a3/P1_nutzerweg_zoom.png` (Ausschnitt x2,5): F280/281 Leon an der Kante, F283 an der
Kurbel, F302-F304 die 7-Bild-Blende in den Raum-RBJ-Clip (der Waffenarm schwingt nach aussen), ab F305 Kurbeln. Dieselbe Blende zeigt
das Original-Video (Bogen des Bau-Agenten `<sp>/E41n1/pr_cut3_k0_30.png`, k21: Original v480 und Port F544 gleiche Zwischenpose).
Kein Abheben, kein Gleiten, keine Fremdpose.

### 3.6 Weitere Anlaeufe (Riegel, selbst gefahren, §1)
Gehen (`geh`: kein Luecken-Bild, Pos_set F382 = Frage zu + 1, gehalten), Weg 2050 -> Cut 3 gehend (`kurbel`: Lage (3650,-3018)
197/197, 4 Umlaeufe, max 1 Bild auf Slot 39), Rennen nach o_renn2 (`renn`). Stand/Gehen/Cut-3/"No" sind seit Abnahme 2 unveraendert
(der NB3-Code betrifft nur den Unterzustand 2 und den Blenden-Takt). Stand selbst nachgefahren (`P1_stand`: (2600,-2800) Blick 1024,
Quadrat @330/380/420/460): Frage zu F461, Pos_set F462 (3650,-3018) gehalten, Wiege-Clip mo 212 laeuft weiter (af 49 -> 50..69),
Plc_motion F482 = Pos_set + 20 — wie Abnahme 2 §3.2 / Original o_stand1.

### 3.7 Urteil Punkt 1: **nicht_pruefbar**
Alles Messbare ist original-gleich — einschliesslich der in Abnahme 2 offenen Renn-Varianten (§3.1, §3.3, §3.4) und einer neuen,
eigens am Original gemessenen Variante (§3.2). Die einzige je gefundene Port-Abweichung an der Kurbel (Standbild nach einem Umlauf,
Nachbesserung 0) ist behoben. Das vom Nutzer beschriebene "Herumfliegen" ist aber in KEINEM Lauf (Port vorher/nachher, Original, sein
eigener Weg aus dem befund.log) zu finden; welcher Anblick gemeint war, laesst sich ohne den Nutzer nicht feststellen — also auch nicht,
ob er weg ist ("Fix erklaert den Befund" ist nicht belegbar). Grund des Urteils: Symptom nicht reproduzierbar, alle Anlauf-Varianten
original-gleich. Damit ist die Bedingung des Orchestrator-Hinweises 2026-10-24 03:21 fuer die Rueckfrage an den Nutzer erfuellt; die
Rueckfrage im Dossier ("Fuer den Nutzer (Stand NB3)", Moeglichkeiten a-e) ist sachlich richtig gestellt. **Kein Bau-Mangel** — eine
weitere Nachbesserung kann diesen Punkt nicht schliessen, nur die Antwort des Nutzers.

## 4. Punkt 2 — Karte B1, vordere Tuer der "Waffen Armory" (ROOM1230 Gang -> ROOM1190 Ostkammer) — neu gemessen
Gemeinsam `RE15_DEBUG_JUMP=1230@240 RE15_PLAYER_POS=96,14500,1024,0 RE15_SET_FLAG=4:0xf3,3:0x89,7:0x88,7:0x89
RE15_INPUT_SCRIPT=<anlauf>,W0.2,S0.1,W1.0,Q0.1 RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.txt
RE15_INV_FB_SHOT=map.bmp RE15_INV_FB_SHOT_AT=120 RE15_MAP_SHOT_PAGE=0 RE15_EXIT_AT=560`; Marker per `test_r41_kleinkram_b_exe marker
map.bmp`; BASIS = 116e8b0f, NACHHER = HEAD. Laeufe `<sp>/E41a3/runs/P2_*`.

| Lauf | Spieler (state.txt) | BASIS | NACHHER |
|---|---|---|---|
| Leon U0.2 (Kontrolle, weit weg) | (96,14050) | MARKER 145 104 | MARKER 145 104 |
| Leon U0.6 | (96,13150) | **MARKER 137 109** | MARKER 145 106 |
| Leon U1.0 (an der Tuer) | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Elza `RE15_TITLE_SHOT_CHAR=1`, room1231.rdt, U1.0 | (96,12779) | **MARKER 130 112** | MARKER 145 107 |
| Leon RENNEND (XU0.25, Auslauf bis zur Tuer) | (96,12799) | — | MARKER 145 107 |
| Leon echte Tuer `U1.0,W0.3,A0.1` + `RE15_INV_OPEN_AT=60#1190` (`[tuer] Sequenz fertig: 301 Bilder`, room1190.rdt) | (19515,-12109) | — | MARKER 145 108 |

Ostkammer-Marke (146,109), Hallen-Marke (127,115). BASIS wandert beim Herantreten an die vordere Tuer bis zu 15 px zur Hallen-
Seite (= "Sprung zum hinteren Teil, obwohl man gar nicht da ist"), NACHHER bleibt der Marker an der Ostkammer — Leon und Elza,
gehend und rennend (Renn-Stopp der NB3 aendert daran nichts), nach dem Durchgang (145,108). Ausschnitt x4
`<sp>/E41a3/P2_karte_basis_nach.png` (Reihenfolge: BASIS U0.6, BASIS U1.0, NACHHER U0.6, NACHHER U1.0).
Code von Punkt 2 (karte_r41.c, Haken re15_inv_screen.c) seit Abnahme 1 unveraendert. **Urteil Punkt 2: erfuellt.**

## 5. Punkt 3 — Werkstatt-Tuer ROOM11B1 bei Elza dauerhaft zu — neu gemessen
### 5.1 ECHTER Tuerweg Elza ROOM1261 -> Tuer Slot 0 -> ROOM11B1 mit der Szene "Sherry!? / Where has she gone!?"
`RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=1261@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3 RE15_PLAYER_POS=27650,9450,1024,1
RE15_PRESS=square@300,square@310 RE15_SCD_TRACE=1 RE15_MSG_LOG=1 RE15_CAM_TRACE=1 RE15_STATE_LOG=state.txt RE15_KAMERAHALT_LOG=1
RE15_FRAMEDUMP=0-700/10:fd_ RE15_EXIT_AT=700#11b1` (Flags = Zustand nach 10B1 sub04 @0x0195E und 2001 @0x01958), Laeufe
`P3a_szene_{nach,basis}`:
- NACHHER: `loaded room1261.rdt` -> `[tuer] Sequenz fertig: 331 Bilder` -> `loaded room11b1.rdt` -> `[wk11b1-r41] ROOM11B1 (3,0x83)=1:
  Ereignis 24 im Init-Lauf hinter sub00 — Werkstatt-Tuer bleibt zu` -> `[msgtext] room=11b1 id=10 "Elza: Sherry!?"`, `id=11 "Elza:
  Where has she gone!?"`, Plc_ret @0x14E6 F473; Kameras **2,3,4,5,6, letzte 2** (nie Cut 7/8 = gemaltes offenes Tuerblatt).
- BASIS: kein Ereignis 24, Kameras 3,4,5,6,8, **letzte 8**.
- Bild F690 `<sp>/E41a3/P3a_ende_basis_nach.png`: links BASIS Tuerblatt hinter Elza aufgeschwungen (hell), rechts NACHHER
  geschlossene dunkle Tuer mit Klinke.
### 5.2 "dauerhaft": gegen die Tuer von beiden Seiten (`RE15_DEBUG_JUMP=11b1@240 RE15_SET_FLAG=3:0x83,4:0xf3`, Start 262)
- Garagenseite `P3b_garage_nach` (`RE15_PLAYER_POS=-20200,-16230,2048,0`, `U1.5,W0.3,A0.1`): ab F272 **(-20942,-16230) blockiert**,
  `[msgtext] room=11b1 id=12 "The door is locked."`, `EXIT_AT: Bild 380 in Raum 11b1`.
- Werkstattseite `P3c_werkstatt_{nach,basis}` (`RE15_PLAYER_POS=-24000,-16230,0,0`, `U2.0,W0.3,A0.1`): NACHHER ab F282 **(-22618,-16230)
  blockiert**, Cut 0, msg 12; BASIS laeuft durch (F302 -20925, F322 -19768, Kameras 7 -> 8).
- Mechanismus (Code gelesen, werkstatt11b1_r41.c): sub00 des Originals oeffnet nur bei (3,0x83)=1 (RDT-Anker @0x010F6, sonst
  Else-Zweig = zu); das Modul kehrt genau diesen Zweig bei JEDEM Raumeintritt mit (3,0x83)=1 um (Init-Lauf) -> vor und nach dem
  Schacht-Ereignis zu. Leon (11B0) unberuehrt (Raum-Pruefung `RE15_WK11B1_RAUM`).
### 5.3 Grenze
Der Abschnitt 2001 -> 1261 ist wie in Abnahme 2 nicht gefahren (Zustand nach 2001 sub04 per RE15_SET_FLAG); 1261 -> 11B1 ist die
echte Tuer. Code von Punkt 3 seit Abnahme 1 unveraendert. **Urteil Punkt 3: erfuellt.**

## 6. Nebenwirkungen des spielweiten Renn-Stopps (NB3) — eigene Gegenproben
### 6.1 Tuer im Stopp (`N1_tuer_im_stopp`): kein Rest-Rutschen
ROOM1230 (96,14500) Blick 1024, XU 6 Bilder, dann Aktion (= Loslassen + Aktion im selben Bild) an der Ostkammer-Tuer: F306 Stopp-
Schritt 171 (mo 200) und Tuer ausgeloest (`[tuer] Sequenz ... 301 Bilder`), `loaded room1190.rdt`; in 1190 F1..F12 steht Leon auf
(19515,-12109) mo 210 (Eintrittspose `re15_player_room_entry_pose` setzt 210 -> das Modul bricht den Stopp ab, Bedingung
`p->motion != s_steh`). In Ordnung.
### 6.2 **Treppe im Stopp (`N2_treppe_im_stopp`, `N2_treppe_bild_{nach,vorher}`): Leon rutscht NACH der Treppe weiter — Regression**
`RE15_STAIR_DBG=1 RE15_DEBUG_JUMP=2040@250 RE15_PLAYER_POS=-1350,-19200,3072,5 RE15_EQUIP=3
RE15_INPUT_SCRIPT=XU0.4,A0.034,W0.034,A0.034,W0.034,A0.034,W0.034,A0.034,W5` (Start 300, Basis spiel; Galerie Band 5 -> Treppe sce 13
Slot 5/6 = derselbe Weg wie Riegel `test_r36_dachkiste` treppe2040): F300..F311 Rennen (ab F304 an der Kante (−1350,−18368)), F312
Loslassen + Aktion -> `[stair] START dir=DOWN cur_band=5 target_band=3`, mo 220 bis F374, F375 Treppen-Finalize (mo 200, z -13782).
| Bild | NACHHER (HEAD) z | VORHER (11b8f867) z |
|---|---|---|
| F375 (Finalize) | -13782 | -13782 |
| F376 | **-13665 (+117)** | -13782 |
| F377 | **-13588 (+77)** | -13782 |
| F378 | **-13551 (+37)** | -13782 |
| F379.. | **-13554 (-3)**, dann Stand | -13782 |
Die Schritte 117/77/37/-3 sind die Signatur des Renn-Stopps (Tempo - 40 je Bild, letzter negativer Schritt): der in F312
begonnene Stopp (Modulzustand `s_aktiv`, rennstopp_r41.c:21) ueberlebt die Treppe, weil der Treppen-FSM den Spieler-Takt auslaesst
(game_step_common.c:1504-1514, kein `re15_player_tick`) und im Finalize genau die Steh-Bewegung zurueckgibt
(stair_common.c:149 `p->motion = RE15_PLAYER_MOTION_IDLE`) — das Modul bricht nur ab, wenn es in einem Spieler-Takt eine andere
Bewegung oder `state != 1` SIEHT (rennstopp_r41.c:193). Leon gleitet nach der Treppe 228 Einheiten in der Steh-Pose weiter.
Variante `N2b_treppe_nach_2_stoppbildern` (Loslassen 2 Bilder vor der Aktion, Leon an der Kante blockiert): Finalize F375 wie VORHER
(-13782), danach nur noch der letzte Stopp-Schritt F376 -1 — wie viel nach der Treppe uebrig bleibt, haengt davon ab, wie viele der 6
Stopp-Bilder vor dem Treppenstart verbraucht waren.
**Original (selbst disassembliert):** Treppen-Finalize LAB_80038e50 @0x80038ea8 `sb zero,0x800aca59` (+5 = Unterzustand 0) und
@0x80038eb0 `sb zero,0x800aca5a` (+6 = 0) — der Renn-Stopp lebt nur in +5 = 2 / +6 = 2..3 (0x80032604 @0x800326e0-@0x800326ec, 0x80032778
@0x800327b0-@0x800327bc) und ist nach der Treppe geloescht; Stehen-ANIMATE Fall 0 setzt das Tempo auf 0 (@0x800320a4). Ein
Weiterrutschen nach der Treppe gibt es im Original nicht. -> **Mangel 1.**
Dieselbe Klasse (Spieler-Takt ausgelassen, Rueckgabe mit Bewegung 200 / state 1) haben im Port auch die Opfer-Freigabe nach einem
Griff (enemy_ai_common.c:2007 `player->motion = 200`) und Plc_ret (scd_vm.c:2860 `re15_actor_set_motion(pl, 200)`; Original
@0x80041f88: @0x80041f90-@0x80041f9c `sb 1,4` / `sb zero,5/6/7` = Stopp geloescht) — dort nicht gemessen (Griff/Skriptende waehrend der 6 Stopp-Bilder).
### 6.3 Elza (`E_elza_renn_{nach,vorher}`, ROOM2021 (4000,-3150) Blick 0, XU 12 Bilder)
NACHHER: Renn-Tempo aus PL04W03 184 192 163 183 196 201 205 197 158 175 187 191, Stopp 151 111 71 31 -9; VORHER konstant 200, sofort
Stand. Mechanismus wie Leon (Tabelle @0x80073f24 Figur 4..7 = 0x48D2, Fall 1 ueberschreibt mit dem Fussweg); fuer Elza KEIN
Original-Vergleich gemessen (Hinweis H3).
### 6.4 Wahrnehmung der Gegner waehrend des Stopps (Code gelesen, nicht gemessen)
Der Hund (enemy_ai_common.c:7276-7291, Original DAT_800aca58 == 0x0201) und die RE2-Zombies/Kraehe/Spinne (`pl->motion == 100`
-> cfbf6 0x04) lesen "Spieler rennt" aus der Port-Bewegung. Im Stopp ist die Port-Bewegung 200, im Original bleibt der Unterzustand
+5 = 2 (0x0201) bis zum Stopp-Ende — 6 Bilder lang "rennt" der Spieler fuer den Hund im Original, im Port nicht. Vor NB3 war es
genauso (sofort Stehen); das Modul bietet `re15_rennstopp_r41_unterzustand2()`, die Praedikate nutzen es nicht (fremde Dateien) ->
Hinweis H4.
### 6.5 Statusschirm im Stopp (`N3_inventar_im_stopp`, Tunnel (4000,-3150) Blick 0, XU 8 Bilder, 1 Stopp-Bild, dann START)
F68 Stopp-Schritt 170, F69 START (`[r38q] F69 Inventar-Ton Satz 6 (Oeffnen)`), Schirm offen bis F130 (`Satz 5 (Abbruch)`), danach
steht Leon (5778,-3150); F137 mo 210 (Port-Eintrittspose wie nach einem Raumeintritt) -> das Modul bricht den Rest-Stopp ab. Kein
Weiterrutschen, kein Sprung. Wie das Original den Stopp ueber den Statusschirm fuehrt, ist nicht gemessen (Hinweis, kein Mangel).
