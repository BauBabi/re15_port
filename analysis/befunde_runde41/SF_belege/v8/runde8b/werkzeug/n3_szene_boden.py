"""Messung: In welchen Takten der 7e-Szene greift die Bodenregel des Codes (schwanz_r41.c glaetten, V8: Gelenk j + radius[j] und Dornpunkt
Gelenk 6 + Achse x dorn_y; Boden y 0) - mit den Mesh-Massen von 07b6703f (alt) und eines Kandidaten (neu, sw_mesh_mass nachgerechnet)."""
import re, sys
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
def masse(p):
    mo = V.modell(open(p, 'rb').read())
    rad = [0.0] * 8; dorn = 0.0
    for g in range(7):
        m = mo.meshes[16 + g]
        P = np.array([v[:3] for v in m.tv] + [v[:3] for v in m.qv], float)
        r = np.hypot(P[:, 0], P[:, 2]).max()
        rad[g] = max(rad[g], r); rad[g + 1] = max(rad[g + 1], r)
        if g == 6: dorn = P[:, 1].max()
    return np.array(rad), dorn
def lese(p):
    G = {}
    for l in open(p, encoding='latin-1'):
        m = re.match(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', l)
        if m:
            G[int(m.group(1))] = np.array([[int(a), int(b), int(c)] for a, b, c in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(3))], float)
    return G
G = lese(sys.argv[1])
for p in sys.argv[2:]:
    rad, dorn = masse(p)
    flag = []
    for h, P in sorted(G.items()):
        y = P[:, 1] + rad              # + = unter dem Boden
        dp = P[6] + (P[7] - P[6]) * dorn / 541.0
        if (y[2:] > 0.5).any() or dp[1] > 0.5:
            flag.append(h)
    print(p.split('/')[-1], 'radius', rad.round(0).tolist(), 'dorn_y', dorn, '-> 7e-Kette verletzt die Bodenregel in Takten', flag)
