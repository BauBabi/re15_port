"""S14 (b): fast weisse Bildpunkte Knochen 8 von vorn (riegel_v8.weiss_px) je Flaeche, P1 + F336-F372 (jedes 2.).  python weiss.py NAME,..."""
import os, sys
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, riegel_v8 as RV, f_rast as FR, r3_gemein as R3
from collections import Counter
for n in sys.argv[1].split(','):
    mo = V.modell(open(R7 + 'm/' + n + '.EMD', 'rb').read())
    tot = 0; c = Counter()
    for p in ['P1'] + list(RV.S14_WEISS_TAKTE):
        A = FR.mit_bg(mo, RV.s14_pose(p))
        b = A['bone'][:480]; L = RV.G.lab(A['img'][:480].astype(float))
        w = (b == 8) & (L[..., 0] > 75) & (np.hypot(L[..., 1], L[..., 2]) < 12)
        tot += int(w.sum())
        if 'flaeche' in A:
            c.update(A['flaeche'][:480][w].tolist())
    print(n, 'weiss', tot, c.most_common(10), flush=True)
