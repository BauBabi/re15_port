# Offline-Referenz: relative oertliche L*-Streuung (5x5) auf dem Original-Fleisch des RE1.5-Birkin (Zeile 482, Texel v 92-125) gegen die
# Schwanz-Unterseite der Varianten, gleiche Posen/Licht (f_rast mit_bg), dazu Rumpf (Knochen 8) und Beinhaut.
import sys, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, r3_gemein as R3, f_rast as FR, glaette_r7b as GL
posen = []
for g in (0, 512, 1024, 1536, 2048, 2560, 3072, 3584):
    for nm, p in (('PV', V.PV), ('P2', R3.P2)):
        q = dict(p); q['gier'] = (p['gier'] + g) % 4096; posen.append(('%s+%d' % (nm, g), q))
def mass(blob, auswahl):
    mo = V.modell(blob)
    res = {}
    for nm, pose in posen:
        A = FR.mit_bg(mo, pose)
        L = GL.lstar(A['img'][:480])
        kl = np.full((480, 640), -1)
        for k, sel in auswahl.items():
            kl[sel(A)] = k
        r = GL.mass_bild(L, kl)
        for k, v in r.items():
            res.setdefault(k, []).append(v['rel'])
    return {k: (len(v), round(float(np.median(v)), 4)) for k, v in res.items()}
bone = lambda A: A['bone'][:480]; zl = lambda A: A['zeile'][:480]; tv = lambda A: A['v'][:480]
orig = open('emd/ORIG.EMD', 'rb').read()
print('ORIG', mass(orig, {0: lambda A: (zl(A) == 482) & (tv(A) >= 92) & (tv(A) <= 125), 2: lambda A: bone(A) == 8, 3: lambda A: (bone(A) >= 1) & (bone(A) <= 6) & (zl(A) == 480)}))
for n in sys.argv[1:]:
    b = open('emd/%s.EMD' % n, 'rb').read()
    print(n, mass(b, {0: lambda A: (bone(A) >= 16) & (bone(A) <= 22) & (zl(A) == 495), 2: lambda A: bone(A) == 8, 3: lambda A: (bone(A) >= 1) & (bone(A) <= 6) & (zl(A) == 480)}))
