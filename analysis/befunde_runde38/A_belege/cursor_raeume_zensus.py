"""cursor_raeume_zensus.py - Runde 38 Spur A: alle Cursor-Raeume (Zensus Runde 34 Nacht B_hebetisch.md §3.4 + Varianten):
Obj_model_set (0x2D, 34 Byte) Typ 4 mit Box-Halbmass (900,900,900) im ganzen RDT suchen (Byte-Muster), Modell = Eintrag
obj_id der Prop-Tabelle (RDT-Kopf @0x30 -> je 8 Byte TIM/MD1), Flaechen mit Muster (Texel != 0) je Ebene zaehlen."""
import struct, sys, os
from collections import Counter, defaultdict

WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_cursor/re15_port/shared_assets/PSX'
RAEUME = ['1080', '1081', '4020', '4021', '30E0', '30E1', '10D0', '10D1', '11E0', '11E1', '1230', '1231',
          '11F0', '11F1', '1100', '1101', '2060', '2061', '2040', '2041', '5050', '5051', '3050', '3051']


def pfad(r):
    return os.path.join(WT, 'STAGE%s' % r[0], 'ROOM%s.RDT' % r)


def tim_pix(d, o):
    cl_len = struct.unpack_from('<I', d, o + 8)[0]
    po = o + 8 + cl_len
    pl, px, py, pw, ph = struct.unpack_from('<IHHHH', d, po)
    return d[po + 12:po + 12 + pw * 2 * ph], pw * 2, ph


def flaechen(d, m, t):
    pix, W, H = tim_pix(d, t)
    ln, unk, nobj = struct.unpack_from('<III', d, m)
    b = m + 12
    erg = Counter()
    for mi in range(nobj // 2):
        h = struct.unpack_from('<14I', d, b + mi * 56)
        tv, tvn, tn, tnn, tf, tfn, tuv, qv, qvn, qn, qnn, qf, qfn, quv = h
        V = [struct.unpack_from('<hhh', d, b + tv + 8 * i) for i in range(tvn)]
        QV = [struct.unpack_from('<hhh', d, b + qv + 8 * i) for i in range(qvn)]

        def ebene(p):
            for ax, nm in ((0, 'x'), (1, 'y'), (2, 'z')):
                if len(set(q[ax] for q in p)) == 1:
                    return '%s=%d' % (nm, p[0][ax])
            return 'schief'

        def muster(uvs):
            us = [u for u, v in uvs]; vs = [v for u, v in uvs]
            return any(pix[v * W + u] for v in range(min(vs), max(vs) + 1) for u in range(min(us), max(us) + 1)
                       if 0 <= v < H and 0 <= u < W)

        for i in range(tfn):
            n0, v0, n1, v1, n2, v2 = struct.unpack_from('<6H', d, b + tf + 12 * i)
            u0, a0, c, u1, a1, pg, u2, a2 = struct.unpack_from('<BBHBBHBB', d, b + tuv + 12 * i)
            erg[('D', ebene([V[v0], V[v1], V[v2]]), muster([(u0, a0), (u1, a1), (u2, a2)]))] += 1
        for i in range(qfn):
            n0, v0, n1, v1, n2, v2, n3, v3 = struct.unpack_from('<8H', d, b + qf + 16 * i)
            u0, a0, c, u1, a1, pg, u2, a2, pa, u3, a3, pb = struct.unpack_from('<BBHBBHBBHBBH', d, b + quv + 16 * i)
            erg[('V', ebene([QV[v0], QV[v1], QV[v2], QV[v3]]), muster([(u0, a0), (u1, a1), (u2, a2), (u3, a3)]))] += 1
    return erg


def main():
    for r in RAEUME:
        d = open(pfad(r), 'rb').read()
        tab = struct.unpack_from('<I', d, 0x30)[0]
        n_om = d[2]
        treffer = []
        for o in range(len(d) - 34):
            if d[o] == 0x2D and d[o + 2] == 4 and struct.unpack_from('<hhh', d, o + 28) == (900, 900, 900):
                treffer.append((o, d[o + 1]))
        zeilen = []
        for o, oid in treffer:
            tim, md1 = struct.unpack_from('<II', d, tab + 8 * oid)
            f = flaechen(d, md1, tim)
            mit = sorted('%s %s:%d' % (k[0], k[1], n) for k, n in f.items() if k[2])
            zeilen.append('  @0x%05X obj %d (nOmodel %d) MD1 @0x%05X TIM @0x%05X  Muster-Flaechen: %s' % (
                o, oid, n_om, md1, tim, ', '.join(mit)))
        print('ROOM%s: %d Kandidat(en)' % (r, len(treffer)))
        print('\n'.join(zeilen))


if __name__ == '__main__':
    main()
