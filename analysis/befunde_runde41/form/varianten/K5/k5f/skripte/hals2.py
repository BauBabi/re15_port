import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np
import rl
K5 = rl.K5
mat = json.load(open(K5 + '/ruhe/messung/material.json'))['material']
def liste(n):
    l = mat[n]['liste'] if isinstance(mat[n], dict) and 'liste' in mat[n] else mat[n]
    return set(l)
print({k: (len(v['liste']) if isinstance(v, dict) and 'liste' in v else '?') for k, v in mat.items()})
print('Beispiel', list(liste('Maulinneres'))[:5])
mo = rl.fl.lade_var(K5 + '/ruhe/K5e_f1')
fl_rot = {}; ein = {}
for name, az, el in (('unten', 0, -85), ('unten_seite', 90, -45), ('unten_3_4', 45, -40), ('unten_vorn', 0, -60)):
    img, K = rl.weiss_id(mo, 2, az, el, 500)
    lab = rl.fl.lab(img.reshape(-1,3)).reshape(img.shape)
    C = np.hypot(lab[...,1], lab[...,2])
    rot = K['maske'] & (lab[...,1] > 12) & (C > 15) & (K['mesh'] == 22)
    for f, e in zip(K['flaeche'][rot], K['eintrag'][rot]):
        key = '22|%d|%d' % (f // 100000, f % 100000)
        fl_rot[key] = fl_rot.get(key, 0) + 1
        ein[int(e)] = ein.get(int(e), 0) + 1
print('Flaechen', sorted(fl_rot.items(), key=lambda t: -t[1]))
mi = liste('Maulinneres'); au = liste('Auge'); ls = liste('Lippensaum'); ha = liste('Haut')
for k in fl_rot:
    print(k, 'Maulinneres' if k in mi else '', 'Auge' if k in au else '', 'Lippe' if k in ls else '', 'Haut' if k in ha else '')
print('Eintraege', sorted(ein.items(), key=lambda t: -t[1])[:20])
pal = mo.pal
import cv2
for e, n in sorted(ein.items(), key=lambda t: -t[1])[:12]:
    c = pal[0][e]; L = rl.fl.lab(np.array([c]))[0]
    print(e, n, c, np.round(L, 1))
