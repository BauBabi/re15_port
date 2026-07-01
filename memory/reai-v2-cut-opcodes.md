---
name: reai-v2-cut-opcodes
description: "Byte-true SCD-Opcodes 0x29/0x2A (Cut_chg/Cut_old) = Hintergrund einfrieren auf Schwarz — Mechanik des 1240-Briefings"
metadata:
  type: reference
  originSessionId: 073aec74-f763-4c50-aa34-afe1225f60d0
---

Byte-true Mechanik „Text auf Schwarz" (1240-Pre-Intro-Briefing) — vollständig RE'd 2026-06-28, statisch + dynamisch belegt. Für [[reai-v2-open-gaps]].

**SCD-Dispatch-Tabelle**: Basis `PTR_LAB_800744a8 = 0x800744a8` (Opcode 0; XREF von SCD-VM FUN_80016518/FUN_8003f0a0/FUN_8004d4c4). Opcode N → `*(0x800744a8 + N*4)`.

**Opcode 0x29 = Cut_chg → FUN_800402a0** (@`0x800402a0`, Tabelleneintrag `0x8007454c`; (0x8007454c−0x800744a8)/4 = 0x29). 2 Bytes `[op, cut_idx]`:
- `DAT_800aca3c |= 0x100` → BG-Renderer einfrieren
- `DAT_800b3f7b = DAT_800b0fe4` (alten Cut sichern), `DAT_800b0fe4 = operand_byte` (neuer Cut), `FUN_800142f4()` (einmal zeichnen), PC += 2.

**Opcode 0x2A = Cut_old → FUN_8004032c** (@`0x8004032c`, Eintrag `0x80074550`, Index 0x2A). 1 Byte:
- `DAT_800b0fe4 = DAT_800b3f7b` (Cut zurück), `FUN_800142f4()`, `DAT_800aca3c &= ~0x100` (Freeze aus), PC += 1.

**Leser** (disp-Hauptfunktion, scratchpad/disp_funcs.c Z.442 / EXE @`0x8001cce0`): `if (... && ((DAT_800aca3c & 0x100) == 0)) FUN_80014230(0);` → Bit 0x100 gesetzt = live-BG/MDEC-Renderer FUN_80014230 NICHT aufgerufen → Cut bleibt statisch eingefroren.

**Dynamischer Beleg** (DuckStation-Savestate, MZD-Disc = identische Original-PSX.EXE, RAM-Base 0x31a62): im 1240-Briefing `DAT_800aca3c=0x110` (Bit 0x100 BG-Skip + 0x10 Fade-Richtung), `DAT_800b0fe4=0` (Cut 0 = schwarz), `DAT_800b5568=0xf0` (Fade voll offen). ⇒ **1240-Briefing = Cut 0 (schwarz), eingefroren, Text drauf.**

**Port-Bug**: `op_cut_chg`/`op_cut_old` (scd_vm.c) führen nur cam_id+pending, OHNE Freeze. main.c:666 nimmt fälschlich „Cut 0 = Helipad-Shot" an (gilt für 1170, nicht 1240). Der Port vermischt 1240-Briefing (schwarz) mit 1170-Helipad-Cinematic.

**ROOM1240 echtes Pre-Intro = SUB2** (disassembliert aus `info/Re1.5/PSX/STAGE1/ROOM1240.RDT`, main_scd@0x40, sub_scd@0x44; RDT-Struktur via rdt_common.c parse_main_scd/parse_sub_scd_table). Ausgelöst: room-load → main00 (`Door_aot_set`+evt_end) → SUB0 `04 FF 18 02`=Evt_exec(typ0x18,event2) → **SUB2**. SUB2 = getaktete 9-Cut-Montage mit 6 Messages:
`Cut_chg(0)=schwarz sleep0xA0` (langes schwarzes Opening, KEIN Text) → `Cut_chg(1) sleep0x14 Message_on(0) sleep0xB4` → `Cut_chg(2) Message_on(1) Message_on(2)` → `Cut_chg(3) Message_on(3)` → `Cut_chg(4)` → `Cut_chg(5) Message_on(4)` → `Cut_chg(6)` → `Cut_chg(7) Message_on(5)` → `Cut_chg(8) Aot_on(0) evt_end`. Die `Set(2,0x07,0/1)`-Paare vor jedem Cut sind Sound/Flag-Toggles. ⇒ 1240 ist eine MONTAGE über alle 9 BG-Cuts mit Narration, „schwarz" ist nur das ~5s-Opening (Cut 0). Der Port muss SUB2 echt fahren statt des 1170-Helipad-Hacks.
