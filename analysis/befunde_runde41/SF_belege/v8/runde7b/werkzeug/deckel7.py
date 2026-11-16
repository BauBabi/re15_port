import sys, json, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, v8_gemein as V, kopf_orig_bau as KB, unterseite_r5 as U5, szene_marker as SM
par = json.load(open(os.path.join(U5.HIER, 'unterseite_v8.json')))
for p in sys.argv[1:]:
    blob = open(p, 'rb').read(); ms = KB.teile(blob)[2]
    m = ms[16]; phi = float(par['innen_phi']['16'])
    w = SM.wurzel_flaechen(m)
    print('==', p, 'Wurzelflaechen', w)
    for key in w:
        i = int(key[1:]); f = (m['tf'] if key[0] == 'd' else m['qf'])[i]; uv = (m['tuv'] if key[0] == 'd' else m['quv'])[i]
        ie = list(f[1::2])
        th = [U5.theta_normale(m['n'][f[2*k]], phi) if U5.ring_ecke(16, ie[k]) else None for k in range(len(ie))]
        thp = [U5.theta_signiert(np.array(m['v'][j][:3], float), phi) for j in ie]
        print('  ', key, 'ecken', ie, 'zeile', (uv[2] >> 6) & 0x1FF, 'thetaN', [None if t is None else round(t) for t in th], 'thetaP', [None if t is None else round(t) for t in thp],
              'y', [m['v'][j][1] for j in ie], 'uv', [(uv[3*k], uv[3*k+1]) for k in range(len(ie))])
