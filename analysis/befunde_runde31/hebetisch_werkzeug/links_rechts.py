# -*- coding: utf-8 -*-
"""Runde 31 / H — Links/rechts GEMESSEN im Framedump (320x240): je Bild die Punkte, die zur
Granate bzw. zur Sicherung gehoeren, ueber die MIT/OHNE-Differenz (Kill-Switch Genommen-Flag):
  Granate   = MIT  gegen OHNE-G (RE15_SET_FLAG=9:56)  -> sichtbare Granate im vollen Bild
  Sicherung = MIT  gegen OHNE-S (RE15_SET_FLAG=9:53)  -> sichtbare Sicherung im vollen Bild
Ausgabe je Bild: Punkte, Schwerpunkt x/y, bbox; Trennung = Schwerpunkt-x Sicherung - Granate,
Anteil der Granaten-Punkte links vom Sicherungs-Schwerpunkt und umgekehrt.
links_rechts.py <mit> <ohneG> <ohneS> <von> <bis> [schritt]"""
import os, sys
import numpy as np
from PIL import Image
mit, og, os_, von, bis = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), int(sys.argv[5])
st = int(sys.argv[6]) if len(sys.argv) > 6 else 2


def lade(o, f):
    p = os.path.join(o, 'f_%06d.ppm' % f)
    return np.asarray(Image.open(p).convert('RGB'), np.int16) if os.path.exists(p) else None


zeilen = []
for f in range(von, bis + 1, st):
    a, g, s = lade(mit, f), lade(og, f), lade(os_, f)
    if a is None or g is None or s is None:
        continue
    mg = np.abs(a - g).max(-1) > 0
    ms = np.abs(a - s).max(-1) > 0
    def kenn(m):
        ys, xs = np.nonzero(m)
        if not len(xs):
            return 0, None, None, None
        return len(xs), xs.mean(), ys.mean(), (xs.min(), xs.max(), ys.min(), ys.max())
    ng, gx, gy, gb = kenn(mg)
    ns, sx, sy, sb = kenn(ms)
    txt = 'F%d: Granate %4d' % (f, ng)
    if ng: txt += ' x%.1f y%.1f bbox x%d..%d y%d..%d' % ((gx, gy) + gb)
    txt += ' | Sicherung %4d' % ns
    if ns: txt += ' x%.1f y%.1f bbox x%d..%d y%d..%d' % ((sx, sy) + sb)
    if ng and ns:
        ys, xs = np.nonzero(mg); lg = float(np.mean(xs < sx))
        ys2, xs2 = np.nonzero(ms); rs = float(np.mean(xs2 > gx))
        txt += ' | Trennung %+.1f px, Granate links vom S-Schwerpunkt %.0f %%, Sicherung rechts vom G-Schwerpunkt %.0f %%' % (sx - gx, 100 * lg, 100 * rs)
    print(txt)
