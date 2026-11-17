import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, rl
AUSSEN = {(22, 1, 1), (22, 1, 8), (22, 1, 9), (22, 1, 194), (22, 1, 201), (22, 1, 209)}
for d in sys.argv[1:]:
    for n in (1, 2):
        s, K = rl.v4f.szene_id(d, n)
        lab = rl.fl.lab(s.reshape(-1, 3)).reshape(s.shape)
        a = np.zeros(s.shape[:2], bool)
        for (m, q, fi) in AUSSEN:
            a |= (K['mesh'] == m) & (K['flaeche'] == q * 100000 + fi)
        a &= K['maske']
        rot = a & (lab[..., 1] > 6) & (np.degrees(np.arctan2(lab[..., 2], lab[..., 1])) > -10)
        print(d[-10:], 'Bild', n, 'Aussen-Maulflaechen px', int(a.sum()), 'davon warm-rot (a*>6, h>-10)', int(rot.sum()),
              'Lab-Mittel', np.round(lab[a].mean(0), 1) if a.any() else '-', 'Zeile', np.unique(K['zeile'][a]))
