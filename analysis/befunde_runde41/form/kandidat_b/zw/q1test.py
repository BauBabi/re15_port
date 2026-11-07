import sys, json
sys.path.insert(0, '.')
from b_geom import *
import b_mesh as BM
import b_teile as T
import numpy as np
fit = BM.lade(sys.argv[1] if len(sys.argv) > 1 else 'zw/fit2.pt')
F = BM.Feld(fit, {'rumpf_m'}, 70.0)
c = np.array(T.KUPPEL_MITTE, float)
print('bbox', np.round(F.lo), np.round(F.hi))
g2, l2 = fit['gier2'], np.array(fit['lage2'])
print('gier2', g2, 'lage2', l2, 'dy2', fit['dy2'], 'Rumpf w', np.round(np.degrees(fit['w2'][[0, 9, 10]]), 1).tolist())
J = fit['J']
import b_bau as BB
R2, T2 = BB.fk(fit['w2'], J, T.ELTERN, fit['dy2'])
b = T.IDX['rumpf_m']


def oberflaeche(az, lat):
    a, l = math.radians(az), math.radians(lat)
    d = np.array([math.cos(a) * math.cos(l), -math.sin(l), math.sin(a) * math.cos(l)])
    t = np.linspace(1500, 300, 500)
    p = c + t[:, None] * d
    s = F(p.astype(np.float32))
    i = np.argmax(s < 0)
    return p[i]


def nach_bild2(p):
    q = R2[b] @ (p - J[b]) + T2[b]
    return FK.projiziere(K2, Y(g2) @ q + l2)


L = json.load(open(FORM + '/lesung_zeichnung.json', encoding='utf-8'))
M = {m['id']: m for m in L['merkmale']}
for i in ('B01', 'B02', 'B03', 'B04', 'B06', 'B07', 'B08', 'B09', 'B13', 'B14', 'B17', 'B20'):
    print(i, M[i].get('punkt'), M[i].get('farbe_roh_rgb'), M[i].get('raum_rest_y'))
for dz in (0, 10, 17, 25, 35):
    z = []
    for lat in (-20, 0, 15, 30, 45, 60, 75):
        p = oberflaeche(25 + dz, lat)
        z.append('(%d,%d)' % tuple(np.round(nach_bild2(p))))
    print('Azimut %d: ' % (25 + dz) + ' '.join(z))
for az, lat in ((25, 0), (86, 0), (132, 0)):
    p = oberflaeche(az, lat)
    print(az, lat, np.round(p), 'Bild1', np.round(bild(p), 0), 'Bild2', np.round(nach_bild2(p), 0))
