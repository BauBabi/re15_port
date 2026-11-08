# Zahlen-Pruefer V3 r1: Gang/Halten im Bild - Schwanzkette aus debug.log als Rohr (Radien SCHWANZ_V3 radien_s) in die
# Spielkamera projiziert (ohne Verdeckung durch den Koerper), dann dieselbe Bildmessung wie R25/R26 (PZ_mess.form):
# max Winkelaenderung je 0,05 H und Knicke > 20 Grad, ab dem sichtbaren Austritt s 0,115.
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import b2
import birkin_formen as BF
from PZ_gang import lesen
from PZ_mess import mittellinie, form

RS = BF.SCHWANZ_V3['radien_s']
HE = 3829.0                     # Port-H in E. (Generator-Ausgabe "H 3829")

def maske(Q, by, x, z):
    welt = dict(gier=0, x=0, z=0, y=0)
    seg = np.linalg.norm(np.diff(Q, axis=0), axis=1); cum = np.r_[0, np.cumsum(seg)]; Ltot = cum[-1]
    s = np.linspace(0, 1, 400); t = s * Ltot
    P = np.stack([np.interp(t, cum, Q[:, k]) for k in range(3)], 1)
    r = np.interp(s, [a for a, _ in RS], [b for _, b in RS])
    q = b2.projiziere(welt, P)
    oben = b2.projiziere(welt, np.array([[x, by - HE, z], [x, by, z]], float))
    H = float(abs(oben[1, 1] - oben[0, 1]))
    sc = H / HE
    m = np.zeros((480, 640), np.uint8)
    for (px, py), rr in zip(q, r):
        cv2.circle(m, (int(round(px * 4)), int(round(py * 4))), max(1, int(round(rr * sc * 4))), 1, -1, lineType=cv2.LINE_8, shift=2)
    return m.astype(bool), q, s, H

if __name__ == '__main__':
    G, PO = lesen(sys.argv[1])
    import re
    pos = {}
    for zl in open(sys.argv[1], errors='replace'):
        mm = re.search(r'\[hangar-takt\] h(-?\d+) F\d+ B clip \d+ bild \d+ gier -?\d+ \((-?\d+),(-?\d+)\)', zl)
        if mm:
            pos[int(mm.group(1))] = (int(mm.group(2)), int(mm.group(3)))
    out = {}
    for h, (by, Q) in sorted(G.items()):
        if h not in pos:
            continue
        m, q, s, H = maske(Q, by, *pos[h])
        i0 = int(np.argmin(np.abs(s - 0.115)))
        yy, xx = np.mgrid[:480, :640]
        Qs = m & (np.hypot(xx - q[i0, 0], yy - q[i0, 1]) <= max(2.0, 0.02 * H))
        if not Qs.any() or m.sum() < 30:
            continue
        try:
            C = mittellinie(m, Qs, H)
            if len(sys.argv) > 3:
                C = C[int(float(sys.argv[3]) * len(C)):]   # Wurzelbereich (end-on, hinter dem Buckel) ausnehmen
            fo = form(m, C, H)
        except Exception as e:
            continue
        out[h] = dict(H=round(H, 1), max=fo['max_dwinkel'], knicke=fo['knicke'], knick_s=fo['knick_s'])
    for lo, hi, nm in ((84, 132, 'gang'), (16, 70, 'halten')):
        hs = [h for h in out if lo <= h <= hi]
        mx = max(hs, key=lambda h: out[h]['max'])
        print(nm, 'Takte', len(hs), 'max', out[mx]['max'], 'bei h%d' % mx, 'Knicke max', max(out[h]['knicke'] for h in hs),
              'Takte >36:', [h for h in hs if out[h]['max'] > 36], 'Takte Knicke>3:', [h for h in hs if out[h]['knicke'] > 3])
    for h in (84, 90, 96, 102, 108, 114, 120, 126, 132):
        print(h, out.get(h))
    if len(sys.argv) > 2:
        json.dump(out, open('C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/pz/gangmaske_%s.json' % sys.argv[2], 'w'), indent=1)
