import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, rl
mat = json.load(open(rl.K5 + '/ruhe/messung/material.json'))['material']
wo = {}
for nm, v in mat.items():
    for s in v['liste']: wo[s] = nm
for d in sys.argv[1:]:
    mo = rl.fl.lade_var(d); z = {}
    for az, el in ((0, -85), (90, -45), (45, -40), (0, -60), (-90, -45)):
        img, K = rl.weiss_id(mo, 2, az, el, 500)
        lab = rl.fl.lab(img.reshape(-1, 3)).reshape(img.shape)
        m = K['maske'] & np.isin(K['mesh'], (21, 22, 23)) & (K['zeile'] != 9) & (lab[..., 1] > 12) & (np.hypot(lab[..., 1], lab[..., 2]) > 15) & (np.degrees(np.arctan2(lab[..., 2], lab[..., 1])) > -10)
        for ms, f, ze in zip(K['mesh'][m], K['flaeche'][m], K['zeile'][m]):
            k = '%d|%d|%d' % (ms, f // 100000, f % 100000); key = (wo.get(k, '?'), int(ze))
            z[key] = z.get(key, 0) + 1
    print(d[-12:], sorted(z.items(), key=lambda t: -t[1]))
