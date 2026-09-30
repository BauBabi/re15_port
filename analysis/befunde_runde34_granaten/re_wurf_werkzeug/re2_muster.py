#!/usr/bin/env python3
"""Runde 34 / Granate: Gibt es die RE1.5-Granaten-Routinen 29/30/31 auch in RE2 Retail?
Sucht die kennzeichnenden Instruktionsworte (exakte 32-Bit-Muster) in RE1.5 PSX.EXE und in
RE2 info/re2leon/PSX.EXE + allen info/re2leon/COMMON/BIN/*.BIN.

Muster (aus RE1.5 Routine 30/29/31, Dossier §1):
  0x3402017c  ori v0,zero,0x17c   (R30 HOCH vx=380  @0x80018494)
  0x34020118  ori v0,zero,0x118   (R30 MITTE vx=280 @0x800184bc)
  0x2402ff92  addiu v0,zero,-110  (R30 HOCH vy      @0x8001849c)
  0x2402ffce  addiu v0,zero,-50   (R30 MITTE vy     @0x800184c4; auch sonst haeufig)
  0x3c04010a  lui a0,0x10a        (R29 SE-Bank1-Satz 0x0A @0x80018350)
  0x3c040408  lui a0,0x408        (R31 SE-Bank4-Satz 8    @0x800185e4)
  0x3c040319  lui a0,0x319        (R31 Kind 0x03195000    @0x800185c0)
  0x3c04040d  lui a0,0x40d        (FSM-Spawn 0x040Dxxxx   @0x800336bc)
Aufruf: python re2_muster.py
"""
import os, struct

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
PATS = {0x3402017c: "ori v0,zero,0x17c", 0x34020118: "ori v0,zero,0x118",
        0x2402ff92: "addiu v0,zero,-110", 0x2402ffce: "addiu v0,zero,-50",
        0x3c04010a: "lui a0,0x10a", 0x3c040408: "lui a0,0x408",
        0x3c040319: "lui a0,0x319", 0x3c04040d: "lui a0,0x40d"}

def scan(path):
    b = open(path, "rb").read()
    if os.path.basename(path).upper() == "PSX.EXE":
        base = struct.unpack_from("<I", b, 0x18)[0]; data = b[0x800:]
    else:
        base = 0x80100000; data = b
    n = len(data) // 4
    w = struct.unpack_from("<%dI" % n, data, 0)
    return [(base + 4 * i, PATS[x]) for i, x in enumerate(w) if x in PATS]

def main():
    files = [os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), os.path.join(REPO, "info", "re2leon", "PSX.EXE")]
    d = os.path.join(REPO, "info", "re2leon", "COMMON", "BIN")
    files += [os.path.join(d, f) for f in sorted(os.listdir(d)) if f.upper().endswith(".BIN")]
    for f in files:
        h = scan(f)
        rel = os.path.relpath(f, REPO)
        if not h:
            print("%-40s keine Treffer" % rel); continue
        print(rel)
        for a, t in h:
            print("   %08x  %s" % (a, t))

if __name__ == "__main__":
    main()
