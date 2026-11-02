"""Bildleiste RE2 | Port fuer das Blut beim Hieb (Nachbesserung 2). Paare im gleichen Abstand zum ersten Blutbild."""
import sys
from PIL import Image, ImageDraw

re2dir, portdir, ziel = sys.argv[1], sys.argv[2], sys.argv[3]
port0 = int(sys.argv[4])      # erstes Port-Bild mit Blut (10 Bilder/s)
W, H = 400, 300
re2_t = ['70.91', '71.09', '71.19', '71.29', '71.39', '71.49', '71.69', '71.89', '72.09', '72.29']


def lab(im, txt):
    im = im.convert('RGB').resize((W, H))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, W, 18), fill=(0, 0, 0))
    d.text((5, 4), txt, fill=(255, 220, 80))
    return im


zeilen = []
for k, t in enumerate(re2_t):
    dt = float(t) - 71.09
    a = lab(Image.open('%s/re2_%s.png' % (re2dir, t)), 'RE2 Claire %s s (%+.2f s zum ersten Blut)' % (t, dt))
    n = port0 + int(round(dt * 10))
    b = lab(Image.open('%s/bild_%03d.png' % (portdir, n)), 'Port Bild %d (%+.1f s zum ersten Blut, 10 Bilder/s)' % (
        n, (n - port0) * 0.1))
    zeilen.append((a, b))
blatt = Image.new('RGB', (2 * W, H * len(zeilen)), (20, 20, 20))
for i, (a, b) in enumerate(zeilen):
    blatt.paste(a, (0, i * H))
    blatt.paste(b, (W, i * H))
blatt.save(ziel, quality=85)
print(ziel, blatt.size)
