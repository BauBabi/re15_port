"""K-b Suche: UV der Ecke 17 in t8 (Mesh 8, Zeile 493) -> Dichte (Checker-Mass flaechen.kennz, Bezug Median wie flaechen.py) und
Texel, die keiner der Staende R6/R4/V7/R2/N7B auf Zeile 493 benutzt.  python t8such.py KAND"""
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
m = K.meshes[8]
f = m['tf'][8]; vi = list(f[1::2]); P = m['tv'][vi].astype(float); uv = m['tuv'][8]
print('t8 Ecken', vi, 'UV', [(uv[3*k], uv[3*k+1]) for k in range(3)], 'Median', med)
po = (uv[5] & 0xF) * 128
best = []
for u in range(20, 34):
    for v in range(34, 46):
        T = np.array([(uv[0] + po, uv[1]), (u + po, v), (uv[6] + po, uv[7])], float)
        A3, A2, d, s = F.kennz(P, T)
        nt = F.texel(T, K.tex.shape) - belegt[493]
        if d / med >= 0.5 and s <= 2.0 and not nt:
            best.append((abs(u - uv[3]) + abs(v - uv[4]), u, v, round(d / med, 3), round(s, 2)))
best.sort()
print('Kandidaten (Abstand, u, v, Dichte, Streckung):', best[:12])
