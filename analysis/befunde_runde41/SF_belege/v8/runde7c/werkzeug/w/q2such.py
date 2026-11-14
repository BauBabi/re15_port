"""q2 Ecke 46 UV: S7 (a) adern_r4.herkunft_r4 texel_ohne_original_flaeche und Streckung q2.0/q2.1 (Pruefer flaechen.kennz)."""
import os, sys, json
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, R7 + 'pz')
import numpy as np, pl, flaechen as F
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V, kopf_orig_bau as KB, adern_r4 as A4
basis = open(R7 + 'm/' + sys.argv[1] + '.EMD', 'rb').read(); v7 = open(R7 + 'm/V7.EMD', 'rb').read()
for spec in sys.argv[2].split(';'):
    d, _o, ms, tim = KB.teile(basis)
    if spec != '-':
        u, v = [int(x) for x in spec.split(',')]
        ms[8]['quv'][2][3], ms[8]['quv'][2][4] = u, v
    blob = KB.packen(basis, d, ms, tim)
    h = A4.herkunft_r4(blob, v7)
    E = pl.EMD(blob); st = [round(F.kennz(P, T)[3], 2) for key, P, T, z, vi, ni, N in F.haelften(E.meshes[8]) if key[:2] == ('q', 2)]
    print(spec, 'S7a ok', h['ok'], 'texel_ohne_original_flaeche', h['texel_ohne_original_flaeche'], 'q2 Streckung', st, flush=True)
