"""Offline-Scan Ecke 17 (Mesh 8): S9 (riegel_v8.s9, P1/F344/F354/F364 gegen V7) und Gesichts-Bildpunkte von V7, die im Kandidaten nicht
Gesicht sind (Pruefer-Code pz/vorn.render, Maler, Variante 0) in ausgewaehlten Ansichten.  python scan17.py BASIS 'x,y,z;x,y,z;...' [ANSICHTEN]"""
import os, sys, json
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, R7 + 'pz')
import numpy as np
import pl, vorn as Vn, luecke_eigen as L
cwd = os.getcwd()
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V, kopf_orig_bau as KB, riegel_v8 as RV
basis = open(R7 + 'm/' + sys.argv[1] + '.EMD', 'rb').read()
v7b = open(R7 + 'm/V7.EMD', 'rb').read()
V7 = pl.EMD(v7b); n7t, n7q = len(V7.meshes[9]['tf']), len(V7.meshes[9]['qf'])
want = (sys.argv[3] if len(sys.argv) > 3 else 'P2+2048,F313,F312,F311').split(',')
an = dict((n, p) for n, p in L.ansichten() if n in want)
for spec in sys.argv[2].split(';'):
    d, _o, ms, tim = KB.teile(basis)
    if spec != '-':
        ms[8]['v'][17][:3] = [int(x) for x in spec.split(',')]
    blob = KB.packen(basis, d, ms, tim)
    s9 = RV.s9(blob, v7b)
    K = pl.EMD(blob); gK, g7 = Vn.gesicht_flaechen(K, n7t, n7q), Vn.gesicht_flaechen(V7, n7t, n7q)
    ges = {}
    for nm in want:
        rk, rb = Vn.render(K, an[nm]), Vn.render(V7, an[nm])
        m = lambda r, s: np.array([k in s for k in r['key'][:-1]] + [False])[r['face']]
        v = m(rb, g7) & ~m(rk, gK)
        wer = {}
        for f in rk['face'][v]:
            k = rk['key'][f] if f >= 0 else None
            if k is not None and k[0] == 9: continue
            wer[str(k)] = wer.get(str(k), 0) + 1
        ges[nm] = wer
    print(spec, 'S9', [(a['ansicht'], a['ueber_v7_px'], a['neue_ecken']) for a in s9['ansichten']], 'Gesicht', ges, flush=True)
