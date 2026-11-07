#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 9: Masken-freie Probe. Umriss des Renders gegen Kanten im Original (Canny, ohne jede Nutzer-Maske). Vergleichswerte: Nutzer-Soll als Umriss und Verschiebung.
Dazu dunkle Keile ausserhalb aller Teile (O2, O3, O4) und Fundstelle Ring bei B."""
import sys
import numpy as np
import cv2
from m1_skelett import F, W, NAME
sys.path.insert(0, W + '/re15_port/tools')
import form_pruefung as P

LOGO = P.logo_maske()


def kanten(n, tief=30, hoch=80):
    o = P.original(n).astype(np.float32) @ P.YK
    y = cv2.GaussianBlur(o, (0, 0), 1.2).astype(np.uint8)
    e = cv2.Canny(y, tief, hoch) > 0
    return e


def kontur(m):
    q = P.geschlossen(m)
    k = q & ~P.enge(q, 1)
    return k


def mass(m, dt, box):
    k = kontur(m) & ~P.weite(LOGO, 3)
    x0, y0, x1, y1 = box
    k[:y0] = False
    k[y1:] = False
    k[:, :x0] = False
    k[:, x1:] = False
    d = dt[k]
    return float(np.mean(d)), float(np.median(d)), float(np.percentile(d, 90)), int(k.sum())


def main():
    for n, box in ((1, (180, 170, 450, 436)), (2, (190, 140, 480, 436))):
        e = kanten(n)
        dt = cv2.distanceTransform((~e).astype(np.uint8), cv2.DIST_L2, 5)
        soll = np.zeros((480, 640), bool)
        for t in P.teile_von(n):
            soll |= P.teil_roh(t)
        print('Bild %d: Kantenpixel %d. Abstand Umriss zur naechsten Bildkante: Mittel, Median, 90 %%, Laenge' % (n, int(e.sum())))
        print('   Nutzer-Soll        %.2f %.2f %.2f %d' % mass(soll, dt, box))
        print('   Soll +7 px rechts  %.2f %.2f %.2f %d' % mass(np.roll(soll, 7, 1), dt, box))
        for x in ['final']:
            kn = np.load(F + '/rohbau_final/mess/kn_%s_%d.npy' % (x, n))
            print('   Rohbau %s           %.2f %.2f %.2f %d' % (x.upper(), *mass(kn >= 0, dt, box)))
    # Keile: nur Bild 1
    y1 = P.original(1).astype(np.float32) @ P.YK
    alle = np.zeros((480, 640), bool)
    for t in P.teile_von(1):
        alle |= P.teil_roh(t)
    for nm, (x0, y0, x1, y1_) in (('O3 Keil links', (218, 345, 255, 445)), ('O1 rechts neben Arm', (405, 262, 429, 300))):
        m = np.zeros((480, 640), bool)
        m[y0:y1_ + 1, x0:x1 + 1] = True
        z = m & ~alle & (y1 < 95) & ~LOGO
        print(nm, 'dunkle Pixel ausserhalb aller Teile:', int(z.sum()), ' gedeckt:', ' '.join('%s %.2f' % (x.upper(), (np.load(F + '/rohbau_final/mess/kn_%s_1.npy' % x)[z] >= 0).mean()) for x in ['final']))
    # Ring B
    kn = np.load(F + '/rohbau_final/mess/kn_b_2.npy')
    w, c = np.unique(kn[300:320, 208:232], return_counts=True)
    print('Bild 2, B, Ausschnitt (208-232,300-320):', {NAME.get(int(a), 'leer'): int(b) for a, b in zip(w, c)})
    # Masken-Umriss Rendering ausserhalb aller Masken je Bild: wo steht Modell ohne Lesung?
    for n in (1, 2):
        soll = np.zeros((480, 640), bool)
        for t in P.teile_von(n):
            soll |= P.teil_roh(t)
        s3 = P.weite(soll, 3)
        print('Bild %d: Modellpixel ausserhalb aller Teile plus 3 px Saum:' % n, ' '.join('%s %d' % (x.upper(), int(((np.load(F + '/rohbau_final/mess/kn_%s_%d.npy' % (x, n)) >= 0) & ~s3 & ~LOGO).sum())) for x in ['final']))


if __name__ == '__main__':
    main()
