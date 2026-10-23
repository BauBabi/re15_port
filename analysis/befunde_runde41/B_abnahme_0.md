# Runde 41 — Spur B "re2gator" — Abnahme 0 (unabhängig)

Prüfer: Abnahme-0-Agent, 2026-10-23 (22:3x–23:xx). Baum `.claude/worktrees/r41_re2gator`, HEAD c2404c61
(Zweig r41/re2gator, Basis master 116e8b0f). Geprüft wurde NUR der gebaute Stand; nichts aus dem Dossier
wurde ungemessen übernommen. Alle Messdateien liegen im Prüfer-Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn0/`
(Läufe `lauf_A`, `lauf_K`, `lauf_C`, `lauf_D1`, `lauf_D2`, Bilder `png/`, Helfer `lauf.sh`, `statelog.py`,
`schuettel.py`, `ppm2png.py`, `probe_clips.c`).

## 0. Ergebnis in einem Satz

Die RE2-Mechanik ist sauber belegt (13 selbst disassemblierte Adressen stimmen), gebaut (784 Schritte, 0 neue
Warnungen) und in den Riegeln gemessen (87/87 unit); am gebauten Spiel laufen Kanister-Aufnahme → Schuss →
Explosionstod mit geplatztem Kiefer (Punkt 3) und das Klettern über die Plattform. **Nicht erfüllt im Spiel**
ist der Kern von Punkt 2: im normalen Ablauf (Leon aus der Türnische, Gator vom Lauerplatz) zündet weder der
RE2-Zonen-Dash (Clip 2→3 mit SE 4) noch der RE2-Intro (Clip 6→3) noch der daran gehängte Putz — gemessen in
zwei exe-Läufen (0 "RE2 DASH/INTRO"-Zeilen) und nachgerechnet (Bahn-Probe `gb_bahn_frei` scheitert im Westkanal
systematisch). Die Dossier-Erklärung dafür ("Zonenlinie nicht überschritten") ist falsch. → `bestanden = false`.

## 1. Was geprüft wurde (Befehle)

- Lektüre: AUFTRAG.md, VERTRAG.md, B_re2gator.md (482 Zeilen), `git log --oneline master..HEAD` (9 Commits),
  `git diff master --stat` (42 Dateien, +2665/−198), kompletter Code-Diff (`enemy_ai_boss_gator.c`,
  `enemy_ai_re2_zombie.c`, `re2_fx.c`, `main.c`, `gator_re2_r41.c` 842 Z., `re15_gator_re2_r41.h`,
  `gator_re2_pc.c`, Tests, Tool).
- Bau: `bash re15_port/tools/local_build.sh configure` + `build` → "ninja: no work to do" (exe vom Bau-Agenten
  22:03). Danach alle 8 Spur-B-Quellen per `touch` neu übersetzt: `build` → 784 Schritte, `LOCAL-BUILD-OK`,
  EXIT 0, 3 Warnungen = main.c 8803/10132/10512, per `git blame` aus 2026-06-28 / 2026-08-18 (Altbestand, nicht
  Spur B). Keine Warnung in Spur-B-Dateien.
- Tests gezielt (keine volle Suite):
  `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|sca_wall_hit|re2_gore|re2_gore_render|re2_gore_fly|re2_gore_decoder|re15_re2z_import|re15_re2z_gore_se|re2z_.*|re2_hit_repeat_crow_spider|re2_crow_.*|crow_ai|crow_fixes|crow_death_corpse|r36_schuss|r36_schuss_kraehe|r36_kleidung_kraehe|r35_fenster_kraehe|r37_re2zombie_.*|r34_re2fx.*|montage_fx|r35_cut1150_montage.*|r38_montage.*|r38_wasser.*|r38_wasserzombie|r39_heliport1171_kraehen|r30_granate|r35_granate|r36_granate)$|^probe_(re2z_stomp|crow_kill)$"`
  → **100% tests passed, 0 tests failed out of 87** (45,8 s).
  `ctest -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate)$"` → Ergebnis in §6.
- exe-Läufe (eigene exe-Kopien `re15_pc_abn0_<Name>.exe`, eigene Arbeitsordner, `timeout`): Basis-Umgebung
  `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
  RE15_STATE_LOG=state.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=audio.raw RE15_DEBUG_JUMP=2090@250` plus je Lauf:

| Lauf | Zusatz | Zweck |
|---|---|---|
| A | `RE15_GB_TEST=1 RE15_EXIT_AT=460#2090 RE15_FRAMEDUMP=1-460/1:fd/a_` | natürlicher Ablauf: Leon in der Türnische (−8200,−25550), Gator vom Lauerplatz |
| K | `RE15_GB_TEST=5 RE15_PLAYER_POS=6500,-25000,3072 RE15_GATOR_KANISTER=4430,-26160 RE15_GIVE=3:15,21:50 RE15_EQUIP=3 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W2.0,(M0.3,MA0.1)x10 RE15_FORCE_CUT=1 RE15_EXIT_AT=260#2090 RE15_FRAMEDUMP=40-200/2:fd/k_` | Kanister-Aufnahme → Pistolenschuss beim Tragen → R1 |
| C | `RE15_GB_TEST=2 RE15_EXIT_AT=460#2090` | erzwungener Plattform-Bogen (Klettern bleibt?) |
| D1 | `RE15_GB_TEST=1 RE15_PLAYER_POS=-7300,-26000,3072 RE15_EXIT_AT=460#2090 RE15_FRAMEDUMP=1-460/1:fd/d_` | Leon exakt südlich im Westkanal: Dash-Bahn gerade nach Süden |
| D2 | `RE15_GB_TEST=5 RE15_PLAYER_POS=-7300,-7000,1024 RE15_EXIT_AT=1500#2090 RE15_FRAMEDUMP=900-1500/2:fd/e_` | Gator parkt SO (4997,−20318), Leon NW: Verfolgung nach NORDEN (Dash-Bahn frei?) |

- RE-Stichproben: `re2_disasm.py dis … --bin EM23_OVL_0000.BIN` bzw. RE2-`PSX.EXE` (§4).

## 2. Urteile je Nutzer-Punkt (Wortlaut AUFTRAG.md)

### Punkt 1 — "Finisher und fehlende Animationen aus Resident Evil 2 extrahieren" → **teilweise**

Messung (Lauf A, state.log Slot 20 t=23, identisch mit Dossier-Lauf A2 = deterministisch):

| Bilder | Clip | Was | RE2-Beleg (selbst disassembliert) |
|---|---|---|---|
| F1–F6 | 0 | INIT/LURK (Teleport (−7200,−9200), Yaw 1089) | — |
| F7–F79 | 1 | Paddeln (z −9406 → −21078, Mittel 157,7/Bild) | Sub 2 `addiu a1,zero,1 / addiu a2,zero,4 / jal 0x8001a330` @0x80100cf8–d00 |
| F80–F122 | 4 | Schnapp (Nutzer-Design LUNGE), Treffer F87 Leon 80→30 | — (Port-Biss) |
| F123–F156 | 0 | Schwimmen = Biss-Sperre 70 | `addiu v0,zero,70 / sb v0,568(a0)` @0x801016b0–b4 |
| F157–F186 | 1 | Paddeln | |
| F187–F211 | 4 | Schnapp, F197 Leon 30→−20 → FRESSEN | |
| F212–F331 | 5 | FRESSEN P2 (120 Bilder) | `addiu a1,zero,5` @0x80101180 (+ Opfer-Ruf `jal 0x80015cb8` @0x80101174) |
| F332–F451 | 11 | FRESSEN P3 Kau-Schleife (120 Bilder) | `addiu a1,zero,11 / jal 0x8001a240` @0x801011d8–e0 |

SE (debug.log `[re2se]`): se=2 Bank 17 (Frame-Flag der Clips 0/1) mehrfach, se=4 Bank 17 am Schnapp-Start
(Z. 467, 610), se=1/3 Bank 17 im Fressen. Der Finisher (Clip 4 → 5 → 11) ist der RE2-FRESSEN-Pfad (Clip 4
`addiu a1,zero,4` @0x801010ec, Frame-13-Gate `lbu v1,333 / addiu v0,13` @0x801010fc–1100) — er bestand schon
vor dieser Runde (Runde 6/16) und wurde nicht verändert (unit_gator_fress grün).

Clip-Zensus EM23 (alle `jal 0x8001a330/0x8001a240`-Rufe des Overlays, selbst gelistet): Clips 0,1,2,3,4,5,6,7,8,9,
10,11. Port vorher: 0/4/5/7/10/11. Port jetzt zusätzlich: 1 (Paddeln, gemessen im Spiel), 2/3 (Dash), 6 (Intro),
9 (R1, gemessen im Spiel). Nicht genutzt: Clip 8 (= R3 @0x80102ac8, Beschuss-Tod bei offenem Maul; vom
Orchestrator-Entscheid B2 "Tod nur über den Kanister" gedeckt).

**Warum nur teilweise:** Die extrahierten Clips 2/3/6 sind im Spiel aus der Türnische unerreichbar (s. Punkt 2,
Mangel M1); am gebauten Stand zeigt der normale Ablauf von den "fehlenden Animationen" nur Clip 1 — und Clip 9
nur über den Kanister-Testhaken.

### Punkt 2 — Verfolgung wie RE2 (Animation, Schreie, an gleichen Stellen beschleunigt), Wände wackeln + Putz, Klettern bleibt → **teilweise** (Dash/Putz im Spiel nicht erreichbar)

a) Animation + Tempo (erfüllt, gemessen): Paddeln Clip 1 mit authored Wurzelversatz. Meine Probe
`probe_clips.exe` (gegen `libre15_engine.a`, EM23-Bank aus CDEMD0.EMS): Clip 1 = 26 Bilder, Summe 6109 →
Mittel 235,0/Bild roh, ×2731/4096 = 156,7/Bild; gemessen im Lauf A: mo=1 Mittel 157,7/Bild (n=103, max 258 =
Bild 3 der Schleife 387×2/3 = 258 ✓). Clip 0 = 163 Bilder, Mittel 37,4 roh = 24,9/Bild. Hysterese 0x65/0x5a
auf +0x1F0 = SquareRoot0(dx²+dz²) in Welteinheiten (`lw v0,56 / lw v1,56 / subu / mult … jal 0x8008d2f4 /
sw v0,496(s0)` @0x800265a4–e0; 0x8008d2f4 beginnt mit `mtc2/mfc2 LZCS/LZCR` = GTE-SquareRoot0) → im Becken
praktisch immer Paddeln außer in der Biss-Sperre; das IST RE2 (Orchestrator B1).

b) Schreie (erfüllt): SE 4 Bank 17 am Angriffs-Start (Lauf A Z. 467/610; RE2 `addiu a0,zero,4` @0x80100d64 +
`jal 0x8005bd6c` @0x80100d84), Frame-Flag-SEs 2 der Schwimm-/Paddel-Clips, 0x48-Wörter → SE 4 mit 55er-Sperre
(`lui v1,0x4800 … addiu v0,zero,55 / sb v0,562` @0x80100424–34).

c) "An gleichen Stellen beschleunigt" = RE2-LUNGE als Zonen-Dash (Clip 2 Blend 16 @0x80100dac–b4 → Clip 3
Blend 0 @0x80100e20–28, +0x225=1 @0x80100e04–0c, SE 4): **im Spiel NICHT gemessen**.
- Lauf A (Türnische): gator_boss.log enthält KEINE Zeile `RE2 DASH start` / `RE2 INTRO start` (grep "^RE2 " = 0
  Treffer). Der Gator kreuzte dabei die Zonen 1 → 2 → 0 (z −14990 @F40 … −18020 @F61 → −18278 @F62), Leon stand
  in Zone 0: die Vorbedingung `re15_gr2_dash_zonenwechsel` (neue Zone == Leons Zone) war erfüllt. Die
  Dossier-Erklärung "Leon und Gator blieben in derselben Beckenzone, die Zonenlinie wurde nicht überschritten"
  ist falsch.
- Ursache nachgerechnet: Bahn-Probe `gb_bahn_frei` (enemy_ai_boss_gator.c) verlangt Ende-x ≥ −8900+1600 = −7300.
  Clip-2-Netto (Probe) = (3535, 0, −29) roh → (2356, 0, −20) ×2/3; Clip-3-Netto = (2884, 0, −4045) → (1922, 0,
  −2697): Clip 3 trägt die RE2-Korridor-Ecke als seitlichen Wurzelversatz von ~2700 Einheiten. Beim Kreuzen
  (Pos (−7289,−18278), Yaw 1027 ≈ Süd) ergibt `gr2_rig_welt` (x' = e.x + c·vx + s·vz) für Clip 2 x = −7289−20 =
  **−7309 < −7300** (scheitert um 9 Einheiten), für Clip 3 x ≈ −9986 (in der Westwand). Die CHASE-Route läuft im
  Westkanal konstant bei x ≈ −7290…−7300 (state.log F40–F79), d.h. der Dash kann in Richtung Süden dort NIE
  zünden; der Intro vom Lauerplatz (−7200,−9200) ebenso nicht (Clip 6 Netto 9498 → 6332 ×2/3 mit Yaw 1089 →
  Ende-x ≈ −7830 < −7300).
- Lauf D1 (Leon exakt südlich, Bahn gerade nach Süden): ebenfalls 0 RE2-Zeilen (vorhergesagt: −7320 < −7300).
- Lauf D2 (Verfolgung nach Norden): s. §6 (Nachtrag).
- Der Riegel unit_r41_re2gator_dash misst den Dash in einer Geometrie (Gator (−6500,−22000) nach Norden), die im
  Spiel nur eintritt, wenn Leon zuerst an der Plattform vorbei nach Norden gelaufen ist und der Gator von Süden
  folgt.

d) Wände wackeln (erfüllt, gemessen): Framedump Lauf A, Referenz Bild 30, Region Zeilen 10–110 (Wand/Rohre),
Versatzmodell `frame[y][x] = ref[y+sy][x+sx]`: Hintergrund-Versatz in Bildern 33,34,37,39,42,43,45,49,50,53,55,
58,59,61,65,66,69,71,74,75,77,81,82,85,87,90 = (0,1)/(1,1) mit Periode 16 exakt an den Tabellenplätzen
0/3/4/6/10/11/14 der Modus-1-Tabelle @0x8009daf4 (`00 01 | 00 00 | 00 00 | 00 01 | 01 01 | 00 00 | 00 01 | …`,
selbst gelesen); starker Modus 2 (0,2) in Bildern 176/178 (Clip 1 f12 → Ereignis → Bit 4 nach +3). Während des
Paddelns kommt alle 13 Bilder ein 0x28-Wort (f12/f25), die Ereignis-Maschine (@0x80101d44, Zähler 2/5/10, Bit-8-
Vorrang `andi v0,v1,0x8` @0x8002aa34) hält Bit 8 dann dauerhaft — ein fortlaufendes 1-px-Zittern, so wie der
RE2-Code es rechnet. "Ab und an" stark wackelt es nur bei ≥ 18 Bildern Abstand (Schwimm-Clip 0, Dash). 3D bleibt
stehen (nur `re15_bg_blit(-dx,-dy)`), wie DR_MOVE in FUN_8002ac94.

e) Putz fällt (**nicht gemessen im Spiel**): Spawn hängt am Dash-Start (`g->putz_t = 1` in
`re15_gr2_dash_start`, PORT-WAHL) — ohne Dash kein Putz (Läufe A, D1: 0 Dashes). Nur der Riegel unit_r41_re2gator_putz
(54 Spawns, Op 0/11/73 ohne unbekannte Ops) belegt die Mechanik; die exe hat keine Messzeile dafür.

f) Klettern über die Plattform bleibt (erfüllt, gemessen): Lauf C (GB_TEST=2): gator_boss.log
`CROSS start=(4450,-11900) ende=(3350,-20700) frames=240 fenster=62..177`; state.log F1–F246 mo=0 von Nord nach
Süd über die Rampe, danach F247– Paddeln/Schnapp/Fressen wie Lauf A. Lauf D2 prüft die Routenwahl ohne Zwang (§6).

### Punkt 3 — Kanister aufnehmen wie RE2, beim Tragen beschießbar, Sterbeanimation mit geplatztem Kiefer wie RE2 → **erfüllt** (Mechanik mit Testhaken)

Lauf K (Rezept Dossier §7, bitgleich mit dem Bau-Lauf B2/B3):

| Bild | Messung | RE2-Beleg (selbst disassembliert) |
|---|---|---|
| F2 | `RE2 KANISTER gesetzt (4430,-1200,-26160)` | Testhaken (Vorgabe "Mechanik mit Testhaken") |
| F7 | `Stufe 1 laengs=5868 quer=17` | Fenster `addiu v0,v0,-8601 / sltiu v0,v0,0x18f` @0x80100ae4–ec (×2/3: 5734..6000) |
| F20 | `angehaengt lokal=(4211,1178,5)` an Part 6 | @0x801018d0–60 (Tabelle T2 @0x80100024: 0x8010183c/1860/1970/19dc/1a64/1aec/1b58 selbst gelesen) |
| F47 | `getragen (Stufe 7)`; Zeichner-Zeilen Stufe 1→7 mit Kasten/Tiefe | |
| F70 | Pistolentreffer: HP 25300→25293; `R1 start pos=(4997,-20318)`; `[re2se] se=4 bank=17`; Kanister Stufe 13 | HURT-Selektor `lbu v1,550 / bne v1,v0(7)` @0x80101ef4–fc → R1; R1 P0 `jal 0x80104318 / addiu a1,s3,1204` @0x80102354–58, `lui a0,0x20f / ori a0,a0,0x1 / jal 0x8005ba28` @0x8010237c–88, `addiu v0,zero,9 / sb v0,363` @0x80102390–94, `addiu v1,v1,160 / sw v1,0x801048bc` @0x8010239c–a4 |
| F70–F260 | Clip 9 (af 1..36, 37 Bilder), Yaw 1164→1249, Pos (4997,−20318)→(3413,−25001), Leiche hält af=36 | `addiu a1,zero,9` @0x8010248c |
| Bild k_076 (Cut 1) | dunkelrote Scherbenwolke + heller Spritzring am Kopf (geplatzter Kiefer) | FUN_80104318 @0x80104318–58: `ori v0,v0,0x4a / sw v0,0(a1)`, `ori v0,zero,0x8000 / sh v0,160(a1)`, `addiu v0,zero,-10 / sh v0,154(a1)`, `lui a2,0x20 / ori a2,a2,0x2038 / sw a2,112(a1)`, `lhu v1,118(a0) / addiu v1,v1,2048 / sh v1,152(a1)` — alle 17 Instruktionen wie im Dossier §2.6; s3 = `lw s3,408(s1)` @0x80102174 = Modellblock |
| k_160 | Leiche (Clip-9-Endpose) | P3 @0x80102654 |
| Framedump 72–78 (Cut 1, Region Zeilen 10–60) | Hintergrund-Versatz (0,2)/(1,0)/(0,2)/(0,2) = Modus-2-Tabelle @0x8009db14 (starkes Schütteln), danach 80–86 (0,1)/(1,1) = Modus 1 | R1 P0 `0x800cfbd8 \|= 4` @0x801022a8–b4 + Ereignis 1 @0x801022d0–d4 |

Unit-Riegel dazu: unit_r41_re2gator_kanister (Stufen 13/6/6/7/7, Ease-Summen, R1: SE 4, Raum-Satz 15, Rumble,
Clip 9, Kanister 10 Bilder, Leiche, Unverwundbarkeit) und unit_r41_re2gator_kiefer (Part-Record 7: 0x4B,
0x202038, 0x8000, −10, yaw+0x800; 30 Scherbenbilder, Skala 180..2790, weg ab 31) — beide grün (eigener Lauf).
Einschränkungen (keine Mängel der Spur, aber zu wissen): Raum-Brüller Satz 15 (GATOR40A0.VBS) ist in der exe
nicht nachweisbar (kein Logpfad; nur Riegel). R1 ist auch aus LURK/CHASE auslösbar (RE2 schließt +0x230==1 aus,
@0x80101f04–0c) — PORT-WAHL nach Nutzer-Wortlaut, im Code so kommentiert. Zielhilfe trifft nur den NÄCHSTEN
Gegner (Spinnen → Spur G).

## 3. Gates

### 3.1 RE-Gate (Konstanten mit @0x / NUTZER-VORGABE / PORT-WAHL)
- Diff-Grep nach "deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME": **0 Treffer**. Env-Schalter
  nur `RE15_GB_STUMM` (Log aus, bestehendes Muster) und `RE15_GATOR_KANISTER` (der geforderte Testhaken).
- 13 Adressen SELBST disassembliert, alle stimmen mit dem Behaupteten überein:
  FUN_80104318 (Kiefer-Stempel, 17 Instr.), R1 P0 @0x80102340–ac (Kiefer-Ruf, Se_on 0x020f0001, +0x16B=9,
  Ziel-Yaw +160), `lw s3,408(s1)` @0x80102174, Hysterese `sltiu 0x65` @0x80100b88/ba4 + `sltiu 0x5a`
  @0x80100be4/ca4 + +0x238 @0x80100b5c/bf4/cb4, EXE-Distanz @0x800265a4–e0 (+ 0x8008d2f4 = SquareRoot0),
  INIT HP `slti`-freie Tabelle @0x80100518–580 ([0x801045bc]=0x012c=300, [0x801045be]=300 → 25300; bei Bit 0x40
  von 0x800cfb74 würden [0x801045ec/ee] gelten — Dossier nennt "Bit 0x40 aus" ohne Beleg, beide 0x20-Zweige geben
  aber 300), LUNGE Sub 3 @0x80100d30–e50 (SE 4, Clip 2 Blend 16, +0x225=1, Clip 3 Blend 0), HURT-Selektor
  @0x80101eac–fe8 (25000, +0x226==7, +0x230==1-Ausnahme, Maul 0x10 & [0x800d4820]==8, rnd&3, +0x22E=rnd&0x3f+120),
  Root-Frame-Wort @0x801003f0–40c (0xf8000000/0x2800 → +0x227=1) + 0x4800 → 55 @0x80100424–34, Ereignis-Maschine
  @0x80101d44–e98 (Rampen (10,120,50)/(8,85,40), Zähler 2/5/10, Bits 4/8), FRESSEN-Clips 4/5/11, Biss-Sperre 70
  @0x801016b0–b4, Intro-Sperre 110 + `yaw += 1024` @0x8010091c–48, EXE-Schütteln FUN_8002aa28/aa6c (Bit-8-Vorrang,
  Zähler 0x800dfd6b &0xf) + beide 32-Byte-Tabellen @0x8009daf4/@0x8009db14 byteidentisch mit `k_schuettel1/2`.
- ROOM40A0.RDT selbst gelesen: Header-Offsets (EDT 0x9DE0/VH 0x9EA0/VB 0xACC0, SCD-main 0x25C0), sub14 @main+0x086E
  = `3a 00 04 10 00 00 00 0e 78 a1 54 f2 14 05 38 03` ✓, sub13 `0d 00 04 00 06 00 18 0e` (For 6 / Gosub 14) ✓,
  Sce_em_set @0x25A0 ✓, Obj_model_set `2d 01 …` @main+0x0452 ✓, Pos_set/Dir_set `32 00 e0 a6 40 f8 e4 da 33 00
  90 06 00 00 00 fc` @main+0x0484 ✓ (Dir 0x690/0/0xfc00).
- PORT-WAHLEN sind als solche gekennzeichnet (Zonenlinien = RDT-Wasserzonen, Bahn-Probe, kein `yaw += 0x400`,
  Putz am Dash-Start, Aufnahme längs der Blickachse, R1 aus jedem Sub, Kanister-Höhe −1200). Sie sind belegt,
  aber eine davon (Bahn-Probe + Westkanal) macht den Dash im Spiel unerreichbar (M1).
- "Erklärt der Fix den Befund?": Punkt 3 ja (gemessen), Punkt 2c/e nein (im Spiel nicht eingetreten).

### 3.2 Vertrag (VERTRAG.md)
- Bank-9-Bits / Nachrichten-Ids: keine belegt (Diff-Grep leer) ✓. Verbotene Pfade (release/, platform/android/,
  shared_assets/PSX/, pri/, synchro/, Test-CMakeLists): keine ✓; neue RE2-Daten unter `shared_assets/RE2/`
  (KANISTER40A0.MD1/.TIM, GATOR40A0.VBS) im Dossier §7 genannt ✓.
- Gemeinsame Dateien: `enemy_ai_boss_gator.c` (Eigentümer B) ✓; **aber** B änderte drei Dateien, die ihr in
  §1.3 nicht zugeteilt sind: `platform/pc/main.c` (3 Haken, je 1–3 Zeilen, "Runde 41 Spur B"; Datei gehört D/F/G),
  `enemy_ai_re2_zombie.c` (4-Zeilen-Haken in `re15_re2z_gore_active`; Datei gehört I/G), `re2_fx.c` (+40 Zeilen
  Op 11/73 am Dateiende; nicht gelistet). Alles klein und markiert — bei der Integration mit D (main.c
  Lautstärke) und I/G (re2_zombie) zu beachten (M3).
- Tests je Punkt vorhanden und messend (8 unit + 1 integration), parallel-fest (Zufalls-Id, exe-Kopie
  `re15_pc_r41b_*`), Linux-fest (`cmake_minimum_required`, `m`, `_putenv`/`setenv`, keine Backslash-Pfade) ✓.
  Hinweis: der Integrationsriegel löscht beim Start ALLE `re15_pc_r41b_*.exe` im Bauordner (M5).
- Beide Szenarien: der Boss ist an Raum 0x2090 gebunden (ROOM2091.RDT existiert, der Boss-Code kennt 0x2091 nicht —
  unverändert gegenüber master, keine neue Nebenwirkung). Nachbarräume: unit r38_wasser*/r38_wasserzombie,
  r35/r36/r30-Granaten grün; Integrationsriegel s. §6.

## 4. Nebenbefunde (keine Mängel)
- Lauf A ist bitgleich mit dem Bau-Lauf A2 (gleiche Bild-/Clip-Grenzen F7/F80/F87/F123/F157/F187/F197/F212/F332)
  → deterministisch.
- `GBP_DIE` (Clip 7 = RE2 R2-Clip, `addiu a1,zero,7` @0x801027e0) bleibt für `state==3` erreichbar: mit
  HP 25300 nur über Explosionsschaden (Granate 1000/Treffer → 26 Granaten) — ein Nicht-RE2-Tod, den B2 nicht
  ausdrücklich behandelt.
- Kanister im Becken dunkel (RE2-Textur 41/41/41) — Spur G wählt Kamera/Lage; im Lauf K bei Cut 1 ist der
  Scherbenregen klar sichtbar (png/k_076.png).

## 5. Mängel (nummeriert, nachprüfbar)

1. **M1 (Punkt 2c, wesentlich): RE2-Dash (Clip 2→3, SE 4) und RE2-Intro (Clip 6→3) zünden im normalen
   Spielablauf nicht.** Nachprüfung: Lauf A/D1 wie §1 → `grep "^RE2 " gator_boss.log` = 0 Zeilen, obwohl der
   Gator die Zonenlinie z=−18100 bei (−7289,−18278) in Leons Zone kreuzt. Rechnung: `gb_bahn_frei` verlangt
   Ende-x ≥ −7300; Clip-2-Ende −7309, Clip-3-Ende ≈ −9986 (Seitversatz −2697 aus der RE2-Korridor-Ecke). Beheben
   heißt: Bahn-Probe/Route oder Dash-Geometrie so wählen, dass ein Zonenwechsel Richtung Leon im Westkanal
   zündet (z.B. Clip-3-Seitversatz um den Yaw so legen, dass er vom Rand weg zeigt, oder die Klemmlinie am
   Westkanal nicht auf die Clip-Bahn anwenden) — mit Beleg, nicht nach Gefühl.
2. **M2 (Punkt 2e): Putz im Spiel nicht belegt.** Hängt an M1; zusätzlich fehlt jede exe-Messzeile für die
   Putz-Spawns (`re15_gr2_putz_spawns` nur im Riegel). Nachprüfung: nach Behebung von M1 Framedump um
   "RE2 DASH start" + Zählerzeile.
3. **M3 (Vertrag §1.3): Änderungen an nicht zugeteilten gemeinsamen Dateien** (`main.c` 3 Haken,
   `enemy_ai_re2_zombie.c` 4 Zeilen, `re2_fx.c` +40 Zeilen). Klein und markiert; bei der Integration Konflikte mit
   D/I/G prüfen (Orchestrator-Entscheid, kein Code-Fehler).
4. **M4 (Dossier §4.3): falsche Erklärung** "Leon und Gator blieben in derselben Beckenzone, die Zonenlinie
   wurde nicht überschritten" — state.log widerlegt das (Zonen 1→2→0). Dossier korrigieren, sonst sucht der
   nächste Bearbeiter an der falschen Stelle.
5. **M5 (klein, Test): `test_r41_re2gator.cmake` löscht `re15_pc_r41b_*.exe` global** im Bauordner — bei zwei
   gleichzeitig laufenden Instanzen desselben Riegels (zwei Suiten auf einem Bauordner) stirbt der Zwilling.
   Besser nur die eigene Kopie am Ende entfernen.
6. **M6 (Beleg-Lücke, klein): Raum-Brüller Satz 15 in der exe nicht messbar** (kein Logpfad im Raumbank-Haken) —
   nur unit_r41_re2gator_kanister pinnt ihn. Eine `[gator-re2] Raum-SE 15`-Zeile im Haken würde das schließen.

## 6. Nachträge (D2, Integrationsriegel)

### 6.1 Lauf D2 — Verfolgung nach Norden (Dash-Bahn frei?) → **nicht prüfbar** in dieser Konstellation
Aufbau: `RE15_GB_TEST=5` (Gator parkt bei (4997,−20318), Aggro erst nach 900 LURK-Bildern oder Treffer,
enemy_ai_boss_gator.c:771/1263), Leon per `RE15_PLAYER_POS=-7300,-7000,1024` im Nordwesten (Zone 1).
Ergebnis: EXIT=124 (timeout 300 s), 6309 state.log-Zeilen; der Gator blieb F1–F915 in LURK (mo=0), Leon wurde
von den beiden Spinnen gefressen, bevor der Gator scharf wurde (HP 100 → 80 @F61 → 0 @F598 → −40 @F882, Game
Over → Neustart, Bildzähler auf F0). `grep "^RE2 " gator_boss.log` = 0. Mit den vorhandenen Haken ist eine
Verfolgung nach NORDEN (die einzige Richtung, in der die Bahn-Probe im Westkanal durchgeht: Yaw 3072 → Clip-3-
Seitversatz +2697 nach Osten, Ende x ≈ −4600 außerhalb der Inselmarge −3500) im Becken mit Spinnen nicht
herstellbar: Zielhilfe trifft die nächste Spinne, GB_TEST=5 wartet 900 Bilder. Der Dash bleibt damit NUR im
Riegel unit_r41_re2gator_dash gemessen (Gator (−6500,−22000) nach Norden, Leon (−6500,−8000)). Für den
Nutzer-Ablauf (Türnische, Gator kommt von Norden) gilt M1 unverändert.

Nebenbefund zur HP-Tabelle: [0x801045ec]/[0x801045ee] = 0x00f0 = 240 (bei gesetztem Bit 0x40 in 0x800cfb74 wäre
die RE2-HP 25240 statt 25300). Welcher Spielmodus Bit 0x40 setzt, ist im Dossier nicht belegt ("Bit 0x40 aus"
ist eine Annahme); für den Port-Sockel 25000 ohne Folgen.

### 6.2 Integrationsriegel
`ctest -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate)$" --timeout 900` (eigener Lauf am
neu gebauten Stand, 22:53–23:02): **100% tests passed, 0 tests failed out of 4** — integration_r35_granate
166,29 s, integration_r36_schuss 147,81 s, integration_r38_wasser 183,43 s, integration_r41_re2gator 22,33 s.
Keine roten Fenster-Haken, kein Nachfahren nötig. Gebauter Stand: re15_pc.exe 22:39:09, test_r41_re2gator.exe
22:39:32, probe_gator_sweep.exe 22:39:20 (mein touch+Neubau), alle Messungen danach.

## 7. Gesamturteil

`bestanden = false`. Punkt 3 erfüllt, Punkte 1 und 2 teilweise: die RE2-Bausteine sind belegt, gebaut und in den
Riegeln gemessen, aber der RE2-Dash/Intro samt Putz tritt im Spielablauf aus der Türnische nicht ein (M1/M2), und
das Dossier erklärt das falsch (M4). Dazu die Vertragsabweichung an drei nicht zugeteilten gemeinsamen Dateien
(M3, klein) und zwei Kleinigkeiten (M5, M6). Nach Behebung von M1 (mit Beleg) und M4 ist eine erneute Messung
des Dash/Putz im Spiel (Framedump um "RE2 DASH start") Pflicht.
