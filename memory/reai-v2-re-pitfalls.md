---
name: reai-v2-re-pitfalls
description: RE-Fallen 2026-06-30 — Overlay-File-Offset-Trap (kein 0x800-Header; ad-hoc-Offset las Garbage, fälschlich als „runtime-patched" gedeutet) + state-erreicht≠aktiv (+0x90-Gate) + Debug-JUMP-Nummern HEX
metadata:
  node_type: memory
  type: feedback
  originSessionId: 6e7d9ee4-acd2-4d38-85d6-10eed74d305f
---

Drei Fallen aus der ROOM1140-Combat-RE (2026-06-30). Tools: [[reai-v2-skills-ported]]. Disziplin: [[disasm-verify-decompiles]]. Combat: [[reai-v2-foundation-combat]].

## 1. OVERLAY-FILE-OFFSET-TRAP — kein 0x800-Header (NIE den Offset selbst rechnen)
Die PSX.EXE hat einen 0x800-Byte-Header (file-off = `0x800 + addr − t_addr`). Die **STAGE*.BIN-Overlays haben KEINEN Header** → file-off = `addr − 0x80100000`. Wer ad-hoc `addr − 0x80100000 + 0x800` rechnet (den EXE-Header fälschlich annimmt), liest **0x800 Bytes zu weit = Garbage/Nullen**.
**Mein Fehler:** ich las die AI-Dispatch-Tabellen `@0x8011f840`(decide)/`@0x8011f890`(animate) mit `+0x800`, bekam Nullen und schloss fälschlich „die Tabellen sind RUNTIME-GEPATCHT, on-disc null". **FALSCH.** Beim korrekten Offset stehen sie vollständig in STAGE1.BIN und sind **40/40 identisch zum Savestate-RAM** (`re15_runtime_table.py` als Cross-Check bewies es) → die Tabellen sind **statische Overlay-Daten, NICHT gepatcht**. Runtime-Werte = on-disc-Werte: 0=search(0x80101b64) 2=engage(0x80102058) 3/4=grab(0x80102548) 5/6=walk(0x80102bd0) 7=turn(0x80102dc8) 0xc=feeding(0x801048a8) 0x13=engage+fw-precheck(0x8010561c).
**Lehre:** NIE den File-Offset selbst rechnen — `re15-psx-disasm` `re15_disasm.py` (dis/table/read, nutzt `load()` = immer korrekt) bzw. `re15_ss.Ram` verwenden. Liest eine Tabelle als null → ERST Offset/Adresse/`--bin` verdächtigen, NICHT „runtime-patched" schließen. `re15_runtime_table.py <sav> <addr> N` = der Cross-Check statisch↔RAM (Match ⇒ statisch; Diff ⇒ wirklich runtime-gebaut).

## 2. State-ERREICHT ≠ state-AKTIV (das +0x90-Gate)
`+0x5=0x13` (FUN_8010561c) = engage(FUN_80102058) PLUS ein führender, durch `+0x90 & 3` gegateter forward-walk-Vorabcheck; Rest byte-identisch zu engage. Im Live-Save ist `+0x90 == 0x00` für ALLE 5 Zombies → der fw-Branch wird NIE genommen → 0x13 verhält sich byte-identisch zu engage für ROOM1140 → der Port (wake→engage 2) ist faithful, NICHTS zu portieren. **Regel:** einen Dispatch-State zu erreichen heißt nicht, dass sein Verhalten aktiv ist. VOR dem Portieren eines gegateten Pfads: (1) `re15_enemy_state.py <sav> --ai` (wertet den +0x90-Gate aus), (2) das Gate-Flag über ALLE Combat-Saves sweepen `re15_flag_sweep.py`, (3) Gate==0 in allen → dormant, nicht portieren.

## 2b. Frame-Counts (und gemerkte Konstanten) NIE aus der Doku vertrauen — Asset-Bytes parsen
8.16: der Handover/Workflow notierte PL00.EDD clip 17=25f / clip 18=20f für die Player-Aim-Anim. Beim Implementieren die clip-Tabelle geparst (`u16 @ i*4`, 2× unabhängig extrahiert, identisch): **clip 17 = 10 Frames, clip 18 = 25** — vertauscht/falsch. Selbe Lehre wie 2 + [[disasm-verify-decompiles]]: ein erinnerter/notierter Wert (auch aus einem gründlichen Workflow) ist kein Beleg. Für Frame-Counts: EDD clip-Tabelle parsen (`clip[i].fc = u16@i*4`, clip[0] = u16@0). Disasm gibt die clip-INDIZES byte-true (`ori v0,0x11`→motion 17), die Asset-Bytes geben die FRAME-COUNTS.

## 3. Debug-JUMP-Nummern sind HEXADEZIMAL
ROOM1140 = **0x114 BRIEFING = `re15_quickload.py --left 16`** (von der Base mzd_debugmenu.sav). 0x11A SEWER EXIT = --left 10 — 0x11A als „114" dezimal zu lesen kostete 6 Schritte. Live-Engage-Capture (player alive, alle 5 wach): `re15_quickload.py --left 16 --postload 10 --path "R0.5,U2,R0.3,U6"` → stage_saves/mzd_stage1_engage_live.sav. --path L/R DREHEN (Tank, kein Strafe), R = Bildschirm-links zu den Zombies, zwei-R robuster. Tool: re15-room-capture.
