"""Zahlen-Pruefer V4 r4: Belegbild T5 - Original-Maske A (Vorlage 2) | V4 r2 | V4 r4 (Schwanz-Ids, Pose 85), Mittellinie (t2_mass_v4.linie)
und die Breitenstrahlen bei f 0,1..0,9 (Knick -> Beginn); Ausschnitt x 250-400, y 180-360, x3."""
import numpy as np, cv2, json, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import t2_mass_v4 as T2M
from pz5_t5orig import breiten
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz5/'
OUT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_t5_breitenprofil.png'
M = json.load(open(SP + 'mess_V4_V4R4_V3.json'))
def kachel(t, P, s, H, beg, kn, titel):
    img = np.zeros(t.shape + (3,), np.uint8); img[t] = (150, 150, 150)
    W = breiten(t, P); a = int(np.argmin(np.linalg.norm(P - beg, axis=1))); k = int(np.argmin(np.linalg.norm(P - kn, axis=1))); L = s[k] - s[a]
    for p in P: cv2.circle(img, (int(round(p[0])), int(round(p[1]))), 0, (0, 255, 255), -1)
    for f in np.arange(0.1, 0.95, 0.1):
        j = int(np.argmin(np.abs(s - (s[k] - f * L)))); d = P[min(j + 2, len(P) - 1)] - P[max(j - 2, 0)]; d /= np.linalg.norm(d) + 1e-9; n = np.array([-d[1], d[0]])
        c = (255, 0, 0) if abs(f - 0.4) < 0.01 else (0, 160, 255)
        cv2.line(img, tuple(int(v) for v in P[j] - n * W[j] / 2), tuple(int(v) for v in P[j] + n * W[j] / 2), c, 1)
    for q in (beg, kn): cv2.circle(img, (int(q[0]), int(q[1])), 2, (0, 255, 0), -1)
    c = cv2.resize(img[180:360, 250:400], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
    cv2.putText(c, titel, (5, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255, 255, 255), 1)
    return c
d = np.load(T2M.MASKEN[0]); t = np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)
ys, xs = np.nonzero(t); P, s = T2M.linie(t, (int(xs.max()), int(ys[np.argmax(xs)])))
K = [kachel(t, P, s, 253.0, np.array([357, 243]), np.array([290, 308]), 'Original Maske A (n1435)')]
for nm, beg, ti in (('V4R4', (360, 236), 'V4 r2'), ('V4', (358, 238), 'V4 r4 (f0,4 rot)')):
    ids = np.load(SP + 'r_%s_85.npz' % nm)['ids']; tt = (ids >= 16) & (ids <= 22); r = T2M.port(ids, M[nm]['seite']['H'])
    P = r['P']; s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    K.append(kachel(tt, P, s, M[nm]['seite']['H'], np.array(beg), np.array(M[nm]['seite']['T']['T3']['p']), ti))
cv2.imwrite(OUT, cv2.cvtColor(np.hstack(K), cv2.COLOR_RGB2BGR)); print(OUT)
