"""Nachbesserung 3: Pixelvergleich zweier Abzuege bzw. zweier Sweep-Ordner (s00..s12.bmp).
vgl_bilder.py <a.bmp|ordnerA> <b.bmp|ordnerB>  -> je Bild: Zahl verschiedener Pixel + Kasten"""
import sys, os
import numpy as np
from PIL import Image
def lade(p): return np.array(Image.open(p).convert('RGB')).astype(int)
def vgl(a, b, name):
    A, B = lade(a), lade(b)
    d = (np.abs(A - B).max(axis=2) > 0); n = int(d.sum())
    if n:
        ys, xs = np.nonzero(d)
        print('%s: %d Pixel verschieden, Kasten x %d..%d y %d..%d' % (name, n, xs.min(), xs.max(), ys.min(), ys.max()))
    else:
        print('%s: 0 Pixel verschieden' % name)
    return n
a, b = sys.argv[1], sys.argv[2]
if os.path.isdir(a):
    ges = 0
    for s in range(13):
        ges += vgl(os.path.join(a, 's%02d.bmp' % s), os.path.join(b, 's%02d.bmp' % s), 's%02d' % s)
    print('GESAMT %d' % ges)
else:
    vgl(a, b, os.path.basename(a))
