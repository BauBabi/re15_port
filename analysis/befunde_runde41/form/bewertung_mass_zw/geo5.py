#!/usr/bin/env python3
"""Querschnitte der Beine (Brett-Probe), Rumpfmass je Pose, Lagen im Raum.

Querschnitt: Knochenmesh in Ecken, Hauptachse = Laenge. Quer dazu die zwei Breiten, gemessen in den mittleren 60 Prozent der Laenge, als Median ueber Scheiben.
"""
import json, math, os, sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
import geo2 as G2

GLIEDER = {
 'a': ['fr_unter', 'fl_unter', 'nl_ober', 'nl_unter', 'nr_ober', 'nr_unter', 'fr_ober', 'hals'],
 'b': ['fr_unter', 'fl_unter', 'nl_unter', 'nr_unter', 'nl_innen', 'fr_fuss', 'nl_faust', 'nr_faust'],
 'c': ['fr_bein', 'fl_bein', 'nl_kugel_o', 'nl_kugel_u', 'nl_unter', 'nr_ober', 'nr_unter', 'nr_hand', 'kiefer'],
}


def quer(v, t):
    # Dichte Oberflaechenproben, nicht nur Ecken
    p, _ = G2.flaechen_proben(v, t, 1 / 400.0)
    c = p.mean(0)
    u, s, vt = np.linalg.svd(p - c, full_matrices=False)
    a = (p - c) @ vt[0]
    lo, hi = np.percentile(a, 2), np.percentile(a, 98)
    L = hi - lo
    z = []
    for q in np.linspace(.2, .8, 7):
        mit = lo + q * L
        sl = np.abs(a - mit) < L * 0.05
        if sl.sum() < 12:
            continue
        S = (p[sl] - c)
        S = S - np.outer(S @ vt[0], vt[0])
        u2, s2, v2 = np.linalg.svd(S - S.mean(0), full_matrices=False)
        w1 = np.ptp(S @ v2[0]); w2 = np.ptp(S @ v2[1])
        z.append((max(w1, w2), min(w1, w2)))
    z = np.array(z)
    return L, np.median(z[:, 0]), np.median(z[:, 1])


if __name__ == '__main__':
    for k in 'abc':
        f, _ = G.lade(k)
        nm = G.NAMEN[k]
        print('\n====', k.upper(), '  Glied: Laenge, Breite, Dicke, Dicke/Breite')
        vs = []
        for n in GLIEDER[k]:
            b = nm.index(n)
            v, _, t, _ = G.mesh_arrays(f, b)
            L, w, d = quer(v, t)
            vs.append(d / w)
            print(' %-11s L %5.0f  B %5.0f  D %5.0f  %.2f' % (n, L, w, d, d / w))
        print(' Median Dicke/Breite %.2f, kleinstes %.2f' % (np.median(vs), min(vs)))
