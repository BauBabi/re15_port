"""Gitter fuer Ecke 17: S9 F344 (riegel_v8.s9), Gesicht P2+2048 (Pruefer-Render, Variante 0, Mesh 8), Bildverschiebung von 17 gegen R3 in F276/F308/F310/F311 (960 px).
python grid17.py BASIS 'x,y,z;...'"""
import os, sys
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, R7 + 'pz')
import numpy as np
import pl, vorn as Vn, luecke_eigen as LE
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V, kopf_orig_bau as KB, riegel_v8 as RV, luecke_r5 as L
basis = open(R7 + 'm/' + sys.argv[1] + '.EMD', 'rb').read(); v7b = open(R7 + 'm/V7.EMD', 'rb').read()
V7 = pl.EMD(v7b); n7t, n7q = len(V7.meshes[9]['tf']), len(V7.meshes[9]['qf']); g7 = Vn.gesicht_flaechen(V7, n7t, n7q)
an = dict(L.ansichten('alle')); anP = dict(LE.ansichten())
mo0 = V.modell(basis); m0 = KB.teile(basis)[2][8]
def bild(mo, m, nm):
    p = an[nm]; return np.asarray(V.proj(p, V.b8_zu_modell(mo, np.array([m['v'][17][:3]], float), p)), float)[0, :2] * 1.5
ref = {nm: bild(mo0, m0, nm) for nm in ('F276', 'F308', 'F310', 'F311')}
rb = Vn.render(V7, anP['P2+2048'])
for spec in sys.argv[2].split(';'):
    d, _o, ms, tim = KB.teile(basis); ms[8]['v'][17][:3] = [int(x) for x in spec.split(',')]
    blob = KB.packen(basis, d, ms, tim); mo = V.modell(blob)
    s9 = [a['ueber_v7_px'] for a in RV.s9(blob, v7b)['ansichten'] if a['ansicht'] == 'F344'][0]
    K = pl.EMD(blob); gK = Vn.gesicht_flaechen(K, n7t, n7q); rk = Vn.render(K, anP['P2+2048'])
    mm = lambda r, s: np.array([k in s for k in r['key'][:-1]] + [False])[r['face']]
    v = mm(rb, g7) & ~mm(rk, gK)
    n8 = sum(1 for f in rk['face'][v] if f >= 0 and rk['key'][f][0] == 8)
    dv = {nm: tuple(np.round(bild(mo, ms[8], nm) - ref[nm], 2)) for nm in ref}
    print(spec, 'F344', s9, 'P2+2048 Mesh8', n8, 'Verschiebung (dx,dy) 960px', dv, flush=True)
