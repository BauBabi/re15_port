import sys, math
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import numpy as np, kf_k5e as E
X = E.eingaenge(); F, mat, quell = X['F'], X['mat'], X['quell']
zs, ws, ls, cs = [], [], [], []
for k, (nm, s) in E.material_seite(F, mat, quell).items():
    if nm not in ('Haut', 'Gelenk', 'Lippensaum', 'Kralle'): continue
    z = [int(x) for x in X['cl_g4'][s]]
    L, haut, hell, ader = E.klassen(z)
    idx = F[k][2]; m = haut[idx]
    l = L[idx[m]]
    C = np.hypot(l[:, 1], l[:, 2])
    zs.append((C * np.exp(1j * np.arctan2(l[:, 2], l[:, 1]))).sum()); ls.append(l[:, 0]); cs.append(C)
z = sum(zs); L = np.concatenate(ls); C = np.concatenate(cs)
print('G4-Haut auf allen Haut/Gelenk-Flaechen (Texelzugriffe):', len(L), 'Ton C-gew.', round(math.degrees(np.angle(z)), 1),
      'C mittel', round(C.mean(), 2), 'L mittel', round(L.mean(), 2), 's = sum C / sum L', round(C.sum() / L.sum(), 3), 'Median C/L', round(float(np.median(C / np.maximum(L, 1))), 3))
