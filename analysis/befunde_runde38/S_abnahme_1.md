# Runde 38 — Spur S "affen3": Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-17. Baum `.claude/worktrees/r38_affen3`, Zweig `r38/affen3`, HEAD 0b1bf561, Basis cb782bda.
Pruefer: unabhaengige Abnahme 1 (glaubt dem Bau-Agenten nichts, was nicht selbst gemessen ist).
Bericht wird fortlaufend geschrieben (Sitzungslimit). Scratch: `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\S38abn1\`.

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)
> - In Stage 3 die kleinen Affen können einen nicht treffen, es passiert nichts

Gegenstand dieser Abnahme zusaetzlich: Mangel 1 aus Abnahme 0 (Ereignis-Affen Stage 4/5 beissen aus y = -10000).

## 0. Stand und Bau
- `git log --oneline master..HEAD`: 34 Commits; seit Abnahme 0 (8bb74063): 11 Commits (eb5ffa7e .. 0b1bf561), Code nur in
  6bd30a91 (affen3_r38.c +65, enemy_ai_common.c +28/-11 netto, re15_affen3_r38.h +12), Riegel in 3bfe08ef/71695061/aad46e72/5c54afd7.
- Arbeitsbaum sauber (`git status --porcelain` leer).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`
  `=== LOCAL-BUILD-OK (build)`. re15_pc.exe 09:28:51 ist juenger als jede Engine-Quelle (affen3_r38.c 09:27:42,
  enemy_ai_common.c 09:28:21, re15_affen3_r38.h 09:27:55) -> die exe ist der Stand HEAD.

## 1. Code-Diff gelesen (seit Abnahme 0, `git diff 8bb74063..HEAD -- re15_port/engine re15_port/include`)
- affen3_r38.c `re15_affen3_init`: nach dem Grid-Block `grid == 0x40 -> state 4 / Sub 0`, `grid == 0x41 -> state 4 / Sub 1`
  (+0x6/+0x7 = 0); neue Funktion `re15_affen3_zustand4` (Unterzustand 0 = 0x8011577c, Unterzustand 1 = 0x801158e8, 2/3 leer).
- enemy_ai_common.c: `case 4: case 5: re15_affen3_zustand4(e)` in re15_cockroach_ai_tick; neue static
  `re15_affen3_wurzel_rumpf` (Zustand + aec4-Selbst-Schub + b498/b544 + Wandklemme) — aus run_all hinter dem Tor
  `!(grid & 0x20)` (vorher dieselben vier Aufrufe inline) und NEU aus `re15_enemy_spawn_root` fuer 0x29 (Typ-Liste
  um 0x29 erweitert, zwischen Sperrbit-Loeschen und -Zuruecksetzen).
- Header: Deklaration + Kopf-Kommentar. Keine weiteren Engine-/Plattform-Dateien beruehrt.
- Gesamt-Diff der Spur (`git diff master...HEAD --stat`): 14 Dateien; Code affen3_r38.c (neu), re15_affen3_r38.h (neu),
  enemy_ai_common.c +61/-?, affen_11c0.c, re15_damage.c (1 Zeile); Tests/Dossier/Abnahme 0/AUFTRAG-Nachtrag (Orchestrator).

## 2. RE-Gate — Stichproben selbst disassembliert (re15_disasm.py; STAGE3.BIN/STAGE4.BIN ohne Kopf, PSX.EXE)
Alle Stichproben betreffen die NEUEN Konstanten/Wege der Nachbesserung 1:

| # | Behauptung (Code-Kommentar) | selbst gelesen |
|---|---|---|
| 1 | INIT-Zweig @0x80110e80-eb8 (STAGE3) | `80110e80 lbu v1,9(a0)` / `80110e84 ori v0,zero,0x40` / `80110e88 bne v1,v0,0x80110eac` (Delay `ori v0,zero,0x41`) / `80110e90 ori v0,zero,0x4` / `80110e94 sw v0,4(a0)` / `80110ea4 lbu v1,9(a0)` / `80110ea8 ori v0,zero,0x41` / `80110eac bne v1,v0,0x80110ebc` / `80110eb4 ori v0,zero,0x104` / `80110eb8 sw v0,4(a0)` — Wort-Store: +0x4..7 = 04 00 00 00 bzw. 04 01 00 00 = Port state 4 / Sub 0 bzw. 1, +0x6/+0x7 = 0 |
| 1b | STAGE4 @0x8010c520-558 befehlsgleich | `8010c520 lbu v1,9(a0)` .. `8010c534 sw v0,4(a0)` .. `8010c554 ori v0,zero,0x104` / `8010c558 sw v0,4(a0)` — identisch |
| 2 | Zustandstabelle @0x8011eca4 [4] = [5] = 0x80115734, Unter-Tabelle @0x8011ee34 | `table 0x8011eca4`: [0] 0x80110c98 [1] 0x8011101c [2] 0x80114790 [3] 0x80114fb4 **[4] 0x80115734 [5] 0x80115734** [6] 0x80116228 [7] 0x80115a6c; 0x80115734: `80115744 lbu v0,5(v0)` / `80115754 addiu at,at,-4556` (= 0x8011ee34) / `80115764 jalr v0`; `table 0x8011ee34` = {0x8011577c, 0x801158e8, 0x80115a5c, 0x80115a64}; `80115a5c jr ra` / `80115a64 jr ra` |
| 3 | Unterzustand 1 0x801158e8 (alle Stores) | `80115900 ori v0,v0,0x1`/`80115904 sb v0,147` ; `8011591c ori v0,v0,0x2`/`80115920 sb` ; `8011592c ori a0,zero,0x1`/`80115930 sb a0,480` ; `80115940 lbu v0,484`/`80115950 addiu at,at,-5048` (0x8011ec48)/`80115960 sw v0,120` ; `80115978 andi v0,v0,0xbf`/`8011597c sb v0,9` ; `8011598c sb a0,4` (1) ; `80115998 ori v0,zero,0xa`/`8011599c sb v0,5` ; `801159a8 ori v0,zero,0x2`/`801159ac sb v0,6` ; `801159bc sb zero,7` ; `801159c8 ori v0,zero,0xd`/`801159cc sb v0,148` ; `801159d8 ori v0,zero,0x13`/`801159dc sb v0,149` ; `801159ec sb zero,143` ; `801159fc sh zero,140` ; `80115a0c sh zero,156` ; `80115a24 jal 0x8001f314` (a3 0x200) ; `80115a34 ori v0,zero,0x14`/`80115a38 sb v0,149` ; `80115a48 sb zero,143` — Port Zeile fuer Zeile gleich |
| 4 | Unterzustand 0 0x8011577c | Verteiler `8011578c lbu v1,6(a0)` (==1 -> 0x8011581c, ==0 -> 0x801157c8 mit v0 = 1, ==2 -> 0x80115858, sonst Ende); P0 `801157c8 sb v0,6` / `801157d4 ori v0,zero,0x1f`/`801157d8 sb v0,148` / `801157e8 sb zero,149` / `801157f4 ori v0,zero,0x7`/`801157f8 sb v0,143` / `80115808 sh zero,140` / `80115818 sh zero,156`, faellt in P1 `80115830 jal 0x8001f314` / `8011584c addu v1,v1,v0` / `80115854 sb v1,6`; P2 `80115860 andi v0,v0,0xbf` / `80115874 sb a0,4` (1) / `80115880 ori v0,zero,0x2`/`80115884 sb v0,5` / `80115894 sb a0,6` (1) / `801158a4 sb zero,7` / `801158b0 ori v0,zero,0x1d`/`801158b4 sb v0,149` / `801158c4 sb zero,143` / `801158d0 ori v0,zero,0x3`/`801158d4 sb v0,148` — Port gleich |
| 5 | Spawn-Wurzelaufruf PSX.EXE Sce_em_set | `80042564 lbu v1,9(s0)` / `8004256c andi v0,v1,0xdf` / `80042570 sb v0,9(s0)` / `80042578 sw s0,-14460(at)` (cur) / `8004257c lbu v0,8(s0)` .. `8004258c addiu at,at,11180` (0x80072bac) / `8004259c jalr v0` / Delay `800425a0 andi s4,v1,0x20` / `80042604 or v0,v0,s4` / `80042608 sb v0,9(s0)` |
| 5b | (selbst gefunden, nicht im Diff zitiert) zweiter Wurzelaufruf @0x800425a4-f4 | `800425a4 lh v0,16(s2)` (s2 = pc+2 `800420f4 addiu s2,a1,2` -> pc[18..19]); != 0 -> +0x4 = 4, +0x5 = (pc[19])-1, +0x94 = pc[18], `800425f4 jalr v0`. Zensus aller 32 `Sce_em_set` Typ 0x29 (shared_assets, scd_dump_room.py): pc[18..19] = `00 00` ueberall -> der Zweig wird fuer Affen nie genommen; das Fehlen im Port (scd_vm.c Kommentar "DEFERRED") ist fuer 0x29 folgenlos |
| 6 | Fall B[10] Phase 2 c1a4(+0x8c,0,-70,+0x1ba) @0x8011322c | `80113200 lbu v1,149(a0)` / `80113204 ori v0,zero,0x18` / `80113208 bne` / `80113210 ori v0,zero,0x17` / `80113214 sb v0,149` (Bild 0x17 halten) / `8011320c addu a1,zero,zero` / `80113224 lh a0,140` / `80113228 lh a3,442` / `8011322c jal 0x8001c1a4` / `80113230 addiu a2,zero,-70` / Landung `80113248 sb v1,6` (3) / `80113258 sh zero,140` |
| 7 | Luftkasten *(0x8011ec48 + 4*(+0x1e4)) | `table 0x8011ec44`: [1] = 0x8011ec20, [5] = 0x8011ec38; `read 0x8011ec20` / `0x8011ec38` = {0,-1530,0,800,1530,800} beide -> RE15_AFFEN3_KASTEN_LUFT_R 800 / _H 1530 richtig fuer +0x1e4 = 0 und 4 |
| 8 | ROOM4040 Freigabe (Auslieferungs-RDT shared_assets/PSX) | `0x011CE Pos_set 32 00 fe 9d f0 d8 0e 24` = (-25090,-10000,9230); `0x011D6 Member_set 34 0c 41 00` = +0x9 := 0x41; Spawn `0x00EB0 Sce_em_set 44 00 29 61 00 00 00 70 28 c9 00 00 ba 4a ..` |

Ergebnis: 8 Stichproben (+1 selbst gefundener Nebenweg), alle Adressen tragen genau die zitierten Befehle, richtige
Binaerdateien (STAGE3.BIN/STAGE4.BIN fuer 0x8010/0x8011xxxx, PSX.EXE fuer 0x8004xxxx). Sprungziele/Tabellen selbst
aufgeloest (0x80115734 -> 0x8011ee34 -> 0x8011577c/0x801158e8; 0x80072bac[typ]).
Diff-Suche (`git diff master...HEAD -- re15_port | grep -i -E "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|Platzhalter"`):
Ergebnis s. Abschnitt 6.

### 2b. Original-Spur o11 selbst dekodiert (nicht dem Dossier geglaubt)
Rohdatei des Bau-Agenten `scratchpad\s_nb1\gdb\o11_4040.txt` (450 Zeilen, je Zeile VSync + aca58..5b + Spieler/Entity 0/1
je 0x1f4 Byte) mit EIGENEM Dekoder (`S38abn1\tools\o11_eigen.py`, Felder +0x4..7, +0x9, +0x34/38/3c, +0x93, +0x94/95,
+0x1e0, Spieler +0x9a):
`F0 st0.0.0.0 g41` (Halt im Spawn-Aufruf, Sperrbit geloescht) -> `F18 st4.1.0.0 g61` gesperrt an (-14040,0,19130) ->
`F57 st4.1.0.0 g41 @(-25090,-10000,9230)` (Freigabe) -> `F58 st1.10.2.0 g01 mo13 af20 hr03 air1` -> `F59 hr01` ->
y F58..F76 = -10000 -10000 -9930 -9790 -9580 -9300 -8950 -8530 -8040 -7480 -6850 -6150 -5380 -4540 -3630 -2650 -1600 -480 0
-> `F77 st1.10.4.0 af25 air0` -> `F92 st1.4.0.0` -> HP: `F128 100->90 (st1.6.1.0 mo19 af23)`, `F225/F283/F340/F397/F454 je -5
(st1.5.1.0 mo18)`. Deckt sich mit den Dossier-Angaben. Code-Identitaet laut Bau-Agent nur fuer den Overlay-Code (MZD-Disc);
Sce_em_set (PSX.EXE) habe ich statisch am Auslieferungsstand gelesen (Stichprobe 5) — passt zu F0/F18 der Spur.

## 3. Nutzer-Punkt an der exe gemessen (Stage 3)
Messkopie `re15_port/build/platform/pc/re15_pc_s_abn1_mess.exe` (byte-gleich zu re15_pc.exe, `cmp`). Laeufe ueber
`S38abn1\lauf.sh` (eigener Ordner je Lauf, `aufruf.txt` mit Umgebung), Auswertung mit eigenen Skripten
`S38abn1\tools\hits.py` (HP-Abzuege im letzten Raumsegment + Affen-Zustand + Leon-Folgeclip + Affen-y aus gy.log) und
`tools\diffseg.py` (Bild-fuer-Bild-Vergleich zweier state.log).

### M1 — echter Weg wie Abnahme 0 M1 (Debug-Sprung 3050, Tuer 3050 -> 3040, Treppe, Kistenschalter, Plattform, Affengang)
Identische Umgebung/Eingabe wie Abnahme 0 (b13_weg), zusaetzlich `RE15_GEGNER_Y_LOG=gy.log`:
```
RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=3050@250 RE15_SET_FLAG=3:0xb0 RE15_STATE_LOG=state.log RE15_GEGNER_Y_LOG=gy.log RE15_INPUT_SCRIPT_START=250
RE15_EXIT_AT=2400#3040 RE15_INPUT_SCRIPT=W1,L0.3667,W0.3,A0.1,W1,R0.3333,U3.6,L0.3333,U1.5,W0.3,A0.1,W1.5,U0.17,L1.0667,U1,
  W0.3,A0.1,W1.5,A0.1,W1.5,A0.1,W8,R1.0667,U1.3,R1.0667,U1,L1.0667,U1.2,R1.0667,U5.6,L1.0667,U4.27,L1.0667,W2,U8,W20
```
Lauf m1_weg_std (RE2-KI, 97 s, rv 0) — **9 HP-Abzuege, jeder -5**, HP 100 -> 55:
```
F1358 d=5 hp=95 Leon(-8843,-4093 mo8 -> 8) | P1 st1/5/1/0 mo18 af14 @(-8127,-6082) y -1800
F1414 d=5 hp=90 Leon(-9296,-4838 mo8 -> 8) | P1 st1/5/1/0 mo18 af12 @(-8227,-6082)
F1470 d=5 hp=85 ... F1526 hp=80 ... F1585 hp=75 ... F1642 hp=70 ... F1701 hp=65 ... F1760 hp=60
F1819 d=5 hp=55 Leon(-9360,-3230 mo8 -> 8) | P1 st1/5/1/0 mo18 af15 @(-8083,-6082) y -1800
Abstaende 56,56,56,59,57,59,59,59
```
Jeder Treffer aus Sub 5 (Biss), Clip 18, Clip-Bild 12..15 (Fenster @0x8011ed48), Affe auf y = -1800 (= +0x1ba, Plattform-Band 1),
Leon-Folgeclip 8 (Flinch). Lauf m1b_weg_re15 (`RE15_AI_FLAVOR=re15`): bildgleich (9 Treffer, dieselben Bilder/Lagen).

**Abweichung gegen Abnahme 0 (gleicher Weg, 19 Treffer bis F2369) — selbst untersucht:** `diffseg.py` b13_weg (Stand 4ff3b973)
gegen m1_weg_std: erster Unterschied schon in **F1** — alt `st=0 @(-8800,-6150)` (INIT erst im ersten run_all-Bild F6), neu
`st=1 @(-8800,-6082)` (INIT + Wandklemme im Spawn-Aufruf, Original o1: "Spawn (-8800,-6150) -> nach dem ersten Klemmen
(-8800,-6082)", o11 F0 = Halt im Spawn-Aufruf). Der Affe laeuft damit ein KI-Bild frueher (B[0] ab F6 statt F7) und zieht
seine Zufallszahlen in anderer Reihenfolge; die Lagen unterscheiden sich bis F1500 nur um 1..20 Einheiten (F1358 Affe -8127
statt -8128, F1470 Leon z -5411 statt -5418), ab dem 4. Biss (F1526) entscheidet das, ob Leon nach dem Rueckstoss an der
Ecke vorbei nach Sueden (alt: z -6732, Affe beisst weiter) oder nach Norden geschoben wird (neu: Leon steht ab F1830 bei
(-9360,-2730), Abstand ~3415 > 0xbb8; der Affe wartet bis F2399 sehend in Sub 3 / Clip 5 an seiner Nische (-8698..-8722,-6082)
— genau die Lage des Originals o2 "(-8707..-8725, -6082)"). Ursache = die originalgetreuere Spawn-Reihenfolge der
Nachbesserung, kein Fehler im Treffer-Weg; Ergebnis bleibt: der Affe trifft (-5) und Leon reagiert.

### M2 — Elza (ROOM3041), Plattform-Weg wie Abnahme 0 M2
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=3040@250 RE15_SET_FLAG=3:0xb0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=260
RE15_STATE_LOG=state.log RE15_GEGNER_Y_LOG=gy.log RE15_EXIT_AT=2700#3041 RE15_INPUT_SCRIPT=W0.5,L1.0667,U5.25,R1.0667,U1.5,W0.3,
A0.1,W1.5,U0.17,L1.0667,U1,W0.3,A0.1,W1.5,A0.1,W1.5,A0.1,W8,R1.0667,U1.3,R1.0667,U1,L1.0667,U1.2,R1.0667,U5.6,L1.0667,U4.27,L1.0667,W2,U8,W20`
Lauf m2_elza_weg (115 s, rv 0): debug.log `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[room] PC loaded room3041.rdt`.
**3 HP-Abzuege je -5**: `F1695 hp 95 (Elza mo8)`, `F1853 hp 90 (mo9)`, `F1912 hp 85 (mo9)`, Affe P1 jeweils Sub 5 / Clip 18 / Bild 15,
y -1800. Abnahme 0 hatte F1695/F1852/F1911 (+1 Bild bei 2 und 3 = Spawn-Reihenfolge). Danach wandert Affe A in Sub 3 sehend
im schmalen Westgang (x -11630 F1950 -> -12377 F2699), Elza steht bei (-8299,-6732) HP 85 — dasselbe Bild wie Abnahme 0 M2
und wie die Basis-exe dort (Hinweis 1 unten, vorbestehend).

### M3 — Gegenprobe VORHER: Basis-exe cb782bda, derselbe echte Weg wie M1
Basis-Bau aus Abnahme 0 (`scratchpad\vorher_src\build_vorher\platform\pc\re15_pc.exe`); Quellen selbst gegen cb782bda
geprueft (`git show cb782bda:<datei>` vs. vorher_src ohne CR: enemy_ai_common.c, affen_11c0.c, re15_damage.c, scd_vm.c gleich;
affen3_r38.c fehlt dort). Umgebung wie M1 + `RE15_CD_ROOT=<baum>/re15_port/shared_assets/PSX` (debug.log `cd-root: ...r38_affen3/
re15_port/shared_assets/PSX`). Lauf m3_weg_vorher (97 s, rv 0, `[room] PC loaded room3040.rdt`):
**0 HP-Abzuege**, aber 125 Bilder mit Affe P1 in Sub 5 (Biss-Clip) — der Nutzer-Befund "es passiert nichts". Gleicher Weg mit
der Spur-exe (M1): 9 x -5 mit Flinch.

## 4. Mangel 1 aus Abnahme 0 an der exe nachgemessen (Ereignis-Affen Stage 4/5)
Auswertung mit eigenem Skript `S38abn1\tools\ereignis.py` (gy.log je Bild: Zustand, +0x9, Lage, +0x1ba; HP aus state.log mit
Affen-Lage im selben Bild). Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_STATE_LOG=state.log RE15_GEGNER_Y_LOG=gy.log`.

### M4 — Aufbau der Abnahme-0-Messung M5 (der Mangel-Aufbau), beide KI
`RE15_DEBUG_JUMP=4040@250 RE15_PLAYER_POS=-25000,4000,1024,0 RE15_EXIT_AT=900#4040` (Lauf s1_4040_std; s2_4040_re15 mit
`RE15_AI_FLAVOR=re15`, beide 39 s, rv 0):
```
F1..F15  Platz 1 st 4/1/0/0 g61 @(-14040,0,19130)        <- gesperrt in Zustand 4/1 (o11 F18..F56: 4.1.0.0 g61)
F16 (R)  st 1/10/2/0 g01 mo13 af20 @(-25090,-10000,9230) <- Freigabe-Bild = o11 F58
y R..R+20: -10000 -10000 -9930 -9790 -9580 -9300 -8950 -8530 -8040 -7480 -6850 -6150 -5380 -4540 -3630 -2650 -1600 -480 0 0 0
Landung (Sub 10 Phase 3) R+18 (o11 F76), Sub 4 ab R+34 (o11 F92)
HP F87 (R+71) 100->90 | Affe st1/6/1 mo19 af23 @(-24144,0,5296) f1ba=0     (o11: R+70, -10, Sub 6 Clip 19 Bild 23)
HP F193 (R+177) 90->85 | Affe st1/5/1 mo18 af14 y 0  ... 13 Bisse je -5 bis F885 (30->25), jeder mit Affe auf y = 0
y nach R+18: min 0 max 0
```
re15-KI: bildgleich. Vorher (Abnahme 0 M5, Stand 4ff3b973): Affe bis F900 auf y = -10000, Bisse F324/F380 aus der Luft.
**Mangel 1 im Mangel-Aufbau behoben.**

### M5 — ECHTER Weg durch die Tuer: ROOM4050 Tuer slot 4 -> ROOM4040, in den Ausloeser gehen
Tuer selbst aus dem RDT gelesen: ROOM4050 `0x01BD6 Door_aot_set 3b 04 .. 36 de 5c 95 d0 07 e8 03 98 a4 00 00 26 1b 00 08 03 04 01`
= Rechteck x -8650..-6650 / z -27300..-26300, Ziel Stage-Index 3 / Raum 4 (ROOM4040) bei (-23400,0,6950), Gier 0x800.
`RE15_DEBUG_JUMP=4050@250 RE15_PLAYER_POS=-7650,-26600,1024,0 RE15_INPUT_SCRIPT_START=250
RE15_INPUT_SCRIPT=W1,A0.1,W8,R1.0667,U1.2,W30 RE15_EXIT_AT=1500#4040` (Lauf s3_tuer_4050, 65 s, rv 0); debug.log:
`[room] PC loaded room4050.rdt` -> `[tuer] Sequenz Archiv 2 DOOR29 ... Sequenz fertig: 286 Bilder` -> `[room] PC loaded room4040.rdt`.
```
F1..F292 Platz 1 st 4/1/0/0 g61 @(-14040,0,19130)   (Leon geht nach der Tuer nach Sueden in AOT 5)
F293 (R) st 1/10/2/0 g01 mo13 af20 @(-25090,-10000,9230)
y R..R+20 = dieselbe Bahn wie o11; Landung R+18, Sub 4 R+34
HP F360 (R+67) 100->90 Leon(-23400,4250) | Affe st1/6/1 mo19 af23 @(-23991,0,6813) y 0
HP F464 (R+171) 90->85 ... 20 Bisse je -5 bis F1488 (0 -> -5), jeder mit Affe auf y = 0; y nach R+18: min 0 max 0
```

### M6 — Stage 5: ROOM50A0 (zweiter Raum mit Deckenfall-Freigabe)
`RE15_DEBUG_JUMP=50A0@250 RE15_PLAYER_POS=-12350,150,1024,0 RE15_EXIT_AT=900#50A0` (Lauf s4_50a0_std): Platz 1 F1..F15
`4/1/0/0 g61 @(-28038,0,-588)` (= Sce_em_set @0x01670 `7a 92 .. b4 fd`), F16 Freigabe `1/10/2/0 g01 mo13 af20 @(-12490,-10000,3080)`
(= sub06 Pos_set @0x01982 `32 00 36 cf f0 d8 08 0c` = (-12490,-10000,3080) + Member_set(12,0x41) @0x0198A), Bahn wie o11, Landung R+18, Sub 4 R+34; HP F83 (R+67) -10 aus Sub 6,
danach 13 x -5 aus Sub 5, alle mit Affe auf y = 0. Platz 2 (Ausloeser AOT 1 nicht betreten) bleibt alle 900 Bilder
`4/1/0/0 g61` an seiner Parkstelle.

### M7 — Bildkontrolle (Hardware-Renderer, OHNE RE15_SOFTWARE_RENDER, `RE15_WINDOW_SCALE=1`)
Weg M5, `RE15_FRAMEDUMP=270-420/3:fd_ RE15_EXIT_AT=420#4040` (Lauf b1_tuer_bild; Freigabe wieder F293, Landung F311,
erster Treffer F360 = M5). Kontaktbogen `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\S38abn1\fall_blatt.png`,
Ausschnitt `...\S38abn1\fall_zoom.png`, Angriff `...\S38abn1\angriff_blatt.png`: F294-F303 die zwei Deckengitter (obj 1/2,
Evt_exec 5/6) fallen, F303 der Affe erscheint an der Decke, F306/F309 faellt er gestreckt (Clip 13 Bild 23 gehalten), F312 steht
er am Boden, F315-F330 kauert er bei den Gittern; F333-F357 umrundet er Leon, F360 schwerer Angriff, F363-F372 Leon am Boden
(Niederschlag, aca59 = a780+4). Modelle geschlossen, kein Schweben ueber dem Boden, keine zerrissenen Meshes.

## 5. Zensus/Randfaelle selbst nachgeprueft
- **Alle 32 `Sce_em_set` Typ 0x29** (shared_assets/PSX, scd_dump_room.py): grid nur `00` (3040/3041, 4000, 4040/4041 Else-Zweig,
  4080/4081, 5000/5001, 50A0/50A1) oder `61` (4000 @0x01318, 4040/4041 @0x00EB0, 4080/4081 @0x00B84, 5031 @0x010CA, 50A0/50A1
  @0x01670/@0x01742, 50B0 @0x00526/@0x0053A, 5100/5101 @0x007B2/@0x007C6); pc[18..19] ueberall `00 00` (s. Stichprobe 5b).
- **Alle `Member_set` mit member 0x0C** in allen RDTs: in Raeumen mit Affen nur die Werte `41`, `61`, `00` (4000, 4040/4041, 4080/4081,
  5031, 50A0/50A1, 50B0, 5100/5101); `Member_set(12,0x40)` gibt es nirgends. Damit erreicht kein ausgeliefertes Skript den
  Unterzustand 0 (0x8011577c) — die Aussage des Dossiers stimmt; der Unterzustand ist trotzdem befehlsgleich portiert
  (Stichprobe 4) und vom Riegel `unit_r38_affen3_ereignis40` gemessen.
- **OFFEN 4 (Bank beim Spawn) selbst eingeordnet:** `re15_affen_pool_anim` (affen_11c0.c Z. 209) setzt ohne Bank `J->ok = 0`;
  das erste anim_set mit Bank mischt dann nicht (`misch = (frac != 0 && J->ok)`, Z. 215), sondern setzt den Keyframe direkt.
  Fuss-Sperre (`!P->jetzt.ok || !P->zeichen.ok -> return 1`, Z. 254) und Trefferpunkt brauchen den Pool erst in B[3]/B[5]/B[6];
  B[0]/B[1] (Ruhe/Schlaf, >= 59 Bilder Zeitgeber @0x80111330-44) und Zustand 4/1 (+0x8f = 0 vor anim_set @0x801159ec) rufen
  keine Fuss-Sperre. Die Abweichung wirkt also nur im o3-Randfall (Spieler schon im ersten aktiven Bild im Bissbereich), wie
  das Dossier sagt.

## 6. Gates
### RE-Gate
- Neue Konstanten der Nachbesserung tragen alle eine Adresse (0x40/0x41/0x4/0x104, 0x1f, 7, 0x1d, 3, 0xa, 2, 0xd, 0x13, 0x14, 0xbf,
  Tabellen @0x8011eca4/@0x8011ee34/@0x8011ec48); 8 Stichproben + 1 Nebenweg selbst disassembliert, alle stimmen (Abschnitt 2).
- Diff-Suche `deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|Platzhalter|ungefaehr|geschaetzt|approx`
  ueber `git diff master...HEAD -- re15_port`: **0 Treffer**. Neue `getenv` in engine/include/platform: **0** (nur der
  Test-Messhaken `R38_AFFEN3_SPUR` in tests/unit/test_r38_affen3.c).
- Erklaert der Fix den Befund? Ja: Abnahme 0 M5 (Affe bleibt auf y = -10000, Zustand 1 Sub 1, beisst aus der Luft) folgt genau
  aus den drei fehlenden Stuecken (Spawn-Aufruf fehlte fuer 0x29 -> INIT erst nach Member_set; INIT-Zweig fehlte -> Schlaf;
  Zustand 4 fehlte -> kein B[10]-Fall). Nachher (M4/M5/M6): Fall in 18 Bildern wie o11, alle Treffer vom Boden.
- Offene Punkte des Dossiers: OFFEN 1 (Zufallsquelle FUN_8001af20, Port re15_engine_rand8) und OFFEN 2 (Spieler-Rueckstoss/
  Gehtempo) sind vorbestehende Engine-/Spieler-Abweichungen ausserhalb der Spur; OFFEN 3 (Anzeige-Bits aca3c) ohne
  Port-Gegenstueck wie beim Gorilla; OFFEN 4 eingeordnet (Abschnitt 5). Keiner davon ist ein fehlender Affen-Mechanismus.

### Vertrag (VERTRAG.md)
- Ressourcen: S hat keine Bank-9-Bits/Nachrichten-Ids/Items — Diff hat keine (Abnahme 0, seit dem keine neuen).
- Pfade (`git diff master...HEAD --name-only`, 14 Dateien): affen3_r38.c, re15_affen3_r38.h (eigen), enemy_ai_common.c,
  affen_11c0.c, re15_damage.c (gemeinsam), tests/unit/test_r38_affen3.c, tests/unit/probes/r38_affen3.cmake (Sonden-Datei,
  per GLOB @tests/unit/CMakeLists.txt:4060 eingebunden — keine Test-CMakeLists geaendert), tests/integration/test_r38_affen3*.cmake,
  tests/unit/test_cockroach_ai.c (Alt-Riegel, begruendet), Dossier/Abnahme 0/AUFTRAG-Nachtrag (Orchestrator). Keine
  release/, platform/android/, shared_assets/PSX/.
- Gemeinsame Dateien: enemy_ai_common.c gesamt +45/-16 in 14 Hunks (`--numstat`), 21 der 45 neuen Zeilen tragen
  "Runde 38 Spur S", der Rest sind Folgezeilen derselben Bloecke. Nachbesserung 1 dort: Typ-Liste + Aufruf im Spawn (2 Zeilen),
  case 4/5 (3 Zeilen), `re15_affen3_wurzel_rumpf` (8 Zeilen, ersetzt dieselben 6 Inline-Zeilen in run_all — muss in der Datei
  stehen, weil re15_cockroach_ai_tick/selfpush/body_push_tail dort static sind). Logik des Zustands 4 steht in affen3_r38.c.
  VERTRAG §1.4 nennt enemy_ai_common.c fuer B/T, nicht fuer S — wie in Abnahme 0 als Hinweis, kein Mangel (die Affen-Wurzel
  liegt dort).
- Merge: s. Abschnitt 8.

(Abschnitte folgen fortlaufend.)
