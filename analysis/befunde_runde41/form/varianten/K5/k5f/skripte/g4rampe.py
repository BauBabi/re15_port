import sys, json, math
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import numpy as np
import kf_k5e as E, kf_g4 as KG
X = E.eingaenge()
F, mat, quell = X['F'], X['mat'], X['quell']
nutz = {}
for k, (nm, s) in E.material_seite(F, mat, quell).items():
    n = nutz.setdefault(s, np.zeros(256)); np.add.at(n, F[k][2], 1)
for s in range(3):
    z = [int(x) for x in X['cl_g4'][s]]
    L, haut, hell, ader = E.klassen(z)
    w = nutz[s]
    for nm, m in (('haut', haut), ('hell', hell), ('ader', ader)):
        mm = m & (w > 0)
        if not mm.any(): continue
        ww = w[mm]; l = L[mm]
        C = np.hypot(l[:, 1], l[:, 2]); h = np.degrees(np.arctan2(l[:, 2], l[:, 1]))
        zc = (ww * C * np.exp(1j * np.radians(h))).sum()
        print('Seite', s, nm, 'Eintraege', int(mm.sum()), 'Zugriffe', int(ww.sum()), 'L p10/50/90', [round(E.gew_quantil(l[:, 0], ww, q), 1) for q in (.1, .5, .9)],
              'C mittel', round(float((ww * C).sum() / ww.sum()), 1), 'Ton (C-gew.)', round(math.degrees(np.angle(zc)), 1),
              'a* mittel', round(float((ww * l[:, 1]).sum() / ww.sum()), 1))
