"""Versuch MITTE (Messung): s .1-.9 in PV (m1_pv) je Profil."""
import os, sys
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); sys.dont_write_bytecode = True
import v8_gemein as V
import schwanz_r8 as Q
import m1_pv as M
basis = open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8/emd/ALT_07b6703f.EMD', 'rb').read()
alt = M.messen(basis)['breite']
print('alt', alt)
for mi in ({3: 0.86, 4: 0.78}, {3: 0.85, 4: 0.79}, {3: 0.87, 4: 0.78}, {3: 0.85, 4: 0.80}):
    Q.MITTE = mi
    r = M.messen(Q.stufe(basis, 'b'))['breite']
    print(mi, r, 's.5 %.1f %%' % (100 * (r[2] / alt[2] - 1)))
