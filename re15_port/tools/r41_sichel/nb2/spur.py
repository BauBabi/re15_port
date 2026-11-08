"""Runde 41 SF NB2: Nachzeichen-Hilfe. Links Videobild (Lupe), rechts Differenz zum BSS-Hintergrund (verschiebungstolerant,
3 Bildpunkte) als Graubild, beide mit Linealen; optional Polygon(e) aus umrisse.json eingezeichnet.
    python spur.py <n> <x0> <y0> <x1> <y1> <zoom> <aus.png> [umrisse.json]"""
import json, os, sys
import numpy as np
import cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import umriss as U, lupe as L   # noqa: E402


def diffbild(n):
    v = U.video(n); bb = cv2.GaussianBlur(U.hintergrund(), (0, 0), 2.0); fb = cv2.GaussianBlur(v, (0, 0), 1.0)
    best = None
    for dy in range(-3, 4):
        for dx in range(-3, 4):
            d = np.sqrt(((fb - np.roll(np.roll(bb, dy, 0), dx, 1)) ** 2).sum(2))
            best = d if best is None else np.minimum(best, d)
    g = np.clip(best * 4, 0, 255)
    return np.stack([g, g, g], -1)


if __name__ == '__main__':
    n, x0, y0, x1, y1, z = (int(a) for a in sys.argv[1:7])
    polys = []
    if len(sys.argv) > 8 and os.path.exists(sys.argv[8]):
        for p in json.load(open(sys.argv[8])).get(str(n), []):
            polys.append((p, (255, 0, 255)))
    a = L.lupe(n, x0, y0, x1, y1, z, polys=polys)
    b = L.lupe(n, x0, y0, x1, y1, z, polys=polys, bild=diffbild(n))
    w = Image.new('RGB', (a.size[0] * 2 + 6, a.size[1]), (60, 60, 60)); w.paste(a, (0, 0)); w.paste(b, (a.size[0] + 6, 0))
    w.save(sys.argv[7])
