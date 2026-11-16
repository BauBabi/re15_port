"""Kachel helle Bildpunkte (Regel szene2 C1) im Kasten (420,200)-(640,400): Zeilen R3, R2, R4, V7; Spalten F346, F355, F364, F366; links echt,
rechts helle Punkte rot markiert."""
import sys, numpy as np, szene as S
from PIL import Image, ImageDraw
rows = []
for n in ('R3', 'R2', 'R4', 'V7'):
    cols = []
    for f in (346, 355, 364, 366):
        im = S.ppm(n, f).astype(np.uint8)[200:400, 420:640]; i = im.astype(int); r, g, b = i[..., 0], i[..., 1], i[..., 2]
        m = (b >= r - 6) & (b > 120) & (r > 110); ov = im.copy(); ov[m] = (255, 0, 0)
        for a, t in ((im, ''), (ov, ' %d' % m.sum())):
            c = Image.fromarray(a).resize((330, 300), Image.NEAREST); d = ImageDraw.Draw(c); d.rectangle([0, 0, 110, 14], fill=(0, 0, 0)); d.text((2, 1), '%s F%d%s' % (n, f, t), fill=(255, 255, 0)); cols.append(np.asarray(c))
    rows.append(np.concatenate(cols, 1))
Image.fromarray(np.concatenate(rows, 0)).save(sys.argv[1])
