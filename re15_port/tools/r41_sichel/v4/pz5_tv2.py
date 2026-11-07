"""Zahlen-Pruefer V4 r5: TV (O8) als Querschnittsmittel - waagerechter Teil: Spalten x +-3 um die Station (erodierte Maske, y < 262);
senkrechter Teil/Endstueck: Zeilen y +-3 (x 270-315); Biegung: Kreis r 0,03 H. Stationen auf dem Polygonzug der Punkte (pz5_neu).
Original roh (3 Leser-Masken) und weich (Gauss 1,5); Port Videoraum."""
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import t2_mass_v4 as T2M
from pz5_mess import lab, orig, vraum, SP
yy, xx = np.mgrid[:480, :640]
def maske(f):
    d = np.load(f); return np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)
def quer(V, m, pk, H):
    L = lab(V); me = nd.binary_erosion(m); pk = [np.array(p, float) for p in pk]
    st = dict(Wurzel=('x', pk[0] + 0.15 * (pk[1] - pk[0])), Mitte=('x', pk[0] + 0.5 * (pk[1] - pk[0])), Biegung=('k', pk[1]),
              senkrecht=('y', pk[1] + 0.5 * (pk[2] - pk[1])), Endstueck=('y', pk[2] + 0.5 * (pk[3] - pk[2])))
    r = {}
    for k, (a, p) in st.items():
        if a == 'x':
            mm = me & (np.abs(xx - p[0]) <= 3) & (yy < 262)
        elif a == 'y':
            mm = me & (np.abs(yy - p[1]) <= 3) & (xx >= 270) & (xx <= 315)
        else:
            mm = me & (np.hypot(xx - p[0], yy - p[1]) <= 0.03 * H)
        r[k] = [round(float(v), 1) for v in L[mm].mean(0)[:2]] + [int(mm.sum())]
    return r
O2 = orig(2); O2g = cv2.GaussianBlur(O2.astype(np.float32), (0, 0), 1.5).astype(float)
pko = [(357, 243), (295.0, 252.1), (290, 308), (295, 343)]
ro = [quer(O2, maske(f), pko, 253.0) for f in T2M.MASKEN]; rw = [quer(O2g, maske(f), pko, 253.0) for f in T2M.MASKEN]
mit = {k: np.mean([r[k][:2] for r in ro], 0) for k in ro[0]}
print('Original roh', ro); print('Original weich', rw)
M = json.load(open(SP + 'mess_V4_V4R4_V3.json'))
for nm in ('V4', 'V4R4'):
    d = np.load(SP + 'r_%s_85.npz' % nm); sch = json.load(open(SP + 'schwanz_%s.json' % nm)); T = M[nm]['seite']['T']
    pk = [sch['Id16']['p'], sch['Id16']['T2']['biegung'], T['T3']['p'], T['T4']['p']]
    r = quer(vraum(d['img']), (d['ids'] >= 16) & (d['ids'] <= 22), pk, M[nm]['seite']['H'])
    print(nm, {k: (v, 'OK' if abs(v[0] - mit[k][0]) <= 4 and abs(v[1] - mit[k][1]) <= 3 else 'ROT %+.1f/%+.1f' % (v[0] - mit[k][0], v[1] - mit[k][1])) for k, v in r.items()})
