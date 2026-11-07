import sys, json
sys.path.insert(0, '.')
from b_geom import *
import b_teile as T
import numpy as np
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
import form_buehne as FB
k = P.Kandidat(sys.argv[1] if len(sys.argv) > 1 else 'zw/v3/kandidat.json')
kam = P.kamera(1)
lage, gier, clip, bild = k.stand(1)
kf = k.kf(clip, bild)
print(lage, gier, clip, bild, kf, len(k.mo.meshes), k.mo.skel.bone_count)
R, Tt = k.mo.pose(kf)
for b in (0, 9, 10, 5):
    m = k.mo.meshes[b]
    print(b, 'tv', len(m.tv), 'qv', len(m.qv), 'tf', len(m.tf), 'qf', len(m.qf), 'T', Tt[b], 'R0', R[b][0])
    tr = FB.dreiecke(k.mo, kf, lage, gier, kam, None, teile={b})
    print('   Dreiecke sichtbar', len(tr), tr[0][1] if tr else None)
