"""G9: Grundhelligkeit der Original-Texturseiten aus den Original-Texeln, die das RE2-G4-Modell SELBST liest (keine K5-Region, kein Video)."""
import sys, json, collections
sys.dont_write_bytecode = True
G = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, G)
import numpy as np
import kf_k5g2_tex as TX, kf_k5e as E, kf_g4 as KG
X = E.eingaenge()
cl = [[int(x) for x in z] for z in X['cl_g4']]
lab = [TX.lab_zeile(z) for z in cl]
kl = [E.klassen(z) for z in cl]
mo, t, q = KG.g4_modell()
pxg, clg = KG.g4_cluts(t)
print('G4 atlas', np.asarray(pxg).shape, 'gleich K5e-Pixel Seiten 0-2:', bool((np.asarray(pxg)[:, :384] == X['T']['pixel'][:, :384]).all()) if np.asarray(pxg).shape[1] >= 384 else '?')
pxg = np.asarray(pxg)
n = {s: np.zeros(256) for s in range(4)}
nseite = collections.Counter()
cy_set = collections.Counter()
for mi in range(len(mo.meshes)):
    for V, T, cw, qq in KG.flaechen(mo, mi):
        s = int(T[0, 0]) // 128
        tx = KG.texel_im_dreieck(T)
        if not len(tx): continue
        idx = pxg[tx[:, 1], tx[:, 0]]
        cy_set[(s, (cw >> 6) & 0x1FF)] += 1
        np.add.at(n[s], idx, 1); nseite[s] += 1
print('G4-Dreiecke je Seite', dict(nseite), 'Seite/CLUT-y', dict(cy_set))
res = {}
for s in range(3):
    L = kl[s][0][:, 0]; haut = kl[s][1]
    sel = haut & (n[s] > 0) & (np.array(cl[s]) != 0)
    sel_all = (n[s] > 0) & (np.array(cl[s]) != 0)
    m = TX.gew_quantil(L[sel], n[s][sel], 0.5)
    ma = TX.gew_quantil(L[sel_all], n[s][sel_all], 0.5)
    res[s] = (m, ma, n[s][sel].sum(), n[s][sel_all].sum())
    print('Seite %d: G4-Modell liest %d Texel (Hautklasse %d); Median L* Hautklasse %.3f, alle Klassen %.3f' % (s, n[s][sel_all].sum(), n[s][sel].sum(), m, ma))
for s in range(3):
    print('g_%d = Median Seite 1 / Seite %d: Hautklasse %.4f, alle %.4f' % (s, s, res[1][0] / res[s][0], res[1][1] / res[s][1]))
json.dump({str(s): dict(median_haut=round(res[s][0], 3), median_alle=round(res[s][1], 3), texel_haut=int(res[s][2]), texel_alle=int(res[s][3]),
                        g_haut=round(res[1][0] / res[s][0], 4), g_alle=round(res[1][1] / res[s][1], 4)) for s in range(3)},
          open('seiten_g4.json', 'w'), indent=1)
