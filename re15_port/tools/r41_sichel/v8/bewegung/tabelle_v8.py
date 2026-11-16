"""Schwanzbahn V8 je Videobild aus den Videolinien (video_linien.py; Nutzer-Mittellinien mitte_h22/24/64/90.npy, y-40):
Spitze = letzter Punkt, Austritt = erster Punkt, Durchgangsgelenk aus der sichtbaren Laenge (Glieder = Laenge / Videopunkte je Glied aus
knick_video.json; Kette ab Gelenk j bis zur Nadelspitze = 7 - j + 759/541 Glieder), Bauch der Linie (Seite, auf der die Zwischenpunkte
neben der Sehne liegen, Pfeil in Videopunkten)."""
import sys, json, math, numpy as np
AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
sys.path.insert(0, AN)
from video_linien import LINIEN
K = json.load(open('knick_video.json'))
NADEL = (1300 - 541) / 541.0
aus = {}
for h in range(18, 101, 2):
    sicht, P = LINIEN.get(h, ('nein', None))
    if sicht == 'nutzer':
        C = np.load(AN + '/mitte_h%d.npy' % h).copy(); C[:, 1] -= 40; P = C
    if P is None:
        continue
    P = np.asarray(P, float)
    k = K.get(str(h))
    gl = k['glieder']
    j = int(round(7 + NADEL - gl)); j = max(1, min(6, j))
    a, e = P[0], P[-1]; d = e - a; n = np.array([d[1], -d[0]]) / np.linalg.norm(d)
    off = (P[1:-1] - a) @ n if len(P) > 2 else np.array([0.0])
    pfeil = float(off[np.argmax(np.abs(off))]) if len(off) else 0.0
    aus[h] = dict(sicht=sicht, spitze=[int(round(v)) for v in e], austritt=[int(round(v)) for v in a], via_j=j, glieder=gl,
                  pfeil=round(pfeil, 1), seite=(1 if pfeil > 0 else -1) if abs(pfeil) >= 3 else 0)
    print(h, aus[h])
json.dump(aus, open('tabelle_v8.json', 'w'), indent=1)
