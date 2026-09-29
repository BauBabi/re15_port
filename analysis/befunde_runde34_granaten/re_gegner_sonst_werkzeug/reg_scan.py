#!/usr/bin/env python3
"""reg_scan.py — Registrierungs-Scanner: sucht in jedem STAGE*.BIN (RAW @0x80100000, kein Header)
`sw rt, imm(base)` mit Zieladresse im Bereich 0x80072bac..0x80072bac+0x100*4 (Typ-Tick-Tabelle,
EXE-Dispatch @0x80072bac, Aufrufer FUN_8001a50c) und rekonstruiert den Wert von rt ueber
lui/addiu/ori-Registerverfolgung im selben Basisblock-Fenster (32 Instruktionen rueckwaerts).
Ausgabe: Stage, Adresse der sw-Instruktion, Typ, Handler.
"""
import struct, os, glob, sys
ROOT = os.path.join(os.path.dirname(__file__), '..', '..', '..')
BIN = os.path.join(ROOT, 'info', 'Re1.5', 'PSX', 'BIN')
TAB = 0x80072bac
def sx(v): return v - 0x10000 if v & 0x8000 else v
def scan(path):
    d = open(path, 'rb').read()
    n = len(d) // 4
    W = [struct.unpack_from('<I', d, i*4)[0] for i in range(n)]
    res = []
    for i, w in enumerate(W):
        op = w >> 26
        if op != 0x2b: continue
        base = (w >> 21) & 31; rt = (w >> 16) & 31; imm = sx(w & 0xffff)
        # Basisregister-Wert ermitteln (lui)
        regs = {}
        # rueckwaerts: suche lui base
        bval = None; rval = None
        for j in range(i-1, max(0, i-40), -1):
            wj = W[j]; opj = wj >> 26
            rs_ = (wj >> 21) & 31; rt_ = (wj >> 16) & 31; im = wj & 0xffff
            if bval is None and opj == 0x0f and rt_ == base:
                bval = (im << 16) & 0xffffffff
        if bval is None: continue
        addr = (bval + imm) & 0xffffffff
        if not (TAB <= addr < TAB + 0x100*4): continue
        typ = (addr - TAB) // 4
        # rt-Wert: lui rt + addiu rt,rt / ori
        hi = None; lo = 0
        for j in range(i-1, max(0, i-40), -1):
            wj = W[j]; opj = wj >> 26
            rs_ = (wj >> 21) & 31; rt_ = (wj >> 16) & 31; im = wj & 0xffff
            if opj == 0x09 and rt_ == rt and rs_ == rt and lo == 0:
                lo = sx(im)
            elif opj == 0x0d and rt_ == rt and rs_ == rt and lo == 0:
                lo = im
            elif opj == 0x0f and rt_ == rt:
                hi = im << 16; break
        val = ((hi or 0) + lo) & 0xffffffff if hi is not None else None
        res.append((0x80100000 + i*4, typ, val))
    return res
for p in sorted(glob.glob(os.path.join(BIN, 'STAGE*.BIN'))):
    name = os.path.basename(p)
    for a, t, v in scan(p):
        print(f'{name:11s} sw@0x{a:08x}  Typ 0x{t:02x}  -> {("0x%08x" % v) if v is not None else "?"}')
