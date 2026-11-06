"""Zahlen-Pruefer: dieselbe Mittellinie/Drehungsmessung am Original-Schwanz (Masken der Leser, sfv4/schwanz_maske*.npy) und am V4-Render."""
import numpy as np, cv2, json, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz_mess import geod
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
def linie(t, seed):
    n, lb, st, _ = cv2.connectedComponentsWithStats(t.astype(np.uint8), connectivity=8)
    t = lb == (1 + int(np.argmax(st[1:, 4])))
    if not t[seed[1], seed[0]]:
        ys, xs = np.nonzero(t); i = np.argmin((xs - seed[0]) ** 2 + (ys - seed[1]) ** 2); seed = (int(xs[i]), int(ys[i]))
    D = geod(t, seed); pts = []
    for d0 in range(0, D.max() + 1, 3):
        m = (D >= d0) & (D < d0 + 3)
        if m.any():
            yy, xx = np.nonzero(m); pts.append((xx.mean(), yy.mean()))
    P = np.array(pts); P = np.array([P[max(0, j - 1):j + 2].mean(0) for j in range(len(P))])
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    return P, s
def drehung(P, s, H, xmax):
    at = lambda x: np.array([np.interp(x, s, P[:, 0]), np.interp(x, s, P[:, 1])])
    best = (0, 0); best2 = (0, 0)
    for x in np.arange(0, s[-1] - 0.10 * H, 0.5):
        if at(x)[0] > xmax: continue
        a = at(x + 0.05 * H) - at(x); b = at(x + 0.10 * H) - at(x + 0.05 * H)
        w = np.degrees(np.arctan2(b[1], -b[0]) - np.arctan2(a[1], -a[0]))
        if w > best[0]: best = (w, x)
        # zentrierte Tangente +-0,025 H am Anfang und Ende eines 0,10-H-Fensters
        t0 = at(x + 0.025 * H) - at(max(0, x - 0.025 * H)); t1 = at(min(s[-1], x + 0.125 * H)) - at(x + 0.075 * H)
        w2 = np.degrees(np.arctan2(t1[1], -t1[0]) - np.arctan2(t0[1], -t0[0]))
        if w2 > best2[0]: best2 = (w2, x)
    prof = [(round(x / H, 2), int(np.degrees(np.arctan2(*(at(x + 0.025 * H) - at(max(0, x - 0.025 * H)))[::-1] * [1, -1])))) for x in np.arange(0, s[-1], 0.05 * H)]
    return round(best[0], 1), np.round(at(best[0] and best[1] + 0.05 * H), 1).tolist(), round(best2[0], 1), prof
for f in ('schwanz_maske.npy', 'schwanz_maske_hi.npy', 'schwanz_maske_lo.npy'):
    t = np.load(S + f)
    ys, xs = np.nonzero(t); seed = (int(xs.max()), int(ys[np.argmax(xs)]))
    P, s = linie(t, seed)
    print('Original', f, 'Saat', seed, 'Sehnen-Drehung, Mitte, Tangenten-Drehung:', drehung(P, s, 253.0, 357 - 0.10 * 253)[:3])
    print('   Profil (s/H, Grad 0=waagerecht nach hinten, 90=senkrecht nach unten):', drehung(P, s, 253.0, 357 - 0.10 * 253)[3])
d = np.load(S + 'pz/r_V4_85.npz'); ids = d['ids']; t = (ids >= 16) & (ids <= 22)
m = json.load(open(S + 'pz/mess_V4_V3_REF2.json'))['V4']['seite']['T']
P, s = linie(t, tuple(m['saat']))
print('V4', 'Sehnen-Drehung, Mitte, Tangenten-Drehung:', drehung(P, s, 264.0, m['beginn'][0] - 0.10 * 264)[:3])
