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

**(b) Vorher/nachher am selben Ankunftsort (exe, Sprung statt Tür, sonst E1).** `RE15_DEBUG_JUMP=2090@250
RE15_PLAYER_POS=6300,-16250,2048,1` (Band 1 = y −1800, Leon steht wie nach der 20A0-Tür oben auf der Ostrampe)
`RE15_GB_TEST=1 RE15_EXIT_AT=1500#2090`: Vorher-Bau `mess/RA` (`lauf_alt.sh`): Bisse F417 (Gier 169, 67°) und F528 (Gier 2431,
−141°), danach keiner bis F1500 — Leon überlebt (HP 0); neuer Bau `mess/RN`: F417, F522 (Gier 13, 73°), **F627 (Gier 115, 64°)
→ Tod**, `GUARD … pitch=-350 arc=-220 … pos=(5763,-12583)` vor dem Tod, Finisher-Kamera Cut 7 → Cut 2 wie E1. Beide Läufe
treffen die Biss-Bilder von E1alt_lang bzw. E1/E1neu_lang Bild für Bild — der Sprung-Weg ist für diese Frage gleichwertig zum
Türweg. **Der Ablauf am Ankunftsort ändert sich also durch die Nachbesserung 4 von "überlebt" zu "stirbt mit leerem Maul".**

**(c) Was die Nachbesserung 4 dabei geändert hat — und was nicht (Code, `git show 47efb932:… / HEAD:…`).**
- Unverändert seit 47efb932 (Stand der Abnahme 3): der ganze `case GBP_FRESSEN` (187 Zeilen bitgleich), `gb_biss_abschluss`
  (gegen master nur `re15_gr2_biss_cd` im NICHT-tödlichen Zweig) und der ganze `case GBP_GUARD` (gegen master nur
  `re15_gr2_biss_erlaubt`, NB2). Geändert hat die Nachbesserung 4 die Befreiung (kein Sofort-Snap mehr, nur noch Schub im
  echten Gefängnis), den Zeitpunkt der Wand-Klemme (Takt-Ende) und die Überblendung.
- Folge in E1/RN: ohne den 32-Bild-Snap bleibt der Gator an der Klemmlinie x 5716 auf sein unerreichbares GUARD-Ziel
  (6500,−12600) gerichtet (Gier 13…115) und beißt im 105-Bild-Takt weiter (F417/F522/F627) — der dritte Biss tötet.
- Warum das Maul leer ist (Code-Befund, ALT, nicht von Spur B): im GUARD wird je Bild `g->pitch_vz = -(350*guard_t)/24` und
  `g->arc_vz = -(220*guard_t)/24` gesetzt (Z. 2232-2233, 68f37634 2026-09-10); der tödliche Biss (`gb_biss_abschluss`, Z. 421,
  12d3b0c3 2026-09-11) nullt nur `arc_vz`; `pitch_vz` bleibt durch P0–P3 stehen und wird erst am FRESSEN-Ende genullt
  (Z. 2564). Der Zeichner neigt damit die Wurzel (`re15_gator_spine_arc_vz` Bone 0 → `pitch_vz`, Z. 2751) um −350
  (≈ −31°), während P0/P2/P3 Leon aus dem ungeneigten Rig setzen (`gb_opfer_skalieren`, Anker-Paar; kein `pitch_vz` im
  FRESSEN-Fall). In A2 (Tod aus dem CHASE-Biss, pitch 0, Biss mit Blick auf Leon: F397 Gier 2569 / Peilung 2571) liegt Leon
  sichtbar im Maul, obwohl er dort ebenfalls 1100–2650 hinter der Westwand steht (FSYNC2 x −10041…−11556); in E1/RN liegt er
  3200–3900 hinter der Ostwand (FSYNC2 x 10374…11062). Das ist eine aus dem Code gelesene Ursache, nicht gemessen — die Messung
  des Abstands "gezeichneter Kiefer ↔ Leon" je Bild ist Teil des Mangels (§8).

## 5. Integration (gezielt, am gebauten Stand)
- Lauf der Vorgängerin (12:09, `-R "^integration_(r41_re2gator|r36_schuss|r38_wasser|r35_granate|r37_itemband|r36_tueren|
  r37_re2zombie)$"`, `B41abn4/ctest_integ.log`): 6 von 7 grün (r38_wasser 162,7 s, r35_granate 163,4 s, r37_re2zombie 51,9 s,
  r36_tueren 27,5 s, r36_schuss 143,2 s, r41_re2gator 203,7 s); r37_itemband lief beim Sitzungslimit noch.
- `ctest --test-dir re15_port/build -R "^integration_r37_itemband$" --timeout 1800 --output-on-failure` einzeln
  (12:47:32–13:01:00, `B41abn4/ctest_itemband.log`): **Passed 807,18 s** → zusammen **7/7 grün**. Die beim Abbruch
  liegengebliebene Kopie `re15_pc_r37d_haken_2b5e9bd5.exe` hat der Test selbst entfernt; kein Prozess aus diesem Baum lief
  mehr (geprüft per Get-Process, nur fremde Prozesse aus r41_ventilkette).
- Unit-Riegel: 112/112 (§1), dazu `unit_gator_sweep` (`gator-sweep: 3342 Paare, 0 nicht konvergiert`, `gator-biss: 80 Faelle,
  0 ohne Treffer`) und `unit_gator_fress` (`PIN P3 … dRef=(-2,-1,-1)`, `Maulraum … 776/776 = 100.0%`) einzeln mit -V
  (`B41abn4/unit_sweep_V.txt`). Der Fress-Riegel misst den Finisher nur aus einer CHASE-Lage (Gier 4095, pitch 0) — die
  GUARD-Lage aus §3.4 deckt er nicht ab.

**(d) Tödlicher GUARD-Biss im Vorher-Bau: der Finisher ist dort ebenso kaputt (ALT, gemessen).** Leon mittig-nord auf der
Rampe, wo das GUARD-Ziel erreichbar ist: `RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=4000,-15000,2048,1 RE15_GB_TEST=1`, Dump
nur im Finisher-Fenster und `EXIT_AT` VOR dem Raumwechsel (sonst überschreibt der Neustart in ROOM1170 nach dem Game Over die
Framedump-Dateien, die je Raum nummeriert sind — so geschehen in den ersten Läufen `mess/MA`/`mess/MN`):
- Vorher-Bau `mess/MA2` (`EXIT_AT=850#2090`, `FRAMEDUMP=560-850/2`): Bisse F377 (50°), F484 (−79°), **F608 (Gier 1911, Leon
  33° daneben) → Tod**, `GUARD … pitch=-350 arc=-220` davor, `RE2 KAMERA FINISHER … RE2-Cut 7 -> Cut 7 F609` (Kandidatenzeile
  `Cut 7 kopf_bild=(265,105) massstab=627`), FSYNC2 `pl=(27,-1200,-12222)`, `(19,-1200,-14008)`, `(-357,-1200,-13104)` = am bzw. im
  Plattformblock (x −1700…1850, z −20000…−12450). Bild (`B41abn4f/MA2_finisher.png`, `MA2_kopf_zoom.png`): in Cut 7 ist von F609
  bis zum YOU DIED **kein Gator zu sehen**; Leon steht F610/F620 noch klein links der Maschine und ist ab F630 verschwunden.
- Neuer Bau `mess/MN2` (`EXIT_AT=830#2090`, `FRAMEDUMP=540-830/2`): F377 (50°), F482 (Gier 83, 61°), **F587 (Gier 3405, Leon 128°
  daneben) → Tod**, Gator an der Klemmlinie (5716,−12583), `RE2-Cut 7 -> Cut 4 F588`, FSYNC2 `pl=(9100,-1200,-8831)`,
  `(7762,-1200,-7648)`, `(8689,-1200,-7960)` = hinter der Ostwand. Bild (`MN2_finisher.png`, `MN2_kopf_zoom.png`): der Gator bäumt
  sich sichtbar auf und schüttelt, das Maul ist leer; Leon schwebt F610–F650 klein links unter dem Kopf, F660/F670 über dem Kopf,
  F680–F700 neben dem Hals.
- Die ersten Läufe `mess/MA`/`mess/MN` (gleiches Rezept, `EXIT_AT=1500`) sind für den Ablauf bitgleich (gleiche Biss-Bilder), ihre
  Bilder ab ~F580 zeigen aber schon den Neustart.

**(e) Antwort auf die Frage der Fortsetzung — Regression der Nachbesserung 4?**
- **Im Ablauf ja:** am Ankunftsort der 20A0-Tür (E1 Tür, RN Sprung) überlebte Leon im Vorher-Bau 1500 Bilder (2 Bisse), jetzt
  stirbt er bei F627 — Folge der M2-Änderung (kein Snap: der Gator bleibt an der Klemmlinie auf das GUARD-Ziel gerichtet und beißt
  weiter).
- **Im Defekt nein:** der Finisher nach einem tödlichen GUARD-Biss zeigt Leon schon im Vorher-Bau nicht im Maul (MA2: Kamera ohne
  Gator, Leon verschwindet); FRESSEN, `gb_biss_abschluss` und GUARD sind seit 47efb932 unverändert (c). Abnahme 1–3 haben den Fall
  nicht gesehen, weil alle Todesläufe aus einem CHASE-Biss kamen (A2) und E1 nur 900 Bilder ohne dritten Biss lief; auch der Fress-
  Riegel misst nur eine CHASE-Lage (§5).
- **Der Bau-Agent hätte es sehen können:** alle drei Läufe des Integrationsriegels "belagerung" (`EXIT_AT=700#2090`, Rezept E1;
  `build/tests/integration/r41_re2gator_wd/lauf_e6c7aac8` 11:21, `lauf_afc79e09` 11:34 — beide vom Bau-Agenten —, `lauf_cd8d5830`
  12:16) enthalten `RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 2 F628` und `Halt Cut 2 F629`; der Riegel prüft nur GIER-WACHE und
  die Zahl der GUARD-Zeilen.
- **Einordnung:** ALT, aber sichtbar, gegen den Wortlaut von Punkt 1 ("den Finisher … aus Resident Evil 2": im RE2-Finisher liegt
  Leon im Maul, so wie hier in A2) und seit der Nachbesserung 4 auf dem natürlichen Weg (20A0-Tür → Ostrampe, wo der Spieler steht
  und schießt) erreicht. Nach dem Maßstab der Abnahme 3 (deren M2 war ebenfalls ALT) ist das ein **Mangel dieser Abnahme (M1, §9)**.

## 6. Nebenbefunde (keine Mängel dieser Abnahme, zur Kenntnis)
- **Türweg 20A0 → 2090 ohne Testhebel (EN):** der Gator bleibt 2400 Bilder untergetaucht am Lauerplatz (−7200,−9200), obwohl Leon
  per Autopilot über die Rampe bis (−1200,−13200) und zurück läuft — der Auslöser des Kampfes auf diesem Weg ist die Szene der
  Spur F (Nachtrag 6: "… Macht danach Richtung Leon eine schrei und Schüttel Animation. Danach geht der Kampf los"). Alle
  Belagerungsläufe dieser Abnahme brauchen deshalb `RE15_GB_TEST=1`.
- **Spinnen in 2090** ziehen Leon weiter je 20 ab (A2 F57/F175 bei 9327/8928 Abstand zum Gator, BA/BN F382/F515/F873) — Spur F
  entfernt sie (AUFTRAG).
- **Vorher-Bau MA2:** die Finisher-Kamera wählt Cut 7 (Kandidatenzeile `kopf_bild=(265,105)`, also im Bild), im Bild ist aber kein
  Gator zu sehen; die Ursache (Verdeckung/Licht) ist nicht geprüft. Im neuen Bau zeigen die gewählten Schnitte den Gator (E1/RN
  Cut 2, MN2 Cut 4) — Teil der Forderung von M1 (Kamera zeigt Gator UND Leon).
- **Prüfer-Sonde `probe_tod`** (Vorgängerin): Leon bleibt nach dem Tod an seiner Statuenlage stehen (der Harness hat keinen
  Spieler-Schritt) — sie misst nur den Todeszeitpunkt und die Gier, nicht die Finisher-Lage.
- **Messfalle (für spätere Abnahmen):** `RE15_FRAMEDUMP` nummeriert je Raum; nach dem Game Over startet ROOM1170 neu und
  überschreibt die 2090-Bilder gleicher Nummer → `RE15_EXIT_AT` vor den Raumwechsel legen (MA/MN → MA2/MN2).
- **Befreiungs-Schub (PORT-WAHL der Nachbesserung 4):** in keinem exe-Lauf und keiner Sonde aufgetreten (`probe_cross` 32000 Takte,
  34 CROSS-Starts: 0 Befreiungen), nur im Riegel `sitz (c)` (17 Schub-Bilder) — die vom Bau-Agenten offene Sichtprüfung im Spiel
  ist damit praktisch nicht erreichbar.

## 7. Behauptungen des Bau-Agenten — Abgleich am gebauten Stand
| Behauptung | gemessen | Ergebnis |
|---|---|---|
| M1: RE2 hängt den Kanister als Kind an die Part-6-Weltmatrix (`addiu a0,s0,1104` @0x801018d0, `sw a0,128(s2)` @0x801018e0, FUN_8002d030 @0x801018f0, Kopf-az +0x22A @0x8010037c-90) | §2 selbst disassembliert; dazu RE2-Hauptschleife: Zeichnen 0x80027160 je Aktor VOR der Objekt-Schleife 0x800363f0 | ✓ |
| Wand-Klemme am Takt-Ende, `re15_gr2_nachtakt` rechnet die gezeichnete Pose, Anhängen, Weltlage, Effekt-Quellen | Code gelesen (§1, §4.1); echte Zeichner-Messzeile 0 in 270/420/30 Bildern (C3/Z/K) | ✓ |
| Riegel sitz 0 bei Wende, Biss, Befreiung, Überblendung | `unit_r41_re2gator_sitz` (a)–(d) je 0 | ✓ |
| Gegenprobe alter Zeitpunkt: Wende 626 | passt zur eigenen Messung der Abnahme 3 (619–637 je Bild) | ✓ (nicht wiederholt) |
| exe C3: Kanister-Sitz 270 Bilder, max 0 | `Kanister-Sitz F1388: angehaengt 270 Bilder, Abweichung max 0 (F1119)`; Bildbögen der Vorgängerin (Wende/Biss mit Kanister im Maul) | ✓ |
| Nebenbefund Überblendung 4096/(n+1) + Abbau; Dash-Pose vorher 150 Bilder eingefroren | §2 selbst gelesen; C3 POSE_DUMP: 150 verschiedene Bone-9-Posen F635–F784 (vorher bitgleich) | ✓ |
| M2: RE2 schreibt die Gier in Sub 1/2 nie | Volldisassembly EM23 (§2): kein Gier-Schreiber in Sub 1/2/T2 | ✓ |
| PORT-WAHL: Befreiung nur im echten Gefängnis, Schub ohne Drehung | Code + Kennzeichnung; nur im Riegel aufgetreten (§6) | ✓ |
| Riegel gier E1/C3 je 800 Takte max 64/96, 0 Sprünge; Gegenprobe alter Snap ±1984 | Riegel-Ausgabe; Prüfer-Sonde `probe_kopfwand` alt/neu (E1 15 → 0 Sprünge) | ✓ |
| exe E1, C3, A2, D2, N, NZ ohne Sprung außer +1024 | gier.py: nur +1024, Intro-/Lauer-/CROSS-Starts (keine CHASE/GUARD-Bilder) | ✓ |
| Sweep 3342/0 | `gator-sweep: 3342 Paare, 0 nicht konvergiert` | ✓ |
| K, Intro, Dash/Putz und Elza unverändert | K/D2/NZ wie Abnahme 3 (Schütteln Bild für Bild), A2-Clipfolge 5 Bilder früher (fehlender Snap) | ✓ |
| Unit 110, Integration 7 bzw. 2 grün | Unit 112/112 (Obermenge), Integration 7/7 (§5) | ✓ |
| (nicht berichtet) Folgen von M2 im echten Türweg | E1/RN: Tod F627, Finisher ohne Leon im Maul (§3.4); in den eigenen Riegel-Läufen "belagerung" 11:21/11:34 als `… -> Cut 2 F628` protokolliert, nicht geprüft | ✗ |

## 8. Urteile je Nutzer-Punkt
| Punkt (Wortlaut AUFTRAG.md) | Urteil | Beleg |
|---|---|---|
| 1. "den Finisher und fehlende Animationen aus Resident Evil 2 extrahierst" | **teilweise** | RE2-Clips im Spiel (A2 6/3/4/1/5/11, D2 2/3, K 9, Z 10 (R4 F70–F98)), RE2-FRESSEN + Finisher-Kamera bis Game Over aus einem CHASE-Biss mit Leon im Maul (A2 F398–F659, `beleg_A2_finisher.png`). ABER: nach einem tödlichen GUARD-Hochbiss (Leon oben auf der Rampe = Ankunftsort der 20A0-Tür) liegt Leon nicht im Maul — E1/RN F628–F868 (Cut 2), MN2 F588–F700 (Cut 4), im Vorher-Bau MA2 sogar ohne Gator im Bild; seit der Nachbesserung 4 auf dem Türweg erreicht (§3.4, M1) |
| 2. Verfolgung wie RE2 (Animation, Schreie, Beschleunigung an gleichen Stellen), Wände wackeln + Putz, Klettern bleibt | **erfüllt** | Gier-Sprünge der Abnahme 3 (M2) weg: gier.py/GIER-WACHE in A2/C3/D2/E1/N/NZ/Z, Riegel gier, Sonden `probe_kopfwand`/`probe_sprung`/`probe_cross` gegen den Vorher-Bau (§3.2); Dash Clip 2/3 mit RE2-Planung, 27 Bank-17-SE, Schütteln Bild für Bild = Tafeln @0x8009db14/@0x8009daf4, Putz 6 × 9 = 54 (D2), Dash jetzt animiert, Klettern C3 `CROSS … frames=240` (§3.3); untergetaucht bis zum Durchbruch (N 201/201 bytegleich mit "ohne Gator", Elza gleich). Hinweis H2 (Belagerungsziel) gehört zur Ursachenkette von M1 |
| 3. Kanister aufnehmen "GENAUSO wie im original", im Maul beschießbar, Sterbeanimation mit geplatztem Kiefer wie RE2 | **erfüllt** | M1 der Abnahme 3 behoben: Kanister-Sitz 0 in jedem gezeichneten Bild (C3 270, Z 420, K 30 Bilder; Riegel sitz (a)–(d)), Kanister dreht bei Wende/Biss mit dem Kopf zwischen den Kiefern (C3-Bögen); Aufnahme ohne Lenkung in 12 Bildern, Anschwimm-Sperre (Z); Schuss → R1 mit Zeitlupe 38 Bilder/2516 ms, Kiefer-Splitter F71–F80, Leiche mit geplatztem Unterkiefer ab F82 (K); RE2-Kind-Matrix und Bildreihenfolge selbst nachgelesen. Hinweis H1 (Hals-Zielmodus) wie in Abnahme 3 kein Mangel |

## 9. Mängel (nummeriert, nachprüfbar)

1. **M1 (Punkt 1, sichtbar; ALT im Finisher, seit der Nachbesserung 4 auf dem natürlichen Türweg erreicht): nach einem
   tödlichen GUARD-Hochbiss liegt Leon im RE2-Finisher nicht im Maul.**
   - Wortlaut: "Ich will das du den Finisher … aus Resident Evil 2 extrahierst!" — im RE2-Finisher hängt Leon im Maul (so auch hier
     nach einem CHASE-Biss, A2 F416–F504). Nach dem Tod aus der Plattform-Belagerung bleibt das Maul leer, Leon schwebt getrennt neben
     bzw. über dem Kopf oder ist gar nicht im Bild.
   - Nachprüfung (Basis-Umgebung wie §3, eigene exe-Kopie, `timeout`):
     (a) echter Türweg: `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1 RE15_EXIT_AT=900#2090` + Framedump →
     Bisse F417/F522/F627 (state.log, Gier 169/13/115, Leon 67°/73°/64° daneben), `RE2 KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 2
     F628`, Bilder F628–F696 und F736–F868 ohne Leon im Maul, F700–F728 Leons Körper neben dem Kopf am Rohr
     (`B41abn4/beleg_E1_finisher.png`, `E1_fress_zoom2.png`, `E1_fress_p3_zoom.png`); dieselbe Folge mit
     `RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=6300,-16250,2048,1` (`mess/RN`);
     (b) Rampenmitte: `RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=4000,-15000,2048,1 RE15_GB_TEST=1 RE15_EXIT_AT=830#2090
     RE15_FRAMEDUMP=540-830/2:fd/m_` → Tod F587 (Gier 3405, Leon 128° daneben), `-> Cut 4 F588`, Leon F610–F700 neben/über dem Kopf
     (`B41abn4f/MN2_kopf_zoom.png`); (c) Vorher-Bau mit (b) (`B41abn4/probe/alt/re15_pc_alt.exe`, `EXIT_AT=850#2090`): Tod F608,
     Cut 7 ohne Gator im Bild (`B41abn4f/MA2_kopf_zoom.png`) → der Defekt ist ALT.
   - Code-Befunde (aus dem Code gelesen, Ursache nicht einzeln gemessen): GUARD-Hochbiss nur nach Abstand (`dist < 4200` / `< 3900`,
     enemy_ai_boss_gator.c:2234-2251) — der tödliche Biss kommt mit der Gier 33…141° neben Leon; das GUARD-Ziel liegt hinter der
     Ostwand-Klemmlinie, sobald Leon östlich x ≈ 5716 auf der Rampe steht (E1: (6500,−12600), Gier in die Wand); `pitch_vz` (−350 aus
     GUARD, Z. 2232) wird beim tödlichen Biss nicht genullt (Z. 421 nur `arc_vz`) und vom Bone-0-Haken (Z. 2751) durch P0–P3
     gezeichnet, der Opfer-Anker rechnet ohne sie; P0 setzt Leon 7096 entlang der Gator-Gier (Z. 2409-2423), in E1/MN2 hinter die
     Ostwand.
   - Verlangt: nach JEDEM tödlichen Biss — CHASE und GUARD (Rampe Ost wie E1, Rampenmitte, Plattformblock) — zeigt der Finisher
     Leon (und Elza, ROOM2091) über P1–P3 bis zum Game Over im Maul wie in A2, und der gehaltene Finisher-Schnitt zeigt Gator und Opfer.
     Die Form ist frei (Nachtrag 6: "Betreff Aligator darfst du alles machen was du willst"), z. B. Ausrichten auf Leon vor dem
     tödlichen Biss, `pitch_vz` beim Eintritt in FRESSEN abbauen, erreichbares Belagerungsziel — als PORT-WAHL benennen; RE2-Teile mit
     @0x. Riegel: der Maulraum-Messung von `unit_gator_fress` (heute nur CHASE-Lage, `Maulraum … 776/776 = 100.0%`) eine GUARD-Lage
     hinzufügen (Leon oben auf der Rampe bei (6300,−16250) und (4000,−15000), Tod durch den Hochbiss, gezeichneter Kiefer MIT
     pitch/jaw-Haken gegen Leons Körper in jedem P1–P3-Bild); integration "belagerung" bis nach dem Tod prüfen (Leon im Maul je
     Bild bzw. Abstand Leon ↔ gezeichneter Kiefer); Bildbeleg E1-Türweg vorher/nachher F628–F870.

Hinweise an den Orchestrator (keine Mängel dieser Abnahme):
- **H1** (unverändert seit Abnahme 3): die RE2-Halsführung (Root @0x80100330-74 → FUN_80017d98 auf Leon, im Zielmodus
  FUN_800177c0) fehlt; der Bau-Agent führt sie OFFEN mit Beleg, die Umsetzung braucht einen Part-5-Gier-Kanal außerhalb der
  Spur-B-Dateien — Entscheid steht aus.
- **H2:** seit der Nachbesserung 4 wartet der Gator in der Belagerung am E1-Ankunftsort dauerhaft an der Ostwand-Klemmlinie mit Blick
  auf sein unerreichbares GUARD-Ziel (vorher drehte ihn der Snap alle 32 Bilder weg); Teil der Ursachenkette von M1, nach Nachtrag 6
  frei gestaltbar.
- **H3 (Spur F/G):** auf dem 20A0-Weg taucht der Gator ohne Testhebel nicht auf (EN, §6); der Kampf beginnt erst mit der Szene der
  Spur F — M1 betrifft genau die Lage danach, wenn Leon auf der Rampe steht und schießt.

## 10. Gesamturteil

`bestanden = false`. Die beiden Mängel der Abnahme 3 sind am gebauten Spiel behoben und gegen den Vorher-Bau gegengeprüft: der
getragene Kanister sitzt in jedem gezeichneten Bild im Maul (echte Zeichner-Messzeile 0 in 270/420/30 Bildern, Riegel sitz 0 in
allen vier Lagen; die RE2-Reihenfolge Zeichnen vor Objekt-Schleife selbst nachgelesen), und die Gier springt in Verfolgung und
Belagerung nicht mehr (alle Läufe und drei Prüfer-Sonden: 0 Sprünge außer den RE2-Knicken +1024). Alle selbst geprüften RE2-Stellen
stimmen (§2), Diff ohne Rate-Wörter, Vertrag eingehalten, Unit 112/112, Integration 7/7. Punkt 2 und Punkt 3 sind erfüllt.
Offen gegen den Wortlaut: **M1** — der RE2-Finisher zeigt nach einem tödlichen Belagerungsbiss Leon nicht im Maul; der Defekt ist
ALT (im Vorher-Bau ebenso), wird aber seit dieser Nachbesserung auf dem natürlichen Türweg 20A0 → Ostrampe erreicht (vorher überlebte
Leon dort 1500 Bilder, jetzt stirbt er bei F627 mit leerem Maul) — Punkt 1 teilweise.

Neue Prüfer-Dateien dieser Fortsetzung (nicht im Baum): `B41abn4f/bisse.py` (Gier gegen Peilung je HP-Abzug), Bildbögen
`B41abn4f/MA2_finisher.png`, `MA2_kopf_zoom.png`, `MN2_finisher.png`, `MN2_kopf_zoom.png`, Läufe `B41abn4/mess/RA|RN|BA|BN|MA|MN|MA2|
MN2` (Rezepte in je `lauf.txt`), `B41abn4f/fr_47ef.c`/`fr_head.c` (FRESSEN-Fall beider Stände, bitgleich).
