"""Pruefer P5 (G) Rest gegen r6: Figur-Umriss ausserhalb des Kopffensters (echte Dumps), Schlitz (eingeschlossener Hintergrund in den
obersten 40 bzw. 120 Zeilen der Figur, wie Pruefer r6), Boden (tiefster Punkt ueber alle 912 Keyframes, Mesh 8)."""
import numpy as np, sys, os
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import szene as S, pl, vorn as Vn
N = ['R3', 'R2', 'R6', 'R4', 'V7', 'R5D']
K = {n: np.load(S.L_ + n + '_kl.npz')['K'] for n in N}
print('(G1) Figur-Bildpunkte (Klasse >= 0) anders als r6, F225-F450 (960x720): im Kopffenster (Kopf +-60) / ausserhalb')
for n in N:
    if n == 'R6': continue
    inn = aus = 0; bild = []
    for i in range(K[n].shape[0]):
        a, b = K[n][i] >= 0, K['R6'][i] >= 0; d = a != b
        H = K['R6'][i] >= 3; W = np.zeros(d.shape, bool)
        if H.any():
            ys, xs = np.nonzero(H); W[max(ys.min() - 60, 0):ys.max() + 61, max(xs.min() - 60, 0):xs.max() + 61] = True
        inn += int((d & W).sum()); o = int((d & ~W).sum()); aus += o
        if o > 50: bild.append((225 + i, o))
    print('   %-4s im Fenster %d | ausserhalb %d | Bilder ausserhalb > 50 px: %s' % (n, inn, aus, bild[:12]))
print('(G2) Schlitz: eingeschlossener Hintergrund (Loecher der Figurmaske) in den obersten 40 / 120 Zeilen der Figur, F235-F378, Summe')
for n in N:
    s40 = s120 = 0; worst = []
    for f in range(235, 379):
        k = K[n][f - S.F0]; F = k >= 0
        if not F.any(): continue
        y0 = np.nonzero(F.any(1))[0].min()
        loch = ndimage.binary_fill_holes(F) & ~F
        a, b = int(loch[y0:y0 + 40].sum()), int(loch[y0:y0 + 120].sum()); s40 += a; s120 += b
        if a: worst.append((f, a))
    print('   %-4s oberste 40: %d | oberste 120: %d | Bilder mit Loch oben-40: %s' % (n, s40, s120, worst[:10]))
print('(G3) Boden: groesstes Welt-y (unten) ueber 912 Keyframes, nur Mesh 8 / alle Meshes (Knochenraum + Wurzel, ohne Gieren)')
for n in N:
    E = Vn.lade(n); m8 = []; al = []
    for kf in range(E.nkf):
        R, T = E.pose(kf)
        ys8 = ((R[8] @ E.meshes[8]['tv'].T) >> 12)[1] + T[8][1]; m8.append(int(ys8.max()))
        al.append(max(int((((R[b] @ E.meshes[b]['tv'].T) >> 12)[1] + T[b][1]).max()) for b in range(len(E.meshes)) if len(E.meshes[b]['tv'])))
    print('   %-4s Mesh 8 max y %d (Keyframe %d) | alle Meshes max y %d' % (n, max(m8), int(np.argmax(m8)), max(al)))
