"""Pruefer P5 R2 (E) Normalen: mittlerer Winkel Eckennormale <-> geometrische Flaechennormale je Haelfte (Mesh 8), Kandidat gegen
Eingang R6 und R4 (gleicher Schluessel = Flaechenart, Index, Haelfte; nur Flaechen < 77). Zunahme > 5 Grad wird gelistet."""
import sys, numpy as np, flaechen as F, vorn as Vn
def winkel(m):
    out = {}
    for key, P, T, z, vi, ni, N in F.haelften(m):
        g = np.cross(P[1] - P[0], P[2] - P[0]); g = g / (np.linalg.norm(g) + 1e-12)
        out[key] = (float(np.mean([np.degrees(np.arccos(np.clip(abs(N[j] @ g) / (np.linalg.norm(N[j]) + 1e-12), 0, 1))) for j in ni])), vi)
    return out
K = winkel(Vn.lade(sys.argv[1]).meshes[8])
for ref in ('R6', 'R4'):
    R = winkel(Vn.lade(ref).meshes[8]); d = []
    for k, (w, vi) in K.items():
        if k[1] >= 77 and k[0] == 't': continue
        if k in R and R[k][1] == vi: d.append((round(w - R[k][0], 1), k, vi, round(w, 1)))
    gr = sorted([x for x in d if x[0] > 5], reverse=True)
    print('%s gegen %s: Haelften %d | Mittel Aenderung %+.2f Grad | Zunahme > 5 Grad: %d -> %s' % (sys.argv[1], ref, len(d), np.mean([x[0] for x in d]), len(gr), gr[:12]))
