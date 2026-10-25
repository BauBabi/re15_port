# Runde 41 — Spur G "kanister2080" — UNABHAENGIGE ABNAHME 4 (2026-10-25, letzte Abnahme nach FG-E6/Nachtrag 32b)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, HEAD 7b3d29ff (merge-base mit master = 9f10bdb0; master steht auf
636f4c81 mit fremden Spuren - massgeblich ist `git diff 9f10bdb0..HEAD`, Nachbesserung 4 = `cc089cde..HEAD`). Pruefer: Abnahme-Agent 4
(glaubt nichts Ungemessenes). Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).
Laufordner/Bilder (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g4abn`.

Gegenstand: Nachbesserung 4 (Maengel M1-M3 + V1 der Abnahme 3, Orchestrator-Entscheide G-E3..G-E6) und Regression der Nutzer-Punkte
P1/P2/P3. Urteilsregel: FG-E6 / Nachtrag 32b ("gut genug": NICHT BESTANDEN nur bei blockierenden Fehlern - Absturz, Haenger/Softlock,
Regression ausserhalb 2080/2090, ein Nutzer-Punkt fehlt ganz oder ist im Spiel kaputt; Feinheiten kommen als "offen" in den Bericht).
Der Wortlaut, der diesen Lauf ausgeloest hat, ist Nachtrag 29 (Leiche/Kette weiter weg von der Leiter) - das ist Spur F; hier nur der
Querbezug (Abschnitt 9).

## 0. Stand der Abnahme
- [x] Lektuere AUFTRAG (Wortlaut + Nachtraege 5/17/20-32b, FG-E1..FG-E6, G-E1..G-E6), VERTRAG, Dossier §11, Abnahme 3
- [x] Code-Diff gelesen (9f10bdb0..HEAD; Schwerpunkt cc089cde..HEAD = Nachbesserung 4)
- [x] Bau (local_build.sh configure + build)
- [x] gezielte Unit-Tests (105/105)
- [ ] gezielte Integrations-Tests
- [x] RE-Gate Stichproben (8 Zitatgruppen)
- [ ] Messungen P1/P2/P3 + M1/M2/M3 an der exe (Leon + Elza)
- [ ] Vertrag
- [ ] Urteil

## 1. Diff, Bau
- Nachbesserung 4 (`git diff cc089cde..HEAD --stat -- re15_port`, 12 Dateien +625/-78): gator_re2_r41.c (+182: re15_gr2_r1_rang
  Rangfolge G-E3, Kopf-Klemme re15_gr2_kopf_klemme/gr2_kk_rechnen/gr2_kk_takt, dieselbe Klemme in der R1-Vorausschau gr2_r1_pfad),
  enemy_ai_boss_gator.c (+6: Haken (2b) nach der Wurzel-Klemme, markiert), re15_gator_re2_r41.h (+11), gator_fx_r41_pc.c (+141/-?:
  kern_laden() beim ersten Bild in jedem Raum = G-E4, Raum-Atlas 512 x (16 x 216) in Platz 54), gator_re2_pc.c (+20: Regions-Test des
  Kanister-Zeichners vor Mesh und Scherben), render_pc.c (Kommentar der Zeile RE15_TIM_SLOT_MAX 57 = G-E6), Riegel (unit r1wahl,
  kopfklemme; integration kampf +Plattform-Lauf/Kanister-innen/Spritzring, NEU raumfremd), Referenz-Werkzeug re2_explosion_referenz.py.
- Gemeinsame Dateien seit der Basis (`git diff 9f10bdb0..HEAD`): enemy_ai_boss_gator.c +20 (Include, Steuerziel, G-E1-Anker nur in
  GBP_RE2_TOD, (2b) Kopf-Klemme, Anschwimmen - alle markiert "Runde 41 Spur G"), scd_room_setup.c +1 (Installer am Blockende), scd_vm.c +1
  (Tick), main.c +4 (Init, Kanister-Zeichner 2080, gator_fx-Zeichner), render_pc.c 1 Zeile (RE15_TIM_SLOT_MAX 57, G-E6), re2_fx.c +136
  (Ops-Block Nachtrag 28, seit Abnahme 3 unveraendert), test_r36_kartenbits.c +5 (k_port[] Bits 114-117).
- Kleinigkeit im Code (kein Mangel): gator_fx_r41_pc.c `kern_laden();` traegt am Zeilenende zwei Kommentare, der zweite ("G-E1 ...
  gr2_kam_r1_start") gehoert zur Zeile davor (re15_gr2_maske_hook).
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (Logs `<SP>/build_configure.log`, `<SP>/build_build.log`). Kontrolle: `find engine include platform tests tools
  -newer re15_pc.exe` liefert nur tools/r41_kanister2080/re2_explosion_referenz.py (Python, kein Kompilat); `git status` sauber; exe
  2026-10-25 19:42:41, md5 b1d73cca881173cb036a14a90589abd0 (letzter Code-Commit 4c72aaba 19:43:08, danach nur Doku/Werkzeug).
  Messkopien re15_pc_g41abn4{,b,c,d}.exe (md5 gleich).

### 1a Gezielte Unit-Tests (Log `<SP>/ctest_unit.log`, `<SP>/ctest_unit_v.log`)
Ausdruck des Bau-Agenten (82) erweitert um alle uebrigen Tests der re2_fx-/Glas-/Werfer-/Gore-Nutzer und der 20B0-/Kleinkram-A-Riegel
(`<SP>/unit_regex.txt`: ... `|unit_re2_gore|unit_fx_emitter_ai|unit_re2_sfx_cadence|unit_re15_re2z_gore_se|unit_r30_granate|unit_r36_elza_fenster|
unit_r38_kleinkram_flammenwerfer|unit_r39_dining1141_fenster|unit_r41_wasser20b0_|unit_r41_kleinkram_a|prop_scd_pc_advance`):
`ctest --test-dir re15_port/build -R "<regex>" --timeout 600 --output-on-failure -j4` -> **`100% tests passed, 0 tests failed out of 105`**
(44,47 s, neben fremden Laeufen anderer Agenten). Ausgaben (-V) der neuen Riegel:
- r1wahl: `r1wahl A (plat_leon F1278, laufender Cut 4 mit 6/19): Wahl Cut 3 Stufe 1`, Faelle B-D ok.
- kopfklemme: `Wurzel (2000,-25400) Gier 0x400 mit Kanister -> Ueberstand vor dem Takt (0,3260)`, `30 Bilder ... Kanister z min -27000`,
  `R1-Start Kanister (2077,-1937,-26418), Spritzring-Punkte im Regions-Viereck 6/6`, `R1-Todes-Clip 38 Bilder, Hals/Kopf/Kiefer z min -27000,
  ausserhalb (Toleranz 300) 0`.
- scherben `10 Bilder Skala 180..1530 Farbe 0x404040, weg ab Bild 10`; re2gator_kamera `R1-Fall 0 (aus Cut 7): G-E1 zeigt Cut 6 (Schnitt),
  Kopf (161,112)`, `R1-Fall 2: G-E1 Cut 3, Kopf (168,105)`.

## 2. RE-Gate (Stichproben der Nachbesserung 4 selbst disassembliert/gelesen; `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` aus dem
Hauptbaum, nur lesend, EXE = info/re2leon/PSX.EXE, Overlays info/re2leon/COMMON/BIN; Datei-Bytes per Python)

| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| Kopf-Klemme: Wandflaechen ROOM2090.RDT SCA @0x6DC, Bloecke @0x6F4 West -8900 / @0x700 Sued -27000 / @0x70C Nord -5400 / @0x718 Ost 7200, 2091 bytegleich | RDT-Kopf +0x20 -> SCA @0x6DC, Gruppen 10/10/10/10/10; @0x6F4 `d0 07 00 64 6c d5 b8 8e 01 ff 00 03` = w 2000, d 25600, x -10900, z -29000 -> Innenflaeche x -8900; @0x700 w 20100 d 2000 z -29000 -> z -27000; @0x70C z -5400 (Innenflaeche); @0x718 x 7200; ROOM2091.RDT 0x6DC..0x724 bytegleich | stimmt |
| anderes Kanister-Ende: KANISTER40A0.MD1 = ROOM40A0 Modell 1 @RDT 0x4024, y -2784..0 | MD1 liegt in ROOM40A0.RDT genau @0x4024; Ecken (Objekttafel-Basis +12): x -267..267, y -2784..0, z -260..274 | stimmt |
| Spritzring-Tafel @0x801000a8 (EM23) | `bytes 0x801000a8 48 --bin EM23_OVL_0000.BIN`: (0,-2800,0) (300,-2100,0) (0,-700,0) (0,0,300) (0,-1400,-200) (-500,-400,200) = Riegel kopfklemme `ring[6]` | stimmt |
| RE2-Bein x -23078 @0x80101048-4c (EM23) | `addiu v0,zero,-23078` / `sw v0,56(s0)` | stimmt |
| Kanister-Regionstest: Obj_model_set-Handler 0x80055260 (Tafel 0x800a74c8[0x2D]) @0x800552d0-e4, ROOM40A0.RDT @0x2A12 word0 0x000B | Tafel @0x800a757c -> 0x80055260; `lhu v0,10(s2) / ori v0,v0,0x1 / sll 16 / sra 16 / sw v0,0(s1)` @0x800552d0-e4; RDT @0x2A12 `2d 01 00 00 00 00 00 00 00 00 0a 00` -> 0x000A oder 1 = 0x000B, Bit 0x10 nicht gesetzt; FUN_800363f0 `lw a1,-8172(s4) / jal 0x8002c820` @0x80036530-34, `bne v0,zero,0x80036558`, `andi v0,v0,0x10 / beq v0,zero,0x800366a4` @0x8003653c-50, Zaehler +0x1EC erst @0x800365bc-c8 | stimmt |
| G-E4 Boot-Task: `lui a1,0x8003 / addiu a1,a1,-18732 / jal 0x80031f6c / addu a0,zero,zero` @0x8002b65c-68 | genau so (a1 = 0x8002b6d4); 0x8002b6d4: `jal 0x800769c4` mit (0,0,1024,512) @0x8002b70c-1c | stimmt |
| G-E4 "TEX TIM" Datei 225 nach 0x8011a000, FUN_80076a40 | `addiu a0,zero,225 / lui a1,0x8011 / ori a1,a1,0xa000 / lui a3,0x8001 / addiu a3,a3,2568 / jal 0x80012fb8 / addiu a2,zero,1` @0x8002b8ac-c4; `addiu v0,zero,28 / sh v0,-1040(at)` (0x800cfbf0) / `jal 0x80076a40` / `ori a0,a0,0xa000` @0x8002b8cc-dc; Bytes @0x80010a08 = "TEX TIM" | stimmt |
| G-E4 CORE00.ESP: FUN_8001babc laedt Datei 144 nach 0x801fc400 und registriert (@0x8001bb38-8c); einziger Aufruf TITLE.BIN @0x801062dc; Raumlader FUN_8001bba4 einziger Aufruf @0x8004a2ec | `addiu a0,zero,144 / lui a1,0x801f / ori a1,a1,0xc400 / ... addiu a3,a3,2252 (0x800108cc) / jal 0x80012fb8` @0x8001bb38-50, `jal 0x8001bca0` @0x8001bb8c; Bytes @0x800108cc = "CORE00.ESP"; Wortsuche jal/j/Zeiger auf 0x8001babc in PSX.EXE und allen COMMON/BIN/*.BIN: nur TITLE.BIN `jal 0x8001babc` @0x801062dc; auf 0x8001bba4: nur EXE @0x8004a2ec | stimmt |

- Suche im Code-Diff 9f10bdb0..HEAD (engine/include/platform) nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/geschaetzt/
  ungefaehr/placeholder/platzhalter/approx/vorlaeufig/"nach Gefuehl"/"sieht richtig": **0 Treffer**. Neue getenv in Nachbesserung 4: keine.
- Konstanten der Nachbesserung 4 ohne Adresse, als PORT-WAHL/Entscheid benannt: Kopf-Klemme selbst (PORT-WAHL; RE2 hat keine Wandklemme,
  belegt mit dem Bein @0x80101048), Bones 5/6/7 (= Bahn-Massstab der Spur B ohne Rumpf-Bones), Rangfolge (= Wortlaut G-E3), Atlas-Grenze 4096
  (GLES, re2fx_pc.c N5), Riegel-Toleranz 300 (Ueberblend-Differenz, im Test begruendet), EXIT_AT 5000 (nur Testlaufdauer).
- Ergebnis RE-Gate: **haelt** (8 Zitatgruppen der Nachbesserung 4 selbst gelesen, alle stimmen).

## 3. P1 / P2 (Regression; Code seit Abnahme 3 dort unveraendert, nachgemessen)
- **p1_leon** (`RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1 RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_KEIN_GATOR=1
  RE15_PLAYER_POS=-8900,-25244,2048 RE15_INPUT_SCRIPT=W1.0,A0.2,W2.0 RE15_SPAWN_DIAG=1 RE15_FRAMEDUMP=0-400/20:f_ RE15_EXIT_AT=400#2080`,
  ECHTER Leiterweg 2090 -> 2080; Bit 110 = Fs Szene per RE15_SET_FLAG, weil Spur F in diesem Zweig fehlt): debug.log Z.161 `[tuer] Sequenz
  Archiv 2 DOOR16`, Z.184 `[k2080] F31 Knall (Knall-Bank Satz 0 = RE2 ROOM1060 Se_on 0x0a @0x0F48 ...) - erstes Betreten nach Bit 110, Bit
  114 = 1`, Z.186 `8 lebende Zombies (Tot-Bits 236..243) Faden 10, Band 8`, Z.196-203 8 x `Sce_em_set type=0x16 ... pos=(..,-14400,..)`,
  F30..F390 `Ebene: 8 Zombies x -12800..-9900 y -14400..-14400, auf der Treppe (x < -16650) 0`. Mitschnitt (Werkzeug der Abnahme 2
  `knall_im_mitschnitt.py`, nach `<SP>/tools_k` kopiert): `RE2_1060 (11025 Hz): t=15.57 s Bild 467 r=0.891 Huelle-max 6853` = erstes
  2080-Bild (867 - 401 + 1); DOOR04/R1030 kein Treffer.
- **p1_elza** (dasselbe mit 2091 -> 2081, `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`): dieselben Zeilen fuer ROOM2081, `RE2_1060 ...
  Bild 1276 r=0.891` = erstes 2081-Bild (1676 - 401 + 1), 8 x Sce_em_set 0x16 y -14400, keiner auf der Treppe.
- Bild `<SP>/p1_leon_elza_2080_zombies_x2.png` (Cut 4, oben Leon F40/F380, unten Elza F40/F380): die Zombies stehen bzw. laufen auf der
  oberen Ebene vor dem Regal/Fass, Leon/Elza an der Leiter links.
- **p2_cut5 / p2_cut5_elza** (`RE15_DEBUG_JUMP=2080@250` bzw. 2081, `RE15_PLAYER_POS=-12000,-24000,0,8 RE15_FORCE_CUT=5
  RE15_FRAMEDUMP=40:f40.ppm`): `F1 Kanister pos=(-6745,-14400,-17625) Cut 5 Kasten=(159..177,75..158) Dreiecke 46`; rote Marke in
  Gas_Can.bmp selbst ausgemessen x 159..175 y 149..163 -> der Kanister steht mit dem Fuss auf der Marke an der Reling-Luecke. Bilder
  pixelgleich mit Abnahme 3 (max. Abweichung 0, Leon und Elza). Bild `<SP>/p2_gascan_bmp_leon_elza_x2.png` (Gas_Can.bmp, Leon, Elza).
- Nebenbefund (kein G-Mangel, gemessen): in diesen erzwungenen Cut-5-Bildern liegt ein grosses dunkles Viereck ueber Laufsteg/Reling.
  Gegenprobe `p2_cut5_bit115` (`RE15_SET_FLAG=9:115` = Szene gelaufen, kein Kanister-Prop) zeigt dasselbe Viereck -> nicht vom Kanister
  der Spur G; Abnahme 2/3 hatten dasselbe Bild (Bild `<SP>/p2_dunkel_test.png`). Im echten Szenen-Cut 5 siehe Abschnitt 4.
