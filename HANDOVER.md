# RE1.5 Port — Session-Handover (Stand 2026-06-28)

Kanonisches „lies-mich-zuerst" für die nächste Session. Ergänzt `BYTE_TRUE_AUDIT.md`
(44 priorisierte Fixes) und die Auto-Memory (`reai-v2-byte-true-audit`,
`reai-v2-duckstation-dynamic-re`, `reai-v2-ghidra-pipeline`).

## TL;DR — Wo stehe ich

- **Git:** HEAD = `0aed072b` (master, sauber). Der **Code-Work** (alle u.g. Fixes) liegt in
  `3b4be5b8 "Many things finished"`; `0aed072b` legt Skills/Doku/Cleanup obendrauf.
- **Build/Test:** `cmake --build re15_port/build` + `ctest --test-dir re15_port/build --timeout 30`
  → **26/26 grün** (mingw64 GCC + Ninja, `PATH` muss `C:\msys64\mingw64\bin` enthalten).
- **Nächster Schritt „der Reihe nach":** **#10 Evt_chain/Evt_exec** (Thread-Reinit + Slot-Bereich).
  (#9 Case/Switch ist ERLEDIGT — byte-true, build+ctest grün; siehe unten.)
- **Arbeitsweise:** jeden Audit-Fix VOR Anwendung per Agent gegen die Disasm verifizieren
  (≈70 % der Audit-Werte waren ungenau), dann anwenden → build → ctest.

## Diese Session geliefert (alles gebaut + 26/26 ctest grün)

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

### #10 Evt_chain/Evt_exec (Thread-Reinit + Slot-Bereich) — JETZT
- **Audit #10:** Evt_chain (0x03) ist im Port No-Op → muss In-place-Thread-Reinit machen
  (LAB_8003f270 @0x800744b4: sub_id=pc[3] @0x8003f280, jal FUN_8003edec = Thread-Reset PC/+4/+8,
  thread+0x140, PC=base+base[sub_id*2]). Evt_exec (LAB_8003f2a0): cond=pc[1], FUN_8003ee3c
  Slot-Allocator (cond<10/<14 → Direkt-Slot; 0xFF Normalmodus Slot 2..9). Asset room1021/sub02
  'Evt_exec(9,0)'. Ort: `scd_vm.c:2701-2707 (op_evt_chain No-Op), :736-743 (op_evt_exec), re15_scd.h:42-43`.
- **Muster:** Agent gegen LAB_8003f270/LAB_8003f2a0 + FUN_8003edec/FUN_8003ee3c verifizieren → anwenden → build+ctest.

### Danach (Audit-Reihenfolge, je per Agent verifizieren)
- **#13** Player-Damage (FUN_80012d60, komplett fehlend) — CRITICAL; HP-Global `DAT_800acaee`
  jetzt per Savestate lesbar → ideal für Laufzeit-Verifikation. Braucht Enemy-Attack-Hitbox als Caller.
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
