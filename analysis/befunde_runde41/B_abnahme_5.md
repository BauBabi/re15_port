# Runde 41 — Spur B "re2gator" — Abnahme 5 (unabhängig)

Prüfer: Abnahme-5-Agent, 2026-10-24 (ab 15:09). Baum `.claude/worktrees/r41_re2gator`, HEAD 73979140 (Zweig r41/re2gator,
merge-base mit master 116e8b0f). Geprüft wird der Stand der Nachbesserung 5 (10 Commits 27d45d33 … 73979140, Code-Diff
`git diff 71192fbc..HEAD -- re15_port`). Aus Dossier und Bau-Meldung wird nichts ungemessen übernommen. Messdateien:
Prüfer-Scratchpad `C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/B41abn5/`
(Läufe unter `mess/<Lauf>/`, eigene exe-Kopien `re15_pc_b41a5_<Lauf>.exe` bzw. `re15_pc_b41a5z_<Lauf>.exe` für Elza, je Lauf
`lauf.txt` mit allen Variablen). "Vorher" = Läufe der Abnahme 4 (`.../scratchpad/B41abn4/mess/<Lauf>/`, `B41abn4f/`).

(Fortlaufend geschrieben; Gesamturteil am Ende.)

## 1. Lektüre + Bau

- Gelesen: AUFTRAG.md (Wortlaut; Spurbaum = master-Stand bis Nachtrag 11, die neueren master-Einträge 12/13 und I-E1/I-E2
  betreffen nur C/I), VERTRAG.md, UEBERGABE.md, B_abnahme_4.md vollständig (M1, H1–H3), Dossier B_re2gator.md §13.0–§13.9,
  `git log --oneline master..HEAD`, `git diff master...HEAD --stat` (Zweig gesamt 72 Dateien, +10632/−60) und den Code-Diff der
  Nachbesserung 5 vollständig: enemy_ai_boss_gator.c +41/−3 (gb_klemme_aus, gb_biss_abschluss, FRESSEN P0), gator_re2_r41.c +358/−1
  (Bühne gr2_fin_tabelle/gr2_fin_frei/gr2_fin_sicht/re15_gr2_finisher_buehne/_setzen/_cut, re15_gr2_fressen_p0,
  re15_gr2_fress_sitz, Kamera-Zweig in re15_gr2_kamera_finisher), gator_re2_pc.c +80 (Messzeile Maul-Sitz), die zwei Header (+1, +23),
  test_r41_re2gator.c +320 (Teile maul/buehne), probes/r41_re2gator.cmake, integration/test_r41_re2gator.cmake +75/−3
  (r41b_maul_sitz, belagerung bis F890), probe_gator_fress.c (Schranke 5 → 16).
- Bau: `bash re15_port/tools/local_build.sh configure` → `LOCAL-BUILD-OK (configure)` (15:10); alle Spur-B-Quellen des Zweigs
  (`git diff --name-only master...HEAD -- re15_port`, 15 Dateien .c/.h/.inc) per `touch` (nur mtime), dann `local_build.sh build`
  → 798 Schritte, `LOCAL-BUILD-OK (build)`, re15_pc.exe 15:11:16. Warnungen nur main.c 8803/10133/10513
  (`-Wdiscarded-qualifiers`, Altbestand), keine in Spur-B-Dateien (`B41abn5/build.log`).
- Gezielte Unit-Riegel (Regex des Bau-Agenten + `re2_zombie_.*|re15_zombie_death_anim`, weil der Zweig enemy_ai_re2_zombie.c
  berührt; `B41abn5/unit_regex.txt`): `ctest -R "<regex>" --timeout 600 -j 6 --output-on-failure` → **100% tests passed, 0 tests
  failed out of 121** (15:13–15:16, `B41abn5/ctest_unit.log`; unit_gator_sweep 189,9 s).
  Einzeln mit -V (`B41abn5/unit_maul_V.txt`):
  - `unit_r41_re2gator_maul`: CHASE-Bezug `Tod Takt 7 …, Finisher 254 Bilder (P1 14 P2 120 P3 120), Cut 0, Maul >= 50 %: P1 0 P2 106
    P3 120, … Kiefer-Abstand max 3`; GUARD Rampe Ost (6300) `Takt 135 aus Phase 5 … Cut 4 … Kopf-lokal 5, Maulraum 10 Promille,
    Kiefer 2`; E1-Lage (6732) `Takt 138 … Cut 4 … 5 / 10 / 2`; Rampenmitte `Takt 95 … Cut 2 … 9 / 11 / 3`; Plattformblock
    `Takt 323 … Cut 7 … 8 / 11 / 2`; BL2-Lage `Takt 7 … Cut 2 … 14 / 13 / 2`; überall `nicht im Wasser 0, Kopf nicht im Bild 0,
    Leon nicht im Bild 0, Wandklemme verschob Leon in 0 Bildern, Band falsch 0` → `maul: OK (0 Fehler)`.
  - `unit_r41_re2gator_buehne`: `2128 Gator-Lagen …, ohne Buehne 0, Buehne falsch 0, Versatz max 9216` (Dossier §13.5 nennt 8984 —
    kleine Doku-Abweichung, Schranke 27000 hält).
  - `unit_gator_fress`: `PIN P3: … yaw=289 … dRef=(-8,-1,-2) … 6=1/7=1`, `Maulraum … 776/776 = 100.0%`.

## 2. RE-Stichproben (selbst disassembliert; `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` bzw. `re15_disasm.py`)

RE2-Overlay `info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN` (md5 0293e099…, = `shared_assets/RE2/EM23_OVL_0000.BIN`), RE2-EXE
`info/re2leon/PSX.EXE`, RE1.5-EXE `info/Re1.5/PSX.EXE`. Ausgaben `B41abn5/re2_p0.dis`, `re2_8001a330.dis`, `re2_80015cb8.dis`,
`re15_80031c44.dis`, `re15_80038440.dis`.
- **FRESSEN P0 (EM23)** `dis 0x80100fbc 80`: `ori v0,v0,0x100 / sw v0,0(a0)` @0x80100fcc-d0 (Sperre), `addiu a0,zero,14` @0x80100fec /
  `addiu a0,zero,7` @0x80100ff0 / `jal 0x8002c7ac` @0x80100ff4 (Schnitt), `lui at,0x800d / sw zero,1308(at)` @0x80101008-0c
  (= 0x800d051c, Kanister-Objekt aus), `sh v0,-690(at)` @0x80101040 (HP −1), **`addiu v0,zero,-23078 / sw v0,56(s0)`
  @0x80101048-4c (Gator-x umsetzen)**, **`sh zero,554(s0)` @0x80101050 / `sh zero,552(s0)` @0x80101054 / `sb zero,548(s0)`
  @0x80101058**, `lw v0,56(s0) / addiu v1,zero,-915 / sw v1,-968(at) / addiu v0,v0,10643 / sw v0,-976(at)` @0x8010106c-84. ✓
- **P1-Wechsel** @0x801010e8-f8: `addu a0,s0,zero / addiu a1,zero,4 / addu a2,zero,zero / jal 0x8001a330 / addu a3,zero,zero` =
  FUN_8001a330(em,4,0,0). ✓ **FUN_8001a330 (RE2-EXE)** @0x8001a34c-64: `lbu v0,332(s0) / beq v0,a1,0x8001a368 / sb a1,332(s0) /
  sb zero,333(s0) / sb s1,334(s0)` (s1 = a2 = 0 → Überblend-Zähler 0, nur bei Clip-Wechsel). ✓
- **FUN_80015cb8 (RE2-EXE)** @0x80015cb8-dac: kf `jal 0x80015db0`, Einheitsmatrix 0x8009db44, `lh a0,118(s1) / jal 0x8008e8b4`
  (RotMatrixY Gier), `jal 0x8008dba4` (ApplyMatrix), Lage = Anker +356/+358/+360 + Versatz: `sw v0,56(s1)` @0x80015d68,
  `sw v0,60(s1)` @0x80015d7c, `sw v0,64(s1)` @0x80015d90, `jr ra` @0x80015da8 — **kein Kollisionsaufruf**. ✓
- **P2/P3 (EM23)** `dis 0x801010fc 60`: P2 `jal 0x80015cb8` @0x80101174 + `FUN_8001a330(em,5,0,0)` @0x8010117c-88, Phase +1 nur
  bei Clip-Ende (@0x80101190-c0); **P3 @0x801011c4-e4: `jal 0x80015cb8` @0x801011cc + `FUN_8001a240(em,11,0,0)` @0x801011d8-e0
  und KEIN Phasen-Schreiber** — die RE2-Kauschleife läuft ohne Ende weiter (wichtig für §3.3).
- **Spieler-Wandklemme im Tod (RE1.5-EXE)** `dis 0x80031c44 76`: Kommando-Handler `jalr v0` @0x80031cb4 (Tafel 0x80073f90,
  Index 0x800aca58), `jal 0x8002b544` @0x80031cbc, `jal 0x8003b0a4` @0x80031d70 mit `ori a2,zero,0x1` @0x80031d74. ✓
- **Band = −(h/1800) (RE1.5-EXE)** `dis 0x80038440 36`: `jal 0x8001c6e8` (room_coll) @0x80038448, `lui a0,0x91a2 / ori a0,a0,0xb3c5`
  @0x80038450-54 (0x91A2B3C5 = ⌈2^42/1800⌉), `mult t0,a0` @0x80038460, `mfhi / addu / sra 10 / subu v1,v1,v0` @0x800384a8-b4,
  `sb v1,-13610(at)` = player.floor (+0x82) @0x800384bc, danach `jal 0x8003b0a4` @0x800384c4. ✓
- **Kamera-Apply am Bildanfang (RE1.5-EXE)** @0x800214f4-514: `lh v1,4068(v1)` (0x800b0fe4) gegen `lbu v0,-1099(v0)` (0x800afbb5),
  bei Ungleichheit `sb v0,21591(at)` (0x800b5457 = 1). ✓ (stützt die Begründung "Schnitt-Auftrag im Biss-Bild").
- Schluss: **keine falsche Adresse** gefunden; alle zitierten Stellen enthalten das Behauptete.

## 3. exe-Läufe (eigene Kopien `re15_pc_b41a5(z)_<Lauf>.exe`, Ordner `B41abn5/mess/<Lauf>/`)

Basis (`B41abn5/lauf.sh`, = Rezepte der Abnahme 4): `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_STATE_LOG=state.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=audio.raw`, `timeout`,
Umgebung des Aufrufers ohne RE15_*-Testhebel (`env -u …`); Elza (`lauf_elza.sh`) mit `RE15_TITLE_CONFIRM_MS=1500
RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`. Auswertung `bisse.py` (Gier gegen Peilung je HP-Abzug), `sheet.py`/`crop.py`
(Bildbögen/Ausschnitte; Aufhellung nur zur Sichtung), `schuettel.py`, `cmp` der state.log gegen Abnahme 4.

| Lauf | Rezept (zusätzlich zur Basis) | Zweck |
|---|---|---|
| E1 | `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1 RE15_EXIT_AT=890#2090 RE15_FRAMEDUMP=600-890:fd/e_` | echter Türweg 20A0 → 2090, GUARD-Tod an der Ostrampe, jedes Bild |
| E1E | wie E1, `RE15_EXIT_AT=902#2090 RE15_FRAMEDUMP=870-902:fd/x_` | Ende des Finishers bis zum letzten 2090-Bild |
| E1L | wie E1, `RE15_EXIT_AT=1000` | Zeitpunkt des Raumwechsels nach dem Game Over |
| MN | `RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=4000,-15000,2048,1 RE15_GB_TEST=1 RE15_EXIT_AT=850#2090 RE15_FRAMEDUMP=560-850:fd/m_` | GUARD-Tod Rampenmitte |
| A2 | `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_EXIT_AT=670#2090 RE15_FRAMEDUMP=380-670:fd/a_` | CHASE-Tod Türnische |
| EZT / EZT2 | Elza, `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A1 RE15_GB_TEST=1`, EZT `EXIT_AT=1300` (Zeitlinie), EZT2 `EXIT_AT=902#2091 FRAMEDUMP=620-902/2:fd/z_` | echter Türweg 20A1 → 2091 mit Elza |
| D2 / C3 / K / N | Rezepte der Abnahme 4 (`B41abn4/mess/<Lauf>/lauf.txt`), D2 mit `FRAMEDUMP=529-560`, K mit `FRAMEDUMP=60-120/2` | Regression Punkte 2/3 |
| S1 / S2 / S3 | `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_PLAYER_POS=<-4000,-22000 / 3000,-24000 / -6000,-9000>,2048 RE15_EXIT_AT=1500#2090` | Tod im offenen Wasser (Bühnen-Kamera gegen vorige Kamera) |
| G1 / G2 / G3 (+ G1F, G2F mit Framedump) | wie S, `RE15_PLAYER_POS=<-7950,-25050 / -5950,-20550 / -3000,-25500>,2048`, `EXIT_AT=1200#2090`; G1F `EXIT_AT=420#2090 FRAMEDUMP=394-420:fd/g_`, G2F `EXIT_AT=460#2090 FRAMEDUMP=434-460:fd/h_` | dto., gezielt im Cut-0-/Cut-6-Bereich |
| BL | `RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=0,-13200,2048,1 RE15_GB_TEST=1 RE15_EXIT_AT=800#2090 RE15_FRAMEDUMP=520-800/2:fd/b_` | Tod nach Sturz von der Plattform (Band-Haken, = BL3 des Bau-Agenten) |

Alle Läufe rc = 0 außer EZT, S1–S3, G1–G3 (rc = 124: nach dem Game Over ohne Raum-Bedingung bis zum timeout weitergelaufen bzw. bei Elza
in der Game-Over-/Titelfolge des Ports stehen geblieben — kein Spur-B-Befund; S1/S2/G3 starben an einer Raum-Spinne, nicht am Gator).
- **BL (Sturz von der Plattform, Band-Fall):** Tod F526 (CHASE, 0°), `Buehne … von (4741,-11236) Gier 233 -> (4117,-7033) Gier 1083 Cut 2,
  Versatz 4222 Drehung 850`, `Maul-Sitz Ende F781: … Kiefer-Abstand max 2 …, Leon im Bild 254/254 …, im Wasser 254/254`; Bild
  `BL_zoom.png` (×4): F528–F540 Leon vor dem Gator, F544–F580 im Maul, F620 Wurf, F700/F760 Beine aus dem Maul — die Ursache (4) des
  Bau-Agenten (Band 1 nach dem Sturz) ist im Spiel behoben.

### 3.1 M1 der Abnahme 4 (Leon nach dem GUARD-Hochbiss nicht im Maul) — im Kern behoben, gemessen

- **E1 (Türweg):** debug.log `[aot] DOOR FIRE slot=0 rect=(4950,5000,hw=1500,hh=500) target_cut=2 spawn=(6300,-1800,-16250)`,
  `[tuer] Sequenz fertig: 286 Bilder`; Bisse (`bisse.py`) `F417 hp 100 -> 50 … Gier 169 … daneben +757 (67 Grad)`, `F522 hp 50 -> 0 …
  Gier 13 … +835 (73 Grad)`, `F627 hp 0 -> -50 … (5716,-12762) Gier 115 … +724 (64 Grad)` (Ablauf bis zum Tod Bild für Bild wie
  Abnahme 4); gator_boss.log `RE2 FINISHER Buehne (Nachbesserung 5, RE2-P0 @0x80101048-4c): von (5763,-12762) Gier 115 ->
  (5638,-12546) Gier 2323 Cut 4, Versatz 249 Drehung 1888 … F627`, `RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 4 F627 … frontal=4115
  … (Buehne)`, `Halt Cut 4` F628…F888, `RE1.5-Todesbild unterdrueckt … ab F692`; debug.log `[gator-re2] Maul-Sitz Ende F882: Bilder
  254 (P1 14 P2 120 P3 120), Abweichung max 0 (F628), Kiefer-Abstand max 2 (F763), Kopf im Bild 254/254, Leon im Bild 244/254 + oben
  hinaus 10 (Cut 4), im Wasser 254/254` (Zwischenzeilen F628…F867 alle `Abweichung max 0`, Kiefer ≤ 2).
  Bilder (`B41abn5/E1_schnitt.png`, `E1_p1_zoom.png`, `E1_p23_zoom.png`, `E1_p3_zoom4.png`): **F627 zeigt den Biss noch in Cut 2,
  F628 schneidet auf Cut 4** (Gator frontal hinten im Becken) — kein Lagesprung in der alten Kamera; F642–F646 Leon (blau) im Maul
  gepackt, F648–F696 Schütteln mit Leon im Maul, F704–F720 Wurf (Leon oben aus dem Bild), F728–F736 wieder im Maul, P3 F760–F880
  Kauschleife mit Leons Beinen/Körper im Maul (F776–F808, F856–F864 deutlich). Gegenstück vorher (Abnahme 4): Maul leer, Leon
  hinter der Ostwand (`B41abn4/beleg_E1_finisher.png`).
- **MN (Rampenmitte):** Bisse F377 (50°), F482 (61°), **F587 (Gier 3405, 128° daneben) → Tod**; `Buehne … von (5716,-12583) Gier
  3405 -> (5716,-12583) Gier 2323 Cut 4, Versatz 0 Drehung 1082`, Kamera vorher cam=0 (state.log F584–F586) → Cut 4 ab F587;
  `Maul-Sitz Ende F842: … 254 (14/120/120), Abweichung max 0, Kiefer-Abstand max 2, Kopf 254/254, Leon 244/254 + oben 10, Wasser
  254/254`. Bild `B41abn5/MN_p1_ende.png`: F586/F587 Biss in Cut 0, F588 Cut 4, F602–F640 Leon im Maul, F700 Wurf, F760–F840 P3.
  Vorher (Abnahme 4 MN2): Leon neben/über dem Kopf.
- **A2 (Türnische, CHASE-Tod):** Bisse F290 (−1°), **F397 (0°) → Tod**; `Buehne … von (-7407,-22250) Gier 2569 -> (-6657,-22250)
  Gier 3027 Cut 6, Versatz 749 Drehung 458`, Kamera vorher cam=0 → Cut 6; `Maul-Sitz Ende F652: … 254 …, Kiefer-Abstand max 2, Kopf/Leon
  254/254, Wasser 254/254`. Bild `A2_schnitt.png`: F397 Biss in Cut 0, F398 Cut 6 (frontal), F398–F410 Leon sichtbar vor dem Gator
  (P1), F412 gepackt.
- **Elza, echter Türweg 20A1 → 2091 (EZT/EZT2):** `JUMP -> 20a SERVICE TUNNEL (ROOM20A1)`, `Spieler-Familie PL04 (character=4,
  Elza-Bit=1)`, `DOOR FIRE slot=0 … spawn=(6300,-1800,-16250)`; Bisse und Bühne bitgleich mit E1 (F417/F522/F627, `… -> (5638,-12546)
  Gier 2323 Cut 4`), `Maul-Sitz Ende F882` wie E1. Bild `EZT2_finisher.png`: Elza (rot) F642–F644 gepackt, F650–F680 im Maul, F700–F720
  Wurf, F740–F880 im Maul.
- **Plattformblock:** im Spiel kein Standort mit Band 1 (SCA ROOM2090 `[7] x[-1700..1850] z[-20000..-12450] floor 19` = fester
  Block auf Band 1, `[4]` derselbe Block auf Band 0; Builder-Lauf BL1: Klemme schiebt Leon nach (−2168,−16250)) — der GUARD-Tod von dort
  ist nur im Riegel `maul` (Lage D, Cut 7, 0 Fehler) gemessen; das ist für einen im Spiel unerreichbaren Ort angemessen.
- **Riegel:** `unit_r41_re2gator_maul` misst in sechs Lagen je Bild mit dem gezeichneten Kiefer (Pose mit allen Haken) und vergleicht mit
  dem CHASE-Bezug; Gegenprobe des Bau-Agenten (ohne Band-Haken 10 Fehler) plausibel zur gemessenen BL2-Ursache, nicht wiederholt.

### 3.2 Regression Punkte 2/3 (Verfolgung, Dash, Putz, Klettern, Kanister, Kiefer-Tod, untergetaucht)
NB5 ändert nur FRESSEN, die Klemme in FRESSEN und Messzeilen; die übrigen Abläufe müssen deshalb gleich bleiben:
- **state.log bitgleich mit Abnahme 4** (`cmp`): D2 (1351 Zeilen), C3 (1651), K (511), N (1351).
- D2: `RE2 DASH Planung: an (-6052,-18113) yaw 1139 -> Start-Gier 624`, `Vordrehung … F481`, `RE2 PUTZ runde 6/6 spawns=9 gesamt=54`,
  `DASH ende … wand_draussen_max=0`, 25 `Putz-Sicht`, 27 Bank-17-SE; Schütteln `schuettel.py fd d_ 536 560 529`: F542 (0,2) F543 (0,0)
  F544 (0,2) F545 (0,3) F546 (1,0) F547 (0,0) F548 (0,1) F549–F551 (0,0) F552 (0,1) F553 (0,1) F554–F555 (0,0) F556 (1,1) — Bild für Bild
  wie Abnahme 4 (Tafeln @0x8009db14/@0x8009daf4).
- C3: `CROSS start=(4450,-11900) ende=(3350,-20700) frames=240`, `KANISTER Zielmodus 1 … F1107`, `angehaengt lokal=(4614,-253,616) F1119`,
  `Kanister-Sitz F1388: angehaengt 270 Bilder, Abweichung max 0`, `GIER-WACHE bis F1302: … max 96 …, Spruenge > 0x60: 0, Befreiungen 0`.
- K: `angehaengt lokal=(4373,-161,11) F18`, `R1 VSync 4 (Zeitlupe) F70`, `R1 ende (Leiche)`, `VSync 2 F108`; Bild `K_kiefer.png`: F72–F80
  Blitz/Splitter am Kopf, ab F82 Leiche mit geplatztem Unterkiefer.
- N: `Sicht F1 untergetaucht`, `Sicht F201 aufgetaucht pos=(5085,-1200,-19880) clip=6 bild=0`.

### 3.3 Neue sichtbare Befunde am Finisher (gemessen; nicht im Bau-Bericht)

**(a) Nach dem P3-Ende steht Leon (bzw. Elza) bis zum Game Over aufrecht neben dem Gator — seit der Bühne in jedem Finisher sichtbar.**
- E1E (`EXIT_AT=902#2090`, letztes 2090-Bild; E1L zeigt den Raumwechsel: state.log springt nach F902 auf ROOM1240): state.log
  F881 `PL(2082,-11098,…) … mo=1 … gr=1` → **F882 `… gr=0`** (Opfer-Modus aus) bis F902; Bild `B41abn5/E1_ende2.png` (und
  `E1_ende.png`): F878–F881 Gator aufgerichtet mit Leon im Maul, **F882–F901 Leon steht in voller Größe aufrecht vor dem Gator**
  (YOU DIED darüber, Abblende F890–F901), **F902 ungeblendet ohne Schrift: Leon steht neben dem Gator** — 21 Bilder (~0,7 s).
- Ebenso A2 (`A2_ende.png`: P3-Ende F651, Leon steht ab F652 bis zur Abblende F670), MN (`MN_p1_ende.png`: F842/F843/F846 Leon groß
  und aufrecht vor dem Gator) und Elza (`EZT2_finisher.png`: F882–F900 Elza steht rot vor dem Gator, F902 ungeblendet).
- Mechanismus (Code, ALT): FRESSEN endet nach P3 = 120 Bildern (`} else if (g->timer <= 340) {` P3, danach
  `re15_player_victim_force_end(); pl->render_scale_q12 = 0; … g->phase = GBP_CHASE;` enemy_ai_boss_gator.c:2599-2604, git blame
  9b6e0b35/c0f98034 2026-09-12/19); der Port-Game-Over wechselt den Raum erst ~20 Bilder später (E1: P3-Ende F881, Raumwechsel nach
  F902). **RE2 beendet P3 nie:** P3 @0x801011c4-e4 = `jal 0x80015cb8` + `FUN_8001a240(em,11,0,0)`, kein Schreiber auf das
  Phasenbyte +6 (§2) — die Kauschleife mit dem Opfer im Maul läuft bis zum Game Over.
- Vorher unsichtbar: im Vorher-Bau lag Leons P3-Lage hinter den Wänden — Abnahme 4 E1 F882–F899 (`B41abn5/E1alt_ende.png`, aus
  `B41abn4/mess/E1/fd`): Gator sinkt, **kein Leon im Bild**; A2-Vorher F648–F664 (`B41abn5/A2alt_ende.png`): kein Leon. Erst die
  Bühne bringt die P3-Lage in jedes Finisher-Bild → der Aufsteh-Moment ist jetzt in JEDEM Finisher zu sehen.
- Gegen M1 der Abnahme 4 ("über P1–P3 **bis zum Game Over** im Maul") und gegen RE2 (Kauschleife ohne Ende). Die Messzeile
  `Maul-Sitz Ende` wird genau im ersten Aufsteh-Bild (F882) geschrieben und prüft danach nichts mehr; integration "belagerung" endet
  bei F890 und wertet die Bilder F882–F890 nicht aus.

**(b) Die Bühne wählt oft die Kamera, die schon läuft — dann gibt es keinen Schnitt, und Gator UND Leon springen sichtbar.**
- exe G1 (`RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_PLAYER_POS=-7950,-25050,2048 RE15_EXIT_AT=1200#2090`, Wiederholung G1F mit
  `FRAMEDUMP=394-420`, bitgleicher Ablauf): Tod durch CHASE-Biss `F399 hp 40 -> -10 | Gator mo=4 af=1 (-5734,-21085) Gier 1499 | Leon
  (-8024,-23663) … daneben -2 (-0 Grad)`; gator_boss.log `FINISHER Buehne … von (-5734,-21085) Gier 1499 -> (-5734,-21085) Gier 351
  Cut 0, Versatz 0 Drehung 1148`, `KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 0 F399 … (Buehne)`; state.log **F395–F403 durchgehend
  `cam=0`** — die Kamera vor dem Biss ist schon Cut 0; F399 Gator `r1499`, **F400 `r351`** (101° in einem Bild), Leon F399
  (−8024,−23663) → **F400 (−118,−25508)** (8100 Einheiten). Bild `B41abn5/G1_gleiche_kamera.png`: F396–F399 Seitenansicht, Biss links
  am Rohr; **F400 dieselbe Kamera, Gator schlagartig frontal gedreht, Leon groß im Vordergrund** (F400–F410), F414 im Maul.
- Prüfer-Sonde `B41abn5/probe/probe_kamreg.c` (gegen die gebaute Bibliothek; BFS über (Zelle, Kamera) mit den RVD-Wechselsätzen
  von ROOM2090 wie re15_aot_scan — Karte der erreichbaren Zonenkamera im Kopf von `probe/kamreg.txt`: Cut 0 SW-Becken, 1 SO, 2/3
  Rampe, 4 NO, 5 NW, 6/7 Westkanal; Gator im Ring 1500/2700/3900 um Leon in 16 Richtungen mit Blick auf Leon, Wurzel im Wasser):
  `24295 Bisslagen …, 0 ohne Buehne; Buehnen-Kamera = Kamera an Leons Lage: sicher 10090, wegabhaengig 49 (je Cut 0..7: 2178 2521 49 0
  2179 1993 591 628); Versatz max 5999, Drehung max 2047, Leon-Sprung min 496 max 11184` — in **41 %** der Wasser-Bisslagen schneidet
  der Finisher nicht, und das Umsetzen (Gator bis 180° gedreht / bis 6000 versetzt, Leon bis 11184 versetzt) passiert in der laufenden
  Kamera.
- Zweiter exe-Fall G2/G2F (`RE15_PLAYER_POS=-5950,-20550,2048`): Tod F440 (CHASE), `Buehne … von (-7224,-18989) Gier 3209 -> (-6792,-19239)
  Gier 3021 Cut 6, Versatz 498 Drehung 188`, state.log F437–F442 durchgehend `cam=6`; Leon F440 (−6505,−15595) → F441 (−6424,−12096)
  (3500 Einheiten), Gator r3209 → r3021; Bild `B41abn5/G2_gleiche_kamera.png`: F440 Biss rechts an der Wand, F441 dieselbe Kamera,
  Leon steht plötzlich links im Vordergrund.
- Der Bau-Agent begründet das Umsetzen ausdrücklich mit dem Schnitt ("Das Umsetzen fällt wie in RE2 in den Finisher-Schnitt",
  gator_re2_r41.c; "sonst sprang der Gator ein Bild lang sichtbar in der alten Kamera", gb_biss_abschluss) — die Suche
  re15_gr2_finisher_buehne schließt die laufende Kamera aber nicht aus (Kosten nur Versatz + 4 × Drehung, alle Zonenkameras).
  RE2 schneidet in P0 immer auf Cut 7/14 (@0x80100fec-ff4). Neu durch die Nachbesserung: die Gator-Drehung/-Versetzung (vorher blieb
  der Gator, wo er biss); Riegel `buehne`/`maul` prüfen nur die Bühnen-Kamera, nicht die Kamera davor.
- Gegenprobe "alt oder neu" (Prüfer-Sonde `probe/probe_altwahl.c`: die alte Finisher-Wahl `re15_gr2_kamera_wahl(BLICK_FINISHER)` mit
  dem Gator am Bissort gegen die Bühne): G1 `alte Wahl … (-5734,-21085) Gier 1499: Cut 0 frontal -1856` / `Buehne: Cut 0`; A2 alt Cut 6
  (frontal 2541) / Bühne Cut 6; E1 alt Cut 2 / Bühne Cut 4. Das Ausbleiben des Schnitts in G1 gab es also schon (dort lag Leon nach dem
  alten P0 bei Gator + RotY(1499) × (7096, −915) ≈ (−11136, −25778), hinter der Westwand x −8900, also unsichtbar — gerechnet, nicht
  gelaufen). **Neu ist der sichtbare Sprung:** die Bühne dreht den Gator in der laufenden Kamera um 101° und setzt Leon groß in den
  Vordergrund. In G2 (alte Wahl ebenfalls Cut 6, frontal 3898; altes P0 hätte Leon nach ≈ (−4849,−12240) gesetzt — gerechnet) gab es
  Leons Sprung ohne Schnitt schon vorher; hinzu kommt jetzt der Gator-Sprung (498/188).

**(c) In P1 der Cut-4-Bühne (E1-Türweg, MN, Elza-Türweg) ist das Opfer verdeckt — nicht von Spinnen, sondern von einer Raum-Leiche.**
- E1 F628–F641 (`B41abn5/E1_p1_zoom.png`, `E1_p1_zoom4.png`, Ausschnitt ×4, Aufhellung ×3): vor Leon liegt die schwimmende
  Zombie-Leiche, Leon ist nur als dunkler Rest darunter zu ahnen; erst F642 erscheint er im Maul. MN F588–F600 gleich
  (`MN_p1_ende.png`), Elza F628–F640 nur die roten Beine unter der Leiche (`EZT2_finisher.png`). In A2 (Cut 6) und G1 (Cut 0) ist das
  Opfer in P1 zu sehen.
- Verdecker = Raum-Objekt `Obj_model_set[3] id=0x03 type=0 pos=(-25,-1740,-7423) rot=(0,1656,0)` aus der ROOM2090-SCD (debug.log E1);
  Prüfer-Sonde `probe/probe_proj.c` (Projektion wie der Zeichner, Cut 4, Kamera (−7740,−3132,−6462)): Objekt → Bild (98,116), Leons
  P1-Wurzel (−457,−1186,−8807) → (138,128), Füße → (139,158). Die Spinnen stehen in diesen Bildern bei (6182,−13482)/(6182,−19118)
  (state.log F628), nicht vor Leon — die Angabe des Bau-Agenten ("Raum-Spinnen … vor Leon; Spur F entfernt sie") trifft den Verdecker
  nicht, und Spur F muss diese Leiche nicht entfernen.
- Die Messzeile/Riegel prüfen "Leon im Bild" nur als Projektion seiner Wurzel (keine Verdeckung) — `Leon im Bild 244/254` ist hier
  ein Stellvertreter-Maß.

### 3.4 Kamera-Maßstab der Bühnen (Nebenbefund, kein Mangel nach Entscheid B8)
`KAMERA FINISHER FINISHER … massstab` je Lauf: E1/EZT2 Cut 4 779, MN Cut 4 774, A2 Cut 6 712, **BL Cut 2 573**, S3 Cut 5 1021, G1 Cut 0
1318 (RE2 Cut 7: 1354). BL (`B41abn5/BL_finisher.png`, `BL_zoom.png` ×4): der Finisher ist im Normalbild sehr klein (Gator und Leon im
Bildmittelstreifen), im Ausschnitt korrekt (Leon F544–F580 im Maul, F620 Wurf, F700/F760 Beine aus dem Maul) — und ab F782 steht Leon
auf (§3.3 a). Entscheid B8 setzt nur die RE2-Obergrenze; die Entscheidungsfrage des Bau-Agenten an den Nutzer (§13.8) deckt das ab.

## 4. Gates

### 4.1 RE-Gate
- Diff-Grep (nur `+`-Zeilen) nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|XXX|vorerst|geschaetzt|ungefaehr|approx":
  Nachbesserung 5 (`git diff 71192fbc..HEAD -- re15_port`) **0 Treffer**, Zweig gesamt (`master...HEAD`) **0 Treffer**. Kein neues
  `getenv` in der Nachbesserung 5 (0 Treffer) — kein Env-Schalter als Abschluss.
- Konstanten der Nachbesserung 5 und ihre Belege: Zusatzwinkel 0 (@0x80101050-54 ✓), keine Überblendung (FUN_8001a330(em,4,0,0)
  @0x801010e8-f8 / @0x8001a364 ✓), Kanister aus (@0x80101008-0c ✓), Umsetzen (@0x80101048-4c ✓, Lage als **PORT-WAHL** nach Nachtrag 6
  gekennzeichnet), keine Klemme in FRESSEN (FUN_80015cb8 ohne Kollision @0x80015d68/@0x80015d90 ✓), Band −(y/1800) (@0x80038460-bc ✓),
  Spielerradius 450 = 0x1c2 (Verweis auf den Beleg im Boss-Modul GR2_LEON_MIN/gb_koerper_kontakt; nicht neu nachgeprüft), GR2_FIN_N/P2/P3 = 14/120/59 Bilder aus den Clip-Längen (FRESSEN-Fall,
  @0x801010fc-104), GR2_FIN_WEIT 27000 = Becken-Diagonale √(16100² + 21600²) = 26940 (nachgerechnet ✓), Suchraster 250 und Kosten
  Versatz + 4 × Drehung als Teil der PORT-WAHL "wie die Intro-Lage", Riegel-Schranken 32/20/8 bzw. Integration 4/8 als Messtoleranzen mit
  gemessenen Werten, unit_gator_fress-Schranke 5 → 16 mit Rundungsrechnung (3 Q12-Drehungen × 3,6 × 1,5; gemessen −8) — nachvollziehbar.
- Kleiner Hinweis: `pl->y = 0` in P0 (Leon auf den Beckenboden) ist im Kommentar begründet ("A2: y 0", RE2 P0 schreibt nur x/z), aber
  nicht ausdrücklich als PORT-WAHL benannt — die Höhe ist der Beckenboden (Geometrie), kein Stellwert; kein Mangel.
- Selbst disassembliert (§2): FRESSEN P0/P1/P2/P3 (EM23), FUN_8001a330, FUN_80015cb8 (RE2-EXE), FUN_80031c44-Schwanz, Band-Rechnung
  @0x80038440-c4, Kamera-Apply @0x800214f4-514 (RE1.5-EXE). **Keine falsche Adresse.**
- "Erklärt der Fix den Befund?" Für M1 der Abnahme 4: ja — die vier gemessenen Ursachen (Neigung, Klemme, Ort, Band) sind je einzeln
  geändert, und Leon liegt in P2/P3 aller gemessenen GUARD- und CHASE-Tode im gezeichneten Maul (§3.1). Die drei neuen Befunde §3.3
  sind Folgen der gewählten Form (Bühne ohne Schnitt-Garantie, Bühne ohne Rücksicht auf Raum-Objekte) bzw. werden durch sie erst sichtbar
  (P3-Ende); RE2 macht in allen drei Punkten etwas anderes (Schnitt @0x80100fec-ff4 immer, P3 ohne Ende @0x801011c4-e4).

### 4.2 Vertrag
- Bank-9-Bits / Nachrichten-Ids: keine (`git diff 71192fbc..HEAD -- re15_port` ohne game_flag_set/g_game.flags/MSG_ID).
- Pfade (`git diff --name-only master...HEAD`): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine
  CMakeLists.txt; RE2-Daten nur shared_assets/RE2/ (seit Abnahme 0 genannt). Nachbesserung 5 berührt nur Spur-B-Dateien
  (enemy_ai_boss_gator.c = Eigentümer B, gator_re2_r41.c, gator_re2_pc.c, zwei Header) und Riegel (test_r41_re2gator.c,
  probes/r41_re2gator.cmake, integration/test_r41_re2gator.cmake, probe_gator_fress.c mit Beleg); keine neuen Haken in gemeinsamen Dateien.
- Tests: neu `unit_r41_re2gator_maul` (messend, gezeichneter Kiefer je Bild, sechs Lagen), `unit_r41_re2gator_buehne` (Garantie der
  Bühne), integration "belagerung" bis F890 mit `r41b_maul_sitz`. Parallel-fest (eigener Laufordner/eigene exe-Kopie wie bisher),
  Linux-fest (keine neuen .exe-/Backslash-Pfade, `re15_squareroot0` statt libm). **Lücken:** kein Riegel prüft die Bilder nach dem P3-Ende
  bis zum Raumwechsel (§3.3 a), die Kamera VOR der Bühne (§3.3 b) oder die Sichtbarkeit des Opfers (nur Projektion, §3.3 c).

### 4.3 Nebenwirkungen
- Beide Szenarien: Elza über den echten Türweg 20A1 → 2091 bitgleich mit Leon (§3.1); die Befunde §3.3 a/c gelten für Elza ebenso.
- Punkte 2/3 und Nachbarabläufe: D2/C3/K/N state.log bitgleich mit Abnahme 4 (§3.2); Integrations-Querriegel §5 grün.

## 5. Integration (gezielt, am gebauten Stand)
`ctest --test-dir re15_port/build -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate|r37_itemband|r36_tueren|r37_re2zombie)$"
--timeout 1800 -j 3 --output-on-failure` (15:25:09–15:39:53, `B41abn5/ctest_integ.log`) → **100% tests passed, 0 tests failed out of 7**
(r41_re2gator 282,5 s, r37_itemband 883,8 s, r35_granate 180,8 s, r38_wasser 182,2 s, r36_schuss 152,5 s, r36_tueren 35,7 s,
r37_re2zombie 55,2 s). Lauf `build/tests/integration/r41_re2gator_wd/lauf_2b0b130b`: belagerung `Maul-Sitz Ende F882: Bilder 254
(P1 14 P2 120 P3 120), Abweichung max 0 (F628), Kiefer-Abstand max 2 (F763), Kopf im Bild 254/254, Leon im Bild 244/254 + oben hinaus
10 (Cut 4), im Wasser 254/254`; nische `Maul-Sitz F637: Bilder 240 (P1 14 P2 120 P3 106), … Leon im Bild 240/240 (Cut 6), im Wasser
240/240`. Rote Fenster-Haken: keine.

## 6. Nebenbefunde (keine Mängel dieser Abnahme)
- **Elza nach dem Game Over (EZT, `EXIT_AT=1300` ohne Raum):** nach F902 kein Raumwechsel und keine state.log-Zeilen mehr, Lauf bis zum
  timeout (rc = 124) — die Game-Over-/Titelfolge des Ports wartet (Entscheid B6: "Die Game-Over-Folge danach bleibt die des Ports");
  Leon (E1L) landet nach F902 in ROOM1240. Kein Spur-B-Befund.
- **Raum-Spinnen** töten Leon in offenen-Wasser-Läufen vor dem Gator (S1 F632, S2 F442, je −20) und beißen die Leiche nach dem Finisher
  weiter (G1 F672, S3 F681/F685) — Spur F entfernt sie.
- **Kamera vor dem Biss bei Lagen per RE15_PLAYER_POS:** ohne Laufweg bleibt die Kamera nach dem Intro auf der "vorher"-Kamera (BL: Leon
  an der Rampe, Kamera Cut 0 zeigt das SW-Becken ohne Leon/Gator, F522–F526) — Testhebel-Folge, im Spiel läuft die Zonenkamera mit.
- **Doku:** `unit_r41_re2gator_buehne` misst heute `Versatz max 9216` (Dossier §13.5: 8984 bzw. §13.4: 9996) — Schranke 27000 hält.

## 7. Behauptungen des Bau-Agenten — Abgleich am gebauten Stand
| Behauptung | gemessen | Ergebnis |
|---|---|---|
| Nach jedem tödlichen Biss liegt das Opfer über P1–P3 bis zum Game Over im gezeichneten Maul (CHASE, GUARD Rampe Ost/Mitte/Block, Leon/Elza) | P2/P3 im Maul: E1, MN, A2, BL, G1, Elza-Türweg (Bilder §3.1); P1 = Opfer vor dem Gator (RE2-Form), in Cut 4 aber von der Raum-Leiche verdeckt (§3.3 c); **nach P3 steht das Opfer bis zum Game Over auf** (§3.3 a) | ✗ (bis P3 ✓, "bis zum Game Over" ✗) |
| Ursachen (1) pitch −350, (2) Klemme, (3) Ort hinter der Ostwand, (4) Band 1 + Spieler-Wandklemme | Code gelesen (P0 nullt pitch/arc/jaw, gb_klemme_aus FRESSEN, Bühne, Band-Haken); Riegel maul 0 Fehler; BL (Band-Fall) Kiefer 2, Wasser 254/254 | ✓ |
| RE2 P0 @0x80101048-58, FUN_8001a330(em,4,0,0) @0x801010e8-f8, Blend-Zähler @0x8001a34c-64 | selbst disassembliert (§2) | ✓ |
| Spieler-Wandklemme FUN_80031c44 @0x80031d70 läuft auch im Tod; Band −(y/1800) @0x80038460-bc | selbst disassembliert (§2) | ✓ |
| Finisher-Bühne PORT-WAHL, Form RE2-P0, "Das Umsetzen fällt wie in RE2 in den Finisher-Schnitt" | in E1/MN/A2/BL/Elza Schnitt im Biss-Bild → neue Kamera ✓; **G1: Bühne = laufende Kamera, kein Schnitt, Gator dreht 101° und Leon springt 8100 sichtbar; G2 ebenso (Leon 3500)**; Sonde: 41 % der Wasser-Bisslagen (§3.3 b) | ✗ |
| Bühne prüft jedes Bild, Suchgrenze 27000, 0 Lagen ohne Bühne | Code gelesen; Riegel buehne `2128 …, ohne Buehne 0, falsch 0`; Sonde kamreg `0 ohne Buehne` in 24295 Lagen | ✓ |
| Messzeile "Maul-Sitz" + Summenzeile | in allen Läufen vorhanden (§3.1); Werte reproduziert | ✓ (prüft Sichtbarkeit nur als Projektion) |
| Riegel maul: 6 Lagen, 254 Bilder, Maul ≥ 50 % P2 106 / P3 120, Kiefer ≤ 3, Klemme 0 | `unit_r41_re2gator_maul` -V wörtlich so | ✓ |
| Riegel buehne 2128 Lagen 0 ohne Bühne; Sonde 21320/24256 | Riegel ✓ (Versatz max 9216 statt 8984); Sonde nicht wiederholt | ✓ |
| exe E1, MN3, BL3, A2, EZ3: je 254 Bilder, Kiefer 2, Wasser 254/254, Kopf 254/254 | E1/MN/A2/BL/Elza-Türweg (statt EZ3-Sprung) gleich | ✓ |
| integration "belagerung" bis F890, prüft den Finisher nach dem Tod; "nische" ebenso | 7/7 grün; belagerung `Maul-Sitz Ende F882 …`; die Bilder F882–F890 (Opfer steht) werden nicht geprüft | ✓ (Lücke §4.2) |
| Punkte 2 und 3 nicht berührt | D2/C3/K/N state.log bitgleich mit Abnahme 4; Schütteln/Putz/Dash/Klettern/Kanister/Kiefer-Tod wie dort | ✓ |
| unit_gator_fress P3-Schranke 5 → 16 mit Beleg | `dRef=(-8,-1,-2)`, `776/776`; Rundungsrechnung nachvollzogen | ✓ |
| OFFEN: Spinnen stehen in P1 vor Leon (E1/MN3/EZ3), Spur F entfernt sie | Verdecker ist die Raum-Leiche `Obj_model_set[3]` (−25,−1740,−7423), nicht eine Spinne (§3.3 c) | ✗ (falsche Zuordnung) |
| Unit 114/114, Integration 7/7 | Unit 121/121 (Obermenge), Integration 7/7 | ✓ |

## 8. Urteile je Nutzer-Punkt
| Punkt (Wortlaut AUFTRAG.md) | Urteil | Beleg |
|---|---|---|
| 1. "Der Aligator in ROOM 2090 hat bereits einen finisher in Resident Evil 2 … Ich will das du den Finisher und fehlende Animationen aus Resident Evil 2 extrahierst!" | **teilweise** | M1 der Abnahme 4 im Kern behoben: nach GUARD- und CHASE-Toden liegt das Opfer in P2/P3 im gezeichneten Maul (E1-Türweg F642–F880, MN, A2, BL, G1, Elza-Türweg; Messzeile `Maul-Sitz Ende … Kiefer-Abstand max 2–3, Wasser 254/254`; Riegel maul 0 Fehler; RE2-Stellen selbst disassembliert). Gegen den Wortlaut ("den Finisher aus RE2") bleiben drei sichtbare Abweichungen: **(M1)** nach dem P3-Ende steht das Opfer bis zum Game Over aufrecht neben dem Gator (E1 F882–F902, A2 F652–F670, MN F842–, BL F782–, Elza F882–F902; RE2-P3 @0x801011c4-e4 endet nie) — durch die Bühne jetzt in jedem Finisher zu sehen; **(M2)** die Bühne nimmt oft die laufende Kamera: kein Schnitt, Gator dreht/versetzt sich und Leon springt im Bild (G1 F399→F400: 101°, 8100 Einheiten; G2 F440→F441: Leon 3500; Sonde 41 % der Wasser-Bisslagen) — RE2 schneidet in P0 immer; **(M3)** in P1 der Cut-4-Bühne (Türweg-Ankunft, Elza) verdeckt die Raum-Leiche `Obj_model_set[3]` das Opfer. Die RE2-Clips selbst (6/3/4/1/5/11/2/3/9/10) laufen unverändert (§3.2) |
| 2. Verfolgung wie RE2 (Animation, Schreie, Beschleunigung an gleichen Stellen), Wände wackeln + Putz, Klettern bleibt | **erfüllt** | unverändert gegenüber Abnahme 4 (dort erfüllt): D2/C3/N state.log bitgleich, Dash-Planung/Vordrehung, `PUTZ runde 6/6 … gesamt=54`, 25 Putz-Sicht, 27 Bank-17-SE, Schütteln Bild für Bild (F542–F556) wie die Tafeln @0x8009db14/@0x8009daf4, `GIER-WACHE … Spruenge > 0x60: 0`, `CROSS … frames=240`, untergetaucht bis F201 |
| 3. Kanister aufnehmen "GENAUSO wie im original", im Maul beschießbar, Sterbeanimation mit geplatztem Kiefer wie RE2 | **erfüllt** | unverändert gegenüber Abnahme 4 (dort erfüllt): C3/K state.log bitgleich, `angehaengt lokal=(4614,-253,616) F1119`, `Kanister-Sitz F1388: 270 Bilder, Abweichung max 0`, K `R1 VSync 4 (Zeitlupe) F70` → Splitter F72–F80, Leiche mit geplatztem Unterkiefer ab F82 (`K_kiefer.png`); FRESSEN P0 schaltet den Kanister wie RE2 aus (@0x80101008-0c) |

## 9. Mängel (nummeriert, nachprüfbar)

1. **M1 (Punkt 1; Mechanismus ALT, seit der Bühne in jedem Finisher sichtbar): nach dem P3-Ende steht das Opfer bis zum Game Over
   aufrecht neben dem Gator.**
   - Nachprüfung: Basis wie §3 + `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1 RE15_EXIT_AT=902#2090
     RE15_FRAMEDUMP=870-902:fd/x_` → state.log F881 `… gr=1`, F882 `… gr=0`; Bilder F882–F901 Leon steht in voller Größe vor dem Gator
     (YOU DIED, Abblende), F902 ungeblendet ohne Schrift (`B41abn5/E1_ende2.png`); dasselbe A2 (F652–F670, `A2_ende.png`), MN (F842–,
     `MN_p1_ende.png`), BL (F782–, `BL_finisher.png`), Elza-Türweg (F882–F902, `EZT2_finisher.png`). Vorher (Abnahme 4) lag die P3-Lage
     hinter den Wänden: E1 F882–F899 ohne Leon (`E1alt_ende.png`).
   - Ursache (Code): FRESSEN endet nach 120 P3-Bildern (`g->timer <= 340`, enemy_ai_boss_gator.c:2569) mit
     `re15_player_victim_force_end(); pl->render_scale_q12 = 0; … g->phase = GBP_CHASE;` (Z. 2599-2604); der Port-Game-Over wechselt
     den Raum erst ~20 Bilder später. RE2: P3 @0x801011c4-e4 (`jal 0x80015cb8` / `FUN_8001a240(em,11,0,0)`) ohne Phasen-Schreiber.
   - Verlangt: das Opfer bleibt bis zum Raumwechsel im Maul (Kauschleife wie RE2, Opfer geparkt), kein Bild mit stehendem Opfer nach
     dem tödlichen Biss — Leon und Elza; Riegel/integration "belagerung" bis zum letzten 2090-Bild vor dem Raumwechsel (F902) mit
     Opfer-Modus/Maul-Sitz je Bild; Bildbeleg F878–F902 vorher/nachher.

2. **M2 (Punkt 1; Schnitt-Lücke teils ALT, der sichtbare Gator-/Leon-Sprung neu durch die Bühne): Wählt die Bühne die laufende Kamera,
   gibt es keinen Schnitt, und das Umsetzen von Gator und Leon ist zu sehen.**
   - Nachprüfung: `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_PLAYER_POS=-7950,-25050,2048 RE15_EXIT_AT=420#2090
     RE15_FRAMEDUMP=394-420:fd/g_` → gator_boss.log `FINISHER Buehne … von (-5734,-21085) Gier 1499 -> (-5734,-21085) Gier 351 Cut 0,
     Versatz 0 Drehung 1148`; state.log F395–F403 `cam=0`; Gator `r1499` (F399) → `r351` (F400); Leon (−8024,−23663) → (−118,−25508);
     Bild `B41abn5/G1_gleiche_kamera.png` (F399 Seitenansicht, F400 dieselbe Kamera, Gator frontal gedreht, Leon groß im Vordergrund).
     Zweiter Fall G2F (`RE15_PLAYER_POS=-5950,-20550,2048`, `EXIT_AT=460#2090 FRAMEDUMP=434-460:fd/h_`): `cam=6` F437–F442, Bühne Cut 6
     (Versatz 498, Drehung 188), Leon (−6505,−15595) → (−6424,−12096) im selben Bild (`G2_gleiche_kamera.png`).
     Häufigkeit: Sonde `B41abn5/probe/probe_kamreg.c` (gegen die gebaute Bibliothek) → `sicher 10090` von 24295 Wasser-Bisslagen,
     Drehung bis 2047, Versatz bis 5999, Leon-Sprung bis 11184. Alt/neu: `probe/probe_altwahl.c` — die alte Finisher-Wahl hätte in G1/G2
     ebenfalls Cut 0/Cut 6 genommen (Schnitt-Lücke ALT); neu sind die Gator-Drehung/-Versetzung in der laufenden Kamera und Leons
     sichtbare Landung (G1: vorher hinter der Westwand).
   - Verlangt: das Umsetzen (Gator + Opfer) ist nie in der laufenden Kamera zu sehen — wie RE2, das in P0 immer schneidet
     (@0x80100fec-ff4); z. B. die laufende Kamera als Bühnen-Kamera ausschließen (Form frei, PORT-WAHL benennen). Riegel: Bühnen-Kamera ≠
     Kamera an Leons Lage für Bisslagen in allen Zonenkamera-Bereichen (Wasser + Rampe, Kamera an Leons Lage aus den RVD-Sätzen),
     exe G1 vorher/nachher F396–F414.

3. **M3 (Punkt 1, Teil "der Finisher-Schnitt zeigt Gator und Opfer"): In P1 der Cut-4-Bühne ist das Opfer von der schwimmenden
   Raum-Leiche verdeckt.**
   - Nachprüfung: E1 (§3) F628–F641 (`B41abn5/E1_p1_zoom4.png`: Leon nur als dunkler Rest unter der Leiche, erst F642 im Maul), MN
     F588–F600, Elza-Türweg F628–F640 (nur rote Beine). Verdecker `Obj_model_set[3] id=0x03 type=0 pos=(-25,-1740,-7423)` (debug.log),
     Projektion Cut 4: Objekt (98,116), Leons P1-Wurzel (138,128) (`probe/probe_proj.c`); die Spinnen stehen bei x 6182.
   - Verlangt: das Opfer ist in P1 sichtbar (Bühne berücksichtigt die Raum-Objekte im Sichtstrahl, oder belegte Abhängigkeit, dass
     diese Leiche vor dem Kampf nicht mehr im Becken liegt — dann im Dossier mit Spur F festhalten und messen); Dossier §13.7 richtigstellen
     (Verdecker = Raum-Leiche, nicht Spinne).

Hinweise an den Orchestrator (keine Mängel dieser Abnahme):
- **H1** (unverändert seit Abnahme 3): RE2-Halsführung/Zielmodus (Root @0x80100330-74 → FUN_80017d98 / FUN_800177c0) OFFEN, Entscheid
  zum Part-5-Gier-Kanal steht aus.
- **H2** (ALT, Abnahme 4): GUARD-Hochbiss ohne Blick auf Leon — das Biss-Bild in der alten Kamera zeigt den Biss 50–128° neben Leon
  (E1 F627 64°, MN F587 128°); der Finisher hängt nicht mehr davon ab.
- **H3:** Bühnen-Maßstab 573–1318 gegen RE2 Cut 7 1354 (BL im Normalbild sehr klein) — nach B8 zulässig, Nutzerfrage des Bau-Agenten
  (§13.8) offen.
- **H4:** `pl->y = 0` in P0 ist begründet, aber nicht als PORT-WAHL benannt.

## 10. Gesamturteil

`bestanden = false`. Der Mangel der Abnahme 4 ist im Kern behoben und gemessen: nach tödlichen GUARD-Hochbissen (Türweg 20A0 → Ostrampe,
Rampenmitte, Plattformblock im Riegel) und CHASE-Bissen liegt das Opfer in P2/P3 im gezeichneten Maul, für Leon und Elza, im Wasser und im
Bild (Messzeile und Riegel `maul` mit dem gezeichneten Kiefer; RE2-Belege selbst disassembliert, keine falsche Adresse; Unit 121/121,
Integration 7/7; Punkte 2 und 3 bitgleich mit Abnahme 4 und weiter erfüllt). Die gewählte Form (Bühne) erzeugt bzw. zeigt aber drei
sichtbare Abweichungen vom RE2-Finisher, die weder der Bau-Bericht noch die Riegel erfassen: das Opfer steht nach dem P3-Ende bis zum Game
Over auf (M1, in jedem Finisher), das Umsetzen ist ohne Schnitt zu sehen, wenn die Bühne die laufende Kamera nimmt (M2, in 41 % der
Wasser-Bisslagen der Sonde, exe G1/G2), und in der Türweg-Bühne verdeckt eine Raum-Leiche das Opfer in P1 (M3). Punkt 1 bleibt deshalb
teilweise.

Prüfer-Dateien (nicht im Baum): `B41abn5/` (lauf.sh, lauf_elza.sh, bisse.py, sheet.py, crop.py, schuettel.py, Läufe `mess/E1|E1E|E1L|MN|A2|
EZT|EZT2|D2|C3|K|N|S1|S2|S3|G1|G1F|G2|G2F|G3|BL`, Sonden `probe/probe_kamgleich.c|probe_kamreg.c|probe_proj.c|probe_altwahl.c` mit `kamreg.txt`,
Bildbögen E1_schnitt/E1_p1_zoom/E1_p1_zoom4/E1_p23_zoom/E1_p3_zoom4/E1_ende/E1_ende2/E1alt_ende/A2_schnitt/A2_ende/A2alt_ende/MN_p1_ende/
EZT2_finisher/K_kiefer/G1_gleiche_kamera/G2_gleiche_kamera/BL_finisher/BL_zoom .png, Logs build/configure/ctest_unit/ctest_integ/unit_maul_V).
