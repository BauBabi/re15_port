"""Runde 41 SF NB2: Kopf-Vergleich Video | Port (Buehne, Lage/Gier/Clip-Bild wie in stoer.json) mit Kopf-Mesh-9-Umriss.
    python kopfblatt.py <emd> <stoer.json> <schluessel> <aus.png> [<emd2> <schluessel2> ...]"""
import json, os, sys
import numpy as np
from PIL import Image, ImageDraw
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import portmaske as PM, umriss as U, lupe as L   # noqa: E402
import birkin_buehne as BB                        # noqa: E402

FENSTER = tuple(int(v) for v in os.environ.get('FENSTER', '330,120,470,260').split(','))
ZOOM = int(os.environ.get('ZOOM', '3'))
TITEL = os.environ.get('TITEL', 'vorher 3799d7e94,nachher,3,4').split(',')


def port_bild(blob, s, kopf_umriss=True):
    mo = BB.modell(blob)
    bild, m = PM.maske(mo, s['clip'], s['bild'], s['gier'], s['x'], s['z'], hintergrund=U.hintergrund().astype(np.uint8))
    if kopf_umriss:
        _b, k = PM.maske(mo, s['clip'], s['bild'], s['gier'], s['x'], s['z'], nur={9}, licht=False)
        sicht = k & m
        # sichtbarer Teil des Kopfes: Pixel, die sich aendern, wenn Mesh 9 fehlt
        ohne, _m2 = PM.maske(mo, s['clip'], s['bild'], s['gier'], s['x'], s['z'], hintergrund=U.hintergrund().astype(np.uint8),
                             nur=set(range(len(mo.meshes))) - {9})
        vis = np.abs(bild.astype(int) - ohne.astype(int)).sum(2) > 0
        rand = k & ~ndimage.binary_erosion(k, iterations=1)
        bild = bild.copy(); bild[rand & ~vis] = (0, 140, 140); bild[rand & vis] = (0, 255, 255)
    return bild


if __name__ == '__main__':
    a = sys.argv[1:]
    emd, stoer, key, aus = a[0], a[1], a[2], a[3]
    extra = [(a[i], a[i + 1]) for i in range(4, len(a), 2)]
    St = json.load(open(stoer))
    x0, y0, x1, y1 = FENSTER
    zeilen = []
    for n in sorted(int(k) for k in St[key]):
        tiles = [L.lupe(n, x0, y0, x1, y1, ZOOM)]
        for e, k in [(emd, key)] + extra:
            s = St[k][str(n)]
            tiles.append(L.lupe(n, x0, y0, x1, y1, ZOOM, bild=port_bild(open(e, 'rb').read(), s)))
        w = Image.new('RGB', (sum(t.size[0] + 6 for t in tiles), tiles[0].size[1] + 16), (30, 30, 30))
        d = ImageDraw.Draw(w); xx = 0
        for i, t in enumerate(tiles):
            w.paste(t, (xx, 16)); d.text((xx + 4, 2), ('Video n%d h%d' % (n, n - 1350)) if i == 0 else TITEL[i - 1] + ' (Kopf Mesh 9 tuerkis)', fill=(255, 255, 0)); xx += t.size[0] + 6
        zeilen.append(w)
    W = Image.new('RGB', (zeilen[0].size[0], sum(z.size[1] for z in zeilen)))
    yy = 0
    for z in zeilen: W.paste(z, (0, yy)); yy += z.size[1]
    W.save(aus); print(aus, W.size)
