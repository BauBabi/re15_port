"""Fremde Texel in Mesh 8: (Zeile, Texel) unter Mesh-8-Flaechen des Kandidaten (KB.raster wie bank_p5.k_praefix), die KEIN Stand der Liste
auf derselben Zeile benutzt (alle Meshes).  python fremd.py KAND [STAENDE=N7B,R4,V7,R2,R6]"""
import sys
import numpy as np
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools')
import kopf_orig_bau as KB
M = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/m/'
def flaechen(m):
    return [(u, 3) for u in m['tuv']] + [(u, 4) for u in m['quv']]
def belegt(names):
    t = {}
    for n in names:
        for m in KB.teile(open(M + n + '.EMD', 'rb').read())[2]:
            for uv, k in flaechen(m):
                z = (uv[2] >> 6) & 0x1FF
                t.setdefault(z, np.zeros((256, 384), bool))
                t[z] |= KB.raster(uv, k, (uv[5] & 0xF) * 128)
    return t
kn = sys.argv[1]; st = (sys.argv[2] if len(sys.argv) > 2 else 'N7B,R4,V7,R2,R6').split(',')
B = belegt(st)
m8 = KB.teile(open(M + kn + '.EMD', 'rb').read())[2][8]
tot = 0; wo = {}
for i, (uv, k) in enumerate(flaechen(m8)):
    z = (uv[2] >> 6) & 0x1FF
    mk = KB.raster(uv, k, (uv[5] & 0xF) * 128) & ~B.get(z, np.zeros((256, 384), bool))
    if mk.any():
        name = ('d%d' % i) if i < len(m8['tuv']) else ('v%d' % (i - len(m8['tuv'])))
        wo[name] = (z, int(mk.sum()))
        tot += int(mk.sum())
print(kn, 'gegen', st, ': fremde Texel in Mesh 8:', tot, wo)
if '--liste' in sys.argv:
    for i, (uv, k) in enumerate(flaechen(m8)):
        z = (uv[2] >> 6) & 0x1FF
        mk = KB.raster(uv, k, (uv[5] & 0xF) * 128) & ~B.get(z, np.zeros((256, 384), bool))
        for y, x in zip(*np.nonzero(mk)): print('  Flaeche', i, 'Zeile', z, 'Texel (y, x)', int(y), int(x))
