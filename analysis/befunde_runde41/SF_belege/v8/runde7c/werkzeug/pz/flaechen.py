"""Pruefer P5 (E) Flaechen: Texeldichte/Streckung der neuen und geaenderten Flaechen (Mesh 8/9 gegen Eingang r6) gegen den Median der
unberuehrten Rueckenflaechen (Mesh 8, Schwerpunkt x8 < 0, im Kandidaten unveraendert); Texel-Herkunft; Normalen.
Dichte = UV-Flaeche (Texel^2) / 3D-Flaeche (E^2), relativ zum Median. Streckung = Verhaeltnis der Singulaerwerte der affinen Abbildung
Ebene -> UV (1 = winkeltreu). Vierecke als Haelften (0,1,3),(0,3,2) wie der Port (main.c:11080). Aufruf: python flaechen.py KAND"""
import sys, os
import numpy as np
from skimage.draw import polygon
from skimage.color import rgb2lab
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, vorn as Vn


def haelften(m):
    for kind, V, N, F, U in (('t', m['tv'], m['tn'], m['tf'], m['tuv']), ('q', m['qv'], m['qn'], m['qf'], m['quv'])):
        for fi, (f, uv) in enumerate(zip(F, U)):
            vi = list(f[1::2]); ni = list(f[0::2])
            po = (uv[5] & 0xF) * 128; zeile = (uv[2] >> 6) & 0x1FF
            tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if kind == 'q' else [])
            sets = ((0, 1, 3), (0, 3, 2)) if kind == 'q' else ((0, 1, 2),)
            for h, s in enumerate(sets):
                yield (kind, fi, h), V[[vi[i] for i in s]].astype(float), np.array([tx[i] for i in s], float), zeile, [vi[i] for i in s], [ni[i] for i in s], N


def kennz(P, T):
    e1, e2 = P[1] - P[0], P[2] - P[0]; A3 = np.linalg.norm(np.cross(e1, e2)) / 2
    t1, t2 = T[1] - T[0], T[2] - T[0]; A2 = abs(t1[0] * t2[1] - t1[1] * t2[0]) / 2
    if A3 < 1e-6: return A3, A2, np.nan, np.nan
    u = e1 / np.linalg.norm(e1); n = np.cross(e1, e2); w = np.cross(n, u); w /= np.linalg.norm(w)
    Pm = np.array([[e1 @ u, e1 @ w], [e2 @ u, e2 @ w]]); Tm = np.array([t1, t2])
    J = np.linalg.solve(Pm, Tm) if abs(np.linalg.det(Pm)) > 1e-9 else None
    if J is None: return A3, A2, np.nan, np.nan
    s = np.linalg.svd(J, compute_uv=False)
    return A3, A2, A2 / A3, (s[0] / s[1] if s[1] > 1e-9 else np.inf)


def texel(T, shape):
    rr, cc = polygon(T[:, 1], T[:, 0], shape)
    return set(zip(rr.tolist(), cc.tolist()))


def lab(E, zeile, tex):
    c = np.array([E.clut[zeile - E.clut_y][E.tex[r, cc]] for r, cc in tex], np.int64)
    rgb = np.stack([(c & 31), (c >> 5) & 31, (c >> 10) & 31], -1) * 255.0 / 31
    return rgb2lab(rgb[None] / 255.0)[0]


if __name__ == '__main__':
    kn = sys.argv[1]
    K, Ein = Vn.lade(kn), Vn.lade('R6')
    ch = Vn.geaendert(K, Ein)
    # Bezug: unberuehrte Rueckenflaechen Mesh 8
    ref = []
    for key, P, T, z, vi, ni, N in haelften(K.meshes[8]):
        if (8, key[0], key[1]) in ch or P.mean(0)[0] >= 0: continue
        A3, A2, d, s = kennz(P, T)
        if np.isfinite(d): ref.append(d)
    med = float(np.median(ref)); print(kn, 'Bezug: %d unberuehrte Rueckenhaelften Mesh 8, Median Dichte %.4f Texel^2/E^2' % (len(ref), med))
    # Texel, die der Eingang je Zeile benutzt
    belegt = {}
    for mi, m in enumerate(Ein.meshes):
        for key, P, T, z, vi, ni, N in haelften(m):
            belegt.setdefault(z, set()).update(texel(T, Ein.tex.shape))
    rot = []; neu_tex = {}; zeilen = set()
    for mi in (8, 9):
        for key, P, T, z, vi, ni, N in haelften(K.meshes[mi]):
            if (mi, key[0], key[1]) not in ch: continue
            A3, A2, d, s = kennz(P, T); zeilen.add(z)
            rel = d / med if np.isfinite(d) else np.nan
            tx = texel(T, K.tex.shape); nt = tx - belegt.get(z, set())
            if nt: neu_tex.setdefault(z, set()).update(nt)
            # Normalen: Winkel Eckennormale zur geometrischen Flaechennormale (Umlauf egal -> Betrag)
            g = np.cross(P[1] - P[0], P[2] - P[0]); g = g / (np.linalg.norm(g) + 1e-12)
            wn = [round(float(np.degrees(np.arccos(np.clip(abs(N[j] @ g) / (np.linalg.norm(N[j]) + 1e-12), -1, 1)))), 0) for j in ni]
            ok = (0.5 <= rel <= 2.0) and s <= 2.0
            print('   %s M%d %s%d.%d Ecken %s Zeile %d Dichte %.2f-fach Streckung %.2f Normalen-Winkel %s %s' % ('  ' if ok else '!!', mi, key[0], key[1], key[2], vi, z, rel, s, wn, '' if ok else '<- ausserhalb Soll'))
            if not ok: rot.append((mi, key, round(rel, 2), round(s, 2)))
    print('   ausserhalb Soll (0,5-2-fach, Streckung <= 2):', rot)
    print('   CLUT-Zeilen der neuen/geaenderten Flaechen:', sorted(zeilen), '| im Eingang benutzt:', sorted(belegt))
    for z, t in neu_tex.items():
        L = lab(K, z, sorted(t))
        print('   Zeile %d: %d Texel, die der Eingang auf dieser Zeile nicht benutzt; L* %.0f-%.0f (Median %.0f), a* %.0f-%.0f; davon L* >= 80: %d' % (
            z, len(t), L[:, 0].min(), L[:, 0].max(), np.median(L[:, 0]), L[:, 1].min(), L[:, 1].max(), int((L[:, 0] >= 80).sum())))
