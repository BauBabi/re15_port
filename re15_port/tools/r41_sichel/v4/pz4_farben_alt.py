"""Zahlen-Pruefer V4 r4 (Kopie pz2_farben_alt, Renders sfv4/pz4): F7/F8/F14 nach Messblatt-Wortlaut (Id ganz, ohne/mit 2 px Erosion) und nach der Runde-2-Umdeutung des Bauers
(obere 58 % / 56 % der Zeilen von Id 13 / Id 10), eigene Rechnung auf den pz2-Renders; ausserdem F-Mittel in beiden Lesarten."""
import numpy as np, json
from scipy import ndimage as nd
from pz4_mess import vraum, lab, de, SP
d = np.load(SP + 'r_V4_135.npz'); V = vraum(d['img']); ids = d['ids']; LAB = lab(V)
d2 = np.load(SP + 'r_V4_85.npz'); V2 = vraum(d2['img']); ids2 = d2['ids']; LAB2 = lab(V2)
er = lambda m: nd.binary_erosion(m, iterations=2)
def teil(m, f0, f1):
    ys = np.nonzero(m.any(1))[0]; y0, y1 = ys.min(), ys.max() + 1
    b = np.zeros_like(m); b[int(round(y0 + f0 * (y1 - y0))):int(round(y0 + f1 * (y1 - y0)))] = True; return m & b
Z = dict(F7=[33.3, 5.5, 2.7], F8=[16.4, 8.3, 1.5], F14=[60.0, 11.0, 1.2])
out = {}
def mess(k, Vx, m, z):
    l = lab(Vx[m].mean(0)); out[k] = dict(lab=[round(float(x), 1) for x in l], n=int(m.sum()), dE=round(de(l, z), 2), dL=round(float(l[0] - z[0]), 1))
mess('F7 Id13 ganz', V, ids == 13, Z['F7']); mess('F7 Id13 ganz er2', V, er(ids == 13), Z['F7'])
mess('F7 Id13 obere 58% er2 (Bauer r2)', V, teil(er(ids == 13), 0, 0.58), Z['F7'])
mess('F8 Id10 ganz', V, ids == 10, Z['F8']); mess('F8 Id10 ganz er2', V, er(ids == 10), Z['F8'])
mess('F8 Id10 obere 56% er2 (Bauer r2)', V, teil(er(ids == 10), 0, 0.56), Z['F8'])
mess('F7 Fensterlage er2 (Bauer r3 F7)', V, teil(er(ids == 13), 0, 0.58), Z['F7'])
for k6, m6 in (('F6 Id8 RG>=15', (ids == 8)), ('F6 Id8 RG>=15 er2', er(ids == 8))):
    RG = V[..., 0] - V[..., 1]; mess(k6, V, m6 & (RG >= 15), [16.6, 17.3, 6.2])
L2 = LAB2[..., 0]
for nm, m8 in (('ohne Erosion', ids2 == 8), ('er2', er(ids2 == 8))):
    p70 = np.percentile(L2[m8], 70); mess('F14 Id8 L>=P70 %s' % nm, V2, m8 & (L2 >= p70), Z['F14'])
for k, v in out.items(): print('%-36s %s' % (k, v))
m = json.load(open(SP + 'mess_V4_V4R3_V3.json'))['V4']
F = dict(m['vorn']['F']); F.update(m['seite']['F'])
basis = [F[k]['dE'] for k in ('F6', 'F9', 'F10_id12', 'F19')]
print('F-Mittel Messblatt-Lesart (F7/F8 Id ganz, F14 ohne Erosion):', round(np.mean(basis + [out['F7 Id13 ganz']['dE'], out['F8 Id10 ganz']['dE'], out['F14 Id8 L>=P70 ohne Erosion']['dE']]), 2))
print('F-Mittel Lesart F7 Fensterlage, F8 Id ganz, F14 er2:', round(np.mean(basis + [out['F7 Fensterlage er2 (Bauer r3 F7)']['dE'], out['F8 Id10 ganz er2']['dE'], out['F14 Id8 L>=P70 er2']['dE']]), 2))
