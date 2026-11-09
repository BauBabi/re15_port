import sys, os, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, cv2
import rl
K5 = rl.K5
mo = rl.fl.lade_var(K5 + '/ruhe/K5e_f1'); mo.T = rl.FB.Textur(mo.tim) if not hasattr(mo,'T') else mo.T
S = sys.argv[1]
res = {}
for name, az, el in (('unten_vorn', 0, -60), ('unten_seite', 90, -45), ('unten', 0, -85), ('seite', 0, 0), ('unten_3_4', 45, -40)):
    img, K = rl.weiss_id(mo, 2, az, el, 500)
    lab = rl.fl.lab(img.reshape(-1,3)).reshape(img.shape)
    C = np.hypot(lab[...,1], lab[...,2]); h = np.degrees(np.arctan2(lab[...,2], lab[...,1]))
    rot = K['maske'] & (lab[...,1] > 12) & (C > 15) & (h > -40) & (h < 60)
    cv2.imwrite(S + '/hals_%s.png' % name, cv2.cvtColor(img, cv2.COLOR_RGB2BGR))
    ov = img.copy(); ov[rot] = (0,255,0)
    cv2.imwrite(S + '/hals_%s_rot.png' % name, cv2.cvtColor(ov, cv2.COLOR_RGB2BGR))
    d = {}
    for m_, z_, e_ in zip(K['mesh'][rot], K['zeile'][rot], K['eintrag'][rot]):
        d[(int(m_), int(z_))] = d.get((int(m_), int(z_)), 0) + 1
    tot = {}
    for m_ in (21,22,23):
        mm = K['maske'] & (K['mesh']==m_)
        tot[m_] = int(mm.sum())
    res[name] = dict(rot=int(rot.sum()), je_mesh_zeile=sorted([(k[0],k[1],v) for k,v in d.items()], key=lambda t:-t[2])[:15], hals_kopf_kiefer_px=tot)
    print(name, res[name])
