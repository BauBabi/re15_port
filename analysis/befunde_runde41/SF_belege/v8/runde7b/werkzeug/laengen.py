import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, v8_gemein as V, kopf_orig_bau as KB
blob = open(V.ASSET, 'rb').read(); ms = KB.teile(blob)[2]
for b in V.SCHWANZ:
    m = ms[b]; nr = 3 if b in (16, 18) else 2
    for k in range(nr - 1):
        L = [np.linalg.norm(np.array(m['v'][(k + 1) * 11 + j][:3], float) - np.array(m['v'][k * 11 + j][:3], float)) for j in range(11)]
        print(b, 'band', k, '->', k + 1, 'Kanten j0..10', [int(x) for x in L])
