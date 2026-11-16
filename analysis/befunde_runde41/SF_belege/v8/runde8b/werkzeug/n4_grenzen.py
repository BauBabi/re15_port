"""Messung: groesste Mesh-Masse (radius[j] je Gelenk, Dornlaenge dorn_y), bei denen die 7e-Szenenkette die Bodenregel des Codes noch in KEINEM
Takt verletzt (dann laeuft die Szene wie 7e). Boden y 0, Gelenkhoehe = -y."""
import re, sys
import numpy as np
G = {}
for l in open(sys.argv[1], encoding='latin-1'):
    m = re.match(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', l)
    if m:
        G[int(m.group(1))] = np.array([[int(a), int(b), int(c)] for a, b, c in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(3))], float)
H = np.array([-P[:, 1] for P in G.values()])          # Hoehe je Takt und Gelenk
print('Takte', len(G), '; kleinste Hoehe je Gelenk 0..7 (= groesster zulaessiger radius[j]):', H.min(0).round(0).tolist())
lim = []
for h, P in G.items():
    dy = (P[7] - P[6])[1] / 541.0                        # Absenkung je E. Dornlaenge
    if dy > 1e-9:
        lim.append((-P[6][1] / dy, h))
lim.sort()
print('groesste zulaessige Dornlaenge dorn_y (ab Gelenk 6):', round(lim[0][0]), 'bei h', lim[0][1], '; naechste', [(round(a), b) for a, b in lim[1:6]])
