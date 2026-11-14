"""Kachel aus Klassenkarten (640x480-Verkleinerung): Ausschnitt um den Kopf, Farben je Klasse, Luecken-Maske gelb. python klbild.py OUT.png F N1,N2,.."""
import sys, numpy as np
from PIL import Image
import szene as SZ
from scipy import ndimage
from skimage.morphology import convex_hull_image
out, fl, names = sys.argv[1], [int(x) for x in sys.argv[2].split(',')], sys.argv[3].split(',')
FAR = {-2: (40, 40, 40), -1: (0, 0, 0), 0: (0, 140, 0), 1: (200, 60, 60), 2: (60, 90, 230), 3: (200, 0, 200), 4: (0, 210, 210), 5: (230, 230, 0)}
zeilen = []
for n in names:
    K = np.load(SZ.L_ + n + '_kl.npz')['K']; reihe = []
    for f in fl:
        k = SZ.klein(K[f - SZ.F0]); img = np.zeros(k.shape + (3,), np.uint8)
        for c, col in FAR.items(): img[k == c] = col
        H = k >= 3
        ys, xs = np.nonzero(H); cy, cx = int(ys.mean()), int(xs.mean())
        y0, x0 = max(0, cy - 50), max(0, cx - 70)
        crop = img[y0:y0 + 100, x0:x0 + 140]
        crop = np.pad(crop, ((0, 100 - crop.shape[0]), (0, 140 - crop.shape[1]), (0, 0)))
        reihe.append(np.kron(crop, np.ones((3, 3, 1), np.uint8)))
        reihe.append(np.full((300, 6, 3), 255, np.uint8))
    zeilen.append(np.concatenate(reihe, 1)); zeilen.append(np.full((6, zeilen[-1].shape[1], 3), 255, np.uint8))
Image.fromarray(np.concatenate(zeilen, 0)).save(out); print(out)
