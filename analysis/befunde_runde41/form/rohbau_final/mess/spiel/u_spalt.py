# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: echter Abstand Kind-Teil zu Eltern-Teil am Gelenk, ohne Schliessen der Silhouette.
Kontakt: kleinster Abstand der Oberflaechenproben. Eintauchtiefe: wie weit das Kind in den Eltern steckt (Anteil Proben des Kindes im Eltern-Volumen, Windungszahl).
Schlitz: durchgehender Spalt quer durch die Silhouette, Blick entlang der Achse, 1 px je Einheit, ohne Schliessen: Pixel im Band am Gelenk, die Hintergrund sind und von beiden Teilen eingeschlossen werden."""
import cv2
from scipy.spatial import cKDTree
from u_biege import *
from u_biege import maske as maske_geschlossen


def proben(p, F, abstand=14.0, seed=1):
    a, b, c = p[F[:, 0]], p[F[:, 1]], p[F[:, 2]]
    fl = 0.5 * np.linalg.norm(np.cross(b - a, c - a), axis=1)
    n = np.maximum(np.ceil(fl / (abstand * abstand)).astype(int), 1)
    rng = np.random.default_rng(seed)
    idx = np.repeat(np.arange(len(F)), n)
    r1 = np.sqrt(rng.random(len(idx)))[:, None]
    r2 = rng.random(len(idx))[:, None]
    return (1 - r1) * a[idx] + r1 * (1 - r2) * b[idx] + r1 * r2 * c[idx]


def gwn(q, p, F):
    a = p[F[:, 0]][None] - q[:, None]
    b = p[F[:, 1]][None] - q[:, None]
    c = p[F[:, 2]][None] - q[:, None]
    la, lb, lc = [np.linalg.norm(x, axis=2) for x in (a, b, c)]
    z = np.einsum('ijk,ijk->ij', a, np.cross(b, c))
    n = la * lb * lc + np.einsum('ijk,ijk->ij', a, b) * lc + np.einsum('ijk,ijk->ij', b, c) * la + np.einsum('ijk,ijk->ij', c, a) * lb
    return np.abs(np.arctan2(z, n).sum(axis=1) / (2 * math.pi))


def maske_offen(form, R, T, teile, A, J, s, halb):
    """Wie u_biege.maske, aber ohne Schliessen. Je Teil eigene Maske."""
    G = 2 * halb
    aus = {}
    for b in teile:
        M = np.zeros((G, G), np.uint8)
        m = form['meshes'][b]
        V = np.array(m['ecken'], float).reshape(-1, 3)
        if len(V) == 0:
            continue
        q = ((V @ R[b].T + T[b]) - J) @ A.T
        xy = q[:, :2] * s + halb
        fp = np.round(xy * 16).astype(np.int32)
        for f in m['dreiecke']:
            e = list(f['e'])
            if mac0(xy[e]) > 0:
                cv2.fillConvexPoly(M, fp[e], 1, lineType=cv2.LINE_8, shift=4)
        for f in m['vierecke']:
            a, b2, c, d = f['e']
            if mac0(xy[[a, b2, c]]) >= 0:
                cv2.fillConvexPoly(M, fp[[a, b2, d]], 1, lineType=cv2.LINE_8, shift=4)
                cv2.fillConvexPoly(M, fp[[a, d, c]], 1, lineType=cv2.LINE_8, shift=4)
        aus[b] = M
    return aus


if __name__ == '__main__':
    PAARE = [('ellbogen_l', 'oberarm_l', 'unterarm_l'), ('ellbogen_r', 'oberarm_r', 'unterarm_r'), ('knie_l', 'oberschenkel_l', 'unterschenkel_l'), ('knie_r', 'oberschenkel_r', 'unterschenkel_r'),
             ('wirbel', 'huefte', 'wirbel'), ('brust', 'wirbel', 'brust'), ('wurzel', 'huefte', 'schwanz1')] + [('schwanz%d' % i, 'schwanz%d' % i, 'schwanz%d' % (i + 1)) for i in range(1, 6)]
    erg = {}
    for k in ROHBAU:
        form = lade(k)
        R0, T0 = bindepose(form)
        erg[k] = {}
        print('==', k, 'Paar: Abstand in Bindepose, Anteil Kind im Eltern, schlechtester Abstand ueber +-45 Grad z und y (Ellbogen, Knie: 0-150 Grad), Schlitz-Pixel')
        for nm, a, b in PAARE:
            p, c = I[a], I[b]
            Fp, Fc = dreiecke(form['meshes'][p]), dreiecke(form['meshes'][c])
            Wb = welt(form, R0, T0)
            Pp = proben(Wb[p], Fp)
            Pc = proben(Wb[c], Fc)
            d0 = float(cKDTree(Pp).query(Pc)[0].min())
            st = np.random.default_rng(2).choice(len(Pc), min(400, len(Pc)), replace=False)
            inn = float((gwn(Pc[st], Wb[p], Fp) > 0.5).mean())
            if nm.startswith('ellbogen'):
                test = [Rz(-g) for g in (30, 60, 90, 120, 150)]
                ach = [(np.array([0, 0, 1.0]), None)]
            elif nm.startswith('knie'):
                test = [Rz(g) for g in (30, 60, 90, 120, 150)]
            else:
                test = [f(g) for f in (Rz, Ry) for g in (-45, -30, 30, 45)]
            sch = 0.0
            for D in test:
                Rn, Tn = gebeugt(form, R0, T0, {c: D})
                W2 = welt(form, Rn, Tn)
                d = float(cKDTree(proben(W2[p], Fp)).query(proben(W2[c], Fc))[0].min())
                sch = max(sch, d)
            # Schlitz: Silhouette ohne Schliessen, Blick z, 1 px je Einheit
            schl = 0
            for D, ax in ((np.eye(3), np.array([0, 0, 1.0])), (np.eye(3), np.array([0, 1.0, 0]))):
                J = T0[c]
                if nm.startswith('ellbogen') or nm.startswith('knie'):
                    u1 = np.array([0, 1.0, 0])
                    ax2 = ax if abs(ax[1]) < 0.5 else np.array([1.0, 0, 0])
                else:
                    u1 = np.array([1.0, 0, 0])
                    ax2 = ax
                A = ansicht(u1, ax2)
                ms = maske_offen(form, R0, T0, [p, c], A, J, 1.0, 900)
                vers = sum((m > 0).astype(np.uint8) for m in ms.values())
                ges = (vers > 0).astype(np.uint8)
                gesch = cv2.morphologyEx(ges, cv2.MORPH_CLOSE, np.ones((25, 25), np.uint8))
                sp = (gesch > 0) & (ges == 0)
                # nur innerhalb 500 um das Gelenk
                yy, xx = np.ogrid[:1800, :1800]
                sp &= ((xx - 900) ** 2 + (yy - 900) ** 2) < 500 ** 2
                schl = max(schl, int(sp.sum()))
            print('  %-11s Abstand %5.1f  Kind im Eltern %.2f  schlimmster Abstand %6.1f  Schlitz %6d' % (nm, d0, inn, sch, schl))
            erg[k][nm] = dict(abstand=round(d0, 1), kind_im_eltern=round(inn, 2), schlimm=round(sch, 1), schlitz=schl)
    json.dump(erg, open(AUS + '/spalt.json', 'w'), indent=1)
