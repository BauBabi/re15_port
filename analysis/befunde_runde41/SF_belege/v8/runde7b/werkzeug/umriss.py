import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, v8_gemein as V, r3_gemein as R3, f_rast as FR
ref = V.modell(open(sys.argv[1], 'rb').read())
posen = [('PV', V.PV), ('P2', R3.P2), ('P1', R3.P1)] + [('F%d' % k, R3.szene_posen()[k]) for k in (264, 276, 300, 318, 350)]
masks = {n: FR.zeichne(ref, p, None, licht=False)['bone'][:480] >= 0 for n, p in posen}
for q in sys.argv[2:]:
    mo = V.modell(open(q, 'rb').read())
    print(q, [(n, int((masks[n] ^ (FR.zeichne(mo, p, None, licht=False)['bone'][:480] >= 0)).sum())) for n, p in posen])
