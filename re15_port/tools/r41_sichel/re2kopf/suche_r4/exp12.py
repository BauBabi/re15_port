import sys, json, itertools
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, kopf_re2_mess as KM, kopf_re2_bau as KB, riegel_re2kopf as RG
P = G.posen()
import os
for weich, rel, ady, adx, d in itertools.product((1.0, 0.9), (1, 2), (60, 70), (20,), (60,)):
    sx = 0.85
    p = json.loads(json.dumps(KB.PARAM_R4))
    p["form"].update(relief=rel, weich=weich, sx=sx, auge_d=[-adx, ady, 0]); p['versatz'] = [140 - d, 10 + d, 0]
    neu, prot, _ = KB.bauen(p)
    fn = 'tmp_r4.EMD'; open(fn, 'wb').write(neu)
    mo = b2.modell(neu); M = KM.messen(mo, P, bilder=True); RG.hell_unten(mo, M)
    Z = RG.tr2_tr3(mo, M) + RG.kopf_r2(mo, M, P, prot['zonen'])
    z2, i2 = RG.tv2(fn, mo, P, json.loads(json.dumps(prot, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))), None)
    z3, i3 = RG.tv3(fn, mo, P, M, json.loads(json.dumps(prot, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))), i2['silhouette'])
    rot = [z['mass'][:14] for z in Z + z2 + z3 if not z['ok'] and not z.get('info')]
    b = M['85']; a = M['135']
    print("weich %.2f rel %d ady %d adx %d" % (weich, rel, ady, adx), prot["form"]["relief_ausgleich"], 'S2 %s K4 %s K10 %s ueber %d prof %.3f v85 %.3f' % (np.round(b['S2'], 3).tolist(), np.round(a['K4'], 3).tolist(), np.round(a['K10'], 3).tolist(), i3['ueber'], i3['profil'], i3['pose85']),
          'V2a/b:', [z['ist'][:60] for z in z2[:1]], [z['ist'][-60:] for z in z2[1:2]], 'ROT', rot, flush=True)
