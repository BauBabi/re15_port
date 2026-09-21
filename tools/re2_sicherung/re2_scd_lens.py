#!/usr/bin/env python3
"""re2_scd_lens.py - die RE2-SCD-Opcode-Laengen DIREKT aus den Handlern ziehen.

Die RE2-SCD-Dispatch-Tabelle liegt @0x800A74C8 (143 Eintraege, Index = Opcode-Byte);
belegt in analysis/re2_ermittlung_2026-08-30/STRAENGE_ROH.md:725 und hier nachgeprueft.

Jeder Handler bekommt in a0 den Task-Zeiger und fuehrt den Skript-PC ueber
    lw   s0,28(a0)        ; PC laden
    addiu vX,s0,N         ; PC + Recordlaenge
    sw   vX,28(a0)        ; PC zurueckschreiben
fort. Dieses Muster wird hier gesucht; N ist die Recordlaenge.
"""
import struct, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
D = open(EXE, "rb").read()
TADDR = struct.unpack_from("<I", D, 0x18)[0]
TSIZE = struct.unpack_from("<I", D, 0x1c)[0]

DISPATCH = 0x800A74C8
N_OPS = 143


def fo(a):
    return 0x800 + (a - TADDR)


def valid_code(a):
    return TADDR <= a < TADDR + TSIZE


def handler_len(addr, maxn=400):
    """Recordlaenge aus dem 'addiu rX,rPC,N / sw rX,28(a0)'-Muster."""
    if not valid_code(addr):
        return None, "kein Code"
    # 1) Register finden, das den PC haelt (lw rPC,28(a0) oder lw rPC,28(sX))
    pcregs = set()
    cand = []
    a = addr
    for _ in range(maxn):
        if not valid_code(a):
            break
        w = struct.unpack_from("<I", D, fo(a))[0]
        op = w >> 26
        rs = (w >> 21) & 31
        rt = (w >> 16) & 31
        imm = w & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        if op == 0x23 and imm == 28:                    # lw rt,28(rs)
            pcregs.add(rt)
        elif op == 0x09 and rs in pcregs and 0 < simm < 256:   # addiu rt,rPC,N
            cand.append((rt, simm, a))
        elif op == 0x2B and imm == 28:                  # sw rt,28(rs)
            for (r, n, aa) in reversed(cand):
                if r == rt:
                    return n, "0x%08X addiu +%d / 0x%08X sw 28" % (aa, n, a)
        if op == 0 and (w & 0x3F) == 0x08 and rs == 31:  # jr ra
            break
        a += 4
    return None, "kein PC-Vorschub gefunden"


def table():
    out = {}
    for i in range(N_OPS):
        h = struct.unpack_from("<I", D, fo(DISPATCH + i * 4))[0]
        n, why = handler_len(h)
        out[i] = (h, n, why)
    return out


if __name__ == "__main__":
    t = table()
    for i in range(N_OPS):
        h, n, why = t[i]
        print("0x%02X  handler 0x%08X  len=%s   %s" % (i, h, n if n else "?", why))
