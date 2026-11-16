"""S9-Diagnose: in welcher Spalte liegt die Oberkontur (Knochen 0-15) ueber V7, und welche Flaeche bildet sie dort.  python s9diag.py KAND [F]"""
import os, sys
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, r3_gemein as R3, luecke_r5 as L
f = int(sys.argv[2]) if len(sys.argv) > 2 else 344
mo, m7 = V.modell(open(R7 + 'm/' + sys.argv[1] + '.EMD', 'rb').read()), V.modell(open(R7 + 'm/V7.EMD', 'rb').read())
pose = R3.szene_posen()[f]
o7 = R3.oben(V.b2.umriss_schnell(m7, pose, knochen=list(range(16))), 395, 515)
o = R3.oben(V.b2.umriss_schnell(mo, pose, knochen=list(range(16))), 395, 515)
A = L.zeichne(mo, pose, None)
for x in sorted(o):
    if x in o7 and o7[x] - o[x] >= 2:
        y = o[x]; print('x', x, 'oben', y, 'V7', o7[x], 'diff', o7[x] - y, 'Knochen', A['bone'][y, x], 'Flaeche', A['flaeche'][y, x], 'darunter', [(A['bone'][y + k, x], A['flaeche'][y + k, x]) for k in (1, 2, 3)])
