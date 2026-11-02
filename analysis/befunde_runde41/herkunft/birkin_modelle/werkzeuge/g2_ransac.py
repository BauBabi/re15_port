#!/usr/bin/env python3
"""1) Beine M1[1,2,4,5]: Regel 'negative Komponente +1' auf G2 anwenden, Mengen-Vergleich und Zuordnung (Hungarian).
2) RANSAC rigide (Rotation+Translation, Skala 1) G2/G1/D0-Teile -> M1[0], M1[15]. Inlier < 5 Einheiten."""
import sys, os, random
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from scipy.optimize import linear_sum_assignment
M1, D0 = zerlege(blob('M1')), zerlege(blob('D0'))
e, *_ = re2_rec(0x31, 3); t, *_ = re2_rec(0x31, 2); G2 = zerlege(e, t)
e, *_ = re2_rec(0x30, 3); t, *_ = re2_rec(0x30, 2); G1 = zerlege(e, t)
e, *_ = re2_rec(0x33, 3); t, *_ = re2_rec(0x33, 2); G3 = zerlege(e, t)
V = lambda m: np.array([v[:3] for v in m.tv], float)
regel = lambda a: np.where(a < 0, a + 1, a)
print('== Beine')
for i, j in ((1, 12), (2, 13), (3, 14), (4, 15), (5, 16), (6, 17)):
    a, b = V(M1.meshes[i]), V(G2.meshes[j])
    r = regel(b)
    sa, sr = set(map(tuple, a)), set(map(tuple, r))
    D = np.abs(a[:, None, :] - r[None, :, :]).sum(-1)
    ri, ci = linear_sum_assignment(D)
    diff = np.abs(a[ri] - r[ci])
    print(f'M1[{i}] G2[{j}] n={len(a)}: nach Regel exakt gleiche Punkte {len(sa & sr)}; beste 1:1-Zuordnung max Abweichung je Achse {diff.max(0)}, Summe {D[ri, ci].sum():.0f}, exakte Paare {(D[ri,ci]==0).sum()}')
    if len(sa & sr) < len(a):
        z = [(tuple(a[x].astype(int).tolist()), tuple(r[y].astype(int).tolist())) for x, y in zip(ri, ci) if D[x, y] > 0]
        print('    Abweichungen:', z[:8])


def rigide(P, Q):
    """Kabsch: R,t mit R@P+t ~ Q"""
    pc, qc = P.mean(0), Q.mean(0)
    H = (P - pc).T @ (Q - qc)
    U, S, Vt = np.linalg.svd(H)
    d = np.sign(np.linalg.det(Vt.T @ U.T))
    R = Vt.T @ np.diag([1, 1, d]) @ U.T
    return R, qc - R @ pc


def ransac(P, A, n=6000, tol=5.0, dist_tol=6.0):
    best = (0, None, None)
    nP = len(P)
    if nP < 3:
        return best
    DA = np.sqrt(((A[:, None] - A[None]) ** 2).sum(-1))
    for _ in range(n):
        i = random.sample(range(nP), 3)
        p = P[i]
        d01 = np.linalg.norm(p[0] - p[1]); d02 = np.linalg.norm(p[0] - p[2]); d12 = np.linalg.norm(p[1] - p[2])
        if min(d01, d02, d12) < 60:
            continue
        a0 = random.randrange(len(A))
        c1 = np.where(np.abs(DA[a0] - d01) < dist_tol)[0]
        if not len(c1):
            continue
        for a1 in c1[:6]:
            c2 = np.where((np.abs(DA[a0] - d02) < dist_tol) & (np.abs(DA[a1] - d12) < dist_tol))[0]
            for a2 in c2[:6]:
                R, t = rigide(p, A[[a0, a1, a2]])
                T = (R @ P.T).T + t
                d = np.sqrt(((T[:, None] - A[None]) ** 2).sum(-1)).min(1)
                inl = (d < tol).sum()
                if inl > best[0]:
                    best = (int(inl), R, t)
    return best


random.seed(1)
ziele = {'M1[15]': V(M1.meshes[15]), 'M1[0]': V(M1.meshes[0])}
kand = {}
for j in (0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11):
    kand[f'G2[{j}]'] = V(G2.meshes[j])
for j in (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18):
    kand[f'G1[{j}]'] = V(G1.meshes[j])
for j in (0, 5, 7, 15, 16):
    kand[f'D0[{j}]'] = V(D0.meshes[j])
for zn, A in ziele.items():
    print('==', zn, 'n', len(A))
    erg = []
    for kn, P in kand.items():
        inl, R, t = ransac(P, A)
        erg.append((inl / len(P), inl, len(P), kn, None if R is None else float(np.degrees(np.arccos(np.clip((np.trace(R) - 1) / 2, -1, 1)))),
                    None if t is None else t.round().tolist()))
    erg.sort(key=lambda x: (-x[1], -x[0]))
    for x in erg[:8]:
        print(f'   {x[3]:8s} Inlier {x[1]}/{x[2]} ({x[0]:.2f}) Drehwinkel {None if x[4] is None else round(x[4], 1)} t {x[5]}')
