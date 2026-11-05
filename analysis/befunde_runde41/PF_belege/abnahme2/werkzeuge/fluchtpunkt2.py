import numpy as np
from PIL import Image
g = np.asarray(Image.open('bg/ROOM20606.ppm').convert('RGB')).astype(float)
lum = g.mean(-1); gelb = g[..., 0] + g[..., 1] - 2 * g[..., 2]
YH = 67.975
def kante(img, p0, p1, vorz, halb=4, rand=1.5):
    (xa, ya), (xb, yb) = p0, p1
    m0 = (yb - ya) / (xb - xa)
    pts = []
    for x in range(int(np.ceil(xa + rand)), int(np.floor(xb - rand)) + 1):
        yc = ya + m0 * (x + .5 - xa)
        y0 = int(round(yc)) - halb; y1 = int(round(yc)) + halb + 1
        p = img[y0:y1, x]; d = np.diff(p) * vorz
        i = int(np.argmax(d))
        off = 0.0
        if 0 < i < len(d) - 1:
            a, b, c = d[i - 1], d[i], d[i + 1]
            den = a - 2 * b + c
            off = 0.5 * (a - c) / den if den else 0.0
        pts.append((x + .5, y0 + i + 1 + off))
    p = np.array(pts)
    A = np.c_[p[:, 0], np.ones(len(p))]
    m, b = np.linalg.lstsq(A, p[:, 1], rcond=None)[0]
    r = float(np.sqrt(np.mean((p[:, 1] - m * p[:, 0] - b) ** 2)))
    return m, b, r, len(p)
grob = {'weiss oben': (lum, (25, 117.5), (43.1, 111.0), +1), 'weiss unten': (lum, (30.6, 150.0), (46.9, 138.75), -1),
        'gelb oben': (gelb, (30.6, 156.0), (47.5, 144.4), +1), 'gelb unten': (gelb, (35.6, 190.6), (51.25, 170.0), -1)}
for halb in (3, 4, 5):
    out = []
    for n, (img, a, b, v) in grob.items():
        m, bb, r, k = kante(img, a, b, v, halb)
        out.append('%s %.1f (Rest %.2f, %d)' % (n, (YH - bb) / m, r, k))
    print('Fenster +-%d:' % halb, ' | '.join(out))
