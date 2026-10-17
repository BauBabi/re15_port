"""cursor_md1.py - Runde 38 Spur A: Flaechen eines Cursor-MD1 (RDT-Prop) mit Ebene, UV-Bereich und Texel-Inhalt.
Layout wie include/re15_md1.h (Kopf len/unk/nobj, je Mesh 14 u32: tri v/n/f/uv, quad v/n/f/uv; Offsets ab MD1+12;
Dreieck n0 v0 n1 v1 n2 v2, Viereck n0 v0 n1 v1 n2 v2 n3 v3; UV 12/16 Byte).
Aufruf: python cursor_md1.py <rdt> <md1_off> <tim_off>"""
import struct, sys
from collections import Counter


def tim_pixels(d, o):
    flags = struct.unpack_from('<I', d, o + 4)[0]
    cl_len = struct.unpack_from('<I', d, o + 8)[0]
    po = o + 8 + cl_len
    pl, px, py, pw, ph = struct.unpack_from('<IHHHH', d, po)
    return d[po + 12:po + 12 + pw * 2 * ph], pw * 2, ph


def main():
    d = open(sys.argv[1], 'rb').read()
    m = int(sys.argv[2], 0)
    t = int(sys.argv[3], 0)
    pix, W, H = tim_pixels(d, t)
    ln, unk, nobj = struct.unpack_from('<III', d, m)
    b = m + 12
    for mi in range(nobj // 2):
        h = struct.unpack_from('<14I', d, b + mi * 56)
        tv, tvn, tn, tnn, tf, tfn, tuv, qv, qvn, qn, qnn, qf, qfn, quv = h
        V = [struct.unpack_from('<hhh', d, b + tv + 8 * i) for i in range(tvn)]
        QV = [struct.unpack_from('<hhh', d, b + qv + 8 * i) for i in range(qvn)]

        def ebene(pts):
            for ax, name in ((0, 'x'), (1, 'y'), (2, 'z')):
                if len(set(p[ax] for p in pts)) == 1:
                    return '%s=%d' % (name, pts[0][ax])
            return 'schief'

        def texel(uvs):
            us = [u for u, v in uvs]; vs = [v for u, v in uvs]
            c = Counter()
            for v in range(min(vs), max(vs) + 1):
                for u in range(min(us), max(us) + 1):
                    if 0 <= v < H and 0 <= u < W:
                        c[pix[v * W + u]] += 1
            nz = sum(n for k, n in c.items() if k != 0)
            return 'u %d..%d v %d..%d, Texel!=0 im Rechteck: %d' % (min(us), max(us), min(vs), max(vs), nz)

        print('Mesh %d: %d Dreiecke, %d Vierecke' % (mi, tfn, qfn))
        grp = Counter()
        for i in range(tfn):
            n0, v0, n1, v1, n2, v2 = struct.unpack_from('<6H', d, b + tf + 12 * i)
            u0, vv0, clut, u1, vv1, page, u2, vv2 = struct.unpack_from('<BBHBBHBB', d, b + tuv + 12 * i)
            e = ebene([V[v0], V[v1], V[v2]])
            print('  D%02d %-8s %s' % (i, e, texel([(u0, vv0), (u1, vv1), (u2, vv2)])))
        for i in range(qfn):
            n0, v0, n1, v1, n2, v2, n3, v3 = struct.unpack_from('<8H', d, b + qf + 16 * i)
            u0, vv0, clut, u1, vv1, page, u2, vv2, pa, u3, vv3, pb = struct.unpack_from('<BBHBBHBBHBBH', d, b + quv + 16 * i)
            e = ebene([QV[v0], QV[v1], QV[v2], QV[v3]])
            tx = texel([(u0, vv0), (u1, vv1), (u2, vv2), (u3, vv3)])
            grp[(e, 'leer' if tx.endswith(': 0') else 'Inhalt')] += 1
        for k, n in sorted(grp.items()):
            print('  Vierecke %-8s %-6s : %d' % (k[0], k[1], n))


if __name__ == '__main__':
    main()
