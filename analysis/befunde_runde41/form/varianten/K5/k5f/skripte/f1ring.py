import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import numpy as np, cv2, rl, kf_k5e as E
kern = set(E.f1_kern_eintraege())
F1 = [(0, 1, q) for q in (7, 8, 19, 20, 31, 32)]
for d in sys.argv[1:]:
    mo = rl.fl.lade_var(d); out = []
    for wn, az, el in (('Seite', 0, 8), ('3/4 hinten', -135, 20), ('Gegenseite', 180, 8)):
        img, K = rl.weiss_id(mo, 2, az, el, 500)
        L = rl.fl.lab(img.reshape(-1, 3)).reshape(img.shape)[..., 0]
        f1 = np.zeros(L.shape, bool)
        for m, q, fi in F1: f1 |= (K['mesh'] == m) & (K['flaeche'] == q * 100000 + fi)
        f1 &= K['maske']
        if f1.sum() < 20: continue
        rk = f1 & ~np.isin(K['eintrag'], list(kern))
        nb = (cv2.dilate(f1.astype(np.uint8), np.ones((13, 13), np.uint8)) > 0) & ~f1 & K['maske'] & (K['mesh'] == 0)
        out.append('%s: F1 %d px, Nicht-Kern-Mittel %.1f, Nachbarhaut %.1f, dL %+.1f' % (wn, f1.sum(), L[rk].mean(), L[nb].mean(), L[rk].mean() - L[nb].mean()))
    print(d[-12:], ' | '.join(out))
