# Zahlen-Pruefer V3 r2: Schwanzfarbe gegen Koerperhaut im ECHTEN Spielbild (R15-R19-Masse wie PZ_mess.farbe), Masken aus den
# Markerlaeufen (Schwanz 16-22 / Koerper 0-15 ohne Kopf 9 gruen), 2 px erodiert, Haut = Band OK..OK+0,45 H, a* >= -3, L* >= 5.
# python PZ2_farbe_spiel.py LAUF LAUF_SCHWANZ LAUF_KOERPER AUS.json
import os, sys, json
import numpy as np
from scipy import ndimage as nd
from skimage.color import rgb2lab, deltaE_ciede2000
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import c6
A, MS, MK, AUS = sys.argv[1:5]
def maske(a, b):
    return ((b[..., 1].astype(int) - np.maximum(b[..., 0], b[..., 2]).astype(int)) > 40) & (np.abs(a.astype(int) - b.astype(int)).sum(2) > 30)
out = {}
for h in range(-10, 140):
    a, s, k = c6.port(A, h), c6.port(MS, h), c6.port(MK, h)
    if a is None or s is None or k is None: continue
    t = nd.binary_erosion(maske(a, s), iterations=2); kb = maske(a, k)
    if t.sum() < 150 or kb.sum() < 1000: continue
    ys = np.nonzero(kb.any(1))[0]; OK, so = ys.min(), ys.max(); H = so - OK + 1
    band = np.zeros_like(kb); band[OK:int(OK + 0.45 * H) + 1] = True
    L = rgb2lab(a[..., ::-1] / 255.0)
    haut = nd.binary_erosion(kb & band, iterations=2) & (L[..., 2] >= -3) & (L[..., 0] >= 5)
    ls, lh = L[t].mean(0), L[haut].mean(0)
    cs, ch = np.hypot(*ls[1:]), np.hypot(*lh[1:])
    out[h] = dict(n_s=int(t.sum()), L=round(ls[0] / lh[0], 3), C=round(cs / ch, 3),
                  dh=round(np.degrees(np.arctan2(ls[2], ls[1]) - np.arctan2(lh[2], lh[1])), 1),
                  rot_s=round(float((L[t][:, 1] > 15).mean()), 3), rot_h=round(float((L[haut][:, 1] > 15).mean()), 3),
                  de00=round(float(deltaE_ciede2000(ls[None], lh[None])[0]), 2), lab_s=[round(v, 1) for v in ls], lab_h=[round(v, 1) for v in lh])
json.dump(out, open(AUS, 'w'), indent=1)
def rot(o): return o['rot_s'] <= 0.8 * o['rot_h']
for h, o in out.items():
    ok = 0.50 <= o['L'] <= 0.72 and 0.45 <= o['C'] <= 0.90 and abs(o['dh']) <= 10 and rot(o) and 6 <= o['de00'] <= 11
    o['alle_ok'] = ok
print('Takte', len(out), 'davon R15-R19 alle gruen', sum(o['alle_ok'] for o in out.values()))
for key, lo, hi in (('L', .5, .72), ('C', .45, .9), ('de00', 6, 11)):
    v = [o[key] for o in out.values()]; print(key, 'min', min(v), 'median', round(float(np.median(v)), 3), 'max', max(v), 'ausserhalb', sum(1 for x in v if not lo <= x <= hi))
v = [o['dh'] for o in out.values()]; print('dh min', min(v), 'max', max(v), 'ausserhalb', sum(1 for x in v if abs(x) > 10))
print('Rot ausserhalb', sum(1 for o in out.values() if not rot(o)))
for h in (84, 90, 96, 102, 108, 120, 132, 20, 30, 60, 65):
    if h in out: print(h, out[h])
