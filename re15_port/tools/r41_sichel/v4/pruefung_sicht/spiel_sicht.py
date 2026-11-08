import sys, os, numpy as np, cv2
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, W + '/nb2neu'); sys.path.insert(0, W + '/v4')
import c6
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
runs = {'V4': S + '/sfv4/r/V4', 'V3r2': S + '/sfv3/r/V3r2'}
for k, r in runs.items():
    print(k, 'off', c6.off(r))
    for h in (85, 100, 118, 135):
        p = c6.port(r, h)
        if p is None: print(k, h, 'None'); continue
        cv2.imwrite('spiel_%s_h%d.png' % (k, h), p)
print('ok')
