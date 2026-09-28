#!/usr/bin/env python
"""Runde 30 karten-marken (Fortsetzung): ZENSUS ueber ALLE Blaetter.
Zaehlt je Abzug (320x240, s_fb5) die Punkte in RE2-Blau (16,64,176) und die gelben
Tuermarken (224,168,40), die an KEINEN gezeichneten Punkt grenzen (8er-Nachbarschaft
der ganzen Marken-Komponente: nur Panelfarben (0,16,88)/(0,16,120) ringsum).
Aufruf: python r30_blau_zensus.py <abzug.bmp|png> [...]"""
import sys, os
import numpy as np
from PIL import Image

BLAU = (16, 64, 176)
GELB = (224, 168, 40)
PANEL = [(0, 16, 88), (0, 16, 120)]

def komponenten(mask):
    h, w = mask.shape
    seen = np.zeros_like(mask, dtype=bool)
    out = []
    ys, xs = np.nonzero(mask)
    for y, x in zip(ys, xs):
        if seen[y, x]:
            continue
        st = [(y, x)]; seen[y, x] = True; pts = []
        while st:
            cy, cx = st.pop(); pts.append((cy, cx))
            for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                ny, nx = cy + dy, cx + dx
                if 0 <= ny < h and 0 <= nx < w and mask[ny, nx] and not seen[ny, nx]:
                    seen[ny, nx] = True; st.append((ny, nx))
        out.append(pts)
    return out

def main():
    gb = gs = 0
    for p in sys.argv[1:]:
        im = np.array(Image.open(p).convert('RGB'))
        if im.shape[0] == 720:
            im = im[::3, ::3]
        h, w = im.shape[:2]
        blau = np.all(im == np.array(BLAU, dtype=np.uint8), axis=2)
        gelb = np.all(im == np.array(GELB, dtype=np.uint8), axis=2)
        panel = np.zeros((h, w), dtype=bool)
        for c in PANEL:
            panel |= np.all(im == np.array(c, dtype=np.uint8), axis=2)
        txt = []
        bk = komponenten(blau)
        for pts in bk:
            ys = [q[0] for q in pts]; xs = [q[1] for q in pts]
            txt.append("blau n=%d x %d..%d y %d..%d" % (len(pts), min(xs), max(xs), min(ys), max(ys)))
        frei = []
        for pts in komponenten(gelb):
            rand = set()
            for (y, x) in pts:
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        ny, nx = y + dy, x + dx
                        if 0 <= ny < h and 0 <= nx < w and not gelb[ny, nx]:
                            rand.add((ny, nx))
            if rand and all(panel[q] for q in rand):
                ys = [q[0] for q in pts]; xs = [q[1] for q in pts]
                frei.append("(%d..%d,%d..%d)" % (min(xs), max(xs), min(ys), max(ys)))
        n = int(blau.sum())
        gb += n; gs += len(frei)
        print("%-46s blau %5d Punkte in %2d Stueck | gelbe Marken %2d, davon frei schwebend %d %s"
              % (os.path.basename(p), n, len(bk), len(komponenten(gelb)), len(frei), ' '.join(frei)))
        for t in txt[:12]:
            print("      " + t)
    print("SUMME: blau %d Punkte, frei schwebende Marken %d" % (gb, gs))

if __name__ == '__main__':
    main()
