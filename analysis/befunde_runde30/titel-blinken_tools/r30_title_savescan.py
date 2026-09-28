#!/usr/bin/env python3
"""r30_title_savescan.py - Runde 30 / Thema D (Titelmenue blinkt zu schnell).

Liest aus JEDEM DuckStation-Savestate unter stage_saves/ den Zustand des Titel-
Tasks (TITLE.BIN @0x80100000) und der Bildtaktung:
  - ist TITLE.BIN geladen?  (Bytes @0x801028ec..0x80102944 == Datei TITLE.BIN)
  - DAT_800b5456   = Argument von VSync() im Flip FUN_8002137c @0x80021388
  - 0x80102944 u16 = Pulswert,  0x80102946 u16 = Pulszaehler (FUN_801028ec)
  - 0x801026c4..cb = Titel-State / Sub-State / Cursor
  - Task-Slots @0x800b2924 (Stride 0x80): state u16, wait s16, entry u32
  - VSync-Zaehler der libetc (Vcount), sofern auffindbar
Nur lesend. Ausgabe: Tabelle auf stdout.
"""
import sys, os, glob, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
import re15_ss

TITLE = open(r"C:\workspace\git\reAi_v2\info\Re1.5\PSX\BIN\TITLE.BIN", "rb").read()
EXE   = open(r"C:\workspace\git\reAi_v2\info\Re1.5\PSX.EXE", "rb").read()

def title_loaded(ram):
    a0, a1 = 0x801027a0, 0x80102944          # Zeichner + Puls-Handler (reiner Code)
    return ram.bytes(a0, a1 - a0) == TITLE[a0 - 0x80100000:a1 - 0x80100000]

def patched(ram):
    return ram.bytes(0x80026e4c, 4) == bytes([0x24, 0xc2, 0x01, 0x08])

def main():
    pats = sys.argv[1:] or [r"C:\workspace\git\reAi_v2\stage_saves\*.sav",
                            r"C:\workspace\git\reAi_v2\stage_saves\*\*.sav"]
    files = []
    for p in pats: files += glob.glob(p)
    print("%-44s %-5s %-5s %-6s %-6s %-5s %-24s %s" % (
        "savestate", "TITLE", "vsArg", "pulse", "ctr", "patch", "state c4..cb", "tasks(state,wait,entry)"))
    for f in sorted(files):
        try:
            ram = re15_ss.Ram(f)
        except Exception as e:
            print("%-44s FEHLER %s" % (os.path.basename(f), e)); continue
        tl = title_loaded(ram)
        st = ram.bytes(0x801026c4, 8).hex() if tl else "-"
        tasks = []
        for i in range(3):
            b = 0x800b2924 + i * 0x80
            tasks.append("%x,%d,%08x" % (ram.u16(b), ram.s16(b + 2), ram.u32(b + 4)))
        print("%-44s %-5s %-5d %-6s %-6s %-5s %-24s %s" % (
            os.path.relpath(f, r"C:\workspace\git\reAi_v2\stage_saves"),
            "JA" if tl else "nein", ram.u8(0x800b5456),
            ("0x%02x" % ram.u16(0x80102944)) if tl else "-",
            ("%d" % ram.u16(0x80102946)) if tl else "-",
            "JA" if patched(ram) else "nein", st, " | ".join(tasks)))

if __name__ == "__main__":
    main()
