#!/usr/bin/env python3
"""Gegenpruefung R34: liest aus sauberen Savestates (a) die Dispatch-Tabelle 0x80072bac
Eintraege 0..0x50, (b) die Spawn-Voreinstellung 0x80072be0 (12 Byte), (c) je aktivem
Gegner-Slot (0x800acc2c + i*500) Typ/+0x4/+0x78/+0x93/HP und den Kasten hinter +0x78."""
import sys, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import Ram

def rd(ram, a, n):
    o = ram.base + (a - 0x80000000)
    return ram.blob[o:o+n]

for p in sys.argv[1:]:
    ram = Ram(p)
    print("==", p)
    exe_sig = rd(ram, 0x80026e4c, 4).hex()
    print("  @0x80026e4c =", exe_sig, "(sauber = 0800e003)")
    box = rd(ram, 0x80072be0, 12)
    print("  0x80072be0 =", box.hex(' '), struct.unpack("<6h", box))
    tab = struct.unpack("<80I", rd(ram, 0x80072bac, 320))
    print("  tab[0x0c..0x10] =", [hex(x) for x in tab[0x0c:0x11]])
    nz = [(hex(i), hex(v)) for i, v in enumerate(tab) if v and 0x10 <= i]
    print("  registriert (>=0x10):", nz)
    cnt = rd(ram, 0x800aca4e, 1)[0]
    print("  g_active_count =", cnt)
    for i in range(16):
        b = 0x800acc2c + i*500
        w0 = struct.unpack("<I", rd(ram, b, 4))[0]
        if not (w0 & 1):
            continue
        typ = rd(ram, b+8, 1)[0]
        st = rd(ram, b+4, 4).hex(' ')
        p78 = struct.unpack("<I", rd(ram, b+0x78, 4))[0]
        h93 = rd(ram, b+0x93, 1)[0]
        hp = struct.unpack("<h", rd(ram, b+0x9a, 2))[0]
        bx = struct.unpack("<6h", rd(ram, p78, 12)) if 0x80000000 <= p78 < 0x80200000 else None
        print("   slot %2d w0=%08x typ=0x%02x st=%s +0x78=%08x box=%s +0x93=%02x hp=%d" % (i, w0, typ, st, p78, bx, h93, hp))
