"""Die 53 NEUTRALEN hellen Punkte, die an Marke F3843 uebermalt wurden und keine Maske
tragen: wo liegen sie, und gehoeren sie zum Stuhl?"""
import numpy as np
from scipy import ndimage as nd

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
rn = np.load("build/r19b/render_F3843.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy"); reg2 = np.load("build/r19b/reg0702.npy")
deck = np.load("build/r19b/deck.npy")
L = bg.sum(2); BR = bg[:, :, 2] - bg[:, :, 0]
ueber = (np.abs(rn - bg).sum(2) > 30)
fen = np.zeros_like(reg); fen[118:240, 0:76] = True

m = ueber & fen & ~deck & (L >= 150) & (np.abs(BR) < 8)
print("neutrale helle uebermalte ungedeckte Punkte im Stuhlfenster: %d" % int(m.sum()))
lab, n = nd.label(m, np.ones((3, 3), bool))
sz = sorted(((int((lab == i).sum()), i) for i in range(1, n + 1)), reverse=True)
d2 = nd.distance_transform_edt(~reg)
for s, i in sz:
    mm = lab == i; yy, xx = np.nonzero(mm)
    print("  %3d Punkte  x%2d..%2d y%3d..%3d  b-r %+3d  Hell %4d  Abstand zur Freistellung %.1f..%.1f"
          % (s, xx.min(), xx.max(), yy.min(), yy.max(), int(np.median(BR[mm])),
             int(np.median(L[mm])), d2[mm].min(), d2[mm].max()))
print()
for s, i in sz[:4]:
    mm = lab == i
    print("Komponente %d Punkte:" % s)
    for y in sorted(set(np.nonzero(mm)[0])):
        c = np.nonzero(mm[y])[0]
        print("   y%3d  x %s  Hell %s  b-r %s" % (y, " ".join("%d" % v for v in c),
              " ".join("%d" % L[y, v] for v in c), " ".join("%+d" % BR[y, v] for v in c)))
np.save("build/r19b/neutral_uebermalt.npy", m)
