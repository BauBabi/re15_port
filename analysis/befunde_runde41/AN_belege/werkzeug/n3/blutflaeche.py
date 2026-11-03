"""Blut-Flaeche: Anteil rein roter Pixel (R >= 30, G und B <= 0.12 R + 3) im sichtbaren Band, abzueglich der vor dem Treffer schon
roten Pixel. RE2 (Aufnahme 320x240, Band y 32..208) und Port (RE15_FRAMEDUMP 640x480, Balken 48/432)."""
import numpy as np, sys
from PIL import Image
def maske(p, band):
    a = np.asarray(Image.open(p).convert('RGB')).astype(int)[band[0]:band[1]]
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    return (r >= 30) & (g <= 0.12 * r + 3) & (b <= 0.12 * r + 3)
def reihe(basis, bilder, band):
    m0 = maske(basis, band)
    return [(n, float((maske(p, band) & ~m0).mean())) for n, p in bilder]
if __name__ == '__main__':
    R = 're2/re2_%s.png'
    for n, v in reihe(R % '70.9', [(t, R % t) for t in ('71.09', '71.3', '71.43', '71.6', '71.78', '71.95', '72.13')], (32, 208)):
        print('RE2  %s s  %.3f' % (n, v))
    L = sys.argv[1] + '/f%06d.ppm'
    f0 = int(sys.argv[2])
    for n, v in reihe(L % (f0 - 2), [(f, L % f) for f in range(f0, f0 + 34, 4)], (48, 432)):
        print('Port F%d (+%.2f s)  %.3f' % (n, (n - f0) / 30.0, v))
