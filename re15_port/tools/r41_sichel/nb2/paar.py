"""Runde 41 SF NB2: Video | Port-Buehne (gleiche Kamera, gleicher Ausschnitt, Lineale) je h.
    python paar.py <emd> <debug.log> <h,h,..> <aus.png> [zoom=2] [x0,y0,x1,y1]"""
import os, sys
import numpy as np
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import portmaske as PM, umriss as U, lupe as L   # noqa: E402
import birkin_buehne as BB                        # noqa: E402
from scipy import ndimage                         # noqa: E402


def main():
    emd, log, hs, aus = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(',')], sys.argv[4]
    z = int(sys.argv[5]) if len(sys.argv) > 5 else 2
    x0, y0, x1, y1 = (int(v) for v in sys.argv[6].split(',')) if len(sys.argv) > 6 else (250, 120, 540, 470)
    mo = BB.modell(open(emd, 'rb').read()); P = PM.posen(log)
    zeilen = []
    for h in hs:
        c, b, g, x, zz = P[h]
        bild, m = PM.maske(mo, c, b, g, x, zz, hintergrund=U.hintergrund().astype(np.uint8))
        rand = m & ~ndimage.binary_erosion(m, iterations=1)
        v = U.video(1350 + h).copy(); v[rand] = (0, 255, 255)
        a = L.lupe(1350 + h, x0, y0, x1, y1, z, bild=v)
        p = L.lupe(1350 + h, x0, y0, x1, y1, z, bild=bild)
        w = Image.new('RGB', (a.size[0] * 2 + 8, a.size[1]), (40, 40, 40)); w.paste(a, (0, 0)); w.paste(p, (a.size[0] + 8, 0))
        zeilen.append(w)
    W = Image.new('RGB', (zeilen[0].size[0], sum(q.size[1] + 6 for q in zeilen)))
    yy = 0
    for q in zeilen: W.paste(q, (0, yy)); yy += q.size[1] + 6
    W.save(aus); print(aus, W.size)


main()
