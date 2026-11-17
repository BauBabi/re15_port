"""Gesichts-Bildpunkte von V7, die im Kandidaten von Mesh 8 verdeckt sind (riegel_r6.s16_verdeckt-Logik), je Halbpixel-Variante (luecke_r5.VARIANTEN)
fuer ausgewaehlte Ansichten; dazu die verdeckende Flaeche.  python gesvar.py KAND 'Ecke=x,y,z;...' [ANSICHTEN]   ('-' = unveraendert)"""
import os, sys
os.environ.setdefault('SFV8', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/tmp')
R7 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/'
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, kopf_orig_bau as KB, luecke_r5 as L, ruecken_r5 as RR5, riegel_v8 as RV
basis = open(R7 + 'm/' + sys.argv[1] + '.EMD', 'rb').read()
mo7 = V.modell(open(R7 + 'm/V7.EMD', 'rb').read())
want = (sys.argv[3] if len(sys.argv) > 3 else 'F312,F313,F314,P2+2048,P2+1792,P2+2304').split(',')
an = dict((n, p) for n, p in L.ansichten('alle') if n in want)
for spec in sys.argv[2].split(';'):
    d, _o, ms, tim = KB.teile(basis)
    if spec != '-':
        for part in spec.split('|'):
            e, xyz = part.split('=')
            ms[8]['v'][int(e)][:3] = [int(x) for x in xyz.split(',')]
    blob = KB.packen(basis, d, ms, tim)
    mo = V.modell(blob)
    res = {}
    for nm in want:
        r = []
        for var in L.VARIANTEN:
            A, A7 = L.zeichne(mo, an[nm], var), L.zeichne(mo7, an[nm], var)
            ges = (A7['bone'][:480] == 9) & (A7['lok'][:480][..., 0] >= RR5.GESICHT_X)
            b = A['bone'][:480]; ver = ges & (b == 8)
            r.append((int(ver.sum()), sorted(set(A['flaeche'][:480][ver].tolist()))))
        res[nm] = [x for x in r]
    s9 = RV.s9(blob, open(R7 + 'm/V7.EMD', 'rb').read())
    print(spec, 'S9 F344', [a['ueber_v7_px'] for a in s9['ansichten'] if a['ansicht'] == 'F344'],
          {k: ([n for n, _ in v], sorted(set(f for _, fl in v for f in fl))) for k, v in res.items()}, flush=True)
