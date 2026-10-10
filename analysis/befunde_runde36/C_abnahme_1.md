# Runde 36 — Spur C "schuss": Abnahme 1 (unabhaengig, 2026-10-10)

Baum `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, geprueft am Commit 89e58fc1 (Nachbesserung 1 nach
Abnahme 0, `C_abnahme_0.md` M1-M7). Nur gelesen/gemessen, kein Code im Baum geaendert. Eigene Sonden/Skripte/Logs
liegen im Scratchpad dieser Abnahme (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn1C_r36/`
(Kurzform `abn1C/...`).

(Bericht wird fortlaufend geschrieben — Stand: in Arbeit.)

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
(`git log 1e8cd231..HEAD -- re15_port/engine/src/schuss_r36.c` beruehrt nur Punkt 2).

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
| 8 Remington, +x / -x / +z Beine | F66 | 2 / 2 / 4 | 0 | 0-1 (Arm-Abriss (0,0,0)/(0,400,0)/(0,800,0) an Part 3 d=0-1) |
| 13 SPAS, +x / +z / -x / +x Beine | F66 | 13 (11) / 13 (11) / 13 (11) / 4 (1) | 0 | 0-1 (RIP (0,-500,0) 2x) |
| 12 Ingram, +z | F102 | 33 (2) | 0 | 58 (MG-Zufallsvektor; Zombie dreht/schiebt NACH dem Spawn @0x8010810C-6C, Schranke 100 im Pin) |
| 19 MC51, -x / +z | F102 | 19 (0) / 19 (0) | 0 | 40 / 43 |
Befund: JEDER Spawn mit a3 traegt exakt einen Original-Vektor (HURT s=+-1, TOD, FLINCH/Tropf, NACH/RIP, MAGNUM 1/2/3, AUF,
ZONE0, Arm, MG-Band), und die Lage ist Anker.R*a3 + Anker.T der im selben Bild gezeichneten Pose (d <= 32; MG <= 58).
Der Abstand zum alten Defekt (Anker ohne a3) betraegt je Spawn 283..884 — die Pins des Bau-Agenten (Schranke 40) sind
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
