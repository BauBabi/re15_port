import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np
import rl
K5 = rl.K5
mat = json.load(open(K5 + '/ruhe/messung/material.json'))['material']
mi = set(mat['Maulinneres']['liste'])
mo = rl.fl.lade_var(K5 + '/ruhe/K5e_f1')
cnt = {k: [0, 0] for k in mi}   # clip2 px, clip0 px
for ci, clip in enumerate((2, 0)):
    for az in range(0, 360, 30):
        for el in (-80, -45, -15, 15, 45, 80):
            img, K = rl.weiss_id(mo, clip, az, el, 400)
            m = K['maske'] & ((K['mesh'] == 22) | (K['mesh'] == 23))
            for msh, f in zip(K['mesh'][m], K['flaeche'][m]):
                k = '%d|%d|%d' % (msh, f // 100000, f % 100000)
                if k in cnt: cnt[k][ci] += 1
# Seite je Flaeche
info = {}
for b, m in enumerate(mo.meshes):
    for q, uvs in ((0, m.tuv), (1, m.quv)):
        for fi, uv in enumerate(uvs):
            k = '%d|%d|%d' % (b, q, fi)
            if k in mi: info[k] = dict(seite=uv[5] & 0xF, zeile=(uv[2] >> 6) & 0x1FF)
out = sorted([(k, cnt[k][0], cnt[k][1], info.get(k)) for k in mi], key=lambda t: -t[1])
for o in out: print(o)
json.dump(out, open(sys.argv[1] + '/maulinneres_sicht.json', 'w'), indent=1)
