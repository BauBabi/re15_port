#!/usr/bin/env python
"""Runde 30, Thema karten-marken-abzug: Pixelmessung an den Nutzer-Abzuegen.
Aufruf: python r30_abzug_messen.py <bild> [<bild> ...]
Gibt je Bild: Groesse, Haeufigkeit der Farben im Kartenfeld, Bbox je Zielfarbe
(RE2-Blau 1040b0, Tuer-Gelb, Wandgrau), jeweils in 960er- und 320er-Koordinaten."""
import sys
from collections import Counter
import numpy as np
from PIL import Image

ZIEL = {
    "RE2-Blau (16,64,176)": (16, 64, 176),
    "Tuer-Gelb (224,168,40)": (224, 168, 40),
    "Wandgrau (176,176,176)": (176, 176, 176),
    "Aktuell (104,8,8)": (104, 8, 8),
}

def komponenten(mask):
    """4er-Zusammenhang, liefert Liste (n, x0,y0,x1,y1)."""
    h, w = mask.shape
    seen = np.zeros_like(mask, dtype=bool)
    out = []
    ys, xs = np.nonzero(mask)
    for y, x in zip(ys, xs):
        if seen[y, x]:
            continue
        st = [(y, x)]
        seen[y, x] = True
        n = 0
        x0 = x1 = x
        y0 = y1 = y
        while st:
            cy, cx = st.pop()
            n += 1
            x0 = min(x0, cx); x1 = max(x1, cx)
            y0 = min(y0, cy); y1 = max(y1, cy)
            for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                ny, nx = cy + dy, cx + dx
                if 0 <= ny < h and 0 <= nx < w and mask[ny, nx] and not seen[ny, nx]:
                    seen[ny, nx] = True
                    st.append((ny, nx))
        out.append((n, x0, y0, x1, y1))
    return out

def main():
    for p in sys.argv[1:]:
        im = np.array(Image.open(p).convert("RGB"))
        h, w = im.shape[:2]
        f = w / 320.0
        print("==", p, "%dx%d" % (w, h), "Faktor %.3f" % f)
        # Kartenfeld grob: 320er x 20..300, y 25..215
        feld = im[int(25 * f):int(215 * f), int(20 * f):int(300 * f)]
        c = Counter(map(tuple, feld.reshape(-1, 3)))
        print("   haeufigste Farben im Kartenfeld:")
        for col, n in c.most_common(14):
            print("     %-18s %7d" % (col, n))
        for name, col in ZIEL.items():
            m = np.all(im == np.array(col, dtype=np.uint8), axis=2)
            n = int(m.sum())
            if not n:
                print("   %-26s 0 Pixel" % name)
                continue
            ks = komponenten(m)
            print("   %-26s %d Pixel in %d Komponente(n)" % (name, n, len(ks)))
            for (kn, x0, y0, x1, y1) in sorted(ks, key=lambda k: -k[0])[:24]:
                print("       n=%5d  960er x %d..%d y %d..%d   320er x %.1f..%.1f y %.1f..%.1f"
                      % (kn, x0, x1, y0, y1, x0 / f, x1 / f, y0 / f, y1 / f))

if __name__ == "__main__":
    main()
