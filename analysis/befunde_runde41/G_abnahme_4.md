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

## 4. Nachbesserung 4 / Entscheide G-E3..G-E6 - Messungen an der exe

### 4.1 M2 / G-E4 - Kern-Zeilen 485..498 ab dem Start in jedem Raum (Messkopie re15_pc_g41abn4d.exe, `RE15_GATOR_FX_LOG=1`, Granatwerfer Id 15)
Rezepte = Abnahme 3 Abschnitt 5 (`RE15_GIVE=15:6 RE15_EQUIP=15`):
- gl_2080_direkt (`RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_PLAYER_POS=-9950,-25700,2907,8 RE15_FORCE_CUT=4`, Eingabe
  `W6.0,M0.6,MA1.5,M1.0,W4`, `RE15_FRAMEDUMP=196-260/2:f_`): `[gator-fx] F0 Kern-Zeilen 485..498 (RE2 TEX.TIM) Slot 56 ok - spielweit ab dem
  Start (G-E4)` (im Startraum ROOM1240, vor jedem Sprung); `F200 ROOM2080 ... Quads 6: kern480 0 kern485 6 ... | gezeichnet G 6 (kern485 6
  raum 0)`, F203..F206 `G 5`, F207..F209 `G 2`, ab F210 `G 0` (Abnahme 3: `G 0` in allen Bildern).
- gl_2080_nach2090 (`RE15_DEBUG_JUMP=2090@250 ... RE15_KEIN_GATOR=1 RE15_PLAYER_POS=-8900,-25244,2048`, Leiter DOOR16 nach 2080, derselbe
  Schuss): Atlas `Raum-Atlas Slot 54 ok: 512 x 3456 (16 Bloecke a 216 Zeilen)`, `ROOM2090: Raum-ESP ROOM40A0 -> re2fx_register_raum rc=0`,
  `ROOM2080: Raum-ESP ROOM40A0 ausgetragen`; F236..F247 Zahl fuer Zahl dieselbe Folge wie direkt F200..F211 (6/6/6/5/5/5/5/2/2/2/0/0 bzw.
  kern480 0/20/25/30/35/40/45/45/90/87/105/120).
- gl_1140 (fremder Raum, Lage des Riegels r35_werfer, `RE15_AI_FLAVOR=re2`): `F21 ROOM1140 ... kern485 5 ... | gezeichnet G 5 (kern485 5
  raum 0)` (Abnahme 3: `G 0`), op_unbekannt 0, aus_fenster 0 in allen Bildern.
- Bild `<SP>/gl_werfer_2080_direkt_nach2090_1140_x2.png` (Zeile 1 2080 direkt F200-F206, Zeile 2 2080 nach 2090 F236-F242, Zeile 3 1140
  F20-F26): in 2080 mit und ohne 2090-Besuch derselbe gelb-weisse Muendungsblitz und dieselben rot gluehenden Granatkoerner; in 1140
  Muendungsblitz/Koerner und die Granaten-Explosion, keine Fehlzeichnung (keine falsche Textur/Riesen-Quads).
- RE2-Referenz selbst erzeugt (`python re15_port/tools/r41_kanister2080/re2_explosion_referenz.py <SP>/re2_werfer_referenz.png werfer`,
  liest nur CORE00.ESP + TEX.TIM): `Muendungsblitz Kern 1/0 (0x01002000): CLUT-Zeile 492, TPage 0x1F, ABR 0, 3 Bilder`, `Granatkoerner Kern
  2/0x0c (0x020c0a00): CLUT-Zeile 495, TPage 0x1F, 8 Bilder` - pixelgleich mit G_belege/nb4_ge4_werfer_re2_referenz_aus_re2_daten.png;
  Formen/Farben (weiss-gelber Kern mit rotem Rand -> rot -> dunkelrot; orange-rote Staebchen) wie im Port-Bild.
- Riegel integration_r41_kanister2080_raumfremd prueft genau das (Abschnitt 1b).
- Urteil M2/G-E4: **behoben** (RE2-Beleg selbst gelesen, Abschnitt 2).

### 4.2 Kampf-Verlaeufe (M1/G-E3, M3/G-E5, P3 Ende-zu-Ende) - eigene Laeufe, eigene Auswertung
Helfer `<SP>/kampf.sh` (= Rezept integration_r41_kanister2080_kampf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=2090@250`
bzw. 2091 mit `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `RE15_SET_FLAG=9:110,3:245 RE15_GIVE=3:60 RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100`,
`RE15_INPUT_SCRIPT=W6.0,n x (M0.6,MA0.1,M0.3,MA0.1,W3.0)`, `RE15_GATOR_FX_LOG=1`; KEIN RE15_GB_TEST/RE15_GATOR_KANISTER/RE15_KEIN_GATOR/
RE15_K2080_WURF_GIER, lauf.sh loescht sie aus der Umgebung), Auswertung `<SP>/auswertung.py` (selbst geschrieben, liest debug.log/
gator_boss.log). "Haken HP" = Messhaken RE15_K2080_GATOR_HP=24990 (ein echter Treffer statt 43); v1 = OHNE jeden G-Haken.

| Lauf | Ausloeser | Aufnahme (Stufe 1 / angehaengt) | R1 / Alligator | Kanister beim R1 | R1-Kamera (G-E1/G-E3) | Explosion in den 38 R1-Bildern | Kopf-Klemme |
|---|---|---|---|---|---|---|---|
| **v1_leon** (kein Haken, 250 Bloecke, `RE15_GIVE=3:200,0x15:200`, EXIT_AT 8000#2090) | F7375, Treffer 37 | F143 / F155 | F2290 (3294,-23729) | (6453,-1932,-20955) innen | laufender Cut 1 (160,96) 8/8, 19/19 "voll" -> Wahl Stufe 0 Cut 1, Sperre ohne Schnitt | Spritzring re2fx_pc max 96 in 12 Bildern (Abnahme 3: 0), G kern485 max 31, raum max 64, op_unbekannt 0; Scherben 10 Bilder | 82 Episoden, groesster Ueberstand 1517 |
| **v1_elza** (dto., 2091) | F7375, Treffer 37 | F143 / F155 | F2290 (3294,-23729) | (6453,-1932,-20955) innen | wie v1_leon | wie v1_leon | 82, groesster 1489 |
| k_leon (Haken HP, Start (-6000,-17500)) | F902, Treffer 1 | F103 / F115 | F1429 (-4160,-18621) | (-3557,-1814,-22674) innen | laufender Cut 0 -> Schnitt Cut 6, (127,106), 8/8, 19/19, Stufe 0 | Spritzring max 170 in 38 Bildern, kern485 31, raum 53, op_unbekannt 0; Scherben 10 | 45, groesster 564 |
| plat_leon (Haken HP, Leon (0,-16000)) | F2606, Treffer 1 | F109 / F121 | F2659 (2822,-22589) | (7014,-1864,-22427) innen | laufender Cut 2 (299,158) Start 2/8 ungueltig, 0/19 -> Schnitt Cut 1, (173,96), 8/8, 19/19, Stufe 0 | Spritzring max 96 in 12, kern485 30, raum 65; Scherben 10 | 65, groesster 714 |

Alle vier stimmen Zahl fuer Zahl mit der Endmessung des Dossiers 11.5 (R1-Bild, Lagen, Wahl) - der Ablauf ist deterministisch. In allen:
`Tuer-Ton DOOR06 Satz 1` F8, `Kopf ab ... 8 von 8` F352, `Kanister aufgehoben (Loco c11 Bild 24 ...)` F465, `Zeile 42 stand F531..F629
(99 Bilder)`, `Wsleeping frei` F630, `Werfer Cut 7: Gier 3678, Gier zur Kamera 3791, Versatz -113, frontal 4035/4096`, `Aufschlag im Wasser`
F43, `Kanister im Becken ... = Aufschlagstelle (Bit 116)`, `FG-E3: ... derselbe Alligator`, Kanister-Regionstest nur "im Regions-Viereck",
keine Zeile AUSSERHALB.
Bilder (RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1, Helligkeit x1,8):
- `<SP>/v1_leon_elza_r1_f2288-2318_x2.png` (Zeilen 1-2 Leon, Zeile 3 Elza): F2288 der Alligator mit dem Kanister vor dem Maschinenblock,
  F2290 grau-weisse Wolke und Gluehen am Kopf, ab F2292 Blutausbruch und -fontaene, Kopf und Koerper im Bild bis F2318 (vorher, Abnahme 3:
  Kanister hinter der Suedwand, keine Wolke). Gelb-schwarze Figuren = Spinnen 0x25/0x26 dieses Zweigs (Spur F entfernt sie).
- `<SP>/k_leon_r1_f1428-1466_x2.png`: F1428 Cut 0 (Leon schiesst), Schnitt auf Cut 6, Wolke + Fontaene am Kopf, F1466 zurueck auf Cut 0;
  in Cut 6 ist der Alligator klein (Massstab 587 gegen RE2 1154).
- `<SP>/plat_leon_r1_f2658-2698_x2.png`: F2658 laufender Cut 2 (Koerper vor der Linse), Schnitt auf Cut 1, Explosion und Fontaene mit Kopf im
  Bild bis F2694, F2698 zurueck auf Cut 2 - M1 (Kopf hinter dem Maschinenblock) tritt hier nicht mehr auf.
- Weitere Laeufe: **k_elza** (Haken HP, 250 Bloecke, EXIT_AT 9000#2091): Ausloeser F902, Aufnahme F102/F114, R1 F2536 (-4581,-16671),
  Kanister (-6641,-2024,-19991) innen, laufender Cut 0 (264,102) 8/8 19/19 -> Wahl Stufe 0 Cut 6 (123,109) 8/8 19/19, Spritzring 96 in 12
  Bildern, Scherben 10 = Dossier 11.5 k_elza*. Der Riegel integration_r41_kanister2080_kampf (eigener Lauf lauf_3d5e5348, mit meinem
  auswertung.py nachgelesen): kampf_leon F1429 Cut 6, kampf_elza F2536 Cut 6, nordost_leon F1409 Cut 7 (200,119), plattform_leon F2659
  Cut 1, west_elza F3040 Cut 1 (182,96) - jeweils Kanister innen, Start 8/8, Todes-Clip 19/19 "voll", Spritzring > 0.

- west_leon (Haken HP, Leon (-8000,-16000)): Ausloeser F802 (Treffer 4), R1 F322 (-6643,-13099), Kanister (-6615,-1956,-9182) innen, Cut 5
  (160,89) 8/8 19/19, Spritzring 96 in 12; d15_leon (`SCHIESS_W=W15.0`): R1 F1022 (-1994,-9624), Kanister (-1139,-1788,-5493) innen, Cut 4
  (81,123) Start 5/8 "ungueltig" (3 Sprite-Raender am Bildrand), Todes-Clip 19/19 "voll" - beides = Dossier 11.5. Groesster Sprung je Bild
  bis zum R1: 435 / 586.

### 4.3 P3 Szene in 2080/2081 und Fall in 2090 (Bilder; Haken HP nur fuer einen schnellen Ausloeser, die Szene selbst laeuft ohne Haken)
- s_leon (`RE15_EXIT_AT=630#2080 RE15_FRAMEDUMP=0-630/6:s_`): Zeitlinie debug.log `F118 Schuss 1: Treffer Slot 6 ... hp=-1 state=3` ->
  `F119 Kopf ab: Slot 6 (Part 8 Flagwort 0x4B ...)`, ... `F351 Schuss 8` -> `F352 Kopf ab ... 8 von 8` (je Zombie EIN Schuss, Kopf ab im
  Folgebild), `F405..F435 NPC Lauf Sub 5` zum Kanister, `F465 Kanister aufgehoben (Loco c11 Bild 24)`, `Zeile 42 stand F531..F629`, Wurf F631;
  Kameras Cut 2 (F0-F360), Cut 5 (F420-F528), Cut 7 (ab F540).
  Bilder `<SP>/s_leon_kopfschuesse_cut2_x2.png` (Marvin in der Tuer, die Koepfe platzen nacheinander als rote Wolke, die Zombies fallen,
  F396 alle 8 am Boden) und `<SP>/s_leon_aufnahme_cut5_wurf_cut7_x2.png` (F426-F444 Marvin laeuft zur Reling-Luecke mit dem Kanister,
  F456-F468 kniet, F480-F504 hebt den Kanister an und kippt ihn, F528 haelt ihn quer mit beiden Haenden vor sich; Cut 7 F540-F624 Marvin
  oben an der Reling mit dem Kanister, Untertitel "Marvin: Hey, Leon, try with this!").
- s_elza (2081): `ROOM2081 SZENE (John 0x49 ...)`, dieselbe Zeitlinie; Bild `<SP>/s_elza_john_szene_x2.png` (John koepft die 8 Zombies,
  kniet, haelt den Kanister mit beiden Haenden, Cut 7 "John: Hey, Elza, try with this!").
- fall_leon (`RE15_EXIT_AT=1000#2090 RE15_FRAMEDUMP=0-160/4:f_`): nach dem Wurf 2090 Cut 12 (F4-F56: Reling oben, der Kanister faellt, der
  Alligator schwimmt nah an der Kamera vorbei), `F43 Aufschlag im Wasser bei (-6096,-2076,-21600)`, `Kanister im Becken ... (Bit 116)`,
  `F43 ANSCHWIMMEN an`, ab F58 Cut 0, `KANISTER Stufe 1` F103 / angehaengt F115 im Hintergrund; Bild `<SP>/fall_leon_cut12_aufnahme_x2.png`.

### 4.4 Nebenwirkung der Kopf-Klemme (M3/G-E5): Lagespruenge des Alligators beim Tragen (gemessen, im Dossier nicht benannt)
Werkzeug `<SP>/spruenge.py` (selbst geschrieben): Wurzel-Lage des Alligators (state.log Slot 35) je Bildpaar im letzten 2090-Aufenthalt
bis zum R1; dazu Spur Bs eigene Messzeile `[gator-re2] Sicht Lagesprung max` (gator_re2_pc.c, groesster Wurzelschritt zwischen zwei
aufgetauchten Bildern; Spur Bs Riegel integration_r41_re2gator verlangt im natuerlichen Lauf <= 340 = "groesster authored Wurzelschritt").

| Lauf | groesster Sprung je Bild bis zum R1 | Bildpaare > 300 / > 400 / > 600 / > 1000 | Abnahme 3 (vor der Kopf-Klemme), gleicher Verlauf |
|---|---|---|---|
| v1_leon (kein Haken) | **1517** F156 (-5173,-18311) -> (-3656,-18311) | 59 / 37 / 3 / 1 | F0..F445: max 258, 0 / 0 / 0 / 0 (Sicht-Messzeile dort max 1039 im ersten Aufenthalt, Spur B) |
| v1_elza (kein Haken) | **1489** F156 | 59 / 37 / 3 / 1 | - |
| k_leon (Haken HP) | 523 F708 (-7019,-10798) -> (-6496,-10818) | 17 / 8 / 0 / 0 | F0..F2290: max 307 (das R1-Bild selbst), 1 / 0 / 0 / 0 |
| plat_leon / k_elza / plat_elza | 562 / 526 / 519 | 20/10/0/0, 28/11/0/0, 14/3/0/0 | - |

Mechanismus (state.log + gator_boss.log v1_leon, Bild fuer Bild): F155 Kanister angehaengt, der Alligator dreht auf der Stelle
(Gier 2785 -> 1921). Vorher (Abnahme 3) blieb die Wurzel dabei auf (-5173,-18311). Jetzt: `KOPF-KLEMME an F156: Ueberstand (1517,0)` ->
Wurzel x -5173 -> -3656 -> -2996 -> -2472 -> -2105 -> -1899 -> -1867 (F161), dann F163 vor der Klemme x -3184 (zurueck um 1317), Klemme
+791 -> -2393, F165 wieder -3184. Naheliegende Ursache (Lage gemessen, der Einzelschritt der Gegenbewegung nicht im Code verfolgt): Kopf + Kanister (2784 lang)
passen beim Drehen nicht zwischen die Westwand (Innenflaeche x -8900, SCA @0x6F4) und den Plattform-Block (SCA[4] @0x724 x -1700..1850
z -20000..-12450, selbst gelesen): die Kopf-Klemme schiebt den ganzen Alligator nach Osten bis x -1867 (167 vor der Blockkante, im
Klemmstreifen Kante + Koerperradius), im naechsten Takt steht die Wurzel schon VOR der Kopf-Klemme wieder bei x -3184 (gator_boss.log
`DREH ... pos=(-3184,-18310)`) - die Wand-/Slab-Klemme der Spur B gegen die Plattform und die Kopf-Klemme gegen die Westwand arbeiten
gegeneinander. An der Suedwand im Treffer-Clip 4 (F471..F474: +131/+457/+582/+379 nach Norden, `KOPF-KLEMME ... groesster Ueberstand
601`) dasselbe Muster, im Lauf v1 an derselben Stelle F473/F584/F954/F1186/F1900 wiederholt.
Bilder: `<SP>/v1j_leon_sprung_f153-168_x2.png` (Cut 1: der Alligator liegt beim 1517-Sprung fast ganz ausserhalb des Bildes, nur das
Rumpfende am linken Rand ruckt), `<SP>/v1j2_leon_sprung_f468-479_x2.png` (Cut 1: der aufgerichtete Alligator rutscht im Treffer-Clip in
3 Bildern rund 1400 Einheiten zur Kamera - als schnelles Schlagen lesbar, kein Bildfehler), `<SP>/kj_leon_sprung_f704-711_x2.png` (Cut 0:
Alligator am rechten Rand, kaum zu sehen). Bewertung nach FG-E6: Feinheit (in den gemessenen Bildern kein kaputtes Bild, kein Haenger,
der Kampf endet in allen Verlaeufen mit dem R1), aber eine neue, messbare Unstetigkeit, die die Nachbesserung einfuehrt und die das Dossier
nicht nennt ("kommt ... etwas weniger weit heran" beschreibt sie nicht) -> offen O1 (Abschnitt 8).

### 1b Gezielte Integrations-Tests (Log `<SP>/ctest_integ.log`; -j3 neben fremden Laeufen anderer Agenten)
Ausdruck des Bau-Agenten (21) + integration_r30_granate_laden, integration_r36_elza_fenster, integration_fx_region_cull,
integration_r41_wasser20b0, integration_r41_kleinkram_a_blink (alle Nutzer von re2_fx/Glas/Werfer/Granate bzw. Riegel, die durch Raeume mit
den jetzt spielweit gezeichneten Kern-Zeilen laufen; `<SP>/integ_regex.txt`):
`ctest --test-dir re15_port/build -R "<regex>" --timeout 3000 --output-on-failure -j3` -> **`100% tests passed, 0 tests failed out of 26`**
(1570,32 s; kanister2080_kampf 1239,63 s, r37_itemband 797,59 s, r36_kartenitems_bild 443,84 s, re2gator 214,01 s, kanister2080 155,33 s,
raumfremd 48,82 s). Kein Fenster-Haken rot, kein Nachfahren noetig. Riegel-Lauf kampf (`build/tests/integration/r41_kanister2080_kampf_wd/
lauf_3d5e5348`, mit `<SP>/auswertung.py` nachgelesen): Zahlen siehe 4.2.

## 5. Vertrag
- Pfade (`git diff 9f10bdb0..HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine CMakeLists
  (gezaehlt: 0). Neue RE2-Daten nur unter shared_assets/RE2/ (GATOR40A0.ESP + 7 TIM, KANISTER2080.VBS), im Dossier 11.8 fuer das
  Paket-Gate genannt. tests/unit/probes/r41_kanister2080.cmake = Spur-Registrierung (wie in den Abnahmen 0-3 angenommen).
- Bank-9-Bits 114-117 (Zuteilung 114-119), in test_r36_kartenbits.c k_port[] eingetragen, unit_r36_kartenbits gruen; Nachrichten-Id 42 in
  2080/2081 (gen/kanister2080_r41_texte.inc: "Marvin: Hey, Leon, try with this!" / "John: Hey, Elza, try with this!"), Zuteilung 42-47.
- Gemeinsame Dateien der Nachbesserung 4: enemy_ai_boss_gator.c (2b) 6 Zeilen markiert (FG-E3/FG-E5/G-E5 erlauben Haken in Bs Modulen),
  gator_re2_r41.c (Rangfolge/Kopf-Klemme, markiert, G-E3/G-E5), gator_re2_pc.c (Regionstest, markiert, G-E5), render_pc.c eine Zeile
  RE15_TIM_SLOT_MAX 57 = **G-E6 angenommen** (V1 der Abnahme 3 erledigt), re2_fx.c/re2_fx.h gegenueber Abnahme 3 unveraendert
  (`git diff cc089cde..HEAD -- re15_port/engine/src/re2_fx.c re15_port/include/re2_fx.h` leer). Textur-Platz 54/56: auf master nicht belegt
  (master render_pc.c RE15_TIM_SLOT_MAX 56; `git grep` auf master: 53 Kanister, 55 Glas) - kein Platz-Konflikt bei der Integration.
- Fremde Riegel: tests/unit/test_r41_re2gator.c um die Teile "kopfklemme" (neu) erweitert, registriert in Gs probes-Datei; Bs Riegel
  (unit_r41_re2gator_*, integration_r41_re2gator) gruen.
- Tests je Punkt vorhanden und messend (P1 unit spawn/ebene/start + integration ankunft; P2 unit lage + integration kanister; P3 unit szene/
  bahn/texte/landung/kippen/hals/blick/anschwimmen/r1wahl/fx_*/scherben/kopfklemme + integration szene/elza/kampf/raumfremd); neue Riegel
  parallel-fest (raumfremd: eigene exe-Kopie re15_pc_r41gr_<zufall>, eigener Laufordner, unset der Testhaken) und Linux-fest (exe ohne
  .exe ausserhalb WIN32, cmake_minimum_required).
- Nicht pruefbar hier: PSX-Bau (kein PSn00bSDK auf der Maschine); die Engine-Aenderungen der Nachbesserung 4 sind reines C (int64-
  Arithmetik in gator_re2_r41.c), gator_fx_r41_pc.c/gator_re2_pc.c sind PC-Dateien.

## 6. Querbezug Nachtrag 29 (der Wortlaut, der diesen Lauf ausgeloest hat: Leiche/Kette zu nah an der Leiter, Spur F)
- In diesem Zweig fehlt Spur F ganz (keine Ankunftsszene, kein Bit-110-Setzer; Bit 110 in allen Laeufen per RE15_SET_FLAG, Spinnen noch
  in 2090/2091). G aendert die Startlage des Kampfes nicht; Gs Haken in enemy_ai_boss_gator.c wirken nur mit Kanister/R1 ((2b), G-E1-
  Anker nur in GBP_RE2_TOD) bzw. bei geworfenem Kanister (Anschwimmen) - die Ankunftsszene/der Kampfbeginn der Spur F wird davon nicht
  beruehrt (Kopf-Klemme ohne Kanister und ohne R1 aus: `traegt || r1_phase != 0`).
- Spur F (r41/gatorszene 43cc5f57) setzt in enemy_ai_boss_gator.c zwei Haken: `re15_gatorszene_r41_nachtakt(slot, e)` DIREKT hinter
  `re15_gr2_nachtakt(slot, e); /* (3) */` und einen Block in gb_takt - Gs (2b) steht unmittelbar davor: beim Merge F+G textueller Konflikt
  im selben Bereich, beide Haken additiv zu behalten (Reihenfolge: (2) Wurzel-Klemme, (2b) Kopf-Klemme G, (3) nachtakt B, F-nachtakt).
- Nach der Integration F+G aendern sich Kampfbeginn und -verlauf (Fs neue Startlage, keine Spinnen) - R1-Lagen, Kamerawahl, Kopf-Klemme
  und die Lagespruenge (4.4) neu messen (Riegel kampf mit seinen fuenf Laeufen; Spinnen-bedingte EXIT_AT 5000 dann pruefen).
