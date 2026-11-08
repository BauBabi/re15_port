# Zahlen-Pruefer V3 r2: Schwanz im ECHTEN Spielbild (Lauf mit Marker-EMD PZ2_markiert.py, Maske = gruen UND Differenz zum
# unmarkierten Lauf), Mittellinie/Winkel/Knicke wie R25/R26 (PZ_mess.form), H aus der Projektion Buckel..Sohle (PZ_gang_maske).
# python PZ2_gang_bild.py LAUF LAUF_MARKIERT AUSGABE.json
import os, sys, json, re
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2')); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import c6, b2
from PZ_gang import lesen
from PZ_mess import mittellinie, form
HE = 3829.0
A, M, AUS = sys.argv[1:4]
G, _ = lesen(os.path.join(A, 'debug.log'))
pos = {}
for zl in open(os.path.join(A, 'debug.log'), errors='replace'):
    mm = re.search(r'\[hangar-takt\] h(-?\d+) F\d+ B clip \d+ bild \d+ gier -?\d+ \((-?\d+),(-?\d+)\)', zl)
    if mm: pos[int(mm.group(1))] = (int(mm.group(2)), int(mm.group(3)))
welt = dict(gier=0, x=0, z=0, y=0)
out = {}
for h in sorted(G):
    a, b = c6.port(A, h), c6.port(M, h)
    if a is None or b is None or h not in pos: continue
    g = (b[..., 1].astype(int) - np.maximum(b[..., 0], b[..., 2]).astype(int)) > 40
    d = np.abs(a.astype(int) - b.astype(int)).sum(2) > 30
    m = g & d
    by, Q = G[h]
    x, z = pos[h]
    pr = b2.projiziere(welt, np.array([[x, by - HE, z], [x, by, z]], float)); H = float(abs(pr[1, 1] - pr[0, 1]))
    if m.sum() < 30:
        out[h] = dict(px=int(m.sum()), H=round(H, 1)); continue
    lb, k = nd.label(m); groesse = nd.sum(m, lb, range(1, k + 1))
    mf = lb == 1 + int(np.argmax(groesse))
    q = b2.projiziere(welt, Q)
    yy, xx = np.mgrid[:480, :640]
    dd = np.hypot(xx - q[0, 0], yy - q[0, 1]); dd[~mf] = 1e9
    S = np.zeros_like(mf); S[np.unravel_index(np.argmin(dd), dd.shape)] = True
    try:
        C = mittellinie(mf, S, H); fo = form(mf, C, H)
    except Exception as e:
        out[h] = dict(px=int(m.sum()), fehler=str(e)); continue
    out[h] = dict(px=int(m.sum()), teile=int(k), H=round(H, 1), sicht_laenge_H=fo['laenge_H'], max=fo['max_dwinkel'], knicke=fo['knicke'],
                  knick_s=fo['knick_s'])
json.dump(out, open(AUS, 'w'), indent=1)
for lo, hi, nm in ((84, 132, 'gang'), (16, 70, 'halten')):
    hs = [h for h in out if lo <= h <= hi and 'max' in out[h] and out[h]['sicht_laenge_H'] >= 0.15]
    if not hs: continue
    mx = max(hs, key=lambda h: out[h]['max'])
    print(nm, 'Takte mit >= 0,15 H sichtbarem Schwanz', len(hs), 'max', out[mx]['max'], 'bei h%d' % mx, 'Knicke max', max(out[h]['knicke'] for h in hs),
          '>36:', [h for h in hs if out[h]['max'] > 36], 'Knicke>3:', [h for h in hs if out[h]['knicke'] > 3])
for h in (84, 90, 96, 102, 108, 114, 120, 126, 132):
    print(h, out.get(h))
