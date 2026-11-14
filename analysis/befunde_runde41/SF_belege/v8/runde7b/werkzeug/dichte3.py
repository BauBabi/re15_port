import sys, json, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'); sys.path.insert(0, '.')
import numpy as np, v8_gemein as V, kopf_orig_bau as KB, unterseite_r5 as U5
from dichte import tris, abb
par = json.load(open(os.path.join(U5.HIER, 'unterseite_v8.json'))); par['fleisch_r7b'] = 1
ref = 0.0714
for p in sys.argv[1:]:
    ms = KB.teile(open(p, 'rb').read())[2]
    kl = U5.flaechen_klassen(ms, par)
    stat = {}
    for b in V.SCHWANZ:
        k2 = {k: (c, np.mean([t for t in th if t is not None]) if any(t is not None for t in th) else None) for k, c, th in kl[b]}
        for key, pp, t, uv in tris(ms[b]):
            if ((uv[2] >> 6) & 0x1FF) != 495: continue
            r = abb(pp, t)
            if r is None: continue
            c, thm = k2[key]
            grp = c + ('/fleisch' if thm is not None and thm < 110 else '/uebergang')
            s = stat.setdefault(grp, [0, 0, 0, 0.0, 0.0])
            s[0] += 1; s[1] += int(r[3] > 2); s[2] += int(not (0.5 * ref <= r[2] <= 2 * ref)); s[3] += r[0]; s[4] += r[0] * r[2]
    print('==', p)
    for g, s in sorted(stat.items()):
        print('  %-22s Dreiecke %3d  Streckung>2 %3d  Dichte ausserhalb %3d  Flaeche %8.0f  Dichte gew %.4f' % (g, s[0], s[1], s[2], s[3], s[4] / s[3]))
    if os.environ.get('LISTE'):
        for b in V.SCHWANZ:
            k2 = {k: (c, th) for k, c, th in kl[b]}
            for key, pp, t, uv in tris(ms[b]):
                if ((uv[2] >> 6) & 0x1FF) != 495: continue
                r = abb(pp, t)
                if r is None: continue
                c, th = k2[key]
                tr = [x for x in th if x is not None]
                if tr and np.mean(tr) < 110 and (r[3] > 2 or not (0.5 * ref <= r[2] <= 2 * ref)):
                    print('   ', b, key, c, 'th', [None if x is None else round(x) for x in th], 'A %.0f' % r[0], 'dichte %.3f streck %.1f' % (r[2], r[3]), 'uv', t.astype(int).tolist())
