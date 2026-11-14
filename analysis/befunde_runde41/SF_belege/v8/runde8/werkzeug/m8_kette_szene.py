"""Messung: Schwanzkette der Szene (debug.log "[hangar-takt] h.. Schwanz B") 7e gegen Runde 8 - je h groesste Gelenkverschiebung (E.), tiefster
Punkt mit Nadel (Gelenk 6 + (7-6) x 1139/541), Gelenkwinkel max."""
import re, sys
import numpy as np
def lese(p):
    G = {}
    for l in open(p, encoding='latin-1'):
        m = re.match(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', l)
        if m:
            G[int(m.group(1))] = np.array([[int(a), int(b), int(c)] for a, b, c in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(3))], float)
    return G
A, B = lese(sys.argv[1]), lese(sys.argv[2])
anders = []
for h in sorted(set(A) & set(B)):
    d = np.linalg.norm(A[h] - B[h], axis=1).max()
    if d > 0.5:
        anders.append((h, round(float(d))))
print('Takte gesamt', len(A), 'mit anderer Kette', len(anders))
print('h und groesste Gelenkverschiebung (E.):', anders)
def nadel(P): return P[6] + (P[7] - P[6]) * 1139.0 / 541.0
tn = max(float(nadel(P)[1]) for P in B.values()); ta = max(float(nadel(P)[1]) for P in A.values())
print('tiefstes y der Nadelspitze: 7e-Kette %.0f, Runde-8-Kette %.0f (Boden y 0, + = darunter)' % (ta, tn))
print('tiefstes Gelenk y: 7e %.0f, Runde 8 %.0f' % (max(P[:, 1].max() for P in A.values()), max(P[:, 1].max() for P in B.values())))
