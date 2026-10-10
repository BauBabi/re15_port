# Runde 36 — Spur C "schuss": Abnahme 1 (unabhaengig, 2026-10-10)

Baum `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, geprueft am Commit 89e58fc1 (Nachbesserung 1 nach
Abnahme 0, `C_abnahme_0.md` M1-M7). Nur gelesen/gemessen, kein Code im Baum geaendert. Eigene Sonden/Skripte/Logs
liegen im Scratchpad dieser Abnahme (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn1C_r36/`
(Kurzform `abn1C/...`).

(Bericht abgeschlossen 2026-10-10.)

## 0. Gelesen / gebaut
* AUFTRAG.md (Wortlaut der drei Punkte), VERTRAG.md (Spur C, §1.4, §2), Dossier C_schuss.md (§0-§8.12),
  C_abnahme_0.md (M1-M7), `git log master..HEAD` (25 Commits inkl. 2 Merges), `git diff master --stat`
  (34 Dateien, +3129/-124), Code-Diff engine/include/tests vollstaendig gelesen.
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
  re15_pc.exe mtime 2026-10-10 20:03:59; einzige Quelle juenger als die exe: tests/unit/probe_r26_mg_blut.c
  (Test, eigenes Ziel); `git status re15_port` leer.

## 1. RE-Gate — selbst disassembliert (re2_disasm.py / re15_disasm.py, Hauptbaum-Skill, richtige Binaerdateien)
| Stelle (Binaerdatei) | Behauptung im Diff/Dossier | Selbst gelesen | Urteil |
|---|---|---|---|
| EMZ0.BIN @0x80107680-708 (Arm-Abriss) | a3 (0,0,0)/(0,400,0)/(0,800,0) an Part 3, Winkel 0 / r<<2 / r<<4, 4. Spawn a3 = 0 | `addiu a0,zero,5000` / `addu a1,zero,zero` / `addiu s1,sp,24` / `sh zero,24/26/28(sp)` / `addiu s0,s0,588` (3*172+72) / `jal 0x8001bf10` @0x801076a4; `addiu v0,zero,400` / `jal rand` / `sh v0,26(sp)` (Delay) / `sll a1,v0,2` / `jal` @0x801076c4; `addiu v0,zero,800` / `sll a1,v0,4` / `jal` @0x801076e4; `lui a0,0x800` / `ori 0x1000` / `sll a1,v0,4` / `jal` @0x80107704 / `addu a3,zero,zero` | wie behauptet |
| EMZ0.BIN @0x80105F5C-60C (Stagger-P1-Nachbluten) + @0x80105458-70 (Einstieg) | nur gerades +0x15A, Zweig B (+0x222 != 0): +0x146 -= 500, Id 5096, Part 0, a3 = +0x144, Winkel +0x76 + r1<<4 - r2<<4; Stagger nur bei +0x222 == 1 | `lh v0,346(s4)` / `beq` / `lb v0,362(s4)` / `beq` / `andi v0,v1,0x1` (Delay) / `bne v0,zero,0x80105ffc`; `lbu v0,546(s4)` / `bne -> 0x80105fbc`; B: `lhu v0,326(s4)` / `addiu v0,v0,-500` / `sh v0,326(s4)` (Delay von `jal rand`) / 2. `jal rand` / `addiu a0,zero,5096` / `addiu a2,s3,72` / `addiu a3,s4,324` / `sll s0,s0,4` / `lh a1,118(s4)` / `sll v0,v0,4` / `addu a1,a1,s0` / `jal 0x8001bf10` / `subu a1,a1,v0`; Einstieg `lbu v0,546(s1)` / `bne v0,a0(=1)` / `jal 0x80105bc0` @0x80105458-68; P2 `sb zero,546(s4)` @0x80106034 | wie behauptet (Port: re2z_flag222 == 1 als Einstieg, enemy_ai_re2_zombie.c:5363) |
| EMZ0.BIN @0x8010864C-6B4 (TOD) | a3 = sp+24 in beiden Zweigen; Zone!=0 Part 0 (P8.x, P8.y+300, P8.z) Id 8096; Zone 0 Part 1 (0,800,0) Id 6096 | `beq a0,zero,0x80108694` / `addiu a3,sp,24` (Delay); `lw v0,1420(v1)` / `sh v0,24(sp)` / `lw v0,1424(v1)` / `addiu a0,zero,8096` / `addiu v0,v0,300` / `sh v0,26(sp)` / `lw v0,1428(v1)` / `sh v0,28(sp)` / `lh a1,118(s1)` / `addiu a2,s2,72`; Zone 0: `addiu a0,zero,6096` / `addiu v0,zero,800` / `sh zero,24` / `sh v0,26` / `sh zero,28` / `addiu a2,a2,244`; `jal 0x8001bf10` @0x801086b4 | wie behauptet |
| EMZ0.BIN @0x8010887C-8D4 (Tropfblut Tod P1) | a3 (P8.x, P8.y+500, P8.z), Id (r1<<3)+4048, Winkel r2<<4, Part 0 | `lw v0,1420/1424/1428(v1)`, `addiu v0,v0,500`, `sh 24/26/28(sp)`; `jal rand` / `sll s0,v0,3` / `jal rand` / `addiu s0,s0,4048` / `sll a1,v0,4` / `addiu a3,sp,24` / `jal 0x8001bf10` / `addiu a2,s2,72` | wie behauptet |
| EMZ0.BIN @0x80107FC8-0C4 (MG-Stoss) | Zone!=0 Part 0 a3 = (256-2r, 312-4r, 256-2r); Zone 0 Part 1 (127-r, 927-r, 127-r) | `addiu s0,zero,256` / `sll v0,v0,1` / `subu` / `sh 16(sp)`; `sll v0,v0,2` / `addiu v1,zero,312` / `subu` / `sh 18(sp)`; ... `addiu a3,sp,16` / `addiu a2,s1,72`; Zone 0 `addiu s0,zero,127` / `addiu v1,zero,927` / ... / `addiu a2,s1,244` / `jal 0x8001bf10` @0x801080c4 | wie behauptet (Port re2z_hit_light gleich) |
| EMZ0.BIN @0x801068A0-94C, @0x80106D7C, @0x801071A0 (Zensus-Gruppe B) | a3 = 0 | `jal 0x8001bf10` @0x801068d8 / @0x80106908 / @0x80106940 je mit `addu a3,zero,zero` im Delay-Slot; @0x80106d7c / @0x801071a0 ebenso | wie behauptet (Port: a3 = NULL ist dort richtig) |
| EMOVL21_S0.BIN Tabelle @0x80104A18 (Kraehe HURT-Dispatch) | [5]/[6] GIB 0x80102CA0, [7]/[8] Launch, [9..11]/[17] GIB | `table`: [0..4] 0x80102934, [5] [6] 0x80102ca0, [7] [8] 0x8010327c, [9] [10] [11] 0x80102ca0, [12] 0x80102fb4, [13..16] 0x80102934, [17] 0x80102ca0, [18] 0x80102934 | wie behauptet |
| RE1.5 PSX.EXE @0x80012150-184 + FUN_8001b9b4 | `jal 0x8001b9b4` mit a0 = Gewinner+52 @0x80012168, `bne v0,zero,0x80012540` @0x80012170 VOR `lbu v0,8(s1)` / `sltiu 0x40`; FUN_8001b9b4 Regionen 3..0 mit 0xf00/0x300, ODER | wortgleich gelesen (`ori s0,zero,0x3` @0x8001ba08, `ori a2,zero,0xf00` / `jal 0x8003dcc4` / `ori a3,zero,0x300` @0x8001ba1c-24, `or s1,s1,v0` @0x8001ba28, `bne` / `addiu s0,s0,-1` @0x8001ba34-38) | wie behauptet |
Ergebnis: alle 8 Stichproben enthalten genau das Behauptete. (Abnahme 0 hatte dazu FUN_800410CC-Gates, EXEC[8]-Latches,
Deskriptor 7/8 und den Lagen-Tick 0x8001D894 geprueft; M5 ist im Code berichtigt, s. §5.)

## 2. Punkt 3 — "Mit der Super Redhawk zerplatzen/verschwinden die Kraehen komplett — ist das in RE2 auch so?"

### 2.1 Beleg (selbst, §1): EMOVL21_S0.BIN Tabelle @0x80104A18 — Zeile 5 (Magnum, Port w7 -> s_re2c_row_from_weapon[7] = 5)
-> 0x80102CA0 = GIB; Zeilen 0..4 (Pistolen) -> 0x80102934 Flug-Treffer, 7/8 (Schrot) -> 0x8010327C Launch. Die Antwort des
Dossiers §3.0 ("Ja, in RE2 zerplatzt die Kraehe beim Magnum-Treffer ebenfalls, 13 Teile fliegen ~0,5-1 s, kein Kadaver,
keine Lache") stimmt mit dem Dispatch ueberein; WURF/WIRBEL-Felder hat Abnahme 0 wortgleich gegen 0x80102EF4/0x80102F34
geprueft, der Port-Code (schuss_r36.c kraehe_wurf/kraehe_wirbel/re15_schuss_r36_kraehe_gib) ist seitdem unveraendert
(`git diff 1e8cd231 HEAD`: enemy_ai_re2_crow.c unveraendert, schuss_r36.c nur im Punkt-2-Teil re15_schuss_r36_blut_anker).

### 2.2 Messung an der exe (eigene Messkopie `re15_pc_abn1c.exe`, `abn1C/p3_batch.sh`)
Aufbau: `RE15_DEBUG_JUMP=10C0@250`, `RE15_PLAYER_POS=3522,5556,581`, `RE15_FORCE_CUT=1`, RE2-KI, `RE15_GIVE=<w>:6`,
`RE15_INPUT_SCRIPT=MD0.6,MDA0.4,MD1.0,W3` (Tiefzielen, Kraehe sitzt), `RE15_FRAMEDUMP=14-70/1`, `RE15_WINDOW_SCALE=1`,
`RE15_EXIT_AT=110#10C0`; alle vier Laeufe exit=0, je ein Schuss (`wf.log arms_rec=0 bank=<w>`). state.log Platz 3 (Kraehe):
| Waffe | Zustandsfolge Platz 3 | Bild (abn1C/...) |
|---|---|---|
| 7 Super Redhawk | F6 st1 hp10 -> **F19 st=7 ss1=1 hp=-1** (GIB-Leiche 0x80103C0C) | `p3_w7_sheet.png`: F18 Kraehe sitzt; F19 Treffer; F20-F28 dunkelrote Scherben/Teile fliegen von Leon weg auseinander; F30 schwach; ab F34 nichts mehr — kein Kadaver, keine Lache |
| 3 Browning (Gegenprobe) | F19 st=3 ss1=3 hp -5 -> F31 st=7 ss1=0 | `p3_w3_sheet.png`: Kraehe schlaegt mit den Fluegeln, faellt, liegt mit Blutlache (F48) |
| 8 Remington | F20 st=3 ss1=8 -> F39 st=7 ss1=3 (Launch-Weg, kein GIB) | — |
| 20 Colt Python | F19 st=3 ss1=20 -> F31 st=7 ss1=0 (Flug-Treffer, kein GIB) | — |
Nebenbefund (nicht Teil des Wortlauts, vor Runde 36 entstanden): fuer Kraehen bildet der Port die Python auf RE2-Zeile 13
ab (`s_re2c_row_from_weapon[20] = 13`, enemy_ai_re2_crow.c:1479, Commit c2adafe3), fuer Zombies auf Zeile 5
(`re2z_row_from_weapon[20] = 5`) — der Kommentar "Werte == re2z_row_from_weapon" stimmt an dieser Stelle nicht. Die
Python laesst Kraehen deshalb NICHT zerplatzen, Zombies aber den Magnum-Tod sterben.
* Tests: `unit_r36_schuss_kraehe` (Felder/Draws/Physik/Dispatch w7 -> GIB, w3 -> Flug-Treffer), `unit_re2_crow_ai`
  (angepasst: 13 Zeilen mit Bit 0x40 statt crow_hide) — Ergebnis s. §6.

**Urteil Punkt 3: erfuellt.** Frage beantwortet und am Original belegt (RE2 laesst die Kraehe beim Magnum-Treffer ebenfalls
zerplatzen); der Port zeigt jetzt das RE2-Zerplatzen (sichtbare Teile ~0,4-0,5 s, danach nichts) statt des sofortigen
Verschwindens.

## 3. Punkt 2 — "manchmal das Gefuehl, dass das Blut nicht an der richtigen Stelle auftaucht, wenn sie getroffen werden"

### 3.1 Eigene Sonde `abn1C/abn1_blut.c` (nicht im Repo; Aufbau wie probe_r36_blut: ROOM1140, RE2-KI, RE2-Bank CDEMD0.EMS,
echter game_step, gebaut mit den Flags aus build.ninja gegen engine/libre15_engine.a + tests/libre15_test_support.a)
Unterschied zur Sonde des Bau-Agenten: Waffe/Anlaufrichtung/Zielhoehe frei (Gier != 0, Magnum-/Schrot-/MG-Tode), EIGENE
Posenrechnung fuer ALLE Knochen (gezeichnete Pose wie main.c, Bilder T-1 und T), jeder Ring-Spawn wird gegen
Anker.R*a3 + Anker.T seines eigenen Parts gemessen, und sein a3 wird EXAKT gegen die Vektorfamilien aus §1 geprueft
(kein "naechster Kandidat"). Laeufe (Ausgaben `abn1C/blut_w<w>_<seite>_<hoehe>.txt`), alle bis Tod + 120 Bilder:
| Waffe / Lauf | Tod | Spawns (ohne a3) | a3 ausserhalb der Original-Familien | d(Anker.R*a3+T) max (Spawns mit a3) |
|---|---|---|---|---|
| 3 Browning, +x, Rumpf | F354 | 18 (0) | 0 | 24 (HURT d=1, NACH 5x d=1, AUF d=24/6, TOD d=1, Tropfblut 6x d=0-1) |
| 3 Browning, +z (Gier != 0), Beine | F186 | 12 (1) | 0 | 32 |
| 4 SIG, -x, Rumpf | F354 | 18 (0) | 0 | 24 |
| 5 Beretta, +x, Beine | F186 | 12 (1) | 0 | 32 |
| 7 Super Redhawk, +x | F66 | 3 (0) | 0 | 1 (MAGNUM: (P8.x,P8.y+300,P8.z) dann 2x (P8.x,P8.y-100,P8.z) an Part 0) |
| 20 Colt Python, +z | F66 | 3 (0) | 0 | 1 (wie Redhawk) |
| 8 Remington, +x / -x / +z Beine | F66 | 2 (2) / 2 (2) / 4 (1) | 0 | 0-1 (Arm-Abriss (0,0,0)/(0,400,0)/(0,800,0) an Part 3 d=0-1) |
| 13 SPAS, +x / +z / -x / +x Beine | F66 | 13 (11) / 13 (11) / 13 (11) / 4 (1) | 0 | 0-1 (RIP (0,-500,0) 2x) |
| 12 Ingram, +z | F102 | 33 (2) | 0 | 58 (MG-Zufallsvektor; Zombie dreht/schiebt NACH dem Spawn @0x8010810C-6C, Schranke 100 im Pin) |
| 19 MC51, -x / +z | F102 | 19 (0) / 19 (0) | 0 | 40 / 43 |
Befund: JEDER Spawn mit a3 traegt exakt einen Original-Vektor (HURT s=+-1, TOD, FLINCH/Tropf, NACH/RIP, MAGNUM 1/2/3, AUF,
ZONE0, Arm, MG-Band), und die Lage ist Anker.R*a3 + Anker.T der im selben Bild gezeichneten Pose (d <= 32; MG <= 58).
Der Abstand zum alten Defekt (Anker ohne a3) betraegt je Spawn 278..884 (MG-Zufallsvektoren ab 180; ausgenommen die a3-(0,0,0)-Spawns des Arm-Abrisses) — die Pins des Bau-Agenten (Schranke 40) sind
trennscharf. Die Pins selbst nachgefahren: `probe_r36_blut pin eben` -> `Treffer 7, Spawns 18, Tod bei Bild 354, Tropfblut
nach dem Tod 6` / `probe_r36_blut: OK`; `pin tief` -> `Spawns 12, Tod bei Bild 186` / OK; `pin mg` -> `MG: 22 Stoesse,
Summe d(Anker-Modell) 590, Summe d(Welt-Versatz) 6665` / OK (Zahlen = Dossier §8.3).

### 3.2 exe (eigene Messkopie, `abn1C/p2_batch.sh`; ROOM1140 per `RE15_DEBUG_JUMP=1140@250`, Leon (-5000,-19600) Blick +x,
RE2-KI = Hybrid-Rig der exe (RE2-Gehirn/-Animation + RE1.5-Netz/-Bindlaengen, main.c:2511-2523), `RE15_FX_LOG`,
`RE15_FRAMEDUMP`, `RE15_WINDOW_SCALE=1`; alle vier Laeufe exit=0)
* Pistole Rumpf (`p2/pistole_x`): state.log Platz 2 F55 50->34 (Treffer im Aufstehen), F77 ->18, F99 ->2, F121 2->-14 st 3;
  fx.log erste Blut-Bilder: F55 `w(-1781,-2163,-19518)`, F77 `w(-1723,-2140,-19787)`, F99 `w(-1867,-355,-19977)`, F121
  `w(-1862,-2176,-20446)`, Tropfblut F126..F136 y -1934..-1199 — bitgleich mit den nachher-Werten des Dossiers §8.9.
  Bild `abn1C/p2_pistole_sheet.png` (F54-57/F76-79/F98-101/F120-123, 3x): Blut im Aufstehen an Nacken/Schulter, beim
  Rumpftreffer auf Brusthoehe zur Schuetzenseite, beim Beintreffer am Knie, beim Tod an Brust/Schulter.
* Pistole Beine (`p2/pistole_tief`, Eingabe MD..): Blut F78 `w(-1490,-974,-19669)` / F101 `w(-1677,-241,-21109)`;
  `abn1C/p2_tief_sheet.png`: Oberschenkel/Knie.
* SPAS (`p2/spas_x`): Platz 2 stirbt im Aufstehen (F55 -> st 3 ss1 8), Platz 3 F76 (st 3 ss1 8); `abn1C/p2_spas_sheet.png`:
  Blut an Nacken bzw. Kopf/Oberkoerper.

### 3.3 Was der Fix NICHT abdeckt (gemessen) — Zensus-Gruppe B (a3 = 0) ankert an der ROHEN Pose
Die a3 = 0-Emitter laufen weiter ueber re2z_gore_fx_ex -> re15_enemy_bone_world_pos (`g_anim_pose_actor = NULL`, keine
Crossfade-Blende, re15_damage.c:3205-3210), die a3-Emitter dagegen ueber re15_schuss_r36_blut_anker (gezeichnete Pose).
Das Dossier fuehrt das als OFFEN 11 ("Unterschied nur waehrend eines Crossfades (gemessen bis 98)"). Jeder Treffer/Tod
startet aber einen Clipwechsel mit Blende, also genau das Bild, in dem Blut entsteht. Gemessen (Abstand Spawn ->
gezeichneter Part im selben Bild, `abn1C/blut_w*.txt`, Spalte "Abstand zum Anker ohne a3"):
| Lauf | Emitter (Original, a3 = 0 selbst gelesen) | Abstand zur gezeichneten Lage |
|---|---|---|
| SPAS, Tod (alle drei Anlaufrichtungen gleich) | RIP-Kopf: `addiu a0,zero,3000` / `addiu s0,s0,1448` (Part 8) / `jal 0x8001bf10` / `addu a3,zero,zero` @0x80108E18-2C | **442-443** (der Kopf startet seinen Flug an der GEZEICHNETEN Lage: re15_re2z_gore_part_matrix seedet aus der Renderer-Pose, enemy_ai_re2_zombie.c:4758-4766) |
| SPAS, Tod | RIP-P1-Nachbluten @0x80109108 / @0x8010913C (a3 = +0x144, `addiu s2,s3,324` @0x801090DC; Port NULL) | 109 (F66), 15-16 (F68), 0 danach |
| Ingram, Treffer F76 | Glied-Abriss Part 12/13: `addiu a0,zero,8000` / `addiu a2,s0,72` / `jal 0x8001bf10` / `addu a3,zero,zero` @0x8010535C-6C und `addiu a0,zero,7000` / ... @0x801053C4-D4 (Port re2z_gore_fx, enemy_ai_re2_zombie.c:5039/5057) | 168 / 140 |
| Remington/SPAS Beinschuss | Arm-Abriss 4. Spawn @0x80107704 (`addu a3,zero,zero`) | 35-37 — im selben Bild wie der 1. Spawn (a3 (0,0,0) ueber gore_fx_a3, d = 0): zwei a3 = 0-Spawns desselben Parts 35 auseinander |
Der Fix-Weg ist im Diff schon vorhanden (re15_schuss_r36_blut_anker liefert bei a3 = (0,0,0) den gezeichneten Anker.T,
so ruft ihn der 1. Arm-Abriss-Spawn), fuer die uebrigen ~30 Stellen aber nicht genutzt.
Einordnung: das Original setzt auch bei a3 = 0 an die Matrix, die der Zeichner geschrieben hat (Dossier §2.2 "Record +0x48");
der Port hat fuer die a3-Emitter als PORT-KONVENTION "gezeichnete Pose dieses Bilds" gewaehlt (§2.3/§8.2) — Gruppe B weicht
von genau dieser eigenen Konvention ab. Beim SPAS-Tod faellt das sichtbar aus: der Kopf fliegt ab der gezeichneten Lage los,
sein Blut entsteht 442-443 daneben.

### 3.4 Fix erklaert den Befund?
Ja fuer die Hauptursache: vorher sass jedes Blut der Treffer-/Todes-Emitter ohne a3 am Becken (Rumpf ~500 zu tief, Bein ~800
zu hoch, Tod ~480); jetzt traegt jeder a3-Spawn den Original-Vektor (exakt, §3.1) und liegt an Anker.R*a3 + Anker.T. Keine
Regression aus v0.8.22/23 (§5 M6). Nicht erklaert/behoben: die Gruppe-B-Lagen (§3.3).

**Urteil Punkt 2: teilweise.** Die Treffer-, Aufsteh-, Todes-, Tropf-, MG-, Magnum- und RIP/BURST-Emitter mit a3 sitzen jetzt
an der RE2-Stelle (eigene Sonde, 16 Laeufe, und exe-Bilder). Beim Abriss von Gliedern (MG), beim Arm-Abriss und beim
Schrot-RIP-Tod (Kopf, Becken) entsteht das Blut weiter 35-443 neben dem gezeichneten Koerperteil (Mangel M1) — der Wortlaut
"Blut ... wenn sie getroffen werden" umfasst diese Treffer.

## 4. Punkt 1 — "Alle Schusswaffen treffen ueber den Konferenztisch und treffen fressende Zombies. Im Original geht beides nicht."

### 4.1 Messung an der exe (eigene Messkopie `re15_pc_abn1c.exe`, `abn1C/p1_batch.sh` + `abn1C/p1b_batch.sh`, Auswertung `abn1C/hp.py`)
Aufbau je Lauf: `RE15_DEBUG_JUMP=<raum>@250` (normaler Raumwechsel re15_room_request_change), `RE15_PLAYER_POS`, `RE15_AI_FLAVOR`,
`RE15_GIVE=<w>:6`, `RE15_EQUIP=<w>`, `RE15_INPUT_SCRIPT=M0.6,MA1.5,M1.0,W4` (Basis spiel), `RE15_STATE_LOG`/`RE15_WAFFEN_LOG`/
`RE15_FX_LOG`, `RE15_EXIT_AT=200#<raum>`. Jeder Lauf: Munition `mg` faellt (Schuss belegt), HP je Platz = Start -> Minimum.
Waffen: alle 15 Schusswaffen-Ids 3/4/5/6/7/8/12/13/19/20 (Hitscan) + 14 (Flammenwerfer) + 15/16/17 (GL) + 18 (Rakete).
**Tisch (ROOM1140, Konferenztisch = SCA-Zelle 6 x -4650..7450 / z -17450..-11350, Band 0 / Klasse 3 — selbst aus der RDT
gelesen, `abn1C/sca.py`).** Unter RE2-KI stehen hinter dem Tisch nur gesperrte Fresser (Abnahme 0 M4); deshalb hier unter
RE1.5-KI, wo dieselben Fresser TREFFBAR sind (Kontrollen unten):
| Aufstellung | Waffen | Ergebnis |
|---|---|---|
| tisch_re15: Leon (200,-10300) Blick -z (wie Test) | 3,4,5,6,7,8,12,13,19,20 + 14,15,16,17,18 | alle 15: mg faellt (z.B. w3 6->3, w12 6->0, w15 6->4), KEIN Platz verliert HP (P1 65, P2 75, P3 65, P4 250, P5 250) |
| diag_re15 (EIGENE Diagonale): Leon (6000,-10300) rot 1387 -> Strecke zu Platz 5 (200,-19600) quert die Zelle von (5345,-11350) bis (1541,-17450) | dieselben 15 | alle 15: mg faellt, KEIN Platz verliert HP |
| Kontrolle frei_re15: Leon (200,-18200) hinter dem Tisch | 3,4,5,6,7,8,12,13,19,20 | ALLE 10 treffen: w3/4/5/6 P5 250->218 (F19); w7/w20 P5 F19 + P4 F44 -> -1; w8 P5 F20/P4 F23 -> -1; w12 P5/P4 -> 242; w13 P5/P4 -> -50, P2 -> -225; w19 P5/P4 -> 234 |
| Kontrolle frei_diag_re15 (EIGENE): Leon (6000,-18500) Blick -x, Strecke z -18500..-19600 unterhalb der Tischkante | 3,4,5,6,7,8,12,13,19,20 | ALLE 10 treffen: w3/w4 P5 -> 202, w5/w6 -> 218, w7/w20 P5 + P2 -> -1, w8 P5/P2/P3 -> -150/-125/-135, w12 P5/P2/P3, w13 P5/P4/P2, w19 P5/P2 |
=> Dieselben Ziele, dieselben 10 Hitscan-Waffen: mit freier Strecke Treffer, durch die Klasse-3-Zelle (gerade UND eigene
Diagonale) kein Treffer; Werfer 14-18 an der Tischzelle ebenfalls ohne HP-Verlust.

**Fresser-Sperre (RE2-KI = Vorgabe/Default):**
| Aufstellung | Waffen | Ergebnis |
|---|---|---|
| fresser_tuer: Leon am Tuerpunkt (-7600,-17600) Blick +x (= Sprung-Spawn = Original-Eintrittspunkt mzd_stage1_briefing.sav, re15_room_spawns.h:50), alle Fresser > 4000 | alle 15 (3..8, 12, 13, 19, 20, 14..18) | alle 15: mg faellt, KEIN Platz verliert HP (P2..P5 st1/ss1 8 = EXEC[8], P1 passiver Liegender st1/ss1 7) |
| fresser_eigen (EIGENER Standort): Leon (5000,-19000) Blick -x, freie Strecke unterhalb der Tischkante, Platz 5 in 4837 | alle 15 | alle 15: KEIN Platz verliert HP |
| Weck-Kontrolle weck_re2: Leon (200,-18200), Platz 5 in 1400 (Wecker dist < 0xFA0) | 3, 8, 12, 20 | geweckte Zombies treffbar: w3 P5 250->202; w8 P2/P3/P4/P5 -> -1/-120/-150/-1; w12 P5 -> 226; w20 P5/P4 -> -1 |
| ZWEITER RAUM ROOM10E0 (Zensus `abn1C/abn1_fresser_zensus.c` ueber alle STAGE1-RDTs: Fresser nur in 1140, 10E0, 10E1), RE2-KI: Leon (150,-800) rot 2964, Strecke durch die Luecke x -600..900 zwischen SCA-Zelle 9 und 8, naechster Fresser (-700,4100) in 4973 | 3, 7, 8, 12, 13, 20 | alle 6: mg faellt, KEIN Fresser verliert HP |
| dieselbe Aufstellung unter RE1.5-KI (Kontrolle: Fresser treffbar) | 3, 7, 8, 12, 13, 20 | alle 6 treffen: P5 79 -> 47 / -1 / -121 / 71 / -221 / -1, P4 82 -> 66 / -1 / -1 |
| echter Weg (Tuer): ROOM10D0 Tuerzone (6750,4350) Blick -z, Aktion 1,5 s -> `[room] PC loaded room10e0.rdt`, Leon (-8050,3200) | — | Raumzustand nach der echten Tuer identisch mit dem Sprung (P1 st1/ss1 7 g88 hp 79, P2..P5 st1/ss1 8 hp 50/80/50/80); vom Eintrittspunkt liegt Zelle 2 (Wand x -5500..-5000) vor den Fressern, deshalb wurde nach der echten Tuer nicht geschossen |
Nebenwirkung geprueft: der LIEGENDE (passiv, g 0x88) wird in keinem Lauf getroffen; geweckte Zombies bleiben treffbar.

### 4.2 Fix erklaert den Befund
Vorher (Roh-Logs `mess_r36c/vorher/{tisch,fresser}` des Bau-Agenten, von Abnahme 0 ausgewertet): Python ueber den Tisch
P4/P5 -> -1, vom Tuerpunkt P2/P5 -> -1. Ursache 1: der Port-Resolver hatte die Sichtlinie `jal 0x8001b9b4` @0x80012168 nicht
(selbst gelesen §1) — jetzt re15_damage.c nach `if (best < 0) return 0;`. Ursache 2: die v0.3.5-Ausnahme `spawn_pose` galt
fuer EXEC[8] — jetzt `&& e->sub_state_1 != 8` (enemy_ai_re2_zombie.c:9128), RE2 Gate 2 `lbu v0,467(s2)` / `bne` @0x80041270-78
(Abnahme 0 selbst gelesen). Beide Teile sind an der exe getrennt belegt (RE1.5-KI isoliert den Tisch, RE2-KI + Kontrollen
isoliert die Fresser-Sperre, zwei Raeume).

**Urteil Punkt 1: erfuellt.**

## 5. Nachbesserung 1 — Maengel M1-M7 der Abnahme 0, je selbst nachgeprueft
| Mangel | Pruefung dieser Abnahme | Stand |
|---|---|---|
| M1 toedlicher Treffer am Becken | Sonde §3.1: TOD-Spawn a3 (P8.x,P8.y+300,P8.z) d=1 (w3 F354), MAGNUM d<=1 (w7/w20), Tropfblut 6x d=0-1; exe F121 `w(-1862,-2176,-20446)` (vorher -1808) | behoben |
| M2 Aufsteh-Treffer / Nachbluten | Sonde: AUF a3 (0,300,0) an Part 8 d=24/6; NACH 5 Spawns (F90..F98, nur gerade +0x15A) a3 (0,-500,0) d=1; Disassembly §1 Zeile 2 | behoben |
| M3 Pin blind | `probe_r36_blut pin eben/tief/mg` nachgefahren (Zahlen = Dossier); Defekt-Abstand je Spawn 278..884 (MG ab 180) >> Schranke 40 | behoben (deckt nur Part 0/1/8 der Pistole/Ingram — Gruppe B ungepinnt, s. §3.3) |
| M4 Tisch-Trennschaerfe | integration_r36_schuss (LastTest.log: `[frei_re15]: Platz 4 HP 250 -> -1`, `Platz 5 HP 250 -> -1`, `[tisch_re15]` alle gleich) + eigene Laeufe §4.1 | behoben |
| M5 Zitate | re15_re2z_hit_filter_apply-Block, test_r36_schuss.c Kopf/52/53, test_re2_zombie_hitfilter.c 4a, re15_schuss_r36.h (@0x8001BFE8-FFC, @0x8001DB74-DC1C) berichtigt. Rest (nicht Spur C): re2z_init enemy_ai_re2_zombie.c:8553 zitiert fuer das 0x4004 des Port-Fressers weiter `@0x80100A88-8C` (= Deskriptor 7) — vom Bau-Agenten als Spur-F-Bestand benannt (Dossier §8.5) | behoben (Spur-C-Stellen) |
| M6 Regressionsaussage | `git log 9362dd7e..3ec5b1f8 -L :<f>:<datei>` je 0 Commits fuer re2z_blood_fx_scaled, re2z_gore_fx_ex, re2z_blood_fx_dir, re15_enemy_bone_world_pos, re15_esp_fx_spawn_ex; Gegenprobe master..HEAD liefert a752ff99/62cc4289/df7ecdc5 | bestaetigt: keine Regression aus v0.8.22/23 |
| M7 Messkopie | test_r36_schuss.cmake kopiert nach re15_pc_r36c_haken_<id><ext>; nach dem ctest-Lauf `ls build/platform/pc | grep -c r36c_haken` = 0 | behoben |

## 6. Gates

### 6.1 Tests (gezielt im Spurbaum, keine Voll-Suite)
`ctest --test-dir re15_port/build -R "<Regex des Bau-Agenten, 199 Tests: r36|crow|kraehe|hitfilter|zombie|re2z|re2_|1140|1030|1040|10d0|1100|1210|damage|werfer|dog_hit|spider|gator|birkin|g5_|shootable|hit_|autofire|waffen|blood|blut|gore|feeder|straight_shot|enemy_ai|combat|aim|r16_|rise_|knockdown|stagger|death|crawl|writher|r26_mg|r35_inhalt|r35_raeume|r35_cut1150|r35_zgirl|r30_hund|redhawk|r17_|zfall|situp|fx_region|granate>" --timeout 600 --output-on-failure -j 2`
(deckt alle beruehrten Module: enemy_ai_re2_zombie.c, enemy_ai_re2_crow.c, re15_damage.c-Schusspfad, enemy_ai_common.c-LOS,
Raeume 1140/1030/1040/10d0/1100/1210) -> **`100% tests passed, 0 tests failed out of 199`**, `Total Test time (real) = 879.13 sec`
(Log `abn1C/ctest_gezielt.log`), ohne Wiederholungslauf. Darin u.a. integration_r36_schuss 134.10 s, unit_r36_schuss,
unit_r36_blut_pin_eben/_tief/_mg, unit_r36_schuss_kraehe, unit_crow_death_corpse, unit_re2_zombie_teardeath, unit_r26_mg_blut,
unit_re2_zombie_hitfilter/_abc, unit_re2_crow_ai, integration_r35_werfer(_ton/_elza/_form), integration_r35_zgirl(_5090),
integration_r35_cut1150, integration_r35_granate, integration_r34_granaten, unit_gator_*, unit_*birkin*.

### 6.2 Vertrag (VERTRAG.md §1, §1.4, §1.6)
* Pfade (`git diff master --name-only`): engine/src enemy_ai_common.c, enemy_ai_re2_crow.c, enemy_ai_re2_zombie.c, re15_damage.c,
  schuss_r36.c (neu); include re15_actor.h, re15_schuss_r36.h (neu); tests/integration/test_r36_schuss.cmake,
  tests/unit/probes/r36_schuss.cmake, eigene Tests/Sonden + 7 angepasste Riegel; analysis/. Keine release/, platform/android/,
  shared_assets/PSX/, RELEASE_NOTES, tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt — ok.
* Gemeinsame Dateien nur kleine Haken: re15_damage.c +4 (Include + Sichtlinie), enemy_ai_common.c +4/-2 (static weg),
  re15_actor.h +5 (Feld) — ok. enemy_ai_re2_zombie.c (Modul F) traegt den Blut-Anker (VERTRAG §1.4 "C = ... + Blut-Anker").
  `git merge-tree --write-tree` gegen r36/zombieki (a0c36009), r36/granate, r36/gegnerzahl, r36/elza, r36/klettern, r36/kleidung,
  r36/szenenleon, r36/integration: alle konfliktfrei.
* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine (Diff-Suche re15_game_flag_set / re15_msg_install_text / scd_event_fire leer).
* Env-Schalter: einziges neues getenv = RE15_ASSET_DIR in der Sonde probe_r36_blut.c — kein Abschluss ueber Schalter.
* Rate-Woerter (deferred/tunable/interim/for now/faithful/plausib/TODO/approx) in den hinzugefuegten Zeilen: keine.
* Konstanten: jede neue verhaltensrelevante Zahl im Diff traegt @0x… (Stichproben §1); Port-Entscheidungen als PORT/
  "Port-Konvention" gekennzeichnet (Kraehen-Boden +0x1C2 "[PORT (1)]", Anker im laufenden Bild, Peilung).

## 7. Maengel (nummeriert, nachpruefbar)
* **M1 (Punkt 2) — Zensus-Gruppe B (a3 = 0) ankert an der ROHEN statt an der gezeichneten Pose.** re2z_gore_fx_ex /
  re2z_gore_fx / re2z_blood_fx_at -> re15_enemy_bone_world_pos (`g_anim_pose_actor = NULL`, re15_damage.c:3205-3210),
  waehrend die a3-Emitter ueber re15_schuss_r36_blut_anker die gezeichnete Pose nehmen (die PORT-KONVENTION des Dossiers).
  Gemessen mit `abn1C/abn1_blut.c` (Spalte "Abstand zum Anker ohne a3" = Spawn gegen gezeichneten Part im selben Bild):
  SPAS-Tod RIP-Kopf @0x80108E28 (a3 = 0 selbst gelesen) **442-443** in allen drei Anlaufrichtungen; RIP-P1-Nachbluten
  @0x80109108/@0x8010913C im Todesbild 109; Ingram Glied-Abriss @0x80105368/@0x801053D0 140/168; Arm-Abriss 4. Spawn
  @0x80107704 37 (der 1. Spawn desselben Parts im selben Bild, ueber gore_fx_a3 mit (0,0,0): 0). Das Dossier (OFFEN 11) nennt
  "Unterschied nur waehrend eines Crossfades (gemessen bis 98)" — jeder Treffer/Tod startet aber eine Blende, und die
  Abweichung erreicht 443. Nachpruefen: `abn1_blut.exe 13 px eben` (Zeile `F66 ... part 8 ... (ohne) ... 443`).
  (Der Weg ist im Diff schon da: re15_schuss_r36_blut_anker mit a3 = (0,0,0) liefert den gezeichneten Anker.T.)

## 8. Beobachtungen ohne Maengel-Status (fuer Orchestrator/Nutzer)
* B1 Anker-Zeitpunkt: das Original nimmt die Matrix des VORBILDS (T-1), der Port die gezeichnete Pose des laufenden Bilds
  (als PORT-KONVENTION dokumentiert, §8.2 Dossier). Das Dossier §2.1 beziffert den Unterschied mit "12-20"; gemessen sind es
  bei Erst-Treffern 16-64, beim Nachtreffer im Taumel aber 196/369 (w3 F78/F90) und beim SPAS-Tod 537 — dort springt die
  Port-Pose selbst (Becken T-1 x -1098 -> T x -1547 bei F90), weil die Taumel-Wurzelbewegung fehlt (Dossier OFFEN 7, Spur F).
* B2 Kraehe + Colt Python: `s_re2c_row_from_weapon[20] = 13` (enemy_ai_re2_crow.c:1479, Commit c2adafe3, vor Runde 36),
  bei Zombies `re2z_row_from_weapon[20] = 5` (Magnum) — der Kommentar "Werte == re2z_row_from_weapon" stimmt dort nicht.
  Gemessen: Python laesst die Kraehe NICHT zerplatzen (F19 st=3 ss1=20 -> Leiche), die Redhawk schon. Nicht Teil des Wortlauts.
* B3 re2z_init (enemy_ai_re2_zombie.c:8553) zitiert fuer das 0x4004 des Port-Fressers weiter `@0x80100A88-8C`
  (= Deskriptor 7); vom Bau-Agenten als Spur-F-Bestand benannt (Dossier §8.5).
* B4 Pins: probe_r36_blut bewertet nur Part 0/1/8 fuer Pistole/Ingram; Magnum-/SPAS-/Arm-Emitter sind nur ueber diese Abnahme
  gemessen (§3.1), Gruppe B gar nicht gepinnt.
* B5 Echter Weg: die Tuer ROOM10D0 -> ROOM10E0 liefert denselben Gegnerzustand wie der Sprung (§4.1); geschossen wurde nur
  nach Sprung + RE15_PLAYER_POS (der 1140-Sprung-Spawn ist der Original-Eintrittspunkt).
* B6 RE1.5-KI (Menue-Option): Fresser bleiben treffbar (Dossier OFFEN 1), Blut im RE1.5-Pfad an der Wurzel (OFFEN 2) —
  Vorgabe ist RE2-KI.

## 9. Ergebnis
| Punkt | Urteil |
|---|---|
| 1 Tisch + Fresser, alle Schusswaffen | **erfuellt** (exe: 15 Waffen x Tisch gerade/diagonal unter RE1.5-KI + Kontrollen; Fresser 15 Waffen x 2 Standorte + Weck-Kontrolle in ROOM1140, 6 Waffen + Kontrolle in ROOM10E0) |
| 2 Blut an der richtigen Stelle | **teilweise** (alle a3-Emitter byte-true und gemessen; Gruppe B a3 = 0 weiter 35-443 neben dem gezeichneten Part, M1) |
| 3 Redhawk/Kraehe wie RE2 | **erfuellt** (Antwort belegt; exe zeigt das RE2-Zerplatzen) |
RE-Gate: haelt (8 Stichproben selbst disassembliert, keine Rate-Woerter, Konstanten mit @0x…). Vertrag: haelt. Tests: 199/199.
**bestanden = false** (Punkt 2 teilweise, Mangel M1).
