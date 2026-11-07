#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 6: Lupe auf Stellen. Rand-Schleifen je Knochen (wo sitzen sie) und Nahblicke aus freien Richtungen mit Form-Licht."""
import sys
import numpy as np
import cv2
from m1_skelett import F, W, NAME, lade, fk
from m3_innen import tris_bone
from m6_loecher import rand_kanten
sys.path.insert(0, W + '/re15_port/tools')
import form_pruefung as P
import form_buehne as FB


def schleifen(x):
    j, f = lade(x)
    for i in range(32):
        v, t = tris_bone(f, i)
        if not len(t):
            continue
        s, inv = np.unique(np.round(v).astype(int), axis=0, return_inverse=True)
        tt = inv.reshape(-1)[t]
        tt = tt[(tt[:, 0] != tt[:, 1]) & (tt[:, 1] != tt[:, 2]) & (tt[:, 0] != tt[:, 2])]
        r = np.array(rand_kanten(tt), int).reshape(-1, 2)
        if not len(r):
            continue
        # Komponenten
        par = {}
        def fnd(a):
            while par.setdefault(a, a) != a:
                par[a] = par[par[a]]
                a = par[a]
            return a
        for a, b in r:
            par[fnd(int(a))] = fnd(int(b))
        gr = {}
        for a in list(par):
            gr.setdefault(fnd(a), []).append(a)
        z = []
        for g in gr.values():
            q = s[g]
            z.append('%d Ecken Mitte (%.0f,%.0f,%.0f) Ausdehnung (%.0f,%.0f,%.0f)' % (len(g), *q.mean(0), *(q.max(0) - q.min(0))))
        print('  %-11s %s' % (NAME[i], ' | '.join(z)))


def blick(x, clip, az, el, gross, ziel):
    k = P.Kandidat(F + '/urteil_mass/streng_%s/form.json' % x)
    kf = k.kf(clip, 0)
    a = np.full((gross, gross, 3), 92, np.uint8)
    m = FB.rastern(P.rundum_dreiecke(k.mo, kf, az, el, gross), k.tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    h = a.copy()
    h[m] = np.clip(np.power(a[m] / 255.0, 0.5) * 255, 0, 255).astype(np.uint8)
    cv2.imwrite(ziel, h[:, :, ::-1])


if __name__ == '__main__':
    for x in 'bc':
        print('Rohbau', x.upper(), 'offene Schleifen im Knochenraum:')
        schleifen(x)
    blick('b', 0, 90, 5, 1100, F + '/urteil_mass/lupe_b_p1_vorn.png')
    blick('b', 0, 180, 5, 1100, F + '/urteil_mass/lupe_b_p1_links.png')
    blick('b', 1, 0, 5, 1100, F + '/urteil_mass/lupe_b_p2_rechts.png')
    blick('c', 1, 0, 5, 1100, F + '/urteil_mass/lupe_c_p2_rechts.png')
    blick('a', 1, 0, 5, 1100, F + '/urteil_mass/lupe_a_p2_rechts.png')
