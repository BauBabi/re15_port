#!/usr/bin/env python3
"""R41 Spur SF, V8 Runde 6a (P6): Kacheln zu Riegel S17 (g) - Keyframes, in denen die neue Hand im Koerper steckt und die alte nicht.
Je Keyframe eine Zeile V8 r5d | NEU, Spielkamera 50A0 am Standort von P1, zwei Blickrichtungen (Gier von P1 und + 1024), roh x3.

  python durchdringung_kacheln.py --r5d <EM30 r5d> --kf 15/105 10/18 --aus <png>
"""
import os, sys, argparse
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import hand_mass as HM                # noqa: E402
import tafeln_v8 as TV                # noqa: E402
import f_rast as FR                   # noqa: E402

KANTE, ZOOM = 150, 3                  # Darstellung, kein Messwert


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', default=V.ASSET)
    ap.add_argument('--r5d', required=True)
    ap.add_argument('--kf', nargs='+', required=True)
    ap.add_argument('--aus', required=True)
    a = ap.parse_args()
    blobs = [('V8 r5d', open(a.r5d, 'rb').read()), ('NEU', open(a.neu, 'rb').read())]
    zeilen = []
    for kf in a.kf:
        c, b = (int(x) for x in kf.split('/'))
        ks = []
        for dg in (0, 1024):
            pose = dict(HM.POSEN['P1'], clip=c, bild=b, gier=(HM.POSEN['P1']['gier'] + dg) % 4096)
            box = None
            for nm, blob in blobs:
                A = FR.mit_bg(V.modell(blob), pose)
                if box is None:
                    ys, xs = np.nonzero(A['bone'][:480] == 12)
                    cx, cy = (xs.mean(), ys.mean()) if len(xs) else (320, 240)
                    x0 = int(min(max(cx - KANTE / 2, 0), 640 - KANTE)); y0 = int(min(max(cy - KANTE / 2, 0), 480 - KANTE))
                    box = (x0, y0, x0 + KANTE, y0 + KANTE)
                ks.append(TV.kachel(A['img'][:480], box, ZOOM, '%s Clip %d Bild %d Gier +%d' % (nm, c, b, dg)))
        zeilen.append(TV.reihe(ks))
    TV.speichern(TV.kopf(np.vstack(zeilen), 'SF V8 S17 (g): Hand im Koerper - V8 r5d | NEU je Keyframe, zwei Blickrichtungen (roh, x%d)' % ZOOM), a.aus)


if __name__ == '__main__':
    main()
