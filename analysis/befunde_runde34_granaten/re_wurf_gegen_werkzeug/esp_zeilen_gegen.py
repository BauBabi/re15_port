#!/usr/bin/env python3
"""Gegenpruefung R34: CORE00.ESP Effekt/sub -> Stromkopf, Zeilen (40 B), Anim-Saetze, Koordinaten.
Layout aus FUN_8001945c (@0x8001948c-0x800194ec) und FUN_80019700 (@0x8001970c-0x800198f8)."""
import struct, sys
d = open('re15_port/shared_assets/PSX/DATA/CORE00.ESP', 'rb').read()
ids = list(d[:8]); hdr = {}; off = len(d) - 4
for i in ids:
    if i == 0xff: break
    hdr[i] = struct.unpack_from('<I', d, off)[0]; off -= 4
def eff(cat, sub):
    h = hdr[cat]; ca, cb, clut, tp = struct.unpack_from('<HHHH', d, h)
    st = h + (2 * ca + cb + 2) * 4
    ent_off = st + (sub & 7) * 2
    ent = struct.unpack_from('<H', d, ent_off)[0]
    sh = st + ent * 4
    nstreams = struct.unpack_from('<H', d, sh)[0]
    print('eff %d sub %#x: hdr %#x ca %d cb %d CLUT %#06x+%#x=%#06x TPAGE %#06x; subtab %#x entry@%#x=%#x -> stream hdr %#x (%d streams) bytes %s' % (
        cat, sub, h, ca, cb, clut, (sub >> 3) * 0x40, clut + (sub >> 3) * 0x40, tp, st, ent_off, ent, sh, nstreams, d[sh:sh+8].hex(' ')))
    a = sh + 4
    for s in range(nstreams):
        nrows = struct.unpack_from('<H', d, a)[0]
        print('  stream %d @%#x rows %d' % (s, a, nrows))
        r = a + 4
        for k in range(nrows):
            row = d[r:r + 40]
            f = struct.unpack_from('<20h', row)
            print('   row %d @%#x: %s' % (k, r, row.hex(' ')))
            print('      A=%d B=%d w=%#x h=%#x acc=(%d,%d,%d) +0e=%#x vel=(%d,%d,%d) +16=%#x angv=(%d,%d,%d) +1e=%d euler=(%d,%d,%d) +26=%d' % (
                f[0] & 0xffff, f[1] & 0xffff, f[2] & 0xffff, f[3] & 0xffff, f[4], f[5], f[6], f[7] & 0xffff, f[8], f[9], f[10], f[11] & 0xffff, f[12], f[13], f[14], f[15], f[16], f[17], f[18], f[19]))
            r += 40
        a += 4 + nrows * 40
    return h, ca
def anims(cat, lo, hi):
    h = hdr[cat]; ca, cb = struct.unpack_from('<HH', d, h)
    for k in range(lo, hi + 1):
        o = h + 8 + k * 8
        print('  satz %2d @%#x: %s' % (k, o, d[o:o + 8].hex(' ')))
def coords(cat, lo, hi):
    h = hdr[cat]; ca, cb = struct.unpack_from('<HH', d, h)
    base = h + 8 + ca * 8
    for k in range(lo, hi + 1):
        o = base + k * 4
        u, v, dx, dy = d[o], d[o + 1], struct.unpack_from('<b', d, o + 2)[0], struct.unpack_from('<b', d, o + 3)[0]
        print('  koord %2d @%#x: u %d v %d dx %d dy %d' % (k, o, u, v, dx, dy))
if __name__ == '__main__':
    for cat, sub in ((4, 0x0d), (3, 0x19), (3, 0x0b)):
        eff(cat, sub)
    print('Effekt 4 Anim-Saetze 0..1, 20..36:'); anims(4, 0, 1); anims(4, 20, 35)
    print('Effekt 4 Koord 16..23:'); coords(4, 16, 23)
    print('Effekt 3 Anim-Saetze 0, 7..24:'); anims(3, 0, 0); anims(3, 7, 24)
