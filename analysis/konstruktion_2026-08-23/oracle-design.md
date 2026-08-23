# Parity-Oracle — Entwurf (Phase 0B, A6-I2)

MECHANISMUS — Bestandsaufnahme + belegte Adress-Grundlage des Oracles

## 1) MECHANISMUS (was der Oracle liest — jede Zahl mit Beleg)

### 1.1 Ist-Zustand: ALLE Bausteine existieren bereits einzeln — nur die Spine fehlt

| Baustein | Datei | Status |
|---|---|---|
| PC-Per-Frame-Emitter (Text) | `re15_port/platform/pc/main.c:4957-4996` (`RE15_STATE_LOG`) | LÄUFT. Format `F%u pad=%04x PL(x,z,rot,hp) pst/ps1/ps2/mo/ac/fx/mg/bx/sl/cam` + je Gegner `[slot t st ss1 ss2 ss3 g mo af stun d @(x,z,r)]` |
| PC-Determinismus | `input_pc.c:44-119` (`RE15_INPUT_SCRIPT`, 30fps-Ticks) + RNG default-deterministisch (Commit ac7dae64, Seed 0x2545f491, `re15_damage.c:66-78`) | LÄUFT; A/B byte-identisch verifiziert (Memory reai-v2-rng-determinism) |
| PSX-Per-Frame-Trace, frame-exakt | `.claude/skills/re15-pcsx-watchpoint/scripts/parity_trace.lua` (+ `parity_calib.lua`) | **HAT FUNKTIONIERT**: `shots/pcsx_1140_stable.log` = 1725 Frames ROOM1140, `# >>> ROOM1140 GEFUNDEN mit 16 Links-Schritten`, `# f19501 ROOM LIVE (act=5)`, Roster `16,10,10,11,11` == Port. Input rein vsync-getaktet (kein Wall-Clock) |
| Differ (Text-Format) | `re15_port/tools/parity_diff.py` | EXISTIERT: Zeitbasis ab „ROOM LIVE", Yaw-Maske `&0xFFF` (Original schreibt ungemaskt, gemessen −5952), Slot-Versatz Port N ⇔ PSX N−1, first-divergence je Größe |
| End-State-Vergleicher | `re15_port/tools/parity_run.py` | **STALE** — benutzt `RE15_START_ROOM`, das seit 79e8504f ENTFERNT ist; nur End-Frame, nicht per-Frame |
| Savestate→Werte | `.claude/skills/re15-savestate-ghidra/scripts/` (`re15_ss.py` Ram-Klasse, `re15_enemy_state.py`, `re15_savestate_inspect.py`, `re15_diff.py`, `re15_flag_sweep.py`, `re15_ss_patch.py`, `re15_provoke.py`) + `re15-parity-verify/scripts/ppos.py`, `orig_timeline.py` | LÄUFT (Einzel-Frame-Ground-Truth) |
| Bildbasierter Frame-Matcher | `.claude/skills/re15-parity-verify/parity_net/` (parity_net.py + .pt, 89,3 % top-1) | LÄUFT; Score = Triage, kein Beleg |
| Engine-only-Raum-Harness | Skill re15-room-probe, Vorlage `re15_port/tests/unit/test_room1140_spawn.c`, 278 Tests in `tests/unit/` | LÄUFT (deterministisch, ctest) |

Roadmap-Anker: `PORTING_ROADMAP.md` Phase 0B = A6·I2‑1…I2‑5 (Schema → PC-Emitter → PSX-Exporter → Differ → parity_net-Alignment); `AUDIT_GESAMT_2026-08-23.md` §Restaufwand Punkt 2/5: „Parity-Oracle M-L, höchster Hebel".

### 1.2 Die kanonische Adress-Map (PSX-Seite; alles bereits repo-belegt, hier konsolidiert)

**Spieler** (Basis `0x800ACA54`; `parity_trace.lua:32-38`, `ppos.py`, `parity_run.py:39-46`):
- Pos = Entity-MATRIX-Translation `0x800ACA88` (x), `0x800ACA8C` (y), `0x800ACA90` (z) = Basis+0x34/38/3c. ⚠️ `0x800ACA74` ist `m[0][0]=cos(yaw)` — NIE als Winkel lesen (belegte Phantom-Divergenz „−45 vs −96", parity_trace.lua:33-35).
- Yaw `0x800ACABE` (+0x6a, 4096=360°), **ungemaskt** geschrieben → Vergleich immer `&0xFFF` (parity_diff.py:9-14, gemessen −5952&0xFFF=2240).
- HP `0x800ACAEE` (+0x9a; DAT_800acaee, Schreiber FUN_80012d60).
- State `DAT_800ACA58` (1=idle 2=hit 3=death 5/6=struggle/collapse 7=you-died; FUN_80031c44), Sub-Bytes aca59/5a.
- Clip +0x94 / Clip-Bild +0x95 (Entity-Tabelle, pcsx-watchpoint-Skill §Entity, `@0x800420f8-118` Stride-Beleg).

**Gegner-Array** `0x800ACC2C`, Stride `0x1F4` (`s0 = 0x800acc2c + slot*0x1F4` @0x800420f8-118): +0x0 Wort (belegt = ohne Bit 0x8000; parity_trace prüft bit0), +0x4 Main-State, +0x5/+0x6/+0x7 Sub-States, +0x8 Typ, +0x9 grid/skip (+0x9&0x20 = Skip-Tick), +0x34/+0x38/+0x3c Pos, +0x6a Rot, +0x93 Ein-Treffer-Latch (Memory reai-v2-hit-latch-93), +0x94/+0x95 Clip/Bild, +0x9a HP, +0x1d0 SquareRoot0-Distanz-**Cache** (nur Zombie-Familie schreibt ihn; Maggot/Dog rechnen frisch → `d=0` dort byte-true erwartet, Memory reai-v2-parity-oracle), +0x1de Behavior-Byte.
- Active-Count `0x800ACA4E` (parity_trace.lua:38; Gate `sltiu 0x5` in der Beh-Tabellen-Wahl @0x8011f-Bereich).

**Raum/Global**: Raum aktuell `0x800B0FE2`, vorheriger `0x800B0FE6` (JUMP-Kette `sh v0,0x800b0fe2` @0x8001D660 / @0x8001D644 — Disasm im pcsx-watchpoint-Skill; ⚠️ 0x800B0FE6 ist der VORHERIGE Raum, nicht die Stage). Kamera-Cut `DAT_800B532E` (Ghidra-belegt: indiziert Cut-Tabellen @0x8008f82c; re15_savestate_inspect). Fade `DAT_800B5568` (FUN_80021a0c). Pause-Wort `DAT_800ACA40`.

**Flag-Bänke** — Zeiger-Tabelle `@0x80074664` (Set-Opcode LAB_8003fdd0, `lw v1,PTR_DAT_80074664[zone]` @0x8003fe04; vollständige Instruktions-Zitate in `re15_port/engine/src/game_state.c:84-110`):
`[0]0x800ACA38 [1]0x800ACA3C [2]0x800ACA40 [3]0x800B0FF8 [4]0x800B1018 [5]0x800B1028 [6]0x800B1030 [7]0x800B1038 [8]0x800B1058 [9]0x800B1078(Item-taken) [10]0x800B1098`; Bit = `0x80000000 >> (idx&0x1f)`, Wort = `idx>>5` (== Port `re15_game_flag_get`, game_state.c:76-82, FUN_8004ef90/FUN_8004efe4).

**Pad-Wörter** (FUN_80030444, komplett disassembliert im pcsx-watchpoint-Skill): `0x800AC758` roh-held, `0x800AC75C` rohe Flanke (low=0x800AC762), `0x800AC760` Kopie, `0x800AC768` virtuelles held, `0x800AC76C` virtuelle Flanke. SELECT steht konstruktionsbedingt NIE im virtuellen Wort.

**RNG**: Gameplay-RNG-State `@0x800AC774` ist ein **DEAD STORE** (exakt 3 Xrefs, toter Read @0x8001af28; FUN_8001af20 hasht das Caller-a0-Register) → RNG-**Werte** sind zwischen Port (dokumentierte xorshift32-Substitution) und PSX **prinzipiell nicht wert-vergleichbar**; beide Seiten sind aber je für sich deterministisch. Schema-Konsequenz: RNG-Felder side-specific, vom Diff ausgeschlossen; Draw-Count nur PC-vs-PC.

**Debug-Menü** (für die PSX-Raumanfahrt): Offen-Kennung `0x800BBE5C` (Toggle `xori v0,v0,1` @0x80014480, `sb` @0x80014488), Zeile `0x800BBE5D`, Stage `0x800BBE5E`, Raumindex `(0x800BBE5F+stage)`; Bestätigen liest die ROHE Flanke `0x800AC762` (`lhu` @0x80014a30, `andi 0x80` @0x80014a38; Kreuz `&0x40` in FUN_8001443c). Raumnummer = `(stage+1)<<8|idx`, Port-ID = ×16 (BRIEFING idx 0x14 → 0x114 → ROOM1140). Der JUMP baut KEINEN Türdatensatz (`sb 1→0x800b5359` @0x80014a44/48, `sw zero→0x800ac9a8` @0x80014a4c/50).

### 1.3 Die zwei PSX-Harness-Wege, ehrlich bewertet

**Weg A — PCSX-Redux + Lua (parity_trace.lua): DER TIMELINE-WEG. Empfohlen.**
- Frame-exakt by construction: Input UND Dump hängen am `GPU::Vsync`-Listener; „`R x24` = wirklich 24 Frames" ist im Log belegt (parity_trace.lua Kopfkommentar + `shots/pcsx_1140_stable.log` Segmente W60/R24/U240).
- Per-Frame-RAM-Dump läuft (Spieler + 8 Gegner-Slots pro vsync, Format kompatibel zu `parity_diff.py`).
- Raum-Anfahrt gelöst über Roster-Kalibrierung (`parity_calib.lua`: Select→Down→N×Left→Square, Abbruchkriterium = Gegner-Roster-Signatur; EIN Emulator-Boot für alle Versuche). Der direkte Cursor-Schreibweg scheiterte (geschriebenes `0x800bbe5f+stage` überlebt nicht bis zum `lbu` @0x8001d654 — offener Punkt im Skill), READ-Closed-Loop + Verifikation über `0x800B0FE2` nach dem Load ist der robuste Generalisierungsweg.
- Kosten: `-interpreter -debugger` ~5× langsam; Boot+Anfahrt ~19500 vsyncs (~5-8 min/Lauf). Amortisierbar: mehrere Räume pro Boot (Calib-Muster beweist Mehrfach-JUMPs in einem Lauf).
- Bekannte Fallen sind ALLE dokumentiert und im Skript bereits eingebaut (LuaJIT-bit-Ops, `getMemPtr()` frisch pro Frame, pcall-Fehlerreport, Listener am Leben halten, `return true` in Haltepunkten, Prozessname `pcsx-redux.main`, ein Lauf zur Zeit).
- **Zwei Messartefakte im vorhandenen Log, die der Differ behandeln muss:** (a) 60Hz-vsync vs 30Hz-Game-Tick — Zeilen F19501/F19502 sind state-identisch = derselbe Game-Tick zweimal; (b) ROOM-LIVE-Anker: bei +0 steht der Spieler noch auf dem ALTEN Wert (−7600,−17600), die Debug-Platzierung (−1300,−13950, rot 2048) erscheint erst 2 vsyncs später.

**Weg B — DuckStation-Savestate-Ketten (orig_timeline.py): NUR Ground-Truth-Stichproben, KEINE Timeline.**
- fps-Falle (Skill §4, gemessen): vgamepad-Wall-Clock-Holds mappen auf variable Game-Frames (Hold „60 Frames" → ~3,4 Frames Wirkung; `R0.3` ergab in 5 Läufen +96/+768/+960°). JUMP-nav-quickload ist nie frame-genau. Direct-Load (`-statefile`) ist deterministisch, aber nur für Einzel-Hold-Primitive.
- Savestates liefern EINEN Frame, nicht jede Frame — für den Per-Frame-Differ strukturell ungeeignet.
- Bleibt wertvoll als: (1) Schiedsrichter-Einzelframes (`re15_enemy_state.py`-Feldkarte ist die verifizierte Referenz), (2) golden Clean-Load-Save je Raum (Roadmap A6·I3‑4), (3) Provoke-Pipeline (`re15_ss_patch.py`/`re15_provoke.py`) für Zustände ohne Navigationsweg.

**Spawn-Parität by construction (NEU seit v0.3.17):** `RE15_DEBUG_JUMP` landet jetzt am ORIGINAL-Debug-Punkt (DEBUG.BIN-Tabellensatz X/Z/Band, Y=−Band·0x708, Cut 0 — FUN_8001d600-Debug-Zweig; HANDOVER_2026-08-23 §0a Punkt 10). PSX-Seite (Debug-Menü-JUMP) und PC-Seite starten damit am IDENTISCHEN Punkt — die frühere Spawn-Delta-Sonderbehandlung (`parity_run.py --psx-start`) entfällt. ⚠️ Grenze bleibt (Memory reai-v2-playthrough-not-jumpin): Debug-Sprung ≠ Türweg; der Oracle in Stufe 2 beweist Raum-Parität unter Sprung-Bedingungen, Türketten-Bugs braucht weiterhin die Raumketten-/E2E-Schiene.

---

## 2) PORT-STAND (Datei:Zeile)

- `re15_port/platform/pc/main.c:4957-4996` — der einzige Per-Frame-Emitter (`RE15_STATE_LOG`, Text), direkt nach `re15_game_step(&gctx)`; Felder: Player x/z/rot/hp, pst/ps1/ps2, motion, aim-clip, fx-count, Magazin/Box-Qty, 4 Render-Slots, `g_scd.cam_id`; je Gegner slot/t/st/ss1/ss2/ss3/g/mo/af/stun/d/x/z/r. Fehlt: Raum-ID, Fade, Flag-Bänke, Pause-Wort, y-Koordinate, +0x93-Latch, RNG-Draw-Count, Schema-/Versions-Header, JSON.
- `re15_port/platform/pc/src/input_pc.c:44-119` — deterministischer Script-Input (identische Token-Sprache wie DuckStation `--path` und `RE15_PT_SCRIPT`), `RE15_FPS`, `RE15_INPUT_SCRIPT_START` (Default 90).
- `re15_port/engine/src/re15_damage.c:66-80` — geteilte RNG (`re15_engine_rand8`), seedbar, default-deterministisch; kein Draw-Counter vorhanden.
- `re15_port/engine/src/game_state.c:64-110` + `include/re15_scd.h:376-385` — Flag-Store `g_game.flags[16][8]` MSB-first == FUN_8004ef90/efe4; Pause-Wort `g_re15_pauseflags`.
- `re15_port/include/re15_room.h:22` — `g_current_room_id`.
- `re15_port/tools/parity_diff.py` — Differ (Text): live-Anker, Yaw-Maske, Slot-Versatz, first-divergence, Roster-Check. `tools/parity_run.py` — stale (RE15_START_ROOM).
- `.claude/skills/re15-pcsx-watchpoint/scripts/parity_trace.lua` + `parity_calib.lua` — der funktionierende PSX-Timeline-Tracer (Beleg-Logs `shots/pcsx_1140_run.log`, `shots/pcsx_1140_stable.log`, `shots/pcsx_parity_stage2.log`).
- Savestate-Reader-Familie: `.claude/skills/re15-savestate-ghidra/scripts/*` (§1.1-Tabelle), `re15-parity-verify/scripts/ppos.py`, `orig_timeline.py`.
- Test-Harness: `re15_port/tests/unit/` (278 Dateien, GLOB-gebaut), Probe-Muster laut Skill re15-room-probe (`test_room1140_spawn.c`; `scd_room_reenter`-Minimal-Sequenz), ctest 224/224 grün (HANDOVER).
- `parity_net/parity_net.py` + `parity_net.pt` — Bild-Alignment (DTW `--align`), 89,3 % top-1; nur Triage.

---

## 3) IMPLEMENTIERUNGS-PLAN

### Stufe 0 — Schema festschreiben (A6·I2‑1; Aufwand S, reine Doku + Validator)

**Datei `re15_port/tools/oracle/SCHEMA.md`** (+ `schema_check.py`): JSON-Lines, versioniert (`v:1`).

Header-Record (1. Zeile): `{v, side:"pc"|"psx"|"psx-sav", room, stage, commit/build, input_script, fps, ai_flavor, seed, anchor:"debug_jump"|"door"|"savestate"}`.

Frame-Record (Feld ⇄ PSX-Adresse ⇄ Port-Quelle ⇄ Toleranz):

| Feld | PSX | Port | Diff-Regel |
|---|---|---|---|
| `f` | vsync-Zähler rel. ROOM-LIVE, **auf Game-Ticks kollabiert** | `g_engine.frame_count` rel. Anker | Schlüssel |
| `pad` | `0x800AC758` (roh held) | `g_engine.pad_current` | exakt |
| `pl.x/y/z` | 0x800ACA88/8C/90 | `g_actors[0].x/y/z` | exakt (Default 0; CLI-Toleranz nur explizit) |
| `pl.rot` | 0x800ACABE | `rot_y` | `&0xFFF` beidseitig, dann exakt |
| `pl.hp` | 0x800ACAEE | `hp` | exakt |
| `pl.st/s1/s2` | 0x800ACA58/59/5a | `state/sub_state_1/2` | exakt, MIT Ausnahme-Map: Port-Victim-Modell `g_player_victim` ⇄ PSX st 5/6 (dokumentierte Substitution, Memory reai-v2-parity-oracle) |
| `pl.mo/af` | +0x94/+0x95 | `motion/anim_frame` | exakt |
| `en[i]` slot,type,st,ss1,ss2,ss3,grid,mo,af,hp,rot,x,z,d,latch93 | 0x800ACC2C+i·0x1F4 Offsets aus §1.2 | `g_actors[i+1]` (Slot-Versatz!) | Typ/States/grid/hp exakt; `d` NUR für Cache-Typen (Zombie-Familie), sonst ignore; Roster-Anzahl exakt |
| `cut` | DAT_800B532E | `g_scd.cam_id` | exakt |
| `room` | 0x800B0FE2 (×16 = Port-ID) | `g_current_room_id` | exakt |
| `fade` | DAT_800B5568 | Port-Fade-Global (fade_common) | exakt |
| `pause` | DAT_800ACA40 | `g_re15_pauseflags` | exakt |
| `flags[0..10]` | Bänke via @0x80074664 (je 8 u32) | `g_game.flags[zone][0..7]` | exakt (Bit-Layout identisch belegt) |
| `act` | 0x800ACA4E | Zählung aktiver g_actors | exakt |
| `rng_draws` | — (nicht existent, Dead Store @0x800AC774) | neuer Zähler | NUR pc-vs-pc |

Feld-Begründung aus den realen Divergenz-Klassen: Turn-48-vs-96 → `pl.rot`; 81 AI-Divergenzen → `en[].st/ss1/ss2/mo`; Grab/HP-Drain → `pl.hp`, `en[].latch93`; Kamera-Zonen-Statemachine/RVD → `cut`; SCD-Event-/Latch-Bugs (1090) → `flags`, `pause`; Pickup/Heal → Inventar-Kurzblock (equipped id, mag, Slots-Hash) wie heute `mg/bx`.

### Stufe 1 — PC-vs-PC-Golden-Regression (A6·I2‑2 + Regressionsnetz; Aufwand M, ~1 Session)

1. **Emitter engine-seitig ziehen:** neu `re15_port/engine/src/state_trace.c` + `re15_port/include/re15_state_trace.h`. API: `re15_state_trace_open(path)`, `re15_state_trace_frame(void)` (liest ausschließlich vorhandene Globals aus §2), env `RE15_STATE_TRACE=<pfad.jsonl>`. Aufruf-Site 1: `platform/pc/main.c` unmittelbar nach `re15_game_step(&gctx)` (heutiger `RE15_STATE_LOG`-Block bleibt als Legacy unangetastet — parity_diff.py-Kompatibilität). Aufruf-Site 2: aus Probe-ctests (engine-only, kein Fenster). RNG-Draw-Counter: 1 `static uint32_t` Inkrement in `re15_engine_rand8()` (re15_damage.c:72) + Getter — kein Verhaltens-Code.
2. **Anker-Event:** eine Zeile `{ev:"room_live", room, f}` beim Abschluss von `re15_room_apply_pending`/Debug-JUMP-Landung — der Differ alignt darauf (Pendant zu `# ROOM LIVE` im Lua-Trace).
3. **Runner `re15_port/tools/oracle/golden_run.py`:** kill-lock (`taskkill //F //IM re15_pc.exe`), Start `re15_pc.exe` mit `RE15_DEBUG_JUMP=<room>@<frame>`, `RE15_INPUT_SCRIPT`, `RE15_STATE_TRACE`, `RE15_NOAUDIO=1`; ⛔ NICHT `RE15_AUTOSHOT`/`RE15_SOFTWARE_RENDER` als Beweis (Memory reai-v2-visual-verify-gdigrab) — State-Trace ist renderunabhängig, das ist ok.
4. **Determinismus-Gate:** derselbe Lauf 2×, JSONL byte-identisch (das ist heute schon gegeben — ac7dae64; das Gate hält es).
5. **Goldens:** `re15_port/tests/golden/room1140_R015U25.jsonl.zst` (Pfad `R0.15,U2.5` = der verifizierte Grab-Death-Pfad) + je 1 Kurz-Golden pro Ketten-Raum (1120/1130/1140/1150/1170/1240). Ctest `integration_state_golden` (neues `tests/unit/probe_state_golden.c` nach room-probe-Muster ODER Python-ctest via `add_test`): replay → `state_diff.py` gegen Golden, Toleranz 0.
6. **Differ v2 `re15_port/tools/oracle/state_diff.py`:** parity_diff.py-Logik übernehmen (live-Anker, Yaw-Maske, Slot-Versatz), auf Schema-JSONL umstellen, Toleranz-Tabelle aus SCHEMA.md, Report = erstes Divergenz-Tripel (frame, slot, feld, a-vs-b) + JSON-Summary. Selbsttest: synthetisches Paar mit injizierter Turn-48-Störung → muss `rot @ erster Pivot-Frame` melden (Roadmap-Abnahme I2‑4).

✔ Abnahme Stufe 1: 2 Läufe byte-identisch; Golden-ctest grün; absichtliche Konstanten-Störung (lokaler Build) → ctest rot mit benanntem Frame+Feld.

### Stufe 2 — PC-vs-PSX (A6·I2‑3/4/5; Aufwand L, 2-3 Sessions)

1. **PSX-Game-Tick-Kollaps klären (kleiner RE-Schritt ZUERST):** im Lua-Trace pro vsync zusätzlich einen In-Game-Tick-Zähler mitschreiben, damit 60Hz-vsync→30Hz-Tick exakt (nicht heuristisch über „Zeile identisch") kollabiert wird. Kandidat: der Frame-Zähler der Main-Loop @0x8001cce0 — Adresse per `re15_disasm.py dis 0x8001cce0` bestimmen. Fallback bis dahin: Duplikat-Kollaps + Anker auf den ersten Frame, in dem `pl.x/z == DEBUG.BIN-Punkt` (behebt zugleich das gemessene +2-vsync-Platzierungs-Artefakt F19501→F19503).
2. **`parity_trace.lua` produktisieren** (Kopie nach `re15_port/tools/oracle/pcsx_state_trace.lua`, Skill-Original unangetastet): (a) Zeilenformat → Schema-Felder erweitern (cut DAT_800B532E, room 0x800B0FE2, fade DAT_800B5568, flags-Bänke @0x80074664-Liste, pause 0x800ACA40 — alles reine `u8/u16/u32`-Reads, Adressen §1.2); (b) Raum-Anfahrt generalisieren: Closed-Loop-READ auf `0x800BBE5E`/`(0x800BBE5F+stage)` mit echten Pad-Taps + Erfolgskontrolle `u16(0x800B0FE2)==(stage+1)<<8|idx` nach Square; Roster-Signatur (aus RDT-`Sce_em_set`-Parse je Raum, offline generierbar) bleibt als zweite, objektive Abnahme; (c) mehrere Räume pro Emulator-Boot (Calib-Muster) gegen die ~130s-Interpreter-Bootzeit.
3. **Konverter `pcsx_trace_to_schema.py`:** Lua-Text → JSONL (oder Lua schreibt direkt JSONL — Text+Konverter ist robuster gegen Lua-Fehler).
4. **PSX-Determinismus-Beweis (Pflicht, nicht annehmen):** denselben Lauf (gleicher Raum, gleicher `RE15_PT_SCRIPT`) 2× fahren, Traces ab ROOM LIVE diffen. Erwartung: identisch (RNG hat null Entropie, Input vsync-exakt); jede Abweichung ist ein Harness-Befund, KEIN Spiel-Befund (Lehre „Sonde lügt").
5. **Savestate-Exporter `psx_export.py`** (A6·I2‑3): wrappt `re15_ss.Ram` + die `re15_enemy_state.py`-Feldkarte → EIN Schema-Frame pro .sav; dient als unabhängige Gegenprobe der Lua-Reads (gleicher Raum: Lua-Frame ⇄ Savestate-Frame müssen matchen) und als Anschluss für den Golden-Save-Korpus (A6·I3‑4) und `re15_provoke.py`-Zustände.
6. **Lauf-Matrix:** Ketten-Räume × verifizierte Pfade (ROOM1140 `W90,R24,U240` liegt als Referenz-Log schon vor); Artefakte nach `stage_saves/oracle/` bzw. `shots/oracle/`.
7. **parity_net (A6·I2‑5) als OPTIONALES Bild-Alignment** hinter dem State-Differ, nur wenn zusätzlich Screens verglichen werden — im Kern-Statepfad ist es unnötig, weil der PCSX-Weg die fps-Falle gar nicht hat (vsync-indiziert).

✔ Abnahme Stufe 2 (== Roadmap I2‑4): ROOM1140, gleicher Pfad, 0 Divergenzen über die Grab-Death-Sequenz; injizierter Turn-48-Bug im Port wird am ersten Pivot-Frame als `pl.rot` gemeldet.

### Stufe 3 — Skalierung (A6·I3‑5; Aufwand M, nach Stufe 2)

Per-Raum-Parity-Job: für jeden Raum mit (Golden-Save ∨ PCSX-Trace) + Golden-Pfad → Port deterministisch fahren, `state_diff.py`, Ergebnis-Spalte in die Coverage/Scorecard; Engine-only-Goldens (Probe-Harness, ohne Fenster) für alle 40 Leon-Räume als billige Vorstufe. Später: PSX-ISO-Build TTY-Schema-Frames → Cross-Target (A6·I6‑3), gleiche Differ-Infrastruktur.

---

## 4) OFFEN (ehrlich)

1. **In-Game-Tick-Zähler des Originals nicht belegt** (für den exakten 60→30Hz-Kollaps). Nächster RE-Schritt: `re15_disasm.py dis 0x8001cce0` (Main-Loop) nach dem inkrementierten Frame-Wort absuchen; dynamische Gegenprobe per Watchpoint (Skill re15-pcsx-watchpoint). Bis dahin trägt der Differ den dokumentierten Duplikat-Kollaps als Heuristik-Flag.
2. **PSX-Trace-Determinismus über volle Läufe ist NICHT gemessen** (nur konstruktiv plausibel: vsync-Input + Null-Entropie-RNG). Der A/B-Doppellauf in Stufe 2 Schritt 4 ist Pflicht-Abnahme, bevor irgendein Diff-Ergebnis als Port-Bug gilt.
3. **Debug-JUMP-Cursor-WRITE in PCSX-Redux ungelöst** (geschriebener Raumindex überlebt nicht bis `lbu` @0x8001d654; Skill-Notiz). Der Plan umgeht das per READ-Closed-Loop + `0x800B0FE2`-Verifikation; der Exec-Haltepunkt-auf-0x8001d654-Ansatz bleibt als dokumentierte Alternative.
4. **Ende des Referenz-Logs zeigt `act=0`** (`pcsx_1140_stable.log` F21061: Roster verschwunden, Spieler @(−2582,−13950)) — Ursache unerklärt (Raum verlassen? De-Spawn? Trace-Artefakt?). Vor Produktivnahme mit einem frischen Lauf + `room`-Feld (0x800B0FE2, das der Trace bisher nicht loggt) klären.
5. **Fade-Global auf Port-Seite:** welcher konkrete Port-State `DAT_800B5568` spiegelt (fade_common.c vs. Render-Fade-Setter in main.c) ist für den Emitter noch zu bestimmen (reine Code-Lektüre, kein RE).
6. **Victim-Substitution (pst 5/6 vs `g_player_victim`)** ist eine dokumentierte, bewusste Port-Abweichung — der Differ braucht die Ausnahme-Map, sonst produziert jeder Grab einen False-Positive. Langfristig steht die exakte pl->state-FSM als eigener RE-Punkt im Backlog (Memory reai-v2-parity-oracle).
7. **Geltungsbereich:** Debug-JUMP-basierte Parität kann Türweg-/Raumketten-Fehler prinzipiell nicht sehen (Memory reai-v2-playthrough-not-jumpin, Regel „Raumkette statt Debug-Sprung"). Der Oracle ersetzt die E2E-/Türketten-Verifikation nicht — er macht die Raum-Ebene automatisch; für Türwege ist die spätere Erweiterung „Anker=door" im Schema vorgesehen (PSX-Seite bräuchte dann einen Lua-gefahrenen Türdurchgang statt JUMP).
8. **`tools/parity_run.py` ist stale** (RE15_START_ROOM entfernt) — im Zuge von Stufe 1 auf `RE15_DEBUG_JUMP` heben oder durch `golden_run.py` ersetzen und als ersetzt markieren, damit kein Agent das tote Werkzeug benutzt.
