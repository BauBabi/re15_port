---
name: re15-pcsx-watchpoint
description: Lokalisiert die EXAKTE Instruktion, die eine PSX-RAM-Adresse schreibt (auch indirekte/DMA-Writes, die ein statischer Disasm-Scan verfehlt) — via PCSX-Redux + scriptbarem Lua-Memory-Write-Breakpoint, das Intro autonom über einen virtuellen Gamepad gefahren. Verwenden, wenn "wer setzt Adresse X / bit Y" statisch NICHT auffindbar ist (indirekter Store über ein Basis-Register, DMA, memcpy) und der literale PC-Beweis gebraucht wird. PCSX-Redux ist der EINZIGE lokale Emulator hier mit scriptbaren Watchpoints (DuckStation-Debugger ist GUI-only).
---

# RE1.5 PCSX-Redux Memory-Watchpoint (literale Setter-Instruktion finden)

**Zweck:** die byte-true Frage „**welche Instruktion schreibt Adresse X / setzt bit Y?**" beantworten,
wenn der statische Scan (alle sw/sh/sb, direkt + base-tracked, in EXE/DEBUG.BIN/STAGE*.BIN) sie NICHT
findet — weil der Store durch ein **Basis-Register** geht, das der Disassembler nicht auflöst (z.B. eine
Adresse aus einer Tabelle geladen), oder es ein **DMA**-Write ist. Ein adress-basierter Watchpoint
feuert für JEDEN Write auf die physische Adresse (CPU-Store UND DMA) und liefert den PC.

Bewährt 2026-07-21: der Fade-zu-schwarz-Setter `DAT_800aca3c & 0x10` war statisch **airtight
non-existent** (0 direkte Stores). Der Watchpoint fing ihn bei **`PC=0x8003fe74 sw v1,0(a3)`** — der
SET-Pfad des generischen Flag-Helfers `FUN_8003fdf8` (a3 = `bank_table[bank]` aus @0x80074664, deshalb
statisch unsichtbar). Ergebnis: aca3c = Flag-Bank 1, Fade = SCD-Opcode `Set(1, 27)`. Siehe Memory
[[reai-v2-global-ai-freeze]].

## Voraussetzungen (auf dieser Maschine)
- **PCSX-Redux**: `winget install -e --id GrumpyCoders.PCSX-Redux --accept-package-agreements --accept-source-agreements`
  → `%LOCALAPPDATA%\Microsoft\WinGet\Packages\GrumpyCoders.PCSX-Redux_*\pcsx-redux.exe`.
- **BIOS**: `SCPH1001.BIN` (aus DuckStation: `%LOCALAPPDATA%\DuckStation\bios\SCPH1001.BIN`) → nach `C:\tmp\scph1001.bin` kopieren.
- **Disc**: die MZD-`.bin` (`…\ePSXe2018\Biohazard 1.5 (MZD Mod)….bin`). ⚠️ **Pfad OHNE Leerzeichen/Klammern** — PCSX-Redux' Arg-Parser bricht sonst. Hardlink auf simplen Pfad:
  `New-Item -ItemType HardLink -Path C:\tmp\mzd.bin -Target "…\Biohazard 1.5 (MZD Mod)….bin"` (gleiches Laufwerk, kein 215MB-Copy) + eine `C:\tmp\mzd.cue` (`FILE "mzd.bin" BINARY / TRACK 01 MODE2/2352 / INDEX 01 00:00:00`).
- **vgamepad** (ViGEmBus) — wie in `re15-room-capture`.

## Ablauf (ein Kommando)
```bash
# Watch 0x800aca3c, flagge das Setzen von bit 0x10:
PCSX_WATCH_ADDR=0x800aca3c PCSX_WATCH_BITMASK=0x10 \
PCSX_WATCH_LOG=/c/workspace/git/reAi_v2/shots/pcsx_watch.log \
  python .claude/skills/re15-pcsx-watchpoint/scripts/pcsx_drive.py
# → shots/pcsx_watch.log:  "#N pc=8003fe74 … old=0 newval=0x10  <== SETS mask!"  +  ">>> SETTER: pc=8003fe74"
```
Der Treiber: erstellt den vgamepad → startet PCSX-Redux (`-interpreter -debugger -run -dofile watch_addr.lua`)
→ wartet auf Boot (~110s Interpreter) → fährt New Game (Up×3 + Cross ×3 + Advance-Crosses) → das Intro
läuft und schreibt die Adresse → das Lua loggt jeden Write mit PC. Läuft bis `>>> SETTER` oder `PCSX_MAX_SECS` (600).

## Lua-Watchpoint-API (das Wesentliche)
```lua
-- PCSX-Redux = LuaJIT (Lua 5.1): KEINE 5.3-Bit-Operatoren (& | >> <<) -> bit.band/bit.rshift/bit.bor!
local bp = PCSX.addBreakpoint(0x800aca3c, 'Write', 4, 'name', function(address, width, cause)
  local pc  = PCSX.getRegisters().pc                 -- der schreibende PC
  local rt  = bit.band(bit.rshift(instr,16),0x1f)    -- Store-Quell-Reg (sw/sh/sb: rt)
  local val = PCSX.getRegisters().GPR.r[rt]          -- geschriebener Wert (sw)
  local mem = PCSX.getMemPtr()                       -- uint8_t* auf 2MB RAM; Index = addr & 0x1fffff
end)
_G.__keep = bp   -- MUSS am Leben gehalten werden (GC entfernt sonst den Breakpoint!)
```
- Breakpoint braucht **`-interpreter -debugger`** (Dynarec hat keine akkuraten Breakpoints; Interpreter ~5x langsamer).
- `cause` unterscheidet CPU (`Lua Breakpoint`) vs `DMA channel N write` — DMA-Writes triggern den Breakpoint ebenfalls.
- Lua-Pad-Input (Alternative zum vgamepad, deterministisch): `PCSX.SIO0.slots[1].pads[1].setOverride(PCSX.CONSTS.PAD.BUTTON.CROSS)` (drücken) / `clearOverride(...)` (loslassen). Buttons: `UP=4, RIGHT=5, DOWN=6, LEFT=7, CROSS=14, CIRCLE=13, TRIANGLE=12, SQUARE=15, START=3, SELECT=0`.

## Fallen (teuer bezahlt)
1. **LuaJIT ≠ Lua 5.3**: `&`/`>>` = Parse-Error → das ganze Chunk lädt nicht → io.open läuft nie (kein Log). Immer `bit.band` etc.
2. **Disc-Pfad mit Leerzeichen/Klammern** bricht `-iso` still → Hardlink auf `C:\tmp\mzd.bin`.
3. **Alte PCSX-Redux-Instanz** hält die Log-Datei offen → `os.remove` crasht; einfach mit Mode `"w"` truncaten und vorher `taskkill //F //IM pcsx-redux.exe`.
4. **vgamepad NACH PCSX-Redux erstellt** → GLFW sieht ihn nicht. Immer zuerst den Pad, dann den Emulator.
5. **Adresse→RAM-Offset**: `addr & 0x1fffff` (KSEG0/KUSEG-Mirror), nicht `addr`.
6. Statik ist trotzdem ZUERST dran (billiger): erst alle Stores/Base-Tracking scannen; den Watchpoint nur, wenn statisch nichts (indirekt/DMA).

## Werkzeuge (`scripts/`)
- `watch_addr.lua` — der Watchpoint (env: `PCSX_WATCH_ADDR`/`_LOG`/`_BITMASK`).
- `pcsx_drive.py` — vgamepad + PCSX-Redux-Start + New-Game-Navigation + Log-Überwachung.

Schwester-Skills: `re15-room-capture` (DuckStation-Savestates), `re15-savestate-ghidra` (RAM aus Savestate),
`re15-psx-disasm` (der statische Scan, der ZUERST läuft). Memory [[reai-v2-global-ai-freeze]].
