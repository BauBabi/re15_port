#!/usr/bin/env python3
"""Hilfsblatt: die groessten Nahtloecher eines Messlaufs (k_loecher JSON) als Kennungsbild, Loch rot. python k_lochblick.py EMD JSON AUS.png [N] [Naht]"""
import json
import sys

import cv2
import numpy as np

import k_loecher as KL


def blatt(emd, js, aus, n=8, nur=None):
    fig = KL.Figur(emd)
    R = json.load(open(js, encoding='utf-8'))
    L = []
    for c in R['clips']:
        for b in c['bilder']:
            for l in b['loecher']:
                if l['naht'] and (nur is None or l['naht'] == nur):
                    L.append((l['px'], c['clip'], b['bild'], b['ansicht'], l['naht']))
    L.sort(reverse=True)
    V = {a[0]: a for a in KL.ANSICHTEN}
    out = []
    for px, c, b, an, nm in L[:n]:
        _, az, el = V[an]
        idb, piv, _ = KL.rendern(fig, fig.kf(c, b), KL.blick(az, el), gross=900, s=0.15)
        Lh = KL.loecher(idb, piv, s=0.15)
        img = np.full(idb.shape + (3,), 90, np.uint8)
        lut = np.random.RandomState(3).randint(60, 230, (40, 3)).astype(np.uint8)
        img[idb >= 0] = lut[idb[idb >= 0]]
        for l in Lh:
            x, y, w, h = l['box']
            sub = idb[y:y + h, x:x + w] < 0
            img[y:y + h, x:x + w][sub] = (0, 0, 255) if l['naht'] else (0, 200, 255)
            if l['naht']:
                cv2.putText(img, '%s %d' % (l['naht'], l['px']), (x, max(y - 3, 12)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        cv2.putText(img, 'Clip %d Bild %d %s' % (c, b, an), (5, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)
        out.append(cv2.resize(img, (450, 450)))
    while len(out) % 4:
        out.append(np.full((450, 450, 3), 40, np.uint8))
    cv2.imwrite(aus, np.vstack([np.hstack(out[i:i + 4]) for i in range(0, len(out), 4)]))


if __name__ == '__main__':
    blatt(sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]) if len(sys.argv) > 4 else 8, sys.argv[5] if len(sys.argv) > 5 else None)
