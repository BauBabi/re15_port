"""Runde 41 SF NB2: Umriss-Blatt je nachgezeichnetem Videobild: Videobild mit Video-Umriss (gelb, von Hand), Port vorher
(rot) und nachher (tuerkis) an der gespielten Pose der Messzeile (Buehne, gleiche Kamera Schnitt 12).
    python umrissblatt.py <vorher.emd> <vorher debug.log> <nachher.emd> <nachher debug.log> <aus.jpg>"""
import json, os, sys
import numpy as np
import cv2
from PIL import Image, ImageDraw
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import portmaske as PM, umriss as U, messung as MS   # noqa: E402
import birkin_buehne as BB                            # noqa: E402

ve, vl, ne, nl, aus = sys.argv[1:6]
mv, mn = BB.modell(open(ve, 'rb').read()), BB.modell(open(ne, 'rb').read())
Pv, Pn = PM.posen(vl), PM.posen(nl)
kacheln = []
for n in sorted(int(k) for k in json.load(open(MS.UMRISSE))):
    h = n - 1350
    a = U.video(n).astype(np.uint8).copy()
    for mo, P, farbe in ((mv, Pv, (255, 60, 60)), (mn, Pn, (0, 255, 255))):
        c, b, g, x, z = P[h]
        _bild, m = PM.maske(mo, c, b, g, x, z, licht=False)
        r = m & ~ndimage.binary_erosion(m, iterations=1)
        a[ndimage.binary_dilation(r)] = farbe
    for p in json.load(open(MS.UMRISSE))[str(n)]:
        cv2.polylines(a, [np.array(p, np.int32)], True, (255, 255, 0), 2)
    t = Image.fromarray(a[60:480, 200:580])
    d = ImageDraw.Draw(t)
    d.text((4, 4), 'h%d (n%d)  gelb Video (von Hand)  rot vorher  tuerkis nachher' % (h, n), fill=(255, 255, 255))
    kacheln.append(t)
w, hh = kacheln[0].size
S = Image.new('RGB', (w * 3 + 8, hh * 2 + 4), (0, 0, 0))
for i, t in enumerate(kacheln): S.paste(t, ((i % 3) * (w + 4), (i // 3) * (hh + 4)))
S.save(aus, quality=90); print(aus, S.size)
