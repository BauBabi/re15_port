#!/usr/bin/env python3
"""global_leser_scan.py - Runde 34, re_gegner_re2_familie.
Sucht in RE2-Binaerdateien (EXE + Overlays roh @0x80100000) Zugriffe auf eine globale Adresse
ueber das Paar `lui rX,hi` + `lw/lh/lhu/lb/lbu/sw/sh/sb rY,lo(rX)` (bis 8 Instruktionen Abstand,
gleiches Basisregister). Aufruf: python global_leser_scan.py 0x800cfb88,0x800cfb8c <datei>...
"""
import sys, struct, re, os
targets = [int(x, 16) for x in sys.argv[1].split(",")]
for path in sys.argv[2:]:
    data = open(path, "rb").read()
    if path.upper().endswith("PSX.EXE"):
        taddr = struct.unpack_from("<I", data, 0x18)[0]; base = taddr; off0 = 0x800
    else:
        base = 0x80100000; off0 = 0
    n = (len(data) - off0) // 4
    words = [struct.unpack_from("<I", data, off0 + i * 4)[0] for i in range(n)]
    hits = 0
    for i, w in enumerate(words):
        if (w >> 26) != 0x0F:            # lui
            continue
        rt = (w >> 16) & 31; hi = w & 0xFFFF
        for k in range(1, 9):
            if i + k >= n: break
            v = words[i + k]; op = v >> 26
            if op in (0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B) and ((v >> 21) & 31) == rt:
                lo = v & 0xFFFF; lo = lo - 0x10000 if lo & 0x8000 else lo
                ea = ((hi << 16) + lo) & 0xFFFFFFFF
                if ea in targets:
                    kind = {0x20:"lb",0x21:"lh",0x23:"lw",0x24:"lbu",0x25:"lhu",0x28:"sb",0x29:"sh",0x2B:"sw"}[op]
                    print("%s: %08x %s -> %08x" % (os.path.basename(path), base + (i + k) * 4, kind, ea))
                    hits += 1
            # Basisregister ueberschrieben? (grob: rt als Ziel eines anderen lui)
            if (v >> 26) == 0x0F and ((v >> 16) & 31) == rt: break
    print("# %s: %d Zugriffe" % (os.path.basename(path), hits))
