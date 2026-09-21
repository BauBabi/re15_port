"""Die 187 Punkte des Nutzers nach zusammenhaengenden Gruppen (8-Nachbarschaft) — damit
das Dossier benennen kann, WAS er markiert hat, statt nur eine Summe zu nennen.
"""
import numpy as np
from scipy import ndimage as nd

alt = np.load("build/r19c/alt.npy")
neu = np.load("build/r19c/neu.npy")
saum = neu & ~alt
lab, n = nd.label(saum, np.ones((3, 3), bool))
print("187 Punkte in %d zusammenhaengenden Gruppen:" % n)
gr = []
for i in range(1, n + 1):
    ys, xs = np.nonzero(lab == i)
    gr.append((len(ys), xs.min(), xs.max(), ys.min(), ys.max()))
for (c, x0, x1, y0, y1) in sorted(gr, reverse=True):
    print("   %4d Punkte  x%2d..%2d  y%3d..%3d" % (c, x0, x1, y0, y1))
print("Gruppen mit mindestens 5 Punkten: %d (zusammen %d Punkte)"
      % (sum(1 for g in gr if g[0] >= 5), sum(g[0] for g in gr if g[0] >= 5)))
print("Einzelpunkte: %d" % sum(1 for g in gr if g[0] == 1))
