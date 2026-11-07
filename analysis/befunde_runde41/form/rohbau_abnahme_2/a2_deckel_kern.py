import numpy as np
def treffer(o, d, tri):
    """Strahlparameter t>0 aller Treffer auf tri (N,3,3)."""
    e1 = tri[:, 1] - tri[:, 0]; e2 = tri[:, 2] - tri[:, 0]
    p = np.cross(d, e2)
    det = (e1 * p).sum(1)
    ok = np.abs(det) > 1e-9
    inv = np.zeros_like(det); inv[ok] = 1 / det[ok]
    s = o - tri[:, 0]
    u = (s * p).sum(1) * inv
    q = np.cross(s, e1)
    v = (q * d).sum(1) * inv
    t = (e2 * q).sum(1) * inv
    m = ok & (u >= 0) & (v >= 0) & (u + v <= 1) & (t > 1e-6)
    return t[m]
