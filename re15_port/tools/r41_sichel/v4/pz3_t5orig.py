"""Zahlen-Pruefer V4 r3: dasselbe Breitenprofil (Breite quer zur Mittellinie, Mittellinie t2_mass_v4.linie) am ORIGINAL (3 Leser-Masken)
und an V4 r3 / V4 r2 (Schwanz-Ids, Pose 85), Achse Knick -> Beginn zwischen den Mittellinienpunkten naechst Knick/Beginn
(Original: Nutzerpunkte (290,308)/(357,243); Port: T1 (Id 16) und Mittellinienpunkt minimaler Breite zwischen Biegung und Ende)."""
import numpy as np, json, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import t2_mass_v4 as T2M
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz3/'
def breiten(t, P):
    W = []
    for j in range(len(P)):
        a = P[min(j + 2, len(P) - 1)] - P[max(j - 2, 0)]; a = a / (np.linalg.norm(a) + 1e-9); n = np.array([-a[1], a[0]]); w = 0
        for sg in (1, -1):
            for st in np.arange(0.5, 80, 0.5):
                x, y = P[j] + sg * st * n; xi, yi = int(round(x)), int(round(y))
                if not (0 <= xi < t.shape[1] and 0 <= yi < t.shape[0]) or not t[yi, xi]: break
                w += 0.5
        W.append(w)
    return np.array(W)
def profil(t, P, s, H, beg, kn):
    W = breiten(t, P); a = int(np.argmin(np.linalg.norm(P - beg, axis=1))); k = int(np.argmin(np.linalg.norm(P - kn, axis=1)))
    L = s[k] - s[a]; wf = lambda f: W[int(np.argmin(np.abs(s - (s[k] - f * L))))] / H
    seq = [wf(f) for f in np.linspace(0.05, 0.75, 15)]
    return [round(wf(f), 3) for f in np.arange(0, 1.0001, 0.1)], round(max(max(seq[:i + 1]) - seq[i] for i in range(len(seq))), 3)
for f in T2M.MASKEN:
    d = np.load(f); t = np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)
    ys, xs = np.nonzero(t); P, s = T2M.linie(t, (int(xs.max()), int(ys[np.argmax(xs)])))
    pr, ab = profil(t, P, s, 253.0, np.array([357, 243]), np.array([290, 308]))
    print('Original %-22s f 0..1 je 0,1: %s  Abfall %.3f' % (os.path.basename(f), pr, ab))
M = json.load(open(SP + 'mess_V4_V4R2_V3.json'))
for nm, beg in (('V4', (358, 238)), ('V4R2', (360, 236))):
    ids = np.load(SP + 'r_%s_85.npz' % nm)['ids']; t = (ids >= 16) & (ids <= 22); H = M[nm]['seite']['H']
    r = T2M.port(ids, H); P = r['P']; s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    kn = np.array(M[nm]['seite']['T']['T3']['p'])
    pr, ab = profil(t, P, s, H, np.array(beg), kn)
    print('%-5s Knick %s f 0..1 je 0,1: %s  Abfall %.3f' % (nm, kn, pr, ab))
