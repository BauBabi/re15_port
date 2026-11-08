import sys, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
import birkin_formen as BF
from P_farbe import lab
mo = BF.modell_aus_blob(open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD','rb').read())
A = lab(mo.pal[2][mo.tex[:, 256:384]].astype(float))   # Seite 2, Zeile 482
# Spaltenprofil: mittleres L/a je v in Streifen A (u 44-83) und B (u 87-126)
for nm, (u0, u1) in (('A', (44, 83)), ('B', (87, 126)), ('C', (0, 40))):
    print(nm, 'v: L a b (Zeilenmittel)')
    for v in range(0, 96, 4):
        x = A[v:v+4, u0:u1+1].reshape(-1, 3)
        print('  v%3d L %.1f a %.1f b %.1f  std_a %.1f' % (v, *x.mean(0), x[:, 1].std()))
