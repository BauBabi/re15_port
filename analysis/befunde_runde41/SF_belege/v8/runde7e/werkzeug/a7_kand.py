# Kandidaten fuer die Handfarbe (Scratch): K1 Hand (24) -> 482 nur CLUT; K1b dazu Kastenfinger (10) -> 482;
# K2 Hand+Kasten (34) UV x0.92 + (6,6) -> 482, Mesh 11 unten (8) UV x0.82 + (5,3); K2h nur der Mesh-12-Teil von K2
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, v8_gemein as V, kopf_orig_bau as KB, riegel_handfarbe as RF
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7e'
blob = open(V.ASSET, 'rb').read()
def faces(m):
    for F, U in (('tf', 'tuv'), ('qf', 'quv')):
        for i, f in enumerate(m[F]):
            yield f, m[U][i]
def tr(uv, s, u0, v0, tu, tv):
    n = 3 if len(uv) == 9 else 4
    for j in range(n):
        uv[3*j] = int(round(s * (uv[3*j] - u0) + tu)); uv[3*j+1] = int(round(s * (uv[3*j+1] - v0) + tv))
def kand(name):
    d, _o, ms, tim = KB.teile(blob)
    for f, uv in faces(ms[12]):
        k = RF.krallen_klasse(f)
        if k == 'hand' or (k == 'kastenfinger' and name in ('K1b', 'K2', 'K2h')):
            if name in ('K2', 'K2h'):
                tr(uv, 0.92, 4, 51, 6, 6)
            uv[2] = (482 << 6) | (uv[2] & 0x3F)
    if name == 'K2':
        for f, uv in faces(ms[11]):
            n = 3 if len(uv) == 9 else 4
            if max(uv[3*j+1] for j in range(n)) > 56:
                tr(uv, 0.82, 0, 25, 5, 3)
    neu = KB.packen(blob, d, ms, tim)
    open(SP + '/emd/%s.EMD' % name, 'wb').write(neu)
    print(name, V.sha(neu))
import os; os.makedirs(SP + '/emd', exist_ok=True)
for n in ('K1', 'K1b', 'K2', 'K2h'):
    kand(n)
