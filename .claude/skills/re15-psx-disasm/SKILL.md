---
name: re15-psx-disasm
description: Disassembliert RE1.5 PSX.EXE- und STAGE*.BIN-Overlay-Funktionen DIREKT als MIPS R3000 (kein Ghidra-Setup nötig) und decodiert Pointer-/Dispatch-Tabellen — der "Weg C" für Code, der NICHT in den Decompile-Dumps (RE_15_Quellcode_V2 / RE_15_Quellcode_Overlays) liegt. Verwenden, wenn eine FUN_/LAB_-Adresse roh-Byte im Ghidra-Dump steht, eine Sprung-/Vtable byte-true aufgelöst werden muss, oder eine Funktion schnell zusammengefasst werden soll (State-Writes, Calls, Schwellen).
---

# RE1.5 PSX-Direkt-Disassembler (Weg C)

Die Decompile-Dumps sind primär (`ghidra-mapping`, `RE15_FUN_CATALOG.md`). **Aber:** nicht jede Funktion ist dort decompiliert — die generischen Humanoid-AI-Leaves (`0x8004f..`) z.B. lagen 2026-06-29 nur als **roh-Byte** in `ghidra1_V2.txt`. Dann ist der schnelle Weg, direkt aus dem Binary zu disassemblieren ("Weg C"), statt ein Ghidra-Headless aufzusetzen.

## Datei-Layout (Adresse → Binary → Datei-Offset)

| Adressbereich | Binary | Offset |
|---|---|---|
| `0x80010000`–`0x800effff` (EXE) | `info/Re1.5/PSX.EXE` | PS-X-EXE-Header: text @ Datei `0x800`, `t_addr` @Header `0x18` (meist `0x80010000`) → `off = 0x800 + addr − t_addr` |
| `0x80100000`–`0x80121fff` (Overlay) | `info/Re1.5/PSX/BIN/<stage>.BIN` | **RAW @`0x80100000`, KEIN `0x800`-Header** → `off = addr − 0x80100000` (Default-Stage `STAGE1.BIN`, sonst `--bin`) |

Das Script wählt das Binary automatisch nach Adressbereich.

## Befehle (`scripts/re15_disasm.py`)

```bash
S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
python $S dis   0x8004f100 60         # n Instruktionen disassemblieren (Globals/Calls annotiert)
python $S table 0x801217a0 24         # n Words als Pointer-Tabelle (taggt EXE/overlay/data)
python $S scan  0x8004f3a4            # Funktion zusammenfassen bis jr ra
python $S bytes 0x80121790 12         # roher Hex-Dump
python $S dis   0x8011d6d4 30 --bin STAGE5.BIN
```

- **`dis`** — vollständiger R3000-Disassembler (lui/lw/sb/sh/sltiu/beq/bne/jal/jr/…), annotiert bekannte Globals (`g_entity 0x800ac784`, `playerX/Z`, `player.hit_react 0x800acae7`, `attack_workstruct 0x800b52c4`, …) und Call-Ziele (`arc_test`, `atan2_q12`, `resolve_attack`, `anim_set`, …). Die Maps `GLOBALS`/`CALLS` im Script erweitern, wenn neue Adressen bestätigt sind.
- **`table`** — liest Words als Funktionspointer; markiert `overlay` (`0x8010xxxx`) vs `EXE` (`0x800xxxxx`) vs `data/?`. Das Werkzeug für AI-Dispatch-/Action-Tabellen.
- **`scan`** — symbolischer Schnell-Überblick einer Funktion: `sb`-Writes auf `+0x4/+0x5/+0x6/+0x7` (FSM-State-Transitions), `jal`-Ziele, `sltiu`-Schwellen (z.B. Distanz-Gates), bis `jr ra`. Ideal, um den Transition-Graph mehrerer Leaves zu kartieren, ohne jede Zeile zu lesen.

## Verifikation (so wurde es belegt)

- Pointer-Tabelle gegen die **Live-RAM** gegenprüfen (ist sie zur Laufzeit gepatcht?): mit `re15-savestate-ghidra` (`re15_ss.Ram`) dieselben Adressen aus einem Savestate lesen und mit der `table`-Ausgabe vergleichen. 2026-06-29 so bewiesen: die STAGE1-AI-Tabellen sind **nicht** gepatcht (Live == on-disc) → der Live-Pfad sind wirklich die EXE-Leaves.
- Prolog-Spot-Check für die Load-Adresse: `dis <fn> 4` muss ein plausibles `addiu sp,sp,-N; sw ra,..` zeigen.

## Verwandte Skills / Quellen

- `ghidra-mapping` + `RE15_FUN_CATALOG.md` — IMMER zuerst (vielleicht ist die Adresse schon kartiert).
- `re15-savestate-ghidra` — dynamische RAM-Werte + Tabellen-Patch-Check (Schwester-Methode).
- `scd-disassembly` — für SCD-Bytecode (nicht MIPS), eigene Opcode-Tabelle.
- Memory `reai-v2-foundation-combat` — die Zombie-AI-RE, die dieses Tooling erzwang.
