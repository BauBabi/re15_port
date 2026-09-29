#!/usr/bin/env python3
"""Runde 34 (Granaten) - Port-Inventar: alle Lade-/Speicherbefehle auf 0x800b5358 (Latch aus Routine 9/31).

Scannt info/Re1.5/PSX.EXE (PS-X EXE: 0x800 Kopf, t_addr aus [0x18], Groesse [0x1c]) und optional
STAGE*.BIN (roh ab 0x80100000, Memory reai-v2-re-pitfalls: STAGE*.BIN ohne Kopf) nach MIPS-Load/Store
(lb/lbu/lh/lhu/lw/sb/sh/sw) mit 16-bit-Offset 0x5358 bzw. 0x5359 (signed: 21336/21337), deren Basisregister
in den 8 Befehlen davor per `lui rX,0x800b` gesetzt wurde. Nur Ortsbestimmung, keine Semantik.
"""
import os
import struct
import sys

ROOT = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'info', 'Re1.5')
OPS = {0x20: 'lb', 0x24: 'lbu', 0x21: 'lh', 0x25: 'lhu', 0x23: 'lw', 0x28: 'sb', 0x29: 'sh', 0x2b: 'sw'}
REG = ['zero', 'at', 'v0', 'v1', 'a0', 'a1', 'a2', 'a3', 't0', 't1', 't2', 't3', 't4', 't5', 't6', 't7',
       's0', 's1', 's2', 's3', 's4', 's5', 's6', 's7', 't8', 't9', 'k0', 'k1', 'gp', 'sp', 'fp', 'ra']


def scan(code, base, name):
    n = len(code) // 4
    words = struct.unpack_from('<%dI' % n, code, 0)
    hits = []
    for i, w in enumerate(words):
        op = w >> 26
        if op not in OPS:
            continue
        imm = w & 0xffff
        if imm not in (0x5358, 0x5359):
            continue
        rs = (w >> 21) & 31
        rt = (w >> 16) & 31
        ok = False
        for k in range(max(0, i - 8), i):
            p = words[k]
            if (p >> 26) == 0x0f and ((p >> 16) & 31) == rs and (p & 0xffff) == 0x800b:
                ok = True
        if ok:
            hits.append((base + 4 * i, OPS[op], REG[rt], imm, REG[rs]))
    for a, o, rt, imm, rs in hits:
        print(f'{name} @0x{a:08x}: {o} {rt},{imm}({rs})  -> 0x{0x800b0000 + imm:08x}')
    return hits


def main():
    exe = open(os.path.join(ROOT, 'PSX.EXE'), 'rb').read()
    t_addr, t_size = struct.unpack_from('<II', exe, 0x18)
    scan(exe[0x800:0x800 + t_size], t_addr, 'PSX.EXE')
    for st in sys.argv[1:] or ['STAGE1.BIN', 'STAGE2.BIN', 'STAGE3.BIN', 'STAGE4.BIN', 'STAGE5.BIN',
                               'STAGE6.BIN']:
        p = os.path.join(ROOT, 'PSX', 'BIN', st)
        if os.path.exists(p):
            scan(open(p, 'rb').read(), 0x80100000, st)


if __name__ == '__main__':
    main()
