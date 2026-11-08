"""K5d: Lippensaum an K5c (messen.lippe_flaechen, Messblatt V4F 2.4) -> pickle fuer kf_k5d.py (lp['lc'], lp['lrot'], lp['keys']).
    python kf_k5d_lippe.py <aus.pkl>"""
import os, sys, collections, pickle
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'farbe_lesung', 'V4F_skripte'))
import numpy as np
import v4f, referenzen as RF, messen as M
oa = v4f.ST['Asset K5c']
F = RF.flaechen_texel(oa)
lc, lrot = M.lippe_flaechen(oa, F)
rot = [i for i in lrot if lrot[i] >= 3]
print('lippe', len(lc), 'rot', len(rot))
print(collections.Counter((F[i]['zeile'], F[i]['mesh']) for i in rot))
print('alle lippe', collections.Counter((F[i]['zeile'], F[i]['mesh']) for i in lc))
s, K = v4f.szene_id(oa, 2)
pos = {(f['mesh'], f['viereck'], f['fi']): i for i, f in enumerate(F)}
vis = collections.Counter()
for y, x in zip(*np.nonzero(K['maske'])):
    key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
    vis[pos[key]] += 1
z6 = [i for i, f in enumerate(F) if f['zeile'] == 6]
print('Zeile 6 Flaechen', len(z6), 'sichtbar Bild 2:', [(F[i]['mesh'], vis[i], i in lc, lrot.get(i, 0)) for i in z6])
z0rot = [i for i in rot if F[i]['zeile'] == 0]
print('Z0 rot Lippe sichtbar Bild 2 px:', sum(vis[i] for i in z0rot), [(F[i]['mesh'], vis[i]) for i in z0rot if vis[i]])
z14rot = [i for i in rot if F[i]['zeile'] == 14]
print('Z14 rot Lippe sichtbar Bild 2 px:', sum(vis[i] for i in z14rot))
pickle.dump(dict(lc=dict(lc), lrot=dict(lrot), keys=[(f['mesh'], f['viereck'], f['fi']) for f in F]), open(sys.argv[1], 'wb'))
