# Runde 38 — Spur S "affen3": Abnahme 0 (unabhaengig)

Datum 2026-10-17. Baum `.claude/worktrees/r38_affen3`, Zweig `r38/affen3`, HEAD 4ff3b973, Basis cb782bda.
Pruefer: unabhaengige Abnahme 0 (glaubt dem Bau-Agenten nichts, was nicht selbst gemessen ist).
Bericht wird fortlaufend geschrieben (Sitzungslimit).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)
> - In Stage 3 die kleinen Affen können einen nicht treffen, es passiert nichts

## 0. Stand und Bau
- `git log --oneline master..HEAD`: 23 Commits (49a4b741 .. 4ff3b973), davon 0686d425 Orchestrator-Nachtrag AUFTRAG.md.
- Zweig-eigener Diff (`git diff master...HEAD --stat`, Merge-Basis cb782bda): 13 Dateien, +2827/-18.
  (`git diff master --stat` zeigt zusaetzlich 9 Orchestrator-Dateien als "geloescht/geaendert" — das sind
  master-Commits NACH der Basis, nicht Aenderungen dieser Spur.)
- Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`, `ninja: no work to do`
  (exe 07:42:31 juenger als die letzte Quell-Aenderung 07:36:54; Arbeitsbaum sauber) -> die exe ist der Stand HEAD.


## 1. RE-Gate — Stichproben selbst disassembliert (re15_disasm.py, STAGE3.BIN ohne Kopf bzw. PSX.EXE)
Alle zitierten Stellen enthalten, was der Code behauptet:

| Behauptung (Code/Kopf) | selbst gelesen |
|---|---|
| Biss-Tor Spieler+0x93 @0x80111eac | `80111ea4 lui a3,0x800b` / `80111ea8 addiu a3,a3,-13593` (= 0x800acae7 = 0x800aca54+0x93) / `80111eac lbu v0,0(a3)` / `80111eb4 bne v0,zero,0x8011201c` |
| Quadrat FUN_8001bff8(Pool+1612 = Record 9, a1, r 0x3e8, &Spieler+0x34) @0x80111eb8-c8 | `80111eb8 addiu a0,s2,1612` / `80111ec0 ori a2,zero,0x3e8` / `80111ec4 jal 0x8001bff8` / `80111ec8 addiu a3,a3,-95` (0x800acae7-0x5f = 0x800aca88 = Spieler+0x34) |
| Fenster-Tabelle @0x8011ed48 | `80111efc lui at,0x8012` / `80111f00 addiu at,at,-4792` (= 0x8011ed48) |
| Biss -5 @0x80111f60, Sperre 0x2d, SE 5 | `80111f4c ori a0,zero,0x5` / `80111f60 addiu v0,v0,-5` / `80111f64 sh v0,0(s0)` (s0 = player.hp +0x9a) / `80111f68 ori v0,zero,0x2d` / `80111f70 sh v0,476(v1)` (+0x1dc) / `80111f6c jal 0x800453d0` |
| aca58 = 2, aca59 = a780+2, aca5a = 0, HP<0 -> 3/0, +0x93 \|= 1 | `80111f7c sb v0,-13736(at)` (v0 = 2) / `80111f80 jal 0x8001a780` / `80111f8c addiu v0,v0,2` / `80111f94 sb v0,-13735(at)` / `80111f9c sb zero,-13734(at)` / `80111fa0 bgez v1,...` / `80111fc4 ori v0,v0,0x1` / `80111fcc sb v0,-13593(at)` |
| Todessprung nur bei Spieler-HP < 50 @0x8011269c | `80112694 lh v0,0(a2)` (player.hp) / `8011269c slti v0,v0,50` / `801126a0 beq v0,zero,...`; danach Tor `801126ac lbu v0,-13593(v0)` / `801126b4 bne`; a804(0x1388,0xc0) `801126b8/bc`, +0x1dc == 0 `801126dc-e4`, +0x5 = 8 `801126ec`, SE 6 `80112708-0c` |
| Todessprung -500 @0x80112b00, Sperre 0x12c, SE 0, aca58 = 6 (Wort) | `80112adc addu a0,zero,zero` / `80112b00 addiu v0,v0,-500` / `80112b08 ori v0,zero,0x12c` / `80112b10 sh v0,476(a1)` / `80112b1c ori v0,zero,0x6` / `80112b24 sw v0,-13736(at)` / acbfc/acbcc/acbd0 `80112b38/48/60` / `80112b64 jal 0x80037edc` + `80112b68 ori a1,zero,0x32` |
| Wurzel-Tor +0x9 & 0x20 @0x80110b34-40 | `80110b14 lw v0,0(s0)` (g_pauseflags) / `80110b18 lui v1,0x2000` / `80110b20 bne` ; `80110b34 lbu v0,9(v1)` / `80110b3c andi v0,v0,0x20` / `80110b40 bne v0,zero,0x80110bd0` |
| Selbst-Schub aec4(&Spieler, Affe) @0x80110b88, Wandklemme @0x80110ba0-cc | `80110b78 jal 0x8002b498`; `80110b84 lw a1,-14460(a1)` (cur) / `80110b88 jal 0x8002aec4` / `80110b8c addiu a0,s0,20` (0x800aca40+0x14 = 0x800aca54 = Spieler) / `80110b9c sh v0,466(v1)` (+0x1d2); `80110b98 jal 0x8002b544`; `80110bac lw v0,120(a0)` / `80110bb4 lhu a1,6(v0)` / `80110bb0 ori a2,zero,0x4` / `80110bb8 jal 0x8003b0a4` / `80110bcc sh v0,470(v1)` (+0x1d6) |
| Griff-Entscheid A[13] liest +0x93 @0x80113a24 | `80113a20 addiu a0,a0,-13593` / `80113a24 lbu v0,0(a0)` / `80113a2c bne v0,zero,...`; +0x1d4 < 0x7d0 `80113a40-48`; a780(Spieler) `80113a54 jal 0x8001a780` (`80113a58 addiu a0,a0,-147` = Spieler); +0x1dc `80113a70`; +0x5 = 0xf `80113a80-84` |
| Hieb -10 @0x801141f4 | `801141f4 addiu v0,v0,-10` / `801141f8 sh v0,0(s0)` (player.hp) / `80114204 ori v0,zero,0x2d` / `8011420c sh v0,476(v1)` |
| Griff-Ende +0x1dc = 0x3c @0x80114718 | `80114718 ori v0,zero,0x3c` / `8011471c sh v0,476(v1)` / `80114728 ori v0,zero,0x2` / `8011472c sb v0,5(v1)` |
| a780 = ((a0.Gier - cur.Gier + 0x400) & 0xfff) < 0x800 (PSX.EXE) | `8001a788 lh v0,106(a0)` / `8001a78c lh v1,106(v1)` / `8001a794 subu` / `8001a798 addiu v0,v0,1024` / `8001a79c andi v0,v0,0xfff` / `8001a7a4 slti v0,v0,2048` |
| Quadrat FUN_8001bff8 (PSX.EXE) | `8001c080 lhu v0,36(sp)` / `8001c084 lhu v1,0(s1)` / `8001c088 subu v0,v0,s0` / `8001c08c subu v0,v1,v0` / `8001c090 andi v0,v0,0xffff` / `8001c094 sll v1,s0,16` / `8001c098 sra a0,v1,15` (2r) / `8001c09c slt v0,a0,v0` / Z ebenso `8001c0a8-c0`, `8001c0c0 xori v0,v0,0x1` — Port `(u16)(pl - pt + r) <= (u16)(2r)` je Achse ist aequivalent |
| Typ 0x29 = Blob 13 (PSX.EXE FUN_80022300, Tabelle @0x80073000) | `80022324 lbu v0,8(s2)` / `80022338 addiu a0,v0,-16` / `80022344 sll v1,a0,3` / `80022370 addiu at,at,12088` (0x80072f38) / `80022378 lw v0,0(at)`; `read 0x80073000` = [994, 222424] = `e2 03 00 00 d8 64 03 00` |

Ergebnis: 13 Stichproben (statt der verlangten 3), alle Adressen tragen genau die zitierten Befehle; Sprungziele
(0x8001bff8, 0x8001a780, 0x8002aec4) selbst disassembliert, richtige Binaerdatei (STAGE3.BIN fuer 0x8011xxxx, PSX.EXE fuer
0x8001/0x8002/0x8007xxxx).

Nachtrag zu 1: FUN_8002aec4 selbst gelesen: `8002af14 and v0,a0,v1` / `8002af18 andi v0,v0,0x1000` / `8002af1c bne v0,zero,0x8002b464`
(Griff-Paar-Ausnahme, wie im Code zitiert). Weitere Daten selbst gelesen: Fenster @0x8011ed48 = `0c 0d 0e 0f`, @0x8011ed4c =
`15 16 17 18 21 22`, @0x8011ed54 = `08 09 0a`, @0x8011ed58 = `18 31 00`, @0x8011ed5c = `08..0d`, @0x8011ed64 = `2b..30`;
Kaesten @0x8011ec14 = {0,-1080,0,1100,1080,1100}, @0x8011ec20 = {0,-1530,0,800,1530,800}, @0x8011ec2c = {..,800,1080,800};
Zeigertabelle @0x8011ec44 [0]=ec14 [1]=ec20 [4]=ec2c [5]=ec38, Index +0x1e4 = (grid&8)?4:0 (`80110e64 andi v0,v0,0x8` /
`80110e6c ori v0,zero,0x4` / `80110e70 sb v0,484(v1)`; `80110f68 lbu v0,484(v1)` / `80110f70 sll v0,v0,2` / `80110f78 ..0x8011ec44`);
Sce_em_set-Saat `8004220c subu v0,zero,v0` / `80042210 sh v0,442(s0)`. Die fest eingetragene Bildzahl-Tabelle
`s_clip_len[32]` (affen3_r38.c:34, ohne @0x) ist mit der EDD von EM029 identisch (selbst geparst mit
tools/emd_ansichtsblatt.py `Modell(0x29).clips`: 32 Clips, 0 Abweichungen) — Datenbeleg, kein Ratewert.
Diff-Suche nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|Platzhalter|vorerst": 0 Treffer;
kein neuer getenv-Schalter in engine/include/platform.

## 2. Nutzer-Punkt an der exe gemessen

### M1 — echter Weg (Tuer 3050 -> 3040, Treppe, Kistenschalter, Plattform), Standard-KI (RE2)
Messkopie `re15_port/build/platform/pc/re15_pc_abn0S_mess.exe` (= re15_pc.exe des Baums). Weg (selbst erkundet, Kollision
von ROOM3040 aus dem RDT geplottet): Debug-Sprung nach ROOM3050 (Spawn (-23996,1523) = Ankunft der Tuer 3040->3050),
Leon dreht nach Norden, ACTION an der Tuer 3050 slot 0 -> ROOM3040 (-26350,-5100) (echter Tuerwechsel, `[room] PC loaded
room3040.rdt`), geht im Erdgeschoss nach Sueden, steigt Treppe slot 2 (sce 12) auf die Plattform (Band 1, Landung
(-21282,-13096)), drueckt den Kistenschalter (AOT 7, "There's a button here. Push it?" -> Ja; dafuer `RE15_SET_FLAG=3:0xb0`
= der Strom-Fortschritt aus einem frueheren Raum, sonst "The control panel is not responding"), geht durch den freien Gang
nach Norden, im Nordgang nach Osten und im Affengang nach Sueden auf Affe A zu (8 s), steht dann still.
```
RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=3050@250 RE15_SET_FLAG=3:0xb0 RE15_STATE_LOG=state.log RE15_INPUT_SCRIPT_START=250 RE15_EXIT_AT=2400#3040
RE15_INPUT_SCRIPT=W1,L0.3667,W0.3,A0.1,W1,R0.3333,U3.6,L0.3333,U1.5,W0.3,A0.1,W1.5,U0.17,L1.0667,U1,W0.3,A0.1,W1.5,A0.1,
  W1.5,A0.1,W8,R1.0667,U1.3,R1.0667,U1,L1.0667,U1.2,R1.0667,U5.6,L1.0667,U4.27,L1.0667,W2,U8,W20
```
Ergebnis (state.log, ausgewertet mit eigenem Skript treffer.py; Lauf b13_weg, 97 s, rv 0):
- **19 HP-Abzuege, jeder genau -5**, HP 100 -> 5 (F1358 .. F2369). Im Trefferbild steht Affe Platz 1 (Typ 0x29) jedes Mal
  in Sub 5 (Biss) mit Clip 18 (0x12), Clip-Bild 12..14 (Fenster {12..15} @0x8011ed48). Abstaende 56,56,56,57,57,57,56,...56.
- Leon reagiert: Folgebild Leon-Clip 8 (Treffer 1..4, Affe vor ihm), danach Clip 9 (von der Seite/hinten) — der Flinch.
- Auszug: `F1358 dHP=5 hp=95 Leon(-8843,-4093 rot-3072 mo8) | slot1 t29 st1 ss5/1/0 mo18 af14 @(-8128,-6082)`,
  `F1583 dHP=5 hp=75 Leon(-9756,-6732 mo9) | slot1 ss5 mo18 af13 @(-8418,-6082)`, `F2369 dHP=5 hp=5 Leon(-10137,-6732 mo9)`.
- Affe B (Platz 2) bleibt in seinem Gang (Sub 3 blind) — Leon kommt ihm auf diesem Weg nicht nahe.
=> Der Nutzer-Befund "koennen einen nicht treffen, es passiert nichts" ist auf dem Spielweg behoben (Treffer + Reaktion).

Nebenbefund des Weges (NICHT diese Spur, Port-Treppen-Code): ACTION mit Blick nach Sueden 1350 Einheiten oestlich der
Treppenkante (-21282,-13096, Sonde (-21321,-13895) ausserhalb des Treppen-Rechtecks x <= -21600) startete den Abstieg der
Treppe slot 3 statt den Kistenschalter (Lauf b9_knopf F284). Von (-20907,-13757) aus greift der Schalter (b10).

### M2 — Elza (ROOM3041), derselbe Plattform-Weg ab dem Sprung-Spawn, Standard-KI
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=3040@250 RE15_SET_FLAG=3:0xb0
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=260` (Spielerwahl -> `[pl] Spieler-Familie PL04 (Elza-Bit=1)`,
`[room] PC loaded room3041.rdt`), Weg: Sprung-Spawn (-25600,-1350) -> Sueden -> Treppe slot 2 -> Schalter -> Nordgang ->
Affengang (Skript `W0.5,L1.0667,U5.25,R1.0667,U1.5,W0.3,A0.1,W1.5,U0.17,L1.0667,U1,W0.3,A0.1,W1.5,A0.1,W1.5,A0.1,W8,R1.0667,
U1.3,R1.0667,U1,L1.0667,U1.2,R1.0667,U5.6,L1.0667,U4.27,L1.0667,W2,U8,W20`), Lauf e2_elza_weg (118 s, rv 0):
- **3 HP-Abzuege, je -5** (F1695 hp 95, F1852 hp 90, F1911 hp 85), Affe Platz 1 jeweils Sub 5 / Clip 18 / Bild 15,
  Elza-Folgeclip 8 bzw. 9 (Flinch).
- Danach haelt der Affe Abstand: er war vor Elzas Ankunft im Sub 3 (blind) aus der Nische nach Westen in den schmalen
  Gang zwischen den Zellen 27/31 gekrochen ((-8707) F1520 -> (-9171) F1560), wurde in den Bissen weiter nach Westen versetzt
  (-10612 F1911) und wartet ab F1935 in Sub 3 sehend (Clip 5) bei x -11561 .. -11930 (Abstand ~3400-3700 > 0xbb8), ohne
  erneut anzugreifen; Elza steht bis F2700 bei (-8226,-6732), HP 85. Ob das Original den Affen in diesem Fall ebenso
  abtreibt, ist nicht gegen das Original gemessen (Bau-Agent: o1..o10, keiner mit Spieler-Annaeherung ueber den Nordgang
  von Westen und Elza). Im Original o1/o2 blieb Affe A immer an der Nische (-8707..-8725); hier fuehrt der Port ihn bis
  x -11930 — siehe Maengel/Hinweise.

## 3. Riegel (selbst gefahren, gezielt, KEINE volle Suite)
- Unit (95 Tests, `-j3`): `ctest --test-dir re15_port/build -R "^unit_(cockroach_ai|maggot_|enemy_body_push|re2_victim_collapse|
  dog_grab_anchor|re2_dog_grab_anchor|re2z_|dog_aim_band|dog_devour|sca_wall_hit|knockdown_|re2_stagger_knockdown|r35_affen_|
  r35_cut1150_montage_11c0|r36_kleidung_|r36_zombieki_|r36_szenenleon_|r36_dachkiste_|r36_schuss|r36_elza_schuss|r36_ziel1190|
  r36_karte_tuer11c0|r37_szene11c0_|r37_bissarm_|r38_affen3_|npc_wall_clamp|r20_birkin_push_pin|1090_push_pin|
  r34n_f_leiche_latch|sherry_wall_pin)" --timeout 600` -> **100% tests passed, 0 tests failed out of 95** (alle 8
  unit_r38_affen3_*, Alt-Riegel unit_cockroach_ai, Gorilla-Familie unit_maggot_*/unit_r35_affen_*, alle geteilten Opfer-/
  Griff-/Schub-/Knockdown-Wege).
- exe-Riegel seriell (13 Tests): `-R "^integration_(r38_affen3|r38_affen3_decke|r38_affen3_tor|r37_szene11c0|
  r36_kleidung_hundeweg|r36_kleidung_spielweg|r37_elzaems_elza1181griff|r36_schuss|r36_elza_schuss|r36_ziel1190_scheiben|
  r36_dachkiste|r36_szenenleon|r37_bissarm_echtlauf)$" --timeout 900` -> **100% tests passed, 0 tests failed out of 13**.
  Spur-Riegel im Wortlaut:
  `r38_affen3 [std]: 15 Treffer in Bild 169;225;...;954; Abstaende 56 x13 + 57 x1; Clip-Bilder 14;12;...; Ost-Anschlag -8068,
  letzter Treffer x -8707` (re15 identisch); `r38_affen3_decke [std]: getroffen=1, Subs 0;3;9;12;13;15;2;3;5;...; Griff F350,
  Sub 2 F561; Leon-Clips im Griff 1;2;3;16;11; HP-Abzuege im Griff 0; Bisse danach 8` (re15 identisch);
  `r38_affen3_tor [std_ruhe]: gesperrt 400, aktiv 0 / [std_frei]: Freigabe F16, erstes aktives Bild F16` (re15 identisch).
- Die Riegel messen den Nutzer-Punkt (HP-Abzug + Spieler-Reaktion + Takt gegen GDB-Spur o10) und wuerden am alten Stand
  rot (Bau-Agent: master 0 Abzuege; eigene Gegenprobe s. M3).
- Merge-Probe (ohne Schreiben): `git merge-tree <basis> r38/integration HEAD` -> nur AUFTRAG.md mit Konfliktmarken
  (Doku-Nachtrag); enemy_ai_common.c und re15_damage.c "changed in both", aber ohne Konflikt.

### M3 — Gegenprobe VORHER: derselbe echte Weg mit der Basis-exe (cb782bda = master-Code)
Eigener Bau im Scratch (`git archive cb782bda re15_port` ohne shared_assets, cmake/ninja mit der msys64-Kette, Assets per
`RE15_CD_ROOT=<baum>/re15_port/shared_assets/PSX`, debug.log: `[asset] cd-root: ...r38_affen3/re15_port/shared_assets/PSX`),
identische Umgebung und identisches Skript wie M1, Lauf v13_weg_vorher (99 s, rv 0, `[room] PC loaded room3040.rdt`):
- **0 HP-Abzuege**; Affe Platz 1 startet 5 Bisse (125 Bild in Sub 5, Clip 18), Leon bleibt bei HP 100 — genau der
  Nutzer-Befund "es passiert nichts". Gleicher Weg mit der Spur-exe (M1): 19 x -5.
- Auch die alte exe treibt den Affen in den schmalen Westgang (x -10798 .. -12303) — die in M2 gesehene Westdrift ist also
  kein neues Verhalten dieser Spur.
=> Der Fix erklaert den Befund: alt prueft der Biss den MITTEN-Abstand < 1000 (`re15_dog_arc(e, pl, 1000, 0x400)`,
Basis enemy_ai_common.c re15_cockroach_ai_tick case 5), den die Koerperschiebung nie zulaesst; neu das Quadrat um den
Kieferpunkt (FUN_8001bff8, selbst verifiziert) — gemessen: gleicher Weg, 0 -> 19 Treffer.

## 4. Vertrag (VERTRAG.md)
- Ressourcen: keine Bank-9-Bits, keine Nachrichten-Ids, keine Items/Dokumente (Diff-Suche nach `flag_set|MSG_ID|k_port|
  Message_on`: 0 Treffer) — entspricht der Zuteilung (S hat keine).
- Pfade (`git diff master...HEAD --name-only`, 13 Dateien): eigene Dateien affen3_r38.c, re15_affen3_r38.h,
  tests/unit/test_r38_affen3.c, tests/unit/probes/r38_affen3.cmake (Sonden-Datei, keine Test-CMakeLists),
  tests/integration/test_r38_affen3{,_decke,_tor}.cmake, Dossier, AUFTRAG-Nachtrag (Orchestrator-Commit 0686d425).
  KEINE release/, platform/android/, shared_assets/PSX/, keine CMakeLists.
- Gemeinsame Dateien: enemy_ai_common.c +28/-11 in 15 Hunks (je 1-2 Zeilen, alle "Runde 38 Spur S"; groesster Hunk 12
  Zeilen = zwei bestehende Einzeiler re15_player_victim_own_bank/_gorilla um 0x29 erweitert + neue 7-Zeilen-Funktion
  re15_player_victim_affen3, die file-statische Opfer-Variablen setzen muss); affen_11c0.c +6/-4 (Bank des Aktor-Typs statt
  fest 0x27 — fuer den Gorilla wertgleich, alle Aufrufer der Familien-Helfer sind 0x27-Code oder affen3_r38.c; Griff-Paar
  um 0x29 erweitert); re15_damage.c 1 Zeile (Zielband 0x29 wie 0x27). VERTRAG §1.4 nennt enemy_ai_common.c fuer B/T, nicht
  fuer S; die Affen-Wurzel liegt aber dort, die Logik selbst steht in der eigenen Datei — als Hinweis notiert, kein Mangel.
- Nebenwirkung Gorilla (0x27, ROOM11C0): unit_maggot_*/unit_r35_affen_* (31) und integration_r37_szene11c0 gruen.
- Spieler-Tor (AUFTRAG-Nachtrag 07:36): jeder neue Angriffsweg liest Spieler+0x93 wie das Original (Stichproben Biss
  @0x80111eac, Todessprung-Entscheid @0x801126ac, Griff-Entscheid @0x80113a24 selbst gelesen); B[15] selbst liest kein Tor
  (selbst gelesen: einzige Zugriffe `801143e0 lbu v1,-13593(v1)` / `8011441c sb v1,-13593(at)` = Setzen, Verzweigung nur
  ueber +0x6 `801142dc sltiu v0,v1,0x9`) — im Dossier vermerkt. Kein Weg setzt einen Status (Gift o.ae.).
- Stage 4/5: Affen-Code in STAGE4.BIN @0x8010c1a0 / STAGE5.BIN @0x8010c320 selbst gegen STAGE3.BIN @0x80110b00 verglichen
  (5580 Woerter): 175 Abweichungen, ausschliesslich j/jal-Ziele (111) und `addiu` nach `lui 0x8012/0x8010` (64, Tabellen-
  adressen, konstante Verschiebung); Datenbereich @0x8011ec14..ee60: gleich bzw. verschobene Zeiger. => "wortgleich" bestaetigt,
  ein Port-Gehirn fuer alle drei Stages ist richtig.

### M1b — derselbe echte Weg, RE1.5-KI (`RE15_AI_FLAVOR=re15`), Lauf b14_weg_re15
19 Treffer je -5, Bilder und Lagen identisch zu M1 (F1358 .. F2369, Abstaende 56,56,56,57,57,57,56...), Leon-Folgeclip 8/9.

### M1c — Bildkontrolle (Hardware-Renderer, `RE15_WINDOW_SCALE=1`, OHNE RE15_SOFTWARE_RENDER)
Weg M1, `RE15_FRAMEDUMP=1340-1600/4:fd_` (66 Bilder, Lauf f1_bild; Treffer identisch F1358/1414/1470/1526/1583).
Kontaktbogen: `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\abn0\bisse_blatt1.png`,
Ausschnitt F1352-F1364: `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\abn0\biss_zoom.png`. Zu sehen: Leon auf dem Laufsteg, der Affe duckt sich (F1352),
springt Leon an (F1356), Leon knickt im Treffer ein (F1360) und weicht zurueck (F1364); zweiter Biss F1404-F1420 ebenso.
Beide Modelle geschlossen, keine zerrissenen Meshes, kein Schweben.

### M4 — Nebenwirkung Waffe/Tod (Plattform, Super Redhawk)
`RE15_GIVE=7:6 RE15_EQUIP=7 RE15_DEBUG_JUMP=3040@250 RE15_PLAYER_POS=-8800,-2400,1024,1 RE15_GEGNER_Y_LOG=gy.log`, Skript
`W5,M1,MA0.3,M1,W3,MU1,MUA0.3,...` (Lauf k2_magnum): der erste Schuss (F177) toetet Affe A (HP 87 -> -113), DEATH (st 3,
Sub 7, Clip 10), ab F217 CORPSE (st 7), Leiche am Boden y = -1800 (= +0x1ba), keine Schaeden an Leon, kein Absturz/Haenger.
(Erster Versuch k1 ohne RE15_GIVE: Redhawk ohne Munition -> kein Schuss; kein Befund.)

### M5 — Nebenwirkung Stage 4/5: Ereignis-Affe ROOM4040 nach der Freigabe (MANGEL)
Aufbau wie der Spur-Riegel `integration_r38_affen3_tor` [frei] (Leon im Ausloeser), zusaetzlich `RE15_GEGNER_Y_LOG=gy.log`:
`RE15_DEBUG_JUMP=4040@250 RE15_PLAYER_POS=-25000,4000,1024,0 RE15_EXIT_AT=900#4040` (Lauf s4_4040, Spur-exe):
```
F16  affe st1/1/0/0 g41 @(-25090,-10000,9230) b0 f1ba0      <- Freigabe: Pos_set y = -10000, Member_set(12,0x41), INIT -> ACTIVE Sub 1
F224 affe st1/2/0/2 g41 @(-25090,-10000,9230)                <- B[1]-Zeitgeber weckt (byte-true @0x80111584-5c4)
F311 affe st1/5/1/0 g41 mo18 @(-24044,-10000,6661)           <- Biss in 10000 Einheiten Hoehe
F324 dHP=5 hp=95 Leon(-25000,0,4000 mo9) | affe ss5 mo18 af14 @(-24674,-10000,5639)
F380 dHP=5 hp=90 Leon(-25000,0,3500 mo9) | affe ss5 mo18 af12 @(-24959,-10000,3615)
F900 affe st1/3/1/0 @(-23859,-10000,2476)                    <- bleibt bis zum Ende auf y = -10000
```
Der Affe kommt NIE herunter, laeuft 10000 Einheiten (> 5 Baender) ueber dem Boden umher und beisst Leon von dort
(FUN_8001bff8 prueft nur X/Z). Basis-exe (cb782bda) gleicher Aufbau (Lauf s4_4040_vorher): Affe ebenfalls auf y = -10000,
aber er bleibt schlafend (Sub 1) — **0 Treffer**. Die Spur macht aus einem stummen Fehler einen sichtbaren: Schaden aus dem
Nichts in Stage 4 (dasselbe Freigabe-Muster Pos_set y = 0xd8f0 = -10000 + Member_set(12,0x41) steht auch in ROOM50A0
@0x01982/@0x019B6, Member_set(12,0x41) in ROOM5031 @0x0119E).
Ursache selbst disassembliert (STAGE3.BIN, wortgleich STAGE4/5): der INIT verzweigt NACH dem Grid-Bit-Block auf die
EXAKTEN Werte +0x9 == 0x40 / 0x41 und schreibt das Zustandswort neu:
```
80110e80 lbu v1,9(a0) / 80110e84 ori v0,zero,0x40 / 80110e88 bne v1,v0,0x80110eac / 80110e90 ori v0,zero,0x4 / 80110e94 sw v0,4(a0)
80110ea4 lbu v1,9(a0) / 80110ea8 ori v0,zero,0x41 / 80110eac bne v1,v0,0x80110ebc / 80110eb4 ori v0,zero,0x104 / 80110eb8 sw v0,4(a0)
```
-> +0x4 = 4 (+0x5 = 0 bzw. 1). Zustand 4 = Tabelle @0x8011eca4[4] = 0x80115734, Unter-Tabelle @0x8011ee34 = {0x8011577c,
0x801158e8, jr ra, jr ra}. Unterzustand 1 (0x801158e8, der 0x41-Fall): +0x93 |= 1|2, +0x1e0 = 1 (`80115930 sb a0,480(v0)`),
Kasten = @0x8011ec48[+0x1e4] (Luft), +0x9 &= 0xbf, **+0x4 = 1, +0x5 = 10, +0x6 = 2, +0x7 = 0** (`8011598c`..`801159bc`),
Clip 0xd Bild 0x13 -> anim_set -> Bild 0x14 = **B[10] Phase 2: der Fall von der Decke** (c1a4(+0x8c,0,-70) bis +0x1ba, Landung
SE 1, Bodenkasten, -> Sub 4; im Port als b_decke_runter vorhanden). Unterzustand 0 (0x40-Fall): Clip 0x1f, danach ACTIVE Sub 2
Phase 1, Clip 3 Bild 0x1d (`80115858`..`801158d4`).
Der Port-INIT (enemy_ai_common.c re15_cockroach_ai_tick case 0 + re15_affen3_init) kennt diesen Zweig nicht (state = 1,
Sub = grid&1), Zustand 4 ist nicht portiert. Der Bau-Agent hat genau diese Luecke als OFFEN 5 ("wie der freigegebene Affe aus
y=-10000 herunterkommt, nicht gemessen") stehen lassen und trotzdem "fertig" gemeldet — CLAUDE.md: "Mechanismus nicht
gefunden = NICHT fertig". Der Riegel integration_r38_affen3_tor prueft nur das Tor, nicht die Lage nach der Freigabe.

## 5. Urteil je Nutzer-Punkt (gegen den Wortlaut)
| Punkt | Urteil | Beleg |
|---|---|---|
| "In Stage 3 die kleinen Affen können einen nicht treffen, es passiert nichts" | **erfuellt** | M1/M1b/M2/M3: echter Weg (Tuer 3050->3040, Treppe, Schalter, Plattform): Spur-exe 19 x -5 mit Flinch (RE2- und RE1.5-KI bildgleich), Elza ROOM3041 3 x -5; Basis-exe gleicher Weg 0 Abzuege bei 5 Biss-Starts; Bildkontrolle M1c; Riegel 95 + 13 gruen; Biss/Schwer/Todessprung/Hieb/Griff/Tor gegen Disasm gelesen |

## 6. Gates
- RE-Gate: alle Konstanten im Diff tragen @0x... oder sind Asset-Daten (s_clip_len = EDD EM029, nachgemessen); 13+
  Stichproben selbst disassembliert, alle stimmen; keine Rate-Woerter, kein Env-Schalter. ABER: der portierte INIT ist
  unvollstaendig (Zweig @0x80110e80-eb8 / Zustand 4 fehlt) und der Bau-Agent meldet "fertig" mit genau dieser offenen
  Mechanik (OFFEN 5) — Verstoss gegen "Mechanismus nicht gefunden = NICHT fertig" (s. Mangel 1).
- Vertrag: haelt (keine Ressourcen, nur erlaubte Pfade, Logik in eigener Datei; Hinweis zu Hunk-Groessen in §4).
- Riegel: haelt (95/95 unit, 13/13 exe, beide KI-Varianten).
- Nebenwirkungen: **haelt NICHT** — Stage-4/5-Ereignis-Affen beissen nach der Freigabe aus 10000 Einheiten Hoehe (M5).
  Gorilla 0x27 und geteilte Opfer-/Schub-Wege: gruen.

## 7. Maengel (nummeriert, nachpruefbar)
1. **Ereignis-Affen Stage 4/5 (Typ 0x29, Freigabe per Member_set(12,0x41)) fallen nicht von der Decke, sondern greifen aus
   y = -10000 an.** Nachpruefen: `RE15_DEBUG_JUMP=4040@250 RE15_PLAYER_POS=-25000,4000,1024,0 RE15_GEGNER_Y_LOG=gy.log
   RE15_STATE_LOG=state.log RE15_EXIT_AT=900#4040` -> gy.log Platz 1 ab F16 `@(-25090,-10000,9230)`, bleibt bis F900 auf
   y = -10000; state.log F324/F380 je hp -5 mit Affe in Sub 5 auf y = -10000 (Leon y = 0). Basis-exe: 0 Treffer (Affe schlaeft).
   Original (selbst disassembliert): INIT @0x80110e80-eb8 setzt bei +0x9 == 0x40/0x41 das Zustandswort auf 0x4/0x104 ->
   Zustand 4 (0x80115734, Unter-Tabelle @0x8011ee34), Unterzustand 1 (0x801158e8) leitet B[10] Phase 2 ein (+0x93 |= 3,
   +0x1e0 = 1, Luftkasten @0x8011ec48[+0x1e4], +0x9 &= 0xbf, +0x4/5/6/7 = 1/10/2/0, Clip 0xd Bild 0x13->0x14) = Fall auf
   +0x1ba, Landung, Sub 4; Unterzustand 0 (0x8011577c, +0x9 == 0x40): Clip 0x1f, dann Sub 2 Phase 1 Clip 3 Bild 0x1d.
   Abhilfe: Zweig im INIT + Zustand 4 (beide Unterzustaende) nach diesen Adressen portieren; Riegel
   integration_r38_affen3_tor [frei] um "Affe erreicht +0x1ba (y = 0) vor dem ersten Biss" erweitern, den Fallweg moeglichst
   gegen eine GDB-Spur des Originals (ROOM4040, Ausloeser AOT 5 @0x00E9C) messen. OFFEN 5 des Dossiers ist damit kein
   "offener Rest", sondern ein Fehler, den die Spur sichtbar macht (vorher stumm).

## 8. Hinweise (nicht blockierend)
- M2/M3: Affe A kriecht/wird in den schmalen Westgang (Breite 1875 < 2 x 1100) versetzt und wartet dort auf ~3500 Abstand;
  das tut auch die Basis-exe, kein neues Verhalten dieser Spur; kein Original-Vergleich fuer diesen Weg vorhanden.
- Port-Treppen-Code (nicht diese Spur): ACTION oestlich neben der Treppe slot 3 (Sonde ausserhalb des Rechtecks) startete den
  Abstieg (Lauf b9_knopf F284).
- OFFEN 3 des Dossiers (aca3c |= 0xc0 ohne Gegenstueck): bestehende Analyse enemy_ai_common.c ~Z. 7645 (nur PSX-Anzeige-
  Re-Inits) — plausibel folgenlos.
- Merge: einziger Textkonflikt AUFTRAG.md (Doku); Code konfliktfrei gegen r38/integration (merge-tree, ohne Schreiben).

## 9. Messdaten (Scratch, nicht im Baum)
Laeufe unter `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\abn0\laeufe\`:
b13_weg (M1), b14_weg_re15 (M1b), f1_bild (M1c), e2_elza_weg (M2), v13_weg_vorher (M3), k2_magnum (M4), s4_4040 /
s4_4040_vorher / s4_4040_bild (M5). Auswerte-Skripte sl.py / treffer.py, Kollisionsplot room3040_bands.png ebenda.
Basis-Bau: `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\vorher_src\build_vorher\platform\pc\re15_pc.exe` (cb782bda).

## Ergebnis
**bestanden = false** — der Nutzer-Punkt (Stage 3) ist erfuellt und gut belegt, aber die Spur haelt das Nebenwirkungs-Gate
nicht: Mangel 1 (Stage-4/5-Ereignis-Affen beissen aus der Luft; fehlender INIT-Zweig/Zustand 4 @0x80110e80-eb8 / 0x80115734).
