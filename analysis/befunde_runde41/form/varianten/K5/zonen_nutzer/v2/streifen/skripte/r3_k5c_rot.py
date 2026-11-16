"""Runde 3: Woher kommt das Rot auf Hals/Kopf/Kiefer von unten in K5c (a005b357a) gegenueber K5e_f1? Clip 2 Bild 0, Kamera 'von unten'.
Je Flaeche die roten Tiefenpuffer-Bildpunkte."""
import json
import sys
from collections import Counter
from s_basis import *  # noqa
import s_hals as SH
import r3_rot_alle as RA
V = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/streifen/versionen/'
out = {}
for nm, o in (('K5c a005b357a', V + 'a005b357a'), ('K5e_f1', ASSET)):
    k = kand(o)
    kam, lage, gier = RA.kameras()['von unten']
    for clip, bild in ((2, 0), (5, 7)):
        tris = dreiecke_3d(k.mo, k.kf(clip, bild), lage, gier, kam)
        R = rastern_beide(tris, k.tex, 480, 640, np.diag([2.0, 2.0, 1.0]))
        ip = R['puffer_i']; tm = tri_mesh(tris)
        mp = np.where(ip >= 0, tm[np.maximum(ip, 0)], -1)
        rp = SH.rot(R['rgb_puffer']) & (ip >= 0) & np.isin(mp, (21, 22, 23))
        c = Counter(SH.schluessel(tris[i][4]) for i in ip[rp].tolist())
        out['%s Clip %d/%d' % (nm, clip, bild)] = dict(rot=int(rp.sum()), je_teil=dict(Counter(int(x.split('|')[0]) for x in c.elements())),
                                                      flaechen=dict(c.most_common(10)))
json.dump(out, open(ZIEL + '/messung/r3_k5c_rot.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
print(json.dumps(out, ensure_ascii=False, indent=1))
