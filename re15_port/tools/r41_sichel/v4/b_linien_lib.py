# Hilfsfunktionen Leser B (aus b_linien.py)
import numpy as np
R_BEL = 1
def belegung(M, p, q, r=R_BEL):
    p = np.array(p, float); q = np.array(q, float); L = np.linalg.norm(q - p)
    n = max(2, int(L / 0.25)); hit = 0
    H, W = M.shape
    for t in np.linspace(0, 1, n):
        x, y = p + t * (q - p); xi, yi = int(round(x)), int(round(y))
        y0, y1, x0, x1 = max(0, yi - r), min(H, yi + r + 1), max(0, xi - r), min(W, xi + r + 1)
        hit += M[y0:y1, x0:x1].any()
    return hit / n

def seg_intersect(a, b, c, d):
    a, b, c, d = [np.array(v, float) for v in (a, b, c, d)]
    r = b - a; s = d - c; den = r[0] * s[1] - r[1] * s[0]
    if abs(den) < 1e-9: return None
    t = ((c - a)[0] * s[1] - (c - a)[1] * s[0]) / den; u = ((c - a)[0] * r[1] - (c - a)[1] * r[0]) / den
    if 0 < t < 1 and 0 < u < 1: return (a + t * r).round(1).tolist()
    return None

