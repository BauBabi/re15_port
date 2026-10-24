# Runde 41 — Spur B "re2gator" — Abnahme 2 (unabhängig)

Prüfer: Abnahme-2-Agent, 2026-10-24 (ab 07:20). Baum `.claude/worktrees/r41_re2gator`, HEAD 06bc6578
(Zweig r41/re2gator, merge-base mit master 116e8b0f; master selbst steht auf bbcdefef). Geprüft wird NUR der gebaute
Stand; nichts aus dem Dossier wird ungemessen übernommen. Messdateien: Prüfer-Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/B41abn2/`
(Läufe unter `mess/<Lauf>/`, eigene exe-Kopien `re15_pc_b41a2_<Lauf>.exe` bzw. `re15_pc_b41a2z_<Lauf>.exe` für Elza).

(Fortlaufend geschrieben; Gesamturteil am Ende.)

## 1. Lektüre + Bau + gezielte Riegel

- Gelesen: AUFTRAG.md (Wortlaut + Nachtrag 4 + Entscheide B1–B8), VERTRAG.md, UEBERGABE.md, B_abnahme_1.md (Mängel
  M1–M4), Dossier B_re2gator.md §10 (Nachbesserung 2) und §2.5/§2.6, `git log --oneline master..HEAD` (8 neue Commits
  seit der Abnahme 1: 193542bc … 06bc6578), `git diff 393bb59b..HEAD --stat` (17 Dateien, +1560/−175), kompletter
  Code-Diff der Nachbesserung 2 gelesen: gator_re2_r41.c (+438: Aufnahme Sub 2→1, hysterese_ruht, biss_erlaubt,
  Finisher-Halt, Intro-Lage, Dash-Planung + Vordrehung, Wand-Messzeile), enemy_ai_boss_gator.c (+73: Push-Kreise auf
  Dateiebene, gb_becken_draussen, 3 Quellen-Anmeldungen im INIT, intro_lage vor intro_start, dash_planen statt
  gb_seg_frei, biss_erlaubt vor 4 Biss-Starts), Header (+26), re2_fx.c (Kommentar Op 73), Tests/Probe.
- Bau: `bash re15_port/tools/local_build.sh configure` → `LOCAL-BUILD-OK (configure)` (07:22:50); erster `build` →
  "ninja: no work to do"; danach alle Spur-B-Quellen per `touch` (nur mtime) und erneut `local_build.sh build` →
  785 Schritte, `LOCAL-BUILD-OK (build)`, EXIT 0 (07:23:59). Einzige Warnungen: main.c 8803/10132/10512
  (`-Wdiscarded-qualifiers`, Altbestand wie in Abnahme 0/1), keine in Spur-B-Dateien. re15_pc.exe 07:23.
- Gezielte Unit-Riegel = Regex der Abnahme 1 + Bau-Agent (`r36_kartenbits`) + zusätzlich Module, die g_death_*/den
  Gator-Raum berühren (`text_freeze`, `r34_reaktion`, `load_after_death`, `r35_affen_finisher`, `r36_tueren_.*`,
  `r37_itemband`, `r36_kartenitems`): `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|…|
  r36_kartenbits|text_freeze|r34_reaktion|load_after_death|r35_affen_finisher|r36_tueren_.*|r37_itemband|
  r36_kartenitems)$|^probe_(re2z_stomp|crow_kill)$" --timeout 600 -j 4` → **100% tests passed, 0 tests failed out of
  101** (116,4 s, 07:24:44–07:26:41; unit_gator_sweep 116,3 s unter Last). Ausgaben der 11 Spur-Riegel
  (`B41abn2/unit_r41_verbose.txt`, `ctest -R "^unit_r41_re2gator_" -V` → 11/11):
  - aufnahme: `Paddeln 12/12 -> Stufe 1 Clip 0 Blend 32, Stufe 1..6 39/39 Clip 0, Biss 0, Stufe 7 Zone 0 30/30 Clip 0,
    Zone 2 Paddeln 12/12, Biss Stufe 7 nach 0 Takten`
  - wand: `intro Start (-7345,-13277) yaw 576 Clip6 125 Clip3 150 -> yaw 1600 (Peilung Leon 1505, Abw 95), draussen 0,
    Kopf x min -6889 | dash Vordrehung 6 Bilder 1024 -> 512, ab f40 Clip2 150 Clip3 150, Bahn (797,-5119), draussen 0,
    Kopf x min -8701, yaw 1536`
  - kamera: `FINISHER (FRESSEN P0): ... gewaehlt Cut 0` / `FINISHER-Halt: 400/400 Takte Cut 0 gesperrt, RE1.5-Todesbild
    (Orbit/Weiss/Schwarz) aus, Leon-HP -49`
  - kanister: `Zeitlupe VSync 4 ueber 38 Takte`, `Stufen 13/6/6/7/7/80 ... getragen ab 40`; kiefer/putz/sockel/intro/
    dash/clips/modell wie Abnahme 1.

## 2. RE-Stichproben (selbst disassembliert, `re2_disasm.py`, EM23_OVL_0000.BIN bzw. info/re2leon/PSX.EXE)

Alle zitierten Stellen der Nachbesserung 2 enthalten das Behauptete:
- **Sub-1/2-Pre @0x80100964** (`dis 0x80100964 180`): `jal 0x801012fc` @0x80100974 (T1-Schärfung zuerst);
  Zone-1-Block `lbu v1,538 / addiu v0,zero,1 / bne v1,v0,0x80100b20` @0x801009c8–d0; Bit 1 (`jal 0x80077360` a0 =
  0x800d4874, a1 = 1) → `+0x1C0 |= 4`, Ziel (x, y−800, obj[1].z) @0x80100a1c–70; Bit 0 `jal 0x80077360 / addu a1,zero,zero
  / beq v0,zero,0x80100b20` @0x80100ab8–c8; Fenster `addiu v0,v0,-8601 / sltiu v0,v0,0x18f / beq v0,zero,0x80100c08`
  @0x80100ae4–ec (verfehlt → Ende OHNE Hysterese); Stufe ≠ 0 → `bne v0,zero,0x80100c08` @0x80100afc; Stufe 0 →
  `sb v0(1),550(s0)` @0x80100b08, Sub 2 → `addiu v0,zero,257 / j 0x80100c04` @0x80100b14–18 → `sw v0,4(s0)` @0x80100c04;
  Hysterese-Sperre `lbu a0,550 / beq a0,zero,0x80100b38 / addiu v0,zero,7 / bne a0,v0,0x80100c08` @0x80100b20–30. ✓
- **Dispatcher** `lbu v0,5(a0)` @0x801007b8 nach dem Pre (Tafel @0x8010463c), Main-Tafel @0x8010465c; **Sub-1-Main**
  @0x80100c20: bei +6 == 0 `addu a1,zero,zero / addiu a2,zero,32 / jal 0x8001a330 / addu a3,zero,zero` @0x80100c34–40. ✓
- **T1 FUN_801012fc**: `lbu v1,550(a0) / beq v1,zero,0x80101378 / addiu v0,zero,7 / bne v1,v0,0x80101380` @0x80101360–70,
  `addiu v0,zero,1 / sb v0,548(a0)` @0x80101378–7c. ✓ (Zusätzlich gelesen: davor `lbu v0,448(a0) / andi v0,v0,0x4 / bne
  → keine Schärfung` @0x8010134c–58 — siehe Befund §3.3 d.)
- **FRESSEN P0** (Sub 4 Main @0x80100f58): `lui a0,0x800d / addiu a0,a0,-1064 / lw / ori v0,v0,0x100 / sw` @0x80100fbc–d0,
  Cut 14 nur bei Bit 0x40000000 + Flag 16 (@0x80100fd4–ec), sonst `addiu a0,zero,7` @0x80100ff0, `jal 0x8002c7ac`
  @0x80100ff4. Wortsuche über das ganze Overlay nach Immediate 0xFEFF: `addiu v1,zero,-257` NUR @0x80100910,
  @0x80102688, @0x801028ac, @0x80102b1c (+ zwei `lui …,0xfeff` @0x80102888/@0x80102af4 = obere Halbwort-Masken, nicht
  Bit 0x100) — kein Freigabe-Pfad in Sub 4. ✓
- **Leon-Lage im FRESSEN** `addiu v0,zero,-23078 / sw v0,56(s0)` @0x80101048–4c, `lw v0,56(s0) / addiu v1,zero,-915 /
  sw v1,0x800cfc38 / addiu v0,v0,10643 / sw v0,0x800cfc30` @0x8010106c–84 → (−12435, −915). RVD selbst gelesen
  (`rvd_40a0.py` + eigener Dump aller 36 Sätze ab RDT 0x25C): der Punkt liegt in 0x3EC (6→7) und in den Anker-Sätzen
  0x400 (7→0), 0x43C (8→0), 0x464 (9→0) (jede Kamera hat als ersten Satz (N→0) = Eigenbereich), NICHT in 0x428 (7→8)
  (Westkante bei z −915 ≈ x −11905) und nicht in 0x414 (7→6) — Cut 7 ist dort Leons Zonenkamera. ✓
- **Op 73**: `table 0x8009d980` → [0x8009d98c] = 0x8002387c (= 0x8009d868 + 73×4), [0x8009d99c] = 0x80023c70 (Platz 77). ✓
- **ROOM40A0-SCD** (`re2_scd_walk.py`, selbst gefahren): sub10 `Set 22 05 01 01` @0x0659 (Bit 1 = Kanister fällt) und
  `Set 22 05 00 01` @0x0712 (Bit 0 = liegt); kein `Set 22 05 00 00`/`22 05 01 00` im ganzen Skript; sub11
  `Pos_set 32 00 cd 11 f8 f8 c3 b8` @0x0736 = (4557, −1800, −18237). ✓
- **Schüttel-Tafeln** (PSX.EXE, `read --w 1`): Modus 1 @0x8009daf4 = (0,1)(0,0)(0,0)(0,1)(1,1)(0,0)(0,1)(0,0)(0,0)(0,0)
  (0,1)(0,1)(0,0)(0,0)(1,1)(0,0); Modus 2 @0x8009db14 = (0,2)(0,0)(0,2)(0,3)(1,0)(0,1)(0,2)(0,0)(0,2)(0,0)(2,1)(0,2)
  (0,2)(0,0)(1,1)(0,2) (Vergleich mit der Messung §3.2).

## 3. exe-Läufe (eigene Kopien `re15_pc_b41a2_<Lauf>.exe`, Ordner `B41abn2/mess/<Lauf>/`)

Basis (`B41abn2/lauf.sh`): `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
SDL_ASSERT=always_ignore RE15_STATE_LOG=state.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=audio.raw`, `timeout`, Umgebung
des Aufrufers ohne RE15_GB_TEST/RE15_GATOR_KANISTER/RE15_PLAYER_POS/RE15_GB_STUMM; Elza (`lauf_elza.sh`) mit
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`. Auswertung `statelog.py` (Clip-Läufe
Slot t=23), `schuettel.py` (Hintergrundversatz), `sheet.py` (Bildbögen; Aufhellung nur zur Sichtung).
Hinweis Messfalle: der Framedump zählt ab Raumeintritt; läuft ein Lauf über das Game Over hinaus, überschreibt das
neue Spiel (ROOM1240-Vorspann) die Bilder 1..n — Lauf A (EXIT_AT 720, Game Over F677) ist deshalb nur für die Logs
gültig, die Bilder stammen aus A2 (EXIT_AT 676).

| Lauf | Zusatz | Zweck |
|---|---|---|
| A / A2 | `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1`, EXIT_AT 720 / 676, Framedump A2 1–676/1 | Türnische: Intro, Biss, Finisher bis Game Over (B6) |
| EZ | Elza, sonst wie A, EXIT_AT 670#2091 | beide Szenarien (ROOM2091) |
| N | `RE15_PLAYER_POS=-8200,-25550,3072 RE15_R39_MESS_SPIELER_HP=100`, Skript `W2,U12,W40`, OHNE GB_TEST, Framedump 1–1100/1 | natürlicher Aggro beim Annähern aus der Türnische |
| E1 | `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1`, EXIT_AT 900 | ECHTER Weg Tür 20A0 → 2090 (`[tuer] Sequenz Archiv 2 DOOR26`) |
| E3 / E4 | wie E1 OHNE GB_TEST, HP-Haken, Skripte `W15,U3,L0.55,U5,W30` / `W15,U7,W40` | natürlicher Weg von der Osttür (kein Aggro: Leon bleibt > 9000 vom Lauerplatz) |
| D2 | `RE15_GB_TEST=1 RE15_PLAYER_POS=-4000,-24000,2048 RE15_R39_MESS_SPIELER_HP=100`, Framedump 1–1100/1 | Zonen-Dash mit Vordrehung, Schreie, Schütteln, Putz |
| C3 | `RE15_GB_TEST=1 RE15_PLAYER_POS=2500,-25000,2048` HP-Haken, `RE15_GATOR_KANISTER=-1500,-23400`, Autopilot Südbecken, Framedump 1–1400/1 | Kanister-Aufnahme in der freien Verfolgung, Klettern |
| K | Rezept Dossier §7 (`GB_TEST=5`, Kanister (4430,-26160), Pistole, `W2.0,(M0.3,MA0.1)x10`) | Kanister → Schuss → R1-Kiefer-Tod |
| F2 / F3 | `RE15_GB_TEST=1`, Leon (−4000,−24000) / (2500,−25000) ohne HP-Haken | Finisher-Kamera an anderen Todesorten — ohne Ergebnis: Leon starb an Spinnengift (F425 / F456 hp 0 → −20), kein Gator-Finisher |
| L1 | `RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080` | echter Leiterweg 2080 → 2090 — Slot 1 löst per FIRE_AOT nicht aus (rc 124, Raum 2080 bleibt); der Nischen-Zugang wurde deshalb über den Sprung-Spawn (−8200,−25550) gemessen (= Tür-A-Spawn an der Leiter, Kommentar zu GB_LADDER_X in enemy_ai_boss_gator.c) |

### 3.1 Punkt 1 — "Finisher und fehlende Animationen aus Resident Evil 2" → **erfüllt**

- Clip-Folgen (state.log, Slot 20 t=23): A/A2 und EZ bitgleich: F6–F130 **6** · F131–F280 **3** · F283–F325 4 ·
  F360–F390 1 · F391–F416 4 (Biss, Leon-HP F402 10 → −40) · F417–F536 **5** · F537–F656 **11** (RE2-FRESSEN) ·
  F657– 0. D2: F487–F636 **2** · F637–F786 **3** (Dash). K: F70–F260 **9** (R1 + Leiche). E1: F828–F856 **10** (R4 —
  erstmals im Spiel gemessen; Abnahme 1 hatte Clip 10 nur im Riegel). C3: Paddeln/Schwimmen **1/0**. Damit sind im
  Spiel 0, 1, 2, 3, 4, 5, 6, 9, 10, 11 belegt (Clip 7/8 = R2/R3 = Tod durch Beschuss, Entscheid B2).
- **Finisher-Kamera bis Game Over (Entscheid B6, Abnahme-1-M1 b) — behoben, gemessen:** A2 gator_boss.log
  `RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 6 F403 clip=4 bild=0 frontal=1666 soll=4094 massstab=649 re2_m=1354`,
  `RE2 KAMERA FINISHER haelt Cut 6: RE1.5-Todesbild unterdrueckt (cam=0 weiss=1 schwarz=0) ab F467`, 14 Zeilen
  `RE2 KAMERA FINISHER Halt Cut 6 F404 … F664` (je 20 Bilder); state.log `cam=` F403 → 6 und bis F676 kein Wechsel.
  Bildbogen `B41abn2/A2_finisher.png` (F396–F676): F396–F403 Biss in Cut 0, ab F404 Cut 6 (Seitensicht, Gator mit
  Leon im Maul F410–F504, `A2_finisher_x2_gain25.png`), YOU DIED ab F524 über dem Fress-Bild, Abblende F660–F676 —
  keine Weißblende, kein Orbit (Abnahme 1: F735–F745 weiß, ab F750 Orbit). EZ (Elza, ROOM2091) identisch: Cut 6 ab F403,
  14 Halt-Zeilen, cam=6 bis F670.
- **Abnahme-1-M1 a ("kein sichtbarer Schnitt") — Widerlegung geprüft und angenommen:** RE2 setzt mit
  `jal 0x8002c7ac / Cut 7` @0x80100ff0–f4 die Kamera der INSZENIERTEN Lage (Leon (−12435, −915) @0x8010106c–84); in
  ROOM40A0 ist Cut 7 dort Leons eigene Zonenkamera (RVD selbst gelesen, §2) — auch RE2 schneidet also nur, wenn Leon
  vorher in einem anderen Bereich war. Die Port-Form (beste Zonenkamera unter der RE2-Maßstabsgrenze, Entscheid B8)
  schneidet im Nischen-Ablauf jetzt sichtbar (Cut 0 → Cut 6, Kopf (222,106) im Bild). Kein Mangel.

### 3.2 Punkt 2 — Verfolgung wie RE2, Wände wackeln + Putz, Klettern bleibt → **teilweise**

a) **Intro/Dash ohne Wanddurchdringung (Abnahme-1-M2, Entscheid B7) — behoben, gemessen.** Messzeile
`INTRO ende … wand_draussen_max=` (Rumpf 0/1, Hals 5, Kopf 6, Kiefer 7 in jedem Intro-Bild, gezeichnete Skala):
A2/EZ 0 (Kopf x min −6889), N 0, D2 0, E1 0, C3 **2** (zwei Einheiten, unsichtbar; die Planung prüft nur jedes 3. Bild).
Dash: D2/N/C3 `DASH ende … wand_draussen_max=0` (Kopf x min −8642 / −6299 / −1759). Bildbogen A2 F6–F360
(`B41abn2/A2_intro.png`): Durchbruch F40 im Becken (Cut 0), F131–F280 Cut 7 Kopf/Leib im offenen Wasser, kein Kopf in
der Westwand mehr (Abnahme 1: F140–F279 Kopf in der Westwand). Die RE2-Form (Clip 6 → 3 bzw. 2 → 3, `yaw += 1024`)
ist unverändert: A2 `INTRO ende … yaw=1600` = Start 576 + 1024; D2 `DASH ende … yaw=1648` = 624 + 1024.

b) **Schreie / Beschleunigung an den Zonenlinien / Schütteln / Putz — erfüllt.** D2: Zonenwechsel am Südausgang des
Westkanals `DASH Planung: an (-6052,-18113) yaw 1139 -> Start-Gier 624 (Drehung 515)`, Vordrehung F481–F486,
`[re2se] se=4 … bank=17` bei F486, Clip 2 F487–F636, Clip 3 F637–F786 (Tempo Clip 1 Mittel 124/Bild, Clip 2/3 25–27);
ebenso N (Dash F632, SE 4 F631) und C3 (Dash F645, SE 4 F644). Schütteln D2 (`schuettel.py mess/D2/fd d_ 528 566
529`): F542 (0,2) F543 (0,0) F544 (0,2) F545 (0,3) F546 (1,0) = Modus-2-Tafel @0x8009db14 Index 0–4, dann F547–F556
(0,0)(0,1)(0,0)(0,0)(0,0)(0,1)(0,1)(0,0)(0,0)(1,1) = Modus-1-Tafel @0x8009daf4 Index 5–14 — Bild für Bild die RE2-Werte
(§2). Putz D2 `RE2 PUTZ runde 1..6/6 … gesamt=54 um=(-7097,0,-24000)`, `[gator-re2] Putz-Sicht` ab F488 mit
`gezeichnet_vorbild` 27..98, Staubwolke über Leon F530–F560 (`B41abn2/D2_dash.png`).

c) **Klettern über die Plattform bleibt — erfüllt.** C3 (freie Verfolgung, nicht erzwungen): `CROSS start=(4450,-11900)
ende=(3350,-20700) frames=240 fenster=62..177`, Clip 0 F393–F644 auf der Rampe, danach Dash im Südbecken.

d) **NEUER MANGEL (sichtbar, durch Nachbesserung 2 eingeführt): der lauernde Gator verschwindet beim Aggro und taucht
bis zu 16 300 Einheiten entfernt wieder auf.** `gr2_intro_lage` (Pos_set-Zwilling) versetzt den Gator vor dem Intro
auf die gerechnete Startlage. Gemessene Sprünge vom Lauerplatz (−7200,−9200): E1 1125, C3 999, D2 2266, A2/EZ 4080,
**N 16 278** (`RE2 INTRO Lage … von (-7200,-9200) yaw 1089 -> (5085,-19880) yaw 1536`). Lauf N ist der natürliche
Zugang (Spawn Türnische, Leon geht nach Norden, Aggro F201 bei Distanz < 9000 = GB_AGGRO): in Cut 7 ist der lauernde
Gator F183–F200 am Kanalende vor Leon sichtbar, F201 ist er im selben Bild verschwunden, ab F202 (Cut 1) bricht er im
Südostbecken HINTER Leon aus (`B41abn2/N_teleport.png`, Ausschnitt 4x `B41abn2/N_lauer_crop_x4.png`). Der Sprung
erreicht dabei nicht einmal sein Ziel: Leon bleibt während der 275 Intro-Bilder steuerbar und steht bei F386 schon am
Nordende (−8440,−5868); der Intro endet F475 bei (−2791,−23752) mit Blick auf Leons Aggro-Lage, 18 750 von Leon
entfernt. RE2 kennt diesen sichtbaren Sprung nicht: der Gator wird per `Sce_em_set 44 00 00 23 … 4a 16 b8 0b 3c eb 00 04`
@RDT 0x25A0 bei y = **3000** (unter dem Boden, unsichtbar) gesetzt und erst im Intro per `Pos_set` @SCD 0x0736 auf
(4557,−1800,−18237) gehoben — der RE2-Pos_set ist unsichtbar, der Port-Zwilling nicht, weil der Port-Gator vorher sichtbar
an der Leiter lauert (Nutzer-Design 2026-09-10). Der Bau-Agent führt die Fernlage als OFFEN (§10.6), nicht aber das
sichtbare Verschwinden.

e) Nebenbeobachtung (keine Wertung als Mangel): durch die neue Intro-Lage endet der Intro im Nischen-Ablauf vor Leon,
der Nutzer-Design-Biss startet F283 (2 Bilder nach dem Intro-Ende) und trifft F290 (Leon 60 → 10), Tod F402; der
frühere Dash dieses Ablaufs (Abnahme 1: F341) entfällt dort. RE2 sperrt nach dem Intro nur die Hysterese (+0x238 = 110,
@0x8010091c-20), nicht T1 (FUN_801012fc prüft +0x238 nicht, §2) — kein Widerspruch zu RE2.

### 3.3 Punkt 3 — Kanister aufnehmen "GENAUSO wie im original", beim Tragen beschießbar, Kiefer-Tod wie RE2 → **teilweise**

a) **Fortbewegung bei der Aufnahme (Abnahme-1-M3) — behoben, gemessen.** C3 (Rezept wie Abnahme 1 / Bau-Agent):
`RE2 KANISTER Stufe 1 laengs=5868 quer=1593 sub 2 -> 1 F1098` → `angehaengt lokal=(7611,1258,1513)` →
`getragen (Stufe 7)`; state.log Clip 0 F1096–F1162 (Stufen 1..6 vollständig darin, kein Clip 4), erster Biss erst F1163
(Stufe 7, RE2 erlaubt T1 dort @0x8010136c-70), danach in der Kanister-Zone weiter Clip 0 (F1206–F1289, F1333–F1400) —
RE2 Bit 0 + Zone → keine Hysterese (@0x80100ab8-c8/@0x80100aec). Riegel `unit_r41_re2gator_aufnahme` grün (§1). Der
Kanister liegt am Ende im Maul (`B41abn2/C3_aufnahme_zoom.png` F1155/F1162).

b) **Beschießbar beim Tragen + Kiefer-Tod — erfüllt (unverändert seit Abnahme 1).** K: `Stufe 1 … sub 1 -> 1 F6` →
`getragen (Stufe 7)` → `RE2 R1 VSync 4 (Zeitlupe) F70`, `RE2 R1 start pos=(4997,-20318) hp=25293`, `[gator-re2] Raum-SE
15 -> GATOR40A0.VBS Satz 1`, Clip 9 F70–F260, `R1 ende (Leiche)`, `RE2 R1 VSync 2 F108`; Bildtakt `VSync 4 (Bildbudget
66 ms) ab F70 t=11456 ms` → `VSync 2 … ab F108 t=13980 ms` = 38 Bilder in 2524 ms = 66,4 ms/Bild. Bilder
(`B41abn2/K_bogen.png`): F71–F84 Blitz/Fontänen am Kopf, dunkelrote Kiefer-Scherben, ab F88 Leiche mit offenem Maul.

c) **MANGEL: der Kanister wird beim Aufnehmen im Kreis über das Becken geschleppt.** RE2 lenkt den Körper nie —
Wortsuche über EM23_OVL_0000.BIN nach `sh …,118(…)` (Gier +0x76): nur INIT @0x80100280/@0x801002a0, Intro-Knick
@0x80100948, Dash-Knick @0x80100ec0, R1-Rückstoß @0x80102404/@0x80102440, R2/R3 @0x801027c8/@0x80102a34 — in Sub 1/2 und
in der Aufnahme-Maschine T2 (@0x801017c4…) keine. Stufe 2 schiebt den Kanister deshalb in RE2 nur GERADEAUS vor die
Wurzel (`obj[1].z = z + 8100` @0x801018a4-c0) bis zum Anhängen an Part 6 bei t = 13 (@0x801018c4-f4). Im Port lenkt
der Boss-Takt während der Stufen 1..6 weiter (Nutzer-Design-Wende 0x60/Bild bzw. Wand-Logik), und
`gr2_kan_takt` Stufe 2 schiebt den Kanister längs der JEWEILIGEN Blickachse nach — er läuft auf einem Kreis
(Radius 8100 × 2731/4096 = 5400) um den Gator mit. Gemessen: (1) exe C3 F1100–F1114 (`B41abn2/C3_stufe2.png`, je Bild):
der bei (−1500,−23400) liegende Kanister rutscht vom treibenden Zombie bis vor die Kamera und schwenkt mit der Gier-
Sägezahnfolge des Gators an der Westwand (state.log F1097 r4095, F1098 r95 … F1107 r575, F1108 r4095) hin und her;
(2) Prüfer-Sonde `B41abn2/probe/probe_abnahme2.c` Teil `schleppen` (Kopie des Spur-Riegels, gegen
`build/engine/libre15_engine.a`, NICHT im Baum): Gator (−3000,−24000) Blick Ost, Kanister im Fenster (8800 × Skala
voraus), Leon im Norden → während Stufe 2 dreht der Gator auf der Stelle 4000 → 2992 (Gator-Weg 101), der Kanister
wandert von (2867,−24000) nach (5200,−18302) = **6157 Einheiten** bis zum Anhängen. Verlangt: während der Aufnahme
(mindestens Stufe 1 bis zum Anhängen bei t = 13) keine Lenkung des Körpers (RE2 Sub 1 ohne Gier-Schreiber) oder ein
Schub nur entlang der Gier beim Fenstertreffer; Riegel auf den Kanister-Weg in Stufe 2 ≤ Gator-Weg.

d) **MANGEL (klein): die RE2-Anschwimm-Sperre fehlt.** In Zone 1 mit Bank 5 Bit 1 (ROOM40A0 sub10 `Set 22 05 01 01`
@SCD 0x0659, nie gelöscht) setzt RE2 bei Stufe < 2 den Zielmodus `+0x1C0 |= 4` mit Ziel (x, y−800, obj[1].z)
@0x80100a34-70 und löscht ihn erst bei Stufe 0 und obj[1].z − z < 8880 (@0x80100a74-b4) bzw. beim Anhängen
(`andi v0,v0,0xfb` @0x801018e4-f4); solange er steht, macht FUN_801012fc T1 NICHT scharf (`lbu v0,448(a0) / andi 0x4 /
bne` @0x8010134c-58), und der Root ruft im Zielmodus FUN_800177c0 statt der normalen Halsführung FUN_80017d98
(@0x80100344-74; welches Ziel FUN_800177c0 über +0x1B8 liest, habe ich nicht weiter verfolgt).
Der Port bildet aus FUN_801012fc nur die Stufen-Bedingung @0x80101360-70 ab (`re15_gr2_biss_erlaubt`). Prüfer-Sonde
Teil `anschwimmen`: Kanister liegt 10000 bzw. 14000 RE2-Einheiten (6667 / 9334 Port) voraus in der Zone des Gators,
Leon 2000 vor der Wurzel → `Biss-Start nach 0 Takten, Stufe 0, biss_erlaubt 1` (ohne Kanister ebenso) — RE2 schnappt
hier nicht. Das Dossier nennt den Zielmodus (§2.5 "nur Hals-Tracking"), weder umgesetzt noch als OFFEN/PORT-WAHL geführt.

## 4. Gates

### 4.1 RE-Gate
- Diff-Grep der Nachbesserung 2 (`git diff 393bb59b..HEAD -- re15_port/engine re15_port/include re15_port/platform`,
  nur `+`-Zeilen) nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|XXX|vorerst|geschaetzt|ungefaehr|
  approx": **0 Treffer**; kein neues `getenv` im Engine-Code (nur der Riegel liest `R41B_LOG` zur Diagnose).
- Konstanten der Nachbesserung 2: RE2-Werte mit @0x im selben Kommentar (Fenster/Stufen/Sub-Wechsel, Knick 1024,
  T1-Stufen); die Such-Parameter der Intro-Lage/Dash-Planung (Toleranz 128/256/512, Raster 32/16, Abstand 1000..24000
  in 250, Kosten 4 x Drehung + 16 x Toleranz, Vordrehung 0x60, max. Drehung 0x400, GR2_LEON_MIN 1350 = 900 + 0x1c2,
  GR2_KLEMM_R 1466 = 2200 x 2731/4096, Beckengrenzen = ROOM2090-SCA) stehen im Block-Kommentar als PORT-WAHL mit
  Herleitung. Commit-Texte tragen die Adressen.
- Selbst disassembliert und bestätigt: §2 (Sub-1/2-Pre komplett, Dispatcher, Sub-1-Main, T1, FRESSEN P0, alle −257-
  Stellen, Leon-Lage + RVD, Op-73-Tafel, SCD sub10/sub11, Schüttel-Tafeln, Stufe-2-Schub @0x801018a4-c0, Gier-Schreiber
  des Overlays, Sce_em_set y = 3000 @RDT 0x25A0, Flag-Prüfer FUN_80077360). Keine falsche Adresse gefunden.
- "Erklärt der Fix den Befund?": Abnahme-1-M1 b ja (Boss-Takt nach der FSM nimmt Weiß/Schwarz/Orbit zurück, gemessen
  A2/EZ); M1 a mit RVD-Beleg widerlegt; M2 ja für die Wand (wand 0), aber mit neuer sichtbarer Nebenwirkung (§3.2 d);
  M3 ja für die Fortbewegung (Sub 2 → 1, Hysterese-Ruhe, Biss-Sperre 1..6), die Lenkung während der Aufnahme blieb
  (§3.3 c); M4 ja (re2_fx.c-Kommentar @0x8009d98c, Dossier §3.2 durchgestrichen, Riegel-Kopf F403/F677).

### 4.2 Vertrag
- Bank-9-Bits / Nachrichten-Ids: keine (Diff-Grep `game_flag_set|flag_set(|MSG_ID|msg_id|bank9|g_game.flags` leer).
- Pfade (`git diff master...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/,
  keine CMakeLists. Gemeinsame Dateien: Nachbesserung 2 ändert main.c/enemy_ai_re2_zombie.c nicht, re2_fx.c nur
  Kommentar (Op 73); Stand der Spur insgesamt main.c +13/−3 (4 markierte Haken, Entscheid B5), re2_fx.c +41,
  enemy_ai_re2_zombie.c +4 — unverändert seit Abnahme 1.
- Tests: neuer Riegel `unit_r41_re2gator_aufnahme` (messend, Clip/Blend/Biss/Zone), `wand` neu gefasst (Bone-Lage
  gegen das Wasser je Bild), `kamera (c)` Finisher-Halt, `integration_r41_re2gator` zwei Läufe (Lage/Wand/Kamera-Folge/
  Finisher-Halt ≥ 100 Bilder; Dash/Putz). Parallel-fest (Zufalls-Id, eigene exe-Kopie, nur eigene Kopie gelöscht).
  `probe_gator_fress` mit Begründung angepasst (Gier 0 vor dem Biss, Pins unverändert).
- Gezielte Läufe am gebauten Stand: Unit 101/101 (§1); `ctest -R "^integration_(r41_re2gator|r36_schuss|r38_wasser|
  r35_granate|r37_itemband|r36_tueren)$" --timeout 900 -j 3` → **100% tests passed, 0 tests failed out of 6**
  (1074 s, 07:27:36–07:45:31; r41_re2gator 103,6 s, r36_schuss 201,6 s, r35_granate 216,2 s, r38_wasser 217,4 s,
  r36_tueren 63,2 s, r37_itemband 856,5 s unter Last); `integration_r41_re2gator -V` einzeln: `nische: RE2-Zeilen 56,
  Intro Lage/Start/Ende 1/1/1 wand 0, Clip 6/3 125/150, Kamera DURCHBRUCH:6/0;SEITE:6/70;ANGRIFF:6/124;FREI:3/149;
  FINISHER:4/0, Finisher Cut 6 ab F403 gehalten 248 Bilder / 13 Halt-Zeilen | dash: DASH 1/1 wand 0, Putz 6 x 9 = 54,
  gezeichnet bis 98 Quads/Bild` (81,3 s).
- Kein Riegel deckt das Verschwinden des lauernden Gators (§3.2 d) oder den Kanister-Weg in Stufe 2 (§3.3 c) ab.

### 4.3 Nebenwirkungen
- Elza (ROOM2091, `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `JUMP -> 209 ALLIGATOR POOL (ROOM2091)`):
  gator_boss.log und Clip-Folge identisch mit Leon (Intro-Lage, Kamera, Finisher-Halt Cut 6 F403–F670) ✓.
- Echter Türweg 20A0 → 2090 (E1, `[tuer] Sequenz Archiv 2 DOOR26`) ✓; natürlicher Weg ohne Testhaken von der Osttür
  (E3/E4): kein Aggro in 1300–1500 Bildern, Gator bleibt am Lauerplatz (Leon > 9000) — unverändertes Nutzer-Design.
- Nachbarraum-/Querriegel r36_tueren (2090 → 20A0), r37_itemband (Weg 20A0 → 2090-Plattform), r36_schuss, r38_wasser,
  r35_granate grün; Unit-Querriegel text_freeze/load_after_death/r34_reaktion/r35_affen_finisher (g_death_*) grün.
- Die Finisher-Rücknahme wirkt nur, solange das FRESSEN-Kameraprogramm läuft; `re15_gr2_reset` → `gr2_kam_reset` beim
  Raumstart (gator_re2_r41.c) — kein Nachlauf in andere Räume.
- Rechenzeit `re15_gr2_intro_lage` ohne Speichertreffer (Prüfer-Sonde Teil `lagezeit`, -O0): 2–13 ms je Aggro —
  kein Ruckler.

Prüfer-Sonde (nachfahrbar, liegt NICHT im Baum): `B41abn2/probe/probe_abnahme2.c` = `tests/unit/test_r41_re2gator.c`
+ Teile `anschwimmen`, `schleppen`, `lagezeit`; gebaut mit `C:/msys64/mingw64/bin/gcc.exe -std=c11 -DRE15_PLATFORM_PC=1
-DRE15_ASSET_PSX_DIR=… -DRE15_ASSET_RE2_DIR=… -I re15_port/include probe_abnahme2.c re15_port/build/engine/libre15_engine.a
re15_port/build/tests/libre15_test_support.a -lm` (PATH mit msys64 zuerst), Aufruf `probe_abnahme2.exe <teil>`.

## 5. Nebenbefunde (keine Mängel dieser Nachbesserung, zur Kenntnis)
- C3: `INTRO ende … wand_draussen_max=2` — zwei Einheiten außerhalb (unsichtbar); die Planung prüft jedes 3. Bild, die
  Messzeile jedes Bild. Die Aussage "wand 0" gilt für A/N/D2/EZ/E1, nicht allgemein.
- Gier-Sägezahn an den Wänden (Einzelbilder `mo=0 af=1` mit r4095 an der Westwand, r2047 an der Rampe) ist ALT — schon
  in den Abnahme-1-Läufen C2/C3/A/E1/N (`B41abn1/mess/*/state.log`, z.B. A F641–F642 r2048 → r4095); er verstärkt
  Mangel 2 sichtbar.
- Ein-Bild-Aufblitzen des Gators an der Spawn-Lage (−5300,−16300) vor dem INIT (A2 F5, `B41abn2/A2_start_crop.png`) —
  alt (Abnahme 1: F1–F5 an derselben Lage).
- RE2-Halsführung in der Verfolgung (Root → FUN_80017d98 bei +0x218 & 4, @0x8010036c-74) ist im Port nicht vorhanden
  (der Port hat nur die Kanister-Ease-Winkel kopf_az/kiefer_az auf Part 6/7); enemy_ai_re2_dog.c:461-471 führt dieselbe
  Funktion für den Hund ausdrücklich als OPEN. Von Abnahme 0/1 nicht verlangt — für die Spur-Planung nennen.
- E1: Leon überlebt mit HP 0 (steht ab F701 wieder) — RE1.5-Todesregel (hp < 0), nicht Spur B.
- Zielhilfe trifft in 2090 weiter fast nur Spinnen; E1 zeigt R4 (Clip 10) F828–F856 im Spiel nach 40 HP Schaden
  (Quelle nicht weiter verfolgt) — Spinnen entfernt Spur F.

## 6. Mängel (nummeriert, nachprüfbar)

1. **M1 (Punkt 2, sichtbar, NEU durch Nachbesserung 2): der lauernde Gator verschwindet beim Aggro und bricht bis zu
   16 300 Einheiten entfernt hinter Leon aus.** Nachprüfung: Lauf N (`RE15_DEBUG_JUMP=2090@250
   RE15_PLAYER_POS=-8200,-25550,3072 RE15_R39_MESS_SPIELER_HP=100 RE15_INPUT_SCRIPT_BASIS=spiel
   RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W2,U12,W40 RE15_EXIT_AT=1100#2090 RE15_FRAMEDUMP=1-1100/1:fd/n_`, OHNE
   RE15_GB_TEST): gator_boss.log `RE2 INTRO Lage … von (-7200,-9200) yaw 1089 -> (5085,-19880) yaw 1536`; Bilder F190/F200
   (Cut 7) Gator am Kanalende sichtbar, F201 im selben Bild weg, ab F202 Cut 1, F220–F270 Durchbruch im Südostbecken
   (`B41abn2/N_teleport.png`); Sonde `lagezeit`: Leon (−8440,−12000) / (−8440,−18096) im Westkanal bzw. (−6000,−22000) im Südbecken West → Sprünge 15 100 / 16 278 / 16 848. RE2 verdeckt
   seinen Pos_set: `Sce_em_set` @RDT 0x25A0 setzt den Gator bei y = 3000 (unter dem Boden) und hebt ihn erst im Intro
   (`Pos_set` @SCD 0x0736); der Port-Gator lauert dagegen sichtbar ("Blick zur Leiter", Nutzer-Design 2026-09-10,
   enemy_ai_boss_gator.c:85-91). Der Sprung erreicht auch sein Ziel nicht (Leon steuerbar, Intro endet 18 750 von Leon).
   Verlangt: im natürlichen Ablauf kein sichtbarer Sprung des Gators (Startlage, die der Durchbruch-Schnitt verdeckt
   bzw. nahe der Lauerstelle bleibt, oder die RE2-Form "vor dem Intro unsichtbar"), Bildbeleg Lauf N, Riegel auf
   Sprungabstand/Sichtbarkeit.
2. **M2 (Punkt 3, sichtbar): der Kanister wird beim Aufnehmen mit der Gier des lenkenden Gators im Kreis geschleppt.**
   Nachprüfung: Lauf C3 (`RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_PLAYER_POS=2500,-25000,2048
   RE15_R39_MESS_SPIELER_HP=100 RE15_GATOR_KANISTER=-1500,-23400 RE15_AUTOPILOT=…Südbecken… RE15_EXIT_AT=1400#2090
   RE15_FRAMEDUMP=1-1400/1:fd/c_`) Bilder F1100–F1114 (`B41abn2/C3_stufe2.png`); Sonde `schleppen`: Gator dreht in
   Stufe 2 von 4000 auf 2992 (Weg 101), Kanister wandert 6157. RE2: kein Gier-Schreiber in Sub 1/2/T2 (Overlay-Suche
   `sh …,118(…)`: nur @0x80100280/2a0/948/ec0/2404/2440/27c8/2a34), Schub nur geradeaus `obj[1].z = z + 8100`
   @0x801018a4-c0 bis zum Anhängen @0x801018c4-f4. Verlangt: während der Aufnahme bis zum Anhängen keine Körperlenkung
   (oder Schub nur entlang der Gier des Fenstertreffers), Riegel "Kanister-Weg in Stufe 2 ≤ Gator-Weg".
3. **M3 (Punkt 3, klein): RE2-Anschwimm-Sperre/Zielmodus fehlt.** Nachprüfung: Sonde `anschwimmen` — Kanister liegt
   10000 / 14000 RE2-Einheiten voraus in der Zone des Gators, Leon 2000 vor der Wurzel → `Biss-Start nach 0 Takten,
   Stufe 0, biss_erlaubt 1`. RE2: Bank 5 Bit 1 (sub10 @SCD 0x0659) → `+0x1C0 |= 4` bei Stufe < 2 @0x80100a34-70, gelöscht
   bei Stufe 0 und obj[1].z − z < 8880 @0x80100a74-b4 bzw. beim Anhängen @0x801018e4-f4; T1 nicht scharf bei
   `+0x1C0 & 4` @0x8010134c-58; Root ruft dann FUN_800177c0 statt FUN_80017d98 @0x80100344-74. Verlangt: dieselbe
   Biss-Sperre in der Kanister-Zone (Stufe 0, Kanister ≥ 8880 × Skala voraus; Stufe 1 bis zum Anhängen) + Riegel; den
   Hals-Zielmodus umsetzen oder mit Beleg als OFFEN führen (kein Part-Winkel-Kanal).

## 7. Gesamturteil

`bestanden = false`. Die vier Mängel der Abnahme 1 sind nachgebessert und am gebauten Spiel belegt: der Finisher-Schnitt
steht bis zum Game Over ohne Weißblende/Orbit (A2/EZ, B6), die Finisher-Wahl ist mit dem RVD-Beleg als RE2-Form
angenommen, Intro und Dash bleiben mit Kopf und Rumpf im Wasser (wand 0, Bildbogen), die Kanister-Aufnahme schaltet wie
RE2 auf Schwimmen ohne Hysterese und ohne Biss in Stufe 1..6, Belege gepflegt; Schütteln Bild für Bild = RE2-Tafeln,
Putz, Schreie, Klettern, Kiefer-Tod mit Zeitlupe laufen; alle selbst geprüften Adressen stimmen; Riegel 101/101 + 6/6
grün; Vertrag eingehalten. Offen: **M1** — die neue Intro-Startlage erzeugt im natürlichen Ablauf einen sichtbaren
Sprung des lauernden Gators (bis 16 300 Einheiten, hinter Leon), **M2** — der Kanister wird bei lenkendem Gator im Kreis
geschleppt, **M3** — die RE2-Anschwimm-Sperre fehlt. Punkt 1 (mit Nachtrag 4) erfüllt, Punkte 2 und 3 teilweise.
