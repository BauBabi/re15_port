# Leser A V3: findet in jedem Freisteller den Augenfleck = dunkles Kompaktgebiet (L*<22) mit hellem Kern (L*>55)
# im oberen Drittel; gibt Alpha-Bbox (H) und Kandidaten aus. Reine Messung, keine Aenderung.
import numpy as np, cv2, sys
from PIL import Image
SRC = r'C:/workspace/git/reAi_v2/Version_80%/'
def lab(rgb):
    a = cv2.cvtColor((rgb[None] if rgb.ndim == 2 else rgb).astype(np.float32) / 255.0, cv2.COLOR_RGB2Lab)
    return a
for nn in range(1, 25):
    im = np.array(Image.open(SRC + 'birkin%02d.png' % nn).convert('RGBA'))
    al = im[..., 3] > 128
    ys, xs = np.nonzero(al)
    y0, y1, x0, x1 = ys.min(), ys.max(), xs.min(), xs.max()
    L = lab(im[..., :3])[..., 0]
    print('birkin%02d bbox x %d-%d y %d-%d H=%d' % (nn, x0, x1, y0, y1, y1 - y0 + 1))

print('--- Augenkandidaten: dunkle Kompaktflecken (L*<22, Flaeche 8-120) mit hellem Kern (L*>50) im Umkreis 3 px, obere 40 %')
for nn in range(1, 25):
    im = np.array(Image.open(SRC + 'birkin%02d.png' % nn).convert('RGBA'))
    al = cv2.erode((im[..., 3] > 128).astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    ys, xs = np.nonzero(im[..., 3] > 128)
    ytop, ybot = ys.min(), ys.max()
    L = lab(im[..., :3])[..., 0]
    dark = ((L < 22) & al).astype(np.uint8)
    dark[int(ytop + 0.4 * (ybot - ytop)):] = 0
    n, lbl, st, cen = cv2.connectedComponentsWithStats(dark, 8)
    out = []
    for i in range(1, n):
        a = st[i, 4]
        if not (8 <= a <= 120):
            continue
        x, y, w, h = st[i, :4]
        if w > 3.5 * max(h, 1) * 2 or h > 2.5 * w:
            pass
        reg = L[max(y - 3, 0):y + h + 3, max(x - 3, 0):x + w + 3]
        regal = al[max(y - 3, 0):y + h + 3, max(x - 3, 0):x + w + 3]
        mx = reg[regal].max() if regal.any() else 0
        # heller Kern: hellstes Pixel innerhalb der Bbox des Flecks
        inner = L[y:y + h, x:x + w]
        if inner.max() > 50:
            out.append('(%d,%d %dx%d A%d kernL%.0f)' % (x, y, w, h, a, inner.max()))
    print('birkin%02d' % nn, ' '.join(out))
