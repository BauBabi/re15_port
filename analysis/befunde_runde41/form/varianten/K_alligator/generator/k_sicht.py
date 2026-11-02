#!/usr/bin/env python3
"""Hilfsbild: Kennungskarte (Kopf gelb, Kiefer magenta, Hals cyan, Rest grau) mit den Umrissen der Nutzer-Teile Oberkiefer (gelb) und Unterkiefer (magenta)."""
import sys
import cv2
import numpy as np
import k_kern as KK


def bild(idb, n=2, box=(130, 130, 480, 480), s=2):
    im = cv2.imread(KK.os.path.join(KK.FORM, 'bild%d.png' % n))
    a = (im * 0.45).astype(np.uint8)
    a[idb >= 0] = (110, 110, 110)
    a[idb == 21] = (200, 200, 0)
    a[idb == 22] = (0, 200, 230)
    a[idb == 23] = (200, 0, 200)
    for nm, c in (('oberkiefer', (0, 255, 255)), ('unterkiefer', (255, 0, 255))) if n == 2 else ():
        m = KK.maske(nm).astype(np.uint8)
        cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(a, cs, -1, c, 1)
    z = cv2.imread(KK.os.path.join(KK.FORM, 'ziel2_bild%d.png' % n), 0)
    cs, _ = cv2.findContours((z == 255).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, cs, -1, (0, 255, 0), 1)
    x0, y0, x1, y1 = box
    return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)


if __name__ == '__main__':
    idb = np.load(sys.argv[1])
    n = int(sys.argv[3]) if len(sys.argv) > 3 else 2
    cv2.imwrite(sys.argv[2], bild(idb, n, (130, 130, 480, 480) if n == 2 else (240, 160, 480, 470)))
