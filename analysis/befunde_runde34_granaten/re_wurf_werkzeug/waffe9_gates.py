#!/usr/bin/env python3
"""Runde 34 / Granate: jede Stelle in PSX.EXE (+ DEBUG.BIN, STAGE*.BIN), die die ausgeruestete
Waffen-Id (u8 @0x800aca5d, Spieler +0x09) laedt und innerhalb der naechsten N Instruktionen mit
einer Konstanten vergleicht, die Id 9 betrifft (ori/addiu/slti/sltiu mit 9/10 oder addiu -9).
Zweck: pruefen, ob irgendwo Id-9-spezifischer Code die Granate in der Hand ausblendet.

Aufruf: python waffe9_gates.py [N]
"""
import os, sys, importlib.util
HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("x", os.path.join(HERE, "xref_global.py"))
X = importlib.util.module_from_spec(spec); spec.loader.exec_module(X)
d = X.d
import struct

N = int(sys.argv[1]) if len(sys.argv) > 1 else 8
hits = X.scan(0x800aca5d)
for name, a, txt, luiat in hits:
    if not txt.startswith(("lbu", "lb")):
        continue
    img = [i for i in X.images() if i[0] == name][0]
    data, base = img[1], img[2]
    rt = None
    w = struct.unpack_from("<I", data, a - base)[0]
    rt = (w >> 16) & 31
    out = []
    for k in range(1, N + 1):
        aa = a + 4 * k
        if aa - base + 4 > len(data):
            break
        ww = struct.unpack_from("<I", data, aa - base)[0]
        t, _ = d.dis_one(ww, aa)
        op = ww >> 26
        imm = ww & 0xffff
        simm = imm - 0x10000 if imm & 0x8000 else imm
        if op in (0x0d, 0x09, 0x0a, 0x0b) and (simm in (9, 10, 11) or (op == 0x09 and simm == -9)):
            out.append("%08x: %s" % (aa, t))
    if out:
        print("%-10s %08x: %-26s | %s" % (name, a, txt, " ; ".join(out)))
