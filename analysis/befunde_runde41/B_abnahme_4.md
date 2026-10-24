# Runde 41 — Spur B "re2gator" — Abnahme 4 (unabhängig)

Prüfer: Abnahme-4-Agent, 2026-10-24 (ab 11:38). Baum `.claude/worktrees/r41_re2gator`, HEAD b20f1496 (Zweig r41/re2gator,
merge-base mit master 116e8b0f; master selbst steht auf 7b59e297 — die dort neuen Nachträge 7–10 und Entscheide E-E1..E-E3
betreffen nur die Spuren C/E). Code-Stand = 7831a6d5 (b20f1496 ändert nur das Dossier). Geprüft wird NUR der gebaute Stand;
aus dem Dossier wird nichts ungemessen übernommen. Messdateien: Prüfer-Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/B41abn4/`
(Läufe unter `mess/<Lauf>/`, eigene exe-Kopien `re15_pc_b41a4_<Lauf>.exe` bzw. `re15_pc_b41a4z_<Lauf>.exe` für Elza).
"Vorher" = die Läufe der Abnahme 3 am Stand 47efb932 (`.../scratchpad/B41abn3/mess/<Lauf>/`, gleiche Rezepte).

(Fortlaufend geschrieben; Gesamturteil am Ende.)

## 1. Lektüre + Bau

- Gelesen: AUFTRAG.md (Wortlaut, Nachträge 4/5/6, Entscheide B1–B9; im Spurbaum und die neueren Einträge im master),
  VERTRAG.md, B_abnahme_3.md (Mängel M1/M2), Dossier B_re2gator.md §12 (Nachbesserung 4, 12.0–12.6),
  `git log --oneline master..HEAD` (11 Commits der Nachbesserung 4: 5f10b1a5 … b20f1496), `git diff master...HEAD --stat`
  (Zweig gesamt: 70 Dateien, +8905/−55; `git diff master` ohne merge-base zeigt zusätzlich die seither in master gelandeten
  Spuren A/D als Rückbau — nicht Teil der Spur), Code-Diff der Nachbesserung 4 vollständig gelesen (`git diff d4e36ca6..HEAD
  -- re15_port`: enemy_ai_boss_gator.c +165/−20, gator_re2_r41.c +165/−36, gator_re2_pc.c +26, Header +11/−2 und +14/−1,
  Riegel test_r41_re2gator.c +205/−1, probes/r41_re2gator.cmake +11/−2, test_r41_re2gator.cmake +101/−4), dazu die gemeinsamen Dateien des Zweigs
  (main.c 5 Haken, enemy_ai_re2_zombie.c +4, probe_gator_fress.c/probe_gator_sweep.c/probe_r34_reaktion.c) und die
  Dispatcher-Klemme in enemy_ai_common.c:14863-14878 (unverändert; sie fragt `re15_gator_boss_skip_clamp`).
- Bau: `bash re15_port/tools/local_build.sh configure` → `LOCAL-BUILD-OK (configure)` (11:40:22); alle Spur-B-Quellen
  (`git diff --name-only master...HEAD -- re15_port`, .c/.h/.inc) per `touch` (nur mtime), dann `local_build.sh build` →
  798 Schritte, `LOCAL-BUILD-OK (build)`, EXIT 0 (11:40:35–11:41:05), re15_pc.exe 11:40. Warnungen nur main.c 8803/10133/10513
  (`-Wdiscarded-qualifiers`, Altbestand), keine in Spur-B-Dateien (`B41abn4/build.log`).
- Gezielte Unit-Riegel (Regex des Bau-Agenten + `r36_kartenbits` + `irons_mittelmodell` + `r36_story_sweep`, die 2090 bzw. den
  Boss-Takt berühren): `ctest -R "^unit_(r41_re2gator_.*|alligator_ai|gator_sweep|gator_fress|re2_spider_water_2090|sca_wall_hit|
  …|r36_kartenbits|text_freeze|r34_reaktion|r34_schaden|g5_boss|…|irons_mittelmodell|r36_story_sweep)$|^probe_(re2z_stomp|
  crow_kill)$|^r31_trefferboxen.*$" --timeout 600 -j 4` → **100% tests passed, 0 tests failed out of 112** (11:41–11:43,
  `B41abn4/ctest_unit.log`; unit_gator_sweep 73,5 s). `-R "^unit_r41_re2gator_(sitz|gier|schleppen)$" -V`
  (`B41abn4/unit_r41_sitz_gier_V.txt`):
  - `sitz: (a) Aufnahme Stufe 3..7 + 40: 66 angehaengte Bilder, groesster Abstand zu Part6(Bild) x lokal 0` | `(b) 240 Bilder,
    Wende 105 (max 0), Biss 43 (max 0), mit Ueberblendung 0, groesster Abstand 0` | `(c) Befreiung 17 Schub-Bilder, Gier fest 1,
    groesster Abstand 0` | `(d) Ueberblendung 16 Bilder (Zaehler bis 16), groesster Abstand 0; Effekt-Quelle Part 6 falsch in 0 Bildern`
  - `gier: E1 Plattform-Belagerung … 800 Bildpaare CHASE/GUARD (GUARD 800, an der Wand 461), max |Gier-Delta| 64 (ohne +1024),
    Spruenge > 0x60 0, Befreiungen 0` | `C3 Westwand vor der Tuernische … 536 Bildpaare … max 96, Spruenge > 0x60 0, Befreiungen 0`
  - `schleppen: Fall 3 (quer 1200): Lokallage beim Anhaengen (4430,959,487) -> nach Stufe 7 + 40 Takte (1750,-210,0)`

## 2. RE-Stichproben (selbst disassembliert: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`, `--bin EM23_OVL_0000.BIN` bzw. info/re2leon/PSX.EXE)

Alle zitierten Stellen der Nachbesserung 4 enthalten das Behauptete:
- **Kanister = Kind der Part-6-Weltmatrix** (`dis 0x801018c0 24`): `lh v1,346(s1) / addiu v0,zero,13 / bne v1,v0,0x80101b90`
  @0x801018c4-cc, `addiu a0,s0,1104` @0x801018d0 (1104 = 6 × 172 + 0x48), `sh zero,346(s1)` @0x801018dc, `sw a0,128(s2)`
  @0x801018e0, `andi v0,v0,0xfb` / `sb v0,448(s1)` @0x801018ec/f4, `jal 0x8002d030` @0x801018f0 (a1 = obj[1]+84, a2 = sp+16),
  `sw v0,56(s2) / sw v1,60(s2) / sw a3,64(s2)` @0x80101910-18. ✓
- **Root-Reihenfolge je Bild** (`dis 0x801001e8 112`): Routine `jalr v0` @0x80100200 (Tafel 0x8010461c) → `jal 0x8001a308`
  @0x8010020c → Winkel sichern/nullen, `jal 0x80035408` @0x80100284 (Kollisionsboxen, nicht Matrix), Rückschreiben
  @0x80100294-2c8 → `jal 0x801017c4` (T2) @0x80100320 → `jal 0x80101388` @0x80100328 → Hals @0x80100330-74 → Kopf-az
  `lhu v0,1140(s2) / lhu v1,554(s0) / addiu a1,s2,1056 / addu / jal 0x8008e1f4 / sh v0,1140(s2)` @0x8010037c-90, Rücknahme
  @0x80100394-a4. ✓ (Wann RE2 die Weltmatrix +0x48 zusammensetzt — vor oder nach dem Kind obj[1] — steht dort nicht; der
  Bau-Agent führt das für die Effekt-Quellen ehrlich als OFFEN, §12.4.)
- **Überblendung FUN_8001a330 (EXE)** (`dis 0x8001a330 44`): `lbu v0,332(s0) / beq v0,a1 / sb a1,332(s0) / sb zero,333(s0) /
  sb s1,334(s0)` @0x8001a34c-64; `addiu v1,s1,1 / addiu v0,zero,4096 / divu v0,v1 / … / mflo v0 / … / jal 0x8002959c / or
  a3,v0,a3` @0x8001a37c-ac. 0x80029614: `lbu t3,334(s2) / beq t3,zero / andi v0,a3,0xffff / mult v0,t3 / … / addiu v0,zero,4096 /
  mflo t1 / subu v0,v0,t1` @0x800296a8-ec; `bne t3,zero,0x800299c0` @0x80029994; `lbu v0,334(s2) / addiu v0,v0,-1 /
  sb v0,334(s2)` @0x800299c0-cc. ✓
- **Blend-Argumente**: `addiu a2,zero,32 / jal 0x8001a330` @0x80100c38-3c (Clip 0), `addiu a2,zero,4` @0x80100cfc (Clip 1),
  `addiu a1,zero,2 / addiu a2,zero,16` @0x80100dac-b0 (Dash-Clip 2), `addiu a1,zero,10 / addiu a2,zero,3` @0x80102c40-44
  (Großreaktion). ✓ (Port: anim_blend_rate = 4096/(n+1) → 1024 für n = 3, 128 für 31 … wie im Code.)
- **Gier-Schreiber im ganzen EM23-Overlay** (eigene Volldisassembly `B41abn4/em23_full.dis`, 4660 Wörter, grep
  `(sh|sw|sb|swl|swr) …,(115..121)(…)`): `sh zero,116/118/120(s0)` @0x8010027c/280/288 und Rückschreiben @0x80100294/2a0/2c8
  (Sichern um 0x80035408), sonst `sh v0,118(…)` nur @0x80100948, @0x80100ec0, @0x80102404, @0x80102440, @0x801027c8, @0x80102a34 —
  keiner in Sub 1/2 (Pre 0x80100964, Main 0x80100c20/0x80100cdc) oder T2. ✓ Die Befreiung ist damit Port-Design, ihre neue
  Form (Schub ohne Drehung) ist als PORT-WAHL gekennzeichnet.
- Schluss: **keine falsche Adresse** gefunden.

## 3. exe-Läufe (eigene Kopien `re15_pc_b41a4_<Lauf>.exe`, Ordner `B41abn4/mess/<Lauf>/`)

Basis (`B41abn4/lauf.sh`, = Rezepte der Abnahme 3): `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_STATE_LOG=state.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=audio.raw`, `timeout`,
Umgebung des Aufrufers ohne RE15_GB_TEST/RE15_GATOR_KANISTER/RE15_PLAYER_POS/RE15_KEIN_GATOR/RE15_INPUT_SCRIPT/RE15_AUTOPILOT/
RE15_FRAMEDUMP/RE15_EXIT_AT/RE15_DEBUG_JUMP; Elza (`lauf_elza.sh`) mit `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1`. Auswertung `statelog.py` (Clip-Läufe t=23), `gier.py`/`gierliste.py` (Gier je Bild), `relativ.py`
(Leon gator-lokal), `schuettel.py`, `vergleich.py`, `sheet.py`/`crop.py` (Bildbögen; Aufhellung nur zur Sichtung).

**Vorher-Bau zum Gegenprüfen (neu in dieser Abnahme):** `B41abn4/probe/alt/re15_pc_alt.exe` = derselbe Bau, nur die drei
Spur-B-Objekte der Nachbesserung 4 (enemy_ai_boss_gator.c, gator_re2_r41.c, gator_re2_pc.c) aus `git show 47efb932:…` mit den
Flags aus build.ninja neu übersetzt und gegen `libre15_engine_alt.a` (= aktuelle Bibliothek mit den zwei ersetzten Objekten)
gelinkt; der Engine-Diff 47efb932..HEAD besteht nur aus diesen Dateien + zwei Headern (nur neue Deklarationen). Gegenprobe:
Lauf E1 mit diesem Bau = Abnahme-3-Lauf `B41abn3/mess/E1/state.log` **900/900 Zeilen bitgleich**.

| Lauf | Zusatz | Zweck |
|---|---|---|
| A2 | `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1`, EXIT_AT 664 (676 aus Abnahme 3 liegt jetzt nach dem Raumwechsel: der Finisher kommt 5 Bilder früher, das Spiel lief bis zum Timeout weiter) | Türnische: Intro, Biss, Finisher bis Game Over |
| D2 | `RE15_GB_TEST=1 RE15_PLAYER_POS=-4000,-24000,2048 RE15_R39_MESS_SPIELER_HP=100`, EXIT_AT 1100 | Dash, Schreie, Schütteln, Putz |
| C3 | `RE15_GB_TEST=1 RE15_PLAYER_POS=2500,-25000,2048` HP-Haken, `RE15_GATOR_KANISTER=-1500,-23400`, Autopilot Südbecken, `RE15_POSE_DUMP=pose.log`, EXIT_AT 1400 | Klettern, Kanister-Aufnahme + Tragen, Wende/Biss mit Kanister, Dash-Pose |
| K | `GB_TEST=5`, Kanister (4430,−26160), Pistole, `W2.0,(M0.3,MA0.1)x10`, EXIT_AT 260 | Kanister → Schuss → R1-Kiefer-Tod |
| Z | wie K, Kanister (4430,−26900), 3 Schüsse, EXIT_AT 600 | Anschwimm-Sperre, Tragen |
| E1 / E1neu_lang / E1alt_lang | `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1`, EXIT_AT 900 (Framedump jedes Bild) bzw. 1500 (neu und Vorher-Bau) | echter Türweg 20A0 → 2090, Belagerung an der Rampe |
| N / N0 / NZ | `RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=-8200,-25550,3072` HP-Haken, `RE15_INPUT_SCRIPT=W2,U12,W40`; N0 mit `RE15_KEIN_GATOR=1`; NZ = Elza (ROOM2091) | natürlicher Zugang (B9), Elza |

Alle Läufe rc = 0 (A2 im ersten Versuch rc = 124, s. Tabelle; wiederholt mit EXIT_AT 664 → rc = 0).

### 3.1 Punkt 3 / Abnahme-3-M1 — Kanister "im Maul" in jedem gezeichneten Bild → **behoben, gemessen**
- C3 (gleiches Rezept wie Abnahme 3; der Ablauf weicht ab F1097 ab, weil der Befreiungs-Snap fehlt): gator_boss.log `RE2 KANISTER
  Zielmodus 1 (Anschwimm-Sperre) laengs=5953 Stufe 0 F1107` → `Stufe 1 laengs=5953 quer=1335 sub 2 -> 1 F1107` → `angehaengt
  lokal=(4614,-253,616) F1119` (12 Bilder, @0x801018c4-cc) → `getragen (Stufe 7)`; debug.log `[gator-re2] Kanister-Sitz F1388:
  angehaengt 270 Bilder, Abweichung max 0 (F1119)` (10 Zeilen, alle `jetzt 0`, darunter `F1119 … clip=0 bild=12 frac=20` =
  Bild mit Überblendung, `F1178 … clip=4` = Biss). Die Messzeile vergleicht Part6 aus dem Überblend-Schnappschuss, den der
  ECHTE Zeichner (main.c NPC-Pass, skeleton_common.c:303/333) in diesem Bild geschrieben hat, mit der Kanister-Weltlage, die
  gator_re2_pc.c zeichnet (`re15_gr2_kanister_welt` = `s_kan.welt`) — also nicht selbstbestätigend (zwei getrennte Rechnungen,
  eine davon der echte Zeichner); Zeichner-Wurzel = `npc->x/y/z` + `nyaw × render_scale` (main.c:10161-10163), gleich wie
  `gr2_part_welt`.
- Wenden/Bisse mit Kanister im Bild (Gier-Liste `B41abn4/C3_gierliste.txt`): F1120–F1126 Wende −96/Bild, F1150–F1154 Biss mit
  −80/Bild, F1194–F1200 −96/Bild, F1258–F1296 Biss mit +80/Bild. Bildbögen: `B41abn4/C3_wende1_zoom.png` (F1118 Kanister liegt vor
  dem Maul, ab F1119 dreht er mit dem Kopf, Zylinder zwischen den Kiefern, gelbes Ende an der Schnauze — kein Nachlauf),
  `C3_biss_zoom.png` (F1256–F1268: beim Aufbäumen mit +80/Bild steckt der Kanister im offenen Maul am Kiefergelenk),
  `C3_biss1_zoom.png` (F1148–F1180). Kein Bild mit dem Kanister neben/auf/hinter dem Kopf gefunden.
- Z (Anschwimm-Sperre): `Zielmodus 1 … F6`, `… F100`, `Stufe 1 laengs=5956 quer=-1511 … F147`, `angehaengt lokal=(3745,-173,-2300)
  F159`, `Kanister-Sitz F578: angehaengt 420 Bilder, Abweichung max 0 (F159)`; erste Bisse mit Kanister erst in Stufe 7 (Clip 4 ab
  F185) — wie Abnahme 3.
- K (Kanister-Tod): `Stufe 1 laengs=5868 quer=17 sub 1 -> 1 F6` → `angehaengt lokal=(4373,-161,11) F18` → `RE2 R1 VSync 4 (Zeitlupe)
  F70` → `[gator-re2] Raum-SE 15 -> GATOR40A0.VBS Satz 1` → `RE2 R1 ende (Leiche)` → `VSync 2 F108`; Bildtakt `VSync 4 (Bildbudget 66
  ms) ab F70 t=12062 ms` / `VSync 2 … ab F108 t=14578 ms` = 38 Bilder in 2516 ms; `Kanister-Sitz F47: … 30 Bilder, Abweichung max 0`.
  Clip 9 F70–F260; Bogen `B41abn4/K_kiefer_zoom.png`: F71–F80 Blitz/Splitter am Kopf, ab F82 Leiche mit geplatztem Unterkiefer.
- Riegel: unit_r41_re2gator_sitz (a)–(d) je 0 (§1); integration "kanister" (§5).
- **RE2-Gegenprüfung (eigene, über den Bau-Agenten hinaus):** RE2 hat beim getragenen Kanister tatsächlich KEINEN Nachlauf. Haupt-
  schleife (PSX.EXE, `dis 0x80026700 140`): Entity-Schleife 0x800267c0–0x80026930 ruft je Aktor erst die KI (`lw v0,516(s0)` /
  `jalr v0` @0x80026878-88), dann sofort dessen Zeichnen `jal 0x80027160` @0x8002689c; FUN_80027160 setzt die Part-Weltmatrizen
  (+0x48 = `puVar3 = param_2 + 0x12`, RE2_Quellcode_V2/FUN_80027160.c) über FUN_80027434/FUN_80027ff0 (Eltern × Lokal +0x18).
  ERST danach läuft die Objekt-Schleife `jal 0x800363f0` @0x80026964: Objekte ab 0x800d0324 im Abstand 504 (`addiu s4,s4,804` /
  `addiu s6,s1,504` @0x80036434-40 → obj[1] = 0x800d051c = der Kanister), `RotMatrix(+0x74 → +0x24)` @0x80036468-70, `lw a0,128(s1)`
  (Eltern-Zeiger) + `jal 0x8002ce94` @0x80036480-84 = Welt = Part6(dieses Bild) × Lokal. Die Effekt-Pumpe `jal 0x8001d300`
  @0x80026980 läuft hinter beiden Schleifen — damit ist auch der OFFEN-Punkt "Effekt-Quellmatrizen dieses oder des vorigen Bildes"
  (Dossier §12.4) beantwortet: dieses Bild, wie der Port es jetzt macht.

### 3.2 Punkt 2 / Abnahme-3-M2 — keine Gier-Sprünge in Verfolgung/Belagerung → **behoben, gemessen**
- `gier.py <state.log> 1 9999 96` (Sprünge > 0x60 je Bild): A2 nur F6 (+576 Intro-Start aus dem Versteck) und F280 (+1024);
  D2 nur F280/F786 (+1024); C3 nur F280/F934 (+1024) und F393 (+348 CROSS-Start, Bahn-Blick, ausgenommen); E1 nur F6 (−352) und F280
  (+1024); N/NZ nur F6 (Lauer-INIT, untergetaucht), F201 (+447 Auftauchen), F475/F931 (+1024). Vorher (Abnahme 3) A2 F379 +1650,
  C3 F1097/F1207/F1327 ±1474..1490, D2 F1075 −1302, N F1057 +1557, E1 F426…F546 ±1944..1984.
- gator_boss.log `GIER-WACHE`: D2 `bis F980: 300 Bildpaare … max 96, Spruenge > 0x60: 0, Befreiungen 0`, C3 `bis F1302: … max 96,
  … 0, … 0`, E1 `bis F581: … max 64 … 0 … 0`, Z `bis F575: … max 96 … 0 … 0`; keine `GIER-SPRUNG`-/`BEFREI`-Zeile in irgendeinem Lauf.
- Prüfer-Sonden (Harness = Kopf von test_r41_re2gator.c, `B41abn4/probe/`, gegen die gebaute Bibliothek UND gegen den Vorher-Bau):
  - `probe_kopfwand` (5 Leon-Lagen × 800 Takte): neu Gier max 64/96/64/96 und 0 Sprünge in den Lagen E1/C3/Block/Nord, Lage Ost
    1 Sprung (+413 = CROSS-Start); vorher E1 15 Sprünge (max 1984), C3 9 (1954), Nord 3 (1967), Ost 8 (2016).
  - `probe_sprung` (neu; 3 Lagen × 800 + Raster 7 × 6 Leon-Lagen × 600 Takte, je Sprung Phase/Clip): 13 Sprünge, alle Phase 2 → 7
    (CROSS-Start, +361..+1055), 0 Sprünge CHASE/GUARD → CHASE/GUARD, 0 Befreiungen.
  - `probe_cross` (neu; Leon wechselt alle 120–300 Takte die Seite, 32000 Takte, 34 CROSS-Starts): 0 Befreiungen, 0 Gier-Sprünge in
    CHASE/GUARD, längster Stillstand in CHASE 14 Takte (kein Hänger).
- Wand-Klemm-Zittern (Nebenwirkung, `zittern.py`, C3, x < −7300): vorher 277 Bilder / 19 Richtungswechsel z ≥ 20 (max 50), jetzt
  162 / 2 (max 31).
- Der Befreiungs-Schub (neue Form, PORT-WAHL) trat in keinem exe-Lauf und in keiner Sonde auf — nur im Riegel sitz (c), dort
  17 Schub-Bilder mit fester Gier.

### 3.3 Übrige Teile der Punkte 1 und 2 (Regression gegen Abnahme 3)
- **Untergetaucht bis zum Durchbruch (B9):** N `Sicht F1 untergetaucht` → `Sicht F201 aufgetaucht pos=(5085,-1200,-19880) clip=6
  bild=0`, `Lagesprung max 338 F229`; `vergleich.py mess/N/fd mess/N0/fd n_ 1 300` → **F1–F201 201/201 bytegleich** mit "ohne
  Gator", erste Abweichung F202. NZ (Elza, `JUMP -> 209 ALLIGATOR POOL (ROOM2091)`, `Spieler-Familie PL04`): Clip-Folge, Auftauchen
  F201 Clip 6 Bild 0 und Lagesprung 338 identisch mit Leon. E1 (Türweg 20A0 → 2090, `[aot] DOOR FIRE slot=0 … spawn=(6300,-1800,
  -16250)`, `Sequenz fertig: 286 Bilder`): `Sicht F6 aufgetaucht pos=(-6604,-1200,-10154) clip=6 bild=0`.
- **RE2-Animationen (Punkt 1):** A2 Clip-Läufe F1–F5 0 (unsichtbar) · F6–F130 **6** · F131–F280 **3** · F283–F325 4 (Leon 60 → 10
  F290) · F360–F389 1 · F390–F411 4 (Leon 10 → −40 F397) · F412–F531 **5** · F532–F651 **11** · F652– 0 (5 Bilder früher als in
  Abnahme 3, weil der Befreiungs-Snap F379 und seine Rückdrehung fehlen). D2 F487–F636 **2**, F637–F786 **3**; K F70–F260 **9**.
  **Dash jetzt animiert (Nebenbefund der Nachbesserung, gemessen):** C3 `RE15_POSE_DUMP` Dash-Clip 2 F635–F784: 150 verschiedene
  Bone-9-Posen, 147 verschiedene Bone-13-Posen, `frac` 16 → 0 in 16 Bildern; vorher (Bau-Agent `B41nb4/mess/C3v/pose.log`, Stand
  47efb932) F644–F794 151 Bilder bitgleiche Pose.
- **Finisher-Kamera bis Game Over (B6), Kamera-Programm (Nachtrag 4):** A2 `RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 6 F398`,
  14 Zeilen `Halt Cut 6` F399…F659, `RE1.5-Todesbild unterdrueckt (cam=0 weiss=1 schwarz=0) ab F462`; Bogen `B41abn4/A2_finisher.png`
  (Seitensicht, YOU DIED ab F520, keine Weißblende, kein Orbit), Leon sichtbar im Maul `B41abn4/beleg_A2_finisher.png` (F416–F504).
  N `DURCHBRUCH RE2-Cut 10 -> Cut 1 F201`, `SEITE … Cut 1 F271`, `ANGRIFF … Cut 1 F325`, `zurueck -> Cut 7 F475`; A2 Cut 0 F6 / Cut 1
  F76 / Cut 7 F130; K `R1 EXPLOSION RE2-Cut 5 -> Cut 1 F70`, `zurueck -> Cut 0 F106`, `frei … F108`.
- **Dash/Schreie/Schütteln/Putz:** D2 `RE2 DASH Planung: an (-6052,-18113) yaw 1139 -> Start-Gier 624`, `Vordrehung … F481`,
  `RE2 PUTZ runde 6/6 spawns=9 gesamt=54 um=(-7097,0,-24000)`, `DASH ende … wand_draussen_max=0`, 25 `Putz-Sicht`-Zeilen ab F488,
  27 Bank-17-SE-Zeilen. Schütteln (`schuettel.py mess/D2/fd d_ 536 560 529`): F542 (0,2) F543 (0,0) F544 (0,2) F545 (0,3) F546 (1,0)
  = Modus-2-Tafel @0x8009db14 Index 0–4, F547–F556 (0,0)(0,1)(0,0)(0,0)(0,0)(0,1)(0,1)(0,0)(0,0)(1,1) = Modus-1-Tafel @0x8009daf4
  Index 5–14 — Bild für Bild wie in Abnahme 3.
- **Klettern bleibt:** C3 `CROSS start=(4450,-11900) ende=(3350,-20700) frames=240 fenster=62..177`, Clip 0 F393–F634.

### 3.4 Nebenwirkung der M2-Änderung im echten Türweg (E1): Belagerung endet mit Leons Tod, Finisher mit leerem Maul → **MANGEL**
- Weg: `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1` → debug.log `[aot] DOOR FIRE slot=0 rect=(4950,5000,hw=1500,
  hh=500) target_cut=2 spawn=(6300,-1800,-16250)`, `[tuer] Sequenz fertig: 286 Bilder` — Leon kommt aus dem Service-Tunnel OBEN
  auf der Ostrampe an (genau der Ankunftsort, an dem nach Nachtrag 6 künftig die Szene der Spur F endet und der Kampf beginnt) und
  bleibt dort stehen.
- Belagerung (GUARD, Nutzer-Design v0.7.42 "Hochbiss"): gator_boss.log `GZIEL kante=(6500,-12600) zg=1 rampe=1`, `GPAT
  ziel=(6500,-12600)` — das Ziel liegt 700 vor der Ostwand, also hinter der Klemmlinie 7200 − 1466 (ALT, vom Bau-Agenten selbst
  OFFEN geführt). Seit der Nachbesserung bleibt der Gator dort an der Klemmlinie x 5716 und schaut mit Gier 13–41 (Osten) in
  die Wand (`B41abn4/gierliste` E1 F443–F523), statt alle 32 Bilder wegzuschnappen.
- Folge, gemessen mit demselben Rezept auf beiden Bauständen (je 1500 Bilder, `mess/E1neu_lang`, `mess/E1alt_lang`):
  vorher (47efb932) Hochbisse F417 (100 → 50), F528 (50 → 0), danach keiner mehr — **Leon überlebt**; jetzt F417, F522 (50 → 0),
  **F627 (0 → −50) → Tod**, FRESSEN-Finisher Clip 4 F627–F641 / 5 F642–F761 / 11 F762–F881, `RE2 KAMERA FINISHER … Cut 7 -> Cut 2
  F628`, Halt Cut 2 bis F889, YOU DIED.
- Der tödliche Biss kommt mit Gier 115, Leon liegt von dort unter ~840 (64° daneben). FRESSEN P0 setzt Leon auf Gator +
  RotY(Gier) × (10643 × 2731/4096 = 7096, −915) (Code-Kommentar @0x8010106c-84, PORT-VERALLGEMEINERUNG des RE2-Korridors), P2/P3
  halten ihn 3300–5400 vor dem Gator (`relativ.py`): state.log `F638 … Leon(12538,-14909)`, FSYNC2 `pl=(10374,-1200,-14727)` …
  `(9668,-1200,-13306)`, FSYNC3 `(9517,-1200,-13306)` — 2300–5300 HINTER der Ostwand (x 7200). Im Bild (Cut 2, Blick nach Osten)
  ist das Maul leer: F628–F696 und F736–F868 kein Leon, F700–F728 schwebt Leons Körper getrennt vom Kopf neben dem Rohr
  (`B41abn4/beleg_E1_finisher.png`, `E1_fress_zoom2.png`, `E1_fress_p3_zoom.png`, Übersicht `E1_belagerung_tod.png`).
- Gegenstück A2 (Türnische, derselbe Finisher): Leon liegt dort nur ~1260 hinter der Westwand, Cut 6 zeigt ihn im Maul
  (`beleg_A2_finisher.png`, F416–F504) — der Finisher selbst ist also in Ordnung, sein Ort an der Belagerungswand nicht.
- Einordnung: Die Bausteine (GUARD-Ziel hinter der Klemmlinie, Hochbiss ohne Blick auf Leon, P0-Versatz entlang der Gier) sind
  ALT; NEU durch die Nachbesserung ist, dass der natürliche Türweg-Ablauf jetzt zuverlässig in diesem Tod endet (vorher 1500
  Bilder überlebt). Gegen den Wortlaut von Punkt 1 ("den Finisher … aus Resident Evil 2") ist ein Finisher, in dem das Maul leer
  bleibt und Leon neben dem Kopf schwebt, nicht der RE2-Finisher.
- Dazu passend (ALT, durch die Nachbesserung etwas häufiger, Prüfer-Sonde `probe_kopfwand`, Knochen 0/1/5/6/7 außerhalb der
  Beckenwände bzw. im Plattformblock je 800 Takte, vorher → jetzt): E1 379 → 497 Takte, C3 535 → 569, Nord 361 → 383, Ost
  394 → 400, Block 26 → 26 (Schnauze lokal 2500: E1 562 → 653). Der Kopf steckt beim Wand-Druck in der Wand; sichtbar wird das
  vor allem im Finisher oben.

## 4. Gates

### 4.1 RE-Gate
- Diff-Grep (nur `+`-Zeilen) nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|XXX|vorerst|geschaetzt|ungefaehr|
  approx": Nachbesserung 4 (`git diff d4e36ca6..HEAD -- re15_port`) **0 Treffer**, Zweig gesamt (`master...HEAD`) **0 Treffer**.
  Kein neuer `getenv` in der Nachbesserung 4 (kein Env-Schalter als Abschluss).
- Konstanten der Nachbesserung 4: Überblend-Abbau −1 je Takt (@0x800299c0-cc), Rate 4096/(n+1) (@0x8001a37c-ac), n = 32/4/16/3
  (@0x80100c38 / @0x80100cfc / @0x80100dac-b4 / @0x80102c44), Anhängen bei t == 13 gegen die Part-6-Weltmatrix (@0x801018c4-f0),
  Kopf-az +0x22A (@0x8010037c-90) — alle mit Adresse im Kommentar und in den Commit-Texten (642ad014, b20f1496). Befreiung als Schub
  (GB_SWIM_SPEED 48 je Takt, Ausgang Slab-Rand + 120, befrei = 2 — die Werte der bisherigen Befreiung) ist als PORT-WAHL benannt
  (enemy_ai_boss_gator.c:1400-1411, Dossier §12.2). GIER-WACHE-Schwellen (0x60 = Port-Wenderate, +1024 @0x80100938-48 /
  @0x80100eb0-c0, 300 = Zeilentakt) sind Messzeilen ohne Verhaltenswirkung.
- Selbst disassembliert/gelesen und bestätigt (§2, §3.1): Kanister-Anhängen @0x801018c4-918, Root-Reihenfolge @0x80100200-3a4,
  FUN_8001a330/0x8002959c/0x80029614 (Rate, Mischung, Abbau), die vier Blend-Argumente, die Gier-Schreiber des Overlays, dazu
  die RE2-Hauptschleife (Entity-Schleife 0x800267c0-930 mit Zeichnen 0x80027160, Objekt-Schleife 0x800363f0 @0x80026964,
  Pumpe @0x80026980). **Keine falsche Adresse.**
- "Erklärt der Fix den Befund?": M1 ja — Ursache war Zeitpunkt (Takt-Anfang) UND Pose (Keyframe ohne +0x22A/Überblendung); beides
  ist im Code ersetzt (gr2_kan_folgen nur noch aus re15_gr2_nachtakt, Pose = Zeichner-Pose mit Schnappschuss-Schutz), und die
  echte Zeichner-Messzeile zeigt 0 in 270/420/52 Bildern. M2 ja — die einzigen Gier-Schreiber mit 0x800 in CHASE/GUARD waren die
  zwei Befreiungs-Zweige (enemy_ai_boss_gator.c alt Z. 1286/1326), beide ersetzt; Sprünge in CHASE/GUARD sind in allen Läufen und
  Sonden weg, gegen den Vorher-Bau gegengeprüft.

### 4.2 Vertrag
- Bank-9-Bits / Nachrichten-Ids: keine (`git diff master...HEAD -- re15_port` ohne game_flag_set/g_game.flags/MSG_ID-Schreiber).
- Pfade (`git diff master...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine
  CMakeLists; RE2-Daten nur unter shared_assets/RE2/ (GATOR40A0.VBS, KANISTER40A0.MD1/.TIM, seit Abnahme 0 im Dossier genannt).
  Nachbesserung 4 berührt NUR Spur-B-Dateien (enemy_ai_boss_gator.c = Eigentümer B, gator_re2_r41.c, gator_re2_pc.c, die zwei
  Header, Riegel); keine neuen Haken in gemeinsamen Dateien (main.c/enemy_ai_common.c/skeleton_common.c unverändert). Die
  Dispatcher-Klemme in enemy_ai_common.c bleibt unverändert und fragt weiter `re15_gator_boss_skip_clamp` — die Verlagerung ist
  über den Rückgabewert gelöst (für den aktiven Boss 1, sonst 0 → Typ-0x23-Alligator außerhalb 2090 klemmt wie bisher).
- Tests: neu `unit_r41_re2gator_sitz` (messend: |Kanister − Part6(gezeichnet) × lokal| je Bild in vier Lagen, Effekt-Quelle),
  `unit_r41_re2gator_gier` (messend: Gier-Delta je Bildpaar, ≥ 300 Paare je Lage, GUARD/Wandlage erreicht), integration Läufe
  "kanister" (Kanister-Sitz ≥ 200 Bilder, max 0) und "belagerung" (GIER-WACHE), GIER-SPRUNG-Prüfung in allen fünf Läufen.
  Parallel-fest (eigener Laufordner `lauf_<zufall>`, eigene exe-Kopie `re15_pc_r41b_<zufall>.exe`). Kein Riegel prüft, ob Leon im
  Finisher sichtbar im Maul liegt bzw. innerhalb der Beckenwände bleibt (§3.4).

### 4.3 Nebenwirkungen
- Elza (ROOM2091, NZ): identisch mit Leon (§3.3). Echter Türweg 20A0 → 2090 (E1): Gier-Sprünge weg, ABER Belagerung endet jetzt
  mit Tod und leerem Finisher-Maul (§3.4). Granaten-/Wasser-/Zombie-/Türen-Querriegel siehe §5.

---

## Fortsetzung (Prüfer 2, ab 2026-10-24 12:50; die Vorgängerin wurde ~12:23 vom Sitzungslimit beendet)

Gleicher Baum, gleicher Bau (re15_pc.exe 11:40:40; seit b20f1496 nur Doku-Commits 11c7a348 und c36d537c =
AUFTRAG/VERTRAG vom master mit Nachtrag 6-11, kein Code). Messungen der Vorgängerin werden nicht wiederholt; neue Dateien
im Prüfer-Scratchpad `.../scratchpad/B41abn4f/` bzw. `B41abn4/mess/RN|RA` (Läufe mit den Skripten der Vorgängerin).
Nicht ausgewertete Reste der Vorgängerin: Lauf EN (Türweg 20A0 → 2090 OHNE GB_TEST, Autopilot `xz:-1200,-13200;
xz:6300,-16250`, 2400 Bilder): `Sicht F1 untergetaucht`, danach nur `TICK ph=1` (Lauern) bis zum Ende — ohne Testhebel
taucht der Gator auf diesem Weg in 2400 Bildern nicht auf (Auslöser des Intros = Spur F, Nachtrag 6). Prüfer-Sonde
`probe/probe_tod.c` (Leon als Statue, Riegel-Harness): auch der VORHER-Bau tötet Leon an der Rampe (Lage E1 Takt 159
Gier 703, Lage E1b Takt 136 Gier 159 — E1b bitgleich mit dem neuen Bau), in der Türnische beide Takt 9.

### 3.4 (Fortsetzung) — ist der leere Finisher eine Regression der Nachbesserung 4?

**(a) Treffer ohne Blick ist ALT (gemessen am Vorher-Bau).** `B41abn4f/bisse.py <state.log>` (je HP-Abzug Gator-Gier gegen
die Peilung Gator → Leon, Konvention wie `relativ.py`):
- E1alt_lang (Vorher-Bau 47efb932): `F417 hp 100 -> 50 | Gator mo=4 af=18 (5716,-12406) Gier 169 | Leon (6300,-16250)
  Abstand 3888 Peilung 926 -> daneben +757 (67 Grad)`, `F528 hp 50 -> 0 | Gator mo=4 af=6 (5668,-12827) Gier 2431 | Leon
  (6732,-16250) Abstand 3584 Peilung 828 -> daneben -1603 (-141 Grad)` — der alte Gator biss Leon also sogar mit dem
  Rücken zu ihm.
- E1neu_lang (= E1, bitgleich): F417 wie oben, `F522 hp 50 -> 0 … Gier 13 … daneben +835 (73 Grad)`, `F627 hp 0 -> -50 |
  Gator mo=4 af=0 (5716,-12762) Gier 115 | Leon (6732,-16250) Abstand 3632 Peilung 839 -> daneben +724 (64 Grad)` → Tod.
- A2 (Türnische, CHASE-Biss): `F290 hp 60 -> 10 … Gier 1471 … daneben -7 (-1 Grad)`, `F397 hp 10 -> -40 … Gier 2569 …
  daneben +2 (0 Grad)` → Finisher mit Leon im Maul. (F57/F175 je −20 im Intro bei Abstand 9327/8928 = Spinnen, t=25.)
- Code: der GUARD-Hochbiss (enemy_ai_boss_gator.c:2234-2251) prüft nur `dist < 4200` (Start) und `dist < 3900` im Fenster
  Bild 6..34, keine Blickrichtung; `git blame`: 68f37634 (2026-09-10), c9f74499 (2026-09-11), nur die Kanister-Sperre
  `re15_gr2_biss_erlaubt` ist von Spur B (b5516dda, NB2). FRESSEN P0 = Gator + RotY(Gier) × (10643·s, −915)
  (Z. 2409-2423): 9b6e0b35 (2026-09-12) / e3fb82bb (2026-09-19), in master unverändert. Das GUARD-Ziel (Z. 2052-2061,
  `gx = pl->x` geklemmt, `gz = Rampenkante ± GB_GPAT_M`) ist ebenfalls ALT; es liegt in E1 bei (6500,−12600), östlich der
  Klemmlinie x 5716 — der Gator steuert mit 0x40/Bild auf dieses unerreichbare Ziel (Gier 13…115) statt auf Leon (Peilung ~840).
