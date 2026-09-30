#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: alle RE2-Raum-ESP-Bloecke (RDT+0x50 Id-Liste, RDT+0x54 Ende der
Offset-Tabelle; Registrierung FUN_8001bca0 laut analysis/konstruktion_2026-08-23/re2-fx-system.md
§1b) der Leon-RDTs nach Skript-Steps mit bestimmten Opcodes durchsuchen (Default: 15 = GL-Flug
FUN_8001ed9c, 40 = Nachbrenner FUN_80020758, 47/48/49 = Explosion/Brand/Saeure).

Aufruf: re2_rdt_esp_ops.py [--ops 15,40,47,48,49] [--rdt-dir info/re2leon/PL0/RDT]
"""
import os, sys, struct, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))


def u16(d, o): return struct.unpack_from('<H', d, o)[0]


def scan_block(d, idh, ptr_end, ops, tag, out):
    ids = []
    for i in range(8):
        b = d[idh + i]
        if b == 0xFF:
            break
        ids.append(b)
    for i, bid in enumerate(ids):
        ent = struct.unpack_from('<i', d, ptr_end - 4 * i)[0]
        off = idh + ent
        if off < 0 or off + 8 > len(d):
            continue
        n1, n2 = u16(d, off), u16(d, off + 2)
        st = off + 8 + 8 * n1 + 4 * n2
        if st + 16 > len(d):
            continue
        tab = [u16(d, st + 2 * k) for k in range(8)]
        for k in range(8):
            if tab[k] == 0:
                continue
            p = st + tab[k] * 4
            if p + 4 > len(d):
                continue
            nparts = u16(d, p)
            if nparts > 32:
                continue
            q = p + 4
            for part in range(nparts):
                if q + 4 > len(d):
                    break
                ns = u16(d, q)
                if ns > 64:
                    break
                q += 4
                for s in range(ns):
                    b = d[q:q + 24]
                    if len(b) < 24:
                        break
                    if b[0] in ops or b[1] in ops:
                        out.append('%s bank 0x%02X skript %d part %d step %d @0x%X: OpA=%d OpB=%d s2=%d s3=%d  %s'
                                   % (tag, bid, k, part, s, q, b[0], b[1], b[2], b[3], b.hex()))
                    q += 24


def main():
    ops = {15, 40, 47, 48, 49}
    rdir = os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT')
    for a in sys.argv[1:]:
        if a.startswith('--ops='):
            ops = set(int(x, 0) for x in a.split('=', 1)[1].split(','))
        if a.startswith('--rdt-dir='):
            rdir = a.split('=', 1)[1]
    out = []
    n = 0
    for p in sorted(glob.glob(os.path.join(rdir, '*.RDT'))):
        d = open(p, 'rb').read()
        if len(d) < 0x60:
            continue
        idh = struct.unpack_from('<I', d, 0x50)[0]
        pend = struct.unpack_from('<I', d, 0x54)[0]
        if idh == 0 or idh >= len(d) or pend >= len(d):
            continue
        n += 1
        scan_block(d, idh, pend, ops, os.path.basename(p), out)
    print('; %d RDTs mit ESP-Block, Opcodes %s' % (n, sorted(ops)))
    for l in out:
        print(l)
    print('; %d Treffer' % len(out))


if __name__ == '__main__':
    main()
