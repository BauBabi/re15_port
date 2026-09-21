"""Der SAUM: Punkte des gemalten Chromrohrs, die die Nutzer-Freistellung um einen Punkt
verfehlt. Zwei UNABHAENGIGE Kriterien, nur ihr Schnitt wird genommen:

  (1) FARBE: hell (r+g+b >= 150) und farbneutral (-5 <= b-r <= +5).
      Geeicht an drei Referenzen, die NICHT aus dieser Regel stammen:
        Chrom IN der Nutzer-Freistellung (L>=250): b-r Quartile -1 / +1 / +3
        blauer Gegenstand x44..65 y126..156 (L>=150): b-r Quartile +15 / +21 / +25
        Boden x80..110 y150..170: b-r Median -8, Helligkeit Median 95
  (2) FORTSETZUNG: das Rohr laeuft UNTER der Freistellung weiter — im Umkreis 2 liegt
      ein Punkt, der in der Freistellung liegt UND selbst hell+neutral ist.
      Dieses Kriterium kennt keine Farbe des Kandidaten; es prueft die Nachbarschaft.

Ein Punkt wird nur genommen, wenn er 8-Nachbar der Freistellung ist. Damit kann kein
neuer Gegenstand entstehen — nur die Kante des vorhandenen geschlossen werden.
"""
import numpy as np
from scipy import ndimage as nd

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy")
deck = np.load("build/r19b/deck.npy")
L = bg.sum(2); BR = bg[:, :, 2] - bg[:, :, 0]

HELL, LO, HI = 150, -5, 5
fen = np.zeros_like(reg); fen[118:239, 0:76] = True        # Zeile 239 = Bildrand, raus
neutral = (L >= HELL) & (BR >= LO) & (BR <= HI)
nah = nd.binary_dilation(reg, np.ones((3, 3), bool)) & ~reg
k1 = neutral & nah & fen & ~deck
# (2) Fortsetzung
kern = reg & neutral
fort = nd.binary_dilation(kern, np.ones((5, 5), bool))
k2 = nah & fen & ~deck & fort

print("Kriterium (1) Farbe          : %4d Punkte" % int(k1.sum()))
print("Kriterium (2) Fortsetzung    : %4d Punkte" % int(k2.sum()))
saum = k1 & k2
print("SCHNITT (1) UND (2)          : %4d Punkte" % int(saum.sum()))
print("nur (1), von (2) verworfen   : %4d" % int((k1 & ~k2).sum()))
print("nur (2), von (1) verworfen   : %4d" % int((k2 & ~k1).sum()))
ys, xs = np.nonzero(k1 & ~k2)
print("   von (2) verworfen:", ", ".join("(%d,%d)" % (x, y) for x, y in zip(xs, ys)))

lab, n = nd.label(saum, np.ones((3, 3), bool))
print("\n%d Komponenten:" % n)
for i in range(1, n + 1):
    m = lab == i; yy, xx = np.nonzero(m)
    print("   %2d Punkte  x%2d..%2d y%3d..%3d  b-r %+3d  Hell %4d"
          % (int(m.sum()), xx.min(), xx.max(), yy.min(), yy.max(),
             int(np.median(BR[m])), int(np.median(L[m]))))
np.save("build/r19b/saum.npy", saum)

# Nullmodell: wie viele Punkte faengt dieselbe Regel, wenn man sie auf den BODEN legt?
for name, sl in (("Boden x80..110 y150..200", (slice(150, 201), slice(80, 111))),
                 ("Flurboden x120..200 y170..220", (slice(170, 221), slice(120, 201))),
                 ("Tischplatte x0..45 y150..170", (slice(150, 171), slice(0, 46)))):
    m = np.zeros_like(reg); m[sl] = True
    print("Nullmodell %-32s: %d von %d Punkten erfuellen die Farbregel (%.1f %%)"
          % (name, int((m & neutral).sum()), int(m.sum()), 100.0 * (m & neutral).sum() / m.sum()))
