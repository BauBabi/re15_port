# Abnahme 5: Beben im Port-Framedump (960x720 -> 320x240 grau): Verschiebung je Bild gegen das Schnittbild (Bildmitte)
import numpy as np, sys
from PIL import Image
L = sys.argv[1]; f0, f1 = int(sys.argv[2]), int(sys.argv[3]); ref = int(sys.argv[4])
def g(f): return np.asarray(Image.open('%s/fd/f%06d.ppm' % (L, f)).convert('L').resize((320, 240), Image.BILINEAR)).astype(np.int16)
R = g(ref)
for f in range(f0, f1 + 1):
    v = g(f); best = None
    for s in range(-12, 13):
        a = v[40 + s:200 + s, 40:280]; b = R[40:200, 40:280]
        e = np.abs(a - b).mean()
        if best is None or e < best[1]: best = (s, e)
    print('F%d dy(gegen F%d) %+d (%.2f)  mittel %.1f' % (f, ref, best[0], best[1], v.mean()))
