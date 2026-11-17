import sys, json, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, cv2
import rl, fam
S = sys.argv[1]; aus = {}
ROT = lambda lab: (lab[..., 1] > 12) & (np.hypot(lab[..., 1], lab[..., 2]) > 15)
for name in sys.argv[2:]:
    d = name if '/' in name else S + '/vorschau/' + name
    mo = rl.fl.lade_var(d)
    W = []; kach = []
    for wn, az, el in rl.WINKEL4:
        img, K = rl.weiss_id(mo, 2, az, el, 380); W.append(fam.alles(img, K['maske'])); kach.append(img)
    hals = 0; hpx = 0
    for az, el in ((0, -85), (90, -45), (45, -40), (0, -60), (-90, -45)):
        img, K = rl.weiss_id(mo, 2, az, el, 500)
        lab = rl.fl.lab(img.reshape(-1, 3)).reshape(img.shape)
        m = K['maske'] & np.isin(K['mesh'], (21, 22, 23)) & (K['zeile'] != 9)
        hals += int((m & ROT(lab)).sum()); hpx += int(m.sum())
    mit = {k: round(float(np.mean([w[k] for w in W])), 2) for k in W[0] if isinstance(W[0][k], (int, float))}
    mit['hals_rot_px'] = hals; mit['hals_px'] = hpx
    aus[name] = mit
    cv2.imwrite(S + '/weiss_%s.png' % os.path.basename(d) if False else S + '/weiss_v_%s.png' % name.replace('/', '_')[-20:], cv2.cvtColor(np.hstack(kach), cv2.COLOR_RGB2BGR))
    print(name, {k: mit[k] for k in ('L_mittel', 'L_std', 'L_cv', 'lokal5', 'lokal5_rel', 'C_mittel', 'C_std', 'dunkel_pct', 'hell_pct', 'S5', 'F_fremd', 'fremd_pct', 'spanne90', 'block_ton_streu', 'h0', 'hals_rot_px', 'hals_px')})
json.dump(aus, open(S + '/weiss_var_' + '_'.join(os.path.basename(a) for a in sys.argv[2:]) + '.json', 'w'), indent=1)
