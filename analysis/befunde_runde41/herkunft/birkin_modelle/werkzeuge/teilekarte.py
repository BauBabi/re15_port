#!/usr/bin/env python3
"""Teilekarte M1: Meshes nach Herkunft eingefaerbt, Vorne/Links/Hinten (Clip 0, Bild 0). Plus Einzelbild Mesh 0 texturiert."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from PIL import Image, ImageDraw, ImageFont
import blaetter_lib as BL
AUS = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
ROT, BLAU, GRUEN, GELB, GRAU = (220, 90, 80), (70, 150, 230), (80, 200, 120), (230, 200, 60), (120, 120, 120)
FARBE = {0: GELB, 1: BLAU, 2: BLAU, 3: BLAU, 4: BLAU, 5: BLAU, 6: BLAU, 7: GRAU,
         8: ROT, 9: ROT, 10: ROT, 11: ROT, 12: ROT, 13: ROT, 14: ROT, 15: GRUEN}
mo = BL.modell(blob('M1'), 0x30, 'M1', 'M1')
kf = mo.frames[mo.clips[0][0]] & 0xfff
Vv, N, F = mo.geometrie(kf)
W, H = 800, 800
V1 = np.array(Vv)
lo, hi = V1.min(0), V1.max(0)
ctr = (lo + hi) / 2
sc = 720 / (hi - lo).max()
views = [('VORNE', (0, 0, 1), (0, 1, 0), (1, 0, 0)), ('LINKS', (-1, 0, 0), (0, 1, 0), (0, 0, 1)), ('HINTEN', (0, 0, -1), (0, 1, 0), (-1, 0, 0))]
bild = Image.new('RGB', (W * 3, H + 110), (24, 25, 29))
fnt = ImageFont.truetype(r'C:\Windows\Fonts\segoeui.ttf', 22)
for vi, (nm, r, d, de) in enumerate(views):
    r, d, de = (np.array(x, float) for x in (r, d, de))
    sx = (V1 - ctr) @ r * sc + W / 2
    sy = (V1 - ctr) @ d * sc + H / 2
    sz = (V1 - ctr) @ de
    Z = np.full((H, W), -1e18)
    C = np.zeros((H, W, 3), np.uint8)
    li = r * .3 - d * .5 + de * .8
    li /= np.linalg.norm(li)
    for f in F:
        i0, i1, i2 = f['v']
        x = np.array([sx[i0], sx[i1], sx[i2]])
        y = np.array([sy[i0], sy[i1], sy[i2]])
        z = np.array([sz[i0], sz[i1], sz[i2]])
        ar = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(ar) < 1e-6:
            continue
        n = np.cross(V1[i1] - V1[i0], V1[i2] - V1[i0])
        ln = np.linalg.norm(n)
        if ln == 0:
            continue
        sh = .35 + .65 * abs(n @ li) / ln
        x0, x1 = max(int(x.min()), 0), min(int(x.max()) + 2, W)
        y0, y1 = max(int(y.min()), 0), min(int(y.max()) + 2, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + .5, np.arange(y0, y1) + .5)
        w0 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / ar
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / ar
        w2 = 1 - w0 - w1
        inn = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        zz = w2 * z[0] + w1 * z[1] + w0 * z[2]
        sub = Z[y0:y1, x0:x1]
        hit = inn & (zz > sub)
        if not hit.any():
            continue
        sub[hit] = zz[hit]
        cc = C[y0:y1, x0:x1]
        cc[hit] = (np.array(FARBE[f['bone']]) * sh).astype(np.uint8)
    bild.paste(Image.fromarray(C), (vi * W, 0))
    ImageDraw.Draw(bild).text((vi * W + 12, 10), nm, fill=(235, 235, 235), font=fnt)
dr = ImageDraw.Draw(bild)
x = 20
for nm, c in (('Capcom: Mesh 8-14 byte-gleich D0', ROT), ('RE2 G2: Beine Mesh 1-6', BLAU),
              ('RE2 G2 + Umbau: Klaue Mesh 15', GRUEN), ('Mod neu: Rumpf Mesh 0', GELB)):
    dr.rectangle([x, H + 30, x + 26, H + 56], fill=c)
    dr.text((x + 34, H + 28), nm, fill=(235, 235, 235), font=fnt)
    x += 34 + int(dr.textlength(nm, font=fnt)) + 40
bild.save(os.path.join(AUS, 'M1_herkunft.png'))
print('ok', bild.size)
