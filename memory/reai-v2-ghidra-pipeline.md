---
name: reai-v2-ghidra-pipeline
description: "Funktionierende Ghidra-Headless-Decompile-Pipeline für die RE1.5 PSX.EXE (für byte-true RE)"
metadata:
  type: reference
  originSessionId: 073aec74-f763-4c50-aa34-afe1225f60d0
---

Ghidra-Headless-Decompile der RE1.5-`PSX.EXE` funktioniert (eingerichtet 2026-06-28) — für byte-true RE, wenn `RE_15_Quellcode_V2`/`ghidra1_V2.txt` eine Funktion nicht (dekompiliert) enthält.

- **Ghidra**: `C:/Users/mjoedicke/Downloads/ghidra_11.4.2_PUBLIC_20250826/ghidra_11.4.2_PUBLIC/support/analyzeHeadless.bat`; Java 17 (Temurin) vorhanden; PSX-Loader-Extension + PsyQ-GDTs unter `…/Downloads/ghidra_11.4.2_PUBLIC_20250906_ghidra_psx_ldr/`.
- **EXE**: `info/Re1.5/PSX.EXE` (PS-X EXE, load@`0x80010000`, size `0xAF000`). Header (0x800 B) strippen → reiner Code: `tail -c +2049 PSX.EXE > code.bin`.
- **Import (ohne PSX-Loader reicht)**: `analyzeHeadless <projdir> <name> -import code.bin -processor "MIPS:LE:32:default" -loader BinaryLoader -loader-baseAddr 0x80010000 -scriptPath <dir> -postScript <script.py> -deleteProject`. Analyse ~22 s.
- **Post-Skript (Jython, Py2)**: `DecompInterface().decompileFunction(f,90,monitor).getDecompiledFunction().getC()`. Funktionen über `ReferenceManager.getReferencesTo(addr)` + `FunctionManager.getFunctionContaining` finden. **Ausgabepfad MUSS Windows-Form sein** (`C:/Users/…`), nicht MSYS `/c/…` (Jython = Windows-Python).
- Beispiel-Lauf: `scratchpad/decomp_disp.py` dekompilierte 61 Funktionen, die die Display-/Fade-Register referenzieren → `scratchpad/disp_funcs.c`.

**OVERLAYS (`info/Re1.5/PSX/BIN/STAGE{1..6}.BIN`, 2026-06-29):** Laden **RAW @0x80100000, KEIN 0x800-Header** (CLAUDE.md irrt hier — bei 0x800-Strip lägen die Daten-Tabellen am Ende jenseits der Dateigröße). **file-offset = addr − 0x80100000.** Verifiziert: STAGE1.BIN @0x1d6d4 = FUN_8011d6d4-Prolog (`addiu sp,-0x18; lui s0,0x800b; addiu s0,..0xca40`=_DAT_800aca40). → Für Overlay-Disasm/Tabellen reicht oft **direktes `xxd -s <offset>` + manuelles MIPS-Decode** (z.B. `lbu vX,OFF(vY)` = Dispatch-Byte-Offset; `addr`-Tabellen = 4-Byte-LE-Pointer) — schneller als ein Ghidra-Import, byte-true. Das löste die Zombie-AI-Dispatch-Typing-Ambiguität ([[reai-v2-foundation-combat]]), die das Decompilat inkonsistent (byte* vs word*) riet. Ghidra-Import des Overlays analog zur EXE: `-loader BinaryLoader -loader-baseAddr 0x80100000` (ohne Header-Strip).

Display-/Fade-Mechanik (byte-true, für [[reai-v2-open-gaps]] 1240-Text-auf-Schwarz): schwarzer BG = `FUN_80014230` (MDEC/BG-Renderer) in Hauptschleife @`0x8001cce0` übersprungen wenn `DAT_800aca3c & 0x100`. Vollbild-Fade = `FUN_80021a0c` (Level `DAT_800b5568` 0..0xF0=schwarz, Richtung `DAT_800aca3c & 0x10`). Offen: Setzer von Bit 0x10/0x100 (kein Wort-/Byte-OR im Dump → berechneter Store/zu isolierende Funktion).
