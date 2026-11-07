#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 8: Pose gegen Gerippe je Knochen, Lueckenmass Schulter, Kopf auf Zug Q, Masken-Mass der Haende im Bild."""
import json, sys
import numpy as np
from m1_skelett import F, W, G, NAME, lade, fk, welt, FK
from m3_innen import pose_tris, proben, windung


def rot(a):
    a = np.array(a, float) * 2 * np.pi / 4096
    cx, sx, cy, sy, cz, sz = np.cos(a[0]), np.sin(a[0]), np.cos(a[1]), np.sin(a[1]), np.cos(a[2]), np.sin(a[2])
    return np.array([[1, 0, 0], [0, cx, -sx], [0, sx, cx]]) @ np.array([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]]) @ np.array([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]])


def main():
    rng = np.random.default_rng(4)
    for x in ['final']:
        j, f = lade(x)
        print('\n=========== Rohbau', x.upper())
        for c, nm, key in ((0, 'Pose 1', 'bild1'), (1, 'Pose 2', 'bild2')):
            b = f['clips'][c]['bilder'][0]
            w = f['pool'][b['kf']]['winkel']
            g = G['posen'][key]['winkel']
            d = []
            for i in range(30):
                r = rot(w[i]).T @ rot(g[i])
                d.append((float(np.degrees(np.arccos(np.clip((np.trace(r) - 1) / 2, -1, 1)))), NAME[i]))
            d.sort(reverse=True)
            print('  %s Abstand zur Gerippe-Pose in Grad: %s' % (nm, ', '.join('%s %.0f' % (n, a) for a, n in d[:9] if 'klaue' not in n)))
            print('     Wurzel: %s gegen %s' % (f['pool'][b['kf']]['wurzel'], G['posen'][key]['wurzel']))
        # Schulterluecke
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = fk(f, c)
            M = {i: pose_tris(f, R, T, i) for i in range(32)}
            z = []
            for a, b in ((10, 16), (10, 11), (0, 8), (0, 9), (0, 1), (10, 21)):
                qa = proben(*M[a], 6000, rng)
                qb = proben(*M[b], 6000, rng)
                d = 1e9
                for s in range(0, len(qa), 1000):
                    d = min(d, np.sqrt(((qa[s:s + 1000, None, :] - qb[None, :, :]) ** 2).sum(2)).min())
                inn = (windung(qb, *M[a]) > .5).mean()
                z.append('%s-%s Abstand %.0f, innen %.2f' % (NAME[a], NAME[b], d, inn))
            print('  %s: %s' % (nm, '; '.join(z)))
        # Kopf auf Zug Q
        kn = np.load(F + '/rohbau_final/mess/kn_%s_2.npy' % x)
        H, W_ = kn.shape
        yy, xx = np.mgrid[0:H, 0:W_]
        kreis = (xx - 325) ** 2 + (yy - 245) ** 2 <= 62 ** 2
        kopf = np.isin(kn, [21, 22, 23])
        print('  Kopf Bild 2: Kreis (325,245) r62 zu %.2f vom Kopf gedeckt; vom Kopf liegen %.2f im Kreis, %d px ausserhalb' % ((kopf & kreis).sum() / kreis.sum(), (kopf & kreis).sum() / max(kopf.sum(), 1), (kopf & ~kreis).sum()))
        # Handflaeche im Bild
        for n, c in ((1, 0), (2, 1)):
            R, T, k = fk(f, c)
            Rw, Tw = welt(j, n, R, T)
            km = FK.lade_kamera(n)
            for b, s in ((13, 'hand_l'), (18, 'hand_r')):
                print('  Bild %d %s Tiefe %.0f' % (n, s, float(np.atleast_1d(FK.tiefe(km, Tw[b]))[0])), end=';')
        print()


if __name__ == '__main__':
    main()
