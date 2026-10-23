# Runde 41 — Spur E "kleinkram_b" — Abnahme 0 (unabhaengig)

Pruefer: Abnahme-Agent 0, 2026-10-23. Baum `.claude/worktrees/r41_kleinkram_b`, Stand HEAD ed7bd4ff
(Basis master 116e8b0f; master ist seitdem nur um Doku-Commits weiter, `git diff --stat 116e8b0f master -- re15_port`
leer). Gebaut mit `local_build.sh configure` + `build` (ninja: no work to do = Baum des Bau-Agenten aktuell,
exe 23:23). Messkopie `re15_port/build/platform/pc/re15_pc_abn0.exe`; jeder Lauf in eigenem Ordner
`<scratchpad>/runs/<lauf>/` (debug.log dort). Alle exe-Laeufe mit
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1`, Elza-Laeufe
zusaetzlich `RE15_TITLE_SHOT_CHAR=1`; Bilder als PPM-Framedump (RE15_FRAMEDUMP) zu Kontaktbogen-PNG gewandelt.

Status dieses Berichts: FORTLAUFEND (nach jedem gemessenen Punkt gespeichert). §0-§3.4 und §5 vom ersten Pruefer
(Fable-Limit 2026-10-23 ~23:45, Commit 0e6c9a70); §3.5ff, §4, §6-§8 Fortsetzung 2026-10-24 (zweiter Pruefer, Stand HEAD
5ef6329b = ed7bd4ff + Doku, Code unveraendert).

## 0. Gelesen
AUFTRAG.md (Baum), VERTRAG.md, Dossier E_kleinkram_b.md, `git log master..HEAD` (11 Commits), `git diff master --stat`,
Code-Diff `re15_port/engine`, `re15_port/include`, `re15_port/tests` vollstaendig gelesen.

Beruehrte Dateien (git diff master --name-only, nur re15_port):
- neu: engine/src/karte_r41.c, plcloop_r41.c, werkstatt11b1_r41.c; include/re15_karte_r41.h, re15_plcloop_r41.h,
  re15_werkstatt11b1_r41.h; tests/unit/probes/r41_kleinkram_b.cmake; tests/unit/test_r41_kleinkram_b_{11b1,karte,kurbel}.c
- Haken: engine/src/player_common.c (+2), re15_inv_screen.c (+4/-1), scd_room_setup.c (+2), scd_vm.c (+2)
- Nicht beruehrt: release/, platform/android/, shared_assets/, pri/, synchro/, tests/CMakeLists.txt (probes/*.cmake wird
  per GLOB eingelesen, tests/unit/CMakeLists.txt:4060) — Pfad-Gate OK.
- Suche im Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/hack: kein Treffer.

## 1. Tests (gezielt, im Spurbaum)
- `ctest -R r41_kleinkram_b --timeout 600 --output-on-failure`: **6/6 bestanden** (11b1_programm/zu/original,
  karte_pin, kurbel_laenge/schleife).
- Gruppe des Bau-Agenten (`^unit_(map_|karte_|r30_karte|r33_karte|r35_karte|r36_karte|r36_kartenbits|r36_kartenitems|
  r36_szenenleon|r38_werkstatt11b0|r38_tagebuch_karte|r39_karten|r39_johnruf1041_tuer|plc_back_yaw_1090|
  r30_android_n3_plc_aim|r41_kleinkram_b)`, -j 4): **66/66 bestanden** (`<scratchpad>/ctest_grp1.log`).
- Zusatzgruppe Szenen/Posen/Tueren (alle Raeume mit `Plc_flg(0,4)` im Skript, s. §3.5: r35_cut1150, r36_elza, r37_roy,
  r38_elzatoene, r27_ada_cutscene_1050, 1090_push_anim, probe_1090_flinch_anim, r35_cut10f0, r36_szene1150,
  r37_szene11c0, r38_npckante, r39_elzazombies, r38/r39_heliport1171, r39_funk10f1, npc_scd_pose, anim_pose_order,
  anim_rate, cut_old_displayed, r34n_*, elza_zweig, r36_zombieki_szenen, r38_szenenschutz, plc_*, r30_elza_selbsttuer,
  r33_tuer1120, r35_affen_szene, r35_inventar1050_elza, r39_optik_elza, r39_parkplatz11c0_elza, r37_sperre1050,
  marvin_render_pose, zombie_pose, zombie_idle_clip, re2_zombie_pose_bank, npc_back_walk, npc_wall_clamp,
  r36_schluessel, r30/r31_tuer, r36_tueren, r35_zgirl, eintritt_pose_einmal; -j 4): **157/157 bestanden**
  (`<scratchpad>/ctest_grp2.log`).
- Integrationsgruppe (echte exe-Kopien, -j 2): s. §6 (Ergebnis dort nachgetragen).

## 2. RE-Gate (Stichproben SELBST disassembliert / gelesen)
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (PSX.EXE, Auslieferungsstand) und `ghidra1_V2.txt`.

1. **FUN_80030660 (Kommandowort 4) → 0x80050cb8** — bestaetigt:
   `@0x8003068c lbu v0,DAT_800aca59` (Spieler-Basis 0x800aca54 + 0x5; Gegenprobe: +0x94 = 0x800acae8 wie in
   player_common.c zitiert), `@0x80030694 sll v0,v0,2`, `@0x8003069c addiu at,at,0x3e30`, `@0x800306a4 lw v0,0(at)` =
   PTR_LAB_80073e30, `@0x800306ac jalr v0`. Tabelle `@0x80073e30[0] = 0x80050cb8` (table-Dump: [0] 80050cb8,
   [1] 80050ddc, [2] 80050f00, [3] 80051024, [4] 80030af0, [5] 80030d28, [6] 800517f0, [7] 80031080). Dispatcher-
   Tabelle `@0x80073f90[4] = 0x80030660` ebenfalls gelesen. Plc_motion `@0x80041bb0 sb v1(=4),4(v0)` und
   `@0x80041bc4 sb a2,5(v0)` (Sub-Byte +0x5) bestaetigt.
2. **0x80050cb8 Phasen-FSM** — bestaetigt: Phase 0 (`+0x6==0`) `@0x80050cec sb 1,+0x6`, `@0x80050cfc sb 7,+0x8f`,
   `@0x80050d0c sb 0,+0x95`, (`+0x1c4 & 0x40` → `+0x8f=0`); Phase 1 `@0x80050d4c jal 0x8001f314`
   (a0=+0x180, a1=+0x184, a2=+0x1c4>>7); Rueckgabe != 0 → `@0x80050da4 sb v0(=2),6(v1)` = HALTEN; danach
   `@0x80050db4 lhu +0x1c4` / `@0x80050dbc andi 0x4` / `@0x80050dc8 sb 1,+0x6` = Phase bleibt 1 = SCHLEIFE.
   Genau wie im Header re15_plcloop_r41.h zitiert.
3. **FUN_8001f314 Clip-Ende** — bestaetigt: `@0x8001f618 addiu v0,v0,1` / `@0x8001f61c sb v0,149(v1)` (+0x95++),
   `@0x8001f624 sltu v0,v0,s4` (Bild < Clipzahl), sonst `@0x8001f638 ori v0,zero,1` + `@0x8001f63c sb zero,149(v1)`
   (Wrap auf 0, Rueckgabe 1).
4. **Cut_replace LAB_80040414** — bestaetigt selbstinvers: je RVD-Zone (Stride 20 `@0x80040498 addiu v1,v1,20`)
   `@0x80040458 bne v0,a2` → `sb a1` / `@0x80040468 bne v0,a1` → `sb a2` (Byte 2) und `@0x8004047c..@0x80040494`
   dasselbe fuer Byte 3: ein Tausch a↔b, zweimal angewandt = Identitaet.
5. **Cut_chg LAB_800402a0** — `@0x800402fc sh a0,0x800b0fe4` (gezeigter Cut); der Port liest denselben Wert als
   work_vars[0x0A] (Form von sub00 @0x01102 `23 00 0a 00 00 00` selbst). Plausibel belegt.
6. **RDT-Bytes (selbst gedumpt, xxd/rdt_msgdump/scd_dump_room)**: ROOM11B1 md5 identisch zu info/Re1.5/PSX;
   sub00 @0x010F6 (102 B) == k_wk11b1_sub00; SCA (3,0x0C) @0x00EC8 `e4 02 d0 07 7a a9 b2 bc 01 ff 00 03` (u0=ff @0x00ED1,
   fl=03 @0x00ED3); @0x0112A `2c 08 01 31 … 0c 00 ff ff 00 00` (Platz 8, msg 12); msg 12 @0x179F = "The door is
   locked."; Schreiber der Zelle (3,0x0C) im ganzen RDT nur @0x01116/@0x0111A, Cut_replace nur @0x0110E/@0x01112.
   ROOM1230 @0x00CE6 `3b 04 02 31 … 3b 4c 00 00 b3 d0 00 04 00 19 0c` (next (19515,0,-12109), Raum 1190, Cut 12) und
   @0x00B54 (next (3174,0,-11182)); ROOM1180 @0x00A54 (Halle) / @0x00A94 `3b 04 00 …` (sce 0). ROOM2020 sub02 @0x00A9C
   `3f 00 00 00` Plc_motion(0,0,0), @0x00AA0 `43 00 04 00` Plc_flg(0,4), @0x00A8C Pos_set (3650,-1800,-2925),
   @0x00A98 Sleep 20, @0x00B16 Plc_ret; RBJ-Zeiger @0x5C = 0x0BB0.
7. **Marken-Tabelle** `re15_map_zones.h:986` Gang↔1190 Halle (zid 26) vor `:987` Gang↔1190 Ostkammer (zid 110); Zonen
   `:319` (0x1190 … zid 26) / `:321` (0x1190 … zid 110). Alte Suche in tuer_anziehen (`re15_map_zone_fuer(ziel, ix)`,
   ix 0..7) traf damit zuerst die Hallen-Marke — Ursache wie behauptet.

Alle verhaltensrelevanten Konstanten im Diff tragen `@0x…`/Datei-Offset oder sind als NUTZER-VORGABE (Punkt 3) bzw.
PORT-WAHL (Punkt 2, Tuer-Zug ist Port-Ergaenzung) gekennzeichnet. RE-Gate: **haelt**.

## 3. Punkt 1 — Kurbel ROOM2020 ("komische herumfliegende Animation am Anfang")
### 3.1 Messung (Lauf A, Leon, ECHTER Weg mit Aktionstaste)
`RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_PLAYER_POS=2600,-2800,1024,1
RE15_PRESS=square@330,square@380,square@420,square@460 RE15_SCD_TRACE=1 RE15_ANIM_TRACE=anim.txt RE15_MOTRACE=1
RE15_CAM_TRACE=1 RE15_FRAMEDUMP=300-720/3:fd_ RE15_EXIT_AT=740` (`runs/A_kurbel_leon/`).
- debug.log: `[scd] F331 slot=10 @0x0A5C op=0x2B` (msg 0 nach Quadrat an der Kurbel-AOT Slot 3), `F382 @0x0A62`
  (msg 1 "Will you use the Crank?"), `F462 @0x0A62` (Ja), `F463 @0x0A7E Cut_chg 3 / @0x0A8C Pos_set / @0x0A94 /
  @0x0A98 Sleep`, `F483 @0x0A9C Plc_motion / @0x0AA0 Plc_flg / @0x0AA4`, `F661 @0x0B0E Cut_chg 3 / @0x0B10 /
  @0x0B16 Plc_ret`. `[scd F463] Cut_chg(3)`, `[scd F591] Cut_chg(4)`, `[scd F661] Cut_chg(3)`.
- anim.txt (Aktor 0): F463..F482 Motion 211 (Idle-Sway W01 Clip 2, 52 Bilder) am Kurbelplatz; **F483 Motion 0, Clip 0,
  fc 40**, Slot 0..39, Wrap 39→0 bei **F523, F563, F603, F643** (vier volle Umlaeufe), Slot 17 bei F660, dann Plc_ret.
  Vorher laut Dossier: ein Umlauf + 138 Bilder Standbild. [mot] F483 `mo=0 af=0 flg=0x24 frac=7` (Flag 0x04 gesetzt,
  Crossfade 7 Bilder wie @0x80050cfc), F484 `flg=0x04` (0x20 kippt jedes Bild, @0x800306c4).
- Bilder: `sheetA_300_417.png`, `sheetA_420_537.png`, `sheetA_540_657.png`, `sheetA_start_detail.png` (F462..F495 einzeln):
  Leon steht nach dem Schnitt auf Cut 3 an der Kurbel, Idle 20 Bilder, dann durchgehendes Kurbeln; Buehne faehrt ab
  F591 (Cut 4). **Kein Fliegen, kein Schweben, kein Gleiten** in 141 Bildern (alle 3 Bilder) sichtbar.
### 3.2 Beleg
Siehe §2 Nr. 1-3: Mit Plc_flg(0,4) @0x00AA0 bleibt das Original in Phase 1 (`@0x80050dbc-c8`), der Clip wrappt
(`@0x8001f63c`) — die Schleife ist byte-true, das Standbild war der Port-Defekt. Umsetzung plcloop_r41.c nur im
Kommandowort 4 + Sub 0 (`@0x8003068c`), Port-Besitzer des Bildzaehlers (Schub 11B0, Substate 8) ausgenommen; W-Bank-
Klemmung bleibt dahinter (player_common.c).
### 3.3 Nebenwirkungen
`Plc_flg(0,4)` (`43 00 04 00`) steht in 60 Skriptstellen von 29 RDTs (1011, 1021, 1031, 1090, 1150/1151, 11B0, 1211,
2001, 2020/2021, 3000/3001, 3061, 3091, 4000/4001, 4010, 5011, 5021, 50B0, 6030/6031): dort laeuft ein Spieler-Clip im
Kommandowort 4 jetzt als Schleife statt Standbild — byte-true nach §2. Gezielte Tests dieser Szenen gruen (§1: 157/157,
darunter r36_szenenleon 11B0, r35_cut1150, r36_elza 1011/1021/1031, 1090_push, r38_npckante 2001). Integration s. §6.
### 3.4 Urteil Punkt 1: **teilweise**
Die einzige am Port messbare Abweichung (Standbild nach einem Umlauf) ist belegt behoben und am echten Weg gemessen.
Das vom Nutzer beschriebene "Herumfliegen" ist aber weder vom Bau-Agenten (7 Laeufe) noch von mir (Lauf A, echter Weg,
Leon) reproduziert worden; eine Original-Gegenprobe (DuckStation-Savestate der Kurbelszene) fehlt. Ob der Nutzer-Befund
damit behoben ist, ist nicht bewiesen — es bleibt die Rueckfrage an den Nutzer (Weg/Spielstand/Waffe/Bild), die das
Dossier selbst stellt.

### 3.5 Port mit anderen Waffen (Fortsetzung 2026-10-24) — die ersten ~25 Bilder nach Cut_chg 3 / Pos_set @0x0A8C
Laeufe `<sp>/runs_abn0/K_w{1,4,7,8}` (NACHHER-exe): `RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_PLAYER_POS=2600,-2800,1024,1
RE15_EQUIP=<w> RE15_PRESS=square@330,square@380,square@420,square@460 RE15_SCD_TRACE=1 RE15_ANIM_TRACE=anim.txt RE15_MOTRACE=1
RE15_STATE_LOG=state.txt RE15_FRAMEDUMP=455-700/1:fd_ RE15_EXIT_AT=700` mit w = 1 (Messer), 4 (SIG P228), 7 (Super Redhawk =
Magnum), 8 (Remington M870 = Schrotflinte); Log `[equip] W-bank -> W01 / W04 / W07 / W08`.
- Ablauf in allen vier Laeufen bildgleich: `[scd] F463 @0x0A8C op=0x32` (Pos_set) + `Cut_chg(3)`, `F483 @0x0A9C op=0x3F`
  (Plc_motion), `Cut_chg(4)` F591, `F661 @0x0B16 op=0x42` (Plc_ret). anim.txt: F463 Motion 211 (Idle-Sway), ab F483 Motion 0.
- Pixelvergleich gegen w1 im Leon-Ausschnitt (40,60)-(140,190): F464..F482 nur 5-16 px Unterschied in einem 6x30-px-Kasten
  (die Waffe in der Hand), F484 0 px, F490..F560 4-32 px (Waffe beim Kurbeln). Der Koerper posiert also mit jeder Waffe
  identisch: Motion 211 bildet der Port fuer alle Waffen auf PL00W01 Clip 2 ab (anim_select_common.c:10), nur das
  Waffen-Mesh wechselt.
- Bilder je Bild F462..F489 (Ausschnitt x2): `<sp>/abn0_bilder/K_w{1,4,7,8}_crop_F462_489.png`; Ueberblick
  `<sp>/abn0_bilder/K_w8_F460_506.png`. F464-F482 Leon steht mit dem Ruecken zur Kamera an der Kurbel (Gier 1024 aus
  `34 04 00 04` @0x00A94), leichtes Wiegen; F483 Uebergang (Crossfade 7 Bilder @0x80050cfc) in den Kurbel-Clip; danach
  Kurbeln. **Kein Fliegen, Springen oder Gleiten — mit keiner der vier Waffen.**

### 3.6 Original-Gegenprobe in DuckStation (Fortsetzung 2026-10-24) — Stand
Vorbereitung (nachpruefbar): Debug-Menue-Basis `stage_saves/mzd_debugmenu.sav` mit `re15_ss_patch.py` gepatcht ->
`<sp>/abn0_orig/menu_kurbel.sav`: `0x800b1004:4:0x2000` = Flag (3,0x72) (Bank-3-Basis 0x800b0ff8 aus der Op_set-Tabelle
@0x80074664[3]; Bit 114 -> Wort 3, Maske 0x80000000>>18 = 0x2000 nach dem Setzer FUN_8004ef90 `srl v1,a1,5` /
`lui v0,0x8000` / `srlv v0,v0,a1` @0x8004ef90-a8), JUMP-Satz 0x202 "ZOMBIE TUNNEL" (RAM 0x800c2b68, DEBUG.BIN @0x2b68,
Satzformel @0x8001476C-A8) X 11350 -> 2600 / Z -3150 -> -2800 (Band 1 bleibt; der Debug-Zweig des JUMP-Executors schreibt
X/Z/Band @0x8001d6b4/@0x8001d72c/@0x8001d7a4, aber keine Gier), Gier 0x800acabe -> 1024 — derselbe Startpunkt wie
`RE15_PLAYER_POS=2600,-2800,1024,1` der Port-Laeufe. Treiber `<sp>/abn0_orig_kurbel.py` (Quick-Load, Hoch x2 / Runter /
Dreieck / Rechts x2 / Quadrat = Stage 2 Raum 0x202; ToggleMediaCapture voruebergehend auf SDL-0/LeftShoulder wie Spur A,
settings.ini bytegleich zurueck; Close per `taskkill /PID` ohne /F, nie /IM).
Ergebnis bisher (Laeufe v1..v4): DuckStation startet und beendet sauber (playtime.dat aktualisiert), aber WEDER
SaveStateOnExit (resume.sav) NOCH der LB-Hotkey (Slot 1) schreiben einen Zustand. Befund dazu: die aktuelle settings.ini
steht auf `ConfirmPowerOff = true` (die Sicherung `settings.ini.vor_ffmpeg_2026-10-23` von 17:25 hatte `false`; der Skill
re15-room-capture verlangt `false`); das DuckStation-Fenster war in der Desktop-Aufnahme (`<sp>/abn0_orig/diag/desk.mp4`)
nicht im Vordergrund. -> siehe 3.7 (wird ergaenzt).

## 4. Punkt 2 — Karte B1, Ostkammer-Tuer ROOM1230 -> ROOM1190
(Fortsetzung 2026-10-24, Abnahme 0 zweiter Pruefer; `<sp>` = Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`.)

### 4.1 Zwei exe, derselbe Weg
- **NACHHER** = HEAD: `re15_port/build/platform/pc/re15_pc_abn0b.exe` (md5 757da3a9132018190427a32a39b67d96 = re15_pc.exe
  nach `local_build.sh configure`+`build`, "ninja: no work to do").
- **VORHER** = Basis 116e8b0f, selbst gebaut: `git archive 116e8b0f -- re15_port ':(exclude)re15_port/shared_assets'` nach
  `<sp>/abn0_base`, Junction `shared_assets` -> Assets des Spurbaums (der Zweig aendert sie nicht), `local_build.sh configure`,
  `ninja re15_pc test_map_uebergang` -> `<sp>/abn0_base/re15_port/build/platform/pc/re15_pc_abn0_basis.exe`
  (md5 2310696431b26d115bc00f3796663c01; Log `[asset] cd-root: <sp>/abn0_base/re15_port/shared_assets/PSX`).
- Laeufe `<sp>/runs_abn0/<lauf>/` (cmd.txt, debug.log, state.txt, map.bmp), Helfer `<sp>/abn0_run.sh`. Gemeinsam:
  `RE15_DEBUG_JUMP=1230@240 RE15_SET_FLAG=4:0xf3,3:0x89,7:0x88,7:0x89 RE15_PLAYER_POS=96,14500,1024,0
  RE15_INPUT_SCRIPT="U<t>,W0.2,S0.1,W1.0,Q0.1" RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.txt
  RE15_INV_FB_SHOT=map.bmp RE15_INV_FB_SHOT_AT=120 RE15_MAP_SHOT_PAGE=0 RE15_EXIT_AT=560` (S = START/Statusschirm, Q = L1 =
  Karten-Sofortstart, input_pc.c:86/91).
- Flags: (4,0xF3) Strom; (3,0x89) -> ROOM1230 main00 @0x00B94 Ifel (3,0x89)==0 sonst @0x00CE2 Else -> Slot 4
  `Door_aot_set` @0x00CE6 (rect (-1052,11694,2297,934), next (19515,0,-12109), ROOM1190, Cut 12); (7,0x88)/(7,0x89) = Kill-Flags
  der zwei Hunde `Sce_em_set` @0x00D08/@0x00D1C (pc[7] = 0x88/0x89; Spawn-Gate FUN_800420a0 @0x80042120-38, Stage 1 = Zone 7,
  scd_vm.c:3861-3869). Ohne sie beissen die Hunde (erste Laeufe `P2_*`: hp 100 -> 40, Lauf stockt bei z 13252) — die Lage waere
  verfaelscht. `RE15_MAP_SHOT_PAGE=0` deckt nur die Zeichnung von Blatt 0 (B1) auf; die Markerrechnung bleibt unberuehrt.
- Marker = Mittelpunkt des 8x8-Rings (re15_inv_screen.c:2233 `sprt(... map_marker_x - 4, map_marker_y - 4, 8, 8, 224, 128,
  ecg_glow ...)`), aus map.bmp per Texelfilter B<=8, |R-G|<=8, R>=0x18 (`<sp>/abn0_marker.py`; 13 Ringtexel je Bild).
  Marken laut Dossier/re15_map_zones.h:986/987: Hallen-Tuer (127,115), Ostkammer-Tuer (146,109).

| Lauf | Spieler am Ende (state.txt / [walk]) | Marker VORHER | Marker NACHHER |
|---|---|---|---|
| U0.4 | (96,13600), dz 1439 > Zugbereich hh 1384 | (145,105) | (145,105) |
| U0.6 | (96,13150), dz 989 | **(137,109)** | (145,106) |
| U0.8 / U1.0 / U1.5 (Lauf endet an der Tuer) | (96,12779), dz 618 | **(130,112)** | (145,107) |
| Elza: `RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=1231@240`, U1.5 (PL04, room1231.rdt) | (96,12779) | **(130,112)** | (145,107) |
| Panel-Weg (erster Besuch): ohne (3,0x89), `RE15_FORCE_EVENT=19@262` = sub19 @0x01428 (@0x0143C Aot_reset Slot 4 sce 0, @0x01450 Aot_reset Slot 6 sce 2 next (19515,0,-12109)), Text "You've opened the lock." per Aktionstaste weg, dann U1.2 | (96,12779) | **(130,112)** | (145,107) |
| Rueckkehr aus der Ostkammer: Ankunftspunkt ROOM1190 sub14 Slot 6 @0x02B40 next (142,0,13001), PLAYER_POS 142,13001,3072 | (142,13001) | **(134,111)** | (145,107) |
| Hallen-Tuer Slot 2 @0x00B54: PLAYER_POS -8118,11200,1024, U1.5 | (-8118,9944) | (126,113) | (126,113) |
| ECHTER Tuerweg Leon: `U1.0,W0.3,A0.1`, `RE15_INV_OPEN_AT=60#1190` -> `[tuer] Sequenz Archiv 2 DOOR19 ... 301 Bilder`, room1190.rdt, Cut 12 | (19515,-12109) | (145,108) | (145,108) |
| ECHTER Tuerweg Elza 1231 -> 1191 (gleich, `RE15_INV_OPEN_AT=60#1191`) | (19515,-12109) | (145,108) | (145,108) |

**VORHER reproduziert die exe den Nutzer-Befund:** je naeher Leon bzw. Elza an die vordere Tuer (Ostkammer) tritt, desto
weiter wandert der Marker den Gang-Arm entlang nach Westen bis an die Hallen-Tuer ((130,112), 15 px) — "Sprung zum hinteren
Teil, obwohl man garnicht da ist"; nach dem Durchgang springt er zurueck in die Ostkammer (145,108), und beim Rueckweg aus der
Ostkammer erscheint er wieder an der Hallen-Tuer ((134,111)). **NACHHER** bleibt der Marker in allen Lagen an der Ostkammer-
Marke ((145,105..107)) und geht beim Durchgang um 1 px weiter ((145,108)). Hallen-Tuer und Ankunft unveraendert.
Bilder: `<sp>/abn0_bilder/P2_leon_reihe_vor_nach.png` (oben vorher, unten nachher: z 13600 / 13150 / 12779 / Ankunft 1190),
`<sp>/abn0_bilder/P2_leon_x96_vor_nach.png`, `<sp>/abn0_bilder/P2_elza_1231_und_panel_vor_nach.png`.

### 4.2 Erklaert der Fix den Befund?
Ja, quantitativ: Zug in tuer_anziehen mit `RE15_ZUGKURVE` (Default 1, re15_inv_screen.c:655 `wert = e ? atoi(e) : 1`):
hh = 467*2+450 = 1384, th = 467; fz = (hh-dz)*256/(hh-th). dz 989 -> 110/256, dz 618 -> 213/256. Mit Ziel = Hallen-Marke x 127
und unverzogenem Marker x 145 (Lauf U0.4): 145 - 18*110/256 = 137,3 -> **137** und 145 - 18*213/256 = 130,0 -> **130** — genau
die gemessenen VORHER-Werte. Mit Ziel = Ostkammer-Marke x 146: 145 + 1*213/256 -> 145 (gemessen 145). Die Wanderung ist also
der Tuer-Zug auf die falsche Marke (Ursache §2 Nr. 7), und die neue Wahl der Marke ueber die Zone der Tuer-Ankunft
(karte_r41.c: `re15_map_zone_at(ziel, door_params[k].spawn_x/z)`) beseitigt sie. Auch der Panel-Weg haelt: Aot_reset setzt
door_params ueber `re15_aot_retype` mit `(int16_t)p2` = -12109 (aot_common.c:293-295), die Ankunftszone stimmt.
Die Ankunftszone vergleicht die Zonen-Nummer (zid); Gast-Zeilen anderer Blaetter tragen dieselbe zid wie die Hauptzeile
(re15_room.h:108 Kommentar zu `etage`), `re15_map_zone_at` ueberspringt sie (re15_map_zones.c:320) — kein Blatt-Problem.

### 4.3 Nebenwirkungen (alle Tueren)
`integration_map_uebergang` (laedt JEDEN Raum wie das Spiel, liest die Tueren aus g_aot, ruft die echte Marker-Funktion an
Trigger und Gegen-Spawn) mit `RE15_UEBERGANG_LISTE=1`, einmal mit dem Basis-Bau (`<sp>/abn0_base/.../test_map_uebergang.exe`) und
einmal mit HEAD: beide Listen 226 Zeilen, **`diff` leer** (`<sp>/abn0_uebergang/{vor,nach}/liste.txt`; 214 Tueren, 151
Uebergaenge auf 12 Blaettern, Median 6 px vorher wie nachher). Keine andere Tuer zieht anders. Grenzen des Werkzeugs: nur
Leon-Raeume; ROOM1230 -> 1190 faellt dort als "AUSFALL ohne Gegen-Tuer" heraus — deshalb die exe-Laeufe in 4.1.

### 4.4 Urteil Punkt 2: **erfuellt**
Am echten Spiel gemessen, Leon UND Elza, Annaeherung, Panel-Weg, Durchgang (echte Tuer) und Rueckkehr: kein Sprung des
Markers in den hinteren Teil mehr; VORHER-exe zeigt den Befund an derselben Stelle (15 px zur Hallen-Tuer). Riegel
`unit_r41_kleinkram_b_karte_pin` (17 Pruefungen) gruen (§1).

## 5. Punkt 3 — Werkstatt-Tuer ROOM11B1 (Elza)
### 5.1 Messung D1 — ECHTER Weg: Elza aus ROOM1261 durch die Schacht-Tuer nach 11B1, Szene laeuft
`RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=1261@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3 RE15_PLAYER_POS=27650,9450,1024,1
RE15_PRESS=square@300,square@310 RE15_SCD_TRACE=1 RE15_CAM_TRACE=1 RE15_MSG_LOG=1 RE15_FRAMEDUMP=320-900/6:fd_
RE15_EXIT_AT=920` (`runs/D1_elza_1261_band1/`; Band 0 traf die Tuer nicht — Band-Gate @0x80042cac, Lauf band0 blieb in
1261). Flags = Zustand nach 10B1 sub04 @0x0195E (Schacht offen) und 2001 @0x01958 (Sherry-Suche), Tuer 1261 Slot 0
@0x0112A rect (27150,8700,1000,1500) → 11B1 (-195,0,-21821) Cut 6.
- debug.log Reihenfolge: `[room] PC loaded room11b1.rdt` → `[wk11b1-r41] ROOM11B1 (3,0x83)=1: Ereignis 24 im Init-Lauf
  hinter sub00 …` → Init-Lauf F300: slot 0 main00 @0x0F4E..@0x10E6, **slot 1 sub00 @0x10F6 … @0x110E 4B, @0x1112 4B,
  @0x1116 37, @0x111A 39, @0x111E 2E, @0x1122 34, @0x1126 07, @0x115A 01**, **slot 10 (Ereignis 24, statisches
  Programm) 4B 00 4B 00 37 39 2C 06 23 06 23 01**, dann slot 1 weiter (sub01 @0x115C…) — alles im selben Init-Lauf vor
  dem ersten Bild (Dispatcher-Form @0x8003f17c `addiu s2,s2,1 / sltiu s2,0xa`: Slots der Reihe nach).
- Szene sub03: `[scd] F6 @0x1438`, `[msg] id=10 "Elza: Sherry!?"` F84, `[msg] id=11 "Elza: Where has she gone!?"` F372,
  `@0x14E6 Plc_ret` F473. Kamera: Cut 6 → 5 (F188) → 4 (F236) → 3 (F279) → **2 (F302) und bleibt 2** bis F660+
  (`[cam-trace] … req(cam_id)=2 shown=2 pl=(-18192,-20453)`), nicht 8 (= offenes Tuerblatt). Bilder
  `sheetD1_320_560.png`, `sheetD1_566_900.png`, `sheet_D1_detail.png`: Elza steht vor dem GESCHLOSSENEN Tuerblatt
  (graues Paneel links hinten), kein offener Durchgang, kein Bildflackern.
### 5.2 Messung D2 — Elza gegen die Tuer, Aktionstaste
`RE15_DEBUG_JUMP=11b1@240 RE15_SET_FLAG=3:0x83,4:0xf3 RE15_PLAYER_POS=-20200,-16230,2048,0
RE15_INPUT_SCRIPT="U1.5,W0.3,A0.1" (Start F262, Basis spiel) RE15_STATE_LOG=state.txt RE15_MSG_LOG=1
RE15_FRAMEDUMP=258-360/6:fd_ RE15_EXIT_AT=380` (`runs/D2_elza_11b1_tuer/`).
- state.txt: F260 PL(-20200,-16230) → F264 (-20425) → F268 (-20725) → **F272..F304 (-20942,-16230) blockiert**
  (Zellkante -21410 + Radius), mo=105 (Lauf gegen die Wand), kein Raumwechsel; F316 Quadrat → debug.log
  `[msgtext] room=11b1 id=12 "The door is locked."`; `EXIT_AT: Bild 380 in Raum 11b1` (kein Raumwechsel).
  Bild `sheet_D_tuer_detail.png` (F258/F288/F324): Elza laeuft an das geschlossene Paneel, Text "The door …".
### 5.3 Beleg / Vertrag
NUTZER-VORGABE (AUFTRAG.md). Form = Runde 36 J (Ereignis im Init-Lauf hinter sub00), Programm Byte fuer Byte aus der RDT
(§2 Nr. 4-6). Nur ROOM11B1; kein Bank-9-Bit, keine neue Nachricht (msg 12 ist Original), kein Asset. Leon 11B0 unberuehrt
(unit_r41_kleinkram_b_11b1_original + r36_szenenleon/r38_werkstatt11b0 gruen). Rueckweg fuer Elza ueber den Schacht
(1261 Slots 2/3 → 10B1/11D1, Slots 1/4 → 2001) — kein Softlock.
### 5.4 Nebenbefund (nicht Teil des Punkts)
Die Szene fuehrt Elza per Plc_dest 09 @0x14B6 nach (-22695,-20321) WESTLICH der Trennwand; im Port endet sie schon bei
(-18192,-20453) (vorher wie nachher, Dossier w6/n13 und mein Lauf D1) — vermutlich Container obj 0 (Member_set z=-19368
@0x01122). Mit dauerhaft geschlossener Tuer ist das Ziel ohnehin unerreichbar; sichtbar ist nur, dass Elza vor dem
Container stehen bleibt und dort "Where has she gone!?" sagt. Kein Fehler dieser Spur.
### 5.5 Urteil Punkt 3: **erfuellt**
Echter Weg (Tuer aus 1261, Szene) gemessen: Tuerblatt im Bild zu (Cut 2 statt 8), Zelle solid (Lauf blockiert), Text
"The door is locked.", kein Raumwechsel; Wiederbetreten im Riegel geprueft (unit_r41_kleinkram_b_11b1_zu).

## 6. Vertrag / Gates
(wird ergaenzt)

## 7. Maengel
(wird ergaenzt)

## 8. Urteil
(wird ergaenzt)
