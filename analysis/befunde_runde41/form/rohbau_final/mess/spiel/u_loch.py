# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: (a) Woher kommt der Koerper-Durchblick: je Knochen getrennt, bester Blick als Bild. (b) Lose Stuecke: wahrer Abstand Punkt zu Flaeche, nicht zu Ecken."""
import trimesh, cv2
from u_basis import *
from u_posen import blickachsen
from u_mass import RUMPF_GLIEDER


def je_knochen(form, W_, tris, gross=900, azs=range(0, 360, 30), els=(-35, 0, 20, 40, 65)):
    alle = np.vstack([w for w in W_ if len(w)])
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max())
    s = gross * 0.46 / rad
    ke = np.ones((3, 3), np.uint8)
    n = len(W_)
    su = np.zeros(n, int)
    best = (0, None, None, None, None)
    for az in azs:
        for el in els:
            A = blickachsen(az, el)
            vorne = np.zeros((gross, gross), np.uint8)
            hint = {}
            for b, (w, F) in enumerate(zip(W_, tris)):
                if len(F) == 0:
                    continue
                q = (w - mitte) @ A.T
                xy = np.round(q[:, :2] * s * 16 + gross * 8).astype(np.int32)
                t = xy[F]
                d = (t[:, 1, 0] - t[:, 0, 0]) * (t[:, 2, 1] - t[:, 0, 1]) - (t[:, 2, 0] - t[:, 0, 0]) * (t[:, 1, 1] - t[:, 0, 1])
                h = None
                for tt, dd in zip(t, d):
                    if dd > 0:
                        cv2.fillConvexPoly(vorne, tt, 1, lineType=cv2.LINE_8, shift=4)
                    else:
                        if h is None:
                            h = np.zeros((gross, gross), np.uint8)
                        cv2.fillConvexPoly(h, tt, 1, lineType=cv2.LINE_8, shift=4)
                if h is not None:
                    hint[b] = h
            for b, h in hint.items():
                loch = cv2.morphologyEx(((h > 0) & (vorne == 0)).astype(np.uint8), cv2.MORPH_OPEN, ke)
                x = int(loch.sum())
                su[b] += x
                if b in RUMPF_GLIEDER and x > best[0]:
                    best = (x, b, (az, el), loch.copy(), vorne.copy())
    return su, best


def punkt_dreieck(P, a, b, c):
    """Kleinster Abstand der Punkte P (N x 3) zu Dreiecken a, b, c (M x 3), Ericson."""
    ab, ac = b - a, c - a
    aus = np.full(len(P), 1e18)
    for i, p in enumerate(P):
        ap = p - a
        d1 = (ab * ap).sum(1)
        d2 = (ac * ap).sum(1)
        bp = p - b
        d3 = (ab * bp).sum(1)
        d4 = (ac * bp).sum(1)
        cp = p - c
        d5 = (ab * cp).sum(1)
        d6 = (ac * cp).sum(1)
        vc = d1 * d4 - d3 * d2
        vb = d5 * d2 - d1 * d6
        va = d3 * d6 - d5 * d4
        nen = va + vb + vc
        nen = np.where(np.abs(nen) < 1e-9, 1e-9, nen)
        v = vb / nen
        w = vc / nen
        q = a + ab * v[:, None] + ac * w[:, None]
        # Regionen
        m = (d1 <= 0) & (d2 <= 0)
        q[m] = a[m]
        m = (d3 >= 0) & (d4 <= d3)
        q[m] = b[m]
        m = (d6 >= 0) & (d5 <= d6)
        q[m] = c[m]
        m = (vc <= 0) & (d1 >= 0) & (d3 <= 0)
        t = d1 / np.where(np.abs(d1 - d3) < 1e-9, 1e-9, d1 - d3)
        q[m] = (a + ab * t[:, None])[m]
        m = (vb <= 0) & (d2 >= 0) & (d6 <= 0)
        t = d2 / np.where(np.abs(d2 - d6) < 1e-9, 1e-9, d2 - d6)
        q[m] = (a + ac * t[:, None])[m]
        m = (va <= 0) & ((d4 - d3) >= 0) & ((d5 - d6) >= 0)
        t = (d4 - d3) / np.where(np.abs((d4 - d3) + (d5 - d6)) < 1e-9, 1e-9, (d4 - d3) + (d5 - d6))
        q[m] = (b + (c - b) * t[:, None])[m]
        aus[i] = np.sqrt(((p - q) ** 2).sum(1)).min()
    return aus


def lose(form, R, T):
    n = len(form['knochen'])
    W_ = welt(form, R, T)
    aus = []
    for b in range(n):
        m = form['meshes'][b]
        if not m['ecken']:
            continue
        F = dreiecke(m)
        rund = np.round(np.array(m['ecken'], float), 0)
        u, inv = np.unique(rund, axis=0, return_inverse=True)
        inv = inv.reshape(-1)
        Fu = inv[F]
        E = np.vstack([Fu[:, [0, 1]], Fu[:, [1, 2]], Fu[:, [2, 0]]])
        comps = [c for c in trimesh.graph.connected_components(E, nodes=np.arange(len(u)), min_len=1) if len(c) >= 3]
        if len(comps) <= 1:
            continue
        Pw = u @ R[b].T + T[b]
        for ci, c in enumerate(comps):
            cs = set(int(x) for x in c)
            ein = np.array([all(int(v) in cs for v in t) for t in Fu])
            A_, B_, C_ = [Pw[Fu[~ein]][:, i] for i in range(3)]
            ta, tb, tc = [A_], [B_], [C_]
            for b2 in range(n):
                if b2 == b or not len(W_[b2]):
                    continue
                F2 = dreiecke(form['meshes'][b2])
                if not len(F2):
                    continue
                ta.append(W_[b2][F2[:, 0]])
                tb.append(W_[b2][F2[:, 1]])
                tc.append(W_[b2][F2[:, 2]])
            d = punkt_dreieck(Pw[list(c)], np.vstack(ta), np.vstack(tb), np.vstack(tc))
            if d.min() > 8:
                aus.append((NAMEN[b], len(c), round(float(d.min()), 1)))
    return aus


if __name__ == '__main__':
    for k in ROHBAU:
        form = lade(k)
        n = len(form['knochen'])
        tris = [dreiecke(m) for m in form['meshes'][:n]]
        for c, nm in ((2, 'ruhe'), (0, 'pose1'), (1, 'pose2')):
            R, T = pose(form, c)
            W_ = welt(form, R, T)
            su, best = je_knochen(form, W_, tris)
            print(k, nm, 'je Knochen', {NAMEN[b]: int(su[b]) for b in np.argsort(-su)[:8] if su[b] > 0})
            if best[3] is not None:
                g = np.zeros(best[3].shape + (3,), np.uint8)
                g[best[4] > 0] = (150, 150, 150)
                g[best[3] > 0] = (255, 0, 0)
                cv2.imwrite(AUS + '/lochkoerper_%s_%s.png' % (k, nm), g[:, :, ::-1])
                print('    bester Koerperblick', best[0], NAMEN[best[1]], best[2])
            print('    lose Stuecke (wahrer Abstand > 8):', lose(form, R, T))
