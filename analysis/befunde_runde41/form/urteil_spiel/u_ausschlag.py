# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Welche Gelenkausschlaege faehrt RE2 in echten Clips? Je Knochen groesster Winkel zwischen zwei Bildern (Spanne), ueber alle Clips des ersten Paars.
Liefert die Soll-Winkel der Biegeprobe. Eichmass Handwerk, keine Formvorlage."""
from u_basis import *


def rel(form, c, i):
    R, T = pose(form, c, i)
    n = len(form['knochen'])
    L = []
    for b in range(n):
        p = form['knochen'][b]['eltern']
        L.append(R[b] if p < 0 else R[p].T @ R[b])
    return L, T


erg = {}
for k in ('K34', 'K33', 'K28', 'K23', 'K22', 'K20'):
    form = lade(k)
    n = len(form['knochen'])
    alle = [[] for _ in range(n)]
    Bilder = 0
    for c in range(len(form['clips'])):
        bl = form['clips'][c]['bilder']
        Bilder += len(bl)
        for i in range(len(bl)):
            L, _ = rel(form, c, i)
            for b in range(n):
                alle[b].append(L[b])
    print('==', k, 'Clips', len(form['clips']), 'Bilder', Bilder)
    z = []
    for b in range(n):
        Q = np.array(alle[b])
        M = Q.mean(0)
        U, _, Vt = np.linalg.svd(M)
        M = U @ Vt
        d = np.einsum('ijk,jk->i', Q, M)
        win = np.degrees(np.arccos(np.clip((d - 1) / 2, -1, 1)))
        idx = np.argsort(-win)[:40]
        sp = 0.0
        for i in idx:
            dd = np.einsum('jk,ijk->i', Q[i], Q)
            sp = max(sp, float(np.degrees(np.arccos(np.clip((dd - 1) / 2, -1, 1))).max()))
        o = form['knochen'][b].get('versatz')
        z.append((b, form['knochen'][b]['eltern'], o, round(sp, 1), round(float(np.percentile(win, 95)), 1)))
    for r in z:
        print('  b%2d elt %2d vers %s  Spanne %6.1f  p95 um Mitte %6.1f' % (r[0], r[1], r[2], r[3], r[4]))
    erg[k] = z
json.dump(erg, open(AUS + '/ausschlag.json', 'w'), indent=1, default=str)
