import os, sys
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, kopf_orig_bau as KB, luecke_r5 as L
blob = open(R7 + 'm/' + sys.argv[1] + '.EMD', 'rb').read(); mo = V.modell(blob); m = KB.teile(blob)[2][8]
an = dict(L.ansichten('alle')); p = an[sys.argv[2]]
ecken = [int(x) for x in sys.argv[3].split(',')]
P = V.proj(p, V.b8_zu_modell(mo, np.array([m['v'][i][:3] for i in ecken], float), p))
for i, q in zip(ecken, np.asarray(P, float)):
    print(i, m['v'][i][:3], 'Bild 640x480', np.round(q[:2], 1), '-> 960x720', np.round(np.asarray(q[:2]) * 1.5, 1))
A = L.zeichne(mo, p, None)
ys, xs = np.nonzero((A['bone'][:480] == 9)); print('Kopf 640: x', xs.min(), xs.max(), 'y', ys.min(), ys.max())
