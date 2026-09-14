#!/usr/bin/env python3
"""Findet ALLE Message_on-Stellen eines RDT — mit korrekter Blockgrenze.

Blockgrenze eines Sub-Eintrags = kleinster Wert groesser als der Start aus
  (a) allen main/sub/extra-Eintraegen,
  (b) allen Basisadressen der drei Bloecke,
  (c) allen 24 Zeigern der RDT-Adresstabelle (0x08..0x5c, rdt_common.c).
Damit laeuft der lineare Scan nie in den naechsten Block hinein.
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scd_zeit as SZ


def grenzen(r):
    pts = set()
    for blk in (r.main, r.sub, r.extra):
        pts.update(blk)
    for hdr in range(0x08, 0x60, 4):
        p = SZ.u32(r.d, hdr)
        if 0 < p <= len(r.d):
            pts.add(p)
    pts.add(len(r.d))
    return sorted(pts)


def block_ende(r, start, pts):
    for p in pts:
        if p > start:
            return p
    return len(r.d)


def alle_stellen(r):
    """-> Liste (blockname, index, pc, msg_id, mask)."""
    pts = grenzen(r)
    out = []
    for bname, blk in (('main', r.main), ('sub', r.sub), ('extra', r.extra)):
        for i, start in enumerate(blk):
            end = block_ende(r, start, pts)
            pc = start
            guard = 0
            while pc < end and guard < 20000:
                guard += 1
                if pc + 1 > len(r.d):
                    break
                op = r.d[pc]
                n = SZ.oplen(r.d, pc)
                if op == 0x2B and pc + 4 <= len(r.d):
                    out.append((bname, i, pc, r.d[pc + 1], SZ.u16(r.d, pc + 2)))
                if n <= 0:
                    break
                pc += n
    return out


def dump(r, bname, idx, pts=None):
    """Lineare Disassembly eines Blocks — zur Kontrolle."""
    pts = pts or grenzen(r)
    blk = {'main': r.main, 'sub': r.sub, 'extra': r.extra}[bname]
    start = blk[idx]
    end = block_ende(r, start, pts)
    pc = start
    lines = []
    while pc < end:
        op = r.d[pc]
        n = SZ.oplen(r.d, pc)
        hx = ' '.join('%02x' % c for c in r.d[pc:pc + min(n, 16)])
        extra = ''
        if op == 0x2B:
            extra = ' msg=%d mask=%04x' % (r.d[pc + 1], SZ.u16(r.d, pc + 2))
        elif op == 0x09:
            extra = ' %d Bilder' % SZ.u16(r.d, pc + 2)
        elif op == 0x18:
            extra = ' sub%d' % r.d[pc + 1]
        elif op == 0x04:
            extra = ' -> sub%d' % r.d[pc + 3]
        elif op == 0x17:
            extra = ' rel=%d -> 0x%04X' % (SZ.s16(r.d, pc + 4), pc + SZ.s16(r.d, pc + 4))
        elif op == 0x06:
            extra = ' len=%d -> falsch@0x%04X' % (SZ.u16(r.d, pc + 2), pc + 4 + SZ.u16(r.d, pc + 2))
        lines.append('0x%04X  %-16s%-28s | %s' % (pc, SZ.NAMES.get(op, 'OP%02X' % op), extra, hx))
        if n <= 0:
            break
        pc += n
    return lines


if __name__ == '__main__':
    r = SZ.Rdt(sys.argv[1])
    if len(sys.argv) > 3:
        for l in dump(r, sys.argv[2], int(sys.argv[3])):
            print(l)
    else:
        print('main-Eintraege: %s' % ['0x%04X' % x for x in r.main])
        print('sub-Eintraege : %s' % ['0x%04X' % x for x in r.sub])
        print('extra         : %s' % ['0x%04X' % x for x in r.extra])
        for s in alle_stellen(r):
            print('%s%02d @0x%04X  msg=%2d mask=%04x' % (s[0], s[1], s[2], s[3], s[4]))
