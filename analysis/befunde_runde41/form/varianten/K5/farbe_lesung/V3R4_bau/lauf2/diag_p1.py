"""P1 je Zeile, RV6, H2 an einer Variante (Diagnose Bauer R4)."""
import sys, os
K5 = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5'
sys.path.insert(0, K5 + '/generator')
os.chdir(K5 + '/generator')
import numpy as np
import riegel_farbe_v3 as R
KG = R.KG
vdir = sys.argv[1]
v = R.Var(vdir)
g4, g4tim, _ = KG.g4_modell()
px_g4, cl_g4 = KG.g4_cluts(g4tim)
rgb, u = R.zeilen_atlas(v)
n = len(v.cl)
print('P1 gesamt', R.p1_mass(rgb, u))
for z in range(n):
    sl = slice(256 * z, 256 * (z + 1))
    uu = np.zeros_like(u); uu[sl] = u[sl]
    m = R.p1_mass(rgb, uu)
    if m['fleischtexel']:
        print(' Zeile %2d' % z, m)
print('RV6', R.rv6(v, cl_g4))
h = R.h2(v, cl_g4)
for z, d in h[0]['zeilen'].items():
    print(' H2', z, d)
