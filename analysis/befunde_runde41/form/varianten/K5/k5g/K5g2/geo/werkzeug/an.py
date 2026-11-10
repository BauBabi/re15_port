import sys, time
import numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import kf_k5g2_mulde as M
G = M.G
emd0, f0 = M.grundform()
cb, R, T = M.posen_von(emd0, f0)
E = np.array(f0['meshes'][16]['ecken'], np.int64)
print('Teil 16 Ecken', len(E), 'bbox', E.min(0).tolist(), E.max(0).tolist(), 'Schwerpunkt', E.mean(0).round(0).tolist())
bl = [i for i, (c, b, k) in enumerate(cb) if c in (10, 12)]
HK = G.Huellen(f0, R, T, teile=(22, 23))
lo, hi = E.min(0), E.max(0)
st = 60
ax = [np.arange(lo[k], hi[k] + 1, st) for k in range(3)]
Pg = np.array(np.meshgrid(*ax, indexing='ij')).reshape(3, -1).T.astype(np.int64)
tri16 = np.array([[E[a] for a in t] for t in G.dreiecke_idx(f0['meshes'][16])], float)
d16 = G.tiefe(Pg.astype(float), tri16)
print('Gitter', len(Pg), 'in 16:', int((d16 > 0).sum()))
t0 = time.time()
inn = np.where(d16 > 0)[0]
print('Kandidaten in 16:', len(inn), flush=True)
dk = np.full(len(Pg), 1e9)
dk[inn] = -1e9
for n, i in enumerate(bl):
    dk[inn] = np.maximum(dk[inn], HK.tiefe_max(i, G.welt(R[i], T[i], 16, Pg[inn])))
    if n % 20 == 0: print(n, len(bl), '%.0f s' % (time.time() - t0), flush=True)
print('%.0f s' % (time.time() - t0))
c = E.mean(0)
for res in (16, 40, 80, 120):
    ok = (dk <= -res) & (d16 > 0)
    if ok.any():
        j = np.where(ok)[0]; a = j[np.argmin(np.linalg.norm(Pg[j] - c, axis=1))]; b = j[np.argmax(d16[j])]
        print('Reserve', res, 'Kandidaten', len(j), '| naechster am Schwerpunkt', Pg[a].tolist(), 'Abstand %.0f' % np.linalg.norm(Pg[a] - c), 'Kopf %.0f in16 %.0f' % (dk[a], d16[a]),
              '| tiefster in 16', Pg[b].tolist(), 'Kopf %.0f in16 %.0f' % (dk[b], d16[b]))
    else:
        print('Reserve', res, 'kein Punkt in 16 ausserhalb des Schwenks')
np.savez('anker16.npz', Pg=Pg, dk=dk, d16=d16)
