#!/usr/bin/env python3
"""Tafel 10 der K5g3-Geometrie (roh): K5e_f1 | K5g3-Geometrie | Unterschied, je in Bild 1 (Clip 0 Bild 11), Bild 2 (Clip 12 Bild 0) und
spiel5080_vorn Clip 12 Bild 0; dazu Ausschnitte rechte Hand (Bild 2) und Kopf/Nacken (Bild 1 und spiel5080_vorn) vergroessert.

Bilder = Buehne des Ports (re15_port/tools/form_buehne.py), weisses Licht, Textur K5e_f1 fuer beide Modelle, keine Aufhellung.
Unterschied: weiss = Bildpunkt anders (Farbabstand > 24), sonst das K5g3-Bild abgedunkelt.

  python kf_k5g3_tafel.py [<ziel.png>]     -> K5/k5g/K5g3/tafeln/10_geometrie.png
"""
import os
import sys

sys.dont_write_bytecode = True
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5g3_geo as K  # noqa: E402

G, FE = K.G, K.FE
FB = G.FB
TIM0 = open(K.GEO_BASIS_TIM, 'rb').read()
E0 = open(K.GEO_BASIS, 'rb').read()
EG = open(K.GEO3_ZIEL, 'rb').read()
AN = G.ansichten()
GRAU = 20
ZIEL = os.path.join(K.K5, 'k5g', 'K5g3', 'tafeln', '10_geometrie.png')
SZENEN = (('bild1', 0, 11, 'Bild 1 (Clip 0 Bild 11)'), ('bild2', 12, 0, 'Bild 2 (Clip 12 Bild 0)'),
          ('spiel5080_vorn', 12, 0, 'Spiel 5080 von vorn, Clip 12 Bild 0'))
HAND_R = (18, 19, 20, 29, 31)
KOPF = (10, 16, 21, 22, 23)


def bild(emd, name, clip, b):
    kam, lage, gier, _H, _W, _ab = AN[name]
    img, _m = FB.buehne(emd, TIM0, clip, b, lage, gier, kam, licht=None)
    return np.asarray(img)[:, :, :3].copy()


def teilkarte(emd, name, clip, b):
    mo = FE.modell(emd, TIM0)
    cb, R, T = G.posen(mo)
    kam, lage, gier, H, W, ab = AN[name]
    i = [j for j, (c, bb, _k) in enumerate(cb) if c == clip and bb == b][0]
    tris = G.dreiecke_teile(mo, R[i], T[i], lage, gier, kam, teile=tuple(range(len(mo.meshes))))
    return G.keil_raster(tris, H, W, ab)[1]


def kasten(karten, teile, rand=12):
    m = np.zeros(karten[0].shape, bool)
    for k in karten:
        m |= np.isin(k, teile)
    ys, xs = np.where(m)
    H, W = m.shape
    return (max(int(xs.min()) - rand, 0), max(int(ys.min()) - rand, 0), min(int(xs.max()) + rand + 1, W), min(int(ys.max()) + rand + 1, H))


def zoom(img, box, ziel=300):
    x0, y0, x1, y1 = box
    f = max(1, min(6, ziel // max(x1 - x0, y1 - y0, 1)))
    return cv2.resize(img[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST), f


def titel(img, t, h=20, gross=0.45):
    b = max(img.shape[1], cv2.getTextSize(t, cv2.FONT_HERSHEY_SIMPLEX, gross, 1)[0][0] + 10)
    o = np.full((img.shape[0] + h, b, 3), GRAU, np.uint8)
    o[h:, :img.shape[1]] = img
    cv2.putText(o, t, (4, h - 6), cv2.FONT_HERSHEY_SIMPLEX, gross, (255, 255, 255), 1, cv2.LINE_AA)
    return o


def reihe(bl):
    h = max(b.shape[0] for b in bl)
    aus = []
    for b in bl:
        aus += [np.concatenate([b, np.full((h - b.shape[0], b.shape[1], 3), GRAU, np.uint8)], 0), np.full((h, 6, 3), 255, np.uint8)]
    return np.concatenate(aus[:-1], 1)


def stapel(zs):
    w = max(z.shape[1] for z in zs)
    return np.concatenate([np.concatenate([z, np.full((z.shape[0], w - z.shape[1], 3), GRAU, np.uint8)], 1) for z in zs], 0)


def unterschied(a, b):
    d = np.abs(a.astype(int) - b.astype(int)).sum(2) > 24
    o = (b * 0.35).astype(np.uint8)
    o[d] = 255
    return o, int(d.sum())


def main(ziel=ZIEL):
    zeilen, bericht = [], []
    for name, c, b, t in SZENEN:
        a, g = bild(E0, name, c, b), bild(EG, name, c, b)
        u, n = unterschied(a, g)
        zeilen.append(titel(reihe([titel(a, 'K5e_f1 (heute im Spiel)'), titel(g, 'K5g3-Geometrie (Textur K5e_f1)'),
                                   titel(u, 'Unterschied: %d Bildpunkte weiss' % n)]), t, 26, 0.55))
        bericht.append('%s: %d Bildpunkte anders' % (t, n))
        ka, kg = teilkarte(E0, name, c, b), teilkarte(EG, name, c, b)
        teile, wort = (HAND_R, 'rechte Hand') if name == 'bild2' else (KOPF, 'Kopf/Nacken/Schulter')
        box = kasten([ka, kg], teile)
        za, f = zoom(a, box)
        zg, _f = zoom(g, box)
        zu, _f = zoom(u, box)
        zeilen.append(titel(reihe([titel(za, 'K5e_f1'), titel(zg, 'K5g3'), titel(zu, 'Unterschied')]),
                            '%s - Ausschnitt %s, %d-fach' % (t, wort, f), 26, 0.55))
    img = stapel(zeilen)
    os.makedirs(os.path.dirname(ziel), exist_ok=True)
    cv2.imwrite(ziel, cv2.cvtColor(img, cv2.COLOR_RGB2BGR))
    print('Tafel', ziel.replace(chr(92), '/'), img.shape[1], 'x', img.shape[0])
    for z in bericht:
        print(' ', z)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else ZIEL)
