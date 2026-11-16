"""tafel_r7c.py - Tafeln der Runde 7c (P5-Einbau, K-a..K-e) aus ECHTEN Framedumps (lauf_v4.sh, 960x720), roh (kein Glaetten, Vergroesserung
nur durch Pixelwiederholung).  python tafel_r7c.py <aus.png> <bilder f1,f2,..> <name=laufordner> ... [--kasten x0,y0,w,h] [--zoom k] [--kopf]

Ohne --kasten gilt je Bild der feste Ausschnitt 240 x 180 aus KAESTEN (um Kopf und Nacken, an V7/R3 gewaehlt), sonst die Bildmitte."""
import os, sys
import numpy as np
from PIL import Image, ImageDraw

KAESTEN = {278: (500, 190, 240, 180), 310: (500, 190, 240, 180), 311: (520, 190, 240, 180), 312: (540, 200, 240, 180),
           313: (540, 200, 240, 180), 344: (520, 170, 240, 180), 354: (500, 180, 240, 180), 362: (480, 190, 240, 180),
           364: (480, 190, 240, 180), 348: (500, 180, 240, 180)}


def bild(ordner, f):
    p = os.path.join(ordner, 'fd', 'f%06d.ppm' % f)
    if not os.path.exists(p):
        p = os.path.join(ordner, 'f%06d.ppm' % f)
    return np.asarray(Image.open(p).convert('RGB'))


def main():
    a = sys.argv[1:]
    aus, bilder = a[0], [int(x) for x in a[1].split(',')]
    kasten = zoom = None
    laeufe = []
    i = 2
    while i < len(a):
        if a[i] == '--kasten':
            kasten = tuple(int(x) for x in a[i + 1].split(',')); i += 2
        elif a[i] == '--zoom':
            zoom = int(a[i + 1]); i += 2
        else:
            n, o = a[i].split('=', 1); laeufe.append((n, o)); i += 1
    z = zoom or 2
    zeilen = []
    for n, o in laeufe:
        reihe = []
        for f in bilder:
            x0, y0, w, h = kasten or KAESTEN.get(f, (360, 270, 240, 180))
            c = bild(o, f)[y0:y0 + h, x0:x0 + w]
            c = np.kron(c, np.ones((z, z, 1), np.uint8))
            im = Image.fromarray(c); d = ImageDraw.Draw(im)
            d.rectangle((0, 0, 92, 11), fill=(0, 0, 0)); d.text((2, 0), '%s F%d' % (n, f), fill=(255, 255, 255))
            reihe.append(np.asarray(im)); reihe.append(np.full((c.shape[0], 4, 3), 255, np.uint8))
        zeilen.append(np.concatenate(reihe[:-1], 1)); zeilen.append(np.full((4, zeilen[-1].shape[1], 3), 255, np.uint8))
    Image.fromarray(np.concatenate(zeilen[:-1], 0)).save(aus)
    print(aus)


if __name__ == '__main__':
    main()
