"""Runde 41 SF NB2: Labor 50E0 Schnitt 5 (Steg) - Video 4 | vorher | nachher, Buehne (birkin_buehne, RID-Kamera + RDT-Licht,
Abbildung SF_belege/kamera_50E0_5.json, ECC 0,85), Lagen = Rueckprojektion der Sohlen (Gelaender verdeckt die Fuesse: +-10 Bildpunkte),
Clip 1 (Gang) Bild 10, Gier zur Kamera. Im echten Fenster wartet der Labor-Birkin ausserhalb bis zum Pult-Ausloeser
(SF Abschnitt 5) - deshalb Buehne.  python laborblatt.py <vorher.emd> <nachher.emd> <aus.png>"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HIER, '..', '..'))
import birkin_buehne as BB   # noqa: E402

LAGEN = {500: (-13878, -17031), 530: (-13796, -16904), 560: (-13069, -17165)}   # boden_aus_video(50E0, 5, Sohle ~(348,232)/(352,232)/(358,238))
KASTEN = (210, 60, 470, 300)


def main():
    va, na, aus = sys.argv[1:4]
    zeilen = []
    for n, (x, z) in LAGEN.items():
        vid = BB.video_bild(n)
        t = [vid]
        for e in (va, na):
            ueber, m, _g = BB.render(open(e, 'rb').read(), '50E0', 5, 1, 10, (x, 0, z), 'zur_kamera', vid)
            r = ueber.copy(); r[m & ~ndimage.binary_erosion(m, iterations=1)] = (0, 255, 255)
            t.append(r)
        x0, y0, x1, y1 = KASTEN
        t = [Image.fromarray(a[y0:y1, x0:x1]).resize(((x1 - x0) * 2, (y1 - y0) * 2), Image.BICUBIC) for a in t]
        z_ = Image.new('RGB', (sum(i.size[0] + 4 for i in t), t[0].size[1] + 16), (20, 20, 20)); d = ImageDraw.Draw(z_)
        for i, (im, tt) in enumerate(zip(t, ('Video 4 n%d (Labor 50E0 Steg)' % n, 'vorher 3799d7e94', 'nachher'))):
            z_.paste(im, (i * (im.size[0] + 4), 16)); d.text((i * (im.size[0] + 4) + 2, 2), tt, fill=(255, 255, 0))
        zeilen.append(z_)
    W = Image.new('RGB', (zeilen[0].size[0], sum(z.size[1] for z in zeilen)))
    yy = 0
    for z in zeilen: W.paste(z, (0, yy)); yy += z.size[1]
    W.save(aus); print(aus, W.size)


main()
