"""Gelenkwinkel, die das Video verlangt: Mittellinie je h (video_linien.py, Nutzer-Mittellinien mitte_h*.npy, y-40) in Sehnen der
Gliedlaenge 541 E. zerlegen (Bildmassstab aus der Blicktiefe des Schwanzes im Vorher-Lauf), Drehwinkel zwischen Nachbarsehnen."""
import sys, json, math, numpy as np
AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
sys.path.insert(0, AN)
from video_linien import LINIEN
from kam import proj, lese
G, Z, B = lese(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/bau/vorher/debug.log')

def resample(P, step):
    P = np.asarray(P, float)
    d = np.r_[0, np.cumsum(np.hypot(*np.diff(P, axis=0).T))]
    s = np.arange(0, d[-1] + 1e-6, step)
    return np.stack([np.interp(s, d, P[:, 0]), np.interp(s, d, P[:, 1])], 1), d[-1]

aus = {}
for h in range(18, 101, 2):
    sicht, P = LINIEN.get(h, ('nein', None))
    if sicht == 'nutzer':
        C = np.load(AN + '/mitte_h%d.npy' % h).copy(); C[:, 1] -= 40; P = C
    if P is None:
        continue
    g = G.get(h + 1)
    if g is None:
        continue
    _p, zt = proj(g)
    D = float(np.median(zt))
    step = 541.0 * 471.0 / D          # Videopunkte je Glied (x 2*231, y 2,079*231; Mittel)
    Q, L = resample(P, step / 4.0)    # fein, dann Sehnen je Glied
    S, _ = resample(P, step)
    w = []
    for i in range(1, len(S) - 1):
        a, b = S[i] - S[i - 1], S[i + 1] - S[i]
        w.append(abs(math.degrees(math.atan2(a[0] * b[1] - a[1] * b[0], a @ b))))
    # Knick ueber zwei Glieder (Sehne je 2 Glieder) - Mittel je Gelenk
    aus[h] = dict(sicht=sicht, tiefe=round(D), px_je_glied=round(step, 1), laenge_px=round(L, 1), glieder=round(L / step, 2),
                  winkel_je_gelenk=[round(x, 1) for x in w], max=round(max(w), 1) if w else None)
    print(h, aus[h])
json.dump(aus, open('knick_video.json', 'w'), indent=1)
