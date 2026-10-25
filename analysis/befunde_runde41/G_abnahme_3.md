# Runde 41 — Spur G "kanister2080" — UNABHAENGIGE ABNAHME 3 (2026-10-25)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, HEAD 8d7e738a (Basis = merge-base r41/integration 9f10bdb0;
letzter Code-Commit 4c622ad4). Pruefer: Abnahme-Agent 3 (glaubt nichts Ungemessenes). Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).
Laufordner/Bilder (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g3abn`.

Gegenstand: Nachbesserung 3 (Maengel M1-M3 der Abnahme 2 = G-E1 Tod im Bild, G-E2 Werfer der Kamera zugewandt, Nachtrag 28
Explosionseffekt des Gaskanisters) und Regression der in Abnahme 2 erfuellten Punkte P1/P2/P3.

## 0. Stand der Abnahme
- [x] Lektuere AUFTRAG (Wortlaut + Nachtraege 5/17/20-29, FG-E1..FG-E5, G-E1/G-E2), VERTRAG, Dossier §10, Abnahme 2
- [x] Code-Diff gelesen (9f10bdb0..HEAD, Schwerpunkt d09d5fff..HEAD = Nachbesserung 3)
- [x] Bau (local_build.sh configure + build)
- [x] gezielte Unit-Tests (80/80)
- [ ] gezielte Integrations-Tests
- [x] RE-Gate Stichproben (Abschnitt 4: 12 Zitatgruppen selbst disassembliert/gelesen)
- [x] Messungen M3 (Explosion), M1 (Tod im Bild), M2 (Werfer), Regression P1/P2/P3 (Leon + Elza; 27 exe-Laeufe)
- [x] Vertrag
- [ ] Urteil

## 1. Diff, Bau
- Massgeblich ist `git diff 9f10bdb0..HEAD` (merge-base; master steht inzwischen auf 275d42d2 mit C+K, `git diff master` enthielte deren
  Stand rueckwaerts). Seit Abnahme 2 (`git diff d09d5fff..HEAD -- re15_port`, 34 Dateien +1836/-48): gator_re2_r41.c (+269: G-E1-Kamerawahl
  gr2_r1_pfad/_beurteilen/_wahl, Fontaene 1:1, Kanister-Scherben), NEU platform/pc/src/gator_fx_r41_pc.c (325: Raum-ESP ROOM40A0
  registrieren/umlegen wie FUN_8001bd38, Kern-Zeilen 485..498, Zeichner, Masken-Haken), re2_fx.c (+136: Ops 4/6/37/38/41/42/69/78),
  gator_re2_pc.c (+45: Scherben statt Mesh), enemy_ai_boss_gator.c (+4: Anker-Nachfuehrung der Wandklemme nur in GBP_RE2_TOD),
  kanister2080_r41.c (+59: re15_k2080_werfer_gier, Bahn in Wurfrichtung, Messzeile "Werfer Cut 7"), main.c (+2: 1 Aufruf),
  render_pc.c (RE15_TIM_SLOT_MAX 56 -> 57), Header (+21), 8 neue Assets shared_assets/RE2/GATOR40A0.ESP + 7 TIM, Riegel (unit fx/blick/
  scherben, re2gator_kamera, integration kanister2080/kampf), Werkzeug re2_explosion_referenz.py.
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (Log `<SP>/build_build.log`). Kontrolle: `find engine include platform tests -newer re15_pc.exe` liefert nur
  tests/integration/test_r41_kanister2080_kampf.cmake (-P-Skript, kein Kompilat); `git status` sauber; exe 2026-10-25 15:38:57, md5
  c4aa7ff6df4c73ee92d811b593b0168f = Stand HEAD (letzte Quellaenderung gator_re2_pc.c 15:38:55).

### 1a Gezielte Unit-Tests (Log `<SP>/ctest_unit.log`, `<SP>/ctest_unit_v.log`)
`ctest --test-dir re15_port/build -R "unit_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_|unit_msg_|unit_action_msg_gate|
unit_alligator_ai|unit_gator_|unit_g5_boss|unit_r35_granate|unit_r34_reaktion|unit_re2_spider_water_2090|unit_r34_re2fx|unit_r34_plattform|
unit_r34_wasser|unit_r34_wurf|unit_r35_fenster|unit_r35_werfer|unit_r36_granate|unit_r36_schuss|unit_r39_hund1121|unit_re2_gore_render|
unit_marvin_render_pose" --timeout 600 --output-on-failure -j2` (Regex des Bau-Agenten + unit_re2_gore_render / unit_marvin_render_pose; alle
Nutzer von re2_fx.c per `grep -l re2fx_ tests/unit/*.c`) -> **`100% tests passed, 0 tests failed out of 80`** (49,43 s).
Ausgaben (-V): blick `RE2 Ada: Gier zur Kamera 3811.4, Dir_set 3700, Versatz -111.4 (frontal 0.985)`, `ROOM2080 Cut 7 Kamera (8095,-9867):
... Werfer 3677 (Nenn 3677), frontal 0.985` (2081 gleich); op69 `Sender frei im 91. Aufruf, hoechstens 45 Kinder ... Endskala 420`; op41
`Bank 9 Sub 8..12 gesehen 5 5 5 4 4`; op4_6 `Teil 0 frei nach 37 Bildern`; op37_78 `Op 16 1, Op 5 31, Op 37 1`; scherben `10 Bilder Skala
180..1530 Farbe 0x404040, weg ab Bild 10`; re2gator_kamera `R1-Fall 0 (aus Cut 7): G-E1 zeigt Cut 6 (Schnitt), Kopf (161,112)`, `R1-Fall 2:
G-E1 Cut 3, Kopf (168,105)`.

## 4. RE-Gate (Stichproben selbst disassembliert/gelesen; `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` aus dem Hauptbaum, nur
lesend, EXE = info/re2leon/PSX.EXE, Overlay `--bin EM23_OVL_0000.BIN`; Datei-Bytes per Python)
| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| R1 P0 `jal 0x80036eb4` a0 = 0x800d051c @0x80102348 (EM23) | `80102348: jal 0x80036eb4` / Verzoegerung `addu a0,s2,zero`; s2 = `lui s2,0x800d / addiu s2,s2,1308` = 0x800d051c @0x801021a0-a4; danach Kiefer `jal 0x80104318` @0x80102354, `sb v0(4),-998(at)` @0x80102364, Fontaene `jal 0x80103eb4` @0x80102368, Spritzring `jal 0x8010338c` @0x80102374, Se_on `lui a0,0x20f / ori 0x1` @0x8010237c-80, `addiu v0,zero,9 / sb v0,363(s1)` @0x80102390-94 | stimmt |
| FUN_80036eb4 (+0x1EC = 0x8000, +0x1E6 = -10, +0x1BC = 0x404040, +0x14C \|= 0x4A, +0x1E4 = Gier + 0x800) | `lui v1,0x40` / `ori v0,zero,0x8000` / `sh v0,492(a0)` / `addiu v0,zero,-10` / `sh v0,486(a0)` / `lw v0,332(a0)` / `ori v1,v1,0x4040` / `sw v1,444(a0)` / `lhu v1,118(a0)` / `sh zero,488/490/496(a0)` / `ori v0,v0,0x4a` / `addiu v1,v1,2048` / `sw v0,332(a0)` / `jr ra` / `sh v1,484(a0)` @0x80036eb4-ef4 | stimmt |
| Objekt-Zeichner FUN_800363f0: Regions-Test @0x80036534 (Ausnahme 0x10), Bit 0x08 @0x80036558-64 -> FUN_8002d3c8/FUN_8002d718, +0x1EC += 1, bei 30 loeschen @0x800365bc-dc | `jal 0x8002c820` @0x80036534, `bne v0,zero` / `lw v0,0(s1)` / `andi v0,v0,0x10` / `beq` @0x8003653c-50; `lw v0,332(s1)` / `andi v0,v0,0x8` / `beq v0,zero,0x800365e0` @0x80036558-64; `jal 0x8002d3c8` @0x80036588, `jal 0x8002d718` @0x800365b4 (a2 = s1+332); `lhu v0,492(s1)` / `addiu v1,zero,30` / `addiu v0,v0,1` / `sh v0,492(s1)` / `andi 0xffff` / `bne v0,v1` / `sw zero,0(s1)` @0x800365bc-dc | stimmt |
| Scherben-Tafel @0x8009DC28 / @0x8009DC64 | `bytes 0x8009dc28 120`: Zeile 0 = 30 59 87 114 140 165 189 212 234 255 275 ... 465, Zeile 1 (@0x8009dc64) = 180 354 522 684 840 990 1134 1272 1404 1530 1650 ... 2790 = k_scherben_skala | stimmt |
| FUN_8002d718 / FUN_8002d3c8: Index `lhu a1,160(a2) / andi 0x7fff / srl 15`, Versatz `sll 16 / sra 26` mal Skala, Farbe `lw t0,112(a2)`, GT4 `lui v0,0x3c00`, KEIN NCLIP | d718: `lhu a1,160(a2)` @0x8002d750, `andi v1,a1,0x7fff` / `srl a1,a1,15` @0x8002d764-68, `lhu v0,0(a3) / lh v1,0(a1) / sll 16 / sra 26 / mult` @0x8002d7d8-ec, `lw t0,112(a2)` @0x8002d84c, `lui v0,0x3c00` @0x8002d858; GTE-Befehle im ganzen Bereich bis `jr ra` @0x8002da78 nur MVMVA 0x4a480012, RTPT 0x4a280030 @0x8002d8ec, RTPS 0x4a180001 @0x8002d93c, AVSZ4 0x4b68002e @0x8002d950 (kein 0x4b400006); OTZ nur nach oben geklemmt (`slti 8192` -> 8191 @0x8002da08-14). d3c8: RTPT @0x8002d5a4, AVSZ3 0x4b58002d @0x8002d5dc, kein NCLIP bis `jr ra` @0x8002d710 | stimmt |
| Fontaene FUN_80103eb4: Tick 1 faellt in den Block 1..8, 0x00064000 UND 0x00034000 an Part 7 (s2+1276), 0x08024000 an Part 6, Versaetze | `beq v1,v0(1),0x80103f20` @0x80103ed8; 0x26032000 an `addiu a2,s2,1104` mit (sra 23 +300, sra 23, sra 22 +200) @0x80103f20-6c, OHNE Sprung weiter bei 0x80103f74 (`addiu s0,s2,1276`); 0x00064000 (sra 22 +300, sra 23, sra 23 +200, a1 `sra 20`) @0x80103f74-cc; 0x00034000 (sra 22 +300, sra 23, sra 23 +200) an s0 = Part 7 @0x80103fd0-0x80104028, `addiu s0,s2,1104` erst im Verzoegerungsplatz @0x80104030; 2 x 0x08024000 (sra 23 +300, sra 23, sra 22 +200) an Part 6 @0x8010402c-a8; Tick 64 `sw zero,564(s1)` @0x801040bc | stimmt (Port gr2_fontaene_takt gleich, inkl. Reihenfolge der rnd-Aufrufe) |
| Op-Tafel 0x8009D868 [4]/[6]/[37]/[38]/[41]/[42]/[69]/[78] | 0x8001df50 / 0x8001e11c / 0x80020660 / 0x800206e0 / 0x800207e4 / 0x800208b8 / 0x80022ebc / 0x800241c8 | stimmt |
| Op 69 (+3 = 90, `sltiu 0x41`, Skala * 955 / 1000) | `andi s2,v0,0xf8` @0x80022ef0, Phase 0: `ori v0,zero,0x8002 / sh v0,24`, +2 := 1, `addiu v0,zero,90 / sb v0,3` @0x80022f08-24 ohne Ruecksprung; +3 == 0 -> `sb zero,1 / sb zero,0 / sh zero,24` @0x800231d4-dc; `sltiu v0,v0,0x41` @0x80023170, Skala = ((v*16-v)*16-v)*4-v = v*955, `lui 0x1062 / ori 0x4dd3`, `sra 6`, `subu` @0x8002317c-b0, +3 -= 1 @0x800231c8 | stimmt |
| Op 41 (Bank 9, Sub +0/+1/+2, Skala 0x800/0x1000/0xc00, a2 = *(Platz+0x6C), a3 = Platz+0x2C, Op A := 42) | `lui t0,0x900 / ori t0,t0,0x800` @0x800207f4-f8, `ori 0x1000` @0x80020830, `ori 0xc00` @0x8002086c; `sll v0,v0,20 / sra a1,v0,16`; `addiu a3,v1,44` / `lbu a0,2(v1)` / `lw a2,108(v1)` / `addiu a0,a0,1|2` / `jal 0x8001cbe8` (3x); `addiu v0,zero,42 / sb v0,0` @0x800208a4-ac | stimmt |
| Lader FUN_8001bd38 (Spalte 15, y laufend, neue Spalte bei 0x100 < y+h, CLUT (0x120, ab 480), +4/+6, v += y) | RE2_Quellcode_V2/FUN_8001bd38.c: `iVar10 = 0xf`, `y = 0x1e0`, `if (0x100 < iVar8 + h) { iVar10--; iVar8 = 0; }`, prect = (iVar10*0x40, iVar8), crect = (0x120, y), `puVar6[2] = GetClut(0x120,y)`, `(char)puVar6[3] = GetTPage(0,0,x,y)`, `*pcVar4 += iVar8` je Zelle ab `+*puVar6*8+9` = gator_fx_r41_pc.c raum_umlegen | stimmt |
| Assets GATOR40A0.ESP = ROOM40A0.RDT[0x59CC:0x65E8], 7 TIM = RDT[0x221FC + Tafel vor RDT[21]] | Python: RDT-Kopf Eintrag 18 = 0x59CC, ESP bytegleich (3100 B, sha1 bcce378c, Ids 16 19 08 09 26 1A 18); Eintraege [20] 0x221FC / [21] 0x2FCFC; Tafel rueckwaerts 0/0x10A0/0x38E0/0x5D20/0x6D80/0x9A20/0xC2A0 -> alle 7 TIM bytegleich | stimmt |
| Sprite-Rand CORE00.ESP Bank 3 Anim 17/18 @0x98/@0xA0, Zellen x -36..36, y -32..36 | Python (Bank 3 @0x8, n1 48, n2 163, Zellen @0x190): A17 x -28..36 y -32..32, A18 x -36..28 y -28..36, Vereinigung A11..A22 x -36..36 y -32..36 (Zellgroesse 16) | stimmt |
| G-E1: RE2 R1 schneidet nur bei Cut 6/13 und z < -13300, +0x218 \|= 0x40; RE2 integriert die Lage FUN_800152c8 @0x801025a4-ac | `addiu v0,zero,6 / beq` / `addiu v0,zero,13 / bne` @0x80102218-24, `lw v0,64(s1) / slti v0,v0,-13300` @0x8010222c-34, `lhu v0,536(s1) / ori v0,v0,0x40 / sh` @0x80102240-4c; `jal 0x800152c8` a1 = 2048 @0x801025a8-ac; FUN_800152c8: RotMatrix der Gier + 2048, `lw v0,56(s1) / addu / sw v0,56(s1)` @0x80015314-20 (inkrementell) | stimmt |
| G-E2: RE2 ROOM7020 Ada Pos_set @0x0740, Dir_set @0x0748 = 3700, Cut 7 RID @RDT 0x144 | RDT-Kopf [17] = SCD-main 0x1128, [7] = RID 0x64; 0x0740 `32 00 a0 92 b4 e2 14 ba` = (-28000,-7500,-17900), 0x0748 `33 00 00 00 74 0e 00 00` = 3700; RID Cut 7 @0x144 = (-22158,-3852,-15174); Gier Ada -> Kamera 3811,4, Versatz -111,4, frontal 0,985 (eigene Rechnung, Konvention re15_atan2_q12(dz,dx) - 0x400 = atan2(dx,dz) - 1024) | stimmt |
- Suche im Code-Diff 9f10bdb0..HEAD -- re15_port nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/geschaetzt/ungefaehr/
  placeholder/platzhalter/approx/vorlaeufig: **0 Treffer**. Neue getenv seit Abnahme 2 nur `RE15_GATOR_FX_LOG` (nur Messzeilen); weiter
  RE15_K2080_WURF_GIER / RE15_K2080_GATOR_HP (Messhaken aus Abnahme 2).
- Konstanten ohne Adresse, als Port-Abbildung benannt: Textur-Platz 54/56 (render_pc.c-Pool), GR2_R1_SOLL (170,111) = gemessene RE2-Bildlage
  der Spur B (k_blick EXPLOSION), GR2_R1_PROBEN 24 / jedes zweite Bild (Vorausschau), Rangfolge der Wahl = PORT-WAHL (G-E1), Toleranz des
  Riegels +-12 (begruendet mit RUN-Ankunft `slti 300` @0x80051764 auf 17061 Kamera-Abstand).

## 5. Nebenwirkung ausserhalb 2090/2091: Kern-Zeilen 485..498 werden nach dem ersten 2090-Besuch in JEDEM Raum gezeichnet (gemessen)
Code: `re15_pc_gator_fx_draw` (gator_fx_r41_pc.c) laeuft in main.c in jedem Raum; der Zweig `kern485` (Seite 0x1E/0x1F, CLUT-Spalte
0x11, Zeile 485..498) prueft NICHT `s_reg`/den Raum, nur `s_kern_tex_ok` - und das wird erst in `laden()` gesetzt, das nur beim
Betreten von 2090/2091 laeuft und danach fuer die ganze Sitzung gilt. Folge: Effekte der RE2-Kernbanken 0/1/2/6/7, die re2fx_pc.c
bewusst auslaesst (Kommentar N5, Runde 34 Spur D: Texturhoehe/GLES), erscheinen ab dem ersten 2090-Besuch ueberall.
Messung (exe-Kopie re15_pc_g41abn3f.exe, `RE15_GATOR_FX_LOG=1`, Granatwerfer Id 15 wie integration_r35_werfer: `RE15_GIVE=15:6
RE15_EQUIP=15`, Eingabe `M0.6,MA1.5,M1.0,W4`):
- gl_1140 (`RE15_DEBUG_JUMP=1140@250`, Lage des Riegels r35_werfer (-1676,-18070,1076)): `F21 ROOM1140 ... Quads 5: kern480 0 kern485 5
  ... | gezeichnet G 0 (kern485 0 raum 0)` - die Muendung 0x01002000 (Kernbank 1 = Zeile 492) wird erzeugt, aber nicht gezeichnet.
- A/B im selben Raum, gleiche Lage/Gier/Kamera, gleicher Schuss (wf.log je `RE2SPAWN a0=01002000` + 5 x `a0=020c0a00`, gleiche knochen):
  gl_2080_direkt (`RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_PLAYER_POS=-9950,-25700,2907,8 RE15_FORCE_CUT=4`,
  Eingabe `W6.0,M0.6,MA1.5,M1.0,W4`): `F200 ROOM2080 ... kern485 6 ... | gezeichnet G 0 (kern485 0 raum 0)`;
  gl_2080_nach2090 (`RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_KEIN_GATOR=1 RE15_PLAYER_POS=-8900,-25244,2048`, Leiter
  DOOR16 nach 2080, Eingabe `W1.0,A0.2,W6.0,M0.6,MA1.5,M1.0,W4`; Spieler dort (-9950,-25700) Gier 2907 cam 4):
  `F236 ROOM2080 ... kern485 6 ... | gezeichnet G 6 (kern485 6 raum 0)` ... F245 `gezeichnet G 2`.
- Bild `<SP>/gl_2080_ab_direkt_oben_nach2090_unten.png` (x2; oben F200-F206 ohne 2090-Besuch, unten F236-F242 nach dem 2090-Besuch):
  unten ein grosser orange-gelber Muendungsblitz und danach rot gluehende Granatkoerner, oben fehlen beide.
Damit sieht derselbe Granatwerfer-Schuss (Runde 35 Spur B, nicht Teil dieser Spur) anders aus, je nachdem, ob in diesem Prozess schon
2090/2091 betreten wurde (nach Laden eines Spielstands wieder ohne). Im Dossier (10.2 "Zeichner fuer genau die Quads, die re2fx_pc/
glas1120 auslassen") ist das nicht benannt und nicht gemessen; kein Riegel der Spur prueft andere Raeume.

## 2. Messungen M3 / M1 / M2 im echten Ablauf (Laufordner `<SP>/runs/<lauf>`; Helfer `<SP>/lauf.sh`, `<SP>/kampf.sh` = Rezept des Riegels
integration_r41_kanister2080_kampf Stand 8d7e738a: 60 Schussbloecke, `RE15_GATOR_FX_LOG=1`; KEIN RE15_GB_TEST / RE15_GATOR_KANISTER /
RE15_KEIN_GATOR / RE15_K2080_WURF_GIER, lauf.sh loescht sie zusaetzlich aus der Umgebung; Messkopien re15_pc_g41abn3{,b..f}.exe, md5 = re15_pc.exe)
Gemeinsam: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=2090@250` (Elza 2091 + `RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1`) `RE15_SET_FLAG=9:110,3:245 RE15_GIVE=3:60 RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_INPUT_SCRIPT=W6.0,n x
(M0.6,MA0.1,M0.3,MA0.1,W3.0)`. Bit 110 per RE15_SET_FLAG (Spur F fehlt in diesem Zweig). "Haken HP" = Messhaken RE15_K2080_GATOR_HP=24990
(ein echter Treffer statt 43); v1 = OHNE jeden G-Haken (37 echte Treffer, `RE15_GIVE=3:200,0x15:200`, 250 Bloecke).

### 2.1 Verlaeufe (debug.log [k2080]/[gator-fx]/[gator-re2], gator_boss.log; Auswertung `<SP>/fxsum.py`)
| Lauf | Ausloeser | Wurf | Werfer Cut 7 | Aufnahme | R1 / Kamera (G-E1) | Explosion (R1-Bilder, gezeichnete Quads) | Scherben |
|---|---|---|---|---|---|---|---|
| k_leon (Riegel-Startlage (-6000,-17500), Haken HP) | F902, Treffer 1 | Sued 1024 -> (-6096,-21600) | Gier 3678, Versatz -113, frontal 4035/4096 | ANSCHWIMMEN F43, Stufe 1 F103 | F2290: laufender Cut 2 Kopf (83,160) 8/8 -> Schnitt Cut 1, Kopf (142,104), Start 8/8, Kopf ok, Kiefer ok, Todes-Clip 19/19; zurueck Cut 2 F2326; Leiche (3208,-21484) | 38 Bilder F2290-F2327: re2fx_pc bis 96, G kern485 bis 30, G raum bis 67, op_unbekannt 0 | 10 Bilder Skala 180..1530, 95 Prims |
| k_elza (dto.) | F902, Treffer 1 | Sued 1024 -> (-6068,-21590) | dto. | F43, Stufe 1 F102 | F568: laufender Cut 0 Kopf (320,81) ungueltig -> Schnitt Cut 7, Kopf (110,101), Start 6/8, Kopf ok, Kiefer ok, 19/19; zurueck Cut 0 F604; Leiche (-7416,-6884) | 38 Bilder F568-F605: re2fx_pc bis 64, G kern485 bis 29, G raum bis 65, op_unbekannt 0 | 10 Bilder |
| **v1_leon (kein G-Haken)** | **F7375, Treffer 37** | Nord 3072 -> (-6856,-12670) | Gier 3678, Versatz -113, frontal 4035 | F50, Stufe 1 F143 | F445: Cut 1 Kopf (265,100), Start **2/8 (6 ausserhalb)**, Kopf ok, Kiefer ok, 19/19 -> Sperre ohne Schnitt; keine Zonenkamera hat einen gueltigen Start (Cut 3: 1/8, 13/19; Cut 4/6: 0/8); Leiche (5322,-25516) | 38 Bilder F445-F482: **re2fx_pc 0** (Spritzring-Wolke ganz verworfen), G kern485 bis 24, G raum bis 61 | 10 Bilder |
| **v1_elza (kein G-Haken)** | F7375, Treffer 37 | Nord 3072 -> (-6828,-12660) | dto. | F50, Stufe 1 F143 | F322: Cut 1 Kopf (223,95), Start 5/8 (3 ausserhalb), Kopf ok, Kiefer ok, 19/19 -> Sperre ohne Schnitt | 38 Bilder: re2fx_pc bis 48, G kern485 bis 30, G raum bis 61 | 10 Bilder |
Die Zeilen decken sich Bild fuer Bild mit den Angaben des Bau-Agenten (10.3/10.4a: kampf_leon F2290, kampf_elza F568, v1 Cut 1 (265,100) /
(223,95)) - der Ablauf ist deterministisch. In allen vier Laeufen ausserdem: `Tuer-Ton DOOR06 Satz 1` F8, `Kopf ab ... 8 von 8` F352,
`Kanister aufgehoben` F465, `Zeile 42 stand F531..F629 (99 Bilder)`, `Wsleeping frei` F630, `FG-E3: ... derselbe Alligator`, Aufschlag F43.
Farbe des Blutes (`<SP>/rot.py`, k_leon_r1 F2294-F2308, Bereich (40..140,50..130)): haeufigste Rotwerte (88,0,0)/(104,0,0)/(64,0,0) = wie im
RE2-Referenzbild des Bau-Agenten (88,0,0).

### 2.2 Bilder der R1-Phase (zweiter Lauf mit `RE15_FRAMEDUMP`, gleiche Zeilen)
- k_leon_r1 (`RE15_FRAMEDUMP=2280-2340/2`): `<SP>/k_leon_r1_f2286-2316.png` (1x) und `<SP>/k_leon_r1_f2290-2304_x3.png` (x3, Helligkeit 1,8):
  F2290 = letztes Bild in Cut 2 (Apply-Bild des Schnitts) mit grau-weisser Wolke + Blut + weissen Splittern gross im Vordergrund; ab F2292
  Cut 1: der Alligator liegt links, am Kopf grau-weisse Wolke, roter Blutausbruch, blau-weisses Gluehen, ab F2296 steigt die Blutfontaene
  bis F2316. Kopf und Kiefer im Bild, nicht verdeckt. (Im Vordergrund unten eine Spinne 0x25 - in diesem Zweig noch in 2090, Spur F.)
- k_elza_r1 (`RE15_FRAMEDUMP=560-620/2`): `<SP>/k_elza_r1_f564-612.png` (1x), `<SP>/k_elza_r1_f568-590_x4.png` (x4, Helligkeit 2,2):
  Cut 7 = Fernblick den Kanal entlang; der Alligator liegt mit dem Kopf links an der Pfeilerkante (Kopf (110,101)), Explosion und Blutfontaene
  am Kopf sichtbar, bei 1x aber klein und dunkel (Massstab 455 gegen RE2 1154 = 0,39); F610 zurueck auf Cut 0.

### 2.3 G-E1 in den weiteren Pflicht-Verlaeufen (Rezept 2, Haken HP; gator_boss.log "KAMERA R1 G-E1 ...")
| Lauf | R1 | laufender Cut | G-E1-Wahl | Kandidaten (Stufe 0) | Ergebnis |
|---|---|---|---|---|---|
| d15_leon (`SCHIESS_W=W15.0`) | F592 pos (-7389,-7156) | Cut 0, Kopf (302,84), 8/8, 19/19 | Schnitt Cut 5, Kopf (168,92), Start 8/8, Kopf ok, Kiefer ok, 19/19; zurueck Cut 0 F628 | Cut 7 (126,105) 8/8 19/19 d=1972 | Tod im Bild (wie Bau-Agent d15h) |
| west_elza (Start (-8000,-16000)) | F322 pos (-6928,-12375) | Cut 0, Kopf (321,85) ungueltig | Schnitt Cut 5, Kopf (175,96), 8/8, 19/19; zurueck Cut 0 F358 | Cut 7 (138,104) 8/8 19/19 | Tod im Bild |
| **plat_leon (Leon auf der Plattform (0,-16000))** | **F1278 pos (5722,-7102)** | **Cut 4, Kopf (128,99), Start 8/8, Todes-Clip 6/19** | **Sperre ohne Schnitt: Cut 4 bleibt, "Kopf ok, Kiefer ok, Todes-Clip 6/19 Proben"** | Cut 3: Start 8/8, 19/19, aber massstab 2158 > grenze 1892 (Stufe 0 gesperrt); Cut 2: Start 2/8 (6 verdeckt), 16/19 | **in 13 von 19 Proben des Todes-Clips Kopf/Kiefer NICHT sichtbar** (Bilder 2.4) |
Der Bau-Agent meldet fuer "Plattform Leon (plath)" einen anderen R1 (Leiche (-7416,-6884), Schnitt Cut 5, 19/19); sein Lauf ist nicht im Baum,
das Rezept (Startlage/Bloecke) weicht offenbar ab. Mit dem Riegel-Rezept (60 Bloecke, Haken HP) und Leon auf der Plattform kommt der R1 bei
(5722,-7102) und die Rangfolge "gueltiger Start vor sichtbaren Proben" (gr2_r1_wahl (1) vor (2)) waehlt den laufenden Cut 4 mit 6/19.

### 2.4 Bilder plat_leon / v1_leon (zweiter Lauf mit RE15_FRAMEDUMP; Zeilen gleich dem ersten Lauf)
- plat_leon_r1 (`RE15_FRAMEDUMP=1272-1320/2`, `RE15_EXIT_AT=2700#2090`): `<SP>/plat_leon_r1_f1276-1318.png` (1x),
  `<SP>/plat_leon_r1_f1278-1316_x3.png` (x3, Helligkeit 2,0): F1278-F1286 Explosion (grau-weisse Wolke, Blut, Glut) am Kopf links der
  Maschine gut im Bild; danach rutscht der Alligator im Todes-Clip nach rechts, ab F1290 liegt der Kopf hinter dem Maschinenblock/Gelaender
  am rechten Rand - sichtbar bleibt die Blutfontaene ueber der Maschine, Kopf und geplatzter Kiefer sind nicht mehr zu erkennen (gator_boss.log:
  6/19 Proben). Leon steht ab F1300 links im Bild.
- v1_leon_r1 (`RE15_FRAMEDUMP=436-500/2`, `RE15_EXIT_AT=8000#2090`): `<SP>/v1_leon_r1_f440-478.png` (1x), `<SP>/v1_leon_r1_f444-458_x4.png`
  (x4, Helligkeit 1,8): F446 ff. Blutausbruch, Fleischbrocken, dunkle Kanister-Scherben, orange Glut, Blutfontaene am Kopf am rechten Rand;
  KEINE grau-weisse Wolke (Spritzring, re2fx_pc 0 in allen 38 R1-Bildern). Ursache gemessen: debug.log `[gator-re2] Kanister Stufe 13
  pos=(5996,-2041,-29290)` - der Kanister liegt beim R1 jenseits der Suedwand des Beckens (Steuerziel-Klemme der Spur B bis z -27000+1600),
  alle sechs Ring-Punkte (Kanister-Matrix x Tafel @0x801000a8, z.B. (0,-2800,0) -> (8761,-2159,-29619)) liegen ausserhalb jedes
  Regions-Vierecks und werden ueber `jal 0x8002c820` @0x80077a30 verworfen. Kopf und Kiefer sind im Bild (19/19).
  Regions-Vierecke von ROOM2090 selbst gelesen (RDT @0x28 = RVD, 29 Zonen, erste Zone je cam_from = Anker wie re15_rdt_get_region_quad):
  ALLE 13 Cuts haben dasselbe Anker-Viereck x -11000..9500, z -29000..-3500 = der ganze Raum. Der Kanister liegt beim R1 also AUSSERHALB
  DES RAUMS (z -29290 < -29000): der Kopf des Alligators steckt in der Suedwand (die Wandklemme haelt nur die Wurzel). Mit RE2 (R1 im Gang,
  Schnitt auf Cut 5) hat das nichts zu tun; das Dossier (10.3/10.6.2 "wie im Original verworfen, keine Zonenkamera hatte dort alle acht")
  benennt die Ursache nicht.

### 2.5 M2 (G-E2) Bilder Cut 7
- v1_leon_c7 (Nordwurf ohne Testhaken, `RE15_FRAMEDUMP=522-630/4`, `RE15_EXIT_AT=630#2080`): `<SP>/v1_leon_c7_nord_f526-626.png` (1x),
  `<SP>/v1_leon_c7_nord_marvin_x5.png` (x5): F534-F630 Marvin an der oberen Reling von vorn (Gesicht, Hemd), den Kanister vor sich, ab F546
  ueber dem Kopf gestemmt, Zeile "Marvin: Hey, Leon, try with this!" F530-F630; die gekoepften Zombies liegen hinter ihm. F530 = Apply-Bild
  (Cut 5 mit der Zeile, H1 der Abnahme 2, Zweig ohne Spur E).
- Blickwert unabhaengig nachgerechnet (Python, RID von ROOM2080/2081.RDT @0x60, Cut 7 @0x144-Form: Kamera (8095,-18819,-9867)): Werfer
  laut state.log F531 Slot 9 `@(-7168,-17609,r3678)` -> Gier zur Kamera 3790,0, Versatz -112,0, frontal 0,9853 (RE2 Ada -111,4 / 0,985); 2081
  gleich. Werfer-Zeile in ALLEN zehn Laeufen dieser Abnahme (Nord und Sued, Leon und Elza): `Werfer Cut 7: Gier 3678, Gier zur Kamera 3791,
  Versatz -113, frontal 4035/4096`; Wurfbahn Sued v=(10,20,-25) -> (-6096,-21600), Nord v=(-10,20,25) -> (-6856,-12670) - Bahn in Weltrichtung.

### 2.6 Regression P1 / P2 (Code seit Abnahme 2 unveraendert; nachgemessen)
- p1_leon (`RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1 RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_KEIN_GATOR=1
  RE15_PLAYER_POS=-8900,-25244,2048 RE15_INPUT_SCRIPT=W1.0,A0.2,W2.0 RE15_SPAWN_DIAG=1 RE15_EXIT_AT=400#2080`, echter Leiterweg DOOR16):
  Z.153 `[tuer] Sequenz Archiv 2 DOOR16`, Z.173/176 `F31 Knall ... Bit 114 = 1`, Z.178 `8 lebende Zombies ... Band 8`, Z.188-195 8 x
  `Sce_em_set type=0x16 ... pos=(..,-14400,..)`, `Ebene: 8 Zombies ... auf der Treppe (x < -16650) 0`. Mitschnitt (Werkzeug der Abnahme 2
  knall_im_mitschnitt.py): `RE2_1060 (11025 Hz): t=15.57 s Bild 467 r=0.891` = erstes 2080-Bild (867 - 401 + 1). p1_elza (2091 -> 2081):
  dieselben Zeilen, `RE2_1060 ... Bild 1280` = 1680 - 401 + 1 = erstes 2081-Bild, 8 x Sce_em_set 0x16.
- p2_cut5 / p2_cut5_elza (`RE15_DEBUG_JUMP=2080@250` / 2081, `RE15_PLAYER_POS=-12000,-24000,0,8 RE15_FORCE_CUT=5 RE15_FRAMEDUMP=40:f40.ppm`):
  `F1 Kanister pos=(-6745,-14400,-17625) Cut 5 Kasten=(159..177,75..158) Dreiecke 46`; Bild `<SP>/p2_gascan_leon_elza_x2.png`: Kanister auf der
  roten Marke von Gas_Can.bmp; gegen die Bilder der Abnahme 2 (g2abn/runs/p2_cut5*/f40.ppm) Elza pixelgleich, Leon max. Abweichung 10/255
  (kein Pixel > 16).

- s_leon / s_elza (Suedwurf, Haken HP, `RE15_FRAMEDUMP=522-630/4`, `RE15_EXIT_AT=630#2080` bzw. 2081): `<SP>/s_leon_sued_marvin_x5.png`,
  `<SP>/s_elza_sued_john_x5.png` - Marvin bzw. John von vorn mit dem Kanister vor sich/ueber dem Kopf, dasselbe Bild wie beim Nordwurf;
  Zeile `Werfer Cut 7: Gier 3678, ..., frontal 4035/4096 ..., Wurfrichtung 1024` (s_elza: `ROOM2081 SZENE (John 0x49 ...)`).

### 2.7 Alle R1-Verlaeufe dieser Abnahme (Kanister-Lage beim R1 aus `[gator-re2] Kanister Stufe 13 pos=`; Raum-Viereck x -11000..9500, z -29000..-3500)
| Lauf | R1-Lage Alligator | Kanister beim R1 | G-E1-Cut, Kopf | Start | Todes-Clip | Spritzring-Wolke (re2fx_pc max) | Raum-/Kern485-Quads (G max) |
|---|---|---|---|---|---|---|---|
| k_leon | (950,-24882) | (1874,-1939,-20754) | Cut 1 (142,104) | 8/8 | 19/19 | 96 | 67 / 30 |
| k_elza | (-6797,-7359) | (-9986,-1880,-4622) | Cut 7 (110,101) | 6/8 | 19/19 | 64 | 65 / 29 |
| d15_leon | (-7389,-7156) | (-7394,-1839,-11396) | Cut 5 (168,92) | 8/8 | 19/19 | 96 | 67 / 30 |
| d15_elza | (-7383,-7160) | (-7374,-1839,-11400) | Cut 5 (167,92) | 8/8 | 19/19 | 96 | 67 / 30 |
| west_leon | (-7380,-25346) | (-10831,-2028,-27801) | Cut 6 (217,102) | 3/8 (5 ausserhalb) | 19/19 | 16 | 65 / 30 |
| west_elza | (-6928,-12375) | (-3482,-1959,-10330) | Cut 5 (175,96) | 8/8 | 19/19 | 96 | 65 / 30 |
| plat_leon | (5722,-7102) | (5402,-2032,-11294) | **Cut 4 (128,99) ohne Schnitt** | 8/8 | **6/19** | 96 | 69 / 30 |
| plat_elza | (-7188,-6884) | (-10422,-1890,-4136) | Cut 7 (105,100) | 5/8 | 19/19 | 48 | 65 / 30 |
| v1_leon (ohne Haken) | (4324,-25408) | **(5996,-2041,-29290) = ausserhalb des Raums** | Cut 1 (265,100) ohne Schnitt | **2/8 (6 ausserhalb)** | 19/19 | **0** | 61 / 24 |
| v1_elza (ohne Haken) | (4585,-25219) | (8714,-1875,-26043) | Cut 1 (223,95) ohne Schnitt | 5/8 | 19/19 | 48 | 61 / 30 |
In allen zehn R1: Raum-ESP ROOM40A0 registriert (`Banken 0x26 1, 0x08 1, 0x09 1, 0x1A 1`), op_unbekannt 0, zehn Kanister-Scherben-Bilder
180..1530 / 0x404040 / 95 Prims. Wo der Alligator an einer Wand stirbt, ragt der Kanister (RE2-Groesse, Ring-Punkte bis 2800 entlang seiner
Achse) ueber die Raumgrenze, und die Ring-Sprites werden verworfen - im natuerlichen Leon-Verlauf (v1) alle sechs.

## 6. Vertrag
- Pfade (`git diff 9f10bdb0..HEAD --name-only`): nur G-eigene Dateien, markierte Haken und neue RE2-Daten unter shared_assets/RE2/ (GATOR40A0.ESP
  + 7 TIM, KANISTER2080.VBS; im Dossier 10.7 fuer den Nutzer genannt); keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/,
  keine Test-CMakeLists (probes/r41_kanister2080.cmake = Spur-Registrierung). AUFTRAG.md/VERTRAG.md = vom Orchestrator gemergt.
- Bank-9-Bits / Nachrichten-Ids / AOT-Slots seit Abnahme 2 unveraendert (114-117, Id 42 in 2080/2081, Slot 21); test_r36_kartenbits.c seit
  d09d5fff unveraendert, unit_r36_kartenbits gruen.
- Gemeinsame Dateien seit Abnahme 2: main.c +2 (1 Aufruf re15_pc_gator_fx_draw, markiert), enemy_ai_boss_gator.c +4 (Anker-Nachfuehrung nur in
  GBP_RE2_TOD, markiert "Runde 41 Spur G, G-E1" - gedeckt durch G-E1/FG-E3; Querbezug Spur F Nachtrag 29 im selben Modul), gator_re2_r41.c
  (+269 seit der Basis; G-E1-Kamerawahl, Fontaene, Scherben - gedeckt durch G-E1 und Nachtrag 28 "kleine markierte Haken in B's Modulen und
  in re2_fx.c erlaubt"), gator_re2_pc.c +45 (Scherben, B's PC-Zeichner, markiert), re2_fx.c +136 (acht Ops als markierter Block - fuer
  Nachtrag 28 noetig, aber kein "kleiner Haken"; Merge-Querbezug).
- **V1: render_pc.c** (`RE15_TIM_SLOT_MAX 56 -> 57`, 1 Zeile + Kommentar, markiert "Runde 41 Spur G (Nachtrag 28)") steht in VERTRAG 1.3 nur
  fuer Spur D; Nachtrag 28 erlaubt Haken nur in B's Modulen und re2_fx.c. Klein und markiert, braucht aber einen Orchestrator-Entscheid
  (wie B5/J-E1). Texturplaetze: auf master sind 53 (Kanister, B) und 55 (Glas) belegt, 54 frei - G nimmt 54 und 56; Hoehe Platz 54 =
  256 x 16 CLUT-Zeilen = 4096 (genau an der GLES-Grenze aus re2fx_pc.c N5), Platz 56 = 3584.
- Fremder Riegel geaendert: tests/unit/test_r41_re2gator.c teil_kamera (Spur B) - die alte Erwartung "R1 aus Cut 7 ohne Schnitt / Winkelwahl" ist
  durch G-E1 ersetzt (Kommentar mit Begruendung im Test, F-E1-Form); unit_r41_re2gator_kamera gruen.
- Tests je Punkt: unit fx_register/op69/op41/op4_6/op37_78 (Ops gegen die Disassembly), blick (Ada-Versatz aus der RE2-RDT nachgerechnet),
  scherben (echter Boss-Takt), re2gator_kamera (G-E1); integration kanister2080 (Werfer-Zeile, Raum-ESP-Umlage/Registrierung/Austragen) und
  kanister2080_kampf (4 Laeufe: Kopf/Kiefer + Todes-Clip n/n, Werfer, Explosions-Quads > 0, Scherben 180..1530). LUECKE: kein Riegel faehrt
  die Plattform-Lage (dort 6/19, Abschnitt 2.3) und keiner prueft andere Raeume auf die Kern-Zeilen 485..498 (Abschnitt 5).

## 7. Urteile (gegen den Wortlaut, AUFTRAG.md inkl. Nachtraege 5/17/20-28 und Entscheide FG-E1..FG-E5, G-E1/G-E2)
| Punkt | Urteil | Beleg |
|---|---|---|
| P1 erst Metalltuer-Sound (N17: Knall wie nach Irons), dann 8 gespawnte Zombies oben im Hauptbereich (N20) | **erfuellt** | 2.6: echter Leiterweg DOOR16, Leon + Elza: Knall-Bank Satz 0 (Mitschnitt RE2_1060 r 0,891 im ersten 2080/2081-Bild) vor 8 x Sce_em_set 0x16 y -14400, keiner auf der Treppe |
| P2 Gaskanister am Rand der Reling (Gas_Can.bmp) | **erfuellt** | 2.6: Cut 5, Kasten (159..177,75..158) auf der roten Marke, Leon + Elza, Bild wie Abnahme 2 |
| P3 "oft genug angeschossen" -> Szene (N22: RE2-Schwelle 300) | **erfuellt** | v1 Leon + Elza ohne jeden G-Haken: `F7375 AUSLOESER ... Treffer 37` |
| P3 erneut schliessende Tuer, Marvin/John im Raum, 8 Kopfschuesse | **erfuellt** | alle Szenen-Laeufe: `Tuer-Ton DOOR06 Satz 1` F8, Marvin 0x40 / John 0x49, `Kopf ab ... 8 von 8` F352 |
| P3 "rennt hin, nimmt ihn auf in beide Haende" (N24b/N26) | **erfuellt** (Regression) | Code seit Abnahme 2 unveraendert; `Kanister aufgehoben (Loco c11 Bild 24 ...)` F465 in allen Szenen-Laeufen |
| P3 sagt "Hey, Leon/Elza, try with this!" | **erfuellt** | `Zeile 42 stand F531..F629 (99 Bilder)`, `Wsleeping frei` F630 vor dem Wurf; Zeile im Bild (2.5) |
| P3 Wurf die Reling runter, Kamera "GENAUSO" wie Adas Wurf (N5/FG-E4/N26/G-E2) | **erfuellt** | 2.5: Werfer in Cut 7 der Kamera zugewandt in allen zehn Laeufen (Versatz -113, frontal 4035/4096; RE2 Ada -111 / 0,985 selbst nachgerechnet), Bilder Nord/Sued Leon + Elza von vorn; Bahn in Weltrichtung |
| P3 "liegt dann in 2090 an der Stelle im Wasser" | **erfuellt** | `Aufschlag im Wasser`, `Kanister im Becken ... = Aufschlagstelle (Bit 116)` F43 in allen Laeufen |
| P3 "wenn der Aligator dran vorbei kommt, ... aufnehmen, GENAUSO wie RE2" | **erfuellt** | ANSCHWIMMEN F43/F50, `KANISTER Stufe 1` F102/F103/F143 in allen Laeufen, auch v1 ohne Haken; derselbe Alligator (FG-E3) |
| P3 "Sobald er den Kanister im Maul hat, soll man auf ihn schiessen koennen" | **erfuellt** | R1 in allen zehn Kampf-Laeufen (2.7) |
| P3 "die Sterbe Animation mit geplatzen Kiefer etc. ... genauso wie im Original" + N28 (Explosionseffekt) + G-E1 (Tod im Bild) | **teilweise** | Explosion jetzt aus allen RE2-Quellen (Raum-ESP 40A0 registriert, Raum-/Kern485-Quads in allen 38 R1-Bildern, Scherben 180..1530, Bilder 2.2/2.4; Blutfarbe wie RE2-Referenz); Tod im Bild in 9 von 10 Verlaeufen. ABER: Plattform Leon Todes-Clip nur 6/19 im Bild (M1); im natuerlichen Leon-Verlauf (v1) fehlt die grau-weisse Wolke ganz, weil der Kanister ausserhalb des Raums steckt (M3); die neue Zeichnung wirkt nach dem ersten 2090-Besuch in allen Raeumen (M2) |
| RE-Gate | **haelt** (Stichproben) | Abschnitt 4: 14 Zitatgruppen selbst disassembliert/gelesen, alle stimmen; 0 Rate-Woerter. Aber: "wie im Original verworfen" (Dossier 10.3/10.6.2) ist fuer v1 Leon kein Beleg - die Ursache ist der Kanister ausserhalb des Raums (M3) |
| Nebenwirkungen | **nicht ok** | Abschnitt 5 (M2) |
| Vertrag | **teilweise** | V1 render_pc.c (Abschnitt 6) |

## 8. Maengel (nummeriert, nachpruefbar)

**M1 (G-E1 "Tod im Bild", Pflicht-Verlauf Plattform) - im Plattform-Verlauf Leon bleibt der Tod groesstenteils unsichtbar.** Lauf plat_leon
(Rezept 2, `RE15_PLAYER_POS=0,-16000,3072`, Haken HP; zweimal gefahren, deterministisch): gator_boss.log `RE2 R1 start pos=(5722,-7102)`,
`KAMERA R1 G-E1 laufender Cut 4: kopf_bild=(128,99) Start 8/8 ..., Todes-Clip 6/19 Proben F1278` -> `KAMERA R1 Sperre ohne Schnitt ... (G-E1:
laufender Cut ist die Wahl, Start 8/8, Kopf ok, Kiefer ok, Todes-Clip 6/19 Proben)`. Bilder `<SP>/plat_leon_r1_f1276-1318.png`,
`<SP>/plat_leon_r1_f1278-1316_x3.png`: ab F1290 liegt der Kopf hinter dem Maschinenblock, sichtbar ist nur noch die Fontaene. Ursache im Code:
gr2_r1_wahl bewertet (1) gueltigen Start VOR (2) sichtbaren Proben, und Stufe 0 sperrt Cut 3 (Start 8/8, Todes-Clip 19/19) wegen Spur B's
Massstab-Grenze (2158 > 1892), die G-E1 nicht verlangt. Das Dossier (10.3: "In allen Verlaeufen bleiben Kopf und Kiefer im Bild ... in jeder
Probe des Todes-Clips") trifft fuer diesen Verlauf nicht zu; der Riegel kampf faehrt keine Plattform-Lage (er wuerde mit seiner eigenen
Bedingung "Todes-Clip n/n" hier rot). Forderung: Wahl so, dass der Todes-Clip im Bild bleibt (Belege fuer jede Abweichung von G-E1), Messung
Plattform Leon + Elza und die uebrigen Pflicht-Verlaeufe, Riegel um einen Plattform-Lauf ergaenzen.

**M2 (Nebenwirkung ausserhalb 2090/2091) - die Kern-Zeilen 485..498 werden nach dem ersten 2090-Besuch in JEDEM Raum gezeichnet.**
gator_fx_r41_pc.c `re15_pc_gator_fx_draw`: der kern485-Zweig prueft weder Raum noch `s_reg`, nur `s_kern_tex_ok`, das `laden()` erst beim
ersten Betreten von 2090/2091 setzt und nie zuruecknimmt. Gemessen (Abschnitt 5): derselbe Granatwerfer-Schuss in ROOM2080, gleiche Lage/Gier/
Kamera/Spawns - ohne vorherigen 2090-Besuch `kern485 6 ... gezeichnet G 0`, nach dem Leiterweg aus 2090 `gezeichnet G 6 (kern485 6 ...)`;
Bild `<SP>/gl_2080_ab_direkt_oben_nach2090_unten.png`: Muendungsblitz (0x01002000, Kernbank 1 Zeile 492) und gluehende Granatkoerner
(0x020c0a00, Zeile 495) erscheinen nur nach dem 2090-Besuch. Damit haengt das Aussehen der Runde-35-Werfer (fremde Spur) vom Spielverlauf
dieser Sitzung ab (nach Laden eines Spielstands wieder anders); re2fx_pc.c laesst diese Zeilen bewusst aus (N5). Nicht im Dossier, kein
Riegel. Forderung: Zeichnung der Kern-Zeilen 485..498 auf die R1-Folge in 2090/2091 beschraenken - oder per Orchestrator-Entscheid spielweit
von Anfang an gleich laden und die Werfer-Effekte messen (Android-Texturhoehe beachten); Riegel: ein anderer Raum zeichnet mit und ohne
vorherigen 2090-Besuch dieselben Quads.

**M3 (Nachtrag 28 "der Explosionseffekt vom Gaskanister vom Original", natuerlicher Verlauf) - im Leon-Verlauf ohne Testhaken fehlt die
grau-weisse Wolke der Explosion vollstaendig.** Lauf v1_leon (37 echte Treffer, kein G-Haken): `KAMERA R1 G-E1 laufender Cut 1: ... Start 2/8
(0 verdeckt, 6 ausserhalb)`, in allen 38 R1-Bildern `re2fx_pc 0` (Spritzring, Kernbank 3 Sub 0x0f), Bilder `<SP>/v1_leon_r1_f440-478.png` /
`<SP>/v1_leon_r1_f444-458_x4.png` (nur Blut, Glut, Brocken, Scherben, Fontaene). Ursache gemessen: `Kanister Stufe 13 pos=(5996,-2041,-29290)`
- der Kanister steckt beim R1 hinter der Suedwand AUSSERHALB DES RAUMS (alle 13 Cuts von ROOM2090 haben dasselbe Regions-Viereck x -11000..9500,
z -29000..-3500), deshalb verwirft FUN_8002c820 (@0x80077a30) jeden Ring-Sprite und keine Kamera kann ihn zeigen. Teilweise auch west_leon
(5 von 6 ausserhalb, re2fx_pc max 16), v1_elza / plat_elza (3), k_elza (2). Das Dossier (10.3, 10.6.2) nennt das "wie im Original verworfen" -
der Verwurf-Mechanismus ist RE2s, die Lage aber nicht: dass der Kanister ausserhalb des Raums steckt, folgt aus der Port-Wandklemme
(re15_gator_boss_tick (2): re15_collision_constrain_enemy klemmt nur die Wurzel e->x/e->z mit hit_radius_min, der Kopf des RE2-grossen Modells
ragt darueber hinaus), fuer RE2 ist das nicht belegt. Forderung: Ursache (Kopf/Kanister dringt in die Wand) belegt beheben - Ort der Behebung per Orchestrator (Spur B Wandklemme/
Kopf oder Spur G) -, Messung v1 Leon/Elza und west_leon: Ring-Sprites im R1 gezeichnet.

**V1 (Vertrag) - render_pc.c ist fuer Spur G nicht freigegeben.** `RE15_TIM_SLOT_MAX 56 -> 57` (1 Zeile + Kommentar, markiert). VERTRAG 1.3 nennt
render_pc.c nur fuer D, Nachtrag 28 erlaubt Haken in B's Modulen und re2_fx.c. Klein und markiert - Orchestrator-Entscheid noetig (wie B5/J-E1).

## 9. Hinweise (kein Mangel dieser Abnahme bzw. klein)
- H1 Apply-Bild bei jedem Schnitt (Zweig ohne Spur E's kamerahalt_r41.c): F530 Cut 5 mit Zeile 42 (2.5), F2290 = R1 noch in Cut 2 mit der
  Explosion gross im Vordergrund (2.2). Bei der Integration mit Spur E nachmessen.
- H2 Spinnen 0x25 stehen in diesem Zweig noch in 2090 (Spur F) und sind in den R1-Bildern zu sehen (k_leon_r1); der Riegel kampf zielt auf sie.
- H3 Elza-Standardlage (k_elza) und plat_elza: G-E1 waehlt Cut 7 = Fernblick (Massstab 455/447 gegen RE2 1154); Kopf, Kiefer und Explosion
  sind im Bild, bei 1x aber klein und dunkel (`<SP>/k_elza_r1_f564-612.png`). G-E1 verlangt keinen Massstab - fuer den Nutzer erwaehnen.
- H4 Querbezug Nachtrag 29 (Spur F, Leiche/Kette weiter weg von der Leiter = Startlage des Kampfes): die R1-Lagen und damit die G-E1-Wahl,
  M1 und M3 aendern sich mit Fs neuer Startlage; nach der Integration F+G alle Verlaeufe (Abschnitt 2.7) neu messen. Gs Haken in
  enemy_ai_boss_gator.c wirkt nur in GBP_RE2_TOD.
- H5 re15_pc_gator_fx_draw ruft re2fx_quads() in jedem Raum ein drittes Mal je Bild (neben re2fx_pc und glas1120) - Rechenzeit, kein Fehler.
- H6 Aufraeumen: Messkopien re15_pc_g41abn3{,b..f}.exe werden nach der Abnahme geloescht; Laufordner/Bilder nur im scratchpad g3abn; im Baum
  nur dieser Bericht.
