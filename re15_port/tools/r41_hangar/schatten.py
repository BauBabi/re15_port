# Runde 41 Spur H: dunkle Bodenflecken (Schatten) je Videobild im PSX-Bild. Aufruf: schatten.py <bildordner f_%05d.png> <von> <bis> <schritt>
import sys, os, numpy as np
from PIL import Image
src = sys.argv[1]
def psx(i):
    v = Image.open(os.path.join(src, 'f_%05d.png' % i)).convert('L').resize((320, 240), Image.BILINEAR)
    p = Image.new('L', (320, 240)); p.paste(v, (-1, -3)); return np.asarray(p, dtype=np.float32)
# Hintergrund ohne Figuren: Median ueber spaete Bilder (Birkin wandert) - nur Boden
ref = np.median(np.stack([psx(i) for i in (110, 125, 140, 30)]), axis=0)
for i in range(int(sys.argv[2]), int(sys.argv[3]) + 1, int(sys.argv[4])):
    a = psx(i)
    d = ref - a                     # dunkler als der Boden = Schatten/Figur
    m = (d > 25) & (a < 40)
    m[:178, :] = False; m[212:, :] = False
    cols = np.nonzero(m.sum(axis=0) > 2)[0]
    # zusammenhaengende Spaltenlaeufe
    runs = []; s = None
    for c in range(321):
        on = c < 320 and (m[:, c].sum() > 2)
        if on and s is None: s = c
        if not on and s is not None:
            if c - s >= 6:
                ys = np.nonzero(m[:, s:c].any(axis=1))[0]
                runs.append((s, c - 1, int(ys.min()), int(ys.max())))
            s = None
    print('h%-3d' % (i - 30), ' '.join('x%d-%d y%d-%d' % r for r in runs))
