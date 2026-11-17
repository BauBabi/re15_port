# Unterkiefer: nur Flaechen teilen, deren Kantenmitten in der Textur auf ganze Texel fallen (alle UV-Differenzen gerade) -> Textur exakt gleich
import sys, json, copy
import numpy as np
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde as M
G = M.G; U = M.U
emd0, form0 = M.grundform()
tab = json.load(open(M.TABELLE))
f = copy.deepcopy(form0)
for t, n in tab['teile'].items():
    f['meshes'][int(t)] = copy.deepcopy(n)
m = f['meshes'][23]
def gerade(uv):
    uv = np.array(uv, int)
    return all(((uv[a] - uv[b]) % 2 == 0).all() for a in range(len(uv)) for b in range(a))
sel = set(); nd = nq = 0
for fi, d in enumerate(m['dreiecke']):
    if gerade(d['uv']): sel.add((0, fi)); nd += 1
for fi, q in enumerate(m['vierecke']):
    if gerade(q['uv']): sel.add((1, fi)); nq += 1
print('Teil 23: Dreiecke', len(m['dreiecke']), 'davon gerade', nd, '| Vierecke', len(m['vierecke']), 'davon gerade', nq)
U.unterteilen(f, {23: sel})
m = f['meshes'][23]
print('j1e Port-Dreiecke 23:', len(m['dreiecke']) + 2 * len(m['vierecke']))
open('j1e.EMD', 'wb').write(G.emd_mit_md1(emd0, f))
