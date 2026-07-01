---
name: reai-v2-aot-mechanism
description: "Byte-true RE1.5 AOT-Scan-Mechanismus (FUN_800436a8 3-Pool + FUN_80042bac per-AOT) RAM-validiert; warum #14/#15-Refactor riskant ist"
metadata:
  type: reference
  originSessionId: 2fc24f5a-47cf-487a-9546-6de01c8a2791
---

Byte-true RE des RE1.5 AOT-Systems (2026-06-28), für [[reai-v2-byte-true-audit]] #14/#15. **RAM-validiert** gegen `stage_saves/mzd_demo/mzd_stage1_room.sav` via `re15-savestate-ghidra`-Tooling.

**Zwei getrennte Systeme in der Hauptschleife @0x8001ce14:**
1. `FUN_8002bd44` (@8001ce14) — Tür/Objekt-Interaktion (#15): 9-Frame-Halte-Zähler obj+0x8c, Held-Pad, Band obj+0x82, Reach FUN_8002d1e8, Öffnen bei Zähler==9. Iteriert Objekt-Pool DAT_800b3f98 stride 0x94.
2. `FUN_800436a8` (@8001ce1c) — **AOT-3-Pool-Scan** (#14): ruft `FUN_80042bac(entity, kind, 0)` für PLAYER (DAT_800aca54, kind=1) + JEDEN aktiven Gegner (DAT_800acc2c stride 0x1f4, count DAT_800aca4e, kind=2) + JEDES Objekt/AOT (DAT_800b3f98 stride 0x94, count DAT_800ac778[2], kind=4). ghidra1:154537-154617.

**`FUN_80042bac` Per-Entity-Test (ghidra1:153690-154057)** — iteriert Zone-Ptr-Tabelle DAT_800ac9b0[] (count DAT_800afbb4). **Zone-Struct (savestate-validiert):**
- `[0]` = type → Dispatch `PTR_LAB_8007469c[type]`, Handler-Arg = zone+0x14 (wenn flags&0x80) sonst zone+0xc.
- `[1]` = **kind-mask in low 3 bits (0x1 player / 0x2 enemy / 0x4 object, `&kind`@80042c84)** + 0x10 (RVD/separate-Caller-Filter gegen a2 @80042c98) + 0x20 (center-test) + 0x40 (reach-test) + 0x80 (Handler-Arg-Select).
- `[2]` = **band: (band&0x80) → ignorieren, sonst entity+0x82 == band** (@80042cac).
- `[3]` = shape (0x80 has-shape, 0x40 quad FUN_80014368 sonst rect FUN_80042b64, &0x3f shape-idx).
- `[4..0x13]` = rect/quad-Eck-Offsets.
- **KEIN Vorframe-„inside"-Feld → LEVEL-Trigger (feuert JEDEN Frame inside; Re-Trigger-Sperre liegt im Typ-HANDLER, nicht im Scan).**

**Echte STAGE1-RAM-Daten (Validierung):** 7 AOTs — 5×{type2, mask 0x31 = player(0x1)+0x10(RVD)+0x20, band 1} = die **RVD-Kamera-Zonen** (per bit0x10 von separatem Caller, NICHT dem a2=0-Haupt-Scan) + 2×{type8, mask 0x47 = alle 3 Pools(0x7)+0x40(reach), band 0} = die Action-AOTs. Player band 1. → Struct + Maske + Band exakt wie aus der Disasm abgeleitet.

**Warum der #14/#15-Port-Refactor RISKANT ist (Entscheidungs-Kontext):** Der Port (`re15_port/engine/src/aot_common.c` `re15_aot_scan`) ist **result-korrekt aber mechanismus-divergent**: scannt NUR den Player, mit Edge-Trigger (`was_inside`) + Action-Press, und faltet die Tür-Logik IN den Scan (statt separater FUN_8002bd44-Schleife mit 9-Frame-Zähler). Diese Implementierung ist über VIELE Sessions für ROOM1170 (Intro-Cinematic: door6-Return, CAM_SWITCH-Cuts, Auto-Advance-Intro-Tür, Message-AOTs) **savestate-verifiziert getunt** (Door-Reach FUN_8002d1e8 @563 Units, Band-Gate, Cinematic-Unterdrückung — alles laufzeit-verifiziert laut Code-Kommentaren). Ein byte-true Vollumbau (3-Pool-Scan + Level-Trigger + separate Tür-9-Frame-Schleife + kind-mask + generischer Band-Gate) würde die EINZIGE laufzeit-verifizierte Logik neu schreiben mit **hohem 1170-Regressionsrisiko** und **null beobachtbarem 1170-Vorteil** (1170 ist player-only, same-band, keine Gegner-/Objekt-AOT-Trigger). Der Mechanismus ist jetzt belegt+RAM-validiert (Katalog „RVD / AOT": FUN_800436a8/FUN_80042bac) — der Umbau selbst sollte worktree-isoliert mit echtem ROOM1170-Savestate-Baseline (re15-room-capture) erfolgen, nicht blind auf master.

**Tooling-Notiz:** `re15-savestate-ghidra/scripts/re15_ss.py` (Ram-Klasse: zstd→RAM-Base via PSX.EXE-Signatur→u8/s16/u32-Reads). `re15-room-capture` (vgamepad/ViGEm, DuckStation @`%LOCALAPPDATA%/Programs/DuckStation`, Cue in Downloads/ePSXe2018) funktioniert, aber ROOM1170-Room-Step-Count (--right) ist nicht festgelegt (Boot startet in ROOM1240). Siehe [[reai-v2-duckstation-dynamic-re]].

**VERIFIKATIONS-SACKGASSEN für einen #14/#15-Port-Refactor (2026-06-29, im Worktree aot-1415 empirisch belegt — NICHT wiederholen):** (1) **Headless-AOT-Log ist NUTZLOS** als Regressions-Check: die Action-AOTs (Türen/Switches) brauchen Spieler-Input (Hingehen + Aktion drücken), den der Headless-Lauf NICHT hat → nur passive Auto-/Cam-Trigger laufen, die nicht loggen → der ROOM1170-Headless-Lauf produziert 0 AOT-Log-Zeilen. (2) **Kein Unit-Test des vollen Raum-Ladens** möglich: der Loader (room_pc.c, baut die AOTs aus den SCD-Opcodes) ist PLATTFORM-Code, NICHT in re15_engine.a → ein Unit-Test kann ROOM1170 nicht laden+AOTs aufbauen (test_room_load.c testet nur den RDT-Parser). (3) **Architektur-Mismatch:** der Port nutzt HOCHGRADIGE typed-AOTs (re15_aot_t: DOOR/CAM_SWITCH/ITEM/GENERIC/MESSAGE/STAIR aus den SCD-Opcodes gebaut), NICHT die originalen 0x94-Byte-Zone-Structs (zone[0]=type/[1]=mask/[2]=band) → man kann die echten Savestate-Zone-Bytes NICHT in `re15_aot_scan` einspeisen; ein byte-true Scan-Refactor wäre ein VOLLER AOT-Daten-Modell-Umbau (typed→0x94-struct), kein lokaler Fix. **Fazit:** Der byte-true #14/#15-Refactor hat (a) keinen autonomen Trigger-Verifikations-Pfad, (b) seine sicheren Teile (3-Pool-Scan) sind im Port INERT (keine Gegner-/Objekt-AOT-Effekte), (c) seine wirksamen Teile (Level-Trigger, 9-Frame-Tür) sind genau die 1170-Risiko-Teile, (d) null beobachtbarer 1170-Vorteil. → Empfehlung: validierte RE bankiert lassen (master), Refactor NICHT blind machen. Erst sinnvoll, wenn der Port ohnehin auf das 0x94-Struct-Modell + skript-gesteuerte Input-Tests umgestellt wird.
