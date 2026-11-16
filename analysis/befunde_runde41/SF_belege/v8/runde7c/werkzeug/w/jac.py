import os, sys
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, kopf_orig_bau as KB, luecke_r5 as L
blob = open(R7 + 'm/' + sys.argv[1] + '.EMD', 'rb').read(); mo = V.modell(blob); m = KB.teile(blob)[2][8]
an = dict(L.ansichten('alle'))
e = int(sys.argv[2])
for nm in sys.argv[3].split(','):
    p = an[nm]; P0 = np.array([m['v'][e][:3]], float)
    q0 = np.asarray(V.proj(p, V.b8_zu_modell(mo, P0, p)), float)[0, :2] * 1.5
    row = []
    for ax in range(3):
        P = P0.copy(); P[0, ax] += 10
        q = np.asarray(V.proj(p, V.b8_zu_modell(mo, P, p)), float)[0, :2] * 1.5
        row.append(tuple(np.round(q - q0, 2)))
    print(nm, 'Ecke', e, 'Bild 960', np.round(q0, 1), 'je +10 E. in x/y/z (px 960x720):', row)
