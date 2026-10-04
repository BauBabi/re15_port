#!/usr/bin/env python3
"""em_zensus.py - Runde 35 Spur C: Zensus aller Sce_em_set (0x44) in ALLEN RDTs (alle Stages,
main + alle subs), opcode-exakt ueber scd_walk_lib (keine Rohbyte-Suche).

Layout Sce_em_set (engine/src/scd_vm.c op_sce_em_set): pc[1]=slot pc[2]=type pc[3]=behavior
pc[7]=kill-flag-index pc[8..13]=x,y,z (LE s16) pc[16..17]=dir (LE).
Aufruf: python em_zensus.py [typ_hex]   (ohne Typ: Typen-Histogramm)
"""
import os, sys, glob, struct
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import scd_walk_lib as L

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "shared_assets", "PSX")
want = int(sys.argv[1], 16) if len(sys.argv) > 1 else None
hist = {}
for p in sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    room = os.path.basename(p)[4:8]
    try:
        regs = L.regionen(d)
    except Exception as e:
        print("FEHLER", room, e); continue
    for (tag, idx), ops in sorted(regs.items()):
        for pc, op, sz in ops:
            if op != 0x44:
                continue
            slot, typ, beh, kf = d[pc+1], d[pc+2], d[pc+3], d[pc+7]
            x, y, z = struct.unpack_from("<hhh", d, pc+8)
            di = struct.unpack_from("<h", d, pc+16)[0]
            hist.setdefault(typ, set()).add(room)
            if want is not None and typ == want:
                print("ROOM%s %s%02d @0x%05x slot=%d type=0x%02x beh=0x%02x killflag=0x%02x pos=(%d,%d,%d) dir=%d bytes=%s"
                      % (room, tag, idx, pc, slot, typ, beh, kf, x, y, z, di, d[pc:pc+sz].hex(" ")))
if want is None:
    for t in sorted(hist):
        print("type 0x%02x: %d Raeume: %s" % (t, len(hist[t]), " ".join(sorted(hist[t]))))
