#!/usr/bin/env python3
"""Abnahme 1: Ton aus genau den Screenshot-Kameras, Tiefenpuffer, perspektivisch mit derselben Abbildung wie die Pruefung. Daneben Original aufgehellt. Kein RE-Wert."""
import os, sys, math
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from abn_netz import K, P, netz, pose, schreib
import form_buehne as FB

AUS = os.path.join(HIER, 'netz')
SK = 3


def welt(V, lage, gier):
    a = 2 * math.pi * gier / 4096.0
    s, c = math.sin(a), math.cos(a)
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    return V @ Y.T + np.array(lage, float)


def bildpunkte(W, kam):
    v = (kam.V.astype(float) / 4096.0) @ W.T + kam.t.astype(float)[:, None]
    u = 160 + kam.h * v[0] / v[2]
    w = 120 + kam.h * v[1] / v[2]
    x, y = FB.abbilden(u + FB.PHASE, w + FB.PHASE, kam.abbildung)
    return np.stack([x, y], 1), v[2]


def ton(px, z, F, W, B, breite, hoehe):
    zb = np.full((hoehe, breite), 1e18)
    bild = np.full((hoehe, breite), 0.0)
    kn = np.full((hoehe, breite), -1, int)
    t = px[F]
    d = (t[:, 1, 0] - t[:, 0, 0]) * (t[:, 2, 1] - t[:, 0, 1]) - (t[:, 2, 0] - t[:, 0, 0]) * (t[:, 1, 1] - t[:, 0, 1])
    n3 = np.cross(W[F[:, 1]] - W[F[:, 0]], W[F[:, 2]] - W[F[:, 0]])
    n3 /= np.linalg.norm(n3, axis=1, keepdims=True) + 1e-9
    L = np.array([.35, -.8, .45])
    L /= np.linalg.norm(L)
    for i in range(len(F)):
        if d[i] == 0:
            continue
        q = t[i]
        x0 = max(int(math.floor(q[:, 0].min())), 0); x1 = min(int(math.ceil(q[:, 0].max())) + 1, breite)
        y0 = max(int(math.floor(q[:, 1].min())), 0); y1 = min(int(math.ceil(q[:, 1].max())) + 1, hoehe)
        if x0 >= x1 or y0 >= y1:
            continue
        X, Y = np.meshgrid(np.arange(x0, x1) + .5, np.arange(y0, y1) + .5)
        w1 = ((X - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (Y - q[0, 1])) / d[i]
        w2 = ((q[1, 0] - q[0, 0]) * (Y - q[0, 1]) - (X - q[0, 0]) * (q[1, 1] - q[0, 1])) / d[i]
        w0 = 1 - w1 - w2
        inn = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        if not inn.any():
            continue
        tz = z[F[i]]
        zz = w0 * tz[0] + w1 * tz[1] + w2 * tz[2]
        neu = inn & (zz < zb[y0:y1, x0:x1])
        zb[y0:y1, x0:x1][neu] = zz[neu]
        bild[y0:y1, x0:x1][neu] = abs(float(n3[i] @ L))
        kn[y0:y1, x0:x1][neu] = B[i]
    return bild, kn


def main():
    k = P.Kandidat(K)
    n = netz(k)
    for nr, box in ((1, (180, 170, 445, 480)), (2, (135, 140, 480, 480))):
        lage, gier, clip, bild = k.stand(nr)
        kam = P.kamera(nr)
        V, F, B = pose(k, n, clip, bild)
        W = welt(V, lage, gier)
        px, z = bildpunkte(W, kam)
        # Vorderseiten wie im Spiel: Vorzeichen der Windung in Bildkoordinaten aus form_buehne, mac0 > 0 sichtbar
        a = px[F]
        m0 = (a[:, 1, 0] - a[:, 0, 0]) * (a[:, 2, 1] - a[:, 0, 1]) - (a[:, 2, 0] - a[:, 0, 0]) * (a[:, 1, 1] - a[:, 0, 1])
        sicht = m0 > 0
        bh, kn = ton(px * SK, z, F[sicht], W, B[sicht], 640 * SK, 480 * SK)
        g = ((.22 + .75 * bh) * 255).astype(np.uint8)
        ton_rgb = np.stack([g, g, g], -1)
        ton_rgb[kn < 0] = (70, 70, 70)
        # Knochengrenzen duenn einzeichnen
        gr = (kn != np.roll(kn, 1, 0)) | (kn != np.roll(kn, 1, 1))
        ton_rgb[gr & (kn >= 0)] = (255, 170, 60)
        x0, y0, x1, y1 = box
        t = ton_rgb[y0 * SK:y1 * SK, x0 * SK:x1 * SK]
        o = P.original(nr).astype(np.float32)
        h = 255 * np.power(np.clip(o / 255 * 2.2, 0, 1), .7)
        oo = cv2.resize(h[y0:y1, x0:x1].astype(np.uint8), None, fx=SK, fy=SK, interpolation=cv2.INTER_CUBIC)
        z2 = np.hstack([schreib(oo.copy(), 'Original hell'), schreib(t.copy(), 'Ton aus der Kamera von Bild %d, Linien = Knochengrenzen' % nr)])
        cv2.imwrite(os.path.join(AUS, 'kamton_bild%d.png' % nr), z2[:, :, ::-1])
        print('Bild', nr, 'sichtbar', int(sicht.sum()), 'von', len(F))


if __name__ == '__main__':
    main()
