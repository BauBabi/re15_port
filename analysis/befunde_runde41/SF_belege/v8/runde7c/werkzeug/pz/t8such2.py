"""K-b Suche 2: wie t8such.py, dazu Zahl der fast weissen Texel (Lab L > 75, Chroma < 12; Mass riegel_v8.weiss_px auf Texelebene) unter t8."""
import sys, os, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vorn as Vn, flaechen as F
kn = sys.argv[1]
K, Ein = Vn.lade(kn), Vn.lade('R6')
ch = Vn.geaendert(K, Ein)
ref = [F.kennz(P, T)[2] for key, P, T, z, vi, ni, N in F.haelften(K.meshes[8]) if (8, key[0], key[1]) not in ch and P.mean(0)[0] < 0]
med = float(np.median([r for r in ref if np.isfinite(r)]))
belegt = {}
for n in ('R6', 'R4', 'V7', 'R2', 'N7B'):
    E = Vn.lade(n)
    for m in E.meshes:
        for key, P, T, z, vi, ni, N in F.haelften(m):
            belegt.setdefault(z, set()).update(F.texel(T, E.tex.shape))
m = K.meshes[8]; f = m['tf'][8]; vi = list(f[1::2]); P = m['tv'][vi].astype(float); uv = m['tuv'][8]
po = (uv[5] & 0xF) * 128
out = []
for u in range(14, 40):
    for v in range(28, 50):
        T = np.array([(uv[0] + po, uv[1]), (u + po, v), (uv[6] + po, uv[7])], float)
        A3, A2, d, s = F.kennz(P, T)
        tx = F.texel(T, K.tex.shape)
        if not (d / med >= 0.53 and s <= 2.0) or (tx - belegt[493]):
            continue
        L = F.lab(K, 493, sorted(tx))
        w = int(((L[:, 0] > 75) & (np.hypot(L[:, 1], L[:, 2]) < 12)).sum())
        out.append((w, abs(u - 29) + abs(v - 38), u, v, round(d / med, 3), round(s, 2)))
out.sort()
print('t8 (weiss, Abstand zu R3 (29,38), u, v, Dichte, Streckung):', out[:15])
