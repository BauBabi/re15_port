# Bild mit dem groessten Abstand Kopf - Mulde (Welt): kleinster Abstand der Kopf-Ecken (22, 23) zu den Mulden-Ecken (geteilte Flaechen von 10, 21)
import sys, json
import numpy as np
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde as M
G = M.G
emd0, form0, form, hk, q = M.arbeitsform()
cb, R, T = M.posen_von(emd0, form)
mul = {}
for t in (10, 21):
    m = form['meshes'][t]; s = set()
    for (tt, vi, fi), ur in hk.items():
        if tt == t and ur != (tt, vi, fi):
            s.update((m['vierecke'] if vi else m['dreiecke'])[fi]['e'])
    mul[t] = np.array(m['ecken'], np.int64)[sorted(s)]
    print('Teil', t, 'Mulden-Ecken', len(mul[t]))
kopf = {t: np.array(form['meshes'][t]['ecken'], np.int64) for t in (22, 23)}
d = np.zeros(len(R))
for i in range(len(R)):
    A = np.concatenate([G.welt(R[i], T[i], t, mul[t]) for t in (10, 21)])
    B = np.concatenate([G.welt(R[i], T[i], t, kopf[t]) for t in (22, 23)])
    d[i] = np.sqrt(((A[:, None, :] - B[None, :, :]) ** 2).sum(2)).min()
o = np.argsort(-d)
print('groesster Abstand:', [(int(cb[i][0]), int(cb[i][1]), round(float(d[i]), 1)) for i in o[:8]])
print('kleinster:', [(int(cb[i][0]), int(cb[i][1]), round(float(d[i]), 1)) for i in o[-3:]])
# mittlerer Abstand als zweites Mass
dm = np.zeros(len(R))
for i in range(len(R)):
    A = np.concatenate([G.welt(R[i], T[i], t, mul[t]) for t in (10, 21)]).mean(0)
    B = G.welt(R[i], T[i], 22, kopf[22]).mean(0)
    dm[i] = np.linalg.norm(A - B)
o2 = np.argsort(-dm)
print('groesster Schwerpunkt-Abstand:', [(int(cb[i][0]), int(cb[i][1]), round(float(dm[i]), 1)) for i in o2[:8]])
json.dump(dict(naechst=[[int(cb[i][0]), int(cb[i][1]), float(d[i])] for i in range(len(R))], schwer=[float(x) for x in dm]), open('hm.json', 'w'))
