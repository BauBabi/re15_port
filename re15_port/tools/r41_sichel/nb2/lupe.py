"""Runde 41 SF NB2: vergroesserter Ausschnitt eines Videobilds (640x480) mit Linealen am Rand und einem Punktraster alle
10 Bildpunkte, zum Ablesen von Umriss- und Gelenkpunkten von Hand.
    python lupe.py <n> <x0> <y0> <x1> <y1> <zoom> <aus.png> [punkte.json]   (punkte.json: {"n": {"name": [x, y], ...}})"""
import json, os, sys
import numpy as np
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
V = os.path.join(HIER, '..', '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')


def lupe(n, x0, y0, x1, y1, z, punkte=None, polys=None, bild=None):
    a = np.asarray(Image.open(V % n).convert('RGB'), dtype=float) if bild is None else bild.astype(float)
    a = 255 * (a[y0:y1, x0:x1] / 255.0) ** 0.8
    R = 24
    im = Image.new('RGB', ((x1 - x0) * z + R, (y1 - y0) * z + R), (0, 0, 0))
    im.paste(Image.fromarray(np.clip(a, 0, 255).astype(np.uint8)).resize(((x1 - x0) * z, (y1 - y0) * z), Image.BICUBIC), (R, R))
    d = ImageDraw.Draw(im)
    for x in range((x0 // 10 + 1) * 10, x1, 10):
        X = R + (x - x0) * z
        d.line([(X, R - (10 if x % 50 == 0 else 4)), (X, R)], fill=(0, 255, 0))
        if x % 50 == 0: d.text((X + 2, 0), str(x), fill=(0, 255, 0))
        for y in range((y0 // 10 + 1) * 10, y1, 10):
            Y = R + (y - y0) * z
            d.point([(X, Y)], fill=(0, 255, 0) if (x % 50 == 0 or y % 50 == 0) else (0, 150, 0))
    for y in range((y0 // 10 + 1) * 10, y1, 10):
        Y = R + (y - y0) * z
        d.line([(R - (10 if y % 50 == 0 else 4), Y), (R, Y)], fill=(0, 255, 0))
        if y % 50 == 0: d.text((0, Y + 2), str(y), fill=(0, 255, 0))
    for poly, farbe in (polys or []):
        pts = [(R + (x - x0) * z, R + (y - y0) * z) for x, y in poly]
        d.line(pts + [pts[0]], fill=farbe, width=2)
    for name, (x, y) in (punkte or {}).items():
        X, Y = R + (x - x0) * z, R + (y - y0) * z
        d.ellipse([X - 4, Y - 4, X + 4, Y + 4], outline=(255, 0, 255), width=2); d.text((X + 6, Y - 6), name, fill=(255, 0, 255))
    return im


if __name__ == '__main__':
    n, x0, y0, x1, y1, z = (int(a) for a in sys.argv[1:7])
    P = json.load(open(sys.argv[8])).get(str(n)) if len(sys.argv) > 8 else None
    lupe(n, x0, y0, x1, y1, z, P).save(sys.argv[7])
