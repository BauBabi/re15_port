"""SF V8 Runde 7b: Tafeln 14_schwanz_unterseite.png und 14b_tonne.png aus ECHTEN Framedumps (Spalten Videobild | V7 | Stand 6d | NEU).

    python tafel_r7b.py --v7 FD --r6d FD --neu FD --kl FD_KLASSEN_NEU --emd-6d EMD --emd-neu EMD --aus DIR

Tafel 14: Zeilen F264, F276 (Griff-Phase, "rote Klinge" des Sicht-Pruefers r6) und F528 / F564 / F600 (Seitwaertsgang; das Video endet bei
f1538 = F430, dort steht "kein Videobild"); darunter offline die Nahansicht der Unterseite von unten (PV, Modell 1800 E. hoeher), von
vorn (PV und P2, Gier + 1024) und von der anderen Seite (PV, Gier + 2048) fuer Stand 6d und NEU. Tafel 14b: F312-F324 (Wurf-Phase, "roter Kasten"). Ausschnitt je Bild = Huelle des
Schwanzes im Klassen-Marker-Lauf von NEU (glaette_r7b.py) plus Rand, fuer alle Spalten gleich; Videobild f = F + 1108 im selben Ausschnitt
(640 x 480 = Framedump / 1,5). Roh, nichts geglaettet. Kein RE-Wert.
"""
import os, sys, argparse
import numpy as np
import cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import glaette_r7b as GL              # noqa: E402

VIDEO = os.path.join(os.path.dirname(V.BAUM), 'r41_sichel', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')
HOEHE = 300


def fd(d, F):
    p = os.path.join(d, 'f%06d.ppm' % F)
    return np.asarray(Image.open(p).convert('RGB')) if os.path.exists(p) else None


def schrift(im, s, x=6, y=20, sc=0.55):
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (255, 255, 0), 1, cv2.LINE_AA)


def box_schwanz(kl_dir, neu_dir, F, rand=40):
    m, a = fd(kl_dir, F), fd(neu_dir, F)
    k = GL.klassen_bild(m, a)
    ys, xs = np.nonzero((k == 0) | (k == 1))
    if len(ys) < 30:
        ys, xs = np.nonzero(k >= 0)
    x0, x1, y0, y1 = xs.min() - rand, xs.max() + rand, ys.min() - rand, ys.max() + rand
    w, h = x1 - x0, y1 - y0
    s = max(w, h * 4 / 3)                                      # Seitenverhaeltnis 4:3
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    x0, x1 = int(max(cx - s / 2, 0)), int(min(cx + s / 2, 960)); y0, y1 = int(max(cy - s * 3 / 8, 0)), int(min(cy + s * 3 / 8, 720))
    return x0, y0, x1, y1


def kachel(im, box, titel, skala=1.0):
    x0, y0, x1, y1 = [int(round(v * skala)) for v in box]
    if im is None:
        t = np.zeros((HOEHE, int(HOEHE * 4 / 3), 3), np.uint8)
    else:
        t = np.ascontiguousarray(im[y0:y1, x0:x1])
        f = HOEHE / max(t.shape[0], 1)
        t = cv2.resize(t, (max(int(t.shape[1] * f), 1), HOEHE), interpolation=cv2.INTER_NEAREST)
    t = np.ascontiguousarray(t)
    schrift(t, titel)
    return t


def reihe(ks):
    h = max(k.shape[0] for k in ks)
    return np.hstack([np.vstack([k, np.zeros((h - k.shape[0], k.shape[1], 3), np.uint8)]) for k in ks]
                     + [np.zeros((h, 4, 3), np.uint8)])


def stapel(zs):
    w = max(z.shape[1] for z in zs)
    return np.vstack([np.hstack([z, np.zeros((z.shape[0], w - z.shape[1], 3), np.uint8)]) for z in zs])


def szenenzeilen(a, takte):
    zs = []
    for F in takte:
        bx = box_schwanz(a.kl, a.neu, F)
        pv = os.path.join(VIDEO, 'f%04d.png' % (F + 1108))
        vb = np.asarray(Image.open(pv).convert('RGB')) if os.path.exists(pv) else None
        zs.append(reihe([kachel(vb, bx, ('Video f%d' % (F + 1108)) if vb is not None else 'kein Videobild (f%d > f1538)' % (F + 1108), 1 / 1.5),
                         kachel(fd(a.v7, F), bx, 'V7 F%d' % F), kachel(fd(a.r6d, F), bx, 'Stand 6d F%d' % F),
                         kachel(fd(a.neu, F), bx, 'NEU F%d' % F)]))
    return zs


def offline(a):
    import f_rast as FR
    import r3_gemein as R3
    import tafeln_v8 as TV
    unten = dict(V.PV); unten['y'] = -1800
    vorn_pv = dict(V.PV); vorn_pv['gier'] = (V.PV['gier'] + 1024) % 4096
    vorn_p2 = dict(R3.P2); vorn_p2['gier'] = (R3.P2['gier'] + 1024) % 4096
    seite_pv = dict(V.PV); seite_pv['gier'] = (V.PV['gier'] + 2048) % 4096
    zs = []
    for nm, p in (('Stand 6d', a.emd_6d), ('NEU', a.emd_neu)):
        blob = open(p, 'rb').read()
        ks = []
        for an, pose in (('PV von unten', unten), ('PV von vorn (Gier+1024)', vorn_pv), ('P2 von vorn (Gier+1024)', vorn_p2),
                         ('PV andere Seite (Gier+2048)', seite_pv), ('PV', V.PV), ('P2', R3.P2)):
            mo = V.modell(open(a.emd_neu, 'rb').read())
            b = FR.zeichne(mo, pose, None, licht=False)['bone'][:480]
            ys, xs = np.nonzero((b >= 16) & (b <= 22))
            bx = (max(xs.min() - 10, 0), max(ys.min() - 10, 0), min(xs.max() + 10, 640), min(ys.max() + 10, 480))
            k = kachel(TV.render(blob, pose), bx, nm)
            schrift(k, an, 6, 40, 0.45)
            ks.append(k)
        zs.append(reihe(ks))
    return zs


def main():
    ap = argparse.ArgumentParser()
    for k in ('v7', 'r6d', 'neu', 'kl', 'emd-6d', 'emd-neu', 'aus'):
        ap.add_argument('--' + k, required=True)
    a = ap.parse_args()
    kopf = lambda im, s: np.vstack([np.full((28, im.shape[1], 3), 0, np.uint8), im])
    t14 = stapel(szenenzeilen(a, (264, 276, 528, 564, 600)) + offline(a))
    t14 = kopf(t14, ''); schrift(t14, 'SF V8 14 (Runde 7b): Schwanz-Unterseite - echte Framedumps Video | V7 | Stand 6d | NEU (F264, F276, Seitwaertsgang); '
                         'offline Nahansicht Stand 6d / NEU (roh)', 6, 19, 0.5)
    cv2.imwrite(os.path.join(a.aus, '14_schwanz_unterseite.png'), t14[:, :, ::-1])
    t14b = stapel(szenenzeilen(a, range(312, 325)))
    t14b = kopf(t14b, ''); schrift(t14b, 'SF V8 14b (Runde 7b): Schwanzwurzel F312-F324 - echte Framedumps Video | V7 | Stand 6d | NEU (roh)', 6, 19, 0.5)
    cv2.imwrite(os.path.join(a.aus, '14b_tonne.png'), t14b[:, :, ::-1])
    print('geschrieben', a.aus)


if __name__ == '__main__':
    main()
