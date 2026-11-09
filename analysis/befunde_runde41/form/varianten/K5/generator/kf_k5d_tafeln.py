#!/usr/bin/env python3
"""K5d Tafeln (roh, kein Gamma; gleiche Szenenkette fuer alle Spalten):
    python kf_k5d_tafeln.py <K5d-Ordner> <aus-Ordner>
f1_bild1.png / f2_bild2.png: Spalten Vorlage | Asset K5c | K5d neu; Zeile 1 Disc-Licht x k ohne Filter (Vorlage: Video-Standbild),
  Zeile 2 mit Videofilter (Saat 15).
f3_details_nah.png: je Nutzerpunkt ein Ausschnitt (x3, nearest) Vorlage | Asset | neu: Schwanzspitze, Gelenke (Bild 1), Buckelstelle, Krallen vorn
  (Bild 2, Kette mit Videofilter und ohne), dazu Lippe/Kopf nah (Clip 2 Ruhe und Clip 1 Maul offen, weisses Licht wie Tafel 06/m2) Asset | neu.
"""
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import riegel_k5d as R  # noqa: E402

v4f = R.v4f
SCHRIFT = cv2.FONT_HERSHEY_SIMPLEX


def beschriften(im, text, hoehe=22):
    k = np.full((hoehe, im.shape[1], 3), 24, np.uint8)
    cv2.putText(k, text, (4, hoehe - 7), SCHRIFT, 0.45, (235, 235, 235), 1, cv2.LINE_AA)
    return np.vstack([k, im])


def kachel(im, box, f=3):
    x0, y0, x1, y1 = box
    a = im[y0:y1, x0:x1]
    return cv2.resize(a, ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)


def reihe(bilder, titel, rand=4):
    h = max(b.shape[0] for b in bilder)
    out = []
    for b, t in zip(bilder, titel):
        b = beschriften(b, t)
        if b.shape[0] < h + 22:
            b = np.vstack([b, np.full((h + 22 - b.shape[0], b.shape[1], 3), 24, np.uint8)])
        out += [b, np.full((b.shape[0], rand, 3), 24, np.uint8)]
    return np.hstack(out[:-1])


def stapeln(reihen, kopf):
    w = max(r.shape[1] for r in reihen)
    rr = [beschriften(np.zeros((0, w, 3), np.uint8), kopf, 26)]
    for r in reihen:
        if r.shape[1] < w:
            r = np.hstack([r, np.full((r.shape[0], w - r.shape[1], 3), 24, np.uint8)])
        rr += [r, np.full((6, w, 3), 24, np.uint8)]
    return np.vstack(rr)


def speichern(p, rgb):
    cv2.imwrite(p, cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR))


def spiel(sp, aus):
    """f4: echtes Spielfenster (gdigrab, kf_k5d_spiel.sh): Original-Standbild | Asset heute | K5d neu, je Pose Bild 1 / Bild 2 und Ruhe."""
    lade = lambda p: cv2.cvtColor(cv2.imread(os.path.join(sp, p)), cv2.COLOR_BGR2RGB)
    leer = np.full((480, 640, 3), 24, np.uint8)
    z = []
    for titel, n, datei in (('5080 Schnitt 1, Nachstellung Bild 1 (Clip 0 Bild 11)', 1, 'pose1_nachstellung_r5080.png'),
                            ('5090 Schnitt 11, Pose Bild 2 (Clip 12 Bild 0)', 2, 'pose2_r5090_s11.png'),
                            ('5080 Schnitt 1, Ruhe (Clip 2)', 0, 'ruhe_r5080_s1.png'),
                            ('5090 Schnitt 11, Ruhe (Clip 2)', 0, 'ruhe_r5090_s11.png')):
        z.append(reihe([v4f.original(n) if n else leer, lade('heute_' + datei), lade('k5d_' + datei)],
                       ['Original-Standbild Bild %d' % n if n else '(kein Original-Standbild)', 'Asset heute (K5c): ' + titel, 'K5d neu: ' + titel]))
    speichern(os.path.join(aus, 'f4_spiel.png'), stapeln(z, 'K5d im echten Spielfenster (gdigrab, kein AUTOSHOT/SOFTWARE_RENDER, Variante ueber '
                                                           'RE15_RE2_ASSET_ROOT): Original | Asset heute | K5d neu, roh'))


def main():
    if sys.argv[1] == '--spiel':
        spiel(sys.argv[2], sys.argv[3])
        return
    neu, aus = sys.argv[1], sys.argv[2]
    os.makedirs(aus, exist_ok=True)
    st = {'Asset K5c': R.Stand(R.BASIS), 'K5d neu': R.Stand(neu)}
    vor = {n: v4f.original(n) for n in (1, 2)}
    for n, datei in ((1, 'f1_bild1.png'), (2, 'f2_bild2.png')):
        z1 = reihe([vor[n]] + [st[k].d[n]['sauber'] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] +
                   ['%s: Disc-Licht x k, ohne Filter, roh' % k for k in st])
        z2 = reihe([vor[n]] + [st[k].d[n]['video'][15] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] +
                   ['%s: Disc-Licht x k + Videofilter Saat 15, roh' % k for k in st])
        speichern(os.path.join(aus, datei), stapeln([z1, z2], 'K5d Feinschliff (Nutzer 2026-11-08), Bild %d: Vorlage | Asset K5c (Spiel seit E30) | K5d neu. '
                                                       'Gleiche Szenenkette je Zeile, kein Gamma.' % n))
    # f3 Details nah
    zeilen = []
    fall = [('Schwanzspitze (Bild 1, Mesh 6, Fleck F15)', 1, (205, 380, 345, 480), 'video'),
            ('Gelenke Beine/Arme (Bild 1: Huefte, Knie, Ellbogen, Hand r)', 1, (190, 250, 440, 400), 'video'),
            ('Buckelstelle vorn (Bild 2, Flecken F01/F02)', 2, (240, 140, 380, 230), 'video'),
            ('Schultern vorn (Bild 2, F05/F06)', 2, (200, 190, 440, 290), 'video'),
            ('Krallen vorn (Bild 2), Kette mit Videofilter', 2, (150, 290, 500, 480), 'video'),
            ('Krallen vorn (Bild 2), Disc-Licht ohne Filter', 2, (150, 290, 500, 480), 'sauber')]
    for titel, n, box, art in fall:
        f = 3 if (box[2] - box[0]) <= 150 else 2
        bilder = [kachel(vor[n], box, f)]
        for k in st:
            im = st[k].d[n]['video'][15] if art == 'video' else st[k].d[n]['sauber']
            bilder.append(kachel(im, box, f))
        zeilen.append(reihe(bilder, ['Vorlage: ' + titel, 'Asset K5c', 'K5d neu']))
    # Lippe / Kopf nah, weisses Licht (rundum wie m2), Clip 2 Ruhe und Clip 1 Maul offen
    kopf = []
    for clip, az, el in ((2, 60, 10), (2, 90, 0), (2, 120, 10), (1, 60, 10), (1, 90, 0)):
        for k, o in (('Asset K5c', R.BASIS), ('K5d neu', neu)):
            a, _K = v4f.rundum_bild(o, clip, 0, az, el, 300, teile=(22, 23))
            kopf.append((a, '%s Clip %d %s, %d/%d' % (k, clip, 'Ruhe' if clip == 2 else 'Maul offen', az, el)))
    zeilen.append(reihe([a for a, _t in kopf[:6]], [t for _a, t in kopf[:6]]))
    zeilen.append(reihe([a for a, _t in kopf[6:]], [t for _a, t in kopf[6:]]))
    # rundum ganz
    rund = []
    for az, el in ((60, 15), (-120, 20), (0, 5)):
        for k, o in (('Asset K5c', R.BASIS), ('K5d neu', neu)):
            a, _K = v4f.rundum_bild(o, 2, 0, az, el, 380)
            rund.append((a, '%s rundum Clip 2, %d/%d' % (k, az, el)))
    zeilen.append(reihe([a for a, _t in rund], [t for _a, t in rund]))
    speichern(os.path.join(aus, 'f3_details_nah.png'), stapeln(zeilen, 'K5d Details nah: Vorlage | Asset K5c | K5d neu (Ausschnitte x2/x3 nearest, '
                                                                       'Kette wie f1/f2, roh). Kopf/rundum: weisses Licht 0,46+0,86 N.L, roh.'))
    print('Tafeln fertig:', aus)


if __name__ == '__main__':
    main()
