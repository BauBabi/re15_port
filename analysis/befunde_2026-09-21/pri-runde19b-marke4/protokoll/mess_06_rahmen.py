"""Welche GEMALTEN Rahmenpunkte des Stuhls fehlen der Freistellung — und WOZU gehoeren sie?
Farbsignatur b-r trennt das neutrale Chromrohr vom blauen Eimer/Kasten dahinter
(gleiche Messung wie analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl.md §3)."""
import numpy as np
from scipy import ndimage as nd

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy"); reg2 = np.load("build/r19b/reg0702.npy")
deck = np.load("build/r19b/deck.npy")
L = bg.sum(2)
BR = bg[:, :, 2] - bg[:, :, 0]

print("=== Referenzfarben (Median b-r / Helligkeit) ===")


def ref(name, sl):
    m = np.zeros_like(reg); m[sl] = True
    print("  %-38s n=%5d  b-r %+4d  Hell %4d" % (name, int(m.sum()),
          int(np.median(BR[m])), int(np.median(L[m]))))
    return m


ref("Chromrohr vorn, GEDECKT (y196..232 x1..40)", (slice(196, 233), slice(1, 41)))
m = reg & (L >= 200)
print("  %-38s n=%5d  b-r %+4d  Hell %4d" % ("helle Punkte IN der Freistellung", int(m.sum()),
      int(np.median(BR[m])), int(np.median(L[m]))))
ref("Eimer/Kasten rechts (y118..150 x73..90)", (slice(118, 151), slice(73, 91)))
ref("Boden (y150..170 x80..110)", (slice(150, 171), slice(80, 111)))

# Kandidaten: hell, ungedeckt, im Stuhlfenster, an der Freistellung
nah = nd.binary_dilation(reg, np.ones((7, 7), bool)) & ~reg
fen = np.zeros_like(reg); fen[120:240, 0:75] = True
kand = nah & fen & ~deck & (L >= 150)
print("\n=== helle ungedeckte Punkte im Umkreis 3 um die Freistellung: %d ===" % int(kand.sum()))
lab, n = nd.label(kand, np.ones((3, 3), bool))
sz = sorted(((int((lab == i).sum()), i) for i in range(1, n + 1)), reverse=True)
for s, i in sz[:10]:
    m = lab == i
    ys, xs = np.nonzero(m)
    print("  %3d Punkte  x%2d..%2d y%3d..%3d  b-r %+3d  Hell %4d" %
          (s, xs.min(), xs.max(), ys.min(), ys.max(), int(np.median(BR[m])), int(np.median(L[m]))))

# die groesste Komponente = Kandidat "obere Rahmenschiene"
m = lab == sz[0][1]
np.save("build/r19b/kand_schiene.npy", m)
ys, xs = np.nonzero(m)
print("\ngroesste Komponente je Zeile:")
for y in range(ys.min(), ys.max() + 1):
    c = np.nonzero(m[y])[0]
    if len(c):
        print("  y%3d  x %s   Hell %s" % (y, " ".join("%d" % v for v in c),
              " ".join("%d" % L[y, v] for v in c)))
