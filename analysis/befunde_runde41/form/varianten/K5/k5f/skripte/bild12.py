import sys, os, json
G = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, G); sys.path.insert(0, os.path.join(os.path.dirname(G), 'ruhe', 'skripte'))
import numpy as np, rl, kf_k5e_global as KGL, kf_k5e as KE, riegel_k5d as RK
vor = RK.Vorlage(); aus = {}
fig = {}
for n in (1, 2):
    _s, Kc = rl.v4f.szene_id(KE.BASIS, n)
    fig[n] = RK.nutzer_union(n) & Kc['maske'] & ~vor.d[n]['clip']
    aus['Vorlage Bild %d Video' % n] = round(float(vor.d[n]['LV'][fig[n]][:, 0].mean()), 2)
for d in sys.argv[1:]:
    d = d if '/' in d else r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f/vorschau/' + d
    r = {}
    for n in (1, 2):
        s, _K = rl.v4f.szene_id(d, n)
        r['bild%d_video' % n] = round(float(np.mean([RK.labm(rl.v4f.video(s, n, sa))[fig[n]][:, 0].mean() for sa in rl.v4f.SAATEN])), 2)
        r['bild%d_ohne' % n] = round(float(RK.labm(s)[fig[n]][:, 0].mean()), 2)
    r['fleisch_abstaende'] = [round(x, 2) for x in KGL.abstaende(d)]
    aus[d[-40:]] = r; print(d[-25:], r, flush=True)
print(json.dumps(aus))
