"""Zoom um die Kerbe R3 F340 (echte Dumps): plain + Klassen + Kerbe gelb, R3/R2/V7, F339/F340."""
import sys, numpy as np, szene as S, kl_luecke as KL
from PIL import Image, ImageDraw
rows = []
m = S.mass(S.klein(np.load(S.L_ + 'R3_kl.npz')['K'][340 - S.F0])); ox, oy = m['ort']; cx, cy = int(ox * 1.5), int(oy * 1.5)
print('Kerbe R3 F340', m)
for n in ('R3', 'R2', 'V7'):
    K = np.load(S.L_ + n + '_kl.npz')['K']; cols = []
    for f in (339, 340, 341):
        im = S.ppm(n, f).astype(np.uint8); k = S.klein(K[f - S.F0]); G = KL.G_von(k)
        Gb = np.repeat(np.repeat(G, 3, 0), 3, 1)[::2, ::2][:720, :960]
        ov = im.copy(); ov[Gb] = (ov[Gb] * 0.4 + np.array([255, 255, 0]) * 0.6).astype(np.uint8)
        for a in (im, ov):
            c = Image.fromarray(a[cy - 60:cy + 60, cx - 60:cx + 60]).resize((360, 360), Image.NEAREST)
            d = ImageDraw.Draw(c); d.rectangle([0, 0, 110, 14], fill=(0, 0, 0)); d.text((2, 1), '%s F%d' % (n, f), fill=(255, 255, 0)); cols.append(np.asarray(c))
    rows.append(np.concatenate(cols, 1))
Image.fromarray(np.concatenate(rows, 0)).save(sys.argv[1])
