import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, kopf_re2_mess as KM, kopf_re2_bau as KB
P = G.posen()
def zonen(prot):
    return prot['zonen']
def mess(emd, prot):
    mo = b2.modell(emd); z = prot['zonen']; out = {}
    for h in ('135', '85'):
        img, ids = G.zeichne(mo, P[h]); Lab = G.lab(img.astype(float)); k = ids == 9
        r = dict(kopf=np.median(Lab[k], 0).round(1).tolist())
        for nm, tt, qq in (('grau', z['grau_T'], z['grau_Q']), ('dunkel', z['dunkel_T'], z['dunkel_Q'])):
            _i, idz = G.zeichne(KM.teil(mo, tt, qq), P[h], licht=False); mz = (idz == 9) & k
            r[nm] = (int(mz.sum()), np.median(Lab[mz], 0).round(1).tolist() if mz.any() else None)
        b8 = ids == 8; C = np.hypot(Lab[..., 1], Lab[..., 2])
        r['rumpf'] = np.median(Lab[b8], 0).round(1).tolist(); r['rumpf_haut_C12'] = np.median(Lab[b8 & (C < 12)], 0).round(1).tolist()
        sch = np.isin(ids, [3, 4, 5, 6, 7, 8, 10, 11, 12]) & (C < 12)
        r['oberkoerper_haut_C12'] = np.median(Lab[sch], 0).round(1).tolist()
        out[h] = r
    return out
emd = sys.argv[1]; prot = json.load(open(sys.argv[2]))
for h, r in mess(emd, prot).items(): print(h, r)
