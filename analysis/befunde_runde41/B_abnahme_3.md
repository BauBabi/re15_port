# Runde 41 — Spur B "re2gator" — Abnahme 3 (unabhängig)

Prüfer: Abnahme-3-Agent, 2026-10-24 (ab 09:33). Baum `.claude/worktrees/r41_re2gator`, HEAD 47efb932
(Zweig r41/re2gator, merge-base mit master 116e8b0f; master selbst steht auf af7e0e8a — Nachträge 7-9 betreffen nur
Spur C). Geprüft wird NUR der gebaute Stand; nichts aus dem Dossier wird ungemessen übernommen. Messdateien:
Prüfer-Scratchpad `C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/B41abn3/`
(Läufe unter `mess/<Lauf>/`, eigene exe-Kopien `re15_pc_b41a3_<Lauf>.exe`).

(Fortlaufend geschrieben; Gesamturteil am Ende.)

## 1. Lektüre + Bau

- Gelesen: AUFTRAG.md (Wortlaut + Nachträge 4/6 + Entscheide B1–B9), VERTRAG.md, UEBERGABE.md, B_abnahme_2.md (Mängel
  M1–M3), Dossier B_re2gator.md §11 (Nachbesserung 3), `git log --oneline master..HEAD` (9 Code-/Doku-Commits seit
  Abnahme 2: 63d80825 … 47efb932, dazu Orchestrator 0a90a394/2fa4c43c), `git diff 0a90a394..HEAD --stat` (16 Dateien,
  +933/−31), kompletter Code-Diff der Nachbesserung 3 gelesen (enemy_ai_boss_gator.c +98, gator_re2_r41.c +91,
  gator_re2_pc.c +47, Header +22, main.c +1, probe_r34_reaktion.c +10, Tests).
- Bau: `bash re15_port/tools/local_build.sh configure` → `LOCAL-BUILD-OK (configure)` (09:35:41); alle Spur-B-Quellen
  per `touch` (nur mtime, 15 Dateien aus `git diff --name-only master...HEAD`), dann `local_build.sh build` → 798 Schritte,
  `LOCAL-BUILD-OK (build)`, EXIT 0 (09:35:52–09:36:24), re15_pc.exe 09:35. Einzige Warnungen: main.c 8803/10133/10513
  (`-Wdiscarded-qualifiers`, Altbestand wie Abnahme 0–2, um die neue Haken-Zeile verschoben), keine in Spur-B-Dateien.
- Gezielte Unit-Riegel (Regex des Bau-Agenten + `r36_kartenbits`): `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|
  gator_sweep|gator_fress|re2_spider_water_2090|…|r36_kartenbits|text_freeze|r34_reaktion|…|r36_kartenitems)$|^probe_(re2z_stomp|
  crow_kill)$|^r31_trefferboxen.*$" --timeout 600 -j 4` → **100% tests passed, 0 tests failed out of 108** (09:36:49–09:37:45).
  `ctest -R "^unit_r41_re2gator_" -V` → 14/14 (`B41abn3/unit_r41_verbose.txt`), u.a.:
  - lauer: `untergetaucht 123 Takte am Lauerplatz (-7200,-9200), Auftauchen Takt 123 auf der Intro-Lage (5085,-19894) -
    Pos_set-Sprung 16287 im untergetauchten Takt; Intro 274 Schritte, groesster 339 (Takt 151), Grenze 340`
  - schleppen: `Fall 1 (Clip 0): Gier fest 1, Gator-Weg 198, Kanister-Weg 0, Anhaengen 12 Takte nach dem Fenster` |
    `Fall 2 (+300 je Takt): … Kanister-Weg 2861 (laengs, quer-Abweichung 0)` | `Fall 3 (quer 1200): Lokallage beim Anhaengen
    (4199,1186,1783) -> nach Stufe 7 + 40 Takte (1750,-210,0)`
  - anschwimmen: 10000/12000 voraus gleiche Zone `Zielmodus 1, Biss-Start nie (40 Takte)`; 8000 voraus / hinter dem Gator /
    14000 im Westkanal / ohne Kanister `Zielmodus 0, Biss-Start nach 0`
  - kanister: `Stufen 12/6/6/7/7/82 … getragen ab 38`; aufnahme `Stufe 1..6 37/37 Clip 0, Biss 0`.

## 2. RE-Stichproben (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py --bin EM23_OVL_0000.BIN`)

EM23_OVL_0000.BIN: info/re2leon/COMMON/BIN = shared_assets/RE2 = analysis/re2_alligator_2026-09-09 (md5 0293e099…, gleich).
Alle zitierten Stellen der Nachbesserung 3 enthalten das Behauptete:
- **Sub 0 P0 @0x80100854-68** (`dis 0x80100840 32`): `lh v0,468(s0) / nop / beq v0,zero,0x80100950 / addiu v0,a0,1 /
  j 0x80100950 / sb v0,6(s0)` — kein `jal 0x8001a330` (keine Animation) in P0; P1 @0x8010086c: `ori v0,v0,0x110` auf
  0x800cfbd8, bei Bild 70 `jal 0x8002c7ac`, dann `addiu a1,zero,6 / … / jal 0x8001a330` @0x801008ac-b4 (Clip 6). ✓
- **Pre-Tafel @0x8010463c** (`table`): [0] 0x801007fc (= `jr ra / nop`, leer), [1]/[2] 0x80100964 (Sub-1/2-Pre). ✓
- **T2-Stufentafel @0x80100024**: [1] 0x8010183c, [2] 0x80101860, [3] 0x80101970 … [7] 0x80101b58. Stufe 1 @0x8010183c-5c
  `jal 0x8007730c` (a1 = 29) / `sh zero,346(s1)` / `lbu/addiu/sb v0,550(s1)` und fällt OHNE Sprung in 0x80101860. ✓
- **Stufe 2 @0x80101860-c0**: Ease 13 (240 / −320), `t++` (`sh a0,346(s1)`), Schub `lw v0,64(s2) / lw v1,64(s1) / subu /
  slti v0,v0,8100 / beq v0,zero,0x801018c4 / addiu v0,v1,8100 / sw v0,64(s2)` = nur obj[1].z; Anhängen bei
  `lh v1,346(s1) / addiu v0,zero,13 / bne` @0x801018c4-cc; Zielmodus aus `lbu v0,448(s1) / andi v0,v0,0xfb / … /
  sb v0,448(s1)` @0x801018e4-f4; Lokallage `sw v0,56(s2) / sw v1,60(s2) / sw a3,64(s2)` @0x80101910-18. ✓
- **Stufen 3–7**: x (+0x38) per `jal 0x8001a1e0` a1 = 1750, a2 = 70 @0x801019c4-cc / 145 @0x80101a38 / 100 @0x80101ac0 /
  100 @0x80101b68-70; y (+0x3C) a1 = −210, a2 = 25 @0x80101a4c-54 / 55 @0x80101adc / 30 @0x80101b84-8c; kein Zugriff auf
  `64(s2)` / `s2,64` (z) in 0x80101970–0x80101b90 (grep leer). ✓ (Der Quer-Ausgleich z → 0 ist damit wie angegeben
  KEIN RE2-Verhalten, sondern PORT-WAHL.)
- **Zielmodus @0x801009c8-ab4**: Zone 1 `lbu v1,538 / addiu v0,zero,1 / bne` @0x801009c8-d0, Zone-2-Linie `slti v0,v0,-7322`
  @0x801009e0, `lbu v0,556 / bne` @0x80100a0c-14, Bank 5 Bit 1 `jal 0x80077360` (a0 0x800d4874, a1 1) @0x80100a1c-28,
  Stufe < 2 `lbu v0,550 / sltiu v0,v0,0x2 / beq` @0x80100a34-40, `ori v0,v0,0x4 / … / sb v0,448(s0)` @0x80100a54-5c, Ziel
  `sh a0,368 / sh v1(y−800),370 / sh v0(obj[1].z),372` @0x80100a58-70; Löschen `lw v0,0x800d055c / lw v1,64(s0) / subu /
  addiu v0,v0,-9000 / slti v0,v0,-120 / beq` + Stufe 0 → `andi v0,v0,0xfb` @0x80100a74-b4. ✓
- **T1 @0x8010134c-7c**: `lbu v0,448(a0) / andi v0,v0,0x4 / bne v0,zero,0x80101380` (keine Schärfung bei Zielmodus), Stufe
  0/7 → `addiu v0,zero,1 / sb v0,548(a0)`. ✓
- **Root-Hals @0x80100330-74**: `lhu v0,536(s0) / andi 0x4 / beq` → bei `+0x1C0 & 4` `lh a1,118(s0) / jal 0x800177c0`, sonst
  `jal 0x80017d98` (je Bild nach dem Intro). ✓ — Die RE2-Halsführung ist damit belegt und im Port nicht vorhanden (§3.3 e).
- **ROOM40A0.RDT** (info/re2leon/PL0/RDT, selbst gelesen): Datei 0x25A0 = `44 00 00 23 00 00 01 16 00 00 4a 16 b8 0b 3c eb 00 04
  00 00 00 00` → Pos (5706, **3000**, −5316), Cdir 1024; SCD-Walker (re2_scd_walk.py, vorher gelesen) sub11 `Member_set2 351710`
  @0x0730, `Evt_next` @0x0734, `Pos_set 3200cd11f8f8c3b8` @0x0736 = (4557, −1800, −18237). ✓
- **Schüttel-Tafeln** (`bytes`, PSX.EXE): Modus 1 @0x8009daf4 = (0,1)(0,0)(0,0)(0,1)(1,1)(0,0)(0,1)(0,0)(0,0)(0,0)(0,1)(0,1)(0,0)
  (0,0)(1,1)(0,0); Modus 2 @0x8009db14 = (0,2)(0,0)(0,2)(0,3)(1,0)(0,1)(0,2)(0,0)(0,2)(0,0)(2,1)(0,2)(0,2)(0,0)(1,1)(0,2). ✓
- **Gier im RE2-Root @0x80100264-2c8**: `sh zero,116/118/120(s0)` @0x8010027c-88 → `jal 0x80035408` → Rückschreiben der
  gesicherten Winkel `sh v0,116/118(s0)`, `sh a0,120(s0)` @0x80100294-2c8 — reines Sichern/Rückschreiben um die Matrix-
  rechnung, keine Lenkung. Mit der Wortsuche der Abnahme 2 (Gier-Schreiber nur Root/Intro/Dash/R1–R3) heißt das: der RE2-Gator
  dreht seinen Körper in der Verfolgung (Sub 1/2) NIE; Leon wird nur mit dem Hals verfolgt (FUN_80017d98). Eigene Wortsuche über
  das ganze Overlay (sh imm 0x76, sw imm 0x74, sb/swl/swr 0x73–0x77): genau 8 Treffer @0x80100280/2a0/948/ec0/2404/2440/27c8/2a34;
  Sub-1/2-Pre (@0x80100964) und -Main (@0x80100c20 / @0x80100cdc, Tafel @0x8010465c [1]/[2]) rufen nur 0x801012fc, 0x80077360,
  0x8001a330. (Für §3.3 d.)
- **Anhängen = Eltern-Zeiger**: T2-Prolog `lw s0,408(s1)` (s0 = Part-Tafel +0x198) @0x801017dc, `s2 = 0x800d051c` (obj[1]) @0x8010180c-10;
  beim Anhängen `addiu a0,s0,1104` @0x801018d0 + `sw a0,128(s2)` @0x801018e0 (obj[1]+0x80 = Zeiger in Part-Satz 6, +0x450 =
  6 × 0xAC + 0x48), dann FUN_8002d030 (Lokalmatrix) @0x801018f0 — der Kanister ist ab da Kind der Part-6-Matrix (dieselbe
  +0x80-Verkettung nutzt der Root @0x80100248-4c/@0x801002b4-bc). (Für §3.4 d.)

## 3. exe-Läufe (eigene Kopien `re15_pc_b41a3_<Lauf>.exe`, Ordner `B41abn3/mess/<Lauf>/`)

Basis (`B41abn3/lauf.sh`): `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
SDL_ASSERT=always_ignore RE15_STATE_LOG=state.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=audio.raw`, `timeout`, Umgebung des
Aufrufers ohne RE15_GB_TEST/RE15_GATOR_KANISTER/RE15_PLAYER_POS/RE15_KEIN_GATOR/RE15_INPUT_SCRIPT/RE15_AUTOPILOT/
RE15_FRAMEDUMP/RE15_EXIT_AT/RE15_DEBUG_JUMP; Elza (`lauf_elza.sh`) mit `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1`. Auswertung `statelog.py` (Clip-Läufe t=23), `gier.py` (Gier-Sprünge je Bild), `schuettel.py`,
`vergleich.py` (Bild-Bytevergleich), `sheet.py` (Bildbögen; Aufhellung nur zur Sichtung). Alle Läufe rc=0.

| Lauf | Zusatz | Zweck |
|---|---|---|
| N / N0 | `RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=-8200,-25550,3072 RE15_R39_MESS_SPIELER_HP=100 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W2,U12,W40`, Framedump je Bild; N0 = dasselbe mit `RE15_KEIN_GATOR=1` | natürlicher Zugang ohne GB_TEST (B9) |
| A2 | `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1`, EXIT_AT 676 | Türnische: Intro, Biss, Finisher bis Game Over |
| D2 | `RE15_GB_TEST=1 RE15_PLAYER_POS=-4000,-24000,2048 RE15_R39_MESS_SPIELER_HP=100`, EXIT_AT 1100 | Dash, Schreie, Schütteln, Putz |
| C3 | `RE15_GB_TEST=1 RE15_PLAYER_POS=2500,-25000,2048` HP-Haken, `RE15_GATOR_KANISTER=-1500,-23400`, Autopilot Südbecken (Rezept Abnahme 2), EXIT_AT 1400 | Klettern, Kanister-Aufnahme + Tragen in der freien Verfolgung |
| K | Rezept Dossier §7 (`GB_TEST=5`, Kanister (4430,-26160), Pistole, `W2.0,(M0.3,MA0.1)x10`), EXIT_AT 260 | Kanister → Schuss → R1-Kiefer-Tod |

### 3.1 M1 / Entscheid B9 — untergetaucht bis zum Durchbruch (Punkt 2) → **behoben, gemessen**
- Lauf N: debug.log `[gator-re2] Sicht F1 untergetaucht pos=(-5300,0,-16300)` … `Sicht F201 aufgetaucht pos=(5085,-1200,-19880)
  clip=6 bild=0`, `Sicht erstes Bild F201 gezeichnet … (untergetaucht vorher 200 Bilder)`, nur diese zwei Wechsel; größter
  Bildschritt danach `Sicht Lagesprung max 338 F229 … clip=6 bild=28` (Clip 6 authored ≤ 340 laut Riegel lauer). gator_boss.log
  `RE2 AUFTAUCHEN F201 pos=(5085,-19880) yaw=1536 phase=11 clip=6 bild=0 r=1466`. state.log F1–F200 `mo=0 af=0` am Lauerplatz
  (−7200,−9200) (keine Animation), erster Gator-SE `se=4 … bank=17` erst im Bild nach F207 (Clip 6 Bild 7).
- **Bytevergleich N gegen N0 (ohne Gator): F1–F201 201/201 bytegleich**, F202 erste Abweichung (Schnitt auf Cut 1). Bildbogen
  `B41abn3/N_auftauchen.png`: F195–F201 Cut 7 ohne Gator, ab F202 Cut 1 (Südostbecken), der Kopf bricht ab ~F210 aus dem Wasser.
- A2 (GB_TEST=1): `Sicht F1 untergetaucht` → `Sicht F6 aufgetaucht … clip=6 bild=0`, Bildbogen `B41abn3/A2_start.png` F1–F5 ohne
  Gator (das alte Ein-Bild-Aufblitzen an der Spawn-Lage aus Abnahme 2 §5 ist weg), F20 Kopf über dem Wasser.
- Rest wie vom Bau-Agenten benannt (OFFEN, kein Mangel dieser Abnahme): der Durchbruch beim Westkanal-Zugang liegt im
  Südostbecken hinter Leon (Intro-Lage `von (-7200,-9200) … -> (5085,-19880)`), Leon bleibt steuerbar; wer nie näher als 9000
  an den Lauerplatz kommt, sieht den Gator nicht (Form B9, Auftritt über Spur F).
- Elza (Lauf NZ, `lauf_elza.sh`, Rezept N, EXIT_AT 700#2091): `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `JUMP -> 209
  ALLIGATOR POOL (ROOM2091)`; `Sicht F1 untergetaucht` → `Sicht F201 aufgetaucht pos=(5085,-1200,-19880) clip=6 bild=0`, größter
  Schritt 338 (F229), Clip-Folge F1–F200 0 (af 0) / F201–F325 6 / F326–F475 3 — identisch mit Leon. ✓
- Echter Türweg (Lauf E1, `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1`, EXIT_AT 900, Framedump jedes 2. Bild):
  `[tuer] Sequenz Archiv 2 DOOR26 … Sequenz fertig: 286 Bilder`, `Sicht F1 untergetaucht` → `Sicht F6 aufgetaucht
  pos=(-6604,-1200,-10154) clip=6 bild=0`, Intro F6–F280. ✓

### 3.2 Punkt 1 — "Finisher und fehlende Animationen aus Resident Evil 2" → **erfüllt** (unverändert seit Abnahme 2)
- A2 (Clip-Läufe state.log t=23): F1–F5 Spawn-Bild (unsichtbar) · F6–F130 **6** · F131–F280 **3** · F283–F325 4 (Biss, Leon 60 → 10
  F290) · F360–F390 1 · F391–F416 4 (Leon 10 → −40 F402) · F417–F536 **5** · F537–F656 **11** (RE2-FRESSEN) · F657– 0 — bitgleich mit
  Abnahme 2. D2: F487–F636 **2**, F637–F786 **3** (Dash). K: F70–F260 **9** (R1 + Leiche). E1: F828–F856 **10** (R4).
- Finisher-Kamera bis Game Over (B6): gator_boss.log `RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 6 F403 clip=4 bild=0`,
  `… haelt Cut 6: RE1.5-Todesbild unterdrueckt (cam=0 weiss=1 schwarz=0) ab F467`, 14 Zeilen `Halt Cut 6 F404 … F664`. Bildbogen
  `B41abn3/A2_finisher.png`: F404 Cut 6 Seitensicht, YOU DIED ab F524, Abblende F660–F676, keine Weißblende, kein Orbit.
- Kamera-Programm (Nachtrag 4): N `RE2 KAMERA INTRO DURCHBRUCH RE2-Cut 10 -> Cut 1 F201 clip=6 bild=0`, `SEITE RE2-Cut 0 -> Cut 1
  F271 … bild=70`, `ANGRIFF RE2-Cut 1 -> Cut 1 F325 … bild=124`, `zurueck -> Cut 7 F475 clip=3 bild=149`; A2 `DURCHBRUCH -> Cut 0 F6`,
  `SEITE -> Cut 1 F76`, `ANGRIFF -> Cut 7 F130`; K `R1 EXPLOSION RE2-Cut 5 -> Cut 1 F70`, `zurueck -> Cut 0 F106`, `frei … F108`;
  integration_r41_re2gator -V (einzeln, 10:05) `nische: … Kamera DURCHBRUCH:6/0;SEITE:6/70;ANGRIFF:6/124;FREI:3/149;FINISHER:4/0,
  Finisher Cut 6 ab F403 gehalten 248 Bilder / 13 Halt-Zeilen, HP min 25300`.
- Kanister-Tod (K): `KANISTER Stufe 1 laengs=5868 quer=17 sub 1 -> 1 F6` → `angehaengt … F18` (12 Bilder) → `RE2 R1 VSync 4 (Zeitlupe)
  F70` → `[gator-re2] Raum-SE 15 -> GATOR40A0.VBS Satz 1` → `RE2 R1 ende (Leiche)` → `VSync 2 F108`; Bildtakt `VSync 4 (Bildbudget
  66 ms) ab F70 t=11376 ms` / `VSync 2 … ab F108 t=13894 ms` = 38 Bilder in 2518 ms. Bildbogen `B41abn3/K_bogen.png`: F72–F84 Blitz
  und Scherben am Kopf, ab F88 Leiche mit offenem Maul.

### 3.3 Punkt 2 — Verfolgung wie RE2 (Animation, Schreie, Beschleunigung), Wände wackeln + Putz, Klettern bleibt → **teilweise**

a) **Untergetaucht bis zum Durchbruch (B9, Abnahme-2-M1)** — behoben, §3.1.

b) **Intro/Dash im Wasser, Schreie, Beschleunigung an den Zonenlinien, Schütteln, Putz — erfüllt (unverändert).**
`INTRO ende … wand_draussen_max=0` in N/NZ/A2/D2/E1 (C3: 2 Einheiten, Nebenbefund Abnahme 2), `DASH ende … wand_draussen_max=0` in
N/D2/C3. D2: `RE2 DASH Planung: an (-6052,-18113) yaw 1139 -> Start-Gier 624`, `Vordrehung … F481`, Clip 2 F487–F636, Clip 3
F637–F786, `RE2 PUTZ runde 6/6 … gesamt=54 um=(-7097,0,-24000)`, `[gator-re2] Putz-Sicht` 25 Zeilen ab F488. Schütteln D2
(`schuettel.py mess/D2/fd d_ 536 560 529`): F542 (0,2) F543 (0,0) F544 (0,2) F545 (0,3) F546 (1,0) = Modus-2-Tafel Index 0–4,
F547–F556 (0,0)(0,1)(0,0)(0,0)(0,0)(0,1)(0,1)(0,0)(0,0)(1,1) = Modus-1-Tafel Index 5–14 (§2) — Bild für Bild RE2. Gator-SE der
Bank 17 (`se=4 … bank=17` Schreie) in allen Kampfläufen (N 8, D2 9, C3 11 Zeilen).

c) **Klettern über die Plattform bleibt — erfüllt.** C3: `CROSS start=(4450,-11900) ende=(3350,-20700) frames=240
fenster=62..177`, Clip 0 F393–F644 auf der Rampe, danach Dash F645.

d) **MANGEL (sichtbar, ALT): der Gator dreht sich in der Verfolgung an Wänden und an der Plattform in EINEM Bild um 100–180°.**
Ursache: die SELBST-BEFREIUNG des Boss-Moduls (enemy_ai_boss_gator.c:1258-1333, Nutzer-Sweep 2026-09-11) lenkt mit
`re15_enemy_steer_point(e, bx, bz, 0x800)` (Voll-Slew = Sofort-Snap, Z. 1286/1326), sobald das Zentrum im Wand-/Plattform-Slab
steht und sich < 8 bewegt hat; danach dreht die Verfolgung mit 0x60/Bild zurück, und der Zyklus beginnt neu. Gemessen
(`gier.py`, Gier-Sprünge > 400 je Bild, ohne die RE2-Knicke +1024 am Clip-3-Ende):
| Lauf | Bild: Gier vorher → nachher (Δ) | Lage |
|---|---|---|
| A2 (Türnische) | F379 r2445 → r4095 (+1650) | Westwand (−7368,−23813) |
| N (natürlich) | F1057 r3563 → r1024 (+1557) | Nordbecken (−6541,−6951) |
| D2 | F1075 r1301 → r4095 (−1302) | Westwand |
| C3 | F1097 r1487 → r4095 (−1488), F1207 (−1474), F1218 (−576), F1327 (−1490), F1338 (−528) | Westwand vor der Türnische |
| E1 (echter Türweg 20A0) | F426 r103 → r2047 (+1944), F458 (+1984), F490 (−1984), F522 (+1984), F546 (−1472) — alle 32 Bilder | Plattform Ost, Leon oben |
Dieselbe Art Sprünge steht in den Abnahme-1/2-Läufen (B41abn1 `mess/C3` F282 +1982 / F645 −1720 / F1041 +1567, `mess/A` F642
+2047; B41abn2 `mess/E1` F426…F546 bildgleich, `mess/C3` F1097/F1108/F1207/F1218) — also ALT (Befreiung von 2026-09-11, vor Runde
41), in Abnahme 2 §5 nur als Nebenbefund geführt. Bildbeleg `B41abn3/beleg_gier_kanister.png` (Zeilen 1–2: C3 F1096→F1097 der Gator
liegt längs der Wand und schaut im nächsten Bild in die Kamera; A2 F378→F379; E1 F425→F427), Bögen `A2_snap.png`, `N_snap.png`,
`E1_snap.png`, `C3_aufnahme.png`. RE2 dreht den Körper in der Verfolgung NIE (kein Gier-Schreiber in Sub 1/2/T2, Root
@0x80100264-2c8 sichert/schreibt nur zurück, §2) und hat keinen solchen Sprung; im Wortlaut ("beim hinterher laufen das gleiche
Verhalten bei Animation") ist ein 130–180°-Bildsprung ein sichtbarer Fehler. Seit Nachtrag 6 (Spur F: Leon kommt an, der Gator
kommt nach der Szene zu ihm) ist die Plattform-Belagerung aus E1 der Normalfall des Kampfes.

e) Hinweis (kein Mangel dieser Abnahme, Entscheid des Orchestrators nötig): die RE2-HALSFÜHRUNG fehlt im Port ganz — Root
@0x80100330-74 ruft nach dem Intro je Bild `FUN_80017d98` (Hals Part 5 auf Leon, Grenzen 560/312, Rate 20 aus dem INIT
@0x801005a0-c4) bzw. im Zielmodus `FUN_800177c0` (§2). Der Bau-Agent führt beides als OFFEN mit Beleg und Weg (Dossier §11.3/§11.5);
die Umsetzung braucht einen Gier-Kanal für Part 5 in skeleton_common.c bzw. den G5-Haken (keine Spur-B-Datei, VERTRAG §1.3) — das
entspricht der Forderung der Abnahme 2 (M3: "umsetzen oder mit Beleg als OFFEN führen").

### 3.4 Punkt 3 — Kanister aufnehmen "GENAUSO wie im original", "im Maul" beschießbar, Kiefer-Tod wie RE2 → **teilweise**

a) **Aufnahme ohne Körperlenkung (Abnahme-2-M2) — behoben, gemessen.** C3 gator_boss.log `RE2 KANISTER Stufe 1 laengs=5868
quer=1593 sub 2 -> 1 F1098` → `angehaengt lokal=(4277,1187,2372) F1110` (12 Bilder = RE2 t 0..12 → 13 @0x801018c4-cc) → `getragen
(Stufe 7)`; state.log F1097–F1110 Gier durchgehend r4095, z fest −24993, nur x schwingt mit dem Clip-0-Zyklus (−7368 … −7295 …
−7420). Bildbogen `B41abn3/C3_anhaengen_zoom.png`: der Kanister liegt F1104–F1110 unbewegt im Wasser vor dem Gator, F1112 wandert er
zum Maul. Riegel `unit_r41_re2gator_schleppen` grün (§1). Zu beachten: das Fenster trifft in C3 genau EIN Bild nach einem
Befreiungs-Snap (F1097 r1487 → r4095, §3.3 d) — die Aufnahme beginnt also mit einem sichtbaren 130°-Sprung.

b) **Anschwimm-Sperre (Abnahme-2-M3) — Biss-Sperre umgesetzt.** Riegel `anschwimmen` (6 Fälle, §1). exe-Lauf Z (`GB_TEST=5`,
Rezept K mit `RE15_GATOR_KANISTER=4430,-26900`, 3 Schüsse, EXIT_AT 600): `RE2 KANISTER Zielmodus 1 (Anschwimm-Sperre) laengs=6604
Stufe 0 F6`, erneut `… F100` (dazwischen R4/Clip 10 F70–F98, Bit stumm gelöscht), `Stufe 1 laengs=5956 … F147`, `angehaengt … F159`,
erster Biss mit dem Kanister erst in Stufe 7 (Clip 4 ab F185, RE2 erlaubt T1 in Stufe 7 @0x80101360-70). Das Löschen über den
`!im_chase`-Zweig und beim Anhängen schreibt keine `Zielmodus 0`-Zeile (Messzeile unvollständig, Nebenbefund). Hals-Zielmodus
OFFEN mit Beleg (§3.3 e).

c) **Beschießbar beim Tragen + Kiefer-Tod — erfüllt** (§3.2, Lauf K).

d) **MANGEL (sichtbar): der getragene Kanister läuft dem Kopf ein Bild hinterher und steht in Bewegung neben/auf/hinter dem
Kopf statt "im Maul".** Ursache: die Weltlage des angehängten Kanisters (`gr2_kan_folgen`, gator_re2_r41.c:607) wird in
`re15_gr2_takt` → `gr2_kan_takt` gerechnet, das der Boss-Takt ZU BEGINN aufruft (enemy_ai_boss_gator.c:1257), also VOR Befreiung,
Wende, `re15_ai_advance` (Z. 1438), Bildvorschub und Dispatcher-Klemme desselben Takts; der Zeichner (gator_re2_pc.c:108) zeichnet
diese alte Lage zum Gator in der NEUEN Lage. RE2 hängt den Kanister als Kind an die Part-6-Matrix (`addiu a0,s0,1104`
@0x801018d0, `sw a0,128(s2)` @0x801018e0, §2). Messung:
- Prüfer-Sonde `B41abn3/probe/probe_abnahme3.c` Teil `nachlauf` (= Spur-Riegel-Harness + eigener Teil, gegen
  `build/engine/libre15_engine.a`, NICHT im Baum): Kanister wie Riegel `schleppen` Fall 1 aufgenommen, Stufe 7 + 40 Takte
  (Lokallage fest (1750,−210,0)), dann 240 Takte Verfolgung mit Wende und Biss (Leon am Leben gehalten). Fehler je Bild =
  |gezeichnete Lage − Lage aus der Part-6-Pose desselben Bildes|: **Clip 0 (Wende 0x60/Bild) 619–637 Einheiten je Bild** (Takte
  160–170), Clip 4 (Biss) bis 409, Mittel 124, > 300 in 51 von 240 Bildern. Zum Vergleich: die Kanister-Lokallage selbst liegt
  1175 vor dem Part-6-Ursprung.
- exe C3 (Stufe 7 ab F1136; Lokallage nach den RE2-Schritten ab ~F1166 fest): bei den Befreiungs-Snaps steht der Kanister an der Kopflage des Vorbilds —
  F1207, F1218, F1327, F1338 ragt er links neben/hinter dem in die Kamera gedrehten Kopf heraus; bei der normalen Wende F1276–F1282
  (r3403 → r2923, −48/−96 je Bild, Clip 0) wandert er vom Maul (F1278, gelbes Ende an der Schnauze) auf den Kopf (F1280–F1284).
  Bildbeleg `B41abn3/beleg_gier_kanister.png` (Zeilen 2–3), Bögen `C3_kanister_zoom.png`, `C3_getragen_zoom2.png`,
  `C3_snap_getragen.png`.
- Die Bildbelege des Bau-Agenten (`bilder_r41b/nb3_m2_C3_getragen_quer_ohne_mit_x3.png`, F1215/F1260/F1290) zeigen Ruhebilder; die
  Aussage "endet zwischen den Kiefern" gilt nur ohne Bewegung. Kein Riegel prüft die Kanister-Lage gegen die Kopf-Pose desselben
  Bildes (schleppen Fall 3 prüft nur die Lokallage).

## 4. Gates

### 4.1 RE-Gate
- Diff-Grep (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen) nach "deferred|tunable|interim|for now|faithful|plausib|TODO|
  FIXME|XXX|vorerst|geschaetzt|ungefaehr|approx": **0 Treffer** (Nachbesserung 3 und Gesamtstand). Kein neuer Env-Schalter als
  Abschluss (Nachbesserung 3 liest nur das bestehende `RE15_GB_TEST` für den Marker-Repro 5).
- Konstanten der Nachbesserung 3: Untertauchen = Trefferbox 0 / kein Zeichnen (Form B9, RE2 Sce_em_set y +3000 @RDT 0x25A0, Sub 0 P0
  @0x80100854-68, Pre `jr ra` @0x801007fc); Lenksperre Stufe 1/2 bis t 13 (@0x801018c4-cc); Zielmodus 9000 − 120 (@0x80100a88-8c) ×
  Zeichen-Skala, Zone = PORT-WAHL; Quer-Ausgleich z → 0 mit den x-Schritten 70/145/100/100 = PORT-WAHL, als solche markiert
  (RE2 führt z nie, §2 bestätigt); probe_r34_reaktion Leon 6000 vor dem Lauerplatz = Testlage (< GB_AGGRO 9000). Alle mit @0x im
  Kommentar oder als PORT-WAHL benannt; Commit-Texte tragen die Adressen.
- Selbst disassembliert/gelesen und bestätigt (§2): Sub 0 P0/P1, Pre-Tafel, T2-Tafel, Stufe 1→2-Durchfall, Stufe-2-Schub, t 13,
  Zielmodus setzen/löschen, T1-Sperre, Lokallage-Schritte x/y und kein z-Schreiber, Root-Hals, RDT 0x25A0, SCD 0x0730/0x0736,
  Schüttel-Tafeln, Anhängen über den Eltern-Zeiger +0x80. **Keine falsche Adresse gefunden.**
- "Erklärt der Fix den Befund?": M1 ja (bytegleich mit "ohne Gator" bis zum Durchbruch, kein Aufblitzen, Elza gleich); M2 ja für die
  Aufnahme (Gier fest, Kanister still, 12 Bilder), aber der Befreiungs-Snap direkt vor dem Fenster (C3 F1097) und das Nachlaufen des
  getragenen Kanisters (§3.4 d) bleiben; M3 ja für die Biss-Sperre (Riegel + exe-Zeile), Hals OFFEN mit Beleg.

### 4.2 Vertrag
- Bank-9-Bits / Nachrichten-Ids: keine (Diff ohne `game_flag_set`/`MSG_ID`/`g_game.flags`-Schreiber).
- Pfade (`git diff master...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine
  CMakeLists; neue RE2-Daten nur unter shared_assets/RE2/ (GATOR40A0.VBS, KANISTER40A0.MD1/.TIM, im Dossier genannt).
  Gemeinsame Dateien: main.c +1 Zeile in Nachbesserung 3 (insgesamt 5 markierte Haken, Entscheid B5), enemy_ai_re2_zombie.c +4 und
  re2_fx.c +41 unverändert seit Abnahme 1; probe_r34_reaktion.c +10 (Zensus misst den aufgetauchten Boss, begründet).
- Tests: neu `unit_r41_re2gator_lauer` (messend: Box/Ziel/Bewegung je Takt, Auftauchen, Schrittgrenze aus den authored Clips),
  `_schleppen` (Gier, Wege, Takte, Lokallage), `_anschwimmen` (6 Fälle), integration Lauf 3 "natuerlich". Parallel-fest (eigene
  Kopie `re15_pc_r41b_<id>.exe`, eigener Laufordner). Es fehlt ein Riegel für die Kanister-Lage gegen die Kopf-Pose desselben Bildes
  und für Gier-Sprünge in der Verfolgung (§3.3 d / §3.4 d).

### 4.3 Nebenwirkungen
- Elza (ROOM2091): NZ identisch mit Leon (§3.1). Echter Türweg 20A0 → 2090 (E1) ✓.
- Nachbarraum-/Querriegel siehe §5 (Integration).

## 5. Integration (gezielt, am gebauten Stand)
- `ctest --test-dir re15_port/build -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate|r37_itemband|r36_tueren|
  r37_re2zombie)$" --timeout 1500 -j 3 --output-on-failure` (09:57:32–10:12:30, neben fremden Läufen anderer Bäume) → **100% tests
  passed, 0 tests failed out of 7** (r35_granate 178,8 s, r38_wasser 189,8 s, r36_tueren 73,9 s, r36_schuss 215,0 s, r41_re2gator
  131,0 s, r37_re2zombie 59,6 s — zusätzlich zum Satz des Bau-Agenten, weil enemy_ai_re2_zombie.c im Diff steht —, r37_itemband
  897,5 s). `integration_r41_re2gator -V` einzeln: `nische: RE2-Zeilen 57, Intro Lage/Start/Ende 1/1/1 wand 0, Clip 6/3 125/150, …
  Finisher Cut 6 ab F403 gehalten 248 Bilder / 13 Halt-Zeilen, HP min 25300 | natuerlich: untergetaucht bis F201 (Lauerplatz
  -7200,-9200), Auftauchen Clip/Bild 6/0, erstes Bild F201 Clip 6, groesster Bildschritt 338 | dash: DASH 1/1 wand 0, Putz 6 x 9 = 54,
  gezeichnet bis 98 Quads/Bild in 20 Messbildern, Clip 2/3 150/300` (`B41abn3/ctest_integ_r41_V.log`).
- Keine rote Fenster-Probe, keine Wiederholung nötig.

## 6. Nebenbefunde (keine Mängel dieser Abnahme, zur Kenntnis)
- Granate am Versteck (OFFEN des Bau-Agenten, Nutzerfrage 1): integration_r35_granate Lauf "gator" (`lauf_4e853456/gator`):
  `[harness] RE15_FORCE_EXPLOSION F60 art=2 slot=20 t=23 P=(-7200,-500,-9200) Treffer=1` → `Sicht F61 aufgetaucht … clip=6 bild=0`,
  `RE2 AUFTAUCHEN F61`, keine R4-/SE-0/5-Zeile — wie im Dossier beschrieben (RE2 erreicht den Gator 3000 unter dem Boden nicht).
- Zielmodus-Messzeile: das Löschen im `!im_chase`-Zweig und beim Anhängen schreibt keine `Zielmodus 0`-Zeile (Lauf Z: zweimal
  `Zielmodus 1` ohne 0 dazwischen).
- Wand-Klemm-Zittern: an der Westwand springt die Wurzel in der Verfolgung jedes Bild um 30–60 hin und her (C3 F1272–F1282:
  z −25396/−25427/−25388/−25425/…), ALT.
- C3 F1136 (Stufe 7 erreicht, Kanister noch vor der Schnauze + Nachlauf): Kanister-Weltlage `pos=(-6859,-1509,-29781)` = 2781 hinter der
  Südwand (z −27000), im Bild über der Wandfläche (`B41abn3/C3_suedwand.png` F1128–F1136).
- E1: nach R4 (Clip 10 F828–F856) springt die Wurzel F857 um 861 Einheiten zurück ((5716,−11877) → (4985,−12332)) — ALT
  (B41abn2/mess/E1 bildgleich).
- Der Durchbruch beim natürlichen Westkanal-Zugang liegt im Südostbecken hinter Leon, Leon bleibt steuerbar (OFFEN §10.6/§11.5 des
  Dossiers, B9 verlangt nur Unsichtbarkeit) — für Spur F (Szene nach Nachtrag 6) relevant.

## 7. Behauptungen des Bau-Agenten — Abgleich am gebauten Stand
| Behauptung | gemessen | Ergebnis |
|---|---|---|
| M1: vor dem Durchbruch weder gezeichnet (auch kein Schatten) noch Trefferbox, keine Animation/SE | N: F1–F201 bytegleich mit N0 (ohne Gator), state.log mo 0 af 0, erster Bank-17-SE nach F207; Riegel lauer (Box 0, kein Ziel) | ✓ |
| Auftauchen mit dem Intro-Start (Clip 6 Bild 0), größter Bildschritt 338 (Grenze 340) | N/NZ: `Sicht F201 aufgetaucht … clip=6 bild=0`, `Lagesprung max 338 F229`; A2/E1 F6 | ✓ |
| Treffer aus dem Versteck ohne Reaktion/Blut/Ruf (nur Granate erreicht ihn) | r35_granate Lauf gator: `Treffer=1` → Auftauchen F61, keine R4/SE-Zeile | ✓ (Granaten-Fall OFFEN benannt) |
| Elza (ROOM2091) identisch | NZ (PL04, ROOM2091) identisch | ✓ |
| M2: Lenksperre vom Fenster bis zum Anhängen, Stufe 1 im selben Takt wie RE2 | C3: Gier r4095 F1097–F1110, z fest, Anhängen 12 Bilder nach dem Fenster; RE2 @0x8010183c-5c → @0x80101860 selbst gelesen | ✓ |
| Quer-Ausgleich (PORT-WAHL): Kanister endet zwischen den Kiefern | im Ruhebild ja (C3 F1278); in Bewegung NEIN — der getragene Kanister läuft ein Bild nach (§3.4 d) | ✗ teilweise |
| M3: Zielmodus +0x1C0 & 4 sperrt den Biss | Riegel anschwimmen 6/6; exe Z `Zielmodus 1 … laengs=6604` | ✓ (Hals OFFEN) |
| K (Kanister-Tod) unverändert | K: Stufe 1 F6, angehängt F18, R1 F70, Zeitlupe 38 Bilder, Leiche | ✓ |
| Unit 108/108, Integration 6/6, Endstand 14/14 | Unit 108/108 (mit r36_kartenbits), r41-Unit 14/14, Integration 7/7 (§5) | ✓ |

## 8. Mängel (nummeriert, nachprüfbar)

1. **M1 (Punkt 3, sichtbar): der getragene Kanister läuft dem Kopf ein Bild hinterher und steht in Bewegung neben, auf oder hinter
   dem Kopf statt "im Maul".** Ursache (Spur-B-Code, 4c376639): `gr2_kan_folgen` (gator_re2_r41.c:607) rechnet die Weltlage in `re15_gr2_takt`, das der
   Boss-Takt zu Beginn aufruft (enemy_ai_boss_gator.c:1257) — vor Befreiung, Wende, `re15_ai_advance` (Z. 1438), Bildvorschub und
   Dispatcher-Klemme desselben Takts; gator_re2_pc.c:108 zeichnet diese alte Lage zum Gator in der neuen Pose. RE2 hängt den Kanister
   als Kind an Part 6 (`addiu a0,s0,1104` @0x801018d0, `sw a0,128(s2)` @0x801018e0). Nachprüfung: Prüfer-Sonde
   `B41abn3/probe/probe_abnahme3.c` (Harness des Spur-Riegels + Teil `nachlauf`, Bau-Befehl §9) → `nachlauf: Clip 0 … max 637`,
   Takte 160–170 (Wende 0x60/Bild) 619–637 Einheiten je Bild, Clip 4 (Biss) bis 409, > 300 in 51/240 Bildern (Lokallage fest
   (1750,−210,0) = 1175 vor dem Part-6-Ursprung); exe C3 (Rezept §3) F1206→F1207, F1278→F1282, F1337→F1338
   (`B41abn3/beleg_gier_kanister.png`). Verlangt: Weltlage des angehängten Kanisters aus der Pose des gezeichneten Bildes (nach dem
   Boss-Takt bzw. beim Zeichnen aus der aktuellen Part-6-Matrix), dasselbe für die Effekt-Quellmatrizen; Riegel: |gezeichnete
   Kanister-Lage − Part6(Bild) × lokal| = 0 bei Wende, Biss und Befreiung; Bildbeleg vorher/nachher an C3 F1276–F1284.
2. **M2 (Punkt 2, sichtbar, ALT seit 2026-09-11): der Gator dreht sich in der Verfolgung in einem Bild um 100–180°.** Ursache:
   Selbst-Befreiung (enemy_ai_boss_gator.c:1258-1333, d52a679f 2026-09-11) lenkt mit `re15_enemy_steer_point(e, bx, bz, 0x800)` (Z. 1286/1326) = Sofort-
   Snap; danach dreht die Verfolgung zurück, an der Plattform im 32-Bild-Takt. Nachprüfung (`B41abn3/gier.py <state.log> 1 9999 400`):
   E1 (`RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1`, echter Türweg) F426 +1944, F458 +1984, F490 −1984, F522
   +1984, F546 −1472 (Plattform Ost, Leon oben); C3 F1097 −1488, F1207 −1474, F1327 −1490 (Westwand); A2 F379 +1650; D2 F1075 −1302;
   N F1057 +1557; Bilder `beleg_gier_kanister.png` Zeilen 1–2, `E1_snap.png`, `A2_snap.png`. RE2: im ganzen EM23-Overlay nur 8
   Gier-Schreiber (`sh …,118(…)` @0x80100280/2a0 = Sichern/Rückschreiben, @0x80100948/@0x80100ec0 = Knicke, @0x80102404/2440/27c8/
   2a34 = R1–R3; kein `sw …,116(…)`), Sub 1/2 rufen nur 0x801012fc/0x80077360/0x8001a330 — der RE2-Gator dreht den Körper in der
   Verfolgung nie. Verlangt: keine Gier-Änderung über der Port-Wenderate (0x60/Bild) in Verfolgung/Belagerung (CHASE/GUARD; das Klettern CROSS mit seinem Bahn-Blick ist Nutzer-Design und ausgenommen) außer den RE2-Knicken
   (+1024 am Clip-3-Ende @0x80100938-48/@0x80100eb0-c0), Befreiung ohne Sofort-Snap (Form frei, PORT-WAHL benennen); Riegel auf die
   Lagen E1 (Plattform-Belagerung) und C3 (Westwand/Türnische) über ≥ 300 Bilder; Bildbeleg.

Hinweis an den Orchestrator (kein Mangel dieser Abnahme): die RE2-Halsführung (Root @0x80100330-74 → FUN_80017d98 auf Leon,
Zielmodus FUN_800177c0; Grenzen 560/312, Rate 20 @0x801005a0-c4) fehlt im Port; der Bau-Agent führt sie OFFEN mit Beleg, die
Umsetzung braucht einen Part-5-Gier-Kanal außerhalb der Spur-B-Dateien (skeleton_common.c oder G5-Haken) — Entscheid nötig.

## 9. Gesamturteil

`bestanden = false`. Die drei Mängel der Abnahme 2 sind am gebauten Spiel nachgebessert: der Gator ist bis zum Durchbruch
untergetaucht (Lauf N F1–F201 bytegleich mit "ohne Gator", Auftauchen mit Clip 6 Bild 0, größter Schritt 338, Elza gleich, kein
Spawn-Aufblitzen mehr), die Aufnahme lenkt nicht mehr (Gier fest, Kanister still, Anhängen nach 12 Bildern wie RE2), die
Anschwimm-Sperre sperrt den Biss (Riegel + exe-Zeile); alle selbst geprüften RE2-Stellen (§2) stimmen; Riegel Unit 108/108,
r41-Unit 14/14, Integration 7/7 (§5); Vertrag eingehalten; Punkt 1 (mit Nachtrag 4 und B6) weiter erfüllt. Offen gegen den
Wortlaut: **M1** — "Sobald er den Kanister im Maul hat": in Bewegung hängt der Kanister ein Bild hinter dem Kopf (bis 637 Einheiten
je Bild bei der normalen Wende, bei Befreiungs-Snaps an der alten Kopflage), Punkt 3 teilweise; **M2** — "beim hinterher laufen das
gleiche Verhalten bei Animation": 100–180°-Gier-Sprünge der Befreiung an Wand und Plattform (ALT, aber sichtbar und gegen RE2),
Punkt 2 teilweise.

Prüfer-Sonde (nachfahrbar, liegt NICHT im Baum): `B41abn3/probe/probe_abnahme3.c` = Kopf von `tests/unit/test_r41_re2gator.c` + Teil
`nachlauf`; gebaut mit `C:/msys64/mingw64/bin/gcc.exe -std=c11 -O1 -DRE15_PLATFORM_PC=1 -DRE15_ASSET_PSX_DIR=… -DRE15_ASSET_RE2_DIR=…
-I re15_port/include probe_abnahme3.c re15_port/build/engine/libre15_engine.a re15_port/build/tests/libre15_test_support.a -lm` (PATH mit
msys64 zuerst), Aufruf `probe_abnahme3.exe nachlauf` (Ausgabe `probe/nachlauf.txt`).
