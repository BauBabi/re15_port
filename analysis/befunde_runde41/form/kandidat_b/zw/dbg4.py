import sys, pickle
sys.path.insert(0, '.')
from b_geom import *
import b_teile as T
import numpy as np
import b_snap as SN, b_alles as AL, b_mesh as BM, b_naht as BN, torch
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
import form_buehne as FB
k = P.Kandidat('zw/g9/v/kandidat.json')
kam = P.kamera(1)
lage, gier, clip, bild = k.stand(1)
kf = k.kf(clip, bild)
pix = [(312, 349), (311, 350), (312, 350), (314, 350), (313, 350), (313, 349)]
for b in range(21):
    a = np.zeros((480, 640, 3), np.uint8)
    m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None, teile={b}), k.tex, a, kam.abbildung)
    c = sum(int(m[y, x]) for x, y in pix)
    if c:
        print('Knochen', b, T.KNOCHEN[b][0], c)
fit = torch.load('zw/g9/fit.pt', map_location='cpu', weights_only=False)
V, w2, l2, g2 = pickle.load(open('zw/g9/snap_fein.pkl', 'rb'))
te = SN.stuecke('zw/g9/quad')
for st in te:
    st['V'] = np.array(V[st['name']], float)
fit['lage2'], fit['gier2'] = l2, g2
s = AL.alle_sichten(fit, te, w2)[1]
for st in te:
    if st['name'] not in ('k16', 'k17', 'k18', 'k19'):
        continue
    kon, w = s.konturecken(st)
    q = s.proj(w)
    d = np.linalg.norm(q - [312, 350], axis=1)
    for i in np.argsort(d)[:7]:
        print(st['name'], i, np.round(q[i], 1).tolist(), 'kon', bool(kon[i]), round(float(d[i]), 1))
