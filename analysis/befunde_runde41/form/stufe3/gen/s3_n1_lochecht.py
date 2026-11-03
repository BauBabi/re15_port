"""Nachbesserung 1, B: Loch-Stichprobe im echten Fenster (nach stufe3/abnahme1/werkzeug/loch_echt.py).

    python s3_n1_lochecht.py AUS.png

Je Fall (ROOM5080 Schnitt 4, RE15_BB_POSE -23500,-18200, Gier, Clip, Bild): echtes Fenster vorher (Lauf der Abnahme 1,
re15_port/build/abn1_spiel/<fall>/bild/b_0020.png, Asset J) und nachher (re15_port/build_n1a/spiel/<fall>, K3-Asset), dazu die Buehne
(form_buehne, gleiche Kamera und Lage) des jeweiligen Assets: geschlossene Hintergrundinseln >= 12 px (640 x 480) magenta eingezeichnet.
"""
import os, sys
import numpy as np
from scipy import ndimage
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_buehne as B     # noqa: E402

BAUM = S.WURZEL
VOR = (os.path.join(HIER, '..', 'roh', 'n1_vorher', 'BAHNSTEIG_FORM.EMD'), os.path.join(HIER, '..', 'roh', 'n1_vorher', 'BAHNSTEIG_FORM.TIM'),
       os.path.join(BAUM, 're15_port', 'build', 'abn1_spiel'))
NACH = (os.path.join(HIER, '..', 'asset', 'BAHNSTEIG_FORM.EMD'), os.path.join(HIER, '..', 'asset', 'BAHNSTEIG_FORM.TIM'),
        os.path.join(BAUM, 're15_port', 'build_n1a', 'spiel'))
FAELLE = [('loch_5_28_2048', 5, 28, 2048, 'Achsel'), ('loch_5_28_0', 5, 28, 0, 'Huefte'), ('loch_10_100_2048', 10, 100, 2048, 'Achsel')]


def inseln(emd, tim, c, b, g, sch=4):
    R = B.Raum('5080')
    s = R.schnitt(sch)
    kam = B.Kamera.aus_rid(s['pos'], s['ziel'], s['fov'])
    r, m = B.buehne(emd, tim, c, b, (-23500, 0, -18200), g, kam, R.licht(sch), None)
    h = ndimage.binary_fill_holes(m) & ~m
    lab, k = ndimage.label(h)
    gr = ndimage.sum(h, lab, range(1, k + 1)) if k else []
    gross = [int(v) for v in gr if v >= 12]
    keep = np.isin(lab, [i + 1 for i, v in enumerate(gr) if v >= 12]) if k else np.zeros_like(h)
    return m, keep, sorted(gross, reverse=True)


def main():
    aus = sys.argv[1]
    tafeln, zahlen = [], []
    for name, c, b, g, was in FAELLE:
        reihe = []
        for tag, (e, t, lauf) in (('vorher J', VOR), ('nachher K3', NACH)):
            emd, tim = open(e, 'rb').read(), open(t, 'rb').read()
            m, h, gr = inseln(emd, tim, c, b, g)
            echt = np.asarray(Image.open(os.path.join(lauf, name, 'bild', 'b_0020.png')).convert('RGB'))
            mark = echt.copy()
            rand = h & ~ndimage.binary_erosion(h, iterations=2)
            mark[rand] = (255, 0, 255)
            reihe.append((tag, echt, mark, gr, m | h))
            zahlen.append((name, tag, gr))
        mm = reihe[0][4] | reihe[1][4]
        ys, xs = np.nonzero(mm)
        y0, y1, x0, x1 = max(ys.min() - 10, 0), min(ys.max() + 10, 480), max(xs.min() - 10, 0), min(xs.max() + 10, 640)
        txt = 'Clip %d Bild %d Gier %d (%s), ROOM5080 Schnitt 4. Inseln >= 12 px: vorher %s, nachher %s' % (
            c, b, g, was, reihe[0][3] or 'keine', reihe[1][3] or 'keine')
        tafeln.append((txt, np.hstack([reihe[0][1][y0:y1, x0:x1], reihe[0][2][y0:y1, x0:x1], reihe[1][1][y0:y1, x0:x1], reihe[1][2][y0:y1, x0:x1]])))
    W = max(t[1].shape[1] for t in tafeln)
    H = sum(t[1].shape[0] + 22 for t in tafeln) + 24
    out = Image.new('RGB', (W, H), (18, 18, 22))
    d = ImageDraw.Draw(out)
    d.text((4, 4), 'Nachbesserung 1, M2: echtes Fenster je Fall: vorher (J) | vorher mit Inseln der Buehne | nachher (K3) | nachher mit Inseln (magenta)',
           fill=(240, 200, 120))
    y = 24
    for t, a in tafeln:
        d.text((4, y + 3), t, fill=(220, 220, 220))
        out.paste(Image.fromarray(a), (0, y + 20))
        y += a.shape[0] + 22
    out.save(aus)
    for z in zahlen:
        print(z)


if __name__ == '__main__':
    main()
