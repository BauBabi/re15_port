# RE1.5 Port — Session-Handover (Stand 2026-06-29)

Kanonisches „lies-mich-zuerst" für die nächste Session. Aktueller Fokus: **Gegner-/Zombie-AI**.
Ergänzt die Auto-Memory (v.a. `reai-v2-foundation-combat` = die laufende AI-RE) +
`reai-v2-duckstation-dynamic-re`, `reai-v2-ghidra-pipeline`, `reai-v2-byte-true-audit`.

## TL;DR — Wo stehe ich (2026-06-29)

- **Git:** HEAD = `c278b08e` (master, sauber). Diese Session = **6 AI-Commits**
  (`bb942f38` Phase 2 → `8580df9c` 3 → `06c0c055` 4 → `9ae78de1` 5 → `9c86ae3c` 6 → `c278b08e` 7).
- **Build/Test:** `export PATH="/c/msys64/mingw64/bin:$PATH"; cmake --build re15_port/build;
  ctest --test-dir re15_port/build --timeout 30` → **29/29 grün** (mingw64 GCC + Ninja).
  Neuer Test: `unit_enemy_ai` (17/17 Sub-Tests); `unit_damage` (17/17, inkl. Lunge).
- **Die byte-true RE der STAGE1-Zombie-AI ist KOMPLETT** (Decision-FSM + Live-Decision-Graph +
  Lunge-Attack-Sequenz + Hitbox + Damage; alles portiert + getestet, additiv, **NICHT** in
  `game_step` verdrahtet = kein 1170-Risiko). Full-Chain bewiesen: spawn→tick→lunge→Spieler-HP fällt.
- **Nächster Schritt = PHASE 8 (Integration, NICHT mehr reines byte-true):** siehe unten.
- **Neues Tooling:** Skill **`re15-psx-disasm`** (MIPS-Direkt-Disasm der PSX.EXE/Overlays, „Weg C").
- **Disziplin:** jede Konstante zitiert eine Disasm-Adresse/Datei-Offset; additiv + getestet;
  kein `game_step`-Eingriff bis Phase 8 (vorsichtig, savestate-verifiziert).

## Enemy-AI byte-true RE (Phase 2–7, diese Session) — Architektur-Karte

Tick `FUN_8011d6d4` → Main-State `+0x4` (`PTR_FUN_801217a0[0..4]`: 0 init/1 active/2 hurt/3 death/4 idle)
→ active `FUN_8011d9f4` → sub `+0x9 & 0xf` (`PTR_FUN_801217b4[0..15]`; **live = sub 0** = `FUN_8011da48`)
→ `+0x5` logic `SUB[1+(+0x5)]` + anim `SUB[4+(+0x5)]`.

**ZWEI parallele Decision-Systeme** (savestate-disambiguiert, Tabellen NICHT laufzeit-gepatcht):
- **(A) LIVE** = die EXE-Generic-Leaves (`FUN_8004f100` assess `+0x5`=0 / `FUN_8004f3a4` search 1 /
  `FUN_8004f5e8` turn 2; `+0x5`≥3 = Movement/Anim-Exec). **Kein Leaf setzt `+0x5`=7** — die Lunge
  emergiert aus Anim + Action-Driver `FUN_80019e20` (action `0x16`/`0x17`/`0x18` → Hitbox `FUN_80017fa4`).
- **(B) PARALLEL** (byte-true, aber NICHT live) = die Overlay-Handler `FUN_80101b64/c7c/de4/2058`
  via `FUN_8010168c`-Familie → vtables `f840/…` auf `+0x5`.

**Portierter Code** (alles additiv, getestet):
- `re15_port/engine/src/enemy_ai_common.c` + `include/re15_enemy_ai.h`: `re15_enemy_ai_init`
  (`FUN_8011d84c`), `re15_enemy_ai_tick` (`FUN_8011d6d4`), `re15_enemy_ai_active` (`FUN_8011d9f4`,
  ruft live `re15_ai_exe_dispatch`), System-A-Leaves `re15_ai_exe_assess/search/turn`, System-B-Brain
  `re15_ai_decide_search_timer/approach/search/engage` + `re15_ai_facing_aligned`, `re15_ai_set_state_word`,
  und der Integrations-Entry **`re15_enemy_ai_step(slot)`** (FSM-Tick + Lunge-Slice — den ruft game_step).
- `re15_port/engine/src/re15_damage.c`: Damage-Resolver + Hitbox (schon vorher) + **Lunge-Driver**
  `re15_enemy_lunge_begin/tick` (`LAB_80017eb0`/`LAB_80017f50`: 0x20-Frame-Window, feuert `re15_enemy_attack`
  jeden Frame, beißt 1×). Neue Actor-Felder: `ai_dist/ai_timer/ai_flags/ai_contact/ai_arc/…/ai_target_x/z/
  lunge_frames` (`re15_actor.h`, alle offset-zitiert).

## PHASE 8 — wo es weiter geht (Integration + dynamische Verifikation)

Reihenfolge + Risiko:
1. **Skeleton-Mapping (faithful-line, kein reines byte-true):** `atk_pt_*` pro Frame aus der
   Attack-Bone-Weltpos (`re15_skel_compute_pose`) speisen — der GTE-Attack-Point `FUN_80019e20`
   `[0x14/0x15/0x16]` / `FUN_80104178` wird bewusst NICHT als PSX-Model-Pool nachgebaut. Die
   Movement/Anim-Exec-Leaves (`+0x5`≥3, rufen `anim_set`/walker) + das Anim-Keyframe-Lunge-Begin
   auf Walker/Skeleton mappen.
2. **room1140-Overlay-Spawn:** der bisher fehlende Spawn-Pfad — die Briefing-Zombies kommen NICHT
   via `Sce_em_set`, sondern aus der Overlay-Entity-Liste. Ohne diesen Spawn gibt es keinen
   aktiven Gegner zum Ticken.
3. **`game_step`-Wiring:** `re15_enemy_ai_step` pro aktivem Gegner aufrufen. **1170-Risiko** →
   vorsichtig additiv (inert solange kein Gegner spawnt) + savestate-verifizieren.
4. **Dynamische Verifikation:** mit `re15-room-capture` (`re15_quickload.py`/`--provoke`) +
   `re15-savestate-ghidra` (`re15_enemy_state.py`): Spieler in Range → Gegner-`+0x5`-Transition +
   HP-Fall vergleichen Port vs. Original.

Werkzeuge: **`re15-psx-disasm`** (EXE/Overlay-Disasm), **`re15-savestate-ghidra`** (Live-RAM +
Tabellen-Patch-Check), **`re15-room-capture`** (Raum laden/provozieren). Memory `reai-v2-foundation-combat`
hat die volle Detail-Karte (alle Adressen, FSM-Stufen, deferred-Teile).

---
*Historischer Handover (2026-06-28, der 44-Fix-Byte-true-Audit) — unten als Referenz erhalten.*

## TL;DR (2026-06-28, historisch)

- **Git:** HEAD war `0aed072b`; Code-Work in `3b4be5b8 "Many things finished"`.
- **Build/Test:** damals 28/28 grün.
- **Audit-Arbeitsweise:** jeden Audit-Fix VOR Anwendung gegen die Disasm verifizieren
  (≈70 % der Audit-Werte waren ungenau), dann anwenden → build → ctest.

## Diese Session geliefert (alles gebaut + 28/28 ctest grün)

### Byte-true Fixes — Wave 2 (#28/#32/#34/#36)
- **#28** Rotor-SE (rotor_common.c): Pan `& 0x7f` statt clamp (andi @0x80045d28/d3c); Distanz mit
  Y-Term `cam.y-|cam.y-heli.y|` + verschachteltem SquareRoot0 (@0x80045b04/b34/b38).
- **#32a** RVD-Zonen-Cap 32→64 (re15_rdt.h; FUN_80014230 hat kein 0x20-Limit). **#32b** FLR
  width/depth signed `lh` (@0x800437fc/0x80043814). #32c (Terminator-nach-Advance) **deferred**
  (OOB-Risiko, nur Leer-FLR-Edge).
- **#34** SCD 0x5F–0x63 RE2-Handler entfernt + Größen 0x5F–0x8E→1 (Tabelle endet 0x5E @0x80074620).
- **#36a** Dialog-Dismiss `& 0xc000` Cross|Square Edge (@0x80028458/698). **#36e** Page-Resume
  `message_timer=message_scroll` (@0x80028538). #36b/c/d **deferred** (Glyph-Cap braucht
  Font-Atlas-Check; FF/Lookahead = Mechanismus-Umbau).

### Byte-true Fixes — Wave 3 (#6/#7)
- **#6** Cmp (0x23): var_idx `pc[1]→pc[2]` (andi 0xff @0x8003ff80) + Operator-Cases 2/3/4 →
  `>=`/`<`/`<=` (switchD_8003ffc0). RISIKO: PSX-Boot-Recheck wert (ROOM1170-Boot hat aber kein Cmp).
- **#7** Save/Copy/Calc/Calc2 (0x24–0x27): work_vars[256] existierte schon; ALU-Helfer
  `scd_work_alu` = 12-Op-Tabelle FUN_80040140 (inkl. SHR-logical 0xA / SHR-arith 0xB, Div-durch-0=kein Write).

### #8 — Loop-Modell-Umbau (KOMPLETT, byte-true)
- **Unified Modell** in `scd_thread_t`: `loop_back[4]`/`loop_exit[4]`/`loop_for_cnt[4]`/`loop_count`
  (PSX +0x20/+0x60/+0xA0/+0x08) ersetzt do_stack/for_*.
- **7 Handler** byte-true: For(0x0D)/Next(0x0E)/**While(0x0F)**/**Ewhile(0x10)**/Do(0x11)/Edwhile(0x12)/**Break(0x1A)**.
- **`scd_eval_pred_chain`** — AND/OR-Prädikat-Kette via echtem Dispatcher; Returns auf
  `==SCD_R_CONTINUE` normalisiert (Prädikate liefern 1/3, nicht 0/1).
- `op_if/else/endif` + `block_stack` BLEIBEN getrennt (eigenes Subsystem).
- **Neue byte-true Tests:** `test_do_edwhile_loop` + `test_break_exits_loop` (test_scd_opcodes.c).
- Regressionsbeweis: For/Next-Logik identisch (nur Feld-Rename), Do/Edwhile-Ck byte-identisch, Rotoren=Goto.

### DuckStation Dynamic-RE Skills (live verifiziert)
- **`re15-room-capture`** (`.claude/skills/`): vgamepad-Treiber lädt einen RE1.5-Raum übers
  Debug-Menü (`--right N --triangle M --postload S`, Cross-Stage braucht ≥16) → SaveStateOnExit
  → `re15_capture.py` macht Capture+Auto-Verify mit Schwarzbild-Erkennung.
- **`re15-savestate-ghidra`** (`.claude/skills/`): `re15_ss.py` (zstd→RAM-Base 0x31a62→Reads+VRAM-PNG),
  `re15_savestate_inspect.py` (Stage-Overlay-Fingerprint @0x80100000 + HP/State/Cut/Flags),
  `re15_diff.py` (unbekanntes Global finden), `re15_ghidra.py` (Headless-Decompile-Wrapper, gecacht).
- Demo-Savestates: `stage_saves/mzd_demo/` (Stage 1 fp 0x96290818, Stage 2 fp 0x1c0394d0).
- **Nutzen:** riskante Fixes (#6 PSX-Boot, #13 Damage, #14/15 AOT) jetzt laufzeit-verifizierbar,
  ohne dass der Nutzer selbst spielt.

### Repo-Hygiene
- ~15.000 Build-Artefakte aus dem Index entfernt (`re15_port/build`, das versehentliche
  `re15_port/cmake/build` mit 7888 Dateien, `psx_dev/.../build*`). `.gitignore` robust:
  `build/`, `.claude/worktrees/`, `debug.log`, `.idea/`. **NIE `build/`-Dateien committen.**

### #9 — Switch/Case/Default/Eswitch (KOMPLETT, byte-true)
- **op_switch (0x13, LAB_8003fa5c)** scannt jetzt die GANZE Case/Default-Tabelle inline und springt
  direkt auf matched-case-body / default-body / past-eswitch (der Audit-Hinweis „Case off-by-2" war
  zu klein gegriffen — der echte Mechanismus liegt im Switch-Handler). Pusht/poppt das unified
  Loop-Modell (#8): Break (0x1A) verlässt den Switch über `loop_exit`, Eswitch (0x16) balanciert
  `loop_count`. **op_case=+6 (reiner Fall-through), op_default=+2, op_end_switch=loop_count--/+2.**
- **Index-Konvention belegt** gegen op_do (LAB_8003f8bc) + op_break (LAB_8003fca8): Push-am-neuen-Index,
  Break-liest-aktuellen, Pop-dekrement (PSX 1-indexiert → Port konsistent 0-indexiert). `switch_val`-Feld
  entfernt (kein Konsument mehr). Verifikation: eigener Disasm-Trace + Agent (alle 5 Claims CONFIRMED).
- **Neuer Test:** `test_switch_case_break` (3 Pfade: Match-mid-table+Break, erstes-Case-Match, No-Match→Default;
  loop_count balanciert je auf 0). **26/26 ctest grün.**

### #10 — Evt_chain (0x03) + Evt_exec (0x04) (KOMPLETT für Normal-Modus, byte-true)
- **Evt_chain (0x03, LAB_8003f270 → FUN_8003edec)** war ein No-Op (chained Sub lief NIE). Jetzt
  **in-place Thread-Reinit**: `pc=sub_scd[pc[3]]`, `loop_count=0`, `block_sp=0`, KEIN pc+=4;
  call_depth/work/locals/sleep BLEIBEN (FUN_8003edec macht KEIN memset — der Audit-„Reset call_depth/sleep"
  war falsch).
- **Evt_exec (0x04, LAB_8003f2a0 → FUN_8003ee3c)**: `cond=pc[1]`, `sub_id=pc[3]`, pc+=4, dann Slot-Wahl.
  Byte-true Normal-Modus (DAT_800b3f7a==0, agent-belegt): **`cond<10` → Slot=cond DIREKT** (force-overwrite) —
  fixt den room1021 `Evt_exec(9,0)`-Drop. `cond>=10` → Auto-Scan.
- **#24-Kopplung:** Die exakte PSX-Auto-Scan-Range [2..9]/[10..13] + Mode-Flag hängt am Thread-Pool-Umbau
  (24→14, #24). Port-Auto-Scan bleibt [10..23] (Slot 2 dediziert) bis #24 — ROOM1170-Intro Evt_exec(255,11)
  unverändert → keine Regression.
- **Verifikation:** eigener Disasm-Trace + Agent (4/4 Claims CONFIRMED inkl. FUN_8003edec-Felder + 5 DAT_800b3f7a-Writer).
  Neue Tests `test_evt_chain_reinit` + `test_evt_exec_direct_slot` (RDT-Mock). **26/26 ctest grün.**

### #13 — Player-Damage (KOMPLETT für Player-Branch, byte-true)
- **Neu:** `engine/src/re15_damage.c` + `include/re15_damage.h`; actor-Felder `hit_react`(+0x93)/`status_flags`(+0x98).
  `re15_player_take_damage(p, attack_type, src_x, src_z)` = Player-Branch von **FUN_80012d60** (@80012e18-f04).
- **Mechanik (instruktionsgenau, `ghidra1_V2.txt:77607-77814`, Disasm == Decompile):** Hit-Once-Gate
  `+0x93 bit0` (@80012e30 → 1 Treffer/Attacke) → `HP -= re15_damage_table[type]` (HP=s16@+0x9a, @80012e44-64) →
  `type<2`: Bleed/Gift-Roll `+0x98 bit0x2` via 2×RNG&1 (@80012ea4) + SE#10 (deferred) → state=2(hurt) /
  sub_state_1=Front/Back+2 / sub_state_2=0 / `+0x93|=1` → **signed HP<0 → state=3(death)**, sub-states 0.
- **Tabellen byte-true:** `DAT_8006f418` (dmg) = **{10,20,1000,1000,1000,50,100,200,300,1000,0}** — **11** Einträge
  (`[10]=0`, ghidra:223455-223478; der Audit listete nur 10). `DAT_8006f430` (Reaktion) = {03,03,09,0A,0B,0E,0F,10,11,12,14}.
- **Caller:** `FUN_80017fa4` (Gegner-Lunge, Dispatch-Tab @0x80071da4) → `FUN_80012d60(0x1f4, hitbox, 0)`,
  `clear a2` @80018004 = **attack_type 0 = 10 dmg** (Zombie-Biss). Hitbox = Angreifer-Pos +0x28/+0x2a/+0x2c via FUN_8001c6e8.
- **DEFERRED (ehrlich):** Gegner-Attack-FSM (@0x80071da4) + Actor-Hitbox `FUN_8002b5d0` sind NICHT portiert (Port hat
  keine Gegner-AI/Hitbox) → nichts ruft den Resolver in-game; er ist der getestete byte-true Kern für die Gegner-AI-Phase.
  Hit-Once-Clear (`re15_player_clear_hit_guard`) + 2. Call-Site @800185b8 (Audit „a2=2 Instakill", ungeprüft) = selbe FSM.
  Front/Back-Anim faithful abgeleitet, unbeobachtbar bis Hurt-Clips. Bleed-RNG cycle-exakt unmöglich (leftover-Register),
  1/4-Wkt erhalten. Verwandt: **#22** (Idle-FSM auf state==Idle gaten), **#41** (Poison-Drain HP-=2 — Status wird jetzt gesetzt).
- **Verifikation:** eigener Disasm-Trace (LAB-für-LAB) + Damage-/Reaktions-Tabelle direkt aus den Daten-Bytes gelesen.
  Test `tests/unit/test_damage.c` (6 Tests: 10-dmg-Biss, Hit-Once-Sperre+Re-arm, HP<0→death, Instakill, Bleed-Gate
  507/2000≈1/4, Tabelle byte-true). **27/27 ctest grün; Headless-Boot ~10s ohne Crash.**

### #11 + #12 — Member_set/cmp id-Tabelle (KOMPLETT, byte-true)
- **Kern:** Der Port übersetzte RE1.5-Member-ids → RE2-ids (`re15_to_re2_member_id`) — eine **Fiktion**. RE1.5 nutzt
  die DIREKTE Tabelle `FUN_8004116c` (set, ghidra:151322-151380) / `FUN_80041358` (get, :151486-151575): id 0..0x13
  → Actor-Offset. Übersetzung GELÖSCHT; `re15_actor_get/set_member` (actor_common.c) nehmen jetzt die rohe RE1.5-id.
- **Tabelle:** 0→x 1→y 2→z 3→rot_x 4→rot_y 5→rot_z 6→flags 7→+0x0c 8→**state** 9→sub_state_1 10→sub_state_2
  11→sub_state_3 12→**+0x09 grid_id** 13→sub_state_4 14→+0x0a 15→+0x0b 16→anim_flags 17→status_flags 18→floor 19→hp.
- **Bugs behoben:** id12 (häufigster Member_set!) schrieb via RE2-0x0C → **Leon.y = Spieler unter Boden**; jetzt grid_id.
  id8 schrieb motion statt **state**; id6/9/10/11/13/16/17/18/19 falsch/gedroppt. 4 Orphan-Felder ergänzt → alle 20 ids
  round-trippen. **ids 0–5 (Koords/Rot) byte-identisch zu vorher → ROOM1170 unverändert (keine Regression).**
- **#12:** Operatoren `{0:== 1:> 2:>= 3:< 4:<= 5:!= 6:&}` + LE-Wert waren schon byte-true (frühere Welle); der
  Restfehler war NUR die geteilte kaputte id-Quelle → mit #11 mitbehoben (cmp liest jetzt das richtige Feld).
- **GET-Vorzeichen** exakt über Port-Feldtypen (lbu zero-extend, lh sign-extend, lhu, lw). **Verifikation:** eigener
  Disasm-Read aller 20 Case-Bodies + Sprungtabellen. Test `tests/unit/test_member.c` (Direkt-Tabelle + VM-cmp, Feld-
  Quelle/LE/Operatoren). **28/28 ctest grün; Headless-Boot ~9s ohne Crash.**

## Tote Themen (NICHT wieder aufmachen)
- **„Pixel-Verschiebung"**: Der Nutzer hat bestätigt — es gibt KEINE sichtbare. Render-Math
  **#1–3** (RotMatrix/Trig-LUT/Kamera-Integer) sind reine byte-true-Korrektheit OHNE sichtbaren
  Bug → **keine Priorität**. War aus CLAUDE.md entfernt; nicht erneut als Pixel-Shift-Fix framen.

## Arbeitsweise für die nächste Session

1. **Build-Recipe:** `export PATH="/c/msys64/mingw64/bin:$PATH"` →
   `cmake --build re15_port/build` → `ctest --test-dir re15_port/build --timeout 30`.
   (Test-Flags beim Configure: `-DRE15_BUILD_TESTS=ON -DRE15_BUILD_TOOLS=ON`.)
2. **Verifikations-Workflow (bewährt):** pro Fix einen `general-purpose`-Agenten ansetzen, der
   die Audit-Behauptung gegen `ghidra1_V2.txt` + `RE_15_Quellcode_V2/` belegt und einen
   apply-ready Edit (alter→neuer Schnipsel) + VERDICT (CONFIRMED/WRONG/UNSURE) liefert.
   Unabhängige Fixes parallel. **Audit-Werte NIE blind übernehmen.**
3. **Nach jedem Fix:** build + ctest grün halten; bei riskanten Verhaltensänderungen zusätzlich
   per Savestate-Pipeline (Skills) gegen das echte Spiel verifizieren.
4. **Memory pflegen:** angewandte Fixes in `reai-v2-byte-true-audit` nachtragen.

## Nächste Schritte „der Reihe nach"

### #13 Player-Damage (FUN_80012d60, komplett fehlend) — JETZT, CRITICAL
- **Audit #13:** Der komplette Player-Damage-Pfad fehlt im Port (FUN_80012d60 = Damage-Entry,
  Damage-Tabelle, Death/Hit-State, I-Frames). HP-Global `DAT_800acaee` jetzt per Savestate lesbar
  (Skill `re15-savestate-ghidra`) → ideal für Laufzeit-Verifikation. Braucht Enemy-Attack-Hitbox als
  Caller (wer ruft FUN_80012d60?). Vor RE: `RE15_FUN_CATALOG.md` + Skill `ghidra-mapping` prüfen.
- **Muster:** Agent gegen FUN_80012d60 (+ Caller-Xrefs, Damage-Tabelle, I-Frame-Timer) verifizieren →
  anwenden → build+ctest → zusätzlich per Savestate-Pipeline gegen echtes Spiel (HP-Wert) prüfen.

### Danach (Audit-Reihenfolge, je per Agent verifizieren)
- **#14/#15** AOT-Scan-Refactor (Edge→per-Frame + Multi-Entity + 9-Frame-Tür) — riskant, gegen
  Savestate verifizieren.
- **#17/#18** Goto/Gosub-Unwind + Do-Exit-PC (jetzt mit dem unified Loop-Modell tractabler).
- **#21** Letterbox-Parität, **#22** Player-State-FSM (verzahnt mit #13), **#25** Aot_on,
  **#26** Work_set kind=5 (offene EXE-Auflösung DAT_800b23f4), **#27** Message_on, **#29**
  Collision-Slopes (5 Handler), **#30** Stair-Finalize, **#31** Camera-Orbit, **#33** EMD-MD1.

### Deferred / nicht byte-true (dokumentiert in BYTE_TRUE_AUDIT.md + Memory)
- #32c (FLR-Terminator, OOB-Risiko), #36b/c/d (Glyph-Cap/FF/Lookahead), Render-Math #1–3 (keine
  Priorität), For-count==0-Skip in #8 (bewusst defensiv, als nicht-byte-true markiert).
- NOCH OFFEN aus #8: Break-Flag-Byte (thread+0x4-Konsument unbekannt), Loop-N pro call-depth statt flach.

## Schlüssel-Referenzen
- `BYTE_TRUE_AUDIT.md` — 44 priorisierte Fixes mit zitierten Adressen.
- `RE_15_Quellcode_Overlays/` (Raum-/Gegner-Logik, PRIMÄR) · `RE_15_Quellcode_V2/` + `ghidra1_V2.txt`
  (EXE-Engine, Fallback) · `RE15_KNOWLEDGE.md` (Formate) · `RE15_FUN_CATALOG.md` (verifizierte FUNs).
- Memory: `reai-v2-byte-true-audit`, `reai-v2-duckstation-dynamic-re`, `reai-v2-ghidra-pipeline`,
  `reai-v2-build-recipe`.
