"""w_flacker.py - Runde 41 Spur W: zaehlt in einer Bildfolge (frames/f_*.png, 640x480) die Bilder mit Aenderung
(> 8 Graustufen) ausserhalb einer Ausschlussspalte (Leon), meldet Lage und Vorzeichen.
Aufruf: python w_flacker.py <frames-dir> [von] [ausschluss_x0 ausschluss_x1] [--liste]"""
import os, sys
import numpy as np
from PIL import Image
d = sys.argv[1]; von = int(sys.argv[2]) if len(sys.argv) > 2 else 50
ax0, ax1 = (int(sys.argv[3]), int(sys.argv[4])) if len(sys.argv) > 4 else (130, 210)
liste = "--liste" in sys.argv
fs = sorted(f for f in os.listdir(d) if f.endswith(".png"))
prev = None; cnt = 0; n = 0; px = 0
for f in fs[von - 1:]:
    a = np.asarray(Image.open(os.path.join(d, f))).astype(int).mean(axis=2)
    if prev is not None:
        n += 1
        m = np.abs(a - prev) > 8
        m[:, ax0:ax1] = False
        if m.sum() > 0:
            cnt += 1; px += int(m.sum())
            if liste:
                ys, xs = np.nonzero(m)
                print(f, int(m.sum()), "x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()), "%+.1f" % (a - prev)[m].mean())
    prev = a
print("FLACKER %d von %d Bildwechseln, %d Bildpunkte" % (cnt, n, px))
