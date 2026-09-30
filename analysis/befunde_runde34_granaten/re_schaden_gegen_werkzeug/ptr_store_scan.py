#!/usr/bin/env python3
"""ptr_store_scan.py - Gegenpruefung Runde 34: sucht `lw rt,OFF(rs)` und danach (innerhalb N Instruktionen,
solange rt nicht ueberschrieben wird) Speicher-Instruktionen MIT rt als Basis (sb/sh/sw k(rt)).
Zweck: den Schreiber hinter einem Zeiger-Feld finden (z.B. +0x7c = Versatzvektor der Hitbox).

Aufruf: python ptr_store_scan.py 124 [N=24]
"""
import os, sys, struct

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R  # noqa: E402
sys.path.insert(0, HERE)
from zensus import images  # noqa: E402


def writes_reg(w):
    """Zielregister, das w beschreibt (oder None)."""
    op = w >> 26
    if w == 0:
        return None
    if op == 0:
        fn = w & 63
        if fn in (0x08, 0x18, 0x19, 0x1a, 0x1b):  # jr, mult*, div*
            return None
        if fn == 0x09:
            return (w >> 11) & 31
        return (w >> 11) & 31
    if op == 3:
        return 31
    if op in (2, 4, 5, 6, 7, 1):
        return None
    if op in (0x28, 0x29, 0x2b, 0x2a, 0x2e):
        return None
    if op == 0x12:  # cop2
        rs = (w >> 21) & 31
        if rs in (0, 2):  # mfc2/cfc2
            return (w >> 16) & 31
        return None
    if op in (0x32, 0x3a):  # lwc2/swc2
        return None
    return (w >> 16) & 31


def main():
    off = int(sys.argv[1], 0)
    N = int(sys.argv[2]) if len(sys.argv) > 2 else 24
    for name, buf, base in images():
        ws = [struct.unpack_from("<I", buf, o)[0] for o in range(0, len(buf) - 3, 4)]
        for i, w in enumerate(ws):
            if w >> 26 == 0x23 and (w & 0xffff) == (off & 0xffff):
                rt = (w >> 16) & 31
                if rt == 0:
                    continue
                hits = []
                for k in range(1, N + 1):
                    if i + k >= len(ws):
                        break
                    w2 = ws[i + k]
                    op2 = w2 >> 26
                    if op2 in (0x28, 0x29, 0x2b, 0x3a) and ((w2 >> 21) & 31) == rt:
                        a2 = base + 4 * (i + k)
                        s, _ = R.dis_one(w2, a2)
                        hits.append(f"@0x{a2:08x} {s}")
                    if writes_reg(w2) == rt:
                        break
                if hits:
                    a = base + 4 * i
                    s, _ = R.dis_one(w, a)
                    print(f"{name:10s} @0x{a:08x} {s:28s} -> " + " | ".join(hits))


if __name__ == "__main__":
    main()
