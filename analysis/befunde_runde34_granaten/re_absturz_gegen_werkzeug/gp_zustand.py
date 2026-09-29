#!/usr/bin/env python3
"""gp_zustand.py - Kurzbild eines Savestates (eigener Leser gp_ss): CPU (pc/EPC/RA), aktive ESP-Plaetze
(Pool 0x800a73b8, 96 x 0x84; aktiv = +0x6c & 1; A = +0, B = +2, Zuender +0x1e, Lage +0x28/2a/2c),
Gegner (0x800acc2c + i*500, aktiv = +0 & 1: Typ +8, +4..+7, +9, +0x93, HP +0x9a, Lage +0x34/+0x3c),
Spieler 0x800aca54 (+4..+7, HP +0x9a), aktueller Aktor 0x800ac784.
Aufruf: gp_zustand.py SAV [SAV ...]
"""
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gp_ss import SS


def kurz(p):
    s = SS(p)
    epc = s.c["EPC"]; ra = s.reg("ra")
    out = ["== %s" % os.path.relpath(p)]
    out.append("   EPC=%08x pc=%08x RA=%08x v0=%08x CAUSE=%08x k0=%08x" % (epc, s.c["pc"], ra, s.reg("v0"), s.c["CAUSE"], s.reg("k0")))
    esp = []
    for i in range(96):
        a = 0x800a73b8 + i * 0x84
        fl = s.u8(a + 0x6c)
        if fl & 1:
            esp.append("[%d] A=%d B=%d Z=%d fl=%02x (%d,%d,%d)" % (i, s.u16(a), s.u16(a + 2), s.u16(a + 0x1e), fl,
                                                                 s.s16(a + 0x28), s.s16(a + 0x2a), s.s16(a + 0x2c)))
    out.append("   ESP aktiv: " + ("; ".join(esp) if esp else "keine"))
    for i in range(8):
        a = 0x800acc2c + i * 500
        if s.u32(a) & 1:
            x = struct.unpack("<i", s.mem(a + 0x34, 4))[0]; z = struct.unpack("<i", s.mem(a + 0x3c, 4))[0]
            out.append("   G%d @%08x typ %02x +4..+7 %s +9 %02x +93 %02x HP %d (%d,%d)" % (
                i, a, s.u8(a + 8), s.mem(a + 4, 4).hex(" "), s.u8(a + 9), s.u8(a + 0x93), s.s16(a + 0x9a), x, z))
    pl = 0x800aca54
    out.append("   Spieler +4..+7 %s HP %d  Aktor %08x" % (s.mem(pl + 4, 4).hex(" "), s.s16(pl + 0x9a), s.u32(0x800ac784)))
    return "\n".join(out)


if __name__ == "__main__":
    for p in sys.argv[1:]:
        print(kurz(p))
