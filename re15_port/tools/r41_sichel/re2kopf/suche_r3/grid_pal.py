import sys, json, os
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, kopf_re2_mess as KM, kopf_re2_bau as KB, riegel_re2kopf as RG
P = G.posen()
fall = json.loads(sys.argv[1])
for f in fall:
    p = json.loads(json.dumps(KB.PARAM_R3))
    for k, v in f.items():
        p['pal'][k].update(v)
    neu, prot, _ = KB.bauen(p)
    fn = 'grid_tmp.EMD'; open(fn, 'wb').write(neu)
    mo = b2.modell(fn)
    M = KM.messen(mo, P, bilder=True)
    Z = RG.kopf_r2(mo, M, P, prot['zonen'])
    img, ids = G.zeichne(mo, P['135']); Lab = G.lab(img.astype(float)); C = np.hypot(Lab[..., 1], Lab[..., 2])
    haut = np.median(Lab[..., 0][(ids == 8) & (C < 12)])
    print(json.dumps(f), 'k_grau %.3f' % prot['palette_r3']['grau']['k'], 'Haut(Id8,C<12) %.1f' % haut)
    for z in Z:
        if z['mass'].split()[1] in ('K5', 'T-K9e', 'K1k', 'K6'):
            print('   ', 'OK ' if z['ok'] else 'ROT', z['mass'][:30], z['ist'][:260])
