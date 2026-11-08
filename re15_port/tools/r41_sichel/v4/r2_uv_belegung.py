"""SF V4 Runde 2 (Bauer): Texel-Belegung je Mesh der Koerper-UVs (Referenz 2) - welche Meshes teilen Texel (Seite, Zeile)?"""
import os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2


def belegung(mo, meshes=range(16)):
    """dict mesh -> bool-Maske (texh x texw) der Texel unter den UV-Dreiecken, dazu dict mesh -> Menge der CLUT-Zeilen."""
    out, zeilen = {}, {}
    for b in meshes:
        m = mo.meshes[b]
        mk = np.zeros((mo.texh, mo.texw), np.uint8)
        zs = set()
        for faces, uvs, n in ((m.tf, m.tuv, 3), (m.qf, m.quv, 4)):
            for uv in uvs:
                po = (uv[5] & 0xF) * 128
                pts = [(uv[3 * k] + po, uv[3 * k + 1]) for k in range(n)]
                if n == 4:
                    pts = [pts[0], pts[1], pts[3], pts[2]]
                cv2.fillConvexPoly(mk, np.array(pts, np.int32), 1)
                zs.add((uv[2] >> 6) & 0x1FF)
        out[b] = mk.astype(bool); zeilen[b] = zs
    return out, zeilen


if __name__ == '__main__':
    mo = b2.modell(sys.argv[1] if len(sys.argv) > 1 else G.REF2)
    print('tex', mo.tex.shape, 'lut', mo.lut.shape, 'clut_y', mo.clut_y)
    bel, zl = belegung(mo)
    for b in range(16):
        print(b, 'texel', int(bel[b].sum()), 'zeilen', sorted(zl[b]))
    for a in range(16):
        for c in range(a + 1, 16):
            n = int((bel[a] & bel[c]).sum())
            if n:
                print('geteilt', a, c, n)
