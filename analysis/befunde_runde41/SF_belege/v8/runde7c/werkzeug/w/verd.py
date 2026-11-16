"""riegel_r6.s16_verdeckt je Ansicht mit verdeckender Flaeche (Mesh 8).  python verd.py NAME[,NAME] [ANSICHTEN]"""
import os, sys
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, luecke_r5 as L, ruecken_r5 as RR5
mo7 = V.modell(open(R7 + 'm/V7.EMD', 'rb').read())
want = (sys.argv[2] if len(sys.argv) > 2 else 'F253,PV+1024,P2+2048').split(',')
an = dict(L.ansichten('alle'))
for n in sys.argv[1].split(','):
    mo = V.modell(open(R7 + 'm/' + n + '.EMD', 'rb').read())
    res = {}
    for nm in want:
        A, A7 = L.zeichne(mo, an[nm], L.VARIANTEN[0]), L.zeichne(mo7, an[nm], L.VARIANTEN[0])
        ges = (A7['bone'][:480] == 9) & (A7['lok'][:480][..., 0] >= RR5.GESICHT_X)
        b = A['bone'][:480]; ver = ges & (b == 8)
        res[nm] = (int(ver.sum()), sorted(set(A['flaeche'][:480][ver].tolist())))
    print(n, res, flush=True)
