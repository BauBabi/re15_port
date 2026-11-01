"""wa2_vergleich.py - Abnahme W2: Fix gegen Nutzer-exe je Cut. Vergleicht die Wechselzahl-Karten (blitz_wz.npy aus wa2_blitz.py,
Zahl der Bildwechsel > 16 je Bildpunkt, 320x240) zweier Laeufe: Bildpunkte mit Wechselzahl >= 6 je Lauf, Ueberdeckung (Jaccard),
Korrelation, und listet Flecken (>= 4 Bildpunkte), die nur in einem der Laeufe unruhig sind.
Aufruf: wa2_vergleich.py <runs_dir> <lauf_fix> <lauf_vorher> [fenster x0 x1 y0 y1]"""
import os, sys, numpy as np
from scipy import ndimage
runs, a, b = sys.argv[1:4]
fa = np.load(os.path.join(runs, a, "blitz_wz.npy")).astype(float); fb = np.load(os.path.join(runs, b, "blitz_wz.npy")).astype(float)
ma, mb = fa >= 6, fb >= 6
j = (ma & mb).sum() / max(1, (ma | mb).sum())
c = np.corrcoef(fa.ravel(), fb.ravel())[0, 1] if fa.std() > 0 and fb.std() > 0 else float("nan")
nur = []
for nm, m in (("nur Fix", ma & ~ndimage.binary_dilation(mb, iterations=3)), ("nur Nutzer-exe", mb & ~ndimage.binary_dilation(ma, iterations=3))):
    lab, k = ndimage.label(m, structure=np.ones((3, 3)))
    for i, sl in enumerate(ndimage.find_objects(lab), 1):
        n = int((lab[sl] == i).sum())
        if n >= 4: nur.append("%s %d px x%d..%d y%d..%d" % (nm, n, sl[1].start, sl[1].stop - 1, sl[0].start, sl[0].stop - 1))
s = "%s | %s: unruhig Fix %d / Nutzer-exe %d Bildpunkte, Jaccard %.2f, Korrelation %.2f" % (a, b, ma.sum(), mb.sum(), j, c)
if len(sys.argv) > 7:
    x0, x1, y0, y1 = map(int, sys.argv[4:8])
    s += " | Fenster x%d..%d y%d..%d: Wechselzahl max %d/%d, Punkte>=6 %d/%d" % (x0, x1, y0, y1, fa[y0:y1 + 1, x0:x1 + 1].max(), fb[y0:y1 + 1, x0:x1 + 1].max(),
                                                                              ma[y0:y1 + 1, x0:x1 + 1].sum(), mb[y0:y1 + 1, x0:x1 + 1].sum())
print(s)
for t in nur[:8]: print("   ", t)
