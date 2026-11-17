# Versuch: Teil 16 NUR teilen (kein Zuruecknehmen) - Flaechen, durch die der Kopf geht (Clips 10/12), 1 und 2 Stufen.
import sys, pickle, copy, time
import numpy as np
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde as M
G = M.G; U = M.U
emd0, form0, form, hk, q = M.arbeitsform()
cb, R, T = M.posen_von(emd0, form)
bl = [i for i, (c, b, k) in enumerate(cb) if c in (10, 12)]
HK = G.Huellen(form, R[bl], T[bl], teile=M.KOPF)
t0 = time.time()
for k in (1, 2):
    sel = M.auswahl(form, R[bl], T[bl], HK, (16,))
    print('Stufe', k, 'Auswahl', len(sel[16]), '%.0f s' % (time.time() - t0), flush=True)
    hk = U.unterteilen(form, sel, hk)
    m = form['meshes'][16]
    print('  Teil 16: Ecken', len(m['ecken']), 'Port-Dreiecke', len(m['dreiecke']) + 2 * len(m['vierecke']), flush=True)
    open('s2n%d.EMD' % k, 'wb').write(G.emd_mit_md1(emd0, form))
    pickle.dump((form['meshes'][16], {a: b for a, b in hk.items() if a[0] == 16}), open('s2n%d.pkl' % k, 'wb'))
print('FERTIG')
