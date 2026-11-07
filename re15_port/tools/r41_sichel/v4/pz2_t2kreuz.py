"""Zahlen-Pruefer V4 r2: Kreuzprobe T2-Drehung - Mittellinie des Bauers (v3/P_schwanz.mittellinie, Saat = vorderste Spalte) UND
eigene Mittellinie (pz2_t2orig.linie) je am Original-Schwanz (Leser-Masken) und am V4-Render; Drehung = Tangenten +-0,025 H am
Anfang/Ende eines 0,10-H-Fensters, nur hinter beginn - 0,10 H (wie riegel_v4 / pz_t2orig)."""
import os, sys, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'v3'))
from P_schwanz import mittellinie
from pz2_t2orig import linie
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
def dreh(C, H, xmax):
    sl = np.r_[0, np.cumsum(np.hypot(*np.diff(C, axis=0).T))]
    at = lambda x: np.array([np.interp(x, sl, C[:, 0]), np.interp(x, sl, C[:, 1])])
    best = (0.0, 0.0)
    for x in np.arange(0, sl[-1] - 0.10 * H, 0.5):
        if at(x)[0] > xmax: continue
        t0 = at(x + 0.025 * H) - at(max(0.0, x - 0.025 * H)); t1 = at(min(sl[-1], x + 0.125 * H)) - at(x + 0.075 * H)
        w = np.degrees(np.arctan2(t1[1], -t1[0]) - np.arctan2(t0[1], -t0[0]))
        if w > best[0]: best = (float(w), float(x))
    return round(best[0], 1), np.round(at(best[1] + 0.05 * H), 1).tolist()
def bauer_linie(t, H):
    ys, xs = np.nonzero(t); Q = np.zeros_like(t); sel = xs >= xs.max() - 1; Q[ys[sel], xs[sel]] = True
    C, _ = mittellinie(t, Q, H); return C
for f in ('schwanz_maske.npy', 'schwanz_maske_hi.npy', 'schwanz_maske_lo.npy'):
    t = np.load(S + f).astype(bool); ys, xs = np.nonzero(t); seed = (int(xs.max()), int(ys[np.argmax(xs)]))
    P, s = linie(t, seed)
    print('Original %-22s Bauer-Linie %s   eigene Linie %s' % (f, dreh(bauer_linie(t, 253.0), 253.0, 357 - 25.3), dreh(P, 253.0, 357 - 25.3)))
d = np.load(S + 'pz2/r_V4_85.npz'); t = np.isin(d['ids'], range(16, 23))
import json
m = json.load(open(S + 'pz2/mess_V4_V3_REF2.json'))['V4']['seite']['T']
P, s = linie(t, tuple(m['saat']))
for bx in (360, 349):
    print('V4 r2 (beginn x %d)            Bauer-Linie %s   eigene Linie %s' % (bx, dreh(bauer_linie(t, 264.0), 264.0, bx - 26.4), dreh(P, 264.0, bx - 26.4)))
