"""Zahlen-Pruefer: M4 Texeldichte je Mesh = sqrt(UV-Flaeche / Modellflaeche) mit eigenem MD1-Parser; Vergleich Haut 0-15."""
import sys, os, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz4_g1 import secs, md1, R, S
def fl3(a, b, c): return 0.5 * np.linalg.norm(np.cross(b - a, c - a))
def fl2(a, b, c): return 0.5 * abs((b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1]))
for nm, p in (('V4', S + 'EM30_V4.EMD'), ('REF2', R)):
    M = md1(secs(open(p, 'rb').read())[7])
    out = []
    for i, m in enumerate(M):
        A3 = A2 = 0.0
        tv = m['tv'][:, :3].astype(float); qv = m['qv'][:, :3].astype(float)
        for f, uv in zip(m['tf'], m['tuv_raw']):
            v = [tv[f[1]], tv[f[3]], tv[f[5]]]; u = [np.array(uv[0:2]), np.array(uv[3:5]), np.array(uv[6:8])]
            A3 += fl3(*v); A2 += fl2(*u)
        for f, uv in zip(m['qf'], m['quv_raw']):
            v = [qv[f[1]], qv[f[3]], qv[f[5]], qv[f[7]]]; u = [np.array(uv[0:2]), np.array(uv[3:5]), np.array(uv[6:8]), np.array(uv[9:11])]
            A3 += fl3(v[0], v[1], v[2]) + fl3(v[1], v[3], v[2]); A2 += fl2(u[0], u[1], u[2]) + fl2(u[1], u[3], u[2])
        out.append(round(float(np.sqrt(A2 / A3)), 5) if A3 else None)
    print(nm, 'Kopf 9:', out[9], 'Schwanz 16-22:', out[16:], 'Haut 0-15 ohne 9: min %.3f max %.3f' % (min(o for k, o in enumerate(out[:16]) if k != 9 and o), max(o for k, o in enumerate(out[:16]) if k != 9 and o)))
