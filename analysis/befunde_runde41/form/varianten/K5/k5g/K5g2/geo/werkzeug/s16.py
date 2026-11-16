import sys, collections, time
import numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import kf_k5g2_mulde as M
G = M.G
emd0, f0 = M.grundform(); emd0, f1, hk = M.arbeitsform_stufe1()
cb, R, T = M.posen_von(emd0, f1)
# eindeutige Lage von 16 gegen 10
rel = {}
for i in range(len(R)):
    k = (R[i][10].T @ R[i][16]).tobytes() + (R[i][10].T @ (T[i][16] - T[i][10])).tobytes()
    rel.setdefault(k, i)
rep = sorted(rel.values())
print('Bilder', len(R), 'eindeutige Lagen 16 gegen 10:', len(rep))
HK = G.Huellen(f1, R, T, teile=(22, 23))
H0 = G.Huellen(f0, R, T, teile=(10,)); H1 = G.Huellen(f1, R, T, teile=(10,))
E = np.array(f0['meshes'][16]['ecken'], np.int64)
dk = np.full(len(E), -1e9); d0min = np.full(len(E), 1e9); d1min = np.full(len(E), 1e9); raus = np.zeros(len(E), bool)
for i in range(len(R)):
    W = G.welt(R[i], T[i], 16, E)
    dk = np.maximum(dk, HK.tiefe_max(i, W))
for i in rep:
    W = G.welt(R[i], T[i], 16, E)
    a = H0.tiefe_max(i, W); b = H1.tiefe_max(i, W)
    d0min = np.minimum(d0min, a); d1min = np.minimum(d1min, b)
    raus |= (a > 0) & (b < 0)
ims = dk > -16; verd = d0min > 0
print('Ecken 16:', len(E), '| im Kopf-Schwenk (>-16):', int(ims.sum()), '| in ALLEN Bildern in 10 (K5e_f1) verdeckt:', int(verd.sum()),
      '| im Schwenk UND immer verdeckt:', int((ims & verd).sum()), '| im Schwenk, nicht immer verdeckt:', int((ims & ~verd).sum()))
print('ragt aus der Mulde von 10 (in 10 alt, ausserhalb 10 neu, irgendein Bild):', int(raus.sum()), '| davon immer verdeckt', int((raus & verd).sum()),
      '| davon im Schwenk', int((raus & ims).sum()))
print('Tiefe im Kopf der sichtbaren Schwenk-Ecken:', np.sort(dk[ims & ~verd]).round(0).tolist())
print('d0min der sichtbaren Schwenk-Ecken:', np.sort(d0min[ims & ~verd]).round(0).tolist())
np.savez('s16.npz', dk=dk, d0min=d0min, d1min=d1min, raus=raus, rep=np.array(rep))
